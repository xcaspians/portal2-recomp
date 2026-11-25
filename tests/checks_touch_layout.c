// Jeu du build iOS (runtime/ios/game_ios.c) et dispositions des commandes
// tactiles (runtime/ios/touch_layout.c) : profil choisi par la clé P2Game,
// boutons dans la zone sûre, sans chevauchement, hors du coin du compteur
// d'images et du stick gauche, Portal 2 inchangé.
#include "checks.h"
#include <math.h>
#include <string.h>
#include "../runtime/ios/game_ios.c"
#include "../runtime/ios/touch_layout.c"

RT_CHECK(ios_game_profile_from_plist_key) {
    const rt_ios_game *hl2 = rt_ios_game_named("hl2"), *p2 = rt_ios_game_named("portal2");
    // Portal 1 : Source 2013 comme HL2 (même ICvar), mais les deux boutons de
    // portail et les tapes au dos de Portal 2.
    const rt_ios_game *p1 = rt_ios_game_named("portal1");
    assert(p1 != p2 && !strcmp(p1->mod, "portal") && !strcmp(p1->app_id, "400"));
    assert(!strcmp(p1->userdata, "portal-userdata") && p1->layout == RT_IOS_LAYOUT_PORTAL2);
    assert(p1->back_taps && !strcmp(p1->cvar_interface, "VEngineCvar004") && p1->find_command_base == 10);
    assert(!strcmp(hl2->mod, "hl2_complete") && !strcmp(hl2->app_id, "220"));
    assert(!strcmp(hl2->userdata, "hl2-userdata") && hl2->layout == RT_IOS_LAYOUT_HL2);
    assert(!hl2->back_taps && !strcmp(hl2->cvar_interface, "VEngineCvar004") && hl2->find_command_base == 10);
    // Portal 2 : dossier des installations existantes, tapes au dos, ICvar connu.
    assert(!strcmp(p2->userdata, "portal2-userdata") && !strcmp(p2->app_id, "620") && p2->back_taps);
    assert(!strcmp(p2->cvar_interface, "VEngineCvar007") && p2->find_command_base == 13);
    // Portal Stories: Mel: its own saves and Steam app, Portal 2's controls and engine interfaces.
    const rt_ios_game *mel = rt_ios_game_named("mel");
    assert(mel != p2 && !strcmp(mel->mod, "portal_stories") && !strcmp(mel->app_id, "317400"));
    assert(!strcmp(mel->userdata, "mel-userdata") && mel->layout == RT_IOS_LAYOUT_PORTAL2 && mel->back_taps);
    assert(!strcmp(mel->cvar_interface, "VEngineCvar007") && mel->find_command_base == 13);
    // Builds d'avant la clé P2Game, ou valeur inconnue : Portal 2.
    assert(rt_ios_game_named(NULL) == p2 && rt_ios_game_named("") == p2 && rt_ios_game_named("ep2") == p2);
    assert(rt_ios_current_game() == p2);   // hors iOS
}

// Zones sûres en paysage (points) : iPhone SE, iPhone 15 Pro Max, iPad 11".
static const struct { double x, y, w, h; } areas[] = {
    {0, 0, 667, 375}, {59, 0, 932 - 118, 430 - 21}, {0, 0, 1194, 834 - 20}};

