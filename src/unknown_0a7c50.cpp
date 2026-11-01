#include "unknown_11c920.h"
#include "globals.h"
#include "game_engine_events.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

/* Sends a game engine event to the clients (as a simulation event of type 7,
   with the player indices made absolute). Decompiled by lane O because the
   engines (unknown_2420a0.cpp, juggernaut.cpp, unknown_19de80.cpp) pass the
   event in a register. */

long g_4cedf0;

void __stdcall function_b5a70(long a, long type, long b, long c, long size, void const *data, long d);

PRIVATE inline bool event_mode_sends(long mode)
{
    switch (mode)
    {
    case 2:
    case 4:
        return false;
    default:
        return true;
    }
}

// @retail 0xa7c50
void function_a7c50(s_event *event)
{
	long mode = g_4e6948->mode;

	if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
	{
		s_event copy = *event;

		if (copy.a != NONE)
			copy.a &= 0xffff;
		if (copy.cause_player_index != NONE)
			copy.cause_player_index &= 0xffff;
		if (copy.effect_player_index != NONE)
			copy.effect_player_index &= 0xffff;
		function_b5a70(NONE, 7, 0, 0, sizeof(copy), &copy, g_4cedf0);
	}
}

void function_b58c0(long index, dword mask);
long function_184400(long a, long b);

// @retail 0xa7810
void __stdcall function_a7810(dword mask)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
		function_b58c0(g_4e9ae8->value24, mask);
}

// @retail 0xa7840
void __stdcall function_a7840(short index, dword mask)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->slots[index] != NONE)
		function_b58c0(g_4e9ae8->slots[index], mask);
}

// @retail 0xa8ee0
void function_a8ee0(long a, long b)
{
	long mode = g_4e6948->mode;
	if (mode >= 4 && mode <= 5 && mode != 4)
	{
		long identifier = function_184400(a, b);
		if (identifier != NONE)
			function_b58c0(identifier, 1);
	}
}


bool function_a76b0(long index, long which);
bool function_a7700(long index, long which, long *player_out);
void __stdcall function_b5ba0(long player_index, long type, long count, long object_indices,
    long size, void const *data, long timeout);

long g_4ceee0;
long g_4cef08;
long g_4cef44;
long g_4cef58;

// @retail 0xa90e0
void function_a90e0(long first, long second)
{
    long objects[2];
    if (function_a76b0(first, 0))
    {
        objects[0] = second;
        objects[1] = first;
        function_b5a70(NONE, 0x13, 2, (long)objects, 0, 0, g_4ceee0);
    }
}

// @retail 0xa9180
void function_a9180(long first, long second)
{
    long objects[2];
    if (function_a76b0(first, 0))
    {
        objects[0] = second;
        objects[1] = first;
        function_b5a70(NONE, 0x15, 2, (long)objects, 0, 0, g_4cef08);
    }
}

struct s_player_pair_event_data
{
    long first;
    long second;
    s_player_pair_event_data() { memset(&second, 0, sizeof(second)); }
};

// @retail 0xa9400
void function_a9400(long first, long second)
{
    s_player_pair_event_data data;
    data.first = first & 0xffff;
    data.second = second & 0xffff;
    function_b5a70(NONE, 0x19, 0, 0, sizeof(data), &data, g_4cef58);
}

// @retail 0xa9340
void function_a9340(long first, long second, short seat)
{
    long objects[2];
    if (function_a76b0(first, 0))
    {
        long data;
        objects[0] = first;
        objects[1] = second;
        data = seat;
        function_b5a70(NONE, 0x18, 2, (long)objects, sizeof(data), &data, g_4cef44);
    }
}

long g_4ceef4;
long g_4cee40;
long g_4cee18;

struct s_event_object_header
{
    byte unknown00[8];
    byte *object;
};
#define EVENT_OBJECT(index) (((s_event_object_header *)g_4e0300->data)[(index) & 0xffff].object)

