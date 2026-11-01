// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_23B500.CPP: the influences that weigh the game engine's spawn
   points (0x23b500..0x23bc40) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>
#include "globals.h"
#include "unknown_157450.h"
#include "object_iterator.h"
#include "unknown_0d0690.h"

/* the influences' weights (g_502258, cleared by unknown_157450.cpp): two
   shared values, then three per influence type */
struct s_spawn_influence_type
{
	real value0;
	real value4;
	real value8;
};

struct s_spawn_influence_globals
{
	real value0;
	real value4;
	s_spawn_influence_type types[12];
};

extern dword g_502258[0x27];

PRIVATE const byte g_4709f0[48] =
{
	140, 7, 0, 5, 141, 7, 0, 6, 142, 7, 0, 13, 143, 7, 0, 16,
	144, 7, 0, 13, 145, 7, 0, 17, 146, 7, 0, 17, 147, 7, 0, 14,
	148, 7, 0, 14, 149, 7, 0, 14, 150, 7, 0, 9, 151, 7, 0, 20
};
byte const *g_5022f0;

struct s_spawn_type_override_23aea0
{
	short type;
	short unknown02;
	s_spawn_influence_type values;
};

struct s_spawn_settings_23aea0
{
	real value0;
	real value4;
	byte unknown08[0x48 - 8];
	long count;
	s_spawn_type_override_23aea0 *types;
};

struct s_spawn_scenario_23aea0
{
	byte unknown00[0x318];
	long count;
	s_spawn_settings_23aea0 *settings;
};

