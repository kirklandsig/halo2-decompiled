// @flags /O2 /arch:SSE /Gr
/* STRUCTURES.CPP: queries of the structure bsp's render geometry: the
   lightmap triangle under a collision point */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "geometry_cache.h"

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
short function_10a0b0(long object_index);
bool function_bf9a0(long object_index, long *render_index, long *animation_index);
extern vector3f *g_4687bc;

/* a range of a section's triangles: the cluster (or instance definition)
   it is in, and its first triangle reference (8 bytes) */
struct s_structure_surface_range
{
	short cluster_index;
	short count;
	long first;
};

/* a triangle reference: its first index in the section's strip, its
   lightmap part, and its collision surface (8 bytes) */
struct s_structure_triangle_reference
{
	word first_index;
	word part_index;
	long surface_index;
};

/* a collision bsp's surfaces, as read here: the plane first (8 bytes) */
struct s_structure_collision_surface
{
	short plane_index;
	byte unknown02[6];
};

struct s_structure_collision_bsp
{
	long surface_count;
	s_structure_collision_surface *surfaces;
	byte unknown08[0x40 - 0x08];
};

/* a section's vertices: the count and the data, at an offset in the
   section's block (0x20 bytes) */
struct s_structure_vertex_block
{
	word unknown00;
	word count;
	byte unknown04[4];
	long offset;
	byte *data;
	byte unknown10[0x20 - 0x10];
};

/* a section of render geometry (0x44 bytes): its parts (the list
   function_16e1b0 searches), its strip indices and its vertices */
struct s_structure_section
{
	long part_count;
	void *parts;
	byte unknown08[0x20 - 0x08];
	long strip_index_count;
	word *strip_indices;
	byte unknown28[0x38 - 0x28];
	long vertex_block_count;
	s_structure_vertex_block *vertex_blocks;
	byte unknown40[4];
};

/* a cluster (0xb0 bytes) and an instanced geometry definition (0xc8 bytes)
   both start their geometry the same way */
struct s_structure_cluster_view
{
	byte unknown00[0x28];
	s_geometry_block_info field_28;
	byte unknown4c[4];
	s_structure_section *sections;
	byte unknown54[0xb0 - 0x54];
};

struct s_structure_instanced_geometry_definition
{
	byte unknown00[0x28];
	s_geometry_block_info field_28;
	byte unknown4c[4];
	s_structure_section *sections;
	byte unknown54[0x74 - 0x54];
	s_structure_collision_surface *surfaces;
	byte unknown78[0xb8 - 0x78];
	long surface_range_count;
	s_structure_surface_range *surface_ranges;
	byte unknownc0[4];
	s_structure_triangle_reference *triangle_references;
};

struct s_structure_instance
{
	transform4x3f matrix;
	short definition_index;
	byte unknown36[0x58 - 0x36];
};

struct s_structure_bsp_view
{
	byte unknown000[0x18];
	s_structure_collision_bsp *collision_bsps;
	byte unknown01c[0x30 - 0x1c];
	s_structure_surface_range *surface_ranges;
	byte unknown034[0x50 - 0x34];
	s_structure_triangle_reference *triangle_references;
	byte unknown054[0xa0 - 0x54];
	s_structure_cluster_view *clusters;
	byte unknown0a4[0x13c - 0xa4];
	s_structure_instanced_geometry_definition *instanced_geometry_definitions;
	byte unknown140[4];
	s_structure_instance *instances;
};

/* a collision result, as read here */
struct s_structure_collision_result
{
	long type;
	byte unknown04[4];
	point3f point;
	byte unknown14[0x3c - 0x14];
	long instance_index;
	byte unknown40[0x4c - 0x40];
	long surface_range_index;
	byte unknown50[4];
	long plane_index;
};

/* the lightmap triangle found */
struct s_structure_lightmap_triangle
{
	long cluster_index;
	long instance_index;
	byte unknown08[0x18 - 0x08];
	long unknown18;
	byte unknown1c[4];
	long part_index;
	long part_offset;
	long lightmap_part_index;
	byte unknown2c[4];
	real u;
	real v;
};

bool structure_get_lightmap_triangle(s_structure_collision_result const *collision,
    s_structure_lightmap_triangle *triangle);
bool __stdcall function_14a8e0(s_structure_collision_result const *collision,
    long model_index, s_structure_lightmap_triangle *triangle);

