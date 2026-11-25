// Point d'entrée iOS : SDL2 (libSDL2main) démarre UIKit puis appelle ce main
// sur le thread principal. Il prépare l'environnement et appelle le main du
// runtime (src/main.c, compilé avec -Dmain=p2_main).
//
// Arguments : ceux du lancement (simctl launch … -- arguments), sinon le
// fichier launch.args du paquet (un argument par ligne, # pour commenter),
// complété par Documents/launch.args s'il existe.
// --user-dir, --images, --game-dir et --linux-root manquants : dossiers
// <jeu>-userdata (portal2-userdata, hl2-userdata), prep, game et linux de
// Documents (s'ils existent). Le jeu vient de l'Info.plist (runtime/ios/game_ios.c).
#include <SDL_main.h>
#include <CoreFoundation/CoreFoundation.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include "device_defaults_ios.h"
#include "device_ios.h"
#include "game_ios.h"
#include "game_picker_ios.h"
#include "metal_ios.h"
#include "settings.h"

int p2_main(int argc, char **argv);
int rt_select_game_code(const char *name);   // runtime/rt.h, RT_MULTI_GAME (generated game_codes.c)

static char *arguments[256];
static int count;
static void push(const char *value) { if (count < 255) arguments[count++] = strdup(value); }

// launch.args of a folder; in the Portal app a hosted game has its own,
// launch-<game>.args in the app and Documents/<game>/launch.args.
static void read_launch_file(const char *folder, const char *name) {
    char path[PATH_MAX], line[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", folder, name);
    FILE *file = fopen(path, "r");
    if (!file) return;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] && line[0] != '#') push(line);
    }
    fclose(file);
}

// Données du jeu copiées dans Documents (game, prep, linux : ~11,5 Go) :
// exclues de la sauvegarde iCloud/iTunes, qui les recopierait à chaque
// sauvegarde du téléphone alors qu'elles viennent du Mac. Les sauvegardes du
// joueur (<jeu>-userdata) restent sauvegardées. Attribut posé sur le dossier
// (il vaut pour son contenu), seulement s'il manque : coût d'un stat par dossier.
static void exclude_from_backup(const char *documents) {
    static const char *const folders[] = {"game", "prep", "linux", "mel"};
    char done[64] = "", already[64] = "";
    for (size_t i = 0; i < sizeof(folders) / sizeof(*folders); ++i) {
        char path[PATH_MAX]; struct stat st;
        snprintf(path, sizeof(path), "%s/%s", documents, folders[i]);
        if (stat(path, &st) || !S_ISDIR(st.st_mode)) continue;
        CFURLRef url = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)path, (CFIndex)strlen(path), true);
        if (!url) continue;
        CFTypeRef value = NULL;
        int excluded = CFURLCopyResourcePropertyForKey(url, kCFURLIsExcludedFromBackupKey, &value, NULL) && value &&
                       CFBooleanGetValue((CFBooleanRef)value);
        if (value) CFRelease(value);
        char *list = excluded ? already : done;
        CFErrorRef error = NULL;
        if (!excluded && !CFURLSetResourcePropertyForKey(url, kCFURLIsExcludedFromBackupKey, kCFBooleanTrue, &error)) {
            fprintf(stderr, "[ios] sauvegarde iCloud : %s non exclu\n", folders[i]);
            list = NULL;
        }
        if (error) CFRelease(error);
        CFRelease(url);
        if (list) snprintf(list + strlen(list), sizeof(done) - strlen(list), "%s%s", list[0] ? ", " : "", folders[i]);
    }
    if (done[0] || already[0])
        fprintf(stderr, "[ios] sauvegarde iCloud : exclus %s%s%s%s, %s gardé\n", done,
                done[0] && already[0] ? " ; " : "", already[0] ? "déjà exclus " : "", already, rt_ios_current_game()->userdata);
}

