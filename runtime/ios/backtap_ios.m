// Tapes au dos (CoreMotion) : voir backtap_ios.h.
// Échantillons de mouvement (accélération utilisateur, rotation, gravité) sur
// une file dédiée, jugés par back_tap.c ; une tape acceptée est déposée dans
// backtap_pending et cliquée par la minuterie de la surcouche (touch_controls.m,
// step), qui connaît l'état menu/jeu. Les débuts et fins de toucher arrivent du
// thread principal : verrou autour du détecteur. Pas d'autorisation à demander
// pour ces capteurs (seule l'activité physique, CMMotionActivityManager, en exige une).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <CoreMotion/CoreMotion.h>
#import <QuartzCore/QuartzCore.h>
#include <math.h>
#include <os/lock.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "back_tap.h"
#include "backtap_dataset.h"
#include "backtap_ios.h"
#include "device_ios.h"
#include "env_ios.h"

static CMMotionManager *backtap_motion;
static void params_now(rt_backtap_params *p);
static int backtap_off;   // Back taps switched off in game (P2_BACKTAP=0 after start)
// ---- Calibration capture (settings menu, backtap_calibration_ios.m) ----
static _Atomic int capture_on;
static rt_backtap capture_detector;
enum { CAPTURE_RING = 32 };
static rt_backtap_report capture_ring[CAPTURE_RING];
static unsigned capture_head, capture_tail;
static double noise_sq_a, noise_sq_j, noise_sq_w, noise_n;
static float noise_last_a[3];
static int noise_have_last;

static NSOperationQueue *backtap_queue;
static os_unfair_lock backtap_lock = OS_UNFAIR_LOCK_INIT;
static rt_backtap backtap_detector;
static _Atomic int backtap_orient;    // +1 : axe y de l'appareil vers la droite de l'écran
static _Atomic int backtap_active;    // en partie, surcouche affichée
static _Atomic int backtap_pending;   // tape à cliquer : -1 gauche, +1 droite
static int backtap_log;               // P2_BACKTAP_LOG=1 : chaque candidat, et le bruit toutes les 5 s
static float backtap_hold;            // s : durée de l'appui
// P2_BACKTAP_RAW=<fichier> (relatif à HOME ; finissant par « / » : un fichier daté
// par lancement dans ce dossier) : échantillons et touchers bruts,
// rejoués sur le Mac par tools/backtap_replay.c pour régler les seuils.
static FILE *backtap_raw;
// Jeu de données à étiquettes devinées (backtap_dataset.c), sur la file des
// capteurs : détecteur large (quart des seuils), fichier
// Documents/backtap/dataset.csv gardé d'une partie à l'autre, bouton ✗ et
// ligne DATA. Seulement à la demande du joueur : avec l'enregistrement brut
// (réglage « Record taps », P2_BACKTAP_RAW) ou P2_BACKTAP_DATASET=1 ou
// =<fichier> (relatif à HOME) ; P2_BACKTAP_DATASET=0 le coupe toujours.
// P2_BACKTAP_TAG étiquette la session.
static rt_backtap backtap_wide;
static rt_btds backtap_dataset;
static int backtap_dataset_on;
static _Atomic long backtap_look;          // caméra : comptes de souris cumulés (|dx| + |dy|)
static _Atomic long backtap_data_taps = -1, backtap_data_fixes, backtap_data_reported;   // pour l'affichage (-1 : rien)
static double backtap_flag_at;             // appui sur ✗ pas encore transmis (sous backtap_lock ; 0 : aucun)
static CMDeviceMotionHandler backtap_handler;   // traitement d'un échantillon (rt_ios_backtap_start)
static int backtap_running;                 // capteurs en marche (rt_ios_backtap_run, thread principal)
static _Atomic double backtap_resumed_until; // échantillons d'avant cet instant traités comme en menu
// Journal, sur la file des capteurs seulement : origine des temps, bruit de fond.
static struct { double origin, window, last_t, sq; float last_a[3], max_a, max_jerk, max_rot; long samples; } backtap_noise;

