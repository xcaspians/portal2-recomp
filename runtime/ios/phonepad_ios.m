// A phone as a controller, no app (round 23 item 9b; rules in phonepad.c).
//
// Started from gear menu > Phone controller, Sauce serves ios/phonepad (a page
// with Portal 2's touch layout, installable with "Add to Home Screen") on port
// P2_PHONEPAD_PORT (8017), on Wi-Fi and wired networks only, to local
// addresses only. The QR code carries the iPad's address and a fresh 4-digit
// code; the page then moves to sauce-XXXX.local, a name Sauce registers for
// itself with Bonjour (DNSServiceRegisterRecord), so a saved home-screen app
// survives the iPad's address changing. One phone at a time (Portal 2 co-op
// has two players): it becomes a full virtual controller (phone_pad), after
// any real controller, so it's player 2 in co-op with a controller or touch
// as player 1, and player 1 when nothing else is driving. "Kick phone" in the
// panel disconnects it and makes a new code. Pings measure the input lag over
// Wi-Fi, logged ("aller-retour").
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <CoreImage/CoreImage.h>
#import <Network/Network.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <SDL.h>
#include <arpa/inet.h>
#include <dns_sd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env_ios.h"
#include "phonepad.h"

void rt_ios_touch_sync_pads(void);   // touch_controls.m: hand the first slot over (or take it back)

static nw_listener_t listener;
static nw_connection_t phone;         // the connected controller, or nil
static dispatch_queue_t queue;
static char code[5];
static char ip[64], local_name[64];
static int port;
static SDL_Joystick *phone_pad;
static rt_phone_pad want, sent;   // what the phone holds, what SDL has (phonepad.c)
static _Atomic double last_heard;   // CACurrentMediaTime of the phone's last message (pings every second)
static dispatch_source_t watchdog;
static DNSServiceRef dns;
static double last_rtt = -1;
static char last_status[96] = "Not running";
static int phone_connected;

// A phone is (being) connected: the touch controls leave it the first free slot.
int rt_ios_phonepad_active(void) { return phone_connected; }

