// @flags /O2 /Gr
/* UNKNOWN_290C80.CPP: the ai's iterator over a chain of objects (an outside
   function lane I's handlers call) */

#include "unknown_11c920.h"
#include "unknown_2551c0.h"
#include "globals.h"
#include "object_iterator.h"
#include "unknown_123b30.h"
#include <string.h>

struct s_290c50_iterator
{
	long unknown00;
	s_record_pool_iterator records;
	long object_index;
};

// @retail 0x290c50
void function_290c50(s_290c50_iterator *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->records.data = g_5044c8;
		iterator->records.index = NONE;
		iterator->records.datum_index = NONE;
		iterator->object_index = NONE;
	}
}

/* the ai data of an object, at the object's ai_offset */
struct s_object_ai_data
{
	union
	{
		word flags;
		struct { word attached : 1; word other_flags : 15; };
	};
	byte unknown02[2];
	long index04;
	long index08;
	long next_object_index;
	long index10;
	point3f position14;
	long index20;
	byte unknown24[0x4c - 0x24];
	short countdown;
	byte unknown4e[2];
	long index50;
	byte unknown54[2];
	bool flag56;
	bool position_pending;
	point3f position58;
	short index64;
	short count66;
	byte unknown68[0x74 - 0x68];
	point3f position74;
};

// @retail 0x28da50
void function_28da50(s_object_ai_data *data)
{
	data->flags = 0;
	data->index04 = NONE;
	data->index10 = NONE;
	data->next_object_index = NONE;
	data->index20 = NONE;
	data->index08 = NONE;
	data->position14 = *g_468788;
	data->index50 = NONE;
	data->flag56 = false;
	data->count66 = 0;
	data->index64 = NONE;
	data->position74 = *g_468788;
}

point3f *function_b9dd0(long object_index, point3f *result);

// @retail 0x28f890
void function_28f890(long object_index, s_object_ai_data *data)
{
	s_handler_object_view *object = handler_object_get(object_index);
	function_b9dd0(object_index, &data->position14);
	data->index20 = NONE;
	if (*(long *)((byte *)object + 0x14) != NONE)
		data->attached = true;
	else
		data->attached = false;
}

inline s_object_ai_data *object_ai_data(s_handler_object_view *object)
{
	return object->flags134 ? NULL : (s_object_ai_data *)((byte *)object + object->ai_offset);
}

__forceinline s_object_ai_data *object_ai_data_checked(s_handler_object_view *object)
{
	s_object_ai_data *result;
	if (!object->flags134)
		result = (s_object_ai_data *)((byte *)object + object->ai_offset);
	else
		result = NULL;
	return result;
}

__forceinline s_handler_object_view *next_object_290040(s_ai_object_iterator *iterator)
{
	s_handler_object_view *object = NULL;
	if (iterator->next_index != NONE)
	{
		object = handler_object_get(iterator->next_index);
		iterator->index = iterator->next_index;
		s_object_ai_data *data;
		if (!object->flags134 && (data = (s_object_ai_data *)((byte *)object + object->ai_offset)) != NULL)
			iterator->next_index = data->next_object_index;
		else
			iterator->next_index = NONE;
	}
	return object;
}

// @retail 0x28fdf0
void function_28fdf0(long perception_index)
{
	long object_index = perception_get(perception_index)->object_index;
	while (object_index != NONE)
	{
		s_object_ai_data *data = object_ai_data(handler_object_get(object_index));
		if (!data)
			break;
		object_index = data->next_object_index;
		data->index20 = NONE;
	}
}

// @retail 0x290bf0
void function_290bf0(long perception_index, short team)
{
	long object_index = perception_get(perception_index)->object_index;
	while (object_index != NONE)
	{
		s_handler_object_view *object = handler_object_get(object_index);
		s_object_ai_data *data = object_ai_data(object);
		if (!data)
			break;
		*(short *)((byte *)object + 0x12e) = team;
		object_index = data->next_object_index;
	}
}

// @retail 0x28fd90
void function_28fd90(long perception_index, long index)
{
	long object_index = perception_get(perception_index)->object_index;
	while (object_index != NONE)
	{
		s_object_ai_data *data = object_ai_data(handler_object_get(object_index));
		if (!data)
			break;
		if (data->index10 == index)
			data->index10 = NONE;
		if (data->index50 == index)
			data->index50 = NONE;
		object_index = data->next_object_index;
	}
}

void function_b7360(long object_index);

