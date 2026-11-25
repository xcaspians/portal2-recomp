// Community Maps browser (see workshop_ios.h and workshop.h).
//
// Browsing (Popular, Newest, Trending, Search; single player by default, or
// co-op) asks IPublishedFileService/QueryFiles with the player's own Steam Web
// API key, kept in the Keychain (this device only). Without a key the page
// says how to get one, and "Paste a Workshop link or ID" still works: details
// and downloads go through ISteamRemoteStorage, which needs no key. A pasted
// collection opens as the list of its maps.
//
// Maps are saved in <user folder>/portal2/maps/workshop/<id>/<name>.bsp: the
// engine reads the user folder before the game's (runtime/paths.c), the Maps &
// Mods list shows them, and p2workshop.txt next to the map keeps the
// Workshop's update date for the update badge. A download goes to
// <name>.bsp.part and is renamed only once complete and checked (size, "VBSP"),
// so a canceled or broken one never shows up as a map; rt_ws_cleanup removes
// leftovers when the browser opens. The folder is kept out of iCloud backups.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <QuartzCore/QuartzCore.h>
#import <Security/Security.h>
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <string.h>
#include "settings_menu_ios.h"
#include "workshop.h"
#include "workshop_ios.h"

const char *rt_user_root(void);                                        // runtime/paths.c
int rt_mods_map_name(const char *relative, char *out, size_t size);    // mods_list.c

static NSString *const kKeyService = @"Portal2Recomp.SteamWebAPIKey";
static NSString *const kKeyLink = @"https://steamcommunity.com/dev/apikey";
static NSString *const kDetails = @"https://api.steampowered.com/ISteamRemoteStorage/GetPublishedFileDetails/v1/";
static NSString *const kCollection = @"https://api.steampowered.com/ISteamRemoteStorage/GetCollectionDetails/v1/";
static NSString *const kNames = @"https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v2/";

// ---- the key, in the Keychain ----
static NSDictionary *key_query(void) {
    return @{(__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword, (__bridge id)kSecAttrService: kKeyService,
             (__bridge id)kSecAttrAccount: @"key"};
}
static NSString *key_load(void) {
    NSMutableDictionary *q = [key_query() mutableCopy];
    q[(__bridge id)kSecReturnData] = @YES;
    CFTypeRef data = NULL;
    if (SecItemCopyMatching((__bridge CFDictionaryRef)q, &data) != errSecSuccess || !data) return nil;
    NSString *key = [[NSString alloc] initWithData:(__bridge_transfer NSData *)data encoding:NSUTF8StringEncoding];
    return key.length ? key : nil;
}
static BOOL key_save(NSString *key) {
    SecItemDelete((__bridge CFDictionaryRef)key_query());
    if (!key.length) return YES;
    NSMutableDictionary *q = [key_query() mutableCopy];
    q[(__bridge id)kSecValueData] = [key dataUsingEncoding:NSUTF8StringEncoding];
    q[(__bridge id)kSecAttrAccessible] = (__bridge id)kSecAttrAccessibleAfterFirstUnlockThisDeviceOnly;
    return SecItemAdd((__bridge CFDictionaryRef)q, NULL) == errSecSuccess;
}

// ---- files ----
static NSString *workshop_dir(void) {
    return [@(rt_user_root()) stringByAppendingPathComponent:@"portal2/maps/workshop"];
}
static NSString *item_dir(unsigned long long id) {
    return [workshop_dir() stringByAppendingPathComponent:[NSString stringWithFormat:@"%llu", id]];
}
// The saved map of this item (its file name), or nil.
static NSString *saved_map(unsigned long long id) {
    for (NSString *name in [NSFileManager.defaultManager contentsOfDirectoryAtPath:item_dir(id) error:nil])
        if ([name.pathExtension.lowercaseString isEqualToString:@"bsp"]) return name;
    return nil;
}
void rt_ios_community_cleanup(void) {
    int removed = rt_ws_cleanup(workshop_dir().UTF8String);
    if (removed) fprintf(stderr, "[ios] cartes de la communauté : %d restes de téléchargement retirés\n", removed);
}
static NSString *megabytes(long long bytes) {
    return bytes >= 1 << 20 ? [NSString stringWithFormat:@"%.0f MB", bytes / 1048576.0]
                            : [NSString stringWithFormat:@"%lld KB", MAX(1, bytes / 1024)];
}

// A sentence for what went wrong, never an empty screen.
static NSString *failure(NSError *error, NSInteger status, BOOL keyed) {
    if (error) {
        switch (error.code) {
        case NSURLErrorNotConnectedToInternet: case NSURLErrorNetworkConnectionLost: case NSURLErrorDataNotAllowed:
        case NSURLErrorInternationalRoamingOff:
            return @"You're offline. Connect to the internet and try again.";
        case NSURLErrorCancelled: return @"Canceled.";
        default: return @"Steam isn't answering right now. Try again in a few minutes.";
        }
    }
    if ((status == 401 || status == 403) && keyed)
        return @"Steam refused the API key. Check it at steamcommunity.com/dev/apikey and paste it again.";
    if (status == 429) return @"Steam is limiting requests right now. Wait a minute and try again.";
    if (status >= 500) return @"Steam isn't answering right now. Try again in a few minutes.";
    if (status != 200) return [NSString stringWithFormat:@"Steam answered with an error (%ld). Try again later.", (long)status];
    return @"Steam sent an answer this page can't read. Try again later.";
}

@interface P2WSItem : NSObject {
@public
    rt_ws_item it;
}
@property (nonatomic, copy) NSString *author;
@property (nonatomic, copy) NSString *failure;            // a download's problem, until it completes
@property (nonatomic) long long savedUpdated, savedSize;  // the saved copy's note (Downloaded tab)
@end
@implementation P2WSItem
@end

// Something the controller can reach: its view, what A does, what left and right do.
@interface P2Focus : NSObject
@property (nonatomic, weak) UIView *view;
@property (nonatomic, copy) void (^activate)(void);
@property (nonatomic, copy) void (^change)(int dir);
@end
@implementation P2Focus
@end

@interface P2CommunityMaps : UIView <UIScrollViewDelegate, NSURLSessionDownloadDelegate, UITextFieldDelegate>
@end

@implementation P2CommunityMaps {
    void (*onClose)(void *, int);
    void *closeCtx;
    UIView *header;
    UIScrollView *main;
    UIStackView *stack, *cards;
    UISegmentedControl *tabs, *modes;
    UIView *searchRow, *keyCard;
    UITextField *searchField, *keyField, *pasteField;
    UIButton *keyButton;
    UILabel *status;
    NSMutableArray<P2WSItem *> *items;
    NSString *cursor;
    BOOL loading, ended;
    int request;               // the answer of an older request is dropped
    NSString *key;
    NSCache<NSString *, UIImage *> *images;
    // details
    UIScrollView *details;
    P2WSItem *shown;
    UIProgressView *progress;
    UILabel *detailStatus;
    UIStackView *detailButtons;
    CFTimeInterval deleteArmedUntil;
    BOOL playAfterDownload;
    // download
    NSURLSession *downloads;
    NSURLSessionDownloadTask *task;
    P2WSItem *downloading;
    // controller
    NSMutableArray<P2Focus *> *focus;
    NSInteger focused;
}

- (instancetype)initWithClose:(void (*)(void *, int))close ctx:(void *)ctx {
    if (!(self = [super initWithFrame:CGRectZero])) return nil;
    onClose = close; closeCtx = ctx;
    self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.backgroundColor = [UIColor colorWithWhite:.06 alpha:1];   // opaque: the settings panel stays hidden behind
    items = [NSMutableArray array];
    images = [NSCache new];
    focus = [NSMutableArray array];
    focused = -1;
    key = key_load();
    downloads = [NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.defaultSessionConfiguration delegate:self
                                         delegateQueue:NSOperationQueue.mainQueue];
    rt_ios_community_cleanup();
    [self build];
    [self reload];
    return self;
}
- (void)dealloc { [downloads invalidateAndCancel]; }

// ---- building blocks ----
static UILabel *label(NSString *text, CGFloat size, UIFontWeight weight, CGFloat alpha) {
    UILabel *l = [UILabel new];
    l.text = text;
    l.font = [UIFont systemFontOfSize:size weight:weight];
    l.textColor = [UIColor colorWithWhite:1 alpha:alpha];
    l.numberOfLines = 0;
    return l;
}
static UIButton *button(NSString *title, NSString *symbol, void (^action)(void)) {
    UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
    conf.title = title;
    if (symbol) { conf.image = [UIImage systemImageNamed:symbol]; conf.imagePadding = 6; }
    conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    UIButton *b = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; action();
    }]];
    b.layer.cornerRadius = 12;
    return b;
}
static UITextField *field(NSString *placeholder, id<UITextFieldDelegate> delegate) {
    UITextField *f = [UITextField new];
    f.attributedPlaceholder = [[NSAttributedString alloc] initWithString:placeholder
        attributes:@{NSForegroundColorAttributeName: [UIColor colorWithWhite:1 alpha:.4]}];
    f.textColor = UIColor.whiteColor;
    f.backgroundColor = [UIColor colorWithWhite:1 alpha:.1];
    f.layer.cornerRadius = 10;
    f.leftView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 10, 10)];
    f.leftViewMode = UITextFieldViewModeAlways;
    f.autocorrectionType = UITextAutocorrectionTypeNo;
    f.autocapitalizationType = UITextAutocapitalizationTypeNone;
    f.returnKeyType = UIReturnKeyGo;
    f.delegate = delegate;
    [f.heightAnchor constraintEqualToConstant:40].active = YES;
    return f;
}
static UIStackView *row(NSArray<UIView *> *views) {
    UIStackView *s = [[UIStackView alloc] initWithArrangedSubviews:views];
    s.spacing = 8;
    s.alignment = UIStackViewAlignmentCenter;
    return s;
}
- (P2Focus *)focusOn:(UIView *)view activate:(void (^)(void))activate change:(void (^)(int))change {
    P2Focus *f = [P2Focus new];
    f.view = view; f.activate = activate; f.change = change;
    [focus addObject:f];
    return f;
}

