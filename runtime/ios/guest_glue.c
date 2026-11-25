// A second game hosted in the Portal app (tools/build_ios.py, "ios_guests" in
// tools/games.py): compiled with that game's generated code and linked with it
// into one relocatable object (ld -r) that exports only these six functions,
// named <prefix>_rt_..., so its fn_<address> functions and generated tables
// stay local and never meet the host game's. ios/main_ios.c picks the game at
// launch; runtime/rt.h (RT_MULTI_GAME) reaches its code through rt_code.
#include "rt.h"
#include "modules.h"

#ifndef RT_GUEST_PREFIX
#error RT_GUEST_PREFIX (the guest game's name) is required
#endif
#define RT_CAT2(a, b) a##_##b
#define RT_CAT(a, b) RT_CAT2(a, b)
#define GUEST(name) RT_CAT(RT_GUEST_PREFIX, name)

int GUEST(rt_load_images)(const char *directory) { return rt_load_images(directory); }
uint32_t GUEST(rt_export)(const char *name) { return rt_export(name); }
gfn GUEST(rt_lookup)(uint32_t target) { return rt_lookup(target); }
const char *GUEST(rt_import_name)(uint32_t target) { return rt_import_name(target); }
const GuestModule *GUEST(rt_modules)(void) { return rt_modules; }
size_t GUEST(rt_module_count)(void) { return rt_module_count; }
