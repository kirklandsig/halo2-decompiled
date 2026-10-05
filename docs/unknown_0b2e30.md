# Session search analysis (unknown_0b2e30)

Retail range covered: `0xb2e30`–`0xb36bf` (the list of sessions a
machine has found: the system link broadcast search, the online search
with its QoS probes, and the shared result list). The range has 13
inventory entries. 9 of them are `todo`, with 1,956 retail bytes in all;
none of them has an `@retail` marker, and two (`0xb3610`, `0xb3670`) have
lane H's `@stub` markers. The other 4 are lane H's matched entries, which
are described here only for context. **Analysis only:** this document adds no source, and nothing
here has been built or checked against retail with the original compiler.
Names are provisional. The `function_<va>` form is primary; the suggested
names describe behaviour and are offered for whoever decompiles the range.

## Boundary

- The preceding entry, `0xb2de0`, is the last message codec of the finished
  codec range (`src/unknown_0b2440.cpp`). It is excluded.
- The following entry, `0xb36c0`, checks the online service list
  (`0x467180`–`0x46721c`) for one service. It does not touch this range's
  globals, and is excluded.
- `0xb3570`, `0xb35a0`, `0xb35d0` and `0xb35e0` are matched
  (`src/unknown_0b3570.cpp`, `src/unknown_0b35e0.cpp`, lane H). They are
  the reference-counted start and stop and the entry getter. They call
  `0xb3610` and `0xb3670`, which `src/stubs/lane_h.cpp` stubs.
