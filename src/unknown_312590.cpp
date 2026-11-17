// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object built from a description;
// comparing two transforms within a tolerance; and an object placed by a
// transform filling a description, and moved to a new transform (its bounds
// brought up to date).
#include "placed_shape_312800.h"
#include "reference_holder_30b3a0.h"
#include "unknown_1cec30.h"

// every bit but the sign bit, in four lanes
__declspec(align(16)) static const unsigned long absolute_mask_455940[4] = {
	0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff
};

// @retail 0x312590
class_30b3a0::class_30b3a0(description_312430 const *description)
	: field_8(0), collidable((referenced_2d88a0 *)description->field_4, (signed char)description->field_8)
{
	collidable.owner = this;
	if (collidable.body.shape)
	{
		collidable.body.shape->reference_count++;
	}
	((class_312430 *)this)->function_3124c0(description);
}

// @retail 0x312630
bool_result_2da7f0 transform_312630::equal_within(transform_312630 const &other, float tolerance) const
{
	return bool_result_2da7f0(rotation.equal_within(other.rotation, tolerance).value &&
		(_mm_movemask_ps(_mm_cmplt_ps(_mm_set1_ps(tolerance), _mm_and_ps(_mm_sub_ps(translation, other.translation),
		*(__m128 const *)absolute_mask_455940))) & 7) == 0);
}

// @retail 0x3126b0
void placed_shape_312800::function_3126b0(description_315ba0 *description) const
{
	((class_312430 const *)this)->function_312430(description);
	description->transform.rotation.column[0] = transform.rotation.column[0];
	description->transform.rotation.column[1] = transform.rotation.column[1];
	description->transform.rotation.column[2] = transform.rotation.column[2];
	description->transform.translation = transform.translation;
}

// the game reaches this through its own view of the object
// (include/unknown_1cec30.h)
// @retail 0x3126f0
void hkRigidBody::pose_write(hkTransform const &transform)
{
	placed_shape_312800 *self = (placed_shape_312800 *)this;
	transform_312630 const &placement = (transform_312630 const &)transform;
	((hkTransform &)self->transform).set(transform);
	bounds_312800 bounds;
	((shape_312800 *)self->body.shape)->get_bounds(placement, ((world_312590 *)self->field_8)->tolerance->value, &bounds);
	self->function_315a10(&bounds);
}

// @retail 0x312760
void placed_shape_312800::function_312760(bounds_312800 *bounds_out) const
{
	((shape_312800 *)body.shape)->get_bounds(transform, ((world_312590 *)field_8)->tolerance->value, bounds_out);
}
