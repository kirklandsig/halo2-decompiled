// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A7D50.CPP: sending the action and collision events of an object to
   the clients (built with SSE, apart from its neighbours in
   unknown_0a7c50.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "game_engine_events.h"
#include <string.h>

extern long g_4cee54;
extern long g_4ceddc;
extern long g_4cedc8;

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

struct s_event_object_header
{
    byte unknown00[8];
    byte *object;
};
#define EVENT_OBJECT(index) (((s_event_object_header *)g_4e0300->data)[(index) & 0xffff].object)

bool function_a76b0(long index, long which);
long function_a5930(long index);

PRIVATE __forceinline long event_player(s_event_object_header const *headers, long index, long mode)
{
    long player = NONE;
    if (4 <= mode && 5 >= mode)
    {
        byte *object = headers[index & 0xffff].object;
        if ((1 << object[0xaa]) & 3)
        {
            player = *(long *)(object + 0x13c);
            if (player == NONE && *(long *)(object + 0x24c) != NONE)
                player = *(long *)(headers[*(long *)(object + 0x24c) & 0xffff].object + 0x13c);
        }
    }
    return player;
}

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

// @retail 0xa7d50
void function_a7d50(long index, long group, long target, short node, point3f const *point)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5)
    {
        s_event_object_header *headers = (s_event_object_header *)g_4e0300->data;
        byte *object = headers[index & 0xffff].object;
        byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
        byte *entry = *(byte **)(definition + 0x2d4) + group * 0xec;
        if (*(short *)(entry + 0x34) != 1)
        {
            long parent = NONE;
            if (object[0x12c] & 1)
                parent = *(long *)(object + 0x154);
            long slot = function_101f50(index);
            long objects[2] = {NONE, NONE};
            long source = NONE;
            if (parent != NONE)
            {
                if (slot >= 0 && slot < 4)
                {
                    long linked = *(long *)(headers[parent & 0xffff].object + 0x24c);
                    objects[0] = parent;
                    source = linked != NONE ? linked : parent;
                }
            }
            else if (slot == NONE)
                objects[0] = index;
            long subject = objects[0];
            s_event_object_header *header = &headers[subject & 0xffff];
            if (header->unknown00[3] == 1)
            {
                long linked = *(long *)(header->object + 0x24c);
                if (linked == NONE || *(long *)(headers[linked & 0xffff].object + 0x13c) == NONE)
                    subject = function_1b8c80(subject);
            }
            if (subject != NONE)
            {
                long player;
                bool send = true;
                if (mode != 4)
                    player = event_player(headers, subject, mode);
                else
                {
                    send = function_a76b0(subject, 1);
                    player = NONE;
                }
                if (send)
                {
                    s_action_event_data data;
                    memset(&data, 0, sizeof(data));
                    data.slot = slot;
                    data.definition = *(long *)object;
                    data.group = group;
                    data.relevance.first = 0;
                    data.relevance.second = 0;
                    data.relevance.object_index = NONE;
                    data.relevance.identifier = NONE;
                    data.direction = *g_4687a8;
                    if (target != NONE)
                    {
                        objects[1] = target;
                        data.node = node;
                        data.point = *point;
                        data.field_c_9 = true;
                    }
                    if (source != NONE)
                    {
                        function_82ac0((s_object_relevance_source *)(EVENT_OBJECT(source) + 0x1c8), &data.relevance);
                        function_cb7e0(source, &data.direction);
                    }
                    if ((bool)((*(dword *)entry >> 13) & 1))
                    {
                        long linked = *(long *)(object + 0x194);
                        if (linked != NONE)
                        {
                            long identifier = function_138880() ? function_a5980(linked) : function_a5930(linked);
                            if (identifier != NONE)
                            {
                                data.linked_identifier = identifier;
                                data.linked_slot = *(signed char *)(object + 0x177);
                                data.linked = true;
                            }
                        }
                    }
                    function_b5a70(player, 0xc, 2, (long)objects, sizeof(data), &data, g_4cee54);
                }
            }
        }
    }
}
// @retail 0xa85c0
void function_a85c0(long index, s_collision_result_1697c0 const *collision,
    vector3f const *direction, bool flag, real first_scale, real second_scale)
{
    long mode = g_4e6948->mode;
    if (mode >= 4 && mode <= 5 && event_mode_sends(mode))
    {
        byte *object = EVENT_OBJECT(index);
        if (*(long *)(object + 0xd4) != NONE)
        {
            byte const *input = (byte const *)collision;
            long target;
            if (*(long *)input == 4 && (target = *(long *)(input + 0x40)) != NONE && *(long *)(EVENT_OBJECT(target) + 0xd4) != NONE)
            {
                struct { long target; s_collision_target_event_data data; } packet;
                s_collision_target_event_data &data = packet.data;
                packet.target = target;
                data.definition = *(long *)object;
                data.direction = *direction;
                data.first_scale = first_scale;
                data.second_scale = second_scale;
                data.normal = *(vector3f const *)(input + 0x28);
                data.point = *(point3f const *)(input + 8);
                data.material = *(short const *)(input + 0x24);
                data.node = *(short const *)(input + 0x46);
                data.flag = flag;
                function_b5a70(NONE, 6, 1, (long)&packet.target, sizeof(data), &data, g_4ceddc);
            }
            else
            {
                struct { long target; s_collision_event_data data; } packet;
                s_collision_event_data &data = packet.data;
                data.definition = *(long *)object;
                data.first_scale = first_scale;
                data.second_scale = second_scale;
                data.direction = *direction;
                data.normal = *(vector3f const *)(input + 0x28);
                data.point = *(point3f const *)(input + 8);
                data.material = *(short const *)(input + 0x24);
                function_b5a70(NONE, 5, 0, 0, sizeof(data), &data, g_4cedc8);
            }
        }
    }
}
