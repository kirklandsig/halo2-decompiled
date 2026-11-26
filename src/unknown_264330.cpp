#include "unknown_11c920.h"
#include <stdlib.h>
#include "globals.h"
#include "unknown_1fb7e0.h"
#include "unknown_25d020.h"
#include "unknown_26b230.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_1df560(short team, short other_team);
void function_25b910(long actor_index, long prop_ref_index);
point3f *function_b9dd0(long object_index, point3f *position);
real function_30bf0(vector3f *vector);
long function_264260(vector3f const *direction, vector3f const *facing, real distance);
short ai_player_index_get(long player_index);
void function_268c60(long actor_index);
long function_25d810(long object_index, long actor_index, bool create);
bool function_25ccd0(s_type_5cfb45 *state, long object_index, short value, s_2640c0 *motion);
void function_265c30(long prop_index, long actor_index, bool active);
bool __stdcall function_265290(long actor_index, long prop_index);
bool __stdcall function_265550(long actor_index, long prop_index);
void function_ba1d0(long object_index, vector3f *linear, vector3f *angular);
void function_cb7e0(long unit_index, vector3f *vector);
__declspec(noinline) long function_10f8f0(long object_index);
struct s_28fb50;
long function_28fb50(long object_index, s_28fb50 const *context, long target_index);
__forceinline long real_to_long(real value);
long function_10f5f0(long object_index);
real function_10f690(long object_index, real *duration);
long function_1469f0(real seconds);
long function_25d770(long actor_index, long object_index);
bool function_2651e0(long object_index, short *volatile output_index);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);
bool __stdcall function_25d020(long actor_index, long object_index, long type,
	long prop_index, long a, long b);

// @retail 0x265680
void __stdcall function_265680(long actor_index)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	if (*(short *)(actor + 0x358) <= 0) return;
	byte *object = (byte *)function_badc0(*(long *)(actor + 0x360), NONE);
	if (!object) { *(short *)(actor + 0x358) = 0; return; }
	point3f &position = *(point3f *)(actor + 0x370);
	function_b9dd0(*(long *)(actor + 0x360), &position);
	vector3f &velocity = *(vector3f *)(actor + 0x37c);
	velocity = *(vector3f *)(object + 0x88);
	point3f &midpoint = *(point3f *)(actor + 0x3a0);
	s_2641c0 context;
	if (function_2641c0(actor_index, &context, &position))
	{
		vector3f offset;
		vector3d_from_points3d(&context.field_c, &position, &offset);
		*(real *)(actor + 0x394) = (real)sqrt(length_sq3f(&offset));
		real dx = midpoint.x - context.field_c.x;
		real dy = midpoint.y - context.field_c.y;
		real dz = midpoint.z - context.field_c.z;
		*(real *)(actor + 0x398) = dx * dx + dy * dy + dz * dz;
	}
	point3f &prediction = *(point3f *)(actor + 0x388);
	prediction.x = velocity.i * 1.5f + position.x;
	prediction.y = velocity.j * 1.5f + position.y;
	prediction.z = velocity.k * 1.5f + position.z;
	point3f const *origin = (point3f const *)(actor + 0x370);
	midpoint.x = (origin->x + prediction.x) * 0.5f;
	midpoint.y = (origin->y + prediction.y) * 0.5f;
	midpoint.z = (origin->z + prediction.z) * 0.5f;
	vector3f offset;
	vector3d_from_points3d((point3f const *)(actor + 0x3a0), &position, &offset);
	*(real *)(actor + 0x39c) = (real)sqrt(length_sq3f(&offset)) + *(real *)(actor + 0x36c);
	s_record_pool *references = g_502418;
	byte *reference = record_pool_lookup(references, *(long *)(actor + 0x368));
	if (!reference) { *(short *)(actor + 0x358) = 0; return; }
	bool related = false;
	bool attached = false;
	bool active = *(short *)(reference + 0x24) >= 1 && *(short *)(reference + 0x24) <= 2;
	switch (*(short *)(actor + 0x358))
	{
	case 2:
		if (*(long *)(actor + 0x18) != NONE && *(long *)(object + 0x14) == *(long *)(actor + 0x18))
			attached = true;
		if (*(real *)(object + 0x158) > 0.0f && *(real *)(object + 0x15c) > 0.0f)
			*(short *)(actor + 0x3ac) = (short)real_to_long((1.0f - *(real *)(object + 0x158)) / *(real *)(object + 0x15c));
		else *(short *)(actor + 0x3ac) = NONE;
		if (attached && !actor[0x35e])
			function_1fb7e0(0xae, actor_index, NULL, *(long *)(actor + 0x360), NONE);
		break;
	case 4:
		{
			short index = NONE;
			if (function_2651e0(*(long *)(actor + 0x360), &index)) *(short *)(actor + 0x3ac) = index;
			else { *(short *)(actor + 0x3ac) = NONE; *(short *)(actor + 0x358) = 0; }
		}
		break;
	case 3:
		{
			vector3f const &v = *(vector3f *)(object + 0x88);
			byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
			if (0.2f * 0.2f > v.i * v.i + v.j * v.j + v.k * v.k ||
				*(real *)(actor + 0x394) > *(real *)(definition + 4) + 10.0f)
				*(short *)(actor + 0x358) = 0;
			else if (active) goto finished;
			else if (*(long *)(object + 0x248) != NONE)
			{
				long index = function_25d770(actor_index, *(long *)(object + 0x248));
				if (index != NONE)
				{
					byte *other = references->data + (index & 0xffff) * 0x3c;
					active = *(short *)(other + 0x24) >= 1 && *(short *)(other + 0x24) <= 2;
					related = active;
				}
			}
		}
		break;
	case 1:
		if (function_10f5f0(*(long *)(actor + 0x360)) == 0xc000043)
		{
			real duration;
			real elapsed = function_10f690(*(long *)(actor + 0x360), &duration);
			*(short *)(actor + 0x3ac) = (short)function_1469f0(duration - elapsed);
		}
		else *(short *)(actor + 0x3ac) = NONE;
		break;
	}
	if (!active)
	{
		if (related && *(short *)(actor + 0x684) <= 0)
		{
			*(short *)(actor + 0x686) = (short)real_to_long((real)g_510c54->field_2_3 * 2.0f);
			*(short *)(actor + 0x688) = 5;
			*(short *)(actor + 0x684) = 0;
		}
		if (*(short *)(actor + 0x35a) != 0 || attached)
			function_25d020(actor_index, *(long *)(actor + 0x360), 0, *(long *)(actor + 0x368), NONE, NONE);
	}
finished:
	actor[0x35e] = attached;
	if (attached)
	{
		actor[0x35c] = 0;
		function_1a8220(actor_index, 0x19, 1, 3, 1, NONE, 0);
		function_1a8220(actor_index, 0xc, 1, 3, 1, NONE, 0);
	}
}

// @retail 0x265050
void function_265050(long actor_index, long prop_ref_index)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
	long object_index = *(long *)(reference + 0x20);
	byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	byte *definition = *(byte **)((byte *)g_4e3b44 + (*(long *)object & 0xffff) * 16 + 8);
	if (*(real *)(definition + 0xd0) + 10.0f > *(real *)(reference + 0x28))
	{
		short state = *(short *)(actor + 0x358);
		if (state < 2 || (state == 2 && *(long *)(actor + 0x360) != object_index &&
			*(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
		{
			long owner_index = NONE;
			memset(actor + 0x358, 0, 0x58);
			*(short *)(actor + 0x358) = 2;
			*(long *)(actor + 0x360) = *(long *)(reference + 0x20);
			*(real *)(actor + 0x36c) = *(real *)(definition + 0xd0);
			*(short *)(actor + 0x35a) = 0;
			*(long *)(actor + 0x368) = prop_ref_index;
			if (*(long *)(object + 0xc8) != NONE)
			{
				byte *parent = (byte *)function_badc0(*(long *)(object + 0xc8), NONE);
				if (parent && ((1 << parent[0xaa]) & 3))
				{
					owner_index = *(long *)(object + 0xc8);
					long unit_index = *(long *)(actor + 0x18);
					if (unit_index != NONE && owner_index == unit_index)
						*(short *)(actor + 0x35a) = 2;
					else if (!function_1df560(*(short *)(actor + 0x24), *(short *)(parent + 0x138)))
						*(short *)(actor + 0x35a) = 1;
				}
			}
			*(long *)(actor + 0x364) = owner_index;
			function_25b910(actor_index, prop_ref_index);
		}
	}
}

// matches: the facing is fetched by calling function_cb7e0, the unit vector getter,
// which LTCG inlines here. That inlined copy is what loads the object into ecx,
// adds 0x168 and reads the three components, the way retail does; the else branch
// reads the object index out of the reference again, so its second pool lookup
// stays separate from the inlined one.
// score 67 (was 67): retail loads the array base into esi and copies the three fields in a different order, ours
// uses ecx. The cause is one choice: retail keeps the object array base live in esi across the two calls and
// fetches the object again from ebx in the else branch, while ours keeps the first object pointer in esi and
// reuses it there (LTCG treats function_30bf0 and function_264260 as leaving the table alone). Tried: the else
// branch reading g_4e0300->data afresh (96, both loads folded); both reads written through g_4e0300->data with
// or without an object local (96); the facing as a const reference (99); a volatile load of the second object
// pointer (103; 102 without the object local: it keeps the element address in esi instead); no object local
// (67); the two ifs folded to if / else if (66); direction.i computed before the facing (67).
// Round 242: native displacement initializer / position copy then subtract 67/67;
// either with native-width narrowed category 76/76; existing facing getter plus either
// direction form 109/109, with native-width category 75/75. Original body restored;
// header-base retention and the second object lookup still differ.
// Round 248: Per-use reference handle, late/early headers 67/110;
// typed counterparts 67/110; wide narrowed category 76/77, typed 76/77.
// All restored; header-base retention and the second object lookup still differ.
// Round 96: the second lookup's address as headers + 8 + i * 12, &headers[i * 12] + 8 or an
// element-pointer index 67/67/67 (VC folds them into one address); the facing copied field by
// field 102; the else branch re-reading g_4e0300->data into headers 96. Diagnostic (reverted):
// calling 0x264260 or 0x30bf0 through a function pointer makes ours reload the object pointer
// but keep the element address (lea esi), not retail's header base; BLOCKED-BY 0x264260's
// side-effect summary (it reads only through its pointers) in part.
// Round 78: a compile-only build of this body (every callee opaque) keeps the header base and
// fetches the object again after the calls exactly as retail; only LTCG's knowledge that
// function_30bf0 and function_264260 write no object memory merges the second lookup into the
// first (function_264260 is a pure leaf in an avoid-listed file). Direction through
// vector3d_from_points3d 67; header base read before the position call 109; facing copied
// member-wise 102; direction before the object lookup 75; object lookup after the zero-length
// fallback 154.
// @retail 0x264940
void function_264940(long actor_index, long prop_ref_index)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
	byte *prop = g_50241c->data + (*(long *)(reference + 8) & 0xffff) * 0xc4;
	actor[0x30c] = false;
	if (!prop[0x23] && 3.0f > *(real *)(reference + 0x28) && *(char *)(reference + 0x27) >= 3)
	{
		long object_index = *(long *)(reference + 0x20);
		point3f position;
		function_b9dd0(object_index, &position);
		vector3f facing;
		function_cb7e0(object_index, &facing);
		vector3f direction;
		direction.i = position.x - *(real *)(actor + 0x238);
		direction.j = position.y - *(real *)(actor + 0x23c);
		direction.k = position.z - *(real *)(actor + 0x240);
		if (function_30bf0(&direction) == 0.0f)
			direction = *g_4687a8;
		short category = (short)function_264260(&direction, &facing, *(real *)(reference + 0x28));
		if (!prop[0x31])
		{
			if (category <= 2)
			{
				function_1fb7e0(0xa3, actor_index, NULL, object_index, NONE);
				prop[0x31] = true;
				*(short *)(actor + 0x30e) = 0;
			}
		}
		else
		{
			byte *owner = *(byte **)(g_4e0300->data + (*(long *)(reference + 0x20) & 0xffff) * 12 + 8);
			if (*(long *)(owner + 0x13c) != NONE)
			{
				if (category <= 0)
				{
					actor[0x30c] = true;
					*(short *)(actor + 0x310) = ai_player_index_get(*(long *)(owner + 0x13c));
				}
				else
				{
					actor[0x30c] = false;
					*(short *)(actor + 0x310) = NONE;
					*(short *)(actor + 0x30e) = 0;
				}
			}
		}
	}
	long child_index = *(long *)(actor + 0x7c);
	if (child_index != NONE && *(long *)(reference + 0x14) != NONE)
	{
		byte *tracking = g_502414->data + (*(long *)(reference + 0x14) & 0xffff) * 0x124;
		if (tracking && tracking + 0x70 && *(short *)(tracking + 0x70) >= 4 &&
			25.0f > *(real *)(reference + 0x28))
			function_268c60(child_index);
	}
}

// @retail 0x264330
void __stdcall function_264330(long actor_index, long prop_ref_index, s_2641c0 *context,
	s_2640c0 *motion, bool force)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	if (!actor[9]) return;
	s_prop_datum *reference = prop_ref_get(prop_ref_index);
	byte *object = *(byte **)(g_4e0300->data + (reference->object_index & 0xffff) * 12 + 8);
	byte *prop = (byte *)prop_get(reference->prop_index);
	byte *unit = NULL;
	s_type_f95cd3 *view = NULL;
	bool update = false;
	long time = g_510c54->game_time;
	if ((1 << object[0xaa]) & 3)
		unit = object;
	if ((reference->state >= 1 && reference->state <= 2) || force ||
		(reference->state >= 1 && reference->unknown26 > 0))
		update = true;
	if (reference->state >= 1 && reference->state <= 2)
		reference->type = *(short *)(prop + 2);
	else if (reference->type != *(short *)(prop + 2) && unit && *(short *)(prop + 2) == 6 &&
		(real)(time - *(long *)(unit + 0x2e0)) * g_510c54->rate > 5.0f)
		reference->type = 6;
	s_type_5cfb45 *state = NULL;
	if (reference->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
		state = &tracking->state;
		view = &tracking->view;
	}
	else if (g_470f10[*(short *)(prop + 2)].kind == 1 && g_510c54->game_time > *(long *)(prop + 0x58))
		state = (s_type_5cfb45 *)(prop + 0x58);
	if (state && update)
	{
		if (view && prop[0x22] && *(long *)(prop + 0x1c) != NONE)
		{
			byte *other = g_4f55f0->data + (*(long *)(prop + 0x1c) & 0xffff) * 0x888;
			long target = function_28fb50(*(long *)(other + 0x1c), (s_28fb50 const *)context, reference->object_index);
			if (target != reference->object_index) reference->object_index = target;
		}
		function_25ccd0(state, reference->object_index, reference->unknown1c, motion);
	}
	s_type_5cfb45 *known_state = function_25d690(reference);
	if (view)
	{
		vector3f *direction = (vector3f *)((byte *)view + 0x2c);
		direction->i = known_state->position.x - context->field_c.x;
		direction->j = known_state->position.y - context->field_c.y;
		direction->k = known_state->position.z - context->field_c.z;
		reference->unknown28 = function_30bf0(direction);
		if (reference->unknown28 == 0.0f) *direction = *g_4687a8;
		if (view->unknown50 != NONE && view->unknown50 + real_to_long((real)g_510c54->field_2_3 * 5.0f) < time)
			function_265c30(prop_ref_index, actor_index, false);
	}
	else
	{
		real x = known_state->position.x - context->field_c.x;
		real y = known_state->position.y - context->field_c.y;
		real z = known_state->position.z - context->field_c.z;
		reference->unknown28 = (real)sqrt(z * z + y * y + x * x);
	}
	if (view && update)
	{
		vector3f *direction = (vector3f *)((byte *)view + 0x2c);
		vector3f velocity;
		function_ba1d0(reference->object_index, &velocity, NULL);
		real speed = (real)sqrt(velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k);
		byte *fields = (byte *)view;
		if (speed < 0.1f) fields[0x3a] = 0;
		else if (speed < 0.5f) fields[0x3a] = 1;
		else if (speed < 1.0f) fields[0x3a] = 2;
		else fields[0x3a] = 3;
		real approach = 0.0f - (direction->k * velocity.k + direction->j * velocity.j + direction->i * velocity.i);
		if (approach < -1.0f) fields[0x3b] = 0;
		else if (approach < -0.5f) fields[0x3b] = 1;
		else if (approach < -0.1f) fields[0x3b] = 2;
		else if (approach < 0.1f) fields[0x3b] = 3;
		else if (approach < 0.5f) fields[0x3b] = 4;
		else if (approach < 1.0f) fields[0x3b] = 5;
		else fields[0x3b] = 6;
		if (*(char *)(fields + 0x3b) < 4)
		{
			view->unknown64 = false;
			view->unknown66 = 0;
		}
		else
		{
			++view->unknown66;
			if (view->unknown66 >= real_to_long((real)g_510c54->field_2_3 * 0.2f))
			{
				view->unknown66 = g_510c54->field_2_3;
				view->unknown64 = true;
			}
		}
		if (reference->unknown28 < 1.0f) fields[0x38] = 0;
		else if (reference->unknown28 < 6.0f) fields[0x38] = 1;
		else if (reference->unknown28 < 10.0f) fields[0x38] = 2;
		else if (reference->unknown28 < 30.0f) fields[0x38] = 3;
		else fields[0x38] = 4;
		vector3f facing;
		if (unit) function_cb7e0(reference->object_index, &facing);
		else facing = *(vector3f *)(object + 0x70);
		view->unknown39 = (char)function_264260(direction, &facing, reference->unknown28);
	}
	if (known_state->unknown66)
	{
		long related_index = function_25d810(known_state->unknown3c, actor_index, false);
		if (related_index != NONE)
		{
			s_prop_datum *related = prop_ref_get(related_index);
			point3f position;
			function_b9dd0(related->object_index, &position);
			real x = position.x - context->field_c.x;
			real y = position.y - context->field_c.y;
			real z = position.z - context->field_c.z;
			related->unknown28 = (real)sqrt(z * z + y * y + x * x);
			function_265290(actor_index, related_index);
		}
	}
	else if (unit && (known_state->unknown5f || function_10f8f0(reference->object_index) == 0x500000a))
		function_265550(actor_index, prop_ref_index);
	else if (reference->type == 3)
		function_265050(actor_index, prop_ref_index);
	else if (reference->type == 4)
		function_265290(actor_index, prop_ref_index);
	if (prop[0x25]) function_264940(actor_index, prop_ref_index);
	if (view) *(real *)((byte *)view + 0x3c) = function_265d30(actor_index, prop_ref_index);
}

