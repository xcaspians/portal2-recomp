// A phone as a controller (runtime/ios/phonepad.c, round 23 item 9b):
// pairing, local-only peers, the HTTP request, the WebSocket handshake and
// frames, and the controller messages.
#include "checks.h"
#include <string.h>
#include "../runtime/ios/phonepad.c"

RT_CHECK(phonepad_pairing_and_local_peers) {
    char code[5];
    rt_phonepad_code(427, code);
    assert(!strcmp(code, "0427"));
    rt_phonepad_code(123456789, code);
    assert(strlen(code) == 4);
    assert(rt_phonepad_code_ok("code=0427", "0427") && rt_phonepad_code_ok("x=1&code=0427&y=2", "0427"));
    assert(!rt_phonepad_code_ok("code=04271", "0427") && !rt_phonepad_code_ok("code=1111", "0427"));
    assert(!rt_phonepad_code_ok("", "0427") && !rt_phonepad_code_ok(NULL, "0427") && !rt_phonepad_code_ok("code=", ""));
    assert(rt_phonepad_local_address("192.168.1.20") && rt_phonepad_local_address("10.0.0.5"));
    assert(rt_phonepad_local_address("172.20.3.4") && !rt_phonepad_local_address("172.40.3.4"));
    assert(rt_phonepad_local_address("fe80::1c2b:3a") && rt_phonepad_local_address("fd12::1"));
    assert(rt_phonepad_local_address("::ffff:192.168.0.9") && !rt_phonepad_local_address("8.8.8.8"));
    assert(!rt_phonepad_local_address("2a01:cb00::1") && !rt_phonepad_local_address(""));
}

RT_CHECK(phonepad_http_and_websocket) {
    const char *req = "GET /ws?code=0427 HTTP/1.1\r\nHost: sauce-ab12.local:8017\r\nUpgrade: websocket\r\n"
                      "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n";
    rt_phonepad_request r;
    assert(rt_phonepad_parse_request(req, strlen(req), &r) && r.complete);
    assert(!strcmp(r.method, "GET") && !strcmp(r.path, "/ws") && !strcmp(r.query, "code=0427"));
    assert(!strcmp(r.ws_key, "dGhlIHNhbXBsZSBub25jZQ=="));
    assert(!rt_phonepad_parse_request("GET / HTTP/1.1\r\nHost: x", 24, &r) && !r.complete);   // not finished
    char accept[32];
    rt_phonepad_ws_accept("dGhlIHNhbXBsZSBub25jZQ==", accept);   // RFC 6455's own example
    assert(!strcmp(accept, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo="));
    const char *type = NULL;
    assert(!strcmp(rt_phonepad_file("/", &type), "index.html") && strstr(type, "text/html"));
    assert(!strcmp(rt_phonepad_file("/manifest.webmanifest", &type), "manifest.webmanifest"));
    assert(!rt_phonepad_file("/../Info.plist", &type) && !rt_phonepad_file("/etc/passwd", &type));
    // A masked text frame "m 0.5 1" from a phone, then the server's unmasked reply.
    uint8_t frame[32] = {0x81, 0x80 | 7, 1, 2, 3, 4};
    const char *msg = "m 0.5 1";
    for (int i = 0; i < 7; ++i) frame[6 + i] = (uint8_t)msg[i] ^ frame[2 + (i & 3)];
    char payload[64];
    int op = 0;
    assert(rt_phonepad_ws_decode(frame, 12, &op, payload, sizeof(payload)) == 0);   // one byte short
    assert(rt_phonepad_ws_decode(frame, 13, &op, payload, sizeof(payload)) == 13 && op == 1 && !strcmp(payload, msg));
    uint8_t out[32];
    size_t n = rt_phonepad_ws_text("q 123", out, sizeof(out));
    assert(n == 7 && out[0] == 0x81 && out[1] == 5 && !memcmp(out + 2, "q 123", 5));
}

RT_CHECK(phonepad_controller_messages) {
    rt_phone_message m;
    assert(rt_phonepad_parse("m 0.5 -1", &m) && m.kind == RT_PHONE_MOVE && m.x == .5 && m.y == -1);
    assert(rt_phonepad_parse("l 3 0", &m) && m.kind == RT_PHONE_LOOK && m.x == 1);   // clamped
    assert(rt_phonepad_parse("b jump 1", &m) && m.button == 0 && m.down && m.trigger == -1);
    assert(rt_phonepad_parse("b blue 1", &m) && m.trigger == 5 && m.button == -1);
    assert(rt_phonepad_parse("b orange 0", &m) && m.trigger == 4 && !m.down);
    assert(rt_phonepad_parse("b pause 1", &m) && m.button == 6);
    assert(rt_phonepad_parse("p 1712345.5", &m) && m.kind == RT_PHONE_PING && !strcmp(m.ping, "1712345.5"));
    assert(!rt_phonepad_parse("b fly 1", &m) && !rt_phonepad_parse("m nan 0", &m) && !rt_phonepad_parse("", &m));
    assert(rt_phonepad_axis(1, 0) == 32767 && rt_phonepad_axis(1, 1) == -32767 && rt_phonepad_axis(0, 0) == 0);
}

// Round 24 bug 2: fire released from the phone, or from touch as player 2,
// must read as released (SDL's virtual trigger rests at -32768; 0 was half
// pressed and kept the portal gun firing in split-screen co-op).
RT_CHECK(phonepad_trigger_release_player2) {
    assert(rt_pad_trigger_seen(rt_pad_trigger_axis(1)) == 32767);
    assert(rt_pad_trigger_seen(rt_pad_trigger_axis(0)) == 0);
    assert(rt_pad_trigger_seen(0) > 16000);   // the old release value: half pressed
    // The player-2 pad (the phone's controller after a real one): press, release, nothing further.
    rt_phone_pad p2; rt_phone_message m;
    rt_phonepad_pad_rest(&p2);
    assert(!rt_phonepad_pad_held(&p2));
    assert(rt_phonepad_parse("b blue 1", &m)); rt_phonepad_pad_apply(&p2, &m);
    assert(rt_pad_trigger_seen(p2.axes[5]) == 32767 && rt_phonepad_pad_held(&p2));
    assert(rt_phonepad_parse("b blue 0", &m)); rt_phonepad_pad_apply(&p2, &m);
    assert(rt_pad_trigger_seen(p2.axes[5]) == 0 && !rt_phonepad_pad_held(&p2));
    assert(rt_phonepad_parse("b blue 0", &m)); rt_phonepad_pad_apply(&p2, &m);   // a second release changes nothing
    assert(!rt_phonepad_pad_held(&p2));
    // Safety net: everything held, then "x" (page to the background) or a quiet phone: all released.
    const char *held[] = {"b orange 1", "b jump 1", "m 0 1", "l 0.5 0"};
    for (int i = 0; i < 4; ++i) { assert(rt_phonepad_parse(held[i], &m)); rt_phonepad_pad_apply(&p2, &m); }
    assert(rt_phonepad_pad_held(&p2) && p2.buttons == 1u);
    assert(rt_phonepad_parse("x", &m) && m.kind == RT_PHONE_RELEASE); rt_phonepad_pad_apply(&p2, &m);
    assert(!rt_phonepad_pad_held(&p2) && rt_pad_trigger_seen(p2.axes[4]) == 0);
}
