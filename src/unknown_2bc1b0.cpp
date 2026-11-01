#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1523c0.h"
#include "data_array.h"
#include "game_engine_events.h"

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_2BC1B0.CPP: the game engine whose vtable is at 0x45c7a8 (the second
   engine object at 0x47fc84): its slots 30..50, which unknown_1523c0.h numbers
   v0..v20 (its slots 0..28 are c_game_engine_derived's v22..v50 in
   unknown_072c70.cpp), and the helpers they use. The engine keeps up to three
   marker indices in its state (g_51ecc4, defined by unknown_072c70.cpp). */

struct s_game_engine_data;
extern s_game_engine_data *g_51ecc4;

/* the engine's state, as these functions read it */
struct s_state_2bc1
{
	byte unknown00[0xc];
	long markers[3];
	/* each team's countdown to its next point while a player holds its
	   item, and the points counted so far in this hold */
	char point_timers[3];
	char point_tallies[3];
	/* how long each team's item has been away from home */
	short ticks_away[3];
};

long player_find_unit_item_of_type(long player_index, long type);
bool function_15eaf0();
void function_15b930(long player_index, bool by_team, long counter, long delta);

class c_game_engine_45c7a8 : public c_game_engine
{
public:
	virtual void v0(long, long, bool, long);
	virtual void v2(long, long);
	virtual bool v5(long, long);
	virtual long v7(long, byte *);
};

__declspec(noinline) bool function_2bc1b0(point3f *position, long index);

// @retail 0x2bc1b0
bool function_2bc1b0(point3f *position, long index)
{
	bool marker_found_flag = false;
	long marker_index = ((s_state_2bc1 *)g_51ecc4)->markers[index];

	if (marker_index != NONE)
	{
		s_marker_entry *marker = &g_4e0350->marker_entries[marker_index];

		if (position)
		{
			*position = marker->position;
		}
		marker_found_flag = true;
	}
	return marker_found_flag;
}

// @retail 0x2bcc10
void c_game_engine_45c7a8::v0(long player_index, long other_player_index, bool flag, long)
{
	if (player_find_unit_item_of_type(other_player_index, 2) != NONE && !flag && player_index != NONE && player_index != other_player_index)
	{
		function_15b930(player_index, function_15eaf0(), 0x1a, 1);
	}
	if (player_index != NONE && !flag && player_index != other_player_index && player_find_unit_item_of_type(player_index, 2) != NONE)
	{
		function_15b930(player_index, function_15eaf0(), 0x19, 1);
	}
}

// @retail 0x2bcc90
bool c_game_engine_45c7a8::v5(long player_index, long type)
{
	bool result = false;

	if (player_find_unit_item_of_type(player_index, 2) != NONE)
	{
		if (type == 2)
			result = g_4e6948->s232 == 0;
		else if (type == 1)
			result = g_4e6948->flags22c_bits.bit1;
		else if (type == 3)
			result = g_4e6948->flags22c_bits.bit2;
	}
	else
		result = c_game_engine::v5(player_index, type);
	return result;
}

// @retail 0x2bcd20
long c_game_engine_45c7a8::v7(long player_index, byte *b)
{
	long result = NONE;

	*b = 1;
	if (player_find_unit_item_of_type(player_index, 2) != NONE)
		result = 0xe;
	return result;
}

struct s_team_entry;
struct s_spawn_influence_list;
s_team_entry *function_15e410(short team);
void function_15e360(point3f const *point, long object_index);

// @retail 0x2bc340
void function_2bc340(long team)
{
	long *entry = (long *)function_15e410((short)team);
	if (entry)
	{
		point3f position;
		function_2bc1b0(&position, team);
		function_15e360(&position, *entry);
	}
}

void function_23ba10(long type, point3f const *point, s_spawn_influence_list *list);

struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void function_15e050(long object_index, short team);
void __stdcall function_a7870(long object_index);
struct s_placement_2bc1
{
	byte field_0[0x1c];
	point3f position;
	byte field_28[0xc4 - 0x28];
};

