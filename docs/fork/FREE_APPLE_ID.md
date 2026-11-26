# Sauce with a free Apple ID (round 28, item 4B)

What someone with no paid developer account gets when the Natif Installer builds Sauce on their Mac and
signs it with their own Apple ID.

## How it works

- **The installer never sees the Apple ID password.** The user signs in once in **Xcode > Settings >
  Accounts**, which is Apple's own window. `setup.sh --ios` then runs `xcodebuild -allowProvisioningUpdates
  -allowProvisioningDeviceRegistration`, which uses Xcode's stored sign-in to make the profile. App Store
  Connect API keys aren't an option: free accounts can't get them.
- **Needs full Xcode** (Mac App Store, free), not just the command line tools: xcodebuild and devicectl ship
  with Xcode.
- **Needs an Apple silicon Mac.** setup.sh builds the Mac side first, and the Mac games are arm64.
- **On the device: Developer Mode** (Settings > Privacy & Security), and after the first install,
  Settings > General > VPN & Device Management > trust the Apple ID.

## Limits (Apple's [membership comparison](https://developer.apple.com/support/compare-memberships/))

| Limit | What it means for Sauce |
|---|---|
| Profiles last **7 days** | Sauce stops opening after 7 days. The installer shows the days left, reminds the user the day before, and **Refresh Sauce** re-signs and reinstalls in one click. The bundle ID doesn't change, so saves and games stay. |
| **3 apps** per device | Sauce is one app since round 19, so it uses one slot. |
| **10 App IDs** per 7 days | setup.sh registers one per install (Sauce). Not a problem. |
| **3 devices**, re-registered every 7 days | An iPad and an iPhone fit. |

## Capabilities

| Sauce uses | Free account | Effect |
|---|---|---|
| **iCloud Documents** (saves, button layouts) | **No** (paid only) | Off unless `ios_icloud` is set, which it isn't by default. Layouts stay on the device; the editor already says so. Nothing breaks. |
| **Increased Memory Limit** | **Unclear.** Apple's table doesn't list it; upstream's signing project (`ios/signing/project.yml`) notes that only Extended Virtual Addressing was refused to a free team. | If Apple refuses it, the profile step fails, so the whole install fails. See the workaround below. |
| **Extended Virtual Addressing** | No | Not used (already left out for this reason). |
| **Local network / Bonjour** (Play nearby, phone controller) | Yes, no entitlement needed | Works. |
| **Game controllers, TV output, back taps** | Yes, not signed capabilities | Works. |

## The one possible big break, and the proposed workaround (not built)

If a free team can't get Increased Memory Limit, `xcodebuild` refuses the profile and Install stops with
"Xcode couldn't sign Sauce with your Apple ID".

- **Proposed:** when the profile step fails on that entitlement, retry once with a signing project that leaves
  it out (`project-free.yml`), and warn: "Without the higher memory limit, big maps (Half-Life 2, Left 4 Dead
  2) may close on iPads with less memory."
- iPads with 8 GB or more (M-series) probably still run Portal 2 fine, but that's untested.
- **Settling it takes one install with a free Apple ID.** That's part of the newcomer run, or Caspian can try
  with a second, free Apple ID.
