#include "unknown_4cf790.h"
// @flags /O2 /Gr
#include <string.h>
#include "unknown_11c920.h"
#include "unknown_0662e0.h"
#include "unknown_059ad0.h"
#include "globals.h"
#include <xtl.h>
#include <xonline.h>

/* The two vtables at 0x450b44 (slots 0..8 and 9..16) belong to two small
   helper classes sharing one layout; only the slots decompiled so far have
   bodies, the others are placeholders. */

struct s_helper_game
{
	byte unknown00[0x18];
	long l18;
	byte unknown1c[0x1118 - 0x1c];
	long l1118;
	byte unknown111c[0x4994 - 0x111c];
	long l4994;
	byte unknown4998[0x741c - 0x4998];
	long mode;
};

struct s_helper_source
{
	long type;
	byte unknown04[0x2c];
	s_helper_game *game;
	long unknown34;
	c_class_58d20 *other_game;
};

struct s_helper_output
{
	long l0;
	long l4;
	long l8;
	long lc;
	long l10;
	long l14;
	long l18;
	long l1c;
	long l20;
	long l24;
	long l28;
};


struct s_online_match_session_info
{
	XNKEY key;
	XNKID session_id;
	XNADDR address;
	s_helper_output output;
};

class c_helper_a
{
public:
	virtual void v0(long a, long b);
	virtual void v1();
	virtual void v2();
	virtual void v3(s_helper_output *out);
	virtual long v4();
	virtual long v5(long size, void *data) { return 0; }
	virtual long v6();
	virtual bool v7();
	virtual void v8(long a);

	s_helper_source *source;
	bool b8;
	bool b9;
	bool ba;
	byte unknownb;
	long lc;
	long l10;
	long l14;
	long l18;
	long l1c;
	long l20;
	bool listening;
	byte unknown25[3];
	long listen_time;
	long l2c;
	bool b30;
	byte unknown31[3];
	s_online_match_session_info info;
};

class c_helper_b
{
public:
	virtual void v0(long a, long b) {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3(s_helper_output *out);
	virtual long v4();
	virtual long v5(long size, void *data);
	virtual long v6();
	virtual bool v7();
	s_helper_source *source;
	bool b8;
	bool b9;
};

// @retail 0x72db0
void c_helper_a::v0(long a, long b)
{
	source = (s_helper_source *)a;
	ba = 0;
	lc = NONE;
	l14 = NONE;
	l1c = NONE;
	l10 = 0;
	l18 = 0;
	l20 = 0;
	l2c = b;
	b30 = 0;
	b9 = 0;
	b8 = 1;
}

// @retail 0x73750
void c_helper_a::v8(long a)
{
	source = (s_helper_source *)a;
	ba = 0;
	lc = NONE;
	l10 = 0;
	l14 = NONE;
	l18 = 0;
	l1c = NONE;
	l20 = 0;
	l2c = 3;
	b30 = 0;
	b9 = 0;
	b8 = 1;
}

// @retail 0x73790
long c_helper_a::v4()
{
	return 60000;
}

// @retail 0x73820
bool c_helper_a::v7()
{
	bool result = false;

	if (b9)
	{
		if (source->type != 0)
		{
			s_helper_game *game = source->game;

			long mode = game->mode;

			if (mode == 5 || mode == 6 || mode == 7 || mode == 8)
			{
				if (game->l18 != 0)
					result = true;
			}
			else
			{
				volatile long local_0 = mode;
			}
		}
	}
	return result;
}

// @retail 0x73860
void c_helper_a::v3(s_helper_output *out)
{
	s_helper_game *game = source->game;

	memset(out, 0, sizeof(s_helper_output));
	out->l0 = out->l4 = out->l8 = out->lc = 0;
	out->l10 = NONE;
	out->l14 = NONE;
	long bits = 16;
	if (game->mode > 2 && game->mode <= 8)
		bits = game->l4994;
	bits -= game->l1118;
	out->l24 = 0;
	out->l1c = NONE;
	out->l20 = NONE;
	out->l18 = bits;
	out->l28 = (1 << bits) - 1;
}

// @retail 0x738d0
long c_helper_a::v6()
{
	long result = g_network_configuration.valueca0;
	long type = source->type;

	if (type == 3 || type == 8)
		result = g_network_configuration.valueca4;
	return result;
}

// @retail 0x73350
long c_helper_b::v4()
{
	return g_network_configuration.valuec94;
}

// @retail 0x73470
long c_helper_b::v6()
{
	return g_network_configuration.valuec9c;
}

void function_6b640(long task_index);

struct s_xnet_registry_entry
{
	bool valid;
	byte unknown1[7];
	XNKID kid;
	XNKEY key;
};



// @retail 0x72df0
void function_72df0(c_helper_a *helper)
{
	if (helper->ba)
	{
		if (helper->l14 != NONE)
		{
			function_6b640(helper->l14);
			helper->l14 = NONE;
		}
		s_xnet_registry_entry *entry = &g_4cf7d4[helper->l2c];
		if (entry->valid)
		{
			XNetUnregisterKey(&entry->kid);
			entry->valid = false;
		}
		helper->ba = false;
	}
}

struct s_online_match_session;
long online_match_session_create(const s_online_match_session *session);

#pragma inline_depth(0)
// @retail 0x72e40
void function_72e40(c_helper_a *helper)
{
	long last = helper->l10;
	if (!last || (long)(g_510548 ? g_51054c : GetTickCount()) - last >= g_network_configuration.valuec90)
	{
		if (helper->lc != NONE)
		{
			function_6b640(helper->lc);
			helper->lc = NONE;
		}
		if (helper->l14 != NONE)
		{
			function_6b640(helper->l14);
			helper->l14 = NONE;
		}
		function_72df0(helper);
		if (!helper->b30)
		{
			memset(&helper->info, 0, sizeof(helper->info));
			helper->v3(&helper->info.output);
			helper->lc = online_match_session_create((const s_online_match_session *)&helper->info);
			if (helper->lc == NONE) memset(&helper->info, 0, sizeof(helper->info));
			helper->l10 = g_510548 ? g_51054c : GetTickCount();
		}
	}
}
#pragma inline_depth(255)

// @retail 0x73310
void c_helper_a::v1()
{
	if (lc != NONE)
	{
		function_6b640(lc);
		lc = NONE;
	}
	if (l14 != NONE)
	{
		function_6b640(l14);
		l14 = NONE;
	}
	b8 = false;
}

bool online_match_session_get_info(long task_index, s_online_match_session_info *info);
bool transport_security_register_key(long index, long local, bool host, const XNKID *kid, const XNKEY *key);

struct s_72f00
{
	unsigned __int64 field_0 : 64;
};

// @retail 0x72f00
void function_72f00(c_helper_a *helper)
{
	if (helper->lc != NONE && online_match_session_get_info(helper->lc, &helper->info))
	{
		const s_long_pair *id = (const s_long_pair *)&helper->info.session_id;
		if (((const s_72f00 *)id)->field_0 != 0 && *(long *)&helper->info.key != 0)
		{
			helper->ba = true;
			helper->b30 = true;
			if (!transport_security_register_key(helper->l2c, 0, false, &helper->info.session_id, &helper->info.key))
				helper->ba = false;
		}
		if (helper->lc != NONE)
		{
			function_6b640(helper->lc);
			helper->lc = NONE;
		}
	}
}

// @retail 0x737b0
void function_737b0(c_helper_a *helper)
{
	if (helper->v7())
	{
		c_class_58d20 *game = (c_class_58d20 *)helper->source->game;
		const s_long_pair *previous = NULL;
		if (game->state > 2 && game->state <= 8 && game->flag4998)
			previous = &game->data4999;
		const s_long_pair *current = NULL;
		if (helper->ba)
			current = (const s_long_pair *)&helper->info.session_id;
		if (current != previous && (!current || !previous || memcmp(current, previous, sizeof(*current)) != 0))
			game->set_data_4999(current);
	}
}

// @retail 0x746b0
long function_746b0(long current, long previous, long first, long second)
{
	long result = 0;
	long difference = current - previous + 127;
	short *scale = NULL;
	short *offset;
	byte *weight;
	if (first != NONE && (second == NONE || first < second))
	{
		scale = g_network_configuration.value378;
		offset = (short *)g_network_configuration.valueb74;
		weight = (byte *)g_network_configuration.value9f4;
	}
	else if (second != NONE && (first == NONE || second < first))
	{
		scale = g_network_configuration.value576;
		offset = (short *)g_network_configuration.valuea74;
		weight = (byte *)g_network_configuration.value974;
	}
	if (scale)
		result = scale[difference] * weight[current] / 100 + offset[current];
	return result;
}

struct s_surface_description
{
	long type;
	long field_4;
	dword field_8;
	char names[9][0x10];
	char descriptions[9][0x80];
	char field_51c[128];
	byte unknown59c[0x5a4 - 0x59c];
	long points[16];
	long field_5e4;
	bool flag_5e8;
	byte unknown5e9[3];
	long width;
	long height;
	long depth;
	long field_5f8;
	long field_5fc;
	long field_600;
	bool flag_604;
	byte unknown605[3];
	long field_608;
	long field_60c;
	byte unknown610[4];
};
struct s_game_variant_globals
{
	dword unknown0;
	dword flags;
	word count;
	word state;
};
extern s_surface_description g_551ae8[16];
extern s_game_variant_globals g_47d8f4;
bool function_1934f0(s_surface_description *variant);

// @retail 0x75790
bool __stdcall function_75790(dword identifier, long *index)
{
	bool result = false;
	for (long i = 0; i < 16 && !result; i++)
	{
		s_surface_description *variant = NULL;
		if (i >= 0 && i < 16 && g_47d8f4.count && (g_47d8f4.flags & 2) &&
			function_1934f0(&g_551ae8[i]))
			variant = &g_551ae8[i];
		if (variant && variant->field_8 == identifier)
		{
			*index = i;
			result = true;
		}
	}
	return result;
}

struct s_surface_description;
s_surface_description *function_192e60(long index);
long function_193300(s_surface_description *variant);
long network_session_get_maximum_players(c_class_58d20 *session);

// Matches in the full image since 0x64d70, which also calls 0x5b180, moved out of src/unknown_054fe0.cpp into
// src/unknown_064d70.cpp; this body is unchanged. Before the move the full image called 0x5b180 ahead of 0x193300 and
// kept the variant in EBP (33, 138 bytes against 134) while the reduced image matched. The move alone does it: a full
// check with 0x64d70's old body and flags in the new file already matched this function.
// @retail 0x73360
bool c_helper_b::v7()
{
	bool result = false;
	if (b9 && source->type == 6)
	{
		c_class_58d20 *game = (c_class_58d20 *)source->game;
		if (game->function_058d20() && game->type == 11)
		{
			c_class_58d20 *other = source->other_game;
			if (other->function_058d20())
			{
				long index = other->get_value_49c8();
				s_surface_description *variant = function_192e60(index);
				if (index != NONE && variant && network_session_get_maximum_players(other) <= function_193300(variant))
					result = true;
			}
		}
	}
	return result;
}

struct s_match_rating_entry
{
	bool active;
	signed char rank;
	short unknown02;
	short skill;
	short unknown06;
	long value;
	byte unknown0c[12];
};
struct s_match_rating_collection
{
	byte unknown00[0xdc4];
	s_match_rating_entry entries[16];
};

#pragma optimize("g", off)
static __forceinline long function_74721(const s_match_rating_entry *arg_0, long *arg_1, long *arg_2)
{
	long local_0 = arg_0->skill;
	long local_2 = arg_0->value;
	long local_1 = arg_0->rank;
	*arg_1 = local_0;
	*arg_2 = local_2;
	return local_1;
}
#pragma optimize("g", on)

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x74720
long __stdcall function_74720(const s_match_rating_collection *collection, long selected)
{
	const s_match_rating_entry *current = &collection->entries[selected];
	struct s_74720
	{
		long field_0;
		long field_4;
		long field_8;
	} local_0;
	long count = 0;
	long total = 0;
	local_0.field_4 = function_74721(current, &local_0.field_0, &local_0.field_8);
	for (long i = 0; i < 16; i++)
	{
		const s_match_rating_entry *entry = &collection->entries[i];
		if (i != selected)
		{
			long other_skill = entry->skill;
			if (entry->active && other_skill != NONE)
			{
				long local_1 = entry->rank;
				_ReadWriteBarrier();
				long local_2 = *(volatile long *)&local_0.field_0;
				long local_3 = *(volatile long *)&local_0.field_4;
				total += function_746b0(local_2, other_skill, local_3, local_1);
				count++;
			}
		}
	}
	if (count > 0)
		total /= count;
	if (total + local_0.field_8 < 0)
		total = -local_0.field_8;
	if (total + local_0.field_8 > 0x3fffffff)
		total = 0x3fffffff - local_0.field_8;
	return total < g_network_configuration.valuec7c ? g_network_configuration.valuec7c :
		(total > g_network_configuration.valuec78 ? g_network_configuration.valuec78 : total);
}
#pragma function(_ReadWriteBarrier)

struct s_match_player_rating_entry
{
	bool active;
	byte unknown01[9];
	byte flags;
	byte unknown0b[0x8c - 0x0b];
	signed char team;
	byte unknown8d[7];
	long key;
	short skill;
	short unknown9a;
	long value;
	signed char rank;
	byte unknowna1[3];
};
/* per-player and per-team counters reported with a rating: three fifteen-bit counts */
struct s_rating_counters_player
{
	word first : 15;
	word : 1;
	byte unknown02[4];
	word second : 15;
	word : 1;
	word third : 15;
	word : 1;
	byte unknown0a[0x36a - 0xa];
};
struct s_rating_counters_team
{
	word first : 15;
	word : 1;
	byte unknown02[4];
	word second : 15;
	word : 1;
	word third : 15;
	word : 1;
	byte unknown0a[0x5a - 0xa];
};
struct s_match_player_rating_collection
{
	byte unknown00[4];
	long key;
	byte unknown08[0x12b - 8];
	bool team_mode;
	byte unknown12c[0x384 - 0x12c];
	s_match_player_rating_entry players[16];
	s_match_rating_entry teams[16];
	s_rating_counters_player player_counters[16];
	byte unknown45e4[0x49e4 - 0x45e4];
	s_rating_counters_team team_counters[16];
};

/* the rating change for a player: the average of what each active opponent is worth, kept within the
   configured bounds and within what the player's value can take */
// @retail 0x74800
long __stdcall function_74800(const s_match_player_rating_collection *collection, long selected)
{
	long key = collection->key;
	const s_match_player_rating_entry *current = &collection->players[selected];
	long count = 0;
	long skill = current->skill;
	long team = current->team;
	long value = current->value;
	bool team_mode = collection->team_mode;
	long total = 0;
	long rank;
	if (team_mode)
		rank = collection->teams[team].rank;
	else
		rank = current->rank;
	for (long i = 0; i < 16; i++)
	{
		const s_match_player_rating_entry *entry = &collection->players[i];
		if (entry->key == key)
		{
			long other_skill = entry->skill;
			if (entry->active && entry->team != NONE && !(entry->flags & 3) && other_skill != NONE)
			{
				long other_team = entry->team;
				long other_rank;
				if (team_mode)
					other_rank = collection->teams[other_team].rank;
				else
					other_rank = entry->rank;
				if (i != selected && other_team != team)
				{
					total += function_746b0(skill, other_skill, rank, other_rank);
					count++;
				}
			}
		}
	}
	if (count > 0)
		total /= count;
	if (value + total < 0)
		total = -value;
	if (value + total > 0x3fffffff)
		total = 0x3fffffff - value;
	return total < g_network_configuration.valuec7c ? g_network_configuration.valuec7c :
		(total > g_network_configuration.valuec78 ? g_network_configuration.valuec78 : total);
}

extern long g_510518;
long g_510514;
long g_510530;

// @retail 0x73dc0
void function_73dc0(long value)
{
	if (g_510518)
	{
		if (g_510530 != NONE)
		{
			function_6b640(g_510530);
			g_510530 = NONE;
		}
		g_510518 = 0;
		g_510514 = value;
	}
}

extern s_session_id g_510540;

extern long g_467214;
long online_task_get_logon_status(long task_index);

// @retail 0x73ae0
bool function_73ae0(unsigned __int64 *created_id)
{
	bool result = false;
	if (g_467214 != NONE)
	{
		switch (online_task_get_logon_status(g_467214))
		{
		case 1:
			if (SUCCEEDED(XOnlineArbitrationCreateRoundID(created_id)))
				result = true;
			break;
		}
	}
	return result;
}

// @retail 0x73b10
long __stdcall function_73b10(long a, long b)
{
	const long *a_reference = &a;
	long result = 0;
	if (g_510540.a == *a_reference && g_510540.b == b)
	{
		switch (g_510518)
		{
		case 1: result = 1; break;
		case 2: result = 2; break;
		case 3: result = 3; break;
		case 0:
			switch (g_510514)
			{
			case 0: result = 4; break;
			case 1: result = 5; break;
			case 2: result = 6; break;
			case 3: result = 7; break;
			case 4: result = 8; break;
			}
			break;
		}
	}
	return result;
}

extern bool g_51051c;
extern bool g_51051d;
extern s_session_id g_510520;
extern s_session_id g_510528;
long g_510534;
long g_510538;
long online_round_register(bool free_for_all, XNKID const *session_id, ULONGLONG const *round_key, long seconds);

// @retail 0x73bc0
void function_73bc0(const s_session_id *session_id, const s_session_id *round_key,
	__int64 arg_2, bool free_for_all)
{
	if (g_510518)
	{
		if (g_510530 != NONE)
		{
			function_6b640(g_510530);
			g_510530 = NONE;
		}
		g_510518 = 0;
		g_510514 = 4;
	}
	long task = online_round_register(free_for_all, (const XNKID *)session_id,
		(const ULONGLONG *)round_key, g_network_configuration.value374);
	g_510528 = *session_id;
	memcpy(&g_510520, round_key, sizeof(g_510520));
	g_510540 = *(const s_session_id *)&arg_2;
	g_510514 = 4;
	g_510534 = 0;
	g_510538 = NONE;
	g_51051c = false;
	g_51051d = false;
	g_510530 = task;
	if (task != NONE)
		g_510518 = 1;
	else
	{
		g_510518 = 0;
		g_510514 = 1;
	}
}

struct s_online_match_session;
long online_match_session_update(const s_online_match_session *session);

// @retail 0x73100
void function_73100(c_helper_a *helper)
{
	s_helper_output output;
	bool changed = false;
	helper->v3(&output);
	long last = helper->l18;
	if (!last || output.l28 != helper->info.output.l28 || output.l1c != helper->info.output.l1c || output.l20 != helper->info.output.l20)
		changed = true;
	long now = g_510548 ? g_51054c : GetTickCount();
	if ((now - last >= helper->v4() && memcmp(&helper->info.output, &output, sizeof(output)) != 0) || changed)
	{
		helper->info.output = output;
		helper->l14 = online_match_session_update((const s_online_match_session *)&helper->info);
		helper->l18 = g_510548 ? g_51054c : GetTickCount();
	}
}

long online_task_poll(long task_index);
bool online_task_continue_failed(long task_index);
struct s_search_session;
long online_match_session_delete(const s_search_session *session, bool *unavailable);
long function_75870(void);
long function_75890(long time);

// @retail 0x72f60
void function_72f60(c_helper_a *helper)
{
	if (helper->l1c != NONE)
	{
		bool clear = false;
		switch (online_task_poll(helper->l1c))
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			function_6b640(helper->l1c);
			helper->l1c = NONE;
			clear = true;
			break;
		case 3:
			if (!online_task_continue_failed(helper->l1c))
				clear = true;
			function_6b640(helper->l1c);
			helper->l1c = NONE;
			break;
		case 4:
			function_6b640(helper->l1c);
			helper->l1c = NONE;
			break;
		case 5:
			clear = false;
			helper->l1c = NONE;
			break;
		default:
			helper->l1c = NONE;
			break;
		}
		if (clear)
		{
			memset(&helper->info, 0, sizeof(helper->info));
			helper->b30 = false;
		}
	}
	if (!helper->ba && helper->b30 && helper->l1c == NONE && helper->b9 &&
		(!helper->l20 || function_75890(helper->l20) >= 3000))
	{
		bool unavailable;
		helper->l1c = online_match_session_delete((const s_search_session *)&helper->info, &unavailable);
		if (unavailable)
		{
			memset(&helper->info, 0, sizeof(helper->info));
			helper->b30 = false;
		}
		helper->l20 = function_75870();
	}
}

