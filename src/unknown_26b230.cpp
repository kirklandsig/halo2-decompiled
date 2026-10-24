// @flags /O2 /Gr /arch:SSE
#include "object_masks_storage.h"
#include "unknown_11c920.h"
#include "props.h"
#include <float.h>
#include "unknown_26b230.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "unknown_20f040.h"
#include "unknown_2551c0.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "object_iterator.h"
#include "unknown_1e46c0.h"
#include "unknown_0d0690.h"

void function_1e2a90(long actor_index);
void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);
void __stdcall function_b8540(long object_index);
void __stdcall swarm_delete(long index);
bool function_bba80(long object_index);
point3f *function_b9dd0(long object_index, point3f *position);
void function_2691b0(long clump_index);
void function_2687a0(long clump_index);
void function_268900(long clump_index);
void function_2689d0();
__declspec(noinline) void function_268c90();
void function_268700(long clump_index);
void function_269870(long clump_index);
void function_26a210(long clump_index);
void function_26b290(long clump_index);
__declspec(noinline) void function_268840();

static __forceinline long real_to_long(real value);

// score 144 (was 144): frame 0x18 versus 0x20; first-pass pointer/datum homes and slice-call interface differ.
// Native budget with retained helper/count 146; slice datum result with long count 192; stored counter 149; maximum result 143; count arm first 144.
// Depth one 144; unsigned maximum 146; word update state 155; first datum API 272; calling scan 208.
// Activity-controlled first/slice/both loops 158/156/168; signed byte update with short/long maximum 146/152.
// @retail 0x268510
void function_268510()
{
	short interval = (short)real_to_long((real)g_510c54->field_2_3);
	short slice_interval = (short)real_to_long((real)g_510c54->field_2_3 * 0.1f);
	s_record_pool_iterator iterator;
	if (g_4f55d0->active)
	{
		iterator.data = g_502420;
		iterator.index = iterator.datum_index = NONE;
	}
	for (;;)
	{
		byte *clump = g_4f55d0->active ? data_iterator_next_inlined(&iterator) : NULL;
		if (!clump) break;
		if (clump[0x1c]) function_268700(iterator.datum_index);
	}
	long time = g_510c54->game_time;
	if (time % interval == 0) function_268840();
	else if (time % slice_interval == 0)
	{
		struct slice_budget { short maximum; bool updated; } budget = { 0, false };
		short &maximum = budget.maximum;
		bool &updated = budget.updated;
		if (g_4f55d0->active)
		{
			iterator.data = g_502420;
			iterator.index = iterator.datum_index = NONE;
		}
		for (;;)
		{
			byte *clump = g_4f55d0->active ? record_pool_iterator_step(&iterator) : NULL;
			if (!clump) break;
			if (clump[0x1c])
			{
				if (*(short *)(clump + 0x1e) == *(long *)((byte *)g_4f55d0 + 0x18) && !updated)
				{
					function_269870(iterator.datum_index);
					function_26a210(iterator.datum_index);
					function_26b290(iterator.datum_index);
					*(short *)(clump + 0x1e) = 0;
					updated = true;
				}
				else
				{
					long count = ++*(short *)(clump + 0x1e);
					if (count > maximum) maximum = count;
				}
			}
			else *(short *)(clump + 0x1e) = 0;
		}
		*(long *)((byte *)g_4f55d0 + 0x18) = maximum;
	}
}

// score 81 (was 81): retail loads the global and tests its active byte before it reserves the
// frame and pushes the registers, and it keeps the pool pointer in ebx and the index in edi;
// ours reserves the frame first and holds the global in ebp, so the prologue and the loop
// head differ.
// Entry guard 90; inactive return 86; outer current 81; zero initialization 53
// (extra entry stores, restored); byte activity 81; calling scanner 120;
// shared datum iterator 158; inactive continue 81. Original body retained.
// @retail 0x268840
void function_268840()
{
	struct { byte *current; s_record_pool_iterator pool; long datum_index; } iterator;
	if (g_4f55d0->active)
	{
		iterator.pool.data = g_502420;
		iterator.pool.index = NONE;
	}
	for (;;)
	{
		iterator.current = g_4f55d0->active ? data_iterator_next_inlined(&iterator.pool) : NULL;
		byte *clump = iterator.current;
		iterator.datum_index = clump ? iterator.pool.datum_index : NONE;
		if (!clump) break;
		if (clump[0x1c])
		{
			function_2687a0(iterator.datum_index);
			function_268900(iterator.datum_index);
		}
	}
	function_2689d0();
	function_268c90();
}

// score 296 (was 296): frame 0x48 versus 0x50; iterator result/exhaustion and
// player/child-call carriers differ. Current-datum iterator 314; scoped keep 314;
// scoped bool keep 320; entry point scratch 314; entry player pool 297
// (the latter four use the current-datum iterator). All body forms restored.
// frame 0x48 versus 0x50; iterator result and pool homes differ;
// outer while 296; player while 296; actor for-loop 331; native handle parts 327;
// proxy-before-unit loads 296. Original body retained.
// @retail 0x268c90
void function_268c90()
{
	s_record_pool_iterator clumps;
	if (g_4f55d0->active)
	{
		clumps.data = g_502420;
		clumps.index = NONE;
	}
	for (;;)
	{
		byte *clump = g_4f55d0->active ? data_iterator_next_inlined(&clumps) : NULL;
		if (!clump) break;
		bool expired = false;
		if (clump[0x3c])
		{
			if (*(long *)(clump + 0x38) == NONE) expired = true;
			else
			{
				real interval = (real)g_510c54->field_2_3 * 10.0f;
				long ticks = real_to_long(interval);
				expired = *(long *)(clump + 0x38) + ticks < g_510c54->game_time;
			}
		}
		if (expired)
		{
			clump[0x3c] = 0;
			long next_actor = *(long *)(clump + 0x18);
			while (next_actor != NONE)
			{
				long actor_index = next_actor;
				byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
				next_actor = *(long *)(actor + 0x80);
				if (!actor[0xc]) continue;
				long keep = false;
				if (actor[9])
				{
					s_record_pool *players = g_4e8c24;
					long player_index = NONE;
					for (;;)
					{
						player_index = data_next_absolute_index_inlined(players, player_index + 1);
						if (player_index == NONE) break;
						byte *player = players->data + players->size * player_index;
						if (!player) break;
						long unit = *(long *)(player + 0x2c);
						if (unit != NONE)
						{
							point3f point;
							function_b9dd0(unit, &point);
							real z = point.z - *(real *)(actor + 0x240);
							real y = point.y - *(real *)(actor + 0x23c);
							real x = point.x - *(real *)(actor + 0x238);
							if (225.0f > z * z + y * y + x * x) { keep = true; break; }
						}
					}
					if (!keep && *(long *)(actor + 0x18) != NONE)
						keep = function_bba80(*(long *)(actor + 0x18));
				}
				if (keep) { clump[0x3c] = 1; continue; }
				long extra = *(long *)(actor + 0x26c);
				if (extra == *(long *)(actor + 0x18)) extra = NONE;
				byte *current = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
				long unit = *(long *)(current + 0x18);
				long proxy = *(long *)(current + 0x1c);
				function_1e2a90(actor_index);
				if (unit != NONE)
				{
					function_b83b0(unit, true);
					function_b8460(unit, true);
				}
				else if (proxy != NONE) swarm_delete(proxy);
				if (extra != NONE)
				{
					byte *object = NULL;
					if ((extra & 0xffff) < g_4e0300->high_water_index)
					{
						byte *header = g_4e0300->data + (extra & 0xffff) * g_4e0300->size;
						if (*(short *)header && *(short *)header == (extra >> 16) &&
							(1 << header[3])) object = *(byte **)(header + 8);
					}
					if (object)
					{
						s_object_child_iterator children;
						function_d0620(extra, &children);
						bool child_available = function_d0690(&children);
						while (child_available)
						{
							byte *child = *(byte **)(g_4e0300->data + (children.child_index & 0xffff) * 12 + 8);
							if (!child[0xaa]) break;
							child_available = function_d0690(&children);
						}
						if (!child_available) function_b8540(extra);
					}
				}
			}
		}
		if (!*(short *)(clump + 0x10)) function_2691b0(clumps.datum_index);
	}
}


typedef bool (__stdcall *t_transition_test)(long, real *, real *);
typedef short (__stdcall *t_state_update)(long, real *, real *);

struct s_clump_state_entry
{
	long state;
	t_transition_test test;
	t_state_update update;
};

