// Plafond d'images par seconde sur iOS (runtime/ios/power_ios.m).
#pragma once

// Plafond voulu : P2_IOS_MAX_FPS (30, 40, 60 par défaut, 120), ramené à 30
// au plus en mode économie d'énergie ou téléphone chaud (état thermique
// « sérieux » ou « critique ») si l'économie automatique est active
// (P2_IOS_POWER_SAVE, 1 par défaut). Followed live by both renderers: the
// display link's range (gl_ios.m), OpenGL's limiter, and under Metal the
// app's pacing after each Present (metal_ios.m; mtld3d itself starts at the
// screen's maximum, rt_ios_metal_launch_cap).
int rt_ios_max_fps(void);
// P2_IOS_MAX_FPS changed in game: recomputed and applied at once (round 11).
void rt_ios_power_reload(void);
// Suit les changements d'état thermique et du mode économie d'énergie (une
// fois, n'importe quel thread).
void rt_ios_power_watch(int metal);
// The display link follows a new cap (gl_ios.m; any thread).
void rt_ios_frame_rate_changed(void);
// The display's current refresh rate (Hz), as the system reports it (gl_ios.m).
double rt_ios_display_hz(void);