int __cdecl function_2631f0(void const *first, void const *second);
void function_25c780(long actor_index, long prop_ref_index);
bool function_25db60();
void prop_view_initialize(s_type_f95cd3 *view);
struct s_actor_object_sample;
bool function_28fa60(long perception_index, point3f const *point, s_actor_object_sample *sample);
struct s_object_motion_view;
void function_2640c0(long object_index, s_object_motion_view *result);
void __stdcall function_263210(long actor_index);
void __stdcall function_265680(long actor_index);
void function_263670(s_prop_datum *node);
bool function_25d610(s_prop_datum *node);
bool function_25da40(s_prop_datum *node);
bool __stdcall function_25c230(long actor_index, long node_index, short type);
void function_25c860(long node_index);
void function_25c820(long node_index, long actor_index);
void function_25ac00(long node_index, long actor_index);
void function_1e2150(long actor_index, long node_index);
void *function_1e5240(long actor_index);
real function_2683f0(long actor_index, long node_index, short type);
void __stdcall function_264b50(long actor_index, long node_index, s_2641c0 *sample, s_2640c0 *motion);

// MiniMax E (11-20): no new form, no lower score (404): retail's entry takes the actor index in EAX (`sub esp, 0xa0;
// push ebx; mov ebx, eax`) and keeps it there, and its tail is a plain `ret` with no stack argument, while this body
// takes the index from its stack slot and gives the frame 0xa4 against retail's 0xa0; that EAX convention belongs to
// retail's own call graph and no spelling of this body reaches it, so the difference stays where it is:
// BLOCKED-BY 0x2641c0 (its noinline declaration is in src/unknown_265cb0.cpp, another lane's file). The body's own
// arithmetic and the rest of its homes are the same code; the score stands at 404.
// MiniMax B (11-19): no new form, no lower score (412): the maximum as a long instead of a word gives 416 and the prior
// state as a short instead of a word gives 413, so both were reverted and the body stands. What still differs is the
// frame, 0xa4 against retail's 0xa0, and the actor index, which retail takes in EAX because it inlines 0x2641c0 at its
// caller, so this body's own signature cannot change: BLOCKED-BY 0x2641c0 (its noinline declaration is in
// src/unknown_265cb0.cpp, another lane's file).
// MiniMax A (11-19): no new form, no lower score (412): retail still takes the actor index in EAX (and keeps it in EBX)
// and gives the frame 0xa0, while ours takes it from the stack and gives the frame 0xa4; the slot offset and the actor
// pointer that retail saves at [esp+0x28] and [esp+0x20] are the values its inlined 0x2641c0 reuses, and ours saves the
// pointer at [esp+0x18] instead. The inline of 0x2641c0 and the calling convention are outside this file (its noinline
// declaration lives in another lane's file), so the difference cannot be closed here; the body is unchanged.
// MiniMax H (11-18): no new form, no lower score (412): the frame stays 0xa4 against retail's 0xa0 and the early
// homes keep their order (ours the actor base at 0x18, the flag byte at 0x17 and the constant 1 at 0x1c, where retail
// has the index at 0x28, the actor base at 0x20, the constant 1 at 0x1c and the flag byte at 0x13, since retail saves
// four registers and this build three), and the actor index still comes from the stack where retail takes it in EAX;
// a 300-second permuter run (21 tries) left the baseline at 409 differing instructions in both images; the body stands.
// MiniMax R (11-14): no new form, no lower score (412): retail's entry is `sub esp, 0xa0; push ebx; mov ebx, eax`
// with the actor index arriving in EAX, where ours reserves 0xa4 and reads the argument from the stack; the request
// convention is fixed by the callee 0x2641c0, whose noinline declaration is in src/unknown_265cb0.cpp, another lane's
// file: BLOCKED-BY 0x2641c0.
// MiniMax G (11-10): no new form, no lower score (412).
// MiniMax S (11-10): no lower score (412): a plain long for the maximum gives 428 (worse) and is reverted; retail still takes the actor index in EAX where ours loads it from the stack, and the frame is 0xa4 against retail's 0xa0.
// MiniMax R (11-05): no new form, no lower score; retail takes the actor index in EAX and pushes ebx/ebp/esi
// before the actor lookup, ours reads the argument from the stack and pushes after it.
// MiniMax B (11-04): no new form, no lower score.
// MiniMax S (11-06): no new form, no lower score.
// MiniMax D (11-09): no new form, no lower score; the frame is four bytes larger than retail's
// (0xa4 against 0xa0), retail takes the actor index in EAX where ours loads it from the stack,
// and the body is 1620 bytes against retail's 1616.
// MiniMax B (11-12): no new form, no lower score (412): sharing the release path's 0x25c820 call through a goto to the
// tracking block gives 457, and a short for the node's old state gives 413; the frame stays 0xa4 against retail's 0xa0.
// MiniMax A (11-13): no new form, no lower score (412): retail takes the actor index in EAX where ours loads it from
// the stack, the frame stays 0xa4 against retail's 0xa0 and the body 1620 bytes against retail's 1616; the two callees
// that held this one back now match, so what is left is the actor index's home and the frame.
// MiniMax E (11-16): no new form, no lower score (412): the frame is still 0xa4 against retail's 0xa0 and retail still takes the actor index in EAX where ours loads the argument from the stack, so the body stays four bytes longer than retail's.
// score 404 (was 404; 412 before lane 14 r124): the frame is 0xa4 against retail's 0xa0; retail takes the actor
// MiniMax H (11-20): no new form, no lower score (404): retail takes the actor index in EAX (mov ebx, eax right after the first push) while ours reads it from its stack home at esp+0xac; that difference follows from the inlined 0x2641c0, whose noinline declaration sits in another lane file, so the forms of this body cannot move it; the frame stays 0xa4 against retail 0xa0 and the body 1624 bytes against retail 1616.
// Earlier at 404 (was 412): the frame is 0xa4 against retail's 0xa0; retail takes the actor
// index in EAX where ours loads the argument from the stack. The body is 1620 bytes
// against retail's 1616.
// Frame 0xa4 versus 0xa0; stacked actor and node/workspace homes still differ.
// Earlier at 426 (was 426): frame 0xa4 versus 0xa0; actor input and workspaces differ.
// Wide prior state and byte sampling flags 426; byte sampled-one 426;
// native for traversal 436; later update init 427; later flag inits 426;
// for plus wide update/sample/both 439/437/429; release noinline with
// for/original traversal 475/456.
// frame 0xa4 versus 0xa0, actor stacked versus EAX/EBX;
// initial update predicate 441 (frame 0xa0); counter word snapshot 426;
// prior-state word 426; case-one tracking scope 426; separate final guards 426.
// Earlier: frame 0xa4 versus 0xa0 and actor input/home still differ. Word maximum 420
// kept; word update/sample/sampled count 430/449/449; native view 426; later view lifetime 426;
// word maximum plus native view/later view/update 420/420/424.
// Earlier: frame 0xa4 versus 0xa0, actor input and workspace homes remain.
// frame 0xa4 versus 0xa0 and actor input/home still differ. Word maximum 420
// frame 0xa4 versus 0xa0, actor input and workspace homes remain.
// Earlier at 420 (was 426): frame 0xa4 versus 0xa0 and actor input/home still differ. Word maximum 420
// Earlier at 420 (was 420): frame 0xa4 versus 0xa0, actor input and workspace homes remain.
// Earlier: frame 0xa4 versus 0xa0 and actor input/home still differ. Word maximum 420
// Earlier: frame 0xa4 versus 0xa0, actor input and workspace homes remain.
// Native signed maximum 420; signed maximum/original loads 420; later maximum/signed
// maximum 422/422; signed counter/original and signed maximum 420/420;
// later counter/original and signed maximum 422/422. Each fast and plain.
// Existing sampling-helper automatic, inline and force-inline boundaries 486 each;
// all declarations restored after the force-inline form lost a checked caller.
// Round 166: typed actor startup/admission/tracking/completion/all 420 each; native actor
// getter, native node fields and both 420 each. Actual shared type layout restored.
// Immutable prior state/node handle/both 420/420/420; current field observations 420;
// distance-axis names 420; update-first validation 421; availability-first admission 422;
// admission plus validation order 423.
// Sample after counter/maximum 437/437 (retail frame and length); split true flags 424;
// test-derived sample 434; timeout conditional 420; decrement branches 420; common maximum exit 437; nested sample-success scope 420.
// Shared long/word actor slots 444/441; node slots 429/438; both long 439;
// long actor/word node 423 (frame 0xa0, exact length); word actor/long node 440; both word 456.
// On the repaired frame: long/short update 430/429; update predicate 430; single exit 433.
// Sampled-once acquisition after startup/at traversal/before actor 422/422/420; startup plus later
// node flags 422; completed-sample name 420; separate/reversed node flag declarations 420/422;
// startup plus reversed flags 424.
// Initial refresh, spatial startup, node refresh and progress call declarations, each ordinary
// inline/noinline 420/420; all eight compiled forms and every separate declaration restored.
// Round 166: typed actor startup/admission/tracking/completion/all 420 each; native actor
// getter, native node fields and both 420 each. Actual shared type layout restored.
// Round 212: Three-state sampling enum/byte/short/long with entry success 434/426/434/434;
// with success at consumption 434/430/438/434. Byte states recover frame 0xa0 and 1616 bytes.
// Complementary run-wide availability bool/byte/short/long 424/424/422/443;
// per-node skip/defer/both 429/424/433; all three complements 436 at retail length. All restored.
// Round 205: Shared planar difference with native state, early planar output and both
// reused progress-result combinations 424 each in eight forms (control 420); all restored.
// Round 223: original independent admission flags with a separate query-completion
// bool/byte/short/long at its consumer 418/418/430/438; at node entry 433/433/449/457.
// Two-point forms and added phase flags rejected without a match; all restored.
// Round 226: Consumption scopes for update, tracking and each real view, all seven subsets
// 420 each; all scopes with run-wide constant initialization before actor lookup 420. All restored.
// Round 254: complete per-use actor/node/both getters 465/668/705; each on native
// for traversal 460/664/705; paired getters with long sample/update 720/711.
// First six recover frame 0xa0 but grow code and retain stacked actor input; all restored.
// Round 239: original shared actor/reference/prop getter force-inline declarations,
// all seven subsets and all three with unsigned full next/current handles: 420 each.
// Original getters, handle domains and body restored.
// Round 384: retained actor/node/both lookups 441/442/462; unsigned full handles 420;
// unsigned handles with retained actor/node/both 441/442/462; all on a plain linked-list for 466.
// All lookup lifetimes, handle domains, traversal and complete original body restored.
// Round 231: case orders 04231/04321/02341/03241 give 436/441/436/441;
// 40231/40321/23041/32041 give 436/441/446/433. Last form recovers frame 0xa0
// and 1616 bytes but keeps stacked actor input; every original case and body restored.
// Round 389: native double timer/planar pair/both 423/421/424; unsigned bounded
// decrement 420, with timer/planar/both 423/421/424; complete precision graph with
// wide decrement 424 (fast control 420). Original precision, domains and body restored;
// frame 0xa4 and actor/node/flag homes remain. Planar precision forms were exact-only.
// Round 238: Concrete motion/context/both output types 420 each; motion/context/both with the
// native actor receiver 420 each; both with context-first declarations 420; all with entry
// workspaces 420. All original types, declarations, fields, qualifier and directives restored.
// Round 250: dispatch release bool/byte/short/long at dispatch entry 420/420/420/420;
// node entry 420/420/420/420. All exact-only result flags restored; frame and actor homes remain.
// Round 41: Joint reset/eligibility/tracking predicate declarations, all eight ordinary
// inline/noinline combinations, 420 each. Callees remain matched; frame 0xa4 and stacked
// actor input remain. Every separate declaration and complete original body restored.
// Round 442: Completion/release/publication ordinary-inline declarations, seven subsets and all
// with final publication, 420 each in eight compiled forms. All temporary declarations and
// original body restored.
// Round 114 (lane 5): retail's loop slots are an {index, next} iterator struct ([esp+0x14]/[esp+0x18])
// and its loop is un-rotated: the walk through s_iterator with a forceinline step (the matched
// 0x25d510/0x25d580 form) reading iterator.index gives retail's length, slots and stack sample/update
// bytes, 443 (plain inline step 443; with the two true flags split in either order 443/443; with a
// plain short maximum 442, its dword store and spill as retail). Retail inlines 0x2641c0 here and in
// 0x263210 but not in 0x264210: the noinline declaration dropped 485 (with the iterator walk 469, frame
// 0xa0) and 0x264210 then inlines it too and stops matching. Ours also inlines 0x25c820 at the release
// label where retail calls it out of line (tail-merged with case 1's call); the release pair written in
// each case 583 (inlined at every site). The actor index stays stacked: ours gives EBX to the index copy.
// Round 21 (m3 lane 2, fast; EBX goes to the `update` bool in the walk, the only callee-saved byte
// register, as it lives across the 0x2640c0/0x2641c0 calls): `sample = true; update = true;` 424;
// sample/update declared after the node call 420; update declared first 422; sampled_one declared
// before maximum 420; the update test nested under `if (sample && update)` 420.
// Round 256: Case priorities beginning with 1 10423/14023/14203/14230/13042/13402/13420/14320 give
// 422/422/425/419/449/452/457/445; one-point form rejected. Original cases and body restored;
// frame and actor homes remain.
// Round 144 (lane 4, fast with 0x1e34e0): retail inlines 0x2641c0 here (actor flag test, then 0x28fa60 or the
// 0x38-byte copy), which our build calls out of line (pinned for 0x264210). Diagnostic only, a local forceinline
// copy of that body 486 (1700 bytes; the update flag still takes EBX and the actor stays a stack argument, so the
// inline alone does not move the convention); sample/update declared at function scope 420; the maximum test
// as `(short)maximum < field` 421; the 0x25d610 test nested under `sample && update` 420; the sampler result
// clearing the flag with the 0x264b50 call in its else arm 458 (1656 bytes).
// Round 45 (lane 13, fast with 0x1e34e0): iterator walk 443 (1616 bytes, frame 0x9c; index copy in EBX,
// actor copy in EDI, retail the reverse); iterator.index at each use 443 (same code); with maximum a plain
// word 442; with sample/update at function scope 442; with sampled_one declared first 442. The actor's
// stack passing may come from the build's stand-in caller (a second call site passing a buffered argument).
// Round 215: terminal maximum publication through native reset/count arms 420;
// twice-used elapsed values on that graph 420; each with plain word maximum 472/472;
// terminal reset result bool/long 440/444, each with plain word maximum 448/448.
// Exact-only reset-result forms and every executable change restored; frame and actor homes remain.
// Round 66 (lane 9, fast): retail's 0x2683f0, 0x1e2150 and 0x25ac00 take the actor index in EBX, where retail
// keeps it for the whole function; ours reloads EBX from the stack argument before each. Case 1's 0x25c820 call
// and the release one made a single call (a `release_only` label) 431 (frame 0xa0, 1616 bytes, the three flags go
// to stack bytes, but `index` then takes EBX); with the iterator walk 452; the iterator walk reading
// `iterator.index` at every use 443 (identical to the listed 443); the 0x25d510 step made an extern function and
// inlined across files 443 (identical; 0x25d510/0x25d580/0x25dac0/0x25b440 still match); the single call with
// 0x2641c0 inlinable 464, with the iterator walk too 483. All restored.
// Round 119 (lane 8, fast with the file and 0x1e34e0): the three inline_depth pragmas dropped 560 (1768
// bytes: 0x25d700 inlines where retail calls it); both progress updates as `field = field + call` 420
// and as a named `progress` sum stored and tested 420 (same code: ours still folds `addss xmm0, [ebp+0x10]`
// where retail loads the field into xmm1 after the call and adds xmm0); `update && sample` 421;
// sample/update declared as separate bools at the loop-body top 421.
// Round 229: timer/view-first case orders 30421/34021/34201/34210/30241/32401/32104/34102
// give 437/434/431/461/441/431/433/452. Fast frame 0xa0 recovered, actor input stays stacked;
// all original cases, qualifiers, call boundaries and executable body restored.
// Round 260: tail completion bool/byte/short/long at finalization/entry 437 each;
// exact-only flags restored; frame 0xa4 and stacked actor input remain.
// Round 263: Independent native-long sample/update/sampled-one subsets 433/424/430/425/452/425/430; all long with ordinary maximum 445.
// Original while traversal, Boolean flags, maximum qualifier, workspaces and complete body restored.
// DH round 273: independent spatial/timer/publication view lifetimes 420/420/420/420; existing typed
// views with shared/spatial/timer/all receivers 420/420/420/420. Every original receiver, field access
// and executable statement restored; actor input and frame remain.
// DH276: spatial-first orders 20413/20143/20314/20341/21043/21403/21304/21340
// 420/426/418/433/418/423/413/422. On 21304, native byte sample/update/both
// 413/413/413; native signed prior-state carrier 412, plain-confirmed and kept.
// The seven-point order alone and two-point orders were rejected; keep only the
// combined form at 1620/1616 bytes. Frame 0xa4, stacked actor and flag homes remain.
// DH round 279: Spatial-first case priorities 24013/24031/24103/24130/24301/24310/23401/23410 on the
// retained signed prior-state graph [423, 434, 421, 417, 448, 421, 447, 425]. Original executable body
// restored.
// Round 236: Run-wide sampling result before maximum, logical/direct and qualified/plain
// maximum 450/454/450/420; after maximum logical 450/454, direct with native signed-short
// prior state 427/417. Several recover frame 0xa0 and 1616 bytes, but actor input stays
// stacked. Original prior-state type, maximum qualifier, predicates, cases and body restored.
// DH281: Sampling admission priorities ACB/BAC/BCA/CBA with original and update-first validation on
// retained 21304/signed-state graph; exact-length 412 ties the opening score [414, 450, 450, 422, 412, 436,
// 436, 424]. Original executable body restored.
// Round 324: post-query timer limit from branches/conditional/default, each alone and with
// native short decrement, then branch/conditional with named elapsed: 412/412/412/412/412/412/412/412. Original restored.
// P262: Complete sample/update/sampled-once lexical priorities, all six native Boolean
// orders 412 each; signed prior-state word on sample-first/update-first flag graphs 413/413.
// Original flags, signed-long state, workspaces, cases and complete body restored.
// N313: Equal-size motion/query workspace lexical ranks motion/query_result, body_motion/context,
// motion_sample/sample_context and sample_motion/query_result, each with promoted/native signed
// saved state: 412/413 in all four pairs. Native state recovers the word store but regresses one;
// workspace ranks leave the frame and homes unchanged. Original names, full saved-state domain,
// qualifier and body restored; frame 0xa4 versus 0xa0 and stacked actor remain.
// DH291: Progress-first case priorities 40123/40132/40213/40312/41023/41032/41203/41230
// on the retained signed-state graph 424/448/423/450/422/452/421/417. All complete cases,
// release joins, sampling, qualifiers and original body restored; actor/frame homes remain.
// DH293: Case-local admission completion, timer reset-first and terminal completion,
// all seven nonempty combinations 412 each; complete graph with native signed
// saved state 413. Original cases and complete body restored; frame/actor homes remain.
// DH295: Signed-state graph with zero-first complete priorities 01234/01243/01324/01342/
// 01423/01432/02134/02143: 418/424/440/448/424/444/420/426. Original cases restored;
// actor/update register conflict, frame excess and workspace homes remain.
// DH298: Timer-first priorities 30124/30142/30214/30412/31024/31042/31204/31240
// on the retained signed-state graph 440/452/435/447/437/448/433/461. All recover frame
// 0xa0 but retain the stacked actor; original complete cases and body restored.
// F336: Maximum dword/once-read twice-used priority/later constant acquisition,
// seven subsets on the retained signed-state graph, then plain dword with priority:
// 416/412/414/414/418/414/416/428. Original maximum, observations, cases and complete body restored.
// Round 46 (m3 lane 15, fast): volatile sample/update flags (diagnostic, reverted) 424 (frame 0xa0; `index`
// then takes EBX and the actor is still re-read from its stack slot; our loop holds the actor pointer in EDI
// where retail spills it); loop actor fields through fresh actor lookups 457; plain word maximum 429 (1616
// bytes); the current-target test as one named bool 438 (1616 bytes); the list head through the held actor
// pointer 442 (1616 bytes); sample/update declared before the prior state 412 (same code).
// DH310: Complete case orders 04123/04132/02413/02431/02314/03124/03142/03214 on the signed-state graph,
// 422/448/426/433/416/444/451/431. All original case-body hashes agree; every new priority regresses.
// Original cases, observations, qualifier and whole body restored; frame excess and stacked actor remain.
// Round 80 (m3 lane 9, fast; positional/aligned, base 412/228; retail has one 0x25c820 call and inlines 0x2641c0):
// iterator walk 442/211 (1616 bytes); with a long maximum 442/225; with a short saved state too 442/222; walk and a
// local forceinline copy of 0x2641c0's body 492/195 (1672); with the short saved state 492/194; all three 496/198
// (1676); case 1's 0x25c820 and the release one as a single call 457/226 (1592); with the walk 465/212 (1592); with
// the walk and the inline copy 477/200 (1656); with the short saved state too 476/200. In all of them the actor stays
// a stack argument, in EDI, with the index in EBX (retail actor EBX, index EDI).
// Round 203 (fast with 0x1e34e0): `update = sample = true` 413; motion/context at function scope 412;
// the prior state read after the flag declaration 412; a for loop over the node index with `next` 453
// (1600 bytes); the sampler failure clearing the sample flag with the 0x264b50 test in its else 452.
// BLOCKED-BY 0x2641c0 (retail inlines it here; its noinline declaration sits in src/unknown_265cb0.cpp,
// another lane's file this round). Dropping that declaration inlines it into 0x264210 too (round 114), so
// the lead is a form that inlines it here but not in 0x264210.
// Round 35 (m3 lane 17, fast with 0x1e34e0): prior state as short 413; the sampler's result in its own
// `context_valid` flag (initialised false, tested for the 0x264330 call) 415 (frame 0xa0 and 1616 bytes as
// retail, but `update` still takes BL and the actor stays a stack argument; a flag tested later, not kept); that
// with the short prior state 419 (1620 bytes); sampled_one declared after the two entry calls 414; sampled_one
// declared before maximum 412 (same code).
// F402: Complete case priorities 42013/42031/42103/42130/42301/42310/43012/43021
// on the retained signed prior-state graph,
// 423/434/421/417/448/421/450/440. NOT kept: no exact or qualifying result;
// original complete body restored.
// N393: Next/current/prior-state signed/unsigned eight-byte storage, eight forms 455/455/453/453/490/490/456/529.
// NOT kept: all regress and enlarge frame/body; original handles, state, flags and body restored.
// Frame 0xa4/0xa0 and stacked actor versus EAX/EBX remain.
// Round 69 (m3 lane 14, fast): our actor goes to EBX at entry but EBX then holds a loop byte flag and the actor is re-read
// from its stack home 12 times (retail keeps it in EBX, flags in stack bytes). `maximum` as a plain word 429 (1616 bytes);
// sample/update declared at function scope 412; the 0x2641c0 result in its own bool 412; old_state short 413; the actor
// pointer declared then assigned 412.
// MiniMax E (11-10): no lower score (412). A 20-minute permuter (95 tries) found no variant below the baseline,
// and the blockers are unchanged: retail takes the actor index in EAX (plain `ret`), keeps the actor pointer in
// EBX with the node index in EDI, reserves 0xa0 of frame and holds three byte flags at +0x11/+0x12/+0x13, while
// ours passes the index on the stack (`ret 4`), holds the actor in EDI with the index in EBX, reserves 0xa4 and
// keeps the flags at +0x13/+0x17/+0x1b. Retail inlines 0x2641c0 here and the noinline declaration that stops it
// is in this lane's src/unknown_265cb0.cpp, but dropping it inlines the call in 0x264210 as well (round 143 above:
// 497 at 1688 bytes), which costs more than it gains here. Nothing kept.
// Round 143 (m3 lane 8, fast with 0x1e34e0): without 0x2641c0's noinline declaration this body inlines it as retail
// does (497, 1688 bytes), but so does 0x264210 (68 -> 139 bytes, unmatched), while 0x25d020, 0x1be120, 0x1fba10,
// 0x25c230 and 0x265680 stay matched: the lead narrows to keeping it out of 0x264210 alone. On that inlined form: update
// as long 495, sample/update at function scope 497, maximum a plain word 497, sampled_one as long 496; the actor stays a
// stack argument (0x1e34e0 26) in all of them. All restored.
// Round 78 (m3 lane 13, fast with 0x1e34e0): retail's case-2 and completion view lookups have 0x25d740's shape (NONE test,
// NULL test, +0x70); both through the existing 0x25d740 413 (inlined at both, masked aligned 219 -> 221; 0x1e34e0 26).
// Case 2's distance through the existing vector2d_from_points2d and dot_product2d (from `&state->position`) gives retail's
// block instruction for instruction (y first, the dead `add eax, 4`), 425 at 1624 bytes (masked aligned 218); with the
// 0x25d740 lookups 417 (masked 219). Not kept: positional rise.
// Round 66 (m3 lane 15, fast with 0x1e34e0 26, 0x263210 231, 0x264330/0x264210/0x2641c0 matched throughout): retail's inlined 0x2641c0 copies its 0x38 bytes with
// `rep movsd`, which takes ESI and EDI inside the loop, and 0x2640c0 takes its output in EDI, so retail keeps the node index and the next index in stack slots
// (0x14/0x18, reloaded before each use), the three flags in stack bytes (0x11/0x12/0x13), the prior state as a word at 0x2c, the actor pointer at 0x20 and the actor's
// pool offset at 0x28 (shared with the inlined body), with only the actor index in EBX and the node in EBP. Retail's maximum is a 32-bit slot set from a zero-extended
// word and compared as 16 bits. The maximum without its qualifier: long 428 (1616 bytes), dword 428, short 429, long with the compare reversed 428; long or dword with
// a short prior state 433 / 433 (1620 bytes). The qualifier stays (412). Still BLOCKED-BY 0x2641c0 (its noinline declaration is in src/unknown_265cb0.cpp, another
// lane's file this round).
// Round 114 (m3 lane 9, fast with 0x2641c0's callers, which agrees with plain here): the walk through the existing 0x26be00/0x26be30
// (as 0x263210 and 0x1e23c0 now do) 442 at 1616 bytes, the listed iterator-walk code, also with `iterator.index` at each use. The
// 0x2641c0 lead, src/unknown_265cb0.cpp being free this round: its noinline declaration dropped re-measured 497 (1688 B, round 143's
// result); new: with 0x263210 calling 0x2641c0 in place of its written-out copy, 0x263210 stays matched (LTCG inlines the call to
// retail's bytes), and 0x264210 still inlines it too (68 -> 139 B, 114; retail calls it there) while 0x265680, 0x25d020, 0x1be120,
// 0x1fba10 and 0x25c230 keep calling it. On that base the walk re-measured 492 (1672 B, round 80's result; frame 0xa0, the three flags
// in stack bytes 0x11-0x13 and the iterator at 0x14/0x18 as retail, but `index` takes EBX and the actor index EDI, retail the reverse,
// so the index stays a stack argument), with `iterator.index` at each use 492 (same code); new: the loop's five actor-field reads
// through actor_get(actor_index) 522 (1696 B); 0x2641c0 as a single-exit result 501 (0x2641c0 15). Tried on 0x264210's side, none
// stops that inline: 0x2641c0's copy as an aggregate assignment, 0x2641c0 without its `point_reference` read (it still matches once
// inlinable), 0x264210's reference through prop_ref_get, 0x264210's known state named before the call (497 here, 0x264210 114 each).
// Even with 0x264210 solved the inlined body scores 492-497 against 412, so this is a structural lead, not a near win.
// BLOCKED-BY 0x264210's inline decision for 0x2641c0. Also round 114: the case-2 and completion view lookups through the existing
// tracking_get (`&tracking_get(index)->view`) 412 (same code, kept: it replaces the written-out lookup).
// Round 92 (m3 lane 11, fast with 0x1e34e0, 26 throughout): case 3's limit as one conditional 412 (other code); case 3's
// decrement exit as `if (>= 0) break; goto release_tracking;` 433 (1616 B, frame 0xa0); the loop-tail state through the typed
// `->unknown68` field 412 (same code); case 4's arms the other way round 443 (1644 B); sample/update declared after the 0x263670
// call 412 (other code).
// MiniMax C (11-14): no new form, no lower score (412); the actor/index/flag allocation and the
// 0xa8-versus-0xa0 frame are unchanged.
// M3 lane 12 r123 (fast with 0x1e34e0, 26 each): the loop's registers are retail's but EBX (ours the `update` byte, retail
// the actor index): a loop-local actor pointer re-read from actor_get at the loop top 471 (1648 B), that with `update` as
// long 479 (1656 B); the actor index stays a stack argument in both.
// MiniMax I (11-14): no new form, no lower score (412): the record pointer is still copied into edi and read with
// edi plus the index where retail keeps it in eax, and the frame is still four bytes larger with its pushes in a
// different order.
// Round 183 (m3 lane 6, fast with 0x1e34e0 and the 0x2641c0 family): `update` as the named sampling condition with
// `sample = update` 430 (1616 bytes and frame 0xa0 as retail, but the actor index still loads from the stack into EDI;
// a test held in a named bool, not kept); a __forceinline declaration of 0x2641c0 here, its noinline kept in its own
// file, 412 (same code); old_state as an unsigned word 413; the named condition with `update` long 449 (1600 bytes);
// the named condition with `!sampled_one` tested last 454 (1600 bytes). Retail keeps all three flags in stack bytes and
// the actor index in EBX; ours gives BL to `update`. Original body restored.
// M3 lane 9 r126 (fast with 0x1e34e0, 26 throughout; control 412): retail's inlined 0x2641c0 reuses the actor's slot offset
// computed at entry (`[esp + 0x28]`) as well as the pointer (`[esp + 0x20]`). The current target read once per pass into a
// named `current` for its two tests 422 (frame 0xa0 and 1616 B, but the actor is still a stack argument); the progress word
// named once for the maximum test and store 412 (same code); `p` fetched first with `node` cast from it 412 (same code);
// `byte *const actor` 412 (same code); the list head read through the held actor pointer 442 (1616 B, re-measure of
// round 46's form). Body unchanged.
// b7A (11-17) walk against retail, fast build on r829: every condition, offset, call argument (register-passed ones
// too: 0x263670 reads only ESI, 0x2683f0/0x1e2150/0x25ac00 take the actor in EBX) and constant (0x45dbc0 1.0, 0x45e1f4
// 0.33f, 0x43ff08 1.5, 0x45dbd8 0.0, 0x45dc00 10.0) agrees with retail except one BEHAVIOUR FIX in case 2: retail
// releases the prop (0x262f27 calls 0x25c860) when the squared planar distance between the known state and the object
// exceeds 1.0 (0x262f1a comiss against 1.0, 0x262f21 `jbe 0x2630a4` skips the release when it is <= 1.0; a missing view
// or a zero timer at 0x262edc/0x262ee6 also reach it), where the old source broke out when the distance exceeded 1.0
// and released when it did not (`ja`). Now `if (!(y * y + x * x > 1.0f)) break;` compiles to retail's `comiss xmm2,
// [1.0]; jbe` (412, 1620 bytes, unchanged: that is a positional count); the `<= 1.0f` spelling gives 424 (1624 bytes:
// it loads the 1.0 into xmm0 and uses `jae`). Also tried: retail's code order of the cases (0x262d70, 0x262e2a,
// 0x262e81, 0x262f35, 0x262fad = 0/4/2/1/3) 422 (1624 bytes); the final `current != NONE` guard split in two as
// retail's second `cmp esi, -1` at 0x26316c 412 (the compiler folds the second test); both reverted but the fix. No new
// include, helper or double. Checked with the file's callers/callees (0x1e34e0 25, 0x264b50 91 as before; the rest
// MATCH).
// M3 lane 3 r258 (fast with 0x1e34e0, 0x263210, 0x264b50 and the 0x2641c0 family; control 412, aligned 218): the three
// flags with their address taken (`(void)&flag` each, or the outer one read through a pointer local) 412 / 412 (same
// code, the compiler sees through it). The walk through the existing 0x26be00/0x26be30 re-measured 442 at 1616 bytes with
// the frame 0x9c (aligned 198: the flags then sit in retail's stack bytes 0x11-0x13 and the iterator at 0x14/0x18); on
// it a short prior state 442 (aligned 196, the word store at the loop top as retail), the iterator's index read at each
// use 442 (same code), `sample = true; update = true;` 442 (aligned 195, retail's store order 0x11 then 0x12), a plain
// long maximum 442 (aligned 212). On the walk form the node index takes EBX and the actor index EDI, retail the reverse
// (0x2683f0, 0x1e2150 and 0x25ac00 read the actor from EBX, so ours reloads EBX from the stack argument before each).
// Diagnostic, reverted at once (a volatile view of the iterator): the index read from memory at every use 449 (1640
// bytes, frame 0xa0), and the actor index is still a stack argument loaded into EDI, so the index's register is not what
// keeps the argument on the stack. Left: the EAX convention, the slot offset saved at entry ([esp+0x28], which the
// inlined 0x2641c0 reuses) and the walk. BLOCKED-BY 0x2641c0's inline (its noinline declaration is in another lane's
// file this round).
// M3 lane 7 r182 (fast with 0x1e34e0, 0x263210, 0x2641c0, 0x264210, 0x264b50, 0x264330, 0x265680; control 412, aligned 342):
// case 3's decrement as an if/else on the current target 412 (same count); `sampled_one = true` ahead of the counter reset
// 413; the tail's early return as if/else arms with one final maximum store 412. Diagnostics, reverted: the `update` flag
// folded into `sampled_one` 458 (1596 B, frame 0xa0) and the parameter copied into a const local used everywhere 412 (same
// code): in both the actor index is still a stack argument, so neither the byte register `update` takes nor the
// parameter's own uses keep it there. No named double sits in this function's call tree (searched nine levels; 0x265550's
// `(double)range` is a cast in a test), so it is not the stack-parity effect that pads 0x1197b0, and the walk model
// (m3-lane5 build/r195 search.py, run on this build's image) gives 0x262ba0 the custom convention, so it is not the walk
// either. What keeps the argument on the stack is still open; BLOCKED-BY 0x2641c0's inline stands.
// M3 lane 13 r117 (fast with 0x1e34e0, 0x263210, 0x265680, 0x264330, 0x264b50, 0x2641c0, 0x264210; control 412): the whole
// body as a new forceinline inner function behind the entry point 412 (same code: the inner parameter is merged with
// the argument, which stays on the stack); the actor pointer fetched at each use instead of the `actor` local 460
// (1644 bytes, frame 0xa0, argument still on the stack); the final view test through `node` instead of a second
// prop fetch 434 at 1616 bytes (retail's length, not kept: the count rises); re-measured: the for loop over the index
// 453 (1600 bytes), 0x2641c0's
// noinline declaration dropped (diagnostic, restored) 497 at 1688 bytes with 0x264210 lost, and that with the inner
// function 497 (same code). Reading: in every form ours keeps the argument in its stack home and reloads it (twelve
// reloads in the 412 body) because the prop index holds EBX; retail holds the actor index in EBX for the whole body
// (the three callees that take it in EBX get it without a load) and the prop index in EDI with a stack home. The
// argument register follows from that allocation, not from an inner function. Body unchanged.
// M3 lane 3 r273: retail's listing calls 0x28fa60 itself (34 calls against our 35), so 0x2641c0 is expanded here; its noinline
// declaration sits in a file another lane held this round, so no trial. A one-object link-time screen of this file with a direct
// caller does not reproduce the full build (frame 0xa8, the index kept in EDI), so it is no screen for this function.
// M3 lane 6 r204 (fast with 0x1e34e0, 0x263210, 0x264330, 0x264210, 0x2641c0, 0x26be00, 0x26be30, 0x265680, 0x264b50; control
// 412, aligned [218, 198, 149] as exact / stack offsets masked / registers masked; body unchanged). Where the stack argument
// comes from, by cutting the body (diagnostics, each restored): with no loop, with only the walk and the 0x263670 call, or
// with the loop's first block alone (up to the maximum test, where the actor index is not read) the function takes the
// index in EAX (`mov ebx, eax` or `mov edi, eax` at the entry) and the caller 0x1e34e0 MATCHES, so that caller's 25 is this
// convention and nothing else; adding any one later block that reads the actor index inside the loop (the sampling block,
// the state-change view update, the 0x264330 call or the 0x25ac00 call, each alone) puts it back on the stack, and so do
// the loop without the switch and the loop without the sampling block. In this body the index then takes EDI and the
// `update` byte BL; removing 0x263670's call from the first-block cut homes the argument in EBP and keeps it on the stack.
// Flag forms, none moves it: sample and update as long 428 (1624 B), update alone as long 417, all three flags long 428,
// the update flag dropped (diagnostic) 478 with the prop index in EBX; a short prior state re-measured 413. The walk
// through the existing 0x26be00/0x26be30 re-measured 442 at retail's 1616 bytes, fast and plain, aligned [198, 182, 141]
// (the two prototypes have to stand ahead of this function: declared inside the body the second one stays a call, 460 at
// 1592 B); with the iterator's index read at each use 442 (same code). On the walk form the whole difference at the loop
// head is one pair: this build keeps the prop index in EBX for the loop and loads the actor index into EDI from the
// stack, where retail keeps the actor index in EBX (the register 0x2683f0, 0x1e2150 and 0x25ac00 take it in) and reloads
// the prop index from the iterator's slot at every use (its EDI is free for the view pointer of cases 2 and 3).
// Diagnostic on the walk form, reverted: the iterator's address stored in a volatile pointer 434 (1624 B), the index
// still held in EBX across the calls, so it is not the iterator's storage that decides the pair. Not kept: the walk
// raises the count (412 -> 442) although it removes the hand-written step; the next round should start from the walk
// and look for what ranks the actor index above the prop index (retail's inlined 0x2641c0 is one read of it fewer).
// MiniMax C (11-19): no gain (412, baseline; body restored). Retail holds the actor index in EBX for the whole body and gives the prop index EDI with a stack home; ours keeps the argument in its stack home and reloads it, and the prop index takes EBX, which also leaves the frame at 0xa4 against retail 0xa0. The inlined 0x2641c0 inline decision belongs to another lane file.
// M3 lane 2 r115 (fast with 0x1e34e0; control 412; body unchanged). Why the actor index stays a stack argument: the
// argument goes in EAX exactly when no instruction of this body reads its stack home again (main's body reads it 12
// times, the walk form 12, see below), and three things in our build each force such reads. (1) Our link expands
// 0x25c820 inline at the release site (`mov word [prop + 0x24], 0` and a direct 0x25c780 call are in our listing; retail
// has neither, it calls 0x25c820 once, case 1 jumping to that call): the expanded body's 0x25c780 call takes the prop
// index in EBX, so EBX cannot hold the actor index across it. (2) The `update` byte takes BL. (3) The prop index is
// promoted to a register. Retail keeps the prop index and the next index in the iterator's cells (+0x14 / +0x18,
// reloaded at each use) and all three flags in stack bytes.
// Diagnostics, each reverted at once (D = an unreachable call taking a local's address, which keeps that local in
// memory; P = an inline_depth(0) pragma around the release site's 0x25c820 call): the walk through the existing
// 0x26be00 / 0x26be30 with the iterator's index read at each use re-measured 442 at 1616 B (12 home reads); + D on the
// iterator 453 (1644 B); + D on the iterator and the update flag 431 at 1616 B with retail's frame 0xa0 and 5 home
// reads left, all beside the expanded 0x25c820; that with the 0x25c820 calls cut out: no home read, entry `push ebx;
// mov ebx, eax` and 0x1e34e0 MATCHES; + D on both and P: 0x1e34e0 MATCHES (this function 545 at 1496 B, first
// difference +0x2b); P with D on the iterator only or on the flag only 498 / 497 (still stacked); P alone on the walk
// 497 (1552 B), on main's body 462 (1568 B). Other cuts on the D-both form (0x1e34e0 25 each): without the 0x2641c0
// call 549, without 0x1e5240 455, without the three callees that take the actor in EBX 504, without the two entry
// calls 458.
// Forms: case 1's 0x25c820 call and the release one as a single call (a `release_only` label) on main's body 457
// (1592 B), on the walk 465 (1592 B), on the walk with D on both 430 (1616 B, first difference +0x6; the single site
// is still expanded inline); the release calls written out in each arm with no goto 445 (1644 B; one of the four
// sites is expanded, the first in source order), on the walk 481 (1656 B). 0x2641c0's body expanded here through a
// forced-inline copy (diagnostic) 497 at 1688 B on main's body, the same bytes as dropping its noinline declaration
// (round 143), 492 (1672 B) on the walk, 668 (1812 B) without the three inline_depth pragmas: 0x25c820 is still
// expanded in each, so retail's call there does not follow from 0x2641c0's expansion using up an inline budget.
// BLOCKED-BY our link expanding 0x25c820 (and, without its pragmas, 0x25d700) inline where retail calls them; what
// keeps retail's iterator and flags in memory is open.
// M3 lane 14 r124 (fast with 0x1e34e0 and 0x25d740, plain-confirmed with the file's functions, which all still match;
// control 412): kept, 412 -> 404: the current-prop block after the loop as two tests of the field, each with its own
// view lookup, as retail compares the index with NONE twice (cmp esi, -1 at 0x26314f and 0x26316c); alone 404
// (1624 B). Kept with it: case 2's view through the existing 0x25d740 instead of its body written out (retail
// expands the getter there, with its null test of the element); on main's body alone 409 at retail's 1616 B, on the
// kept form 404 (same count). Not kept: the getter at the state-change site too 413 (on the kept form 427); the old
// state `short` 413 (on the kept form 404, no gain); a `long volatile` maximum read as `(short)` on the kept form 406,
// a plain `long` maximum 432; the maximum store written in both arms of the 0x350 test 421 (1644 B: retail's three
// epilogues, and maximum's slot below the actor's as retail, but the count rises); the final block as two ternary
// lookups on a `current` local 409 (the second test folds away); lane 6's walk through 0x26be00/0x26be30 on the case-2
// form 443 at 1616 B (aligned 310 against 319). Still BLOCKED-BY 0x2641c0's inline (retail expands it here; its
// noinline declaration is in another lane's file this round): the actor index arrives on the stack, not in EAX.
// M3 lane 16 r153 (fast with 0x1e34e0, 0x264330, 0x265680; control 404). Retail expands 0x2641c0 here (its actor
// lookup reuses the actor's scaled offset saved at entry, [esp+0x28]) and calls it everywhere else. New: the existing
// noinline declaration of 0x2641c0 dropped 509 (1696 B; 0x264210 then expands it too, 68 -> 139 B, lost; the other five
// callers stay matched; restored); `sample = update = true` as two statements 404 (same code); the final block on one
// `current` local tested twice 409 (1616 B, the second test folds away, as lane 14's ternary form); `sampled_one` declared
// ahead of `maximum` 404 (same code); `update` declared ahead of `sample` 402 (1620 B; a declaration order, not kept).
// M3 lane 1 r136 (fast with 0x1e34e0 25 throughout; control 404): retail's loop is top-tested (the NONE test of the next
// index at the loop head, every tail branch jumping back to it), ours is rotated; lane 6's walk gives that shape. Trials: 0x25c820
// reading the prop through prop_ref_get at each use (so our link would stop expanding it at the release site) 423 and 0x25c820
// itself 25 (restored); 0x2641c0's noinline declaration dropped with its perception index in a named local 511 (0x2641c0 2,
// 0x264210 still expands it, 116; restored); the motion and context records at function scope 404, at the loop-body top 404
// (same code each); the prop's fields through the typed s_prop_datum members (all but the short at +0xc) 404 (same code). Body
// unchanged.
// M3 lane 12 r159 (fast with 0x1e34e0 (25 throughout); control 404 at 1624 B): the release block's two calls under a
// redundant `if (index != NONE)` (the loop already guarantees it; a frequency diagnostic) 439 (1620 B, retail's frame
// 0xa0, but 0x25c820 still expands at the release site and the actor index stays stacked); only the release's 0x25c820
// call under that test 449 (1640 B); the last block's view lookup through `node` instead of a second prop lookup 430
// (retail's 1616 B); `motion` and `context` declared at function scope 404 (same code); the first `next` read from the
// `actor` local instead of a second actor lookup 442 (1616 B). Blockers unchanged: the release site's 0x25c820
// expansion and 0x2641c0's noinline declaration (dropping it expands 0x264210's call too).
// M3 lane 7 r201 (fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330 and 0x265680, all three MATCH; control 404 at
// 1624 B, register/offset-masked aligned 251): the release block reached through a `release` flag set in the switch
// and tested after it, in place of the two gotos, 442 (1644 B: the flag is not threaded on the case-3 paths); the
// sampling test with `update` ahead of `sample` 402 (1620 B; an operand order, not kept); `sampled_one` as a long 408
// (1628 B); the 0x264b50 test nested under its own `if (sample)` 404 (same code); the sampling result written as
// `if (!0x2641c0(...)) sample = false; else if (update && ...)` 460 (1652 B). Reading of the calls: 0x25c820 has six
// call sites in this build (three in its own file, two here, one in 0x263210, where it is expanded as retail's is),
// and retail leaves 0x25d610, 0x25da40, 0x25d690 and all three 0x25d700 sites as calls here while expanding 0x2641c0
// and the first 0x25d740 site only; ours wants every one of the small ones expanded (hence the three pragmas) and one
// 0x25c820 site, so the difference is how much this function is allowed to expand, not one callee's spelling.
// Blockers unchanged (0x2641c0's expansion, the release site's 0x25c820 expansion, the EAX convention).
// M3 lane 12 r162 (fast with 0x1e34e0, 25 throughout; control 404 at 1624 B): `update && sample` in the 0x264b50 test
// 402 (1620 B); `update` as a short 408 (1628 B); `update` read through a const reference at that test 404 (same code);
// case 1's 0x25c820 call jumping to the release block's call with a short `old_state` 452 (1600 B, frame 0xa0); that with
// `update && sample` 431 at retail's 1616 B. The actor index stays a stack argument in each. Body unchanged.
// M3 lane 12 r163 (H2M_FAST with 0x263210/0x264330 MATCH, 0x1e34e0 25 throughout; control 404): the tail's 0x25d690
// lookup through the loop's `node` instead of a second prop fetch 430 at 1616 B (frame still 0xa4: the saved prop handle
// behind the extra slot feeds our expanded 0x25c820 at the release site, not the tail); with a `short` old_state 431;
// with `update && sample` 431; the walk through 0x26be00/0x26be30 (0x263210's idiom) re-measured 443, with the node
// tail lookup 452 (1600 B), with the iterator's index read at each use as well 452. Body unchanged.
// M3 lane 3 r289 (fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330 and 0x265680, all three MATCH; control 404):
// 0..35 unused initialised locals at the top 404 each (no symbol-count state). Reading: retail keeps `sample` and `update`
// in adjacent stack bytes and stores the constant 1 to each, with the actor index in EBX for the whole function (so it
// arrives in EAX); ours gives BL to `update` inside the loop and reloads the actor index from its stack home. Both flags
// set through an inline helper taking their addresses 404 (same code); set as two statements, update first 404;
// `function_25d610(node)` tested ahead of `update` 524 (1708 B); the flags as two declarations 404; the 0x2641c0 result
// in its own bool 404; the old state as a short 404 again; the node fetched from `next` with a const index 404. Still
// BLOCKED-BY 0x2641c0's noinline declaration (src/unknown_265cb0.cpp, another lane's file this round): its expansion
// here is what keeps the actor index in EBX. Body unchanged.
// M3 lane 5 r223 (H2M_FAST with 0x1e34e0 (25 throughout) and 0x263210 (MATCH); control 404 at 1624 B): retail's case code
// order 0/4/2/1/3 with the release calls as the switch's last labelled arm (the tracking_complete goto gone) 412 (1628 B);
// that with case 1's 0x25c820 call jumping to the release block's 0x25c820 call 436 (1616 B, frame 0xa0, first
// difference +0x6: the actor index still read from the stack); that with a short old state 440 (1620 B); the walk
// through 0x26be00/0x26be30 re-measured 443 (1616 B), with the first form 447 (1620 B), with the second 442 (1616 B).
// Body unchanged; BLOCKED-BY 0x2641c0's inline as before.
// M3 lane 8 r212 (h2m fast with 0x1e34e0, 25 throughout; control 404): case 2's view in its own local 404, the completion
// block's view in its own local 404, the case-1 tracking pointer declared in its arm 404 (same code each: the shared
// `view` local has no register of its own); `bool update = false; bool sample = update;` 402 (1620 B, the declaration
// order effect listed above, not kept); case 3's limit as one conditional on the weapon test 404 (same code). Ours gives
// EBX to `sample` (the only callee-saved byte register, live across the 0x2640c0/0x2641c0 calls), so the actor index has
// no register and stays a stack argument. Still BLOCKED-BY 0x2641c0's expansion (its noinline declaration is in another
// lane's file).
// M3 lane 11 r147 (fast, single address, control 404): `update` and `sample` as two declarations, update first 402
// (1620 B, aligned 318 against 317: BL still holds update; a declaration order worth 2, not kept); the sampled flag
// declared ahead of the maximum 404; the two flags declared at the loop top ahead of the index 405; the old state read
// after the flags' declaration 404; the 0x25d610 test given the node view 404 (same code). Still BLOCKED-BY 0x2641c0's
// inline decision (rounds 114, 117 and 143: dropping its noinline loses 0x264210).
// M3 lane 4 r244 (h2m fast; control 404; body unchanged). Inline-cost reading (one-object link-time build of this file with
// 0x2641c0/0x264210's definitions and 0x25d690's object, threshold search; the build uses 81): with the noinline declaration
// dropped, 0x2641c0 expands here from 32, at 0x264210 from 52 and at 0x265680 between 72 and 81, so at 81 all three expand
// where retail expands only this one; spelling 0x2641c0 as a single exit, nested tests, an aggregate copy or a `result =
// true` default moves those to 32/52 or 34/54 (its cost follows the optimised body, not the written form); 0x264210's call
// under one or two folded levels 61/80, a named result 52 (it would need three levels). Trials: `sample`/`update` declared
// after the 0x263670 call 404; both at function scope 404; the walk as `for (index = first; index != NONE; index = next)`
// 473 (1580 B); diagnostics, noinline dropped: with the flags after the call 509 (1696 B, still `ret 4`: the expansion alone
// does not give retail's EAX convention), with the `for` walk 454 (1640 B, `ret 4`). BLOCKED-BY 0x2641c0's expansion and the
// stacked actor index, as before.
// M3 lane 1 r152 (h2m fast with 0x1e34e0 25 throughout; control 404): the sampler's result tested directly, `sample = false`
// only on failure (`if (!0x2641c0(...)) sample = false; else if (update && ...)`) 460 (1652 B), the same with the success
// arm first and the failure in an else 460 (retail stores the false only on the failure path, but the form adds 28 bytes);
// a `__forceinline` redeclaration of 0x2641c0 just ahead of this body 404 (same code: the definition's noinline wins, and
// 0x265680 / 0x264210 / 0x2641c0 stay matched); case 3's decrement as an if/else on the tracked-prop test 404 (same code);
// the closing reset as a flag set under the view test with one shared maximum store 421 (1644 B).
// M3 lane 13 r142 (h2m fast with 0x1e34e0, 25 throughout; control 404): the release calls written at case 3's underflow test
// (retail's block order: its `jge` skips them) with case 2's and case 3's first gotos jumping there, `goto tracking_complete`
// gone and the decrement written in the subtraction, 404 (the same code: the release block is placed and 0x25c820 expanded
// exactly as before, so written depth does not move that inline here); diagnostic, the three flags as members of one local
// aggregate 394 (1616 B, frame 0xa0 as retail, `update` still scalarised into BL, so the actor index stays stacked; a
// grouping steer, not kept); `update` as a long 404 (it keeps EBX as a long); the 0x2641c0 result tested directly with
// `sample = false` only on its failed path (retail's store) 460 (1652 B), with the long `update` 454 (1648 B). All restored.
// M3 lane 15 r130 (H2M_FAST with 0x1e34e0 25 and 0x263210 MATCH throughout; control 404): `maximum` as a plain word,
// long, dword or short 432 each (1620 B); the tracked prop index (actor +0x338) read once into a named local for its two
// tests 423 (1616 B, frame 0xa0); the maximum compare written `(short)maximum < value` 405; `update` assigned false in
// the else arm instead of at its declaration 439 (1616 B); both flags assigned false in the else arm 428. Diagnostic,
// reverted at once: the flags as volatile locals 428 / 418 (EBX then goes to the prop index) and every form keeps
// `ret 4`, so the flags' register is not what keeps the actor index on the stack; the retail call graph has no cycle
// through this function or its caller.
// M3 lane 13 r143 (h2m fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, all three MATCH; control 404):
// the byte view `p` fetched by its own prop_ref_get instead of a copy of `node` 438 (1624 B); the release pair through a
// file-local forceinline helper at the label 404 (same code: 0x25c820 still expanded there); the two flags declared ahead
// of the prop lookup 404 (same code); `update` declared on its own just ahead of the sampling test, after the 0x263670
// call 405; `index` declared at function scope and assigned at the loop top 404 (same code). Retail's listing keeps all
// three flags (+0x11/+0x12/+0x13), `next` (+0x18) and `maximum` (a dword at +0x1c) in memory with EBX the actor index
// for the whole function. All restored.
// M3 lane 3 r298 (h2m fast with 0x1e34e0, 25 there throughout; control 404 at 1624 B): `sample`/`update` as `char` 404;
// the prop index `long const` 404; the 0x25d610 test nested under `if (sample && update)` 404; the sample counter through
// a `short &` bound to its field 404 (same code each); diagnostic, the prop walk as a do-while under `if (next != NONE)`
// 464 (1584 B). The actor index stays a stack argument in each. Still BLOCKED-BY 0x2641c0's noinline declaration.
// M3 lane 6 r232 (h2m fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, all three MATCH; control 404):
// sample/update declared at function scope and cleared at the loop top 404 (re-measured); `short old_state` 404 (same code); the
// flags declared `update, sample` 402 (1620 B; EBX still the sample flag in the loop); a `long const` actor parameter 404;
// sampled_one declared ahead of maximum 404. All restored.
// M3 lane 3 r300 (h2m fast with 0x1e34e0 (25 throughout), 0x2641c0, 0x264210, 0x265680, 0x263210, 0x264330, all MATCH;
// control 404): the maximum as `short volatile` 404; `short old_state` 404; `long const index` 404; the byte view declared
// ahead of the node pointer 404; the state switch's cases in numeric order 404 (all the same code). Diagnostic: 0x2641c0's
// noinline declaration dropped 509 (1696 B; 0x264210 114). All restored.
// M3 lane 8 r229 (h2m fast with 0x1e34e0 (25 throughout), 0x263210, 0x263670, 0x264210, 0x2641c0, 0x264330, 0x265680,
// 0x25c820, 0x26be00 and 0x26be30, all MATCH unless said; control 404 at 1624 B, aligned 317, register-masked 278; body
// unchanged; every form keeps `ret 4`). New forms: the state switch as a forceinline predicate (false = release) with the
// release pair under its test, in place of the two gotos, 470 (1656 B, frame 0xa0), on the walk through 0x26be00/0x26be30
// 445 (1620 B); an unread actor-index parameter added to 0x263670 (definition, declaration and call; 0x263670 stays
// matched) 404 (the same code: a parameter the callee drops is no reference for the EBX choice), on the walk 443; the
// sampler's result tested directly on the walk 442 (1628 B), with the predicate 443 (1636 B), on this body 460 (re-measure).
// Diagnostics, each reverted at once (N = 0x2641c0's noinline declaration dropped, which loses 0x264210 at 114 each time):
// N 509 (1696 B); N with the release pair written in each arm 572 (1744 B), also without the three inline_depth pragmas
// 758 (1892 B); N on the walk 497 (1680 B, frame 0xa0, masked 242), with the release pair in each arm 534 (1704 B), also
// without the pragmas 720 (1852 B); the release pair in each arm alone 488 (1680 B), on the walk 548 (1712 B); N with the
// predicate 536 (1724 B), on the walk 538 (1704 B, masked 239); N with the sampler tested directly on the walk 529 (1696 B,
// masked 233), with the predicate 545 (1712 B); N with the unread parameter 509, on the walk 497 (same code as without it).
// One unconditional reference to the actor index at the loop top (a store to a global, diagnostic) 455 (1636 B), on the
// walk 406 (1628 B), with N on the walk 495; a second one at the loop tail 436 (1616 B, frame 0xa8), on the walk 415 (1620 B,
// frame 0xa0), with N on the walk 522: unconditional loop references do not move the argument off the stack either.
// Reading of the calls: with the sampler expanded on the walk this build makes retail's 34 calls plus one, and the one
// is the release site, where 0x25c820 is expanded (its inner 0x25c780 call) beside case 1's own 0x25c820 call; retail
// has one 0x25c820 call that both paths share. Every form above keeps exactly one expansion of 0x25c820 (the release
// pair in each arm expands the first site in source order only). Retail clears the sample flag only on the sampler's failed
// path and reads the update flag from its stack byte after the expanded sampler; it reloads the prop index from the
// iterator's cell at fifteen sites, because the expanded sampler's 0x38-byte copy and 0x2640c0's output register take
// ESI and EDI inside the loop. Still BLOCKED-BY 0x2641c0's expansion (0x264210 expands it too once the noinline
// declaration goes) and the release site's 0x25c820 expansion.
// M3 lane 16 r183 (h2m fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, all MATCH; control 404): the
// context record declared ahead of the motion record 404; the two flags declared after the 0x263670 call 404; the prior
// state read ahead of the next link 404; case 1's tracking pointer declared in its own block 404; the prior state as a
// `word` at both uses 404 (all the same code). All restored.
// M3 lane 5 r242 (H2M_FAST with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, 0x2641c0, 0x264210 and
// 0x264b50; control 404 at 1624 B; body unchanged). The allocator's ranking in this body: the node 634 (EBP); the
// `update` byte 30, which takes EBX in the first pass because BL is the only byte register that survives the calls; the
// actor index over its whole range -56 (its references add 228, the 61 blocks it only lives through take 284), so it is
// cut into pieces and its stack home is read again; the prop index 134 once split (EDI). With `update` kept out of the
// registers (a forced compile, not source) the frame is retail's 0xa0 and the first difference moves to +0x6, the
// argument's stack read; with the actor index's largest piece ranked first instead, the node moves to EBX and the count
// rises (440). So the frame follows from `update` losing BL, as it does once the sampler is expanded between its store
// and its test (four or five more blocks it lives through unread), and the register argument needs the actor index's
// whole range above zero on top of that. Forms: the tail's maximum store written in each exit (retail has three
// returns), as `if (...) { ++; store; return; } store;` 421 (1644 B), as if/else arms 421 (same code), with the NONE
// test first 421 (same code); `sample = true; update = true;` as two statements 404 (same code); the node fetched from
// `next` ahead of `long index = next;` 438 (1616 B, retail's length). Still BLOCKED-BY 0x2641c0's expansion (its
// noinline declaration is in a file another lane holds this round) and the release site's 0x25c820 expansion.
// M3 lane 6 r240 (H2M_FAST with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, 0x2641c0, 0x264210, 0x264b50,
// 0x263670 and 0x25c820, all MATCH unless said; control 404 at 1624 B, aligned 210, 35 calls against retail's 34, frame
// 0xa4). Diagnostics on the blocker, each reverted at once (N = 0x2641c0's noinline declaration dropped, 0x264210 114 each
// time): N re-measured 509 (1696 B, aligned 202, retail's 34 calls, frame 0xa8); N without the three inline_depth pragmas
// 696 (1844 B, 31 calls); that with 300 unused initialised longs at the top 696 (same code: the symbol count does not
// move the expansions); N with 10 or 40 unused prop_ref_get(index) expansions at the loop top 509 / 509 (same code: extra
// expanded accessors ahead of the release site do not use up what lets 0x25c820 expand there). Forms on main's body: the
// motion and context records declared at function scope 404; `view` declared ahead of `tracking` 404; the 0x264b50 test
// nested under its own `if (sample)` 404 (the same code each). Still BLOCKED-BY 0x2641c0's expansion here (and not in
// 0x264210) and the release site's 0x25c820 expansion.
// Sampling update derived from the prior and current walk state, with bool/byte/short/long
// snapshots before node refresh 427/427/444/440 and after refresh 440/440/438/440.
// The later short and long reach frame 0xa0 and 1616 bytes, with a stacked actor and higher scores.
// M3 lane 12 r187 (H2M_FAST with 0x263210, 0x264330, 0x265680, 0x2641c0 and 0x264210; control 404 at 1624 B, aligned
// unmatched rows [exact, stack masked, registers masked] 209/184/143; body unchanged). `short old_state` 404
// [209/185/141] (the word store is still a dword here); the first prop read from `actor + 0x58` instead of a second
// actor lookup 442 (1616 B) [220/195/147]; `sampled_one = true` ahead of the counter's reset 405; the closing step as
// `+= 1` 404 (same code); the changed-state view through the existing 0x25d740 428 (1628 B); the maximum compare with
// its sides exchanged 405. Diagnostic, reverted at once: 0x2641c0's noinline declaration dropped 509 (1696 B)
// [201/174/132], 0x264210 lost; there retail's two flag bytes and the actor index in EBX are still missing and the
// switch's arms sit in another order (retail lays case 0 and case 4 out first and puts the expanded sampler's failed
// path after the 0x264b50 call), so the expansion alone leaves 132 structural rows. Still BLOCKED-BY 0x2641c0's
// expansion here and not in 0x264210 or 0x265680 (the build's inline threshold expands all three) and by the release
// site's 0x25c820 expansion.
// M3 lane 2 r151 (fast screen with 0x1e34e0, 25 in every form; control 404 at 1624 B): the 0x2641c0 call written out as
// its body (a diagnostic, reverted at once) 509 (1696 B), and 0x1e34e0 still pushes the actor index; the update flag
// computed from the sampling test after the 0x263670 call with `sample` initialised from it 453 (1636 B); both flags
// declared after the 0x263670 call 404 (same code); the parameter copied into a local at entry 404 (same code); both
// flags declared at function scope and cleared at the loop top 402 (1620 B, two lower: not kept).
// M3 lane GA r8 (fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, 0x2641c0, 0x264210, 0x264b50,
// 0x263670 and 0x25c820, all 8 MATCH each run; control 404 at 1624 B, aligned 292, register-and-offset-masked 248; body
// unchanged). New forms on the 404 body: plain retail case order 0/4/2/1/3 (release label still outside switch) 412
// (1628 B, aligned 192, masked 122); case 2 distance through existing vector2d_from_points2d + dot_product2d 416 (1624 B,
// retail's y-first + dead `add eax, 4` restored); case 0/4 progress through `real delta = function_2683f0(...);
// *(real *)(p + 0x10) += delta;` 432 (1632 B: recovers retail's `movss xmm1, [ebp+0x10]; addss xmm1, xmm0; comiss xmm1,
// 1.0` at both sites, unlike round 119's `field = field + call`); pre-call `real prev = *(real *)(p + 0x10);` 432 (same
// code); sequential `if (!0x2641c0(...)) sample = false; if (sample && update && ...)` 460 (1652 B); retail case order
// 0/4/2/1/3 + vec2d 428 (1632 B, aligned 190, masked 118), + tail's 3 returns 441 (1652 B, aligned 197, masked 112);
// case 3 nested `if (view) { if (!view[0x88]) ... }` 404 (same code); retail case order 0/4/2/1/3 with release calls
// under case 3's `< 0` arm (case 3 last, `goto tracking_complete` gone) 412 (1628 B, aligned 192, masked 122); full
// retail-shaped combination (`0x26be00`/`0x26be30` walk + 0/4/2/1/3 + vec2d + `short old_state` + `0x25d740` state-change
// + release-in-case-3) 455 (1628 B, frame 0x9c, aligned 159, masked 95), + 3 tail returns + split true flags + `delta`
// 463 (1656 B, aligned 146, masked 86). Still BLOCKED-BY 0x2641c0's noinline declaration (src/unknown_265cb0.cpp) and
// the release site's 0x25c820 expansion.
// M3 lane 8 r239 (h2m fast with 0x1e34e0 (25 throughout), 0x263210, 0x264330, 0x265680, all MATCH; control 404 at
// 1624 B, aligned 317). A compile-only all-stack screen of this file counts the actor index's home reads: main 6 (the
// index in EDI, given up to the case 2, case 3 and completion view pointers). The update flag at function scope, false
// at entry and cleared again after its one use in the sampling block, gives 1 there (the actor index then held in EBX
// as retail) but 431 here at retail's 1616 B and 0xa0 frame (aligned 331; the actor index is still a stack argument
// and EBX goes to the prop index in the loop); cleared after the sampling block instead 431; the 0x264b50 test as
// `if (sample && update) if (0x25d610(node)) ...` 404 (same code); `actor[0x48] != 0` in the sampling test 404 (same
// code); the 0x26be00/0x26be30 walk (round 220's form) re-measured 446 at 1632 B, with the function-scope update flag
// 447. So holding the actor index in a register in that screen does not make it a register argument here. Still
// BLOCKED-BY 0x2641c0's noinline declaration (src/unknown_265cb0.cpp, another lane's file this round).
// @retail 0x262ba0
void function_262ba0(long actor_index)
{
	byte *actor = (byte *)actor_get(actor_index);
	/* volatile keeps maximum in its stack word, as retail does: 420, against 426 as a plain word (see the note) */
	word volatile maximum = 1;
	bool sampled_one = false;
	if (actor[0x44]) function_263210(actor_index);
	function_265680(actor_index);
	long next = *(long *)((byte *)actor_get(actor_index) + 0x58);
	while (next != NONE)
	{
		long index = next;
		s_prop_datum *node = prop_ref_get(index);
		byte *p = (byte *)node;
		next = *(long *)(p + 0x2c);
		long old_state = *(short *)(p + 0x24);
		bool sample = false, update = false;
		function_263670(node);
		if (!sampled_one && *(short *)(p + 0xc) >= *(short *)(actor + 0x50) && actor[0x48])
		{
			sample = update = true;
			*(short *)(p + 0xc) = 0;
			sampled_one = true;
		}
		else
		{
			*(short *)(p + 0xc) += g_470f10[*(short *)(p + 0x18)].unknown2;
			if (index == *(long *)(actor + 0x338)) *(short *)(p + 0xc) += 25;
		}
		if (*(short *)(p + 0xc) > (short)maximum) maximum = *(word *)(p + 0xc);
		if (index == *(long *)(actor + 0x338))
		{
			*(short *)(p + 0x1a) = 3;
			sample = true;
		}
		s_2640c0 motion;
		s_2641c0 context;
		if (sample)
		{
			function_2640c0(*(long *)(p + 0x20), (s_object_motion_view *)&motion);
			sample = function_2641c0(actor_index, &context, &motion.field_c);
			if (sample && update && function_25d610(node)) function_264b50(actor_index, index, &context, &motion);
		}
		s_type_f95cd3 *tracking;
		byte *view;
		switch (*(short *)(p + 0x24))
		{
		case 2:
			if (!function_25da40(node)) goto release_tracking;
			else if ((char)p[0x27] > 0) function_25c230(actor_index, index, NONE);
			else
			{
				view = (byte *)function_25d740((s_prop_node *)node);
				point3f position;
				function_b9dd0(*(long *)(p + 0x20), &position);
				if (view && *(short *)(view + 0xa0))
				{
					byte *state = (byte *)function_25d690(node);
					real y = *(real *)(state + 8) - position.y;
					real x = *(real *)(state + 4) - position.x;
					if (!(y * y + x * x > 1.0f)) break;
				}
				function_25c860(index);
			}
			break;
		case 1:
			if (!p[0x27] && function_25d610(node))
			{
				if (function_25da40(node))
				{
					tracking = function_25d740((s_prop_node *)node);
					*(short *)(p + 0x24) = 2;
					if (tracking) *(short *)((byte *)tracking + 0xa0) = (short)real_to_long((real)g_510c54->field_2_3 * 0.33f);
				}
				else function_25c820(index, actor_index);
			}
			break;
		case 3:
			if (!function_25da40(node)) goto release_tracking;
#pragma inline_depth(0)
			view = (byte *)function_25d700(index);
#pragma inline_depth()
			if (view && !view[0x88])
			{
				real limit = 1.5f;
				byte *weapon = (byte *)function_1e5240(actor_index);
				if (weapon && *(real *)(weapon + 0x1c) > 0.0f) limit = 10.0f;
				if (*(short *)(view + 0x8a) >= 2)
				{
					++*(short *)(view + 0x90);
					if ((real)*(short *)(view + 0x90) * g_510c54->rate >= limit) view[0x88] = true;
				}
				else *(short *)(view + 0x90) = 0;
			}
			if ((char)p[0x27] > 0) { *(short *)(p + 0x24) = 4; *(real *)(p + 0x10) = 0.0f; }
			else if (view)
			{
				long decrement = index == *(long *)(actor + 0x338) ? (view[0x69] ? 2 : 0) : (view[0x69] ? 3 : 1);
				*(short *)(view + 0x8e) -= (short)decrement;
				if (*(short *)(view + 0x8e) < 0) goto release_tracking;
			}
			break;
		case 0:
			if (*(short *)((byte *)prop_get(*(long *)(p + 8)) + 4) == 1 && !g_470f10[*(short *)(p + 0x18)].kind)
			{
				function_25c230(actor_index, index, 0);
				*(real *)(p + 0x10) = 1.0f;
			}
			else if ((char)p[0x26] > 0)
			{
				*(real *)(p + 0x10) += function_2683f0(actor_index, index, (char)p[0x26]);
				if (*(real *)(p + 0x10) >= 1.0f)
				{
					function_25c230(actor_index, index, 0);
					if ((char)p[0x27] < 1 && function_25da40(node)) function_25c860(index);
					*(real *)(p + 0x10) = 1.0f;
				}
			}
			else *(real *)(p + 0x10) = 0.0f;
			break;
		case 4:
			if (!p[0x27]) { function_25c860(index); *(real *)(p + 0x10) = 0.0f; }
			else
			{
				*(real *)(p + 0x10) += function_2683f0(actor_index, index, (char)p[0x26]);
				if (*(real *)(p + 0x10) >= 1.0f) function_25c230(actor_index, index, NONE);
			}
			break;
		}
		goto tracking_complete;
release_tracking:
		function_1e2150(actor_index, index);
		function_25c820(index, actor_index);
tracking_complete:
		if (old_state != *(short *)(p + 0x24))
		{
			view = *(long *)(p + 0x14) == NONE ? NULL : (byte *)&tracking_get(*(long *)(p + 0x14))->view;
			if (view) *(real *)(view + 0x3c) = function_265d30(actor_index, index);
		}
		if (sample) function_264330(actor_index, index, &context, &motion, false);
		if (((byte *)function_25d690(prop_ref_get(index)))[0x68] && *(short *)(p + 0x24) >= 1)
			function_25ac00(index, actor_index);
	}
	if (*(long *)(actor + 0x338) != NONE)
	{
#pragma inline_depth(0)
		byte *view = (byte *)function_25d700(*(long *)(actor + 0x338));
#pragma inline_depth()
		if (view && *(short *)view >= 5) actor[0x34c] = true;
	}
	if (*(long *)(actor + 0x338) != NONE)
	{
#pragma inline_depth(0)
		byte *view = (byte *)function_25d700(*(long *)(actor + 0x338));
#pragma inline_depth()
		if (view && *(short *)view >= 6) { *(long *)(actor + 0x350) = 0; *(short *)(actor + 0x50) = maximum; return; }
	}
	if (*(long *)(actor + 0x350) != NONE) ++*(long *)(actor + 0x350);
	*(short *)(actor + 0x50) = maximum;
}

