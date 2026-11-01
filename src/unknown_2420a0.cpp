#include <math.h>
#include <new.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "bit_vector.h"
#include "unknown_19ec40.h"
#include "unknown_1efac0.h"
#include "unknown_2420a0.h"
#include "unknown_13927e.h"
#include "unknown_1523c0.h"
#include "game_engine_events.h"

// @flags /O2 /arch:SSE /Gr
#include "view_context_storage.h"

/* ---- globals ---- */
s_slot_table *g_51ec80;

/* the two colors (alpha, then red, green and blue) of the markers drawn by
   function_243ed0 */
color4f g_468c80[2] =
{
	{ 1.0f, 0.4588235318660736f, 0.729411780834198f, 1.0f },
	{ 1.0f, 0.8078431487083435f, 0.5607843399047852f, 0.8705882430076599f },
};

/* ---- views of other data ---- */
struct s_slot_object
{
	long definition_index;
	byte unknown04[0xaa - 0x04];
	byte object_type;
	byte unknownab[0xc2 - 0xab];
	short sc2;
	long lc4;
	long lc8;
	byte unknowncc[0x13c - 0xcc];
	long player_index;
	byte unknown140[0x17e - 0x140];
	short slot;
	byte unknown180[0x1fc - 0x180];
	short s1fc;
	byte unknown1fe[0x212 - 0x1fe];
	char field_x11c898;
};

struct s_slot_object_header
{
	byte unknown00[8];
	s_slot_object *object;
};

struct s_player_view
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x88 - 0x30];
	byte type;
	byte unknown89[0xc0 - 0x89];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

/* the options of the engine (the game options, g_4e6948) */
struct s_ctf_options
{
	byte unknown00[0xc];
	char mode;
	byte unknown0d[0x180 - 0xd];
	long engine_type;
	byte unknown184[0x22c - 0x184];
	union
	{
		dword flags;
		struct
		{
			dword bit0 : 1;
			dword bit1 : 1;
			dword bit2 : 1;
			dword bit3 : 1;
			dword bit4 : 1;
			dword bit5 : 1;
			dword bit6 : 1;
			dword bit7 : 1;
			dword unknown : 24;
		} bits;
	} flags22c;
	short s230;
	short s232;
	long carrier_speed;
	long l238;
	byte unknown23c[4];
	long team_mode;
	short scale_a;
	short scale_b;
	byte unknown248[0x1128 - 0x248];
	bool b1128;
};

static inline s_ctf_options *ctf_options()
{
	return (s_ctf_options *)g_4e6948;
}

static inline s_slot_object *slot_object_get(long object_index)
{
	return ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

static inline s_player_view *ctf_player_get(long player_index)
{
	return (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_player_view));
}

void function_b58c0(long index, dword mask);

/* the engine's globals changed: tell the clients */
static inline void ctf_globals_changed(dword mask)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
		function_b58c0(g_4e9ae8->value24, mask);
}

/* rounds as the x87 does */
static __forceinline long ctf_real_to_long(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

struct s_team_entry
{
	long object_index;
	short team;
	byte unknown06[2];
	long l8;
	long lc;
};

long player_find_unit_item_of_type(long, long);
bool player_iterator_next(long *iterator);
void function_1967d0(long a, long c, long b, long delta);
void function_1970a0(long b, long a, long delta);
void function_197210(long player_index, long value14, long value10);
s_event *game_engine_event_initialize(long type, long subtype, s_event *event);
void game_engine_event_set_cause_player(long player_index, s_event *event);
point3f *function_b9dd0(long object_index, point3f *result);
s_team_entry *function_15e410(short team);
bool function_161e10(long team);
long function_161eb0(long team);
bool function_15b2f0();
short function_101280(long object_index);
long function_cbd50(long object_index, short index);

/* callees not decompiled yet (stubs in src/stubs/lane_o.cpp) */
short function_158990(long team);
void __stdcall function_a7810(dword mask);
void __stdcall function_a7870(long object_index);
struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *data);
void function_15e050(long object_index, short value);
void function_15e130(long object_index);
void __stdcall function_b8540(long a);
void function_157670();
void function_15e4d0();
color3f *function_7f720(color3f *color, short team_index);
bool function_bacc0(long object_index, long index, point3f const *point);
bool function_138860();
bool function_138880();
void function_15fe70(long player_index);
void function_15b3a0(long team, bool a);

/* ---- the game engine class whose vtable is at 0x459d18 ---- */
struct s_marker_update;

struct s_ctf_query
{
	byte unknown00[0xc];
	dword flags;
};

class c_game_engine_markers
{
public:
	virtual long v0();
	virtual bool v1();
	virtual void v2() {}
	virtual void v3() {}
	virtual bool v4(long index);
	virtual void v5() {}
	virtual void v6(long player_index);
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual void v10() {}
	virtual void v11() {}
	virtual void v12();
	virtual void v13(long arg_0);
	virtual void v14(long arg_0);
	virtual void v15(long player_index) {}
	virtual bool v16(long player_index, long object_index);
	virtual void v17(long player_index, long object_index) {}
	virtual void v18();
	virtual real v19(long player_index);
	virtual long v20(long object_index);
	virtual void v21(long object_index);
	virtual void v22() {}
	virtual void v23(long object_index, long unit_index);
	virtual void v24(long object_index, long unit_index);
	virtual long v25(long ticks, bool a, bool b);
	virtual long v26();
	virtual void v27() {}
	virtual void v28() {}
	virtual void v29() {}
	virtual void v30(long killer, long victim, bool suicide, long);
	virtual long v31(long killer, long victim, bool suicide);
	virtual void v32() {}
	virtual bool v33(long player_index, s_ctf_query const *query);
	virtual bool v34(long value);
	virtual bool v35(long player_index, long type);
	virtual void v36(long value);
	virtual long v37(long player_index, byte *flag);
	virtual void v38() {}
	virtual void v39() {}
	virtual bool v40();
	virtual void v41(s_marker_update *update) {}
	virtual void v42(dword mask, dword *changed, s_marker_update *update) {}
	virtual byte v43(dword mask, s_marker_update *update) { return 0; }
	virtual void v44(long, s_marker_update *update);
	virtual void v45(dword *flags, long, s_marker_update *update);
	virtual bool v46(dword flags, long, s_marker_update *update);
	virtual void v47() {}
	virtual void v48() {}
	virtual void v49() {}
	virtual void v50() {}
};

/* the copy of the slot table a client keeps */
struct s_marker_update
{
	byte unknown00[0x24];
	long l24;
	long l28;
	short a[9];
	short b[9];
	byte c[9];
	byte unknown59[3];
	s_long_triple triples[3];
	byte b80;
	byte unknown81[3];
};

// @retail 0x2420a0
bool c_game_engine_markers::v4(long index)
{
	long none = NONE;

	if (g_51ec80->a[index] == none && g_51ec80->b[index] == none)
		return false;
	return true;
}