- (void)build {
    __weak P2CommunityMaps *weakSelf = self;
    header = [UIView new];
    [self addSubview:header];
    UIButton *back = button(@"Back", @"chevron.left", ^{ [weakSelf backPressed]; });
    UILabel *title = label(@"Community Maps", 21, UIFontWeightBold, 1);
    for (UIView *v in @[back, title]) { v.translatesAutoresizingMaskIntoConstraints = NO; [header addSubview:v]; }
    [NSLayoutConstraint activateConstraints:@[
        [back.leadingAnchor constraintEqualToAnchor:header.leadingAnchor constant:12],
        [back.centerYAnchor constraintEqualToAnchor:header.centerYAnchor],
        [title.leadingAnchor constraintEqualToAnchor:back.trailingAnchor constant:14],
        [title.centerYAnchor constraintEqualToAnchor:header.centerYAnchor],
    ]];
    main = [UIScrollView new];
    main.delegate = self;
    main.indicatorStyle = UIScrollViewIndicatorStyleWhite;
    main.keyboardDismissMode = UIScrollViewKeyboardDismissModeOnDrag;
    [self addSubview:main];
    stack = [UIStackView new];
    stack.axis = UILayoutConstraintAxisVertical;
    stack.spacing = 10;
    stack.layoutMargins = UIEdgeInsetsMake(4, 16, 24, 16);
    stack.layoutMarginsRelativeArrangement = YES;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    [main addSubview:stack];
    [NSLayoutConstraint activateConstraints:@[
        [stack.topAnchor constraintEqualToAnchor:main.contentLayoutGuide.topAnchor],
        [stack.bottomAnchor constraintEqualToAnchor:main.contentLayoutGuide.bottomAnchor],
        [stack.leadingAnchor constraintEqualToAnchor:main.contentLayoutGuide.leadingAnchor],
        [stack.trailingAnchor constraintEqualToAnchor:main.contentLayoutGuide.trailingAnchor],
        [stack.widthAnchor constraintEqualToAnchor:main.frameLayoutGuide.widthAnchor],
    ]];
    tabs = [[UISegmentedControl alloc] initWithItems:@[@"Popular", @"Newest", @"Trending", @"Search", @"Downloaded"]];
    modes = [[UISegmentedControl alloc] initWithItems:@[@"Single player", @"Co-op"]];
    for (UISegmentedControl *s in @[tabs, modes]) {
        s.selectedSegmentIndex = 0;
        s.selectedSegmentTintColor = [UIColor colorWithRed:.3 green:.68 blue:1 alpha:1];
        [s setTitleTextAttributes:@{NSForegroundColorAttributeName: UIColor.whiteColor} forState:UIControlStateNormal];
        [s addAction:[UIAction actionWithHandler:^(UIAction *a) { (void)a; [weakSelf reload]; }]
            forControlEvents:UIControlEventValueChanged];
    }
    [stack addArrangedSubview:tabs];
    [stack addArrangedSubview:modes];
    [self focusOn:tabs activate:nil change:^(int dir) {
        P2CommunityMaps *s = weakSelf; if (!s) return;
        NSInteger i = MIN(MAX(s->tabs.selectedSegmentIndex + dir, 0), (NSInteger)s->tabs.numberOfSegments - 1);
        if (i != s->tabs.selectedSegmentIndex) { s->tabs.selectedSegmentIndex = i; [s reload]; }
    }];
    [self focusOn:modes activate:^{ P2CommunityMaps *s = weakSelf; s->modes.selectedSegmentIndex ^= 1; [s reload]; }
           change:^(int dir) { P2CommunityMaps *s = weakSelf; s->modes.selectedSegmentIndex = dir > 0; [s reload]; }];
    searchField = field(@"Search chambers by name", self);
    UIButton *go = button(@"Search", @"magnifyingglass", ^{ [weakSelf reload]; });
    searchRow = row(@[searchField, go]);
    [stack addArrangedSubview:searchRow];
    // The key: a card with the how-to while it's missing, a small button once it's there.
    keyField = field(@"Paste your Steam Web API key", self);
    keyField.secureTextEntry = YES;
    UIButton *save = button(@"Save", nil, ^{ [weakSelf saveKey]; });
    UIButton *open = button(@"Get a free key", @"safari", ^{
        [UIApplication.sharedApplication openURL:[NSURL URLWithString:kKeyLink] options:@{} completionHandler:nil];
    });
    UILabel *how = label(@"Browsing needs your own free Steam Web API key. Get one at steamcommunity.com/dev/apikey "
                         "(signed in to Steam; any domain name works, for example localhost), copy it, and paste it here. "
                         "It stays in this device's Keychain and is only sent to Steam. Without a key you can still "
                         "paste a Workshop link or ID below.", 14, UIFontWeightRegular, .75);
    UIStackView *card = [[UIStackView alloc] initWithArrangedSubviews:@[how, row(@[keyField, save]), open]];
    card.axis = UILayoutConstraintAxisVertical;
    card.spacing = 8;
    card.alignment = UIStackViewAlignmentLeading;
    [keyField.widthAnchor constraintGreaterThanOrEqualToConstant:220].active = YES;
    keyCard = card;
    [stack addArrangedSubview:keyCard];
    keyButton = button(@"API key saved · Change", @"key.fill", ^{ [weakSelf forgetKey]; });
    [stack addArrangedSubview:keyButton];
    pasteField = field(@"Paste a Workshop link or ID", self);
    UIButton *get = button(@"Get", @"arrow.down.circle", ^{ [weakSelf pasted]; });
    [stack addArrangedSubview:row(@[pasteField, get])];
    [self focusOn:get activate:^{ [weakSelf pasted]; } change:nil];
    status = label(@"", 15, UIFontWeightRegular, .8);
    [stack addArrangedSubview:status];
    cards = [UIStackView new];
    cards.axis = UILayoutConstraintAxisVertical;
    cards.spacing = 8;
    [stack addArrangedSubview:cards];
}
- (void)layoutSubviews {
    [super layoutSubviews];
    header.frame = CGRectMake(0, 0, self.bounds.size.width, 56);
    main.frame = CGRectMake(0, 56, self.bounds.size.width, self.bounds.size.height - 56);
    details.frame = main.frame;
}
- (void)setStatus:(NSString *)text { status.text = text; status.hidden = !text.length; }