struct s_tracking_candidate
{
	long index;
	short priority;
};

void prop_state_initialize(s_type_5cfb45 *state);

/* the walk over an actor's props: 0x26be00 starts it, 0x26be30 returns the
   next prop (NULL at the end) */
struct s_node_view;
void function_26be00(long actor_index, s_iterator *iterator);
s_node_view *function_26be30(s_iterator *iterator);

// @retail 0x263210
void __stdcall function_263210(long actor_index)
{
	s_iterator iterator;
	function_26be00(actor_index, &iterator);
	short count = 0;
	short active_count = 0;
	short current = 0;
	bool reclaimed = false;
	s_tracking_candidate candidates[50];
	s_2640c0 motion;
	s_2641c0 sample;
	s_actor_prop_view *actor = actor_prop_view_get(actor_index);
	++g_4de2fc;
	g_4de2f8 = true;
	long *tracked = actor->tracked_prop_indices;
	long slots_remaining = 8;
	do
	{
		long index = *tracked;
		if (index != NONE)
		{
			s_prop_datum *reference = (s_prop_datum *)datum_get_inlined(g_502418, index);
			if (reference && reference->tracking_index != NONE)
			{
				long absolute_index = reference->object_index & 0xffff;
				if (g_4de300[absolute_index] != g_4de2fc)
					g_4de300[absolute_index] = g_4de2fc;
				short priority = *(short *)((byte *)prop_get(reference->prop_index) + 0xc);
				reference->unknown1a = priority;
				if (priority > 0 && g_470f10[reference->type].unknown8 == 2)
				{
					candidates[count].index = index;
					candidates[count].priority = priority;
					count++;
				}
				else
					function_25c780(actor_index, index);
			}
			else
				*tracked = NONE;
		}
		tracked++;
	}
	while (--slots_remaining);
	s_prop_datum *reference;
	while ((reference = (s_prop_datum *)function_26be30(&iterator)) != NULL)
	{
		if (reference->state >= 1)
		{
			long absolute_index = reference->object_index & 0xffff;
			if (g_4de300[absolute_index] != g_4de2fc)
			{
				g_4de300[absolute_index] = g_4de2fc;
				short priority = *(short *)((byte *)prop_get(reference->prop_index) + 0xc);
				reference->unknown1a = priority;
				if (g_470f10[reference->type].unknown8 == 2 && count < 50)
				{
					candidates[count].index = iterator.index;
					candidates[count].priority = priority;
					count++;
				}
			}
		}
	}
	g_4de2f8 = false;
	if (count > 8)
		qsort(candidates, count, sizeof(s_tracking_candidate), function_2631f0);
	while (active_count < 8 && current < count)
	{
		long index = candidates[current].index;
		s_prop_datum *reference = prop_ref_get(index);
		if (reference->tracking_index == NONE)
		{
			reference->tracking_index = record_pool_allocate(g_502414);
			if (reference->tracking_index == NONE && !reclaimed)
			{
				if (function_25db60())
					reference->tracking_index = record_pool_allocate(g_502414);
				reclaimed = true;
			}
			if (reference->tracking_index != NONE)
			{
				s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
				prop_state_initialize(&tracking->state);
				prop_view_initialize(&tracking->view);
				function_2640c0(reference->object_index, (s_object_motion_view *)&motion);
				byte *local_bbc190 = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
				bool sampled = true;
				if (local_bbc190[7] && *(long *)(local_bbc190 + 0x1c) != NONE)
					sampled = function_28fa60(*(long *)(local_bbc190 + 0x1c), &motion.field_c, (s_actor_object_sample *)&sample);
				else
					memcpy(&sample, local_bbc190 + 0x22c, sizeof(sample));
				if (sampled)
				{
					function_264330(actor_index, index, &sample, &motion, false);
				}
			}
		}
		if (reference->tracking_index != NONE)
			actor->tracked_prop_indices[active_count++] = index;
		current++;
	}
	for (; active_count < 8; active_count++)
		actor->tracked_prop_indices[active_count] = NONE;
	for (; current < count; current++)
	{
		long index = candidates[current].index;
		if (prop_ref_get(index)->tracking_index != NONE)
			function_25c820(index, actor_index);
	}
}

