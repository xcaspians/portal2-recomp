// In-game settings menu (see settings_menu_ios.h).
//
// The gear shows only while the game's pause menu is up (game_state.c: game
// UI visible and a level loaded), never in gameplay or the main menu. The
// panel is a view inside the touch overlay, so the overlay's "stay in front
// of SDL's views" also keeps it on top. Items come from settings_menu.c and
// are stored through live_settings_ios.m (the app's preferences, shared with
// the Settings app), so every change applies at once.
//
// Controller: the options button (View/Select/−) opens or closes it, d-pad or
// left stick moves and changes values, A changes a switch or choice or presses
// a button, B closes. While it is open the game gets no controller press
// (sdl_ios.m) and the touch buttons are drawn behind it as a preview.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <GameController/GameController.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device_defaults_ios.h"
#include "game_ios.h"
#include "game_picker_ios.h"
#include "backtap_calibration_ios.h"
#include "backtap_ios.h"
#include "icloud_saves.h"
#include "live_settings.h"
#include "mods_list.h"
#include "rt.h"
const char *rt_user_root(void);   // runtime/paths.c
#include "settings_menu.h"
#include "settings_menu_ios.h"
#include "workshop_ios.h"

extern _Atomic int rt_ios_metal_active;   // metal_ios.m

static int menu_open, closing_hold;
int rt_ios_settings_menu_captures_pad(void) { return menu_open || closing_hold; }
int rt_ios_settings_menu_open(void) { return menu_open; }

typedef enum { ROW_SETTING, ROW_RESET, ROW_CLOSE, ROW_MAP, ROW_COMMUNITY, ROW_RECOMMENDED, ROW_SWITCH_GAME, ROW_CALIBRATE } RowKind;

@interface P2MenuRow : NSObject
@property (nonatomic) RowKind kind;
@property (nonatomic) const rt_menu_item *item;
@property (nonatomic, strong) UIView *view;
@property (nonatomic, strong) UILabel *value;
@property (nonatomic, strong) UISlider *slider;
@property (nonatomic, strong) UISwitch *toggle;
@property (nonatomic, strong) UISegmentedControl *segments;
@property (nonatomic, strong) UIButton *button;
@property (nonatomic, copy) NSString *map;   // ROW_MAP: console name (mods_list.c)
@end
@implementation P2MenuRow
@end

// One axis of the d-pad or stick: a step at once, then repeats while held.
static int axis_step(int dir, int *held, CFTimeInterval *next, CFTimeInterval now) {
    if (!dir) { *held = 0; return 0; }
    if (dir != *held) { *held = dir; *next = now + .4; return dir; }
    if (now >= *next) { *next = now + .09; return dir; }
    return 0;
}

@interface P2SettingsMenu : UIView
@end

@implementation P2SettingsMenu {
    UIButton *gear;
    UIView *dim, *panel, *header;
    UIScrollView *scroll;
    UIStackView *stack;
    NSMutableArray<P2MenuRow *> *rows;
    NSInteger focus;          // focused row (controller), -1 : none
    BOOL prevA, prevB, prevOptions;
    int held[2];
    CFTimeInterval repeatAt[2];
    CFTimeInterval resetArmedUntil, switchArmedUntil;
    NSString *padName;        // localized name of the options button, for the gear
    UIView *community;        // Community Maps browser (workshop_ios.m), over the panel while open
    UIView *calibration;      // "Calibrate back taps" (backtap_calibration_ios.m), over the panel while open
    UILabel *icloudStatus;    // "Last synced ..." under the iCloud switch
    CFTimeInterval icloudShownAt;
}

- (instancetype)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        self.backgroundColor = UIColor.clearColor;
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        focus = -1;
        [self makeGear];
        // A setting changed elsewhere (the Settings app, read again on return):
        // the open panel shows it.
        __weak P2SettingsMenu *weakSelf = self;
        [NSNotificationCenter.defaultCenter addObserverForName:@"P2SettingsChanged" object:nil
                                                         queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification *n) {
            (void)n;
            P2SettingsMenu *menu = weakSelf;
            if (menu) [menu refreshRows];
        }];
    }
    return self;
}

// ---- gear (pause menu only) ----
- (void)makeGear {
    __weak P2SettingsMenu *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration filledButtonConfiguration];
    conf.image = [UIImage systemImageNamed:@"gearshape.fill"];
    conf.imagePadding = 6;
    conf.title = @"Touch Controls";
    conf.baseBackgroundColor = [UIColor colorWithWhite:0 alpha:.62];
    conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleCapsule;
    conf.contentInsets = NSDirectionalEdgeInsetsMake(9, 14, 9, 16);
    gear = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *action) {
        (void)action; [weakSelf openMenu];
    }]];
    gear.hidden = YES;
    [self addSubview:gear];
}
- (void)updateGearHint {
    GCController *pad = [self pad];
    NSString *name = pad.extendedGamepad.buttonOptions.localizedName;
    if ((name || padName) && ![name isEqualToString:padName]) {
        padName = name;
        UIButtonConfiguration *conf = gear.configuration;
        conf.subtitle = name ? [NSString stringWithFormat:@"or %@ on the controller", name] : nil;
        gear.configuration = conf;
        [self setNeedsLayout];
    }
}
- (void)layoutSubviews {
    [super layoutSubviews];
    CGRect safe = UIEdgeInsetsInsetRect(self.bounds, self.safeAreaInsets);
    CGSize g = [gear sizeThatFits:CGSizeMake(320, 80)];
    gear.frame = CGRectMake(CGRectGetMaxX(safe) - g.width - 12, CGRectGetMinY(safe) + 12, g.width, g.height);
    if (!panel) return;
    dim.frame = self.bounds;
    CGFloat w = MIN(600, safe.size.width - 24), h = safe.size.height - 24;
    panel.frame = CGRectMake(CGRectGetMidX(safe) - w / 2, CGRectGetMidY(safe) - h / 2, w, h);
    header.frame = CGRectMake(0, 0, w, 56);
    scroll.frame = CGRectMake(0, 56, w, h - 56);
}
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    if (menu_open) return [super hitTest:point withEvent:event];
    if (!gear.hidden && CGRectContainsPoint(gear.frame, point))
        return [gear hitTest:[self convertPoint:point toView:gear] withEvent:event];
    return nil;   // everything else goes to the game
}

