// Round 26 (item 4): the Sauce app on an iPhone as the controller, from the
// launcher's "Use as controller". It finds Sauce on an iPad nearby whose Phone
// controller panel is open (phonepad_ios.m advertises _sauce-pad._tcp with its
// pairing code) and, with one tap, loads that iPad's controller page (the same
// page a phone without Sauce opens from the QR code, ios/phonepad/index.html)
// in a full-screen web view. The page reaches back for native haptics and its
// Close button through the "sauce" message handler.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#include <arpa/inet.h>
#include <dns_sd.h>
#include <stdio.h>
#include <string.h>
#include "phone_controller_ios.h"

typedef struct {
    int used;
    char name[64], game[64], code[8], address[64];
    uint16_t port;
    uint32_t iface;
    DNSServiceRef resolve, addr;
} pad_host;
static pad_host hosts[8];
static DNSServiceRef browse_ref;
static dispatch_queue_t queue;
static NSString *const kChanged = @"P2PadHostsChanged";
static int denied;

static void changed(void) {
    dispatch_async(dispatch_get_main_queue(), ^{ [NSNotificationCenter.defaultCenter postNotificationName:kChanged object:nil]; });
}
static void drop(pad_host *h) {
    if (h->resolve) DNSServiceRefDeallocate(h->resolve);
    if (h->addr) DNSServiceRefDeallocate(h->addr);
    memset(h, 0, sizeof(*h));
}
static void txt_value(const unsigned char *txt, uint16_t len, const char *key, char *out, size_t size) {
    uint8_t n = 0;
    const void *v = TXTRecordGetValuePtr(len, txt, key, &n);
    snprintf(out, size, "%.*s", v ? (int)n : 0, v ? (const char *)v : "");
}
static void got_address(DNSServiceRef ref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType e, const char *host,
                        const struct sockaddr *sa, uint32_t ttl, void *ctx) {
    (void)ref; (void)flags; (void)iface; (void)host; (void)ttl;
    pad_host *h = ctx;
    if (e) fprintf(stderr, "[ios] manette : adresse de « %s », erreur %d\n", h->name, e);
    if (e || !h->used || !sa || sa->sa_family != AF_INET) return;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((const struct sockaddr_in *)sa)->sin_addr, ip, sizeof(ip));
    snprintf(h->address, sizeof(h->address), "%s:%u", ip, h->port);
    fprintf(stderr, "[ios] manette : « %s » (%s) à %s\n", h->name, h->game, h->address);
    changed();
}
static void resolved(DNSServiceRef ref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType e, const char *fullname,
                     const char *target, uint16_t port, uint16_t txt_len, const unsigned char *txt, void *ctx) {
    (void)ref; (void)flags; (void)fullname;
    pad_host *h = ctx;
    if (e) fprintf(stderr, "[ios] manette : résolution de « %s », erreur %d\n", h->name, e);
    if (e || !h->used) return;
    h->port = ntohs(port);
    fprintf(stderr, "[ios] manette : « %s » résolu en %s:%u\n", h->name, target, ntohs(port));
    txt_value(txt, txt_len, "code", h->code, sizeof(h->code));
    txt_value(txt, txt_len, "game", h->game, sizeof(h->game));
    if (h->addr) DNSServiceRefDeallocate(h->addr);
    h->addr = NULL;
    if (!DNSServiceGetAddrInfo(&h->addr, 0, iface, kDNSServiceProtocol_IPv4, target, got_address, h))
        DNSServiceSetDispatchQueue(h->addr, queue);
    else h->addr = NULL;
    changed();
}
static void browsed(DNSServiceRef ref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType e, const char *name,
                    const char *type, const char *domain, void *ctx) {
    (void)ref; (void)ctx;
    if (e) {
        denied = e == kDNSServiceErr_PolicyDenied || e == kDNSServiceErr_NoAuth;
        fprintf(stderr, "[ios] manette : recherche, erreur %d\n", e);
        changed();
        return;
    }
    pad_host *h = NULL;
    for (size_t i = 0; i < sizeof(hosts) / sizeof(*hosts); ++i)
        if (hosts[i].used && !strcmp(hosts[i].name, name)) h = &hosts[i];
    if (!(flags & kDNSServiceFlagsAdd)) { if (h) { drop(h); changed(); } return; }
    if (h) return;
    for (size_t i = 0; i < sizeof(hosts) / sizeof(*hosts) && !h; ++i) if (!hosts[i].used) h = &hosts[i];
    if (!h) return;
    h->used = 1;
    h->iface = iface;
    snprintf(h->name, sizeof(h->name), "%s", name);
    fprintf(stderr, "[ios] manette : trouvé « %s » (interface %u)\n", name, iface);
    if (!DNSServiceResolve(&h->resolve, 0, iface, name, type, domain, resolved, h)) DNSServiceSetDispatchQueue(h->resolve, queue);
    else h->resolve = NULL;
    changed();
}
static void browse(int on) {
    if (!queue) queue = dispatch_queue_create("padhosts", DISPATCH_QUEUE_SERIAL);
    dispatch_async(queue, ^{
        if (!on) {
            if (browse_ref) DNSServiceRefDeallocate(browse_ref);
            browse_ref = NULL;
            for (size_t i = 0; i < sizeof(hosts) / sizeof(*hosts); ++i) if (hosts[i].used) drop(&hosts[i]);
            return;
        }
        if (browse_ref) return;
        DNSServiceErrorType e = DNSServiceBrowse(&browse_ref, 0, kDNSServiceInterfaceIndexAny, "_sauce-pad._tcp", NULL, browsed, NULL);
        if (!e) e = DNSServiceSetDispatchQueue(browse_ref, queue);
        if (e) { browse_ref = NULL; denied = e == kDNSServiceErr_PolicyDenied; changed(); }
        fprintf(stderr, "[ios] manette : recherche des iPad avec Sauce (%d)\n", e);
    });
}

