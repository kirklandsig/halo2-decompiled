# Unit request update scan (`0xe6830`)

Retail `0xe6830` is a 137-byte function that updates active unit requests.
The existing callers declare it as `bool __stdcall function_e6830(long)`.
This note describes retail behavior; it does not establish a compiler match.

The function reads the object-header array through `g_4e0300`, indexes its
12-byte entries with the low 16 bits of the argument, and takes the object
pointer at entry offset 8. It does not check the argument's high 16 bits.
The signed 16-bit displacement at object offset `0x346` locates a block whose
two active-request words start at offset 4. The block address is captured
before iteration.

For each type from 0 through 59, the function tests the corresponding active
bit. If set, it loads the definition from `g_4677c8[type]`, then the update
callback at definition offset 4. A null callback leaves the bit unchanged.
Otherwise the callback receives the original argument and the type.

A true callback result sets the aggregate return value to true, but does
not stop iteration. A false result clears that type's bit from the word
reloaded after the callback. Other changes made by the callback survive.
Later iterations read their active bits and callback definitions afresh;
changing the object's block displacement or its object-header entry during
a callback does not redirect this scan. Bits 60 through 63 are not visited.

The return value is false when no invoked callback returns true. Retail
returns the boolean in `AL` and removes one stack argument with `ret 4`.

## Evidence

The retail instructions were read from the executable identified by the
project's published SHA-256. The table layout is consistent with the
existing `s_unit_request_definition` in `src/unknown_0e6900.cpp`.
An independently written local control-flow model agreed with retail
execution for 2,048 cases and 33,962 callback invocations, comparing callback
order, arguments, persistent memory, and the return value. Cases included
null callbacks, signed displacements, bit mutations, object/block replacement,
and replacement of later callback definitions. The same cases also passed
when executing the XDK-compiled function against retail in separate emulator
memory backings, using the linked map to resolve the compiled globals.
Neither behavioral comparison establishes a byte match.

The current compiled function is 137 bytes, like retail. The checker finds
three instruction differences: its bitmap accesses encode `[ebx + esi + 4]`
where retail encodes `[esi + ebx + 4]`. Both address the same word. A full
check against the baseline at `ecd4d2f3` retained all 7,477 existing matches,
with none gained or lost. A bounded permutation search tried 15 variants
without improving the three-instruction difference. The function remains
unmatched.
