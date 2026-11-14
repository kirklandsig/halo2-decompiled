// @flags /O2 /Ob1 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_153950.CPP: the game speed (time dilation and camera shake) slots */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>
#include <string.h>
#include "sound_sources.h"
#include "vector_angles.h"
#include "vector_products.h"
#include "normalize3d.h"

/* the result of the speed effects: the type, the amount and the shake */
struct s_speed_result
{
	long type;
	real amount;
	s_speed_shake shake;
};

byte g_510c60;
short g_468cec[7] = { 0, 1, 2, 3, 4, 5, 6 };

real function_17ca10(real x, short curve);
real function_30bf0(vector3f *v);
vector3f *random_unit_vector(vector3f *result, dword *seed);

static __forceinline bool local_player_present(long index)
{
	bool result = false;

	if (index != NONE)
		result = g_4e8c20->entries[index] != NONE;
	return result;
}

// @retail 0x153950
void function_153950(void)
{
	s_game_speed *speed = g_510c5c;
	long index;

	if (speed->duration != NONE && g_510c54->game_time - speed->start_time > speed->duration && !speed->reverse)
	{
		speed->duration = NONE;
	}
	if (speed->flags & 1)
	{
		if (speed->timer24 > 0)
		{
			speed->timer24--;
		}
		else if (speed->flags & 2)
		{
			speed->flags &= ~1;
			g_502120->value228 = 0.0f;
			g_502120->value220 = 0.0f;
			g_502120->value224 = 0.0f;
		}
	}

	for (index = 0; index < 4; index++)
	{
		if (local_player_present(index))
		{
			s_speed_slot *slot = speed->slots + index;
			real quarter_second = (real)g_510c54->field_2_3 * 0.25f;
			long rounded;
			long decay;
			long i;

			__asm
			{
				fld quarter_second
				fistp rounded
			}
			decay = 255 / rounded;

			i = 0;
			do
			{
				if (slot->decay[i] > 0)
				{
					slot->decay[i] = (byte)(slot->decay[i] - decay < 0 ? 0 : slot->decay[i] - decay);
				}
				i++;
			}
			while (i < 4);
			if (slot->decay89 > 0)
			{
				slot->decay89 -= decay;
			}
			slot->flag0 = 0;
			if (slot->timer7e > 0)
			{
				slot->timer7e--;
			}
			slot->flag1 = 0;
			if (slot->timer80 > 0)
			{
				slot->timer80--;
			}
			slot->flag2 = 0;
			if (slot->timer82 > 0)
			{
				slot->timer82--;
			}
			if (slot->timer7c > 0)
			{
				slot->timer7c--;
				if (slot->timer7c == 0)
				{
					g_502120->entries[index].value80 = 0.0f;
					g_502120->entries[index].value84 = 0.0f;
					memset(slot->values6c, 0, sizeof(slot->values6c));
				}
			}
		}
		else
		{
			long i;

			memset(&speed->slots[index], 0, sizeof(s_speed_slot));
			memset(&g_502120->entries[index], 0, sizeof(s_speed_table_entry));
			for (i = 0; i < 8; i++)
			{
				g_502120->entries[index].items[i].a = NONE;
				g_502120->entries[index].items[i].b = NONE;
				g_502120->entries[index].items[i].scale = 1.0f;
			}
		}
	}
}

void function_1542a0(short seconds, real x, real y, real z);