short __stdcall function_26b630(long clump_index, real *b, real *a);
short __stdcall function_26b660(long clump_index, real *b, real *a);
short __stdcall function_26b6b0(long clump_index, real *b, real *a);
bool __stdcall function_26b6e0(long clump_index, real *b, real *a);

short __stdcall function_26b700(long clump_index, real *b, real *a);
bool __stdcall function_26b750(long clump_index, real *b, real *a);

s_clump_state_entry g_470fac[4] =
{
	{0, 0, function_26b630},
	{1, 0, function_26b660},
	{2, function_26b6e0, function_26b6b0},
	{3, function_26b750, function_26b700},
};

// @retail 0x26b230
long function_26b230(long clump_index, long prop_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	long result = NONE;
	long index = clump->first_prop;

	while (index != NONE)
	{
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (index & 0xffff) * sizeof(s_clump_prop));
		if (prop->type == prop_index)
		{
			result = index;
			break;
		}
		index = prop->next;
	}

	return result;
}

PRIVATE __forceinline s_actor_view *clump_next_actor(s_iterator *iterator);

// @retail 0x26b290
void function_26b290(long clump_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	real a[13];
	real b[13];
	s_iterator objects;
	long i;

	for (i = 0; i < 13; i++)
	{
		b[i] = 0.0f;
		a[i] = 0.0f;
	}

	function_26bda0(clump_index, &objects);
	s_actor_view *actor;
	while ((actor = clump_next_actor(&objects)) != NULL)
	{
		short count = ((s_clump_object *)actor)->count;

		for (i = 0; i <= count; i++)
		{
			a[i] += 1.0;
		}
		b[count] += 1.0;
	}

	if (clump->divisor > 0)
	{
		for (i = 0; i < 13; i++)
		{
			b[i] /= clump->divisor;
			a[i] /= clump->divisor;
		}
	}

	long state;
	short new_state = NONE;
	for (state = 3; state > clump->state; state--)
	{
		t_transition_test test = g_470fac[state].test;
		if (test && test(clump_index, b, a))
		{
			new_state = state;
			break;
		}
	}
	if (new_state == NONE)
	{
		new_state = g_470fac[clump->state].update(clump_index, b, a);
	}

	if (new_state != NONE && new_state != clump->state)
	{
		clump->state_time = g_510c54->game_time;
		clump->state = new_state;
	}
}

// @retail 0x26b630
short __stdcall function_26b630(long clump_index, real *b, real *a)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	clump->unknown3e = 0;
	return NONE;
}

// @retail 0x26b660
short __stdcall function_26b660(long clump_index, real *b, real *a)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	if ((real)(g_510c54->game_time - clump->state_time) * g_510c54->rate > 30.0f)
	{
		clump->unknown30 = 0;
		return 0;
	}
	return NONE;
}

// @retail 0x26b6b0
short __stdcall function_26b6b0(long clump_index, real *b, real *a)
{
	short result = NONE;
	if (a[2] == 0.0f)
	{
		result = 1;
	}
	return result;
}

// @retail 0x26b6e0
bool __stdcall function_26b6e0(long clump_index, real *b, real *a)
{
	return a[3] > 0.0;
}

/* ---- whether a clump may use a prop ---- */
struct s_clump_prop_object
{
	byte unknown000[0x10a];
	word unknown10a_0 : 2;
	word flag10a_2 : 1;
	word unknown10a_3 : 13;
};

struct s_clump_prop_object_header
{
	byte unknown00[8];
	s_clump_prop_object *object;
};

s_ai_player *ai_player_get(long player_index);

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

// @retail 0x26b8a0
bool function_26b8a0(long prop_index, long actor_index)
{
	bool result = false;
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
	s_iterator iterator;
	iterator.next = prop->first_node;
	s_clump_node *node;

	while ((node = clump_next_node(&iterator)) != NULL)
	{
		if (node->state < 1 || node->state > 2 || node->actor_index == actor_index)
			continue;

		result = true;
		break;
	}

	return result;
}

struct s_clump_pool_iterator
{
	s_clump *current;
	s_record_pool_iterator pool;
};

PRIVATE __forceinline bool next_clump_in_pool(s_clump_pool_iterator *iterator)
{
	s_clump *result = NULL;
	if (g_4f55d0->active)
		result = (s_clump *)data_iterator_next_calling(&iterator->pool);
	iterator->current = result;
	return result != NULL;
}

// score 68 (was 68): retail sign-extends the first team argument and passes the second one
// narrow, and its frame is four bytes larger than ours, so the stack offsets and the register
// that holds the clump pointer differ as well.
// Shared datum iterator 174; wide side 68; unsigned side 68; initial done 68; complete iterator 115; current
// initialized 68; prop type snapshot 93; prop reference 68.
// @retail 0x26b900
long function_26b900(long prop_index, long actor_index, long clump_index)
{
	bool result = function_26b8a0(prop_index, actor_index);
	if (!result)
	{
		s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
		short side = (short)function_20f040(clump->team);
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
		s_clump_pool_iterator iterator;
		if (g_4f55d0->active)
		{
			iterator.pool.data = g_502420;
			iterator.pool.index = NONE;
		}
		while (next_clump_in_pool(&iterator))
		{
			if (clump_index != iterator.pool.datum_index && side == (short)function_20f040(iterator.current->team))
			{
				long other_prop_index = function_26b230(iterator.pool.datum_index, prop->type);
				if (other_prop_index != NONE && function_26b8a0(other_prop_index, NONE))
				{
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

// @retail 0x26ba60
bool function_26ba60(long prop_index, long actor_index, long clump_index)
{
	bool result = false;
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));

	if (!prop->unknown34)
	{
		if (TEST_FIELD_BIT(((s_clump_prop_object_header *)g_4e0300->data)[prop->type & 0xffff].object->flag10a_2))
			result = false;
		else if (clump_index != NONE)
		{
			s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
			if (!team_is_enemy(clump->team, 1))
			{
				s_game_time_globals *game_time_globals = g_510c54;
				long game_time = game_time_globals->game_time;
				s_data_datum_iterator players;

				/* true unless a player's time stamp is too recent or an actor's prop node is in state 1 or 2 */
				result = true;
				players.data = g_4e8c24;
				players.index = NONE;
				players.datum_index = NONE;
				while (data_datum_iterator_next(&players))
				{
					s_ai_player *player = ai_player_get(players.datum_index);
					if (player && game_time - player->unknown0c < game_time_globals->field_2_3)
						result = false;
				}

				if (result)
				{
					s_iterator actors;
					s_actor_view *actor;

					function_26bda0(clump_index, &actors);
					while ((actor = clump_next_actor(&actors)) != NULL)
					{
						if (actor->prop_index != NONE)
						{
							s_clump_node *node = (s_clump_node *)(g_502418->data + (actor->prop_index & 0xffff) * sizeof(s_clump_node));
							if (node->state < 1 || node->state > 2)
								continue;

							result = false;
							break;
						}
					}
				}
			}
			else
				result = !function_26b900(prop_index, actor_index, clump_index);
		}
	}

	return result;
}


/* Additional views of the same 0x50-byte group records and 0x3c-byte nodes. */
struct s_clump_activity_view
{
	byte unknown00[4];
	point3f center;
	short count;
	short team;
	long first_prop;
	long first_actor;
	bool active;
	byte unknown1d[3];
	long active_time;
	short update_state;
	short state;
	long state_time;
	byte unknown2c[0xc];
	long time38;
	byte unknown3c[4];
	long time40;
	byte unknown44[0xc];
};

struct s_clump_link_view
{
	byte unknown00[4];
	long actor_index;
	long prop_index;
	byte unknown0c[0x28];
	long next;
	long previous;
};

PRIVATE __forceinline s_actor_view *clump_next_actor(s_iterator *iterator)
{
	s_actor_view *actor = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		actor = actor_get(index);
		iterator->index = index;
		iterator->next = actor->next_index;
	}
	return actor;
}

PRIVATE __forceinline void clump_add_position(point3f const *a, point3f const *b, point3f *result)
{
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	result->z = a->z + b->z;
}

// @retail 0x2687a0
void function_2687a0(long clump_index)
{
	s_clump_activity_view *clump = &((s_clump_activity_view *)g_502420->data)[clump_index & 0xffff];
	if (clump->active)
	{
		s_iterator iterator;
		iterator.next = ((s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view)))->first_actor;
		s_actor_view *actor;
		while ((actor = clump_next_actor(&iterator)) != NULL)
		{
			if (actor->unknown009)
				break;
		}
		if (!actor)
		{
			clump->active = false;
			clump->active_time = g_510c54->game_time - 1;
		}
		else
		{
			clump->active_time = g_510c54->game_time;
		}
	}
}

// @retail 0x268810
void function_268810(long clump_index)
{
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->active = true;
	clump->update_state = 1;
}

// @retail 0x268900
void function_268900(long clump_index)
{
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->center.x = 0.0f;
	clump->center.y = 0.0f;
	clump->center.z = 0.0f;
	clump->count = 0;
	s_iterator iterator;
	iterator.next = ((s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view)))->first_actor;
	s_actor_view *actor;
	while ((actor = clump_next_actor(&iterator)) != NULL)
	{
		clump_add_position(&actor->position, &clump->center, &clump->center);
		clump->count++;
	}
	if (clump->count > 0)
	{
		real scale = (real)(1.0 / clump->count);
		clump->center.x = scale * clump->center.x;
		clump->center.y = scale * clump->center.y;
		clump->center.z = scale * clump->center.z;
	}
}

