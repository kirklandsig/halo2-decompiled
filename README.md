# Halo 2 Decompilation (work in progress)

Halo 2 Decompilation is an independent preservation and reverse-engineering
project. It studies the retail executable of **Halo 2 for the original Xbox**
and recreates its program logic in human-readable C and C++ through
independent analysis. Each recreated function is checked against the retail
executable: built with the period-correct compiler, it must produce exactly
the same bytes. That check is what makes the recreated code verifiable. A
longer-term research goal is to study how the game's code could run on other
platforms, using game data that each user supplies from their own copy.

Alongside the game's own code, the project also decompiles **Havok**, the
physics library linked into the game, because Halo 2's vehicles, ragdolls and
projectiles have to behave exactly as they do on the Xbox. The executable's
other libraries (the Xbox SDK's, Bink video and so on) are not decompiled; a
port would replace them.

The project is non-commercial and is not affiliated with or endorsed by
Microsoft, Bungie or 343 Industries. **Read [LEGAL.md](LEGAL.md)** for the
project's scope, what it does not contain, and its rules for contributions,
and [PROVENANCE.md](PROVENANCE.md) for where its information comes from.

**This is a community-driven decompilation.** It is built in the open by many
people: contributors who claim a range and send pull requests, and maintainers
who review, merge and keep the build honest. Many of the people working on it,
the maintainer included, work alongside AI coding agents such as Claude and
Codex, and every change still has to pass the same byte-for-byte check and
review. Everyone who has helped is credited in
[The community](#the-community).

**Contributors are welcome.** No prior decompilation experience is needed,
only patience and some C. **New here? Start with
[docs/START_HERE.md](docs/START_HERE.md):** the setup checklist, how to claim
a range, and a prompt you can paste into an AI coding agent. Then see
[How to help](#how-to-help) and [CONTRIBUTING.md](CONTRIBUTING.md).

## What this repository does not contain

- No Halo 2 game assets: no maps, textures, models, audio or cinematics.
- No game executable (XBE) and no disc image.
- No Microsoft Xbox SDK/XDK: no compiler, libraries or headers.
- No Havok SDK material: no headers, documentation, samples or source. The
  Havok code here is recreated from the retail executable and named by what it
  does.
- No original or leaked Halo 2 source code.
- No leaked symbols, PDBs or linker maps. Names that came from such sources
  through a third-party dataset have been removed from the inventory, and
  names in the source that matched such sources have been renamed; see
  [PROVENANCE.md](PROVENANCE.md).
- No confidential or internal Microsoft, Bungie or 343 Industries material.

## Requirements

To build or check anything, you must supply any original game files you need
from a copy of Halo 2 that you are legally entitled to use. The project does
not provide game files.

The project does not distribute the Microsoft Xbox SDK/XDK or any proprietary
Microsoft development tools. Contributors are responsible for ensuring that
any development tools they use are obtained and used lawfully. The
maintainers cannot provide, link to, or assist with obtaining proprietary
SDK/XDK materials. Byte-for-byte checking currently depends on the original
compiler, and no open-source toolchain can reproduce its output yet.
Analysis, documentation and review do not need it.

## Status

Decompilation is under way, and 10196 retail functions now match byte for
byte. The checker reports:

```
matched 10196 of 11326 game functions (1780149 of 2786206 bytes, 63.89%)
matched 1267 of 1436 Havok functions (174339 of 290362 bytes, 60.04%)
matched 11492 of 17654 functions in scope (1957921 of 3904218 bytes, 50.15%)
```

Matched code so far includes:
- the script engine's built-in functions;
- AI, actor behaviours and actor slot handlers;
- object damage, shields and vitality;
- network sessions, message codecs and the bitstream;
- UI screens and widgets, and the game engines;
- sound sources, looping sounds and Bink movie playback;
- the game's data arrays, object lists and hash tables;
- geometry, quaternion and matrix maths;
- input, file paths, localized strings, random numbers and game state;
- most of the Havok physics library's functions, from collision agents and
  constraints to ragdolls and file streams.

Every function is checked automatically on every build, so a match stays a
match. Refer to [docs/PROGRESS.md](docs/PROGRESS.md), which is updated as work
lands.

## The target

The retail disc build of Halo 2, as found on the retail disc:

| | |
| --- | --- |
| `default.xbe` SHA-256 | `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d` |
| Internal name | `halo2ship.exe` |
| Linked | 2004-09-28 |
| Xbox SDK | XDK 5849 (libraries 1.0.5849) |
| Compiler | Visual C++ 7.1 from XDK 5849 (`cl` 13.10.3077), `/GL /Gr` with `/O2` or `/O1` per file, linked with `/LTCG` |

## The approach

The aim is a **matching** decompilation: C source that the original compiler
turns back into the same bytes as the retail XBE. Matching gives every
function an automatic pass or fail, and that check is what made the Halo CE
decompilation dependable.

Halo 2's retail build is harder than Halo CE's. It was compiled with
**link-time code generation (LTCG)**: the compiler generates code for the
whole program at once, when it is linked. So functions are inlined across
source files, and internal functions get custom calling conventions. 42.5%
of the functions called in the retail XBE take arguments in `eax`, `ebx`,
`esi` or `edi`, which MSVC's standard conventions never do. Under LTCG, a
function's bytes depend on its callers and callees, not only on its own
source. We know of no earlier project that has matched such a build.

So the work runs in stages:

1. **Feasibility spike (done).** Rebuild retail functions with the XDK 5849
   compiler in LTCG mode, and find out whether their bytes can be matched.
   They can: custom calling conventions, deleted arguments and inlining
   decisions all reproduce. The test sources are in `spike/`.
2. **Project set-up (done).** The function inventory, the build with each
   source file's flags, the checker and the ready queue.
3. **Decompilation (in progress)**, from the leaf functions up. What has
   worked best on this build, such as fixing a callee's calling convention so
   its callers match, and the pitfalls (aligned locals, link order), is
   collected in [docs/DECOMPILING.md](docs/DECOMPILING.md).
4. **Readability (in progress, alongside 3).** Once a group of functions
   matches, it gets names from what the code does, structs for the data it
   reads, and a source file named after its subsystem, without changing a byte
   of the build.
5. **Havok (in progress, alongside 3).** The physics library is decompiled the
   same way. It was a prebuilt library compiled without LTCG, so it matches
   with plain per-file flags; see
   [docs/DECOMPILING.md](docs/DECOMPILING.md#havok).
6. **Native port.** The game and Havok code from stages 3 and 5, with the Xbox
   SDK, Bink and the other libraries replaced.

## Build and check

1. Install the Python dependencies: `pip install -r requirements-dev.txt`.
2. Point the tools at your local files. The retail XBE from your own copy
   goes at `orig/default.xbe` (or set `RETAIL_XBE`), and the development
   toolchain's `xbox` folder at `sdk/xbox` (or set `XDK_DIR`); see
   [Requirements](#requirements). Both folders are git-ignored and must never
   be committed. In Git Bash, also run `export MSYS_NO_PATHCONV=1`, which
   stops Git Bash rewriting the `/`-style arguments (such as `/O2`) that the
   compiler takes. To read the XBE out of a disc image of your own copy:

   ```
   python tools/xiso_extract.py "Halo 2.iso" orig default.xbe
   ```
3. The inventory, `config/functions.csv`, is already committed, so you don't
   need to regenerate it. Its `name` column holds only library functions'
   names, from their signatures in the SDK libraries; its `object` column is
   empty. See [PROVENANCE.md](PROVENANCE.md).
4. Run `python tools/check.py`. It builds the whole game as one LTCG image,
   compares every decompiled function with the retail bytes, and writes each
   function's status (`matched`, `near` or `todo`) back to
   `config/functions.csv`. It exits 1 while any function differs.
5. Pick work with `python tools/ready.py`, following
   [docs/DECOMPILING.md](docs/DECOMPILING.md). `--claims` takes a saved copy
   of the [Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
   table so the list skips ranges someone already has.

| Tool | Use |
| --- | --- |
| `tools/xiso_extract.py` | Lists or extracts the files of an Xbox disc image. |
| `tools/xbe.py` | Summarises an XBE: sections, linked libraries, certificate. |
| `tools/ltcg_probe.py` | Counts the functions that take arguments in registers (the LTCG evidence above). Needs capstone. |
| `tools/inventory.py` | Finds every function in the XBE and writes `config/functions.csv`, naming library functions by their signatures. |
| `tools/functions.py` | Function discovery that the inventory uses. |
| `tools/libsig.py` | Recognises library code by byte signature from the SDK's `.lib` files. |
| `tools/build.py` | Builds the whole game as one LTCG image, with each source file's flags. |
| `tools/check.py` | Compares our functions with retail and records progress. Needs the SDK and capstone. |
| `tools/ready.py` | Lists the functions that are ready to decompile next. `--claims` drops addresses from a saved copy of the Active claims table; `--owner third:havok` lists Havok functions instead of the game's. |
| `tools/open_work.py` | Lists work a contributor can claim (source files with functions left, and functions ready to write) outside every person's claim, with the claim line to use. Takes a saved copy of issue #9. |
| `tools/permute.py` | Searches variants of a source function for ones that turn a near-miss into a match. |
| `tools/near.py` | Counts the near-misses in the csv by source file (functions, bytes). No XBE. `--list` prints each function. |
| `tools/masked.py` | Lists what `check.py` cannot see because it masks address fields: float constants one step away from the source's literals, strings, and script function definitions that differ from retail. Needs the retail XBE and capstone. |
| `tools/disasm.py` | Disassembles retail code. |
| `tools/match.py` | The spike's one-file matcher, kept for reference. Replaced by `check.py`. |

## How to help

Halo 2 has about 11,300 game functions, so there is room for many people.
[docs/START_HERE.md](docs/START_HERE.md) walks through these steps and has a
ready-made prompt for AI coding agents.
1. Get set up as in [Build and check](#build-and-check), with your own
   lawfully owned copy of Halo 2 (see [Requirements](#requirements)).
2. Pick a source file or an address range that no other person holds (the
   pinned [Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
   issue lists claims; the maintainers' automated lanes make way for
   contributors), and open a draft pull request saying what you are taking.
   `python tools/open_work.py` lists what is open.
3. Decompile, run `python tools/check.py`, and push as functions match.
   [docs/DECOMPILING.md](docs/DECOMPILING.md) explains the conventions and
   the compiler's quirks.

Havok functions are open too, and a gentler start than the game's: no
whole-program optimisation, so they match with plain per-file flags.
`python tools/ready.py --owner third:havok` lists the ready ones; claim a
range the same way.

Not ready to write code? Analysis helps too: several contributors have
mapped a function or a subsystem in a `docs/` note that the next person then
decompiled.

Details are in [CONTRIBUTING.md](CONTRIBUTING.md), including the
[contribution provenance policy](CONTRIBUTING.md#contribution-provenance).
Questions are welcome as issues.

## The community

Thank you to everyone who has contributed to this project so far:

- [@Banshee64](https://github.com/Banshee64), the first contributor: input,
  team allegiances, Bink playback, looping sounds, damage, projectiles, unit
  actions, vehicles, path search, cloth, recorded animations and devices, and
  then the match-tuning of all of it.
- [@coldspear](https://github.com/coldspear): matches across the game, among
  them object damage and its owner, online tasks and the upload connection,
  and big score cuts in dozens of near misses; much of the tooling (the
  permuter's mutations, `masked.py`, the near-miss report, claim-aware
  `ready.py`, the ByteMatch report (then decomp.dev) and the hardening of the disc and XBE
  readers); and analysis documents for the object core, weapons, surface
  lighting, Havok, the Xbox Live logon and more.
- [@BirchWoodGod](https://github.com/BirchWoodGod): matched ranges all over
  the game, from units and bipeds to network voice, team balancing, map paths,
  animation channels and lights, analysis of the network channel and session
  search code, and the published analysis credited below.
- [@BrassMonkey71](https://github.com/BrassMonkey71): matched functions in the
  unit and biped code, AI and elsewhere.
- [@mjshfrvstj-sys](https://github.com/mjshfrvstj-sys): the CI that runs the
  Python tests on every pull request, and disc-image tests.
- [@mrv777](https://github.com/mrv777): an analysis of the capture the flag and
  assault objects.
- [@RWayne93](https://github.com/RWayne93): online messages, a documented
  near miss, and the match of 0x119280, the creature death timer.
- [@andrewnakas](https://github.com/andrewnakas): the inventory fixes that
  took WMADEC's data tables and the DSOUND, XNET and D3D section tails out of
  the function list, and dozens of fixes checked against retail in the render
  state, sound, AI and camera code, including disabled drafts brought to
  compile and behave as retail.
- [@Roasted-Codes](https://github.com/Roasted-Codes): the first contributor
  matches in Havok, the physics library: its collision-pair dispatch and
  collision agents, constraint serialization, motion steps, and the
  library's file streams and string helpers.
- [@ccgman123](https://github.com/ccgman123): a careful diagnosis of seven
  near misses, the cleanup that removed an in-body pragma from 0x9d1c0, the
  match of 0x1f3230, and the tools that keep disabled drafts out of the ready
  and open-work lists.
- [@marioag0412](https://github.com/marioag0412): matches across the game,
  starting with 0x1e3480 (an object's actor index) and taking in the global
  preferences, network voice, online tasks and AI code, big score cuts in
  render and tag-function code, and notes on what still differs in a dozen
  near misses.
- [@averycpawelek](https://github.com/averycpawelek): matches in the unit
  code and nearby (0xc7100, 0xe5840, 0xe2d80 and more) and near misses
  brought closer, then much of Havok: its constraint helpers, ragdoll and
  hinge descriptions, service lifecycle and stream helpers, and shared
  physics helpers.

[@kirklandsig](https://github.com/kirklandsig) maintains the project: runs the
build, reviews and merges every pull request, and keeps the
[Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
table. Much of the maintainer's own decompilation runs in automated lanes
driven by **Claude** (Anthropic) and **Codex** (OpenAI): AI coding agents that
write and tune the source under the same checks and provenance rules as
everyone else, with each batch reviewed before it lands. Many contributors use
the same tools. The techniques in [docs/DECOMPILING.md](docs/DECOMPILING.md)
were worked out by all of them, people and agents alike, and the commit
history credits each change to its author.

Want to be on this list? See [How to help](#how-to-help).

## Credits

- [halo-symbol-atlas](https://github.com/tinkerer-red/halo-symbol-atlas)
  (CC BY 4.0): the `name` and `object` columns of `config/functions.csv` once
  came from this dataset. Because its names for this build derive from linker
  maps of unreleased builds, that data has been removed. See
  [PROVENANCE.md](PROVENANCE.md).
- [punpckhdq/halo](https://github.com/punpckhdq/halo),
  [bnunu/halo-1](https://github.com/bnunu/halo-1) and
  [halo-ce-universal](https://github.com/cybersecurity/halo-ce-universal):
  the Halo CE decompilation and port that this project follows. punpckhdq/halo
  is AGPL-3.0 since 2026-10-17 (CC0 before); its AGPL revisions are read for
  understanding only, never copied from (see [PROVENANCE.md](PROVENANCE.md)).
- [BirchWoodGod/halo2-decomp](https://github.com/BirchWoodGod/halo2-decomp):
  a functional recovery of the same XBE. Its published analysis gave us
  leads: the subsystem lifecycle table, the data-array layout and some
  register conventions. We verified each one against the retail code
  ourselves; none of its code or text is used here.

This project is not affiliated with, endorsed by or sponsored by Microsoft,
Bungie, 343 Industries or Activision. Halo, Halo 2 and Xbox are trademarks of
Microsoft; other names and marks belong to their respective owners. See
[LEGAL.md](LEGAL.md).

## License

Copyright (C) 2026 the halo2-decompiled contributors. The git history and
[The community](#the-community) list them.

The contributors' own work in this repository is licensed under the GNU
General Public License, version 3 or (at your option) any later version
(GPL-3.0-or-later). Refer to [LICENSE](LICENSE). Forks and other works based
on it that are distributed must stay under the GPL, with their source
available, and must keep the copyright and license notices.

Versions published before this change were released under CC0 1.0, and
copies of those versions stay under CC0. The GPL applies from the commit that
introduced this notice, on 2026-10-27, onward.

- The GPL applies only to material the contributors have the right to
  license.
- It grants no rights in Halo 2, or in any intellectual property of
  Microsoft, Bungie, 343 Industries, Activision or anyone else, and the
  earlier CC0 release did not place any of it into the public domain.
  Halo 2's game content and trademarks remain the property of their owners.
- The `name` and `object` columns of `config/functions.csv` once came from
  [halo-symbol-atlas](https://github.com/tinkerer-red/halo-symbol-atlas) under
  [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) and were not
  covered by the repository's own license. That data has been removed (see
  [PROVENANCE.md](PROVENANCE.md)).

Details are in [LEGAL.md](LEGAL.md#licence-scope).

### Additional permission for the Xbox Development Kit

The matching build links the libraries of the Microsoft Xbox Development Kit.
As an additional permission under section 7 of the GPL:

If you modify this program, or any covered work, by linking or combining it
with the libraries of the Microsoft Xbox Development Kit (or modified versions
of those libraries), containing parts covered by the terms of their licenses,
the licensors of this program grant you additional permission to convey the
resulting work.

This permission covers only those libraries. The project's own code stays
under the GPL.
