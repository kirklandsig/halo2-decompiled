// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unknown_1e3920.h"
#include "unknown_2605d0.h"
#include "props.h"

/* slot type 0x3a (its first callbacks, from 0x1afde0, precede the region) */

struct s_slot_3a
{
	s_slot_header header;
	byte unknown0c[4];
	s_reference reference;
	byte unknown14;
	bool unknown15;
	bool unknown16;
	byte unknown17[0x40 - 0x17];
};

short __stdcall function_1afde0(long actor_index);
bool __stdcall function_1afe50(long actor_index, s_slot *slot);
void __stdcall function_1c0b60(long actor_index, s_slot *slot, long index);
long __stdcall function_1aff10(long actor_index, s_slot *slot);
void __stdcall function_1b0020(long actor_index, s_slot *slot);
void __stdcall actor_behavior_3a_update48(long actor_index, s_slot *slot);

// @retail 0x1b0270
short __stdcall function_1b0270(long actor_index, s_slot *slot, bool active)
{
	s_slot_3a *state = (s_slot_3a *)slot;
	short result = g_46fbe8;

	if (state->unknown15 || state->unknown16)
	{
		g_46eeb8[0x3a]->unknown8 = g_46f348;
		result = g_46fbe4;
	}
	return result;
}

// @retail 0x1b06f0
void __stdcall function_1b06f0(long actor_index, s_slot *slot, bool active)
{
	s_slot_3a *state = (s_slot_3a *)slot;

	if (!active || (state->reference.unknown2 & 0x8000))
		state->reference = g_470fa0;
}

s_slot_handler_2 g_47dec0 =
{
	{
		0x3a, 2, 0, -2, 0,
		function_1afde0, function_1b0270, function_1afe50, slot_proc_nothing, 0x38, {0},
		function_1c0b60, 0, 0, 0, function_1b06f0, 0, 0
	},
	(t_slot_proc)function_1aff10, function_1b0020, actor_behavior_3a_update48
};

/* Whether the actor has arrived at its reference and is free of a waiting prop. */
// @retail 0x1b02a0
bool function_1b02a0(long actor_index)
{
	bool result = false;
	s_record_pool *actors = g_4f55f0;
	s_actor_view *actor = (s_actor_view *)(actors->data + (actor_index & 0xffff) * sizeof(s_actor_view));

	if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
	{
		s_type_c3b527 *target = (s_type_c3b527 *)function_262b40(actor->unknown418);

		if (target)
		{
			if (((s_actor_view *)actors->data)[actor_index & 0xffff].unknown504 == 2 &&
				(actor->unknown4ac == 4 || actor->unknown4ac == 6 || actor->unknown4ac == 5) &&
				REFERENCE_EQUAL(actor->unknown4b8, actor->unknown418))
			{
				result = true;
				goto done;
			}

			real radius = function_1e3920(actor_index);
			point3f point;
			vector3f offset;

			function_210850(target, &point);
			vector3d_from_points3d(&actor->position, &point, &offset);
			if (offset.j * offset.j + offset.i * offset.i + offset.k * offset.k < radius * radius)
			{
				if (actor->unknown344 == NONE)
				{
					result = true;
					goto done;
				}
				bool waiting = false;
				s_prop_view_fields *view = prop_view_fields_get(actor->unknown344);

				if (view)
					waiting = view->unknown06 == 0 || view->unknown06 == 1;
				result = !waiting;
			}
		}
	}
done:
	return result;
}

struct s_follow_search_state
{
	short count;
	bool flag2;
	bool flag3;
	s_reference reference;
	bool flag8;
	byte unknown09[0x10 - 9];
	long actor_index;
};

struct s_follow_search_fields
{
	byte unknown00[0x20];
	bool use_point;
	byte unknown21[3];
	point3f point;
	s_type_c3b527 location;
	long location_index;
	short value44;
	bool flag46;
	byte unknown47;
	real distance;
	real minimum;
	byte unknown50[2];
	bool flag52;
};

void function_26c180(long actor_index);

