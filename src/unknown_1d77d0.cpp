#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "globals.h"
#include "havok_reference.h"
#include "constraint_helpers_314090.h"
#include "joint_descriptions_312920.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

/* builds a Havok constraint for every joint of an object's physics model
   whose two ends land on two different rigid bodies of the component */

/* the joints of a physics model: six kinds, each a tag block */
struct s_physics_constraint_iterator
{
	byte *physics;
	short type;
	short index;
};

struct s_physics_constraint_block
{
	long count;
	byte *elements;
};

struct s_physics_model_shape_key;
struct s_impact_tag_block;
void function_1eb110(s_physics_constraint_iterator *iterator);
void function_1eb160(s_physics_constraint_iterator *iterator);
s_impact_tag_block *physics_model_shape_block_get(byte *physics_model, s_physics_model_shape_key const *key, long *element_size);
bool function_db4c0(long object_index, long key_a, long key_b);

struct s_list_rigid_bodies
{
	long count;
	char lists[256];
	char unlisted;
};

s_list_rigid_bodies *function_181bd0(long object_index, s_list_rigid_bodies *rigid_bodies);

/* the joint frames (0x1d7ef0) */
struct s_component_frame_pair
{
	long unknown00;
	short node_a;
	short node_b;
	transform4x3f frame_a;
	transform4x3f frame_b;
};

struct s_component_node_body_model;

struct s_component_frame_owner
{
	byte unknown00[0x48];
	s_component_node_body_model *physics;
};

struct s_component_frame_map
{
	long unknown00;
	char bodies[256];
	char fallback;
};

void function_1d7ef0(hkTransform *frame_a, s_havok_component *component,
	s_component_frame_owner *owner, s_component_frame_map const *mapping, hkTransform *frame_b,
	hkRigidBody **body_a, hkRigidBody **body_b, s_component_frame_pair const *pair);

/* a joint as the physics model holds it: its frames, then its override group */
struct s_component_joint
{
	s_component_frame_pair frames;
	short override_index;
};

struct s_component_joint_override
{
	short type;
	short index;
	long unknown04;
	real strength;
};

struct s_component_joint_override_group
{
	byte unknown00[8];
	long count;
	s_component_joint_override *entries;
	byte unknown10[8];
};

struct s_component_joint_hinge
{
	byte unknown00[0x78];
	real strength;
	real minimum, maximum;
};

struct s_component_joint_angles
{
	byte unknown00[0x78];
	real twist_minimum, twist_maximum;
	real cone_minimum, cone_maximum;
	real plane_minimum, plane_maximum;
	real strength;
};

struct s_component_joint_kind3
{
	byte unknown00[0x78];
	real unknown78;
};

struct s_component_joint_kind5
{
	byte unknown00[0x78];
	real unknown78;
	real unknown7c;
	real unknown80;
};

struct s_component_joint_physics
{
	byte unknown00[0x30];
	long override_count;
	s_component_joint_override_group *overrides;
	byte unknown38[0xb8 - 0x38];
	long angle_count;
	s_component_joint_angles *angles;
	byte unknownc0[0xe8 - 0xc0];
	long hinge_count;
	s_component_joint_hinge *hinges;
	byte unknownf0[0xf8 - 0xf0];
	long kind3_count;
	s_component_joint_kind3 *kind3;
	long kind5_count;
	s_component_joint_kind5 *kind5;
};

struct s_component_joint_model_link
{
	byte unknown00[0x24];
	long physics_model_index;
};

struct s_physics_model_owner
{
	byte unknown00[0x44];
	s_component_joint_model_link *model;
	s_component_joint_physics *physics;
};

/* what the joints' constructors read */
/* the setters that fill a snapshot from the two bodies and their frames */
struct s_1ce4f0
{
	void function_1ce4f0(hkRigidBody *arg_1, hkRigidBody *arg_2,
		__m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5, __m128 const *arg_6);
};

struct s_1ce2f0
{
	void function_1ce2f0(hkRigidBody *arg_1, hkRigidBody *arg_2,
		__m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
		__m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8);
};

struct s_1ce700
{
	void function_1ce700(hkRigidBody *arg_1, hkRigidBody *arg_2,
		__m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
		__m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8);
};