// @retail 0xa9120
void __stdcall function_a9120(long index, long trick)
{
    long player = NONE;
    bool send = true;
    if (g_4e6948->mode != 4)
        function_a7700(index, 0, &player);
    else
    {
        send = function_a76b0(index, 0);
        player = NONE;
    }
    if (send)
    {
        long data = trick;
        function_b5a70(player, 0x14, 1, (long)&index, sizeof(data), &data, g_4ceef4);
    }
}

// @retail 0xa9390
void function_a9390(long index)
{
    byte *object = EVENT_OBJECT(index);
    long objects[2];
    long seat;
    if (function_a76b0(index, 0))
    {
        long parent = *(long *)(object + 0x14);
        seat = *(short *)(object + 0x1fc);
        objects[0] = index;
        objects[1] = parent;
        function_b5a70(NONE, 0xb, 2, (long)objects, sizeof(seat), &seat, g_4cee40);
    }
}

struct s_player_event_data
{
    short type;
    short unknown02;
    long definition;
    short count;
    short unknown0a;
    s_player_event_data() { memset(&definition, 0, 8); }
};

// @retail 0xa8aa0
void function_a8aa0(long index)
{
    if (index != NONE)
    {
        long player = *(long *)(EVENT_OBJECT(index) + 0x13c);
        if (player != NONE)
        {
            long object = index;
            s_player_event_data data;
            data.definition = NONE;
            data.type = 4;
            function_b5ba0(player, 9, 1, (long)&object, sizeof(data), &data, g_4cee18);
        }
    }
}

// @retail 0xa8a30
void function_a8a30(long index, long definition)
{
    if (index != NONE && definition != NONE)
    {
        long player = *(long *)(EVENT_OBJECT(index) + 0x13c);
        if (player != NONE)
        {
            long object = index;
            s_player_event_data data;
            data.type = 0;
            data.definition = definition;
            function_b5ba0(player, 9, 1, (long)&object, sizeof(data), &data, g_4cee18);
        }
    }
}

// @retail 0xa8950
void function_a8950(long index, long definition)
{
    if (index != NONE && definition != NONE)
    {
        long player = *(long *)(EVENT_OBJECT(index) + 0x13c);
        if (player != NONE)
        {
            long object = index;
            s_player_event_data data;
            data.type = 2;
            data.definition = definition;
            function_b5ba0(player, 9, 1, (long)&object, sizeof(data), &data, g_4cee18);
        }
    }
}

// @retail 0xa89c0
void function_a89c0(long index, long definition)
{
    if (index != NONE && definition != NONE)
    {
        long player = *(long *)(EVENT_OBJECT(index) + 0x13c);
        if (player != NONE)
        {
            long object = index;
            s_player_event_data data;
            data.type = 3;
            data.definition = definition;
            function_b5ba0(player, 9, 1, (long)&object, sizeof(data), &data, g_4cee18);
        }
    }
}

long g_4ceea4;
long g_4ced78;
long g_4cee04;
long g_4cef1c;
long g_4ceeb8;
long g_4ceecc;
long g_4cee2c;
long function_a5930(long index);
short function_101280(long index);

// @retail 0xa8f10
void function_a8f10(long index, long target, short value)
{
    struct s_pair_message
    {
        long data[2];
        long objects[2];
        s_pair_message(long source, long target, short value)
        {
            objects[1] = target;
            objects[0] = source;
            data[0] = *(long *)(EVENT_OBJECT(source) + 0x210);
            data[1] = value;
        }
    };
    if (function_a76b0(index, 1))
    {
        s_pair_message message(index, target, value);
        function_b5a70(NONE, 0x10, 2, (long)message.objects, sizeof(message.data), message.data, g_4ceea4);
    }
}

struct s_region_event_data
{
    long region;
    long permutation;
    long active;
    s_region_event_data() { memset(&permutation, 0, 8); }
};

// @retail 0xa8360
void function_a8360(long index, long region, long permutation, long active)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode) && function_a5930(index) != NONE)
    {
        long object = index;
        s_region_event_data data;
        data.region = region;
        data.permutation = permutation;
        data.active = active;
        function_b5a70(NONE, 1, 1, (long)&object, sizeof(data), &data, g_4ced78);
    }
}

