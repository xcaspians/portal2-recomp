// Sauce's launcher and "Manage games" screen (see game_picker_ios.h).
//
// Shown by ios/main_ios.c before anything game-specific, in a window of its
// own over SDL's (not created yet): the run loop turns until a game is
// chosen, then the window goes away and the game starts as before. Round 23:
// series on the main screen (game_picker.c's table) with a "Continue" card for
// the last game, each series' games on equal cards (covers/<name>.png, made by
// tools/build_ios.py from the game's own icon on the Mac; the episodes and
// Lost Coast carry a label), touch or controller focus. A game not on the
// device says how to get it from the Mac. Manage games: data and saves sizes,
// last played, "Remove game data" (game, prep and linux only; saves and
// settings stay). Rules in game_picker.c.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <GameController/GameController.h>
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_ios.h"
#include "game_picker.h"
#include "game_picker_ios.h"
#include "rt.h"
#include "tv_output.h"

#ifdef RT_MULTI_GAME
static NSString *const kRemember = @"P2GameRemember", *const kLast = @"P2GameLast";

static NSString *documents(void) {
    return NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
}
static rt_picker_game picker_game(const rt_ios_game *g) {
    return (rt_picker_game){g->name, g->mod, g->data_dir, g->userdata, g->base};
}
static BOOL installed(const rt_ios_game *g) {
    rt_picker_game p = picker_game(g);
    char rel[256];
    for (int i = 0; i < 2; ++i)   // game/<mod> and prep: linux is copied with them
        if (![NSFileManager.defaultManager fileExistsAtPath:[documents() stringByAppendingPathComponent:
                                                                 @(rt_picker_data_folder(&p, i, rel, sizeof(rel)))]])
            return NO;
    return YES;
}
// Slim pack without Portal 2's files: it can't start (a sentence says why).
static BOOL slim(const rt_ios_game *g) {
    rt_picker_game p = picker_game(g);
    char rel[256];
    return rt_picker_slim_marker(&p, rel, sizeof(rel)) &&
           [NSFileManager.defaultManager fileExistsAtPath:[documents() stringByAppendingPathComponent:@(rel)]];
}
static BOOL needs_portal2(const rt_ios_game *g) {
    return installed(g) && slim(g) &&
           ![NSFileManager.defaultManager fileExistsAtPath:[documents() stringByAppendingPathComponent:@RT_PICKER_BASE_FILE]];
}
static BOOL playable(const rt_ios_game *g) { return installed(g) && !needs_portal2(g); }
// Bytes under a folder (allocated size, what deleting frees); 0 if absent.
static long long folder_size(NSString *path) {
    long long total = 0;
    NSArray *keys = @[NSURLTotalFileAllocatedSizeKey, NSURLIsRegularFileKey];
    NSDirectoryEnumerator *e = [NSFileManager.defaultManager enumeratorAtURL:[NSURL fileURLWithPath:path]
                                                  includingPropertiesForKeys:keys options:0 errorHandler:nil];
    for (NSURL *url in e) {
        NSDictionary *v = [url resourceValuesForKeys:keys error:nil];
        if ([v[NSURLIsRegularFileKey] boolValue]) total += [v[NSURLTotalFileAllocatedSizeKey] longLongValue];
    }
    return total;
}
static long long data_size(const rt_ios_game *g) {
    rt_picker_game p = picker_game(g);
    char rel[256];
    long long total = 0;
    for (int i = 0; rt_picker_owned(&p, i, rel, sizeof(rel)); ++i)
        total += folder_size([documents() stringByAppendingPathComponent:@(rel)]);
    return total;
}
static long long saves_size(const rt_ios_game *g) {
    return folder_size([NSString pathWithComponents:@[documents(), @(g->userdata), @(g->mod), @"SAVE"]]);
}
static NSString *size_text(long long bytes) {
    char s[32];
    rt_picker_size(bytes, s, sizeof(s));
    return @(s);
}
static NSString *last_played(const rt_ios_game *g) {
    NSDate *d = [NSUserDefaults.standardUserDefaults objectForKey:[@"P2GamePlayed." stringByAppendingString:@(g->name)]];
    if (![d isKindOfClass:NSDate.class]) return @"never";
    NSDateFormatter *f = [NSDateFormatter new];
    f.doesRelativeDateFormatting = YES;
    f.dateStyle = NSDateFormatterMediumStyle; f.timeStyle = NSDateFormatterShortStyle;
    return [f stringFromDate:d];
}
static UILabel *text(NSString *s, CGFloat size, UIFontWeight w, CGFloat alpha) {
    UILabel *l = [UILabel new];
    l.text = s; l.font = [UIFont systemFontOfSize:size weight:w];
    l.textColor = [UIColor colorWithWhite:1 alpha:alpha];
    l.numberOfLines = 0;
    return l;
}

// ---- The launcher (round 23) ----
// Main screen: a large "Continue" card for the last game played, then one tile
// per series (game_picker.c's table). A series shows its games, every card the
// same size, titles on one line (shrunk to fit), sizes and buttons at the same
// height. Touch, or a controller: D-pad or left stick moves the focus, A plays
// or opens, B goes back. Tunables below (sizes in points), scaled by
// P2_LAUNCHER_SCALE (1 by default).
static const CGFloat kCardW = 190, kCardH = 300, kArt = 166, kSeriesW = 270, kSeriesH = 320, kGap = 24;
static const CGFloat kFeatureH = 210, kFocusScale = 1.06;
static const double kStickRepeat = .22;   // seconds between focus moves while a stick is held

