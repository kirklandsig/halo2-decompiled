# Interface quad draw (unknown_052040)

Retail range covered: `0x52020`–`0x52d33`. This is two inventory entries,
`todo` with no source: `function_52040` (`0x52040`, 3,316 retail bytes), which
draws one screen-space quad textured with up to three bitmaps, and `0x52020`
(24 bytes), the callback that copies the quad's four vertices into the push
buffer. The first sets the render state and the vertex-shader constants, loads
three texture stages, builds a register-combiner program in `g_484f68`, binds
it and draws the quad through `function_4b220`. **Analysis only:** this
document adds no source, and nothing in it has been built or checked against
retail with the original compiler. Names are provisional. The `function_<va>`
form is primary; the descriptions are offered for whoever decompiles it.

It uses the names of `src/unknown_01cf50.cpp` (the shader cache, the immediate
vertex descriptors, `function_4b220` and `function_35b90`),
`src/unknown_020560.cpp` (the render-state cache and `function_0222d0`),
`src/unknown_13c050.cpp`, `src/unknown_13e635.cpp` and `src/unknown_236299.cpp`
(the three callers, which build the request) and `src/unknown_131e50.cpp`
(`pack_color4f`). [A depth-range pixel shader](unknown_03a5c0.md) describes
several of the Direct3D routines it calls. `function_35b90` is the closest
relative in the source: it reads the same request block and builds nearly the
same program, and its draw is a different one. Ten callees (all Direct3D
library routines) have no source and are described only as far as this function
needs them.

## Implementation follow-up

`src/unknown_01cf50.cpp` now contains this renderer beside the related
rendering code; `src/unknown_052020.cpp` contains its copy callback. The
inventory/status statements in the original analysis above
record the state when that analysis was written. The callback matches all
24 retail bytes with its real registration caller present; the renderer
remains unmatched (the current local build is 3,368 bytes, versus 3,316 in
retail). Equal behavior in isolated tests does not establish an exact match.

Local instruction comparisons cover shader construction, viewport/constants
preparation, texture-state setup, descriptor selection and the final draw
sequence. The texture and draw-boundary tests substitute external callees;
the descriptor test runs the real cache-reset helper but substitutes binding
calls. Those tests do not verify GPU rendering or whole-function equivalence.
Their reports identify the executable tested; results from an older build
must be rechecked after changes.

The initial descriptor selection is expanded in this renderer, as in retail;
the final draw sequence calls `function_1ccb0(36)`. Both use the existing
private descriptor table in the same translation unit, preserving pointer
identity without exporting the table. Grouping this renderer with its related
code also avoids adding another Direct3D-header translation unit, which
changed the matched player-count parser under LTCG in an isolated reproducer.
The optional-record alpha path
keeps the reciprocal as a float, multiplies by 255 on x87, then uses `FISTP`;
rounding that product to float first changes some integer results.

## Bitmap fallback dependency

The bitmap loader at `0x12310` calls `0x12ce00` when its first cache lookup
fails. That fallback now has source in `src/unknown_12c0d0.cpp`, replacing its
empty stub. It chooses shared, requested, resident, or default textures and
propagates cache-failure state. Its level-selection bias is accumulated in
retail's float-operation order; the early shared-texture comparison uses the
original bias instead.

The fallback remains unmatched (645 compiled bytes versus 658 retail).
The resident-level scan uses an advancing pointer and a three-iteration
countdown, retaining a loop like retail instead of three unrolled copies.
Default texture selection uses a switch, reproducing retail's signed type
load and decrement-based branches.
A const pointer to its bitmap parameter's own slot, read into a local view,
keeps that argument on the stack as in retail without changing its value.
This makes `0x12310` match all 77 retail bytes, and also enables exact
`0x12360`, `0x1cfb0`, `0x3bcb0`, `0xd15e0`, `0xd1630`, `0xd1680`, and
`0x2a04f0`, without editing those callers. Replacing the stub previously
enabled `0x42b20`, `0x23625d`, and `0x2b1179`. Full checks retain every
previous match.
A 4,096-case actual-instruction comparison covers the fallback's return
paths, cache timestamps, failure state, arguments and stack cleanup. Format
and level helpers execute their real bodies; the shared-header builder and
cache loader are explicit stand-ins. This does not validate loading,
allocation, or GPU behavior.

A separate 16,384-case integration comparison executes the renderer texture
loop, the complete bitmap loader at `0x12310`, and the raw-texture path of
`0x12ce00`. It covers frame-cache hits, initial-lookup hits, raw fallback
textures and null returns across all three bitmap slots. Only the initial
cache lookup is replaced by a stand-in. State/cache writes, call routes,
request preservation and stack balance agree; streamed loading and GPU
execution remain outside this test.

## Boundary

- `0x516d0`, just before, is `todo` with source in `src/unknown_050690.cpp` and
  ends at `0x52020`. `0x52d40`, just after (1,136 bytes), is `todo` with no
  source; it calls `function_142f0`, `function_15180`, `function_1c590` and
  several of the same Direct3D routines, so it looks like a sibling (inferred;
  it was not analysed). Both are excluded.
- This function has nothing to do with the liquid draw callback `0x508d0` or
  the noise points of `0x516d0`: neither calls it, it calls neither, and it
  only neighbours them by address. It belongs with the interface drawing code
  (its three callers) and with the rasterizer functions of
  `src/unknown_01cf50.cpp` (inferred).
- Lane D's row of the Active claims table (issue #9: `0x050000`–`0x06ffff`)
  covers the range by address. Two callers, `0x13c050` and `0x13e703`, lie in
  lane P's range (`0x130000`–`0x13ffff`), and the stub for `function_52040` is
  in `src/stubs/lane_p.cpp`; the third, `0x23675a`, is in the UI-core lane's
  range. This document makes no claim and is offered to lane D, with a note for
  lane P.