// @retail 0xa8b10
void function_a8b10(long index)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        byte *object = EVENT_OBJECT(index);
        long parent = *(long *)(object + 0x14);
        if (parent != NONE && *(short *)(object + 0x1fc) != NONE)
        {
            long objects[2];
            long seat;
            objects[0] = index;
            objects[1] = parent;
            seat = *(short *)(object + 0x1fc);
            function_b5a70(NONE, 11, 2, (long)objects, sizeof(seat), &seat, g_4cee40);
        }
    }
}

// @retail 0xa8b90
void __stdcall function_a8b90(long index)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        byte *object = EVENT_OBJECT(index);
        long parent = *(long *)(object + 0x14);
        if (parent != NONE && *(short *)(object + 0x1fc) != NONE)
        {
            long objects[2];
            long seat;
            objects[0] = index;
            objects[1] = parent;
            seat = *(short *)(object + 0x1fc);
            function_b5a70(NONE, 8, 2, (long)objects, sizeof(seat), &seat, g_4cee04);
        }
    }
}

struct s_direction_event_data
{
    short value;
    short unknown02;
    point3f origin;
    vector3f direction;
};

// @retail 0xa91c0
void function_a91c0(long index, short value, point3f const *origin, vector3f const *direction)
{
    s_direction_event_data data;
    long current = index;
    if (function_a76b0(current, 1))
    {
        byte *object = EVENT_OBJECT(current);
        if (*(signed char *)(object + 0x23c) >= 0 && *(signed char *)(object + 0x23c) < 2)
        {
            data.value = value;
            data.origin = *origin;
            data.direction = *direction;
            function_b5a70(NONE, 0x16, 1, (long)&index, sizeof(data), &data, g_4cef1c);
        }
    }
}

struct s_slot_event_data
{
    short slot;
    short selected;
    long definition;
};

// score 17 (was 17): the constant 0x212 is folded into the load in ours and
// into the address in retail, so ours reads the byte at [eax+ecx+0x212] and
// retail computes the address first and reads the byte at [ecx]. Tried: the
// unit and header typed as the weapon slot query's own views, the query doing
// the NONE test 60, with the slot tested first 50; the header pointer without
// volatile 48, the selection without volatile 53, read at each use 55.
// Round 118 (no volatile, so both reads are cached): the slot stored into the
// event data before the other fields 59 (the selection spelled three ways, 59
// each); the slot stored last 59; the header read from the global at the
// second lookup 60; a char first read with a signed char second read 60; the
// item's object in a local 59. The volatile selection pointer without the
// constant (indexed by it at both reads) 27. (Round 118 listing: ours folds 0x212
// into the address, `lea ecx, [edx+eax+0x212]` then `[ecx]`; retail keeps
// object+slot in ecx and reads `[ecx+0x212]`, as 0xa9030's note says.)
// @retail 0xa8f80
void function_a8f80(long index, short slot)
{
    s_slot_event_data data;
    long current = index;
    if (function_a76b0(current, 1))
    {
        s_record_pool *objects = g_4e0300;
        byte *volatile *headers = &objects->data;
        byte *object = ((s_event_object_header *)*headers)[current & 0xffff].object;
        long offset = slot;
        signed char const volatile *selection = (signed char const volatile *)(object + offset + 0x212);
        short selected = selection[0];
        if (selected != NONE)
        {
            long item = ((long *)(object + 0x218))[selected];
            if (item != NONE)
            {
                data.selected = selection[0];
                data.definition = *(long *)((s_event_object_header *)*headers)[item & 0xffff].object;
                data.slot = slot;
                function_b5a70(NONE, 17, 1, (long)&index, sizeof(data), &data, g_4ceeb8);
            }
        }
    }
}

