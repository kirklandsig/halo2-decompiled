// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_0497A0.CPP: view parameter computation */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include <math.h>
#include "unknown_0494b0.h"
#include "globals.h"


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

#include "vector_products.h"
#include "normalize3d.h"
// Round 16: the argument registers agree (ECX is the source, EDX the result, the camera and the
// flags on the stack); retail holds the secondary shape in EDI, the primary shape at [esp+0x14]
// and the inverse at [esp+0x10], ours holds the secondary shape at [esp+0x10] and the inverse at
// [esp+0x14], and our frame is eight bytes short of retail's.
// score 581 (was 581): the frame is 0x58 against retail's 0x60 (ret 0x20 and the EAX result agree) and the body is
// thirteen bytes longer; retail keeps the primary shape in the stack slot at [esp+0x14]
// and the secondary shape in EDI, ours keeps the inverse at [esp+0x14] and the
// secondary shape at [esp+0x10].
// Behaviour difference kept for score: `long tmp2 = scale * d.k` truncates the sixth input
// product through an integer; retail (0x49f19) is a plain mulss. Coordinator decision.
// Round 134: retail reads the wave's sine arguments from the global point: that form 701 (frame 0x60,
// s1 sinks to its use), s1 before the first cross 701, s0's only 602; both shape pointers
// NULL-initialized then set by an if/else (retail's entry, retail length 2622) 599, with s0's
// global arguments 596, with all global arguments 604; as ternaries with global arguments 716.
// frame 0x64 versus 0x60; shape/basis/scalar homes and nine excess bytes remain.
// Frame 0x64 versus 0x60; shape/basis/scalar homes and nine excess bytes remain.
// Frame 0x64 versus 0x60, primary-shape/basis homes and wave scheduling still differ.
// Earlier: Frame 0x64 versus 0x60 remains; primary-shape, basis and scalar scheduling still differ; nine excess bytes remain.
// frame 0x64 versus 0x60; basis/scalar scheduling remains, retail length recovered.
// Frame 0x64 versus 0x60 remains; primary-shape, basis and scalar scheduling still differ; nine excess bytes remain.
// Earlier: frame 0x64 versus 0x60; basis/scalar scheduling remains, retail length recovered.
// Earlier 647 from 647: frame 0x54 vs 0x60 and camera/source saved registers differ; wave vectors fix the frame but lose overall.
// primary output scale 744; default depth scale 743; alternate depth scales 727; product scale workspaces 734;
// late wave lifetime 743; wave offset workspaces 660; inverse before basis 765; direct integer component 743;
// complete shape branches 743; wave alternate scales 695; wave product scales 692; wave primary scales 659;
// wave basis copy blocks 806; shape discriminant 776; wave shape discriminant 696
// Ordinary camera reads with early/late special cross products 737/747;
// early sine amplitude 738; accumulated maxima 796; common mode publication 743.
// Earlier 604 from 647: Frame 0x60 agrees; scalar/basis scheduling and final integer conversion remain.
// wave with conditional shapes fixed 738; wave with camera offsets 634; wave with early projection
// scales 672; wave with phased cross results 691; shared wave offset 667; camera with early projection
// scales 630; camera with phased cross results 620; camera with shared wave offset 604; camera with late
// inverse 648; shared camera early scales 631; shared camera phased cross 620; shared camera early
// phased 625; shared camera late inverse 618.
// DL134: existing wave origin 599 kept; coordinate/point inputs 631/720; first phase 604;
// first/second/both saved-axis copies 604/604/604; phase with native origin 620.
// Reciprocal/shape/parallel-product names and their five combinations 599;
// reciprocal/source/projection case order and all three together 599;
// wave cross-result/offset homes 599; second input products first 598 (below threshold);
// wave homes with case order 599. Frame 0x64 versus 0x60 and wave scheduling remain;
// original native origin, products, case order and conversion retained.
// consumer-shape 594; direct-primary-field 658.
// bool-shape-domain 633; word-shape-domain 645.
// consumer-shape-wide-special 599; direct-primary-field-wide-special 662.
// bool-shape-domain-wide-special 654; word-shape-domain-wide-special 642.
// Shared double-factor wave scalers first/second/both 671/671/669; with separate
// second displacement 661/670/675; both scalers with shared integer truncation
// 683, with separate displacement 704. Second-scaler forms recover frame 0x60
// (first +6), but regress; original shared scalers and all values restored.
// Round 197: complete first/second input field reads 674/671; both camera offsets at each use 700;
// both inputs 694; first/second input with offsets 661/661; all inputs/offsets 692;
// all observations with read-only secondary shape 692. All retain frame 0x64; original body restored.
// Round 162: Secondary shape null-first/present-first/zero-arm/present-arm forms
// 676/671/594/676; reciprocal constant after the shape 679/672/596/678.
// Original shape selection and reciprocal lifetime restored; frame 0x64 remains.
// Ordinary decision bool/byte/short/long 595/595/605/600; before shape selection
// 721/721/714/727. Original decision polarity, shape scope and arithmetic restored.
// named trig factors 673; projection terms 673.
// camera offset terms 594; trig/projection terms 594.
// trig/camera terms 673; projection/camera terms 673.
// all three term families 594; named modulation 673.
// Round 211: Late special cross 707; reversed sine producers/after-amplitude sine 594/594;
// late cross with either sine order 707/707; cross at first scale 592 (two-point gain dropped);
// first-scale cross with late sine 594; complete native basis arms 727. Original body restored.
// Round 174: consume cross vector in place 673; cross at displacement 707;
// early sine factor 652, with late cross 709; factor after modulation 652, with late cross 709;
// cross after first sine 673, with early factor 652. Original cross/scaler graph restored;
// frame 0x64/0x60 and shape/basis/wave homes remain.
// Camera-shift homes, alternate shift homes, wave-result homes, wave-scale homes,
// and four paired combinations all 594; frame and initial shape homes unchanged.
// Round 201: basis/camera/input-copy consumer scopes, all seven nonempty combinations,
// and the independent wave-scratch end scope 594 each (2631/2622). Original scopes and values restored.
// Round 206: shared vector-add first/second/both wave stages 690/581/690; paired camera
// add-scaled 702, with first/second/both waves 708/703/709; all with shared default scale 708.
// Second-wave vector-add kept at plain 581 (2635 bytes); frame 0x64 and shape/basis homes remain.
// Round 88 (frame now 0x58): both shape pointers NULL then one if/else at entry 622 at retail
// length (retail's entry exactly; the 8-byte frame shift costs every slot); declarations swapped
// 622; test inverted 627; inv after the shapes 621; both ternaries at entry 708.
// Round 275: complete clamp operands 581; named lower/upper-first bounded values
// 747/755; in-place lower/upper-first fields 727/790; explicit result arms 890;
// independent lower-upper/upper-lower bounds 747/755. All restored; current frame
// 0x58 versus 0x60, thirteen excess bytes and native shape/basis/wave homes remain.
// Round 209: Intrinsic copies at history/publication/all outputs 581/581/581; conditional origin 726;
// origin and basis 825; basis 817; ordinary and late input snapshots 581; all boundaries 825. Restored;
// actual frame 0x58/0x60, shape/basis/wave homes remain; original sixth-component conversion unchanged.
// Round 385: Current 0x58-frame basis/wave-vector/wave-scalar homes, all seven subsets, and all with
// the reciprocal home 581 each. Original names and complete body restored; frame 0x58/0x60,
// shape/basis homes and thirteen excess bytes remain.
// Round 227: Complete projected add-scaled values in existing/separate cross outputs 721/721;
// with shared final scales 797/797; second cross at its consumer 699/699,
// with shared final scales 825/825. Original projection graph restored;
// frame 0x58/0x60 and shape/basis/wave homes remain.
// Round 177: Existing vector setters for source-zero/source-other/both/depth/zero+depth/
// other+depth/all 580/700/700/702/700/700/700; all plus native point setters 740.
// One-point gain dropped; original body restored; frame 0x58 versus 0x60 remains.
// Round 180: Separate second displacement, entry projection cross outputs and entry
// uninitialized primary shape, masks 1-7, then all with entry special cross output:
// 656/581/656/581/656/581/656/656. Original executable body restored.
// Round 237: existing integer truncation helper 716; first input scaled in place 581,
// with truncation helper 716; second input scaled in place 593, with helper 716;
// both input scales 593, with helper 716, with direct sixth scalar 716.
// Original snapshots, conversion and complete body restored; frame 0x58/0x60 remains.
// Round 110: retail has no integer truncation of the sixth input product (scale * d.k): written
// as a plain real product 648 (2629 bytes); with both shape pointers set at entry 620 (2622 bytes);
// plus the wave's sine arguments read from the global point, frame 0x60 agrees: 600 (2636 bytes);
// plus the first wave through vector3f_add_scaled 575 (2622 bytes, frame 0x60; v2/v4 homes,
// shape0's slot and the sine evaluation order still differ: not kept, the sines then read the
// global while the waves add to its local copy); that form with the sines reading the copy 660;
// both waves/second wave through vector3f_add_scaled 646/627 (frame 0x54); s1 before the cross
// product or after s0 575/575. The truncation is a behaviour difference retail does not have.
// Round 184: Native source/mode/flag snapshots, masks 1-7, then all plus low-word shape selector:
// [704, 691, 715, 700, 721, 695, 710, 711]. Original selector observations, shape pointers, wave
// helpers, scalar trees and sixth-component integer conversion restored.
// Round 186: Selected native basis receivers, seven subsets and the shared selected source
// 713/680/777/698/755/767/754/800. Original copies, helpers, floating trees and sixth-component
// integer conversion restored.
// Round 403: native double storage for rounded sine/modulation and complete maxima,
// masks 1-7: 580/580/581/756/753/753/756; all with both wave factors 771.
// One-point leads dropped; original scalar precision, frame graph and conversion restored.
// Round 213: existing staged cross for first/final special view and paired projection
// families, seven subsets, 581 each; all with independent second wave displacement 656.
// Original body restored; frame 0x58/0x60 and shape/basis/wave homes remain.
// Round 433: Exhaustive reciprocal result with entry shapes/source case order, four forms
// 611/678/611/678; single denominator counterparts 600/696/600/696. All restored;
// the live-through-wave inverse default is not the missing frame cause.
// Round 317: Existing shared scalar clamp for six parallel rows in mode-family masks 1-7,
// then all with force-inline declaration, 702/702/702/581/702/702/702/702. Scaled
// families grow frame to 0x64; original declarations, inputs and body restored.
// Round 93: truncation removed 648; plus both shape pointers set at entry 620; plus the sine
// arguments from the camera point 600; plus the first wave through vector3f_add_scaled 575
// (2622 bytes, frame 0x60; not kept: the sines read the global while the waves use its local
// copy). On that 575 form: first sine before the cross product 575; second wave through the
// helper 642; second cross product right after the first 679; first sine first with the
// helper 642; `w` declared before v3 or at the top 575/575; renamed locals 575 (identical:
// names and declaration order do not move these slots). Left: slot order (shape0 0x18, v2
// 0x28, v4 0x4c, a wave-shift home at 0x64 against retail's 0x14/0x40/0x64 and no home), the
// x87 sine order (retail rounds both sines to float before the SSE scaling).
// Round 49: the type switch as `if (type == 0) ... else if (type != 1) ...` 580 (retail's layout:
// case 0 inline first); with the sine arguments from the global point 701; with both shape pointers
// NULL then one if/else at entry 638 (entry exact but frame 0x58), plus global sine arguments 593
// (frame 0x60), plus the trig factors as named doubles 591; with the plain sixth product 638.
// Round 26: s1 from the global point with the 8.2x phase, sin x and both cosines as named `real`
// locals 701, as `double` locals 694 (forward-substituted, no factor stack); the same reading the
// local copy v1: `real` 581 (identical), `double` 651; both shape pointers NULL then one if/else 622
// (2622 bytes), plus the sines from the global point 604 (frame 0x60). Retail rounds s0 to float before
// an SSE `* 0.03`; ours folds `* 0.5 * 0.03` in x87.
// @retail 0x497a0
void function_0497a0(
	s_view_source *source,
	s_view_result *result,
	s_view_camera *camera,
	s_view_flags *flags,
	vector3f const *c_in,
	vector3f const *d_in,
	real scale,
	long *mode,
	vector4f *old20,
	vector4f *old30)
{
	real inv = -1.0f;
	s_view_shape_1 *shape1 = source->type_low == 0 ? 0 : &source->shape1;

	bool special = ((0x20 & flags->flags)) && (0x40000 & flags->flags);
	point3f v1 = special ? g_4b9da0 : source->v2c;
	vector3f v2 = special ? g_4b9dac : camera->v28;
	vector3f v3 = special ? g_4b9dac : (shape1 ? shape1->v4 : camera->v28);
	vector3f v4 = special ? g_4b9db8 : camera->v34;
	vector3f w;

	if (special)
	{
		vector3f c;
		cross3f(&v4, &v2, &c);
		real s1 = (real)sin(v1.x * 5.6f + v1.y);
		real s0 = (real)((sin(v1.x * 8.2f) * cos(v1.y * 7.8f) * sin(v1.x) + cos(v1.y * 7.75f)) * 0.5f);
		real amp = s0 * 0.03f;
		vector3f wave_shift;
		vector3f_scale(&v4, amp, &wave_shift);
		v1.x = (wave_shift.i) + v1.x;
		v1.y += wave_shift.j;
		v1.z = v1.z + (wave_shift.k);
		real k = s1 * 0.025f;
		vector3f_scale(&c, k, &wave_shift);
		vector3f_add((vector3f const *)&v1, &wave_shift, (vector3f *)&v1);
		cross3f(&v2, &v4, &w);
	}
	else
	{
		w = source->v1c;
	}

	switch (flags->w8e)
	{
		case 1:
			if (shape1)
			{
				inv = -1.0f / (shape1->f2c - shape1->f28);
			}
			break;

		default:
		{
			s_view_shape_0 *shape0 = source->type_low == 0 ? &source->shape0 : 0;
			inv = -1.0f / (shape0 ? shape0->f0c : shape1->f2c);
			break;
		}
	}

	if (source->type != 1)
	{
		result->p0 = v1;
		result->f0c = 1.0f;
		result->p10 = v1;
	}

	switch (source->type)
	{
	case 0:
	{
		result->v20.i = source->v1c.i * inv;
		result->v20.j = (inv * source->v1c.j);
		result->v20.k = inv * source->v1c.k;
		result->v30.i = camera->v34.i * inv;
		result->v30.j = inv * camera->v34.j;
		result->v30.k = inv * camera->v34.k;
		result->f2c = 0.0f;
		result->f3c = 0.0f;
		break;
	}

	case 1:
		break;

	default:
	{
		vector3f r;
		vector3f s;
		cross3f(&v3, &v4, &r);
		cross3f(&v3, &w, &s);
		real sx = -1.0f / shape1->f14;
		real sy = -1.0f / shape1->f18;
		real k = inv * 0.5f;
		result->v20.i = k * (r.i * sx + v2.i);
		result->v20.j = (r.j * sx + v2.j) * k;
		result->v20.k = (r.k * sx + v2.k) * k;
		result->v30.i = k * (s.i * sy + v2.i);
		result->v30.j = k * (s.j * sy + v2.j);
		result->v30.k = k * (s.k * sy + v2.k);
		result->f2c = 0.0f;
		result->f3c = 0.0f;
		break;
	}
	}

	switch (flags->w8e)
	{
	default:
	{
		result->v40.i = v2.i * inv;
		result->v40.j = inv * v2.j;
		result->v40.k = v2.k * inv;
		result->f4c = inv * 0.5f;
		result->v50 = *g_4687a4;
		result->f5c = 0.0f;
		break;
	}

	case 1:
	{
		vector3f const * up = special ? &g_4b9dac : &camera->v28;
		if (source->type != 1)
		{
			result->v40.i = up->i * inv;
			result->v40.j = inv * up->j;
			result->v40.k = inv * up->k;
			result->f4c = inv * (shape1->f28 + source->f28);
			if (!(0x20 & flags->flags))
			{
				real h = (shape1->f2c - shape1->f28) * 0.025f * 0.5f;
				result->v50.i = h * result->v40.i;
				result->v50.j = h * result->v40.j;
				result->v50.k = result->v40.k * h;
				result->f5c = (source->f28 + 0.0078125f) * h * inv + 0.5f;
			}
			else
			{
				result->v50 = *g_4687a4;
				result->f5c = 1.0f;
			}
		}
		break;
	}
	}

	*old20 = *(vector4f *)&result->v20;
	*old30 = *(vector4f *)&result->v30;
	real side_shift = camera->f6c;
	real vertical_shift = camera->f70;
	result->v20.i = result->v20.i + (side_shift * result->v40.i);
	result->v20.j = (result->v40.j * side_shift) + result->v20.j;
	result->v20.k += side_shift * result->v40.k;
	result->v30.i += result->v40.i * vertical_shift;
	result->v30.j = (result->v40.j * vertical_shift) + result->v30.j;
	result->v30.k = (result->v30.k + (vertical_shift * result->v40.k));

	vector3f c = *c_in;
	vector3f d = *d_in;
	real ca = scale * c.i;
	real cb = scale * c.j;
	real cc = c.k * scale;
	real da = (d.i * scale);
	real db = d.j * scale;
	long tmp2 = (((scale * d.k)));
	real dc = tmp2;
	real mc = MAX(ca, MAX(cb, cc));
	real md = MAX(da, MAX(db, dc));
	real m = MAX(mc, md);

	if (m > 2.0f)
	{
		result->f60 = PIN(ca * 0.25f, 0.0f, 1.0f);
		result->f64 = PIN(cb * 0.25f, 0.0f, 1.0f);
		result->f68 = PIN(cc * 0.25f, 0.0f, 1.0f);
		result->f70 = PIN(da * 0.25f, 0.0f, 1.0f);
		result->f74 = PIN(db * 0.25f, 0.0f, 1.0f);
		result->f78 = PIN(dc * 0.25f, 0.0f, 1.0f);
		*mode = 2;
	}
	else if (m > 1.0f)
	{
		result->f60 = PIN(ca * 0.5f, 0.0f, 1.0f);
		result->f64 = PIN(cb * 0.5f, 0.0f, 1.0f);
		result->f68 = PIN(cc * 0.5f, 0.0f, 1.0f);
		result->f70 = PIN(da * 0.5f, 0.0f, 1.0f);
		result->f74 = PIN(db * 0.5f, 0.0f, 1.0f);
		result->f78 = PIN(dc * 0.5f, 0.0f, 1.0f);
		*mode = 1;
	}
	else
	{
		result->f60 = PIN(ca, 0.0f, 1.0f);
		result->f64 = PIN(cb, 0.0f, 1.0f);
		result->f68 = PIN(cc, 0.0f, 1.0f);
		result->f70 = PIN(da, 0.0f, 1.0f);
		result->f74 = PIN(db, 0.0f, 1.0f);
		result->f78 = PIN(dc, 0.0f, 1.0f);
		*mode = 0;
	}
}


