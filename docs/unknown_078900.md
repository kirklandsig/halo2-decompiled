# Channel bandwidth controller analysis (unknown_078900)

Retail range covered: `0x78900`–`0x7a83f` (the network observer's
per-channel bandwidth controller). This is an inferred boundary: 24
inventory entries totaling 7,799 retail bytes. **Analysis only:** this
document adds no source, and nothing here has been built or checked against
retail with the original compiler. Names are provisional. The `function_<va>`
form is primary; the suggested names describe behaviour and are offered for
whoever decompiles the range.

## Boundary

- The preceding entries, `0x783d0`–`0x78880`, are channel and address code in
  `src/unknown_075870.cpp`, or are stubbed (`0x785d0`, `0x78880`). They are
  excluded.
- The following entry, `0x7a840`, initialises a transport address and is
  stubbed in `src/stubs/lane_d.cpp`. It is excluded.
- `0x78900` is the only entry in the range that does not touch the bandwidth
  block. It reopens connections for the observer's channels; it is listed
  here because it sits inside the range and has no other owner yet.
- Every entry in the range is `todo` at `9189415`, and none has an
  `@retail` or `@stub` marker. The Active claims table (issue #9) leaves
  `0x783d0`–`0x7a89f` open: lane D's listed range ends at `0x783d0`, and its
  next listed range starts at `0x7a8a0`. This range borders lane D's observer
  code. If lane D wants it, this document is meant for that lane.

## Where it is called from

| Caller | What it does with the range |
| --- | --- |
| `0x75da0` | The observer's per-frame update. After its channel loop it calls `0x78ac0`, then `0x7a4a0` |
| `0x76670` (`packet_sent`) | Adds the packet size to the channel's sent bytes at `+0x4a4`, and sets `+0x4a1` when the flag is set |
| `0x76720` (listener slot 2) | Updates the received statistics at channel `+0x178`, then calls `0x79660` while the bandwidth block is active |
| `0x76810` (listener slot 3) | Pushes the outcome flags into the two sample windows at channel `+0x250` and `+0x360`, then calls `0x797b0` |
| `0x7f410` | Stores a new estimate in observer `+0x4e04` (at most `0x100000`) and `+0x4e01`, calls `0x75af0`, then `0x7a740` |
| `0x81780` | Ends with a tail jump to `0x78900`, with the observer at `0x5291a0` |
| `0x68800` | Calls `0x78a10` |

The listener vtable at `0x450e10` holds `0x76670`, `0x76640`, `0x76720`,
`0x76810`, `0x72c70`, `0x75fc0`, in slot order.

## Units

`GetTickCount` (XAPI `0x3314b0`, or the override in `g_510548`/`g_51054c`)
counts milliseconds. Sent and received rates are `bytes * 8000 / elapsed`
(`0x79560`, `0x795b0`), so rates and budgets are bits per second. Burst
sizes are bytes: `0x78e60` and `0x79850` size them as
`delay * budget / (steps * 8000)`. The float rate at `+0x49c` is consistent
with packets per second, since `delay * rate * 0.001` (`0x45dc70`) gives a
packet count.

## Configuration fields

`observer->configuration` (`s_network_observer +0x10`) is `0x4cf4e0`. That is
`g_network_configuration + 0x14a0`, which `0x75970` receives from
`0x8db3b` (`push 0x4cf4e0`). The offsets below are from `0x4cf4e0`. The last
column gives the matching `s_network_configuration` field from
`include/unknown_0662e0.h`. The defaults were read by running retail
`0x67a40` (`function_67a40`, already matched) in an emulator with the XBE
mapped.

| Offset | Default | Use in this range | Configuration field |
| --- | --- | --- | --- |
| `+0xe0` | 200 | Minimum burst | `value1580` |
| `+0x144` | 1 | When the round's probe limit is reached, `0x79de0` still calls both rate functions (results unused) | `flag15e4` |
| `+0x148` | 2000 | Probe round period (ms), `0x78ac0` | `value15e8` |
| `+0x14c` | 4096 | Minimum budget | `value15ec` |
| `+0x150` | 71680 | Maximum budget a probe may reach | `value15f0` |
| `+0x154` | 1000 | Startup window after `0x75af0` reset (ms) | `value15f4` |
| `+0x158` | 30720 | Budget of a new channel | `value15f8` |
| `+0x15c` | 122880 | Cap on half of `+0x4e04` | `value15fc` |
| `+0x160` | 512000 | Cap on three quarters of `+0x4e04` (when `+0x4e01`) | `value1600` |
| `+0x164` | 8192 | Per-channel startup floor | `value1604` |
| `+0x16c` | 320 | Minimum baseline delay | `value160c` |
| `+0x170` | 3 | Smoothing shift for delay and interval | `value1610` |
| `+0x174` | 32 | Outcome window length (bits) | `value1614` |
| `+0x178` | 0.1 | Loss fraction that triggers backoff | `real1618` |
| `+0x17c` | 4 | Backoff hold, in multiples of the delay | `value161c` |
| `+0x180` | 0.8 | Backoff scale for budget and burst | `real1620` |
| `+0x184` | 10 | Loss penalty added per backoff | `value1624` |
| `+0x188` | 21 | Loss penalty that forces a reduction | `value1628` |
| `+0x18c` | 3072 | Largest budget step for a probe | `value162c` |
| `+0x190` | 0.2 | Budget step for a probe, as a fraction | `real1630` |
| `+0x194` | 5120 | Largest budget step for a reduction | `value1634` |
| `+0x198` | 0.3 | Budget step for a reduction, as a fraction | `real1638` |
| `+0x19c` | 5000 | Measurement cycle length (ms) | `value163c` |
| `+0x1a0` | 1500 | Cycle age before a probe can start or be judged | `value1640` |
| `+0x1a4` | 1500 | Cycle age before measurements are taken | `value1644` |
| `+0x1a8` | 30720 | Received rate that qualifies a budget-limited channel | `value1648` |
| `+0x1b4` | 3 | Smoothing shift for the total sent rate | `value1654` |
| `+0x1b8` | 6144 | Margin for picking the busiest channel | `value1658` |
| `+0x1bc` | 20000 | Holdoff between cross-channel reductions (ms) | `value165c` |
| `+0x1c0` | 6 | Most probes per round | `value1660` |
| `+0x1c4` | 0.5 | Probes per round, per active channel | `real1664` |
| `+0x1c8` | 20.0 | Largest probe priority | `real1668` |
| `+0x1cc` | 0 | Priority gained per ms since the last probe | `value166c` |
| `+0x1d0` | 1/2560 | Priority lost per unit of budget | `real1670` |
| `+0x1d4` | 160 | Delay tolerance | `value1674` |
| `+0x1d8` | 40 | Delay tolerance with a connection callback (probe checks) | `value1678` |
| `+0x1dc` | 120 | Delay tolerance with a connection callback (`0x78ac0`) | `value167c` |
| `+0x1e0` | 3 | Reduce on every Nth probe failure | `value1680` |
| `+0x1e4` | 10 | Failures before the baseline is raised | `value1684` |
| `+0x1e8` | 10 | Baseline raise (ms) | `value1688` |
| `+0x1ec` | 3 | Constrained cycles needed before probing | `value168c` |
| `+0x1f0` | 8000 | Wait after a probe reset (ms) | `value1690` |
| `+0x25c` | 0 | Mode flag read by `0x7a4a0` | `flag16fc` |

`0x78ac0` uses `+0x1dc` and the probe checks use `+0x1d8` for the same
channel condition. That difference is in retail.

## The channel's bandwidth block

The block is the `0x94` bytes from channel `+0x48c` up to `message_mask` at
`+0x520`. `0x78e60` clears all of it with one `0x94`-byte `memset` before it
sets `+0x48c`. The offsets below are from the start of
`s_network_observer_channel`. "Callback" means the connection's
`s_network_connection::callback` (`+0x3c`), and "active" means that
callback's `active` byte (`+0x30`).

| Offset | Type | Meaning |
| --- | --- | --- |
| `+0x48c` | byte | Block in use (`flag48c`) |
| `+0x48d` | byte | Channel wanted: an owner or the game session wants it. Passed to `0x78190` |
| `+0x48e` | byte | Connection had a callback when the block started |
| `+0x48f` | byte | That callback was active |
| `+0x490` | byte | Callback present but not active |
| `+0x494` | long | Budget (bits/s) |
| `+0x498` | long | Burst (bytes) |
| `+0x49c` | real | Rate |
| `+0x4a0` | byte | Rate-limited: `0x78190`'s result, stored by `0x79600` |
| `+0x4a1` | byte | Budget-limited this cycle: set by `packet_sent`'s flag (`flag4a1`) |
| `+0x4a2` | byte | Burst-limited this cycle: set by the send check at `0x763c8` |
| `+0x4a4` | long | Bytes sent this cycle (`value4a4`) |
| `+0x4a8` | long | Bytes received this cycle (`0x79660`) |
| `+0x4ac` | long | Last delay from the reliable stream (stream `+0x96c`) |
| `+0x4b0` | long | Smoothed delay sample |
| `+0x4b4` | long | Smoothed interval between received samples |
| `+0x4b8` | long | Losses in the outcome window |
| `+0x4bc` | dword | Outcome window bits (1 = loss) |
| `+0x4c0` | byte | Backoff pending: the saved rate below is restored later |
| `+0x4c4` | long | Backoff start time |
| `+0x4c8` | long | Delay when the backoff started |
| `+0x4cc` | long | Budget to restore |
| `+0x4d0` | long | Burst to restore |
| `+0x4d4` | real | Rate to restore |
| `+0x4d8` | long | Loss penalty. Probing waits while it is positive |
| `+0x4dc` | long | Probe state: 0 waiting, 1 ready, 2 probing, 3 probe succeeded |
| `+0x4e0` | long | Probe reset time (-1 initially) |
| `+0x4e4` | long | Last received-sample time (-1 initially) |
| `+0x4e8` | long | Last probe attempt time (-1 initially) |
| `+0x4ec` | long | Probe failures |
| `+0x4f0` | long | Baseline (lowest) delay |
| `+0x4f4` | byte | A probe is under way and the values before it are saved |
| `+0x4f8` | long | Budget before the probe |
| `+0x4fc` | long | Burst before the probe |
| `+0x500` | real | Rate before the probe |
| `+0x504` | long | Smoothed delay when the probe started |
| `+0x508` | long | Received rate when the probe started |
| `+0x50c` | byte | Budget-limited at some point since the probe started |
| `+0x50d` | byte | Rate-limited at some point since the probe started |
| `+0x50e` | byte | Burst-limited at some point since the probe started |
| `+0x510` | long | Sent rate, last measurement |
| `+0x514` | long | Received rate, last measurement |
| `+0x518` | long | Consecutive constrained cycles |
| `+0x51c` | long | Consecutive unconstrained cycles |

The two sample windows at `+0x250` and `+0x360` (`samples250` and
`samples360`, `0x110` bytes each) have the same layout. `0x929d0` resets one
and `0x92a30` inserts into one:

| Offset | Meaning |
| --- | --- |
| `+0x0` | Slot count |
| `+0x4` | Next slot |
| `+0x8` | 32 slots of `{ long time; long value; }` |
| `+0x108` | Sum of the values |
| `+0x10c` | Age of the slot being replaced |

`0x76810` inserts `!flag2` into the `+0x250` window and `flag3` into the
`+0x360` window. So `+0x250`'s sum counts losses.

## Observer fields

These are offsets in `s_network_observer`. `0x75970` and `0x75af0` set their
initial values.

| Offset | Meaning |
| --- | --- |
| `+0x4e00` | Bandwidth control enabled (`flag4e00`) |
| `+0x4e01` | Use three quarters of the estimate rather than half (`unknown4e01`) |
| `+0x4e04` | Total bandwidth estimate (`value4e04`) |
| `+0x4e0c` | Smoothed total sent rate, -1 while unset (`value4e0c`) |
| `+0x4f2c` | Time of the last `0x75af0` reset (`time4f2c`) |
| `+0x4f30` | Time of the last probe round (`time4f30`) |
| `+0x4f34` | Time of the last cross-channel reduction, -1 initially (`time4f34`) |
| `+0x4f38` | Start of the current measurement cycle (`value4f38`) |
| `+0x4f3c` | Rates changed: commit this frame (`flag4f3c`) |
| `+0x4f3d` | A reduction happened: cancel running probes (`flag4f3d`) |
| `+0x4f3e` | A reduction was passed to other channels: recompute `+0x4e0c` (`flag4f3e`) |
| `+0x4f40` | Probes started this round |
| `+0x4f44` | Probe limit this round |

The names in parentheses are the fields of `s_network_observer` in
`include/unknown_075870.h`. The declaration ends at `flag4f3e`, so
`+0x4f40` and `+0x4f44` are not declared yet.

## Functions

The conventions were read from each function's register use, its `ret N`
and every caller in retail. "stack" arguments are listed in argument order.
Floats come back in `xmm0`.

| Retail | Suggested name | Convention | Behaviour |
| --- | --- | --- | --- |
| `0x78900` | `channels_reopen_connections` | stack: observer; `ret 4` | For each channel in use, formats the first dword of `remote_id` as `"%hd.%hd.%hd.%hd"` (`0x450c38`) through `0x11c9c0`. It opens a connection with `0x82060(name, 0x38)` into `connection_index`, sets that connection's `owner` (`+0x40`) to the observer, then calls `0x76f50`, `0x76ff0` and `0x776a0` |
| `0x78a10` | `bandwidth_get_channel_status` | ecx index, edx observer; stack: 4 out pointers; `ret 0x10`; returns bool | False for an index outside 0..14, or a channel unused or without the block. Otherwise it writes `+0x4b0`, the raw bits of `+0x49c`, `+0x514`, and `trunc(window250.sum / window250.count * 100.0)` (the loss percentage) |
| `0x78a90` | `bandwidth_compare_probe_priority` | stack: left, right, priorities; `ret 0xc` | `priorities[right] > priorities[left]`. This is the `sort_4byte` (`0x13de30`) comparator, so the sort is by descending priority |
| `0x78ac0` | `bandwidth_update` | stack: observer; `ret 4` | Per channel: drops the block when control is off, the connection's state is not 5, or the callback presence or active state changed. Starts a block (`0x78e60`) for a connected channel without one. Recomputes `+0x48d` from owner vtable slot 1 (`channel_is_trusted`) for each bit of `owner_mask`, and from the game session's slot 5 (`find_member_by_channel`) with `0x54890`. Reduces (`0x79c00`, passing the cut on) any probing channel whose `+0x4b0 - +0x4b4` is above `+0x504` plus the tolerance. Every `+0x148` ms: scores the channels (`0x7a2a0`), sets the probe limit to `clamp(round(n * +0x1c4), 1, +0x1c0)`, sorts, runs `0x7a330` on each, commits, turns state 3 into 1 (clearing failures) and stamps `+0x4f30`. Ends with `0x79260` |
| `0x78e60` | `bandwidth_channel_start` | eax index; stack: observer; `ret 4` | Sums the other active budgets. The target is `+0x158`. The available amount is `min(+0x4e04/2, +0x15c)`, or `min(3*+0x4e04/4, +0x160)` when `+0x4e01`, at least `(n+1) * +0x164` within `+0x154` ms of the reset. When over, the new budget is `max(max(sum, available)/(n+1), +0x14c)`, and every active channel is scaled down by `(total - budget) / sum`. In that loop, retail passes **the new channel's** `+0x48e`/`+0x490` to `0x78090` for every peer. Then it clears and fills the block, takes the delay from the reliable stream (at least `+0x16c`), and applies `0x78090`'s rate with a burst of `(steps + 1) * delay * budget / (steps * 8000)`, at least `+0xe0` |
| `0x79260` | `bandwidth_cycle_measure` | eax observer; `ret` | When the rates changed or the cycle is `+0x19c` old: if a reduction happened, undoes probes in state 2 or 3 (`0x7a110`). Otherwise, once `+0x1a4` has passed, stores the sent and received rates and, unless a channel has a loss penalty, calls `0x77f90` with the total received rate, the channel count and the qualifying count. If cuts were passed on, smooths the total sent rate into `+0x4e0c`. Then calls `0x79480`. Always clears `+0x4f3c`..`+0x4f3e` |
| `0x79480` | `bandwidth_cycle_finish` | stack: observer; `ret 4` | Per active channel: ORs the limited flags into `+0x50c`..`+0x50e` when the sent rate is positive, and counts constrained or unconstrained cycles. Lowers the baseline to `+0x4b0` when that is below it, clearing the failure count. Clears the counters and `+0x4a1`/`+0x4a2`, then stamps `+0x4f38`. Without the override, retail reads the tick count a second time and discards it |
| `0x79560` | `bandwidth_received_rate` | ecx observer, eax index; `ret` | `+0x4a8 * 8000 / (now - +0x4f38)`, or 0 when no time has passed |
| `0x795b0` | `bandwidth_sent_rate` | ecx observer, eax index; `ret` | The same, with `+0x4a4` |
| `0x79600` | `bandwidth_channel_set_rate` | ecx observer, eax index, xmm5 rate; stack: budget, burst; `ret 8` | Stores budget, burst and rate, and stores `0x78190(cl=+0x48d, xmm1=rate, observer, +0x48e, +0x490)` in `+0x4a0` |
| `0x79660` | `bandwidth_channel_receive_sample` | eax delay sample, ecx byte count; stack: observer, index, stream delay; `ret 0xc` | Adds the byte count to `+0x4a8` and stores the stream delay in `+0x4ac`. Smooths the sample into `+0x4b0` and the time since the last sample into `+0x4b4`: `x += (new - x) >> +0x170`, an arithmetic shift. While a backoff is pending and `+0x17c * max(+0x4b0, +0x4c8)` ms have passed since `+0x4c4`, restores the saved rate and ends the backoff. With no backoff pending, it decrements a positive loss penalty |
| `0x797b0` | `bandwidth_channel_record_outcome` | eax observer, edx index; stack: bool ok; `ret 4` | Shifts the `+0x174`-bit window at `+0x4bc`, keeping the count `+0x4b8` in step. A false `ok` adds a loss. When the count reaches `+0x174 * +0x178`, calls `0x79850` and clears the window |
| `0x79850` | `bandwidth_channel_loss_backoff` | edi observer; stack: index; `ret 4` | Saves budget, burst and rate as the backoff values and records the delay and start time (also into `+0x4e0`). Undoes a probe in state 2 or 3. Scales budget and burst by `+0x180` (rounded, with floors `+0x14c`/`+0xe0`), caps the burst by the delay-based size, and applies the result. Adds `+0x184` to the penalty; at `+0x188` it reduces (`0x79c00`, not passed on) and clears the penalty |
| `0x79a10` | `bandwidth_reduce_busiest_channel` | eax index; stack: observer; `ret 4` | At most once per `+0x1bc` ms. Finds the channel whose larger measured rate is highest and above this channel's by more than `+0x1b8`. It reduces that channel unless the found one has a callback and this one does not, then stamps `+0x4f34` |
| `0x79c00` | `bandwidth_channel_reduce` | ecx observer; stack: index, bool pass_on; `ret 8` | With `pass_on`, first calls `0x79a10`. Undoes a running probe. Cuts the budget by `min(round(budget * +0x198), +0x194)` (floor `+0x14c`) and scales the burst with it (floor `+0xe0`). During a backoff it cuts the values to restore and applies only the minimum of old and new, with the rate truncated to an integer value. Sets `+0x4f3c`/`+0x4f3d` (and `+0x4f3e` with `pass_on`), then `0x79d90` |
| `0x79d90` | `bandwidth_probe_reset` | ecx observer, eax index; `ret` | When the state is not 0, sets it to 0 and stamps `+0x4e0` |
| `0x79de0` | `bandwidth_probe_start` | edi observer; stack: index, bool *exhausted; `ret 8`; returns bool | Proposes a budget of `budget + min(round(budget * +0x190), +0x18c)` (at most `+0x150`), a burst of `burst + trunc(budget / (rate * 8.0))`, and a rate from `0x78210`. If none would grow, sets `*exhausted`. Once `+0x1a0` has passed and the round's limit is not reached, it grows one of them, in the order burst (if `+0x4a2`), budget (if `+0x4a1`), rate (if `+0x4a0`). A higher rate raises the budget to `0x78150(rate)` and the burst scales with the budget. On a start it saves the old values, delay and received rate, clears `+0x50c`..`+0x50e`, applies the new values, sets state 2 and counts the probe. Stamps `+0x4e8` |
| `0x7a110` | `bandwidth_probe_undo` | ecx observer, eax index; `ret` | If a probe is under way, applies the saved values, clears `+0x4f4` and sets state 1 |
| `0x7a160` | `bandwidth_probe_failed` | ecx observer, edi index; `ret` | Counts a failure. From `+0x1e4` failures on, it raises the baseline by `+0x1e8`. Otherwise every `+0x1e0`th failure reduces (`0x79c00`, not passed on) |
| `0x7a1c0` | `bandwidth_probe_judge` | ebx observer; stack: index; `ret 4`; returns 0/1/2 | 1 before `+0x1a0`. 2 when `+0x4b0` is above `+0x504` plus the tolerance. 0 when the received rate beats `+0x508` and the delay rise is within baseline plus tolerance. Otherwise 1, after `0x7a160` if the rise was too large |
| `0x7a2a0` | `bandwidth_probe_priority` | esi observer, eax index; `ret`; returns float | 0.0 unless the state is 1. Otherwise `min((now - +0x4e8) * +0x1cc, +0x1c8)` (or `+0x1c8` if the channel never probed), minus `budget * +0x1d0` |
| `0x7a330` | `bandwidth_probe_update` | eax observer; stack: index; `ret 4` | State 0 becomes 1 after `+0x1f0` ms and `+0x1ec` constrained cycles. In any other state, an unconstrained cycle resets the probe. State 2: result 0 sets state 3, 1 undoes the probe, 2 reduces and passes the cut on. States 1 and 3: unless a backoff or penalty is active, a delay rise beyond the baseline plus tolerance counts a failure. Otherwise it tries `0x79de0` and resets the probe when nothing can grow |
| `0x7a4a0` | `bandwidth_publish_mode` | eax observer; `ret` | Over the active channels: 1 + (any backoff pending) if a channel has an active callback. Otherwise 1 if `+0x25c` is set and a backoff is pending, or a channel with a callback was budget- or burst-limited since its probe. Otherwise 0. When `0x4c99b8` is set, stores the result in `0x4c987c`. Retail unrolls the 15-channel loop five times |
| `0x7a740` | `bandwidth_publish_levels` | stack: observer; `ret 4` | For each of `levels[1]` and `levels[0]`, finds the highest `i` in 1..16 with `+0x4e04 >= values[i]` (0 if none). When `0x4c99b8` is set, stores the results in `0x4c9880` and `0x4c9884`. `0x54cc0` returns the first when `0x4c9888` is 1 and the second when it is 2 or 3 (and `0x4c99b8` and `0x476fcc` are set). By default `values[0]` and `values[1]` are 0 and `values[i] = base + (i - 1) * 12288` |

`0x78090`, `0x78150`, `0x78190`, `0x78210` and `0x77f90` are rate and
measurement helpers just before this range. They are outside it.

## Notes on existing declarations

- `c_network_connection_listener::packet_received` (slot 2, `0x76720`):
  retail adds the **second** argument to the received statistics' `bytes`
  and `current.bytes`, and passes the **third** to `0x79660` as the delay
  sample. The declaration names them `a` and `size`, so the names appear to
  be the wrong way round.
- `s_network_observer_configuration` (`0x4cf4e0`) is part of
  `g_network_configuration`. Its offset `+0x0` is `value14a0` (`0x67a40`
  sets it to 1), and the fields above add `+0x144`..`+0x1f0` and `+0x25c`.
- The bandwidth block fills `unknown48d` up to `message_mask`; `flag4a1`
  and `value4a4` keep their positions.

## Evidence and verification

- Every entry was disassembled from the retail XBE (SHA-256 `03215919…`)
  with `tools/disasm.py`. The conventions, field offsets, constants
  (`100.0` at `0x445420`, `0.0` at `0x45dbd8`, `0.0001` at `0x45dbdc`,
  `8.0` at `0x45dc18`, `0.001` at `0x45dc70`) and callers were read from that
  code.
- The configuration defaults come from running retail `0x67a40` in an
  emulator (unicorn) and reading `0x4cf4e0`..`0x4cf73c` afterwards. The
  level tables come from the default code at `0x664e6`–`0x6657a`.
- 20 of the 24 entries (all except `0x79660`, `0x797b0`, `0x79850` and
  `0x7a740`) have independent functional recoveries in
  BirchWoodGod/halo2-decomp (CC0). There, x86 differential tests run the
  retail instructions in an emulator and compare full memory, return
  registers and callee calls with the native versions over thousands of
  randomised cases per function. The other four were analysed for this
  document from the disassembly alone.
- No source was compiled, and no claim here has been checked with the
  original compiler.

## Sources

Our own disassembly and emulation of the retail executable. The leads came
from our own published functional recovery,
[BirchWoodGod/halo2-decomp](https://github.com/BirchWoodGod/halo2-decomp)
(CC0). Its function names were not used here. The type and field names come
from this repository's own headers. No leaked, internal or third-party
symbol data was used. Game and SDK files remain outside the contribution.