// score 105 (was 105): addressing the weights through the neighbouring global's own symbol
// (g_502248: four longs, the two shared values, then the twelve type records) still folds the
// destination into a pointer induction (148), with the type records read as dwords or as reals,
// so retail's integer table offset with its base in the displacement stays out of reach.
// Earlier (105): the destination still folds into a pointer induction — a struct-typed
// member access and a byte-offset access both canonicalise back to it — so retail's integer
// table offset with its base in the displacement stays out of reach; the definition register
// (ECX against retail's EAX) and the override block's extra lea esp, [esp] still differ.
// Earlier (105): definition uses ECX and destination uses a pointer instead of retail integer offset; override alignment differs.
// Earlier (105): a block struct over the merged global, byte offsets taken from the array and
// a real global struct all leave VC strength-reducing the destination into a pointer induction, so
// retail's integer offset with its base in the displacement is not reachable from this side.
// retail's weights are addressed from a base 16 bytes below the array (its
// displacement base is the neighbouring global g_502248) with the byte offset in a register; an
// element pointer reaches retail's store form (103) but the base, the bound and the override
// alignment still differ.
// definition uses ECX and destination uses a pointer instead of retail integer offset; override alignment differs.
// Earlier (105): retail's weights are addressed from a base 16 bytes below the array (its
// Earlier (105): definition uses ECX and destination uses a pointer instead of retail integer offset; override alignment differs.
// Earlier at 103: the definition now stays in EAX and the source pointer in ECX, but
// the destination is still an absolute pointer induction instead of retail's integer table
// offset 0x18..0xa8; the prologue publishes the default table after the first scalar store
// and the override loop keeps an extra lea esp, [esp].
// the destination is an absolute pointer induction instead of retail's
// integer table offset 0x18..0xa8 (16 stores 3 bytes shorter each, so the override loop gains
// an alignment pad); retail keeps the definition in EAX and forms the source pointer in ECX
// between the two scalar stores.
// actual batch/type ranges 139/227; inner type ordinal 139; direct twelve types 254;
// source records with direct/batch/read-only ranges 257/227/227; complete override 140.
// Word/byte batch ordinals 141/141; word/byte entry ordinals 242/242;
// both word/byte 253/253; early table publication and later scenario receiver 105/105.
// integer destination offset and definition register remain; adjacent
// component loop/word table 242/242; record cursor/batch cursor 240/240; actual batch bases 239;
// direct twelve types 254; unsigned component/native records 242/242.
// destination pointer replaces the retail integer table offset;
// flat index 254; zero-based batches 139; typed records 137; flat typed records 121;
// unsigned indices 141; batch steps 139; const records 137; full source table 139.
// Read-only source 105; direct header link 105; scenario after copy 158;
// isolated default-source phase 158; header field at consumers 135;
// global object/override fields at consumers 105/107; explicit override values 105.
// Default/override/both memcpy 145/140/172; native scalar defaults 105; shared tuple address 123;
// raw word
// array/seven-word records 121/121; shared override type 122.
// native source groups 121; readonly source groups 121; real source groups 121; native destination
// records 121; native tuple assignment 143; complete source table 103; native group owner 121;
// native record range 154. Integer destination induction remains; two-point table gain rejected.
// Unsigned outer counter 141; outer do-while 105; inner do-while 242; table pointer as the
// globals' last member (retail's memset clears it) 102; running destination index 120.
// No-link compiles of 1728 loop shapes, counter types and destination forms all fold the
// global into a pointer induction variable; retail keeps the integer offset 0x18..0xa8.
// Scalar-default memcpy/memmove 142/156; defaults plus scalar overrides 142/141;
// derived type dword/short/word/byte with long ordinals 105/325/325/348.
// A twelve-byte destination record indexed by the type ordinal moves the definition into
// EAX and the source pointer into ECX (105 -> 103); the same record reached through a
// pointer below the table regresses to 149.
// Round 421: override cursor 120; scalar override snapshots 119; cursor/scalar snapshots 137; scenario each-use
// 158; cursor/scenario each-use 163; scalar/scenario each-use 161; all override ownership 169; all
// ownership/native override assignment 179.
// Lane 14 r11: one `for (type = 2; type < 14; type++)` loop over struct fields 105 (VC unrolls it
// by six into retail's two passes; same code as the nested loops); that loop with an entry
// pointer 103; nested loops with an entry reference 103; an inline element accessor (index
// parameter) 103; a twelve-byte array wrapper with an indexing operator 103; a walking source
// record pointer 121. One-file screens (no link), all a pointer induction: const pointer or
// reference to the globals, a global defined here/static/as a struct, unsized extern, array
// reference local, while loop, `<= 13`, zero-based ordinal, struct copy (not unrolled).
// Screens show VC keeps an integer offset only when two different global symbols share the
// induction, and then rebases it to start at 0; retail's starts at 0x18 (12 * type, not rebased).
// Round 9 (lane 2, fast): retail's offset is 12 * type for type 2..13 with the base 0x18 below the first
// weight, i.e. `types[type - 2]` with `type` the definition's entry index; written that way as one loop
// 105, the same over the raw dwords 254 (278 bytes, not unrolled), as the nested loops 105, zero-based
// nested loops 139 (374 bytes), nested loops through an element pointer 103. Ours always ends the loop on a
// pointer compare (`cmp eax, &weights_end`), retail on the offset (`cmp eax, 0xa8`).
// Round 126 (real `--fast`): a zero-based loop over the twelve records (source from +0x180) 105; a named
// `real const *` source indexed by type * 7 121; one twelve-byte record copy per type 257 (279 bytes, not
// unrolled); the table pointer stored after the two scalar defaults 105; byte-offset destination 254 (278).
// One-link LTCG screens (exact for this function, which calls nothing): a forceinline copy helper taking
// the record pointer (2..13 or 0..11) gives the base code; an out-of-file copy function is not inlined into
// this /Ob1 file even when its definition is `inline` (called). Retail's 12 * type offset with the table
// address folded into the displacement looks like a loop strength-reduced over a register base.
// DL200: Overlap-safe default/override/both tuple copies 258/146/282, each with a new reduced
// caller loss at 0x1574c0; all rejected. Consistent tuple byte-field orders 0/8/4, 4/0/8, 4/8/0,
// 8/0/4 and 8/4/0 give 117/117/111/111/117. Original two-by-six copies, real field values, scalar
// overrides, signed guards and includes restored at 105. Integer destination induction and
// definition receiver remain.
// Round 46 (lane 9): one loop over type 2..13 with the `type * 3 - 4` index 254 (278 bytes, not unrolled), the
// same with `(type - 2) * 3 + 2` 254, with an unsigned short type 256; nested loops with a per-batch source
// pointer 121; one loop storing through `(&globals->value0)[type * 3 - 4]` 105. Diagnostic (not kept): /Os,
// /Ot and /Oy- keep the pointer induction; /Og- drops the loop optimisation altogether.
// Round 322: Actual two-by-six destination array/per-batch receiver with field/native/library/grouped-input copies 105/145/145/123 each. Integer destination induction and definition EAX remain; all original tables, copies, overrides and body restored.
// Round 56 (lane 12, fast): the outer loop over the first type of each batch (`first = 2; first < 14;
// first += 6`, type = first + j) 139 (374 bytes); that through the records 139; the outer counter
// stepping by six (`i < 12; i += 6`) 105; one type loop through a source record pointer and
// `types[type - 2]` 123; a walking source pointer with dword copies 255 (278 bytes). One-link
// screens with the global unresolved (an external symbol) still fold it into a pointer induction.
// Round 40 (screen, base 100): a per-type memcpy in one type loop 257 (279 bytes), in the two-by-six loops
// 145 (406 bytes), into `&g_502258[type * 3 - 4]` 257; `long index = type * 3` named in the nested loops 98
// (aligned 25), in one loop 252; a named dword source record pointer 118; a types pointer two records below
// the array 157 (378 bytes); a two-dimensional view 100; real-typed fields in one type loop 100: always the
// pointer induction.
// Round 104 (lane 8, fast): retail's displacement 0x502248 is g_502258 - 16 (`g_502258[type * 3 - 4]`
// on an integer IV type * 12 from 0x18, exit `cmp eax, 0xa8`). `long index = type * 3` named in the
// nested loops 103 (definition in retail's EAX and the source lea between the scalar stores; IV still a
// pointer); with the scenario read first 110; one type loop with the index 252 (not unrolled); `index =
// type * 3 - 4` 103; the index plus a named source record 121; the index with the table store after
// the defaults 104.
// Round 37 (m3 lane 14, fast): the override type read once into a local 122 (367 bytes; retail re-reads
// it per store); the inner loop over type (`type = i * 6 + 2; type < i * 6 + 8`) 226 (313); one
// function-scope type for both loops 122; nested loops with a twelve-byte record assignment 145 (406);
// nested loops through a named source record pointer 123. No other retail function uses this
// integer-offset store idiom, so no matched sibling shows its form.
// @retail 0x23aea0
void function_23aea0()
{
	byte *tag = g_4e3b44[g_4e034c->index & 0xffff].bytes;
	byte *definition = *(byte **)(tag + 0xc);
	s_spawn_scenario_23aea0 *scenario = (s_spawn_scenario_23aea0 *)g_4e0350;
	s_spawn_influence_globals *globals = (s_spawn_influence_globals *)g_502258;
	g_5022f0 = g_4709f0;
	globals->value4 = *(real *)(definition + 0x154);
	globals->value0 = *(real *)(definition + 0x150);
	for (long i = 0; i < 2; i++)
	{
		for (long j = 0; j < 6; j++)
		{
			long type = i * 6 + j + 2;
			g_502258[type * 3 - 4] = *(dword *)(definition + 0x148 + type * 0x1c);
			g_502258[type * 3 - 3] = *(dword *)(definition + 0x14c + type * 0x1c);
			g_502258[type * 3 - 2] = *(dword *)(definition + 0x150 + type * 0x1c);
		}
	}
	if (scenario->count > 0)
	{
		s_spawn_settings_23aea0 *settings = scenario->settings;
		if (settings->value0 != 0.f)
			globals->value4 = settings->value0;
		if (settings->value4 != 0.f)
			globals->value0 = settings->value4;
		for (long j = 0; j < settings->count; j++)
		{
			s_spawn_type_override_23aea0 *entry = &settings->types[j];
			globals->types[entry->type].value0 = entry->values.value0;
			globals->types[entry->type].value4 = entry->values.value4;
			globals->types[entry->type].value8 = entry->values.value8;
		}
	}
}

