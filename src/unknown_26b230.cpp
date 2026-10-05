// @flags /O2 /Gr /arch:SSE2
#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "unknown_20f040.h"


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

// @retail 0x26b290
void function_26b290(long clump_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	real a[13];
	real b[13];
	long object_index = clump->first_object;
	long i;

	for (i = 0; i < 13; i++)
	{
		a[i] = 0.0f;
		b[i] = 0.0f;
	}

	for (;;)
	{
		if (object_index == NONE)
		{
			break;
		}
		s_clump_object *object = (s_clump_object *)(g_4f55f0->data + (object_index & 0xffff) * sizeof(s_clump_object));
		object_index = object->next;
		short count = object->count;

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

	short state;
	short new_state = NONE;
	for (state = 3; clump->state < state; state--)
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
	if ((real)(g_510c54->game_time - clump->state_time) * g_510c54->rate > 1.0f)
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
	return a[3] > 0.5;
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

PRIVATE __forceinline s_clump *next_clump_in_pool(s_record_pool_iterator *iterator)
{
	s_clump *result = NULL;
	if (g_4f55d0->active)
		result = (s_clump *)data_iterator_next_calling(iterator);
	return result;
}

// @retail 0x26b900
long function_26b900(long prop_index, long actor_index, long clump_index)
{
	bool result = function_26b8a0(prop_index, actor_index);

	if (!result)
	{
		s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
		short side = (short)function_20f040(clump->team);
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
		s_record_pool_iterator iterator;
		s_clump *other;

		if (g_4f55d0->active)
		{
			iterator.data = g_502420;
			iterator.index = NONE;
		}

		while ((other = next_clump_in_pool(&iterator)) != NULL)
		{
			if (clump_index != iterator.datum_index && (short)function_20f040(other->team) == side)
			{
				long other_prop_index = function_26b230(iterator.datum_index, prop->type);
				if (other_prop_index != NONE && function_26b8a0(other_prop_index, NONE))
					return true;
			}
		}
	}

	return result;
}

// @retail 0x26ba60
bool function_26ba60(long prop_index, long actor_index, long clump_index)
{
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
	bool result = false;

	if (prop->unknown34)
		return result;

	if (TEST_FIELD_BIT(((s_clump_prop_object_header *)g_4e0300->data)[prop->type & 0xffff].object->flag10a_2))
		return result;

	if (clump_index == NONE)
		return result;

	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	if (!team_is_enemy(clump->team, 1))
	{
		s_game_time_globals *game_time_globals = g_510c54;
		long game_time = game_time_globals->game_time;
		s_data_datum_iterator players;
		bool ready = true;

		players.data = g_4e8c24;
		players.index = NONE;
		players.datum_index = NONE;
		while (data_datum_iterator_next(&players))
		{
			s_ai_player *player = ai_player_get(players.datum_index);
			if (player && game_time - player->unknown0c < game_time_globals->field_2_3)
				ready = false;
		}

		if (ready)
		{
			s_iterator actors;

			function_26bda0(clump_index, &actors);
			while (actors.next != NONE)
			{
				s_clump_object *actor = (s_clump_object *)(g_4f55f0->data + (actors.next & 0xffff) * sizeof(s_clump_object));
				actors.next = actor->next;
				if (actor->node_index != NONE)
				{
					s_clump_node *node = (s_clump_node *)(g_502418->data + (actor->node_index & 0xffff) * sizeof(s_clump_node));
					if (node->state < 1 || node->state > 2)
						continue;

					return false;
				}
			}
		}

		return ready;
	}

	return !function_26b900(prop_index, actor_index, clump_index);
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
	byte unknown00[8];
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
		iterator.next = clump->first_actor;
		s_actor_view *actor;
		while ((actor = clump_next_actor(&iterator)) != NULL)
		{
			if (actor->unknown009)
			{
				clump->active_time = g_510c54->game_time;
				return;
			}
		}
		clump->active = false;
		clump->active_time = g_510c54->game_time - 1;
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

// @retail 0x26add0
bool function_26add0(long prop_index)
{
	bool result = false;
	s_clump_prop *prop = &((s_clump_prop *)g_50241c->data)[prop_index & 0xffff];
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
	if (a[2] == 0.5)
		result = 1;
	else if (a[5] == 0.0f)
		result = 2;
	return result;
}

// @retail 0x26b750
bool __stdcall function_26b750(long clump_index, real *b, real *a)
{
	return a[5] > 0.5;
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