// ---- the key ----
- (void)saveKey {
    NSString *k = [keyField.text stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet];
    if (!k.length) { [self setStatus:@"Paste the key first."]; return; }
    if (!key_save(k)) { [self setStatus:@"The key couldn't be saved in the Keychain."]; return; }
    key = k; keyField.text = @"";
    [self endEditing:YES];
    [self reload];
}
- (void)forgetKey {
    key_save(nil);
    key = nil;
    [self reload];
}

// ---- lists ----
- (BOOL)browsing { return tabs.selectedSegmentIndex <= 3; }
- (void)reload {
    ++request;
    [items removeAllObjects];
    for (UIView *v in cards.arrangedSubviews) [v removeFromSuperview];
    [focus filterUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(P2Focus *f, NSDictionary *b) {
        (void)b; return ![f.view isDescendantOfView:self->cards];
    }]];
    cursor = nil; ended = NO; loading = NO;
    searchRow.hidden = tabs.selectedSegmentIndex != 3;
    modes.hidden = tabs.selectedSegmentIndex == 4;
    keyCard.hidden = key != nil || ![self browsing];
    keyButton.hidden = key == nil || ![self browsing];
    [self setStatus:nil];
    if (tabs.selectedSegmentIndex == 4) { [self loadDownloaded]; return; }
    if (!key) { [self setStatus:@"Add your Steam Web API key above to browse."]; return; }
    if (tabs.selectedSegmentIndex == 3 && !searchField.text.length) { [self setStatus:@"Type a name and press Search."]; return; }
    [self loadMore];
}
- (void)loadMore {
    if (loading || ended || !key || ![self browsing]) return;
    char url[2048];
    if (!rt_ws_query_url(url, sizeof(url), key.UTF8String, (rt_ws_sort)tabs.selectedSegmentIndex, cursor.UTF8String,
                         searchField.text.UTF8String, (int)modes.selectedSegmentIndex)) return;
    loading = YES;
    if (!items.count) [self setStatus:@"Loading…"];
    int mine = request;
    [self fetch:@(url) body:nil then:^(NSData *data, NSError *error, NSInteger code) {
        if (mine != self->request) return;
        self->loading = NO;
        rt_ws_item found[20];
        char next[256];
        long long total = 0;
        int n = data && code == 200 ? rt_ws_parse_items(data.bytes, data.length, found, 20, next, sizeof(next), &total) : -1;
        if (n < 0) { [self setStatus:failure(error, code, YES)]; self->ended = YES; return; }
        NSString *nextCursor = @(next);
        self->ended = !n || !nextCursor.length || [nextCursor isEqualToString:self->cursor ?: @"*"];
        self->cursor = nextCursor;
        NSMutableArray<P2WSItem *> *page = [NSMutableArray array];
        for (int i = 0; i < n; ++i) if (found[i].found && found[i].is_map) {
            P2WSItem *w = [P2WSItem new]; w->it = found[i]; [page addObject:w];
        }
        [self addCards:page];
        [self setStatus:self->items.count ? nil : @"No chambers found."];
        [self fetchAuthors:page];
    }];
}
// Already downloaded maps, then their Workshop details for the update badge.
- (void)loadDownloaded {
    NSMutableArray<P2WSItem *> *list = [NSMutableArray array];
    for (NSString *name in [NSFileManager.defaultManager contentsOfDirectoryAtPath:workshop_dir() error:nil]) {
        unsigned long long id = strtoull(name.UTF8String, NULL, 10);
        if (!id || !saved_map(id)) continue;
        P2WSItem *w = [P2WSItem new];
        w->it.id = id; w->it.found = 1; w->it.is_map = 1;
        long long updated, size;
        char title[128];
        NSString *meta = [item_dir(id) stringByAppendingPathComponent:@"p2workshop.txt"];
        if (rt_ws_meta_read(meta.UTF8String, &updated, &size, title, sizeof(title))) {
            snprintf(w->it.title, sizeof(w->it.title), "%s", title);
            w->it.time_updated = updated; w->it.file_size = size;
        } else snprintf(w->it.title, sizeof(w->it.title), "%s", saved_map(id).stringByDeletingPathExtension.UTF8String);
        [list addObject:w];
    }
    ended = YES;
    if (!list.count) { [self setStatus:@"No downloaded maps yet."]; return; }
    [self addCards:list];
    [self refreshDetails:list];
}
// Keyless details for these items: fills in previews, descriptions and the
// Workshop's update date, then redraws their cards (update badges).
- (void)refreshDetails:(NSArray<P2WSItem *> *)list {
    if (!list.count) return;
    unsigned long long ids[50];
    int n = 0;
    for (P2WSItem *w in list) if (n < 50) ids[n++] = w->it.id;
    char body[2048];
    if (!rt_ws_details_body(body, sizeof(body), ids, n)) return;
    int mine = request;
    [self fetch:kDetails body:@(body) then:^(NSData *data, NSError *error, NSInteger code) {
        (void)error;
        if (mine != self->request || code != 200) return;
        rt_ws_item found[50];
        int got = rt_ws_parse_items(data.bytes, data.length, found, 50, NULL, 0, NULL);
        for (int i = 0; i < got; ++i)
            for (P2WSItem *w in list)
                if (w->it.id == found[i].id && found[i].found) {
                    rt_ws_item saved = w->it;
                    w->it = found[i];
                    if (!w->it.title[0]) snprintf(w->it.title, sizeof(w->it.title), "%s", saved.title);
                    w.savedUpdated = saved.time_updated; w.savedSize = saved.file_size;
                }
        [self redrawCards];
    }];
}

