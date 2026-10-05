# Progress log

The newest entry comes first.

## 2026-10-05: 5180 functions match

```
matched 5180 of 11321 game functions (510156 of 2785198 bytes, 18.32%)
```

**The first two Codex lanes:** 11 new matches, none lost. Codex wrote the code; a Claude agent checked and committed it.
- **Lane J (network handle tables):** 0x995c0, 0x99690 and their callers. This also completed two of lane D's functions.
- **Lane L (preference getters and font loading):** 0x121100, 0x1210a0, 0x1223a0, and the UI caller 0x2bba01.

## 2026-10-05: 5169 functions match

```
matched 5169 of 11321 game functions (508526 of 2785198 bytes, 18.26%)
```

42 new matches, none lost:
- **Lane D**, round 12: network voice, session and observer code.
- **Lane H**, round 8: input, widgets and the game variant code. 0x19a76d now matches, which finally settles its calling convention.
- **The first Codex-assisted commit:** four near misses fixed (0x1b7900, 0x1bba20, 0x223b20 and 0x248d50).

## 2026-10-05: 5127 functions match

```
matched 5127 of 11321 game functions (503104 of 2785198 bytes, 18.06%)
```

**Contributor pull requests**, merged after independent review:
- @Banshee64: bipeds (#28), Euler-vector helpers (#38), vehicle SSE tuning (#40) and the cached language getter (#42);
- @coldspear: a decomp.dev objdiff export (#30), `tools/near.py` (#32) and follow-ups to earlier tooling (#41);
- @BirchWoodGod: an analysis of the network bandwidth controller (#44).

**Lane S**, round 4: weapons, devices, scenery and items.

Together: 29 new matches, none lost. Leftover include guards from the provenance renames now follow their file names.

## 2026-10-05: 5098 functions match

```
matched 5098 of 11321 game functions (499631 of 2785198 bytes, 17.94%)
```

**Lanes C (round 12) and A (round 13)**, merged together: 22 new matches, none
lost. They include render model marker lookups, actor masks, object-tree
resource prediction and script evaluators.

## 2026-10-04: 5076 functions match

```
matched 5076 of 11321 game functions (497600 of 2785198 bytes, 17.87%)
```

**The UI lane**, round 14: 14 new matches, none lost. They include game engine
handlers, playlist lists and clan member data.

## 2026-10-04: provenance clean-up, part 2

Identifiers and file names that matched non-permitted sources exactly are
renamed: 703 identifiers, mostly to address-based placeholders such as
`function_<va>` and `field_<offset>`, with descriptive names for the most
used ones (`point3f`, `string_handle`, `s_record_pool`), and 120 source files
to `unknown_<va>`. Generic idioms and single common words are kept.
[PROVENANCE.md](../PROVENANCE.md) describes the method. The match count is
unchanged at 5062, and no function changed status.

## 2026-10-04: provenance clean-up, part 1

The `name` and `object` columns of `config/functions.csv`, which came from a
third-party dataset, are now empty. Library functions are named instead by
the project's own byte-signature matching against the contributor's SDK
libraries. The tools no longer read that dataset. Comments and documents that
cited unreleased builds as a source have been revised. The match count is
unchanged at 5062. The next step renames identifiers whose names came from
those sources; see [PROVENANCE.md](../PROVENANCE.md).

## 2026-10-04: 5062 functions match; legal notice and provenance policy

```
matched 5062 of 11321 game functions (496330 of 2785198 bytes, 17.82%)
```

**Project policy.**
- New [LEGAL.md](../LEGAL.md) and [PROVENANCE.md](../PROVENANCE.md) set out the
  project's scope: it is independent, non-commercial preservation and
  research, it does not contain game files, executables or SDK files, and
  users supply their own lawfully owned copy.
- They also state the contribution rule: contributions must not contain or
  copy material from leaked or internal sources.
- Pull requests now include a provenance checklist.
- Name data that came from a third-party dataset derived from unreleased
  builds' linker maps is being removed, as PROVENANCE.md describes.

**Progress.** 58 new matches, none lost:
- **Lane L**, round 7, and **the UI lane**, round 13 (the dashboard dialog
  callbacks and more);
- **a build fix:** the compiler's link-time inliner changed its decisions as
  the program grew. A discarded "ballast" object now keeps them stable, so
  adding code no longer costs matches elsewhere.

**Tooling from @coldspear:**
- safer disc-image extraction and XBE parsing;
- jump tables shown as data in the disassembler;
- `ready.py --claims` to skip claimed ranges;
- fixes to the LTCG probe and the permuter.

## 2026-10-04: 5004 functions match

```
matched 5004 of 11321 game functions (492273 of 2785198 bytes, 17.67%)
```

**Lane C**, round 11: 4 new matches, none lost. They include sector geometry
for pathfinding and animation helpers.

## 2026-10-04: 5000 functions match

```
matched 5000 of 11321 game functions (491662 of 2785198 bytes, 17.65%)
```

Five thousand of the game's functions now compile to exactly retail's bytes.
This batch added 20 new matches and lost none:
- **Lane D**, round 11: network connections, the player configuration cache
  and online game invites. Lanes J, M and the UI lane gain functions as a
  result.
- **Lane I**, round 4: AI props and firing positions.
- **A research finding.** VC7.1's link-time code generation tracks which
  registers each out-of-line callee really writes. Retail's math helpers were
  `inline`, so callers assume the standard registers are clobbered. Marking
  one helper `inline` matched two more functions.

## 2026-10-04: 4980 functions match; 17.5%

```
matched 4980 of 11321 game functions (487361 of 2785198 bytes, 17.50%)
```

**The UI lane**, round 12: 40 new matches, none lost. They are the virtual
keyboard screen (31 functions) and the main menu music.

## 2026-10-04: 4940 functions match

```
matched 4940 of 11321 game functions (483661 of 2785198 bytes, 17.37%)
```

**@Banshee64**: `path.cpp` (#26), with all 18 path search routines written
and 4 new matches, and the `unknown_0259d0` rectangle helpers (#29).

## 2026-10-04: 4935 functions match

```
matched 4935 of 11321 game functions (482890 of 2785198 bytes, 17.34%)
```

**Lane J**, round 7: 11 new matches, none lost. They are network message
handlers, link routes and transport endpoint options.

## 2026-10-04: 4924 functions match

```
matched 4924 of 11321 game functions (482034 of 2785198 bytes, 17.31%)
```

**Lane B**, round 8: 7 new matches, none lost. They are unit helpers, actor
slot handlers and path code.

## 2026-10-04: 4917 functions match

```
matched 4917 of 11321 game functions (480513 of 2785198 bytes, 17.25%)
```

**Lane M**, round 5: 9 new matches, none lost.
- They include friends-list functions, player-profile reads and AI action
  nodes.
- The friends globals are one structure, as in retail. Its address is
  taken, so fields reload after calls the way retail's do.
- Two UI functions match as a result.

## 2026-10-04: 4908 functions match

```
matched 4908 of 11321 game functions (478795 of 2785198 bytes, 17.19%)
```

**Lane T**, round 4: 17 new matches, none lost. It wrote the rest of the
first-person weapons file, models, scoreboard rows, the loading screen,
camera scripting and the Windows cache-file helpers. It also replaced many
stubs that other lanes had been calling, so two of lane A's script functions
now match.

## 2026-10-04: 4891 functions match; past 17%

```
matched 4891 of 11321 game functions (476869 of 2785198 bytes, 17.12%)
```

**The UI lane**, round 11: 43 new matches, none lost.
- The postgame statistics screen family is written: the screen, six tabs and
  their lists.
- A long-standing puzzle is solved: retail's zero-extended flag-bit test
  comes from `(bool)(((dword)flags >> bit) & 1)` on a short flags word, not
  from a bitfield. Six UI functions match with it, and the same shape turns
  up in other lanes' code.

## 2026-10-04: 4848 functions match

```
matched 4848 of 11321 game functions (473081 of 2785198 bytes, 16.99%)
```

**Lane D**, round 10: 22 new matches, none lost. They include online mute
lists, the online message block chain, voice mail and voice observers. Four
UI functions match as a result.

## 2026-10-04: 4826 functions match

```
matched 4826 of 11321 game functions (469815 of 2785198 bytes, 16.87%)
```

**Lane A**, round 12: 26 new matches, none lost. Every script built-in
function in `0x2a0000`–`0x2affff` is now written. New matches include the
object list helpers and their callers, more evaluators, and AI-script
helpers.

## 2026-10-04: 4800 functions match

```
matched 4800 of 11321 game functions (467763 of 2785198 bytes, 16.79%)
```

**Lane P**, round 4: 29 new matches, none lost.
- `ascii_string_to_unicode` takes its arguments in a different order,
  copied into locals, so six UI functions that call it now match.
- New in the region: the interface game system, new-HUD and visibility pool
  functions, game options and scenario fog.

## 2026-10-04: 4771 functions match

```
matched 4771 of 11321 game functions (464312 of 2785198 bytes, 16.67%)
```

**Lane F**, round 5: 19 new matches, none lost. They include player control
entries rebuilt in a new shape, sound cluster lookups and a batch
helper. Lanes N and A each gain one function as a result.

## 2026-10-04: 4752 functions match

```
matched 4752 of 11321 game functions (461659 of 2785198 bytes, 16.58%)
```

**Lane C**, round 10: 7 more animation samplers match (37 of 60), none lost.
Each sampler shares or copies its loop variables across the three passes in
its own pattern, and a generic loop now takes them by reference.

## 2026-10-04: 4745 functions match

```
matched 4745 of 11321 game functions (454928 of 2785198 bytes, 16.33%)
```

**The UI lane**, round 10: 33 new matches, none lost. They are the Xbox Live
message display screen and list, and six postgame statistics screens.

## 2026-10-04: 4712 functions match

```
matched 4712 of 11321 game functions (451105 of 2785198 bytes, 16.20%)
```

**Lane S**, round 3: 13 new matches, none lost. They include weapons, devices
and items code, and a lane A script function. Several functions that retail
calls out of line now live in their own `/Ob1` files.

## 2026-10-04: 4699 functions match

```
matched 4699 of 11321 game functions (449650 of 2785198 bytes, 16.14%)
```

**Lane N**, round 4: 13 new matches, none lost. They are the font cache,
player speed requests and window manager clan tasks, plus one of lane A's
script functions that matches once lane N's callee passes its reals in
retail's registers.

## 2026-10-04: 4686 functions match; past 16%

```
matched 4686 of 11321 game functions (447822 of 2785198 bytes, 16.08%)
```

**Lane J**, round 6: 19 new matches, none lost. They are the network link's
packet decode, send and receive loop, and the whole winsock transport
endpoint.

## 2026-10-04: 4667 functions match; almost 16% of the code

```
matched 4667 of 11321 game functions (444572 of 2785198 bytes, 15.96%)
```

**Lane C**, round 9: 20 new matches, none lost. Half of the 60 animation
samplers now match. These are large functions, so the matched share of the
code rose by almost a whole point. The fix was a missing root-z offset
statement and one shared sampling-state structure.

## 2026-10-04: 4647 functions match

```
matched 4647 of 11321 game functions (421089 of 2785198 bytes, 15.12%)
```

**Lane A**, round 11: 25 new matches, none lost. They are script built-in
evaluators and helpers. `scenario_location_from_point` (0x11bed0) now takes
its arguments in Halo CE's order, which matches two more callers.

## 2026-10-04: 4622 functions match; past 15%

```
matched 4622 of 11321 game functions (418535 of 2785198 bytes, 15.03%)
```

**The UI lane**, round 9: 71 new matches, none lost. They include the voice
message record screen, the online Y menu's player selected list, five more clan
tasks, and the window channel methods. Retail built those methods without
link-time code generation, so they live in a `/GL-` file.

## 2026-10-04: 4551 functions match

```
matched 4551 of 11321 game functions (412721 of 2785198 bytes, 14.82%)
```

**@Banshee64**: `vehicles.cpp` (#27). All 52 functions of the vehicle object
type are written; 2 match so far.

## 2026-10-04: 4549 functions match

```
matched 4549 of 11321 game functions (412575 of 2785198 bytes, 14.81%)
```

**Lane K**, round 5: 23 new matches, none lost. They include the rest of the
DirectSound stream code, input abstraction, the sound class lookup, sound
tracks, the attract mode and marketing screens' helpers, and S3TC texture
helpers.

## 2026-10-04: 4526 functions match

```
matched 4526 of 11321 game functions (409673 of 2785198 bytes, 14.71%)
```

**Lane H**, round 7: 8 new matches beyond those lane D's round 9 already
brought, none lost. They include HUD messaging, the multiplayer game variant
menu and a dialog helper the UI lane needed. Lane H's game variant type is
folded into the UI lane's.

## 2026-10-04: 4518 functions match

```
matched 4518 of 11321 game functions (408082 of 2785198 bytes, 14.65%)
```

**Lane D**, round 9: 26 new matches, none lost. They include voice port
checks, network session and observer code, and online Bungie.net user
functions. Eight of the UI, H and J lanes' functions match as a result.

## 2026-10-04: 4492 functions match

```
matched 4492 of 11321 game functions (405232 of 2785198 bytes, 14.55%)
```

**Lane L**, round 6: 14 new matches, none lost.
- The sound manager's table lookups now have retail's conventions.
- The main loop's byte globals turned out to be one structure, `main_globals`.
- A sound stream flush that retail calls out of line now sits in its own
  `/Ob1` file.
- Three of lane A's script functions match as a result.

## 2026-10-04: 4478 functions match; past 14%

```
matched 4478 of 11321 game functions (402586 of 2785198 bytes, 14.45%)
```

27 new matches, none lost:
- **Lane K**, round 4: the sound cache's `0x218850`, the first use of the
  `standard` marker; a dispose helper that retail calls out of line, now in its
  own `/Ob1` file; and with lane A's fixes, two DirectSound stream functions.
- **Lane C**, round 8: all 60 animation samplers written from one shared
  inline body (14 match so far), and three animation-graph functions.

## 2026-10-04: 4451 functions match

```
matched 4451 of 11321 game functions (389205 of 2785198 bytes, 13.97%)
```

**The UI lane**, round 8: 47 new matches, none lost. They include the player
selected list, Havok's fixed memory allocator and hkMemory, the window
manager's update, and the clan (online team) tasks.

## 2026-10-04: 4404 functions match

```
matched 4404 of 11321 game functions (385576 of 2785198 bytes, 13.84%)
```

**Lane A**, round 10: 17 new matches, none lost. They are script built-in
evaluators and the sound stream class's conventions, which unblock lane K's
DirectSound code.

## 2026-10-04: 4387 functions match

```
matched 4387 of 11321 game functions (384213 of 2785198 bytes, 13.79%)
```

54 new matches, none lost:
- **@Banshee64**: `projectiles.cpp` (#15, 11 match), `unknown_207b90.cpp` (#19),
  `unknown_0a76b0.cpp` (#23, 8 of 100 written match), crates (#24) and
  the unknown_11c920 string comparator (#25).
- **Lane K**, round 3: the DirectSound driver layer and the sound voice pool.
- **Lane B**, round 7: ten actor getters and lane A's `0x11ade0`, now that
  the seat lookup has retail's convention.
- **Lane J**, round 5: network link sends and the message gateway flush.

Tools: a `standard` marker, a last resort for functions that retail keeps in
the stack convention although nothing takes their address (see
docs/DECOMPILING.md).

## 2026-10-03: 4333 functions match

```
matched 4333 of 11321 game functions (377820 of 2785198 bytes, 13.57%)
```

**@Banshee64**: `garbage.cpp` (#20, written, near), `unknown_0b9fc0.cpp`
(#21, 1 of 7 match) and `unknown_11bdc0.cpp` (#22, 1 of 1).

## 2026-10-03: 4331 functions match

```
matched 4331 of 11321 game functions (377541 of 2785198 bytes, 13.56%)
```

**Lane D**, round 8: 20 new matches, none lost. They are the simulation
entity table's virtual methods, a view buffer release that keeps its size
argument on the stack because the function takes its address, voice port
flags, and the online message of the day.

## 2026-10-03: 4311 functions match; six more contributions from @Banshee64

```
matched 4311 of 11321 game functions (375948 of 2785198 bytes, 13.50%)
```

83 new matches, none lost:
- **@Banshee64**, six files:
  - `unknown_0b7300.cpp` (#12), 10 of 20 match;
  - `unknown_050650.cpp` (#13), 3 of 17;
  - `unknown_29ed40.cpp` (#14) and its legacy v1 format (#16),
    9 of 13 and 11 of 13;
  - `equipment.cpp` (#17), 2 of 3;
  - `unknown_11b980.cpp` (#18), 3 of 4.
- **The UI lane**, round 7: squad settings, sending Xbox Live messages, the
  multiple-choice dialog, variant parameter editing, clan and friends
  handlers.

## 2026-10-03: 4228 functions match

```
matched 4228 of 11321 game functions (368996 of 2785198 bytes, 13.25%)
```

**Lane N**, round 3: 10 new matches, none lost. They include player and
structure functions, and a UI function that matches now that its callee is
real. `function_1420f0` now takes its arguments in the debug
build's order, which gives it retail's register convention.

## 2026-10-03: 4218 functions match

```
matched 4218 of 11321 game functions (367773 of 2785198 bytes, 13.20%)
```

**Lane L**, round 5: 28 new matches, none lost. It fixed the conventions of
the datum-array unlink helper (`0x13d830`) and of `0x12d520`, which also
matched about ten simulation view functions in lane D's area. A texture cache
update now gets its own `/Ob1` file, so retail's out-of-line calls to it
match, and there is more sound manager code.

## 2026-10-03: 4190 functions match; function discovery fixes

```
matched 4190 of 11321 game functions (363435 of 2785198 bytes, 13.05%)
```

Function discovery (`tools/functions.py`, `tools/inventory.py`) now handles:
- tail jumps to the next function;
- `int3` bytes inside a looping function's own instructions;
- guessed starts that land inside an instruction;
- callbacks passed as pushed immediates;
- code reached only by a jump past the next function's start;
- library signatures that matched small game functions by chance.

The inventory gained 146 rows, mostly library code that no row covered
before. In game code, 19 rows were added and 20 removed, and 3 functions now
match because their extents are right. The checker also tells apart overloads
with pointer and class parameters.

## 2026-10-03: 4185 functions match; past 13%

```
matched 4185 of 11317 game functions (363298 of 2783395 bytes, 13.05%)
```

**The UI lane**, round 6: 69 new matches, none lost.
`c_class_1473c9::build` now has retail's register convention. The fix was
reading the pane array into a local as soon as the count is known. Ten screen
functions that call it match with it. Also new: list item handlers for the
pause, main menu, handicap, difficulty and team screens, the gamertag select
screen, and controller sign-in. The checker now tells overloaded constructors
apart by their parameter types.

## 2026-10-03: 4116 functions match

```
matched 4116 of 11317 game functions (357078 of 2783395 bytes, 12.83%)
```

**Lane A**, round 9: 34 new matches, none lost. They are script built-in
functions and the AI-script, flock and unit helpers behind them.

## 2026-10-03: 4082 functions match

```
matched 4082 of 11317 game functions (353593 of 2783395 bytes, 12.70%)
```

**Lane C**, round 7: 19 new matches, none lost. They are the animation
sampler dispatcher and its ten sub-dispatchers, plus animation graph lookups
that retail inlines. A sorted-array helper that retail passes everything on
the stack to now matches when it is built without link-time code generation.

## 2026-10-03: 4063 functions match

```
matched 4063 of 11317 game functions (351907 of 2783395 bytes, 12.64%)
```

**Lane D**, round 7: 16 new matches in the simulation world, view, players
and watcher, the network observer and network sessions; none lost.

## 2026-10-03: 4047 functions match; past 4000

```
matched 4047 of 11317 game functions (349798 of 2783395 bytes, 12.57%)
```

**The UI lane**, rounds 3 to 5: 233 new matches and none lost. They include
the text parser, the virtual keyboard, the online Y menu (friends, players and
recent players lists), the press-start and four-way sign-in screens, campaign
level select, playlists, the squad browser, clan and Live message screens, and
the screen, button, bitmap and model widget classes.

## 2026-10-03: 3814 functions match; past 12%

```
matched 3814 of 11317 game functions (335076 of 2783395 bytes, 12.04%)
```

**Lane H**, rounds 5 and 6 (`0x190000`): error dialogs, input and HUD code,
78 new matches and none lost. It found that some retail functions were built
without link-time code generation: building `player_slot_get` with `/GL-` in
its own file matched it and 13 of its callers.

## 2026-10-03: 3736 functions match; the backlog of lane rounds is merged

```
matched 3736 of 11317 game functions (325759 of 2783395 bytes, 11.70%)
```

116 new matches, none lost:
- **@Banshee64**: the updated `damage.cpp` (#8, #10), now calling the
  shared functions in place of its stubs;
- **lane C**, round 6: animation codecs, sampling and AI;
- **lane D**, round 6: simulation world, view players and the transport
  layer;
- **lane Q**, round 2 (`0x150000`): the object deletion callbacks and more;
- **lane A**, rounds 7 and 8: AI-script and script built-ins, flocks and unit
  helpers.

## 2026-10-03: 3620 functions match; damage and looping sounds

```
matched 3620 of 11317 game functions (310801 of 2783395 bytes, 11.17%)
```

113 new matches, none lost:
- **@Banshee64**: all of `damage.cpp` written (#8, #10; 9 match so far)
  and `unknown_12a1b0.cpp` (#11; 24 of 44 match);
- **lane L**, round 3 (`0x120000`): physical memory, game state globals and
  the texture cache;
- **lane F**, round 4: sound records and effects, now built on
  @Banshee64's looping sound controller;
- **lane P**, round 2 (`0x130000`): string drawing, interpolators and lists;
- **lane T**, round 2 (`0x160000`).

## 2026-10-03: 3507 functions match; past 10%

```
matched 3507 of 11317 game functions (294394 of 2783395 bytes, 10.58%)
```

One batch merge of seven lane rounds, 103 new matches and none lost:
- **lane B**, round 6: actor slot-handler callbacks;
- **lane S**, round 2 (`0x100000`): weapons, devices, scenery and items;
- **lane R**, round 2 (`0x170000`): effects and particle systems;
- **lane N**, round 2 (`0x140000`);
- **lane O**, round 2 (`0x240000`);
- **lane M**, round 3 (`0x1a0000`);
- **lane J**, round 4: network code and five unit event handlers that
  match now that the shared `0xe6900` helper is on main.

## 2026-10-03: 3404 functions match; nearly 10%

```
matched 3404 of 11317 game functions (274476 of 2783395 bytes, 9.86%)
```

One batch merge of eight lane rounds, 343 new matches and none lost:
- **lane P**, round 1 (`0x130000`): sorting, string drawing and the tag
  function evaluator;
- **lane T**, round 1 (`0x160000`): first-person weapons;
- **lane A**, round 6: script built-ins and the shared `0xe6900` helper that
  68 callers use;
- **lane C**, round 5: animation graphs and AI;
- **lane Q**, round 1 (`0x150000`);
- **lane K**, round 2: impacts;
- **UI lane**, round 2: about 170 screens, lists and their destructors;
- **lane I**, round 3: props and command scripts.

The work now runs as three agents at a time instead of a dozen or more:
one merges, two decompile.

## 2026-10-03: 3061 functions match; past 3000

```
matched 3061 of 11317 game functions (247847 of 2783395 bytes, 8.90%)
```

- **lane S**, round 1 (`0x100000`): weapons, devices, scenery and items;
- **lane R**, round 1 (`0x170000`): effects, particle systems, contrails and
  decals;
- **lane L**, round 2: the async job queue's last pieces, physical memory and
  the texture cache;
- **lane M**, round 2: the friends list and online-task screens;
- **lane H**, round 4: game engine events, and stack conventions for two
  widely used helpers.

## 2026-10-03: 2883 functions match; past 8%

```
matched 2883 of 11317 game functions (229262 of 2783395 bytes, 8.24%)
```

- **lane O**, round 1 (`0x240000`): the CTF/assault game engine and HUD
  messages;
- **lane D**, round 5: mute lists, online presence, friends, messages and
  matchmaking;
- **lane N**, round 1 (`0x140000`): the font cache and players code;
- **lane J**, round 2: the network message handlers and gateway, and the
  sequence windows.

## 2026-10-03: 2765 functions match

```
matched 2765 of 11317 game functions (217777 of 2783395 bytes, 7.82%)
```

- **the UI lane**, round 1: 114 screen, list and widget functions, the window
  manager as one object, and a new marker form for a class's implicit,
  non-deleting destructor (see docs/DECOMPILING.md);
- **lane B**, round 5: `transform4x3f_apply_point`. Retail keeps its matrix
  argument on the stack because the body takes the parameter's address; that
  one finding matched it and five functions built on it;
- **lane C**, round 4: the Havok component functions other lanes were waiting
  on;
- **lane I**, round 2: command scripts and props.

## 2026-10-03: 2604 functions match

```
matched 2604 of 11317 game functions (205329 of 2783395 bytes, 7.38%)
```

- **lane J**, round 1 (`0x090000`): network links and streams, team
  balancing, game engine entity definitions and online session search;
- **lane H**, round 3: the level list functions, the player iterators and the
  rest of the Juggernaut engine;
- **lane A**, round 5: the AI-script index register (the cause was in the
  function's own body), more script evaluators and localized text lookups;
- **lane M**, round 1 (`0x1a0000`): the motion sensor and actor behaviour
  handlers.

Two source patterns that unlocked many functions this round, now in our
notes for every lane: a getter that returns `valid ? p : NULL` matches when
written as if/else with the caller assigning its result right after the
call, and a stack parameter among register parameters usually means its
address is taken somewhere in the body.

## 2026-10-03: 2423 functions match

```
matched 2423 of 11317 game functions (191340 of 2783395 bytes, 6.87%)
```

- **lane D**, round 4: the network session manager and voice chat. It also
  found the source of a dead stack store in retail's inlined `is_host` check:
  a `volatile` local on the non-host path. That one change matched the check
  and the session setters it is inlined into.
- **lane L**, round 1 (`0x120000`): the async job queue and worker thread,
  global preferences, font loading, cache files, the sound manager and the
  texture cache.

## 2026-10-03: 2283 functions match

```
matched 2283 of 11317 game functions (179180 of 2783395 bytes, 6.44%)
```

- **lane H**, round 2 (`0x190000`): the Juggernaut game engine, game engine
  events, voice DSP effects and HUD messaging;
- **lane C**, round 3: the animation graph, which the animation channels
  and AI were waiting on;
- **lane I**, round 1 (`0x250000`): props, AI slot handlers, command scripts
  and firing-position evaluators;
- **lane K**, round 1 (`0x220000`): impacts, the sound driver and timing.

## 2026-10-03: 2152 functions match; 6% of the game's code

```
matched 2152 of 11317 game functions (167081 of 2783395 bytes, 6.00%)
```

- **lane D**, round 3: `c_class_58d20` (about 110 methods), the network
  observer, connections and transport keys;
- **lane B**, round 4: actor code at `0x1f0000`, and a finding about the build.
  An internal function's register convention comes from its own body and its
  callees, not its callers. So a function whose body is right can still be
  waiting on a callee that takes an argument differently in retail;
- **lane A**, round 4: the WMA and PCM sound codecs and the sound-effects
  class, signed saved-game file tasks and more AI-script functions;
- **lane F**, round 3: the outside functions its sound-source callbacks need.

New lanes have started on regions nobody had touched (`0x250000`, `0x090000`,
`0x220000`, `0x120000`), and lanes E and G now continue as a single UI lane.

## 2026-10-03: 2049 functions match; past 2000

```
matched 2049 of 11317 game functions (157385 of 2783395 bytes, 5.65%)
matched 2050 of 17069 functions in scope (157397 of 3731252 bytes, 4.22%)
```

Six more lane stints landed:
- **lane B**, round 3: the outside functions its slot handlers call (character
  block getters, unit seats, node points, clumps);
- **lane C**, round 2: AI scratch buffers, physics and animation channels;
- **lane E**, round 2: list constructors, settings-edit lists and the widget
  base methods;
- **lane F**, round 2: sound decibel conversion and object queries;
- **lane G**, round 1 (`0x230000`): screen widgets, window manager channels and
  2D polygon clipping;
- **lane H**, round 1 (`0x190000`): local controllers, game-variant checks and
  data compression.

A lesson from this round: in an LTCG build, a function's argument registers
follow its callers. Many functions with correct bodies wait only for more of
their callers to be decompiled before they match.

## 2026-10-03: 1809 functions match; lanes go vertical

```
matched 1809 of 11317 game functions (142921 of 2783395 bytes, 5.13%)
matched 1809 of 17069 functions in scope (142921 of 3731252 bytes, 3.83%)
```

Most functions left in each lane's region call code outside it, which takes
its arguments in registers that a stub can't reproduce. So the lanes now
decompile those outside functions first, then their callers:
- **lane A**, round 3: 98 more, among them the AI-script and squad functions
  the script evaluators call, and `function_1dee50`;
- **lane D**, round 2: network configuration, `xuid_equal` and the online game
  invite.

Contributors can see who is working where in the pinned
[Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
issue.

## 2026-10-02: 1703 functions match; five region lanes merged

```
matched 1703 of 11317 game functions (131004 of 2783395 bytes, 4.71%)
matched 1703 of 17069 functions in scope (131004 of 3731252 bytes, 3.51%)
```

Five region lanes landed their stints:
- **lane A** (0x2A0000): 127 more of the script engine's built-in functions;
- **lane B** (0x1B0000): 20 more actor slot handlers;
- **lane D** (0x060000): 91 functions of the simulation world, online tasks and
  network sessions;
- **lane E** (0x2B0000–0x2CB8C0): 103 UI screen, list and widget methods, three
  game engines and the uncompressed animation codecs;
- **lane F** (0x180000): 51 functions for local player slots, player control,
  looping sounds, sound sources and the loop allocator.

Two more regular batches added game-speed, camera and quaternion code. The
checker now treats libcmt's `memmove` and `memcpy` as one function, since they
are the same code.

The README and the new [CONTRIBUTING.md](../CONTRIBUTING.md) explain how to
join in.

## 2026-10-02: 1292 functions match; AI code and nine more batches

```
matched 1292 of 11317 game functions (101876 of 2783395 bytes, 3.66%)
matched 1292 of 17069 functions in scope (101876 of 3731252 bytes, 2.73%)
```

- **Lane C** (0x1C0000–0x1CFFFF) finished its first stint: 64 AI and actor
  slot-handler functions, including `src/ai.cpp`. The link now reads its
  object list from a response file, since the command line had outgrown
  Windows' 32K limit.
- **Nine regular batches** landed: geometry and axis tables, input records,
  visibility tests and render state, handle tables, aim assist, transport
  addresses and more. Their duplicated constants, inline vector helpers and
  types now live in shared headers.

## 2026-10-02: 1183 functions match; joint behaviour and Bink playback

```
matched 1183 of 11317 game functions (89984 of 2783395 bytes, 3.23%)
matched 1183 of 17069 functions in scope (89984 of 3731252 bytes, 2.41%)
```

Two more files from @Banshee64 are merged: `unknown_26e370.cpp` (actor joint
behaviour, whose seven callbacks are now wired into the slot-handler tables
with their retail `__stdcall` convention) and `unknown_01e930.cpp` (Bink movie
playback and its memory callbacks).

The checker now handles identical functions that the linker folded into one
body: a call into one of them matches through any of their names.

## 2026-10-02: 1166 functions match; shared engine headers

```
matched 1166 of 11317 game functions (88136 of 2783395 bytes, 3.17%)
matched 1166 of 17069 functions in scope (88136 of 3731252 bytes, 2.36%)
```

Six more batches landed: text formatting, game-engine marker objects,
geometry helpers, random-number users and player state. Duplicated
declarations from those batches are now shared: one `c_game_engine` class
in `include/unknown_1523c0.h`, common float helpers in `include/unknown_0259d0.h`,
and the player-state and match-globals layouts in `include/globals.h`.

## 2026-10-02: 1125 functions match; independent region lanes

```
matched 1125 of 11317 game functions (83039 of 2783395 bytes, 2.98%)
matched 1125 of 17069 functions in scope (83039 of 3731252 bytes, 2.23%)
```

**Lanes.** Work now also runs in "lanes". A lane is an independent agent that
owns one address region and works through it on its own branch, like an
outside contributor. Two lanes finished their first stint:
- one decompiled 143 of the script engine's built-in function evaluators, each
  followed by its retail definition data;
- one decompiled 65 actor slot-handler (AI behaviour) callbacks, with their
  handler structs at retail addresses.

**Contributors.** @Banshee64's `unknown_07aec0.cpp` is merged, and more
files are in progress. Claimed address ranges are kept free of our
automated work.

## 2026-10-02: 913 functions match; network message codecs

```
matched 913 of 11317 game functions (67094 of 2783395 bytes, 2.41%)
matched 913 of 17069 functions in scope (67094 of 3731252 bytes, 1.80%)
```

**Network messages.** Each network message type has an encoder, a decoder and
a clear/compare callback. A registration function stores them in the
message-type table. The codecs, and the bit-stream module they write
through, are now decompiled, and most of them match byte for byte.

**The data arrays are complete.** All 18 core handle-pool routines match.

**More contributors.** @Banshee64 contributed `unknown_1c9830.cpp` and
`unknown_1248b0.cpp`. Work is coordinated by address range, so contributors don't
collide.

## 2026-10-02: 723 functions match; subsystem lifecycle callbacks

```
matched 723 of 11317 game functions (40978 of 2783395 bytes, 1.47%)
matched 723 of 17069 functions in scope (40978 of 3731252 bytes, 1.10%)
```

**Subsystem lifecycle.** The engine starts and stops its subsystems through a
table of 68 entries (`0x440DD8`). Each entry has up to nine callbacks:
initialize, dispose, per-map and per-BSP set-up and teardown, and change
notifications. 55 of 58 callbacks attempted so far match. They are named after
their subsystems: `function_14b4b0`, `function_17d2a0`,
`arena_initialize_for_new_map` and so on.

**Also matching:**
- the session-state manager;
- script value casts;
- `function_11c9c0`;
- color conversions;
- binary search and short sort;
- HUD helpers;
- model variant lookups;
- path-finding heap operations.

## 2026-10-02: 612 functions match; the core data arrays

```
matched 612 of 11317 game functions (34854 of 2783395 bytes, 1.25%)
matched 612 of 17069 functions in scope (34854 of 3731252 bytes, 0.93%)
```

**Data arrays.** The engine keeps most of its runtime state in handle-addressed
pools: players, objects, effects, AI and more. 16 of the 18 core routines now
match:
- create and dispose;
- allocating an element at the next free slot or a given one;
- deleting by handle;
- handle-to-pointer lookup;
- iteration.

`include/data_array.h` is the single type every subsystem now uses.

**Also matching:**
- the unit, item, weapon, projectile and device object types;
- the game-engine player entity and breakable-surface entity definitions;
- about 25 event-definition classes;
- surface descriptions;
- tag lookups.

**Contributions.** The tools now also run on Linux under Wine, thanks to a
pull request from @Banshee64.

## 2026-10-02: 442 functions match; more than 1% of the game's code

```
matched 442 of 11317 game functions (29446 of 2783395 bytes, 1.06%)
matched 442 of 17069 functions in scope (29446 of 3731252 bytes, 0.79%)
```

**Switch statements are whole functions now.** Function discovery used to
record each `switch` case label, and the jump tables themselves, as separate
functions. Now they belong to the function that owns them: 525 fragment rows
are gone, and 126 functions have their full extent. The game-function total
dropped from 11,802 to 11,317 for that reason, not because work was lost.

**Also matching:**
- the game-engine class;
- network session state classes;
- UI widgets;
- the vehicle and turret type classes;
- table-driven data readers;
- input mapping;
- and more.

**Next.** The engine's core data-array routines (handle-addressed pools that
nearly every subsystem uses) are decompiled and waiting to merge. They
unblock many callers whose calls into those routines could not match while
the routines were stubs.

## 2026-10-02: 354 functions match; object types as C++ classes

```
matched 354 of 11802 game functions (26036 of 2782989 bytes, 0.94%)
matched 354 of 17586 functions in scope (26036 of 3730854 bytes, 0.70%)
```

**Object types are a class hierarchy.** The vtables for vehicles, turrets and
related object types share most of their slots. Every slot that two or more
of these vtables share is now a method of one base class,
`c_object_type_definition`, in `include/object_type_definitions.h`. Each type's
own overrides live in its derived class. The methods decompiled so far match
with the class written as plain C++.

**Also matching:**
- two interface vtables of 21 slots each, almost entirely;
- table-driven data readers;
- 256-bit bit-vector helpers;
- string and Unicode helpers, including variadic formatting functions.

**Build fixes.** Stand-ins now handle variadic functions. Stubs for code that
is not decompiled yet are built without function-level linking, so the
linker no longer folds identical ones into one address.

## 2026-10-02 (night): 271 functions match

```
matched 271 of 11802 game functions (22986 of 2782989 bytes, 0.83%)
matched 271 of 17586 functions in scope (22986 of 3730854 bytes, 0.62%)
```

**Tables lead to whole families of functions.** Retail's data holds tables of
function pointers: callback tables and C++ vtables. The functions one table
points at usually come from one source file and share one shape. A table at
`0x470828` led to 62 field-descriptor callbacks. All 62 now match, with the
table itself rebuilt entry for entry. A scanner now finds such tables. The
next batches are drawn from them: more vtables and callback tables.

**Also matching since 163:**
- input-device state;
- timed screen effects;
- Direct3D texture and palette set-up;
- a page heap that implements a library allocator;
- object iteration helpers;
- input mapping;
- more class hierarchies, with their deleting destructors.

## 2026-10-02 (later): 163 functions match; C++ destructors and library callers

`python tools/check.py` reports:

```
matched 163 of 11802 game functions (17564 of 2782989 bytes, 0.63%)
matched 163 of 17586 functions in scope (17564 of 3730854 bytes, 0.47%)
```

**What matches now:**
- Random numbers: the seed, and random vectors in a cone.
- Hash tables, bit vectors and integer log2.
- Network-message counters.
- An actor action-slot system.
- A handle table whose classes use virtual methods.

**What the build now handles:**
- **Functions that library code calls.** Havok, the C runtime and the XDK
  libraries were not built with link-time code generation. Any game function
  they call keeps its standard calling convention. The build reads those
  callers from the inventory and models them. Three Havok-called methods now
  match without any changes to their source.
- **Deleting destructors.** The destructor slot of a vtable holds a function
  the compiler generates: it calls the destructor, then `operator delete`.
  A `deleting` marker now ties that function to its class, so a class is
  written as Bungie wrote it, even when its destructor is implicit.
- **Data tables of function pointers.** These are written only where retail's
  data actually holds them, at their retail addresses. For example, the
  handler struct at `0x47d930` is reproduced field for field.

**What the build taught us:**
- VC7.1 fully unrolls `for (i = 0; i < 3; i++)` over a small body. Retail's
  3-iteration loops were written `do { ... } while (i < 3);`.

## 2026-10-02: 132 functions match; virtual methods supported

`python tools/check.py` reports:

```
matched 132 of 11802 game functions (14647 of 2782989 bytes, 0.53%)
matched 132 of 17586 functions in scope (14647 of 3730854 bytes, 0.39%)
```

**How the work is organised.** Parallel workers each take a batch of
neighbouring functions. A batch is usually one source file. Workers
send their work back in waves. Each wave is merged into `main`, and the
globals that several files share are unified in `include/globals.h`. Wave 2
and wave 3 together added 91 matches.

**What matches now:**
- Object list management and the object header table.
- Localized wide-string getters.
- Game state globals, and more of the AI and animation code.
- Several C++ classes, including a class that overrides two virtual methods of
  its interface.

**What the build taught us:**
- **Bit flags:** flag words are 1-bit bitfields tested with a `bool` cast
  (`TEST_FIELD_BIT` in `unknown_11c920.h`). That is the only form that compiles to
  retail's `shr reg, N; test reg, 1` sequence. It appears about 385 times in the
  game.
- **Virtual methods:** stand-ins now call methods by qualified name and
  copy-construct each class that has a vtable. A virtual method's address then
  escapes into the vtable, as in retail, and LTCG keeps its `thiscall`
  convention. Virtual methods are written as Bungie wrote them, with no
  workarounds.
- **Inlining across files:** LTCG inlines a small function into callers in
  other files unless that function's own file is built with `/Ob1`. Per-file
  flags therefore matter across files.
- **Caller-driven conventions:** some near-misses differ only in which
  registers carry their arguments. LTCG picks those registers from the
  callers, which are not decompiled yet. The checker re-tests every function
  on each build, so these can turn into matches later.
- **More code outside Bungie's:** a Havok collision query inlined into game
  code at `0x183910` was excluded from game code.

## 2026-10-02: decompilation has started: 41 functions match

`python tools/check.py` reports:

```
matched 41 of 11815 game functions (4923 of 2785826 bytes, 0.18%)
matched 41 of 17599 functions in scope (4923 of 3733691 bytes, 0.13%)
```

**What matches.** 41 retail functions rebuild byte for byte:
- File path helpers: `function_137320`,
  `function_137370` and `function_1373c0`.
- Unicode classification and UTF-8 encoding.
- 3x3 and 4x3 matrix maths, including two hand-written assembly routines.
- AI firing-position evaluation and AI clumps.
- Recorded-animation playback readers.
- S3TC and texture helpers.
- The CRC functions and the game state allocator from the spike.

About 20 more are near-misses: the same length, but the register allocation or
operand order differs.

**Where Bungie's code ends.** Bungie's code ends at `0x2cb8c0`. Everything
above that in `.text` is Xbox SDK libraries and third-party code: Havok, Bink,
the C runtime, voice, WMA, DSOUND and compiler-generated stubs. Applying that
boundary in `config/owners.json` cut the game-code total from 12,959 to 11,815
functions. Two regions that had been taken for game code turned out to be
Havok physics code.

**What the build taught us:**
- Floating-point code needs `/arch:SSE` (some files need `/arch:SSE2`), because
  Bungie's build used SSE.
- Callbacks stored in tables are `__stdcall`.
- Some maths routines are hand-written inline assembly.
- Game code that calls Direct3D called the public D3D API. The SDK's
  `d3d8ltcg.lib` is linked, and LTCG inlines parts of it, as in retail.

**Tooling changes:**
- Per-file compiler flags now live in the source, in a `// @flags` comment.
- The build links the SDK libraries.
- The checker also verifies call targets. A call must reach the function with
  the same retail address or the same name.
- Stand-in callers are generated inside each source file's own translation
  unit.
- `tools/permute.py` searches variants of a source function for near-misses.

**Next.** Fix the near-misses, then continue up from the leaf functions with
`tools/ready.py`.

## 2026-10-01: project set-up: inventory, whole-game build, checker

The set-up is in place. `python tools/check.py` reports:

```
matched 8 of 12959 game functions (421 of 2891676 bytes, 0.01%)
matched 8 of 17599 functions in scope (421 of 3733691 bytes, 0.01%)
```

Eight functions are MATCH: `function_163ba0`, `function_163c00`,
`function_123d40`, `distance3d`, `function_259d0` and three game state
initializers. `game_state_malloc_aligned` (`0x123d80`) is the one near-miss: a
single `lea` operand order.

**The inventory** (`config/functions.csv`) has 19,509 functions. By owner:

| Owner | Functions |
| --- | ---: |
| `game` | 12,959 |
| `eh` (MSVC exception-handling stubs) | 586 |
| `third:havok` | 1,034 |
| `third:bink` | 290 |
| `xdk:xonline` | 915 |
| `xdk:xvoice` | 816 |
| `xdk:wmadec` | 649 |
| `xdk:dsound` | 554 |
| `xdk:xnet` | 416 |
| `xdk:libcmt` | 394 |
| `xdk:xapi` | 270 |
| `xdk:d3d8` | 254 |
| `xdk:xapilib` | 219 |
| `xdk:libcpmt` | 93 |
| `xdk:d3dx` | 36 |
| `xdk:xonlines` | 23 |
| `xdk:rockall` | 1 |

Only `game` functions are ours to decompile. "In scope" is everything except
`eh`, Havok and Bink (19,509 less 586, 1,034 and 290 is 17,599).

**What changed from the spike's tool.** `tools/check.py` replaces
`tools/match.py`:
- Exact extents. Ours come from the linker map and retail's from the
  inventory. When the rest of our function is `0xCC` fill, retail's size is
  compared.
- Exact masks. The masked bytes are the base relocations plus the linker's
  `/MAPINFO:FIXUPS` relative fields, not guesses from values.
- Each masked field is validated against retail: an absolute field must hold a
  retail-image address, a relative field must leave the function, and every
  field must lie inside both extents. A field that points inside the function
  (a jump table entry, a self-call) must point at the same offset in retail.
- Status (`matched`, `near`, `todo`) is written back to
  `config/functions.csv`.

**What we found while building the inventory:**
- Discovery needed end clamping, and had to drop weak starts that land in the
  middle of an instruction.
- Library code is recognised by byte signature from the SDK's own `.lib`
  files (CRT, XAPI, DSOUND, XONLINE, XVOICE, XNET, D3DX) and by section (D3D8,
  XPP, Bink, WMA).
- Unnamed functions take the owner of their neighbours.
- The 586 MSVC exception-handling stubs are classed `eh`: they are
  compiler-generated, not work items, and the checker does not check them.
- A bug that let the checker accept a wrong address field (an absolute field
  was masked without checking that retail held an address there) was fixed
  before first use.

**What we found about `PRIVATE`.** Bungie's static functions can be
compiled with external linkage (the `PRIVATE` macro is empty). That did not
change `function_163c00`'s code, which still matches. So generated stand-in
callers in other files can reach static functions.

**Note.** The disassembler is `tools/disasm.py`, not `dis.py`: a `dis.py`
would shadow Python's standard-library `dis` module.

**Next.** Decompile from the leaf functions up, picking work with
`tools/ready.py`.

## 2026-10-01: the spike's answer: the LTCG build can be matched

Eight retail functions now rebuild byte for byte, and a ninth is one
instruction short:

| Function | Retail | What it tests |
| --- | --- | --- |
| `function_163ba0` | `0x163ba0` | a custom calling convention |
| `function_163c00` | `0x163c00` | a custom calling convention |
| `function_123d40` | `0x123d40` | an argument moved from the stack to `eax` |
| a game state initializer | `0x1edbc0` | `function_123d40` inlined into a caller optimized for speed |
| two game state initializers | `0x24c819`, `0x165cc3` | callers optimized for size, which call it out of line |
| `distance3d` | `0x3ea30` | x87 floating point and evaluation order (the body only) |
| `function_259d0` | `0x259d0` | LTCG deleting unused arguments (the body only) |
| `game_state_malloc_aligned` | `0x123d80` | one instruction short: `lea eax, [ebx + ecx]` against our `[ecx + ebx]` |

The game state functions and both CRC functions match together in one LTCG
image, with each source file built with its own flags. "The body only" means
the function matches when kept out of line. What keeps retail's copies of
these two out of line is not yet known.

**What the spike found about Bungie's build:**

- **Two kinds of code.** Most is optimized for speed (`/O2`): functions
  aligned to 16 bytes, no frame pointer. Some is optimized for size (`/O1`):
  functions packed without padding, `ebp` frames, `push 4; pop ecx`. LTCG
  keeps each source file's flags.
- **Inlining follows the flags.** `function_123d40` is inlined at about 51
  call sites, all in code optimized for speed. It is called at 7, all in code
  optimized for size. Our compiler makes the same choices from the same
  flags.
- **Some files need `/Ob1`.** With `/Ob2`, our compiler inlines
  `function_163ba0` into `function_123d40`, which retail does not. With
  `crc.cpp` built `/Ob1`, everything matches. Which files differ like this
  is still to be mapped.
- **The source's shape matters, as in any matching decompilation.** Examples:
  - `short` loop counters.
  - The order of the terms in `distance3d`.
  - Checksumming a local copy rather than a parameter. Taking a parameter's
    address keeps it on the stack.
- **SSE.** Retail uses `movss`, `ucomiss`, `xorps` and `fcomi` in some float
  code. We have not reproduced that in a test yet.

**The tool.** `tools/match.py` now takes several source files, each with its
own flags, and links them into one image. It masks only the bytes that the
test image relocates; before, it guessed from values.

**Next.** We will design the project set-up:
- the build, with per-file flags;
- a function inventory of the retail XBE;
- a way to tell each function's flags from its code;
- progress tracking.

## 2026-10-01: the first two functions match

**LTCG can be matched.** Two retail functions now rebuild byte for byte:

| Function | Retail address | Arguments in retail |
| --- | --- | --- |
| `function_163ba0` | `0x163ba0` | buffer in `eax`, size in `edi`, CRC pointer on the stack (`ret 4`) |
| `function_163c00` | `0x163c00` | table in `edx` |

- **The source:** Halo CE's `crc.c` from the
  [punpckhdq/halo](https://github.com/punpckhdq/halo) decompilation (CC0),
  compiled as C++.
- **The toolchain:** XDK 5849's compiler, `/O2 /GL /Gr` (fastcall by default),
  linked with `/LTCG`.
- **The calling conventions:** the compiler chose the same custom ones as the
  retail build, from the source alone.
- **What "byte for byte" means here:** every instruction and every byte is
  equal, except the data addresses, which differ because the test image lays
  out its own data.
- **One source detail mattered:** the loop counters must be `short`, as in
  Halo CE. With `long` ones, the compiler unrolls the inner loop.

This answers the spike's main question for simple functions: the XDK 5849
compiler and these flags reproduce Bungie's LTCG output. The next tests use
harder cases: floating point, C++ member functions, and functions whose
callees were inlined.

Reproduce it with `tools/match.py`; the command is at the top of
`spike/crc.cpp`.

## 2026-10-01: the target, the toolchain, and LTCG

**The target build.** The retail disc's `default.xbe`:

- SHA-256 `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`.
- Internal name `c:\halo2\bin\halo2ship.exe`, linked 2004-09-28.
- Title ID `4D530064`, all regions.
- `.text` is 0x367C4C bytes (3.4 MiB) at 0x12000.
- Linked libraries:
  - XAPILIB, XBOXKRNL, LIBCMT and LIBCPMT
  - DSOUND, XVOICE, VOICMAIL and XONLINES
  - D3D8LTCG and XGRAPHCL

  Every one is version 1.0.5849.
- Its own sections hold DSOUND, WMADEC, XONLINE, XNET, Bink, D3D and XPP.

**Earlier work.** We found no existing Halo 2 decompilation.

**LTCG.** The retail game code was compiled with link-time code generation:

- `tools/ltcg_probe.py` finds 9,954 direct call targets in `.text`.
- 4,232 of them (42.5%) read `eax`, `ebx`, `esi` or `edi` at entry before
  writing them. MSVC's standard conventions never pass arguments in those
  registers.

Some examples:

| Function | Arguments |
| --- | --- |
| `0x128c0` | A pointer in `eax` |
| `0x14390` | Values in `edi` and `esi` |
| `0x16b10` | A pointer in `esi` |

That rules out the usual decompilation workflow, where each function is
compiled alone and compared with the original. Under LTCG the code is
generated for the whole program at once, when it is linked.

**The toolchain.** XDK 5849 ships its own compiler in `xbox\bin\vc71`:

- `cl` 13.10.3077 and `link` 7.10.3077 (Visual C++ .NET 2003).
- Both run natively on Windows 11.
- The SDK's `d3d8ltcg.lib` and `xgraphicsltcg.lib` are the LTCG forms of the
  libraries linked into the XBE.

**Next.** The feasibility spike asks whether XDK 5849's compiler, in LTCG
mode, can reproduce retail functions byte for byte.
