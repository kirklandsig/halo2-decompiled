// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "view_context_storage.h"
#include "bitmap_sequences.h"

struct s_float_rect;
struct s_widget_quad_2b11;
void function_22a664(s_widget_quad_2b11 const *arg_0, s_float_rect const *arg_1,
	long arg_2, long arg_3, long arg_4);
extern real g_4e69c0[4];


struct s_4e6950
{
	color3f field_0;
	point3f field_c;
	real value18;
	real value1c;
	real value20;
	real value24;
	byte unknown28[0x70 - 0x28];
	real user_values[4];
};
extern s_4e6950 g_4e6950;

/* the hud globals' marker kinds (+0x408) */
struct s_24e9c7_kind
{
	long unknown00;
	long bitmap_index;
	long unknown08;
	long shader_index;
	short sequence_index;
	byte unknown12[0x24 - 0x12];
};

struct s_24e9c7_kinds_view
{
	byte unknown00[0x408];
	long count;
	s_24e9c7_kind *entries;
};

struct s_24e9c7_marker
{
	bool use_full_alpha;
	byte unknown01[3];
	long kind;
	byte unknown08[0x24 - 8];
	color3f color;
	point3f point;
	real alpha;
	real value40;
};

struct s_24e9c7_placement
{
	real forward_i;
	real forward_j;
	real x;
	real y;
	real size;
	bool hidden;
	byte unknown15[3];
	real fade;
	real unknown1c;
	real scale;
};

struct s_24e9c7_point
{
	real x, y;
};

struct s_24e9c7_points
{
	s_24e9c7_point n[4];
};