struct s_search_value
{
	bool valid;
	byte unknown01[3];
	long value;
};

struct s_search_player_values
{
	long state;
	long time;
	s_search_value values[6];
	byte unknown38[0x98 - 0x38];
};

struct s_search_value_group
{
	byte unknown00[4];
	XUID owner;
	byte unknown10[0x1c - 0x10];
	s_search_player_values players[16];
};

struct s_cache_property
{
	long unknown00;
	long type;
	long unknown08;
	long unknown0c;
};
struct s_cache_property_record
{
	byte identity[12];
	long type;
	long count;
	s_cache_property *properties;
};

/* the search value globals (0x509450, 0x70f8 bytes): set by the network
   initialize, cleared by the network dispose */
struct s_search_value_globals
{
	bool initialized;
	byte unknown01[3];
	s_search_value_group groups[5];
	long read_failures;
	long read_state;
	long read_time;
	long read_task;
	long read_record_count;
	s_cache_property_record read_records[80];
	long read_property_count;
	s_cache_property read_properties[480];
	long unknown55a8;
	byte read_buffers[2048];
	long write_state;
	long write_time;
	long write_task;
	long write_record_count;
	s_cache_property_record write_records[32];
	long write_property_count;
	s_cache_property write_properties[128];
	long unknown68c0;
	byte write_buffers[2048];
	byte unknown70c4[0x70f8 - 0x70c4];
};
s_search_value_globals g_509450;
#define g_509454 g_509450.groups
#pragma pack(push, 1)
struct s_session_interface_user
{
	bool valid;
	XUID xuid;
	byte unknown0d[3];
	long unknown10;
	byte properties[0x90];
	long unknowna4;
	long unknowna8[3];
	long unknownb4[3];
	long unknownc0[3];
	byte unknowncc[4];
};
#pragma pack(pop)

struct s_session_interface_globals
{
	bool initialized;
	byte unknown01;
	wchar_t machine_name[16];
	wchar_t session_name[32];
	bool unknown62;
	byte unknown63;
	union
	{
		byte unknown64[32];
		struct
		{
			byte unknown64_00[0x14];
			long unknown78;
		};
	};
	long unknown84;
	long unknown88;
	long unknown8c;
	long unknown90;
	long unknown94;
	long unknown98;
	s_session_interface_user users[4];
	long update3dc[3];
	long unknown3e8[3];
	long value3f4[3];
	byte data400[3][0x130];
	byte unknown790[0x7d4 - 0x790];
	void *session_manager;
};

extern s_session_interface_globals g_4cd868;

// @retail 0x73a10
bool function_73a10(long player, long group_index, long field, long *value)
{
	bool result = false;
	s_search_value_group *group = &g_509454[group_index];
	if (player >= 0 && player < 16 && g_47d8f4.count && (g_47d8f4.flags & 2) &&
		function_1934f0(&g_551ae8[player]))
	{
		s_search_player_values *values = &group->players[player];
		if (values->state == 3)
		{
			XUID xuid;
			if (g_4cd868.users[group_index].valid)
			{
				xuid = g_4cd868.users[group_index].xuid;
				if (memcmp(&group->owner, &xuid, sizeof(xuid)) == 0)
				{
					result = true;
					*value = values->values[field].value;
				}
			}
		}
	}
	return result;
}

const long g_46722c[6][2] =
{
	{0, 0x3fffffff}, {0, 127}, {0, 127},
	{0, 0x7fffffff}, {0, 0x7fffffff}, {0, 0x7fffffff}
};

// @retail 0x74970
long function_74970(long value, long previous)
{
	long level = 0;
	for (long i = 1; i < 128; i++)
	{
		if (value < g_network_configuration.value774[i])
		{
			if (level == previous - 1 && value > (g_network_configuration.value774[level] + g_network_configuration.value774[level + 1]) / 2)
				level = previous;
			return level;
		}
		level++;
	}
	return level;
}

// @retail 0x749b0
void function_749b0(long group, long player, long index, long value)
{
	if (value < g_46722c[index][0])
		value = g_46722c[index][0];
	else if (value > g_46722c[index][1])
		value = g_46722c[index][1];
	if (!g_509454[group].players[player].values[index].valid || value != g_509454[group].players[player].values[index].value)
	{
		g_509454[group].players[player].values[index].value = value;
		g_509454[group].players[player].values[index].valid = true;
	}
}

// @retail 0x74a00
void function_74a00(long group, long player, long index, long amount)
{
	if (g_509454[group].players[player].values[index].valid && amount)
	{
		long value = g_509454[group].players[player].values[index].value + amount;
		if (value < g_46722c[index][0])
			value = g_46722c[index][0];
		else if (value > g_46722c[index][1])
			value = g_46722c[index][1];
		g_509454[group].players[player].values[index].value = value;
	}
}

// @retail 0x74a60
void function_74a60(long group, long player, long index, long value)
{
	if (g_509454[group].players[player].values[index].valid)
	{
		long bounded = g_46722c[index][0];
		if (value >= bounded)
		{
			bounded = g_46722c[index][1];
			if (value <= bounded)
				bounded = value;
		}
		if (bounded > g_509454[group].players[player].values[index].value)
			g_509454[group].players[player].values[index].value = bounded;
	}
}

// @retail 0x739a0
void function_739a0(long group, long player)
{
	for (long i = 0; i < 5; i++)
	{
		if ((group == NONE || group == i) && g_509454[i].players[player].state == 3)
		{
			g_509454[i].players[player].state = 4;
			g_509454[i].players[player].time = g_510548 ? g_51054c : GetTickCount();
		}
	}
}

// @retail 0x731d0
void function_731d0(c_helper_a *helper)
{
	byte data[0x3fc];
	if (helper->listening && !helper->ba)
	{
		if (g_4cf8d4)
			XNetQosListen(&helper->info.session_id, NULL, 0, 0, 16);
		helper->listening = false;
	}
	if (helper->ba)
	{
		if (!helper->listening)
		{
			helper->listen_time = 0;
			if (g_4cf8d4 && XNetQosListen(&helper->info.session_id, NULL, 0, 0, 1) == 0)
			{
				if (g_4cf8d4)
					XNetQosListen(&helper->info.session_id, NULL, 0, 0, 5);
				helper->listening = true;
			}
		}
		if (!helper->listening)
			return;
		long last = helper->listen_time;
		if (!last || (long)(g_510548 ? g_51054c : GetTickCount()) - last >= g_network_configuration.valuec98)
		{
			long size = helper->v5(sizeof(data), data);
			if (g_4cf8d4)
				XNetQosListen(&helper->info.session_id, size > 0 ? data : NULL, size, 0, 5);
			long rate = helper->v6();
			if (g_4cf8d4)
				XNetQosListen(&helper->info.session_id, NULL, 0, rate, 9);
			helper->listen_time = g_510548 ? g_51054c : GetTickCount();
		}
	}
}

bool online_task_get_finished(long task_index, bool *succeeded);

class c_helper_update : public c_helper_a
{
public:
	virtual void v2();
};

struct s_query_batch
{
	long attribute_count;
	long unused04;
	XONLINE_STAT attributes[224];
	long operation_count;
	long unused_e0c;
	XONLINE_STAT_PROC operations[1];
};

static inline void query_stat_set(XONLINE_STAT *stat, word id, long value)
{
	stat->wID = id;
	stat->type = XONLINE_STAT_LONG;
	stat->lValue = value;
}

static inline void query_conditional_set(XONLINE_STAT_PROC *procedure, const XUID *xuid,
	dword leaderboard, byte comparison, const XONLINE_STAT *stat)
{
	procedure->wProcedureID = XONLINE_STAT_PROCID_CONDITIONAL;
	procedure->Conditional.xuid = *xuid;
	procedure->Conditional.dwLeaderBoardID = leaderboard;
	procedure->Conditional.bComparisonType = comparison;
	procedure->Conditional.StatToCompare = *stat;
}

static inline void query_update_set(XONLINE_STAT_PROC *procedure, word procedure_id, const XUID *xuid,
	dword leaderboard, dword conditional_index, dword stat_count, XONLINE_STAT *stats)
{
	procedure->wProcedureID = procedure_id;
	procedure->Update.dwConditionalIndex = conditional_index;
	procedure->Update.dwLeaderBoardID = leaderboard;
	procedure->Update.xuid = *xuid;
	procedure->Update.dwNumStats = stat_count;
	procedure->Update.pStats = stats;
}

// @retail 0x73e00
void function_73e00(long lower, s_query_batch *batch, const XUID *xuid, long value1,
	long value4, long value5, long value6, long upper, long count, const long *leaderboards)
{
	long attribute_count = batch->attribute_count;
	long operation_count = batch->operation_count;
	XONLINE_STAT *first = &batch->attributes[attribute_count];
	first[0].wID = 0xfffe;
	first[0].type = XONLINE_STAT_LONGLONG;
	first[0].llValue = value1;
	query_stat_set(&first[1], 1, value1);
	query_stat_set(&first[2], 4, value4);
	query_stat_set(&first[3], 5, value5);
	query_stat_set(&first[4], 6, value6);
	query_stat_set(&first[5], 2, lower);
	query_stat_set(&first[6], 3, lower);
	attribute_count += 7;
	XONLINE_STAT *second = &batch->attributes[attribute_count];
	second[0].wID = 0xfffe;
	second[0].type = XONLINE_STAT_LONGLONG;
	second[0].llValue = value1;
	query_stat_set(&second[1], 1, value1);
	query_stat_set(&second[2], 4, value4);
	query_stat_set(&second[3], 5, value5);
	query_stat_set(&second[4], 6, value6);
	attribute_count += 5;
	XONLINE_STAT *upper_first = &batch->attributes[attribute_count];
	query_stat_set(upper_first, 2, upper);
	attribute_count++;
	XONLINE_STAT *upper_second = &batch->attributes[attribute_count];
	query_stat_set(upper_second, 3, upper);
	attribute_count++;
	for (long i = 0; i < count; i++)
	{
		long leaderboard = leaderboards[i];
		if (leaderboard != NONE)
		{
			// an update names its conditional by the conditional's one-based index
			XONLINE_STAT_PROC *procedure = &batch->operations[operation_count];
			long first_reference = ++operation_count;
			query_conditional_set(procedure, xuid, leaderboard, XONLINE_STAT_COMPTYPE_NOT_EXISTS, first);
			procedure = &batch->operations[operation_count];
			long second_reference = ++operation_count;
			query_conditional_set(procedure, xuid, leaderboard, XONLINE_STAT_COMPTYPE_EXISTS, upper_first);
			procedure = &batch->operations[operation_count];
			long third_reference = ++operation_count;
			query_conditional_set(procedure, xuid, leaderboard, XONLINE_STAT_COMPTYPE_LESS, upper_second);
			procedure = &batch->operations[operation_count++];
			query_update_set(procedure, XONLINE_STAT_PROCID_UPDATE_REPLACE, xuid, leaderboard, first_reference, 7, first);
			procedure = &batch->operations[operation_count++];
			query_update_set(procedure, XONLINE_STAT_PROCID_UPDATE_REPLACE, xuid, leaderboard, second_reference, 1, upper_first);
			procedure = &batch->operations[operation_count++];
			query_update_set(procedure, XONLINE_STAT_PROCID_UPDATE_REPLACE, xuid, leaderboard, third_reference, 1, upper_second);
			procedure = &batch->operations[operation_count++];
			query_update_set(procedure, XONLINE_STAT_PROCID_UPDATE_INCREMENT, xuid, leaderboard, second_reference, 5, second);
		}
	}
	batch->operation_count = operation_count;
	batch->attribute_count = attribute_count;
}

// @retail 0x737a0
void c_helper_update::v2()
{
	c_helper_a::v2();
	function_737b0(this);
}

#pragma inline_depth(0)
// @retail 0x73040
void c_helper_a::v2()
{
	bool connected = false;
	if (g_467214 != NONE)
	{
		switch (online_task_get_logon_status(g_467214))
		{
		case 1: connected = true; break;
		}
	}
	b9 = connected;
	bool active = v7();
	if (!ba && lc == NONE && active)
		function_72e40(this);
	function_72f00(this);
	if (l14 != NONE)
	{
		bool succeeded;
		if (online_task_get_finished(l14, &succeeded))
		{
			if (!succeeded) function_72df0(this);
			if (l14 != NONE)
			{
				function_6b640(l14);
				l14 = NONE;
			}
		}
	}
	function_72f60(this);
	if (ba)
	{
		if (!active) function_72df0(this);
		if (ba && l14 == NONE) function_73100(this);
	}
	function_731d0(this);
}
#pragma inline_depth(255)

#include "language.h"
#include <time.h>
struct s_matchmaking_ratings;
struct s_member_quality_collection;
bool function_7e210(c_class_58d20 *session, s_matchmaking_ratings *ratings);
void function_7e100(long current, const s_member_quality_collection *collection,
	long *selected, long *first, long *second, long *level);
long function_1931a0(long count, s_surface_description *variant);

