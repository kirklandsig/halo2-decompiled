/* SLOT_HANDLER.H: the actor slot handlers of 0x1b0000..0x1bffff (lane B).

   g_46eeb8 (actor_behavior.cpp) points at one handler per slot type. Each
   handler is a static struct in the data of its own source file, next to
   its callbacks; all callbacks take the actor index first and are called
   through these structs, so they keep the standard __stdcall convention.

   - kind 0 handlers are 0x14 bytes: the header and one test.
   - kind 1 handlers are 0x4c bytes: a choice callback, then a count and an
     array of child entries.
   - kind 2 handlers are 0x4c bytes, three updates at the end; some (those
     built on the shared callbacks at 0x26e4b0..0x26e710) are 0x70 bytes. */

#ifndef SLOT_HANDLER_H
#define SLOT_HANDLER_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "matrix_transform.h"
#include "globals.h"
#include "slot_owner.h"
#include "unit_requests.h"
#include "unknown_20fe20.h"
#include "unknown_1fb7e0.h"
#include "unknown_1f4460.h"
#include <math.h>

/* the slot header every handler state starts with (s_slot of slot_owner.h
   is the whole 0x40 bytes) */
struct s_slot_header
{
	short type;
	short state;
	short unknown4;
	byte unknown6[2];
	long time;
};

typedef short (__stdcall *t_slot_priority)(long actor_index);
typedef short (__stdcall *t_slot_evaluate)(long actor_index, s_slot *slot, bool active);
typedef bool (__stdcall *t_slot_start)(long actor_index, s_slot *slot);
typedef void (__stdcall *t_slot_proc)(long actor_index, s_slot *slot);
typedef void (__stdcall *t_slot_notify)(long actor_index, s_slot *slot, bool active);
typedef void (__stdcall *t_slot_release)(long actor_index, s_slot *slot, long index);
struct s_slot_target_list;
typedef void (__stdcall *t_slot_list)(long actor_index, s_slot *slot, s_slot_target_list *list);
typedef short (__stdcall *t_slot_choose)(long actor_index, short level, bool active);
typedef void (__stdcall *t_slot_proc4)(long actor_index, s_slot *slot, long a, long b);
typedef short (__stdcall *t_slot_trigger)(long actor_index, s_slot *slot);
/* the same callbacks as the query and evaluate procedures take them from the
   action lists (unknown_1a58b0.cpp): a plain argument in place of the flag */
typedef short (__stdcall *t_slot_query)(long actor_index, long argument);
typedef short (__stdcall *t_slot_evaluate_argument)(long actor_index, s_slot *slot, long argument);

struct s_slot_handler_0
{
	short index;
	short kind;
	long mask;
	long unknown8;
	long unknownc;
	union
	{
		t_slot_trigger trigger;
		t_slot_query query;
	};
};

/* the part all kind 1 and 2 handlers share; the handler table g_46eeb8
   points at it */
struct s_slot_handler
{
	short index;
	short kind;
	long mask;
	long unknown8;
	long unknownc;
	t_slot_priority priority;
	union
	{
		t_slot_evaluate evaluate;
		t_slot_evaluate_argument evaluate_argument;
	};
	t_slot_start start;
	t_slot_proc stop;
	short wanted_type;
	byte unknown22[2];
	t_slot_release release24;
	t_slot_release release28;
	t_slot_release release2c;
	t_slot_proc proc30;
	t_slot_notify notify34;
	long unknown38;
	long unknown3c;
};

/* the handlers by slot type (actor_behavior.cpp) and what enables a type:
   its handler's unknown8 differs from g_46f348, its mask covers g_4ee4ec,
   and its bit in g_557c40 is set (globals.h) */
enum
{
	k_slot_type_count = 0x83
};

extern s_slot_handler *g_46eeb8[k_slot_type_count];

inline bool slot_type_enabled(short type)
{
	s_slot_handler *handler = g_46eeb8[type];

	return handler->unknown8 != g_46f348 &&
		(handler->mask & g_4ee4ec) == g_4ee4ec &&
		(bool)((g_557c40[type >> 5] >> (type & 31)) & 1);
}