// @retail 0x2420d0
long c_game_engine_markers::v37(long player_index, byte *flag)
{
	s_player_view *player = (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long result = NONE;

	*flag = 1;
	if (*((byte *)player + 0xc0) != 0xff)
	{
		if (player_find_unit_item_of_type(player_index, 1) != NONE)
			result = (g_4e6948->mode_180 == 9) * 8 + 0xb;
	}

	return result;
}

// @retail 0x242140
real function_242140(long object_index)
{
	s_slot_object *object = ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	real result = 0.0f;
	long slot = object->slot;
	short scale = g_4e6948->scale_a;
	real divisor = (real)(scale == 0 ? 1 : scale);

	if (slot != NONE)
	{
		if (g_51ec80->flags[slot] & 8)
		{
			result = 1.0f;
		}
		else
		{
			short count = g_51ec80->e[slot];
			long n = count > 0 ? count : 0;

			result = g_510c54->rate / divisor * (real)n;
		}
	}

	return result;
}

/* the slot (0..8) whose first or second identifier is the value, or 9 */
PRIVATE long slot_find(long value)
{
	long i;

	for (i = 0; i < 9; i++)
	{
		if (g_51ec80->b[i] == value || g_51ec80->c[i] == value)
			break;
	}

	return i;
}

PRIVATE long slot_name(long slot)
{
	if (slot < 9)
		return (short)g_4e9ae8->name.c[slot];
	return NONE;
}

// @retail 0x242b60
long function_242b60(long value)
{
	return slot_name(slot_find(value));
}

// @retail 0x243400
void c_game_engine_markers::v45(dword *flags, long, s_marker_update *update)
{
	dword changed = 0;
	dword mask = *flags & 0x1f;
	s_slot_table *g;
	long i;

	if (mask)
		v42(mask, &changed, update);

	g = g_51ec80;
	if (*flags & 0x20)
	{
		long value = g->l1f4;

		if (update->l24 != value)
		{
			update->l24 = value;
			changed |= 0x20;
		}
	}

	if ((*flags >> 7) & 1)
	{
		for (i = 0; i < 9; i++)
		{
			short value = g->d[i];

			if (update->a[i] != value)
			{
				update->a[i] = value;
				changed |= 0x80;
			}
		}
	}

	if (*flags & 0x100)
	{
		for (i = 0; i < 9; i++)
		{
			short value = g->e[i];

			if (update->b[i] != value)
			{
				update->b[i] = value;
				changed |= 0x100;
			}
		}
	}

	if (*flags & 0x200)
	{
		for (i = 0; i < 9; i++)
		{
			byte value = g->flags[i];

			if (update->c[i] != value)
			{
				update->c[i] = value;
				changed |= 0x200;
			}
		}
	}

	if (*flags & 0x400)
	{
		i = 0;
		do
		{
			long a = g->triples[i].a == NONE ? NONE : g->triples[i].a & 0xffff;

			if (update->triples[i].a != a)
			{
				update->triples[i].a = a;
				changed |= 0x400;
			}
			long b = g->triples[i].b == NONE ? NONE : g->triples[i].b & 0xffff;
			if (update->triples[i].b != b)
			{
				update->triples[i].b = b;
				changed |= 0x400;
			}
			long c = g->triples[i].c == NONE ? NONE : g->triples[i].c & 0xffff;
			if (update->triples[i].c != c)
			{
				update->triples[i].c = c;
				changed |= 0x400;
			}
			i++;
		}
		while (i < 3);
	}

	*flags = changed;
}

/* true when a marker with these flags applies in the current team mode */
#define MARKER_APPLIES(flags, options) \
	(((flags) & 1) && (options)->team_mode != 1 && (options)->team_mode != 2 || \
	 ((flags) & 2) && (options)->team_mode == 1 || \
	 ((flags) & 4) && (options)->team_mode == 2)

// @retail 0x243b10
long function_243b10(long key_b, bool team_only, long *second)
{
	s_game_options_view *options = g_4e6948;
	long results[8];
	long key_a;

	if (team_only)
		key_a = options->mode_180 != 9 ? 0 : 2;
	else
		key_a = (options->mode_180 == 9) * 2 + 1;

	long count = function_19ec40(0, 0.0f, (short)key_a, (short)key_b, 0, 8, results, 0.0f);
	long result = NONE;
	long i = 0;

	if (count > 0)
	{
		do
		{
			long index = results[i];
			s_marker_entry *entry = g_4e0350->marker_entries + index;

			if (MARKER_APPLIES(entry->flags, options))
			{
				if (result == NONE)
				{
					result = index;
				}
				else
				{
					if (second)
						*second = results[i];
					break;
				}
			}
			i++;
		}
		while (i < count);
	}

	return result;
}

// score 173 (was 173): frame 0x40 against retail's 0x44. Retail keeps the slot table in EBP through
// the inner loop (re-reading g_51ec80 when EBP is borrowed, and in the marker select's else arm) and
// keeps the inner index on the stack; ours re-reads the table at its use and keeps the index in EAX.
// Retail's x87 distance squares dx first and pops it there; ours squares dy first.
// Round 77 (plain): no table local 200; the select's else arm through the global 200; the result index
// named first 173; the marker position through function_2417a1 (moved above, retail's y-first copy
// shape) 172; the index declared in the for 173; the count named result_count 173.
// Round 8 (plain, on the function_2417a1 copy): the distance through magnitude2d 172; the other
// position read at each use 223 (666 B); the table local at the loop top 169 / 180 (the slot moves to
// the stack, caller 0x240160 lost); the marker select as if/else 188; the index declared first 172.
// Round 87 (plain): the entry as `marker_entries + results[j]` 173, the sum as dy * dy + dx * dx 173,
// a flags local for the three clauses 173, rejected markers `continue` 173 (all four identical code);
// the squared distance accumulated (`d = dx * dx; d += dy * dy;` as 0x1b0cd0's file does) then sqrt
// 173: that gives retail's exact x87 sequence, so keep it in any later fix. Left: retail spills the
// inner index (the frame's extra dword) and keeps the table in EBP through the inner loop; ours keeps
// the index in EAX and folds the entry flags load to `[ecx + ebp + 0x16]`, borrowing EBP.
// Round 89 (plain, each with the accumulated distance): 0x243b10's `if (count > 0) do ... while`
// loop 186; an options local as 0x243b10 has 171 (clause code identical, only the key's registers
// move); the select through the global with the table local after it 169 at 717 B (the slot moves
// to the stack, caller 0x240160 lost); the index renamed 171 (identical); ctf_options() in the key
// and the marker test 173; the position through marker_position() 173; the table at each use 200.
// Retail loads the team mode into EBP while EAX still holds the options pointer (ours coalesces both
// in EBP), so EAX looks busy in retail's clause 1 too; the index spill is the cause to find.
// Round 71 (plain, each on the accumulated distance, which alone stays 173): the select's else arm
// through the global 200; both select arms through the global 200; the inner index and count
// declared at function scope 173; the table local assigned at the outer loop top 169 (the slot
// parameter moves to the stack and caller 0x240160 is lost). Retail also keeps count in memory
// from its definition (stored straight from EAX) and stores j = 0 at once.
// Round 111 (fast; a fast image of this function alone builds 0x19ec40 differently, so include
// 0x1e5bb0/0x1e2a90/0x29b00): accumulated distance through a distance2d helper 173; ten placements
// of the j/key/count/point declarations 173 (193-195 with the point read before the query); named
// result index 173; table local inside the `marker_index != NONE` block 169 at 717 B, but matched
// caller 0x240160 drops to 46. Original restored.
// Round 176 (fast, each on the accumulated distance, which alone stays 173): an options local per inner
// iteration 173 (identical); a short marker index 180; the array selected by k, then indexed, 204
// (caller 0x240160 lost); the y-first copy helper function_2417a1 172 (the copy is then retail's; the
// count stays in ECX and the index in EAX where retail keeps the count in EAX and spills the index);
// sqrtf 173. The table local at the loop top or in the marker block (169, 717 B) hoists the table to
// EDI and moves the slot to EBP from the stack, so it is not retail's allocation either.
// Round 45 (lane 11, plain; a fast image builds 0x19ec40 differently here, 203 for this body): the distance's
// squares named (`dx_squared`, `dy_squared`) 173; the delta as a vector2f through dot_product2d 173; a named
// squared distance 173 (all identical code); the x maximum written bound-first like y and z 152 (701 bytes, a
// positional gain only: the x compare then loads the bound into a register where retail compares the distance
// with the memory bound, so not kept), with the squares reversed 152, with the accumulated distance 152.
// Round 126 (m3 lane 6, fast with 0x1e5bb0/0x1e2a90/0x29b00/0x19ec40 in the image; alone it builds 203 at 690 B),
// all on the accumulated distance (aligned difference 58 -> 53, still 173): the inner index declared in its for 173,
// at the marker block's top 173; with the y-first copy helper function_2417a1 for the marker position 172, plus the
// index in the for 172 or at the block top 172; the table local at the loop top with the select's else arm through
// the global 169 (717 B, caller 0x240160 46). Retail's count stays in EAX and is stored straight to its slot; the
// spilled inner index and the EBP table remain.
// Round 186 (fast, with 0x1e5bb0/0x1e2a90/0x29b00 in the image): a loop-invariant marker-entries pointer
// read before the inner loop 170 (702 bytes; frame 0x44 as retail and the first difference moves to +0x20,
// but the pointer takes the spilled slot retail gives the inner index, and the index stays in EAX), with the
// accumulated distance 170, with an options local for the clauses 170, with both 170; the table read through
// the global at each use with the accumulated distance 200 (688 bytes). Retail keeps the table in EBP across
// the inner loop and reuses it after the 0x19ec40 call (LTCG knows the call leaves g_51ec80 alone); ours
// reloads the table for the x updates and borrows EBP for the entries base, so the index keeps a register.
// @retail 0x243c00
void function_243c00(long slot)
{
	s_slot_table *slots = g_51ec80;
	long k;

	for (k = 0; k < 2; k++)
	{
		long marker_index = k == 0 ? slots->b[slot] : slots->c[slot];

		if (marker_index != NONE)
		{
			long results[8];
			long key = (g_4e6948->mode_180 == 9) * 2 + 1;
			long count = function_19ec40(0, 0.0f, (short)key, (short)slot, (short)(k + 1), 8, results, 0.0f);
			point3f point = g_4e0350->marker_entries[marker_index].position;
			long j;

			if (k == 0)
			{
				slots->bounds[0][slot].x = 0.5f;
				slots->bounds[0][slot].y = 0.0f;
				slots->bounds[0][slot].z = 0.0f;
			}
			else
			{
				slots->bounds[1][slot].x = 0.5f;
				slots->bounds[1][slot].y = 0.0f;
				slots->bounds[1][slot].z = 0.0f;
			}

			for (j = 0; j < count; j++)
			{
				s_marker_entry *other = &g_4e0350->marker_entries[results[j]];

				if (MARKER_APPLIES(other->flags, g_4e6948))
				{
					point3f other_point = other->position;
					real dx = point.x - other_point.x;
					real dy = point.y - other_point.y;
					real distance = (real)sqrt(dx * dx + dy * dy);

					if (k == 0)
					{
						slots->bounds[0][slot].x = distance > slots->bounds[0][slot].x ? distance : slots->bounds[0][slot].x;
						slots->bounds[0][slot].y = slots->bounds[0][slot].y > point.z - other_point.z ? slots->bounds[0][slot].y : point.z - other_point.z;
						slots->bounds[0][slot].z = slots->bounds[0][slot].z > other_point.z - point.z ? slots->bounds[0][slot].z : other_point.z - point.z;
					}
					else
					{
						slots->bounds[1][slot].x = distance > slots->bounds[1][slot].x ? distance : slots->bounds[1][slot].x;
						slots->bounds[1][slot].y = slots->bounds[1][slot].y > point.z - other_point.z ? slots->bounds[1][slot].y : point.z - other_point.z;
						slots->bounds[1][slot].z = slots->bounds[1][slot].z > other_point.z - point.z ? slots->bounds[1][slot].z : other_point.z - point.z;
					}
				}
			}

			if (k == 0)
			{
				slots->bounds[0][slot].y += 0.1f;
				slots->bounds[0][slot].z += 0.9f;
			}
			else
			{
				slots->bounds[1][slot].y += 0.1f;
				slots->bounds[1][slot].z += 0.9f;
			}
		}
	}
}

/* ---- the marker list ---- */
PRIVATE s_color_bits *marker_color()
{
	color4f *color = &g_468c80[0];

	if (g_4b9ed8 != NONE)
	{
		long player_index = g_4e8c20->entries[g_4b9ed8];

		if (player_index != NONE)
		{
			s_player_view *player = (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);

			if (player->type == 1 || player->type == 3)
				color = &g_468c80[1];
		}
	}

	return (s_color_bits *)&color->red;
}

PRIVATE __forceinline s_color_bits *function_243ed1()
{
    s_color_bits *local_0 = (s_color_bits *)((byte *)g_468c80 + 4);
    if (g_4b9ed8 != NONE)
    {
        long local_1 = g_4e8c20->entries[g_4b9ed8];
        if (local_1 != NONE)
        {
            s_player_view *local_2 = (s_player_view *)(g_4e8c24->data + (local_1 & 0xffff) * 0x21c);
            if (local_2->type == 1 || local_2->type == 3)
                local_0 = (s_color_bits *)((byte *)(g_468c80 + 1) + 4);
        }
    }
    return local_0;
}

#pragma inline_depth(1)
// @retail 0x243ed0
bool function_243ed0(s_marker_list *list, point3f const *position)
{
	list->b0 = 1;
	list->b1 = 0;
	list->l4 = 1;
	list->position = *position;
	list->r14 = 0.0f;
	list->r18 = 0.1f;
	list->r1c = 0.0f;
	list->l20 = NONE;
	list->color24 = *function_243ed1();
	list->color30 = *function_243ed1();
	list->r3c = 1.0f;
	list->r40 = 1.0f;
	list->count = 1;
	list->items[0].a = *function_243ed1();
	list->items[0].b = *function_243ed1();
	list->items[0].kind = 0;
	list->items[0].r = 1.0f;
	list->items[0].index = NONE;
	return true;
}
#pragma inline_depth(255)

// @retail 0x2440a0
bool function_2440a0(s_marker_list *list, long object_index, point3f const *point)
{
	s_slot_object *object = ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;

	if (list->count < 2 && object->slot != NONE)
	{
		s_game_options_view *options = g_4e6948;
		long divisor = options->divisor == 0 ? 1 : options->divisor;
		long scale = options->scale_b == 0 ? 5 : options->scale_b;
		long value = NONE;

		list->items[list->count].a = *(s_color_bits *)point;
		list->items[list->count].b = *(s_color_bits *)point;
		list->items[list->count].kind = (options->mode_180 == 9) + 1;
		list->items[list->count].index = NONE;

		short slot = object->slot;
		if (slot >= 0 && slot < 9)
			value = g_51ec80->d[slot];

		if (options->mode_180 == 9 && (g_51ec80->flags[slot] & 2))
			list->items[list->count].r = value == NONE ? 0.0f : (real)value / (real)scale;
		else
			list->items[list->count].r = value == NONE ? 0.0f : (real)value / (real)divisor;

		list->b0 = 1;
		list->count++;
	}

	return true;
}

/* ---- the object whose destructor is at 0x244930 ---- */
#include "unknown_244980.h"

// @retail 0x244930 deleting c_marker_object

// @retail 0x244bb0
c_marker_object *c_marker_object::create(dword value, void *memory)
{
	return new (memory) c_marker_object(value, value0c);
}

/* ---- the list of lines drawn by a marker ---- */
struct s_line_a
{
	long a;
	long b;
	long c;
	byte d;
	byte e;
	short f;
	point3f position;
	real r1c;
};

struct s_line_b
{
	long a;
	long b;
	long c;
	byte d;
	byte e;
	short f;
	point3f position;
	real r1c;
	real r20;
	real r24;
	real r28;
};

struct s_line_list
{
	short count_a;
	short count_b;
	byte unknown04[4];
	s_line_a lines_a[0x100];
	s_line_b lines_b[0x100];
};

PRIVATE inline void function_244d51(point3f *arg_0, real arg_1, real arg_2, real arg_3)
{
	arg_0->x = arg_1;
	arg_0->y = arg_2;
	arg_0->z = arg_3;
}

// @retail 0x244ca0
void function_244ca0(real height, real radius, s_line_list *list, long a, long b, long c, byte d, byte e, point3f const *position, short f)
{
	if (list->count_a < 0x100)
	{
		s_line_a *line = &list->lines_a[list->count_a++];

		line->a = a;
		line->b = b;
		line->d = d;
		line->e = e;
		line->c = c;
		line->f = f;
		line->position = *position;
		line->r1c = radius;
	}

	if (height > 0.0f)
	{
		real z = position->z - height;

		if (list->count_a < 0x100)
		{
			s_line_a *line = &list->lines_a[list->count_a++];

			line->a = a;
			line->b = b;
			line->c = c;
			line->d = d;
			line->e = e;
			line->f = f;
			function_244d51(&line->position, position->x, position->y, z);
			line->r1c = radius;
		}

		if (list->count_b < 0x100)
		{
			s_line_b *line = &list->lines_b[list->count_b++];

			line->a = a;
			line->b = b;
			line->c = c;
			line->d = d;
			line->e = e;
			line->f = f;
			function_244d51(&line->position, position->x, position->y, z);
			line->r1c = 0.0f;
			line->r20 = 0.0f;
			line->r24 = height;
			line->r28 = radius;
		}
	}
}

/* ---- the engine's handlers (lane O) ---- */

/* the multiplayer globals tag (the definitions of the flag and the bomb, and
   the engine's constants) */
struct s_ctf_constants
{
	byte unknown00[0xc4];
	long lc4;
	byte unknownc8[0xd8 - 0xc8];
	real pickup_radius;
};

struct s_mp_globals_block
{
	byte unknown00[4];
	long flag_definition_index;
	byte unknown08[0x3c - 0x08];
	long bomb_definition_index;
	byte unknown40[0x534 - 0x40];
	s_ctf_constants *constants;
};

struct s_mp_globals_tag
{
	byte unknown00[0xc];
	s_mp_globals_block *block;
};

struct s_ctf_tag_globals
{
	byte unknown00[0x16c];
	long mp_globals_index;
};

static inline s_mp_globals_block *mp_globals_block()
{
	long index = ((s_ctf_tag_globals *)g_4e034c)->mp_globals_index;

	return ((s_mp_globals_tag *)g_4e3b44[index & 0xffff].bytes)->block;
}

/* the team table of the multiplayer globals */
struct s_ctf_team_view
{
	byte unknown00[0x10];
	short slot_teams[9];
	byte unknown22[0xc18 - 0x22];
	long team_count;
	s_team_entry teams[1];
};

/* the object placement data of 0xb7930/0xb7b40 */
struct s_object_placement
{
	byte unknown00[0x1c];
	point3f position;
	byte unknown28[0xc4 - 0x28];
};

/* the iterator over the players of 0x19f300 */
struct s_player_iterator
{
	s_player_view *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

bool player_iterator_next_with_unit(long *iterator);

static inline real distance_sq3f(point3f const *a, point3f const *b)
{
	vector3f v;

	vector3d_from_points3d(a, b, &v);
	return length_sq3f(&v);
}

/* copies of function_15eaf0 and function_161e10, which retail inlines in
   function_242ef0 */
static inline bool ctf_game_has_teams()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = g_4e6948->flags184.bit0;

	return result;
}

static inline bool ctf_team_is_active(long team)
{
	bool result = false;

	if (ctf_game_has_teams() && team >= 0 && team < 8)
		result = (g_4e9ae8->wc & (1 << team)) != 0;

	return result;
}

static inline point3f *marker_position(long marker_index)
{
	s_marker_entry *entry = &g_4e0350->marker_entries[marker_index];
	return &entry->position;
}

long function_2421d0(long team);
long function_242210(point3f const *point);
void function_2422d0(long slot);
bool function_2423f0();
long function_242510(long object_index);
struct s_242ef1 { point3f const *field_0; };
long function_242ef0(s_242ef1 arg_0, long team);
void function_2430a0(long player_index, bool flag);
void function_2431a0();
void function_243250(long player_index);
void function_2432e0(long team);
void function_243a20(long object_index);
bool function_243a80(long slot);
bool function_244300(long object_index, long *player_index);
bool function_244680(long team, long unit_index);
bool function_2447f0();

// @retail 0x240140
long c_game_engine_markers::v0()
{
	return ctf_options()->engine_type == 9 ? 9 : 1;
}

// @retail 0x240160
bool c_game_engine_markers::v1()
{
	s_slot_table *table = (s_slot_table *)&g_4e9ae8->stats;
	long i;

	memset(table, 0, sizeof(s_slot_table));
	g_51ec80 = table;

	for (i = 0; i < 9; i++)
		table->c[i] = NONE;

	for (i = 0; i < 9; i++)
	{
		long second = NONE;

		table->a[i] = (short)function_243b10(i, true, NULL);
		table->b[i] = (short)function_243b10(i, false, &second);
		table->c[i] = (short)second;
		function_243c00(i);
	}

	table->l38 = function_243b10(NONE, true, NULL);
	table->initialized = true;

	for (i = 0; i < 9; i++)
		table->d[i] = NONE;

	for (i = 0; i < 9; i++)
		table->objects[i] = NONE;

	for (i = 0; i < 3; i++)
	{
		table->triples[i].a = NONE;
		table->triples[i].b = NONE;
		table->triples[i].c = NONE;
	}

	return true;
}

// @retail 0x240290
void c_game_engine_markers::v12()
{
	if (ctf_options()->mode != 4)
	{
		if (ctf_options()->team_mode == 1)
		{
			g_51ec80->l1f4 = function_161eb0(NONE);
			ctf_globals_changed(0x20);
		}

		for (long i = 0; i < 9; i++)
		{
			if (function_243a80(i))
				function_2422d0(i);
		}
	}
}

#pragma inline_depth(1)
// @retail 0x240300
void c_game_engine_markers::v6(long player_index)
{
	s_player_view *player = ctf_player_get(player_index);

	if (ctf_options()->mode != 4 && *(volatile char const *)&player->team != NONE)
	{
		s_event event;

		game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, 0);
		event.a = player_index;
		game_engine_event_send_inline(&event);

		if (ctf_options()->team_mode == 1)
			function_243250(player_index);
	}
}
#pragma inline_depth(255)


