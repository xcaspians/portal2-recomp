// "Edit button layout" (in-game settings menu): the overlay's buttons, drawn
// where the game puts them, become draggable; each has a size handle. Rules in
// touch_layout.c (rt_touch_custom_*): inside the screen, snapped to an 8-point
// grid, out of the sticks' resting areas. Saved per device kind (phone or iPad)
// and game; the Opacity and Button size sliders still apply on top. The game is
// in its menu meanwhile (the editor opens from the settings menu).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "env_ios.h"
#include "game_ios.h"
#include "touch_editor_ios.h"
#include "layout_sync_ios.h"
#include "touch_layout.h"

NSString *rt_ios_touch_layout_key_for(rt_ios_layout layout);                          // touch_controls.m
const char *rt_ios_touch_layout_text_for(rt_ios_layout layout, char *out, size_t size);  // touch_controls.m
int rt_ios_backtap_available(void);                           // backtap_ios.m
int rt_ios_backtap_dataset_on(void);

@interface P2TouchEditor : UIView
@end

@implementation P2TouchEditor {
    void (*onClose)(void *);
    void *ctx;
    rt_ios_layout which;   // the game layout edited
    rt_touch_custom_layout layout;
    const rt_touch_button *buttons;
    int count;
    NSMutableArray<UIView *> *discs, *handles;
    int dragging;      // index of the button being moved or resized, -1
    BOOL resizing;
    CGPoint grab;      // finger offset from the centre when the drag began
    float sizeScale;
    UILabel *hint;
}

