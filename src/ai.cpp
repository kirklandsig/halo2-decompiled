// @flags /O2 /Ob1 /arch:SSE /Gr
/* AI.CPP: the ai globals, the ai's view of the players and units, and small
   ai helpers (0x1c7790..0x1caxxx) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_123b30.h"
#include "data_array.h"
#include "unknown_0259a0.h"
#include "unknown_1e46c0.h"
#include "unknown_0d0690.h"
#include <string.h>
#include <math.h>

s_ai_player *g_4f55cc;

void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);

struct s_ai_capsule_view
{
	byte active;
	byte unknown01[3];
	point3f center;
	real value10;
	real value14;
	real height;
	long value1c;
	long object_index;
	real radius;
};

// @retail 0x1c8940
void function_1c8940(s_ai_capsule_view *capsule, long object_index, long value)
{
	real height;
	real radius;
	function_df5f0(object_index, &capsule->center, &height, &radius);
	*(volatile byte *)&capsule->active = height == 0.0f ? 1 : 0;
	*(volatile real *)&capsule->value10 = 0.0f;
	*(volatile real *)&capsule->value14 = 0.0f;
	capsule->height = height;
	capsule->radius = radius + 0.15f;
	capsule->value1c = value;
	capsule->object_index = object_index;
}

// @retail 0x1c7fe0
inline void ai_players_reset(void)
{
	long i;

	for (i = 0; i < MAXIMUM_AI_PLAYERS; i++)
	{
		g_4f55cc[i].player_index = NONE;
		g_4f55cc[i].unit_index = NONE;
		g_4f55cc[i].unknown0a = 0;
	}
}

// @retail 0x1c7fa0
void ai_globals_clear(void)
{
	memset(g_4f55d0, 0, sizeof(s_ai_globals));
	ai_players_reset();
}

// @retail 0x1c80f0
void ai_player_add(long player_index)
{
	if (g_4e6948->state == 1)
	{
		bool added = false;
		long i;

		for (i = 0; i < MAXIMUM_AI_PLAYERS; i++)
		{
			s_ai_player *player = &g_4f55cc[i];

			if (player->player_index == NONE && !added)
			{
				memset(player, 0, sizeof(s_ai_player));
				player->player_index = player_index;
				player->unit_index = NONE;
				player->unknown08 = NONE;
				player->unknown0a = 0;
				added = true;
			}
		}
	}
}

// @retail 0x1c8010
void ai_globals_initialize_for_new_map(void)
{
	s_ai_globals *globals = g_4f55d0;
	s_record_pool_iterator iterator;

	memset(globals, 0, sizeof(s_ai_globals));
	globals->enabled = true;
	globals->unknown02 = true;
	globals->unknown14 = NONE;
	globals->unknown340 = true;
	globals->unknown20 = true;
	globals->unknown364 = NONE;
	globals->unknown36c = NONE;
	globals->unknown24.clear();
	globals->unknown2c.clear();
	globals->unknown34.clear();
	ai_players_reset();
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		ai_player_add(iterator.datum_index);
	}
}

// @retail 0x1c8150
inline short ai_player_index_get(long player_index)
{
	short index = NONE;
	long i;

	for (i = 0; i < MAXIMUM_AI_PLAYERS; i++)
	{
		if (g_4f55cc[i].player_index == player_index)
		{
			index = (short)i;
			break;
		}
	}
	return index;
}

// @retail 0x1c8180
s_ai_player *ai_player_get(long player_index)
{
	s_ai_player *player = NULL;
	long index = ai_player_index_get(player_index);

	if (index != NONE)
	{
		player = &g_4f55cc[index];
	}
	return player;
}

// @retail 0x1c8390
void ai_players_unit_deleted(long unit_index)
{
	long i = 0;

	do
	{
		s_ai_player *player = &g_4f55cc[i];

		if (player->unit_index == unit_index)
		{
			player->unit_index = NONE;
			player->unknown08 = NONE;
			player->unknown0a = 0;
		}
		i++;
	}
	while (i < MAXIMUM_AI_PLAYERS);
}

/* the objects (0bad50.cpp) */
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf40(long object_index);

struct s_ai_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

struct s_ai_unit
{
	byte unknown000[0x248];
	long unknown248;
	long unknown24c;
};

struct s_ai_impulse_unit
{
	byte unknown000[0x12c];
	long actor_index;
};

void function_1e28b0(long actor_index, vector3f const *direction, real magnitude);

