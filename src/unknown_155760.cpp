#include "unknown_4e8c38.h"
// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_155760.CPP: entry classification */

#include "unknown_11c920.h"
#include "globals.h"
#include "local_players.h"
#include <string.h>



byte g_51ec10;

struct s_observer_command;
void __stdcall function_23c110(void *state, void *input, s_observer_command *command);
void __stdcall function_23cbb0(void *state, void *input, s_observer_command *command);
void __stdcall function_16c840(long user_index, long unused, s_observer_command *command);
void __stdcall function_23d090(void *state, void *input, s_observer_command *command);
void __stdcall function_23de50(void *state, void *input, s_observer_command *command);

// @retail 0x155710
void function_155710(long index)
{
	if (g_4e8c3c == 0)
	{
		(volatile word &)g_4e8c44[index].word58 = 0;
		(volatile real &)g_4e8c44[index].value10 = 0.0f;
		(volatile real &)g_4e8c44[index].value14 = 0.0f;
		g_4e8c44[index].proc = function_23c110;
		g_4e8c44[index].value10c = 1.0f;
		g_4e8c44[index].flag108 = 0;
	}
}

// @retail 0x155d60
bool function_155d60(long index)
{
	return g_4e8c44[index].proc == function_23de50;
}

// @retail 0x155760
long function_155760(long index)
{
	s_entry_155760 *entry = g_4e8c44 + index;
	long result = 3;
	if (entry->proc == function_23c110)
	{
		if (entry->value == g_45dbd8)
		{
			result = 0;
		}
	}
	else if (entry->proc == function_23cbb0)
	{
		result = 1;
	}
	else if (entry->proc == function_16c840 || (entry->proc == function_23d090 && g_51ec10))
	{
		result = 2;
	}
	return result;
}

/* the camera defaults of the globals tag (g_4e034c + 0xec) */
struct s_camera_defaults
{
	byte unknown00[8];
	real value08;
	real value0c;
	real value10;
};

struct s_camera_globals_view
{
	byte unknown00[0xec];
	s_camera_defaults *defaults;
};

// @retail 0x155dd0
void function_155dd0(long index, void *proc, bool use_defaults)
{
	s_entry_155760 *entry = &g_4e8c44[index];

	entry->proc = proc;
	entry->value10c = 1.0f;
	entry->flag108 = 0;
	if (use_defaults && g_4e0350)
	{
		s_camera_defaults *defaults = ((s_camera_globals_view *)g_4e034c)->defaults;
		real value;

		if (proc == function_23cbb0)
		{
			value = defaults->value10;
		}
		else if (proc == function_23c110)
		{
			value = defaults->value0c;
		}
		else
		{
			value = defaults->value08;
		}
		if (value != g_45dbd8)
		{
			entry->value = value;
		}
	}
}

/* the objects (g_4e0300) the camera code reads */
struct s_camera_object
{
	long definition_index;
	byte unknown04[0x14 - 4];
	long parent_index;
	byte unknown18[0xaa - 0x18];
	byte type;
	byte unknownab[0x1fc - 0xab];
	short seat_index;
};

struct s_camera_object_header
{
	byte unknown00[8];
	s_camera_object *object;
};

struct s_camera_seat
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword : 25;
	byte unknown04[0xb0 - 4];
};

struct s_camera_unit_definition
{
	byte unknown00[0x1cc];
	s_camera_seat *seats;
};

struct s_camera_player
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_0c7070(long object_index);
long function_10f5f0(long object_index);

