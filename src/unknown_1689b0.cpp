// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"
#include "partition_colliding_storage.h"
#include "projection_storage.h"
/* UNKNOWN_1689B0.CPP: collision queries (0x1689b0..0x16b569): the flag
   conversions between the query flags and the collision tests' own, and the
   test of a surface against the query */

#include "unknown_11c920.h"
#include "segment_sphere.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "vehicle_ray.h"
#include "vector_products.h"
#include "normalize3d.h"

#define FLAG(bit) (1 << (bit))
#define TEST_FLAG(flags, bit) (((flags) & FLAG(bit)) != 0)
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

/* a collision result, as read here */
struct s_collision_surface_entry
{
	byte unknown0[4];
	dword flags;
};

struct s_collision_result_view
{
	byte unknown00[0x98];
	long surface_count;
	s_collision_surface_entry *surfaces;
};

struct s_collision_index_block
{
	byte unknown00[0x40];
	long count;
	short *indices;
};

struct s_match_globals_collision_view
{
	byte unknown00[0xc4];
	long block_count;
	s_collision_index_block *blocks;
};

// @retail 0x1689b0
bool collision_surface_test(s_collision_result_view const *result, long surface_index, dword flags)
{
	bool valid = true;

	if (result->surface_count <= 0 || (result->surfaces[result->surface_count - 1].flags & 0x10))
	{
		valid = false;
	}
	else if (flags & 0x40000)
	{
		s_match_globals_collision_view *globals = (s_match_globals_collision_view *)g_4e0348;

		if (globals->block_count > 0 && globals->blocks)
		{
			s_collision_index_block *block = globals->blocks;

			if (surface_index < 0 || surface_index >= block->count || block->indices[surface_index * 2] == NONE)
			{
				valid = false;
			}
		}
	}
	return valid;
}

// @retail 0x168ae0
inline dword collision_flags_to_test_flags(dword flags)
{
	dword result = 0;

	SET_FLAG(result, 0, TEST_FLAG(flags, 23));
	SET_FLAG(result, 1, TEST_FLAG(flags, 24));
	SET_FLAG(result, 2, TEST_FLAG(flags, 25));
	SET_FLAG(result, 3, TEST_FLAG(flags, 26));
	SET_FLAG(result, 4, TEST_FLAG(flags, 27));
	SET_FLAG(result, 5, TEST_FLAG(flags, 28));
	return result;
}

// @retail 0x16b500
word collision_flags_to_object_flags(dword flags)
{
	word result = 0;

	SET_FLAG(result, 0, true);
	SET_FLAG(result, 1, TEST_FLAG(flags, 19));
	SET_FLAG(result, 2, TEST_FLAG(flags, 18));
	SET_FLAG(result, 3, TEST_FLAG(flags, 30));
	SET_FLAG(result, 4, TEST_FLAG(flags, 20));
	SET_FLAG(result, 5, TEST_FLAG(flags, 22));
	SET_FLAG(result, 6, TEST_FLAG(flags, 21));
	return result;
}

/* the object header and object fields the collision filters read */
struct s_collision_object_header
{
	byte unknown00[2];
	byte flags;
	byte type;
	byte unknown04[4];
	struct s_collision_object *object;
};

struct s_collision_object
{
	byte unknown000[4];
	dword unknown_bit0 : 1;
	dword unknown_bits1 : 19;
	dword bit20 : 1;
	dword bit21 : 1;
	dword bit22 : 1;
	dword unknown_bits23 : 8;
	dword bit31 : 1;
	byte unknown008[4];
	long next_object;
	long field_xf86fb0;
	byte unknown014[0x40 - 0x14];
	point3f center;
	real radius;
	byte unknown050[0xaa - 0x50];
	char type;
	byte unknown0ab[0x10a - 0xab];
	word unknown10a_bits0 : 2;
	word bit10a_2 : 1;
	word unknown10a_bits3 : 13;
	byte unknown10c[0x12c - 0x10c];
	dword unknown12c_bit0 : 1;
	dword bit12c_1 : 1;
	dword unknown12c_bits2 : 30;
	byte unknown130[0x348 - 0x130];
	word unknown348_bits0 : 3;
	word bit348_3 : 1;
	word unknown348_bits4 : 12;
};

// @retail 0x168a10
bool collision_object_test(long object_index, s_collision_object_header const *header, s_collision_object const *object, dword flags,
	long ignore_object_index, long ignore_object_index2)
{
	bool result = false;
	s_collision_object const volatile *observed = object;

	if (object_index != ignore_object_index && object_index != ignore_object_index2 && !(header->flags & 0x10) &&
		!(*(dword const *)((byte const *)object + 4) & 1))
	{
		word type = header->type;

		if ((flags & (1 << (type + 4))) &&
			!((flags & 0x80000) && TEST_FIELD_BIT(observed->bit20)) &&
			!((flags & 0x40000) && !TEST_FIELD_BIT(observed->bit22)) &&
			!((flags & 0x40000000) && !TEST_FIELD_BIT(observed->bit31)))
		{
			if (type == 0)
			{
				if (!((flags & 0x100000) && TEST_FIELD_BIT(object->bit10a_2)) && !((flags & 0x400000) && TEST_FIELD_BIT(object->bit348_3)))
				{
					result = true;
				}
			}
			else if (type != 0xb || !((flags & 0x200000) && TEST_FIELD_BIT(object->bit12c_1)))
			{
				result = true;
			}
		}
	}
	return result;
}

// @retail 0x16b430
word collision_object_flags(long object_index)
{
	s_collision_object_header *header = &((s_collision_object_header *)g_4e0300->data)[object_index & 0xffff];
	s_collision_object *object = header->object;
	long type = object->type;
	word result = 0;

	if ((header->flags & 0x10) || (*(dword *)((byte *)object + 4) & 1))
	{
		result = 1;
	}
	SET_FLAG(result, 1, TEST_FIELD_BIT(object->bit20));
	SET_FLAG(result, 2, !TEST_FIELD_BIT(object->bit22));
	if (!TEST_FIELD_BIT(object->bit31))
	{
		result |= FLAG(3);
	}
	if (type == 0)
	{
		SET_FLAG(result, 4, TEST_FIELD_BIT(object->bit10a_2));
		SET_FLAG(result, 5, TEST_FIELD_BIT(object->bit348_3));
	}
	else if (type == 0xb)
	{
		SET_FLAG(result, 6, TEST_FIELD_BIT(object->bit12c_1));
	}
	return result;
}

/* the lookup of 0x1efb40 (unknown_1efac0.cpp) */
struct s_lookup
{
	long handle;
	void *tag_a;
	void *tag_b;
	void *pointer_a;
	void *pointer_b;

	bool initialize(long object_handle);
};

struct s_table_holder;
void *function_1efd80(s_table_holder *holder, dword position);

bool function_1efdc0(s_lookup *lookup, point3f const *point);
bool object_or_parent_hidden(long object_index);

// @retail 0x168e20
bool __stdcall function_168e20(long object_index, bool skip_test, dword flags,
	point3f const *point, long ignore_object_index, long ignore_object_index2)
{
	// The recursive call passes these arguments on the stack.
	(void)&object_index;
	(void)&skip_test;
	do
	{
		s_collision_object_header *header = &((s_collision_object_header *)g_4e0300->data)[object_index & 0xffff];
		s_collision_object *object = header->object;
		if (!skip_test)
		{
			if (!collision_object_test(object_index, header, object, flags, ignore_object_index, ignore_object_index2))
				goto next;
			real x = object->center.x - point->x;
			real y = object->center.y - point->y;
			real z = object->center.z - point->z;
			real radius = object->radius;
			real distance_squared = x * x;
			distance_squared += y * y;
			distance_squared += z * z;
			if (!(distance_squared <= radius * radius))
				goto next;
		}
		{
			s_lookup lookup;
			s_lookup *lookup_pointer = &lookup;
			if (lookup_pointer->initialize(object_index) && function_1efdc0(lookup_pointer, point))
				return true;
		}
		if (!(flags & 0x20000))
		{
			long child = object->field_xf86fb0;
			if (child != NONE && !object_or_parent_hidden(child) &&
				function_168e20(child, false, flags, point, ignore_object_index, ignore_object_index2))
				return true;
		}
	next:
		object_index = object->next_object;
	} while (object_index != NONE);
	return false;
}

/* defined in unknown_183ee0.cpp */
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;

/* a collision result's surface reference, as read here */
struct s_collision_reference
{
	long type;
	byte unknown04[0x3c - 0x4];
	long permutation_index;
	long object_handle;
	byte unknown44[4];
	dword position;
};

struct s_168ce0_section
{
	byte unknown00[0x70];
	byte data[0xc8 - 0x70];
};

struct s_168ce0_permutation
{
	byte unknown00[0x34];
	short section_index;
	byte unknown36[0x58 - 0x36];
};

struct s_168ce0_bsp_view
{
	byte unknown000[0x13c];
	s_168ce0_section *sections;
	byte unknown140[4];
	s_168ce0_permutation *permutations;
};

// @retail 0x168ce0
inline void *collision_reference_get_data(s_collision_reference const *reference)
{
	void *result = NULL;

	switch (reference->type)
	{
	case 1:
		result = g_4e0340;
		break;
	case 3:
	{
		s_168ce0_bsp_view *bsp = (s_168ce0_bsp_view *)g_4e0348;

		result = bsp->sections[bsp->permutations[reference->permutation_index].section_index].data;
		break;
	}
	case 4:
	{
		s_lookup lookup;

		if (lookup.initialize(reference->object_handle))
		{
			result = function_1efd80((s_table_holder *)&lookup, reference->position);
		}
		break;
	}
	}
	return result;
}
struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);

struct s_168d60_instance;
struct s_168d60_bsp_view
{
	byte unknown000[0x13c];
	byte *sections;
	byte unknown140[4];
	struct s_168d60_instance *instances;
};

/* the structure's instanced geometry, as the point test reads it */
struct s_168d60_instance
{
	transform4x3f matrix;
	short section_index;
	byte unknown36[0x3c - 0x36];
	point3f center;
	real radius;
	byte unknown4c[0x58 - 0x4c];
};

// @retail 0x168d60
bool collision_point_inside_instance(long instance_index, point3f const *point, dword flags)
{
	bool result = false;
	s_168d60_bsp_view *bsp = (s_168d60_bsp_view *)g_4e0348;
	s_168d60_instance *instance = &bsp->instances[instance_index];
	byte *section = bsp->sections + instance->section_index * 0xc8;

	if (collision_surface_test((s_collision_result_view const *)section, instance_index, flags))
	{
		vector3f delta;

		vector3d_from_points3d(point, &instance->center, &delta);
		if (delta.j * delta.j + delta.k * delta.k + delta.i * delta.i <= instance->radius * instance->radius)
		{
			point3f local_point;

			function_142700(&instance->matrix, point, &local_point);
			if (function_14a280((s_bsp3d *)(section + 0x70), 0, &local_point) == NONE)
				result = true;
		}
	}
	return result;
}

/* a surface found by a collision test, before it is transformed */
struct s_168b40_surface
{
	long unknown00;
	plane3f const *plane;
	long unknown08;
	long unknown0c;
	dword flags;
	byte unknown14;
	byte unknown15;
	short unknown16;
};

/* the surface part of a collision result */
struct s_168b40_result
{
	byte unknown00[4];
	long unknown04;
	byte unknown08[0x28 - 0x8];
	plane3f plane;
	byte unknown38[0x4c - 0x38];
	long unknown4c;
	long unknown50;
	dword flags;
	byte unknown58;
	byte unknown59;
	short unknown5a;
};

