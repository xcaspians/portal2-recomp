// The Portal app's game picker and "Manage games" screen (see game_picker_ios.h).
//
// Shown by ios/main_ios.c before anything game-specific, in a window of its
// own over SDL's (not created yet): the run loop turns until a game is
// chosen, then the window goes away and the game starts as before. Each game:
// its cover (covers/<name>.png in the app, made by tools/build_ios.py from
// the game's own icon on the Mac), title and size; a game whose files are not
// on the device shows "Not installed". Manage games: data and saves sizes,
// last played, "Remove game data" (game, prep and linux only; saves and
// settings stay) and "Get it from your Mac". Rules in game_picker.c.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_ios.h"
#include "game_picker.h"
#include "game_picker_ios.h"
#include "rt.h"

#ifdef RT_MULTI_GAME
static NSString *const kRemember = @"P2GameRemember", *const kLast = @"P2GameLast";

static NSString *documents(void) {
    return NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
}
static rt_picker_game picker_game(const rt_ios_game *g) {
    return (rt_picker_game){g->name, g->mod, g->data_dir, g->userdata};
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

@interface P2GamePicker : UIViewController
@property (nonatomic, copy) NSString *chosen;
@end

@implementation P2GamePicker {
    UIStackView *cards, *manage;
    UIScrollView *scroll;
    UISwitch *remember;
    NSMutableDictionary<NSString *, UILabel *> *sizeLabels;
}
- (const rt_ios_game *)game:(size_t)i { return rt_ios_game_named(rt_game_codes[i].name); }
- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor colorWithWhite:.05 alpha:1];
    sizeLabels = [NSMutableDictionary dictionary];
    scroll = [UIScrollView new];
    scroll.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:scroll];
    UIStackView *page = [UIStackView new];
    page.axis = UILayoutConstraintAxisVertical; page.spacing = 18; page.alignment = UIStackViewAlignmentCenter;
    page.translatesAutoresizingMaskIntoConstraints = NO;
    [scroll addSubview:page];
    UILayoutGuide *safe = self.view.safeAreaLayoutGuide;
    [NSLayoutConstraint activateConstraints:@[
        [scroll.topAnchor constraintEqualToAnchor:safe.topAnchor], [scroll.bottomAnchor constraintEqualToAnchor:safe.bottomAnchor],
        [scroll.leadingAnchor constraintEqualToAnchor:safe.leadingAnchor], [scroll.trailingAnchor constraintEqualToAnchor:safe.trailingAnchor],
        [page.topAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.topAnchor constant:24],
        [page.bottomAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.bottomAnchor constant:-24],
        [page.centerXAnchor constraintEqualToAnchor:scroll.frameLayoutGuide.centerXAnchor],
        [page.widthAnchor constraintLessThanOrEqualToAnchor:scroll.frameLayoutGuide.widthAnchor constant:-32],
    ]];
    [page addArrangedSubview:text(@"Choose a game", 28, UIFontWeightBold, 1)];
    cards = [UIStackView new];
    cards.axis = UILayoutConstraintAxisHorizontal; cards.spacing = 20; cards.alignment = UIStackViewAlignmentTop;
    [page addArrangedSubview:cards];
    for (size_t i = 0; i < rt_game_code_count; ++i) [cards addArrangedSubview:[self card:[self game:i]]];
    UIStackView *row = [UIStackView new];
    row.spacing = 10; row.alignment = UIStackViewAlignmentCenter;
    remember = [UISwitch new];
    remember.on = [NSUserDefaults.standardUserDefaults boolForKey:kRemember];
    remember.onTintColor = [UIColor colorWithRed:.3 green:.68 blue:1 alpha:1];
    [row addArrangedSubview:remember];
    [row addArrangedSubview:text(@"Remember my choice (Switch game in the settings menu brings this back)", 14,
                                 UIFontWeightRegular, .75)];
    [page addArrangedSubview:row];
    UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
    conf.title = @"Manage games";
    conf.image = [UIImage systemImageNamed:@"externaldrive"];
    conf.imagePadding = 6;
    conf.baseForegroundColor = UIColor.whiteColor;
    __weak P2GamePicker *weakSelf = self;
    [page addArrangedSubview:[UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf toggleManage];
    }]]];
    manage = [UIStackView new];
    manage.axis = UILayoutConstraintAxisVertical; manage.spacing = 10; manage.hidden = YES;
    [page addArrangedSubview:manage];
    [manage.widthAnchor constraintEqualToConstant:560].active = YES;
    [self measure];
}
- (UIView *)card:(const rt_ios_game *)g {
    UIStackView *card = [UIStackView new];
    card.axis = UILayoutConstraintAxisVertical; card.spacing = 8; card.alignment = UIStackViewAlignmentCenter;
    NSString *cover = [NSBundle.mainBundle pathForResource:[@"covers/" stringByAppendingString:@(g->name)] ofType:@"png"];
    UIImageView *image = [[UIImageView alloc] initWithImage:cover ? [UIImage imageWithContentsOfFile:cover]
                                                                  : [UIImage systemImageNamed:@"gamecontroller"]];
    image.contentMode = UIViewContentModeScaleAspectFit;
    image.tintColor = UIColor.whiteColor;
    image.layer.cornerRadius = 22; image.clipsToBounds = YES;
    [image.widthAnchor constraintEqualToConstant:150].active = YES;
    [image.heightAnchor constraintEqualToConstant:150].active = YES;
    [card addArrangedSubview:image];
    [card addArrangedSubview:text(@(g->title), 17, UIFontWeightSemibold, 1)];
    UILabel *size = text(needs_portal2(g) ? @"Needs Portal 2" : installed(g) ? @"…" : @"Not installed", 13,
                         UIFontWeightRegular, .6);
    sizeLabels[@(g->name)] = size;
    [card addArrangedSubview:size];
    NSString *name = @(g->name);
    __weak P2GamePicker *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration filledButtonConfiguration];
    conf.title = playable(g) ? @"Play" : needs_portal2(g) ? @"Needs Portal 2" : @"Not installed";
    conf.cornerStyle = UIButtonConfigurationCornerStyleCapsule;
    if (!playable(g)) conf.baseBackgroundColor = [UIColor colorWithWhite:.3 alpha:1];
    [card addArrangedSubview:[UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf pick:name];
    }]]];
    UIControl *tap = [UIControl new];   // the cover is a button too
    [tap addAction:[UIAction actionWithHandler:^(UIAction *a) { (void)a; [weakSelf pick:name]; }]
        forControlEvents:UIControlEventTouchUpInside];
    tap.translatesAutoresizingMaskIntoConstraints = NO;
    [card addSubview:tap];
    [NSLayoutConstraint activateConstraints:@[[tap.topAnchor constraintEqualToAnchor:image.topAnchor],
        [tap.bottomAnchor constraintEqualToAnchor:image.bottomAnchor], [tap.leadingAnchor constraintEqualToAnchor:image.leadingAnchor],
        [tap.trailingAnchor constraintEqualToAnchor:image.trailingAnchor]]];
    return card;
}
// Sizes off the main thread: the picker shows at once.
- (void)measure {
    for (size_t i = 0; i < rt_game_code_count; ++i) {
        const rt_ios_game *g = [self game:i];
        if (!installed(g) || needs_portal2(g)) continue;
        NSString *name = @(g->name);
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
            NSString *s = size_text(data_size(g));
            dispatch_async(dispatch_get_main_queue(), ^{ self->sizeLabels[name].text = s; });
        });
    }
}
- (void)say:(NSString *)title message:(NSString *)message {
    UIAlertController *a = [UIAlertController alertControllerWithTitle:title message:message
                                                        preferredStyle:UIAlertControllerStyleAlert];
    [a addAction:[UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:nil]];
    [self presentViewController:a animated:YES completion:nil];
}
static NSString *const kFromMac = @"The game's files come from your Mac: open the Natif Installer, plug in this device "
                                   "(or keep it on the same Wi-Fi once paired), and press Download next to the game.";