// ---- panel ----
static UILabel *label(NSString *text, CGFloat size, UIFontWeight weight, CGFloat alpha) {
    UILabel *l = [UILabel new];
    l.text = text;
    l.font = [UIFont monospacedDigitSystemFontOfSize:size weight:weight];
    l.textColor = [UIColor colorWithWhite:1 alpha:alpha];
    l.translatesAutoresizingMaskIntoConstraints = NO;
    return l;
}
- (void)openMenu {
    if (menu_open) return;
    menu_open = 1;
    gear.hidden = YES;
    dim = [UIView new];
    dim.backgroundColor = [UIColor colorWithWhite:0 alpha:.35];
    [self addSubview:dim];
    panel = [UIView new];
    panel.layer.cornerRadius = 20; panel.clipsToBounds = YES;
    panel.layer.borderWidth = 1; panel.layer.borderColor = [UIColor colorWithWhite:1 alpha:.12].CGColor;
    UIVisualEffectView *blur = [[UIVisualEffectView alloc] initWithEffect:
                                [UIBlurEffect effectWithStyle:UIBlurEffectStyleSystemUltraThinMaterialDark]];
    blur.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [panel addSubview:blur];
    UIView *tint = [UIView new];
    tint.backgroundColor = [UIColor colorWithWhite:0 alpha:.5];
    tint.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [panel addSubview:tint];
    [self addSubview:panel];
    rows = [NSMutableArray array];

    header = [UIView new];
    [panel addSubview:header];
    UILabel *title = label(@"Touch Controls", 21, UIFontWeightBold, 1);
    [header addSubview:title];
    P2MenuRow *close = [P2MenuRow new];
    close.kind = ROW_CLOSE;
    __weak P2SettingsMenu *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration plainButtonConfiguration];
    conf.image = [UIImage systemImageNamed:@"xmark.circle.fill"
                         withConfiguration:[UIImageSymbolConfiguration configurationWithPointSize:24]];
    conf.baseForegroundColor = [UIColor colorWithWhite:1 alpha:.8];
    close.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf closeMenu];
    }]];
    close.button.translatesAutoresizingMaskIntoConstraints = NO;
    close.view = close.button;
    close.view.layer.cornerRadius = 10;
    [header addSubview:close.button];
    [NSLayoutConstraint activateConstraints:@[
        [title.leadingAnchor constraintEqualToAnchor:header.leadingAnchor constant:20],
        [title.centerYAnchor constraintEqualToAnchor:header.centerYAnchor],
        [close.button.trailingAnchor constraintEqualToAnchor:header.trailingAnchor constant:-10],
        [close.button.centerYAnchor constraintEqualToAnchor:header.centerYAnchor],
        [close.button.widthAnchor constraintEqualToConstant:44], [close.button.heightAnchor constraintEqualToConstant:44],
    ]];

    scroll = [UIScrollView new];
    scroll.indicatorStyle = UIScrollViewIndicatorStyleWhite;
    [panel addSubview:scroll];
    stack = [UIStackView new];
    stack.axis = UILayoutConstraintAxisVertical;
    stack.spacing = 4;
    stack.layoutMargins = UIEdgeInsetsMake(0, 12, 16, 12);
    stack.layoutMarginsRelativeArrangement = YES;
    stack.translatesAutoresizingMaskIntoConstraints = NO;
    [scroll addSubview:stack];
    [NSLayoutConstraint activateConstraints:@[
        [stack.topAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.topAnchor],
        [stack.bottomAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.bottomAnchor],
        [stack.leadingAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.leadingAnchor],
        [stack.trailingAnchor constraintEqualToAnchor:scroll.contentLayoutGuide.trailingAnchor],
        [stack.widthAnchor constraintEqualToAnchor:scroll.frameLayoutGuide.widthAnchor],
    ]];

    // Portal 2 only: iCloud saves, Maps & Mods and Community Maps (its maps
    // folders and Workshop app). Portal Stories: Mel has the rest.
    BOOL portal2 = !strcmp(rt_ios_current_game()->name, "portal2");
    for (int i = 0; i < rt_menu_item_count; ++i) {
        const rt_menu_item *it = &rt_menu_items[i];
        if (!portal2 && !strcmp(it->key, "P2ICloudSaves")) continue;
        if (it->section) [self addSection:@(it->section)];
        [self addSettingRow:it];
        if (!strcmp(it->key, "P2MaxFPS")) [self addRecommendedRow];
        if (!strcmp(it->key, "P2BackTapSensitivity") && rt_ios_current_game()->back_taps) [self addCalibrateRow];
    }
    if (portal2) [self addMapsSection];
    [self addSection:@""];
    [self addResetRow];
    if (rt_ios_has_game_choice()) [self addSwitchGameRow];
    [rows addObject:close];   // last in controller order, first on screen
    NSString *version = [NSBundle.mainBundle objectForInfoDictionaryKey:@"P2ForkVersion"] ?: @"unknown";
    UILabel *footer = label([NSString stringWithFormat:@"%@ · recomp fork · version %@", @(rt_ios_current_game()->title),
                                                       version], 13,
                            UIFontWeightRegular, .5);
    footer.textAlignment = NSTextAlignmentCenter;
    [stack addArrangedSubview:footer];
    [footer.heightAnchor constraintEqualToConstant:36].active = YES;

    focus = [self pad] ? 0 : -1;
    [self showFocus];
    [self setNeedsLayout];
    [self layoutIfNeeded];
}
- (void)showICloudStatus {
    char text[256];
    rt_ios_icloud_status(text, sizeof(text));
    icloudStatus.text = @(text);
    icloudShownAt = CACurrentMediaTime();
}
// ---- Community Maps ----
static void community_closed(void *ctx, int all) { [(__bridge P2SettingsMenu *)ctx communityClosed:all]; }
- (void)openCommunity {
    if (community || !panel) return;
    community = (__bridge_transfer UIView *)rt_ios_community_create(community_closed, (__bridge void *)self);
    community.frame = panel.bounds;
    [panel addSubview:community];
}
- (void)communityClosed:(int)all {
    [community removeFromSuperview];
    community = nil;
    [self closeMenu];
    if (!all) [self openMenu];   // back to the settings, Maps & Mods read again (new downloads)
}
- (void)closeMenu {
    if (!menu_open) return;
    menu_open = 0;
    closing_hold = [self padActive];   // the B that closed it stays away from the game
    [dim removeFromSuperview]; [panel removeFromSuperview];
    [calibration removeFromSuperview];
    dim = panel = header = nil; scroll = nil; stack = nil; rows = nil; icloudStatus = nil; community = nil; calibration = nil;
    focus = -1;
    resetArmedUntil = 0;
}