static CGFloat launcher_scale(void) {
    const char *v = getenv("P2_LAUNCHER_SCALE");
    double s = v ? atof(v) : 1;
    return s >= .5 && s <= 2 ? (CGFloat)s : 1;
}
static UIImage *cover_of(const char *name) {
    NSString *path = [NSBundle.mainBundle pathForResource:[@"covers/" stringByAppendingString:@(name)] ofType:@"png"];
    return path ? [UIImage imageWithContentsOfFile:path] : [UIImage systemImageNamed:@"gamecontroller.fill"];
}
static UILabel *one_line(NSString *s, CGFloat size, UIFontWeight w, CGFloat alpha) {
    UILabel *l = text(s, size, w, alpha);
    l.numberOfLines = 1;
    l.adjustsFontSizeToFitWidth = YES;   // long names shrink instead of wrapping
    l.minimumScaleFactor = .55;
    l.textAlignment = NSTextAlignmentCenter;
    return l;
}
static UIColor *accent(void) { return [UIColor colorWithRed:.98 green:.55 blue:.16 alpha:1]; }

// A focusable card: art frame, labels, and an action. Same frame for every card of a kind.
@interface P2Card : UIControl
@property (nonatomic, copy) void (^action)(void);
@property (nonatomic) BOOL lit;   // the controller focus is here
@property (nonatomic, strong) UILabel *detail;
@end
@implementation P2Card
- (instancetype)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        self.layer.cornerRadius = 18;
        self.backgroundColor = [UIColor colorWithWhite:1 alpha:.06];
        self.layer.borderWidth = 2;
        self.layer.borderColor = UIColor.clearColor.CGColor;
        self.layer.shadowColor = UIColor.blackColor.CGColor;
        self.layer.shadowOpacity = .45; self.layer.shadowRadius = 14; self.layer.shadowOffset = CGSizeMake(0, 8);
        [self addTarget:self action:@selector(fire) forControlEvents:UIControlEventTouchUpInside];
    }
    return self;
}
- (void)fire { if (self.action) self.action(); }
- (void)setLit:(BOOL)focused {
    _lit = focused;
    [UIView animateWithDuration:.18 delay:0 usingSpringWithDamping:.75 initialSpringVelocity:0 options:0 animations:^{
        self.transform = focused ? CGAffineTransformMakeScale(kFocusScale, kFocusScale) : CGAffineTransformIdentity;
        self.layer.borderColor = (focused ? [UIColor colorWithWhite:1 alpha:.9] : UIColor.clearColor).CGColor;
        self.backgroundColor = [UIColor colorWithWhite:1 alpha:focused ? .14 : .06];
    } completion:nil];
}
- (void)setHighlighted:(BOOL)highlighted {
    [super setHighlighted:highlighted];
    [UIView animateWithDuration:.12 animations:^{
        self.transform = highlighted ? CGAffineTransformMakeScale(.97, .97)
                                     : self.lit ? CGAffineTransformMakeScale(kFocusScale, kFocusScale) : CGAffineTransformIdentity;
    }];
}
@end

@interface P2GamePicker : UIViewController
@property (nonatomic, copy) NSString *chosen;
@end

@implementation P2GamePicker {
    UIScrollView *scroll;
    UIView *page;                      // the current screen's content
    const rt_picker_series *series;    // NULL: the main screen
    NSMutableArray<P2Card *> *cards;   // focusable, in reading order
    NSInteger focus;                   // index in cards, -1: none (touch only)
    UIStackView *manage;
    UIScrollView *manageScroll;
    BOOL remember;
    NSMutableDictionary<NSString *, UILabel *> *sizeLabels;
    NSMutableDictionary<NSString *, NSString *> *sizes;
    CFTimeInterval lastStick;
}
- (const rt_ios_game *)game:(size_t)i { return rt_ios_game_named(rt_game_codes[i].name); }
// The games of a series this app holds, in the series' order.
- (NSArray<NSString *> *)gamesOf:(const rt_picker_series *)s {
    NSMutableArray *out = [NSMutableArray array];
    for (int j = 0; s->games[j]; ++j)
        for (size_t i = 0; i < rt_game_code_count; ++i)
            if (!strcmp(rt_game_codes[i].name, s->games[j])) [out addObject:@(s->games[j])];
    return out;
}
- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor colorWithRed:.035 green:.035 blue:.05 alpha:1];
    CAGradientLayer *glow = [CAGradientLayer layer];   // a faint warm light from the top
    glow.colors = @[(id)[accent() colorWithAlphaComponent:.16].CGColor, (id)UIColor.clearColor.CGColor];
    glow.frame = CGRectMake(0, 0, 4000, 420);
    [self.view.layer addSublayer:glow];
    sizeLabels = [NSMutableDictionary dictionary];
    sizes = [NSMutableDictionary dictionary];
    remember = [NSUserDefaults.standardUserDefaults boolForKey:kRemember];
    focus = GCController.controllers.count ? 0 : -1;
    scroll = [UIScrollView new];
    [self.view addSubview:scroll];
    [self watchControllers];
    [self measure];
    [self show:NULL animated:NO];
}
- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    UIEdgeInsets safe = self.view.safeAreaInsets;
    CGRect frame = UIEdgeInsetsInsetRect(self.view.bounds, safe);
    if (!CGRectEqualToRect(scroll.frame, frame)) { scroll.frame = frame; [self show:series animated:NO]; }
}

