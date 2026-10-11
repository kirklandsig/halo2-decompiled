# Start here

Want to help decompile Halo 2? This page gets you from nothing to your first
pull request. Most of it is a one-time setup. After that you can work by hand,
or paste the prompt below into an AI coding agent (Claude Code, Codex, Cursor
or similar) and review what it does.

No decompilation experience is needed. Some C helps.

## 1. Set up (once)

- [ ] **Your own copy of Halo 2** for the original Xbox, which you are legally
  entitled to use. Extract the executable from your disc image:

  ```
  python tools/xiso_extract.py "Halo 2.iso" orig default.xbe
  ```

  Its SHA-256 must be
  `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d` (the
  retail disc build). Other builds, such as title updates, won't match.
- [ ] **The development toolchain:** the `xbox` folder of the Xbox SDK (XDK
  5849, whose `bin/vc71/cl.exe` is version 13.10.3077), at `sdk/xbox` or
  pointed to by `XDK_DIR`. The project does not distribute it and can't help
  you obtain it; see the README's [Requirements](../README.md#requirements).
  Analysis, documentation and review don't need it.
- [ ] **Python 3**, then `pip install -r requirements-dev.txt`.
- [ ] **Windows**, or Linux with Wine. In Git Bash, also run
  `export MSYS_NO_PATHCONV=1`.
- [ ] **Check it works:** run `python tools/check.py`. It builds the whole
  game and takes several minutes. It ends with a line like
  `matched N of 11318 game functions`, and N should equal the number in the
  README's [Status](../README.md#status). If it does, you're
  ready. It exits with code 1 while any function differs, which is normal.

`orig/`, `sdk/` and `build/` are git-ignored. Never commit anything from them.

## 2. Claim some work

Several people and automated lanes work at once, so everyone works on their
own part of the game.

The pinned
[Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
issue lists who has what. The maintainers' automated lanes cover the whole
game, but **they make way for you**. You may claim one source file, or an
address range of up to 0x2000 bytes, anywhere except a range another person
holds (an `@user` row on that issue), and the lanes will leave it alone.

1. Save the claims issue and list open work:

   ```
   gh issue view 9 -R kirklandsig/halo2-decompiled --json body -q .body > build/claims.md
   python tools/open_work.py --claims build/claims.md
   ```

   Without the GitHub CLI, copy the issue's text from the web page into
   `build/claims.md` instead. The tool lists source files whose functions don't
   all match yet, fewest bytes left first. It also lists functions nobody has
   written yet whose callees are all done. Both are the easiest places to start.
2. Check the [open pull requests](https://github.com/kirklandsig/halo2-decompiled/pulls)
   too, in case someone claimed something a moment ago.
3. Open a **draft pull request** that says what you are taking. The tool
   prints the line to use, for example
   "Retail range claimed: `0x224240`-`0x224da0` (src/unknown_224240.cpp)".
   That is your claim; the maintainers add it to the issue. A claim with no
   activity for two weeks lapses.

## 3. Decompile

To work by hand, follow [DECOMPILING.md](DECOMPILING.md). To work with an AI
agent, open it in your clone of the repository and paste this prompt. If you
have already claimed something, replace the claim line with it; if not, the
agent finds open work with you first.

```text
You are helping with a matching decompilation of Halo 2 for the original Xbox, in this repository. A function counts as done only when the original compiler turns our C/C++ back into the retail executable's exact bytes; `python tools/check.py` decides that.

My claim: none yet.

First read docs/START_HERE.md, README.md (Build and check), CONTRIBUTING.md, PROVENANCE.md and docs/DECOMPILING.md, and follow them.

Setup check. Stop and tell me if any step fails:
1. `orig/default.xbe` exists (or `RETAIL_XBE` points to it) and its SHA-256 is 03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d.
2. `sdk/xbox/bin/vc71/cl.exe` exists (or `XDK_DIR` points to the `xbox` folder).
3. `python tools/check.py` runs to the end. Record its "matched N of 11318 game functions" line and copy `config/functions.csv` to `build/baseline.csv`.

Finding work, if my claim above is "none yet":
1. Save issue #9 with `gh issue view 9 -R kirklandsig/halo2-decompiled --json body -q .body > build/claims.md`. If the GitHub CLI isn't available, ask me to paste the issue's text into that file.
2. Run `python tools/open_work.py --claims build/claims.md` and show me the three best options: a small source file with few bytes left, or a few functions nobody has written yet. Say briefly why each is a good start.
3. When I pick one, give me the claim line the tool prints for it. I'll open a draft pull request with that line, or you open it with `gh pr create --draft` if I ask you to. Don't start until the claim is up.

Work only on the functions my claim covers.

Work loop:
1. Find candidates in my claim: the functions `python tools/ready.py` lists, and the near misses `python tools/near.py --list` lists. Start with the smallest.
2. For each function, read retail's code with `python tools/disasm.py <va>`. Write or tune the C++ in the right `src/` file, with `// @retail 0x<va>` on the line above it, and stub callees that aren't decompiled yet as docs/DECOMPILING.md describes.
3. After each change, run `python tools/check.py <va>` for the function and for its matched callers and callees.
4. When a function is down to a few differing instructions and your own edits stop helping, run `python tools/permute.py <va> --fast --anneal --jobs 1 --time-limit 600 --write-only-match`. Then move on; don't spend more than about 20 attempts on one function.
5. Never keep a change that makes a function that matched before stop matching.

Rules:
- Provenance: never use or copy anything from leaked source code, PDBs, linker maps, internal or debug builds, or datasets derived from them: no names, strings, comments or code. Don't look up Halo 2 symbol names online. Name things by what they do, or use the `function_<va>` and `unknown_<va>` placeholders.
- Stay inside my claim. Outside it, change only declarations, or the arguments at a call site when a fix needs it, and list every such change.
- No false matches: never swap the arguments at a call to a function that doesn't match yet, unless you also change that function's definition and every other call site the same way. The bytes can match while the code passes the wrong values.
- Don't add attributes Bungie's code wouldn't have had, and don't cast a global's own address to `volatile`.
- Never commit anything from `orig/`, `sdk/` or `build/`, or any game or SDK file. Commit with my GitHub no-reply email address.

Finish:
1. Run the full `python tools/check.py` again.
2. Compare `config/functions.csv` with `build/baseline.csv`. List every function that went from `matched` to anything else (there must be none) and every new match.
3. Write a summary for my pull request: what matches, what is near, and every change outside my claim.
```

Read what the agent did before you push. A function's bytes matching is
checked automatically, but whether the code reads like the original, and
follows the provenance rules, is up to you.

## 4. Open the pull request

- Push your branch and mark the draft pull request ready.
- Fill in the template's provenance checklist, and say what matched, what is
  near, and any change outside your claim.
- Commit with your GitHub no-reply email address
  (`<id>+<username>@users.noreply.github.com`).
- You don't need to commit `config/functions.csv`; it is regenerated when your
  pull request is merged.

We check every pull request against the current `main` and merge it with a
merge commit, so your authorship stays in the history. Questions are welcome
as issues.
