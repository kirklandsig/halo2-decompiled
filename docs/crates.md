# Crate callback recovery

Retail range claimed: `0x11bbf0`–`0x11bd5f` (crate creation and update).

## Mapping evidence

Retail's object-type table at `0x468630`, index 11, points to the named
`crate` definition at `0x468180` (`bloc`, datum size `0x130`). Its creation
slot at `+0x2c` points to `0x11bbf0` (237 bytes), and its update slot at
`+0x40` points to `0x11bce0` (121 bytes). Both entries are untouched at
upstream `c1bcd3c`.

Crates and these routine names are absent from the consulted 2003 profile
and debug maps. The `crates.cpp`, `crate_new`, and `crate_update` names are
inferred from the named retail definition and callback roles; the original
object filename is not established. The named retail data and call graph
provide the mapping evidence for this contribution.

The creation callback clears crate flags, follows the placement definition's
model and physics-model references, and examines each `0x90`-byte rigid-body
record's short motion type at `+0x1e`. Flag bit 1 is set when the definition
flag at `+0xbc` bit 0 is set, or all examined motion types are 1 or 2
(including an empty body list). It then queries object model information
through `0x20a9a0` and succeeds only when that query succeeds and its physics
model's body count at `+0x38` is positive. The third creation argument is
unused and remains provisionally `long`.

Update checks object `+0xc0` bit 12 and its Havok component index at `+0xb4`.
When both allow it, it calls `0x1d24a0` with the component and `1.0f`.
When object `+0xc1` bit 0 is clear, it records current game time at `+0xbc`.
It returns true if either action ran. Flag and timestamp meanings remain
neutral where retail alone does not establish their original names.

The type's third callback, `0x11bd60` at `+0x4c`, is shared with machine
objects and is outside this implementation claim. PR #21 already provides
its stub. Garbage ends at `0x11bbef` (PR #20); light fixtures begin at
`0x11bdc0` (PR #22). Those ranges are excluded. Issue #9, open PRs, and the
local message board were checked for overlap.

## Recovery results

Both crate-specific callbacks are implemented in `src/crates.cpp`, along
with the actual type-definition prefix through its update slot.

| Function | Code bytes (ours / retail) | Remaining differences |
| --- | --- | --- |
| `crate_new`, `0x11bbf0` | 233 / 237 | Dependency argument registers, register/stack scheduling, first flag OR simplified to a store, loop alignment, and boolean return width/layout |
| `crate_update`, `0x11bce0` | 114 / 121 | Direct byte flag test instead of word load/shift/test, register allocation, and load scheduling |

The checker reports 237 and 121 bytes respectively because it includes four
and seven bytes of alignment padding. Both entries are labeled `todo` by
the checker; neither is an exact match. One source implementation and full
check were used. Matching stopped with the documented compiler/dependency
differences; no forced attributes, artificial callers, or flag tuning were
introduced.

Creation preserves the three `NONE` guards, definition flag, full body scan
without early exit, empty-body behavior, and the query/positive-count return.
Update preserves the two independent conditional actions and returns true
when either runs. Unknown flag and timestamp names remain neutral.

## Validation and dependency sharing

Against upstream `c1bcd3c`:

- Full original-compiler `tools/check.py`: **4,228 game / 4,228 total
  matches**, no upstream match lost.
- Thirty-four SDK compile-time assertions validate the local object, header,
  tag, model, rigid-body, model-info, component, time-global, and type layouts.
- Linked type metadata/name and both callback pointers, five global loads,
  both dependency targets, flag masks/offsets, rigid-body stride/motion-type
  comparisons, return stack cleanup, and `1.0f` call constant were verified.
- No game runtime tests were run.

The draft claim was published before source. The two external callees
`0x1d24a0` and `0x20a9a0` reuse PR #21's `src/stubs/device_machines.cpp`
**byte-for-byte at the same path**, verified against its published commit
`372ee99`. The file also includes that PR's five other stubs: `0x1d0ee0`,
`0xbf600`, `0xb9fc0`, `0xbba20`, and the shared `0x11bd60` callback.
Keeping the complete file identical lets both PRs merge without competing
versions of it. This sharing was announced on the local message board.

All stub signatures/conventions remain unchanged, and their real
implementations stay outside the claim. The `s_machine_node_matrices`
dependency type remains opaque in crate source; a separate `0x54`-byte local
view reads the physics-model pointer at `+0x48` without changing PR #21's
type definition.

Only `src/crates.cpp`, this document, and the shared stub file change.
No shared headers, upstream function bodies, other files' flags, or
inventory changes are included.

## Sources

The retail XBE supplies the disassembly and named type definition. The
[Halo Symbol Atlas](https://github.com/tinkerer-red/halo-symbol-atlas),
licensed CC BY 4.0, was searched for original names and object ownership;
no crate entries were found in the two 2003 maps:

- Retail: `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`.
- 2003 profile: `4f4f09b181eec4a434418b38efe581e75aaf3047c24add8a712751d6ae0d34d3`.
- 2003 debug: `96ea21d862dfe6a0bebb23e1a4311202a6e18a79970189a4df320d4ededa439d`.

No crate counterpart was used from Halo CE. Game and SDK files remain
outside the contribution.
