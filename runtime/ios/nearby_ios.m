// Round 25: Play nearby, Portal 2 co-op between devices on the same Wi-Fi
// (rules in nearby.c, nearby.h).
//
// - Host: from the launcher, Portal 2 starts with "+sv_lan 1 +map mp_coop_lobby_3";
//   from the gear menu, the same two commands go to the engine. Once the lobby
//   is up, Sauce advertises `_sauce-coop._tcp` under the device's Bonjour name
//   and shows "Waiting for a friend…" with Cancel until a second player is on
//   the server.
// - Join: Sauce browses for that service (the same DNS-SD API the phone
//   controller uses for its .local name), resolves each host to an IPv4 address
//   and lists it by name; one tap starts the game with "+connect <address>" (or
//   sends "connect" from the gear menu).
// - Both sides then ask the game server how many players it has (A2S_INFO,
//   every 2 s, its own UDP socket): the host sees its friend leave, the guest a
//   host that stopped answering. Either way a plain message, and "disconnect"
//   brings the game back to its menu.
// - iOS asks once for local network access (NSLocalNetworkUsageDescription,
//   NSBonjourServices in Info.plist). Declined: the panel says so and how to
//   turn it on again, with a button to the app's page in Settings.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <GameController/GameController.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#include <arpa/inet.h>
#include <dns_sd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "env_ios.h"
#include "nearby.h"
#include "nearby_ios.h"

int rt_ios_game_in_level(void);            // game_state.c
int rt_ios_game_commands_available(void);
int rt_ios_game_command(const char *line);
int rt_env_flag(const char *name);         // rt.h

static dispatch_queue_t queue;             // every DNS-SD callback and the list
static const char *kGame = "portal2";
static NSString *const kChanged = @"P2NearbyChanged";

// ---- Discovery (queue) ----
typedef struct {
    int used;
    char name[64], type[64], domain[64], target[256];
    uint32_t iface;
    uint16_t port;                          // host order
    char address[64];                       // "192.168.1.21:27015" once resolved
    DNSServiceRef resolve, addr;
} service;
static service services[RT_NEARBY_MAX + 8];
static rt_nearby_list list;                 // what the panel shows
static DNSServiceRef browse_ref;
static int denied;                          // local network access declined (or not declared)
static int browsing;

