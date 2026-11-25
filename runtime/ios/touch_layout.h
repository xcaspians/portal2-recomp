// Disposition des boutons des commandes tactiles, par jeu (touch_controls.m
// les dessine et les traduit en touches ; tests/checks_touch_layout.c vérifie
// qu'ils tiennent à l'écran sans se chevaucher).
#pragma once
#include <stddef.h>
#include "game_ios.h"

typedef enum {
    RT_TOUCH_JUMP,          // espace
    RT_TOUCH_CROUCH,        // Ctrl gauche
    RT_TOUCH_USE,           // E
    RT_TOUCH_PAUSE,         // Échap
    RT_TOUCH_FIRE,          // clic gauche (portail bleu, tir)
    RT_TOUCH_ALT_FIRE,      // clic droit (portail orange, tir secondaire)
    RT_TOUCH_LOOK_MODE,     // stick ou trackpad pour la caméra
    RT_TOUCH_FLAG_ERROR,    // ✗ du jeu de données des tapes au dos
    RT_TOUCH_RELOAD,        // R
    RT_TOUCH_SPRINT,        // Maj gauche (+speed)
    RT_TOUCH_FLASHLIGHT,    // F (impulse 100)
    RT_TOUCH_WEAPON_PREV,   // molette vers le haut (invprev)
    RT_TOUCH_WEAPON_NEXT,   // molette vers le bas (invnext)
    RT_TOUCH_QUICKSAVE,     // F6 (save quick)
    RT_TOUCH_ACTION_COUNT
} rt_touch_action;

typedef enum { RT_TOUCH_INK, RT_TOUCH_BLUE, RT_TOUCH_ORANGE, RT_TOUCH_RED, RT_TOUCH_FLAG } rt_touch_color;

enum {
    RT_TOUCH_PORTAL = 1,    // bouton de portail : seulement sans tapes au dos (ou P2_PORTAL_BUTTONS=1)
    RT_TOUCH_DATASET = 2,   // seulement avec le jeu de données des tapes au dos
    RT_TOUCH_TOGGLE = 4,    // un appui enfonce, le suivant relâche (allumé tant qu'enfoncé)
    RT_TOUCH_AIM = 8,       // le doigt qui le tient tourne aussi la caméra en glissant
};

// Centre : coin de la zone sûre + fraction (ax, ay) de sa taille + décalage
// (ox, oy) en points ; size : diamètre en points.
typedef struct {
    rt_touch_action action;
    const char *symbol;     // SF Symbol (NULL : disque plein, portails)
    rt_touch_color color;
    float ax, ay, ox, oy, size;
    unsigned flags;
} rt_touch_button;

// Boutons de cette disposition ; *count reçoit leur nombre.
const rt_touch_button *rt_touch_layout(rt_ios_layout layout, int *count);
// Abscisse (fraction de la zone sûre) du stick de la caméra au repos.
double rt_touch_look_rest(rt_ios_layout layout);
// Centre d'un bouton dans la zone (x, y, w, h).
void rt_touch_place(const rt_touch_button *b, double x, double y, double w, double h, double *cx, double *cy);
// Same, for the Button size setting (P2_TOUCH_SIZE): size and offset scaled
// around the button's anchor, then moved back inside the area if the larger
// button would cross its edge.
void rt_touch_place_scaled(const rt_touch_button *b, double scale, double x, double y, double w, double h,
                           double *cx, double *cy);

// FPS counter (gear menu > FPS counter): its box in points from the safe
// area's corner (width area_w), for `lines` lines of at most `chars`
// characters of the 5x7 pixel font at `pixel` points per font pixel
// (touch_controls.m draws it). Clear of every button: top centre in Portal's
// layout (the portal buttons sit under the top left corner), top left in
// Half-Life 2's (its weapon buttons are at the top centre).
void rt_touch_fps_box(rt_ios_layout layout, double area_w, int chars, int lines, double pixel, double *x, double *y,
                      double *w, double *h);

// The counter shows when its setting is on and a rate is measured, whether or
// not a controller or keyboard hides the touch buttons (concealed): a
// controller only hides the buttons.
int rt_touch_fps_visible(int setting_on, int fps, int concealed);

// ---- Button layout editor (settings menu, touch_editor_ios.m) ----
// A player's own place and size for some buttons, per device kind (phone or
// iPad) and per game; the others keep the built-in layout.
typedef struct { int set; float fx, fy, scale; } rt_touch_custom;   // centre as fractions of the area; size multiplier
typedef struct { rt_touch_custom b[RT_TOUCH_ACTION_COUNT]; } rt_touch_custom_layout;
// Stored as "v1 jump=0.930,0.600,1.10 use=...": unknown or damaged entries are skipped.
int rt_touch_custom_format(const rt_touch_custom_layout *l, char *out, size_t size);
int rt_touch_custom_parse(const char *text, rt_touch_custom_layout *out);
// Saving from one game into the layout every game shares: the buttons this
// game's layout has take the edited entries (set or back to the default); a
// button only another game has keeps its stored entry. Writes into *stored.
void rt_touch_custom_merge(rt_touch_custom_layout *stored, const rt_touch_custom_layout *edited, rt_ios_layout layout);
// Where a dragged button may go (area w x h, its diameter d): fully inside the
// area, snapped to a grid of `grid` points, and pushed out of the sticks'
// resting areas (centres stick[i] and radius stick_r) so a thumb on a stick
// doesn't press it. Moves (*cx, *cy) in place (points from the area's corner).
void rt_touch_custom_clamp(double *cx, double *cy, double d, double w, double h, const double (*stick)[2], int nstick,
                           double stick_r, double grid);
// Resting centres of the move and camera sticks (fractions of the area).
void rt_touch_stick_rests(rt_ios_layout layout, double out[2][2]);
// A button's centre and diameter with a custom entry (else rt_touch_place_scaled).
void rt_touch_place_custom(const rt_touch_button *b, const rt_touch_custom *c, double scale, double x, double y,
                           double w, double h, double *cx, double *cy, double *size);
// Action names in the stored text ("jump", "use"...).
const char *rt_touch_action_name(rt_touch_action a);

// Overlay hidden for a controller or a keyboard (touch_controls.m draws it,
// tests/checks_touch_layout.c checks these rules). mode is P2_TOUCH_CONTROLS:
// NULL or "auto" hides it while a controller or keyboard is connected, until a
// touch brings it back and the next controller input hides it again; "1" always
// shows it; "pad" keeps it off (touches ignored) while a controller is connected.
typedef enum {
    RT_OVERLAY_DEVICES,     // a controller or keyboard connected or disconnected
    RT_OVERLAY_TOUCH,       // a finger touched the screen
    RT_OVERLAY_PAD_POLL,    // once per frame: is a controller button or stick in use
} rt_overlay_event;
typedef struct {
    int concealed;          // overlay hidden
    int pad_held;           // controller input already in use at the last event
} rt_overlay_state;
void rt_touch_overlay_update(rt_overlay_state *s, rt_overlay_event event, int pad, int keyboard,
                             int pad_active, const char *mode);