// ---- the controller page, full screen ----
@interface P2PadWeb : UIView <WKScriptMessageHandler>
@property (nonatomic, copy) void (^close)(void);
@end
@implementation P2PadWeb {
    WKWebView *web;
    UIImpactFeedbackGenerator *light, *heavy;
}
- (instancetype)initWithURL:(NSURL *)url {
    if (!(self = [super initWithFrame:CGRectZero])) return nil;
    self.backgroundColor = [UIColor colorWithRed:.043 green:.043 blue:.06 alpha:1];
    WKWebViewConfiguration *conf = [WKWebViewConfiguration new];
    // A weak relay: WKUserContentController retains its handler.
    [conf.userContentController addScriptMessageHandler:self name:@"sauce"];
    conf.allowsInlineMediaPlayback = YES;
    web = [[WKWebView alloc] initWithFrame:CGRectZero configuration:conf];
    web.scrollView.scrollEnabled = NO;
    web.scrollView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
    web.opaque = NO;
    web.backgroundColor = self.backgroundColor;
    web.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self addSubview:web];
    light = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleLight];
    heavy = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleMedium];
    [light prepare]; [heavy prepare];
    [web loadRequest:[NSURLRequest requestWithURL:url]];
    UIApplication.sharedApplication.idleTimerDisabled = YES;   // a controller doesn't fall asleep mid-game
    return self;
}
- (void)layoutSubviews { [super layoutSubviews]; web.frame = self.bounds; }
- (void)userContentController:(WKUserContentController *)c didReceiveScriptMessage:(WKScriptMessage *)m {
    (void)c;
    NSString *what = [m.body isKindOfClass:NSString.class] ? m.body : @"";
    if ([what isEqualToString:@"tap"]) { [light impactOccurred]; [light prepare]; }
    else if ([what isEqualToString:@"fire"]) { [heavy impactOccurred]; [heavy prepare]; }
    else if ([what isEqualToString:@"close"]) [self leave];
}
- (void)leave {
    UIApplication.sharedApplication.idleTimerDisabled = NO;
    [web.configuration.userContentController removeScriptMessageHandlerForName:@"sauce"];
    [web loadHTMLString:@"" baseURL:nil];   // the WebSocket closes: the iPad unplugs the controller
    fprintf(stderr, "[ios] manette : fermée\n");
    if (self.close) self.close();
}
@end