// @retail 0x1c9e10
void function_1c9e10(long unit_index, vector3f const *direction, real shake)
{
	if (g_4f55d0->active)
	{
		s_ai_impulse_unit *unit = (s_ai_impulse_unit *)((s_ai_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
		if (unit->actor_index != NONE)
			function_1e28b0(unit->actor_index, direction, shake);
	}
}

bool function_1df560(short team_a, short team_b);
void function_25c050(long player_index, long actor_index);

/* whether the unit is a player's unit friendly to the actor; if so and asked
   to, tells the actor about the player */
// @retail 0x1c9500
bool function_1c9500(long unit_index, long actor_index, bool notify)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (unit_index != NONE)
	{
		s_slot_object_view *unit = object_get(unit_index);

		if (unit->player_index != NONE && !function_1df560(unit->team, actor->unknown024))
		{
			result = true;
			if (notify)
			{
				function_25c050(unit->player_index, actor_index);
			}
		}
	}
	return result;
}

// @retail 0x1c9580
long function_1c9580(long object_index, bool a)
{
	long result = NONE;

	if (object_index != NONE)
	{
		s_ai_unit *unit = (s_ai_unit *)function_badc0(object_index, 3);

		if (unit)
		{
			if (a && unit->unknown24c != NONE)
			{
				result = unit->unknown24c;
			}
			else if (unit->unknown248 != NONE)
			{
				result = unit->unknown248;
			}
			else
			{
				result = object_index;
			}
		}
	}
	return result;
}

/* the ai globals tag, a block at +0xc8 of the tag header globals */
struct s_ai_globals_definition
{
	real unknown00;
	byte unknown04[4];
	real unknown08;
	byte unknown0c[4];
	real unknown10;
	byte unknown14[4];
	real unknown18;
	byte unknown1c[4];
	real unknown20;
	real unknown24;
	real unknown28;
	real unknown2c;
};

struct s_ai_tag_header_globals
{
	byte unknown00[0xc8];
	long ai_globals_count;
	s_ai_globals_definition *ai_globals;
};

/* the actor field the mask below collects */
struct s_ai_mask_actor
{
	byte unknown000[0x254];
	short unknown254;
};

/* sets the bit of each actor's index at +0x254 in a mask of count bits */
// @retail 0x1c9b50
bool function_1c9b50(dword *mask, short count)
{
	memset(mask, 0, ((count + 31) >> 5) * sizeof(dword));
	if (g_4f55d0->active)
	{
		s_actor_iterator iterator;
		s_ai_mask_actor *actor;

		function_x66da2b(&iterator, true);
		while ((actor = (s_ai_mask_actor *)function_1e46c0(&iterator)) != NULL)
		{
			if (actor->unknown254 >= 0 && actor->unknown254 < count)
			{
				mask[actor->unknown254 >> 5] |= 1 << (actor->unknown254 & 31);
			}
		}
	}
	return true;
}

// @retail 0x1c9e50
real function_1c9e50(short index)
{
	s_ai_tag_header_globals *globals = (s_ai_tag_header_globals *)g_4e034c;
	real result = 0.0f;

	if (globals && globals->ai_globals_count > 0)
	{
		s_ai_globals_definition *definition = globals->ai_globals;

		switch (index)
		{
		case 0:
			result = 0.0f;
			break;
		case 1:
			result = definition->unknown00;
			break;
		case 2:
			result = definition->unknown08;
			break;
		case 3:
			result = definition->unknown10;
			break;
		case 4:
			result = definition->unknown18;
			break;
		case 5:
			result = definition->unknown20;
			break;
		case 6:
			result = definition->unknown24;
			break;
		case 7:
			result = definition->unknown28;
			break;
		case 8:
			result = definition->unknown2c;
			break;
		}
	}
	return result;
}

// @retail 0x1c9ee0
real function_1c9ee0(real fraction)
{
	if (fraction < 0.0f)
	{
		fraction = 0.0f;
	}
	else if (fraction > 1.0f)
	{
		fraction = 1.0f;
	}
	return 1.0f - (real)pow(1.0f - fraction, g_510c54->rate);
}

// @retail 0x1caa10
long function_1caa10(long object_index)
{
	long parent_index = function_baf40(object_index);

	if (((s_ai_object_header *)g_4e0300->data)[parent_index & 0xffff].type != 1)
	{
		parent_index = object_index;
	}
	return parent_index;
}

/* what 0x1c8440 scales (its flags at +4) */
struct s_ai_scale_source
{
	byte unknown00[4];
	byte flags;
};

// @retail 0x1c8440
bool function_1c8440(long actor_index, real *value, s_ai_scale_source const *source)
{
	bool result = false;

	if (actor_index != NONE)
	{
		s_actor_view *actor = actor_get(actor_index);

		if ((source->flags & 8) && actor->unknown7c0 > 0.0f)
		{
			*value *= actor->unknown7c0;
			result = true;
		}
	}
	return result;
}
/* the ai's data arrays and game state (0x1c7790 builds them) */
s_record_pool *g_51e9dc;
s_record_pool *g_502404;
s_record_pool *g_51ecb4;
void *g_5044cc;
void *g_5044d0;
void *g_5047f4;
short g_4f5768;

void function_1dfae0(void);

struct s_output_entry;
extern s_output_entry *g_4f93a0;
struct hash_table;
__declspec(noinline) hash_table *function_13e1a0(char const *name, long data_size, long bucket_count,
	dword (__stdcall *hash_proc)(void const *), bool (__stdcall *compare_proc)(void const *, void const *),
	long maximum_count, c_data_allocator *allocator);
long __stdcall function_25dd20(long key);
bool __stdcall function_25dd30(long a, long b);

PRIVATE __forceinline s_record_pool *function_1dfae1(c_data_allocator *arg_0)
{
	s_record_pool *local_0 = (s_record_pool *)arg_0->allocate(0x8886c);
	if (local_0)
	{
		function_16b5f0(local_0, "actor", 0x100, 0x888, 0, arg_0);
		((byte *)local_0)[0x2a] |= 4;
	}
	return local_0;
}

#pragma inline_depth(0)
PRIVATE __forceinline hash_table *function_1dfae2()
{
	return function_13e1a0("actor firing-position owners", 4, 0x400,
		(dword (__stdcall *)(void const *))function_25dd20,
		(bool (__stdcall *)(void const *, void const *))function_25dd30, 0x100, g_510c2c);
}
#pragma inline_depth(255)

// @retail 0x1dfae0
void function_1dfae0(void)
{
	g_4f55f0 = function_1dfae1(g_510c2c);
	g_4f93a0 = (s_output_entry *)function_123d40(NULL, NULL, 0x640);
	g_557c6c = (s_game_proc_table_557c6c *)function_1dfae2();
}
void function_28d930(void);
void function_25c170(void);
void function_200930(void);
void function_257d00(void);
void function_20b930(void);
void function_292130(void);
void function_1a6d80(void);
void function_28d9d0(void);
void function_292e00(void);
void function_292f60(void);

// @retail 0x1c7790
void function_1c7790(void)
{
	g_4f55d0 = (s_ai_globals *)function_123d40("ai globals", NULL, sizeof(s_ai_globals));
	g_4f55cc = (s_ai_player *)function_123d40("ai players", NULL, MAXIMUM_AI_PLAYERS * sizeof(s_ai_player));
	ai_globals_clear();
	function_1dfae0();
	function_28d930();
	function_25c170();
	function_200930();
	g_5044cc = function_123d40("ai 5044cc", NULL, 0x20);
	g_502420 = data_new_inlined("clump", 20, 0x50, 0, g_510c2c);
	g_502424 = data_new_inlined("joint state", 20, 0xbc, 0, g_510c2c);
	g_4f5768 = NONE;
	g_51eca4 = data_new_inlined("dynamic firing points", 15, 0x484, 0, g_510c2c);
	function_257d00();
	function_20b930();
	g_5044d0 = function_123d40("ai 5044d0", NULL, 0x10);
	function_292130();
	function_1a6d80();
	g_51ecb4 = data_new_inlined("flocks", 10, 0x28, 0, g_510c2c);
	g_5047f4 = function_123d40("ai 5047f4", NULL, 0x24);
}

// @retail 0x1c7b20
void ai_dispose_from_old_map(void)
{
	if (g_4f55d0->active)
	{
		g_51e9d8->valid = false;
		g_51e9dc->valid = false;
		g_502420->valid = false;
		g_502424->valid = false;
		g_502408->valid = false;
		g_502404->valid = false;
		g_51eca4->valid = false;
		g_50241c->valid = false;
		g_502418->valid = false;
		g_502414->valid = false;
		g_4f55f0->valid = false;
		function_28d9d0();
		g_4f9398->valid = false;
		function_292e00();
		g_4f55d0->active = false;
	}
}

/* a player as 0x1c7e70 reads it */
struct s_ai_player_datum
{
	byte unknown000[0x2c];
	long unit_index;
	byte unknown030[0x21c - 0x30];
};

/* the actor field 0x1c7e70 compares */
struct s_ai_rider_actor
{
	byte unknown000[0x3e];
	short unknown03e;
};

long function_baf80(long object_index);
bool function_e68c0(long type, long unit_index);

/* when the player's unit rides a biped or vehicle, sends request 0x1e to
   each rider whose actor's value at +0x3e differs */
// @retail 0x1c7e70
void function_1c7e70(long player_index, short value)
{
	s_ai_player_datum *player = &((s_ai_player_datum *)g_4e8c24->data)[player_index & 0xffff];

	if (player->unit_index != NONE && object_get(player->unit_index)->unknown1fc != NONE)
	{
		long parent_index = function_baf80(player->unit_index);

		if (parent_index != NONE && ((1 << object_get(parent_index)->type) & 3))
		{
			s_object_child_iterator iterator;

			function_d0620(parent_index, &iterator);
			while (function_d0690(&iterator))
			{
				long actor_index = object_get(iterator.child_index)->actor_index;

				if (actor_index != NONE && ((s_ai_rider_actor *)actor_get(actor_index))->unknown03e != value)
				{
					function_e68c0(0x1e, iterator.child_index);
				}
			}
		}
	}
}

// @retail 0x1c7f60
void function_1c7f60(void)
{
	if (g_4f55d0->active)
	{
		function_292f60();
	}
}
/* two scratch buffers the ai borrows (0x22974 bytes each, carved from
   g_510c44); while one is out, g_510c48 is set, and the physics work list
   (0x146de0/0x146b80) is paused if it was running */
struct s_ai_scratch_buffer
{
	bool used;
	byte unknown01[3];
	byte *address;
	long size;
};

#define AI_SCRATCH_BUFFER_COUNT 2
#define AI_SCRATCH_BUFFER_SIZE 0x22974

s_ai_scratch_buffer g_4f55b4[AI_SCRATCH_BUFFER_COUNT];
bool g_51e9b4;
long g_51e9b0;
byte *g_510c44;
bool g_510c48;

void function_146de0(void);
void function_146b80(void);

inline bool ai_physics_work_list_running(void)
{
	return g_47989c != NULL;
}

// @retail 0x1caae0
byte *ai_scratch_buffer_get(void)
{
	byte *result = NULL;
	long i;

	if (!g_51e9b4)
	{
		byte *address;

		g_51e9b0 = ai_physics_work_list_running();
		if (g_51e9b0)
		{
			function_146de0();
		}
		address = g_510c44;
		g_510c48 = true;
		for (i = 0; i < AI_SCRATCH_BUFFER_COUNT; i++)
		{
			g_4f55b4[i].used = false;
			g_4f55b4[i].size = AI_SCRATCH_BUFFER_SIZE;
			g_4f55b4[i].address = address;
			address += g_4f55b4[i].size;
		}
		g_51e9b4 = true;
	}
	for (i = 0; i < AI_SCRATCH_BUFFER_COUNT; i++)
	{
		if (!g_4f55b4[i].used)
		{
			g_4f55b4[i].used = true;
			result = g_4f55b4[i].address;
			break;
		}
	}
	return result;
}

// @retail 0x1cab80
void ai_scratch_buffer_release(byte *address)
{
	long used_count = 0;
	long i;

	for (i = 0; i < AI_SCRATCH_BUFFER_COUNT; i++)
	{
		if (g_4f55b4[i].used)
		{
			if (g_4f55b4[i].address == address)
			{
				g_4f55b4[i].used = false;
			}
			else
			{
				used_count++;
			}
		}
	}
	if (!used_count)
	{
		g_51e9b4 = false;
		g_510c48 = false;
		if (g_51e9b0)
		{
			function_146b80();
		}
	}
}

/* the ai's list of actors and squads by importance (0x1c8700) */
struct s_ai_importance_entry
{
	byte kind;
	long index;
	long importance;
};

#define MAXIMUM_AI_IMPORTANCE_ENTRIES 0x100

struct s_ai_importance_list
{
	short count;
	short unknown2;
	s_ai_importance_entry entries[MAXIMUM_AI_IMPORTANCE_ENTRIES];
};

/* the actor (g_4f55f0) and the squad (g_51e9d8) as the list reads them */
struct s_ai_importance_actor
{
	byte unknown00[9];
	bool unknown09;
	byte unknown0a[0x10 - 0xa];
	long importance;
	byte unknown14[0x20 - 0x14];
	long next_index;
};

struct s_ai_importance_squad
{
	byte unknown00[2];
	byte flags;
	byte unknown03[0xa - 0x3];
	short unknown0a;
	byte unknown0c[0x78 - 0xc];
	long importance;
};

typedef bool (__stdcall *t_sort_compare_function)(void const *a, void const *b, void const *context);
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_sort_compare_function compare, void const *context);