/* Draws one marker's sprite at its placement; returns the sprite's height. */
// score 125 (was 125): round 112 (m3 lane 12) note at the end. The frame size, the bounds stores and the marker copy agree; what is
// MiniMax G (11-14): no new form, no lower score (125, was 125): retail's early across-product spill at [ebp-8] still has no counterpart here, so the result, kind and bitmap homes stay eight bytes above retail's and the 1.0 constant stays in xmm4 where retail uses xmm5; the extra room comes from retail's scratch chain of summed values below the marker copy, which this build keeps in registers, and no form listed in the earlier rounds' notes produces that chain.
// MiniMax B (11-12): no new form, no lower score (125, was 125): retail computes the two across products early and
// spills the second one to [ebp-8], which it then reuses for the side vector's y, so its result home is [ebp-0x10];
// this build sinks the products into the side computation, so they never get a stack home and the frame slots stay
// eight bytes above retail's.
// MiniMax G (11-11): no new form, no lower score (125): the result, kind and bitmap homes stay eight bytes above
// retail's, the 1.0 constant stays in xmm4 where retail uses xmm5, and the sprite bounds are copied through esi where
// retail loads the sprite pointer into eax. This round: the corner offsets written as `-side.x` and `-side.y` in place
// of the back vector 167 (722 bytes), the two across products as one point-typed local 144 (746 bytes), the half width
// and side before the height 131, the back vector from the across products 154 (753 bytes), the corner sums written
// out per component 229 (664 bytes), the negation spelled `size * (0.0f - forward_j)` 138 (739 bytes), the across-j
// operand order swapped 125 and the scale read before the size 125 (the same code each). A named sprite pointer for
// the bounds copy gives retail's `mov eax` / `lea esi` staging and 123, but it is a pointer local used once, so it is
// not kept; the across values as one array 142 and the size inlined into the products 141. The permuter (612 s, 103
// tries) also ends at 123, again by swapping two arguments of function_22a664, which changes what that call receives,
// so the body stays.
// M3 lane 1 r96 (fast, 0x22a664 matched): the alpha as one conditional expression stored straight into the global 125;
// the kind test as two nested ifs 125; across i as `-size * forward_j` 125; the half width converted once into a named
// real used for both side products 123 (two points: not kept), with the full width named then halved 123, with the named
// real declared right after the half width 125, with `size * (0.0f - forward_j)` 134 (739 B), with side before up 125;
// the side and back products moved after the first corner's origin copy 138 (740 B; across still sinks). Re-measured:
// constant-first size 125, the bounds as one initializer 125, the marker's kind in a long local 130.
// MiniMax C (11-09): no new form, no lower score (125): the full build's diff shows the group of
// the result, kind and bitmap homes eight bytes above retail's, with retail's spill at -8 and its
// unused slot at -0xc above them, and the 1.0 constant in xmm4 where retail uses xmm5; the bounds
// copy also differs in one instruction pair, retail loading the sprite pointer into eax and taking
// its bounds with lea where ours loads it into esi and adds the offset, so the sprite copy's
// source pointer is a different kind of value here. The permuter was stopped by its time limit
// without a result.
// MiniMax S (11-06): no new form, no lower score (125): nothing new for the eight-byte-higher homes, the 1.0 constant's register or the SSE schedule.
// MiniMax B (11-06): no new form, no lower score (125); the source permuter (240 s) found 123, but it adds a bool set by one test and tested by the next and swaps two arguments of function_22a664, so the original body is restored.
// MiniMax b (11-04): no new form, no lower score; the result/kind/bitmap homes stay eight bytes above retail's, the 1.0 constant stays in xmm4 where retail uses xmm5, and the geometric SSE schedule stays ours.
// MiniMax D (run 46): no new form, no lower score.
// left is the result, kind and bitmap homes eight bytes above retail's, the 1.0 constant in
// xmm4 where retail uses xmm5, and the geometric SSE schedule — no source form found this round.
// Earlier (125, was 125): reassociating the corner sums and splitting them into two steps both
// canonicalise back to retail's association, so what is left is the SSE schedule itself, the
// eight-byte-higher stack homes and the sprite pointer staging; no source form found.
// Earlier (125, was 125): result/kind/bitmap homes sit 8 bytes above retail's (retail keeps a spill at -8
// and an unused slot at -0xc above them), the 1.0 constant is in xmm4 where retail uses xmm5, and the
// geometric SSE schedule differs. Tried: table read at each use 125 plain (743/743), real half width
// 135f, direct height 135f, sprite pointer 133f, global optimization off 716f. Truncation helper at
// height 194, width 167, both 198; side before upper 139; both sides before upper 138; direct alpha
// stores 125; const metadata 125; native sprite rectangle 125. Round 280: plain quad passes 198; faded
// alpha first 125; lookup scope ended 125; shared bounds value 125; x then y coordinates 130. Upper
// components 125; side components 125; reverse components 133; all components 133; earlier upper
// displacement 123 plain (two points, dropped), with components 123; direct reverse uses 167. shader
// snapshot 174; sequence count snapshot 160; sequence field at use 125; unscaled height/width/both 125;
// shared forward values 161; external bitmap reference 125. upper/side/reverse/all value initialization
// 125; origin/sequence references 125; speed policy 279; frame omission 125. Native input
// direction/center/both 125/125/125; direct quad array 125; span locals 125; early basis / scale /
// basis with spans 125/125/125 (fast). Basis i/j/both product input orders 125; upper or side inputs
// 125; basis with upper / side / both input orders 125. Point origin library copies 252; native
// bitmap/placement/sequence values 215/176/205; bitmap plus sequence 230; placement plus copies 176;
// opacity snapshot 139; placement plus bitmap 183. Origin / upper-copy / upper / positive-side passes
// 188/220/240/204; origin-upper 195; origin-side 206; upper-side 214; lower-copy pass 193. Scalar
// origin x/y/both 194/194/194; scaled height/width/both accumulation 125/125/125; side-product updates
// 125; y capture with side updates 194. upper / negative-side / positive-side addition inputs
// 125/125/125; upper-negative / upper-positive / both sides / all additions 125/125/125/125; all
// additions with constant-first size 125. selected bitmap word 153; bitmap after bounds 125; word
// bitmap/long sequence 169; late word bitmap 153; shared opacity tail 142; tail with word/late bitmap
// 161/142; branch-set tail 142. external group/live count/live shader 125/125/125; live scale/group
// with scale 141/141; count with shader/size accumulator/across component lifetimes 125/125/138. Round
// 132: across as a point struct 144; 0 - forward_j 138; size inline 143; first_bitmap declared before
// bounds 125; result assigned after its declaration 125.
// Entry declarations with in-place assignments: across carriers 125; metadata carriers 125;
// both pairs 125; bounds workspace 125; three displacement workspaces 125; all geometry
// workspaces 125; reversed geometry declarations 125; coupled scalar role names 125.
// Round 193: complete per-use kind/bitmap/group observations 207/182/205;
// kind+bitmap/kind+group/bitmap+group/all 225/223/201/225;
// all with per-use sequence field 326. All original observations restored.
// Round 303: early outer/texture/selected-bitmap admissions 128/127/149;
// outer+texture/outer+bitmap/texture+bitmap/all 130/149/149/183;
// all with returned height declared after admission 183. Original scopes and body restored.
// Round 41 (fast): retail computes size * (0 - forward_j) and size * forward_i before the height
// block, spills the second at -8 and reuses forward_i for the upper vector; ours folds the
// single-use across terms into the side products (computed late, forward_i read twice). Side
// vector scaled in place by the half width 144, with the half width first 144, negated forward
// copied then scaled in place 139 (gives retail's 0 - forward_j); named perpendicular components
// both/one 138/138. Original body restored.
// Round 46: retail computes the across pair right after size as size * (0 - forward_j) and spills
// its j part at -8, the same home later used for the scaled side j (ours sinks both products to the
// side and negates after multiplying). Side vector built then scaled in place 144; perpendicular
// then in-place size scale 139; scale field at each use 141; negated operand first 125; marker
// global stores before the geometry 149. Original body restored.
// Round 313: Shared real half-width, with direct/named full pixel width: current basis 125/125, early
// upper 123/123, deferred basis 129/129, both lifetimes 128/128. Two-count gain rejected; original
// geometry and body restored; early spill and scalar homes remain.
// Round 48: VC rewrites `size * -forward_j` as `-(forward_j * size)` and sinks it (ours); `0.0f -
// forward_j` or a negated-component local keeps retail's early `size * (0 - forward_j)`, yet the
// pair's home stays below result: across built in the side vector and scaled in place by the half
// width, with `0.0f -` 139; with unary minus 144; `0.0f -` alone (a listed form, as a base) 138;
// negated-component locals scaled by size 138; scale read before size with `0.0f -` 137.
// Round 322: Shared upper-origin point copy/value/twice-used coordinates/first vertex at assembly 168/147/190/147; at the height producer 153/146/189/147. Original corner sums, geometry and body restored; early across spill and shifted scalar homes remain.
// Round 454: Complete center point on the live-table entry graph, native copy at corner
// assembly 125, with all corner origins initialized first 129; library move 240/240.
// Basis-boundary counterparts 125/129/195/196. Original point reads, copies and geometry
// restored; the early across spill and shifted scalar homes remain.
// Round 9 (lane 17): a compile-only /O1 /FAs build reproduces this frame and code (with the two pointer
// parameters' registers swapped). An 8-byte point local always homes below the four scalars (ours: the
// back pair at -0x18, its y spilled at -0x14), while a spilled scalar homes above result; retail's -0xc/-8
// are two scalar homes above result (-8 holds size * forward_i, then the scaled side y; -0xc is never
// touched). 144 harness forms (scale read first, across as a point/scalars/scalars scaled in place, up and
// back as points or scalars, `0.0f -` or unary minus, size sum order, named forward components) never
// spill the across pair, so none reaches retail's two homes. Real (fast): back as two scalars 133 (frame
// 0x44: back y at -8 above result -0xc); early across scalars scaled in place, scalar up/back, `0.0f -`
// 155 (735 bytes); the same with separate side scalars 155; across built in the side point and scaled in
// place with scalar back 139 (the listed form; side y spilled at -0x14); that scalar form with scale read
// first 155.
// Round 50 (lane 1, fast): on the in-place side point with `0.0f -` (139), the point renamed across /
// width_offset / lateral / a_side 139 each with every home unchanged (names do not move it above result);
// j component first 138; (0 - forward_j) * size operand order 139; `side.x = (real)half_width * side.x`
// 139; that with both pairs j first 139.
// Round 35 (lane 13, fast): retail's -0xc/-8 pair reads as one 8-byte point (x kept in a register, its home
// unused; y spilled, then scaled in place to the side y); tried: across as a point scaled in place into the
// side vector, declared at function entry 139 (742 bytes) or at its computation 139; a separate across point
// declared at entry 139; an initialised across point 139; size computed before the bitmap pointer 125 (same
// code). Every point local still homes below the scalars.
// Lane 18 round 15 (fast): result renamed to sort first 125 (its home stays at -8: these are spill slots, names
// do not order them); back as scalars re-measured 133; back plus up scalars 133; back plus side scalars 133;
// up, side and back all scalars 133; back scalars with `size * (0.0f - forward_j)` 146 (739 bytes); all scalars
// with it 146 (739 bytes).
// Round 145: a compile-only frame screen shows a spilled scalar homes above result only when the back pair
// is scalar (frame 0x44, one slot short of retail's two). Real (fast): across scaled in place as side
// scalars with scalar up/back and `0.0f -` 155 (735 bytes); scalar back only 146 (739); no back, negated
// side terms at the corners 170 (718); side as a point built from across and scaled in place with scalar
// up/back 139 (742); the half width named and multiplied first 143 (739).
// Round 493: double size/across-pair/scale precision subsets 169/166/166/189/202/201/201;
// complete double graph with double returned-height carrier 211. All real carriers restored;
// early across spill, shifted scalar homes and SSE schedule remain.
// Round 233: y-first coordinate producers upper/side/reverse, each pair and all three
// 137/125/125/139/137/125/139; all three with early upper displacement 122. Original
// geometry, observations and complete body restored; early spill/scalar homes remain.
// Round 71 (lane 12): a compile-only /FAs screen names each home: a spilled 8-byte point always homes below
// bitmap_index, a spilled scalar above result (so retail's -0xc/-8 are two scalar homes, -0xc never used, as if
// two spilled scalars were later merged into one slot), and none of 48 across/side/up/back point-or-scalar shapes
// with unary minus or `0.0f -` gives two such homes (at most one: frame 0x44). Real (fast): across as a point with
// separate side scalars, `0.0f -` 143 (742 bytes, frame 0x44, side y at -8), with unary minus 148 (746); across
// and side as separate points with `0.0f -` 139 (742); that with up as scalars 139 (742); across built in side
// scalars scaled in place with scalar up and a back point, `0.0f -` 138 (739).
// Round 493: double size/across-pair/scale precision subsets 169/166/166/189/202/201/201;
// complete double graph with double returned-height carrier 211. All real carriers restored;
// early across spill, shifted scalar homes and SSE schedule remain.
// Round 233: y-first coordinate producers upper/side/reverse, each pair and all three
// 137/125/125/139/137/125/139; all three with early upper displacement 122. Original
// geometry, observations and complete body restored; early spill/scalar homes remain.
// Round 35 (lane 15, fast): ours spills back's y (pair at -0x18/-0x14), retail side's y (pair at -0xc/-8, above
// result). back renamed reverse_side / away, side renamed width_offset, up renamed upward 125 each (names do not
// order the struct homes either); corners as full sums `x + up.x + back.x` 229 (664 bytes); scale read before size
// 125; result assigned after the half width 131; the full width named, then halved 125.
// Round 19 (lane 18): retail's frame is points 0x20 + bounds 0x10 + six 4-byte spill slots
// (first_bitmap -4, across j then side j -8, never-touched -0xc, result, kind, bitmap index), no 8-byte
// slot; the across i product stays in xmm6. Compile-only frame screens: spill slots follow spill order,
// not names (eight names for the back y, and result renamed, never move); only a scalar back pair puts
// a spill above result (0x44, one slot short); up/side forms do not. Real (fast): side scalars set to the
// across products and scaled in place, `0.0f -`, back scalars, up point 155; same, no back variable, up
// scalars 153 (732 B); same as the first with unary minus 155; separate across scalars, back scalars, up
// point 155; in-place side, back point, up scalars 147 (735 B otherwise); all sink the across products.
// Round 194: a compile-only /FAs frame screen puts an 8-byte side point between the scalars once its
// address goes to an inline pointer helper (side -0x10, between result -8 and kind -0x14; retail's pair
// sits above result); with its address taken first (perpendicular then scale helpers) it stays below them.
// Real (fast), side scaled in place by the half width through a pointer helper 134 (752 bytes), with up
// and back as scalars 134; side from the negated forward scaled by size and then by the half width 145
// (755); up scaled through the helper as well 143 (745); back negated through a helper 172 (766).
// Round 150, each on the early-across form (`size * (0.0f - forward_j)`): side products across-first 138 (739
// bytes); a named real half width for both side products 134; up computed before the half width 134; up as
// `0.0f -` products 147 (735); the across pair declared right after size 138.
// Round 493: double size/across-pair/scale precision subsets 169/166/166/189/202/201/201;
// complete double graph with double returned-height carrier 211. All real carriers restored;
// early across spill, shifted scalar homes and SSE schedule remain.
// Round 233: y-first coordinate producers upper/side/reverse, each pair and all three
// 137/125/125/139/137/125/139; all three with early upper displacement 122. Original
// geometry, observations and complete body restored; early spill/scalar homes remain.
// Round 342: Across scalars scaled into the actual side displacement, upper/reverse scalar-pair and shared real half-width subsets: 125/125/133/133/123/123/132/132.
// Original complete body restored; early across spill, shifted scalar homes and SSE schedule remain.
// Round 301: paired rectangle limit chains, minima/maxima first and either chain direction,
// 131/130/127/129; with explicit zero-minus across 144/143/140/142. Original body restored.
// Round 232: Complete rectangle zero-fill then maxima through memset/zero aggregate, at
// group/selector/bitmap/default-workspace boundaries 211/191/221/191/222/192/222/192.
// Original defaults and body restored; early across spill and shifted scalar homes remain.
// Round 344: separate real geometry height with long/double return carriers at producer, width
// and draw boundaries, late real return carrier, then long carrier with early upper displacement:
// 153/153/147/161/161/150/187/152. Original body restored; early spill and scalar homes remain.
// Round 347: Existing two-component setter at upper/side/both/reverse/upper-reverse/side-
// reverse/all/all-with-direct-across producers: 125/125/125/137/137/137/137/137.
// Original complete body restored; early across spill and shifted scalar homes remain.
// Round 467: One native displacement shared across upper/reverse phases, entry storage,
// all origins first and early upper-production subsets: 144/144/143/143/145/145/144/144.
// Scalar homes remain eight bytes high at frame 0x48; original complete body restored.
// Round 355: reused output-corner receivers for each corner, upper/lower pairs and all
// corners 125/125/125/125/125/125/125; all with early upper displacement 123.
// Two-point reduction rejected; original body restored, early spill/scalar homes remain.
// Round 40 (m3 lane 2, fast; all restored): the perpendicular point scaled by size and then by the half width through an inline scale helper
// 144 (745 bytes, aligned 121); side from the size products, then through that helper 134 (752, aligned 118: the across products stay ahead of
// the height conversions as retail, the pair homes at -0x10 under result -8; retail has result -0x10 under the pair -0xc); scaled in place with
// no helper 139 (742, aligned 116); `0.0f -` early across 138 (739); corner sums written offset first 125 (same code), with those forms 139/134.
// Compile-only frame screen on the helper form: side declared at function top, side or result renamed to sort first or last, result assigned
// late: pair -0x10 and result -8 in all eight.
// Round 40, continued (fast): unnamed values: size written out at both products 143 (739), with `0.0f -` 149 (736), across written into the side products 125, scale
// written out 141 (740), height not named 125, half width at each use 139 (738), several of these 151 (729). Corner copies ahead of the offsets: first corner before up
// 138 (740), all four before up 128 (743, aligned 114), first before size 124, all four before size 131, first before side 138, the points declared earlier 125/125.
// Grid (corner copies in place/all before up/first two before up/all before side, each with unary or `0.0f -` across and unary or `0.0f -` offsets): 125/139/138/147,
// 128/128/138/138, 137/137/138/138, 129/139/138/147; best aligned 111 at the 129.
// P284: Existing shared point type for upper/side/reverse displacements, all seven
// subsets, then the complete graph with native quad point elements: eight at 125.
// Original private record types and full body restored; early across spill,
// eight-byte-high scalar homes and geometric SSE scheduling remain.
// Lane 17 r36 (fast): across as a point local right after size 144 (746 bytes); side assigned the `0.0f -` across
// products and then scaled `side.x = (real)half_width * side.x` 139 (742; retail's -8 slot is that across/side y),
// with unary minus 144, with `*=` 139, with side declared at the top of the inner block 139 (same code). Original
// body restored; side gets no frame home, so result stays at -8 (retail -0x10 under the side pair).
// Round 33 (m3 lane 18, fast; 0x24e59f matched each time): the default bounds as one initializer
// 125 (same code); the kinds table read once into a local 135 (744 bytes); the marker's kind in a
// long local 130; the alpha preset to 0 without the hidden arm 125 (same code); the half width
// computed before the height 131. All restored; early across spill and scalar homes remain.
// Round 390: Actual four-real rectangle array with memcpy/memmove, producer/entry/
// kind/group storage: 194 in all eight forms (760/743 bytes); caller also loses its match.
// Original record, copy and full body restored; early across spill and scalar homes remain.
// Round 82 (m3 lane 12, fast): forward_i named for the across j product and the upper x 140 (742 B); scale
// read before the across products 125 (same code); first_bitmap and bounds declared before the group 125;
// back as `half_width * -across` products 154 (753 B); upper vector before the half width 123 (gain
// under four, not kept). Original body restored.
// Round 129 (m3 lane 8, fast): retail reads placement->y once (EDX, copied to each corner) and x at each
// corner; origin y read once into a local, x per corner 194 (699 bytes); scalar x/y corner copies 191 (695);
// corners 0/1 as `back.x + (up.x + corner.x)` single sums 137 (749); early upper with y-first corner 0 126;
// early upper with `0.0f -` across 134 (739). Original body restored.
// Round 390: Actual four-real rectangle array with memcpy/memmove, producer/entry/
// kind/group storage: 194 in all eight forms (760/743 bytes); caller also loses its match.
// Original record, copy and full body restored; early across spill and scalar homes remain.
// Round 68 (m3 lane 1; "h" = a one-link LTCG harness with unknown_22a664, unknown_24e59f, globals.cpp and the other globals defined and
// their addresses taken, listing equal to the full build's, reads 123 for this body): retail's corner sums copy the offset and add the memory
// corner, which says the corners are defined before the offsets. Grid of across `size * -j`/`size * (0.0f - j)`, up and back unary/`0.0f -`,
// all four corner copies in place/before the height/after the result/after the half width, sums `+=`/`offset + corner`: `+=` and
// `offset + corner` same code, unary and `0.0f -` on up same code; corners after the half width 128 plain (aligned 114 against 119, the
// lowest aligned), after the result 129h, before the height 131h; `0.0f -` across 135h (739 bytes), after the result 133h, after the half width
// 135h. Across in place into side scalars 136h (738 bytes), after the result 129h; separate side scalars and a side point with `0.0f -`
// across 136h; back scalars with those 144h. forward_i named for the across product and up.x (retail's one read in XMM4) with
// `0.0f -` across: scale first or last, half width before or after up, all corner places: best 131h; corners after the result with half width after
// up 143 plain (748 bytes), the first form to put kind and bitmap at retail's -0x14/-0x18 with a 5-reference spill beside result, but
// ours orders result -8, spill -0xc, unused -0x10 where retail has spill -8, unused -0xc, result -0x10. The across pair declared at the function's
// top, ahead of result, in both orders, on all 32 of those forms: every home unchanged, best 131h.
// Round 143 (m3 lane 6, fast): scale read before size and the across products 125 (same code); across_j before
// across_i 125 (same code); across_i as `-forward_j` then `*= size` 138 (739 B: the across products are still
// substituted into their single uses, after the height conversions); result renamed to sort first/last 125/125
// (diagnostic). Retail computes both across products right after size (across_j spilled to -8), so they are not
// single-use temporaries there.
// B413: Reverse/side/upper/full geometry scopes ending at final corner consumption,
// with original/immutable complete scalar inputs: 125/125/125/125/125/125/125/125.
// Original complete body restored; early across spill and shifted scalar homes remain.
// B416: Plain two-coordinate corner traversal, long/int/short/unsigned-short indices
// and successive/original-tree equations: 214/214/214/214/209/209/209/209. NOT kept: frame grows and
// extent shrinks away from retail; original point type, equations and complete body restored.
// B419: Native two-axis fraction/pixel/scaled-pixel/raster-extent values, height-first
// fields then readonly aggregates: 125/125/125/125/125/126/126/131. NOT kept: grouped dimensional values
// do not match. Original scalar owners, integer conversions, corner equations and body restored.
// M376: Signed-int selected-bitmap/height/half-width producer domains, seven subsets,
// then all with signed-int bitmap handle: 125/125/125/125/125/125/125/125. NOT kept: no match or qualifying gain;
// original domains/body restored; early across spill and shifted scalar homes remain.
// M3 lane 13 r63 (fast, restored; retail 9 `addss reg, [corner]` and 1 corner load, ours 7-9 loads whatever the spelling): retail's `mov eax, edx` before each
// corner y is a post-allocation reuse of the first corner's y load, which appears when the first corner is copied before up while EAX still holds the width
// (order height, result, width, first corner, up, side, later corners; early-across products unnamed forward_i) 147 (757 bytes, aligned 110). On it side and back
// as scalars, `0.0f -`, side scaled in place 146 (757): first form with retail's frame 0x48, result -0x10, kind -0x14, bitmap -0x18 (masked aligned 94 -> 85), but the
// across products sink into the side products, so -8/-0xc hold back and the half width. With forward_i named 145 (751, frame 0x4c, result -0xc); side as a point
// 160 (761, frame 0x50: early across as retail, pair below bitmap). Same homes with up scalar, points/side/back declared first or reversed, sums written offset first
// (same code). Corner orders on the `0.0f -` across form, plain/(real)height in up/forward_i unnamed: height width result first-corner 148/142/153; result before width
// 142/142/147; first corner after up 148/133/153; first two corners before up 154/148/164; first corner before result 153/142/161; first corner before up, side
// before the sums 152/146/146; first corner after side 149/148/144; second corner's sums after side 149/143/154; all four before up 151/145/161. Inline scale and add
// helpers (diagnostic) 136-176 (best add helper with named forward_i 136, 749 bytes, frame 0x4c; 142, 738 bytes, without it); corner writes through a walking pointer,
// named pointers or references 139-146. Result only assigned, (real)height for the up products, 142-156. Original body restored.
// On that form the across pair given a second use (back written from the across products, so CSE merges it): side and back scalars 155 (758, frame 0x50), with forward_i named 159, a named real half width 162; no side or back locals 156 (728, frame 0x48, result -8);
// back before side 150; `0.0f -` back 155. Original body restored.
// B435: Existing vector2f component-array upper/side/reverse observations, seven
// subsets, then all with the perpendicular pair: 125/125/125/125/125/125/125/144.
// NOT kept: no exact vector-storage or early-spill recovery; original types/body restored.
// Early across spill, eight-byte-high return/metadata homes and SSE scheduling remain.
// B439: Output-origin transfers from the first real vertex, seven nonempty receiver
// subsets then a successive output-copy chain: 129/129/129/129/129/129/129/129.
// Original complete body restored; early across spill and scalar homes remain.
// B441: Existing early perpendicular setter, separate/reused side ownership,
// direct/pre-scaled complete inputs and unary/zero-minus negative direction:
// 144/139/139/139/144/139/139/139. NOT kept: no equality; original scalar pair, full body and
// declaration set restored. Early spill and eight-byte-shifted scalar homes remain.
// B443: Exhaustive returned-height producers at three admission gates, all eight
// rejection-first/success-first arm masks: 186/186/186/186/186/186/186/186. NOT kept: all grow
// to 748 bytes at frame 0x48; original entry zero/full body restored.
// Early across spill, eight-byte-high scalar homes and SSE scheduling remain.
// B445: Complete opacity producer at global-copy/corner/reverse/side/upper/width/height/basis boundaries:
// 139/173/172/185/166/171/171/172. NOT kept: moved one-use producers do not match; original whole body restored.
// Early across spill and eight-byte-high scalar homes remain.
// Round 130 (m3 lane 7, fast): retail computes the across terms ahead of the height conversion and reuses across j's home for side y:
// a side vector scaled in place, with slots -0xc/-8 above the result. Direct fields scaled in place 144 (746 B), through `real half` 154
// (756 B); named scalars in place 125/125, with `real half` 123, through a copy 123 (2 points, slots unmoved). Pointer helpers: set and
// scale 134 (752 B; kind and bitmap homes become retail's, side at -0x10 below the result), scale only 154/134, unit perpendicular 144.
// Result references lowered: temporary for up 141, named extent 125, with the side 142/143, on the helpers 134/134, assigned last 159;
// up through a helper 134; `0.0f -` forms for up with the scalars 139 (738 B). Retail keeps the y coordinate in EDX and re-reads x: named y 194/186/207, both at each use 191 (684-705 B).
// Round 217 (m3 lane 3, fast, base 125 at 743 B): across products as `size * (0.0f - forward_j)` scaled in place by a `real half` with back as `0.0f - across` 134 (739 B); side and back computed after the first corner's base copy 138 (740 B), and with points declared there too 138;
// back from the across products (`half_width * (0.0f - across)`) 164 (764 B); half_width kept as a real from the division 125 (same positions, other masked code). Slot order unchanged in all: retail's across homes sit above the result.
// M3 lane 11 r72 (fast, control 125, masked 92): side scalars set to `size * (0.0f - forward_j)` / `size * forward_i` and scaled in place by
// the half width, side and back kept as points 138 (739; homes unchanged); the bitmap entry indexed at both uses 144 (746, masked 78); `up.x`
// written `-(placement->forward_i * result)` 125 (same code); `across_i` written `-placement->forward_j * size` 125 (same code, still sunk); bitmap
// at each use with `size * (0.0f - forward_j)` 138 (742, masked 79).
// M397: Immutable actual upper/side/reverse records, masks 1/2/4/3/5/6/7,
// then all with immutable scalar producers: 125/125/125/125/125/125/125/125. NOT kept: no exact or
// eligible gain; original records, observations and complete body restored. Early spill/homes remain.
// DH381: Native scalar-SSE dimension conversion/full-expression producers, height/width
// modes 10/01/11/20/02/22/12/21: 241/241/241/230/229/251/233/242. NOT kept: all force frame alignment absent in retail and expand the body; original restored.
// N396: Signed eight-byte selected ordinal/pixel-height/half-width storage, seven subsets
// then full bitmap-handle graph: 220/125/220/148/235/148/235/238. NOT kept: no native narrow-domain
// equality or gain; original body restored, early across spill and eight-byte-high scalar homes remain.
// M3 lane 16 r97 (fast, 0x24e59f matched each time; body restored): plain two-real arrays in place of the point
// locals, each with `size * (0.0f - forward_j)`: side set to the across products and scaled in place 139 (742 bytes);
// across an array with side a point 139 (742); side and back both arrays 139 (742); the side array with forward_i
// named for the across product and the upper x 141 (741); across and side both arrays 139 (742). The sprite's bounds
// declared as a four-member record and copied from its address 125 (same code: the `mov eax / lea esi` pair of
// retail's bounds copy stays `mov esi / add esi`). An array homes like a point, below the scalars.
// M3 lane 11 r81 (fast): frame map: retail's 8-byte pair (-8 used for across j then side y, -0xc untouched) sits above result,
// ours (-0x14 back y, -0x18 untouched) below kind/bitmap. Side, up or back declared at function top (before or after result) 125 each
// (same code); first corner's base copy after the half width 138 (740 bytes, as round 217); all four corner base copies there 128 (aligned 121 -> 116,
// the across terms still sink: points has not escaped); first corner's base copy right after the across terms 124.
// Round 147 (m3 lane 7, fast with 0x24e59f and 0x22a664, both matched each time; "masked" = aligned count with registers
// and frame offsets masked, 93 for this body): all four corner base copies right after the across terms 131 (masked 95);
// the first two there 125 (93); the first one there re-measured 124 (96); on that, up before the half width 141 (738 B,
// masked 90); the across products as a side point scaled in place by the half width 145 (746 B, masked 86); both 157
// (756 B, masked 92); on the first-copy form, the other three base copies right after the half width 127 (masked 114)
// and the half width computed before the height 131 (masked 94). The in-place side gives retail's shared
// across-j/side-y home but the across products still sink.
// Round 65 (m3 lane 2, fast; 0x24e59f matched each time; register-masked aligned in brackets, control [94]): re-measured lane 17
// r36's in-place side point 144 (746 bytes) [107], with `*=` 144, with `0.0f -` 139 (742) [105], round 33's bounds initializer
// 125, the constant-first size 125. New: size and both across products before the bitmap pointer 125 (same code); the
// shader-index test first 129 [102]; the bounds by chained assignments 131 [107]; height and half_width `long const` 125 (same
// code); `result = height` 125 (same code).
// Lane 5 r166 (fast, 0x24e59f matched each time, control 125): the default bounds as two chained assignments 131; the
// sequence index read from the kind at each use 125 (same code); the bitmap index read from the kind at each use 182 (739 bytes);
// `if (first_bitmap == NONE) return result;` with the drawing block un-nested 149 (first difference +0xe); the two outer
// guards as early returns 130. Body restored.
// m3 lane 14 r77 (fast, 0x24e59f matched each time; masked = difflib count with registers and offsets masked, control 60):
// re-measured listed forms: an initialised across point, `{ size * (0.0f - forward_j), size * forward_i }`, with side
// from it 139 (742 B, masked 64; lane 13 r35's), side initialised with the across products and scaled in place 139 (same
// code; lane 17 r36's). On that initialised across point: forward_i named for the across product and up.x 141 (741 B);
// unary minus in the initialiser 144 (746 B); the scale read right after size 140 (masked 63); up initialised as
// `{ -(result * forward_i), -(forward_j * result) }` 139, back as `{ -side.x, -side.y }` 139, both 139 (all the same code).
// There the across products stay ahead of the height conversions but in XMM registers through that block (retail
// spills across j to -8), so result keeps -8 and side y spills at -0x14 below the scalars. Retail's -8 slot has five
// references (across j stored and reloaded, side y stored, reloaded for back and for the corners), which puts it above
// result by weight.
// Round 91 (m3 lane 1, fast with 0x24e59f and 0x22a664, both matched each time; control 125): the forward pair copied
// into a planar point local (the vector's first two members) for the across products and up 147 (752 bytes), for the
// across products only 151 (755); the negated forward j named (`real perpendicular_i = -forward_j;`) after size 138 (739),
// before size 138; an unscaled perpendicular point `{ -forward_j, forward_i }` scaled by size in place 139 (742).
// Diagnostic: `back` renamed to sort last 125 (same code). In every form the compiler folds the negation out of the
// product (`0 - fj * size`) and sinks it into the side products; retail computes `size * (0 - fj)` right after size
// and spills across j at -8. Body restored.
// M3 lane 3 r231 (fast with 0x24e59f and 0x22a664, both matched each time; control 125): the scale read before size 125 (same
// code); the across j product before across i 125 (same code); the half width halved as a second statement (`half_width /=
// 2;`) 125 (same code); the four marker stores to the view globals before the corner points 131 (retail interleaves them with
// the last corner sums); `up.y` as `-(result * forward_j)` 125 (same code). Close to round 147's half width before the height:
// the result conversion after the half width 131.
// M3 lane 12 r112 (fast with 0x24e59f/0x22a664, both matched; 125, aligned 87, masked 61). Kept (same code, 125, plain
// confirmed): the shared bitmap types from bitmap_sequences.h (tag, sequence, sprite, dimensions) and box2f for the bounds,
// in place of this file's private views and the cast of the sprite's bounds through a private four-real record. Checked:
// our unary minus already compiles to retail's `0 - x` (xorps, subss) for the up and back offsets.
// No other untried natural form: the note covers the across/side/up/back products as points, scalars, arrays and the
// shared point types, in place and through helpers, every corner copy placement, the bounds and opacity spellings, the
// domains of the conversions and the declaration orders; what is left is the early across spill to -8 and its pair's home
// above the result, the 1.0 register and the sprite pointer's `mov eax` / `lea esi` staging.
// M3 lane 12 r114 (fast with 0x24e59f/0x22a664, control 125 [masked aligned 85, register-masked 60]): the sprite's bounds as
// `sequence->sprites[0].bounds` 125 (same code); through a named `box2f const *` to the sprite's bounds 155 (745 B: the staging
// becomes `mov eax, [eax+0x38]; add eax, 8; mov esi, eax`, still not retail's `lea esi, [eax+8]`); `sequence->sprite_count > 0`
// 126 (`jle` for `je`); the named bounds pointer with `size * (0.0f - forward_j)` 155 (741 B); the bounds by memcpy 194 (760 B,
// 0x24e59f then 294, so it is out).
// Also: the alpha's hidden arm first (`else if (placement->hidden) alpha = 0.0f; else ...`) 125 (same code); back as a copy of side
// negated in place 175 (767 B); each corner's x sums before its y sums 137 (749 B).
// M3 lane 5 r177 (fast, 0x24e59f and 0x22a664 matched each time): the up products as `-result * forward_i` and
// `-result * forward_j` 139 (738 B); across i as `-(size * forward_j)` 125 (same code); the bitmap index read inside
// the bitmap/shader test (the test reading the field) 125 (same code); corner 3 before corner 2 125 (masked 60 for 61:
// stores only); the kind pointer `const` 125 (same code).
// M3 lane 9 r118: no trial run. First difference +0x17 is still the result, kind and bitmap homes eight bytes above retail's
// (retail's early across-j spill at -8 sits above them). Lane 12 r112 above records that no untried natural form is left,
// and every lead here (across/side/up/back as points, scalars, arrays, helpers, in-place scaling, corner orders, bounds and
// alpha spellings, conversion domains) is listed; I found no code-changing form outside the note.
// @retail 0x24e9c7
real function_24e9c7(s_24e9c7_marker const *marker, s_24e9c7_placement const *placement)
{
	real result = 0.0f;
	if (g_510c94 && marker->kind >= 0 && marker->kind < ((s_24e9c7_kinds_view *)g_510c94)->count)
	{
		s_24e9c7_kind *kind = &((s_24e9c7_kinds_view *)g_510c94)->entries[marker->kind];
		long bitmap_index = kind->bitmap_index;
		if (bitmap_index != NONE && kind->shader_index != NONE)
		{
			s_bitmap_tag_22ae21 *group = (s_bitmap_tag_22ae21 *)g_4e3b44[bitmap_index & 0xffff].bytes;
			short sequence_index = kind->sequence_index;
			box2f bounds;
			long first_bitmap = NONE;
			bounds.x0 = 0.0f;
			bounds.x1 = 1.0f;
			bounds.y0 = 0.0f;
			bounds.y1 = 1.0f;
			if (sequence_index >= 0 && sequence_index < group->sequence_count)
			{
				s_bitmap_sequence_22ae21 *sequence = &group->sequences[sequence_index];
				first_bitmap = sequence->first_bitmap;
				if (sequence->sprite_count)
					bounds = sequence->sprites->bounds;
			}
			else if (sequence_index == 0 && group->sequence_count == 0 && group->bitmap_count > 0)
			{
				first_bitmap = 0;
			}
			if (first_bitmap != NONE)
			{
				s_bitmap_dimensions_22ae21 *bitmap = &group->bitmaps[first_bitmap];
				real size = placement->size + 1.0f;
				real across_i = size * -placement->forward_j;
				real across_j = size * placement->forward_i;
				real scale = placement->scale;
				long height = (long)((bounds.y1 - bounds.y0) * bitmap->height * scale);
				result = (real)height;
				long half_width = (long)((bounds.x1 - bounds.x0) * bitmap->width * scale) / 2;
				s_24e9c7_point up;
				up.x = -(result * placement->forward_i);
				up.y = -(placement->forward_j * result);
				s_24e9c7_point side;
				side.x = (real)half_width * across_i;
				side.y = (real)half_width * across_j;
				s_24e9c7_point back;
				back.x = -side.x;
				back.y = -side.y;
				s_24e9c7_points points;
				points.n[0] = *(s_24e9c7_point const *)&placement->x;
				points.n[0].x += up.x;
				points.n[0].y += up.y;
				points.n[0].x += back.x;
				points.n[0].y += back.y;
				points.n[1] = *(s_24e9c7_point const *)&placement->x;
				points.n[1].x += up.x;
				points.n[1].y += up.y;
				points.n[1].x += side.x;
				points.n[1].y += side.y;
				points.n[2] = *(s_24e9c7_point const *)&placement->x;
				points.n[2].x += side.x;
				points.n[2].y += side.y;
				points.n[3] = *(s_24e9c7_point const *)&placement->x;
				points.n[3].x += back.x;
				points.n[3].y += back.y;
				g_4e6950.field_0 = marker->color;
				g_4e6950.field_c = marker->point;
				g_4e6950.value18 = 0.0f;
				g_4e6950.value24 = marker->value40;
				real alpha;
				if (marker->use_full_alpha)
					alpha = g_4e69c0[g_4b9ed8] * marker->alpha;
				else if (!placement->hidden)
					alpha = (1.0f - placement->fade) * g_4e69c0[g_4b9ed8] * marker->alpha;
				else
					alpha = 0.0f;
				g_4e6950.value1c = alpha;
				function_22a664((s_widget_quad_2b11 const *)&points, (s_float_rect const *)&bounds,
					bitmap_index, first_bitmap, kind->shader_index);
			}
		}
	}
	return result;
}
