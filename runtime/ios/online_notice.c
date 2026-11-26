// Round 24 bug 3: when to explain that Play Online can't work here (online_notice.h).
#include "online_notice.h"

void rt_online_asked(rt_online_wait *w, double now) {
    if (w->asked_at <= 0) w->asked_at = now > 0 ? now : 1e-9;   // the first request starts the wait
}
int rt_online_due(rt_online_wait *w, double now, double timeout, int in_level) {
    if (w->asked_at <= 0) return 0;
    if (in_level) { w->asked_at = 0; return 0; }
    if (timeout > 0 && now - w->asked_at < timeout) return 0;
    w->asked_at = 0;
    return 1;
}
const char *rt_online_title(void) { return "Online co-op needs Steam"; }
const char *rt_online_message(void) {
    return "Standard and Challenge Mode co-op find friends through Steam, which doesn't run on iPad or iPhone, "
           "so no one can be found from here.\n\n"
           "To play co-op locally:\n"
           "• With another iPad, iPhone or Mac running Sauce on the same Wi-Fi: Play nearby (button below), "
           "one device hosts, the other joins.\n"
           "• On this device alone: go Back and choose Local Splitscreen. Player 2 can use a controller, "
           "the touch controls, or a phone (gear menu > Phone controller).";
}
