// @flags /O2 /GL-
// Havok (third-party physics library): sweeping a segment through a tree
// stored as a command stream in fixed-point coordinates, each range widened by
// how far the swept shape reaches, and handing each leaf reached to the
// collision function the pair's types choose.
#include <xmmintrin.h>
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

static __declspec(align(16)) const float g_412420[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// a grid: its origin, with its cell size in the fourth lane
struct grid_301a60
{
	__m128 origin;
};

// whether a test passed, given back by value
struct answer_301a60
{
	answer_301a60() {}

	bool value;
};

// a leaf's shape as the sweep sees it: its slot 5 gives its type
class leaf_301a60
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long leaf_kind() = 0;
};

// the shape the tree's leaves belong to: its slot 13 gives a leaf's shape
class shape_301a60
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	virtual leaf_301a60 *leaf_at_key(long key, void *buffer) = 0;
};

struct shape_holder_301a60
{
	char unknown0[0xc];
	shape_301a60 *shape;
};

// the tree's side of the pair: its shape's holder, and a value passed on
struct body_301a60
{
	shape_holder_301a60 *holder;
	long field_4;
	long field_8;
};

struct dispatcher_301a60;

// what decides whether a leaf is swept against
class filter_301a60
{
public:
	virtual answer_301a60 slot_0(dispatcher_301a60 *dispatcher, void const *swept, body_301a60 const *body,
		shape_301a60 *shape, long key) = 0;
};

// the leaf and its body as the collision function sees them
struct hit_info_301a60
{
	leaf_301a60 *leaf;
	long key;
	long field_8;
	body_301a60 const *body;
};

typedef void (__cdecl *collide_301a60)(void const *swept, hit_info_301a60 const *leaf, dispatcher_301a60 *dispatcher,
	void *collector, long arg);

// the table of collision functions, by the two shapes' types (32 to a row)
struct dispatcher_301a60
{
	char *table;
	filter_301a60 *filter;
};

// a segment and the direction it sweeps, and the two sides of the pair
struct input_301a60
{
	__m128 start;
	__m128 end;
	__m128 direction;
	dispatcher_301a60 *dispatcher;
	void const *swept;
	body_301a60 const *body;
};

// where the closest hit so far is kept
struct collector_301a60
{
	long field_0;
	float fraction;
};

// the walk's state: the offset of the tree's fixed point, how far the swept
// shape reaches along each axis and along the diagonals, the depth, the size
// of one unit, the base of the leaves' keys, and a value the commands set
// a vector of four floats, copied as a whole
struct vector_301a60
{
	__m128 value;

	void operator=(vector_301a60 const &other)
	{
		value = other.value;
	}
};

struct walk_3028c0
{
	vector_301a60 offset;
	vector_301a60 reach;
	float diagonal_reach;
	long depth;
	float scale;
	long key_base;
	long values[1];
};

struct segment_3028c0
{
	__m128 start;
	__m128 end;
};

class class_301a60
{
public:
	char unknown0[0x10];
	grid_301a60 const *grid;
	float inverse_cell_size;
	long swept_type;
	float fraction;
	float last_fraction;
	collector_301a60 *collector;
	long field_28;
	input_301a60 const *input;

	void function_301a60(walk_3028c0 *walk, char const *cells, segment_3028c0 const *segment);
};

// a big-endian number of two, three or four bytes
#define READ_2_301A60(p) (((p)[0] << 8) + (p)[1])
#define READ_3_301A60(p) (((((p)[0] << 8) + (p)[1]) << 8) + (p)[2])
#define READ_4_301A60(p) (((((((p)[0] << 8) + (p)[1]) << 8) + (p)[2]) << 8) + (p)[3])

// the position as a copy that may be changed: the caller's at first
__forceinline walk_3028c0 *writable_301a60(walk_3028c0 *&position, walk_3028c0 &copy)
{
	if (position != &copy)
	{
		copy = *position;
		position = &copy;
	}
	return &copy;
}