static inline vector3f *transform4x3f_apply_normal(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
	real x = vector->i, y = vector->j, z = vector->k;
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

static __forceinline void plane_negate_168b40(plane3f const *plane, plane3f *out)
{
	out->i = 0.0f - plane->i;
	out->j = 0.0f - plane->j;
	out->k = 0.0f - plane->k;
	out->d = 0.0f - plane->d;
}

// @retail 0x168b40
void function_168b40(s_168b40_result *result, s_168b40_surface const *surface, transform4x3f const *matrix)
{
	result->unknown04 = surface->unknown00;
	if (matrix)
	{
		plane3f const *plane = surface->plane;

		real x = plane->i;
		real y = plane->j;
		real z = plane->k;
		result->plane.i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
		result->plane.j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
		result->plane.k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
		point3f const *position = &matrix->position;
		real distance = position->z * result->plane.k + plane->d * matrix->scale;
		distance += position->y * result->plane.j;
		distance += result->plane.i * position->x;
		result->plane.d = distance;
		if (surface->flags & 0x8000)
		{
			plane_negate_168b40(&result->plane, &result->plane);
		}
	}
	else if (surface->flags & 0x8000)
	{
		plane3f const *plane = surface->plane;

		result->plane.i = 0.0f - plane->i;
		result->plane.j = 0.0f - plane->j;
		result->plane.k = 0.0f - plane->k;
		result->plane.d = 0.0f - plane->d;
	}
	else
	{
		result->plane = *surface->plane;
	}
	result->unknown4c = surface->unknown08;
	result->unknown50 = surface->unknown0c;
	result->flags = surface->flags;
	result->unknown58 = surface->unknown14;
	result->unknown59 = surface->unknown15;
	result->unknown5a = surface->unknown16;
}

/* a collision result (0x5c bytes) */
struct s_collision_location
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	s_collision_location start_location;
	s_collision_location end_location;
	short material_type;
	byte unknown26[2];
	plane3f plane;
	long instance_index;
	long unknown3c;
	long unknown40;
	byte unknown44[0x4c - 0x44];
	long bsp_surface_reference[3];
	byte bsp_surface_flags[2];
	short bsp_surface_index;
};

/* the structure bsp's test of a vector (lane C's 0x1de630), as read here */
struct s_collision_bsp_test_vector_result
{
	real t;
	plane3f const *plane;
	long surface_reference[3];
	byte surface_flags[2];
	short surface_index;
	long leaf_count;
	long leaves[0x100];
};

/* the structure bsp, as the vector test reads it */
struct s_1697c0_material
{
	byte unknown00[8];
	short material_type;
	byte unknown0a[0x14 - 0xa];
};

struct s_1697c0_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_1697c0_instanced_plane
{
	byte unknown00[2];
	short material_type;
	plane3f plane;
	byte unknown14[0x18 - 0x14];
};

struct s_1697c0_cluster
{
	byte unknown00[0x70];
	char instanced_plane_reference;
	byte unknown71[0x98 - 0x71];
	long instance_count;
	short *instances;
	byte unknowna0[0xb0 - 0xa0];
};

struct s_1697c0_bsp
{
	byte unknown00[0x10];
	s_1697c0_material *materials;
	byte unknown14[0x30 - 0x14];
	s_1697c0_leaf *leaves;
	byte unknown34[0x68 - 0x34];
	s_1697c0_instanced_plane *instanced_planes;
	byte unknown6c[0xa0 - 0x6c];
	s_1697c0_cluster *clusters;
};

/* an object's reference in a cluster (the object cluster iterator's) */
struct s_object_cluster_reference
{
	byte type;
	byte unknown01;
	word flags;
	point3f center;
	real radius;
};

struct s_object_cluster_iterator
{
	long next;
};

struct s_bsp3d;
inline dword collision_flags_to_test_flags(dword flags);
word collision_flags_to_object_flags(dword flags);
bool function_1de630(dword test_flags, s_slot_entry_list *bsp, s_collision_bsp_test_vector_result *result, real maximum_t,
	long maximum_leaf_count, byte const *arg_c9e1f7, point3f const *point, vector3f const *vector);
bool function_244980(long cluster_index, point3f const *point, vector3f const *vector, dword flags,
	dword test_flags, s_collision_result_1697c0 *result);
long function_b8940(long cluster_index, s_object_cluster_reference **reference, s_object_cluster_iterator *iterator);
long function_b89b0(s_object_cluster_reference **reference, s_object_cluster_iterator *iterator);
bool __stdcall function_169510(long object_index, bool a, dword flags, dword test_flags, point3f const *point,
	vector3f const *vector, long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

struct s_lookup_ray_hit_ad
{
	real t;
	plane3f const *plane;
	long surface_reference[3];
	byte surface_flags[2];
	short surface_index;
	long leaf_count;
	long leaves[256];
};

struct s_lookup_ray_result
{
	dword position;
	short node;
	short item;
	s_lookup_ray_hit_ad hit;
};

bool function_1eff40(s_lookup *lookup, long flags, point3f const *point,
	vector3f const *direction, s_lookup_ray_result *result);
short *function_d8ae0(long object_index, long region_index, short *material);

// score 12 (was 26): the material local sits at esp+0x18 where retail has
// esp+0x1a, and the material store is scheduled after the hit copies; tried
// the local declared first, between found and lookup, in the inner block and
// as a one-element array, all 12.
// @retail 0x169510
bool __stdcall function_169510(long object_index, bool a, dword flags, dword test_flags, point3f const *point,
	vector3f const *vector, long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result)
{
	bool found = false;
	s_lookup lookup;
	s_lookup_ray_result hit;
	short material;
	do
	{
		s_collision_object_header *header = &((s_collision_object_header *)g_4e0300->data)[object_index & 0xffff];
		s_collision_object *object = header->object;
		s_collision_object *saved_object = object;
		if (a || (collision_object_test(object_index, header, object, flags, ignore_object_index, ignore_unit_index) &&
			function_11e5e0(point, vector, (point3f const *)((byte *)object + 0x40), *(real *)((byte *)object + 0x4c))))
		{
			s_lookup *lookup_pointer = &lookup;
			if (lookup_pointer->initialize(object_index) && function_1eff40(lookup_pointer, test_flags, point, vector, &hit) && result->t > hit.hit.t)
			{
				result->type = 4;
				result->material_type = *function_d8ae0(lookup.handle, hit.hit.surface_index, &material);
				result->instance_index = NONE;
				result->unknown3c = NONE;
				*(short *)result->unknown44 = hit.item;
				*(short *)(result->unknown44 + 2) = hit.node;
				*(dword *)(result->unknown44 + 4) = hit.position;
				result->unknown40 = object_index;
				function_168b40((s_168b40_result *)result, (s_168b40_surface const *)&hit.hit,
					(transform4x3f const *)lookup.pointer_b + hit.node);
				found = true;
				if (flags & 0x10000000) goto done;
			}
			if (!(flags & 0x20000))
			{
				long child_index = *(long *)((byte *)saved_object + 0x10);
				if (child_index != NONE && function_169510(child_index, false, flags, test_flags, point, vector,
					ignore_object_index, ignore_unit_index, result))
				{
					found = true;
					if (flags & 0x10000000) goto done;
				}
			}
		}
		object_index = *(long *)((byte *)saved_object + 0xc);
	} while (object_index != NONE);
done:
	return found;
}

struct s_biped_ground_collision;
struct s_location;
extern short g_4686c4;
void function_11bed0(s_location *location, point3f const *point);

PRIVATE inline void collision_location_reset_1696d0(s_collision_location *location)
{
	location->leaf_index = NONE;
	location->cluster_index = NONE;
	location->bsp_index = g_4686c4;
}

PRIVATE inline void point_along_vector_1696d0(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}

// @retail 0x1696d0
bool function_1696d0(long flags, s_biped_ground_collision *output, long object_index,
	point3f const *point, vector3f const *vector, long ignore_object_index, long ignore_unit_index)
{
	/* retail keeps the point on the stack */
	point3f const *const *point_reference = &point;
	s_collision_result_1697c0 *collision = (s_collision_result_1697c0 *)output;
	collision->type = 0;
	collision->t = 1.0f;
	collision_location_reset_1696d0(&collision->start_location);
	collision_location_reset_1696d0(&collision->end_location);
	bool found = false;
	collision->instance_index = NONE;
	collision->unknown3c = NONE;
	collision->unknown40 = NONE;
	if ((flags & 8) && !(flags & 0x1fff0)) flags |= 0x1fff0;
	if (!object_or_parent_hidden(object_index) &&
		function_169510(object_index, false, flags, collision_flags_to_test_flags(flags),
			*point_reference, vector, ignore_object_index, ignore_unit_index, collision))
		found = true;
	point_along_vector_1696d0(*point_reference, vector, collision->t, &collision->point);
	function_11bed0((s_location *)&collision->end_location, &collision->point);
	return found;
}
struct s_location;
void function_11bed0(s_location *location, point3f const *point);

extern short g_4686c4;
extern short g_47d8e0;
extern byte *g_4ed280;
#include "cluster_marks_storage.h"
long g_4e7c1c;
bool g_4e7c18;
extern s_bsp3d *g_4e033c;

static inline long collision_leaf_cluster(long leaf_index)
{
	long result;

	if (leaf_index != NONE)
	{
		result = ((s_1697c0_bsp *)g_4e0348)->leaves[leaf_index].cluster_index;
	}
	else
	{
		result = NONE;
	}
	return result;
}

struct s_material_168f40
{
	byte unknown00[0xc];
	short material;
	byte unknown0e[2];
};

struct s_palette_168f40
{
	byte unknown000[0x34c];
	s_material_168f40 *materials;
};

// `inline`: retail's callers keep no XMM value across this call (0xf6670 re-makes its zero after it, where a
// plain definition lets the caller hold it in XMM7); the function stays out of line and matches either way.
// @retail 0x168f40
inline bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray,
	long ignore_object_index, long ignore_object_index2)
{
	bool result = false;
	point3f const *point = &ray->point;
	long leaf = function_14a280(g_4e033c, 0, (point3f *)point);
	if (leaf == NONE)
		result = true;
	else if ((flags & 2) || (flags & 0xc))
	{
		s_1697c0_bsp *bsp = (s_1697c0_bsp *)g_4e0348;
		long cluster_index = bsp->leaves[leaf].cluster_index;
		if (flags & 2)
		{
			s_1697c0_cluster *cluster = &bsp->clusters[cluster_index];
			if (cluster->instanced_plane_reference != NONE)
			{
				char reference = cluster->instanced_plane_reference;
				if (reference & 0x80)
				{
					s_1697c0_instanced_plane *entry = &bsp->instanced_planes[reference & 0x7f];
					if (entry->material_type != NONE)
					{
						real distance = entry->plane.k * point->z;
						distance += entry->plane.j * point->y;
						distance += entry->plane.i * point->x;
						distance -= entry->plane.d;
						if (0.0f > distance)
							result = true;
					}
				}
				else if (((s_palette_168f40 *)g_4e0350)->materials[reference & 0x7f].material != NONE)
					result = true;
			}
		}
		if ((flags & 4) && !result)
		{
			s_1697c0_cluster *cluster = &bsp->clusters[cluster_index];
			for (long i = 0; i < cluster->instance_count; ++i)
			{
				long instance_index = cluster->instances[i];
				if (collision_point_inside_instance(instance_index, point, flags))
				{
					result = true;
					break;
				}
			}
		}
		if ((flags & 8) && !result)
		{
			s_object_cluster_reference *reference = NULL;
			word object_flags = collision_flags_to_object_flags(flags);
			word types = (word)(((dword)flags >> 4) & 0x1fff);
			s_object_cluster_iterator iterator;
			for (long object_index = function_b8940(cluster_index, &reference, &iterator);
				object_index != NONE; object_index = function_b89b0(&reference, &iterator))
			{
				if ((types & (1 << reference->type)) && !(reference->flags & object_flags))
				{
					real x = reference->center.x - point->x;
					real y = reference->center.y - point->y;
					real z = reference->center.z - point->z;
					real radius = reference->radius;
					real distance = x * x;
					distance += y * y;
					distance += z * z;
					if (distance <= radius * radius && object_index != ignore_object_index && object_index != ignore_object_index2 &&
						function_168e20(object_index, true, flags, point, ignore_object_index, ignore_object_index2))
					{
						result = true;
						break;
					}
				}
			}
		}
	}
	return result;
}