// ---- network ----
- (void)fetch:(NSString *)url body:(NSString *)body then:(void (^)(NSData *, NSError *, NSInteger))done {
    NSMutableURLRequest *r = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]
                                                     cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:20];
    if (body) {
        r.HTTPMethod = @"POST";
        r.HTTPBody = [body dataUsingEncoding:NSUTF8StringEncoding];
        [r setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    }
    [[NSURLSession.sharedSession dataTaskWithRequest:r completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        NSInteger code = [response isKindOfClass:NSHTTPURLResponse.class] ? ((NSHTTPURLResponse *)response).statusCode : 0;
        dispatch_async(dispatch_get_main_queue(), ^{ done(data, error, code); });
    }] resume];
}
// Authors' names (needs the key; without it cards say "Workshop author").
- (void)fetchAuthors:(NSArray<P2WSItem *> *)page {
    if (!key || !page.count) return;
    NSMutableOrderedSet<NSString *> *ids = [NSMutableOrderedSet orderedSet];
    for (P2WSItem *w in page) if (w->it.creator[0]) [ids addObject:@(w->it.creator)];
    if (!ids.count) return;
    NSString *url = [NSString stringWithFormat:@"%@?key=%@&steamids=%@", kNames,
                     [key stringByAddingPercentEncodingWithAllowedCharacters:NSCharacterSet.alphanumericCharacterSet],
                     [ids.array componentsJoinedByString:@","]];
    int mine = request;
    [self fetch:url body:nil then:^(NSData *data, NSError *error, NSInteger code) {
        (void)error;
        if (mine != self->request || code != 200) return;
        NSArray<NSString *> *list = ids.array;
        const char *c_ids[100];
        char (*names)[64] = calloc(list.count, 64);
        for (NSUInteger i = 0; i < list.count && i < 100; ++i) c_ids[i] = list[i].UTF8String;
        if (names && rt_ws_parse_names(data.bytes, data.length, c_ids, (int)MIN(list.count, 100u), names) > 0)
            for (P2WSItem *w in page)
                for (NSUInteger i = 0; i < list.count && i < 100; ++i)
                    if (!strcmp(w->it.creator, c_ids[i]) && names[i][0]) w.author = @(names[i]);
        free(names);
        [self redrawCards];
    }];
}
- (void)loadImage:(NSString *)url into:(UIImageView *)view {
    if (!url.length) return;
    UIImage *cached = [images objectForKey:url];
    if (cached) { view.image = cached; return; }
    __weak UIImageView *weakView = view;
    [[NSURLSession.sharedSession dataTaskWithURL:[NSURL URLWithString:url] completionHandler:^(NSData *data, NSURLResponse *r, NSError *e) {
        (void)r; (void)e;
        UIImage *image = data ? [UIImage imageWithData:data] : nil;
        if (!image) return;
        dispatch_async(dispatch_get_main_queue(), ^{ [self->images setObject:image forKey:url]; weakView.image = image; });
    }] resume];
}

