// @flags /O2 /GL-
// Havok (third-party physics library): allocating face records and extending a hull.

#include <xmmintrin.h>
#include "reference_counted_2d88a0.h"

struct record_30a950
{
	char bytes[0x50];
};

struct list_30a950
{
	record_30a950 **data;
	long size;
};

class pool_30a950
{
public:
	long field_0;
	long field_4;
	record_30a950 **free_records;
	long free_count;
	char pad_10[0x1a0];
	record_30a950 *records;
	long used_count;

	void function_30a950(list_30a950 *out, long count);
};

// @retail 0x30a950
void pool_30a950::function_30a950(list_30a950 *out, long count)
{
	long i = free_count - 1;
	while (count != 0)
	{
		if (i < 0)
		{
			break;
		}
		out->data[out->size] = free_records[i];
		out->size++;
		free_count--;
		count--;
		i--;
	}
	while (count > 0)
	{
		record_30a950 *block = records;
		long index = used_count;
		used_count = index + 1;
		out->data[out->size] = &block[index];
		out->size++;
		count--;
	}
}


struct edge_30aac0;
struct face_30aac0;

struct vertex_30ac50
{
	__m128 position;
	edge_30aac0 *horizon_edge;
	char unknown14[0xc];
};

struct face_list_30ac50
{
	face_30aac0 **faces;
	int count;
};

class class_30a810
{
public:
	char unknown0[0x1b0];
	face_30aac0 *faces;
	int face_count;
	char unknown1b8[0x2100 - 0x1b8];
	vertex_30ac50 *vertices;
	int vertex_count;

	int function_30ac50(face_list_30ac50 const *new_faces, void const *unused, edge_30aac0 *horizon, vertex_30ac50 const *const *apex);
	int function_30ae70(long count, edge_30aac0 *horizon, __m128 const *position);
};

// a list of faces that starts out in the room for 100 held inside it
struct new_faces_30ae70
{
	face_30aac0 **data;
	int size;
	long capacity_and_flags;
	face_30aac0 *embedded[100];

	new_faces_30ae70()
	{
		data = embedded;
		size = 0;
		capacity_and_flags = 0x80000064;
	}
	~new_faces_30ae70()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(face_30aac0 *), 0x12);
		}
	}
};

// adds a vertex at the position, and a fan of new faces from it over the
// horizon; the object starts with the pool the faces come from
// score 0 (was 4): matched with the vertex written through a reference, the
// apex pointer taken after the writes, and the face list and both calls in their
// own block.
// Before, 4 (was 59): the apex pointer slot is stored before the vertex writes,
// rather than after the stack array size is cleared; the remaining code agrees.
// Earlier at 59: retail keeps the object in ecx across the call to the
// pool and reloads it for neither call, ours copies the object into esi and
// reloads ecx before both calls, which is eight bytes more
// Trials: placement after allocator 46; edge first 50; position snapshot 57;
// element reference 11; constructor store order 11; copy before clear 3;
// intrinsic load 61; earlier array 23; intrinsic store 3; initializer list 3;
// direct counter 48; split increment 3; long index 3; 100 permutations, best 3.
// Call-only apex forms at 3 removed; apex edge write 5; reference from apex 4;
// that reference with edge first 14; constructor zero first 4.
// With the multi-use pointer: aligned load 61; load plus store 61; array
// between writes 28; split count 4; outer result 4; member initializers 4;
// external base reference 4; long index 4.
// Const pointer, input reference and captured array base 4; intrinsic
// write on this reference 5; input reference with pointer writes 46; array
// before pointer 24; named result and unsigned flags 4. Body retained.
// Further forms: pointer after array construction 23; pointer, array and
// element-reference renames 4 each; early pointer declaration 4; field
// reference 5; unsigned ordinal and input-pointer copy 4.
// More forms: separate mutable/const pointers 3; their aligned store 3;
// base header 4; member header 4; base with split pointers 3;
// indexed vector write 46; indexed edge clear 51; unaligned read 61;
// edge-first split pointers 11; memcpy 73. Opening body retained.
// @retail 0x30ae70
int class_30a810::function_30ae70(long count, edge_30aac0 *horizon, __m128 const *position)
{
	int index = vertex_count++;
	vertex_30ac50 &vertex = vertices[index];
	vertex.position = *position;
	vertex.horizon_edge = 0;
	vertex_30ac50 const *apex = &vertex;
	{
		new_faces_30ae70 new_faces;
		((pool_30a950 *)this)->function_30a950((list_30a950 *)&new_faces, count);
		return function_30ac50((face_list_30ac50 const *)&new_faces, (void const *)count, horizon, &apex);
	}
}
