// @flags /O2 /arch:SSE /Gr
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1a58b0.h"
#include "object_markers.h"

/* ---- views of the data used below ---- */

struct s_object_view
{
	long definition_index;
	dword flags4;
	byte unknown08[0x2b0 - 8];
	real value2b0;
};

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object_view *object;
};

struct s_player_view
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_unit_view
{
	byte unknown00[0x14];
	long unknown14;
	byte unknown18[0x1fc - 0x18];
	short unknown1fc;
};

struct s_definition_view
{
	byte unknown00[0x86];
	byte unknown86;
	byte unknown87;
	long tag_index88;
	byte unknown8c[0xd4 - 0x8c];
};

struct s_sort_element
{
	long identifier;
	byte unknown04[0x24];
	byte unknown28;
	byte unknown29[0x1b];
	real value44;
	real value48;
	real value4c;
	real value50;
	real value54;
	byte flags58;
};

#define OBJECT_HEADER(index) (&((s_object_header *)g_4e0300->data)[(index) & 0xffff])


/* ---- callees ---- */

void function_1c2330(long bit_count, dword *a, dword *b, dword *result);

/* ---- globals ---- */

s_candidate_table g_4ee4f0[3];
byte g_4f09c8[0x84];
dword g_4f0a4c[5];
s_sort_globals *g_51e99c;
s_flag_bits g_557c74;

/* ---- helpers ---- */

#define ACTION_ID_VALID(id) ((id) >= 0 && (id) < 0x83)

/* whether the handler is enabled in this build */
#define ACTION_HANDLER_ENABLED(id) \
	(g_46eeb8[id]->unknown8 != g_46f348 && (g_46eeb8[id]->mask & g_4ee4ec) == g_4ee4ec && (g_557c40[(id) >> 5] & (1 << ((id) & 0x1f))))

/* link a node in after an element */
#define ACTION_NODE_INSERT(element, node) \
	do \
	{ \
		(node)->next = (element)->next; \
		(element)->next = (node); \
		(node)->order = 0; \
	} \
	while (0)

/* link a node into the list of an element, after the nodes of a lower order */
#define ACTION_NODE_INSERT_SORTED(element, node, order_value) \
	do \
	{ \
		s_action_node *previous = (element); \
		\
		if (!previous->next) \
		{ \
			(node)->next = 0; \
			previous->next = (node); \
		} \
		else \
		{ \
			do \
			{ \
				if (!previous->next || previous->next->order >= (order_value)) \
				{ \
					(node)->next = previous->next; \
					previous->next = (node); \
					break; \
				} \
				previous = previous->next; \
			} \
			while (previous); \
		} \
		(node)->order = (order_value); \
	} \
	while (0)
/* ---- functions ---- */

// @retail 0x1a58b0
bool function_1a58b0(long object_index)
{
	s_object_header *header = (s_object_header *)(g_4e0300->data + (object_index & 0xffff) * sizeof(s_object_header));
	s_object_view *object = header->object;

	if (((1 << header->type) & 0x83) && !(header->flags & 0x10) && !(object->flags4 & 1))
	{
		if (!((1 << header->type) & 3) || 1.0f > object->value2b0)
			return true;
	}
	return false;
}
void __stdcall function_df6c0(point3f *bottom, vector3f *axis, long object_index, real *radius);
void function_1a6340(vector3f const *a, vector3f const *b, point3f *out, point3f const *c, point3f const *p, real radius);

// @retail 0x1a62e0
bool function_1a62e0(long object_index, point3f *out, vector3f const *direction, point3f const *origin)
{
	bool result = false;
	real radius;
	point3f bottom;
	vector3f axis;

	function_df6c0(&bottom, &axis, object_index, &radius);
	if (radius > 0.0001f)
	{
		function_1a6340(&axis, direction, out, &bottom, origin, radius);
		result = true;
	}
	return result;
}

PRIVATE inline vector3f *local_scale_vector(vector3f const *input, double scale, vector3f *output);

// @retail 0x1a6340
void function_1a6340(vector3f const *a, vector3f const *b, point3f *out, point3f const *c, point3f const *p, real radius)
{
	real ni = a->k * b->j - b->k * a->j;
	real nj = b->k * a->i - b->i * a->k;
	real nk = b->i * a->j - a->i * b->j;
	real m = nj * nj;
	m += nk * nk;
	m += ni * ni;

	if (m > 0.0f)
	{
		real dz = c->z - p->z;
		real dy = c->y - p->y;
		real dx = c->x - p->x;
		real ci = a->k * dy - dz * a->j;
		real cj = a->i * dz - a->k * dx;
		real ck = dx * a->j - dy * a->i;
		real t = cj * nj;
		t += ck * nk;
		t += ci * ni;
		t /= m;

		t = 0.0f > t ? 0.0f : t > 1.0f ? 1.0f : t;
		out->x = b->i * t + p->x;
		out->y = t * b->j + p->y;
		out->z = b->k * t + p->z;
	}
	else
	{
		*out = *p;
	}

	real vx = out->x - c->x;
	real vz = out->z - c->z;
	real vy = out->y - c->y;
	real dot = a->k * vz;
	dot += a->i * vx;
	dot += vy * a->j;
	real s = 0.0f - dot;
	vector3f w;

	w.i = a->i * s + vx;
	w.j = s * a->j + vy;
	w.k = a->k * s + vz;
	real wm = w.k * w.k + w.j * w.j + w.i * w.i;

	if (wm > radius * radius)
	{
		local_scale_vector(&w, radius / sqrt(wm), &w);
	}
	out->x = out->x - w.i;
	out->y = out->y - w.j;
	out->z = out->z - w.k;
}

PRIVATE inline vector3f *local_scale_vector(vector3f const *input, double scale, vector3f *output)
{
	output->i = scale * input->i;
	output->j = scale * input->j;
	output->k = scale * input->k;
	return output;
}

// @retail 0x1a65b0
void function_1a65b0(vector3f const *a, point3f *out, point3f const *p, point3f const *c, real radius)
{
	*out = *p;

	vector3f delta;
	vector3d_from_points3d(c, out, &delta);
	real dot = a->j * delta.j;
	dot += a->i * delta.i;
	dot += a->k * delta.k;
	real s = 0.0f - dot;
	vector3f w;

	w.i = a->i * s + delta.i;
	w.j = a->j * s + delta.j;
	w.k = a->k * s + delta.k;
	real wm = w.k * w.k + w.j * w.j + w.i * w.i;

	if (wm > radius * radius)
	{
		local_scale_vector(&w, radius / sqrt(wm), &w);
	}
	out->x = out->x - w.i;
	out->y = out->y - w.j;
	out->z = out->z - w.k;
}
// @retail 0x1a66f0
int __cdecl function_1a66f0(s_sort_element const *a, s_sort_element const *b)
{
	real score_a = a->value4c;
	real score_b = b->value4c;

	if (g_51e99c)
	{
		dword flags = g_51e99c->flags;

		if (flags & 0x20)
		{
			if (!(a->flags58 & 1))
				score_a -= 0.5f;
			if (!(b->flags58 & 1))
				score_b -= 0.5f;
		}
		if (flags & 0x10)
		{
			if (!(a->flags58 & 2))
				score_a -= 0.5f;
			if (!(b->flags58 & 2))
				score_b -= 0.5f;
		}
	}

	if (score_a > score_b)
		return -1;
	if (score_b > score_a)
		return 1;
	if (a->value50 > b->value50)
		return -1;
	if (b->value50 > a->value50)
		return 1;
	if (a->value54 > b->value54)
		return -1;
	if (b->value54 > a->value54)
		return 1;
	if (b->value44 > a->value44)
		return -1;
	if (a->value44 > b->value44)
		return 1;
	if (b->value48 > a->value48)
		return -1;
	if (a->value48 > b->value48)
		return 1;
	if (!a->unknown28 && b->unknown28)
		return -1;
	if (a->unknown28 && !b->unknown28)
		return 1;
	return (a->identifier & 0xffff) - (b->identifier & 0xffff);
}

