// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"

/* slot type 0x23 */

struct s_slot_23
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	short unknown0e;
	byte unknown10[0x1b - 0x10];
	bool unknown1b;
	bool unknown1c;
	char unknown1d;
	byte unknown1e[2];
	s_type_c3b527 point;
	vector3f facing;
	byte unknown3c[0x40 - 0x3c];
};

struct s_slot_23_data
{
    bool unknown00;
    bool unknown01;
    short unknown02;
    real unknown04;
    byte unknown08[0x0f - 0x08];
    bool unknown0f;
    bool unknown10;
    char unknown11;
    byte unknown12[2];
    s_type_c3b527 point;
    vector3f facing;
    byte unknown30[0x34 - 0x30];
};

short __stdcall function_1b84a0(long actor_index);
bool __stdcall function_1b85a0(long actor_index, s_slot *slot);
void __stdcall function_1b89d0(long actor_index, s_slot *slot);
void __stdcall function_1b8ae0(long actor_index, s_slot *slot);
bool function_25da00(s_prop_node_view *node);

// @retail 0x1b84a0
short __stdcall function_1b84a0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && !actor->unknown223)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && node->unknown24 >= 3 && (view->unknown70 == 1 || !function_25da00(node)))
			result = 3;
	}
	return result;
}

// @retail 0x1b8540
bool __stdcall function_1b8540(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && prop_node_get(actor->prop_index)->unknown27 < 2)
	{
		s_slot_23 *state = (s_slot_23 *)slot;

		state->unknown0c = false;
		state->unknown0d = false;
		state->unknown0e = 0;
		result = true;
	}
	return result;
}

real function_1f8940(long actor_index);
void *function_1e4db0(long actor_index);

/* the block of the actor's character tag function_1e4db0 returns */
struct s_character_db0_flags
{
	byte flags;
	byte unknown01[0xc - 1];
	real unknown0c;
	real unknown10;
};

bool function_1f86f0(long index);
bool function_1f4f40(long actor_index, short unknown, s_type_c3b527 const *point, bool face_prop, vector3f const *facing);
void function_262800(long actor_index, s_reference reference, bool unknown);

/* slot type 0x23, update: once the actor stands at its prop's position, it
   turns to face it; then it lets the prop go */
// @retail 0x1b89d0
void __stdcall function_1b89d0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_23_data *state = (s_slot_23_data *)slot->unknownc;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (!view)
		{
			state->unknown00 = true;
			return;
		}
		if (state->unknown00)
			return;

		state->unknown00 = state->unknown01 && function_1f86f0(actor_index);
		if (state->unknown00)
		{
			if (!state->unknown0f || !state->unknown11 || state->unknown10)
				return;

			function_1f4f40(actor_index, state->unknown11, &state->point, true, &state->facing);
			state->unknown10 = true;
			state->unknown00 = false;
		}

		if (view->unknown70 != 0 || !function_25da00(node))
			return;

		function_262800(actor_index, actor->unknown418, false);
		state->unknown00 = true;
	}
	else
		state->unknown00 = true;
}

// @retail 0x1b8ae0
void __stdcall function_1b8ae0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));
		s_slot_23_data *state = (s_slot_23_data *)slot->unknownc;

		actor->unknown4a2 = true;
		if (view->unknown06 != 2)
			state->unknown02 = g_510c54->field_2_3;
		else if (state->unknown02 > 0)
			state->unknown02--;

		s_actor_view *current = actor_get(actor_index);
		if (current->unknown50c && current->unknown504 == 1 &&
			!(function_1f8940(actor_index) < 2.0f) && state->unknown02 <= 0)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 0;
		}
		else if (view->unknown70 == 0)
		{
			actor->unknown420 = 2;
			actor->unknown41c = 4;
		}
		else if (view->unknown70 == 1)
		{
			point3f point;

			function_210850(&view->unknown78, &point);
			actor->unknown420 = 3;
			actor->unknown424.point = point;
			actor->unknown41c = 4;
		}

		if (actor->unknown229)
			actor->unknown482 = true;

		s_character_db0_flags *character = (s_character_db0_flags *)function_1e4db0(actor_index);

		if (character && (character->flags & 2) && actor->unknown086 <= 3)
			actor->unknown483 = true;
	}
}

// @retail 0x1b8c60
short __stdcall function_1b8c60(long actor_index, s_slot *slot, bool active)
{
	s_slot_23 *state = (s_slot_23 *)slot;
	short result = g_46fbe8;

	if (state->unknown0c)
		result = g_46fbe4;
	return result;
}

