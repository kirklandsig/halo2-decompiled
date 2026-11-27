// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"
#include "material_records_storage.h"
#include "light_visibility_storage.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "instance_marks_storage.h"
#include "light_marks_storage.h"
/* UNKNOWN_1321F0.CPP */

#include "unknown_11c920.h"
#include "visibility_storage.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "object_queries.h"
#include <math.h>
#include <string.h>
#include <xtl.h>

#define k_pi 3.14159265359f

struct s_1321f0
{
	byte unknown000[0x21c];
	dword flags[1];
};

// @retail 0x1321f0
bool function_1321f0(long index, s_1321f0 const *data)
{
	return (data->flags[index >> 5] & (1 << (index & 31))) != 0;
}

struct s_sort_entry
{
	short value;
	byte unknown02[0x1a - 2];
};

struct s_sort_context
{
	byte unknown000[0xa6c];
	short count;
	s_sort_entry entries[128];
	byte unknown176e[2];
	long order[0x200];
};

// @retail 0x134950
bool __stdcall function_134950(long a, long b, void const *context)
{
	s_sort_context const *data = (s_sort_context const *)context;
	return data->entries[a].value > data->entries[b].value;
}

struct s_132d60
{
	s_sort_context *context;
	byte unknown04[4];
	short cluster_index;
	byte unknown0a[0x2a68 - 0xa];
	point3f point;
	real radius;
};

#include "cluster_marks_storage.h"
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, short maximum_count, short *clusters);
void sort_4byte(void *unused, long *elements, unsigned long count, bool (__stdcall *compare)(long, long, const void *), const void *context);

// @retail 0x132d60
void function_132d60(s_132d60 *data)
{
	real radius = data->radius;
	short clusters[0x200];
	short count = 0;

	if (data->cluster_index != NONE)
	{
		if (radius > 0.0f)
		{
			g_4e7414++;
			g_4e7411 = true;
			count = function_14a5b0(data->cluster_index, &data->point, radius, 0x200, clusters);
			g_4e7411 = false;
		}
		else
		{
			count = 1;
			clusters[0] = data->cluster_index;
		}
		if (count > 0x200)
		{
			count = 0x200;
		}
	}

	data->context->count = count;
	for (long i = 0; i < data->context->count; i++)
	{
		s_sort_entry *entry = &data->context->entries[i];
		entry->value = clusters[i];
		*(short *)&entry->unknown02[0] = 0;
		data->context->order[i] = i;
	}
	sort_4byte(&radius, data->context->order, data->context->count, function_134950, data->context);
}

/* ---- bit vector pools ---- */

/* the entry list of unknown_163110.cpp (its constructor and swap are there) */
class c_entry_list
{
public:
	c_entry_list(long maximum_count);
	~c_entry_list()
	{
		delete[] shorts_b;
		shorts_b = NULL;
		delete[] longs_a;
		longs_a = NULL;
		delete[] longs_c;
		longs_c = NULL;
		delete[] shorts_d;
		shorts_d = NULL;
	}
	void swap(short index0, short index1);
	bool add(long a, short b, long c, short d);
	short find(long a);

	long maximum_count;
	short count;
	short *shorts_b;
	long *longs_a;
	long *longs_c;
	short *shorts_d;
};





/* the records are 0x9c bytes each */
#define k_bit_vector_pool_record_size 0x9c

struct s_frustum_set_view;
bool function_165010(s_frustum_set_view const *set, long section_index, point3f const *center, real radius, bool *contained);
long function_461c0(point3f const *a, point3f const *b, real radius);
bool function_c3110(long index);
bool function_c3140(long index);
extern s_record_pool *g_4e030c;

struct s_132a30_light
{
	byte unknown00[0xc];
	long stamp;
	byte unknown10[8];
	point3f center;
	real radius;
	byte unknown28[0x48 - 0x28];
	short unknown48;
	byte unknown4a[0x110 - 0x4a];
};

// @retail 0x132a30
void function_132a30(s_bit_vector_pool *data, long index, long section_index, bool visible)
{
	if (function_c3140(index))
	{
		s_132a30_light *light = &((s_132a30_light *)g_4e030c->data)[index & 0xffff];
		if (((s_132a30_light *)g_4e030c->data)[index & 0xffff].stamp != g_4e0308)
		{
			if (!visible)
			{
				visible = false;
				point3f center = light->center;
				real radius = light->radius;
				char intersects = true;
				if (data->query_type == 0)
					intersects = function_165010((s_frustum_set_view *)data->context, section_index, &center, radius, &visible);
				else if (data->query_type == 1)
					intersects = (char)function_461c0(&data->center, &center, data->radius + radius);
				if (!(char)intersects && ((char)data->flags2a60 >> 7) == 0)
					return;
			}
			if (data->lists[1]->add(index, 0, 0, NONE))
				function_c3110(index);
		}
	}
}

PRIVATE __forceinline real function_132e61(point3f const *arg_1, point3f const *arg_2)
{
	double local_1 = (double)arg_1->x - arg_2->x;
	double local_2 = (double)arg_1->y - arg_2->y;
	double local_3 = (double)arg_1->z - arg_2->z;
	return sqrt(local_3 * local_3 + local_2 * local_2 + local_1 * local_1);
}

// @retail 0x132e60
bool function_132e60(s_bit_vector_pool *data, point3f const *center, real radius,
	c_entry_list *list, long index, short section, bool visible, bool *contained, bool *first, bool *second)
{
	bool result = true;
	*first = true;
	*second = true;
	*contained = false;
	if (data->query_type == 0)
	{
		if (!visible)
			result = function_165010((s_frustum_set_view *)data->context, section, center, radius, contained);
		else
			result = true;
		if (result && !*contained && data->mode == 0 && radius <= 2.5f)
			*contained = true;
	}
	else if (data->query_type == 1)
	{
		real sum = radius + data->radius;
		vector3f delta;
		vector3d_from_points3d(&data->center, center, &delta);
		result = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k <= sum * sum;
		if (result)
			*contained = function_132e61(&data->center, center) + radius <= data->radius;
	}
	if (result && list && (data->flags2a60 & 1))
	{
		bool found = list->find(index) != NONE;
		*second = found;
		*first = found;
	}
	return result;
}

struct s_132b80_frustum
{
	byte unknown00[0x74];
	plane3f plane;
	byte unknown84[0x1bc - 0x84];
};

void function_11bed0(s_location *location, point3f const *point);

// @retail 0x132b80
long function_132b80(s_bit_vector_pool *data, long mode, s_132b80_frustum const *frusta,
	long cluster, long count, dword flags, void *filter, point3f const *center, real radius,
	bool use_plane, real plane_offset)
{
	s_location *location = &data->location;
	location->cluster_index = cluster;
	location->bsp_index = g_4686c4;
	location->leaf_index = NONE;
	data->mode = mode;
	data->flags2a60 = flags;
	data->frustum_count = (short)count;
	data->center = *center;
	data->radius = radius;
	data->plane_offset = plane_offset;
	if (use_plane)
		data->flags2a60 |= 0x200;
	else
		data->flags2a60 &= ~0x200;
	if (use_plane)
	{
		data->plane = frusta->plane;
		data->plane.d += plane_offset;
	}
	void *destination = (byte *)data->context + 4;
	if (frusta != destination)
		memcpy(destination, frusta, count * sizeof(s_132b80_frustum));
	*(short *)data->context = (short)count;
	if (cluster == NONE)
		function_11bed0(location, center);
	data->filter = filter;
	if (filter)
	{
		data->flags2a60 |= 1;
		/* the counters are one total and two rows of six (see s_132780) */
		data->counters[0] = 0;
		for (long i = 0; i < 6; i++)
			data->counters[1 + i] = 0;
		for (long i = 0; i < 6; i++)
			data->counters[7 + i] = 0;
	}
	if (data->mode == 0 && (data->flags2a60 & 0x80))
		data->query_type = 2;
	else if (count && (!(3.0f > radius) || (data->flags2a60 & 0x100)))
		data->query_type = 0;
	else
		data->query_type = 1;
	return data->query_type;
}