// @retail 0x1a6d30
bool __fastcall function_1a6d30(long player_index)
{
	bool result = true;
	long unit_index = ((s_player_view *)g_4e8c24->data)[player_index & 0xffff].unit_index;

	if (unit_index != NONE)
	{
		s_unit_view *unit = (s_unit_view *)OBJECT_HEADER(unit_index)->object;

		if (unit->unknown14 == NONE || unit->unknown1fc == NONE)
			result = false;
	}
	return result;
}
// @retail 0x1a6ea0
short function_1a6ea0(s_action_node **out, s_candidate_list *list, s_action_node *nodes, short node_count)
{
	short current = g_46fbe4;
	short i = 0;
	short out_count = 0;
	short j = 0;
	short count = 0;

	if (list)
		count = list->count;

	do
	{
		s_candidate_entry *entry = 0;

		if (i < count)
		{
			entry = &list->entries[i];
			switch (entry->kind)
			{
			case 0:
				break;
			case 1:
				entry = 0;
				break;
			case 3:
				if (current != entry->key)
					entry = 0;
				break;
			case 2:
				if (nodes[j].key == entry->key)
					j++;
				else
					entry = 0;
				break;
			case 4:
				if (j < node_count)
					entry = 0;
				break;
			default:
				entry = 0;
				break;
			}
		}

		if (entry)
		{
			out[out_count] = &entry->node;
			i++;
		}
		else
		{
			if (j >= node_count)
				break;
			out[out_count] = &nodes[j];
			current = nodes[j].key;
			j++;
		}
		out_count++;
	}
	while (out_count < 0x32);
	return out_count;
}

// @retail 0x1a6f70
void function_1a6f70(long owner_index)
{
	s_actor_view *owner = actor_get(owner_index);
	long stamp = ++g_46f34c;
	short i = 0;

	if (owner->current >= 0)
	{
		do
		{
			short type = owner->slots[i].type;

			if (ACTION_ID_VALID(type))
				g_46eeb8[type]->unknownc = stamp;
			i++;
		}
		while (i <= owner->current);
	}
}

/* a slot and the one after it, seen from the owner base (the slots start at +0x90) */
struct s_slot_pair_view
{
	byte unknown00[0x94];
	short unknown94;
	byte unknown96[0x3a];
	s_slot next;
};

// @retail 0x1a7030
short function_1a7030(long owner_index, short slot_index, long argument, short *out)
{
	s_actor_view *owner = actor_get(owner_index);
	s_slot_pair_view *slot = (s_slot_pair_view *)((byte *)owner + slot_index * sizeof(s_slot));
	s_slot *next = &slot->next;
	short result = g_46fbe4;
	short selected = NONE;
	short type = next->type;

	if (next->state == 0 && ACTION_ID_VALID(type) && slot->unknown94 != NONE && ACTION_HANDLER_ENABLED(type))
	{
		s_slot_handler *handler = g_46eeb8[type];
		short code;

		if (handler->evaluate_argument)
			code = handler->evaluate_argument(owner_index, next, argument);
		else
			code = g_46fbe8;

		if (code == g_46fbe8)
		{
			selected = next->type;
			result = code;
		}
		else
		{
			next->state = 1;
			if (ACTION_ID_VALID(code))
			{
				if (ACTION_HANDLER_ENABLED(code))
				{
					selected = code;
					result = code;
				}
				else
				{
					slot->unknown94 = NONE;
					result = g_46fbe4;
				}
			}
			else
			{
				slot->unknown94 = NONE;
				if (code == g_46fbec)
					result = code;
				else
					result = g_46fbe4;
			}
		}
	}
	else
	{
		slot->unknown94 = NONE;
	}

	*out = selected;
	return result;
}
// @retail 0x1a71f0
bool function_1a71f0(short count, short id, s_action_node **list)
{
	if (!(g_4f0a4c[id >> 5] & (1 << (id & 0x1f))))
		return false;

	short i;

	for (i = 0; i < count; i++)
		list[i]->next = 0;

	long row;

	for (row = 0; row < g_4f09c8[id]; row++)
	{
		s_candidate_entry *entry = &g_4ee4f0[row].entries[id];
		s_action_node *node = &entry->node;

		if (entry->kind == 0)
		{
			ACTION_NODE_INSERT(list[0], node);
		}
		else if (entry->kind == 4)
		{
			ACTION_NODE_INSERT_SORTED(list[count - 1], node, 2);
		}
		else
		{
			short j = 0;

			while (j < count)
			{
				if (list[j]->key == entry->key)
					break;
				j++;
			}
			if (j < count)
			{
				s_action_node *element = list[j];

				if (entry->kind == 1)
					ACTION_NODE_INSERT(element, node);
				else if (entry->kind == 2)
					ACTION_NODE_INSERT_SORTED(element, node, 1);
				else if (entry->kind == 3)
					ACTION_NODE_INSERT_SORTED(element, node, 2);
			}
		}
	}
	return true;
}

/* where each handler's nodes start in g_4f0a60 and how many there are, by
   handler and actor type (filled by 0x1a6d80) */
struct s_action_table_entry
{
	short index;
	char count;
	byte unknown03;
};

s_action_table_entry g_4f2cc0[k_slot_type_count][0x14];
s_action_node *g_4f0a60[0x898];

/* the candidate lists of each actor type (0x14 types): the lists by slot
   type, each with its candidate entries (retail's data at 0x470c60..0x470f10) */
#define CANDIDATE(kind, key, node_key, unknown4, unknown8) { kind, key, { node_key, 1, unknown4, { 0 }, unknown8, 0, 0 } }

