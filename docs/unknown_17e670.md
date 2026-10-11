# Decal placement recovery: `0x17e670`–`0x17fd1f`

This work-in-progress batch covers three routines with no source in the
inventory at main `4e084bdf`. Existing projection and commit helpers outside
the range are dependencies, not claimed for edits.

| Retail entry | Bytes | Current evidence |
| --- | ---: | --- |
| `0x17e670` | 1,787 | Placement entry called by effect and decal-related source |
| `0x17ee20` | 240 | Iterates a decal chain, preparing and submitting projection work |
| `0x17ef10` | 3,591 | Preparation routine called by `0x17ee20`; detailed recovery pending |

The intervening placement-copy helper `0x17ed70` already has matching source
in `src/unknown_17d9a0.cpp`; preserve it. The three missing bodies total 5,618
retail bytes. No new implementations or exact matches are claimed yet.

## Independently observed flow at `0x17ee20`

The entry aligns its stack to eight bytes and reserves `0x5938` bytes through
the compiler's stack-probe helper. An initial handle arrives in ECX, a
record pointer in EBX, and seven arguments on the stack (`ret 0x1c`).

A handle of `-1` returns false. Otherwise the loop stops if the signed short
at record `+0x20` equals `-1`. Each iteration clears a `0x110`-byte local
region and calls `0x17ef10`. A false return ends the loop. Success leads to
calls to `0x17fd20` and then `0x180d80`. The latter returns the next handle.
A positive signed short in the projection scratch region sets an accumulated
success byte; later iterations do not clear it. The loop ends when the next
handle is `-1` or an earlier exit condition applies.

Local structure names and exact argument types remain to be recovered from
the callees and the retail caller. Stack allocation size alone is not a
complete structure layout. The existing source around `0x17d9a0` supplies
independently checkable decal geometry and placement structures.

## Scope and validation

The byte checker and behavioral comparisons have not yet been run for new
code in this batch. Existing matching functions and outside callee bodies
must remain unchanged. Source work must be checked against the full matched
function set before publication; behavioral probes must distinguish actual
instructions from any helper stand-ins. No gameplay or GPU claim is made.

Sources: independent disassembly of the project's SHA-256-pinned retail
executable and the existing CC0 repository source and inventory. No leaked
symbols or proprietary SDK implementation is used. AI assistance: Codex.
This analysis is released under CC0 1.0.
