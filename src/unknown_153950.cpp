// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_153950.CPP: the game speed (time dilation and camera shake) slots */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>
#include <string.h>
#include "sound_sources.h"

/* the result of the speed effects: the type, the amount and the shake */
struct s_speed_result
{
	long type;
	real amount;
	s_speed_shake shake;
};

byte g_510c60;
short g_468cec[7] = { 0, 1, 2, 3, 4, 5, 6 };

real function_17ca10(real x, short curve);
real function_30bf0(vector3f *v);
vector3f *random_unit_vector(vector3f *result, dword *seed);

// @retail 0x153950
void function_153950(void)
{
	s_game_speed *speed = g_510c5c;
	long index;

	if (speed->duration != NONE && g_510c54->game_time - speed->start_time > speed->duration && !speed->reverse)
	{
		speed->duration = NONE;
	}
	if (speed->flags & 1)
	{
		if (speed->timer24 > 0)
		{
			speed->timer24--;
		}
		else if (speed->flags & 2)
		{
			speed->flags &= ~1;
			g_502120->value228 = 0.0f;
			g_502120->value220 = 0.0f;
			g_502120->value224 = 0.0f;
		}
	}

	for (index = 0; index < 4; index++)
	{
		s_speed_slot *slot = speed->slots + index;

		if (index != NONE && g_4e8c20->entries[index] != NONE)
		{
			real quarter_second = (real)g_510c54->field_2_3 * 0.25f;
			long rounded;
			long decay;
			long i;

			__asm
			{
				fld quarter_second
				fistp rounded
			}
			decay = 255 / rounded;

			i = 0;
			do
			{
				if (slot->decay[i] > 0)
				{
					long value = slot->decay[i] - decay;
					byte mask = (byte)((value < 0) - 1);
					slot->decay[i] = (byte)value & mask;
				}
				i++;
			}
			while (i < 4);
			if (slot->decay89 > 0)
			{
				slot->decay89 -= decay;
			}
			slot->flag0 = 0;
			if (slot->timer7e > 0)
			{
				slot->timer7e--;
			}
			slot->flag1 = 0;
			if (slot->timer80 > 0)
			{
				slot->timer80--;
			}
			slot->flag2 = 0;
			if (slot->timer82 > 0)
			{
				slot->timer82--;
			}
			if (slot->timer7c > 0)
			{
				slot->timer7c--;
				if (slot->timer7c == 0)
				{
					g_502120->entries[index].value80 = 0.0f;
					g_502120->entries[index].value84 = 0.0f;
					memset(slot->values6c, 0, sizeof(slot->values6c));
				}
			}
		}
		else
		{
			long i;

			memset(slot, 0, sizeof(s_speed_slot));
			memset(&g_502120->entries[index], 0, sizeof(s_speed_table_entry));
			for (i = 0; i < 8; i++)
			{
				g_502120->entries[index].items[i].a = NONE;
				g_502120->entries[index].items[i].b = NONE;
				g_502120->entries[index].items[i].scale = 1.0f;
			}
		}
	}
}

void function_1542a0(short seconds, real x, real y, real z);