s_candidate_entry g_470c60[1] = { CANDIDATE(3, 0x2a, 0x78, NONE, 0.0f) };
s_candidate_entry g_470c78[1] = { CANDIDATE(0, NONE, 0x7f, NONE, 0.0f) };
s_candidate_list g_470c90[2] = { { 0x1, { 0 }, g_470c60, 1 }, { 0x6a, { 0 }, g_470c78, 1 } };
s_candidate_entry g_470ca8[1] = { CANDIDATE(3, 0x2a, 0x78, NONE, 0.0f) };
s_candidate_entry g_470cc0[1] = { CANDIDATE(0, NONE, 0x7f, NONE, 0.0f) };
s_candidate_entry g_470cd8[1] = { CANDIDATE(0, NONE, 0x82, NONE, 0.0f) };
s_candidate_list g_470cf0[3] = { { 0x1, { 0 }, g_470ca8, 1 }, { 0x6a, { 0 }, g_470cc0, 1 }, { 0xe, { 0 }, g_470cd8, 1 } };
s_candidate_entry g_470d14[1] = { CANDIDATE(3, 0x4, 0x7e, NONE, 0.0f) };
s_candidate_list g_470d2c[1] = { { 0x1, { 0 }, g_470d14, 1 } };
s_candidate_entry g_470d38[1] = { CANDIDATE(0, NONE, 0xa, -2, -1.0f) };
s_candidate_entry g_470d50[1] = { CANDIDATE(0, NONE, 0x6c, NONE, 0.0f) };
s_candidate_list g_470d68[2] = { { 0x2a, { 0 }, g_470d38, 1 }, { 0x6a, { 0 }, g_470d50, 1 } };
s_candidate_entry g_470d80[1] = { CANDIDATE(0, NONE, 0x82, NONE, 0.0f) };
s_candidate_list g_470d98[1] = { { 0xe, { 0 }, g_470d80, 1 } };
s_candidate_entry g_470da4[2] = { CANDIDATE(0, NONE, 0x77, NONE, 0.0f), CANDIDATE(2, 0x2a, 0x78, NONE, 0.0f) };
s_candidate_entry g_470dd4[1] = { CANDIDATE(2, 0x5, NONE, NONE, 0.0f) };
s_candidate_list g_470dec[2] = { { 0x1, { 0 }, g_470da4, 2 }, { 0x1b, { 0 }, g_470dd4, 1 } };
s_candidate_entry g_470e04[1] = { CANDIDATE(0, NONE, 0x7b, NONE, 0.0f) };
s_candidate_entry g_470e1c[1] = { CANDIDATE(2, 0x10, 0x7c, NONE, 0.0f) };
s_candidate_list g_470e34[2] = { { 0x1, { 0 }, g_470e04, 1 }, { 0xe, { 0 }, g_470e1c, 1 } };
s_candidate_entry g_470e4c[1] = { CANDIDATE(2, 0x2a, 0x79, NONE, 0.0f) };
s_candidate_list g_470e64[1] = { { 0x1, { 0 }, g_470e4c, 1 } };

#undef CANDIDATE

struct s_candidate_group
{
	s_candidate_list *lists;
	short count;
	byte unknown06[2];
};

s_candidate_group g_470e70[0x14] =
{
	{ g_470c90, 2 },
	{ g_470d2c, 1 },
	{ g_470cf0, 3 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ g_470d98, 1 },
	{ 0, 0 },
	{ g_470dec, 2 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ g_470e34, 2 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ 0, 0 },
	{ g_470d68, 2 },
	{ g_470e64, 1 },
};

/* builds g_4f2cc0 and g_4f0a60: the actions of each kind 1 handler for each
   actor type */
// @retail 0x1a6d80
void function_1a6d80(void)
{
	short node_index = 0;
	short group;

	for (group = 0; group < 0x14; group++)
	{
		s_candidate_list *lists[k_slot_type_count];
		short i;
		short type;

		memset(lists, 0, sizeof(lists));
		for (i = 0; i < g_470e70[group].count; i++)
			lists[g_470e70[group].lists[i].type] = &g_470e70[group].lists[i];
		for (type = 0; type < k_slot_type_count; type++)
		{
			s_slot_handler_1 *handler = (s_slot_handler_1 *)g_46eeb8[type];

			if (handler && handler->head.kind == 1)
			{
				s_action_node *nodes[50];
				short count = function_1a6ea0(nodes, lists[type], (s_action_node *)handler->children, (short)handler->child_count);

				g_4f2cc0[type][group].index = node_index;
				g_4f2cc0[type][group].count = (char)count;
				if (count > 0)
				{
					memcpy(&g_4f0a60[node_index], nodes, count * sizeof(s_action_node *));
					node_index += count;
				}
			}
			else
			{
				g_4f2cc0[type][group].index = NONE;
				g_4f2cc0[type][group].count = 0;
			}
		}
	}
}

// @retail 0x1a73c0
s_action_node **function_1a73c0(long owner_index, short id, bool *valid, short *count)
{
	s_actor_view *owner = actor_get(owner_index);
	short local_1 = *(short volatile *)&id;
	short local_2 = g_46eeb8[local_1]->index;
	s_action_table_entry *entry = &g_4f2cc0[local_1][owner->unknown004];
	s_action_node **nodes = &g_4f0a60[entry->index];
	short node_count = entry->count;

	*valid = function_1a71f0(node_count, local_2, nodes);
	*count = node_count;
	return nodes;
}
struct s_action_request
{
	short id;
	byte unknown02[2];
	short timer;
	byte unknown06[2];
	real seconds;
};

struct s_action_context
{
	byte unknown00[8];
	long time;
};

// @retail 0x1a7430
void function_1a7430(long owner_index, s_action_request *request, short *out_id, short *out_state, s_action_context *context, long argument)
{
	short id = request->id;

	*out_id = NONE;
	*out_state = 0;
	if (!ACTION_ID_VALID(id))
		return;

	s_slot_handler *handler = g_46eeb8[id];

	if (g_46eeb8[id]->unknown8 == g_46f348 || (g_4ee4ec & handler->mask) != g_4ee4ec || !(g_557c40[id >> 5] & (1 << (id & 0x1f))))
		return;

	s_actor_view *owner = actor_get(owner_index);
	short timer = request->timer;
	s_game_time_globals *time = g_510c54;

	if (timer != NONE)
	{
		if (timer == -2 && context->time == time->game_time)
		{
		}
		else if (timer >= 0 && timer < 14)
		{
			long last = owner->times[timer];

			if (last != NONE && !((real)(time->game_time - last) * g_510c54->rate > request->seconds))
				return;
		}
		else
		{
			return;
		}
	}

	if (handler->kind == 0)
	{
		short result = ((s_slot_handler_0 *)handler)->query(owner_index, argument);

		if (ACTION_ID_VALID(result) && ACTION_HANDLER_ENABLED(result))
		{
			*out_id = result;
			*out_state = 3;
		}
	}
	else
	{
		*out_id = id;
		if (handler->priority)
			*out_state = handler->priority(owner_index);
		else
			*out_state = 0;
	}

	timer = request->timer;
	if (timer >= 0 && timer < 14)
		owner->times[timer] = g_510c54->game_time;
}

// @retail 0x1a7ad0
dword function_1a7ad0(long owner_index)
{
	s_actor_view *owner = actor_get(owner_index);
	long shift = 0;

	if (owner->unknown267)
		shift = 2;
	else if (owner->unknown266)
		shift = 1;
	return (1 << owner->unknown086_byte) | ((1 << (byte)shift) << 10);
}