static void changed(void) {
    dispatch_async(dispatch_get_main_queue(), ^{ [NSNotificationCenter.defaultCenter postNotificationName:kChanged object:nil]; });
}
static int is_denied(DNSServiceErrorType e) { return e == kDNSServiceErr_PolicyDenied || e == kDNSServiceErr_NoAuth; }
static void note_error(const char *what, DNSServiceErrorType e) {
    fprintf(stderr, "[ios] jeu à proximité : %s, erreur %d%s\n", what, e, is_denied(e) ? " (accès au réseau local refusé)" : "");
    if (is_denied(e) && !denied) { denied = 1; changed(); }
}
static service *find_service(const char *name) {
    for (size_t i = 0; i < sizeof(services) / sizeof(*services); ++i)
        if (services[i].used && !strcmp(services[i].name, name)) return &services[i];
    return NULL;
}
static void drop_service(service *s) {
    if (s->resolve) DNSServiceRefDeallocate(s->resolve);
    if (s->addr) DNSServiceRefDeallocate(s->addr);
    memset(s, 0, sizeof(*s));
}
static void got_address(DNSServiceRef ref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType e, const char *host,
                        const struct sockaddr *sa, uint32_t ttl, void *ctx) {
    (void)ref; (void)flags; (void)iface; (void)host; (void)ttl;
    service *s = ctx;
    if (e) { note_error("adresse", e); return; }
    if (!s->used || !sa || sa->sa_family != AF_INET) return;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((const struct sockaddr_in *)sa)->sin_addr, ip, sizeof(ip));
    snprintf(s->address, sizeof(s->address), "%s:%u", ip, s->port);
    fprintf(stderr, "[ios] jeu à proximité : « %s » à %s\n", s->name, s->address);
    changed();
}
static void resolved(DNSServiceRef ref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType e, const char *fullname,
                     const char *target, uint16_t port, uint16_t txt_len, const unsigned char *txt, void *ctx) {
    (void)ref; (void)flags; (void)fullname;
    service *s = ctx;
    if (e) { note_error("résolution", e); return; }
    if (!s->used) return;
    s->port = ntohs(port);
    snprintf(s->target, sizeof(s->target), "%s", target);
    rt_nearby_add(&list, s->name, txt, txt_len, kGame);
    if (s->addr) DNSServiceRefDeallocate(s->addr);
    s->addr = NULL;
    DNSServiceErrorType a = DNSServiceGetAddrInfo(&s->addr, 0, iface, kDNSServiceProtocol_IPv4, target, got_address, s);
    if (!a) a = DNSServiceSetDispatchQueue(s->addr, queue);
    if (a) { note_error("adresse demandée", a); s->addr = NULL; }
    changed();
}
static void browsed(DNSServiceRef ref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType e, const char *name,
                    const char *type, const char *domain, void *ctx) {
    (void)ref; (void)ctx;
    if (e) { note_error("recherche", e); return; }
    if (denied) { denied = 0; changed(); }   // allowed after all (the prompt was answered)
    if (!(flags & kDNSServiceFlagsAdd)) {
        service *s = find_service(name);
        if (s) drop_service(s);
        if (rt_nearby_remove(&list, name)) {
            fprintf(stderr, "[ios] jeu à proximité : « %s » ne joue plus\n", name);
            changed();
        }
        return;
    }
    if (!strcmp(name, list.own) || find_service(name)) return;   // ourselves, or seen on another interface
    service *s = NULL;
    for (size_t i = 0; i < sizeof(services) / sizeof(*services) && !s; ++i)
        if (!services[i].used) s = &services[i];
    if (!s) return;
    s->used = 1;
    snprintf(s->name, sizeof(s->name), "%s", name);
    snprintf(s->type, sizeof(s->type), "%s", type);
    snprintf(s->domain, sizeof(s->domain), "%s", domain);
    s->iface = iface;
    fprintf(stderr, "[ios] jeu à proximité : trouvé « %s »\n", name);
    if (rt_nearby_add(&list, name, NULL, 0, kGame)) changed();
    DNSServiceErrorType r = DNSServiceResolve(&s->resolve, 0, iface, name, type, domain, resolved, s);
    if (!r) r = DNSServiceSetDispatchQueue(s->resolve, queue);
    if (r) { note_error("résolution demandée", r); s->resolve = NULL; }
}
static void start_queue(void) {
    if (!queue) queue = dispatch_queue_create("nearby", DISPATCH_QUEUE_SERIAL);
}
void rt_ios_nearby_browse(int on) {
    start_queue();
    dispatch_async(queue, ^{
        if (on == browsing) return;
        browsing = on;
        if (!on) {
            if (browse_ref) DNSServiceRefDeallocate(browse_ref);
            browse_ref = NULL;
            for (size_t i = 0; i < sizeof(services) / sizeof(*services); ++i) if (services[i].used) drop_service(&services[i]);
            char own[64];
            memcpy(own, list.own, sizeof(own));
            memset(&list, 0, sizeof(list));
            memcpy(list.own, own, sizeof(own));
            changed();
            return;
        }
        // Device checks of the declined message, without resetting the device's privacy settings.
        if (getenv("P2_NEARBY_TEST_DENIED")) { note_error("essai : accès refusé", kDNSServiceErr_PolicyDenied); browsing = 0; return; }
        // The first local-network use: iOS shows its one-time prompt here.
        DNSServiceErrorType e = DNSServiceBrowse(&browse_ref, 0, kDNSServiceInterfaceIndexAny, RT_NEARBY_SERVICE, NULL, browsed, NULL);
        if (!e) e = DNSServiceSetDispatchQueue(browse_ref, queue);
        if (e) { note_error("recherche impossible", e); browse_ref = NULL; browsing = 0; }
        else fprintf(stderr, "[ios] jeu à proximité : recherche des hôtes\n");
    });
}