long function_baf40(long object_index);
short __stdcall function_1c8df0(short source_cell, point3f const *source, point3f const *target,
	short target_cell, long object_index, long mode, bool narrow, bool attached, bool alternate,
	long *hit_object);
short function_25c9d0(long actor_index, long prop_ref_index, bool player, short sight,
	void *point, byte *sample, long reported);
short function_25cb60(long actor_index, long object_index, short type, void *sample,
	void *point, long location, short sight);
short function_25cca0(long prop_ref_index);
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
real function_296600(long actor_index, long prop_ref_index);

// Updates one actor's view of a prop: the sight result, the awareness levels and the last-seen times.
// Forms the match depends on: the root lookup is written in both arms of the unit test (retail merges the two
// calls and keeps the root in memory), `!current` is written at both sight calls, the 0x25c9d0 call has its own
// unread output flag (it shares the suppression flag's byte once that is dead), the game time is read after the
// object lookup, and the other unit's actor index is read from its field at both uses.
// @retail 0x264b50
void __stdcall function_264b50(long actor_index, long prop_ref_index, s_2641c0 *sample, s_2640c0 *motion)
{
	byte *actor = (byte *)actor_get(actor_index);
	if (!actor[9])
		return;
	byte *squad;
	if (*(long *)(actor + 0x30) == NONE)
		squad = NULL;
	else
		squad = g_51e9d8->data + (*(long *)(actor + 0x30) & 0xffff) * 0x98;
	s_prop_datum *reference = prop_ref_get(prop_ref_index);
	s_type_f95cd3 *view = NULL;
	if (reference->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
		if (tracking)
			view = &tracking->view;
	}
	s_type_76cf92 *prop = prop_get(reference->prop_index);
	byte *object = *(byte **)(g_4e0300->data + (reference->object_index & 0xffff) * 12 + 8);
	long time = g_510c54->game_time;
	byte *unit = NULL;
	bool suppressed = (squad && (squad[2] & 1)) || *(short *)(actor + 0x84) == 1 || actor[0x228];
	bool current = *(long *)(actor + 0x338) == prop_ref_index;
	if (object[0xaa] == 0)
		unit = object;
	if (*(short *)(actor + 4) == 15 && *(long *)(actor + 0x26c) != NONE)
	{
		byte *parent = *(byte **)(g_4e0300->data + (*(long *)(actor + 0x26c) & 0xffff) * 12 + 8);
		long other_unit = *(long *)(parent + 0x248);
		long other_index = NONE;
		if (other_unit != NONE && other_unit != *(long *)(actor + 0x18))
		{
			byte *other = *(byte **)(g_4e0300->data + (other_unit & 0xffff) * 12 + 8);
			if (*(long *)(other + 0x12c) != NONE)
				other_index = function_25d810(reference->object_index, *(long *)(other + 0x12c), false);
		}
		if (other_index != NONE)
		{
			s_prop_datum *other_reference = prop_ref_get(other_index);
			s_type_f95cd3 *other_view = function_25d740((s_prop_node *)other_reference);
			reference->unknown26 = other_reference->unknown26;
			reference->unknown27 = other_reference->unknown27;
			if (view)
			{
				if (other_view)
				{
					view->unknown06 = other_view->unknown06;
					*(short *)((byte *)view + 2) = *(short *)((byte *)other_view + 2);
					*(short *)((byte *)view + 4) = *(short *)((byte *)other_view + 4);
					view->unknown8a = other_view->unknown8a;
					view->unknown8c = other_view->unknown8c;
				}
				else
				{
					view->unknown06 = 4;
					view->unknown8c = 4;
					*(short *)((byte *)view + 4) = 0;
					*(short *)((byte *)view + 2) = 0;
					view->unknown8a = 0;
				}
			}
		}
		else
		{
			reference->unknown27 = 0;
			reference->unknown26 = 0;
			if (view)
			{
				view->unknown06 = 4;
				view->unknown8c = 4;
				*(short *)((byte *)view + 4) = 0;
				*(short *)((byte *)view + 2) = 0;
				view->unknown8a = 0;
			}
		}
	}
	else
	{
		long root;
		if (unit && *(long *)(unit + 0x14) != NONE)
			root = function_baf40(*(long *)(unit + 0x14));
		else
			root = function_baf40(reference->object_index);
		short sight = function_1c8df0(*(short *)((byte *)sample + 0x28), (point3f const *)sample,
			&motion->field_0, *(short *)((byte *)motion + 0x28), root, prop->unknown25 && prop->unknown23 ? 2 : 0, false, *(long *)(actor + 0x26c) != NONE, !current,
			current ? (long *)(actor + 0x348) : NULL);
		if (*(long *)(actor + 0x7c) != NONE && prop->unknown25 && 25.0f > reference->unknown28 &&
			(sight == 0 || sight == 1))
			function_268c60(*(long *)(actor + 0x7c));
		if (view)
			view->unknown06 = (short)sight;
		if (!prop->unknown26[0] && !suppressed)
		{
			bool out_of_range;
			short priority = (short)function_25c9d0(actor_index, prop_ref_index, prop->unknown25, sight,
				sample, (byte *)motion, (long)&out_of_range);
			if (reference->state >= 3 && view)
			{
				s_type_5cfb45 *state = function_25d690(reference);
				if (!view->unknown88)
				{
					short other_sight = function_1c8df0(*(short *)((byte *)sample + 0x28),
						(point3f const *)sample, (point3f const *)((byte *)state + 0x30),
						*(short *)((byte *)state + 0x2c), root, prop->unknown25 && prop->unknown23 ? 2 : 0, false, *(long *)(actor + 0x26c) != NONE, !current,
						current ? (long *)(actor + 0x348) : NULL);
					view->unknown8c = (short)other_sight;
					view->unknown8a = function_263810((point3f const *)sample, actor_index, (point3f const *)((byte *)state + 0x30), NULL, 2, (short)other_sight, true, NULL);
				}
				else
				{
					view->unknown8a = 3;
					if (current)
						*(long *)(actor + 0x348) = NONE;
				}
			}
			short object_priority = function_25cb60(actor_index, reference->object_index, reference->unknown1c,
				sample, &motion->field_c, (long)&motion->field_24, sight);
			short extra_priority = function_25cca0(prop_ref_index);
			reference->unknown27 = (char)priority;
			if (view)
			{
				*(short *)((byte *)view + 2) = (short)object_priority;
				*(short *)((byte *)view + 4) = (short)extra_priority;
			}
			reference->unknown26 = (char)MAX(priority, MAX(object_priority, extra_priority));
			if (reference->unknown26 == 1 && reference->state >= 1)
				reference->unknown26 = 2;
		}
		else
		{
			reference->unknown27 = 0;
			reference->unknown26 = 0;
			if (view)
			{
				*(short *)((byte *)view + 4) = 0;
				*(short *)((byte *)view + 2) = 0;
			}
		}
	}
	if (view)
	{
		if (reference->unknown26 >= 2)
			*(long *)((byte *)view + 0xc) = time;
		if (reference->unknown27 > 0)
		{
			function_26bfa0(reference->object_index, NULL, (s_location_view *)((byte *)view + 0x18));
			view->unknown10 = time;
		}
		*(real *)((byte *)view + 0x3c) = function_265d30(actor_index, prop_ref_index);
		*(real *)((byte *)view + 0x40) = function_296600(actor_index, prop_ref_index);
	}
	function_25d690(reference)->unknown68 = 1;
}