// @retail 0x268c60
void function_268c60(long clump_index)
{
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->time38 = g_510c54->game_time;
}

void function_1e2150(long actor_index, long object_index);

// @retail 0x26abd0
void function_26abd0(s_clump_link_view *node)
{
	s_actor_view *actor = actor_get(node->actor_index);
	long removed;
	if (*(long *)((byte *)node + 0x30) == NONE)
	{
		removed = *(long *)((byte *)actor + 0x58);
		*(long *)((byte *)actor + 0x58) = *(long *)((byte *)node + 0x2c);
	}
	else
	{
		s_clump_link_view *previous = (s_clump_link_view *)(g_502418->data + (*(long *)((byte *)node + 0x30) & 0xffff) * sizeof(s_clump_link_view));
		removed = *(long *)((byte *)previous + 0x2c);
		*(long *)((byte *)previous + 0x2c) = *(long *)((byte *)node + 0x2c);
	}
	if (*(long *)((byte *)node + 0x2c) != NONE)
	{
		s_clump_link_view *next = (s_clump_link_view *)(g_502418->data + (*(long *)((byte *)node + 0x2c) & 0xffff) * sizeof(s_clump_link_view));
		*(long *)((byte *)next + 0x30) = *(long *)((byte *)node + 0x30);
	}
	function_1e2150(node->actor_index, removed);
	node->actor_index = NONE;
	*(long *)((byte *)node + 0x30) = NONE;
	*(long *)((byte *)node + 0x2c) = NONE;
}

// @retail 0x26ac60
void function_26ac60(s_clump_link_view *node)
{
	if (node->previous == NONE)
	{
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (node->prop_index & 0xffff) * sizeof(s_clump_prop));
		prop->first_node = node->next;
	}
	else
	{
		s_clump_link_view *previous = (s_clump_link_view *)(g_502418->data + (node->previous & 0xffff) * sizeof(s_clump_link_view));
		previous->next = node->next;
	}
	if (node->next != NONE)
	{
		s_clump_link_view *next = (s_clump_link_view *)(g_502418->data + (node->next & 0xffff) * sizeof(s_clump_link_view));
		next->previous = node->previous;
	}
	node->prop_index = NONE;
	node->previous = NONE;
	node->next = NONE;
}

PRIVATE __forceinline s_prop_node_view *clump_member_prop_next(s_iterator *iterator);
PRIVATE __forceinline s_type_76cf92 *clump_next_prop(s_iterator *iterator);

// @retail 0x26b1d0
void function_26b1d0(long actor_index)
{
	s_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = clump_member_prop_next(&iterator)) != NULL)
	{
		if (node->unknown24 <= 0)
		{
			function_26abd0((s_clump_link_view *)node);
			record_pool_release(g_502418, iterator.index);
		}
	}
}

// @retail 0x26aaf0
void function_26aaf0(s_clump *clump, long prop_index)
{
	s_clump *const *clump_reference = &clump;
	long const *prop_reference = &prop_index;
	long slot = prop_index & 0xffff;
	long index = *(long *)((byte *)prop_get(prop_index) + 0x18);
	while (index != NONE)
	{
		s_clump_link_view *node = (s_clump_link_view *)(g_502418->data + (index & 0xffff) * sizeof(s_clump_link_view));
		long next = node->next;
		function_26abd0(node);
		function_26ac60(node);
		long tracking = ((s_prop_datum *)node)->tracking_index;
		if (tracking != NONE) record_pool_release(g_502414, tracking);
		record_pool_release(g_502418, index);
		index = next;
	}
	long *link = &(*clump_reference)->first_prop;
	while (*link != NONE)
	{
		long current = *link & 0xffff;
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + current * sizeof(s_clump_prop));
		if (current == slot)
		{
			*link = prop->next;
			break;
		}
		link = &prop->next;
	}
	record_pool_release(g_50241c, *prop_reference);
}

void function_2694d0(long clump_index, long actor_index);

PRIVATE inline s_clump *clump_get(long clump_index)
{
	return (s_clump *)((clump_index & 0xffff) * 0x50 + g_502420->data);
}

// @retail 0x26a480
short function_26a480(long clump_index, short minimum)
{
	short const *minimum_reference = &minimum;
	s_iterator iterator;
	iterator.next = clump_get(clump_index)->first_prop;
	s_clump *clump = clump_get(clump_index);
	short count = 0;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		if (*(short *)((byte *)prop + 0xc) < *minimum_reference)
		{
			function_26aaf0(clump, iterator.index);
			count++;
		}
	}
	return count;
}

// @retail 0x268700
void function_268700(long clump_index)
{
	s_iterator iterator;
	iterator.next = clump_get(clump_index)->first_prop;
	s_clump *clump = clump_get(clump_index);
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		long object_index = *(long *)((byte *)prop + 8);
		byte *object = NULL;
		if (object_index != NONE && (object_index & 0xffff) < g_4e0300->high_water_index)
		{
			s_object_header_view *header = (s_object_header_view *)(g_4e0300->data + g_4e0300->size * (object_index & 0xffff));
			if (header->salt && header->salt == (object_index >> 16) && ((1 << header->type) & -1))
				object = header->object;
		}
		if (!object) function_26aaf0(clump, iterator.index);
	}
}

// @retail 0x2691b0
void function_2691b0(long clump_index)
{
	long const *clump_reference = &clump_index;
	s_iterator props;
	props.next = clump_get(clump_index)->first_prop;
	s_clump *clump = clump_get(clump_index);
	while (clump_next_prop(&props) != NULL)
		function_26aaf0(clump, props.index);
	s_iterator actors;
	actors.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_object;
	while (clump_next_actor(&actors) != NULL)
		function_2694d0(*clump_reference, actors.index);
	record_pool_release(g_502420, *clump_reference);
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);
bool function_bec70(long object_index);
bool function_26add0(long prop_index);

struct s_prop_candidate_view
{
	short type;
	short unknown02;
	long object_index;
	short priority;
	short unknown0a;
	long prop_index;
};