// ---- Screens ----
- (void)show:(const rt_picker_series *)s animated:(BOOL)animated {
    series = s;
    UIView *old = page;
    page = [[UIView alloc] initWithFrame:scroll.bounds];
    cards = [NSMutableArray array];
    CGFloat height = s ? [self buildSeries:s] : [self buildMain];
    page.frame = CGRectMake(0, 0, scroll.bounds.size.width, height);
    scroll.contentSize = page.frame.size;
    [scroll addSubview:page];
    if (focus >= (NSInteger)cards.count) focus = cards.count ? 0 : -1;
    [self applyFocus];
    if (animated && old) {
        CGFloat dx = s ? 60 : -60;
        page.alpha = 0; page.transform = CGAffineTransformMakeTranslation(dx, 0);
        [UIView animateWithDuration:.28 delay:0 options:UIViewAnimationOptionCurveEaseOut animations:^{
            self->page.alpha = 1; self->page.transform = CGAffineTransformIdentity;
            old.alpha = 0; old.transform = CGAffineTransformMakeTranslation(-dx, 0);
        } completion:^(BOOL done) { (void)done; [old removeFromSuperview]; }];
    } else [old removeFromSuperview];
    scroll.contentOffset = CGPointZero;
}
- (UILabel *)title:(NSString *)t at:(CGFloat)y size:(CGFloat)size {
    UILabel *l = text(t, size, UIFontWeightHeavy, 1);
    l.numberOfLines = 1;
    l.frame = CGRectMake(32, y, page.bounds.size.width - 64, size * 1.3);
    [page addSubview:l];
    return l;
}
// Main: header, the featured "Continue" card, the series tiles, remember and manage.
- (CGFloat)buildMain {
    CGFloat k = launcher_scale(), w = page.bounds.size.width, y = 24;
    NSString *app = [NSBundle.mainBundle objectForInfoDictionaryKey:@"CFBundleDisplayName"] ?: @"Games";
    [self title:app at:y size:34 * k];
    y += 34 * k * 1.3 + 18;
    NSString *last = [NSUserDefaults.standardUserDefaults stringForKey:kLast];
    const rt_ios_game *lg = last ? rt_ios_game_named(last.UTF8String) : NULL;
    BOOL has_last = lg && !strcmp(lg->name, last.UTF8String) && playable(lg);
    if (has_last) {
        CGFloat fw = MIN(w - 64, 760 * k), fh = kFeatureH * k;
        P2Card *f = [[P2Card alloc] initWithFrame:CGRectMake((w - fw) / 2, y, fw, fh)];
        f.backgroundColor = [UIColor colorWithWhite:1 alpha:.08];
        UIImageView *art = [[UIImageView alloc] initWithImage:cover_of(lg->name)];
        art.contentMode = UIViewContentModeScaleAspectFit;
        art.frame = CGRectMake(18 * k, 18 * k, fh - 36 * k, fh - 36 * k);
        art.layer.cornerRadius = 16; art.clipsToBounds = YES;
        art.userInteractionEnabled = NO;
        [f addSubview:art];
        CGFloat tx = fh + 6 * k, tw = fw - tx - 24 * k;
        UILabel *cap = text(@"CONTINUE", 13 * k, UIFontWeightBold, .55);
        cap.frame = CGRectMake(tx, 34 * k, tw, 18 * k);
        UILabel *name = text(@(lg->title), 34 * k, UIFontWeightBold, 1);
        name.numberOfLines = 1; name.adjustsFontSizeToFitWidth = YES; name.minimumScaleFactor = .5;
        name.frame = CGRectMake(tx, 56 * k, tw, 44 * k);
        const rt_picker_series *ls = rt_picker_series_of(lg->name);
        UILabel *sub = text([NSString stringWithFormat:@"%@ · last played %@", ls ? @(ls->title) : @"", last_played(lg)],
                            15 * k, UIFontWeightRegular, .6);
        sub.numberOfLines = 1; sub.frame = CGRectMake(tx, 104 * k, tw, 20 * k);
        UILabel *play = [self pill:@"▶  Play" filled:YES];
        play.frame = CGRectMake(tx, fh - 64 * k, 150 * k, 40 * k);
        for (UIView *v in @[cap, name, sub, play]) { v.userInteractionEnabled = NO; [f addSubview:v]; }
        __weak P2GamePicker *weak = self;
        f.action = ^{ [weak pick:last]; };
        [page addSubview:f];
        [cards addObject:f];
        y += fh + 34;
    }
    UILabel *lib = text(@"LIBRARY", 14 * k, UIFontWeightBold, .55);
    lib.frame = CGRectMake(32, y, 300, 20 * k);
    [page addSubview:lib];
    y += 20 * k + 14;
    // The series with at least one game in this app, side by side (wrapping if narrow).
    NSMutableArray *shown = [NSMutableArray array];
    for (int i = 0; i < rt_picker_series_count(); ++i)
        if ([self gamesOf:rt_picker_series_at(i)].count) [shown addObject:[NSValue valueWithPointer:rt_picker_series_at(i)]];
    CGFloat tw = kSeriesW * k, th = kSeriesH * k;
    y = [self grid:shown.count width:tw height:th top:y make:^P2Card *(NSUInteger i, CGRect frame) {
        return [self seriesTile:[shown[i] pointerValue] frame:frame];
    }];
    // Remember my choice and Manage games: focusable too.
    y += 26;
    CGFloat bw = 300 * k, bh = 50 * k, bx = (w - 2 * bw - 16) / 2;
    P2Card *rem = [[P2Card alloc] initWithFrame:CGRectMake(bx, y, bw, bh)];
    rem.layer.cornerRadius = bh / 2;
    UILabel *rl = one_line([self rememberText], 15 * k, UIFontWeightSemibold, .9);
    rl.frame = CGRectInset(rem.bounds, 14, 0); rl.userInteractionEnabled = NO;
    [rem addSubview:rl];
    __weak P2GamePicker *weak = self;
    __weak UILabel *wrl = rl;
    rem.action = ^{
        P2GamePicker *me = weak;
        if (!me) return;
        me->remember = !me->remember;
        [NSUserDefaults.standardUserDefaults setBool:me->remember forKey:kRemember];
        wrl.text = [me rememberText];
    };
    P2Card *man = [[P2Card alloc] initWithFrame:CGRectMake(bx + bw + 16, y, bw, bh)];
    man.layer.cornerRadius = bh / 2;
    UILabel *ml = one_line(@"Manage games", 15 * k, UIFontWeightSemibold, .9);
    ml.frame = CGRectInset(man.bounds, 14, 0); ml.userInteractionEnabled = NO;
    [man addSubview:ml];
    man.action = ^{ [weak toggleManage]; };
    for (P2Card *c in @[rem, man]) { [page addSubview:c]; [cards addObject:c]; }
    y += bh + 12;
    UILabel *hint = one_line(@"Remember goes straight into the last game next time; Switch game in the gear menu brings this back.",
                             12 * k, UIFontWeightRegular, .45);
    hint.frame = CGRectMake(32, y, w - 64, 18 * k);
    [page addSubview:hint];
    return y + 18 * k + 30;
}
- (NSString *)rememberText {
    return remember ? @"✓  Remember my game: on" : @"Remember my game: off";
}
- (UILabel *)pill:(NSString *)t filled:(BOOL)filled {
    UILabel *l = one_line(t, 16 * launcher_scale(), UIFontWeightBold, 1);
    l.backgroundColor = filled ? accent() : [UIColor colorWithWhite:1 alpha:.12];
    l.textColor = filled ? UIColor.blackColor : UIColor.whiteColor;
    l.layer.cornerRadius = 20 * launcher_scale(); l.clipsToBounds = YES;
    return l;
}
// Lays out `count` equal cards centred, wrapping to new rows; returns the bottom.
- (CGFloat)grid:(NSUInteger)count width:(CGFloat)cw height:(CGFloat)ch top:(CGFloat)top
           make:(P2Card *(^)(NSUInteger i, CGRect frame))make {
    CGFloat w = page.bounds.size.width, gap = kGap * launcher_scale();
    NSUInteger per = MAX(1, (NSUInteger)((w - 48 + gap) / (cw + gap)));
    CGFloat y = top;
    for (NSUInteger row = 0; row * per < count; ++row) {
        NSUInteger n = MIN(per, count - row * per);
        CGFloat x = (w - n * cw - (n - 1) * gap) / 2;
        for (NSUInteger i = 0; i < n; ++i) {
            P2Card *c = make(row * per + i, CGRectMake(x + i * (cw + gap), y, cw, ch));
            [page addSubview:c];
            [cards addObject:c];
        }
        y += ch + gap;
    }
    return y;
}
- (P2Card *)seriesTile:(const rt_picker_series *)s frame:(CGRect)frame {
    CGFloat k = launcher_scale();
    P2Card *t = [[P2Card alloc] initWithFrame:frame];
    NSArray<NSString *> *games = [self gamesOf:s];
    // Art: the series' first game large, the next ones small in front.
    CGFloat big = frame.size.width - 70 * k;
    UIImageView *main = [[UIImageView alloc] initWithImage:cover_of(games[0].UTF8String)];
    main.contentMode = UIViewContentModeScaleAspectFit;
    main.frame = CGRectMake((frame.size.width - big) / 2, 22 * k, big, big);
    main.layer.cornerRadius = 20; main.clipsToBounds = YES;
    [t addSubview:main];
    CGFloat small = 58 * k;
    NSUInteger extra = MIN(games.count - 1, (NSUInteger)3);
    for (NSUInteger i = 0; i < extra; ++i) {
        UIImageView *v = [[UIImageView alloc] initWithImage:cover_of(games[i + 1].UTF8String)];
        v.contentMode = UIViewContentModeScaleAspectFit;
        v.frame = CGRectMake(frame.size.width - 22 * k - small - i * (small * .62), 22 * k + big - small * .7, small, small);
        v.layer.cornerRadius = 12; v.clipsToBounds = YES;
        v.layer.borderWidth = 2; v.layer.borderColor = [UIColor colorWithWhite:.05 alpha:1].CGColor;
        [t addSubview:v];
    }
    UILabel *name = one_line(@(s->title), 24 * k, UIFontWeightBold, 1);
    name.frame = CGRectMake(12, frame.size.height - 74 * k, frame.size.width - 24, 30 * k);
    UILabel *count = one_line(games.count == 1 ? @"1 game" : [NSString stringWithFormat:@"%lu games", (unsigned long)games.count],
                              14 * k, UIFontWeightRegular, .55);
    count.frame = CGRectMake(12, frame.size.height - 42 * k, frame.size.width - 24, 20 * k);
    for (UIView *v in t.subviews) v.userInteractionEnabled = NO;
    [t addSubview:name]; [t addSubview:count];
    name.userInteractionEnabled = count.userInteractionEnabled = NO;
    __weak P2GamePicker *weak = self;
    t.action = ^{ P2GamePicker *me = weak; if (me) { me->focus = 1; [me show:s animated:YES]; } };
    return t;
}
// A series: back, its title, its games as equal cards.
- (CGFloat)buildSeries:(const rt_picker_series *)s {
    CGFloat k = launcher_scale(), y = 20;
    P2Card *back = [[P2Card alloc] initWithFrame:CGRectMake(24, y, 120 * k, 44 * k)];
    back.layer.cornerRadius = 22 * k;
    UILabel *bl = one_line(@"‹  Back", 16 * k, UIFontWeightSemibold, .9);
    bl.frame = back.bounds; bl.userInteractionEnabled = NO;
    [back addSubview:bl];
    __weak P2GamePicker *weak = self;
    back.action = ^{ [weak goBack]; };
    [page addSubview:back];
    [cards addObject:back];
    y += 44 * k + 14;
    [self title:@(s->title) at:y size:34 * k];
    y += 34 * k * 1.3 + 22;
    NSArray<NSString *> *games = [self gamesOf:s];
    return [self grid:games.count width:kCardW * k height:kCardH * k top:y make:^P2Card *(NSUInteger i, CGRect frame) {
        return [self gameCard:games[i] frame:frame];
    }] + 10;
}
- (P2Card *)gameCard:(NSString *)name frame:(CGRect)frame {
    CGFloat k = launcher_scale(), w = frame.size.width, art = kArt * k;
    const rt_ios_game *g = rt_ios_game_named(name.UTF8String);
    BOOL can = playable(g), here = installed(g);
    P2Card *c = [[P2Card alloc] initWithFrame:frame];
    UIView *frameView = [[UIView alloc] initWithFrame:CGRectMake((w - art) / 2, 12 * k, art, art)];
    frameView.backgroundColor = [UIColor colorWithWhite:0 alpha:.35];   // same frame for every cover
    frameView.layer.cornerRadius = 18; frameView.clipsToBounds = YES;
    UIImageView *cover = [[UIImageView alloc] initWithImage:cover_of(g->name)];
    cover.contentMode = UIViewContentModeScaleAspectFit;
    cover.frame = CGRectInset(frameView.bounds, 8 * k, 8 * k);
    cover.alpha = can ? 1 : .35;
    [frameView addSubview:cover];
    const char *badge = rt_picker_badge(g->name);
    if (badge) {   // the episodes and Lost Coast share Half-Life 2's cover: a label tells them apart
        UILabel *b = one_line(@(badge), 17 * k, UIFontWeightHeavy, 1);
        b.backgroundColor = accent(); b.textColor = UIColor.blackColor;
        b.layer.cornerRadius = 10; b.clipsToBounds = YES;
        b.frame = CGRectMake(art - 64 * k, art - 38 * k, 56 * k, 30 * k);
        [frameView addSubview:b];
    }
    [c addSubview:frameView];
    // Short title in the card (the series is on screen): "Episode One" rather than "Half-Life 2: Episode One".
    NSString *title = @(g->title);
    NSRange colon = [title rangeOfString:@": "];
    if (colon.location != NSNotFound && rt_picker_series_of(g->name) == series && rt_picker_badge(g->name))
        title = [title substringFromIndex:colon.location + 2];
    UILabel *t = one_line(title, 18 * k, UIFontWeightSemibold, 1);
    t.frame = CGRectMake(10, 12 * k + art + 12 * k, w - 20, 24 * k);
    UILabel *size = one_line(needs_portal2(g) ? @"Needs Portal 2" : here ? (sizes[name] ?: @"…") : @"Not on this device",
                             13 * k, UIFontWeightRegular, .55);
    size.frame = CGRectMake(10, t.frame.origin.y + 26 * k, w - 20, 18 * k);
    sizeLabels[name] = size;
    UILabel *button = [self pill:can ? @"▶  Play" : @"How to get it" filled:can];
    button.frame = CGRectMake(16, frame.size.height - 52 * k, w - 32, 40 * k);
    for (UIView *v in @[t, size, button]) [c addSubview:v];
    for (UIView *v in c.subviews) v.userInteractionEnabled = NO;
    __weak P2GamePicker *weak = self;
    c.action = ^{ [weak pick:name]; };
    return c;
}
- (void)goBack {
    if (manageScroll && !manageScroll.hidden) { [self toggleManage]; return; }
    if (!series) return;
    const rt_picker_series *from = series;
    focus = 0;
    [self show:NULL animated:YES];
    // Focus the series we came from.
    NSUInteger i = 0;
    for (int s = 0; s < rt_picker_series_count(); ++s) {
        if (![self gamesOf:rt_picker_series_at(s)].count) continue;
        if (rt_picker_series_at(s) == from) break;
        ++i;
    }
    BOOL featured = cards.count && [cards[0].subviews.firstObject isKindOfClass:UIImageView.class] &&
                    cards[0].frame.size.width > kSeriesW * launcher_scale() * 1.5;
    if (GCController.controllers.count) { focus = (NSInteger)i + (featured ? 1 : 0); [self applyFocus]; }
}