int main(int argc, char **argv) {
    // tools/ios_device.sh (space, workshop push, mel data push) : place libre,
    // modèle et mémoire, écrits dans Documents/device-report.txt, puis sortie
    // sans démarrer le jeu (devicectl ne donne que la capacité totale).
    if (getenv("P2_DEVICE_REPORT") && getenv("HOME")) {
        char documents[PATH_MAX], report[PATH_MAX];
        snprintf(documents, sizeof(documents), "%s/Documents", getenv("HOME"));
        mkdir(documents, 0755);
        snprintf(report, sizeof(report), "%s/device-report.txt", documents);
        return rt_ios_device_report_write(report, documents) ? 0 : 1;
    }
    // Réglages de l'app Réglages d'iOS : avant tout le reste (P2Log pose P2_LOG_FILE).
    // App Portal (plusieurs jeux, runtime/ios/game_picker_ios.m) : le jeu est
    // choisi avant tout le reste, qui en dépend (réglages, dossiers, Steam).
#ifdef RT_MULTI_GAME
    {
        const char *chosen = rt_ios_choose_game();
        rt_ios_set_game(chosen);
        if (!rt_select_game_code(chosen)) return 3;
    }
#endif
    // Défauts de cet appareil (ios/device_defaults.txt) : au premier lancement
    // seulement, et jamais sur un réglage que le joueur a choisi.
    char device[512];
    rt_ios_device_defaults_first_launch(device, sizeof(device));
    char settings[1024];
    int applied = rt_ios_settings_load(settings, sizeof(settings));
    // Journal fiable : la console de simctl perd la fin quand le processus meurt.
    const char *log = getenv("P2_LOG_FILE");   // relatif : au conteneur de l'app (HOME)
    char log_path[PATH_MAX];
    if (log && *log && *log != '/' && getenv("HOME")) {
        snprintf(log_path, sizeof(log_path), "%s/%s", getenv("HOME"), log);
        log = log_path;
    }
    if (log && *log && freopen(log, "w", stderr)) {
        dup2(fileno(stderr), fileno(stdout));
        setvbuf(stderr, NULL, _IONBF, 0);
    }
    // Le moteur garde ouvertes ses archives VPK (172 pour Portal 2) : la limite
    // de 256 descripteurs d'une app iOS ne suffit pas (ReadFile échoue alors).
    struct rlimit files;
    if (!getrlimit(RLIMIT_NOFILE, &files)) {
        rlim_t wanted = files.rlim_max == RLIM_INFINITY || files.rlim_max > 10240 ? 10240 : files.rlim_max;
        if (files.rlim_cur < wanted) { files.rlim_cur = wanted; setrlimit(RLIMIT_NOFILE, &files); }
        getrlimit(RLIMIT_NOFILE, &files);
        fprintf(stderr, "[ios] descripteurs de fichiers : %llu\n", (unsigned long long)files.rlim_cur);
    }
    fprintf(stderr, "[ios] appareil : %s\n", device);
    fprintf(stderr, "[ios] réglages : %s\n", applied ? settings : "aucun modifié");
    // Garde mémoire (4 Go) : plafonds de mtld3d plus bas, ajoutés après ceux
    // de l'utilisateur pour l'emporter (la dernière clé gagne).
    if (rt_ios_device_low_memory() && *rt_ios_device_low_memory_metal()) {
        const char *given = getenv("MTLD3D_CONFIG");
        char joined[1024];
        snprintf(joined, sizeof(joined), "%s%s%s", given && *given ? given : "", given && *given ? ";" : "",
                 rt_ios_device_low_memory_metal());
        setenv("MTLD3D_CONFIG", joined, 1);
        fprintf(stderr, "[ios] mémoire réduite : mtld3d %s, jeu %s\n", rt_ios_device_low_memory_metal(),
                rt_ios_device_low_memory_args());
    }
    const rt_ios_game *game = rt_ios_current_game();
    fprintf(stderr, "[ios] jeu : %s (mod %s, %s)\n", game->name, game->mod, game->userdata);
    // Tapes au dos coupées par défaut dans HL2 (game_ios.c) ; P2_BACKTAP=1 les rend.
    if (!game->back_taps) setenv("P2_BACKTAP", "0", 0);
    // Rendu Metal (P2_RENDERER=d3d9, Réglages > Rendu) : vérifié avant que
    // src/main.c initialise libd3d9metal ; OpenGL sinon.
    rt_ios_metal_prepare();
    char exe[PATH_MAX], bundle[PATH_MAX];
    uint32_t size = sizeof(exe);
    if (_NSGetExecutablePath(exe, &size)) return 2;
    snprintf(bundle, sizeof(bundle), "%s", exe);
    char *slash = strrchr(bundle, '/');
    if (slash) *slash = 0;
    // Stub Steam du build iOS (runtime/ios/steam_stub.c) : lié dans l'exécutable.
    setenv("P2_STEAM_API_DYLIB", exe, 0);
    setenv("SteamAppId", game->app_id, 0);
    setenv("SteamGameId", game->app_id, 0);
    push(argc > 0 ? argv[0] : game->name);
    if (argc > 1) for (int i = 1; i < argc; ++i) push(argv[i]);
    else {
        char bundled[64];
        snprintf(bundled, sizeof(bundled), game == rt_ios_app_game() ? "launch.args" : "launch-%s.args", game->name);
        read_launch_file(bundle, bundled);
        // Documents/launch.args (facultatif) : arguments du jeu ajoutés, par
        // exemple « +map sp_a2_laser_intro » pour démarrer dans une salle.
        if (getenv("HOME")) {
            char documents[PATH_MAX];
            snprintf(documents, sizeof(documents), "%s/Documents", getenv("HOME"));
            if (game->data_dir[0]) {
                char own[PATH_MAX];
                snprintf(own, sizeof(own), "%s/%s", documents, game->data_dir);
                read_launch_file(own, "launch.args");
            } else read_launch_file(documents, "launch.args");
        }
    }
    // Options du runtime absentes : dossiers de Documents (sur appareil, les
    // données du jeu y sont copiées ; voir tools/ios_device.sh). Elles
    // précèdent « -- » : insérées en tête.
    if (getenv("HOME")) {
        char documents[PATH_MAX];
        snprintf(documents, sizeof(documents), "%s/Documents", getenv("HOME"));
        mkdir(documents, 0755);
        static const struct { const char *option, *folder; int create; } defaults[] = {
            {"--user-dir", NULL, 1}, {"--images", "prep", 0},
            {"--game-dir", "game", 0}, {"--linux-root", "linux", 0}};
        for (size_t d = 0; d < sizeof(defaults) / sizeof(*defaults) && count + 2 < 256; ++d) {
            int present = 0;
            for (int i = 1; i < count; ++i) present |= !strcmp(arguments[i], defaults[d].option);
            char path[PATH_MAX]; struct stat st;
            // Données d'un jeu hébergé dans Documents/<data_dir> (celles de Portal 2 à
            // la racine, comme toujours) ; sauvegardes dans Documents/<userdata>.
            if (defaults[d].folder && game->data_dir[0])
                snprintf(path, sizeof(path), "%s/%s/%s", documents, game->data_dir, defaults[d].folder);
            else snprintf(path, sizeof(path), "%s/%s", documents, defaults[d].folder ? defaults[d].folder : game->userdata);
            if (present) continue;
            if (defaults[d].create) mkdir(path, 0755);
            else if (stat(path, &st) || !S_ISDIR(st.st_mode)) continue;
            memmove(arguments + 3, arguments + 1, (size_t)(count - 1) * sizeof(*arguments));
            arguments[1] = strdup(defaults[d].option); arguments[2] = strdup(path);
            count += 2;
        }
        exclude_from_backup(documents);
    }
    // Échelle de rendu choisie (Réglages ou P2_IOS_RENDER_SCALE) : la
    // résolution du jeu suit, sinon video.txt garderait l'ancienne. HL2 : toujours
    // (sinon il démarre en 640×480, étiré sur l'écran).
    if ((getenv("P2_IOS_RENDER_SCALE") || strcmp(game->name, "portal2")) && count + 4 < 256) {
        int present = 0;
        for (int i = 1; i < count; ++i) present |= !strcmp(arguments[i], "-w") || !strcmp(arguments[i], "-width");
        int w = 0, h = 0;
        if (!present) rt_ios_render_size(&w, &h);
        if (w > 0 && h > 0) {
            char value[16];
            push("-w"); snprintf(value, sizeof(value), "%d", w); push(value);
            push("-h"); snprintf(value, sizeof(value), "%d", h); push(value);
            fprintf(stderr, "[ios] résolution %dx%d (échelle %s)\n", w, h,
                    getenv("P2_IOS_RENDER_SCALE") ? getenv("P2_IOS_RENDER_SCALE") : "par défaut");
        }
    }
    // Garde mémoire : réglages du moteur qui économisent la mémoire (textures
    // à demi-résolution : +mat_picmip 1), après ceux du joueur.
    if (rt_ios_device_low_memory()) {
        char words[256];
        snprintf(words, sizeof(words), "%s", rt_ios_device_low_memory_args());
        for (char *save = NULL, *w = strtok_r(words, " \t", &save); w; w = strtok_r(NULL, " \t", &save)) push(w);
    }
    arguments[count] = NULL;
    fprintf(stderr, "[ios] %d arguments, Steam : stub iOS (%s)\n", count, exe);
    return p2_main(count, arguments);
}