// ---- cards ----
- (NSString *)badgesOf:(P2WSItem *)w {
    NSMutableArray<NSString *> *b = [NSMutableArray array];
    if (w->it.coop) [b addObject:@"Co-op"];
    if (saved_map(w->it.id)) {
        long long updated = w.savedUpdated, size = w.savedSize;
        char title[8];
        if (!updated) {
            NSString *meta = [item_dir(w->it.id) stringByAppendingPathComponent:@"p2workshop.txt"];
            rt_ws_meta_read(meta.UTF8String, &updated, &size, title, sizeof(title));
        }
        [b addObject:rt_ws_update_available(updated, size, w->it.time_updated, w->it.file_size) ? @"Update available" : @"Downloaded"];
    }
    return [b componentsJoinedByString:@" · "];
}
- (NSString *)infoOf:(P2WSItem *)w {
    NSMutableArray<NSString *> *parts = [NSMutableArray array];
    [parts addObject:[@"by " stringByAppendingString:w.author ?: @"a Workshop author"]];
    if (w->it.subscriptions) [parts addObject:[NSString stringWithFormat:@"%@ subscribers",
        [NSNumberFormatter localizedStringFromNumber:@(w->it.subscriptions) numberStyle:NSNumberFormatterDecimalStyle]]];
    if (w->it.file_size) [parts addObject:megabytes(w->it.file_size)];
    return [parts componentsJoinedByString:@" · "];
}
- (void)addCards:(NSArray<P2WSItem *> *)page {
    __weak P2CommunityMaps *weakSelf = self;
    for (P2WSItem *w in page) {
        [items addObject:w];
        UIControl *card = [UIControl new];
        card.backgroundColor = [UIColor colorWithWhite:1 alpha:.07];
        card.layer.cornerRadius = 12;
        UIImageView *preview = [UIImageView new];
        preview.contentMode = UIViewContentModeScaleAspectFill;
        preview.clipsToBounds = YES; preview.layer.cornerRadius = 8;
        preview.backgroundColor = [UIColor colorWithWhite:1 alpha:.08];
        UILabel *title = label(@(w->it.title[0] ? w->it.title : "Untitled"), 16, UIFontWeightSemibold, 1);
        title.numberOfLines = 2;
        UILabel *info = label([self infoOf:w], 13, UIFontWeightRegular, .65);
        UILabel *badges = label([self badgesOf:w], 13, UIFontWeightSemibold, 1);
        badges.textColor = [UIColor colorWithRed:.45 green:.78 blue:1 alpha:1];
        UIStackView *text = [[UIStackView alloc] initWithArrangedSubviews:@[title, info, badges]];
        text.axis = UILayoutConstraintAxisVertical; text.spacing = 3;
        UIStackView *line = [[UIStackView alloc] initWithArrangedSubviews:@[preview, text]];
        line.spacing = 12; line.alignment = UIStackViewAlignmentCenter;
        line.userInteractionEnabled = NO;
        line.translatesAutoresizingMaskIntoConstraints = NO;
        [card addSubview:line];
        [NSLayoutConstraint activateConstraints:@[
            [preview.widthAnchor constraintEqualToConstant:128], [preview.heightAnchor constraintEqualToConstant:72],
            [line.leadingAnchor constraintEqualToAnchor:card.leadingAnchor constant:8],
            [line.trailingAnchor constraintEqualToAnchor:card.trailingAnchor constant:-8],
            [line.topAnchor constraintEqualToAnchor:card.topAnchor constant:8],
            [line.bottomAnchor constraintEqualToAnchor:card.bottomAnchor constant:-8],
        ]];
        [card addAction:[UIAction actionWithHandler:^(UIAction *a) { (void)a; [weakSelf showDetails:w]; }]
       forControlEvents:UIControlEventTouchUpInside];
        [self loadImage:@(w->it.preview_url) into:preview];
        [cards addArrangedSubview:card];
        [self focusOn:card activate:^{ [weakSelf showDetails:w]; } change:nil];
    }
}
// Info and badges again (authors arrived, details refreshed, a download finished).
- (void)redrawCards {
    NSArray<UIView *> *views = cards.arrangedSubviews;
    for (NSUInteger i = 0; i < views.count && i < items.count; ++i) {
        UIStackView *line = (UIStackView *)views[i].subviews.firstObject;
        UIStackView *text = (UIStackView *)line.arrangedSubviews.lastObject;
        ((UILabel *)text.arrangedSubviews[0]).text = @(items[i]->it.title[0] ? items[i]->it.title : "Untitled");
        ((UILabel *)text.arrangedSubviews[1]).text = [self infoOf:items[i]];
        ((UILabel *)text.arrangedSubviews[2]).text = [self badgesOf:items[i]];
        [self loadImage:@(items[i]->it.preview_url) into:(UIImageView *)line.arrangedSubviews.firstObject];
    }
}
- (void)scrollViewDidScroll:(UIScrollView *)view {
    if (view == main && view.contentOffset.y + view.bounds.size.height > view.contentSize.height - 400) [self loadMore];
}

