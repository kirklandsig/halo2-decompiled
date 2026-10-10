# Copy callback (`0x52020`)

Retail `0x52020` is a 24-byte callback. It copies twenty dwords (80 bytes)
from its third stack argument to its first, returns true in `AL`, and ends
with `ret 0xc`. The second stack argument is unused. `ESI` and `EDI` are
preserved.

The function's address is passed to `0x4b220` at `0x52cd3`–`0x52ce2`, inside
`0x52040`. This address escape is relevant when reproducing its stack calling
convention under link-time optimization. The registration function and its
caller are evidence only, outside this contribution's scope.

These observations come from independent disassembly of the retail
executable identified by the project's published SHA-256. No original
compiler match is claimed by this analysis note.
