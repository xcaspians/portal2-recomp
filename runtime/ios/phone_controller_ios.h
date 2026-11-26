// Round 26 (item 4): this device as the controller of Sauce on an iPad nearby
// (phone_controller_ios.m). Objective-C only.
#pragma once
#ifdef __OBJC__
@class UIView;
// Full-screen view over the launcher: the iPads found, one tap connects; `close` when left.
UIView *rt_ios_phone_controller_panel(void (^close)(void));
#endif
