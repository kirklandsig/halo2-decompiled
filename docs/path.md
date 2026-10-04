# Path search recovery

## Claim and boundaries

Retail ranges: **`0x270590`–`0x2712ff` and `0x2713c0`–`0x2729af`**.
This is the surviving path-search cluster from `path.obj`: 18 inventory
entries, 8,939 retail bytes. Lane C's existing initializer `0x271300`
through `0x2713bf` is explicitly excluded and remains untouched.

Keep `src/unknown_271e50.cpp` unchanged, including its initializer,
near `path_heap_bubble_up` (`0x271e50`) and matched
`path_heap_bubble_down` (`0x271ef0`). The other 16 entries need recovery.
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

| Retail | Bytes | Initial state |
| --- | ---: | --- |
| `0x270590` | 41 | Unwritten |
| `0x2705c0` | 58 | Unwritten |
| `0x270600` | 63 | Unwritten |
| `0x270640` | 263 | Unwritten |
| `0x270750` | 467 | Unwritten |
| `0x270930` | 1116 | Unwritten |
| `0x270d90` | 1388 | Unwritten |
| `0x2713c0` | 46 | Unwritten |
| `0x2713f0` | 425 | Unwritten |
| `0x2715a0` | 135 | Unwritten |
| `0x271630` | 2076 | Unwritten |
| `0x271e50` | 150 | Existing todo; preserve |
| `0x271ef0` | 221 | Existing matched; preserve |
| `0x271fd0` | 65 | Unwritten |
| `0x272020` | 1760 | Unwritten |
| `0x272700` | 57 | Unwritten |
| `0x272740` | 206 | Unwritten |
| `0x272810` | 402 | Unwritten |

## Plan

Start with both input setters, the destination setter, and the hash lookup.
Use local views and leave `include/path.h` unchanged. Replace only the hash
lookup's existing stub in the first batch. Then recover the remaining search
helpers and callers in small batches, retaining the existing upstream code.
A full original-compiler check must preserve every upstream match before
publication. The draft claim precedes all source implementation.