// @retail 0x28e2b0
void function_28e2b0(long perception_index, bool flag)
{
	struct s_flags
	{
		byte unknown00[0x10a];
		word bits0_4 : 5;
		word bit5 : 1;
		word bit6 : 1;
		word others : 9;
	};
	long object_index = perception_get(perception_index)->object_index;
	while (object_index != NONE)
	{
		s_handler_object_view *object = handler_object_get(object_index);
		s_object_ai_data *data = object_ai_data(object);
		if (!data)
			break;
		if (flag)
			((s_flags *)object)->bit6 = true;
		else
			((s_flags *)object)->bit5 = true;
		function_b7360(object_index);
		object_index = data->next_object_index;
	}
}

// @retail 0x290190
bool function_290190(long object_index, point3f *position)
{
	bool result = false;
	s_handler_object_view *object = handler_object_get(object_index);
	s_object_ai_data *data = object_ai_data_checked(object);
	if (data && data->position_pending)
	{
		*position = data->position58;
		data->position_pending = false;
		result = true;
	}
	return result;
}

struct s_290b90_entry
{
	long key;
	byte unknown04[0x48 - 4];
	long perception_index;
	short count;
	byte unknown4e[2];
};

s_290b90_entry *g_5044c4;

struct s_28e200
{
	byte field_0[4];
	long field_4;
	byte field_8[0xc];
	short field_14;
	byte field_16[2];
	long field_18;
	byte field_1c[0x15];
	bool field_31;
	byte field_32[2];
};

void function_26ae30(long arg_0);
void __stdcall function_b8540(long arg_0);
void __stdcall function_1e1a00(long arg_0, long arg_1);
bool __stdcall function_28e390(long arg_0, long arg_1, bool arg_2, bool arg_3);

// @retail 0x28e200
void __stdcall function_28e200(long arg_0)
{
	s_28e200 *local_0 = (s_28e200 *)perception_get(arg_0);
	if (local_0->field_14 > 0)
	{
		while (local_0->field_18 != NONE)
			function_28e390(arg_0, local_0->field_18, true, false);
	}
	if (local_0->field_4 != NONE)
	{
		s_handler_actor_view *local_1 = (s_handler_actor_view *)actor_get(local_0->field_4);
		if (local_1->perception_index == arg_0)
			local_1->perception_index = NONE;
	}
	if (local_0->field_31)
	{
		long local_2 = 0;
		do
		{
			if (g_5044c4[local_2].perception_index == arg_0)
				g_5044c4[local_2].perception_index = NONE;
			local_2++;
		} while (local_2 < 5);
	}
	record_pool_release(g_5044c8, arg_0);
}

// @retail 0x28e390
bool __stdcall function_28e390(long arg_0, long arg_1, bool arg_2, bool arg_3)
{
	bool local_2 = false;
	s_28e200 *local_0 = (s_28e200 *)perception_get(arg_0);
	long *local_1 = &local_0->field_18;
	while (*local_1 != NONE)
	{
		s_object_ai_data *local_3 = object_ai_data_checked(handler_object_get(*local_1));
		if (!local_3)
			break;
		if (*local_1 == arg_1)
		{
			*local_1 = local_3->next_object_index;
			*(byte *)((byte *)handler_object_get(arg_1) + 0x12c) &= 0xfe;
			local_2 = true;
			local_3 = object_ai_data_checked(handler_object_get(arg_1));
			if (arg_2)
			{
				function_26ae30(arg_1);
				function_b8540(arg_1);
			}
			if (local_3)
			{
				local_3->index08 = NONE;
				local_3->index04 = NONE;
				local_3->next_object_index = NONE;
			}
			local_0->field_14--;
			if (local_0->field_14 == 0 && arg_3)
			{
				if (local_0->field_4 != NONE)
					function_1e1a00(local_0->field_4, 0);
				function_28e200(arg_0);
			}
			break;
		}
		local_1 = &local_3->next_object_index;
	}
	return local_2;
}

// @retail 0x28d930
void function_28d930(void)
{
	g_5044c8 = data_new_inlined("swarm", 32, 0x34, 0, g_510c2c);
	g_5044c4 = (s_290b90_entry *)function_123d40(NULL, NULL, 0x190);
	for (long i = 0; i < 5; i++)
	{
		g_5044c4[i].count = 0;
		g_5044c4[i].key = NONE;
	}
}

// @retail 0x290b90
void function_290b90(long key)
{
	for (long i = 0; i < 5; i++)
	{
		if (g_5044c4[i].key == key)
		{
			if (g_5044c4[i].perception_index != NONE)
				*((byte *)perception_get(g_5044c4[i].perception_index) + 0x31) = false;
			g_5044c4[i].key = NONE;
			g_5044c4[i].perception_index = NONE;
			g_5044c4[i].count = 0;
		}
	}
}