// @retail 0x155810
short function_155810(long unit_index, short *mode)
{
	short result = 0;
	s_camera_object *unit = (s_camera_object *)function_badc0(unit_index, 3);

	*mode = 0;
	if (unit && unit->parent_index != NONE)
	{
		s_camera_object *parent = ((s_camera_object_header *)g_4e0300->data)[unit->parent_index & 0xffff].object;
		long label = function_10f5f0(unit_index);

		if ((1 << parent->type) & 3)
		{
			s_camera_unit_definition *definition = (s_camera_unit_definition *)g_4e3b44[parent->definition_index & 0xffff].bytes;
			s_camera_seat *seat = &definition->seats[unit->seat_index];
			bool flag6 = seat->flag6;

			if (TEST_FIELD_BIT(seat->flag4))
			{
				result = 1;
			}
			if (flag6)
			{
				bool mode_selected = false;

				if (function_0c7070(unit_index))
				{
					*mode = 1;
					mode_selected = true;
				}
				else if (label == 0x400004a)
				{
					*mode = 3;
					mode_selected = true;
				}
				if (mode_selected)
				{
					result = 1;
					goto done;
				}
			}
		}
		*mode = 2;
	}
done:
	return result;
}

/* the state of the camera procs at +0x10 */
struct s_camera_state_23cbb0
{
	byte value10;
	byte value11;
	byte value12;
	byte value13;
	byte value14;
	byte unknown15;
	short value16;
	long value18;
	short value1c;
	byte unknown1e[2];
	real value20;
	real value24;
	real value28;
};

void function_155dd0(long index, void *proc, bool use_defaults);

// @retail 0x155920
void function_155920(long local_index, bool force)
{
	s_entry_155760 *entry = &g_4e8c44[local_index];
	long unit_index;
	short mode;
	short result;

	if (local_player_exists(local_index))
	{
		unit_index = ((s_camera_player *)g_4e8c24->data)[g_4e8c20->entries[local_index] & 0xffff].unit_index;
	}
	else
	{
		unit_index = NONE;
	}
	result = function_155810(unit_index, &mode);
	if (force || entry->word58 != (word)mode)
	{
		if (result == 1)
		{
			if (force || entry->proc == function_23c110)
			{
				s_camera_state_23cbb0 *state = (s_camera_state_23cbb0 *)&entry->value10;

				state->value10 = 0;
				state->value11 = 0;
				state->value12 = 0;
				state->value13 = 0;
				state->value16 = 0;
				state->value18 = NONE;
				state->value1c = NONE;
				state->value24 = 0.0f;
				state->value20 = 0.0f;
				state->value28 = 1.0f;
				state->value14 = 0;
				function_155dd0(local_index, function_23cbb0, !force);
			}
		}
		else if (force || entry->proc == function_23cbb0)
		{
			entry->value10 = 0.0f;
			entry->value14 = 0.0f;
			function_155dd0(local_index, function_23c110, !force);
		}
		entry->word58 = mode;
	}
}

struct s_unknown_78;
extern s_unknown_78 *g_510c6c;

// @retail 0x155a30
void __stdcall function_155a30(byte value)
{
	byte const *value_reference = &value;
	*g_4e8c34 = *value_reference;
	for (long index = 0; index < 4; index++)
	{
		s_entry_155760 *entry = &g_4e8c44[index];
		if (*value_reference)
		{
			entry->proc = function_16c840;
			entry->value10c = 1.0f;
			entry->flag108 = 0;
		}
		else
		{
			function_155920(index, true);
		}
		((byte *)g_510c6c)[0] = *value_reference;
		((byte *)g_510c6c)[1] = 1;
	}
}

struct s_view_state;
struct s_observer_state;
void function_23cea0(s_view_state *state, long local_index);
void function_23da00(s_observer_state *observer, long local_index, long target);

// @retail 0x155d80
void function_155d80(long local_index, bool force)
{
    s_entry_155760 *entry = &g_4e8c44[local_index];
    if (force || entry->proc != function_23d090)
    {
        function_23cea0((s_view_state *)&entry->value10, local_index);
        function_155dd0(local_index, function_23d090, false);
    }
}