// score 5 (was 5): the placement pointer copied for the creation call uses ECX where retail uses
// EAX, and the game options pointer read for the mode test uses EDX where retail uses ECX.
// Existing initializer 89; initializer result 89; initializer before mode 88; paired helpers 88;
// event before team 63; early event plus sender 63; event before creation 63; returning initializer before creation 90.
// Unconditional event with options pointer 32; byte mode before event 31; mode against event type 31;
// mode declaration before team 31; object result before query 16; creation condition assignment 16;
// function-scope object result 16; combined result/mode lifetimes 31; event before mode declaration 31.
// event filled before the mode test 32 (stores now precede the test but the mode load folds into the
// compare after them, 223 bytes); test result as a bool before the fill 76; fill then send helper 32;
// mode read into a local before the fill 31 (load order right, constant 4 not shared in eax);
// function-scope event filled before the test 32. The placement copy is ecx here, eax in retail.
// Round 70: byte-array placement 5; creation assignment in the marker test 5 (on the current
// body); placement at function scope 5; event without its block scope 32 (fill moves before the
// test); named marker-test bool 5.
// Marker-query inline declarations 5; native tag hierarchy 5; creation/announcement/both names 5/5/5;
// missing-tag return 5; tag field at both uses 50; direct tag receiver 5; native tag with both names 5.
// Round 123: const object result 5; early return without an engine 5; team passed without
// its short cast 5; position pointer local for the marker query 5; creation pointer local 5. Diagnostics:
// without the team call 38 (the copy then uses EDX), without the event 142, without the final
// call 39: the copy's register follows the allocation of the whole tail.
// Round 55: early return when no object is created 5; the existing game_engine_get() for the engine
// test 5; the out-of-line event sender 61 (not inlined); tag index read without the definition
// local 5; event declared at the top of the marker block, filled in place 32.
// Round 67: short team at both calls 44 (228 bytes; mode tail exact, marker prefix regresses); short team only
// at the update or at entry with the original marker argument 5 each; event pointer 61 (198 bytes, send not
// inlined); shared creation pointer 5; full-width team copy 5; shared creation pointer plus earlier object
// result 5; NONE-initialized object result 5. A 2400 s permuter run timed out during its initial full build.
// @retail 0x2bc1f0
void function_2bc1f0(long team)
{
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
		long tag_index = *(long *)(*(byte **)(definition + 0xc) + 0xc);
		if (tag_index != NONE)
		{
			s_placement_2bc1 placement;
			function_b7930(&placement, tag_index, NONE, 0);
			if (function_2bc1b0(&placement.position, team))
			{
				long object_index = function_b7b40(&placement);
				if (object_index != NONE)
				{
					function_15e050(object_index, (short)team);
					{
						s_event event;

						game_engine_event_initialize_inline(&event, 4, 1);
						game_engine_event_send_inline(&event);
					}
					function_a7870(object_index);
				}
			}
		}
	}
}

struct s_object_header_2bc1
{
	dword unknown00[2];
	byte *object;
};

struct s_item_position_2bc1
{
	byte unknown00[0x30];
	point3f position;
};