struct s_1ce8d0
{
	void function_1ce8d0(hkRigidBody *arg_1, hkRigidBody *arg_2, const __m128 *arg_3, const __m128 *arg_4);
};

struct s_1ce970
{
	void function_1ce970(hkRigidBody *arg_1, hkRigidBody *arg_2,
		__m128 const *arg_3, __m128 const *arg_4, __m128 const *arg_5,
		__m128 const *arg_6, __m128 const *arg_7, __m128 const *arg_8);
};

class c_component_joint_allocator
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void *allocate(long size, long kind) = 0;
};

/* the six constraints, reference counted, from Havok's constraint memory */
class c_component_joint_constraint
{
public:
	static void *operator new(size_t size)
	{
		void *block = ((c_component_joint_allocator *)g_480118)->allocate(size, 0x27);
		((c_havok_reference_counted *)block)->allocation_size = (word)size;
		return block;
	}
};

class c_component_joint_3 : public c_component_joint_constraint
{
public:
	c_component_joint_3(c_component_joint_snapshot_3 const *info);
	byte unknown00[0x50];
};

class c_component_joint_4 : public c_component_joint_constraint
{
public:
	c_component_joint_4(c_component_joint_snapshot_4 const *info);
	byte unknown00[0x60];
};

class c_component_joint_5 : public c_component_joint_constraint
{
public:
	c_component_joint_5(c_component_joint_snapshot_5 const *info);
	byte unknown00[0xf0];
};

void function_1d0150(s_havok_component *component, s_havok_contact_entities *contact, short a, short b);
byte *__fastcall function_30c170(hkWorld *world);
extern hkWorld *g_51e9a4;

/* the rigid body a node of the model belongs to */
static __forceinline char rigid_body_list_get(s_list_rigid_bodies const *bodies, long node)
{
	return node == NONE ? bodies->unlisted : bodies->lists[node];
}

