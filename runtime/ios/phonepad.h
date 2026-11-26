// A phone as a controller, no app (round 23 item 9b): Sauce serves a web page
// on the local network (phonepad_ios.m), a phone opens it from a QR code and
// sends its touch controls over a WebSocket. This part is plain C, checked in
// tests/checks_phonepad.c: the pairing code, the HTTP request, the WebSocket
// handshake and frames, and the controller messages.
#ifndef RT_PHONEPAD_H
#define RT_PHONEPAD_H
#include <stddef.h>
#include <stdint.h>

// ---- Pairing ----
// A fresh 4-digit code each session (from `random`, 0..9999), "0427".
void rt_phonepad_code(uint32_t random, char out[5]);
// Is `query` ("code=0427&x=1", without '?') carrying this session's code?
int rt_phonepad_code_ok(const char *query, const char *code);
// A local-network peer (IPv4 private ranges, link-local, loopback; IPv6
// link-local fe80::/10, unique-local fc00::/7, loopback). Others are refused.
int rt_phonepad_local_address(const char *address);

// ---- HTTP ----
typedef struct {
    char method[8];
    char path[128];       // without the query
    char query[128];      // after '?', or ""
    char ws_key[64];      // Sec-WebSocket-Key, or "" (not an upgrade)
    int complete;         // the blank line after the headers was seen
} rt_phonepad_request;
// Parses one request's head from `text` (len bytes). 1 when complete and valid.
int rt_phonepad_parse_request(const char *text, size_t len, rt_phonepad_request *out);
// The page's files, by path ("/" is index.html); NULL for anything else.
const char *rt_phonepad_file(const char *path, const char **content_type);
// Sec-WebSocket-Accept for a key (base64 of SHA-1 of key + the RFC 6455 GUID).
void rt_phonepad_ws_accept(const char *key, char out[32]);

// ---- WebSocket frames (RFC 6455; the phone's frames are masked) ----
// Decodes one frame from buf (len bytes): 0 if more bytes are needed, -1 on
// an unsupported frame (fragmented, > 4096 bytes), else the bytes used, with
// *opcode and the unmasked payload (NUL-terminated) in payload[cap].
int rt_phonepad_ws_decode(const uint8_t *buf, size_t len, int *opcode, char *payload, size_t cap);
// Encodes an unmasked text frame (server to phone): bytes written to out.
size_t rt_phonepad_ws_text(const char *text, uint8_t *out, size_t cap);

// ---- Controller messages (the page's script, ios/phonepad/index.html) ----
// "m x y" move stick, "l x y" look stick (x, y in -1..1, y up), "b NAME 1|0"
// a button (jump, crouch, use, blue, orange, pause), "p T" a ping (answered
// "q T"). Buttons map to SDL_GameController numbers, triggers to axes 4 and 5:
// Portal 2's own bindings, as for touch as player 2 (touch_layout.c).
typedef enum { RT_PHONE_NONE, RT_PHONE_MOVE, RT_PHONE_LOOK, RT_PHONE_BUTTON, RT_PHONE_PING } rt_phone_kind;
typedef struct {
    rt_phone_kind kind;
    double x, y;          // sticks
    int button, trigger;  // SDL button (-1) or trigger axis (-1)
    int down;
    char ping[24];
} rt_phone_message;
int rt_phonepad_parse(const char *text, rt_phone_message *out);
// A stick value (-1..1, y up) as an SDL axis (-32767..32767, y down for LEFTY/RIGHTY).
int16_t rt_phonepad_axis(double value, int is_y);

#endif
