#include "unknown_11c920.h"
#include "unknown_1d5120.h"
#include "object_queries.h"
#include "unknown_11cc90.h"
#include <float.h>
#include <math.h>
// @flags /O2 /arch:SSE /Gr

extern hkWorld *g_51e9a4;
struct s_vehicle_ray;
bool __stdcall function_168f40(long arg_0, s_vehicle_ray const *arg_1, long arg_2, long arg_3);
class c_bounded_query_world
{
public:
	bool function_182800(s_vehicle_ray *arg_0, long arg_1);
};
void function_1d43d0(s_havok_component *arg_0, transform4x3f const *arg_1, transform4x3f *arg_2);
void function_11bed0(s_location *arg_0, point3f const *arg_1);

class c_contact_shape_view
{
public:
 virtual void slot0() = 0;
 virtual void slot1() = 0;
 virtual void slot2() = 0;
 virtual void slot3() = 0;
 virtual void slot4() = 0;
 virtual long type() = 0;
 long unknown04;
 long metadata;
};
struct s_1d48f0
{
 point3f field_0;
 vector3f field_c;
 real field_18;
 vector3f field_1c;
};
const dword g_455978[81] = {
 0x0, 0x0, 0x0, 0x3f800000, 0x0, 0x0, 0x0, 0x0, 0x3f800000,
 0x3f3504f3, 0x0, 0x3f3504f3, 0x3f13cd3a, 0x3f13cd3a, 0x3f13cd3a, 0x3f13cd3a, 0xbf13cd3a, 0x3f13cd3a,
 0x3f3504f3, 0x3f3504f3, 0x0, 0x3f3504f3, 0xbf3504f3, 0x0, 0x0, 0x3f3504f3, 0x3f3504f3,
 0x0, 0xbf3504f3, 0x3f3504f3, 0xbf800000, 0x0, 0x0, 0x0, 0x3f800000, 0x0,
 0x0, 0xbf800000, 0x0, 0xbf3504f3, 0xbf3504f3, 0x0, 0xbf3504f3, 0x3f3504f3, 0x0,
 0xbf3504f3, 0x0, 0x3f3504f3, 0xbf13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0x3f13cd3a,
 0x0, 0x0, 0xbf800000, 0x3f3504f3, 0x0, 0xbf3504f3, 0xbf3504f3, 0x0, 0xbf3504f3,
 0x0, 0x3f3504f3, 0xbf3504f3, 0x0, 0xbf3504f3, 0xbf3504f3, 0x3f13cd3a, 0x3f13cd3a, 0xbf13cd3a,
 0x3f13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0xbf13cd3a, 0x3f13cd3a, 0xbf13cd3a
};
// score 268 (was 298): r286 and r239 unchanged. Earlier rounds' entries follow in order, m3 lane 9 r142's last, then m3 lane 4 r230's.
// MiniMax G (11-20): no new form, no lower score (298): the first difference is still at +0x21, where 0x11bed0 is expanded here but called out of line in retail, so the optional-state homes stay four bytes off.
// MiniMax B (11-19): no new form, no lower score (298): the first difference is still at +0x21, where 0x11bed0 is
// expanded into this body but called out of line in retail; that expansion boundary belongs to src/unknown_11bed0.cpp,
// which this lane may not edit, so the optional-state homes stay four bytes off (ours [esp+0x2c]/[esp+0x3c] against
// retail's [esp+0x30]/[esp+0x40]) and the direction block's two divisions stay.
// MiniMax G (11-18): no new form, no lower score (298): the first difference is still at +0x21 and 0x11bed0 is
// still expanded here where retail calls it out of line; that expansion boundary is settled in
// src/unknown_11bed0.cpp, which this lane may not edit, so the optional-state homes (ours [esp+0x2c]/[esp+0x3c]
// against retail's [esp+0x30]/[esp+0x40]) and the colouring behind it stay as they are. Retail's direction block
// (0x1d4c70-0x1d4d78) keeps the table entry's three floats and divides once, ours keeps two and divides twice;
// the vector3f local for it measures 369, the magnitude2d restructures 312 and 331 and the operand swap 298 (same
// code), so none of them is the form. The permuter found no better form either: an annealed search over this body
// (600 s, 71 tries) lowered nothing, best 268 against a baseline of 298.
// src/unknown_11bed0.cpp, which this lane may not edit, so the optional-state homes and the colouring behind it
// stay as they are. The permuter found no better form: an annealed search over this body (300 s) lowered nothing,
// best 268 against a baseline of 268.
// m3 lane 9 r134 (fast unless noted; the score line below moved to the top of this one note):
// `inline` on 0x279740's own definition (src/unknown_279740.cpp) 418 plain and fast (2215 bytes: both releases then
// expand the range check and 0x11bed0 still expands, so the expansion is not a per-caller budget); the loop counter
// renamed to sort after the exclusion index (diagnostic for the name-ordered cells) 298 (same code); the upper bound
// set before the lower 315; the padded radius written at each use 508; the bounds loop through
// `m_quad.m128_f32[i]` in place of the `(real *)&m_quad` cast 298 (same code). Still BLOCKED-BY 0x11bed0's
// expansion here: with it out of line (earlier diagnostics) the entry and the optional-state homes are retail's.
// MiniMax H (11-14): no new form, no lower score (298): declaring the result flag before the two
// optional-state homes, and between them, both measure 400 against the body's 298, so the flag's
// declaration point is what sets the small homes' packing; the body is unchanged.
// MiniMax D (11-14): no new form, no lower score (298): the first difference is still at +0x21 with the frame 0x1d4
// in agreement, and the expansion that shapes everything after it (retail expands 0x279740 at the first release and
// calls 0x11bed0 out of line where this build does the opposite) is settled in src/unknown_11bed0.cpp, which this
// lane may not edit. BLOCKED-BY 0x11bed0's own file.
// MiniMax A (11-13): no new form, no lower score (298): the three-component planar copy (308), that copy with the same three length tests (369), the drop of the explicit epsilon test (321) and the component-wise copy of the two planar components (298) were all tried and restored, so the optional-state homes, the colouring and the 0x11bed0 and 0x279740 expansion stay as they are: BLOCKED-BY 0x11bed0's own file.
// MiniMax C (11-14): no new form, no lower score (298): the two optional-state homes still sit four bytes below retail's and 0x11bed0 is still expanded here where retail calls it; the callee's own file fixes that boundary, so the webs behind it stay.
// MiniMax H (11-12): no new form, no lower score (298): retail still expands 0x279740's range test at the
// first release and calls 0x11bed0 out of line where this build does the opposite, and the caller-side
// inline/noinline declarations the earlier rounds tried do not move it; the expansion boundary is set by
// 0x11bed0's own file (src/unknown_11bed0.cpp), which is not mine.
// MiniMax H (11-10): no new form, no lower score (298): the optional-state homes still sit four bytes
// low and our LTCG inlines 0x11bed0 where retail calls it, so the webs that follow cannot be recovered
// here: BLOCKED-BY 0x11bed0's own file (src/unknown_11cc90.cpp).
// Statement walk (b4L, 11-06) against retail 0x1d48f0-0x1d5110: every field, offset, condition direction, callee,
// argument order and add order agrees (bounds 0x1d4948-0x1d4a8c, local_1/local_2 0x1d4a9f-0x1d4b26, rigid body and
// shape 0x1d4b4b-0x1d4c09, release of the info objects 0x1d4c10-0x1d4c39, ray 0x1d4e61-0x1d4ee7 with `&& !local_0`
// at 0x1d4ee9, copies into the 1d43d0 input 0x1d4ef5-0x1d4fde, location/cluster test 0x1d5007-0x1d501c, release
// order 0x1d503e-0x1d50de); no behaviour difference found. The only differences in shape: (1) retail has ONE
// planar-length test (fst dword + fabs at 0x1d4cc1-0x1d4d01, `ja` straight to the loop end), ours has two: the
// one inside normalize2d_fast and the explicit `fabs(local_16) < 0.0001f` (the second reads the rounded float,
// retail's only test reads the unrounded x87 value; the table's planar lengths are 0 or >= 0.7, so no change in
// behaviour). The explicit test and the `1.0f / local_16` that scales the zero k term are the hand-written leftovers
// of the helper. Dropping the explicit test 321 (2076 B; loop head then equals retail's), and also with
// `radial_height = 0.0f` 339. (2) retail copies the table entry as three dwords (0x1d4c97-0x1d4cb9) and its k is
// `0 * (1 / length) * arg_6` (0x1d4d03, 0x1d4d1d): the radial is a vector3f with k cleared (planar normaliser of
// 0x29b2e0), not a vector2f. With the existing normalize_inline on that copy (`k = 0.0f; if (normalize_inline(&v)
// == 0.0f) continue;`, scaled by arg_6) 308 (2078 B), with 0x29b2e0's helper copied in 306; neither kept (higher than
// 298, no behaviour change). All other differences are register/stack homes and the 0x11bed0/0x279740 inlining.
// MiniMax R (11-10): no new form, no lower score (298); the two optional-state homes still sit four bytes below
// retail's, and the constant retail scales by is still read from a different address.
// MiniMax R (11-10): no new form, no lower score; the first difference is at +0x21, where retail reloads the bound's component into xmm3 and adds the offset to it and ours copies the offset into xmm3 and adds the component to it.
// MiniMax C (11-05): no new form, no lower score; ours is 2123 bytes against retail's 2083.
// MiniMax A (11-04): no new form, no lower score; the optional-state homes sit four bytes low (retail +0x30/+0x40, ours
// +0x2c/+0x3c) and retail hoists the bounds' zero component above the centre loads.
// MiniMax E (11-11): no new form, no lower score (298): the aligned frame agrees; the optional bounds/exclusion homes and the selective helper expansion remain, and the helper 0x11bed0 is defined in its own file: BLOCKED-BY 0x11bed0.
// MiniMax S (11-16): no new form, no lower score (298); retail still inlines 0x279740 at the first release and calls 0x11bed0 out of line, and both are set in files this lane may not edit.
// Earlier (298): aligned frame 0x1d4 agrees; optional bounds/exclusion homes and selective helper expansion remain.
// Lane 18 r37 (fast): result flag declared at function entry 400; sample count declared before the FPU
// setup 529; contact presence declared inside the sample loop 353 (2121 B); result flag declared after the
// presence 356; exclusion index declared before the bounds volume 298. Ours keeps body/shape in ESI/EDI
// (retail EDI/ESI) and the two optional-state slots 4 bytes lower than retail.
// Earlier: aligned frame 0x1d4 agrees; first +0x21 is bounds/exclusion homes.
// Round 87: the sample end as `point3f p = *arg_11;` plus in-place adds (retail's three dword copies and
// `movss x, [home]; addss x, [vector]`) gives retail's homes for the bounds/exclusion locals and the loop
// spills, 341 (2106 bytes; the entry esi/edi roles swap back while 0x11bed0 stays inlined). With that form
// and a noinline 0x11bed0 (diagnostic) the entry matches through +0x54 and aligned falls to 80/96 (from
// 129/130), but 0x14a280's bsp moves to the stack (0x14a280 14, 0x11bed0 19). Also tried: renamed
// bounds/exclusion locals 306 (names do not move them); upper bound set first 323; vector-first sums
// 341; existing vector3f_add on the copy 341; 0x11bed0 through 0x11be90 (still matched, still inlined).
// Earlier: the sample count matches (retail's
// setne/dec/and/sub is `27 - (flag ? 0 : 9)`, which keeps the flag on the stack). Structural cause of
// the register differences: our LTCG inlines 0x11bed0 here (retail calls it, esi = location), and the
// aligned frame 0x1d4 and incoming registers agree; small stack homes
// and selective location/range expansion remain. Count is `27 - (flag ? 0 : 9)`. Earlier graph:
// The register differences came from our LTCG inlining 0x11bed0 here (retail calls it, esi = location), and the
// inlined copy takes esi, which swaps the optional inputs (esi/edi) and evicts the table cursor from esi
// in the loop; retail also inlines 0x279740's range check at the first release only, ours calls it twice.
// A separate noinline declaration of 0x11bed0 in its own file keeps the call (entry registers then match
// retail through +0x54, 337; with the first release's range check written inline 291 at 2064 bytes), but
// it moves 0x14a280's bsp parameter from ecx to the stack and loses its 18 matched callers (0x21a100,
// which also inlines 0x11bed0 in our build, then matches). Round 83: description shape stored before its
// transform 317 kept; then (re-tested on this graph) length terms swapped 316 and translation as three
// statements 339; success flag declared first 407; upper bound set before lower 334; filter stored
// first 317.
// Earlier forms: optional-input registers, scalar homes and selective helper expansion: retail calls
// 0x11bed0 out of line and inlines the cache-range check of the first reference release (the form of
// 0x1d01c0), while ours does the opposite.
// Aligned frame 0x1d4 agrees; the sample count now matches (retail's
// setne/dec/and/sub is `27 - (flag ? 0 : 9)`, which keeps the flag on the stack); optional input
// registers (esi/edi swapped), scalar homes and selective helper expansion remain.
// Round 8 (lane 10): noinline redeclaration of 0x11bed0 here 321 (ignored); a separate noinline
// declaration in 0x11bed0's own file 337 (called as retail, but 0x11bed0 and its caller 0x168f40 stop
// matching); bounds coordinate as a named local 360; table direction pointer for both samples 321;
// excluded index declared first, its test braced 323.
// Earlier: Aligned 0x1d4 frame agrees; optional-input registers, scalar homes and selective helper expansion remain.
// Earlier 350 (was 350): Aligned 0x1d4 frame agrees; optional-input registers, scalar homes and selective helper expansion remain.
// Frame 0x1d4 agrees; optional input registers, scalar homes and selective location/release expansion remain.
// Earlier: Aligned frame 0x1d4 agrees; optional-input registers, scalar homes and helper expansion remain.
// frame 0x1d4 agrees; optional-input registers, scalar homes and inline boundaries differ.
// aligned frame 0x1d4 agrees; optional input/register roles (arg_9/arg_10 in
// esi/edi swapped), scalar homes and selective helper expansion remain: ours inlines 0x11bed0 (retail
// calls it at all its sites) and calls 0x279740 at both releases (retail inlines the first).
// aligned frame 0x1d4 agrees; optional-input registers (ESI/EDI swapped), scalar homes
// and selective helper expansion remain: retail calls 0x11bed0 out of line and inlines the cache-range
// check of the first reference release (the form of 0x1d01c0), while ours does the opposite.
// Native/byte success 427/382; path-local excluded index 449; complete volume arms 487;
// both path lifetimes 485; count just before scan 366; read-only body 370; native transform copies 370.
// the presence callback at its retail 12-byte size, initialised by its constructor.
// Frame 0x1d4 agrees; optional input/scalar homes and helper expansion differ. Tried early
// bounds filter 430; early transform filter 361; both early filters 423; reset at query 361; direct ray rejection
// 393; per query callback 376; callback and direct ray 376; early filters and reset 423.
// Native callback context: conditional count 389; earlier conditional 495; earlier arithmetic count 354 kept (plain).
// Lane 6 r85: ray endpoint and translation copy through `real_point3d_set` kept (317 -> 308: entry esi/edi
// roles and the movss translation copy now retail's); planar length through `magnitude2d` in place of the
// volatile casts kept (306). Translation alone 339; sample end through `real_point3d_set` 424, with the
// kept forms 308; direction locals renamed to flip their name order 308; `normalize3d` with k cleared 323
// (the short-vector exit is no longer threaded straight to the loop end); shape pointer declared before
// the body 306; radius scale and unit scale through `vector3f_scale` 306/308; scale factor first 306.
// With 0x11bed0 built /Ob1 (diagnostic) it is called as in retail but 0x14a280's bsp moves to the stack;
// with 0x279740 inlinable (diagnostic) 0x11bed0 is called here but both releases inline.
// Exact sample end 361; transform slot names 361; native ray endpoint 394; expanded radius name 361; unsigned sample ordinal 361.
// Candidate/endpoint/ray component loops 382/354/354; basis loop 524;
// translation loop 350 kept (plain); radial scaling/blend loops 354/354; combined producers 492.
// Direction/radial/both/endpoint/ray/all spatial declaration scopes 350 each;
// complete native endpoint value 403; paired bounds-axis producer 466. All restored.
// Complete candidate/blend/both product phases 350/350/350; both radial square terms 391;
// radial terms with candidate/all phases 391/391; six-use bounds origin 492, with candidate products 492. All restored.
// Complete lower/upper/both bound selections 413/466/484; complete horizontal square 350;
// bounds plus square 484; callback ending before cleanup 350; bounds/callback 484; square/callback 350. All restored.
// Round 187: bounds short/byte, sample long, translation long/short all 548 (2011 bytes);
// the four corresponding short-sample forms all 573 (1986 bytes). All eight ordinal-width forms restored.
// Sample-element reference 350; object-index field reference 357; both references 357;
// world snapshot alone/with sample 484/484; with object field/with both references 483/483;
// body/shape declaration homes at entry 350. All restored.
// Transform description after body/shape load/shape choice/pose copy 351/351/351/411;
// bounds description ends before add/exclusion 406/363; full shape selection 350; word success 415.
// Round 214: single native vector table, separate/reused radial sample and direct/shared ray difference
// 350/350/350/350; real-row table with the same producer grid 447/447/362/362. All restored.
// Noinline redeclaration of 0x11bed0 in this file 350 (ignored; still inlined); sample count as a
// 27/18 ternary 477 (retail's form, but the flag then goes in a register and caller 0xc5460 changes);
// ray endpoint adds as `x = x + i` 350; translation copy as three statements 354; upper bound as
// radius-plus-coordinate 350.
// Round 54: noinline declaration of 0x11bed0 447 (and 0x11bed0 itself 19 in --fast: CAVEAT on its
// callee 0x14a280); first release with the inline range check 442; both 401 (structural diff 36 vs 66);
// with an `arg_8 ? 27 : 18` count 427, count alone 477.
// Round 223: Both positive validation stages 350; result-terminated scan 395; optional failure
// first 351; both 395; zero rejection removed on these graphs 408/411/407/411. All restored.
// Round 148: native candidate value 486; native ray-offset value 467; both vector values 516;
// coordinated transform inline 350; inline/candidate 486; inline/offset 467; inline/both values
// 516; force-inline/both values 954. All executable forms restored.
// Lane 5 r77: `arg_8 ? 27 : 18` gives neg/sbb and makes the flag a register argument (477, caller
// 0xc5460 251 -> 255); `18`, then `27` if the flag 399; `27 - (arg_8 ? 0 : 9)` kept (350 -> 321).
// Then: volatile casts dropped from the radial length 388 (before the count change); length terms
// swapped 320; component index as a ternary 427; k cleared before the length, with or without its
// term 321/321; named lower/upper pointers in the bounds loop 321.
// Shared radial producers on the accepted count graph: in-place radius scale 321; complete blend setter 321.
// both shared producers 321; blend factor before normalization 331.
// factor with radius scale 331; factor with blend setter 331.
// all three producers 331; complete normalized-vector setter 321.
// All executable forms restored.
// Round 222: scan-lifetime transform description, original/late construction with set/native
// copies 320/320/330/330; description-owned pose on the same four graphs 382/382/391/391.
// Original description scope, standalone pose and copies restored; scalar homes and selective expansion remain.
// Current query/radial/all scalar identities 306; shared planar normalization and its
// three identity combinations 307; presence object identity 306. Coordinated range-helper
// inline 419, plus shared planar normalization 416; both lose matched controls and are
// restored. Original executable body and every external declaration retained.
// CRT input samples/ray origin/both 306/306/306; native body pose 374, with samples/origin/both
// 374/374/374; all inputs plus query pose 422. All restored.
// Shared setters for all three native basis rows, with early/late scale publication, entry/local
// input-output matrix declarations and native/library point publication: all eight 306. All restored.
// Round 161: Transform opaque/reference ownership, then optional opaque with
// derived/opaque/reference transform, then optional reference with derived/opaque/reference transform:
// 306/306/306/306/306/306/306/306. Original persistent pointer types and all casts restored.
// Round 206: Optional bounds/exclusion initialization after control-word setup [315, 315, 315];
// after SSE setup [315, 315, 315]; early presence construction alone/with late optional state [467, 458]. All restored.
// Round 228: optional-state/descriptor/receiver storage identity subsets 306 each;
// all three plus immutable body binding 306. First +0x21 optional-state homes remain; all restored.
// Round 232: shared affine candidate alone/radius/blend/both 306 each;
// with planar normalization on the same four graphs 307/307/307/304.
// Two-point gain rejected; original producers and all helper contracts restored.
// Round 214: Endpoint home alone/with sample/with radial/with both direction names 306 each;
// immutable named count alone/with endpoint/with all three/with full ray-query name 306 each.
// All restored; optional-state, sample/ray homes and selective helper expansion remain.
// Round 163: shared endpoint sum into point/native copy/float-set copy/vector
// 306/341/314/306; each with shared optional-ray difference 306/341/314/306.
// Native copies lengthen the body; optional-state homes and selective
// location/range expansion remain. All endpoint producers and helpers restored.
// Round 387: Optional-state homes and selective location/range expansion remain. Actual in-place
// transform with input/output native home, aggregate/CRT point publication and complete basis
// setter combinations: eight forms 352 each at 2070/2083 bytes. Original separate input/output
// matrices and every existing producer restored.
// Round 389: native missing-location result bool/byte/short/long 477/477/487/486;
// each with entry constant initialization 477/477/487/486. Original positive result and
// lifetime restored; aligned frame agrees, optional-state homes and helper expansion remain.
// Round 392: selected-cluster outcome in short at current/entry/control/SSE boundaries
// 314/474/472/472; long 314/477/475/475. Current lifetime reaches retail length,
// but homes and selective helper expansion remain. Original Boolean outcome restored.
// Round 257: inflated-bound scalar before descriptor construction 461, const 461;
// both with six-use center 496/496; early filter 459/459; center and filter 497/497.
// Original scalar lifetime, field observations and descriptor stores restored.
// Round 250: native lower/upper/translation vector producers, subsets 1..7
// 494/490/547/365/508/521/562; all with complete endpoint copy 601.
// All restored; optional-state homes and selective helper expansion remain.
// Round 181: complete table-sample/ray-origin/output coordinate-copy subsets 395/306/306/395/395/306/395; all with basis-row loops 430. Original native copies and body restored.
// Round 397: scan-lifetime bounds descriptor; entry constructor, native endpoint copy and shared table-sample masks 0..7
// 465/470/486/491/465/470/486/491; all restored.
// Round 185: Optional/required/both registration returns 307/312/313; integrated
// optional/both-return/required/both construction 307/313/330/331; early count 424. Required-result frame
// shrinks to 0x194; actual transform descriptor through scan/cleanup restores 0x1d4 at 326/327/320/321, with
// both complete native copies the same four scores. All original ownership, scopes, copies, query inputs and
// body restored; optional homes and selective helper expansion remain.
// Round 191: native planar radial scratch 319; with shared planar normalization 298 kept
// (plain whole-program, no match losses); ray-difference counterparts 319/298; in-place
// transform counterparts 351/351; complete candidate setter counterparts 323/323. Direct
// zero-on-failure admission 321; inherited later zero guard removed 334; both restored. Frame
// 0x1d4 and optional-state homes remain; original admission tests, zero products and cleanup preserved.
// Typed row-pointer planar read 298; the same radial row, two input components,
// vertical zero product, query inputs and original admission tests are preserved.
// Round 224: complete affine input aggregate with output-first, readonly input and
// native matrix identity subsets: eight forms 298 each. Original complete input
// producer, separate output matrix and every call restored.
// Round 234: Complete native radius-offset vector on planar scratch, producer/sample lifetime and
// component/shared setter 358/358/358/358; each with complete table contribution 357/357/347/347. All
// restored; optional homes and selective helper expansion remain.
// Round 205: Complete native full/planar/both sample inputs 373/298/373; ray-origin setter 317;
// full-origin/planar-origin/all inputs 370/317/370; all with shared ray difference 370.
// All restored; optional-state homes and selective location/range expansion remain.
// Round 253: Coherent unsigned bounds/sample/count domains in seven subsets, then immutable complete
// count; scores 298/298/298/298/298/298/298/298.
// Original executable source restored; no qualifying gain.
// Round 62 (m3 lane 16, fast): retail's planar block reads as `vector3f v = table[i]; v.k = 0.0f; if
// (normalize_inline(&v) == 0.0f) continue;` (a three-component copy, the fabs < 0.0001 skip, i, j and the zero k
// scaled by the inverse, then the `length == 0` skip), with v.i/j/k scaled by arg_6: 308 at 2078 bytes (LCS-aligned
// 205 -> 173; the planar block is then retail's code apart from its frame slots: ours puts the table vector at +0x20,
// the length at +0x2c and the copy at +0x30, retail the copy at +0x2c, the length at +0x44 and the table vector at
// +0x48); the length as a named local 308; the two vectors renamed offset/horizontal, sample_direction/radial,
// direction/planar 308 each (names do not move these slots).
// On that form: the copy declared before the table vector 308; the blend factor first and the copy in its own
// nested block 317 (2075 bytes).
// Round 473: coherent component identity after SSE setup, original/shared filter and per-sample/shared
// invariant blend: 488/476/512/524; entry identity on the same four graphs 493/521/494/502. Original body
// restored; optional-state/sample/ray homes and selective location/release expansion remain.
// Round 65 (lane 16, fast): the exclusion ray's difference through the existing vector3d_from_points3d 298
// (identical code, kept: it was that helper's body by hand); from arg_9 directly 340 (2112 bytes); round 62's
// planar form (normalize_inline on a k-zeroed copy) 308 again, with the helper 308; round 87's sample-end copy
// with in-place adds 407 (2149 bytes), both forms together 338 (2104 bytes). Retail calls 0x11bed0 (location in
// ESI, point in EDX) at every site and inlines 0x279740 only at the first release; ours inlines 0x11bed0 and
// calls 0x279740 twice. Earlier diagnostics tie both to 0x279740's /Ob1 file flags (a build-flags decision).
// Round 67 (lane 16, fast): upper bound as `local_4 + x` in all three rows 298 and the inflated radius as
// `0.001f + arg_7` 298 (both identical code); round 62's planar form with round 87's sample-end copy 338 again
// (2104 bytes): the frame homes are then retail's throughout (+0x2c copy, +0x44 length, +0x48 table vector,
// +0x1c sample end, +0x30/+0x40 optional state), and what is left is colouring: arg_9/arg_10 and the body/shape
// pair in ESI/EDI swapped against retail, the table cursor spilled from ECX instead of kept in ESI, arg_6/zero/
// inverse in XMM3/XMM7/XMM2 (retail XMM2/XMM5/XMM3), plus the 0x11bed0/0x279740 inline difference. On it the
// sample end as `x = x + i` rows 338 and as real_point3d_set on the copy 338 (identical code each).
// Round 30 (lane 2): retail's radial block is 0x29b2e0's inlined planar normaliser (function_29b32b in
// src/unknown_29b2e0.cpp): the whole table entry copied, k cleared, x87 sqrt of i*i + j*j, the epsilon test,
// i scaled by (1 / length) first, and `continue` when it returns 0. That helper moved unchanged into
// include/unknown_11cc90.h and used here (3-D copy, `if (function_29b32b(&radial) == 0.0f) continue;`, scaled
// by arg_6) 306 plain (2078 bytes against 2083, sequence-aligned 205 -> 172, 485 against 486 instructions;
// 0x29b990 kept its match): the positional count rises only because the sample/radial/length/count/optional
// homes differ (ours sample +0x20, radial +0x30, length +0x2c; retail endpoint +0x1c, radial +0x2c, length
// +0x44, sample +0x48). On that form: the endpoint copied from *arg_11 then offset in place 337 (2104 bytes);
// the sample renamed, the radial scale through vector3f_scale, the endpoint declared at the loop top 306 each
// (same code). Restored; a later round should keep this helper form once the homes can be moved.
// DL226: Bounded table cursors on the retained planar scratch, vector-row/dword strides,
// direct/cached range ends and relational/sentinel termination: eight forms 354. All restored;
// optional bounds/exclusion homes and selective location/release expansion remain.
// Round 70 (m3 lane 16, fast): the existing shared `normalize_inline` (include/unknown_0259d0.h) on a k-zeroed
// three-component copy gives the planar block without the private 0x29b2e0 helper; with round 87's sample-end copy
// and in-place adds 338 (2104 bytes, aligned 137); on that: `normalize3d` 339, radial scale through
// `vector3f_scale` 338, one table-entry pointer for both copies 338, shape declared before the body 338 (same code
// each). The first difference is then +0xe, arg_10/arg_9 and body/shape swapped in ESI/EDI and the cursor spilled,
// the webs that go back to retail's with 0x11bed0 out of line: BLOCKED-BY 0x11bed0's inlining here.
// Round 284: joint optional bounds/index/computed vertical-product identities, six lexical
// priorities 298 each; two complete vertical-scalar declaration lifetimes 298 each.
// Original body restored; optional scalar homes and selective helper expansion remain.
// Round 131 (m3 lane 8, fast with 0x11bed0/0x14a280): the query filter named once for both volumes 505 (2111 bytes);
// the sample counter declared before the presence object 298 (same code); the counter declared outside the for 298;
// the presence object declared with the transform 354; the count as `18 + (arg_8 ? 9 : 0)` 433. Same budget pattern as
// 0x116b00 (round 131 there): retail inlines 0x279740 (/Ob1 src/unknown_279740.cpp) at the first release and so calls
// 0x11bed0; ours can inline neither 0x279740 nor keep 0x11bed0 out (round 54's diagnostic: 0x279740 inlinable flips it).
// T62: shared planar normalizer zero-result/magnitude-result/success-first/zero-length
// returns 365/333/298/306; complete named squared parts give the same four scores.
// Plain two-component array scaling 298, with zero-result return 365. NOT kept:
// no target gain; all but the 306 form also raise other shared users. Header restored.
// M401: Complete lower/upper/translation output defaults, subsets 1/2/4/3/5/6/7,
// then complete CRT defaults: 508/508/298/298/508/508/298/620.
// NOT kept: ties or regressions; original setters, output lifetimes and whole body restored.
// Frame 0x1d4 agrees; optional-state homes and selective location/release expansion remain.
// N393: Excluded handle/radius/magnitude eight-byte storage, seven subsets
// 548/576/593/376/562/587/607; all with unsigned handle 607. NOT kept: unmatched,
// larger storage forms; full body restored. Optional homes and selective expansion remain.
// m3 lane 6 r181 (fast, control 298 = plain): the cluster result as `if (cluster != NONE) { found = true; break; }` 513
// (2114 B; caller 0xc5460 then 277: the flag moves to a register); the exclusion index as a ternary inside the bounds
// block 445 (2139 B). Same code as the control (VC canonicalises them): a per-file `inline` declaration of 0x279740,
// the two 1d43d0 matrices declared at the loop top, the blend sums written radial-first, the sample end through the
// existing vector3f_add_scaled, the zero-length test ahead of the inverse. Retail's first release is 0x279740 inlined
// (with its size getter) and its second a call; ours can inline neither from 0x279740's /Ob1 file, which leaves the
// budget that inlines 0x11bed0 here (build flags: the user's decision; `inline` on its definition is listed above).
// MiniMax D (11-17): no new form, no lower score (298): retail takes the first argument in a register
// (EAX into EBX) and so keeps a 0x1d4 frame with its locals four bytes higher than ours, which still takes
// that argument on the stack; that transport is decided by this function's callers in other files. The
// first difference also puts retail's zero register before the load of the first bound and our version
// after it, and our build still reads a different global and calls a different function at the first
// dispatch point.
// m3 lane 9 r136 (fast with 0x11bed0, 0x11be90, 0x14a280, 0x279740, 0xc5460, 0x21a100, 0x168f40, 0x1d43d0; control 298,
// aligned rows 205; body unchanged). Retail expands the range check at two sites in the whole image (here and 0x1d01c0;
// 59 functions call 0x279740, 14 of them twice or more with no expanded copy) and calls 0x11bed0 from all 41 of its
// callers; ours expands 0x11bed0 here and in 0x21a100 only. Diagnostics on 0x11bed0's own body (src/unknown_11bed0.cpp),
// none kept: it sits just under the link-time expansion limit, so one extra branch with a store, three arithmetic
// statements or one more call in it keep it out of line at both sites (this function then 278 at 2049 bytes, and 0x14a280
// keeps its match), while ten repeated stores, a one-line accessor for the collision tree and a guarded accessor do not.
// Spellings of 0x11bed0 that leave its bytes alone, all still expanded here (298): both arms through the existing
// function_11be90 (0x11bed0 still matches); the else arm through it with a file-local clearing helper; `inline` on
// function_11be90's definition with either; a pointer local for the leaf table. Spellings that change its bytes: the leaf
// as a ternary or preset to NONE then one function_11be90 call (0x11bed0 35/38, this 277/271), the level index read once
// into a short local (0x11bed0 5, this 297), a short cluster local (0x11bed0 1), a leaf element pointer (0x11bed0 25, this
// 295). One compiler-setting diagnostic: without the inline restriction on src/unknown_279740.cpp both releases expand the
// range check (418 at 2215 bytes, as `inline` on its definition did), so that file's flags are not the lever either.
// Still BLOCKED-BY 0x11bed0: the original has slightly more in it than ours when the expansion is decided.
// M3 lane 9 r142 (fast with 0x11bed0, 0x14a280 and all 42 functions that call 0x11bed0): the same 278 (2049 B) comes from
// the call site alone: an always-true `if (arg_11)` around the 0x11bed0 call emits no code but keeps 0x11bed0 out of line
// with retail's setup (location in ESI, point in EDX, then `mov edx, esi; cmp word [edx+4], -1`); 0x11bed0, 0x14a280 and
// every caller unchanged. A guard, so match-only and not kept. With 0x11bed0 called that way, `inline` on 0x279740's
// definition expands both releases 306 (2141 B), and so does 0x279740's file built without its inline restriction
// (diagnostic, restored) 306: the nested second release still expands. Guards that emit code: `if (arg_5)` 310 (2063 B,
// 0x11bed0 called), `if (!local_0)` 309 (2061 B). The point copy through real_point3d_set 320 (2133 B); 0x11bed0's else
// arm through the existing 0x11be90 298 (re-measured, 0x11bed0 still matches).
// SDK exception-mask setter 298, exception-state clearer 298, both 298; one named control word,
// read separately at setup and cleanup, 298. Axis loop seeding both bounds with explicit fourth
// components 509, with either or both exception macros 509. The two optional-state homes
// and the selective location and release expansions remain.
// m3 lane 9 r145 (fast with 0x11bed0, 0x14a280, 0x21a100 and 0x11be90; control 298): retail calls 0x11bed0 from all its callers, ours
// expands it here and in 0x21a100, so 0x11bed0 was respelled to look for a form with retail's bytes that the link-time inliner leaves
// out of line (that file is this machine's): the leaf branch as if/else with the stores in each arm 298 (0x11bed0 8), the bsp index
// read once into a short local 297 (0x11bed0 5), an early return for the no-bsp case 298 (0x11bed0 still matches, still expanded
// here), the arms swapped 311 (0x11bed0 90 bytes), the cluster preset to NONE and set under `if` 295 (0x11bed0 76 bytes). 0x11bed0
// restored. Still BLOCKED-BY 0x11bed0's expansion here and 0x279740's first-release expansion (its file's inline restriction).
// M3 lane 14 r122 (fast with 0x11bed0, 0x14a280 and 0x29b990, all MATCH; control 298): the found flag carried by the loop
// condition with no break keeps 0x11bed0 out of line here (no guard): `!local_0 && local_13 < local_6` 358 (2043 B),
// `local_13 < local_6 && !local_0` 311 (2047 B); with the k-cleared planar copy and the sample-end copy 370 (2022 B) and
// 310 (2026 B), the exit then tests after the increment where retail breaks before it. Diagnostic (an always-true guard,
// not keepable): planar, sample-end copy and `if (arg_11)` around the 0x11bed0 call 324 at 2030 B, first difference
// +0x54; what is left is mostly 0x279740's first-release expansion (53 bytes) and scratch-register rotation.
// M3 lane 1 r129 (fast with 0x11bed0, 0x11be90, 0x14a280, 0x1d43d0 and 0x279740, all matched throughout, and 0x21a100 93;
// control 298, aligned 144/136 = unmatched instructions ours/retail, masked 67/59; body unchanged). Lane 9 r142's
// always-true guard keeps 0x11bed0 out of line, so natural spellings that put its call one level deeper were tried:
// the rest of the loop body after the query under `if (!local_12.found) { ... }` in place of the `continue` 298, the
// same for the ray test 298, both 298 (same code each: the nesting leaves the flow graph as it was, so the link-time
// expansion does not move); the loop ended through its condition (`local_13 < local_6 && !local_0`, no break) 311
// (2047 B, first difference +0xd: the result lives in memory and is tested at the loop top); the tail as
// `if (cluster != NONE) { local_0 = true; break; }` 513 (2114 B, first difference +0x8). Still BLOCKED-BY 0x11bed0's
// expansion here and 0x279740's first-release expansion.
// M3 lane 13 r124 (fast with 0x21a100, 0x11bed0 and 0x14a280; 0x21a100 93 and the other two MATCH each time; control
// 298): `inline` on 0x11bed0's own definition 298 (still expanded here; 0x127400 then expands it too, 484 -> 563); the
// planar copy as the full table entry with normalize2d_fast on its first two members 369 (2132 B, a repeat of the
// listed vector3f local); round 87's ray start copied from `*arg_11` with `arg_4` added in place 407 (2149 B on
// today's body: the entry ESI/EDI swap and the homes stay off); the length test as `0.0001f > fabs(...)` 298 and the
// radial height as `local_17 * 0.0f` 298 (same code each). With registers masked, what is left is the expanded
// 0x11bed0, retail's expanded 0x279740 range check at the first release, the second length test and that ray copy.
// M3 lane 4 r230 (fast, faithful here; base 298, difflib-aligned 205, register/offset-masked 108). Kept: the table entry
// copied whole as a vector3f (retail copies three dwords at 0x1d4c97-0x1d4cb9) and normalised through normalize2d_fast
// on its planar part, without the explicit `fabs(local_16) < 0.0001f` test (normalize2d_fast already returns 0 below
// that length, and the `local_16 == 0.0f` test then continues): 268 (2085 B, retail's 486 instructions, masked 93; the
// loop head and the single reciprocal are retail's). Tried: the upper bounds written `local_4 + arg_3->x` 298 (same code);
// the copy with k cleared through normalize3d 323 (2094 B, masked 100), with vector3f_scale for the arg_6 scale 323,
// with the commuted upper bounds 323; on 268: the radial as `local_17 * 0.0f` 268 (same code); the ray start copied from
// `*arg_11` and moved by `+=` 350, the arg_6 test as `0.0f < arg_6` 268 (same code), through vector3f_add (either operand order) 350/350 (2111 B; the ray copy then sits at
// retail's +0x1c and the direction block's homes become retail's +0x2c/+0x44/+0x48, masked 92, but arg_9/arg_10 and the
// loop's table pointer swap ESI/EDI against retail). Left: retail's square order i then j in the planar length (ours j
// first), its zero register reused for the radial product (ours reads 0.0f from memory), the 0x11bed0 expansion here and
// retail's expanded 0x279740 range check at the first release.
// M3 lane 1 r136 (fast with 0x11bed0, 0x14a280 and 0xc5460, all MATCH; 0x21a100 93 throughout; control 268): the always-true
// `if (arg_11)` guard (diagnostic, 0x11bed0 then out of line) on today's body 373 (2011 B: the kept planar copy fits only the
// expanded form; arg_11 then takes EDI where retail has EBX); the two direction copies' names swapped (diagnostic for
// name-ordered homes) 268 (same code); a typed `directions` table pointer for both copies 268 and one `direction` entry pointer
// for both 268 (same code each); 0x11bed0 given `&local_22.position` in place of arg_11 273 (2090 B). Body unchanged.
// M3 lane 12 r159 (fast; control 268 at 2085 B, homes +0x24/+0x48 for retail's +0x30/+0x40): the direction count
// declared with the two optional-state locals at the top 481; the transform volume pointer declared ahead of the transform
// 268 (same code); the presence declared inside the loop body 311 (retail's 2083 B); the found test as `if (cluster !=
// NONE) { result = true; break; }` 485 (2076 B); a redundant `if (0 < count)` around the direction loop 373 (2011 B:
// 0x11bed0 is then called out of line as in retail, but the entry's EBX/EDI swap and the homes stay), which confirms the
// expansion follows the loop site's estimated frequency; no natural no-code test was found that lowers it.
// M3 lane 3 r286 (single-address fast, control 268): the found test as the loop condition (`!local_0 && local_13 <
// local_6`, the break dropped) 403 (2003 B), the same with the count test first 392 (2007 B); the planar ratio computed once
// ahead of the loop 336 (2099 B); the presence cleared ahead of the translation set 268 (same code); the two transforms
// declared at function scope 268 (same code). All restored.
// M3 lane 8 r206 (h2m fast with 0x11bed0 and 0x14a280, both MATCH unless noted; control 268): the loop's branch structure
// up to the 0x11bed0 call is retail's jump for jump, so the expansion (and, with it, 0x279740's range check staying out of
// line at the first release) follows 0x11bed0's own size. Diagnostic, reverted: 0x11bed0's else arm through the existing
// structure_leaf_cluster_get (0x11bff0, whose file restricts inlining) leaves a real call: 0x11bed0 52 (58 B), 0xbf510 88,
// 0x21a100 86, this 335. Tried here: the direction blend through vector3f_scale and vector3f_add with the radial term in
// the planar copy's k 282; the sample point through vector3f_add_scaled 268 (same code); `bool local_0 = false;` declared
// first 370 (its byte moves to +0x1f); the two optional-state locals renamed (bounds_volume, bounds_body_index) 268 (same
// homes +0x24/+0x48). The planar length's squares come j first from the shared magnitude2d (retail i first); that header
// serves about twenty files, so it was not changed. Still BLOCKED-BY 0x11bed0's inlinability at this site.
// Bounds through the existing component reader 268, with one cached endpoint coordinate 341.
// Clipped bounds before stores 480, each clipped edge per axis 491, and separate lower and upper passes 491.
// Conditional edge values 481, clipped point pair into setters 478, and setters before conditional stores 486.
// The optional-state homes and the selective location and release expansions still differ.
// M3 lane 1 r144 (h2m fast, control 268): the hkTransform-to-matrix copy written through a file-local inline
// vector3d-from-havok helper (the form 0x1d08e0 uses) 268; the planar copy taken from the direction copy instead of the
// table 268; the zero-length test ahead of the reciprocal 268; the transform copied by aggregate assignment instead of
// set() 268 (all the same code); 0x11bed0's else arm as a call of 0x11be90 268 (a repeat: 0x11bed0 stays matched and
// still expands here). `inline` on 0x279740's definition 427 (2177 B: both releases expand, 0x1d01c0 207). Diagnostics,
// reverted: eight dead real_point3d_set expansions just before the 0x11bed0 call 268 (no caller-wide inline budget
// moves the expansion); 0x279740's file without /Ob1 427 (0x1d01c0 207), the same as `inline`, so retail's single
// expanded range check is the helper's code (round 212's reading), not an auto-inlined 0x279740.
// M3 lane 8 r212 (h2m fast with 0x279740, 0x11bed0 and 0x14a280, all MATCH; control 268, aligned 206, register-masked 130).
// Retail expands 0x279740's range check in only two game functions: this one's first release and matched 0x1d01c0, whose
// file writes it through a local rigid-body release helper (component_body_release, the check written out). Diagnostic:
// the first release through a file-local copy of that helper gives retail's release block instruction for instruction
// (`dec; mov ax; mov ebx, 1; jne; cmp edi, 0x80061000 ...`) but 367 positional (2131 B: 0x11bed0 is still expanded here),
// aligned 187, masked 111; with the always-true `if (arg_11)` guard as well 331 (2057 B), aligned 171, masked 70 (lead
// diff kept outside the tree). Tried: the first release under `do { } while (0)` 268 (same code, still a call); on the
// helper form, the code after the query nested under `if (!found)` 367 (same code, 0x11bed0 still expanded), the direction
// count as `arg_8 ? 27 : 18` 448 (2119 B), the helper forceinline 367 (same code). Diagnostics: the 0x11bed0 call removed
// 382 (0x279740 still a call at both releases), the second release removed 534. So the first release's expansion is that
// helper's code, not an inlining budget shared with 0x11bed0; what keeps 0x11bed0 out of line here is still open.
// M3 lane 4 r242 (H2M_FAST with 0x11bed0 and 0x14a280, both MATCH each run; control 268): the planar length through the
// existing planar_normalize of include/vector2f_normalize.h 268, with the radial as `local_17 * 0.0f` 268 (the same code
// as normalize2d_fast, address-normalised listing identical); the transform's position copied from `*arg_11` (the values
// the translation was set from) 305 (2072 B); the ray's difference taken from `arg_9` instead of its copy 315 (2074 B);
// the ray end as a brace initialiser 268 (same code). Diagnostic, restored: magnitude2d's sum written j first 268.
// M3 lane 15 r126 (h2m fast with 0x11bed0 and 0x14a280, both MATCH; control 268): the code after the query nested under
// `if (!found)` instead of its `continue` 268, and also under `if (!ray_blocked)` 268 (two written levels, the same code:
// 0x11bed0 stays expanded, so written nesting alone does not move this call); the call and the found assignment under
// `if (!local_0)` (always true there, as retail's `&& !local_0` in the bounded test is) 360 (2023 B: 0x11bed0 out of
// line, entry roles still EBX/EDI swapped and the two optional-state homes at +0x24/+0x48 for retail's +0x30/+0x40); the
// call alone under it 367 (2028 B); with the break inside it 375 (2019 B).
// M3 lane 8 r218 (H2M_FAST with 0x11bed0, 0x14a280 and 0x279740, all MATCH unless noted; control 268). Reading: retail
// calls 0x11bed0 out of line at all 46 of its call sites (a raw call scan of retail matches the 46 calls in src); this build
// expands it at the two loop sites only, here and 0x21a100 (whose `if (audible == 1)` loop call is a call in retail), while
// matched 0x125920 keeps the same loop shape's call out of line. Tried: the exclusion index as a ternary with NONE 405
// (2101 B); the sample rows written point first 268 (same code); a local location filled by 0x11bed0, copied to arg_5 and
// tested (0x125920's shape) 311 (0x11bed0 still expanded); re-measured: the rest under `if (!local_12.found)` 268 (same
// code), the call under the always-true `if (!local_0)` 360. Diagnostic, reverted: `/Ob1` on 0x11bed0's file 373 (a call
// here, 0x21a100 156), but matched 0x11bed0 19 and 0x14a280 14. Still BLOCKED-BY 0x11bed0's expansion here.
// M3 lane 7 r211 (h2m fast with 0x11bed0, 0x14a280, 0x279740, all MATCH; control 268): the direction loop ending through
// its condition (`local_13 < local_6 && !local_0`, no break) 392 (2007 B), with `!local_0` first 403 (2003 B); the bounds'
// upper corner as `local_4 + arg_3->x` (all three) 268 (same code); the found flag declared at the top with the two
// optional-state locals 370; the direction count as `arg_8 ? 27 : 18` 412 (2073 B). Still BLOCKED-BY 0x11bed0's expansion.
// M3 lane 16 r173 (H2M_FAST with 0x11bed0, MATCH each run; control 268, aligned 206, register-masked 130): the ray end
// as retail's copy of `*arg_11` then `local_19.x += arg_4->i;` per row (round 87's form) 350 (2111 B); with the direction
// loop as `long local_13 = 0; if (0 < local_6) { for (; ...) }` (0x11bed0 then called out of line, as with the always-
// true guard) 323 (2037 B, first difference +0x54: the entry registers and both optional-state homes are retail's); with
// the first release through a file-local copy of 0x1d01c0's release helper (diagnostic) 277 at retail's 2083 B, aligned
// 121, register-masked 42 (lead saved as build/r173/lead-1d48f0-277.patch in m3-lane16). Left on it: the bounds block's
// upper x (retail loads x again), one SIB operand order, the planar block's x87 square order (y first here) and its
// products' XMM numbers (retail multiplies the reciprocal by the zero register, this build by a 0.0 constant), the ray
// end's operand orders. On it: the upper bounds written `local_4 + arg_3->x` (all three) 277; the reciprocal and the
// radial term after the zero test, written `local_17 * 0.0f`, 277 (same code each). The keepable route for the helper
// is moving 0x1d01c0's helper into a header both files include. Body unchanged.
// M3 lane 4 r253 (h2m fast with 0x11bed0, 0x14a280 and 0x279740, all MATCH; control 268): the cluster result and the break
// as `if (arg_5->cluster_index != NONE) { local_0 = true; break; }` 485 (2076 B); the planar test as `0.0f < arg_6` 268; the
// transform's three rotation rows through the existing vector3f_assign 268; the reciprocal and radial term after the zero
// test 268; the direction counter declared ahead of its loop 268 (all the same code). Reading: retail's jcc/call skeleton
// from the loop head to the 0x11bed0 call is ours test for test, so the expansion is not the site's written tests; the
// lane 15 r126 diagnostic (0x279740 inlinable makes 0x11bed0 a call while both releases expand) points at a per-function
// link-time inline budget that retail's first-release expansion used up. Body unchanged; still BLOCKED-BY 0x11bed0's
// expansion here.
// M3 lane 5 r239 (H2M_FAST with 0x11bed0, 0x14a280 and 0x279740, all MATCH; control 268): the direction loop as
// `long local_13 = 0; do { ... } while (++local_13 < local_6);` 328 (2071 B: the first form without a redundant guard
// that leaves 0x11bed0 a real call here, as retail has it; the layout is not retail's); the loop test as `local_13 !=
// local_6` 268 (same code); the do-while with the first release through a file-local copy of 0x1d01c0's release helper
// (diagnostic) 360 (2117 B); that copy alone 367 (2131 B); the count written into the loop condition, no local, 373
// (2085 B). Body unchanged; still BLOCKED-BY 0x11bed0's expansion here.
// M3 lane 12 r184 (h2m fast with 0x11bed0, 0x14a280 and 0x279740, all MATCH; control 268). Retail: 0x11bed0 called in the
// loop, the first release with 0x279740's pinned-range test expanded, the second release calling 0x279740 (17 calls; ours
// 18). Diagnostics of the held route (`inline` on 0x279740's definition, restored at once): alone 427 (2177 B, both
// releases expanded, 0x11bed0 still expanded); with the loop as a do-while on `++local_13 < local_6` 328 (2071 B, neither
// release expanded; the do-while alone 328, same code); with the loop under `if (0 < local_6)` and its counter zeroed
// ahead of it 331 (2057 B: retail's call skeleton, 0x11bed0 called, first release expanded, second called; entry
// registers EBX/EDI swapped); that guard alone 373 (2011 B). BLOCKED-BY 0x279740 (/Ob1 file; needs the held `inline`).
// Use world registration results for bounds, transform or both 269/274/275. Bounds ray, rigid body pose,
// query dispatch, paired ray and pose, or two reference-counted receivers 268 each. NOT kept: registration
// results alter live registers without restoring the optional homes, and all typed views tie.
// Exclusion defined on the bounds path by branches or a conditional 430/464,
// with absent-path defaults 494/494; null volume on the absent path 430,
// with both defaults in their arms 485 or conditional exclusion 460;
// publish the volume after selecting the exclusion 465.
// NOT kept: the retail homes and selective call expansion remain.
// M3 lane 16 r186 (H2M_FAST with 0x11bed0, 0x21a100, 0x1d43d0, 0x279740 and 0x1d01c0; control 268, aligned 206; body
// unchanged). Diagnostics on the two callee files' flags, restored at once: `/Ob1` on 0x11bed0's file 373 (2011 B, 0x11bed0
// called here, but 0x11bed0 itself then 19: its 0x14a280 call pushes the collision bsp where retail passes it in ECX, and
// 0x21a100 80 -> 156); 0x279740's file without its /Ob1 427 (2177 B: both release tests and 0x11bed0 expanded; 0x1d01c0
// 682 B against 572); both together 331 (2103 B, aligned 212: 0x11bed0 called, both release tests expanded where retail
// expands only the first). 0x11bed0's else arm written through the existing 0x11be90 (retail's 0x11bed0 is that call
// expanded) is still a byte match of 0x11bed0 and changes nothing here (268; 0x21a100 80 -> 81, so not kept). The frame
// here is packed by reference density (references over size, densest lowest), so the optional volume's home sits below
// the 12-byte direction where retail has it above: the counts follow from which callee is expanded, not from a
// declaration. Still BLOCKED-BY 0x11bed0's expansion here and 0x279740's /Ob1 file (lane 12 r184's held route).
// GA3 (fast with 0x11bed0, 0x279740, 0x1d43d0, 0x1d01c0, all MATCH each run; control 268): unrolling the 3-axis bounds
// adjustment loop into six plain per-axis `if` statements keeps 0x11bed0 as a real out-of-line call WITHOUT any
// redundant guard or callee-file change (373, 2011 B); combined with `point3f local_19 = *arg_11; local_19.x += ...;`
// 323 (2037 B, first diff +0x54: entry registers EBX/ESI/EDI and both optional-state homes [esp+0x30]/[esp+0x40] match
// retail); + `local_15.k = 0.0f; normalize3d(&local_15)` 330 (2030 B; single divss and `mulss reg, zero_reg` match
// retail); + `sqrt(local_15.j * local_15.j + local_15.i * local_15.i)` 324 (2030 B: all 20 x87 length instructions
// match retail byte-for-byte). Diagnostics adding 0x1d01c0's release check at the first release: on unrolled+ray_copy
// 277 (exact 2083 B, aligned 165), + normalize3d 284 (2076 B), + j*j+i*i radial 278 (2076 B, aligned 140; all 20 x87
// instructions match, 2-instruction radial scale shift). Other clean forms on baseline 268: normalize3d / normalize_inline
// 308/308 (2078 B), + ray_copy 339 (+ do-while 343), + vector3f_add 311; bounds center locals 359; directions table
// pointer / vector3f_add_scaled for arg_11 / transforms before loop / vector3f_add for ray 268 each (same code);
// vector3f_add_scaled for blend 285; j*j+i*i radial alone 308. Body unchanged (still needs shared header for 0x1d01c0's
// release helper).
// M3 lane 15 r146 (H2M_FAST with 0x11bed0, 0x14a280 and 0x279740, all MATCH each run; control 268, aligned 206, masked
// 130; "aligned" = difflib-aligned rows with addresses masked, then registers masked too). Re-measured: the bounds test
// written per axis through the vector's element accessor 373 (2011 B), with the ray end as a copy of `*arg_11` plus the
// offset 323 (2037 B), with the first release through a file-local copy of 0x1d01c0's release helper (diagnostic) 277
// (2083 B, aligned 121 / 42). New on that 277 form: the direction's planar part as the table entry with its height set
// to 0 and normalized by the shared normalize_inline (the zero height then multiplies the reciprocal in the zero
// register, the x87 length is retail's i-first order, and the too-short exit threads to the loop's continue as retail),
// scaled by arg_6 through vector3f_scale: 278 (2076 B) but aligned 80 / 30, the planar block then retail's but for
// registers; the same with the blend through vector3f_add_scaled 276 (aligned 85 / 30), the blend factor computed
// ahead of the scale 280, the scale written per component 278, the blend written k, j, i 278, the position through
// vector3f_add_scaled 278 or real_point3d_set 280. Without the per-axis bounds and the release copy the normalize_inline
// form is 324 (2030 B) or 308 (2078 B, listed). normalize3d there 298 (2092 B: its short exit is not threaded); the
// radial term as `0.0f` then `*= local_17` 268 (same code). Allocator (diagnostic): on the 278 form the three blended
// direction components tie at priority 312 and colour k, j, i (ties follow creation order); forcing k one below the tie
// gives 133 (not source: the trio then colours j, i, k, so retail's i, j, k needs i above j as well). The 278 form is held as a patch by the coordinator. Still
// needs 0x1d01c0's release helper in a shared header for the first release and a non-steering way to keep 0x11bed0 out
// of line (BLOCKED-BY 0x11bed0's expansion here).
// M3 lane 5 r252 (H2M_FAST with 0x11bed0, 0x14a280 and 0x279740, all MATCH; control 268): 0x11bed0 given the
// transform's position ahead of the copy to arg_11 (`function_11bed0(arg_5, &local_22.position); *arg_11 = ...`) 273
// (2090 B); a short direction counter 330 (2072 B); the sample point built in a point3f local and copied to arg_11 465
// (2107 B); the direction table typed `vector3f const [27]` with its float values (no casts at the two reads) 268 (same
// code); the transform's unit scale stored after the rotation rows 268 (same code). Body unchanged; the keepable
// route for the first release (0x1d01c0's release helper moved into a shared header) needs an edit in 0x1d01c0's file.
// M3 lane 14 r163 (H2M_FAST with 0x11bed0, 0x14a280 and 0x279740, all MATCH; control 268): the per-axis bound read
// once into a named two-use local 341; the result flag declared at the top beside the two query locals 370; the
// direction count as `arg_8 ? 27 : 18` 412 (2073 B); the query flag cleared ahead of the translation set 268 (same
// code); the bounds offset computed ahead of the bounds info 482 (2101 B). Body unchanged.
// @retail 0x1d48f0
bool function_1d48f0(point3f *arg_11, s_havok_component *arg_0, long arg_1, long arg_2, point3f const *arg_3,
 vector3f const *arg_4, s_location *arg_5, real arg_6, real arg_7, bool arg_8,
 point3f const *arg_9, long arg_10)
{
 c_contact_query_bounds_volume *local_1 = NULL;
 long local_2 = NONE;
 _control87(0x9001f, 0x8001f);
 _mm_setcsr(_mm_getcsr() | 0x1f80);
 if (arg_9)
 {
  c_contact_query_bounds_info local_3;
  real local_4 = arg_7 + 0.001f;
  local_3.lower.set(arg_3->x - local_4, arg_3->y - local_4, arg_3->z - local_4);
  local_3.upper.set(arg_3->x + local_4, arg_3->y + local_4, arg_3->z + local_4);
  for (long local_5 = 0; local_5 < 3; ++local_5)
  {
   if (((real *)&local_3.lower.m_quad)[local_5] + 0.001f > arg_9->n[local_5])
    ((real *)&local_3.lower.m_quad)[local_5] = arg_9->n[local_5] - 0.001f;
   if (arg_9->n[local_5] > ((real *)&local_3.upper.m_quad)[local_5] - 0.001f)
    ((real *)&local_3.upper.m_quad)[local_5] = arg_9->n[local_5] + 0.001f;
  }
  local_3.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_1 = new c_contact_query_bounds_volume(&local_3);
  ((c_contact_query_world *)g_51e9a4)->add(local_1);
  if (arg_10 != NONE) local_2 = havok_object_get(arg_10)->havok_component_index;
 }
 bool local_0 = false;
 long local_6 = 27 - (arg_8 ? 0 : 9); /* the table's 27 directions, or the first 18 */
 hkTransform local_7;
 c_contact_query_transform_volume *local_8;
 {
  c_contact_query_transform_info local_9;
  hkRigidBody *local_10 = havok_component_rigid_body_get(arg_1, arg_0);
  c_contact_shape_view *local_11 = *(c_contact_shape_view **)((byte *)local_10 + 0xc);
  if (local_11->type() == 0x17) local_11 = *(c_contact_shape_view **)((byte *)local_11 + 0x30);
  local_7.set(local_10->m_motion->m_transform);
  local_9.shape = local_11;
  local_9.transform.set(local_7);
  local_9.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_8 = new c_contact_query_transform_volume(&local_9);
 }
 ((c_contact_query_world *)g_51e9a4)->add(local_8);
 c_contact_presence local_12;
 for (long local_13 = 0; local_13 < local_6; ++local_13)
 {
  vector3f local_14 = ((vector3f const *)g_455978)[local_13];
  if (arg_6 > 0.0f)
  {
   vector3f local_15 = ((vector3f const *)g_455978)[local_13];
   real local_16 = normalize2d_fast((vector2f *)&local_15);
   real local_17 = 1.0f / local_16;
   real radial_height = 0.0f * local_17;
   if (local_16 == 0.0f) continue;
   local_15.i *= arg_6;
   local_15.j *= arg_6;
   radial_height *= arg_6;
   real local_18 = (arg_7 - arg_6) / arg_7;
   local_14.i = local_14.i * local_18 + local_15.i;
   local_14.j = local_14.j * local_18 + local_15.j;
   local_14.k = local_14.k * local_18 + radial_height;
  }
  arg_11->x = local_14.i * arg_7 + arg_3->x;
  arg_11->y = local_14.j * arg_7 + arg_3->y;
  arg_11->z = local_14.k * arg_7 + arg_3->z;
  local_7.m_translation.set(arg_11->x, arg_11->y, arg_11->z);
  local_12.found = false;
  ((hkRigidBody *)local_8)->pose_write(local_7);
  ((c_contact_query_dispatch *)local_8)->query(&local_12);
  if (local_12.found) continue;
  point3f local_19;
  real_point3d_set(&local_19, arg_11->x + arg_4->i, arg_11->y + arg_4->j, arg_11->z + arg_4->k);
  if (function_168f40(0x1480040d, (s_vehicle_ray const *)&local_19, arg_0->object_index, NONE)) continue;
  if (local_1)
  {
   s_1d48f0 local_20;
   local_20.field_0 = *arg_9;
   vector3d_from_points3d(&local_20.field_0, &local_19, &local_20.field_c);
   if (((c_bounded_query_world *)local_1)->function_182800((s_vehicle_ray *)&local_20, local_2) && !local_0) continue;
  }
  transform4x3f local_21;
  transform4x3f local_22;
  local_21.scale = 1.0f;
  local_21.forward.i = local_7.m_rotation.m_col0(0);
  local_21.forward.j = local_7.m_rotation.m_col0(1);
  local_21.forward.k = local_7.m_rotation.m_col0(2);
  local_21.left.i = local_7.m_rotation.m_col1(0);
  local_21.left.j = local_7.m_rotation.m_col1(1);
  local_21.left.k = local_7.m_rotation.m_col1(2);
  local_21.up.i = local_7.m_rotation.m_col2(0);
  local_21.up.j = local_7.m_rotation.m_col2(1);
  local_21.up.k = local_7.m_rotation.m_col2(2);
  real_point3d_set(&local_21.position, local_7.m_translation(0), local_7.m_translation(1), local_7.m_translation(2));
  function_1d43d0(arg_0, &local_21, &local_22);
  *arg_11 = local_22.position;
  function_11bed0(arg_5, arg_11);
  local_0 = arg_5->cluster_index != NONE;
  if (local_0) break;
 }
 ((c_contact_query_world *)g_51e9a4)->remove(local_8);
 havok_reference_remove((c_havok_reference_counted *)local_8);
 if (local_1)
 {
  ((c_contact_query_world *)g_51e9a4)->remove(local_1);
  havok_reference_remove((c_havok_reference_counted *)local_1);
 }
 _mm_setcsr(_mm_getcsr() & 0xffffffc0);
 _clearfp();
 _control87(0x9001f, 0xfffff);
 return local_0;
}
