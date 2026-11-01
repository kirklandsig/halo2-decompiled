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
long function_746b0(long previous, long current, long first, long second)
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
				total += function_746b0(other_skill, local_2, local_3, local_1);
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

// score 16 (was 22): retail holds the entry skill sign-extended in ESI (movsx at the load, mov ebx,esi for the call); a 32-bit local gives that movsx but MSVC then picks EBX for it and moves the team-mode byte to CL, so the whole frame shifts.
// Also tried with a 32-bit skill: the call reading the field instead 24 (the compare narrows back to 16 bits); the local at the loop top 104; team mode declared first 113; a const team mode 113; locals renamed 113; an unsigned 32-bit skill 113;
// declared at function scope 113; team mode as byte or char 113; team mode read from the collection in the loop 113 (in both places 105); the compare reading the field 99; declared inside the condition's block 103;
// loads and tests in one condition 104; the tests as continues 113; the key and active tests split from the rest 113; the selected skill read from the entry at the call 103; the key read from the collection 117;
// the call passing (long)other_skill 16; other_team read before the tests 65; an int skill 113; a long skill with the field compared 99; a long skill and long team mode 92.
// Round 103: the other rank as a ternary 106; the fields read through collection->players[i] without the entry pointer 16 (same code), with a 32-bit skill 113;
// the id typed as the XUID it is with the guest test as XOnlineIsUserGuest 16 (same code), with a 32-bit skill 113; permuter 47 variants, none better.
// Diagnostic: a 32-bit skill without its NONE test lets team mode keep BL (118; the load then sinks to the call).
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
			short other_skill = entry->skill;
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
					total += function_746b0(other_skill, skill, rank, other_rank);
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

/* the search value globals (0x509450, 0x70f8 bytes): set by the network
   initialize, cleared by the network dispose */
