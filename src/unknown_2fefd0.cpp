// @flags /O2 /GL-
// Havok (third-party physics library): starting a pair's search state: the
// first object's support vertex towards the second's centre, and the
// second's support vertex back towards that vertex, in the second's frame.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"
#include "stream_holder_2d8e70.h"

// the sign bit, in four lanes
__declspec(align(16)) static const unsigned long sign_mask_4124a0[4] = {
	0x80000000, 0x80000000, 0x80000000, 0x80000000
};

struct c_query_transform
{
	__m128 axes[3];
	__m128 position;
};

struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};

struct vector_2da6e0
{
	__m128 value;
	void inverse_rotate(const c_query_transform *matrix, const __m128 *direction);
};

// a point whose fourth lane holds the vertex's id in its low half
union support_2fefd0
{
	__m128 point;
	struct
	{
		float xyz[3];
		unsigned short id;
	} vertex;
};

class convex_2fefd0
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
	virtual void support(__m128 const *direction, support_2fefd0 *out) const {}
	virtual void slot_13() {}
	virtual void centre(__m128 *out) const {}
	virtual bool slot_15() const { return false; }
};

// a triangle: three corners after a 16-byte header
struct triangle_2ff0c0
{
	char unknown0[0x10];
	__m128 corners[3];
};

// a point placed by a transform
struct placed_point_2ff0c0
{
	__m128 value;

	void set_transformed(c_query_transform const *transform, __m128 const *point)
	{
		__m128 p = *point;
		__m128 z = _mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->position);
		__m128 y = _mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55));
		__m128 x = _mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00));
		value = _mm_add_ps(_mm_add_ps(x, y), z);
	}
};

// the cross product of two vectors' first three terms
inline __m128 cross_2ff0c0(__m128 const &a, __m128 const &b)
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9)));
}

struct pair_state_2fefd0
{
	unsigned short first_id;
	unsigned short second_id;
	unsigned short field_4;
	unsigned short field_6;
	bool field_8;
	unsigned char field_9;
	bool first_flag;
	unsigned char second_flag;

	void start(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform);
	void function_2ff0c0(convex_2fefd0 const *first, triangle_2ff0c0 const *triangle, c_query_transform const *transform);
};

// @retail 0x2fefd0
void pair_state_2fefd0::start(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform)
{
	__m128 first_centre;
	first->centre(&first_centre);
	__m128 second_centre;
	second->centre(&second_centre);
	c_transformed_point second_world;
	second_world.transform(transform, &second_centre);
	__m128 direction = _mm_sub_ps(second_world.value, first_centre);
	support_2fefd0 first_vertex;
	first->support(&direction, &first_vertex);
	first_id = first_vertex.vertex.id;
	direction = _mm_sub_ps(second_world.value, first_vertex.point);
	vector_2da6e0 local;
	local.inverse_rotate(transform, &direction);
	local.value = _mm_xor_ps(local.value, *(__m128 const *)sign_mask_4124a0);
	support_2fefd0 second_vertex;
	second->support(&local.value, &second_vertex);
	second_id = second_vertex.vertex.id;
	field_4 = 0xffff;
	field_6 = 0xffff;
	field_8 = true;
	field_9 = true;
	first_flag = first->slot_15();
	second_flag = second->slot_15();
}

// starting the search state of an object and a triangle: the triangle's
// corners placed by the transform, its normal turned to face away from the
// object's centre, and the object's support vertex against that normal
// @retail 0x2ff0c0
void pair_state_2fefd0::function_2ff0c0(convex_2fefd0 const *first, triangle_2ff0c0 const *triangle, c_query_transform const *transform)
{
	second_id = 0;
	field_4 = 0x10;
	field_6 = 0x20;
	placed_point_2ff0c0 corners[3];
	__m128 const *corner = triangle->corners;
	placed_point_2ff0c0 *placed = corners;
	for (int count = 3; count != 0; count--)
	{
		placed->set_transformed(transform, corner);
		placed++;
		corner++;
	}
	__m128 const ab = _mm_sub_ps(corners[1].value, corners[0].value);
	__m128 const cb = _mm_sub_ps(corners[2].value, corners[1].value);
	__m128 normal = cross_2ff0c0(ab, cb);

	__m128 centre;
	first->centre(&centre);
	__m128 const product = _mm_mul_ps(_mm_sub_ps(centre, corners[0].value), normal);
	if (_mm_movemask_ps(_mm_add_ss(_mm_shuffle_ps(product, product, 0xaa), _mm_add_ss(_mm_shuffle_ps(product, product, 0x55), product))) & 1)
	{
		normal = _mm_xor_ps(normal, *(__m128 const *)sign_mask_4124a0);
	}
	support_2fefd0 vertex;
	first->support(&normal, &vertex);
	first_id = vertex.vertex.id;
	field_8 = true;
	field_9 = 3;
	first_flag = first->slot_15();
	second_flag = 3;
}