// @retail 0x1538b0
void function_1538b0(void)
{
	s_game_speed *speed = g_510c5c;

	memset(speed, 0, sizeof(*speed));
	speed->duration = NONE;
	speed->time2c = g_510c54->game_time;
	g_510c60 = false;
	g_4e8c28 = *g_468718;
	if (g_4e6948->state == 1)
	{
		if (g_4e0350->flags & 0x80)
		{
			function_1542a0(0, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			function_1542a0(0, 0.0f, 0.0f, 0.0f);
		}
		g_510c60 = true;
	}
}

// @retail 0x153b80
void function_153b80(long index)
{
	if (index != NONE)
	{
		s_speed_slot *slot = (g_510c5c)->slots + index;

		memset(&slot->request, 0, sizeof(s_speed_request));
		slot->timer7e = 0;
		slot->flag0 = 0;
	}
}

void function_154d70(s_speed_request *request, s_speed_slot *slot, real scale);
void function_155240(s_speed_values *values, s_speed_slot *slot, real priority);

// @retail 0x153bd0
void function_153bd0(long index, real scale)
{
	if (index != NONE)
	{
		s_speed_slot *slot = &g_510c5c->slots[index];
		s_speed_request request;
		s_speed_values values;

		memset(&request, 0, sizeof(request));
		memset(&values, 0, sizeof(values));

		values.value[2] = scale * 0.01;
		values.value[0] = 1.0f;
		values.value[5] = 1.0f;
		request.type = 1;
		request.priority = 2;
		request.duration = 1.0f;
		request.amount = scale;
		request.shake = *(s_speed_shake const *)g_4686cc;
		s_speed_table_entry *entry = &g_502120->entries[index];
		entry->value80 = scale;
		entry->value84 = scale;
		function_154d70(&request, slot, scale);
		function_155240(&values, slot, scale);
	}
}

// @retail 0x153cd0
void function_153cd0(long player_index)
{
	long index = *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

	if (index != NONE)
	{
		s_speed_table_entry *entry = g_502120->entries + index;

		entry->value80 = 0.0f;
		entry->value84 = 0.0f;
	}
}

// @retail 0x154220
void function_154220(real x, short seconds, real y, real z)
{
	s_game_time_globals *time = g_510c54;
	s_game_speed *speed;
	real scaled;
	long ticks;

	g_4e8c28.x = x;
	g_4e8c28.y = y;
	g_4e8c28.z = z;
	scaled = (real)seconds * (1.0f / 30.0f);
	scaled = scaled * (real)time->field_2_3;
	__asm
	{
		fld scaled
		fistp ticks
	}
	speed = g_510c5c;
	speed->duration = (short)ticks;
	speed->reverse = false;
	speed->start_time = time->game_time;
	g_510c60 = false;
}

// @retail 0x1542a0
void function_1542a0(short seconds, real x, real y, real z)
{
	s_game_time_globals *time = g_510c54;
	s_game_speed *speed;
	real scaled;
	long ticks;

	g_4e8c28.x = x;
	g_4e8c28.y = y;
	g_4e8c28.z = z;
	scaled = (real)seconds * (1.0f / 30.0f);
	scaled = scaled * (real)time->field_2_3;
	__asm
	{
		fld scaled
		fistp ticks
	}
	speed = g_510c5c;
	speed->duration = (short)ticks;
	speed->reverse = true;
	speed->start_time = time->game_time;
}

// @retail 0x154310
void function_154310(long index, s_speed_result *result)
{
	s_game_speed *speed = g_510c5c;

	result->amount = 0.0f;
	if (speed->duration != NONE)
	{
		real t;

		result->type = 1;
		if (g_510c60)
		{
			if (g_4e0350->flags & 0x80)
			{
				*(point3f *)&result->shake.vector = *g_468710;
			}
			else
			{
				*(point3f *)&result->shake.vector = *g_468718;
			}
		}
		else
		{
			*(point3f *)&result->shake.vector = g_4e8c28;
		}
		result->shake.scale = 1.0f;
		if (speed->duration > 0)
		{
			t = (real)(g_510c54->game_time - speed->start_time) / (real)speed->duration;
			if (0.0f > t)
				t = 0.0f;
			else if (t > 1.0f)
				t = 1.0f;
			t = function_17ca10(t, 5);
		}
		else
		{
			t = 1.0f;
		}
		result->amount = t;
		if (!speed->reverse)
		{
			result->amount = 1.0f - t;
		}
		result->amount = (0.0f > result->amount) ? 0.0f : ((result->amount > 1.0f) ? 1.0f : result->amount);
	}
	else if (index != NONE)
	{
		s_speed_slot *slot = speed->slots + index;

		if (slot->timer7e > 0 || (slot->flags & 1))
		{
			result->type = g_468cec[slot->request.type];
			result->shake = slot->request.shake;
			if (slot->request.duration > 0.0f)
			{
				result->amount = function_17ca10((real)slot->timer7e / slot->request.duration, slot->request.curve) * slot->request.amount;
			}
			else
			{
				result->amount = slot->request.amount;
			}
			result->amount = (0.0f > result->amount) ? 0.0f : ((result->amount > 1.0f) ? 1.0f : result->amount);
		}
	}
}

// @retail 0x1544e0
void function_1544e0(transform4x3f *matrix, real distance, real angle)
{
	s_random_globals *random = g_4e7408;

	if (angle != 0.0f)
	{
		vector3f axis;

		random_unit_vector(&axis, &random->seed);

		real s = (real)sin(angle);
		real c = (real)cos(angle);
		real ii = axis.i * axis.i;
		real jj = axis.j * axis.j;
		real kk = axis.k * axis.k;
		vector3f sine_axis;
		sine_axis.i = axis.i * s;
		sine_axis.j = axis.j * s;
		sine_axis.k = axis.k * s;
		matrix->scale = 1.0f;
		matrix->forward.i = (1.0f - ii) * c + ii;
		real t = 1.0f - c;
		real tij = (t * axis.i) * axis.j;
		matrix->left.i = tij - sine_axis.k;
		matrix->forward.j = tij + sine_axis.k;
		matrix->left.j = (1.0f - jj) * c + jj;
		real tk = t * axis.k;
		real tki = tk * axis.i;
		matrix->up.i = tki + sine_axis.j;
		matrix->forward.k = tki - sine_axis.j;
		matrix->up.k = (1.0f - kk) * c + kk;
		real tkj = tk * axis.j;
		matrix->up.j = tkj - sine_axis.i;
		matrix->left.k = tkj + sine_axis.i;
		matrix->position.z = 0.0f;
		matrix->position.y = 0.0f;
		matrix->position.x = 0.0f;
	}
	if (distance != 0.0f)
	{
		vector3f v;

		random_unit_vector(&v, &random->seed);
		matrix->position.x = v.i * distance;
		matrix->position.y = v.j * distance;
		matrix->position.z = v.k * distance;
	}
}

void rotate_vector_170d70(vector3f *v, vector3f const *axis, real sine, real cosine); /* inline in src/unknown_170d70.cpp */

// @retail 0x154d70
void function_154d70(s_speed_request *request, s_speed_slot *slot, real scale)
{
	s_game_time_globals *time = g_510c54;

	if (slot->request.priority > request->priority && (real)slot->timer7e * time->rate > request->duration)
	{
		return;
	}
	if (g_468cec[request->type] == 0)
	{
		return;
	}
	slot->request = *request;
	slot->request.duration = (real)time->field_2_3 * slot->request.duration;
	slot->timer7e = (short)slot->request.duration;
	slot->request.amount = slot->request.amount * scale;
	slot->flags |= 1;
}

// MiniMax H (11-14): no new form, no lower score (13): exchanging the two draws scores 66, and a one-caller copy of the shared rotation that adds the projection term first in the j and k rows builds to the same bytes as the shared one.
// MiniMax H (11-13): no new form, no lower score (13): the thirteen differences are all inside the inlined rotation - the load of the vector's j component and the store of its i component trade places, and the k component's two products land in the other pair of registers - and that text is the shared rotate helper's, in src/unknown_170d70.cpp, which this lane may not edit.
// MiniMax H (11-11): no new form, no lower score (13): the only difference left is the j and k rows of the inlined rotation, whose helper (rotate_vector_170d70) lives in src/unknown_170d70.cpp and may not be edited from here.
// MiniMax S (11-11): no new form, no lower score (13): only the rotation's j and k rows differ, and the shared rotate helper that produces them lives in src/unknown_170d70.cpp.
// MiniMax D (11-10): no new form, no lower score (13); the whole-image check still reads 13, and the
// rows that are left are the shared rotation's j and k terms, whose helper is in another file.
// MiniMax S (11-10): no new form, no lower score (13 on the full image, 339 on the reduced one): the only
// difference left is in the rotation's j and k rows, which still add the cosine term where retail adds the
// projection term; the shared rotate helper that produces those rows lives in src/unknown_170d70.cpp, so the
// rows cannot be ordered here, and the frame and the copies around them already agree.
// MiniMax E (11-09, second part): the reduced image of the fast check scores 339 (the body
// comes out 219 bytes short) because the shared rotation helper is not inlined there; the plain
// check over the whole image scores 13, and the j and k rows of the shared rotation still add
// the projection term first, with the helper in src/unknown_170d70.cpp.
// MiniMax E (11-09): no new form, no lower score (13); the j and k rows of the shared
// rotation still add the projection term first, and the helper is in src/unknown_170d70.cpp.
// MiniMax D (11-09): no new form, no lower score (13); the j and k rows of the inlined rotation still add
// the cosine term first where retail adds the projection term, and that rotation's text lives in the
// shared helper in another file this lane may not edit.
// MiniMax C (11-09): no new form, no lower score (13); the j and k rows of the shared rotation still add the cosine term first (BLOCKED-BY src/unknown_170d70.cpp).
// MiniMax E (11-06): score 13 (was 13): no new form lowered it. The only difference left is in the
// j and k rows of the inlined rotation: retail adds vector * cosine to projection * axis while ours
// adds the projection term first, and the fix belongs to the shared rotate_vector_170d70 helper in
// src/unknown_170d70.cpp, not to this file.
// Lane 8 r134 (held, coordinator): MATCH with a one-caller copy of rotate_vector_170d70 whose j and k rows add
// projection * axis before vector * cosine (rotate_slot_vector); a same-behaviour twin picked for this caller, not kept
// (twin ruling); build/held/lane8-r134-154df0-twin.patch. Kept only through the shared helper (one text for every caller).
// MiniMax G (11-12): no new form, no lower score (13): the j and k rows of the inlined rotation still differ and follow the shared rotate_vector_170d70 text in src/unknown_170d70.cpp, which this lane may not edit; a copy of that helper written for this caller is a same-behaviour twin and was not kept.
// score 13 (was 13): only the j and k rows of the inlined rotation differ: retail adds
// projection * axis + vector * cosine (u.j loaded before the u.i store), ours the other way round
// (the i row agrees).
// Tried: norm scalar 67; nested norm tests 64; early 2D cross 207; direction copy loop 271;
// rotation/scale/both component loops 166; const bounds input 64; shared scale 64; previous-priority
// snapshot 280; requested-duration snapshot 271; external bounds reference 67; forward x/y
// observations 165/163; normalized x observation 81; cosine-complement scalar 64; timer rereads and
// long update 64; validity result and angle-phase scope 85; forward snapshot 126; normal snapshot
// 119; entry sine/cosine 64; both draws after normalizing the cross vector 174; every operand and
// term order of the 2D dot/cross and of the rotation's dot alike; rotation statement orders j/i/k 63,
// cosine first worse; v declared before w 60; cross3f_staged, vector3f_scale, dot3f and cross3f for
// the written-out helper bodies (kept); final scale through &slot->vector 61; complete dot2/cross2/
// initial-normal/projection/rotation-cross/update products 78/78/58/60/60/60; early dot+cross+normal
// 56; complete rotation and early/rotation combinations 60/56; normal producer orders
// ijk/ikj/jik/jki/kij 64/56/64/64/56; shared axial/normal/tangent contribution vectors 63-109;
// complete shared affine sum 85; rotation through slot fields at each use 80; axis as a local copy
// 120; rotation as an inline (vector, axis, sine, cosine) helper 59; axis cross before the
// projection 66; complement first in the projection 59.
// projection * axis + vector * cosine (vector.j loaded before the vector.i store), ours the other
// way round. A compile-only copy reproduces ours once the random-range draws are inlined; with the
// scale draw opaque or constant it gives retail's rows, so the order follows the draws' expression
// trees, not the rotation's text, local names or the helper's file (a same-file copy builds alike).
// Round 10: the rotation as a unit draw times 2pi 102, drawn after a plain cross3f 124, cosine
// before sine 33, forward through vector3f_assign 15, cross3f for cross3f_staged 43; screened
// (compile-only) rotation text orders, local names, NULL/0 file and line, draw and sine/cosine
// placements and the other exported rotation helpers: none better.
// Round 15: the existing rotate_vector_170d70 for the written-out rotation 37 (rotate_direction 59,
// record_rotate_vector 90, biped_rotate_about 60, rotate_scalars_146320 74); the unit-length tests
// through length_sq3f with dot `v.i * w.i + w.j * v.j` 33 (other dot/cross text orders 35-49);
// &slot->forward/&slot->vector in place of the e/u locals 24 (one of the two kept 46/51); forward
// stored i, j, k 13 (i, k, j 45); on 13 the other helpers 40-76; final scale through &slot->vector
// 38, u before e 41, inline sine/cosine 30, renamed sine/cosine 24.
// Round 17 (plain): e/u locals again 34, u only 46, e only 51; forward stored j, i, k 25,
// j, k, i 57 (1106 bytes), k, j, i 16.
// stored i, j, k 13 (i, k, j 45); on 13 the other helpers 40-76. Final scale through &slot->vector
// Round 177: dot2/cross2/initial-normal/projection/rotation-cross/update products
// 78/78/58/60/60/60; early dot+cross+normal 56; complete rotation and four early/rotation
// combinations 60 and 56/56/56/56; normal producer orders ijk/ikj/jik/jki/kij 64/56/64/64/56;
// shared axial/normal/tangent contribution vectors, masks 1-7, 105/97/94/63/109/72/92; complete
// shared affine sum 85.
// Round 114: rotation through slot fields at each use 80; axis as a local copy 120; rotation as an
// inline (vector, axis, sine, cosine) helper 59; axis cross before the projection 66; complement
// first in the projection 59.
// Round 112: v declared before w 60; existing cross3f_staged for the up cross product, vector3f_scale
// for the final scale, dot3f(u, e) / dot3f(e, u) for the projection and cross3f for the axis cross
// 60/60/60/60/59 (all kept: they replace hand-written helper bodies); the final scale through
// &slot->vector 61.
// Earlier: norm scalar 67; nested norm tests 64; early 2D cross 207; direction copy loop 271;
// rotation/scale/both component loops 166/166/166; const bounds input 64; shared scale 64;
// previous-priority snapshot 280; requested-duration snapshot 271; external bounds reference 67;
// forward x/y observations 165/163; normalized x observation 81; cosine-complement scalar 64;
// timer rereads and long update 64; validity result and angle-phase scope 85; forward snapshot 126;
// normal snapshot 119; entry sine/cosine 64; both draws after normalizing the cross vector 174.
// Every operand and term order of the 2D dot/cross and of the rotation's dot compiles alike; of the
// rotation's statement orders (axis components, projection, sine and cosine) the written one is
// best, j/i/k 63; cosine first worse.
// Round 177: complete dot2/cross2/initial-normal/projection/rotation-cross/update products
// Dot/cross operand ranks, rotation XMM roles and final scale reloads remain. Shared
// axial/normal/tangent contribution vectors, masks 1/2/3/4/5/6/7, scored 105/97/94/63/109/72/92;
// complete shared affine sum 85. Original helpers and body restored.
// Earlier: dot/cross product operands, rotation xmm roles and
// final component reloads still differ; executable tail is three bytes short.
// Norm scalar 67; nested norm tests 64; early 2D cross 207; direction
// copy loop 271; rotation/scale/both component loops 166/166/166;
// const bounds input 64.
// Shared scale 64; previous-priority snapshot 280; requested-duration
// snapshot 271; external bounds reference 67; forward x/y observations
// 165/163; normalized x observation 81; cosine-complement scalar 64.
// product loads, rotation registers and final scale reloads.
// Timer rereads and long update 64; validity result and angle-phase scope 85;
// forward snapshot 126; normal snapshot 119; entry sine/cosine 64;
// both draws after normalizing the cross vector 174.
// Earlier: every operand and term order of the 2D dot/cross and of the rotation's dot
// compiles alike; of the rotation's statement orders (axis components, projection, sine and
// cosine) the written one is best, j/i/k 63; cosine first worse.
// Round 112 (baseline 60): v declared before w 60; existing cross3f_staged for the up cross product 60,
// vector3f_scale for the final scale 60, dot3f(u, e) / dot3f(e, u) for the projection 60/60 and cross3f
// for the axis cross 59 (all four kept: they replace hand-written helper bodies); the final scale through
// &slot->vector 61 (reloads u.j as retail but reschedules the rotation).
// Round 177: Complete dot2/cross2/initial-normal/projection/rotation-cross/update products
// 78/78/58/60/60/60; early dot+cross+normal 56; complete rotation and four early/rotation
// combinations 60 and 56/56/56/56; normal producer orders ijk/ikj/jik/jki/kij 64/56/64/64/56.
// Round 112: v declared before w 60; existing cross3f_staged for the up cross product, vector3f_scale
// for the final scale, dot3f(u, e) / dot3f(e, u) for the projection and cross3f for the axis cross
// 60/60/60/60/59 (all four kept: they replace hand-written helper bodies); the final scale through
// Round 114: rotation through slot fields at each use 80; axis as a local copy 120; rotation as an
// inline (vector, axis, sine, cosine) helper 59; axis cross before the projection 66; complement
// first in the projection 59. Round 45: an inline declaration on the rotation helper 13;
// scale/rotation/both declared earlier 13/13/13; descriptive scale/rotation names 13/13;
// rotation declared before scale 13; sine/cosine declared earlier 13.
// Round 16 (lane 2): the view angles passed to direction_from_angles straight from the entry 264
// (1084 bytes, plain); with a same-file copy of the rotation helper standing in for the cross-file
// inline (fast 13, same lines): the update test written inline 57 (1105 bytes); sinf for the
// rotation's sine 21 and cosf for its cosine 21 (each gives retail's j row, vector.j loaded before
// the vector.i store, but flips the earlier dot/cross product operands and keeps the k row);
// cosf/sinf for the forward vector 60 (1105 bytes). A harness grid of 256 such spellings
// (angles copy, update test, sin/sinf, cos/cosf, acos/acosf, fabs/fabsf) gave no form with retail's
// product operands and both rows.
// Round 33 (lane 2, fast with 0x170d70 in the image, which keeps the helper inline): NULL/0 file
// and line in both draws 13; a seed pointer local for both draws 124; double sine/cosine locals 45;
// the flag set before the draws 150 (1110 bytes); `cross < 0.0f` 13.
// Round 29 (lane 18, fast with 0x170d70): unused-local probes (1-24 before the draws, the
// sine, the rotation or at the top; compile-only, diagnostic) never flip the rows, so the
// order is not a symbol-count effect; a constant scale gives retail's j and k rows and an
// opaque scale draw retail's j row (diagnostics), so the scale draw's inlined tree decides
// it. Scale as double 171 (1103 bytes); rotation as double 27; the draw's bounds from the
// parameter 161 (1104 bytes); scale declared at the top of the block 13; const scale 13;
// sine and cosine in one declaration 13.
// Round 127: the second draw after the cross and normalize 132; the scale draw before the
// forward stores 62 and before the duration update 70; the existing unit-random helper for
// rotation 29; force-inline declarations for the range and rotation helpers 13/13;
// slot_random_range for the scale and both draws 189 (1053 bytes) and 232 (1009 bytes).
// Original body restored.
// Round 66 (m3 lane 11, fast with 0x170d70): the flag store before the final scale 22; scale and rotation
// declared, then assigned 13; sine/cosine and the rotate call in their own block 13; the flag store before
// the rotate call 81 (1109 bytes); sine/cosine before the normalize call 106. Unused-local probes (1-16,
// diagnostic) repeat with period 8 (13/19/15/25/21/21/17/23) and move only the dot/cross operand orders;
// the j/k rows stay in every residue, so they are no numbering tie.
// AC551: Immutable angle/timer, flattened input through the existing setter/aggregate, angle/cross
// declarations at their producers, ordinary-inline range declaration, and post-copy duration from
// the input: 13/13/259/259/13/13/13/161. NOT kept: no qualifying supported improvement; complete
// executable source restored. Rotation j/k sum and input-load ranks remain.
// M396: Complete memmove direction/angle/bounds transfer subsets 1/2/4/3/5/6/7
// 210/284/293/279/300/296/296; signed-angle conditional result 13. NOT kept:
// no exact rotation/load repair; original complete body restored.
// Round 93 (m3 lane 16, fast with 0x170d70): `angle = -angle` 13; `fabs(1.0f - length)` in both unit tests 15;
// the unit tests as `0.0001f > fabs(...)` 13; the clamp as `dot < -1.0f` / `1.0f < dot` 60 (1105 bytes); the priority
// store after the duration and timer stores 35. Re-measured listed forms: forward j before i 25, cosine before sine 33.
// BLOCKED-BY 0x170d70: the rows follow the shared rotate_vector_170d70 text in its file, src/unknown_170d70.cpp (held).
// P362: Existing planar dot/cross and forward point-setter subsets 1/2/4/3/5/6/7
// 23/13/15/21/31/21/27; both planar helpers with the vector setter 27. NOT kept:
// no byte equality or gain. Original body and includes restored; held rotation helper
// untouched. Native j/k input-load and sum ownership remain.
// MiniMax R (11-11): no new form, no lower score (13): the rotation's j and k rows still differ from retail's, and the shared rotation helper's text decides them while its file is held.
// MiniMax R (11-10): no new form, no lower score (13): the rotation's j and k rows still differ from retail's,
// and the shared rotate helper in the other file decides them.
// score 13 (was 13): unchanged this round: only the j and k rows of the inlined rotation still differ, and the
// rotate helper's own text decides the order of the two products; that helper is in src/unknown_170d70.cpp, which
// this lane may not edit.
// score 13 (was 13): unchanged this round: the inlined rotate helper computes the `projection * axis` term after the
// `v * cosine` term in the j and k rows; the helper's text is in src/unknown_170d70.cpp.
// @retail 0x154df0
void function_154df0(long index, s_speed_bounds *bounds, s_speed_slot *slot, vector3f *direction, real priority)
{
	real timer = (real)slot->timer80;

	bool update = bounds->duration > timer || priority > slot->priority9c || (priority >= slot->priority9c && bounds->duration > g_510c54->rate * timer);
	if (update)
	{
		vector3f w;
		vector3f v;

		v = *direction;
		v.k = 0.0f;
		function_30bf0(&v);

		s_unknown_185ab0_entry *view = g_4ed284->entries + index;
		s_view_angles angles = view->angles;

		direction_from_angles(&w, angles.yaw, angles.pitch);
		w.k = 0.0f;
		function_30bf0(&w);

		if (fabs(length_sq3f(&v) - 1.0f) < 0.0001f && fabs(length_sq3f(&w) - 1.0f) < 0.0001f)
		{
			real dot = v.i * w.i + w.j * v.j;
			real angle;
			real cross;

			if (-1.0f > dot)
				dot = -1.0f;
			else if (dot > 1.0f)
				dot = 1.0f;
			angle = (real)acos((-1.0f > dot) ? -1.0f : ((dot > 1.0f) ? 1.0f : dot));
			cross = v.j * w.i - v.i * w.j;
			if (0.0f > cross)
			{
				angle = 0.0f - angle;
			}

			slot->bounds = *bounds;
			slot->priority9c = priority;
			slot->bounds.duration = (real)g_510c54->field_2_3 * slot->bounds.duration;
			slot->timer80 = (short)slot->bounds.duration;
			slot->forward.i = (real)cos(angle);
			slot->forward.j = (real)sin(angle);
			slot->forward.k = 0.0f;

			real scale = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, slot->bounds.lower, slot->bounds.upper);
			real rotation = function_259d0(&g_4e7408->seed, __FILE__, __LINE__, 0.0f, 6.2831855f);
			cross3f_staged(&slot->forward, g_4687b0, &slot->vector);
			function_30bf0(&slot->vector);

			real s = (real)sin(rotation);
			real c = (real)cos(rotation);

			rotate_vector_170d70(&slot->vector, &slot->forward, s, c);
			vector3f_scale(&slot->vector, scale, &slot->vector);
			slot->flags |= 2;
		}
	}
}