// @retail 0x240e30
real c_game_engine_markers::v19(long player_index)
{
	real result = 1.0f;

	if (player_find_unit_item_of_type(player_index, 1) != NONE)
	{
		switch (ctf_options()->carrier_speed)
		{
		case 1:
			result = 1.0f;
			break;
		case 2:
			result = 1.25f;
			break;
		default:
			result = 0.75f;
			break;
		}
	}

	return result;
}

// @retail 0x240e80
long c_game_engine_markers::v20(long object_index)
{
	long result = NONE;
	s_slot_object *object = slot_object_get(object_index);

	if ((1 << object->object_type) & 4)
	{
		short slot = object->slot;

		if (slot >= 0 && slot < 9)
			result = g_51ec80->objects[slot];
	}

	return result;
}

// @retail 0x240ee0
void c_game_engine_markers::v30(long killer, long victim, bool suicide, long)
{
	if (killer != NONE && victim != NONE && !suicide && player_find_unit_item_of_type(victim, 1) != NONE)
	{
		long absolute_index = killer & 0xffff;
		s_player_view const *local_0 = ctf_player_get(killer);

		if (ctf_options()->engine_type == 9)
		{
			long team = local_0->team;
			function_1967d0(absolute_index, team, 0x14, 1);
			function_1970a0(absolute_index, 0x16, 1);
		}
		else
		{
			long team = local_0->team;
			function_1967d0(absolute_index, team, 0x10, 1);
			function_1970a0(absolute_index, 0x13, 1);
		}
	}
}

// @retail 0x240f90
long c_game_engine_markers::v31(long killer, long victim, bool suicide)
{
	long result = NONE;

	if (killer != result && victim != result && !suicide && player_find_unit_item_of_type(victim, 1) != result)
		result = ctf_options()->engine_type == 9 ? 0x28 : 0x27;

	return result;
}

// @retail 0x240fe0
void c_game_engine_markers::v21(long object_index)
{
	function_243a20(object_index);
}

// score 40 (was 40): the entry swaps the object-header registers (retail: header in EAX, index in ECX),
// and retail materializes the player pointer before the flag test (memory-operand test) and loads the
// team after the engine-type compare, hoisted from both arms. Kept: retail's long slot and both bits
// through byte_flags_set (retail stores the flag byte twice), 80 -> 50; the slot table read through
// the global at each use (retail's EBP table, EDI player), 50 -> 40. Tried: the helper with a short
// slot 103; the long slot alone 94; the flag tested as a byte 50; the player index read before the
// table pointer 50; the team read at each call (retail's merged call setup and player pointer, but the
// options pointer is then kept from entry) 119, plus the table read through the global 119, the player
// pointer first 119, the unit before the slot 119 (428 bytes); through the player getter at each call
// 100. Round 59, on 40: no absolute-index local 93; per-arm team local/char/declared once 119;
// arms swapped 119; object pointer local 130; unit inside the slot test 129; player pointer from the
// absolute index 82..128; unit before slot 120; entry through g_4e6948, type local, byte flag test,
// second player index read directly 40. Round 63: the team read at each call gives retail's middle
// (memory-operand flag test, player pointer before it, team after the type compare) but keeps the
// options pointer in EDX from entry; on it: the slot table in a local 119, arms swapped 119, unit
// first 119, player from the absolute index 82; the mode and type tests joined 40. Round 103: the
// player getter as typed pointer arithmetic 40; the team per arm as in the matched v30 (unfolded
// player data in v30, folded here) 119, with a const player 119, the player before the absolute
// index 119, the mode and type tests joined 119, an early return on the mode 119 or on either test 119.
// Round 149: the absolute index inside the flag test 93; the team read before the flag test 89; the
// flag in a named bool 40; the slot read before the type test 117 (428 bytes); the type test written
// in this function instead of the inlined 0x101280 40 (the entry swap is not from that inlining).
// Round 16 (lane 10): the player pointer before the absolute index 40; on the team-per-call form, the
// mode read through g_4e6948's own view 119 and the absolute index inside the flag block 119; guard
// returns for the mode, type and slot tests 40; the object fetched before the type test as in the
// matched v24 119 (the object is then kept, where retail reloads it).
// Earlier note (40): the entry swaps EAX/ECX for the object index and the header; retail computes the
// player pointer before the flag test with the players' data load unfolded and reloads the options
// pointer at the engine-type compare.
// Round 77: the team at each call again 119 (the options pointer then stays in EDX from entry and the
// type lookup takes ESI/EDI); a short slot 93, with the team at each call 93 (438 bytes); the unit
// looked up before the type test 119, with the team at each call 132; an object pointer after the
// test with the team at each call 130. Round 36, on the kept form: the flag as `flags[slot] & 1` 40
// (identical); the team through the player getter, no player local 40 (identical); the absolute
// index, player and team all inside the flag block 93; a char team 88; the unit through the object
// getter at both player-index reads 113 (439 bytes).
// test with the team at each call 130. Round 19, on the team at each call (its whole difference is
// the options pointer kept in EDX from entry, which moves the type test to ESI/EDI): the flag as a
// byte mask 119; an empty `do {} while (false)` after the type test 119 or before the engine-type
// test 119 (no reload); the unit's player index read at each use 93; the flag in a named bool before
// the player pointer 119. Round 117: the flag as `!(flags[slot] & 1)` 40 (same code); on the team at
// each call: the flag mask 119, the player pointer before the absolute index 119, the engine type read
// into a local before the flag test 119, the unit looked up inside the slot test 129, the absolute index
// written at each call 119, the unit looked up before the slot 119 (428 bytes; the options pointer is
// reloaded as retail and the middle is retail's, but the object is no longer fetched again after the
// type test and the flag is loaded into cl), with the flag mask 119; the unit first alone 120.
// Round 119: the inlined type test's own body (0x101280, matched; also inlined by the matched v24)
// spelled with the index first, a header pointer, one expression or a definition local: 40 each
// (identical code); the second player index read at each use 40. Round 121, on the team at each call
// (the players' data load stays folded while the options pointer holds EDX): the player as an indexed
// typed pointer 119, indexed by the absolute index 120, through a data local 120; the unit's player
// index read before the slot test 73, with the team at each call 119; permuter, 10 tries, none lower.
// Round 33 (fast, agreeing with the full count): only the entry (index/header EAX/ECX) and the middle
// differ. On the team at each call (119 again): the player pointer declared inside the flag block 98
// (the options pointer still held in EDX from entry); the unit and slot both read before the type
// test 129; the player inside the flag block with the unit first 118, with the unit and slot first
// 126; on the kept form the player pointer inside the flag block 40 (same code). Unit first with the
// team at each call 119 and the unit before the type test 132 re-measured as listed.
// Round 90 (fast; the reduced image builds this 437 bytes, so relative only): `1 == ` type test 40
// (identical); team at each call 119-equivalent (117 reduced); that with the type test flipped 117; that
// with the player pointer taken inside the flag block 92; the player-index and flag tests joined in
// one condition with the team at each call 117.
// Round 176 (fast): 0x101280's object fetch (inlined here) as byte-pointer arithmetic 40 and with the
// index term first 40 (identical code); the unused effect-team helper function_2411b1 59. On the team at
// each call (119, 430 B: retail's middle, but the options pointer stays in EDX from entry and the
// header/index swap remains): one options local for the mode test, the compare and the event type 120;
// the player through event_player_get 119.
// Round 20 (lane 15, fast; reduced image 40 here): the team at each call again 119 (the options pointer held in
// EDX from entry; retail cannot hold it because it builds the player pointer in EDX/ECX, table then data, where
// ours folds the data load into the add); on it: the absolute index computed before the player-index test 94
// (432 bytes), the unit's player index read through the object getter at both reads 113 (entry becomes v24's,
// object kept across the type test), `bool away = !flag` after the player pointer 86 (438 bytes; tail and the
// hoisted call setup retail's, but `not cl; test cl, 1` and the EDX hold remain), `bool home = flag; if (!home)` 119.
// Round 23 (lane 15, fast agreeing with the full count): const player pointer 40 (identical); the object in a
// local after the type test with the slot read from it 119 (428 bytes: the object is kept, not re-fetched);
// engine type `== 9` with the arms swapped 48; on the team at each call, the players' data read into a local
// before the absolute index 120 (the data load still folds into the add); the player-index and flag tests
// joined in one condition with the absolute index, player and team inside 93.
// Round 109 (lane 6, fast): permuter 600 s, 10 tries, none below 40; a separate local for the event block's second
// player-index read 40 (same code); team at each call with a const player and the unit looked up before the slot
// 119 (428 bytes); the engine-type arms swapped (`== 9` first) 48; a const player pointer 40 (same code). The
// matched v30 (0x240ee0) keeps the players' data load unfolded with the team per arm; here that form folds it
// into `add eax, [ecx+0x44]`, which leaves EDX free for the options pointer from entry.
// Round 97 (lane 8, fast with 0x2411b0/0x240ee0): the team at each call re-measured 119 (430 bytes: the
// player pointer is then built before the flag test and the flag test takes the memory operand, as
// retail, but the data load folds and the options pointer stays in EDX from entry); the player getter
// at each call, no player local, 100; the player pointer from a re-read `unit->player_index` with the
// team at each call 119 (CSE'd back); the team at each call with the unit fetched again for the event
// block's player index 117 (446 bytes; options still in EDX), with v30's per-arm team and a const player 117.
// Round 7 (m3 lane 18, fast with 0x2411b0/0x240ee0 matched): a g_4e0300 header local held across the type
// test and used for both slot lookups 40 (entry still index EAX, header ECX); the type test's result in
// a short local 40; the absolute index written at each call on the kept form 93; one options local for
// the mode test, the engine-type compare and the event type 46; an early return on the mode only 40.
// Round 113 (lane 6, fast with v24/v30): the object index read once through its own address for the type
// test 40 (same code), with the team at each call 119; one header-array local for the slot and unit fetches
// 104; the event declared at function scope 40 (same code); the team at each call rebuilt (119, 430 bytes:
// retail's flag test and player pointer, the options pointer held in EDX from entry).
// Round 100 (lane 8, one-file harness exact, six re-run as real fast trials): a grid of the team local or the team at
// each call, the unit fetched before the slot, the absolute index after the player pointer, an object local for the
// slot, the unit fetched inside the slot test and a separate cause-player local (48 forms): best 40 (same code); unit
// first 120, object local 119, unit inside the slot test 118, team at each call with unit first 119, with the
// absolute index after, object local and cause local 130, with unit inside and cause local 129; on the team at each
// call: arms swapped 119, one options local for both tests 119, mode and type tests joined 119; slot_object_get
// with the index first, a header-array local or a pointer add 40 each (same code), `(word)` index 109.
// Round 43 (one-file LTCG link of this file with the callee closure, 0x101280's file and the build's inlining
// ballast object, equal to the full listing): the event test with the slot range first about 47 (harness 33
// against 26), the initialized event in a named pointer 40 (same code), the cause player read from the unit
// at both uses 40 (same code); the same three on the team at each call 119 (harness 109, 430 bytes each). The
// matched sibling v24 holds the options pointer in EDX from entry as the team-at-each-call form does here.
// Round 45 (lane 11, fast): matched 0x241620's form (one options local for every engine read, the player through
// event_player_get, the team at each call) 120 (429 bytes); on the team at each call: the unit and its two reads
// renamed (names do not move registers) 119, a slot-table local for the flag test and the slot-block stores 119,
// a const unit pointer 119, a separate local for the event block's player-index re-read 119. Diagnostic: the unit
// first with the team at each call reloads the options as retail but keeps the type test's object pointer (no
// re-fetch of the slot), so the options hold follows the register pressure across the type test.
// Round 126 (m3 lane 6, fast): on the per-arm team (119; the options pointer then held in EDX from entry, so the
// players' data load folds): the type test's result in a short local 119, the players' data named once and indexed
// by the absolute index 120, the event block reading the unit's player index at both uses 119, a char team per arm
// 119; on the kept form the players' data local alone 87.
// Round 186 (fast): on the team at each call, a second player pointer for the else arm's team 95
// (431 bytes: retail's unfolded players-data load, player pointer before the flag test and team after
// the compare, but the options pointer still held in EDX from entry, so the type test takes ESI/EDI),
// with an early return on the mode 95, a second pointer for both teams 119; the object in a local after
// the type test 119 (428 bytes); the unit looked up through the header expression in place 40 (same
// code); the team through a second lookup in the kept form 40 (same code, reduced image).
// Round 108 (lane 8; round 100's one-file LTCG harness still exact, each re-run as a real fast trial): a const unit
// pointer 40 (same code); on the team at each call (119, 430 bytes): a const unit 119, a const player 119, the flag
// read into a named bool before its test 119, the event block's slot range tested before the player index 119, the
// slot read through the typed header array 119. The options pointer keeps EDX from entry in every per-call form.
// Round 59 (fast): 1-8 unused locals at the top (constant or parameter copies; diagnostic) change nothing
// on this body or on the team at each call; a slot-table local as in v24 (`g = g_51ec80`) 50, with the
// team at each call 119; v30's per-arm team with a const player 119; early returns for the mode and the
// type test 40 (same code), with the team at each call 119.
// Round 64 (lane 9, fast): declaration order only: the unit declared before the slot 40, player_index
// declared at the function's top 40, absolute_index at the type block's top 40 (same code each); on the team
// at each call: the unit declared first 119, player_index at the top 119; the player getter at each call
// without a player local 100 (re-measured). Diagnostic: the slot read through the unit index (no shared
// object-index CSE) 117 still loads the index into EAX. A compile-only build with 0x101280's body pasted in
// as an inline reproduces the entry swap and keeps it with the header in a local, the index masked into a
// local, or without the mode test.
// @retail 0x240ff0
void c_game_engine_markers::v23(long object_index, long unit_index)
{
	if (ctf_options()->mode != 4)
	{
		if (function_101280(object_index) == 1)
		{
			long slot = slot_object_get(object_index)->slot;
			s_slot_object *unit = slot_object_get(unit_index);

			if (slot != NONE)
			{
				long player_index = unit->player_index;

				if (player_index != NONE)
				{
					long absolute_index = player_index & 0xffff;
					s_player_view *player = ctf_player_get(player_index);

					if (!g_51ec80->flag_bits[slot].bit0)
					{
						long team = player->team;

						if (ctf_options()->engine_type != 9)
						{
							function_1967d0(absolute_index, team, 0xf, 1);
							function_1970a0(absolute_index, 0x12, 1);
						}
						else
						{
							function_1967d0(absolute_index, team, 0x15, 1);
						}
					}
				}

				byte_flags_set(&g_51ec80->flags[slot], 0, true);
				byte_flags_set(&g_51ec80->flags[slot], 1, false);
				function_a7810(0x200);

				if (slot >= 0 && slot < 9)
				{
					g_51ec80->d[slot] = NONE;
					g_51ec80->times[slot] = 0;
					function_a7810(0x80);
				}

				player_index = unit->player_index;
				if (player_index != NONE && slot >= 0 && slot < 9)
				{
					s_event event;

					game_engine_event_set_cause_player(player_index,
						game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 1, &event));
					event.effect_team = (short)((short)slot == 8 ? NONE : (short)slot);
					function_19eb90(&event);
				}
			}
		}
	}
}