s_slot_handler_2 g_47e908 =
{
	{
		0x23, 2, 0, -2, 0,
		function_1b84a0, function_1b8c60, function_1b8540, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b85a0, function_1b89d0, function_1b8ae0
};

void *function_1e4be0(long actor_index);
bool __stdcall function_1ac610(long actor_index, point3f const *point,
    s_object_marker *markers, short *indices);
short __stdcall function_272af0(s_match_globals *structure, point3f const *point);
struct s_1f4a20_entry;
struct s_1f4a20_source;
bool function_1f4a20(s_1f4a20_entry const *entries, long actor_index, s_reference reference, s_1f4a20_source const *source,
    bool alternate, s_type_c3b527 *point, vector3f *facing,
    signed char *type, bool *available);

// @retail 0x1b85a0
bool __stdcall function_1b85a0(long actor_index, s_slot *slot)
{
    bool result = true;
    s_actor_view *actor = actor_get(actor_index);
    s_slot_23_data *state = (s_slot_23_data *)slot->unknownc;
    if (actor->prop_index == NONE)
        result = false;
    else if (*((bool *)actor + 0x40))
    {
        byte *character;
        bool has_markers;
        s_object_marker markers[32];
        short marker_indices[32];
        s_2605d0_request request;
        real distance;
        {
            s_prop_node_view *node = prop_node_get(actor->prop_index);
            s_prop_view_fields *view = prop_node_view(node);
            character = (byte *)function_1e4be0(actor_index);
            state->unknown0f = false;
            state->unknown10 = false;
            state->unknown11 = 0;
            has_markers = false;
            if (!*((bool *)actor + 0x3c) && actor->unknown227)
            {
                s_prop_state_view *node_state = prop_node_state(node);
                has_markers = function_1ac610(actor_index, &node_state->position,
                    markers, marker_indices);
            }

            memset(&request, 0, sizeof(request));
            request.type = 3;
            if (view && view->unknown70 == 1)
            {
                function_210850(&view->unknown78, (point3f *)((byte *)&request + 0x24));
                short sector = *(short *)((byte *)view + 0x84) == NONE ?
                    *(short *)((byte *)view + 0x72) :
                    function_272af0(g_4e0348, (point3f *)((byte *)&request + 0x24));
                *((bool *)&request + 0x20) = true;
                *(s_type_c3b527 *)((byte *)&request + 0x30) = view->unknown78;
                *(long *)((byte *)&request + 0x40) = *(long *)((byte *)view + 0x74);
                *(short *)((byte *)&request + 0x44) = sector;
                distance = function_210ac0(&view->unknown78, &actor->position);
            }
            else
            {
                *((bool *)&request + 0x52) = false;
                distance = node->unknown28;
            }
    }
        s_character_db0_flags const *movement = (s_character_db0_flags const *)function_1e4db0(actor_index);
        long minimum;
        if (movement)
        {
            minimum = (long)(movement->unknown0c > distance ? distance : movement->unknown0c);
            *(long *)((byte *)&request + 0x684) = minimum;
            *(long *)((byte *)&request + 0x688) = (long)movement->unknown10;
    }
        else
        {
            minimum = 0;
            *(long *)((byte *)&request + 0x684) = 0;
            *(long *)((byte *)&request + 0x688) = 100;
    }
        *(real *)((byte *)&request + 0x48) = 2.0f;
        *((bool *)&request + 0x46) = true;
        *(real *)((byte *)&request + 0x4c) = (real)minimum;
        *((bool *)&request + 0x50) = true;
        *((bool *)&request + 0x59) = character && (*character & 1);
        s_object_marker *request_markers = has_markers ? markers : NULL;
        *(s_object_marker **)((byte *)&request + 0x64) = request_markers;
        if (state->unknown04 != 0.0f)
            *(real *)((byte *)&request + 0x1c) = state->unknown04;
        if (actor->unknown50c && actor_get(actor_index)->unknown504 != 2)
        {
            s_reference actor_reference = actor->unknown418;
            if (!REFERENCE_EQUAL(actor_reference, g_470fa0))
            {
                *((bool *)&request + 0x692) = true;
                *(s_reference *)((byte *)&request + 0x694) = actor_reference;
            }
    }

        byte *scratch = ai_scratch_buffer_get();
        s_261d20_entry entry;
        {
            long other_actor;
            bool unknown;
            s_reference reference = function_261280((s_prop_search *)&request, actor_index,
                &entry, &other_actor, scratch, &unknown);
            if (REFERENCE_EQUAL(reference, g_470fa0) || *(short *)((byte *)&entry + 8) > 1)
            {
                state->unknown00 = true;
                g_46eeb8[0x23]->unknown8 = g_46f348;
            }
            else
            {
                s_262b40_result *location = function_262b40(reference);
                bool available = false;
                bool success = false;
                if (*(short *)((byte *)&entry + 0x5c) != 0 &&
                    *(long *)((byte *)location + 0x14) != NONE &&
                    *(long *)((byte *)location + 0x14) != 0xffff)
                {
                    success = function_1f4a20((s_1f4a20_entry const *)&entry, actor_index, reference,
                        (s_1f4a20_source const *)request_markers, true, &state->point, &state->facing,
                        (signed char *)&state->unknown11, &available);
                    if (success)
                    {
                        state->unknown0f = true;
                        state->unknown10 = false;
                    }
                    else if (available)
                        reference = g_470fa0;
                }
                if (!available && !success)
                    reference = function_2626b0(actor_index, reference, other_actor, scratch, unknown, true);
                if (!REFERENCE_EQUAL(reference, g_470fa0))
                {
                    state->unknown01 = true;
                    actor->unknown4ae = true;
                }
                else
                {
                    state->unknown00 = true;
                    g_46eeb8[0x23]->unknown8 = g_46f348;
                }
            }
    }
        ai_scratch_buffer_release(scratch);
    }
    return result;
}