/* an influence on the spawn points (0x20 bytes) */
struct s_spawn_influence
{
	point3f point;
	real value0c;
	real value10;
	real value14;
	real value18;
	real value1c;
};

/* the influences gathered for one spawn (at most 0x80) */
struct s_spawn_influence_list
{
	long count;
	s_spawn_influence influences[0x80];
};

__forceinline void spawn_influence_add(s_spawn_influence_list *list, point3f const *point,
	real value0c, real value10, real value14, real value18, real value1c)
{
	if (list->count < 0x80)
	{
		s_spawn_influence *influence = &list->influences[list->count++];

		influence->point = *point;
		influence->value0c = value0c;
		influence->value10 = value10;
		influence->value14 = value14;
		influence->value18 = value18;
		influence->value1c = value1c;
	}
}

/* adds an influence of this type at the point */
// @retail 0x23ba10
void function_23ba10(long type, point3f const *point, s_spawn_influence_list *list)
{
	s_spawn_influence_globals *globals = (s_spawn_influence_globals *)g_502258;
	s_spawn_influence_type *definition = &globals->types[type];

	spawn_influence_add(list, point, definition->value0, definition->value4, globals->value0,
		globals->value4, definition->value8);
}

/* the influences' weight at the point: an influence counts fully within
   its inner radius and less towards its outer radius (on the ground plane,
   between its heights above and below) */