RT_CHECK(ios_touch_layout_fits_the_screen) {
    for (int layout = RT_IOS_LAYOUT_PORTAL2; layout <= RT_IOS_LAYOUT_HL2; ++layout) {
        int n;
        const rt_touch_button *b = rt_touch_layout((rt_ios_layout)layout, &n);
        assert(n > 0 && n <= 16);   // MAX_BUTTONS de touch_controls.m
        for (size_t a = 0; a < sizeof(areas) / sizeof(*areas); ++a) {
            double x = areas[a].x, y = areas[a].y, w = areas[a].w, h = areas[a].h;
            // Coin du compteur d'images (fpsCorner de touch_controls.m, « OPENGL »/« 000 FPS ») :
            // un toucher y bascule le compteur avant tout bouton.
            double corner_w = x + 8 + (7 * 6 - 1) * 2 + 8 + 20, corner_h = y + 8 + 16 * 2 + 8 + 20;
            // Stick gauche au repos (restingStick: et stickRadius de touch_controls.m).
            double stick_x = x + w * .16, stick_y = y + h * .72, stick_r = fmin(66, h * .15);
            double look_x = x + w * rt_touch_look_rest((rt_ios_layout)layout);
            for (int i = 0; i < n; ++i) {
                double cx, cy, r = b[i].size / 2;
                rt_touch_place(&b[i], x, y, w, h, &cx, &cy);
                assert(cx - r >= x - .5 && cx + r <= x + w + .5 && cy - r >= y - .5 && cy + r <= y + h + .5);
                assert(!(cx < corner_w && cy < corner_h));
                if (layout == RT_IOS_LAYOUT_HL2) assert(!(cx - r < corner_w && cy - r < corner_h));   // pas même le bord
                assert(hypot(cx - stick_x, cy - stick_y) > stick_r + r);
                // HL2 : aucun bouton sur le stick de la caméra au repos (Portal 2 :
                // le saut et l'accroupi y touchent, disposition d'origine).
                if (layout == RT_IOS_LAYOUT_HL2) assert(hypot(cx - look_x, cy - stick_y) > stick_r + r);
                for (int j = 0; j < i; ++j) {
                    // Portal 2 sur iPhone SE : le portail bleu de droite touche la
                    // pause (disposition d'origine ; ses boutons ne s'affichent
                    // que sans tapes au dos).
                    if (layout == RT_IOS_LAYOUT_PORTAL2 && a == 0 && ((b[i].flags | b[j].flags) & RT_TOUCH_PORTAL)) continue;
                    double dx, dy;
                    rt_touch_place(&b[j], x, y, w, h, &dx, &dy);
                    assert(hypot(cx - dx, cy - dy) >= r + b[j].size / 2 + 4);
                }
            }
        }
    }
}

RT_CHECK(ios_touch_layout_hl2_has_every_command) {
    int n, seen[RT_TOUCH_ACTION_COUNT] = {0};
    const rt_touch_button *b = rt_touch_layout(RT_IOS_LAYOUT_HL2, &n);
    for (int i = 0; i < n; ++i) {
        ++seen[b[i].action];
        assert(b[i].symbol && !(b[i].flags & (RT_TOUCH_PORTAL | RT_TOUCH_DATASET)));   // tout est toujours affiché
    }
    static const rt_touch_action wanted[] = {
        RT_TOUCH_FIRE, RT_TOUCH_ALT_FIRE, RT_TOUCH_RELOAD, RT_TOUCH_USE, RT_TOUCH_JUMP, RT_TOUCH_CROUCH,
        RT_TOUCH_SPRINT, RT_TOUCH_FLASHLIGHT, RT_TOUCH_WEAPON_PREV, RT_TOUCH_WEAPON_NEXT, RT_TOUCH_PAUSE,
        RT_TOUCH_QUICKSAVE, RT_TOUCH_LOOK_MODE};
    for (size_t i = 0; i < sizeof(wanted) / sizeof(*wanted); ++i) assert(seen[wanted[i]] == 1);
    assert(!seen[RT_TOUCH_FLAG_ERROR]);
    // Sprint et accroupi en bascule ; les tirs visent en glissant.
    for (int i = 0; i < n; ++i) {
        int toggle = b[i].action == RT_TOUCH_SPRINT || b[i].action == RT_TOUCH_CROUCH;
        int aim = b[i].action == RT_TOUCH_FIRE || b[i].action == RT_TOUCH_ALT_FIRE;
        assert(!!(b[i].flags & RT_TOUCH_TOGGLE) == toggle && !!(b[i].flags & RT_TOUCH_AIM) == aim);
    }
}

// Portal 2 : mêmes centres que les formules d'avant les dispositions par jeu.
RT_CHECK(ios_touch_layout_portal2_unchanged) {
    int n;
    const rt_touch_button *b = rt_touch_layout(RT_IOS_LAYOUT_PORTAL2, &n);
    double x = 59, y = 0, w = 814, h = 409, k = 54, top = y + h * .19, gap = 60 * 1.15;
    const struct { rt_touch_action action; double cx, cy, size; unsigned flags; } old[] = {
        {RT_TOUCH_FIRE, x + w * .06, top, 60, RT_TOUCH_PORTAL}, {RT_TOUCH_ALT_FIRE, x + w * .06 + gap, top, 60, RT_TOUCH_PORTAL},
        {RT_TOUCH_FIRE, x + w * .94, top, 60, RT_TOUCH_PORTAL}, {RT_TOUCH_ALT_FIRE, x + w * .94 - gap, top, 60, RT_TOUCH_PORTAL},
        {RT_TOUCH_JUMP, x + w * .94, y + h * .50, k, 0}, {RT_TOUCH_CROUCH, x + w * .80, y + h * .40, k, 0},
        {RT_TOUCH_USE, x + w * .94, y + h * .80, k, 0}, {RT_TOUCH_PAUSE, x + w - k * .45, y + k * .45, k * .75, 0},
        {RT_TOUCH_FLAG_ERROR, x + w - k * 1.35, y + k * .45, k * .75, RT_TOUCH_DATASET},
        {RT_TOUCH_LOOK_MODE, x + k * .45, y + h - k * .45, k * .75, 0}};
    assert(n == (int)(sizeof(old) / sizeof(*old)));
    for (int i = 0; i < n; ++i) {
        double cx, cy;
        rt_touch_place(&b[i], x, y, w, h, &cx, &cy);
        assert(b[i].action == old[i].action && b[i].flags == old[i].flags);
        assert(fabs(cx - old[i].cx) < .01 && fabs(cy - old[i].cy) < .01 && fabs(b[i].size - old[i].size) < .01);
    }
}

