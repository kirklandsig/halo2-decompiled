// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A58D0.CPP: the simulation entity database: an entity's object */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "unknown_xa19f52.h"
#include <string.h>

// The serializer and initializer now use the complete stream and state-output
// interfaces at their definitions, declarations, and callers.
#if 0
void scenario_object_name_encode(long object_name, s_bitstream *stream);
__declspec(noinline) void *function_122c10(long group_tag, long tag_index);
void function_11df60(vector3f const *rotation, vector3f *forward, vector3f *up);

#endif
void scenario_object_name_encode(long object_name, s_bitstream *stream);
// @retail 0xa6660
void __stdcall function_a6660(s_entity_info const *info, s_bitstream *stream)
{
	scenario_object_name_encode(info->definition_index, stream);
	stream_write_bit(stream, info->field0 != NONE);
	if (info->field0 != NONE)
		stream_write_checked(stream, info->field0, 13);
	stream_write_checked(stream, (signed char)info->byte8 + 1, 7);
	byte const *fields = (byte const *)info;
	if (!fields[0xc] && !fields[0xd] && !fields[0xe])
    {
        stream_write_bit(stream, false);
        return;
    }
    stream_write_bit(stream, true);
    {
		stream_write_checked(stream, fields[0xc], 6);
		stream_write_checked(stream, fields[0xd], 6);
		stream_write_checked(stream, fields[0xe], 4);
	}
}
#if 0


#endif
__declspec(noinline) void *function_122c10(long group_tag, long tag_index);
void function_11df60(vector3f const *rotation, vector3f *forward, vector3f *up);
struct s_z_initial_placement_type
{
	byte unknown00[0xa];
	short placements_offset;
	short palette_offset;
	short placement_size;
};

struct s_z_initial_placement_block
{
	long count;
	byte *data;
};

struct s_z_initial_placement
{
	byte unknown00[8];
	point3f position;
	vector3f rotation;
	real scale;
};

struct s_z_initial_state
{
	point3f position;
	vector3f forward;
	vector3f up;
	real scale;
	byte unknown28[0x40 - 0x28];
	real value40;
	byte flag44;
	byte unknown45[3];
	real value48;
	byte flag4c;
	byte unknown4d[3];
	byte value50;
	byte unknown51[0x61 - 0x51];
	byte value61;
};

// @retail 0xa5bd0
bool function_a5bd0(s_entity_info const *info, s_z_initial_state *state)
{
    bool result = false;
	if (function_122c10(0x6f626a65, info->definition_index))
	{
		byte *definition = g_4e3b44[info->definition_index & 0xffff].bytes;
		state->position = *g_468788;
		state->forward = *g_4687a8;
		state->up = *g_4687b0;
		state->scale = 1.0f;
		if (info->field0 != NONE)
		{
			s_z_initial_placement_type *type = (s_z_initial_placement_type *)g_468630[*(short *)definition];
			if (type->placements_offset != NONE && type->palette_offset != NONE)
			{
				long placement_size = ((s_z_initial_placement_type *)g_468630[*(short *)definition])->placement_size;
				s_z_initial_placement_block *block = (s_z_initial_placement_block *)((byte *)g_4e0350 + ((s_z_initial_placement_type *)g_468630[*(short *)definition])->placements_offset);
				long clamped = info->field0 < 0 ? 0 : (block->count - 1 < info->field0 ? block->count - 1 : info->field0);
				if (clamped == info->field0)
				{
					s_z_initial_placement *placement = (s_z_initial_placement *)(block->data + info->field0 * placement_size);
					state->position = placement->position;
					function_11df60(&placement->rotation, &state->forward, &state->up);
					state->scale = placement->scale > 0.0f ? placement->scale : 1.0f;
				}
			}
		}
		state->value50 = 1;
		state->value61 = 0;
		state->flag44 = true;
		state->flag4c = true;
		long model_index = *(long *)(definition + 0x38);
		if (model_index != NONE)
		{
			byte *model = g_4e3b44[model_index & 0xffff].bytes;
			if (*(long *)(model + 0x60) > 0)
			{
				byte *settings = *(byte **)(model + 0x64);
				if (*(real *)(settings + 0x28) > 0.0f)
					state->value40 = 1.0f;
				if (*(real *)(settings + 0x8c) > 0.0f)
					state->value48 = 1.0f;
				state->value61 = settings[0xe0];
			}
			if (*(long *)(model + 0x70) > 0)
				state->value50 = (byte)*(long *)(model + 0x70);
		}
		result = true;
	}
	return result;
}
#if 0

#endif

/* the entity an index stands for, or none when its salt is stale; retail
   has no copy of its own (LTCG inlines it everywhere), and inlining it from
   here keeps the null test after the identifier comparison that retail has */
s_simulation_entity *simulation_entity_try_get(s_simulation_entity_table *table, long entity_index)
{
	s_simulation_entity *result = 0;
	if (table->entities[entity_index & 0x3ff].identifier == entity_index)
		result = &table->entities[entity_index & 0x3ff];
	return result;
}

/* 0xa58d0, kept out of the build: retail takes the index in ecx (the
   __fastcall the stub in src/stubs/unknown_09a9f0.cpp has), our LTCG passes it
   in eax or edx (it depends on the body), which breaks the matched caller
   0xa3a20. With simulation_entity_try_get as it is now, the body is otherwise
   the same as retail's (the null test after the identifier comparison
   included). */
// Disabled: round 18 reactivation after new event callers still breaks a3a20 and other matched entity callers; EDX input versus retail ECX
// Disabled: round 35: five new fruitless trials, 15 differences; EDX input versus retail ECX loses eleven matched callers
#if 0
// Retail 0xa58d0
long function_a58d0(long entity_index)
{
	long object_index = NONE;
	if (entity_index != NONE)
	{
		s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
		s_simulation_entity *entity = simulation_entity_try_get(&world->database->table, entity_index);
		if (entity)
		{
			s_entity_definitions *definitions = (s_entity_definitions *)g_4cf784;
			if (definitions->definitions[entity->type]->v7((s_entity *)entity) && entity->object_index != NONE)
				object_index = entity->object_index;
		}
	}
	return object_index;
}
#endif