#pragma inline_depth(0)
// MiniMax B (11-05): no new form, no lower score (6); the elapsed-store forms (a named local for the difference, the start read inlined, a named local for the clock read) match on the reduced image and stay at 6 in the full build; all six differences follow from 0x7e210's full-image convention (ours reads the session from the stack, retail passes it in EAX), which also orders the elapsed store after the stack cleanup and shifts the later calls; 0x7e210 has three call sites and sits in src/unknown_07a9a0.cpp, which is not this lane's file.
// Matches retail (coordinator 11-16, lane 5 r188's lead): with 0x7e210's group-highest scan through 0x631f0 the session
// reaches 0x7e210 in EAX. Earlier 6 (was 6): BLOCKED-BY 0x7e210: everything else matches. Still differed: the
// elapsed store is ordered after the stack cleanup and the 0x7e210 call pushes the
// session (retail passes it in EAX); the one-byte-shorter push shifts the 0x7e100 call
// field and leaves one more pad byte. Earlier: the group-identity copy matches retail's
// bytes; the summary stores loaded their ratings in another order.
// Earlier transport, summary-load and copy-tail forms had fast control 32;
// native rating record, native role names, native capacity expression, native
// minimum observations, array minimum observations, capacity expression,
// inline rating declaration and inline quality declaration 32/32/60/32/32/60/32/32.
// Native output-record graph; maximum/population/both snapshots; record with maximum/population/both; native address assignment: 32 each (fast control 32). Original output graph and all calls restored.
// Further forms (fast control 17): ordinary summary/elapsed/both stores 19/17/19;
// elapsed accumulator/named expression 17/17; ratings at entry/before time 17/17;
// complete range-state observations 17. Original body and inherited qualifications restored.
// Round 114: BLOCKED-BY 0x7e210 for the session push and the elapsed store (a register-session stand-in
// for 0x7e210 gives retail's order there, fast 17 -> 10). On that stand-in: plain summary stores 19;
// minimum after/before the capacity 10/10; named capacity 56; player-count store later 23; minimum at
// each use 10; capacity ternary 54. The group/add masks still take EDX/ECX (retail EAX/EDX).
// Round 47 (m3 lane 1, fast control 17): elapsed store without the volatile cast 17; the six summary
// stores plain in the old order (0x78, 0x68, 0x60, 0x7c, 0x80, 0x6c) 19; 32 plain store orders 8-19,
// best available, maximum, minimum, average, add mask, group mask 8 (kept: plain 15 -> 6, no volatile
// left; the summary block and both masks then match); on it, minimum read after the capacity 8.
// Round 129 (lane 5, fast): diagnostic only, 0x7e210 compiled with no inlining (an inline-depth pragma
// around it): this function matches (0x7e210 then takes the session in EAX, retail's head) while 0x7e210
// goes 766 -> 1593. Ours inlines 0x192e60 into 0x7e210 and calls 0x193300 from an inlined callee; retail
// calls 0x192e60 and never 0x193300. The body here is complete; the fix is in 0x7e210's inlining.
// Round 146 (m3 lane 5, fast control 8, plain 6): session on the stack comes from 0x7e210 giving the session EBP and
// the inlined getter's pointer EDI. In 0x7e210: the output pointer's address taken 8, read through its address 8; the
// getter in the player loop, the capacity helper force-inline, a long histogram and their combinations 8 each. Diagnostic
// (not kept): the top getter call inside `if (1)` or a constant-true local test stays out of line, this function matches,
// 0x7e210 942 (2700 bytes); the call in both arms of the state test matches too, 0x7e210 915, with the capacity
// helper force-inline 856 (2828 bytes). Diagnostic in a scratch copy of 0x192e60's file (not kept): the getter's
// element pointer computed first gives the session EAX while still inlined, this function matches, but 0x192e60 31,
// 0x63220 102, 0x73ca0 148. More source in the getter with the same bytes, or `inline` on it, changes nothing.
// Round 155 (m3 lane 6, fast control 8 with 0x7e210 766): the elapsed time with the start field read after the clock
// call 78 (528 bytes); the 0x7e210 result named before its test 8. In 0x7e210 (this function / 0x7e210): the capacity
// read before the summary 8/785; the variant looked up on the accessor with the index read again after 8/867 (2844
// bytes); the result flag declared after the admission test 8/766. Retail's 0x7e210 calls 0x192e60 at its top and
// has 0x192e60's body inline in the player loop (one 0x1934f0 call there), so the two sites in one caller differ.
// Round 96 (m3 lane 16, fast control 8 / plain 6, still BLOCKED-BY 0x7e210): in 0x7e210, the player loop's written-out
// variant lookup replaced by the existing 0x192e60 call 8 (0x7e210 766 -> 857, 2772 bytes: the loop call stays out of
// line, the top one inlines); its result flag declared first 8 (766), declared last and set after the clear 8 (766).
// Here: the ratings buffer declared at the top 8; an early return when 0x7e210 fails 8. All restored.
// Round 226 (m3 lane 3, fast, still BLOCKED-BY 0x7e210): 0x7e210's player loop written as 0x63220's body with its calls
// (0x192e60, 0x631f0, a two-evaluation max of 0x1931a0) gives this function its match (the top 0x192e60 call stays out of line
// and the session arrives in EAX) but raises 0x7e210 766 -> 1032; with its capacity helper forceinline 8 here, 998 there.
// Round 109 (m3 lane 16, fast control 8 / plain 6, still BLOCKED-BY 0x7e210): the matched caller 0x6fa60's ratings buffer without
// its `__declspec(align(8))` 8 (0x7e210 766; aligned caller locals are not what keeps the session on the stack); in 0x7e210, the
// write-only machine-id copy dropped 8 (0x7e210 937, 2700 bytes: retail has the copy); the capacity test through the existing
// 0x193300 (our link already folds this file's twin into it) 8 (766, same code), with a forceinline redeclaration of it 8 (766);
// that call with the top lookup after the capacity read 1 (942, 2732 bytes, as the private-helper form). All restored.
// Round 76 (m3 lane 2, fast control 8 / plain 6; trials in 0x7e210, all restored): the output cleared first 8 (0x7e210 815);
// the top lookup made inside the `||` validity test 1 (0x7e210 954, 2700 bytes; the call there stays out of line and 0x7e210
// takes the session in ECX, retail EAX; not kept: assignment inside the test, a rise in 0x7e210, retail calls it first); the
// loop's lookup through 0x192e60 at the loop body's top level (every pass) MATCH here (0x7e210 930, 2716 bytes: the hotter loop
// site takes the one inline copy and the top call stays out of line, as retail; not kept: it looks the variant up on every pass
// and raises 0x7e210); the variant index by an explicit state test 8; the validity test split in two 8. Diagnostic (compiler
// setting, not kept): 0x7e210's file without /arch:SSE 8 (0x7e210 836; the top call still inlines).
// M3 lane 10 r100 (fast with 0x7e210, 0x63220, 0x192e60 and nine of its callers; control 6 here, 0x7e210 766 at retail's 2828
// bytes with the top 0x192e60 inlined and the capacity helper called, retail the reverse; still BLOCKED-BY 0x7e210). In
// 0x7e210: the player loop's written-out rating adjustment replaced by a call to the existing 0x63220 (retail's loop is that
// body inlined) MATCH here, 0x7e210 1097 at 2524 bytes (the 0x63220 call stays out of line in the loop; not kept: a rise in
// 0x7e210; diff saved as build/r100/lead-0x73530-loop-call.patch), the result through a named local the same; that with the
// capacity helper forceinline 8 here, 0x7e210 1033 at 2592 bytes (the top call inlines again); the capacity helper forceinline
// alone 8, 0x7e210 906 at 2872 bytes (both inlined). In 0x192e60 (does a heavier spelling keep the top call out of line?): the
// validity test written out 6 here (0x192e60 20, 0x7e210 906 at 2764 bytes, the top call still inlined), through the
// out-of-line 0x192db0 6 (0x192e60 and 0x7e210 unchanged), early returns or one ternary 6 (0x192e60 35 at 60 bytes, 0x63220
// 112, 0x73ca0 140), an element pointer local 6 (0x192e60 20). The top call inlines in every spelling.
// Round 197 (m3 lane 4, fast control 8 / plain 6; trials in 0x7e210, all restored): 0x7e210's result flag declared first 8
// (0x7e210 766, same code); the top lookup taking the getter's value directly with the loop reading the variant index from the
// session getter again 8 (0x7e210 856, 2844 bytes); the opening test with `!variant` first 8 (766, same code); the type-5 scan's
// exit as `return false` 8 (766, same code); the loop's written-out variant lookup replaced by the existing 0x192e60 called
// inside `if (variant_index)` MATCH here (0x7e210 931 at 2732 bytes: the top lookup still inlines, but its pointer takes EBX and
// the output pointer EBP, so the session takes EDI as in retail and arrives in EAX; not kept: a rise in 0x7e210; diff saved as
// build/r197/lead-0x73530-loop-lookup.patch), plus `long histogram[16]` (retail clears 16 dwords) MATCH, 0x7e210 933 (2704
// bytes). So the session's register needs no change to the inline choices, only to the first pass's register pressure.
// Diagnostic (compiler setting, reverted at once): no inlining around only the top lookup statement MATCH here, 0x7e210 942.
// M3 lane 14 r97 (fast with 0x7e210, 0x192e60, 0x63220; control 8 / 766; trials in 0x7e210, all restored): the loop's variant
// through the existing 0x192e60 after the rating reads plus `long histogram[16]` MATCH here, 0x7e210 918 (2736 B, frame 0xb4,
// session in EAX, but the top 0x192e60 still inlines, the loop call and the capacity switch stay calls); that with the
// capacity switch `__forceinline` 8 (0x7e210 868, 2788 B); that with the capacity case labels grouped 8 (888, 2768 B); the
// player value through the existing 0x63220 plus the long histogram MATCH, 0x7e210 1141 (2476 B); the 0x192e60 call as the
// loop body's first statement plus the long histogram MATCH, 0x7e210 937 (2704 B, frame 0xb0, top still inlined). Still
// BLOCKED-BY 0x7e210: every form that gives the session EAX raises 0x7e210.
// Round 123 (m3 lane 16, fast control 8 / plain 6): five new 0x7e210 forms (its note, round 123: the conditional balance
// clear, the single-exit form with it, with the player value through 0x63220, with grouped capacity labels, with a long
// histogram) leave this at 8 each; the top 0x192e60 call inlines in all of them. Still BLOCKED-BY 0x7e210.
// Round 188 (m3 lane 5, fast control 8 / plain 6, 0x7e210 766; trials in 0x7e210, all restored): its machine-id copy declared
// in the group loop 8 (766); its group-highest scan through the existing 0x631f0 MATCH here (0x7e210 964 at 2684 bytes: the
// session arrives in EAX with nothing else changed; not kept by the lane: a rise there; build/r188/lead-0x73530-631f0-call.patch; applied by the coordinator 11-16: MATCH in the full check); that
// with `long histogram[16]` MATCH (984, frame 0xb4); that with the loop's variant through 0x192e60 MATCH (1011); the group
// named before the written-out scan 8 (766). Still BLOCKED-BY 0x7e210.
// @retail 0x73530
void function_73530(c_helper_a *helper, void *description)
{
	c_class_58d20 *session = helper->source->other_game;
	long member = *(long *)((byte *)session + 0x40);
	long variant_index = NONE;
	if (session->state > 2 && session->state <= 8)
		variant_index = *(long *)((byte *)session + 0x49c8);
	s_surface_description *variant = function_192e60(variant_index);
	byte *output = (byte *)description;
	memset(output, 0, 0x14c);
	*(word *)(output + 2) = 0;
	*(long *)(output + 8) = 0x2651;
	*(long *)(output + 0xc) = 0x2651;
	*(word *)output = 2;
	*(long *)(output + 4) = 4;
	long language = g_47ff38;
	if (language == NONE)
	{
		language = function_11ca80(XGetLanguage());
		g_47ff38 = language;
	}
	if (session->state > 2 && session->state <= 8)
		language = *(long *)((byte *)session + 0x4988);
	*(long *)(output + 0x10) = language;
	XNKEY *key = (XNKEY *)(output + 0x1c);
	s_session_id *id = (s_session_id *)(output + 0x14);
	if (session->state && session->flag24)
	{
		if (id) *id = *(s_session_id *)((byte *)session + 0x1c);
		if (key) *key = *(XNKEY *)((byte *)session + 0x25);
	}
	memcpy(output + 0x2c, (byte *)session + 0x58 + member * 0x10c, sizeof(XNADDR));
	*(long *)(output + 0x70) = variant_index;
	long started = session->time78b4;
	*(long *)(output + 0x74) = time(NULL) - started;
	long ratings[0x364 / 4];
	if (function_7e210(session, (s_matchmaking_ratings *)ratings))
	{
		function_7e100(member, (const s_member_quality_collection *)&session->value4c,
			NULL, (long *)(output + 0x50), (long *)(output + 0x54), NULL);
		*(long *)(output + 0x58) = ratings[3];
		long available = 16;
		long minimum = ratings[2];
		if (session->state > 2 && session->state <= 8)
			available = *(long *)((byte *)session + 0x4994);
		available -= ratings[3];
		*(long *)(output + 0x60) = available;
		*(long *)(output + 0x7c) = ratings[1];
		*(long *)(output + 0x80) = minimum;
		*(long *)(output + 0x78) = ratings[0];
		*(long *)(output + 0x68) = ratings[0x35c / 4];
		*(long *)(output + 0x6c) = ratings[0x360 / 4];
		if (minimum != NONE)
			*(long *)(output + 0x84) = minimum > function_1931a0(ratings[1], variant) ? minimum : function_1931a0(ratings[1], variant);
		else
			*(long *)(output + 0x84) = NONE;
		*(long *)(output + 0x64) = available;
		*(long *)(output + 0x88) = 0;
		if (*(long *)variant == 5)
		{
			long group_count = ratings[0x1d4 / 4];
			*(long *)(output + 0x88) = group_count;
			if (group_count > 0)
			{
				struct s_group_identity { dword words[3]; };
				for (long i = 0; i < group_count; i++)
					((s_group_identity *)(output + 0x8c))[i] =
						((const s_group_identity *)&ratings[0x298 / 4])[i];
			}
		}
	}
}
#pragma inline_depth(255)

#include "bitstream.h"
struct s_session_description_payload;
void function_7db10(s_bitstream *stream, const s_session_description_payload *message);

// @retail 0x733f0
void c_helper_b::v3(s_helper_output *out)
{
	long description[0x14c / 4];
	function_73530((c_helper_a *)this, description);
	memset(out, 0, sizeof(*out));
	out->lc = 0;
	out->l4 = description[0x60 / 4];
	out->l8 = description[0x5c / 4];
	out->l0 = description[0x58 / 4];
	out->l10 = description[0x70 / 4];
	out->l28 = description[0x68 / 4];
	out->l18 = description[0x64 / 4];
	out->l14 = description[0x78 / 4];
	out->l20 = description[0x7c / 4];
	out->l1c = description[0x84 / 4];
	out->l24 = description[0x74 / 4];
}

// @retail 0x73480
long c_helper_b::v5(long size, void *data)
{
	long description[0x14c / 4];
	s_bitstream stream;
	stream.data = (byte *)data;
	stream.size_in_bytes = size;
	long result = 0;
	function_73530((c_helper_a *)this, description);
	stream.unknown08 = 1;
	stream.mode = 1;
	memset(data, 0, size);
	stream.bit_position = 0;
	stream.checkpoint_count = 0;
	stream.error = false;
	stream.unknown2c = 0;
	stream.unknown30 = 0;
	function_7db10(&stream, (const s_session_description_payload *)description);
	if (stream.bit_position <= (stream.size_in_bytes << 3))
		result = (stream.bit_position + 7) / 8;
	return result;
}

struct s_address_table;
long function_199250(s_address_table *table, byte const *address);
long function_199290(byte *results);
char *__stdcall function_1537a0(byte const *address);

struct s_rating_summary
{
    ULONGLONG session;
    long values[14];
    char identity[64];
    char name[64];
};

#pragma inline_depth(0)
// @retail 0x741f0
void __stdcall function_741f0(s_match_player_rating_collection *collection, s_query_batch *batch)
{
    long value, change, level, lower;
    long key = collection->key;
    s_surface_description *variant = function_192e60(key);
    batch->attribute_count = 0;
    batch->operation_count = 0;
    long players[3] = { variant->field_8, variant->field_8 + 32, variant->field_8 + 64 };
    if (variant->type != 5)
    {
        for (long i = 0; i < 16; ++i)
        {
            const s_match_player_rating_entry *player = &collection->players[i];
            if (!player->active || player->team == NONE || (*(const dword *)((const byte *)player + 0xa) & 3)) continue;
            long skill = player->unknown9a;
            value = player->value;
            if (*(const dword *)((const byte *)player + 0xa) != 0xbad00000 &&
                player->key == key && skill != NONE && value != NONE)
            {
                change = function_74800(collection, i);
                value += change;
                level = function_74970(value, skill);
                lower = value >= 0 ? function_74970(value, 0) : 0;
                long third = collection->player_counters[i].third;
                long first = collection->player_counters[i].first;
                long second = collection->player_counters[i].second;
                for (long group = 0; group < 5; group++)
                {
                    if (g_509454[group].players[key].state == 3 &&
                        memcmp((const byte *)&g_509454[group] + 4, (const byte *)player + 2, 12) == 0)
                    {
                        function_74a00(group, key, 0, change);
                        function_749b0(group, key, 1, level);
                        if (first) function_74a00(group, key, 3, first);
                        if (second) function_74a00(group, key, 4, second);
                        if (third) function_74a00(group, key, 5, third);
                        function_74a60(group, key, 2, level);
                        function_739a0(group, key);
                    }
                }
                s_rating_summary summary = { 0 };
                summary.values[0] = function_199290((byte *)collection);
                summary.values[1] = function_199250((s_address_table *)collection, g_4cf7cc);
                summary.values[2] = key;
                summary.session = *(const ULONGLONG *)((byte *)collection + 0x130);
                const XUID *identity = (const XUID *)((const byte *)player + 2);
                const char *identity_string = function_1537a0((const byte *)identity);
                strncpy(summary.identity, identity_string, 64);
                summary.identity[63] = 0;
                const word *source = (const word *)((const byte *)player + 0x10);
                char *destination = summary.name;
                long remaining = 64;
                word character;
                do
                {
                    character = *source;
                    if (remaining == 1) *destination = 0;
                    else if (character <= 127) *destination = (char)character;
                    else *destination = '?';
                    source++;
                    destination++;
                    remaining--;
                } while (character && remaining > 0);
                summary.values[3] = *(const short *)((const byte *)player + 0xa2);
                summary.values[4] = player->rank;
                summary.values[5] = value;
                summary.values[6] = change;
                summary.values[7] = level;
                summary.values[8] = first;
                summary.values[9] = second;
                summary.values[10] = third;
                summary.values[11] = player->team;
                summary.values[12] = collection->teams[player->team].unknown02;
                summary.values[13] = collection->teams[player->team].rank;
                function_73e00(lower, batch, identity, change, first, second, third, level, 3, players);
            }
        }
    }
    else
    {
        for (long i = 0; ; ++i)
        {
            long count = NONE;
            switch (variant->type)
            {
            case 1: break;
            case 2: break;
            case 3: count = variant->width; break;
            case 4: count = variant->width; break;
            case 5: count = variant->width; break;
            default: __assume(0);
            }
            if (i >= count)
                break;
            if (collection->teams[i].active && memcmp(collection->teams[i].unknown0c, g_440070, 12) != 0)
            {
                const s_match_rating_entry *team = &collection->teams[i];
                if (team->skill != NONE)
                {
                    value = team->value;
                    if (value != NONE)
                    {
                        short skill = team->skill;
                        change = function_74720((const s_match_rating_collection *)collection, i);
                        long adjusted = value + change;
                        level = function_74970(adjusted, skill);
                        lower = adjusted >= 0 ? function_74970(adjusted, 0) : 0;
                        const s_rating_counters_team *stats = &collection->team_counters[i];
                        function_73e00(lower, batch, (const XUID *)team->unknown0c, change,
                            stats->first, stats->second, stats->third, level, 3, players);
                    }
                }
            }
        }
    }
    function_739a0(NONE, key);
}
#pragma inline_depth(255)

void function_80940(long record_capacity, s_cache_property_record *records,
    long property_capacity, s_cache_property *properties, long buffer_capacity,
    byte *buffers, long *record_count, long *property_count, long *buffer_count);
void function_80bf0(long record_capacity, s_cache_property_record *records,
    long property_capacity, s_cache_property *properties, byte *buffers,
    long *record_count, long *property_count, long *buffer_count);
void function_80aa0(s_cache_property_record *records, long count);
void function_80d70(s_cache_property_record *records, long count);
long online_stats_read(word count, XONLINE_STAT_SPEC *specs);
bool online_stats_read_result(long task_index, XONLINE_STAT_SPEC *specs, word count, byte *extra_buffer, word extra_size);
long online_stats_write(XONLINE_STAT_SPEC const *specs, word count);
bool online_stats_write_succeeded(long task_index);
long online_task_poll(long task_index);

static __forceinline long query_time_now();

static inline long query_time_now()
{
    return g_510548 ? g_51054c : GetTickCount();
}
static __forceinline void query_restore_pending()
{
    for (long group = 0; group < 5; group++)
        for (long player = 0; player < 16; player++)
        {
            long *state = &g_509454[group].players[player].state;
            if (*state == 2) *state = 1;
            else if (*state == 5) *state = 4;
        }
}

bool function_1900a5(long index);