// @retail 0x1c86d0
bool __stdcall ai_importance_compare(void const *a, void const *b, void const *context)
{
	s_ai_importance_entry const *entry_a = (s_ai_importance_entry const *)a;
	s_ai_importance_entry const *entry_b = (s_ai_importance_entry const *)b;
	long importance_a = entry_a->importance;
	long importance_b = entry_b->importance;
	bool result;

	if (importance_b < importance_a)
	{
		return true;
	}
	if (importance_b > importance_a)
	{
		return false;
	}
	result = entry_a->kind < entry_b->kind;
	return result;
}

// @retail 0x1c8700
void __stdcall ai_importance_list_build(long unused, s_ai_importance_list *list, long unused2)
{
	long actor_index;
	s_record_pool_iterator iterator;

	list->count = 0;
	list->unknown2 = 0;
	if (g_4f55d0->active)
	{
		actor_index = g_4f55d0->unknown14;
	}
	while (g_4f55d0->active && actor_index != NONE)
	{
		s_ai_importance_actor *actor = (s_ai_importance_actor *)(g_4f55f0->data + (actor_index & 0xffff) * 0x888);
		long index = actor_index;

		if (list->count >= MAXIMUM_AI_IMPORTANCE_ENTRIES)
		{
			break;
		}
		actor_index = actor->next_index;
		if (!actor->unknown09 && actor->importance != NONE)
		{
			list->entries[list->count].kind = 1;
			list->entries[list->count].index = index;
			list->entries[list->count].importance = actor->importance;
			list->count++;
		}
	}
	if (g_4f55d0->active)
	{
		iterator.data = g_51e9d8;
		iterator.index = NONE;
	}
	while (g_4f55d0->active)
	{
		s_ai_importance_squad *squad = (s_ai_importance_squad *)data_iterator_next_inlined(&iterator);

		if (!squad || list->count >= MAXIMUM_AI_IMPORTANCE_ENTRIES)
		{
			break;
		}
		if (!(squad->flags & 0x80) && squad->unknown0a > 0 && squad->importance != NONE)
		{
			list->entries[list->count].kind = 0;
			list->entries[list->count].index = iterator.datum_index;
			list->entries[list->count].importance = squad->importance;
			list->count++;
		}
	}
	if (list->count > 0)
	{
		function_13da70(list->entries, list->count, sizeof(s_ai_importance_entry), ai_importance_compare, NULL);
	}
}

