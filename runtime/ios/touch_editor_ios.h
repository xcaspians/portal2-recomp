// "Edit button layout" (runtime/ios/touch_editor_ios.m; rules in touch_layout.c).
#pragma once

// A full-screen UIView (retained, __bridge_transfer it); close() on Save or Cancel.
void *rt_ios_touch_editor_create(void (*close)(void *), void *ctx);
