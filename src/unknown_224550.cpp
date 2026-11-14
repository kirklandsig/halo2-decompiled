#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <stddef.h>

// @flags /O2 /arch:SSE /Gr

struct s_tag_data { long size; byte *address; };
struct s_particle_property
{
	short input_index, range_index;
	word modifier;
	short modifier_index;
	s_tag_data function;
};
struct s_particle_value_cache
{
	real values[17];
	dword mask;
	void *field_48;
	void *field_4c;
	void const *field_50;

	void function_173c10(dword mask);
};
union s_draw_sprite_uv { box2f bounds; real values[4]; };
struct s_draw_sprite_definition
{
	dword flags;
	byte field_4[4];
	short first_sequence, sequence_count;
	byte field_c[0x1c-0xc];
	s_particle_property color, alpha, size, angle, frame;
	byte field_6c[0x98-0x6c];
	dword field_98;
	s_draw_sprite_uv geometry;
	box2f bounds;
};
struct s_draw_sprite_seed
{
	byte field_0[0x14];
	word field_14, field_16;
	byte field_18[0x3c-0x18];
	dword color;
};
struct s_draw_sprite_frame
{
	byte field_0[8];
	box2f bounds;
	point2f pivot;
};
struct s_draw_sprite_sequence
{
	byte field_0[0x34];
	long count;
	s_draw_sprite_frame const *frames;
};
struct s_draw_sprite_bitmap
{
	short type;
	byte field_2[0x3c-2];
	long count;
	s_draw_sprite_sequence const *sequences;
};
struct s_draw_sprite_context
{
	s_draw_sprite_definition const *definition;
	s_draw_sprite_bitmap const *bitmap;
	s_draw_sprite_sequence const *sequence;
	real width;
};
typedef char check_draw_sprite_mask[offsetof(s_draw_sprite_definition, field_98)==0x98?1:-1];
typedef char check_draw_sprite_frame[sizeof(s_draw_sprite_frame)==0x20?1:-1];

real function_246cd0(s_particle_property const *, real const *);
void function_246d80(s_particle_property const *, real const *, color3f *);
color3f *unpack_color3f(dword, color3f *);
dword __cdecl pack_color4f(color4f const *);
long function_016ae0(real);
void function_23ed30(point3f *,point3f *,real,real,dword,real,real *,real *,real *);

PRIVATE inline long sprite_flip(s_draw_sprite_definition const *definition,s_draw_sprite_seed const *seed)
{
	long flip=(definition->flags&2) && (seed->field_14&1)?1:0;
	if((definition->flags&4) && (seed->field_16&1)) flip|=2;
	else flip&=~2;
	return flip;
}
PRIVATE inline void function_x0ec080(box2f const *bounds,long flip,real *out)
{
	real dy=(flip&2)?bounds->y0-bounds->y1:bounds->y1-bounds->y0;
	real dx=(flip&1)?bounds->x0-bounds->x1:bounds->x1-bounds->x0;
	real y=(flip&2)?bounds->y1:bounds->y0;
	real x=(flip&1)?bounds->x1:bounds->x0;
	out[0]=x; out[1]=y; out[2]=dx; out[3]=dy;
}

