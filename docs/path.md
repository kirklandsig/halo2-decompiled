# Path search recovery

## Claim and boundaries

Retail ranges: **`0x270590`–`0x2712ff` and `0x2713c0`–`0x2729af`**.
This is the surviving path-search cluster from `path.obj`: 18 inventory
entries, 8,939 retail bytes. Lane C's existing initializer `0x271300`
through `0x2713bf` is explicitly excluded and remains untouched.

Keep `src/unknown_271e50.cpp` unchanged, including its initializer,
near `path_heap_bubble_up` (`0x271e50`) and matched
`path_heap_bubble_down` (`0x271ef0`). Nine of the other 16 entries now have source: four exact and five with
byte differences; seven remain unwritten.
The preceding obstacle-query helper `0x270400` and following scenario
starting-location lookup `0x2729b0` are outside this claim. This does not
claim all historical path-related routines or scattered initializers.

Issue #9 and every open PR were checked before claiming. No claimed lane
range overlaps these two ranges. Existing lane C stubs within the claim
will be replaced with their exact published signatures when implemented:
`0x270750`, `0x2715a0`, and `path_node_from_hash_table` at `0x272700`.

## Mapping evidence

The retail map explicitly names `path_heap_bubble_up` at `0x271e50`,
`path_heap_bubble_down` at `0x271ef0`, and `path_node_from_hash_table` at
`0x272700` in `path.obj`. Both 2003 maps identify the same three routines
in that object file:

| Routine | 2003 profile | 2003 debug | Retail |
| --- | --- | --- | --- |
| path_heap_bubble_up | `0x135110` | `0x236d00` | `0x271e50` |
| path_heap_bubble_down | `0x1351b0` | `0x236f80` | `0x271ef0` |
| path_node_from_hash_table | `0x1354d0` | `0x2377c0` | `0x272700` |
| path_input_set_start | `0x134e80` | `0x236a00` | `0x270590` (inferred) |
| path_input_set_attractor | `0x134eb0` | `0x236a30` | `0x2705c0` (inferred) |
| path_state_destination | `0x1350e0` | `0x236c80` | `0x2713c0` (inferred) |
| path_heap_insert | `0x135290` | `0x2373b0` | `0x271fd0` (inferred) |
| closest_point_to_attractor | `0x135510` | `0x237810` | `0x272740` (inferred) |
| path_state_approach_point | `0x1356d0` | `0x237910` | `0x270640` (inferred) |
| path_attractor_weight | `0x135620` | `0x2384d0` | `0x272810` (inferred) |

The three setter mappings use retail field stores and the established path
state/source layouts, not address order alone. Retail copies a 16-byte start
or destination value and has an extra attractor flag; older point types and
signatures cannot be copied unchanged. The 16-byte value is the existing `s_node_point` (actor movement stores one
at +0x4ec, and its caller passes that address to the destination setter).
Unknown fields keep neutral names.

