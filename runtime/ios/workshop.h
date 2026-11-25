// Community Maps (workshop_ios.m): Portal 2's Steam Workshop chambers, browsed
// and downloaded through Valve's public Web API, without Steam on the device.
// Pure C, checked on the Mac by tests/checks_workshop.c: reading the API's
// answers, the browse request, links typed by the player, where a map is
// saved, the update badge and the cleanup of a canceled download.
//
// Details and downloads need no key (ISteamRemoteStorage/GetPublishedFileDetails
// and GetCollectionDetails); browsing (IPublishedFileService/QueryFiles) needs
// the player's own Steam Web API key. docs/fork/ROUND3_REPORT.md, docs/mods.md.
#pragma once
#include <stddef.h>

// ---- a small JSON reader (tokens over the original text, like jsmn) ----
enum { RT_JSON_OBJECT = 1, RT_JSON_ARRAY, RT_JSON_STRING, RT_JSON_PRIMITIVE };
typedef struct { int type, start, end, size; } rt_json_tok;   // size: members or elements
// Tokens of the whole text, in document order; their number, or -1 when the
// text is not JSON or needs more than max tokens.
int rt_json_parse(const char *js, size_t len, rt_json_tok *toks, int max);
// Index just past token i and everything inside it.
int rt_json_next(const rt_json_tok *toks, int count, int i);
// Value of key in object obj, or -1.
int rt_json_get(const char *js, const rt_json_tok *toks, int count, int obj, const char *key);
// A string's text, unescaped to UTF-8 (a primitive's text as is); 0 if it doesn't fit.
int rt_json_text(const char *js, const rt_json_tok *t, char *out, size_t size);
// A number, or a string holding one (the API gives file_size as "12345").
long long rt_json_number(const char *js, const rt_json_tok *t);

// ---- items ----
typedef struct {
    unsigned long long id;
    int found;              // result 1, a Portal 2 item
    int is_map;             // its file is a .bsp
    int is_collection;      // a list of other items (GetCollectionDetails)
    int coop, singleplayer; // from the Workshop tags
    char title[128], creator[24], file_url[512], filename[192], preview_url[512];
    char description[1536];
    long long file_size, time_created, time_updated, subscriptions, favorited;
} rt_ws_item;

// Items of a GetPublishedFileDetails or QueryFiles answer
// (response.publishedfiledetails); their number, or -1 for an unreadable
// answer. QueryFiles also gives the next page's cursor and the total.
int rt_ws_parse_items(const char *json, size_t len, rt_ws_item *items, int max, char *cursor, size_t cursor_size,
                      long long *total);
// Children of a GetCollectionDetails answer; their number, or -1.
int rt_ws_parse_collection(const char *json, size_t len, unsigned long long *ids, int max);
// Display names of a GetPlayerSummaries answer, matched to the given SteamIDs
// (names[i], 64 bytes each; "" when not found). The number found, or -1.
int rt_ws_parse_names(const char *json, size_t len, const char *const *steamids, int n, char (*names)[64]);
// Workshop descriptions use BBCode ([b], [url=...], [h1]...): tags removed,
// [*] list items become "• ", text kept.
void rt_ws_plain_text(const char *in, char *out, size_t size);

// ---- requests ----
typedef enum { RT_WS_POPULAR, RT_WS_NEWEST, RT_WS_TRENDING, RT_WS_SEARCH } rt_ws_sort;
// QueryFiles URL for app 620: maps only (no collections), one mode
// (Singleplayer or Cooperative tag), 20 a page, previews, descriptions and
// tags. cursor "*" (or NULL) for the first page. 0 if it doesn't fit.
int rt_ws_query_url(char *out, size_t size, const char *key, rt_ws_sort sort, const char *cursor, const char *search,
                    int coop);
// Form body of GetPublishedFileDetails for these ids.
int rt_ws_details_body(char *out, size_t size, const unsigned long long *ids, int n);
// Form body of GetCollectionDetails for one collection.
int rt_ws_collection_body(char *out, size_t size, unsigned long long id);
// An item id from what the player pasted: a Workshop link
// (steamcommunity.com/sharedfiles/filedetails/?id=..., /workshop/filedetails/)
// or the number alone. 0 if there is none.
int rt_ws_parse_link(const char *text, unsigned long long *id);

// ---- on the device ----
// The map's path under the folder the game reads (the user folder on iOS):
// "portal2/maps/workshop/<id>/<name>.bsp", the name from the Workshop file
// name, kept to letters, digits, '_', '-', '.' (else "<id>"). The console
// loads it as "workshop/<id>/<name>" (mods_list.c agrees).
int rt_ws_map_path(const rt_ws_item *it, char *out, size_t size);
// The folder of an item: "portal2/maps/workshop/<id>".
int rt_ws_item_dir(unsigned long long id, char *out, size_t size);
// Saved next to the map (p2workshop.txt): the Workshop's update date and size
// when it was downloaded, and its title. 1 on success.
int rt_ws_meta_write(const char *path, const rt_ws_item *it);
int rt_ws_meta_read(const char *path, long long *updated, long long *size, char *title, size_t title_size);
// Update badge: the Workshop has a newer version than the saved copy.
int rt_ws_update_available(long long saved_updated, long long saved_size, long long workshop_updated,
                           long long workshop_size);
// Downloads land in "<name>.bsp.part" and are renamed when complete. Under the
// workshop folder, removes every .part file, then item folders left with no
// map (their p2workshop.txt too). The number of files removed.
int rt_ws_cleanup(const char *workshop_dir);
// Deletes one downloaded map: its folder and everything in it. 1 if it existed.
int rt_ws_delete(const char *item_dir);