static void backtap_log_report(const rt_backtap_report *r, double origin) {
    char touch[32], verdict[80];
    if (fabs(r->touch_dt) < 10) snprintf(touch, sizeof(touch), "%+.0f ms", r->touch_dt * 1000);
    else snprintf(touch, sizeof(touch), "aucun");
    if (r->reject) snprintf(verdict, sizeof(verdict), "rejet : %s", r->reject);
    else snprintf(verdict, sizeof(verdict), "%s (%s%s)", r->side < 0 ? "GAUCHE, portail bleu" : "DROITE, portail orange",
                  r->method, r->gyro ? ", gyroscope" : "");
    fprintf(stderr, "[ios] tape %.3f s : pic %.2f g (x %+.2f y %+.2f z %+.2f), secousse %.0f g/s, "
            "rotation x %+.2f y %+.2f z %+.2f rad/s, montée %.0f ms, durée %.0f ms, avant %.3f g, "
            "gravité %.2f g (x %+.2f y %+.2f z %+.2f), "
            "toucher %s, orientation %+d -> %s\n",
            r->t - origin, r->peak, r->acc[0], r->acc[1], r->acc[2], r->jerk, r->rot[0], r->rot[1], r->rot[2],
            r->rise * 1000, r->width * 1000, r->pre_rms, r->gravity, r->grav[0], r->grav[1], r->grav[2], touch,
            r->orient, verdict);
}

// Mode journal : chaque candidat jugé, et toutes les 5 s le bruit de fond
// (maxima et moyenne quadratique) pour placer les seuils au-dessus du jeu normal.
static void backtap_log_sample(const rt_backtap_sample *s, const rt_backtap_report *r) {
    typeof(backtap_noise) *n = &backtap_noise;
    if (!n->origin) { n->origin = n->window = s->t; fprintf(stderr, "[ios] tapes au dos : origine des temps %.3f s\n", s->t); }
    if (r) backtap_log_report(r, n->origin);
    float mag = sqrtf(s->a[0] * s->a[0] + s->a[1] * s->a[1] + s->a[2] * s->a[2]);
    if (n->samples && s->t > n->last_t) {
        float da[3] = {s->a[0] - n->last_a[0], s->a[1] - n->last_a[1], s->a[2] - n->last_a[2]};
        n->max_jerk = fmaxf(n->max_jerk, sqrtf(da[0] * da[0] + da[1] * da[1] + da[2] * da[2]) / (float)(s->t - n->last_t));
    }
    n->max_a = fmaxf(n->max_a, mag); n->max_rot = fmaxf(n->max_rot, fabsf(s->w[0])); n->sq += (double)mag * mag;
    memcpy(n->last_a, s->a, sizeof(n->last_a)); n->last_t = s->t; ++n->samples;
    if (s->t - n->window < 5) return;
    fprintf(stderr, "[ios] tapes au dos, bruit %.0f-%.0f s : %.0f Hz, acc max %.3f g (moy. quadr. %.3f), "
            "secousse max %.1f g/s, rotation x max %.2f rad/s, %s\n",
            n->window - n->origin, s->t - n->origin, n->samples / (s->t - n->window), n->max_a, sqrt(n->sq / n->samples),
            n->max_jerk, n->max_rot, atomic_load(&backtap_active) ? "en partie" : "menu");
    n->window = s->t; n->sq = 0; n->max_a = n->max_jerk = n->max_rot = 0; n->samples = 0;
}

// Chemin absolu : tel quel, sinon relatif à HOME.
static void home_path(char *out, size_t size, const char *name) {
    snprintf(out, size, "%s%s%s", name[0] == '/' ? "" : getenv("HOME"), name[0] == '/' ? "" : "/", name);
}

