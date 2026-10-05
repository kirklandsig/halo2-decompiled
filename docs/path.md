# Path search recovery

## Claim and boundaries

Retail ranges: **`0x270590`–`0x2712ff` and `0x2713c0`–`0x2729af`**.
This is the path-search cluster: 18 inventory
entries, 8,939 retail bytes. Lane C's existing initializer `0x271300`
through `0x2713bf` is explicitly excluded and remains untouched.

Keep `src/unknown_271e50.cpp` unchanged, including its initializer,
differing `function_271e50` (`0x271e50`) and matched
`function_271ef0` (`0x271ef0`). All 16 other entries now have source.
The 18 claimed entries contain five exact matches and thirteen with byte
differences.
The preceding obstacle-query helper `0x270400` and following scenario
starting-location lookup `0x2729b0` are outside this claim. This does not
claim other path-related routines or scattered initializers.

Issue #9 and every open PR were checked before claiming. No claimed lane
range overlaps these two ranges. Existing lane C stubs within the claim
are replaced as their implementations land: `0x270750`, `0x2715a0`, and
`function_272700` at `0x272700`. Parameter types are retained.
The approved return-type correction for `0x270750` is described below.

## Evidence

The setters at `0x270590`, `0x2705c0` and `0x2713c0` are identified by their
retail field stores and the established path state/source layouts, not by
address order alone. Retail copies a 16-byte start or destination value and
has an extra attractor flag. The 16-byte value is the existing `s_type_c3b527`
(actor movement stores one at +0x4ec, and its caller passes that address to
the destination setter). Unknown fields keep neutral names.

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
| `0x270750` | 467 | 450/467 bytes; conventions, FP order, and layout |
| `0x270930` | 1116 | 1119/1116 bytes; conventions, stack frame, and scheduling |
| `0x270d90` | 1388 | 1288/1388 bytes; conventions, register allocation, and layout |
| `0x2713c0` | 46 | Exact match |
| `0x2713f0` | 425 | 425/425 bytes; registers, scheduling, and constants |
| `0x2715a0` | 135 | 137/135 bytes; scheduling and short-field updates |
| `0x271630` | 2076 | 2044/2076 bytes; layout, FP order, and helper calls |
| `0x271e50` | 150 | Existing todo; preserve |
| `0x271ef0` | 221 | Existing matched; preserve |
| `0x271fd0` | 65 | 65/65 bytes; registers |
| `0x272020` | 1760 | 1600/1760 bytes; registers, layout, and dependency convention |
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
`s_type_136112`/`s_type_f17a25` views. There are no globals or external calls in
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
- `function_271fd0`: 65/65 bytes, first difference +1. If signed heap
  count is below 1024, increment it, write node/cost at the old index,
  then call the existing `function_271e50`. Only register allocation
  differs (including the existing callee's selected argument register).
  That helper's source and flags remain unchanged.
- `function_272740`: 216/206 bytes, first difference +3.
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

- `function_270640` (`0x270640`): 243/263 bytes, first difference
  +7. Finds a node with the existing hash lookup, then walks its parents
  while coordinate-space indices agree and a segment trace reports clear.
  At a root it returns the source's start point with `arg_c793c4 = true`;
  otherwise it returns the current entry point with `arg_c793c4 = false`.
  A missing hash key returns false without touching either output.
  Argument registers/stack cleanup, the trace stub convention, early
  failure, and loop/return layout differ from retail.
- `function_272810` (`0x272810`): 408/402 bytes, first difference +3.
  Converts the endpoint and attractor into the start point's space through
  existing `function_210690`, ignoring its boolean result as retail does.
  Uses retail's `(start-attractor)` projection and endpoint policy. Returns
  zero weight and `FLT_MAX` distance unless squared distance is strictly
  below squared radius. Inside the radius, writes sqrt(distance squared)
  and returns `(1 - distance/radius) * field_40` using x87 arithmetic.
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

## Fourth batch results

`function_270750` corresponds to the old `path_distance_estimate`
helper. It finds the requested node, adds its stored path distance to the
distance from its entry point to the target, and optionally returns an
attractor distance and normalized travel direction. Requesting direction
rebuilds child links along the parent chain and walks forward until the
accumulated entry distances reach 0.8. A missing key returns false,
`FLT_MAX` distances, and the existing default vector for optional direction.

Retail returns a boolean. The upstream stub and declaration previously
returned void. With explicit user approval, this PR changes that return type
to bool in `include/unknown_0259a0.h` and replaces the stub. All six parameter
types remain unchanged, including the two longs that hold optional pointer
values. The existing caller `0x1c0b80` ignores the return value; its source
is unchanged.

`function_2713f0` (`0x2713f0`) rejects absent pathfinding data, invalid start
node indices, and a start z that is not greater than -1000. With a destination,
it quantizes the distance by truncating distance*10 and rejects values at
least 32767. It fills the first search node, copies the start point, derives
two flags from the pathfinding node, sets closest-node information when a
destination exists, records the hash entry, and calls the existing heap
insertion helper. It does not clear unrelated state bytes.

Both new routines remain checker `todo`: `function_270750` is 450/467
bytes, first difference +4; `function_2713f0` is 425/425 bytes, first
difference +0. The former differs in argument registers and stack cleanup,
branch layout, x87 sum order, and scheduling. The initializer differs in
register allocation, scheduling, and loading the exact multiplier 10 as a
double rather than a float. Existing helper sources and flags are unchanged.

Full check5 against `6f40393`: **4,337 game / total matches**, no upstream
or earlier path match lost. All 58 SDK layout assertions pass. The existing
caller `0x1c0b80` compiles and links to the recovered function, passes all six
arguments, and ignores the result. It was already checker `todo` and remains
so; its source is unchanged. Two incremental full checks covered this batch,
first the initializer and then the approved return correction and body.

Isolated machine-code comparisons:

- 384 estimated-distance cases agree, including missing keys, optional
  outputs, enabled/disabled attractors, cached distance minima, roots and
  parent chains, and entry distances below/at/above 0.8. Real helper bodies
  execute; direction-helper arguments and all state/output bytes agree.
- 80 initializer cases agree, covering absent pathfinding data, invalid
  indices, the strict z cutoff and NaN, and enabled/disabled destinations.
- Of 32 distance/sector-flag cases, eight differ with the existing distance
  helper `0x210970`. For length 3276.5, retail quantizes to 32764 and this
  build to 32765. At the float representation of 3276.7, retail rejects
  while this build accepts. The helper uses SSE squared-distance arithmetic
  in retail and x87 arithmetic in the current source; it already has byte
  differences and is outside this claim.
- All 32 cases agree when both initializers receive identical distances at
  that helper boundary. This isolates the observed cutoff difference to
  the dependency; it does not establish general floating-point equivalence.

The emulator uses fresh instances when installing replacement hooks, so
previously translated code cannot bypass them. No in-game runtime tests.
The distance-helper discrepancy remains documented for future recovery.

## Fifth batch results

`function_2715a0` resets node/heap counts and the hash table, clears closest-node information,
then calls the initializer and traversal. On failure, it advances the
signed location mask: zero becomes 32; other values increment, wrapping to
zero above 32. It retains upstream's `bool(byte *)` declaration and replaces
its lane C stub. The checker reports 137/135 bytes, first difference +47:
boolean initialization scheduling and loading/updating the short mask differ.

`function_271630` (`0x271630`) removes nodes from the heap, stops at a
matching destination sector or the distance pruning threshold, and expands
up to 64 links per sector. It filters previous sectors, flags, excluded
locations, blocked surfaces, and narrow links. It selects a midpoint or
clamped destination projection, adds path/attractor/link costs, enforces the
distance budget and quantized-cost limit, and inserts or improves open nodes.
Hash collisions wrap through 4096 slots; special links distinguish nearby
entry positions. Link types control the depth increment. The routine tracks
the closest destination point and returns whether it lies within the radius;
searches without a destination return true after exhausting the heap.

Traversal is 2032/2076 bytes, first difference +0, including jump-table data.
Registers, stack layout, loop/branch organization, floating-point operand
order, and heap insertion inlining differ. Both new routines remain checker
`todo`; no attributes, artificial callers, or flag changes were added.

`src/stubs/path.cpp` temporarily supplies claimed routine `0x272020` as
`function_272020`, pending its recovery. Retail passes four stack
arguments (pathfinding data, current node, output links, state) and cleans
16 bytes, so this stub uses that convention. This stub means
normal execution cannot yet expand links. The earlier trace stub `0x26c4e0`
also remains. No further shared-header changes were made.

Validation after rebasing onto upstream `d66e2fc`:

- Full check6: **4,482 game / total matches**, versus 4,478 upstream; no
  upstream or earlier path match lost. All four new exact matches survive.
- All 79 original-compiler layout assertions pass, including the 0x30-byte
  link, its flags/types/point/vector, input controls, and closest-node fields.
- 950 isolated retail/linked machine-code comparisons agree. Cases cover
  empty/exhausted heaps, destination hits/pruning, all link types, penalties,
  depth branches, link filters, location masks, distance limits, open/closed
  node updates, projection clamps, attractors, hash collisions, allocation
  capacity, cost rejection, and wrapper failure-mask cycling.
- Link expansion was intercepted with identical controlled records; the
  pathfinding/node/state arguments and expansion sequence were checked.
  The surface-blocking query was intercepted with chosen responses. Other
  helper bodies ran normally. Two unspecified padding bytes in copied
  node points were excluded from state comparisons. These tests validate
  traversal independently of the missing link builder.
- Existing callers `0x1c0b80` and `0x1c2130` link to the recovered wrapper;
  one ignores the result and the other consumes it. Their source and the
  initializer/heap source remain unchanged. Both callers were already `todo`.
- One implementation/full build for this batch. No in-game runtime tests;
  earlier floating-point limitations still apply.

## Sixth batch results

`function_272020` (`0x272020`) now replaces its temporary stub.
It walks the sector's surface chain, dispatches enabled surface types,
and then walks the edge ring when the sector is traversable or a root has
no surface links. The local views preserve the 16-byte edge, 20-byte surface,
and pathfinding-data offsets without changing shared types.

- Type 0 produces an ordinary edge link from its opposite sector and
  endpoint vertices. Surface-derived links set the second link flag true;
  links from the final edge ring set it false.
- Types 1/6 filter by movement settings, optional proximity, and transitions
  between coordinate spaces. Conversions use existing `function_2104b0`;
  their return values are ignored. Enabled movement types are tried in order,
  stopping at a successful feasibility check or the surface's movement mask.
  Type 6 sets the special-entry flag.
- Type 2 applies a two-dimensional proximity test for modes 1/2; mode 3
  sets the special-entry flag. Type 5 requires a matching pair of setting
  bits 0x400/0x800/0x1000 and record bits 1/2/4. Both use the existing default
  vector rather than calculating an edge vector. Types 3/4 are skipped.
- Only the low short of the link type is written. Output padding and the
  upper short retain their original bytes. Widened unsigned vertex indices
  are not rejected at 0xffff by retail's comparisons with long NONE; actual
  destination-sector sentinel checks are preserved.
- Retail can return 65 links when the surface pass fills 64 and the edge
  ring adds another: the ring checks `count == 64` after insertion. This
  behavior is retained. It requires more than the traversal's usual 64-slot
  buffer, so the boundary comparison used a larger output allocation; normal
  map-data invariants for this case have not been established.

The new function is 1600/1760 bytes, first difference +2, checker `todo`.
Registers, stack layout, branch organization, redundant widened-index tests,
point-vector load scheduling, and the transition dependency convention differ.
Its normal C++ declaration lets LTCG select four stack arguments and 16-byte
cleanup, matching retail without the old stub's explicit `__stdcall`.
Traversal is now 2044/2076 bytes after linking the real callee; earlier exact
matches and all other earlier path function sizes are unchanged.

Added only `function_26f150`, a missing transition-feasibility dependency,
to `src/stubs/path.cpp`. It accepts a movement type, start/end points, and
two optional alternate points; this caller passes NULL for both alternates.
Retail uses type EAX, end ECX, alternate start EBX, and two stack arguments;
the stub's standard fastcall convention differs. This helper is outside the
claim and remains unrecovered, as does trace dependency `0x26c4e0`.
No shared headers or other files' flags changed.

Validation after rebasing onto upstream `d7c29cc`:

- Full check9: **4,496 game / total matches**, none lost against upstream
  or previous batches (4,492 upstream matches). All 102 original-compiler layout assertions pass.
- 1,264 link-builder comparisons agree on the return value and every output
  byte, including untouched padding. Cases cover surface dispatch, flags,
  signed masks, distance tests, coordinate conversion, transition success
  and early rejection, missing sectors, widened vertex indices, edge-ring
  direction/fallback, and the 63/64/65-link boundary.
- Conversion and transition dependencies were intercepted for those tests;
  complete arguments, order, ignored conversion failures, and movement-type
  attempts were checked. This validates the builder, not the missing helper.
- 576 integrated search cases agree with no dependency hooks, running the
  real wrapper, initializer, traversal, builder, heap, and math routines over
  two-, three-, and five-sector chains. Cases include goals, unreachable
  targets, blocked sectors, distance budgets, radii, and attractors.
- All 950 earlier controlled traversal/wrapper cases still agree after the
  callee replacement. Copied node-point padding is excluded only in these
  traversal/integration checks, as in the fifth batch.
- One body implementation; a final full check followed comment/include
  cleanup and another followed the upstream rebase. No in-game tests. Earlier floating-point limitations still apply,
  and dynamic transitions depend on the unrecovered feasibility helper.

## Result construction

The final two entries now have source. `function_270d90` reconstructs the
parent chain into 28-byte steps and captures up to ten link descriptors.
It preserves depth truncation, special transition handling, edge projection
with a `1.2 * radius` margin, and the 40-unit distance cap. The cap is tested
only for steps associated with a valid edge. Transition processing can write
through the parent node-key pointer; the later walk reads the modified key.
Unsigned vertex words retain retail's ineffective comparison with long NONE.

`function_270930` (`0x270930`) selects
the destination or closest node, reconstructs up to 40 steps, and calls the
smoothing and finalization dependencies. Closest-node acceptance uses a
strict radius comparison. Actor flag +0x478 bypasses finalization and leaves
the result's object/type fields untouched. The result preserves location
entries, retry-mask cycling, completion state, and the final endpoint/distance.
A type-6 result can notify the actor; its timer uses x87 `fistp` rounding.

The result view is 0xc4 bytes: destination +4, node key +0x14, distance +0x18,
start +0x1c, completion/count/index +0x2c/+0x2d/+0x2e, four steps +0x30,
object/type/flag +0xa0/+0xa4/+0xa6, and location +0xa8. These are local views;
shared `path.h` remains unchanged.

Three additional dependencies remain stubs in `src/stubs/path.cpp`:

- `0x26f3f0`: transition-point construction, including parent-key/output pointers.
  Retail uses three registers plus seven stack arguments; the stub's fastcall
  convention differs.
- `0x2c2060`: smoothing, six stack arguments with 24-byte cleanup.
- `0x2c41b0`: finalization, eleven stack arguments with 44-byte cleanup.

The latter two use `__stdcall` to retain their observed retail convention.
Existing actor notification, point conversion, normalization, and point
separation routines are reused without declaration or source changes.

Final validation on upstream `8ad5b57`:

- Full check11: **4,912 game / total matches**, preserving all 4,908 upstream
  matches and all four earlier new matches. All 18 claimed entries have
  source: five exact matches, thirteen `todo` entries with byte differences.
- `0x270d90`: 1288/1388 bytes, first difference +2. `0x270930`: 1119/1116
  bytes, first difference +0. The former uses ten stack arguments in this
  build versus retail's state register and nine stack arguments. The latter
  uses an aligned EBP frame and three stack arguments versus retail's state
  register and two stack arguments. Register allocation, scheduling, branch
  layout, and dependency conventions differ. Existing function sizes and
  statuses are unchanged. No flag tuning or forced function attributes.
- All 124 original-compiler layout assertions pass.
- 1,235 isolated reconstruction comparisons agree on the return value,
  complete output buffers, state writes, and intercepted dependency calls.
  Coverage includes depth/step/link limits, transition types 1/2/5/6,
  parent-key alias writes, projection clamps, coordinate conversion failures,
  distance-cap boundaries, 40/41/50-node chains, optional outputs, and
  128 seeded three-dimensional projection samples.
- 1,255 result-assembly comparisons agree on return/result/state bytes and
  dependency arguments. These run the recovered reconstruction routine;
  smoothing/finalization are intercepted with controlled outputs. Cases
  include missing destinations, strict closest-node acceptance, NONE indices,
  zero/four steps, actor bypass, retry flags, ordered first/last/middle links,
  truncation, and notification arguments/tick rounding. Only the two
  unspecified bytes at step +0xa are excluded from comparisons of the
  uninitialized local reconstruction array passed to smoothing.
- The previous 1,264 link-builder cases, 950 controlled traversal cases,
  and 576 integrated searches without dependency hooks still pass on the
  final linked image. Their previously documented padding exclusions apply.
- Two full builds for this final implementation: the second aligns the two
  post-processing stubs with retail's stack convention and preserves integer
  promotion before clamping the reconstruction initialization count.
  No new shared-header or other source-file flag changes.

## Review status

All claimed entries are represented by source. The unmatched routines retain
retail markers for later matching work. The five dependency stubs and earlier
floating-point/capacity limitations remain; there has been no in-game test.
