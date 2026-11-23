// @flags /O2 /GL-
// Havok (third-party physics library): the walk of a tree's command stream
// for a range of cells (0x2ff8b0), and starting a query of a tree of cells
// for the keys of the cells a pair of bounds touches, the bounds taken into
// the tree's fixed-point grid; 0x2fff90 first takes the bounds of a box
// turned by a transform. The same for a sphere (0x3008d0), and the walk of the
// tree's command stream for it (0x300290).
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"
#include "stream_holder_2d8e70.h"

struct s_surface_key_array;

// three axes and a position (0x2da4d0)
struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

// every lane but the sign, and the sign alone
static __declspec(align(16)) const unsigned long g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};
static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

struct s_query_bounds
{
	__m128 minimum, maximum;
};

// a value rounded to the nearest integer by the FPU
__forceinline int round_300150(float value)
{
	int result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

// a tree's grid: its origin, its scale, and its nodes
struct tree_300150
{
	float origin[3];
	float scale;
	char unknown10[0x30 - 0x10];
	unsigned char nodes[1];
};

// a range of cells, in whole cells of the current level, and the walk's
// state: the corner of the current cell, the first key of the cell's subtree,
// how many levels the walk went down, and values the commands set
struct cell_box_2ff8b0
{
	long upper[3];
	long unknownc;
	long lower[3];
	long unknown1c;
	long corner[3];
	long key_base;
	long depth;
	long values[1];
};

// the same, as the queries build it
struct __declspec(align(16)) cell_range_2ff8b0 : cell_box_2ff8b0
{
};

class class_300150
{
public:
	s_surface_key_array *keys;
	long unknown4[3];
	long upper[3];
	long unknown1c;
	long lower[3];
	long unknown2c;
	tree_300150 const *tree;

	void function_300150(void const *tree, const s_query_bounds *bounds, s_surface_key_array *keys);
	void function_2ff8b0(cell_range_2ff8b0 const *range, unsigned char const *node);
};

// the same query for a box given by its half extents and a transform
class class_2fff90 : public class_300150
{
public:
	void function_2fff90(void const *tree, const c_query_transform *matrix, const __m128 *extent, float tolerance, s_surface_key_array *keys);
};

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

ELEMENTS_2D91D0(keys_300290, long);

// a big-endian number of two, three or four bytes
#define READ_2_300290(p) (((p)[0] << 8) + (p)[1])
#define READ_3_300290(p) (((((p)[0] << 8) + (p)[1]) << 8) + (p)[2])
#define READ_4_300290(p) (((((((p)[0] << 8) + (p)[1]) << 8) + (p)[2]) << 8) + (p)[3])

inline reporter_2ff210 *reporter_2ff8b0()
{
	return g_48017c;
}

// runs a tree's command stream for a range of cells: commands narrow the
// range to a child cell, split the walk where the range spans both sides of a
// plane (the first side by recursion), cut it off where the range misses a
// slab, move the key base, or give a key to add to the list
// Matched (was 132): the four times-three cases (0x19..0x1c) bind each product to a
// constant reference before copying it to the shared bound, so each product is a
// value of its own case and the byte is read into eax and multiplied into the
// bound's register, as in retail.
// Before, 132 (was 197): the default case reads the error reporter through a small
// accessor, so it is loaded before the command is pushed, as retail does.
// Before, 197 (was 216): cases 0x23..0x25 read the
// upper bound from the range, as retail does (it was read from the query's own
// bounds, a different value), and the corner case adds one to each upper bound and
// takes the corner off in a second statement (retail's inc before sub); with the
// second change the first no longer costs a frame slot.
// Before, 216 (was 234): cases 0x60..0x63 read the byte before stepping and
// storing, as in 0x300290.
// Before, 234 (was 466): each case continues the loop itself instead of breaking
// to a shared step (the compiler merges the copies where retail has its step),
// and the second bound test of cases 0x23..0x25 reads its bounds again.
// Before, 466 (was 466): Frame 0x3c versus 0x38; the two-child case takes
// the saved-object register for a bound instead of using the argument homes.
// Byte command 542; copy rename 466; memcpy 466; corner loop 466;
// bounds loop 482; byte OR 515; shared bound tuple 466; positive branch 469;
// case-local bounds 466; range cursor 467; command cursor 466; both 467;
// both byte bounds loaded first 492.
// Shared value block with inner switch 476, ascending width tests 587,
// descending width tests 515; trailing update 503; on trailing update:
// two-word decode 503, next-slot key 508, value cases first 556, saved-value
// rename 503. All restored.
// Base input type 466; four bound names 466; inner diagonal switch 706;
// sole void exit 466; recursion before second step 463; separate bound loops
// 466; low value-slot bits 466; append by reference 466. All restored.
// NOT kept: the recursion change gains three and moves farther from the size.
// @retail 0x2ff8b0
void class_300150::function_2ff8b0(cell_range_2ff8b0 const *range, unsigned char const *node)
{
	cell_box_2ff8b0 copy;
	long offset;
	long value;
	long below;
	long above;
	long low;
	long high;
	for (;;)
	{
		long const op = *node;
		switch (op)
		{
		// planes across two axes or three
		case 0x13:
			low = node[1] * 2;
			high = node[2] * 2;
			below = range->lower[2] + range->lower[1];
			above = range->upper[2] + range->upper[1];
			goto split;
		case 0x14:
			low = node[1] * 2 - 0xff;
			high = node[2] * 2 - 0xff;
			below = range->lower[1] - range->upper[2];
			above = range->upper[1] - range->lower[2];
			goto split;
		case 0x15:
			low = node[1] * 2;
			high = node[2] * 2;
			below = range->lower[2] + range->lower[0];
			above = range->upper[2] + range->upper[0];
			goto split;
		case 0x16:
			low = node[1] * 2 - 0xff;
			high = node[2] * 2 - 0xff;
			below = range->lower[0] - range->upper[2];
			above = range->upper[0] - range->lower[2];
			goto split;
		case 0x17:
			low = node[1] * 2;
			high = node[2] * 2;
			below = range->lower[1] + range->lower[0];
			above = range->upper[1] + range->upper[0];
			goto split;
		case 0x18:
			low = node[1] * 2 - 0xff;
			high = node[2] * 2 - 0xff;
			below = range->lower[0] - range->upper[1];
			above = range->upper[0] - range->lower[1];
			goto split;
		case 0x19:
			{
				long const &scaled_low = node[1] * 3;
				long const &scaled_high = node[2] * 3;
				low = scaled_low;
				high = scaled_high;
			}
			below = range->lower[2] + range->lower[1] + range->lower[0];
			above = range->upper[2] + range->upper[1] + range->upper[0];
			goto split;
		case 0x1a:
			{
				long const &scaled_low = (node[1] - 0x55) * 3;
				long const &scaled_high = (node[2] - 0x55) * 3;
				low = scaled_low;
				high = scaled_high;
			}
			below = range->lower[1] - range->upper[2] + range->lower[0];
			above = range->upper[1] - range->lower[2] + range->upper[0];
			goto split;
		case 0x1b:
			{
				long const &scaled_low = (node[1] - 0x55) * 3;
				long const &scaled_high = (node[2] - 0x55) * 3;
				low = scaled_low;
				high = scaled_high;
			}
			below = range->lower[2] - range->upper[1] + range->lower[0];
			above = range->upper[2] - range->lower[1] + range->upper[0];
			goto split;
		case 0x1c:
			{
				long const &scaled_low = (node[1] - 0xaa) * 3;
				long const &scaled_high = (node[2] - 0xaa) * 3;
				low = scaled_low;
				high = scaled_high;
			}
			below = range->lower[0] - range->upper[2] - range->upper[1];
			above = range->upper[0] - range->lower[2] - range->lower[1];
		// the range below the plane's first value goes on to the first side,
		// the range above its second value to the second side, the range
		// across both to both, the first by recursion
		split:
			node += 4;
			if (above <= high)
			{
				if (below >= low)
				{
					return;
				}
				continue;
			}
			{
				long const skip = node[-1];
				if (below >= low)
				{
					node += skip;
					continue;
				}
				function_2ff8b0(range, node);
				node += skip;
			}
			continue;

		// planes across one axis
		case 0x10:
		case 0x11:
		case 0x12:
		{
			long const first_end = node[2];
			long const second_start = node[1];
			long const along_below = range->lower[op - 0x10];
			node += 4;
			if (range->upper[op - 0x10] <= first_end)
			{
				if (along_below >= second_start)
				{
					return;
				}
				continue;
			}
			long const skip = node[-1];
			node += skip;
			if (along_below >= second_start)
			{
				continue;
			}
			function_2ff8b0(range, node - skip);
			continue;
		}

		// one plane across an axis, the second side's start given
		case 0x20:
		case 0x21:
		case 0x22:
		{
			long const plane = node[1];
			node += 3;
			if (range->upper[op - 0x20] <= plane)
			{
				continue;
			}
			long const skip = node[-1];
			long const along_below = range->lower[op - 0x20];
			node += skip;
			if (along_below > plane)
			{
				continue;
			}
			function_2ff8b0(range, node - skip);
			continue;
		}
		// two planes across an axis, both sides' starts given
		case 0x23:
		case 0x24:
		case 0x25:
		{
			long const low_2 = node[1];
			long const high_2 = node[2];
			long const below_2 = range->lower[op - 0x23];
			long const first_skip = READ_2_300290(node + 3);
			long const above_2 = range->upper[op - 0x23];
			node += 7;
			if (above_2 <= high_2)
			{
				if (below_2 >= low_2)
				{
					return;
				}
				node += first_skip;
				continue;
			}
			long const second_skip = READ_2_300290(node - 2);
			node += second_skip;
			if (below_2 >= low_2)
			{
				continue;
			}
			function_2ff8b0(range, node - second_skip + first_skip);
			continue;
		}
		// a slab across an axis the range must touch
		case 0x26:
		case 0x27:
		case 0x28:
			if (range->upper[op - 0x26] < node[1])
			{
				return;
			}
			if (range->lower[op - 0x26] >= node[2])
			{
				return;
			}
			node += 3;
			continue;
		// the same in the whole grid
		case 0x29:
		case 0x2a:
		case 0x2b:
			if (upper[op - 0x29] < READ_3_300290(node + 1))
			{
				return;
			}
			if (lower[op - 0x29] > READ_3_300290(node + 4))
			{
				return;
			}
			node += 7;
			continue;
		case 0x05:
			node += node[1] + 2;
			continue;
		case 0x06:
			node += READ_2_300290(node + 1) + 3;
			continue;
		case 0x07:
			node += READ_3_300290(node + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		{
			long const step = op;
			copy.corner[0] = (node[1] + range->corner[0]) << step;
			copy.corner[1] = (node[2] + range->corner[1]) << step;
			copy.corner[2] = (node[3] + range->corner[2]) << step;
			copy.depth = range->depth + step;
			long const shift = 0x10 - copy.depth;
			copy.lower[0] = (lower[0] >> shift) - copy.corner[0];
			copy.lower[1] = (lower[1] >> shift) - copy.corner[1];
			copy.lower[2] = (lower[2] >> shift) - copy.corner[2];
			copy.upper[0] = (upper[0] >> shift) + 1;
			copy.upper[0] -= copy.corner[0];
			copy.upper[1] = (upper[1] >> shift) + 1;
			copy.upper[1] -= copy.corner[1];
			copy.upper[2] = (upper[2] >> shift) + 1;
			copy.upper[2] -= copy.corner[2];
			copy.values[0] = range->values[0];
			copy.key_base = range->key_base;
			range = (cell_range_2ff8b0 const *)&copy;
			node += 4;
			continue;
		}
		case 0x09:
		{
			long const delta = node[1];
			if (range != &copy)
			{
				copy = *range;
				range = (cell_range_2ff8b0 const *)&copy;
			}
			copy.key_base += delta;
			node += 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_300290(node + 1);
			if (range != &copy)
			{
				copy = *range;
				range = (cell_range_2ff8b0 const *)&copy;
			}
			copy.key_base += delta;
			node += 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_300290(node + 1);
			if (range != &copy)
			{
				copy = *range;
				range = (cell_range_2ff8b0 const *)&copy;
			}
			copy.key_base = start;
			node += 5;
			continue;
		}

		// the value in the first of four slots (retail writes the slot the
		// command names, past the end for the other three)
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		{
			long const byte = node[1];
			node += 2;
			copy.values[op - 0x60] = byte;
			break;
		}
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			copy.values[op - 0x64] = READ_2_300290(node + 1);
			node += 3;
			break;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_300290(node + 1);
			node += 5;
			break;
		default:
			reporter_2ff8b0()->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)reporter_2ff8b0()->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		case 0x50:
			offset = node[1];
			goto found;
		case 0x51:
			offset = READ_2_300290(node + 1);
			goto found;
		case 0x52:
			offset = READ_3_300290(node + 1);
			goto found;
		case 0x53:
			offset = READ_4_300290(node + 1);
			goto found;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			offset = op - 0x30;
			goto found;
		}
		value = copy.values[0];
		if (range != &copy)
		{
			copy = *range;
			range = (cell_range_2ff8b0 const *)&copy;
		}
		copy.values[0] = value;
	}
found:
	((keys_300290 *)keys)->append(range->key_base + offset);
}

// the box's bounds in the world, widened by the tolerance, in the tree's
// fixed-point grid, and the walk of the tree from its root
// @retail 0x2fff90
void class_2fff90::function_2fff90(void const *tree_, const c_query_transform *matrix, const __m128 *extent, float tolerance, s_surface_key_array *keys_)
{
	keys = keys_;
	tree = (tree_300150 const *)tree_;
	__m128 const e = *extent;
	__m128 const magnitude_mask = *(__m128 const *)g_455940;
	__m128 const x = _mm_and_ps(_mm_mul_ps(_mm_shuffle_ps(e, e, 0x00), matrix->axes[0]), magnitude_mask);
	__m128 const y = _mm_and_ps(_mm_mul_ps(_mm_shuffle_ps(e, e, 0x55), matrix->axes[1]), magnitude_mask);
	__m128 const z = _mm_and_ps(_mm_mul_ps(_mm_shuffle_ps(e, e, 0xaa), matrix->axes[2]), magnitude_mask);
	__m128 const tolerances = _mm_set1_ps(tolerance);
	s_query_bounds bounds;
	bounds.maximum = _mm_add_ps(_mm_add_ps(_mm_add_ps(x, y), z), tolerances);
	bounds.minimum = _mm_xor_ps(bounds.maximum, *(__m128 const *)g_4124a0);
	bounds.maximum = _mm_add_ps(bounds.maximum, matrix->position);
	bounds.minimum = _mm_add_ps(bounds.minimum, matrix->position);
	float const *minimum = (float const *)&bounds.minimum;
	float const *maximum = (float const *)&bounds.maximum;
	lower[0] = round_300150((minimum[0] - tree->origin[0]) * tree->scale) - 1;
	upper[0] = round_300150((maximum[0] - tree->origin[0]) * tree->scale) - 1;
	lower[1] = round_300150((minimum[1] - tree->origin[1]) * tree->scale) - 1;
	upper[1] = round_300150((maximum[1] - tree->origin[1]) * tree->scale) - 1;
	lower[2] = round_300150((minimum[2] - tree->origin[2]) * tree->scale) - 1;
	upper[2] = round_300150((maximum[2] - tree->origin[2]) * tree->scale) - 1;
	cell_range_2ff8b0 range;
	range.lower[0] = lower[0] >> 16;
	range.upper[0] = (upper[0] >> 16) + 1;
	range.lower[1] = lower[1] >> 16;
	range.upper[1] = (upper[1] >> 16) + 1;
	range.lower[2] = lower[2] >> 16;
	range.upper[2] = (upper[2] >> 16) + 1;
	range.corner[0] = 0;
	range.corner[1] = 0;
	range.corner[2] = 0;
	range.key_base = 0;
	range.depth = 0;
	range.values[0] = 0;
	function_2ff8b0(&range, ((tree_300150 const *)tree_)->nodes);
}

// the bounds' corners in the tree's fixed-point grid, and the walk of the
// tree from its root
// @retail 0x300150
void class_300150::function_300150(void const *tree_, const s_query_bounds *bounds, s_surface_key_array *keys_)
{
	keys = keys_;
	tree = (tree_300150 const *)tree_;
	float const *minimum = (float const *)&bounds->minimum;
	float const *maximum = (float const *)&bounds->maximum;
	lower[0] = round_300150((minimum[0] - tree->origin[0]) * tree->scale) - 1;
	upper[0] = round_300150((maximum[0] - tree->origin[0]) * tree->scale) - 1;
	lower[1] = round_300150((minimum[1] - tree->origin[1]) * tree->scale) - 1;
	upper[1] = round_300150((maximum[1] - tree->origin[1]) * tree->scale) - 1;
	lower[2] = round_300150((minimum[2] - tree->origin[2]) * tree->scale) - 1;
	upper[2] = round_300150((maximum[2] - tree->origin[2]) * tree->scale) - 1;
	cell_range_2ff8b0 range;
	range.lower[0] = lower[0] >> 16;
	range.upper[0] = (upper[0] >> 16) + 1;
	range.lower[1] = lower[1] >> 16;
	range.upper[1] = (upper[1] >> 16) + 1;
	range.lower[2] = lower[2] >> 16;
	range.upper[2] = (upper[2] >> 16) + 1;
	range.corner[0] = 0;
	range.corner[1] = 0;
	range.corner[2] = 0;
	range.key_base = 0;
	range.depth = 0;
	range.values[0] = 0;
	function_2ff8b0(&range, ((tree_300150 const *)tree_)->nodes);
}

// a box of cells around the sphere, in whole cells of the current level: the
// sphere's centre from the cell's corner and its radius, the corner, the
// first key of the cell's subtree, how many bits of the fixed-point grid the
// level leaves, and a value the commands set
struct cell_box_300290
{
	long centre[3];
	long radius;
	long corner[3];
	long key_base;
	long shift;
	long values[1];
};

// a value rounded to the nearest integer by the FPU
__forceinline int round_3008d0(float value)
{
	int result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

// a tree's grid: its origin, its scale, and its nodes
struct tree_3008d0
{
	float origin[3];
	float scale;
	char unknown10[0x30 - 0x10];
	unsigned char nodes[1];
};

class class_3008d0
{
public:
	s_surface_key_array *keys;
	long unknown4[3];
	// the sphere's centre and radius in the tree's fixed-point grid
	long centre[3];
	long radius;

	void function_3008d0(void const *tree, const __m128 *sphere, s_surface_key_array *keys);
	void function_300290(cell_box_300290 const *box, unsigned char const *node);
};

// runs a tree's command stream for a sphere: commands narrow the box to a
// child cell, split the walk where the sphere spans both sides of a plane
// (the first side by recursion), cut it off where the sphere misses a slab,
// move the key base, or give a key to add to the list
// Matched (was 3): the command byte is kept as a byte and the switch value made from it;
// cases 0x26 and 0x29 name the radius after the position along the axis; and case 0x29's
// first three-byte read takes its address from a named pointer. With those values defined
// before it, the read gathers into the middle byte's register as in retail.
// @retail 0x300290
void class_3008d0::function_300290(cell_box_300290 const *box, unsigned char const *node)
{
	cell_box_300290 copy;
	long offset;
	long value;
	long position;
	long extent;
	long low;
	long high;
	for (;;)
	{
		unsigned char const code = *node;
		long const op = code;
		switch (op)
		{
		// planes across two axes or three, then one
		case 0x13:
			low = node[1] * 2;
			high = node[2] * 2;
			position = box->centre[1] + box->centre[2];
			extent = box->radius + (box->radius >> 1) + 1;
			goto split;
		case 0x14:
			low = node[1] * 2;
			high = node[2] * 2;
			position = box->centre[1] - box->centre[2] + 0xff;
			extent = box->radius + (box->radius >> 1) + 1;
			goto split;
		case 0x15:
			low = node[1] * 2;
			high = node[2] * 2;
			position = box->centre[0] + box->centre[2];
			extent = box->radius + (box->radius >> 1) + 1;
			goto split;
		case 0x16:
			low = node[1] * 2;
			high = node[2] * 2;
			position = box->centre[0] - box->centre[2] + 0xff;
			extent = box->radius + (box->radius >> 1) + 1;
			goto split;
		case 0x17:
			low = node[1] * 2;
			high = node[2] * 2;
			position = box->centre[0] + box->centre[1];
			extent = box->radius + (box->radius >> 1) + 1;
			goto split;
		case 0x18:
			low = node[1] * 2;
			high = node[2] * 2;
			position = box->centre[0] - box->centre[1] + 0xff;
			extent = box->radius + (box->radius >> 1) + 1;
			goto split;
		case 0x19:
			low = node[1] * 3;
			high = node[2] * 3;
			position = box->centre[2] + box->centre[1] + box->centre[0];
			extent = box->radius * 4;
			goto split;
		case 0x1a:
			low = node[1] * 3;
			high = node[2] * 3;
			position = box->centre[1] - box->centre[2] + box->centre[0] + 0xff;
			extent = box->radius * 4;
			goto split;
		case 0x1b:
			low = node[1] * 3;
			high = node[2] * 3;
			position = box->centre[2] - box->centre[1] + box->centre[0] + 0xff;
			extent = box->radius * 4;
			goto split;
		case 0x1c:
			low = node[1] * 3;
			high = node[2] * 3;
			position = box->centre[0] - box->centre[2] - box->centre[1] + 0x1fe;
			extent = box->radius * 4;
			goto split;

		case 0x10:
		case 0x11:
		case 0x12:
			low = node[1];
			high = node[2];
			position = box->centre[op - 0x10];
			extent = box->radius;
		// the sphere on the first side of the plane goes on there; on the
		// second, to the second side; on both, the first side by recursion
		split:
		{
			node += 4;
			if (position + extent < high)
			{
				continue;
			}
			long const skip = node[-1];
			node += skip;
			if (position > low + extent)
			{
				continue;
			}
			function_300290(box, node - skip);
			continue;
		}

		// one plane across an axis, the second side's start given
		case 0x20:
		case 0x21:
		case 0x22:
		{
			long const plane = node[1];
			long const reach = box->radius;
			position = box->centre[op - 0x20];
			node += 3;
			if (position + reach < plane)
			{
				continue;
			}
			long const skip = node[-1];
			node += skip;
			if (position > reach + plane + 1)
			{
				continue;
			}
			function_300290(box, node - skip);
			continue;
		}
		// two planes across an axis, both sides' starts given
		case 0x23:
		case 0x24:
		case 0x25:
		{
			long const second_low = node[1];
			long const first_high = node[2];
			long const along = box->centre[op - 0x23];
			long const reach = box->radius;
			long const first_skip = READ_2_300290(node + 3);
			node += 7;
			if (along + reach < first_high)
			{
				node += first_skip;
				continue;
			}
			long const second_skip = READ_2_300290(node - 2);
			node += second_skip;
			if (along > reach + second_low)
			{
				continue;
			}
			function_300290(box, node - second_skip + first_skip);
			continue;
		}
		// a slab across an axis the sphere must touch
		case 0x26:
		case 0x27:
		case 0x28:
		{
			long const along = box->centre[op - 0x26];
			long const reach = box->radius;
			if (along + reach < node[1])
			{
				return;
			}
			if (along > node[2] + reach)
			{
				return;
			}
			node += 3;
			continue;
		}
		// the same in the whole grid
		case 0x29:
		case 0x2a:
		case 0x2b:
		{
			long const along = centre[op - 0x29];
			long const reach = radius;
			unsigned char const *const first = node + 1;
			if (reach + along < READ_3_300290(first))
			{
				return;
			}
			if (along > READ_3_300290(node + 4) + reach)
			{
				return;
			}
			node += 7;
			continue;
		}
		case 0x05:
			node += node[1] + 2;
			continue;
		case 0x06:
			node += READ_2_300290(node + 1) + 3;
			continue;
		case 0x07:
			node += READ_3_300290(node + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		{
			long const shift = op;
			copy.shift = box->shift - shift;
			copy.corner[0] = (node[1] + box->corner[0]) << shift;
			copy.centre[0] = (centre[0] >> copy.shift) - copy.corner[0];
			copy.corner[1] = (node[2] + box->corner[1]) << shift;
			copy.centre[1] = (centre[1] >> copy.shift) - copy.corner[1];
			copy.corner[2] = (node[3] + box->corner[2]) << shift;
			copy.centre[2] = (centre[2] >> copy.shift) - copy.corner[2];
			copy.radius = (radius >> copy.shift) + 1;
			copy.key_base = box->key_base;
			copy.values[0] = box->values[0];
			box = &copy;
			node += 4;
			continue;
		}
		case 0x09:
		{
			long const delta = node[1];
			if (box != &copy)
			{
				copy = *box;
				box = &copy;
			}
			copy.key_base += delta;
			node += 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_300290(node + 1);
			if (box != &copy)
			{
				copy = *box;
				box = &copy;
			}
			copy.key_base += delta;
			node += 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_300290(node + 1);
			if (box != &copy)
			{
				copy = *box;
				box = &copy;
			}
			copy.key_base = start;
			node += 5;
			continue;
		}

		// the value in the first of four slots (retail writes the slot the
		// command names, past the end for the other three)
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		{
			long const byte = node[1];
			node += 2;
			copy.values[op - 0x60] = byte;
			break;
		}
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			copy.values[op - 0x64] = READ_2_300290(node + 1);
			node += 3;
			break;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_300290(node + 1);
			node += 5;
			break;
		default:
			reporter_2ff8b0()->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)reporter_2ff8b0()->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		case 0x50:
			offset = node[1];
			goto found;
		case 0x51:
			offset = READ_2_300290(node + 1);
			goto found;
		case 0x52:
			offset = READ_3_300290(node + 1);
			goto found;
		case 0x53:
			offset = READ_4_300290(node + 1);
			goto found;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			offset = op - 0x30;
			goto found;
		}
		value = copy.values[0];
		if (box != &copy)
		{
			copy = *box;
			box = &copy;
		}
		copy.values[0] = value;
	}
found:
	((keys_300290 *)keys)->append(box->key_base + offset);
}

// the sphere's centre and radius in the tree's fixed-point grid, and the
// walk of the tree from its root
// score 0 (was 65): matched with the node array bound by a constant reference
// to the tree's member (retail forms its address early in esi), the keys stored
// after it, and the box filled in field order.
// Before, 65 (was 65): Frame 0x28 and extent 213 agree; first +4 exchanges
// argument registers, followed by different saved registers and spill homes.
// Tree view after publication 65; direct vector lanes 65; box declared first 65;
// command pointer renamed 65; direct command argument 65; const helper input
// 65; helper result renamed 65; coordinate stores together 65.
// NOT kept: none improves the original body.
// Indexed coordinate loop 87; scale captured once 66; vector copied once 84;
// ordinary conversion inline 84; long conversion result 65; chained clearing 65;
// aggregate zero initialization 80; mutable node pointer 65.
// NOT kept: all eight forms tie or regress; original executable text restored.
// Round 426: float input renamed, shared conversion call, complete scaled
// expressions, immutable tree pointer, tree reference, root-array reference,
// earlier root address and separate radius increment all 65.
// NOT kept: no improvement; original helper and body restored.
// @retail 0x3008d0
void class_3008d0::function_3008d0(void const *tree_, const __m128 *sphere, s_surface_key_array *keys_)
{
	tree_3008d0 const *tree = (tree_3008d0 const *)tree_;
	unsigned char const *const &nodes = tree->nodes;
	keys = keys_;
	float const *sphere_centre = (float const *)sphere;
	centre[0] = round_3008d0((sphere_centre[0] - tree->origin[0]) * tree->scale) - 1;
	centre[1] = round_3008d0((sphere_centre[1] - tree->origin[1]) * tree->scale) - 1;
	centre[2] = round_3008d0((sphere_centre[2] - tree->origin[2]) * tree->scale) - 1;
	radius = round_3008d0(sphere_centre[3] * tree->scale);
	cell_box_300290 box;
	box.centre[0] = centre[0] >> 16;
	box.centre[1] = centre[1] >> 16;
	box.centre[2] = centre[2] >> 16;
	box.radius = (radius >> 16) + 1;
	box.corner[0] = 0;
	box.corner[1] = 0;
	box.corner[2] = 0;
	box.key_base = 0;
	box.values[0] = 0;
	box.shift = 0x10;
	function_300290(&box, nodes);
}

struct class_3009b0
{
    char unknown0[0x24];
    long field_24;

    void function_3009b0();
};

// @retail 0x3009b0
void class_3009b0::function_3009b0()
{
    field_24++;
}
