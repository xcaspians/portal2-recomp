// Réglages iOS (runtime/ios/settings.c) : traduction en variables P2_*,
// priorité des variables déjà posées, concordance avec ios/Settings.bundle.
#include "checks.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../runtime/ios/settings.c"
#include "../runtime/ios/settings_menu.c"

typedef struct { const char *key, *value; } Pref;
static int fake_get(void *ctx, const char *key, char *out, size_t size) {
    for (const Pref *p = ctx; p->key; ++p)
        if (!strcmp(p->key, key)) { snprintf(out, size, "%s", p->value); return 1; }
    return 0;
}
static void clear_env(void) {
    for (int i = 0; i < rt_ios_settings_count; ++i) unsetenv(rt_ios_settings[i].env);
}

RT_CHECK(ios_settings_become_environment) {
    clear_env();
    Pref prefs[] = {{"P2ResolutionScale", "1.5"}, {"P2ShowFPS", "0"}, {"P2BackTap", "1"}, {"P2TouchControls", "auto"},
                    {"P2PortalButtons", "1"}, {"P2Log", "0"}, {"P2BackTapInvert", "1"}, {"P2Renderer", "d3d9"},
                    {"P2BackTapRecord", "1"}, {"P2Haptics", "0"}, {"P2MaxFPS", "40"}, {"P2PowerSave", "0"}, {"P2Upscale", "0.75"}, {NULL, NULL}};
    char summary[512];
    int n = rt_ios_settings_apply(fake_get, prefs, summary, sizeof(summary));
    assert(n == 11);
    assert(!strcmp(getenv("P2_IOS_UPSCALE"), "0.75"));
    assert(!strcmp(getenv("P2_IOS_MAX_FPS"), "40"));
    assert(!strcmp(getenv("P2_IOS_POWER_SAVE"), "0"));           // économie d'énergie automatique coupée
    assert(!strcmp(getenv("P2_HAPTICS"), "0"));                   // vibration au tir coupée
    assert(!strcmp(getenv("P2_BACKTAP_RAW"), "Documents/backtap/"));   // un fichier daté par lancement
    assert(!strcmp(getenv("P2_RENDERER"), "d3d9"));               // Rendu : Metal (src/main.c)
    assert(!strcmp(getenv("P2_IOS_RENDER_SCALE"), "1.5"));
    assert(!strcmp(getenv("P2_SHOW_FPS"), "0"));                 // interrupteur éteint
    assert(!strcmp(getenv("P2_BACKTAP"), "1"));
    assert(!strcmp(getenv("P2_BACKTAP_INVERT"), "1"));
    assert(!strcmp(getenv("P2_PORTAL_BUTTONS"), "1"));
    assert(!getenv("P2_TOUCH_CONTROLS"));                         // « auto » : défaut du code
    assert(!getenv("P2_LOG_FILE"));                               // journal éteint : aucune variable
    assert(!getenv("P2_TOUCH_LOOK"));                             // jamais touché
    assert(strstr(summary, "P2_IOS_RENDER_SCALE=1.5") && strstr(summary, "P2_SHOW_FPS=0"));
    clear_env();
}

// Une variable déjà posée (devicectl, bancs) prime sur le réglage.
RT_CHECK(ios_settings_yield_to_existing_environment) {
    clear_env();
    setenv("P2_IOS_RENDER_SCALE", "native", 1);
    Pref prefs[] = {{"P2ResolutionScale", "1"}, {"P2Log", "1"}, {NULL, NULL}};
    char summary[256];
    assert(rt_ios_settings_apply(fake_get, prefs, summary, sizeof(summary)) == 1);
    assert(!strcmp(getenv("P2_IOS_RENDER_SCALE"), "native"));
    assert(!strcmp(getenv("P2_LOG_FILE"), "Documents/game.log"));
    clear_env();
}

// Chaque réglage de la page iOS est lu par le code, et inversement
// (P2TouchTrackpad est lu directement par touch_controls.m).
RT_CHECK(ios_settings_match_settings_bundle) {
    FILE *f = fopen("ios/Settings.bundle/Root.plist", "r");
    assert(f);
    static char plist[65536];
    size_t n = fread(plist, 1, sizeof(plist) - 1, f);
    fclose(f);
    plist[n] = 0;
    for (int i = 0; i < rt_ios_settings_count; ++i) {
        char needle[128];
        snprintf(needle, sizeof(needle), "<string>%s</string>", rt_ios_settings[i].key);
        assert(strstr(plist, needle));
    }
    int keys = 0;
    for (const char *p = plist; (p = strstr(p, "<key>Key</key>")); ++p) {
        const char *v = strstr(p, "<string>"), *e = v ? strstr(v, "</string>") : NULL;
        assert(v && e);
        v += strlen("<string>");
        char key[64];
        snprintf(key, sizeof(key), "%.*s", (int)(e - v), v);
        int known = !strcmp(key, "P2TouchTrackpad");
        for (int i = 0; i < rt_ios_settings_count; ++i) known |= !strcmp(key, rt_ios_settings[i].key);
        assert(known);
        ++keys;
    }
    assert(keys == rt_ios_settings_count + 1);
}