/* takes enough dwords from the pool for a bit vector of this many bits */
// @retail 0x1332b0
dword *function_1332b0(long bit_count, s_bit_vector_pool *data)
{
	dword *result = NULL;
	if (bit_count > 0)
	{
		long count = (bit_count + 31) >> 5;
		result = data->pool;
		if (data->pool_used + count < 0x200)
		{
			result = &data->pool[data->pool_used];
			data->pool_used += count;
		}
	}
	return result;
}

// @retail 0x134300
long function_134300(s_bit_vector_pool *data, short bit)
{
	long index = data->entry_count++;
	if (index >= 0 && index < 0x200)
	{
		data->entries[index][3] = 0;
		data->entries[index][2] = 0;
		data->entries[index][1] = 0;
		data->entries[index][0] = 0;
		data->entries[(short)index][bit >> 5] |= 1 << (bit & 31);
		return index;
	}
	data->entry_count = 0x200;
	return 0;
}

// @retail 0x1348e0
void function_1348e0(s_bit_vector_pool *data)
{
	data->lists[0]->count = 0;
	data->lists[1]->count = 0;
	data->lists[2]->count = 0;
	data->lists[3]->count = 0;
	memset(data->flags, 0, sizeof(data->flags));
	memset(data->pool, 0, data->pool_used * sizeof(dword));
	data->pool_used = 0;
	data->entry_count = 0;
	for (long i = 0; i < 0x80; i++)
	{
		data->indices[i] = NONE;
	}
}

/* a bit field from seven flags */
// @retail 0x132b10
dword function_132b10(bool d, bool c, bool f, bool g, bool e, bool a, bool b)
{
	return (b ? 0x4000 : 0) | (c ? 0x2000 : 0) | (d ? 0x1000 : 0) | (e ? 0x800 : 0) | (f ? 0x400 : 0) | (g ? 0x200 : 0) | (a ? 0x8000 : 0);
}

/* a 16 bit real: sign, ten bits of mantissa, then five of exponent */
// @retail 0x135880
inline real function_135880(word value)
{
	union s_135881
	{
		long field_0;
		real field_00;
	};
	long local_2 = value >> 15;
	local_2 = (local_2 << 8) | ((value & 0x1f) + 0x70);
	local_2 = (local_2 << 10) | ((value >> 5) & 0x3ff);
	s_135881 local_1;
	local_1.field_0 = local_2 << 13;
	return local_1.field_00;
}
// @retail 0x134980
void function_134980(s_bit_vector_pool *data, s_bit_vector_pool_sizes const *sizes, void *context)
{
	data->context = context;
	data->sizes = *sizes;
	data->lists[0] = new c_entry_list(data->sizes.list_sizes[0]);
	data->lists[2] = new c_entry_list(data->sizes.list_sizes[2]);
	data->lists[3] = new c_entry_list(data->sizes.list_sizes[3]);
	data->lists[1] = NULL;
	if (data->sizes.list_sizes[1])
	{
		data->lists[1] = new c_entry_list(data->sizes.list_sizes[1]);
	}
	data->pool_used = 0;
	data->entry_count = 0;
	data->records = NULL;
	if (data->sizes.record_count)
	{
		data->records = (byte *)operator new(data->sizes.record_count * k_bit_vector_pool_record_size);
	}
}

// @retail 0x134b20
void function_134b20(s_bit_vector_pool *data)
{
	delete data->lists[0];
	data->lists[0] = NULL;
	delete data->lists[1];
	data->lists[1] = NULL;
	delete data->lists[2];
	data->lists[2] = NULL;
	delete data->lists[3];
	data->lists[3] = NULL;
	operator delete(data->records);
	data->records = NULL;
}

/* the two bit vector pools, each with a scratch buffer carved from one
   virtual allocation (ai.cpp borrows the same buffer) */
extern byte *g_510c44;

