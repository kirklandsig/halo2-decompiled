// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_050690.CPP: random seeds drawn from the game's random generator
   (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
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
