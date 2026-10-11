# Decal placement recovery: `0x17e670`–`0x17fd1f`

This work-in-progress batch covers three routines with no source in the
inventory at main `4e084bdf`. Existing projection and commit helpers outside
the range are dependencies, not claimed for edits.

| Retail entry | Bytes | Current evidence |
| --- | ---: | --- |
| `0x17e670` | 1,787 | Placement entry called by effect and decal-related source |
| `0x17ee20` | 240 | Iterates a decal chain, preparing and submitting projection work |
| `0x17ef10` | 3,591 | Preparation body recovered; behavior checked, byte matching pending |

The intervening placement-copy helper `0x17ed70` already has matching source
in `src/unknown_17d9a0.cpp`; preserve it. The three missing bodies total 5,618
retail bytes. Traversal and preparation implementations are present. The
placement entry remains stubbed. No new exact matches are claimed.

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
The traversal and preparation now share explicit layouts for these fields.

## Preparation behavior

The preparation routine transforms the placement point for its nearby-decal
check, but uses the original point when constructing the projection matrix.
When the definition's nearby limit is nonzero and the final option is zero,
it scans 24 records at decal globals `+0x380c`, each `0x2c` bytes. It rejects
placements that reach the limit of overlapping, nonexpired records with the
same tag. Otherwise it records the placement in the first empty or expired
slot, or selects the oldest frame value. A zero lifetime keeps expiry zero.
At the exact expiry time, a record is neither counted as active nor immediately
reused: counting uses `expiry > now`, replacement uses `now > expiry`.

Orientation follows the definition flags and incoming direction. It either
uses a persistent random angle, projects the direction against the plane, or
selects from eight directions in the plane basis. Both resulting tangent vectors
are normalized. The random angle retains intermediate precision through both
multiplications before its single-precision store, as retail does; rounding the
random fraction first changed matrix bytes in the initial candidate.

Preparation then selects the sequence and first frame, samples the radius,
obtains sprite bounds or builds rectangular fallback bounds, and optionally
checks texture availability. A failed lookup and failed fallback reject the
placement. Successful preparation calls the existing quad helper and initializes
the orientation bounds. Persistent random choices are reused when the state
flag is set. The matrix's scale field is not written here; it retains its
incoming value, including the zero supplied by the traversal's cleared buffer.

## Scope and validation

The compiled preparation passes 2,048 differential cases against retail,
comparing return value, the entire synthetic input/output memory region,
random state, recent-placement cache, and helper-call order. The stronger run
executes the actual plane-basis, sprite-bound and quad-preparation helpers on
both sides. Texture lookup and fallback are mocked in this run. Coverage
includes 1,221 successful preparations, 827 failures, 163 plane-basis calls,
639 sprite-bound calls, 528 texture lookups, 263 fallbacks, and 702 cache updates.
Inputs vary flags, retained random state, transformed points, normals, radius,
sequence counts and selection, sprite type, aspect handling, cache expiry,
texture residency, and lookup/fallback results. A separate run with geometry
helpers also mocked passes the same 2,048 cases. Neither establishes GPU,
streamed texture, or gameplay behavior.

The compiled traversal still passes its 216 helper-boundary cases against the
retail contract. That separate probe mocks preparation, projection, and commit;
it checks forwarding, zeroed preparation, persistent state, signed polygon
counts, sticky success, early exits, stack balance, and saved registers.

The full byte check on base `4e084bdf` preserves all 7,490 existing game matches,
including placement copy `0x17ed70`, with no gains or losses. Neither new body
is exact: traversal is 259 compiled bytes versus 240 retail; preparation is
3,453 versus 3,591. Preparation's temporary stub has been removed. The placement
entry `0x17e670` remains stubbed. No outside helper bodies or flags changed.

Sources: independent disassembly of the project's SHA-256-pinned retail
executable and the existing CC0 repository source and inventory. No leaked
symbols or proprietary SDK implementation is used. AI assistance: Codex.
This analysis is released under CC0 1.0.
