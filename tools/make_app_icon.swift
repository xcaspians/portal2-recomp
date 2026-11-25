// The one app's icon (round 19): original art, no Valve mark or logo.
// A white drop of sauce on a warm gradient, drawn with CoreGraphics.
//   swift tools/make_app_icon.swift ios/AppIcon.png
// iOS rounds the corners itself: the square is filled edge to edge.
import CoreGraphics
import Foundation
import ImageIO
import UniformTypeIdentifiers

let size = 1024
let out = CommandLine.arguments.count > 1 ? CommandLine.arguments[1] : "ios/AppIcon.png"
let space = CGColorSpace(name: CGColorSpace.sRGB)!
guard let ctx = CGContext(data: nil, width: size, height: size, bitsPerComponent: 8, bytesPerRow: 0, space: space,
                          bitmapInfo: CGImageAlphaInfo.premultipliedLast.rawValue) else { exit(1) }
let s = CGFloat(size)

// Background: tomato red at the bottom to amber at the top.
let colors = [CGColor(srgbRed: 0.80, green: 0.16, blue: 0.10, alpha: 1),
              CGColor(srgbRed: 0.98, green: 0.55, blue: 0.16, alpha: 1)] as CFArray
let gradient = CGGradient(colorsSpace: space, colors: colors, locations: [0, 1])!
ctx.drawLinearGradient(gradient, start: CGPoint(x: 0, y: 0), end: CGPoint(x: s * 0.35, y: s),
                       options: [.drawsBeforeStartLocation, .drawsAfterEndLocation])

// The drop: a circle with a pointed top, slightly off centre.
let cx = s * 0.5, cy = s * 0.42, r = s * 0.25
let drop = CGMutablePath()
drop.move(to: CGPoint(x: cx, y: cy + r * 2.15))   // the tip
drop.addCurve(to: CGPoint(x: cx + r, y: cy), control1: CGPoint(x: cx + r * 0.25, y: cy + r * 1.55),
              control2: CGPoint(x: cx + r, y: cy + r * 0.85))
drop.addArc(center: CGPoint(x: cx, y: cy), radius: r, startAngle: 0, endAngle: .pi, clockwise: true)
drop.addCurve(to: CGPoint(x: cx, y: cy + r * 2.15), control1: CGPoint(x: cx - r, y: cy + r * 0.85),
              control2: CGPoint(x: cx - r * 0.25, y: cy + r * 1.55))
drop.closeSubpath()
ctx.setShadow(offset: CGSize(width: 0, height: -s * 0.012), blur: s * 0.03,
              color: CGColor(srgbRed: 0.35, green: 0.05, blue: 0.02, alpha: 0.45))
ctx.setFillColor(CGColor(srgbRed: 1, green: 0.97, blue: 0.93, alpha: 1))
ctx.addPath(drop)
ctx.fillPath()

// A highlight on the drop.
ctx.setShadow(offset: .zero, blur: 0, color: nil)
ctx.setFillColor(CGColor(srgbRed: 1, green: 1, blue: 1, alpha: 0.9))
ctx.fillEllipse(in: CGRect(x: cx - r * 0.55, y: cy + r * 0.05, width: r * 0.3, height: r * 0.42))

let image = ctx.makeImage()!
let url = URL(fileURLWithPath: out) as CFURL
guard let dest = CGImageDestinationCreateWithURL(url, UTType.png.identifier as CFString, 1, nil) else { exit(1) }
CGImageDestinationAddImage(dest, image, nil)
exit(CGImageDestinationFinalize(dest) ? 0 : 1)
