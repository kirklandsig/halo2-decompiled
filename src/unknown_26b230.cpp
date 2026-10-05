// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "props.h"
#include <float.h>
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

// @retail 0x26b900
long function_26b900(long prop_index, long actor_index, long clump_index)
{
	bool result = function_26b8a0(prop_index, actor_index);
	volatile bool saved_result = result;

	if (!result)
	{
		s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
		s_clump *const *clump_reference = &clump;
		long team = ((s_clump volatile *)*clump_reference)->team;
		long side = function_20f040((short)team);
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
		s_clump_pool_iterator iterator;
		s_clump *other;

		if (g_4f55d0->active)
		{
			iterator.pool.data = g_502420;
			iterator.pool.index = NONE;
		}

		while (next_clump_in_pool(&iterator))
		{
			other = iterator.current;
			if (clump_index != iterator.pool.datum_index && (short)side == (short)function_20f040(other->team))
			{
				long other_prop_index = function_26b230(iterator.pool.datum_index, prop->type);
				if (other_prop_index != NONE)
				{
					if (function_26b8a0(other_prop_index, NONE))
					{
						result = true;
						goto done;
					}
					result = saved_result;
				}
			}
		}
	}

done:
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


struct s_prop_notice_view
{
	short fields[10];
};

struct s_prop_copy_view
{
	short salt;
	short type;
	short state;
	byte unknown06[0x28 - 6];
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
	if (object->type == 0xc && object->owner_kind == 0)
	{
		byte *owner = (byte *)object + object->owner_offset;
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
long function_26b120(long object_index, long clump_index)
{
	long result = NONE;
	long actor_index = clump_object_actor_index(object_index);
	s_iterator iterator;
	iterator.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		if (*(long *)((byte *)prop + 8) == object_index)
		{
			result = iterator.index;
			break;
		}
		if (prop->unknown22 && actor_index != NONE && actor_index == prop->actor_index)
			return iterator.index;
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