// Choix d'une liste de la page Réglages : la valeur par défaut est celle que le
// code applique quand le réglage n'a jamais été touché, et les valeurs sont
// celles que le code sait lire.
static char settings_plist[65536];
static const char *settings_entry(const char *key, size_t *length) {
    if (!settings_plist[0]) {
        FILE *f = fopen("ios/Settings.bundle/Root.plist", "r");
        assert(f);
        settings_plist[fread(settings_plist, 1, sizeof(settings_plist) - 1, f)] = 0;
        fclose(f);
    }
    char needle[128];
    snprintf(needle, sizeof(needle), "<key>Key</key>\n\t\t\t<string>%s</string>", key);
    const char *at = strstr(settings_plist, needle);
    assert(at);
    const char *start = at;
    while (start > settings_plist && strncmp(start, "<dict>", 6)) --start;
    const char *end = strstr(at, "</dict>");
    assert(end);
    *length = (size_t)(end - start);
    return start;
}
static void assert_choices(const char *key, const char *def, const char *const *values) {
    size_t length;
    const char *entry = settings_entry(key, &length);
    char text[8192], needle[128];
    assert(length < sizeof(text));
    memcpy(text, entry, length); text[length] = 0;
    snprintf(needle, sizeof(needle), "<key>DefaultValue</key>\n\t\t\t<string>%s</string>", def);
    assert(strstr(text, needle));
    const char *list = strstr(text, "<key>Values</key>");
    assert(list);
    int n = 0;
    for (const char *p = list; (p = strstr(p, "<string>")); ++p) ++n;
    for (int i = 0; values[i]; ++i) {
        snprintf(needle, sizeof(needle), "<string>%s</string>", values[i]);
        assert(strstr(list, needle));
        --n;
    }
    assert(n == 0);   // ni plus ni moins
}
RT_CHECK(ios_settings_choices_match_code) {
    // P2_IOS_MAX_FPS : 60 par défaut (power_ios.m), 40 demande l'écran à 120 Hz (gl_ios.m).
    assert_choices("P2MaxFPS", "60", (const char *const[]){"30", "40", "60", "120", "auto", NULL});
    assert_choices("P2ResolutionScale", "2", (const char *const[]){"native", "2", "1.5", "1", NULL});
    assert_choices("P2Upscale", "1", (const char *const[]){"1", "0.75", "0.5", NULL});
    // P2_GYRO_SPEED : multiplicateur, 1 par défaut (gyro_aim.c).
    assert_choices("P2GyroSpeed", "1", (const char *const[]){"0.5", "0.75", "1", "1.5", "2", NULL});
}

// Agrandissement MetalFX : render.scale seulement pour une fraction de ]0, 1[.
RT_CHECK(ios_metal_keys_add_render_scale) {
    char keys[128];
    double scale = 0;
    rt_ios_metal_keys(keys, sizeof(keys), 60, "0.75", &scale);
    assert(!strcmp(keys, "present.maxFps=60;render.scale=0.75") && scale == 0.75);
    rt_ios_metal_keys(keys, sizeof(keys), 30, "0.5", &scale);
    assert(!strcmp(keys, "present.maxFps=30;render.scale=0.5"));
    const char *none[] = {"1", "", "0", "-0.5", "1.5", "abc", "0.75x"};
    for (size_t i = 0; i < sizeof(none) / sizeof(*none); ++i) {
        rt_ios_metal_keys(keys, sizeof(keys), 120, none[i], &scale);
        assert(!strcmp(keys, "present.maxFps=120") && scale == 1);
    }
    rt_ios_metal_keys(keys, sizeof(keys), 60, NULL, NULL);
    assert(!strcmp(keys, "present.maxFps=60"));
}

