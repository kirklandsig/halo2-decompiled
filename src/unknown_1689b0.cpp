// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1689B0.CPP: collision queries (0x1689b0..0x16b569): the flag
   conversions between the query flags and the collision tests' own, and the
   test of a surface against the query */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"

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
dword collision_flags_to_test_flags(dword flags)
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
void *collision_reference_get_data(s_collision_reference const *reference)
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
	s_168d60_bsp_view *bsp = (s_168d60_bsp_view *)g_4e0348;
	s_168d60_instance *instance = &bsp->instances[instance_index];
	byte *section = bsp->sections + instance->section_index * 0xc8;

	if (collision_surface_test((s_collision_result_view const *)section, instance_index, flags))
	{
		real dx = instance->center.x - point->x;
		real dz = instance->center.z - point->z;
		real dy = instance->center.y - point->y;
		real radius = instance->radius;

		if (dx * dx + dz * dz + dy * dy <= radius * radius)
		{
			point3f local_point;

			function_142700(&instance->matrix, point, &local_point);
			if (function_14a280((s_bsp3d *)(section + 0x70), 0, &local_point) == NONE)
			{
				return true;
			}
		}
	}
	return false;
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
	out->i = matrix->up.i * vector->k + matrix->left.i * vector->j + matrix->forward.i * vector->i;
	out->j = matrix->up.j * vector->k + matrix->left.j * vector->j + matrix->forward.j * vector->i;
	out->k = matrix->up.k * vector->k + matrix->left.k * vector->j + matrix->forward.k * vector->i;
	return out;
}