// ---- the controller (main thread: SDL's) ----
static SDL_Joystick *create_pad(void) {
    SDL_VirtualJoystickDesc desc = {0};
    desc.version = SDL_VIRTUAL_JOYSTICK_DESC_VERSION;
    desc.type = SDL_JOYSTICK_TYPE_GAMECONTROLLER;
    desc.naxes = SDL_CONTROLLER_AXIS_MAX;
    desc.nbuttons = SDL_CONTROLLER_BUTTON_DPAD_RIGHT + 1;
    desc.axis_mask = (1u << SDL_CONTROLLER_AXIS_MAX) - 1;
    desc.button_mask = (1u << (SDL_CONTROLLER_BUTTON_DPAD_RIGHT + 1)) - 1;
    desc.name = "Phone controller";
    int index = SDL_JoystickAttachVirtualEx(&desc);
    SDL_Joystick *pad = index < 0 ? NULL : SDL_JoystickOpen(index);
    if (index >= 0 && !pad) SDL_JoystickDetachVirtual(index);
    return pad;
}
static void pad_on(void) {
    if (phone_pad || !SDL_WasInit(SDL_INIT_GAMECONTROLLER)) return;
    phone_connected = 1;
    rt_ios_touch_sync_pads();   // the touch controls give up the first slot when nothing else holds it
    // Tests (P2_PHONEPAD_TEST_SLOT1=1): a stand-in for a real controller in the first slot,
    // so the phone is player 2, as with a controller as player 1.
    static SDL_Joystick *stand_in;
    if (!stand_in && getenv("P2_PHONEPAD_TEST_SLOT1")) {
        stand_in = create_pad();
        fprintf(stderr, "[ios] manette téléphone : manette témoin au premier rang (%s)\n", stand_in ? "ok" : SDL_GetError());
    }
    phone_pad = create_pad();
    rt_phonepad_pad_rest(&want);   // SDL's own rest: sticks centred, triggers at the minimum
    sent = want;
    fprintf(stderr, "[ios] manette téléphone : %s\n", phone_pad ? "branchée" : SDL_GetError());
}
static void pad_off(void) {
    phone_connected = 0;
    if (!phone_pad) return;
    SDL_JoystickID id = SDL_JoystickInstanceID(phone_pad);
    SDL_JoystickClose(phone_pad);
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        if (SDL_JoystickGetDeviceInstanceID(i) == id) { SDL_JoystickDetachVirtual(i); break; }
    phone_pad = NULL;
    rt_ios_touch_sync_pads();
    fprintf(stderr, "[ios] manette téléphone : débranchée\n");
}
// Everything that differs between what the phone holds and what SDL has.
static void sync_pad(void) {
    if (!phone_pad) return;
    for (int a = 0; a < 6; ++a)
        if (want.axes[a] != sent.axes[a]) { sent.axes[a] = want.axes[a]; SDL_JoystickSetVirtualAxis(phone_pad, a, want.axes[a]); }
    for (int b = 0; b < 32; ++b)
        if (((want.buttons ^ sent.buttons) >> b) & 1u)
            SDL_JoystickSetVirtualButton(phone_pad, b, (want.buttons >> b) & 1u ? SDL_PRESSED : SDL_RELEASED);
    sent.buttons = want.buttons;
}
// P2_PHONEPAD_TRACE=1: 0.2 s after each trigger change, what the game reads (tests).
static void trace_triggers(const char *why) {
    static int on = -1;
    if (on < 0) on = getenv("P2_PHONEPAD_TRACE") != NULL;
    if (!on) return;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC / 5), dispatch_get_main_queue(), ^{
        SDL_GameController *gc = phone_pad ? SDL_GameControllerFromInstanceID(SDL_JoystickInstanceID(phone_pad)) : NULL;
        int slot = -1;
        for (int i = 0; i < SDL_NumJoysticks(); ++i)
            if (SDL_JoystickGetDeviceInstanceID(i) == SDL_JoystickInstanceID(phone_pad)) slot = i;
        if (gc) fprintf(stderr, "[ios] manette téléphone : gâchettes lues par le jeu %d / %d (%s ; rang %d, joueur SDL %d)\n",
                        SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT),
                        SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT), why, slot,
                        SDL_GameControllerGetPlayerIndex(gc));
    });
}
static void apply(const rt_phone_message *m) {
    if (!phone_pad) return;
    rt_phonepad_pad_apply(&want, m);
    sync_pad();
    if (m->trigger >= 0) trace_triggers(m->down ? "appui" : "relâché");
}
// Round 24 safety net: let go of everything the phone holds (quiet phone, the
// iPad's app going inactive, the page hidden).
static void release_all(const char *why) {
    if (!phone_pad || !rt_phonepad_pad_held(&want)) return;
    rt_phonepad_pad_rest(&want);
    sync_pad();
    fprintf(stderr, "[ios] manette téléphone : tout relâché (%s)\n", why);
    trace_triggers(why);
}
static void start_watchdog(void) {
    if (watchdog) return;
    double quiet = rt_ios_env_float("P2_PHONEPAD_QUIET", 2.5f, 0.5f);   // seconds without a message (pings: 1 s)
    watchdog = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
    dispatch_source_set_timer(watchdog, DISPATCH_TIME_NOW, NSEC_PER_SEC / 4, NSEC_PER_SEC / 20);
    dispatch_source_set_event_handler(watchdog, ^{
        if (phone_pad && CACurrentMediaTime() - last_heard > quiet) release_all("plus de nouvelles du téléphone");
    });
    dispatch_resume(watchdog);
    [NSNotificationCenter.defaultCenter addObserverForName:UIApplicationWillResignActiveNotification object:nil
                                                     queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification *n) {
        (void)n; release_all("Sauce en arrière-plan");
    }];
}

