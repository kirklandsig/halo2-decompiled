// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1C9830.CPP: alliances between teams

LTCG dropped the last argument of function_1df6c0 (always false) and
the bool * of function_1df820.

Two team-by-team bit vectors hold the alliances: ally_bits (read by
function_1df5d0) and peace_bits (function_1df560 is its inverse). A
broken alliance keeps its ally bit but loses its peace bit until enough
time passes without incidents. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_123b30.h"
#include <string.h>

enum
{
	k_maximum_game_teams = 16,
	k_maximum_game_allegiances = 8
};

enum
{
	_game_mode_campaign = 1,
	_game_mode_multiplayer = 2
};

struct s_type_e695f2
{
	short team_a;
	short team_b;
	short incident_threshold;
	short incident_decay_ticks;
	bool team_b_provokes;
	bool team_a_provokes;
	bool broken;
	bool changed;
	short incidents;
	short decay_timer;
	long last_incident_time;
};

struct s_game_allegiance_globals
{
	short allegiance_count;
	byte unknown02[2];
	s_type_e695f2 allegiances[k_maximum_game_allegiances];
	dword ally_bits[k_maximum_game_teams * k_maximum_game_teams / 32];
	dword peace_bits[k_maximum_game_teams * k_maximum_game_teams / 32];
};

s_game_allegiance_globals *g_4f55ec;

bool function_0bfe60(const dword *flags, long bit);
bool function_15e020(short a, short b);
void __stdcall function_1c9830(short team_a, short team_b, bool broken, bool removed);

PRIVATE void __stdcall function_1df9c0(s_type_e695f2 *volatile allegiance, bool broken, bool removed);

// @retail 0x1df460
void function_1df460(void)
{
	g_4f55ec = (s_game_allegiance_globals *)function_123d40("game allegiance globals", "game_allegiance_globals",
		sizeof(s_game_allegiance_globals));
	memset(g_4f55ec, 0, sizeof(s_game_allegiance_globals));
}

// @retail 0x1df4b0
void function_1df4b0(void)
{
	memset(g_4f55ec, 0, sizeof(s_game_allegiance_globals));
	for (short team = 0; team < k_maximum_game_teams; team++)
	{
		long bit = team * k_maximum_game_teams + team;
		g_4f55ec->peace_bits[bit >> 5] |= 1 << (bit & 31);
	}
}

// @retail 0x1df500
void function_1df500(void)
{
	s_type_e695f2 *allegiance = g_4f55ec->allegiances;

	for (short i = 0; i < g_4f55ec->allegiance_count; i++, allegiance++)
	{
		if (allegiance->decay_timer > 0 && --allegiance->decay_timer == 0)
		{
			if (--allegiance->incidents == 0)
				function_1df9c0(allegiance, false, false);
			else
				allegiance->decay_timer = allegiance->incident_decay_ticks;
		}
	}
}

// @retail 0x1df560
bool function_1df560(short team_a, short team_b)
{
	bool result = true;

	if (team_a == NONE || team_b == NONE)
		return true;

	long mode = g_4e6948->state;

	if (mode == _game_mode_campaign)
	{
		if (team_a >= 0 && team_a < k_maximum_game_teams && team_b >= 0 && team_b < k_maximum_game_teams)
		{
			long bit = team_a * k_maximum_game_teams + team_b;
			result = !function_0bfe60(g_4f55ec->peace_bits, bit);
		}
	}
	else if (mode == _game_mode_multiplayer)
	{
		result = function_15e020(team_a, team_b);
	}
	else
	{
		result = team_a != team_b;
	}
	return result;
}

// @retail 0x1df5d0
bool function_1df5d0(short team_a, short team_b)
{
	bool result = false;

	if (team_a == NONE || team_b == NONE)
		return false;

	long mode = g_4e6948->state;

	if (mode == _game_mode_campaign)
	{
		if (team_a >= 0 && team_a < k_maximum_game_teams && team_b >= 0 && team_b < k_maximum_game_teams)
		{
			long bit = team_a * k_maximum_game_teams + team_b;
			result = function_0bfe60(g_4f55ec->ally_bits, bit);
		}
	}
	else if (mode == _game_mode_multiplayer)
	{
		result = !function_15e020(team_a, team_b);
	}
	else
	{
		result = team_a == team_b;
	}
	return result;
}

/* allies whose alliance is broken */
// @retail 0x1df640
bool function_1df640(short team_a, short team_b)
{
	bool result = false;

	if (team_a == NONE || team_b == NONE || team_a == team_b)
		return false;

	if (g_4e6948->state == _game_mode_campaign &&
		team_a >= 0 && team_a < k_maximum_game_teams && team_b >= 0 && team_b < k_maximum_game_teams)
	{
		if (!function_0bfe60(g_4f55ec->ally_bits, team_a * k_maximum_game_teams + team_b) ||
			function_0bfe60(g_4f55ec->peace_bits, team_a * k_maximum_game_teams + team_b))
			return false;
		result = true;
	}
	return result;
}