// @retail 0x269e60
short function_269e60(long object_index, s_clump_activity_view const *clump, long prop_index,
	s_prop_candidate_view *candidate, bool force)
{
	long const *object_reference = &object_index;
	s_clump_activity_view const *const *clump_reference = &clump;
	long const *prop_reference = &prop_index;
	s_prop_candidate_view *const *candidate_reference = &candidate;
	bool const *force_reference = &force;
	short type;
	short priority;
	bool unsupported;
	type = priority = 0;
	unsupported = false;
	s_slot_object_view *object = (s_slot_object_view *)function_badc0(object_index, (dword)NONE);
	if (object)
	{
		if (candidate)
		{
			long object_slot = object_index & 0xffff;
			if (g_4de300[object_slot] == g_4de2fc)
				goto done;
			g_4de300[object_slot] = g_4de2fc;
		}
		if (prop_index != NONE)
			force = prop_get(prop_index)->unknown10 > g_510c54->game_time;
		point3f point;
		function_b9dd0(*object_reference, &point);
		vector3f delta;
		delta.i = point.x - clump->center.x;
		delta.j = point.y - clump->center.y;
		delta.k = point.z - clump->center.z;
		real distance_squared = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
		if (*force_reference || distance_squared < 2500.0)
		{
			switch ((char)object->type)
			{
			case 1:
				if (object->actor_index == NONE)
				{
					priority = 1;
					type = 4;
					break;
				}
			case 0:
				{
					bool flag = (bool)((*(byte *)((byte *)object + 0x10a) >> 2) & 1);
					bool player = false;
					short team;
					if (object->actor_index != NONE)
						team = actor_get(object->actor_index)->unknown024;
					else if (object->player_index != NONE)
					{
						team = *(char *)(g_4e8c24->data + (object->player_index & 0xffff) * 0x21c + 0xc0);
						player = true;
					}
					else
						team = object->team;
					bool enemy = team == clump->team ? false : function_1df560(team, clump->team);
					if (flag)
					{
						priority = 1;
						type = enemy ? 6 : 7;
					}
					else if (enemy)
					{
						priority = 3;
						type = 1;
					}
					else
					{
						type = 2;
						if (player) priority = 3;
					}
				}
				break;
			case 12:
				if (*(long *)((byte *)object + 0x134) == 0)
				{
					byte *data = (byte *)object + *(short *)((byte *)object + 0x13a);
					if (function_1df560(*(short *)((byte *)object + 0x12e), clump->team))
					{
						type = 1;
						priority = 3;
					}
					else
					{
						type = 2;
						priority = 0;
					}
					long index = *(long *)(data + 8);
					if (index != NONE)
					{
						s_ai_object_iterator iterator;
						iterator.index = NONE;
						iterator.next_index = perception_get(index)->object_index;
						while (swarm_creature_iterator_next(&iterator))
							function_bec70(iterator.index);
					}
				}
				break;
			case 5:
				{
					byte *tag = g_4e3b44[object->tag_index & 0xffff].bytes;
					if (*(real *)(tag + 0xd0) > 0.0f &&
						(object->parent_index == NONE || (bool)((*(dword *)((byte *)object + 0x12c) >> 5) & 1)))
					{
						priority = 3;
						type = 3;
					}
				}
				break;
			default:
				unsupported = true;
				break;
			}
			if (!*force_reference && priority > 0)
			{
				real distance = (real)sqrt(distance_squared);
				if (*prop_reference != NONE && function_26add0(prop_index))
					priority -= (long)(distance / (*(real *)((byte *)*clump_reference + 0x2c) * 0.6666666865348816f));
				else
					priority -= (long)(distance / (*(real *)((byte *)*clump_reference + 0x2c) * 0.3333333432674408f));
				priority = priority > 0 ? priority : 0;
			}
		}
	}
done:
	if (*candidate_reference && !unsupported)
	{
		candidate->type = type;
		candidate->prop_index = prop_index;
		candidate->object_index = object_index;
		candidate->priority = priority;
	}
	return priority;
}

// @retail 0x269de0
void __stdcall function_269de0(long object_index, s_clump_activity_view const *clump, s_prop_candidate_view *candidates, short *count)
{
	long const *object_reference = &object_index;
	s_clump_activity_view const *const *clump_reference = &clump;
	s_prop_candidate_view *const *candidates_reference = &candidates;
	short *const *count_reference = &count;
	while (*object_reference != NONE)
	{
		if (**count_reference >= 100) break;
		s_slot_object_view *object = object_get(object_index);
		if (function_269e60(object_index, *clump_reference, NONE, &(*candidates_reference)[*count], false) >= (*clump_reference)->update_state)
			++*count;
		if (object->first_child_index != NONE)
			function_269de0(object->first_child_index, *clump_reference, candidates, count);
		object_index = object->next_object_index;
	}
}

// @retail 0x26add0
bool function_26add0(long prop_index)
{
	bool result = false;
	s_clump_prop *const props = (s_clump_prop *)g_50241c->data;
    s_clump_prop *const *props_reference = &props;
    s_clump_prop *prop = &(*props_reference)[prop_index & 0xffff];
	if (*(short *)((byte *)prop + 4) >= 1)
	{
		result = true;
	}
	else
	{
		s_iterator iterator;
		iterator.next = prop->first_node;
		s_clump_node *node;
		while ((node = clump_next_node(&iterator)) != NULL)
		{
			if (node->state >= 1)
			{
				result = true;
				goto done;
			}
		}
	}
done:
	return result;
}

// @retail 0x26b700
short __stdcall function_26b700(long clump_index, real *b, real *a)
{
	short result = NONE;
	if (a[2] == 0.0)
		result = 1;
	else if (a[5] == 0.0f)
		result = 2;
	return result;
}

// @retail 0x26b750
bool __stdcall function_26b750(long clump_index, real *b, real *a)
{
	return a[5] > 0.0;
}

// @retail 0x26b770
void function_26b770(long clump_index)
{
	if (clump_index != NONE)
	{
		s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
		clump->time40 = g_510c54->game_time;
	}
}

// @retail 0x26ba40
bool function_26ba40(long clump_index, long actor_index, long prop_index)
{
	bool result = true;
	if (clump_index != NONE)
		result = !function_26b900(prop_index, actor_index, clump_index);
	return result;
}

PRIVATE __forceinline s_prop_node_view *clump_member_prop_next(s_iterator *iterator)
{
	s_prop_node_view *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = prop_node_get(index);
		iterator->index = index;
		iterator->next = node->next_index;
	}
	return node;
}

// @retail 0x2694d0
void function_2694d0(long clump_index, long actor_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	long *link = &clump->first_object;
	while (*link != NONE)
	{
		long index = *link;
		s_actor_view *actor = actor_get(index);
		if (index == actor_index)
		{
			*link = actor->next_index;
			actor->unknown07c = NONE;
			s_iterator iterator;
			iterator.next = actor_get(actor_index)->first_prop_index;
			s_prop_node_view *node;
			while ((node = clump_member_prop_next(&iterator)) != NULL)
				function_26ac60((s_clump_link_view *)node);
			clump->divisor--;
			return;
		}
		link = &actor->next_index;
	}
}


struct s_prop_notice_view
{
	short fields[10];
};

struct s_prop_copy_view
{
	short salt;
	short type;
	short state;
	short unknown06;
	long object_index;
	short field0c;
	short unknown0e;
	long field10;
	long field14;
	long field18;
	long actor_index;
	short team;
	bool field22;
	bool field23;
	bool field24;
	bool field25;
	short unknown26;
	long field28;
	long field2c;
	byte field30;
	byte field31;
	byte field32;
	byte field33;
	byte field34;
	byte field35;
	byte field36;
	byte unknown37;
	long field38;
	short unknown3c;
	s_prop_notice_view notice;
	byte field52;
	byte field53;
	long field54;
	s_type_5cfb45 observed;
};

void prop_state_initialize(s_type_5cfb45 *state);

// @retail 0x26a310
void function_26a310(s_prop_copy_view *prop, short type, long object_index, s_clump *clump, short value)
{
	prop->type = type;
	prop->object_index = object_index;
	prop->field14 = NONE;
	prop->field0c = value;
	prop->state = g_470f10[type].unknown4 == 0;
	prop->field10 = NONE;
	prop->field18 = NONE;
	s_slot_object_view volatile *object = object_get(object_index);
	object->unknownb2 |= 2;
	unsigned long type_mask = 1 << object->type;
	if (type_mask & 3)
	{
		long actor_index = object->actor_index;
		prop->actor_index = actor_index;
		prop->team = object->team;
		prop->field23 = function_1df560(clump->team, prop->team);
		prop->field24 = !prop->field23;
		prop->field25 = NONE != object->player_index;
		prop->field22 = false;
	}
	else if (type_mask & 0x1000)
	{
		switch (*(long *)((byte *)object + 0x134))
		{
		case 0:
		{
			byte *extra = (byte *)object + *(short *)((byte *)object + 0x13a);
			prop->actor_index = *(long *)(extra + 4);
			prop->team = *(short *)((byte *)object + 0x12e);
			prop->field23 = function_1df560(clump->team, prop->team);
			prop->field24 = !prop->field23;
			prop->field25 = false;
			prop->field22 = *(long *)(extra + 8) != NONE;
			break;
		}
		case 1:
			prop->actor_index = NONE;
			prop->team = NONE;
			prop->field23 = false;
			prop->field24 = false;
			prop->field25 = false;
			prop->field22 = false;
			break;
		}
	}
	else
	{
		prop->actor_index = NONE;
		prop->team = NONE;
		prop->field23 = false;
		prop->field24 = false;
		prop->field25 = false;
		prop->field22 = false;
	}
	prop_state_initialize(&prop->observed);
	*(short *)&prop->field54 = NONE;
	*((short *)&prop->field54 + 1) = NONE;
	prop->field38 = NONE;
	*(real *)&prop->field2c = 0.0f;
	*(real *)&prop->field28 = 0.0f;
	prop->field36 = false;
	prop->field53 = false;
	prop->field52 = false;
	*(bool *)&prop->unknown3c = false;
	prop->field30 = false;
	prop->field32 = false;
	prop->field33 = false;
	prop->field34 = false;
	prop->field35 = false;
}

