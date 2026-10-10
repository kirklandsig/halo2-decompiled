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

## Current implementation

The source uses the three-argument `__stdcall` signature and an 80-byte
`memcpy`. Its compiled copy body agrees with retail in 2,048 cases covering
all 32-by-32 source/destination alignment combinations, random payloads and
unused-argument values, separate nonoverlapping buffers, preserved registers,
and the true return value. These checks supply each body's observed inputs:
the current compiled body takes its source in `EAX` and destination in `EDX`,
where retail reads stack arguments. This is not a calling-convention match.

The address-taking caller `0x52040` has no recovered source yet, so the build
does not reproduce its registration of this callback. LTCG removes the unused
argument and chooses a register convention. The declaration retains the
retail signature for that caller's eventual recovery. No synthetic address
global or `standard` marker is used. A full check against `ecd4d2f3` preserves
all 7,477 recorded matches with no gains or losses; this function remains
unmatched.
