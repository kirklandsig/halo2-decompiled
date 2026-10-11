# Shader setup at `0x52d40`

This 1,136-byte retail routine prepares texture/render state and a pixel
shader. It does not itself submit a draw. Source replaces the empty stub in
`src/stubs/lane_w.cpp` and lives in `src/unknown_01cf50.cpp`, alongside related
graphics functions. The compiled body remains unmatched (1,112 bytes).
Replacing the stub makes its existing caller `0x43720` match exactly without
changing that caller's body.

## Inputs and control flow

Retail takes the bitmap index in ECX and three stack arguments: a shader
record, a tag, and flags. It returns with `ret 12`. The declaration's former
boolean final argument was incorrect: retail tests bits `0x20` and `4`.
The declaration in `src/unknown_0497a0.cpp` now uses `dword flags`; the caller
continues to pass zero.

The function returns immediately unless the signed short at shader `+0x24`
is 1. Otherwise it selects a bitmap through `0x143c0` and configures stage 0:

- Shader byte `+0x2e` bits 1 and 2 select U and V address modes; bit 0 selects
  filter value 1 instead of 2 for magnification, minification and mip filtering.
- Anisotropy, maximum mip level, LOD bias, color sign and alpha kill are zero.
- Culling, alpha testing, stencil, depth writing and depth bias are disabled;
  color writes are `0x10101`, alpha blending is enabled, depth mode is 2 and
  depth comparison is `0x203`.
- The short at `+0x2a` is passed to `0x142f0`. Stipple is reset to 1 when its
  cached value is not within the retail tolerance, using `0x1c290` and the
  public Direct3D API.

Shader-cache selection uses the value at `g_485a80->field_5c + 0x24` and
`0x1c590`. The routine uploads three four-float constants starting at public
vertex constant register **78**. The default is a 3-by-4 identity matrix.
Flag `0x20` instead uploads the transpose of the nine floats at `g_4856c4`,
with the position at `g_485618` in the fourth column. Retail's upload helper
receives index `0xae` and 12 float words; the public API applies a 96-register
bias. The SDK's constant-cache suppression bit is preserved by using its API.

The pixel-shader structure is cleared. Shader byte `+0x28` bit 1 selects
three initial RGB combiners instead of one. Unless flag 4 is set, the signed
blend-mode short at `+0x2a` selects an additional alpha and/or RGB stage.
Modes 1 and 5 share a branch, as do 3, 4 and 6. Mode 7 sets both alpha and
RGB outputs. The combiner count advances even for an out-of-range mode.
The resulting program is passed to `0x15180`.

## Validation and limits

The full original-compiler check preserves the previously verified 7,502
matches and gains only `0x43720`, for 7,503 matches. This is not a claim that
`0x52d40` matches.

Independent actual-instruction comparisons against retail pass:

- 3,072 transform/shader cases: all low-six-bit flag combinations, signed
  blend modes including invalid values, both initial-combiner variants and
  constant-cache suppression states, with randomized float bit patterns.
- 2,048 entry/setup cases, including 512 early returns.
- 2,048 complete-body cases comparing state/cache writes, call order and
  arguments, upload contents, shader bytes, nonvolatile registers and stack
  cleanup. Called helpers and GPU boundaries are explicit stand-ins.

The focused comparison caught and corrected an initial upload-register
mistake (46 instead of 78). These tests do not establish helper internals,
actual GPU execution, rendered output or complete gameplay.

Sources: independent disassembly of the repository's SHA-256-pinned retail
executable and existing CC0 graphics code, with public SDK API calls. No SDK
implementation or files are copied. The independent behavioral harness is
in BirchWoodGod/halo2-decomp; its symbol names were not used. AI assistance:
Codex. This analysis and source contribution are released under CC0 1.0.
