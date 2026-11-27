// SDL calls the x86-64 runtime answers itself, before the Mac's SDL2 (which
// stands in for P2CE's Linux copy: native_libs64.h).
//
// SDL_ShowMessageBox: P2CE's libtier0 asks "Consent for Crash Dump Upload"
// (Sentry) at start-up. Our builds never upload anything (curl64.c refuses
// every network handle, and Sentry's crashpad_handler is a Linux program
// that doesn't run here), so the runtime answers that question "No" itself,
// every time, without showing it: the answer doesn't depend on any file in
// the copy-on-write game folder. Every other message box goes to SDL.
// SDL_MessageBoxData and SDL_MessageBoxButtonData have the same layout on
// Linux x86-64 and Apple arm64.
#include "rt64.h"
#include "native_libs64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct { uint32_t flags; int32_t buttonid; const char *text; } MessageBoxButton;
typedef struct {
    uint32_t flags; void *window; const char *title; const char *message;
    int32_t numbuttons; const MessageBoxButton *buttons; const void *colors;
} MessageBoxData;

enum { RETURNKEY_DEFAULT = 1, ESCAPEKEY_DEFAULT = 2 };

static int is_crash_consent(const MessageBoxData *d) {
    return (d->title && strstr(d->title, "Crash Dump Upload")) ||
           (d->message && strstr(d->message, "upload of crash dumps"));
}

// The button meaning "no": its text, else the one Escape picks.
int rt64_decline_button(const MessageBoxData *d, int32_t *id);
int rt64_decline_button(const MessageBoxData *d, int32_t *id) {
    static const char *const no[] = {"No", "Don't Upload", "Do not upload", "Never", "Decline", "Deny", NULL};
    for (int i = 0; i < d->numbuttons; ++i)
        for (int k = 0; no[k]; ++k)
            if (d->buttons[i].text && !strcasecmp(d->buttons[i].text, no[k])) { *id = d->buttons[i].buttonid; return 1; }
    for (int i = 0; i < d->numbuttons; ++i)
        if (d->buttons[i].flags & ESCAPEKEY_DEFAULT) { *id = d->buttons[i].buttonid; return 1; }
    return 0;
}

static void s_show_message_box(CPU64 *c) {
    const MessageBoxData *d = PTR(c, 0);
    int32_t *buttonid = PTR(c, 1);
    if (d && is_crash_consent(d)) {
        int32_t id;
        if (rt64_decline_button(d, &id)) {
            if (buttonid) *buttonid = id;
            fprintf(stderr, "rt64: answered \"%s\": No (crash dumps are never uploaded)\n", d->title ? d->title : "?");
            RET(c, 0);
            return;
        }
        // No button reads as "no": report the box as closed without an answer.
        if (buttonid) *buttonid = -1;
        fprintf(stderr, "rt64: closed \"%s\" without consent\n", d->title ? d->title : "?");
        RET(c, 0);
        return;
    }
    uint64_t native = rt64_native_symbol("SDL_ShowMessageBox");
    if (!native) { RET(c, (uint64_t)-1); return; }
    rt64_native_call(c, native);
}

static const RT64Shim shims[] = {
    {"SDL_ShowMessageBox", s_show_message_box}, {0, 0},
};
RT64_SHIMS(shims, NULL)