/* the game's deterministic random (g_4e7408), inlined as Bungie's macros do */
static inline real slot_random(void)
{
	dword *seed = &g_4e7408->unknown0;

	*seed = 1664525 * *seed + 1013904223;
	return (real)(*seed >> 16) * (1.f / 65535.f);
}

static inline real slot_random_range(real lower, real upper)
{
	return lower + (upper - lower) * slot_random();
}

/* rounds as the x87 does (unknown_0259d0's fld/fistp idiom); retail has the
   fld/fistp inline at every call */
static __forceinline long real_to_long(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

/* unknown_023540.cpp */
real magnitude3d(vector3f const *v);

/* an element (0xb4 bytes) of the block of the actor's tag, found by
   function_1e5450 (character_tag_accessors.cpp) */
struct s_tag_element
{
	byte unknown00[4];
	long key;
	byte unknown08[4];
	long tag_index;
	byte unknown10[0x94 - 0x10];
	real unknown94;
	byte unknown98[0xa4 - 0x98];
	real unknowna4;
	real unknowna8;
	real unknownac;
	short unknownb0;
	byte unknownb2[0xb4 - 0xb2];
};

s_tag_element *function_1e5450(long owner_index, long key);

/* a child entry of a kind 1 handler */
struct s_slot_child
{
	short type;
	short flags;
	short unknown4;
	byte unknown6[2];
	real unknown8;
	long unknownc;
	long unknown10;
};

struct s_slot_handler_1
{
	s_slot_handler head;
	t_slot_choose choose;
	long child_count;
	s_slot_child *children;
};

/* a kind 1 handler built on the shared callbacks at 0x26e750..0x26e8a0 */
struct s_slot_handler_1x
{
	s_slot_handler_1 base;
	t_slot_proc update4c;
	t_slot_release release50;
	t_slot_proc4 proc54;
	t_slot_proc4 proc58;
	short unknown5c;
	short unknown5e;
	real unknown60;
};

struct s_slot_handler_2
{
	s_slot_handler head;
	t_slot_proc update40;
	t_slot_proc update44;
	t_slot_proc update48;
};

struct s_slot_handler_2x
{
	s_slot_handler_2 base;
	t_slot_proc update4c;
	t_slot_release release50;
	t_slot_release release54;
	t_slot_release release58;
	t_slot_list list5c;
	t_slot_proc4 proc60;
	short unknown64;
	short unknown66;
	real unknown68;
	long unknown6c;
};

/* the actor's recent slot entries (12 bytes each at +0x194), and the
   iterator over those of one type (function_26f0c0) */
struct s_slot_memory_entry
{
	short type;
	byte unknown2[2];
	long unknown4;
	long time;
};

struct s_slot_entry_iterator
{
	long actor_index;
	s_reference reference;
};

s_slot_memory_entry *function_26f0c0(s_slot_entry_iterator *iterator);

/* a location in the world (0x14 bytes; function_26bfa0 fills one) */
struct s_location_view
{
	s_type_c3b527 point;
	long unknown10;
};

/* the 6 byte entries of the actor's table at +0x400 (s_reference and its
   unset value g_470fa0 are in globals.h) */
struct s_reference_entry
{
	bool unknown0;
	byte unknown1;
	s_reference reference;
};

/* the list some kind 2x callbacks get (an element of g_502424): entries of
   actor indices; the first entry's actor leads */
struct s_slot_target_entry
{
	long unknown0;
	long actor_index;
	short type;
	byte unknown0a[2];
};

struct s_slot_target_list
{
	union
	{
		struct
		{
			short unknown0;
			short count;
		};
		s_slot_target_entry entries[10];
	};
	byte unknown78[4];
	short entry_count;
	byte unknown7e[2];
	bool unknown80;
	byte unknown81;
	short mode;
	long unknown84;
	long unknown88;
};

/* trivial callbacks; retail folds each with identical functions elsewhere */
static bool __stdcall slot_start_true(long actor_index, s_slot *slot)
{
	return true;
}

static void __stdcall slot_proc_nothing(long actor_index, s_slot *slot)
{
}

static bool __stdcall slot_release_true(long actor_index, s_slot *slot, long index)
{
	return true;
}

static void __stdcall slot_release_nothing(long actor_index, s_slot *slot, long index)
{
}

/* callbacks shared by several handlers (identical functions folded) */
short __stdcall actor_behavior_priority_minimum(long actor_index);
short __stdcall function_1bced0(long actor_index, s_slot *slot, bool active);
short __stdcall function_1b2d90(long actor_index, s_slot *slot, bool active);
short __stdcall function_1a79e0(long actor_index, short level, bool active);
short __stdcall function_1adcd0(long actor_index);

/* the prop data (unknown_25d690.cpp): g_502418 holds 0x3c byte nodes, whose
   view (0x124 bytes in g_502414, from +0x70) function_25d740 returns */
struct s_type_f95cd3;
struct s_prop_node;
s_type_f95cd3 *function_25d740(s_prop_node *node);

struct s_prop_datum;
struct s_type_5cfb45;
s_type_5cfb45 *function_25d690(s_prop_datum *datum);

struct s_prop_node_view
{
	byte unknown00[4];
	long unknown04;
	long unknown08;
	byte unknown0c[8];
	long view_index;
	short type;
	byte unknown1a[6];
	long object_index;
	short unknown24;
	char unknown26;
	char unknown27;
	real unknown28;
	long next_index;
	byte unknown30[0x3c - 0x30];
};

struct s_prop_view_fields
{
	short unknown00;
	byte unknown02[0x6 - 0x2];
	short unknown06;
	byte unknown08[0x10 - 0x8];
	long unknown10;
	byte unknown14[0x18 - 0x14];
	s_type_c3b527 unknown18;
	byte unknown28[0x2c - 0x28];
	vector3f unknown2c;
	byte unknown38[0x3c - 0x38];
	real unknown3c;
	byte unknown40[0x4c - 0x40];
	bool unknown4c;
	byte unknown4d[0x54 - 0x4d];
	real unknown54;
	byte unknown58[0x60 - 0x58];
	real unknown60;
	byte unknown64[0x68 - 0x64];
	bool unknown68;
	bool unknown69;
	byte unknown6a[0x6d - 0x6a];
	bool unknown6d;
	byte unknown6e[0x70 - 0x6e];
	short unknown70;
	byte unknown72[0x78 - 0x72];
	s_type_c3b527 unknown78;
	bool unknown88;
	byte unknown89[3];
	short unknown8c;
};

struct s_prop_state_view
{
	long unknown00;
	point3f position;
	byte unknown10[0x38 - 0x10];
	real unknown38;
	long unknown3c;
	byte unknown40[0x48 - 0x40];
	s_type_c3b527 unknown48;
};

s_type_f95cd3 *function_25d700(long index);

inline s_prop_view_fields *prop_view_fields_get(long index)
{
	return (s_prop_view_fields *)function_25d700(index);
}

inline s_prop_state_view *prop_node_state(s_prop_node_view *node)
{
	return (s_prop_state_view *)function_25d690((s_prop_datum *)node);
}

/* the objects: g_4e0300 holds 12 byte headers with the object at +8 */
struct s_object_header_view
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

struct s_slot_object_view
{
	long tag_index;
	byte unknown004[0xc - 0x4];
	long next_object_index;
	long first_child_index;
	long parent_index;
	char parent_node;
	byte unknown019[0x30 - 0x19];
	point3f unknown030;
	real unknown03c;
	byte unknown040[0x70 - 0x40];
	vector3f forward;
	byte unknown07c[0x88 - 0x7c];
	vector3f velocity;
	byte unknown094[0xaa - 0x94];
	byte type;
	byte unknownab[0xb2 - 0xab];
	byte unknownb2;
	byte unknownb3[0xec - 0xb3];
	real unknownec;
	real unknownf0;
	byte unknownf4[0x100 - 0xf4];
	real unknown100;
	byte unknown104[0x116 - 0x104];
	short node_matrices_offset;
	byte unknown118[0x12c - 0x118];
	long actor_index;
	byte unknown130[0x138 - 0x130];
	short team;
	byte unknown13a[2];
	long player_index;
	byte unknown140[0x1fc - 0x140];
	short unknown1fc;
	byte unknown1fe[0x34f - 0x1fe];
	byte unknown34f;
	byte unknown350[0x3a0 - 0x350];
	long unknown3a0;
	byte unknown3a4[0x3b0 - 0x3a4];
	dword unknown3b0;
	dword unknown3b4;
};

inline s_object_header_view *object_header_get(long object_index)
{
	return &((s_object_header_view *)g_4e0300->data)[object_index & 0xffff];
}

inline s_slot_object_view *object_get(long object_index)
{
	return (s_slot_object_view *)((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
}

/* the node matrices of an object start node_matrices_offset bytes into it */
inline transform4x3f *object_node_matrix(s_slot_object_view *object, long node)
{
	return (transform4x3f *)((byte *)object + object->node_matrices_offset) + node;
}

/* the object's forward vector in world space */
inline void object_get_forward(long object_index, vector3f *forward)
{
	s_slot_object_view *object = object_get(object_index);

	if (object->parent_index == NONE)
		*forward = object->forward;
	else
		matrix_transform_vector(object_node_matrix(object_get(object->parent_index), object->parent_node), &object->forward, forward);
}

/* the game allegiance globals (unknown_1c9830.cpp): the peace bits are at
   +0xc4 */
bool function_0bfe60(const dword *flags, long bit);
bool function_15e020(short a, short b);
bool function_1df560(short team_a, short team_b);

struct s_game_allegiance_globals;
extern s_game_allegiance_globals *g_4f55ec;

struct s_allegiance_view
{
	byte unknown00[0xc4];
	dword peace_bits[8];
};

/* a copy of function_1df560 (0x1df560): retail inlines it in some
   callers, but unknown_1c9830.cpp is /Ob1 */
static inline bool team_is_enemy(short team_a, short team_b)
{
	bool result = true;

	if (team_a == NONE || team_b == NONE)
		return true;

	long mode = g_4e6948->state;

	if (mode == 1)
	{
		if (team_a >= 0 && team_a < 16 && team_b >= 0 && team_b < 16)
		{
			long bit = team_a * 16 + team_b;
			result = !function_0bfe60(((s_allegiance_view *)g_4f55ec)->peace_bits, bit);
		}
	}
	else if (mode == 2)
	{
		result = function_15e020(team_a, team_b);
	}
	else
	{
		result = team_a != team_b;
	}
	return result;
}

/* the data arrays of 0xbc byte (g_502424) and 0x50 byte (g_502420) elements
   the handlers keep indices into (globals.h) */
struct s_502424_target
{
	long unknown0;
	long unknown4;
	union
	{
		long unknown8;
		point3f point;
	};
	short unknown14;
	byte unknown16[2];
};

struct s_502424_element
{
	byte unknown00[4];
	long unknown04;
	byte unknown08[0x80 - 0x8];
	s_502424_target target;
	byte unknown98[0xbc - 0x98];
};

struct s_502420_element
{
	byte unknown00[0x10];
	short member_count;
	byte unknown12[0x18 - 0x12];
	long first_actor_index;
	byte unknown1c[0xa];
	short unknown26;
	byte unknown28[0x50 - 0x28];
};

inline s_502424_element *element_502424_get(long index)
{
	return (s_502424_element *)(g_502424->data + (index & 0xffff) * sizeof(s_502424_element));
}

inline s_502420_element *element_502420_get(long index)
{
	return (s_502420_element *)(g_502420->data + (index & 0xffff) * sizeof(s_502420_element));
}

inline s_prop_node_view *prop_node_get(long index)
{
	return (s_prop_node_view *)(g_502418->data + (index & 0xffff) * sizeof(s_prop_node_view));
}

inline s_prop_view_fields *prop_node_view(s_prop_node_view *node)
{
	return (s_prop_view_fields *)function_25d740((s_prop_node *)node);
}

/* a target of the actor: a point or an object */
union u_actor_target
{
	point3f point;
	vector3f vector;
	long object_index;
};

/* a point the actor moves to or aims at (16 bytes) */
typedef s_type_c3b527 s_actor_point_target;

/* the flags at +0x314 of the actor */
struct s_actor_flags314
{
	dword bit0 : 1;
	dword bit1 : 1;
	dword unknown : 30;
};

/* an entry of the actor's points at +0x53c (lane M's 1ada70) */
struct s_actor_point_entry
{
	byte unknown00[0xc];
	s_type_c3b527 point;
};

/* the actor (0x888 bytes, g_4f55f0) as the slot handlers see it */
struct s_actor_view
{
	// BEGIN s_actor_view
	byte unknown000[0x4 - 0x0];
	short unknown004;
	byte unknown006[0x7 - 0x6];
	bool unknown007;
	byte unknown008;
	bool unknown009;
	byte unknown00a[0x18 - 0xa];
	long unknown018;           /* the unit */
	byte unknown01c[0x20 - 0x1c];
	long unknown020;
	short unknown024;
	byte unknown026[0x30 - 0x26];
	long unknown030;
	byte unknown034[0x40 - 0x34];
	bool unknown040;
	byte unknown041[0x54 - 0x41];
	long unknown054;           /* the character */
	long first_prop_index;
	byte unknown05c[0x7c - 0x5c];
	long unknown07c;
	long next_index;
	short unknown084;
	union
	{
		short unknown086;
		byte unknown086_byte;
	};
	byte unknown088[0x90 - 0x88];
	s_slot slots[4];
	short current;
	byte unknown192[0x194 - 0x192];
	s_slot_memory_entry memory[4];
	byte unknown1c4[0x1e8 - 0x1c4];
	long times[14];            /* +0x1f4 (times[3]) and +0x1f8 (times[4]): lane M's 1f4 and 1f8 */
	bool unknown220;
	bool unknown221;
	byte unknown222;
	bool unknown223;
	byte unknown224;
	bool unknown225;
	bool unknown226;
	bool unknown227;
	bool unknown228;
	byte unknown229;
	bool unknown22a;
	byte unknown22b[0x238 - 0x22b];
	point3f position;
	byte unknown244[0x258 - 0x244];
	vector3f unknown258;
	bool unknown264;
	byte unknown265;
	byte unknown266;
	byte unknown267;
	byte unknown268;
	byte unknown269;
	byte unknown26a[2];
	long unknown26c;
	short unknown270;
	byte unknown272[0x274 - 0x272];
	long unknown274;
	byte unknown278[0x27c - 0x278];
	s_location_view unknown27c;
	vector3f unknown290;
	byte unknown29c[0x2d0 - 0x29c];
	real unknown2d0;
	real unknown2d4;
	byte unknown2d8[0x2e8 - 0x2d8];
	long unknown2e8;
	short unknown2ec;
	byte unknown2ee[0x2f0 - 0x2ee];
	bool unknown2f0;
	byte unknown2f1;
	short unknown2f2;
	byte unknown2f4[0x2f8 - 0x2f4];
	long unknown2f8;
	long unknown2fc;
	long unknown300;
	byte unknown304[0x314 - 0x304];
	s_actor_flags314 unknown314;
	byte unknown318[0x31c - 0x318];
	short unknown31c;
	short unknown31e;
	byte unknown320[0x328 - 0x320];
	short unknown328;
	byte unknown32a[0x338 - 0x32a];
	long prop_index;
	byte unknown33c[0x344 - 0x33c];
	long unknown344;
	long unknown348;
	byte unknown34c[0x354 - 0x34c];
	bool unknown354;
	byte unknown355[0x358 - 0x355];
	short unknown358;
	byte unknown35a[0x35e - 0x35a];
	bool unknown35e;
	byte unknown35f[0x360 - 0x35f];
	long unknown360;
	byte unknown364[0x368 - 0x364];
	long unknown368;
	real unknown36c;
	point3f unknown370;
	vector3f unknown37c;
	byte unknown388[0x398 - 0x388];
	real unknown398;
	real unknown39c;
	byte unknown3a0[0x3b0 - 0x3a0];
	short unknown3b0;
	byte unknown3b2[0x3b4 - 0x3b2];
	long unknown3b4;
	bool unknown3b8;
	byte unknown3b9[0x3bc - 0x3b9];
	real unknown3bc;
	vector3f unknown3c0;
	long unknown3cc;
	short unknown3d0;
	short unknown3d2;
	byte unknown3d4[0x3d8 - 0x3d4];
	real unknown3d8;
	byte unknown3dc[0x3e0 - 0x3dc];
	short unknown3e0;
	short unknown3e2;
	byte unknown3e4[0x3e8 - 0x3e4];
	long unknown3e8;
	byte unknown3ec[0x3f0 - 0x3ec];
	bool unknown3f0;
	bool unknown3f1;
	bool unknown3f2;
	byte unknown3f3[0x3fe - 0x3f3];
	short unknown3fe;
	s_reference_entry unknown400[4];
	s_reference unknown418;
	short unknown41c;
	byte unknown41e[0x420 - 0x41e];
	short unknown420;
	byte unknown422[0x424 - 0x422];
	u_actor_target unknown424;
	short unknown430;
	byte unknown432[0x434 - 0x432];
	short unknown434;
	byte unknown436[0x438 - 0x436];
	u_actor_target unknown438;
	short unknown444;
	byte unknown446[0x449 - 0x446];
	bool unknown449;
	bool unknown44a;
	byte unknown44b[0x44d - 0x44b];
	bool unknown44d;
	byte unknown44e[0x450 - 0x44e];
	dword unknown450;
	byte unknown454[0x456 - 0x454];
	bool unknown456;
	byte unknown457[0x458 - 0x457];
	vector3f unknown458;
	bool unknown464;
	bool unknown465;
	bool unknown466;
	byte unknown467[0x468 - 0x467];
	s_type_c3b527 unknown468;
	byte unknown478[0x480 - 0x478];
	bool unknown480;
	byte unknown481;
	bool unknown482;
	bool unknown483;
	bool unknown484;
	bool unknown485;
	byte unknown486[0x488 - 0x486];
	bool unknown488;
	byte unknown489[0x48c - 0x489];
	bool unknown48c;
	byte unknown48d[0x490 - 0x48d];
	union
	{
		struct
		{
			point3f unknown490;
			short unknown49c;
		};
		s_type_c3b527 unknown490_point;
	};
	bool unknown4a0;
	bool unknown4a1;
	bool unknown4a2;
	byte unknown4a3;
	byte unknown4a4;
	byte unknown4a5;
	byte unknown4a6[0x4a8 - 0x4a6];
	long unknown4a8;
	short unknown4ac;
	bool unknown4ae;
	byte unknown4af[0x4b0 - 0x4af];
	real unknown4b0;
	real unknown4b4;
	s_type_c3b527 unknown4b8;
	long unknown4c8;
	real unknown4cc;
	real unknown4d0;
	bool unknown4d4;
	bool unknown4d5;
	byte unknown4d6[0x4e4 - 0x4d6];
	long unknown4e4;
	bool unknown4e8;
	byte unknown4e9[0x504 - 0x4e9];
	short unknown504;
	bool unknown506;
	byte unknown507[0x50c - 0x507];
	bool unknown50c;
	byte unknown50d[0x510 - 0x50d];
	s_type_c3b527 unknown510;
	byte unknown520[0x524 - 0x520];
	real unknown524;
	byte unknown528[0x539 - 0x528];
	char unknown539;          /* the count of unknown53c (lane M's 1ada70) */
	char unknown53a;
	byte unknown53b;
	s_actor_point_entry unknown53c[4];
	long unknown5ac;
	short unknown5b0;
	byte unknown5b2[0x5b4 - 0x5b2];
	short unknown5b4;
	short unknown5b6;
	byte unknown5b8[0x5d0 - 0x5b8];
	bool unknown5d0;
	byte unknown5d1;
	bool unknown5d2;
	bool unknown5d3;
	bool unknown5d4;
	byte unknown5d5;
	short unknown5d6;
	bool unknown5d8;
	byte unknown5d9;
	short unknown5da;           /* ticks the actor has stayed near unknown5dc */
	point3f unknown5dc;
	short unknown5e8;
	byte unknown5ea[0x5ec - 0x5ea];
	vector3f unknown5ec;
	vector3f unknown5f8;
	bool unknown604;
	bool unknown605;
	byte unknown606[0x61e - 0x606];
	short unknown61e;
	short unknown620;
	bool unknown622;
	byte unknown623;
	real unknown624;
	real unknown628;
	real unknown62c;
	real unknown630;
	long unknown634;
	s_type_c3b527 unknown638;
	vector3f unknown648;
	byte unknown654[0x656 - 0x654];
	short unknown656;
	byte unknown658[0x684 - 0x658];
	short unknown684;
	short unknown686;
	byte unknown688[0x6c8 - 0x688];
	long unknown6c8;
	bool unknown6cc;
	byte unknown6cd[0x6d1 - 0x6cd];
	bool unknown6d1;
	bool unknown6d2;
	byte unknown6d3;
	vector3f unknown6d4;
	byte unknown6e0[0x6fa - 0x6e0];
	short unknown6fa;
	dword unknown6fc;
	byte unknown700[0x70a - 0x700];
	short unknown70a;
	byte unknown70c[0x710 - 0x70c];
	long unknown710;
	byte unknown714[0x7c0 - 0x714];
	real unknown7c0;
	byte unknown7c4[0x7f8 - 0x7c4];
	long unknown7f8;
	long unknown7fc;
	byte unknown800[0x810 - 0x800];
	union
	{
		struct
		{
			dword unknown0 : 13;
			dword bit13 : 1;
			dword unknown14 : 18;
		} unknown810;
		dword flags810;
	};
	vector3f unknown814;
	byte unknown820[0x858 - 0x820];
	long unknown858;
	long command_script_index;  /* the command script it runs (actor_behavior_x87.cpp), or NONE */
	byte unknown860[0x888 - 0x860];
	// END s_actor_view
};

/* whether two references are the same (compared as one dword) */
#define REFERENCE_EQUAL(a, b) (*(long *)&(a) == *(long *)&(b))

/* the ai's scratch buffers (ai.cpp) */
byte *ai_scratch_buffer_get(void);
void ai_scratch_buffer_release(byte *address);

inline s_actor_view *actor_get(long actor_index)
{
	return (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));
}

/* walks the actors of an element of 0x502420 from its first_actor_index
   through each actor's next_index; retail tests the next index at the loop
   head (0x1bb3a0) and keeps index below next_index on the stack (0x1b6740) */
struct s_element_502420_member_iterator
{
	long index;
	long next_index;
};

inline s_actor_view *element_502420_member_next(s_element_502420_member_iterator *iterator)
{
	s_actor_view *member = 0;

	if (iterator->next_index != NONE)
	{
		member = actor_get(iterator->next_index);
		iterator->index = iterator->next_index;
		iterator->next_index = member->next_index;
	}
	return member;
}

/* clears the actor's state at 0x4ac..0x5b6 (inlined by several start
   callbacks) */
inline void actor_reset_state(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown50c = false;
	actor->unknown5ac = NONE;
	actor->unknown5b0 = NONE;
	actor->unknown5b4 = 0;
	actor->unknown5b6 = 0;
	actor->unknown4ac = 0;
	actor->unknown504 = 0;
}

/* callees of both lane B (0x1b0000..0x1bffff) and lane C (0x1c0000..0x1cffff)
   not decompiled yet; src/stubs/lane_b.cpp defines them */

long function_c8f60(long unit_index, short seat_index);
bool __stdcall function_110ab0(long unit_index);
bool __stdcall function_1f4810(long actor_index, long prop_index, real distance, bool unknown);
bool function_25ab50(long reference);
void __stdcall function_2628f0(long actor_index, s_reference reference);
void function_26c180(long actor_index);

#endif