// @retail 0x1a7b30
void function_1a7b30(s_flag_bits *result, long owner_index)
{
	s_actor_view *owner = actor_get(owner_index);
	s_tag_instance *tags = g_4e3b44;
	s_flag_bits *source;
	long index = owner->unknown858;

	if (index != NONE && ((s_definition_view *)(g_502408->data + (index & 0xffff) * sizeof(s_definition_view)))->unknown86)
	{
		s_definition_view *definition = (s_definition_view *)(g_502408->data + (index & 0xffff) * sizeof(s_definition_view));

		source = (s_flag_bits *)(tags[definition->tag_index88 & 0xffff].bytes + 0x38);
		*result = *source;
	}
	else if (owner->unknown030 != NONE)
	{
		source = (s_flag_bits *)(g_51e9d8->data + (owner->unknown030 & 0xffff) * 0x98 + 0x84);
		*result = *source;
	}
	else
	{
		*result = g_557c74;
	}

	if (owner->unknown26c != NONE && !owner->unknown269)
	{
		s_object_view *object = OBJECT_HEADER(owner->unknown26c)->object;
		s_tag_element *element = function_1e5450(owner_index, object->definition_index);

		if (element && element->tag_index != NONE)
		{
			function_1c2330(0x83, (dword *)(tags[element->tag_index & 0xffff].bytes + 0x38), result->d, result->d);
			return;
		}
	}

	long other = *(long *)(tags[owner->unknown054 & 0xffff].bytes + 0x20);

	if (other != NONE)
	{
		s_flag_bits *mask = (s_flag_bits *)(tags[other & 0xffff].bytes + 0x38);
		long k;

		for (k = 0; k < 5; k++)
			result->d[k] &= mask->d[k];
	}
}

/* the slot of an actor at a level */
static inline s_slot *actor_slot_get(long actor_index, short level)
{
	return &((s_slot_owner_entry *)actor_get(actor_index))->slots[level];
}

/* the best action of the nodes from first to last (and of their children):
   the one the request evaluates highest, stopping at the first that is
   urgent (more than 1) or at the current one */
// @retail 0x1a75f0
short function_1a75f0(long actor_index, s_action_node **nodes, short first, short last, s_slot *slot, long argument,
	short current, bool children, bool skip_first, short *out_score, short *out_index)
{
	short best_result = g_46fbe4;
	short best_score = 0;
	short best_index = NONE;
	short i;

	for (i = first; i < last; i++)
	{
		s_action_node *child = NULL;
		short result;
		short score;

		if (children)
		{
			child = nodes[i]->next;
			if (child)
			{
				bool found = false;

				do
				{
					if (child->order >= 2)
						break;
					if (!skip_first || i != first)
					{
						function_1a7430(actor_index, (s_action_request *)child, &result, &score, (s_action_context *)slot, argument);
						if (score > best_score)
						{
							best_result = result;
							best_score = score;
							best_index = i;
							if (score > 1)
								goto done;
						}
					}
					if (child->order == 1)
						found = true;
					child = child->next;
				}
				while (child);
				if (found)
					continue;
			}
		}

		s_action_node *node = nodes[i];

		if (current == node->key)
		{
			slot->unknown4 = i;
			goto done;
		}
		if ((!skip_first || i != first) && (current == NONE || (byte)node->flags))
		{
			function_1a7430(actor_index, (s_action_request *)node, &result, &score, (s_action_context *)slot, argument);
			if (score > best_score)
			{
				best_result = result;
				best_score = score;
				best_index = i;
				if (score > 1)
					goto done;
			}
		}
		for (; child; child = child->next)
		{
			function_1a7430(actor_index, (s_action_request *)child, &result, &score, (s_action_context *)slot, argument);
			if (score > best_score)
			{
				best_result = result;
				best_score = score;
				best_index = i;
				if (score > 1)
					goto done;
			}
		}
	}
done:
	*out_score = best_score;
	*out_index = best_index;
	return best_result;
}

/* the choice of a kind 1 handler: the next action after the current one */
// @retail 0x1a77a0
short function_1a77a0(long actor_index, long argument, short level)
{
	s_slot *slot = actor_slot_get(actor_index, level);
	short state = slot->unknown4;
	short count = 0;
	short index = NONE;
	short out;
	short result = function_1a7030(actor_index, level, argument, &out);

	if (result == g_46fbec)
		return g_46fbe4;
	if (result == g_46fbe4)
	{
		bool valid;
		s_action_node **nodes = function_1a73c0(actor_index, slot->type, &valid, &count);

		if (state < count - 1)
		{
			short score;

			result = function_1a75f0(actor_index, nodes, state == NONE ? 0 : state, count, slot, argument, NONE, valid, state != NONE, &score, &index);
		}
		if (result != g_46fbe4)
			slot->unknown4 = index;
	}
	return result;
}

/* the choice of a kind 1 handler: the next action after the current one, or
   else one before it */
// @retail 0x1a78a0
short function_1a78a0(long actor_index, long argument, short level)
{
    long local_0 = actor_index;
	s_slot *slot = actor_slot_get(local_0, level);
	short state = slot->unknown4;
	short count = 0;
	short index = NONE;
	short result = function_1a7030(local_0, level, argument, (short *)&actor_index);

	if (result == g_46fbec)
		return g_46fbe4;
	if (result == g_46fbe4)
	{
		bool valid;
		short score;
		s_action_node **nodes = function_1a73c0(local_0, slot->type, &valid, &count);

		if (state < count - 1)
			result = function_1a75f0(local_0, nodes, state == NONE ? 0 : state, count, slot, argument, NONE, valid, state != NONE, &score, &index);
		if (result == g_46fbe4 && state > 0)
			result = function_1a75f0(local_0, nodes, 0, state, slot, argument, NONE, valid, false, &score, &index);
		if (result != g_46fbe4)
			slot->unknown4 = index;
	}
	return result;
}

/* the choice of a kind 1 handler: the best action of all */
// @retail 0x1a79e0
short __stdcall function_1a79e0(long actor_index, short level, bool active)
{
	short count = 0;
	s_slot *slot = actor_slot_get(actor_index, level);
	short local_0;
	short result = function_1a7030(actor_index, level, *(long *)&active, &local_0);

	if (result == g_46fbec)
		return g_46fbe4;

	bool valid;
	short score;
	short index;
	s_action_node **nodes = function_1a73c0(actor_index, slot->type, &valid, &count);
	short choice = function_1a75f0(actor_index, nodes, 0, local_0 == NONE ? count : slot->unknown4, slot, *(long *)&active, local_0, valid, false, &score, &index);

	if ((result == g_46fbe4 && score > 0) || score > 1)
	{
		slot->unknown4 = index;
		return choice;
	}
	return result;
}

struct s_surface_unit_view
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0xc1 - 0x18];
	byte flags_c1;
	byte unknownc2[0x3dc - 0xc2];
	byte movement_type;
	byte unknown3dd[0x40c - 0x3dd];
	long body_index;
	byte unknown410[0x448 - 0x410];
	long support_index;
	byte unknown44c[4];
	vector3f normal;
};

struct s_surface_support_view
{
	byte unknown00[0xb4];
	long component_index;
};

struct s_surface_component_view
{
	byte unknown00[0x74];
	long body_count;
	byte unknown78[0xa0 - 0x78];
};