real function_17ca10(real value, short curve);

// @retail 0x42c50
real function_42c50(point3f const *position, vector3f const *normal, short mode, byte const *flags)
{
	(void)&flags;
	real result = 1.0f;
	if (mode)
	{
		real x = g_4b9da0.x - position->x;
		real y = g_4b9da0.y - position->y;
		real z = g_4b9da0.z - position->z;
		result = (real)fabs((z * normal->k + y * normal->j + x * normal->i) / sqrt(z * z + y * y + x * x));
		if (*flags & 0x40)
			result = function_17ca10(result, 2);
		if (mode == 2)
			result = 1.0f - result;
	}
	return result;
}


extern s_record_pool *g_4ea940;
bool function_bad50(long object_index, long index, point3f *out);

struct s_ribbon_source
{
    byte unknown00[8];
    long object_index;
    short marker_index;
    byte unknown0e[6];
    real scale;
    byte unknown18[4];
    real u, v;
    byte unknown24[12];
    short point_count[4];
    long point_index[4];
};
struct s_ribbon_state
{
    byte unknown00[0x18];
    real width;
    color4f lower, upper;
    dword flags;
};
struct s_ribbon_definition
{
    word flags00, flags02;
    byte unknown04[0x14];
    short mode;
    byte unknown1a[2];
    real u_step, v_width;
    byte unknown24[0xc4];
    long state_count;
    s_ribbon_state *states;
};
struct s_ribbon_point
{
    short salt;
    byte flags;
    char state;
    real transition, unknown08, scale;
    byte unknown10[0xc];
    point3f position;
    vector3f velocity;
    long next;
};
struct s_ribbon_input
{
    s_ribbon_source *source;
    s_ribbon_definition *definition;
    short slot, unknown0a;
    byte *shader;
    bool normal;
    byte unknown11[3];
    long vertex_count;
};
struct s_ribbon_vertex
{
    point3f position;
    real u, v;
    dword color;
};