// Traitement d'un échantillon, sur la file des capteurs.
static void backtap_sample(CMDeviceMotion *m, NSError *error) {
    if (!m) { if (error) fprintf(stderr, "[ios] tapes au dos : %s\n", error.localizedDescription.UTF8String); return; }
    CMAcceleration a = m.userAcceleration, g = m.gravity;
    CMRotationRate w = m.rotationRate;
    rt_backtap_sample s = {m.timestamp, {(float)a.x, (float)a.y, (float)a.z},
                           {(float)w.x, (float)w.y, (float)w.z}, {(float)g.x, (float)g.y, (float)g.z}};
    rt_backtap_report r;
    // Juste après une reprise des capteurs : comme en menu (le toucher qui a
    // fermé le menu, passé à SDL, échappe à la garde des touchers).
    int active = atomic_load(&backtap_active) && s.t >= atomic_load(&backtap_resumed_until);
    os_unfair_lock_lock(&backtap_lock);
    if (backtap_raw)
        fprintf(backtap_raw, "m %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %d %d\n", s.t, s.a[0], s.a[1], s.a[2],
                s.w[0], s.w[1], s.w[2], s.g[0], s.g[1], s.g[2], atomic_load(&backtap_orient), active);
    int judged = rt_backtap_feed(&backtap_detector, &s, atomic_load(&backtap_orient), active && !backtap_off, &r);
    if (atomic_load(&capture_on)) {   // calibration screen: its own detector, always "in game"; noise held still
        rt_backtap_report c;
        if (rt_backtap_feed(&capture_detector, &s, atomic_load(&backtap_orient), 1, &c) && !c.weak &&
            capture_head - capture_tail < CAPTURE_RING)
            capture_ring[capture_head++ % CAPTURE_RING] = c;
        float a2 = s.a[0] * s.a[0] + s.a[1] * s.a[1] + s.a[2] * s.a[2], w2 = s.w[0] * s.w[0] + s.w[1] * s.w[1] + s.w[2] * s.w[2];
        noise_sq_a += a2; noise_sq_w += w2;
        if (noise_have_last) {
            float j2 = 0;
            for (int k = 0; k < 3; ++k) j2 += (s.a[k] - noise_last_a[k]) * (s.a[k] - noise_last_a[k]);
            noise_sq_j += j2 * 100 * 100;   // per second, at CoreMotion's 100 Hz
        }
        memcpy(noise_last_a, s.a, sizeof(noise_last_a)); noise_have_last = 1;
        noise_n += 1;
    }
    rt_backtap_report rw;
    int wide = backtap_dataset_on &&
               rt_backtap_feed(&backtap_wide, &s, atomic_load(&backtap_orient), active, &rw);
    double flag = backtap_flag_at; backtap_flag_at = 0;
    // Touchers arrivés depuis le dernier échantillon (le jeu de données les
    // veut même après le bilan d'un candidat). touches_seen n'est lu et écrit
    // qu'ici, sur la file des capteurs (une opération à la fois) : pas d'atomique.
    static unsigned touches_seen;
    double touches[RT_BACKTAP_TOUCHES]; unsigned ntouch = backtap_wide.ntouch, first = touches_seen;
    if (ntouch - first > RT_BACKTAP_TOUCHES) first = ntouch - RT_BACKTAP_TOUCHES;
    for (unsigned i = first; i < ntouch; ++i) touches[i - first] = backtap_wide.touches[i % RT_BACKTAP_TOUCHES];
    touches_seen = ntouch;
    os_unfair_lock_unlock(&backtap_lock);
    if (judged && !r.reject && !atomic_load(&capture_on)) atomic_store(&backtap_pending, r.side);
    if (backtap_dataset_on) {   // file des capteurs seulement (pas de verrou)
        double look = (double)atomic_load(&backtap_look);
        if (wide) rt_btds_candidate_add(&backtap_dataset, &backtap_wide, &rw, look);
        if (judged && !r.reject) rt_btds_fire_add(&backtap_dataset, r.t, r.side, look);
        if (flag) rt_btds_flag_add(&backtap_dataset, flag);
        for (unsigned i = first; i < ntouch; ++i) rt_btds_touch_add(&backtap_dataset, touches[i - first]);
        rt_btds_tick(&backtap_dataset, s.t);
        if (backtap_dataset.write_error == 1) {   // une fois
            backtap_dataset.write_error = 2;
            fprintf(stderr, "[ios] tapes au dos : écriture du jeu de données en échec\n");
        }
        atomic_store(&backtap_data_taps, backtap_dataset.taps);
        atomic_store(&backtap_data_fixes, backtap_dataset.corrections);
        atomic_store(&backtap_data_reported, backtap_dataset.reported);
    }
    if (backtap_log) backtap_log_sample(&s, judged ? &r : NULL);
}

