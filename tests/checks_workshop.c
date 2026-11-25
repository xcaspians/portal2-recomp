// Community Maps (runtime/ios/workshop.c): the Workshop API's answers, the
// browse request, pasted links, where maps are saved, the update badge, and
// the cleanup of a canceled download. The answers below have the shape of
// Valve's (GetPublishedFileDetails, QueryFiles, GetCollectionDetails,
// GetPlayerSummaries) with invented content.
#include "checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../runtime/ios/workshop.c"

int rt_mods_map_name(const char *relative, char *out, size_t size);   // mods_list.c (checks_mods_list.c)

static const char details[] =
    "{\"response\":{\"result\":1,\"resultcount\":4,\"publishedfiledetails\":["
    "{\"publishedfileid\":\"111\",\"result\":1,\"creator\":\"76561190000000001\",\"creator_app_id\":644,"
    "\"consumer_app_id\":620,\"filename\":\"mymaps/sp_laser \\\"maze\\\".bsp\",\"file_size\":\"14021328\","
    "\"file_url\":\"https://steamusercontent.example/ugc/1/ABC/\",\"preview_url\":\"https://img.example/p.jpg\","
    "\"title\":\"Laser Maze \\u00e9t\\u00e9 \\ud83d\\ude00\",\"description\":\"[h1]Hi[/h1]\\r\\nTwo [b]cubes[/b]:[list][*]one[*]two[/list]"
    "[img]https://x/y.png[/img]End\",\"time_created\":1336500000,\"time_updated\":1336600000,\"subscriptions\":5,"
    "\"favorited\":7,\"lifetime_subscriptions\":777312,\"tags\":[{\"tag\":\"Singleplayer\"},{\"tag\":\"Laser Emitter\"}]},"
    "{\"publishedfileid\":\"222\",\"result\":1,\"consumer_app_id\":620,\"filename\":\"mymaps/1794882194.bsp\","
    "\"file_size\":\"6666408\",\"file_url\":\"https://u/2/\",\"title\":\"Two Bots\",\"time_created\":10,"
    "\"tags\":[{\"tag\":\"Cooperative\"}]},"
    "{\"publishedfileid\":\"333\",\"result\":1,\"consumer_app_id\":620,\"creator_app_id\":766,"
    "\"filename\":\"steamworkshop/collection/1351184136_collection_branding.jpg\",\"title\":\"A Collection\"},"
    "{\"publishedfileid\":\"444\",\"result\":9}"
    "]}}";

static const char query[] =
    "{\"response\":{\"total\":51234,\"publishedfiledetails\":["
    "{\"result\":1,\"publishedfileid\":\"3845455194\",\"creator\":\"76561190000000002\",\"creator_appid\":644,"
    "\"consumer_appid\":620,\"consumer_shortcutid\":0,\"filename\":\"mymaps/1795496734.bsp\",\"file_size\":\"4920600\","
    "\"preview_file_size\":\"123\",\"file_url\":\"https://u/3/\",\"preview_url\":\"https://img/3.jpg\","
    "\"title\":\"Fresh\",\"short_description\":\"A [i]short[/i] one\",\"time_created\":1764000000,\"time_updated\":1764000500,"
    "\"visibility\":0,\"flags\":0,\"file_type\":0,\"subscriptions\":12,\"favorited\":1,\"previews\":[{\"previewid\":\"9\","
    "\"sortorder\":1,\"url\":\"https://img/3b.jpg\",\"size\":10,\"filename\":\"p.jpg\",\"preview_type\":0}],"
    "\"tags\":[{\"tag\":\"Singleplayer\",\"display_name\":\"Singleplayer\"}],\"vote_data\":{\"score\":0.5,\"votes_up\":3,"
    "\"votes_down\":1},\"language\":0,\"maybe_inappropriate_sex\":false}"
    "],\"next_cursor\":\"AoJ4o+bK/pQDf8i9Bw==\"}}";

