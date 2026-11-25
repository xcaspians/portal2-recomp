#!/usr/bin/env python3
"""Generate runtime/steam_generated.c: Steam interface tables for the guest bridge (Portal 2: SDK 1.53a;
Half-Life 2: SDK 1.60 and the Timeline V004 of 1.61, see SOURCES).

Sources, all local:
- vtable slots: ARM64 disassembly of the official `libsteam_api.dylib`. Each flat function
  `SteamAPI_ISteamX_Method` is a tail call `ldr x8,[x0]; ldr xN,[x8,#off]; br xN`: the slot is
  off/8 (Itanium ABI, same order as i386 GCC) and N, the first free register, gives the number
  of integer arguments (cross-check of the signatures);
- signatures: `NativeMethods.cs` of the same Steamworks.NET package (MIT), and the return
  type of the C# wrappers (CSteamID/CGameID returned by value, strings, interfaces);
- ISteamController has no C# counterpart: its methods take the signature of the ISteamInput
  method of the same name when the disassembled argument count agrees.

A method whose marshalling cannot be derived safely from the types stays a named trap.
The generated file only holds names, slots and argument conversions.
"""
import argparse
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Versions demandées par Portal 2 (docs/steam.md), identiques aux accesseurs du SDK 1.53a.
VERSIONS_PORTAL2 = {
    'ISteamClient': 'SteamClient020', 'ISteamUser': 'SteamUser021', 'ISteamFriends': 'SteamFriends017',
    'ISteamUtils': 'SteamUtils010', 'ISteamMatchmaking': 'SteamMatchMaking009',
    'ISteamGameSearch': 'SteamMatchGameSearch001', 'ISteamMatchmakingServers': 'SteamMatchMakingServers002',
    'ISteamUserStats': 'STEAMUSERSTATS_INTERFACE_VERSION012', 'ISteamApps': 'STEAMAPPS_INTERFACE_VERSION008',
    'ISteamNetworking': 'SteamNetworking006', 'ISteamRemoteStorage': 'STEAMREMOTESTORAGE_INTERFACE_VERSION016',
    'ISteamScreenshots': 'STEAMSCREENSHOTS_INTERFACE_VERSION003', 'ISteamHTTP': 'STEAMHTTP_INTERFACE_VERSION003',
    'ISteamController': 'SteamController008', 'ISteamUGC': 'STEAMUGC_INTERFACE_VERSION016',
    'ISteamAppList': 'STEAMAPPLIST_INTERFACE_VERSION001', 'ISteamMusic': 'STEAMMUSIC_INTERFACE_VERSION001',
    'ISteamMusicRemote': 'STEAMMUSICREMOTE_INTERFACE_VERSION001',
    'ISteamHTMLSurface': 'STEAMHTMLSURFACE_INTERFACE_VERSION_005', 'ISteamInventory': 'STEAMINVENTORY_INTERFACE_V003',
    'ISteamVideo': 'STEAMVIDEO_INTERFACE_V002', 'ISteamParentalSettings': 'STEAMPARENTALSETTINGS_INTERFACE_VERSION001',
    'ISteamInput': 'SteamInput006', 'ISteamGameServer': 'SteamGameServer014',
    'ISteamGameServerStats': 'SteamGameServerStats001',
}
# Versions demandées par Half-Life 2 (mise à jour des 20 ans) et Portal 1, en plus de celles
# de Portal 2 : SDK 1.60, sauf ISteamTimeline que client.so de HL2 demande en V004 (SDK 1.61,
# voir SOURCES) ; Portal 1 n'utilise pas Timeline (docs/steam.md).
VERSIONS_HL2 = {
    'ISteamClient': 'SteamClient021', 'ISteamUser': 'SteamUser023', 'ISteamUGC': 'STEAMUGC_INTERFACE_VERSION020',
    'ISteamVideo': 'STEAMVIDEO_INTERFACE_V007', 'ISteamGameServer': 'SteamGameServer015',
    'ISteamParties': 'SteamParties002', 'ISteamRemotePlay': 'STEAMREMOTEPLAY_INTERFACE_VERSION002',
    'ISteamNetworkingUtils': 'SteamNetworkingUtils004', 'ISteamNetworkingSockets': 'SteamNetworkingSockets012',
    'ISteamNetworkingMessages': 'SteamNetworkingMessages002',
}
VERSIONS_TIMELINE = {'ISteamTimeline': 'STEAMTIMELINE_INTERFACE_V004'}
# Versions demandées par Portal Stories: Mel (SDK 1.40 de 2017, plus anciennes que celles de
# Portal 2) ; les autres interfaces qu'il demande ont la même version que Portal 2. Aucune
# dylib arm64 de ce SDK n'existe : les emplacements viennent des fonctions flat i386 de son
# propre libsteam_api.so (dépôt Linux 317404), les signatures de Steamworks.NET 20.1.0.
VERSIONS_MEL = {
    'ISteamClient': 'SteamClient017', 'ISteamUser': 'SteamUser019', 'ISteamFriends': 'SteamFriends015',
    'ISteamUtils': 'SteamUtils009', 'ISteamUserStats': 'STEAMUSERSTATS_INTERFACE_VERSION011',
    'ISteamRemoteStorage': 'STEAMREMOTESTORAGE_INTERFACE_VERSION014', 'ISteamUGC': 'STEAMUGC_INTERFACE_VERSION010',
    'ISteamHTTP': 'STEAMHTTP_INTERFACE_VERSION002', 'ISteamController': 'SteamController005',
    'ISteamGameServer': 'SteamGameServer012', 'ISteamNetworking': 'SteamNetworking005',
}
DOWNLOADS = ROOT.parent / 'work/downloads'
# (dossier Steamworks.NET dépaqueté, versions tirées de sa dylib). Une version déjà produite
# par une source précédente n'est pas reprise : sa disposition de vtable ne change jamais.
SOURCES = [
    ('steamworks', VERSIONS_PORTAL2),          # Steamworks.NET 20.1.0, SDK 1.53a
    ('steamworks-2024.8.0', VERSIONS_HL2),      # SDK 1.60
    ('steamworks-2025.161.0', VERSIONS_TIMELINE),  # SDK 1.61
]
# Sources dont les emplacements viennent d'un libsteam_api.so i386 (signatures : premier paquet).
GUEST_SOURCES = [
    ('mel', VERSIONS_MEL),   # Portal Stories: Mel, SDK 1.40
]
# Scalaires C# -> (type C, octets sur la pile i386).
SCALARS = {'int': ('int32_t', 4), 'uint': ('uint32_t', 4), 'short': ('int16_t', 4), 'ushort': ('uint16_t', 4),
           'byte': ('uint8_t', 4), 'sbyte': ('int8_t', 4), 'bool': ('bool', 4), 'long': ('int64_t', 8),
           'ulong': ('uint64_t', 8), 'float': ('float', 4), 'double': ('double', 8)}
