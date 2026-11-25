// iCloud save sync rules (see save_sync_rules.h).
#include "save_sync_rules.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

int rt_sync_same(rt_sync_file a, rt_sync_file b) {
    if (!a.present || !b.present) return a.present == b.present;
    long long gap = a.mtime > b.mtime ? a.mtime - b.mtime : b.mtime - a.mtime;
    return a.size == b.size && gap <= 2;
}

rt_sync_action rt_sync_decide(rt_sync_file local, rt_sync_file remote, const rt_sync_record *r) {
    int known = r && r->known;
    if (!local.present && !remote.present) return RT_SYNC_NOTHING;
    if (local.present && !remote.present) {
        // Deleted in iCloud since the last sync, and unchanged here: the deletion stays there.
        if (known && r->remote.present && rt_sync_same(local, r->local)) return RT_SYNC_NOTHING;
        return RT_SYNC_UPLOAD;
    }
    if (!local.present) {
        if (known && r->local.present && rt_sync_same(remote, r->remote)) return RT_SYNC_NOTHING;
        return RT_SYNC_DOWNLOAD;
    }
    if (rt_sync_same(local, remote)) return RT_SYNC_NOTHING;
    if (!known) return RT_SYNC_CONFLICT;   // two different saves of the same name, never synced
    int here = !rt_sync_same(local, r->local), there = !rt_sync_same(remote, r->remote);
    if (here && !there) return RT_SYNC_UPLOAD;
    if (there && !here) return RT_SYNC_DOWNLOAD;
    return RT_SYNC_CONFLICT;
}

rt_sync_record rt_sync_after(rt_sync_action action, int copied, rt_sync_record before, rt_sync_file local,
                             rt_sync_file remote) {
    if (action != RT_SYNC_NOTHING && !copied) return before;
    rt_sync_record r = {1, local, remote};
    if (before.known && !local.present && before.local.present) r.local = before.local;
    if (before.known && !remote.present && before.remote.present) r.remote = before.remote;
    return r;
}

void rt_sync_conflict_name(const char *name, const char *device, const struct tm *when, char *out, size_t size) {
    char tag[25];
    size_t n = 0;
    int dash = 0;
    for (const char *p = device ? device : ""; *p && n < sizeof(tag) - 1; ++p) {
        unsigned char c = (unsigned char)*p;
        if (isalnum(c) && c < 128) { tag[n++] = (char)c; dash = 0; }
        else if (c == '\'' || c >= 128) continue;   // "Caspian’s" -> "Caspians"
        else if (n && !dash) { tag[n++] = '-'; dash = 1; }
    }
    while (n && tag[n - 1] == '-') --n;
    tag[n] = 0;
    if (!n) snprintf(tag, sizeof(tag), "other-device");
    const char *dot = strrchr(name, '.');
    int base = dot ? (int)(dot - name) : (int)strlen(name);
    snprintf(out, size, "%.*s_%s_%04d-%02d-%02d_%02d%02d%s", base, name, tag, when->tm_year + 1900, when->tm_mon + 1,
             when->tm_mday, when->tm_hour, when->tm_min, dot ? dot : "");
}

int rt_sync_wanted(const char *name) {
    size_t n = strlen(name);
    if (n < 5 || name[0] == '.') return 0;
    return !strcasecmp(name + n - 4, ".sav") || !strcasecmp(name + n - 4, ".tga");
}
