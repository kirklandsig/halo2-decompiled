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
bool function_1f4f40(long actor_index, vector3f const *facing, short unknown, s_type_c3b527 const *point, bool face_prop);
void function_262800(long actor_index, s_reference reference, bool unknown);

/* slot type 0x23, update: once the actor stands at its prop's position, it
   turns to face it; then it lets the prop go */
// score 7 (was 7): only argument temporaries differ: the 0x1f4f40 short argument is converted into DX
// and pushed after `mov eax, ebp` (retail converts AL in place into AX and pushes it first), and the
// 0x262800 reference is loaded into EAX (retail ECX).
// Earlier (47): the actor index, actor, view and state registers differ (retail EBP/EDI/EBX/ESI,
// ours EDI/EBX/EBP/ESI), and retail saves EBX late, giving the NONE path its own epilogue. The forms
// with one actor lookup at the top give index EBP and view EBX as retail, but actor ESI and the state
// EDI, loaded and pushed late. Tried: typed slot data block/current/entry 61/62; on that block actor
// once/at use 66/85; long/word/byte readiness 76/78/77; the readiness stored in the field and tested
// from it 47 (kept, retail's mov eax, 1); on it, one actor lookup at the top 52, plus the state
// declared first 52, a prop index local 52, the typed slot data block 52, node/view declared at the
// top 52, slot cast at each use 52, the last test as if/else-return 52; the NONE case as an early
// store and return 91 (actor EDI as retail, index and state then in scratch registers); getter in
// both NONE test and node lookup 47; actor before the view test 97; actual input/required/done byte
// fields, all subsets, 61 each; completion byte field 61; role names in all index/actor/view subsets
// 47; indexed data block with complete names 47; direct concrete callback input 47/52; native
// success/else-return gates, all subsets 47; native node reference with/without the data block
// 47/47; actor reference after the view 47; entry actor reference variants 52; the slot read once
// through its address 47, with the top actor 52, with the top actor and the state first 52; the
// getter at the 0x262800 call instead of the inner actor local 88 (276 bytes).
// Round 76: retail keeps one actor pointer across the calls (EDI, read at +0x418 at the end); a second
// lookup at the final call reloads the pool after the calls here 71, so the source looks the actor up
// once at the top. That form (52) gives retail's index EBP and view EBX and the shared final store
// (retail duplicates it for the NONE path); left are actor/state ESI/EDI swapped, EBX saved at entry and
// the 0x1f4f40/0x262800 argument temporaries. On it: the slot parameter cast at each use 52; the view
// computed first as NULL-or-view and tested once 52; the NONE case as an early store and return 91.
// Round 198: typed slot data block/current/entry 61/62; on that block actor once/at use 66/85; long/word/byte readiness 76/78/77; stored readiness 47 (exact-match-only, restored). Opening body restored.
// Round 238: actual input/required/done byte fields, all seven subsets, 61 each;
// completion byte field 61. Original Boolean field types and body restored.
// Round 89: the readiness stored in the field and tested from it, kept (retail's mov eax, 1) 47; on
// it, one actor lookup at the top 52 (index EBP and view EBX as retail, actor/state ESI/EDI swapped),
// plus the state declared first 52, a prop index local 52 or the typed slot data block 52; the
// NONE case as an early store and return 91 (actor EDI as retail, more pushes sink).
// Round 173: Role allocation names in all seven index/actor/view subsets 47 each; indexed data block with
// complete names 47. Direct concrete callback input, alone/one actor lookup/role names/both, 47/52/47/52.
// Original body, input declaration and table entry restored; late EBX save and register ownership remain.
// Round 270: native success/else-return gates for active, facing and release phases,
// all seven subsets 47; full gates with the slot data member 47. Original restored.
// Round 252: native node reference with/without the data block 47/47; actor reference
// after the view, alone/with node 47/47; entry actor reference alone/with node/with
// node and data block/with data block 52/52/52/52. Original complete body restored.
// Round 155: on the top actor lookup (index EBP, view EBX as retail; actor ESI/state EDI swapped): node/view
// declared at the top 52, prop-index local 52, slot cast at each use 52, last test as if/else-return 52, NONE
// as an early store and return 91; getter in both NONE test and node lookup 47; actor before the view test 97.
// Round 7 (lane 13): --fast frees this handler's convention (the table is dropped from the reduced image), so
// all trials ran plain. Retail's actor is a local (first load folded, then `add edi, edx`). On the top actor
// form, the completion store written again for the NONE case (if/else) gives retail's late EBX push and own
// NONE epilogue with state ESI and actor EDI, but the index then stays on the stack and the node takes EBX:
// 69; the same with the slot parameter typed `s_slot_23 *` (table cast) 69, node/view declared first 69,
// NONE arm first 91; that store on the current body 97.
// Round 429: complete readiness arms/path-owned completion/both/separate release/
// readiness-release/completion-release/all/all with one actor acquisition
// 104/104/94/47/104/104/94/101 fast; all restored.
// Round 106 (plain): top actor with the unknown0c test folded into the view test 52; the current body as
// single-exit nesting 132 (304 bytes); the top-actor body nested the same way 102 (302 bytes); readiness as
// `... ? true : false` 66; state declared first with both locals const 52.
// Round 238: named initial-completion/facing/release decisions, seven subsets 47 each;
// complete decisions with native slot data block 47. Original callback restored;
// initial one-use completion snapshot rejected without an exact match.
// Round 45 (lane 16): the actor looked up once at the top, the state typed as the slot's data block, a
// guard clause storing the completion for a null view, the completion stored after the final call and
// again in an else for the NONE prop index: 47 -> 7 (kept; retail's registers, both epilogues and the
// late EBX push). Steps: top actor with the NONE store in an else 69, plus state declared first/prop-index
// local/node and view at function scope 69 each; plus the data block 51 (head and NONE epilogue as retail,
// node EBX and view spilled), view renamed 51, view declared first 51, two actor lookups 92, NONE as an
// early store and return 79, view through the index getter after/before the node 74/88, one shared store
// 52; the guard clause 7. On 7: the final call under a positive test 7, `(short)` cast 7, the type byte
// declared signed char 7, a positive test around the 0x1f4f40 call 7.
// Round 23 (lane 15, plain): the type byte in a named char local, tested and passed 11; the 0x262800
// reference read into a local first 7 (its temporary moves EAX -> EDI, retail ECX); the state pointer
// declared before the actor 9; the readiness in a named bool stored and tested 22 (byte setcc); the
// completion store shared after an `if (view)` block 51 (276 bytes). At 0x1afb30's call to 0x1f4f40
// our build converts the byte in place as retail does; here EAX holds the actor index too early.
// Lane 16 r46 (plain): the 0x262800 reference copied into a named local first 7 (its temp moves to EDI).
// Round 213: inline facing callee 7; inline release callee 7; inline node test 7; both call contracts
// inline 7; short facing mode 44; short mode with inline facing callee 44; long facing mode 46; long mode
// with inline facing callee 46; release result used for completion 7; release result with short mode 44;
// release result with long mode 46; original executable source restored.
// Round 59: the facing mode through a field reference/pointer 7 each; the release reference through a field
// reference/pointer 7 each; one actor-index address read at the facing/release call 7 each; the data block
// through its array's address 7; one actor-index reference shared by all three calls 7. All neutral.
// Round 148 (fast): the release reference through a fresh actor getter 57 (286 bytes); the readiness as an
// if/else of two stores 78 (254 bytes); the null-view and already-complete tests merged into one guard 8;
// the facing call in an else of its guard 7; named facing/point pointers for the call 7 (same code); a 600 s
// permuter run (fast, anneal, 80 tries), none below 7.
// Round 56 (lane 11, fast): the seven differences are one register-rotation step. A diagnostic extra dword load
// and global store placed after the guard, in the 0x1f4f40 call's block, gives exactly retail's point EDX, in-place
// type EAX, `push eax` before `mov eax, ebp`, and reference ECX (the two later temporaries take the next registers in
// ECX, EDX, EAX order); the same probe before the guard takes EAX and moves nothing. So retail has one more dword
// temporary before these, one that leaves no code. Forms: type in a local read between the guards 7; short/long/char
// mode local after the guard 7/7/7; named facing/point pointers 7; state copy 7; index copy 7; bool true local 7;
// readiness as embedded assignment 7; long readiness 65 (283 bytes; registers as retail, setne added); byte readiness
// 61 (276; same); dword readiness with a byte store 8; bool readiness with embedded store 22; inline/forceinline
// readiness helper 7/7, on the flag 7, with a result local 22; dead char/long/getter locals in the block 7/7/7;
// a repeated type/flag/done test around the call 7/7/7. Next: a form whose extra temporary coalesces away.
// Round 125 (m3 lane 7, fast; positional/aligned/masked, base 7/6/1): mode assigned in the test 7; read before the guards
// 11; three separate guards 7; short mode after the guards 7; nested first two tests 7; long ready with a bool store
// 65/4/2 (registers retail's, setne added; that setne's temporary is the rotation step); ready through a byte view 7,
// tested as (byte) 7; (byte)ready != 0 stored 64/3/2; byte ready 22; field typed byte with long ready 7, without 22;
// readiness as !!, == true, (bool), !(!a || !b), a ? f() : false, ? 1 : 0, != 0 pair, != false, (long), (byte):
// 7/66/22/7/64/7/7/7/7/22.
// Round 136: node/view hoisted in either order 7/7; actor-first 7; state-first 9; node and view
// hoisted separately 7/7; const actor/state 7; all four pointer locals const 7; read-only view and
// read-only actor/view 7/7. Original declarations restored; NOT kept: no exact match from declaration/lifetime forms.
// Round 133 (fast, which agrees with the full build here): the call and its two stores through a block-scoped
// `s_slot_23 *` pointer, a block data-block pointer re-derived from the slot, or `((s_slot_23 *)slot)->` casts
// 28 each (275 bytes: the type is re-read by a memory movsx and the rotation moves one step the other way,
// reference EDX); the guard, call and stores all through a block data-block or `s_slot_23 *` pointer 7/7 (same
// code); the block pointer declared ahead of the guard with the guard on `state` 28.
// Round 73 (m3 lane 11, fast): the two stores after the 0x1f4f40 call swapped 9; readiness `... != false` 7; the
// type passed as `(short)(signed char)` 7; the done flag tested first in the guard 13; the completion cleared as
// `!state->unknown10` right after setting it 7. All four callees match.
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

			function_1f4f40(actor_index, &state->facing, state->unknown11, &state->point, true);
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