- (void)addSection:(NSString *)name {
    UILabel *l = label(name.uppercaseString, 12, UIFontWeightSemibold, .55);
    UIView *box = [UIView new];
    [box addSubview:l];
    [NSLayoutConstraint activateConstraints:@[
        [l.leadingAnchor constraintEqualToAnchor:box.leadingAnchor constant:14],
        [l.bottomAnchor constraintEqualToAnchor:box.bottomAnchor constant:-4],
        [box.heightAnchor constraintEqualToConstant:name.length ? 34 : 10],
    ]];
    [stack addArrangedSubview:box];
}
- (void)addSettingRow:(const rt_menu_item *)it {
    P2MenuRow *row = [P2MenuRow new];
    row.kind = ROW_SETTING;
    row.item = it;
    UIView *v = [UIView new];
    v.layer.cornerRadius = 12;
    row.view = v;
    UILabel *title = label(@(it->title), 16, UIFontWeightMedium, .95);
    [v addSubview:title];
    row.value = label(@"", 15, UIFontWeightRegular, .7);
    row.value.textAlignment = NSTextAlignmentRight;
    [v addSubview:row.value];
    NSMutableArray<NSLayoutConstraint *> *c = [NSMutableArray arrayWithArray:@[
        [title.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:14],
        [title.topAnchor constraintEqualToAnchor:v.topAnchor constant:12],
        [row.value.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:-14],
        [row.value.centerYAnchor constraintEqualToAnchor:title.centerYAnchor],
    ]];
    __weak P2SettingsMenu *weakSelf = self;
    __weak P2MenuRow *weakRow = row;
    UIView *below = title;
    switch (it->kind) {
    case RT_MENU_SLIDER: {
        UISlider *s = [UISlider new];
        s.minimumValue = (float)it->min; s.maximumValue = (float)it->max;
        s.minimumTrackTintColor = [UIColor colorWithRed:.3 green:.68 blue:1 alpha:1];
        s.translatesAutoresizingMaskIntoConstraints = NO;
        [s addAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a;
            char value[32];
            snprintf(value, sizeof(value), "%g", weakRow.slider.value);
            [weakSelf store:weakRow value:value];
        }] forControlEvents:UIControlEventValueChanged];
        [v addSubview:s];
        row.slider = s;
        [c addObjectsFromArray:@[
            [s.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:14],
            [s.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:-14],
            [s.topAnchor constraintEqualToAnchor:title.bottomAnchor constant:8],
        ]];
        below = s;
        break;
    }
    case RT_MENU_TOGGLE: {
        UISwitch *t = [UISwitch new];
        t.onTintColor = [UIColor colorWithRed:.3 green:.68 blue:1 alpha:1];
        t.translatesAutoresizingMaskIntoConstraints = NO;
        [t addAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a; [weakSelf store:weakRow value:weakRow.toggle.on ? "1" : "0"];
        }] forControlEvents:UIControlEventValueChanged];
        [v addSubview:t];
        row.toggle = t;
        row.value.hidden = YES;
        [c addObjectsFromArray:@[
            [t.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:-14],
            [t.centerYAnchor constraintEqualToAnchor:title.centerYAnchor],
        ]];
        break;
    }
    case RT_MENU_CHOICE: {
        NSMutableArray<NSString *> *labels = [NSMutableArray array];
        for (int i = 0; it->labels[i]; ++i) [labels addObject:@(it->labels[i])];
        UISegmentedControl *seg = [[UISegmentedControl alloc] initWithItems:labels];
        seg.selectedSegmentTintColor = [UIColor colorWithRed:.3 green:.68 blue:1 alpha:1];
        [seg setTitleTextAttributes:@{NSForegroundColorAttributeName: UIColor.whiteColor} forState:UIControlStateNormal];
        seg.translatesAutoresizingMaskIntoConstraints = NO;
        [seg addAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a;
            NSInteger i = weakRow.segments.selectedSegmentIndex;
            if (i >= 0) [weakSelf store:weakRow value:weakRow.item->values[i]];
        }] forControlEvents:UIControlEventValueChanged];
        [v addSubview:seg];
        row.segments = seg;
        [c addObjectsFromArray:@[
            [seg.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:14],
            [seg.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:-14],
            [seg.topAnchor constraintEqualToAnchor:title.bottomAnchor constant:8],
        ]];
        below = seg;
        break;
    }
    }
    // iCloud saves: when they last synced, or why they can't.
    if (!strcmp(it->key, "P2ICloudSaves")) {
        icloudStatus = label(@"", 13, UIFontWeightRegular, .6);
        icloudStatus.numberOfLines = 0;
        [v addSubview:icloudStatus];
        [c addObjectsFromArray:@[
            [icloudStatus.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:14],
            [icloudStatus.trailingAnchor constraintEqualToAnchor:v.trailingAnchor constant:-14],
            [icloudStatus.topAnchor constraintEqualToAnchor:below.bottomAnchor constant:6],
        ]];
        below = icloudStatus;
        [self showICloudStatus];
    }
    // Metal reads its frame rate cap once, at start.
    if ((it->flags & RT_MENU_RELAUNCH_ON_METAL) && rt_ios_metal_active) {
        UILabel *note = label(@"Applies on next launch (Metal renderer)", 13, UIFontWeightRegular, 1);
        note.textColor = [UIColor colorWithRed:1 green:.78 blue:.35 alpha:1];
        [v addSubview:note];
        [c addObjectsFromArray:@[
            [note.leadingAnchor constraintEqualToAnchor:v.leadingAnchor constant:14],
            [note.topAnchor constraintEqualToAnchor:below.bottomAnchor constant:6],
        ]];
        below = note;
    }
    [c addObject:[v.bottomAnchor constraintEqualToAnchor:below.bottomAnchor constant:12]];
    [NSLayoutConstraint activateConstraints:c];
    [stack addArrangedSubview:v];
    [rows addObject:row];
    [self refresh:row];
}
// "Defaults for <device>": what device_defaults.txt picked here (resolution,
// MetalFX, frame rate), and a button that applies it again.
- (void)addRecommendedRow {
    const rt_dd_choice *c = rt_ios_device_choice();
    if (!c) return;
    static const char *const scale_names[][2] = {{"native", "Native"}, {"2", "×2"}, {"1.5", "×1.5"}, {"1", "×1"}};
    const char *scale = c->scale;
    for (size_t i = 0; i < sizeof(scale_names) / sizeof(*scale_names); ++i)
        if (!strcmp(scale_names[i][0], c->scale)) scale = scale_names[i][1];
    const char *upscale = !strcmp(c->upscale, "0.75") ? "MetalFX Quality" : !strcmp(c->upscale, "0.5") ? "MetalFX Performance"
                                                                                                         : "no MetalFX";
    // %@, not %s: %s reads C strings as Mac Roman, and "×" came out as "√ó".
    UILabel *info = label([NSString stringWithFormat:@"Defaults for %@: resolution %@, %@, %@ fps%@", @(c->name), @(scale),
                           @(upscale), @(c->fps), c->low_memory ? @", memory saving on" : @""], 13, UIFontWeightRegular, .65);
    info.numberOfLines = 0;
    [stack addArrangedSubview:info];
    P2MenuRow *row = [P2MenuRow new];
    row.kind = ROW_RECOMMENDED;
    __weak P2SettingsMenu *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
    conf.title = @"Use recommended";
    conf.subtitle = @"Resolution and MetalFX apply on next launch";
    conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    row.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf useRecommended];
    }]];
    row.view = row.button;
    row.view.layer.cornerRadius = 12;
    [row.button.heightAnchor constraintGreaterThanOrEqualToConstant:52].active = YES;
    [stack addArrangedSubview:row.button];
    [rows addObject:row];
}
- (void)useRecommended {
    rt_ios_device_defaults_use_recommended();
    for (P2MenuRow *r in rows) [self refresh:r];
    for (P2MenuRow *r in rows) if (r.kind == ROW_RECOMMENDED) {
        UIButtonConfiguration *conf = r.button.configuration;
        conf.title = @"Recommended settings applied";
        r.button.configuration = conf;
    }
}
// "Switch game" (the Portal app, several games): the game runs in this
// process, so switching means a new launch. First press: what will happen;
// second press: the remembered choice is forgotten and the app closes, and
// the picker shows when it is opened again.
// "Calibrate back taps" (backtap_calibration_ios.m): its screen over the panel.
- (void)addCalibrateRow {
    P2MenuRow *row = [P2MenuRow new];
    row.kind = ROW_CALIBRATE;
    __weak P2SettingsMenu *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
    char summary[96];
    conf.title = @"Calibrate back taps";
    conf.subtitle = @(rt_ios_backtap_calibration_summary(summary, sizeof(summary)));
    conf.image = [UIImage systemImageNamed:@"hand.tap"];
    conf.imagePadding = 8;
    conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    conf.contentInsets = NSDirectionalEdgeInsetsMake(10, 14, 10, 14);
    row.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf openCalibration];
    }]];
    row.button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeading;
    row.view = row.button;
    row.view.layer.cornerRadius = 12;
    [stack addArrangedSubview:row.button];
    [rows addObject:row];
}
static void calibration_closed(void *ctx) { [(__bridge P2SettingsMenu *)ctx calibrationClosed]; }
- (void)openCalibration {
    if (calibration || !panel || !rt_ios_backtap_available()) return;
    calibration = (__bridge_transfer UIView *)rt_ios_backtap_calibration_create(calibration_closed, (__bridge void *)self);
    calibration.frame = panel.bounds;
    [panel addSubview:calibration];
}
- (void)calibrationClosed {
    [calibration removeFromSuperview];
    calibration = nil;
    [self closeMenu];
    [self openMenu];   // the row shows the new "Calibrated on"
}
- (void)addSwitchGameRow {
    P2MenuRow *row = [P2MenuRow new];
    row.kind = ROW_SWITCH_GAME;
    __weak P2SettingsMenu *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
    conf.title = @"Switch game";
    conf.image = [UIImage systemImageNamed:@"square.grid.2x2"];
    conf.imagePadding = 6;
    conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    row.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf switchGamePressed:nil];
    }]];
    row.view = row.button;
    row.view.layer.cornerRadius = 12;
    [row.button.heightAnchor constraintGreaterThanOrEqualToConstant:48].active = YES;
    [stack addArrangedSubview:row.button];
    [rows addObject:row];
}
- (void)switchGamePressed:(id)unused {
    (void)unused;
    CFTimeInterval now = CACurrentMediaTime();
    P2MenuRow *row = nil;
    for (P2MenuRow *r in rows) if (r.kind == ROW_SWITCH_GAME) row = r;
    if (!row) return;
    if (now > switchArmedUntil) {
        switchArmedUntil = now + 4;
        UIButtonConfiguration *conf = row.button.configuration;
        conf.title = @"Press again to close the app";
        conf.subtitle = @"Save first. Open the app again to pick a game.";
        conf.baseBackgroundColor = [UIColor colorWithRed:.55 green:.35 blue:.1 alpha:1];
        row.button.configuration = conf;
        return;
    }
    rt_ios_forget_game_choice();
    [NSUserDefaults.standardUserDefaults synchronize];
    fprintf(stderr, "[ios] jeux : changer de jeu, l'app se ferme\n");
    exit(0);
}
- (void)addResetRow {
    P2MenuRow *row = [P2MenuRow new];
    row.kind = ROW_RESET;
    __weak P2SettingsMenu *weakSelf = self;
    UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
    conf.title = @"Reset to defaults";
    conf.baseForegroundColor = UIColor.whiteColor;
    conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
    row.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
        (void)a; [weakSelf resetPressed];
    }]];
    row.view = row.button;
    row.view.layer.cornerRadius = 12;
    [row.button.heightAnchor constraintEqualToConstant:48].active = YES;
    [stack addArrangedSubview:row.button];
    [rows addObject:row];
}