RT_CHECK(workshop_json_reader) {
    rt_json_tok t[32];
    const char *js = "{\"a\":[1,2,{\"b\":\"x\\ny\"}],\"c\":true,\"d\":{}}";
    int n = rt_json_parse(js, strlen(js), t, 32);
    assert(n == 12 && t[0].type == RT_JSON_OBJECT && t[0].size == 3);   // object, 3 keys, values, inner tokens
    int a = rt_json_get(js, t, n, 0, "a");
    assert(a == 2 && t[a].type == RT_JSON_ARRAY && t[a].size == 3);
    assert(rt_json_next(t, n, a) == 8);   // past the array and the object inside it: the key "c"
    int c = rt_json_get(js, t, n, 0, "c");
    char out[16];
    assert(rt_json_text(js, &t[c], out, sizeof(out)) && !strcmp(out, "true"));
    int b = rt_json_get(js, t, n, 5, "b");
    assert(rt_json_text(js, &t[b], out, sizeof(out)) && !strcmp(out, "x\ny"));
    assert(rt_json_get(js, t, n, 0, "missing") == -1 && rt_json_get(js, t, n, a, "b") == -1);
    // Not JSON, or too many tokens: refused, never half-read.
    assert(rt_json_parse("{\"a\":", 5, t, 32) == -1);
    assert(rt_json_parse("{\"a\":1]", 7, t, 32) == -1);
    assert(rt_json_parse("{1:2}", 5, t, 32) == -1);
    assert(rt_json_parse("[1,2,3,4,5]", 11, t, 3) == -1);
    assert(rt_json_parse("<html>", 6, t, 32) == -1);
    // Text cut at a character boundary, never inside one.
    rt_json_tok s = {RT_JSON_STRING, 1, 7, 0};
    assert(!rt_json_text("\"\xC3\xA9\xC3\xA9\xC3\xA9\"", &s, out, 6) && !strcmp(out, "\xC3\xA9\xC3\xA9"));
}

RT_CHECK(workshop_details_answer) {
    rt_ws_item items[8];
    int n = rt_ws_parse_items(details, sizeof(details) - 1, items, 8, NULL, 0, NULL);
    assert(n == 4);
    const rt_ws_item *m = &items[0];
    assert(m->id == 111 && m->found && m->is_map && !m->is_collection && m->singleplayer && !m->coop);
    assert(!strcmp(m->title, "Laser Maze \xC3\xA9t\xC3\xA9 \xF0\x9F\x98\x80"));   // é and an emoji from \u escapes
    assert(!strcmp(m->creator, "76561190000000001") && m->file_size == 14021328 && m->time_updated == 1336600000);
    assert(m->subscriptions == 777312 && m->favorited == 7);   // lifetime count preferred
    assert(!strcmp(m->filename, "mymaps/sp_laser \"maze\".bsp"));
    assert(!strcmp(m->description, "Hi\nTwo cubes:\n\xE2\x80\xA2 one\n\xE2\x80\xA2 twoEnd"));
    assert(items[1].coop && !items[1].singleplayer && items[1].is_map && items[1].time_updated == 10);
    assert(items[2].is_collection && !items[2].is_map);
    assert(!items[3].found && items[3].id == 444 && !items[3].file_url[0]);   // removed or private chamber
    // The QueryFiles page: cursor, total, short description, previews and vote data skipped.
    char cursor[64]; long long total;
    n = rt_ws_parse_items(query, sizeof(query) - 1, items, 8, cursor, sizeof(cursor), &total);
    assert(n == 1 && total == 51234 && !strcmp(cursor, "AoJ4o+bK/pQDf8i9Bw=="));
    assert(items[0].found && items[0].id == 3845455194ULL && !strcmp(items[0].description, "A short one"));
    assert(items[0].subscriptions == 12 && items[0].time_updated == 1764000500 && items[0].singleplayer);
    // Unreadable answers (an HTML error page, a wrong key's empty body) are -1, not an empty list.
    assert(rt_ws_parse_items("<html>Forbidden</html>", 22, items, 8, NULL, 0, NULL) == -1);
    assert(rt_ws_parse_items("{}", 2, items, 8, NULL, 0, NULL) == -1);
    assert(rt_ws_parse_items("{\"response\":{\"total\":0}}", 24, items, 8, NULL, 0, NULL) == 0);
}