struct s_object;
struct s_havok_component;
s_object *function_badc0(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
bool function_e4050(long object_index);
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void havok_component_rigid_body_point_velocity_get(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f *velocity);

#pragma optimize("g", off)
PRIVATE __forceinline void function_1a68a2(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
    arg_2->i = arg_0->i * arg_1;
    arg_2->j = arg_0->j * arg_1;
    arg_2->k = arg_0->k * arg_1;
}
#pragma optimize("", on)

// @retail 0x1a68a0
void function_1a68a0(long object_index, vector3f *velocity)
{
	s_object_header *header = OBJECT_HEADER(object_index);
	if (((1 << header->type) & 1))
	{
		s_surface_unit_view *unit = (s_surface_unit_view *)header->object;
		if (unit->parent_index == NONE && !(unit->flags_c1 & 1) && unit->movement_type == 1 && !function_e4050(object_index))
		{
			/* The queried support velocity uses a separate scratch vector in retail. */
			struct s_1a68a1 { vector3f field_0; point3f field_c; vector3f field_18; };
			s_1a68a1 local_3;
			local_3.field_0 = *g_4687a4;
			if (!function_109e00(object_index, &local_3.field_18, false) && unit->support_index != NONE)
			{
				s_surface_support_view *support = (s_surface_support_view *)function_badc0(unit->support_index, NONE);
				if (support && support->component_index != NONE)
				{
					s_surface_component_view *component = &((s_surface_component_view *)g_51e9b8->data)[support->component_index & 0xffff];
					long body_index = unit->body_index;
					long clamped = body_index < 0 ? 0 : body_index > component->body_count - 1 ? component->body_count - 1 : body_index;
					if (clamped == body_index)
					{
						function_b9dd0(object_index, &local_3.field_c);
						havok_component_rigid_body_point_velocity_get(unit->body_index, (s_havok_component *)component, &local_3.field_c, &local_3.field_18);
					}
				}
			}
			function_ba1d0(object_index, velocity, NULL);
			vector3f relative;
			relative.i = velocity->i - local_3.field_0.i;
			relative.j = velocity->j - local_3.field_0.j;
			relative.k = velocity->k - local_3.field_0.k;
			real projection = unit->normal.i * relative.i + unit->normal.k * relative.k + unit->normal.j * relative.j;
			vector3f projected;
			function_1a68a2(&unit->normal, projection, &projected);
			velocity->i -= projected.i;
			velocity->j -= projected.j;
			velocity->k -= projected.k;
			return;
		}
	}
	function_ba1d0(object_index, velocity, NULL);
}

struct s_object_marker_target
{
	long object_index;
	long marker_index;
	byte unknown08[0x10];
	byte flags;
};

struct s_target_marker
{
	long name;
	real radius;
	real maximum_facing_angle;
	byte unknown0c[4];
	real field_10;
	dword flags;
	real maximum_distance;
};

struct s_target_marker_table
{
	byte unknown00[0x68];
	long count;
	s_target_marker *markers;
};

struct s_object;
s_object *function_bae20(long object_index, dword type_mask);

/* Resolves an object's target marker and its one or two transforms. */
// @retail 0x1a6bf0
bool function_1a6bf0(s_object_marker_target const *target, long *object_index,
	long *marker_index, s_target_marker **definition, transform4x3f *first,
	transform4x3f *second, bool *has_second)
{
	bool result = false;
	long index = target->object_index;

	if (index != NONE && function_bae20(index, NONE) && !(target->flags & 1))
	{
		*object_index = index;
		*marker_index = target->marker_index;
		if (*marker_index == NONE)
			result = true;
		else
		{
			long object_definition = OBJECT_HEADER(*object_index)->object->definition_index;
			long marker_tag = *(long *)(g_4e3b44[object_definition & 0xffff].bytes + 0x38);
			if (marker_tag != NONE)
			{
				s_target_marker_table *table = (s_target_marker_table *)g_4e3b44[marker_tag & 0xffff].bytes;
				if (*marker_index >= 0 && *marker_index < table->count)
				{
					s_object_marker markers[2];
					*definition = &table->markers[*marker_index];
					long marker_name = (*definition)->name;
					long marker_object = *object_index;
					long count = function_b8d30(marker_object, marker_name, markers, 2, false);
					if (count)
					{
						*first = markers[0].matrix;
						result = true;
						if (count == 2)
						{
							*second = markers[1].matrix;
							*has_second = true;
						}
						else
							*has_second = false;
					}
				}
			}
		}
	}
	return result;
}

struct s_sort_candidate_view
{
	long object_index;
	long marker_index;
	point3f point;
	real radius;
	vector3f facing;
	real maximum_facing_angle;
	bool alternate;
	byte unknown29[3];
	vector3f offset;
	vector3f direction;
	real distance;
	real angle;
	real score;
	real secondary_score;
	real scaled_score;
	dword flags;
	real maximum_distance;
};

struct s_sort_weight_view
{
	real distance;
	real angle;
	real scaled_distance;
	real scaled_angle;
	real outer_distance;
	dword flags;
	real secondary_distance;
	real secondary_angle;
	real maximum_distance;
};

real function_1a4870(real distance, real maximum_distance, real angle, real maximum_angle);
real function_50650(real cosine);
real function_30bf0(vector3f *vector);

struct s_weight_unit_view
{
	byte unknown00[0x212];
	char primary_slot;
	char secondary_slot;
	byte unknown214[4];
	long weapons[4];
};

struct s_weight_weapon_definition
{
	byte unknown00[0x12c];
	dword flags;
	byte unknown130[0x208 - 0x130];
	real distance;
	real angle;
	real scaled_distance;
	real scaled_angle;
	real outer_distance;
	byte unknown21c[0x294 - 0x21c];
	short mode;
};

real function_101090(long weapon_index, short zoom);
bool function_101160(long weapon_index);
bool function_1011d0(long weapon_index);

#define WEIGHT_MAX(a, b) ((a) > (b) ? (a) : (b))

/* The pool data pointer is re-read for each object lookup. */
#define WEIGHT_OBJECT_HEADER(index) (&((s_object_header *)((s_record_pool volatile *)g_4e0300)->data)[(index) & 0xffff])

// @retail 0x1a50a0
bool function_1a50a0(long unit_index, short zoom, s_sort_weight_view *weights)
{
	bool result = false;
	if (unit_index != NONE)
	{
		s_weight_unit_view *unit = (s_weight_unit_view *)WEIGHT_OBJECT_HEADER(unit_index)->object;
		short primary_slot = unit->primary_slot;
		if (primary_slot != NONE)
		{
			long weapon_index = unit->weapons[primary_slot];
			if (weapon_index != NONE)
			{
				s_weight_weapon_definition *definition = (s_weight_weapon_definition *)g_4e3b44[WEIGHT_OBJECT_HEADER(weapon_index)->object->definition_index & 0xffff].bytes;
				if (zoom != NONE || !(bool)((definition->flags >> 5) & 1))
				{
					real scale = function_101090(weapon_index, zoom);
					real inverse = 1.0f / scale;
					weights->distance = definition->distance * inverse;
					weights->angle = definition->angle * scale;
					weights->scaled_distance = definition->scaled_distance * inverse;
					weights->scaled_angle = definition->scaled_angle * scale;
					real outer_distance = WEIGHT_MAX(definition->outer_distance, definition->distance) * inverse;
					weights->outer_distance = outer_distance;
					weights->flags = 0;
					weights->maximum_distance = 0.0f;
					if (function_1011d0(weapon_index))
					{
						if (definition->mode == 1)
						{
							weights->flags = 0x20;
							weights->maximum_distance = weights->angle;
						}
					}
					else if (function_101160(weapon_index))
					{
						if (definition->mode == 1)
						{
							weights->flags = 8;
							weights->maximum_distance = weights->angle;
						}
						else if (definition->mode == 2)
						{
							weights->flags = 0x10;
							weights->maximum_distance = weights->angle;
						}
					}
					unit = (s_weight_unit_view *)WEIGHT_OBJECT_HEADER(unit_index)->object;
					short secondary_slot = unit->secondary_slot;
					if (secondary_slot != NONE && unit->weapons[secondary_slot] != NONE)
					{
						long secondary_index = unit->weapons[secondary_slot];
						definition = (s_weight_weapon_definition *)g_4e3b44[WEIGHT_OBJECT_HEADER(secondary_index)->object->definition_index & 0xffff].bytes;
						scale = function_101090(secondary_index, zoom);
						inverse = 1.0f / scale;
						weights->secondary_distance = definition->distance;
						weights->secondary_angle = definition->angle;
						weights->scaled_distance = WEIGHT_MAX(weights->scaled_distance, definition->scaled_distance * inverse);
						weights->scaled_angle = WEIGHT_MAX(weights->scaled_angle, definition->scaled_angle * scale);
						weights->outer_distance = WEIGHT_MAX(outer_distance, WEIGHT_MAX(definition->outer_distance, definition->distance) * inverse);
					}
					else
					{
						weights->secondary_distance = 0.0f;
						weights->secondary_angle = 0.0f;
					}
					result = true;
				}
			}
		}
	}
	return result;
}

#undef WEIGHT_OBJECT_HEADER
#undef WEIGHT_MAX

struct s_unit_child_iterator
{
	long object_index;
	long unit_index;
	short seat_index;
	long next_index;
};

struct s_target_occupant_view
{
	long definition_index;
	byte unknown04[0x10 - 4];
	long first_child_index;
	byte unknown14[0x138 - 0x14];
	short team;
};

struct s_target_seat_view
{
	dword flags;
	byte unknown04[0xb0 - 4];
};

struct s_target_seats_definition
{
	byte unknown00[0x1cc];
	s_target_seat_view *seats;
};

struct s_damage_object;
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);
bool function_1df560(short team_a, short team_b);