/* builds the constraint of one joint between its two rigid bodies */
static __forceinline s_havok_contact_entities *joint_constraint_create(s_havok_component *component, s_physics_model_owner *owner,
	s_list_rigid_bodies *bodies, s_component_joint *joint, s_physics_constraint_iterator *iterator, long type, long index)
{
	s_component_joint_override_group *group = &owner->physics->overrides[joint->override_index];
	hkTransform frame_a, frame_b;
	hkRigidBody *local_560343, *rigid_body_b;
	real strength = 0.0f;
	function_1d7ef0(&frame_a, component, (s_component_frame_owner *)owner,
		(s_component_frame_map const *)bodies, &frame_b, &local_560343, &rigid_body_b, &joint->frames);
	for (long i = 0; i < group->count; i++)
	{
		s_component_joint_override *entry = &group->entries[i];
		if (entry->type == iterator->type && entry->index == iterator->index)
		{
			strength = entry->strength;
			break;
		}
	}
	s_havok_contact_entities *constraint;
	switch (type)
	{
	case 0:
		{
			c_component_joint_snapshot_0 info;
			((s_1ce4f0 *)&info)->function_1ce4f0(local_560343, rigid_body_b,
				&frame_a.m_translation.m_quad, &frame_b.m_translation.m_quad,
				&frame_a.m_rotation.m_col2.m_quad, &frame_b.m_rotation.m_col2.m_quad);
			constraint = (s_havok_contact_entities *)new lane_a_hinge(&info);
		}
		break;
	case 1:
		{
			s_component_joint_hinge *hinge = &owner->physics->hinges[index];
			c_component_joint_snapshot_1 info;
			((s_1ce2f0 *)&info)->function_1ce2f0(local_560343, rigid_body_b,
				&frame_a.m_translation.m_quad, &frame_b.m_translation.m_quad,
				&frame_a.m_rotation.m_col2.m_quad, &frame_b.m_rotation.m_col2.m_quad,
				&frame_a.m_rotation.m_col1.m_quad, &frame_b.m_rotation.m_col1.m_quad);
			info.friction = strength == 0.0f ? hinge->strength : strength;
			real lower = 0.0f - hinge->minimum;
			real upper = 0.0f - hinge->maximum;
			info.angle_min = (lower > upper ? upper : lower) * 0.01745329238474369f;
			lower = 0.0f - hinge->minimum;
			upper = 0.0f - hinge->maximum;
			info.angle_max = (lower > upper ? lower : upper) * 0.01745329238474369f;
			constraint = (s_havok_contact_entities *)new lane_a_limited_hinge(&info);
		}
		break;
	case 2:
		{
			s_component_joint_angles *angles = &owner->physics->angles[index];
			c_component_joint_snapshot_2 info;
			((s_1ce700 *)&info)->function_1ce700(local_560343, rigid_body_b,
				&frame_a.m_translation.m_quad, &frame_b.m_translation.m_quad,
				&frame_a.m_rotation.m_col2.m_quad, &frame_b.m_rotation.m_col2.m_quad,
				&frame_a.m_rotation.m_col1.m_quad, &frame_b.m_rotation.m_col1.m_quad);
			info.twist_low = (angles->twist_minimum > angles->twist_maximum ? angles->twist_maximum : angles->twist_minimum) * 0.01745329238474369f;
			info.twist_high = (angles->twist_minimum > angles->twist_maximum ? angles->twist_minimum : angles->twist_maximum) * 0.01745329238474369f;
			info.plane_low = (real)sin((angles->plane_minimum > angles->plane_maximum ? angles->plane_maximum : angles->plane_minimum) * 0.01745329238474369f);
			info.plane_high = (real)sin((angles->plane_minimum > angles->plane_maximum ? angles->plane_minimum : angles->plane_maximum) * 0.01745329238474369f);
			info.cone_limit = (real)cos((angles->cone_minimum > angles->cone_maximum ? angles->cone_minimum : angles->cone_maximum) * 0.01745329238474369f);
			info.friction = strength != 0.0f ? strength : angles->strength;
			constraint = (s_havok_contact_entities *)new lane_a_ragdoll(&info);
		}
		break;
	case 3:
		{
			s_component_joint_kind3 *joint3 = &owner->physics->kind3[index];
			c_component_joint_snapshot_3 info;
			((s_1ce8d0 *)&info)->function_1ce8d0(local_560343, rigid_body_b,
				&frame_a.m_translation.m_quad, &frame_b.m_translation.m_quad);
			info.length = joint3->unknown78;
			constraint = (s_havok_contact_entities *)new c_component_joint_3(&info);
		}
		break;
	case 4:
		{
			c_component_joint_snapshot_4 info;
			info.body_a = local_560343;
			info.body_b = rigid_body_b;
			info.pivot_a = frame_a.m_translation.m_quad;
			info.pivot_b = frame_b.m_translation.m_quad;
			constraint = (s_havok_contact_entities *)new c_component_joint_4(&info);
		}
		break;
	case 5:
		{
			s_component_joint_kind5 *joint5 = &owner->physics->kind5[index];
			c_component_joint_snapshot_5 info;
			((s_1ce970 *)&info)->function_1ce970(local_560343, rigid_body_b,
				&frame_a.m_translation.m_quad, &frame_b.m_translation.m_quad,
				&frame_a.m_rotation.m_col2.m_quad, &frame_b.m_rotation.m_col2.m_quad,
				&frame_a.m_rotation.m_col1.m_quad, &frame_b.m_rotation.m_col1.m_quad);
			info.maximum = joint5->unknown7c;
			info.minimum = joint5->unknown78;
			info.friction = joint5->unknown80;
			constraint = (s_havok_contact_entities *)new c_component_joint_5(&info);
		}
		break;
	default:
		__assume(0);
	}
	return constraint;
}