// ---- pasted links and collections ----
- (BOOL)textFieldShouldReturn:(UITextField *)f {
    if (f == searchField) [self reload];
    else if (f == keyField) [self saveKey];
    else if (f == pasteField) [self pasted];
    [f resignFirstResponder];
    return NO;
}
- (void)pasted {
    unsigned long long id = 0;
    if (!rt_ws_parse_link(pasteField.text.UTF8String, &id)) {
        [self setStatus:@"That isn't a Workshop link or ID. Copy the chamber's page link from Steam, "
                         "or the number after id= in it."];
        return;
    }
    [self endEditing:YES];
    [self openItem:id then:nil];
}
// Details of one item; a collection becomes the list of its maps.
- (void)openItem:(unsigned long long)id then:(void (^)(P2WSItem *))then {
    char body[128];
    unsigned long long one[] = {id};
    rt_ws_details_body(body, sizeof(body), one, 1);
    [self setStatus:@"Asking Steam…"];
    int mine = ++request;
    [self fetch:kDetails body:@(body) then:^(NSData *data, NSError *error, NSInteger code) {
        if (mine != self->request) return;
        rt_ws_item it;
        int n = data && code == 200 ? rt_ws_parse_items(data.bytes, data.length, &it, 1, NULL, 0, NULL) : -1;
        if (n < 0) { [self setStatus:failure(error, code, NO)]; return; }
        if (!n || !it.found) { [self setStatus:@"No Portal 2 chamber has that ID (removed, private, or from another game)."]; return; }
        if (it.is_collection) { [self openCollection:id title:@(it.title)]; return; }
        if (!it.is_map) { [self setStatus:@"That Workshop item isn't a map."]; return; }
        [self setStatus:nil];
        P2WSItem *w = [P2WSItem new];
        w->it = it;
        [self showDetails:w];
        if (then) then(w);
    }];
}
- (void)openCollection:(unsigned long long)id title:(NSString *)title {
    char body[128];
    rt_ws_collection_body(body, sizeof(body), id);
    int mine = request;
    [self fetch:kCollection body:@(body) then:^(NSData *data, NSError *error, NSInteger code) {
        if (mine != self->request) return;
        unsigned long long ids[50];
        int n = data && code == 200 ? rt_ws_parse_collection(data.bytes, data.length, ids, 50) : -1;
        if (n <= 0) { [self setStatus:n < 0 ? failure(error, code, NO) : @"That collection is empty."]; return; }
        char body2[2048];
        rt_ws_details_body(body2, sizeof(body2), ids, n);
        [self fetch:kDetails body:@(body2) then:^(NSData *data2, NSError *error2, NSInteger code2) {
            if (mine != self->request) return;
            rt_ws_item found[50];
            int got = data2 && code2 == 200 ? rt_ws_parse_items(data2.bytes, data2.length, found, 50, NULL, 0, NULL) : -1;
            if (got < 0) { [self setStatus:failure(error2, code2, NO)]; return; }
            [self clearList];
            NSMutableArray<P2WSItem *> *page = [NSMutableArray array];
            for (int i = 0; i < got; ++i) if (found[i].found && found[i].is_map) {
                P2WSItem *w = [P2WSItem new]; w->it = found[i]; [page addObject:w];
            }
            [self setStatus:[NSString stringWithFormat:@"Collection “%@”: %lu maps.", title, (unsigned long)page.count]];
            [self addCards:page];
        }];
    }];
}
- (void)clearList {
    [items removeAllObjects];
    for (UIView *v in cards.arrangedSubviews) [v removeFromSuperview];
    [focus filterUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(P2Focus *f, NSDictionary *b) {
        (void)b; return ![f.view isDescendantOfView:self->cards];
    }]];
    ended = YES;
}

// ---- details ----
- (void)showDetails:(P2WSItem *)w {
    [details removeFromSuperview];
    shown = w;
    deleteArmedUntil = 0;
    details = [UIScrollView new];
    details.backgroundColor = [UIColor colorWithWhite:.06 alpha:1];
    details.indicatorStyle = UIScrollViewIndicatorStyleWhite;
    details.frame = main.frame;
    [self addSubview:details];
    UIStackView *s = [UIStackView new];
    s.axis = UILayoutConstraintAxisVertical; s.spacing = 10;
    s.layoutMargins = UIEdgeInsetsMake(8, 16, 24, 16); s.layoutMarginsRelativeArrangement = YES;
    s.translatesAutoresizingMaskIntoConstraints = NO;
    [details addSubview:s];
    [NSLayoutConstraint activateConstraints:@[
        [s.topAnchor constraintEqualToAnchor:details.contentLayoutGuide.topAnchor],
        [s.bottomAnchor constraintEqualToAnchor:details.contentLayoutGuide.bottomAnchor],
        [s.leadingAnchor constraintEqualToAnchor:details.contentLayoutGuide.leadingAnchor],
        [s.trailingAnchor constraintEqualToAnchor:details.contentLayoutGuide.trailingAnchor],
        [s.widthAnchor constraintEqualToAnchor:details.frameLayoutGuide.widthAnchor],
    ]];
    UIImageView *preview = [UIImageView new];
    preview.contentMode = UIViewContentModeScaleAspectFit;
    preview.backgroundColor = [UIColor colorWithWhite:1 alpha:.05];
    preview.layer.cornerRadius = 12; preview.clipsToBounds = YES;
    [preview.heightAnchor constraintEqualToConstant:200].active = YES;
    [self loadImage:@(w->it.preview_url) into:preview];
    [s addArrangedSubview:preview];
    [s addArrangedSubview:label(@(w->it.title[0] ? w->it.title : "Untitled"), 20, UIFontWeightBold, 1)];
    NSMutableString *info = [[self infoOf:w] mutableCopy];
    if (w->it.time_updated) {
        NSDateFormatter *f = [NSDateFormatter new];
        f.dateStyle = NSDateFormatterMediumStyle;
        [info appendFormat:@" · updated %@", [f stringFromDate:[NSDate dateWithTimeIntervalSince1970:w->it.time_updated]]];
    }
    [s addArrangedSubview:label(info, 14, UIFontWeightRegular, .7)];
    if (w->it.coop)
        [s addArrangedSubview:label(@"Co-op map: it needs a second player. Host it on a Mac (LAN co-op, docs/usage.md) "
                                    "and join from this device.", 14, UIFontWeightSemibold, .9)];
    detailButtons = [UIStackView new];
    detailButtons.spacing = 8;
    detailButtons.distribution = UIStackViewDistributionFillEqually;
    [s addArrangedSubview:detailButtons];
    progress = [[UIProgressView alloc] initWithProgressViewStyle:UIProgressViewStyleDefault];
    [s addArrangedSubview:progress];
    detailStatus = label(@"", 14, UIFontWeightRegular, .85);
    [s addArrangedSubview:detailStatus];
    if (w->it.description[0]) [s addArrangedSubview:label(@(w->it.description), 15, UIFontWeightRegular, .85)];
    [self updateDetailButtons];
    // Missing description or preview (from a list): fetch the full details.
    if (!w->it.description[0] || !w->it.file_url[0]) {
        unsigned long long one[] = {w->it.id};
        char body[128];
        rt_ws_details_body(body, sizeof(body), one, 1);
        [self fetch:kDetails body:@(body) then:^(NSData *data, NSError *error, NSInteger code) {
            (void)error;
            rt_ws_item it;
            if (self->shown != w || code != 200 || rt_ws_parse_items(data.bytes, data.length, &it, 1, NULL, 0, NULL) != 1 || !it.found) return;
            NSString *author = w.author;
            w->it = it; w.author = author;
            [self showDetails:w];
        }];
    }
}
- (void)updateDetailButtons {
    __weak P2CommunityMaps *weakSelf = self;
    for (UIView *v in detailButtons.arrangedSubviews) [v removeFromSuperview];
    [focus filterUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(P2Focus *f, NSDictionary *b) {
        (void)b; return f.view && ![f.view isDescendantOfView:self->details];
    }]];
    P2WSItem *w = shown;
    NSString *map = saved_map(w->it.id);
    BOOL busy = downloading && downloading->it.id == w->it.id;
    progress.hidden = !busy;
    NSMutableArray<UIButton *> *list = [NSMutableArray array];
    if (busy) [list addObject:button(@"Cancel", @"xmark", ^{ [weakSelf cancelDownload]; })];
    else if (!map) {
        UIButton *b = button(w->it.file_size ? [@"Download · " stringByAppendingString:megabytes(w->it.file_size)] : @"Download",
                             @"arrow.down.circle.fill", ^{ [weakSelf download:w]; });
        b.enabled = !downloading;
        [list addObject:b];
        if (!w->it.file_url[0] && w->it.description[0])
            detailStatus.text = @"Steam gives no download link for this chamber, so it can't be downloaded here.";
    } else {
        UIButton *play = button(@"Play", @"play.fill", ^{ [weakSelf play:w]; });
        play.enabled = !w->it.coop && rt_ios_game_commands_available();
        [list addObject:play];
        long long updated = 0, size = 0;
        char title[8];
        NSString *meta = [item_dir(w->it.id) stringByAppendingPathComponent:@"p2workshop.txt"];
        rt_ws_meta_read(meta.UTF8String, &updated, &size, title, sizeof(title));
        if (rt_ws_update_available(updated, size, w->it.time_updated, w->it.file_size))
            [list addObject:button(@"Update", @"arrow.triangle.2.circlepath", ^{ [weakSelf download:w]; })];
        [list addObject:button(@"Delete", @"trash", ^{ [weakSelf deletePressed:w]; })];
    }
    for (UIButton *b in list) {
        [b.heightAnchor constraintEqualToConstant:48].active = YES;
        [detailButtons addArrangedSubview:b];
        __weak UIButton *weakButton = b;
        [self focusOn:b activate:^{ if (weakButton.enabled) [weakButton sendActionsForControlEvents:UIControlEventTouchUpInside]; }
               change:nil];
    }
    focused = -1;
    for (NSUInteger i = 0; i < focus.count; ++i) if ([focus[i].view isDescendantOfView:details]) { focused = (NSInteger)i; break; }
    [self showFocus];
}
- (void)hideDetails {
    [details removeFromSuperview];
    details = nil; shown = nil;
    [focus filterUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(P2Focus *f, NSDictionary *b) {
        (void)b; return f.view != nil && f.view.window != nil;
    }]];
    focused = -1;
    [self redrawCards];
}