Symbol names and map associations are from
[halo-symbol-atlas](https://github.com/tinkerer-red/halo-symbol-atlas),
CC BY 4.0. Map hashes:

- Retail: `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`
- 2003 profile: `4f4f09b181eec4a434418b38efe581e75aaf3047c24add8a712751d6ae0d34d3`
- 2003 debug: `96ea21d862dfe6a0bebb23e1a4311202a6e18a79970189a4df320d4ededa439d`

[Halo CE's path.h/path.c](https://github.com/punpckhdq/halo/tree/master/source/ai)
(CC0) provide terminology and hash constants (511, 8, 4095). The retail
instructions determine this implementation's types, offsets, and behavior.

## Inventory

| Retail | Bytes | Current state |
| --- | ---: | --- |
| `0x270590` | 41 | Exact match |
| `0x2705c0` | 58 | Exact match |
| `0x270600` | 63 | 63/63 bytes; registers and return layout |
| `0x270640` | 263 | 243/263 bytes; conventions and layout |
| `0x270750` | 467 | Unwritten |
| `0x270930` | 1116 | Unwritten |
| `0x270d90` | 1388 | Unwritten |
| `0x2713c0` | 46 | Exact match |
| `0x2713f0` | 425 | Unwritten |
| `0x2715a0` | 135 | Unwritten |
| `0x271630` | 2076 | Unwritten |
| `0x271e50` | 150 | Existing todo; preserve |
| `0x271ef0` | 221 | Existing matched; preserve |
| `0x271fd0` | 65 | 65/65 bytes; registers |
| `0x272020` | 1760 | Unwritten |
| `0x272700` | 57 | Exact match |
| `0x272740` | 206 | 216/206 bytes; FP ordering and scheduling |
| `0x272810` | 402 | 408/402 bytes; FP ordering and scheduling |

## First batch results

All four new routines match exactly on the first implementation and build:
start setter (41 bytes), attractor setter (58), destination setter (46), and
hash lookup (57), totaling 202 new matched bytes. Including the existing
heap helpers, six of the 18 claimed entries have source and five match.

The hash starts at `(node_index & 511) * 8`, probes signed short entries,
wraps with `& 4095`, and stops at NONE or a node with the requested key.
Retail's hash table begins at state +0x120b6; node keys are at
state +0xb8 + index*0x44. The local lookup view covers the full nodes,
1025 heap entries, and hash table without altering the existing partial
`path_node`/`path_state` views. There are no globals or external calls in
these four routines.

Validation against upstream `6f40393`:

- Full baseline: 4,333 game / total matches.
- Full implementation check: **4,337 game / total matches**, none lost.
- All 27 SDK layout assertions pass, including the shared point type,
  setter fields, node stride, heap count, heap, and hash table offsets.
- Linked code sizes and return stack cleanup agree with retail; the hash
  masks, node stride, and memory offsets were independently inspected.
- Existing initializer and heap source, shared headers, and other flags
  remain unchanged. Only the existing `0x272700` stub was removed.
- No game runtime tests.

## Second batch results

Three more routines are implemented. The checker records all three as
`todo` with byte differences; no earlier match was lost.

- `function_270600`: 63/63 bytes, first difference +0. Tests a location's
  signed count, entries with a NONE short field and matching node key,
  and the corresponding active bit in its signed-short mask. The local
  view is 0x1c bytes: mask +0, count +2, three 8-byte entries +4 (short
  field +0, key +4). Registers, initial boolean setup, alignment padding,
  and return blocks differ. The original name is unknown.
- `path_heap_insert`: 65/65 bytes, first difference +1. If signed heap
  count is below 1024, increment it, write node/cost at the old index,
  then call the existing `path_heap_bubble_up`. Only register allocation
  differs (including the existing callee's selected argument register).
  That helper's source and flags remain unchanged.
- `closest_point_to_attractor`: 216/206 bytes, first difference +3.
  Uses `delta = end - start`. For positive squared length, retail's
  numerator is `(start-attractor) dot delta`, summed in y/z/x order.
  A parameter below zero or above one selects `end`; otherwise it
  interpolates from `start`. Nonpositive/unordered squared length selects
  `start`. This unusual numerator sign and endpoint policy are preserved.
  The compiler sums squared length in z/y/x order, versus retail x/y/z,
  and uses different SSE registers, loads, interpolation operands, and a
  longer conditional jump. Rounding equivalence is not established.

Full original-compiler check2 against `6f40393`: **4,337 game / total
matches**, no upstream or first-batch match lost. All 37 layout assertions
pass. No additional stubs, shared-header changes, or other files' flag
changes were needed. One implementation/build for this batch; stopped at
register/operand differences under the decompilation guidelines.

Isolated x86/SSE emulation compared retail and linked code:

- 240 location cases agree, covering signed/empty counts, first/last hits,
  key mismatch, NONE fields, and active/inactive masks.
- 20 heap cases agree, executing each image's real bubble-up helper;
  includes empty heap (count 1), full capacity, and low/equal/high costs.
- 32 closest-point boundary/alias cases agree, including zero-length
  segments, parameters 0/1 and outside that interval, NaNs, and output
  aliasing each input.
- In 200 deterministic random finite cases, six closest-point results
  differ in their bits. The largest absolute coordinate difference in
  that sample is `9.5367431640625e-7`, consistent with the changed sum
  order. This sample is not a bound or an exhaustive equivalence proof.

These are isolated machine-code checks; no in-game runtime tests were run.
Emulation dependencies and evidence remain local and are not added to the
repository.

## Third batch results

Two more routines are implemented, both currently checker `todo`:

- `path_state_approach_point` (`0x270640`): 243/263 bytes, first difference
  +7. Finds a node with the existing hash lookup, then walks its parents
  while coordinate-space indices agree and a segment trace reports clear.
  At a root it returns the source's start point with `at_start = true`;
  otherwise it returns the current entry point with `at_start = false`.
  A missing hash key returns false without touching either output.
  Argument registers/stack cleanup, the trace stub convention, early
  failure, and loop/return layout differ from retail.
- `path_attractor_weight` (`0x272810`): 408/402 bytes, first difference +3.
  Converts the endpoint and attractor into the start point's space through
  existing `function_210690`, ignoring its boolean result as retail does.
  Uses retail's `(start-attractor)` projection and endpoint policy. Returns
  zero weight and `FLT_MAX` distance unless squared distance is strictly
  below squared radius. Inside the radius, writes sqrt(distance squared)
  and returns `(1 - distance/radius) * attractor_weight` using x87 arithmetic.
  SSE sums, register/stack scheduling, and x87 division/constant loading
  differ. Rounding equivalence is not established.

Local node fields now expose parent +2 and the 16-byte entry point +0x18,
retaining the 0x44 stride and hash key +8. No shared layout was edited.
A new `src/stubs/path.cpp` supplies only dependency `0x26c4e0`, the segment
trace wrapper which calls `0x26c590` in retail. It is outside this claim and
its implementation is still missing. Its declaration carries the two
node points, existing 0x24-byte trace result, pathfinding pointer, both node
indices, and flags. No existing dependency signature or flag was changed.

Full check3 against `6f40393`: **4,337 game / total matches**, no upstream
or prior path match lost. All 41 SDK layout assertions pass. One source
implementation/full build in this batch.

Isolated machine-code comparisons:

- 64 approach cases agree, including missing keys (outputs untouched),
  roots, coordinate-space mismatches, and blocked/clear parent segments.
  The trace dependency was intercepted with chosen responses; complete
  argument lists and call sequences were compared. This checks the caller,
  not the missing trace implementation.
- 150 attractor boundary cases agree, using the actual point-conversion
  routine with output index NONE: degenerate segments, projection endpoint
  choices, zero/negative/positive radii, strict radius boundaries, and weights.
- 32 more cases agree with intercepted successful/failed coordinate
  conversion. Both calls receive the expected index and point, in order;
  failure does not stop the caller.
- 21 of 200 deterministic random finite attractor cases differ bitwise.
  Largest observed absolute differences: weight `2.384185791015625e-7`,
  distance `4.76837158203125e-7`. These samples are not error bounds.

No in-game runtime tests. The new trace stub remains a dependency to
replace when its implementation becomes available.

## Remaining work

Seven entries remain unwritten: `0x270750`, `0x270930`, `0x270d90`,
`0x2713f0`, `0x2715a0`, `0x271630`, `0x272020`.
Next, recover `0x270750` and `0x2713f0`, building on the recovered helpers.
Preserve the five exact matches and the existing near heap helper. Replace
only the remaining claimed stubs (`0x270750`, `0x2715a0`) when their real
implementations are ready. Keep this PR draft during recovery.
