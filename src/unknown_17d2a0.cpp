// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_17D2A0.CPP: the decals (entry 37 of the lifecycle table) */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"
#include "unknown_123b30.h"
#include "physical_memory.h"
#include <string.h>
#include <xtl.h>

D3DResource *g_509444;

/* a decal (g_4ea950, 0x40 bytes) */
struct s_decal_datum
{
	short salt;
	union
	{
		byte flags;
		struct
		{
			byte flag0 : 1;
			byte flag1 : 1;
			byte : 6;
		};
	};
	char cell_x;
	long definition_index;
	short unknown08;
	short cell_y;
	point3f position;
	long creation_time;
	real lifetime;
	real fade_time;
	union
	{
		dword color;
		struct
		{
			byte unknown24[3];
			byte alpha;
		};
	};
	byte unknown28[8];
	long previous_index;
	long next_index;
	long first_index;
	long next_in_group_index;
};

/* the decal cells in the game state (g_4ea94c, 0x3c2c bytes) */
struct s_decal_globals
{
	long cells[7][512];
	long unassigned_index;
	long fading_count;
	long permanent_count;
	byte unknown380c[0x3c2c - 0x380c];
};

s_decal_globals *g_4ea94c;

/* the first decal of each decal definition (unknown_03d380.cpp) */
extern dword g_4c8798[256];

void function_163ba0(dword *crc_reference, void const *buffer, long buffer_size);
void function_43890(void);
__declspec(noinline) void function_43990(void);
void function_23aa70(void);
void __stdcall function_23aad0(long a, long b, long c);
void function_17d5f0(bool permanent);
void function_17d860(long decal_index);

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_decal_structure_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_decal_structure_bsp
{
	byte unknown00[0x30];
	s_decal_structure_leaf *leaves;
};

#define DECAL(index) ((s_decal_datum *)g_4ea950->data + ((index) & 0xffff))

// @retail 0x17d220
void function_17d220(void)
{
	g_4ea950 = data_new_inlined("decals", 0x400, sizeof(s_decal_datum), 0, g_510c2c);

	long size = sizeof(s_decal_globals);
	byte *base = game_state_globals.base_address + game_state_globals.cpu_allocation_size;

	game_state_globals.cpu_allocation_size += sizeof(s_decal_globals);
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
	g_4ea94c = (s_decal_globals *)base;
	function_43890();
}

// @retail 0x17d2a0
void function_17d2a0(void)
{
	g_4ea950 = 0;
	if (g_509444)
	{
		D3DResource_Release(g_509444);
		g_509444 = 0;
	}
	g_509448->allocator->deallocate(g_509448);
}

// @retail 0x17d2e0
void decals_initialize_for_new_map(void)
{
	s_decal_globals *globals = g_4ea94c;
	s_record_pool *decals = g_4ea950;

	memset(globals, 0, sizeof(*globals));
	memset(globals->cells, 0xff, sizeof(globals->cells));
	globals->unassigned_index = NONE;
	globals->fading_count = 0;
	globals->permanent_count = 0;
	decals->valid = true;
	record_pool_release_all(decals);
}

// @retail 0x17d330
void function_17d330(void)
{
	function_17d5f0(true);
	physical_memory_flush((s_physical_object *)g_509448);
	g_4ea950->valid = false;
}

// @retail 0x17ce00
void decal_link(short cell_y, short cell_x, long decal_index)
{
	long *cell = &g_4ea94c->cells[0][(cell_x << 9) + cell_y];
	long next_index = *cell;
	s_decal_datum *decal = DECAL(decal_index);

	decal->previous_index = NONE;
	decal->next_index = next_index;
	decal->cell_y = cell_y;
	decal->cell_x = (char)cell_x;
	decal->unknown08 = NONE;
	if (next_index != NONE)
		DECAL(next_index)->previous_index = decal_index;
	*cell = decal_index;
}