// ---- Maps & Mods (docs/mods.md) ----
// Custom maps under the game folder: portal2/maps/custom and workshop (any
// depth), and maps/ of the portal2_dlcN addons; rt_mods_map_name keeps those
// the console can start.
- (NSArray<NSString *> *)customMaps:(NSMutableArray<NSString *> *)addons {
    const char *root = rt_paths_game_root();
    if (!root || !*root) return @[];
    NSString *game = @(root);
    NSFileManager *files = NSFileManager.defaultManager;
    NSMutableOrderedSet<NSString *> *found = [NSMutableOrderedSet orderedSet];
    // Maps can also be in the user folder, which the engine reads first:
    // Community Maps downloads go there (workshop_ios.m).
    NSMutableArray<NSString *> *roots = [NSMutableArray arrayWithObject:game];
    const char *user = rt_user_root();
    if (user && *user && strcmp(user, root)) [roots addObject:@(user)];
    for (NSString *base in roots)
        for (NSString *sub in @[@"portal2/maps/custom", @"portal2/maps/workshop"])
            for (NSString *relative in [files enumeratorAtPath:[base stringByAppendingPathComponent:sub]])
                [found addObject:[sub stringByAppendingPathComponent:relative]];
    for (NSString *folder in [files contentsOfDirectoryAtPath:game error:nil]) {
        if (!rt_mods_addon_folder(folder.UTF8String)) continue;
        [addons addObject:folder];
        NSString *maps = [folder stringByAppendingPathComponent:@"maps"];
        for (NSString *name in [files contentsOfDirectoryAtPath:[game stringByAppendingPathComponent:maps] error:nil])
            [found addObject:[maps stringByAppendingPathComponent:name]];
    }
    NSMutableArray<NSString *> *maps = [NSMutableArray array];
    char name[256];
    for (NSString *relative in found.array)
        if (rt_mods_map_name(relative.UTF8String, name, sizeof(name))) [maps addObject:relative];
    [addons sortUsingSelector:@selector(localizedStandardCompare:)];
    return [maps sortedArrayUsingSelector:@selector(localizedStandardCompare:)];
}
- (void)addNote:(NSString *)text {
    UILabel *l = label(text, 14, UIFontWeightRegular, .6);
    l.numberOfLines = 0;
    UIView *box = [UIView new];
    [box addSubview:l];
    [NSLayoutConstraint activateConstraints:@[
        [l.leadingAnchor constraintEqualToAnchor:box.leadingAnchor constant:14],
        [l.trailingAnchor constraintEqualToAnchor:box.trailingAnchor constant:-14],
        [l.topAnchor constraintEqualToAnchor:box.topAnchor constant:4],
        [l.bottomAnchor constraintEqualToAnchor:box.bottomAnchor constant:-6],
    ]];
    [stack addArrangedSubview:box];
}
- (void)addMapsSection {
    [self addSection:@"Maps & Mods"];
    {
        P2MenuRow *row = [P2MenuRow new];
        row.kind = ROW_COMMUNITY;
        __weak P2SettingsMenu *weakMenu = self;
        UIButtonConfiguration *conf = [UIButtonConfiguration filledButtonConfiguration];
        conf.title = @"Community Maps";
        conf.subtitle = @"Browse and download Workshop chambers";
        conf.image = [UIImage systemImageNamed:@"globe"];
        conf.imagePadding = 10;
        conf.baseBackgroundColor = [UIColor colorWithRed:.2 green:.45 blue:.8 alpha:.85];
        conf.baseForegroundColor = UIColor.whiteColor;
        conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
        row.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a; [weakMenu openCommunity];
        }]];
        row.button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeading;
        row.view = row.button;
        row.view.layer.cornerRadius = 12;
        [row.button.heightAnchor constraintGreaterThanOrEqualToConstant:56].active = YES;
        [stack addArrangedSubview:row.button];
        [rows addObject:row];
    }
    NSMutableArray<NSString *> *addons = [NSMutableArray array];
    NSArray<NSString *> *maps = [self customMaps:addons];
    BOOL can_start = rt_ios_game_commands_available() != 0;
    __weak P2SettingsMenu *weakSelf = self;
    for (NSString *relative in maps) {
        char name[256], origin[64];
        rt_mods_map_name(relative.UTF8String, name, sizeof(name));
        rt_mods_map_origin(relative.UTF8String, origin, sizeof(origin));
        P2MenuRow *row = [P2MenuRow new];
        row.kind = ROW_MAP;
        row.map = @(name);
        __weak P2MenuRow *weakRow = row;
        UIButtonConfiguration *conf = [UIButtonConfiguration grayButtonConfiguration];
        conf.title = relative.lastPathComponent.stringByDeletingPathExtension;
        conf.subtitle = @(origin);
        conf.image = [UIImage systemImageNamed:@"map.fill"];
        conf.imagePadding = 10;
        conf.baseForegroundColor = UIColor.whiteColor;
        conf.cornerStyle = UIButtonConfigurationCornerStyleLarge;
        row.button = [UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a; [weakSelf startMap:weakRow];
        }]];
        row.button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeading;
        row.button.enabled = can_start;
        row.view = row.button;
        row.view.layer.cornerRadius = 12;
        [row.button.heightAnchor constraintGreaterThanOrEqualToConstant:52].active = YES;
        [stack addArrangedSubview:row.button];
        [rows addObject:row];
    }
    if (!maps.count)
        [self addNote:@"No custom maps yet. Put .bsp files in Files > Portal 2 > game > portal2 > maps > custom, "
                       "then open this menu again."];
    else if (!can_start)
        [self addNote:@"Maps can't be started from here in this game: use launch.args (docs/mods.md)."];
    if (addons.count)
        [self addNote:[NSString stringWithFormat:@"Addon folders, loaded at start: %@",
                       [addons componentsJoinedByString:@", "]]];
}
- (void)startMap:(P2MenuRow *)row {
    if (!row.map || !rt_ios_game_commands_available()) return;
    NSString *command = [@"map " stringByAppendingString:row.map];
    if (rt_ios_game_command(command.UTF8String)) [self closeMenu];   // the engine loads it next frame
}