// ---- the network ----
static void send_text(nw_connection_t c, const char *text) {
    uint8_t frame[256];
    size_t n = rt_phonepad_ws_text(text, frame, sizeof(frame));
    if (!n) return;
    dispatch_data_t d = dispatch_data_create(frame, n, queue, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    nw_connection_send(c, d, NW_CONNECTION_DEFAULT_MESSAGE_CONTEXT, true, ^(nw_error_t e) { (void)e; });
}
static void send_raw(nw_connection_t c, NSData *data, bool close_after) {
    dispatch_data_t d = dispatch_data_create(data.bytes, data.length, queue, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    nw_connection_send(c, d, NW_CONNECTION_DEFAULT_MESSAGE_CONTEXT, true, ^(nw_error_t e) {
        (void)e;
        if (close_after) nw_connection_cancel(c);
    });
}
static void respond(nw_connection_t c, int status, const char *reason, const char *type, NSData *body) {
    NSMutableData *out = [[NSString stringWithFormat:@"HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %lu\r\n"
                           "Cache-Control: no-store\r\nConnection: close\r\n\r\n", status, reason, type ? type : "text/plain",
                           (unsigned long)body.length] dataUsingEncoding:NSUTF8StringEncoding].mutableCopy;
    if (body) [out appendData:body];
    send_raw(c, out, true);
}
static void set_status(const char *s) {
    snprintf(last_status, sizeof(last_status), "%s", s);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSNotificationCenter.defaultCenter postNotificationName:@"P2PhonePadChanged" object:nil];
    });
}
static void ws_loop(nw_connection_t c, NSMutableData *buffer) {
    nw_connection_receive(c, 1, 4096, ^(dispatch_data_t content, nw_content_context_t ctx, bool complete, nw_error_t error) {
        (void)ctx;
        if (content) dispatch_data_apply(content, ^bool(dispatch_data_t r, size_t o, const void *b, size_t n) {
            (void)r; (void)o; [buffer appendBytes:b length:n]; return true;
        });
        for (;;) {
            int op = 0;
            char text[4100];
            int used = rt_phonepad_ws_decode(buffer.bytes, buffer.length, &op, text, sizeof(text));
            if (used == 0) break;
            if (used < 0 || op == 8) { nw_connection_cancel(c); return; }
            [buffer replaceBytesInRange:NSMakeRange(0, (NSUInteger)used) withBytes:NULL length:0];
            if (op != 1) continue;
            last_heard = CACurrentMediaTime();
            if (!strncmp(text, "r ", 2)) {   // the phone's measured round trip (ms)
                last_rtt = atof(text + 2);
                static int logged;
                if (logged++ % 10 == 0) fprintf(stderr, "[ios] manette téléphone : aller-retour %.1f ms\n", last_rtt);
                continue;
            }
            rt_phone_message m;
            if (!rt_phonepad_parse(text, &m)) continue;
            if (m.kind == RT_PHONE_PING) {
                char reply[32];
                snprintf(reply, sizeof(reply), "q %s", m.ping);
                send_text(c, reply);
            } else dispatch_async(dispatch_get_main_queue(), ^{ apply(&m); });
        }
        if (error || complete) { nw_connection_cancel(c); return; }
        ws_loop(c, buffer);
    });
}
static NSData *page_file(const char *name) {
    NSString *path = [NSBundle.mainBundle pathForResource:[@"phonepad/" stringByAppendingString:@(name)] ofType:nil];
    NSData *data = path ? [NSData dataWithContentsOfFile:path] : nil;
    if (data && !strcmp(name, "index.html")) {   // the .local name the page moves to
        NSString *html = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSString *where = local_name[0] ? [NSString stringWithFormat:@"%s:%d", local_name, port] : @"";
        data = [[html stringByReplacingOccurrencesOfString:@"%%LOCAL%%" withString:where] dataUsingEncoding:NSUTF8StringEncoding];
    }
    return data;
}
static void handle(nw_connection_t c, NSMutableData *buffer) {
    nw_connection_receive(c, 1, 8192, ^(dispatch_data_t content, nw_content_context_t ctx, bool complete, nw_error_t error) {
        (void)ctx;
        if (content) dispatch_data_apply(content, ^bool(dispatch_data_t r, size_t o, const void *b, size_t n) {
            (void)r; (void)o; [buffer appendBytes:b length:n]; return true;
        });
        rt_phonepad_request req;
        if (!rt_phonepad_parse_request(buffer.bytes, buffer.length, &req)) {
            if (req.complete || error || complete || buffer.length > 8000) nw_connection_cancel(c);
            else handle(c, buffer);
            return;
        }
        if (!strcmp(req.path, "/ws")) {
            if (!req.ws_key[0] || !rt_phonepad_code_ok(req.query, code)) { respond(c, 403, "Forbidden", NULL, nil); return; }
            if (phone) {   // one phone at a time: the handshake and "busy" together, closed once sent
                char accept[32];
                rt_phonepad_ws_accept(req.ws_key, accept);
                NSMutableData *out = [[NSString stringWithFormat:@"HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n"
                                       "Connection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n", accept]
                                      dataUsingEncoding:NSUTF8StringEncoding].mutableCopy;
                uint8_t frame[16];
                size_t n = rt_phonepad_ws_text("busy", frame, sizeof(frame));
                [out appendBytes:frame length:n];
                send_raw(c, out, true);
                return;
            }
            char accept[32];
            rt_phonepad_ws_accept(req.ws_key, accept);
            send_raw(c, [[NSString stringWithFormat:@"HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n"
                          "Connection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n", accept]
                         dataUsingEncoding:NSUTF8StringEncoding], false);
            phone = c;
            set_status("Phone connected");
            fprintf(stderr, "[ios] manette téléphone : connectée\n");
            last_heard = CACurrentMediaTime();
            dispatch_async(dispatch_get_main_queue(), ^{ pad_on(); start_watchdog(); });
            ws_loop(c, [NSMutableData data]);
            return;
        }
        if (!strcmp(req.path, "/ping")) { respond(c, 204, "No Content", NULL, nil); return; }
        const char *type = NULL, *file = rt_phonepad_file(req.path, &type);
        NSData *data = file ? page_file(file) : nil;
        if (data) respond(c, 200, "OK", type, data);
        else respond(c, 404, "Not Found", NULL, [@"Not found" dataUsingEncoding:NSUTF8StringEncoding]);
    });
}
static void accept_connection(nw_connection_t c) {
    nw_endpoint_t remote = nw_connection_copy_endpoint(c);
    char host[INET6_ADDRSTRLEN] = "";
    const struct sockaddr *sa = nw_endpoint_get_address(remote);
    if (sa) getnameinfo(sa, sa->sa_len, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
    char *pct = strchr(host, '%');   // fe80::1%en0
    if (pct) *pct = 0;
    if (!rt_phonepad_local_address(host)) {   // local network only
        fprintf(stderr, "[ios] manette téléphone : %s refusé (pas sur le réseau local)\n", host);
        nw_connection_cancel(c);
        return;
    }
    nw_connection_set_queue(c, queue);
    nw_connection_set_state_changed_handler(c, ^(nw_connection_state_t state, nw_error_t e) {
        (void)e;
        if (state == nw_connection_state_cancelled || state == nw_connection_state_failed) {
            if (phone == c) {
                phone = nil;
                set_status("Waiting for a phone");
                fprintf(stderr, "[ios] manette téléphone : déconnectée\n");
                dispatch_async(dispatch_get_main_queue(), ^{ pad_off(); });
            }
        }
    });
    nw_connection_start(c);
    handle(c, [NSMutableData data]);
}

// The iPad's Wi-Fi (or wired) IPv4 address, for the QR code and the .local record.
static int find_address(char *out, size_t size, struct in_addr *raw) {
    struct ifaddrs *list = NULL, *it;
    int found = 0;
    if (getifaddrs(&list)) return 0;
    for (it = list; it && !found; it = it->ifa_next) {
        if (!it->ifa_addr || it->ifa_addr->sa_family != AF_INET || !(it->ifa_flags & IFF_UP)) continue;
        if (strncmp(it->ifa_name, "en", 2)) continue;   // Wi-Fi and Ethernet, not cellular (pdp_ip) or loopback
        *raw = ((struct sockaddr_in *)it->ifa_addr)->sin_addr;
        inet_ntop(AF_INET, raw, out, (socklen_t)size);
        found = rt_phonepad_local_address(out);
    }
    freeifaddrs(list);
    return found;
}
static void registered(DNSServiceRef ref, DNSRecordRef rec, DNSServiceFlags flags, DNSServiceErrorType e, void *ctx) {
    (void)ref; (void)rec; (void)flags; (void)ctx;
    fprintf(stderr, "[ios] manette téléphone : nom %s %s (%d)\n", local_name, e ? "refusé" : "confirmé", e);
}
// sauce-XXXX.local -> this iPad's address, through Bonjour (mDNSResponder).
static void register_name(struct in_addr address) {
    NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
    NSString *suffix = [d stringForKey:@"P2PhonePadName"];
    if (!suffix) { suffix = [NSString stringWithFormat:@"%04x", arc4random_uniform(0x10000)]; [d setObject:suffix forKey:@"P2PhonePadName"]; }
    snprintf(local_name, sizeof(local_name), "sauce-%s.local", suffix.UTF8String);
    if (dns) { DNSServiceRefDeallocate(dns); dns = NULL; }
    DNSRecordRef record = NULL;
    DNSServiceErrorType e = DNSServiceCreateConnection(&dns);
    char fqdn[72];   // fully qualified, with the root's dot (without it: kDNSServiceErr_BadParam)
    snprintf(fqdn, sizeof(fqdn), "%s.", local_name);
    if (!e) e = DNSServiceRegisterRecord(dns, &record, kDNSServiceFlagsUnique, kDNSServiceInterfaceIndexAny, fqdn,
                                         kDNSServiceType_A, kDNSServiceClass_IN, 4, &address, 120, registered, NULL);
    if (!e) e = DNSServiceSetDispatchQueue(dns, queue);
    if (e) {
        fprintf(stderr, "[ios] manette téléphone : nom %s non publié (erreur %d), adresse IP seulement\n", local_name, e);
        local_name[0] = 0;
    } else fprintf(stderr, "[ios] manette téléphone : nom %s publié (Bonjour)\n", local_name);
}

void rt_ios_phonepad_start(void) {
    if (listener) return;
    if (!queue) queue = dispatch_queue_create("phonepad", DISPATCH_QUEUE_SERIAL);
    rt_phonepad_code(arc4random_uniform(10000), code);
    port = (int)rt_ios_env_float("P2_PHONEPAD_PORT", 8017, 1024);
    struct in_addr raw = {0};
    if (!find_address(ip, sizeof(ip), &raw)) { snprintf(last_status, sizeof(last_status), "No Wi-Fi: join a network"); return; }
    register_name(raw);
    nw_parameters_t params = nw_parameters_create_secure_tcp(NW_PARAMETERS_DISABLE_PROTOCOL, NW_PARAMETERS_DEFAULT_CONFIGURATION);
    nw_parameters_prohibit_interface_type(params, nw_interface_type_cellular);   // local network only
    char p[8];
    snprintf(p, sizeof(p), "%d", port);
    listener = nw_listener_create_with_port(p, params);
    nw_listener_set_queue(listener, queue);
    nw_listener_set_new_connection_handler(listener, ^(nw_connection_t c) { accept_connection(c); });
    nw_listener_set_state_changed_handler(listener, ^(nw_listener_state_t state, nw_error_t e) {
        if (state == nw_listener_state_ready) set_status("Waiting for a phone");
        if (state == nw_listener_state_failed) {
            set_status("Couldn't start (port in use?)");
            fprintf(stderr, "[ios] manette téléphone : serveur en échec (%d)\n", e ? nw_error_get_error_code(e) : 0);
        }
    });
    nw_listener_start(listener);
    fprintf(stderr, "[ios] manette téléphone : http://%s:%d/?code=%s\n", ip, port, code);
}
// The panel closed before any phone connected: pairing stops; the next opening
// starts again with a new code. A connected phone keeps playing.
void rt_ios_phonepad_stop_if_idle(void) {
    if (!queue || getenv("P2_PHONEPAD")) return;   // P2_PHONEPAD: the server from launch (tests)
    dispatch_sync(queue, ^{   // done before the panel can open again (never waits on the main thread)
        if (phone || !listener) return;
        nw_listener_cancel(listener);
        listener = nil;
        if (dns) { DNSServiceRefDeallocate(dns); dns = NULL; }
        snprintf(last_status, sizeof(last_status), "Not running");
        fprintf(stderr, "[ios] manette téléphone : appairage annulé (fenêtre fermée sans téléphone)\n");
    });
}
void rt_ios_phonepad_kick(void) {
    if (!queue) return;
    dispatch_async(queue, ^{
        if (phone) { send_text(phone, "kicked"); nw_connection_cancel(phone); }
        rt_phonepad_code(arc4random_uniform(10000), code);   // the old code stops working
        set_status("Kicked: new code");
        fprintf(stderr, "[ios] manette téléphone : expulsée, nouveau code\n");
    });
}

// ---- the panel (gear menu > Phone controller) ----
static UIImage *qr(NSString *text, CGFloat size) {
    CIFilter *f = [CIFilter filterWithName:@"CIQRCodeGenerator"];
    [f setValue:[text dataUsingEncoding:NSUTF8StringEncoding] forKey:@"inputMessage"];
    [f setValue:@"M" forKey:@"inputCorrectionLevel"];
    CIImage *img = f.outputImage;
    if (!img) return nil;
    CGFloat k = size / img.extent.size.width;
    img = [img imageByApplyingTransform:CGAffineTransformMakeScale(k, k)];
    CGImageRef cg = [[CIContext context] createCGImage:img fromRect:img.extent];
    UIImage *out = [UIImage imageWithCGImage:cg];
    CGImageRelease(cg);
    return out;
}
NSString *rt_ios_phonepad_url(void) {
    return ip[0] ? [NSString stringWithFormat:@"http://%s:%d/?code=%s", ip, port, code] : nil;
}
// The panel stops listening for changes once it leaves the screen, however it was closed.
@interface P2PhonePanel : UIView
@property(nonatomic, strong) id observer;
@end
@implementation P2PhonePanel
- (void)willMoveToWindow:(UIWindow *)window {
    if (!window && self.observer) { [NSNotificationCenter.defaultCenter removeObserver:self.observer]; self.observer = nil; }
}
@end

UIView *rt_ios_phonepad_panel(void (^close)(void)) {
    rt_ios_phonepad_start();
    P2PhonePanel *panel = [P2PhonePanel new];
    panel.backgroundColor = [UIColor colorWithWhite:.06 alpha:.97];
    panel.layer.cornerRadius = 22;
    UIStackView *s = [UIStackView new];
    s.axis = UILayoutConstraintAxisVertical; s.spacing = 10; s.alignment = UIStackViewAlignmentCenter;
    s.translatesAutoresizingMaskIntoConstraints = NO;
    [panel addSubview:s];
    [NSLayoutConstraint activateConstraints:@[
        [s.topAnchor constraintEqualToAnchor:panel.topAnchor constant:22], [s.bottomAnchor constraintEqualToAnchor:panel.bottomAnchor constant:-18],
        [s.leadingAnchor constraintEqualToAnchor:panel.leadingAnchor constant:22], [s.trailingAnchor constraintEqualToAnchor:panel.trailingAnchor constant:-22]]];
    UILabel *title = [UILabel new];
    title.text = @"Phone as a controller"; title.font = [UIFont systemFontOfSize:22 weight:UIFontWeightBold]; title.textColor = UIColor.whiteColor;
    [s addArrangedSubview:title];
    UILabel *how = [UILabel new];
    how.text = @"Scan with the phone's camera (same Wi-Fi). It becomes player 2 in co-op, or player 1 if nothing else plays.";
    how.numberOfLines = 0; how.textAlignment = NSTextAlignmentCenter; how.font = [UIFont systemFontOfSize:14];
    how.textColor = [UIColor colorWithWhite:1 alpha:.7];
    [how.widthAnchor constraintLessThanOrEqualToConstant:360].active = YES;
    [s addArrangedSubview:how];
    NSString *url = rt_ios_phonepad_url();
    UIImageView *image = [[UIImageView alloc] initWithImage:url ? qr(url, 220) : nil];
    image.layer.magnificationFilter = kCAFilterNearest;
    image.backgroundColor = UIColor.whiteColor; image.layer.cornerRadius = 10; image.clipsToBounds = YES;
    [image.widthAnchor constraintEqualToConstant:220].active = YES;
    [image.heightAnchor constraintEqualToConstant:220].active = YES;
    [s addArrangedSubview:image];
    UILabel *codeLabel = [UILabel new];
    codeLabel.text = url ? [NSString stringWithFormat:@"Code %s", code] : @"";
    codeLabel.font = [UIFont monospacedDigitSystemFontOfSize:26 weight:UIFontWeightBold]; codeLabel.textColor = UIColor.whiteColor;
    [s addArrangedSubview:codeLabel];
    UILabel *where = [UILabel new];
    where.text = url ? [NSString stringWithFormat:@"%@%@", url, local_name[0] ? [NSString stringWithFormat:@"\nor http://%s:%d", local_name, port] : @""]
                     : @(last_status);
    where.numberOfLines = 0; where.textAlignment = NSTextAlignmentCenter; where.font = [UIFont systemFontOfSize:12];
    where.textColor = [UIColor colorWithWhite:1 alpha:.55];
    [s addArrangedSubview:where];
    UILabel *state = [UILabel new];
    state.font = [UIFont systemFontOfSize:15 weight:UIFontWeightSemibold]; state.textColor = [UIColor colorWithRed:.98 green:.55 blue:.16 alpha:1];
    void (^refresh)(void) = ^{
        state.text = last_rtt >= 0 && phone_connected ? [NSString stringWithFormat:@"%s · %.0f ms round trip", last_status, last_rtt]
                                                      : @(last_status);
        codeLabel.text = url ? [NSString stringWithFormat:@"Code %s", code] : @"";
        NSString *now = rt_ios_phonepad_url();
        if (now) image.image = qr(now, 220);
    };
    refresh();
    panel.observer = [NSNotificationCenter.defaultCenter addObserverForName:@"P2PhonePadChanged" object:nil
                                                                     queue:NSOperationQueue.mainQueue
                                                                usingBlock:^(NSNotification *n) { (void)n; refresh(); }];
    [s addArrangedSubview:state];
    UIStackView *buttons = [UIStackView new];
    buttons.spacing = 10;
    UIButtonConfiguration *kick = [UIButtonConfiguration grayButtonConfiguration];
    kick.title = @"Kick phone"; kick.baseForegroundColor = [UIColor colorWithRed:1 green:.45 blue:.4 alpha:1];
    [buttons addArrangedSubview:[UIButton buttonWithConfiguration:kick primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; rt_ios_phonepad_kick();
    }]]];
    UIButtonConfiguration *done = [UIButtonConfiguration filledButtonConfiguration];
    done.title = @"Done";
    [buttons addArrangedSubview:[UIButton buttonWithConfiguration:done primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a;
        if (close) close();
    }]]];
    [s addArrangedSubview:buttons];
    // Round 24: a close button in the corner too (touch); a controller's A or B also closes it.
    UIButtonConfiguration *x = [UIButtonConfiguration plainButtonConfiguration];
    x.image = [UIImage systemImageNamed:@"xmark.circle.fill"
                      withConfiguration:[UIImageSymbolConfiguration configurationWithPointSize:28]];
    x.baseForegroundColor = [UIColor colorWithWhite:1 alpha:.75];
    UIButton *xButton = [UIButton buttonWithConfiguration:x primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a;
        if (close) close();
    }]];
    xButton.accessibilityLabel = @"Close";
    xButton.translatesAutoresizingMaskIntoConstraints = NO;
    [panel addSubview:xButton];
    [NSLayoutConstraint activateConstraints:@[[xButton.topAnchor constraintEqualToAnchor:panel.topAnchor constant:6],
                                              [xButton.trailingAnchor constraintEqualToAnchor:panel.trailingAnchor constant:-6],
                                              [xButton.widthAnchor constraintEqualToConstant:44],
                                              [xButton.heightAnchor constraintEqualToConstant:44]]];
    return panel;
}
#endif