// @retail 0x1538b0
void function_1538b0(void)
{
	s_game_speed *speed = g_510c5c;

	memset(speed, 0, sizeof(*speed));
	speed->duration = NONE;
	speed->time2c = g_510c54->game_time;
	g_510c60 = false;
	g_4e8c28 = *g_468718;
	if (g_4e6948->state == 1)
	{
		if (g_4e0350->flags & 0x80)
		{
			function_1542a0(0, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			function_1542a0(0, 0.0f, 0.0f, 0.0f);
		}
		g_510c60 = true;
	}
}

// @retail 0x153b80
void function_153b80(long index)
{
	if (index != NONE)
	{
		s_speed_slot *slot = (g_510c5c)->slots + index;

		memset(&slot->request, 0, sizeof(s_speed_request));
		slot->timer7e = 0;
		slot->flag0 = 0;
	}
}

void function_154d70(s_speed_request *request, s_speed_slot *slot, real scale);
void function_155240(s_speed_values *values, s_speed_slot *slot, real priority);

// @retail 0x153bd0
void function_153bd0(long index, real scale)
{
	if (index != NONE)
	{
		s_speed_slot *slot = &g_510c5c->slots[index];
		s_speed_request request;
		s_speed_values values;

		memset(&request, 0, sizeof(request));
		memset(&values, 0, sizeof(values));

		values.value[2] = scale * 0.01;
		values.value[0] = 1.0f;
		values.value[5] = 1.0f;
		request.shake = *(s_speed_shake const *)g_4686cc;
		request.duration = 1.0f;
		g_502120->entries[index].value80 = scale;
		g_502120->entries[index].value84 = scale;
		request.type = 1;
		request.priority = 2;
		request.amount = scale;
		function_154d70(&request, slot, scale);
		function_155240(&values, slot, scale);
	}
}

// @retail 0x153cd0
void function_153cd0(long player_index)
{
	long index = *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

	if (index != NONE)
	{
		s_speed_table_entry *entry = g_502120->entries + index;

		entry->value80 = 0.0f;
		entry->value84 = 0.0f;
	}
}

// @retail 0x154220
void function_154220(real x, short seconds, real y, real z)
{
	s_game_time_globals *time = g_510c54;
	s_game_speed *speed;
	real scaled;
	long ticks;

	g_4e8c28.x = x;
	g_4e8c28.y = y;
	g_4e8c28.z = z;
	scaled = (real)seconds * (1.0f / 30.0f);
	scaled = scaled * (real)time->field_2_3;
	__asm
	{
		fld scaled
		fistp ticks
	}
	speed = g_510c5c;
	speed->duration = (short)ticks;
	speed->reverse = false;
	speed->start_time = time->game_time;
	g_510c60 = false;
}

// @retail 0x1542a0
void function_1542a0(short seconds, real x, real y, real z)
{
	s_game_time_globals *time = g_510c54;
	s_game_speed *speed;
	real scaled;
	long ticks;

	g_4e8c28.x = x;
	g_4e8c28.y = y;
	g_4e8c28.z = z;
	scaled = (real)seconds * (1.0f / 30.0f);
	scaled = scaled * (real)time->field_2_3;
	__asm
	{
		fld scaled
		fistp ticks
	}
	speed = g_510c5c;
	speed->duration = (short)ticks;
	speed->reverse = true;
	speed->start_time = time->game_time;
}

// @retail 0x154310
void function_154310(long index, s_speed_result *result)
{
	s_game_speed *speed = g_510c5c;

	result->amount = 0.0f;
	if (speed->duration != NONE)
	{
		real t;

		result->type = 1;
		if (g_510c60)
		{
			if (g_4e0350->flags & 0x80)
			{
				*(point3f *)&result->shake.vector = *g_468710;
			}
			else
			{
				*(point3f *)&result->shake.vector = *g_468718;
			}
		}
		else
		{
			*(point3f *)&result->shake.vector = g_4e8c28;
		}
		result->shake.scale = 1.0f;
		if (speed->duration > 0)
		{
			t = (real)(g_510c54->game_time - speed->start_time) / (real)speed->duration;
			if (0.0f > t)
				t = 0.0f;
			else if (t > 1.0f)
				t = 1.0f;
			t = function_17ca10(t, 5);
		}
		else
		{
			t = 1.0f;
		}
		result->amount = t;
		if (!speed->reverse)
		{
			result->amount = 1.0f - t;
		}
		result->amount = (0.0f > result->amount) ? 0.0f : ((result->amount > 1.0f) ? 1.0f : result->amount);
	}
	else if (index != NONE)
	{
		s_speed_slot *slot = speed->slots + index;

		if (slot->timer7e > 0 || (slot->flags & 1))
		{
			result->type = g_468cec[slot->request.type];
			result->shake = slot->request.shake;
			if (slot->request.duration > 0.0f)
			{
				result->amount = function_17ca10((real)slot->timer7e / slot->request.duration, slot->request.curve) * slot->request.amount;
			}
			else
			{
				result->amount = slot->request.amount;
			}
			result->amount = (0.0f > result->amount) ? 0.0f : ((result->amount > 1.0f) ? 1.0f : result->amount);
		}
	}
}

// @retail 0x1544e0
void function_1544e0(transform4x3f *matrix, real distance, real angle)
{
	s_random_globals *random = g_4e7408;

	if (angle != 0.0f)
	{
		vector3f axis;

		random_unit_vector(&axis, &random->seed);

		real s = (real)sin(angle);
		real c = (real)cos(angle);
		real t = 1.0f - c;

		matrix->scale = 1.0f;
		matrix->forward.i = (1.0f - axis.i * axis.i) * c + axis.i * axis.i;
		matrix->forward.j = t * axis.i * axis.j + axis.k * s;
		matrix->forward.k = t * axis.k * axis.i - axis.j * s;
		matrix->left.i = t * axis.i * axis.j - axis.k * s;
		matrix->left.j = (1.0f - axis.j * axis.j) * c + axis.j * axis.j;
		matrix->left.k = t * axis.k * axis.j + axis.i * s;
		matrix->up.i = t * axis.k * axis.i + axis.j * s;
		matrix->up.j = t * axis.k * axis.j - axis.i * s;
		matrix->up.k = (1.0f - axis.k * axis.k) * c + axis.k * axis.k;
		matrix->position.z = 0.0f;
		matrix->position.y = 0.0f;
		matrix->position.x = 0.0f;
	}
	if (distance != 0.0f)
	{
		vector3f v;

		random_unit_vector(&v, &random->seed);
		matrix->position.x = v.i * distance;
		matrix->position.y = v.j * distance;
		matrix->position.z = v.k * distance;
	}
}

// @retail 0x154d70
void function_154d70(s_speed_request *request, s_speed_slot *slot, real scale)
{
	s_game_time_globals *time = g_510c54;

	if (slot->request.priority > request->priority && (real)slot->timer7e * time->rate > request->duration)
	{
		return;
	}
	if (g_468cec[request->type] == 0)
	{
		return;
	}
	slot->request = *request;
	slot->request.duration = (real)time->field_2_3 * slot->request.duration;
	slot->timer7e = (short)slot->request.duration;
	slot->request.amount = slot->request.amount * scale;
	slot->flags |= 1;
}

// @retail 0x154df0
void function_154df0(vector3f *direction, s_speed_bounds *bounds, long index, s_speed_slot *slot, real priority)
{
	s_game_time_globals *time = g_510c54;
	real timer = (real)slot->timer80;

	if (bounds->duration > timer || priority > slot->priority9c || (priority >= slot->priority9c && bounds->duration > time->rate * timer))
	{
		vector3f w;
		vector3f v;

		v = *direction;
		v.k = 0.0f;
		function_30bf0(&v);

		s_unknown_185ab0_entry *view = g_4ed284->entries + index;
		real yaw = view->yaw;
		real pitch = view->pitch;

		w.i = (real)(cos(yaw) * cos(pitch));
		w.j = (real)(sin(yaw) * cos(pitch));
		w.k = 0.0f;
		function_30bf0(&w);

		if (fabs(v.j * v.j + v.i * v.i + v.k * v.k - 1.0f) < 0.0001f && fabs(w.j * w.j + w.i * w.i + w.k * w.k - 1.0f) < 0.0001f)
		{
			real dot = v.j * w.j + w.i * v.i;
			real angle;
			real cross;

			if (-1.0f > dot)
				dot = -1.0f;
			else if (dot > 1.0f)
				dot = 1.0f;
			angle = (real)acos((-1.0f > dot) ? -1.0f : ((dot > 1.0f) ? 1.0f : dot));
			cross = v.j * w.i - w.j * v.i;
			if (0.0f > cross)
			{
				angle = 0.0f - angle;
			}

			slot->bounds = *bounds;
			slot->priority9c = priority;
			slot->bounds.duration = (real)time->field_2_3 * slot->bounds.duration;
			slot->timer80 = (short)slot->bounds.duration;
			slot->forward.k = 0.0f;
			slot->forward.i = (real)cos(angle);
			slot->forward.j = (real)sin(angle);

			s_random_globals *random = g_4e7408;
			real scale = function_259d0(&random->seed, __FILE__, __LINE__, slot->bounds.lower, slot->bounds.upper);
			real rotation = function_x82e52f(&random->seed, __FILE__, __LINE__) * 6.2831855f;
			vector3f *up = g_4687b0;
			vector3f *e = &slot->forward;
			vector3f *u = &slot->vector;

			real ci = up->k * e->j - up->j * e->k;
			real cj = up->i * e->k - up->k * e->i;
			real ck = e->i * up->j - e->j * up->i;
			u->i = ci;
			u->j = cj;
			u->k = ck;
			function_30bf0(u);

			real s = (real)sin(rotation);
			real c = (real)cos(rotation);
			real t = (u->k * e->k + u->j * e->j + e->i * u->i) * (1.0f - c);
			real x = u->i;
			real y = u->j;
			real z = u->k;

			u->i = e->i * t + x * c - (y * e->k - z * e->j) * s;
			u->j = e->j * t + y * c - (z * e->i - x * e->k) * s;
			u->k = e->k * t + z * c - (x * e->j - y * e->i) * s;
			u->i = u->i * scale;
			u->j = u->j * scale;
			u->k = u->k * scale;
			slot->flags |= 2;
		}
	}
}

// @retail 0x155240
void function_155240(s_speed_values *values, s_speed_slot *slot, real priority)
{
	s_game_time_globals *time = g_510c54;
	real rate = time->rate;
	real timer = (real)slot->timer82;
	real t = rate * timer;
	real scaled;
	long ticks;

	if (values->value[0] > t || priority > slot->priority98 || (priority >= slot->priority98 && values->value[0] > t))
	{
		slot->values50 = *values;
		slot->priority98 = priority;
		scaled = (real)time->field_2_3 * slot->values50.value[0];
		__asm
		{
			fld scaled
			fistp ticks
		}
		slot->timer82 = (short)ticks;
		slot->flags |= 4;
	}
}

struct s_object;
s_object *function_badc0(long index, dword mask);
point3f *function_b9dd0(long index, point3f *point);
void function_caf60(long index, point3f *point);
__declspec(noinline) s_player_state *function_16f3a0(long index);
long function_189fe0(s_sound_request const *request, long tag_index);
void rumble_player_play_effect(long player_index, long definition_index, long effect_index, real scale);
void function_225df0(long tag_index, long entry_index);

struct s_camera_effect_153d10
{
    short type;
    short unknown02;
    s_speed_request request;
    byte unknown24[0x4c - 0x24];
};
struct s_camera_effect_tag_153d10
{
    byte unknown00[0x14];
    dword flags;
    byte unknown18[0x6c - 0x18];
    long count;
    s_camera_effect_153d10 *effects;
    s_speed_bounds bounds;
    s_speed_values values;
    byte unknowna8[4];
    long sound_index;
};

// @retail 0x153d10
void __stdcall function_153d10(short user_index, long tag_index, void *owner, void *direction, long unused, real scale, real priority, long directional)
{
    long local_index = user_index;
    if (local_index == NONE)
        return;
    s_camera_effect_tag_153d10 *definition = (s_camera_effect_tag_153d10 *)g_4e3b44[tag_index & 0xffff].bytes;
    s_speed_slot *slot = &g_510c5c->slots[local_index];
    long player_index = g_4e8c20->entries[local_index];
    if (player_index != NONE)
    {
        long unit_index = *(long *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);
        if (unit_index != NONE)
        {
            byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
            bool active = *(real *)(unit + 0xf0) > 0.0f;
            for (long i = 0; i < definition->count; i++)
            {
                s_camera_effect_153d10 *effect = &definition->effects[i];
                if (effect->type != (active ? 1 : 0))
                {
                    if (effect->request.duration > 0.05f && effect->request.amount > 0.05f)
                    {
                        s_speed_request request = effect->request;
                        request.shake.scale = 1.0f;
                        if (active)
                        {
                            request.shake.vector.i = 0.0f;
                            request.shake.vector.j = 0.6f;
                            request.shake.vector.k = 0.8f;
                        }
                        else
                        {
                            request.shake.vector.i = 1.0f;
                            request.shake.vector.j = 0.0f;
                            request.shake.vector.k = 0.0f;
                        }
                        if (request.duration > 0.5f && *(short *)(unit + 0x1fc) == NONE)
                        {
                            request.duration = 0.8f;
                            request.amount = 0.6f;
                            request.curve = 1;
                        }
                        else
                        {
                            request.duration = active ? 0.2f : 0.3f;
                            request.amount = 0.3f;
                            request.curve = 0;
                        }
                        request.priority = 1;
                        request.type = 1;
                        function_154d70(&request, slot, function_17ca10(scale, 2));
                    }
                    rumble_player_play_effect(local_index, tag_index, i, priority);
                    function_225df0(tag_index, i);
                    break;
                }
            }
        }
    }
    function_154df0((vector3f *)direction, &definition->bounds, local_index, slot, priority);
    function_155240(&definition->values, slot, priority);
    if (definition->sound_index != NONE)
    {
        s_sound_request request;
        request.location.unknown02 = 0;
        request.location.flags = 0;
        ((byte *)&request.location)[3] = 0;
        request.location.scale = 1.0f;
        request.location.unknown08 = 0;
        request.object_index = NONE;
        request.platform_playback = NONE;
        request.marker = NULL;
        request.source = NULL;
        request.variant = NULL;
        function_189fe0(&request, definition->sound_index);
    }
    if (directional && ((long *)owner)[1] != NONE)
    {
        if (definition->flags & 0x100)
        {
            slot->decay[2] = 0xff;
            return;
        }
        long unit_index = NONE;
        long player = g_4e8c20->entries[local_index];
        if (player != NONE)
            unit_index = *(long *)(g_4e8c24->data + (player & 0xffff) * 0x21c + 0x2c);
        if (function_badc0(unit_index, 3) && function_badc0(((long *)owner)[1], (dword)NONE))
        {
            s_player_state *state = function_16f3a0(local_index);
            if (state)
            {
                point3f origin, target;
                function_caf60(unit_index, &origin);
                function_b9dd0(((long *)owner)[1], &target);
                vector3f *forward = (vector3f *)((byte *)state + 0x20);
                vector3f *up = (vector3f *)((byte *)state + 0x2c);
                vector3f delta = { target.x - origin.x, target.y - origin.y, target.z - origin.z };
                vector3f relative;
                relative.i = (forward->k * up->i - up->k * forward->i) * delta.j
                    + (forward->i * up->j - forward->j * up->i) * delta.k
                    + (forward->j * up->k - forward->k * up->j) * delta.i;
                relative.j = forward->k * delta.k + forward->j * delta.j + forward->i * delta.i;
                relative.k = up->k * delta.k + up->j * delta.j + up->i * delta.i;
                *(real *)((byte *)slot + 0x8c) = relative.i;
                *(real *)((byte *)slot + 0x90) = relative.k;
                *(real *)((byte *)slot + 0x94) = relative.j;
                slot->decay89 = 0xff;
                if (function_30bf0(&relative) != 0.0f)
                {
                    if (fabs(relative.k) > 0.5f)
                    {
                        if (relative.k > 0.0f) slot->decay[0] = 0xff;
                        else slot->decay[2] = 0xff;
                    }
                    real angle = (real)atan2(relative.j, relative.i);
                    real absolute = (real)fabs(angle);
                    if (angle < 0.785398185f || angle > 2.356194496f)
                    {
                        if (absolute > 1.570796371f) slot->decay[1] = 0xff;
                        else slot->decay[3] = 0xff;
                    }
                }
            }
        }
    }
}

extern transform4x3f *g_4687d0;
void function_141ce0(real a, real b, real c, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
real function_17c900(short curve, real input);

// @retail 0x1546f0
void __stdcall function_1546f0(long local_index, transform4x3f *matrix)
{
    if (local_index == NONE)
        return;
    s_game_speed *speed = g_510c5c;
    if (speed->flags & 1)
    {
        real amount = *(real *)&speed->value20;
        transform4x3f transform = *g_4687d0;
        if (speed->timer24 > 0)
        {
            if (speed->flags & 2)
                amount *= (real)speed->timer24 / (real)speed->timer26;
            else
                amount *= 1.0f - (real)speed->timer24 / (real)speed->timer26;
        }
        amount = 0.0f > amount ? 0.0f : (amount > 1.0f ? 1.0f : amount);
        s_random_globals *random = g_4e7408;
        g_502120->value228 = amount;
        function_141ce0(
            (function_x82e52f(&random->seed, 0, 0) * 2.0f - 1.0f) * speed->angles[0] * amount,
            (function_x82e52f(&random->seed, 0, 0) * 2.0f - 1.0f) * speed->angles[1] * amount,
            (function_x82e52f(&random->seed, 0, 0) * 2.0f - 1.0f) * speed->angles[2] * amount, &transform);
        transform.position.x = (function_x82e52f(&random->seed, 0, 0) * 2.0f - 1.0f) * speed->point.x * amount;
        transform.position.y = (function_x82e52f(&random->seed, 0, 0) * 2.0f - 1.0f) * speed->point.y * amount;
        transform.position.z = (function_x82e52f(&random->seed, 0, 0) * 2.0f - 1.0f) * speed->point.z * amount;
        function_142a60(matrix, &transform, matrix);
    }
    else
    {
        s_speed_slot *slot = &speed->slots[local_index];
        if (slot->timer80 > 0 || (slot->flags & 2))
        {
            real amount;
            if (slot->flags & 2)
                amount = 1.0f;
            else
                amount = function_17ca10(1.0f - (slot->bounds.duration - (real)slot->timer80) / slot->bounds.duration,
                    *(short *)&slot->bounds.unknown04[0]) * slot->priority9c;
            real angle = amount * slot->bounds.unknown04[1];
            vector3f axis;
            axis.i = g_4687b0->j * slot->forward.k - g_4687b0->k * slot->forward.j;
            axis.j = slot->forward.i * g_4687b0->k - slot->forward.k * g_4687b0->i;
            axis.k = g_4687b0->i * slot->forward.j - slot->forward.i * g_4687b0->j;
            real sine = (real)sin(angle), cosine = (real)cos(angle);
            real x = axis.i, y = axis.j, z = axis.k;
            transform4x3f transform;
            transform.scale = 1.0f;
            transform.forward.i = x * x + (1.0f - x * x) * cosine;
            transform.left.j = y * y + (1.0f - y * y) * cosine;
            transform.up.k = z * z + (1.0f - z * z) * cosine;
            real xy = (1.0f - cosine) * y * x;
            real xz = (1.0f - cosine) * z * x;
            real yz = (1.0f - cosine) * z * y;
            transform.left.i = xy - z * sine;
            transform.forward.j = xy + z * sine;
            transform.up.i = xz + y * sine;
            transform.forward.k = xz - y * sine;
            transform.up.j = yz - x * sine;
            transform.left.k = yz + x * sine;
            real distance = slot->bounds.unknown04[2] * amount;
            transform.position.x = slot->vector.i * amount + slot->forward.i * distance;
            transform.position.y = slot->vector.j * amount + slot->forward.j * distance;
            transform.position.z = slot->vector.k * amount + slot->forward.k * distance;
            function_142a60(matrix, &transform, matrix);
        }
        if (slot->timer82 > 0 || (slot->flags & 4))
        {
            transform4x3f transform = *g_4687d0;
            s_game_time_globals *time = g_510c54;
            real amount;
            if (slot->flags & 4)
                amount = 1.0f;
            else
                amount = function_17ca10(1.0f - (slot->values50.value[0] - (real)slot->timer82 * time->rate) / slot->values50.value[0],
                    *(short *)&slot->values50.value[1]) * slot->priority98;
            real elapsed = (slot->values50.value[0] - (real)slot->timer82 * time->rate) / slot->values50.value[5];
            real curve = function_17c900(*(short *)&slot->values50.value[4], elapsed);
            real scale = (slot->values50.value[6] * curve + (1.0f - slot->values50.value[6])) * amount;
            real distance = slot->values50.value[2] * scale;
            distance = distance > 0.0f ? distance : 0.0f;
            real angle = slot->values50.value[3] * scale;
            angle = angle > 0.0f ? angle : 0.0f;
            if (slot->timer7c > 0)
            {
                real fade = (real)slot->timer7c * time->rate * 2.0f;
                distance += slot->values6c[2] * fade;
                angle += slot->values6c[3] * fade;
                g_502120->entries[local_index].value80 = slot->values6c[0];
                g_502120->entries[local_index].value84 = slot->values6c[1];
            }
            function_1544e0(&transform, distance, angle);
            function_142a60(matrix, &transform, matrix);
        }
    }
}