# Structures sans pointeur, pack(4) sur Linux comme sur macOS (Packsize.cs) : même disposition,
# recopiées telles quelles. Leur taille est calculée par SDK (struct_sizes) : SteamUGCDetails_t
# gagne m_ulTotalFilesSize en 1.60 (9764 -> 9772 octets).
PLAIN_STRUCTS = ('FriendGameInfo_t', 'LeaderboardEntry_t', 'SteamUGCDetails_t')
# Tableaux de taille fixe documentée (STEAM_INPUT_MAX_COUNT, _MAX_ACTIVE_LAYERS, _MAX_ORIGINS).
FIXED_ARRAYS = {'GetConnectedControllers': 16, 'GetActiveActionSetLayers': 16,
                'GetDigitalActionOrigins': 8, 'GetAnalogActionOrigins': 8}
# ISteamController -> méthode équivalente d'ISteamInput (Init/RunFrame n'y prennent aucun argument).
CONTROLLER_TWINS = {'TriggerHapticPulse': 'Legacy_TriggerHapticPulse',
                    'TriggerRepeatedHapticPulse': 'Legacy_TriggerRepeatedHapticPulse',
                    'GetControllerBindingRevision': 'GetDeviceBindingRevision',
                    'GetGlyphForActionOrigin': 'GetGlyphForActionOrigin_Legacy'}