// @retail 0x17d100
long __stdcall function_17d100(long cell_y, long source_index)
{
	long decal_index = record_pool_allocate(g_4ea950);

	if (decal_index != NONE)
	{
		s_decal_datum *decal = DECAL(decal_index);
		s_decal_datum *source = DECAL(source_index);
		short salt = decal->salt;

		*decal = *source;
		decal->salt = salt;
		decal->first_index = source_index;
		decal->next_in_group_index = NONE;
		if (source->flag1)
			g_4ea94c->permanent_count++;
		if (source->flag0)
			g_4ea94c->fading_count++;
		decal_link((short)cell_y, source->cell_x, decal_index);
	}
	return decal_index;
}

// @retail 0x17d350
void decals_update_locations(void)
{
	if (g_4ea950->valid)
	{
		long decal_index = g_4ea94c->unassigned_index;

		while (decal_index != NONE)
		{
			s_decal_datum *decal = DECAL(decal_index);
			long next_index = decal->next_index;

			if (g_4686c4 != NONE)
			{
				long leaf_index = function_14a280(g_4e033c, 0, &decal->position);

				if (leaf_index != NONE)
				{
					s_location cluster_location;
					cluster_location.leaf_index = leaf_index;
					volatile short &cluster_index = cluster_location.cluster_index;
					short cluster_value = ((s_decal_structure_bsp *)g_4e0348)->leaves[leaf_index].cluster_index;
					cluster_index = cluster_value;

					if (cluster_value != NONE)
					{
						if (next_index != NONE)
							DECAL(next_index)->previous_index = decal->previous_index;
						if (decal->previous_index != NONE)
							DECAL(decal->previous_index)->next_index = decal->next_index;
						else
							g_4ea94c->unassigned_index = decal->next_index;
						decal_link(cluster_index, decal->cell_x, decal_index);
					}
				}
			}
			decal_index = next_index;
		}
	}
}

// @retail 0x17d440
void function_17d440(void)
{
	function_23aa70();

	if (g_4ea950->valid)
	{
		s_decal_globals *globals = g_4ea94c;

		for (long cell_y = 0; cell_y < 512; cell_y++)
		{
			for (long cell_x = 0; cell_x < 7; cell_x++)
			{
				long first_index = globals->cells[cell_x][cell_y];
				long decal_index = first_index;

				while (decal_index != NONE)
				{
					s_decal_datum *decal = (s_decal_datum *)g_4ea950->data + (decal_index & 0xffff);
					long next_index = decal->next_index;

					decal->cell_y = NONE;
					if (next_index == NONE)
					{
						decal->next_index = globals->unassigned_index;
						if (globals->unassigned_index != NONE)
							((s_decal_datum *)g_4ea950->data + (globals->unassigned_index & 0xffff))->previous_index = decal_index;
						globals->unassigned_index = first_index;
						globals->cells[cell_x][cell_y] = NONE;
					}
					decal_index = next_index;
				}
			}
		}
	}
}

PRIVATE __forceinline long data_scan_17d5f0(s_record_pool *data, long index)
{
	if (index >= 0)
	{
		long count = data->high_water_index;
		if (index < count)
		{
			dword const *bits = data->bitmap;
			do
			{
				if (bits[index >> 5] & (1 << (index & 31)))
					return index;
				++index;
			} while (index < count);
		}
	}
	return NONE;
}

// @retail 0x17d5f0
void function_17d5f0(bool permanent)
{
	s_record_pool *decals = g_4ea950;

	if (decals->valid)
	{
		s_decal_globals *globals = g_4ea94c;
		long index = NONE;
		s_decal_datum *decal;

		for (;;)
		{
			long next_index = data_scan_17d5f0(decals, index + 1);
			if (next_index == NONE)
				break;
			decal = (s_decal_datum *)(decals->data + decals->size * next_index);
			if (!decal)
				break;
			if (decal->flag0)
			{
				decal->flag0 = false;
				globals->fading_count--;
			}
			if (permanent && decal->flag1)
			{
				decal->flag1 = false;
				globals->permanent_count--;
			}
			index = next_index;
		}
	}
}

// @retail 0x17d5e0
void __stdcall decals_render(long a, long b, long c)
{
	long const *a_reference = &a;
	long const *b_reference = &b;
	long const *c_reference = &c;
	function_23aad0(*a_reference, *b_reference, *c_reference);
}