// Overlay hidden for a controller (rt_touch_overlay_update): a touch shows it,
// the next controller input hides it again; "1" never hides it; "pad" keeps it
// off while a controller is connected, touches included.
RT_CHECK(ios_touch_overlay_hides_on_controller_input) {
    rt_overlay_state s = {0};
    // Automatic: hidden when the controller connects, shown by a touch.
    rt_touch_overlay_update(&s, RT_OVERLAY_DEVICES, 1, 0, 0, NULL);
    assert(s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_TOUCH, 1, 0, 0, "auto");
    assert(!s.concealed);
    // No controller input yet: stays shown, frame after frame.
    for (int i = 0; i < 3; ++i) rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 1, 0, 0, "auto");
    assert(!s.concealed);
    // Back on the controller: hidden again (the bug was that nothing did this).
    rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 1, 0, 1, "auto");
    assert(s.concealed);
    // A button held through the touch is not new input; releasing then pressing is.
    rt_touch_overlay_update(&s, RT_OVERLAY_TOUCH, 1, 0, 1, "auto");
    assert(!s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 1, 0, 1, "auto");
    assert(!s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 1, 0, 0, "auto");
    rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 1, 0, 1, "auto");
    assert(s.concealed);
    // Controller gone: shown, and input polling cannot hide it.
    rt_touch_overlay_update(&s, RT_OVERLAY_DEVICES, 0, 0, 0, "auto");
    assert(!s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 0, 0, 1, "auto");
    assert(!s.concealed);
    // A keyboard alone hides it until a touch, as before.
    rt_touch_overlay_update(&s, RT_OVERLAY_DEVICES, 0, 1, 0, "auto");
    assert(s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_TOUCH, 0, 1, 0, "auto");
    assert(!s.concealed);
    // "1": never hidden.
    rt_touch_overlay_update(&s, RT_OVERLAY_DEVICES, 1, 0, 0, "1");
    assert(!s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_PAD_POLL, 1, 0, 1, "1");
    assert(!s.concealed);
    // "pad": off while a controller is connected, a touch does not bring it back.
    s = (rt_overlay_state){0};
    rt_touch_overlay_update(&s, RT_OVERLAY_DEVICES, 1, 0, 0, "pad");
    assert(s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_TOUCH, 1, 0, 0, "pad");
    assert(s.concealed);
    // ...and comes back once the controller disconnects.
    rt_touch_overlay_update(&s, RT_OVERLAY_DEVICES, 0, 0, 0, "pad");
    assert(!s.concealed);
    rt_touch_overlay_update(&s, RT_OVERLAY_TOUCH, 0, 0, 0, "pad");
    assert(!s.concealed);
}

// Button size setting (P2_TOUCH_SIZE, 75 % to 150 %): every button of every
// layout stays on screen on the three screens above; at 100 % the placement
// is unchanged.
RT_CHECK(ios_touch_layout_scaled_stays_on_screen) {
    static const double scales[] = {.75, 1, 1.25, 1.5};
    for (int layout = RT_IOS_LAYOUT_PORTAL2; layout <= RT_IOS_LAYOUT_HL2; ++layout) {
        int n;
        const rt_touch_button *b = rt_touch_layout((rt_ios_layout)layout, &n);
        for (size_t a = 0; a < sizeof(areas) / sizeof(*areas); ++a) {
            double x = areas[a].x, y = areas[a].y, w = areas[a].w, h = areas[a].h;
            for (size_t k = 0; k < sizeof(scales) / sizeof(*scales); ++k)
                for (int i = 0; i < n; ++i) {
                    double cx, cy, r = b[i].size * scales[k] / 2;
                    rt_touch_place_scaled(&b[i], scales[k], x, y, w, h, &cx, &cy);
                    assert(cx - r >= x - .01 && cx + r <= x + w + .01 && cy - r >= y - .01 && cy + r <= y + h + .01);
                    if (scales[k] == 1) {
                        double ux, uy;
                        rt_touch_place(&b[i], x, y, w, h, &ux, &uy);
                        assert(fabs(ux - cx) < 1e-9 && fabs(uy - cy) < 1e-9);
                    }
                }
        }
    }
}