// P2_IOS_MAX_FPS=auto: the screen's maximum (ProMotion 120, else 60); numbers
// unchanged; nothing or garbage gives the 60 default.
RT_CHECK(ios_fps_auto_follows_the_screen) {
    assert(rt_ios_fps_choice("auto", 120) == 120);
    assert(rt_ios_fps_choice("auto", 60) == 60);
    assert(rt_ios_fps_choice("auto", 0) == 60);   // screen rate unknown
    assert(rt_ios_fps_choice("40", 120) == 40 && rt_ios_fps_choice("120", 60) == 120);
    assert(rt_ios_fps_choice(NULL, 120) == 60 && rt_ios_fps_choice("", 120) == 60);
    assert(rt_ios_fps_choice("fast", 120) == 60 && rt_ios_fps_choice("0", 120) == 60);
}

// In-game menu (settings_menu.c): every item is a Settings.bundle setting with
// the same default and the same choices or slider range, so the two pages
// stay in sync through the shared preferences.
static void assert_entry_has(const char *key, const char *fragment) {
    size_t length;
    const char *entry = settings_entry(key, &length);
    char text[8192];
    assert(length < sizeof(text));
    memcpy(text, entry, length); text[length] = 0;
    if (!strstr(text, fragment)) { fprintf(stderr, "%s: missing %s\n", key, fragment); assert(0); }
}
RT_CHECK(ios_menu_items_match_settings_page) {
    char fragment[256];
    for (int i = 0; i < rt_menu_item_count; ++i) {
        const rt_menu_item *it = &rt_menu_items[i];
        assert(rt_ios_setting_named(it->key));
        assert(rt_menu_item_named(it->key) == it);
        switch (it->kind) {
        case RT_MENU_CHOICE: assert_choices(it->key, it->def, it->values); break;
        case RT_MENU_TOGGLE:
            snprintf(fragment, sizeof(fragment), "<key>DefaultValue</key>\n\t\t\t<%s/>", strcmp(it->def, "0") ? "true" : "false");
            assert_entry_has(it->key, fragment);
            break;
        case RT_MENU_SLIDER:
            snprintf(fragment, sizeof(fragment), "<key>DefaultValue</key>\n\t\t\t<real>%s</real>", it->def);
            assert_entry_has(it->key, fragment);
            snprintf(fragment, sizeof(fragment), "<key>MinimumValue</key>\n\t\t\t<real>%g</real>", it->min);
            assert_entry_has(it->key, fragment);
            snprintf(fragment, sizeof(fragment), "<key>MaximumValue</key>\n\t\t\t<real>%g</real>", it->max);
            assert_entry_has(it->key, fragment);
            break;
        }
    }
    assert(!rt_menu_item_named("P2Log") && !rt_menu_item_named(NULL));
}

// Controller steps and shown values.
RT_CHECK(ios_menu_steps_and_display) {
    char out[64];
    const rt_menu_item *tap = rt_menu_item_named("P2BackTapSensitivity");
    assert(!strcmp(rt_menu_step(tap, "1", 1, out, sizeof(out)), "1.05"));
    assert(!strcmp(rt_menu_step(tap, "3", 1, out, sizeof(out)), "3"));         // clamped
    assert(!strcmp(rt_menu_step(tap, "0.5", -1, out, sizeof(out)), "0.5"));
    assert(!strcmp(rt_menu_step(tap, "1.33", 0, out, sizeof(out)), "1.35"));   // old High snaps to the step
    assert(!strcmp(rt_menu_step(tap, "loud", 0, out, sizeof(out)), "1"));      // unreadable: the default
    assert(!strcmp(rt_menu_step(tap, "9", 0, out, sizeof(out)), "3"));
    rt_menu_display(tap, "1.35", out, sizeof(out)); assert(!strcmp(out, "1.35"));
    const rt_menu_item *opacity = rt_menu_item_named("P2TouchOpacity");
    assert(!strcmp(rt_menu_step(opacity, "0.3", 1, out, sizeof(out)), "0.35"));
    assert(!strcmp(rt_menu_step(opacity, "0", -1, out, sizeof(out)), "0"));
    rt_menu_display(opacity, "0.75", out, sizeof(out)); assert(!strcmp(out, "75 %"));
    rt_menu_display(opacity, NULL, out, sizeof(out)); assert(!strcmp(out, "100 %"));
    const rt_menu_item *size = rt_menu_item_named("P2TouchSize");
    assert(!strcmp(rt_menu_step(size, "1.5", 1, out, sizeof(out)), "1.5"));
    assert(!strcmp(rt_menu_step(size, "0.75", -1, out, sizeof(out)), "0.75"));
    rt_menu_display(size, "1.25", out, sizeof(out)); assert(!strcmp(out, "125 %"));
    const rt_menu_item *fps = rt_menu_item_named("P2MaxFPS");
    assert(!(fps->flags & RT_MENU_RELAUNCH_ON_METAL));   // applies at once, Metal too (round 11)
    const rt_menu_item *counter = rt_menu_item_named("P2ShowFPS");
    assert(counter && counter->kind == RT_MENU_TOGGLE && !strcmp(counter->def, "0"));   // off by default
    assert(!strcmp(rt_menu_step(fps, "auto", -1, out, sizeof(out)), "120"));
    assert(!strcmp(rt_menu_step(fps, "auto", 1, out, sizeof(out)), "auto"));  // stops at the end
    assert(!strcmp(rt_menu_step(fps, "30", -1, out, sizeof(out)), "30"));
    assert(!strcmp(rt_menu_step(fps, "75", 0, out, sizeof(out)), "60"));      // unknown: the default
    rt_menu_display(fps, "auto", out, sizeof(out)); assert(!strcmp(out, "Auto"));
    const rt_menu_item *haptics = rt_menu_item_named("P2Haptics");
    assert(!strcmp(rt_menu_step(haptics, "1", 1, out, sizeof(out)), "0"));
    assert(!strcmp(rt_menu_step(haptics, "0", -1, out, sizeof(out)), "1"));
    assert(!strcmp(rt_menu_step(haptics, NULL, 1, out, sizeof(out)), "0"));   // default on, flipped
    rt_menu_display(haptics, "0", out, sizeof(out)); assert(!strcmp(out, "Off"));
}

