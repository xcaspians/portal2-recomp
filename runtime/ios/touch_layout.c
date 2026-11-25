// Dispositions des commandes tactiles (voir touch_layout.h).
#include "touch_layout.h"
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

// Tailles de touch_controls.m : bouton 54 pt, portail 60 pt, petits boutons
// des coins aux trois quarts (40,5 pt), à 0,45 bouton du bord.
#define B 54.0f
#define SMALL (B * .75f)
#define EDGE (B * .45f)

// Portal 2 : la disposition d'origine, inchangée.
static const rt_touch_button portal2[] = {
    // Portails, en miroir sous le compteur d'images : bleu à l'extérieur, orange à l'intérieur.
    {RT_TOUCH_FIRE, NULL, RT_TOUCH_BLUE, .06f, .19f, 0, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_ALT_FIRE, NULL, RT_TOUCH_ORANGE, .06f, .19f, 60 * 1.15f, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_FIRE, NULL, RT_TOUCH_BLUE, .94f, .19f, 0, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_ALT_FIRE, NULL, RT_TOUCH_ORANGE, .94f, .19f, -60 * 1.15f, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_JUMP, "arrow.up", RT_TOUCH_INK, .94f, .50f, 0, 0, B, 0},
    {RT_TOUCH_CROUCH, "arrow.down", RT_TOUCH_INK, .80f, .40f, 0, 0, B, 0},
    {RT_TOUCH_USE, "hand.point.up.left.fill", RT_TOUCH_INK, .94f, .80f, 0, 0, B, 0},
    {RT_TOUCH_PAUSE, "pause.fill", RT_TOUCH_INK, 1, 0, -EDGE, EDGE, SMALL, 0},
    // Signaler une erreur des tapes au dos (jeu de données), à gauche de la pause.
    {RT_TOUCH_FLAG_ERROR, "xmark", RT_TOUCH_FLAG, 1, 0, -B * 1.35f, EDGE, SMALL, RT_TOUCH_DATASET},
    // Mode de la caméra, dans le coin en bas à gauche (icône selon le mode).
    {RT_TOUCH_LOOK_MODE, "r.joystick.fill", RT_TOUCH_INK, 0, 1, EDGE, -EDGE, SMALL, 0},
};

// Half-Life 2 : sept commandes de jeu de la taille des boutons, réparties
// entre les deux pouces, le reste en petits boutons dans la barre du haut.
// - Pouce droit, en arc autour du tir (le plus gros, coin bas droit) : tir
//   secondaire, rechargement, saut, accroupi, utiliser. Les deux tirs suivent
//   le doigt : glisser en tirant vise, comme sur le trackpad.
// - Pouce gauche : le stick, et le sprint juste au-dessus (bascule : un appui
//   lance la course, elle s'arrête au relâchement du stick ou au second appui).
// - Accroupi en bascule aussi : un pouce ne peut pas tenir Ctrl tout en
//   visant ou en marchant (conduits, saut accroupi : sauter pendant qu'il est allumé).
// - Haut, petits : armes précédente et suivante au centre, sous la liste des
//   armes que le jeu y affiche (le tir choisit) ; lampe, sauvegarde rapide et
//   pause à droite ; mode de la caméra en haut à gauche (en bas, il cacherait
//   la santé du HUD).
// Décalages à partir du bord droit : la grappe garde sa forme sur tous les
// écrans (de l'iPhone SE à l'iPad).
static const rt_touch_button hl2[] = {
    {RT_TOUCH_FIRE, "scope", RT_TOUCH_RED, 1, .64f, -62, 0, 72, RT_TOUCH_AIM},
    {RT_TOUCH_ALT_FIRE, "burst.fill", RT_TOUCH_INK, 1, .60f, -150, 0, 50, RT_TOUCH_AIM},
    {RT_TOUCH_RELOAD, "arrow.clockwise", RT_TOUCH_INK, 1, .36f, -110, 0, 46, 0},
    {RT_TOUCH_JUMP, "arrow.up", RT_TOUCH_INK, 1, .36f, -34, 0, B, 0},
    {RT_TOUCH_CROUCH, "arrow.down", RT_TOUCH_INK, 1, .88f, -62, 0, 50, RT_TOUCH_TOGGLE},
    {RT_TOUCH_USE, "hand.point.up.left.fill", RT_TOUCH_INK, 1, .86f, -140, 0, 50, 0},
    {RT_TOUCH_SPRINT, "figure.run", RT_TOUCH_INK, .07f, .42f, 0, 0, 50, RT_TOUCH_TOGGLE},
    {RT_TOUCH_WEAPON_PREV, "chevron.left", RT_TOUCH_INK, .5f, 0, -30, EDGE, SMALL, 0},
    {RT_TOUCH_WEAPON_NEXT, "chevron.right", RT_TOUCH_INK, .5f, 0, 30, EDGE, SMALL, 0},
    {RT_TOUCH_FLASHLIGHT, "flashlight.on.fill", RT_TOUCH_INK, 1, 0, -B * 2.25f, EDGE, SMALL, 0},
    {RT_TOUCH_QUICKSAVE, "tray.and.arrow.down.fill", RT_TOUCH_INK, 1, 0, -B * 1.35f, EDGE, SMALL, 0},
    {RT_TOUCH_PAUSE, "pause.fill", RT_TOUCH_INK, 1, 0, -EDGE, EDGE, SMALL, 0},
    // En haut à gauche, après le compteur d'images : le coin bas gauche
    // cacherait la santé du HUD.
    {RT_TOUCH_LOOK_MODE, "r.joystick.fill", RT_TOUCH_INK, 0, 0, 170, EDGE, SMALL, 0},
};

