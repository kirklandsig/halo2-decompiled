# Contributing to the Halo 2 decompilation

Thanks for helping. This is a matching decompilation: a function is done
when the original compiler turns our source back into the retail XBE's exact
bytes, and `tools/check.py` decides that automatically.

Please read [LEGAL.md](LEGAL.md) and [PROVENANCE.md](PROVENANCE.md) before
contributing. For a step-by-step start, including a prompt to paste into an AI
coding agent, see [docs/START_HERE.md](docs/START_HERE.md).

## Contribution provenance

Contributions must be your own work, or work you have permission to
contribute. They must **not** contain, or copy into the repository, code,
symbols, names, strings, comments or other material from:

- leaked Halo or Halo 2 source code;
- leaked Microsoft, Bungie or 343 Industries source code;
- leaked PDB files or leaked linker maps, or symbol data derived from them
  (including names propagated from them by third-party datasets);
- internal or unreleased builds, or the symbols and strings inside them;
- confidential or internal documentation;
- material obtained in breach of a non-disclosure agreement;
- stolen development material;
- proprietary SDK/XDK files.

Sources that are acceptable, where appropriate:

- analysis of a retail executable you are lawfully entitled to use;
- disassembly and decompilation you performed yourself;
- the game's observable behaviour and runtime testing;
- publicly released documentation;
- source material its authors published under a licence that permits reuse;
- independently developed tools and research, credited.

Name things by what they do. If you don't know a function's purpose yet, use
the `function_<address>` and `unknown_<address>` placeholder forms. If an
important lead came from an external source, say which one in your pull
request. This is the repository's contribution policy, not legal advice; you
are responsible for complying with the law that applies to you.

## What you need

- **Your own copy of Halo 2** for the original Xbox (the retail disc build;
  the README lists its SHA-256), which you are legally entitled to use.
  Extract `default.xbe` with `tools/xiso_extract.py`. Never commit it, or
  any other game file.
- **A development toolchain** for byte-for-byte checking. The project does
  not distribute the Microsoft Xbox SDK/XDK or any proprietary Microsoft
  development tools. You are responsible for ensuring that any tools you use
  are obtained and used lawfully, and the maintainers cannot provide, link
  to, or help with obtaining proprietary SDK/XDK materials. Never commit any
  part of an SDK. Analysis, documentation and review don't need it.
- Python 3 with `pip install -r requirements-dev.txt`. The build runs on
  Windows, and on Linux under Wine.

The README's [Build and check](README.md#build-and-check) section has the
setup steps.

## Claim before you start

Several people and automated lanes work at once, so we split the game by
address range or by source file:

1. Check the pinned
   [Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
   issue, which lists every range being worked on (contributors' and our
   own), and the open pull requests, each of which names its range.
2. Pick something nobody else holds. The maintainers' automated lanes cover
   the whole game, but **they make way for contributors**:
   - You may claim one source file, or an address range of up to 0x2000
     bytes, inside any maintainer lane, and the lanes then leave it alone.
   - Only another person's claim (an `@user` row) is off limits.
   - A claim with no activity for two weeks lapses.

   `python tools/open_work.py --claims claims.md` lists what is open, with
   the claim line to use. It takes a saved copy of the issue. A group of
   related functions, for example one object type's callbacks found through
   its type table in the executable, is a good unit. Small leaf functions
   are the easiest start; `python tools/ready.py` lists functions whose
   callees are already done, and `--claims` leaves every claimed address out.
   Neither tool reads GitHub, and neither treats the Finished section as
   claimed.
3. Open a **draft pull request** early with the range in its description,
   for example "Retail range claimed: `0xd5990`–`0xd9fff` (damage code)".
   We keep our own work out of claimed ranges. If something you need is
   already claimed, say so in the pull request and we'll sort it out.

## Working

- Follow [docs/DECOMPILING.md](docs/DECOMPILING.md): the `// @retail 0x...`
  markers, per-file `// @flags`, stubs for callees that aren't decompiled
  yet, and the known compiler idioms.
- Run `python tools/check.py <va> ...` for your functions and a full
  `python tools/check.py` before you push. A function that matched before
  must still match.
- Near-misses are fine to commit with their markers; the checker re-tests
  them on every build, and they often match once their callers or callees
  land. `python tools/near.py` counts those near-misses from the csv, with no XBE.
- You don't need to commit `config/functions.csv`; we regenerate its
  statuses when we merge.

## Pull requests

- Commit with your GitHub no-reply email address
  (`<id>+<username>@users.noreply.github.com`). Our publishing checks reject
  other addresses.
- Fill in the pull request template's provenance checklist.
- Say in the description what matched and what is near, and list any change
  outside your own files (shared headers, stubs, other files' flags).
- Mark the pull request ready when you want it merged. We check it against
  current `main` and merge it with a merge commit, so your authorship stays
  in the history.

## Licence

By contributing you agree that your contribution is released under CC0 1.0,
like the rest of the repository (see [LICENSE](LICENSE)). CC0 covers only
material you have the right to license. It does not cover Halo 2's code,
content or trademarks, which belong to their owners (see
[LEGAL.md](LEGAL.md#licence-scope)).