- `function_52040` has a `@stub` marker and `0x52020` has none; see
  [Existing declarations](#existing-declarations).

## Conventions

"Callers" counts the functions that call an entry directly, from outside the
range and then from inside it. `0x52020` has none: it is reached only through a
pointer, as described below.

| Retail | Bytes | Arguments | Returns | Callers | What it does |
| --- | --- | --- | --- | --- | --- |
| `0x52020` | 24 | stack: destination, byte count, source (`ret 0xc`) | `al` = 1 | 0 + 0 (by pointer) | Copies the four vertices (20 dwords) |
| `0x52040` | 3316 | stack: request, vertices (`ret 8`) | void | 3 + 0 | Draws one textured quad |

There are no register inputs to either entry. `0x52040` returns nothing that is
used: on the normal path `eax` at the `ret` (`0x52d04`) is the dirty-flags word
stored at `0x52cf0`, and on the early exit it is whatever the caller left. It
clobbers `eax`, `ecx`, `edx` and `xmm0`–`xmm7`. `sub esp,0xbc` and the pushes
of `ebp` and `edi` come first (`0x52040`–`0x5204e`); `ebx` and `esi` are pushed
only after the early-exit test (`0x5205e`, `0x5205f`). Below, L is `esp` after
those four pushes (the entry `esp` less `0xcc`), so the request is at `L+0xd0`
and the vertices at `L+0xd4`; the vertices are read once, at `0x52ccb`. The
locals are: `L+0x10` (the stage counter, and a temporary on path A), `L+0x14`
and `L+0x68` (path A temporaries), the twenty reals `transform` at
`L+0x18`–`L+0x67` and the twenty-four reals `constants` at `L+0x6c`–`L+0xcb`.
The `transform` space is reused for three `color4f` records at `L+0x18`,
`L+0x28` and `L+0x38` once it has been uploaded.

`ebp` holds the request until path A overwrites it with 1 (`0x52670`). `ebx` is
0 in the texture loop. `edi` is 0 from `0x5204f` for most of the body and is
re-zeroed at `0x5246d`, `0x52497`, `0x52878` and `0x52c6e`; the texture loop
uses it as the bitmap pointer and then as the `1 << stage` mask (`0x524b9`,
`0x524fb`–`0x5258c`), so when the loop ends it is 0 if it stopped at a null
bitmap and 4 if all three stages ran. Path A loads it with the value for
`function_0222d0` (`0x526aa` on).

**The callers.** Three functions call `function_52040`, all with `call rel32`
and both arguments pushed (the vertices first). No jump, conditional jump or
32-bit pointer in any section refers to `0x52040`. Each zeroes the 0x98-byte
request with `rep stosd` (0x26 dwords) and stores 1.0 in the reals at `+0x28`,
`+0x2c`, `+0x40` and `+0x44`.

| Caller | Call at | Bytes | Source | What it passes |
| --- | --- | --- | --- | --- |
| `0x13c050` | `0x13c1d1` | 396 | `src/unknown_13c050.cpp` | A rectangle sampling a bitmap of tag data with u = v = 0 on all four vertices; blend mode 0; the bitmap is the tag data's `+0x48` pointer plus `0x74` (`0x13c143`–`0x13c151`). Skipped unless `g_4e0350` is not 0 (`0x13c05c`–`0x13c066`) |
| `0x13e703` | `0x13e890` | 411 | `src/unknown_13e635.cpp` | A rotated and scaled bitmap draw; blend mode 7 (`0x13e887`); the bitmap is `esi` (`0x13e88d`) |
| `0x23675a` | `0x23686b` | 285 | `src/unknown_236299.cpp` | A textured rectangle; the blend mode is its mode argument (`0x236832`–`0x236836`), the bitmap its `+0x70` argument (`0x23683a`); skipped when `function_12360` (bitmap, 0.0) returns 0 (`0x236854`) |

In every case the record pointer at request `+0`, the second and third bitmaps,
both selector words and the byte at `+0x80` are zero. So the known callers only
ever run path B with one bitmap and two combiner stages (`PSCombinerCount`
`0x11102`); the rest of the function is written but not reached by them.

## Data

**The request** is a 0x98-byte block, read only: `s_13c050`
(`src/unknown_13c050.cpp:22`) and `s_interface_draw_request`
(`src/unknown_236299.cpp:636`) both declare its first member as a `dword` and
most of the rest as opaque bytes. `function_35b90`
(`src/unknown_01cf50.cpp:4974`) reads the same layout, except that it never
reads `+0x00`, `+0x80`, `+0x90` or `+0x92`, and it reads `+0x97`, which this
function does not.

| Offset | Meaning | Use here |
| --- | --- | --- |
| `+0x00` | Pointer to a record (the sources type it `dword`) | Null selects path B, non-null path A. No known caller sets it |
| `+0x04` | Pointer to two reals, or null | The screen offset (x, y); null gives 0.0, 0.0 |
| `+0x08`, `+0x09`, `+0x0a` | Three bytes | Each makes a pair in `constants[4..9]`: non-zero gives (1.0, 0.0), zero gives (0.0, 1.0) |
| `+0x0c`, `+0x10`, `+0x14` | Three bitmap pointers (`s_bitmap_view *`) | Stages 0, 1 and 2. The texture loop stops at the first null; a null `+0x0c` skips the program build |
| `+0x18`, `+0x19`, `+0x1a` | Three bytes | One per stage: non-zero gives address mode 1, zero gives 3, for both U and V |
| `+0x1c`, `+0x20`, `+0x24` | Three pointers to two reals | Copied to `constants[10..15]`; null gives 0.0, 0.0 |
| `+0x28`–`+0x3c` | Six reals | `constants[16..21]`; `+0x28` and `+0x2c` are 1.0 from every caller |
| `+0x40`, `+0x44` | Two reals | `transform[16]`, `transform[17]`; 1.0 from every caller |
| `+0x48`–`+0x54` | Four reals | `constants[0..3]` |
| `+0x58`, `+0x60`, `+0x68` | Three pointers to `color3f` (an 8-byte stride; the odd dwords are not read) | Colours 0, 1 and 2; null selects the default, three 1.0 reals |
| `+0x70` | An embedded `color4f` (alpha, red, green, blue) | Packed four times, into `PSConstant0[4..7]` |
| `+0x80` | Byte | Read once, at `0x52b6b`: in the second selector's case 0, when bitmap 2 is set and the first selector is 5 |
| `+0x84`, `+0x88`, `+0x8c` | Three pointers to reals | The alpha of colours 0, 1 and 2; null gives 1.0 |
| `+0x90` | Word | The first combiner selector (0 to 5 are valid) |
| `+0x92` | Word | The second combiner selector (0 to 4 are valid) |
| `+0x94` | Word | The blend mode, the argument of `function_142f0` |
| `+0x96` | Byte | The filter flag: non-zero gives filter value 1, zero gives 2 (for MAG, MIN and MIP) |
| `+0x97` | Byte | Not read |

**The vertices** are four records of 20 bytes: x, y, u, v and a colour dword
(`s_13c052`, `src/unknown_13c050.cpp:15`; `s_interface_draw_vertex`,
`src/unknown_236299.cpp:629`). `function_52040` never reads them; `0x52020`
copies all 80 bytes.

**The record at request `+0`** (path A only, read through `ebx`) has at least
0x1c bytes. No known caller builds one, so its purpose is unknown (a tint or
fade overlay is only a guess).

| Offset | Meaning | Use here |
| --- | --- | --- |
| `+0x00` | Dword colour | `PSConstant0[0]`, `[1]` and `[2]` |
| `+0x04` | Dword colour | `PSConstant1[1]`, and with its top byte replaced `PSConstant1[0]` |
| `+0x08` | Dword colour | `PSConstant0[3]` |
| `+0x0c` | Dword colour | `PSConstant1[2]` |
| `+0x10` | Byte | Non-zero ORs `0xe0` into `PSRGBInputs[2]` |
| `+0x14` | Dword | `PSConstant1[3]`, and the value of `D3DRS_BLENDCOLOR` |
| `+0x18` | Real | The top byte of `PSConstant1[0]`: `n` = `fistp` (255.0 × (1.0 / max(1.0, value × 8.0))), as under path A |

**The program** `g_484f68` is a `D3DPIXELSHADERDEF`
(`src/unknown_01cf50.cpp:15`). The names below are the ones `function_35b90`
uses; the addresses are those of element 0, from the stores in this function
(the layout is inferred from them).

| Field | Retail address |
| --- | --- |
| `PSAlphaInputs[i]` | `0x484f68` + 4i |
| `PSConstant0[i]` | `0x484f90` + 4i |
| `PSConstant1[i]` | `0x484fb0` + 4i |
| `PSAlphaOutputs[i]` | `0x484fd0` + 4i |
| `PSRGBInputs[i]` | `0x484ff0` + 4i |
| `PSRGBOutputs[i]` | `0x48501c` + 4i |
| `PSCombinerCount` | `0x48503c` |
| `PSTextureModes` | `0x485040` |

The combiner words are given below as raw numbers, as `docs/unknown_03a5c0.md`
gives them. The operations named for them are an inferred decode of the input
and output word layouts (the low nibble of an input byte is the register, with
1 = C0, 2 = C1, 4 = v0, 8–0xb = t0–t3 and 0xc = r0), not a repository document.

**Game state it reads and writes.**

| Name or address | What it is |
| --- | --- |
| `g_485602` | Short; if it is not 0 the function returns at once (`src/unknown_01cf50.cpp:474`). `function_35b90` and `function_363a0` test it too |
| `g_485648`, `g_48564a`, `g_48564c`, `g_48564e` | Words, the viewport rectangle (`src/unknown_020560.cpp:106`–`109`); width = (short)(`g_48564e` − `g_48564a`), height = (short)(`g_48564c` − `g_485648`) |
| `g_4b81fc[state]` | The game's cache of render states (`src/unknown_020560.cpp:110`). Slots written here: `0x4b8448` CULLMODE (state 0x93), `0x4b8308` COLORWRITEENABLE (0x43), `0x4b82e8` ALPHABLENDENABLE (0x3b), `0x4b82ec` ALPHATESTENABLE (0x3c), `0x4b82f4` SRCBLEND (0x3e), `0x4b8438` ZENABLE (0x8f), `0x4b8450` ZBIAS (0x95) |
| `0x407488` | The Direct3D device pointer (inferred). `[device]` is the push-buffer write pointer and `[device+4]` its limit (inferred from use) |
| `0x407490` | The dirty-flags word (inferred); bit 0 is stage 0's, and the texture loop sets bit `stage` |
| `0x407498` + 4 × state | The Direct3D library's own copy of each render state (inferred); every state set here is also written to its copy |
| `0x404dd0` + stage × 0x80 + 4 × type | The texture-stage-state array |
| `0x404fe0`, `0x404fe4`, `0x404fe8` | Push pointer, limit and a flag byte of the device object (inferred) |
| `0x408248`, `0x408298` | Shadow copies of vertex-shader constant registers 81 and 86 (inferred: 16 bytes per register, 5 registers apart) |
| `g_51f0f0` | The shader cache (`s_shader_cache`, `src/unknown_01cf50.cpp:1182`): `wanted[0]` at `0x51f17c`, `current[0]` at `0x51f1bc`, `descriptors_changed` at `0x51f2fc` |
| `g_51f3c8[2][4]` | The textures: `[1][stage]` (`0x51f3d8` + 4 × stage) is set here; `function_1cf50` binds the ones that differ from `[0]` |
| `g_485af4[4]`, `g_485b04[4]` | Long arrays; the bitmap's two shorts (`+4`, `+6`) per stage (`src/unknown_01cf50.cpp:568`) |
| `g_485a80` | Pointer; its dword at `+0x5c` points to 8-byte shader slots (`s_shader_slot`, `src/unknown_01cf50.cpp:1318`), with the tag at `+4`. Slot 14 is used |
| `g_468710` | Pointer (`src/unknown_162550.cpp:26`) to the default colour, three 1.0 reals (`0x4409e4`) |
| `g_43f408[36]` | The immediate vertex descriptor of format 36 (`0x43f6fc`; `src/unknown_01cf50.cpp:1547`) |
| `g_43fdc0` | `function_4b220`'s primitive table, {1, 2, 5, 8, 6} |

**Constants** read from `.rdata`: `0x45dc0c` (2.0), `0x45df10` (-2.0),
`0x45dbc0` (1.0), `0x45dbbc` (0.5), `0x45dc18` (8.0) and `0x45dc38` (255.0).

## The function

`function_52040` is close to `function_35b90`: it does the same state and
program setup for the same 0x98-byte request. It differs in selecting slot 14
and format 36 itself (`function_35b90` ends with `function_1cc60(0)`), and in
flushing the textures and drawing the quad, which `function_35b90` leaves to
its caller. The select, flush and draw sequence is the one `function_35b00`
uses for its own, different payload. In order: (1) return at once if `g_485602`
is not 0; (2) set a fixed render state and the request's blend mode; (3) build
the screen-to-clip `transform` and the per-draw `constants` and upload them to
vertex-shader registers 81 to 91; (4) fetch the textures and set the
texture-stage states for stages 0 to 2; (5) select the vertex shader in slot 14
and the vertex descriptor 36; (6) build the program in `g_484f68`, along path A
(a fixed four-stage program from a record, when request `+0` is non-null) or
path B (texture 0 times colour 0 times the vertex colour, with bitmaps 1 and 2
optionally combined by two selector words); (7) bind it, set the cull mode and
the shader again, flush the textures and draw the four vertices as a quad list.

The control flow has 93 basic blocks (counting the `nop` at `0x52d07`) and one
loop, the texture loop (`0x524b1`–`0x52610`). The early exit is the
`jne 0x52cfc` at `0x52058`; the only `ret` is at `0x52d04`. Three tests
(`0x522c2`, `0x522e1` and `0x52303`) have out-of-line `else` blocks, at
`0x5232d`, `0x52338` and `0x52343`, that end by jumping back to `0x522d3`,
`0x522f5` and `0x52317`; they are not loops. The only other backward jump is
the loop's `jl` at `0x52610`. A stack-depth trace over every path found no
conflicting depth at a join (196 bytes beyond the entry depth at `0x52cfc`) and
depth 0 at the `ret`.

### The fill callback (`0x52020`–`0x52037`)

1. It saves `esi` and `edi`, loads the source (the third stack argument) into
   `esi` and the destination (the first) into `edi` (`0x52020`–`0x52029`).
2. `ecx` = 0x14 and `rep movsd` copies 20 dwords (80 bytes), whatever byte
   count the second argument holds (`0x5202a`–`0x5202f`).
3. It restores the registers, sets `al` to 1 and returns with `ret 0xc`
   (`0x52031`–`0x52035`). The second argument is never read.

The only reference to `0x52020` in the image is the `push 0x52020` at
`0x52cd3`. `function_4b220` calls it through `call dword ptr [esp+0x28]`
(`0x4b282`) with the push pointer in `esi`, the byte count in `ebx` and the
context pushed at `0x4b27a`–`0x4b281`, so its stack arguments are (push
pointer, bytes, context); it drops the result (`0x4b28c`). The count of 20
dwords is fixed in the callback and fits the single draw call of
`function_52040` (`ecx` 0x14, four vertices).

### Entry and the fixed render state (`0x52040`–`0x52150`)

1. It reserves the frame, pushes `ebp` and `edi`, loads the request into `ebp`
   (`0x52047`) and zeroes `edi` (`0x5204f`). If the word `g_485602` is not 0 it
   jumps to the shared epilogue (`0x52051`–`0x52058`), where only `edi` and
   `ebp` are restored; nothing is drawn or written.
2. CULLMODE = 0: `g_4b8448` = 0 (`0x52061`), `push edi`, call `0x3f53d0`
   (`0x52067`).
3. COLORWRITEENABLE = `0x10101`: `g_4b8308` (`0x52078`); `0x3f5cf0` with `ecx`
   = `0x40358` and `edx` = `0x10101` (`0x5207e`); the library copy at
   `0x4075a4` (`0x52083`). This is red, green and blue without alpha (inferred:
   the all-channel value elsewhere is `0x1010101`,
   `src/unknown_01cf50.cpp:144`–`145`).
4. ALPHABLENDENABLE = 1: `g_4b82e8` (`0x52095`); `0x3f5cf0` with `ecx` =
   `0x40304`, `edx` = 1 (`0x5209b`); the copy at `0x407584` (`0x520a7`).
   ALPHATESTENABLE = 0: `g_4b82ec` (`0x520ad`); `0x3f5cf0` with `ecx` =
   `0x40300`, `edx` = 0 (`0x520b3`); the copy at `0x407588` (`0x520be`).
5. ZENABLE = 0, inline: the body of `0x3f54f0` for the value 0 (compare
   [A depth-range pixel shader](unknown_03a5c0.md)). `g_4b8438` = 0
   (`0x520c4`); `esi` is the device (`0x520b8`). If `[esi]` is not below
   `[esi+4]` it calls `0x3faae0` for room (`0x520ca`–`0x520db`). It writes
   method `0x4030c` ← 0 and method `0x41d78` ← `[0x4075e0]` (the library copy
   of state 0x52, DEPTHCLIPCONTROL) and advances `[esi]` by 0x10
   (`0x520e0`–`0x520fc`), then reads the library copy `[0x4076d4]` and stores 0
   there (`0x520fe`–`0x52106`). Only when the old value was 2 (`0x52103`,
   `0x5210c`) it calls `0x3f6840` and `0x3f6f40`, makes room again, and calls
   `0x3f6390` and `0x3f6190`, storing the new pointer to `[esi]`
   (`0x5210e`–`0x5213b`).
6. ZBIAS = 0: `g_4b8450` = 0 (`0x5213e`); `push edi` and call `0x3f59b0`
   (`0x52144`).
7. The blend mode: `ax` = the word at request `+0x94`; call `function_142f0`
   (`0x52149`–`0x52150`). It sets ALPHABLENDENABLE to (mode is not 10) again,
   overwriting the 1 above, and, unless the mode is 10, SRCBLEND, DESTBLEND and
   BLENDOP from the tables at `0x450718`, `0x4506e8` and `0x4506b8`
   (`src/unknown_0494b0.cpp:50`). Mode 0 gives SRCALPHA `0x302`, INVSRCALPHA
   `0x303` and ADD `0x8006`; mode 7 gives ONE 1, INVSRCALPHA `0x303` and ADD
   `0x8006`.

### The vertex-shader constants (`0x52155`–`0x524a4`)

1. width = (short)(`g_48564e` − `g_48564a`) (`0x52155`, `0x5216a`); height =
   (short)(`g_48564c` − `g_485648`) (`0x5215b`, `0x52161`, `0x5217c`).
2. `offset` is the pointer at request `+4` (`0x52167`). x = offset ? offset[0]
   × 2.0 / width : 0 (`0x52180`–`0x52197`); y = offset ? offset[1] × -2.0 /
   height : 0 (`0x5219a`–`0x521bc`). `inverse_x` = 1.0 / width (`0x521d6`);
   `inverse_y` = 1.0 / height (`0x521f5`).
3. `transform[20]` (at `L+0x18`): [0] = `inverse_x` × 2.0 (`0x52254`,
   `0x5225c`); [3] = x − (`inverse_x` + 1.0) (`0x521dd`–`0x521e8`); [5] =
   `inverse_y` × -2.0 (`0x52258`, `0x52274`); [7] = `inverse_y` + y + 1.0
   (`0x521fc`–`0x52204`); [11] = 0.5 (`0x5220a`–`0x52212`); [15] = 1.0
   (`0x522a4`); [16] and [17] = the reals at request `+0x40` and `+0x44`
   (`0x52218`, `0x52223`); [19] = 1.0 (`0x522b0`); every other element is 0.0
   (`0x52262`–`0x522aa`).
4. `constants[24]` (at `L+0x6c`): [0..3] = the reals at request `+0x48`,
   `+0x4c`, `+0x50`, `+0x54` (`0x5222e`–`0x522b6`); [4..9] are three pairs from
   the bytes at request `+8`, `+9` and `+0xa`, (1.0, 0.0) when the byte is
   non-zero and (0.0, 1.0) otherwise (`0x521ce`, `0x522bc`–`0x522ec`,
   `0x522f5`–`0x5230e`, `0x5232d`–`0x52343`); [10..15] are three (x, y) pairs
   read through the pointers at request `+0x1c`, `+0x20` and `+0x24`, 0.0 for a
   null pointer (`0x52317`–`0x523ec`); [16..21] = the six reals at request
   `+0x28` to `+0x3c` (`0x523ec`–`0x5243b`); [22] and [23] = 0.0 (`0x52449`,
   `0x52452`). These are the `transform[20]` and `constants[24]` of
   `function_35b90` (`src/unknown_01cf50.cpp:5001`–`5043`).
5. The upload is done twice, as an inline `D3DDevice_SetVertexShaderConstant`.
   Unless bit 4 of the byte at `0x404fe8` is set (`0x52430`, `0x5245b`), the
   first copies 0x14 dwords from `transform` to the shadow at `0x408248`
   (`0x5245d`–`0x5246b`); then `push 0x14`, `edx` = `transform`, `ecx` = `0xb1`
   (register 81 plus 96) and call `0x3f7330` (`0x5246f`–`0x5247a`). The second
   does the same for `constants`: 0x18 dwords to `0x408298`, `ecx` = `0xb6`
   (register 86 plus 96) (`0x52487`–`0x524a4`). The source form is
   `D3DDevice_SetVertexShaderConstant(81, transform, 5)` and
   `(86, constants, 6)` (`src/unknown_01cf50.cpp:5044`–`5045`).

### The texture loop (`0x524a9`–`0x52610`)

This is the only loop. `[L+0x10]` (the stage) and `ebx` are 0 before it
(`0x524a9`, `0x524ad`). It is entered at `0x524b4`, skipping the
`xorps xmm2,xmm2` at `0x524b1` that zeroes `xmm2` on later passes; on the first
pass `xmm2` still holds the 0.0 written at `0x52179` (inferred: nothing between
writes it, and the buffer-full path of `0x3faae0` was not examined for `xmm`
use). Each pass:

1. `esi` = (short)`[L+0x10]` (`0x524b4`); `edi` = the bitmap pointer
   `[ebp+0xc+4*esi]` (`0x524b9`). If it is null the loop ends at `0x52616`
   (`0x524bd`–`0x524bf`).
2. `eax` = the bitmap; call `function_12310` (the priority is `xmm2`, 0.0;
   `0x524c5`–`0x524c7`). The result goes to `g_51f3c8[1][stage]`, the dword at
   `0x51f3d8` + 4 × stage (`0x524cc`).
3. `g_485af4[stage]` = the short at bitmap `+4` (`0x524d3`, `0x524db`) and
   `g_485b04[stage]` = the short at bitmap `+6` (`0x524d7`, `0x524e4`).
4. Ten stores into the texture-stage-state array at `0x404dd0` + stage × 0x80 +
   4 × type (`ecx` = stage << 7): `+0x00` and `+0x04` = 1 if the byte
   `[ebp+0x18+stage]` is non-zero, else 3 (`0x524eb`–`0x5252c`; ADDRESSU and
   ADDRESSV, `src/unknown_01cf50.cpp:5054`–`5055`); `+0x0c`, `+0x10` and
   `+0x14` = 1 if the byte `[ebp+0x96]` is non-zero, else 2
   (`0x52532`–`0x52592`; MAGFILTER, MINFILTER and MIPFILTER, `:5056`–`5058`);
   then 0 to `+0x20`, `+0x18`, `+0x1c`, `+0x28` and `+0x2c`, in that order
   (`0x525a6`, `0x525ba`, `0x525d0`, `0x525e2`, `0x525f6`). Each store ORs
   `1 << stage` into the dirty word `[0x407490]` (read at `0x524f4`, written
   once at `0x525fe`).
5. The loop continues while (word)(stage + 1) is below 3 (`0x52603`–`0x52610`).

The five zero stores are named MAXANISOTROPY, MIPMAPLODBIAS, MAXMIPLEVEL,
COLORSIGN and ALPHAKILL (inferred), from the order and offsets of the stores in
retail `0x32ac0` (`0x32b38`–`0x32b55`), whose source lists them so
(`src/unknown_01cf50.cpp:128`–`132`). `function_35b90`'s source lists
MAXMIPLEVEL first (`:5059`–`5063`).

### The vertex shader and the descriptor (`0x52616`–`0x52665`)

1. `eax` = `g_485a80`; `eax` = the dword at `+0x5c`, plus `0x70`; `eax` = the
   dword at `+4` there, the tag of shader slot 14 (`0x52616`–`0x52621`).
   `push ebx` (the index, 0), `esi` = `g_51f0f0`, and call `function_1c590`
   (state in `esi`, tag in `eax`, index on the stack; `0x52624`–`0x5262a`).
2. The inlined `select_immediate_descriptor(36)`
   (`src/unknown_01cf50.cpp:2420`–`2429`): `edx` = `esi` and call
   `function_1c6b0` (`0x5262f`–`0x52631`); then `wanted[0]` (`0x51f17c`) =
   `0x43f6fc`, the address of `g_43f408[36]` (`0x5263c`–`0x52643`); if
   `current[0]` (`0x51f1bc`) is not `0x43f6fc`, `descriptors_changed`
   (`0x51f2fc`) = 1 (`0x52636`, `0x52641`, `0x52648`–`0x5264a`). Then
   `push 0x51f0f0` and call `function_1c710` twice (`0x52651`–`0x52660`). The
   pair is the inline select's own `function_1c710` and an explicit
   `function_1c710(g_51f0f0)` (inferred from retail `0x35b00`, at
   `0x35b4e`–`0x35b5d`, whose source has `select_immediate_descriptor` followed
   by `function_1c710(g_51f0f0)`; `0x35b00` is `todo`, so that source is
   unproven). There is no texture flush here; that happens at `0x52cc6`.
3. `ebx` = the dword at request `+0`; if it is 0 the function jumps to path B
   at `0x52857`, otherwise it runs path A (`0x52665`–`0x5266a`).

### Path A, request `+0` non-null (`0x52670`–`0x52852`)

No known caller reaches this path. `ebx` points to the read-only record
described under Data.

1. `ebp` = 1 (`0x52670`); the request is not read again on this path.
2. State: ALPHABLENDENABLE = 1 again (`g_4b82e8` at `0x5267c`; `0x3f5cf0` with
   `ecx` = `0x40304`, `edx` = 1 at `0x52682`; the copy `0x407584` at
   `0x52693`). SRCBLEND (0x3e) = `0x8001` (`D3DBLEND_CONSTANTCOLOR`):
   `g_4b82f4` (`0x52699`); `0x3f5cf0` with `ecx` = `0x40344`, `edx` = `0x8001`
   (`0x5269f`); the copy `0x407590` (`0x526a4`). `function_0222d0` (state in
   `esi`, value in `edi`) then sets DESTBLEND (0x3f) = `0x302` (SRCALPHA) at
   `0x526b4`, BLENDCOLOR (0x4b) = the dword at record `+0x14` at `0x526c1`, and
   BLENDOP (0x4a) = `0x8006` (ADD) at `0x526d0`.
3. `[0x407490]` is ORed with 1 (`0x526d5`–`0x526e4`). `g_484f68` is cleared
   with `rep stosd` (0x3c dwords, `0x526e9`–`0x526f5`). The texture-stage word
   `0x404dfc` (stage 0, `+0x2c`) is set to 4 (`0x526f7`; the alpha-kill enable,
   inferred; it is reset to 0 at `0x52cf5`).
4. `PSTextureModes` = 1 (`0x52701`); `PSCombinerCount` = `0x11104` (`0x52707`).
5. Constants: `PSConstant0[0]`, `[1]` and `[2]` = the dword at record `+0`
   (`0x52713`, `0x5279b`, `0x527d4`); `PSConstant0[3]` = record `+8`
   (`0x52817`); `PSConstant1[1]` = record `+4` (`0x527a3`); `PSConstant1[2]` =
   record `+0xc` (`0x527dd`); `PSConstant1[3]` = record `+0x14` (`0x52820`);
   `PSConstant1[0]` = (record `+4` & `0xffffff`) \| (n << 24), with n = `fistp`
   (255.0 × (1.0 / max(1.0, record `+0x18` × 8.0))) (`0x52719`–`0x52770`). 8.0
   and 255.0 are `0x45dc18` and `0x45dc38`, and the `comiss`/`ja` keeps 1.0 for
   a NaN. This is the body of `function_131fc0` (`src/unknown_131e50.cpp:115`)
   inlined.
6. The four stages, written as raw words:

| Stage | `PSAlphaInputs` | `PSAlphaOutputs` | `PSRGBInputs` | `PSRGBOutputs` |
| --- | --- | --- | --- | --- |
| 0 | `0x12081208` (`0x52776`) | `0x20c00` (`0x5278f`) | `0x1120e820` (`0x52780`) | `0x20c00` (`0x52794`) |
| 1 | `0x6c200000` (`0x527a9`) | `0xc0` (`0x527b3`) | `0x3c011c02` (`0x527bd`) | `0xc00` (`0x527cc`) |
| 2 | `0x0820b220` (`0x527e2`) | `0xc00` (`0x527ec`) | `0x0c201c02`, ORed with `0xe0` when the byte at record `+0x10` is non-zero (`0x527f2`–`0x5280e`) | `0xc00` (`0x527f7`) |
| 3 | `0x12201120` (`0x5282a`) | `0x4c00` (`0x52834`) | `0x0c200120` (`0x52839`) | `0x4c00` (`0x52843`) |

7. `PSFinalCombinerInputsABCD` = `0x0c180000` (`0x52848`); the path then jumps
   to `0x52c58`, where the final combiner's second word is set and the program
   is bound. The meaning of the record is unknown.

### Path B, request `+0` null (`0x52857`–`0x52c58`)

All three known callers run only this path, with bitmap 0 set and nothing else.

1. **The guard** (`0x52857`–`0x528a0`). `eax` = bitmap 0 (`[ebp+0xc]`). If it
   is null the function jumps to `0x52c62` (`0x5285c`): nothing is built, and
   whatever `g_484f68` already holds is bound and drawn (`function_35b90` binds
   only inside its equivalent guard). Otherwise `g_484f68` is cleared with
   `rep stosd` (0x3c dwords, `0x52862`–`0x5286e`) and `PSTextureModes`
   (`0x485040`) is set to ((bitmap 2 is set) << 10) \| ((bitmap 1 is set) << 5)
   \| (bitmap 0 is set), with bitmap 1 at `[ebp+0x10]`, bitmap 2 at
   `[ebp+0x14]` and `edx` = `[0x468710]` loaded on the way
   (`0x52870`–`0x528a0`). The same expression is in `function_35b90`
   (`src/unknown_01cf50.cpp:5069`–`5070`).
2. **The three colours** (`0x528a6`–`0x529ad`). For i = 0, 1, 2 the colour
   pointer is `[ebp+0x58+8*i]` (`0x528a6`, `0x528c2`, `0x528da`); a null
   pointer selects the one at `0x468710` (`cmove`, three reals of 1.0). Its
   red, green and blue are copied into local `color4f` records at `L+0x18`,
   `L+0x28` and `L+0x38` (stored in the order alpha, red, green, blue;
   `0x528ae`–`0x528f3`). The alpha is the real through the pointer
   `[ebp+0x84+4*i]`, or 1.0 (`0x45dbc0`) when that pointer is null
   (`0x528f6`–`0x52955`). These records overlay `transform`, which is dead by
   now. Then seven `cdecl` calls of `pack_color4f` (the caller pushes the
   pointer; one `add esp,0x1c` at `0x529fb` removes all seven): `colors[0]` →
   `PSConstant0[0]` (call `0x5295b`, store `0x52965`); `colors[1]` →
   `PSConstant1[0]` (`0x5296a`, `0x52974`); `colors[2]` → `PSConstant0[1]`
   (`0x52979`, `0x52982`); and the `color4f` at request `+0x70`
   (`lea esi,[ebp+0x70]`, `0x5297e`) → `PSConstant0[4]`, `[5]`, `[6]` and `[7]`
   (calls `0x52987`, `0x52992`, `0x5299d`, `0x529a8`; stores `0x5298d`,
   `0x52998`, `0x529a3`, `0x529ad`). This is `function_35b90`
   (`src/unknown_01cf50.cpp:5071`–`5088`).
3. **Stages 0 and 1**, always built (`0x529b2`–`0x529f3`): `PSRGBOutputs[0]`
   and `PSAlphaOutputs[0]` = `0x89` (`0x529b7`, `0x529bc`); `PSRGBInputs[0]` =
   `0x08010902` (`0x529c6`); `PSAlphaInputs[0]` = `0x18111912` (`0x529d0`);
   `PSRGBInputs[1]` = `0x0a010804` (`0x529da`); `PSRGBOutputs[1]` and
   `PSAlphaOutputs[1]` = `0xac` (`0x529e4`, `0x529f3`); `PSAlphaInputs[1]` =
   `0x1a111814` (`0x529e9`). Inferred decode: stage 0 computes t0 = t0 × C0 and
   t1 = t1 × C1, and stage 1 computes t2 = t2 × C0 and r0 = t0 × v0. With the
   final combiner below (ABCD `0xc` is r0; EFG `0x1c00` is the alpha of r0) the
   default result is texture 0 times colour 0 times the vertex colour, alpha
   likewise.
4. **The first selector**, the word at request `+0x90` (`0x529f8`–`0x52b3a`).
   `ecx` = 2 (the running stage count), `edx` = `0xc0`, `esi` = `0xc00`, `ebx`
   = `0x100c0` (`0x52a00`–`0x52a0f`). If bitmap 1 (`[ebp+0x10]`) is null the
   block is skipped, with its increment (`0x52a14`). Otherwise `eax` = the word
   sign-extended (`0x52a1a`); `cmp eax,5` and `ja 0x52b3a`
   (`0x52a21`–`0x52a24`; unsigned, so a negative value or 6 or more writes
   nothing for stage 2); `jmp [eax*4+0x52d08]` (`0x52a2a`). Every case ends at
   `0x52b3a` with `inc ecx`, so the count is 3 after cases 0 to 4 or an
   out-of-range selector, and 6 after case 5. Cases 0, 2 and 5 meet at
   `0x52b2e` (both stage-2 outputs `0xc00`); cases 1 and 4 meet at `0x52a9a`.
   The words written for stage 2:

| Case | Target | `PSRGBInputs[2]` | `PSAlphaInputs[2]` | `PSRGBOutputs[2]` | `PSAlphaOutputs[2]` | Inferred: r0 with t1 |
| --- | --- | --- | --- | --- | --- | --- |
| 0 | `0x52a31` | `0x0c200920` | `0x1c201920` | `0xc00` | `0xc00` | add |
| 1 | `0x52a4a` | `0x0c090000` | `0x1c190000` | `0xc0` | `0xc0` | multiply |
| 2 | `0x52a52` | `0x0c20e920` | `0x1c20f920` | `0xc00` | `0xc00` | subtract |
| 3 | `0x52a6b` | `0x0c090000` | `0x1c190000` | `0x100c0` | `0x100c0` | multiply, doubled |
| 4 | `0x52a90` | `0x0c090000` | `0x1c190000` | `0x20c0` | `0xc0` | RGB dot product; alpha multiplied |
| 5 | `0x52ab9` | `0x1920b820` | `0x0820a920` | `0xc00` | `0xc00` | a six-stage program (not decoded) |

Case 5 (`0x52ab9`–`0x52b34`) also writes the later stages: `PSAlphaInputs[3]` =
`0x1c1c0c0c`, `PSAlphaOutputs[3]` = `0x24c00`, `PSRGBInputs[3]` and
`PSRGBOutputs[3]` = 0; `PSAlphaInputs[4]` = `0x5c5c`, `PSAlphaOutputs[4]` =
`0x4d00`, `PSRGBInputs[4]` and `PSRGBOutputs[4]` = 0; `PSAlphaInputs[5]` = 0,
`PSAlphaOutputs[5]` = `0xc00`, `PSRGBInputs[5]` = `0x1ca01da0`,
`PSRGBOutputs[5]` = `0xc00`; and `ecx` = 5.

5. **The second selector**, the word at request `+0x92` (`0x52b3b`–`0x52c40`).
   Only if bitmap 2 (`[ebp+0x14]`) is non-null; otherwise `je 0x52c41` skips
   the block and its increment (`0x52b3b`–`0x52b3e`). `eax` = the word
   sign-extended (`0x52b44`); `cmp eax,4` and `ja 0x52c40`
   (`0x52b4b`–`0x52b4e`, which only increments `ecx`); `jmp [eax*4+0x52d20]`
   (`0x52b54`). Each case writes the stage numbered by the running count: `eax`
   = (short)`ecx` × 4 is the byte offset into the four arrays (`movsx eax,cx`
   and `shl eax,2` at the start of each case).

| Case | Target | `PSRGBInputs[stage]` | `PSRGBOutputs[stage]` | `PSAlphaInputs[stage]` | `PSAlphaOutputs[stage]` |
| --- | --- | --- | --- | --- | --- |
| 0 | `0x52b5b` | the first selector is 5: `0x0c010a00` \| (byte at `+0x80` non-zero ? 4 : `0x20`) (`0x52b61`–`0x52b81`); otherwise `0x0c200a20` (`0x52b89`) | `0xc00` | `0x1c201a20` | `0xc00` |
| 1 | `0x52bae` | `0x0c0a0000` | `0xc0` | `0x1c1a0000` | `0xc0` |
| 2 | `0x52bc6` | `0x0c20ea20` | `0xc00` | `0x1c20fa20` | `0xc00` |
| 3 | `0x52bee` | `0x0c0a0000` | `0x100c0` | `0x1c1a0000` | `0x100c0` |
| 4 | `0x52c16` | `0x0c0a0000` | `0x20c0` | `0x1c1a0000` | `0xc0` |

Inferred: these are the five operations of the first selector again, now for t2
(add, multiply, subtract, multiply by 2, dot product); after first selector 5,
case 0 multiplies r0 by C0 of that stage and adds t2, or t2 × v0 when the byte
at `+0x80` is set. At `0x52c40` `inc ecx` ends every case.

6. **The stage count and the final combiner** (`0x52c41`–`0x52c58`).
   `PSCombinerCount` (`0x48503c`) = `0x11100` \| (short)`ecx`
   (`0x52c41`–`0x52c49`), where `ecx` = 2 + (bitmap 1 is set) + (bitmap 2 is
   set), or 6 + (bitmap 2 is set) after first selector 5.
   `PSFinalCombinerInputsABCD` = `0xc` (`0x52c4e`); `PSFinalCombinerInputsEFG`
   = `0x1c00` (`0x52c58`, also reached from path A). With the three known
   callers the count is 2, `PSCombinerCount` is `0x11102`, and the program is
   the one `function_35b90` builds (`src/unknown_01cf50.cpp:5089`–`5100`).

### Binding the program and drawing (`0x52c62`–`0x52ce2`)

The paths join at `0x52c62` (also the target of the bitmap-0 skip at
`0x5285c`).

1. **Bind** (`0x52c62`–`0x52c86`). `ecx` = `[0x407488]` (the device); `eax` =
   `ecx` + `0x924`; the dword at `[eax]` = 1, `[ecx+0x928]` = 0 (`edi`) and
   `[ecx+0x92c]` = `0x484f68`; then call `0x3fa3d0` with `eax` = that object.
   This is `D3DDevice_SetPixelShaderProgram(&g_484f68)` inlined, with the same
   stores as matched `function_15180` (`0x1518a`–`0x151a6`) and
   `function_1ccf0` (`0x1ccfa`–`0x1cd16`; `src/unknown_01cf50.cpp:962`, `975`),
   and the pointer folded to the constant.
2. **The cull mode** (`0x52c8b`–`0x52c9a`). `push 0x901`; `g_4b8448` = `0x901`;
   call `0x3f53d0`. (The other value, 0, is the first state, at `0x52067`.)
3. **The shader and the descriptor again** (`0x52c9f`–`0x52cc6`). The slot-14
   tag is loaded again (`ecx` = `g_485a80`, `edx` = `[ecx+0x5c]`, `eax` =
   `[edx+0x74]`), `push edi` (0), `esi` = `0x51f0f0`, and call `function_1c590`
   (`0x52c9f`–`0x52cb1`). `eax` = `0x24`; call `function_1ccb0` (the format 36,
   that is `select_immediate_descriptor(36)`, which itself calls
   `function_1c710`; `0x52cb6`–`0x52cbb`). `push esi` and call `function_1c710`
   (`0x52cc0`–`0x52cc1`). Call `function_1cf50` (`0x52cc6`), which calls
   `D3DDevice_SetTexture` for each of the four stages whose wanted texture
   `g_51f3c8[1][i]` differs from `g_51f3c8[0][i]`.
4. **The draw** (`0x52ccb`–`0x52ce2`). `eax` = the vertices (`[L+0xd4]`); push
   `eax`, push `0x52020` (the fill callback), push 4, push 3, push `edi` (0),
   `ecx` = `0x14`, and call `function_4b220`. At its entry the stack is (0, 3,
   4, `0x52020`, vertices) and `ecx` is 0x14; it pops five arguments
   (`ret 0x14`) and returns -1 on every path (`long result = NONE;`,
   `src/unknown_01cf50.cpp:2253`), which this call does not use.
   `function_4b220` (`src/unknown_01cf50.cpp:2251`, `todo`) does nothing unless
   its first stack argument is 0; forms the byte count `ecx` × its third stack
   argument = 0x14 × 4 = 80 (`0x4b22d`–`0x4b22f`); indexes `g_43fdc0` = {1, 2,
   5, 8, 6} with the second (3 gives 8, the `D3DPT_QUADLIST` value that
   `function_1ee60` stores for `D3DDevice::Begin(D3DPT_QUADLIST)` at `0x1ef40`,
   `src/unknown_020560.cpp:921`); begins a push-buffer run; calls the fill
   callback `0x52020` with (push pointer, bytes, fifth argument); and ends the
   run. So this draws four 20-byte vertices as one quad list. Its declaration
   names the first parameter `count` (the register `ecx`) and the fourth
   `stride` (the third stack argument); this call passes 0x14 and 4, the
   reverse of the vertex meaning, and the product is the same; see Existing
   declarations.

### The exit (`0x52ce7`–`0x52d04`)

`[0x407490]` is ORed with 1 (stage 0's bit; `0x52ce7`–`0x52cf0`, with `pop esi`
at `0x52cef`); `[0x404dfc]` = 0 (`0x52cf5`; it undoes path A's 4 at `0x526f7`);
then it pops `ebx`, `edi` and `ebp`, adds `0xbc` to `esp` and returns with
`ret 8` (`0x52cfb`–`0x52d04`).

What is left set for later draws: cull mode `0x901`, colour write `0x10101`, Z
test off, z-bias 0, alpha test off, the program in `g_484f68`, vertex-shader
slot 14 with descriptor 36, and texture stages 0 to 2 (with their `g_51f3c8`,
`g_485af4` and `g_485b04` slots). The blend state follows the request's mode on
path B; path A leaves the constant-colour blend (SRCBLEND `0x8001`, DESTBLEND
`0x302`, BLENDOP `0x8006`, BLENDCOLOR from record `+0x14`). Unlike
`function_35b90` and `function_35b00`, this function neither writes `g_4670bc`
nor calls `function_16b10`.

### The jump tables (`0x52d07`–`0x52d33`)

A `nop` at `0x52d07` is followed by two tables, which are inside the 3,316
bytes:

| Table | Retail | Entries | Used by |
| --- | --- | --- | --- |
| 1 | `0x52d08`–`0x52d1f` | six dwords: `0x52a31`, `0x52a4a`, `0x52a52`, `0x52a6b`, `0x52a90`, `0x52ab9` | `jmp [eax*4+0x52d08]` at `0x52a2a` (the first selector) |
| 2 | `0x52d20`–`0x52d33` | five dwords: `0x52b5b`, `0x52bae`, `0x52bc6`, `0x52bee`, `0x52c16` | `jmp [eax*4+0x52d20]` at `0x52b54` (the second selector) |

Each table is referenced only by the operand of its `jmp` (the dwords at
`0x52a2d` and `0x52b57`), and its entries are the only dwords equal to the
eleven targets, apart from three dwords in the XPP library section (`0x409584`,
`0x40d78b`, `0x40f8d1`) that equal `0x52bc6`: each is the bytes `c6 2b 05 00`
across the pair `mov eax,esi` / `sub eax,[0x55f000]` in library code, not a
pointer. Twelve `0xcc` bytes (`0x52d34`–`0x52d3f`) separate the range from the
next function.

## Callees without source

These ten routines are Direct3D library code with no source and no name in the
repository. [A depth-range pixel shader](unknown_03a5c0.md) describes most of
them; `0x3f7330` and `0x3fa3d0` are described here only.

| Retail | Convention | What it does here |
| --- | --- | --- |
| `0x3f53d0` | value on the stack (`ret 4`) | Sets the cull mode: 0 (`0x52067`), then `0x901` (`0x52c9a`). The game cache `g_4b8448` is stored before the call |
| `0x3f59b0` | value on the stack (`ret 4`) | Sets ZBIAS to 0 (`0x52144`); emits methods `0x40384`, `0x40388`, `0x40330`, `0x40334` and `0x40338` through `0x3f5cf0` |
| `0x3f5cf0` | method header in `ecx`, value in `edx`; plain `ret` | Appends one method and its value to the push buffer, through the pointer and limit at `0x404fe0` and `0x404fe4`. Called five times: `0x5207e`, `0x5209b`, `0x520b3`, `0x52682`, `0x5269f` |
| `0x3faae0` | `eax` = `[0x408648]`, one stack argument (`[0x408648]` >> 1; `ret 4`) | Makes room in the push buffer and returns the new write pointer in `eax`. Called at `0x520db` and `0x52129` |
| `0x3f6840` | none; plain `ret` | Only when the old ZENABLE copy was 2 (`0x5210e`): reads the device from `[0x407488]` itself and ORs `0x200` into `0x407490` at its end |
| `0x3f6f40` | `esi` = device; plain `ret` | Same condition (`0x52113`): reads device fields (`[esi+8]`, `[esi+0x794]`, `[esi+0x944]`–`[esi+0x960]`, `[esi+0xef8]`, `[esi+0xefc]`), writes methods `0x41e9c`, `0x41ea4` and `0x200b80` and calls `0x3f6b70` (inferred: it re-emits the viewport scale and offset). It skips the writes when bit 1 of the dword at `[[esi+0x794]+4]` is clear |
| `0x3f6390` | `edx` = write pointer, `esi` (the device) on the stack (`ret 4`) | Same condition (`0x52131`): writes method `0x40290` with a flag word and returns the pointer past it in `eax` |
| `0x3f6190` | `eax` = the pointer from `0x3f6390`, `esi` = device; plain `ret` | Same condition (`0x52136`): writes methods `0x100a20`, `0x100af0` (first branch) and `0x80394` and returns the new pointer in `eax`, which `0x5213b` stores to `[esi]` |
| `0x3f7330` | first register plus 96 in `ecx`, data in `edx`, dword count on the stack (`ret 4`) | Uploads vertex-shader constants: `ecx` = `0xb1` with 0x14 dwords (`0x5247a`), and `0xb6` with 0x18 (`0x524a4`). Matched `function_151c0` (`0x151c7`) and `function_0494b0` (`0x495ce`) call it the same way |
| `0x3fa3d0` | `eax` = the object {1, 0, `0x484f68`}; plain `ret` | The internal pixel-shader load (`0x52c86`); it reads only `eax` and reloads the device from `[0x407488]` itself (the call site happens to have `ecx` = device); it copies the program into the push buffer (about `0xfc` bytes of combiner methods). Matched `function_1ccf0` (`0x1cd16`) and `function_15180` (`0x151a6`) reach it with the same three stores |

## Existing declarations

| Retail | Declared as | Where | What retail shows |
| --- | --- | --- | --- |
| `0x52040` | `void __stdcall function_52040(void const *arg_1, void const *arg_2) { }` | `src/stubs/lane_p.cpp` (`// @stub 0x52040`) | `__stdcall`, two stack arguments, `ret 8`, no result: the same, and `const` is right (neither block is written) |
| `0x52040` | `void __stdcall function_52040(void const *arg_1, void const *arg_2);` | `src/unknown_13c050.cpp:37` and `src/unknown_13e635.cpp:110` (prototypes) | The same |
| `0x52040` | `void __stdcall function_52040(void const *request, void const *vertices);` | `src/unknown_236299.cpp:652` (prototype) | The same, with names |
| `0x52020` | `typedef void (__stdcall *t_4b220_fill)(void *, long, void *);` | `src/unknown_01cf50.cpp:2248` | Three stack words and `ret 0xc` agree; retail sets `al` to 1 where the typedef says `void`. No declaration, stub or source names `0x52020` itself |
| `0x12310` | `D3DTexture *function_12310(s_bitmap_view *bitmap, real priority)` | `src/unknown_01cf50.cpp:595` | The bitmap is in `eax` and the priority in `xmm2` |
| `0x142f0` | `void function_142f0(short mode)` | `src/unknown_0494b0.cpp:50` | The mode is in `ax` |
| `0x1c590` | `void function_1c590(s_shader_cache *state, long tag, long index)` | `src/unknown_01cf50.cpp:1269` | The state is in `esi`, the tag in `eax` and the index on the stack (`ret 4`) |
| `0x1c6b0` | `void function_1c6b0(void *memory)` | `src/unknown_01cf50.cpp:1251` | The memory is in `edx` |
| `0x1c710` | `void __stdcall function_1c710(void *memory)` | `src/unknown_01cf50.cpp:1201` | One stack argument (`ret 4`) |
| `0x1ccb0` | `void function_1ccb0(long format)` | `src/unknown_01cf50.cpp:2432` | The format is in `eax` |
| `0x1cf50` | `void function_1cf50()` | `src/unknown_01cf50.cpp:982` | No arguments |
| `0x222d0` | `void function_0222d0(D3DRENDERSTATETYPE state, dword value)` | `src/unknown_020560.cpp:663` | The state is in `esi` and the value in `edi` |
| `0x131e50` | `dword __cdecl pack_color4f(const color4f *color)` | `src/unknown_131e50.cpp:8` | `cdecl`, one pushed pointer; the caller pops it |
| `0x4b220` | `long function_4b220(long count, long mode, long primitive, long stride, t_4b220_fill fill, void *context)` | `src/unknown_01cf50.cpp:2251` | Five stack arguments (`ret 0x14`) and `ecx` (the declared `count`); no parameter is missing. The byte count is `ecx` × the third stack argument (`stride`). This call passes 0x14 in `ecx` and 4 as the third stack argument, so by the names `count` is 0x14 and `stride` is 4, the reverse of the vertex meaning (4 vertices of 20 bytes); the product is the same |

Notes on the declarations of `function_52040`:

- The request's first member is typed `dword` in `s_13c050` and
  `s_interface_draw_request`, but retail dereferences it as a pointer to a
  record of at least 0x1c bytes (`0x52711`–`0x5281d`).
- The selector words at `+0x90` and `+0x92` and the byte at `+0x80` lie inside
  opaque members (`unknown48[0x4c]` in `s_interface_draw_request`;
  `field_48[0x48]` and `field_90` in `s_13c050`). The mode at `+0x94` is the
  named member `mode` in `s_interface_draw_request` but lies inside `field_90`
  in `s_13c050`.
- The vertex argument is four `s_13c052` or `s_interface_draw_vertex` records
  of 0x14 bytes.
- The call sites are `src/unknown_13c050.cpp:83`, `src/unknown_13e635.cpp:156`
  and `src/unknown_236299.cpp:691`. No callee declaration above is missing a
  parameter; the register conventions are the compiler's whole-program choices.

A typing from the repository's own structures would be the following; it is
offered, not applied.

```
void __stdcall function_52040(s_interface_draw_request const *request,
    s_interface_draw_vertex const *vertices);
```

## Evidence

- The two entries were disassembled from the retail XBE with capstone 5.0.9,
  through `tools/xbe.py`. A control-flow graph with a stack-depth walk over
  every path put every stack operand at an offset from L, and the depth is
  consistent at every join. Registers, offsets, flags, jump-table words and
  constants were read from retail code and data, and the callees with source
  were read from it.
- A reader analysed both entries and a verifier checked each claim with its own
  disassembly, rescanning the image and reading the bodies of the callees: 34
  confirmed, 5 corrected and 1 that cannot be checked here (that the 4 stored
  at `0x526f7` is the alpha-kill enable, which depends on a Direct3D constant;
  it is marked inferred), with the corrections applied here. A third agent then
  checked the finished document against retail and the repository: 620 claims
  confirmed and 15 corrected or reworded, with the changes applied here.
- The callers come from an image-wide scan of every section for `call` and
  `jmp` (near, short and conditional) and for absolute dwords at any alignment.
  For `0x52040` it finds only the three calls above; for `0x52020` only the
  push at `0x52cd3`. The jump tables and their targets are referenced only as
  described under the tables.
- Constants are read from retail `.rdata`: `0x45dc0c` (2.0), `0x45df10` (-2.0),
  `0x45dbc0` (1.0), `0x45dbbc` (0.5), `0x45dc18` (8.0) and `0x45dc38` (255.0),
  and the default colour (three 1.0 reals) at `0x4409e4` behind the pointer at
  `0x468710`. The state values, masks and combiner words are immediates.
- No document covered this range before. Source line numbers are at `ad1dab2`.
- No emulator, runtime testing, SDK or outside dataset was used. Names are the
  repository's own, or describe behaviour.