/* the object as 0x1caa40 reads it */
struct s_ai_position_object
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x30 - 0x18];
	point3f center;
	byte unknown3c[0xaa - 0x3c];
	char type;
	byte unknownab[0xb4 - 0xab];
	long havok_component_index;
};

point3f *function_b9ef0(long object_index, point3f *position);

/* where the ai looks at an object: a unit's or vehicle's head marker, the
   center of mass of a free physics object, otherwise its center */
// @retail 0x1caa40
void function_1caa40(long object_index, point3f *position)
{
	s_ai_position_object *object = (s_ai_position_object *)((s_ai_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	long type_mask = 1 << object->type;

	if ((type_mask & 3) || (type_mask & 0x1000))
	{
		s_object_marker marker;

		function_b8d30(object_index, 0x40000bd, &marker, 1, false);
		*position = marker.matrix.position;
	}
	else if (object->parent_index == NONE && object->havok_component_index != NONE)
	{
		function_b9ef0(object_index, position);
	}
	else
	{
		*position = object->center;
	}
}

/* the props (g_50241c, 0xc4 bytes) as 0x1ca2d0 reads them */
struct s_ai_prop_target
{
	byte unknown00[8];
	long object_index;
	byte unknown0c[0x23 - 0xc];
	bool unknown23;
	byte unknown24;
	bool unknown25;
	byte unknown26[0xc4 - 0x26];
};

/* the actor (0x888 bytes) as 0x1ca2d0 reads it */
struct s_ai_conversation_actor
{
	byte unknown000[7];
	bool unknown007;
	byte unknown008;
	bool unknown009;
	byte unknown00a[0x18 - 0xa];
	long unit_index;
	byte unknown01c[0x238 - 0x1c];
	point3f position;
	byte unknown244[0x338 - 0x244];
	long prop_index;
	byte unknown33c[0x6fe - 0x33c];
	short unknown6fe;
	byte unknown700[0x888 - 0x700];
};

struct s_ai_conversation_object
{
	long definition_index;
	byte unknown004[0x13c - 0x4];
	long player_index;
};

struct s_ai_conversation_unit_definition
{
	byte unknown00[0xbc];
	dword unknownbc_0 : 19;
	dword unknownbc_19 : 1;
	dword unknownbc_20 : 12;
};

/* the prop view (g_502414 + 0x70) as 0x1ca2d0 reads it */
struct s_ai_conversation_view
{
	byte unknown00[0x10];
	long time;
};

/* what 0x1e5280 returns for an actor and a weapon definition */
struct s_ai_weapon_properties
{
	byte flags;
	byte unknown01[0xc - 0x1];
	real range;
};

struct s_prop_node;
struct s_type_f95cd3;
s_type_f95cd3 *function_25d740(s_prop_node *node);
long function_1e1f20(long actor_index); /* unknown_1e1f20.cpp */
point3f *function_b9dd0(long object_index, point3f *result);

/* game ticks in the given number of seconds, rounded */
static inline long ai_seconds_to_ticks(real seconds)
{
	real ticks = g_510c54->field_2_3 * seconds;
	long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}
void *function_1e5280(long actor_index, long key); /* unknown_1e5240.cpp */

static inline s_ai_conversation_object *ai_conversation_object_get(long object_index)
{
	return (s_ai_conversation_object *)((s_ai_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

long function_1e4990(long index);
void __stdcall function_290250(long tag_index, long ticks, long object_index, long node_index, real lower, real upper,
	transform4x3f const *matrix);

/* passes an effect's request on to the ai when the ai is running and the
   tag's first flag is set */
// @retail 0x1ca290
void function_1ca290(long tag_index, long ticks, long object_index, long node_index, real lower, real upper, transform4x3f const *matrix)
{
	if (g_4f55d0->active && (*(byte *)function_1e4990(tag_index) & 1))
	{
		function_290250(tag_index, ticks, object_index, node_index, lower, upper, matrix);
	}
}

// @retail 0x1ca2d0
long function_1ca2d0(bool unknown)
{
	long game_time = g_510c54->game_time;
	long node_index = NONE;
	long index = NONE;

	while ((index = data_next_absolute_index_inlined(g_502418, index + 1)) != NONE)
	{
		s_prop_node_view *node = (s_prop_node_view *)(g_502418->data + g_502418->size * index);
		s_ai_prop_target *target;
		long actor_index;
		s_ai_conversation_actor *actor;
		bool ignore;
		s_ai_conversation_view *view;
		point3f position;
		real distance;
		real range;

		node_index = (*(short *)node << 16) | index;
		target = &((s_ai_prop_target *)g_50241c->data)[node->unknown08 & 0xffff];
		if (!target->unknown25 || !target->unknown23 ||
			ai_conversation_object_get(target->object_index)->player_index == NONE)
		{
			continue;
		}
		actor_index = node->unknown04;
		actor = &((s_ai_conversation_actor *)g_4f55f0->data)[actor_index & 0xffff];
		ignore = false;
		if (!actor->unknown009)
		{
			ignore = true;
		}
		if (!actor->unknown007 &&
			TEST_FIELD_BIT(((s_ai_conversation_unit_definition *)g_4e3b44[ai_conversation_object_get(actor->unit_index)->definition_index & 0xffff].bytes)->unknownbc_19) &&
			node->unknown28 > 4.0f)
		{
			ignore = true;
		}
		if (unknown && node->unknown28 > 15.0f && (actor->unknown6fe == 0 || actor->prop_index != node_index))
		{
			continue;
		}
		if (ignore)
		{
			continue;
		}
		view = (s_ai_conversation_view *)function_25d740((s_prop_node *)node);
		function_b9dd0(node->object_index, &position);
		distance = (real)sqrt((position.x - actor->position.x) * (position.x - actor->position.x) +
			(position.y - actor->position.y) * (position.y - actor->position.y) +
			(position.z - actor->position.z) * (position.z - actor->position.z));
		if (view && !(node->unknown24 >= 1 && node->unknown24 <= 2) && view->time != NONE &&
			game_time - view->time < ai_seconds_to_ticks(3.0f) && 12.0f > distance)
		{
			return actor_index;
		}
		if (4.0f > distance)
		{
			return actor_index;
		}
		if (actor->prop_index != node_index)
		{
			continue;
		}
		range = 15.0f;
		{
			long weapon_index = function_1e1f20(actor_index);

			if (weapon_index != NONE)
			{
				s_ai_weapon_properties *properties = (s_ai_weapon_properties *)function_1e5280(actor_index, ai_conversation_object_get(weapon_index)->definition_index);

				if (properties && (properties->flags & 4))
				{
					real weapon_range = properties->range + 5.0f;

					if (!(15.0f > weapon_range))
					{
						range = weapon_range;
					}
				}
			}
		}
		if (node->unknown24 >= 1 && node->unknown24 <= 2 && range > distance)
		{
			return actor_index;
		}
		if (node->unknown24 >= 3)
		{
			view = (s_ai_conversation_view *)function_25d740((s_prop_node *)node);
			if (view && 12.0f > distance && game_time - view->time < ai_seconds_to_ticks(5.0f))
			{
				return actor_index;
			}
		}
	}
	return NONE;
}

// @retail 0x1ca630
bool function_1ca630(long *unit_index)
{
	long actor_index = function_1ca2d0(false);

	if (actor_index != NONE)
	{
		*unit_index = ((s_ai_conversation_actor *)g_4f55f0->data)[actor_index & 0xffff].unit_index;
		return true;
	}
	return false;
}

// @retail 0x1ca670
bool function_1ca670(void)
{
	return function_1ca2d0(true) != NONE;
}


void function_200240(long object_index, long target_index);

#pragma optimize("y", off)
// @retail 0x1ca9f0
void __stdcall function_1ca9f0(long object_index, long target_index)
{
	(void)&object_index;
	(void)&target_index;
	s_ai_globals *local_0 = g_4f55d0;
	long local_1 = *(volatile byte const *)&local_0->active;
	if (local_1)
		function_200240(object_index, target_index);
}
#pragma optimize("y", on)


void hash_table_initialize(hash_table *table);
void function_200b30(void);
void function_20b9b0(void);
void function_295970(void);
extern s_record_pool *g_5044c8;
extern long g_502428;
extern long g_50242c;

PRIVATE inline void ai_pool_reset(s_record_pool *pool)
{
	pool->valid = true;
	record_pool_release_all(pool);
}

// @retail 0x1c79e0
void function_1c79e0(void)
{
	if (g_4e6948->state == 2)
		g_4f55d0->active = false;
	else
	{
		ai_globals_initialize_for_new_map();
		ai_pool_reset(g_4f55f0);
		hash_table_initialize((hash_table *)g_557c6c);
		ai_pool_reset(g_5044c8);
		ai_pool_reset(g_50241c);
		ai_pool_reset(g_502418);
		ai_pool_reset(g_502414);
		function_200b30();
		memset(g_5044cc, 0, 0x20);
		ai_pool_reset(g_502420);
		ai_pool_reset(g_502424);
		ai_pool_reset(g_51eca4);
		ai_pool_reset(g_502408);
		ai_pool_reset(g_502404);
		g_502428 = NONE;
		g_50242c = NONE;
		function_20b9b0();
		memset(g_5044d0, 0, 0x10);
		ai_pool_reset(g_51ecb4);
		function_295970();
		*(short *)((byte *)g_4f55d0 + 0x3e) = 0;
		*(short *)((byte *)g_4f55d0 + 0x3c) = 0;
		memset((byte *)g_4f55d0 + 0x40, 0, 0x300);
		g_4f55d0->active = true;
	}
}


long function_25d810(long object_index, long actor_index, bool create);

// @retail 0x1c89b0
short __stdcall function_1c89b0(long actor_index, short maximum_count, s_ai_capsule_view *capsules)
{
 (void)&actor_index;
 (void)&maximum_count;
 (void)&capsules;
 s_actor_view *actor = actor_get(actor_index);
 long count = 0;
 if (actor->unknown07c != NONE)
 {
  long next = element_502420_get(actor->unknown07c)->first_actor_index;
  while (next != NONE)
  {
   long index = next;
   s_actor_view *other = actor_get(index);
   next = other->next_index;
   if (index != actor_index && (short)count < maximum_count && other->unknown018 != NONE && other->unknown26c == NONE)
   {
    long object_index = other->unknown018;
    if (((s_ai_object_header *)g_4e0300->data)[object_index & 0xffff].type == 0)
     function_1c8940(&capsules[(short)count++], object_index, NONE);
   }
  }
 }
 long next = actor_get(actor_index)->first_prop_index;
 while (next != NONE)
 {
  long index = next;
  s_prop_node_view *node = &((s_prop_node_view *)g_502418->data)[index & 0xffff];
  next = node->next_index;
  s_prop_state_view *state = prop_node_state(node);
  s_ai_prop_target *target = &((s_ai_prop_target *)g_50241c->data)[node->unknown08 & 0xffff];
  if (!target->unknown23 && !*((bool *)state + 0x5e) && node->unknown24 == 1 && state->unknown3c == NONE)
  {
   long object_index = node->object_index;
   if (((1 << object_get(object_index)->type) & 1) && (short)count < maximum_count)
    function_1c8940(&capsules[(short)count++], object_index, index);
  }
 }
 if (!team_is_enemy(actor->unknown024, 1))
 {
  s_record_pool_iterator iterator;
  iterator.data = g_4e8c24;
  iterator.index = NONE;
  s_ai_player_datum *player;
  while ((player = (s_ai_player_datum *)data_iterator_next_inlined(&iterator)) != NULL)
  {
   long object_index = player->unit_index;
   if (object_index == NONE)
    continue;
   s_ai_object_header *headers = (s_ai_object_header *)g_4e0300->data;
   if (headers[object_index & 0xffff].type != 0)
    continue;
   long parent = object_get(object_index)->parent_index;
   if (parent != NONE)
   {
    long root = function_baf40(parent);
    if (headers[root & 0xffff].type == 1)
     parent = root;
    if (parent == actor->unknown26c)
     continue;
   }
   long prop_index = function_25d810(object_index, actor_index, false);
   object_index = player->unit_index;
   s_ai_capsule_view *capsule = &capsules[(short)count];
   real height;
   real radius;
   function_df5f0(object_index, &capsule->center, &height, &radius);
   capsule->active = height == 0.0f ? 1 : 0;
   capsule->value10 = 0.0f;
   capsule->value14 = 0.0f;
   capsule->height = height;
   capsule->radius = radius + 0.15f;
   capsule->value1c = prop_index;
   capsule->object_index = object_index;
   ++count;
  }
 }
 return (short)count;
}

struct s_actor_creation_definition
{
    dword flags;
    short type;
};

struct s_actor_creation_tag
{
    byte unknown00[8];
    long parent_index;
    byte unknown0c[0x34 - 0xc];
    long count;
    dword *flags;
};

bool function_1a80e0(long index, short type, s_slot *data, short slot);

// @retail 0x1dfb90
long __stdcall function_1dfb90(long definition_index)
{
    long const *definition_reference = &definition_index;
    long result = NONE;
    if (*definition_reference != NONE)
    {
        s_record_pool *pool = g_4f55f0;
        result = record_pool_allocate(pool);
        if (result != NONE)
        {
            byte *actor = pool->data + (result & 0xffff) * 0x888;
            s_actor_creation_definition *definition = (s_actor_creation_definition *)function_1e4990(*definition_reference);
            *(long *)(actor + 0x54) = *definition_reference;
            *(bool *)(actor + 7) = (bool)(definition->flags & 1);
            *(short *)(actor + 4) = definition->type;
            *(long *)(actor + 0x18) = NONE;
            *(long *)(actor + 0x1c) = NONE;
            *(long *)(actor + 0x30) = NONE;
            *(long *)(actor + 0x28) = NONE;
            *(long *)(actor + 0x34) = NONE;
            *(short *)(actor + 0x2c) = NONE;
            *(byte *)(actor + 0x8) = 1;
            *(long *)(actor + 0x10) = NONE;
            *(long *)(actor + 0x14) = NONE;
            *(byte *)(actor + 0xa) = 0;
            *(byte *)(actor + 0x3c) = 0;
            *(byte *)(actor + 0x9) = 0;
            *(byte *)(actor + 0xc) = 0;
            *(long *)(actor + 0x4c) = g_510c54->game_time;
            *(short *)(actor + 0x42) = 0;
            *(short *)(actor + 0x46) = 0;
            *(long *)(actor + 0x58) = NONE;
            for (long i = 0; i < 8; ++i)
                ((long *)(actor + 0x5c))[i] = NONE;
            volatile short local_1 = g_4686c4;
            *(short *)(actor + 0x3e) = local_1;
            *(long *)(actor + 0x7c) = NONE;
            *(long *)(actor + 0x32c) = NONE;
            *(s_reference *)(actor + 0x418) = g_470fa0;
            *(byte *)(actor + 0x3f2) = 0;
            *(long *)(actor + 0x3f4) = NONE;
            *(long *)(actor + 0x3f8) = NONE;
            *(short *)(actor + 0x3fc) = NONE;
            s_actor_view *actor_view = (s_actor_view *)(pool->data + (result & 0xffff) * 0x888);
            actor_view->unknown3fe = 3;
            long local_2 = 4;
            s_reference *local_3 = &actor_view->unknown400[0].reference;
            do
            {
                *local_3 = g_470fa0;
                local_3 = (s_reference *)((byte *)local_3 + sizeof(actor_view->unknown400[0]));
            } while (--local_2);
            *(short *)(actor + 0x84) = 3;
            *(short *)(actor + 0x86) = 1;
            *(long *)(actor + 0x88) = 0;
            *(byte *)(actor + 0x220) = 0;
            *(byte *)(actor + 0x221) = 0;
            *(bool *)(actor + 0x229) = (bool)((definition->flags >> 1) & 1);
            *(byte *)(actor + 0x227) = 0;
            *(byte *)(actor + 0x228) = 0;
            *(byte *)(actor + 0x222) = 0;
            *(byte *)(actor + 0x22a) = 0;
            *(byte *)(actor + 0x223) = 0;
            *(byte *)(actor + 0x225) = 0;
            *(byte *)(actor + 0x226) = 0;
            *(byte *)(actor + 0x224) = 0;
            memset(actor + 0x90, 0, 0x100);
            long slot_index = 0;
            do
            {
                *(short *)(actor + 0x90 + slot_index * 0x40) = NONE;
                *(long *)(actor + 0x98 + slot_index * 0x40) = NONE;
                ++slot_index;
            } while (slot_index < 4);
            *(short *)(actor + 0x190) = NONE;
            if (!*(bool *)(actor + 7))
                function_1a80e0(result, 1, NULL, 0);
            else
                function_1a80e0(result, 0x72, NULL, 0);
            for (long i = 0; i < 14; ++i)
                ((long *)(actor + 0x1e8))[i] = NONE;
            long memory_index = 0;
            do
            {
                *(short *)(actor + 0x1c4 + memory_index * 0xc) = NONE;
                ++memory_index;
            } while (memory_index < 3);
            memory_index = 0;
            do
            {
                *(short *)(actor + 0x194 + memory_index * 0xc) = NONE;
                ++memory_index;
            } while (memory_index < 4);
            *(long *)(actor + 0x858) = NONE;
            *(long *)(actor + 0x85c) = NONE;
            *(long *)(actor + 0x28c) = NONE;
            *(long *)(actor + 0x26c) = NONE;
            *(long *)(actor + 0x274) = NONE;
            *(byte *)(actor + 0x2e0) = 0;
            *(byte *)(actor + 0x266) = 0;
            *(byte *)(actor + 0x268) = 0;
            *(byte *)(actor + 0x267) = 0;
            *(short *)(actor + 0x270) = 0;
            memset(actor + 0x3d4, 0, 0x1c);
            *(long *)(actor + 0x3e8) = NONE;
            *(byte *)(actor + 0x3e4) = 0;
            actor_view = (s_actor_view *)(g_4f55f0->data + (result & 0xffff) * 0x888);
            actor_view->unknown50c = false;
            actor_view->unknown5ac = NONE;
            actor_view->unknown5b0 = NONE;
            actor_view->unknown5b4 = 0;
            actor_view->unknown5b6 = 0;
            actor_view->unknown4ac = 0;
            actor_view->unknown504 = 0;
            *(short *)(actor + 0x6fe) = 1;
            *(byte *)(actor + 0x714) = 1;
            *(byte *)(actor + 0x5d0) = 0;
            *(byte *)(actor + 0x5d1) = 0;
            *(long *)(actor + 0x634) = NONE;
            *(short *)(actor + 0x6fc) = 0;
            *(short *)(actor + 0x700) = 0;
            *(long *)(actor + 0x718) = NONE;
            *(real *)(actor + 0x71c) = 0.0f;
            *(short *)(actor + 0x720) = 0;
            *(short *)(actor + 0x702) = 0;
            *(short *)(actor + 0x704) = 0;
            *(short *)(actor + 0x706) = 0;
            *(long *)(actor + 0x734) = 0;
            *(long *)(actor + 0x724) = NONE;
            *(short *)(actor + 0x7c4) = 0;
            *(long *)(actor + 0x7c8) = NONE;
            *(long *)(actor + 0x7e0) = NONE;
            *(byte *)(actor + 0x7cc) = 0;
            *(real *)(actor + 0x7ac) = 0.0f;
            *(real *)(actor + 0x710) = 1.0f;
            *(byte *)(actor + 0x6d0) = 0;
            *(short *)(actor + 0x686) = 0;
            *(byte *)(actor + 0x6c0) = 0;
            *(long *)(actor + 0x6c4) = NONE;
            *(vector3f *)(actor + 0x6e0) = *g_4687a8;
            *(vector3f *)(actor + 0x6d4) = *g_4687a8;
            *(vector3f *)(actor + 0x6ec) = *g_4687a8;
            *(byte *)(actor + 0x6f8) = 1;
            *(short *)(actor + 0x6fa) = 0;
            *(byte *)(actor + 0x605) = 0;
            *(short *)(actor + 0x61e) = 0;
            *(long *)(actor + 0x6c8) = 0;
            *(byte *)(actor + 0x6cc) = 0;
            memset(actor + 0x654, 0, 0x28);
            *(byte *)(actor + 0x5d8) = 0;
            *(short *)(actor + 0x5da) = 0;
            *(point3f *)(actor + 0x5dc) = *g_468788;
            *(long *)(actor + 0x7f8) = NONE;
            *(short *)(actor + 0x5e8) = 0;
            *(byte *)(actor + 0x604) = 0;
            *(long *)(actor + 0x338) = NONE;
            *(long *)(actor + 0x33c) = 0;
            *(byte *)(actor + 0x340) = 0;
            *(long *)(actor + 0x350) = NONE;
            *(long *)(actor + 0x344) = NONE;
            *(long *)(actor + 0x348) = NONE;
            *(byte *)(actor + 0x354) = 0;
            *(long *)(actor + 0x2e8) = NONE;
            *(short *)(actor + 0x2ec) = NONE;
            *(short *)(actor + 0x2ee) = NONE;
            *(byte *)(actor + 0x2f0) = 0;
            *(short *)(actor + 0x2f4) = g_510c54->field_2_3 * 10;
            *(short *)(actor + 0x2f2) = NONE;
            *(long *)(actor + 0x2f8) = 0;
            *(long *)(actor + 0x2fc) = NONE;
            *(long *)(actor + 0x300) = NONE;
            *(short *)(actor + 0x304) = 0;
            *(short *)(actor + 0x306) = 0;
            *(long *)(actor + 0x308) = NONE;
            *(byte *)(actor + 0x30c) = 0;
            *(short *)(actor + 0x30e) = 0;
            *(short *)(actor + 0x310) = NONE;
            *(real *)(actor + 0x318) = 0.0f;
            *(short *)(actor + 0x31c) = NONE;
            *(short *)(actor + 0x31e) = 0;
            *(short *)(actor + 0x320) = 0;
            long tag_index = actor_get(result)->unknown054;
            while (tag_index != NONE)
            {
                s_actor_creation_tag *tag = (s_actor_creation_tag *)g_4e3b44[tag_index & 0xffff].bytes;
                if (tag->count > 0)
                {
                    if (tag->flags)
                    {
                        byte *local_0 = (byte *)&definition_index;
                        *(volatile byte *)local_0 = (byte)((*tag->flags >> 3) & 1);
                        if (*local_0)
                            ++*(short *)((byte *)g_5047f4 + 6);
                    }
                    break;
                }
                tag_index = tag->parent_index;
            }
        }
    }
    return result;
}


bool function_11e5e0(point3f const *origin, point3f const *center, vector3f const *direction, real radius);
long function_11ea30(point3f const *a, vector3f const *u, point3f const *b, vector3f const *v, real radius);

// @retail 0x1c8d20
bool function_1c8d20(long actor_index, point3f const *origin, long excluded_object,
    vector3f const *direction, long *blocking_index)
{
    bool result = true;
    long blocker = NONE;
    s_ai_capsule_view capsules[32];
    short count = function_1c89b0(actor_index, 32, capsules);
    for (short i = 0; i < count; i++)
    {
        if (capsules[i].object_index == excluded_object)
            continue;
        char intersects;
        if (capsules[i].active)
            intersects = function_11e5e0(&capsules[i].center, origin, direction, capsules[i].radius);
        else
            intersects = (char)function_11ea30(&capsules[i].center,
                (vector3f *)&capsules[i].value10, origin, direction, capsules[i].radius);
        if (intersects)
        {
            blocker = capsules[i].value1c;
            result = false;
            break;
        }
    }
    if (blocking_index)
        *blocking_index = blocker;
    return result;
}


bool function_1df5d0(short team_a, short team_b);
real __stdcall function_265d30(long actor_index, long prop_index);

// @retail 0x1c9a00
void function_1c9a00(void)
{
    s_actor_iterator iterator;
    function_x66da2b(&iterator, true);
    s_actor_view *actor;
    while ((actor = (s_actor_view *)function_1e46c0(&iterator)) != NULL)
    {
        long next = actor_get(iterator.actor_index)->first_prop_index;
        while (next != NONE)
        {
            long index = next;
            s_prop_node_view *node = &((s_prop_node_view *)g_502418->data)[index & 0xffff];
            s_ai_prop_target *target = &((s_ai_prop_target *)g_50241c->data)[node->unknown08 & 0xffff];
            byte *object = (byte *)((s_ai_object_header *)g_4e0300->data)[node->object_index & 0xffff].object;
            next = node->next_index;
            byte *state = NULL;
            if (node->view_index != NONE)
            {
                byte *view = g_502414->data + (node->view_index & 0xffff) * 0x124;
                if (view)
                    state = view + 0x70;
            }
            *(short *)((byte *)target + 0x20) = *(short *)(object + 0x138);
            target->unknown23 = function_1df560(actor->unknown024, *(short *)((byte *)target + 0x20));
            target->unknown24 = function_1df5d0(actor->unknown024, *(short *)((byte *)target + 0x20));
            if (state)
                *(real *)(state + 0x3c) = function_265d30(iterator.actor_index, index);
        }
    }
}