// what reports a stream's errors: its second slot gives the stream to write
// the message to, which also ends the report
class reporter_2ff210
{
public:
	virtual void slot_0() {}
	virtual stream_holder_2d8e70 *slot_1() { return 0; }
};

reporter_2ff210 *g_48017c;

struct class_2d9060
{
	long function_2d9060();
};

// where a command stream stands: an offset, and a value the commands set
struct position_2ff210
{
	long offset;
	long values[1];
};

ELEMENTS_2D91D0(offsets_2ff210, long);

// the position as a copy that may be changed: the caller's at first
__forceinline position_2ff210 *writable_2ff210(position_2ff210 const *&position, position_2ff210 &copy)
{
	if (position != &copy)
	{
		copy = *position;
		position = &copy;
	}
	return &copy;
}

// a big-endian number of two, three or four bytes
#define READ_2_2FF210(p) (((p)[0] << 8) + (p)[1])
#define READ_3_2FF210(p) (((((p)[0] << 8) + (p)[1]) << 8) + (p)[2])
#define READ_4_2FF210(p) (((((((p)[0] << 8) + (p)[1]) << 8) + (p)[2]) << 8) + (p)[3])

class offset_list_2ff210
{
public:
	offsets_2ff210 *offsets;

	void function_2ff210(position_2ff210 const *position, unsigned char const *commands);
	void function_2ff570(position_2ff210 const *position, unsigned char const *commands);
};

// runs a command stream until a command gives an offset, and adds the
// position's offset plus that one to the list; commands before it skip their
// operands or move the position
// score 78 (was 161): one structured trailing update fixes the entry registers.
// Earlier: one shared value tail fixes the entry registers.
// The byte-value store and four-byte offset read still differ; the tables
// begin four bytes early. Earlier note 166 (was 166): retail reads the command's byte before it pushes the
// registers and into another register, and the jump table and the table of
// command lengths sit at other addresses than retail's. (The fast check's
// number moves with the reduced image it builds — 150, 169, 179, 182 for the
// same source; the full check's 166 stands.)
// Shared tail 78; word selector 190; one operand 161; cursor first after
// sharing 109; named halves 78; copy pointer 78; copy reference 78;
// value groups first 198.
// NOT kept (coordinator review, Codex P409 11-12): 161 -> 78 came from new `goto record_value` jumps sharing one case tail; a new goto is kept only with a byte match. The body is main's again.
// Nested switch 243; ascending widths 207; descending widths 184;
// continue cases with one trailing update 78; two-word offset read on that form 78;
// byte advance first 109; value declared at trailing update 78; next slot 91.
// NOT kept: next-slot growth changes the later count/store sequence.
// @retail 0x2ff210
void offset_list_2ff210::function_2ff210(position_2ff210 const *position, unsigned char const *commands)
{
	position_2ff210 copy;
	long offset;
	long value;
	for (;;)
	{
		unsigned char const *p = commands;
		long const op = *p;
		switch (op)
		{
		case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16:
		case 0x17: case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c:
			commands = p + p[3] + 4;
			continue;
		case 0x20:
		case 0x21:
		case 0x22:
			commands = p + p[2] + 3;
			continue;
		case 0x23:
		case 0x24:
		case 0x25:
			commands = p + READ_2_2FF210(p + 5) + 7;
			continue;
		case 0x26:
		case 0x27:
		case 0x28:
			commands = p + 3;
			continue;
		case 0x29:
		case 0x2a:
		case 0x2b:
			commands = p + 7;
			continue;
		case 0x05:
			commands = p + p[1] + 2;
			continue;
		case 0x06:
			commands = p + READ_2_2FF210(p + 1) + 3;
			continue;
		case 0x07:
			commands = p + READ_3_2FF210(p + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
			commands = p + 4;
			continue;
		case 0x09:
		{
			long const delta = p[1];
			writable_2ff210(position, copy)->offset += delta;
			commands = p + 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_2FF210(p + 1);
			writable_2ff210(position, copy)->offset += delta;
			commands = p + 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_2FF210(p + 1);
			writable_2ff210(position, copy)->offset = start;
			commands = p + 5;
			continue;
		}
		// the value in the first of four slots (retail writes the slot the
		// command names, past the end for the other three)
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
			copy.values[op - 0x60] = p[1];
			commands = p + 2;
			break;
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			copy.values[op - 0x64] = READ_2_2FF210(p + 1);
			commands = p + 3;
			break;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_2FF210(p + 1);
			commands = p + 5;
			break;
		default:
			g_48017c->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)g_48017c->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		case 0x50:
			offset = p[1];
			goto found;
		case 0x51:
			offset = READ_2_2FF210(p + 1);
			goto found;
		case 0x52:
			offset = READ_3_2FF210(p + 1);
			goto found;
		case 0x53:
			offset = READ_4_2FF210(p + 1);
			goto found;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			offset = op - 0x30;
			goto found;
		}
		value = copy.values[0];
		writable_2ff210(position, copy)->values[0] = value;
	}
found:
	offsets->append(position->offset + offset);
}