struct s_target_link_view
{
	long definition_index;
	dword field_4;
	byte field_8[4];
	long field_c;
	long field_10;
	long field_14;
	byte field_18[0xaa - 0x18];
	byte field_aa;
	byte field_ab[0x10a - 0xab];
	struct { word : 2; word field_2 : 1; word : 13; } field_10a;
	byte field_10c[0x138 - 0x10c];
	short field_138;
};

struct s_target_link_definition
{
	byte field_0[0xbc];
	dword field_bc;
	byte field_c0[0x1ec - 0xc0];
	dword field_1ec;
};

bool function_1a59f0(long object_index, long excluded_index, short team, bool *has_hostile);
bool function_1a5b00(long object_index, short team, bool *has_hostile);

// @retail 0x1a5910
bool __stdcall function_1a5910(long object_index, long excluded_index, short team, bool *has_hostile)
{
	/* Keep the recursive entry arguments in their stack slots. */
	long const *local_object = &object_index;
	long const *local_excluded = &excluded_index;
	bool result;
	*has_hostile = false;
	if (*local_object != *local_excluded)
	{
		s_object_header *header = OBJECT_HEADER(object_index);
		result = true;
		if ((1 << header->type) & 3)
		{
			s_target_link_view *object = (s_target_link_view *)header->object;
			s_target_link_definition *definition = (s_target_link_definition *)((s_tag_instance volatile *)g_4e3b44)[object->definition_index & 0xffff].bytes;
			short object_team = object->field_138;
			result = !(bool)object->field_10a.field_2 && !(bool)((definition->field_bc >> 21) & 1);
			if (result)
			{
				if (((1 << object->field_aa) & 2) && object_team == NONE)
					result = function_1a59f0(object_index, excluded_index, team, has_hostile);
				else
					*has_hostile = !function_1df560(team, object_team);
			}
		}
	}
	else
		result = false;
	return result;
}

