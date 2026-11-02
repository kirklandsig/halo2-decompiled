// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_050690.CPP: random seeds drawn from the game's random generator
   (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
#include "camera_storage.h"
#include "reset_storage.h"
#include "sound_sources.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "slot_handler.h"
#include "unknown_1765e0.h"
#include <xtl.h>
#include <string.h>
#include <math.h>

/* nine random words and two random reals in [0, 1] (0x50 bytes) */
struct s_random_draw
{
	dword words[9];
	real phases[9];
	real reals[2];
};

static inline dword random_draw_next(dword *seed)
{
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return *seed >> 16;
}

static inline real random_draw_real(dword *seed)
{
	return (real)random_draw_next(seed) * (1.f / 65535.f);
}

// @retail 0x50690
void function_50690(s_random_draw *draw)
{
	memset(draw, 0, sizeof(*draw));
	dword *seed = &g_4e7408->seed;
	for (long i = 0; i < 9; i++)
		draw->words[i] = random_draw_next(seed);
	draw->reals[0] = random_draw_real(seed);
	draw->reals[1] = random_draw_real(seed);
}

struct s_random_draw_definition
{
	short unknown00;
	short additional_word_count;
	byte unknown04[12];
	real period;
	byte unknown14[0x90 - 0x14];
	real rates[9];
};

s_random_draw g_4c9828;
bool g_50944d;

static __forceinline void random_draw_advance(real *value, real elapsed)
{
	elapsed += *value;
	*value = elapsed;
}

// @retail 0x507e0
void __stdcall function_507e0(s_random_draw *draw, const s_random_draw_definition *definition, real elapsed)
{
	long count = definition->additional_word_count + 2;
	if (!draw)
	{
		draw = &g_4c9828;
		if (g_50944d)
		{
			function_50690(draw);
			g_50944d = true;
		}
	}
	if (elapsed > 0.f)
	{
		for (long i = 0; i < count; i++)
		{
			if (definition->rates[i] > 0.f)
			{
				draw->phases[i] += definition->rates[i] * elapsed;
				if (draw->phases[i] >= 1.f)
				{
					draw->phases[i] -= (real)floor(draw->phases[i]);
					draw->words[i]++;
				}
			}
		}
		if (definition->period != 0.f)
			draw->reals[0] += elapsed / definition->period;
		random_draw_advance(&draw->reals[1], elapsed);
	}
}

struct s_noise_curve
{
	long size;
	const byte *data;
};

struct s_noise_draw_definition
{
	short unknown00;
	short exponent;
	byte unknown04[0x34 - 4];
	s_noise_curve horizontal_curve;
	s_noise_curve vertical_curve;
	real bias;
	s_noise_curve weight_curve;
	byte unknown50[0xd0 - 0x50];
	word active_levels;
};

struct s_noise_basis
{
	real scale;
	real forward[3];
	real left[3];
	real up[3];
	real origin[3];
};

struct s_noise_pair
{
	real x;
	real y;
};

struct s_noise_point
{
	real position[3];
	real width;
	real amount;
	dword flags;
};

real function_13b390(void const *function, real input, real range);

static __forceinline real noise_curve_value(const s_noise_curve *curve, real input)
{
	real result;
	if (curve->data && curve->size > 0)
	{
		result = function_13b390(curve, input, 0.0f);
		if (!(curve->data[1] & 0xf0))
		{
			real lower = *(const real *)(curve->data + 4);
			real upper = *(const real *)(curve->data + 8);
			if (result < 0.0f)
				result = 0.0f;
			else if (result > 1.0f)
				result = 1.0f;
			result = lower + (upper - lower) * result;
		}
	}
	else
		result = 0.0f;
	return result;
}


/* the liquid draw callback (0x508d0): the types it reads; see docs/unknown_0508d0.md */

/* the payload of a queued liquid record (unknown_01cf50.cpp's 0x429a0) */
struct s_liquid_draw_payload
{
	byte type;
	byte opacity;
	byte unknown02[2];
	long a;
	long definition_index;
	long liquid_index;
	point3f position;
	point3f endpoint;
	vector3f first;
	vector3f second;
};

/* a sub-element of a liquid element (0x38 bytes): one strip */
struct s_liquid_strip
{
	byte unknown00[0xc];
	short sequence;
	byte unknown0e[2];
	s_noise_curve amount_curve;
	s_noise_curve colour_curve;
	s_noise_curve index_curve;
	s_noise_curve facing_curve;
	s_noise_curve width_curve;
};

/* an element of a liquid definition (0xec bytes), read here as copies of an arc */
struct s_liquid_arc
{
	byte flags;
	byte unknown01;
	short exponent;
	real length;
	short copies;
	byte unknown0a[2];
	real cone_angle;
	real period;
	byte unknown14[0x20 - 0x14];
	long effect_tag_index;
	byte unknown24[4];
	long bitmap_tag_index;
	byte unknown2c[0xd4 - 0x2c];
	long strip_count;
	s_liquid_strip *strips;
	s_noise_curve amplitude_curve;
	s_noise_curve alpha_curve;
};

struct s_liquid_arc_definition
{
	short unknown00;
	short kind;
	byte unknown04[0x40 - 4];
	real near_distance;
	real far_distance;
	byte unknown48[0x68 - 0x48];
	long arc_count;
	s_liquid_arc *arc_list;
};

/* the collision result of 0x1697c0 (0x5c bytes), as read here */
struct s_liquid_collision
{
	long type;
	real t;
	point3f point;
	dword start_location[2];
	s_location end_location;
	dword material;
	plane3f plane;
	byte unknown38[0x5c - 0x38];
};

typedef char assert_liquid_strip_size[(sizeof(s_liquid_strip) == 0x38) ? 1 : -1];
typedef char assert_liquid_arc_size[(sizeof(s_liquid_arc) == 0xec) ? 1 : -1];
typedef char assert_liquid_collision_size[(sizeof(s_liquid_collision) == 0x5c) ? 1 : -1];

struct s_render_reset_state;
struct s_collision_result_1697c0;
struct s_tag_data;
struct s_501e0;
extern byte g_46713c;
extern byte g_4670bc;
extern short g_47d8e0;
real function_30bf0(vector3f *v);
void function_16b10(s_render_reset_state *state);
byte *function_c4210(long index, long element_index);
long function_c4250(long index);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, short element_index,
	long entry_element_index, long index, long variant, long *first_value04, long *second_value04,
	long *first_value, long *second_value, long *first_value0c, long *second_value0c);