// @retail 0x23b060
real function_23b060(s_spawn_influence_list *list, point3f const *point)
{
	real total = 0.0f;

	if (game_engine_get())
	{
		long remaining = list->count;
		if (remaining > 0)
		{
			real const *values = &list->influences[0].value10;
			do
			{
				real weight = 0.0f;
				real dy = point->y - values[-3];
				real dx = point->x - values[-4];
				real full_weight = values[3];
				real outer_radius = values[0];
				real inner_radius = values[-1];
				real dz = point->z - values[-2];
				real distance_squared = dx * dx + dy * dy;

				if (outer_radius * outer_radius > distance_squared &&
					values[1] >= dz && dz >= -values[2])
				{
					if (inner_radius * inner_radius > distance_squared || inner_radius >= outer_radius)
					{
						weight = full_weight;
					}
					else
					{
						weight = (real)((1.0f - (sqrt(distance_squared) - inner_radius) / (outer_radius - inner_radius)) * full_weight);
					}
				}
				total += weight;
				values += 8;
			} while (--remaining);
		}
	}
	return total;
}

/* the spawn settings of the game variant (the two distances at +0x1c) */
struct s_spawn_settings_view
{
	byte unknown00[0x1c];
	real distance1c;
	real distance20;
};

/* the scenario's spawn data: its first block gives the distances */
struct s_spawn_scenario_view
{
	byte unknown000[0x318];
	long spawn_data_count;
	real *spawn_data;
};

struct s_spawn_tag_header_view
{
	byte unknown000[0x16c];
	long globals_index;
};

/* the globals' default distances */
struct s_spawn_globals_tag_view
{
	byte unknown00[0xc];
	byte *data;
};

/* the two distances spawning uses: the variant's, else the scenario's, else
   the globals' */