struct s_z_entity_flags
{
	byte flags;
	byte unknown01[7];
};

struct s_z_entity_flags_table
{
	byte unknown00[0x44];
	s_z_entity_flags entries[1];
};

struct s_z_entity_flags_database
{
	byte unknown0000[0x20a4];
	s_z_entity_flags_table *flags;
};

struct s_z_entity_flags_world
{
	byte unknown00[4];
	s_z_entity_flags_database *database;
};

// @retail 0xa5930
long function_a5930(long index)
{
	long result = NONE;
	if (index != NONE)
	{
		long identifier = ((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d4;
		if (identifier != NONE)
		{
			s_z_entity_flags_world *world = (s_z_entity_flags_world *)g_4cf77c;
			if (world->database->flags->entries[identifier & 0x3ff].flags & 4)
				result = identifier;
		}
	}
	return result;
}

// Event distribution retains this entity lookup as an out-of-line call.
__declspec(noinline) long function_a5980(long index);

// @retail 0xa5980
long function_a5980(long index)
{
	long result = NONE;
	if (index != NONE)
	{
		long identifier = ((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d4;
		if (identifier != NONE)
		{
			s_z_entity_flags_world *world = (s_z_entity_flags_world *)g_4cf77c;
			if (!(world->database->flags->entries[identifier & 0x3ff].flags & 4))
				result = identifier;
		}
	}
	return result;
}

// @retail 0xa7670
bool function_a7670(long index)
{
	bool result = false;
	if (g_4e6948->mode == 4)
		result = ((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d4 != NONE;
	return result;
}

void function_b58c0(long index, dword mask);

// @retail 0xa7a30
void function_a7a30(long index, dword mask)
{
	long identifier = ((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d4;
	if (identifier != NONE)
		function_b58c0(identifier, mask);
}


struct s_z_entity_record
{
	long identifier;
	short type;
	bool active;
	byte unknown07;
	long object_index;
	byte unknown0c[0xc];
	long size;
	s_entity_data *data;
};

struct s_z_state_database
{
	byte unknown0000[0x20ac];
	s_z_entity_record entities[0x400];
};

struct s_z_state_world
{
	byte unknown00[4];
	s_z_state_database *database;
	long state;
};

// score 11 (was 11): this round: the fast check reads 11 at 120/120 bytes again and no new form was
// tried; the difference is still the database pointer in ECX where retail keeps it in EDX and the
// active byte folded as `mov cl, [eax + edx + 0x20b2]`.
// score 11 (was 11): this round: reading the active byte from the array element with the database
// read early still folds the byte as retail does (`mov cl, [eax + edx + 0x20b2]`) and keeps the
// database in EDX, but the database load is hoisted above the object lookup and the record address
// comes out as `add eax, edx` with the 0x20ac folded into every field offset (19). Taking the record
// pointer before the active test does not change that (19). A named world pointer moves the world to
// ECX and the state to EDX (21). Still the database pointer in ECX where retail keeps it in EDX.
// score 11 (was 11): the entity pointer is materialised in EAX and the record is read through it;
// retail folds the address into the load itself.
// The fold is the scheduler's rewrite of `lea; mov cl, [eax+6]`; ours can't do it because the
// database pointer is in ecx (retail edx) and the active byte goes to cl. Tried: inline active
// test, record getter or dispatch helper 11; world local in the inner block 11; state through the
// re-read 11; database local 11; active read before the pointer (72 spellings) 14-21; renames 11;
// a 600 s permuter run, none below 11. Round 89: the world global read at each use (no local, no
// address cast) builds the same code; with one world local for both reads ours is retail with ecx and
// edx swapped (world and database in ecx), 18 in every spelling: state local, identifier declared
// first, object local, database and index locals, state-test helper, unsigned identifier, entity
// table types; database read from the global in the state block 11 (sunk past the test).
// Round 60: the records inside a nested table struct at 0x2098 11; the record by pointer addition
// 11; the active test and the dispatch joined in one condition 13; an object pointer local for the
// identifier read 11; the size and data read into named locals before the dispatch 35 (the callers'
// convention flips: 0xa9640, 0xa96d0, 0xa9770, 0xa98b0, 0xa9940 and 0xa9570 lose their match).
// @retail 0xa9820
bool function_a9820(long *block, long index)
{
	bool result = false;
	if (((s_z_state_world *)g_4cf77c)->state == 4 || ((s_z_state_world *)g_4cf77c)->state == 5)
	{
		long identifier = ((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d4;
		if (identifier != NONE)
		{
			s_z_entity_record *entity = &((s_z_state_world *)g_4cf77c)->database->entities[identifier & 0x3ff];
			if (entity->active)
			{
				long type = entity->type;
				s_entity_definitions *definitions = (s_entity_definitions *)g_4cf784;
				if (definitions->definitions[type]->v34(entity->size, entity->data, block))
					result = true;
			}
		}
	}
	return result;
}

// score 11 (was 11): this round: the database as a byte pointer with the active byte read from the
// array 14; the record address as an integer 11; a named world pointer 21; the database read
// before the state test 11; the database as a long address 14; the database local with the record
// read from a second global read 14; the state in a named local 11 (same code). Still the database
// pointer in ECX where retail keeps it in EDX.
// This round, again: the active byte read from the array element with the record taken inside the
// branch 14 (123 bytes, the address is materialised before the byte); the record pointer, the type
// and the definitions each split from their initialiser 11 (the permuter's two candidates); the
// world local used for the database only 11 (same code); the state local with a world local 11
// (same code); the identifier read before the state test 22; the world local used for both reads 18
// (the database is then updated in place, but in ECX with the world). A permuter run produced no
// variant below 11. Still the database pointer in ECX where retail keeps it in EDX.
// MiniMax G (10-31): the database pointer is still loaded into ECX where retail loads
// it into EDX, and the active byte is still read through the materialised record address instead
// of the array; a database local with the byte read from the array, the same with the record
// address taken inside the test, a world local, the record read without any pointer at all, the
// modulo and mask index forms and the database read before the state test all score 14 or worse.
// Earlier: the database pointer takes ECX where retail keeps it in EDX, and ours takes the
// record address before the active test and reads the byte through it (`lea eax, [eax + ecx + 0x20ac]`,
// `mov cl, byte ptr [eax + 6]`), while retail reads the byte from the array and takes the address after.
// Early definitions 31; direct dispatch 13; const record 11; record reference 11; separate index mask 11;
// short type 22; definitions first 13; definition local 13; function-scope database 11; function-scope type 11.
// Signed accepted-state range 12; state/all rejection guards 11/11; database reference/const
// receivers 11/11; complete payload inputs 11; native active byte with nested/rejection guards 11/11.
// This round: the head (world in EDX, state in ECX) only comes out that way when the world pointer is
// read from the global again, but then the database load keeps ECX; reading the database from the world
// pointer itself moves the world to ECX and the state to EDX. Only the early-database form puts the
// database in EDX, and it then folds the active byte as retail does (`mov cl, [eax + edx + 0x20b2]`),
// but it loads the database before the state check, which moves the world to EAX and the index to ECX,
// and the record address becomes `add eax, edx` with the array offset folded into every field.
// Shapes tried: world read once 18; database local 18; re-read of the global 11; state read from the
// global 11; array form of the active byte 14/21; const world 18; void world 18; named database 11/18.
// The permuter could not finish a run here: it builds the whole program for its baseline and the
// machine is busy with other lanes' builds. Round 89 (with 0xa9820): the world global read at each use
// builds the same code; one world local for both reads 18 in every spelling (see 0xa9820).
// Round 60 (with 0xa9820): the records inside a nested table struct at 0x2098 11.
// @retail 0xa99f0
void function_a99f0(long index, long *block)
{
	if (((s_z_state_world *)g_4cf77c)->state == 4 || ((s_z_state_world *)g_4cf77c)->state == 5)
	{
		long identifier = ((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d4;
		if (identifier != NONE)
		{
			s_z_entity_record *entity = &((s_z_state_world *)g_4cf77c)->database->entities[identifier & 0x3ff];
			if (entity->active)
			{
				long type = entity->type;
				s_entity_definitions *definitions = (s_entity_definitions *)g_4cf784;
				definitions->definitions[type]->v35(entity->size, entity->data, block);
			}
		}
	}
}


#include <string.h>

struct s_z_copy_state
{
	dword flags;
	vector3f position;
	vector3f forward;
	vector3f up;
};

PRIVATE inline void z_copy_state_clear(s_z_copy_state *state)
{
	state->flags = 0;
	memset(&state->position, 0, sizeof(*state) - sizeof(state->flags));
}

// @retail 0xa9640
void function_a9640(long index)
{
	s_object_view *object = ((s_object_header *)g_4e0300->data)[index & 0xffff].object;
	if (object->field_d8 & 1)
	{
		s_z_copy_state state;
		z_copy_state_clear(&state);
		function_a9820((long *)&state, index);
		object->field_d8 &= ~1;
		state.flags = 0;
		function_a99f0(index, (long *)&state);
	}
}

// @retail 0xa96d0
void function_a96d0(long index, vector3f const *position)
{
	s_z_copy_state state;
	z_copy_state_clear(&state);
	if (function_a9820((long *)&state, index))
	{
		byte *flags = &((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d8;
		*flags |= 1;
		state.flags |= 1;
		state.position = *position;
		function_a99f0(index, (long *)&state);
	}
}

// @retail 0xa9770
void function_a9770(long index, vector3f const *up, vector3f const *forward)
{
	s_z_copy_state state;
	z_copy_state_clear(&state);
	if (function_a9820((long *)&state, index))
	{
		byte *flags = &((s_object_header *)g_4e0300->data)[index & 0xffff].object->field_d8;
		*flags |= 1;
		state.flags |= 2;
		state.up = *up;
		state.forward = *forward;
		function_a99f0(index, (long *)&state);
	}
}

// @retail 0xa98b0
bool function_a98b0(long index, vector3f *position)
{
	s_object_view *object = ((s_object_header *)g_4e0300->data)[index & 0xffff].object;
	bool result = false;
	if (object->field14 == NONE && (object->field_d8 & 1))
	{
		s_z_copy_state state;
		z_copy_state_clear(&state);
		if (function_a9820((long *)&state, index) && (state.flags & 1))
		{
			*position = state.position;
			result = true;
		}
	}
	return result;
}

// @retail 0xa9940
bool function_a9940(long index, vector3f *up, vector3f *forward)
{
	s_object_view *object = ((s_object_header *)g_4e0300->data)[index & 0xffff].object;
	bool result = false;
	if (object->field14 == NONE && (object->field_d8 & 1))
	{
		s_z_copy_state state;
		z_copy_state_clear(&state);
		if (function_a9820((long *)&state, index) && (state.flags & 2))
		{
			*up = state.up;
			*forward = state.forward;
			result = true;
		}
	}
	return result;
}


struct s_z_unit_mapping_view
{
	byte unknown000[0xd4];
	long identifier;
	byte unknown0d8[0x130 - 0xd8];
	long mapping_index;
	byte unknown134[8];
	long player_index;
};

struct s_z_unit_mapping_entry
{
	long first;
	long second;
	long flags;
	byte unknown0c[0x90 - 0xc];
};

struct s_z_unit_mapping_world
{
	byte unknown000[0x8fc];
	s_z_unit_mapping_entry entries[1];
};

void __stdcall function_cbf60(long unit_index, bool active);
void function_14cad0(long player_index, long unit_index);
void function_152340(void);
long function_101f50(long object_index);

// @retail 0xa7bc0
void function_a7bc0(long index)
{
	s_z_unit_mapping_view *object = (s_z_unit_mapping_view *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
	if (object->mapping_index != NONE)
	{
		s_z_unit_mapping_world *world = (s_z_unit_mapping_world *)g_4cf77c;
		s_z_unit_mapping_entry *entry = &world->entries[object->mapping_index];
		entry->first = NONE;
		entry->second = NONE;
		entry->flags = 0;
		object->mapping_index = NONE;
		function_cbf60(index, false);
		object = (s_z_unit_mapping_view *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
		if (object->identifier != NONE)
			function_b58c0(object->identifier, 0x400);
	}
}

// @retail 0xa94b0
void function_a94b0(long index)
{
	s_z_unit_mapping_view *object = (s_z_unit_mapping_view *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
	long player_index = object->player_index;
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	*(long *)(player + 0x30) = *(long *)(player + 0x2c);
	function_14cad0(player_index, NONE);
	function_152340();
}

// @retail 0xa7cd0
void function_a7cd0(long index)
{
	long mode = g_4e6948->mode;
	if (mode >= 4 && mode <= 5)
	{
		switch (mode)
		{
		case 2:
		case 4:
			break;
		default:
		{
		byte *object = (byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
		long parent = NONE;
		if (object[0x12c] & 1)
			parent = *(long *)(object + 0x154);
		long slot = function_101f50(index);
		if (parent != NONE && slot >= 0 && slot < 4)
			function_a7a30(parent, 1 << (slot + 18));
			break;
		}
		}
	}
}


// @retail 0xa9a70
void function_a9a70(long index, s_z_copy_state *state)
{
	byte *object = (byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
	struct { vector3f difference; } geometry;
	vector3f const *position = (vector3f const *)(object + 0x64);
	vector3f *saved = &state->position;
	vector3f &difference = geometry.difference;
	difference.i = *(real const *)(object + 0x64) - state->position.i;
	difference.j = position->j - saved->j;
	difference.k = position->k - saved->k;
	if (sqrt(difference.i * difference.i + (difference.j * difference.j + difference.k * difference.k)) > 0.05000000074505806f)
	{
		saved->i = ((real volatile *)saved)[0] * 0.550000011920929f + ((real const volatile *)position)[0] * 0.44999998807907104f;
		real const *component = &saved->j;
		saved->j = *component * 0.550000011920929f + position->j * 0.44999998807907104f;
		saved->k = saved->k * 0.550000011920929f + position->k * 0.44999998807907104f;
	}
	else
	{
		*saved = *position;
		state->flags &= ~1;
	}
}


// @retail 0xa7700
bool function_a7700(long index, long which, long *player_out)
{
	long player = NONE;
	long mode = g_4e6948->mode;
	if (mode >= 4 && mode <= 5)
	{
		byte *object = (byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
		if ((1 << object[0xaa]) & 3)
		{
			player = *(long *)(object + 0x13c);
			switch (which)
			{
			case 0:
				if (player == NONE && *(long *)(object + 0x248) != NONE)
				{
					byte *parent = (byte *)((s_object_header *)g_4e0300->data)[*(long *)(object + 0x248) & 0xffff].object;
					player = *(long *)(parent + 0x13c);
				}
				break;
			case 1:
				if (player == NONE && *(long *)(object + 0x24c) != NONE)
				{
					byte *parent = (byte *)((s_object_header *)g_4e0300->data)[*(long *)(object + 0x24c) & 0xffff].object;
					player = *(long *)(parent + 0x13c);
				}
				break;
			default:
				break;
			}
		}
	}
	if (player_out)
		*player_out = player;
	return player != NONE;
}

// Event senders retain this local player eligibility call boundary.
__declspec(noinline) bool function_a76b0(long index, long which);

// @retail 0xa76b0
bool function_a76b0(long index, long which)
{
	bool result = false;
	if (g_4e6948->mode == 4 && function_a7700(index, which, &index))
	{
		byte *player = g_4e8c24->data + (index & 0xffff) * 0x21c;
		result = *(short *)(player + 0x28) != NONE;
	}
	return result;
}


bool function_e4050(long object_index);
bool function_cc410(long unit_index);

// @retail 0xaa970
bool function_aa970(long index)
{
 s_object_header *header = &((s_object_header *)g_4e0300->data)[index & 0xffff];
 bool result = false;
 if (!header->unknown03[0])
 {
  s_object_view *object = header->object;
  if ((TEST_FIELD_BIT(object->flag2) && !function_e4050(index)) || function_cc410(index) ||
   (bool)((*(dword *)((byte *)object + 0x134) >> 27) & 1))
   result = true;
 }
 return result;
}

#include "unknown_0d0690.h"
#include "unknown_0259d0.h"
#include "vector_products.h"

// @retail 0xaa9e0
void function_aa9e0(long index, point3f const *previous_position, vector3f const *velocity)
{
 s_object_header *header = &((s_object_header *)g_4e0300->data)[index & 0xffff];
 if (header->unknown03[0] == 1)
 {
  s_record_pool *objects = g_4e0300;
  byte *object = (byte *)header->object;
  real elapsed = g_510c54->rate;
  vector3f velocity_change;
  s_object_child_iterator iterator;

  if (previous_position)
  {
   vector3f displacement;

   vector3d_from_points3d(previous_position, (point3f const *)(object + 0x64), &displacement);
   /* retail's sums keep each store next to its add: the operand and the destination are
      separate pointers to the accumulated vector */
   vector3f *accumulated = (vector3f *)(object + 0x270);
   vector3f_add(accumulated, &displacement, (vector3f *)(object + 0x270));
  }
  if (velocity)
  {
   velocity_change.i = velocity->i - *(real *)(object + 0x88);
   velocity_change.j = velocity->j - *(real *)(object + 0x8c);
   velocity_change.k = velocity->k - *(real *)(object + 0x90);
   velocity_change.i *= elapsed;
   velocity_change.j *= elapsed;
   velocity_change.k *= elapsed;
  }
  function_d0620(&iterator, index);
  while (function_d0690(&iterator))
  {
   byte *child = (byte *)((s_object_header *)objects->data)[iterator.child_index & 0xffff].object;

   if (velocity)
   {
    vector3f *accumulated = (vector3f *)(child + 0x270);

    vector3f_add(accumulated, &velocity_change, (vector3f *)(child + 0x270));
   }
   *(dword *)(child + 0x134) |= 0x10000000;
  }
 }
}


long function_b5990(long tag_index, bool flag);
void function_b5920(long identifier, long object_index);
void object_dispatch_detach_simulation_entity(long object_index);
struct s_effect_object_placement;

__declspec(noinline)
// @retail 0xa7640
bool function_a7640(s_effect_object_placement *data)
{
 long tag_index = *(long *)data;
 if (tag_index == NONE) return false;
 if (g_4e6948->mode == 4)
 {
  if (function_b5990(tag_index, true) == NONE) return true;
  return false;
 }
 return true;
}

// @retail 0xa7a60
void function_a7a60(long index)
{
 s_object_header *header = &((s_object_header *)g_4e0300->data)[index & 0xffff];
 long identifier = header->object->field_d4;
 if (identifier != NONE)
 {
  function_b5920(identifier, index);
  s_object_view *object = ((s_object_header *)g_4e0300->data)[index & 0xffff].object;
  object_dispatch_detach_simulation_entity(index);
  object->field_d4 = NONE;
  object->field_d8 = 0;
 }
}

// @retail 0xa7970
void __stdcall function_a7970(long index, bool flag, long maximum, long *count, long *types, long *objects)
{
 s_object_header *header = &((s_object_header *)g_4e0300->data)[index & 0xffff];
 s_object_view *object = header->object;
 if (!(header->flags & 0x10) && object->field_d4 == NONE)
 {
  long type = function_b5990(object->definition_index, flag);
  if (type != NONE && *count < maximum)
  {
   types[*count] = type;
   objects[*count] = index;
   ++*count;
   for (long child = object->first_child; child != NONE; child = object->next_sibling)
   {
    object = ((s_object_header *)g_4e0300->data)[child & 0xffff].object;
    if ((bool)((*(dword *)((byte *)object + 4) >> 26) & 1))
     function_a7970(child, false, maximum, count, types, objects);
   }
  }
 }
}


#include "engine_peer.h"
struct c_entry_table
{
 void function_08a400(dword identifier);
};
long function_b57d0(long handler_index, long object_index);

// @retail 0xa77c0
void function_a77c0()
{
 c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
 if (engine)
 {
  long type = engine->get_current_id();
  if (type != NONE)
  {
   long identifier = function_b57d0(type, NONE);
   if (identifier != NONE)
   {
    s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
    c_entry_table *table = (c_entry_table *)&world->database->table;
    g_4e9ae8->value24 = identifier;
    table->function_08a400(identifier);
   }
  }
 }
}

// @retail 0xa8e90
void function_a8e90(long slot)
{
 long mode = g_4e6948->mode;
 if (mode >= 4 && mode <= 5 && mode != 4)
 {
  long identifier = function_b57d0(8, slot);
  if (identifier != NONE)
  {
   s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
   ((c_entry_table *)&world->database->table)->function_08a400(identifier);
   if (slot >= 0 && slot < 8) g_4eca60[slot] = identifier;
  }
 }
}

struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);

// @retail 0xa5d90
bool function_a5d90(void *data, s_entity_info *info, long *flags, long state)
{
 bool result = false;
 byte *creation = (byte *)data;
 if (info->field0 == NONE)
 {
  function_b7930(data, info->definition_index, NONE, 0);
  *(dword *)(creation + 0x18) |= 0x12;
  *(long *)(creation + 0xa8) = *(long *)((byte *)info + 0xc);
  if (*flags & 2)
  {
   *(point3f *)(creation + 0x1c) = *(point3f *)state;
   *flags &= ~2;
  }
  if (*flags & 4)
  {
   *(vector3f *)(creation + 0x28) = *(vector3f *)(state + 0xc);
   *(vector3f *)(creation + 0x34) = *(vector3f *)(state + 0x18);
   *flags &= ~4;
  }
  if (*flags & 8)
  {
   *(real *)(creation + 0x58) = *(real *)(state + 0x24);
   *flags &= ~8;
  }
  if (*flags & 0x10)
  {
   *(vector3f *)(creation + 0x40) = *(vector3f *)(state + 0x28);
   *flags &= ~0x10;
  }
  if (*flags & 0x20)
  {
   *(vector3f *)(creation + 0x4c) = *(vector3f *)(state + 0x34);
   *flags &= ~0x20;
  }
  result = true;
 }
 return result;
}


#include "unknown_1946f0.h"
void simulation_read_position(s_bitstream *stream, real *position, long bits);
void function_195070(s_bitstream *stream, vector3f *out, real lo, real hi, long bits);
bool function_a7570(vector3f const *vector);
bool function_a74c0(vector3f const *forward, vector3f const *up);

PRIVATE inline real z_decode_state_scalar(s_bitstream *stream, long bits, long maximum, real lo, real hi)
{
 long value = function_1959c0(stream, bits);
 real result;
 if (!value) result = lo;
 else if (value >= maximum) result = hi;
 else result = ((maximum - value) * lo + value * hi) * (1.0f / maximum);
 return result;
}
PRIVATE inline bool z_state_real_valid(real value)
{
 return (*(long *)&value & 0x7f800000) != 0x7f800000;
}

// matches retail byte for byte: both decoded counts are read into long locals, the first from the
// stored byte before the first word read and the second from the state field after it, which
// reproduces retail's register use in the widening and in the final bound test.
// Long, unsigned long and int first-count locals all match; an unsigned short local gives 37 and a
// byte second-count local gives 9.
// Earlier forms (before the 0x195240 argument
// score 9 (was 9): the two mask-count arguments are extended into ECX/EDX where retail
// uses EAX (so the first word store comes after the second count load), and the bound test
// holds the size in EAX where retail uses ECX. Earlier forms (before the 0x195240 argument
// fix, at 154/146): scoped inline depth zero 610; scoped global optimization off 368; decode
// cursor/countdown cursor 161/161; final native acceptance 155; mask-width reference 146;
// native external record 154; record/plain result and plain sticky predicates 289; external
// flags reference 154; decoder/classifier noinline declarations 465/349; unsigned ordinal 154;
// position/vector/scale/scalar field bindings 146 each; wide/byte sticky validity 352/146;
// decoder exits/lower-first 146/295; finite classifier 243; exponent extraction 146; decoded
// region count/state 146/146; native long/byte first validation 146/146; range-first and
// stream-last decoder inputs 146; short encoding count 146; unsigned native limit 460;
// scalar code input/lifetimes, field reference/pointer outputs 146 each; default lower result
// 295; native/word/byte code 440/301/291; word endpoint limit 146; native weights 146;
// endpoint products 227. Now: tail written >=, *8, !(>), early || false, nested, or through
// the result 9 each, `return a && b` 16; mask test `if (state[0x61])` 10; a byte mask local
// for every use 6 (keeps it in ESI; retail reads the field again), for the tests only 9;
// a long mask local 45; without the stream self-reference 9.
// Native tail fields/word array/full state/full state array/tail reference 9 each;
// native tail/full-state mask loops 53/53; native-state count reference 44.
// First/two decoded-word locals 9/9; count reference 13; word count 36; word-array
// destination 8 (first store fixed); byte count plus array 6 (ESI); first-count split/guard
// 8/8; remaining-bits bound 9; two-value loop 31; short/implicit word arrays 8/8;
// named bound inputs 8.
// @retail 0xa6d50
bool function_a6d50(long flags_address, long state_address, s_bitstream *stream)
{
 bool result = true;
 long *flags = (long *)flags_address;
 byte *state = (byte *)state_address;
 if (function_1957d0(stream))
 {
  *(bool *)(state + 0x4d) = function_1957d0(stream);
  *flags |= 1;
 }
 if (function_1957d0(stream))
 {
  simulation_read_position(stream, (real *)state, 16);
  *flags |= 2;
  result = function_a7570((vector3f *)state) != false;
 }
 if (function_1957d0(stream))
 {
  function_195240(stream, (vector3f *)(state + 0xc), (vector3f *)(state + 0x18));
  *flags |= 4;
  if (result && (function_a74c0((vector3f *)(state + 0xc), (vector3f *)(state + 0x18)))) result = true; else result = false;
 }
 if (function_1957d0(stream))
 {
  *(real *)(state + 0x24) = z_decode_state_scalar(stream, 7, 127, 0.0f, 10.0f);
  *flags |= 8;
  if (result && (z_state_real_valid(*(real *)(state + 0x24)))) result = true; else result = false;
 }
 if (function_1957d0(stream))
 {
  function_195070(stream, (vector3f *)(state + 0x28), 0.03f, 350.0f, 10);
  *flags |= 0x10;
  if (result && (function_a7570((vector3f *)(state + 0x28)))) result = true; else result = false;
 }
 if (function_1957d0(stream))
 {
  function_195070(stream, (vector3f *)(state + 0x34), 0.03f, 30.0f, 8);
  *flags |= 0x20;
  if (result && (function_a7570((vector3f *)(state + 0x34)))) result = true; else result = false;
 }
 if (function_1957d0(stream))
 {
  *(real *)(state + 0x40) = z_decode_state_scalar(stream, 8, 254, -1.0f, 1.0f);
  *(bool *)(state + 0x44) = function_1957d0(stream);
  *flags |= 0x40;
  if (result && (z_state_real_valid(*(real *)(state + 0x40)))) result = true; else result = false;
 }
 if (function_1957d0(stream))
 {
  *(real *)(state + 0x48) = z_decode_state_scalar(stream, 8, 254, 0.0f, 3.0f);
  *(bool *)(state + 0x4c) = function_1957d0(stream);
  *flags |= 0x80;
  if (result && (z_state_real_valid(*(real *)(state + 0x48)))) result = true; else result = false;
 }
 if (function_1957d0(stream))
 {
  state[0x50] = (byte)function_1959c0(stream, 4);
  if (result && (state[0x50] <= 16)) result = true; else result = false;
  for (long i = 0; i < 16; ++i)
  {
   state[0x51 + i] = (byte)function_1959c0(stream, 3);
   if (result && (state[0x51 + i] < 12)) result = true; else result = false;
  }
  *flags |= 0x100;
 }
 if (stream_read_bit(stream))
 {
  state[0x61] = (byte)function_1959c0(stream, 5);
  if (result && (state[0x61] <= 16)) result = true; else result = false;
  if (state[0x61] > 0)
  {
   long first_bit_count = state[0x61];
   *(word *)(state + 0x62) = (word)function_1959c0(stream, first_bit_count);
   long bit_count = state[0x61];
   *(word *)(state + 0x64) = (word)function_1959c0(stream, bit_count);
  }
  *flags |= 0x200;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3))
  return true;
 return false;
}


bool function_b5830(long *identifiers, long count, long const *handler_indices, long const *object_indices);
void function_bb780(long object_index, long value);

// @retail 0xa7870
void __stdcall function_a7870(long index)
{
 long mode = g_4e6948->mode;
 if (mode >= 4 && mode <= 5)
 {
  switch (mode)
  {
  case 2:
  case 4:
   break;
  default:
   {
    long count = 0;
    long objects[4], types[4], identifiers[4];
    function_a7970(index, true, 4, &count, types, objects);
    if (count == 1)
    {
     long identifier = function_b57d0(types[0], objects[0]);
     if (identifier != NONE)
     {
      s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
      c_entry_table *table = (c_entry_table *)&world->database->table;
      function_bb780(objects[0], identifier);
      table->function_08a400(identifier);
     }
    }
    else if (count > 1 && function_b5830(identifiers, count, types, objects))
    {
     s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
     c_entry_table *table = (c_entry_table *)&world->database->table;
     for (long i = 0; i < count; ++i)
     {
      long identifier = identifiers[i];
      function_bb780(objects[i], identifier);
      table->function_08a400(identifier);
     }
    }
   }
   break;
  }
 }
}


#include "flags_writer.h"
void simulation_write_position(s_bitstream *stream, real const *position, long bits, bool keep_inside);
void function_194830(s_bitstream *stream, bool value);
void function_194d30(s_bitstream *stream, vector3f const *up, vector3f const *forward);
void function_194c10(s_bitstream *stream, vector3f const *vector, real lo, real hi, long bits);
void function_1947e0(s_bitstream *stream, dword value, long bits);

PRIVATE inline void z_encode_state_scalar(s_bitstream *stream, real value, real minimum, real multiplier, long bits)
{
 real scaled = (value - minimum) * multiplier;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, bits);
}

// @retail 0xa69a0
bool function_a69a0(long a, long b, long c, long d, long e, bool f, long g)
{
 bool result = false;
 long const *requested_reference = &b;
 s_bitstream *stream = (s_bitstream *)e;
 byte const *state = (byte const *)d;
 s_flags_writer writer;
 flags_writer_initialize(&writer, stream, 0, 10, *requested_reference, g);
 if (writer.space)
 {
 if (flags_writer_begin(&writer, 0, "dead-exists"))
  function_194830(stream, *(bool const *)(state + 0x4d));
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 1, "position-exists"))
 {
  bool absolute = (byte)a != 0 || f;
  simulation_write_position(stream, (real const *)state, 16, absolute);
 }
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 2, "forward-and-up-exists"))
  function_194d30(stream, (vector3f const *)(state + 0xc), (vector3f const *)(state + 0x18));
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 3, "scale-exists"))
  z_encode_state_scalar(stream, *(real const *)(state + 0x24), 0.0f, 12.699999809265137f, 7);
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 4, "translational-velocity-exists"))
  function_194c10(stream, (vector3f const *)(state + 0x28), 0.03f, 350.0f, 10);
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 5, "angular-velocity-exists"))
  function_194c10(stream, (vector3f const *)(state + 0x34), 0.03f, 30.0f, 8);
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 6, "body-vitality-exists"))
 {
  z_encode_state_scalar(stream, *(real const *)(state + 0x40), -1.0f, 127.0f, 8);
  function_194830(stream, *(bool const *)(state + 0x44));
 }
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 7, "shield-vitality-exists"))
 {
  z_encode_state_scalar(stream, *(real const *)(state + 0x48), 0.0f, 84.66666412353516f, 8);
  function_194830(stream, *(bool const *)(state + 0x4c));
 }
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 8, "region-state-exists"))
 {
  stream_write_checked(stream, state[0x50], 4);
  for (long i = 0; i < 16; ++i) stream_write_checked(stream, state[0x51 + i], 3);
 }
 flags_writer_end(&writer);
 if (flags_writer_begin(&writer, 9, "constraint-state-exists"))
 {
  stream_write_checked(stream, state[0x61], 5);
  if (state[0x61] > 0)
  {
   function_1947e0(stream, *(word const *)(state + 0x62), state[0x61]);
   function_1947e0(stream, *(word const *)(state + 0x64), state[0x61]);
  }
 }
 flags_writer_end(&writer);
 *(dword *)c |= writer.written;
 result = true;
 }
 return result;
}


quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
void function_11d790(quaternionf const *q, real *angle, vector3f *axis);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);

