# Join-request codec analysis (unknown_0acc20)

Retail range covered: `0xacc20`–`0xad1bf`, the writer and reader for the
`"join-request"` message. The two inventory entries contain 1,422 retail
bytes. Both already have implementations in `src/unknown_0acc20.cpp`,
named `message_join_request_encode` and `message_join_request_decode`.
**Analysis only:** this document adds no source or new match claim. It
records the body layout and edge behaviour for review and interoperability
work. The `function_<va>` form is primary; descriptions below name what
the code does, without assigning meanings to the opaque fields.

## Boundary

- `function_0acc20` is 884 bytes; `function_0acfa0` is 538 bytes. The
  range includes the alignment after each body. The preceding entry,
  `0xacb10`, and following entry, `0xad1c0`, are excluded.
- This is a document about existing code, with no reservation against
  source work.
- Registration at `0xadab0` and the bitstream helpers are evidence for
  this document, outside its range. Session admission, transport and the
  meaning of the mode values are outside its scope.

## Registration and calling convention

`function_0adab0` installs the pair in descriptor 8 of a table with
`0x20`-byte entries. At table `+0x100` it sets the enabled byte, at
`+0x104` the label pointer `0x452dbc` (`"join-request"` in retail), at
`+0x10c` and `+0x110` the size `0x1b8`, and at `+0x114` and `+0x118`
the writer and reader pointers. The registered size is the in-memory
payload size, **not** a fixed number of bytes on the wire.

Stack arguments are listed in argument order, relative to the entry
stack pointer. Neither codec reads the size argument.

| Retail | Behaviour | Convention |
| --- | --- | --- |
| `function_0acc20` | Write the message body | stack: stream (`+4`), size (`+8`), payload (`+0xc`); `ret 0xc`; no result |
| `function_0acfa0` | Read the body into the supplied payload; retain writes on failure | same arguments and cleanup; success in al |

The writer reads the payload through edi and stream through ebp after
its prologue; the reader uses esi and edi respectively. These are local
register choices, not incoming register arguments.

## Payload layout

Offsets are relative to the payload. The three arrays have room for 16
entries in the existing declaration. The codec's actual loop bounds are
described below. "Opaque" means a bit block copied without interpretation
by this pair.

| Offset | Storage | Use |
| --- | --- | --- |
| `+0x0` | word | First field, 16 bits on the wire |
| `+0x2` | 2 bytes | Not accessed by either codec |
| `+0x4` | 8 bytes | First opaque block |
| `+0xc` | signed dword | Entry count; written as 5 bits |
| `+0x10` | 16 × 12 bytes | Per-entry opaque blocks |
| `+0xd0` | 16 × dword | First per-entry values; biased by 1, written as 8 bits |
| `+0x110` | 16 × dword | Second per-entry values; biased by 1, written as 31 bits |
| `+0x150` | 8 bytes | Third opaque block on the wire |
| `+0x158` | byte | Presence flag for `+0x15c` |
| `+0x159` | 3 bytes | Not directly accessed; oversized array loops can reach these bytes |
| `+0x15c` | 4 bytes | Optional opaque block |
| `+0x160` | dword | Mode, written as 2 bits |
| `+0x164`, `+0x168`, `+0x16c`, `+0x170` | 4 bytes each | Mode-2 opaque blocks, with the last two exchanged on the wire |
| `+0x174`, `+0x178`, `+0x17c` | dword each | Mode-2 values, written as 7 bits each |
| `+0x180` | 12 bytes | Optional mode-2 opaque block |
| `+0x18c` | 8 bytes | Second opaque block on the wire |
| `+0x194` | 36 bytes | Fourth opaque block on the wire; ends at `+0x1b8` |

These offsets agree with `s_message_join_request` in
`src/unknown_0acc20.cpp`. The declaration's names for addresses, nonces
and player fields are not needed to derive the layout. In particular,
the 12-byte block at `+0x180` is treated here only as an opaque block.

