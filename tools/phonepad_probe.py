#!/usr/bin/env python3
"""Stands in for a phone against Sauce's phone controller (round 23 item 9b):
fetches the page, checks a wrong code is refused, connects the WebSocket with
the right one, measures round trips (the same pings the page sends), then
pushes the move stick forward for a few seconds.

    tools/phonepad_probe.py --host 192.168.1.20 --code 0427 [--port 8017] [--pings 50] [--walk 3]

Prints the page's size, the 403, the round-trip times (min / median / p95 /
max, in ms) and what was sent. Standard library only.
"""
import argparse
import base64
import os
import socket
import statistics
import struct
import time
import urllib.error
import urllib.request


LEFTOVER = {}   # bytes that came with the handshake (a frame sent right behind it, like "busy")


def ws_connect(host, port, code):
    s = socket.create_connection((host, port), timeout=5)
    key = base64.b64encode(os.urandom(16)).decode()
    s.sendall((f'GET /ws?code={code} HTTP/1.1\r\nHost: {host}:{port}\r\nUpgrade: websocket\r\n'
               f'Connection: Upgrade\r\nSec-WebSocket-Key: {key}\r\nSec-WebSocket-Version: 13\r\n\r\n').encode())
    head = b''
    while b'\r\n\r\n' not in head:
        chunk = s.recv(1024)
        if not chunk:
            raise SystemExit('connection closed during the handshake')
        head += chunk
    status = head.split(b'\r\n', 1)[0].decode()
    if ' 101 ' not in status:
        raise SystemExit(f'handshake refused: {status}')
    LEFTOVER[s.fileno()] = head.split(b'\r\n\r\n', 1)[1]
    return s


def send(s, text):   # a masked text frame, as a browser sends it
    data, mask = text.encode(), os.urandom(4)
    s.sendall(bytes([0x81, 0x80 | len(data)]) + mask + bytes(b ^ mask[i % 4] for i, b in enumerate(data)))


def receive(s):
    buf = LEFTOVER.pop(s.fileno(), b'')
    def take(k):
        nonlocal buf
        while len(buf) < k:
            chunk = s.recv(4096)
            if not chunk:
                raise SystemExit('closed by Sauce')
            buf += chunk
        out, buf = buf[:k], buf[k:]
        return out
    head = take(2)
    n = head[1] & 0x7f
    if n == 126:
        n = struct.unpack('>H', take(2))[0]
    data = take(n)
    if buf:
        LEFTOVER[s.fileno()] = buf
    return data.decode()


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--host', required=True)
    ap.add_argument('--code', required=True)
    ap.add_argument('--port', type=int, default=8017)
    ap.add_argument('--pings', type=int, default=50)
    ap.add_argument('--walk', type=float, default=3.0, help='seconds of forward stick (0: none)')
    a = ap.parse_args()
    page = urllib.request.urlopen(f'http://{a.host}:{a.port}/?code={a.code}', timeout=5).read()
    print(f'page: {len(page)} bytes, .local name {"filled in" if b"%%LOCAL%%" not in page else "missing"}')
    try:
        ws_connect(a.host, a.port, '9999' if a.code != '9999' else '0000').close()
        print('wrong code: ACCEPTED (bad)')
    except SystemExit as e:
        print(f'wrong code: refused ({e})')
    s = ws_connect(a.host, a.port, a.code)
    rtts = []
    for _ in range(a.pings):
        t0 = time.perf_counter()
        send(s, f'p {t0 * 1000:.1f}')
        reply = receive(s)
        rtts.append((time.perf_counter() - t0) * 1000)
        assert reply.startswith('q '), reply
        time.sleep(0.05)
    rtts.sort()
    p95 = rtts[min(len(rtts) - 1, int(len(rtts) * .95))]
    print(f'round trip over Wi-Fi: min {rtts[0]:.1f} ms, median {statistics.median(rtts):.1f} ms, '
          f'p95 {p95:.1f} ms, max {rtts[-1]:.1f} ms ({len(rtts)} pings)')
    send(s, f'r {statistics.median(rtts):.1f}')
    if a.walk > 0:
        end = time.time() + a.walk
        while time.time() < end:
            send(s, 'm 0 1')   # forward
            time.sleep(0.03)
        send(s, 'm 0 0')
        print(f'walked forward {a.walk:.0f} s')
    send(s, 'b jump 1'); time.sleep(0.1); send(s, 'b jump 0')
    print('jumped')
    time.sleep(0.5)
    s.close()


if __name__ == '__main__':
    main()