// @retail 0x2bcd50
void c_game_engine_45c7a8::v2(long unused, long list_pointer)
{
	for (long i = 0; i < g_4e6948->s230; i++)
	{
		long *entry = (long *)function_15e410((short)i);

		if (entry && *entry != NONE)
		{
			long object_index = *entry;
			s_item_position_2bc1 *object = (s_item_position_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[object_index & 0xffff].object;

			function_23ba10(6, &object->position, (s_spawn_influence_list *)list_pointer);
			function_23ba10(5, &object->position, (s_spawn_influence_list *)list_pointer);
		}
	}
}

bool function_15b7c0(long delta, long player_index);
long game_engine_get_player_score(dword player_index);
void player_announce_points_to_win(long player_index, long score);
void function_10da60(long item_index, point3f *position);
bool team_ball_away_from_home(long team);
bool function_162b90(long item_index);
void function_1972a0(long player_index, long value14, long value10);
void function_2bce70(long player_index);

/* a team's item, as these functions read it (function_15e410's entries) */
struct s_team_item_2bc1
{
	long object_index;
	short value04;
	byte unknown06[0xc - 6];
	long player_index;
};

/* an item object, as 0x2bc990 reads it */
struct s_item_object_2bc1
{
	long definition_index;
	dword flags;
	byte unknown08[0x14 - 8];
	long parent_index;
	byte unknown18[0x12c - 0x18];
	byte flags12c;
};

/* updates a team's item each tick: how long it has been away from home, its
   return home once it has waited too long, and the points of the player
   holding it */
// MiniMax G run (not kept, m3 review): 33 with the countdown computed before the timer's test, the tally
// counted before the timer reset, and the test written `(unsigned char)timer != 0xff` (a byte compare, as
// retail's cmp al,0xff); the cast is the steering part: without it the same form is 55, under the keep bar.
// A byte cast that only steers the compare is kept only with a byte match.
// score 62 (was 97): retail spills the item pointer to the stack before it
// reads the rate, so the rate and its forty-times multiply land in different
// registers than ours; the countdown is also still decremented in al, while
// retail keeps the decremented byte in cl.
// Round 8: MSVC gives al to whichever byte value is compared as a signed byte
// and moves the other one out of the way, so the countdown can only sit in cl
// while the timer is the compared value; retail compares the countdown and
// copies it into al, which no form here reproduces: the countdown compared
// inside the test 91, before the test 91, as an int expression 67 (a 32-bit
// test), through an int local 91, the expiry test before the store 91, a
// redundant timer test as well 100; the rate in its own short local 61 (the
// same code); the maximum line before the object pointer 70.
// The expiry tested as `timer <= 1` 61 (the countdown lands in cl as in retail, but
// retail compares it through a copy in al); not kept, gain under 8.
// Standard convention (see docs/DECOMPILING.md):
//   1. Retail keeps it __stdcall: the team argument is read from [esp + 0x3c]
//      and the function returns with ret 4. Without the marker LTCG moves the
//      argument into a register and the body changes.
//   2. No data or code in retail holds its address. Its only caller (0x2bcbe5,
//      inside the engine tick at 0x2bcbb0) is LTCG code and pushes the team
//      argument before the call.
//   3. Tried: taking the parameter's address keeps the argument on the stack
//      but changes the body (156 differences); a named team index (97); a
//      separate entry declaration (97); the state pointer loaded once (151);
//      the tick published through a conditional minimum (93); the tick
//      published from the split observation and increment (93); the decremented
//      timer kept as its own byte (91); the ceiling as a signed count (69);
//      the rate read into its own local (69); the tally reached through a
//      pointer to its element (97); the countdown compared as an unsigned byte
//      (62).
// Standard marker with direct input 97; with the conditional tick clamp 93 (not kept: the marker
// needs a matched body); absent holder 98/94; native timer forms 110/108/111/109; direct second
// lookup/test and native/constant team index 93 each.
// team/entry registers and timer/tally schedule remain; conditional tick
// publication 93 (plain 93, rejected: inherited self-address input); publication arms 147;
// direct timer 108; inactive/absent-holder/unexpired exits 97/98/97; split tick increment 97;
// conditional ticks with direct input 156.
// EBP/EDI team-entry roles and timer/tally scheduling differ;
// direct formal 156; entry name 97; object/limit names 97; distinct events 97;
// shared event 97; separate timer values 110; early tally pointer 97; combined 156.
// Separate entry declaration 97; independent/complete failure guards 97/97; multi-use team value 156;
// timer field reference 110; published timer result 97; tally reference/array 97/97; signed ceiling 97; scoring-state lifetime 97.
// Lane 10 r15: retail decrements a copy (`mov cl,al; dec cl; mov al,cl; test al,al`, one jg);
// `countdown <= 0` 91 (one jg, state moves to ESI); the decremented value stored and the field
// tested 91; `--field <= 0` 91; `= --timer` then `timer <= 0` 69 (state back in EBX, only the copy
// and later offsets differ); the decremented value assigned back to the timer 91.
// @retail 0x2bc990
void __stdcall function_2bc990(long team)
{
	s_team_item_2bc1 *entry = (s_team_item_2bc1 *)function_15e410((short)team);

	if (entry && entry->object_index != NONE)
	{
		s_item_object_2bc1 *object = (s_item_object_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[entry->object_index & 0xffff].object;
		dword maximum = g_510c54->field_2_3 * 40;
		point3f position;
		s_state_2bc1 *state;

		function_10da60(entry->object_index, &position);
		if (entry->player_index == NONE && team_ball_away_from_home(team))
		{
			dword ticks;

			state = (s_state_2bc1 *)g_51ecc4;
			ticks = state->ticks_away[team] + 1;
			if (ticks > maximum)
				ticks = maximum;
			state->ticks_away[team] = (short)ticks;
		}
		else
		{
			state = (s_state_2bc1 *)g_51ecc4;
			state->ticks_away[team] = 0;
		}

		/* the item's object, read again for its definition */
		s_item_object_2bc1 *current = (s_item_object_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[entry->object_index & 0xffff].object;
		byte *definition = g_4e3b44[current->definition_index & 0xffff].bytes;

		if (*(short *)(definition + 0x290) == 2 && !(object->flags12c & 1) &&
			((dword)state->ticks_away[team] >= maximum || function_162b90(entry->object_index)) &&
			object->parent_index == NONE && (bool)((object->flags >> 8) & 1))
		{
			s_event event;

			game_engine_event_initialize_inline(&event, 4, 4);
			function_19eb90(&event);
			function_2bc340(team);
			state = (s_state_2bc1 *)g_51ecc4;
		}
		if (entry->player_index != NONE)
		{
			char timer = state->point_timers[team];

			if (timer != NONE)
			{
				char countdown = (char)(timer - 1);
				state->point_timers[team] = countdown;
				if ((unsigned char)countdown == 0 || countdown < 0)
				{
					function_1972a0(entry->player_index, 3, entry->value04);
					function_2bce70(entry->player_index);
					state = (s_state_2bc1 *)g_51ecc4;
					state->point_timers[team] = (char)g_510c54->field_2_3;
					state->point_tallies[team]++;
					function_15b930(entry->player_index, function_15eaf0(), 0x17, 1);
					if (state->point_tallies[team] % 5 == 0)
					{
						s_event event;

						game_engine_event_initialize_inline(&event, 4, 5);
						game_engine_event_set_cause_player(entry->player_index, &event);
						function_19eb90(&event);
					}
				}
			}
		}
		else
		{
			state->point_tallies[team] = 0;
		}
	}
}

// @retail 0x2bce70
void function_2bce70(long player_index)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->w6c == 1 &&
		(g_4e6948->mode == 4 || g_4e9ae8->lc04 == 1))
	{
		long old_score = game_engine_get_player_score(player_index);

		if (function_15b7c0(1, player_index))
		{
			bool by_team = false;

			if (g_55e4d0[g_4e9ae8->engine_index])
			{
				by_team = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
			}
			function_15b930(player_index, by_team, 0x17, 1);
		}
		long score = game_engine_get_player_score(player_index);
		if (old_score != score)
		{
			player_announce_points_to_win(player_index, score);
		}
	}
}