PRIVATE __forceinline short function_2411b1(short arg_0)
{
	short local_0 = NONE;
	if (arg_0 != 8)
		local_0 = arg_0;
	return local_0;
}

// @retail 0x2411b0
void c_game_engine_markers::v24(long object_index, long unit_index)
{
	if (ctf_options()->mode != 4)
	{
		s_slot_object *object = slot_object_get(object_index);

		if (function_101280(object_index) == 1)
		{
			long slot = object->slot;

			if (slot >= 0 && slot < 9)
			{
				s_slot_object *unit = slot_object_get(unit_index);
				s_slot_table *g = g_51ec80;
				long player_index;

				if (g->flags[slot] & 1)
				{
					g->d[slot] = ctf_options()->s230;
					g->times[slot] = g_510c54->field_2_3;
				}
				else
				{
					g->d[slot] = NONE;
					g->times[slot] = 0;
				}
				function_a7810(0x80);

				if (ctf_options()->engine_type == 9)
				{
					if (!(g->flags[slot] & 1))
						g->e[slot] = NONE;
					else if (!(ctf_options()->flags22c.flags & 0x20))
						g->e[slot] = NONE;
					function_a7810(0x100);
				}

				player_index = unit->player_index;
				if (player_index != NONE && !(g->w204 & 1))
				{
					s_event event;

					game_engine_event_set_cause_player(player_index,
						game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 2, &event));
					event.effect_team = (short)((short)slot == 8 ? NONE : (short)slot);
					function_19eb90(&event);
				}
			}
		}
	}
}

// @retail 0x241320
long c_game_engine_markers::v25(long ticks, bool a, bool b)
{
	if ((real)ticks * g_510c54->rate <= 5.0f)
	{
		s_slot_table *g;

		switch (!a)
		{
		case false:
		{
			g = g_51ec80;
			if (b && (ctf_options()->flags22c.flags & 2))
			{
				if (g->b1fc)
					g->l200 = ctf_real_to_long((real)g_510c54->field_2_3 * 5.0f);
				else
					g->l200--;
			}
			else
			{
				g->l200 = 0;
			}
			break;
		}
		default:
			g = g_51ec80;
			break;
		}
		if (ticks <= g->l200)
			ticks = g->l200;
	}

	return ticks;
}

// @retail 0x2413c0
long c_game_engine_markers::v26()
{
	if (g_4e0350->flags & 8)
		return function_161eb0(NONE);

	return g_51ec80->l1f4;
}

// @retail 0x2413f0
bool c_game_engine_markers::v33(long player_index, s_ctf_query const *query)
{
	bool result = true;
	dword flags = query->flags;

	if ((flags & 1) || (flags & 2) && ctf_options()->engine_type != 9)
	{
		long team = ctf_options()->team_mode == 1 ? g_51ec80->l1f4 : ctf_player_get(player_index)->team;

		if (function_15e410((short)team))
		{
			bool away = !g_51ec80->flag_bits[team].bit0;

			if ((flags & 1) && away || (flags & 2) && !away)
				result = false;
		}
	}
	else if ((flags & 4) || (flags & 8) && ctf_options()->engine_type == 9)
	{
		long team = ctf_options()->team_mode == 1 ? g_51ec80->l1f4 : ctf_player_get(player_index)->team;

		if (function_15e410((short)team))
		{
			bool away = !g_51ec80->flag_bits[team].bit0;

			if ((flags & 4) && away || (flags & 8) && !away)
				result = false;
		}
	}

	return result;
}

/* a copy of function_15b2f0, which retail inlines in v16 */
struct s_ctf_engine_state
{
	byte unknown00[0x6c];
	short w6c;
	byte unknown6e[0xc04 - 0x6e];
	long lc04;
	byte unknownc08[0xc14 - 0xc08];
	long engine_index;
};

static inline bool ctf_engine_running()
{
	s_ctf_engine_state *g = (s_ctf_engine_state *)g_4e9ae8;
	bool result = false;

	if (g_55e4d0[g->engine_index] && g->w6c == 1 && (g_4e6948->mode == 4 || g->lc04 == 1))
		result = true;

	return result;
}

// @retail 0x241520
bool c_game_engine_markers::v16(long player_index, long object_index)
{
	bool result = true;

	if (!ctf_engine_running())
		result = false;
	else if (player_index != NONE && object_index != NONE)
	{
		s_player_view *player = ctf_player_get(player_index);
		long slot = slot_object_get(object_index)->slot;

		if (function_101280(object_index) == 1 && slot != NONE)
		{
			if (ctf_options()->engine_type == 9 && (g_51ec80->flags[slot] & 2))
				result = false;
			else if (slot != 8 && (ctf_options()->engine_type != 9 && slot == player->team ||
					ctf_options()->engine_type == 9 && slot != player->team))
				result = false;
		}
	}

	return result;
}

// @retail 0x241fc0
void c_game_engine_markers::v36(long value)
{
	if (value == 1)
	{
		long team = g_51ec80->l1f4;
		long next = function_161eb0(team);

		if (team != NONE && next == NONE)
			next = team;
		function_2432e0(next);
	}
}

// @retail 0x242000
bool c_game_engine_markers::v34(long value)
{
	bool result = false;

	if (!value)
		result = true;

	return result;
}

// @retail 0x242010
bool c_game_engine_markers::v35(long player_index, long type)
{
	bool result = false;

	if (player_find_unit_item_of_type(player_index, 1) != NONE)
	{
		if (type == 2)
			result = ctf_options()->l238 == 0;
		else if (type == 3)
			result = ctf_options()->flags22c.bits.bit6;
		else if (type == 1)
			result = ctf_options()->flags22c.bits.bit7;
	}
	else
	{
		result = ((c_game_engine *)this)->c_game_engine::v5(player_index, type);
	}

	return result;
}

// @retail 0x2421d0
long function_2421d0(long team)
{
	long result = NONE;

	if (team == 8)
	{
		result = g_51ec80->l38;
	}
	else if (team >= 0 && team < 8)
	{
		long slot = function_158990(team);

		if (slot != NONE)
			result = g_51ec80->a[slot];
	}

	return result;
}