// ---- download, play, delete ----
- (void)download:(P2WSItem *)w {
    if (downloading) return;
    if (!w->it.file_url[0]) { detailStatus.text = @"Steam gives no download link for this chamber."; return; }
    NSDictionary *space = [[NSURL fileURLWithPath:NSHomeDirectory()]
                           resourceValuesForKeys:@[NSURLVolumeAvailableCapacityForImportantUsageKey] error:nil];
    long long free_bytes = [space[NSURLVolumeAvailableCapacityForImportantUsageKey] longLongValue];
    if (free_bytes && free_bytes < w->it.file_size + (64 << 20)) {
        detailStatus.text = [NSString stringWithFormat:@"Not enough storage: this map needs %@, and %@ is free. "
                             "Free some space and try again.", megabytes(w->it.file_size), megabytes(free_bytes)];
        return;
    }
    downloading = w;
    task = [downloads downloadTaskWithURL:[NSURL URLWithString:@(w->it.file_url)]];
    [task resume];
    progress.progress = 0;
    detailStatus.text = @"Downloading…";
    [self updateDetailButtons];
    fprintf(stderr, "[ios] carte %llu : téléchargement (%lld octets)\n", w->it.id, w->it.file_size);
}
- (void)cancelDownload { [task cancel]; }
- (void)URLSession:(NSURLSession *)s downloadTask:(NSURLSessionDownloadTask *)t didWriteData:(int64_t)bytes
 totalBytesWritten:(int64_t)written totalBytesExpectedToWrite:(int64_t)expected {
    (void)s; (void)t; (void)bytes;
    long long total = expected > 0 ? expected : downloading->it.file_size;
    if (total > 0 && shown == downloading) {
        progress.progress = (float)written / (float)total;
        detailStatus.text = [NSString stringWithFormat:@"Downloading… %@ of %@", megabytes(written), megabytes(total)];
    }
}
// The file must be moved before this returns. It becomes a map only once checked.
- (void)URLSession:(NSURLSession *)s downloadTask:(NSURLSessionDownloadTask *)t didFinishDownloadingToURL:(NSURL *)location {
    (void)s;
    P2WSItem *w = downloading;
    NSInteger code = [t.response isKindOfClass:NSHTTPURLResponse.class] ? ((NSHTTPURLResponse *)t.response).statusCode : 0;
    NSFileManager *files = NSFileManager.defaultManager;
    char relative[512];
    rt_ws_map_path(&w->it, relative, sizeof(relative));
    NSString *final = [@(rt_user_root()) stringByAppendingPathComponent:@(relative)];
    NSString *dir = final.stringByDeletingLastPathComponent, *part = [final stringByAppendingString:@".part"];
    [files createDirectoryAtPath:dir withIntermediateDirectories:YES attributes:nil error:nil];
    [[NSURL fileURLWithPath:workshop_dir()] setResourceValue:@YES forKey:NSURLIsExcludedFromBackupKey error:nil];
    [files removeItemAtPath:part error:nil];
    NSString *problem = nil;
    if (code != 200) problem = failure(nil, code, NO);
    else if (![files moveItemAtURL:location toURL:[NSURL fileURLWithPath:part] error:nil]) problem = @"The map couldn't be saved.";
    else {
        long long size = (long long)[files attributesOfItemAtPath:part error:nil].fileSize;
        NSFileHandle *h = [NSFileHandle fileHandleForReadingAtPath:part];
        NSData *head = [h readDataOfLength:4];
        [h closeFile];
        if ((w->it.file_size && size != w->it.file_size) || ![head isEqualToData:[NSData dataWithBytes:"VBSP" length:4]])
            problem = @"The file Steam sent isn't a complete Portal 2 map. Try again later.";
    }
    if (!problem) {
        // An update replaces the old copy, whatever its name was.
        for (NSString *name in [files contentsOfDirectoryAtPath:dir error:nil])
            if ([name.pathExtension.lowercaseString isEqualToString:@"bsp"]) [files removeItemAtPath:[dir stringByAppendingPathComponent:name] error:nil];
        NSString *meta = [dir stringByAppendingPathComponent:@"p2workshop.txt"];
        if (!rt_ws_meta_write(meta.UTF8String, &w->it) || ![files moveItemAtPath:part toPath:final error:nil])
            problem = @"The map couldn't be saved.";
    }
    if (problem) {
        [files removeItemAtPath:part error:nil];
        rt_ios_community_cleanup();
        w.failure = problem;
    }
}
- (void)URLSession:(NSURLSession *)s task:(NSURLSessionTask *)t didCompleteWithError:(NSError *)error {
    (void)s; (void)t;
    P2WSItem *w = downloading;
    downloading = nil; task = nil;
    NSString *problem = error ? failure(error, 0, NO) : w.failure;
    w.failure = nil;
    if (error) rt_ios_community_cleanup();   // canceled or cut: nothing half-written stays
    fprintf(stderr, "[ios] carte %llu : %s\n", w->it.id, problem ? problem.UTF8String : "téléchargée");
    if (shown == w) {
        detailStatus.text = problem ?: @"Downloaded. It's also in Maps & Mods.";
        [self updateDetailButtons];
    }
    [self redrawCards];
    if (!problem && playAfterDownload) { playAfterDownload = NO; [self play:w]; }
}
- (void)play:(P2WSItem *)w {
    NSString *map = saved_map(w->it.id);
    char name[256];
    NSString *relative = [NSString stringWithFormat:@"portal2/maps/workshop/%llu/%@", w->it.id, map];
    if (!map || !rt_mods_map_name(relative.UTF8String, name, sizeof(name))) { detailStatus.text = @"This map can't be started."; return; }
    if (w->it.coop) { detailStatus.text = @"Co-op maps need a second player."; return; }
    NSString *command = [@"map " stringByAppendingString:@(name)];
    if (!rt_ios_game_command(command.UTF8String)) { detailStatus.text = @"The game can't start maps from here right now."; return; }
    onClose(closeCtx, 1);
}
- (void)deletePressed:(P2WSItem *)w {
    CFTimeInterval now = CACurrentMediaTime();
    if (now > deleteArmedUntil) {
        deleteArmedUntil = now + 3;
        detailStatus.text = @"Press Delete again to remove this map from the device.";
        return;
    }
    deleteArmedUntil = 0;
    rt_ws_delete(item_dir(w->it.id).UTF8String);
    detailStatus.text = @"Deleted.";
    [self updateDetailButtons];
    if (tabs.selectedSegmentIndex == 4) { [self hideDetails]; [self reload]; }
}

