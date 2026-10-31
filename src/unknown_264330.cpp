#include "unknown_11c920.h"
#include <stdlib.h>
#include "globals.h"
#include "unknown_1fb7e0.h"
#include "unknown_25d020.h"
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

// score 420 (was 420): the frame is 0xa4 against retail's 0xa0; retail takes the actor
// index in EAX where ours loads the argument from the stack. The body is 1624 bytes
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
		short old_state = *(short *)(p + 0x24);
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
		case 2:
			if (!function_25da40(node)) goto release_tracking;
			else if ((char)p[0x27] > 0) function_25c230(actor_index, index, NONE);
			else
			{
				view = *(long *)(p + 0x14) == NONE ? NULL : g_502414->data + (*(long *)(p + 0x14) & 0xffff) * 0x124 + 0x70;
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
		}
		goto tracking_complete;
release_tracking:
		function_1e2150(actor_index, index);
		function_25c820(index, actor_index);
tracking_complete:
		if (old_state != *(short *)(p + 0x24))
		{
			view = *(long *)(p + 0x14) == NONE ? NULL : g_502414->data + (*(long *)(p + 0x14) & 0xffff) * 0x124 + 0x70;
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

PRIVATE inline void tracking_state_initialize(s_type_5cfb45 *state)
{
	state->unknown40 = NONE;
	state->unknown00 = NONE;
	state->unknown3c = NONE;
	state->unknown66 = false;
	state->unknown62 = false;
	state->unknown5e = false;
	state->unknown64 = false;
	state->unknown61 = false;
	state->unknown5f = false;
	state->unknown60 = true;
	state->unknown63 = false;
	state->unknown69 = false;
	state->unknown65 = false;
	state->unknown67 = false;
	state->unknown48 = *g_468788;
	state->unknown54 = NONE;
	state->unknown44 = NONE;
	state->unknown58 = false;
}

// @retail 0x263210
void __stdcall function_263210(long actor_index)
{
	long next_index = actor_prop_view_get(actor_index)->first_prop_index;
	short count = 0;
	short active_count = 0;
	short current = 0;
	bool reclaimed = false;
	s_tracking_candidate candidates[50];
	s_2640c0 motion;
	s_2641c0 sample;
	s_actor_prop_view *initial_actor = actor_prop_view_get(actor_index);
	s_actor_prop_view *actor = initial_actor;
	++g_4de2fc;
	g_4de2f8 = true;
	long *tracked = initial_actor->tracked_prop_indices;
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
	while (next_index != NONE)
	{
		long index = next_index;
		s_prop_datum *reference = prop_ref_get(index);
		next_index = reference->next_index;
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
					candidates[count].index = index;
					candidates[count].priority = priority;
					count++;
				}
			}
		}
	}
	g_4de2f8 = false;
	if (count > 8)
		qsort(candidates, count, sizeof(s_tracking_candidate), function_2631f0);
	s_record_pool *tracking_pool = g_502414;
	while (current < count && active_count < 8)
	{
		long index = candidates[current].index;
		s_prop_datum *reference = prop_ref_get(index);
		if (reference->tracking_index == NONE)
		{
			reference->tracking_index = record_pool_allocate(tracking_pool);
			if (reference->tracking_index == NONE && !reclaimed)
			{
				if (function_25db60())
					reference->tracking_index = record_pool_allocate(tracking_pool);
				reclaimed = true;
			}
			if (reference->tracking_index != NONE)
			{
				s_type_e5ff81 *tracking = (s_type_e5ff81 *)(tracking_pool->data + (reference->tracking_index & 0xffff) * sizeof(s_type_e5ff81));
				tracking_state_initialize(&tracking->state);
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
					tracking_pool = g_502414;
				}
			}
		}
		if (reference->tracking_index != NONE)
			actor->tracked_prop_indices[active_count++] = index;
		current++;
	}
	if (active_count < 8)
		memset(&actor->tracked_prop_indices[active_count], 0xff, (word)(8 - active_count) * sizeof(long));
	for (; current < count; current++)
	{
		long index = candidates[current].index;
		s_prop_datum *reference = prop_ref_get(index);
		if (reference->tracking_index != NONE)
		{
			if (reference->tracking_index != NONE)
				function_25c780(actor_index, index);
			reference->state = 0;
			reference->unknown10 = 0.0f;
		}
	}
}

long function_baf40(long object_index);
short __stdcall function_1c8df0(short source_cell, short target_cell, point3f const *target,
	long object_index, void const *source, long mode, bool narrow, bool attached, bool alternate,
	long *hit_object);
short function_25c9d0(long actor_index, long prop_ref_index, bool player, short sight,
	void *point, byte *sample, long reported);
short function_25cb60(long actor_index, long object_index, short type, void *sample,
	void *point, long location, long sight);
short function_25cca0(long prop_ref_index);
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
real function_296600(long actor_index, long prop_ref_index);

// score 267 (was 265): actor EBX, prop ESI, squad and unit homes, the type-15 arm and the 0x25c9d0
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
// @retail 0x264b50
void __stdcall function_264b50(long actor_index, long prop_ref_index, s_2641c0 *sample, s_2640c0 *motion)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
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
		short sight = function_1c8df0(*(short *)((byte *)sample + 0x28), *(short *)((byte *)motion + 0x28),
			&motion->field_0, root, sample, prop->unknown25 && prop->unknown23 ? 2 : 0, false, *(long *)(actor + 0x26c) != NONE, !current,
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
						root, sample, prop->unknown25 && prop->unknown23 ? 2 : 0, false, *(long *)(actor + 0x26c) != NONE, !current,
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