// score 17 (was 17): the enabled peer is loaded before the compare on the engine type and into
// EDX, retail loads it after the compare and into ECX.
// Tried: the peer read inside each arm 74 (212 bytes: the two arms colour the peer loads
// differently, so they never merge above the branch); that with the else-if's type test kept 64
// (one arm right, the other rotated by a register); a bool for the type test used twice 59; nested
// arms with the peer local 17; a named block, tag or index local in each arm 17/64 (the block
// pointer keeps the next scratch register, retail reuses eax); a 600 s permuter run, none below 17.
// Round 24: the type test nested under one peer test 57 (also with a bool for the type); one
// ternary 55; nested arms with the peer read after the type 30 (before NONE 35); a plain
// `else if (enabled)` 35 (193 bytes); NONE in a final else 56; the peer tested first in each && 55;
// the existing game_engine_get() in each arm 74.
// Round 139: a peer ternary in each type arm 75 (213 bytes; the arms' scratch registers rotate
// by one, ecx/edx/eax then edx/eax/ecx); the peer tested first with the type if/else inside 57;
// the peer local declared before the type 33; the type read at each test, no local 59; the peer
// as a bool 61.
// @retail 0x242210
long function_242210(point3f const *point)
{
	s_object_placement data;
	long object_index;
	long definition_index;

	long engine_type = ctf_options()->engine_type;
	definition_index = NONE;
	c_engine_peer *enabled = g_55e4d0[g_4e9ae8->engine_index];
	if (engine_type == 9 && enabled)
		definition_index = mp_globals_block()->bomb_definition_index;
	else if (engine_type != 9 && enabled)
		definition_index = mp_globals_block()->flag_definition_index;

	function_b7930(&data, definition_index, NONE, 0);
	data.position = *point;
	object_index = function_b7b40(&data);
	if (object_index != NONE)
		function_a7870(object_index);

	return object_index;
}

// @retail 0x2422d0
void function_2422d0(long slot)
{
	s_slot_table *g = g_51ec80;
	long marker_index;

	g->e[slot] = NONE;
	ctf_globals_changed(0x100);
	g->d[slot] = NONE;
	g->times[slot] = 0;
	ctf_globals_changed(0x80);
	g->flags[slot] = 0;
	ctf_globals_changed(0x200);

	marker_index = function_2421d0(slot);
	if (marker_index != NONE)
	{
		point3f point;
		point3f const *local_0 = marker_position(marker_index);
		long local_1 = ((long const *)local_0)[1];
		((long *)&point)[0] = ((long const *)local_0)[0];
		((long *)&point)[1] = local_1;
		((long *)&point)[2] = ((long const *)local_0)[2];
		long object_index = function_242210(&point);

		if (object_index != NONE)
			function_15e050(object_index, slot);
	}
}

// score 7 (was 7): the object index is kept in EDI and the object in EBP, retail keeps them the other
// way round. Native index/eligibility/object reference 7; team-index/team-object/
// other-count names 7; second-loop element address/nested accept 7; native slot table/
// const object pointer/table-entry address 7; slot-field address 60; the object getter at each
// use 86 (reloads after the call); index or object local renamed to sort first or last 7 each;
// the reject arm first 7; the rejected count advanced in its own statement 7.
// Round 53: function-scope object index reused for the second loop's element 7; permuter (58
// variants) 7; lookup written without the inline getter 7 (diagnostic); a `long i` per loop 7;
// `dword` object index 7; the eligibility result in a named bool 79.
// @retail 0x2423f0
bool function_2423f0()
{
	long others[8];
	long slot_objects[9];
	bool result = true;
	long count = 0;
	long i;

	memset(slot_objects, NONE, sizeof(slot_objects));

	for (i = 0; i < ((s_ctf_team_view *)g_4e9ae8)->team_count; i++)
	{
		long object_index = ((s_ctf_team_view *)g_4e9ae8)->teams[i].object_index;
		s_slot_object *object = slot_object_get(object_index);
		long slot = object->slot;

		/* the table test reads the slot again after the call */
		if (function_243a80(slot) && slot_objects[object->slot] == NONE)
		{
			slot_objects[object->slot] = object_index;
		}
		else
		{
			result = false;
			others[count++] = object_index;
		}
	}

	for (i = 0; i < count; i++)
	{
		function_15e130(others[i]);
		function_b8540(others[i]);
	}

	for (i = 0; i < 9; i++)
	{
		if (function_243a80(i) && slot_objects[i] == NONE)
		{
			function_2422d0(i);
			result = false;
		}
	}

	return result;
}