// One setting's variable, as the in-game menu applies it live.
RT_CHECK(ios_setting_env_value_matches_launch) {
    assert(!strcmp(rt_ios_setting_env_value(rt_ios_setting_named("P2Haptics"), "0"), "0"));
    assert(!strcmp(rt_ios_setting_env_value(rt_ios_setting_named("P2Haptics"), "true"), "1"));
    assert(!rt_ios_setting_env_value(rt_ios_setting_named("P2PortalButtons"), "auto"));
    assert(!strcmp(rt_ios_setting_env_value(rt_ios_setting_named("P2PortalButtons"), "1"), "1"));
    assert(!strcmp(rt_ios_setting_env_value(rt_ios_setting_named("P2TouchOpacity"), "0.4"), "0.4"));
    assert(!strcmp(rt_ios_setting_env_value(rt_ios_setting_named("P2BackTapRecord"), "1"), "Documents/backtap/"));
    assert(!rt_ios_setting_env_value(rt_ios_setting_named("P2BackTapRecord"), "0"));
}

// Controller around the in-game menu (audit, round 4): the options button
// stays away from the game's pause menu, and a stick's return to centre
// reaches the game even while the menu holds the controller.
RT_CHECK(ios_menu_pad_filter_lets_releases_and_centring_through) {
    // menu open (captured), game paused
    assert(rt_menu_pad_dropped(RT_PAD_BUTTON_DOWN, 0, 0, 1, 1));
    assert(!rt_menu_pad_dropped(RT_PAD_BUTTON_UP, 0, 0, 1, 1));         // releases pass
    assert(rt_menu_pad_dropped(RT_PAD_AXIS, 0, 20000, 1, 1));
    assert(!rt_menu_pad_dropped(RT_PAD_AXIS, 0, 0, 1, 1));               // back to centre: passes
    assert(!rt_menu_pad_dropped(RT_PAD_AXIS, 0, -3000, 1, 1));
    assert(rt_menu_pad_dropped(RT_PAD_OTHER_MOTION, 0, 0, 1, 1));
    // menu closed, pause menu open: only the options button is ours
    assert(rt_menu_pad_dropped(RT_PAD_BUTTON_DOWN, 1, 0, 1, 0) && rt_menu_pad_dropped(RT_PAD_BUTTON_UP, 1, 0, 1, 0));
    assert(!rt_menu_pad_dropped(RT_PAD_BUTTON_DOWN, 0, 0, 1, 0) && !rt_menu_pad_dropped(RT_PAD_AXIS, 0, 20000, 1, 0));
    // gameplay: the options button is the game's
    assert(!rt_menu_pad_dropped(RT_PAD_BUTTON_DOWN, 1, 0, 0, 0));
    assert(!rt_menu_pad_dropped(RT_PAD_NOT_PAD, 0, 0, 1, 1));
}

