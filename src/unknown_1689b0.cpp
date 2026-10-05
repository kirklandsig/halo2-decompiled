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
	byte unknown008[0xaa - 0x8];
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

	if (object_index != ignore_object_index && object_index != ignore_object_index2 && !(header->flags & 0x10) &&
		!(*(dword const *)((byte const *)object + 4) & 1))
	{
		word type = header->type;

		if ((flags & (1 << (type + 4))) &&
			!((flags & 0x80000) && TEST_FIELD_BIT(object->bit20)) &&
			!((flags & 0x40000) && !TEST_FIELD_BIT(object->bit22)) &&
			!((flags & 0x40000000) && !TEST_FIELD_BIT(object->bit31)))
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
	byte unknown00[0x14];

	bool initialize(long object_handle);
};

struct s_table_holder;
void *function_1efd80(s_table_holder *holder, dword position);

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
long function_14a280(s_bsp3d *bsp, point3f *point, long index);
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
			if (function_14a280((s_bsp3d *)(section + 0x70), &local_point, 0) == NONE)
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
	byte unknown71[0xb0 - 0x71];
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
bool function_169510(long object_index, bool a, dword flags, dword test_flags, point3f const *point,
	vector3f const *vector, long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
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

#if 0
/* Written but left out of the build (lane C's stub in src/stubs/lane_c.cpp
   stays): retail keeps this function's standard stack convention (ret 0x18)
   though nothing holds its address. With our few decompiled callers LTCG
   passes the result in a register instead, which breaks its matched caller
   0x10b190. With the `standard` marker the body lines up instruction for
   instruction except for register choice and the register arguments of its
   callees 0x1de630, 0x244980, 0xb8940, 0xb89b0 and 0x169510, which are stubs;
   once they are real, try the marker. */
/* tests a vector from a point against the structure, its instanced planes,
   the instanced geometry and the objects of the clusters it crosses */
/* retail 0x1697c0 (ray_cast_test) */
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision)
{
	s_1697c0_bsp *bsp = (s_1697c0_bsp *)g_4e0348;
	bool result = false;
	short bsp_index;
	dword test_flags;
	s_collision_bsp_test_vector_result bsp_result;

	if (!(flags & 0x1800000))
	{
		flags |= 0x1800000;
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
	test_flags = collision_flags_to_test_flags(flags);
	if (function_1de630(test_flags, g_4e0340, &bsp_result, 3.4028235e38f, 0x100, g_4ed280 + bsp_index * 0x20 + 1, point,
		vector) && (flags & 1))
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

		if ((flags & 0xe) && (!result || !(flags & 0x10000000)))
		{
			long leaf;

			g_4e7414++;
			g_4e7411 = true;
			if (flags & 4)
			{
				g_4e7c1c++;
				g_4e7c18 = true;
			}
			if (flags & 8)
			{
				g_4de2fc++;
				g_4de2f8 = true;
				if (!(flags & 0x1fff0))
				{
					flags |= 0x1fff0;
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
				if (flags & 2)
				{
					long reference = bsp->clusters[cluster_index].instanced_plane_reference;

					if ((char)reference != NONE && (reference & 0x80))
					{
						long plane_index = reference & 0x7f;
						s_1697c0_instanced_plane *instanced_plane = &bsp->instanced_planes[plane_index];

						if (instanced_plane->material_type != NONE)
						{
							plane3f const *plane = &instanced_plane->plane;
							real point_distance = plane->i * point->x + plane->j * point->y + plane->k * point->z - plane->d;
							real vector_distance = plane->i * vector->i + plane->j * vector->j + plane->k * vector->k;

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
					if (result && (flags & 0x10000000))
					{
						break;
					}
				}
				if (flags & 4)
				{
					if (function_244980(cluster_index, point, vector, flags, test_flags, collision))
					{
						result = true;
					}
					if (result && (flags & 0x10000000))
					{
						break;
					}
				}
				if (flags & 8)
				{
					word object_flags = collision_flags_to_object_flags(flags);
					word type_mask = (word)((flags >> 4) & 0x1fff);
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
							function_11e5e0(point, &reference->center, vector, reference->radius) &&
							object_index != ignore_object_index && object_index != ignore_unit_index &&
							function_169510(object_index, true, flags, test_flags, point, vector, ignore_object_index,
								ignore_unit_index, collision))
						{
							result = true;
						}
						if (result && (flags & 0x10000000))
						{
							break;
						}
					}
					if (result && (flags & 0x10000000))
					{
						break;
					}
				}
			}
			if (flags & 8)
			{
				g_4de2f8 = false;
			}
			if (flags & 4)
			{
				g_4e7c18 = false;
			}
			g_4e7411 = false;
		}
	}

	collision->point.x = vector->i * collision->t + point->x;
	collision->point.y = collision->t * vector->j + point->y;
	collision->point.z = collision->t * vector->k + point->z;
	if (result && (flags & 0x20000000))
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
			function_14a280(g_4e033c, &collision->point, 0) != collision->end_location.leaf_index ||
			surface_bsp && collision->bsp_surface_reference[0] != NONE &&
			function_14a280(surface_bsp, &collision->point, 0) != collision->bsp_surface_reference[0])
		{
			collision->point.x += collision->plane.i * 0.000244140625f;
			collision->point.y += collision->plane.j * 0.000244140625f;
			collision->point.z += collision->plane.k * 0.000244140625f;
			function_11bed0((s_location *)&collision->end_location, &collision->point);
			if (collision->end_location.leaf_index == NONE ||
				surface_bsp && function_14a280(surface_bsp, &collision->point, 0) == NONE)
			{
				real normal_speed = collision->plane.i * vector->i + collision->plane.j * vector->j +
					collision->plane.k * vector->k;
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
					collision->point.x = vector->i * t + point->x;
					collision->point.y = t * vector->j + point->y;
					collision->point.z = t * vector->k + point->z;
					function_11bed0((s_location *)&collision->end_location, &collision->point);
				}
				while (0.0f < collision->t && (collision->end_location.leaf_index == NONE ||
					surface_bsp && function_14a280(surface_bsp, &collision->point, 0) == NONE));
			}
		}
	}

	return result;
}
#endif

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
	if (function_1691a0(object_index, flags, collision_flags_to_test_flags(flags), point, vector, collision))
	{
		result = true;
	}
	collision->point.x = vector->i * collision->t + point->x;
	collision->point.y = vector->j * collision->t + point->y;
	collision->point.z = vector->k * collision->t + point->z;
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
	bool moved = false;

	vector.i = to->x - from->x;
	vector.j = to->y - from->y;
	vector.k = to->z - from->z;
	collision.material_type = NONE;
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
		*result = *to;
		moved = true;
	}
	return moved;
}