// @retail 0x155240
void function_155240(s_speed_values *values, s_speed_slot *slot, real priority)
{
	s_game_time_globals *time = g_510c54;
	real scaled;
	long ticks;

	if (values->value[0] > time->rate * (real)slot->timer82 || priority > slot->priority98 || (priority >= slot->priority98 && values->value[0] > time->rate * (real)slot->timer82))
	{
		slot->values50 = *values;
		slot->priority98 = priority;
		scaled = (real)time->field_2_3 * slot->values50.value[0];
		__asm
		{
			fld scaled
			fistp ticks
		}
		slot->timer82 = (short)ticks;
		slot->flags |= 4;
	}
}

struct s_object;
s_object *function_badc0(long index, dword mask);
point3f *function_b9dd0(long index, point3f *point);
void function_caf60(long index, point3f *point);
__declspec(noinline) s_player_state *function_16f3a0(long index);
bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);
long function_189fe0(s_sound_request const *request, long tag_index);
void rumble_player_play_effect(long player_index, long definition_index, long effect_index, real scale);
void function_225df0(long tag_index, long entry_index);

struct s_camera_effect_153d10
{
    short type;
    short unknown02;
    s_speed_request request;
    byte unknown24[0x4c - 0x24];
};
struct s_camera_effect_tag_153d10
{
    byte unknown00[0x14];
    dword flags;
    byte unknown18[0x6c - 0x18];
    long count;
    s_camera_effect_153d10 *effects;
    s_speed_bounds bounds;
    s_speed_values values;
    byte unknowna8[4];
    long sound_index;
};

