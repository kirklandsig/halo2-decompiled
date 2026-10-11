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
retail bytes. A first traversal implementation is present; preparation
is still an explicit stub. No new exact matches are claimed.

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

The four calls in `0x17e670` establish the seven stack arguments, in order:
mesh pointer, optional transform, direction pointer, radius, a flag, a sequence
selection, and a final option whose meaning is not yet fully recovered.
The initial tag and placement record arrive in ECX and EBX. `0x17ef10` receives
the transform in ECX and nine stack arguments: tag, placement, direction,
radius, flag, sequence selection, final option, preparation, persistent state.

The local buffers are distinguishable by their accesses, not just the frame
size. The persistent state occupies 28 bytes, the cleared preparation occupies
272 bytes, and the output occupies `0x5804` bytes. The output's polygon count
is the signed short at `+0x5802`, initialized before each projection call.
Only preparation is cleared each iteration; persistent state survives calls.

Preparation starts with the existing `s_decal_projection_17fd20` layout
(`0xec` bytes). Its remaining independently observed fields are:

| Offset | Access and evidence |
| --- | --- |
| `0xec` | Four floats passed as texture bounds to `0x17cb00` |
| `0xfc` | Signed short sequence index, selected and clamped at `0x17fa14`–`0x17fa94` |
| `0xfe` | Signed short frame index, initialized to zero at `0x17faaf` |
| `0x100` | Signed short bitmap index, loaded from the selected sprite frame at `0x17fb87` |
| `0x104` | Bitmap-group pointer obtained from the tag table at `0x17f9fc`–`0x17fa0e` |
| `0x108` | Selected sequence pointer, using a `0x3c`-byte stride |
| `0x10c` | Selected sprite-frame pointer, using a `0x20`-byte stride |

The persistent state's first byte controls reuse of random choices. Preparation
uses float `+4` for radius interpolation, float `+0x14` as an angle passed to
sine/cosine, and dword `+0x18` for a sequence choice. The existing commit helper
uses floats `+8`, `+0xc`, and `+0x10` and updates the first byte. These accesses
explain the full 28-byte allocation despite the commit helper's shorter type.
The traversal currently leaves these layouts opaque until preparation recovery.

## Scope and validation

The first compiled traversal passes the same 216 helper-boundary cases as
retail: argument forwarding, preparation-buffer clearing, persistent state,
signed polygon counts, sticky success, early exits, stack balance, and saved
registers. Preparation, projection, and commit are mocked in both runs; their
implementations, graphics output, and gameplay are outside this probe's scope.

The full byte check on base `4e084bdf` preserves all 7,490 existing game matches,
including placement copy `0x17ed70`, with no gains or losses. The traversal is
not an exact match: 260 compiled bytes against retail's 240. Preparation remains
an explicit `@stub` in `src/stubs/lane_r.cpp`, built without LTCG as required
by the repository. Its calling convention therefore differs from the retail
internal helper. Further byte tuning should wait for preparation recovery.
The placement entry `0x17e670` also remains stubbed. No outside helper bodies
or flags were changed.

Sources: independent disassembly of the project's SHA-256-pinned retail
executable and the existing CC0 repository source and inventory. No leaked
symbols or proprietary SDK implementation is used. AI assistance: Codex.
This analysis is released under CC0 1.0.