// @retail 0x23b3f0
void function_23b3f0(s_spawn_settings_view *settings, real *distance20, real *distance1c)
{
	s_spawn_scenario_view *scenario = (s_spawn_scenario_view *)g_4e0350;

	if (settings->distance1c == 0.0f)
	{
		long globals_index = ((s_spawn_tag_header_view *)g_4e034c)->globals_index;
		s_spawn_globals_tag_view *tag = (s_spawn_globals_tag_view *)g_4e3b44[globals_index & 0xffff].flags;
		long scenario_count = scenario->spawn_data_count;
		byte *globals = tag->data;

		if (scenario_count > 0)
		{
			if (scenario->spawn_data[0] == 0.0f)
				*distance1c = (real)fabs(*(real *)(globals + 0x154));
			else
				*distance1c = (real)fabs(scenario->spawn_data[0]);
		}
		else
		{
			*distance1c = (real)fabs(*(real *)(globals + 0x154));
		}
	}
	else
	{
		*distance1c = (real)fabs(settings->distance1c);
	}
	if (settings->distance20 == 0.0f)
	{
		long globals_index = ((s_spawn_tag_header_view *)g_4e034c)->globals_index;
		s_spawn_globals_tag_view *tag = (s_spawn_globals_tag_view *)g_4e3b44[globals_index & 0xffff].flags;
		long scenario_count = scenario->spawn_data_count;
		byte *globals = tag->data;

		if (scenario_count > 0)
		{
			if (scenario->spawn_data[1] == 0.0f)
				*distance20 = (real)fabs(*(real *)(globals + 0x150));
			else
				*distance20 = (real)fabs(scenario->spawn_data[1]);
		}
		else
		{
			*distance20 = (real)fabs(*(real *)(globals + 0x150));
		}
	}
	else
	{
		*distance20 = (real)fabs(settings->distance20);
	}
}

static __forceinline real spawn_random(bool deterministic, dword *seed)
{
	dword random;

	if (deterministic)
	{
		random = _random(seed, NULL, 0);
	}
	else
	{
		random = _random(&g_4e7408->seed, NULL, 0);
	}
	return (real)random * (1.f / 65535.f);
}

/* the weight of a spawn point: the influences' at the point, plus a random
   share of the globals' random weight */
// @retail 0x23ba90
real function_23ba90(point3f const *point, long player_index, bool deterministic, s_spawn_influence_list *list, dword *seed)
{
	/* the player stays on the stack, unused */
	long *player_index_reference = &player_index;
	point3f position = *point;
	real weight = function_23b060(list, &position);

	if (game_engine_get())
	{
		s_spawn_tag_header_view *header = (s_spawn_tag_header_view *)g_4e034c;
		byte *globals = ((s_spawn_globals_tag_view *)g_4e3b44[header->globals_index & 0xffff].flags)->data;

		real *random_weight_pointer = *(real **)(globals + 0x534);
		real *const *random_weight_reference = &random_weight_pointer;
		bool use_seed = *(volatile bool *)&deterministic;
		double random_weight = **random_weight_reference;
		real random;
		if (use_seed)
			random = (real)_random(seed, NULL, 0);
		else
			random = (real)(dword)_random(&g_4e7408->seed, NULL, 0);
		weight += random_weight * ((real)random * (1.f / 65535.f));
	}
	return weight;
}

/* a player as spawning sees it */
struct s_spawn_player_view
{
	byte unknown000[0x2c];
	long unit_index;
	byte unknown030[0xc0 - 0x30];
	char team;
	byte unknown0c1[0x21c - 0xc1];
};

struct s_spawn_zone_23b170
{
	dword unknown00;
	dword teams;
	dword modes;
	dword unknown0c;
	point3f position;
	real distance1c;
	real distance20;
	real inner_radius;
	real outer_radius;
	real weight;
};

struct s_spawn_zone_block_23b170
{
	long count;
	s_spawn_zone_23b170 *zones;
};