struct s_search_value_globals
{
	bool initialized;
	byte unknown01[3];
	s_search_value_group groups[5];
	byte unknown3010[0x70f8 - 0x3010];
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
// score 6 (was 6): BLOCKED-BY 0x7e210: everything else matches. Still differ: the
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
// score 92 (was 92): frame 0xfc and length (1212) agree. Left: level and the group-compare cursor take
// each other's slots (ours 0x18/0x14, retail 0x14/0x18); retail loads the key before the collection
// fields and calls the identity formatter before pushing strncpy's count; in the team loop retail keeps
// the counters cursor in a stack slot and the collection, then level, in EBX (ours: cursor in EBX,
// level spilled). Round 29: the per-player and per-team counters declared in the collection and the
// player counts read straight from `collection->player_counters[i]` (retail's +0xf44 cursor), the team
// fields and identity read from `collection->teams[i]` at each use (retail's +0xdc8 cursor), 177 -> 130;
// the identity declared just before the strncpy with the group compare reading the player address
// 108; the name character as `word` 92 (kept). Tried: counters through record pointers (same code),
// read orders 179-181, identity as a team-loop local 295, identity before the summary 125, word
// character with a short countdown 139, the formatted identity in a local 105, the team counters
// pointer formed at the call (same code), a copy-and-terminate helper for the strncpy 225 (strncpy
// expanded inline), level or all four shared locals per loop 108/108, renaming level (no change).
// Round 67: the team count switch with one statement per hopper
// type (0x66050's form; retail's jump table) 202 -> 177 (kept); on it: word character 183; `key`
// renamed (slot order by name) 177, no slot moved; statistics reads third/second/first 179; the
// statistics pointer computed at its use without the copy 174 (three points, not kept), with the
// team pointer too 179.
// Forms tried before (on 202 unless noted): live key/player/team/both skills 348/320/199/317; key plus
// player/team/both 379/374/388; player-only key phase 266; existing converter 371; converter inline
// boundary 367; native summary fields 231; native skill snapshot 302; immutable query inputs 231;
// native counter bits 231; native session fields 231; frame pointer 412; team value snapshots 225; team
// promoted skill snapshot 239; native player statistics array 231; native statistics arrays 231;
// external player/team/both record references 225/220/220; early player identity 234; native records
// plus formatted identity 219; UTF-16 input 220; shared eligibility 353; bounded name conversion 332;
// direct native team pointer 220; native team pointer/reference plus formatted identity 219/219;
// byte/word player ordinal 335/335; byte group 319; both byte 339; word/byte character budget 215/239;
// dword key 220; shared value/change/level/lower declarations: value 220, change 220, level 206,
// lower 220, value/change 220, level/lower 206, value/level 206, change/lower 220; shared short/promoted
// skill 284/220; entry/shared statistics 206/206; entry group 206; shared statistics with short/promoted
// skill 284/220; statistics plus entry group 206; shared identity/statistic views 206/206, both 206;
// name-source advance at load 218, with the views 218/218/218; shared outer statistics cursor 206;
// bounded character countdown 326; group guards 206; player admission scope 206; team guards 206;
// guards plus countdown 326; player/team admission 206; all admissions plus countdown 326; negative-first
// lower selection 212; unsigned name capacity 207; capacity-first continuation 216; ordinary-slot-first
// output 213; capacity/continuation, capacity/output, continuation/output, all three 241/214/220/248;
// copied-character ordinal with native pointers 207.
// Round 138: player/team/both adjusted values 206/206/206; player table loop 206; zero chain 205;
// table/player result 206; current player value 206; early summary ratings 303; summary count/lookup/
// formatter/completion call declarations 206 each; dword group/player/character 314/206/202; all three
// 316 (full-width character kept at 1167 bytes). Round 148: existing bound helper in a bounded for,
// caller ordinary/force-inline/inline 323/323/323; direct bound-helper break 224; native player pointer
// 325; whole summary clear 353; outer summary workspace 323; native player plus summary clear 353;
// definition-side force-inline with bounded for/direct break 323/216.
// Round 317: late player/team/both statistic producers 199/207/204; late player plus native indexing
// 199; ascending/descending reads on late player 203/201; both late with ascending/native indexing
// 208/204; on late player: key first 199, shared carrier names 199, word character 203, positive
// character budget for 320. Player-change/rating/group-add/group-set inline and noinline declarations
// 202 each. Signed full-width character 203; entry query-table workspace 202; test-derived translated
// character 264; signed character plus query workspace 203; signed/entry/both with translation
// 264/264/264; entry name-capacity workspace 202. Native unsigned masked statistics 202; shared
// player/team ordinal 220 (key home recovered); contiguous summary prefix fill 217; counts/ordinal 220;
// counts/prefix 217; ordinal/prefix 214; all three 214; read-only query table 202.
// Round 230: promoted membership 335; group/team identity-first tests 350/202; membership with
// group/team/both identity-first 345/336/345; both identity-first without membership 349; native-byte
// membership with both 343. Round 366: native fifteen-bit player/team/both statistic fields 195/207/200
// fast; word/short player results 279/279, with both arrays 283/283; native statistic receiver 199; on
// the recovered +0xf44 cursor: native player indexing/shared counts/query-key name 195/195/195; entry
// statistic receiver 202. Round 130: stored summary change/count-set/level consumers, individually and
// all combinations, 202 each; complete stored consumers with word character 206. Native fifteen-bit
// fields with one player/team ordinal: late receiver with dword/word character 216/202;
// completed-summary consumers 216/202; early receiver with dword/word character 220/206; completed-
// summary consumers 220/206; direct native fields with dword/word character 213/199 (the last restores
// key +0x1c and cursor +0xf44). Native full-width unsigned/signed status fields 202/202; native word/
// bitfield admission with full sentinel 350/350; split sentinel halves 335/335; native status with
// player indexing 202, with fifteen-bit counters 199. Round 188: variant-lookup/team-change/query-
// emission/group-bound inline and noinline declarations 202 each. DL159: uniform name-input field
// observations, crossed with shared pass ordinal, shared three-count results and two-word prefix clear,
// 234-236. Round 210: query-count table publication 123/132/213/231/312/321 202/206/213/344/214/345;
// entry table with 123/321 202/345. Round 194: on native fifteen-bit player statistics and a shared
// pass ordinal, byte/short/word/dword group counters with dword or word characters all 311.
// Round 187: Native indexed group pointer, array cursor, body-stepped cursor, record reference, each with dword/word character: 300/295, 321/322, 286/294, 300/295. All restored; key/statistics/change/group homes remain.
// Round 417: on separate real team cases, shared ordinal/native fifteen-bit player fields/both
// 195/170/188; ordinal+word character/fields+word/both+word 181/176/174; all stored summary
// inputs with ordinal+fields, dword/word character 188/174. On fields alone, actual 0..64
// name budget word/short/byte/dword 174/181/230/170; word/short/byte budget with word character
// 170/174/211. Seven-point leads rejected; original types, loops and complete body restored.
// DL190: Native status alone/statistics/both with aggregate zero 177/170/365; status with aggregate zero 365; status with word character 183, with statistics 176, with statistics/aggregate 369; statistics/aggregate without status 365. Native full-width unsigned-long/int/unsigned-int count results on status/statistics 170 each. Seven-point best rejected; original types, loops and body restored.
// Round 12 (m3 lane 2; kept: the team statistics pointer formed in the inner block instead of a loop-top
// pointer copied into it, byte-identical 92): the formatted identity in a local before the strncpy 89
// (retail's call-before-count order); with it identity renamed player_identity or level renamed
// adjusted_level/rating_level 89 (names never move the level/compare-cursor slots); with it the summary's
// key stored before the two collection fields 83 (fixes that block except the identity slot), the two
// fields swapped 87, key between them 89; the key first alone 88; with the inner statistics pointer 88;
// the statistics read through collection->team_counters[i] at the call 212; level declared per loop 108,
// all four shared values per loop 108, each with the formatted local and key first 100. Left after 83:
// level and the compare cursor/identity slot swapped (ours 0x18/0x14), and in the team loop retail keeps
// the collection then level in EBX and the counters cursor in a stack slot (ours the reverse).
// Round 47 (lane 16; kept: the summary as `= { 0 }` with a 64-bit first field, byte-identical 92, in
// place of two zero stores and a hand-written memset of the rest): the team loop bounded by a
// team-count helper in the for condition 331 (loop rotated); a team entry pointer in the team loop
// 245 (1192 bytes); a force-inline terminating copy helper for the identity 225 (it stays a
// register-argument call under the inline_depth(0) region); on the kept form, the key stored before the
// session copy 88 (the summary block then differs only in the formatter call/count push order).
// Slot sharing: retail packs {level, team cursor} and {group-compare cursor, identity, counters
// cursor}; ours spills level in both loops and keeps the counters cursor in EBX, so the slot swap
// likely follows the team loop's register choice (diagnostic: with constant counter arguments level
// takes EBX, but the collection is still reloaded from its parameter slot rather than kept in EBX).
// Round 134 (m3 lane 4): retail's team loop keeps the collection in EBX (pushed for 0x74720, reloaded
// at the loop end), level in EBX after it, and both cursors in slots 0x14/0x18; ours turns EBX into the
// counters cursor, so level spills to 0x18 in both loops, which is likely the level/compare-cursor slot
// swap as well. The four shared values declared level first 92; the key stored before the session 88
// (the summary block then differs only in the formatter call/count push order; 4 points, not kept); on
// it, the group compare against `&g_509454[group].owner` 88 (same code); the team counters as a walking
// pointer (diagnostic) 213 (the collection moves to EDI from entry); a typed collection copy for the
// team loop (diagnostic) 88 (copy-propagated). All restored.
// Round 37 (lane 10, fast): in retail's team loop both cursors are memory-homed (stored at the loop head
// each pass, reloaded at the bottom) and the collection is reloaded into EBX at the bottom, level taking
// EBX after the change call; ours keeps the counters cursor in EBX. The counters pointer defined at the
// top of the team-loop body 92 (same code); the team tests as an early continue 92; a team identity
// pointer used by the compare and the call 245 (1192 bytes); the shared locals declared level, lower,
// value, change 92; the counters read from the collection at the call 212 (as before); the team loop through
// a `const s_match_rating_collection *` view of the collection (entries for the team reads, passed to the
// change call) 92; without the inline_depth(0) region (diagnostic) 407 (1268 bytes).
// Round 30 (lane 14, fast): the key stored before the session copy 88 again (the summary block then differs
// only in the formatter call/count push order; 4 points, not kept); on it: a named group-entry pointer for
// the state test and the owner compare 279 (1196 bytes); level declared per loop 105 (every slot reshuffles,
// the team loop still gives EBX to the counters cursor); lower declared per loop 88; the team statistics
// pointer formed before the change call 88; the team counters read into first/second/third locals
// (diagnostic) 212 (the collection moves to EDI from entry, the counters cursor still EBX).
// On the original source: lower as `lower = 0; if (value >= 0) lower = ...;` in both loops 274.
// Round 35 (lane 13, fast): the team loop's level computed inside the change-call argument list 131 (MSVC calls it
// in place during the pushes, after lower; retail calls it right after the change), level and lower both there
// 138; the team loop as the `type == 5` if-branch 317; the key stored before the session copy 88 (as before);
// with the key read from the collection at that store 90.
// Round 103 (m3 lane 7): the player entry's id typed as an XUID at +2 (guest test and 0xbad00000 compare on
// xuid.dwUserFlags, identity as &player->xuid) 92 (same code); the team statistics pointer formed before the
// change call 92; a typed 64-bit session field at +0x130 92; level renamed updated_level (sorts after the
// compiler's tv temporaries) 92; the team entry's id typed as an XUID 92. Diagnostic: constant team statistics
// arguments 212 (level then takes EBX in the team loop, the collection stays in memory).
// Round 47 (m3 lane 1, fast): the player entry as plain `&collection->players[i]` in place of the
// +0x41e byte cursor and its -0x9a step back 92 (byte-identical code; kept); the identity written at
// both uses with no named local 92 (no slot moves: the level/compare-cursor order is not the identity
// local's name); with the key stored first as well 88; the team count switched once before a plain
// counted team loop 157 (1192 bytes); the team skill as a sign-extended long 102.
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
                summary.session = *(const ULONGLONG *)((byte *)collection + 0x130);
                summary.values[2] = key;
                const XUID *identity = (const XUID *)((const byte *)player + 2);
                strncpy(summary.identity, function_1537a0((const byte *)identity), 64);
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
                short skill = collection->teams[i].skill;
                value = collection->teams[i].value;
                if (skill != NONE && value != NONE)
                {
                    change = function_74720((const s_match_rating_collection *)collection, i);
                    value += change;
                    level = function_74970(value, skill);
                    lower = value >= 0 ? function_74970(value, 0) : 0;
                    const s_rating_counters_team *stats = &collection->team_counters[i];
                    function_73e00(lower, batch, (const XUID *)collection->teams[i].unknown0c, change,
                        stats->first, stats->second, stats->third, level, 3, players);
                }
            }
        }
    }
    function_739a0(NONE, key);
}
#pragma inline_depth(255)

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