// @retail 0x131ff0
void function_131ff0(void)
{
	s_bit_vector_pool_sizes sizes0;
	s_bit_vector_pool_sizes sizes1;
	byte *buffer;

	sizes0.unknown0 = 1;
	sizes0.list_sizes[0] = 0x80;
	sizes0.list_sizes[1] = 0x40;
	sizes0.list_sizes[2] = 0x100;
	sizes0.list_sizes[3] = 0x180;
	sizes0.record_count = 0;
	sizes1.unknown0 = 6;
	sizes1.list_sizes[0] = 0x40;
	sizes1.list_sizes[1] = 1;
	sizes1.list_sizes[2] = 0x80;
	sizes1.list_sizes[3] = 0x60;
	sizes1.record_count = 0x100;

	buffer = (byte *)VirtualAlloc(NULL, 0x452e8, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
	if (!buffer)
	{
		GetLastError();
	}
	g_510c44 = buffer;

	function_134980(&g_547f88, &sizes0, g_510c44);
	function_134980(&g_54aa68, &sizes1, g_510c44 + 0x22974);
}

// @retail 0x1320b0
void function_1320b0(void)
{
	function_134b20(&g_547f88);
	function_134b20(&g_54aa68);

	if (g_510c44)
	{
		if (!VirtualFree(g_510c44, 0, MEM_RELEASE))
		{
			GetLastError();
		}
		g_510c44 = NULL;
	}
}

struct s_134240_object
{
	long definition_index;
};

struct s_134240_object_header
{
	byte unknown00[8];
	s_134240_object *object;
};

/* moves the objects whose model has nodes to the front of the third list */
// @retail 0x134240
void function_134240(s_bit_vector_pool *data)
{
	long kept = 0;
	for (long i = 0; i < data->lists[2]->count; i++)
	{
		long object_index = data->lists[2]->longs_a[(short)i];
		if (object_index == NONE)
		{
			continue;
		}
		s_134240_object *object = ((s_134240_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		if (!object || object->definition_index == NONE)
		{
			continue;
		}
		byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
		if (!definition || *(long *)(definition + 0x38) == NONE)
		{
			continue;
		}
		byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
		if (!model || *(long *)(model + 4) == NONE)
		{
			continue;
		}
		byte *render_model = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
		if (*(long *)(render_model + 0x74) > 0)
		{
			if (i != kept)
			{
				data->lists[2]->swap((short)i, (short)kept);
			}
			kept++;
		}
	}
}

static inline real transition_cosine(real x)
{
	real t;
	if (0.0f > x)
	{
		t = 0.0f;
	}
	else if (x > 1.0f)
	{
		t = 1.0f;
	}
	else
	{
		t = x;
	}
	return (real)(0.5f - cos(t * k_pi) * 0.5f);
}

// @retail 0x134c50
real function_134c50(real x)
{
	return 0.0f > transition_cosine(x) ? 0.0f : (transition_cosine(x) > 1.0f ? 1.0f : transition_cosine(x));
}

/* which sides of the pool's plane a sphere touches (both when the plane is
   off or the sphere crosses it) */
// @retail 0x132fd0
void function_132fd0(s_bit_vector_pool const *data, point3f const *center, bool *behind, bool *in_front, real radius)
{
	if (data->flags2a60 & 0x200)
	{
		real distance = plane_distance_to_point(&data->plane, center);

		*in_front = false;
		*behind = false;
		if (0.0f > distance)
		{
			*behind = true;
		}
		if (distance > 0.0f)
		{
			*in_front = true;
		}
		if ((real)fabs(distance) <= radius)
		{
			*in_front = true;
			*behind = true;
		}
	}
	else
	{
		*in_front = true;
		*behind = true;
	}
}


/* the render flags of an entry from its own flags and the pool's */
// @retail 0x1332f0
dword function_1332f0(s_bit_vector_pool const *data, word flags)
{
	dword result = ((((flags >> 2) & 0x2c00) | (flags & 0x4000)) >> 9) | ((flags & 0x200) << 5);

	if (((char)data->flags2a60 >> 7) != 0)
	{
		result |= 1;
	}
	if (g_4b9ef0 == 3)
	{
		if (g_4b9f8c != NONE && g_4b9ffc == 0.0f && !(flags & 0x400) && !(*g_4e3b44[g_4b9f8c & 0xffff].bytes & 2))
		{
			result |= 0x100;
		}
		if (!g_4ba004 || (flags & 0x400))
		{
			result |= 0x80;
		}
	}

	return result;
}

bool function_bec70(long object_index);

struct s_visibility_object
{
	long definition;
	union
	{
		dword flags;
		struct { dword low_flags : 31; dword flag31 : 1; };
	};
	byte unknown08[0x30 - 8];
	point3f center;
	real radius;
	byte unknown40[0x10];
	point3f alternate_center;
	real alternate_radius;
};
struct s_visibility_object_header
{
	byte unknown00[8];
	s_visibility_object *object;
};

// MATCH (m3 lane 8 r211): the flag packer 0x132b10 declares its 0x4000 flag last. Retail evidence: its call here loads the
// register arguments AL, CL, DL in that order (0x133155, 0x13315a, 0x13315f), last to first of (..., e, a, b), and a call's
// arguments are evaluated right to left, so in_front is read ahead of behind and takes flag_b's dead argument slot
// (lea edx, [esp + 0x38] at 0x133136) while behind keeps the frame byte.
// score 4 (was 4): only the two bool locals' stack slots differ (behind/in_front at
// esp+0x17 and esp+0x38). Permuter: 280 tries in 600 s, best 2 — but that form
// swaps the two pointer arguments at the matched 0x132fd0 and leaves `index`
// uninitialized, so it was not kept.
// Secondary output update inside the visibility branch 170, mode conjunction 115, clear-only guard 152.
// visibility predicate at plane call 276, visibility bool reused for front plane 67, fallback assignment 161.
// duplicate guard with return 203, new-object guard with return 4. The plane outputs still exchange homes.
// Round 98 (m3 lane 2, fast, control 4): `in_front` renamed `ahead` 4 and `behind` renamed `rear` 4 (same code: the
// param-home pick does not follow names); 0x132b10's first and last parameters swapped with the call 202 (0x132b10 12:
// a register parameter's position picks its register); the plane outputs and both calls in their own block 4;
// `contained` declared before first/second 7 (only the init stores move).
// M3 lane 1 r119 (fast, control 4; 0x132fd0 and 0x132b10 checked each time): retail keeps in_front in flag_b's dead
// argument slot and behind in the frame byte at 0x13, ours the other way round, every other byte equal. 0x132fd0's two
// outputs declared in the other order (definition and all three calls) 2 here but 0x132fd0 6: its registers follow the
// declared position, so its present order stands, and the homes do not move (only the two lea operands); in_front ahead
// of the centre 11 (0x132fd0 6); behind last, or the radius ahead of in_front-then-behind, 2 (0x132fd0 6); the radius
// second, third or fourth with behind before in_front 4 each (0x132fd0 MATCH, same code). 0x132b10's AL parameter at
// each of its other four positions ahead of the last two 4 each (0x132b10 MATCH, same code); the other 29
// interleavings of its three register parameters give 12 to 21 in 0x132b10 itself (its DL and CL parameters must be
// the last two). first/second/contained declared c,f,s 7, c,s,f 6, s,f,c 6 (only the three init stores reorder, no
// slot moves). The pair declared behind first 4; behind in the outer block after or before the three flags, or at the
// function top, 4 each; in_front at the function top or in the outer block 4 each; the pair named a/z, z/a,
// front/back, x1/x2, x2/x1 4 each; both read into named locals ahead of the packer call, either order, 4 each (same
// code). Diagnostics, restored at once: `bool in_front = false;` alone 54, and the homes are then retail's (in_front
// in the argument slot, behind at 0x13; only the extra store differs); `bool behind = false;` alone 56 (homes ours);
// both initialised, either order, 128 (homes ours); in_front or behind read once more after the packer call 128/129
// (homes ours: the second read folds into the first load); in_front or behind as a byte with a cast at the call
// 126/130; the visibility result reused as the front output 67 re-measured (retail's homes, one extra store of AL);
// in_front initialised from flag_b 57 (the store stays, flag_b moves to EBX), behind from flag_b 59. So the slot goes
// to behind on a tie and to in_front only when in_front alone has an earlier direct store; declaration order, scope,
// names and both callees' parameter orders do not decide it. Open: what gives in_front that precedence with no
// instruction.
// Round 101 (m3 lane 2, fast with 0x132fd0/0x132b10/0x132e60, all MATCH each time; control 4). The rule, measured in a
// compile-only build of this file (which gives retail's homes: in_front in flag_b's slot) and confirmed here: of two
// equal candidates the dead parameter slot goes to the one whose first read or store comes first, address-taking not
// counted; the packer's arguments are evaluated right to left, so behind (its last parameter) is read first and wins.
// Compile-only, the packer's parameters reordered: in_front first / behind last gives in_front the slot (its register
// argument is evaluated ahead of the pushes), behind first / in_front last gives behind, both as neighbouring stack
// arguments gives whichever is further right. Here: the packer's first argument as `!!in_front`, `in_front != false`
// or `in_front ? true : false` 126 each (retail's homes, but a test/setne pair is emitted for AL), `in_front == true`
// 44, `(bool)in_front` 4 (same code); the packer's first parameter as `byte` or `char`, its last as `byte`, the first
// and sixth as `byte`, all seven as `byte` 4 each (same code, 0x132b10 MATCH); after the plane call an empty
// `if (in_front) { }`, `(void)in_front;`, an unused `bool both = in_front && behind;`, a named copy for the packer
// call, `(in_front, in_front)` as the argument 4 each (same code); the packer reached through a force-inline wrapper
// declared (behind, in_front, second, first, contained, flag_a, flag_b) 4 (same code: the arguments are forwarded in
// the packer's own order); 0x134390's hand-written flag expression as a second call of the packer 4 (0x132b10 and
// 0x133390 MATCH). Diagnostics: `!!` on both arguments 144 (607 bytes; behind is read first, into CL), on behind
// alone 130. Open: a read or store of in_front ahead of the packer's arguments that leaves no instruction.
// Round 103 (m3 lane 2, fast with 0x132fd0/0x132b10, both MATCH each time; control 4). Diagnostics, restored at once:
// in_front stored to a byte global right after the plane call 52 with retail's homes (in_front in flag_b's slot; the
// load is pinned ahead of the packer's arguments, which confirms the first-read rule); in_front or behind stored to
// that global after the 0xbec70 call 88/89 (homes ours: the value is loaded once into BL ahead of the packer, so no
// later memory read exists). Forms: in_front passed through a one-line inline identity function 4; a `bool const`
// copy taken right after the plane call and passed 4; a `bool &` bound to in_front for the plane call and the packer
// 4 (all three the same code: the read is forwarded to the argument); the list read into a local at the top of the
// visible block 107; `long entry = NONE;` declared ahead of the two outputs 160.
// M3 lane 6 r198 (fast, 0x132b10/0x132fd0 MATCH each time; control 4): an unused trailing packer parameter fed in_front
// (dropped at link time) 4 (homes ours: the dropped argument's read does not count); 1 to 8 dead initialised locals at
// the function top 4 each (not a symbol-count tie); the packer declared `__stdcall` 4 (same code), `__cdecl` 124;
// in_front through a pointer-taking inline accessor at the packer call, plain or forced with a result local, 4/4
// (forwarded); the outer test's arms in the other order 203 (601 B, homes still ours); a reference-bound copy of
// in_front made after the plane call and passed to the packer, assigned through the reference or initialised then
// bound, 4/4 (forwarded, homes ours).
// M3 lane 12 r147 (fast with 0x132fd0/0x132b10, both MATCH each time; control 4): `__assume(in_front || behind)` after the
// plane call 4 (same code; diagnostic, not keepable); 0x132b10's definition moved below this function (declared above)
// 4 (same code); the centre and radius chosen by two ternaries 36; the object pointer declared ahead of the three flags
// 8 (only the init stores move); the plane call and the packer in a forceinline helper owning the two outputs 291 (671
// bytes). Retail's frame from the top: object +0x10, first +0xc, radius +8, second +4, behind +3, contained +2.
// M3 lane 7 r191 (fast with 0x132fd0/0x132b10/0x132e60/0x133390, all MATCH each time; control 4): `inline` on 0x132b10's
// definition 4; `inline` on 0x132fd0's definition 4 (both stay out of line, same code); behind read for the packer through
// a `bool const &` bound before the plane call 4; the pair in one declarator `bool in_front, behind;` 4; `dword flags;`
// declared ahead of the pair and assigned from the packer 4 (all the same code, homes still ours).
// M3 lane 11 r127 (fast with 0x132fd0/0x132b10/0x132e60, all MATCH each time; control 4): `in_front = in_front;` or
// `behind = behind;` right after the plane call 4/4 (same code: the self-copy is dropped before the homes are picked);
// the pair in one declaration, in_front first or behind first, 4/4; the packer's result declared ahead of the pair and
// assigned at the call 4 (all the same code).
// M3 lane 14 r119 (fast with 0x132fd0/0x132b10/0x132e60, all MATCH each time; control 4): the visible test as an early
// `return` with the block un-nested 4; `long flags` 4; in_front's address passed to the plane call through a pointer
// local 4 (all three the same code); the entry as one ternary 124; an inline helper owning in_front and returning it
// (behind still an out-parameter) 168 (582 B, the helper stays out of line), forced inline 291 (671 B, the listed
// helper's code). Diagnostic: K = 9 to 39 (odd) dead initialised longs at the function top 4 at every K (not a
// symbol-count tie; rounds before covered 1 to 8).
// M3 lane 4 r228 (fast with 0x132fd0/0x132b10/0x132e60, all MATCH each time; control 4): the plane call through a
// forceinline wrapper taking (data, center, in_front, behind, radius) 291 (671 B: 0x132fd0 is then expanded); behind's
// address through a pointer local set before the plane call 4; in_front alone in an inner block with the plane call and
// the packer (behind outside) 4; behind alone in that inner block 4; one loop index for the three else-arm loops 4; one
// index for the flag_a/flag_b loops only 4 (all the 4s the same code).
// M3 lane 13 r126 (fast with 0x132fd0/0x132b10/0x132e60/0x132220; control 4): `word flags` from the packer 4 (same code);
// the filter as one ternary 186; the visibility fallback as a second statement 161 (repeats the listed fallback form).
// Diagnostics, restored at once, homes read from the plane call's two lea operands: without the mode update of `second`
// 205, with false for flag_b at the visibility call 202 (both outputs then in the frame, flag_b stays live), without the
// entry lookup 233 (behind then takes the object index's freed slot, in_front still the frame byte), without the else
// arm's two flag loops 279 (both in the frame): behind wins every freed parameter slot, so no other part of the body
// changes the pair's precedence.
// M3 lane 8 r205 (fast with 0x132fd0/0x132b10, both MATCH each time; control 4): the visibility fallback as a separate
// `if (...) visible = true;` after the call 171 (582 B); `short entry` with a cast at the lookup 4; the entry default
// declared ahead of the packer call 123; the radius declared ahead of the centre 4; the mode test written `== 0` 4 (each 4
// the same code, homes still behind-first).
// M3 lane 11 r135 (fast with 0x132fd0/0x132b10/0x132e60, all MATCH each time; control 4): both outputs declared
// `register` 4, in_front alone so 4; the packer's result kept as a `short` used by the add 4; the centre and radius
// declared ahead of the filter 4 (all four the same code); the outer test with the global stamp on the left 200.
// Permuter (600 s, 48 tries, annealing): best 2 only by passing behind first and in_front last to the packer (the two
// bits exchanged: wrong values, not kept), which again shows the slot going to whichever output is read first.
// M3 lane 14 r127 (fast with 0x132fd0/0x132b10/0x132e60, all MATCH each time; control 4): 0x132fd0's two outputs as `bool &`
// parameters (definition and all three calls) 4; in_front read for the packer through a `bool const &` bound ahead of the
// plane call 4; `register` on in_front 4; both outputs declared in the outer block ahead of the three flags 4, after them
// 4 (all the same code). This function's register parameter `data` declared second, third, fourth or last (definition and
// the four calls) 4 each; 0x132fd0's stack radius declared first, second, third or fourth (definition and its three calls) 4
// each, 0x132fd0 MATCH (all the same code here).
// M3 lane 6 r215 (fast with 0x132fd0/0x132b10, both MATCH each time; control 4): behind's address taken into a pointer at
// the top of the arm (ahead of the flag_b load) and passed to the plane call 4, the same for in_front 4 (the pointer folds
// back to the lea, the address-taking point does not open the live range); behind read for the packer through a
// pointer-taking inline accessor 4; flag_b copied to a `bool const` at the top of the arm and used for both calls 4 (all the
// same code). Diagnostics: the two outputs as a two-element bool array 97; behind's packer read replaced by false 234.
// M3 lane 8 r209 (fast with 0x132fd0/0x132b10/0x132e60; control 4): 0x132fd0's first two stores (and its pair stores)
// in the other order 4 (0x132fd0 2; the caller's homes do not follow the callee's store order); 0x132fd0's outputs
// declared (in_front, behind) at the definition and all three calls 2 here (0x132fd0 6, 0x134390 69 -> 70), with its
// stores reordered to put behind first 2 here (0x132fd0 2, 0x134390 89): the declared third output always takes ESI, so
// the swap only relabels the two lea lines and the packer reads still differ; the mode update of `second` inside the
// visible block 170 (580 B). Diagnostic, restored at once: flag_b's own slot passed as the front output with a copy for
// the packer 174 (608 B: the parameter's address escapes).
// @retail 0x133050
void function_133050(s_bit_vector_pool *data, long object_index, short section, bool flag_a, bool flag_b)
{
	if (g_4de300[object_index & 0xffff] != g_4de2fc)
	{
		bool first = true;
		bool second = true;
		bool contained = true;
		s_visibility_object *object = ((s_visibility_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		c_entry_list *filter = (c_entry_list *)data->filter;
		if (filter)
			filter = *(c_entry_list **)((byte *)filter + 0x14);
		point3f const *center;
		real radius;
		if (!data->mode)
		{
			center = &object->center;
			radius = object->radius;
		}
		else
		{
			center = &object->alternate_center;
			radius = object->alternate_radius;
		}
		bool visible = function_132e60(data, center, radius, filter, object_index, section, flag_b, &contained, &first, &second) || ((char)data->flags2a60 >> 7) != 0;
		if (data->mode && second)
			second = !(object->flags & 0x10000);
		if (visible)
		{
			bool in_front;
			bool behind;
			function_132fd0(data, center, &behind, &in_front, radius);
			dword flags = function_132b10(first, second, flag_a, flag_b, contained, behind, in_front);
			long entry = NONE;
			if (!(flags & 0x800) && TEST_FIELD_BIT(object->flag31))
				entry = function_134300(data, section);
			if (data->lists[2]->add(object_index, (short)flags, 0, (short)entry))
				function_bec70(object_index);
		}
	}
	else
	{
		c_entry_list *list = data->lists[2];
		short index = NONE;
		bool found = false;
		for (long i = 0; i < list->count && !found; i++)
		{
			found = list->longs_a[(short)i] == object_index;
			index = (short)i;
		}
		if (!(list->shorts_b[index] & 0x800))
		{
			short entry = list->shorts_d[index];
			if (entry != NONE)
				data->entries[entry][section >> 5] |= 1 << (section & 31);
		}
		if (flag_a)
		{
			list = data->lists[2];
			for (long i = 0; i < list->count; i++)
			{
				if (list->longs_a[(short)i] == object_index)
				{
					short *flags = &list->shorts_b[(short)i];
					*flags |= 0x400;
					break;
				}
			}
		}
		if (flag_b)
		{
			list = data->lists[2];
			for (long i = 0; i < list->count; i++)
			{
				if (list->longs_a[(short)i] == object_index)
				{
					short *flags = &list->shorts_b[(short)i];
					*flags |= 0x200;
					break;
				}
			}
		}
	}
}

bool function_16e210(long cluster_index, long value);

struct s_visible_cluster
{
	byte unknown00[0x24];
	word surface_count;
	byte unknown26[0x54 - 0x26];
	real bounds[3][2];
	byte unknown6c[0x98 - 0x6c];
	long instance_count;
	word *instances;
	byte unknowna0[0xb0 - 0xa0];
};
struct s_visible_instance
{
	byte unknown00[0x34];
	short definition;
	byte unknown36[6];
	point3f center;
	real radius;
	byte unknown4c[0x58 - 0x4c];
};
struct s_visibility_bsp
{
	byte unknown00[0xa0];
	s_visible_cluster *clusters;
	byte unknowna4[0x13c - 0xa4];
	byte *definitions;
	long instance_count;
	s_visible_instance *instances;
};

short function_2cbf0(long arg_1, dword arg_3,
    long arg_4, dword arg_5, long arg_6, short arg_2, long arg_7, byte *arg_8, short arg_9, byte arg_10,
    void *arg_11, long arg_12, real arg_13, long arg_14, long arg_15,
    bool arg_16, bool arg_17, bool arg_18, real arg_19, real arg_20,
    real arg_21, word arg_22, point3f const *arg_23, real arg_24);

// score 2 (was 86, fast build on r848): only the order of two register-argument loads before the call differs.
// b15F (11-17) walk against retail, fast build on r848: fields, offsets (cluster stride 0xb0, bounds +0x54..+0x68,
// indices byte at arg_1 + 0x1c), the 0x1332f0 and 0x2cbf0 arguments, flag bit 0x40, jae 0x13349c / ja 0x1334ac and
// the loop agree; no behaviour difference. Forms kept, each on retail evidence: (a) the cluster index is a plain
// long in esi (0x1333b8) and its (short) cast is a register movsx at 0x1333d7, before the bounds loads (the byte
// load stays at 0x13344b); the old `long const &` re-read a word. (b) The extent follows the file's 0x134390 idiom
// (centre and corner point3f through real_point3d_set, vector3f through vector3d_from_points3d); retail reads
// +0x60/+0x68 a second time for the corner (0x1333f8, 0x1333fd). Its sum is retail's tree (x*x + y*y) + z*z (fld
// 0x13341d, 0x13342f, 0x133451), which compiles only when written k, j, i; length_sq3f compiles (z*z + x*x) + y*y
// (34), i, j, k gives 33. (c) The plane sum is retail's tree ((z*g38 + x*g20) + y*g2c) + g44 (0x133455-0x13347b)
// only when written y, x, z; z, x, y compiles ((y + x) + z), a last-bit rounding difference (12). The three
// volatile locals are gone (slots match retail's). Remaining 2: retail loads ecx (or ecx, -1 at 0x1334fd) before
// edx (xor edx, edx at 0x133500) for 0x2cbf0, ours edx first; 0x2cbf0 is todo and two constants are no evidence of
// its parameter order, so no swap. sort_4byte is not called here.
// Tried (fast, r848; entries before the last four were measured on earlier bases, before the cast move or the k,
// j, i sum): volatile locals as before (86; 96 with a plain long), real locals (114), volatile vector3f (96),
// double sums (82-93), no corner point (38+), centre orders (16-20), corner orders (19, 35), plane test before the
// extent (85), corner before centre (49), sqrt inlined (84).
// Round 98 (m3 lane 2, fast, control 2): BLOCKED-BY 0x2cbf0 (contributor's; its register-argument order sets which
// constant loads first). Caller forms: the flags `| 0x40` folded into the 0x1332f0 result's declaration 30; the radius
// product written in the call argument 2; the list pointer read at the top of each iteration (loop bound through
// arg_1->lists[0]) 2; the indices byte read after the cluster pointer 80 (410 bytes); `0x40 | local_3` 2.
// HELD (m3 lane 1 r119; needs the owner's OK, 0x2cbf0 is a contributor's function): MATCH (fast and plain) with
// 0x2cbf0's instance parameter declared after category (definition, both declarations, all three calls): 0x133390
// MATCH, 0x133520 and 0x133f70 stay MATCH, 0x2cbf0 stays 10. Retail evidence: this call sets ECX (instance) at
// 0x1334fd, then EDX (category) at 0x133500, then EAX at 0x133502, last to first in that declared order; 0x133520 loads
// the instance at 0x1336b7 ahead of its two constants. In 0x133f70 each register load sits where the pushes free its
// register, so 0x1341b9/0x1341c8 do not decide the order (b16B's swap moved category ahead of the stack parameters,
// which is what cost 0x133f70 its match). The instance sixth to tenth (after category, material, object, mask or
// mode) gives the same bytes in all four; eleventh or later 13 to 25 here, 0x2cbf0 300 and both other callers lost
// (36 to 49). Patch: build/r119/held-133390.patch (lane 1's worktree). Code left as on main.
// MATCH (2026-11-18): 0x2cbf0 declares instance after category, so its register arguments load ecx before edx as in
// retail. Forms kept, each on retail evidence: (a) the cluster index is a plain long in esi (0x1333b8) and its
// (short) cast is a register movsx at 0x1333d7, before the bounds loads (the byte load stays at 0x13344b); the old
// `long const &` re-read a word. (b) The extent follows the file's 0x134390 idiom (centre and corner point3f through
// real_point3d_set, vector3f through vector3d_from_points3d); retail reads +0x60/+0x68 a second time for the corner
// (0x1333f8, 0x1333fd). Its sum is retail's tree (x*x + y*y) + z*z (fld 0x13341d, 0x13342f, 0x133451), which compiles
// only when written k, j, i; length_sq3f compiles (z*z + x*x) + y*y (34), i, j, k gives 33. (c) The plane sum is
// retail's tree ((z*g38 + x*g20) + y*g2c) + g44 (0x133455-0x13347b) only when written y, x, z; z, x, y compiles ((y +
// x) + z), a last-bit rounding difference (12).
// @retail 0x133390
void function_133390(s_bit_vector_pool const *arg_1)
{
	c_entry_list *local_1 = arg_1->lists[0];
	for (long local_2 = 0; local_2 < local_1->count; local_2++)
	{
		dword local_3 = function_1332f0(arg_1, local_1->shorts_b[(short)local_2]);
		long local_4 = local_1->longs_a[(short)local_2];
		short local_13 = ((signed char const *)arg_1->indices)[(short)local_4];
		s_visible_cluster const *local_5 = &((s_visibility_bsp *)g_4e0348)->clusters[local_4];
		point3f local_6;
		real_point3d_set(&local_6, (local_5->bounds[0][0] + local_5->bounds[0][1]) * 0.5f,
			(local_5->bounds[1][0] + local_5->bounds[1][1]) * 0.5f,
			(local_5->bounds[2][0] + local_5->bounds[2][1]) * 0.5f);
		point3f local_15;
		local_15.x = local_5->bounds[0][1];
		local_15.y = local_5->bounds[1][1];
		local_15.z = local_5->bounds[2][1];
		vector3f local_7;
		vector3d_from_points3d(&local_6, &local_15, &local_7);
		real local_10 = sqrt(local_7.k * local_7.k + local_7.j * local_7.j + local_7.i * local_7.i);
		real local_11 = local_6.y * g_4b9e2c + local_6.x * g_4b9e20 + local_6.z * g_4b9e38 + g_4b9e44;
		if (!(local_11 >= 0.0f))
			local_11 = 0.0f - local_11;
		if (!(local_11 > 0.1f))
			local_11 = 0.1f;
		real local_12 = (g_4b9ed0 / local_11) * local_10 * 2.0f;
		function_2cbf0(local_4, local_3 | 0x40, NONE, NONE, 0, NONE, 0, NULL,
			local_13, 0xff, NULL, NONE, local_12, NONE, NONE, false, false,
			false, 0.0f, 0.0f, 0.0f, 0, g_468788, 0.0f);
		local_1 = arg_1->lists[0];
	}
}

// @retail 0x133520
void __stdcall function_133520(s_bit_vector_pool *arg_1)
{
	s_visibility_bsp *local_1 = (s_visibility_bsp *)g_4e0348;
	long local_2 = 0;
	while (local_2 < arg_1->lists[3]->count)
	{
		long local_4 = (short)arg_1->lists[3]->longs_a[(short)local_2];
		s_visible_instance *local_5 = &local_1->instances[(short)local_4];
		dword local_6 = function_1332f0(arg_1, arg_1->lists[3]->shorts_b[(short)local_2]);
		point3f const *local_12 = &local_5->center;
		real local_7 = local_12->z * g_4b9e38 + local_12->y * g_4b9e2c + local_12->x * g_4b9e20 + g_4b9e44;
		if (!(local_7 >= 0.0f))
			local_7 = 0.0f - local_7;
		if (!(local_7 > 0.1f))
			local_7 = 0.1f;
		real local_9 = (local_5->radius / local_7) * g_4b9ed0 * 2.0f;
		short local_8 = arg_1->lists[3]->shorts_d[(short)local_2];
		if (local_5->radius < 0.13f)
			local_9 *= 1.75f;
		real local_10 = (local_9 - 25.0f) * (1.0f / 7.0f);
		if (0.0f > local_10)
			local_10 = 0.0f;
		else
		{
			real local_13 = local_10;
			local_10 = 1.0f;
			if (!(local_13 > 1.0f))
				local_10 = local_13;
		}
		byte local_11 = (byte)(long)(local_10 * 255.0f);
		if (!(arg_1->lists[3]->shorts_b[(short)local_2] & 0x800))
			local_6 |= 0x40;
		else
			local_6 |= 1;
		if (local_11 > 0)
		{
			function_2cbf0(0, local_6 | 0x1008, NONE, 0xffffffff, 0, local_4,
				0, NULL, local_8, local_11, local_5, NONE, local_9, NONE, NONE,
				false, false, false, 0.0f, 0.0f, 0.0f, 0, local_12, local_5->radius);
		}
		else
			arg_1->lists[3]->shorts_b[(short)local_2] |= 1;
		local_2++;
	}
}

__forceinline bool visibility_append(c_entry_list *list, long index, short flags, short entry)
{
	if ((word)list->count < list->maximum_count - 1)
	{
		list->longs_a[(word)list->count] = index;
		list->shorts_b[(word)list->count] = flags;
		list->longs_c[(word)list->count] = 0;
		list->shorts_d[(word)list->count] = entry;
		list->count++;
		return true;
	}
	return false;
}

// score 11 (was 69) in a stand-in image: the cluster radius is passed straight to function_132fd0 (no local), and the
// entry local is declared with its NONE before the function_132e60 call, which gives retail's registers in the instance
// loop (the centre pointer in edi at 0x134693). Left: that NONE store is scheduled ahead of the call (retail stores it
// after the visible test, 0x1346c0), and the else arm's entry read is `movsx edx, word ptr [eax + esi]` where retail
// has `[esi + eax]` (0x134859).
// Earlier (69, was 340): retail tests the flags' sign bit with test/jns where ours uses jge, combines the
// instance flags with behind last (ours earlier), and reloads data->lists[3] after function_16e210 in the
// duplicate-instance branch; writing that reload swaps edx and edi in the instance loop (score 70).
// b10H (11-17) walk against retail, fast build on r841: fields, offsets, jcc senses (NaN included), call arguments,
// flag bits and both loops agree with retail; no behaviour difference found. Retail's forms kept: the sign test is
// ((char)flags >> 7) != 0 (test al,al; jns at 0x1346aa, as 0x1332f0 spells it), and the duplicate branch reloads
// data->lists[3] after function_16e210 (0x134893; ours held the local in esi): 67 -> 69 together, retail's form.
// Remaining: (1) the 0x8000 term is last in retail's OR chain (0x13477a-0x1347a2), 4th in ours, and every source order
// compiles alike; (2) edx/edi swap in the instance loop head (0x13461a-0x134656: instance_index in edx, bsp in edi).
// Tried: definition computed inside the branch (48, chain then matches; retail computes it first, 0x13463c-0x134649);
// ->add() at both list sites (fast 280, not inlined in the reduced image; retail inlines at 0x1345a5 and 0x1347c5, so
// visibility_append and the written-out add predate this note); function_132b10 for the flags (72, same chain);
// shared behind/in_front pair, selected local, per-term ifs, centre pointer, slot local (109-166); long index (151).
// Round 248: the store moved after the visible test, as `long entry;` before the call with `entry = NONE;` at the top
// of the visible block (70) or `long entry = NONE;` declared there (70): the NONE constant and the entry def leave the
// call-setup block, the centre pointer's register priority falls 244 -> 204 under the instance index (216) and it
// loses EDI; `entry = NONE;` after the 0x132fd0 call 212; the else arm's index named (`short entry_index =
// list->shorts_d[index];`, one use) fixes (0x134859) alone, 10; `list->shorts_d[(short)index]` 11; `(long)(short)i >> 5`
// 11. On the late store plus the named index (70): a centre pointer local for both calls 109, a radius local for both
// calls 366, behind/in_front declared with the three call bools 70, NONE stored after the pair's declarations 70, the
// bsp global read at each use 70, definition inside the stamp test 206, filter started NULL 199, entry passed without
// its cast 70. A forced-ranking search says that form matches once the centre pointer outranks the instance index
// (204 -> 217); no source form found that adds that.
// M3 lane 16 r198 (H2M_FAST, on the late store plus the named index, control 70): the allocator ranks the zero-extended
// instance index 216 (the 0x132e60 argument block 56, the index block 128, the duplicate branch's search 96, minus 64 over
// five blocks it spans unread) above the centre pointer 204 (call block 112, plane test 120, minus 28 over the two test
// blocks between); one more integer value referenced in the plane test (+24) or the index unread in the call block
// (-56) would flip them. Forms, each on that base: the duplicate branch's search compares `cluster->instances[j]` 99
// (1364 B), the stamp store through `cluster->instances[j]` 228, the append argument so 230, the stamp test so 171, the
// 0x132e60 argument so 127, `long instance_index` 116. Body unchanged (11).
// M3 lane 11 r177 (H2M_FAST with 0x163c80 and 0x163d20, both MATCH; control 11): the list's add() member at both
// append sites 321 (1373 B: it expands, but the in-front term is then computed ahead of the capacity test), at the
// instance site only 16 (the first plane test's products flip); the cluster filter search through the list's find() 50
// (find expands to retail's loop; the first plane test's products flip); the else arm's entry bit through
// bit_vector_set 11 (same code), with the cluster flag through it too 52; the 0x132b10 packer for the instance flags 11
// (same code), 70 on the late store. On the late store (70): the visible test held in a named bool 69; centre and radius
// locals for both calls 365 (369 on this body: the radius is then not re-read). Allocator reading on the late store:
// the centre pointer (call block 112, plane test 120, minus 28) ranks 204 under the zero-extended instance index 216
// (loop head 128, call block 56, duplicate search 96, minus 64); a forced-ranking search (30 builds) finds nothing
// better than 20, and that only by moving the duplicate branch's list register. Diagnostic: dead initialised longs at
// the function top on the late store, K = 0..11, give 70/69/109/109/111/110/109/109/70/70/110/110 (the tie does not
// move with the symbol count). Body unchanged (11).
// @retail 0x134390
void __stdcall function_134390(s_bit_vector_pool *data)
{
	if (data->flags2a60 & 0x20)
		return;
	for (long i = 0; (short)i < ((s_sort_context *)data->context)->count; i++)
	{
		s_sort_entry *item = &((s_sort_context *)data->context)->entries[(short)i];
		s_visible_cluster *cluster = &((s_visibility_bsp *)g_4e0348)->clusters[item->value];
		data->flags[item->value >> 5] |= 1 << (item->value & 31);
		((byte *)data->indices)[item->value] = (byte)i;
		if (cluster->surface_count > 0)
		{
			long cluster_index = item->value;
			s_visible_cluster *bounds = &((s_visibility_bsp *)g_4e0348)->clusters[cluster_index];
			point3f center;
			real_point3d_set(&center, (bounds->bounds[0][0] + bounds->bounds[0][1]) * 0.5f,
				(bounds->bounds[1][0] + bounds->bounds[1][1]) * 0.5f, (bounds->bounds[2][0] + bounds->bounds[2][1]) * 0.5f);
			point3f corner;
			real_point3d_set(&corner, bounds->bounds[0][1], bounds->bounds[1][1], bounds->bounds[2][1]);
			vector3f extent;
			vector3d_from_points3d(&center, &corner, &extent);
			bool behind;
			bool in_front;
			function_132fd0(data, &center, &behind, &in_front, sqrt(length_sq3f(&extent)));
			if (data->filter && (data->flags2a60 & 1))
			{
				c_entry_list *list = *(c_entry_list **)((byte *)data->filter + 0xc);
				short index = NONE;
				for (long j = 0; j < (word)list->count; j++)
				{
					if (list->longs_a[j] == cluster_index)
					{
						index = (short)j;
						break;
					}
				}
				if (index == NONE)
					continue;
			}
			short flags = (function_16e210(cluster_index, g_4b9f8c) ? 0x400 : 0) |
				(behind ? 0x8000 : 0) | 0x3000;
			c_entry_list *list = data->lists[0];
			if ((word)list->count < list->maximum_count - 1)
			{
				list->longs_a[(word)list->count] = cluster_index;
				list->shorts_b[(word)list->count] = (in_front ? 0x4000 : 0) | flags;
				list->longs_c[(word)list->count] = 0;
				list->shorts_d[(word)list->count] = NONE;
				list->count++;
			}
		}
		for (long j = 0; j < cluster->instance_count; j++)
		{
			word instance_index = cluster->instances[j];
			s_visibility_bsp *current_bsp = (s_visibility_bsp *)g_4e0348;
			s_visible_instance *instance = &current_bsp->instances[instance_index];
			byte *definition = current_bsp->definitions + instance->definition * 0xc8;
			if (g_4e7c20[(short)instance_index] != g_4e7c1c)
			{
				c_entry_list *filter = (c_entry_list *)data->filter;
				if (filter)
					filter = *(c_entry_list **)((byte *)filter + 0x18);
				if (*(word *)(definition + 0x24) > 0)
				{
					bool contained, first, second;
					long entry = NONE;
					if (function_132e60(data, &instance->center, instance->radius, filter, instance_index, (short)i, false, &contained, &first, &second) || ((char)data->flags2a60 >> 7) != 0)
					{
						bool behind;
						bool in_front;
						function_132fd0(data, &instance->center, &behind, &in_front, instance->radius);
						short flags = (function_16e210(item->value, g_4b9f8c) ? 0x400 : 0) |
							(in_front ? 0x4000 : 0) | (second ? 0x2000 : 0) | (first ? 0x1000 : 0) |
							(contained ? 0x800 : 0) | (behind ? 0x8000 : 0);
						if (!(flags & 0x800))
							entry = function_134300(data, (short)i);
						if (visibility_append(data->lists[3], instance_index, flags, (short)entry))
							g_4e7c20[(short)instance_index] = g_4e7c1c;
					}
				}
			}
			else
			{
				c_entry_list *list = data->lists[3];
				short index = NONE;
				bool found = false;
				for (long k = 0; k < list->count && !found; k++)
				{
					found = list->longs_a[(short)k] == instance_index;
					index = (short)k;
				}
				if (!(list->shorts_b[index] & 0x800))
					data->entries[list->shorts_d[index]][(short)i >> 5] |= 1 << ((short)i & 31);
				if (function_16e210(item->value, g_4b9f8c))
					data->lists[3]->shorts_b[index] |= 0x400;
			}
		}
	}
}

#include <xmmintrin.h>
#include "unknown_0b68c0.h"
long function_c3950(long object_index, long *indices, long maximum);

struct s_visibility_link
{
	long salt;
	long object_index;
	long next;
};
__forceinline long visibility_next(s_record_pool *pool, long *next)
{
	long result;
	if (*next != NONE)
	{
		s_visibility_link *link = (s_visibility_link *)(pool->data + (*next & 0xffff) * pool->size);
		long following = link->next;
		if (following != NONE)
			_mm_prefetch((char const *)(pool->data + (following & 0xffff) * pool->size), _MM_HINT_T0);
		*next = following;
		result = link->object_index;
	}
	else
		result = NONE;
	return result;
}

// @retail 0x132220
void __stdcall function_132220(s_bit_vector_pool *data)
{
	short extra_cluster = NONE;
	for (long i = 0; i < ((s_sort_context *)data->context)->count; i++)
	{
		s_sort_context *context = (s_sort_context *)data->context;
		s_sort_entry *entry = &context->entries[i];
		long cluster = entry->value;
		bool selected = function_16e210(cluster, g_4b9f8c);
		long next;
		long index;
		if (!(data->flags2a60 & 0x40))
		{
			next = ((long *)g_4de2e0)[cluster];
			for (index = visibility_next((s_record_pool *)g_4de2e4, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2e4, &next))
			{
				if (!(data->flags2a60 & 0x400) || *((byte *)((s_visibility_object_header *)g_4e0300->data)[index & 0xffff].object + 0xaa) == 7)
					function_133050(data, index, (short)i, selected, false);
			}
			next = ((long *)g_4de2d4)[entry->value];
			for (index = visibility_next((s_record_pool *)g_4de2d8, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2d8, &next))
				function_133050(data, index, (short)i, selected, false);
		}
		if (data->mode == 0 && !(data->flags2a60 & 0x10))
		{
			next = ((long *)g_4e0310)[entry->value];
			for (index = visibility_next((s_record_pool *)g_4e0314, &next); index != NONE; index = visibility_next((s_record_pool *)g_4e0314, &next))
				function_132a30(data, index, i, false);
		}
		if (extra_cluster == NONE && data->mode == 0 && g_4b9ee9 && g_4b9eec != NONE && g_4686c4 != NONE)
		{
			byte *bsp = (byte *)g_4e0348;
			if (*(long *)(bsp + 0xac) > 0)
				extra_cluster = (*(short **)(bsp + 0xb0))[g_4b9eec];
		}
	}
	if (data->mode == 0 && !(data->flags2a60 & 0x10) && data->location.cluster_index != NONE)
	{
		long indices[4];
		long count = function_c3950(g_4b9ed4, indices, 4);
		for (long i = 0; i < count; i++)
		{
			long index = indices[i];
			s_132a30_light *light = &((s_132a30_light *)g_4e030c->data)[index & 0xffff];
			if (((s_132a30_light *)g_4e030c->data)[index & 0xffff].unknown48 == NONE && function_c3140(index) && light->stamp != g_4e0308)
			{
				if (data->lists[1]->add(index, 0, 0, NONE))
					function_c3110(index);
			}
		}
	}
	if (extra_cluster != NONE && !(data->flags2a60 & 0x40))
	{
		long next = ((long *)g_4de2e0)[extra_cluster];
		long index;
		for (index = visibility_next((s_record_pool *)g_4de2e4, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2e4, &next))
			function_133050(data, index, NONE, false, true);
		next = ((long *)g_4de2d4)[extra_cluster];
		for (index = visibility_next((s_record_pool *)g_4de2d8, &next); index != NONE; index = visibility_next((s_record_pool *)g_4de2d8, &next))
			function_133050(data, index, NONE, false, true);
	}
}

struct s_132780
{
	long field_0;
	long field_4[6];
	long field_1c[6];
	byte *field_34;
};

struct s_view;
struct s_frustum_1648d0;
struct s_clip_1648d0;
struct s_projection_1648d0;
bool function_1648d0(s_frustum_1648d0 const *source, s_view *view, s_clip_1648d0 const *clip, s_projection_1648d0 *result, bool make_hull);

// @retail 0x132780
bool function_132780(s_sort_context const *arg_1, s_sort_context const *arg_2, long arg_3, long arg_4, long arg_5, long arg_6, long arg_7, s_132780 *arg_8)
{
	(void)&arg_1;
	(void)&arg_2;
	(void)&arg_3;
	(void)&arg_4;
	(void)&arg_5;
	s_sort_entry const *local_1 = &arg_1->entries[arg_1->order[arg_6]];
	s_sort_entry const *local_2 = &arg_2->entries[arg_2->order[arg_3]];
	bool local_3 = false;
	for (long local_4 = 0; local_4 < ((short const *)local_2)[arg_7 + 1]; local_4++)
	{
		byte const *local_5 = (byte const *)arg_2 + 0x1974 + (((short const *)local_2)[arg_7 + 7] + local_4) * 0x108;
		for (long local_6 = 0; local_6 < ((short const *)local_1)[arg_4 + 1]; local_6++)
		{
			byte const *local_7 = (byte const *)arg_1 + 0x1974 + (((short const *)local_1)[7] + local_6) * 0x108;
			if (arg_8->field_0 >= arg_5)
				return false;
			byte *local_8 = arg_8->field_34 + arg_8->field_0 * 0x9c;
			if (function_1648d0((s_frustum_1648d0 const *)local_7, (s_view *)((byte const *)arg_2 + 4 + *(long const *)local_5 * 0x1bc), (s_clip_1648d0 const *)local_5, (s_projection_1648d0 *)local_8, false))
			{
				long local_9 = arg_8->field_0++;
				*(long *)(local_8 + 0x94) = ((short const *)local_2)[arg_7 + 7] + local_4;
				*(long *)(local_8 + 0x98) = arg_7;
				if (arg_8->field_1c[arg_7] == NONE)
					arg_8->field_1c[arg_7] = local_9;
				arg_8->field_4[arg_7]++;
				local_3 = true;
			}
		}
	}
	return local_3;
}

// @retail 0x132900
void function_132900(s_bit_vector_pool *arg_1, s_sort_context const *arg_2, s_sort_context const *arg_3)
{
	(void)&arg_1;
	(void)&arg_3;
	long local_1 = 0;
	long local_2 = 0;
	while (local_1 < arg_2->count && local_2 < arg_3->count)
	{
		s_sort_entry const *local_3 = &arg_2->entries[arg_2->order[local_1]];
		s_sort_entry const *local_4 = &arg_3->entries[arg_3->order[local_2]];
		short local_6 = local_3->value;
		if (local_4->value > local_6)
			local_1++;
		else if (local_4->value < local_6)
			local_2++;
		else
		{
			if (*(short const *)local_4->unknown02 > 0)
			{
				for (long local_5 = 0; local_5 < *(short const *)arg_2; local_5++)
				{
					if (((short const *)local_3->unknown02)[local_5] > 0)
						function_132780(arg_3, arg_2, local_1, 0, *(short *)((byte *)arg_1 + 0x2ade), local_2, local_5, (s_132780 *)((byte *)arg_1 + 0x2a98));
				}
			}
			local_1++;
			local_2++;
		}
	}
}

struct s_view_collection_1733e0;
void __stdcall function_173130(long camera_count, byte *cameras, long cluster_index, s_view_collection_1733e0 *collection);
void function_1733e0(long cluster_index, s_view_collection_1733e0 *collection, byte const *camera);

// @retail 0x1320f0
s_bit_vector_pool *function_1320f0(long mode, long count, bool use_plane, long flags, byte const *frusta,
	byte const *center, real radius, void *filter, real plane_offset, long cluster)
{
	s_bit_vector_pool *data = &g_547f88;
	if (mode)
		data = &g_54aa68;
	long type = function_132b80(data, mode, (s_132b80_frustum const *)frusta, cluster, count, flags,
		filter, (point3f const *)center, radius, use_plane, plane_offset);
	g_4c0b78.state = (s_2cb30_state *)data;
	function_1348e0(data);
	switch (type)
	{
	case 0:
		function_173130(count, (byte *)frusta, data->location.cluster_index, (s_view_collection_1733e0 *)data->context);
		if (data->filter)
			function_132900(data, (s_sort_context const *)data->context, *(s_sort_context const **)data->filter);
		break;
	case 1:
		function_132d60((s_132d60 *)data);
		break;
	case 2:
		function_1733e0(cluster, (s_view_collection_1733e0 *)data->context, frusta);
		break;
	}
	g_4e7c1c++;
	g_4e7c18 = true;
	function_134390(data);
	g_4e7c18 = false;
	g_4de2fc++;
	g_4de2f8 = true;
	g_4e0308++;
	g_4e0304 = true;
	function_132220(data);
	g_4de2f8 = false;
	g_4e0304 = false;
	return data;
}

s_visibility_pool_storage g_visibility_pool_storage;

s_light_visibility_storage g_light_visibility_storage;
