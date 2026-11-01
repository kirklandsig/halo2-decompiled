// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_23AA70.CPP: a pass over the structure's clusters */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

/* the structure's clusters (0xb0 bytes each), as this file reads them */
struct s_cluster_23aa
{
	byte unknown00[0x7c];
	short value7c;
	word count7e;
	byte unknown80[0xb0 - 0x80];
};

struct s_cluster_spawn_23aa;

struct s_structure_23aa
{
	byte unknown00[0x9c];
	long cluster_count;
	s_cluster_23aa *clusters;
	byte unknowna4[0xf4 - 0xa4];
	long valuef4;
	s_cluster_spawn_23aa *entries;
};

void function_17d710(short cluster_index);

/* calls 0x17d710 on every cluster that has a value7c and a count */
// @retail 0x23aa70
void function_23aa70(void)
{
	s_structure_23aa *structure = (s_structure_23aa *)g_4e0348;

	if (structure->valuef4)
	{
		for (short i = 0; i < structure->cluster_count; i++)
		{
			s_cluster_23aa *cluster = &structure->clusters[i];

			if (cluster->value7c != NONE && cluster->count7e > 0)
			{
				function_17d710(i);
			}
		}
	}
}

struct s_cluster_spawn_23aa
{
	point3f position;
	byte palette_index;
	byte unknown0d;
	char yaw;
	char pitch;
};

struct s_cluster_palette_23aa
{
	long unused;
	long tag_index;
};

struct s_cluster_palette_globals_23aa
{
	byte unknown00[0x144];
	s_cluster_palette_23aa *types;
};

struct s_effect_source;
void function_17e670(s_effect_source *source, point3f const *point, long tag_index,
	vector3f const *direction, real radius, long first, long second, long third);

/* a cluster's bit in a mask with one bit per cluster */
#define CLUSTER_MASK_TEST(mask, index) ((mask)[(index) >> 5] & (1 << ((index) & 0x1f)))

/* Starts and stops cluster effects when the active cluster masks change. */
// score 16 (was 16): frame and prefix agree; palette/entry register ownership differs from +0xba.
// BLOCKED-BY 0x17e670: its body takes the effect source in edx and the point in ecx here,
// retail passes the source in ecx and the point in edx (also at 0x17a8a0).
// Shared entry 17; entry plus palette 17; shared index 32; const shared entry 17;
// direct tag 52; tag after angles 28; tag after direction 52; shared direction helper 17.
// Further trials: entry/palette ownership and contributor effect-call convention remain.
// uniform structure 125; cluster at use 92.
// spawn at use 82; structure/cluster 105.
// structure/spawn 121; cluster/spawn 136.
// all three owners 149; all owners/tag at call 150.
// Position pointer 16; aggregate direction 16; direction helper with position 16;
// that combination with shared entry 17; entry pointer arithmetic 16;
// full-width first entry 32; short first entry 16; short spawn cursor 78.
// @retail 0x23aad0
void __stdcall function_23aad0(long previous_address, long current_address, long count)
{
	s_structure_23aa *structure = (s_structure_23aa *)g_4e0348;
	if (structure->valuef4)
	{
		for (short i = 0; i < count; i++)
		{
			s_cluster_23aa *cluster = &structure->clusters[i];
			if (cluster->value7c != NONE && cluster->count7e > 0)
			{
				dword const *previous_mask = (dword const *)previous_address;
				dword const *current_mask = (dword const *)current_address;
				if (CLUSTER_MASK_TEST(previous_mask, i) && !CLUSTER_MASK_TEST(current_mask, i))
				{
					function_17d710(i);
				}
				else if (!CLUSTER_MASK_TEST(previous_mask, i) && CLUSTER_MASK_TEST(current_mask, i))
				{
					for (long j = 0; j < cluster->count7e; j++)
					{
						long tag_index = ((s_cluster_palette_globals_23aa *)g_4e0350)->types[structure->entries[cluster->value7c + j].palette_index].tag_index;
						s_cluster_spawn_23aa *entry = &structure->entries[cluster->value7c + j];
						real yaw = entry->yaw * 0.02473695f;
						real pitch = entry->pitch * 0.012368475f;
						vector3f direction;
						direction.i = (real)(cos(pitch) * cos(yaw));
						direction.j = (real)(cos(pitch) * sin(yaw));
						direction.k = (real)sin(pitch);
						function_17e670(0, &entry->position, tag_index, &direction, 1.0f, 1, NONE, 0);
					}
				}
			}
		}
	}
}