PRIVATE __forceinline dword ribbon_pixel(color4f const *color)
{
    long b, g, r, a;
    real scale = 255.0f;
    dword pixel = 0;
    __asm
    {
        mov edx, color
        fld dword ptr [edx]
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 8]
        fld dword ptr [edx + 12]
        fld scale
        fmul st(4), st(0)
        fmul st(3), st(0)
        fmul st(2), st(0)
        fmulp st(1), st(0)
        fistp b
        fistp g
        fistp r
        fistp a
        mov edx, b
        mov ebx, g
        mov ecx, r
        mov eax, a
        shl ebx, 8
        shl ecx, 0x10
        shl eax, 0x18
        or edx, ebx
        or edx, ecx
        or edx, eax
        mov pixel, edx
    }
    return pixel;
}

// Round 16: the argument registers agree (ECX is the source, EDX the result, the camera and the
// flags on the stack); retail keeps the vertex cursor in EDI and ours leaves it in its argument
// slot, and retail reaches the endpoint handling by a forward branch from the loop.
// score 284 (was 512): length 2596 agrees; frame 0xa0 against retail's 0xac (retail spills the first
// edge point's y at +0xb4); retail gives the second edge point XMM0-2 and the half width XMM4 where
// ours gives the first point XMM0/1/4 and the half width XMM6, which moves every geometry case; the
// state clamp keeps the index in the state's register (jle) where ours uses the bound's (jg); slots
// differ (state width +0x54 against +0x4c, tint +0x7c against +0x78).
// Earlier (512): frame 0x9c against retail's 0xac (retail has a second home for the blended
// width, a spill of the first edge point's y and the width at +0x4c where ours rotates it to +0x2c),
// the pool global reloaded per use, the zero colour stored as integers and the 0x80 flag test.
// earlier: the frame 0xac and the body length 2596 agree; retail stages
// the definition in EBX and the output vertices in EDI, ours stages the definition
// in EDI, and retail places the endpoint handling after the loop where ours places
// it before.
// frame 0xac agrees; definition/vertex registers, endpoint-first loop layout and
// color/vector homes differ. Tried definition first 620; paired vertex store order 636; zero color record
// Frame 0xac agrees; definition/output carriers, endpoint placement and color/vector homes remain.
// Tried definition first 620; paired vertex store order 636; zero color record
// 626; shared position setters 621; second output element 640; single result exit 631; plain point chain for
// 620; color factor result 626; shared cross products 615; shared direction difference 620; native vector
// role homes 620; native point initialization 623; loop vertex store order 620; shared geometry helpers 615;
// shared cross position setters 617; shared cross zero color 622.
// round 106: Frame 0xac agrees; definition/vertex carriers, endpoint placement and color/vector homes remain. Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// Iteration geometry scratch 615; iteration tint output 615; shared transition value 630;
// width after initial color 616; live input definition reads 626.
// Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// Advancing output cursor 664; segment/camera-delta/plus-minus point names 615/615/615; second
// position copied from first 629; byte state flags 615; true result and one exit 647.
// Paired displacement loop 615; side scaling loop 615; normal scaling loops 615; RGB tint loop 615; planar geometry cases first 615.
// Width-path result 626; scale override 622; unsigned point handle 615; final endpoint base 634; two-use packed pixel 615.
// Earlier form: round 106: Frame 0xac agrees; definition/vertex carriers, endpoint placement and color/vector homes remain. Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// Earlier form: round 108: definition/output carriers, endpoint placement and color/vector homes remain.
// Persistent workspaces 620; fused color-component loop 643; shared geometry neighbor 620;
// half-width before tint 620; valid-state-first arms 616.
// Current position field pointer with read-only point/state/source contracts,
// all eight combinations 647; frame 0xac and 2632 bytes. Original body restored.
// round 106: Frame 0xac agrees; definition/vertex carriers, endpoint placement and color/vector homes remain. Indexed vertex output 697; clamp branches 622; color pointer walks 615; geometry case four first 626; scales before coordinates 615.
// round 108: definition/output carriers, endpoint placement and color/vector homes remain.
// Round 349: Conversion-local names 615; source pointer rereads 624; normal-output flag 642; position memcpy 615; shared in-place scales 615 (fast).
// Round 211: Lower native/library color seed 684/684; upper native/library seed 661/661;
// complete difference/scaled-difference stages 658/658; upper seed with difference/scaled
// stages 707/700. Original separate color phases, helpers and output ownership restored.
// Native upper bound/point scale/both 615/626/626; state-array receiver/with upper bound 615/615; scale
// plus array/all inputs/all role homes 626/626/626. All restored.
// U/V entry ordering, seven combinations 615; definition then source 615.
// Round 201: state/geometry/both consumer-end scopes 615/615/615; per-point scratch end,
// with state/geometry/both endings, and combined scopes with library tint copy 624 each (2604 bytes).
// Original scratch scopes, tint initialization, helper calls and endpoint masks restored.
// Round 206: Whole camera-facing normal/side normalization through the existing two inline helpers,
// all eight nonoriginal combinations [615, 615, 620, 620, 620, 620, 620, 620]. Grouping changes require an exact match; all restored.
// Round 385: Final vertex extent/endpoint flags/both 618/622/626; native opacity alone/with
// extent/flags/both 624/625/630/628; consistent next handle 634. All restored; frame/carrier, endpoint
// placement and color/vector homes remain.
// Round 227: Geometry point producers: camera plus scaled-add/minus setter/both
// 615/615/615; planar setters alone/with plus/with minus/with both
// 615/615/615/615; all with paired displacement values 630.
// Original point producers restored; definition/output carriers and color/vector homes remain.
// Round 177: Existing x87 conversion with C++ integer expression/accumulator/parallel terms/
// unsigned channels/channel declaration order/late packed result/combined forms 624 each;
// existing complete packer call 728. All restored; definition/output carriers and endpoint layout remain.
// Round 180: First/transition/both short channel indices, first/transition/both word indices,
// both byte and both dword indices: 638/657/739/638/657/739/739/615. Original executable body
// restored.
// Round 396: Direct state-range admission, invalid/valid arm first: native char
// 637/637, short 633/633, long 647/647, consistent field observations 624/624.
// All restored; definition/output carriers, endpoint placement and frame homes remain.
// Round 213: existing staged side/normal/both products 615 each; each with complete
// paired side offset 630, plus shared normal scales 630/630. Paired offset shrinks the
// frame to 0xa8 and length to 2584. Original 0xac-frame body restored; carriers/layout remain.
// Round 433: Independent transition endpoints/shared camera displacement/both 615 each;
// definition observed at every use alone/with either/with both 626 each, all plus source
// observations 634. All restored; native phase receivers do not recover the early carriers.
// Round 93 (kept, 579): the callback's buffer cast to a local vertex cursor, a pointer to the
// second vertex, per-component position stores, the next state's colour staged in its own
// array with the transition read once, the edge points built in each case from the point and
// case 0's normal set in its arms give retail's layout (endpoint block after the loop, second
// cursor at +0x28, definition in EBX). Left: frame 0x9c against 0xac (retail keeps a home for
// the first edge point and a width temporary), EDI holds the point pool where retail keeps the
// second cursor there and reloads the pool, the state-index clamp registers and the zero colour.
// Tried: buffer cursor alone 664; second-vertex pointer alone 666, with component stores 617;
// buffer + pointer + component stores 606, with u/v in vertex order 606, with flipped colour
// chains 610, with per-case points 634, with case-0 normal fields 606/632, definition first
// 606, cursor declared late 609, scale `1.0f` then if 605, transition local 629, staged colour
// 640; then per-case points 595 (frame 0xa4); with the case-0 normal arms 579 (kept); on that,
// scale if 593, object index local 596, flag bitfields 594, both 582, clamp if/else 579 (same
// code) and zero-default 592, zero stores in order 579, renamed locals 579 (no slot change).
// The buffer is untyped at the interface: 0x43740 queues this callback through 0x1e5e0 as
// the fill function 0x35b00 passes to 0x4b220, typed `void (__stdcall *)(void *, long, void *)`
// in unknown_01cf50.cpp.
// Round 443: Existing alpha/tint/pixel ordinary-inline producer boundaries, seven subsets and
// all with definition before source, 615 each. All original declarations, pixel qualifier and
// body restored; output/definition carriers, endpoint layout and color/vector homes remain.
// Lane 7 round 93's 579 form was not kept (coordinator review, round 94): it typed the first
// parameter `void *buffer` with a typed cursor local, a signature form with no call evidence
// (0x42cf0 is only passed as a (long) callback). With `s_ribbon_vertex *vertices` restored, the
// rest of that round's body (transition read once, next colour in its own array then blended,
// edge points set per case, field-wise stores through the second-vertex pointer) scored 622
// against 615 for this body (both --fast), so the body is back to its earlier form.
// Round 49: the round-93 body with the typed parameter and a local output cursor 579 (retail keeps
// the advancing pointer in its own frame slot and never touches the parameter's home); on it, each
// case reads `previous` directly and looks up the next point only in its no-previous arm 512 (kept).
// On 579: memset zero colour 612, with scale if 608; pool read through an address cast (diagnostic)
// 591; clamp `state <= max ? state : max` 579 (same code). On 512: case 0 dx/dy locals 522; memset
// 584, with scale if 584; object index local 566; memset + scale if + object index 577; length sums
// written i,j,k / i,j / dy,dx (VC reverses them into retail's fst-k-first x87 order) 573, with case 0
// dx/dy 593; memset + scale if + object index + those sums 582 (best aligned: retail's loop head and
// pool reloads; the score is positional); with the clamp as if/else on a state-initialised index 596;
// width declared first or renamed 582 (same code). Every `flags00 & 0x80` spelling gives cmp/js.
// Round 26 (kept, 284): endpoint fades through a `faded` local stored own-first 480 (chained own-first
// 578: the own store narrows to a byte store); on that, memset zero colour 592, plus scale `1.0f` then
// if 588, plus `state_width` with `width = state_width` 480 (after the colour 531), plus the state
// record indexed by `point->state` 461, plus an `object_index` local 458 (gives the `test byte, 0x80`),
// plus u/v stores in vertex order 458, plus the tint as a three-component loop 282 (retail length).
// Each part removed alone (on 282, with the pixel stored to vertex 0 first): 320/588/518/562/301/308/
// 290/458; the pixel order itself 284 (-2, not kept); u/v chained again on the kept 284 form 292.
// Other forms: interpolation inline for the colour/width blends 679 (on 512); clamp as an if/else chain
// 604 and with `<` 588 (on the 588 form), `<=`/PIN identical (on 461). On 282: case 0 points as struct
// copies 535, second point first 466, chained copy 535; dy assigned first in cases 1/2 279, plus the
// points declared `second, first` or after the normal 279; `real dy, dx` 282; length sums i,j / i,j,k
// side / i,j,k normal / dy,dx 282/400/282/282. On 284: second point assigned first in every case 479,
// with the stores second-first 479; u/v setup with v first 304.
// Lead: second point assigned first in cases 1/2 and 4 only 386 (frame 0xa8, retail's y spill and blend
// registers appear; edge point/half-width XMM permutation and the u/v slot order left); in all cases 479,
// with second-first stores 479; cases 0+1/2 567, 0+4 474, 4 365, 1/2 398. Renamed pixel locals or
// `v_width` do not move slots (284/386).
// Clamp as `if (state < 0) index = 0; else { index = state; if (index > max) index = max; }` 279 (the
// clamp is then retail's exactly; -5), with dy first in cases 1/2 276; zero-default clamp 579, ternary
// then if 584; the exact clamp with second point first in 1/2 and 4 381 (with dy first 381).
// @retail 0x42cf0
bool __stdcall function_42cf0(s_ribbon_vertex *vertices, long unknown, s_ribbon_input *input)
{
    // Retail keeps all three callback arguments on the stack.
    (void)&unknown;
    s_ribbon_vertex *vertex = vertices;
    s_ribbon_source *source = input->source;
    s_ribbon_definition *definition = input->definition;
    real u = source->u;
    real u_step = definition->u_step;
    if (definition->flags02 & 0x40)
        u_step *= source->scale;
    u_step = -u_step;
    real v = source->v;
    real v_width = definition->v_width;
    if (definition->flags02 & 0x80)
        v_width *= source->scale;
    v_width += v;
    long index = source->point_index[input->slot];
    s_ribbon_point *previous = NULL;
    while (index != NONE)
    {
        s_ribbon_point *point = (s_ribbon_point *)(g_4ea940->data + (index & 0xffff) * 0x38);
        long state_index = point->state < 0 ? 0 : ((long)point->state > definition->state_count - 1 ? definition->state_count - 1 : (long)point->state);
        color4f color;
        real width;
        if (state_index != point->state)
        {
            width = 0.0f;
            memset(&color, 0, sizeof(color));
        }
        else
        {
            s_ribbon_state *state = &definition->states[point->state];
            real scale = 1.0f;
            if (state->flags & 0x20)
                scale = point->scale;
            real state_width = state->width;
            if (state->flags & 0x10)
                state_width *= point->scale;
            width = state_width;
            real *lower = (real *)&state->lower;
            real *upper = (real *)&state->upper;
            real *out = (real *)&color;
            for (long i = 0; i < 4; ++i)
                out[i] = (upper[i] - lower[i]) * scale + lower[i];
            if (point->flags & 2)
            {
                s_ribbon_state *next = &definition->states[point->state + 1];
                real transition = point->transition;
                real next_width = next->width;
                if (next->flags & 0x10)
                    next_width *= point->scale;
                real next_color[4];
                lower = (real *)&next->lower;
                upper = (real *)&next->upper;
                for (long j = 0; j < 4; ++j)
                    next_color[j] = (upper[j] - lower[j]) * scale + lower[j];
                width = (next_width - state_width) * transition + state_width;
                for (long k = 0; k < 4; ++k)
                    out[k] += (next_color[k] - out[k]) * transition;
            }
        }
        long object_index = source->object_index;
        if (object_index != NONE && !(definition->flags00 & 0x80))
        {
            point3f tint = *g_468710;
            byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            byte *object_tag = g_4e3b44[(*(long *)object) & 0xffff].bytes;
            byte *markers = *(byte **)(object_tag + 0x98);
            function_bad50(object_index, *(short *)(markers + source->marker_index * 24 + 12), &tint);
            for (long c = 0; c < 3; ++c)
                (&color.red)[c] *= (&tint.x)[c];
        }
        real half_width = width * 0.5f;
        s_ribbon_vertex *outer = &vertex[1];
        vertex[0].u = u;
        vertex[0].v = v;
        outer->u = u;
        outer->v = v_width;
        point3f first, second;
        vector3f normal;
        switch (definition->mode)
        {
        case 0:
            first.x = point->position.x; first.y = point->position.y; first.z = point->position.z + half_width;
            second.x = point->position.x; second.y = point->position.y; second.z = point->position.z - half_width;
            if (input->normal)
            {
                if (previous)
                {
                    normal.j = point->position.x - previous->position.x;
                    normal.i = previous->position.y - point->position.y;
                }
                else
                {
                    s_ribbon_point *neighbor = (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                    normal.j = neighbor->position.x - point->position.x;
                    normal.i = point->position.y - neighbor->position.y;
                }
                normal.k = 0.0f;
                real length = (real)sqrt(normal.j * normal.j + normal.i * normal.i);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    normal.i *= inverse; normal.j *= inverse; normal.k *= inverse;
                }
            }
            break;
        case 1:
        case 2:
            {
                real dx, dy;
                if (previous)
                {
                    dx = point->position.x - previous->position.x;
                    dy = previous->position.y - point->position.y;
                }
                else
                {
                    s_ribbon_point *neighbor = (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                    dx = neighbor->position.x - point->position.x;
                    dy = point->position.y - neighbor->position.y;
                }
                real length = (real)sqrt(dx * dx + dy * dy);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    dx *= inverse; dy *= inverse;
                }
                first.x = point->position.x + dy * half_width; first.y = point->position.y + dx * half_width; first.z = point->position.z;
                second.x = point->position.x - dy * half_width; second.y = point->position.y - dx * half_width; second.z = point->position.z;
                if (input->normal) normal = *g_4687b0;
            }
            break;
        case 4:
            {
                vector3f view, direction, side;
                if (previous)
                {
                    view.i = g_4b9da0.x - previous->position.x; view.j = g_4b9da0.y - previous->position.y; view.k = g_4b9da0.z - previous->position.z;
                    direction.i = point->position.x - previous->position.x;
                    direction.j = point->position.y - previous->position.y;
                    direction.k = point->position.z - previous->position.z;
                }
                else
                {
                    view.i = g_4b9da0.x - point->position.x; view.j = g_4b9da0.y - point->position.y; view.k = g_4b9da0.z - point->position.z;
                    s_ribbon_point *neighbor = (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                    direction.i = neighbor->position.x - point->position.x;
                    direction.j = neighbor->position.y - point->position.y;
                    direction.k = neighbor->position.z - point->position.z;
                }
                cross3f(&view, &direction, &side);
                real length = (real)sqrt(side.k * side.k + side.j * side.j + side.i * side.i);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    side.i *= inverse; side.j *= inverse; side.k *= inverse;
                }
                first.x = point->position.x + side.i * half_width; first.y = point->position.y + side.j * half_width; first.z = point->position.z + side.k * half_width;
                second.x = point->position.x - side.i * half_width; second.y = point->position.y - side.j * half_width; second.z = point->position.z - side.k * half_width;
                if (input->normal)
                {
                    cross3f(&direction, &side, &normal);
                    length = (real)sqrt(normal.k * normal.k + normal.j * normal.j + normal.i * normal.i);
                    if (!(fabs(length) < 0.0001f))
                    {
                        real inverse = 1.0f / length;
                        normal.i *= inverse; normal.j *= inverse; normal.k *= inverse;
                    }
                }
            }
            break;
        default:
            return false;
        }
        vertex[0].position.x = first.x;
        vertex[0].position.y = first.y;
        vertex[0].position.z = first.z;
        outer->position.x = second.x;
        outer->position.y = second.y;
        outer->position.z = second.z;
        color.alpha *= function_42c50(&point->position, &normal, *(short *)(input->shader + 0x2c), (byte *)definition);
        color.alpha = PIN(color.alpha, 0.0f, 1.0f);
        vertex[0].color = outer->color = ribbon_pixel(&color);
        vertex += 2;
        u += u_step;
        previous = point;
        index = point->next;
    }
    vertex -= input->vertex_count;
    if (!(definition->flags00 & 1) && source->point_count[input->slot] > 2)
    {
        dword faded = vertex[0].color & 0xffffff;
        vertex[0].color = faded;
        vertex[1].color = faded;
    }
    if (!(definition->flags00 & 2))
    {
        dword faded = vertex[input->vertex_count - 1].color & 0xffffff;
        vertex[input->vertex_count - 1].color = faded;
        vertex[input->vertex_count - 2].color = faded;
    }
    return true;
}

