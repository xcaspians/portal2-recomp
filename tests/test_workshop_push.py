"""tools/workshop_push.py: which of the Mac's Workshop maps go to the device, what is skipped, the notes it
writes and the free-space check, with a fake Mac folder, fake devicectl listings and invented API answers."""
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import workshop_push as wp  # noqa: E402


def details(i, handle, filename, size, title='A map', updated=1700000000, result=1, app=620):
    return {'publishedfileid': str(i), 'result': result, 'title': title, 'filename': filename,
            'file_size': str(size), 'hcontent_file': handle, 'time_updated': updated, 'consumer_app_id': app}


def listing(files):
    """devicectl JSON of the device's workshop folder: {relative path: size}; folders too, as devicectl lists them."""
    items = [{'relativePath': p, 'metadata': {'type': 'file', 'size': s}} for p, s in files.items()]
    items += [{'relativePath': p.split('/')[0], 'metadata': {'type': 'directory'}} for p in files]
    return {'result': {'files': items}}


class WorkshopPushTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name) / 'workshop'
        self.root.mkdir()

    def tearDown(self):
        self.tmp.cleanup()

    def mac_map(self, handle, name, size, item_id):
        folder = self.root / handle
        folder.mkdir()
        (folder / name).write_bytes(b'V' * size)
        if item_id:
            (folder / f'thumb{item_id}.jpg').write_bytes(b'jpg')
        return folder

    def test_scan_reads_the_id_from_the_thumbnail_and_skips_folders_without_a_map(self):
        self.mac_map('1018321821847840592', 'chapter2test.bsp', 10, 2092954243)
        (self.root / '1135164641855284366').mkdir()
        (self.root / '1135164641855284366' / 'thumb99649190.jpg').write_bytes(b'j')   # not downloaded
        found = wp.scan(self.root)
        self.assertEqual([(m['handle'], m['bsp'].name, m['size'], m['id']) for m in found],
                         [('1018321821847840592', 'chapter2test.bsp', 10, '2092954243')])

    def test_plan_copies_new_maps_with_the_note_community_maps_reads(self):
        self.mac_map('111', 'chapter2test.bsp', 10, 2092954243)
        d = {'2092954243': details(2092954243, '111', 'mymaps/chapter2test.bsp', 10,
                                   title='Portal Plus :\nChapter 2', updated=1589389790)}
        copies, skipped = wp.plan(wp.scan(self.root), d, {})
        self.assertEqual(skipped, [])
        self.assertEqual(len(copies), 1)
        c = copies[0]
        self.assertEqual((c['id'], c['name'], c['size']), ('2092954243', 'chapter2test.bsp', 10))
        self.assertEqual(c['note'], 'updated=1589389790\nsize=10\ntitle=Portal Plus : Chapter 2\n')

    def test_same_size_on_the_device_is_skipped_and_another_version_is_kept(self):
        self.mac_map('111', 'a.bsp', 10, 1)
        self.mac_map('222', 'b.bsp', 20, 2)
        self.mac_map('333', 'c.bsp', 30, 3)
        d = {'1': details(1, '111', 'a.bsp', 10), '2': details(2, '222', 'b.bsp', 20), '3': details(3, '333', 'c.bsp', 30)}
        device = wp.parse_listing(listing({'1/a.bsp': 10, '1/p2workshop.txt': 40,
                                           '2/b.bsp': 99, '2/p2workshop.txt': 40,
                                           '3/c.bsp': 30}))
        copies, skipped = wp.plan(wp.scan(self.root), d, device)
        self.assertEqual([why for _, why in skipped],
                         ['already on the device', 'another version is on the device (kept; update it in Community Maps)'])
        # Map 3 is there with the same size but without its note: only the note goes.
        self.assertEqual([(c['id'], c['bsp']) for c in copies], [('3', None)])

    def test_odd_names_ids_and_other_games_are_refused(self):
        self.mac_map('111', 'x.bsp', 5, None)                  # no thumbnail: no ID
        self.mac_map('222', 'y.bsp', 5, 7)                     # removed from the Workshop
        self.mac_map('333', 'z.bsp', 5, 8)                     # another game's item
        self.mac_map('444', 'w.bsp', 5, 9)
        d = {'7': details(7, '222', 'y.bsp', 5, result=9), '8': details(8, '333', 'z.bsp', 5, app=400),
             '9': details(9, '444', '../../cfg/autoexec;quit.bsp', 5)}
        copies, skipped = wp.plan(wp.scan(self.root), d, {})
        self.assertEqual(len(skipped), 3)
        self.assertEqual([c['name'] for c in copies], ['autoexec_quit.bsp'])   # rt_ws_map_path's rule

    def test_map_name_follows_the_runtime(self):
        self.assertEqual(wp.map_name('mymaps/sp a2..test.bsp', 5), 'sp_a2_.test.bsp')
        self.assertEqual(wp.map_name('...bsp', 5), '__.bsp')
        self.assertEqual(wp.map_name('.bsp', 77), '77.bsp')
        self.assertEqual(wp.map_name('dir/Été.bsp', 5), '_t_.bsp')

    def test_stage_writes_one_folder_per_map_and_never_touches_the_mac_copy(self):
        folder = self.mac_map('111', 'a.bsp', 10, 1)
        copies, _ = wp.plan(wp.scan(self.root), {'1': details(1, '111', 'a.bsp', 10)}, {})
        stage = Path(self.tmp.name) / 'stage'
        pairs = wp.stage(copies, stage)
        self.assertEqual(pairs, [(str(stage / '1'), 'Documents/portal2-userdata/portal2/maps/workshop/1')])
        self.assertEqual(sorted(p.name for p in (stage / '1').iterdir()), ['a.bsp', 'p2workshop.txt'])
        self.assertEqual((stage / '1' / 'a.bsp').read_bytes(), b'V' * 10)
        self.assertTrue((folder / 'a.bsp').exists())

    def test_sizes_read_plainly(self):
        self.assertEqual((wp.megabytes(0), wp.megabytes(10), wp.megabytes(832082923)), ('0 MB', '1 KB', '794 MB'))

    def test_listing_errors_are_not_empty_folders(self):
        self.assertEqual(wp.parse_listing(None), {})
        with self.assertRaises(wp.PushError):
            wp.parse_listing({'error': {'code': 1}})
        with self.assertRaises(wp.PushError):
            wp.parse_listing({'result': {}})

    def test_free_space(self):
        gb = 1 << 30
        self.assertIsNone(wp.check_space({'free_bytes': str(3 * gb)}, gb))
        self.assertIn('not enough room', wp.check_space({'free_bytes': str(gb)}, gb))
        self.assertIn("couldn't read", wp.check_space({}, gb))
        self.assertEqual(wp.parse_report('model=iPhone14,5\nram_mb=3700\nfree_bytes=12\n'),
                         {'model': 'iPhone14,5', 'ram_mb': '3700', 'free_bytes': '12'})

    def test_dry_run_prints_the_total_and_stages_nothing(self):
        self.mac_map('111', 'a.bsp', 10, 1)
        det = Path(self.tmp.name) / 'details.json'
        det.write_text(json.dumps({'1': details(1, '111', 'a.bsp', 10)}))
        stage = Path(self.tmp.name) / 'stage'
        out = Path(self.tmp.name) / 'out.txt'
        saved = sys.stdout
        try:
            with open(out, 'w') as sys.stdout:
                code = wp.main(['plan', '--mac-dir', str(self.root), '--details', str(det),
                                '--stage', str(stage), '--dry-run'])
        finally:
            sys.stdout = saved
        self.assertEqual(code, 0)
        self.assertEqual(out.read_text(), 'total\t10\n')
        self.assertFalse(stage.exists())


if __name__ == '__main__':
    unittest.main()