class c_spawn_filter_23b170
{
public:
#define SPAWN_FILTER_SLOT(n) virtual void slot##n() = 0;
	SPAWN_FILTER_SLOT(0) SPAWN_FILTER_SLOT(1) SPAWN_FILTER_SLOT(2)
	SPAWN_FILTER_SLOT(3) SPAWN_FILTER_SLOT(4) SPAWN_FILTER_SLOT(5)
	SPAWN_FILTER_SLOT(6) SPAWN_FILTER_SLOT(7) SPAWN_FILTER_SLOT(8)
	SPAWN_FILTER_SLOT(9) SPAWN_FILTER_SLOT(10) SPAWN_FILTER_SLOT(11)
	SPAWN_FILTER_SLOT(12) SPAWN_FILTER_SLOT(13) SPAWN_FILTER_SLOT(14)
	SPAWN_FILTER_SLOT(15) SPAWN_FILTER_SLOT(16) SPAWN_FILTER_SLOT(17)
	SPAWN_FILTER_SLOT(18) SPAWN_FILTER_SLOT(19) SPAWN_FILTER_SLOT(20)
	SPAWN_FILTER_SLOT(21) SPAWN_FILTER_SLOT(22) SPAWN_FILTER_SLOT(23)
	SPAWN_FILTER_SLOT(24) SPAWN_FILTER_SLOT(25) SPAWN_FILTER_SLOT(26)
	SPAWN_FILTER_SLOT(27) SPAWN_FILTER_SLOT(28) SPAWN_FILTER_SLOT(29)
	SPAWN_FILTER_SLOT(30) SPAWN_FILTER_SLOT(31)
    virtual void collect(long player_index, s_spawn_influence_list *list) = 0;
#undef SPAWN_FILTER_SLOT
	virtual bool accepts(long player_index, s_spawn_zone_23b170 const *zone) = 0;
};

// @retail 0x23b170
void function_23b170(long player_index, s_spawn_influence_list *list)
{
	s_spawn_player_view *player = &((s_spawn_player_view *)g_4e8c24->data)[player_index & 0xffff];
	s_spawn_scenario_23aea0 *scenario = (s_spawn_scenario_23aea0 *)g_4e0350;
	if (scenario->count > 0)
	{
		byte *settings = (byte *)scenario->settings;
		s_spawn_zone_block_23b170 *block;
		if (player->unknown000[2] & 8)
			block = (s_spawn_zone_block_23b170 *)(settings + 0x58);
		else
			block = (s_spawn_zone_block_23b170 *)(settings + 0x50);
		for (long i = 0; i < block->count; i++)
		{
			s_spawn_zone_23b170 *zone = &block->zones[i];
			bool teams = game_engine_get() != NULL;
			if (teams)
				teams = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
			if (teams && player->team != NONE)
			{
				bool accepted = false;
				for (long j = 0; j < 9; j++)
				{
					if ((function_xaee93d()->assigned_teams & (1 << j)) && (zone->teams & (1 << j)))
					{
						short designator = function_xaee93d()->team_designators[j];
						if (designator == player->team)
							accepted = true;
					}
				}
				if (!accepted)
					continue;
			}
			if (zone->modes)
			{
				bool accepted;
				switch (function_xaee93d()->engine_index)
				{
				case 2: accepted = (bool)(zone->modes & 1); break;
				case 1: case 9: accepted = (bool)((zone->modes >> 3) & 1); break;
				case 3: accepted = (bool)((zone->modes >> 1) & 1); break;
				case 4: accepted = (bool)((zone->modes >> 2) & 1); break;
				case 7: accepted = (bool)((zone->modes >> 6) & 1); break;
				case 8: accepted = (bool)((zone->modes >> 7) & 1); break;
				default: __assume(0);
				}
				if (!accepted)
					continue;
			}
			if (((c_spawn_filter_23b170 *)game_engine_get())->accepts(player_index, zone))
			{
				real distance20, distance1c;
				function_23b3f0((s_spawn_settings_view *)zone, &distance20, &distance1c);
				spawn_influence_add(list, &zone->position, zone->inner_radius,
					zone->outer_radius, distance20, distance1c, zone->weight);
			}
		}
	}
}