// Enregistrement brut (P2_BACKTAP_RAW), ouvert si demandé.
static void backtap_open_raw(const char *raw) {
    char path[1024];
    home_path(path, sizeof(path), raw);
    size_t len = strlen(path);
    if (len && path[len - 1] == '/') {   // dossier (réglage « Enregistrer les tapes ») : un fichier daté par lancement
        mkdir(path, 0755);
        time_t now = time(NULL);
        strftime(path + len, sizeof(path) - len, "backtap-%Y%m%d-%H%M%S.txt", localtime(&now));
    }
    backtap_raw = fopen(path, "w");
    fprintf(stderr, "[ios] tapes au dos : enregistrement brut %s%s\n", path, backtap_raw ? "" : " impossible");
    if (backtap_raw) setvbuf(backtap_raw, NULL, _IOLBF, 0);   // tout est écrit si le jeu est tué
    if (backtap_raw) fprintf(backtap_raw, "# m t ax ay az wx wy wz gx gy gz orient active | t t_toucher | f t_bouton_✗\n");
}

// Jeu de données, si le joueur le demande (rt_btds_wanted).
static void backtap_open_dataset(const rt_backtap_params *p, const char *raw) {
    const char *dataset = getenv("P2_BACKTAP_DATASET");
    int custom;
    if (!rt_btds_wanted(dataset, raw, &custom)) return;
    char path[1024];
    if (custom) home_path(path, sizeof(path), dataset);
    else {
        snprintf(path, sizeof(path), "%s/Documents/backtap", getenv("HOME"));
        mkdir(path, 0755);
        snprintf(path, sizeof(path), "%s/Documents/backtap/dataset.csv", getenv("HOME"));
    }
    long counts[3] = {0, 0, 0};
    if (rt_btds_repair_tail(path)) fprintf(stderr, "[ios] tapes au dos : fin du jeu de données illisible\n");
    FILE *old = fopen(path, "r");
    rt_btds_count_file(old, counts);
    if (old) fclose(old);
    FILE *out = fopen(path, "a");
    if (out) {
        setvbuf(out, NULL, _IOLBF, 0);
        fseek(out, 0, SEEK_END);
        if (!ftell(out)) fputs(rt_btds_header, out);
        rt_backtap_params pw = *p;
        pw.onset_factor = rt_ios_env_float("P2_BACKTAP_WIDE", RT_BTDS_WIDE_ONSET, 0.01f);
        rt_backtap_init(&backtap_wide, &pw);
        rt_btds_init(&backtap_dataset, p, out, getenv("P2_BACKTAP_TAG"), CACurrentMediaTime(), counts);
        backtap_dataset_on = 1;
        atomic_store(&backtap_data_taps, counts[0]); atomic_store(&backtap_data_fixes, counts[1]);
        atomic_store(&backtap_data_reported, counts[2]);
    }
    fprintf(stderr, "[ios] tapes au dos : jeu de données %s%s (%ld tapes, %ld corrections dont %ld signalées)\n", path,
            out ? "" : " impossible", counts[0], counts[1], counts[2]);
}