// One Portal app, several games: Mel's data under Documents/mel, its own
// preference keys; Portal 2 keeps its historical folders and keys.
RT_CHECK(ios_hosted_game_data_and_preference_keys) {
    const rt_ios_game *p2 = rt_ios_game_named("portal2"), *mel = rt_ios_game_named("mel");
    assert(!strcmp(p2->data_dir, "") && !strcmp(mel->data_dir, "mel"));
    assert(!strcmp(p2->title, "Portal 2") && !strcmp(mel->title, "Portal Stories: Mel"));
    char key[64];
    assert(rt_ios_set_game("portal2") == p2 && !strcmp(rt_ios_pref_key("P2TouchSize", key, sizeof(key)), "P2TouchSize"));
    // Settings are shared by every game (round 11): Mel reads and writes Portal 2's keys...
    assert(rt_ios_set_game("mel") == mel && !strcmp(rt_ios_pref_key("P2TouchSize", key, sizeof(key)), "P2TouchSize"));
    static const char *const shared[] = {"P2TouchLayout.pad", "P2TouchLayout.phone", "P2TouchOpacity", "P2BackTap",
        "P2BackTapSensitivity", "P2Gyro", "P2GyroSpeed", "P2TouchLook", "P2MaxFPS", "P2ShowFPS", "P2DefaultsFor"};
    for (size_t i = 0; i < sizeof(shared) / sizeof(*shared); ++i)
        assert(!strcmp(rt_ios_pref_key(shared[i], key, sizeof(key)), shared[i]));
    // ...except what only means something to one game: its iCloud saves.
    assert(!strcmp(rt_ios_pref_key("P2ICloudSaves", key, sizeof(key)), "mel.P2ICloudSaves"));
    rt_ios_set_game("portal2");
    assert(!strcmp(rt_ios_pref_key("P2ICloudSaves", key, sizeof(key)), "P2ICloudSaves"));
}

// The layout is the same in every game: changed in one, the other reads it;
// a button only one game has keeps its place when the other game saves.
RT_CHECK(touch_layout_shared_between_games) {
    char key_p2[64], key_mel[64], text[1024];
    rt_ios_set_game("portal2"); rt_ios_pref_key("P2TouchLayout.pad", key_p2, sizeof(key_p2));
    rt_ios_set_game("mel"); rt_ios_pref_key("P2TouchLayout.pad", key_mel, sizeof(key_mel));
    rt_ios_set_game("portal2");
    assert(!strcmp(key_p2, key_mel));                       // one stored layout
    // Portal 2 moves jump; HL2's layout (flashlight, reload) saved into the same text keeps it.
    rt_touch_custom_layout stored = {0}, edited = {0};
    edited.b[RT_TOUCH_JUMP] = (rt_touch_custom){1, 0.8f, 0.4f, 1.2f};
    rt_touch_custom_merge(&stored, &edited, RT_IOS_LAYOUT_PORTAL2);
    assert(rt_touch_custom_format(&stored, text, sizeof(text)));
    rt_touch_custom_layout back;
    assert(rt_touch_custom_parse(text, &back) == 1 && back.b[RT_TOUCH_JUMP].set);
    rt_touch_custom_layout hl2 = {0};
    hl2.b[RT_TOUCH_FLASHLIGHT] = (rt_touch_custom){1, 0.5f, 0.2f, 1};
    int count = 0, has_jump = 0, has_flash = 0;
    const rt_touch_button *b = rt_touch_layout(RT_IOS_LAYOUT_HL2, &count);
    for (int i = 0; i < count; ++i) { has_jump |= b[i].action == RT_TOUCH_JUMP; has_flash |= b[i].action == RT_TOUCH_FLASHLIGHT; }
    hl2.b[RT_TOUCH_JUMP] = back.b[RT_TOUCH_JUMP];   // the editor starts from the stored text
    rt_touch_custom_merge(&back, &hl2, RT_IOS_LAYOUT_HL2);
    assert(back.b[RT_TOUCH_JUMP].set && fabsf(back.b[RT_TOUCH_JUMP].fx - 0.8f) < 1e-6f);
    assert(!has_flash || back.b[RT_TOUCH_FLASHLIGHT].set);
    // A game without a button: saving resets nothing of it.
    rt_touch_custom_layout reset = {0};
    rt_touch_custom_layout before = back;
    rt_touch_custom_merge(&back, &reset, RT_IOS_LAYOUT_PORTAL2);
    assert(!back.b[RT_TOUCH_JUMP].set);                    // Portal 2 has jump: its reset applies
    int p2count = 0, p2_flash = 0;
    const rt_touch_button *pb = rt_touch_layout(RT_IOS_LAYOUT_PORTAL2, &p2count);
    for (int i = 0; i < p2count; ++i) p2_flash |= pb[i].action == RT_TOUCH_FLASHLIGHT;
    if (!p2_flash) assert(back.b[RT_TOUCH_FLASHLIGHT].set == before.b[RT_TOUCH_FLASHLIGHT].set);
    (void)has_jump;
}