// @retail 0x1a59f0
bool function_1a59f0(long object_index, long excluded_index, short team, bool *has_hostile)
{
	s_target_link_view *object = (s_target_link_view *)OBJECT_HEADER(object_index)->object;
	s_target_link_definition *definition = (s_target_link_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
	*has_hostile = false;
	if ((bool)((definition->field_1ec >> 21) & 1))
		return true;
	if (object->field_14 != NONE && (bool)((object->field_4 >> 26) & 1))
	{
		long parent_index = object->field_14;
		if ((1 << OBJECT_HEADER(parent_index)->type) & 3)
			return function_1a5910(parent_index, excluded_index, team, has_hostile);
	}
	if (function_1a5b00(object_index, team, has_hostile))
		return true;
	long child_index = object->field_10;
	while (child_index != NONE)
	{
		object = (s_target_link_view *)OBJECT_HEADER(child_index)->object;
		if ((bool)((object->field_4 >> 26) & 1) && ((1 << object->field_aa) & 3))
		{
			if (function_1a5b00(child_index, team, has_hostile))
				return true;
		}
		child_index = object->field_c;
	}
	return false;
}


// @retail 0x1a5b00
bool function_1a5b00(long object_index, short team, bool *has_hostile)
{
	long object_offset = (object_index & 0xffff) * sizeof(s_object_header);
	s_target_seats_definition *definition = (s_target_seats_definition *)g_4e3b44[((s_object_header *)(object_offset + (byte *)g_4e0300->data))->object->definition_index & 0xffff].bytes;
	bool result = false;
	s_unit_child_iterator iterator;
	*has_hostile = false;
	iterator.object_index = object_index;
	iterator.unit_index = NONE;
	iterator.seat_index = NONE;
	iterator.next_index = ((s_target_occupant_view *)((s_object_header *)(object_offset + (byte *)g_4e0300->data))->object)->first_child_index;
	while (function_d05c0(&iterator))
	{
		s_target_occupant_view *occupant = (s_target_occupant_view *)OBJECT_HEADER(iterator.unit_index)->object;
		if (iterator.seat_index != NONE)
		{
			if (!function_1df560(team, occupant->team))
			{
				if (!(bool)((definition->seats[iterator.seat_index].flags >> 11) & 1))
				{
					result = true;
					*has_hostile = result;
					goto local_0;
				}
			}
			else
				result = true;
		}
	}
local_0:
	return result;
}

struct s_candidate_object_view
{
	long definition_index;
	byte unknown04[0x3c - 4];
	real radius;
	byte unknown40[0xaa - 0x40];
	byte type;
};

struct s_candidate_unit_definition
{
	byte unknown00[0x240];
	dword flags;
	real maximum_distance;
};

// @retail 0x1a5e40
bool function_1a5e40(long object_index, long marker_index, s_target_marker const *marker,
	transform4x3f const *first, transform4x3f const *second, bool alternate,
	point3f const *origin, vector3f const *direction, s_sort_candidate_view *candidate)
{
	s_candidate_object_view *object = (s_candidate_object_view *)OBJECT_HEADER(object_index)->object;
	bool result = false;
	candidate->object_index = object_index;
	candidate->marker_index = marker_index;
	candidate->maximum_distance = 0.0f;
	candidate->flags = 0;
	if (marker_index == NONE)
	{
		if (!((1 << object->type) & 1))
			return result;
		if (!function_1a62e0(object_index, &candidate->point, direction, origin))
			return false;
		s_candidate_unit_definition *definition = (s_candidate_unit_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
		if (definition->flags & 1)
			candidate->flags |= 1;
		else
			candidate->flags &= ~1;
		if (definition->flags & 6)
			candidate->flags |= 2;
		else
			candidate->flags &= ~2;
		candidate->maximum_distance = definition->maximum_distance;
		candidate->radius = object->radius;
		candidate->maximum_facing_angle = 0.0f;
	}
	else
	{
		if (second)
		{
			vector3f axis;
			vector3d_from_points3d(&first->position, &second->position, &axis);
			function_1a6340(&axis, direction, &candidate->point, &first->position, origin, marker->radius);
		}
		else
			function_1a65b0(direction, &candidate->point, origin, &first->position, marker->radius);
		if (marker->flags & 1)
			candidate->flags |= 1;
		else
			candidate->flags &= ~1;
		if (marker->flags & 0x12)
			candidate->flags |= 2;
		else
			candidate->flags &= ~2;
		candidate->maximum_distance = marker->maximum_distance;
		candidate->radius = marker->radius;
		candidate->maximum_facing_angle = marker->maximum_facing_angle;
		candidate->facing = first->forward;
	}
	vector3d_from_points3d(origin, &candidate->point, &candidate->offset);
	candidate->direction = candidate->offset;
	candidate->distance = function_30bf0(&candidate->direction);
	candidate->alternate = alternate;
	real cosine = dot3f(&candidate->direction, direction);
	cosine = -1.0f > cosine ? -1.0f : cosine > 1.0f ? 1.0f : cosine;
	candidate->angle = function_50650(cosine);
	result = true;
	return result;
}

// @retail 0x1a60f0
bool function_1a60f0(s_sort_candidate_view *candidate, s_sort_weight_view const *weights, real scale)
{
	bool result = true;
	if (candidate->maximum_facing_angle > 0.0f)
	{
		vector3f inverse;
		inverse.i = 0.0f - candidate->direction.i;
		inverse.j = 0.0f - candidate->direction.j;
		inverse.k = 0.0f - candidate->direction.k;
		real cosine = dot3f(&candidate->facing, &inverse);
		cosine = -1.0f > cosine ? -1.0f : cosine > 1.0f ? 1.0f : cosine;
		if (!(function_50650(cosine) <= candidate->maximum_facing_angle))
			result = false;
	}
	if (result && scale != 0.0f)
	{
		if (weights->flags & 8)
		{
			if (!(candidate->flags & 1) || candidate->distance >
				(candidate->maximum_distance == 0.0f ? weights->maximum_distance : candidate->maximum_distance))
				result = false;
		}
		if ((candidate->flags & 2) && candidate->distance >
			(candidate->maximum_distance == 0.0f ? weights->maximum_distance : candidate->maximum_distance))
			candidate->flags &= ~2;
		if (result)
		{
			if (!candidate->alternate)
			{
				candidate->score = function_1a4870(candidate->angle, weights->distance, candidate->distance, weights->angle);
				candidate->secondary_score = function_1a4870(candidate->angle, weights->secondary_distance, candidate->distance, weights->secondary_angle);
				candidate->scaled_score = scale * function_1a4870(candidate->angle, weights->scaled_distance, candidate->distance, weights->scaled_angle);
				if (candidate->score == 0.0f && candidate->secondary_score == 0.0f && candidate->scaled_score == 0.0f)
					result = false;
			}
			else
			{
				real score = function_1a4870(candidate->angle, weights->distance, candidate->distance, weights->angle);
				candidate->score = 0.0f;
				candidate->secondary_score = 0.0f;
				candidate->scaled_score = 0.0f;
				if (score < 1.0f)
					result = false;
			}
		}
	}
	else
		result = false;
	return result;
}

struct s_candidate_scale_view
{
	byte field_0[8];
	real field_8;
};

struct s_candidate_scale_globals
{
	byte field_0[0xf4];
	s_candidate_scale_view *field_f4;
};

// @retail 0x1a5c00
long function_1a5c00(s_sort_weight_view const *weights, long object_index, bool alternate,
	point3f const *origin, vector3f const *direction, long count, s_sort_candidate_view *candidates)
{
	long &local_0 = count;
	s_target_link_view *object = (s_target_link_view *)OBJECT_HEADER(object_index)->object;
	byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
	real scale = 1.0f;
	if (((1 << object->field_aa) & 3) &&
		(bool)((((s_target_link_definition *)g_4e3b44[(*(long volatile *)&object->definition_index) & 0xffff].bytes)->field_bc >> 19) & 1))
		scale = ((s_candidate_scale_globals *)g_4e034c)->field_f4->field_8;
	if (local_0 < 64 && function_1a5e40(object_index, NONE, NULL, NULL, NULL, alternate,
		origin, direction, &candidates[local_0]) && function_1a60f0(&candidates[local_0], weights, scale))
		local_0++;
	long marker_tag = *(long *)(definition + 0x38);
	if (marker_tag != NONE)
	{
		s_target_marker_table *table = (s_target_marker_table *)g_4e3b44[marker_tag & 0xffff].bytes;
		s_sort_candidate_view *candidate = &candidates[local_0];
		long volatile &local_1 = count;
		for (long marker_index = 0; marker_index < table->count; marker_index++)
		{
			s_target_marker *marker = &table->markers[marker_index];
			s_object_marker markers[2];
			long marker_count = function_b8d30(object_index, marker->name, markers, 2, false);
			if (marker_count != 0)
			{
				s_object_marker *second = marker_count == 2 ? &markers[1] : NULL;
				if (local_1 < 64 && function_1a5e40(object_index, marker_index, marker,
					&markers[0].matrix, second ? &second->matrix : NULL, alternate,
					origin, direction, candidate) &&
					function_1a60f0(candidate, weights, marker->field_10 * scale))
				{
					local_1++;
					candidate++;
				}
			}
		}
	}
	return local_0;
}

struct s_1a56c0
{
	byte field_0[0x30];
	point3f field_30;
	real field_3c;
};

PRIVATE inline real local_0(vector3f const *delta)
{
	real result = delta->i * delta->i;
	result += delta->j * delta->j;
	result += delta->k * delta->k;
	return result;
}

// @retail 0x1a56c0
long __stdcall function_1a56c0(s_sort_weight_view const *weights, long object_index,
	point3f const *origin, vector3f const *direction, real distance, real spread,
	real cosine, long excluded_index, short team, long count, s_sort_candidate_view *candidates)
{
	do
	{
		s_target_link_view *object = (s_target_link_view *)OBJECT_HEADER(object_index)->object;
		bool has_hostile = false;
		if (function_1a58b0(object_index))
		{
			s_1a56c0 *bounds = (s_1a56c0 *)OBJECT_HEADER(object_index)->object;
			real radius = bounds->field_3c;
			vector3f delta;
			vector3d_from_points3d(origin, &bounds->field_30, &delta);
			real projection = direction->j * delta.j;
			projection += direction->k * delta.k;
			projection += delta.i * direction->i;
			if (projection >= 0.0f - radius && projection <= radius + distance &&
				(radius * spread * 2.0f + projection) * projection + radius * radius >=
				local_0(&delta) * cosine * cosine &&
				function_1a5910(object_index, excluded_index, team, &has_hostile))
			{
				count = function_1a5c00(weights, object_index, has_hostile, origin, direction, count, candidates);
				if (count >= 64)
					break;
				if (object->field_10 != NONE)
				{
					count = function_1a56c0(weights, object->field_10, origin, direction,
						distance, spread, cosine, excluded_index, team, count, candidates);
					if (count >= 64)
						break;
				}
			}
		}
		object_index = object->field_c;
	}
	while (object_index != NONE);
	return count;
}

extern bool g_4de2f8;
extern long g_4de2fc;
extern long g_4de300[0x800];

short function_14a6d0(short arg_0, real arg_1, point3f const *arg_2, vector3f const *arg_3,
	real arg_4, real arg_5, short arg_6, short *arg_7, short *arg_8);
short function_bb430(long arg_0, short arg_1, short const *arg_2, short arg_3, long *arg_4);

// @retail 0x1a54a0
long __stdcall function_1a54a0(s_sort_weight_view const *arg_0, short arg_1,
	point3f const *arg_2, vector3f const *arg_3, long arg_4, short arg_5, s_sort_candidate_view *arg_6)
{
	long local_0 = 0;
	short local_1[0x200];
	long local_2[0x800];
	real local_3 = arg_0->angle > (arg_0->secondary_angle > arg_0->scaled_angle ? arg_0->secondary_angle : arg_0->scaled_angle) ? arg_0->angle : (arg_0->secondary_angle > arg_0->scaled_angle ? arg_0->secondary_angle : arg_0->scaled_angle);
	real local_4 = arg_0->distance > (arg_0->secondary_distance > arg_0->scaled_distance ? arg_0->secondary_distance : arg_0->scaled_distance) ? arg_0->distance : (arg_0->secondary_distance > arg_0->scaled_distance ? arg_0->secondary_distance : arg_0->scaled_distance);
	if (local_3 > 0.0f && local_4 > 0.0f)
	{
		real local_5 = sin(local_4);
		real local_6 = cos(local_4);
		short local_7 = function_14a6d0(arg_1, local_6, arg_2, arg_3, local_3, local_5, 0x200, local_1, NULL);
		long local_8 = function_bb430(1, local_7, local_1, 0x800, local_2);
		s_record_pool *local_9 = g_4e8c24;
		g_4de2f8 = true;
		long local_10 = NONE;
		for (;;)
		{
			local_10 = function_16bc00(local_9, local_10 + 1);
			if (local_10 == NONE)
				break;
			s_player_view *local_11 = (s_player_view *)(local_9->data + local_10 * local_9->size);
			if (!local_11)
				break;
			long local_12 = local_11->unit_index;
			if (local_12 != NONE)
			{
				long local_13;
				do
				{
					local_13 = local_12;
					local_12 = ((s_target_link_view *)OBJECT_HEADER(local_12)->object)->field_14;
				}
				while (local_12 != NONE);
				if (g_4de300[local_13 & 0xffff] != g_4de2fc)
				{
					g_4de300[local_13 & 0xffff] = g_4de2fc;
					local_2[local_8++] = local_13;
				}
			}
		}
		g_4de2f8 = false;
		for (long local_14 = 0; local_14 < local_8; local_14++)
		{
			local_0 = function_1a56c0(arg_0, local_2[local_14], arg_2, arg_3,
				local_3, local_5, local_6, arg_4, arg_5, local_0, arg_6);
			if (local_0 >= 0x40)
				break;
		}
	}
	return local_0;
}

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long arg_0, point3f const *arg_1, vector3f const *arg_2,
	long arg_3, long arg_4, s_collision_result_1697c0 *arg_5);

struct s_1a67f0
{
	byte field_0[0x24];
	short field_24;
	byte field_26[0x5c - 0x26];
};

PRIVATE inline long function_1a67f1(long arg_0)
{
	long local_0 = NONE;
	while (arg_0 != NONE)
	{
		local_0 = arg_0;
		arg_0 = ((s_target_link_view *)OBJECT_HEADER(arg_0)->object)->field_14;
	}
	return local_0;
}

// @retail 0x1a67f0
byte function_1a67f0(point3f const *arg_0, point3f const *arg_1, long arg_2, long arg_3)
{
	s_1a67f0 local_0;
	local_0.field_24 = NONE;
	long local_1 = function_1a67f1(arg_2);
	long local_2 = function_1a67f1(arg_3);
	vector3f local_3;
	vector3d_from_points3d(arg_0, arg_1, &local_3);
	return !function_1697c0(0x16808c2d, arg_0, &local_3, local_2, local_1, (s_collision_result_1697c0 *)&local_0);
}

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *arg_0, long arg_2, point3f *arg_1);

