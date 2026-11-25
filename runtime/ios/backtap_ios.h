// Tapes au dos sur iOS (runtime/ios/backtap_ios.m) : capteurs CoreMotion sur
// leur file, jugés par back_tap.c, avec le jeu de données (backtap_dataset.c)
// et l'enregistrement brut. La surcouche tactile (touch_controls.m) démarre
// les capteurs, leur dit l'orientation et l'état de la partie, et clique la
// tape acceptée. En-tête C : rien d'UIKit.
#pragma once

// Démarre les capteurs (thread principal). Sans capteur (simulateur) ou avec
// P2_BACKTAP=0 : rien, et rt_ios_backtap_available rend 0.
void rt_ios_backtap_start(void);
int rt_ios_backtap_available(void);
// Capteurs en marche ou arrêtés (thread principal) ; mêmes réglages à la reprise.
void rt_ios_backtap_run(int on);
// Thresholds and side swap read again from P2_BACKTAP_* (settings changed in game).
void rt_ios_backtap_reload(void);
// Orientation de l'écran (+1 : axe y de l'appareil vers la droite, -1, 0) et
// « en partie » (surcouche affichée, pas de menu), lus par la file des capteurs.
void rt_ios_backtap_set_state(int orient, int playing);
// Tape acceptée depuis le dernier appel : -1 gauche, +1 droite, 0 aucune.
int rt_ios_backtap_take(void);
// Durée de l'appui du clic d'une tape (s, P2_BACKTAP_HOLD).
float rt_ios_backtap_hold(void);
// Début ou fin d'un toucher à cet horodatage (UITouch.timestamp) : pas de tape autour.
void rt_ios_backtap_touch(double timestamp);
// Mouvement de caméra en comptes de souris (règle « mauvais côté » du jeu de données).
void rt_ios_backtap_look(long counts);
// Jeu de données : actif ? ; bouton ✗ ; comptes affichés (taps = -1 : rien).
int rt_ios_backtap_dataset_on(void);
void rt_ios_backtap_flag(void);
void rt_ios_backtap_counts(long *taps, long *fixes, long *reported);

// ---- Calibration (settings menu, backtap_calibration_ios.m; fit in backtap_calibrate.c) ----
#include <stddef.h>
#include "back_tap.h"
// This device model's stored calibration (text, backtap_calibrate.h), or NULL.
const char *rt_ios_backtap_calibration(char *out, size_t size);
// Stores it (NULL: back to the defaults) and applies it at once.
void rt_ios_backtap_store_calibration(const char *text);
// Capture for the calibration screen: sensors on even in a menu, every tap
// reported. judge NULL: a wide gate (calibration); else these values (test mode).
// No portal is fired while it runs.
void rt_ios_backtap_capture(int on, const rt_backtap_params *judge);
int rt_ios_backtap_capture_take(rt_backtap_report *out);   // 1 when a tap was waiting
// Noise since the last reset (held still): RMS acceleration, jerk, rotation.
void rt_ios_backtap_capture_noise(float *a_rms, float *jerk_rms, float *w_rms, int reset);
