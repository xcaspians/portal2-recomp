// Profils de jeu du build iOS (voir game_ios.h). La table suit tools/games.py
// (mod, appid) ; tests/checks_touch_layout.c et tests/test_games.py vérifient
// la concordance.
#include "game_ios.h"
#include <TargetConditionals.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#if TARGET_OS_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#endif

static const rt_ios_game games[] = {
    // Le dossier « portal2-userdata » reste celui des installations existantes.
    // ICvar : IAppSystem à 8 entrées, puis AllocateDLLIdentifier, Register/
    // UnregisterConCommand, UnregisterConCommands, GetCommandLineValue.
    {"portal2", "portal2", "portal2-userdata", "620", 1, RT_IOS_LAYOUT_PORTAL2, "VEngineCvar007", 13, "", "Portal 2"},
    // Tapes au dos coupées : elles ne donnent qu'un clic bref, alors que les
    // armes automatiques de HL2 se tirent en tenant le bouton, et un tir
    // involontaire coûte des munitions ou réveille les ennemis. P2_BACKTAP=1
    // les rend (gauche : tir, droite : tir secondaire).
    // ICvar de Source 2013 : IAppSystem à 5 entrées (Connect, Disconnect,
    // QueryInterface, Init, Shutdown), donc FindCommandBase en 10 ; vérifié
    // dans libvstdlib.so du dépôt 224 (table dont l'entrée 2 compare le nom à
    // « VEngineCvar004 », 5 incrémente un compteur, 10 et 11 parcourent la
    // liste des commandes par GetCommands, 16 et 17).
    {"hl2", "hl2_complete", "hl2-userdata", "220", 0, RT_IOS_LAYOUT_HL2, "VEngineCvar004", 10, "", "Half-Life 2"},
    // Portal 1 : même Source 2013 que Half-Life 2 (ICvar VEngineCvar004, base 10).
    // La disposition tactile de Portal 2 convient : elle a les deux boutons de
    // portail, que le pistolet double de Portal 1 utilise aussi. Tapes au dos :
    // le tir du portail, comme Portal 2.
    {"portal1", "portal", "portal-userdata", "400", 1, RT_IOS_LAYOUT_PORTAL2, "VEngineCvar004", 10, "", "Portal"},
    // Portal Stories: Mel : son propre moteur Portal 2 (version Linux de 2021),
    // mêmes interfaces que Portal 2 (VEngineCvar007, VEngineClient015 et
    // VEngineVGui001 dans ses engine.so et libvstdlib.so) ; joue comme Portal 2
    // (disposition tactile, tapes au dos). Sauvegardes à part : mel-userdata.
    // Hébergé dans l'app Portal : données dans Documents/mel (celles de Portal 2
    // restent à la racine de Documents).
    {"mel", "portal_stories", "mel-userdata", "317400", 1, RT_IOS_LAYOUT_PORTAL2, "VEngineCvar007", 13, "mel",
     "Portal Stories: Mel"},
};

const rt_ios_game *rt_ios_game_named(const char *name) {
    for (size_t i = 0; name && i < sizeof(games) / sizeof(*games); ++i)
        if (!strcmp(games[i].name, name)) return &games[i];
    return &games[0];
}

static const rt_ios_game *current;
const rt_ios_game *rt_ios_set_game(const char *name) { return current = rt_ios_game_named(name); }

const rt_ios_game *rt_ios_app_game(void) {
    static const rt_ios_game *app;
    if (app) return app;
#if TARGET_OS_IPHONE
    char name[32] = "";
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFTypeRef value = bundle ? CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("P2Game")) : NULL;
    if (value && CFGetTypeID(value) == CFStringGetTypeID())
        CFStringGetCString((CFStringRef)value, name, sizeof(name), kCFStringEncodingUTF8);
    app = rt_ios_game_named(name);
#else
    app = rt_ios_game_named(NULL);
#endif
    return app;
}

const char *rt_ios_pref_key(const char *key, char *out, unsigned long size) {
    const rt_ios_game *game = rt_ios_current_game();
    if (game == rt_ios_app_game()) snprintf(out, size, "%s", key);
    else snprintf(out, size, "%s.%s", game->name, key);
    return out;
}

const rt_ios_game *rt_ios_current_game(void) { return current ? current : (current = rt_ios_app_game()); }