## Wire order

The table lists body fields in consumption order. Numeric fields use
their low bits first. Bulk transfers preserve the byte sequence at a
byte-aligned cursor and use the same low-bit-first packing when unaligned
(`0x1955d0`, `0x195820`). There is no alignment between fields or entries.

| Order | Payload source/destination | Bits | Transformation |
| --- | --- | --- | --- |
| 1 | `+0x0` | 16 | Unsigned word |
| 2 | `+0x4`, `+0x18c`, `+0x150` | 64 each | Opaque blocks, in this order |
| 3 | `+0x194` | 288 | Opaque block |
| 4 | `+0xc` | 5 | Entry count |
| 5, repeated | `+0x10 + 12*i` | 96 | Entry block |
| 6, repeated | `+0xd0 + 4*i` | 8 | Add 1 on write; subtract 1 on read |
| 7, repeated | `+0x110 + 4*i` | 31 | Add 1 on write; subtract 1 on read |
| 8 | `+0x158` | 1 | Nonzero byte becomes true |
| 9, if true | `+0x15c` | 32 | Opaque block |
| 10 | `+0x160` | 2 | Mode |
| 11, mode 2 only | `+0x174`, `+0x178`, `+0x17c` | 7 each | Numeric values |
| 12, mode 2 only | `+0x164`, `+0x168`, `+0x170`, `+0x16c` | 32 each | Opaque blocks, in this order |
| 13, mode 2 only | Presence of `+0x180` | 1 | Writer compares all 12 bytes with the zero block at `0x440070` |
| 14, if present | `+0x180` | 96 | Opaque block |

The first count bit is at body bit 496. For a normal input with `n`
entries (0..16), consistent presence bits and a mode in 0..3, the body
length is:

```text
504 + 135*n + 32*a + (mode == 2 ? 150 + 96*b : 0) bits
```

Here `a` is the first presence bit and `b` the mode-2 presence bit.
For example, an empty message without either optional section takes 504
bits (63 bytes); 16 entries with both optionals and mode 2 take 2,942
bits (368 bytes when rounded up). These lengths exclude any enclosing
message header and assume the writer starts with suitable cleared storage.

## Behaviour that matters to a replacement

### Counts and overlapping fields

The writer sends only five bits of `+0xc`, but runs its loop against the
**full signed dword**. It reloads that dword at `0xacd8c` on every
iteration. A negative count skips the loop; a count of 32 sends zero in
the count field but still runs 32 iterations. Range diagnostics do not
clamp or reject the value.

The reader stores the five-bit count and runs the same signed comparison,
reloading the stored count at `0xad03f`. It has **no upper-bound check of
16**. With disjoint stream and payload storage, counts 17..31 can finish
successfully if there are enough bits and the error byte is clear.
They overwrite fields that the 16-entry declaration represents separately.
For example, entry 16 (zero-based) writes its block at `+0xd0`, its first
value at `+0x110`, and its second value at `+0x150`, replacing part of an
opaque block already read from the header. This is codec behaviour, not
evidence that session admission accepts such a request.

The increments and decrements are 32-bit x86 operations. A stored -1
becomes wire zero and decodes back to -1. The 8-bit value decodes into
-1..254; the 31-bit value into -1..`0x7ffffffe`. An implementation in C
needs to account for wraparound without relying on signed overflow.

### Conditional fields

When the first presence bit is false, the reader writes zero to `+0x158`
but does not clear `+0x15c`. A mode other than 2 skips the whole tail;
those tail fields keep their previous contents, including any writes from
an oversized entry loop. Within mode 2, a false final presence bit
**does** zero all 12 bytes at `+0x180` (`0xad178`–`0xad185`). Neither
function clears the whole payload.