// @retail 0x242510
long function_242510(long object_index)
{
	long result = NONE;
	long slot = slot_object_get(object_index)->slot;

	if (slot != NONE)
	{
		s_slot_table *g = g_51ec80;

		if (g->flags[slot] & 2)
		{
			point3f position;

			function_b9dd0(object_index, &position);
			for (long team = 0; team < 8; team++)
			{
				if (function_161e10(team))
				{
					long team_slot = function_158990(team);

					for (long k = 0; k < 2; k++)
					{
						if (team_slot != NONE)
						{
							long marker_index = k ? g->c[team_slot] : g->b[team_slot];

							if (marker_index != NONE)
							{
								point3f point;
								point3f const *local_0 = marker_position(marker_index);
								*(long volatile *)&point.x = ((long const *)local_0)[0];
								long local_1 = ((long const *)local_0)[2];
								((long *)&point)[1] = ((long const *)local_0)[1];
								((long *)&point)[2] = local_1;

								if (distance_sq3f(&position, &point) < 0.09f)
								{
									result = team;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return result;
}

extern vector3f *g_4687bc;

struct s_location
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

struct s_damage_owner
{
	long field_0;
	long field_4;
	short field_8;
};

struct s_effect_owner;

struct s_effect_marker
{
	point3f position;
	vector3f forward;
	dword name;
};

struct s_type_1e6529
{
	long definition_index;
	dword flags;
	s_damage_owner owner;
	long field_14;
	long field_18;
	s_location field_1c;
	point3f position;
	point3f origin;
	vector3f direction;
	byte field_48[0x54 - 0x48];
	real field_54;
	byte field_58[0x7c - 0x58];
	short field_7c;
	byte field_7e[0x84 - 0x7e];
	byte field_84;
	byte field_85[3];
};

struct s_242661
{
	byte field_0[0xe0];
	long field_e0;
	long field_e4;
	long field_e8;
	long field_ec;
	long field_f0;
};

void function_176b60(s_effect_owner const *owner, long marker_count, s_effect_marker *markers, long mode, long tag_index);
void function_a8040(long index, long value, point3f const *position);
void function_d6660(s_type_1e6529 *data, long definition_index);
void function_bc190(long object_index, s_damage_owner *owner);
s_location *object_get_root_location(long object_index, s_location *location);
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index);
void function_15e360(point3f const *point, long object_index);
byte *record_pool_lookup(s_record_pool *data, long datum_index);
void function_242e00(long player_index, long team);

// @retail 0x242660
void function_242660(long object_index, bool arg_1)
{
	s_slot_object *object = slot_object_get(object_index);

	if (ctf_options()->engine_type == 9 && !ctf_options()->b1128)
	{
		long slot = object->slot;
		s_242661 const *constants = (s_242661 const *)mp_globals_block()->constants;
		long damage_index = NONE;

		if (slot != NONE)
		{
			long effect_index;
			long value;

			if (!arg_1 && (g_51ec80->flags[slot] & 2))
			{
				long team = function_242510(object_index);
				long carrier = g_51ec80->carriers[slot];

				if (team != NONE && carrier != NONE && record_pool_lookup(g_4e8c24, carrier))
				{
					function_242e00(carrier, team);
					g_51ec80->carriers[object->slot] = NONE;
					function_a7810(0x400);
				}
				effect_index = constants->field_e0;
				damage_index = constants->field_e8;
				value = 0;
			}
			else
			{
				effect_index = constants->field_f0;
				value = 1;
			}

			if (effect_index != NONE)
			{
				vector3f local_0 = *g_4687b0;
				point3f position;
				s_effect_marker markers[5];
				s_damage_owner owner;

				function_b9dd0(object_index, &position);
				function_bc190(object_index, &owner);
				markers[4].position = position;
				markers[3].position = position;
				markers[2].position = position;
				markers[1].position = position;
				markers[0].position = position;
				markers[0].forward = *g_4687b0;
				markers[1].forward = *g_4687bc;
				markers[2].forward = *g_4687b0;
				markers[3].forward.i = 0.0f - local_0.i;
				markers[3].forward.j = 0.0f - local_0.j;
				markers[3].forward.k = 0.0f - local_0.k;
				markers[4].forward = local_0;
				markers[0].name = 0x700054c;
				markers[1].name = 0x70000c0;
				markers[2].name = 0x20000ca;
				markers[3].name = 0x8000550;
				markers[4].name = 0x400054f;
				function_176b60((s_effect_owner const *)&owner, 5, markers, 1, effect_index);
				function_a8040(object_index, value, &position);
			}

			if (damage_index != NONE)
			{
				s_type_1e6529 data;
				point3f position;

				data.field_7c = NONE;
				function_b9dd0(object_index, &position);
				function_d6660(&data, damage_index);
				function_bc190(object_index, &data.owner);
				data.field_54 = 1.0f;
				object_get_root_location(object_index, &data.field_1c);
				data.origin = position;
				data.position = position;
				data.direction = *g_4687b0;
				data.field_84 = 0x19;
				function_d6c80(&data, NONE);
			}
		}
	}

	if (object->slot != NONE)
	{
		/* retail calls the marker lookup here */
#pragma inline_depth(0)
		long marker_index = function_2421d0(object->slot);
#pragma inline_depth()

		g_51ec80->d[object->slot] = NONE;
		g_51ec80->times[object->slot] = g_510c54->field_2_3;
		ctf_globals_changed(0x80);
		if (ctf_options()->engine_type == 9)
		{
			g_51ec80->e[object->slot] = NONE;
			ctf_globals_changed(0x100);
		}
		if (marker_index != NONE)
		{
			point3f point;

			g_51ec80->flags[object->slot] &= ~1;
			g_51ec80->flags[object->slot] &= ~2;
			g_51ec80->flags[object->slot] &= ~8;
			ctf_globals_changed(0x200);
			g_51ec80->w204 |= 1;
			point = *marker_position(marker_index);
			function_15e360(&point, object_index);
			g_51ec80->w204 &= ~1;
		}
	}
}

PRIVATE __forceinline long function_242ef4(s_slot_table *g, s_242ef1 const &arg_0, long team)
{
	long result = NONE;

	for (long i = 0; i < 8; i++)
	{
		if (ctf_team_is_active(i))
		{
			long slot = function_158990(i);

			if (slot != NONE)
			{
				for (long k = 0; k < 2; k++)
				{
					long marker_index = k == 0 ? g->b[slot] : g->c[slot];
					bool valid;

					if (ctf_options()->team_mode == 1 && ctf_options()->engine_type == 9)
						valid = i != team && i == g->l1f4;
					else
						valid = ctf_options()->engine_type == 9 ? i != team : i == team;

					if (valid && marker_index != NONE)
					{
						point3f marker;
						point3f const *local_0 = marker_position(marker_index);
						((long *)&marker)[0] = ((long const *)local_0)[0];
						long local_1 = ((long const *)local_0)[2];
						((long *)&marker)[1] = ((long const *)local_0)[1];
						((long *)&marker)[2] = local_1;

						g = g_51ec80;
						point3f const *point = *(point3f const *const volatile *)&arg_0.field_0;
						if (distance_sq3f(point, &marker) < 1.0f)
						{
							result = marker_index;
							break;
						}
					}
				}
			}
		}
	}

	return result;
}

// @retail 0x242ef0
long function_242ef0(s_242ef1 arg_0, long team)
{
	return function_242ef4(g_51ec80, arg_0, team);
}

// score 53 (was 53): ours keeps the engine type in ebp (one more callee-save), retail reuses ebx, so every
// event slot is four bytes off and the type is computed and stored first. Compiled without LTCG the same
// source reuses ebx, and with the event sent through an opaque function ours still takes ebp.
// Earlier: the fields of the event are written in the opposite order and the argument is passed in the
// other register. Options pointer local: the player index moves to edi (44-58); subtype/type locals in
// either order 51; game time read into a local before the sum: retail's tail, 53 (67/65); a file-level
// inline event-type helper for the type argument 53 (identical); the sum as game time plus the rounded
// value 53; the subtype in a local at entry 53; the time test reversed 55; the team through the file's
// player getter 53; the cause team stored first 56; the team in a local before the initializer 66 (and
// the matched caller 0x241620 stops matching).
// Round 77: one combined test, three nested tests and the event declared at the top 53 each (same
// code); the out-of-line initializer 103, with the out-of-line cause-player setter 129 (both stay
// calls), the inline initializer with that setter 75. EBP is used for the engine type alone; ruled out
// as a cause: the final table store merged with the first table read across the event call.
// Round 117: the type ternary inverted 53 (aligned 42); the time test with its operands swapped 55; the
// subtype ternary inverted 53; the final sum as game time plus the rounded product 53; a subtype local
// for the initializer 53; an options pointer local for the mode and type 60 (aligned 38: retail's
// event slots, but the options pointer in ecx and the player index in edi, still ebp for the type).
// Diagnostic only (reverted): the table read through a volatile pointer at the final store or at the
// time test keeps ebp, so the table web is not what takes ebx.
// Round 119: retail makes the subtype before the type; with the header inline every spelling makes the
// type first and keeps the engine type in ebp. One-file harness: the time test's table read is what takes
// ebx from the engine type (without it ours reuses ebx); ternaries inverted, cause/team order, final sum
// order, subtype/type locals: identical code (53 each with --fast); team before cause with the subtype
// ternary inverted 56. Round 121: 0x19ebb0 as an inline candidate (`inline` on its definition) is
// inlined here by the linker with identical code, 53 (so not blocked by 0x19ebb0's file), and into its
// five matched callers, which all break; the header inline storing the subtype first 64 (0x19ebb0 then
// differs); the parameters as (flag, player index) 53 (0x241620 then differs); the team read by the
// absolute index through either player getter 57.
// Round 119: BLOCKED-BY 0x19ebb0. Retail makes the subtype before the type (a call's argument order),
// so the initializer is the out-of-line 0x19ebb0 inlined by the linker, which its /Ob1 file prevents
// here; with the header inline every spelling makes the type first and keeps the engine type in ebp.
// One-file harness: the time test's table read is what takes ebx from the engine type (without it ours
// reuses ebx); ternaries inverted, cause/team order, final sum order, subtype/type locals: identical
// code (53 each with --fast); team before cause with the subtype ternary inverted 56.
// Round 36: subtype and type locals before the initializer 53 (identical); an options pointer local
// for the mode and engine type 60 (and caller 0x241620 stops matching); the type ternary inverted 53;
// a guard return on the time test 53; the index declared after the mode test with the time test
// joined 67. Diagnostic: without the final table store EBP is still used.
// Round 176 (fast): a table pointer local for the time test and the final store 59 (the engine type
// then takes EBX, but the options pointer moves to EBP and the table stays in EDI across the call); a
// pointer to the player's time slot 66 (0x241620 lost); a game-time globals local 62; the game time
// through the existing getter function_146650() at both reads, `function_146650() + rounded` in the
// final sum, 55: retail's exact tail, only EBP left (its push shifts every later slot), so keep it in
// any later fix; on it the cause team through ctf_player_get 55, subtype and type locals in either
// order 55. Diagnostics: a time test with one register operand puts it in EBX and the engine type
// shares EBX; with the game time and the table both in registers ours moves the engine type to EBP.
// @retail 0x2430a0
void function_2430a0(long player_index, bool flag)
{
	long absolute_index = player_index & 0xffff;

	if (ctf_options()->mode != 4 && absolute_index >= 0 && absolute_index < 16)
	{
		if (g_510c54->game_time > g_51ec80->player_times[absolute_index])
		{
			s_event event;

			game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, flag ? 13 : 7);
			event.cause_player_index = player_index;
			event.cause_team = event_player_get(player_index)->team;
			function_19eb90(&event);
			g_51ec80->player_times[absolute_index] = ctf_real_to_long((real)g_510c54->field_2_3 * 4.0f) + g_510c54->game_time;
		}
	}
}

// @retail 0x2431a0
void function_2431a0()
{
	s_ctf_options *options = ctf_options();

	g_51ec80->b1fc = false;
	if (options->flags22c.flags & 2)
	{
		for (long i = 0; i < 9; i++)
		{
			s_team_entry *entry = function_15e410((short)i);

			if (entry)
			{
				bool carried = entry->lc != NONE;
				s_slot_flags flags = g_51ec80->flag_bits[i];
				bool dropped = flags.bit2;
				bool away = !flags.bit0;
				bool armed = options->engine_type == 9 && flags.bit1;

				if (carried || dropped || armed || away && function_244300(entry->object_index, NULL))
				{
					g_51ec80->b1fc = true;
					return;
				}
			}
		}
	}
}

#pragma inline_depth(1)
// @retail 0x243250
void function_243250(long player_index)
{
	if (ctf_options()->mode != 4 && ctf_options()->team_mode == 1)
	{
		s_event event;

		game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, 6);
		event.cause_team = g_51ec80->l1f4;
		event.a = player_index;
		game_engine_event_send_inline(&event);
	}
}
#pragma inline_depth(255)


// @retail 0x2432e0
void function_2432e0(long team)
{
	if (ctf_options()->mode != 4)
		function_15e4d0();

	g_51ec80->l1f4 = team;
	ctf_globals_changed(0x20);

	if (ctf_options()->mode != 4)
	{
		function_157670();
		function_243250(NONE);
		for (long i = 0; i < 9; i++)
		{
			if (function_243a80(i))
				function_2422d0(i);
		}
	}

	g_51ec80->l200 = 0;
}

// @retail 0x243380
bool c_game_engine_markers::v40()
{
	return function_2447f0();
}

// @retail 0x243390
void c_game_engine_markers::v44(long, s_marker_update *update)
{
	memset(update, 0, sizeof(s_marker_update));
	v41(update);
	update->l24 = function_161eb0(NONE);
	update->l28 = 0;
	memset(update->a, 0xff, sizeof(update->a));
	memset(update->triples, 0xff, sizeof(update->triples));
	update->b80 = 0;
}

// @retail 0x243a20
void function_243a20(long object_index)
{
	s_slot_object *object = slot_object_get(object_index);

	if (object->slot != NONE)
	{
		color3f buffer;
		long team = object->slot;
		color3f color = *function_7f720(&buffer, (short)team);

		function_bacc0(object_index, 0, (point3f const *)&color);
	}
}

// @retail 0x243a80
bool function_243a80(long slot)
{
	bool result = false;

	if (slot >= 0 && slot < 8)
	{
		long team_mode = ctf_options()->team_mode;

		if (team_mode != 2 && function_161e10(slot))
		{
			if (team_mode == 1)
			{
				if (ctf_options()->engine_type == 9)
					result = slot != g_51ec80->l1f4;
				else
					result = slot == g_51ec80->l1f4;
			}
			else
			{
				result = true;
			}
		}
	}
	else if (slot == 8)
	{
		result = ctf_options()->team_mode == 2;
	}

	return result;
}

PRIVATE __forceinline short function_244301(short arg_0)
{
	return arg_0 == 8 ? NONE : arg_0;
}

// @retail 0x244300
bool function_244300(long object_index, long *player_index)
{
	bool result = false;
	s_ctf_constants *constants = mp_globals_block()->constants;
	real radius_squared = constants->pickup_radius * constants->pickup_radius;
	long slot = slot_object_get(object_index)->slot;
	long team = function_244301((short)slot);
	point3f position;

	function_b9dd0(object_index, &position);
	if (slot != NONE)
	{
		s_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			s_player_view *player = iterator.player;
			long player_team = player->team;
			long unit_index = player->unit_index;

			if ((ctf_options()->engine_type != 9 && player_team != team) || (ctf_options()->engine_type == 9 && player_team == team) || ctf_options()->team_mode == 2)
			{
				point3f unit_position;
				vector3f delta;

				function_b9dd0(unit_index, &unit_position);
				vector3d_from_points3d(&unit_position, &position, &delta);
				if (length_sq3f(&delta) < radius_squared)
				{
					result = true;
					if (player_index)
						*player_index = iterator.index;
					break;
				}
			}
		}
	}

	return result;
}

PRIVATE __forceinline real function_244681(long arg_0, s_palette_source_globals const *arg_1, point3f *arg_2, real arg_3)
{
	point3f const *local_0 = &arg_1->marker_entries[arg_0].position;
	long local_1 = ((long const volatile *)local_0)[0];
	((long *)arg_2)[0] = local_1;
	real local_2 = arg_3 - arg_2->x;
	((long *)arg_2)[1] = ((long const volatile *)local_0)[1];
	((long *)arg_2)[2] = ((long const volatile *)local_0)[2];
	return local_2;
}

// @retail 0x244680
bool function_244680(long team, long unit_index)
{
	short own_slot = function_158990(team);
	bool result = false;
	point3f position;
	s_slot_table *g;

	function_b9dd0(unit_index, &position);
	g = g_51ec80;
	for (long i = 0; i < 9; i++)
	{
		short slot_team = ((s_ctf_team_view *)g_4e9ae8)->slot_teams[i];

		if (i != own_slot && slot_team != NONE && function_161e10(slot_team))
		{
			for (long k = 0; k < 2; k++)
			{
				long marker_index = k == 0 ? g->b[i] : g->c[i];

				if (marker_index != NONE)
				{
					point3f const *bounds = k == 0 ? &g->bounds[0][i] : &g->bounds[1][i];
					point3f marker;
					real dx = function_244681(marker_index, g_4e0350, &marker, position.x);
					real dy = position.y - marker.y;

					if (dx * dx + dy * dy < bounds->x * bounds->x &&
						bounds->z > position.z - marker.z &&
						bounds->y > marker.z - position.z)
					{
						result = true;
						break;
					}
				}
			}
		}
	}

	return result;
}

PRIVATE __forceinline void function_2447f2(long arg_0, s_palette_source_globals const *arg_1, point3f *arg_2)
{
	point3f const *local_0 = &arg_1->marker_entries[arg_0].position;
	long const *local_1 = (long const *)local_0;
	long local_2 = local_1[2];
	long local_3 = local_1[1];
	long local_4 = local_1[0];
	((long *)arg_2)[0] = local_4;
	((long *)arg_2)[1] = local_3;
	((long *)arg_2)[2] = local_2;
}

// @retail 0x2447f0
bool function_2447f0()
{
	bool result = function_2423f0();

	for (long i = 0; i < 9; i++)
	{
		s_team_entry *entry = function_15e410((short)i);

		if (entry && entry->object_index != NONE && !(g_51ec80->flags[i] & 1))
		{
			if (entry->l8 == NONE)
			{
				long marker_index = function_2421d0(i);
				point3f position;
				point3f marker;

				if (marker_index == NONE)
					continue;
				function_b9dd0(entry->object_index, &position);
				function_2447f2(marker_index, g_4e0350, &marker);
				real delta_x = marker.x - position.x;
				real delta_y = marker.y - position.y;
				real delta_z = marker.z - position.z;
				real distance_squared = delta_z * delta_z + delta_y * delta_y + delta_x * delta_x;
				if (!(distance_squared > 0.2f * 0.2f))
					continue;
			}

			g_51ec80->flags[i] |= 1;
			ctf_globals_changed(0x200);
			result = false;
		}
	}

	return result;
}

/* the scores of the players (0x1c bytes each at +0x304 of the multiplayer
   globals) */
struct s_ctf_player_score
{
	byte unknown00[8];
	short score;
	byte unknown0a[0x1c - 0x0a];
};

static inline s_ctf_player_score *ctf_player_scores()
{
	s_ctf_player_score *result = NULL;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = (s_ctf_player_score *)((byte *)g_4e9ae8 + 0x304);

	return result;
}

long function_baf80(long object_index);
bool function_15b7c0(long delta, long player_index);
void function_10da60(long object_index, point3f *position);

PRIVATE __forceinline long function_240741(long arg_0, s_mp_globals const *arg_1)
{
	long local_0 = 0;
	if (g_55e4d0[arg_1->engine_index])
	{
		s_ctf_player_score const *local_1 = (s_ctf_player_score const *)((byte const *)arg_1 + 0x304);
		if (local_1)
			local_0 = local_1[arg_0 & 0xffff].score;
	}
	return local_0;
}

// @retail 0x240740
void function_240740(long object_index)
{
	long slot = slot_object_get(object_index)->slot;
	long best_player_index = NONE;
	long best_score = 0x80000000;
	s_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator) != 0)
	{
		if (slot == 8 || iterator.player->team == slot)
		{
			long score = function_240741(iterator.index, g_4e9ae8);
			if (score > best_score)
			{
				best_player_index = iterator.index;
				best_score = score;
			}
		}
	}

	if (slot >= 0 && slot < 9)
		g_51ec80->objects[slot] = best_player_index;
}

void function_1972a0(long player_index, long value14, long value10);
bool function_162b90(long item_index);
long players_first_active_local_player(void);
long function_14de10(long index);
void function_2007b3(long arg_0, long arg_1, long arg_2, long arg_3);
void function_242660(long object_index, bool arg_1);

struct s_240821
{
	dword field_0;
	dword field_4 : 8;
	dword field_c : 1;
	dword field_d : 23;
};

PRIVATE __forceinline long function_240822(long arg_0)
{
	long local_0 = NONE;
	if (arg_0 == NONE)
		arg_0 = 0;
	else
		arg_0++;
	for (long local_1 = arg_0; local_1 < 4; local_1++)
	{
		if (g_4e8c20->entries[local_1] != NONE)
		{
			local_0 = local_1;
			break;
		}
	}
	return local_0;
}

// @retail 0x240820
void __stdcall function_240820(long arg_0)
{
	s_slot_object *local_0 = slot_object_get(arg_0);
	dword local_1 = *(dword const *)((byte const *)ctf_options() + 0x230);
	long local_2 = local_0->slot;
	s_event local_3;

	if (local_2 == NONE)
		return;
	s_team_entry *local_4 = function_15e410((short)local_2);
	if (local_1 <= 5)
		local_1 = 5;

	if (ctf_options()->mode != 4 && local_2 >= 0 && local_2 < 9 && local_4)
	{
		bool local_8 = local_4->lc != NONE;
		bool local_9 = !(g_51ec80->flags[local_2] & 1);
		bool local_7 = false;
		long local_20 = ctf_options()->engine_type;
		bool local_6 = local_20 == 9 && (g_51ec80->flags[local_2] & 8) != 0;
		long local_10 = NONE;

		if (local_8)
		{
			function_1972a0(local_4->lc, (local_20 == 9) + 1, local_4->team);
		}
		if ((local_8 || local_9) && g_51ec80->d[local_2] != NONE)
		{
			g_51ec80->d[local_2] = NONE;
			function_a7810(0x80);
		}

		if (!(ctf_options()->engine_type == 9 && local_9) && function_244300(arg_0, &local_10))
		{
			if (!(g_51ec80->flags[local_2] & 4) && ((short *)g_51ec80->unknown138)[local_2] > 4 * g_510c54->field_2_3)
			{
				game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, ctf_options()->engine_type == 9 ? 0x10 : 0xc, &local_3);
				local_3.effect_team = function_244301((short)local_2);
				game_engine_event_set_cause_player(local_10, &local_3);
				function_19eb90(&local_3);
			}
			g_51ec80->flags[local_2] |= 4;
			local_7 = true;
			((short *)g_51ec80->unknown138)[local_2] = 0;
		}
		else
		{
			if (!local_9 && !local_8)
				((short *)g_51ec80->unknown138)[local_2]++;
			else
				((short *)g_51ec80->unknown138)[local_2] = 0;
			g_51ec80->flags[local_2] &= ~4;
		}
		function_a7810(0x200);

		short local_11 = g_51ec80->d[local_2];
		if (local_11 != NONE && ((ctf_options()->engine_type == 9 && local_6) || !local_7))
		{
			long local_12 = g_51ec80->times[local_2] - 1;
			s_game_time_globals *local_23 = g_510c54;
			if (local_12 < 0)
				local_12 = 0;
			else if (local_12 > local_23->field_2_3)
				local_12 = local_23->field_2_3;
			g_51ec80->times[local_2] = (short)local_12;
			if ((short)local_12 == 0)
			{
				g_51ec80->d[local_2] = (short)(local_11 - 1 < 0 ? 0 : ((dword)(local_11 - 1) > local_1 ? local_1 : local_11 - 1));
				function_a7810(0x80);

				if (g_51ec80->d[local_2] == 0)
				{
					if (!local_8 && TEST_FIELD_BIT(((s_240821 const *)local_0)->field_c))
					{
						if (!(ctf_options()->engine_type == 9 && (g_51ec80->flags[local_2] & 2)) && (g_51ec80->flags[local_2] & 1))
						{
							game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 4, &local_3);
							local_3.effect_team = function_244301((short)local_2);
							function_19eb90(&local_3);
						}
						function_242660(arg_0, false);
					}
					else
					{
						g_51ec80->times[local_2] = local_23->field_2_3;
					}
				}
				else
				{
					g_51ec80->times[local_2] = local_23->field_2_3;
				}
			}
		}

		if (function_162b90(arg_0))
		{
			game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 4, &local_3);
			local_3.effect_team = function_244301((short)local_2);
			function_19eb90(&local_3);
			function_242660(arg_0, false);
		}
		function_240740(arg_0);
	}

	if (local_2 != NONE)
	{
		if (ctf_options()->engine_type != 9)
		{
			bool local_14 = local_4->lc != NONE;
			bool local_24 = !local_14 && (g_51ec80->flags[local_2] & 1);
			for (long local_15 = players_first_active_local_player(); local_15 != NONE; local_15 = function_240822(local_15))
			{
				long local_16 = local_15 == NONE ? NONE : g_4e8c20->entries[local_15];
				if (local_16 != NONE && ctf_player_get(local_16)->team == local_2)
				{
					if (local_14)
						function_2007b3(local_15, 6, 1, 5);
					if (local_24)
						function_2007b3(local_15, 5, 1, 6);
				}
			}
		}
		else if (g_51ec80->flags[local_2] & 2)
		{
			long local_17 = function_242510(arg_0);
			if (local_17 != NONE)
			{
				for (long local_18 = players_first_active_local_player(); local_18 != NONE; local_18 = function_14de10(local_18))
				{
					long local_19 = local_18 == NONE ? NONE : g_4e8c20->entries[local_18];
					if (local_19 != NONE && ctf_player_get(local_19)->team == local_17)
						function_2007b3(local_18, 8, 1, NONE);
				}
			}
		}
	}
}


