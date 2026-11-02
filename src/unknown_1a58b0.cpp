// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1a58b0.h"
#include "unknown_1cec30.h"
#include "object_markers.h"
#include "vector_products.h"
#include "squads.h"

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
	byte unknown30[0xc0 - 0x30];
	char field_c0;
	byte unknownc1[0x174 - 0xc1];
	long field_174;
	long field_178;
	byte unknown17c[0x21c - 0x17c];
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

/* the tag that holds a set of 0x83 slot types (the definition's tag_index88,
   a tag element's tag_index, a character's +0x20) */
struct s_type_mask_definition
{
	byte unknown00[0x38];
	dword enabled_types[5];
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

void bit_vector_intersect(dword *result, dword *a, dword *b, long bit_count);

/* ---- globals ---- */

s_candidate_table g_4ee4f0[3];
byte g_4f09c8[0x84];
dword g_4f0a4c[5];
s_sort_globals *g_51e99c;
s_flag_bits g_557c74;

/* ---- helpers ---- */

#define ACTION_ID_VALID(id) ((id) >= 0 && (id) < 0x83)

/* whether the type's bit is set in the enabled set g_557c40 */
PRIVATE __forceinline bool action_type_bit_set(short id)
{
	return (g_557c40[id >> 5] & (1 << (id & 0x1f))) != 0;
}

/* whether the handler is enabled in this build */
#define ACTION_HANDLER_ENABLED(id) \
	(g_46eeb8[id]->unknown8 != g_46f348 && (g_46eeb8[id]->mask & g_4ee4ec) == g_4ee4ec && action_type_bit_set(id))

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
void function_1a6340(vector3f const *b, vector3f const *a, point3f *out, point3f const *c, point3f const *p, real radius);

/* the point of the object's capsule axis nearest the line from origin along
   direction, into out; false when the capsule has no radius */
// @retail 0x1a62e0
bool function_1a62e0(long object_index, point3f const *origin, vector3f const *direction, point3f *out)
{
	point3f bottom;
	vector3f axis;
	real radius;
	bool result = false;
	function_df6c0(&bottom, &axis, object_index, &radius);
	if (radius > 0.0001f)
	{
		function_1a6340(&axis, direction, out, origin, &bottom, radius);
		result = true;
	}
	return result;
}

// @retail 0x1a6340
void function_1a6340(vector3f const *b, vector3f const *a, point3f *out, point3f const *c, point3f const *p, real radius)
{
	vector3f n;
	cross3f(b, a, &n);
	real m = n.i * n.i + n.k * n.k + n.j * n.j;

	if (m > 0.0f)
	{
		vector3f d;
		vector3d_from_points3d(p, c, &d);
		vector3f e;
		cross3f(&d, a, &e);
		real t = e.j * n.j;
		t += e.k * n.k;
		t += e.i * n.i;
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
		scale_vector_by_double(&w, radius / sqrt(wm), &w);
	}
	out->x = out->x - w.i;
	out->y = out->y - w.j;
	out->z = out->z - w.k;
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
		scale_vector_by_double(&w, radius / sqrt(wm), &w);
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

// @retail 0x1a7030
short function_1a7030(long owner_index, short slot_index, long argument, short *out)
{
	s_actor_view *owner = actor_get(owner_index);
	s_slot *slot = &owner->slots[slot_index];
	s_slot *next = &owner->slots[slot_index + 1];
	short result = g_46fbe4;
	short selected = NONE;

	if (next->state == 0 && ACTION_ID_VALID(next->type) && slot->unknown4 != NONE && ACTION_HANDLER_ENABLED(next->type))
	{
		s_slot_handler *handler = g_46eeb8[next->type];
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
					slot->unknown4 = NONE;
					result = g_46fbe4;
				}
			}
			else
			{
				slot->unknown4 = NONE;
				if (code == g_46fbec)
					result = code;
				else
					result = g_46fbe4;
			}
		}
	}
	else
	{
		slot->unknown4 = NONE;
	}

	*out = selected;
	return result;
}

/* link a node in after an element */
PRIVATE __forceinline void function_1a71f1(s_action_node *arg_0, s_action_node *arg_1)
{
	arg_1->next = arg_0->next;
	arg_0->next = arg_1;
	arg_1->order = 0;
}

/* link a node into the list of an element, after the nodes of a lower order */
PRIVATE __forceinline void function_1a71f2(s_action_node *arg_0, s_action_node *arg_1, short arg_2)
{
	if (arg_0->next)
	{
		do
		{
			if (!arg_0->next || arg_0->next->order >= arg_2)
			{
				arg_1->next = arg_0->next;
				arg_0->next = arg_1;
				break;
			}
			arg_0 = arg_0->next;
		}
		while (arg_0);
	}
	else
	{
		arg_1->next = 0;
		arg_0->next = arg_1;
	}
	arg_1->order = arg_2;
}

// @retail 0x1a71f0
bool function_1a71f0(short count, short id, s_action_node **list)
{
	bool result = false;

	if (g_4f0a4c[id >> 5] & (1 << (id & 0x1f)))
	{
		result = true;
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
				function_1a71f1(list[0], node);
			}
			else if (entry->kind == 4)
			{
				function_1a71f2(list[count - 1], node, 2);
			}
			else
			{
				short j;

				for (j = 0; j < count; j++)
				{
					s_action_node *element = list[j];

					if (element->key == entry->key)
					{
						if (entry->kind == 1)
							function_1a71f1(element, &entry->node);
						else if (entry->kind == 2)
							function_1a71f2(element, &entry->node, 1);
						else if (entry->kind == 3)
							function_1a71f2(element, &entry->node, 2);
						break;
					}
				}
			}
		}
	}
	return result;
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