// score 633 (was 643): The aligned 0xc0 frame and retail's 2862 bytes agree. The logon compare, properties-based user cursor, slot selection and request-loop scratch homes still differ.
// Earlier at 663 (was 703): the same differences at 2878 bytes.
// Earlier at 703 (was 703). The 0xc0 frame agrees. The logon branch, user cursor and slot selection still differ.
// The 0xc0 frame agrees. The status branch, validity-based user cursor and slot selection still differ.
// Earlier measurement 703 (was 703): The 0xc0 frame agrees. The logon branch, user cursor and slot selection still differ.
// Earlier at 703: the frame agreed. The logon branch, user cursor and slot selection differed.
// MiniMax B (11-19): no new form, no lower score (703): retail's users loop takes the user record itself as its cursor
// (the valid flag at +0, the identity at +1, the slot count at +0x10 and the 0x90 property bytes at +0x14) and keeps it
// at +0x18, while ours takes the property bytes as the cursor and keeps it at +0x14, so the flag, the identity and the
// count are all read 0x14, 0x13 and 4 bytes before the cursor. Retail's logon test is a dec against 1 where ours is a
// cmp, and retail sets the two bases and jumps to the loop head while ours runs the loop in place.
// MiniMax G (11-19): no new form, no lower score (703), body unchanged. Binding the user record in a local so that the
// cursor is the record base (the valid flag at +0, the XUID at +1, the unknown10 at +0x10 and the properties at +0x14, as
// retail has them) instead of the XUID pointer with negative displacements scores 777 (the compiler then keeps the base in
// EBX and the frame grows), so it is not kept. The first difference is still at the top: retail tests the logon status with
// `dec eax` (the status minus one, then a jump) where we compare against the immediate 1, and retail keeps the users base at
// [esp+0x18] where we keep it at [esp+0x14]; the users loop's own body (the valid test, the XUID copy, the properties copy
// and the slot test) matches retail's shape apart from those homes.
// Earlier at 663 (was 703). The 0xc0 frame agrees. The status branch, validity-based user cursor and slot selection still differ.
// Earlier detail: The 0xc0 frame agrees. The status branch, validity-based user cursor and slot selection still differ.
// Previous 703 (was 703): m3 lane 16 r120: no new form measured. The listed blockers stand: the slot test's cmovne (one
// instruction shorter than our je/mov, so every positional line after the first users loop is off by one) needs the
// SSE architecture flag this file does not set, and retail treats calls into 0x1934f0's tree as writing memory (the
// variant count re-read per pass, the loop counted down in EBX). Both are build-flag questions, not source forms.
// M3 lane 11 r113 (fast with 0x641a0/0x8df50/0x749b0/0x75790/0x192e60/0x647a0/0x647d0/0x18fd94, all MATCH; control 703):
// lane 16's lead re-measured (`inline` on 0x647d0's definition, the users loop reading the slot index and properties
// through it) 720 at 2862 B; on it the request builder's free space named (record_space / property_space, tested > 0 and
// passed to 0x80940) 710, the delay as `> 3 ? 300000 : 2000` 720, both 710, the space names with the rolled record loop
// (`j < record->count`, `properties_used += record->count`) 753 (2830 B), with the delay too 753. Left on the 710 form: the
// logon test's `dec eax`, the users cursor's slot (+0x14 against +0x18) and loop-head jump, and the slot test's cmovne
// (a file-flag question) that shifts every later line.
// MiniMax I (11-15): no new form, no lower score (703): /arch:SSE2 on this file's flags line does reach retail's cmovne at
// the slot test (mov ecx, 0; cmovne ecx, eax), but the function scores 748 and the switch is file-wide, so it would change
// the file's other matched functions; /arch:SSE gives the same 748 and dropping the 'g'-off pragma around the slot helper
// changes nothing.
// MiniMax R (11-16): no new form, no lower score (703): the frame agrees at 0xc0, but the slot helper's null test is still a branch where retail uses a conditional move, and the users cursor still rides in EBX where retail keeps it in a stack slot at +0x18.
// Earlier 703 (was 703): earlier 709; 2910 bytes. Frame 0xc0 agrees; initial cursor base/home, slot and later homes remain.
// MiniMax S (11-15): no new form, no lower score (703).
// Earlier 703 (was 703), merged from a second score line: earlier 709; 2910 bytes. Frame 0xc0 agrees; initial cursor base/home, slot and later homes remain.
// MiniMax G (11-12): no new form, no lower score (703); the frame agrees (0xc0); the slot helper is the
// static __forceinline function_74aa1 in this file (src/unknown_072db0.cpp:1588), whose null test is a
// branch here against retail's cmovne, and the users cursor still rides in EBX where retail keeps it in
// a stack slot at +0x18 that points at the record start.
// MiniMax B (11-11): no new form, no lower score (703); the body is unchanged, and the forms listed above are exhausted.
// MiniMax G (11-11, second pass): no new form, no lower score (703); retail keeps the users cursor in a stack slot
// at +0x18 that points at the record start, ours keeps it at +0x14 pointing at the record's properties, and the
// slot helper's null test is a branch here against retail's cmovne; the loop written as a pointer walk over the
// users and the value groups puts the cursor in a register and turns the bound into a counter (784), the slot
// lookup moved ahead of the property copy (847), and the slot helper's null test stays a branch when its "g"-off
// pragma is dropped (703, 2910 bytes) all leave more differences than they remove, so the body stands.
// MiniMax G (11-11): no new form, no lower score (703); the users cursor still rides in EBX where retail keeps it
// in a stack slot at +0x18, and the logon-status test still lacks retail's decrement and branch; this round
// re-read the diff and the statement walk the earlier rounds recorded agrees with the source on every block,
// condition, callee and constant, so what is left is those two homes and the body stands. Reaching the record
// through a named record pointer moves the cursor's anchor from the properties (+0x14) to the unknown10 (+0x10) and
// spelling the copy through the array moves it the same way; the anchor is the largest field offset of the group
// the fields share, so retail's record-start cursor is not spellable from this file (lane 16 r102 reached it only
// by inlining the getter 0x647d0 in src/unknown_054fe0.cpp). Both forms stay at 703.
// MiniMax H (11-11): no new form, no lower score (703); the users cursor still rides in EBX where retail keeps it in a stack slot at +0x18, and the logon-status test still lacks retail's decrement/branch.
// MiniMax B (11-13): no new form, no lower score (703); the users cursor still sits at +0x14 where retail keeps it at +0x18, the member fields are read off the properties address where retail reads them off the element, and the logon-status test still lacks retail's decrement and branch (the element through a named pointer 777, the copy before the slot lookup 847, the status written as (status - 1 == 0) 823; all worse).
// MiniMax E (11-05): no new form, no lower score.
// MiniMax H (11-04): the logon-status test written as (status - 1 == 0) reaches retail's
// `dec eax` and its exact 2862 bytes but scores 729; not kept, the original body is restored.
// Earlier (703, was 703): 709 before that; 2910 bytes. Frame 0xc0 agrees; initial cursor base/home, slot and later homes remain.
// w14D (fast, callers 0x641a0/0x8df50 and the file's matched functions stay MATCH): walked statement by
// statement against retail: no logic difference found (fields, conditions, callees, arguments, loop and
// poll shapes all agree). Retail inlines the existing getter 0x192e60 (pointer range check at 0x74be0,
// 0x74dba, 0x750de, then `test esi, esi`) and the existing 0x749b0 (clamp + store, 0x751c6-0x7520d; it is
// not in the calls column), so the file's written-out query_variant_available() and clamp-and-store are
// now calls of those: getter at the three sites 768, 0x749b0 alone (group index loop) 730 (pointer
// loop 744), both 703. Not kept: a `record_properties` local for the six property initializers rolls
// the loop as retail (2814 bytes, aligned hunks 143 -> 103) but grows the frame to 0xc8 (771).
// Identity as a local XUID (type-5 arm copies it, the scan compares against it) 752 (2926 bytes).
// Not tried: a call of function_75870() in place of query_time_now(): retail inlines the clock at every
// use, but the fast image cannot show cross-file inlining.
// Earlier: Frame 0xc0 agrees; initial cursor base/home, slot and request/completion homes remain.
// frame 0xc0 agrees; native completion-group cursor retained;
// initial cursor base/home, slot expansion and request/completion homes still differ.
// Earlier: Frame 0xc0 agrees; first user/identity homes and protected request-builder inputs remain.
// Frame 0xc0 agrees; initial user/identity homes, logon dispatch and request/completion allocation remain.
// Frame 0xc0 agrees; user cursor spills and identity uses EBX; cursor base, dispatch and completion homes remain.
// Frame 0xc0 agrees; user/identity homes, completion cursors and request-builder argument registers remain.
// Earlier: frame 0xc0 agrees; user/identity and request homes remain.
// Frame 0xc0 agrees; user cursor, identity word and request homes remain.
// user-cursor and identity-word homes differ;
// long validity 770; pending read guard first 786; timestamp before
// counts 770; unsigned property initializer 770; unsigned user group 781.
// Frame 0xc0 agrees; user cursor and identity homes remain. Const user 770; const identity 770; byte group 781; byte accepted count 821; byte descriptor initializer 853 (fast).
// User/identity and request homes remain; direct identity-helper
// return 791; duplicate cursor equality 773; byte team/failure results 770/770;
// incomplete-property arm first 785 (fast). Opening body and helpers retained.
// Frame 0xc0 agrees; user/identity and request homes remain. Terminal record reuse 787; native identifier field 770; clamp-value lifetime 770; type-first duplicate filter 794; request-player cursor 804 (fast).
// Frame 0xc0 agrees; user/identity and request homes remain; native identity flags 770; repeated flag snapshot 770; identity after property copy 772; property value after lookup 786; received-record cursor 770 (fast).
// Existing identity snapshot as copy source 770; cache comparison order 797; request header first 771;
// word request-player index 809; one-based descriptor IDs 770 (fast). Original body retained.
// All five-group word ordinals 741 (frame 0xc8; rejected); byte clear ordinals 770;
// read-submission failure first 770; common delay default 769; clamp bounds 769;
// request/completion/offline word group 775/754/775; completion-only 754 plain, frame 0xc0.
// direct records 754; direct properties 754.
// direct destinations 772; record/property accesses 754.
// record/destination accesses 772; property/destination accesses 772.
// all three accesses 772; accumulated failure 739.
// Native duplicate index 761; request/completion/both direct player fields 794/754/794;
// signed/unsigned word completion-player ordinals 755/755; unsigned word completion group
// 754; duplicate index with unsigned word group 761. Original body retained.
// Admission-test result 739; eager failure OR 753; positive completion 739;
// player-result-first OR 739; accumulated signed/unsigned failure 761/761;
// signed positive completion 759; admission plus positive completion 739.
// Round 71: retail inlines the pointer-returning variant getter (index range as a pointer range, then a NULL
// test, then variant->type) and keeps the six-property loop rolled with a register bound shared by the
// count store and properties_used += 6. Getter at the first site 755 (2894 bytes); at all three 766 (2910);
// getter + `j < record->count` bound and `properties_used += record->count` 759 (2830, aligned lines 560 -> 484);
// the same with the sum before the loop 774; the bound alone 839 (2766).
// Round 137: SSE flag 755 (file gate failed);
// inline read/result/write-result and combinations 739,739,739,739,739,739,739; outlined joint 739.
// Native player-array end bounds in availability/request/completion 837/792/792; availability-request/availability-completion/request-completion/all 878/905/857/951; all fixed clear array ends 771. Original ordinal traversals retained.
// Read-builder count homes in six meaningful lexical orders, write-builder homes and both builders: 739 each on the retained word-completion/failure graph. Original count names, helpers, predicates and call tuples restored.
// Round 343: Native request identity via library/aggregate copy 747/747; with real descriptor count 792/792;
// terminal candidate reuse 742/742; count plus terminal reuse 836/836. All restored; original completion failure and word group remain.
// Initial-pass signed-word group/availability/both/clears/group-clears/availability-clears/all
// 752/761/764/737/752/761/764; all initial-pass unsigned-word indices 764.
// Round 248: Complete returned-property extent and received total, signed/unsigned
// with indexed access and before/after accumulation 737/754 each; native property
// cursor with the same pairs 737/760 each. Original bound and accumulation restored.
// Native admission/returned-value/both validity bytes and signed admission byte 737 each;
// admission byte with immutable identity, byte admission/team/both results 737 each.
// Round 219: Initial user/value/slot uninitialized declarations at function scope, masks 1-7 and
// all with role names: 737/737/737/737/737/737/737/737. Original body restored.
// Round 167: pending-restoration ordinary inline/ordinary/outlined 930 each;
// direct state fields forced/inline/ordinary/outlined 737/930/930/930; direct
// fields with native short restoration ordinals 753. Original forced helper restored.
// Actual read-builder output declarations at entry, all seven nonempty subsets,
// then both read/write output triplets at entry: 737 each. All zero assignments,
// observations, predicates, copies, clocks and call tuples stay at their original points.
// Round 107: retail tests each task poll as `if (state < 0 || state > 1) { if (state == 2) ... else ... }`
// (jl to the failure arm, jle out, jne to failure); that shape for the write task / read task / both
// 738/737/738 (retail's block order there, aligned count 359 -> 353, positional unchanged); retail's
// cmovne in the slot helper implies /arch:SSE (file is /O2 /Gr); identity read through a named xuid
// pointer also used by the later copy 737 (identical); identity copied with memcpy 737 (identical).
// Round 29: Clock/availability inline-inline, inline-forced, inline-outlined, forced-forced, forced-outlined, outlined-inline, outlined-forced, outlined-outlined: 737/737/810/737/810/949/949/980. Original forced clock and inline availability restored.
// Round 221: initial user/cache-group/both/slot/user-slot/cache-slot/all/all-readonly-identity
// observations 728/739/770/737/728/739/770/770. Direct user observations kept, plain 728.
// On that receiver graph, native char/byte admission with mutable/readonly identity: 728 each;
// original admission field and identity retained. All call roles and state transitions preserved.
// Round 387: Existing availability producer with single-exit bool/byte/short/long result and
// combined/nested admission: 728/728/728/728/719/719/719/719. Native long result retained, plain
// 719 at 2878/2862; all file matches preserved. Cursor base, slot expansion and request/completion
// homes remain.
// Round 390: existing native properties object with separate identity assignment,
// producer-scoped team result and readonly slot masks 0-7: 719 each. Original
// buffer, declarations, helper settings and complete body restored.
// Round 392: distinct user/request/completion induction identities, all seven subsets,
// then nested reset/descriptor role names: 719 each. Original identifiers and body restored.
// Round 224: Native cached owner/property/both fields 728 each; owner/property/both with
// test-produced admission 728 each; both with comparison-local team and with both result lifetimes
// 728/728. Original cache views, admission/team lifetimes, predicates and task transitions restored.
// Round 269: Slot/identity/properties, slot/properties/identity, identity/slot/properties
// and properties/slot/identity initializations 767/767/759/741; readonly slot
// forms give the same scores. Original order and full body restored; cursor base still differs.
// Native user/request/completion group-array endpoints in subsets 1/2/4/3/5/6/7
// 781/729/709/748/790/731/745; all equality endpoints 745. Completion-only kept, plain 709.
// Round 184: Native byte/short/long/dword zero-one returns under inline/forced declarations
// 709/709/730/730/709/709/709/709. Original bool return, long internal result, complete
// predicate and helper boundary restored.
// Round 414: actual request records/properties/admission domains short-long-long, long-short-long,
// short-short-long, dword-long-long, long-dword-long, dword-dword-long, word-word-long and
// short-short-dword 764/722/765/709/709/709/765/765. Original totals and complete body restored.
// Round 217: completion mapping/received/clamped scalar role names, seven subsets, 709 each;
// mapped output at completion-player scope 709. Initial cursor base/home and slot expansion
// remain; original complete body restored.
// Request descriptor type once in long/dword 709/709; key once in long/dword
// 725/725; both in signed/unsigned and mixed native storage 725 each. Original reads and
// whole body restored; key forms recover length but enlarge the aligned frame.
// Round 45: twice-used initial-request comparison as bool/byte/char/long, mutable
// and immutable, 737 each; frame grows to 0xc8. Original complete body restored.
// Round 215: identity-change clear cursor 709; invalid-user clear cursor 709; disconnected clear
// cursor 709; both online clear cursors 709; identity-change/disconnected clear cursors 709;
// invalid-user/disconnected clear cursors 709; all three clear cursors 709; all three clear
// cursors with word remaining counts 709.
// Round 103 (m3 lane 7; count is positional, so an aligned diff of the listing was tracked too: baseline 515):
// the existing online_logon_connected() for the opening test 729 (2862 bytes = retail, retail's dec eax; aligned 479);
// with the existing variant getter 0x192e60 in the first loop 763 (aligned 470), in all three loops 802 (2910);
// the existing session-user getters 0x647a0/0x647d0 in the first loop 781 (not inlined, plain check too);
// the existing clock helpers 0x75870/0x75890 932 (not inlined); on the getter form, delay as `> 3 ? 300000 : 2000`
// 763 (retail's setle, aligned 468), with the last read time in a named local 749 (aligned 514). Leads: the slot
// test's cmovne needs /arch:SSE (a file-flag decision), which shifts every later instruction in the positional
// count; retail evaluates the last read time before the clock read and keeps it in ESI (elapsed-time shape).
// Round 47 (m3 lane 1, fast): the user's flags read as identity.dwUserFlags in place of the +8 dword casts
// 709 (byte-identical; kept); the slot as the plain s_player_slot -> s_player_slot_flags conversion without
// the optimize("g", off) wrapper 767 (2894 bytes: the conversion's NULL test is retail's cmovne test of the
// adjusted pointer, so it is the natural source under /arch:SSE, a file-flag lead); a user record pointer
// for the users loop 754 (the loop then gets retail's jump over the padding and xuid-relative loads); with
// online_logon_connected() too 775 (2846 bytes); delay as `> 3 ? 300000 : 2000` 709 (aligned 515 -> 513).
// Round N274: Library property copy before identity capture 711; readonly identity 711;
// producer-scoped team result 711; both 711; those four with entry property workspace 711 each.
// Original identity/property order, native buffer lifetime and complete body restored.
// Round 53 (m3 lane 1, fast; aligned diff of the listing: 308 unaligned lines at 709): the existing
// online_logon_connected() with the getter 0x192e60 in the users loop 763 (the getter inlines as
// retail's pointer-range check, but our build hoists the variant count into EBX where retail reloads it
// and counts the loop down in EBX); plus a user record pointer 755 (2862 bytes; cursor then in EBX at
// the xuid); the slot computed before the properties copy 763; the record loop in retail's shape
// (state == 1 as a request flag set again by the retry test, the getter, the identity copied into a
// local XUID, an index scan only for type 5) 794 at 2942 bytes (aligned 289); the request flag alone
// 730 (no flag byte appears; retail's `xor dl, dl` comes with the other parts).
// Round 290: Indexed property destination from stored pointer/address producer, crossed with native
// record-count bound and advancement, 780/788/796/770/826/788/812/770 (2750-2814 bytes). Original
// initializer restored; frame 0xc0, initial cursor/slot/availability and later request homes remain.
// Round N274: Library property copy before identity capture 711; readonly identity 711;
// producer-scoped team result 711; both 711; those four with entry property workspace 711 each.
// Original identity/property order, native buffer lifetime and complete body restored.
// Round 57 (m3 lane 1, fast): on online_logon_connected() (729, aligned 479): the delay ternary
// 729 (aligned 477); a user record pointer 775 (2846 bytes, cursor xuid-based in EBX); pointer +
// ternary 775; pointer + identity memcpy from it 775; pointer + property copy first 774.
// Round 234: Native user/request-descriptor/both/completed-record/user-completed/descriptor-completed/all values 828/718/824/741/845/741/837; all with descriptor assignment 837. All regress. User frame 0x190 and descriptor frame 0x6d8 versus retail 0xc0. Original full body restored; initial cursor/slot and later request/completion homes remain.
// N303: Actual cache-group/request-player/completion-player element references, seven subsets,
// then complete returned-record/property/destination elements: 709 each. Original receiver
// forms restored; frame agrees, initial user cursor base/home and slot expansion remain.
// Round 523: Initial properties/identity/cache-publication memmove transfers, seven subsets
// then all with properties before identity, 776/767/734/785/804/753/771/771. Cache-only and
// identity/cache reach retail length; properties memmove loses self-alignment. Original
// copies, input observations, cursor forms, helpers and complete body restored.
// P263: equality-only library operands reversed consistently in cache/duplicate/completion
// subsets 1/2/4/3/5/6/7, then all with team comparison: 713/709/709/713/713/709/713/715.
// Original comparisons and body restored; initial cursor/slot and later homes remain.
// M3 lane 4 r155 (fast; LCS-aligned base 344): retail's user loop passes the record pointer in ECX and the xuid pointer
// (record + 1) through EDX/EAX like an inline helper's arguments. New: the identity copied through a const xuid pointer (also
// the later copy's source) with online_logon_connected() 775 (2846 bytes, aligned 362); the user's valid flag typed byte or
// char, team computed before `valid = true`, the availability loop through a player element pointer: 709 each (same code).
// Re-measured: a record pointer with the xuid pointer 754 (aligned 364, twice: at the body top / beside the values pointer),
// online_logon_connected() 729 (2862 bytes, aligned 343), with the record pointer 775. The slot helper's cmovne still needs
// /arch:SSE (a file-flag decision).
// M319: Slot index/native row/byte view/flags-address production before the
// property copy, admission helper kept after it: after identity 709/709/709/761;
// before identity 709/709/709/767. One-use producers require a match; all
// restored. User cursor base/home, slot conversion and later homes remain.
// M321: Returned identity/property copy destinations, mutable/immutable receivers, then either overlap-safe transfer
// 709/709/709/709/709/709/768/756. NOT kept: local output aliases require a match. Original body restored; initial cursor/slot and later homes remain.
// M332: Native due-timestamp captures at read/write/both submission boundaries, before/inside
// admission, then both complete due predicates: 710/715/716/710/715/716/716/716. Original global reads,
// clocks, predicates and body restored; initial cursor/slot and later homes remain.
// Round 141 (m3 lane 6, fast; aligned diff 515): the `valid` flag dropped for a clear loop in each failing arm 751
// (2910 bytes, aligned 521); an early `continue` after clearing for an invalid user 783 (aligned 524); the type-5
// record scan by index 768 (2926, aligned 577); the request identity copied into a local XUID 733 (2910, aligned
// 489); both 812 (2942, aligned 584). The slot helper's cmovne still needs /arch:SSE, so the positional count
// cannot reach a match under the file's flags.
// M339: Complete duplicate-search condition under for/while, ordered/equality endpoint and identity/type
// filter order: 709/709/698/698/746/746/721/721. NOT kept: type-first 698 grows frame 0xc0 to 0xc8;
// original body restored.
// M3 lane 4 r161 (fast): the existing xuid getter 0x647a0 as the users-loop test 757 (2862 B; called, not
// inlined); a const user reference for the users loop 754; that with online_logon_connected() 775 (2846 B);
// the identity flag tests before the slot test 711; team declared before valid 709 (same code).
// Round 146 (m3 lane 5, fast): a user record pointer defined inside the valid test 754 (aligned 364 against 344), with
// a named xuid pointer also used by the later copy 754, defined before the test 754; the pointer with the identity
// or the later copy read directly 709/709 (same code); online_logon_connected() 729 (2862 bytes, aligned 343). Retail's
// `mov ecx, 0; cmovne ecx, eax` is one line shorter than ours, so every later line shifts: the slot test needs
// /arch:SSE (a file-flag decision), and the positional count cannot fall far before that.
// M366: Shared initial-pass clear/availability/fallback ordinals, pairwise/all lifetimes:
// signed 709/709/709/709, unsigned 729/709/729/729. NOT kept: no exact cursor-home repair;
// original independent ordinals and full body restored; initial cursor/slot and later homes remain.
// M369: Existing availability boundary receives the actual descriptor by pointer/reference,
// ordinary/forced inline and long/bool truth: 709/719/709/719/709/719/709/719.
// NOT kept: all tie or regress; original ordinal helper, all three calls and whole body restored.
// Round 129 (m3 lane 7, fast; aligned/masked control 344/215; a match still needs /arch:SSE for the slot's cmovne, a flag decision): `failed` set in the incomplete arm, without the
// `failed || received != 6` line, as one store after the inner test 703 (2862 B), in each inner arm 728 (2894 B), first in each arm 728, with `received != 6` tested first and the store
// in each arm 702 (2862 B, aligned 344: a gain of 7, under the keep threshold, not kept). On 702: retail's poll order (state store before the restore) 702, the failure counter's test
// swapped 702, poll field order 702, delay ternary reversed 702, the last read time in a named local read ahead of the clock 703, `added < 4` first in the loop test 714 and with that local 715,
// count and properties pointer swapped 730 (2846 B), the write poll as `state < 0 || state > 1` first 703, that with `added < 4` first 715.
// M377: Complete availability/provider truth, direct initialization then test-produced
// true, bool/byte/short/long results: 719/719/719/719/719/719/719/719.
// NOT kept: all regress; original producer, three callers and full body restored.
// M379: Complete identity member production identifier/flags first at producer/group/function
// scope, mutable/readonly native-member aggregate initialization: 744 each. NOT kept: all regress;
// original packed-object snapshot and body restored; cursor/slot and later homes remain.
// M382: Complementary initial admission then team decisions, bool/byte/short/long:
// 709/709/709/709/710/710/779/759. NOT kept: no native cursor/slot repair or byte match; original
// positive decisions, snapshots, helpers, task/builder roles and complete body restored.
// M384: Complete identity/property cache-refresh changed/current result, bool/byte/short/long:
// 709/709/709/709/709/709/709/709. NOT kept: no equality for one-use result residence;
// original full body restored; initial cursor/slot and request/completion homes remain.
// Round 148 (m3 lane 6, fast; aligned counts in brackets, control 709 [515]): retail's shape needs the fields at 0x50c460..
// as one struct (they are contiguous, with a 4-byte member at +0x2598 before the buffers; the array fields escape, and
// retail reloads the record count where plain globals hoist it). Match-only recipe, 685 [427], 2862 bytes: that struct,
// `failed = true` in the incomplete arm, the getter 0x192e60 at all three sites, `long last` for the timestamp, remaining
// record and property space locals, a `values` pointer in the request loop, the identity local only in the type-5 arm with
// the other arm copying directly, a `properties` local with `j < 6` (rolls the loop as retail), `added < 4 && i < 16`,
// one `task` local at every use, and two flags (`due`, `duplicate`), which is why it is not kept. The value form of `due`
// 715; `||` for it 749; plain globals 720. Forms measured on the way: failed arm alone 703 (a gain of 6), struct on that 710,
// request and completion rewrite 705 (2894) [459], one identity in both arms 757 [719], selector helper 745, identity by value
// 698, walking group pointer 742/730, direct record fields 746, store orders 747/747, field-wise identity 717/699/735/750,
// retail's poll order 701 [425], named type 701, else-arm named source 715, break 701, continue 701, uniform direct task 693,
// write-side struct 703. Keepable-only climb from the body below: failed arm 703, then none lower; with retail's poll order 702
// [523], online_logon_connected() 793, delay ternary 703, reversed state test 703, swapped failure counter 703.
// Diagnostic, reverted at once: `word volatile count` 731/733/718 (retail's reload appears, `team` then takes EBX).
// M377: Complete availability/provider truth, direct initialization then test-produced
// true, bool/byte/short/long results: 719/719/719/719/719/719/719/719.
// NOT kept: all regress; original producer, three callers and full body restored.
// M379: Complete identity member production identifier/flags first at producer/group/function
// scope, mutable/readonly native-member aggregate initialization: 744 each. NOT kept: all regress;
// original packed-object snapshot and body restored; cursor/slot and later homes remain.
// M382: Complementary initial admission then team decisions, bool/byte/short/long:
// 709/709/709/709/710/710/779/759. NOT kept: no native cursor/slot repair or byte match; original
// positive decisions, snapshots, helpers, task/builder roles and complete body restored.
// M384: Complete identity/property cache-refresh changed/current result, bool/byte/short/long:
// 709/709/709/709/709/709/709/709. NOT kept: no equality for one-use result residence;
// original full body restored; initial cursor/slot and request/completion homes remain.
// Round 74 (m3 lane 10, fast, callers 0x641a0 and 0x8df50 checked): on the note's 702 form rebuilt (`received != 6` first, `failed = true` in each state arm, 2862 bytes, a gain of 7, not kept): bound limits through a
// `long const *` local 702; `value` as `= 0; if (type)` 703; write poll `state == 0 || state == 1` 718 (2878); `long failed` 719 (2878); properties_used declared first 702; `now` read first 702.
// Round 183 (m3 lane 4, fast; aligned/register-masked difflib counts 330/238 at 703): re-measured function_75870()
// in place of query_time_now() at all eight uses 817 (2750 bytes: eight calls, its /Ob1 file keeps it out of line;
// the "Not tried" line above is stale, round 124's clock helpers 932 were the same form). Lead: in the variant loop
// retail re-reads the count word (`cmp word ptr [0x47d8fc], 0`) after each 0x1934f0 call and counts down in EBX;
// ours hoists the word into BX across that call and ends the loop on the variant pointer. LTCG hoists across a call
// only when the callee summary shows no store, so 0x1934f0 or one of its callees in src/matchmaking_hopper.cpp
// (declared in src/unknown_1932c0.cpp with another pointer type) differs in what it may write. The slot test's
// cmovne still needs /arch:SSE (a file-flag decision), so this function cannot match as the file stands.
// Round 86 (m3 lane 1, fast): retail treats calls into 0x1934f0's tree as writing memory, ours as pure.
// Diagnostic, reverted at once: an opaque library call at the top of 0x1934f0 gives retail's per-pass
// reload of the variant count in the users loop (team then takes BL; the loop still ends on a pointer
// compare) and retail's exact completion inner loop (record properties and count reloaded each pass,
// `received` counted per pass), 810 at 2958 B. Lead: the hopper checks behind 0x1934f0 look opaque to
// retail's link (a build-flag question for src/matchmaking_hopper.cpp, not a source form). Trials: the
// identity picked as a member address (`type == 5 ? values->unknown10 : (byte *)&values->owner`) in both
// loops 735 (2894 B; aligned 302 -> 289, masked 204 -> 194: the completion select is then retail's
// branch-and-lea), completion loop only 720, request loop only 733; the opening test through the existing
// 0x6c7e0 805 (stays a call); the property value written `!type ? 0 : value` 703 (same code).
// M3 lane 10 r86 (fast with 0x641a0, 0x8df50, 0x749b0, 0x75790, 0x192e60 and every matched caller of 0x1934f0, all MATCH; control 703,
// aligned 500, masked 330): in the users loop retail re-reads the variant count (`cmp word [count], 0`) after each call of 0x1934f0
// and counts the loop down in EBX; ours hoists the count into EBX. `inline` on 0x1934f0's definition 703 (same code: its noinline
// declaration wins); the missing-variant arm first 703 (masked 335); a player pointer for the three state accesses 703 (same code);
// the team test first 703 (masked 334); a named group pointer in the request-building loop (also the identity base) 737 (2894 bytes,
// masked 314), with the player pointer 737. The slot helper's cmovne still needs /arch:SSE.
// M3 lane 16 r102 (plain checks of the whole file plus 0x641a0, 0x8df50 and the getter files' functions): function_75870() at every
// clock read 817 (2750 B, called), with function_75890(start) for the two elapsed tests 816 (called; round 103's form, re-measured
// plain); the existing function_6c7e0() for
// the opening test 805 (called). The users loop through the existing getters 0x647a0/0x647d0 with a per-file `__forceinline`
// redeclaration 828 (2928 B, both still called). Lead, not kept: with `inline` on 0x647d0's definition (src/unknown_054fe0.cpp) and the
// slot index + properties read through it, the users loop becomes retail's instruction for instruction (cursor at the record start,
// `lea edx, [ecx + 1]; mov eax, edx` for the xuid), 720 at retail's 2862 bytes, aligned 316 against 330, nothing lost (0x18fd94 stays
// matched); `inline` on 0x647a0 as well gives the same code but loses 0x199c47/0x199c94 (they inline it where retail calls it); both
// getters called with only 0x647d0 inline 749; the 720 form with online_logon_connected() 759 (2878 B, aligned 309).
// In the 720 form the identity-change clear loop gains a jmp + pad (its head lands at another offset mod 16: retail's cmovne is one
// byte shorter than our je/mov), so the positional count runs off from there; the loop-head pads follow byte offsets, not the source.
// M3 lane 1 r90 (fast with the file's functions, 0x641a0, 0x8df50, 0x192e60; aligned/register-masked counts in brackets,
// control 703 [506/328]): the request builder's free space named (`record_space = 80 - records_used`, `property_space =
// 480 - properties_used`, tested `> 0` and passed to 0x80940) 711 [484/313] (2894 B; retail's `mov eax, 0x50; mov ecx,
// 0x1e0; sub; sub; cmp; jle` test); plus `j < record->count` with `properties_used += record->count` 736 [404/277] (2830 B,
// retail's rolled loop with the count in ESI); plus the type-5 identity copied into a local XUID, scanned by index with
// `if (k < records_used) continue;` and the other arm copying the owner directly 739 [514/273] (frame 0xc8); one
// function-scope identity for both loops 739 (same code); the identity assigned in both arms and copied into the record
// once after the join 738 [438/256] (2894 B, frame 0xc0: retail's join, both arms load the identity into EAX/ECX/EDX and
// one store follows); plus online_logon_connected() 778 [434/259]. Each step lowers the masked count but shifts the
// positional one; the slot test's cmovne (one byte shorter) still needs /arch:SSE, a file-flag decision, so the function
// cannot match under the file's flags. All restored.
// Round 171 (m3 lane 5, fast with the file's functions, 0x641a0, 0x8df50; control 703, aligned 330, masked 204): each
// clock read in ours stores the global in both arms of the inlined ternary, retail joins first and stores once; the
// matched 0x75870 is `if (g_510548) return g_51054c; return GetTickCount();`. query_time_now() in that form 712 (2878
// B, aligned 323, masked 202; with a `now` local in an if/else, 0x75890's form, the same code); that plus
// online_logon_connected() (with the online tasks header) 784 (2894 B, 323/201); plus the property loop bounded by
// record->count with `properties_used += record->count` 778 (2798 B, 356/185); plus the free space named and tested `>
// 0` 715 (2878 B, 309/195); all three 779 (2798 B, 294/180, the closest structure measured, but 64 bytes short).
// M3 lane 13 r83 (fast with 0x641a0 and 0x8df50, both MATCH; control 703, fully masked aligned 203 in brackets): the
// completion loop as nested ifs with no `continue` (state 2/5 test, variant arm, missing-variant arm last) 766 (2926 B) [197];
// the completion identity computed once before the record loop 737 (2894 B) [201]; `failed |= received != 6;` 736 (2894 B)
// [205]; the write-stats poll in the read poll's shape (cleanup in both arms, no early return) 791 (2958 B) [207]; the delay
// as one nested ternary 703 (same code). Read against retail again: the
// delay's `setle dl ... add edx, 0x7d0` is the `g_50c460 > 3 ? 300000 : 2000` spelling (listed: 709/702/703, never lower);
// retail re-reads g_47d8f4's count word at every users-loop iteration inside the inlined variant getter and counts that
// loop down in EBX, where ours hoists the count into EBX and ends on the variant pointer; the request loop's state==1 flag
// byte and the identity copy are the listed match-only flag/local forms. The slot test's cmovne still needs /arch:SSE.
// P469: Actual 144-byte property payload char/signed-char/short/word/long/dword/real/double
// element storage: 703/703/703/703/703/703/703/824. NOT kept: first seven leave native
// cursor/home gaps; double drops the aligned frame (0xc4, 2929 bytes). Original full body restored.
// T104: Advancing actual input-user records, counted/actual-end and independent/paired cache
// cursors, mutable/readonly: 777/777/784/784/784/784/785/785. NOT kept: all regress;
// original direct-user expressions, loops and full body restored. Native cursor/slot and later homes remain.
// MiniMax B (11-14): no new form, no lower score (703): the slot test's conditional move is one byte shorter than this
// build's jump and move, so the loop head lands on another offset and the counts after it run off; that conditional move
// needs the SSE architecture flag this file's flags do not set, and the flags stay as they are, so the request builder's
// body stands.
// P488: Complete 144-byte property transfers through copy/copy-backward/uninitialized-copy/reverse-copy, byte
// and char payloads: 827/867/827/884/809/867/827/884. NOT kept: Copy boundaries do not recover a native
// keeper. Original payload, library copies, identity observations, helpers, task transitions and protected
// builder inputs restored.
// P496: Shared read/write builder records/properties/buffers outputs in seven subsets, then all at connected admission:
// 703/703/703/703/703/703/703/703. NOT kept: no exact native storage/producer recovery; initial user cursor/home, slot expansion and later request/completion homes remain.
// Original complete executable body restored.
// Round 83 (m3 lane 2, fast): the team flag's comparison moved after the identity-change block 797 (2926 B); the
// request guard as `properties_used > 0 && records_used > 0` 712 (2894 B); the team comparison's operands swapped
// 705; the value-group pointer declared after the two flags 703 (same code); the change test's two identity
// comparisons swapped 789 (2926 B). The slot test's cmovne still needs /arch:SSE (a file-flag decision).
// M3 lane 11 r105 (fast; control 703): the `#pragma optimize("g", off)` / `("", on)` pair around function_74aa1
// dropped 703 (identical code, kept: a pragma retail does not show); on that: the two disconnected-arm timestamp clears
// chained 703; the write section's three counters declared one per line, buffers first 703; the write request test
// as `properties > 0 && records > 0` 721; the first variant loop's player pointer 703 (re-measure, same code); the
// request delay test as `delay < now - last` 703. Every positional line after the users loop is still off by retail's
// cmovne (a file-flag question, see above).
// Round 112 (m3 lane 10, fast; aligned counts in brackets, control 703 [500]): the request test with the state-4 retry
// ahead of `state == 1` (retail's test order) 747 [483]; one forced-inline clear-states helper for the three 16-player
// clear loops 717 [538] (2862 bytes, but the users loop's variant getter is then called, not expanded: inline budget);
// the read-failure counter as `g_50c460 = failed ? g_50c460 + 1 : 0` 703 (same code); the completion's incomplete-state
// reset as `state == 2 ? 1 : 4` 737 [515]; the completion's variant type read once into a local ahead of the record
// loop (retail keeps it in a stack slot at +0x30) 702 [502]. Retail sets `failed` (the +0x13 byte) in both incomplete
// arms and counts `received` per property pass. All restored; the slot test's cmovne still needs the SSE flag.
// M3 lane 2 r92 (fast; control 703, aligned 330): the users-loop record as a const reference (`s_session_interface_user
// const &user`) for every field 777 (aligned 353: the cursor anchors at the xuid in EBX, retail keeps the record start
// on the stack); the value group as a reference 703 (same code); the identity read through a pointer to the record's xuid
// 703 (same code); through a two-use const reference to it 777; the identity through the existing 0x647a0 (the valid test
// and the copy) 757 (called: its file is /Ob1, so it cannot inline here; retail's `lea edx, [ecx + 1]; mov eax, edx`
// before the three-dword copy is that getter's xuid pointer inlined, a file-flag lead like the cmovne).
// P587: Existing slot-converter direct result, empty/occupied early exits, complete
// occupied/empty branches, occupied/empty conditionals and named pointer presence:
// 782/748/748/748/748/782/782/782. NOT kept: all regress; original converter and complete body restored.
// M3 lane 3 r254 (fast with the file's 41 functions, 0x641a0 and 0x8df50; control 703): 0x1934f0's callees in
// src/matchmaking_hopper.cpp are all matched leaf functions with no stores, so retail's per-pass reload of the variant count
// can only come from how that file was linked (the earlier flag lead). Retail also reads g_50c468 once at its test and
// reuses it after the clock read, and holds no zero register (ours keeps 0 in EDI/EBX for the request tests and the two
// counter stores). Forms: the last request time read into one local before the request test 746 (the load hoists to the
// top); `team` computed before `valid = true` 703 (same code); the two flags declared before the value-group pointer 703;
// the slot/identity test split into nested ifs 703; the failure delay as `delay = 2000; if (g_50c460 > 3) delay = 300000;`
// 757.
// M3 lane 7 r173 (fast, control 703, aligned 500): retail's write poll tests the pending range first (`cmp eax, ebx; jl
// cleanup; cmp eax, 1; jle exit; cmp eax, 2`) and shares one cleanup block entered by a jmp after 0x80d70; as `if (state < 0
// || state > 1) { if (state == 2 && succeeded) ...; cleanup }` 720 (aligned 491), as an early `if (state >= 0 && state <=
// 1) return;` ahead of the state-2 test 720 (same code); the first with the delay as `> 3 ? 300000 : 2000` 717 (aligned
// 489); the existing online_logon_connected() for the opening test with online_tasks.h included 823 (2926 B: the header
// changes more than the test); the write timestamp in a named local used for both reads 727 (aligned 501).
// M3 lane 16 r131 (fast; base 703): the poll delay as one nested conditional expression 703 (same code); the completion state
// update as one `else if` chain 703 (same code); `team` declared where it is computed 703 (same code), with the delay expression
// as well 703; the identity-change copy from the local identity instead of the session record 777. Body unchanged; the slot
// helper's cmovne still needs the SSE flag.
// m3 lane 13 r109 (fast with 0x641a0/0x8df50/0x749b0/0x75790/0x192e60/0x647a0/0x647d0/0x18fd94, all MATCH; control 703):
// the slot helper returning `arg_0 ? arg_0 : NULL` 782 (2894 B: the null test is threaded away, still no cmovne), as
// `if (!arg_0) return NULL; return arg_0;` 748; the read poll's failure test as `state != 0 && state != 1` 753; the delay
// as one condition (`(busy session) || g_50c460 > 3 ? 300000 : 2000`) 757; the state advance `? 2 : 5` as an if/else 703
// (same code). Body unchanged; the slot test's cmovne still needs the SSE flag.
// Typed owner publication 703, also with typed team-key copy 703; combined/independent rejection bits 802/717.
// Property slices by index with literal/recorded count 714/714; pointer slices 754/754.
// NOT kept: the indexed slices reach retail length but grow the frame from 0xc0 to 0xc8.
// M3 lane 16 r136 (fast with 0x641a0/0x8df50/0x749b0/0x75790/0x192e60/0x647a0/0x647d0/0x18fd94, all MATCH; control 703 at
// 2910 B, aligned 500). Two forms retail's listing shows, each 16 bytes shorter: `failed = true` in the incomplete arm in
// place of the `failed || received != 6` line, after the state update 731 (2894 B, aligned 497), ahead of it 734, with
// `received != 6` tested first 737; the request builder's free space as named locals tested `> 0` and passed on 711 (2894 B,
// aligned 477). Together 696 / 698 / 697 (2878 B, aligned 474 / 486 / 470): a gain of 7, one short of the keep bar, not
// kept. On the 696 form: the last read time in a named local 716 (aligned 468), in both the read and the write test 716;
// only the write test 696 (same code); a shared forceinline elapsed-time helper at both tests 705, at the write test 698
// (on the plain body 746); the delay as an if/else chain 696 (same code), as `> 3 ? 300000 : 2000` 693 (retail's setle
// and `add 0x7d0`, but an arm swap, so not a gain); `added < 4` first 709; properties_used declared first 696 (same code);
// `now` read first 717; the space test nested 696 (same code); only the record space named 696; only the property space
// named 705; the request state as an if/else or as `!= 1 ? 5 : 2` 696 (same code); the retry arm's `else if (state == 5)`
// as a plain else 698; the property value as `= 0; if (type)` 700; the write poll as `state == 0 || state == 1` 696 (same
// code); the read submission's two `> 0` tests nested 696 (same code). Checked, not a form: retail's hopper checks behind
// 0x1934f0 take their argument in ESI, ECX and EAX, so they were built with link-time code generation; a non-LTCG flag
// on their file would not explain why retail reloads memory after those calls.
// Disconnected group cursor with indexed players, ordered bound/nonempty loop/count 702/702/718.
// With counted players 702/702/718; ordered/equality group bound with a player end 738/738.
// NOT kept: the one-point gains leave the exit span and width unchanged.
// M3 lane 3 r265 (fast with 0x1900a5; plain over the file, 0x1900a5 and its 18 callers, nothing changed): retail's slot
// test (`mov ecx, 0; cmovne ecx, eax; test byte ptr [ecx + 8], 3`) is the matched guest test 0x1900a5 inlined, the
// same code retail inlines into 0x6d080; calling it in place of the file's private copy of its null-tested accessor
// gives the same code here, 703 (kept: it removes the hand copy; the cmovne still needs the SSE flag, so the
// inlined body compiles to our je/mov). On it: the opening test through a switch on the logon status (retail's
// `dec eax`) 823 (2926 B: the users loop loses its alignment pad); `valid = true` after the team test 703 (same
// code); the identity's guest test through the SDK's guest macro 703 (same code); the identity copied after the
// properties 705.
// M3 lane 16 r138 (fast; control 703, aligned 330 by difflib with addresses masked). Round 136's 696 form re-built (the
// incomplete arm setting `failed = true`, the request builder's free space named) 696 (2878 B, aligned 318), still one
// short of the bar. On it: the users loop reading the slot index and properties through 0x647d0 with `inline` on its
// definition 737 (2842 B, aligned 312; 0x647d0/0x647a0/0x18fd94 stay matched); the request test as retail's flag (`request
// = false`, set by the state-1 test and by the state-4 elapsed test, then tested; retail's `xor dl, dl ... mov dl, al`) 707
// (2894 B, aligned 310); the request identity copied into a local (team key by memcpy, owner by assignment) with an
// indexed duplicate search 839 (2974 B, first difference +0x8); a user record pointer for the users loop 758 (2894 B).
// LEAD (flag form, match-only): the completion identity picked as a member address (`variant->type == 5 ?
// completed_values->unknown10 : (const byte *)&completed_values->owner`) on the 696 form gives 688 at retail's 2862 B
// (aligned 310), with the incomplete arm's `failed = true` or a separate `if (received != 6) failed = true;` (688, same
// code); the direct accumulator forms lose it: `failed |= received != 6` 705 (2878 B), main's `failed || received != 6`
// 714 (2894 B), so it is the flag rule's case and not kept. Alone or in other pairs: the completion select 720, with the
// named space 714, with `failed = true` 706; the request loop's identity select the same way 733, with the completion one
// 735, with the space 723, all four pieces 704 (2862 B).
// Body unchanged; the slot test's cmovne still needs the SSE flag.
// The user-base home and slot conditional move still differ with the 0xc0 frame.
// SDK guest predicate for identity/both masks 703/703. Typed slot base through the
// selector 828, via the record reference 821, direct base reference 806.
// With the SDK identity predicate these score 828/821/806. NOT kept: the typed forms
// add sixteen bytes and retain the opening frame and exit-span mismatch.
// Offline-first arm/return using direct status 703/703, shared availability 810/810, one task index 703/703,
// conditional task status 812/812. The shared and conditional forms change the opening schedule and raise the score.
// M3 lane 2 r108 (fast; control 703, aligned 500 by difflib with addresses masked): retail's request loop branches on
// the type-5 test once, copies the 12-byte key into a stack local (+0x34, the users loop's identity slot) in the
// team arm, scans with an index counter and leaves a found entry straight to the next player (`je` to the step).
// Tried: the key as a by-value XUID local, the scan indexed with `if (k < records_used) continue;` 858 (2990 B, frame
// 0xc8); with the users loop's identity variable reused for it 858 (same frame); the indexed scan with `continue` on
// the pointer key 798 (2942 B, frame 0xc8); the key pointer assigned in the arms of the existing type-5 test (no
// ternary) 737 (2894 B, aligned 472: the select is then retail's branch, a lead once the slot's conditional move is
// settled); that plus the completion loop's key pointer assigned by if/else 737 (2894 B, aligned 450).
// M3 lane 7 r185 (fast with the file's 41 functions, 0x641a0, 0x8df50 and the getters' callers; control 703, aligned 500):
// the logon test through the existing online_logon_connected() alone 823 (2926 B: retail's `dec eax`, but the loop pad
// moves); 0x647a0 and 0x647d0 both `inline` at their definitions with the users loop reading the identity, slot and
// properties through them 720 at 2862 B (aligned 456: the users-loop head is then retail's instruction for instruction
// but for the cursor slot +0x14/+0x18 and the cmovne; 0x199c47 and 0x199c94 lose their matches, 29 and 46, because
// their /O1 file then inlines 0x647a0 where retail calls it); 0x647d0 alone inline with the helper 759 (2878 B, aligned
// 454); both with the helper 759; the missing-variant arm as an early `continue` 703 (aligned 505). Diagnostic, not
// kept (flags are the user's decision): without /Ob1 on the getters' file (no `inline`) the users loop through both
// getters gives the same 720 at 2862 B with 0x199c47, 0x199c94 and the getters' 58 file-mates all still matched in
// the fast check: retail's evidence fits getters that an /O2 caller may inline and /O1 callers keep out of line. The
// cmovne row shift (an /arch:SSE question) still dominates the positional count.
// M3 lane 7 r197 (fast with 0x641a0, 0x8df50, 0x1900a5, 0x192e60, 0x749b0, 0x75790, all MATCH each run; control 703 at
// 2910 B, aligned 330 by difflib with addresses masked, register-masked 204). Only forms without a flag: the write poll's
// pending range as an early return ahead of the state-2 test 720 (aligned 327); the read poll as `if (state < 0 || state
// > 1) { if (state == 2) ... else ... }` 728 (2894 B, aligned 328); the free space named (listed) 711 (2894 B, aligned
// 315); write + read polls 731 (2894 B, aligned 325); write + read + space 717 (2894 B, aligned 310); write + space 714
// (aligned 312); read + space 714 (aligned 313). Each piece moves the body toward retail's (shorter, fewer aligned
// lines) but the positional count rises behind the users loop's conditional-move shift.
// Copied team key base 703, cached team key base 791, cached player array base 776. Copied/cached keys
// 791, copied key/player array 776, cached key/player array 776, all three 776, cached owner and team
// destinations 703. NOT kept: the ties leave the first difference in place and the other forms add
// sixteen bytes. The status branch, user cursor and slot selection still differ.
// M3 lane 12 r153 (fast with 0x641a0, 0x8df50, 0x749b0, 0x75790, 0x192e60, 0x647a0, 0x647d0, 0x18fd94, 0x1900a5, all
// matched; aligned exact / register-masked 475 / 325 at 703): the poll delay as `long delay = g_50c460 <= 3 ? 2000 :
// 300000;` with the busy-session override after it 774 (2926 B); the property loop with a signed `long` index against
// `record->count` 703 (same code); the property read through the record at each use (no property pointer) 703 (same
// code); `if (received != 6) failed = true;` in place of the `failed || received != 6` line 731 (2894 B, 470 / 321); the
// received count as `dword` 703 (same code). The slot test's cmovne still needs the SSE flag (a file-flag decision).
// M3 lane 3 r270 (fast with the file's 41 functions; control 703, aligned 500, masked 327). Build-flag lead, measured as
// this function's one compiler-setting diagnostic and not kept (the flags line is the user's decision): with `/arch:SSE`
// added to this file's flags line, retail's `mov ecx, 0; cmovne ecx, eax` appears at the slot test and every other
// function of the file keeps its baseline result, in the fast check and in a plain check of all 41 plus 0x1900a5 (the
// 39 matched ones stay matched, 0x73360 stays at its baseline 33, 0x1900a5 stays matched), so the flag costs nothing
// in this file; this function then reads 748 (aligned 490, masked 317), the positional count rising because the body
// was shaped without the flag. Source forms (current flags / with the diagnostic): the existing
// online_logon_connected() with its header for the opening test 823 at 2926 B (retail's `dec eax`) / 792;
// `failed = true` in the incomplete arm in place of the `||` 731 (2894 B) / 776; the request record's identity as a
// by-value identity local (copied from the team entry, with an index scan of the earlier records and `continue` on a
// duplicate, for type 5; from the owner otherwise) stored by assignment 858 (2990 B, first +0x8, masked 315) / 895
// (masked 305); both 838 / 858 (masked 299); the completion's identity pointer taken once ahead of the record loop
// 737 (2894 B). Reading of retail's request loop: the identity is held by value in three registers and saved to the
// first loop's identity home only in the type-5 arm, whose scan counts an index kept in memory against the record
// count and leaves for the next player on a duplicate.
// M3 lane 7 r193 (fast with the file's 41 functions, 0x641a0, 0x8df50, 0x1900a5 and 0x192e60; control 703). Retail's
// property fill is a rolled loop through the properties pointer kept in a register (`lea edi, [ecx + 1]; mov word [eax],
// di; mov [eax + 4], edx; add eax, 0x10; inc ecx; cmp ecx, esi`, ESI = 6 shared with the count store and the used-count
// add); ours unrolls it and re-reads `record->properties` before each store. Forms with a `properties` pointer named where
// it is computed and used for the record field and the fill: literal bound 771 (2814 B); bound `record->count` with
// `properties_used += record->count` ahead of the fill 731 (2846 B); the same with `+= 6` ahead of it 731 (2846 B); bound
// `record->count` with `+= 6` after the fill 718 (2862 B: retail's rolled loop, but the frame grows from 0xc0 to 0xc8);
// with `+= record->count` after it 718 (same code). Body unchanged.
// M3 lane 15 r115 (fast with 0x1900a5, 0x641a0, 0x8df50, 0x749b0, 0x75790, 0x192e60, 0x647a0, 0x647d0; control 703, aligned
// 500 by difflib with addresses masked). Retail evidence read this round: in the request loop the identity is three registers at
// the record write in both arms, and only the team arm stores it to the users loop's +0x34 slot for the duplicate scan and
// reloads it after, so the scanned key is a separate local from a by-value identity. Forms: a by-value identity with a separate
// key for the scan (an index scan, `if (k == records_used)` for the write) 839 (2974 B, aligned 518); the same with the pointer
// scan 792 (2942 B, aligned 469); the index form with lane 7's rolled fill through a properties pointer 741 (2894 B, aligned
// 474); the index form with the request test as a flag set from state 1 and the state-4 elapsed test 861 (2990 B, aligned 528);
// the identity selected by value with a ternary and the key copied from it 837 (2974 B, aligned 466). Diagnostics on the hopper
// check 0x193560 (another function's body, restored at once): its rating loop through a char pointer, and its result through a
// reference to a local (the callee then summarised as writing) 746 / 784, and each loses most of the hopper check's matched
// callers in the reduced image, so retail's reloads around 0x1934f0 are not that callee's write summary.
// M3 lane 13 r129 (fast with 0x73a10, 0x749b0, 0x75790, 0x1900a5 and 0x192e60, all matched; control 703): `team`
// declared before `valid` 703; the identity copied with memcpy 703; the group's values pointer declared after the two
// bools 703; the owner compare and copy through `&values->owner` 703; the reset arm's test as `valid == false` 703 (all
// the same code); the properties copied ahead of the identity re-measured 705. The slot test's cmovne still needs the
// SSE file flag (a build-flag question). All restored.
// MiniMax C (11-20): no lower score (703): retail reads the user flags dword at base+8 while the struct member is at base+9.
// Indexed player or state, reference or pointer before descriptor lookup, 703 each. Indexed player or
// state pointer after lookup, 703 each. Advancing player cursor under ordinal or pointer-end bound,
// 702/805. The ordinal-bound form moves one address setup and gains one point. The pointer-end
// form adds 32 bytes. The user flags load uses +9 from the user record in both listings.
// Shared logon task index 703; queried-or-absent status selected, initialized or assigned in separate arms 703/703/703.
// Offline cleanup first with selected or initialized status 800/800; assigned connection result 819;
// offline cleanup first with a short-circuit task query 703.
// NOT kept: the connection result emits the decrement but materializes a Boolean; the offline-first status forms change the first branch and raise the score.
// M3 lane 14 r134 (fast, confirmed plain with the file's 41 functions, 0x641a0, 0x8df50, 0x1900a5 and 0x192e60, all
// matched; aligned = difflib count with addresses masked, 330 at 703). The count is positional and most lines after the
// users loop were one or two instructions out of step, so this round worked from the first place the listings lose
// step. Kept, 703 -> 663 at 2878 B (aligned 301): the first variant loop's index as `dword` (retail counts 16 down in
// EBX and re-reads the availability word and the team byte each pass; a signed `long` index makes this compiler test
// the variant pointer instead and hoist the word into EBX; `short`, `word` and `byte` give the same code) 695; with it
// the request identity held by value with an index scan of the earlier records and the property fill bound
// `record->count` (retail's three-register identity, counted scan and rolled fill) 663. On that: the incomplete arm's
// `failed = true` (round 136's form) 663; the named free space (round 136's) 663; both 664 (2846 B, aligned 297), so
// neither is kept. LEAD, not kept (a switch in place of an if is not a kept form without a match;
// build/r134/lead-0x74aa0-switch.patch): the polls as a switch on the task state (cases 0 and 1 pending, 2 done,
// default failed), retail's `jl / cmp 1, jle / cmp 2, jne` chain at both: the read poll 650, the write poll 660, both
// 652, both with failed and space 648 (aligned 284); as nested range tests in place of the switches the write poll is
// the same code as now and the read poll 10 worse. Pieces without the request loop: `dword` + failed 692; + space
// 694; + both 677; + write switch 679 (with case 2 falling into the cleanup, same code); + read switch 682; + both
// switches 666; + failed, space and both switches 667; the 648 set without `dword` 721; a `long` index with `i != 16`,
// `i <= 15` or a `(long)` sizeof bound 703 each. On the 648 set: the 48 request-loop combinations of a group pointer
// local, the by-value identity, the rolled fill, a properties pointer, the used-count add ahead of the fill and a
// candidate pointer give 648 to 801 (30 of them grow the frame to 0xc8; pointer identity with the rolled fill 715,
// by-value with the unrolled fill 801, group pointer with by-value and rolled 688 at aligned 282, group pointer with the
// pointer identity and `+= 6` ahead of an unrolled fill 652 at retail's 2862 B); a user record pointer in the users
// loop 648 (aligned 285) and 744 with the later copy through it; the existing online_logon_connected() with its
// header 768 (retail's `dec eax`, but the loop pad and the team byte's slot move); the completion identity as a
// member-address ternary 661; the last read time in a local under a nested test 676 (EDI then holds the delay and ESI
// the time as in retail, one `mov esi, eax` short), assigned inside the test 667; the last write time the same way
// 656; the delay as `> 3 ? 300000 : 2000` 667 (retail's setle, an arm swap). Diagnostics, reverted at once: `inline`
// on 0x1934f0's definition 703 (same code, its 17 callers unchanged); the variant check as an unresolved call 760 (the
// word is then re-read, the team byte takes EBX). Still differs and not a source form here: retail expands the two
// user getters 0x647a0/0x647d0, the elapsed-time function 0x75890 (its by-value time is the `mov esi, eax` after each
// `test eax, eax`) and the logon test's helper, all in files built /Ob1 (marking the getters `inline` instead makes
// their two size-optimised callers expand them in a loop, where retail calls), and selects the slot with cmovne (the
// SSE flag); after the request test the listings still lose step at the loop set-up (retail stores both zero counts
// to their slots and loads them into ESI/EDI after the clock read).
// MiniMax C (11-20): no lower score (703): retail reads the user flags dword at base+8 while the struct member is at base+9.
// Shared identity flags 663, flags and admission result 663, packed XUID member 760, flags
// and native property source 663, flags and cache members 663, property endpoint 674, duplicate
// result 654, admission and team names 663. Direct duplicate skip, scan-test duplicate skip
// and duplicate skip with candidate each 694. NOT kept: 654 sets a flag in the scan and tests
// it afterwards. The direct skips lengthen the image. The packed member changes the XUID reads.
// One player reset for either invalid or changed user 760/760; a default reset with positive or negative
// cache-key comparisons 760/760. Complete eligibility before the two clearing paths, assigned or
// conditionally set from positive/exclusion tests 663/663/663/663.
// NOT kept: The shared reset reaches 2862 bytes and raises the score. Eligibility assignment
// reproduces the opening code, including the logon comparison and properties-based cursor.
// Shared logon predicate on the retained request loop 766. Identity getter,
// properties getter, both at group scope, or nested getters 760/784/778/778.
// Shared logon predicate with identity, properties, or nested getters 756/766/754.
// NOT kept: the logon decrement returns, but the providers stay out of line and
// the cursor and slot selection still differ. The identity-only forms reach 2862 bytes.
// Loop-head quota test 663, exact quota at loop head 663, quota after publication 642.
// Remaining slots in the loop condition or body 669/669, last slot after publication 642.
// Limit from the running total 662, count from the first request index 664. NOT kept: the
// 642 forms reach retail length but move the quota check out of the retail loop head. The
// logon comparison, validity-based cursor and slot selection remain.
// M3 lane 16 r168 (h2m fast with 0x641a0, 0x8df50, 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 663, aligned
// 301 by difflib with addresses masked). Kept, 663 -> 643 at retail's 2862 B: the request loop reaches each group
// through a group pointer (`values`, the users loop's name) for the player, the scan key and the owner; the group
// cursor then sits at +0x18 and the player cursor steps from group + 0x1c as in retail (aligned 415: the alignment
// shifts, the positional count falls). Tried on 663: the delay as `> 3 ? 300000 : 2000` with the last request time in
// a local read ahead of the request test 660; with that local under a nested test 658 (an arm swap, not kept). On
// 643: the last request time in a local under a nested test 643 (same positional count); the free space named
// (`record_space`, `property_space`) 663 (2846 B, aligned 281: retail's `sub` pair, but the positional rows shift); a
// properties pointer with `properties_used += record->count` ahead of the rolled fill 723 (2910 B), with the add after
// the fill 670 (2894 B); the variant lookup nested under the state test (no assignment inside the condition) 643 (same
// code).
// M3 lane 7 r211 (h2m fast with 0x641a0, 0x8df50, 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643).
// Reading: retail's request gate is the elapsed-time function 0x75890 expanded (`mov esi, eax` copies g_50c468 after its
// test, the delay in EDI); ours re-reads the global after the clock call, so every row after the gate is one instruction
// off. LEAD, not kept: the gate through a file-local forceinline by-value elapsed step (a copy of 0x75890's body) 621 (the
// gate exact; next the loop set-up, where retail stores both zero counts to their homes and loads them into ESI/EDI after
// the clock read); that step at both gates 626; on the 621 form the opening test through the existing
// online_logon_connected() with its header 750, as this file's switch on the logon status 750, the free space named 645
// (2846 B); `inline` on 0x75890's definition with the gate calling it 664 (2846 B, and matched 0x72700 94: reverted).
// M3 lane 16 r170 (h2m fast with 0x641a0, 0x8df50, 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643): a user
// record pointer for the users loop's flag, identity, properties, slot index and the later identity copy 726, without the
// later copy 643 (same code); the existing online_logon_connected() (online_tasks.h) for the logon test 743 (retail's `dec
// eax`, but the users loop loses its alignment pad); the last request time in `long last_read` ahead of the request test
// 645; the last write time in `long last_write` ahead of the write test 637 (retail's `mov esi, eax` ahead of the clock; a
// gain of 6, under the bar), both locals 639. On the 637 form: the completion identity as a member-address ternary 661
// (2846 B), as an if/else of member addresses 661 (2846 B); the request builder's free space named 662 (2846 B); the write
// poll as a nested range test with `state == 2 && succeeded` inside 660 (2846 B); the write time assigned inside the test
// 637 (diagnostic). Reading: the positional count is set by the request loop's instruction count (ours is 10 short ahead of
// its 0x1934f0 call: retail's request flag in DL and the two counts stored before the clock) and by the polls' range
// chains, neither keepable without a match.
// output-driven user ordinal with ordered bound 816; output-driven user ordinal with equality bound 816;
// separate slot test 643; separate identity sentinel test 643; nested slot and identity tests 643;
// unavailable variant early continue 658; team exclusion early continue 643; three availability arms
// 658.
// NOT kept: The nested tests and team exclusion reproduce 643. The output cursor grows to 2910 bytes,
// and the explicit unavailable path changes the branch layout. The user base and logon comparison remain
// different.
// The logon compare and validity-based cursor home still differ. A user cursor deriving the cache index scored 773,
// with direct cache indexing 777. A cache cursor deriving the user index scored 816, with the user bound once 795.
// Relational and equality bounds had identical scores. NOT kept because all forms added bytes and differences.
// M3 lane 11 r154 (h2m fast with 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643, aligned 699 / masked 298 by
// difflib with addresses, then registers too, masked): the opening test through the existing online_logon_connected()
// with online_tasks.h included 743 (aligned 697, masked 304), as this file's own `connected` switch idiom 743, the helper's
// result held in a named bool 743, `g_467214 != NONE && online_logon_connected()` 743 (all the same code: retail's `dec
// eax`, but the users loop then loses its alignment pad: at that offset retail pads 9 bytes, a `jmp` over a 7-byte `lea`,
// where ours drops a pad over 7 bytes). Diagnostic, reverted at once (the one flag trial): `/arch:SSE` on this file 759
// (aligned 691), with the switch idiom 690 (aligned 687): 0x1900a5's expansion then selects with retail's `cmovne` (its
// own file is SSE), but the users loop pad is still missing, so the pad is not the flag. Lead for the user: retail's
// 0x1900a5 expansion here uses cmov, so retail built this function with `/arch:SSE`.
// M3 lane 3 r298 (h2m fast with 0x75790, 0x749b0, 0x192e60, 0x1900a5, all MATCH; control 643, aligned 699 / registers
// masked 298): `inline` on 0x75790's definition (the completion loop's only out-of-line callee, to drop its memory
// summary) 643 (same code: the property count is still hoisted); the users loop's group index as `dword` 715 (2878 B);
// the team and failed flags renamed to sort first, or those two and the valid flag renamed to sort last, 643 each (same
// code: the shared byte cell stays at +0x17 against retail's +0x13); the delay as an `else if` / `else` pair 643 (same
// code, still `setg`).
// M3 lane 12 r178 (h2m fast, control 643 at 2862 B; rows 157 to the end are all one or more instructions off, so a real
// fix only shows once the instruction counts ahead of it agree). Diagnostic, restored: with this file's `connected`
// switch idiom for the logon test (retail's `dec eax`, 743) and the users loop bounded by an undefined extern instead of
// 4, the loop head gets its `jmp` + pad again (758, 2878 B): the build pads a loop head by more than 7 bytes only when
// the trip count is not a known constant, so retail's 9-byte pad says its users loop was not a constant-count loop when
// the pad was decided (it has the pad with the user getters expanded, lane 7 r185's lead). Loop spellings on the
// switch idiom, none brings the pad back: a `short` index 750, a `dword` index 750, do/while with `++group < 4` 743, the
// bound as the user table's element count 743; without the idiom the `short` and `dword` indices 715 / 715 (2878 B), the
// do/while 643 (same code). A 32-combination grid of five forms on the current body (a: the switch idiom; b: the
// builder's free space named and tested `> 0`; c: the last request time in a two-use local ahead of the request test;
// d: the last write time in a two-use local ahead of the write test; f: `failed = true` in the incomplete arm): a 743,
// b 663, c 645, d 637, f 679, ab 758, ac 743, ad 744, af 776, bc 666, bd 662, bf 695, cd 639, cf 681, df 678, abc 758,
// abd 756, abf 789, acd 744, acf 776, adf 775, bcd 665, bcf 698, bdf 695, cdf 680, abcd 756, abcf 789, abdf 789, acdf 775,
// bcdf 698, abcdf 789 (sizes 2814 to 2862 B). The lowest is d alone, 6 under the control and under the keep bar.
// Reading of retail's request loop (0x74d40 to 0x74f2b), for the next round: a byte flag in DL is set for state 1
// and the state-4 time test jumps past its test; the record cursor (EBX, +0x14 into the record) and a property cursor
// (EBP, stepped by 0x60) are both carried; the count 6 sits in ESI for the count store, the `properties_used` add (ahead
// of the fill loop) and the fill loop's bound; `existing`, `records_used` and `properties_used` stay in their homes
// inside the scan loop and are loaded into ESI/EDI at the inner latch. Retail's write poll tests the range first
// (`state < 0` to the release, `state <= 1` to the return) and `state == 2` inside it.
// M3 lane 6 r233 (h2m fast with 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643, aligned 699 / masked 256):
// the property fill written through the global property array (`g_50cbf8[properties_used + j]`, no record re-reads),
// bound `record->count` 677 (2894 B); bound 6 with `properties_used += 6` 669 (2846 B); those two with the last write
// time in a two-use local 678 / 669; bound 6 with `+= 6` and the free space named (`record_space`, `property_space`,
// tested `> 0`) 645 at retail's 2862 B (aligned 453 / masked 258: the frame grows to 0xc8, and with no aliasing store
// the player state stays in EBP, where retail re-reads it, so the property cursor loses EBP); that with the last write
// local 645; bound 6 with `+= record->count` and the named space 645; bound `record->count` with the named space 689.
// Reading: retail re-reads the state after the fill, so its fill stores go through a pointer that may alias it.
// M3 lane 2 r143 (h2m fast with 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643): the two request counts
// zeroed by one chained assignment 643 (same code); the clock read ahead of the two zeroed counts 646; the write poll's
// pending test as `state == 0 || state == 1` 643 (same code); the received count as `dword` 643 (same code); `if (received
// != 6) failed = true;` for the `failed = failed || ...` update 679 (2830 B). Reading: retail lays out the request's
// failure arm (the expanded restore loop, then the failure count) ahead of the read poll, and tests that poll's range
// (`jl`, `cmp 1; jle`, `cmp 2; jne`) before the completion block. Body unchanged.
// M3 lane 4 r256 (h2m fast with 0x72700, 0x75890 and 0x75870; control 643 at 2862 B, aligned 440; body unchanged). The
// function does not answer to names, declaration order or the number of locals: the request outputs' third local renamed
// to sort first or last 643 / 643, the team flag renamed to sort first or last 643 / 643 (same-size slots do not follow
// the names here: retail has the request's third output at +0x28 beside the other two, ours at +0x30; the team byte at
// +0x13, ours +0x17), the flag pair declared team first 643, the request outputs declared third-first, or in three other
// orders 643 each, and 1 to 8 unused initialised longs at the top (diagnostic) 643 each; every one is the same code.
// What is left is not a spelling: the users cursor (retail the record itself in a stack cell, with the copy chain of
// expanded pointer getters), the slot test's conditional move and the elapsed-time copy, as the lines above record.
// M3 lane 2 r145 (h2m fast with 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643; body unchanged): the delay
// as `> 3 ? 300000 : 2000` (retail's `setle` and 0x7d0 base; an arm swap, match-only) 640; that with the last request
// time in a two-use local 642; with the last write time in a two-use local 634 (the local alone 637, re-measured); with
// both locals 636; the write poll's pending range as an early return ahead of the state-2 test 643 (same code), nested
// under `state >= 0` 643 (same code); either with the last write local 660 (2846 B), with both locals 662. The 634 form
// needs the arm swap, so nothing is kept.
// The logon comparison and properties-based user cursor still differ.
// Typed owner publication or comparison 643/643; slot copied between, before, or after user outputs
// 643/643/643; common user record 726; shared logon predicate with typed owner and with common user
// 743/727. NOT kept: the typed owner alone ties and the forms that move the cursor or logon comparison raise the score.
// Single task-index read with direct guard, selected status, initialized readiness, guarded readiness,
// conditional readiness, initialized offline result, default status or an offline early return:
// 643/643/643/754/754/643/643/643. The opening status comparison and first user cursor still differ.
// The logon comparison and properties-based user cursor still differ.
// Successful refresh continue 643, with scoped team result 643, direct admission result 643,
// unavailable reset before promotion 658 or vacancy 658, team-first eligibility 646,
// nested vacancy 643, available vacancy first 649. NOT kept: the equal scores leave the
// cursor at properties and the other forms raise the score.
// M3 lane GA r1 (fast with 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643 at 2862 B, aligned 699 / masked
// 297). New structural cause: `g_50c460..g_50fd14` belong inside `s_search_value_globals g_509450` (at +0x3010..+0x70c4 of
// the existing 0x70f8-byte struct). Moving them into `g_509450` restores retail's aliasing in the completion loop (reloading
// `record->properties`, `record->count` and `g_50c470` each pass as retail does, and placing the team/failed byte at retail's
// +0x13): alone 646 (2878 B, aligned **385**, masked **276**); with `last_write` local 652 (aligned 386), with `last_read` +
// `last_write` 654 (aligned 385), with `property->type == 0 ? 0 : ...` and member-address identity ternary + `last_write` 667
// (aligned 358), with named free space + `last_write` 650 (2862 B, aligned 384, masked 267), with `query_time_now` if/return
// + space + completion + `last_write` 673 (2830 B, aligned 329, masked 236); with `switch (online_task_poll(...))` on read &
// write polls + `failed = true` in else (lead, not kept: switch/flag) 677 (2830 B, aligned **298**, masked **216**; whole
// completion loop and write section/poll/offline tail 0x753f2..0x755cd match retail); plus request `dl` flag 647 (2846 B,
// aligned 315, masked **215**). Also tried on 643 base: `s_cache_property *properties` with literal 6 + `+= 6` ahead of fill
// 704 (indexed or stepped in body); `query_time_now` if/return alone 695, with `last_write` 694; completion `type == 0` +
// member ternary + `failed = true` in else 674 (same via `g_509454[group]`); both poll switches alone 700, with `query_time_now`
// + `last_write` 683; request `dl` flag alone 639 (2878 B), with `last_write` 636 (2878 B, under keep bar and flag form).
// Restored (body unchanged); `s_search_value_globals` lead saved in build/ga1/74aa0_t14_full_combo.log.
// Valid record pointer as a conditional or guarded assignment 759/759, cleared after
// binding 726, and chosen from a bound record 765. Valid identity pointer 758/758,
// valid properties pointer 749, and bound record with identity gate 758.
// NOT kept: the record result at retail length still changes the cursor and scores 726.
// M3 lane 1 r167 (h2m fast; control 643 at 2862 B, aligned 409 / registers masked 154). The write and read polls as a
// `switch` on the poll state (the matched 0x8bf70's idiom; retail tests both polls' ranges first), each alone: write
// 643 (masked 150, same positional count), read (`case 0: case 1: break;`, failure as `default`) 645; with the last
// write time in a two-use local: write switch 660 (2846 B), read switch 639; the write switch with both time locals
// 662 (2846 B). Declaring the file's request globals `static` (a test of whether the clock call's reload of the last
// read time is an aliasing kill) 643 (same code), so the reload is 0x75890 not being expanded. Body unchanged.
// M3 lane GB r2 (20 trials, fast control 643 at 2862 B with 8 peers MATCH; plain confirmed 617 at 2862 B, all 40 other functions in this file plus 0x641a0, 0x8df50, 0x8dd70, 0x80940/aa0/bf0/d70, 0x1900a5, 0x192e60 MATCH, 0 address-target diffs):
// Review (m3): NOT kept. The 617 needs two forms kept only with a byte match: the fill's stores in an order picked by score
// (record->properties ahead of record->count, properties_used += 6 ahead of the loop) and query_time_now() rewritten as
// if/return; with main's order and helper form the same body scores 704, the helper form alone 654, the struct alone 646.
// Patch held (LANES/leads/held-74aa0-gb2-617.patch). The lane's 643 -> 617 form (aligned 631 -> 349, masked 680 -> 492): loose g_50c460..g_50fd14 folded into s_search_value_globals g_509450 (exact +0x3010..+0x70c4 offsets matching memset(&g_509450, 0, 0x70f8) in callers), query_time_now() as 0x75870's if-return form, two-use last_read/last_write and record_space/property_space locals, three-use properties pointer rolling the 6-property loop with properties_used += 6, type-5 duplicate continue, named completion owner/unknown10 fields, failed = true in the incomplete arm, and goto-free state < 0 || state > 1 polls.
// Intermediate forms on g_509450 struct: struct alone 646; +clock 657; +completion (no type==0 swap) 634; +last_write 651; +last_read/last_write 651; +dup continue 648; +properties pointer 657; +ptr+dup 661; +ptr+space 654; +lw+dup 667; +lw+ptr+dup 676; +polls/space/ptr/dup/last_read/last_write 617 (kept). Rejected earlier forms: struct+clock+comp+polls 655; +last/space 693; +ptr 666; +ready flag & poll gotos 629; +logon >3 760; loose globals with comp+polls 696; struct+last+comp+space 660.
// The cursor stays based on properties. Typed properties, typed XUID assignment, or both 643/643/643.
// Each with a shared input receiver 726/726/726. Existing connection predicate with typed properties
// or with both typed copies and shared receiver 743/727. NOT kept: the typed copies tie and the
// predicate restores the decrement but raises the score.
// The logon comparison and properties-based cursor still differ. Cooldown result
// with zero first, timestamp first, initialized or separate arms: read gate
// 641/641/633/641, write gate 643/643/642/643. NOT kept: the 633 form sets and
// then tests a flag at the read gate, and the first difference remains +0x1a.
// The logon compare and properties-based cursor remain. Assigning the actual
// admission result after the copies, with one condition, guest check first,
// sentinel last, or three separate tests scores 643/736/753/792. Carrying the
// same flag from the source validity gives 643/736/753/792 in the same order.
// NOT kept: the combined test ties and the split forms lengthen the body.
// M3 lane GA r5 (24 fast trials with 0x749b0, 0x75790, 0x192e60, 0x1900a5, all MATCH; control 643 at 2862 B; body unchanged). Tested clean subsets of s_search_value_globals g_509450 without the two match-only forms (keeping main's query_time_now() and fill store order): struct alone 646 (2878 B); +named completion owner/unknown10 667; +failed=true in else 647 (2862 B); +direct g_509454[group] 647; +long property_index 647; +goto-free state<0||state>1 polls 644 (2862 B); +named record_space/property_space 654 (2846 B); +last_write 644 (2862 B); +last_read/last_write 646; +all 657 (2846 B); +properties pointer in main's store order 673 (2830 B); +polls/last_write 668. Also tested retail's `if (variant) { ... } else player->state = 1;` completion structure (emits retail's exact `player->state = 1` tail after state==5): on g_509450 struct with main's `failed || received != 6` 702 (2910 B), with per-arm `failed = true` (retail's stores in state==2 and state==5) 660 (2878 B), with single `failed = true` in else 680, per-arm + named id 656 / + last_write 659 / + dup continue 691 / + all clean 673, orig fail + named id 676 / + last_write 706 / + dup+lw 757; on loose globals with main's `failed ||` 663 (2878 B), with per-arm `failed = true` 637 (2862 B, -6 pts, under keep bar and flag-in-test).
// The logon CMP, input cursor and slot conditional move still differ.
// Shared XUID flags before/after the property copy 643/643. Member copies,
// shared member flags, after-copy publication, after-slot publication, on
// admission and late flag sample 743/743/743/745/750/747. NOT kept: the
// after-slot store follows retail, but EDX holds the flags instead of EBX
// and the member forms grow the body.
// M3 lane 14 r156 (h2m fast with 0x749b0, 0x75790, 0x192e60 and 0x1900a5, all MATCH; control 643): the user's identity
// copied after the properties 645; `team` computed ahead of `valid = true` 643 (same code); the variant loop's index as
// `long` 714; the values' identity refreshed from the local identity 726; the variant test as nested ifs 643 (same
// code). Body unchanged.
// M3 lane GA r15 (24 fast trials with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x641a0, 0x8df50, 0x80940/aa0/bf0/d70 all MATCH; control 643):
// Lane form, 643 -> 623 (2878/2862 B, 773/772 insns, addr-aligned 631 -> 514, masked 680 -> 593, plain verified): consolidated
// s_search_value_globals g_509450 (+0x3010..+0x70c4, restoring retail's [esp+0x13] flag slot and completion reloads), completion loop
// reading g_509454[group] directly with loop-hoisted variant_type = variant->type (retail's [esp+0x30]) and named unknown10/&owner fields,
// and range-first if (state < 0 || state > 1) read/write polls — keeping main's query_time_now(), fill store order and failed || update.
// Also tried (not kept): carried record/properties pointers across request loop 679/687 (loose) and 687/690 (struct); typed XUID team 662
// (loose) / 667 (struct); direct completion + variant_type 661 (loose) / 643 (struct, +last_write 665, +dup continue 658, +dup+lw 663,
// +value-if+lw 664, +record_space/property_space 642 at 2862 B / addr-aligned 357); without variant_type 667 (read poll only 663, +space
// 636 at 2862 B / addr-aligned 360); if (added >= 4) break 643 (loose) / 646 (struct); read-only struct 646; combined carried/typed/space
// subsets 659-687.
// Review: the range-first read/write polls are a condition-order change (match-only), so main's if (state == 2) / else-if
// shapes stay; kept at 643 (unchanged): the g_509450 fields at +0x3010..+0x70c4 (0x8d9f0 and 0x8dd70 clear the whole
// 0x70f8-byte block with memset, which now resets these counters and tasks as in retail), the completion loop's direct
// g_509454[group] with the unknown10/&owner fields and the variant_type local. The fields alone 646; the range-first polls on
// this form are the 623 lead.
// M3 lane GA r18 (12 fast trials with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x641a0, 0x8df50 all MATCH; control 643 at 2878/2862 B,
// +0x1a; body unchanged): tested on the consolidated g_509450 + direct completion + variant_type base: variant->type read inside
// completion record loop 667; completion if (variant) { ... } else player->state = 1; with in-loop variant->type 676 (2910 B);
// request identity pointer (no local XUID copy) 661 (2846 B); request due byte flag (state==1 / state==4 elapsed) 648 (2894 B);
// request properties pointer with literal 6 and += 6 after fill 696 (2910 B); users loop direct g_509454[group] 753; completion
// property->type == 0 ? 0 : ... 643; two-use last_write local 664, nested 665; nested last_read local 643; both nested 665,
// with in-loop variant->type 667. Blockers stand: 0x1900a5 cmovne needs /arch:SSE (file flag), users cursor needs inlined 0x647d0.
// M3 lane GA r21 (14 fast trials + plain check across all 51 file/caller/callee VAs, 50/50 peers MATCH; baseline 643 at 2878/2862 B -> kept 633 at exact 2862/2862 B):
// Kept (both retail-verified and strictly additive, -7 and -3 -> -10): three-use `s_cache_property *properties = &g_509450.read_properties[properties_used];` in main's
// store order (recovers retail's rolled property fill without `record->properties` reloads and restores 2862 B; 636 alone) plus `read_failures > 3 ? 300000 : 2000`
// (matches retail's exact `setle` / `and 0x48c10` / `add 0x7d0` at lines 141-147 verbatim; 640 alone, 633 together). Also tried on the 636 base: existing_record pointer 636,
// completion values pointer 636, typed query_restore_pending pointer 636, fill element pointer 636, users player pointer 638, records_used++ after fill 639, dup continue 684;
// with `/arch:SSE` on this file (0 peer regressions): SSE alone 751 (2862 B), + properties pointer 759, + online_logon_connected() 692, + both 687, + space 757, + dup+space 769.
// M3 lane GB r21 (25 fast trials with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x641a0, 0x8df50, 0x80940, 0x80aa0, 0x80bf0, 0x80d70 all MATCH; control 643 at 2878/2862 B, aligned 361 / masked 272; body unchanged):
// three-use s_cache_property *properties pointer in main's store order with j < record->count and properties_used += record->count 636 at retail's 2862 B (rolled property fill, -7 pts, one short of keep bar, not kept);
// on that 636 form: + record_space/property_space 643 (aligned 403 / masked 311), + two-use last_read 639, + two-use last_write 640, + both 643, + space + last_read/last_write/both 645/642/644,
// + space + read_record_count > 0 guard 656 (+ both times 639), + space + nested variant 643 (+ both times 644), + space + both times + real XUID/word/dword fields 644 (all 2862 B except rcount 2878 B);
// on 643 base: real XUID/word fields on s_cache_property_record/s_cache_property (+ XUID team_owner, or + dword count) 643/643/643 (+ space 643 at 2862 B, aligned 359 / masked 261);
// properties pointer with properties_used += 6 736 (2926 B, unrolled; + space 704, + XUID/word + space 704, + dup continue + space 730);
// nested if (variant) + space 643 (2862 B, aligned 358 / masked 260); while duplicate scan 643; read_record_count > 0 guard + space 642; if (variant) ... else state=1 + guard 736 (2942 B).
// M3 lane GA r24 (15 fast trials on the 633 base with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x641a0, 0x8df50, 0x80940,
// 0x80aa0, 0x80bf0, 0x80d70 all MATCH; control 633 at 2862/2862 B, 768/772 insns, al=376, mk=253; body unchanged):
// two-use last_write local 637 (mk=252); two-use last_read local 636; both nested under !state 637 (al=373, mk=249);
// completion per-arm `else if (state == 2) { state = 1; failed = true; } else if (state == 5) { state = 4; failed = true; }`
// 658 (2846 B, al=362, mk=240), with `if (variant) { ... } else player->state = 1;` and `property->type == 0 ? 0 : ...` 658
// (2878 B, al=368, mk=245), plus `if (read_record_count > 0)` around variant_type 680 (2910 B, al=357, mk=235);
// fill `long count = 6` for record->count, loop bound and properties_used += count 696 (2910 B, al=343, mk=233);
// `properties_used += 6` ahead of `j < record->count` loop 681 (2910 B, al=357, mk=227); carried `next_property` cursor
// 673 (2874 B, al=363); named record_space/property_space 640 (2862 B, al=363), + per-arm failed 674 (2830 B, al=348, mk=235),
// + last_write 674; polls as `state != 0 && state != 1` / `state == 0 || state == 1` 633 (al=374, mk=251); `pstate` local in
// request loop 633; `buffers` declared first 633. All retail-aligned structural forms shift positional rows up behind the
// three cross-file/flag blockers (0x1900a5 cmovne /arch:SSE, 0x647a0/0x647d0 and 0x75890 /Ob1).
// M3 lane 12 r194 (h2m fast with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x641a0, 0x8df50, 0x72700, 0x75890, 0x6c7e0, all MATCH;
// control 633): the opening test through the existing logon helper 0x6c7e0 itself 763 (its /Ob1 file keeps it a call); the
// request gate through the existing elapsed-time function 0x75890 itself 646 (a call, same file reason); the delay as one
// conditional expression 633 (same code); the property value as `= 0` then `if (type)` 642; the team flag declared ahead of
// the valid flag 633 (same code).
// M3 lane 11 r174 (h2m fast with 0x749b0, 0x75790, 0x192e60 and 0x1900a5, all MATCH; control 633): the opening test as
// the file's own logon switch (a `connected` flag set in `case 1`, as 0x73ae0 and 0x73040 write it) 759 (2878 B: retail's
// `dec eax`, but the users loop then loses its entry jump and pad); that with a request `due` flag 759; the request
// test as a `due` flag set for state 1 and else-if for an elapsed state 4, 633 (same code); `bool due = state == 1`
// then the elapsed test 634; the fill gate as `80 - records_used > 0 && 480 - properties_used > 0` 641.
// M3 lane GA r27 (15 fast trials on the 633 base with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x641a0, 0x8df50, 0x80940,
// 0x80aa0, 0x80bf0, 0x80d70 all MATCH; control 633 at 2862/2862 B, 768/772 insns, al=376, mk=262; body unchanged):
// XUID record->identity with struct assign 633 (same code); with inner type-5 `if (existing < records_used) continue;`
// 681 (2910 B, al=399, mk=271; + XUID team_owner 681; + named record_space/property_space 664 at 2894 B; + `properties_used += 6`
// before `j < record->count` 730 at 2926 B, mk=234; + `j < 6` 737, mk=239; + space + write_task local 644 at 2878 B, al=374);
// word id field in s_cache_property 633 (same code); users loop `&values->owner` struct copy + `values->unknown10` 633 (same code);
// completion `if (variant) ... else player->state = 1;` keeping `failed ||` 674 (2894 B, mk=259); completion nested `state == 2 || 5`
// 633; request loop `added < 4 && i < 16` 640 (2862 B); completion `long property_index` 633 (al=378); read_task local 655
// (2846 B, al=355, mk=258); write_task local 656 (2846 B, al=358, mk=257). Positional score stays gated by the /arch:SSE and /Ob1 blockers.
// M3 lane 7 r237 (h2m fast with 0x749b0, 0x75790, 0x192e60 and 0x1900a5, all MATCH; control 633): the opening test
// through the existing online_get_logon_status (0x6c7c0, called) 734 (2878 B); the header's online_logon_connected()
// re-measured on this base 759 (2878 B: retail's `dec eax`, the users loop's 9-byte pad, retail's `jmp` over a 7-byte
// lea, dropped), with a user record pointer for the flag, identity, slot and properties 752 (the cursor becomes the
// identity at +1 in EBP, not retail's record base in memory); the logon result in a named bool 633, the request count
// tests as `!= 0` 633, the slot index read into a local ahead of the properties copy 633 (all three the same code).
// Blockers unchanged: retail inlines the session-user getters 0x647a0/0x647d0 (one valid test for both) and the elapsed
// time 0x75890, all in /Ob1 files, and the slot test's cmovne needs the SSE flag.
// M3 lane GB r29 (14 fast trials across 2 batches with all 10 peers MATCH; control 633 at 2862/2862 B, 768/772 insns,
// al=424, mk=303; body unchanged): two-`if` `due` byte flag in request loop 612 (2878 B, 774/772 insns, al=436, mk=302;
// NOT kept: flag set inside test and longer than retail); single `failed = true;` in completion `else` 658 (2846 B,
// al=399, mk=296); `received != 6` first + `property->type == 0` 659 (2846 B, al=393, mk=293); completion without
// `continue` + `failed = true` 658 (2878 B, al=397); structured range-first write poll 640 (2862 B, al=416, mk=298),
// both polls range-first 635 (2862 B, al=410, mk=291); `delay` single ternary / nested read gate / nested write gate /
// request `long state` local / `read_records` pointer all 633; `now` before counters 643; `delay` default 2000 668;
// users loop `players` pointer 742 (2878 B).
// M3 lane GA r32 (30 real + 1 flag diag fast trials across 2 batches with 0x749b0, 0x75790, 0x192e60, 0x1900a5, 0x80940,
// 0x80aa0, 0x80bf0, 0x80d70, and 0x647d0/0x18fd94 on getter trials, all MATCH; control 633 at 2862/2862 B, al=400, mk=574;
// body unchanged): `inline` on existing `0x647d0` (`network_session_interface_get_user_properties`) in `src/unknown_054fe0.cpp`
// and `src/unknown_190001.cpp` (0x647d0/0x18fd94 stay MATCH) 716 (2878 B, al=392; checked or direct call identical; + logon
// switch 767, al=362; + in-loop `variant->type` 716, al=391; + both poll switches + `query_time_now()` if/return 727; diag
// + `/arch:SSE` 759 at 2862 B, al=378, reproducing all 120 users-loop instructions verbatim); on clean 633 base, in-loop
// `variant->type` 633 (al=399, LICM hoists after `jle` as retail), `dword j` fill 633, `switch` read poll 633 (2878 B, al=396),
// `switch` write poll 640 (al=392), both poll switches 654 (al=388), `query_time_now()` if/return 656 (2846 B; + switches 673),
// read-fail cleanup order 633, `j < 6` with `+= record->count` 643 (2862 B, stays rolled), `records_used++` after `record->type`
// 634 (2862 B, al=395), `properties_used += 6` after rolled `j < record->count` loop 736 (2926 B, recovers retail's carried `+0x60`
// properties cursor; + `record_space`/`property_space` 704 at 2910 B, al=343, mk=533; + `last_write` 706, al=337; + `if (variant)`
// per-arm `failed = true` 711, al=337, mk=492; + `dup continue` 742, al=345, mk=486; with `j < 6` 696; carried `next_properties`
// 736 / + space 704). Positional score remains gated by `/arch:SSE` (`0x1900a5` `cmovne`) and `/Ob1` helper inlining blockers.
// @retail 0x74aa0
void function_074aa0(void)
{
    if (g_467214 != NONE && online_task_get_logon_status(g_467214) == 1)
    {
        for (long group = 0; group < 4; group++)
        {
            s_search_value_group *values = &g_509454[group];
            bool valid = false;
            bool team = false;
            if (g_4cd868.users[group].valid)
            {
                XUID identity = g_4cd868.users[group].xuid;
                byte properties[0x90];
                memcpy(properties, g_4cd868.users[group].properties, sizeof(properties));
                if (!function_1900a5(g_4cd868.users[group].unknown10) && !(identity.dwUserFlags & 3) &&
                    identity.dwUserFlags != 0xbad00000)
                {
                    valid = true;
                    team = memcmp(properties + 0x70, g_440070, 12) != 0;
                    if (memcmp((byte *)values + 4, &identity, 12) != 0 ||
                        memcmp((byte *)values + 0x10, properties + 0x70, 12) != 0)
                    {
                        memcpy((byte *)values + 4, &g_4cd868.users[group].xuid, 12);
                        memcpy((byte *)values + 0x10, properties + 0x70, 12);
                        for (long i = 0; i < 16; i++) values->players[i].state = 0;
                    }
                    for (dword i = 0; i < 16; i++)
                    {
                        s_surface_description *variant = function_192e60(i);
                        if (variant)
                        {
                            if ((variant->type != 5 || team) && !values->players[i].state)
                                values->players[i].state = 1;
                        }
                        else
                            values->players[i].state = 0;
                    }
                }
            }
            if (!valid)
                for (long i = 0; i < 16; i++) values->players[i].state = 0;
        }
        long delay;
        if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state != 3)
            delay = 300000;
        else
            delay = g_509450.read_failures > 3 ? 300000 : 2000;
        if (!g_509450.read_state && g_47d8f4.count && (g_47d8f4.flags & 2) &&
            (!g_509450.read_time || query_time_now() - g_509450.read_time > delay))
        {
            long records_used = 0;
            long properties_used = 0;
            long now = query_time_now();
            for (long group = 0; group < 5; group++)
            {
                s_search_value_group *values = &g_509454[group];
                long added = 0;
                for (long i = 0; i < 16 && added < 4; i++)
                {
                    s_search_player_values *player = &values->players[i];
                    s_surface_description *variant;
                    if ((player->state == 1 ||
                        (player->state == 4 && now - player->time >= g_network_configuration.valuec74)) &&
                        (variant = function_192e60(i)) != NULL)
                    {
                        XUID identity;
                        long existing = records_used;
                        if (variant->type == 5)
                        {
                            memcpy(&identity, values->unknown10, 12);
                            for (existing = 0; existing < records_used; existing++)
                            {
                                if (!memcmp(g_509450.read_records[existing].identity, &identity, 12) &&
                                    g_509450.read_records[existing].type == variant->field_8)
                                    break;
                            }
                        }
                        else
                            identity = values->owner;
                        if (existing == records_used)
                        {
                            s_cache_property_record *record = &g_509450.read_records[records_used];
                            memcpy(record->identity, &identity, 12);
                            record->type = variant->field_8;
                            s_cache_property *properties = &g_509450.read_properties[properties_used];
                            record->count = 6;
                            record->properties = properties;
                            records_used++;
                            for (long j = 0; j < record->count; j++)
                            {
                                *(word *)&properties[j].unknown00 = (word)(j + 1);
                                properties[j].type = 1;
                            }
                            properties_used += record->count;
                            added++;
                            player->state = player->state == 1 ? 2 : 5;
                        }
                    }
                }
            }
            if (records_used < 80 && properties_used < 480)
            {
                long records = 0, properties = 0, buffers;
                function_80940(80 - records_used, &g_509450.read_records[records_used],
                    480 - properties_used, &g_509450.read_properties[properties_used], 32, g_509450.read_buffers,
                    &records, &properties, &buffers);
                records_used += records;
                properties_used += properties;
            }
            if (records_used > 0 && properties_used > 0)
            {
                long task = online_stats_read((word)records_used, (XONLINE_STAT_SPEC *)g_509450.read_records);
                g_509450.read_time = query_time_now();
                if (task != NONE)
                {
                    g_509450.read_task = task;
                    g_509450.read_record_count = records_used;
                    g_509450.read_property_count = properties_used;
                    g_509450.read_state = 1;
                }
                else
                {
                    query_restore_pending();
                    g_509450.read_failures++;
                }
            }
        }
        if (g_509450.read_state == 1)
        {
            long state = online_task_poll(g_509450.read_task);
            if (state == 2)
            {
                bool failed = false;
                if (online_stats_read_result(g_509450.read_task, (XONLINE_STAT_SPEC *)g_509450.read_records,
                    (word)g_509450.read_record_count, g_509450.read_buffers, sizeof(g_509450.read_buffers)))
                {
                    for (long group = 0; group < 5; group++)
                    {
                        for (long i = 0; i < 16; i++)
                        {
                            s_search_player_values *player = &g_509454[group].players[i];
                            if (player->state != 2 && player->state != 5) continue;
                            s_surface_description *variant = function_192e60(i);
                            if (!variant) { player->state = 1; continue; }
                            long received = 0;
                            long variant_type = variant->type;
                            for (long record_index = 0; record_index < g_509450.read_record_count; record_index++)
                            {
                                s_cache_property_record *record = &g_509450.read_records[record_index];
                                const byte *identity = variant_type == 5 ? g_509454[group].unknown10 :
                                    (const byte *)&g_509454[group].owner;
                                if (!memcmp(identity, record->identity, 12) && record->type == variant->field_8)
                                    for (dword property_index = 0; property_index < (dword)record->count; property_index++)
                                    {
                                        s_cache_property *property = &record->properties[property_index];
                                        long value = property->type ? property->unknown08 : 0;
                                        long index;
                                        if (function_75790(record->type, &index))
                                            function_749b0(group, index,
                                                *(word *)&property->unknown00 - 1, value);
                                        received++;
                                    }
                            }
                            if (received == 6) player->state = 3;
                            else
                            {
                                if (player->state == 2) player->state = 1;
                                else if (player->state == 5) player->state = 4;
                            }
                            failed = failed || received != 6;
                        }
                    }
                    function_80aa0(g_509450.read_records, g_509450.read_record_count);
                }
                else { query_restore_pending(); failed = true; }
                function_6b640(g_509450.read_task);
                g_509450.read_task = NONE;
                g_509450.read_state = 0;
                g_509450.read_time = query_time_now();
                if (failed) g_509450.read_failures++;
                else g_509450.read_failures = 0;
            }
            else if (state < 0 || state > 1)
            {
                query_restore_pending();
                g_509450.read_state = 0;
                function_6b640(g_509450.read_task);
                g_509450.read_task = NONE;
                g_509450.read_time = query_time_now();
                g_509450.read_failures++;
            }
        }
        if (!g_509450.write_state && (!g_509450.write_time || query_time_now() - g_509450.write_time > 300000))
        {
            long records = 0, properties = 0, buffers;
            function_80bf0(32, g_509450.write_records, 128, g_509450.write_properties, g_509450.write_buffers, &records, &properties, &buffers);
            if (records > 0 && properties > 0)
            {
                long task = online_stats_write((XONLINE_STAT_SPEC *)g_509450.write_records, (word)records);
                g_509450.write_time = query_time_now();
                if (task != NONE)
                {
                    g_509450.write_task = task;
                    g_509450.write_record_count = records;
                    g_509450.write_property_count = properties;
                    g_509450.write_state = 1;
                }
            }
        }
        if (g_509450.write_state == 1)
        {
            long state = online_task_poll(g_509450.write_task);
            if (state == 2)
            {
                if (online_stats_write_succeeded(g_509450.write_task))
                    function_80d70(g_509450.write_records, g_509450.write_record_count);
            }
            else if (state >= 0 && state <= 1) return;
            function_6b640(g_509450.write_task);
            g_509450.write_state = 0;
            g_509450.write_task = NONE;
            g_509450.write_time = query_time_now();
        }
    }
    else
    {
        for (long group = 0; group < 5; group++)
            for (long i = 0; i < 16; i++) g_509454[group].players[i].state = 0;
        if (g_509450.read_state == 1) { function_6b640(g_509450.read_task); g_509450.read_state = 0; }
        if (g_509450.write_state == 1) { function_6b640(g_509450.write_task); g_509450.write_state = 0; }
        g_509450.read_time = 0;
        g_509450.write_time = 0;
    }
}