struct s_vertex_bounds_23ac30
{
	real x0, x1, y0, y1, z0, z1;
};

struct s_vertex_buffer_23ac30
{
	byte format;
	byte unknown01[7];
	long offset;
	byte const *data;
};

struct s_vertex_source_23ac30
{
	byte unknown00[0x3c];
	s_vertex_buffer_23ac30 const *buffer;
};

struct s_vertex_with_node_23ac30
{
	point3f position;
	byte node;
	byte unknown0d[3];
};

struct s_packed_vertex_23ac30
{
	short x, y, z;
};

struct s_packed_vertex_with_node_23ac30
{
	short x, y, z;
	byte node;
	byte unknown07;
};

PRIVATE __forceinline void vertex_decode_ranges_23ac30(short x, short y, short z,
	s_vertex_bounds_23ac30 const *bounds, vector3f const *range, point3f *point)
{
	point3f result;
	result.x = (((real)x * 2.0f + 1.0f) * 0.000007629510946571827f + 0.5f) * range->i + bounds->x0;
	result.y = (((real)y * 2.0f + 1.0f) * 0.000007629510946571827f + 0.5f) * range->j + bounds->y0;
	result.z = (((real)z * 2.0f + 1.0f) * 0.000007629510946571827f + 0.5f) * range->k + bounds->z0;
	*point = result;
}

PRIVATE __forceinline void vertex_decode_23ac30(short x, short y, short z,
	s_vertex_bounds_23ac30 const *bounds, point3f *point)
{
	vector3f range = { bounds->x1 - bounds->x0, bounds->y1 - bounds->y0, bounds->z1 - bounds->z0 };
	vertex_decode_ranges_23ac30(x, y, z, bounds, &range, point);
}

/* reads one vertex position (and its node, for the formats that store one) from a
   buffer in one of four formats; packed formats decode through the bounds */
// @retail 0x23ac30
void function_23ac30(s_vertex_source_23ac30 const *source, long index,
	s_vertex_bounds_23ac30 const *bounds, point3f *point, long *node)
{
	s_vertex_buffer_23ac30 const *buffer = source->buffer;
	switch (buffer->format)
	{
	case 1:
		{
			point3f const *vertices = (point3f const *)(buffer->data + buffer->offset);
			*point = vertices[index];
			if (node) *node = 0;
		}
		break;
	case 2:
		{
			s_packed_vertex_23ac30 const *vertices = (s_packed_vertex_23ac30 const *)(buffer->data + buffer->offset);
			vertex_decode_23ac30(vertices[index].x, vertices[index].y, vertices[index].z, bounds, point);
			if (node) *node = 0;
		}
		break;
	case 3:
		{
			s_vertex_with_node_23ac30 const *vertices = (s_vertex_with_node_23ac30 const *)(buffer->data + buffer->offset);
			*point = vertices[index].position;
			if (node) *node = vertices[index].node;
		}
		break;
	case 4:
		{
			s_packed_vertex_with_node_23ac30 const *vertices = (s_packed_vertex_with_node_23ac30 const *)(buffer->data + buffer->offset);
			vertex_decode_23ac30(vertices[index].x, vertices[index].y, vertices[index].z, bounds, point);
			if (node) *node = vertices[index].node;
		}
		break;
	default:
		*point = *g_468788;
		if (node) *node = 0;
		break;
	}
}
