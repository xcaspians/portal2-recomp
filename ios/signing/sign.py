#!/usr/bin/env python3
"""Signe un paquet .app iOS pour un appareil réel.

Prend le profil de provisionnement le plus récent obtenu par Xcode pour le
CFBundleIdentifier du paquet (voir ios/signing/project.yml), l'embarque et
signe avec l'identité « Apple Development » de son équipe et les droits du
profil.

    python3 ios/signing/sign.py build/ios/device/Portal2.app
"""
import argparse
import hashlib
import plistlib
import subprocess
import sys
import tempfile
from datetime import datetime, timezone
from pathlib import Path

PROFILES = [Path.home() / 'Library/Developer/Xcode/UserData/Provisioning Profiles',
            Path.home() / 'Library/MobileDevice/Provisioning Profiles']


def decode(profile):
    raw = subprocess.run(['security', 'cms', '-D', '-i', str(profile)], capture_output=True, check=True).stdout
    return plistlib.loads(raw)


def choose_profile(candidates, bundle_id, now=None):
    """(path, data) du profil pour bundle_id parmi candidates [(path, data)] :
    le plus récent fait pour cet identifiant, sinon le plus récent profil joker
    (« ÉQUIPE.* », le profil d'équipe de Xcode) ; un profil expiré ne compte
    pas. Le joker évite d'enregistrer un nouvel App ID pour une app de
    développement (Portal Stories: Mel), mais ne porte ni iCloud ni
    increased-memory-limit. None si aucun."""
    now = now or datetime.now(timezone.utc)
    exact = wildcard = None
    for path, data in candidates:
        expires = data.get('ExpirationDate')
        if expires is not None and (expires if expires.tzinfo else expires.replace(tzinfo=timezone.utc)) < now:
            continue
        suffix = data['Entitlements'].get('application-identifier', '').split('.', 1)[-1]
        if suffix == bundle_id:
            if exact is None or data['CreationDate'] > exact[1]['CreationDate']: exact = (path, data)
        elif suffix == '*':
            if wildcard is None or data['CreationDate'] > wildcard[1]['CreationDate']: wildcard = (path, data)
    return exact or wildcard


def app_entitlements(profile_entitlements, bundle_id):
    """Droits signés : ceux du profil, avec l'identifiant de l'app à la place
    du joker d'un profil d'équipe (comme Xcode)."""
    out = dict(profile_entitlements)
    app_id = out.get('application-identifier', '')
    if app_id.endswith('.*'):
        out['application-identifier'] = app_id[:-1] + bundle_id
    return out


def find_profile(bundle_id):
    candidates = [(path, decode(path)) for folder in PROFILES if folder.is_dir()
                  for path in folder.glob('*.mobileprovision')]
    best = choose_profile(candidates, bundle_id)
    if not best:
        sys.exit(f'aucun profil pour {bundle_id} : lancer xcodebuild sur ios/signing (voir project.yml)')
    if best[1]['Entitlements'].get('application-identifier', '').endswith('.*'):
        print(f"{bundle_id} : profil joker de l'équipe (sans increased-memory-limit ni iCloud)", file=sys.stderr)
    return best


ICLOUD_KEYS = ('com.apple.developer.icloud-container-identifiers', 'com.apple.developer.icloud-services',
               'com.apple.developer.ubiquity-container-identifiers', 'com.apple.developer.icloud-container-environment',
               'com.apple.developer.ubiquity-kvstore-identifier')


def narrow_entitlements(entitlements):
    """Droits du profil ramenés à ceux de l'app, comme Xcode le fait. Un profil
    avec iCloud (sauvegardes iCloud, runtime/ios/icloud_saves_ios.m) porte des
    jokers (« * », « ÉQUIPE.* ») qu'une app signée ne peut pas garder : iCloud
    Documents seulement, les conteneurs du profil, l'environnement de
    développement, sans magasin clé-valeur. iCloud coché sans conteneur
    attribué : pas d'iCloud du tout. Sans iCloud : inchangés."""
    narrowed = dict(entitlements)
    if not any(key in narrowed for key in ICLOUD_KEYS):
        return narrowed
    containers = [c for c in narrowed.get('com.apple.developer.icloud-container-identifiers') or [] if '*' not in c]
    for key in ICLOUD_KEYS:
        narrowed.pop(key, None)
    if not containers:
        print('profil avec iCloud mais sans conteneur : app signée sans iCloud (voir docs/ios.md, sauvegardes iCloud)',
              file=sys.stderr)
        return narrowed
    narrowed['com.apple.developer.icloud-container-identifiers'] = containers
    narrowed['com.apple.developer.ubiquity-container-identifiers'] = containers
    narrowed['com.apple.developer.icloud-services'] = ['CloudDocuments']
    narrowed['com.apple.developer.icloud-container-environment'] = 'Development'
    return narrowed


def pick_identity(find_identity_output, profile_certificates):
    """Empreinte de la première identité de signature dont le certificat est
    dans le profil. Avec plusieurs comptes Apple dans Xcode (équipe gratuite
    et équipe payante), la première identité « Apple Development » venue
    pouvait être celle d'une autre équipe : l'iPhone refusait alors l'app."""
    allowed = {hashlib.sha1(der).hexdigest().upper() for der in profile_certificates}
    for line in find_identity_output.splitlines():
        fields = line.split()
        if len(fields) > 1 and fields[1].upper() in allowed:
            return fields[1]   # empreinte : sans ambiguïté s'il y a plusieurs certificats
    sys.exit('aucune identité du trousseau ne correspond aux certificats du profil '
             '(Xcode > Settings > Accounts > Manage Certificates)')


def identity(profile):
    found = subprocess.run(['security', 'find-identity', '-v', '-p', 'codesigning'],
                           capture_output=True, text=True, check=True).stdout
    return pick_identity(found, profile.get('DeveloperCertificates', []))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('app', type=Path, help='paquet .app iOS à signer')
    app = ap.parse_args().app
    if not (app / 'Info.plist').is_file():
        sys.exit(f'{app} : pas de paquet .app (Info.plist absent)')
    bundle_id = plistlib.loads((app / 'Info.plist').read_bytes())['CFBundleIdentifier']
    path, data = find_profile(bundle_id)
    team = data['TeamIdentifier'][0]
    (app / 'embedded.mobileprovision').write_bytes(path.read_bytes())
    with tempfile.TemporaryDirectory() as tmp:
        entitlements = Path(tmp) / 'entitlements.plist'
        entitlements.write_bytes(plistlib.dumps(narrow_entitlements(app_entitlements(data['Entitlements'], bundle_id))))
        subprocess.run(['codesign', '-f', '-s', identity(data), '--entitlements', str(entitlements),
                        '--generate-entitlement-der', str(app)], check=True)
    print(f'{app} signé ({bundle_id}, équipe {team}, profil expirant le {data["ExpirationDate"]:%Y-%m-%d})')


if __name__ == '__main__':
    main()