void rt_ios_backtap_start(void) {
    if (backtap_motion || !rt_ios_env_flag("P2_BACKTAP", 1)) return;
    CMMotionManager *motion = [CMMotionManager new];
    if (!motion.deviceMotionAvailable) {
        fprintf(stderr, "[ios] tapes au dos : pas de capteur de mouvement (simulateur ?)\n");
        return;
    }
    rt_backtap_params p;
    params_now(&p);   // défauts, calibration de l'appareil, sensibilité, variables par seuil (back_tap.c)
    backtap_log = rt_ios_env_flag("P2_BACKTAP_LOG", 0);
    backtap_hold = rt_ios_env_float("P2_BACKTAP_HOLD", 0.05f, 0);
    const char *raw = getenv("P2_BACKTAP_RAW");
    if (raw && *raw) backtap_open_raw(raw);
    float hz = rt_ios_env_float("P2_BACKTAP_HZ", 200, 10);
    rt_backtap_init(&backtap_detector, &p);
    backtap_open_dataset(&p, raw);

    backtap_queue = [NSOperationQueue new];
    backtap_queue.name = @"p2.backtap";
    backtap_queue.maxConcurrentOperationCount = 1;
    backtap_queue.qualityOfService = NSQualityOfServiceUserInteractive;
    motion.deviceMotionUpdateInterval = 1.0 / hz;
    fprintf(stderr, "[ios] tapes au dos : %g Hz demandés ; seuils %.3g g, %.3g g/s, rotation %.3g rad/s ; "
            "gyroscope %.3g g, %.3g rad/s, %.3g rad/s² ; "
            "réfractaire %.0f ms, toucher ±%.0f ms, à plat sous %.2f g, durée %.0f ms, agitation %.3g g, gravité %.3g g ; "
            "latéral %+d, z %+d, inversé %d, appui %.0f ms%s\n",
            hz, p.accel_min, p.jerk_min, p.rot_min, p.gyro_accel_min, p.gyro_rot_min, p.gyro_jerk_min, p.refractory * 1e3, p.touch_guard * 1e3, p.flat_z, p.max_width * 1e3,
            p.pre_rms_max, p.gravity_max, p.lateral_sign, p.z_sign, p.invert, backtap_hold * 1e3,
            backtap_log ? ", journal" : "");
    backtap_handler = ^(CMDeviceMotion *m, NSError *error) { backtap_sample(m, error); };
    backtap_motion = motion;
    rt_ios_backtap_run(1);
}
int rt_ios_backtap_available(void) { return backtap_motion != nil && !backtap_off; }

// ---- Calibration capture (settings menu, backtap_calibration_ios.m) ----

// The device model's stored calibration (app settings, one key per model).
static NSString *calibration_key(void) {
    char model[64];
    rt_ios_device_model(model, sizeof(model));
    return [@"P2BackTapCal." stringByAppendingString:@(model)];
}
const char *rt_ios_backtap_calibration(char *out, size_t size) {
    NSString *v = [NSUserDefaults.standardUserDefaults stringForKey:calibration_key()];
    if (!v || !size) return NULL;
    snprintf(out, size, "%s", v.UTF8String);
    return out;
}
void rt_ios_backtap_store_calibration(const char *text) {
    if (text) [NSUserDefaults.standardUserDefaults setObject:@(text) forKey:calibration_key()];
    else [NSUserDefaults.standardUserDefaults removeObjectForKey:calibration_key()];
    rt_ios_backtap_reload();
}
static void params_now(rt_backtap_params *p) {
    char cal[256];
    rt_backtap_params_from_env_calibrated(p, rt_ios_backtap_calibration(cal, sizeof(cal)));
}

void rt_ios_backtap_capture(int on, const rt_backtap_params *judge) {
    if (!backtap_motion) return;
    rt_backtap_params p;
    if (judge) p = *judge;
    else {   // wide gate: every tap the player makes, judged later by the fit
        rt_backtap_defaults(&p);
        p.accel_min /= 4; p.jerk_min /= 4; p.gyro_accel_min /= 4; p.gyro_rot_min /= 4; p.rot_min /= 2;
        p.onset_factor = 0.5f;
    }
    os_unfair_lock_lock(&backtap_lock);
    rt_backtap_init(&capture_detector, &p);
    capture_head = capture_tail = 0;
    os_unfair_lock_unlock(&backtap_lock);
    atomic_store(&capture_on, on);
    if (on) rt_ios_backtap_run(1);   // in a menu too, while the calibration screen is up
}
int rt_ios_backtap_capture_take(rt_backtap_report *out) {
    os_unfair_lock_lock(&backtap_lock);
    int got = capture_tail != capture_head;
    if (got) *out = capture_ring[capture_tail++ % CAPTURE_RING];
    os_unfair_lock_unlock(&backtap_lock);
    return got;
}
void rt_ios_backtap_capture_noise(float *a_rms, float *jerk_rms, float *w_rms, int reset) {
    os_unfair_lock_lock(&backtap_lock);
    double n = noise_n > 0 ? noise_n : 1;
    *a_rms = (float)sqrt(noise_sq_a / n); *jerk_rms = (float)sqrt(noise_sq_j / n); *w_rms = (float)sqrt(noise_sq_w / n);
    if (reset) { noise_sq_a = noise_sq_j = noise_sq_w = noise_n = 0; noise_have_last = 0; }
    os_unfair_lock_unlock(&backtap_lock);
}
void rt_ios_backtap_reload(void) {
    // Back taps switched in game: off stops the sensors and brings the portal
    // buttons back (touch_controls.m asks rt_ios_backtap_available); on starts them.
    backtap_off = !rt_ios_env_flag("P2_BACKTAP", 1);
    if (!backtap_off && !backtap_motion) rt_ios_backtap_start();
    if (!backtap_motion) return;
    if (backtap_off) rt_ios_backtap_run(0);
    rt_backtap_params p;
    params_now(&p);   // this device's calibration, if any, under the settings
    // Samples are judged under backtap_lock: the new thresholds apply from the next one.
    os_unfair_lock_lock(&backtap_lock);
    backtap_detector.p = p;
    if (backtap_dataset_on) {
        backtap_wide.p = p;
        backtap_wide.p.onset_factor = rt_ios_env_float("P2_BACKTAP_WIDE", RT_BTDS_WIDE_ONSET, 0.01f);
    }
    os_unfair_lock_unlock(&backtap_lock);
    fprintf(stderr, "[ios] tapes au dos : seuils %.3g g, %.3g g/s, inversé %d\n", p.accel_min, p.jerk_min, p.invert);
}