// Half-Life 2's menu (round 8): no back-tap, portal-button or firing-vibration
// items (its Settings page has none), gyro aiming on by default.
RT_CHECK(ios_menu_items_per_game) {
    int hidden = 0;
    for (int i = 0; i < rt_menu_item_count; ++i) {
        const rt_menu_item *it = &rt_menu_items[i];
        assert(rt_menu_item_for_game(it, 1));                        // Portal 2 and Mel: everything
        if (!rt_menu_item_for_game(it, 0)) {
            ++hidden;
            assert(!strncmp(it->key, "P2BackTap", 9) || !strcmp(it->key, "P2PortalButtons") || !strcmp(it->key, "P2Haptics"));
        }
    }
    assert(hidden >= 5);                                             // switch, sensitivity, swap, vibration, portal buttons
    assert(rt_menu_item_for_game(rt_menu_item_named("P2Gyro"), 0) && rt_menu_item_for_game(rt_menu_item_named("P2MaxFPS"), 0));
    const rt_menu_item *gyro = rt_menu_item_named("P2Gyro");
    assert(!strcmp(rt_menu_item_default(gyro, "hl2"), "1") && !strcmp(rt_menu_item_default(gyro, "portal2"), "0"));
    assert(!strcmp(rt_menu_item_default(gyro, "l4d2"), "1"));   // Left 4 Dead 2 aims like Half-Life 2
    assert(!strcmp(rt_menu_item_default(rt_menu_item_named("P2TouchSize"), "hl2"), "1"));
}

// Frames per second: Auto asks the display for 120 on a 120 Hz screen and 60
// on a 60 Hz one; the setting applies live (round 11).
RT_CHECK(ios_frame_rate_auto_range_and_live_pacing) {
    float lo, hi, pref;
    rt_ios_display_range(rt_ios_fps_choice("auto", 120), 120, &lo, &hi, &pref);
    assert(lo == 120 && hi == 120 && pref == 120);
    rt_ios_display_range(rt_ios_fps_choice("auto", 60), 60, &lo, &hi, &pref);
    assert(lo == 60 && hi == 60 && pref == 60);
    rt_ios_display_range(rt_ios_fps_choice("120", 60), 60, &lo, &hi, &pref);   // 120 on a 60 Hz screen: 60
    assert(pref == 60);
    rt_ios_display_range(60, 120, &lo, &hi, &pref); assert(pref == 60);
    rt_ios_display_range(40, 120, &lo, &hi, &pref); assert(pref == 120);       // 40: every third refresh
    rt_ios_display_range(30, 120, &lo, &hi, &pref); assert(pref == 30);
    // Metal starts at the screen's maximum, so Auto can be chosen later without a restart.
    assert(rt_ios_metal_launch_cap(120) == 120 && rt_ios_metal_launch_cap(60) == 60);
    // Pacing at 60 under a 120 cap: a game drawing every 5 ms presents every 16.7 ms.
    double last = 0, t = 100, prev = 0;
    int frames = 0;
    for (int i = 0; i < 200; ++i) {
        double until = rt_ios_pace(t, &last, 60, 120);
        if (until > t) t = until;
        if (prev > 0) assert(t - prev > 1.0 / 60 - 1e-9);
        prev = t; ++frames;
        t += 0.005;
    }
    assert(fabs((t - 100) / frames - 1.0 / 60) < 0.001);
    // At or over Metal's cap: nothing to do (Metal paces itself).
    last = 5; assert(rt_ios_pace(200, &last, 120, 120) == 0 && last == 0);
    // A long hitch: no burst of frames to catch up.
    last = 0; rt_ios_pace(300, &last, 30, 120);
    assert(rt_ios_pace(302, &last, 30, 120) == 0 && rt_ios_pace(302.001, &last, 30, 120) > 302.03);
}

// Round 25: the controls panel opened from the launcher (no game yet) shows only
// settings every game shares: the same preference key whichever game runs, so a
// change there applies in every game.
#include "../runtime/ios/game_ios.h"
RT_CHECK(ios_settings_from_the_launcher_are_shared) {
    static const char *const games[] = {"portal2", "mel", "hl2", "portal1", "l4d2"};
    int shown = 0;
    for (int i = 0; i < rt_menu_item_count; ++i) {
        const rt_menu_item *it = &rt_menu_items[i];
        if (!rt_menu_item_in_launcher(it)) continue;
        ++shown;
        char first[160], other[160];
        rt_ios_set_game(games[0]);
        rt_ios_pref_key(it->key, first, sizeof(first));
        for (size_t g = 1; g < sizeof(games) / sizeof(*games); ++g) {
            rt_ios_set_game(games[g]);
            assert(!strcmp(rt_ios_pref_key(it->key, other, sizeof(other)), first));
        }
    }
    rt_ios_set_game("portal2");
    assert(shown == rt_menu_item_count - 1);   // all but iCloud saves
    assert(!rt_menu_item_in_launcher(rt_menu_item_named("P2ICloudSaves")));
    assert(rt_menu_item_in_launcher(rt_menu_item_named("P2TouchSize")));
}