PRIVATE __forceinline s_decal_datum *data_step_17ce60(s_record_pool_iterator *it)
{
	s_record_pool *data = it->data;
	long next = data_scan_17d5f0(data, it->index + 1);
	if (next == NONE)
		return 0;
	s_decal_datum *datum = (s_decal_datum *)(data->data + data->size * next);
	it->index = next;
	it->datum_index = (datum->salt << 16) | next;
	return datum;
}

// @retail 0x17ce60
bool function_17ce60(void)
{
	s_record_pool_iterator iterator;
	iterator.data = g_4ea950;
	s_decal_globals *globals = g_4ea94c;
	iterator.index = NONE;
	short attempts = 0;

	for (;;)
	{
		if (globals->fading_count <= 0x80)
			return true;
		s_decal_datum *decal = data_step_17ce60(&iterator);
		if (!decal)
		{
			iterator.data = g_4ea950;
			iterator.index = NONE;
			attempts++;
			if (attempts >= 100)
				break;
			continue;
		}

		if (decal->first_index == iterator.datum_index && !decal->flag1 && decal->flag0)
		{
			long chance = random_next(&g_4e7408->seed) * 100;

			if (chance < 41 * 0xffff || decal->cell_y == NONE)
			{
				decal->flag0 = false;
				globals->fading_count--;
				for (long member_index = decal->next_in_group_index; member_index != NONE; )
				{
					s_decal_datum *member = DECAL(member_index);

					if (member->flag0)
					{
						member->flag0 = false;
						globals->fading_count--;
					}
					member_index = member->next_in_group_index;
				}
			}
		}
	}
	return false;
}

// @retail 0x17cfa0
long function_17cfa0(long first_index, long definition_index, short cell_x, short cell_y, bool permanent)
{
	bool first = false;
	long *first_index_reference = &first_index;
	s_record_pool *decals = g_4ea950;
	long decal_index = record_pool_allocate(decals);

	if (decal_index != NONE)
	{
		s_decal_datum *decal = DECAL(decal_index);

		decal->definition_index = definition_index;
		if ((*first_index_reference) != NONE)
		{
			decal->first_index = (*first_index_reference);
			if (DECAL((*first_index_reference))->flag0)
			{
				decal->flag0 = true;
				g_4ea94c->fading_count++;
			}
			else
			{
				decal->flag0 = false;
			}
		}
		else
		{
			decal->first_index = decal_index;
			first = true;
		}
		decal->next_in_group_index = NONE;
		if (DECAL(decal_index)->first_index == decal_index)
			g_4c8798[DECAL(decal_index)->definition_index & 0xffff] = decal_index;
		if (permanent)
		{
			decal->flags = 2;
			g_4ea94c->permanent_count++;
		}
		else if (random_next(&g_4e7408->seed) * 100 < 10 * 0xffff && first)
		{
			decal->flags = 1;
			g_4ea94c->fading_count++;
		}
		else
		{
			decal->flags = 0;
		}
		if (g_4ea94c->fading_count > 0x100 && !function_17ce60())
		{
			record_pool_release(decals, decal_index);
			return NONE;
		}
		decal_link(cell_y, cell_x, decal_index);
	}
	else if (definition_index != NONE)
	{
		long index = definition_index & 0xffff;

		if ((index < 0 ? 0 : (index > 0x3ff ? 0x3ff : index)) == index)
			g_4c8798[index] = NONE;
	}
	return decal_index;
}

// @retail 0x17ccd0
bool function_17ccd0(long decal_index)
{
	s_decal_datum *decal = DECAL(decal_index);
	real age = (real)(g_510c54->game_time - decal->creation_time) * g_510c54->rate;
	bool result = false;

	decal->alpha = 0xff;
	if (!decal->flag1)
	{
		if (decal->lifetime != 0.0f && !(decal->lifetime > age))
		{
			if (decal->flag0)
			{
				decal->flag0 = false;
				g_4ea94c->fading_count--;
				for (long index = decal->next_in_group_index; index != NONE; )
				{
					s_decal_datum *member = DECAL(index);

					if (member->flag0)
					{
						member->flag0 = false;
						g_4ea94c->fading_count--;
					}
					index = member->next_in_group_index;
				}
			}
			((s_physical_object *)g_509448)->block_delete(decal->definition_index);
			result = true;
		}
		else if (decal->lifetime > 0.0f && decal->fade_time > 0.0f)
		{
			real remaining = decal->lifetime - age;

			if (decal->fade_time > remaining)
			{
				real value = remaining / decal->fade_time * 256.0f;
				real alpha = 0.0f > value ? 0.0f : (value > 255.0f ? 255.0f : value);

				decal->color = ((long)alpha << 24) | (decal->color & 0xffffff);
			}
		}
	}
	return result;
}