// ---- Advertising (queue) ----
static DNSServiceRef advert;
static char own_name[64];
static void registered(DNSServiceRef ref, DNSServiceFlags flags, DNSServiceErrorType e, const char *name, const char *type,
                       const char *domain, void *ctx) {
    (void)ref; (void)flags; (void)type; (void)domain; (void)ctx;
    if (e) { note_error("annonce", e); return; }
    snprintf(own_name, sizeof(own_name), "%s", name);
    snprintf(list.own, sizeof(list.own), "%s", name);
    rt_nearby_remove(&list, name);
    fprintf(stderr, "[ios] jeu à proximité : annoncé sous « %s »\n", name);
    changed();
}
static void advertise(int on, int port) {
    start_queue();
    dispatch_async(queue, ^{
        if (advert) { DNSServiceRefDeallocate(advert); advert = NULL; fprintf(stderr, "[ios] jeu à proximité : annonce retirée\n"); }
        if (!on) return;
        unsigned char txt[64];
        size_t n = rt_nearby_txt_build(txt, sizeof(txt), kGame);
        // No name: mDNSResponder uses the device's own (renamed on a clash, "… (2)").
        DNSServiceErrorType e = DNSServiceRegister(&advert, 0, kDNSServiceInterfaceIndexAny, NULL, RT_NEARBY_SERVICE, NULL, NULL,
                                                   htons((uint16_t)port), (uint16_t)n, txt, registered, NULL);
        if (!e) e = DNSServiceSetDispatchQueue(advert, queue);
        if (e) { note_error("annonce impossible", e); advert = NULL; }
    });
}
const char *rt_ios_nearby_own_name(void) { return own_name; }

// ---- Asking the game server (its own queue: blocking UDP with a short timeout) ----
static dispatch_queue_t probe_queue;
static dispatch_source_t probe_timer;
static char probe_host[64];
static int probe_port;
static rt_nearby_session session;
static double now_s(void) { return CACurrentMediaTime(); }
// One A2S_INFO round trip: players, or -1 without an answer.
static int ask_server(const char *ip, int port, char *map, size_t map_size) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;
    struct timeval tv = {1, 200000};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    struct sockaddr_in to = {0};
    to.sin_len = sizeof(to); to.sin_family = AF_INET; to.sin_port = htons((uint16_t)port);
    inet_pton(AF_INET, ip, &to.sin_addr);
    unsigned char q[32], r[1400];
    size_t qn = rt_nearby_info_query(q, sizeof(q));
    int players = -1, max = 0;
    if (sendto(fd, q, qn, 0, (struct sockaddr *)&to, sizeof(to)) == (ssize_t)qn) {
        ssize_t n = recv(fd, r, sizeof(r), 0);
        if (n <= 0 || !rt_nearby_parse_info(r, (size_t)n, &players, &max, map, map_size)) players = -1;
    }
    close(fd);
    return players;
}
static void probe(const char *ip, int port) {
    if (!probe_queue) probe_queue = dispatch_queue_create("nearby-probe", DISPATCH_QUEUE_SERIAL);
    if (probe_timer) { dispatch_source_cancel(probe_timer); probe_timer = nil; }
    if (!ip) return;
    snprintf(probe_host, sizeof(probe_host), "%s", ip);
    probe_port = port;
    probe_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, probe_queue);
    dispatch_source_set_timer(probe_timer, DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC, NSEC_PER_SEC / 4);
    dispatch_source_set_event_handler(probe_timer, ^{
        static int last = -2;
        char map[64] = "";
        int players = ask_server(probe_host, probe_port, map, sizeof(map));
        if (players != last) fprintf(stderr, "[ios] jeu à proximité : serveur %s:%d, %d joueur(s) %s\n", probe_host, probe_port, players, map);
        last = players;
        if (players < 0) return;
        dispatch_async(dispatch_get_main_queue(), ^{ rt_nearby_heard(&session, now_s(), players); });
    });
    dispatch_resume(probe_timer);
}