RT_CHECK(workshop_collections_and_names) {
    const char *c = "{\"response\":{\"result\":1,\"resultcount\":1,\"collectiondetails\":[{\"publishedfileid\":\"333\","
                    "\"result\":1,\"children\":[{\"publishedfileid\":\"104608983\",\"sortorder\":1,\"filetype\":0},"
                    "{\"publishedfileid\":\"104609071\",\"sortorder\":2,\"filetype\":0}]}]}}";
    unsigned long long ids[4];
    assert(rt_ws_parse_collection(c, strlen(c), ids, 4) == 2 && ids[0] == 104608983ULL && ids[1] == 104609071ULL);
    assert(rt_ws_parse_collection("{\"response\":{}}", 15, ids, 4) == -1);
    const char *p = "{\"response\":{\"players\":[{\"steamid\":\"76561190000000002\",\"personaname\":\"Cube Fan\"}]}}";
    const char *who[] = {"76561190000000001", "76561190000000002"};
    char names[2][64];
    assert(rt_ws_parse_names(p, strlen(p), who, 2, names) == 1);
    assert(!names[0][0] && !strcmp(names[1], "Cube Fan"));
}

RT_CHECK(workshop_requests) {
    char url[1024];
    assert(rt_ws_query_url(url, sizeof(url), "ABC123", RT_WS_POPULAR, NULL, NULL, 0));
    assert(!strcmp(url, "https://api.steampowered.com/IPublishedFileService/QueryFiles/v1/?key=ABC123&appid=620"
                        "&query_type=9&numperpage=20&filetype=0&cursor=%2A&requiredtags%5B0%5D=Singleplayer"
                        "&return_previews=1&return_short_description=1&return_tags=1"));
    assert(rt_ws_query_url(url, sizeof(url), "K", RT_WS_TRENDING, "AoJ4o+bK/pQDf8i9Bw==", NULL, 1));
    assert(strstr(url, "query_type=3&") && strstr(url, "cursor=AoJ4o%2BbK%2FpQDf8i9Bw%3D%3D&") &&
           strstr(url, "=Cooperative&") && strstr(url, "&days=7"));
    assert(rt_ws_query_url(url, sizeof(url), "K", RT_WS_SEARCH, NULL, "laser & cube", 0));
    assert(strstr(url, "query_type=12&") && strstr(url, "&search_text=laser%20%26%20cube"));
    assert(rt_ws_query_url(url, sizeof(url), "K", RT_WS_NEWEST, NULL, NULL, 0) && strstr(url, "query_type=1&"));
    assert(!rt_ws_query_url(url, 40, "K", RT_WS_NEWEST, NULL, NULL, 0));   // never a cut URL
    unsigned long long two[] = {68539885ULL, 3845455194ULL};
    char body[256];
    assert(rt_ws_details_body(body, sizeof(body), two, 2));
    assert(!strcmp(body, "itemcount=2&publishedfileids[0]=68539885&publishedfileids[1]=3845455194"));
    assert(rt_ws_collection_body(body, sizeof(body), 104612418ULL) &&
           !strcmp(body, "collectioncount=1&publishedfileids[0]=104612418"));
}

RT_CHECK(workshop_pasted_links) {
    unsigned long long id = 0;
    assert(rt_ws_parse_link("https://steamcommunity.com/sharedfiles/filedetails/?id=68539885", &id) && id == 68539885ULL);
    assert(rt_ws_parse_link("steamcommunity.com/workshop/filedetails/?id=3845455194&searchtext=", &id) &&
           id == 3845455194ULL);
    assert(rt_ws_parse_link("  140534764 \n", &id) && id == 140534764ULL);
    assert(!rt_ws_parse_link("", &id) && !rt_ws_parse_link("laser maze", &id) && !rt_ws_parse_link("12a", &id));
    assert(!rt_ws_parse_link("?id=0", &id) && !rt_ws_parse_link("https://example.com/", &id));
    assert(!rt_ws_parse_link("123456789012345678901", &id));   // longer than any 64-bit id
}

RT_CHECK(workshop_map_paths) {
    rt_ws_item it = {0};
    char path[256], name[256];
    it.id = 111;
    snprintf(it.filename, sizeof(it.filename), "mymaps/cc_01_lasers.bsp");
    assert(rt_ws_map_path(&it, path, sizeof(path)) && !strcmp(path, "portal2/maps/workshop/111/cc_01_lasers.bsp"));
    // The Maps & Mods list (mods_list.c) loads it under the name the engine expects.
    assert(rt_mods_map_name(path, name, sizeof(name)) && !strcmp(name, "workshop/111/cc_01_lasers"));
    snprintf(it.filename, sizeof(it.filename), "mymaps/sp_laser \"maze\" v2..final.BSP");
    assert(rt_ws_map_path(&it, path, sizeof(path)) && !strcmp(path, "portal2/maps/workshop/111/sp_laser__maze__v2_.final.bsp"));   // no "..", no quotes or spaces
    assert(rt_mods_map_name(path, name, sizeof(name)));
    snprintf(it.filename, sizeof(it.filename), "mymaps/.bsp");
    assert(rt_ws_map_path(&it, path, sizeof(path)) && !strcmp(path, "portal2/maps/workshop/111/111.bsp"));
    assert(rt_ws_item_dir(111, path, sizeof(path)) && !strcmp(path, "portal2/maps/workshop/111"));
}