// ---- the list of iPads ----
@interface P2PadHosts : UIView
@property (nonatomic, copy) void (^close)(void);
@end
@implementation P2PadHosts {
    UIStackView *stack;
    UILabel *empty;
    P2PadWeb *pad;
    id observer;
}
static UIButton *pill(NSString *title, NSString *subtitle, NSString *symbol, UIColor *bg, void (^tap)(void)) {
    UIButtonConfiguration *conf = [UIButtonConfiguration filledButtonConfiguration];
    conf.title = title; conf.subtitle = subtitle;
    conf.image = symbol ? [UIImage systemImageNamed:symbol] : nil;
    conf.imagePadding = 10;
    conf.baseBackgroundColor = bg; conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    conf.contentInsets = NSDirectionalEdgeInsetsMake(12, 16, 12, 16);
    UIButton *b = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) { (void)a; tap(); }]];
    b.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeading;
    return b;
}
- (instancetype)init {
    if (!(self = [super initWithFrame:CGRectZero])) return nil;
    self.backgroundColor = [UIColor colorWithWhite:0 alpha:.6];
    UIView *panel = [UIView new];
    panel.backgroundColor = [UIColor colorWithWhite:.11 alpha:1];
    panel.layer.cornerRadius = 20;
    panel.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:panel];
    stack = [UIStackView new];
    stack.axis = UILayoutConstraintAxisVertical; stack.spacing = 10;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    [panel addSubview:stack];
    [NSLayoutConstraint activateConstraints:@[
        [panel.centerXAnchor constraintEqualToAnchor:self.centerXAnchor],
        [panel.centerYAnchor constraintEqualToAnchor:self.centerYAnchor],
        [panel.widthAnchor constraintEqualToConstant:420],
        [panel.widthAnchor constraintLessThanOrEqualToAnchor:self.widthAnchor constant:-32],
        [stack.topAnchor constraintEqualToAnchor:panel.topAnchor constant:18],
        [stack.bottomAnchor constraintEqualToAnchor:panel.bottomAnchor constant:-18],
        [stack.leadingAnchor constraintEqualToAnchor:panel.leadingAnchor constant:18],
        [stack.trailingAnchor constraintEqualToAnchor:panel.trailingAnchor constant:-18],
    ]];
    __weak P2PadHosts *weak = self;
    observer = [NSNotificationCenter.defaultCenter addObserverForName:kChanged object:nil queue:NSOperationQueue.mainQueue
                                                           usingBlock:^(NSNotification *n) { (void)n; [weak refresh]; }];
    browse(1);
    [self refresh];
    // P2_PAD_TEST_ADDRESS="<ip:port>,<code>" (simulator checks: its Bonjour finds nothing):
    // what a tap on a found iPad does, without the search.
    const char *test = getenv("P2_PAD_TEST_ADDRESS");
    char address[64], code[8];
    if (test && sscanf(test, "%63[^,],%7s", address, code) == 2) {
        NSString *a = @(address), *c = @(code);
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
            [weak connect:a code:c name:@"test"];
        });
    }
    return self;
}
- (void)dealloc { [NSNotificationCenter.defaultCenter removeObserver:observer]; }
- (void)refresh {
    for (UIView *v in stack.arrangedSubviews.copy) [v removeFromSuperview];
    UILabel *title = [UILabel new];
    title.text = @"Use as controller";
    title.font = [UIFont systemFontOfSize:22 weight:UIFontWeightBold]; title.textColor = UIColor.whiteColor;
    [stack addArrangedSubview:title];
    UILabel *how = [UILabel new];
    how.numberOfLines = 0; how.font = [UIFont systemFontOfSize:14]; how.textColor = [UIColor colorWithWhite:1 alpha:.6];
    how.text = @"On the iPad, open the game's gear menu > Phone controller. It shows up here: tap it to connect.";
    [stack addArrangedSubview:how];
    int shown = 0;
    for (size_t i = 0; i < sizeof(hosts) / sizeof(*hosts); ++i) {
        pad_host h = hosts[i];   // a copy (the queue may change the list)
        if (!h.used || !h.address[0] || !h.code[0]) continue;
        ++shown;
        NSString *address = @(h.address), *code = @(h.code), *name = @(h.name);
        __weak P2PadHosts *weak = self;
        [stack addArrangedSubview:pill(name, h.game[0] ? [NSString stringWithFormat:@"Playing %s", h.game] : @"Sauce",
                                       @"ipad.landscape", [UIColor colorWithRed:.98 green:.55 blue:.16 alpha:1], ^{
            [weak connect:address code:code name:name];
        })];
    }
    if (!shown) {
        UILabel *none = [UILabel new];
        none.numberOfLines = 0; none.font = [UIFont systemFontOfSize:15 weight:UIFontWeightMedium];
        none.textColor = [UIColor colorWithWhite:1 alpha:.8];
        none.text = denied ? @"Sauce isn't allowed to use the local network. Settings > Privacy & Security > Local Network: "
                              "switch on Sauce, then come back."
                           : @"Looking for an iPad with Sauce on this Wi-Fi…";
        [stack addArrangedSubview:none];
    }
    __weak P2PadHosts *weak = self;
    [stack addArrangedSubview:pill(@"Close", nil, @"xmark", [UIColor colorWithWhite:.25 alpha:1], ^{ [weak finish]; })];
    // P2_PAD_CONNECT_TEST=1 (device checks): the first iPad found is tapped.
    if (shown && getenv("P2_PAD_CONNECT_TEST") && !pad) {
        for (size_t i = 0; i < sizeof(hosts) / sizeof(*hosts); ++i)
            if (hosts[i].used && hosts[i].address[0] && hosts[i].code[0]) {
                [self connect:@(hosts[i].address) code:@(hosts[i].code) name:@(hosts[i].name)];
                break;
            }
    }
}
- (void)connect:(NSString *)address code:(NSString *)code name:(NSString *)name {
    if (pad) return;
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@/?code=%@&app=1", address, code]];
    fprintf(stderr, "[ios] manette : connexion à « %s » (%s)\n", name.UTF8String, address.UTF8String);
    pad = [[P2PadWeb alloc] initWithURL:url];
    pad.frame = self.bounds;
    pad.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    __weak P2PadHosts *weak = self;
    pad.close = ^{
        P2PadHosts *me = weak;
        if (!me) return;
        [me->pad removeFromSuperview];
        me->pad = nil;
    };
    [self addSubview:pad];
}
- (void)finish {
    browse(0);
    if (self.close) self.close();
}
@end

UIView *rt_ios_phone_controller_panel(void (^close)(void)) {
    P2PadHosts *v = [P2PadHosts new];
    v.close = close;
    return v;
}
#endif