// This device's Wi-Fi (or wired) IPv4 address: the game server answers there, not
// on 127.0.0.1 (measured on the iPad Pro, round 25). "127.0.0.1" if none.
static void own_address(char *out, size_t size) {
    struct ifaddrs *all = NULL, *it;
    snprintf(out, size, "127.0.0.1");
    if (getifaddrs(&all)) return;
    for (it = all; it; it = it->ifa_next) {
        if (!it->ifa_addr || it->ifa_addr->sa_family != AF_INET || !(it->ifa_flags & IFF_UP) || strncmp(it->ifa_name, "en", 2)) continue;
        inet_ntop(AF_INET, &((struct sockaddr_in *)it->ifa_addr)->sin_addr, out, (socklen_t)size);
        break;
    }
    freeifaddrs(all);
}

// ---- Launch (main thread, before the game) ----
static char launch_args[160];
const char *rt_ios_nearby_launch_args(void) { return launch_args[0] ? launch_args : NULL; }
static char join_address[64];
static int pending_role;   // 1 host, 2 join: the session starts with the game
static void begin_host(int in_game) {
    rt_nearby_start_host(&session, now_s());
    if (!in_game) rt_nearby_stop(&session);   // from the launcher: the clock starts once the engine is up (tick)
    fprintf(stderr, "[ios] jeu à proximité : héberger (%s)\n", in_game ? "commande" : "lancement");
    if (in_game) {
        rt_ios_game_command("sv_lan 1");
        rt_ios_game_command("map " RT_NEARBY_LOBBY);
    } else snprintf(launch_args, sizeof(launch_args), "+sv_lan 1 +map %s", RT_NEARBY_LOBBY);
}
static void begin_join(int in_game, const char *name, const char *address) {
    rt_nearby_start_join(&session, now_s(), name);
    if (!in_game) rt_nearby_stop(&session);   // as for hosting
    snprintf(join_address, sizeof(join_address), "%s", address);
    fprintf(stderr, "[ios] jeu à proximité : rejoindre « %s » à %s (%s)\n", name, address, in_game ? "commande" : "lancement");
    char ip[64];
    snprintf(ip, sizeof(ip), "%s", address);
    char *colon = strrchr(ip, ':');
    int port = colon ? atoi(colon + 1) : 27015;
    if (colon) *colon = 0;
    probe(ip, port);
    if (in_game) {
        char line[96];
        snprintf(line, sizeof(line), "connect %s", address);
        rt_ios_game_command(line);
    } else snprintf(launch_args, sizeof(launch_args), "+connect %s", address);
}
static int in_game(void) { return rt_ios_game_commands_available(); }