- The Active claims table (issue #9) leaves this range open: the finished
  message codec range ends at `0xb2e00`, and the next claim starts at
  `0xb7740`. The
  callers are in lane D's range (`0x8d9f0`, `0x8dd70`, `0x8df50`), lane J's
  range (`0x940b0`) and lane H's code (`0x199b33`..`0x199bbf`). If one of
  those lanes wants the range, this document is meant for that lane.

## Where it is called from

| Caller | What it does with the range |
| --- | --- |
| `0x199b33` (lane H) | `function_b3570(0x20, &g_47d92c, flag)`: a list of 32 entries, allocated through the allocator at `0x47d92c`. `flag` selects the online search |
| `0x199b45`, `0x199b5b` (lane H) | Stop (`0xb35a0`), and restart or pause (`0xb35d0`, which calls `0xb3610` or `0xb3670`) |
| `0x199b6a`, `0x199ba5`, `0x199bbf` (lane H) | Walk the list with `0xb35e0`. `0x199bbf` returns the entry's description (`entry + 0x70`) when the description's `+0xbc` short is 0..16 |
| `0x8d9f0` | Network initialise. Copies the dword at `0x510568` to `0x4d8eb0`, zeroes `0x4d8ecc`..`0x4d8f03` and `0x4d8f04`..`0x4d8f1b`, and sets `0x4d8ef0` and `0x4d8ef4` to -1 |
| `0x8df50` | Network per-frame update. Calls `0x7f070`, then `0xb2ea0`, then `0xb3200` |
| `0x8dd70` | Network dispose. Calls `0xb3670`, then `0xb31b0`, then zeroes `0x4d8ea8`..`0x4d8ecb` |
| `0x940b0` | Handler of message type 3 (the search reply). Calls `0xb2fc0` (see "Messages") |

A scan of the game code for absolute references to `0x4d8eb0`..`0x4d8f1b`
finds only the functions above, the range itself and `0x93fa0`. (`0x8e210`
also mentions `0x4d8eb0`, but only as the end bound of the array before
it.)

## Messages

`0x938e0` dispatches messages by type through the jump table at `0x93a38`.
Type 2 goes to `0x93fa0` and type 3 to `0x940b0`. Both messages are
broadcast to port 1001.

| Type | Size | Payload | Sender | Receiver |
| --- | --- | --- | --- | --- |
| 2 (query) | `0xc` | `+0x0` word 2 (version), `+0x2` word 0, `+0x4` the 8-byte query identity | `0xb2ea0`, every 1500 ms while the system link search is active | `0x93fa0`. It ignores a query that carries its own active identity. If it has a session to describe (`0x59840`, `0x64d70`) and `0x4cf792` is set, it broadcasts a type 3 reply |
| 3 (reply) | `0x720` | `+0x0` word 2, `+0x4` the query's identity (echoed), `+0xc` the host's session description (`0x714` bytes) | `0x93fa0` | `0x940b0`. It requires version 2, an active search and the searcher's own identity, then calls `0xb2fc0` |

Both are sent through `0x7b140`, with arguments in the order link,
address, type, size, payload. The address is an `s_type_99af70` with
`ipv4_address` `0xffffffff`, `port` 1001 and `address_length` 4; its bytes
`+0x4`..`+0xf` are left as whatever was on the stack. The query uses the
link at `0x4d8eb0`, and the reply uses the handler's `link` (`+0xc`).

## Globals

| Address | Type | Meaning |
| --- | --- | --- |
| `0x4d8eb0` | pointer | The link that queries are sent through (copied by `0x8d9f0` from `0x510568`, cleared by `0x8dd70`) |
| `0x4d8eb4` | byte | System link search active |
| `0x4d8eb5` | byte | System link list changed: set by an expiry or a new description, cleared at start. No reader was found |
| `0x4d8eb8` | long | Time of the last query (ms) |
| `0x4d8ebc` | 8 bytes | Query identity: random bytes from `0x7ad50`, new for each search |
| `0x4d8ec4` | long | System link list entry count |
| `0x4d8ec8` | pointer | System link list base |
| `0x4d8ecc` | byte | Online search active |
| `0x4d8ed0` | `0x20` bytes | Input of the online search task: the first dword -1, the rest 0 |
| `0x4d8ef0` | long | Online search task index (-1 none) |
| `0x4d8ef4` | long | QoS lookup handle (-1 none). `0xb3200` compares its high word (`0x4d8ef6`) with the record's salt |
| `0x4d8ef8` | long | Online list capacity |
| `0x4d8efc` | pointer | Online list base |
| `0x4d8f00` | long | Time the online search was started. No reader was found |
| `0x4d8f04` | long | Start reference count (`g_4d8f04`) |
| `0x4d8f08` | byte | Online mode: use the online search rather than system link (`g_4d8f08`) |
| `0x4d8f0c` | long | Entries to allocate (`g_4d8f0c`) |
| `0x4d8f10` | pointer | Allocator (`g_4d8f10`, a `c_data_allocator`) |
| `0x4d8f14` | long | Entry count of the allocated list (`g_4d8f14`) |
| `0x4d8f18` | pointer | The allocated list (`g_4d8f18`). Both searches use this block: `0xb3610` passes it as the list base |

Times are milliseconds: `GetTickCount` (`0x3314b0`), or `g_51054c` when
`g_510548` is set. Every time comparison here subtracts with 32-bit
wrapping and compares the difference as signed.

## The list entry

Each entry is `0x784` bytes. The same layout serves both searches.
`s_0b35e0_entry` in `src/unknown_0b35e0.cpp` declares `+0x0` and `+0x72`.

| Offset | Type | Meaning |
| --- | --- | --- |
| `+0x0` | byte | In use |
| `+0x4` | long | Time last heard from (system link), or time filled (online) |
| `+0x8` | `XNKID` | Session key ID (online only) |
| `+0x10` | `XNKEY` | Session key (online only) |
| `+0x20` | `XNADDR` | Host address (online only), to `+0x43` |
| `+0x44` | byte | Host answered: a reply was stored (system link), or the QoS probe of this target reached status 2 (partial), 4 (disabled) or 5 (contacted) (online) |
| `+0x45` | byte | QoS result valid (online) |
| `+0x48` | long | QoS target status from `0x7b720`: 0..5 (online) |
| `+0x4c` | `s_qos_result` | QoS result from `0x7b7b0`, `0x20` bytes (online). `0xb3200` zeroes its `data_size` (`+0x64`) and `data` (`+0x68`) after decoding |
| `+0x6c` | byte | Description received or changed |
| `+0x70` | `s_session_description` | The host's description, `0x714` bytes, to the end of the entry |

Description fields this range uses, as offsets in the description
(`s_session_description` in `src/unknown_07b4c0.cpp` and
`include/unknown_058ee0.h`):

- `+0x02` (`field2`): `0xb35e0` returns only entries where this is 0.
- `+0x14` (`field14`) and `+0x9e` (`field9e`): replacement rule in
  `0xb2fc0`, below.
- `+0x70` (`address`, an `XNADDR`): `0xb2fc0` matches system link replies to
  entries by these 36 bytes.

## Functions

The conventions were read from each function's register use, its `ret N`
and every caller in retail. "stack" arguments are listed in argument order.

| Retail | Suggested name | Convention | Behaviour |
| --- | --- | --- | --- |
| `0xb2e30` | `system_link_search_start` | eax list base, esi count; `ret`; returns the active byte in al | If the search is already active, or the transport is not both initialised and started (`g_transport_globals` `0x4d8b18`/`0x4d8b19`), returns the active byte unchanged. Otherwise sets it, clears `0x4d8eb5` and `0x4d8eb8`, writes a new query identity with `0x7ad50` (output in ebx = `0x4d8ebc`), stores the count and base, and zeroes `count * 0x784` bytes of the list |
| `0xb2ea0` | `system_link_search_update` | no arguments; `ret` | Nothing while inactive. If more than 1500 ms have passed since `0x4d8eb8`, sends a type 2 query (see "Messages"), ignores the result, and stores a fresh time in `0x4d8eb8`. Then clears every entry in use that has not been heard from for more than 2000 ms (all `0x784` bytes) and sets `0x4d8eb5`. The count and base are read again on each iteration |
| `0xb2fc0` | `system_link_search_store_reply` | stack: reply; `ret 4` | Nothing while inactive or with a count of 0 or less. Picks a slot: the first entry in use whose description address equals the reply's (`reply + 0x7c`), or else the first free entry, or else the **last** entry in use that the reply may replace. A reply may replace an entry when its own `field9e` is nonzero, its `field14` is 0, and its `field9e` is less than the entry's (signed shorts). With no slot, it returns. A slot that did not match is zeroed. If the reply's description (`reply + 0xc`) differs from the entry's, it copies the `0x714` bytes and sets `+0x6c` and `0x4d8eb5`. Then sets `+0x0`, `+0x44` and the time `+0x4` |
| `0xb3100` | `online_search_start` | eax list base, esi count; `ret`; returns the active byte in al | If already active, returns. Otherwise zeroes the input at `0x4d8ed0`, sets its first dword to -1, and starts `0x8fa80` (`online_match_search`) with it into `0x4d8ef0`. When the task index is valid: sets active, stores the capacity and base, and zeroes the list. Always stamps `0x4d8f00` after a start attempt |
| `0xb31b0` | `online_search_stop` | no arguments; `ret` | Closes the search task with `0x6b640` and releases the QoS lookup with `0x7b650` (`qos_release`), resetting each handle to -1 when it was valid. Clears `0x4d8ecc`, `0x4d8ef8` and `0x4d8efc` |
| `0xb3200` | `online_search_update` | no arguments; `ret` | **Search task.** By `0x6b5d0` (`online_task_poll`): 0 or 1 waits. 2 copies up to the capacity's results into `0x68`-byte records with `0x8fb30` (edx task; stack: records, count pointer), zeroes the whole list and fills one entry per record with `0xb3500`. It then releases any earlier QoS lookup and, if there are results, starts `0x7b4c0` (`qos_lookup`) with kind 0, the result count, 65536 bits per second and targets of `{ entry+0x8, entry+0x10, entry+0x20 }` (`s_qos_target`, `0x3c` bytes). For status 2 or any other status except 0 and 1, it closes the task (`0x6b640`). **QoS.** While a lookup exists, it reads the target count from the lookup's `XNQOS` (0 when the handle is invalid or `g_4cf8d4` is clear). For each target whose `0x7b720` status changed, it stores the status in `+0x48`. Status 4 sets `+0x44`. Status 2 or 5 sets `+0x44`, stores `0x7b7b0`'s result in `+0x4c` and its success in `+0x45`, and, when data came back, decodes a description from it with `0x7c530` (esi data, ecx size; stack: output). On success it copies the description to `+0x70` and sets `+0x6c`. Either way, it then clears `+0x64` and `+0x68`. It releases the lookup when no probe is pending, the handle is invalid, or `g_4cf8d4` is clear |
| `0xb3500` | `online_search_fill_entry` | eax record, ebx entry; `ret` | Zeroes the entry, sets `+0x0`, stamps `+0x4`, then copies record `+0x10` (8 bytes) to `+0x8`, record `+0x0` (16 bytes) to `+0x10`, and record `+0x18` (36 bytes) to `+0x20`. The rest of the record (`+0x3c`..`+0x67`) is not copied. This fits `s_online_match_session_info` in `src/online_matchmaking.cpp` (`key`, `session_id`, `address`) |
| `0xb3610` | `session_search_begin` | no arguments; `ret`; returns bool | If `0x4d8f18` is null, allocates `0x4d8f0c * 0x784` bytes with the allocator's first virtual function into it, returning false on failure. Sets `0x4d8f14` to `0x4d8f0c`. Then returns the result of `0xb3100` (online mode) or `0xb2e30` (system link), with eax the list and esi the count |
| `0xb3670` | `session_search_end` | no arguments; `ret` | Online mode: `0xb31b0`. System link: clears `0x4d8eb4` if it is set. Then, if the list exists, frees it with the allocator's second virtual function and clears `0x4d8f18` and `0x4d8f14` |

## Notes on existing declarations

- `g_4d8eb4` (`s_system_link_globals` in `src/unknown_075800.cpp`) is
  described as "whether this machine advertises a game, and the game's
  session". In retail it is the local system link **search**: `0xb2e30`
  sets `active` when a search starts (from `0xb3610`), and fills the field
  at `+0x8` with new random bytes (`0x7ad50`) for each search. Queries send
  those bytes, and hosts echo them back. `0x93fa0`, the host's handler,
  uses them to skip its own query. So `session_id` is a per-search query
  identity rather than a session ID.
- `s_network_message_session_query`, which `0x940b0` reads, is the header
  of the type **3** reply. `0x938e0` sends type 2 (the query) to `0x93fa0`
  and type 3 to `0x940b0`. Both messages start with the same version word
  and identity, and the reply carries the description after them.
- `function_0b2fc0` is declared as `__stdcall` with one argument. That
  agrees with retail (`ret 4`).
- `s_0b35e0_entry::s72` is the description's `field2`, at `entry + 0x70 +
  0x2`. `s_session_description` declares it as a short with valid values
  0..1.
- The stubs of `0xb3610` (returns false) and `0xb3670` (does nothing) in
  `src/stubs/lane_h.cpp` stand in for the functions above. With the stub,
  `function_b3570` never counts a start.
- `0xb3670`'s system link path frees the list but leaves `0x4d8ec4` and
  `0x4d8ec8` set. Nothing reads them while `0x4d8eb4` is clear, since
  `0xb2ea0` and `0xb2fc0` check it first.

## Evidence and verification

- Every entry was disassembled from the retail XBE (SHA-256 `03215919…`)
  with `tools/disasm.py`, along with `0x938e0`, `0x93fa0`, `0x940b0`,
  `0x7ad50`, `0x7c530`, `0x8fb30`, `0x8d9f0`, `0x8dd70` and `0x8df50`. The
  conventions, offsets, constants (1500 and 2000 ms, port 1001, the sizes
  `0xc`, `0x720`, `0x714` and `0x784`) and callers were read from that
  code. The global references were found with a capstone scan of every
  game function in `config/functions.csv`.
- `0xb3500` was also run in an emulator (unicorn), with the XBE mapped and
  a numbered record, to confirm the copy layout above.
- 5 of the 9 entries (`0xb2e30`, `0xb2ea0`, `0xb2fc0`, `0xb31b0`,
  `0xb3670`), and `0x940b0` and `0x7ad50`, have independent functional
  recoveries in BirchWoodGod/halo2-decomp (CC0). There, x86 differential
  tests run the retail instructions in an emulator and compare full memory
  and callee calls with the native versions: 2,880 comparisons in all,
  including start, query, reply and expiry sequences that run the real
  message encoder. The other four (`0xb3100`, `0xb3200`, `0xb3500`,
  `0xb3610`) were analysed for this document from the disassembly.
- No source was compiled, and no claim here has been checked with the
  original compiler.

## Sources

Our own disassembly and emulation of the retail executable. The leads came
from our own published functional recovery,
[BirchWoodGod/halo2-decomp](https://github.com/BirchWoodGod/halo2-decomp)
(CC0). Its function names were not used here. The type, field and function
names in backticks come from this repository's own headers and sources;
`XNKID`, `XNKEY`, `XNADDR` and `XNQOS` are the SDK's public type names,
which this repository already uses. No leaked, internal or third-party
symbol data was used. Game and SDK files remain outside the contribution.
