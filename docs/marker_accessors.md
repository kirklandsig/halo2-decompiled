# Marker position and angle accessors

Retail range: `0x11c7d0`–`0x11c83f`; two functions, 89 retail bytes.

| Address | Bytes | Behavior | Result |
| --- | ---: | --- | --- |
| `0x11c7d0` | 36 | Copy position and angle from a record prefix | Exact match |
| `0x11c800` | 53 | Copy position and angle from an indexed marker entry | Exact match |

Both output pointers are optional. Position is copied first, then the angle
is read and copied. Neither routine calls another function or checks an
index. The direct-record accessor does not dereference the record when both
outputs are null.

Caller `0x14f2e0` passes records from `g_4e0350 + 0x104` with a 0x34-byte
stride, and uses the scalar at offset 0x0c in sine/cosine calculations. The
source-local `s_marker_position_angle` describes only the 0x10-byte prefix
read by the accessor, not the whole record.

The indexed accessor uses the existing `marker_entries` pointer at
`g_4e0350 + 0x11c`, with a 0x20-byte stride. Its scalar is read through the
existing four-byte `unknown0c` field. Caller `0x19cf20` combines this scalar
with an atan2 result before sine/cosine calculations, supporting the angle
interpretation for this table too.

This bounded pair is not asserted to be a complete original source-file
range; Banshee64 explicitly approved the scope. Callers, shared headers,
stubs and neighboring implementations are unchanged.

A full build/comparison after merging upstream `a04dffc9` reports 5,664
matched game functions, up from the upstream baseline of 5,662. Both
accessors match; no baseline matches are lost. The generated inventory is
not part of this contribution. Evidence is the retail code and callers,
together with the project's existing declarations.