// ---- Messages (main thread) ----
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
// Back to the game's main menu. "disconnect" alone left Portal 2's pause-menu
// panel on screen over the main menu's background when the game had its menu up
// (the guest, its host gone: measured on the iPad Pro); hidden first, the game
// shows its main menu afresh once the level is closed.
static void leave_game(void) {
    rt_ios_game_command("gameui_hide");
    rt_ios_game_command("disconnect");
}
static UIAlertController *waiting;
static void again(void);
static void test_after(const char *var, void (^block)(void)) {
    double s = rt_ios_env_float(var, 0, 0);
    if (s > 0) dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(s * NSEC_PER_SEC)), dispatch_get_main_queue(), block);
}
static void stop_all(void) {
    rt_nearby_stop(&session);
    advertise(0, 0);
    probe(NULL, 0);
    if (waiting) { [waiting dismissViewControllerAnimated:YES completion:nil]; waiting = nil; }
}
static void say(NSString *title, NSString *message) {
    UIViewController *vc = top_controller();
    fprintf(stderr, "[ios] jeu à proximité : message « %s »\n", title.UTF8String);
    if (!vc) return;
    UIAlertController *a = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
    [a addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *x) { (void)x; again(); }]];
    [vc presentViewController:a animated:YES completion:nil];
    test_after("P2_NEARBY_TEST_OK", ^{   // tests: OK pressed after that many seconds
        if (a.presentingViewController) [a dismissViewControllerAnimated:YES completion:^{ again(); }];
    });
}
static void cancel_hosting(void) {
    fprintf(stderr, "[ios] jeu à proximité : hébergement annulé\n");
    waiting = nil;
    stop_all();
    leave_game();
}
static void show_waiting(void) {
    UIViewController *vc = top_controller();
    if (!vc || waiting) return;
    NSString *me = own_name[0] ? [NSString stringWithFormat:@"“%s”", own_name] : @"this device";
    waiting = [UIAlertController alertControllerWithTitle:@"Waiting for a friend…"
                                                  message:[NSString stringWithFormat:@"On the other device (same Wi-Fi), open Play nearby, "
                                                           "tap Join, then tap %@.", me]
                                           preferredStyle:UIAlertControllerStyleAlert];
    [waiting addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction *x) {
        (void)x; cancel_hosting();
    }]];
    [vc presentViewController:waiting animated:YES completion:nil];
    UIAlertController *shown = waiting;
    test_after("P2_NEARBY_TEST_CANCEL", ^{
        if (waiting == shown) [shown dismissViewControllerAnimated:YES completion:^{ cancel_hosting(); }];
    });
}
// The name we advertise arrives a moment after the lobby: the waiting text follows.
static void refresh_waiting(void) {
    if (waiting && own_name[0])
        waiting.message = [NSString stringWithFormat:@"On the other device (same Wi-Fi), open Play nearby, tap Join, then tap “%s”.", own_name];
}

// Tests (P2_NEARBY_TEST_AGAIN=1): once a session has ended with a message, the
// same role again from inside the game (gear-menu path), without restarting.
static int last_role;
static void again(void) {
    static int done;
    if (done || !rt_env_flag("P2_NEARBY_TEST_AGAIN") || !in_game()) return;
    done = 1;
    fprintf(stderr, "[ios] essai jeu à proximité : encore une fois (%s)\n", last_role == 1 ? "héberger" : "rejoindre");
    if (last_role == 1) begin_host(1);
    else if (join_address[0]) {
        char peer[64], address[64];
        snprintf(peer, sizeof(peer), "%s", session.peer[0] ? session.peer : "host");
        snprintf(address, sizeof(address), "%s", join_address);
        begin_join(1, peer, address);
    }
}