struct s_spawn_player_iterator
{
	s_spawn_player_view *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

bool player_iterator_next(long *iterator);

/* a unit as spawning sees it: its position, and whether it is dead */
struct s_spawn_unit_view
{
	byte unknown000[0x30];
	point3f position;
	byte unknown03c[0x10a - 0x3c];
	word flag0 : 1;
	word flag1 : 1;
	word dead : 1;
};

struct s_spawn_object_header_view
{
	byte unknown00[8];
	s_spawn_unit_view *object;
};

/* where each player last died (in the game engine globals at +0x558) */
struct s_spawn_player_state_view
{
	bool valid;
	byte unknown01[3];
	point3f position;
	short value10;
	byte unknown12[0x18 - 0x12];
};

struct s_spawn_engine_globals_view
{
	byte unknown000[0x558];
	s_spawn_player_state_view players[16];
};

/* the engine's test of two teams (false without an engine) */
static inline bool spawn_teams_test_23b8e0(short team, short other_team)
{
	bool result = false;
	c_engine_peer *engine = game_engine_get();

	if (engine)
	{
		result = engine->p27(team, other_team);
	}
	return result;
}

/* adds the other players' influences: a living one as a friend or a foe, a
   dead one where it died */
// @retail 0x23b8e0
void function_23b8e0(long player_index, s_spawn_influence_list *list)
{
	s_spawn_player_view *player = &((s_spawn_player_view *)g_4e8c24->data)[player_index & 0xffff];
	s_spawn_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		s_spawn_player_view *other = iterator.player;
		long index = iterator.index;
		long unit_index = other->unit_index;

		if (unit_index != NONE && index != player_index)
		{
			s_spawn_unit_view *unit = ((s_spawn_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;

			if (!TEST_FIELD_BIT(unit->dead))
			{
				if (other->team == player->team)
				{
					function_23ba10(1, &unit->position, list);
				}
				else
				{
					function_23ba10(0, &unit->position, list);
				}
			}
		}
		else
		{
			if (!game_engine_get() || !function_x340af0() || !spawn_teams_test_23b8e0(player->team, other->team))
			{
				s_spawn_player_state_view *state = &((s_spawn_engine_globals_view *)g_4e9ae8)->players[index & 0xffff];

				if (state->valid && state->value10)
				{
					function_23ba10(10, &state->position, list);
				}
			}
		}
	}
}

struct s_spawn_moving_object_23b500
{
	long tag_index;
	dword flags;
	byte unknown08[0x30 - 8];
	point3f position;
	byte unknown3c[0x88 - 0x3c];
	vector3f velocity;
	byte unknown94[0xaa - 0x94];
	byte type;
	byte unknownab[0x10a - 0xab];
	word unit_flag0 : 1;
	word unit_flag1 : 1;
	word unit_dead : 1;
	byte unknown10c[0x13c - 0x10c];
	long player_index;
};

struct s_spawn_motion_settings_23b500
{
	byte unknown00[8];
	real projectile_weight;
	real projectile_inner;
	real projectile_outer;
	real projectile_time;
	real minimum_speed;
	real vehicle_weight;
	real maximum_radius;
	real vehicle_time;
};

/* the point a scaled vector away from a point */
static inline void point_along_vector_23b500(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}


// @retail 0x23b500
void function_23b500(long player_index, s_spawn_influence_list *list)
{
	byte *globals_tag = *(byte **)(g_4e3b44[g_4e034c->index & 0xffff].bytes + 0xc);
	s_spawn_motion_settings_23b500 *settings = *(s_spawn_motion_settings_23b500 **)(globals_tag + 0x534);
	s_spawn_player_view *player = &((s_spawn_player_view *)g_4e8c24->data)[player_index & 0xffff];
	struct
	{
		s_spawn_moving_object_23b500 *object;
		s_type_f1af8e iterator;
	} objects;
	objects.iterator.signature = 0x86868686;
	objects.iterator.type_mask = 0x22;
	objects.iterator.flags = 0;
	objects.iterator.index = 0;
	objects.iterator.object_index = NONE;
	while ((objects.object = (s_spawn_moving_object_23b500 *)function_baeb0(&objects.iterator)) != 0)
	{
		s_spawn_moving_object_23b500 *object = objects.object;
		long type_mask = 1 << object->type;
		if (type_mask & 2)
		{
			if (TEST_FIELD_BIT(object->unit_dead) || (bool)((object->flags >> 26) & 1))
				continue;
			bool no_unit_child = true;
			bool friendly = true;
			s_object_child_iterator children;
			function_d0620(&children, objects.iterator.object_index);
			while (function_d0690(&children))
			{
				s_spawn_moving_object_23b500 *child = (s_spawn_moving_object_23b500 *)
					((s_spawn_object_header_view *)g_4e0300->data)[children.child_index & 0xffff].object;
				if ((1 << child->type) & 1)
				{
					no_unit_child = false;
					if (child->player_index != NONE &&
						((s_spawn_player_view *)g_4e8c24->data)[child->player_index & 0xffff].team != player->team)
						friendly = false;
				}
			}
			if (no_unit_child)
				function_23ba10(4, &object->position, list);
			else if (friendly)
				function_23ba10(3, &object->position, list);
			else
				function_23ba10(2, &object->position, list);
			real speed_squared = length_sq3f(&object->velocity);
			if (speed_squared > settings->minimum_speed * settings->minimum_speed)
			{
				real radius = (real)sqrt(speed_squared);
				radius = radius < 0.f ? 0.f : radius > settings->maximum_radius ? settings->maximum_radius : radius;
				point3f predicted;
				point_along_vector_23b500(&object->position, &object->velocity, settings->vehicle_time, &predicted);
				spawn_influence_add(list, &predicted, radius, radius * 1.2f,
					((s_spawn_influence_globals *)g_502258)->value0,
					((s_spawn_influence_globals *)g_502258)->value4, settings->vehicle_weight);
			}
		}
		else if (type_mask & 0x20)
		{
			byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
			if (*(real *)(definition + 0xcc) > 0.f && *(real *)(definition + 0xd0) > 0.f)
			{
				point3f predicted;
				point_along_vector_23b500(&object->position, &object->velocity, settings->projectile_time, &predicted);
				spawn_influence_add(list, &predicted, settings->projectile_inner, settings->projectile_outer,
					((s_spawn_influence_globals *)g_502258)->value0,
					((s_spawn_influence_globals *)g_502258)->value4, settings->projectile_weight);
			}
		}
	}
}

// @retail 0x23bc40
void function_23bc40(long player_index, s_spawn_influence_list *list)
{
    list->count = 0;
    if (game_engine_get())
    {
        function_23b170(player_index, list);
        function_23b500(player_index, list);
        function_23b8e0(player_index, list);
        ((c_spawn_filter_23b170 *)game_engine_get())->collect(player_index, list);
    }
}

bool function_1c5210(transform4x3f const *matrix, void *shape, long excluded_component, long filter);

// @retail 0x23bb70
bool function_23bb70(long tag_index, point3f const *position)
{
    bool result = true;
    if (tag_index != NONE)
    {
        byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
        transform4x3f matrix;
        matrix.scale = 1.f;
        matrix.forward.i = 1.f;
        matrix.forward.j = 0.f;
        matrix.forward.k = 0.f;
        matrix.left.i = 0.f;
        matrix.left.k = 0.f;
        matrix.left.j = 1.f;
        matrix.up.i = 0.f;
        matrix.up.j = 0.f;
        matrix.up.k = 1.f;
        matrix.position = *position;
        matrix.position.z += 0.0701f;
        void *shape;
        if (*(long *)(definition + 0x28c))
            shape = *(byte **)(definition + 0x290) + 0x20;
        else
            shape = *(byte **)(definition + 0x298) + 0x30;
        if (function_1c5210(&matrix, shape, NONE, 9))
            result = false;
    }
    return result;
}