// score 25 (was 25): plane k/j product operands at +0x314, late i/y
// products and success-result reload differ; single plane k/j snapshots
// 340/357; start z/y 124/124; movement j 56; plane k+j 390;
// correction movement i 56; start z+y 157; the found path's `result = true;` dropped 92
// (retail's single result-reload exit, but the reload is hoisted onto two inner paths);
// `return true;` there 25; point/vector read directly in the plane distances 124; the k/j
// plane terms through `instanced_plane->plane` 177 (both k products then load the plane
// first, as retail; j does not, and the stepping block grows); the j operands swapped 25.
// Round 57: plane->n products 25; a typed normal pointer for all/k-only/j-only products
// 25/25/25; normal k/j with plane i 56; direct entry k-only/j-only 25/25; return the existing
// result from the successful correction branch 24 (one-point gain and 2168 bytes, restored).
// @retail 0x1697c0
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision)
{
	vector3f const * *vector_reference = &vector;
	point3f const * *point_reference = &point;
	long *flags_reference = &flags;
	s_1697c0_bsp *bsp;
	bool result = false;
	dword test_flags;
	s_collision_bsp_test_vector_result bsp_result;
	bsp = (s_1697c0_bsp *)g_4e0348;

	if (!((*flags_reference) & 0x1800000))
	{
		(*flags_reference) |= 0x1800000;
	}
	collision->t = 1.0f;
	collision->type = 0;
	collision_location_reset_1696d0(&collision->start_location);
	collision_location_reset_1696d0(&collision->end_location);
	collision->instance_index = NONE;
	collision->unknown3c = NONE;
	collision->unknown40 = NONE;
	test_flags = collision_flags_to_test_flags((*flags_reference));
	if (function_1de630(test_flags, g_4e0340, &bsp_result, 3.4028235e38f, 0x100, g_4ed280 + g_4686c4 * 0x20 + 1, (*point_reference),
		(*vector_reference)) && ((*flags_reference) & 1))
	{
		short surface_index = bsp_result.surface_index;
		short const *material_type;

		collision->type = 1;
		if (surface_index != NONE)
		{
			material_type = &bsp->materials[surface_index].material_type;
		}
		else
		{
			material_type = &g_47d8e0;
		}
		collision->material_type = *material_type;
		collision->instance_index = NONE;
		collision->unknown3c = NONE;
		collision->unknown40 = NONE;
		collision->t = bsp_result.t;
		if (bsp_result.surface_reference[2] & 0x8000)
		{
			collision->plane.i = 0.0f - bsp_result.plane->i;
			collision->plane.j = 0.0f - bsp_result.plane->j;
			collision->plane.k = 0.0f - bsp_result.plane->k;
			collision->plane.d = 0.0f - bsp_result.plane->d;
		}
		else
		{
			collision->plane = *bsp_result.plane;
		}
		collision->bsp_surface_reference[0] = bsp_result.surface_reference[0];
		collision->bsp_surface_reference[1] = bsp_result.surface_reference[1];
		collision->bsp_surface_reference[2] = bsp_result.surface_reference[2];
		collision->bsp_surface_flags[0] = bsp_result.surface_flags[0];
		collision->bsp_surface_flags[1] = bsp_result.surface_flags[1];
		collision->bsp_surface_index = surface_index;
		result = true;
	}

	if (bsp_result.leaf_count > 0)
	{
		long leaf_index;

		leaf_index = bsp_result.leaves[0];
		collision->start_location.leaf_index = leaf_index;
		collision->start_location.cluster_index = (short)collision_leaf_cluster(leaf_index);
		collision->start_location.bsp_index = g_4686c4;
		leaf_index = bsp_result.leaves[bsp_result.leaf_count - 1];
		collision->end_location.leaf_index = leaf_index;
		collision->end_location.cluster_index = (short)collision_leaf_cluster(leaf_index);
		collision->end_location.bsp_index = g_4686c4;

		if (((*flags_reference) & 0xe) && (!result || !((*flags_reference) & 0x10000000)))
		{
			long leaf;

			g_4e7414++;
			g_4e7411 = true;
			if ((*flags_reference) & 4)
			{
				g_4e7c1c++;
				g_4e7c18 = true;
			}
			if ((*flags_reference) & 8)
			{
				g_4de2fc++;
				g_4de2f8 = true;
				if (!((*flags_reference) & 0x1fff0))
				{
					(*flags_reference) |= 0x1fff0;
				}
			}
			for (leaf = 0; leaf < bsp_result.leaf_count; leaf++)
			{
				long cluster_index = collision_leaf_cluster(bsp_result.leaves[leaf]);

				if (g_4e7418[(short)cluster_index] == g_4e7414)
				{
					continue;
				}
				g_4e7418[(short)cluster_index] = g_4e7414;
				s_1697c0_cluster *cluster = &bsp->clusters[cluster_index];

				if ((*flags_reference) & 2)
				{
					long reference = cluster->instanced_plane_reference;

					if ((char)reference != NONE && (reference & 0x80))
					{
						long plane_index = cluster->instanced_plane_reference & 0x7f;
						s_1697c0_instanced_plane *instanced_plane = &bsp->instanced_planes[plane_index];

						if (instanced_plane->material_type != NONE)
						{
							plane3f const *plane = &instanced_plane->plane;
							real point_distance = plane->k * (*point_reference)->z;
							point_distance += plane->j * (*point_reference)->y;
							point_distance += plane->i * (*point_reference)->x;
							point_distance -= plane->d;
							real vector_distance = plane->k * (*vector_reference)->k;
							vector_distance += plane->j * (*vector_reference)->j;
							vector_distance += plane->i * (*vector_reference)->i;

							if ((point_distance > 0.0f) != (vector_distance > 0.0f))
							{
								real vector_length = (real)fabs(vector_distance);

								if (vector_length > (real)fabs(point_distance) && vector_length >= 0.0001f)
								{
									real t = 0.0f - point_distance / vector_distance;

									if (collision->t > t)
									{
										bool behind = 0.0f > point_distance;

										collision->type = 2;
										collision->t = t;
										collision->material_type = instanced_plane->material_type;
										collision->plane = *plane;
										if (behind)
										{
											plane_negate_168b40(&collision->plane, &collision->plane);
										}
										collision->instance_index = plane_index;
										collision->unknown3c = NONE;
										collision->unknown40 = NONE;
										result = true;
									}
								}
							}
						}
					}
					if (result && ((*flags_reference) & 0x10000000))
					{
						break;
					}
				}
				if ((*flags_reference) & 4)
				{
					if (function_244980(cluster_index, (*point_reference), (*vector_reference), (*flags_reference), test_flags, collision))
					{
						result = true;
					}
					if (result && ((*flags_reference) & 0x10000000))
					{
						break;
					}
				}
				if ((*flags_reference) & 8)
				{
					s_object_cluster_reference *reference = NULL;
					word object_flags = collision_flags_to_object_flags((*flags_reference));
					word type_mask = (word)(((dword)(*flags_reference) >> 4) & 0x1fff);
					s_object_cluster_iterator iterator;
					long object_index;

					for (object_index = function_b8940(cluster_index, &reference, &iterator); object_index != NONE;
						object_index = function_b89b0(&reference, &iterator))
					{
						long object_mark_index = object_index & 0xffff;

						if (g_4de300[object_mark_index] == g_4de2fc)
						{
							continue;
						}
						g_4de300[object_mark_index] = g_4de2fc;
						if ((type_mask & (1 << reference->type)) && !(reference->flags & object_flags) &&
							function_11e5e0((*point_reference), (*vector_reference), &reference->center, reference->radius) &&
							object_index != ignore_object_index && object_index != ignore_unit_index &&
							function_169510(object_index, true, (*flags_reference), test_flags, (*point_reference), (*vector_reference), ignore_object_index,
								ignore_unit_index, collision))
						{
							result = true;
						}
						if (result && ((*flags_reference) & 0x10000000))
						{
							break;
						}
					}
					if (result && ((*flags_reference) & 0x10000000))
					{
						break;
					}
				}
			}
			if ((*flags_reference) & 8)
			{
				g_4de2f8 = false;
			}
			if ((*flags_reference) & 4)
			{
				g_4e7c18 = false;
			}
			g_4e7411 = false;
		}
	}

	point_along_vector_1696d0((*point_reference), (*vector_reference), collision->t, &collision->point);
	if (result && ((*flags_reference) & 0x20000000))
	{
		s_bsp3d *surface_bsp;

		if (collision->type != 1)
		{
			surface_bsp = (s_bsp3d *)collision_reference_get_data((s_collision_reference const *)collision);
		}
		else
		{
			surface_bsp = NULL;
		}
		if (collision->end_location.leaf_index != NONE &&
			function_14a280(g_4e033c, 0, &collision->point) != collision->end_location.leaf_index ||
			surface_bsp && collision->bsp_surface_reference[0] != NONE &&
			function_14a280(surface_bsp, 0, &collision->point) != collision->bsp_surface_reference[0])
		{
			point_along_vector_1696d0(&collision->point, (vector3f const *)&collision->plane, 0.000244140625f, &collision->point);
			function_11bed0((s_location *)&collision->end_location, &collision->point);
			if (collision->end_location.leaf_index == NONE ||
				surface_bsp && function_14a280(surface_bsp, 0, &collision->point) == NONE)
			{
				real normal_speed = collision->plane.k * (*vector_reference)->k;
				normal_speed += collision->plane.j * (*vector_reference)->j;
				normal_speed += (*vector_reference)->i * collision->plane.i;
				real step;

				if (normal_speed != 0.0f)
				{
					step = 0.000244140625f / (real)fabs(normal_speed);
				}
				else
				{
					step = 0.03125f;
				}
				do
				{
					real t = collision->t - step;

					if (!(t > 0.0f))
					{
						t = 0.0f;
					}
					collision->t = t;
					point_along_vector_1696d0((*point_reference), (*vector_reference), t, &collision->point);
					function_11bed0((s_location *)&collision->end_location, &collision->point);
				}
				while (!(collision->t <= 0.0f) && (collision->end_location.leaf_index == NONE ||
					surface_bsp && function_14a280(surface_bsp, 0, &collision->point) == NONE));
			}
		}
		result = true;
	}

	return result;
}

bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

/* retail calls this out of line (0xcf040, 0x1c8df0, 0x24da18, 0x24e59f) */
__declspec(noinline) bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
	long ignore_unit_index, s_collision_result_1697c0 *result);

/* ray_cast_test between two points */
// @retail 0x16a040
bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
	long ignore_unit_index, s_collision_result_1697c0 *result)
{
	vector3f vector;

	vector.i = point1->x - point0->x;
	vector.j = point1->y - point0->y;
	vector.k = point1->z - point0->z;
	return function_1697c0(flags, point0, &vector, ignore_object_index, ignore_unit_index, result);
}

bool function_1691a0(long object_index, dword flags, dword test_flags, point3f const *point, vector3f const *vector,
	s_collision_result_1697c0 *collision);

/* retail calls this out of line from its one caller (0xdf880) */
__declspec(noinline) bool collision_test_vector_object(dword flags, s_collision_result_1697c0 *collision,
	long object_index, point3f const *point, vector3f const *vector);

/* tests a vector against one object */
// @retail 0x169430
bool collision_test_vector_object(dword flags, s_collision_result_1697c0 *collision, long object_index,
	point3f const *point, vector3f const *vector)
{
	/* retail keeps the point on the stack */
	point3f const *const *point_reference = &point;
	collision->type = 0;
	collision->t = 1.0f;
	collision_location_reset_1696d0(&collision->start_location);
	collision_location_reset_1696d0(&collision->end_location);
	bool found = false;
	collision->instance_index = NONE;
	collision->unknown3c = NONE;
	collision->unknown40 = NONE;
	if ((flags & 8) && !(flags & 0x1fff0)) flags |= 0x1fff0;
	if (function_1691a0(object_index, flags, collision_flags_to_test_flags(flags), *point_reference, vector, collision))
		found = true;
	point_along_vector_1696d0(*point_reference, vector, collision->t, &collision->point);
	function_11bed0((s_location *)&collision->end_location, &collision->point);
	return found;
}