/*	Detaches an actor from its perception entry and clears the
	corresponding AI-data link on associated objects. */
// @retail 0x28e160
void function_28e160(long actor_index)
{
	s_handler_actor_view *actor = (s_handler_actor_view *)actor_get(actor_index);
	if (actor->perception_index != NONE)
	{
		s_perception_datum *perception = perception_get(actor->perception_index);
		long perception_index = actor->perception_index;
		actor->perception_index = NONE;
		*(long *)((byte *)perception + 4) = NONE;
		s_ai_object_iterator iterator;
		iterator.next_index = perception_get(perception_index)->object_index;

		s_handler_object_view *object;
		while ((object = next_object_290040(&iterator)) != NULL)
		{
			s_object_ai_data *data = object_ai_data_checked(object);
			if (data)
				data->index04 = NONE;
		}
	}
}

// @retail 0x28e090
void __stdcall function_28e090(long actor_index, long perception_index)
{
	s_handler_actor_view *actor = (s_handler_actor_view *)actor_get(actor_index);
	s_perception_datum *perception = perception_get(perception_index);
	if (actor->perception_index != perception_index)
	{
		if (actor->perception_index != NONE)
			function_28e160(actor_index);
		long old_actor = *(long *)((byte *)perception + 4);
		if (old_actor != NONE)
			function_28e160(old_actor);
		actor->perception_index = perception_index;
		*(long *)((byte *)perception + 4) = actor_index;
		s_ai_object_iterator iterator;
		iterator.next_index = perception_get(perception_index)->object_index;
		s_handler_object_view *object;
		while ((object = next_object_290040(&iterator)) != NULL)
		{
			s_object_ai_data *data = object_ai_data_checked(object);
			if (data)
				data->index04 = actor_index;
		}
	}
}

struct s_28e5c0_object
{
	byte unknown00[0x12c];
	word bits0_4 : 5;
	word paused : 1;
	word bits6_15 : 10;
};

// @retail 0x28e5c0
void function_28e5c0(long object_index, s_object_ai_data *data)
{
	if (!data->position_pending)
	{
		s_28e5c0_object *object = (s_28e5c0_object *)object_get(object_index);
		if (!(bool)object->paused && data->countdown > 0)
			data->countdown--;
	}
}

// @retail 0x290c80
s_handler_object_view *function_290c80(s_ai_object_iterator *iterator)
{
	s_handler_object_view *object = NULL;

	if (iterator->next_index != NONE)
	{
		object = handler_object_get(iterator->next_index);
		iterator->index = iterator->next_index;

		s_object_ai_data *data;
		if (!object->flags134 && (data = (s_object_ai_data *)((byte *)object + object->ai_offset)) != NULL)
		{
			iterator->next_index = data->next_object_index;
		}
		else
		{
			iterator->next_index = NONE;
		}
	}

	return object;
}

// @retail 0x28d9d0
void function_28d9d0(void)
{
	s_type_f1af8e iterator;
	iterator.signature = 0x86868686;
	iterator.type_mask = 0x1000;
	iterator.flags = 0;
	iterator.index = 0;
	iterator.object_index = NONE;
	s_handler_object_view *object;
	while ((object = (s_handler_object_view *)function_baeb0(&iterator)) != NULL)
	{
		s_object_ai_data *data = object_ai_data_checked(object);
		if (data)
		{
			data->index08 = NONE;
			data->index04 = NONE;
			data->next_object_index = NONE;
		}
	}
	g_5044c8->valid = false;
}

struct s_28ff90_state
{
	dword flags0;
	dword flags4;
	byte unknown08[0x10];
	vector3f forward;
	vector3f up;
};

void function_b9fc0(long object_index, vector3f *forward, vector3f *up);

struct s_28ff90_object
{
	byte unknown00[0x12c];
	word reset : 1;
	word others : 15;
	byte unknown12e[0x13c - 0x12e];
	s_28ff90_state state;
};

struct s_28ff90_header
{
	byte unknown00[8];
	s_28ff90_object *object;
};

// @retail 0x28ff90
void function_28ff90(long object_index)
{
	s_28ff90_state state;
	memset(&state, 0, sizeof(state));
	state.forward = *g_4687a8;
	state.flags0 = 0x6000086;
	state.flags4 = 0x400000c;
	state.up = *g_4687b0;
	function_b9fc0(object_index, &state.forward, &state.up);
	((s_28ff90_header *)g_4e0300->data)[object_index & 0xffff].object->state = state;
	((s_28ff90_header *)g_4e0300->data)[object_index & 0xffff].object->reset = false;
}

