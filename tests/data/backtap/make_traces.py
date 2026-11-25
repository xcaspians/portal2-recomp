#!/usr/bin/env python3
"""Motion traces for tests/checks_back_tap.c, in the raw recording format of
the app's "Record taps" setting (P2_BACKTAP_RAW, runtime/ios/backtap_ios.m):

    m t ax ay az wx wy wz gx gy gz orient active     one CoreMotion sample
    t t_touch                                         a touch starts or ends

Shapes follow what the iPhone and iPad recordings of earlier rounds showed
(docs/backtap.md): a back tap is a 20 ms push along z with a 30 Hz hand ring
and a brief x rotation whose sign is the side; a button press is a push the
other way at the touch, then a jolt as the hand lets go and moves off; a grip
shift turns the device in a hand-sized motion just before a jolt. Each file
says what the detector must make of it ("# expect ..."). Real recordings
dropped in this folder with an expect line are replayed the same way.

    python3 tests/data/backtap/make_traces.py   (rewrites the four .txt files)
"""
import math, os, random

HERE = os.path.dirname(os.path.abspath(__file__))
RATE = 100.0
GRAV = (-0.8, 0.0, -0.6)   # landscape, tilted back


def half_sine(x, w):
    return 0.0 if x < 0 or x > w else math.sin(math.pi * x / w)


def tap(t, x, ry, accel, rot, ring=0.04, axis=2, rot_z=0.0):
    """Contribution of one jolt at time x (s) after its start."""
    a = [0.0, 0.0, 0.0]; w = [0.0, 0.0, 0.0]
    if x < 0 or x > 0.4:
        return a, w
    push = accel * half_sine(x, 0.020) - 0.3 * accel * half_sine(x - 0.020, 0.020)
    if x > 0.020:
        push += ring * math.exp(-(x - 0.020) / 0.08) * math.sin(2 * math.pi * 30 * (x - 0.020))
    a[axis] -= push
    shape = x / 0.005 if x < 0.005 else math.exp(-(x - 0.005) / 0.030)
    w[0] += ry * rot * shape
    w[2] += rot_z * shape
    return a, w


def write(name, expect, length, jolts, touches=(), moves=()):
    rnd = random.Random(name)
    lines = [f"# {name}: synthetic trace (tests/data/backtap/make_traces.py)", f"# expect {expect}",
             "# m t ax ay az wx wy wz gx gy gz orient active | t t_touch"]
    events = []
    for i in range(int(length * RATE)):
        t = 100.0 + i / RATE
        u = t - 100.0
        a = [rnd.uniform(-0.005, 0.005) for _ in range(3)]
        w = [rnd.uniform(-0.01, 0.01) for _ in range(3)]
        g = list(GRAV)
        for j in jolts:
            da, dw = tap(t, u - j["t"], j.get("ry", 1), j["accel"], j.get("rot", 0.4), j.get("ring", 0.04),
                         j.get("axis", 2), j.get("rot_z", 0.0))
            for k in range(3):
                a[k] += da[k]; w[k] += dw[k]
        for m in moves:   # grip shift: a smooth turn and drift, one half-sine of rotation
            s = half_sine(u - m["t"], m["dur"])
            w[1] += m["rot"] * s; w[2] += 0.6 * m["rot"] * s
            a[0] += m["accel"] * s; a[1] += 0.5 * m["accel"] * s
        events.append((t, "m %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f 1 1" % (t, *a, *w, *g)))
    for tt in touches:   # UIKit delivers a touch ~20 ms after its timestamp
        events.append((100.0 + tt + 0.020, "t %.4f" % (100.0 + tt)))
    events.sort(key=lambda e: e[0])
    with open(os.path.join(HERE, name + ".txt"), "w") as f:
        f.write("\n".join(lines + [e[1] for e in events]) + "\n")


# Real back taps, left then right of the screen (orientation +1: ry = side).
write("left_taps", "left=4 right=0 heard=4", 5.0,
      [{"t": 0.8 + 1.0 * i, "ry": -1, "accel": 0.14 + 0.03 * i, "rot": 0.35} for i in range(4)])
write("right_taps", "left=0 right=4 heard=4", 5.0,
      [{"t": 0.8 + 1.0 * i, "ry": 1, "accel": 0.14 + 0.03 * i, "rot": 0.35} for i in range(4)])
# The calibration screen's Start button: the finger pushes the glass (a jolt
# backwards at the touch), lets go 120 ms later, and the hand moving off jolts
# the device 130 ms after that, outside the old ±80 ms guard.
write("button_press", "left=0 right=0 heard=0", 3.0,
      [{"t": 1.0, "ry": 1, "accel": -0.12, "rot": 0.2}, {"t": 1.25, "ry": -1, "accel": 0.16, "rot": 0.3}],
      touches=(1.0, 1.12))
# A hand shifting its grip: the device turns by hand (1 rad/s, 120 ms), then
# the fingers settle with a sharp jolt. Calm before, so the 300 ms agitation
# gate alone let it through.
write("hand_shift", "left=0 right=0 heard=0", 3.0,
      [{"t": 1.10, "ry": 1, "accel": 0.18, "rot": 0.4}],
      moves=[{"t": 0.98, "dur": 0.14, "rot": 1.0, "accel": 0.03}])
