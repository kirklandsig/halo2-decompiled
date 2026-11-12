// @flags /O2 /GL-
// Havok (third-party physics library): walking a tree stored as a command
// stream in fixed-point coordinates, keeping the box of the node reached, and
// collecting the nodes a set of planes holds (0x2dc220) or a sphere touches
// (0x2dce60).
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"
#include "stream_holder_2d8e70.h"

// what reports a stream's errors: its second slot gives the stream to write
// the message to, which also ends the report (src/unknown_2fefd0.cpp)
class reporter_2ff210
{
public:
	virtual void slot_0() {}
	virtual stream_holder_2d8e70 *slot_1() { return 0; }
};

extern reporter_2ff210 *g_48017c;

struct class_2d9060
{
	long function_2d9060();
};

// where a command stream stands, as the offset lists see it
struct position_2ff210
{
	long offset;
	long values[1];
};

ELEMENTS_2D91D0(offsets_2ff210, long);

// a list the commands below a node add their offsets to (src/unknown_2fefd0.cpp)
class offset_list_2ff210
{
public:
	offsets_2ff210 *offsets;

	void function_2ff210(position_2ff210 const *position, unsigned char const *commands);
	void function_2ff570(position_2ff210 const *position, unsigned char const *commands);
};

// where the walk stands: an offset, a value the commands set, the node's
// corner in fixed point, and the shift of its units
struct pending_2ddbd0
{
	long offset;
	long values[1];
	long corner[3];
	long shift;
};

// the position as a copy that may be changed: the caller's at first
__forceinline pending_2ddbd0 *writable_2dce60(pending_2ddbd0 *&position, pending_2ddbd0 &copy)
{
	if (position != &copy)
	{
		copy = *position;
		position = &copy;
	}
	return &copy;
}

// a big-endian number of two, three or four bytes
#define READ_2_2DCE60(p) (((p)[0] << 8) + (p)[1])
#define READ_3_2DCE60(p) (((((p)[0] << 8) + (p)[1]) << 8) + (p)[2])
#define READ_4_2DCE60(p) (((((((p)[0] << 8) + (p)[1]) << 8) + (p)[2]) << 8) + (p)[3])

static __declspec(align(16)) const unsigned long g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

// the sum of a vector's first three terms, in the first
inline __m128 sum_2dce60(__m128 const &v)
{
	return _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v));
}

// the tree's fixed-point coordinates: an offset, then a scale (the box of the
// node reached, as the walk below passes it on)
struct fixed_point_2dc220
{
	__m128 offset;
	__m128 scale;
};

// four planes, one term to a vector
struct planes4_2dc220
{
	__m128 x;
	__m128 y;
	__m128 z;
	__m128 d;
};

// the planes' side of the walk: the list the nodes they hold whole go to, the
// number of groups of four planes, the groups, and their normals' terms'
// magnitudes
struct plane_test_2dc220
{
	offset_list_2ff210 held;
	char unknown4[0xc];
	long group_count;
	char unknown14[0xc];
	planes4_2dc220 groups[8];
	planes4_2dc220 magnitudes[8];

	// 0 when a node's box lies outside one of the planes, 1 when it lies
	// inside all of them (its nodes then collected), 2 otherwise
	__forceinline int test_box(__m128 const *box, pending_2ddbd0 const *position, unsigned char const *commands)
	{
		__m128 const centre = _mm_add_ps(box[0], box[1]);
		__m128 const extent = _mm_sub_ps(box[1], box[0]);
		int inside = 0xf;
		planes4_2dc220 const *group = groups;
		planes4_2dc220 const *magnitude = magnitudes;
		long remaining = group_count - 1;
		if (remaining >= 0)
		{
			__m128 const extent_z = _mm_shuffle_ps(extent, extent, 0xaa);
			__m128 const extent_y = _mm_shuffle_ps(extent, extent, 0x55);
			__m128 const extent_x = _mm_shuffle_ps(extent, extent, 0x00);
			__m128 const centre_z = _mm_shuffle_ps(centre, centre, 0xaa);
			__m128 const centre_y = _mm_shuffle_ps(centre, centre, 0x55);
			__m128 const centre_x = _mm_shuffle_ps(centre, centre, 0x00);
			do
			{
				__m128 const reach = _mm_add_ps(_mm_add_ps(_mm_mul_ps(magnitude->x, extent_x), _mm_mul_ps(magnitude->y, extent_y)), _mm_add_ps(_mm_mul_ps(magnitude->z, extent_z), magnitude->d));
				__m128 const distance = _mm_add_ps(_mm_add_ps(_mm_mul_ps(group->x, centre_x), _mm_mul_ps(group->y, centre_y)), _mm_add_ps(_mm_mul_ps(group->z, centre_z), group->d));
				if (_mm_movemask_ps(_mm_sub_ps(distance, reach)) != 0xf)
				{
					goto outside;
				}
				inside &= _mm_movemask_ps(_mm_add_ps(distance, reach));
				group++;
				magnitude++;
			} while (--remaining >= 0);
		}
		if (inside == 0xf)
		{
			held.function_2ff570((position_2ff210 const *)position, commands);
			held.function_2ff210((position_2ff210 const *)position, commands);
			return 1;
		}
		return 2;
	outside:
		return 0;
	}
};