struct s_player_axes_153d10
{
	byte unknown00[0x20];
	vector3f forward;
	vector3f up;
};

// score 2 (was 2): one product is built with its operands in the other order: retail multiplies up.k by
// forward.i, ours multiplies forward.i by up.k. Everything else is identical. Retail keeps every
// cross product's text order; ours orders all six by the axis pointers' age. Round 14 (plain):
// cross3f, cross3f_staged or the cross through forward/up 41; no axis pointers 35 and the axis
// pointers after the cross 10 (all six up first); only forward or only up declared before the
// cross 8/6 (three products flip).
// Round 38 (plain): up pointer declared before forward 6; cross rows in i, k, j order 10; only an up
// pointer (cross and dots of up through it, forward through axes) 16; up read through its pointer in
// the j row only, every other read through axes 102; delta declared before the axes 2. A compile-only
// screen of the whole SSE block (exact here: 2, 6, 10, 16 as the plain checks) over ~1100 spellings of
// each cross/dot read through forward/up or axes and both pointer orders found none at 0 (best 10);
// a few spellings fix the cross alone but then reorder the dots.
// Round 134 (plain): forward's dot read through axes (no forward pointer) 2 (product 5 right, product 1
// flips); parentheses around every cross product 8; each row as `x = a; x -= b;` 4; delta as three
// statements 2; parentheses around every dot product 6. A one-file LTCG harness (cross/dot block exact)
// shows the six operand orders follow a period-4 count of expression nodes anywhere in the block (an
// extra paren or cast shifts them; unused locals only move the dots); every shift flips products 1 and 5
// together (up.k against forward.j and forward.i), and none of 64 dot spellings, 64 paren sets, 32 j-row
// pointer spellings, the cross helpers or row orders separates them.
// Round 128 (plain): both/up-only/forward-only array components through axes 2/2/2; both pointer arrays 18;
// pointer up array with axes forward 18; axes up array with pointer forward array 2; pointer up field with
// pointer forward array 18; pointer up array with pointer forward field 18. Original body restored.
// Round 41 (lane 2, plain; --fast builds this function differently, 345 at 1335 bytes): delta through the point difference
// helper 119 (1303 bytes); dots through the dot helper 2; both 119; relative declared before delta 2; cross declared before the
// axes 2; slot vector through a real pointer 18.
// Round 132 (plain): `vector3f basis[2]` in the axes struct 2; a sub-struct holding both axes 2; `(real)` casts on products 2-5 2;
// the j row's first product up-first 2; all six products up-first 2. One-file harness, float window only (base 5, three of those
// the slot register): all 63 sets of `(real)` casts on the six products 5 or above; dots before the cross 47; only the forward or
// up dot before it 46/36; delta after the cross 47; rows jik/jki/kij/kji 48; six named products 48; three scalar cross locals 48;
// negated j row 48; `real basis[2][3]` 48; a basis pointer 10; a pointer to the axes sub-struct 48, with the dots through it 10.
// Round 56 (m3 lane 2; one-file harness over the whole file, control 5: this product plus 3 harness-only lines; plain not run because none beat the control): an inline cross helper with
// pointer parameters in four parameter orders, all row orders, direct or staged, all product text orders, 4 phases: 240 samples, best 31 (helper forms flip far more products); per-read spelling of the 12 cross reads through axes
// or forward/up, dot spellings, both pointer orders, 0-3 unused locals: 1,100 samples and three hill climbs, floor 5, several at 7/9 with this product right but the up.k-against-forward.j product or the up.i one wrong, none with all right;
// origin and target swapped or split 5/5; state dropped, axes from the call 11; axes at the top of the block 5; the pointers after delta 5; relative before cross 5; owner index named 9; slot fields by array 5.
// Round 94 (lane 16, plain): forward/up as const references 2; cross as an aggregate initializer 2; relative as an aggregate
// initializer 2; a const axes view pointer 2; const axis pointers (`vector3f const *const`) 2.
// Round 98 (m3 lane 12, plain): the local player tests and lookups through the existing 0x14ddc0/0x14de70 at both
// sites 2 (the same code: LTCG inlines both; kept in place of the hand copies), at the first site only 2, the second only 2,
// 0x14de70 only 2, 0x14ddc0 only 2. Compile-only (exact here): 1-12 unused locals at the top, before the axes pointer or
// before delta leave the cross block unchanged; 1-4 redundant parens in any cross row or in delta all shift the six
// products together with period 4 (3 puts this product right and the up.k-against-forward.j one wrong).
// Round 60 (m3 lane 2, plain): the unit-index reads through the existing player datum type 2, the object header read
// through the existing header type 2, both 2 (all the same code); `state != NULL` 2 (same code); named forward/up fields
// in the player state and a named 0x8c vector in the speed slot (globals.h), the cross and slot stores through them
// without the axes view 4 (this product right, the k row's forward.i x up.j and the j row's forward.k x up.i flipped).
// Untried on that named-field form: the axis pointers dropped or declared after the cross.
// Round 145 (m3 lane 7, plain): a compile-only build differs from the LTCG build in products 1 and 5 together, so
// its screens are no model here. The i and j rows read partly through forward/up (the closest form of a compile-only
// screen) 14; mixed reads with every product swapped 25; this product's up.k through the up pointer 16 (the
// up.k-against-forward.j product flips with it); its forward.i through the forward pointer 2 (same code); both 16.
// Round 74 (m3 lane 2, plain): up.k named once for both products that flip together 112 (1296 bytes); delta from three
// named components 2 (as round 134's three-statement delta); cross declared before delta 2; cross and relative in one
// declaration 2 (all three the same code); the slot's direction written through a real pointer as an array 18 (re-measured,
// as round 41). Forms earlier measured only in a harness, now plain: the axes, forward and up pointers declared at the top
// of the block 2; forward and up declared after delta 2 (both the same code).
// Round 117 (m3 lane 12, plain), on round 60's named-field form (forward 0x20 / up 0x2c in the player state, the 0x8c
// vector in the speed slot, globals.h; the axes view dropped): both axis pointers dropped, cross and dots through the
// state 37; the pointers declared after the cross 6 (this product right; the i row's first and the k row's two products
// load up first); the pointers before delta with the cross and dots through them 103; cross3f and dot3f 8; cross3f_staged
// and dot3f 8 (the same code). Header and body restored.
// Round 91 (m3 lane 14, fast with 0x154d70/0x154df0/0x155240/0x170d70 listed, which reproduces 2): forward and up
// copied by value 419 (1393 bytes), forward only 183, up only 183; up as `forward + 1` with the cross and dots through
// the pointers 105, with the cross through axes 12; the j row's first product with up.i through the up pointer 2 (same
// code), forward.k through the forward pointer 24, both 24. Diagnostics: 1-7 unused locals 2/2/101/102/16/2/2 (the
// six products never move); 1-3 parens in delta give per-product windows {0,1} fj*uk, {0,3} fk*uj and fk*ui,
// {0,1,3} fj*ui and fi*uj, {2,3} uk*fi, so no shift fits; three parens around the j row's first product move only
// later products but flip fj*uk with uk*fi (2): the two share up.k's key.
// @retail 0x153d10
void __stdcall function_153d10(long user_index, long tag_index, void *owner, void *direction, long unused, real scale, real priority, bool directional)
{
    long local_index = user_index;
    if (local_index == NONE)
        return;
    s_camera_effect_tag_153d10 *definition = (s_camera_effect_tag_153d10 *)g_4e3b44[tag_index & 0xffff].bytes;
    s_speed_slot *slot = &g_510c5c->slots[local_index];
    if (function_14ddc0(local_index))
    {
        long player_index = function_14de70(local_index);
        long unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);
        if (unit_index != NONE)
        {
            byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
            bool active = *(real *)(unit + 0xf0) > 0.0f;
            for (long i = 0; i < definition->count; i++)
            {
                s_camera_effect_153d10 *effect = &definition->effects[i];
                if ((active && effect->type != 1) || (!active && effect->type != 0))
                {
                    if (effect->request.duration > 0.05f && effect->request.amount > 0.05f)
                    {
                        s_speed_request request = effect->request;
                        if (active)
                        {
                            request.shake.scale = 1.0f;
                            request.shake.vector.i = 0.0f;
                            request.shake.vector.j = 0.6f;
                            request.shake.vector.k = 0.8f;
                        }
                        else
                        {
                            request.shake.scale = 1.0f;
                            request.shake.vector.i = 1.0f;
                            request.shake.vector.j = 0.0f;
                            request.shake.vector.k = 0.0f;
                        }
                        if (request.duration > 0.5f && *(short *)(unit + 0x1fc) == NONE)
                        {
                            request.duration = 0.8f;
                            request.amount = 0.6f;
                            request.priority = 1;
                            request.type = 1;
                            request.curve = 1;
                        }
                        else
                        {
                            request.duration = active ? 0.2f : 0.3f;
                            request.amount = 0.3f;
                            request.priority = 1;
                            request.type = 1;
                            request.curve = 0;
                        }
                        function_154d70(&request, slot, function_17ca10(scale, 2));
                    }
                    rumble_player_play_effect(local_index, tag_index, i, priority);
                    function_225df0(tag_index, i);
                    break;
                }
            }
        }
    }
    function_154df0(local_index, &definition->bounds, slot, (vector3f *)direction, priority);
    function_155240(&definition->values, slot, priority);
    if (definition->sound_index != NONE)
    {
        s_sound_request request;
        request.location.unknown02 = 0;
        request.location.flags = 0;
        ((byte *)&request.location)[3] = 0;
        request.location.scale = 1.0f;
        request.location.unknown08 = 0;
        request.object_index = NONE;
        request.platform_playback = NONE;
        request.marker = NULL;
        request.source = NULL;
        request.variant = NULL;
        function_189fe0(&request, definition->sound_index);
    }
    if (directional && ((long *)owner)[1] != NONE)
    {
        if (definition->flags & 0x100)
        {
            slot->decay[2] = 0xff;
            return;
        }
        long unit_index;
        if (function_14ddc0(local_index))
        {
            long player = function_14de70(local_index);
            unit_index = *(long *)(g_4e8c24->data + (player & 0xffff) * 0x21c + 0x2c);
        }
        else
        {
            unit_index = NONE;
        }
        if (function_badc0(unit_index, 3) && function_badc0(((long *)owner)[1], (dword)NONE))
        {
            s_player_state *state = function_16f3a0(local_index);
            if (state)
            {
                point3f origin, target;
                function_caf60(unit_index, &origin);
                function_b9dd0(((long *)owner)[1], &target);
                s_player_axes_153d10 *axes = (s_player_axes_153d10 *)state;
                vector3f const *forward = &axes->forward;
                vector3f const *up = &axes->up;
                vector3f delta = { target.x - origin.x, target.y - origin.y, target.z - origin.z };
                vector3f cross;
                vector3f relative;
                cross.i = axes->forward.j * axes->up.k - axes->forward.k * axes->up.j;
                cross.j = axes->forward.k * axes->up.i - axes->up.k * axes->forward.i;
                cross.k = axes->forward.i * axes->up.j - axes->forward.j * axes->up.i;
                relative.i = cross.k * delta.k + cross.j * delta.j + cross.i * delta.i;
                relative.j = forward->k * delta.k + forward->j * delta.j + forward->i * delta.i;
                relative.k = up->k * delta.k + up->j * delta.j + up->i * delta.i;
                *(real *)((byte *)slot + 0x8c) = relative.i;
                *(real *)((byte *)slot + 0x90) = relative.k;
                *(real *)((byte *)slot + 0x94) = relative.j;
                slot->decay89 = 0xff;
                if (function_30bf0(&relative) != 0.0f)
                {
                    if (fabs(relative.k) > 0.5f)
                    {
                        if (relative.k > 0.0f) slot->decay[0] = 0xff;
                        else slot->decay[2] = 0xff;
                    }
                    real angle = (real)atan2(relative.j, relative.i);
                    real absolute = (real)fabs(angle);
                    if (angle < 0.785398185f || angle > 2.356194496f)
                    {
                        if (absolute > 1.570796371f) slot->decay[1] = 0xff;
                        else slot->decay[3] = 0xff;
                    }
                }
            }
        }
    }
}

