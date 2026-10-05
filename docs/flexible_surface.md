# Cloth simulation and rendering pass

Retail range claimed: `0x1168a0`–`0x11850f`.

The retail callback table at `0x4674d8` identifies this group. Its entries
include creation (`0x116980`), update (`0x1168a0`), rendering (`0x116b00`)
and render submission (`0x117060`). They share the record pool `g_4e0338`;
its existing initializer uses the retail string `cloth` and a record size
of `0x1258`. Calls from those callbacks reach the remaining helpers through
`0x118430`. The next routine, `0x118510`, starts a separate call graph and
is outside this claim.

There are 21 discovered entries and 7,138 retail bytes in the range. Six
lifecycle/accessor routines already live in `src/unknown_1169f0.cpp` and
are preserved: `0x1169f0`, `0x116a10`, `0x116a50`, `0x116a70`,
`0x116a80`, and `0x116ac0`. This pass recovers the remaining 15 functions.
The shared callback table now includes their four recovered entries.
The shared callback at `0x24dc50` is outside the claim.

## Evidence and scope

The range is based on retail callback pointers, direct calls, and shared
pool and tag-data references. Names introduced by this pass describe the
observed behavior or use address placeholders. Issue #9 and all open PRs
were checked before claiming; no active range overlaps this pass.

The draft claim was published before implementation. The results below are
from the complete pass on upstream `9b278b5`.


## Recovered functions

| Address | Behavior | Built / retail bytes | Result |
| --- | --- | ---: | --- |
| `0x1168a0` | Update all active records | 218 / 218 | Differs |
| `0x116980` | Allocate and initialize a record | 106 / 106 | Exact match |
| `0x116b00` | Render both sides of the surface | 1585 / 1361 | Differs |
| `0x117060` | Submit the render callbacks | 24 / 24 | Differs |
| `0x117080` | Retire an inactive object attachment | 128 / 128 | Differs |
| `0x117100` | Test visibility for the current camera | 156 / 153 | Differs |
| `0x1171a0` | Sort markers and assign attachment vertices | 573 / 573 | Differs |
| `0x1173e0` | Initialize vertices and settle constraints | 296 / 296 | Differs |
| `0x117510` | Accumulate and normalize vertex normals | 653 / 633 | Differs |
| `0x117790` | Relax distance constraints | 745 / 741 | Differs |
| `0x117a80` | Follow the object and pin marker vertices | 511 / 511 | Differs |
| `0x117c80` | Limit vertical direction and normalize horizontally | 347 / 333 | Differs |
| `0x117dd0` | Advance shared wind state | 880 / 880 | Differs |
| `0x118140` | Integrate unpinned vertices | 664 / 744 | Differs |
| `0x118430` | Reset vertices to the rest configuration | 206 / 224 | Differs |

The 15 recovered entries cover 6,925 retail bytes. Creation matches all 106
bytes. The other 14 retain instruction, register, stack, or dependency-call
convention differences; they are not claimed as exact or near matches.
No source-tuning flags were changed in existing files.

## Layout and behavior

The record is `0x1258` bytes:

| Offset | Field |
| --- | --- |
| `0x04`, `0x08` | Definition handle and attached object handle |
| `0x0c` | Value cleared during creation and update |
| `0x10`, `0x1c` | Previous object position and object speed |
| `0x20` | 128 vertices, each `0x24` bytes: position, previous position, normal |
| `0x1220` | Four words marking pinned vertices |
| `0x1230` | Six attachment records, six bytes each |
| `0x1254` | Signed attachment count |

The definition contains the marker identifier at `+4`, dimensions at
`+0x10/+0x12`, iteration count at `+0x1e`, and gravity, damping, wind scale,
and tangential drag at `+0x20` through `+0x2c`. Its vertex, triangle-index,
strip-index, and distance-link arrays begin at `+0x4c`, `+0x54`, `+0x5c`,
and `+0x64`. Rest vertices are 20 bytes; links are 16 bytes.

Important retail details preserved:

- Markers are sorted on the vertical axis, or the first axis when definition
  flag 1 is set. The six-entry initial marker order is `0,1,2,3,0,0`.
- Creation clears pin bits and settles constraints five times. Allocation
  failure returns `-1`; failure to find attachments releases the record.
