# Sound scenery recovery

Retail range claimed: `0x207b90`–`0x207c1f` (`sound_scenery.obj`).

## Mapping evidence

At base `0300b14`, the range contains two untouched entries, 139 retail bytes:

| Retail | Function | Bytes |
| --- | --- | --- |
| `0x207b90` | `sound_scenery_new` | 44 |
| `0x207bc0` | `sound_scenery_place` (inferred name) | 95 |

Retail's object-type table `0x468630` points to the named `sound_scenery`
definition at `0x4680b8`. Its creation and placement slots, `+0x2c` and
`+0x30`, hold `0x207b90` and `0x207bc0`. The creation callback sets the
shadowless object flag and returns true. Placement copies seven 32-bit
fields from scenario offsets `0x34`–`0x4c` into object offsets
`0x12c`–`0x144`. Their individual meanings are not established here.

The 2003 profile map names `sound_scenery_new` at `0x1242d0` in
`sound_scenery.obj`. The debug map names an empty `sound_scenery_delete`
at `0x24c940` in the same object. The CC0
[Halo CE reference](https://github.com/punpckhdq/halo),
`source/sound/sound_scenery.c`, corroborates the creation routine's
shadowless flag and true return. There is no distinct delete callback in
this retail range. The placement name is inferred from its type-table slot
and its scenario-to-object copies.

The preceding routine at `0x207ab0` uses unrelated per-entry strides and
state; the following `0x207c20` starts code working on different data pools
(`0x4f9384`/`0x4f9394`). Neither accesses these callback fields. Both are
excluded. Issue #9 and all open PR descriptions were checked: no overlap.

## Implementation and validation

Both callbacks are implemented in `src/sound_scenery.cpp`:

| Retail | Result |
| --- | --- |
| `0x207bc0`, `sound_scenery_place` | Exact match, 95 bytes |
| `0x207b90`, `sound_scenery_new` | 37 bytes through `ret 12`, versus retail's 44 |

The creation callback's only code difference is the shadowless flag update:
our compiler emits a direct memory OR, while retail loads the flags into a
register, adjusts the pointer, ORs the flag, and stores it back. The checker
reports a 44-byte comparison including seven trailing alignment bytes.
The same `0x10000` flag is set and the callback returns true. No forced
attributes, artificial callers, or alternate flags were added to shape this
sequence.

The creation callback retains all three stack arguments required by retail's
`ret 12`; the extra two arguments are unused and their types remain provisional.
The upstream generic creation-slot signature also uses three long arguments.
Placement's `ret 8` and its table slot support its two-argument `__stdcall`
convention. Both addresses appear in the actual type-definition prefix at
`0x4680b8`, included through `+0x30`; later fields and the parent-type list
remain outside this partial view.

Full XDK 5849 check2 against `8f8b340`: **4,191 game matches / 4,191 total**,
one above the current upstream baseline, with no upstream matches lost. Thirty-one assertions
compiled with the original SDK validate object, scenario, header, and callback
prefix layouts. Inspection verifies the linked type metadata/name, both
callback targets, both object-global loads, the shadowless mask, and all
seven source-to-destination field offsets. No game runtime tests were run.

Only the new source and this document change. No external function calls,
new stubs, shared-header edits, other files' flags, or inventory changes.
The claim was published before source. One source version was checked at the
initial base and again after rebasing onto upstream's discovery/checker fixes.

## Attribution

Symbol names and object ownership were consulted in the
[Halo Symbol Atlas](https://github.com/tinkerer-red/halo-symbol-atlas),
licensed CC BY 4.0. Builds consulted:

- Retail: `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`.
- 2003 profile: `4f4f09b181eec4a434418b38efe581e75aaf3047c24add8a712751d6ae0d34d3`.
- 2003 debug: `96ea21d862dfe6a0bebb23e1a4311202a6e18a79970189a4df320d4ededa439d`.

Source was reconstructed from retail disassembly with the CC0 reference.
Game and SDK files remain outside the contribution.
