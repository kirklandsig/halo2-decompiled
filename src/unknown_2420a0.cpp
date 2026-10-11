#include <math.h>
#include <new.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19ec40.h"
#include "unknown_1efac0.h"
#include "unknown_2420a0.h"
#include "unknown_13927e.h"
#include "unknown_1523c0.h"
#include "game_engine_events.h"

// @flags /O2 /arch:SSE /Gr

/* ---- globals ---- */
s_slot_table *g_51ec80;
long g_4b9ed8;

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

long function_19f3c0(long, long);
bool function_19f240(long *iterator);
void function_1967d0(long a, long c, long b, long delta);
void function_1970a0(long b, long a, long delta);
void function_197210(long player_index, long value14, long value10);
void game_engine_event_initialize(s_event *event, long type, long subtype);
void game_engine_event_set_cause_player(s_event *event, long player_index);
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
	virtual void v18() {}
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
		if (function_19f3c0(player_index, 1) != NONE)
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
long function_243b10(bool team_only, long key_b, long *second)
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

// @retail 0x243c00
void function_243c00(long slot)
{
	long k;

	for (k = 0; k < 2; k++)
	{
		long marker_index = k == 0 ? g_51ec80->b[slot] : g_51ec80->c[slot];

		if (marker_index != NONE)
		{
			long results[8];
			long count = function_19ec40(0, 0.0f, (short)((g_4e6948->mode_180 == 9) * 2 + 1), (short)slot, (short)(k + 1), 8, results, 0.0f);
			point3f point = g_4e0350->marker_entries[marker_index].position;
			point3f *bounds_a = &g_51ec80->bounds[0][slot];
			point3f *bounds_b = &g_51ec80->bounds[1][slot];
			long j;

			if (k == 0)
			{
				bounds_a->x = 0.5f;
				bounds_a->y = 0.0f;
				bounds_a->z = 0.0f;
			}
			else
			{
				bounds_b->x = 0.5f;
				bounds_b->y = 0.0f;
				bounds_b->z = 0.0f;
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
					real below = point.z - other_point.z;
					real above = other_point.z - point.z;

					if (k == 0)
					{
						bounds_a->x = distance > bounds_a->x ? distance : bounds_a->x;
						bounds_a->y = bounds_a->y > below ? bounds_a->y : below;
						bounds_a->z = bounds_a->z > above ? bounds_a->z : above;
					}
					else
					{
						bounds_b->x = distance > bounds_b->x ? distance : bounds_b->x;
						bounds_b->y = bounds_b->y > below ? bounds_b->y : below;
						bounds_b->z = bounds_b->z > above ? bounds_b->z : above;
					}
				}
			}

			if (k == 0)
			{
				bounds_a->y += 0.1f;
				bounds_a->z += 0.9f;
			}
			else
			{
				bounds_b->y += 0.1f;
				bounds_b->z += 0.9f;
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
bool function_2440a0(s_marker_list *list, point3f const *point, long object_index)
{
	s_slot_object *object = ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;

	if (list->count < 2 && object->slot != NONE)
	{
		s_game_options_view *options = g_4e6948;
		long divisor = 1;

		if (options->divisor)
			divisor = options->divisor;
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
		{
			if (value == NONE)
				list->items[list->count].r = 0.0f;
			else
			{
				real *local_1 = &list->items[list->count].r;
				real volatile local_0 = (real)value / (real)scale;
				*local_1 = local_0;
			}
		}
		else
		{
			if (value == NONE)
				list->items[list->count].r = 0.0f;
			else
				list->items[list->count].r = (real)value / (real)divisor;
		}

		list->b0 = 1;
		list->count++;
	}

	return true;
}

/* ---- the object whose destructor is at 0x244930 ---- */
struct c_marker_base : c_a
{
	dword d8;
	dword dc;

	c_marker_base(dword other)
	{
		dc = other;
		unknown06 = 1;
		d8 = 0;
	}
};

struct c_marker_object : c_marker_base
{
	dword d10;

	c_marker_object(dword value, dword other) : c_marker_base(other)
	{
		d10 = value;
	}

	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
	}
};

// @retail 0x244930 deleting c_marker_object

class c_marker_object_factory
{
public:
	dword unknown04[2];
	dword value0c;

	virtual c_marker_object *create(dword value, void *memory);
	virtual void s1() {}
	virtual void s2() {}
	virtual void s3() {}
};

// @retail 0x244bb0
c_marker_object *c_marker_object_factory::create(dword value, void *memory)
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
void function_244ca0(real height, real radius, s_line_list *list, long a, long b, long c, byte d, byte e, short f, point3f const *position)
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

bool function_19f300(long *iterator);

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
	return &g_4e0350->marker_entries[marker_index].position;
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
	memset(table->c, 0xff, sizeof(table->c));
	g_51ec80 = table;

	for (i = 0; i < 9; i++)
	{
		long second = NONE;

		table->a[i] = (short)function_243b10(true, i, NULL);
		table->b[i] = (short)function_243b10(false, i, &second);
		table->c[i] = (short)second;
		function_243c00(i);
	}

	table->l38 = function_243b10(true, NONE, NULL);
	table->initialized = true;
	memset(table->d, 0xff, sizeof(table->d));

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

	if (function_19f3c0(player_index, 1) != NONE)
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
	if (killer != NONE && victim != NONE && !suicide && function_19f3c0(victim, 1) != NONE)
	{
		long absolute_index = killer & 0xffff;
		long team = ctf_player_get(killer)->team;

		if (ctf_options()->engine_type == 9)
		{
			function_1967d0(absolute_index, team, 0x14, 1);
			function_1970a0(absolute_index, 0x16, 1);
		}
		else
		{
			function_1967d0(absolute_index, team, 0x10, 1);
			function_1970a0(absolute_index, 0x13, 1);
		}
	}
}

// @retail 0x240f90
long c_game_engine_markers::v31(long killer, long victim, bool suicide)
{
	long result = NONE;

	if (killer != result && victim != result && !suicide && function_19f3c0(victim, 1) != result)
		result = ctf_options()->engine_type == 9 ? 0x28 : 0x27;

	return result;
}

// @retail 0x240fe0
void c_game_engine_markers::v21(long object_index)
{
	function_243a20(object_index);
}

// @retail 0x240ff0
void c_game_engine_markers::v23(long object_index, long unit_index)
{
	if (ctf_options()->mode != 4)
	{
		s_slot_object *object = slot_object_get(object_index);

		if (function_101280(object_index) == 1)
		{
			long slot = object->slot;
			s_slot_object *unit = slot_object_get(unit_index);

			if (slot != NONE)
			{
				s_slot_table *g = g_51ec80;
				long player_index = unit->player_index;

				if (player_index != NONE && !g->flag_bits[slot].bit0)
				{
					long absolute_index = player_index & 0xffff;
					long team = ctf_player_get(player_index)->team;

					if (ctf_options()->engine_type == 9)
					{
						function_1967d0(absolute_index, team, 0x15, 1);
					}
					else
					{
						function_1967d0(absolute_index, team, 0xf, 1);
						function_1970a0(absolute_index, 0x12, 1);
					}
				}

				g->flag_bits[slot].bit0 = true;
				g->flag_bits[slot].bit1 = false;
				function_a7810(0x200);

				if (slot >= 0 && slot < 9)
				{
					g->d[slot] = NONE;
					g->times[slot] = 0;
					function_a7810(0x80);
				}

				player_index = unit->player_index;
				if (player_index != NONE && slot >= 0 && slot < 9)
				{
					s_event event;

					game_engine_event_initialize(&event, ctf_options()->engine_type == 9 ? 10 : 3, 1);
					game_engine_event_set_cause_player(&event, player_index);
					event.effect_team = (short)(slot == 8 ? NONE : slot);
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
					if (!(g->flags[slot] & 1) || !(ctf_options()->flags22c.flags & 0x20))
						g->e[slot] = NONE;
					function_a7810(0x100);
				}

				player_index = unit->player_index;
				if (player_index != NONE && !(g->w204 & 1))
				{
					s_event event;

					game_engine_event_initialize(&event, ctf_options()->engine_type == 9 ? 10 : 3, 2);
					game_engine_event_set_cause_player(&event, player_index);
					event.effect_team = function_2411b1((short)slot);
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
	bool result;
	*(bool volatile *)&result = true;

	if (!ctf_engine_running())
	{
		result = false;
	}
	else if (player_index != NONE && object_index != NONE)
	{
		s_player_view *player = ctf_player_get(player_index);
		long slot = slot_object_get(object_index)->slot;

		if (function_101280(object_index) == 1 && slot != NONE)
		{
			long engine_type = ctf_options()->engine_type;

			if (engine_type == 9 && (g_51ec80->flags[slot] & 2))
				result = false;
			else if (slot != 8 && (engine_type != 9 && slot == player->team || engine_type == 9 && slot != player->team))
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

	if (function_19f3c0(player_index, 1) != NONE)
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

// @retail 0x242210
long function_242210(point3f const *point)
{
	long definition_index = NONE;
	s_object_placement data;
	long object_index;

	if (ctf_options()->engine_type == 9)
	{
		if (g_55e4d0[g_4e9ae8->engine_index])
			definition_index = mp_globals_block()->bomb_definition_index;
	}
	else
	{
		if (g_55e4d0[g_4e9ae8->engine_index])
			definition_index = mp_globals_block()->flag_definition_index;
	}

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
		long local_1 = ((long const volatile *)local_0)[1];
		*(long volatile *)&point.x = ((long const *)local_0)[0];
		((long *)&point)[1] = local_1;
		((long *)&point)[2] = ((long const volatile *)local_0)[2];
		long object_index = function_242210(&point);

		if (object_index != NONE)
			function_15e050(object_index, slot);
	}
}

// @retail 0x2423f0
bool function_2423f0()
{
	long others[8];
	long slot_objects[9];
	bool result = true;
	long count = 0;
	long i;

	for (i = 0; i < 9; i++)
		slot_objects[i] = NONE;

	for (i = 0; i < ((s_ctf_team_view *)g_4e9ae8)->team_count; i++)
	{
		long object_index = ((s_ctf_team_view *)g_4e9ae8)->teams[i].object_index;
		s_slot_object *object = slot_object_get(object_index);

		if (function_243a80(object->slot) && slot_objects[object->slot] == NONE)
		{
			slot_objects[object->slot] = object_index;
		}
		else
		{
			others[count++] = object_index;
			result = false;
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

// @retail 0x242ef0
long function_242ef0(s_242ef1 arg_0, long team)
{
	s_slot_table *g = g_51ec80;
	long result = NONE;

	for (long i = 0; i < 8; i++)
	{
		if (ctf_team_is_active(i))
		{
			long slot = function_158990(i);

			if (slot != NONE)
			{
				long team_mode = ctf_options()->team_mode;

				for (long k = 0; k < 2; k++)
				{
					long marker_index = k == 0 ? g->b[slot] : g->c[slot];
					bool valid;

					if (team_mode == 1 && ctf_options()->engine_type == 9)
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

#pragma inline_depth(1)
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
#pragma inline_depth(255)

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
	short slot = slot_object_get(object_index)->slot;

	if (slot != NONE)
	{
		color3f buffer;
		color3f color = *function_7f720(&buffer, slot);

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

// @retail 0x244300
bool function_244300(long object_index, long *player_index)
{
	real radius = mp_globals_block()->constants->pickup_radius;
	real radius_squared = radius * radius;
	short slot = slot_object_get(object_index)->slot;
	long team = slot == 8 ? NONE : slot;
	point3f position;

	function_b9dd0(object_index, &position);
	if (slot != NONE)
	{
		s_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (function_19f300((long *)&iterator))
		{
			long player_team = iterator.player->team;
			bool enemy = ctf_options()->engine_type == 9 ? player_team == team : player_team != team;

			if (enemy || ctf_options()->team_mode == 2)
			{
				point3f unit_position;

				function_b9dd0(iterator.player->unit_index, &unit_position);
				if (distance_sq3f(&unit_position, &position) < radius_squared)
				{
					if (player_index)
						*player_index = iterator.index;
					return true;
				}
			}
		}
	}

	return false;
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

PRIVATE __forceinline real function_2447f1(point3f const *arg_0, point3f const *arg_1)
{
	real local_0 = arg_1->z - arg_0->z;
	real local_1 = arg_1->y - arg_0->y;
	real local_2 = arg_1->x - arg_0->x;
	return local_0 * local_0 + local_1 * local_1 + local_2 * local_2;
}

PRIVATE __forceinline void function_2447f2(long arg_0, s_palette_source_globals const *arg_1, point3f *arg_2)
{
	point3f const *local_0 = &arg_1->marker_entries[arg_0].position;
	long const volatile *local_1 = (long const volatile *)local_0;
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
				if (!(function_2447f1(&position, &marker) > 0.2f * 0.2f))
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
	long volatile best_player_index = NONE;
	long best_score = 0x80000000;
	s_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (function_19f240((long *)&iterator))
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
			while (function_19f300((long *)&iterator))
			{
				if (iterator.index != player_index &&
					iterator.player->team == player->team &&
					function_19f3c0(iterator.index, 1) != NONE &&
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
	event.cause_team = ctf_player_get(player_index)->team;
	event.effect_team = team;
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
		list->r14 = 0.1f;
	else
		list->r14 = 0.8f;
	list->r18 = 0.1f;
	list->r1c = 0.0f;
	list->l20 = object_index;
	list->color24 = *(s_color_bits const *)point;
	list->color30 = *(s_color_bits const *)point;
	list->r3c = 1.0f;
	list->r40 = 1.0f;
	list->count = 0;
	function_2440a0(list, point, object_index);
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
void __stdcall function_15e360(point3f const *point);

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
			point3f point = *marker_position(marker_index);
			s_slot_table *g;
			s_event event;
			s_slot_object *object;

			g_51ec80->w204 |= 1;
			function_15e360(&point);
			g = g_51ec80;
			g->w204 &= ~1;
			g->carriers[slot] = player_index;
			ctf_globals_changed(0x400);
			g->d[slot] = ctf_options()->scale_b;
			if (!ctf_options()->scale_b)
				g->d[slot] = 10;
			g->times[slot] = g_510c54->field_2_3;
			ctf_globals_changed(0x80);
			g->flags[slot] |= 2;
			ctf_globals_changed(0x200);

			game_engine_event_initialize(&event, 10, 13);
			game_engine_event_set_cause_player(&event, player_index);
			event.effect_team = marker_team;
			function_19eb90(&event);

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

// @retail 0x244470
void __stdcall function_244470(long arg_0, long arg_1, long arg_2, long arg_3,
	long arg_4, long arg_5, void *arg_6)
{
	s_marker_entry *local_0 = g_4e0350->marker_entries;
	long local_1;
	real local_2;
	if (arg_5 >= 0)
	{
		local_1 = g_51ec80->b[arg_5];
		local_2 = g_51ec80->bounds[0][arg_5].x;
	}
	else
	{
		local_1 = g_51ec80->c[-1 - arg_5];
		local_2 = g_51ec80->bounds[1][-1 - arg_5].x;
	}
	point3f local_3 = local_0[local_1].position;
	point2f local_4[32];
	for (long local_5 = 0; local_5 < 8; ++local_5)
	{
		real local_6 = (real)local_5 * 0.19634954631328583f;
		real local_7 = (real)sin(local_6) * local_2;
		real local_8 = (real)cos(local_6) * local_2;
		local_4[local_5].x = local_3.x + local_7;
		local_4[local_5].y = local_3.y + local_8;
		local_4[local_5 + 8].x = local_3.x + local_8;
		local_4[local_5 + 8].y = local_3.y - local_7;
		local_4[local_5 + 16].x = local_3.x - local_7;
		local_4[local_5 + 16].y = local_3.y - local_8;
		local_4[local_5 + 24].x = local_3.x - local_8;
		local_4[local_5 + 24].y = local_3.y + local_7;
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
void function_244630(point3f const *arg_0, long arg_1)
{
	byte *local_0 = (byte *)g_4e3b44[g_4e034c->index & 0xffff].data;
	byte *local_1 = *(byte **)(local_0 + 0xc);
	byte *local_2 = *(byte **)(local_1 + 0x534);
	function_41490(0, *(long *)(local_2 + 0xc4), NONE, 640.f,
		(t_record_fill)function_d4bc0, (dword)function_244470, function_244610, (void *)arg_1, arg_0);
}

PRIVATE __forceinline void function_2417a1(s_marker_entry const *arg_0, long arg_1, point3f *arg_2)
{
    long local_0 = *(long const volatile *)&arg_0[arg_1].position.y;
    point3f const *local_1 = &arg_0[arg_1].position;
    ((long *)arg_2)[0] = ((long const *)local_1)[0];
    long local_2 = ((long const *)local_1)[2];
    ((long *)arg_2)[1] = local_0;
    ((long *)arg_2)[2] = local_2;
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
					function_244630(&local_5, local_2);
				}
				local_4 = g_51ec80->c[local_2];
				if (local_4 != NONE)
				{
					function_2417a1(g_4e0350->marker_entries, local_4, &local_5);
					function_244630(&local_5, -1 - local_2);
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

// @retail 0x2419c0
void c_game_engine_markers::v14(long arg_0)
{
	s_marker_list local_14;
	s_marker_list local_31;
	long local_0 = NONE;
	if (arg_0 != NONE)
		local_0 = g_4e8c20->entries[arg_0];
	s_player_view *local_1 = ctf_player_get(local_0);
	bool local_2 = false;
	bool local_3 = false;
	bool local_4 = false;
	if (ctf_options()->engine_type == 9)
	{
		if (function_19f3c0(local_0, 1) != NONE || function_241900(local_0))
		{
			if (ctf_options()->team_mode == 1)
				local_3 = true;
			else
				local_4 = true;
		}
	}
	else if (function_19f3c0(local_0, 1) != NONE || function_241900(local_0))
		local_2 = true;
	if (ctf_options()->team_mode == 2)
	{
		s_team_entry *local_5 = function_15e410(8);
		if (local_5)
		{
			bool local_6 = !(bool)g_51ec80->flag_bits[8].bit0;
			bool local_7 = local_5->lc != NONE;
			bool local_8 = !local_6 && !local_7;
			s_color_bits local_9 = *(s_color_bits *)g_468714;
			bool local_10 = false;
			bool local_11 = false;
			if (local_7 && ctf_player_get(local_5->lc)->team == local_1->team)
				local_10 = true;
			if (ctf_options()->engine_type == 9 && g_51ec80->flag_bits[8].bit3)
				local_11 = true;
			bool local_12 = false;
			long local_13 = *(long *)((byte *)g_4e6948 + 0x23c);
			if (!local_7 || !local_10)
			{
				if (local_6 || local_8)
					local_12 = local_13 != 3;
				else if (local_7 && !local_10)
				{
					if (ctf_options()->engine_type == 9)
					{
						switch (local_13)
						{
						case 0: local_12 = true; break;
						case 2: local_12 = local_11; break;
						}
					}
					else
						local_12 = local_13 == 1 || local_13 == 2;
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
							point3f local_20 = g_4e0350->marker_entries[local_19].position;
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
				long local_30 = *(long *)((byte *)g_4e6948 + 0x23c);
				if (!local_23 || !local_25)
				{
					if (ctf_options()->engine_type == 9)
					{
						if (local_15 == local_1->team)
							local_29 = true;
						else
						{
							switch (local_30)
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
						switch (local_30)
						{
						case 0: local_29 = local_24; break;
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
	if (local_0 != NONE && ctf_player_get(local_0)->unit_index != NONE)
	{
		s_player_iterator local_32;
		local_32.data = g_4e8c24;
		local_32.absolute_index = NONE;
		local_32.index = NONE;
		while (function_19f240((long *)&local_32))
		{
			if (local_32.index != local_0)
			{
				if (!function_2419c1(local_32.player->team, ctf_player_get(local_0)->team))
				{
					if (function_162550(local_32.index, &local_14))
					{
						long local_35 = function_19f3c0(local_32.index, 1);
						if (local_35 != NONE)
						{
							long local_36 = slot_object_get(local_35)->slot;
							if (local_36 != NONE)
							{
								color3f local_37;
								s_color_bits local_38 = *(s_color_bits *)(local_36 == 8 ? g_468714 : function_7f720(&local_37, (short)local_36));
								function_2440a0(&local_14, (point3f *)&local_38, local_35);
							}
						}
						function_24e59f(&local_14);
					}
				}
			}
		}
	}
}