// score 17 (was 17): ours folds the constant 0x212 into the address (lea of
// object+slot+0x212, byte read at [ecx]); retail keeps object+slot in ecx and
// reads the byte at [ecx+0x212] twice. Round 40, without the volatile reads:
// 0xcbd50 with the selection byte through the object local 63; with the
// current copy 60; lookup in the argument and again for the second read 59;
// header read volatile only, definition before/after the second read 56/55.
// @retail 0xa9030
void function_a9030(long index, short slot)
{
    s_slot_event_data data;
    long current = index;
    if (function_a76b0(current, 1))
    {
        s_record_pool *objects = g_4e0300;
        byte *volatile *headers = &objects->data;
        byte *object = ((s_event_object_header *)*headers)[current & 0xffff].object;
        long offset = slot;
        signed char const volatile *selection = (signed char const volatile *)(object + offset + 0x212);
        short selected = selection[0];
        if (selected != NONE)
        {
            long item = ((long *)(object + 0x218))[selected];
            if (item != NONE)
            {
                data.selected = selection[0];
                data.definition = *(long *)((s_event_object_header *)*headers)[item & 0xffff].object;
                data.slot = slot;
                function_b5a70(NONE, 18, 1, (long)&index, sizeof(data), &data, g_4ceecc);
            }
        }
    }
}

struct s_position_event_data
{
    long value;
    point3f position;
    s_position_event_data() { memset(&position, 0, sizeof(position)); }
};

// @retail 0xa8040
void function_a8040(long index, long value, point3f const *position)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode) &&
        *(long *)(EVENT_OBJECT(index) + 0xd4) != NONE && function_101280(index) == 1)
    {
        long object = index;
        s_position_event_data data;
        data.position = *position;
        data.value = value;
        function_b5a70(NONE, 0xa, 1, (long)&object, sizeof(data), &data, g_4cee2c);
    }
}

// @retail 0xa88a0
void function_a88a0(long index, long first_definition, long second_definition, long count)
{
    if (index != NONE && first_definition != NONE && second_definition != NONE)
    {
        long player = *(long *)(EVENT_OBJECT(index) + 0x13c);
        if (player != NONE)
        {
            long object = index;
            s_player_event_data data;
            long type_mask = 1 << *(byte *)g_4e3b44[first_definition & 0xffff].bytes;
            data.definition = (type_mask & 4) ? first_definition : second_definition;
            data.count = count;
            data.type = 1;
            function_b5ba0(player, 9, 1, (long)&object, sizeof(data), &data, g_4cee18);
        }
    }
}

long g_4cee68;
long g_4cee90;
long g_4cee7c;
long g_4cedb4;
long g_4ceda0;
long g_4ced8c;

PRIVATE __forceinline long event_player(long index)
{
    long player = NONE;
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5)
    {
        byte *object = EVENT_OBJECT(index);
        if ((1 << object[0xaa]) & 3)
        {
            player = *(long *)(object + 0x13c);
            if (player == NONE && *(long *)(object + 0x24c) != NONE)
                player = *(long *)(EVENT_OBJECT(*(long *)(object + 0x24c)) + 0x13c);
        }
    }
    return player;
}

// @retail 0xa8dd0
void function_a8dd0(long index)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5)
    {
        long player;
        bool send = true;
        if (mode != 4)
            player = event_player(index);
        else
        {
            send = function_a76b0(index, 1);
            player = NONE;
        }
        if (send)
        {
            function_b5a70(player, 0xd, 1, (long)&index, 0, 0, g_4cee68);
        }
    }
}

struct s_short_event_data
{
    short value;
};

// @retail 0xa8cf0
void __stdcall function_a8cf0(long index, long target, long value)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5)
    {
        long player;
        bool send = true;
        if (mode != 4)
            player = event_player(index);
        else
        {
            send = function_a76b0(index, 1);
            player = NONE;
        }
        if (send)
        {
            long objects[2];
            s_short_event_data data;
            objects[1] = target;
            data.value = (short)value;
            objects[0] = index;
            function_b5a70(player, 0xf, 2, (long)objects, sizeof(data), &data, g_4cee90);
        }
    }
}

