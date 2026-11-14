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
// score 412 (was 412): the frame is 0xa4 against retail's 0xa0; retail takes the actor
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
				view = *(long *)(p + 0x14) == NONE ? NULL : (byte *)&tracking_get(*(long *)(p + 0x14))->view;
				point3f position;
				function_b9dd0(*(long *)(p + 0x20), &position);
				if (view && *(short *)(view + 0xa0))
				{
					byte *state = (byte *)function_25d690(node);
					real y = *(real *)(state + 8) - position.y;
					real x = *(real *)(state + 4) - position.x;
					if (y * y + x * x > 1.0f) break;
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
	long current = *(long *)(actor + 0x338);
	if (current != NONE)
	{
#pragma inline_depth(0)
		byte *view = (byte *)function_25d700(current);
#pragma inline_depth()
		if (view && *(short *)view >= 5) actor[0x34c] = true;
#pragma inline_depth(0)
		view = (byte *)function_25d700(current);
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
short __stdcall function_1c8df0(short source_cell, short target_cell, point3f const *target,
	long object_index, point3f const *source, long mode, bool narrow, bool attached, bool alternate,
	long *hit_object);
short function_25c9d0(long actor_index, long prop_ref_index, bool player, short sight,
	void *point, byte *sample, long reported);
short function_25cb60(long actor_index, long object_index, short type, void *sample,
	void *point, long location, short sight);
short function_25cca0(long prop_ref_index);
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
real function_296600(long actor_index, long prop_ref_index);

// MiniMax G (11-10): no new form, no lower score (91).
// MiniMax I (11-10): no new form, no lower score (163).
// MiniMax B (11-04): no new form, no lower score.
// MiniMax B (11-12): no new form, no lower score (90): the reduced image scores this function at 225, so the fast number
// is not the full build's 90; at the head the actor index stays in EDI where retail keeps it in EBX, and the pool global
// is loaded before the frame setup instead of after it.
// MiniMax A (11-13): no new form, no lower score (90; the reduced image still gives 225): the object pool global and
// the game time swap their places at the head, the suppressed and current byte homes stay swapped, and the first
// 0x1c8df0 call keeps the root in a register where retail stores it into the squad's slot and evaluates !current
// among the pushes.
// score 90 (was 90): retail's length, frame 0x20. Left (masked aligned count 20, registers and esp offsets masked): at the head ours
// loads the object pool global before the game time and the object index, retail after them (so the object and table registers swap:
// ours EAX/ECX, retail ECX/EAX, through the type-15 arm), the suppressed/current byte homes swapped, and the first 0x1c8df0 call, where retail
// stores the root into the squad's slot at once, keeps current in AL and evaluates `!current` with `sete` among the pushes (CSE'd into the
// second call through a stack slot), while ours computes the named not_current before the hit pointer and keeps the root in EAX.
// The two stacked notes below (first at 163, then at 90) are merged into this one.
// Earlier at 163 (was 257): retail's length, frame 0x20, actor EBX. Left: the object and table registers (ours EAX/ECX, retail ECX/EAX, time read after the table pointer), the
// suppression/current byte homes swapped, root and not_current in each other's slots with `!current` evaluated at the named local where retail evaluates it among the first
// call's arguments, and BLOCKED-BY 0x25cb60: retail pushes the short sight as ESI, a short last parameter; a contributor's signature takes a long, so ours adds `movsx`.
// Earlier (257, was 257): frame 0x20 and actor EBX agree; squad/object, flag and root/hit homes differ.
// Earlier: actor EBX, prop ESI, squad home, the type-15 arm and the 0x25c9d0
// call are retail's; left: the object and table registers (ours EAX/ECX, retail ECX/EAX) and time
// scheduling before the branch, the suppression/current byte homes swapped ([esp+0x12]/[esp+0x13]),
// and at the first 0x1c8df0 call retail spills the root into the squad's slot and keeps the hit
// pointer in EDX where ours keeps the root in EAX and spills the hit pointer (so the root and the
// priority swap homes and everything after shifts one instruction); 0x25cb60's sight is pushed after a
// movsx (retail: as is), BLOCKED-BY 0x25cb60 (its sight parameter width). 1276 bytes against 1268.
// Earlier (331): the tracking view is cleared before the tracking test, so the
// pointer is zeroed where retail zeroes it instead of being assigned in an else
// branch. Retail holds the actor in EBX and spills it at [esp+8] right after the
// actor[9] test where ours holds it in ESI and spills it at [esp+0x14]; retail keeps
// the squad pointer at [esp+0x10] where ours uses [esp+0xc]. The body is 1251 bytes
// against retail's 1268.
// Earlier (319): retail holds the actor in EBX (spilled at [esp+8]) and the prop in ESI where
// ours swaps them, keeps the squad pointer at [esp+0x10] where ours uses [esp+0xc], acquires the
// object, suppression, current flag and unit pointer before the actor-type branch, and after the
// 0x1c8df0 call keeps a zero in EBX and the sight in ESI. The body is 1253 bytes against 1268.
// The tracking view is cleared before the tracking test, so the pointer is zeroed where retail
// zeroes it instead of being assigned in an else branch.
// Equal-zero reference, equal-four sight, view zero, priority zero, paired defaults, all defaults chains
// 363/363/363/363/363/363; alternate-view short pair memmove 374; copy field order 363 (control 363).
// Original stores, body and local pointer qualifier retained; full baseline 358.
// actor/view registers and pointer/flag homes; ordinary
// path first 373; maximum result 372 (plain 370); wide other priorities 376;
// visible-view arm first 376; tracking-handle snapshot 373.
// ordinary current scope 375; suppression scope 366 (plain 373); object/unit
// scope 371; all ordinary captures scoped 363 (plain 358); active single exit 373.
// Prop pointer scoped to ordinary arm 369; prop/squad scopes 371; Boolean
// mode snapshots 378; branch-assigned two-priority maximum 363; single final
// priority store 367 (fast control 363, full baseline 358); all restored.
// saved actor in ordinary arm 371 (plain 367); native actor fields 363;
// both 371; timestamp name 363; inline depth one 363 (preview 363).
// object/current/suppression alone at entry 367/368/368; group/prop/reference
// pointer names 363; shared squad getter 363; entry pointer declarations 363
// (fast control 363, full baseline 358); all restored.
// Pairwise object/current, object/suppression, suppression/current entry captures
// 366/375/371; false-default suppression on those pairs 366/375/371;
// optional unit pointer on first two pairs 370/373 (fast control 363).
// Object/current pair plain 373 versus 358; actor ESI and saved-pointer/flag homes remain.
// frame 0x20 agrees; actor ESI versus EBX and saved-actor/flag homes remain.
// Native prop/datum/sample/motion references 363f/363f/363f/363f; direct alternate
// sight result 363f; child/guarded-parent handles 363f/363f; adjacent priority memmove
// 377f (preview 363f); all original bodies and call arguments restored.
// Ordinary object-type/suppression/current orders 012/021/102/120/201/210
// 366/366/351/355/369/388; coordinated priority names 341; names with 210 order 388
// (fast control 341; full baseline 358).
// Round 203: native squad flag with squad-index, tracking-index and masked actor-slot producer
// combinations 372 each in all eight fast trials (control 372/1260, full 331/1251).
// Actor ESI and saved/squad homes remain; all native views, handles and temporary include restored.
// Round 358: Complete different-target bool/byte/short/long carriers 376/341/376/375; before
// suppression 376/341/364/366 (preview 341). Exact-length 364 regresses; original current predicate
// and body restored.
// Round 205: Special copied-result byte/char/short/long 372 each; with complete ancestry
// producer scope 372 each (fast control 372, full 331). Original body restored.
// Round 237: shared initial view lookup 372; actor/reference/prop at uses 414/503/415;
// actor-reference/actor-prop/reference-prop/all 573/468/570/657 (fast control 372; full 331).
// All restored; actor ESI and saved/squad/flag homes remain.
// Round 245: Signed-short three-input priority trees in six candidate orders
// 352/352/350/351/353/353; two reversed comparison trees 352/351
// (fast control 356, full 331). Sub-eight gains rejected; original body restored.
// Round 174 (fast control 372/1262, full 331/1251): existing saved-pointer
// actor_snapshot/retained_actor/actor_saved/saved_owner identities, each with
// original/actor_squad receiver identity, 372 each. All restored; full frame 0x20
// agrees, actor ESI/EBX and saved/squad/flag homes remain.
// Round 231: positive current/unit bool-byte-short pairs 372/372/372/372/372/365/365/365;
// short-current priority tree/accumulator/full-width priorities 366/364/368 (fast control 372).
// Accumulator ordinary 338 versus 331, rejected; original predicates, priorities and body restored.
// Round 389: suppression byte/short/long 372/363/366; unsigned timestamp 372; native
// wide timestamp alone/with byte/short/long suppression 377/377/377/376 (fast control
// 372). Short suppression plain 344 versus 331; all original types and body restored.
// DL167 (fast control 356, full 331): repeated flag25/flag23/parent observations,
// entry masks 1..7 348/349/367/353/354/369/359; all byte 367. First-mode flag25,
// flag25+parent, flag23, both flags and byte flag25 350/355/351/337/357;
// both flags+parent 333; byte pair/+parent 358/359. Shared mode 371/366/368;
// shared object getters ordinary/ancestry/all 333 each. Full flag25/pair/pair+parent
// 354/350/351 versus 331; all restored. Actor ESI/EBX and saved/squad homes remain.
// Round 397 (fast control 356, full 331): complete tracking-root view access; root branches, ordinary saved actor and readonly source-view masks 0..7
// 355/360/378/376/355/360/378/376; all restored.
// Round 240: Complete pre-branch object/suppression/current acquisition in six
// producer orders 360/360/376/394/391/387; first two with the saved-pointer declaration
// before actor construction 360/360 (fast control 356, full 331).
// Original observations, qualified local pointer and complete body restored.
// Round 250: paired ancestry category outputs in char/byte/short/long, category-first
// 375/375/381/377, priority-first 375/375/381/377 (fast control 372, full 331).
// All exact-only field-result forms restored; actor ESI/EBX and saved/squad homes remain.
// Round 90 (full LTCG relink screens; --fast misbuilds this function): the mode argument written in
// both 0x1c8df0 calls 319 (plain, kept: retail evaluates the hit pointer, then the mode). On it: the
// sight parameter of 0x25c9d0 short 350 (sight then in ESI as retail, prop takes EBX); the type-15 arm
// as an if/else on a found index (other_index NONE unless 0x25d810 finds one; view handled in both
// arms) 334 at 1268 bytes, that arm then identical to retail; with MAX(priority, MAX(object, extra))
// 350; with the object/suppression/current/unit acquired before the actor-type branch 347; without the
// volatile saved actor 332 (actor EBX, prop ESI as retail; aligned 128 against 248); with `unit = object;
// if (type) unit = NULL;` 330 (plain 338 at 1270); that plus the short sight 342 (zero in EBX, sight in
// ESI as retail; left: squad kept in EDX not at [esp+0x10], the stack zero for retail's cmovne, root/hit
// pointer slots at the 0x1c8df0 call). Those restructured forms align far better and are the base to
// continue from; renaming current/suppressed 0 change; typed prop record 358 (control 322).
// Round 92 (plain): round 90's restructured body (no volatile saved actor, object/suppression/current/unit
// before the actor-type branch, if/else on the found index, MAX twice) with the unit NULL-first and set
// when the object type is 0: 338 at 1268 bytes (cmov, register zero); renaming current to sort after
// suppressed 338 and declaring it first 338 (byte homes unchanged); with 0x25c9d0's sight short 294;
// the unit's NULL defined before the suppression test 269 (squad spilled, the cmovne from the unit's
// NULL home as retail); the existing prop record type, its +0x25 flag passed directly 265, kept;
// on it, the game time read after the object lookup 268.
// Round 114 (lane 5, plain checks; --fast misbuilds it): the object lookup ahead of the prop lookup and
// the time read (the order matched 0x264330 uses) 242 at 1277 bytes, not kept: an aligned diff of the
// listing is unchanged (96), the head folds the prop table load (`add esi, [edx+0x44]`, retail loads it
// into EAX) and the one-instruction shift realigns the tail; object/prop/unit/time 242 (aligned 96);
// object/time/prop 239 (aligned 98); prop/object/time 270 (aligned 99); object-first with current declared
// before suppressed 361 (1268 bytes, actor then in EBP); object-first with the time read right after the
// reference lookup 262, with current computed there 303 (aligned 105 each); root and sight declared at the
// top of the else block 267; root declared at the function top 267 (both identical code). Left: retail's
// EAX/ECX roles for the object and its table, the current/suppressed byte homes, the root kept in EAX at
// the first 0x1c8df0 call (retail stores it into the squad's slot and loads current into AL, so its hit
// pointer stays in EDX where ours spills it), the root/priority homes after it, and BLOCKED-BY 0x25cb60
// (contributor) for the movsx of the sight.
// Round 38 (lane 15, plain checks; aligned = LCS-aligned differing instructions, 105 at 267): current
// computed before suppressed 372; the time read before the prop lookup 271; the unit as
// `object[0xaa] == 0 ? object : NULL` 257 (kept: plain 1256 bytes; the code is now branchless
// setne/dec/and where retail selects with cmovne from the NULL-first home, aligned 117, so the
// NULL-first form remains the closer structure once the slot swaps are solved). On it: the ternary
// inverted 257; time before prop 258; current before suppressed 366; the unit selected right after the
// object lookup 289; `prop_ref_index ==` operand order 257. Byte homes: ours puts suppressed (3 byte
// refs plus its address) at [esp+0x12] and current (4 refs) at [esp+0x13], retail the reverse.
// Round 254: Squad acquired at suppression producer with readonly ownership, existing object
// getter and null-first unit combinations 380/397/380/397/380/397/380/397 (fast control 372; full
// baseline 257). Ordinary late-squad and late-squad/null-first forms 359/364; both regress.
// Original source and include restored; squad/object, flag and root/hit homes remain.
// Round 126 (lane 8, plain): unit NULL-first (`byte *unit = NULL;` before the suppression test, set when the
// object type is 0) 267 at 1278 bytes: head as retail (squad spilled, NULL home, cmovne) but the current-record
// argument of the first 0x1c8df0 call spills; on it: current renamed to sort after suppressed 267 (byte homes
// unchanged); a named current-record pointer for both calls before/after the root lookup 272/270; a named
// sight mode for both calls 326.
// J247: 25c9d0/25cca0/263810/268c60 inline contracts, original/null-first unit pairs:
// 257/267/257/267/257/267/257/267; frame 0x20, 1256/1278 bytes (retail 1268).
// NOT kept: no gain; body/contracts restored. Squad, flag and root/hit homes remain.
// DH334: Standard inner/outer/both short maxima, with/without null-first unit (fast 372, full 257);
// then full maximum and secondary-result stages: 350/365/366/368/346/346/365/355.
// NOT kept: new maximum call and larger frame absent in retail; original complete body restored.
// Round 87 (m3 lane 16, plain; --fast misbuilds this function, aligned counts in brackets, 257 is [112]): unit NULL-first 267 at 1278 bytes [96], re-measured; on it a named
// `not_current` for both 0x1c8df0 calls at the top 308 at 1270 [76], alone 262 at 1248 [91]; named after the root lookup 163 at 1268 [74], kept. On the kept form: time read
// after the object 163, before the reference 163; a named object table for the three lookups 163; an object index local (before the kept form) 267, the object through
// an array of pointers 267, current declared before suppressed 267; a named record pointer before not_current 159 (one use, not kept); a named mode 258 at 1272 [85]; not_current
// declared before root 163, at function scope 163; root declared bare 163; suppressed renamed to sort before current 163 (byte homes unchanged).
// Round 115 (fast control 372, full 257): scoped early squad 372; suppressed/unit/current 370;
// current/unit/suppressed 390; unit/current/suppressed 381; suppression array 372; array with NULL-first unit 379;
// array with object-first/null-override unit 352 (plain 295, 1255 bytes); scalar object-first/null-override 352.
// NOT kept: scope/array forms have no effect, orders regress or gain too little, and object-first uses a branch
// absent in retail and regresses plain. Original declarations and ternary restored; array forms were exact-only.
// Round 139 (m3 lane 7, plain with the file's other functions; --fast misbuilds this one): the two 0x1c8df0 calls through one
// file-local sight helper (actor, sample, target cell, target, root, prop, current) that writes `!current` and the hit pointer among
// its arguments: plain `inline` 434 (1133 bytes, it stays out of line), forced inline 208 (1273 bytes); `!current` written in both
// calls with no named local, re-measured, 267 (1278 bytes); the three object lookups through the existing ai_object_get 163 (same
// code); current as a byte 163 (same code); the time read ahead of the prop lookup 168 (1269 bytes). Retail computes the hit
// pointer and the mode ahead of the pushes and then `sete` for `!current` between them, storing it for the second call.
// At 90 (was 163; PR #1006, landed in batch r746;
// the same change as lane 12 r107's held patch): 0x25cb60's last two parameters as (long location, short sight), the 0x263ed0 call's `(short)` cast dropped.
// Lane 12 r107 (plain; masked aligned count with registers and esp offsets masked in brackets, 30 at 163): 0x25cb60's last parameter as `short` (contributor code: its definition line and its cast to 0x263ed0, plus the
// declaration here; the held patch is listed in the round 107 report) 90 [27] (1268 B; 0x25cb60 stays 12, the movsx goes). What is
// left on that base: the game time read ahead of the object index and the object pool load (ours loads the pool global first),
// and `!current` (retail's `sete` sits between the first call's pushes, stored for the second call). On the held base: `!current`
// written in both calls 263 [24] (1275 B; the hit pointer spills, current takes CL where retail's root is stored at once and
// current takes AL); the time read after the object 93 [32]; before the prop 92 [34]; current before suppressed 373 [82]
// (1274 B); not_current declared before root 258 [25] (1269 B).
// Round 114 (m3 lane 9, plain; --fast misbuilds it unless 0x1c8df0's file is in the reduced image; masked counts in brackets, base [20]):
// `!current` written in both calls re-measured 263 [23] (the root stays in EAX, so current takes CL, the mode EDX and the hit pointer
// spills); on it both hit pointers as `!current ? NULL : (long *)(actor + 0x348)` 264, current computed before suppressed 371 (1270 B);
// on the base both hit pointers as `not_current ? NULL : ...` 95 [28]; the root's argument as an object_index local set by an if (the
// parent unit's index first) 260 [21], with `!current` in both calls 270 [26] (1277 B).
// The actor through the existing actor_get 90 (same code, kept: it replaces the written-out lookup).
// MiniMax C (11-14): no new form, no lower score (91 in the reduced image, the queue's 90); the actor
// pointer still lives in edi where retail keeps it in ebx.
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
	long time = g_510c54->game_time;
	byte *object = *(byte **)(g_4e0300->data + (reference->object_index & 0xffff) * 12 + 8);
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
			long other_actor = *(long *)(other + 0x12c);
			if (other_actor != NONE)
				other_index = function_25d810(reference->object_index, other_actor, false);
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
		long root = function_baf40(unit && *(long *)(unit + 0x14) != NONE ? *(long *)(unit + 0x14) : reference->object_index);
		bool not_current = !current;
		short sight = function_1c8df0(*(short *)((byte *)sample + 0x28), *(short *)((byte *)motion + 0x28),
			&motion->field_0, root, (point3f const *)sample, prop->unknown25 && prop->unknown23 ? 2 : 0, false, *(long *)(actor + 0x26c) != NONE, not_current,
			current ? (long *)(actor + 0x348) : NULL);
		if (*(long *)(actor + 0x7c) != NONE && prop->unknown25 && 25.0f > reference->unknown28 &&
			(sight == 0 || sight == 1))
			function_268c60(*(long *)(actor + 0x7c));
		if (view)
			view->unknown06 = (short)sight;
		if (!prop->unknown26[0] && !suppressed)
		{
			short priority = (short)function_25c9d0(actor_index, prop_ref_index, prop->unknown25, sight,
				sample, (byte *)motion, (long)&suppressed);
			if (reference->state >= 3 && view)
			{
				s_type_5cfb45 *state = function_25d690(reference);
				if (!view->unknown88)
				{
					short other_sight = function_1c8df0(*(short *)((byte *)sample + 0x28),
						*(short *)((byte *)state + 0x2c), (point3f const *)((byte *)state + 0x30),
						root, (point3f const *)sample, prop->unknown25 && prop->unknown23 ? 2 : 0, false, *(long *)(actor + 0x26c) != NONE, not_current,
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