// the walk that collects what a set of planes holds: the offsets of the nodes
// it reaches, the size of one unit, and the planes' side
class class_2dc220
{
public:
	offsets_2ff210 *reached;
	char unknown4[0xc];
	float scale;
	char unknown14[0xc];
	plane_test_2dc220 test;

	void function_2dc220(pending_2ddbd0 *pending, void const *root, fixed_point_2dc220 const *fixed_point);
};

// score 327 (was 330): the same length as retail. The walk's position is
// the parameter itself, so it is loaded after the first command's range check
// as in retail. A split at the next
// unit stores the high bound first, so the node's corner is read again for
// the low one (as retail does; storing them in retail's order reads it once
// into ebx, 368, also with both bounds named first or the low one computed
// inline), and the plane test collects at the end with the outside case
// after it (a local array for the two bounds scored 806). Still: the first
// side's bound loop loads q[1] once ahead of its
// branch (retail loads it in each arm; testing the corner bounds first, which
// helps 0x2dce60, costs 245 here); in the plane test retail spills the centre
// splats z/y/x to 0x20/0x30/0x40 (ours x/y/z) and reloads z and y at the loop
// top, entering past them, where ours reloads them on the back edge (a 7-byte
// pad and the outside case land there); declaration orders, non-const splats
// and other loop forms did not move it.
// Byte bound selectors 802; bound cursors 849; named coordinate indices 327;
// named products in both sums 327; plane-array end 776; first result tested
// first 781; byte result 754; ordinary inline 1782. All restored.
// The same walk as 0x2dce60 below, for the nodes a set of planes holds: the
// box is the third argument
// @retail 0x2dc220
void class_2dc220::function_2dc220(pending_2ddbd0 *position, void const *root, fixed_point_2dc220 const *fixed_point)
{
	pending_2ddbd0 copy;
	__m128 *box = (__m128 *)fixed_point;
	unsigned char const *commands = (unsigned char const *)root;
	__m128 other[2];
	long first_offset;
	long second_offset;
	long reached_offset;
	long value;
	for (;;)
	{
		long const op = *commands;
		switch (op)
		{
		case 0x13: case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: case 0x19:
		case 0x1a: case 0x1b: case 0x1c:
		{
			long const skip = commands[3];
			other[0] = box[0];
			other[1] = box[1];
			commands += 4;
			function_2dc220(position, commands, (fixed_point_2dc220 const *)other);
			commands += skip;
			continue;
		}
		case 0x10:
		case 0x11:
		case 0x12:
		{
			other[0] = box[0];
			other[1] = box[1];
			long const low = commands[2];
			box[1].m128_f32[op - 0x10] = (float)((commands[1] << position->shift) + position->corner[op - 0x10]) * scale;
			other[0].m128_f32[op - 0x10] = (float)((low << position->shift) + position->corner[op - 0x10]) * scale;
			first_offset = 0;
			commands += 4;
			second_offset = commands[-1];
			goto split;
		}
		case 0x20:
		case 0x21:
		case 0x22:
		{
			other[0] = box[0];
			other[1] = box[1];
			long const low = commands[1];
			box[1].m128_f32[op - 0x20] = (float)(((low + 1) << position->shift) + position->corner[op - 0x20]) * scale;
			other[0].m128_f32[op - 0x20] = (float)((low << position->shift) + position->corner[op - 0x20]) * scale;
			first_offset = 0;
			commands += 3;
			second_offset = commands[-1];
			goto split;
		}
		case 0x23:
		case 0x24:
		case 0x25:
		{
			other[0] = box[0];
			other[1] = box[1];
			long const low = commands[2];
			box[1].m128_f32[op - 0x23] = (float)((commands[1] << position->shift) + position->corner[op - 0x23]) * scale;
			other[0].m128_f32[op - 0x23] = (float)((low << position->shift) + position->corner[op - 0x23]) * scale;
			first_offset = READ_2_2DCE60(commands + 3);
			second_offset = READ_2_2DCE60(commands + 5);
			commands += 7;
			goto split;
		}
		case 0x29:
		case 0x2a:
		case 0x2b:
			box[0].m128_f32[op - 0x29] = (float)READ_3_2DCE60(commands + 1) * scale;
			box[1].m128_f32[op - 0x29] = (float)READ_3_2DCE60(commands + 4) * scale;
			commands += 7;
			goto bounds;
		case 0x26:
		case 0x27:
		case 0x28:
			box[0].m128_f32[op - 0x26] = (float)((commands[1] << position->shift) + position->corner[op - 0x26]) * scale;
			box[1].m128_f32[op - 0x26] = (float)((commands[2] << position->shift) + position->corner[op - 0x26]) * scale;
			commands += 3;
		bounds:
		{
			unsigned char const next = *commands;
			if (next >= 0x26 && next <= 0x2b)
			{
				continue;
			}
			if (test.test_box(box, position, commands) != 2)
			{
				return;
			}
			continue;
		}
		case 0x05:
			commands += commands[1] + 2;
			continue;
		case 0x06:
			commands += READ_2_2DCE60(commands + 1) + 3;
			continue;
		case 0x07:
			commands += READ_3_2DCE60(commands + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
			copy.corner[0] = (commands[1] << position->shift) + position->corner[0];
			copy.corner[1] = (commands[2] << position->shift) + position->corner[1];
			copy.corner[2] = (commands[3] << position->shift) + position->corner[2];
			copy.shift = position->shift - op;
			copy.values[0] = position->values[0];
			copy.offset = position->offset;
			position = &copy;
			commands += 4;
			continue;
		case 0x09:
		{
			long const delta = commands[1];
			writable_2dce60(position, copy)->offset += delta;
			commands += 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_2DCE60(commands + 1);
			writable_2dce60(position, copy)->offset += delta;
			commands += 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_2DCE60(commands + 1);
			writable_2dce60(position, copy)->offset = start;
			commands += 5;
			continue;
		}
		// the value in the first of four slots (retail writes the slot the
		// command names, past the end for the other three)
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		{
			long const read = commands[1];
			commands += 2;
			copy.values[op - 0x60] = read;
			goto record_value;
		}
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			copy.values[op - 0x64] = READ_2_2DCE60(commands + 1);
			commands += 3;
			goto record_value;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_2DCE60(commands + 1);
			commands += 5;
		record_value:
			value = copy.values[0];
			writable_2dce60(position, copy)->values[0] = value;
			continue;
		default:
			g_48017c->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)g_48017c->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		case 0x50:
			reached_offset = commands[1];
			goto reached;
		case 0x51:
			reached_offset = READ_2_2DCE60(commands + 1);
			goto reached;
		case 0x52:
			reached_offset = READ_3_2DCE60(commands + 1);
			goto reached;
		case 0x53:
			reached_offset = READ_4_2DCE60(commands + 1);
			goto reached;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			reached_offset = op - 0x30;
			goto reached;
		}

	split:
		// the bounds that follow each side narrow its box
		for (long bound = commands[first_offset]; bound >= 0x26 && bound <= 0x2b; bound = commands[first_offset])
		{
			unsigned char const *q = commands + first_offset;
			if (bound >= 0x29)
			{
				box[0].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 1) * scale;
				box[1].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 4) * scale;
				first_offset += 7;
			}
			else
			{
				box[0].m128_f32[bound - 0x26] = (float)((q[1] << position->shift) + position->corner[bound - 0x26]) * scale;
				box[1].m128_f32[bound - 0x26] = (float)((q[2] << position->shift) + position->corner[bound - 0x26]) * scale;
				first_offset += 3;
			}
		}
		for (long bound = commands[second_offset]; bound >= 0x26 && bound <= 0x2b; bound = commands[second_offset])
		{
			unsigned char const *q = commands + second_offset;
			if (bound >= 0x29)
			{
				other[0].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 1) * scale;
				other[1].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 4) * scale;
				second_offset += 7;
			}
			else
			{
				other[0].m128_f32[bound - 0x26] = (float)((q[1] << position->shift) + position->corner[bound - 0x26]) * scale;
				other[1].m128_f32[bound - 0x26] = (float)((q[2] << position->shift) + position->corner[bound - 0x26]) * scale;
				second_offset += 3;
			}
		}
		{
			unsigned char const *second = commands + second_offset;
			int const second_state = test.test_box(other, position, second);
			commands += first_offset;
			int const first_state = test.test_box(box, position, commands);
			if (second_state != 2)
			{
				if (first_state != 2)
				{
					return;
				}
			}
			else if (first_state != 2)
			{
				box[0] = other[0];
				box[1] = other[1];
				commands = second;
			}
			else
			{
				function_2dc220(position, second, (fixed_point_2dc220 const *)other);
			}
		}
	}

