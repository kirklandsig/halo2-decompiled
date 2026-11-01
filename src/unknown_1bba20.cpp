// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1BBA20.CPP: what the ai does when units and players act (called
   from the unit code) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"
#include "unknown_0d0690.h"

s_ai_player *ai_player_get(long player_index);

/* a player (0x21c bytes, g_4e8c24) */
struct s_ai_event_player
{
	byte unknown000[0x2c];
	long unit_index;
	byte unknown030[0x21c - 0x30];
};

/* a seat of a unit definition (0xb0 bytes) */
struct s_ai_event_seat
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword flag7 : 1;
	dword flag8 : 1;
	dword flag9 : 1;
	dword flag10 : 1;
	dword flag11 : 1;
	dword unknown : 20;
	byte unknown04[0xb0 - 0x4];
};

struct s_ai_event_unit_definition
{
	byte unknown000[0x1cc];
	s_ai_event_seat *seats;
};

// @retail 0x1bba20
void function_1bba20(long actor_index)
{
	if (g_4f55d0->active)
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown018 != NONE)
			function_20ba60(0x69, actor->unknown018, actor_get(actor_index)->unknown26c, NONE, NONE, NULL);
	}
}

/* a player's unit got into a vehicle */
// @retail 0x1bbc00
void function_1bbc00(long player_index, long vehicle_index)
{
	if (g_4f55d0->active)
	{
		s_ai_event_player *player = (s_ai_event_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_ai_event_player));
		s_ai_player *ai_player = ai_player_get(player_index);
		long unit_index;
		short seat_index;

		if (ai_player)
		{
			ai_player->unit_index = NONE;
			ai_player->unknown08 = NONE;
			ai_player->unknown0a = 0;
		}
		unit_index = player->unit_index;
		s_slot_object_view *unit = object_get(unit_index);
		seat_index = *(volatile short *)&unit->unknown1fc;
		if (seat_index != NONE)
		{
			s_ai_event_unit_definition *definition = (s_ai_event_unit_definition *)g_4e3b44[object_get(vehicle_index)->tag_index & 0xffff].bytes;

			if (!TEST_FIELD_BIT(definition->seats[seat_index].flag11))
				function_20ba60(0x66, unit_index, vehicle_index, NONE, NONE, NULL);
		}
	}
}

static __forceinline long real_to_long(real value);

// score 2 (was 2): BLOCKED-BY 0xd0620 (its formal order): only the setup order of 0xd0620's two
// register arguments differs (retail moves the vehicle index into EAX before it forms the
// iterator's address in EBX; 0xc9200 differs by the same two lines). With 0xd0620 declared as
// (iterator, object_index) in its definition, every declaration and every call site, this
// function, 0xc9200 and 0xdb670 match and nothing is lost, but that needs call-site edits in
// damage.cpp and unknown_26b230.cpp. The child object is kept in a local, so retail's object
// pointer stays in EAX and the tick count converts through ECX/DX like retail.
// Tried: the iterator started from the stored player vehicle 77 (77 again on this base); the
// actor looked up at each store 45; separate conversion locals in the loop 15; the loop product
// passed straight to the conversion 8; the block locals in two other orders 8; the player and
// vehicle parameters swapped 12; the child object in a local with the index read from it 8;
// existing slot-query long/short 61/8; guarded long/short 70/38; query-first long/short 77/46;
// shared-query rounding names/separate loop carriers 61/61; the clock local in the player block
// with the slot local first 8; pointer, type, scope, direct-store and helper forms 8; bounded
// slot-scan short/word/char/byte induction 84/84/85/85, with first-match termination 97 each;
// the iterator's address in a local (declared before the call, with the block locals, assigned
// first) 2; a 300 s permuter run, none below 2; the iterator declared at function scope or first
// in the player block 2/2; the tick conversion before the player stores 21.
// only the EAX vehicle and EBX iterator-address setup order differs; frame 0x1c and retail length agree.
// Earlier: the child object is kept in a local, so retail's object
// pointer stays in EAX and the tick count is converted through ECX/DX like
// retail; only the 0xd0620 call setup still differs, ours forms the iterator
// address in EBX before it moves the vehicle index into EAX.
// Tried: the iterator started from the stored
// the iterator address in EBX for the first child call, and in the child loop
// keeps the object pointer in EAX and converts the tick count through ECX/DX
// where ours uses ECX and EDX/CX. Tried: the iterator started from the stored
// player vehicle 77; the actor looked up at each store 45; separate conversion
// locals in the loop 15; the loop product passed straight to the conversion
// 8; the block locals in two other orders 8; the player and vehicle parameters
// swapped 12; the child object in a local with the index read from it 8.
// 8; the block locals in two other orders 8.
// Earlier 8 from 44.
// Existing slot-query long/short 61/8; guarded long/short 70/38;
// query-first long/short 77/46; shared-query rounding names/separate loop carriers 61/61.
// 8; the block locals in two other orders 8. Separately, from the 44 form: the clock
// local moved into the player block with the slot local declared first, 8 (the same
// lines); pointer, type, scope, direct-store and helper forms on it give the same bytes.
// Earlier 44 -> 8 came from clock/player lifetimes.
// Round 220: Bounded slot-scan short/word/char/byte induction 84/84/85/85;
// first-match result termination with those widths 97/97/97/97. Original body restored.
// Round 398: seconds/ticks/both/elapsed conversion-home names; distinct player/child scratch phases crossed with inner/outer iterator and role names
// 2/2/2/2/2/2/2/2; all restored.
// @retail 0x1bbcc0
void function_1bbcc0(long player_index, long vehicle_index, long seat_index)
{
	if (g_4f55d0->active)
	{
		s_ai_player *player = ai_player_get(player_index);
		long player_slot = NONE;

		for (long i = 0; i < MAXIMUM_AI_PLAYERS; i++)
		{
			if (g_4f55cc[i].player_index == player_index)
			{
				player_slot = i;
				break;
			}
		}
		if (player)
		{
			real time;
			long ticks;
			s_object_child_iterator iterator;

			player->unit_index = vehicle_index;
			player->unknown08 = (short)seat_index;
			time = (real)g_510c54->field_2_3 * 10.0f;
			ticks = real_to_long(time);
			player->unknown0a = (short)ticks;

			function_d0620(&iterator, vehicle_index);
			while (function_d0690(&iterator))
			{
				s_slot_object_view *child = object_get(iterator.child_index);

				if (child->actor_index != NONE)
				{
					s_actor_view *actor = actor_get(child->actor_index);

					actor->unknown31c = (short)player_slot;
					time = (real)g_510c54->field_2_3 * 2.0f;
					ticks = real_to_long(time);
					actor->unknown31e = (short)ticks;
					*(short *)actor->unknown320 = 0;
				}
			}
		}
	}
}