// ---- Focus (controllers) ----
- (void)applyFocus {
    for (NSUInteger i = 0; i < cards.count; ++i) cards[i].lit = (NSInteger)i == focus;
    if (focus >= 0 && focus < (NSInteger)cards.count)
        [scroll scrollRectToVisible:CGRectInset(cards[focus].frame, 0, -30) animated:YES];
}
// The nearest card in a direction (dx, dy), from the focused one's centre.
- (void)moveFocus:(CGPoint)d {
    if (!cards.count) return;
    if (focus < 0) { focus = 0; [self applyFocus]; return; }
    CGPoint from = CGPointMake(CGRectGetMidX(cards[focus].frame), CGRectGetMidY(cards[focus].frame));
    NSInteger best = -1;
    CGFloat bestScore = CGFLOAT_MAX;
    for (NSUInteger i = 0; i < cards.count; ++i) {
        if ((NSInteger)i == focus) continue;
        CGPoint to = CGPointMake(CGRectGetMidX(cards[i].frame), CGRectGetMidY(cards[i].frame));
        CGFloat along = (to.x - from.x) * d.x + (to.y - from.y) * d.y;
        if (along <= 1) continue;
        CGFloat across = fabs((to.x - from.x) * d.y - (to.y - from.y) * d.x);
        CGFloat score = along + across * 2;
        if (score < bestScore) { bestScore = score; best = (NSInteger)i; }
    }
    if (best >= 0) { focus = best; [self applyFocus]; }
}
- (void)watchControllers {
    for (GCController *c in GCController.controllers) [self bind:c];
    __weak P2GamePicker *weak = self;
    [NSNotificationCenter.defaultCenter addObserverForName:GCControllerDidConnectNotification object:nil
                                                     queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification *n) {
        P2GamePicker *me = weak;
        if (!me) return;
        [me bind:n.object];
        if (me->focus < 0) { me->focus = 0; [me applyFocus]; }
    }];
}
- (void)bind:(GCController *)c {
    GCExtendedGamepad *pad = c.extendedGamepad;
    if (!pad) return;
    __weak P2GamePicker *weak = self;
    pad.dpad.valueChangedHandler = ^(GCControllerDirectionPad *p, float x, float y) {
        (void)p;
        if (fabsf(x) < .5f && fabsf(y) < .5f) return;
        [weak moveFocus:fabsf(x) > fabsf(y) ? CGPointMake(x > 0 ? 1 : -1, 0) : CGPointMake(0, y > 0 ? -1 : 1)];
    };
    pad.leftThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *p, float x, float y) {
        (void)p;
        P2GamePicker *me = weak;
        if (!me || (fabsf(x) < .6f && fabsf(y) < .6f)) return;
        CFTimeInterval now = CACurrentMediaTime();
        if (now - me->lastStick < kStickRepeat) return;
        me->lastStick = now;
        [me moveFocus:fabsf(x) > fabsf(y) ? CGPointMake(x > 0 ? 1 : -1, 0) : CGPointMake(0, y > 0 ? -1 : 1)];
    };
    pad.buttonA.pressedChangedHandler = ^(GCControllerButtonInput *b, float v, BOOL pressed) {
        (void)b; (void)v;
        P2GamePicker *me = weak;
        if (pressed && me && me->focus >= 0 && me->focus < (NSInteger)me->cards.count) [me->cards[me->focus] fire];
    };
    pad.buttonB.pressedChangedHandler = ^(GCControllerButtonInput *b, float v, BOOL pressed) {
        (void)b; (void)v;
        if (pressed) [weak goBack];
    };
}