reached:
	reached->append(position->offset + reached_offset);
}

// the sphere's side of the walk: the list the nodes it holds whole go to, and
// the sphere's centre (doubled, from the tree's origin) with its diameter as
// the fourth term
struct sphere_test_2dce60
{
	offset_list_2ff210 held;
	char unknown4[0xc];
	__m128 sphere;

	// 0 when a node's box misses the sphere, 1 when the sphere holds it whole
	// (its nodes then collected), 2 when it holds part of it
	__forceinline int test_box(__m128 const *box, pending_2ddbd0 const *position, unsigned char const *commands)
	{
		float const diameter = sphere.m128_f32[3];
		float const diameter_squared = diameter * diameter;
		__m128 const extent = _mm_sub_ps(box[1], box[0]);
		__m128 const distance = _mm_and_ps(_mm_sub_ps(_mm_add_ps(box[0], box[1]), sphere), *(__m128 const *)g_455940);
		__m128 const outside = _mm_sub_ps(_mm_min_ps(distance, extent), distance);
		float nearest_squared;
		_mm_store_ss(&nearest_squared, sum_2dce60(_mm_mul_ps(outside, outside)));
		if (nearest_squared > diameter_squared)
		{
			return 0;
		}
		__m128 const far_corner = _mm_add_ps(distance, extent);
		float farthest_squared;
		_mm_store_ss(&farthest_squared, sum_2dce60(_mm_mul_ps(far_corner, far_corner)));
		if (farthest_squared < diameter_squared)
		{
			held.function_2ff570((position_2ff210 const *)position, commands);
			held.function_2ff210((position_2ff210 const *)position, commands);
			return 1;
		}
		return 2;
	}
};

