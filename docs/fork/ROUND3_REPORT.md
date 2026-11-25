# Fork round 3 report: community map browser (24 November 2026)

**Status: Part A done; Part B approved and built** (merged into the fork's
`main`). The device build is ready (`build/ios/device/Portal2.app`,
`0.4.0-fork.2 (69b5236)`). Your iPhone and iPad mini were offline the whole
round, so it was checked on the Mac and in the iOS Simulator.

## Part A: can it be done?

### 1. Downloads: yes, for every chamber tried

I called Valve's public `ISteamRemoteStorage/GetPublishedFileDetails` (an
HTTP POST, **no key**) for **22 Portal 2 Workshop chambers** (app 620):
- most subscribed from 2012, top rated from 2013 to 2025, the newest from
  November 2026;
- single-player and co-op.

| Result | |
|---|---|
| Direct `file_url` returned | **22 of 22 (100 %)** |
| What the file is | always one `.bsp` (Portal 2's map format, version 21); the map's custom textures, sounds and models are packed inside it (0.7 MB to 50 MB of the file) |
| Size | 4.7 MB to 105 MB; most 5–30 MB |
| Also returned | title, author's SteamID, description, preview image URL, tags (`Singleplayer` / `Cooperative` plus puzzle elements), subscriber and favourite counts, created and **updated** dates (for the update badge) |

Four were downloaded straight from Valve's file server: a 2012 classic,
a 2021 one with 50 MB of packed content, a brand-new single-player one, and
a co-op one.
- All came back as HTTP 200, in 0.2 s to 2.2 s.
- All are valid map files, and the sizes match the API's.

**Loaded in the Mac build** with `+map workshop/<id>/<name>`. The game's
own console log confirmed `Host_NewGame on map workshop/…` for three maps
(2012, 2021 packed, 2026), with no crash after 80–100 s each. A missing
map name, tried as a control, logged "map load failed".

Notes for the browser:
- One item in the "most subscribed" list was a **collection** (its file is
  a banner image, not a map). Collections need skipping, or opening as a
  list of maps.
- **Co-op maps** need two players: on iOS that means LAN co-op with a Mac
  host. They should show with a "co-op" label, and the list should default
  to single-player.
- Batching several IDs into one request failed (HTTP 400) in my tests,
  while one ID per request always worked. I'll look at the batch format
  again in Part B; one request per page of results is enough anyway.

### 2. Browsing: the official API needs a key

- **`IPublishedFileService/QueryFiles` without a key: refused** (HTTP 403,
  "verify your key= parameter").
- **With a key**, the documented options for app 620 are:
  - sorts: most voted (`RankedByVote`), newest (`RankedByPublicationDate`),
    trending over 1–7 days (`RankedByTrend`), most subscribed
    (`RankedByTotalUniqueSubscriptions`), and search (`RankedByTextSearch` +
    `search_text`);
  - single-player / co-op through `requiredtags` (`Singleplayer`,
    `Cooperative`);
  - paging with a `cursor`;
  - previews, short descriptions and tags in the same answer.
- The page doesn't mark QueryFiles as publisher-only, unlike some other
  methods on it. So a normal user key should work, but **that's untested
  until we have a key.**
- **Keyless way to list:** the public Workshop web page
  (`steamcommunity.com/workshop/browse/?appid=620&browsesort=…`) lists
  item IDs for trending, most subscribed, top rated and newest, plus search
  text. Reading IDs out of it worked today. It's **fragile**: it's a web page,
  not an API, so a page redesign or Valve's rate limiting can break it
  without warning.

### 3. Rules

- **CLAUDE.md ("no Steam emulator, no faking ownership or interfaces"): fine.**
  - The browser would call Valve's public web API over HTTPS, as any website
    or tool does.
  - Nothing pretends to be Steam, nothing claims ownership, and no Steam
    interface is faked.
  - Workshop chambers are free community content that their authors
    published for Portal 2 players, and the player owns Portal 2.
- **Steam Web API Terms of Use** (steamcommunity.com/dev/apiterms):
  - A key is "personal to you and specific to your Application", must be
    kept confidential, and may not be shared or transferred.
  - So **a key may never ship inside the app.** Each player pastes their own
    free key, stored only in their device's Keychain and sent only to
    `api.steampowered.com`.
  - For you, using your own key on your own devices, that fits the terms. If
    the app is ever given to someone else, they need their own key.
  - Limit: 100,000 calls a day, far above what a player uses.
  - The app must not look endorsed by or affiliated with Valve. Show the map
    authors, and don't use Valve's logos.
  - The terms don't mention Workshop files specifically.

### 4. Fallback: the Mac route, which also already works

**While testing, the Mac build downloaded your subscribed chambers by
itself**:
- The game's own Workshop code runs through real Steam on the Mac.
- `workshop_log.txt` shows it querying your subscriptions and downloading
  them.
- 25 maps, 809 MB, are now in the app's folder
  (`~/Library/Application Support/Portal 2 Natif/userdata/portal2/maps/workshop`),
  not in your Steam install.

So subscribing on the Mac (in Steam or in the game's Community Test
Chambers) already brings maps to the Mac. Getting them to the phone needs
one small tool, either over the cable or through round 2's iCloud sync.
It isn't needed as the main route, since direct downloads worked for 100 %
of the sample, but it's a free extra.

### Recommendation

- **Downloads and details: keyless**, through `GetPublishedFileDetails`.
  - Maps go to `Documents/game/portal2/maps/workshop/<id>/<file>.bsp`. That's
    the layout the engine uses and that round 2's Maps & Mods list already
    reads, so they show up there and start with `map workshop/<id>/<name>`.
  - The update badge compares the Workshop's "updated" date with the saved
    copy's.
- **Browsing: with Jacksen's own free Web API key** (official, documented,
  stable):
  - Popular, Newest, Trending and Search, plus single-player / co-op.
  - The key goes in the menu, stored in the Keychain, with a "how to get a
    free key" note and a link.
  - A missing or wrong key gets a clear message.