void function_1e3400(long actor_index, short squad_index);
real function_204950(long squad_index, long actor_index, short mode);
void function_26def0(long actor_index, long owner_index);
__declspec(noinline) bool function_f5dc0(long object_index);

// @retail 0x1bba70
void function_1bba70(long actor_index, long vehicle_index, long seat_index)
{
    if (g_4f55d0->active)
    {
        s_actor_view *actor = actor_get(actor_index);
        byte *vehicle = (byte *)object_get(vehicle_index);
        long tag_index = *(long *)vehicle;
        byte *definition = (byte *)g_4e3b44[tag_index & 0xffff].bytes;
        if (*(short *)(definition + 0x1f0) != 6 || *(long *)(vehicle + 0x14) != NONE)
        {
            s_tag_element *entry = function_1e5450(actor_index, tag_index);
            if (entry && (*(byte *)((byte *)entry + 0x10) & 1) && *(long *)((byte *)actor + 0x28) != NONE)
                function_1e3400(actor_index, (short)*(long *)((byte *)actor + 0x28));
            else
            {
                real distance = 3.4028234663852886e+38f;
                if (*(long *)((byte *)actor + 0x28) != NONE)
                    distance = function_204950(*(long *)((byte *)actor + 0x28), actor_index, 0);
                if (*(long *)((byte *)actor + 0x28) != NONE && distance < 3.0f)
                    function_1e3400(actor_index, (short)*(long *)((byte *)actor + 0x28));
                else
                {
                    long squad_index = *(long *)(vehicle + 0x3a0);
                    if (squad_index == NONE || !(3.0f > function_204950(squad_index, actor_index, 0)))
                    {
                        *((byte *)actor + 0x3c) = true;
                        *(short *)((byte *)actor + 0x2c) = g_510c54->field_2_3 * 10;
                        function_26def0(actor_index, vehicle_index);
                    }
                }
            }
        }
        if (!*((byte *)actor + 0x3c))
            *(long *)((byte *)actor + 0x28) = NONE;
        actor->unknown2ec = (short)seat_index;
        actor->unknown2e8 = vehicle_index;
        *(short *)actor->unknown2ee = g_510c54->field_2_3 * 15;
        actor->unknown2f0 = function_f5dc0(vehicle_index);
        actor = actor_get(actor_index);
        *((byte *)actor + 0x5d4) = false;
        *(dword *)((byte *)actor + 0x810) &= ~1;
    }
}

void __stdcall function_1e1a00(long actor_index, long value);
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);
bool function_1e11b0(long actor_index, bool active);
void function_1e1150(long actor_index, short team);

// @retail 0x1bbdf0
void function_1bbdf0(long object_index)
{
    if (g_4f55d0->active)
    {
        s_slot_object_view *parent = object_get(object_index);
        long child_index = *(long *)((byte *)parent + 0x10);
        while (child_index != NONE)
        {
            s_slot_object_view *child = object_get(child_index);
            if (((1 << *((char *)child + 0xaa)) & 3) && child->unknown1fc == NONE)
            {
                byte *definition = g_4e3b44[child->tag_index & 0xffff].bytes;
                if (*(long *)(definition + 0x138) != NONE)
                {
                    long owner_index = *(long *)((byte *)parent + 0x248);
                    if (owner_index != NONE)
                    {
                        s_slot_object_view *owner = object_get(owner_index);
                        long actor_index = child->actor_index;
                        if (actor_index != NONE && actor_get(actor_index)->unknown024 != *(short *)((byte *)owner + 0x138))
                            function_1e1a00(actor_index, 0);
                        if (child->actor_index == NONE)
                        {
                            child_index = function_1e0160(NONE, *(long *)(definition + 0x138), child_index, false);
                            if (child_index != NONE)
                            {
                                function_1e11b0(child_index, true);
                                function_1e1150(child_index, *(short *)((byte *)owner + 0x138));
                            }
                        }
                    }
                    else if (child->actor_index != NONE)
                        function_1e1a00(child->actor_index, 0);
                }
            }
            child_index = *(long *)((byte *)child + 0xc);
        }
    }
}