// @retail 0x290040
void function_290040(long perception_index)
{
	s_ai_object_iterator iterator;
	iterator.next_index = perception_get(perception_index)->object_index;
	while (next_object_290040(&iterator))
		function_28ff90(iterator.index);
}

// @retail 0x28e4b0
bool function_28e4b0(long index, short type, long *object_index, point3f *position, vector3f *velocity)
{
	bool result = false;
	*object_index = NONE;
	if (index != NONE)
	{
		switch (type)
		{
		case 1:
		{
			s_handler_object_view *object = handler_object_get(index);
			*object_index = index;
			function_b9dd0(index, position);
			*velocity = *(vector3f *)((byte *)object + 0x30);
			result = true;
			break;
		}
		case 2:
		{
			s_prop_node_view *node = prop_node_get(index);
			s_prop_state_view *state = prop_node_state(node);
			*object_index = *(long *)((byte *)node + 0x20);
			*position = state->position;
			*velocity = *(vector3f *)((byte *)state + 0x30);
			result = true;
			break;
		}
		case 3:
		{
			s_actor_view *actor = actor_get(index);
			*object_index = actor->unknown018;
			*position = actor->position;
			*velocity = *(vector3f *)((byte *)actor + 0x22c);
			result = true;
			break;
		}
		default:
			*object_index = NONE;
			break;
		}
	}
	return result;
}

void function_119020(long arg_0, vector3f const *arg_1);

// @retail 0x2901e0
void function_2901e0(long arg_0, vector3f const *arg_1)
{
	long local_0 = perception_get(arg_0)->object_index;
	while (local_0 != NONE)
	{
		s_object_ai_data *local_1 = object_ai_data_checked(handler_object_get(local_0));
		function_119020(local_0, arg_1);
		if (!local_1)
			break;
		local_0 = local_1->next_object_index;
	}
}

// @retail 0x28fd00
void function_28fd00(long arg_0)
{
	s_handler_object_view *local_0 = handler_object_get(arg_0);
	if (local_0->type == 12)
	{
		s_object_ai_data *local_1 = object_ai_data_checked(local_0);
		if (local_1 && local_1->index08 != NONE)
		{
			if (local_1->index50 != NONE)
			{
				long local_2 = *(long *)(g_502418->data + (local_1->index50 & 0xffff) * 0x3c + 8);
				*(byte *)(g_50241c->data + (local_2 & 0xffff) * 0xc4 + 0x36) = false;
				*(short *)local_1->unknown54 = NONE;
			}
			function_28e390(local_1->index08, arg_0, false, true);
		}
	}
}

struct s_orientation_request_118f90;
void function_118f90(s_orientation_request_118f90 *arg_0);

struct s_28f601 { byte field_0[8]; word : 4; word field_8 : 1; word : 11; };
struct s_28f602 { byte field_0[8]; word : 3; word field_8 : 1; word : 12; };
struct s_28f603 { byte field_0[8]; word field_8 : 1; word : 15; };

// @retail 0x28f600
void function_28f600(long arg_0)
{
	s_handler_object_view *local_0 = handler_object_get(arg_0);
	if (!local_0->flags134)
	{
		s_object_ai_data *local_1 = (s_object_ai_data *)((byte *)local_0 + local_0->ai_offset);
		if (local_1)
		{
			s_28ff90_state local_2;
			function_118f90((s_orientation_request_118f90 *)&local_2);
			if (local_1->position_pending)
				((s_28f601 *)&local_2)->field_8 = true;
			else
				((s_28f601 *)&local_2)->field_8 = false;
			((s_28f602 *)&local_2)->field_8 = true;
			local_2.forward = *(vector3f *)((byte *)local_1 + 0x80);
			local_2.up = *(vector3f *)((byte *)local_1 + 0x8c);
			*(vector3f *)((byte *)&local_2 + 0xc) = *(vector3f *)((byte *)local_1 + 0x98);
			if (*((byte *)local_1 + 0xa4))
			{
				local_2.flags0 = *(long *)((byte *)local_1 + 0xac);
				local_2.flags4 = *(long *)((byte *)local_1 + 0xa8);
				((s_28f603 *)&local_2)->field_8 = true;
				*((byte *)local_1 + 0xa4) = false;
			}
			((s_28ff90_header *)g_4e0300->data)[arg_0 & 0xffff].object->state = local_2;
		}
	}
}