- **Without a key it still works partly:**
  - a "Paste a Workshop link or ID" box downloads any chamber;
  - the Downloaded filter manages what's already on the device.
- **Not recommended:** reading the Workshop web page as the main browser.
  It works today but can break silently. It could be an opt-in "no key"
  mode if you want it.
- **Effort for Part B:** about 2–3 days, plus device testing.

## Part B: what shipped

**Community Maps**, inside the in-game menu (pause > gear > Maps & Mods >
Community Maps), on the route you approved:

- **Browse** with your own free Steam Web API key:
  - Popular (most subscribed), Newest, Trending (this week) and Search;
  - **single player by default**, or co-op;
  - 20 at a time, with more loading as you scroll;
  - each card shows the preview, title, author, subscribers and size, plus
    badges: **Co-op**, **Downloaded** and **Update available**.
- **The key:**
  - Paste it in the page; it's kept in the device's Keychain only, and
    "API key saved · Change" removes it.
  - Without a key, the page explains how to get a free one (with a link)
    instead of showing an empty list.
  - A refused key gets its own message.
- **Without a key:** "Paste a Workshop link or ID" works, and a pasted
  collection opens as the list of its maps. Collections never appear in the
  browse lists.
- **Details:**
  - larger preview, description, size and update date;
  - **Download** with progress and **Cancel**, then **Play**, **Update**
    and **Delete** (press twice);
  - co-op maps say they need a second player, and Play is off for them.
- **Downloaded tab:** the maps on the device, checked against the Workshop
  for the update badge.
- **Where maps go:**
  `Documents/portal2-userdata/portal2/maps/workshop/<id>/<name>.bsp`.
  - The engine reads that folder first, and the Maps & Mods list shows the
    maps too.
  - It's kept out of iCloud backups.
  - A download is written as `.part` and becomes a map only once its size
    matches and it's a real map file. A cancel, a failure or a crash leaves
    nothing broken in the list.
- **Failures:** each gets a plain sentence:
  - offline;
  - Steam not answering;
  - a refused key;
  - Steam limiting requests;
  - no download link;
  - not enough storage;
  - an unreadable answer;
  - no results.