// @retail 0x240d70
void c_game_engine_markers::v18()
{
	s_ctf_engine_state *state = (s_ctf_engine_state *)g_4e9ae8;
	if (g_55e4d0[state->engine_index] && state->w6c == 1)
	{
		long const &first_mode = (long)ctf_options()->mode;
		if (first_mode == 4 || state->lc04 == 1)
		{
			long const &mode = (long)ctf_options()->mode;
			if (mode != 4)
				function_2447f0();
			for (long i = 0; i < 9; i++)
			{
				s_team_entry *entry = function_15e410((short)i);

				if (entry)
					function_240820(entry->object_index);
			}
			if (ctf_options()->mode != 4)
			{
				function_2431a0();
				if (ctf_options()->mode != 4 && ctf_options()->team_mode == 1)
				{
					long team = g_51ec80->l1f4;

					if (team >= 0 && team < 8 && !function_161e10(team))
						function_15b3a0(NONE, false);
				}
			}
		}
	}
}

// @retail 0x241900
bool function_241900(long player_index)
{
	s_player_view *player = ctf_player_get(player_index);
	long unit_index = player->unit_index;
	bool result = false;

	if (unit_index != NONE)
	{
		long vehicle_index = function_baf80(unit_index);

		if (vehicle_index != unit_index)
		{
			s_player_iterator iterator;

			iterator.data = g_4e8c24;
			iterator.absolute_index = NONE;
			iterator.index = NONE;
			while (player_iterator_next_with_unit((long *)&iterator))
			{
				if (iterator.index != player_index &&
					iterator.player->team == player->team &&
					player_find_unit_item_of_type(iterator.index, 1) != NONE &&
					iterator.player->unit_index != NONE &&
					function_baf80(iterator.player->unit_index) == vehicle_index)
				{
					result = true;
				}
			}
		}
	}

	return result;
}

#pragma inline_depth(1)
// score 13 (was 13): the cause's team is loaded into EAX where retail loads it into EDX, and the
// fields of the event are written in the other order.
// Round 8: early/top team declarations and player-team name 13 each; early event player 54;
// early players with direct score-call arguments 54; branched initializer/engine-first 37;
// selected type 54; default then override 41; direct array with branched initializer 37;
// defaulted type with if, ternary reassignment or +=7 33 each; Boolean/arithmetic type 54 each.
// The defaulted type reaches retail's early ECX data load but keeps its type branch and late stores.
// Original body restored.
// @retail 0x242e00
void function_242e00(long player_index, long team)
{
	s_event event;

	if (function_15b7c0(1, player_index))
	{
		long local_0 = ctf_options()->engine_type == 9 ? 0x12 : 0xe;
		long local_1 = player_index & 0xffff;
		long local_2 = ctf_player_get(player_index)->team;
		function_1967d0(local_1, local_2, local_0, 1);
	}

	game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, 5);
	event.cause_player_index = player_index;
	long cause_team = ctf_player_get(player_index)->team;
	event.effect_team = team;
	event.cause_team = cause_team;
	game_engine_event_send_inline(&event);
}
#pragma inline_depth(255)


// @retail 0x244240
bool function_244240(s_marker_list *list, long object_index, point3f const *point)
{
	list->b0 = 1;
	list->b1 = 0;
	list->l4 = 1;
	function_10da60(object_index, &list->position);
	if (ctf_options()->engine_type == 9)
	{
		list->r14 = 0.1f;
		list->r18 = 0.1f;
	}
	else
	{
		list->r14 = 0.8f;
		list->r18 = 0.1f;
	}
	list->r1c = 0.0f;
	list->l20 = object_index;
	list->color24 = *(s_color_bits const *)point;
	list->color30 = *(s_color_bits const *)point;
	list->r3c = 1.0f;
	list->r40 = 1.0f;
	list->count = 0;
	function_2440a0(list, object_index, point);
	return true;
}

/* the player index of an absolute index (NONE when that player is free) */
/* copies of datum_get_absolute and index_to_datum_index (unknown_16b570.cpp),
   which retail inlines here */
static inline byte *ctf_datum_get_absolute(s_record_pool *data, long index)
{
	byte *result = 0;

	if (index != NONE && index >= 0 && index < data->high_water_index)
	{
		byte *datum = data->data + data->size * index;

		if (*(short *)datum != 0)
			result = datum;
	}

	return result;
}

static inline long ctf_index_to_datum_index(s_record_pool *data, long index)
{
	long result = NONE;

	if (index != NONE)
		result = (*(short *)(data->data + data->size * index) << 16) | index;

	return result;
}

static inline long ctf_player_index_from_absolute(long absolute_index)
{
	s_record_pool *players = g_4e8c24;
	byte *datum = ctf_datum_get_absolute(players, absolute_index);
	long result = NONE;

	if (datum)
		result = ctf_index_to_datum_index(players, absolute_index);

	return result;
}

// @retail 0x2437a0
bool c_game_engine_markers::v46(dword flags, long, s_marker_update *update)
{
	bool result = true;
	dword mask = flags & 0x1f;
	s_slot_table *g;
	long i;

	if (mask)
		result = v43(mask, update) != 0;

	g = g_51ec80;
	if ((flags & 0x20) && g->l1f4 != update->l24)
	{
		function_2432e0(update->l24);
		g = g_51ec80;
	}

	if (flags & 0x80)
	{
		for (i = 0; i < 9; i++)
			g->d[i] = update->a[i];
	}

	if (flags & 0x100)
	{
		for (i = 0; i < 9; i++)
			g->e[i] = update->b[i];
	}

	if (flags & 0x200)
	{
		for (i = 0; i < 9; i++)
			g->flags[i] = update->c[i];
	}

	if (flags & 0x400)
	{
		for (i = 0; i < 3; i++)
		{
			g->triples[i].a = ctf_player_index_from_absolute(update->triples[i].a);
			g->triples[i].b = ctf_player_index_from_absolute(update->triples[i].b);
			g->triples[i].c = ctf_player_index_from_absolute(update->triples[i].c);
		}
	}

	return result;
}
void function_15e360(point3f const *point, long object_index);

PRIVATE __forceinline void function_2417a1(s_marker_entry const *arg_0, long arg_1, point3f *arg_2);