dword vector3d_compress(vector3f const *vector);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);
dword function_13bc00(s_tag_data const *function, real input);
color3f *unpack_color3f(dword pixel, color3f *color);
bool __stdcall function_4f7e0(long tag, s_501e0 const *points, long count, short bitmap,
	real cosine, real sine, bool segments, bool perspective, bool filtered);
void function_0222d0(D3DRENDERSTATETYPE state, dword value);
long __stdcall function_516d0(bool reduced, dword seed,
	const s_noise_draw_definition *definition, const s_random_draw *draw,
	const s_noise_basis *basis, real amplitude, s_noise_point *points);

/* draws one queued liquid record: up to eight copies of an arc of noise-displaced points
   for each element of the liquid definition, faded by the distance from the
   camera, each arc cut short where it hits something */
// @retail 0x508d0
void __stdcall function_508d0(void *payload_data)
{
	s_liquid_draw_payload *payload = (s_liquid_draw_payload *)payload_data;
	if (payload->definition_index == NONE)
		return;
	s_liquid_arc_definition *definition =
		(s_liquid_arc_definition *)g_4e3b44[payload->definition_index & 0xffff].bytes;
	if (definition->arc_count <= 0)
		return;

	real fade = 1.0f;
	vector3f to_camera;
	to_camera.i = g_4b9da0.x - payload->position.x;
	to_camera.j = g_4b9da0.y - payload->position.y;
	to_camera.k = g_4b9da0.z - payload->position.z;
	real distance = function_30bf0(&to_camera);
	if (definition->far_distance > definition->near_distance)
	{
		if (0.0f > (definition->far_distance - distance) / (definition->far_distance - definition->near_distance))
			return;
		if ((definition->far_distance - distance) / (definition->far_distance - definition->near_distance) > 1.0f)
			fade = 1.0f;
		else
		{
			fade = (definition->far_distance - distance) / (definition->far_distance - definition->near_distance);
			if (fade <= 0.0f)
				return;
		}
	}

	g_46713c = true;
	g_4670bc = true;
	function_16b10((s_render_reset_state *)g_485b48);

	s_noise_basis bases[2];
	s_noise_point points[257];
	s_liquid_collision collision;
	s_sound_position sound_position;
	long first_values[3];
	long second_values[3];
	vector3f ray;

	for (long i = 0; i < definition->arc_count; i++)
	{
		s_liquid_arc *arc = &definition->arc_list[i];
		if (arc->strip_count <= 0 || arc->copies <= 0)
			continue;
		s_random_draw *draw;
		if (payload->liquid_index != NONE)
			draw = (s_random_draw *)function_c4210(payload->liquid_index, i);
		else
			draw = &g_4c9828;
		long copies = arc->copies > 8 ? 8 : arc->copies;
		if (payload->liquid_index == NONE)
			function_507e0(NULL, (const s_random_draw_definition *)arc, 1.0f / 30.0f);

		bases[0].scale = 1.0f;
		bases[0].origin[0] = payload->position.x;
		bases[0].origin[1] = payload->position.y;
		bases[0].origin[2] = payload->position.z;
		vector3f *up = (vector3f *)bases[0].up;
		vector3f *left = (vector3f *)bases[0].left;
		vector3f *forward = (vector3f *)bases[0].forward;
		*up = payload->first;
		if (arc->flags & 1)
		{
			*left = payload->second;
			cross3f_staged(up, left, forward);
		}
		else
		{
			*left = *g_4687b0;
			cross3f_staged(up, left, forward);
			if (function_30bf0(forward) < 0.0001f)
			{
				*forward = *g_4687a4;
				*left = *g_4687a4;
			}
		}
		bases[1] = bases[0];
		bases[1].origin[0] = payload->first.i * arc->length + bases[0].origin[0];
		bases[1].origin[1] = payload->first.j * arc->length + bases[0].origin[1];
		bases[1].origin[2] = payload->first.k * arc->length + bases[0].origin[2];

		for (long j = 0; j < copies; j++)
		{
			real fraction = 1.0f;
			if (arc->cone_angle != 0.0f)
			{
				real angle = (real)j * 6.2831855f / (real)copies;
				if (arc->period != 0.0f)
					angle = draw->reals[0] * 6.2831855f + angle;
				real radius = (real)tan(arc->cone_angle) * arc->length;
				real along = (real)cos(angle) * radius;
				real across = -((real)sin(angle) * radius);
				bases[1].origin[0] = payload->first.i * arc->length + bases[0].origin[0] + along * forward->i + across * left->i;
				bases[1].origin[1] = payload->first.j * arc->length + bases[0].origin[1] + along * forward->j + across * left->j;
				bases[1].origin[2] = payload->first.k * arc->length + bases[0].origin[2] + along * forward->k + across * left->k;
			}
			if (definition->kind == 1)
			{
				bases[1].origin[0] = payload->endpoint.x;
				bases[1].origin[1] = payload->endpoint.y;
				bases[1].origin[2] = payload->endpoint.z;
				if (arc->length > 0.0f)
				{
					real dx = bases[1].origin[0] - bases[0].origin[0];
					real dy = bases[1].origin[1] - bases[0].origin[1];
					real dz = bases[1].origin[2] - bases[0].origin[2];
					fraction = (real)sqrt(dx * dx + dy * dy + dz * dz) / arc->length;
				}
			}
			else if (arc->flags & 4)
			{
				ray.i = bases[1].origin[0] - bases[0].origin[0];
				ray.j = bases[1].origin[1] - bases[0].origin[1];
				ray.k = bases[1].origin[2] - bases[0].origin[2];
				*(word *)&collision.material = 0xffff;
				if (function_1697c0(0x680000d, (point3f const *)bases[0].origin, &ray,
					function_c4250(payload->liquid_index), NONE, (s_collision_result_1697c0 *)&collision))
				{
					fraction = collision.t;
					bases[1].origin[0] = collision.point.x;
					bases[1].origin[1] = collision.point.y;
					bases[1].origin[2] = collision.point.z;
					if (arc->effect_tag_index != NONE && function_30bf0(&ray) > 0.0001f)
					{
						function_1883e0(arc->effect_tag_index, false, &collision.point, g_47d8e0,
							collision.material, 15, 0, &first_values[0], &second_values[0],
							&first_values[1], &second_values[1], &first_values[2], &second_values[2]);
						for (long n = 0; n < 3; n++)
						{
							if (second_values[n] != NONE)
								function_1765e0(&collision.point, &ray, &collision.plane.n, second_values[n], 0, 0);
							if (first_values[n] != NONE)
							{
								memset(&sound_position, 0, sizeof(sound_position));
								sound_position.position = collision.point;
								sound_position.compressed_forward = vector3d_compress(&ray);
								sound_position.velocity = *g_4687a4;
								sound_position.location = collision.end_location;
								function_1895f0(&sound_position, 1.0f, first_values[n]);
							}
						}
					}
				}
			}
			if (0.0f > fraction)
				fraction = 0.0f;
			else if (fraction > 1.0f)
				fraction = 1.0f;

			real amplitude = noise_curve_value(&arc->amplitude_curve, fraction);
			real alpha_scale = noise_curve_value(&arc->alpha_curve, fraction);
			long count = function_516d0(payload->type != 0, j << 8, (const s_noise_draw_definition *)arc,
				draw, bases, amplitude, points);
			if (count <= 1)
				continue;
			real step = 1.0f / (real)(count - 1);
			for (long m = 0; m < arc->strip_count; m++)
			{
				s_liquid_strip *strip = &arc->strips[m];
				real alpha = (real)payload->opacity * alpha_scale * fade * (1.0f / 255.0f);
				alpha = noise_curve_value(&strip->index_curve, (real)m + draw->reals[1]) * alpha;
				real facing = (real)fabs(g_4b9dac.i * payload->first.i + g_4b9dac.j * payload->first.j +
					g_4b9dac.k * payload->first.k);
				alpha = noise_curve_value(&strip->facing_curve, facing) * alpha;
				for (long k = 0; k < count; k++)
				{
					real t = (real)k * step;
					real shade = function_13b390(&strip->colour_curve, t, 0.0f);
					color4f colour;
					if (strip->colour_curve.data && strip->colour_curve.size > 0)
						unpack_color3f(function_13bc00((s_tag_data const *)&strip->colour_curve, shade),
							(color3f *)&colour.red);
					else
					{
						colour.red = g_4686cc->red;
						colour.green = g_4686cc->green;
						colour.blue = g_4686cc->blue;
					}
					colour.alpha = alpha;
					long blue = real_to_long(colour.blue * 255.0f);
					long green = real_to_long(colour.green * 255.0f);
					long red = real_to_long(colour.red * 255.0f);
					long opacity = real_to_long(colour.alpha * 255.0f);
					points[k].flags = blue | green << 8 | red << 16 | opacity << 24;
					points[k].width = noise_curve_value(&strip->width_curve, t);
					points[k].amount = noise_curve_value(&strip->amount_curve, t);
				}
				function_4f7e0(arc->bitmap_tag_index, (s_501e0 const *)points, count, strip->sequence,
					1.0f, 0.0f, true, !(arc->flags & 8), false);
			}
		}
	}

	if (!g_46713c)
		function_0222d0(D3DRS_SAMPLEALPHA, 0);
	g_46713c = true;
}

