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

// score 245 (was 245): frame 0x60 and the cache/colour/count registers are now retail's; the tint and the
// second UV set have swapped homes (ours +0x60/+0x50, retail +0x50/+0x60), and retail keeps the blend in XMM5
// through the aspect block (ours reloads it), so the extents, reciprocals and aspect homes differ.
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
					pivot.x=first->pivot.x*(1.0f-blend)+next->pivot.x*blend;
					pivot.y=first->pivot.y*(1.0f-blend)+next->pivot.y*blend;
				}
			}
		}
		real ax=uv_a.bounds.x1-uv_a.bounds.x0,ay=uv_a.bounds.y1-uv_a.bounds.y0;
		real bx=uv_b.bounds.x1-uv_b.bounds.x0,by=uv_b.bounds.y1-uv_b.bounds.y0;
		real inv_ax=1.0f/ax,inv_ay=1.0f/ay,inv_bx=1.0f/bx,inv_by=1.0f/by;
		long flip=sprite_flip(definition,seed);
		real aspect_x=1.0f,aspect_y=1.0f;
		if(ax>ay) aspect_y=ay*inv_ax; else aspect_x=ax*inv_ay;
		real remaining_blend=1.0f-blend;
		if(bx>by) aspect_y=by*inv_bx*blend+remaining_blend*aspect_y;
		else aspect_x=bx*inv_by*blend+remaining_blend*aspect_x;
		real inv_x=remaining_blend*inv_ax+inv_bx*blend;
		real inv_y=remaining_blend*inv_ay+inv_by*blend;
		real width=context->width*aspect_x;
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