// ---- values ----
- (void)refreshRows { for (P2MenuRow *r in rows) if (r.kind == ROW_SETTING) [self refresh:r]; }
- (void)refresh:(P2MenuRow *)row {
    if (row.kind != ROW_SETTING) return;
    const rt_menu_item *it = row.item;
    char value[64], shown[64];
    rt_ios_setting_value(it->key, value, sizeof(value));
    rt_menu_display(it, value, shown, sizeof(shown));
    row.value.text = @(shown);
    if (row.slider) row.slider.value = (float)rt_menu_slider_value(it, value);
    if (row.toggle) row.toggle.on = strcmp(rt_menu_step(it, value, 0, shown, sizeof(shown)), "0") != 0;
    if (row.segments) {
        rt_menu_step(it, value, 0, shown, sizeof(shown));   // normalized: one of the values
        for (int i = 0; it->values[i]; ++i) if (!strcmp(it->values[i], shown)) row.segments.selectedSegmentIndex = i;
    }
}
// Stores a value (snapped to the item's step) and shows it.
- (void)store:(P2MenuRow *)row value:(const char *)value {
    char normal[64], now[64];
    rt_menu_step(row.item, value, 0, normal, sizeof(normal));
    rt_ios_setting_value(row.item->key, now, sizeof(now));
    if (strcmp(normal, now)) rt_ios_setting_set(row.item->key, normal);
    [self refresh:row];
}
- (void)resetPressed {
    P2MenuRow *row = nil;
    for (P2MenuRow *r in rows) if (r.kind == ROW_RESET) row = r;
    UIButtonConfiguration *conf = row.button.configuration;
    CFTimeInterval now = CACurrentMediaTime();
    if (now > resetArmedUntil) {   // first press: ask again, nothing changed yet
        resetArmedUntil = now + 3;
        conf.title = @"Press again to reset everything";
        conf.baseBackgroundColor = [UIColor colorWithRed:.7 green:.2 blue:.15 alpha:.8];
        row.button.configuration = conf;
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.05 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            if (CACurrentMediaTime() < self->resetArmedUntil || !row.button.window) return;
            UIButtonConfiguration *back = row.button.configuration;
            back.title = @"Reset to defaults"; back.baseBackgroundColor = nil;
            row.button.configuration = back;
        });
        return;
    }
    resetArmedUntil = 0;
    for (int i = 0; i < rt_menu_item_count; ++i) rt_ios_setting_reset(rt_menu_items[i].key);
    for (P2MenuRow *r in rows) [self refresh:r];
    conf.title = @"Defaults restored"; conf.baseBackgroundColor = nil;
    row.button.configuration = conf;
}