// score 626 (was 626): frame 0x1490 versus aligned 0x1474; mode stack home versus CL remains.
// Earlier 638 from 693: ESP realignment and mode convention still differ; native pair results improve length.
// ESP realignment and mode convention still differ; native pair results improve length.
// Frame 0x1490 versus aligned 0x1474 and stacked mode versus CL remain; reduced control differs.
// ESP realignment and mode convention still differ; native pair results improve length.
// Midpoint pair 718; first-phase pair 709; fused output walk 754; sample pointers 735; accumulated pair 702;
// sample and midpoint pairs 693; all three pairs 661 (not kept: the running sum's pair is a one-use copy).
// Independent pass indices 720; additive phase samples 638 kept; curve count-first 693;
// delayed endpoint loads 691; conditional clamp 693. Entry alignment and stacked mode remain.
// Earlier 626 from 638: computed interpolation pair kept; frame 0x1490 versus 0x1474, alignment/mode differ.
// RNG workspace at entry 638; coefficient array at entry 638; real level 640;
// output pair snapshot 623 (copy rejected); scalar horizontal lifetime 633;
// interpolation pair with scalar horizontal 626/2384 (tie, restored).
// DL134: sum/weight fills fast 682/682; both fills 631 full; shared phase/midpoint
// samples 626/626 full; native delta/direct seed offset fast 680/681; all workspaces 631 full;
// smaller-frame exponent input/full-mode-first 641/627 full. Shared samples reach frame
// 0x1478 in the full image but tie; reduced gains do not qualify in the full image.
// Earlier 626 from 638: computed interpolation pair kept; frame 0x1490 versus 0x1474, alignment/mode differ.
// Direct phase masks 1..7: fast 622/634/620/653/608/611/662; native output
// pointers all phases 651 (fast control 680). Both additive phases 691/2306 plain;
// all phases 729/2260 plain. Original computed pairs and body restored.
// computed interpolation pair kept; frame 0x1490 versus 0x1474, alignment/mode differ.
// Reduced control 680: upper endpoint at use 701; both endpoint fields at use 686;
// normalized result pair 704; aggregate computed pairs 680; computed accumulation pair 755.
// All fast forms restored.
// Immutable curve data pointer 702 fast; active-level body guard 683; ordinary level 702;
// data/guard 685; data/ordinary 705; guard/ordinary 702; all three 705; named real falloff
// 688 (reduced control 680). Original body and producer restored.
// Round 127: native basis/output/both 680/680/680; shared displacement helper 694, with native
// position setter 711; sum/weight/both alignments 680/680/680 (reduced control 680). All restored;
// full frame 0x1490 and stacked mode remain.
// Round 171 reduced control 680: short exponent 699; coefficient aggregate 680; symmetric weighted products
// 691; short/aggregate 699; short/products 698; aggregate/products 691; all native boundaries 698; readonly
// coefficients/native boundaries 698.
// Round 174 reduced control 680: native curve reference 680; endpoints after saturation 681;
// named remap span 680; reference/late 681; reference/span 680; late/span 681; all 681;
// all with named scaled range 693. Original curve producer restored; full 626 baseline,
// unaligned 0x1490 frame and stacked mode remain.
// Stride, level seed, random state, seed offset, point count, level count, reciprocal count,
// and random/level seed slot names 680 each (fast control 680); original names restored.
// Round 78: BLOCKED-BY 0x508d0 (the only retail caller, a stub in a contributor range): the build's
// stand-in is our only caller, and with it in the image LTCG neither realigns the frame nor passes the
// flag in a register (the fast image, without the stand-in, realigns either way). Retail's order is
// (seed, definition, draw, basis, amplitude, points) with the flag in CL, i.e. the flag declared last:
// that reorder 625 plain; with `definition->exponent - 2 < 0 ? 0 : definition->exponent - 2` (retail's
// add -2/xor/test/setl sequence exactly) 632 plain; ternary/else-first/read-first exponent forms 687/687/673
// fast. The flag stays a stack load into AL in every form.
// Round 291: Complete paired interpolation partial sums/products 690/692; centered random operands 680;
// partial/product terms with centered operands 690/692; existing native point pair 680; native pair plus
// partial/product and centered terms 690/692 (fast control 680; full 626).
// Round 122: no __stdcall 680 fast, with the flag last 680 fast (still ret 0x1c, flag from the stack);
// `standard` marker 680 plain (realigns, frame 0x148c, 2466 bytes); with whole-array memset fills 666, plus sums
// declared first or renamed 666/666 (sums stay at +0x80 where retail has +0x870).
// Lane 5 r130 (plain): the build's stand-in caller pads its stack (`push ecx`) before pushing all seven
// arguments, so LTCG drops this function's realignment; retail's only caller 0x508d0 is a registered
// callback that does not realign (frame 0x19a4) and loads the flag into CL from its payload, so retail
// realigns here. Ours also folds `* (1.f / 65535.f) * 2.0f` into one multiply where retail keeps
// `fmul (1/65535); fadd st(0), st(0)`. The exponent clamp as `exponent - 2 < 0 ? 0 : exponent - 2` 633;
// the random real as `/ 65535.f` 626 (still folded, the same code); the level counter without volatile
// 652; the weights or the sums cleared by memset 635/635.
// Round 491: native-pair additive/all seeded phase loops, per-sample/shared output pairs 630/762/630/762
// reduced; ordinary-level counterparts 644/778/644/778 (control 680). Best additive loop 757 plain,
// 2239/2384, rejected; original complete body restored.
// Round 292 reduced control 680: complete double accumulation results in all six evaluation
// orders 680 each; native real results 680; symmetric weighted-coordinate parts 691.
// Original complete accumulation and all producers restored; full frame/mode gap remains.
// Round 26 (lane 17, plain; the reduced image realigns, so only plain counts): sums and weights cleared by
// memset 631, as `= {}` 631 (both give retail's whole-array 0x202/0x101 rep stosd fills, register-blind
// differences 331 -> 337); `= {}` with the flag declared last 630 (register-blind 326); the exponent read first
// and clamped by an if 643; the level counter without volatile 652. Still BLOCKED-BY 0x508d0 (a stub).
// @retail 0x516d0
long __stdcall function_516d0(bool reduced, dword seed,
	const s_noise_draw_definition *definition, const s_random_draw *draw,
	const s_noise_basis *basis, real amplitude, s_noise_point *points)
{
	long exponent;
	if (reduced)
	{
		exponent = definition->exponent - 2;
		exponent = exponent < 0 ? 0 : exponent;
	}
	else
		exponent = definition->exponent;
	long count = (4 << exponent) + 1;
	long level_count = exponent + 2;
	real inverse_count = 1.0f / (real)(count - 1);
	__declspec(align(8)) s_noise_pair noise[257];
	s_noise_pair sums[257] = {0};
	real weights[257] = {0};
	long seed_offset = 0;
	/* volatile keeps the level counter in its stack slot; it takes this near miss
	   from score 778 to 734 */
	for (volatile long level = 0; level < level_count; level++, seed_offset += 3)
	{
		if (!(definition->active_levels & (1 << level)))
			continue;
		real phase = draw->phases[level];
		real square = phase * phase;
		real factors[3];
		factors[0] = square * 0.5f - phase + 0.5f;
		factors[1] = phase - square + 0.5f;
		factors[2] = phase * 0.0f + square * 0.5f;
		long stride = (count - 1) >> level;
		dword level_seed = draw->words[level] + seed_offset + seed;
		dword state = level_seed;
		long i;
		for (i = 0; i < count; i += stride)
		{
			s_noise_pair sample;
			sample.x = (random_draw_real(&state) * 2.0f - 1.0f) * factors[0];
			sample.y = (random_draw_real(&state) * 2.0f - 1.0f) * factors[0];
			noise[i] = sample;
		}
		state = level_seed + 1;
		for (i = 0; i < count; i += stride)
		{
			s_noise_pair sample;
			sample.x = noise[i].x + (random_draw_real(&state) * 2.0f - 1.0f) * factors[1];
			sample.y = noise[i].y + (random_draw_real(&state) * 2.0f - 1.0f) * factors[1];
			noise[i] = sample;
		}
		state = level_seed + 2;
		for (i = 0; i < count; i += stride)
		{
			s_noise_pair sample;
			sample.x = noise[i].x + (random_draw_real(&state) * 2.0f - 1.0f) * factors[2];
			sample.y = noise[i].y + (random_draw_real(&state) * 2.0f - 1.0f) * factors[2];
			noise[i] = sample;
		}
		if (stride > 1)
		{
			long half = stride / 2;
			for (i = half; i < count; i += stride)
			{
				s_noise_pair midpoint;
				midpoint.x = (noise[i + half].x + noise[i - half].x) * 0.5f;
				midpoint.y = (noise[i + half].y + noise[i - half].y) * 0.5f;
				noise[i] = midpoint;
			}
			if (stride > 2)
			{
				real inverse_stride = 1.0f / (real)stride;
				s_noise_pair left = noise[0];
				for (i = 0; i < count; i += stride)
				{
					s_noise_pair center = noise[i];
					long right_index = i + half;
					if (right_index > count - 1)
						right_index = count - 1;
					s_noise_pair right = noise[right_index];
					for (long j = 0; j < stride; j++)
					{
						long index = i - half + j;
						if (index < 0 || index >= count)
							continue;
						real fraction = (real)j * inverse_stride;
						real remaining = 1.0f - fraction;
						real middle = fraction * remaining * 2.0f;
						real end = fraction * fraction;
						real start = remaining * remaining;
						s_noise_pair interpolated;
						interpolated.x = end * right.x + middle * center.x + start * left.x;
						interpolated.y = end * right.y + middle * center.y + start * left.y;
						noise[index] = interpolated;
					}
					left = right;
				}
			}
		}
		for (i = 0; i < count; i++)
		{
			real value = noise_curve_value(&definition->weight_curve, (real)i * inverse_count);
			double weight = pow(2.0, -(double)((1.0f - value) * (real)level));
			sums[i].x = (real)(sums[i].x + weight * noise[i].x);
			sums[i].y = (real)(sums[i].y + weight * noise[i].y);
			weights[i] = (real)(weights[i] + weight);
		}
	}
	long i;
	for (i = 0; i < count; i++)
	{
		if (weights[i] > 0.0f)
		{
			real inverse_weight = 1.0f / weights[i];
			sums[i].x *= inverse_weight;
			sums[i].y *= inverse_weight;
		}
		else
		{
			sums[i].x = 0.0f;
			sums[i].y = 0.0f;
		}
	}
	real delta[3];
	for (i = 0; i < 3; i++)
		delta[i] = basis[1].origin[i] - basis[0].origin[i];
	for (i = 0; i < count; i++)
	{
		real fraction = (real)i * inverse_count;
		real vertical = sums[i].y;
		if (definition->bias != 1.0f)
		{
			real linear = (definition->bias + 1.0f) * 0.5f;
			real quadratic = (1.0f - definition->bias) * 0.25f;
			vertical = (linear + vertical * quadratic) * vertical + quadratic;
		}
		for (long axis = 0; axis < 3; axis++)
			points[i].position[axis] = delta[axis] * fraction + basis[0].origin[axis];
		real horizontal_scale = noise_curve_value(&definition->horizontal_curve, fraction) * sums[i].x * amplitude;
		for (long axis = 0; axis < 3; axis++)
			points[i].position[axis] += basis[0].forward[axis] * horizontal_scale;
		real vertical_scale = noise_curve_value(&definition->vertical_curve, fraction) * vertical * amplitude;
		for (long axis = 0; axis < 3; axis++)
			points[i].position[axis] += basis[0].left[axis] * vertical_scale;
		points[i].width = 1.0f;
		points[i].amount = 1.0f;
		points[i].flags = 0;
	}
	return count;
}