RT_CHECK(workshop_update_badge) {
    assert(rt_ws_update_available(100, 5, 200, 5));      // updated on the Workshop since
    assert(!rt_ws_update_available(200, 5, 200, 5));     // same version
    assert(!rt_ws_update_available(300, 5, 200, 5));     // saved copy newer (clock skew): no badge
    assert(rt_ws_update_available(200, 5, 200, 9));      // same date, new file
    assert(!rt_ws_update_available(0, 5, 200, 9));       // no note saved: unknown, no badge
    assert(!rt_ws_update_available(200, 5, 0, 0));       // the Workshop didn't say
}

static void touch_file(const char *path, const char *text) {
    FILE *f = fopen(path, "w");
    assert(f);
    fputs(text, f);
    fclose(f);
}
static int exists(const char *path) { struct stat st; return stat(path, &st) == 0; }

RT_CHECK(workshop_saved_maps_and_canceled_downloads) {
    char root[] = "/tmp/p2-workshop-check-XXXXXX";
    assert(mkdtemp(root));
    char path[512], a[512], b[512], c[512], d[512];
    // A finished map with its note: kept.
    snprintf(a, sizeof(a), "%s/111", root); mkdir(a, 0755);
    snprintf(path, sizeof(path), "%s/cc_01.bsp", a); touch_file(path, "VBSP");
    rt_ws_item it = {0};
    it.id = 111; it.time_updated = 1336600000; it.file_size = 4;
    snprintf(it.title, sizeof(it.title), "Laser\nMaze");
    snprintf(path, sizeof(path), "%s/p2workshop.txt", a);
    assert(rt_ws_meta_write(path, &it));
    long long updated, size; char title[64];
    assert(rt_ws_meta_read(path, &updated, &size, title, sizeof(title)));
    assert(updated == 1336600000 && size == 4 && !strcmp(title, "Laser Maze"));
    // A canceled download: the partial file and its note go, then the folder.
    snprintf(b, sizeof(b), "%s/222", root); mkdir(b, 0755);
    snprintf(path, sizeof(path), "%s/x.bsp.part", b); touch_file(path, "VB");
    snprintf(path, sizeof(path), "%s/p2workshop.txt", b); touch_file(path, "updated=1\n");
    // An update canceled halfway: the old map stays, the partial goes.
    snprintf(path, sizeof(path), "%s/cc_01.bsp.part", a); touch_file(path, "VBS");
    // Something that isn't ours (a name not made of digits, a folder with other files): untouched.
    snprintf(c, sizeof(c), "%s/my_folder", root); mkdir(c, 0755);
    snprintf(path, sizeof(path), "%s/x.bsp.part", c); touch_file(path, "?");
    snprintf(d, sizeof(d), "%s/333", root); mkdir(d, 0755);
    snprintf(path, sizeof(path), "%s/readme.txt", d); touch_file(path, "mine");
    assert(rt_ws_cleanup(root) == 3);   // two .part files and one note
    assert(!exists(b));
    snprintf(path, sizeof(path), "%s/cc_01.bsp", a); assert(exists(path));
    snprintf(path, sizeof(path), "%s/cc_01.bsp.part", a); assert(!exists(path));
    snprintf(path, sizeof(path), "%s/x.bsp.part", c); assert(exists(path));
    assert(exists(d));
    assert(rt_ws_cleanup(root) == 0);   // nothing left to do
    // Deleting a map removes its folder.
    assert(rt_ws_delete(a) && !exists(a));
    assert(!rt_ws_delete(a));
    // tidy up the rest
    snprintf(path, sizeof(path), "%s/x.bsp.part", c); unlink(path); rmdir(c);
    snprintf(path, sizeof(path), "%s/readme.txt", d); unlink(path); rmdir(d);
    assert(rmdir(root) == 0);
}