// the same for a stream whose commands 0x10 to 0x2b have other operands
// score 12 (was 92): the byte store and final four-byte sum still differ.
// Earlier: one shared value tail fixes the entry registers.
// The byte-value store and four-byte offset read still differ. Earlier body:
// retail reads the command's byte before it pushes the
// registers and into another register, and the table of command lengths sits
// at another offset within the second argument.
// Shared tail 12; word selector 221; one operand 92; cursor first after
// sharing 86; named halves 12; copy pointer 12; copy reference 12;
// value groups first 164.
// NOT kept (coordinator review, Codex P409 11-12): 92 -> 12 came from new `goto record_value` jumps sharing one case tail; a new goto is kept only with a byte match. The body is main's again.
// Nested switch 223; ascending widths 204; descending widths 182;
// continue cases with one trailing update 12; two-word offset read on that form 12;
// byte advance first 86; value declared at trailing update 12; next slot 46.
// NOT kept: next-slot growth changes the later count/store sequence.
// @retail 0x2ff570
void offset_list_2ff210::function_2ff570(position_2ff210 const *position, unsigned char const *commands)
{
	position_2ff210 copy;
	long offset;
	long value;
	for (;;)
	{
		unsigned char const *p = commands;
		long const op = *p;
		switch (op)
		{
		case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16:
		case 0x17: case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c:
			commands = p + 4;
			continue;
		case 0x20:
		case 0x21:
		case 0x22:
			commands = p + 3;
			continue;
		case 0x23:
		case 0x24:
		case 0x25:
			commands = p + READ_2_2FF210(p + 3) + 7;
			continue;
		case 0x26:
		case 0x27:
		case 0x28:
			commands = p + 3;
			continue;
		case 0x29:
		case 0x2a:
		case 0x2b:
			commands = p + 7;
			continue;
		case 0x05:
			commands = p + p[1] + 2;
			continue;
		case 0x06:
			commands = p + READ_2_2FF210(p + 1) + 3;
			continue;
		case 0x07:
			commands = p + READ_3_2FF210(p + 1) + 4;
			continue;
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
			commands = p + 4;
			continue;
		case 0x09:
		{
			long const delta = p[1];
			writable_2ff210(position, copy)->offset += delta;
			commands = p + 2;
			continue;
		}
		case 0x0a:
		{
			long const delta = READ_2_2FF210(p + 1);
			writable_2ff210(position, copy)->offset += delta;
			commands = p + 3;
			continue;
		}
		case 0x0b:
		{
			long const start = READ_4_2FF210(p + 1);
			writable_2ff210(position, copy)->offset = start;
			commands = p + 5;
			continue;
		}
		// the value in the first of four slots (retail writes the slot the
		// command names, past the end for the other three)
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
			copy.values[op - 0x60] = p[1];
			commands = p + 2;
			break;
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			copy.values[op - 0x64] = READ_2_2FF210(p + 1);
			commands = p + 3;
			break;
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
			copy.values[op - 0x68] = READ_4_2FF210(p + 1);
			commands = p + 5;
			break;
		default:
			g_48017c->slot_1()->function_2d9000("unknown command %i\n", op);
			((class_2d9060 *)g_48017c->slot_1())->function_2d9060();
			*(long *)0 = 0;
			return;
		case 0x00:
			return;
		case 0x50:
			offset = p[1];
			goto found;
		case 0x51:
			offset = READ_2_2FF210(p + 1);
			goto found;
		case 0x52:
			offset = READ_3_2FF210(p + 1);
			goto found;
		case 0x53:
			offset = READ_4_2FF210(p + 1);
			goto found;
		case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
		case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
			offset = op - 0x30;
			goto found;
		}
		value = copy.values[0];
		writable_2ff210(position, copy)->values[0] = value;
	}
found:
	offsets->append(position->offset + offset);
}
