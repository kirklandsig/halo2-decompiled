# Flexible chain callbacks

**Retail range claimed: `0x115d10`–`0x11689f`**

This pass covers all 11 missing entries in the range (2,875 retail bytes).
The callback table at `0x4674a0` contains nine of the entries. Its creation,
update and attachment callbacks reach the two remaining helpers at
`0x1161f0` and `0x116750`. They share the record pool at `0x4e0334`;
its initialization uses the retail string `antenna` at `0x4532f0`.
The pool holds 12 records of 0x2bc bytes, with a chain of 0x20-byte nodes.

The preceding device callback group ends before `0x115d10`. The next
callback group starts at `0x1168a0` and uses a separate pool and table.
Issue #9 and open pull requests were checked before this claim; no active
claim overlaps this range. The adjacent cloth pass is PR #48.

The published draft claim preceded source work. Names describe retail
behavior or use address placeholders; evidence comes from the retail
executable and existing project declarations.

## Recovered behavior

The callbacks allocate, reset, disable and clear the pool; create and release
records; update active records; catch up a record after idle time; and submit
its origin to the render queue. Clearing the pool pointer does not free it
in this retail routine.

Each record contains 21 nodes. Creation starts at the origin and accumulates
the definition's segment offsets. Segment records carry a rest length,
stiffness and optional bitmap-sequence index. Texture scaling uses the first
frame's horizontal extent, the selected bitmap's width and the border size.
The terminal node's position and velocity are initialized, while its other
fields retain their previous contents, just as in retail.

Periodic updates increment idle ticks on non-fixed records. Records with an
object and fewer than five idle ticks receive a step capped at `1/15` second.
The attachment callback takes three `1/20`-second steps when idle ticks exceed
five, then resets that counter. Short counters retain retail's wraparound.

The attachment helper queries the object marker and location. If any origin
component moves by more than one unit, it translates all nodes by that
movement. It always records the new origin. The solver anchors the first node,
optionally applies the existing physics dependency to later nodes, constrains
each segment's length, blends against the preceding rotated rest offset, and
updates velocity from the change in position. Zero and negative timesteps
still refresh the attachment, but skip node integration.

## Scope and dependencies

All new chain code lives in `src/flexible_chain.cpp` and its own dependency
stub file. The two-line cache integration adjustment below is the only
change to existing source. No shared header, other file's compiler flag or
inventory changes are part of the contribution. `g_4e0334` holds the pool and `g_4674a0` reproduces
the nine populated callback slots and three null slots.

The only new dependency stub is render submission at `0x47870`. The existing
physics stub at `0x211060` remains a dependency. Record-pool operations,
bitmap lookup, object-marker query, location query and vector-angle helpers
reuse the existing project declarations and implementations.

Retail submits an empty two-argument callback at `0x24dc50`. That shared body
already appears upstream as a virtual method. A local ordinary callback
provides the queue's function-pointer signature; its linked body is checked
against retail's three-byte `ret 8` body. It has no additional retail marker.

The record layout is 0x2bc bytes, with nodes at +0x1c and 0x20-byte stride.
Definitions hold stiffness at +0x64 and the segment count/pointer at +0x98/+0x9c.
Segments have 0x80-byte stride, length at +0x24, sequence index at +0x28, and
rest offset at +0x74. The frame and bitmap structures are partial views of
only the fields used by creation.

## Cache integration adjustment

Adding creation at `0x115da0` changes the compiler's argument-register choice
for the existing cache helper at `0x214ac0`. Without an adjustment, its caller
at `0x214b80` loses its exact match. The helper's two parameters are reordered,
and its sole caller passes them in the corresponding order. Both parameters
remain `long`; the function bodies and logic are otherwise unchanged.

The retail call graph has only that caller, and scanning the executable
found no stored absolute references to the helper. The adjustment restores
retail's argument registers and preserves the caller's exact match. The
helper and caller both retain their exact matches on the updated baseline.
The change is confined to two lines in `src/unknown_214c30.cpp`. Banshee64
approved this integration adjustment in lane T's range.

## Matching results

A full implementation pass on `d0f8b75c7f07f3c121ddc9a50bf0e77af4d76849`
reports 5,521 matches, preserving all 5,514 matches in upstream's committed
baseline. Seven of the eleven new functions match
exactly; the other four differ and are not reported as near matches.

| Retail address | Role | Retail bytes | Linked bytes | Result |
| --- | --- | ---: | ---: | --- |
| `0x115d10` | Pool initialization | 64 | 64 | Exact |
| `0x115d50` | Pool reset | 18 | 18 | Exact |
| `0x115d70` | Pool disable | 10 | 10 | Exact |
| `0x115d80` | Clear pool pointer | 20 | 20 | Exact |
| `0x115da0` | Create chain | 585 | 585 | Differs |
| `0x115ff0` | Release record | 18 | 18 | Exact |
| `0x116010` | Catch up attachment | 112 | 112 | Exact |
| `0x116080` | Submit render callback | 61 | 61 | Differs |
| `0x1160c0` | Update active records | 289 | 289 | Differs |
| `0x1161f0` | Advance chain | 1368 | 1360 | Differs |
| `0x116750` | Read attachment and translate nodes | 330 | 330 | Exact |

## Behavioral validation

- 39 compiler assertions verify record sizes, field offsets and shared views.
- 1,275 retail-versus-linked chain comparisons cover all eleven entries:
  allocation failure, invalid handles, empty through maximum-size chains,
  bitmap sequence bounds and scaling, lifecycle calls, render submission,
  translation thresholds, idle-counter boundaries and wraparound, timestep
  clamps, fixed records, complete update/catch-up paths and randomized solvers.
- A further 1,764 cache comparisons cover the helper and its actual caller,
  including invalid indices, timestamps before/equal/after the current time,
  excluded slots, and file-size limits. Return values and time-query traces
  agree with retail.
- The nine callback-table addresses and three null slots are checked against
  retail. Render submission verifies the empty callback's linked machine body.

The chain fixtures observe no nonzero floating-point error. Integer fields,
untouched bytes, call traces and stack cleanup agree. These are controlled
execution comparisons, not in-game tests: allocation, object-marker queries,
location queries, physics and render submission use fixture implementations
at those dependency boundaries. Record-pool operations, bitmap lookup and
vector-angle computation execute their actual bodies where applicable. Cache
tests control the public clock/comparison APIs and execute both cache routines.
The rendering queue and physics integration dependencies remain incomplete in
the project; these tests do not establish their runtime behavior.