# Tampons lus par Steam (méthode, paramètre) ; tous les autres sont écrits par Steam et
# vérifiés en écriture côté invité.
INPUT_BUFFERS = {
    ('FileWrite', 'pvData'), ('FileWriteAsync', 'pvData'), ('FileWriteStreamWriteChunk', 'pvData'),
    ('WriteScreenshot', 'pubRGB'), ('BeginAuthSession', 'pAuthTicket'), ('DecompressVoice', 'pCompressed'),
    ('RequestEncryptedAppTicket', 'pDataToInclude'), ('DownloadLeaderboardEntriesForUsers', 'prgUsers'),
    ('UploadLeaderboardScore', 'pScoreDetails'), ('DownloadClanActivityCounts', 'psteamIDClans'),
    ('HandleIncomingPacket', 'pData'), ('SendUserConnectAndAuthenticate_DEPRECATED', 'pvAuthBlob'),
    ('GetItemsByID', 'pInstanceIDs'), ('DeserializeResult', 'pBuffer'), ('SendLobbyChatMsg', 'pvMsgBody'),
    ('SetPNGIcon_64x64', 'pvBuffer'), ('UpdateCurrentEntryCoverArt', 'pvBuffer'), ('SendP2PPacket', 'pubData'),
    ('SendDataOnSocket', 'pubData'), ('SendMessageToUser', 'pubData'), ('SendMessageToConnection', 'pData'),
    ('ReceivedRelayAuthTicket', 'pvTicket'), ('ReceivedP2PCustomSignal', 'pMsg'), ('SetCertificate', 'pCertificate'),
    ('SendSignal', 'pMsg'), ('SendRejectionSignal', 'pMsg'),
    ('StartPlaytimeTracking', 'pvecPublishedFileID'), ('StopPlaytimeTracking', 'pvecPublishedFileID'),
    ('CreateQueryUGCDetailsRequest', 'pvecPublishedFileID'),
}


def natural_size(scalar):
    if not scalar:
        return None
    return {'bool': 1, 'int8_t': 1, 'uint8_t': 1, 'int16_t': 2, 'uint16_t': 2}.get(scalar[0], scalar[1])


SIZE_NAME = re.compile(r'^(cub|cb|cch|un?Max|unNum\w+|c[A-Z]|n\w*Size|\w*BufferSize|\w*Max\w*|\w*Count\w*|un\w*Size\w*)')


def sdk_files(pkg):
    files = {}
    for entry in Path(pkg).iterdir():
        name = entry / 'pathname'
        if name.is_file() and (entry / 'asset').is_file():
            files[name.read_text().splitlines()[0]] = entry / 'asset'
    return files


def typedefs(files):
    """Wrapper types (AppId_t...) -> C# scalar; enums -> int."""
    result = {}
    for path, asset in files.items():
        if '/Runtime/types/' in path and path.endswith('.cs'):
            text = asset.read_text()
            struct = re.search(r'public struct (\w+)', text)
            fields = re.findall(r'public (\w+) m_\w+;', text)
            if struct and len(fields) == 1 and fields[0] in SCALARS:
                result[struct.group(1)] = fields[0]
    enums = files['Assets/com.rlabrecque.steamworks.net/Runtime/autogen/SteamEnums.cs'].read_text()
    for name, base in re.findall(r'public enum (\w+) : (\w+)', enums):
        result[name] = base
    return result


def struct_sizes(files, types):
    """PLAIN_STRUCTS -> taille i386/arm64 en pack(4), d'après SteamStructs.cs du paquet."""
    root = 'Assets/com.rlabrecque.steamworks.net/Runtime/autogen/'
    constants = dict((k, eval(v, {})) for k, v in re.findall(
        r'public const int (\w+) = ([\d +\-*]+);', files[root + 'SteamConstants.cs'].read_text()))
    text = files[root + 'SteamStructs.cs'].read_text()
    result = {}
    for struct in PLAIN_STRUCTS:
        body = re.search(r'public struct ' + struct + r' \{(.*?)\n\t\}', text, re.S)
        if not body:
            continue
        size = 0
        for decl in re.finditer(r'(?:\[MarshalAs\(UnmanagedType\.(\w+)(?:, SizeConst = Constants\.(\w+))?\)\]\s*)?'
                                r'(?:public|private) ([\w\[\]]+) (m_\w+);', body.group(1)):
            marshal, count, kind, name = decl.groups()
            if kind == 'byte[]' and marshal == 'ByValArray':
                width, align = constants[count], 1
            elif kind in ('CSteamID', 'CGameID'):
                width = align = 8
            elif SCALARS.get(types.get(kind, kind)):
                width = natural_size(SCALARS[types.get(kind, kind)])
                align = width
            else:
                raise SystemExit(f'{struct}.{name} : type {kind} non géré')
            align = min(align, 4)
            size = (size + align - 1) // align * align + width
        result[struct] = (size + 3) // 4 * 4
    return result