long online_round_report(XNKID const *session_id, ULONGLONG const *round_key,
    long count, XONLINE_STAT_PROC const *procedures, bool flag0, bool flag1);

// @retail 0x73ca0
void function_73ca0(byte *results)
{
    byte storage[0xc090];
    bool valid = true;
    bool local_host = false;
    if (results)
    {
        s_surface_description *variant = function_192e60(*(long *)(results + 4));
        if (!variant || g_510540.a != *(long *)(results + 0x130) ||
            g_510540.b != *(long *)(results + 0x134))
            valid = false;
        for (long i = 0; i < 16 && valid; i++) {}
        if (valid)
        {
            long local = function_199250((s_address_table *)results, g_4cf7cc);
            if (local == NONE) valid = false;
            else if (!results[2]) local_host = local == function_199290(results);
        }
    }
    else valid = false;
    long count = 0;
    const XONLINE_STAT_PROC *procedures = NULL;
    if (valid)
    {
        s_query_batch *batch = (s_query_batch *)storage;
        function_741f0((s_match_player_rating_collection *)results, batch);
        count = batch->operation_count;
        procedures = (const XONLINE_STAT_PROC *)batch->operations;
    }
    long task = online_round_report((const XNKID *)&g_510528, (const ULONGLONG *)&g_510520,
        count, procedures, local_host, g_51051d);
    if (task != NONE)
    {
        g_510530 = task;
        g_510518 = 3;
    }
    else
        function_73dc0(2);
}