// @retail 0xa8c10
void __stdcall function_a8c10(long index)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5)
    {
        byte *object = EVENT_OBJECT(index);
        signed char selected = *(signed char *)(object + 0x23c);
        if (selected >= 0 && selected < 2)
        {
            long player;
            bool send = true;
            if (mode != 4)
                player = event_player(index);
            else
            {
                send = function_a76b0(index, 1);
                player = NONE;
            }
            if (send)
            {
                s_short_event_data data;
                data.value = *(signed char *)(object + 0x23c);
                function_b5a70(player, 0xe, 1, (long)&index, sizeof(data), &data, g_4cee7c);
            }
        }
    }
}

struct s_object_event_data
{
    long definition;
    point3f position;
    vector3f forward;
    dword flags;
    vector3f vector;
    short material;
    short unknown2e;
};
point3f *function_b9dd0(long object_index, point3f *position);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);

// @retail 0xa84e0
void __stdcall function_a84e0(long index, short *material, vector3f const *vector, dword flags)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        byte *object = EVENT_OBJECT(index);
        if (*(long *)(object + 0xd4) != NONE)
        {
            s_object_event_data data;
            memset(&data, 0, sizeof(data));
            data.definition = *(long *)object;
            function_b9dd0(index, &data.position);
            vector3f up;
            function_b9fc0(index, &data.forward, &up);
            data.flags = flags;
            if (flags & 4)
            {
                data.material = *material;
                data.vector = *vector;
            }
            function_b5a70(NONE, 4, 0, 0, sizeof(data), &data, g_4cedb4);
        }
    }
}

struct s_attachment_event_data
{
    bool attached;
    byte unknown01;
    short node;
    point3f point;
    long reference;
    short region;
    short material;
    s_attachment_event_data()
    {
        memset(&node, 0, 22);
    }
};

// @retail 0xa83e0
void function_a83e0(long index, long parent, point3f const *point, short node, vector3f const *reference)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        if (*(long *)(EVENT_OBJECT(index) + 0xd4) != NONE)
        {
            long objects[2];
            s_attachment_event_data data;
            objects[0] = index;
            objects[1] = parent;
            data.node = (short)node;
            data.point = *point;
            data.attached = parent != NONE;
            if (reference)
                memcpy(&data.reference, reference, 8);
            else
            {
                data.reference = NONE;
                data.region = NONE;
                data.material = g_4686c4;
            }
            function_b5a70(NONE, 3, 2, (long)objects, sizeof(data), &data, g_4ceda0);
        }
    }
}

struct s_effect_event_data
{
    long first;
    long second;
    long third;
    long field0c;
    long field10;
    bool no_direction;
    byte unknown15[3];
    vector3f direction;
    point3f position;
    long field30;
};

// @retail 0xa87a0
void function_a87a0(long first, long second, long third, byte const *input)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        s_effect_event_data data;
        memset((byte *)&data + 4, 0, sizeof(data) - 4);
        vector3f zero;
        memset(&zero, 0, sizeof(zero));
        data.first = first;
        data.second = second;
        data.third = third;
        data.field30 = *(long *)input;
        vector3f const *direction = (vector3f const *)(input + 0x3c);
        if (memcmp(direction, g_4687b0, sizeof(*direction)) && memcmp(direction, &zero, sizeof(*direction)))
            data.direction = *direction;
        else
            data.no_direction = true;
        data.position = *(point3f const *)(input + 0x30);
        data.field0c = *(long *)(input + 0x1c);
        data.field10 = *(long *)(input + 0x20);
        function_b5a70(NONE, 2, 0, 0, sizeof(data), &data, g_4ced8c);
    }
}

long g_4ceddc;
long g_4cedc8;
long g_4ced64;
long g_4cee54;

struct s_collision_result_1697c0;
struct s_collision_event_data
{
    long definition;
    real first_scale;
    real second_scale;
    vector3f direction;
    point3f point;
    vector3f normal;
    short material;
    short unknown32;
    s_collision_event_data() { memset(&first_scale, 0, 44); }
};
struct s_collision_target_event_data : s_collision_event_data
{
    bool flag;
    byte unknown35[3];
    long node;
    s_collision_target_event_data() { memset(&flag, 0, 8); }
};

