// Round 24 bug 3: Play Online on iOS never spins forever (runtime/ios/online_notice.c).
#include "checks.h"
#include <string.h>
#include "../runtime/ios/online_notice.c"

RT_CHECK(online_notice_timeout) {
    rt_online_wait w = {0};
    assert(!rt_online_due(&w, 100, 8, 0));                 // nothing asked: nothing to say
    rt_online_asked(&w, 10);
    rt_online_asked(&w, 15);                               // asked again: the wait still runs from the first
    assert(!rt_online_due(&w, 17.9, 8, 0));                // still searching, not yet
    assert(rt_online_due(&w, 18, 8, 0));                   // 8 s: the notice
    assert(!rt_online_due(&w, 30, 8, 0));                  // once
    rt_online_asked(&w, 40);
    assert(!rt_online_due(&w, 50, 8, 1));                  // a level loaded (co-op started): no notice
    assert(!rt_online_due(&w, 60, 8, 0));
    assert(strstr(rt_online_message(), "Steam") && strstr(rt_online_message(), "Local Splitscreen"));
}
