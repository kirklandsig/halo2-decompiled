// @flags /O2 /Ob1 /GL-
#include "joint_descriptions_312920.h"
#include "lane_b_constraint_leaves.h"

// Mutable registration records are initially zero; neighbouring records are
// 0x28 bytes apart. These accessors do not inspect their internal fields.
struct joint_registration_record_3128f0 { unsigned char opaque[0x28]; };
joint_registration_record_3128f0 g_480658;
joint_registration_record_3128f0 g_480680;
joint_registration_record_3128f0 g_4806a8;

// @retail 0x3128f0
void *lane_a_ragdoll::v3() { return &g_480658; }
// @retail 0x313310
void *lane_a_limited_hinge::v3() { return &g_480680; }
// @retail 0x313b20
void *lane_a_hinge::v3() { return &g_4806a8; }

// @retail 0x312900
void __cdecl free_constraint_312900(void *object)
{
	g_memory_2d88a0->release_block(object, ((lane_a_constraint *)object)->flags, 0x27);
}

// @retail 0x312ab0
void lane_a_ragdoll::export_description(lane_a_ragdoll_description *description) const
{
	export_base(description);
	description->pivot_a = pivot_a;
	description->pivot_b = pivot_b;
	description->twist_b = plane_a;
	description->plane_b = plane_b;
	description->twist_a = twist_a;
	description->plane_a = twist_b;
	description->cone_limit = cone_limit;
	description->plane_low = plane_low;
	description->plane_high = plane_high;
	description->twist_low = twist_low;
	description->twist_high = twist_high;
	description->friction = friction;
}

// @retail 0x3134a0
void lane_a_limited_hinge::export_description(lane_a_limited_hinge_description *description) const
{
	export_base(description);
	description->pivot_a = pivot_a;
	description->pivot_b = pivot_b;
	description->axle_a = axle_a;
	description->axle_b = axle_b;
	description->perp_a = perp_a;
	description->perp_b = perp_b;
	description->reference_b = reference_b;
	description->angle_min = angle_min;
	description->angle_max = angle_max;
	description->friction = friction;
}

// @retail 0x313c50
void lane_a_hinge::export_description(lane_a_hinge_description *description) const
{
	export_base(description);
	description->pivot_a = pivot_a;
	description->pivot_b = pivot_b;
	description->axle_a = axle_a;
	description->perpendicular_a = perpendicular_a;
	description->axle_b = axle_b;
}

// @retail 0x3128d0
void __cdecl export_ragdoll_3128d0(lane_a_ragdoll const *object, lane_a_ragdoll_description *description)
{
	object->export_description(description);
}
// @retail 0x3132f0
void __cdecl export_limited_hinge_3132f0(lane_a_limited_hinge const *object, lane_a_limited_hinge_description *description)
{
	object->export_description(description);
}
// @retail 0x313b00
void __cdecl export_hinge_313b00(lane_a_hinge const *object, lane_a_hinge_description *description)
{
	object->export_description(description);
}

// Refresh the owner's solver storage when friction changes its row count.
// @retail 0x312b80
void lane_a_ragdoll::set_strength(float strength)
{
	if (owner)
	{
		lane_a_constraint_info sizes;
		info(&sizes);
		((lane_b_constraint_owner *)owner)->remove((lane_b_contact_constraint_prefix *)this,
			(lane_b_constraint_sizes const *)&sizes);
		friction = strength;
		info(&sizes);
		((lane_b_constraint_owner *)owner)->add((lane_b_contact_constraint_prefix *)this,
			(lane_b_constraint_sizes const *)&sizes);
	}
	else
		friction = strength;
}