// Sizes off the main thread: the launcher shows at once.
- (void)measure {
    for (size_t i = 0; i < rt_game_code_count; ++i) {
        const rt_ios_game *g = [self game:i];
        if (!installed(g) || needs_portal2(g)) continue;
        NSString *name = @(g->name);
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            NSString *s = size_text(data_size(g));
            dispatch_async(dispatch_get_main_queue(), ^{ self->sizes[name] = s; self->sizeLabels[name].text = s; });
        });
    }
}
- (void)say:(NSString *)title message:(NSString *)message {
    UIAlertController *a = [UIAlertController alertControllerWithTitle:title message:message
                                                        preferredStyle:UIAlertControllerStyleAlert];
    [a addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
    [self presentViewController:a animated:YES completion:nil];
}
static NSString *from_mac(const rt_ios_game *g) {
    return [NSString stringWithFormat:@"%s's files come from your Mac, from your own copy of the game:\n\n"
            "1. On the Mac, open Natif Installer.\n2. Plug in this device (or keep it on the same Wi-Fi once paired).\n"
            "3. Under Devices, press Download next to %s.\n\nThen open %@ again.", g->title, g->title,
            [NSBundle.mainBundle objectForInfoDictionaryKey:@"CFBundleDisplayName"] ?: @"the app"];
}
- (void)pick:(NSString *)name {
    const rt_ios_game *g = rt_ios_game_named(name.UTF8String);
    if (needs_portal2(g)) {
        [self say:[NSString stringWithFormat:@"%s needs Portal 2", g->title] message:@RT_PICKER_NEEDS_BASE];
        return;
    }
    if (!installed(g)) {
        [self say:[NSString stringWithFormat:@"How to get %s", g->title] message:from_mac(g)];
        return;
    }
    [NSUserDefaults.standardUserDefaults setBool:remember forKey:kRemember];
    self.chosen = name;
}
// ---- Manage games (an overlay over the launcher) ----
- (void)toggleManage {
    if (!manageScroll) {
        manageScroll = [UIScrollView new];
        manageScroll.backgroundColor = [UIColor colorWithWhite:.04 alpha:.97];
        manage = [UIStackView new];
        manage.axis = UILayoutConstraintAxisVertical; manage.spacing = 10;
        manage.translatesAutoresizingMaskIntoConstraints = NO;
        [manageScroll addSubview:manage];
        [self.view addSubview:manageScroll];
        manageScroll.hidden = YES;
    }
    manageScroll.hidden = !manageScroll.hidden;
    if (manageScroll.hidden) return;
    manageScroll.frame = self.view.bounds;
    [NSLayoutConstraint deactivateConstraints:manage.constraints];
    [NSLayoutConstraint activateConstraints:@[
        [manage.topAnchor constraintEqualToAnchor:manageScroll.contentLayoutGuide.topAnchor constant:40],
        [manage.bottomAnchor constraintEqualToAnchor:manageScroll.contentLayoutGuide.bottomAnchor constant:-40],
        [manage.centerXAnchor constraintEqualToAnchor:manageScroll.frameLayoutGuide.centerXAnchor],
        [manage.widthAnchor constraintEqualToConstant:MIN(600, self.view.bounds.size.width - 40)]]];
    [self fillManage];
}
- (void)fillManage {
    for (UIView *v in manage.arrangedSubviews) [v removeFromSuperview];
    UIStackView *top = [UIStackView new];
    top.spacing = 12; top.alignment = UIStackViewAlignmentCenter;
    [top addArrangedSubview:text(@"Manage games", 26, UIFontWeightHeavy, 1)];
    UIButtonConfiguration *done = [UIButtonConfiguration grayButtonConfiguration];
    done.title = @"Done"; done.baseForegroundColor = UIColor.whiteColor;
    __weak P2GamePicker *weakSelf = self;
    [top addArrangedSubview:[UIButton buttonWithConfiguration:done primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf toggleManage];
    }]]];
    [manage addArrangedSubview:top];
    for (size_t i = 0; i < rt_game_code_count; ++i) {
        const rt_ios_game *g = [self game:i];
        UIStackView *row = [UIStackView new];
        row.axis = UILayoutConstraintAxisVertical; row.spacing = 6;
        row.layoutMargins = UIEdgeInsetsMake(12, 14, 12, 14); row.layoutMarginsRelativeArrangement = YES;
        row.backgroundColor = [UIColor colorWithWhite:1 alpha:.07];
        row.layer.cornerRadius = 12;
        [row addArrangedSubview:text(@(g->title), 17, UIFontWeightSemibold, 1)];
        UILabel *info = text(@"Measuring…", 13, UIFontWeightRegular, .7);
        [row addArrangedSubview:info];
        BOOL here = installed(g);
        NSString *played = last_played(g);
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            long long data = here ? data_size(g) : -1, saves = saves_size(g);
            dispatch_async(dispatch_get_main_queue(), ^{
                info.text = [NSString stringWithFormat:@"Game data: %@ · Saves: %@ · Last played: %@",
                             here ? size_text(data) : @"not on this device", size_text(saves), played];
            });
        });
        UIStackView *buttons = [UIStackView new];
        buttons.spacing = 8;
        NSString *name = @(g->name);
        rt_picker_game pg = picker_game(g);
        if (!rt_picker_can_remove(&pg)) {   // Portal 2, and the games on another's data
            [row addArrangedSubview:text(g->base ? @"It plays from Half-Life 2's files." : @RT_PICKER_BASE_NOTE, 13,
                                         UIFontWeightRegular, .7)];
        } else if (here) {
            UIButtonConfiguration *c = [UIButtonConfiguration grayButtonConfiguration];
            c.title = @"Remove game data"; c.baseForegroundColor = [UIColor colorWithRed:1 green:.45 blue:.4 alpha:1];
            [buttons addArrangedSubview:[UIButton buttonWithConfiguration:c primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
                (void)a; [weakSelf confirmRemove:name];
            }]]];
        }
        UIButtonConfiguration *m = [UIButtonConfiguration grayButtonConfiguration];
        m.title = @"Get it from your Mac"; m.baseForegroundColor = UIColor.whiteColor;
        [buttons addArrangedSubview:[UIButton buttonWithConfiguration:m primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a; [weakSelf say:@"Get it from your Mac" message:from_mac(g)];
        }]]];
        [row addArrangedSubview:buttons];
        [manage addArrangedSubview:row];
    }
}
- (void)confirmRemove:(NSString *)name {
    const rt_ios_game *g = rt_ios_game_named(name.UTF8String);
    long long bytes = data_size(g);
    NSString *message = [NSString stringWithFormat:@"This deletes %s's game files and frees %@. Your saves and settings "
                         "stay, and you can copy the game again from your Mac.", g->title, size_text(bytes)];
    UIAlertController *a = [UIAlertController alertControllerWithTitle:[NSString stringWithFormat:@"Remove %s?", g->title]
                                                               message:message preferredStyle:UIAlertControllerStyleAlert];
    [a addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil]];
    __weak P2GamePicker *weakSelf = self;
    [a addAction:[UIAlertAction actionWithTitle:@"Remove" style:UIAlertActionStyleDestructive handler:^(UIAlertAction *x) {
        (void)x; [weakSelf remove:name];
    }]];
    [self presentViewController:a animated:YES completion:nil];
}
- (void)remove:(NSString *)name {
    const rt_ios_game *g = rt_ios_game_named(name.UTF8String);
    rt_picker_game p = picker_game(g);
    if (!rt_picker_can_remove(&p)) return;   // the base game (rt_picker_removable gives nothing either)
    char rel[256];
    NSError *error = nil;
    BOOL ok = YES;
    for (int i = 0; rt_picker_removable(&p, i, rel, sizeof(rel)); ++i) {
        NSString *path = [documents() stringByAppendingPathComponent:@(rel)];
        if ([NSFileManager.defaultManager fileExistsAtPath:path] && ![NSFileManager.defaultManager removeItemAtPath:path error:&error])
            ok = NO;
    }
    fprintf(stderr, "[ios] jeux : données de %s supprimées%s\n", g->name, ok ? "" : " (en partie)");
    if (!ok) [self say:@"Some files couldn't be removed"
               message:[NSString stringWithFormat:@"%@ Try again, or restart the device first.", error.localizedDescription ?: @""]];
    [sizes removeObjectForKey:name];
    [self show:series animated:NO];   // the game shows as not on this device now
    [self fillManage];
    [self measure];
}
@end