// ---- back and the controller ----
- (void)backPressed {
    if (details) { [self hideDetails]; return; }
    onClose(closeCtx, 0);
}
- (void)showFocus {
    for (NSUInteger i = 0; i < focus.count; ++i) {
        UIView *v = focus[i].view;
        BOOL on = (NSInteger)i == focused;
        v.layer.borderWidth = on ? 2 : 0;
        v.layer.borderColor = [UIColor colorWithRed:.45 green:.75 blue:1 alpha:.95].CGColor;
        if (on) {
            UIScrollView *scroll = details && [v isDescendantOfView:details] ? details : main;
            [scroll scrollRectToVisible:CGRectInset([v convertRect:v.bounds toView:scroll], 0, -30) animated:YES];
        }
    }
}
- (void)padDx:(int)dx dy:(int)dy a:(BOOL)a b:(BOOL)b {
    if (b) { [self backPressed]; return; }
    // Only what is on screen: the details' buttons while they're open, the list otherwise.
    NSMutableArray<NSNumber *> *reach = [NSMutableArray array];
    for (NSUInteger i = 0; i < focus.count; ++i) {
        UIView *v = focus[i].view;
        if (!v || v.hidden || !v.window) continue;
        if (![v isDescendantOfView:details ?: main]) continue;   // the details page while it's open, the list otherwise
        [reach addObject:@(i)];
    }
    if (!reach.count) return;
    NSUInteger at = [reach indexOfObject:@(focused)];
    if (dy) {
        at = at == NSNotFound ? 0 : (at + reach.count + (NSUInteger)(dy > 0 ? -1 : 1)) % reach.count;
        focused = reach[at].integerValue;
        [self showFocus];
        if (!details && at + 1 == reach.count) [self loadMore];
    }
    if (at == NSNotFound) return;
    P2Focus *f = focus[focused];
    if (dx) { if (f.change) f.change(dx); else if (details && dx) [self padDx:0 dy:-dx a:NO b:NO]; }
    if (a && f.activate) f.activate();
}
- (void)openId:(unsigned long long)id play:(BOOL)play {
    __weak P2CommunityMaps *weakSelf = self;
    [self openItem:id then:^(P2WSItem *w) {
        P2CommunityMaps *s = weakSelf;
        if (!s || !play) return;
        if (saved_map(w->it.id)) { [s play:w]; return; }
        s->playAfterDownload = YES;
        [s download:w];
    }];
}
@end

void *rt_ios_community_create(void (*on_close)(void *ctx, int all), void *ctx) {
    return (__bridge_retained void *)[[P2CommunityMaps alloc] initWithClose:on_close ctx:ctx];
}
void rt_ios_community_pad(void *view, int dx, int dy, int a, int b) {
    [(__bridge P2CommunityMaps *)view padDx:dx dy:dy a:a != 0 b:b != 0];
}
void rt_ios_community_open_item(void *view, unsigned long long id, int download_and_play) {
    [(__bridge P2CommunityMaps *)view openId:id play:download_and_play != 0];
}
#endif