real function_30bf0(vector3f *v);

/* moves a point toward another until it hits something, stopping just short */
// @retail 0x16a7c0
bool function_16a7c0(point3f const *from, point3f const *to, long ignore_object_index,
	long ignore_unit_index, point3f *result)
{
	vector3f vector;
	s_collision_result_1697c0 collision;
	bool moved;

	vector.i = to->x - from->x;
	vector.j = to->y - from->y;
	collision.material_type = NONE;
	moved = false;
	vector.k = to->z - from->z;
	if (function_1697c0(0x2490000f, from, &vector, ignore_object_index, ignore_unit_index, &collision))
	{
		if (collision.end_location.cluster_index != NONE)
		{
			vector3f direction;

			direction.i = from->x - collision.point.x;
			direction.j = from->y - collision.point.y;
			direction.k = from->z - collision.point.z;
			function_30bf0(&direction);
			result->x = direction.i * 0.01f + collision.point.x;
			result->y = direction.j * 0.01f + collision.point.y;
			result->z = direction.k * 0.01f + collision.point.z;
			moved = true;
		}
	}
	else
	{
		moved = true;
		*result = *to;
	}
	return moved;
}

void function_141590(transform4x3f const *in, transform4x3f *out);
byte *function_183fc0(long index);
struct s_1de2c1 { long field_0; long field_4[256]; };
struct s_1de2c2
{
    s_1de2c1 field_0, field_404, field_808, field_c0c;
};
bool function_1dde10(s_bsp3d const *bsp, short count, dword const *mask,
    point3f const *point, real radius, s_1de2c2 *hits);
struct s_245aa0;
struct s_source_245400;
struct s_collection_245270;
void function_245aa0(s_245aa0 const *hits, s_source_245400 const *source,
    transform4x3f const *matrix, real height, real thickness,
    long object_index, long position, s_collection_245270 *collection);

PRIVATE __forceinline void sweep_transform_point(transform4x3f const *matrix, point3f const *point, point3f *out)
{
    real x = point->x, y = point->y, z = point->z;
    if (matrix->scale != 1.0f)
    {
        x *= matrix->scale;
        y *= matrix->scale;
        z *= matrix->scale;
    }
    out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
    out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
    out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}
PRIVATE __forceinline void sweep_transform_vector(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
    real x = vector->i, y = vector->j, z = vector->k;
    if (matrix->scale != 1.0f)
    {
        x *= matrix->scale;
        y *= matrix->scale;
        z *= matrix->scale;
    }
    out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
    out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
    out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
}

// @retail 0x16a0a0
void function_16a0a0(long instance_index, dword flags, point3f const *point, real radius,
    real height, real thickness, s_collection_245270 *collection)
{
    real const &local_a47c5e = radius;
    s_168d60_bsp_view *bsp = (s_168d60_bsp_view *)g_4e0348;
    s_168d60_instance *instance = &bsp->instances[instance_index];
    byte *section = bsp->sections + instance->section_index * 0xc8;
    if (collision_surface_test((s_collision_result_view const *)section, instance_index, flags))
    {
        real extent = instance->radius + local_a47c5e;
        real dx = instance->center.x - point->x;
        real dy = instance->center.y - point->y;
        real dz = instance->center.z - point->z;
        real distance_squared = dz * dz;
        distance_squared += dx * dx;
        distance_squared += dy * dy;
        if (distance_squared <= extent * extent)
        {
            transform4x3f inverse;
            function_141590(&instance->matrix, &inverse);
            point3f local_point;
            real scale = inverse.scale;
            real x = point->x, y = point->y, z = point->z;
            if (scale != 1.0f)
            {
                x *= scale; y *= scale; z *= scale;
            }
            local_point.x = inverse.forward.i * x;
            local_point.x += inverse.left.i * y;
            local_point.x += inverse.up.i * z;
            local_point.x += inverse.position.x;
            local_point.y = inverse.forward.j * x;
            local_point.y += inverse.left.j * y;
            local_point.y += inverse.up.j * z;
            local_point.y += inverse.position.y;
            local_point.z = inverse.forward.k * x;
            local_point.z += inverse.left.k * y;
            local_point.z += inverse.up.k * z;
            local_point.z += inverse.position.z;
            s_1de2c2 hits;
            s_bsp3d const *geometry = (s_bsp3d const *)(section + 0x70);
            if (function_1dde10(geometry, 8, (dword const *)function_183fc0(instance_index),
                &local_point, scale * local_a47c5e, &hits))
                function_245aa0((s_245aa0 const *)&hits, (s_source_245400 const *)geometry,
                    &instance->matrix, height, thickness, NONE, NONE, collection);
        }
    }
}

// @retail 0x1691a0
bool function_1691a0(long instance_index, dword flags, dword test_flags, point3f const *point,
    vector3f const *vector, s_collision_result_1697c0 *collision)
{
    bool found = false;
    s_168d60_bsp_view *bsp = (s_168d60_bsp_view *)g_4e0348;
    s_168d60_instance *instance = &bsp->instances[instance_index];
    byte *section = bsp->sections + instance->section_index * 0xc8;
    if (collision_surface_test((s_collision_result_view const *)section, instance_index, flags) &&
        function_11e5e0(point, vector, &instance->center, instance->radius))
    {
        transform4x3f inverse;
        function_141590(&instance->matrix, &inverse);
        point3f local_point;
        vector3f local_vector;
        sweep_transform_point(&inverse, point, &local_point);
        sweep_transform_vector(&inverse, vector, &local_vector);
        s_collision_bsp_test_vector_result hit;
        if (function_1de630(test_flags, (s_slot_entry_list *)(section + 0x70), &hit, collision->t,
            8, function_183fc0(instance_index), &local_point, &local_vector))
        {
            collision->type = 3;
            short surface_index = hit.surface_index;
            short const *material_type;
            if (surface_index != NONE)
                material_type = &((s_1697c0_bsp *)g_4e0348)->materials[surface_index].material_type;
            else
                material_type = &g_47d8e0;
            collision->material_type = *material_type;
            collision->instance_index = NONE;
            collision->unknown3c = instance_index;
            collision->unknown40 = NONE;
            function_168b40((s_168b40_result *)collision, (s_168b40_surface const *)&hit, &instance->matrix);
            found = true;
        }
    }
    return found;
}

void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
struct s_line_list;
void function_244ca0(real height, real radius, s_line_list *list, long a, long b, long c,
    byte d, byte e, point3f const *position, short f);
bool __stdcall function_1f0230(s_lookup *lookup, point3f const *point, real radius,
    real height, real thickness, s_collection_245270 *collection);

// @retail 0x16a280
void __stdcall function_16a280(long object_index, dword flags, point3f const *point,
    real radius, real height, real thickness, long ignore_object_index, long ignore_object_index2,
    s_collection_245270 *collection)
{
    do
    {
        s_collision_object_header *header = &((s_collision_object_header *)g_4e0300->data)[object_index & 0xffff];
        s_collision_object *object = header->object;
        if (collision_object_test(object_index, header, object, flags, ignore_object_index, ignore_object_index2))
        {
            real extent = object->radius + radius;
            real dx = object->center.x - point->x;
            real dy = object->center.y - point->y;
            real dz = object->center.z - point->z;
            real distance_squared = dx * dx;
            distance_squared += dy * dy;
            distance_squared += dz * dz;
            if (distance_squared <= extent * extent)
            {
                switch (object->type)
                {
                case 0:
                {
                    point3f center;
                    real object_height, object_radius;
                    function_df5f0(object_index, &center, &object_height, &object_radius);
                    center.z += object_height;
                    function_244ca0(object_height + height, object_radius + thickness,
                        (s_line_list *)collection, object_index, NONE, NONE, 0, 0xff, &center, NONE);
                    break;
                }
                case 1:
                case 6:
                case 7:
                case 8:
                case 11:
                {
                    s_lookup lookup;
                    s_lookup *lookup_pointer = &lookup;
                    if (lookup_pointer->initialize(object_index))
                        function_1f0230(lookup_pointer, point, radius, height, thickness, collection);
                    break;
                }
                default:
                    break;
                }
                if (!(flags & 0x20000) && object->field_xf86fb0 != NONE)
                    function_16a280(object->field_xf86fb0, flags, point, radius, height, thickness,
                        ignore_object_index, ignore_object_index2, collection);
            }
        }
        object_index = object->next_object;
    }
    while (object_index != NONE);
}

extern long g_4e7c20[1024];
struct s_shapes;
struct s_shape_counts
{
    short count[3];
};

#include <xmmintrin.h>
#include <string.h>

PRIVATE __forceinline long shape_cluster_next(s_record_pool *pool, long *next)
{
    long result;
    if (*next != NONE)
    {
        long size = pool->size;
        byte *data = pool->data;
        byte *entry = data + (*next & 0xffff) * size;
        long following = *(long *)(entry + 8);
        if (following != NONE)
            _mm_prefetch((char const *)(data + (following & 0xffff) * size), _MM_HINT_T0);
        *next = following;
        result = *(long *)(entry + 4);
    }
    else
        result = NONE;
    return result;
}

