# Sauce on TestFlight: proposal (round 28, item 4A)

**Status: proposal only.** Nothing has been uploaded, registered or changed on the Apple account. This waits
for Caspian's OK.

## The idea

Caspian's friends get the Sauce app from TestFlight instead of building it. The Natif Installer then only
copies each player's own game files from their Mac onto the device.

## Two problems to settle first

### 1. A TestFlight build ships the recompiled games to other people

Sauce contains the recompiled code of every game: C generated from Valve's Linux binaries, compiled into the
app. Today each person builds that on their own Mac from their own copy. That's the project's core rule
(CLAUDE.md: "the game is built on the user's machine from their own copy"), and the README says never to
share builds.

A TestFlight build is one copy, built on Caspian's Mac and handed to every tester. Even with game data still
coming from each tester's own Steam library, that breaks the rule. Internal testing skips Beta App Review,
so Apple wouldn't object, but Valve's rights don't change. Only Caspian can decide this, ideally after
reading Steam's subscriber agreement and asking upstream.

### 2. The installer can't copy games into a TestFlight copy (very likely)

Every copy goes through `xcrun devicectl device copy to --domain-type appDataContainer`. Apple's DTS says
Xcode tools get no access to the container of a TestFlight or App Store app, which has no `get-task-allow`
([forum 699831](https://developer.apple.com/forums/thread/699831)). Free-space reporting (`devicectl device
process launch` with environment variables) may not work either. Neither is verified, because that needs a
real TestFlight install.

Three ways around it, none built:

- **(a) Finder's file sharing.** Sauce already sets `UIFileSharingEnabled`, so its Documents folder is
  reachable over AFC "VendDocuments". Apple ships no scriptable tool for this; pymobiledevice3 (`apps push
  --documents`) does it but is third-party and untested here. It also works only on Documents, which is
  where Sauce keeps everything. *About 1 round.*
- **(b) Sauce fetches the files itself** over the local network from a small server in the installer. Sauce
  already advertises itself over Bonjour for Play nearby. This needs no Apple tool at all and works for any
  signing. *About 1–2 rounds.*
- **(c) Drag the folders in Finder** (iPad > Files > Sauce). This works today with no code, but it's 10+ GB of
  folders to get exactly right: not painless.

My recommendation: (b) if TestFlight goes ahead, because it also removes the cable and devicectl from the
free-account route.

## If Caspian says yes: exactly what to click

1. **Bundle ID.** developer.apple.com > Certificates, Identifiers & Profiles > Identifiers > +, App IDs, App.
   - Explicit ID, for example `io.github.xcaspians.sauce`. The current `local.portal2recomp…` IDs are
     Xcode-made development IDs.
   - Capabilities: **Increased Memory Limit** and **iCloud** (with CloudKit/iCloud Documents). Then create
     the container `iCloud.io.github.xcaspians.sauce` and tick it on the ID.
2. **App record.** appstoreconnect.apple.com > Apps > + > New App.
   - iOS, name (it must be unique on the store, so maybe "Sauce Beta"), primary language, the bundle ID
     above, and any SKU.
3. **The build (my part, after the OK).**
   - Add an App Store signing path to `tools/build_ios.py` / `ios/signing`: an Apple Distribution
     certificate and an App Store profile. Today everything is signed for development.
   - Set `ITSAppUsesNonExemptEncryption = false` in `ios/Info.plist.in`, which skips the export question.
     Sauce uses only standard networking.
   - Upload with Transporter, or `xcrun altool --upload-package`.
   - For the **Apple Distribution certificate**, Caspian clicks Xcode > Settings > Accounts > Manage
     Certificates > + > Apple Distribution once.
4. **Export compliance.** If asked, answer "No / exempt", standard encryption only.
5. **Testers.**
   - App Store Connect > Users and Access > + : invite the buddy with the **Marketing** role, the least
     access that can be an internal tester.
   - Then TestFlight > Internal Testing > + group > add them.
   - Internal builds need no review, last 90 days each, and allow up to 100 testers.
6. **The buddy** accepts the e-mail, installs TestFlight from the App Store, taps Install, opens Sauce once,
   then runs the Natif Installer for the games.

## Anything else that might bite

- **Increased Memory Limit** is fine on TestFlight with a paid team. iCloud needs the container on the App ID
  (step 1).
- **Local network** (Play nearby, phone controller) needs no entitlement, only the Info.plist keys Sauce
  already has.
- **One build, every device.** The TestFlight build would have to contain every game's code, including
  games the tester doesn't own. Today a Mac build holds only the games found on that Mac.
- **Saves:** a TestFlight Sauce has a different bundle ID from a self-built one, so it's a separate app with
  separate saves. The installer's save backup can carry them over by hand.
