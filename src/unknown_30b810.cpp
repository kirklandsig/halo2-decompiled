// @flags /O2 /GL-
// Havok (third-party physics library): a cast of an object placed by a
// transform along a path: the object moved to the path's start, its bounds
// stretched along the path to find what it may meet, then the cast handed,
// for each body it overlaps (the last first), to the function the world's
// table gives for the two shapes' kinds.
#include "placed_shape_312800.h"

// what the cast is given: where it ends, two values and a tolerance
struct cast_30b810
{
	__m128 end;
	long field_10;
	float tolerance;
};

// the world's settings the cast starts from (16 bytes)
struct settings_30b810
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

// the cast as the shape functions take it (src/unknown_30b710.cpp)
class class_30b710
{
public:
	char unknown0[8];
	long field_8;
	char unknownc[4];
	__m128 vector;
	char unknown20[4];
	float length;

	void set(__m128 const *vector_, long value);
};

struct cast_input_30b810 : class_30b710
{
	cast_input_30b810()
	{
		*(float *)unknown20 = 1.0f / 8388608.0f;
	}
};

// a collidable the object overlaps: its shape first
struct collidable_30b810
{
	shape_312800 *shape;
};

typedef void (__cdecl *cast_function_30b810)(void *collidable, collidable_30b810 *other,
	cast_input_30b810 const *input, void *collector, void *start_collector);

struct dispatcher_30b810
{
	char unknown0[0x318c];
	cast_function_30b810 functions[1];
};

struct world_30b810
{
	char unknown0[0xc8];
	dispatcher_30b810 *dispatcher;
	settings_30b810 *settings;
};

class cast_volume_30b810 : public placed_shape_312800
{
public:
	collidable_30b810 **bodies;
	long body_count;

	void function_30b810(__m128 const *start, cast_30b810 const *cast, void *collector, void *start_collector);
};

// @retail 0x30b810
void cast_volume_30b810::function_30b810(__m128 const *start, cast_30b810 const *cast, void *collector,
	void *start_collector)
{
	transform.translation = *start;
	bounds_312800 bounds;
	((shape_312800 *)body.shape)->get_bounds(transform, ((world_312590 *)field_8)->tolerance->value + cast->tolerance,
		&bounds);
	__m128 path = _mm_sub_ps(cast->end, *start);
	__m128 zero = _mm_setzero_ps();
	bounds.minimum = _mm_add_ps(bounds.minimum, _mm_min_ps(zero, path));
	bounds.maximum = _mm_add_ps(bounds.maximum, _mm_max_ps(zero, path));
	function_315a10(&bounds);
	world_30b810 *world = (world_30b810 *)field_8;
	cast_input_30b810 input;
	settings_30b810 const *from = world->settings;
	settings_30b810 *to = (settings_30b810 *)&input;
	to->field_0 = from->field_0;
	to->field_4 = from->field_4;
	to->field_8 = from->field_8;
	to->field_c = from->field_c;
	input.set(&path, *(long const *)&cast->tolerance);
	*(long *)input.unknown20 = cast->field_10;
	dispatcher_30b810 *dispatcher = world->dispatcher;
	for (int i = body_count - 1; i >= 0; i--)
	{
		collidable_30b810 **other = &bodies[i];
		long kind = (long)(*other)->shape->slot_5();
		dispatcher->functions[((long)field_54 << 5) + kind](&body.shape, *other, &input, collector, start_collector);
	}
}