- (void)pick:(NSString *)name {
    const rt_ios_game *g = rt_ios_game_named(name.UTF8String);
    if (needs_portal2(g)) {
        [self say:[NSString stringWithFormat:@"%s needs Portal 2", g->title] message:@RT_PICKER_NEEDS_BASE];
        return;
    }
    if (!installed(g)) {
        [self say:[NSString stringWithFormat:@"%s isn't installed", g->title] message:kFromMac];
        return;
    }
    [NSUserDefaults.standardUserDefaults setBool:remember.on forKey:kRemember];
    self.chosen = name;
}
// ---- Manage games ----
- (void)toggleManage {
    manage.hidden = !manage.hidden;
    if (!manage.hidden) [self fillManage];
}
- (void)fillManage {
    for (UIView *v in manage.arrangedSubviews) [v removeFromSuperview];
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
                             here ? size_text(data) : @"not installed", size_text(saves), played];
            });
        });
        UIStackView *buttons = [UIStackView new];
        buttons.spacing = 8;
        __weak P2GamePicker *weakSelf = self;
        NSString *name = @(g->name);
        rt_picker_game pg = picker_game(g);
        if (!rt_picker_can_remove(&pg)) {   // Portal 2: one line in place of Remove
            [row addArrangedSubview:text(@RT_PICKER_BASE_NOTE, 13, UIFontWeightRegular, .7)];
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
            (void)a; [weakSelf say:@"Get it from your Mac" message:kFromMac];
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
    // The picker shows the game as "Not installed" now.
    [cards.arrangedSubviews makeObjectsPerformSelector:@selector(removeFromSuperview)];
    for (size_t i = 0; i < rt_game_code_count; ++i) [cards addArrangedSubview:[self card:[self game:i]]];
    [self fillManage];
    [self measure];
}
@end

int rt_ios_has_game_choice(void) { return rt_game_code_count > 1; }

void rt_ios_forget_game_choice(void) { [NSUserDefaults.standardUserDefaults setBool:NO forKey:kRemember]; }

const char *rt_ios_choose_game(void) {
    static char chosen[32];
    const char *names[8];
    int here[8], n = 0;
    for (size_t i = 0; i < rt_game_code_count && n < 8; ++i, ++n) {
        names[n] = rt_game_codes[i].name;
        here[n] = playable(rt_ios_game_named(names[n]));   // remembered but needing Portal 2: the picker says why
    }
    NSUserDefaults *d = NSUserDefaults.standardUserDefaults;
    NSString *last = [d stringForKey:kLast];
    const char *pick = rt_picker_autostart(names, here, n, getenv("P2_GAME_CHOICE"), last.UTF8String, [d boolForKey:kRemember]);
    if (!pick) {
        // Picker window over everything; SDL's comes after it.
        P2GamePicker *picker = [P2GamePicker new];
        UIWindow *window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
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
