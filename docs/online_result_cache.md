# Online result cache

Retail range: `0xb36c0`–`0xb3d2f`, thirteen functions and 1,530 retail bytes.
The group is inferred from its internal calls and shared state. It is not
asserted to be a proven original source-file boundary. Banshee64 approved
this bounded group, and the draft claim preceded implementation.

The functions start and poll an online query, shuffle its result records,
register a selected address and key, and format diagnostic descriptions.
All thirteen bodies are recovered in `src/online_result_cache.cpp`.

| Retail address | Retail bytes | Built bytes | Result |
| --- | ---: | ---: | --- |
| `0xb36c0` | 62 | 62 | Differs |
| `0xb3700` | 23 | 23 | Exact |
| `0xb3720` | 72 | 72 | Exact |
| `0xb3770` | 115 | 106 | Differs |
| `0xb37f0` | 212 | 205 | Differs |
| `0xb38d0` | 153 | 176 | Differs |
| `0xb3970` | 242 | 240 | Differs |
| `0xb3a70` | 156 | 157 | Differs |
| `0xb3b10` | 83 | 83 | Exact |
| `0xb3b70` | 264 | 264 | Exact |
| `0xb3c80` | 34 | 34 | Exact |
| `0xb3cb0` | 52 | 52 | Exact |
| `0xb3cf0` | 62 | 62 | Exact |

A full build and comparison on upstream `d919f0cf` reproduced all 5,671
baseline matches and gained seven: **5,678 matches, zero lost**. The seven
new exact functions cover 590 retail bytes. This is the standalone branch
result on 2026-10-05; the six other bodies are not claimed as byte matches.

The remaining differences are:

- `0xb36c0`: comparison/return instruction selection and loop alignment.
- `0xb3770`: cached output reload and success/failure exit layout.
- `0xb37f0`: placement of saved-register pushes and early-return epilogues.
- `0xb38d0`: loop register allocation, spilling, and alignment.
- `0xb3970`: reuse of the text-buffer register instead of recomputing its
  stack address (`mov` versus `lea`); the two-byte difference changes branch
  displacements.
- `0xb3a70`: zero/result register lifetime and saved-register selection.

No caller, shared header, existing stub, other source file, or inventory
changes are included. Existing online task implementations and their header
are reused. The service-table type repeats its existing definition without
changing its owner.

## Retail evidence

The result buffer at `0x510588` holds eight records of `0x4a` bytes. The
count at `0x5107d8`, cached index at `0x5107dc`, cached address at `0x5107e0`,
and registration count at `0x5107e4` are read and updated by this group.
The record layout follows the formatting and registration accesses:
entity bytes at `+0`, address at `+0xa`, key identifier at `+0x30`, and key
at `+0x3a`. Three intervening words preserve the observed spacing. A
compile-time size assertion protects the stride.

`0xb36c0` consults the existing thirteen-entry service table at `0x467178`
and the existing logon task. `0xb37f0` obtains a task, starts the query,
and sets its type to `0x28`. `0xb3720` validates the task index and salt
before checking its completion flag. `0xb3970` retrieves results, releases
the task, formats descriptions, and then shuffles the records.

The shuffle advances the second existing random seed with the retail LCG
and copies whole records. The address code preserves retail's call to
`XNetConnect` before clearing an old cached address; it does not substitute
another operation based on the surrounding cleanup behavior. Retail's
initial values for the new storage are zero. The text helpers retain the
4096-byte capacity, bounded length scan, and final terminator.

API identities and diagnostic strings came from the retail executable.
`XOnlineQuerySearch` and `XOnlineQuerySearchGetResults` were confirmed using
the repository's documented SDK library-signature workflow. Existing
project calls corroborate the XNet APIs. No SDK header contents or library
implementation code were copied, and no historical game symbols were used.
Public API declarations were also corroborated in
[Ultimate ASI Loader's compatibility header](https://github.com/ThirteenAG/Ultimate-ASI-Loader/blob/master/source/xlive/xliveless.h);
no implementation from that project was copied.

Combined integration with both the online-cache and path-transition contributions on upstream `d919f0cf` passed the full comparison: **5,684 matches, 13 gained, 0 lost**, exit 0. All standalone exact matches were retained.