// the walk that collects what a sphere touches: the offsets of the nodes it
// reaches, the size of one unit, and the sphere's side
class class_2dce60
{
public:
	offsets_2ff210 *reached;
	char unknown4[0xc];
	float scale;
	char unknown14[0xc];
	sphere_test_2dce60 test;

	void function_2dce60(pending_2ddbd0 *pending, void const *root, __m128 const *offset);
};

// score 86 (was 122): the same length as retail. The walk's position is
// the parameter itself, so it is loaded after the first command's range check
// as in retail. A split at the next unit stores its high bound first; retail
// stores the low bound first, but every low-first form tried here (named or
// inline bounds, the high reassigned, `int`, an unsigned char unit, compound
// assignments) computes the high with inc where retail uses lea. Case 0x23's
// low is in edx (retail ecx). The first side's bound
// loop tests for the corner bounds first: with the three-byte arm first (as
// retail lays it out) the compiler loads q[1] once ahead of the branch, five
// bytes short, which misaligned everything after it; this way the length and
// alignment match but the two arms sit in the other order and the three-byte
// arm's registers differ (ecx/edx swapped). Tried for the arm-first order: the
// operands in any order, an inline reader, `!(bound < 0x29)`, `bound > 0x28`,
// a `continue` arm, a separate local for the shift or the index, and an inline
// for the whole loop (+24 bytes) or for one bound (+8 bytes); also the shift
// arm reading through
// commands[first_offset + 1], a pointer for the lower corner, the first read
// as a named local (all 195-197; the hoisted load sits between the cmp and
// the jl, so retail's arm must get another register for it); a 10-minute
// permuter run found nothing below 124. Still: the per-side states trade
// stack slots (also with the second side's commands passed straight to its
// test); the three-byte reads of case 0x29 have ecx/edx swapped (any macro
// form gives the same code).
// First classification outermost 592; positive second classification 105;
// one final return 86; byte scan ordinals 702; unsigned scan ordinals 93;
// aligned intrinsic transfers 758; ordinary helper inline 86; memcpy copy 86.
// NOT kept: no qualifying gain; the body and shared helper are restored.
// Walks the commands below a node whose box is `offset` (a lower and an upper
// corner, narrowed in place as the walk goes down): a split gives each side
// its box, a side the sphere misses is dropped, a side it holds whole has its
// nodes collected, and the walk goes on into a side it holds part of (the
// second side, when both are, by a call of its own); each node reached adds
// its offset to the list
// @retail 0x2dce60
void class_2dce60::function_2dce60(pending_2ddbd0 *position, void const *root, __m128 const *offset)
{
	pending_2ddbd0 copy;
	__m128 *box = (__m128 *)offset;
	unsigned char const *commands = (unsigned char const *)root;
	__m128 other[2];
	long first_offset;
	long second_offset;
	long reached_offset;
	long value;
	for (;;)
	{
		long const op = *commands;
		switch (op)
		{
		case 0x13: case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: case 0x19:
		case 0x1a: case 0x1b: case 0x1c:
		{
			long const skip = commands[3];
			other[0] = box[0];
			other[1] = box[1];
			commands += 4;
			function_2dce60(position, commands, other);
			commands += skip;
			continue;
		}
		case 0x10:
		case 0x11:
		case 0x12:
		{
			other[0] = box[0];
			other[1] = box[1];
			long const low = commands[2];
			box[1].m128_f32[op - 0x10] = (float)((commands[1] << position->shift) + position->corner[op - 0x10]) * scale;
			other[0].m128_f32[op - 0x10] = (float)((low << position->shift) + position->corner[op - 0x10]) * scale;
			first_offset = 0;
			commands += 4;
			second_offset = commands[-1];
			goto split;
		}
		case 0x20:
		case 0x21:
		case 0x22:
		{
			other[0] = box[0];
			other[1] = box[1];
			long const low = commands[1];
			box[1].m128_f32[op - 0x20] = (float)(((low + 1) << position->shift) + position->corner[op - 0x20]) * scale;
			other[0].m128_f32[op - 0x20] = (float)((low << position->shift) + position->corner[op - 0x20]) * scale;
			first_offset = 0;
			commands += 3;
			second_offset = commands[-1];
			goto split;
		}
		case 0x23:
		case 0x24:
		case 0x25:
		{
			other[0] = box[0];
			other[1] = box[1];
			long const low = commands[2];
			box[1].m128_f32[op - 0x23] = (float)((commands[1] << position->shift) + position->corner[op - 0x23]) * scale;
			other[0].m128_f32[op - 0x23] = (float)((low << position->shift) + position->corner[op - 0x23]) * scale;
			first_offset = READ_2_2DCE60(commands + 3);
			second_offset = READ_2_2DCE60(commands + 5);
			commands += 7;
			goto split;
		}
		case 0x29:
		case 0x2a:
		case 0x2b:
			box[0].m128_f32[op - 0x29] = (float)READ_3_2DCE60(commands + 1) * scale;
			box[1].m128_f32[op - 0x29] = (float)READ_3_2DCE60(commands + 4) * scale;
			commands += 7;
			goto bounds;
		case 0x26:
		case 0x27:
		case 0x28:
			box[0].m128_f32[op - 0x26] = (float)((commands[1] << position->shift) + position->corner[op - 0x26]) * scale;
			box[1].m128_f32[op - 0x26] = (float)((commands[2] << position->shift) + position->corner[op - 0x26]) * scale;
			commands += 3;
		bounds:
		{
			unsigned char const next = *commands;
			if (next >= 0x26 && next <= 0x2b)
			{
				continue;
			}
			if (test.test_box(box, position, commands) != 2)
			{
				return;
			}
			continue;
		}
		case 0x05:
			commands += commands[1] + 2;
			continue;
		case 0x06:
			commands += READ_2_2DCE60(commands + 1) + 3;
			continue;
		case 0x07:
			commands += READ_3_2DCE60(commands + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
			copy.corner[0] = (commands[1] << position->shift) + position->corner[0];
			copy.corner[1] = (commands[2] << position->shift) + position->corner[1];
			copy.corner[2] = (commands[3] << position->shift) + position->corner[2];
			copy.shift = position->shift - op;
			copy.values[0] = position->values[0];
			copy.offset = position->offset;
			position = &copy;
			commands += 4;
			continue;
		case 0x09:
		{
			long const delta = commands[1];
			writable_2dce60(position, copy)->offset += delta;
			commands += 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_2DCE60(commands + 1);
			writable_2dce60(position, copy)->offset += delta;
			commands += 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_2DCE60(commands + 1);
			writable_2dce60(position, copy)->offset = start;
			commands += 5;
			continue;
		}
		// the value in the first of four slots (retail writes the slot the
		// command names, past the end for the other three)
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		{
			long const read = commands[1];
			commands += 2;
			copy.values[op - 0x60] = read;
			goto record_value;
		}
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			copy.values[op - 0x64] = READ_2_2DCE60(commands + 1);
			commands += 3;
			goto record_value;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_2DCE60(commands + 1);
			commands += 5;
		record_value:
			value = copy.values[0];
			writable_2dce60(position, copy)->values[0] = value;
			continue;
		default:
			g_48017c->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)g_48017c->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		case 0x50:
			reached_offset = commands[1];
			goto reached;
		case 0x51:
			reached_offset = READ_2_2DCE60(commands + 1);
			goto reached;
		case 0x52:
			reached_offset = READ_3_2DCE60(commands + 1);
			goto reached;
		case 0x53:
			reached_offset = READ_4_2DCE60(commands + 1);
			goto reached;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			reached_offset = op - 0x30;
			goto reached;
		}

	split:
		// the bounds that follow each side narrow its box
		for (long bound = commands[first_offset]; bound >= 0x26 && bound <= 0x2b; bound = commands[first_offset])
		{
			unsigned char const *q = commands + first_offset;
			if (bound < 0x29)
			{
				box[0].m128_f32[bound - 0x26] = (float)((q[1] << position->shift) + position->corner[bound - 0x26]) * scale;
				box[1].m128_f32[bound - 0x26] = (float)((q[2] << position->shift) + position->corner[bound - 0x26]) * scale;
				first_offset += 3;
			}
			else
			{
				box[0].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 1) * scale;
				box[1].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 4) * scale;
				first_offset += 7;
			}
		}
		for (long bound = commands[second_offset]; bound >= 0x26 && bound <= 0x2b; bound = commands[second_offset])
		{
			unsigned char const *q = commands + second_offset;
			if (bound >= 0x29)
			{
				other[0].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 1) * scale;
				other[1].m128_f32[bound - 0x29] = (float)READ_3_2DCE60(q + 4) * scale;
				second_offset += 7;
			}
			else
			{
				other[0].m128_f32[bound - 0x26] = (float)((q[1] << position->shift) + position->corner[bound - 0x26]) * scale;
				other[1].m128_f32[bound - 0x26] = (float)((q[2] << position->shift) + position->corner[bound - 0x26]) * scale;
				second_offset += 3;
			}
		}
		{
			unsigned char const *second = commands + second_offset;
			int const second_state = test.test_box(other, position, second);
			commands += first_offset;
			int const first_state = test.test_box(box, position, commands);
			if (second_state != 2)
			{
				if (first_state != 2)
				{
					return;
				}
			}
			else if (first_state != 2)
			{
				box[0] = other[0];
				box[1] = other[1];
				commands = second;
			}
			else
			{
				function_2dce60(position, second, other);
			}
		}
	}

reached:
	reached->append(position->offset + reached_offset);
}