// Left 4 Dead 2: Half-Life 2's right-thumb cluster (shoot, shove on the
// secondary fire, reload, jump, crouch, use), the item slots in a row at the
// top centre (primary, secondary, throwable, medkit, pills), flashlight and
// pause at the top right, camera mode at the top left as in Half-Life 2.
// Crouch is a toggle (a thumb can't hold it and aim); shoot aims while held.
#define SLOT (SMALL * 1.15f)
static const rt_touch_button l4d2[] = {
    {RT_TOUCH_FIRE, "scope", RT_TOUCH_RED, 1, .64f, -62, 0, 72, RT_TOUCH_AIM},
    {RT_TOUCH_ALT_FIRE, "hand.raised.fill", RT_TOUCH_INK, 1, .60f, -150, 0, 50, RT_TOUCH_AIM},   // shove
    {RT_TOUCH_RELOAD, "arrow.clockwise", RT_TOUCH_INK, 1, .36f, -110, 0, 46, 0},
    {RT_TOUCH_JUMP, "arrow.up", RT_TOUCH_INK, 1, .36f, -34, 0, B, 0},
    {RT_TOUCH_CROUCH, "arrow.down", RT_TOUCH_INK, 1, .88f, -62, 0, 50, RT_TOUCH_TOGGLE},
    {RT_TOUCH_USE, "hand.point.up.left.fill", RT_TOUCH_INK, 1, .86f, -140, 0, 50, 0},
    {RT_TOUCH_SLOT1, "1.circle.fill", RT_TOUCH_INK, .5f, 0, -2 * SLOT, EDGE, SMALL, 0},
    {RT_TOUCH_SLOT2, "2.circle.fill", RT_TOUCH_INK, .5f, 0, -SLOT, EDGE, SMALL, 0},
    {RT_TOUCH_SLOT3, "flame.fill", RT_TOUCH_INK, .5f, 0, 0, EDGE, SMALL, 0},
    {RT_TOUCH_SLOT4, "cross.case.fill", RT_TOUCH_INK, .5f, 0, SLOT, EDGE, SMALL, 0},
    {RT_TOUCH_SLOT5, "pills.fill", RT_TOUCH_INK, .5f, 0, 2 * SLOT, EDGE, SMALL, 0},
    {RT_TOUCH_FLASHLIGHT, "flashlight.on.fill", RT_TOUCH_INK, 1, 0, -B * 1.35f, EDGE, SMALL, 0},
    {RT_TOUCH_PAUSE, "pause.fill", RT_TOUCH_INK, 1, 0, -EDGE, EDGE, SMALL, 0},
    {RT_TOUCH_LOOK_MODE, "r.joystick.fill", RT_TOUCH_INK, 0, 0, 170, EDGE, SMALL, 0},
};