// @retail 0x155c60
void function_155c60(long local_index, bool reset)
{
    s_entry_155760 *entry = &g_4e8c44[local_index];
    if (reset)
    {
        entry->value10 = 0.0f;
        entry->value14 = 0.0f;
        function_155dd0(local_index, function_23c110, false);
    }
    else
    {
        long player_index = NONE;
        if (local_index != NONE)
            player_index = g_4e8c20->entries[local_index];
        s_camera_player *player = &((s_camera_player *)g_4e8c24->data)[player_index & 0xffff];
        bool observer = player->unit_index == NONE;
        if (g_4e6948->state != 2)
            observer = observer && !(((byte *)player)[2] & 8);
        if (!*g_4e8c34 && g_510c54->scale > 0.0f)
        {
            function_155920(local_index, false);
            if (observer)
            {
                if (entry->proc != function_23de50)
                {
                    function_23da00((s_observer_state *)&entry->value10, local_index, NONE);
                    function_155dd0(local_index, function_23de50, true);
                }
            }
            else if (entry->proc == function_23de50)
                function_155920(local_index, true);
        }
    }
}

// @retail 0x155b60
void function_155b60(long unit_index)
{
    byte *unit = (byte *)function_badc0(unit_index, 3);
    long owner = unit ? *(long *)(unit + 0x13c) : NONE;
    if (owner != NONE)
    {
        unit = (byte *)function_badc0(unit_index, 3);
        long player_index = NONE;
        if (unit)
            player_index = *(long *)(unit + 0x13c);
        s_camera_player *player = &((s_camera_player *)g_4e8c24->data)[player_index & 0xffff];
        long local_index = *(short *)((byte *)player + 0x28);
        if (local_index != NONE)
        {
            s_entry_155760 *entry = &g_4e8c44[local_index];
            s_observer_state *state = (s_observer_state *)&entry->value10;
            function_23da00(state, local_index, NONE);
            *(real *)((byte *)state + 0x38) = 2.0f;
            g_4e8c44[local_index].proc = function_23de50;
            g_4e8c44[local_index].value10c = 1.0f;
            g_4e8c44[local_index].flag108 = 0;
            if (g_4e0350)
            {
                s_camera_defaults *defaults = ((s_camera_globals_view *)g_4e034c)->defaults;
                real value;
                if ((void *)function_23de50 == (void *)function_23cbb0)
                    value = defaults->value10;
                else if ((void *)function_23de50 == (void *)function_23c110)
                    value = defaults->value0c;
                else
                    value = defaults->value08;
                if (value != g_45dbd8)
                    g_4e8c44[local_index].value = value;
            }
        }
    }
}

dword g_47fca0[4][8] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0x3f800000, 0, 0, 0 }
};

struct s_camera_slot_155380
{
    dword value;
    real current;
    real target;
};

// @retail 0x155380
void function_155380(void)
{
    memset(g_4e8c38, 0, sizeof(g_4e8c38));
    g_4e8c3c = 0;
    g_4e8c40 = 0;
    for (long index = 0; index < 4; index++)
    {
        s_entry_155760 *entry = &g_4e8c44[index];
        if (!entry->unknown00[0])
        {
            long *field = (long *)((byte *)entry + 0x50);
            ((byte *)field)[4] = 0;
            *field = 0;
            entry->value = 0.0f;
            switch (g_4e8c3c)
            {
            case 0:
            case 1:
            {
                real *value14 = &entry->value14;
                *value14 = 0.0f;
                entry->value10 = 0.0f;
                function_155dd0(index, function_23c110, false);
                break;
            }
            case 2:
                function_23cea0((s_view_state *)&entry->value10, index);
                function_155dd0(index, function_23d090, false);
                break;
            case 4:
            {
                real *value14 = &entry->value14;
                *value14 = 0.0f;
                entry->value10 = 0.0f;
                function_155dd0(index, function_23c110, false);
                break;
            }
            case 3:
                break;
            }
            dword const *value = g_47fca0[0];
            real *target = (real *)((byte *)entry + 0x118);
            long remaining = 4;
            do
            {
                *(dword *)(target - 2) = *value;
                *target = 0.0f;
                target[-1] = 0.0f;
                value += 8;
                target += 3;
            } while (--remaining);
            entry->unknown00[0] = 1;
        }
    }
    short *mode = (short *)((byte *)g_510c6c + 2);
    long *object = (long *)((byte *)g_510c6c + 0x3c);
    *mode = 0;
    *object = NONE;
}