// score 23 (was 170): retail keeps the world mask as a dword (`mov eax, dword; mov ebx, eax; and ebx, 1`, later
// `test ebx, ebx`) where ours loads a byte into bl, and retail loads the instance tick global into edx before the
// instance loop (reloading it after the call) where ours loads it at the loop top.
// Round 67 (m3 lane 13, plain): the leaf loop as a plain for over the hit count field 186 (873 bytes, frame 0x1014);
// with the cluster-reference step taking the pool and reading its size and data once 165 (890); with the instance loop
// as a plain for over a long instance indexed `(short)instance` 115; with the object mark index named 23 (885). On 23:
// the world mask as a dword or long 217/217. On 115: the mask as a dword 221, `flags & 1` at both uses 221, the
// instance test as a `!=` block 115 (same code).
// Earlier at 170: world-mask width, leaf induction and pool/cluster homes differ;
// integer query result 251f; paired world mask 205 plain; leaf step at read 222 plain;
// unsigned leaf/handle/count views and normal iterator inline 251f; direct count returns 249f
// (unchanged fast control 251; full 170).
// Earlier at 170: world-mask width and pool/cluster allocation still differ.
// saved instance predicate 297f; saved object predicate 298f.
// both predicates 318f; saved collection mask 293f.
// word hit ordinal 172f; full-width instance mask 291f.
// full-width object mask 291f; collection mask plus word ordinal 300f.
// External position/collection records 170/170; read-only pool/link views
// 170/170; native pool reference 170; word link slot 180; slot with read-only
// pool or external call receivers 180/180 (complete fast control 170).
// frame 0x1018 agrees; world mask and pool/cluster homes differ.
// BSP/cluster/count/list/both field views 170/170/170/170/170 plain;
// list reload 177; list/count cached and reloaded after each call 166 (hand reload, reverted in review); paired record views 170.
// Round 160 (m3 lane 6, fast over every marker in this file plus 0x16a0a0, 0x16a280, 0x1dde10, 0x245aa0, 0x16a8e0 and
// 0x19cb90: that image reads 23, the function alone reads 267 and with its two callers 187, so earlier single-address
// scores need re-measuring): the world mask as a dword 41 (retail's `mov reg, eax; and reg, 1` and `test reg, reg`, but
// the mask takes EDI and the BSP pointer EBX, the reverse of retail, with the pushes reordered), as a long 41, `flags & 1`
// at both uses 41, the dword mask declared ahead of the counts' clear 211 (901 bytes), the dword mask with the BSP global
// in a named local 41, a bool from `flags & 1` or `(bool)(flags & 1)` 23/23 (same code); the instance mark test with the
// tick first 23, as a `!=` block 23. Diagnostic (single-address image, base 267): the dword mask with the cluster half
// as a force-inline helper taking flags by value 228, the same as the dword mask alone there. Untried lead: the instance
// marks' flag, tick and table (0x4e7c18, 0x4e7c1c, 0x4e7c20) are three separate globals, while the cluster and object
// marks this function also uses are one storage block each; retail hoists only the instance tick out of its loop
// (reloading it after the call). The table is defined in src/unknown_1321f0.cpp (a held file), so it was not tried.
// @retail 0x16a440
bool __stdcall function_16a440(dword flags, point3f const *position, real extent, real height,
    real radius, long ignore_object, long ignore_parent, s_shapes *shapes)
{
    s_shape_counts *counts = (s_shape_counts *)shapes;
    memset(counts, 0, sizeof(*counts));
    bool test_world = (flags & 1) != 0;
    if (test_world || (flags & 0xc))
    {
        s_1de2c2 hits;
        extent += 0.0625f;
        bool found = function_1dde10((s_bsp3d *)g_4e0340, 256,
            (dword const *)(g_4ed280 + g_4686c4 * 32 + 1), position, extent, &hits);
        if (found && test_world)
            function_245aa0((s_245aa0 const *)&hits, (s_source_245400 const *)g_4e0340,
                0, height, radius, NONE, NONE, (s_collection_245270 *)shapes);
        if ((hits.field_c0c.field_0 > 0 && (flags & 4)) || (flags & 8))
        {
            ++g_4e7414;
            g_4e7411 = true;
            if (flags & 4) { ++g_4e7c1c; g_4e7c18 = true; }
            if (flags & 8)
            {
                ++g_4de2fc;
                g_4de2f8 = true;
                if (!(flags & 0x1fff0)) flags |= 0x1fff0;
            }
            s_1697c0_bsp *bsp = (s_1697c0_bsp *)g_4e0348;
            for (long leaf = 0; leaf < hits.field_c0c.field_0; ++leaf)
            {
                long cluster = collision_leaf_cluster(hits.field_c0c.field_4[leaf]);
                if (g_4e7418[(short)cluster] == g_4e7414) continue;
                g_4e7418[(short)cluster] = g_4e7414;
                if (flags & 4)
                {
                    s_1697c0_cluster *entry = &bsp->clusters[cluster];
                    for (long i = 0; i < entry->instance_count; i++)
                    {
                        long instance = entry->instances[i];
                        if (g_4e7c20[(short)instance] == g_4e7c1c) continue;
                        g_4e7c20[(short)instance] = g_4e7c1c;
                        function_16a0a0(instance, flags, position, extent, height, radius,
                            (s_collection_245270 *)shapes);
                    }
                }
                if (flags & 8)
                {
                    long next = ((long *)g_4de2e0)[(short)cluster];
                    for (long object = shape_cluster_next((s_record_pool *)g_4de2e4, &next); object != NONE;
                        object = shape_cluster_next((s_record_pool *)g_4de2e4, &next))
                    {
                        long index = object & 0xffff;
                        if (g_4de300[index] == g_4de2fc) continue;
                        g_4de300[index] = g_4de2fc;
                        function_16a280(object, flags, position, extent, height, radius,
                            ignore_object, ignore_parent, (s_collection_245270 *)shapes);
                    }
                }
            }
            if (flags & 8) g_4de2f8 = false;
            if (flags & 4) g_4e7c18 = false;
            g_4e7411 = false;
        }
    }
    return counts->count[0] != 0 || counts->count[1] != 0 || counts->count[2] != 0;
}

struct s_shape_header
{
	long unknown00;
	long unknown04;
	long unknown08;
	byte unknown0c;
	byte unknown0d;
	short unknown0e;
};
struct s_prism
{
	s_shape_header header;
	plane3f plane;
	real thickness;
	short axis;
	byte side;
	byte unknown27;
	long point_count;
	point2f points[8];
};
struct s_ray_shape_result
{
	s_shape_header header;
	real time;
	point3f position;
	plane3f plane;
};
struct s_camera_shapes_16a8e0
{
	short counts[3];
	byte field_6[2];
	byte field_8[0x4c00];
	s_prism prisms[256];
};
bool function_2469b0(s_shapes const *shapes, point3f const *start,
	vector3f const *direction, s_ray_shape_result *result);

real *function_120790(real *out, real const *plane, real const *point, short axis, byte side);

