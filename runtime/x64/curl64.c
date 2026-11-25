// libcurl as P2CE's libtier0 uses it (crash reports and telemetry through
// Sentry): a lifted program makes no network connections, so every handle
// is refused. curl_global_init succeeds; curl_easy_init returns NULL, which
// the callers treat as "no transport"; the rest report CURLE_FAILED_INIT.
#include "rt64.h"

enum { CURLE_OK = 0, CURLE_FAILED_INIT = 2 };

static void s_ok(CPU64 *c) { RET(c, CURLE_OK); }
static void s_null(CPU64 *c) { RET(c, 0); }
static void s_failed(CPU64 *c) { RET(c, CURLE_FAILED_INIT); }
static void s_nothing(CPU64 *c) { (void)c; }
static void s_strerror(CPU64 *c) { RET(c, (uintptr_t)"network access is disabled in the lifted program"); }
// curl_version_info_data: age, version, version_num, host, features, ...
static uint64_t version_info[32];
static void s_version_info(CPU64 *c) {
    version_info[1] = (uintptr_t)"7.88.1-rt64";
    version_info[2] = 0x075801;
    version_info[3] = (uintptr_t)"x86_64-pc-linux-gnu";
    RET(c, (uintptr_t)version_info);
}

static const RT64Shim shims[] = {
    {"curl_global_init", s_ok}, {"curl_global_cleanup", s_nothing}, {"curl_easy_init", s_null},
    {"curl_easy_cleanup", s_nothing}, {"curl_easy_reset", s_nothing}, {"curl_easy_setopt", s_failed},
    {"curl_easy_perform", s_failed}, {"curl_easy_getinfo", s_failed}, {"curl_easy_strerror", s_strerror},
    {"curl_slist_append", s_null}, {"curl_slist_free_all", s_nothing}, {"curl_version_info", s_version_info},
    {NULL, NULL},
};
RT64_SHIMS(shims, NULL)