struct s_damage_report;
struct s_object;
s_object *function_badc0(long index, dword mask);
real function_30bf0(vector3f *vector);
struct s_damage_event_data
{
    long definition;
    long unknown04;
    short player;
    bool has_direction;
    byte unknown0b;
    vector3f direction;
    real scale;
    real field1c;
    dword flags;
    real field24;
    real field28;
    short field2c;
    short field2e;
    long field30;
    byte field34;
    byte unknown35[3];
};

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the unit flags word at +0x10a that 0xa80f0 tests */
struct s_a80f0_unit_flags
{
    byte unknown000[0x10a];
    word unknown0 : 2;
    word suppressed : 1;
    word unknown3 : 13;
};

// @retail 0xa80f0
void function_a80f0(long index, s_damage_report const *report)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        s_event_object_header *header = &((s_event_object_header *)g_4e0300->data)[index & 0xffff];
        byte *object = header->object;
        byte const *input = (byte const *)report;
        long definition_index = *(long const *)(input + 8);
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        byte *local_446d88 = g_4e3b44[definition_index & 0xffff].bytes;
        long type = header->unknown00[3];
        long mask = 1 << type;
        long player = NONE;
        if (mask & 3)
            player = *(long *)(object + 0x13c);
        if ((mask & 3) || ((1u << type) & 0x800))
        {
            real strength = *(real *)(local_446d88 + 0x40) * *(real *)(definition + 0x14);
            bool send = strength > 0.0001f;
            if (!send && (mask & 3) && !TEST_FIELD_BIT(((s_a80f0_unit_flags *)object)->suppressed))
                send = *(real const *)(input + 0x44) > 0.0f || *(real const *)(input + 0x48) > 0.0f;
            if (!send)
                send = player != NONE && *(long const *)(input + 0xc) != NONE && *(long const *)(input + 0xc) != player;
            if (send && function_a5930(index) != NONE)
            {
                long objects[2];
                objects[0] = index;
                long other = *(long const *)(input + 0x10);
                objects[1] = other != NONE && function_badc0(other, (dword)NONE) ? other : NONE;
                s_damage_event_data data;
                memset(&data, 0, sizeof(data));
                data.definition = definition_index;
                data.player = *(short const *)(input + 0xc);
                data.direction = *(vector3f const *)(input + 0x18);
                if (function_30bf0(&data.direction) > 0.0001f)
                    data.has_direction = true;
                data.scale = PIN(*(real const *)(input + 0x34), 0.0f, 2.0f);
                data.field1c = PIN(*(real const *)(input + 0x38), 0.0f, 2.0f);
                data.flags = *(dword const *)(input + 4);
                data.field30 = *(long const *)(input + 0x50);
                data.field28 = PIN(*(real const *)(input + 0x44), 0.0f, 9.0f);
                data.field24 = PIN(*(real const *)(input + 0x48), 0.0f, 3.0f);
                data.field2c = *(short const *)(input + 0x3c);
                data.field2e = *(short const *)(input + 0x40);
                data.field34 = input[0];
                function_b5a70(NONE, 0, 2, (long)objects, sizeof(data), &data, g_4ced64);
            }
        }
    }
}

struct s_object_relevance_source;
struct s_object_relevance_result
{
    real first;
    real second;
    long object_index;
    long identifier;
};
void function_82ac0(s_object_relevance_source *source, s_object_relevance_result *result);
void function_cb7e0(long index, vector3f *vector);
long function_101f50(long index);
long function_1b8c80(long index);
long function_a5980(long index);
bool function_138880();

struct s_action_event_data
{
    long slot;
    long definition;
    long group;
    bool field_c_9;
    byte unknown0d;
    short node;
    point3f point;
    vector3f direction;
    s_object_relevance_result relevance;
    bool linked;
    byte unknown39[3];
    long linked_identifier;
    long linked_slot;
};