long function_1e4a10(long index);

// @retail 0x265550
bool __stdcall function_265550(long actor_index, long prop_ref_index)
{
 byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
 byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
 bool result = false;
 real range = 0.0f;
 byte *definition = (byte *)function_1e4a10(*(long *)(actor + 0x54));
 if (definition)
  range = *(real *)(definition + 0x64);
 if ((double)range > 0.0 && *(short *)(reference + 0x24) >= 1 && range + 10.0f > *(real *)(reference + 0x28))
 {
  short state = *(short *)(actor + 0x358);
  if (state < 1 || (state == 1 && *(long *)(actor + 0x360) != *(long *)(reference + 0x20) &&
   *(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
  {
   byte *prop = g_50241c->data + (*(long *)(reference + 8) & 0xffff) * 0xc4;
   memset(actor + 0x358, 0, 0x58);
   *(short *)(actor + 0x358) = 1;
   *(long *)(actor + 0x360) = *(long *)(reference + 0x20);
   *(real *)(actor + 0x36c) = range;
   *(short *)(actor + 0x35a) = !prop[0x23];
   function_25b910(actor_index, prop_ref_index);
   result = true;
  }

 }
 return result;
}



bool function_2651e0(long object_index, short *volatile output_index);

// @retail 0x265290
bool __stdcall function_265290(long actor_index, long prop_ref_index)
{
 byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
 byte *reference = g_502418->data + (prop_ref_index & 0xffff) * 0x3c;
 long object_index = *(long *)(reference + 0x20);
 byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
 byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
 bool result = false;
 bool const *result_reference = &result;
 if (*(long *)(object + 0x248) == NONE && 13.5f > *(real *)(reference + 0x28))
 {
  short state = *(short *)(actor + 0x358);
  if (state < 4 || (state == 4 && *(long *)(actor + 0x360) != object_index &&
   *(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
  {
   result = function_2651e0(object_index, NULL);
   if (*result_reference)
   {
    memset(actor + 0x358, 0, 0x58);
    *(short *)(actor + 0x358) = 4;
    *(long *)(actor + 0x360) = *(long *)(reference + 0x20);
    *(real *)(actor + 0x36c) = 3.5f;
    *(short *)(actor + 0x35a) = 0;
    *(long *)(actor + 0x368) = prop_ref_index;
    *(long *)(actor + 0x364) = NONE;
    function_25b910(actor_index, prop_ref_index);
    goto done;
   }
  }
 }
 long other_index = *(long *)(actor + 0x26c);
 if (other_index != NONE)
 {
  byte *other = *(byte **)(g_4e0300->data + (other_index & 0xffff) * 12 + 8);
  byte *other_definition = g_4e3b44[*(long *)other & 0xffff].bytes;
  bool lower = *(short *)(other_definition + 0x244) < *(short *)(definition + 0x244);
  bool const *lower_reference = &lower;
  if (!*lower_reference)
   goto done;
 }
 if ((bool)(((dword)*(dword *)(definition + 0x1ec) >> 7) & 1))
 {
  vector3f *velocity = (vector3f *)(object + 0x88);
  if (velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k > 1.0f &&
   *(real *)(definition + 4) + 10.0f > *(real *)(reference + 0x28))
  {
   short state = *(short *)(actor + 0x358);
   if (state < 3 || (state == 3 && *(long *)(actor + 0x360) != *(long *)(reference + 0x20) &&
    *(real *)(actor + 0x394) > *(real *)(reference + 0x28)))
   {
    memset(actor + 0x358, 0, 0x58);
    *(short *)(actor + 0x358) = 3;
    *(long *)(actor + 0x360) = *(long *)(reference + 0x20);
    dword const &range = *(dword *)(definition + 4);
    *(long *)(actor + 0x364) = *(long *)(object + 0x248);
    *(dword *)(actor + 0x36c) = range;
    *(short *)(actor + 0x35a) = 0;
    *(long *)(actor + 0x368) = prop_ref_index;
    long owner = *(long *)(actor + 0x364);
    if (owner != NONE)
    {
     byte *unit = *(byte **)(g_4e0300->data + (owner & 0xffff) * 12 + 8);
     if (!function_1df560(*(short *)(actor + 0x24), *(short *)(unit + 0x138)))
      *(short *)(actor + 0x35a) = 1;
    }
    function_25b910(actor_index, prop_ref_index);
    result = true;
   }
  }
 }
done:
 return *result_reference;
}