// ---- Per frame (settings_menu_ios.m's tick) ----
void rt_ios_nearby_tick(void) {
    if (pending_role && in_game()) {   // started from the launcher: the session runs once the engine answers
        if (pending_role == 1) rt_nearby_start_host(&session, now_s());
        else { char peer[64]; snprintf(peer, sizeof(peer), "%s", session.peer); rt_nearby_start_join(&session, now_s(), peer); }
        pending_role = 0;
    }
    refresh_waiting();
    if (waiting) {   // a controller's B cancels the waiting message too
        for (GCController *c in GCController.controllers)
            if (c.extendedGamepad.buttonB.isPressed) { [waiting dismissViewControllerAnimated:YES completion:nil]; cancel_hosting(); break; }
    }
    rt_nearby_event e = rt_nearby_step(&session, now_s(), rt_ios_game_in_level());
    if (e == RT_NB_NONE) return;
    char text[400];
    static double leave_armed;
    switch (e) {
    case RT_NB_ADVERTISE:
        last_role = 1;
        advertise(1, 27015);
        { char self_ip[64]; own_address(self_ip, sizeof(self_ip)); probe(self_ip, 27015); }
        show_waiting();
        fprintf(stderr, "[ios] jeu à proximité : salon prêt, en attente d'un ami\n");
        return;
    case RT_NB_FRIEND_JOINED:
        if (waiting) { [waiting dismissViewControllerAnimated:YES completion:nil]; waiting = nil; }
        advertise(0, 0);   // full: no one else should try
        fprintf(stderr, "[ios] jeu à proximité : un ami a rejoint\n");
        leave_armed = 1;
        break;
    case RT_NB_JOINED:
        last_role = 2;
        fprintf(stderr, "[ios] jeu à proximité : connecté à « %s »\n", session.peer);
        leave_armed = 1;
        break;
    case RT_NB_ENDED:
        fprintf(stderr, "[ios] jeu à proximité : partie terminée\n");
        stop_all();
        return;
    default: {
        NSString *peer = @(session.peer);
        stop_all();
        leave_game();
        const char *m = rt_nearby_message(e, peer.UTF8String, text, sizeof(text));
        NSString *title = e == RT_NB_FRIEND_LEFT ? @"Your friend left" : e == RT_NB_HOST_LOST ? @"Connection lost"
                        : e == RT_NB_JOIN_FAILED ? @"Couldn't join" : @"Couldn't host";
        fprintf(stderr, "[ios] jeu à proximité : fin (%d) : %s\n", e, m ? m : "");
        if (m) say(title, @(m));
        return;
    }
    }
    // P2_NEARBY_TEST_LEAVE=<s>: this side leaves that long after playing together (the other side's message).
    if (leave_armed && rt_ios_env_float("P2_NEARBY_TEST_LEAVE", 0, 0) > 0) {
        leave_armed = 0;
        test_after("P2_NEARBY_TEST_LEAVE", ^{
            fprintf(stderr, "[ios] essai jeu à proximité : départ volontaire\n");
            stop_all();
            leave_game();
        });
    }
}