// score 606 (was 610): earlier 637, 640 and 552; frame 0xb964 agrees and the prism cursor now has retail's plane.j anchor; sum/zero homes, axis re-read and early idiv remain.
// Earlier first line: score 610 (was 610), the same status.
// b2C (11-06) retail walk, statement by statement against 0x16a8e0: every condition, field, constant, callee and argument order agrees (entry `radius > 0`, signed clamp to 8 and
// signed `% point_count`, side/perpendicular/along/separation sums in retail's order, inside and 0.01/0.0001 tests, normal path when the length is <= 0.01, ray hits at +0x14, `0 - radius` opposite ray,
// face sum below edge sum). Two fidelity fixes, both with retail evidence: the point reconstructions call the existing 0x120790 (retail inlines it: the two word table loads at 0x16aab5/0x16aabc and
// the axis and side re-read for the second point at 0x16ab14..0x16ab22) instead of a hand-copied body, and the accepted counts are 32-bit (`inc dword ptr` 0x16b274/0x16b2ca, `test eax, eax; jle` 0x16b2fa/0x16b367,
// `cvtsi2ss` of the register, no `movzx`). Neither changes behaviour (the counts stay below 32768); score 552 -> 640 (2917 bytes), helper alone 651, long counts alone 572.
// Still differs: retail re-reads the axis after the first point's stores (ours keeps it in a register), starts the `%` before the first reconstruction, and the sum homes (face +0x50, nearest +0x5c, edge +0x60).
// Tried with the helper: pair struct for first/second 662, `next` index named first (+ long counts) 676.
// MiniMax C (11-05): no new form, no lower score; retail's prism loop walks &prism->plane.j (plane.i at [esi-4], the point count at [esi+0x14]) where ours walks &prism->point_count (plane.i at [esi-0x18]), and our accumulator homes sit sixteen bytes above retail's.
// Earlier at 637 (was 640; a second score line, merged in round 185): earlier 552; frame 0xb964 agrees; sum/zero homes, axis re-read and early idiv remain.
// frame 0xb964 agrees; sum/zero homes, axis re-read and early idiv remain.
// frame 0xb964 agrees; sum homes and EBX/ESI zero carrier remain.
// Round 30 (lane D): the permuter's best variant reorders some declarations and swaps the face-sum
// product (540 -> 539 on the reduced image, one point, below the retention margin). Retail multiplies
// the face sum by the inverse (mulss xmm1, xmm0), the order this body already uses, so the swap is
// not retail's form. Original body kept.
// Earlier: frame 0xb964 agrees; sum homes and EBX/ESI zero carrier remain; the prism cursor is
// anchored at point_count (retail at plane.j, prism + 0x14: every prism displacement differs by 0x14).
// Retail keeps the zero in ESI (pushed first, reused as the prism pointer, based at plane.j where
// ours bases it at the point count) and counts the accepted shapes in 32-bit counters (test/jle,
// cvtsi2ss). Round 152: separate face/edge sum points 594; long counters 572 (2890 bytes); plane
// pointer local for all plane reads 554 (compile-only probe moves the base to plane.k); both long
// counters and separate sums 578. The seven no-op parameter-address lines dropped (identical, kept).
// sum homes and zero register; accumulation loops 597;
// average loops 554; midpoint loop 554; branch-local ray results 552;
// face classification from normal selection 553.
// frame 0xb964 agrees; sum/counter homes and EBX/ESI zero carrier still differ.
// Result-first edge cap 625; long inside 600; long normal selection 552; projected entry lifetime 552; independent sums with word counters 594; shared initial point 553.
// frame 0xb964 agrees; sum homes and query zero register differ. Signed
// accepted counts 552; selected endpoint pointer 653; vertex cursor 628; shared inverse 552; early
// opposite radius 647; word-count shared reconstruction and early successor 686.
// Entry direction 552; prism-scope contact points 552; contact/half role names 552;
// sum memcpy 552; full-width reconstruction axis 593 at retail length.
// DL134: short/long projection indices 621/553; polygon count 555; normal component 647;
// axis at use 744; coordinate inputs 651; short indices/inputs 686; polygon/plane inputs 660.
// Native projected point, relative/edge vectors, side, perpendicular, candidate,
// separation vector, contact midpoint/half and all native producers: 552 each. All restored.
// Round 227: Native prism/query-origin/initial-sum values, their pairs/all,
// all with CRT prism construction 779/758/554/886/779/758/886/886. All restored.
// Round 369: actual clamped point candidate/first/second defaults 628/623/628; with nearest result 629/624/629; candidate/second with completed face result 627/627. All restored.
// Round 235: query/candidate/contact consumption scopes, all seven combinations 552 each; midpoint-half consumption scope 552.
// Round 201: byte inside/face/normal decisions, all seven nonempty subsets, and all three signed
// bytes 552 each (2898/2882). Original canonical bools, accepted word counts and geometry restored.
// Round 226: Plain component loops for paired edge inputs/contact direction/residuals and
// their pairs/all [672, 594, 556, 680, 674, 594, 682]; all with the three ray scales 660. All restored.
// Round 174: reconstruction prism/point/output reference contracts in all seven
// subsets 552; all with the actual point value 712 and frame 0xb970; original helper restored.
// Frame 0xb964, sum homes and EBX/ESI query zero discrepancy remain.
// DL167: Existing scaled-add projected/candidate and shared ray-fallback add producers, seven
// subsets 553/552/553/609/610/609/632; all with in-place vector sums 623. Original
// producers, real inputs, word counts and helper restored (fast control/full baseline 552).
// Round 249: shared final accepted-sum pointer with word/short/long/dword count
// 612/612/612/619; native sum value with those domains 602/602/602/598.
// Original separate averaging branches, count domains and all geometry restored.
// Round 225: projected offset/candidate displacement/both/squared lengths/offset-lengths/
// displacement-lengths/all/all with three shared ray scales 618/555/553/554/618/555/617/553.
// Original expression producers and all geometry/helper inputs restored.
// Round 267: query count long/short 577/623; outer prism cursor live/long/short
// 638/662/662; unsigned ordinal live/long/short 552/581/623. Original
// query-count observations, signed outer ordinal and complete geometry restored.
// Round 415: native plane value and staged side/perpendicular crosses, seven subsets
// 734/553/734/603/707/603/707; all with complete normalization/ray assignments 707.
// Original plane observations, product producers and complete geometry restored.
// Round 424: complementary polygon/normal/contact decisions, seven subsets
// 553/552/553/554/555/554/555; all with full-width polygon 600. Original
// decision values, geometry and helper calls restored; sum homes and query zero remain.
// Round 434: nearest-edge/radii/squared-limit scalar lexical homes, all seven
// subsets 552 each; complete names with accepted radii before accumulator storage 551
// (one-point gain rejected). Original scalar homes, word counts and geometry restored.
// Round 238: point-count-owned upper cap with word/face-long/edge-long/both-long
// accepted counts 605/561/567/587; each with readonly prism 605/561/567/587.
// Original cap, accepted word counts, prism view and complete geometry restored.
// Ray-result-owned positive/negative endpoints and shared ray, seven subsets, then original endpoints with one
// query result: 613/600/638/571/634/619/661/581. Original complete executable source restored.
// Round 47 (lane 10, fast with 0x120790 listed): long counters each declared beside its radius (edge then face)
// 572, face then edge 571; word counters beside their radii 552 (same code); the count clamped by `if (count > 8)`
// 605; the distance through the existing plane_distance_to_point 620; both 614, and a plane pointer local for the
// distance with the `if` clamp 614: these two give retail's plane.j cursor anchor (compile-only agrees: neither
// alone does, and no spelling or term order of the distance moves it) but drop the frame to 0xb958 and keep the
// dot product summed (x*i + z*k) + y*j (all 48 term orders compile the same) where retail has (x*i + y*j) + k*z;
// retail also keeps the ternary clamp's select (`mov ecx, 8; jg; mov ecx, eax`).
// J218: Native point setters for paired sum initialization/averages/accumulations, seven subsets, then result before query storage: 646/552/646/553/646/553/646/646. Original complete body restored.
// Round 53 (lane 10, fast with 0xc2d00/0x17a380/0x16a440/0x2469b0/0x120790): retail keeps the sums apart
// (face at +0x54, the nearest distance at +0x60, edge at +0x64), counts and radii as dwords at +0x88..+0x94,
// computes `(edge_index + 1) % point_count` at the top of the edge loop (stored as the next index) and
// reloads the axis and side for each point reconstruction. Every prism read straight from
// `shapes.prisms[index]` (no prism pointer) 686 (2956 bytes); only the plane reads that way 587 (2892, cursor
// still at the point count); the next index named at the top of the edge loop 602 (2909); both
// reconstructions through the existing 0x120790 (inlined: retail's word loads of the two table indices and the
// second point's +0x9c home, but the first point's home and the axis register differ) 651 (2925); that with
// the next index named first 686 (2955).
// Round 43 (lane 14, fast): long face/edge counts (retail's `test eax, eax; jle; cvtsi2ss`) 572 (2890 bytes,
// re-measured), as edge count/radius, face count/radius pairs 572, face pair first 571; the edge loop's next
// index named at the loop top before the first point (retail's early inc/idiv) 602 (2909), with long counts 597.
// Round 211: existing double-factor scaler for both averages/paired direction rays/normal
// and combinations 668/651/591/670/660/629/638; all plus count-produced result 661.
// Complete products reach 2882 bytes at 638 but disturb matched callers. Original word
// counts, sums, ray producers, result, include and body restored; sum/query-zero homes remain.
// Round 66 (lane 9, fast): retail's four 4-byte count/radius homes sit together as count, radius, count, radius
// (frame 0x88-0x94) and the prism cursor is based at plane.j. Tried: the prism fields read as
// `shapes.prisms[index].field` with no prism pointer 686 (2956 bytes, zero to EBP, no cursor); the distance through
// the existing plane_distance_to_point 620 (frame 0xb958); long counters declared count/radius by kind 571 (2890
// bytes); word counters declared that way 551 (one point); the loop without the `counts[2] > 0` guard 555.
// Round 298: Independent sums/shared normalization with long counts, point-count-first cap and
// shared plane distance, masks 0..7: 591/575/593/579/613/605/551/569. One-point gain rejected;
// original sums, word counts, normalization, cap, plane producer and body restored.
// Sum/query-zero/cursor homes remain.
// Round 30 (lane D): the permuter's best variant reorders some declarations and swaps the face-sum
// product (540 -> 539 on the reduced image, one point, below the retention margin). Retail multiplies
// the face sum by the inverse (mulss xmm1, xmm0), the order this body already uses, so the swap is
// not retail's form. Original body kept.
// Round 55 (lane 10, fast; 0x120790 and 0x16a440 listed, unchanged): both point reconstructions through the
// existing 0x120790 (retail inlines it here) 651 (2925 bytes); with long counters 640 (2917); with the distance
// through plane_distance_to_point 644 (2918); with both 634 (2910); long counters and plane_distance_to_point on
// this body 610 (2887 bytes, frame changes at +0x1).
// Round 441: shared two-use initial sum pointer/radius-result/edge-first accumulation subsets
// 553/552/553/552/553/552/553; complete graph with native long accepted counts 572.
// Original complete body restored; sum homes and query zero ownership remain.
// Round 445: Native double radius-minimum/nearest-separation/normalization result domains,
// seven subsets and full-width-count graph: 651/641/678/621/643/681/697/763.
// Original complete executable body restored.
// Round 451: Edge-reconstruction endpoint declarations at entry/radius/prism/producer
// scopes, original and complete endpoint role names: 552/552/552/552/552/552/552/552. Original declarations, names and
// complete body restored; sum homes, query zero and prism anchor remain.
// Round 241: Native cross-product/point-difference/edge-query direction storage,
// singles/pairs/all 580/552/553/580/581/553/581; all with long accepted counts 592.
// Original vector storage, accepted word counters, helper and complete geometry restored.
// Round 464: bounded short/word/char/byte inner-edge ordinals with cached/live caps:
// 602/602/593/593/619/619/627/627. All regress; original long ordinal, cap sample, word counts and geometry restored.
// Round 38 (m3 lane 2, fast): retail keeps the sums at F+0x44 and F+0x54 with the nearest distance between
// them at F+0x50. Separate face/edge sum locals 594 (2896 bytes; the first sum at retail's F+0x54, the second
// 4 bytes high); that with the edge sum renamed to sort first 594 and with the declarations swapped 594 (same
// code each: names and declaration order do not move these slots); the nearest distance declared in the
// prism loop at its reset 552 (same code), with separate sums 594.
// Round 51 (m3 lane 13, fast): the count clamp as `point_count < 8 ? point_count : 8` 552 (same code), as
// `count = 8; if (point_count <= 8) count = point_count;` 625 (2912 bytes); `inside = true` before the nearest
// reset 552 (same code); a const prism pointer 552 (same code); closest declared before the inside flag 552
// (same code). Retail's clamp is a select into a second register (`mov ecx, 8; jg; mov ecx, eax`).
// Round 528: paired radius result selections, point-before-nearest commit and both
// 552/552/552; with long accepted counts 572/572/572; both with
// independent sums 594; independent sums and long counts 578. Original whole body restored.
// AC499: Both count-zero producers after query, word/short/long/dword, declarations
// at producer or uninitialized before query: 552/552/572/585/552/552/572/585. Query zero stays EBX;
// original counters, producer boundary, geometry, helper and complete body restored.
// Round 129 (m3 lane 8, fast): retail's plane distance follows the text order (x*i, y*j, then z*k) and loads plane.j
// early for the projected y; ours reassociates to x*i, k*z, j*y. The count clamp as an if statement 605 (2902 bytes);
// the distance's third product written point-first 552 (same code); the nearest distance declared in the prism loop
// 552; the existing plane_distance_to_point for the distance 620 (2895); count/radius declarations interleaved
// (edge count, edge radius, face count, face radius) 552. Original body restored.
// Round 64 (lane 10, fast only, under a CAVEAT for callee 0x16a440, so these are not plain scores): retail's table reads are 0x120790's inlined shape, `(edge_index + 1) % count` runs before the first reconstruction and axis
// is read again for the second. The existing 0x120790 for both points 651 (2925, masked aligned 206, against 194 here); with long counters 640 (2917, masked
// 202); the distance through plane_distance_to_point 620 (2895, masked 222); every prism read through shapes.prisms[index] 686 (2956, cursor still point_count);
// a plane pointer local 554 (2896, masked 199); j product first 552 (same code); a point cursor for the first reconstruction 629 (2924); count before the inside flag 552 (masked 195).
// Round 59 (m3 lane 13, fast): retail's direction length sums i*i, k*k, j*j after an fstp of k (ours keeps k on the x87
// stack and starts with it in every form). The direction normalize through the existing normalize_inline 554; that
// with the direction through vector3d_from_points3d 554; through normalize3d 554; vector3d_from_points3d for the
// relative, edge, difference, direction and half vectors 680 (2951 bytes); for the half alone 554; the difference
// alone 553; relative and edge 671 (2947). Original body restored.
// AC504: Radial/side/nearest loop continuations, masks 1/2/4/3/5/6/7:
// 552 each; complete continuation graph with native long accepted counts 572.
// Original full body restored; sum homes, query zero and prism anchor remain.
// AC512: Aggregate-declaration sum initialization with word/long accepted counts,
// counts after/before construction 553/553/573/573; independent point declaration
// initialization 594/594/578/578. All restored; native sum homes/query zero/prism
// anchor remain. Coordinate-table owner stays matched in every trial.
// AC515: Direction-length-owned normal-path decision bool/byte/short/long, with length at
// direction phase or uninitialized radius scope: 613/613/613/613/613/613/613/613.
// NOT kept: unmatched decision/storage graph; original flags, normalization and complete geometry restored.
// AC519: Standard forward/backward edge/face/both initial-sum transfers, then
// both with overlap-safe fallback copies: 655/650/591/655/650/591/623/623.
// NOT kept: all regress and lengthen; original sum inputs and complete body restored.
// AC524: Standalone count-produced success bool/byte/short/long before averaging,
// then after output: 654/657/653/654/630/654/660/657. NOT kept: all regress and
// move extent away from retail; original result, word counts, geometry and body restored.
// AC526: Entry-radius admission bool/byte/short/long, then complementary exclusion:
// 620/620/620/620/619/619/619/619. NOT kept: all regress, frame shrinks to 0xb958 versus
// 0xb964; original body/helper restored; sum homes, query zero and prism anchor remain.
// M3 lane 12 r90 (fast, base 552, masked 446): the cursor anchor follows which fields are read, not their order: no point-count read in the cap 613 (anchor prism+0x20),
// no read in the modulo 620 (still point_count), neither 618 (anchor plane.k), a local points pointer 598 (axis), the named index with no cap read 621, retail's anchor plane.j not reached; distance with the j term first and
// `shapes.prisms + index` 552 (same code). The cap reading the count once 554, as `if (count > 8)` 605; (edge_index + 1) % point_count named before both reconstructions, retail's early idiv, 602
// (2909, masked 400), with the single read 602; short index locals in the reconstruction 621; combinations 622/662/661/657; a long axis 593 at retail's 2882 bytes. Retail's sum homes are not contiguous
// (+0x5c/+0x64/+0x68, +0x50/+0x54/+0x60), but plain scalar sums 630 (2897), face first 630, with the named index 619/630, two point3f locals 594.
// AC537: Complete near-plane/interior direction-admission truth, positive and
// complementary bool/byte/short/long: 553/553/553/553/553/553/553/553. NOT kept: no match; original
// normal selection, sums, word counters, geometry and whole body restored.
// AC539: Nearest-edge improvement result, positive bool/byte/short/long 553 each,
// complementary forms 554 each (control 552). NOT kept: unmatched decision storage;
// original nearest commits, sums, word counts, geometry and full body restored.
// AC542: Existing paired normalization/contact and edge/separation squared-length producers,
// paired side/projection dot producers, seven subsets and full-width accepted counts: 554/552/554/554/554/554/554/573.
// NOT kept: no byte equality for changed reduction associations; original full body restored.
// AC544: Exact-association plane-dot offset, paired root operands and perpendicular-dot
// negation boundaries, seven subsets then immutable parts: 554/554/554/553/554/554/554/554.
// NOT kept: all regress; original reductions, counters, observations and whole body restored.
// M3 lane 9 r88 (fast; masked+stack = aligned count with registers and stack homes masked, base 310): retail has no count guard (`cmp word ptr [esp+0x16c], si`, then the index store, then `jle`) and computes
// radius * radius inside the loop (hoisted to the preheader): plain `for` with the product inside 554 (2901, entry shape as retail), the product inside alone 555. The next index named before the first point with
// 0x120790 for both points 686 (2955, masked+stack 280: retail's axis re-reads, table reads and early idiv appear), with the file's helper 602 (293), plain for + next + 0x120790 669 (2942, 282), + the file's helper
// 606 (294), plain for + 0x120790 without next 654 (342); 0x120790 alone 651 (339). Lead: those three together fix most of the inner edge loop; what remains first is the zero in ESI (retail) against EBX and the cursor at plane.j.
// Round 75 (m3 lane 10, fast with 0xc2d00/0x17a380/0x16a440/0x2469b0/0x120790; control 552): retail's prism reads at the
// loop head are plane.i, plane.j (the cursor base), plane.j again, point_count, plane.k, plane.d; ours plane.i, plane.k,
// point_count (our base), plane.j, plane.d. The count clamp before the distance 553, after it 553; the projected
// point through a named negated distance 618 (2911 bytes); `inside` set before the nearest default 552; a const prism
// pointer 552 (same code). No read order tried moves the cursor base.
// M3 lane 11 r72 (fast, control 552; "masked" = register/immediate-masked aligned count, control 309): both points through the existing
// 0x120790 re-measured 651 (2925, masked 334: retail's word table reads appear, but ours reads the axis once into BX where retail re-reads it per
// point); long counts renamed so the names sort as retail's dword homes 572 (same code as plain long counts: ours edge radius +0x5c, edge count
// +0x7c, face count +0x80, face radius +0x84; retail face count +0x80, edge radius +0x84, edge count +0x88, face radius +0x8c); no anonymous struct,
// `point3f face_sum; real nearest_squared_local; point3f edge_sum;` 594 (2896: edge sum at retail's +0x5c, face sum +0x50 against +0x4c, nearest
// stays +0x6c against +0x58); long counts + 0x120790 for both points + no outer guard + `radius * radius` in the test 640 (2918, masked 335);
// projected declared ahead of distance 552 (same code); `return true`/`return false` with no result local 593 (2901, first difference +0x13).
// AC552: Existing reconstruction output as real pointer/array reference, point/real pointer
// return and point/array reference return: six at 552; native value result ordinary/forced 769/709.
// NOT kept: no gain; value results 2740/2975 bytes, frame 0xb97c versus 0xb964. Original helper/body restored.
// AC554: Paired reconstruction plane-axis degeneracy truth in bool/byte/char/signed-char/
// short/word/int/long 553/553/553/553/553/553/553/553. NOT kept: no exact test-producer repair;
// original helper and body restored.
// AC556: Complete output-path-produced returned status in bool/byte/char/signed-char/
// short/word/int/long: 589/635/635/635/652/652/647/647. NOT kept: all regress and
// lengthen; original entry result, fallback writes, geometry, word counts and full body restored.
// AC558: Complete post-query prism-presence/rejection results in bool/byte/short/long:
// eight at 553, 2898 bytes (control 552). NOT kept: unmatched one-consumer outcomes;
// original signed count guard, all live count reads, sums and full body restored.
// m3 lane 3 r227 (fast; plain for the kept form): kept the two sums as separate point3f locals in place of the anonymous
// two-member grouping 640 -> 637 (2915 bytes, plain-confirmed). Retail computes `(edge_index + 1) % point_count` at the top of the
// edge loop (EAX = edge_index + 1, shared with the increment and spilled at +0xd0) and holds the remainder in EDX through the
// first reconstruction; that pressure is what makes it re-read the axis and side for the second point. Measured on the grouped
// base: separate sums with no outer guard (product in the test) 640 (2918); the named next index with separate sums 625 (2922,
// frame changes at +0x1, aligned 735 -> 587), with no guard too 626; the second point's address first with separate sums and no
// guard 617 (2908); no guard alone 644; the named next index alone 676 (2947). The next index and the point address are one-use
// values moved ahead of a call: match-only. On the kept form: the two-use points pointer 630 (2909: lines -1, length -6, frame
// +0x1; re-measure of the listed 622 form); the point count read once for the cap and the modulo 637 (same code).
// Then on the kept form (fast): no outer guard with radius_squared still named before the loop 639 (2918); the near-plane test
// written `fabs(distance) >= 0.01f || !inside` 640, the length test `fabs(magnitude) >= 0.0001f` 640; separation as
// `inside ? 0.0f : nearest` after the closest copy 642; `face = false` after both edge rays 661 (2939). The second point's
// address first with no guard gives retail's entry exactly (frame 0xb964, ESI zero, `cmp word ptr [esp + 0x16c], si`): the zero
// register follows the edge loop's pressure from the early remainder.
// Round 163 (m3 lane 6, fast; aligned/masked = difflib counts with addresses, or also registers and stack offsets,
// masked; control 735/337): the call list already agrees (0x120790 inlined in both). Retail starts the `%` and the
// next index (`inc eax`, stored for the back edge) at the loop top and re-reads axis and side for the second point.
// Both point pointers taken before the two reconstructions (`point2f const *start = &prism->points[edge_index];`,
// `end` likewise with the `%`) 620 (2902 B, 684/319: retail's idiv placement and axis/side re-read; one-use
// pointers, kept only with a byte match); on it the plane distance terms written k, j, i 620 and projected y as
// `(0.0f - distance) * plane.j` 620 (both same code: the sum order follows the prism cursor's anchor, ours at the
// point count, retail at plane.j); the reconstructed points renamed edge_start/edge_end 620 (same slots). The
// listed `next` index named first re-measured 676 (2947 B, 616/275: retail's loop-head sequence exactly, but the
// side/perpendicular rows spill two XMM values). Scores below split as differing lines + length term (base 605 + 35):
// the distance through the existing plane_distance_to_point 634 (606 + 28, frame kept); that with a two-use
// `point2f const *points = prism->points;` before the edge loop 629 (2903 B, frame 0xb960); that local alone 622 plain
// (607 + 15: the drop is the length term only; it removes retail's spilled point cursor `lea ebp, [esi + 0x18]` at
// +0x98 and its slot, frame 0xb960 versus 0xb964, not kept); the local at the prism's scope 597 (587 + 10, 2872 B,
// frame 0xb954, aligned 793/masked 388, not kept); X1's pointers count 600 + 20. A `next` index used for the `%`
// and as the loop step (`edge_index = next`) 652 (2929 B, 640/296, frame kept); only the `end` pointer taken first
// 620 (same code as both pointers). Outside the edge loop: the direction magnitude through sqrtf 641; the midpoint
// rows as `0.5f * (negative + positive)` 640 (same code); radius_squared declared before the query call 624 (607 + 17:
// the product moves ahead of the call where retail forms it after the count test, the zero moves to EBX; not kept);
// the magnitude through length_sq3f 640 and the midpoint through real_point3d_set 640 (both same code). All restored.
// Lane 10 r91 (fast; masked = aligned lines with registers and stack offsets masked, control 637 [337]): retail lays out the ray
// part as edge-positive cast, the normal-path cast, then the edge-positive fallback (`point + ray`) and the negative cast (ours
// keeps the edge arm whole before the normal arm). The nearest reset at the top of the prism loop 636 [336], `inside` there 637,
// both 636; on the nearest-first body, `edge` computed before `relative` 603 (2875 B, frame 0xb958 [340]: the drop is the length
// term, not more agreeing lines), with the side test's arms swapped 590 (2877 B [365]), the clamped point as a pointer to
// first/second/candidate 633 (2860 B [374]). On 603: the clamped pointer 672, the endpoints declared at prism scope 603, edge
// first alone on this body 604. On 590: the clamped pointer 673, without the nearest move 591, `inside` first too 590. All are
// statement moves or arm swaps (kept only with a match), and the masked count rises, so the body is restored.
// R229 (m3 lane 3, fast with 0x16a440/0x120790/0x2469b0): re-measure of the listed face classification from the normal
// selection (553 then) on the current body: the separate face flag dropped and the final test made on `use_normal` (the
// flag only ever mirrored it) 637 (2915 bytes; now a tie, kept as a clean-up, plain 637); one `ray` vector declared for
// both ray arms (close to the listed shared-ray subsets) 629 (2896 bytes, but aligned 717 -> 778: positional only, and a
// declaration merge picked by score: not kept); one hit record for all three ray calls (close to the listed one query
// result) 652 (2920); the two root sums written i, j, k 637 each (same code); `available` through the existing
// distance3d(&positive, &middle) 637 (2915), the three rays through the existing vector3f_scale 637 (2914), both 637.
// Round 185 (m3 lane 4, fast with 0xc2d00/0x17a380/0x16a440/0x2469b0/0x120790): existing helpers in place of
// hand-written bodies. The perpendicular as `cross3f(&edge, &prism->plane.n, &perpendicular)` 637 -> 628 (2908 bytes),
// the side cross through cross3f too 628 (same code; alone 637); the side test as `dot3f(&prism->plane.n, &side) <
// 0.0f` 628 -> 610 (2890 bytes, aligned 459 -> 409, register-masked 299 -> 255: the prism cursor moves to retail's
// plane.j anchor and the side test is retail's instruction sequence). Re-measured on the 637/610 base (the cursor anchor
// and code changed, so AC542's, R163's, R229's, Round 59's and DL167's results no longer apply): the amount through
// dot3f, `length_sq3f(&edge)` and the separation through length_sq3f 610 (a tie, kept), the magnitude through
// length_sq3f, the normalize and the three rays through vector3f_scale 610 (kept); relative and edge through
// vector3d_from_points3d 613 (2885, aligned 482), the difference 614, the half 623, the distance through
// plane_distance_to_point 623, the ray fallbacks through vector3f_add 627. New on 610: `available` through length_sq3f
// 610 (kept); the projected point through this file's point_along_vector_1696d0 635, the candidate 629, both 627; the
// two averages through vector3f_scale 610 (same code), the accumulations through vector3f_add 622 (aligned 393); no
// `opposite` local 614; no inverse local in the normalize 614; the normal arm's call test negated with the arms
// swapped 610; the accumulation arms swapped 610; use_normal declared first 610 (same code); negative declared before
// positive 610 (same code). Match-only on 610: `(edge_index + 1) % prism->point_count` named at the loop top 610 (2912,
// fully masked 151 -> 114: retail's early idiv), the second point's address taken first 549 (2876, fully masked 138),
// both one-use values moved ahead of a call; `negative = *point;` ahead of the normal arm's ray 606 (2887, aligned 398, a
// statement order). Left: the early remainder, the face/edge sum and count homes, the zero in EBX for ESI and the
// scheduling around the two ray arms.
// Round 188 (m3 lane 4, fast with 0xc2d00/0x17a380/0x16a440/0x2469b0/0x120790; aligned/masked = difflib counts, control
// 409/253): the ray arms as `if (use_normal) { normal } else { edge }` 608 (2872 B, 380/219: retail's block order exactly,
// edge-positive cast, normal arm, edge-positive fallback, negative cast, because the threaded flag test pulls the first
// edge cast up and the rest follows the IL order; an arm swap, so match-only). On it, re-measured: the next index named at
// the loop top 595 (2903 B, 325/213: retail's loop head exactly, the clamp select, `xor eax, eax`, early inc/idiv, the
// zero in ESI; frame 0xb968), and with that no outer guard plus `radius * radius` at its use 547 (2888 B, 304/205; the
// last alone 612, with the swap only 603, with the next index only 595). On that 547 composite the frame's extra 4 bytes
// are one XMM spill in the side cross (relative.i and relative.j take each other's registers); there scalar sum locals
// 618 (0xc2d00 and 0x17a380 break), `relative` through vector3d_from_points3d 591, `edge` through it 600. On the kept
// body: `edge` alone through vector3d_from_points3d 628 (2907); `relative` alone 591 (2885, positional only: frame 0xb94c,
// 463/287, and one of two parallel vectors, not kept); the side cross as `cross3f(&relative, &edge, &side)` tested `>
// 0.0f` 609 (same 420/253); `direction` through vector3d_from_points3d 627; the arm swap with relative and edge through
// it 612. On the kept body (fast): `separation` zeroed in the `inside` arm 610 (code differs); `half` as positive - middle
// 599 at 2882 B (positional: 3 fewer differing lines, aligned 405/masked 256, and retail computes middle - positive);
// `magnitude > 0.01f` tested after the normalize block 630; `amount = dot3f(...)` with rows `projected.x -
// perpendicular.i * amount` 644; projected rows `point->x - distance * prism->plane.i` 602 (2887 B, positional: 420/267,
// and retail forms 0 - distance); same code as the body (610): candidate rows projected first, the radius minimums as
// ternaries, fallback z rows `point->z + ray.k`, midpoint rows positive first, `separation < nearest_squared_local`,
// the magnitude through dot3f, projected as an aggregate initializer. Code-changing on the body: a separate opposite_ray
// 651 (2919 B); face_hit for the negative cast 610; hit for the normal arm 629; `closest = projected` before the edge
// loop with `separation = inside ? 0.0f : nearest_squared_local` 603 (2887 B, 598 differing lines; retail copies only
// when inside); the distance as `dot3f(&prism->plane.n, (vector3f const *)point) - prism->plane.d` 623; length_sq3f(&edge)
// named before the clamp chain 623; the nearest update's statements swapped 610; the edge-positive fallback through
// point_along_vector_1696d0 689; the counts incremented before the radius updates 627. Same code: dot3f arguments
// swapped in the side test and the amount, the two hit records swapped. Left: the composite needs two match-only forms
// (the arm swap and the early next index); no untried natural form found for the side-cross register choice or the
// sum homes.
// T95: Twice-used successor for modulo and back edge on the retained shared-cross graph,
// long/int/short/word in plain for and while traversals: 643/643/641/641/643/643/641/641.
// NOT kept: every form regresses, changes incoming argument registers at +0x13 and
// lengthens to 2920/2922; original signed bounds, geometry and body restored.
// T97: One shared fallback owned by the existing return result, bool/byte/char/signed-char/short/word/int/long:
// 645/619/619/619/614/614/616/616. NOT kept: all regress; original separate fallback paths,
// geometry, counter observations and complete body restored. Sum homes and ESI zero still differ.
// T100: Actual first/second/paired/projected/candidate/clamped/closest/all coordinate
// array receivers with complete library point transfers: 614/614/613/614/614/614/614/613.
// NOT kept: all lengthen and leave native sums/query-zero/early remainder unresolved.
// Original point workspaces, helper calls, geometry and complete body restored.
// P443: Scaled-difference edge/contact pairs, complete normalizer, separation/half setters,
// complete along differences/candidate products, native-count result and radius-owned result:
// 611/620/637/614/623/623/646/630. NOT kept: no native sum/zero/remainder repair; original geometry and helpers restored.
// P446: Actual lower and upper endpoint admissions in bool/byte/short/long, preserving short-circuit edge-length evaluation;
// 623/623/623/623/623/623/623/623. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// M3 lane 10 r101 (fast with 0xc2d00/0x17a380/0x16a440/0x2469b0/0x120790; brackets = difflib counts exact / registers masked /
// registers and stack offsets masked, control 610 [584/398/285], 634 instructions against retail's 629): nothing kept. Lane 4's
// composite re-measured on this body: the ray arms as `if (use_normal) { normal } else { edge }` 608 (2872 B) [497/327/209]; with
// the next index named at the edge loop's top 595 (2903 B) [394/300/144]; with no outer count guard and `radius * radius` at its
// use as well 547 (2888 B) [363/295/128], 629 instructions as retail. New: 0 to 7 unused longs ahead of that composite
// (symbol-count phase scan) 547/593/593/593/582/585/585/585, and ahead of this body 610/623/623/623/613/614/614/614: phase 0 is
// the best phase of both, so the remaining float rows are not a phase effect. Without 0xc2d00 and 0x17a380 in the fast list the
// callee 0x16a440 changes convention (23 -> 187) and this function's call code with it. Frame homes relative to ESP after
// the stack probe: retail face sum 0x44, nearest 0x50, edge sum 0x54, face count 0x84, edge radius 0x88, edge count 0x8c, face
// radius 0x90; a no-link compile of this file (frame table from the assembly listing) puts ours as face sum, edge sum, edge
// radius, edge count, face radius, face count, nearest in one run, whatever the names or declaration order.
// P449: Face/edge radius-improvement and final face/edge count-admission results in bool/long:
// eight 623s. NOT kept: no native accumulator-home repair or byte equality; full original body restored.