// MiniMax B (11-13): the width product is computed before the inverse sums, which lowers the score to 194; the
// blend still leaves XMM5 in the aspect block and the tint's home is still exchanged with the second UV set's bounds.
// score 194 (was 194): earlier 211 (round 95 m3 lane 11 at the end) and 223; the tint and the second UV set's bounds have swapped homes and the aspect pair's
// Earlier first line: score 194 (was 211; round 95 m3 lane 11 at the end): earlier 223; the tint and the second UV set's bounds have swapped homes and the aspect pair's
// Earlier at 194 (was 211; round 95 m3 lane 11 at the end): earlier 223; the tint and the second UV set's bounds have swapped homes and the aspect pair's
// Earlier 211 (was 211; round 95 m3 lane 11 at the end): earlier 223; the tint and the second UV set's bounds have swapped homes and the aspect pair's
// spill homes (the dead `values`/`context` argument slots) are swapped too; the blend leaves XMM5 in the aspect block.
// M3 lane 8 r159 (fast): the aspect defaults declared y first (`aspect_y=1.0f,aspect_x=1.0f`) 209 (the default stores
// then run in retail's order, but aspect_x still takes the context slot +0x8c and aspect_y the values slot +0x78, where
// retail has them the other way round; an order of declarations, not kept); the aspect pair renamed width_scale/
// height_scale 211 (same code: names do not move these slots); the second set's unit defaults stored before the first's
// 215; the direct arm's copy as `uv_b.bounds=uv_a.bounds` 211 (same code); the colour modulation as `tint.red*=color->red`
// 211 (same code); the next frame's bounds copied before the first's 217. All restored.
// MiniMax A (11-12): no new form, no lower score (211); the first difference is still the tint's home,
// ours +0x60 where retail has +0x50, with the second UV set's bounds in the other slot, and the blend
// still leaves XMM5 in the aspect block; the declaration orders the notes above list (both uv sets before
// the tint, the tint in a nested scope, the tint at the top of the function) all give the same code, so
// the swap cannot be moved that way.
// MiniMax b (11-11): no new form, no lower score (211); the tint's home is still exchanged with the second UV
// set's bounds (ours +0x60/+0x50, retail +0x50/+0x60) and the blend still leaves XMM5 in the aspect block; the
// uv sets declared before the tint, the tint in a nested scope (frame 0x50, the tint then at +0x50 but the second
// UV set takes that slot) and the four clamped components written out one by one all give the same code.
// Earlier status line, 211 (was 223): frame 0x60 and the cache/colour/count registers are now retail's; the tint and the
// second UV set have swapped homes (ours +0x60/+0x50, retail +0x50/+0x60), and retail keeps the blend in XMM5
// through the aspect block (ours reloads it), so the extents, reciprocals and aspect homes differ.
// MiniMax R (11-09): no new form, no lower score (211); the tint's home is still exchanged with the second UV set's bounds and the blend still leaves XMM5 in the aspect block, and no spelling of the declaration order moved either this round.
// MiniMax B (11-06) round 2: no new form, no lower score (211); the uv set declared before the
// tint, the two uv sets declared one on each side of the tint and the tint declared at the top
// of the function all give the same code (211), so the tint's home cannot be moved that way: ours
// keeps the tint at +0x60 and the second uv set's bounds at +0x50 where retail has the tint at
// +0x50 and those bounds at +0x60; the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (211); the tint's home stays exchanged with the
// second UV set (ours +0x60/+0x50, retail +0x50/+0x60), and the blend's XMM5 residence, the
// extents, the reciprocals and the aspect homes all follow from that swap; hoisting the two UV
// declarations above the tint, swapping them, computing the packed colour after them, taking
// the clamp pointer from the whole tint and putting the tint in an inner block all give the
// same code or a worse score (236).
// MiniMax S (11-01): the tint computed after the UV block moves the first difference earlier; no lower score.
// Earlier at 381: Frame 0x50 versus 0x60 (the tint lived in a one-caller helper's scope, so uv_b reused its
// slot); the angle is stored in a dead argument slot instead of a frame slot and the second UV set has no
// frame home of its own; the cache is kept in EBP where retail reloads it into EBX; scalar homes remain.
// Inlining the tint into the body or passing it out by pointer gives retail's 0x60 frame but scores 520,
// so the helper stays. (Earlier notes: 391 before round 56.)
// Frame 0x50 versus retail 0x60; helper tint and second UV
// overlap, angle uses an argument home, cache EBP replaces EBX, and scalar homes differ.
// Earlier tint code in the body or caller tint output recovered frame 0x60 at 520.
// Earlier 391 forms and their scores follow.
// Frame pointer 597; size optimization 990; tint input-array parameter 435;
// direct packed-tint return 435; byte flip selector 538.
// UV result by value 553; bounds input by value 687; short sequence count 441; unsigned flip mask 435; paired UV defaults 439.
// Extended tint lifetime 544; shared flip flags 435; UV extents stored first 435;
// positive-count interval scope 421 (plain); branch-owned blend defaults 508.
// Round 112: By-value input color 547; packed seed input 439; both inputs 550 (seed forms
// exact-only); redundant flip clear removed 481; bitmap arm first 508.
// Bitmap field at every use 421; frame-array field at use 442; both 442;
// immutable frame count once 479; with bitmap/frame fields/both 479/479/479;
// shared bitmap sequence clamp count 427. All restored.
// UV output-first 421; cache/definition-first tint contract 421; complete bounds snapshot 656;
// UV output-pointer return 421; reference bounds formal 421. Original helpers and body retained.
// In-place clamp pointer/reference and both forced forms 421 each; pointer clamp plus plain geometry
// table/native UV member copy 421/421; plain geometry table/native UV member copy alone 421/421. Original
// helpers restored.
// Native in-place UV pointer/reference 421/421; by-value result 494; UV-first pointer/reference 421/421;
// force-inline in-place pointer/reference/value 421/421/738. Original helper and all alias-safe captures restored.
// Definition at every use 462; shared cache value array 421; both 462; shared pivot weight 421;
// with definition/cache/both 462/421/462; complete seed products 421. All restored.
// Both scaled-pivot products/all four edge factors/both 421/421/421; real interval
// result alone/with products/with edge factors/with both 424/424/424/424; guarded frame fraction 421.
// Round 129: native four-element UV output reference with bounds/cache/color references
// in all eight combinations 421 each. Original producer contracts, tint inputs and all
// four in-place UV captures restored.
// Round 149: Split origin/extent and scalar UV outputs, output-first order, bounds references and forced
// expansion [421, 421, 421, 421, 421, 421, 421, 421]; all restored.
// Round 353: native caller tint output reference with color reference/value
// 463/579; with cache array 463/579; entry output declaration
// 463/579; entry output and cache array 463/579. Original tint restored.
// Round 146: tint code in the body with the tint at function scope gives retail's frame 0x60 and
// slots (uv_a over the tint, uv_b above) 463 (aligned 237 -> 233); with the definition's geometry
// as a 16-byte block copied whole (retail's dword moves) 517 (aligned 217); declarations reordered
// 463; flip as bool/if-else 517/517; interval count before the count test 532; shared input-array
// pointer 517. Retail spills the cache (reloaded into EBX) and keeps the count in EBX for idiv;
// ours keeps the cache in EBP. Original body restored.
// Scalar x/y/all four UV bound observations 421 each; all four with role names 421.
// Caller tint output with x/y/all four scalar observations 463 each; complete scalar
// bounds with native reference input 421. Original helper contracts and captures restored.
// Round 380: Alpha clamp before seed/before RGB/after RGB with phased RGB 463/421/421; per-channel RGB with alpha after RGB/before seed/before RGB/after red/after green 421/473/421/421/421. Original tint restored.
// Round 242: Plain indexed/pointer tint clamp traversal with the existing
// early-return scalar clamp 391 each; single-exit clamp 468 each, inline or
// forced. Keep ordinary indexed form, plain 391 at 1931/1937; frame remains.
// Round 244: Existing tint producer as cache/seed/definition member, forced 391 each, ordinary inline 722
// each; cache/definition member with caller tint output 520/520. Native tint outputs recover frame 0x60 but
// not cache/UV homes; original producer and clamp loop restored.
// Round 266: native in-place pointer/reference clamps on the retained four-channel
// loop, indexed/pointer walks and inline/forced combinations all 421. Original
// value-return clamp, indexed traversal, tint producer and UV captures restored.
// Round 166: actual fixed-size cache-value input with native color/definition/both references
// 391/391/391/391; each with direct descending channel traversal
// 399/399/399/399. Original tint contract and indexed clamp restored.
// Round 228: Complete unclamped tint returned by value/pointer/reference, caller owns
// the four clamps and packing; producer/entry lifetimes 604, 604, 520, 520, 520, 520; value return
// with a plain pointer clamp at either lifetime 604, 604. Frame 0x60 recovered, tint/UV
// ordering and cache residence still differ; original complete tint producer restored.
// Round 181: Existing tint producer packed-dword output by pointer/reference, first/last,
// with cache receiver or fixed-size input array, eight forms 391 each. Original complete producer
// and callback restored; tint/second-UV overlap and cache/interval homes remain.
// Round 400: Short/word/byte/signed-byte clamp ordinals in the plain for loop
// 502/502/502/502; the same four with a plain four-pass do loop
// 502/502/502/502. These recover entry cache EBX but leave frame 0x50.
// On that graph, caller tint reference/pointer, caller packing and native value return
// 472/472/472/509; the same with entry tint declarations
// 472/472/472/509. Original tint producer and ordinal restored.
// Round 393: Shared cache identity with tint cache pointer/reference or fixed-array/element-pointer inputs, direct update
// or existing update method: 391/391/391/391/391/391/391/391. Original cache, callback declaration and helpers restored.
// Round 253: Branch-produced primary aspects/later next reciprocals/later blend remainder/
// both later phases/aspects-reciprocals/aspects-remainder/all/constant-first aspects with
// later next spans 417/431/409/405/431/412/436/412. Original scalar phases and complete callback restored.
// Round 274: Existing flip producer with a zero result and two native bit tests,
// definition/seed reference inputs and forced expansion in eight combinations 466 each.
// Original flip producer, input contracts, tint clamp and complete callback restored.
// Round 284: Actual two-element UV output array with original packed tint, caller
// tint pointer/reference and native color return 445/519/519/592; lexical array
// homes 445/519/519/592. Exact-only grouping did not match; all native
// separate outputs and complete helper restored, including every in-place input capture.
// Round 214: lower-first conditional clamp 391; upper-first conditional/early-return/complete-result clamps 401 each.
// Real component-end walks with those four producers 494/497/497/497. Original helper and traversal restored;
// frame 0x50/0x60, overlapping tint/second UV and cache/interval homes remain.
// Round 431: Plain input-RGB/seed-RGB/both modulation loops with long then dword ordinals; input/seed
// body-stepped component cursors: 388/388/385/388/388/385/391/388. Original complete tint producer,
// clamp traversal, callback and in-place UV captures restored.
// Round 29 (fast; aligned counts in brackets, control 391 [252]): in-place clamp
// `if (c < 0) c = 0; else if (c > 1) c = 1;` in the channel loop gives retail's store-on-clamp
// sequence exactly but 418 [245] (1955 bytes); the ternary clamp in the loop 388; with the clamp, the
// definition's geometry declared as the 4-real union and copied whole (retail's dword moves) 481
// [221]; the copy alone 487; the flip as if/else 391/481 (same code); on that, the tint as a caller
// local filled by the producer gives retail's frame 0x60 but tint/uv_b slots swapped (0x60/0x50) and
// 1980 bytes, 517 [215]; renaming the tint or declaring it after the UVs moves nothing. Every form
// still holds the cache in EBP and copies it to EBX for each callee.
// Round 439: separate genuine RGB evaluation result before RGBA assembly, producer/scoped
// lifetimes and late/early assembly with scalar/native subrecord copies, 392/540/392/506 twice.
// No frame recovery; original tint helper and all in-place UV captures restored.
// Round 4 (lane 18, fast): the seed products written seed_color * tint * 2 385 (retail's load order;
// six-point gain, not kept); UV/geometry declared ahead of the tint call 391; with the seed order, the
// in-place loop clamp 418 (the tint, seed and clamp code then equals retail's, but our cache's EBP copy
// to EBX before each call shifts the alignment by one); that with the cache's address taken 418 (no
// effect); the tint written in the body (helper removed) with both 460 (frame 0x60, tint and UV slots
// swapped), the tint renamed either way 460. Retail re-reads the cache parameter from its stack slot
// into EBX where ours keeps it in EBP. Restored.
// Round 56 (lane 12, fast; plain 381 confirmed): the interval count computed before the `count > 0`
// test as `intervals = count; if (flags & 8) intervals--;` 387 (1937 bytes, retail's length and test
// order; as a ternary there 393); with the seed products written seed_color * tint * 2 381 (kept,
// 1937 bytes). On 381: the sequence pointer in a local 496; frames read before the count re-read
// 490; the in-place loop clamp 429 (1961 bytes; tint code then retail's but the colour parameter
// in ESI for EBP); the definition's geometry as an s_draw_sprite_uv copied whole 478 (1914 bytes,
// retail's dword moves); both 480 (1938 bytes, aligned 245 -> 214: one byte long); the tint owned by
// the caller 532 (1994 bytes, frame 0x60); the flip as `flip = 0; if (...) flip = 1;` 454.
// Round 30 (lane 14, fast; kept: the cache update through the existing 0x173c10 method of the cache's real
// type, inlined to the same code, 381): the cache's value array in a named local 381 (copy-propagated); the
// geometry copied before the bounds 381; the definition declared before blend 381; geometry declared after
// the UV sets 381. Diagnostic (reverted at once): the cache read through a volatile cast at its last use 490
// (1949 bytes) puts the cache in EBX from entry and the angle in a frame slot as in retail, so the
// remaining root is our cache being allocated to EBP for the whole function.
// Round 457: complete four-channel modulation then/fused with clamps, input-first/tint-first
// products, original/looped seed RGB: 426/430/426/430/426/428/426/428.
// Frame remains 0x50; original complete tint producer and all UV captures restored.
// Round 262: Library copies of definition/selected-frame bounds/both 381/381/381;
// geometry-array copy alone/with definition/with selected/with both/with encoded UV copy 478 each.
// The original arrays and all input-before-store UV captures are restored; frame/cache homes remain.
// Round 45 (lane 13, fast): retail reloads both the cache (EBX, which also holds the idiv frame count) and
// the context (ESI) from their stack slots; ours enregisters the cache in EBP and the context in EBX.
// Re-read frame count in its own local 381 (same code); cache's last use read through its own address 381;
// tint code in the body with the in-place clamp loop 476 (2012 bytes, frame 0x60, cache still EBP); the
// four clamps as explicit statements 476; with a short clamp counter 483.
// Round 23 (lane 18; fast = plain): the sequence count read at each use instead of re-assigning `count`
// 465 (1928 bytes, aligned 245 -> 203: the cache in EBX from entry, the colour in EBP, as retail); with the
// tint clamped in place in the channel loop 415; with the geometry typed as the 4-real rectangle and copied
// whole 304; with the tint code in the body 331 (frame 0x60); with the blend remainder after the first aspect
// test 294; with the frames also read at each use 261; with the UV bounds used directly instead of
// references 252 (kept). On that path: count re-assigned again 491; pivot products weight-first 294;
// pivot before the bounds copies 297; extents/reciprocals in a/b pairs 260; reciprocal sums inline in
// x/y 279; remainder at each use 368; remainder before the flip/before the aspect defaults 277/299;
// tint in a nested block 276 (tint +0x50 as retail, but it shares uv_b's home: frame 0x50), with the
// UV sets declared before the block 276. Tint/uv_b home order unchanged by declaration order, names
// (atint/utint), the four clamps written out, or extra uv_b uses (diagnostic). On 252: the count read
// from the sequence at every use, no count local (one form throughout) 245 (kept: retail's `dec ebp`);
// the tint back in its one-caller helper with every other change 276 (frame 0x50). On 245: the b
// reciprocals before the a ones 239 (not kept: an order of independent values, 6 under the keep bar); with by before bx 242; reciprocals by, bx, ax, ay 239;
// bx, by, ay, ax 239; b extents and reciprocals before the a pair 239; by before bx alone 247; the
// flip before the extents 249; the flip after the width 325.
// Round 25 (lane 18, fast): extents/reciprocals in retail's schedule order (ax, ay, by, inv_ax, inv_ay, bx,
// inv_bx, inv_by) 251; aspect defaults before the flip 319; reciprocal sums before the second aspect test 309;
// blend after the frame bounds copies 275; the flip helper reading the bounds into locals 245 (same code);
// its ternaries as if/else 245 (same code). Diagnostics: blend through a pointer local 245 (copy-propagated);
// the 0x10000 arms swapped 410. Home order follows reference weight measured on the final code: with uv_b's
// flip removed uv_b has 18 references to the tint's 21 and takes the top slot (retail's order); unconditional
// tint updates cut the tint to 13 and it stays on top. Retail has the same 30/21 counts as ours but puts the
// tint lower, so its weights are frequency-scaled: look for the branch shape that lowers uv_b's weight.
// Round 284: native three-channel RGB clamp with separate alpha, phased/fused
// input modulation, ascending/descending ordinals and alpha last/first:
// 383/381/391/389/399/399/506/498. Original complete producer and callback restored;
// frame 0x50 versus 0x60 and tint/UV overlap remain.
// Round 302: Common UV encoding after geometry, original/outer pivot and scalar/copied second defaults
// 697/697/690/690; late blend on those four graphs 734/734/726/726. Frame shrinks and tint/UV homes
// regress; complete original body and alias-safe helper restored.
// AC460: Existing UV producer with two-use positive axis decisions in bool/byte/short/long
// 367/367/402/367; complete complementary decisions 397/397/421/398. Original conditional
// expressions, helper and all in-place input captures restored; tint/UV homes remain.
// Round 308: Native bitmap-default copy primary to secondary with original/input-RGB/seed-RGB/both loops
// 347/347/350/347; reverse default ownership 348/348/351/348. The caller-owned tint frame
// remains 0x60, but the swapped homes persist and the copy grows code to 1952 bytes.
// Original separate default stores, RGB statements and helper input captures restored.
// Round 350: Native-frame blend initialization after packing/in geometry arms/after cache
// update/explicit frame outcomes 493/493/271/261; merged positive-sequence gate 483/483/375/394. Tint/UV
// homes and blend residence remain.
// Round 352: Pointer lower/upper/independent/continue clamp walks 522/527/522/522; indexed
// upper/independent/continue/descending bounds 265/245/245/263. Frame homes and blend residence
// remain; complete original callback and alias-safe UV helper restored.
// Round 248: On the native frame/cache graph, branch-produced primary aspects, paired named secondary
// ratios and cross-phase inverse blend, seven subsets plus inverted primary: 239/245/239/530/529/530/529/529.
// Six-point primary gain below gate; original tint/UV bodies and arithmetic restored.
// Combining the new primary-aspect result pair with secondary-first/vertical-first/primary-vertical
// reciprocal orders and secondary spans first: 245/245/248/246; all complete bodies restored.
// P252: Existing UV producer with one joint origin/extent test per axis, y-first then x-first,
// origin-first/extent-first and flipped/unflipped arm order: 410/411/439/439/420/420/447/447. Original
// helper, complete callback and all in-place input-before-store captures restored; tint/second-UV homes and
// blend residence remain.
// Round 515: Native first/next/both selected-frame values at index production or first use
// 441/432/475/420/419/473; both with branch-produced primary aspects 474/466.
// Tint/second-UV homes remain; complete native inputs, scalar phases and body restored.
// Round 354: Complete seed/input RGB product phases and outer seed workspace subsets
// 1..7, then early native UV declarations: 245/245/245/245/245/245/245/245. Original product groups,
// component stores, workspaces, tint/UV storage and alias-safe UV captures restored.
// Round 311: Returned seed-color unpack output with scalar/seed-loop/input-loop/both RGB consumers,
// branch-local buffer 431/431/431/431, entry buffer 483/483/483/483. Original producer use and full body
// restored; tint/secondary-UV home order and blend residency remain.
// Round 357: Direct-bounds mode secondary-UV producer then primary copy, four geometry-copy phases with union/member copies 263/253/304/308/263/253/304/308. Original body and input-before-store UV encoder restored; tint/secondary-UV homes remain.
// Round 288: Complete previous/next/both interpolation-product stages, both coordinates
// at every interpolation site 280/232/245; with branch-produced primary aspects
// 279/262/239; complete stages with secondary-first reciprocals, original/branch
// primary 239/245. Next-term stages kept at plain 232; other bodies restored.
// Round 367: Conditional sequence result with branch-owned UV defaults, pivot default/result, native secondary
// default copy and reused frame-count subsets: 429/420/436/416/407/426/446/438. These lose
// the recovered frame; original complete callback and all alias-safe UV input captures restored.
// AC477: Complete byte/signed-byte/short/word flip domain through producer, reused selector
// and all UV encoders, original then zero-first result: 247/247/253/259/403/403/411/416.
// Original selector domains, UV input captures and body restored; tint/second-UV homes remain.
// Round 526: Native evaluated RGB apart from alpha/tint, publication before/after input
// modulation, field/library transfers and original/entry seed workspace:
// 236/236/486/486/236/236/498/498. Original tint/UV body and includes restored;
// tint/secondary-UV homes and blend/reciprocal residence remain.
// Round 325: Existing UV producer consumes its real four-element input array, pointer/reference
// input and output with original/scalar pivot storage: 232/232/232/232/325/325/325/325. All alias-safe captures,
// original contracts, workspaces and complete body restored; tint/secondary-UV homes remain.
// Round 318: Direct-bounds UV outputs encoded independently by the existing encoder,
// geometry/output phases GAB/AGB/ABG/GBA, then a shared two-use selector;
// 645/645/645/645/575/575/573/575. Original whole body and helpers restored.
// Round 331: Native primary-aspect/secondary-span/reciprocal stage boundaries, two early-selector
// graphs, two axis-paired graphs and late selector: 241/321/260/296/302/236/295/238. Original
// interpolation stages, scalar lifetimes, selector and complete body restored; tint/second-UV and blend
// homes remain.
// F335: complete lower/upper clamp passes, lower-first then upper-first, both direct ordinal
// directions in each pass: 267/269/271/271/272/272/272/273. Tint/secondary-UV homes persist;
// original component-owned clamp, alias-safe UV producer and complete callback restored.
// Round 376: Optional-bitmap admission truth in bool/byte/short/long,
// valid-first and invalid-first, 232/232/232/232/232/232/232/232.
// Exact-only one-use admission forms restored; tint/secondary-UV homes and blend residence remain.
// Round 378: Shared pivot complement on the retained next-product graph, inner/outer storage,
// recomputed aspect result and before/after first next-product producer: 232/232/306/306/232/232/306/306.
// Original separate complement producers, all arithmetic, native buffers and body restored;
// tint/secondary-UV homes and blend residence remain.
// P278: Geometry/primary/secondary UV declarations at entry, seven subsets, then all with seed-
// colour scratch at entry: 232 each. All initializers and observations fixed; original
// declarations/body restored. Tint/secondary UV and blend homes remain.
// P287: Four-channel input captured once for both tests, lower/upper conditional stores,
// scalar result and conditional result: forward 232/252/428/433, reverse 250/253/440/445.
// Original loop, body and all alias-safe UV captures restored; tint/UV homes and blend remain.
// B381: complete geometry-source decision, explicit/bitmap bounds in bool/byte/short/long,
// 293/293/293/232/419/419/419/419. NOT kept: one-use decisions require exact matching. Original tint/secondary-UV
// homes and blend/reciprocal residence remain.
// Round 77 (m3 lane 9, fast = plain): the pivot interpolation as one expression per axis,
// `(1.0f-blend)*first->pivot.x+blend*next->pivot.x`, 223 (kept: retail's first-term-first schedule; with named
// first-term locals 223); local renames (first_uv/next_uv, color_tint, zuv_b) 232 each; on 223: extents and
// reciprocals in a/b pairs 223, in retail's schedule order (ax, ay, by, inv_ax, inv_ay, bx, inv_bx, inv_by) 221;
// the aspect next-product locals inlined 223 (same code), the inverse ones inlined 245; the four tint clamps as
// explicit statements 223 (same code). Left: tint/uv_b homes swapped (0x60/0x50), blend not held in XMM5
// after the sequence block (retail cross-jumps the context/blend reload), reciprocal schedule.
// DH318: Existing shared seed RGB scale, product output in tint/seed scratch, plain seed/input traversals:
// 244/244/253/253/245/245/245/245. Tint/secondary UV homes and blend residence remain. All restored.
// Round 79 (m3 lane 9, fast): both aspect defaults as ternaries on `ax>ay` 264; blend as a ternary on the
// 0x20 flag (0 otherwise) 261; the aspect updates as `remaining*aspect + next` 223 (same code); blend declared
// after the definition local 223; the tint declared at entry 223 (same code each).
// Round 51 (m3 lane 15): pivot defaults as two stores 223; UV sets declared uv_b, uv_a 223; width after the two
// UV encodes 419; quarter-turn added right after the angle 482; size in a named local before the call 233.
// M3 lane 6 r142 (fast; retail homes tint at 0x50 and uv_b at 0x60, ours the other way round): geometry declared at
// entry 223 (same code); uv_b's default bounds copied from uv_a 347 (1956 bytes); the tint and its clamps in their own
// block ending at the pack 249 (1906 bytes); `uv_b,uv_a` declaration order 223 (same code); the flip read before the
// extents 250 (1949 bytes). Original body restored.
// Lane 14 r60 (fast; "no-home" = aligned count ignoring stack offsets, kept form 123, 223 form 125): kept 211: aspect defaults
// before the flip. Not kept, an order of independent values 6 under the bar: retail's extent order (ax, ay, by, inv_ax, inv_ay,
// bx, inv_bx, inv_by) 208, the width before the inverse products 208, both 205 (plain, 1950 bytes, no-home 117); on 205 the final
// geometry with y before x, x1/y1 before x0/y0, both, y before x in each pair, subtractions then scaling, grouped products
// 205 each. Retail keeps the blend in XMM5 from the join on and stores ax, ay, bx and by once each at their definitions; ours
// keeps both a-extents in registers and stores by twice (the named local and the UV helper's shared expression are two values).
// A named copy of the blend at the join 223 (same code). On 211: complement before the flip 291, before the extents 329;
// inverse sums next-term first in one expression 319 (frame 0x64, no-home 90), first-term first 321 (0x64, 96). Sweep with
// defaults first: ay ax by bx 210; by bx ay ax 211; b pair first 211; ax iax ay iay bx ibx by iby 289 (0x5c); ax ay iay iax by
// bx iby ibx 213; by ax ay 208; ax by ay 208; iay before iax 211; iby before ibx 211; all extents first 211; ay ax iax iay by bx
// 211. With the one-expression sums: nine orders 296-365 (frame 0x64 or 0x68, no-home 85-107). All four extents written at
// each use (a match-only form): named products 304 (0x58), one-expression sums 302 (0x5c, 1917 bytes, no-home 76: the closest
// structure), with iay first 302 (no-home 80, a homes and order as retail, only ay's slot and the blend register missing);
// other reciprocal orders 302 (83-94). Named subsets on that: ay 296, ax+ay 297, ax 302, bx+by 311 (0x64), ay+by 298 (0x64),
// ax+ay with iax first 291; none reaches frame 0x60.
// DL256: Existing one-object in-place UV encoder, encoder/selector expansion pairs inline/inline,
// forced/inline, inline/forced, forced/forced, ordinary/inline, inline/ordinary, ordinary/forced,
// forced/ordinary: 223 each. Complete alpha/RGB record and whole-color/RGB union views: 223/223.
// All remain 1948/1937 bytes, first +0x5b, with every control unchanged. Original restored;
// NOT kept: unmatched encoder or color representation; tint/UV homes and XMM5 blend lifetime remain.
// F361: Existing UV encoder as native output member, pointer/reference bounds,
// ordinary/forced inline and long/dword selector: 223/223/223/223/223/223/223/223.
// NOT kept: tint/secondary-UV homes and blend residence unchanged; original encoder restored.
// B409: Wrapped first/next/both frame indices, successor ordinal and its complete
// combinations, adjacent modulo stage and updated successor index: 223 each.
// Original index producers/body restored; tint/secondary-UV and blend homes remain.
// B411: Persistent input/clamp-cursor/selected-frame lexical identities, subsets 1-7,
// then complete scalar-stage identities: 223 each. Original observations, expressions,
// workspaces and body restored; tint/secondary-UV homes and blend residence remain.
// M348: Native two-axis UV defaults primary/secondary/separate/joint loops
// 407/410/434/408; library zero defaults primary/secondary/both/upper loop
// 351/351/376/432. NOT kept: no byte match; original body and alias-safe captures restored.
// M350: Uninitialized angle/complement/width entry declarations, seven subsets,
// then paired primary-aspect storage at entry: 223 each. All producers stay at
// their original phases; complete body restored, tint/secondary-UV homes remain.
// DH331: Oriented start/end UV endpoints before subtraction, conditional then paired branches,
// bounds pointer/reference and four-real pointer/reference: 473/473/473/473/541/541/541/541.
// NOT kept: smaller frames and regressed extents/scores; original alias-safe encoder and callback restored.
// N348: Native box storage for geometry/primary/secondary UV, seven subsets, then
// native box encoder output, on the current frame/plain-pivot graph: 223 each.
// Original union views and all input-before-store captures restored; tint/UV homes remain.
// N350: Common geometry-arm flip result after packing, long/dword/word/byte carriers,
// original/reference producer inputs: 458/458/437/445/458/458/437/445.
// NOT kept: frame/extent and exchanged tint/UV homes regress; original body restored.
// F378: Interpolation-admission outcome at frame-fraction production, positive then complementary
// bool/byte/short/long: 284/284/284/284/285/285/285/211. NOT kept: one-use decisions remain unmatched,
// exchanged tint/secondary-UV homes persist and other forms grow; original complete body restored.
// B429: Native lower-clamp comparison results in bool/byte/short/long 211/211/211/211;
// paired lower/upper outcomes 579/579/636/628. NOT kept: one-use result forms
// do not match; original tint/UV homes, complete loop and alias-safe captures restored.
// M375: Native secondary encoded-UV library transfers, object/array/bounds/encoder-result,
// memcpy then memmove: 211/211/211/211/366/366/366/366. NOT kept: no byte match or native home repair;
// original whole callback, encoder result contract and all alias-safe input captures restored.
// DL264: Matched scalar/RGB property producer inline/forced contracts,
// independent and complete pairs: 211/211/211/1257/742/1257/742/1541.
// NOT kept: no eligible native repair with preserved shared users; original executable source restored.
// AC533: Requested-sequence clamp result, conditional/complete/lower-default/upper-default,
// original/positive-produced count: 218/218/288/218/223/223/288/223. NOT kept: all regress;
// tint/secondary-UV homes and blend join remain. Original whole body restored.
// F383: Complete immutable spans/inverses/geometry, seven subsets, then immutable
// alias-safe UV input captures: eight at 211. Original scalars and encoder restored;
// tint/secondary-UV homes and persistent blend residence remain.
// B438: Reverse body-stepped channel cursor, lower/upper/independent/continue clamps,
// direct then two-test scalar inputs: 229/232/229/229/229/232/451/229. NOT kept: no exact
// traversal/lifetime gain; original full body and alias-safe UV captures restored.
// M385: Complete pivot/aspect/inverse destination accumulators, seven subsets then
// early uninitialized inverse storage: 211/211/319/211/319/319/319/319. NOT kept: inverse
// accumulation grows frame to 0x64; original body and every alias-safe UV capture restored.
// M387: Actual forward end cursors !=/< with lower/upper/conditional/sequential clamps:
// 515/520/512/515/515/520/644/515. NOT kept: frame remains 0x60 but entry and extents regress;
// original callback and alias-safe encoder restored, tint/secondary-UV and blend homes remain.
// N373: Actual tint/input RGB subobject pointer/reference ownership, eight ordered forms
// 211/211/211/211/211/211/211/211. NOT kept: no equality; original complete color object, callback,
// clamp, input observations and alias-safe UV captures restored. Tint/secondary-UV homes remain.
// DH374: Paired primary/secondary aspect outcomes, positive/complementary bool/byte/short/long:
// 211/211/211/211/211/211/211/211 (fast control 211; full 211).
// NOT kept: no byte match or eligible gain. Original direct aspect decisions, all interpolation trees, alias-
// safe UV captures and complete callback restored; tint/secondary-UV homes and persistent blend remain.
// AC560: Selected-sequence availability at its original guard, positive/complementary
// bool/byte/short/long: 211/211/211/211/211/211/211/211. NOT kept: one-use outcomes do not match; original
// callback and alias-safe UV captures restored. Tint/secondary-UV and blend homes remain.
// Round 91 (m3 lane 9, fast): the tint renamed to sort first or last 211/211, the second UV set renamed 211 and
// the tint declared after the UV sets at the top 211 (same code: the tint/second-UV homes ignore names and
// declaration order; diagnostics adding tint or second-UV reads do not move them either); the first set's
// extents and reciprocals before the second's 211, the second set's y extent first 211 (same code);
// remaining_blend named before the first aspect test 258 (1972 bytes); the inverse sums as one expression each
// 319 (1957 bytes, the frame moves). Retail keeps the blend in XMM5 from the frame block through a join that
// reloads it on the other paths (+0x46f..+0x47c), so its aspect block holds ax/ay/bx/by/1/bx/1/by in registers.
// T71: Fixed-four indexed tint clamp, int/long/unsigned-int/dword, plain while then direct
// do/while: 211/211/211/211/512/512/512/512. NOT kept: ties/regressions; original clamp, full
// body and alias-safe encoder captures restored.
// P393: Seed-color admission at the original tint boundary, positive/complementary
// bool/byte/short/long: 211/211/211/211/211/211/211/211. NOT kept: unmatched one-use outcomes; tint/secondary-UV
// homes and continuous blend residence remain. Original body and all alias-safe UV captures restored.
// M3 lane 16 r97 (fast; body restored; retail's homes from low to high are geometry, tint, second UV, first UV, ours
// geometry, second UV, tint, first UV, with the same 21 and 30 references): the clamp loop indexing the tint directly
// with no component pointer local 211 (same code); the two UV sets as one two-element array with the first set at
// index 1 249 (the array takes the top 0x20 and the tint drops to +0x40, under geometry), with geometry declared
// ahead of the tint 249 (same homes), with natural indices 223 (tint still +0x40); the pivot as two reals 313 (1978
// bytes); the seed colour scratch declared at function scope 211 (same code); the direct-bounds arm encoding the
// second set itself instead of copying the first 649 (2100 bytes).
// M3 lane 11 r81 (fast): both UV encodings right after the flip 431 (1925 bytes); the flip before the extents 250 (1949);
// the 0x2000 quarter-turn right after the angle read 483 (1939); the bitmap read from the context at each use 211 (same code);
// pivot products written pivot-first 211 (same code).
// M3 lane 8 r150 (fast; aligned count 123, register-masked 59 on this body): retail's blend join is three entries, the
// arm with the blend already in XMM5 jumping past both reloads, the count test's exit reloading the context and the blend,
// the other exits the blend alone; ours keeps the blend in XMM2 inside the arm only. Tried: the sequence-frame block as a
// forceinline helper writing the bounds, pivot and blend through pointers 220 (blend still XMM2 in the arm), returning
// the blend by value 330, as a plain inline (left out of line) 644/637; the block after the join (extents to geometry) as a
// forceinline helper taking the blend by value 211 (same code: the parameter is copy-propagated), with the flip passed in
// 250, with the pivot by value 349; both helpers 220; a two-use `&frames[index % count]` getter 439 (first difference
// moves to +0xb); the unit default bounds through a four-real setter 211 (same code).
// m3 lane 3 r227 (fast; re-measures of round 431's loop forms on the current base): the colour modulation as a plain three-pass
// loop 220, the seed modulation as one 214, both 213, the four clamps written out 211 (same code); the tint/second-UV homes stay
// swapped in every form, so loop nesting does not set their order (and per round 91 neither do added reads, names or declaration
// order). Diagnostics (wrong values, reverted): the direct-bounds arm without `uv_b = uv_a` 365, homes unchanged; an early
// uv_b store above the tint is dropped as dead (211, same code). New combinations (fast): the tint in its own block ending at
// the pack (re-measured 236, 1906 bytes, the tint then shares the second set's home, frame 0x50) with the colour modulation
// loop 245, with the seed modulation loop 242, with both 240; the seed loop with the clamps written out 214; both modulation
// loops with the clamps written out 214. The home order rule is still unknown.
// Round 181 (m3 lane 4, fast): the aspect defaults declared with the pivot at the top of the bitmap arm 405 (1964 bytes);
// the next-aspect products written blend first, `blend*(by*inv_bx)` 211 (same code); the inverse next terms blend first 211
// (same code); the second UV set flipped before the first 271; the pivot first in the centre products 211 (same code).
// Round 104 (m3 lane 9, fast): both aspect blends as one expression each 211; the pivot weight `1.0f - blend` named
// once 211; `ax <= ay` with the arms swapped 211 (all the same code); `bx <= by` with the arms swapped 211 (aligned
// 151 -> 159); the width product after the two flip calls 423 (1932 bytes). The compile-only harness does not
// reproduce this body (normalized difference 394 against our own build), so the blend's XMM5 residence could not be
// screened there.
// Round 73 (m3 lane 15, fast = plain; register-masked aligned 59, all in the post-join aspect block plus the tint/second-UV
// homes): geometry as `real geometry[4]` with element copies, or kept as the four-real overlay copied element by element, 400
// (1974 bytes, identical code); the complement and both inverse sums computed ahead of the first aspect test 305 (1963 bytes,
// the frame moves); the tint clamp through the existing PIN_126660 macro 429 (1933 bytes). Re-measured: the complement written
// at each use 338 (1972 bytes). Diagnostics (wrong code, reverted): the tint keeps the top 16-byte slot with the colour
// modulation removed, the clamps removed, both removed, the seed block removed, the tint as a four-real overlay, uv_b's copy
// removed, a second flip of uv_b, uv_b's defaults removed, and the final call's UV arguments swapped (211, homes unchanged),
// so neither reference density, type nor argument order sets these homes; the compile-only frame probe gives a third order
// (geometry on top), so it cannot screen them.
// Round 73, continued (diagnostics): uv_b's flip call under an always-true test 245, under two nested ones 405 (1958 bytes);
// the tint stays on top in both, so lowering uv_b's branch frequency does not move the homes either.
// M3 lane 1 r89 (fast; control 211, aligned 89, masked 34; retail's frame from its final call, low to high: geometry
// +0x20, first UV set +0x30, tint +0x40, second UV set +0x50; ours exchanges the tint and the second set): the second set's
// bitmap-arm defaults as `uv_b=uv_a;` after the first set's four stores 350 (1959 B); the aspect values set in both arms of
// the `ax>ay` test instead of 1.0f defaults 253 (1953 B); the width product written at its two uses 423 (1932 B); the two
// frame bounds copied before the blend 244 (1950 B); the alpha product moved down to the colour modulation 491 (1950 B);
// one flip local for both arms, re-measured (round's "shared flip flags") 458 (1921 B).
// M3 lane 12 r110 (fast; aligned/masked = line diff after address masking / with registers and stack offsets also
// masked, control 211 at 136/34): diagnostics (helper bodies written out, not keepable forms): the flip written in the
// body at both sites (no sprite_flip) 211; the UV encoding written in the body at all three sites (no function_x0ec080)
// 211; both 211 (same code each: the two helpers are not the cause). Forms: the unit bounds as one initialized box copied to both sets 507 (1974 bytes); the aspect pair set in both arms
// of the `ax>ay` test 253 (masked 40); the frame index clamped through a PIN macro 211 (same code); the sequence count
// defaulted by `if (count < 1) count = 1;` 427 (1933 bytes); the frame position as `(real)intervals * f()` 211 (same
// code). Diagnostics (reverted): blend passed as 0 to the last call 196 (masked 35, the
// blend still leaves XMM5); blend zeroed per arm instead of at entry 469.
// M3 lane 13 r82 (fast; control 211, aligned 213, register-masked 34): only the second set's reciprocals moved after the
// first aspect test 239 (1957 bytes, masked 27: the join after the frame block becomes retail's, context reloaded in the arm
// and the blend reloaded into XMM5 on the other paths, so the blend's XMM5 home is register pressure in the extents block);
// the second set's extents and reciprocals after the first aspect test 278 (frame 0x5c, masked 28), between the flip and that
// test 260, after `remaining_blend` 278; retail's order ax, ay, by, inv_ax, inv_ay, bx with the late second reciprocals 317;
// the aspect pair as a `vector2f` 205 (1942 bytes, aligned 217: positional only, the packed colour's home moves), with the
// inverse sums as a `vector2f` too 205 (same code), with the late second reciprocals 231; the extents as two `vector2f` 219,
// with the aspect pair too 210 (aligned 223); the reciprocals as two `vector2f` 216; the geometry corners as two `point2f`
// 221; the centre as a `point2f` 211 (same code).
// M3 lane 11 r95 (fast; base 211, masked aligned 59: the tint code equals retail's but for its slot, +0x60 here against
// +0x50, so most of the count is the tint/second-UV home swap plus the aspect block). Re-measured: remaining_blend named
// before the first aspect test 258 (1972 B). New: both UV encodes moved up to just after the flip, ahead of the aspect
// tests, 431 (1925 B); the clamp's upper test as its own `if` instead of `else if` 211 (same code); the bitmap arm's four
// geometry stores (the existing UV encoder's body with flip 0) as `box2f corners = {x0, x1, y0, y1};
// function_x0ec080(&corners, 0, geometry.values);` 512 (1899 B: the extra box changes the frame); the second set encoded
// before the first 271. Body restored.
// Round 81 (m3 lane 15, fast = plain 194; aligned 133, stack-masked 86, stack-and-register-masked 35). Diagnostics on the
// tint/second-UV homes (wrong code, reverted): this build orders the two by stack references (reads, writes and address
// uses, each counted once in the final code; more references sit lower, a tie puts the tint lower): the tint has 20, the
// second set 29. The second set's flip cut to x only (9 fewer) or removed (12 fewer) gives retail's order; x and dx, x
// and y, or x, y and dx (6, 6, 3 fewer) do not; 4 or 8 extra tint reads after the pack do not, 16 do (the tint then
// sits between the two UV sets, as in retail). Retail's final code has the same 20 and 29, so its order does not come
// from these counts. The tint and the second set as the two members of one 32-byte record: retail's homes (tint +0x50,
// second set +0x60, first difference +0x18c) but 353, because stores into the record then reload the definition flags,
// which retail keeps in EDX across the second set's stores (so the two are separate objects there). No effect (194,
// same code): 1 to 7 unused initialised locals (constant, pointer-valued or float; this function has no operand-order
// phase), the second set or the tint declared as a plain four-real array, library copies for the second set's two bounds
// copies; the seed scratch widened to 16 bytes 195. The UV encode taking the named extents for its unflipped arms (one
// value per extent, as retail stores each once) 304 at 1917 bytes, frame 0x58: the code of round 60's extents-at-each-use
// form. Left as before: the home swap, and retail spilling ax, ay, the first set's two reciprocals and the aspect pair
// while the blend, the second set's reciprocals and its extents hold registers (ours keeps ax and ay in registers down
// to the flip and stores bx and by twice).
// T102: Native callback seed/color/context reference formals, seven subsets,
// then all with cache reference: 194/194/194/194/194/194/194/194. NOT kept: no byte
// equality or tint/secondary-UV/blend-home repair; complete callback and sole
// external declaration restored, including all alias-safe encoder captures.
// m3 lane 7 r159 (fast; a /GL harness of this file alone with its callees' objects reproduces the listing): the slot
// swapped with the tint is `geometry` (the last argument array: retail tint +0x50, geometry +0x60; ours the reverse),
// uv_a +0x30 and uv_b +0x40 agree. With stack offsets masked 121 aligned lines remain, all in the aspect/reciprocal
// block (retail keeps the blend in XMM5). In the harness the tint stays at +0x60 whatever the clamp spelling (indexed
// through `components`, `(&tint.alpha)[channel]`, four statements), six extra tint reads, geometry written through a
// pointer or typed as a 4-real array or a one-member wrapper, tint or geometry declared first, or the tint renamed
// a_tint/z_tint. Measured: geometry copied as its bounds member 194 (same code); the 0x10000 branch's copy as
// `uv_b.bounds=uv_a.bounds` 194 (same code); `remaining_blend` computed before the extents 331 (1980 B); the tint scoped
// to a block ending at the pack call 234 (frame 0x50); the frame blend as one ternary 276 (1953 B).
// P475: Standard direct-geometry/encoded-secondary range transfers and ordered
// full channel clamp, seven subsets then backward/upper-first: 461/344/494/451/491/506/482/482.
// NOT kept: no exact native tint/UV/blend storage repair; original complete body restored.
// @retail 0x224550
void __stdcall function_224550(s_draw_sprite_seed const *seed,s_particle_value_cache *values,
	point3f *position,point3f *direction,real scale,color4f const *color,s_draw_sprite_context *context)
{
	real blend=0.0f;
	s_draw_sprite_definition const *definition=context->definition;
	values->function_173c10(definition->field_98&0xf80f);
	real angle=function_246cd0(&definition->angle,values->values)*6.2831854820251465f;
	color4f tint;
	function_246d80(&definition->color,values->values,(color3f *)&tint.red);
	tint.alpha=function_246cd0(&definition->alpha,values->values)*color->alpha;
	if(definition->flags&0x180)
	{
		color3f seed_color;
		unpack_color3f(seed->color,&seed_color);
		tint.red=seed_color.red*tint.red*2.0f;
		tint.green=seed_color.green*tint.green*2.0f;
		tint.blue=seed_color.blue*tint.blue*2.0f;
	}
	tint.red=color->red*tint.red;
	tint.green=color->green*tint.green;
	tint.blue=color->blue*tint.blue;
	real *components=&tint.alpha;
	for(long channel=0;channel<4;++channel)
	{
		if(components[channel]<0.0f) components[channel]=0.0f;
		else if(components[channel]>1.0f) components[channel]=1.0f;
	}
	dword packed=pack_color4f(&tint);
	s_draw_sprite_uv geometry;
	s_draw_sprite_uv uv_a,uv_b;
	if(definition->flags&0x10000)
	{
		uv_a.bounds=definition->bounds;
		geometry=definition->geometry;
		function_x0ec080(&uv_a.bounds,sprite_flip(definition,seed),uv_a.values);
		uv_b=uv_a;
	}
	else
	{
		s_draw_sprite_bitmap const *bitmap=context->bitmap;
		uv_a.bounds.x0=0.0f; uv_a.bounds.x1=1.0f; uv_a.bounds.y0=0.0f; uv_a.bounds.y1=1.0f;
		uv_b.bounds.x0=0.0f; uv_b.bounds.x1=1.0f; uv_b.bounds.y0=0.0f; uv_b.bounds.y1=1.0f;
		point2f pivot={0.5f,0.5f};
		if(bitmap && bitmap->type==3)
		{
			if(definition->flags&0x10)
			{
				long count=definition->sequence_count<1?1:definition->sequence_count;
				long index=seed->field_16%count+definition->first_sequence;
				if(index<0) index=0; else if(index>bitmap->count-1) index=bitmap->count-1;
				context->sequence=&bitmap->sequences[index];
			}
			if(context->sequence)
			{
				long intervals=context->sequence->count;
				if(definition->flags&8)
					intervals--;
				if(context->sequence->count>0)
				{
					real frame=function_246cd0(&definition->frame,values->values)*(real)intervals;
					long index=function_016ae0(frame);
					s_draw_sprite_frame const *first=&context->sequence->frames[index%context->sequence->count];
					s_draw_sprite_frame const *next=&context->sequence->frames[(index+1)%context->sequence->count];
					if(!(definition->flags&0x20)) blend=frame-(real)index;
					uv_a.bounds=first->bounds; uv_b.bounds=next->bounds;
					pivot.x=(1.0f-blend)*first->pivot.x+blend*next->pivot.x;
					pivot.y=(1.0f-blend)*first->pivot.y+blend*next->pivot.y;
				}
			}
		}
		real ax=uv_a.bounds.x1-uv_a.bounds.x0,ay=uv_a.bounds.y1-uv_a.bounds.y0;
		real bx=uv_b.bounds.x1-uv_b.bounds.x0,by=uv_b.bounds.y1-uv_b.bounds.y0;
		real inv_ax=1.0f/ax,inv_ay=1.0f/ay,inv_bx=1.0f/bx,inv_by=1.0f/by;
		real aspect_x=1.0f,aspect_y=1.0f;
		long flip=sprite_flip(definition,seed);
		if(ax>ay) aspect_y=ay*inv_ax; else aspect_x=ax*inv_ay;
		real remaining_blend=1.0f-blend;
		if(bx>by)
		{
			real aspect_y_next=by*inv_bx*blend;
			aspect_y=aspect_y_next+remaining_blend*aspect_y;
		}
		else
		{
			real aspect_x_next=bx*inv_by*blend;
			aspect_x=aspect_x_next+remaining_blend*aspect_x;
		}
		real inverse_x_next=inv_bx*blend;
		real inv_x=remaining_blend*inv_ax+inverse_x_next;
		real width=context->width*aspect_x;
		real inverse_y_next=inv_by*blend;
		real inv_y=remaining_blend*inv_ay+inverse_y_next;
		function_x0ec080(&uv_a.bounds,flip,uv_a.values); function_x0ec080(&uv_b.bounds,flip,uv_b.values);
		real x=inv_x*pivot.x*2.0f-1.0f,y=inv_y*pivot.y*2.0f-1.0f;
		real x0=(-1.0f-x)*width, y0=(-1.0f-y)*aspect_y;
		real x1=(1.0f-x)*width, y1=(1.0f-y)*aspect_y;
		geometry.values[0]=x0; geometry.values[1]=y0; geometry.values[2]=x1-x0; geometry.values[3]=y1-y0;
	}
	if(definition->flags&0x2000) angle+=1.5707963705062866f;
	function_23ed30(position,direction,function_246cd0(&definition->size,values->values)*scale,
		angle,packed,blend,uv_a.values,uv_b.values,geometry.values);
}