// @retail 0x14b120
bool function_14b120(void *source, void *surface)
{
    void *const *source_reference = &source;
    bool volatile result = false;
    byte local_collision[0x5c];
    *(short *)(local_collision + 0x24) = NONE;
    if (!g_4e0344 || g_4e0344->count <= 0 || !g_4e0348)
        return false;
    byte const *bsp = (byte const *)g_4e0344->bsp;
    if (*(long const *)(bsp + 0x1c) == NONE ||
        *(long const *)(bsp + 4) != *(long const *)((byte const *)g_4e0348 + 8))
        return false;
    s_structure_collision_result const *collision = (s_structure_collision_result const *)*source_reference;
    s_structure_lightmap_triangle *triangle = (s_structure_lightmap_triangle *)surface;
    triangle->cluster_index = NONE;
    triangle->instance_index = NONE;
    *(long *)((byte *)triangle + 8) = NONE;
    bool found = collision->type == 1 || collision->type == 3;
    long object_index = *(long const *)((byte const *)collision + 0x40);
    bool excluded = false;
    if (object_index != NONE)
    {
        byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
        if (object[0xaa] == 6 && function_10a0b0(object_index) != 2)
            excluded = true;
        else if (function_bf9a0(object_index, (long *)((byte *)triangle + 0xc),
            (long *)((byte *)triangle + 0x10)))
        {
            result = function_14a8e0((s_structure_collision_result const *)*source_reference,
                *(long *)((byte *)triangle + 0xc), triangle);
            return result;
        }
        if (!excluded)
        {
            found = function_1697c0(0x4800005, &collision->point, g_4687bc, NONE, NONE,
                (s_collision_result_1697c0 *)local_collision);
            source = local_collision;
            collision = (s_structure_collision_result const *)*source_reference;
        }
    }
    if (!excluded && found)
        result = structure_get_lightmap_triangle(collision, triangle);
    return result;
}

struct s_16e1b0_list;
void function_16e1b0(long value, s_16e1b0_list const *list, long *unknown, long *range_index, long *offset);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
bool function_11e800(point3f const *a, point3f const *b, point3f const *c, point3f const *p, real *u, real *v);

static inline bool structure_section_get_vertex(s_structure_section const *section, long index, point3f *point)
{
	s_structure_vertex_block const *block = &section->vertex_blocks[0];
	point3f const *vertices = (point3f const *)(block->data + block->offset);

	if (vertices && index >= 0 && index < block->count)
	{
		*point = vertices[index];
		return true;
	}
	return false;
}

// @retail 0x14ac60
bool structure_get_lightmap_triangle(s_structure_collision_result const *collision, s_structure_lightmap_triangle *triangle)
{
	bool result = false;
	point3f point = collision->point;
	s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
	long surface_range_index = collision->surface_range_index;
	long plane_index = collision->plane_index & 0x7fff;
	long instance_index = collision->instance_index;
	s_structure_instanced_geometry_definition *definition = NULL;
	s_structure_section *section = NULL;
	s_structure_surface_range *range;

	if (collision->type == 1)
	{
		range = &bsp->surface_ranges[surface_range_index];
		s_structure_cluster_view *cluster = &bsp->clusters[range->cluster_index];

		if (function_12de70(&cluster->field_28, 3))
		{
			section = &cluster->sections[0];
		}
	}
	else if (collision->type == 3)
	{
		s_structure_instance *instance = &bsp->instances[instance_index];

		definition = &bsp->instanced_geometry_definitions[instance->definition_index];
		if (definition->surface_range_count == 0)
		{
			return false;
		}
		range = &definition->surface_ranges[surface_range_index];
		function_142700(&instance->matrix, &point, &point);
		if (function_12de70(&definition->field_28, 3))
		{
			section = &definition->sections[0];
		}
	}

	triangle->unknown18 = 0;
	if (section)
	{
		for (long i = range->first; i < range->first + range->count; i++)
		{
			s_structure_triangle_reference *reference = instance_index == NONE ?
				&bsp->triangle_references[i] :
				&definition->triangle_references[i];

			if (reference->surface_index == NONE)
			{
				continue;
			}

			s_structure_collision_surface *surface = instance_index == NONE ?
				&bsp->collision_bsps[0].surfaces[reference->surface_index] :
				&definition->surfaces[reference->surface_index];

			if (surface->plane_index != plane_index)
			{
				continue;
			}

			word *indices = &section->strip_indices[reference->first_index];
			long unknown;
			long part_index;
			long part_offset;
			point3f vertices[3];
			bool valid = true;

			function_16e1b0(reference->first_index, (s_16e1b0_list const *)section, &unknown, &part_index, &part_offset);
			for (long vertex = 0; vertex < 3; vertex++)
			{
				valid = valid && structure_section_get_vertex(section, indices[vertex], &vertices[vertex]);
			}
			if (valid && function_11e800(&vertices[0], &vertices[1], &vertices[2], &point, &triangle->u, &triangle->v))
			{
				triangle->part_offset = part_offset;
				triangle->lightmap_part_index = reference->part_index;
				triangle->part_index = part_index;
				triangle->unknown18 = unknown;
				triangle->cluster_index = range->cluster_index;
				triangle->instance_index = collision->instance_index;
				result = true;
				break;
			}
		}
	}
	return result;
}

#include "unknown_1efac0.h"

struct s_vertex_source_23ac30;
struct s_vertex_bounds_23ac30;
struct s_table_holder;
void *function_1efd80(s_table_holder *holder, dword position);
void function_23ac30(s_vertex_source_23ac30 const *source, long index,
	s_vertex_bounds_23ac30 const *bounds, point3f *point, long *node);