// score 6 (was 6): the two output pointers are loaded one after the other here and both before the
// first store in retail, and the word is written after the stack is restored in retail.
// Nested count assignment/return count expression/full return expression 6/6/6;
// comma output statement/indexed and offset returns 6/6/6;
// const node-list pointer/handler local 6/6.
// Count at each use 53; table value snapshot 27; validity from a false-default test 52;
// success/failure output arms 15/15; native group snapshot 26; word count 6; true-default test 79.
// Round 88: call result local with the count stored first 6 (retail's ECX/EDX pointer registers,
// but the count store comes first); count stored before the call 12; handler index read in the
// call 26; output pointer locals 6; forceinline output setter 6.
// Round 55: table entry pointer local 28 (104 bytes); call result local stored in order 6; short
// handler index 7; handler index read after the table reads 20; count read before the node list 6.
// Round 49: actor lookup at both table reads 27; group local without the actor local 14 (103 bytes);
// result pointer declared first and set after the count 6; char count 6; group local after the actor
// local 14 (103 bytes); table index read into a local before the node list 6. Diagnostics: the valid
// pointer takes EDX with the count store removed or a NULL return; with the count stored first the count
// pointer loads first into EDX and the valid pointer into ECX (retail's registers, wrong store order).
// @retail 0x1a73c0
s_action_node **function_1a73c0(long owner_index, short id, bool *valid, short *count)
{
	s_actor_view *owner = actor_get(owner_index);
	long local_2 = g_46eeb8[id]->index;
	s_action_node **nodes = &g_4f0a60[g_4f2cc0[id][owner->unknown004].index];
	short node_count = g_4f2cc0[id][owner->unknown004].count;

	*valid = function_1a71f0(node_count, (short)local_2, nodes);
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
		else if (request->timer >= 0 && request->timer < 14)
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
void function_1a7b30(long owner_index, s_flag_bits *result)
{
	s_actor_view *owner = actor_get(owner_index);
	s_tag_instance *tags = g_4e3b44;
	s_definition_view *definition;
	long index = owner->unknown858;

	if (index != NONE && (definition = (s_definition_view *)(g_502408->data + (index & 0xffff) * sizeof(s_definition_view)))->unknown86)
	{
		s_type_mask_definition *mask = (s_type_mask_definition *)tags[definition->tag_index88 & 0xffff].bytes;
		memcpy(result, mask->enabled_types, sizeof(*result));
	}
	else if (owner->unknown030 != NONE)
	{
		s_squad_datum *squad = squad_get(owner->unknown030);
		memcpy(result, squad->enabled_types, sizeof(*result));
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
			bit_vector_intersect(result->d, ((s_type_mask_definition *)tags[element->tag_index & 0xffff].bytes)->enabled_types, result->d, 0x83);
			return;
		}
	}

	byte *owner_definition = tags[owner->unknown054 & 0xffff].bytes;

	if (*(long *)(owner_definition + 0x20) != NONE)
	{
		bit_vector_intersect(result->d, result->d, ((s_type_mask_definition *)tags[*(long *)(owner_definition + 0x20) & 0xffff].bytes)->enabled_types, 0x83);
	}
}

short actor_behavior_get_insert_level(short type, s_slot_owner_entry *entry, short slot);
bool function_1a80e0(long index, short type, s_slot *data, short slot);
void function_25aba0(long actor_index);