// P452: Conditional endpoint value, face/edge real divisors, closest/clamped/query-position/normal-input memmoves, native prism endpoint traversal:
// 620/623/623/719/682/649/610/641. NOT kept: no gain; native accumulator/query-zero homes remain. Original record copies, counts, geometry, helpers and full body restored.
// P454: Isolated function storage for side/perpendicular/candidate/clamped/difference/direction/middle/half
// on the retained long-count/shared-cross graph: eight 610s. NOT kept: no sum/zero/remainder repair;
// every original producer, declaration and helper restored.
// m3 lane 7 r159 (plain; fast prints a reduced-image CAVEAT here): kept, the `shapes.counts[2] > 0` guard that repeated the
// prism loop's own test removed 606 (aligned 584 -> 574). Tried on that: the nearest-distance local declared where the prism
// loop sets it 610 (same code, on the original body); the two sums declared with their initializers 606 (same code); the reach
// test as `separation < radius_squared` 606 (same code); the one-use `opposite` folded into its vector3f_scale call 610 (2895 B);
// the two counts declared after the shape-query call 590 (2874 B; a moved declaration, not kept without a match); one ray-result
// local for both branches 627 (frame changes). The zero carrier stays EBX (retail ESI) and the sums sit 4 bytes off.
// P477: Standard first-product-seeded side/amount/edge-length/separation/direction-root/available-root reductions, paired minima and paired sum transforms:
// 661/751/681/697/644/673/639/637. NOT kept: all regress and lengthen, with no native accumulator/zero repair; all four matched controls survive. Original observations, kernels, geometry and complete body restored.
// @retail 0x16a8e0
bool __stdcall function_16a8e0(long flags, point3f const *point, real radius,
	long ignore_object, long ignore_parent, point3f *out, real *out_radius)
{
	s_camera_shapes_16a8e0 shapes;
	bool result = false;
	if (radius > 0.0f)
	{
		point3f face_sum;
		point3f edge_sum;
		real nearest_squared_local;
		edge_sum = *g_468788;
		face_sum = *g_468788;
		long edge_count = 0;
		long face_count = 0;
		real edge_radius = radius;
		real face_radius = radius;
		function_16a440(flags, point, radius, 0.0f, 0.0f, ignore_object, ignore_parent, (s_shapes *)&shapes);
		real radius_squared = radius * radius;
		for (long index = 0; index < shapes.counts[2]; ++index)
		{
			s_prism *prism = &shapes.prisms[index];
			real distance = point->x * prism->plane.i + point->y * prism->plane.j +
				prism->plane.k * point->z - prism->plane.d;
			point3f projected;
			projected.x = (0.0f - distance) * prism->plane.i + point->x;
			projected.y = prism->plane.j * (0.0f - distance) + point->y;
			projected.z = (0.0f - distance) * prism->plane.k + point->z;
			nearest_squared_local = 3.4028234663852886e+38f;
			bool inside = true;
			long count = prism->point_count > 8 ? 8 : prism->point_count;
			point3f closest;
			for (long edge_index = 0; edge_index < count; ++edge_index)
			{
				point3f first;
				point3f second;
				function_120790(first.n, prism->plane.n.n, (real const *)&prism->points[edge_index], prism->axis, prism->side);
				function_120790(second.n, prism->plane.n.n, (real const *)&prism->points[(edge_index + 1) % prism->point_count], prism->axis, prism->side);
				vector3f relative;
				relative.i = projected.x - first.x;
				relative.j = projected.y - first.y;
				relative.k = projected.z - first.z;
				vector3f edge;
				edge.i = second.x - first.x;
				edge.j = second.y - first.y;
				edge.k = second.z - first.z;
				vector3f side;
				cross3f(&edge, &relative, &side);
				if (dot3f(&prism->plane.n, &side) < 0.0f)
				{
					inside = false;
				}
				else
				{
					vector3f perpendicular;
					cross3f(&edge, &prism->plane.n, &perpendicular);
					real amount = 0.0f - dot3f(&perpendicular, &relative);
					point3f candidate;
					candidate.x = perpendicular.i * amount + projected.x;
					candidate.y = perpendicular.j * amount + projected.y;
					candidate.z = perpendicular.k * amount + projected.z;
					real along = (candidate.x - first.x) * edge.i +
						(candidate.z - first.z) * edge.k + (candidate.y - first.y) * edge.j;
					point3f clamped;
					if (along <= 0.0f)
						clamped = first;
					else if (along >= length_sq3f(&edge))
						clamped = second;
					else
						clamped = candidate;
					vector3f difference;
					difference.i = clamped.x - projected.x;
					difference.j = clamped.y - projected.y;
					difference.k = clamped.z - projected.z;
					real separation = length_sq3f(&difference);
					if (nearest_squared_local > separation)
					{
						nearest_squared_local = separation;
						closest = clamped;
					}
				}
			}
			real separation = 0.0f;
			if (inside)
				closest = projected;
			else
				separation = nearest_squared_local;
			if (radius_squared > separation)
			{
				vector3f direction;
				point3f positive;
				point3f negative;
				s_ray_shape_result hit;
				s_ray_shape_result face_hit;
				bool use_normal = true;
				if (!(fabs(distance) < 0.01f && inside))
				{
					direction.i = closest.x - point->x;
					direction.j = closest.y - point->y;
					direction.k = closest.z - point->z;
					real magnitude = (real)sqrt(length_sq3f(&direction));
					if (!(fabs(magnitude) < 0.0001f))
					{
						real inverse = 1.0f / magnitude;
						vector3f_scale(&direction, inverse, &direction);
						if (magnitude > 0.01f)
							use_normal = false;
					}
				}
				if (!use_normal)
				{
					vector3f ray;
					vector3f_scale(&direction, radius, &ray);
					if (function_2469b0((s_shapes *)&shapes, point, &ray, &hit))
						positive = hit.position;
					else
					{
						positive.x = point->x + ray.i;
						positive.y = point->y + ray.j;
						positive.z = ray.k + point->z;
					}
					real opposite = 0.0f - radius;
					vector3f_scale(&direction, opposite, &ray);
					if (function_2469b0((s_shapes *)&shapes, point, &ray, &hit))
						negative = hit.position;
					else
					{
						negative.x = point->x + ray.i;
						negative.y = point->y + ray.j;
						negative.z = ray.k + point->z;
					}
				}
				else
				{
					vector3f ray;
					vector3f_scale(&prism->plane.n, radius, &ray);
					if (function_2469b0((s_shapes *)&shapes, point, &ray, &face_hit))
						positive = face_hit.position;
					else
					{
						positive.x = point->x + ray.i;
						positive.y = point->y + ray.j;
						positive.z = ray.k + point->z;
					}
					negative = *point;
				}
				point3f middle;
				middle.x = (negative.x + positive.x) * 0.5f;
				middle.y = (negative.y + positive.y) * 0.5f;
				middle.z = (negative.z + positive.z) * 0.5f;
				vector3f half;
				half.i = middle.x - positive.x;
				half.j = middle.y - positive.y;
				half.k = middle.z - positive.z;
				real available = (real)sqrt(length_sq3f(&half));
				if (use_normal)
				{
					face_sum.x += middle.x;
					face_sum.y += middle.y;
					face_sum.z += middle.z;
					if (face_radius > available) face_radius = available;
					++face_count;
				}
				else
				{
					edge_sum.x += middle.x;
					edge_sum.y += middle.y;
					edge_sum.z += middle.z;
					if (edge_radius > available) edge_radius = available;
					++edge_count;
				}
			}
		}
		if (face_count > 0)
		{
			real inverse = 1.0f / face_count;
			out->x = face_sum.x * inverse;
			out->y = face_sum.y * inverse;
			out->z = face_sum.z * inverse;
			*out_radius = face_radius;
			result = true;
		}
		else if (edge_count > 0)
		{
			real inverse = 1.0f / edge_count;
			out->x = edge_sum.x * inverse;
			out->y = edge_sum.y * inverse;
			out->z = edge_sum.z * inverse;
			*out_radius = edge_radius;
			result = true;
		}
		else
		{
			*out = *point;
			*out_radius = radius;
		}
	}
	else
	{
		*out = *point;
		*out_radius = radius;
	}
	return result;
}

s_object_masks_storage g_object_masks_storage;