// @retail 0x14a8e0
bool __stdcall function_14a8e0(s_structure_collision_result const *collision,
	long model_index, s_structure_lightmap_triangle *triangle)
{
	triangle->unknown18 = NONE;
	*(long *)((byte *)triangle + 8) = NONE;
	*(long *)((byte *)triangle + 0x14) = NONE;
	triangle->part_index = NONE;
	triangle->part_offset = NONE;
	triangle->lightmap_part_index = NONE;
	*(long *)((byte *)triangle + 0xc) = NONE;
	*(long *)((byte *)triangle + 0x10) = NONE;
	byte *model = g_4e3b44[model_index & 0xffff].bytes;
	long object_index = *(long *)((byte const *)collision + 0x40);
	byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	signed char *regions = (signed char *)(object + *(short *)(object + 0x11a));
	short region = *(short *)((byte const *)collision + 0x44);
	short node = *(short *)((byte const *)collision + 0x46);
	volatile bool result = false;
	if (regions[region] != -1)
	{
		byte *region_data = *(byte **)(model + 0x20) + region * 16;
		*(long *)((byte *)triangle + 0x14) =
			*(short *)(*(byte **)(region_data + 0xc) + regions[region] * 16 + 0xc);
	}
	else
		*(long *)((byte *)triangle + 0x14) = 0xff;
	long section_index = *(long *)((byte *)triangle + 0x14);
	if (section_index == 0xff) return result;
	byte *section = *(byte **)(model + 0x28) + section_index * 0x5c;
	s_vertex_bounds_23ac30 const *bounds = *(s_vertex_bounds_23ac30 const **)(model + 0x18);
	if (!function_12de70((s_geometry_block_info *)(section + 0x38), 3)) return result;
	object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	point3f point = collision->point;
	if ((unsigned long)*(short *)(object + 0x114) / 0x34 > 0)
	{
		transform4x3f *matrix = (transform4x3f *)(object + *(short *)(object + 0x116)) + node;
		function_142700(matrix, &collision->point, &point);
		matrix = (transform4x3f *)(*(byte **)(model + 0x4c) + node * 0x60 + 0x28);
		function_142700(matrix, &point, &point);
	}
	s_lookup lookup;
	if (!lookup.initialize(object_index)) return result;
	byte *surface_set = (byte *)function_1efd80((s_table_holder *)&lookup,
		*(dword *)((byte const *)collision + 0x48));
	byte *section_mesh = *(byte **)(section + 0x34);
	long node_count = *(long *)(section_mesh + 0x64);
	if (node_count <= 0) return result;
	byte *nodes = *(byte **)(section_mesh + 0x68);
	long node_index;
	for (node_index = 0; node_index < node_count; ++node_index)
		if (nodes[node_index] == node) break;
	if (node_index == node_count || node_index == NONE) return result;
	byte *section_groups = *(byte **)(model + 0x80) + section_index * 8;
	byte *surface_group = *(byte **)(section_groups + 4) + node_index * 16;
	if (*(long *)surface_group != *(long *)(surface_set + 0x10)) return result;
	s_structure_surface_range *range = (s_structure_surface_range *)*(byte **)(surface_group + 4)
		+ collision->surface_range_index;
	for (long reference_index = range->first; reference_index < range->first + range->count; ++reference_index)
	{
		if (reference_index < 0 || reference_index >= *(long *)(surface_group + 8)) continue;
		s_structure_triangle_reference *reference =
			(s_structure_triangle_reference *)*(byte **)(surface_group + 0xc) + reference_index;
		long surface = reference->surface_index;
		if (surface == NONE) return result;
		if (surface >= *(long *)surface_set) continue;
		byte *surfaces = *(byte **)(surface_set + 4);
		if (*(word *)(surfaces + surface * 8) != (collision->plane_index & 0x7fff)) continue;
		section_mesh = *(byte **)(section + 0x34);
		word *indices = *(word **)(section_mesh + 0x24) + reference->first_index;
		point3f vertices[3];
		long vertex_node;
		long vertex = 0;
		do
		{
			function_23ac30((s_vertex_source_23ac30 const *)section_mesh, indices[vertex],
				bounds, &vertices[vertex], &vertex_node);
			++vertex;
		} while (vertex < 3);
		if (function_11e800(&vertices[0], &vertices[1], &vertices[2], &point, &triangle->u, &triangle->v))
		{
			triangle->unknown18 = NONE;
			*(long *)((byte *)triangle + 8) = object_index;
			triangle->part_index = 0;
			result = true;
			long part_count = *(long *)section_mesh;
			byte *parts = *(byte **)(section_mesh + 4);
			long previous_index = triangle->part_offset;
			for (long part = 0; part < part_count; ++part)
			{
				word first = *(word *)(parts + part * 0x48 + 6);
				word count = *(word *)(parts + part * 0x48 + 8);
				if (previous_index >= first && previous_index < first + count)
				{
					triangle->part_index = part;
					break;
				}
			}
			triangle->part_offset = reference->first_index;
			triangle->lightmap_part_index = reference->part_index;
			*(long *)((byte *)triangle + 0xc) = model_index;
			*(long *)((byte *)triangle + 0x1c) = region;
			*(long *)((byte *)triangle + 0x2c) = vertex_node;
			return result;
		}
	}
	return result;
}
