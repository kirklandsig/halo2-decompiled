#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /arch:SSE /Gr

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);
bool function_e68c0(long type, long unit_index);
bool function_f12e0(long unit_index);
bool __stdcall function_14e200(long player_index, long target_index, point3f const *position, bool detach);

// @retail 0x14e970
bool function_14e970(long player_index, point3f const *position, long target_index)
{
	long unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);
	s_object *unit = function_badc0(unit_index, 1);
	bool result = false;
	if (unit)
	{
		if (*(long *)((byte *)unit + 0x14) != NONE)
			function_e68c0(0x1e, unit_index);
		point3f adjusted;
		real offset = 0.03280000016093254f;
		adjusted.x = offset * g_4687b0->i + position->x;
		adjusted.y = offset * g_4687b0->j + position->y;
		adjusted.z = offset * g_4687b0->k + position->z;
		result = function_14e200(player_index, target_index, &adjusted, false);
	}
	return result;
}

// @retail 0x14e0f0
void __stdcall function_14e0f0(long player_index, long target_index, point3f const *position,
	point3f const *alternate)
{
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	long unit_index = *(long *)(player + 0x2c);
	if (unit_index != NONE)
	{
		byte *headers = g_4e0300->data;
		byte *unit = *(byte **)(headers + (unit_index & 0xffff) * 12 + 8);
		long parent = *(long *)(unit + 0x14);
		if (parent != NONE && function_baf80(parent) != function_baf80(target_index))
		{
			long root = function_baf80(unit_index);
			bool moved = false;
			if (alternate && (1 << headers[(root & 0xffff) * 12 + 3]) & 2)
				if (function_f12e0(root))
					moved = function_14e200(player_index, NONE, alternate, true);
			if (moved)
				return;
			function_e68c0(0x1e, *(long *)(player + 0x2c));
		}
		if (*(long *)(unit + 0x14) == NONE)
		{
			if (!function_14e200(player_index, target_index, position, true))
			{
				byte *state = (byte *)g_4e8c20;
				state[0x9e] = true;
				state[0x9f] = true;
				*(long *)(state + 0xa0) = 0;
			}
		}
	}
}


#include <math.h>

struct s_motion_sensor_globals;
extern s_motion_sensor_globals *g_51e994;

/* The nine fixed placement offsets used around an attachment target. */
extern vector3f const g_453918[9] = {
    {1,0,0}, {0,1,0}, {0,-1,0},
    {0.7071067690849304f,-0.7071067690849304f,0},
    {0.7071067690849304f,0.7071067690849304f,0},
    {0.5773502588272095f,0.5773502588272095f,0.5773502588272095f},
    {0.5773502588272095f,0.5773502588272095f,-0.5773502588272095f},
    {0.5773502588272095f,-0.5773502588272095f,0.5773502588272095f},
    {0.5773502588272095f,-0.5773502588272095f,-0.5773502588272095f}
};

void function_14cad0(long player_index, long unit_index);
void function_152340(void);
void function_b7300(long unit_index);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, long flags);
void function_b8b70(long object_index);
void __stdcall function_b9a50(long object_index);
bool __stdcall function_c5460(long object_index, long ignore_index, point3f const *position,
    point3f *result, long unknown, real radius, long mode);
real function_30bf0(vector3f *vector);
__declspec(noinline) void function_1420f0(transform4x3f *out, point3f const *position,
    vector3f const *forward, vector3f const *up);
bool function_11c470(long trigger_index, point3f const *position);
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void function_b7740(long object_index, vector3f const *linear, vector3f const *angular, bool skip);
void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);

static inline byte *attachment_object(long index)
{
    return *(byte **)(g_4e0300->data + (index & 0xffff) * 12 + 8);
}