// @retail 0x1df6c0
void function_1df6c0(short team_a, short team_b, bool team_b_provokes, bool team_a_provokes,
	short incident_threshold, short incident_decay_ticks)
{
	if (g_4e6948->state != _game_mode_campaign)
		return;

	s_game_allegiance_globals *globals = g_4f55ec;
	short count = globals->allegiance_count;
	short i;

	s_type_e695f2 *allegiance = globals->allegiances;

	for (i = 0; i < globals->allegiance_count; i++, allegiance++)
	{
		if (allegiance->team_a == team_a && allegiance->team_b == team_b ||
			allegiance->team_b == team_a && allegiance->team_a == team_b)
			break;
	}

	if (i >= count && count < k_maximum_game_allegiances)
		i = count++, globals->allegiance_count = count;

	if (i < globals->allegiance_count)
	{
		allegiance = &g_4f55ec->allegiances[i];

		allegiance->team_b_provokes = team_b_provokes;
		allegiance->team_a_provokes = team_a_provokes;
		allegiance->team_a = team_a;
		allegiance->team_b = team_b;
		allegiance->incident_threshold = incident_threshold;
		allegiance->incident_decay_ticks = incident_decay_ticks;
		allegiance->incidents = 0;
		allegiance->decay_timer = 0;
		allegiance->broken = true;
		function_1df9c0(allegiance, false, false);
		allegiance->changed = false;
	}
}