// a vector of four copies of a float
__forceinline __m128 splat_301a60(float const &value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the point a fraction of the way from one point to another
__forceinline __m128 lerp_301a60(__m128 from, __m128 to, float fraction)
{
	__m128 const t = splat_301a60(fraction);
	return _mm_add_ps(_mm_mul_ps(_mm_sub_ps(*(__m128 const *)g_412420, t), from), _mm_mul_ps(t, to));
}

// a vector less another, in place
__forceinline void subtract_301a60(__m128 &value, __m128 const &other)
{
	value = _mm_sub_ps(value, other);
}

// a vector times a number, in place
__forceinline void scale_301a60(__m128 &value, float factor)
{
	__m128 const single = _mm_set_ss(factor);
	value = _mm_mul_ps(_mm_shuffle_ps(single, single, 0), value);
}

// score 68 (was 806): rebuilt in the shape of its sibling 0x3009e0: commands are
// stepped before their operands are read (cases 0x20, 0x23, 0x60), the corner
// math uses in-place subtract/scale helpers, the bounds reuse the function's
// a/b, the walk's vectors are copied member by member through a wrapper with its
// own assignment (as 0x3009e0's vector type), and the shape is read before the
// key. Left: key_base folded into an add, the 0x53 key byte order.
// Before, 806 (was 884): the recorded value goes into a writable copy of the walk
// (retail reloads it and stores it back); extent now agrees.
// Before, 884 (was 971): the splat helper takes the value by reference and sets it
// with _mm_set_ss, and the walk is read directly instead of through a copy.
// Before, 971 (was 1230; 980 on the fast check): 3680 against 3672 bytes, the same blocks in the same
// order (the copy of the walk's state is now field by field, as retail's).
// The first difference is at +0xd: retail reads the command into eax and loads
// the walk and segment pointers after the range check. Retail's low stack
// slots sit 4 bytes higher than ours throughout (ours puts the second scale
// splat's float low and retail one at +0x3c), and retail loads each splat's
// float straight from its slot where ours forms the address in a register
// first (_mm_set_ss, _mm_set_ps1 and _mm_load1_ps all scored worse).
// The sweep's walk down the tree, as the ray's (0x3009e0) with each split's
// and bound's range widened by the swept shape's reach; the segment's far end
// is pulled back whenever a leaf brings the closest hit nearer.
// @retail 0x301a60
void class_301a60::function_301a60(walk_3028c0 *walk, char const *cells, segment_3028c0 const *segment)
{
	unsigned char const *commands = (unsigned char const *)cells;
	walk_3028c0 copy;
	segment_3028c0 other;
	long first_offset;
	long second_offset;
	long key;
	long value;
	float first_end;
	float second_start;
	float reach;
	float a;
	float b;
	float low;
	float high;
	for (;;)
	{
		long const op = *commands;
		long axis = 999;
		switch (op)
		{
		case 0x13:
			reach = walk->reach.value.m128_f32[2] + walk->reach.value.m128_f32[1];
			reach += reach;
			second_start = (float)(commands[2] * 2) - reach;
			first_end = (float)(commands[1] * 2) + reach;
			a = segment->start.m128_f32[2] + segment->start.m128_f32[1];
			b = segment->end.m128_f32[2] + segment->end.m128_f32[1];
			goto split_4;
		case 0x14:
			reach = walk->reach.value.m128_f32[2] + walk->reach.value.m128_f32[1];
			reach += reach;
			second_start = (float)(commands[2] * 2 - 0xff) - reach;
			first_end = (float)(commands[1] * 2 - 0xff) + reach;
			a = segment->start.m128_f32[1] - segment->start.m128_f32[2];
			b = segment->end.m128_f32[1] - segment->end.m128_f32[2];
			goto split_4;
		case 0x15:
			reach = walk->reach.value.m128_f32[2] + walk->reach.value.m128_f32[0];
			reach += reach;
			second_start = (float)(commands[2] * 2) - reach;
			first_end = (float)(commands[1] * 2) + reach;
			a = segment->start.m128_f32[2] + segment->start.m128_f32[0];
			b = segment->end.m128_f32[2] + segment->end.m128_f32[0];
			goto split_4;
		case 0x16:
			reach = walk->reach.value.m128_f32[2] + walk->reach.value.m128_f32[0];
			reach += reach;
			second_start = (float)(commands[2] * 2 - 0xff) - reach;
			first_end = (float)(commands[1] * 2 - 0xff) + reach;
			a = segment->start.m128_f32[0] - segment->start.m128_f32[2];
			b = segment->end.m128_f32[0] - segment->end.m128_f32[2];
			goto split_4;
		case 0x17:
			reach = walk->reach.value.m128_f32[1] + walk->reach.value.m128_f32[0];
			reach += reach;
			second_start = (float)(commands[2] * 2) - reach;
			first_end = (float)(commands[1] * 2) + reach;
			a = segment->start.m128_f32[1] + segment->start.m128_f32[0];
			b = segment->end.m128_f32[1] + segment->end.m128_f32[0];
			goto split_4;
		case 0x18:
			reach = walk->reach.value.m128_f32[1] + walk->reach.value.m128_f32[0];
			reach += reach;
			second_start = (float)(commands[2] * 2 - 0xff) - reach;
			first_end = (float)(commands[1] * 2 - 0xff) + reach;
			a = segment->start.m128_f32[0] - segment->start.m128_f32[1];
			b = segment->end.m128_f32[0] - segment->end.m128_f32[1];
			goto split_4;
		case 0x19:
			reach = walk->diagonal_reach;
			second_start = (float)(commands[2] * 3) - reach;
			first_end = (float)(commands[1] * 3) + reach;
			a = segment->start.m128_f32[2] + segment->start.m128_f32[1] + segment->start.m128_f32[0];
			b = segment->end.m128_f32[2] + segment->end.m128_f32[1] + segment->end.m128_f32[0];
			goto split_4;
		case 0x1a:
			reach = walk->diagonal_reach;
			second_start = (float)((commands[2] - 0x55) * 3) - reach;
			first_end = (float)((commands[1] - 0x55) * 3) + reach;
			a = segment->start.m128_f32[1] + segment->start.m128_f32[0] - segment->start.m128_f32[2];
			b = segment->end.m128_f32[1] + segment->end.m128_f32[0] - segment->end.m128_f32[2];
			goto split_4;
		case 0x1b:
			reach = walk->diagonal_reach;
			second_start = (float)((commands[2] - 0x55) * 3) - reach;
			first_end = (float)((commands[1] - 0x55) * 3) + reach;
			a = segment->start.m128_f32[0] - segment->start.m128_f32[1] + segment->start.m128_f32[2];
			b = segment->end.m128_f32[0] - segment->end.m128_f32[1] + segment->end.m128_f32[2];
			goto split_4;
		case 0x1c:
			reach = walk->diagonal_reach;
			second_start = (float)((commands[2] - 0xaa) * 3) - reach;
			first_end = (float)((commands[1] - 0xaa) * 3) + reach;
			a = segment->start.m128_f32[0] - segment->start.m128_f32[1] - segment->start.m128_f32[2];
			b = segment->end.m128_f32[0] - segment->end.m128_f32[1] - segment->end.m128_f32[2];
			goto split_4;
		case 0x20:
		case 0x21:
		case 0x22:
		{
			axis = op - 0x20;
			second_start = (float)commands[1];
			first_end = second_start + 1.0f;
			commands += 3;
			first_offset = 0;
			second_offset = commands[-1];
			second_start -= walk->reach.value.m128_f32[axis];
			first_end += walk->reach.value.m128_f32[axis];
			a = segment->start.m128_f32[axis];
			b = segment->end.m128_f32[axis];
			goto split;
		}
		case 0x23:
		case 0x24:
		case 0x25:
			axis = op - 0x23;
			second_start = (float)commands[2] - walk->reach.value.m128_f32[axis];
			first_end = (float)commands[1] + walk->reach.value.m128_f32[axis];
			a = segment->start.m128_f32[axis];
			b = segment->end.m128_f32[axis];
			commands += 7;
			first_offset = READ_2_301A60(commands - 4);
			second_offset = READ_2_301A60(commands - 2);
			goto split;
		case 0x10:
		case 0x11:
		case 0x12:
			axis = op - 0x10;
			second_start = (float)commands[2] - walk->reach.value.m128_f32[axis];
			first_end = (float)commands[1] + walk->reach.value.m128_f32[axis];
			a = segment->start.m128_f32[axis];
			b = segment->end.m128_f32[axis];
		split_4:
			second_offset = commands[3];
			commands += 4;
			first_offset = 0;
		split:
			// both ends before the second child: only the first is reached
			if (b < second_start && a < second_start)
			{
				commands += first_offset;
				continue;
			}
			commands += second_offset;
			// both ends past the first child: only the second is reached
			if (a > first_end && b > first_end)
			{
				continue;
			}
			{
				float const high_a = a - first_end;
				__m128 const start = segment->start;
				__m128 const end = segment->end;
				other.start = start;
				other.end = end;
				float const low_a = a - second_start;
				float const high_b = b - first_end;
				float const low_b = b - second_start;
				if (high_a < high_b)
				{
					// the first child first, with the segment cut where it leaves it
					if (high_b * high_a < 0.0f)
					{
						other.end = lerp_301a60(start, end, high_a / (high_a - high_b));
					}
					function_301a60(walk, (char const *)(commands - second_offset + first_offset), &other);
					if (low_b * low_a < 0.0f)
					{
						((segment_3028c0 *)segment)->start = lerp_301a60(segment->start, segment->end, low_a / (low_a - low_b));
					}
					if (fraction < last_fraction)
					{
						last_fraction = fraction;
						((segment_3028c0 *)segment)->end = _mm_sub_ps(lerp_301a60(input->start, input->end, fraction), grid->origin);
						scale_301a60(((segment_3028c0 *)segment)->end, walk->scale);
						subtract_301a60(((segment_3028c0 *)segment)->end, walk->offset.value);
						if (axis < 3 && segment->end.m128_f32[axis] < second_start)
						{
							return;
						}
					}
				}
				else
				{
					// the second child first, with the segment cut where it leaves it
					if (low_b * low_a < 0.0f)
					{
						other.end = lerp_301a60(start, end, low_a / (low_a - low_b));
					}
					function_301a60(walk, (char const *)commands, &other);
					if (high_b * high_a < 0.0f)
					{
						((segment_3028c0 *)segment)->start = lerp_301a60(segment->start, segment->end, high_a / (high_a - high_b));
					}
					if (fraction < last_fraction)
					{
						last_fraction = fraction;
						((segment_3028c0 *)segment)->end = _mm_sub_ps(lerp_301a60(input->start, input->end, fraction), grid->origin);
						scale_301a60(((segment_3028c0 *)segment)->end, walk->scale);
						subtract_301a60(((segment_3028c0 *)segment)->end, walk->offset.value);
						if (axis < 3 && segment->end.m128_f32[axis] > first_end)
						{
							return;
						}
					}
					commands += first_offset - second_offset;
				}
			}
			continue;
		case 0x29:
		case 0x2a:
		case 0x2b:
		{
			axis = op - 0x29;
			float const from_low = ((float)READ_3_301A60(commands + 1) * inverse_cell_size) * walk->scale - walk->offset.value.m128_f32[axis];
			low = from_low - walk->reach.value.m128_f32[axis];
			high = ((float)READ_3_301A60(commands + 4) * inverse_cell_size) * walk->scale - walk->offset.value.m128_f32[axis] + walk->reach.value.m128_f32[axis];
			commands += 7;
			goto bounds;
		}
		case 0x26:
		case 0x27:
		case 0x28:
			axis = op - 0x26;
			low = (float)commands[1] - walk->reach.value.m128_f32[axis];
			high = (float)commands[2] + walk->reach.value.m128_f32[axis];
			commands += 3;
		bounds:
			// the segment cut to the node's range along the axis
			{
				a = segment->start.m128_f32[axis];
				b = segment->end.m128_f32[axis];
				long flip;
				if (a < b)
				{
					if (b < low)
					{
						return;
					}
					if (a > high)
					{
						return;
					}
					flip = 0;
				}
				else
				{
					if (a < low)
					{
						return;
					}
					if (b > high)
					{
						return;
					}
					flip = 1;
				}
				__m128 const start = segment->start;
				__m128 const end = segment->end;
				float const high_a = a - high;
				float const high_b = b - high;
				if (high_a * high_b < 0.0f)
				{
					(&((segment_3028c0 *)segment)->start)[1 - flip] = lerp_301a60(start, end, high_a / (high_a - high_b));
				}
				float const low_a = a - low;
				float const low_b = b - low;
				if (low_a * low_b < 0.0f)
				{
					(&((segment_3028c0 *)segment)->start)[flip] = lerp_301a60(start, end, low_a / (low_a - low_b));
				}
			}
			continue;
		case 0x05:
			commands += commands[1] + 2;
			continue;
		case 0x06:
			commands += READ_2_301A60(commands + 1) + 3;
			continue;
		case 0x07:
			commands += READ_3_301A60(commands + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		{
			__m128 corner;
			corner.m128_f32[0] = (float)commands[1];
			corner.m128_f32[1] = (float)commands[2];
			corner.m128_f32[2] = (float)commands[3];
			corner.m128_f32[3] = 0.0f;
			subtract_301a60(((segment_3028c0 *)segment)->start, corner);
			subtract_301a60(((segment_3028c0 *)segment)->end, corner);
			float const factor = (float)(1 << (unsigned char)op);
			__m128 const factor4 = splat_301a60(factor);
			((segment_3028c0 *)segment)->start = _mm_mul_ps(segment->start, factor4);
			((segment_3028c0 *)segment)->end = _mm_mul_ps(segment->end, factor4);
			copy.offset.value = _mm_mul_ps(_mm_add_ps(walk->offset.value, corner), factor4);
			copy.depth = walk->depth + op;
			copy.scale = factor * walk->scale;
			copy.reach.value = _mm_mul_ps(factor4, walk->reach.value);
			copy.diagonal_reach = factor * walk->diagonal_reach;
			copy.values[0] = walk->values[0];
			copy.key_base = walk->key_base;
			walk = &copy;
			commands += 4;
			continue;
		}
		case 0x09:
		{
			long const delta = commands[1];
			writable_301a60(walk, copy)->key_base += delta;
			commands += 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_301A60(commands + 1);
			writable_301a60(walk, copy)->key_base += delta;
			commands += 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_301A60(commands + 1);
			writable_301a60(walk, copy)->key_base = start;
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
			copy.values[op - 0x64] = READ_2_301A60(commands + 1);
			commands += 3;
			goto record_value;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_301A60(commands + 1);
			commands += 5;
		record_value:
			value = copy.values[0];
			writable_301a60(walk, copy)->values[0] = value;
			continue;
		case 0x50:
			key = commands[1];
			goto reached;
		case 0x51:
			key = READ_2_301A60(commands + 1);
			goto reached;
		case 0x52:
			key = READ_3_301A60(commands + 1);
			goto reached;
		case 0x53:
			key = (READ_2_301A60(commands + 1) << 16) + READ_2_301A60(commands + 3);
			goto reached;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			key = op - 0x30;
			goto reached;
		default:
			g_48017c->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)g_48017c->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		}
	}

reached:
	{
		shape_301a60 *shape = input->body->holder->shape;
		key += walk->key_base;
		if (!input->dispatcher->filter->slot_0(input->dispatcher, input->swept, input->body, shape, key).value)
		{
			return;
		}
		char buffer[0x100];
		leaf_301a60 *leaf = shape->leaf_at_key(key, buffer);
		hit_info_301a60 info;
		info.body = input->body;
		info.field_8 = input->body->field_8;
		info.leaf = leaf;
		info.key = key;
		long const type = leaf->leaf_kind();
		dispatcher_301a60 *dispatcher = input->dispatcher;
		((collide_301a60 *)(dispatcher->table + 0x318c))[swept_type * 32 + type](input->swept, &info, dispatcher,
			collector, field_28);
		fraction = collector->fraction;
	}
}
