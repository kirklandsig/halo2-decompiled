# Copy callback (`0x52020`)

Retail `0x52020` is a 24-byte callback. It copies twenty dwords (80 bytes)
from its third stack argument to its first, returns true in `AL`, and ends
with `ret 0xc`. The second stack argument is unused. `ESI` and `EDI` are
preserved.

The source uses the three-argument `__stdcall` signature and an 80-byte
`memcpy`. The recovered caller `0x52040` passes its address to `0x4b220`, as
retail does at `0x52cd3`. This real address escape preserves the callback's
stack calling convention under link-time optimization. With this caller
present, `tools/check.py 0x52020` matches all 24 retail bytes. No synthetic
address global or `standard` marker is used.

Before the caller was recovered, LTCG moved the callback's inputs into
registers and removed the unused argument. The earlier callback-only build
therefore did not match its retail calling convention. That limitation no
longer applies to the current implementation.

The caller itself remains unmatched. See [its retail analysis](unknown_052040.md)
for the request layout, shader paths and draw sequence. A callback match does
not establish that the caller renders correctly.

These observations come from independent disassembly of the retail executable
identified by the project's published SHA-256 and comparisons with the
locally compiled source. No original source or leaked symbols were used.