long function_137590(long group_index, short frame_index, short sequence_index);
struct D3DTexture;
struct s_bitmap_view;
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);
void function_52d40(long bitmap_index, byte *shader, long tag, dword flags);
bool function_1e5e0(long size, point3f const *position, long a, long b, long c,
    long d, long e, long f, long g, void const *data);

struct s_43740_input
{
    s_ribbon_input vertices;
    long tag;
    long bitmap;
};

// @retail 0x43720
void __stdcall function_43720(s_43740_input *input)
{
    function_52d40(input->bitmap, input->vertices.shader, input->tag, false);
}

// score 62 (was 62): retail reads the object's field before the frame is set up
// and compares it with -1, ours loads it into ebp and compares ebp. Retail
// rereads the tag for input.tag (now matched), keeps the slot index in ebp after
// the texture test and tests the normal flag as shader + 0x2c. Tried: early
// return 63; the test reading the field and the tag read inside 62; the body as
// an inline helper taking the tag 62; the normal flag read through the shader
// pointer 66 (definition moves to the stack); a vertex count local first 64 (it
// swaps source and definition registers and 0x42b20 stops matching).
// Round 197 fast trials: Populated request definition readback, direct/stored source and slot x
// definition/shader normal x retained/removed slot anchor 62, 62, 67, 67, 62, 62, 67, 67.
// Round 121: the slot anchor `(void)&slot;` dropped (identical code, 0x42b20 still matches); on that:
// the tag as a dword 62; a slot index local for both arrays 62; the vertex count stored first 63; the
// parameters as (definition, source, slot) at the definition and the call 62 (identical code).
// Round 98: retail tests the tag in eax before `push ebp` and copies it to ebp after (ours keeps it
// in ebp from the load, so ebp is saved before the test). The test on the field with the tag read
// inside 62; the normal flag through the stored shader pointer or a shader pointer local 67 (the
// definition moves to the stack and 0x42b20 stops matching); the input record or the bitmap
// declared at the top 62/62; the record filled count, shader, normal, slot, tag, then source,
// definition, bitmap 63; the group lookup re-reading the tag field 62 (CSE gives the same code).
// Round 213: complete mutable request/zero aggregate/library zero/readonly request
// 91/100/105/91; direct-position counterparts 91/100/105/91. Original body restored;
// tag admission before saved EBP and signed-slot/shader-relative homes remain.
// Round 12: the tag field read at each use (test, call, group lookup) 62 and with a tag local
// inside 62 (the reads merge into one value); the normal flag read through the stored
// input.vertices.shader 67 (213 bytes, the definition moves to the stack, 0x42b20 62); the
// position computed before the input fill 62 (retail computes it before the pushes; ours
// still interleaves it); a named vertex count used for the field and the call 64, with the
// position first 64 (0x42b20 4).
// Round 443: Ordinary-inline declarations at lookup/texture/submission boundaries, seven
// subsets 62 each; all plus shader-relative normal 67 and matched caller 62. All declarations
// and original body restored; early tag/EBP and signed-slot homes remain.
// Round 49: normal flag through the stored shader member with (definition, source, slot) at the
// definition and call 67; tag field read at each use 62 (identical); shader pointer local 67, with
// the lookup rereading the field 67, with every tag use read from the field 67 (each still folds the
// read to [definition + 0x68] but makes the definition a stack argument held in EBP).
// Round 26: retail's EAX test / EBP copy head needs the tag's test read and the body's reads to be
// separate values: field test with the group lookup (field) before the index call (field) 68, or a
// tag local read before the call for the lookup only 68 (both split like retail but the tag outranks
// a parameter: ret 8, 0x42b20 60); either with the record's tag from the local 40 (head through the
// index call identical; the tag then lives across the texture call where retail re-reads the field,
// and it mixes local and field reads of one value); 36-form grid of those splits x normal read x slot
// local x fill order 68-70; early-return guard 68; recomputed count argument 68; tag local used
// everywhere 63, masked in place 62. The normal read through the stored shader (member, local,
// accessor inline) still folds to +0x68 under LTCG (compile-only harness gives [ecx+0x2c]) 67/70.
// @retail 0x43740
void function_43740(s_ribbon_source *source, s_ribbon_definition *definition, short slot)
{
    byte *bytes = (byte *)definition;
    long tag = *(long *)(bytes + 0x34);
    if (tag != NONE)
    {
        long bitmap = function_137590(tag, *(short *)((byte *)source + 0x1a), *(short *)((byte *)source + 0x18));
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        s_bitmap_view *image = (s_bitmap_view *)(*(byte **)(group + 0x48) + bitmap * 0x74);
        if (function_12360(image, 0.0f))
        {
            s_43740_input input;
            input.vertices.source = source;
            input.vertices.definition = definition;
            input.vertices.slot = slot;
            input.vertices.shader = bytes + 0x3c;
            input.vertices.normal = *(short *)(bytes + 0x68) != 0;
            input.vertices.vertex_count = (short)(source->point_count[slot] * 2);
            input.tag = *(long *)(bytes + 0x34);
            input.bitmap = bitmap;
            point3f const *position = (point3f *)(g_4ea940->data + (source->point_index[slot] & 0xffff) * 0x38 + 0x1c);
            function_1e5e0(0x20, position, (long)function_43720, (long)function_42cf0,
                0, 4, 0x28, 0x18, input.vertices.vertex_count, &input);
        }
    }
}

extern s_record_pool *g_4ea944;

// @retail 0x42b20
void __stdcall function_42b20(dword mask)
{
    long index = data_datum_index(g_4ea944, function_16bc00(g_4ea944, 0));
    while (index != NONE)
    {
        s_ribbon_source *source = (s_ribbon_source *)(g_4ea944->data + (index & 0xffff) * 0x48);
        long tag = *(long *)((byte *)source + 4);
        s_ribbon_definition *definition = (s_ribbon_definition *)g_4e3b44[tag & 0xffff].bytes;
        for (short slot = 0; slot < 4; slot++)
        {
            if ((mask & (1 << (byte)definition->mode)) && source->point_count[slot] >= 2)
                function_43740(source, definition, slot);
        }
        index = data_datum_index(g_4ea944,
            data_find_index(g_4ea944, index == NONE ? 0 : (index & 0xffff) + 1));
    }
}

s_camera_storage g_camera_storage;