const rt_touch_button *rt_touch_layout(rt_ios_layout layout, int *count) {
    if (layout == RT_IOS_LAYOUT_HL2) { *count = (int)(sizeof(hl2) / sizeof(*hl2)); return hl2; }
    if (layout == RT_IOS_LAYOUT_L4D2) { *count = (int)(sizeof(l4d2) / sizeof(*l4d2)); return l4d2; }
    *count = (int)(sizeof(portal2) / sizeof(*portal2));
    return portal2;
}

double rt_touch_look_rest(rt_ios_layout layout) { return layout == RT_IOS_LAYOUT_PORTAL2 ? .80 : .64; }

void rt_touch_place(const rt_touch_button *b, double x, double y, double w, double h, double *cx, double *cy) {
    *cx = x + w * b->ax + b->ox;
    *cy = y + h * b->ay + b->oy;
}

static double clamp(double v, double lo, double hi) { return v < lo ? lo : v > hi ? hi : v; }
void rt_touch_place_scaled(const rt_touch_button *b, double scale, double x, double y, double w, double h,
                           double *cx, double *cy) {
    double r = b->size * scale / 2;
    *cx = clamp(x + w * b->ax + b->ox * scale, x + r, x + w - r);
    *cy = clamp(y + h * b->ay + b->oy * scale, y + r, y + h - r);
}

void rt_touch_overlay_update(rt_overlay_state *s, rt_overlay_event event, int pad, int keyboard,
                             int pad_active, const char *mode) {
    int always = mode && *mode == '1';
    int off_with_pad = mode && !strcmp(mode, "pad");
    switch (event) {
    case RT_OVERLAY_DEVICES:
        s->concealed = !always && (pad || keyboard);
        break;
    case RT_OVERLAY_TOUCH:
        s->concealed = !always && off_with_pad && pad;
        s->pad_held = pad_active;   // a button held through the touch does not count as new input
        break;
    case RT_OVERLAY_PAD_POLL:
        // New input only (pressed since the last poll): a stick resting past the
        // dead zone or a button held while touching does not fight the finger.
        if (pad && pad_active && !s->pad_held && !always) s->concealed = 1;
        s->pad_held = pad_active;
        break;
    }
}

int rt_touch_player2_wanted(const char *game, const char *map, int controllers) {
    return game && !strcmp(game, "portal2") && map && !strncmp(map, "mp_coop", 7) && controllers == 1;
}
int rt_touch_pad_button(rt_touch_action action) {
    switch (action) {
    case RT_TOUCH_JUMP: return 0;     // A
    case RT_TOUCH_CROUCH: return 1;   // B
    case RT_TOUCH_USE: return 2;      // X
    case RT_TOUCH_PAUSE: return 6;    // Start
    default: return -1;
    }
}
int rt_touch_pad_trigger(rt_touch_action action) {
    return action == RT_TOUCH_FIRE ? 5 : action == RT_TOUCH_ALT_FIRE ? 4 : -1;
}

// ---- Button layout editor ----
static const char *const action_names[RT_TOUCH_ACTION_COUNT] = {
    "jump", "crouch", "use", "pause", "fire", "alt_fire", "look_mode", "flag", "reload", "sprint", "flashlight",
    "weapon_prev", "weapon_next", "quicksave", "slot1", "slot2", "slot3", "slot4", "slot5"};
const char *rt_touch_action_name(rt_touch_action a) { return a < RT_TOUCH_ACTION_COUNT ? action_names[a] : "?"; }

int rt_touch_custom_format(const rt_touch_custom_layout *l, char *out, size_t size) {
    size_t used = (size_t)snprintf(out, size, "v1");
    for (int i = 0; i < RT_TOUCH_ACTION_COUNT && used < size; ++i)
        if (l->b[i].set)
            used += (size_t)snprintf(out + used, size - used, " %s=%.3f,%.3f,%.2f", action_names[i], l->b[i].fx, l->b[i].fy,
                                     l->b[i].scale);
    return used < size;
}