// ---- Button layout editor (rt_touch_custom_*) ----
RT_CHECK(touch_editor_clamp_keeps_buttons_on_screen_and_off_the_sticks) {
    double w = 852, h = 393, stick[2][2];   // an iPhone's safe area, landscape
    rt_touch_stick_rests(RT_IOS_LAYOUT_PORTAL2, stick);
    double r_stick = 59;
    // Dragged past the corner: back inside, whole.
    double cx = -40, cy = 900;
    rt_touch_custom_clamp(&cx, &cy, 60, w, h, stick, 2, r_stick, 8);
    assert(cx >= 30 && cy <= h - 30);
    // Dropped on the move stick's rest: pushed out of it.
    cx = stick[0][0] * w; cy = stick[0][1] * h;
    rt_touch_custom_clamp(&cx, &cy, 60, w, h, stick, 2, r_stick, 8);
    assert(hypot(cx - stick[0][0] * w, cy - stick[0][1] * h) >= r_stick * .8 + 30 - 1e-6);
    // Grid: 8 points; a spot far from everything stays where snapped.
    cx = 403; cy = 101;
    rt_touch_custom_clamp(&cx, &cy, 60, w, h, stick, 2, r_stick, 8);
    assert(cx == 400 && cy == 104);
}

RT_CHECK(touch_editor_layouts_save_and_load) {
    rt_touch_custom_layout l, back;
    memset(&l, 0, sizeof(l));
    l.b[RT_TOUCH_JUMP] = (rt_touch_custom){1, .93f, .60f, 1.1f};
    l.b[RT_TOUCH_USE] = (rt_touch_custom){1, .50f, .95f, .8f};
    char text[512];
    assert(rt_touch_custom_format(&l, text, sizeof(text)));
    assert(!strcmp(text, "v1 jump=0.930,0.600,1.10 use=0.500,0.950,0.80"));
    assert(rt_touch_custom_parse(text, &back) == 2);
    assert(back.b[RT_TOUCH_JUMP].set && fabsf(back.b[RT_TOUCH_JUMP].fx - .93f) < 1e-3f && !back.b[RT_TOUCH_CROUCH].set);
    // Damaged or unknown entries are skipped, the rest kept; no text: nothing custom.
    assert(rt_touch_custom_parse("v1 jump=2,0.5,1 use=0.5,0.5,1.0 laser=0.1,0.1,1", &back) == 1 && back.b[RT_TOUCH_USE].set);
    assert(!back.b[RT_TOUCH_JUMP].set);
    assert(!rt_touch_custom_parse(NULL, &back) && !rt_touch_custom_parse("v2 jump=0.5,0.5,1", &back));
    // Placed with the Button size slider on top, inside the area.
    int n;
    const rt_touch_button *b = rt_touch_layout(RT_IOS_LAYOUT_PORTAL2, &n);
    const rt_touch_button *jump = NULL;
    for (int i = 0; i < n; ++i) if (b[i].action == RT_TOUCH_JUMP) jump = &b[i];
    assert(jump);
    double cx, cy, size;
    rt_touch_place_custom(jump, &l.b[RT_TOUCH_JUMP], 1.2, 10, 20, 800, 400, &cx, &cy, &size);
    assert(fabs(size - jump->size * 1.1 * 1.2) < 1e-3 && cx <= 10 + 800 - size / 2 && fabs(cy - (20 + 400 * .60)) < 1e-3);
    rt_touch_place_custom(jump, NULL, 1.0, 10, 20, 800, 400, &cx, &cy, &size);   // no entry: the built-in place
    double ex, ey;
    rt_touch_place_scaled(jump, 1.0, 10, 20, 800, 400, &ex, &ey);
    assert(cx == ex && cy == ey && size == jump->size);
}
