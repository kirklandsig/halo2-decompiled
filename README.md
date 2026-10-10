# Halo 2 Decompilation (work in progress)

Halo 2 Decompilation is an independent preservation and reverse-engineering
project. It studies the retail executable of **Halo 2 for the original Xbox**
and recreates its program logic in human-readable C and C++ through
independent analysis. Each recreated function is checked against the retail
executable: built with the period-correct compiler, it must produce exactly
the same bytes. That check is what makes the recreated code verifiable. A
longer-term research goal is to study how the game's code could run on other
platforms, using game data that each user supplies from their own copy.

The project is non-commercial and is not affiliated with or endorsed by
Microsoft, Bungie or 343 Industries. **Read [LEGAL.md](LEGAL.md)** for the
project's scope, what it does not contain, and its rules for contributions,
and [PROVENANCE.md](PROVENANCE.md) for where its information comes from.

**Contributors are welcome.** No prior decompilation experience is needed,
only patience and some C. **New here? Start with
[docs/START_HERE.md](docs/START_HERE.md):** the setup checklist, how to claim
a range, and a prompt you can paste into an AI coding agent. Then see
[How to help](#how-to-help) and [CONTRIBUTING.md](CONTRIBUTING.md).

## What this repository does not contain

- No Halo 2 game assets: no maps, textures, models, audio or cinematics.
- No game executable (XBE) and no disc image.
- No Microsoft Xbox SDK/XDK: no compiler, libraries or headers.
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

Decompilation is under way, and 7477 retail functions now match byte for
byte. The checker reports:

```
matched 7477 of 11318 game functions (876062 of 2784283 bytes, 31.46%)
matched 7477 of 17216 functions in scope (876062 of 3739273 bytes, 23.43%)
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
- input, file paths, localized strings, random numbers and game state.

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
3. **Decompilation (in progress)**, from the leaf functions up.
4. **Native port.**

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
| `tools/ready.py` | Lists the functions that are ready to decompile next. `--claims` drops addresses from a saved copy of the Active claims table. |
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

Details are in [CONTRIBUTING.md](CONTRIBUTING.md), including the
[contribution provenance policy](CONTRIBUTING.md#contribution-provenance).
Questions are welcome as issues.

## Credits

- [halo-symbol-atlas](https://github.com/tinkerer-red/halo-symbol-atlas)
  (CC BY 4.0): the `name` and `object` columns of `config/functions.csv` once
  came from this dataset. Because its names for this build derive from linker
  maps of unreleased builds, that data has been removed. See
  [PROVENANCE.md](PROVENANCE.md).
- [punpckhdq/halo](https://github.com/punpckhdq/halo),
  [bnunu/halo-1](https://github.com/bnunu/halo-1) and
  [halo-ce-universal](https://github.com/cybersecurity/halo-ce-universal):
  the Halo CE decompilation and port that this project follows.
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

The contributors' own work in this repository is released under CC0 1.0.
Refer to [LICENSE](LICENSE).

- CC0 applies only to material the contributors have the right to license.
- It does not place Halo 2, or any intellectual property of Microsoft,
  Bungie, 343 Industries, Activision or anyone else, into the public domain.
  Halo 2's game content and trademarks remain the property of their owners.
- The `name` and `object` columns of `config/functions.csv` once came from
  [halo-symbol-atlas](https://github.com/tinkerer-red/halo-symbol-atlas) under
  [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) and were not
  covered by CC0. That data has been removed (see [PROVENANCE.md](PROVENANCE.md)).

Details are in [LEGAL.md](LEGAL.md#licence-scope).
