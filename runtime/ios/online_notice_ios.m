// Round 24 bug 3: the notice that Play Online can't work on iOS (online_notice.h).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include "env_ios.h"
#include "online_notice.h"
#include "settings_menu_ios.h"

static rt_online_wait wait_state;

static UIViewController *top_controller(void) {
    for (UIScene *scene in UIApplication.sharedApplication.connectedScenes) {
        if (![scene isKindOfClass:UIWindowScene.class]) continue;
        for (UIWindow *w in ((UIWindowScene *)scene).windows) {
            if (!w.isKeyWindow || !w.rootViewController) continue;
            UIViewController *vc = w.rootViewController;
            while (vc.presentedViewController) vc = vc.presentedViewController;
            return vc;
        }
    }
    return nil;
}
// Back: the game's lobby requests get Steam's failure answer (steam_stub.c), and the
// game's own code leaves "Searching for Friends..." ("Failed to create session", OK).
void steam_stub_online_give_up(void);   // steam_stub.c
static void go_back(void) {
    steam_stub_online_give_up();
    fprintf(stderr, "[ios] jeu en ligne : retour\n");
}
static void show_notice(void) {
    UIViewController *vc = top_controller();
    fprintf(stderr, "[ios] jeu en ligne : pas de Steam sur iOS, explication affichée (%s)\n", vc ? "ok" : "pas de fenêtre");
    if (!vc) return;
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@(rt_online_title())
                                                                   message:@(rt_online_message())
                                                            preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:@"Back" style:UIAlertActionStyleDefault handler:^(UIAlertAction *a) {
        (void)a; go_back();
    }]];
    // Round 26: straight to Play nearby (the gear menu's panel, settings_menu_ios.m).
    [alert addAction:[UIAlertAction actionWithTitle:@"Play nearby" style:UIAlertActionStyleDefault handler:^(UIAlertAction *a) {
        (void)a; go_back();
        fprintf(stderr, "[ios] jeu en ligne : vers Play nearby\n");
        [NSNotificationCenter.defaultCenter postNotificationName:@"P2ShowNearby" object:nil];
    }]];
    alert.preferredAction = alert.actions.lastObject;
    [vc presentViewController:alert animated:YES completion:nil];
    double ok = rt_ios_env_float("P2_ONLINE_NOTICE_TEST_OK", 0, 0);   // tests: press Back after this many seconds
    double nearby = rt_ios_env_float("P2_ONLINE_NOTICE_TEST_NEARBY", 0, 0);   // tests: press Play nearby after this many
    if (nearby > 0)
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(nearby * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [alert dismissViewControllerAnimated:YES completion:^{
                go_back();
                [NSNotificationCenter.defaultCenter postNotificationName:@"P2ShowNearby" object:nil];
            }];
        });
    if (ok > 0)
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(ok * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [alert dismissViewControllerAnimated:YES completion:^{ go_back(); }];
        });
}

// Called by the Steam stub (any thread) when the game asks for a lobby.
// Portal 2 only (steam_stub.c calls it for app 620): its co-op menu is the one that waits.
void steam_stub_online_requested(const char *what) {
    fprintf(stderr, "[ios] jeu en ligne : %s demandé, Steam absent sur iOS\n", what);
    dispatch_async(dispatch_get_main_queue(), ^{
        double timeout = rt_ios_env_float("P2_ONLINE_TIMEOUT", RT_ONLINE_DEFAULT_TIMEOUT, 0);
        rt_online_asked(&wait_state, CACurrentMediaTime());
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)((timeout + .3) * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            if (rt_online_due(&wait_state, CACurrentMediaTime(), timeout, rt_ios_game_in_level())) show_notice();
        });
    });
}
// P2_ONLINE_NOTICE_TEST_ASK=<s> (device checks, round 26): the game's lobby request,
// as Play Online makes it, that many seconds after the launch.
__attribute__((constructor)) static void online_test_ask(void) {
    const char *e = getenv("P2_ONLINE_NOTICE_TEST_ASK");
    if (!e) return;
    double after = atof(e);
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(after * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        steam_stub_online_requested("essai : salon");
    });
}
#endif