struct s_observer_command
{
    dword flags;
    byte unknown004[0x88 - 0x4];
    real unknown88;
    byte unknown8c[0x94 - 0x8c];
    vector3f unknown94;
    vector3f unknowna0;
};

struct s_camera_update_input
{
    long local_index;
    long unknown04;
    real elapsed;
    byte unknown0c[0x2c - 0xc];
};

__declspec(noinline) void function_16f190(long user_index, s_observer_command *command);
// retail calls 0x155d80 from function_1554b0; the build would inline it there
__declspec(noinline) void function_155d80(long local_index, bool force);

// @retail 0x1554b0
void function_1554b0(real elapsed)
{
    *(real *)g_4e8c38 = elapsed;
    for (long index = 0; index < 4; index++)
    {
        s_entry_155760 *entry = &g_4e8c44[index];
        bool valid = false;
        if (index != NONE)
            valid = g_4e8c20->entries[index] != NONE;
        if (valid)
        {
            s_camera_update_input input;
            ((byte *)entry)[0x55] = 0;
            ((byte *)entry)[0x56] = 0;
            memset(&input, 0, sizeof(input));
            input.local_index = index;
            input.elapsed = *(real *)g_4e8c38;
            switch (g_4e8c3c)
            {
            case 0:
            case 1:
                function_155c60(index, g_4e8c40);
                break;
            case 2:
                function_155d80(index, g_4e8c40);
                break;
            case 3:
                break;
            case 4:
                if (g_4e8c40)
                {
                    entry->value10 = 0.0f;
                    entry->value14 = 0.0f;
                    entry->proc = function_23c110;
                    entry->value10c = 1.0f;
                    entry->flag108 = 0;
                }
                break;
            }
            s_observer_command command;
            g_4e8c40 = 0;
            memset(&command, 0, sizeof(command));
            if (entry->proc)
            {
                if (entry->proc == function_16c840)
                {
                    long first = NONE;
                    for (long user = 0; user < 4; user++)
                    {
                        if (g_4e8c20->entries[user] != NONE)
                        {
                            first = user;
                            break;
                        }
                    }
                    if (index != first) goto command_ready;
                }
                ((void (__stdcall *)(void *, void *, s_observer_command *))entry->proc)(&entry->value10, &input, &command);
            }
        command_ready:
            if (command.flags & 1)
            {
                if (entry->value != 0.0f)
                {
                    if (entry->value < 0.2f && entry->proc == function_23c110)
                    {
                        entry->value = 0.0f;
                        command.unknown94.i = 0.0f;
                        command.unknown8c[0] = 3;
                        command.unknowna0.i = 0.0f;
                        command.unknown8c[3] = 3;
                    }
                    else if (!(command.unknown88 > entry->value))
                        command.unknown88 = entry->value;
                    real remaining = entry->value - elapsed;
                    if (remaining < 0.0f) remaining = 0.0f;
                    entry->value = remaining;
                }
                *(s_observer_command *)((byte *)entry + 0x5c) = command;
            }
            else
                ((s_observer_command *)((byte *)entry + 0x5c))->flags &= ~1;
            function_16f190(index, (s_observer_command *)((byte *)entry + 0x5c));
        }
    }
}

void function_23bc90(long unit_index, point3f *position, vector3f *forward);
void function_23ca20(long unit_index, point3f *position, vector3f *forward);

// @retail 0x1557c0
short function_1557c0(long unit_index, point3f *position, vector3f *forward)
{
    short mode;
    short result = function_155810(unit_index, &mode);
    if (result == 0)
        function_23bc90(unit_index, position, forward);
    else
        function_23ca20(unit_index, position, forward);
    return result;
}