- Constraint iterations are the definition count plus three times object
  speed, truncated and clamped to 1–15. The solver uses the retail float-bit
  square-root approximation. An unpinned endpoint receives a double
  correction when the other endpoint is pinned. Excess correction resets
  the record and repeats an iteration.
- Object displacement beyond the retail squared-distance threshold moves
  all vertices and synchronizes their history. Marker attachment positions
  are refreshed during each constraint iteration.
- Wind draws from the second random seed, shares direction and magnitude
  across records, and rotates direction only when the frame counter advances.
- Integration preserves pinned vertices. At speed 1.5 or above it copies
  current positions to history instead of advancing the simulation.
- Normal generation adds triangle contributions to the existing normals,
  then normalizes with the retail fallback direction.
- Rendering resolves the material and vertex attributes, emits a triangle
  strip twice with opposite culling and normal signs, and restores culling.
  It uses public D3D API calls, including the inlined vertex commands.

## Dependencies and shared changes

`src/unknown_1169f0.cpp` gains declarations and callback entries for
`0x116980`, `0x1168a0`, `0x116b00`, and `0x117060`. Its six existing
function bodies are unchanged. The two out-of-range callback slots are
unchanged.

Seven missing game dependencies have explicit stubs in
`src/stubs/flexible_surface.cpp`, with declarations in the new
`include/flexible_surface_calls.h`:

| Address | Observed role |
| --- | --- |
| `0x40f60` | Dispatch material passes through test/draw callbacks |
| `0xd4bc0` | Prepare/test an object render pass |
| `0x1bd50` | Select object lighting state |
| `0x1cdd0` | Set render parameters |
| `0x4b2d0` | Select a material pass |
| `0x1c6b0` | Reset pending render state |
| `0x1c710` | Apply pending render state |

These are linkage placeholders, not recovered implementations. In particular,
their standard stub conventions do not reproduce every retail internal
register convention. Existing pool, object, marker, camera, random-vector,
and texture helpers keep their upstream declarations.

New address-named storage covers shared wind state (`0x55e710`, `0x55e714`,
`0x55ecd0`, `0x55ecd4`), the render-context pointer (`0x485a80`), pending
render state (`0x51f0f0`), and the render declaration (`0x43f8df`). Their uses
and offsets were checked against retail. The renderer views virtual slot 10
through a local interface returning the reference pointer observed in retail;
the upstream interface declaration is preserved.

## Validation

A fresh baseline and full implementation check on `9b278b5` report
**5,127 and 5,128 game/total matches**, respectively. Every baseline match
is preserved, including all six existing functions in this range. All 22
new function/stub markers have unique definitions. The generated callback
table entries were checked against the retail callback addresses.

The period compiler passes **43 structure size/offset assertions**.
Retail-versus-linked execution comparisons pass **958 cases**:

| Cases | Coverage |
| ---: | --- |
| 172 | Direction limiting, including aliased output and random vectors |
| 42 | Marker sorting, axis selection, attachment counts 0–6, and grid dimensions |
| 45 | Object displacement thresholds, velocity, and vertex counts |
| 8 | Normal generation, degenerate triangles, and up to 128 vertices |
| 80 | Constraint lengths, pin combinations, iteration limits, and reset |
| 4 | Rest-configuration reset |
| 72 | Wind seeds, frame transitions, initialization, and time values |
| 48 | Integration across speed, pin, and damping boundaries |
| 12 | Object retirement flags and elapsed-time boundary |
| 16 | Camera visibility conditions |
| 32 | Creation success, allocation failure, and missing attachments |
| 6 | Complete update, including an empty pool |
| 1 | Render submission callback identities and argument |
| 20 | Both render sides, attribute subsets, empty/nonempty strips, and fallback material |
| 400 | Randomized integration and normal generation |

Compared floating-point outputs have zero observed error in these fixtures.
Other record fields, random-state changes, callback order/arguments, and stack
cleanup also agree. The render tests compare the emitted GPU command words.
The numeric comparison permits a small tolerance, but no nonzero difference
was observed.

External object marker/velocity queries, camera mode, allocation/release,
retirement, and render setup are controlled at their call boundaries where
needed. D3D vertex commands execute; device validation and cull-state calls
are isolated because there is no live graphics device. These tests validate
this pass's behavior, not the missing dependencies or the game end to end.
No in-game tests were run. The specialized material-provider path is reviewed
against its retail instructions but is not exercised by the render fixtures.