// @retail 0x26a7d0
void function_26a7d0(s_prop_copy_view *destination, s_prop_copy_view const *source)
{
	destination->state = source->state;
	destination->field2c = source->field2c;
	destination->field28 = source->field28;
	destination->observed = source->observed;
	destination->field52 = source->field52;
	destination->field53 = source->field53;
	destination->field54 = source->field54;
	destination->notice = source->notice;
	destination->field30 = source->field30;
	destination->field32 = source->field32;
	destination->field33 = source->field33;
	destination->field34 = source->field34;
	destination->field35 = source->field35;
	destination->field31 = source->field31;
	destination->field36 = source->field36;
	destination->field38 = source->field38;
}

// @retail 0x26a8d0
long function_26a8d0(long prop_index)
{
	s_prop_copy_view *prop = (s_prop_copy_view *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_prop_copy_view));
	long index = record_pool_allocate(g_502418);
	if (index != NONE)
	{
		s_prop_datum *node = prop_ref_get(index);
		node->unknown10 = 0.0f;
		*(short *)node->unknown0c = 0;
		node->tracking_index = NONE;
		node->unknown1c = NONE;
		node->object_index = *(long *)((byte *)prop + 8);
		node->type = prop->type;
		node->state = prop->state;
		node->prop_index = NONE;
		node->actor_index = NONE;
		node->unknown28 = FLT_MAX;
		node->unknown27 = 0;
		node->next_index = NONE;
		*(long *)((byte *)node + 0x30) = NONE;
		*(long *)((byte *)node + 0x34) = NONE;
		*(long *)((byte *)node + 0x38) = NONE;
	}
	return index;
}


point3f *function_b9dd0(long object_index, point3f *position);

// @retail 0x26aa40
void function_26aa40(long actor_index, long node_index)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_datum *node = prop_ref_get(node_index);
	if (actor->first_prop_index != NONE)
		*(long *)((byte *)prop_ref_get(actor->first_prop_index) + 0x30) = node_index;
	node->next_index = actor->first_prop_index;
	actor->first_prop_index = node_index;
	node->actor_index = actor_index;
	point3f position;
	function_b9dd0(node->object_index, &position);
	vector3f delta;
	vector3d_from_points3d(&position, &actor->position, &delta);
	node->unknown28 = (real)sqrt(delta.j * delta.j + (delta.i * delta.i + delta.k * delta.k));
}


struct s_clump_owner_object_view
{
	byte unknown00[0xaa];
	byte type;
	byte unknownab[0x134 - 0xab];
	long owner_kind;
	short unknown138;
	short owner_offset;
};

PRIVATE __forceinline long clump_object_actor_index(long object_index)
{
	s_clump_owner_object_view *object = (s_clump_owner_object_view *)((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	long result = NONE;
	if (object->type == 0xc)
	{
		byte *owner = object->owner_kind == 0 ? (byte *)object + object->owner_offset : NULL;
		if (owner)
			result = *(long *)(owner + 4);
	}
	return result;
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

// @retail 0x26b120
long function_26b120(long clump_index, long object_index)
{
	long result = NONE;
	long actor_index = clump_object_actor_index(object_index);
	s_iterator iterator;
	iterator.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		if (*(long *)((byte *)prop + 8) == object_index ||
			(prop->unknown22 && actor_index != NONE && actor_index == prop->actor_index))
		{
			result = iterator.index;
			break;
		}
	}
	return result;
}

// @retail 0x26b7a0
void function_26b7a0(long prop_index, long clump_index, bool immediate, bool *available, bool *first, bool *notify)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	s_type_76cf92 *prop = prop_get(prop_index);
	long time = g_510c54->game_time;
	*available = prop->unknown23;
	if (prop->unknown23 && !((s_prop_copy_view *)prop)->field30)
	{
		*first = !clump->unknown30;
		if (prop->unknown25 || immediate)
			*notify = true;
		else if (*(long *)((byte *)clump + 0x34) == NONE || (real)(time - *(long *)((byte *)clump + 0x34)) * g_510c54->rate > 5.0f)
			*notify = clump->state < 3;
		else
			*notify = false;
		*(long *)((byte *)clump + 0x34) = time;
		clump->unknown30 = true;
		((s_prop_copy_view *)prop)->field30 = true;
	}
	else
	{
		*notify = false;
		*first = false;
	}
}


// @retail 0x268bb0
real function_268bb0(s_clump_activity_view const *clump, s_actor_view const *actor)
{
	vector3f delta;
	vector3d_from_points3d(&actor->position, &clump->center, &delta);
	real distance = (real)sqrt(delta.k * delta.k + (delta.j * delta.j + delta.i * delta.i));
	if (distance > 6.0)
		return 0.0f;
	if (distance < 2.5)
		return 1.0f;
	double value = (short)(clump->count * 10.0 / distance);
	if (value > 1.0)
		value = 1.0;
	return (real)value;
}

// @retail 0x26a960
void function_26a960(s_clump *clump, long prop_index)
{
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
	prop->next = clump->first_prop;
	clump->first_prop = prop_index;
	s_iterator iterator;
	iterator.next = clump->first_object;
	while (clump_next_actor(&iterator))
	{
		long node_index = function_26a8d0(prop_index);
		if (node_index != NONE)
		{
			prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
			s_prop_datum *node = prop_ref_get(node_index);
			long previous = prop->first_node;
			if (previous != NONE)
				*(long *)((byte *)prop_ref_get(previous) + 0x38) = node_index;
			*(long *)((byte *)node + 0x34) = previous;
			prop->first_node = node_index;
			node->prop_index = prop_index;
			function_26aa40(iterator.index, node_index);
		}
	}
}


// @retail 0x26bc60
long function_26bc60(long clump_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	struct
	{
		real count;
		real value;
		s_clump_pool_iterator iterator;
		long team;
	} state;
	state.count = 0.0f;
	state.value = 0.0f;
	state.team = ((s_clump volatile *)clump)->team;
	long side = function_20f040((short)state.team);
	if (g_4f55d0->active)
	{
		state.iterator.pool.data = g_502420;
		state.iterator.pool.index = NONE;
	}
	for (;;)
	{
		s_clump *other = NULL;
		if (g_4f55d0->active)
			other = (s_clump *)data_iterator_next_inlined(&state.iterator.pool);
		state.iterator.current = other;
		if (!other)
			break;
		long other_side = function_20f040(other->team);
		if ((short)other_side == (short)side)
		{
			state.count += other->divisor;
			state.value += (short)other->unknown3e;
		}
	}
	if (state.count * 0.2f > state.value)
		return 1;
	if (state.count * 0.8f > state.value)
		return 0;
	return 2;
}

/* the clump pool walk 0x269040 makes: like s_clump_pool_iterator with a trailing index slot,
   as s_ai_importance_squad_iterator (ai.cpp) has; retail's frame keeps the slot */
struct s_clump_walk_269040
{
	s_clump *clump;
	s_record_pool_iterator pool;
	long clump_index;
};

/* record_pool_iterator_step with its result cleared before the search */
PRIVATE __forceinline byte *clump_walk_step_269040(s_record_pool_iterator *iterator)
{
	byte *result = NULL;
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
	}
	return result;
}

PRIVATE __forceinline s_clump *clump_walk_advance_269040(s_clump_walk_269040 *walk)
{
	s_clump *clump = NULL;
	if (g_4f55d0->active)
		clump = (s_clump *)clump_walk_step_269040(&walk->pool);
	walk->clump = clump;
	return clump;
}