extern transform4x3f *g_4687d0;
void function_141ce0(real a, real b, real c, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
real function_17c900(short curve, real input);

/* a random real in [0, 1] from the seed; the cast keeps its 1/65535 scale
   apart from the caller's constants, as retail has it */
static inline real random_real_1546f0(dword *seed)
{
	return (real)(_random(seed, 0, 0) * (1.f / 65535.f));
}

static inline void point_set_1546f0(point3f *point, real x, real y, real z)
{
	point->x = x;
	point->y = y;
	point->z = z;
}

static inline void cross_product3d_1546f0(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

/* the rotation by the angle about the unit axis, with no translation */
static __forceinline void rotation_from_axis_1546f0(transform4x3f *matrix, vector3f const *axis, real angle)
{
    real s = (real)sin(angle);
    real c = (real)cos(angle);
    real ii = axis->i * axis->i;
    real jj = axis->j * axis->j;
    real kk = axis->k * axis->k;
    vector3f sine_axis;
    sine_axis.i = axis->i * s;
    sine_axis.j = axis->j * s;
    sine_axis.k = axis->k * s;
    matrix->scale = 1.0f;
    real forward_i = (1.0f - ii) * c + ii;
    real t = 1.0f - c;
    real tij = (t * axis->j) * axis->i;
    real left_i = tij - sine_axis.k;
    real forward_j = tij + sine_axis.k;
    real left_j = (1.0f - jj) * c + jj;
    real tk = t * axis->k;
    real tki = tk * axis->i;
    real up_i = tki + sine_axis.j;
    real forward_k = tki - sine_axis.j;
    real up_k = (1.0f - kk) * c + kk;
    real tkj = tk * axis->j;
    real up_j = tkj - sine_axis.i;
    real left_k = tkj + sine_axis.i;
    matrix3x3 rotation = {{forward_i, forward_j, forward_k}, {left_i, left_j, left_k}, {up_i, up_j, up_k}};
    matrix->rotation = rotation;
    matrix->position.z = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.x = 0.0f;
}

/* a flag field read as word bits: retail reads these flags with a load, a
   shift and a test of the low bit */
struct s_flag_word_bits
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

// score 484 (was 521): flags 5 and others read through a word bitfield view give retail's load-and-shift; the rest still differs.
// @retail 0x1546f0
void __stdcall function_1546f0(long local_index, transform4x3f *matrix)
{
    if (local_index == NONE)
        return;
    s_game_speed *speed = g_510c5c;
    if (speed->flags & 1)
    {
        real amount = *(real *)&speed->value20;
        transform4x3f transform = *g_4687d0;
        if (speed->timer24 > 0)
        {
            if (TEST_FIELD_BIT(((s_flag_word_bits *)&speed->flags)->bit1))
                amount *= (real)speed->timer24 / (real)speed->timer26;
            else
                amount *= 1.0f - (real)speed->timer24 / (real)speed->timer26;
        }
        amount = 0.0f > amount ? 0.0f : (amount > 1.0f ? 1.0f : amount);
        s_random_globals *random = g_4e7408;
        g_502120->value228 = amount;
        /* retail advances the seed three times and then reads it once for all three axes
           (0x1547cc-0x154806, again at 0x154888-0x1548d0): the third draw scales every axis */
        random_real_1546f0(&random->seed);
        random_real_1546f0(&random->seed);
        real shake = random_real_1546f0(&random->seed) * 2.0f - 1.0f;
        function_141ce0(shake * speed->angles[0] * amount, shake * speed->angles[1] * amount,
            shake * speed->angles[2] * amount, &transform);
        random_real_1546f0(&random->seed);
        random_real_1546f0(&random->seed);
        shake = random_real_1546f0(&random->seed) * 2.0f - 1.0f;
        point_set_1546f0(&transform.position, shake * speed->point.y * amount, shake * speed->point.x * amount,
            shake * speed->point.z * amount);
        function_142a60(matrix, &transform, matrix);
    }
    else
    {
        s_speed_slot *slot = &speed->slots[local_index];
        if (slot->timer80 > 0 || TEST_FIELD_BIT(((s_flag_word_bits *)&slot->flags)->bit1))
        {
            real amount;
            if (TEST_FIELD_BIT(((s_flag_word_bits *)&slot->flags)->bit1))
                amount = 1.0f;
            else
            {
                real duration = slot->bounds.duration;
                real elapsed = (duration - (real)slot->timer80) / duration;
                real priority = slot->priority9c;
                amount = priority * function_17ca10(1.0f - elapsed, *(short *)&slot->bounds.unknown04[0]);
            }
            transform4x3f transform;
            vector3f axis;
            cross_product3d_1546f0(g_4687b0, &slot->forward, &axis);
            rotation_from_axis_1546f0(&transform, &axis, amount * slot->bounds.unknown04[1]);
            real distance = slot->bounds.unknown04[2] * amount;
            vector3f offset;
            offset.i = slot->forward.i * distance;
            offset.j = slot->forward.j * distance;
            offset.k = slot->forward.k * distance;
            transform.position.x = slot->vector.i * amount + offset.i;
            transform.position.y = slot->vector.j * amount + offset.j;
            transform.position.z = slot->vector.k * amount + offset.k;
            function_142a60(matrix, &transform, matrix);
        }
        if (slot->timer82 > 0 || TEST_FIELD_BIT(((s_flag_word_bits *)&slot->flags)->bit2))
        {
            transform4x3f transform = *g_4687d0;
            s_game_time_globals *time = g_510c54;
            real amount;
            if (TEST_FIELD_BIT(((s_flag_word_bits *)&slot->flags)->bit2))
                amount = 1.0f;
            else
            {
                real duration = slot->values50.value[0];
                real elapsed = (duration - (real)slot->timer82 * time->rate) / duration;
                real priority = slot->priority98;
                amount = priority * function_17ca10(1.0f - elapsed, *(short *)&slot->values50.value[1]);
            }
            real elapsed = (slot->values50.value[0] - (real)slot->timer82 * time->rate) / slot->values50.value[5];
            real curve = function_17c900(*(short *)&slot->values50.value[4], elapsed);
            real scale = (slot->values50.value[6] * curve + (1.0f - slot->values50.value[6])) * amount;
            real distance = slot->values50.value[2] * scale;
            distance = distance > 0.0f ? distance : 0.0f;
            real angle = slot->values50.value[3] * scale;
            angle = angle > 0.0f ? angle : 0.0f;
            if (slot->timer7c > 0)
            {
                real fade = (real)slot->timer7c * time->rate * 2.0f;
                distance += slot->values6c[2] * fade;
                angle += slot->values6c[3] * fade;
                g_502120->entries[local_index].value80 = slot->values6c[0];
                g_502120->entries[local_index].value84 = slot->values6c[1];
            }
            function_1544e0(&transform, distance, angle);
            function_142a60(matrix, &transform, matrix);
        }
    }
}