// @retail 0x168b40
void function_168b40(s_168b40_result *result, s_168b40_surface const *surface, transform4x3f const *matrix)
{
	result->unknown04 = surface->unknown00;
	if (matrix)
	{
		transform4x3f_apply_normal(matrix, &surface->plane->n, &result->plane.n);
		result->plane.d = matrix->scale * surface->plane->d + dot3f(&result->plane.n, (vector3f const *)&matrix->position);
		if (surface->flags & 0x8000)
		{
			result->plane.i = 0.0f - result->plane.i;
			result->plane.j = 0.0f - result->plane.j;
			result->plane.k = 0.0f - result->plane.k;
			result->plane.d = 0.0f - result->plane.d;
		}
	}
	else if (surface->flags & 0x8000)
	{
		result->plane.i = 0.0f - surface->plane->i;
		result->plane.j = 0.0f - surface->plane->j;
		result->plane.k = 0.0f - surface->plane->k;
		result->plane.d = 0.0f - surface->plane->d;
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
dword collision_flags_to_test_flags(dword flags);
word collision_flags_to_object_flags(dword flags);
bool function_1de630(dword test_flags, s_slot_entry_list *bsp, s_collision_bsp_test_vector_result *result, real maximum_t,
	long maximum_leaf_count, byte const *arg_c9e1f7, point3f const *point, vector3f const *vector);
bool function_244980(long cluster_index, point3f const *point, vector3f const *vector, dword flags,
	dword test_flags, s_collision_result_1697c0 *result);
long function_b8940(short cluster_index, s_object_cluster_reference **reference, s_object_cluster_iterator *iterator);
long function_b89b0(s_object_cluster_reference **reference, s_object_cluster_iterator *iterator);
bool function_11e5e0(point3f const *origin, point3f const *center, vector3f const *direction, real radius);
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
void function_d8ae0(long object_index, long region_index, short *material);
bool function_11e5e0(point3f const *origin, point3f const *center, vector3f const *direction, real radius);

// @retail 0x169510
bool __stdcall function_169510(long object_index, bool a, dword flags, dword test_flags, point3f const *point,
	vector3f const *vector, long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result)
{
	(void)&object_index; (void)&a; (void)&flags; (void)&test_flags; (void)&point;
	(void)&vector; (void)&ignore_object_index; (void)&ignore_unit_index; (void)&result;
	volatile bool found = false;
	s_lookup lookup;
	s_lookup_ray_result hit;
	short material;
	do
	{
		s_collision_object_header *header = &((s_collision_object_header *)g_4e0300->data)[object_index & 0xffff];
		s_collision_object *object = header->object;
		s_collision_object *volatile saved_object = object;
		if (a || (collision_object_test(object_index, header, object, flags, ignore_object_index, ignore_unit_index) &&
			function_11e5e0(point, (point3f const *)((byte *)object + 0x40), vector, *(real *)((byte *)object + 0x4c))))
		{
			if (lookup.initialize(object_index) && function_1eff40(&lookup, test_flags, point, vector, &hit) && result->t > hit.hit.t)
			{
				result->type = 4;
				function_d8ae0(lookup.handle, hit.hit.surface_index, &material);
				result->material_type = material;
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

// @retail 0x1696d0
bool function_1696d0(long flags, s_biped_ground_collision *output, long object_index,
	point3f const *point, vector3f const *vector, long ignore_object_index, long ignore_unit_index)
{
	s_collision_result_1697c0 *collision = (s_collision_result_1697c0 *)output;
	collision->type = 0;
	collision->t = 1.0f;
	collision->start_location.leaf_index = NONE;
	collision->start_location.cluster_index = NONE;
	collision->start_location.bsp_index = g_4686c4;
	collision->end_location.leaf_index = NONE;
	collision->end_location.cluster_index = NONE;
	collision->end_location.bsp_index = g_4686c4;
	collision->instance_index = NONE;
	collision->unknown3c = NONE;
	collision->unknown40 = NONE;
	bool found = false;
	if ((flags & 8) && !(flags & 0x1fff0)) flags |= 0x1fff0;
	if (!object_or_parent_hidden(object_index))
		found = function_169510(object_index, false, flags, collision_flags_to_test_flags(flags),
			point, vector, ignore_object_index, ignore_unit_index, collision);
	collision->point.x = point->x + vector->i * collision->t;
	collision->point.y = point->y + vector->j * collision->t;
	collision->point.z = point->z + vector->k * collision->t;
	function_11bed0((s_location *)&collision->end_location, &collision->point);
	return found;
}
struct s_location;
void function_11bed0(s_location *location, point3f const *point);

extern short g_4686c4;
extern short g_47d8e0;
extern byte *g_4ed280;
extern long g_4e7414;
extern bool g_4e7411;
extern long g_4e7418[0x200];
long g_4e7c1c;
bool g_4e7c18;
long g_4de2fc;
bool g_4de2f8;
long g_4de300[0x800];
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

struct s_vehicle_ray
{
	point3f point;
	vector3f vector;
	real t;
};

// @retail 0x168f40
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray,
	long ignore_object_index, long ignore_object_index2)
{
	bool result = false;
	point3f const *point = &ray->point;
	long leaf = function_14a280(g_4e033c, 0, (point3f *)point);
	if (leaf == NONE)
		return true;
	if ((flags & 2) || (flags & 0xc))
	{
		s_1697c0_bsp *bsp = (s_1697c0_bsp *)g_4e0348;
		long cluster_index = bsp->leaves[leaf].cluster_index;
		if (flags & 2)
		{
			long reference = bsp->clusters[cluster_index].instanced_plane_reference;
			if ((char)reference != NONE)
			{
				if ((char)reference < 0)
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
				if (collision_point_inside_instance(cluster->instances[i], point, flags))
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
			for (long object_index = function_b8940((short)cluster_index, &reference, &iterator);
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
						return true;
				}
			}
		}
	}
	return result;
}

// @retail 0x1697c0
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision)
{
	vector3f const * *vector_reference = &vector;
	point3f const * *point_reference = &point;
	long *flags_reference = &flags;
	s_1697c0_bsp *bsp = (s_1697c0_bsp *)g_4e0348;
	bool result = false;
	short bsp_index;
	dword test_flags;
	s_collision_bsp_test_vector_result bsp_result;

	if (!((*flags_reference) & 0x1800000))
	{
		(*flags_reference) |= 0x1800000;
	}
	bsp_index = g_4686c4;
	collision->t = 1.0f;
	collision->type = 0;
	collision->start_location.leaf_index = NONE;
	collision->start_location.cluster_index = NONE;
	collision->start_location.bsp_index = bsp_index;
	collision->end_location.leaf_index = NONE;
	collision->end_location.cluster_index = NONE;
	collision->end_location.bsp_index = bsp_index;
	collision->instance_index = NONE;
	collision->unknown3c = NONE;
	collision->unknown40 = NONE;
	test_flags = collision_flags_to_test_flags((*flags_reference));
	if (function_1de630(test_flags, g_4e0340, &bsp_result, 3.4028235e38f, 0x100, g_4ed280 + bsp_index * 0x20 + 1, (*point_reference),
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
		collision->start_location.bsp_index = bsp_index;
		leaf_index = bsp_result.leaves[bsp_result.leaf_count - 1];
		collision->end_location.leaf_index = leaf_index;
		collision->end_location.cluster_index = (short)collision_leaf_cluster(leaf_index);
		collision->end_location.bsp_index = bsp_index;

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
				if ((*flags_reference) & 2)
				{
					long reference = bsp->clusters[cluster_index].instanced_plane_reference;

					if ((char)reference != NONE && (reference & 0x80))
					{
						long plane_index = reference & 0x7f;
						s_1697c0_instanced_plane *instanced_plane = &bsp->instanced_planes[plane_index];

						if (instanced_plane->material_type != NONE)
						{
							plane3f const *plane = &instanced_plane->plane;
							real point_distance = plane->i * (*point_reference)->x + plane->j * (*point_reference)->y + plane->k * (*point_reference)->z - plane->d;
							real vector_distance = plane->i * (*vector_reference)->i + plane->j * (*vector_reference)->j + plane->k * (*vector_reference)->k;

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
											collision->plane.i = 0.0f - collision->plane.i;
											collision->plane.j = 0.0f - collision->plane.j;
											collision->plane.k = 0.0f - collision->plane.k;
											collision->plane.d = 0.0f - collision->plane.d;
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
					word object_flags = collision_flags_to_object_flags((*flags_reference));
					word type_mask = (word)(((*flags_reference) >> 4) & 0x1fff);
					s_object_cluster_reference *reference = NULL;
					s_object_cluster_iterator iterator;
					long object_index;

					for (object_index = function_b8940((short)cluster_index, &reference, &iterator); object_index != NONE;
						object_index = function_b89b0(&reference, &iterator))
					{
						if (g_4de300[object_index & 0xffff] == g_4de2fc)
						{
							continue;
						}
						g_4de300[object_index & 0xffff] = g_4de2fc;
						if ((type_mask & (1 << reference->type)) && !(reference->flags & object_flags) &&
							function_11e5e0((*point_reference), &reference->center, (*vector_reference), reference->radius) &&
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

	collision->point.x = (*vector_reference)->i * collision->t + (*point_reference)->x;
	collision->point.y = collision->t * (*vector_reference)->j + (*point_reference)->y;
	collision->point.z = collision->t * (*vector_reference)->k + (*point_reference)->z;
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
			collision->point.x += collision->plane.i * 0.000244140625f;
			collision->point.y += collision->plane.j * 0.000244140625f;
			collision->point.z += collision->plane.k * 0.000244140625f;
			function_11bed0((s_location *)&collision->end_location, &collision->point);
			if (collision->end_location.leaf_index == NONE ||
				surface_bsp && function_14a280(surface_bsp, 0, &collision->point) == NONE)
			{
				real normal_speed = collision->plane.i * (*vector_reference)->i + collision->plane.j * (*vector_reference)->j +
					collision->plane.k * (*vector_reference)->k;
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
					collision->point.x = (*vector_reference)->i * t + (*point_reference)->x;
					collision->point.y = t * (*vector_reference)->j + (*point_reference)->y;
					collision->point.z = t * (*vector_reference)->k + (*point_reference)->z;
					function_11bed0((s_location *)&collision->end_location, &collision->point);
				}
				while (0.0f < collision->t && (collision->end_location.leaf_index == NONE ||
					surface_bsp && function_14a280(surface_bsp, 0, &collision->point) == NONE));
			}
		}
	}

	return result;
}

bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

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

/* tests a vector against one object */
// @retail 0x169430
bool collision_test_vector_object(dword flags, s_collision_result_1697c0 *collision, long object_index,
	point3f const *point, vector3f const *vector)
{
	point3f const *const *point_reference = &point;
	bool result;
	short bsp_index = g_4686c4;

	collision->type = 0;
	collision->t = 1.0f;
	collision->start_location.bsp_index = bsp_index;
	collision->start_location.leaf_index = NONE;
	collision->start_location.cluster_index = NONE;
	collision->end_location.leaf_index = NONE;
	collision->end_location.cluster_index = NONE;
	collision->end_location.bsp_index = bsp_index;
	result = false;
	collision->instance_index = NONE;
	collision->unknown3c = NONE;
	collision->unknown40 = NONE;
	if ((flags & 8) && !(flags & 0x1fff0))
	{
		flags |= 0x1fff0;
	}
	if (function_1691a0(object_index, flags, collision_flags_to_test_flags(flags), (*point_reference), vector, collision))
	{
		result = true;
	}
	real fraction = collision->t;
	collision->point.x = vector->i * fraction + (*point_reference)->x;
	collision->point.y = vector->j * fraction + (*point_reference)->y;
	collision->point.z = vector->k * fraction + (*point_reference)->z;
	function_11bed0((s_location *)&collision->end_location, &collision->point);
	return result;
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
    s_168d60_bsp_view *bsp = (s_168d60_bsp_view *)g_4e0348;
    s_168d60_instance *instance = &bsp->instances[instance_index];
    byte *section = bsp->sections + instance->section_index * 0xc8;
    if (collision_surface_test((s_collision_result_view const *)section, instance_index, flags) &&
        function_11e5e0(&instance->center, point, vector, instance->radius))
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
            collision->material_type = surface_index != NONE ?
                ((s_1697c0_bsp *)g_4e0348)->materials[surface_index].material_type : g_47d8e0;
            collision->instance_index = NONE;
            collision->unknown3c = instance_index;
            collision->unknown40 = NONE;
            function_168b40((s_168b40_result *)collision, (s_168b40_surface const *)&hit, &instance->matrix);
            return true;
        }
    }
    return false;
}

void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
struct s_line_list;
void function_244ca0(real height, real radius, s_line_list *list, long a, long b, long c,
    byte d, byte e, short f, point3f const *position);
bool __stdcall function_1f0230(s_lookup *lookup, point3f const *point, real radius,
    real height, real thickness, s_collection_245270 *collection);

// @retail 0x16a280
void __stdcall function_16a280(long object_index, dword flags, point3f const *point,
    real radius, real height, real thickness, long ignore_object_index, long ignore_object_index2,
    s_collection_245270 *collection)
{
    do
    {
        s_collision_object_header *volatile header = &((s_collision_object_header *)g_4e0300->data)[object_index & 0xffff];
        s_collision_object *object = header->object;
        if (collision_object_test(object_index, header, object, flags, ignore_object_index, ignore_object_index2))
        {
            real dx = object->center.x - point->x;
            real dy = object->center.y - point->y;
            real dz = object->center.z - point->z;
            real extent = object->radius + radius;
            if (dx * dx + dy * dy + dz * dz <= extent * extent)
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
                        (s_line_list *)collection, object_index, NONE, NONE, 0, 0xff, NONE, &center);
                    break;
                }
                case 1:
                case 6:
                case 7:
                case 8:
                case 11:
                {
                    s_lookup lookup;
                    if (lookup.initialize(object_index))
                        function_1f0230(&lookup, point, radius, height, thickness, collection);
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
extern void *g_4de2e0;
extern void *g_4de2e4;

PRIVATE __forceinline long shape_cluster_next(long *next)
{
    if (*next != NONE)
    {
        s_record_pool *pool = (s_record_pool *)g_4de2e4;
        byte *entry = pool->data + pool->size * (*next & 0xffff);
        *next = *(long *)(entry + 8);
        if (*next != NONE)
            _mm_prefetch((char const *)(pool->data + pool->size * (*next & 0xffff)), _MM_HINT_T0);
        return *(long *)(entry + 4);
    }
    return NONE;
}

// @retail 0x16a440
bool __stdcall function_16a440(dword flags, point3f const *position, real extent, real height,
    real radius, long ignore_object, long ignore_parent, s_shapes *shapes)
{
    s_shape_counts *counts = (s_shape_counts *)shapes;
    counts->count[0] = 0;
    counts->count[1] = 0;
    counts->count[2] = 0;
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
                    for (long i = 0; i < entry->instance_count; ++i)
                    {
                        short instance = entry->instances[i];
                        if (g_4e7c20[instance] == g_4e7c1c) continue;
                        g_4e7c20[instance] = g_4e7c1c;
                        function_16a0a0(instance, flags, position, extent, height, radius,
                            (s_collection_245270 *)shapes);
                    }
                }
                if (flags & 8)
                {
                    long next = ((long *)g_4de2e0)[(short)cluster];
                    for (long object = shape_cluster_next(&next); object != NONE;
                        object = shape_cluster_next(&next))
                    {
                        if (g_4de300[object & 0xffff] == g_4de2fc) continue;
                        g_4de300[object & 0xffff] = g_4de2fc;
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

PRIVATE inline void camera_prism_point_16a8e0(s_prism const *prism, point2f const *point, point3f *out)
{
	short axis = prism->axis;
	short const *indices = g_440b94[prism->side + axis * 2];
	out->n[indices[0]] = point->x;
	out->n[indices[1]] = point->y;
	if (fabs(prism->plane.n.n[axis]) < 0.0001f)
		out->n[axis] = 0.0f;
	else
		out->n[axis] = (prism->plane.d - prism->plane.n.n[indices[0]] * point->x -
			prism->plane.n.n[indices[1]] * point->y) / prism->plane.n.n[axis];
}

// @retail 0x16a8e0
bool __stdcall function_16a8e0(long flags, point3f const *point, real radius,
	long ignore_object, long ignore_parent, point3f *out, real *out_radius)
{
	(void)&flags;
	(void)&point;
	(void)&radius;
	(void)&ignore_object;
	(void)&ignore_parent;
	(void)&out;
	(void)&out_radius;
	s_camera_shapes_16a8e0 shapes;
	bool result = false;
	if (radius > 0.0f)
	{
		struct
		{
			point3f face_sum;
			real nearest_squared;
			point3f edge_sum;
		} accumulators;
		accumulators.edge_sum = *g_468788;
		accumulators.face_sum = *g_468788;
		long edge_count = 0;
		long face_count = 0;
		real edge_radius = radius;
		real face_radius = radius;
		function_16a440(flags, point, radius, 0.0f, 0.0f, ignore_object, ignore_parent, (s_shapes *)&shapes);
		if (shapes.counts[2] > 0)
		{
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
				accumulators.nearest_squared = 3.4028234663852886e+38f;
				bool inside = true;
				long count = prism->point_count > 8 ? 8 : prism->point_count;
				point3f closest;
				for (long edge_index = 0; edge_index < count; ++edge_index)
				{
					point3f first;
					point3f second;
					camera_prism_point_16a8e0(prism, &prism->points[edge_index], &first);
					camera_prism_point_16a8e0(prism, &prism->points[(edge_index + 1) % prism->point_count], &second);
					vector3f relative;
					relative.i = projected.x - first.x;
					relative.j = projected.y - first.y;
					relative.k = projected.z - first.z;
					vector3f edge;
					edge.i = second.x - first.x;
					edge.j = second.y - first.y;
					edge.k = second.z - first.z;
					vector3f side;
					side.i = edge.j * relative.k - edge.k * relative.j;
					side.j = edge.k * relative.i - relative.k * edge.i;
					side.k = relative.j * edge.i - edge.j * relative.i;
					if (prism->plane.k * side.k + prism->plane.j * side.j + prism->plane.i * side.i < 0.0f)
					{
						inside = false;
					}
					else
					{
						vector3f perpendicular;
						perpendicular.i = prism->plane.k * edge.j - prism->plane.j * edge.k;
						perpendicular.j = prism->plane.i * edge.k - prism->plane.k * edge.i;
						perpendicular.k = prism->plane.j * edge.i - prism->plane.i * edge.j;
						real amount = 0.0f - (perpendicular.i * relative.i +
							perpendicular.k * relative.k + perpendicular.j * relative.j);
						point3f candidate;
						candidate.x = perpendicular.i * amount + projected.x;
						candidate.y = perpendicular.j * amount + projected.y;
						candidate.z = perpendicular.k * amount + projected.z;
						real along = (candidate.x - first.x) * edge.i +
							(candidate.z - first.z) * edge.k + (candidate.y - first.y) * edge.j;
						point3f clamped;
						if (along <= 0.0f)
							clamped = first;
						else if (along >= edge.j * edge.j + edge.i * edge.i + edge.k * edge.k)
							clamped = second;
						else
							clamped = candidate;
						vector3f difference;
						difference.i = clamped.x - projected.x;
						difference.j = clamped.y - projected.y;
						difference.k = clamped.z - projected.z;
						real separation = difference.i * difference.i + difference.k * difference.k +
							difference.j * difference.j;
						if (accumulators.nearest_squared > separation)
						{
							accumulators.nearest_squared = separation;
							closest = clamped;
						}
					}
				}
				real separation = 0.0f;
				if (inside)
					closest = projected;
				else
					separation = accumulators.nearest_squared;
				if (radius_squared > separation)
				{
					bool face = true;
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
						real magnitude = (real)sqrt(direction.i * direction.i + direction.k * direction.k +
							direction.j * direction.j);
						if (!(fabs(magnitude) < 0.0001f))
						{
							real inverse = 1.0f / magnitude;
							direction.i = inverse * direction.i;
							direction.j = direction.j * inverse;
							direction.k = direction.k * inverse;
							if (magnitude > 0.01f)
								use_normal = false;
						}
					}
					if (!use_normal)
					{
						vector3f ray;
						ray.i = direction.i * radius;
						ray.j = direction.j * radius;
						ray.k = direction.k * radius;
						face = false;
						if (function_2469b0((s_shapes *)&shapes, point, &ray, &hit))
							positive = hit.position;
						else
						{
							positive.x = point->x + ray.i;
							positive.y = point->y + ray.j;
							positive.z = ray.k + point->z;
						}
						real opposite = 0.0f - radius;
						ray.i = direction.i * opposite;
						ray.j = direction.j * opposite;
						ray.k = direction.k * opposite;
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
						ray.i = prism->plane.i * radius;
						ray.j = prism->plane.j * radius;
						ray.k = prism->plane.k * radius;
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
					real available = (real)sqrt(half.i * half.i + half.k * half.k + half.j * half.j);
					if (face)
					{
						accumulators.face_sum.x += middle.x;
						accumulators.face_sum.y += middle.y;
						accumulators.face_sum.z += middle.z;
						if (face_radius > available) face_radius = available;
						++face_count;
					}
					else
					{
						accumulators.edge_sum.x += middle.x;
						accumulators.edge_sum.y += middle.y;
						accumulators.edge_sum.z += middle.z;
						if (edge_radius > available) edge_radius = available;
						++edge_count;
					}
				}
			}
		}
		if (face_count > 0)
		{
			real inverse = 1.0f / face_count;
			out->x = accumulators.face_sum.x * inverse;
			out->y = accumulators.face_sum.y * inverse;
			out->z = accumulators.face_sum.z * inverse;
			*out_radius = face_radius;
			result = true;
		}
		else if (edge_count > 0)
		{
			real inverse = 1.0f / edge_count;
			out->x = accumulators.edge_sum.x * inverse;
			out->y = accumulators.edge_sum.y * inverse;
			out->z = accumulators.edge_sum.z * inverse;
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