// score 106 (was 127): the frame and the walk's slots now match; retail clears the clump
// register (ebp) before testing the active flag and keeps the result in ebx, the pool in esi
// and the index in edi; ours clears it just before the search call and keeps the result in
// ebp, NONE in ebx and the pool in edi. Tried: the stock data_iterator_next_calling in the
// loop body (139, the empty search jumps straight out), next_clump_in_pool (125),
// record_pool_iterator_step (164, not inlined), the step written into one helper with the
// clump cleared first (139), the stock step under this walk (124). Tried at 127 (lane): plain
// oldest-time/index storage separately 136/126; byte activity 127; native pool step 160; activity while 136.
// @retail 0x269040
long function_269040(point3f const *center, short team)
{
	long result = record_pool_allocate(g_502420);
	if (result == NONE)
	{
		s_clump_walk_269040 walk;
		if (g_4f55d0->active)
		{
			walk.pool.data = g_502420;
			walk.pool.index = NONE;
		}
		long oldest = NONE;
		long oldest_time = 0x7fffffff;
		s_clump_activity_view *clump;
		while ((clump = (s_clump_activity_view *)clump_walk_advance_269040(&walk)) != NULL)
		{
			if (clump->active_time < oldest_time)
			{
				oldest_time = clump->active_time;
				oldest = walk.pool.datum_index;
			}
		}
		if (oldest != NONE)
		{
			function_2691b0(oldest);
			result = record_pool_allocate(g_502420);
		}
	}
	if (result != NONE)
	{
		s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (result & 0xffff) * sizeof(s_clump_activity_view));
		clump->center = *center;
		clump->team = team;
		clump->count = 0;
		clump->first_actor = NONE;
		clump->first_prop = NONE;
		clump->active = true;
		*(short *)((byte *)clump + 0x1e) = 0;
		clump->active_time = g_510c54->game_time;
		clump->state = 0;
		clump->state_time = g_510c54->game_time;
		*(bool *)((byte *)clump + 0x30) = false;
		*(long *)((byte *)clump + 0x34) = NONE;
		clump->time38 = NONE;
		*(bool *)((byte *)clump + 0x3c) = false;
		*(real *)((byte *)clump + 0x2c) = 50.0f;
		clump->update_state = 1;
		*(short *)((byte *)clump + 0x3e) = 0;
		*(long *)((byte *)clump + 0x4c) = NONE;
	}
	return result;
}

// @retail 0x26a4f0
long function_26a4f0(s_clump_activity_view *clump, short priority)
{
	long result = NONE;
	if (priority >= clump->update_state)
	{
		result = record_pool_allocate(g_50241c);
		if (result == NONE)
		{
			long time = g_510c54->game_time;
			short removed = 0;
			long selected;
			do
			{
				s_clump_pool_iterator iterator;
				real best_score = 0.0f;
				selected = NONE;
				if (g_4f55d0->active)
				{
					iterator.pool.data = g_502420;
					iterator.pool.index = NONE;
				}
				for (;;)
				{
					iterator.current = NULL;
					if (g_4f55d0->active)
						iterator.current = (s_clump *)data_iterator_next_inlined(&iterator.pool);
					s_clump_activity_view *other = (s_clump_activity_view *)iterator.current;
					if (!other) break;
					if (other->update_state >= 4 || (other == clump && priority <= other->update_state + 1))
						continue;
					real score;
					if (!other->active)
					{
						score = (real)(time - other->active_time) * g_510c54->rate * 0.0033333334140479565f;
						if (score < 0.0f) score = 0.0f;
						else if (score > 1.0f) score = 1.0f;
						score = score * 10.100000381469727f + 8.0f;
					}
					else
					{
						score = (real)(3 - other->state) * 0.3333333432674408f;
						if (score < 0.0f) score = 0.0f;
						else if (score > 1.0f) score = 1.0f;
						score *= 2.0f;
					}
					score = (real)(3 - other->update_state) * 5.0f + score;
					if (score > best_score)
					{
						best_score = score;
						selected = iterator.pool.datum_index;
					}
				}
				if (selected != NONE)
				{
					s_clump_activity_view *other = (s_clump_activity_view *)(g_502420->data + (selected & 0xffff) * sizeof(s_clump_activity_view));
					other->update_state++;
					removed = function_26a480(selected, other->update_state);
				}
			} while (selected != NONE && removed <= 0);
			if (removed > 0) result = record_pool_allocate(g_50241c);
		}
	}
	return result;
}

// @retail 0x26a780
long function_26a780(s_clump_activity_view *clump, s_prop_candidate_view const *candidate)
{
	long result = function_26a4f0(clump, candidate->priority);
	if (result != NONE)
	{
		s_prop_copy_view *prop = (s_prop_copy_view *)(g_50241c->data + (result & 0xffff) * sizeof(s_prop_copy_view));
		function_26a310(prop, candidate->type, candidate->object_index, (s_clump *)clump, candidate->priority);
	}
	return result;
}

// @retail 0x26a860
long function_26a860(s_clump_activity_view *clump, s_prop_copy_view const *source)
{
	s_prop_copy_view copy = *source;
	long result = function_26a4f0(clump, source->field0c);
	if (result != NONE)
	{
		s_prop_copy_view *prop = (s_prop_copy_view *)(g_50241c->data + (result & 0xffff) * sizeof(s_prop_copy_view));
		function_26a310(prop, copy.type, copy.object_index, (s_clump *)clump, copy.field0c);
		function_26a7d0(prop, &copy);
	}
	return result;
}

// @retail 0x26a740
long function_26a740(long object_index, s_clump_activity_view *clump)
{
	s_prop_candidate_view candidate;
	g_4de2fc++;
	g_4de2f8 = true;
	function_269e60(object_index, clump, NONE, &candidate, true);
	g_4de2f8 = false;
	return function_26a780(clump, &candidate);
}

// score 128 (was 192): frame 0xd8; entry generation scheduling, child-next register and count/end traversal differ; fresh destination head 171f; short plus fresh head 167 plain; short/end 216f; typed object views 192f; delayed source base 219f; source first 211f; masked slots 198f.
// Round 280: remaining-count traversal 128 plain; current-child handle 164; collection write cursor 172; array scope 167; early active flag 199.
// Child/countdown 125; short remaining 125; unsigned remaining 125 (each only three below the retained countdown).
// @retail 0x269580
void function_269580(long destination_index, long source_index)
{
	g_4de2fc++;
	s_clump_activity_view *destination = (s_clump_activity_view *)(g_502420->data + (destination_index & 0xffff) * sizeof(s_clump_activity_view));
	s_clump_activity_view *source = (s_clump_activity_view *)(g_502420->data + (source_index & 0xffff) * sizeof(s_clump_activity_view));
	g_4de2f8 = true;
	s_iterator iterator;
	iterator.next = ((s_clump_activity_view *)(g_502420->data + (destination_index & 0xffff) * sizeof(s_clump_activity_view)))->first_prop;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		long object_index = *(long *)((byte *)prop + 8);
		byte *object = (byte *)function_badc0(object_index, (dword)NONE);
		if (!object) continue;
		if (*(char *)(object + 0xaa) == 12 && *(long *)(object + 0x134) == 0)
		{
			byte *extra = object + *(short *)(object + 0x13a);
			if (extra && *(long *)(extra + 8) != NONE)
			{
				long index = perception_get(*(long *)(extra + 8))->object_index;
				while (index != NONE)
				{
					byte *child = (byte *)object_get(index);
					long next = NONE;
					if (*(long *)(child + 0x134) == 0)
					{
						byte *child_extra = child + *(short *)(child + 0x13a);
						if (child_extra) next = *(long *)(child_extra + 0xc);
					}
					if (g_4de300[index & 0xffff] != g_4de2fc)
						g_4de300[index & 0xffff] = g_4de2fc;
					index = next;
				}
				continue;
			}
		}
		if (g_4de300[object_index & 0xffff] != g_4de2fc)
			g_4de300[object_index & 0xffff] = g_4de2fc;
	}
	long indices[50];
	short count = 0;
	iterator.next = ((s_clump_activity_view *)(g_502420->data + (source_index & 0xffff) * sizeof(s_clump_activity_view)))->first_prop;
	while (clump_next_prop(&iterator)) indices[count++] = iterator.index;
	long *cursor = indices;
	for (long remaining = count; remaining > 0; --remaining)
	{
		long index = *cursor;
		++cursor;
		if (index == NONE || (index & 0xffff) >= g_50241c->high_water_index) continue;
		s_prop_copy_view *copy = (s_prop_copy_view *)(g_50241c->data + g_50241c->size * (index & 0xffff));
		if (!copy->salt || copy->salt != (index >> 16)) continue;
		long object_index = copy->object_index;
		if (!function_badc0(object_index, (dword)NONE)) continue;
		if (g_4de300[object_index & 0xffff] == g_4de2fc) continue;
		g_4de300[object_index & 0xffff] = g_4de2fc;
		long added = function_26a860(destination, copy);
		if (added != NONE) function_26a960((s_clump *)destination, added);
	}
	g_4de2f8 = false;
	if (*(real *)((byte *)source + 0x2c) > *(real *)((byte *)destination + 0x2c))
		*(real *)((byte *)destination + 0x2c) = *(real *)((byte *)source + 0x2c);
	*(byte *)((byte *)destination + 0x30) |= *(byte *)((byte *)source + 0x30);
	if (*(long *)((byte *)source + 0x34) > *(long *)((byte *)destination + 0x34))
		*(long *)((byte *)destination + 0x34) = *(long *)((byte *)source + 0x34);
	if (source->state > destination->state)
	{
		destination->state = source->state;
		destination->state_time = source->state_time;
	}
}