// score 12 (was 141): only the players' data pointer is loaded after the event's initializing stores
// (retail loads it into EDX before them). The marker position goes through the existing y-first copy
// helper function_2417a1 (retail's registers and argument convention throughout, the stored scale read
// back for its zero test). Before: retail kept the player in EBP and the marker in EDI and spilled the
// slot to the player's argument home; ours kept the slot in EBP, the marker in ESI and the player in
// EDI. Tried: the stored scale read back 140; a named scale 140; that test without the player reference
// 145; without the masked-index local 140; the team through the masked index 135 (603 B); the masked
// index at entry 136; read through the player reference 140; the player-index reference removed 141.
// An element pointer for the marker entry gave 32, 12 with the stored scale. Tried on 12: the players'
// data pointer read before the initializer 13 (data/team registers swap EAX/EDX; also declared at the
// block top, or const, 13), the cause player's pointer before it 149, the cause team before it 158,
// the team stored first 144, the effect team first 32/42, the event declared at its use 12, the cause
// team read through the event's index 12, the initializer and cause helpers called 178..212, the
// players' data or the cause pointer read before the last change call 63/122 (before the flag store
// 63/140), a named cause team stored after the effect team 14, the cause team before the cause index
// 145, a word-cast index 156. Permuter (600 s, 15 tries): nothing below 12.
// The existing cause setter / initializer / sender forced inline 178/181/107; the player getter forced
// inline 12; the file-local player accessor 12; an event pointer 12; post-initializer player pointers
// through either accessor 12/12.
// 145, a word-cast index 156. Permuter (600 s, 15 tries): nothing below 12. Also tried on 12: an inline
// cause-player setter 12, the existing out-of-line setter called 178 (565 B, not inlined), a chained
// inline initializer returning the event with the inline setter 12, the event at function scope 12,
// typed player-array indexing for the cause team 12.
// @retail 0x242ba0
void function_242ba0(long marker_index, long object_index, long player_index)
{
	long slot = slot_object_get(object_index)->slot;

	if (slot != NONE)
	{
		long marker_team = function_242b60(marker_index);

		if (ctf_options()->engine_type == 9)
		{
			long absolute_index = player_index & 0xffff;

			function_1967d0(absolute_index, ctf_player_get(player_index)->team, 0x13, 1);
			function_1970a0(absolute_index, 0x15, 1);
		}

		if (marker_index != NONE)
		{
			point3f point;
			s_slot_table *g;
			s_event event;
			s_slot_object *object;

			function_2417a1(g_4e0350->marker_entries, marker_index, &point);
			g_51ec80->w204 |= 1;
			function_15e360(&point, object_index);
			g = g_51ec80;
			g->w204 &= ~1;
			g->carriers[slot] = player_index;
			ctf_globals_changed(0x400);
			g->d[slot] = ctf_options()->scale_b;
			if (!g->d[slot])
				g->d[slot] = 10;
			g->times[slot] = g_510c54->field_2_3;
			ctf_globals_changed(0x80);
			g->flags[slot] |= 2;
			ctf_globals_changed(0x200);

			game_engine_event_initialize_inline(&event, 10, 13);
			event.cause_player_index = player_index;
			event.cause_team = event_player_get(player_index)->team;
			event.effect_team = marker_team;
			game_engine_event_send_inline(&event);

			object = slot_object_get(object_index);
			object->lc8 = NONE;
			object->lc4 = NONE;
			object->sc2 = NONE;
		}
	}
}

#include "flexible_surface_calls.h"

void function_19f680(long tag, long group, long pass, long variant, void *context,
	point2f const *vertices, long count, point3f const *center, real radius,
	real perimeter, point3f const *color, real height);
struct s_sort_record;
typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);
void function_41490(short group, long tag, short kind, real distance, t_record_fill fill,
	dword value, void (__stdcall *callback)(void *), void *context, point3f const *position);

/* draws the circle of a slot's marker (a negative slot names the second set): 32 points around the
   marker's position at the slot's radius, eight per quadrant */
// @retail 0x244470
void __stdcall function_244470(long arg_0, long arg_1, long arg_2, long arg_3,
	long arg_4, long arg_5, void *arg_6)
{
	s_marker_entry *entry;
	point3f local_3;
	real local_2;
	if (arg_5 >= 0)
	{
		entry = &g_4e0350->marker_entries[g_51ec80->b[arg_5]];
		local_3 = entry->position;
		local_2 = g_51ec80->bounds[0][arg_5].x;
	}
	else
	{
		long index = -1 - arg_5;

		entry = &g_4e0350->marker_entries[g_51ec80->c[index]];
		local_3 = entry->position;
		local_2 = g_51ec80->bounds[1][index].x;
	}
	point2f local_4[32];
	for (long local_5 = 0; local_5 < 8; ++local_5)
	{
		real local_6 = (real)local_5 * 0.19634954631328583f;
		real sine = (real)sin(local_6);
		real cosine = (real)cos(local_6);
		point2f *first = &local_4[local_5];
		point2f *second = &local_4[local_5 + 8];
		point2f *third = &local_4[local_5 + 16];
		point2f *fourth = &local_4[local_5 + 24];
		first->x = local_3.x + sine * local_2;
		first->y = local_3.y + cosine * local_2;
		second->x = local_3.x + cosine * local_2;
		second->y = local_3.y - sine * local_2;
		third->x = local_3.x - sine * local_2;
		third->y = local_3.y - cosine * local_2;
		fourth->x = local_3.x - cosine * local_2;
		fourth->y = local_3.y + sine * local_2;
	}
	real local_9 = local_4[1].x - local_4[0].x;
	real local_10 = local_4[1].y - local_4[0].y;
	function_19f680(arg_0, arg_2, arg_4, arg_3, arg_6, local_4, 32, &local_3,
		local_2, (real)sqrt(local_9 * local_9 + local_10 * local_10) * 32.f, g_468710, 0.4f);
}

// @retail 0x244610
void __stdcall function_244610(void *arg_0)
{
	function_40f60(arg_0, function_d4bc0, function_244470);
}

// @retail 0x244630
void function_244630(long arg_1, point3f const *arg_0)
{
	byte *local_0 = (byte *)g_4e3b44[g_4e034c->index & 0xffff].data;
	byte *local_1 = *(byte **)(local_0 + 0xc);
	byte *local_2 = *(byte **)(local_1 + 0x534);
	function_41490(0, *(long *)(local_2 + 0xc4), NONE, 640.f,
		(t_record_fill)function_d4bc0, (dword)function_244470, function_244610, (void *)arg_1, arg_0);
}

PRIVATE __forceinline void function_2417a1(s_marker_entry const *arg_0, long arg_1, point3f *arg_2)
{
    point3f const *local_1 = &arg_0[arg_1].position;
    long local_0 = ((long const *)local_1)[1];
    ((long *)arg_2)[0] = ((long const *)local_1)[0];
    ((long *)arg_2)[1] = local_0;
    ((long *)arg_2)[2] = ((long const *)local_1)[2];
}

// @retail 0x2417a0
void c_game_engine_markers::v13(long arg_0)
{
	point3f local_5;
	if (ctf_options()->engine_type == 9)
	{
		s_player_view *local_0 = ctf_player_get(arg_0);
		if (ctf_options()->team_mode != 2)
		{
			if ((byte)local_0->team == 0xff)
				return;
			s_team_entry *local_1 = function_15e410(local_0->team);
			if (!local_1 || local_1->object_index == NONE)
				return;
		}
		for (long local_2 = 0; local_2 < 9; ++local_2)
		{
			short local_3 = ((s_ctf_team_view *)g_4e9ae8)->slot_teams[local_2];
			if (local_3 != NONE && local_3 != local_0->team && function_161e10(local_3))
			{
				short local_4 = g_51ec80->b[local_2];
				if (local_4 != NONE)
				{
					function_2417a1(g_4e0350->marker_entries, local_4, &local_5);
					function_244630(local_2, &local_5);
				}
				local_4 = g_51ec80->c[local_2];
				if (local_4 != NONE)
				{
					function_2417a1(g_4e0350->marker_entries, local_4, &local_5);
					function_244630(-1 - local_2, &local_5);
				}
			}
		}
	}
}

#include "engine_peer.h"

PRIVATE inline bool function_2419c1(short arg_0, short arg_1)
{
	c_engine_peer *local_0 = g_55e4d0[g_4e9ae8->engine_index];
	return local_0 && local_0->p27(arg_0, arg_1);
}
extern color3f *g_468714;
long function_14de70(long local_player_index);

// @retail 0x2419c0
void c_game_engine_markers::v14(long arg_0)
{
	s_marker_list local_14;
	s_marker_list local_31;
	long local_0 = function_14de70(arg_0);
	s_player_view *local_1 = ctf_player_get(local_0);
	bool local_2 = false;
	bool local_4 = false;
	bool local_3 = false;
	if (ctf_options()->engine_type == 9)
	{
		if (player_find_unit_item_of_type(local_0, 1) != NONE || function_241900(local_0))
		{
			if (ctf_options()->team_mode == 1)
				local_3 = true;
			else
				local_4 = true;
		}
	}
	else if (player_find_unit_item_of_type(local_0, 1) != NONE || function_241900(local_0))
		local_2 = true;
	if (ctf_options()->team_mode == 2)
	{
		s_team_entry *local_5 = function_15e410(8);
		if (local_5)
		{
			bool local_6 = !(bool)g_51ec80->flag_bits[8].bit0;
			bool local_7 = local_5->lc != NONE;
			bool local_8 = !local_6 && !local_7;
			bool local_10 = false;
			bool local_11 = false;
			s_color_bits local_9 = *(s_color_bits *)g_468714;
			if (local_7 && ctf_player_get(local_5->lc)->team == local_1->team)
				local_10 = true;
			if (ctf_options()->engine_type == 9 && g_51ec80->flag_bits[8].bit3)
				local_11 = true;
			bool local_12 = false;
			if (!local_7 || !local_10)
			{
				if (ctf_options()->engine_type == 9)
				{
					if (local_6 || local_8)
						local_12 = *(long *)((byte *)g_4e6948 + 0x23c) != 3;
					else if (local_7 && !local_10)
					{
						switch (*(long *)((byte *)g_4e6948 + 0x23c))
						{
						case 0: local_12 = true; break;
						case 2: local_12 = local_11; break;
						}
					}
				}
				else if (local_6 || local_8)
					local_12 = *(long *)((byte *)g_4e6948 + 0x23c) != 3;
				else if (local_7 && !local_10)
				{
					switch (*(long *)((byte *)g_4e6948 + 0x23c))
					{
					case 1: local_12 = true; break;
					case 2: local_12 = true; break;
					}
				}
			}
			if (local_12)
			{
				local_5 = function_15e410(8);
				if (local_5 && (local_5->lc == NONE || ctf_player_get(local_5->lc)->team != local_1->team))
				{
					if (function_244240(&local_14, local_5->object_index, (point3f *)&local_9))
						function_24e59f(&local_14);
				}
			}
		}
	}
	for (long local_15 = 0; local_15 < 8; ++local_15)
	{
		if (ctf_team_is_active(local_15))
		{
			s_team_entry *local_16 = function_15e410((short)local_15);
			if ((local_15 == local_1->team && local_2) || (local_15 != local_1->team && local_4) || (local_15 == g_51ec80->l1f4 && local_3))
			{
				long local_17 = function_158990(local_15);
				if (local_17 != NONE)
				{
					for (long local_18 = 0; local_18 < 2; ++local_18)
					{
						long local_19 = local_18 == 0 ? g_51ec80->b[local_17] : g_51ec80->c[local_17];
						if (local_19 != NONE)
						{
							point3f local_20;
							function_2417a1(g_4e0350->marker_entries, local_19, &local_20);
							if (function_243ed0(&local_14, &local_20))
								function_24e59f(&local_14);
						}
					}
				}
			}
			if (local_16)
			{
				bool local_22 = !(bool)g_51ec80->flag_bits[local_15].bit0;
				bool local_23 = local_16->lc != NONE;
				bool local_24 = !local_22 && !local_23;
				bool local_25 = false;
				bool local_26 = false;
				color3f local_27;
				s_color_bits local_28 = *(s_color_bits *)function_7f720(&local_27, (short)local_15);
				if (local_23 && ctf_player_get(local_16->lc)->team == local_1->team)
					local_25 = true;
				if (ctf_options()->engine_type == 9 && g_51ec80->flag_bits[local_15].bit3)
					local_26 = true;
				bool local_29 = false;
				if (!local_23 || !local_25)
				{
					if (ctf_options()->engine_type == 9)
					{
						if (local_15 == local_1->team)
							local_29 = true;
						else
						{
							switch (*(long *)((byte *)g_4e6948 + 0x23c))
							{
							case 0: local_29 = true; break;
							case 1: local_29 = local_24; break;
							case 2: local_29 = local_26; break;
							}
						}
					}
					else if (local_15 != local_1->team)
						local_29 = true;
					else
					{
						switch (*(long *)((byte *)g_4e6948 + 0x23c))
						{
						case 0: if (local_24) local_29 = true; break;
						case 1: local_29 = true; break;
						case 2: local_29 = !local_22; break;
						}
					}
				}
				if (local_29)
				{
					if (function_244240(&local_31, local_16->object_index, (point3f *)&local_28))
						function_24e59f(&local_31);
				}
			}
		}
	}
	if (local_0 != NONE)
	{
		s_player_view *player = ctf_player_get(local_0);

		if (player->unit_index != NONE)
		{
			s_player_iterator local_32;
			local_32.data = g_4e8c24;
			local_32.absolute_index = NONE;
			local_32.index = NONE;
			while (player_iterator_next((long *)&local_32))
			{
				if (local_32.index != local_0)
				{
					if (!function_2419c1(local_32.player->team, player->team))
					{
						if (function_162550(local_32.index, &local_14))
						{
							long local_35 = player_find_unit_item_of_type(local_32.index, 1);
							if (local_35 != NONE)
							{
								long local_36 = slot_object_get(local_35)->slot;
								if (local_36 != NONE)
								{
									color3f local_37;
									s_color_bits local_38;
									if (local_36 == 8)
										local_38 = *(s_color_bits *)g_468714;
									else
										local_38 = *(s_color_bits *)function_7f720(&local_37, (short)local_36);
									function_2440a0(&local_14, local_35, (point3f *)&local_38);
								}
							}
							function_24e59f(&local_14);
						}
					}
				}
			}
		}
	}
}