// Capteurs en marche ou arrêtés (thread principal) : 200 Hz demandés en
// permanence coûtent de l'énergie pour rien en menu ou avec une manette, où
// aucune tape n'est acceptée (touch_controls.m décide). Mêmes réglages à la reprise.
void rt_ios_backtap_run(int on) {
    if (!on && atomic_load(&capture_on)) return;   // the calibration screen keeps them on
    if (on && backtap_off && !atomic_load(&capture_on)) return;
    if (!backtap_motion || on == backtap_running) return;
    backtap_running = on;
    if (on) {
        atomic_store(&backtap_resumed_until, CACurrentMediaTime() + 0.15);   // horloge de CMLogItem.timestamp
        [backtap_motion startDeviceMotionUpdatesToQueue:backtap_queue withHandler:backtap_handler];
    } else [backtap_motion stopDeviceMotionUpdates];
    static int logged;
    if (logged++ < 20) fprintf(stderr, "[ios] tapes au dos : capteurs %s\n", on ? "en marche" : "arrêtés (menu ou surcouche cachée)");
}
void rt_ios_backtap_set_state(int orient, int playing) {
    atomic_store(&backtap_orient, orient);
    atomic_store(&backtap_active, playing);
}
int rt_ios_backtap_take(void) { return atomic_exchange(&backtap_pending, 0); }
float rt_ios_backtap_hold(void) { return backtap_hold; }
void rt_ios_backtap_look(long counts) { atomic_fetch_add(&backtap_look, counts); }
int rt_ios_backtap_dataset_on(void) { return backtap_dataset_on; }
void rt_ios_backtap_counts(long *taps, long *fixes, long *reported) {
    *taps = atomic_load(&backtap_data_taps); *fixes = atomic_load(&backtap_data_fixes);
    *reported = atomic_load(&backtap_data_reported);
}
// Bouton ✗ : le joueur signale une erreur des tapes au dos (backtap_dataset.c
// en déduit laquelle). Même horloge que les capteurs (CACurrentMediaTime).
void rt_ios_backtap_flag(void) {
    if (!backtap_dataset_on) return;
    os_unfair_lock_lock(&backtap_lock);
    backtap_flag_at = CACurrentMediaTime();
    if (backtap_raw) fprintf(backtap_raw, "f %.4f\n", backtap_flag_at);
    os_unfair_lock_unlock(&backtap_lock);
}
void rt_ios_backtap_touch(double timestamp) {
    if (!backtap_motion) return;
    os_unfair_lock_lock(&backtap_lock);
    rt_backtap_touch(&backtap_detector, timestamp);
    if (backtap_dataset_on) rt_backtap_touch(&backtap_wide, timestamp);
    if (backtap_raw) fprintf(backtap_raw, "t %.4f\n", timestamp);
    os_unfair_lock_unlock(&backtap_lock);
}
#endif