// @retail 0x17cc60
void function_17cc60(long decal_index)
{
	s_decal_datum *decal = DECAL(decal_index);

	if (decal->first_index == decal_index)
	{
		long index = decal->next_in_group_index;
		long alpha = decal->alpha;

		if (!function_17ccd0(decal->first_index))
		{
			while (index != NONE)
			{
				s_decal_datum *member = DECAL(index);

				index = member->next_in_group_index;
				member->color = (alpha << 24) | (member->color & 0xffffff);
			}
		}
	}
}

// @retail 0x17d690
void function_17d690(void)
{
	s_record_pool_iterator iterator;
	iterator.data = g_4ea950;
	if (iterator.data->valid)
	{
		function_43990();
		iterator.index = NONE;
		while (data_iterator_next_inlined(&iterator))
			function_17cc60(iterator.datum_index);
	}
}

// @retail 0x17d520
void function_17d520(void)
{
	function_17d5f0(false);

	s_physical_object *manager = (s_physical_object *)g_509448;

	if (manager->time == 0x7fffffff)
		physical_memory_reset_time(manager);
	else
		manager->time++;
	for (long i = 0; i < 8; i++)
		manager->limits[i] = 0x7fffffff;

	s_record_pool *decals = g_4ea950;
	long index = NONE;

	for (;;)
	{
		long next_index = data_scan_17d5f0(decals, index + 1);
		if (next_index == NONE)
			break;

		s_decal_datum *decal = (s_decal_datum *)(decals->data + decals->size * next_index);

		long datum_index = (decal->salt << 16) | next_index;
		index = next_index;
		if (decal->first_index == datum_index && !decal->flag1)
			((s_physical_object *)g_509448)->block_delete(decal->definition_index);
	}
}

// @retail 0x17d710
void function_17d710(short cluster_index)
{
	if (g_4ea950->valid)
	{
		for (short i = 0; i < 7; i++)
		{
			long decal_index;

			if (cluster_index == NONE)
			{
				if (i != 0)
					continue;
				decal_index = g_4ea94c->unassigned_index;
			}
			else
			{
				decal_index = g_4ea94c->cells[i][cluster_index];
			}
			while (decal_index != NONE)
			{
				s_decal_datum *decal = DECAL(decal_index);
				long next_index = decal->next_index;

				if (decal->flag1 && decal->first_index == decal_index)
				{
					decal->flag1 = false;
					g_4ea94c->permanent_count--;
					for (long index = decal->next_in_group_index; index != NONE; )
					{
						s_decal_datum *member = DECAL(index);

						if (member->flag1)
						{
							member->flag1 = false;
							g_4ea94c->permanent_count--;
						}
						index = member->next_in_group_index;
					}
					((s_physical_object *)g_509448)->block_delete(decal->definition_index);
				}
				decal_index = next_index;
			}
		}
	}
}

// @retail 0x17d810
void decal_delete_group(long decal_index)
{
	long next_index = DECAL(decal_index)->next_in_group_index;

	function_17d860(decal_index);
	while (next_index != NONE)
	{
		long index = next_index;

		next_index = DECAL(index)->next_in_group_index;
		function_17d860(index);
	}
}

// @retail 0x17d860
void function_17d860(long decal_index)
{
	s_decal_datum *decal = DECAL(decal_index);

	if (decal->next_index != NONE)
		DECAL(decal->next_index)->previous_index = decal->previous_index;
	if (decal->previous_index != NONE)
		DECAL(decal->previous_index)->next_index = decal->next_index;
	else if (decal->cell_y == NONE)
		g_4ea94c->unassigned_index = decal->next_index;
	else
		g_4ea94c->cells[0][(decal->cell_x << 9) + decal->cell_y] = decal->next_index;
	record_pool_release(g_4ea950, decal_index);
}