int rt_ios_has_game_choice(void) { return rt_game_code_count > 1; }

void rt_ios_forget_game_choice(void) { [NSUserDefaults.standardUserDefaults setBool:NO forKey:kRemember]; }

const char *rt_ios_choose_game(void) {
    static char chosen[32];
    const char *names[32];
    int here[32], n = 0;
    for (size_t i = 0; i < rt_game_code_count && n < 32; ++i, ++n) {
        names[n] = rt_game_codes[i].name;
        here[n] = playable(rt_ios_game_named(names[n]));   // remembered but needing Portal 2: the picker says why
    }
    NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
    NSString *last = [d stringForKey:kLast];
    const char *pick = rt_picker_autostart(names, here, n, getenv("P2_GAME_CHOICE"), last.UTF8String, [d boolForKey:kRemember]);
    if (!pick) {
        // Picker window over everything; SDL's comes after it.
        P2GamePicker *picker = [P2GamePicker new];
        CGRect frame = UIScreen.mainScreen.bounds;
        int tw = 0, th = 0;   // P2_TV_SIMULATE: the launcher in the stand-in TV's shape (screenshots at 16:9)
        if (rt_tv_simulated(getenv("P2_TV_SIMULATE"), &tw, &th)) frame.size.height = frame.size.width * th / tw;
        UIWindow *window = [[UIWindow alloc] initWithFrame:frame];
        window.rootViewController = picker;
        window.windowLevel = UIWindowLevelAlert;
        [window makeKeyAndVisible];
        // P2_PICKER_TEST=<name> (simulator checks): picks it after a few seconds.
        const char *test = getenv("P2_PICKER_TEST");
        CFTimeInterval auto_pick = test ? CACurrentMediaTime() + 6 : 0;
        while (!picker.chosen) {
            [NSRunLoop.currentRunLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:.05]];
            if (auto_pick && CACurrentMediaTime() > auto_pick) picker.chosen = @(test);
        }
        snprintf(chosen, sizeof(chosen), "%s", picker.chosen.UTF8String);
        window.hidden = YES;
        window.rootViewController = nil;
        pick = chosen;
    }
    snprintf(chosen, sizeof(chosen), "%s", pick);
    [d setObject:@(chosen) forKey:kLast];
    [d setObject:NSDate.date forKey:[@"P2GamePlayed." stringByAppendingString:@(chosen)]];
    fprintf(stderr, "[ios] jeux : %s choisi\n", chosen);
    return chosen;
}
#else
int rt_ios_has_game_choice(void) { return 0; }
void rt_ios_forget_game_choice(void) {}
const char *rt_ios_choose_game(void) { return rt_ios_current_game()->name; }
#endif
#endif