// @retail 0x1df770
bool function_1df770(short volatile team_a, short volatile team_b)
{
	short first_team = team_a;
	short const *second_reference = (short const *)&team_b;
	short second_team = *second_reference;
	bool result = false;

	if (g_4e6948->state == _game_mode_campaign)
	{
		s_type_e695f2 *allegiance = g_4f55ec->allegiances;

		for (short i = 0; i < g_4f55ec->allegiance_count; i++, allegiance++)
		{
			if (allegiance->team_a == first_team && allegiance->team_b == second_team ||
				allegiance->team_b == first_team && allegiance->team_a == second_team)
			{
				function_1df9c0(allegiance, true, true);
				g_4f55ec->allegiance_count--;
				if (g_4f55ec->allegiance_count > i)
					g_4f55ec->allegiances[i] = g_4f55ec->allegiances[g_4f55ec->allegiance_count];
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1df820
bool __stdcall function_1df820(volatile short team_a, volatile short team_b, short incident_type)
{
	bool local_4 = false;
	{
		s_game_options_view *local_0 = g_4e6948;
		short local_1 = *(short const *)&team_a;
		short const *local_2 = (short const *)&team_b;
		if (*(volatile long *)&local_0->state == _game_mode_campaign)
		{

			s_game_allegiance_globals *globals = g_4f55ec;
			s_type_e695f2 *allegiance = globals->allegiances;

			for (short i = 0; i < globals->allegiance_count; i++, allegiance++)
			{

				if (allegiance->team_a == local_1 && allegiance->team_b == (*local_2) && *(volatile bool *)&allegiance->team_a_provokes ||
					allegiance->team_b == local_1 && allegiance->team_a == (*local_2) && *(volatile bool *)&allegiance->team_b_provokes)
				{
					s_game_time_globals *game_time = g_510c54;
					long local_3 = game_time->game_time;
					short delta = 0;
					real seconds = game_time->field_2_3 * 0.2f;
					long ticks;

					__asm
					{
						fld seconds
						fistp ticks
					}

					if (local_3 - allegiance->last_incident_time > ticks)
					{

						allegiance->last_incident_time = local_3;
						switch (incident_type)
						{
						case 0: delta = 1; break;
						case 1: delta = 3; break;
						case 2: delta = -1; break;
						}
						allegiance->incidents += delta;
						if (allegiance->incident_decay_ticks != NONE)
							allegiance->decay_timer = allegiance->incident_decay_ticks;
						if (allegiance->incident_threshold != NONE && allegiance->incidents >= allegiance->incident_threshold)
						{
							function_1df9c0(allegiance, true, false);
							function_1c9830(allegiance->team_a, allegiance->team_b, true, false);
							{
								local_4 = true;
								goto local_5;
							}
						}
						goto local_5;
					}
					local_1 = *(short const *)&team_a;
				}
			}
		}
		goto local_5;
	}
local_5:
	return local_4;
}

/* an incident between the teams restarts the decay of their grudge */
// @retail 0x1df950
void function_1df950(short team_a, short team_b)
{
	if (g_4e6948->state != _game_mode_campaign)
		return;

	s_type_e695f2 *allegiance = g_4f55ec->allegiances;

	for (short i = 0; i < g_4f55ec->allegiance_count; i++, allegiance++)
	{
		if (allegiance->team_a == team_a && allegiance->team_b == team_b && allegiance->team_a_provokes ||
			allegiance->team_b == team_a && allegiance->team_a == team_b && allegiance->team_b_provokes)
		{
			if (allegiance->incidents > 0 && allegiance->incident_decay_ticks != NONE)
				allegiance->decay_timer = allegiance->incident_decay_ticks;
			return;
		}
	}
}

// @retail 0x1dfaa0
PRIVATE void function_1dfaa0(dword *vector, long bit, bool value)
{
	if (value)
		vector[bit >> 5] |= 1 << (bit & 31);
	else
		vector[bit >> 5] &= ~(1 << (bit & 31));
}

/* All 213 bytes match with the standard convention. Without this marker,
   the same body takes the pointer in a register and returns ret 8.
   Retail has no absolute reference to this address; its four callers
   (0x1df500, 0x1df6c0, 0x1df770, 0x1df820) push all three arguments.
   Direct pointer copies, parameter references and a force-inline helper
   all used ret 8. Keeping the earlier guard held ret 12 but gave 217 bytes. */
// @retail 0x1df9c0 standard
PRIVATE void __stdcall function_1df9c0(s_type_e695f2 *volatile allegiance, bool broken, bool removed)
{
	s_type_e695f2 *local_0 = *(s_type_e695f2 *const *)&allegiance;
	if (!removed && local_0->broken == broken)
		return;

	local_0->broken = broken;
	if (local_0->team_a < k_maximum_game_teams && local_0->team_b < k_maximum_game_teams)
	{
		function_1dfaa0(g_4f55ec->ally_bits, local_0->team_a * k_maximum_game_teams + local_0->team_b, !removed);
		function_1dfaa0(g_4f55ec->ally_bits, local_0->team_b * k_maximum_game_teams + local_0->team_a, !removed);
		function_1dfaa0(g_4f55ec->peace_bits, local_0->team_a * k_maximum_game_teams + local_0->team_b, !broken);
		function_1dfaa0(g_4f55ec->peace_bits, local_0->team_b * k_maximum_game_teams + local_0->team_a, !broken);
	}
	local_0->changed = true;
	function_1c9830(local_0->team_a, local_0->team_b, broken, removed);
}

#include "data_array.h"
#include "unknown_1fb7e0.h"
#include "unknown_26b230.h"
#include "props.h"

real __stdcall function_265d30(long actor_index, long prop_index);

/* the iterators of unknown_26b230.cpp (clump_next_prop and clump_next_node
   are copies of its own); this one also keeps the clump's datum index */
struct s_indexed_clump_walk
{
	s_clump *clump;
	s_record_pool_iterator pool;
	long clump_index;
};

PRIVATE __forceinline bool indexed_clump_walk_next(s_indexed_clump_walk *iterator)
{
	s_clump *result = NULL;
	if (g_4f55d0->active)
	{
		byte *datum = data_iterator_next_calling(&iterator->pool);
		if (datum)
			result = (s_clump *)datum;
		iterator->clump_index = iterator->pool.datum_index;
	}
	iterator->clump = result;
	return result != NULL;
}

PRIVATE __forceinline s_type_76cf92 *clump_next_prop(s_iterator *iterator)
{
	s_type_76cf92 *prop = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		prop = prop_get(index);
		iterator->index = index;
		iterator->next = *(long *)((byte *)prop + 0x14);
	}
	return prop;
}

PRIVATE __forceinline s_clump_node *clump_next_node(s_iterator *iterator)
{
	s_clump_node *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = (s_clump_node *)(g_502418->data + (index & 0xffff) * sizeof(s_clump_node));
		iterator->index = index;
		iterator->next = node->next;
	}
	return node;
}

/* passes a change in the alliance of two teams on to the props each team's
   clumps hold of the other team */
// @retail 0x1c9830
void __stdcall function_1c9830(short team_a, short team_b, bool broken, bool removed)
{
	if (!g_4f55d0->active)
		return;

	s_indexed_clump_walk clumps;
	clumps.pool.data = g_502420;
	clumps.pool.index = NONE;
	while (indexed_clump_walk_next(&clumps))
	{
		short other_team;
		if (clumps.clump->team == team_a)
			other_team = team_b;
		else if (clumps.clump->team == team_b)
			other_team = team_a;
		else
			continue;
		if (other_team == NONE)
			continue;

		s_iterator props;
		props.next = ((s_clump *)(g_502420->data + (clumps.clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
		s_type_76cf92 *prop;
		while ((prop = clump_next_prop(&props)) != NULL)
		{
			if (prop->team != other_team)
				continue;
			if (!removed)
				prop->unknown24 = true;
			prop->unknown23 = broken;
			prop->unknown3c = true;
			if (!broken && !removed)
				function_20ba60(0xc4, prop->object_index, NONE, NONE, NONE, NULL);
			if (prop->unknown23)
			{
				s_iterator nodes;
				nodes.next = prop_get(props.index)->first_node_index;
				s_clump_node *node;
				while ((node = clump_next_node(&nodes)) != NULL)
				{
					s_type_e5ff81 *tracking = node->tracking_index == NONE ? NULL : tracking_get(node->tracking_index);
					s_type_f95cd3 *view = tracking ? &tracking->view : NULL;
					if (view && prop->unknown23)
						view->unknown3c = function_265d30(node->actor_index, nodes.index);
				}
			}
		}
	}
}