long g_50c460;
long g_50c464;
long g_50c468;
long g_50c46c;
long g_50c470;
s_cache_property_record g_50c474[80];
long g_50cbf4;
s_cache_property g_50cbf8[480];
byte g_50e9fc[2048];
long g_50f1fc;
long g_50f200;
long g_50f204;
long g_50f208;
s_cache_property_record g_50f20c[32];
long g_50f50c;
s_cache_property g_50f510[128];
byte g_50fd14[2048];

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
static inline bool query_variant_available(long index)
{
    long result = false;
    if (g_47d8f4.count && (g_47d8f4.flags & 2))
        result = function_1934f0(&g_551ae8[index]);
    return result;
}

#pragma optimize("g", off)
static __forceinline s_player_slot_flags *function_74aa1(s_player_slot_flags *arg_0)
{
    s_player_slot_flags *local_0 = NULL;
    if (arg_0) local_0 = arg_0;
    return local_0;
}
#pragma optimize("", on)
// score 709 (was 709): Frame 0xc0 agrees; initial cursor base/home, slot and later homes remain.
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
                s_player_slot_flags *slot = function_74aa1((s_player_slot_flags *)
                    ((byte *)&g_54e8e0[g_4cd868.users[group].unknown10] + 0x470));
                if (!(slot->flags & 3) && !(identity.dwUserFlags & 3) &&
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
                    for (long i = 0; i < 16; i++)
                    {
                        if (query_variant_available(i))
                        {
                            if ((g_551ae8[i].type != 5 || team) && !values->players[i].state)
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
            delay = g_50c460 <= 3 ? 2000 : 300000;
        if (!g_50c464 && g_47d8f4.count && (g_47d8f4.flags & 2) &&
            (!g_50c468 || query_time_now() - g_50c468 > delay))
        {
            long records_used = 0;
            long properties_used = 0;
            long now = query_time_now();
            for (long group = 0; group < 5; group++)
            {
                long added = 0;
                for (long i = 0; i < 16 && added < 4; i++)
                {
                    s_search_player_values *player = &g_509454[group].players[i];
                    if ((player->state == 1 ||
                        (player->state == 4 && now - player->time >= g_network_configuration.valuec74)) &&
                        query_variant_available(i))
                    {
                        const byte *identity = (const byte *)&g_509454[group] + (g_551ae8[i].type == 5 ? 0x10 : 4);
                        s_cache_property_record *candidate = g_50c474 + records_used;
                        if (g_551ae8[i].type == 5)
                            for (candidate = g_50c474; candidate < g_50c474 + records_used; candidate++)
                                if (!memcmp(candidate->identity, identity, 12) &&
                                    candidate->type == g_551ae8[i].field_8)
                                    break;
                        if (candidate == g_50c474 + records_used)
                        {
                            s_cache_property_record *record = &g_50c474[records_used];
                            memcpy(record->identity, identity, 12);
                            record->type = g_551ae8[i].field_8;
                            record->count = 6;
                            record->properties = &g_50cbf8[properties_used];
                            records_used++;
                            for (long j = 0; j < 6; j++)
                            {
                                *(word *)&record->properties[j].unknown00 = (word)(j + 1);
                                record->properties[j].type = 1;
                            }
                            properties_used += 6;
                            added++;
                            player->state = player->state == 1 ? 2 : 5;
                        }
                    }
                }
            }
            if (records_used < 80 && properties_used < 480)
            {
                long records = 0, properties = 0, buffers;
                function_80940(80 - records_used, &g_50c474[records_used],
                    480 - properties_used, &g_50cbf8[properties_used], 32, g_50e9fc,
                    &records, &properties, &buffers);
                records_used += records;
                properties_used += properties;
            }
            if (records_used > 0 && properties_used > 0)
            {
                long task = online_stats_read((word)records_used, (XONLINE_STAT_SPEC *)g_50c474);
                g_50c468 = query_time_now();
                if (task != NONE)
                {
                    g_50c46c = task;
                    g_50c470 = records_used;
                    g_50cbf4 = properties_used;
                    g_50c464 = 1;
                }
                else
                {
                    query_restore_pending();
                    g_50c460++;
                }
            }
        }
        if (g_50c464 == 1)
        {
            long state = online_task_poll(g_50c46c);
            if (state == 2)
            {
                bool failed = false;
                if (online_stats_read_result(g_50c46c, (XONLINE_STAT_SPEC *)g_50c474,
                    (word)g_50c470, g_50e9fc, sizeof(g_50e9fc)))
                {
                    for (s_search_value_group *completed_values = g_509454; completed_values < g_509454 + 5; ++completed_values)
                    {
                        for (long i = 0; i < 16; i++)
                        {
                            s_search_player_values *player = &completed_values->players[i];
                            if (player->state != 2 && player->state != 5) continue;
                            if (!query_variant_available(i)) { player->state = 1; continue; }
                            long received = 0;
                            for (long record_index = 0; record_index < g_50c470; record_index++)
                            {
                                s_cache_property_record *record = &g_50c474[record_index];
                                const byte *identity = (const byte *)completed_values +
                                    (g_551ae8[i].type == 5 ? 0x10 : 4);
                                if (!memcmp(identity, record->identity, 12) && record->type == g_551ae8[i].field_8)
                                    for (dword property_index = 0; property_index < (dword)record->count; property_index++)
                                    {
                                        s_cache_property *property = &record->properties[property_index];
                                        long value = property->type ? property->unknown08 : 0;
                                        long index;
                                        if (function_75790(record->type, &index))
                                        {
                                            long field = *(word *)&property->unknown00 - 1;
                                            long bounded = value;
                                            if (bounded < g_46722c[field][0]) bounded = g_46722c[field][0];
                                            else if (bounded > g_46722c[field][1]) bounded = g_46722c[field][1];
                                            s_search_value *destination = &completed_values->players[index].values[field];
                                            if (!destination->valid || destination->value != bounded)
                                            { destination->value = bounded; destination->valid = true; }
                                        }
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
                    function_80aa0(g_50c474, g_50c470);
                }
                else { query_restore_pending(); failed = true; }
                function_6b640(g_50c46c);
                g_50c46c = NONE;
                g_50c464 = 0;
                g_50c468 = query_time_now();
                if (failed) g_50c460++;
                else g_50c460 = 0;
            }
            else if (state < 0 || state > 1)
            {
                query_restore_pending();
                g_50c464 = 0;
                function_6b640(g_50c46c);
                g_50c46c = NONE;
                g_50c468 = query_time_now();
                g_50c460++;
            }
        }
        if (!g_50f1fc && (!g_50f200 || query_time_now() - g_50f200 > 300000))
        {
            long records = 0, properties = 0, buffers;
            function_80bf0(32, g_50f20c, 128, g_50f510, g_50fd14, &records, &properties, &buffers);
            if (records > 0 && properties > 0)
            {
                long task = online_stats_write((XONLINE_STAT_SPEC *)g_50f20c, (word)records);
                g_50f200 = query_time_now();
                if (task != NONE)
                {
                    g_50f204 = task;
                    g_50f208 = records;
                    g_50f50c = properties;
                    g_50f1fc = 1;
                }
            }
        }
        if (g_50f1fc == 1)
        {
            long state = online_task_poll(g_50f204);
            if (state == 2)
            {
                if (online_stats_write_succeeded(g_50f204))
                    function_80d70(g_50f20c, g_50f208);
            }
            else if (state >= 0 && state <= 1) return;
            function_6b640(g_50f204);
            g_50f1fc = 0;
            g_50f204 = NONE;
            g_50f200 = query_time_now();
        }
    }
    else
    {
        for (long group = 0; group < 5; group++)
            for (long i = 0; i < 16; i++) g_509454[group].players[i].state = 0;
        if (g_50c464 == 1) { function_6b640(g_50c46c); g_50c464 = 0; }
        if (g_50f1fc == 1) { function_6b640(g_50f204); g_50f1fc = 0; }
        g_50c468 = 0;
        g_50f200 = 0;
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