// score 29 (was 29): the kind-2 failure pop is `dec [mem]` where retail loads, decrements and stores
// around the stamp, and the choice-failure pop is split where retail uses `dec [mem]` (aligned 8).
// Round 96: kind-2 dispatch first 147; the kind test reversed 126; the current-slot ternary inverted
// 29 (identical); current-slot assigned in an if 242; the default type assigned in an if 166; the
// next-type test inverted 36; positive next-handler validation 29 and positive enabled dispatch 29
// (both identical). Original dispatch and failure arms restored.
// The choice arm's pop before its stamp is not retail's order there: it is kept only because its 5
// extra bytes balance the kind-2 pop's 5 missing ones, so put the stamp first again once the kind-2
// pop splits. Before this round only the state-one pop and the `= {0}` workspace remained (aligned 7); earlier: actor/result ESI/EDI (0x1a7b30's result arrives in ESI,
// retail EDI), the state-one level pop (ours `dec [mem]`, retail load/dec/store around the handler
// store), the `= {0}` workspace (retail clears it as a memset, rep stosd x16) and 0x1a8080's argument
// set-up order differ. memset plus 0x1a8080 as (type, owner, level) leave only the entry registers and
// the state-one pop (957/957 bytes, aligned 21 from 24, positional 143).
// Round 78: the entry loop as `for (short i = 0; i < 3; i++)` in place of the hand-inverted do/while
// 69 (identical code, kept; a long counter unrolls it: 1292 bytes, 651); next-handler test through
// ACTION_HANDLER_ENABLED 87; state-one pop before the stamp 140 (957 bytes, the kind-2 pop takes
// `dec [mem]`); `if (level < 0) break;` for the head flag 215 (rotated).
// Tried: native workspace initializer 69; owner lifetime 266; level lifetime 320; entry pointer walk
// 261; full-width choice 150; full-width type 338; two-word initializer 68; full member initializer
// 287; retained ordinal 244; shared final exit 69; frame pointer on native initializer 334; outer
// workspace 143; short copy counter 69; long level 173; retained handler reads 69; ordinary dispatch
// first 131; empty native initializer 143; value/assigned-value initialization 249 each; empty
// initializer with early state pop 146; owner/handler records 69 each; current-level snapshot 69;
// level snapshot with empty initializer 143; native initializer with early state pop 140; candidate
// index snapshot 252; candidate receiver 69; choice before workspace 69; callback/insertion names 69;
// completion/eligibility/insertion results 69/163/69; existing mask helper inline 69; with
// 0x1a7b30's sources sharing one copy (NULL-preset source there), both callees' parameters reordered
// and the memset workspace, only the state-one pop differs (retail keeps level in a register around
// the handler store): NULL-preset source 62, plus 0x1a7b30 order 60, plus memset 134 (aligned 4), pop
// before the store 137, plus 0x1a8080 order 134 (aligned 2), without memset 58, the two orders alone
// 67; original body kept (the NULL preset is a tested sentinel; 0x1a7b30 rises to 100). Enclosing
// enabled/state-one/choice dispatch arms in seven combinations 69 each, all three with current-slot at
// enabled use 69; memset 143; 0x1a8080 (type, owner, level) 67; both 143; on both: state-one
// `level -= 1` 143, pop before the store 146, `else if` after the enabled test 143, enabled and
// state-one tests in one arm 219.
// Round 93, on the NULL-preset + both reorders + memset base (957 bytes, only the state-one pop left):
// `level -= 1` 134; pop first 137; state-one, kind-2 and choice arms as one if/else chain without
// `continue` 119 (state-one pop now as retail, but the kind-2 failure pop becomes `dec [mem]`); chain with
// `continue` in the kind-2 arm only 134, in the state-one arm only 119; kind-2 failure statements
// reordered (state first 119; pop before the state store or first 134: the state-one pop goes back to
// `dec [mem]`); `while (level >= 0)` 215 (rotated). Exactly one of the state-one and kind-2 failure pops
// takes the `dec [mem]` form in every arrangement.
// Round 100 (on the kept body): state-one pop as `--level` or `level = level - 1` 69/69; kind-2 failure
// state store before the stamp 69; state-one / kind-2 / choice as one if-else chain 69 (identical code
// on this base: only the state-one pop is fused); slot selection after the enabled test 177.
// Round 10 (lane 13), with 0x1a7b30 unchanged (its result pointer arrives in ESI, retail EDI): memset
// alone 143 (957/957, aligned 12 from 16: entry registers, the state-one pop and 0x1a8080's argument
// set-up); plus 0x1a8080 as (type, owner, level) 143 (aligned 10, 0x1a8080 still matches); plus the
// state-one pop first 146; plus the state-one and kind-2 arms as an if/else chain 128 (aligned 10:
// state-one split as retail, kind-2 fused); on the chain the kind-2 pop first or between its stores
// 143 each (kind-2 split, state-one fused again); the kind-2 success leaving by `break` 128.
// Diagnostics: a different state-one decrement leaves kind-2 fused on the chain, and a different
// kind-2 decrement leaves state-one fused on the continue form, so the two pops aren't merged; the
// choice looks like the allocator's (a temporary register or a memory operand for the decrement).
// Round 99 (lane 6), on the kept body: the first enabled test through the handler local, as the
// next-handler test is written, 69 (identical); kind-2 failure with the state store first 69 (aligned
// 18: only that arm's store order moves, the state-one pop stays `dec [mem]`); the state-one stamp
// through the table 69; `current` from `&owner->slots[level]` 69; `current` declared before the
// handler 159 (966 bytes). The entry registers follow 0x1a7b30's own ESI/EDI colouring (its result
// pointer arrives in ESI, retail EDI).
// Round 88: 0x1a7b30 now matches with retail's owner ESI / result EDI, and the entry follows it
// (69 -> 62); 0x1a7b30 as (owner_index, result) and 0x1a8080 as (type, owner, level), each consistent,
// give retail's set-up order at both calls (58). With the memset workspace on top: 957/957, aligned 8,
// only the state-one pop left (positional 134). On that base, the pop-or-stamp-first order of the
// state-one, kind-2 failure, choice failure and next-handler failure arms (16 combinations tried, 9
// measured): the state-one arm splits as retail only when another arm fuses or a choice-failure pop
// splits too (43-137); stamping through the handler table in each arm 81-314; `--level`, `level = level - 1`,
// kind-2 state store first and the else-if chain 134 each.
// Round 17 (memset workspace, 957/957): only the state-one pop differs (aligned diff 3); the lower
// level computed before the stamp and assigned after 134; the state-one stamp through the table 134;
// state-one/kind-2/choice as an if/else chain with the kind-2 test inverted (failure arm first) 119
// (state-one now as retail, kind-2 failure fused); on that, the workspace declared at the loop top 119.
// DL199: Shared parent-level short/int/long/dword gives 321/285/285/285; with the library
// workspace clear 330/292/292/292. The common value changes loop and frame allocation; original
// 0x4c frame, six pops and aggregate workspace restored. State-one split decrement and zero-fill
// remain.
// Lane 14 r20: the workspace cleared by memset alone 134 (957/957, aligned 8: only the state-one
// pop); with the state-one pop first 137, the kind-2 pop first 134, the choice pop first 46 (aligned
// 8); a grid of 31 stamp/pop orders over the state-one, kind-2, choice and next-handler arms 43-137
// (state-one, kind-2 and choice pops first 43, all four 45); the state-one / kind-2 / rest as an
// if/else chain 119 with retail's stamp-first choice arm (only the kind-2 pop differs), 29 with the
// choice pop first (kept); on the chain the six kind-2 statement orders 29-46/119-134, the
// next-handler pop first 31, the not-enabled arm chained too 29, only the kind-2 arm chained 29, the
// kind-2 check inverted 29/119, `continue` kept in the kind-2 arm (with or without the state-one
// one) 46; `while (level >= 0)` 206 (rotated). Of the state-one and kind-2 pops exactly one fuses:
// the arm that falls through to the loop end fuses, and when both `continue` the state-one one
// does; retail splits both.
// Round 127 (lane 4), on the kept body (every pop but the state-one arm's already as retail: the
// shared enabled-failure and choice==stamp arms `dec [mem]`, kind-2, level++ and next-handler arms
// load/dec/store): `level` declared before the owner lookup 60; `level` set to zero just before
// the slot loop 308 (held in a register); memset workspace alone 134 (957/957, the pop's offset
// runs to the end); `level = (short)(level - 1)` in the state-one arm 58 (same code); its stamp
// through g_46eeb8[type] 58 (same code).
// Round 46 (lane 9): with the memset workspace (957/957, 134) only the state-one arm differs. Ours cross-jumps
// the three enabled-test failures into that arm's own `dec [mem]` (the arm is `mov [esi+8], edx` then the
// shared pop), where retail sends them to the fallback-failure pop (0x1a7e63) and keeps a separate state-one
// arm whose load is scheduled above the stamp store. On memset: state-one pop first 137 (both arms fused, the
// enabled failures then share kind-2's pop), plus kind-2 pop first 131 (state-one fused, kind-2's level store
// before its state store); a `usable` flag for the fallback call tested with the enabled test 223 (not
// threaded: the handler/slot lines sit between), 209 without memset; tested alone right after the join 134
// (threaded, identical code); tested with the enabled test before the handler/slot lines 169 (the fallback
// failure then also jumps into the state-one arm, and the slot select moves after the test). With both arms
// pop first (131) the enabled failures do jump to the fallback-failure pop as in retail; kind-2 fused only
// when it became the host (the host is chosen first, its `level--` then fuses). State-one / kind-2 / choice as
// an if/else chain on memset 119 (state-one split as retail, kind-2 hosts and fuses); the enabled arm in the
// chain too 119 (identical).
// Lane 16 r48 (fast; aligned difflib counts in brackets): the full grid of pop/stamp orders on the
// kept chain body (state-one 2 x kind-2 failure 6 x choice 2 x next-handler 2, 48 forms) 29-137, none
// below the kept 29 [8]: the kind-2 pop splits as retail only with `level--` before its stores, and then
// the state-one pop fuses (best 43 [10], kind-2 store order then stamp/level/state for retail's
// stamp/state/level); one shared `level--` after the state-one/kind-2 chain 29 (same code);
// `level = level - 1` and `level = (short)(level - 1)` in the kind-2 arm 29/29.
// Round 52 (harness exact once 0x1a7ad0 has several callers): the kind-2 failure `level--` fuses in all six
// statement orders with either choice-failure order (aligned 3-10); real: stamp, state, pop with retail's
// stamp-first choice arm 119 (only the kind-2 pop differs, 2 bytes short); pop, stamp, state 46; the
// kind-2 test inverted with the failure statements inside it 29 (identical); success leaving by `break`
// 29 (identical); the check result in a `started` bool 29 (identical). `level = level - 1` there and a
// `continue` after the failure statements: identical / 42 positional.
// Lane 16 r50 (fast): source permuter, 66 tries in 600 s, best 29; the choice==g_46fbe4 arm as an if/else
// without `continue` (the rest of the handler code in its else), stamp first 119 (the choice pop fuses as
// retail, but the enabled failures cross-jump into a fused state-one pop again), pop first 29 (same code);
// on the stamp-first form the kind-2 failure orders stamp/pop/state 134, state/stamp/pop 119,
// state/pop/stamp 133, pop/stamp/state 134, pop/state/stamp 133.
// Round 22 (lane 18): the loop-head flag as `if (level < 0) return;` 206 (rotated); the choice-pop and
// next-handler-failure arms as one shared stamp/pop tail 321 (880 bytes); the start callback called through
// the handler field (no local) 29 (same code); the first enabled test through slot_type_enabled 155 (its
// `(word >> n) & 1` bit test differs, but kind-2's pop splits as retail). LEAD: an inline bool function of
// slot_type_enabled's shape with this file's `word & (1 << n)` test, for the first enabled test, splits both
// the state-one and kind-2 pops as retail (150); with the stamp-first choice arm too 143 (966 bytes) and only
// the enabled failure differs: a fall-through `dec [mem]; jmp` after the third test, where retail's three
// tests jump to the fallback pop. On that: if/return, result-bool and preset bodies 143-150 (the last test's
// polarity never changes); the not-enabled pop chained into the dispatch 143; positive dispatch
// `if (enabled) { chain } else level--;` 134 (the failures jump into the state-one arm, which fuses), with
// `continue` in the state-one arm, the kind-2 arm, the else or both arms 119/134/119/134; state-one and
// kind-2 pops first with the stamp-first choice arm 131 (macro) / 38 (helper, 961 bytes);
// `usable = function_1a80e0(...)` feeding one `level--` at the loop end 159 (it merges into kind-2's tail);
// a goto from the enabled failure to the fallback pop 53 (the shared pop is laid out after the enabled tests).
// @retail 0x1a7cc0
void __stdcall function_1a7cc0(long actor_index)
{
	s_actor_view *owner = actor_get(actor_index);
	short level = 0;

	g_46f348++;
	function_1a7b30(actor_index, (s_flag_bits *)g_557c40);
	g_4ee4ec = function_1a7ad0(actor_index);
	memset(g_4f0a4c, 0, sizeof(g_4f0a4c));
	for (short i = 0; i < 3; i++)
	{
		s_slot_entry *entry = &((s_slot_owner_entry *)owner)->entries[i];

		if (entry->type != NONE)
		{
			if (!(g_4f0a4c[entry->field4 >> 5] & (1 << (entry->field4 & 0x1f))))
			{
				g_4f0a4c[entry->field4 >> 5] |= 1 << (entry->field4 & 0x1f);
				g_4f09c8[entry->field4] = 0;
			}
			s_candidate_entry *candidate = &g_4ee4f0[g_4f09c8[entry->field4]].entries[entry->field4];
			candidate->kind = entry->field2;
			candidate->key = entry->field6;
			candidate->node.key = entry->type;
			candidate->node.field_2 = true;
			candidate->node.unknown4 = NONE;
			candidate->node.unknown8 = 0.0f;
			candidate->node.next = NULL;
			g_4f09c8[entry->field4]++;
		}
	}
	if (owner->unknown222)
		g_46eeb8[0x38]->unknown8 = g_46f348;

	for (;;)
	{
		bool valid = false;
		if (level >= 0)
			valid = true;
		if (!valid)
			break;
		s_slot *slot = &owner->slots[level];
		short type = slot->type;

		if (!ACTION_ID_VALID(type))
		{
			type = owner->unknown007 ? 0x72 : 1;
			if (!function_1a80e0(actor_index, type, NULL, 0))
			{
				level--;
				continue;
			}
		}

		s_slot_handler *handler = g_46eeb8[type];
		s_slot *current = level < 4 ? slot : NULL;

		if (!ACTION_HANDLER_ENABLED(type))
		{
			level--;
			continue;
		}
		if (current->state == 1)
		{
			handler->unknown8 = g_46f348;
			level--;
		}
		else if (handler->kind == 2)
		{
			t_slot_start check = (t_slot_start)((s_slot_handler_2 *)handler)->update40;

			if (!check || check(actor_index, current))
			{
				function_25aba0(actor_index);
				function_1a6f70(actor_index);
				return;
			}
			handler->unknown8 = g_46f348;
			current->state = 2;
			level--;
		}
		else
		{
			s_slot data;
			memset(&data, 0, sizeof(data));
			short choice = ((short (__stdcall *)(long, short, s_slot *))((s_slot_handler_1 *)handler)->choose)(actor_index, level, &data);

			if (choice == g_46fbe4)
			{
				level--;
				handler->unknown8 = g_46f348;
				continue;
			}

			short next_type;
			s_slot_handler *next_handler;

			if (choice == g_46fbe8)
			{
				next_type = owner->slots[level + 1].type;
				next_handler = g_46eeb8[next_type];
			}
			else
			{
				next_type = choice;
				next_handler = g_46eeb8[choice];
			}
			if (next_handler->unknown8 == g_46f348 || (next_handler->mask & g_4ee4ec) != g_4ee4ec ||
				!(g_557c40[next_type >> 5] & (1 << (next_type & 0x1f))))
			{
				handler->unknown8 = g_46f348;
				level--;
				continue;
			}
			if (choice == g_46fbe8)
			{
				level++;
				continue;
			}

			short local_0 = actor_behavior_get_insert_level(choice, (s_slot_owner_entry *)owner, level);

			if (local_0 != NONE && function_1a80e0(actor_index, choice, &data, local_0))
			{
				level = local_0;
				continue;
			}
			next_handler->unknown8 = g_46f348;
			(local_0 < 4 ? &owner->slots[local_0] : NULL)->state = 2;
		}
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
	short probe;
	short score;
	short count = 0;
	short index = NONE;
	long local_0 = actor_index;
	s_slot *slot = actor_slot_get(local_0, level);
	short state = slot->unknown4;
	short result = function_1a7030(local_0, level, argument, &probe);

	if (result == g_46fbec)
		result = g_46fbe4;
	else if (result == g_46fbe4)
	{
		bool valid;
		s_action_node **nodes = function_1a73c0(local_0, slot->type, &valid, &count);

		if (state < count - 1)
			result = function_1a75f0(local_0, nodes, state == NONE ? 0 : state, count, slot, argument, NONE, valid, state != NONE, &score, &index);
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
		result = g_46fbe4;
	else if (result == g_46fbe4)
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

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
bool function_e4050(long object_index);
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void havok_component_rigid_body_point_velocity_get(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f *velocity);

PRIVATE __forceinline void function_1a68a2(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
    arg_2->i = arg_0->i * arg_1;
    arg_2->j = arg_0->j * arg_1;
    arg_2->k = arg_0->k * arg_1;
}

// score 2 (was 2): the returned point is copied from EAX before the body load;
// retail loads the body and then copies the point from ESI. The frame and projection agree.
// Sampling name 18; vector names 18; vectors declared first 18; const unit 18;
// saved body count 124; workspace memcpy 18; indexed output 23; const component 18.
// Support handle 18; nullable component 110; body read first 30;
// valid body interval 85; entry workspace 18; unit-scope workspace 18;
// const support 18; unsigned object handle 18. Position query noinline 18;
// returned point with body index 2; reassigned point pointer 128; output pointer 18;
// file-scope workspace 18; direct body reads 121; support lookup noinline 18;
// returned point with output pointer 2; target noinline with returned point 2.
// const support 18; unsigned object handle 18.
// Round 109: ordinary scale inline 18; scale pointer result 18; three scale formal orders 18;
// scale input reference 18; point-query noinline 18; velocity-query noinline 18.
// Round 277: unit before type 5; eligibility phases, separate support/component guards 2 each;
// exhaustive clamp 2; body-first clamp 9; body-home names and read-only body values 2 each.
// Round 127: validated query index 99; sampled-point result 128; both 136;
// enclosing point lifetime 128; branch-local last body 2; last body with query index 99;
// named validation 2; validation with query index 99.
// Later: const point 18; returned reference 128;
// address of returned point/direct body 2/2; clamped body 99; early normal 2; workspace fields 5;
// samples name 2; early normal with const point/direct body 18/18.
// Round 88: BLOCKED-BY 0x1d0a20 (its parameter order: with velocity before point, retail's
// `mov ecx, esi` follows; its file is held this round). Statement point query with the
// address 18; point pointer local 18; body re-read at the call 2; statement query plus body
// re-read 18; velocity pointer local 2.
// Round 103: the existing component getter and rigid-body count in place of the hand-written lookup 2 (kept,
// same code); the clamp compare operands swapped 3; the object handle read through its own address for the
// point query 2; the clamp as matched 0x182020's if/else 2; a named component index 2. Still BLOCKED-BY
// 0x1d0a20 (its definition file and the held src/impacts.cpp calls).
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
					s_havok_component *component = havok_component_get(support->component_index);
					long body_index = unit->body_index;
					long clamped = body_index < 0 ? 0 : body_index > component->rigid_bodies.size - 1 ? component->rigid_bodies.size - 1 : body_index;
					if (clamped == body_index)
					{
						havok_component_rigid_body_point_velocity_get(body_index, component, function_b9dd0(object_index, &local_3.field_c), &local_3.field_18);
					}
				}
			}
			function_ba1d0(object_index, velocity, NULL);
			vector3f relative;
			vector3d_from_points3d((point3f const *)&local_3.field_0, (point3f const *)velocity, &relative);
			vector3f const *normal = &unit->normal;
			real projection = dot3f(normal, &relative);
			vector3f projected;
			function_1a68a2(normal, projection, &projected);
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
	byte unknown08[4];
	point3f point;
	byte flags;
	byte unknown19[3];
	real field_1c;
	real field_20;
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
	bool result;
	*has_hostile = false;
	if (object_index != excluded_index)
		result = true;
	else
		result = false;
	if (result)
	{
		s_object_header *header = OBJECT_HEADER(object_index);
		if ((1 << header->type) & 3)
		{
			s_target_link_view *object = (s_target_link_view *)header->object;
			s_target_link_definition *definition;
			s_target_link_definition **local_2 = &definition;
			*local_2 = (s_target_link_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
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


#pragma optimize("g", off)
PRIVATE __forceinline s_object_header *function_1a5b01(long arg_0)
{
    long local_0 = arg_0 & 0xffff;
    byte *local_1 = (byte *)g_4e0300->data;
    return (s_object_header *)(local_1 + local_0 * sizeof(s_object_header));
}
#pragma optimize("", on)

// @retail 0x1a5b00
bool function_1a5b00(long object_index, short team, bool *has_hostile)
{
	long object_offset = (object_index & 0xffff) * sizeof(s_object_header);
	s_target_seats_definition *definition = (s_target_seats_definition *)g_4e3b44[function_1a5b01(object_index)->object->definition_index & 0xffff].bytes;
	bool result = false;
	s_unit_child_iterator iterator;
	*has_hostile = false;
	iterator.object_index = object_index;
	iterator.unit_index = NONE;
	iterator.seat_index = NONE;
	iterator.next_index = ((s_target_occupant_view *)function_1a5b01(object_index)->object)->first_child_index;
	while (function_d05c0(&iterator))
	{
		s_target_occupant_view *occupant = (s_target_occupant_view *)function_1a5b01(iterator.unit_index)->object;
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

// score 36 (was 186): frame/length agree; geometry-call convention and angle/failure tails differ.
// Axis name 186; result first 220; query result 195; first-bit assignment 186; size optimization 348.
// Depth zero 386; depth one 186; success return 203; clamped input on current frame 216; word result 227.
// object register, geometry-call convention and failure/angle tails differ;
// axis name 186; result first 220; geometry result guard 195; masked first bit 186; size optimization 348.
// Failure to common exit 98; signed byte result 196; second-bit results 178; angle result 201; radius snapshot 209.
// Exit plus second-bit results 199; exit plus scalar clamp 42.
// Clearing cases first 36; one source flag read 108; alternate before normalization 45;
// shared dot operands reversed 42; global optimization off 361.
// score 18 (was 36): geometry-call convention and exchanged failure exits remain.
// Cosine declared early 36; clamp argument 18 kept; callee-only clamp 76; shared direction/point/flags 67/102/41;
// alternate after angle 88; default-success result 240.
// Frame and length agree; geometry argument register and clamp/failure tails
// remain. Trials: byte result 101; lower test operands 36; separate lower angle 92; native facing
// copy 36; geometry success guard 218; first clearing cases 42.
// score 11 (was 18): geometry-call convention remains; type rejection to the shared exit 11 kept.
// Query result exit 219; type result 253; branch result lifetime 220; output initialization schedule 22; zero order 20; common success 199; object role name 18; flat type rejection 11; query false return 216.
// geometry-call convention and exchanged failure exits remain; native
// flag/limit/adjacent clears 216/152/199; direction memcpy 18; clear plus copy 216/152/199;
// branch-zero clear 199.
// score 11 (was 11): query stack/register boundary remains; object-last interface 15;
// object-last stdcall/inline/both 15/15/15; original inline/stdcall 11/11;
// reordered query outcome/entry axis 126/15. All query interfaces restored.
// frame 0x10 and length agree; geometry query takes a stacked object instead of ECX.
// Query noinline 11; combined rejection/acceptance 18 each; output/marker references 11 each;
// named type mask 11; result after initialization 16; direction producer home 196;
// unconditional successful tail 11. Experimental declaration and receiver aliases restored.
// Object-second/third query interfaces 12/13; twice-used query index 11;
// read-only object view/pointer/index 11/11/11; unsigned query index 11;
// candidate-index query 11. Original interfaces and values restored.
// @retail 0x1a5e40
bool function_1a5e40(long object_index, bool alternate, long marker_index, s_target_marker const *marker,
	transform4x3f const *first, transform4x3f const *second,
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
			goto done;
		if (!function_1a62e0(object_index, origin, direction, &candidate->point))
			goto done;
		s_candidate_unit_definition *definition = (s_candidate_unit_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
		if (definition->flags & 1)
			candidate->flags |= 1;
		else
			candidate->flags &= ~1;
		if (!(definition->flags & 6))
			candidate->flags &= ~2;
		else
			candidate->flags |= 2;
		candidate->maximum_distance = definition->maximum_distance;
		candidate->radius = object->radius;
		candidate->maximum_facing_angle = 0.0f;
		result = true;
	}
	else
	{
		if (second)
		{
			vector3f axis;
			vector3d_from_points3d(&first->position, &second->position, &axis);
			function_1a6340(&axis, direction, &candidate->point, origin, &first->position, marker->radius);
		}
		else
			function_1a65b0(direction, &candidate->point, &first->position, origin, marker->radius);
		if (marker->flags & 1)
			candidate->flags |= 1;
		else
			candidate->flags &= ~1;
		if (!(marker->flags & 0x12))
			candidate->flags &= ~2;
		else
			candidate->flags |= 2;
		candidate->maximum_distance = marker->maximum_distance;
		candidate->radius = marker->radius;
		candidate->maximum_facing_angle = marker->maximum_facing_angle;
		candidate->facing = first->forward;
		result = true;
	}
	if (result)
	{
		vector3d_from_points3d(origin, &candidate->point, &candidate->offset);
		candidate->direction = candidate->offset;
		candidate->distance = function_30bf0(&candidate->direction);
		candidate->alternate = alternate;
		real cosine = dot3f(&candidate->direction, direction);
		candidate->angle = function_50650(-1.0f > cosine ? -1.0f : cosine > 1.0f ? 1.0f : cosine);
	}
done:
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
	if (result)
	{
		if (scale != 0.0f)
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
	}
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

PRIVATE __forceinline s_target_link_definition *function_1a5c01(long object_index)
{
	s_target_link_view *object = (s_target_link_view *)OBJECT_HEADER(object_index)->object;
	return (s_target_link_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
}

// @retail 0x1a5c00
long function_1a5c00(s_sort_weight_view const *weights, long object_index, bool alternate,
	point3f const *origin, vector3f const *direction, long count, s_sort_candidate_view *candidates)
{
	s_target_link_view *object = (s_target_link_view *)OBJECT_HEADER(object_index)->object;
	byte *definition = g_4e3b44[object->definition_index & 0xffff].bytes;
	real scale = 1.0f;
	if (((1 << object->field_aa) & 3) &&
		(bool)((function_1a5c01(object_index)->field_bc >> 19) & 1))
		scale = ((s_candidate_scale_globals *)g_4e034c)->field_f4->field_8;
	if (count < 64)
	{
		s_sort_candidate_view *first_candidate = &candidates[count];
		if (function_1a5e40(object_index, alternate, NONE, NULL, NULL, NULL,
			origin, direction, first_candidate) && function_1a60f0(first_candidate, weights, scale))
			count++;
	}
	long marker_tag = *(long *)(definition + 0x38);
	if (marker_tag != NONE)
	{
		s_target_marker_table *table = (s_target_marker_table *)g_4e3b44[marker_tag & 0xffff].bytes;
		for (long marker_index = 0; marker_index < table->count; marker_index++)
		{
			s_target_marker *marker = &table->markers[marker_index];
			long name = marker->name;
			s_object_marker markers[2];
			long marker_count = function_b8d30(object_index, name, markers, 2, false);
			if (marker_count != 0)
			{
				s_object_marker *second = marker_count == 2 ? &markers[1] : NULL;
				if (count < 64 && function_1a5e40(object_index, alternate, marker_index, marker,
					&markers[0].matrix, second ? &second->matrix : NULL,
					origin, direction, &candidates[count]) &&
					function_1a60f0(&candidates[count], weights, marker->field_10 * scale))
				{
					count++;
				}
			}
		}
	}
	return count;
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
	if (!(local_3 > 0.0f && local_4 > 0.0f))
		goto done;
	{
		real local_5 = sin(local_4);
		real local_6 = cos(local_4);
		long local_7 = function_14a6d0(arg_1, local_6, arg_2, arg_3, local_3, local_5, 0x200, local_1, NULL);
		long local_8 = function_bb430(1, local_7, local_1, 0x800, local_2);
		s_record_pool_iterator local_9;
		g_4de2f8 = true;
		local_9.data = g_4e8c24;
		local_9.datum_index = NONE;
		local_9.index = NONE;
		s_player_view *local_11;
		while ((local_11 = (s_player_view *)data_iterator_next_calling(&local_9)) != 0)
		{
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
				long local_16 = local_13 & 0xffff;
				if (g_4de300[local_16] != g_4de2fc)
				{
					g_4de300[local_16] = g_4de2fc;
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
	done:
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

/* the target's radius is passed but not used */
// @retail 0x1a67f0
byte function_1a67f0(real radius, point3f const *arg_0, point3f const *arg_1, long arg_2, long arg_3)
{
	s_1a67f0 local_0;
	local_0.field_24 = NONE;
	long local_1 = function_1a67f1(arg_3);
	long local_2 = function_1a67f1(arg_2);
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
				for (long local_5 = 0; local_5 < local_4; local_5++)
				{
					s_sort_candidate_view *local_8 = &local_1[local_5];
					if (function_1a67f0(local_8->radius, arg_1, &local_8->point, arg_3, local_8->object_index))
					{
						*arg_5 = local_1[local_5];
						local_0 = true;
						break;
					}
				}
			}
		}
	}
	return local_0;
}

// @retail 0x1a6a80
bool function_1a6a80(s_object_marker_target const *target, point3f const *origin, vector3f const *direction,
	long excluded_index, short team, s_sort_candidate_view *candidate)
{
	bool result = false;

	if (target->field_1c > 0.0f || target->field_20 > 0.0f)
	{
		long object_index;
		long marker_index;
		s_target_marker *definition;
		transform4x3f first;
		transform4x3f second;
		bool has_second;

		if (function_1a6bf0(target, &object_index, &marker_index, &definition, &first, &second, &has_second))
		{
			bool has_hostile = false;

			if (function_1a5910(object_index, excluded_index, team, &has_hostile) && function_1a58b0(object_index))
			{
				if (function_1a5e40(object_index, has_hostile, marker_index,
					marker_index != NONE ? definition : NULL,
					marker_index != NONE ? &first : NULL,
					marker_index != NONE && has_second ? &second : NULL,
					origin, direction, candidate))
				{
					if (!has_hostile)
					{
						candidate->score = target->field_1c;
						candidate->secondary_score = target->field_20;
					}
					else
					{
						candidate->score = 0.0f;
						candidate->secondary_score = 0.0f;
					}
					candidate->scaled_score = 0.0f;
					if (function_1a67f0(candidate->radius, origin, &candidate->point, excluded_index, candidate->object_index))
						result = true;
				}
			}
		}
	}
	return result;
}

struct s_186ab2
{
	long field_0;
	long field_4;
	long field_8;
	byte field_c[0xc];
	short field_18;
	byte field_1a[2];
	real field_1c;
	real field_20;
};

struct s_223241
{
	byte field_0[0x358];
};

extern s_223241 g_4e9c8c[4];

struct s_1a4900
{
	byte field_0[0x13c];
	long field_13c;
};

long function_155760(long index);
long function_cb6d0(long unit_index);
bool function_cb810(long unit_index, vector3f *vector);
long function_baf40(long object_index);

/* the yaw and pitch rates toward the target chosen for a player's unit */
// @retail 0x1a4900
void __stdcall function_1a4900(long arg_0, s_186ab2 *arg_1, vector3f *arg_2)
{
	short local_0 = (short)function_155760(arg_0);

	memset(arg_2, 0, sizeof(*arg_2));
	arg_1->field_1c = 0.0f;
	arg_1->field_20 = 0.0f;
	arg_1->field_0 = NONE;
	arg_1->field_4 = NONE;
	arg_1->field_8 = NONE;
	arg_1->field_18 = 0;
	if (local_0 == 0 || local_0 == 1)
	{
		long local_19 = NONE;

		if (arg_0 != NONE)
			local_19 = g_4e8c20->entries[arg_0];

		s_player_view *local_1 = &((s_player_view *)g_4e8c24->data)[local_19 & 0xffff];
		long local_2 = function_cb6d0(local_1->unit_index);
		short local_20 = NONE;

		if (arg_0 != NONE)
			local_20 = g_4ed284->entries[arg_0].index;

		s_sort_weight_view local_3;

		if (function_1a50a0(local_2, local_20, &local_3))
		{
			point3f const *local_4 = NULL;

			if (arg_0 != NONE && g_4686c4 != NONE)
				local_4 = (point3f const *)&g_4e9c8c[arg_0];

			long local_5 = function_baf40(local_2);
			long local_21 = NONE;

			if (arg_0 != NONE)
				local_21 = g_4e8c20->entries[arg_0];

			s_view_angles const *local_7 = &g_4ed284->entries[arg_0].angles;
			real local_8 = (real)cos(local_7->pitch);
			s_player_view *local_6 = &((s_player_view *)g_4e8c24->data)[local_21 & 0xffff];
			vector3f local_9;

			local_9.i = (real)cos(local_7->yaw) * local_8;
			local_9.j = (real)sin(local_7->yaw) * local_8;
			local_9.k = (real)sin(local_7->pitch);
			if (local_6->unit_index != NONE)
				function_cb810(local_6->unit_index, &local_9);

			s_sort_candidate_view local_10;

			if (function_1a5370(&local_3, local_4, &local_9, local_5, local_1->field_c0, &local_10))
			{
				arg_1->field_1c = local_10.score;
				arg_1->field_20 = local_10.secondary_score;
				arg_2->k = (local_3.flags & 2) ? 1.0f : local_10.scaled_score;

				vector3f local_11;
				vector3f local_12;

				function_1a68a0(local_1->unit_index, &local_11);
				function_1a68a0(local_10.object_index, &local_12);

				vector3f const *offset = &local_10.offset;
				vector3f relative;

				vector3d_from_points3d((point3f const *)&local_11, (point3f const *)&local_12, &relative);

				real local_16 = offset->i * offset->i + offset->j * offset->j;

				if (local_16 > 0.0001f)
				{
					real local_17 = (real)sqrt(local_16);

					real planar_dot = offset->i * relative.i + offset->j * relative.j;

					arg_2->i = (offset->i * relative.j - offset->j * relative.i) / local_16;
					arg_2->j = (relative.k * local_17 - planar_dot / local_17 * offset->k) /
						(offset->k * offset->k + local_16);
				}
				else
				{
					arg_2->i = 0.0f;
					arg_2->j = 0.0f;
				}
				if (arg_1->field_1c > 0.0f || arg_1->field_20 > 0.0f || local_10.alternate)
				{
					arg_1->field_0 = local_10.object_index;
					arg_1->field_4 = local_10.marker_index;
				}
				if (arg_1->field_0 != NONE)
				{
					s_object_header *local_18 = OBJECT_HEADER(arg_1->field_0);

					if ((1 << local_18->type) & 3)
						arg_1->field_8 = ((s_1a4900 *)local_18->object)->field_13c;
				}
			}
		}
	}
}

struct s_entry_pair;
#include "unknown_11cc90.h"
bool function_11d3b0(vector3f const *a, vector3f *out, vector3f const *b, real sine, real cosine);
short function_1557c0(long unit_index, point3f *position, vector3f *forward);
long function_baf80(long object_index);

/* the vector a fraction t of the way from a to b */
static inline void vector3f_interpolate(vector3f const *a, vector3f const *b, real t, vector3f *result)
{
	real s = 1.0f - t;

	result->i = a->i * s + b->i * t;
	result->j = a->j * s + b->j * t;
	result->k = a->k * s + b->k * t;
}

struct s_1a4c00
{
	byte field_0[0x240];
	char field_240;
};

struct s_1a4c01
{
	byte field_0[8];
	point3f point;
	byte field_14[0x24 - 0x14];
	short field_24;
	byte field_26[0x5c - 0x26];
};

// @retail 0x1a4c00
bool function_1a4c00(long player_index, s_entry_pair *pair, bool primary, vector3f *direction, point3f *point)
{
	bool result = false;
	s_object_marker_target const *target = (s_object_marker_target const *)pair;
	s_player_view *player = &((s_player_view *)g_4e8c24->data)[player_index & 0xffff];
	long unit_index = function_cb6d0(player->unit_index);
	long mode = g_4e6948->mode;

	if (mode >= 4 && mode <= 5 && (target->flags & 2))
	{
		vector3d_from_points3d(point, &target->point, direction);
		function_30bf0(direction);
		result = true;
		goto done;
	}

	{
		real distance;
		s_sort_weight_view weights;

		short selection = ((s_1a4c00 *)OBJECT_HEADER(unit_index)->object)->field_240;
		if (!function_1a50a0(unit_index, selection, &weights))
			goto done;

		point3f position;
		vector3f forward;

#pragma inline_depth(0)
		function_1557c0(player->unit_index, &position, &forward);
#pragma inline_depth()

		vector3f target_direction = *direction;
		real fraction = 0.0f;
		long target_index = NONE;

		{
			s_sort_candidate_view candidate;

			if (function_1a6a80(target, &position, &forward, player->unit_index, player->field_c0, &candidate) && !candidate.alternate)
			{
				vector3d_from_points3d(point, &candidate.point, &target_direction);
				if (function_30bf0(&target_direction) == 0.0f)
					target_direction = *direction;
				fraction = primary ? candidate.score : candidate.secondary_score;
				target_index = candidate.object_index;
			}
		}
		vector3f aim_direction;
		{
			s_1a4c01 collision;

			collision.field_24 = NONE;
			{
				point3f unit_position;

				function_b9dd0(unit_index, &unit_position);
				real dx = unit_position.x - position.x;
				real dy = unit_position.y - position.y;
				real dz = unit_position.z - position.z;
				distance = (real)sqrt(dz * dz + dy * dy + dx * dx);
			}
			{
				vector3f offset = forward;

				function_30bf0(&offset);
				offset.i *= distance;
				offset.j *= distance;
				offset.k *= distance;
				position.x += offset.i;
				position.y += offset.j;
				position.z += offset.k;
			}

			vector3f ray;

			ray.i = forward.i * 128.0f;
			ray.j = forward.j * 128.0f;
			ray.k = forward.k * 128.0f;
			function_1697c0(0x2480000f, &position, &ray, function_baf80(unit_index), NONE, (s_collision_result_1697c0 *)&collision);
			vector3d_from_points3d(point, &collision.point, &aim_direction);
		}
		if (function_30bf0(&aim_direction) == 0.0f)
			aim_direction = *direction;

		bool accepted;
		{
			vector3f result_direction;
			vector3f_interpolate(&aim_direction, &target_direction, fraction, &result_direction);

			real length_squared = length_sq3f(&result_direction);

			if (length_squared != 0.0f)
			{
				scale_vector_by_double(&result_direction, 1.0f / sqrt(length_squared), &result_direction);
			}
			if (function_1a6d30(player_index))
			{
				accepted = !function_11d3b0(&result_direction, direction, direction, (real)sin(weights.outer_distance), (real)cos(weights.outer_distance));
			}
			else
			{
				*direction = result_direction;
				accepted = true;
			}
		}
		if (fraction >= 1.0f && accepted)
		{
			result = true;
			player->field_174 = target_index;
			player->field_178 = g_510c54->game_time;
		}
	}
done:
	return result;
}
