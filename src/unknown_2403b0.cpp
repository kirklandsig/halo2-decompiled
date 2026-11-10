#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "unknown_19ec40.h"
#include "unknown_2420a0.h"
#include "bit_vector.h"
#include "game_engine_events.h"

// @flags /O2 /arch:SSE /Gr

bool function_15b2f0();
long function_cbd50(long unit_index, short weapon_index);
short function_101280(long object_index);
bool function_138880();
bool function_138860();
void function_15fe70(long player_index);
bool function_244680(long team, long unit_index);
void __stdcall function_a7810(dword mask);
struct s_242ef1 { point3f const *field_0; };
long function_242ef0(s_242ef1 point, long team);
struct s_team_entry;
s_team_entry *function_15e410(short team);
void function_242ba0(long marker_index, long object_index, long player_index);
void function_197210(long player_index, long value14, long value10);
void function_242e00(long player_index, long team);
void function_242660(long object_index, bool flag);
void function_2430a0(long player_index, bool flag);

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static inline bool slot_flag_clear_2403b0(s_slot_table *table, long team)
{
	return !(table->flags[team] & 1);
}

/* The weapon is looked up through a copy of the object pool pointer taken at entry, so its lookup
   reads the pool again after the two calls, as retail does; the permission is one value (the team's
   entry exists ? its slot is clear : false), which retail materializes (`not al; and al, 1`) before
   the branch. */
// @retail 0x2403b0
void __stdcall function_2403b0(long player_index)
{
	s_event event;
	point3f position;
	s_record_pool *objects = g_4e0300;
	if (g_4e6948->mode == 4 || !function_15b2f0())
		return;
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	long unit_index = *(long *)(player + 0x2c);
	if (unit_index == NONE)
		return;
	byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
	long weapon_index = function_cbd50(unit_index, *(signed char *)(unit + 0x212));
	if (*(short *)(unit + 0x1fc) != NONE || weapon_index == NONE || (short)weapon_index == NONE ||
		function_101280(weapon_index) != 1 || *((byte *)g_4e6948 + 0x1128))
		return;
	byte *weapon = *(byte **)(objects->data + (weapon_index & 0xffff) * 12 + 8);
	long slot = *(short *)(weapon + 0x17e);
	if (slot == NONE)
		return;
	if (!g_55e4d0[g_4e9ae8->engine_index]->p35(player_index, 1) && !function_138880())
		function_15fe70(player_index);
	if (g_4e6948->mode_180 == 9 && !(g_51ec80->flags[slot] & 8))
	{
		signed char team = *(signed char *)(player + 0xc0);
		if (team == NONE)
			return;
		if (function_244680(team, *(long *)(player + 0x2c)))
		{
			if (g_51ec80->e[slot] == NONE)
			{
				game_engine_event_initialize_inline(&event, 10, 14);
				game_engine_event_set_cause_player(player_index, &event);
				function_19eb90(&event);
			}
			long limit = (short)*(short *)((byte *)g_4e6948 + 0x244) * g_510c54->field_2_3;
			long ticks = MIN(g_51ec80->e[slot] + 1, limit);
			g_51ec80->e[slot] = (short)ticks;
			if ((short)ticks == limit)
			{
				byte_flags_set(&g_51ec80->flags[slot], 3, true);
				byte_flags_set(&g_51ec80->flags[slot], 0, true);
				game_engine_event_initialize_inline(&event, 10, 15);
				game_engine_event_set_cause_player(player_index, &event);
				function_19eb90(&event);
				function_a7810(0x200);
			}
		}
		else
			g_51ec80->e[slot] = (short)MAX(g_51ec80->e[slot] - 3, NONE);
		function_a7810(0x100);
		return;
	}
	function_b9dd0(*(long *)(player + 0x2c), &position);
	s_242ef1 point = { &position };
	long marker = function_242ef0(point, *(signed char *)(player + 0xc0));
	if (marker == NONE)
		return;
	bool allowed = true;
	if ((g_4e6948->flags22c & 8) && *(long *)((byte *)g_4e6948 + 0x240) != 1 && *(long *)((byte *)g_4e6948 + 0x240) != 2 && g_4e6948->mode_180 != 9)
	{
		signed char team = *(signed char *)(player + 0xc0);
		allowed = function_15e410(team) ? slot_flag_clear_2403b0(g_51ec80, team) : false;
	}
	if (allowed)
	{
		if (function_138860())
		{
			if (g_4e6948->mode_180 == 9)
				function_242ba0(marker, weapon_index, player_index);
			else
			{
				long team = (short)slot == 8 ? NONE : (short)slot;
				function_197210(player_index, 1, slot);
				function_242e00(player_index, (short)team);
				function_242660(weapon_index, false);
			}
		}
	}
	else
		function_2430a0(player_index, true);
}