// @retail 0x26ace0
long function_26ace0(long object_index, long actor_index, short type)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;
	if (actor->unknown07c != NONE)
	{
		s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (actor->unknown07c & 0xffff) * sizeof(s_clump_activity_view));
		long prop_index = function_26b120(actor->unknown07c, object_index);
		if (prop_index == NONE)
		{
			prop_index = function_26a740(object_index, clump);
			if (prop_index != NONE)
			{
				((s_prop_copy_view *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_prop_copy_view)))->field0c = type;
				function_26a960((s_clump *)clump, prop_index);
			}
			else
			{
				return NONE;
			}
		}
		s_iterator nodes;
		nodes.next = ((s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop)))->first_node;
		s_clump_node *node;
		while ((node = clump_next_node(&nodes)) != NULL)
		{
			if (node->actor_index == actor_index)
			{
				result = nodes.index;
				break;
			}
		}
	}
	return result;
}

void function_269250(long clump_index, long actor_index);

// score 134 (was 134): frame 0x38 versus 0x3c; pool register and common null-current join differ.
// Double candidate/both scores 157/158; activity while 160; role-based scalar names 134; inline depth two 134.
// @retail 0x2689d0
void function_2689d0(void)
{
	s_actor_iterator actors;
	function_x66da2b(&actors, true);
	s_actor_view *actor;
	while ((actor = (s_actor_view *)function_1e46c0(&actors)) != NULL)
	{
		s_clump_pool_iterator groups;
		if (g_4f55d0->active)
		{
			groups.pool.data = g_502420;
			groups.pool.index = NONE;
		}
		long selected = NONE;
		real best = 0.0f;
		for (;;)
		{
			s_clump_activity_view *group = NULL;
			if (g_4f55d0->active)
				group = (s_clump_activity_view *)data_iterator_next_inlined(&groups.pool);
			groups.current = (s_clump *)group;
			if (!group) break;
			if (group->team != actor->unknown024) continue;
			real score = function_268bb0(group, actor);
			if (score > best)
			{
				best = score;
				selected = groups.pool.datum_index;
			}
		}
		if (selected != NONE)
		{
			if (selected != actor->unknown07c)
			{
				if (actor->unknown07c != NONE)
				{
					function_269580(selected, actor->unknown07c);
					function_2694d0(actor->unknown07c, actors.actor_index);
				}
				function_269250(selected, actors.actor_index);
			}
		}
		else
		{
			long created = function_269040(&actor->position, actor->unknown024);
			if (created != NONE)
			{
				if (actor->unknown07c != NONE)
				{
					s_clump_activity_view *group = (s_clump_activity_view *)(g_502420->data + (created & 0xffff) * sizeof(s_clump_activity_view));
					group->center = actor->position;
					function_269580(created, actor->unknown07c);
					function_2694d0(actor->unknown07c, actors.actor_index);
				}
				function_269250(created, actors.actor_index);
			}
		}
	}
}

// score 5 (was 5): node/prop next homes 0x1c/0x14 versus 0x18/0x1c; setup scope 5; typed prop 5; both 5;
// link references 5; array views 5; prop names 5; record references 5; joint declarations 5.
// @retail 0x269250
void function_269250(long clump_index, long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->count++;
	function_26b1d0(actor_index);
	actor->unknown07c = clump_index;
	actor->next_index = clump->first_actor;
	clump->first_actor = actor_index;
	if (actor->unknown009 && !clump->active)
	{
		s_clump_activity_view *updated = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
		updated->active = true;
		updated->update_state = 1;
	}
	if (*(bool *)((byte *)actor + 0xc))
		*(bool *)((byte *)clump + 0x3c) = true;
	g_4de2fc++;
	g_4de2f8 = true;
	s_iterator nodes;
	nodes.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = clump_member_prop_next(&nodes)) != NULL)
	{
		long object_slot = node->object_index & 0xffff;
		if (g_4de300[object_slot] != g_4de2fc)
			g_4de300[object_slot] = g_4de2fc;
		long prop_index = function_26b120(clump_index, node->object_index);
		if (prop_index != NONE)
		{
			s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
			s_clump_link_view *link = (s_clump_link_view *)prop_ref_get(nodes.index);
			long first = prop->first_node;
			if (first != NONE)
				((s_clump_link_view *)prop_ref_get(first))->previous = nodes.index;
			link->next = first;
			prop->first_node = nodes.index;
			link->prop_index = prop_index;
		}
		else
		{
			function_26abd0((s_clump_link_view *)node);
			if (node->view_index != NONE) record_pool_release(g_502414, node->view_index);
			record_pool_release(g_502418, nodes.index);
		}
	}
	s_iterator props;
	props.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&props)) != NULL)
	{
		long slot = *(long *)((byte *)prop + 8) & 0xffff;
		if (g_4de300[slot] != g_4de2fc)
		{
			g_4de300[slot] = g_4de2fc;
			long index = function_26a8d0(props.index);
			if (index != NONE)
			{
				s_clump_prop *current = (s_clump_prop *)(g_50241c->data + (props.index & 0xffff) * sizeof(s_clump_prop));
				s_clump_link_view *link = (s_clump_link_view *)prop_ref_get(index);
				long first = current->first_node;
				if (first != NONE)
					((s_clump_link_view *)prop_ref_get(first))->previous = index;
				link->next = first;
				current->first_node = index;
				link->prop_index = props.index;
				function_26aa40(actor_index, index);
			}
		}
	}
	g_4de2f8 = false;
}

// score 189 (was 189): frame 0x1c versus 0x24; current/replacement/object homes and iterator exit differ.
// Byte type 195; membership result earlier 189; existing group lookup 189; removal arm first 189; guarded pointer-result loop 194.
// Complete workspace scope 189; depth zero 391; member-link result first 222; replacement-result loop 206; promoted category 195.
// Depth one 189; outer datum API 290; word unit 218; word group 192; calling scan 237.
// Direct pool step 254; activity-controlled loop 194; existing prop lookup 189; short/signed byte change 244/189; cached-bound scan 189.
// @retail 0x26ae30
void function_26ae30(long object_index)
{
	bool unit = false;
	bool group_object = false;
	s_slot_object_view *object = object_get(object_index);
	short type = (char)object->type;
	long replacement = NONE;
	if (type == 12)
	{
		if (*(long *)((byte *)object + 0x134) == 0)
		{
			byte *data = (byte *)object + *(short *)((byte *)object + 0x13a);
			if (data)
			{
				long perception = *(long *)(data + 8);
				group_object = true;
				if (perception != NONE)
				{
					long index = perception_get(perception)->object_index;
					for (;;)
					{
						if (index == NONE) break;
						s_slot_object_view *member = object_get(index);
						long current = index;
						byte *member_extension = NULL;
						if (*(long *)((byte *)member + 0x134) == 0)
							member_extension = (byte *)member + *(short *)((byte *)member + 0x13a);
						index = member_extension ? *(long *)(member_extension + 0xc) : NONE;
						if (current != object_index)
						{
							replacement = current;
							break;
						}
					}
				}
			}
		}
	}
	else
		unit = ((1 << type) & 3) != 0;
	s_clump_pool_iterator groups;
	if (g_4f55d0->active)
	{
		groups.pool.data = g_502420;
		groups.pool.index = NONE;
	}
	for (;;)
	{
		groups.current = NULL;
		if (g_4f55d0->active)
			groups.current = (s_clump *)data_iterator_next_inlined(&groups.pool);
		if (!groups.current) break;
		bool changed = false;
		s_iterator props;
		props.next = ((s_clump *)(g_502420->data + (groups.pool.datum_index & 0xffff) * sizeof(s_clump)))->first_prop;
		s_type_76cf92 *prop;
		while ((prop = clump_next_prop(&props)) != NULL)
		{
			if (*(long *)((byte *)prop + 8) == object_index)
			{
				changed = true;
				if (replacement != NONE)
					*(long *)((byte *)prop + 8) = replacement;
				else
				{
					function_26aaf0(groups.current, props.index);
					if (!unit) break;
					continue;
				}
			}
			else if (prop->state.unknown3c == object_index)
			{
				prop->state.unknown3c = NONE;
				prop->state.unknown66 = false;
				prop->state.unknown65 = false;
			}
			if (group_object && prop->unknown22)
			{
				s_iterator nodes;
				nodes.next = ((s_clump_prop *)(g_50241c->data + (props.index & 0xffff) * sizeof(s_clump_prop)))->first_node;
				s_clump_node *node;
				while ((node = clump_next_node(&nodes)) != NULL)
				{
					if (((s_prop_datum *)node)->object_index == object_index)
					{
						((s_prop_datum *)node)->object_index = replacement;
						changed = true;
					}
				}
			}
			if (changed && !unit) break;
		}
	}
	if (object->type == 1)
	{
		for (long index = data_next_absolute_index_inlined(g_502414, 0);
			index != NONE;
			index = data_next_absolute_index_inlined(g_502414, index + 1))
		{
			byte *tracking = g_502414->data + g_502414->size * index;
			if (!tracking) break;
			if (*(long *)(tracking + 0x40) == object_index)
				*(long *)(tracking + 0x40) = NONE;
		}
	}
}

