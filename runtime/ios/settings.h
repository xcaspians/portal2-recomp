// Réglages iOS (app Réglages) traduits en variables P2_* : runtime/ios/settings.c.
#pragma once
#include <stddef.h>

typedef struct { const char *key, *env, *on, *off; } rt_ios_setting;
extern const rt_ios_setting rt_ios_settings[];
extern const int rt_ios_settings_count;

// Lit la valeur d'un réglage (« 1 »/« 0 » pour un interrupteur) ; 0 si absent.
typedef int (*rt_ios_settings_get)(void *ctx, const char *key, char *out, size_t size);
// Pose les variables des réglages présents, sauf celles déjà définies ; rend
// leur nombre et, dans summary, la liste « VAR=valeur, ... ».
int rt_ios_settings_apply(rt_ios_settings_get get, void *ctx, char *summary, size_t size);
// The setting with this key (Settings.bundle and the in-game menu), or NULL.
const rt_ios_setting *rt_ios_setting_named(const char *key);
// Value of its P2_* variable for this stored value ("1"/"0" for a switch), or
// NULL: variable left unset (the code's default; "auto" or empty).
const char *rt_ios_setting_env_value(const rt_ios_setting *s, const char *value);
// On iOS: the stored preference (NSUserDefaults, shared by the Settings app
// and the in-game menu), 0 if never set.
int rt_ios_preference(const char *key, char *out, size_t size);
// Sur iOS : depuis les préférences de l'app.
int rt_ios_settings_load(char *summary, size_t size);
// Taille de rendu (pixels, paysage) : écran en points × échelle de rendu (sdl_ios.m).
void rt_ios_render_size(int *w, int *h);
// Clés MTLD3D_CONFIG propres à iOS : present.maxFps, et render.scale si
// upscale (P2_IOS_UPSCALE) est dans ]0, 1[ ; *scale reçoit l'échelle retenue (1 : aucune).
int rt_ios_metal_keys(char *out, size_t size, int max_fps, const char *upscale, double *scale);
// Frame rate cap from P2_IOS_MAX_FPS: a number (at least 1) is used as is;
// "auto" is the screen's maximum (120 on a ProMotion screen, 60 otherwise);
// anything else, or nothing, gives 60 (power_ios.m applies power saving on top).
int rt_ios_fps_choice(const char *value, int screen_max);
// The display link's frame rate range for a cap (gl_ios.m): min = max =
// preferred = the cap, at most the screen's maximum; a cap that doesn't divide
// 60 (40) asks for 120 on a ProMotion screen (40 fps = every third refresh).
// So "auto" asks for 120 on a 120 Hz screen and 60 on a 60 Hz one.
void rt_ios_display_range(int cap, int screen_max, float *lo, float *hi, float *pref);
// Metal's own cap at launch (mtld3d's present.maxFps, fixed once it starts):
// the screen's maximum, so the setting can go up to it without a restart; a
// lower cap is paced by the app after each Present (rt_ios_pace).
int rt_ios_metal_launch_cap(int screen_max);
// Frame pacing under Metal's cap, after each Present: *last is the previous
// frame's slot (0 to start). Returns the time to wait until (0: go on now).
// A cap at or over `limit` (Metal's own cap) or under 1: no pacing. Late by
// more than a frame: the grid starts again from now (no catching up).
double rt_ios_pace(double now, double *last, int cap, int limit);