bool online_task_service_unavailable(long task_index);
long online_round_extend(XNKID const *session_id, ULONGLONG const *round_key, word seconds);

/* the logon task's poll result: an unknown state still drops the pending task */
// @retail 0x755d0
void function_755d0(void)
{
    if (g_467214 == NONE)
        return;
    switch (online_task_get_logon_status(g_467214))
    {
    case 1:
    {
        if (g_510518 == 2 && g_51051c)
            function_73ca0(NULL);
        if (g_510518 == 3 || g_510518 == 1)
        {
            long state = g_510518;
            long task = g_510530;
            long status = online_task_poll(task);
            if (status < 0)
                goto poll_failed;
            if (status > 1)
            {
                if (status == 2)
                {
                    switch (state)
                    {
                    case 1:
                        g_510518 = 2;
                        g_510534 = function_75870();
                    default:
                        /* an unknown state still drops the pending task */
                        function_6b640(g_510530);
                        g_510530 = NONE;
                        break;
                    case 3:
                        g_510518 = 0;
                        g_510514 = 0;
                        function_6b640(g_510530);
                        g_510530 = NONE;
                        break;
                    }
                }
                else
                {
                poll_failed:
                    function_6b640(task);
                    g_510530 = NONE;
                    switch (g_510518)
                    {
                    case 1: function_73dc0(1); break;
                    case 3: function_73dc0(2); break;
                    }
                }
            }
        }
        if (g_510518 == 2)
        {
            long task = g_510538;
            if (task == NONE)
            {
                long interval = g_network_configuration.value374 * 1000 / 3;
                long last = g_510534;
                if (query_time_now() - last >= interval)
                {
                    word seconds = (word)g_network_configuration.value374;
                    long created = online_round_extend((const XNKID *)&g_510528,
                        (const ULONGLONG *)&g_510520, seconds);
                    if (created != NONE)
                        g_510538 = created;
                }
            }
            else
            {
                long status = online_task_poll(task);
                long time;
                if (status < 0)
                    goto extension_failed;
                if (status <= 1)
                    return;
                if (status == 2)
                    time = function_75870();
                else
                {
                extension_failed:
                    if (status != 3 || !online_task_service_unavailable(task))
                        function_73dc0(3);
                    time = query_time_now();
                }
                g_510534 = time;
                function_6b640(g_510538);
                g_510538 = NONE;
            }
        }
        break;
    }
    }
}