- (instancetype)initWithClose:(void (*)(void *))close ctx:(void *)c layout:(rt_ios_layout)l {
    if (!(self = [super initWithFrame:CGRectZero])) return nil;
    onClose = close; ctx = c; which = l;
    self.backgroundColor = [UIColor colorWithWhite:0 alpha:.55];
    self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    char text[1024];
    rt_touch_custom_parse(rt_ios_touch_layout_text_for(which, text, sizeof(text)), &layout);
    buttons = rt_touch_layout(which, &count);
    sizeScale = fminf(fmaxf(rt_ios_env_float("P2_TOUCH_SIZE", 1, .5f), .5f), 2);
    discs = [NSMutableArray array]; handles = [NSMutableArray array];
    dragging = -1;
    for (int i = 0; i < count; ++i) {
        UIView *d = [UIView new];
        d.layer.borderWidth = 2; d.layer.borderColor = [UIColor colorWithWhite:1 alpha:.9].CGColor;
        d.backgroundColor = buttons[i].color == RT_TOUCH_BLUE ? [UIColor colorWithRed:.15 green:.45 blue:1 alpha:.6]
                          : buttons[i].color == RT_TOUCH_ORANGE ? [UIColor colorWithRed:1 green:.55 blue:.1 alpha:.6]
                                                                 : [UIColor colorWithWhite:.15 alpha:.75];
        UIImageView *icon = [[UIImageView alloc] initWithImage:buttons[i].symbol ? [UIImage systemImageNamed:@(buttons[i].symbol)] : nil];
        icon.tintColor = UIColor.whiteColor; icon.contentMode = UIViewContentModeScaleAspectFit;
        icon.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        [d addSubview:icon];
        d.userInteractionEnabled = NO;
        [self addSubview:d];
        [discs addObject:d];
        UIView *h = [UIView new];
        h.backgroundColor = UIColor.whiteColor; h.layer.cornerRadius = 9;
        h.userInteractionEnabled = NO;
        [self addSubview:h];
        [handles addObject:h];
    }
    hint = [UILabel new];
    // Round 26: where the layout goes (iCloud, or this device only and why).
    hint.text = [NSString stringWithFormat:@"Drag a button to move it, its white dot to resize it.\n%s",
                 rt_ios_layouts_sync_status()];
    hint.numberOfLines = 2;
    hint.textColor = UIColor.whiteColor; hint.font = [UIFont systemFontOfSize:14 weight:UIFontWeightMedium];
    hint.textAlignment = NSTextAlignmentCenter;
    [self addSubview:hint];
    UIStackView *bar = [UIStackView new];
    bar.spacing = 10; bar.tag = 77;
    __weak P2TouchEditor *me = self;
    NSArray *names = @[@"Reset", @"Cancel", @"Save"];
    for (NSUInteger k = 0; k < names.count; ++k) {
        UIButtonConfiguration *conf = k == 2 ? [UIButtonConfiguration filledButtonConfiguration] : [UIButtonConfiguration grayButtonConfiguration];
        conf.title = names[k]; conf.baseForegroundColor = UIColor.whiteColor;
        conf.cornerStyle = UIButtonConfigurationCornerStyleCapsule;
        [bar addArrangedSubview:[UIButton buttonWithConfiguration:conf primaryAction:[UIAction actionWithHandler:^(UIAction *a) {
            (void)a; if (k == 0) [me reset]; else if (k == 1) [me finish:NO]; else [me finish:YES];
        }]]];
    }
    [self addSubview:bar];
    // P2_EDITOR_TEST="<button>,<fx>,<fy>" (device checks, round 26): that button moved there, saved 3 s later.
    const char *test = getenv("P2_EDITOR_TEST");
    char name[32]; float fx, fy;
    if (test && sscanf(test, "%31[^,],%f,%f", name, &fx, &fy) == 3)
        for (int a = 0; a < RT_TOUCH_ACTION_COUNT; ++a)
            if (!strcmp(rt_touch_action_name((rt_touch_action)a), name)) {
                layout.b[a] = (rt_touch_custom){1, fx, fy, 1.3f};
                fprintf(stderr, "[ios] essai éditeur (%s) : %s en %.2f,%.2f\n", rt_touch_layout_id(which), name, fx, fy);
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 3 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{ [me finish:YES]; });
            }
    return self;
}
- (CGRect)area { return UIEdgeInsetsInsetRect(self.bounds, self.safeAreaInsets); }
- (double)stickRadius { return fmin(66, self.area.size.height * .15) * sizeScale; }
- (BOOL)shown:(int)i {
    if ((buttons[i].flags & RT_TOUCH_PORTAL) && !rt_ios_env_flag("P2_PORTAL_BUTTONS", !rt_ios_backtap_available())) return NO;
    if ((buttons[i].flags & RT_TOUCH_DATASET) && !rt_ios_backtap_dataset_on()) return NO;
    return YES;
}
- (void)place:(int)i centre:(CGPoint *)c size:(double *)size {
    CGRect a = self.area;
    double cx, cy;
    rt_touch_place_custom(&buttons[i], &layout.b[buttons[i].action], sizeScale, CGRectGetMinX(a), CGRectGetMinY(a),
                          a.size.width, a.size.height, &cx, &cy, size);
    *c = CGPointMake(cx, cy);
}
- (void)layoutSubviews {
    [super layoutSubviews];
    for (int i = 0; i < count; ++i) {
        CGPoint c; double size;
        [self place:i centre:&c size:&size];
        UIView *d = discs[(NSUInteger)i], *h = handles[(NSUInteger)i];
        d.hidden = h.hidden = ![self shown:i];
        d.frame = CGRectMake(c.x - size / 2, c.y - size / 2, size, size);
        d.layer.cornerRadius = size / 2;
        d.subviews.firstObject.frame = CGRectInset(d.bounds, size * .25, size * .25);
        double hx = c.x + size / 2 * M_SQRT1_2, hy = c.y + size / 2 * M_SQRT1_2;
        h.frame = CGRectMake(hx - 9, hy - 9, 18, 18);
        d.layer.borderColor = (i == dragging ? UIColor.yellowColor : [UIColor colorWithWhite:1 alpha:.9]).CGColor;
    }
    CGRect a = self.area;
    hint.frame = CGRectMake(CGRectGetMinX(a) + 120, CGRectGetMinY(a) + 4, a.size.width - 240, 36);
    UIView *bar = [self viewWithTag:77];
    CGSize s = [bar systemLayoutSizeFittingSize:UILayoutFittingCompressedSize];
    bar.frame = CGRectMake(CGRectGetMidX(a) - s.width / 2, CGRectGetMinY(a) + 44, s.width, s.height);
    [self setNeedsDisplay];
}
// The sticks' resting areas, as guides.
- (void)drawRect:(CGRect)rect {
    (void)rect;
    double rests[2][2];
    rt_touch_stick_rests(which, rests);
    CGRect a = self.area;
    CGContextRef g = UIGraphicsGetCurrentContext();
    CGContextSetStrokeColorWithColor(g, [UIColor colorWithWhite:1 alpha:.35].CGColor);
    CGContextSetLineWidth(g, 2);
    CGFloat dash[] = {6, 6};
    CGContextSetLineDash(g, 0, dash, 2);
    for (int k = 0; k < 2; ++k) {
        double r = self.stickRadius, x = CGRectGetMinX(a) + rests[k][0] * a.size.width, y = CGRectGetMinY(a) + rests[k][1] * a.size.height;
        CGContextStrokeEllipseInRect(g, CGRectMake(x - r, y - r, 2 * r, 2 * r));
    }
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    CGPoint p = [touches.anyObject locationInView:self];
    dragging = -1;
    for (int i = count - 1; i >= 0 && dragging < 0; --i) {
        if (![self shown:i]) continue;
        UIView *h = handles[(NSUInteger)i];
        if (CGRectContainsPoint(CGRectInset(h.frame, -12, -12), p)) { dragging = i; resizing = YES; }
        else if (hypot(p.x - CGRectGetMidX(discs[(NSUInteger)i].frame), p.y - CGRectGetMidY(discs[(NSUInteger)i].frame)) <
                 discs[(NSUInteger)i].frame.size.width / 2) {
            dragging = i; resizing = NO;
            grab = CGPointMake(p.x - CGRectGetMidX(discs[(NSUInteger)i].frame), p.y - CGRectGetMidY(discs[(NSUInteger)i].frame));
        }
    }
    [self setNeedsLayout];
}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    if (dragging < 0) return;
    CGPoint p = [touches.anyObject locationInView:self];
    CGRect a = self.area;
    const rt_touch_button *b = &buttons[dragging];
    rt_touch_custom *c = &layout.b[b->action];
    CGPoint centre; double size;
    [self place:dragging centre:&centre size:&size];
    if (!c->set) *c = (rt_touch_custom){1, (float)((centre.x - CGRectGetMinX(a)) / a.size.width),
                                        (float)((centre.y - CGRectGetMinY(a)) / a.size.height), 1};
    if (resizing) {   // distance from the centre sets the size, 0.6x to 1.8x
        double d = hypot(p.x - centre.x, p.y - centre.y) * 2 / M_SQRT2;
        c->scale = (float)fmin(fmax(d / (b->size * sizeScale), .6), 1.8);
        size = b->size * c->scale * sizeScale;
    }
    double cx = resizing ? centre.x - CGRectGetMinX(a) : p.x - grab.x - CGRectGetMinX(a);
    double cy = resizing ? centre.y - CGRectGetMinY(a) : p.y - grab.y - CGRectGetMinY(a);
    double rests[2][2];
    rt_touch_stick_rests(which, rests);
    rt_touch_custom_clamp(&cx, &cy, size, a.size.width, a.size.height, (const double (*)[2])rests, 2, self.stickRadius, 8);
    c->fx = (float)(cx / a.size.width); c->fy = (float)(cy / a.size.height);
    [self setNeedsLayout];
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)touches; (void)event;
    dragging = -1;
    [self setNeedsLayout];
}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { [self touchesEnded:touches withEvent:event]; }
- (void)reset {
    memset(&layout, 0, sizeof(layout));
    [self setNeedsLayout];
}
- (void)finish:(BOOL)save {
    if (save) {
        char text[1024];
        int any = 0;
        // One layout for every game: this game's buttons change, another game's stay.
        rt_touch_custom_layout stored;
        rt_touch_custom_parse(rt_ios_touch_layout_text_for(which, text, sizeof(text)), &stored);
        rt_touch_custom_merge(&stored, &layout, which);
        layout = stored;
        for (int i = 0; i < RT_TOUCH_ACTION_COUNT; ++i) any |= layout.b[i].set;
        if (any && rt_touch_custom_format(&layout, text, sizeof(text)))
            [NSUserDefaults.standardUserDefaults setObject:@(text) forKey:rt_ios_touch_layout_key_for(which)];
        // Reset: the game's own default ("v1" with nothing set, so the old shared places don't come back).
        else [NSUserDefaults.standardUserDefaults setObject:@"v1" forKey:rt_ios_touch_layout_key_for(which)];
        fprintf(stderr, "[ios] disposition des boutons (%s) : %s\n", rt_touch_layout_id(which), any ? text : "par défaut");
        [NSNotificationCenter.defaultCenter postNotificationName:@"P2SettingsChanged" object:@"P2TouchLayout"];
        rt_ios_layouts_saved(which);   // round 26: stamped, then to iCloud (per device kind)
    }
    onClose(ctx);
}
@end

void *rt_ios_touch_editor_create(void (*close)(void *), void *ctx, rt_ios_layout layout) {
    return (__bridge_retained void *)[[P2TouchEditor alloc] initWithClose:close ctx:ctx layout:layout];
}
#endif
