// @flags /O2 /Ob1 /Gr
#include "object_masks_storage.h"
/* UNKNOWN_0BBF40.CPP: object flag setters of the script functions */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0bbf40.h"
#include "object_iterator.h"
#include "loop_allocator.h"
#include "unknown_1cafc0.h"
#include "object_queries.h"
#include "bit_vector.h"
#include "placement_identifier.h"
#include <string.h>

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

/* the objects (a local view) */
struct s_object_0bbf40
{
	byte unknown00[4];
	dword flags;
};

struct s_object_header_0bbf40
{
	byte unknown00[8];
	s_object_0bbf40 *object;
};

inline s_object_0bbf40 *object_get_0bbf40(long object_index)
{
	return ((s_object_header_0bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
}

// @retail 0xbbf40
void function_bbf40(long object_index, bool flag)
{
	if (object_index != NONE)
	{
		dword *flags = &object_get_0bbf40(object_index)->flags;
		SET_FLAG(*flags, 19, flag);
	}
}

// @retail 0xbbf80
void function_bbf80(long object_index, bool flag)
{
	if (object_index != NONE)
	{
		dword *flags = &object_get_0bbf40(object_index)->flags;
		SET_FLAG(*flags, 24, flag);
	}
}

// @retail 0xbc150
void function_bc150(long object_index, bool flag)
{
	if (object_index != NONE)
	{
		dword *flags = &object_get_0bbf40(object_index)->flags;
		SET_FLAG(*flags, 21, flag);
	}
}
void function_b8b70(long object_index);

struct s_slot_entry_list;
struct s_bsp3d;
extern s_slot_entry_list *g_4e0340;

struct s_1de2c1
{
    long field_0;
    long field_4[256];
};
struct s_1de2c2
{
    s_1de2c1 field_0;
    s_1de2c1 field_404;
    s_1de2c1 field_808;
    s_1de2c1 field_c0c;
};

void function_11bed0(s_location *location, point3f const *point);
void function_11be90(s_location *location, long leaf_index);
bool function_1dde10(s_bsp3d const *bsp, short count, dword const *mask,
    point3f const *centre, real radius, s_1de2c2 *hits);

// @retail 0xbf510
bool function_bf510(long object_index, s_location *location)
{
    (void)&object_index;
    s_1de2c2 hits;
    byte *object = (byte *)((s_object_header_0bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
    point3f const *centre = (point3f const *)(object + 0x40);
    function_11bed0(location, centre);
    if (location->cluster_index == NONE)
    {
        function_1dde10((s_bsp3d const *)g_4e0340, 0, 0, centre, *(real *)(object + 0x4c), &hits);
        if (hits.field_c0c.field_0)
            function_11be90(location, hits.field_c0c.field_4[0]);
        else
            function_11bed0(location, (point3f const *)(object + 0x64));
    }
    return location->cluster_index != NONE;
}

// @retail 0xbc100
void function_bc100(long object_index, bool flag)
{
	if (object_index != NONE)
	{
		dword *flags = &object_get_0bbf40(object_index)->flags;
		SET_FLAG(*flags, 20, flag);
		function_b8b70(object_index);
	}
}

struct s_predicted_resource_block;
bool function_16e5e0(s_predicted_resource_block const *block, short mode);

/* the objects' definitions (a local view) */
struct s_object_definition_0bbf40
{
	byte unknown00[0xb4];
	byte field_84[0xc];
};

/* the objects (another local view) */
struct s_object_tree_0bbf40
{
	long definition_index;
	byte unknown04[0xc - 4];
	long next_object_index;
	long first_child_index;
};

struct s_object_tree_header_0bbf40
{
	byte unknown00[8];
	s_object_tree_0bbf40 *object;
};

/* requests (or releases) the predicted resources of an object definition */
// @retail 0xbbe00
bool function_bbe00(long definition_index, bool load)
{
	bool result = true;
	if (definition_index != NONE)
	{
		if (load)
		{
			s_predicted_resource_block const *block = (s_predicted_resource_block const *)((s_object_definition_0bbf40 *)g_4e3b44[definition_index & 0xffff].bytes)->field_84;
			result = function_16e5e0(block, 1);
		}
		else
		{
			s_predicted_resource_block const *block = (s_predicted_resource_block const *)((s_object_definition_0bbf40 *)g_4e3b44[definition_index & 0xffff].bytes)->field_84;
			result = function_16e5e0(block, 2);
		}
	}
	return result;
}

struct s_object_list;
extern s_object_list *g_4de2f4;
struct s_data_header_40;
extern s_data_header_40 *g_4de2ec;

struct s_object_gc_globals_ab
{
	long unknown00;
	short count;
	short unknown06;
	long first_index;
};

struct s_object_gc_buffer_ab
{
	dword count;
	long indices[0x800];
};

struct s_object_gc_view_ab
{
	byte unknown00[0x14];
	long parent_index;
	long unknown18;
	long next_index;
	byte unknown20[0xb4];
	long simulation_index;
};

// @retail 0xbf1c0
bool function_bf1c0(long level, bool *critical_out)
{
	bool critical = false;
	bool result;
	if (level > 1)
	{
		if (level != 2)
		{
			s_loop_allocator *loop = (s_loop_allocator *)g_4de2ec;
			long used;
			if (!loop->last)
				used = 0;
			else
				used = (byte *)loop->last + loop->last->size - loop->base;
			long remaining = loop->size - used;
			long free_headers = g_4e0300->maximum_count - g_4e0300->high_water_index;
			result = remaining < 0x19999 || free_headers < 0xcc;
			critical = remaining < 0x6666 || free_headers < 0x33;
		}
		else
			result = ((s_object_gc_globals_ab *)g_4de2f4)->count > 30;
	}
	else
		result = true;
	*critical_out = critical;
	return result;
}

// @retail 0xbf240
void __stdcall function_bf240(long level, s_object_gc_buffer_ab *buffer, long size)
{
	buffer->count = 0;
	long index = ((s_object_gc_globals_ab *)g_4de2f4)->first_index;
	while (index != NONE && buffer->count < 0x800)
	{
		s_object_gc_view_ab *object = (s_object_gc_view_ab *)((s_object_header_0bbf40 *)g_4e0300->data)[index & 0xffff].object;
		bool simulation_attached = false;
		if (g_4e6948->mode == 4)
			simulation_attached = object->simulation_index != NONE;
		if (!simulation_attached && object->parent_index == NONE)
			buffer->indices[buffer->count++] = index;
		index = object->next_index;
	}
}

void loop_compact(s_loop_allocator *loop);

// @retail 0xbf360
long __stdcall function_bf360(long level, void *buffer, long size, bool *again, void *unused, long maximum)
{
	loop_compact((s_loop_allocator *)g_4de2ec);
	return 0;
}

struct s_ai_importance_list;
void __stdcall ai_importance_list_build(long level, s_ai_importance_list *buffer, long size);
long __stdcall function_bf2c0(long level, void *buffer, long size, bool *again, void *unused, long maximum);
long __stdcall function_1c8560(long level, void *buffer, long size, bool *again, void *unused, long maximum);
long __stdcall function_1c88c0(long level, void *buffer, long size, bool *again, void *unused, long maximum);

bool function_bba80(long object_index);
void function_bb950(long object_index, bool add, long delta);
void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);

// @retail 0xbf2c0
long __stdcall function_bf2c0(long level, void *buffer, long size, bool *again, void *unused, long maximum)
{
    long result = 0;
    s_object_gc_buffer_ab *entries = (s_object_gc_buffer_ab *)buffer;
    if ((long)entries->count > 0)
    {
        long object_index = entries->indices[--entries->count];
        s_object_header_0bbf40 *header = &((s_object_header_0bbf40 *)g_4e0300->data)[object_index & 0xffff];
        bool remove = true;
        if (g_510c54->game_time < *(long *)((byte *)header->object + 0x20))
            remove = false;
        if (level == 2 && !(((byte *)header)[2] & 1))
            remove = false;
        if ((level == 0 || !function_bba80(object_index)) && remove)
        {
            function_bb950(object_index, false, NONE);
            function_b83b0(object_index, true);
            function_b8460(object_index, true);
            result = 1;
        }
    }
    *again = (long)entries->count > 0;
    return result;
}

typedef void (__stdcall *object_gc_gather_ab)(long, void *, long);
typedef long (__stdcall *object_gc_action_ab)(long, void *, long, bool *, void *, long);
struct s_object_gc_entry_ab
{
    dword levels;
    bool critical_only;
    object_gc_gather_ab gather;
    object_gc_action_ab action;
};

s_object_gc_entry_ab g_440570[6] =
{
    {8, false, 0, function_bf360},
    {15, false, (object_gc_gather_ab)function_bf240, function_bf2c0},
    {8, false, 0, function_bf360},
    {8, true, 0, function_1c8560},
    {8, true, (object_gc_gather_ab)ai_importance_list_build, function_1c88c0},
    {0, false, 0, 0}
};

// score 110 (was 110): the size constant is 0x2a08 where retail uses 0x2a04 and the prologue pushes
// EBP where retail pushes EBX.
// our loop is rotated (the action test copied to the bottom); retail's stays
// top-tested, re-reads the state at its top and duplicates the loop setup into each level path.
// Only nested loops (outer per entry, inner `do ... while (again)`) stay top-tested here: 171, with
// the one exit goto to a shared final clear 192. `again` uninitialised + `if (again) continue` 134
// (gives retail's +8 entry pointer); `next = !again` flag 122 (393 bytes, retail's sete); single
// state variable with the exit goto 161.
// Pressure query inline/noinline/both declarations 110 each; critical output first-written with
// ordinary/inline/noinline/both declarations 138 each; named collection predicate with inline/noinline
// query 110.
// Round 256: Once-observed action/gather/both callbacks 108/110/108; with terminal state at use
// 134/125/134; both with query-produced critical, original/direct cleanup 132/136. All restored.
// Round 300: Named native long/dword level mask at use with compound, rejection-guard and
// nested-positive eligibility 110/110/110/110/110/110; rejection/nested forms with terminal state
// fields at use 125/125. Original frame, loop, field observations and body restored.
// Round 105: retail behaves as if the 0xbf1c0 call may change g_4de2f4 (the loop-top load stays
// in the header and the failure exit reloads); ours keeps it across the call, spilling the level.
// 0xbf1c0 defined after this function 110 (no change); level test as a scope around a while loop
// with the final clear through the entry pointer 179 (330 bytes); state declared before the loop,
// assigned at its top, cleared after it 110; index walk while loop in a level scope 181; that with
// the in-loop action exit 186.
// Round 44: index walk (`g_440570[index].`, retail's +8 entry pointer) 177; index walk + `advance = !again` with the
// in-loop exit through the state local 122 (393 bytes, retail's frame, level register and sete), with an entry pointer
// 122, gathered declared first 121; while + break + advance + after-loop clear 176 (333 bytes), state re-read at the
// loop end 176; index + advance with the in-loop exit reading g_4de2f4 at each use 119 (379 bytes, aligned 59: all but
// the loop rotation). Retail reloads g_4de2f4 after the 0xbf1c0 call on every path (its LTCG treats that call as
// possibly changing it), which keeps its loop top-tested. Original body kept.
// Round 315: Early gather commitment with direct terminal state/query-produced critical/reused action
// ownership masks 0..7: 115/125/138/125/113/134/132/136. Direct-state forms recover the frame but
// shorten to 374 and regress. Original state, query, callbacks, buffers and loop restored at 110.
// Round r4 (lane 17): index walk + advance flag with one final clear (break on the query) 176 (333 bytes); with early
// returns for NONE and the query, global at each use 119 (379, aligned 35: our while is inverted and its first action
// test peeled at the constant first entry, and the NONE exit falls through where retail branches to it); the same
// with the exit test inside for (;;) 119 (same code); NONE as an enclosing if with one final clear and the query
// returning 153 (361); the query in the while condition 176 (333).
// Round 56 (lane 12, fast): the level from an inline pressure-level helper with early returns 270
// (251 bytes); level first, `if (level != NONE)` around `while (entry->action)` with a break on the
// query and one final clear 181 (328 bytes: retail's frame 0x2a04, level in EBP, globals in ESI, but
// VC inverts the loop, testing the first entry's action at its constant address); that with an
// `advance = !again` flag 181, with the query in the while condition 181; the index walk with
// `advance = !again` on this body 131 (400 bytes).
// @retail 0xbf380
void function_bf380()
{
    byte unused[0x200];
    byte scratch[0x2800];
    long level = NONE;
    byte *globals = (byte *)g_4de2f4;
    if (globals[1])
        level = !globals[2];
    else
    {
        s_loop_allocator *loop = (s_loop_allocator *)g_4de2ec;
        long used = loop->last ? (byte *)loop->last + loop->last->size - loop->base : 0;
        if (loop->size - used <= 0xcccc || 0x800 - g_4e0300->actual_count <= 0x66)
            level = 3;
        else if (((s_object_gc_globals_ab *)g_4de2f4)->count >= 0x32)
            level = 2;
    }
    if (level == NONE)
    {
        ((byte *)g_4de2f4)[1] = 0;
        ((byte *)g_4de2f4)[2] = 0;
        return;
    }
    bool critical = false;
    bool again = false;
    bool gathered = false;
    s_object_gc_entry_ab *entry = g_440570;
    for (;;)
    {
        byte *state = (byte *)g_4de2f4;
        if (!entry->action)
        {
            state[1] = 0;
            state[2] = 0;
            return;
        }
        if (!function_bf1c0(level, &critical))
            break;
        if ((entry->levels & (1 << level)) && (!entry->critical_only || critical))
        {
            if (!gathered)
            {
                if (entry->gather)
                    entry->gather(level, scratch, sizeof(scratch));
                gathered = true;
            }
            again = false;
            entry->action(level, scratch, sizeof(scratch), &again, unused, sizeof(unused));
            if (!again)
            {
                entry++;
                gathered = false;
            }
            continue;
        }
        entry++;
        gathered = false;
    }
    ((byte *)g_4de2f4)[1] = 0;
    ((byte *)g_4de2f4)[2] = 0;
}



struct s_model_reference_ab
{
	byte unknown00[0x38];
	long model_index;
};

struct s_model_render_ab
{
	long unknown00;
	long render_model;
	byte unknown08[0xc];
	long field_14_4;
};

transform4x3f *function_b8c00(long object_index, long *node_count);
void object_dispatch_adjust_node_matrices(long object_index, long count, long nodes);

// @retail 0xbe1d0
void function_be1d0(long object_index)
{
	s_object_tree_0bbf40 *object = ((s_object_tree_header_0bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	s_model_reference_ab *definition = (s_model_reference_ab *)g_4e3b44[object->definition_index & 0xffff].bytes;
	if (definition->model_index != NONE)
	{
		s_model_render_ab *model = (s_model_render_ab *)g_4e3b44[definition->model_index & 0xffff].bytes;
		if (model->render_model != NONE && model->field_14_4 != NONE)
		{
			long count;
			transform4x3f *nodes = function_b8c00(object_index, &count);
			object_dispatch_adjust_node_matrices(object_index, count, (long)nodes);
		}
	}
}

/* the same for an object and every object attached to it.
   The standard convention (ret 8):
   1. The body matches retail only with the marker; without it LTCG passes
      the object index in ecx and the flag in dl.
   2. Retail holds no reference to 0xbbec0's address; its callers, 0x2a03d0
      and itself (the recursion over attached objects), push both arguments.
   3. Tried: the plain definition (the register convention above). */
// @retail 0xbbec0 standard
bool __stdcall function_bbec0(long object_index, bool load)
{
	bool result = true;
	if (object_index != NONE)
	{
		s_object_tree_0bbf40 *object = ((s_object_tree_header_0bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
		long child_index;

		result = function_bbe00(object->definition_index, load) & 1;
		for (child_index = object->first_child_index; child_index != NONE; child_index = object->next_object_index)
		{
			object = ((s_object_tree_header_0bbf40 *)g_4e0300->data)[child_index & 0xffff].object;
			result &= function_bbec0(child_index, load);
		}
	}
	return result;
}

// @retail 0xbbe60
bool function_bbe60(long definition_index)
{
	bool result = true;
	if (definition_index != NONE)
	{
		s_predicted_resource_block const *block = (s_predicted_resource_block const *)((s_object_definition_0bbf40 *)g_4e3b44[definition_index & 0xffff].bytes)->field_84;
		result = function_16e5e0(block, 2);
	}
	return result;
}

// @retail 0xbbe90
bool function_bbe90(long definition_index)
{
	bool result = true;
	if (definition_index != NONE)
	{
		s_predicted_resource_block const *block = (s_predicted_resource_block const *)((s_object_definition_0bbf40 *)g_4e3b44[definition_index & 0xffff].bytes)->field_84;
		result = function_16e5e0(block, 1);
	}
	return result;
}

struct s_object_query_bbf40
{
	byte unknown00[0xa4];
	s_scenario_identifier_ab identifier;
	short name_index;
	byte unknownae[0xc2 - 0xae];
	short team;
	long player_index;
	long owner_index;
	byte unknowncc[0x112 - 0xcc];
	short orientations_offset;
	byte unknown114[0x12a - 0x114];
	short animation_offset;
};

struct s_object_query_header_bbf40
{
	byte unknown00[8];
	s_object_query_bbf40 *object;
};

struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

// @retail 0xbc190
void function_bc190(long object_index, s_damage_owner *owner)
{
	s_object_query_bbf40 *object = ((s_object_query_header_bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	owner->object_index = object->owner_index;
	owner->player_index = object->player_index;
	owner->team = object->team;
}

// @retail 0xbf5a0
bool function_bf5a0(long object_index)
{
	s_object_query_bbf40 *object = ((s_object_query_header_bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	long result = object->animation_offset != NONE;
	return result != 0;
}

// @retail 0xbf5d0
bool function_bf5d0(long object_index)
{
	s_object_query_bbf40 *object = ((s_object_query_header_bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	long result = object->orientations_offset != NONE;
	return result != 0;
}


static __forceinline bool object_stamp_ab(long index, long stamp)
{
    bool result = false;
    if (g_4de300[index] != stamp)
    {
        g_4de300[index] = stamp;
        result = true;
    }
    return result;
}

// @retail 0xbec70
bool function_bec70(long object_index)
{
    return object_stamp_ab(object_index & 0xffff, g_4de2fc);
}

extern long *g_4de2d0;

// @retail 0xbf050
void function_bf050(long object_index, short name_index)
{
	s_object_query_bbf40 *object = ((s_object_query_header_bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	if (g_4de2d0[name_index] == NONE)
	{
		g_4de2d0[name_index] = object_index;
		object->name_index = name_index;
	}
}

// @retail 0xbe650
void function_be650(long *list, long object_index)
{
	while (*list != NONE)
	{
		s_object_tree_0bbf40 *object = ((s_object_tree_header_0bbf40 *)g_4e0300->data)[*list & 0xffff].object;
		if (*list == object_index)
		{
			*list = object->next_object_index;
			object->next_object_index = NONE;
			break;
		}
		list = &object->next_object_index;
	}
}

struct s_name_scenario_bbf40
{
	byte unknown00[0x48];
	long count;
};

// @retail 0xbf090
void function_bf090(long object_index)
{
	s_object_query_bbf40 *object = ((s_object_query_header_bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	if (object->name_index != NONE)
	{
		s_name_scenario_bbf40 *scenario = (s_name_scenario_bbf40 *)g_4e0350;
		object->name_index = NONE;
		for (short i = 0; i < scenario->count; i++)
		{
			if (g_4de2d0[i] == object_index)
				g_4de2d0[i] = NONE;
		}
	}
}

struct s_wake_header_bbf40
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_object_tree_0bbf40 *object;
};

void object_widgets_new(long object_index);
void __stdcall function_beca0(long object_index);
void __stdcall function_bd090(long object_index);
bool __stdcall function_bdef0(long object_index);

// @retail 0xbe690
void function_be690(long object_index)
{
	s_wake_header_bbf40 *header = (s_wake_header_bbf40 *)g_4e0300->data + (object_index & 0xffff);
	if (header->type != 5)
		object_widgets_new(object_index);
	else
		*(long *)((byte *)header->object + 0xdc) = NONE;
	function_beca0(object_index);
}

// @retail 0xbd020
void __stdcall function_bd020(long object_index)
{
	s_object_tree_0bbf40 *object = ((s_object_tree_header_0bbf40 *)g_4e0300->data)[object_index & 0xffff].object;
	function_bd090(object_index);
	function_bdef0(object_index);
	long child_index = object->first_child_index;
	while (child_index != NONE)
	{
		s_object_tree_0bbf40 *child = ((s_object_tree_header_0bbf40 *)g_4e0300->data)[child_index & 0xffff].object;
		char type = *(char *)((byte *)child + 0xaa);
		if (!((1 << type) & 0x80))
			function_bd020(child_index);
		child_index = child->next_object_index;
	}
}

void function_b7360(long object_index);

// @retail 0xbc7b0
void __stdcall function_bc7b0(long object_index)
{
	s_wake_header_bbf40 *header = (s_wake_header_bbf40 *)g_4e0300->data + (object_index & 0xffff);
	s_object_tree_0bbf40 *object = header->object;
	if ((1 << header->type) & 0x1003)
		function_b7360(object_index);
	long child_index = object->first_child_index;
	while (child_index != NONE)
	{
		header = (s_wake_header_bbf40 *)g_4e0300->data + (child_index & 0xffff);
		function_bc7b0(child_index);
		child_index = header->object->next_object_index;
	}
}

// the last object whose placement identifier equals the one given, or NONE
// @retail 0xbf760
long function_bf760(long const *unique_id)
{
	s_scenario_identifier_ab const *identifier = (s_scenario_identifier_ab const *)unique_id;
	long result = NONE;
	if (identifier->source != NONE)
	{
		struct
		{
			s_object_query_bbf40 *object;
			s_type_f1af8e iterator;
		} state;
		function_bae80(&state.iterator, 1 << identifier->type, 0);
		while ((state.object = (s_object_query_bbf40 *)function_baeb0(&state.iterator)) != 0)
		{
			if (placement_identifier_equal(identifier, &state.object->identifier))
				result = state.iterator.object_index;
		}
	}
	return result;
}

bool loop_allocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line);
bool loop_reallocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line);

struct s_object_memory_header_ab
{
	short salt;
	byte flags;
	byte type;
	short cluster;
	short size;
	void *object;
};

struct s_object_block_ab
{
	short size;
	short offset;
};

// @retail 0xbc280
long __stdcall function_bc280(short size)
{
	long index = record_pool_allocate(g_4e0300);
	if (index != NONE)
	{
		s_object_memory_header_ab *header = (s_object_memory_header_ab *)g_4e0300->data + (index & 0xffff);
		if (loop_allocate((s_loop_allocator *)g_4de2ec, &header->object, size, 0, 0))
		{
			header->size = size;
			memset(header->object, 0, size);
		}
		else
		{
			record_pool_release(g_4e0300, index);
			index = NONE;
		}
	}
	return index;
}

static __forceinline void object_free_block_ab(s_loop_allocator *loop, void *object)
{
		s_loop_block *block = (s_loop_block *)object - 1;
		loop->free += block->size;
		if (block->previous)
			block->previous->next = block->next;
		else
			loop->first = block->next;
		if (block->next)
			block->next->previous = block->previous;
		else
			loop->last = block->previous;
}

// @retail 0xbc300
void __stdcall function_bc300(long object_index)
{
	s_object_memory_header_ab *header = (s_object_memory_header_ab *)g_4e0300->data + (object_index & 0xffff);
	header->flags = 0;
	if (header->object)
	{
		object_free_block_ab((s_loop_allocator *)g_4de2ec, header->object);
		header->object = 0;
	}
	record_pool_release(g_4e0300, object_index);
}

// score 5 (was 5): the only difference left is the size parameter's load: retail reads it with a
// 16-bit load (a short parameter), ours with a 32-bit one, which shifts every later byte by one.
// With `short size` in the definition and in every declaration (src/unknown_0b7b40.cpp,
// src/unknown_0b8ca0.cpp, src/unknown_118e80.cpp, src/unit_object_type.cpp) the function matches
// byte for byte, but 0x118510 rises 83 -> 102 (its call site then loads the size into ECX where
// retail uses EAX): BLOCKED-BY 0x118510.
// Round 30 (lane C): declaring the third parameter `short` alone makes the reduced image match
// byte for byte, but the full tree then fails to link (LNK2001: ?function_bc380@@YI_NJJJJ@Z is
// still referenced by declarations in four other files), so the parameter stays `long`.
// Forms (score 42 then): a shared absolute index 42; the header reused for the refreshed lookup
// 68; a function-scope block pointer 42; allocator noinline 42; old size read before allocation
// 81; size optimization 107; global optimization off 151; shared index, block scope and allocator
// noinline together 42; full alignment power carried across allocation, signed/unsigned power x
// short/long old size x outcome/allocation result 43, 102, 72, 109, 43, 102, 72, 109; a named
// short length 42; the header table in its own local first 42; mask and extra declared at the top
// 42; the header declared before the result 42; single exit 42; a pool-data local then
// `&headers[index & 0xffff]` (0xbba20's form) 42; the header or the extent renamed 42 each; a
// dword extent 42. With a short size parameter (score 40 then): mask and extent declared first
// 89; a named object-slot pointer for the reallocation and the clear 40; shared absolute index
// 40; single-exit result 40; mask and extent declared at the top, assigned in place 40; header
// declared before the result 40; extent as mask + size 40.
// Round 21 (lane 10): the existing object_get_0bbf40 for the refreshed object only 42, for both
// object reads 8 (header and extent then take retail's EBP/EDI); plus a named new size 6, plus
// `header->size += (short)extra` 7, both 5 (kept); the new size as extra + old_size 8 (same code).
// Round 23 (lane 10): with the long parameter every 16-bit read of it is widened to a 32-bit
// load: the test through a short pointer to the parameter 5; a named short length 5; `!(word)size`
// 5; `(size & 0xffff) == 0` 6; `!(short)size` 5.
// Round 202: entry normalization of size/offset/both/alignment/size-alignment/offset-alignment/
// all domains 89/57/79/5/89/57/79; size normalization with the zero-size result producer 90.
// Original parameters, body and result restored; formal narrowing still needs protected declarations.
// @retail 0xbc380
bool function_bc380(long object_index, long block_offset, long size, long alignment_bits)
{
	bool result = false;
	s_object_memory_header_ab *header = (s_object_memory_header_ab *)g_4e0300->data + (object_index & 0xffff);
	if ((short)size == 0)
	{
		s_object_block_ab *block = (s_object_block_ab *)((byte *)object_get_0bbf40(object_index) + (short)block_offset);
		block->offset = NONE;
		block->size = 0;
		return true;
	}
	long mask = (1 << (byte)alignment_bits) - 1;
	long extra = (short)size + mask;
	long new_size = header->size + extra;
	if (loop_reallocate((s_loop_allocator *)g_4de2ec, &header->object, new_size, 0, 0))
	{
		short old_size = header->size;
		header->size += (short)extra;
		s_object_block_ab *block = (s_object_block_ab *)((byte *)object_get_0bbf40(object_index) + (short)block_offset);
		block->offset = (short)((old_size + mask) & ~mask);
		block->size = (short)size;
		memset((byte *)header->object + old_size, 0, extra);
		result = true;
	}
	return result;
}

long function_baf80(long object_index);
void function_b7360(long object_index);
void __stdcall function_bc7b0(long object_index);

struct s_header_flag7_ab
{
    byte : 7;
    byte flag7 : 1;
};

// @retail 0xbba20
void function_bba20(long object_index)
{
	s_object_header_0bbf40 *headers = (s_object_header_0bbf40 *)g_4e0300->data;
	s_object_header_0bbf40 *header = &headers[object_index & 0xffff];
	if (TEST_FIELD_BIT(((s_header_flag7_ab *)((byte *)header + 2))->flag7))
	{
		object_index = function_baf80(object_index);
		header = &headers[object_index & 0xffff];
	}
	if (*((byte *)header + 2) & 1)
	{
		function_b7360(object_index);
		*((byte *)header + 2) |= 4;
	}
	function_bc7b0(object_index);
}



struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

static __forceinline byte *object_bytes_ab(long index)
{
    return (byte *)((s_object_header_0bbf40 *)g_4e0300->data)[index & 0xffff].object;
}

struct s_object_flags12c_ab
{
    word : 3;
    word flag3 : 1;
    word : 12;
};

// score 49 (was 101): retail saves ESI only after the early return for a unit with a player,
// so that return gets its own epilogue; ours saves ESI in the prologue and shares it.
// Tried: returning early with `if (result) return result;` after the parent walk (118).
// @retail 0xbeb30
bool __stdcall function_beb30(long object_index)
{
    byte *object = object_bytes_ab(object_index);
    byte *unit = (byte *)function_badc0(object_index, 3);
    long player = NONE;
    if (unit) player = *(long *)(unit + 0x13c);
    bool result = player != NONE;
    if (!result)
    {
        long index = *(long *)(object + 0x10);
        while (index != NONE)
        {
            byte *child = object_bytes_ab(index);
            if (function_beb30(index)) return true;
            index = *(long *)(child + 0xc);
        }
        index = *(long *)(object + 0x14);
        while (index != NONE)
        {
            byte *parent = object_bytes_ab(index);
            byte *parent_unit = (byte *)function_badc0(index, 3);
            if (parent_unit && *(long *)(parent_unit + 0x13c) != NONE) return true;
            index = *(long *)(parent + 0x14);
        }
        if (((1 << object[0xaa]) & 0x1c) && TEST_FIELD_BIT(((s_object_flags12c_ab *)(object + 0x12c))->flag3))
            return true;
    }
    return result;
}

struct s_post_physics_object_ab
{
    byte unknown00[0xc];
    long sibling;
    long child;
    byte unknown14[0xb3 - 0x14];
    byte awake_countdown;
    long component;
    byte unknownb8[0xc0 - 0xb8];
    word : 1;
    word flag1 : 1;
    word flag2 : 1;
    word : 3;
    word flag6 : 1;
    word : 9;
    byte unknownc2[0x114 - 0xc2];
    short nodes_size;
};
struct s_post_physics_header_ab
{
    short salt;
    byte flags;
    byte type;
    byte unknown04[4];
    s_post_physics_object_ab *object;
};
void __stdcall function_bc5e0(long object_index);
void __stdcall function_1d3920(byte *component, dword *mask, real value);
void object_dispatch_late_update(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags);

struct s_light_link_ab
{
    byte kind;
    byte unknown01[3];
    long light_index;
};

struct s_light_tree_object_ab
{
    long definition;
    dword flag0 : 1;
    dword : 3;
    dword flag4 : 1;
    dword : 1;
    dword flag6 : 1;
    dword : 25;
    long unknown08;
    long sibling;
    long child;
    byte unknown14[0x11c - 0x14];
    short links_size;
    short links_offset;
};

struct s_light_tree_header_ab
{
    word identifier;
    byte flags;
    byte type;
    dword unknown04;
    s_light_tree_object_ab *object;
};

void function_c3220(long light_index);
void __stdcall function_c2d00(long light_index);
long __stdcall function_c1720(long object_index, byte remove, byte add);

// @retail 0xbef30
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags)
{
    (void)&object_index;
    (void)&remove;
    (void)&add;
    (void)&siblings;
    (void)&own_flags;
    do
    {
        s_light_tree_header_ab *header = &((s_light_tree_header_ab *)g_4e0300->data)[object_index & 0xffff];
        s_light_tree_object_ab *object = header->object;
        bool hidden;
        if ((byte)own_flags)
        {
            if ((header->flags & 0x10) || TEST_FIELD_BIT(object->flag0))
                return;
            hidden = false;
        }
        else
        {
            hidden = object_or_parent_hidden(object_index);
            if (hidden)
                return;
        }
        if (TEST_FIELD_BIT(object->flag4))
        {
            long count = (dword)(long)object->links_size / sizeof(s_light_link_ab);
            s_light_link_ab *links = (s_light_link_ab *)((byte *)object + object->links_offset);
            for (long i = 0; i < count; ++i)
            {
                if (!links[i].kind && links[i].light_index != NONE && !hidden)
                {
                    if ((byte)remove)
                        function_c3220(links[i].light_index);
                    if ((byte)add)
                        function_c2d00(links[i].light_index);
                }
            }
        }
        if (TEST_FIELD_BIT(object->flag6))
            function_c1720(object_index, remove, add);
        if (object->child != NONE)
            function_bef30(object->child, remove, add, 1, !hidden);
        if (!(byte)siblings || object->sibling == NONE)
            return;
        object_index = object->sibling;
    } while (true);
}

// @retail 0xbc820
void __stdcall function_bc820(long object_index)
{
    (void)&object_index;
    dword mask[8];
    s_post_physics_header_ab *header = &((s_post_physics_header_ab *)g_4e0300->data)[object_index & 0xffff];
    s_post_physics_object_ab *object = header->object;
    if (object->component != NONE)
        function_bc5e0(object_index);
    if (!((1 << header->type) & 0x80))
        function_bd090(object_index);
    else if (object->component != NONE && TEST_FIELD_BIT(object->flag6))
    {
        byte *component = g_51e9b8->data + (object->component & 0xffff) * 0xa0;
        if ((bool)((*(dword *)(component + 4) >> 15) & 1))
        {
            long count = (dword)(long)object->nodes_size / sizeof(transform4x3f);
            memset(mask, 0, ((count + 31) >> 5) * sizeof(dword));
            function_1d3920(component, mask, 0.0f);
        }
    }
    object_dispatch_late_update(object_index);
    if (TEST_FIELD_BIT(((s_post_physics_object_ab volatile *)object)->flag2))
        *(word *)((byte *)object + 0xc0) |= 2;
    else
        *(word *)((byte *)object + 0xc0) &= ~2;
    object->flag2 = false;
    bool moved = function_bdef0(object_index);
    if ((header->flags & 0x40) && !moved)
        function_bef30(object_index, 1, 1, 0, false);
    long child = object->child;
    while (child != NONE)
    {
        function_bc820(child);
        child = ((s_post_physics_header_ab *)g_4e0300->data)[child & 0xffff].object->sibling;
    }
    function_be1d0(object_index);
}


void function_a9570(long object_index);
bool object_dispatch_update(long object_index);
bool function_d5de0(long object_index);
bool __stdcall function_be8e0(long object_index);
struct s_havok_component;
void function_1ced00(s_havok_component *component);

// @retail 0xbc470
bool __stdcall function_bc470(long object_index)
{
    s_post_physics_header_ab *header = &((s_post_physics_header_ab *)g_4e0300->data)[object_index & 0xffff];
    bool active = false;
    if (header->flags & 2)
    {
        if (header->flags & 0x20)
            active = true;
        else
        {
            s_post_physics_object_ab *object = ((s_post_physics_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
            header->flags &= ~4;
            if (TEST_FIELD_BIT(object->flag2))
                function_bba20(object_index);
            long mode = g_4e6948->mode;
            if (mode >= 4 && mode <= 5 && (((byte *)object)[0xd8] & 1))
                function_a9570(object_index);
            if (*(short *)((byte *)((s_post_physics_header_ab *)g_4e0300->data)[object_index & 0xffff].object + 0x12a) != NONE)
            {
                s_animation_state *animation = (s_animation_state *)((byte *)object + *(short *)((byte *)object + 0x12a));
                if (animation->graph_tag_index != NONE)
                    active = animation->blend_counters_update();
            }
            active |= object_dispatch_update(object_index);
            active |= function_d5de0(object_index);
            active |= function_be8e0(object_index);
            long child = object->child;
            while (child != NONE)
            {
                active |= function_bc470(child);
                child = ((s_post_physics_header_ab *)g_4e0300->data)[child & 0xffff].object->sibling;
            }
            if (object->component != NONE && TEST_FIELD_BIT(object->flag6))
                function_1ced00((s_havok_component *)(g_51e9b8->data + (object->component & 0xffff) * 0xa0));
            if (object->awake_countdown > 0)
            {
                object->awake_countdown--;
                active = true;
            }
            else if (!active && !(header->flags & 4))
                header->flags &= ~2;
        }
    }
    return active;
}


struct s_object_attachment_ab
{
    char kind;
    byte unknown01[3];
    long index;
};
void __stdcall function_c3260(long light_index, bool clear_object_flag);
bool function_177610(long effect_index);
void contrail_update(long contrail_index, bool detach, real dt);

// @retail 0xbee60
dword function_bee60(long object_index)
{
    s_object_0bbf40 *object = object_get_0bbf40(object_index);
    long count = (dword)(long)*(short *)((byte *)object + 0x11c) / sizeof(s_object_attachment_ab);
    s_object_attachment_ab *attachments = (s_object_attachment_ab *)((byte *)object + *(short *)((byte *)object + 0x11e));
    for (long i = 0; i < count; i++)
    {
        if (attachments[i].kind != NONE && attachments[i].index != NONE)
        {
            switch (attachments[i].kind)
            {
            case 0:
                function_c3260(attachments[i].index, true);
                break;
            case 1:
                record_pool_release(g_4ed28c, attachments[i].index);
                break;
            case 2:
                function_177610(attachments[i].index);
                break;
            case 3:
                function_bd090(object_index);
                contrail_update(attachments[i].index, true, 0.0f);
                break;
            }
        }
        attachments[i].kind = NONE;
        attachments[i].index = NONE;
    }
    bit_vector_set(&object->flags, 4, false);
    bit_vector_set(&object->flags, 5, false);
    return object->flags;
}