def disassemble(dylib):
    with tempfile.TemporaryDirectory() as tmp:
        thin = Path(tmp) / 'arm64.dylib'
        subprocess.run(['lipo', '-thin', 'arm64', str(dylib), '-output', str(thin)], check=True)
        text = subprocess.run(['objdump', '-d', str(thin)], check=True, capture_output=True, text=True).stdout
    bodies, current = {}, None
    for line in text.splitlines():
        m = re.match(r'^[0-9a-f]+ <_(\w+)>:', line)
        if m:
            current = m.group(1); bodies[current] = []; continue
        parts = line.split('\t')
        if current and len(parts) >= 3:
            bodies[current].append((parts[1].strip(), parts[2].strip()))
    slots = {}
    for name, body in bodies.items():
        if not name.startswith('SteamAPI_ISteam') or len(body) != 3:
            continue
        a, b, c = body
        m = re.fullmatch(r'x(\d+), \[x8(?:, #0x([0-9a-f]+))?\]', b[1])
        if a == ('ldr', 'x8, [x0]') and b[0] == 'ldr' and m and c == ('br', f'x{m.group(1)}'):
            slots[name] = (int(m.group(2) or '0', 16) // 8, int(m.group(1)))
    return slots, set(bodies)


I386_LOADS8 = ('movsd', 'movq', 'fldl')
# Noms flat des anciens SDK -> nom actuel. Les surcharges y étaient numérotées dans l'ordre de
# déclaration (GetStat, GetStat0) ; le SDK 1.53a leur donne des noms distincts aux mêmes
# emplacements (vérifié : GetStat/GetStatInt32 en 1, GetStat0/GetStatFloat en 2, etc.).
OLD_FLAT_NAMES = {
    **{f'SteamAPI_ISteamUserStats_{old}': f'SteamAPI_ISteamUserStats_{new}' for old, new in {
        'GetStat': 'GetStatInt32', 'GetStat0': 'GetStatFloat', 'SetStat': 'SetStatInt32', 'SetStat0': 'SetStatFloat',
        'GetUserStat': 'GetUserStatInt32', 'GetUserStat0': 'GetUserStatFloat',
        'GetGlobalStat': 'GetGlobalStatInt64', 'GetGlobalStat0': 'GetGlobalStatDouble',
        'GetGlobalStatHistory': 'GetGlobalStatHistoryInt64', 'GetGlobalStatHistory0': 'GetGlobalStatHistoryDouble',
    }.items()},
    'SteamAPI_ISteamGameServer_EnableHeartbeats': 'SteamAPI_ISteamGameServer_SetAdvertiseServerActive',
    'SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate': 'SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate_DEPRECATED',
    'SteamAPI_ISteamGameServer_SendUserDisconnect': 'SteamAPI_ISteamGameServer_SendUserDisconnect_DEPRECATED',
    'SteamAPI_ISteamUser_InitiateGameConnection': 'SteamAPI_ISteamUser_InitiateGameConnection_DEPRECATED',
    'SteamAPI_ISteamUser_TerminateGameConnection': 'SteamAPI_ISteamUser_TerminateGameConnection_DEPRECATED',
    'SteamAPI_ISteamUGC_CreateQueryAllUGCRequest': 'SteamAPI_ISteamUGC_CreateQueryAllUGCRequestPage',
}
# Paramètres ajoutés à la fin depuis l'ancien SDK (nombre retiré de la signature actuelle) :
# ActivateGameOverlayToWebPage gagne eMode en 1.46.
OLD_TRAILING_PARAMS = {'SteamAPI_ISteamFriends_ActivateGameOverlayToWebPage': 1}


def disassemble_i386(elf):
    """Flat functions of an i386 libsteam_api.so -> (slot, 4-byte argument words).

    Each one loads `this` from its first argument, reads the vtable (`movl (%eA), %eB`) and calls
    or jumps through `*off(%eB)`: the slot is off/4. The argument words are the highest stack
    argument the function reads (frame-pointer or esp-relative, tracking sub/push), a cross-check
    of the signature taken from Steamworks.NET."""
    text = subprocess.run(['objdump', '-d', str(elf)], check=True, capture_output=True, text=True).stdout
    bodies, current = {}, None
    for line in text.splitlines():
        m = re.match(r'^[0-9a-f]+ <(\w+)>:', line)
        if m:
            current = m.group(1); bodies[current] = []; continue
        parts = line.split('\t')
        if current and len(parts) >= 2:
            bodies[current].append((parts[1].strip(), parts[2].strip() if len(parts) > 2 else ''))
    result = {}
    for name, body in bodies.items():
        if not name.startswith('SteamAPI_ISteam'):
            continue
        delta, frame, words, vreg, slot = 0, False, 1, None, None
        for op, args in body:
            if op == 'subl' and args.endswith(', %esp'):
                delta += int(args.split(',')[0].lstrip('$'), 16)
            elif op == 'pushl':
                delta += 4
            elif op == 'movl' and args == '%esp, %ebp':
                frame = True
            source = args.split(', ')[0] if ', ' in args else args
            m = re.fullmatch(r'(0x[0-9a-f]+)?\(%(esp|ebp)\)', source)
            if m and op not in ('calll', 'jmpl') and not op.startswith('lea'):
                off = int(m.group(1) or '0', 16)
                first = 8 if m.group(2) == 'ebp' and frame else delta + 4 if m.group(2) == 'esp' else None
                if first is not None and off >= first:
                    words = max(words, (off - first) // 4 + (2 if op in I386_LOADS8 else 1))
            m = re.fullmatch(r'\(%(e(?:[a-d]x|si|di))\), %(e(?:[a-d]x|si|di))', args)
            if op == 'movl' and m and vreg is None:
                vreg = m.group(2)
            m = re.fullmatch(r'\*(0x[0-9a-f]+)?\(%(e(?:[a-d]x|si|di))\)', args)
            if op in ('calll', 'jmpl') and m and vreg and m.group(2) == vreg:
                slot = int(m.group(1) or '0', 16) // 4
                break
        if slot is not None:
            result[name] = (slot, words)
    return result


def native_methods(files):
    text = files['Assets/com.rlabrecque.steamworks.net/Runtime/autogen/NativeMethods.cs'].read_text()
    result = {}
    for m in re.finditer(r'EntryPoint = "(SteamAPI_ISteam\w+)"[^\n]*\n((?:\s*\[return:[^\n]*\n)?)\s*public static extern (\S+) \w+\((.*)\);\s*$', text, re.M):
        params = []
        text_params = re.sub(r'\[MarshalAs\(UnmanagedType\.I1\)\]\s*', '', m.group(4)).replace('[In, Out] ', 'inout ')
        for raw in [p.strip() for p in text_params.split(',')][1:]:
            *kind, name = raw.split()
            params.append((' '.join(kind), name))
        result[m.group(1)] = (m.group(3), params)
    return result


def wrapper_returns(files):
    result = {}
    for path, asset in files.items():
        if '/Runtime/autogen/isteam' not in path:
            continue
        text = asset.read_text()
        for m in re.finditer(r'public static (\S+) \w+\([^)]*\) \{(.*?)\n\t\t\}', text, re.S):
            for call in re.findall(r'NativeMethods\.(ISteam\w+)\(', m.group(2)):
                result.setdefault('SteamAPI_' + call, m.group(1))
    return result



# Méthodes écrites à la main dans runtime/steam.c (rappels invités, etc.).
MANUAL = {
    'SteamAPI_ISteamClient_SetWarningMessageHook': 'steam_manual_client_warning_hook',
    'SteamAPI_ISteamUtils_SetWarningMessageHook': 'steam_manual_utils_warning_hook',
}

class Unsupported(Exception):
    pass


def plan_method(flat, method, ret, params, wrapper, types, gprs, slot, structs=None):
    """Return (C body, sret); raise Unsupported with the reason."""
    def scalar(t):
        t = types.get(t, t)
        return SCALARS.get(t)
    count = 1 + sum(1 for kind, _ in params if not (scalar(kind) and scalar(kind)[0] in ('float', 'double')))
    if count != gprs:
        raise Unsupported(f'signature C# ({count} entiers) != dylib ({gprs})')
    lines, call, offset, post = [], ['self'], 0, []
    proto = ['void *']
    names = [n for _, n in params]
    for index, (kind, name) in enumerate(params):
        var = f'a{index}'
        at = f'args + {offset}u'
        if scalar(kind):
            ctype, size = scalar(kind)
            read = {'float': f'RF({at})', 'double': f'RD({at})', 'bool': f'(R8({at}) != 0)'}.get(
                ctype, f'({ctype})R64({at})' if size == 8 else f'({ctype})R32({at})')
            lines.append(f'{ctype} {var} = {read};')
            proto.append(ctype); offset += size
        elif kind in ('CSteamID', 'CGameID'):
            lines.append(f'uint64_t {var} = R64({at});'); proto.append('uint64_t'); offset += 8
        elif kind == 'HServerListRequest':
            # Requêtes de liste de serveurs non pontées (rappels vers des objets invités) :
            # l'invité ne peut détenir que le handle nul, que Steam traite comme invalide.
            lines.append(f'if (R32({at})) rt_trap(c, c->eip, "Steam: server list requests are not bridged");')
            lines.append(f'void *{var} = NULL;'); proto.append('void *'); offset += 4
        elif kind == 'InteropHelp.UTF8StringHandle':
            lines.append(f'const char *{var} = steam_in_string(c, R32({at}));'); proto.append('const char *'); offset += 4
        elif kind == 'out IntPtr' and name.startswith('ppch'):
            # char ** rempli par Steam (GetUGCDetails) : la chaîne hôte est recopiée, internée,
            # en mémoire invitée et l'invité reçoit son adresse.
            lines.append(f'void *{var}_slot = steam_out(c, R32({at}), 4u); const char *{var}_text = NULL;')
            lines.append(f'const char **{var} = {var}_slot ? &{var}_text : NULL;')
            post.append(f'if ({var}_slot) {{ uint32_t {var}_guest = steam_guest_string(c, {var}_text); '
                        f'memcpy({var}_slot, &{var}_guest, 4); }}')
            proto.append('const char **'); offset += 4
        elif kind.startswith(('out ', 'ref ')):
            inner = kind.split()[1]
            if scalar(inner):
                size = natural_size(scalar(inner))
            elif inner in ('CSteamID', 'CGameID'):
                size = 8
            elif inner in (structs or {}):
                size = structs[inner]
            else:
                raise Unsupported(f'paramètre {kind} {name}')
            lines.append(f'void *{var} = steam_out(c, R32({at}), {size}u);'); proto.append('void *'); offset += 4
        elif kind in ('byte[]', 'IntPtr') or (kind.startswith('inout ') and kind.endswith('[]')):
            following = params[index + 1] if index + 1 < len(params) else None
            esize = 1
            if kind.startswith('inout '):
                element = kind[len('inout '):-2]
                esize = 8 if element in ('CSteamID', 'CGameID') else natural_size(scalar(element))
                if not esize:
                    raise Unsupported(f'tableau {kind} {name}')
            helper = 'steam_in_buffer' if (method, name) in INPUT_BUFFERS else 'steam_out_buffer'
            if method in FIXED_ARRAYS and index == len(params) - 1 and kind.startswith('inout '):
                lines.append(f'void *{var} = {helper}(c, R32({at}), {FIXED_ARRAYS[method]}, {esize}u);')
            elif following and scalar(following[0]) and scalar(following[0])[1] == 4 and SIZE_NAME.match(following[1]):
                lines.append(f'void *{var} = {helper}(c, R32({at}), (int64_t)(int32_t)R32(args + {offset + 4}u), {esize}u);')
            else:
                raise Unsupported(f'tampon {kind} {name} sans taille')
            proto.append('void *'); offset += 4
        else:
            raise Unsupported(f'paramètre {kind} {name}')
        call.append(var)
    sret = False
    rtype = types.get(ret, ret)
    if ret == 'void':
        rproto, store = 'void', None
    elif wrapper in ('CSteamID', 'CGameID'):
        rproto, store, sret = 'uint64_t', 'steam_ret_sret(c, sret, r);', True
    elif rtype in SCALARS:
        ctype, size = SCALARS[rtype]
        rproto = ctype
        store = {'bool': 'c->eax = r ? 1u : 0u;', 'float': 'rt_return_st0(c, r);', 'double': 'rt_return_st0(c, r);'}.get(
            ctype, 'steam_ret64(c, (uint64_t)r);' if size == 8 else 'c->eax = (uint32_t)r;')
    elif ret == 'IntPtr' and wrapper == 'string':
        rproto, store = 'const char *', 'c->eax = steam_guest_string(c, r);'
    elif ret == 'IntPtr' and flat.startswith('SteamAPI_ISteamClient_GetISteam') and 'pchVersion' in names:
        rproto, store = 'void *', f'c->eax = steam_proxy(c, r, a{names.index("pchVersion")});'
    else:
        raise Unsupported(f'retour {ret} ({wrapper})')
    if slot is None:
        # Version du SDK de la dylib chargée par Portal 2 : appel de la fonction flat par son nom
        # (le bouchon Steam d'iOS, runtime/ios/steam_stub.c, définit ces mêmes noms).
        body = [f'typedef {rproto} (*F)({", ".join(proto)});', 'static void *cache;',
                'void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);',
                f'if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "{flat}"), __ATOMIC_RELAXED);', *lines]
    else:
        # Version d'un autre SDK : la fonction flat n'est que `ldr x8,[x0]; ldr xN,[x8,#off]; br xN`
        # (vérifié par disassemble) avec la disposition du SDK de sa dylib. On appelle donc
        # directement l'emplacement de la vtable de l'objet hôte, propre à la version demandée.
        body = [f'typedef {rproto} (*F)({", ".join(proto)});', f'void *fn = steam_slot(c, self, {slot}u);', *lines]
    body.append(f'{"" if rproto == "void" else rproto + " r = "}((F)fn)({", ".join(call)});')
    body.extend(post)
    if store:
        body.append(store)
    return body, sret


def generate(sources, guest_sources=()):
    """sources: [(dossier du paquet, dylib, {classe: version})], dans l'ordre de priorité ;
    guest_sources: [(dossier du paquet, libsteam_api.so i386, {classe: version})]."""
    methods, interfaces, stats = [], [], {}
    for pkg, dylib, versions in sources:
        wanted = {k: v for k, v in versions.items() if v not in {i[0] for i in interfaces}}
        generate_source(pkg, dylib, wanted, methods, interfaces, stats, direct=bool(interfaces))
    for pkg, elf, versions in guest_sources:
        wanted = {k: v for k, v in versions.items() if v not in {i[0] for i in interfaces}}
        generate_source(pkg, None, wanted, methods, interfaces, stats, direct=True, guest_elf=elf)
    return emit(methods, interfaces), stats, methods


def i386_words(params, types):
    """4-byte stack words of `this` plus the C# parameters."""
    words = 1
    for kind, _ in params:
        if kind in ('CSteamID', 'CGameID'):
            words += 2
            continue
        size = SCALARS.get(types.get(kind, kind))
        words += 2 if size and size[1] == 8 else 1
    return words


def generate_source(pkg, dylib, versions, methods, interfaces, stats, direct, guest_elf=None):
    files = sdk_files(pkg)
    types = typedefs(files)
    structs = struct_sizes(files, types)
    guest_words = disassemble_i386(guest_elf) if guest_elf else None
    slots = {f: (slot, None) for f, (slot, _) in guest_words.items()} if guest_words else disassemble(dylib)[0]
    natives = native_methods(files)
    wrappers = wrapper_returns(files)
    # ISteamController : signatures empruntées à ISteamInput (voir docstring).
    for flat in list(slots):
        if flat.startswith('SteamAPI_ISteamController_'):
            method = flat[len('SteamAPI_ISteamController_'):]
            twin = 'SteamAPI_ISteamInput_' + CONTROLLER_TWINS.get(method, method)
            if twin in natives:
                ret, params = natives[twin]
                natives[flat] = (ret, [] if method in ('Init', 'RunFrame') else params)
                wrappers.setdefault(flat, wrappers.get(twin))
    for klass, version in versions.items():
        prefix = f'SteamAPI_{klass}_'
        own = sorted((s, f) for f, s in slots.items() if f.startswith(prefix))
        if not own:
            raise SystemExit(f'{klass}: aucune fonction flat trouvée')
        count = max(s[0] for s, _ in own) + 1
        interfaces.append((version, klass, count))
        seen = {}
        for (slot, gprs), flat in own:
            method = flat[len(prefix):]
            if slot in seen:
                raise SystemExit(f'{klass}: emplacement {slot} partagé par {seen[slot]} et {method}')
            seen[slot] = method
            entry = dict(name=f'{version}::{method}', iface=len(interfaces) - 1, slot=slot, flat=flat)
            if flat in MANUAL:
                entry['fn'] = MANUAL[flat]
                methods.append(entry)
                continue
            try:
                if OLD_FLAT_NAMES.get(flat, flat) not in natives:
                    raise Unsupported('signature absente de Steamworks.NET')
                if guest_words is not None:
                    current = OLD_FLAT_NAMES.get(flat, flat)
                    if current not in natives:
                        raise Unsupported('signature absente de Steamworks.NET')
                    ret, params = natives[current]
                    params = params[:len(params) - OLD_TRAILING_PARAMS.get(flat, 0)]
                    wrappers.setdefault(flat, wrappers.get(current))
                else:
                    ret, params = natives[flat]
                if guest_words is not None:
                    # SDK i386 : vérifier le nombre de mots de pile, puis passer à plan_method le
                    # nombre d'entiers qu'il attend (aucune dylib arm64 à comparer).
                    found, expected = guest_words[flat][1], i386_words(params, types)
                    if found != expected:
                        raise Unsupported(f'signature C# ({expected} mots de pile) != libsteam_api.so ({found})')
                    gprs = 1 + sum(1 for kind, _ in params
                                   if (SCALARS.get(types.get(kind, kind)) or ('',))[0] not in ('float', 'double'))
                entry['body'], entry['sret'] = plan_method(flat, method, ret, params, wrappers.get(flat), types, gprs, slot if direct else None, structs)
            except Unsupported as exc:
                entry['why'] = str(exc)
            methods.append(entry)
        stats[version] = (len(own), sum(('body' in m or 'fn' in m) for m in methods if m['iface'] == len(interfaces) - 1), count)


def emit(methods, interfaces):
    methods.sort(key=lambda m: m['name'])
    out = ['/* Généré par tools/gen_steam.py depuis les libsteam_api.dylib officielles (SDK 1.53a, 1.60,',
           ' * 1.61), le libsteam_api.so i386 de Portal Stories: Mel (SDK 1.40, emplacements seulement)',
           ' * et Steamworks.NET 20.1.0, 2024.8.0, 2025.161.0 (MIT). Ne pas modifier : relancer',
           ' * le générateur. */',
           '#include "steam.h"', '']
    out.extend(f'void {fn}(CPU *c, void *self, uint32_t args, uint32_t sret);' for fn in sorted(set(MANUAL.values())))
    for index, m in enumerate(methods):
        if 'body' in m:
            m['fn'] = f'm{index}'
            out.append(f'/* {m["name"]} */')
            out.append(f'static void m{index}(CPU *c, void *self, uint32_t args, uint32_t sret) {{')
            out.append('    (void)args; (void)sret;')
            out.extend('    ' + line for line in m['body'])
            out.append('}')
    out.append('const SteamInterfaceInfo steam_interfaces[] = {')
    out.extend(f'    {{{json.dumps(v)}, {json.dumps(k)}, {n}u}},' for v, k, n in interfaces)
    out.append('};')
    out.append(f'const size_t steam_interface_count = {len(interfaces)};')
    out.append('const SteamMethodInfo steam_methods[] = {')
    for m in methods:
        why = json.dumps(m.get('why')) if 'why' in m else 'NULL'
        out.append(f'    {{{json.dumps(m["name"])}, {m["iface"]}u, {m["slot"]}u, {int(m.get("sret", False))}, '
                   f'{m.get("fn", "NULL")}, {why}}},')
    out.append('};')
    out.append(f'const size_t steam_method_count = {len(methods)};')
    return '\n'.join(out) + '\n'


def mel_steam_api():
    try:
        config = json.loads((ROOT / 'config/local.json').read_text())
        return Path(config['games']['mel']['linux_root']) / 'bin/libsteam_api.so'
    except (OSError, ValueError, KeyError):
        return None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--downloads', type=Path, default=DOWNLOADS,
                        help='folder holding the unpacked Steamworks.NET packages of SOURCES')
    parser.add_argument('--out', type=Path, default=ROOT / 'runtime/steam_generated.c')
    parser.add_argument('--mel-steam-api', type=Path,
                        help='libsteam_api.so of Portal Stories: Mel (bin/ of its Linux depot 317404); '
                             'default: linux_root of the "mel" game in config/local.json')
    args = parser.parse_args()
    sources = [(args.downloads / name / 'pkg', args.downloads / name / 'steam_api.bundle/Contents/MacOS/libsteam_api.dylib',
                versions) for name, versions in SOURCES]
    mel = args.mel_steam_api or mel_steam_api()
    if not mel or not Path(mel).is_file():
        raise SystemExit('libsteam_api.so of Portal Stories: Mel not found: pass --mel-steam-api '
                         '(download_depot 317400 317404, then <depot>/bin/libsteam_api.so)')
    guest_sources = [(args.downloads / SOURCES[0][0] / 'pkg', Path(mel), versions) for _, versions in GUEST_SOURCES]
    code, stats, methods = generate(sources, guest_sources)
    args.out.write_text(code)
    total = sum(s[0] for s in stats.values()); done = sum(s[1] for s in stats.values())
    for version, (n, ok, slots) in stats.items():
        print(f'{version:45} {ok:3}/{n:3} méthodes, {slots} emplacements')
    print(f'total : {done}/{total} méthodes marshallées', file=sys.stderr)


if __name__ == '__main__':
    main()