struct s_1a5371
{
	short field_0;
	byte field_2[6];
};

struct s_1a5370
{
	byte field_0[0x30];
	s_1a5371 *field_30;
};

// @retail 0x1a5370
bool __stdcall function_1a5370(s_sort_weight_view const *arg_0, point3f const *arg_1,
	vector3f const *arg_2, long arg_3, short arg_4, s_sort_candidate_view *arg_5)
{
	bool local_0 = false;
	volatile bool local_6 = local_0;
	s_sort_candidate_view local_1[0x40];
	long local_2 = function_14a280(g_4e033c, 0, (point3f *)arg_1);
	if (local_2 != NONE)
	{
		short local_3 = ((s_1a5370 *)g_4e0348)->field_30[local_2].field_0;
		if (local_3 != NONE)
		{
			long local_4 = function_1a54a0(arg_0, local_3, arg_1, arg_2, arg_3, arg_4, local_1);
			if (local_4 > 0)
			{
				g_51e99c = (s_sort_globals *)arg_0;
				qsort(local_1, local_4, sizeof(s_sort_candidate_view), (int (__cdecl *)(void const *, void const *))function_1a66f0);
				real *volatile local_7 = &local_1[0].radius;
                for (long local_5 = 0; local_5 < local_4; local_5++, local_7 = (real *)((byte *)local_7 + 0x60))
				{
					real *local_8 = local_7;
                    if (function_1a67f0(arg_1, (point3f *)((byte *)local_8 - 0x0c), *(long *)((byte *)local_8 - 0x14), arg_3))
					{
						*arg_5 = local_1[local_5];
						return true;
					}
				}
				return local_6;
			}
		}
	}
	return local_0;
}