// ---- controller ----
- (GCController *)pad {
    for (GCController *c in GCController.controllers) if (c.extendedGamepad) return c;
    return nil;
}
- (BOOL)padActive {
    GCController *pad = [self pad];
    GCExtendedGamepad *g = pad.extendedGamepad;
    if (!g) return NO;
    for (GCControllerButtonInput *b in pad.physicalInputProfile.allButtons) if (b.isPressed) return YES;
    return fabsf(g.leftThumbstick.xAxis.value) > .3f || fabsf(g.leftThumbstick.yAxis.value) > .3f;
}
- (void)showFocus {
    for (NSUInteger i = 0; i < rows.count; ++i) {
        UIView *v = rows[i].view;
        BOOL on = (NSInteger)i == focus;
        v.backgroundColor = on ? [UIColor colorWithWhite:1 alpha:.14] : UIColor.clearColor;
        v.layer.borderWidth = on ? 1.5 : 0;
        v.layer.borderColor = [UIColor colorWithRed:.45 green:.75 blue:1 alpha:.9].CGColor;
    }
    if (focus >= 0 && focus < (NSInteger)rows.count && rows[focus].kind != ROW_CLOSE) {
        UIView *v = rows[focus].view;
        [scroll scrollRectToVisible:CGRectInset([v convertRect:v.bounds toView:scroll], 0, -24) animated:YES];
    }
}
- (void)moveFocus:(int)dir {
    if (!rows.count) return;
    focus = focus < 0 ? 0 : (focus + dir + (NSInteger)rows.count) % (NSInteger)rows.count;
    [self showFocus];
}
- (void)change:(int)dir {
    if (focus < 0 || rows[focus].kind != ROW_SETTING) return;
    P2MenuRow *row = rows[focus];
    char value[64], next[64];
    rt_ios_setting_value(row.item->key, value, sizeof(value));
    [self store:row value:rt_menu_step(row.item, value, dir, next, sizeof(next))];
}
- (void)activate {
    if (focus < 0) return;
    P2MenuRow *row = rows[focus];
    switch (row.kind) {
    case ROW_CLOSE: [self closeMenu]; return;
    case ROW_RESET: [self resetPressed]; return;
    case ROW_RECOMMENDED: [self useRecommended]; return;
    case ROW_SWITCH_GAME: [self switchGamePressed:nil]; return;
    case ROW_CALIBRATE: [self openCalibration]; return;
    case ROW_MAP: [self startMap:row]; return;
    case ROW_COMMUNITY: [self openCommunity]; return;
    case ROW_SETTING: break;
    }
    const rt_menu_item *it = row.item;
    if (it->kind == RT_MENU_SLIDER) return;   // left and right move sliders
    char value[64], next[64];
    rt_ios_setting_value(it->key, value, sizeof(value));
    rt_menu_step(it, value, 1, next, sizeof(next));
    char normal[64];
    rt_menu_step(it, value, 0, normal, sizeof(normal));
    if (it->kind == RT_MENU_CHOICE && !strcmp(next, normal)) snprintf(next, sizeof(next), "%s", it->values[0]);   // wraps
    [self store:row value:next];
}
- (void)pollPad:(BOOL)paused {
    GCExtendedGamepad *g = [self pad].extendedGamepad;
    BOOL a = g.buttonA.isPressed, b = g.buttonB.isPressed, options = g.buttonOptions.isPressed;
    if (options && !prevOptions) {
        if (menu_open) [self closeMenu];
        else if (paused) [self openMenu];
    }
    prevOptions = options;
    if (community && g) {
        float cx = g.dpad.xAxis.value, cy = g.dpad.yAxis.value;
        if (fabsf(g.leftThumbstick.xAxis.value) > fabsf(cx)) cx = g.leftThumbstick.xAxis.value;
        if (fabsf(g.leftThumbstick.yAxis.value) > fabsf(cy)) cy = g.leftThumbstick.yAxis.value;
        CFTimeInterval t = CACurrentMediaTime();
        int dx = axis_step(cx > .5f ? 1 : cx < -.5f ? -1 : 0, &held[0], &repeatAt[0], t);
        int dy = axis_step(cy > .5f ? 1 : cy < -.5f ? -1 : 0, &held[1], &repeatAt[1], t);
        BOOL pa = a && !prevA, pb = b && !prevB;
        prevA = a; prevB = b;
        rt_ios_community_pad((__bridge void *)community, dx, dy, pa, pb);
        return;
    }
    if (menu_open && g) {
        if (b && !prevB) [self closeMenu];
        else if (a && !prevA) [self activate];
    }
    prevA = a; prevB = b;
    if (!menu_open || !g) return;
    float x = g.dpad.xAxis.value, y = g.dpad.yAxis.value;
    if (fabsf(g.leftThumbstick.xAxis.value) > fabsf(x)) x = g.leftThumbstick.xAxis.value;
    if (fabsf(g.leftThumbstick.yAxis.value) > fabsf(y)) y = g.leftThumbstick.yAxis.value;
    CFTimeInterval now = CACurrentMediaTime();
    int dx = axis_step(x > .5f ? 1 : x < -.5f ? -1 : 0, &held[0], &repeatAt[0], now);
    int dy = axis_step(y > .5f ? 1 : y < -.5f ? -1 : 0, &held[1], &repeatAt[1], now);
    if (dy) [self moveFocus:-dy];   // stick up: previous row
    if (dx) [self change:dx];
}
- (void)tick {
    BOOL paused = rt_ios_game_paused() != 0;
    if (menu_open && !paused) [self closeMenu];   // the game left its pause menu
    gear.hidden = menu_open || !paused;
    if (!gear.hidden) [self updateGearHint];
    // Simulator checks, no finger: P2_SETTINGS_MENU_OPEN=1 opens the panel once,
    // at the first pause; =map also starts the first map of Maps & Mods.
    static int auto_open = -1;
    if (auto_open < 0) {
        const char *e = getenv("P2_SETTINGS_MENU_OPEN");
        auto_open = !e ? 0 : !strcmp(e, "1") ? 1 : !strcmp(e, "map") ? 2 : !strcmp(e, "display") || !strcmp(e, "end") ? 3 : 0;
    }
    if (auto_open && !gear.hidden) {
        int start = auto_open == 2;
        auto_open = 0;
        [self openMenu];
        for (P2MenuRow *row in rows) if (start && row.kind == ROW_MAP) { [self startMap:row]; break; }
        // =end: scrolled to the bottom (Reset, Switch game, the version line).
        if (auto_open == 0 && getenv("P2_SETTINGS_MENU_OPEN") && !strcmp(getenv("P2_SETTINGS_MENU_OPEN"), "end")) {
            [scroll layoutIfNeeded];
            [scroll setContentOffset:CGPointMake(0, MAX(0, scroll.contentSize.height - scroll.bounds.size.height)) animated:NO];
        }
        // =display: scrolled to "Defaults for <device>" (screenshots of the defaults row).
        if (auto_open == 0 && getenv("P2_SETTINGS_MENU_OPEN") && !strcmp(getenv("P2_SETTINGS_MENU_OPEN"), "display"))
            for (P2MenuRow *row in rows) if (row.kind == ROW_RECOMMENDED) {
                [scroll layoutIfNeeded];
                CGRect r = [scroll convertRect:row.view.bounds fromView:row.view];
                [scroll setContentOffset:CGPointMake(0, MAX(0, MIN(CGRectGetMinY(r) - 160,
                                                     scroll.contentSize.height - scroll.bounds.size.height))) animated:NO];
            }
    }
    // P2_COMMUNITY_TEST=<Workshop id> (simulator checks): at the first pause, opens
    // Community Maps on that chamber, downloads it and starts it;
    // =show:<id> only opens its details (screenshots of the browser).
    static int community_test = -1;
    static unsigned long long community_id;
    if (community_test < 0) {
        const char *e = getenv("P2_COMMUNITY_TEST");
        int show = e && !strncmp(e, "show:", 5);
        community_id = e ? strtoull(e + (show ? 5 : 0), NULL, 10) : 0;
        community_test = !community_id ? 0 : show ? 2 : 1;
    }
    if (community_test && !gear.hidden) {
        int play = community_test == 1;
        community_test = 0;
        [self openMenu];
        [self openCommunity];
        if (community) rt_ios_community_open_item((__bridge void *)community, community_id, play);
    }
    if (closing_hold && ![self padActive]) closing_hold = 0;
    if (icloudStatus && CACurrentMediaTime() - icloudShownAt > 1) [self showICloudStatus];
    if (paused || menu_open) [self pollPad:paused];
}
@end

void *rt_ios_settings_menu_create(void) {
    return (__bridge_retained void *)[[P2SettingsMenu alloc] initWithFrame:CGRectZero];
}
void rt_ios_settings_menu_tick(void *menu) { [(__bridge P2SettingsMenu *)menu tick]; }
#endif