int rt_touch_custom_parse(const char *text, rt_touch_custom_layout *out) {
    memset(out, 0, sizeof(*out));
    if (!text || strncmp(text, "v1", 2)) return 0;
    int found = 0;
    for (const char *p = text + 2; (p = strchr(p, ' ')); ) {
        ++p;
        char name[32]; float fx, fy, scale;
        if (sscanf(p, "%31[a-z_0-9]=%f,%f,%f", name, &fx, &fy, &scale) != 4) continue;
        if (!(fx >= 0 && fx <= 1 && fy >= 0 && fy <= 1 && scale >= 0.5f && scale <= 2)) continue;   // damaged entry
        for (int i = 0; i < RT_TOUCH_ACTION_COUNT; ++i)
            if (!strcmp(name, action_names[i])) { out->b[i] = (rt_touch_custom){1, fx, fy, scale}; ++found; }
    }
    return found;
}

void rt_touch_fps_box(rt_ios_layout layout, double area_w, int chars, int lines, double pixel, double *x, double *y,
                      double *w, double *h) {
    double margin = 2 * pixel;
    *w = (chars * 6 - 1) * pixel + 2 * margin;
    *h = (lines * 9 - 2) * pixel + 2 * margin;
    // Top centre; in Half-Life 2's and Left 4 Dead 2's layouts, under their top
    // row of small buttons (weapons, item slots) at their largest size.
    *x = round((area_w - *w) / 2);
    *y = layout == RT_IOS_LAYOUT_PORTAL2 ? 8 : round(8 + SMALL * 1.5 + 8);
}

int rt_touch_fps_visible(int setting_on, int fps, int concealed) {
    (void)concealed;
    return setting_on && fps > 0;
}

void rt_touch_custom_merge(rt_touch_custom_layout *stored, const rt_touch_custom_layout *edited, rt_ios_layout layout) {
    int count = 0;
    const rt_touch_button *b = rt_touch_layout(layout, &count);
    for (int i = 0; i < count; ++i) stored->b[b[i].action] = edited->b[b[i].action];
}

int rt_touch_custom_import(rt_touch_custom_layout *stored, const rt_touch_custom_layout *old, rt_ios_layout layout) {
    int count = 0, taken = 0;
    const rt_touch_button *b = rt_touch_layout(layout, &count);
    for (int i = 0; i < count; ++i) {
        rt_touch_custom *to = &stored->b[b[i].action];
        if (to->set || !old->b[b[i].action].set) continue;
        *to = old->b[b[i].action];
        ++taken;
    }
    return taken;
}

void rt_touch_stick_rests(rt_ios_layout layout, double out[2][2]) {
    out[0][0] = .16; out[0][1] = .72;                         // move stick (touch_controls.m restingStick:)
    out[1][0] = rt_touch_look_rest(layout); out[1][1] = .72;  // camera stick
}

void rt_touch_custom_clamp(double *cx, double *cy, double d, double w, double h, const double (*stick)[2], int nstick,
                           double stick_r, double grid) {
    double r = d / 2;
    if (grid > 0) { *cx = round(*cx / grid) * grid; *cy = round(*cy / grid) * grid; }
    // Out of each stick's resting area: pushed out along the line from its centre.
    for (int i = 0; i < nstick; ++i) {
        double sx = stick[i][0] * w, sy = stick[i][1] * h, dx = *cx - sx, dy = *cy - sy, dist = hypot(dx, dy);
        double keep = stick_r * .8 + r;   // overlapping the outer fifth of the ring is fine
        if (dist >= keep) continue;
        if (dist < 1e-6) { dx = 0; dy = -1; dist = 1; }   // dead centre: straight up
        *cx = sx + dx / dist * keep; *cy = sy + dy / dist * keep;
    }
    *cx = clamp(*cx, r, w - r);
    *cy = clamp(*cy, r, h - r);
}

void rt_touch_place_custom(const rt_touch_button *b, const rt_touch_custom *c, double scale, double x, double y,
                           double w, double h, double *cx, double *cy, double *size) {
    if (!c || !c->set) {
        rt_touch_place_scaled(b, scale, x, y, w, h, cx, cy);
        *size = b->size * scale;
        return;
    }
    *size = b->size * c->scale * scale;   // the Button size slider still applies on top
    double r = *size / 2;
    *cx = clamp(x + w * c->fx, x + r, x + w - r);
    *cy = clamp(y + h * c->fy, y + r, y + h - r);
}