The writer tests the full source mode against 2 at `0xace32`, after
sending its low two bits. Thus source mode 6 emits the same mode bits as
2 but omits the tail that the reader would expect. The normal-input
length formula above is not a round-trip guarantee for out-of-range
values or overlapping input/output buffers.

### Stream errors and storage

The stream holds a buffer pointer at `+0x0`, byte capacity at `+0x4`,
bit cursor at `+0x10` and error byte at `+0x14`. Numeric and bulk helpers
advance by the requested bit count even when the available data is
short. The decoder keeps reading and writing fields; it does not roll
back on overflow or an error byte that was already set.

At `0xad188`–`0xad1b7`, success requires a clear error byte, a bit cursor
not greater than `capacity << 3` under a signed 32-bit comparison, and
a nonnegative final payload count. There is no further mode or opaque
field validation in this function. The shifted capacity and cursor are
32-bit values, so this test is not a general host buffer bounds check.

Both inline boolean readers (`0xad055` and `0xad124`) still inspect a
buffer byte when the cursor equals the bit capacity, then increment the
cursor. The final overflow check fails afterwards. A host reader cannot
assume that the logical byte capacity includes all backing storage that
the original accesses; the bulk helpers also use word loads on unaligned
paths.

The writer's inline boolean operations OR a true bit into the buffer;
false only advances the cursor. The numeric helper at `0x195720` also
ORs into the current word. Writing over arbitrary nonzero storage is
therefore not equivalent to replacing each field's bits. Bulk transfers
have their own copy and masking paths. A replacement should preserve the
stream's initialization contract as well as the field order.

The writer's range diagnostics call `0xb66f0` with a stack-local text
buffer, then continue to the bit write. They provide no failure return.
The reader's result is only al; the rest of eax is not a boolean result.

## Evidence and verification

- Both bodies and `0xadab0`, `0x1955d0`, `0x195720`, `0x195820`,
  `0x1959c0`, `0x1946f0` and `0xb66f0` were disassembled with this
  repository's `tools/disasm.py` from the retail XBE, SHA-256
  `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`.
  The registration label and the 12 zero bytes at `0x440070` were read
  through `tools/xbe.py`. The wire lengths above are arithmetic derived
  from the verified field widths.
- BirchWoodGod/halo2-decomp has independent functional recoveries of
  both codecs, with ABI and registration records. Its differential test
  supplies 512 writer and 512 reader comparisons: all five-bit counts,
  bit alignments, short and empty streams, sticky errors, ignored sizes,
  out-of-range writer values and absent optionals. It compares the full
  mapped guest memory, stack cleanup and reader al. The stream and
  payload are disjoint; oversized entry arrays overlap fields within
  the payload. Original codec and bitstream instructions run intact.
- On 2026-10-05, a fresh native Release build passed that entire suite
  (17,530 comparisons, including the 1,024 above). Ten additional direct
  comparisons confirmed successful counts 0, 16, 17 and 31, the absent
  mode-2 block, and writer cursor lengths including source mode 6 and
  count 32. This repository's non-SDK pytest run passed 226 tests, with
  four SDK tests deselected.
- No original compiler or SDK/XDK material was obtained or used. Native
  differential tests exercise sampled behaviour; they do not establish
  an original-compiler byte match or working network transport.

To reproduce the primary listings with a local retail copy:

```sh
export RETAIL_XBE=/path/to/default.xbe
python tools/disasm.py acc20
python tools/disasm.py acfa0
python tools/disasm.py adab0
```

## Sources

Our own disassembly and analysis of the retail executable. The lead came
from our own published functional recovery,
[BirchWoodGod/halo2-decomp](https://github.com/BirchWoodGod/halo2-decomp)
(CC0), especially `ENGINE.md`'s join-request codec notes. Its names were
not used here. Each behavioural statement was checked against retail.
`"join-request"` is the retail registration string; field roles here are
descriptions of accesses, not recovered original symbols. No leaked,
internal or third-party symbol data was used. Game and SDK files remain
outside the contribution.