// score 22 (was 22): the entry matches retail (the component's object index and the physics
// model index are read into named locals before their uses); what still differs is the final
// `|= 0x400`: retail keeps it whole as `or dword ptr [ecx+4],0x400` in a block that falls into
// the function's exit (the zero-size `jle` target), and gives the loop's normal exit the copied
// epilogue; ours places the found block after the exit, so its copied epilogue splits the
// update into load/or/store around the pops (11 bytes against 6) and the body is 4 bytes longer
// than retail's 1820. An earlier score 25 (was 32) with the same entry locals and only that
// update differing was reverted in review.
// Tried: the tail as a separate static or plain inline helper (not inlined) 193 each; do-while
// inside `if (size > 0)` 80; found handling after the loop on `i < size` 58; the loop reusing
// the entry's object index 465; owner-first builder 31; joint-first 31; key-first 31; map-first
// 31; first-body name 31; body outputs first 31; reversed frames 31; shared fresh object index
// 31; returned map 431; block reference 31; size lifetime 31; flags reference 34; paired bits
// 31; contact entry 174; size name 31; model pointer 36 (frame 0x454 agrees). Harness:
// forced-inline search helpers (bool + out index, index or NONE, whole tail), explicit early
// return, `!=` + continue, a store after the loop: all keep the found block last. This round:
// `break` for the found return 22; the found index kept and handled after the loop 51; `!=` +
// continue in a real build 22; the loop as a plain while 22; an explicit final return 22.
// Lane 10 r15 (real builds): the search as a forceinline index-or-NONE helper 63 (component in
// EBX, the NONE test kept); the whole tail as a forceinline helper 22; a contact element pointer
// 23; the index declared at function scope 22; a bool set at the found break and tested after the
// loop 22 (the same layout: the size guard still joins the loop's normal exit).
// This round: retail's tail has two copies of the epilogue, and its zero-size guard jumps to
// the one the found path falls into, so that block has two predecessors and the flag update
// stays whole; ours jumps to the loop's own copy, so the found path's block has one
// predecessor and the update sinks into it. `break`, `goto done`, the update after the loop
// behind a flag, the bit-field set, the `|` and `+` forms and the update dropped all compile
// to the same bytes (22); an explicit `if (size > 0)` adds a second guard (1828 bytes, 86).
// This round: an early `if (size <= 0) return;` before the loop adds its own copy of the
// epilogue (1828 bytes, 86); `break` in the found path is canonicalised into the same bytes (22).
// @retail 0x1d77d0
void __stdcall function_1d77d0(s_havok_component *component, s_physics_model_owner *owner)
{
	long component_object_index = component->object_index;
	s_list_rigid_bodies bodies;
	function_181bd0(component_object_index, &bodies);
	s_physics_constraint_iterator iterator;
	long physics_model_index = owner->model->physics_model_index;
	iterator.physics = g_4e3b44[physics_model_index & 0xffff].bytes;
	for (function_1eb110(&iterator); ; function_1eb160(&iterator))
	{
		long element_size;
		s_physics_constraint_block *block = (s_physics_constraint_block *)physics_model_shape_block_get(iterator.physics,
			(s_physics_model_shape_key const *)&iterator.type, &element_size);
		long index = iterator.index;
		s_component_joint *joint = index < block->count ? (s_component_joint *)(block->elements + index * element_size) : NULL;
		if (!joint)
			break;
		long object_index = component->object_index;
		long type = iterator.type;
		if (!function_db4c0(object_index, type, index) && joint->frames.node_a != NONE)
		{
			long body_a = rigid_body_list_get(&bodies, joint->frames.node_a);
			long body_b = rigid_body_list_get(&bodies, joint->frames.node_b);
			if (body_a != body_b && body_a != NONE &&
				!component->rigid_bodies.data[body_a].rigid_body->m_fixed &&
				(body_b == NONE) == (joint->frames.node_b == NONE))
			{
				s_havok_contact_entities *constraint = joint_constraint_create(component, owner, &bodies, joint, &iterator, type, index);
				function_1d0150(component, constraint, (short)type, (short)index);
			}
		}
	}
	component->unknown04 &= ~0x400;
	for (long i = 0; i < component->unknown7c.size; i++)
	{
		hkEntity *entity = component->unknown7c.data[i].contact->entity_b;
		if (entity == (hkEntity *)function_30c170(g_51e9a4))
		{
			if (component->unknown18 != NONE &&
				(hkEntity *)component->rigid_bodies.data[component->unknown18].rigid_body == component->unknown7c.data[i].contact->entity_a)
			{
				component->unknown18 = NONE;
				component->unknown19 = NONE;
			}
			component->unknown04 |= 0x400;
			return;
		}
	}
}