// @retail 0x14ee20
void function_14ee20(long player_index, long target_index)
{
    long const *target_reference = &target_index;
    byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
    long unit_index = *(long *)(player + 0x2c);
    if (unit_index != NONE)
    {
        byte *unit = attachment_object(unit_index);
        *(long *)(player + 0x30) = unit_index;
        function_14cad0(player_index, NONE);
        function_152340();
        function_b7300(unit_index);
        byte *current = attachment_object(unit_index);
        if (!(current[4] & 1))
        {
            if (function_b9d20(unit_index))
                function_bef30(unit_index, 1, 0, 0, 0);
            *(dword *)(current + 4) |= 1;
            function_b8b70(unit_index);
        }
        if (*target_reference != NONE)
            *(long *)(player + 0x30) = *target_reference;
        byte *state = (byte *)g_4e8c20;
        state[4] = false;
        state[5] = false;
        if (*(long *)(unit + 0x14) != NONE)
            function_b9a50(unit_index);
        *(long *)((byte *)g_4e8c20 + 0xac) = unit_index;
    }
}

// @retail 0x14e200
bool __stdcall function_14e200(long player_index, long target_index, point3f const *position, bool detach)
{
    long const *target_reference = &target_index;
    s_record_pool volatile const *objects = g_4e0300;
    byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
    long unit_index = *(long *)(player + 0x2c);
    byte *unit = *(byte **)(objects->data + (unit_index & 0xffff) * 12 + 8);
    bool result = false;
    union { point3f point; vector3f vector; } location;
    point3f &placed = location.point;
    if (*target_reference != NONE && function_baf80(*target_reference) != *target_reference)
    {
        long root = function_baf80(unit_index);
        target_index = function_baf80(*target_reference);
        byte *headers = objects->data;
        long volatile target_snapshot = *target_reference;
        byte *target = *(byte **)(headers + (target_snapshot & 0xffff) * 12 + 8);
        real placement_radius = ((1 << headers[(root & 0xffff) * 12 + 3]) & 2) ? 2.0f : 0.35f;
        vector3f &direction = location.vector;
        direction = *(vector3f *)(target + 0x88);
        if (!(direction.j * direction.j + direction.i * direction.i > 0.0f))
        {
            if (*(real *)(target + 0x78) < 0.7071067690849304f)
                direction = *(vector3f *)(target + 0x70);
            else
                direction = *(vector3f *)(target + 0x7c);
        }
        byte const *definition = g_4e3b44[*(long *)unit & 0xffff].bytes;
        real search_radius = *(real const *)(definition + 0x270);
        real offset_scale = search_radius * 3.0f + *(real *)(target + 0x3c);
        direction.i = 0.0f - direction.i;
        direction.j = 0.0f - direction.j;
        direction.k = 0.0f;
        function_30bf0(&direction);
        transform4x3f frame;
        function_1420f0(&frame, (point3f *)(target + 0x30), &direction, g_4687b0);
        for (long pass = target_snapshot == NONE ? 1 : 0; pass < 2 && !result; ++pass)
        {
            long ignore = pass == 0 ? target_snapshot : NONE;
            for (unsigned short offset_index = 0; offset_index < 9 && !result; ++offset_index)
            {
                real x = g_453918[(short)offset_index].i;
                real y = g_453918[(short)offset_index].j;
                real z = g_453918[(short)offset_index].k;
                if (offset_scale != 1.0f)
                {
                    x *= offset_scale;
                    y *= offset_scale;
                    z *= offset_scale;
                }
                placed.x = frame.up.i * z + frame.left.i * y + frame.forward.i * x + frame.position.x;
                placed.y = frame.up.j * z + frame.left.j * y + frame.forward.j * x + frame.position.y;
                placed.z = frame.up.k * z + frame.left.k * y + frame.forward.k * x + frame.position.z;
                result = function_c5460(root, ignore, &placed, NULL, *(long *)&placement_radius, 1.0f, 0);
                for (short attempt = 0; attempt < 8 && !result; ++attempt)
                {
                    g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
                    short random_index = (short)(((g_4e7408->unknown0 >> 16) * 1026) >> 16);
                    vector3f random_offset = g_4417f0[random_index];
                    point3f candidate;
                    candidate.x = random_offset.i * search_radius + placed.x;
                    candidate.y = random_offset.j * search_radius + placed.y;
                    candidate.z = random_offset.k * search_radius + placed.z;
                    result = function_c5460(root, ignore, &candidate, NULL, *(long *)&placement_radius, 1.0f, 0);
                }
            }
        }
    }
    else
        result = function_c5460(function_baf80(unit_index), target_index, position, NULL, 0, 1.0f, 0);
    *(short *)(player + 0x2a) = NONE;
    if (result)
    {
        byte const *scenario = (byte const *)g_4e0350;
        for (short i = 0; i < *(long const *)(scenario + 0x130); ++i)
        {
            byte const *trigger = *(byte const **)(scenario + 0x134) + i * 0xe;
            if (*(short const *)(trigger + 2) == g_4686c4 && *(long *)(player + 0x2c) != NONE &&
                function_11c470(*(short const *)trigger, (point3f *)(attachment_object(*(long *)(player + 0x2c)) + 0x30)))
            {
                result = false;
                break;
            }
        }
    }
    if (!result)
    {
        function_14ee20(player_index, target_index);
        return result;
    }
    *(vector3f *)(unit + 0x88) = *g_4687a4;
    short user_index = *(short *)(player + 0x28);
    if (user_index != NONE)
    {
        byte *sensor_globals = (byte *)g_51e994;
        byte *sensor = sensor_globals + user_index * 0x2f0;
        *(long *)(sensor_globals + 0xbc4) = 0;
        *(long *)(sensor + 0x2ec) = 0;
        *(long *)(sensor + 0x2e8) = 0;
        for (long i = 0; i < 10; ++i)
            *(long *)(sensor + 0x40 + i * 0x44) = 0;
    }
    if (target_index != NONE)
    {
        byte *target = attachment_object(target_index);
        vector3f direction = *(vector3f *)(target + 0x70);
        *(vector3f *)(unit + 0x150) = direction;
        *(vector3f *)(unit + 0x15c) = direction;
        *(vector3f *)(unit + 0x180) = direction;
        if (user_index != NONE)
        {
            s_unknown_185ab0_entry *view = &g_4ed284->entries[user_index];
            view->yaw = (real)atan2(direction.j, direction.i);
            view->pitch = (real)atan2(direction.k, sqrt(direction.i * direction.i + direction.j * direction.j));
            if (view->yaw < 0.0f)
                view->yaw += 6.2831854820251465f;
        }
        if (((1 << g_4e0300->data[(target_index & 0xffff) * 12 + 3]) & 1) &&
            ((1 << g_4e0300->data[(*(long *)(player + 0x2c) & 0xffff) * 12 + 3]) & 1))
        {
            vector3f velocity;
            if (function_109e00(target_index, &velocity, false))
            {
                byte *current = attachment_object(*(long *)(player + 0x2c));
                target = attachment_object(target_index);
                current[0xc0] = (current[0xc0] & ~6) | (target[0xc0] & 6);
                *(long *)(current + 0xb8) = *(long *)(target + 0xb8);
                *(long *)(current + 0x3e4) = *(long *)(target + 0x3e4);
                *(long *)(current + 0x3e8) = *(long *)(target + 0x3e8);
                long current_index = *(long *)(player + 0x2c);
                function_b7740(current_index, &velocity, NULL, false);
                function_1c4b00(current_index, &velocity, NULL, 1);
                if (velocity.i * velocity.i + velocity.k * velocity.k + velocity.j * velocity.j > 0.0001f)
                {
                    function_b9b90(current_index, false);
                    function_b7360(current_index);
                    function_bba20(current_index);
                }
            }
        }
    }
    if (detach)
        player[3] |= 0x20;
    return result;
}