// score 29 (was 29): retail loads `unknown` once into ebx, pushes it to
// function_2626b0 and tests bl afterwards; ours reloads it from the stack
// after the call. Tried: a copy `bool not_found = unknown` used in the call
// and the test (109: the tail then matches but for `mov bl, cl`, and the
// top keeps actor_index in edi), the copy only in the test or as `found =
// !unknown` (109, 113), plain locals instead of the struct (31), flag8 set
// by if/else (49), the first reference through a local (29); flag8 as a ternary 27
// (sete), as a result local set by an if 27, with the bool tested first 31, flag3
// cleared before flag8 51, scratch acquired after the output declarations 29.
// Round 44 (lane 10, fast): flag8 as `!(equal || unknown)` 29; the 0x2605d0 result in a reference
// local passed on to 0x2626b0 29; plain `long other_index; bool flag;` locals 31; the entry declared
// before the request 29; flag8 as `equal ? false : !unknown` 27 (sete, still reloaded).
// BLOCKED-BY 0x260670 (its stub keeps the bool's address escaped).
// @retail 0x1b0540
void function_1b0540(long actor_index, s_follow_search_state *state)
{
	s_2605d0_request request;
	memset(&request, 0, sizeof(request));
	s_actor_view *actor = actor_get(actor_index);
	s_follow_search_fields *fields = (s_follow_search_fields *)&request;
	fields->distance = 10.0f;
	fields->minimum = 3.0f;
	request.type = 1;
	fields->flag46 = true;
	request.unknown008 = 4.0f;
	request.unknown00c = 4.0f;
	request.unknown010 = 7.0f;
	if (actor->unknown344 != NONE)
	{
		fields->flag52 = state->flag2;
	}
	else if (state->actor_index != NONE)
	{
		function_26c180(state->actor_index);
		s_actor_view *other = actor_get(state->actor_index);
		s_slot_object_view *unit = object_get(other->unknown018);
		fields->use_point = true;
		fields->point = other->position;
		fields->location_index = other->unknown27c.unknown10;
		fields->location = other->unknown27c.point;
		fields->value44 = *(short *)((byte *)unit + 0x2c);
	}
	if (state->count > 0)
	{
		request.unknown014 = true;
		request.unknown015 = true;
	}
	byte *scratch = ai_scratch_buffer_get();
	s_261d20_entry entry;
	struct { long field_0; bool field_4; } local_0;
	long &other_index = local_0.field_0;
	bool &unknown = local_0.field_4;
	state->reference = function_2605d0(actor_index, &request, (long)&entry, (long)&other_index, scratch, &unknown);
	state->reference = function_2626b0(actor_index, state->reference, other_index, scratch, unknown, false);
	state->flag8 = !REFERENCE_EQUAL(state->reference, g_470fa0) && !unknown;
	state->flag3 = false;
	ai_scratch_buffer_release(scratch);
}


bool unit_submit_entry_request(short fallback_index, short entry_index, long unit_index, long priority, void const *extra);

#pragma inline_depth(0)
// @retail 0x1b0020
void __stdcall function_1b0020(long actor_index, s_slot *slot)
{
    s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));
    ++*(long *)((byte *)slot + 0x18);
    if (*(short *)((byte *)slot + 0xc) > 0)
        --*(short *)((byte *)slot + 0xc);
    long unit_index = actor->unknown018;
    byte *unit = (byte *)((s_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;
    if (*(short *)(unit + *(short *)(unit + 0x342) + 0xc) <= 0)
    {
        if (*(short *)((byte *)slot + 0x22) > 0)
            --*(short *)((byte *)slot + 0x22);
        else
        {
            unit_submit_entry_request(0xc, NONE, unit_index, 0xd, 0);
            dword *local_0 = &g_4e7408->unknown0;
            *local_0 = 1664525 * *local_0 + 1013904223;
            real local_1 = (real)(*local_0 >> 16) * (1.f / 65535.f);
            real local_2 = (0.5f + (1.5f - 0.5f) * local_1) * g_510c54->field_2_3;
            long local_3;
            __asm
            {
                fld local_2
                fistp local_3
            }
            *(short *)((byte *)slot + 0x22) = (short)local_3;
        }
    }
}
#pragma inline_depth()

short __stdcall function_272af0(s_match_globals *arg_0, point3f const *arg_1);
void function_1e3b00(long arg_0, long arg_1, point3f const *arg_2,
    void const *arg_3, void const *arg_4, point3f *arg_5);
long function_baf40(long arg_0);
short __stdcall function_1c8df0(short arg_2, short arg_3, point3f const *arg_4, long arg_0,
    void const *arg_1, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9);

// @retail 0x1b0400
bool function_1b0400(long arg_0, s_follow_search_state *arg_1)
{
    s_actor_view *local_0 = actor_get(arg_0);
    bool local_1 = false;
    s_type_5cfb45 *state;
    s_262b40_result *entry;
    if (local_0->unknown344 != NONE && !REFERENCE_EQUAL(arg_1->reference, g_470fa0))
    {
        entry = function_262b40(arg_1->reference);
        if (entry)
        {
            s_prop_node_view *local_3 = prop_node_get(local_0->unknown344);
            state = function_25d690((s_prop_datum *)local_3);
            point3f local_5;
            point3f local_6;
            function_210850((s_type_c3b527 const *)entry, &local_5);
            if (((s_type_c3b527 const *)entry)->output_index != NONE)
                function_272af0(g_4e0348, &local_5);
            function_1e3b00(local_0->unknown018, 2, &local_5, NULL, NULL, &local_6);
            long local_7 = local_3->object_index;
            byte *local_8 = (byte *)object_get(local_7);
            if (!local_8[0xaa] && *(long *)(local_8 + 0x14) != NONE)
                local_7 = *(long *)(local_8 + 0x14);
            local_7 = function_baf40(local_7);
            short local_9 = function_1c8df0(entry->cluster_index, state->location.cluster_index,
                (point3f const *)((byte *)state + 0x30), local_7, &local_6, 1, false,
                local_0->unknown26c != NONE, false, NULL);
            if (local_3->unknown24 >= 1)
                local_1 = local_9 == 0;
        }
    }
    return local_1;
}