struct s_2640c0;
bool function_25ccd0(s_type_5cfb45 *state, long object_index, short value, s_2640c0 *motion);

// @retail 0x26a210
void function_26a210(long clump_index)
{
    s_iterator iterator;
    iterator.next = clump_get(clump_index)->first_prop;
    s_type_76cf92 *prop;
    while ((prop = clump_next_prop(&iterator)) != NULL)
    {
        long object_index = *(long *)((byte *)prop + 8);
        s_object *object = function_badc0(object_index, 3);
        if (object)
        {
            prop->unknown26[0] = (bool)((*(dword *)((byte *)object + 0x134) >> 7) & 1);
            if (prop->unknown23)
                *((bool *)prop + 0xbf) = (bool)((*(dword *)((byte *)object + 0x134) >> 8) & 1);
        }
        if (g_470f10[*(short *)((byte *)prop + 2)].kind == 0 && prop->unknown04 == 1)
            function_25ccd0(&prop->state, object_index, NONE, NULL);
    }
}

int __cdecl function_269da0(void const *a, void const *b);
void *actor_get_character_situation_entry(long actor_index);

// score 322 (was 322): frame 0x6a4 agrees; reset-word/cursor allocation and selection/cleanup layout differ; typed cluster view 322f; isolated cluster iterator 322f; unsigned cursor 328f; unsigned selected 322f; case-result assignment 391f; existing first 347f; indexed cleanup 351f.
// Round 280: full-width reset snapshot 373; current-child handle 322; late cursor 350; late selected total 335; ascending word merge 374.
// @retail 0x269870
void function_269870(long clump_index)
{
    s_clump_activity_view *clump = (s_clump_activity_view *)clump_get(clump_index);
    byte *structure = (byte *)g_4e0348;
    short count = 0, cursor = 0, selected = 0;
    real distance = 50.0f;
    s_iterator actors;
    actors.next = clump->first_actor;
    while (clump_next_actor(&actors) != NULL)
    {
        byte *settings = (byte *)actor_get_character_situation_entry(actors.index);
        if (settings && *(real *)(settings + 4) > distance)
            distance = *(real *)(settings + 4);
    }
    *(real *)((byte *)clump + 0x2c) = distance;
    dword clusters[16];
    memset(clusters, 0, ((*(long *)(structure + 0x9c) + 31) >> 5) * 4);
    actors.next = clump_get(clump_index)->first_object;
    s_actor_view *actor;
    while ((actor = clump_next_actor(&actors)) != NULL)
    {
        short cluster = *(short *)((byte *)actor + 0x254);
        if (cluster >= 0 && cluster < *(long *)(structure + 0x9c))
        {
            long word_count = (*(long *)(structure + 0x9c) + 31) >> 5;
            dword *visible = *(dword **)(structure + 0x58) + word_count * cluster;
            for (long word = word_count - 1; word >= 0; --word)
                clusters[word] |= visible[word];
        }
        else
        {
            *(long *)((byte *)actor + 0x250) = NONE;
            *(short *)((byte *)actor + 0x254) = NONE;
            *(short *)((byte *)actor + 0x256) = g_4686c4;
        }
    }
    ++g_4de2fc;
    g_4de2f8 = true;
    actors.next = clump_get(clump_index)->first_object;
    while ((actor = clump_next_actor(&actors)) != NULL)
    {
        if (*((bool *)actor + 7))
        {
            long object_index = perception_get(*(long *)((byte *)actor + 0x1c))->object_index;
            while (object_index != NONE)
            {
                s_handler_object_view *object = handler_object_get(object_index);
                long next = NONE;
                if (!object->flags134)
                {
                    byte *owner = (byte *)object + object->ai_offset;
                    if (owner)
                        next = *(long *)(owner + 0xc);
                }
                if (g_4de300[object_index & 0xffff] != g_4de2fc)
                    g_4de300[object_index & 0xffff] = g_4de2fc;
                object_index = next;
            }
        }
        else
        {
            long object_index = actor->unknown018;
            if (g_4de300[object_index & 0xffff] != g_4de2fc)
                g_4de300[object_index & 0xffff] = g_4de2fc;
        }
    }
    s_prop_candidate_view candidates[100];
    s_iterator props;
    props.next = clump_get(clump_index)->first_prop;
    s_type_76cf92 *prop;
    while ((prop = clump_next_prop(&props)) != NULL)
    {
        if (function_269e60(*(long *)((byte *)prop + 8), clump, props.index, &candidates[count], false) < clump->update_state)
            function_26aaf0((s_clump *)clump, props.index);
        else
            ++count;
    }
    s_type_f1af8e iterator;
    iterator.signature = 0x86868686;
    iterator.type_mask = 0x1023;
    iterator.flags = 0;
    iterator.index = 0;
    iterator.object_index = NONE;
    s_slot_object_view *object;
    while ((object = (s_slot_object_view *)function_baeb0(&iterator)) != NULL)
    {
        short cluster = *(short *)((byte *)object + 0x2c);
        if (clusters[cluster >> 5] & (1 << (cluster & 31)))
            function_269de0(iterator.object_index, clump, candidates, &count);
    }
    g_4de2f8 = false;
    qsort(candidates, count, sizeof(*candidates), function_269da0);
    s_prop_candidate_view *candidate = candidates;
    while (cursor < count)
    {
        if (candidate->priority < 3 && selected >= 20)
            break;
        if (candidate->prop_index == NONE)
        {
            long index = function_26a4f0(clump, candidate->priority);
            if (index != NONE)
            {
                s_prop_copy_view *new_prop = (s_prop_copy_view *)prop_get(index);
                function_26a310(new_prop, candidate->type, candidate->object_index, (s_clump *)clump, candidate->priority);
                function_26a960((s_clump *)clump, index);
                ++selected;
            }
        }
        else
        {
            s_prop_copy_view *existing = (s_prop_copy_view *)datum_get_inlined(g_50241c, candidate->prop_index);
            if (existing)
            {
                existing->field0c = candidate->priority;
                if (existing->type != candidate->type)
                {
                    s_type_5cfb45 *state = NULL;
                    switch (g_470f10[candidate->type].unknown4)
                    {
                    case 0:
                        break;
                    case 1:
                        {
                            s_iterator nodes;
                            nodes.next = ((s_clump_prop *)prop_get(candidate->prop_index))->first_node;
                            s_clump_node *node;
                            while ((node = clump_next_node(&nodes)) != NULL)
                            {
                                if (node->state >= 1)
                                {
                                    if (*(long *)((byte *)node + 0x14) != NONE)
                                        state = function_25d690((s_prop_datum *)node);
                                    break;
                                }
                            }
                            if (!node)
                                goto type_ready;
                        }
                        break;
                    default:
                        goto type_ready;
                    }
                    existing->state = 1;
                    if (state)
                        existing->observed = *state;
                    else
                        function_25ccd0(&existing->observed, existing->object_index, NONE, NULL);
                type_ready:
                    existing->type = candidate->type;
                }
                ++selected;
            }
        }
        ++cursor;
        ++candidate;
        if (selected >= 50)
            break;
    }
    while (cursor < count)
    {
        long index = candidate->prop_index;
        if (index != NONE && datum_get_inlined(g_50241c, index))
            function_26aaf0((s_clump *)clump, index);
        ++cursor;
        ++candidate;
    }
}

// @retail 0x269da0
int __cdecl function_269da0(void const *first, void const *second)
{
    s_prop_candidate_view const *a = (s_prop_candidate_view const *)first;
    s_prop_candidate_view const *b = (s_prop_candidate_view const *)second;
    if (a->priority > b->priority)
        return -1;
    if (a->priority >= b->priority && a->prop_index != NONE && b->prop_index == NONE)
        return -1;
    return 1;
}