- **Controller:**
  - d-pad or stick moves;
  - left/right on the tabs switches tab, and on the mode row switches
    single player / co-op;
  - A opens or presses;
  - B goes back.
  - Typing (search, key, link) needs touch.
- **No web-page scraping mode**, as agreed.

**Checks:** 8 new checks, 286 in all, plus 236 Python tests, all passing.
They cover:
- reading the API's answers: details, browse pages, collections, names,
  plus broken and empty answers, escapes and emoji;
- the browse URL and request bodies, and pasted links;
- file names and paths;
- the update badge;
- cleaning up canceled and half-finished downloads.

The device and simulator apps compile with no warnings.

**Simulator:**
- A chamber (`3845455194`, 4.8 MB) was downloaded without a key and saved
  with its note. It started from the menu: the game's console log shows
  `Host_NewGame on map workshop/3845455194/1795496734`.
- A second run started it again from the saved copy.
- The details page of "12 Angry Tests: Part 2" drew with its preview,
  subscribers, size, date and the Download button.
- Browsing with a key couldn't be tried: there's no key yet.

## Needs Jacksen (for Part A)

1. **OK on the route above** (or pick: key-based browsing, page-reading
   without a key, or both).
2. **A free Steam Web API key**, if you agree: steamcommunity.com/dev/apikey,
   any domain name (for example `localhost`). Paste it into the app's key
   field (not into a chat).
3. **The 809 MB of Workshop maps** the Mac game downloaded on its own: keep
   them (they can go to the phone later), or delete
   `~/Library/Application Support/Portal 2 Natif/userdata/portal2/maps/workshop`
   to free the space. The game downloads them again whenever it starts with
   Steam, as on PC.

## Needs Jacksen (everything, batched)

1. **Plug in and unlock your iPhone and iPad mini.** I install this build on
   both; your game data stays.
2. **Get your free Steam Web API key** (steamcommunity.com/dev/apikey) and
   paste it in Community Maps on each device. This is the first real test
   of browsing with a key.
3. **The hand tests below.**
4. **The 809 MB of Workshop maps** the Mac game downloaded on its own
   (Part A): keep them or delete them.
5. **Left over from rounds 1 and 2:**
   - the controller test of the overlay fix;
   - a back-tap recording with light taps;
   - the 120 Hz measurement;
   - the `portal2_dlc3` addon test;
   - the one-time iCloud setup (`docs/ios.md`, "iCloud saves");
   - the OK on the upstream PR (E).

## Hand-test checklist (iPhone and iPad mini)

- [ ] Pause > gear > Maps & Mods > **Community Maps**. Without a key, the
  page explains how to get one, and "Get a free key" opens the Steam page.
- [ ] Paste the key and Save: **Popular** fills with single-player
  chambers, with previews, authors and subscriber counts. Scroll down: more
  load.
- [ ] **Newest** and **Trending** fill too. **Co-op** shows co-op maps,
  each labelled "Co-op".
- [ ] **Search** for a chamber by name (for example "12 Angry Tests"): it's
  found.
- [ ] Open a map: description, larger preview, size, update date.
- [ ] **Download** it: the bar fills. Start another one and press
  **Cancel**: nothing broken shows up in Downloaded or in Maps & Mods.
- [ ] **Play** the downloaded map: the menu closes and the chamber loads.
- [ ] It's in **Downloaded** and in **Maps & Mods**.
- [ ] **Delete** it (press twice): it's gone from both lists.
- [ ] Airplane mode, then Community Maps: "You're offline…", not an empty
  screen. Downloaded maps still play.
- [ ] Without a key (Change), paste a Workshop link (for example
  `https://steamcommunity.com/sharedfiles/filedetails/?id=68539885`): its
  details open and it downloads. Paste a collection's link: its maps are
  listed.
- [ ] A wrong key: "Steam refused the API key…".
- [ ] Controller:
  - left/right on the tabs switches tab, and on the mode row switches
    single player / co-op;
  - up/down moves through the cards;
  - A opens a card and presses Download or Play;
  - B goes back to the list, then to the settings.