// ---- The panel: Host, Join and the hosts nearby ----
@interface P2NearbyPanel : UIView
@property (nonatomic, copy) void (^close)(void);
@property (nonatomic, copy) void (^started)(void);
@property (nonatomic, strong) id observer;
@end
@implementation P2NearbyPanel {
    UIStackView *stack, *hosts;
    UILabel *status;
    UIButton *settingsButton;
    BOOL joining, autoJoined;
}
- (void)dealloc {
    if (_observer) [NSNotificationCenter.defaultCenter removeObserver:_observer];
}
- (void)willMoveToWindow:(UIWindow *)window {
    if (!window) rt_ios_nearby_browse(0);
}
static UILabel *label(NSString *t, CGFloat size, UIFontWeight w, CGFloat alpha) {
    UILabel *l = [UILabel new];
    l.text = t; l.font = [UIFont systemFontOfSize:size weight:w];
    l.textColor = [UIColor colorWithWhite:1 alpha:alpha];
    l.numberOfLines = 0; l.textAlignment = NSTextAlignmentCenter;
    return l;
}
static UIButton *big_button(NSString *title, NSString *subtitle, NSString *symbol, BOOL filled, void (^action)(void)) {
    UIButtonConfiguration *c = filled ? [UIButtonConfiguration filledButtonConfiguration] : [UIButtonConfiguration grayButtonConfiguration];
    c.title = title; c.subtitle = subtitle;
    c.image = symbol ? [UIImage systemImageNamed:symbol] : nil;
    c.imagePadding = 10;
    c.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    c.contentInsets = NSDirectionalEdgeInsetsMake(14, 18, 14, 18);
    c.baseForegroundColor = filled ? UIColor.blackColor : UIColor.whiteColor;
    if (filled) c.baseBackgroundColor = [UIColor colorWithRed:.98 green:.55 blue:.16 alpha:1];
    UIButton *b = [UIButton buttonWithConfiguration:c primaryAction:[UIAction actionWithHandler:^(UIAction *a) { (void)a; action(); }]];
    b.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeading;
    return b;
}
- (void)build {
    self.backgroundColor = [UIColor colorWithWhite:.06 alpha:.97];
    self.layer.cornerRadius = 22;
    stack = [UIStackView new];
    stack.axis = UILayoutConstraintAxisVertical; stack.spacing = 12;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:stack];
    [NSLayoutConstraint activateConstraints:@[
        [stack.topAnchor constraintEqualToAnchor:self.topAnchor constant:22], [stack.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-20],
        [stack.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:22], [stack.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-22],
        [self.widthAnchor constraintEqualToConstant:420]]];
    UIStackView *top = [UIStackView new];
    top.alignment = UIStackViewAlignmentCenter;
    UILabel *title = label(@"Play nearby", 22, UIFontWeightBold, 1);
    title.textAlignment = NSTextAlignmentNatural;
    [top addArrangedSubview:title];
    UIButtonConfiguration *x = [UIButtonConfiguration plainButtonConfiguration];
    x.image = [UIImage systemImageNamed:@"xmark.circle.fill" withConfiguration:[UIImageSymbolConfiguration configurationWithPointSize:24]];
    x.baseForegroundColor = [UIColor colorWithWhite:1 alpha:.8];
    __weak P2NearbyPanel *weak = self;
    UIButton *closeButton = [UIButton buttonWithConfiguration:x primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; if (weak.close) weak.close();
    }]];
    closeButton.accessibilityLabel = @"Close";
    [closeButton setContentHuggingPriority:UILayoutPriorityRequired forAxis:UILayoutConstraintAxisHorizontal];
    [top addArrangedSubview:closeButton];
    [stack addArrangedSubview:top];
    [stack addArrangedSubview:label(@"Portal 2 co-op with a friend's iPhone, iPad or Mac on the same Wi-Fi. Each of you plays on your own screen.",
                                    14, UIFontWeightRegular, .7)];
    [self showChoice];
}
- (void)clear {
    while (stack.arrangedSubviews.count > 2) { UIView *v = stack.arrangedSubviews.lastObject; [stack removeArrangedSubview:v]; [v removeFromSuperview]; }
}
- (void)showChoice {
    [self clear];
    joining = NO;
    __weak P2NearbyPanel *weak = self;
    [stack addArrangedSubview:big_button(@"Host", @"Start the co-op lobby and wait for a friend", @"person.2.wave.2", YES, ^{ [weak host]; })];
    [stack addArrangedSubview:big_button(@"Join", @"Find a friend who is hosting", @"magnifyingglass", NO, ^{ [weak showJoin]; })];
    if (in_game() && rt_ios_game_in_level())
        [stack addArrangedSubview:label(@"Hosting or joining leaves this level; progress since your last save is lost.", 12,
                                        UIFontWeightRegular, .55)];
    const char *test = getenv("P2_NEARBY_TEST");
    if (test && !strcmp(test, "host")) dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{ [weak host]; });
    if (test && !strcmp(test, "join")) dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{ [weak showJoin]; });
}
- (void)host {
    fprintf(stderr, "[ios] jeu à proximité : bouton Héberger\n");
    rt_ios_nearby_browse(0);
    pending_role = in_game() ? 0 : 1;
    begin_host(in_game());
    if (self.started) self.started();
}
- (void)showJoin {
    [self clear];
    joining = YES;
    status = label(@"Looking for friends who are hosting…", 15, UIFontWeightSemibold, .85);
    [stack addArrangedSubview:status];
    hosts = [UIStackView new];
    hosts.axis = UILayoutConstraintAxisVertical; hosts.spacing = 8;
    [stack addArrangedSubview:hosts];
    __weak P2NearbyPanel *weak = self;
    UIButtonConfiguration *s = [UIButtonConfiguration grayButtonConfiguration];
    s.title = @"Open Settings";
    s.baseForegroundColor = UIColor.whiteColor;
    settingsButton = [UIButton buttonWithConfiguration:s primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a;
        [UIApplication.sharedApplication openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString] options:@{} completionHandler:nil];
    }]];
    settingsButton.hidden = YES;
    [stack addArrangedSubview:settingsButton];
    [stack addArrangedSubview:big_button(@"Back", nil, @"chevron.left", NO, ^{ rt_ios_nearby_browse(0); [weak showChoice]; })];
    [stack addArrangedSubview:label(@"The other device taps Play nearby > Host. It shows up here by its name.", 12, UIFontWeightRegular, .5)];
    if (!self.observer)
        self.observer = [NSNotificationCenter.defaultCenter addObserverForName:kChanged object:nil queue:NSOperationQueue.mainQueue
                                                                    usingBlock:^(NSNotification *n) { (void)n; [weak refresh]; }];
    rt_ios_nearby_browse(1);
    [self refresh];
}
- (void)refresh {
    if (!joining) return;
    static rt_nearby_list copy;                    // main thread only
    static service addresses[RT_NEARBY_MAX + 8];
    static int is_denied_now;
    dispatch_sync(queue, ^{ copy = list; memcpy(addresses, services, sizeof(services)); is_denied_now = denied; });
    for (UIView *v in hosts.arrangedSubviews) { [hosts removeArrangedSubview:v]; [v removeFromSuperview]; }
    NSString *app = [NSBundle.mainBundle objectForInfoDictionaryKey:@"CFBundleDisplayName"] ?: @"Sauce";
    settingsButton.hidden = !is_denied_now;
    if (is_denied_now) {
        status.text = @(rt_nearby_denied_message(app.UTF8String));
        return;
    }
    status.text = copy.count ? @"Tap a friend to join:" : @"Looking for friends who are hosting…";
    __weak P2NearbyPanel *weak = self;
    for (int i = 0; i < copy.count; ++i) {
        const rt_nearby_host *h = &copy.hosts[i];
        const char *address = "";
        for (size_t j = 0; j < sizeof(addresses) / sizeof(*addresses); ++j)
            if (addresses[j].used && !strcmp(addresses[j].name, h->name)) address = addresses[j].address;
        BOOL ready = h->usable && address[0];
        NSString *sub = !h->game[0] || (h->usable && !address[0]) ? @"Connecting…"
                      : !h->usable ? @"Another game or another version of Sauce" : @"Hosting Portal 2 co-op";
        NSString *name = @(h->name), *addr = @(address);
        UIButton *b = big_button(name, sub, @"ipad.landscape", ready, ^{ [weak join:name address:addr]; });
        b.enabled = ready;
        [hosts addArrangedSubview:b];
        const char *test = getenv("P2_NEARBY_TEST");
        if (ready && !autoJoined && test && !strcmp(test, "join")) {
            autoJoined = YES;
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC), dispatch_get_main_queue(), ^{ [weak join:name address:addr]; });
        }
    }
}
- (void)join:(NSString *)name address:(NSString *)address {
    fprintf(stderr, "[ios] jeu à proximité : bouton « %s »\n", name.UTF8String);
    rt_ios_nearby_browse(0);
    pending_role = in_game() ? 0 : 2;
    begin_join(in_game(), name.UTF8String, address.UTF8String);
    if (self.started) self.started();
}
@end

UIView *rt_ios_nearby_panel(void (^close)(void), void (^started)(void)) {
    start_queue();
    P2NearbyPanel *p = [P2NearbyPanel new];
    p.close = close;
    p.started = started;
    [p build];
    return p;
}
#endif
