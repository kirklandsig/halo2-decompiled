# Path transition geometry and marker selection

This recovery covers the user-approved bounded group `0x26f150`–`0x27058f`:
16 previously missing bodies, totaling 5,052 retail bytes. The neighboring
addresses, shared settings and marker records, and internal call graph support
this grouping; an original source-file boundary is not asserted.

The first helpers read type-indexed movement settings at `g_4e034c +0xc8/+0xcc`.
Three collectors request marker IDs through `0xb8d30` into 0x70-byte records.
The selection routines compare orientation and distance, and `0x270130` projects
an actor position onto a marker segment. The final routines transform transition
points and directions and use the existing path-location query `0x26d100`.
`0x26f3f0` dispatches the surface types and generates paired path endpoints.

The only changes outside the new source and this evidence document are removal
of its three old stubs (`0x26f150`, `0x26f3f0`, `0x26fc80`), and the expressly
approved two-line correction to the `0x1be410` declaration/call. Retail saves that
caller's output point in EBX at `0x1be42b`, then passes it through `0x1be477` to
`0x26fc80`; the old declaration omitted that fifth argument.

`0x26f3f0` retains the existing const-qualified declaration for compatibility.
Retail's caller at `0x270ffb` passes a mutable node field at +0x18 in EDI, and
retail writes that field. The new body uses a documented local const_cast;
the caller and shared declarations are unchanged.

No shared headers, tables or external callee bodies are changed. `0x26d100`
remains the existing stub, so its caller conventions remain a matching risk.
All behavior and layout evidence comes from the retail executable and current
project source. No historical symbol sources were consulted.

## Validation

Full pass after merging upstream `d919f0cf`: **5,677 matches**, **6 gained**,
**LOST 0**, wrapper exit **0** against the 5,671-match upstream baseline.
All 16 bodies are recovered: six exact (523 retail bytes), ten differ.

| Retail function | Retail bytes | Built bytes | Result |
| --- | ---: | ---: | --- |
| `0x26f150` | 312 | 312 | Differs |
| `0x26f290` | 52 | 52 | Exact |
| `0x26f2d0` | 132 | 134 | Differs |
| `0x26f360` | 129 | 123 | Differs |
| `0x26f3f0` | 1280 | 1244 | Differs |
| `0x26f8f0` | 57 | 57 | Exact |
| `0x26f930` | 94 | 94 | Exact |
| `0x26f990` | 608 | 572 | Differs |
| `0x26fbf0` | 139 | 139 | Exact |
| `0x26fc80` | 406 | 400 | Differs |
| `0x26fe20` | 97 | 97 | Exact |
| `0x26fe90` | 660 | 607 | Differs |
| `0x270130` | 260 | 241 | Differs |
| `0x270240` | 84 | 84 | Exact |
| `0x2702a0` | 342 | 292 | Differs |
| `0x270400` | 400 | 268 | Differs |

The remaining differences include register allocation, expression scheduling,
loop/exit layout and conventions imposed by the existing `0x26d100` stub.
`0x26f150` has the same instruction shape and size with different registers;
matching attempts stopped there as required. No exactness is claimed for the
ten differing functions.

The source uses `/Ob1` to retain calls which retail keeps out of line.
The two transform routines keep their matrices in a local scratch aggregate.
Without it, LTCG coalesced independent input buffers around the existing
register-only assembly implementation of `0x142a60` and removed the inverse
calculation. The final generated `0x270240` is exact; the generated `0x2702a0`
retains the inverse call and passes distinct source/inverse inputs with the
result in the local matrix, as retail does. No callee edits or special
compiler attributes were needed.

Collision storage is extended locally beyond the existing 0x4c-byte prefix:
retail `0x26d100` reads through +0x50 after its collision call. The local
0x54-byte buffer preserves the existing declaration and header unchanged.

Independent source review covered all 16 functions, the optional outputs,
marker offsets, surface dispatch, and the caller correction. Full comparison
supplies the match and regression evidence; the differing bodies remain work
for future matching.

Combined integration with both the online-cache and path-transition contributions on upstream `d919f0cf` passed the full comparison: **5,684 matches, 13 gained, 0 lost**, exit 0. All standalone exact matches were retained.