PRIVATE inline void z_orientation_matrix(vector3f const *forward, vector3f const *up, matrix3x3 *matrix)
{
 vector3f *left = &matrix->left;
 matrix->forward = *forward;
 left->i = up->j * forward->k - forward->j * up->k;
 left->j = forward->i * up->k - forward->k * up->i;
 left->k = up->i * forward->j - forward->i * up->j;
 matrix->up = *up;
}

// @retail 0xa9b40
void function_a9b40(long index, s_z_copy_state *state)
{
 byte *object = (byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
 real saved_angle = 0.0f;
 real current_angle = 0.0f;
 matrix3x3 saved_matrix, current_matrix;
 quaternionf saved_rotation, current_rotation, blended;
 vector3f saved_axis, current_axis;
 z_orientation_matrix(&state->up, &state->forward, &saved_matrix);
 vector3f const *current_up = (vector3f const *)(object + 0x7c);
 z_orientation_matrix((vector3f const *)(object + 0x70), current_up, &current_matrix);
 vector3f const *current_forward = (vector3f const *)(object + 0x70);
 function_141f60(&saved_matrix, &saved_rotation);
 function_141f60(&current_matrix, &current_rotation);
 function_11d790(&saved_rotation, &saved_angle, &saved_axis);
 function_11d790(&current_rotation, &current_angle, &current_axis);
 if (current_angle - saved_angle > 0.0001f)
 {
  real weight = 0.35f;
  real dot = saved_rotation.i * current_rotation.i + saved_rotation.j * current_rotation.j +
   saved_rotation.k * current_rotation.k + saved_rotation.w * current_rotation.w;
  if (dot < 0.0f) weight = -0.35f;
  blended.i = current_rotation.i * weight + saved_rotation.i * 0.65f;
  blended.j = current_rotation.j * weight + saved_rotation.j * 0.65f;
  blended.k = current_rotation.k * weight + saved_rotation.k * 0.65f;
  blended.w = current_rotation.w * weight + saved_rotation.w * 0.65f;
  quaternion_unit_or_identity(&blended);
  function_141e10(&current_matrix, &blended);
  state->up = current_matrix.forward;
  state->forward = current_matrix.up;
 }
 else
 {
  state->up = *current_forward;
  state->forward = *current_up;
  state->flags &= ~2;
 }
}

// @retail 0xa9570
void function_a9570(long index)
{
 s_object_view *object = ((s_object_header *)g_4e0300->data)[index & 0xffff].object;
 if (object->field_d8 & 1)
 {
  if (*(long *)((byte *)object + 0x14) != NONE)
   function_a9640(index);
  else
  {
   bool changed = false;
   s_z_copy_state state;
   z_copy_state_clear(&state);
   function_a9820((long *)&state, index);
   if (state.flags & 1)
   {
    function_a9a70(index, &state);
    changed = true;
   }
   if (state.flags & 2)
   {
    function_a9b40(index, &state);
    changed = true;
   }
   if (changed)
   {
    function_a99f0(index, (long *)&state);
    if (state.flags) return;
   }
   object->field_d8 &= ~1;
  }
 }
}

class c_class_6a600;
long function_6a690(long index, c_class_6a600 *world, long value);

// @retail 0xa9500
bool function_a9500(long unit_index, long index)
{
 s_object_header *headers = (s_object_header *)g_4e0300->data;
 s_z_unit_mapping_view *object = (s_z_unit_mapping_view *)headers[unit_index & 0xffff].object;
 bool result = false;
 if (object->player_index == NONE)
 {
  s_z_unit_mapping_world *world = (s_z_unit_mapping_world *)g_4cf77c;
  if (world->entries[index].first == NONE)
  {
   long mapping = function_6a690(index, (c_class_6a600 *)world, unit_index);
   object = (s_z_unit_mapping_view *)((s_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
   object->mapping_index = mapping;
   function_cbf60(unit_index, true);
   result = true;
  }
 }
 return result;
}

// @retail 0xa7b30
void function_a7b30(long index)
{
 long mode = g_4e6948->mode;
 if (mode >= 4 && mode <= 5)
 {
  switch (mode)
  {
  case 2:
  case 4:
   break;
  default:
  {
   s_z_unit_mapping_view *object = (s_z_unit_mapping_view *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
   object->mapping_index = function_6a690(NONE, (c_class_6a600 *)g_4cf77c, index);
   function_cbf60(index, true);
   function_a7870(index);
   object = (s_z_unit_mapping_view *)((s_object_header *)g_4e0300->data)[index & 0xffff].object;
   if (object->identifier != NONE)
    function_b58c0(object->identifier, 0x400);
   break;
  }
  }
 }
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0xa7ab0
void function_a7ab0(long index)
{
 s_object_view *object = ((s_object_header *)g_4e0300->data)[index & 0xffff].object;
 long identifier = object->field_d4;
 if (identifier != NONE)
 {
  object_dispatch_detach_simulation_entity(index);
  object->field_d4 = NONE;
  object->field_d8 = 0;
  _ReadWriteBarrier();
  long state = ((s_z_state_world *)g_4cf77c)->state;
  s_z_state_world *world = (s_z_state_world *)g_4cf77c;
  if (state == 4 || state == 5)
  {
   s_z_entity_record *entity = &world->database->entities[identifier & 0x3ff];
   entity->active = false;
   entity->object_index = NONE;
  }
  function_b8540(index);
 }
}
#pragma function(_ReadWriteBarrier)


PRIVATE inline long creation_definition_lookup(byte *globals, long index)
{
    long result = NONE;
    if (globals)
    {
        long count = *(long *)(globals + 0x3d8);
        if (count > 0 && (index < 0 ? 0 : index > count - 1 ? count - 1 : index) == index)
            result = (*(long **)(globals + 0x3dc))[index];
    }
    return result;
}

// @retail 0xa6810
bool function_a6810(s_entity_info *info, s_bitstream *stream)
{
    long definition_index = NONE;
    long index = function_1959c0(stream, 9) - 1;
    byte *globals = (byte *)g_4e0350;
    if (index != NONE)
        definition_index = creation_definition_lookup(globals, index);
    info->definition_index = definition_index;
    if (function_1957d0(stream))
        info->field0 = function_1959c0(stream, 13);
    else
        info->field0 = NONE;
    info->byte8 = (byte)(function_1959c0(stream, 7) - 1);
    if (function_1957d0(stream))
    {
        ((byte *)info)[0xc] = (byte)function_1959c0(stream, 6);
        ((byte *)info)[0xd] = (byte)function_1959c0(stream, 6);
        ((byte *)info)[0xe] = (byte)function_1959c0(stream, 4);
    }
    else
    {
        ((byte *)info)[0xc] = 0;
        ((byte *)info)[0xd] = 0;
        ((byte *)info)[0xe] = 0;
    }
    bool valid = stream->bit_position <= stream->size_in_bytes * 8 && info->definition_index != NONE;
    if (info->byte8 != 0xff)
        valid = valid && (char)info->byte8 >= 0 && (char)info->byte8 < *(long *)(globals + 0x120);
    return valid;
}


bool __stdcall function_14f2e0(long player_index);

// @retail 0xa9440
bool function_a9440(long unit_index, long player_index)
{
    byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
    bool result = false;
    if (!(player[2] & 2) && *(long *)(player + 0x2c) == NONE)
    {
        byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
        if (*(long *)(unit + 0x130) != NONE)
            function_a7bc0(unit_index);
        function_14cad0(player_index, unit_index);
        function_14f2e0(player_index);
        result = true;
    }
    return result;
}
