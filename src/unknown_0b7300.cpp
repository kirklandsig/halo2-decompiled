// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0B7300.CPP: the scenario's object placements

The objects the scenario places, made from its datums
and palettes when a map or structure bsp loads, and kept in step with the
game. function_d4de0 and function_d4e20
(0xd4de0, 0xd4e20) are in unknown_0d4de0.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "object_iterator.h"
#include "loop_allocator.h"
#include <string.h>

typedef long string_handle;

/* the object type definitions' scenario fields (g_468630, by object type) */
struct s_object_type_placement_view
{
	byte unknown00[0xa];
	short scenario_datums_offset;
	short scenario_palette_offset;
	short scenario_datum_size;
};

enum
{
	k_object_type_count = 13
};

/* a block of the scenario (g_4e0350), at the offsets above */
struct s_scenario_block
{
	long count;
	byte *elements;
};

/* an object the scenario places (a datum of a per-type block) */
struct s_type_4f0dcc
{
	short palette_index;
	short name_index;
	dword flags;
	point3f position;
	vector3f rotation;
	real scale;
	byte unknown24[2];
	word bsp_mask;
	long unique_id;
	short origin_bsp_index;
	char type;
	char source;
	char bsp_policy;
	byte unknown31[3];
	long unknown34;
	long unknown38;
	dword field_3c[4];
	byte unknown4c[0x5a - 0x4c];
	word multiplayer_flags;
};

/* a palette entry (0x28 bytes) */
struct s_scenario_palette_entry
{
	byte unknown00[4];
	long tag_index;
	byte unknown08[0x28 - 0x8];
};

/* what a new object is made from (b7930) */
struct s_object_placement_data
{
	byte unknown00[4];
	long unique_id;
	long unknown08;
	long unknown0c;
	long unknown10;
	char bsp_policy;
	byte unknown15[3];
	dword flags;
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown40[0x58 - 0x40];
	real scale;
	byte unknown5c[0x74 - 0x5c];
	long unknown74;
	color3f field_3c[4];
};

/* the object fields the placement code reads */
struct s_placement_object
{
	long tag_index;
	struct
	{
		dword unknown0 : 7;
		dword hidden : 1;
		dword unknown8 : 1;
		dword unknown9 : 18;
		dword hidden_by_bsp : 1;
		dword : 4;
	} flags;
	byte unknown08[0xc];
	long parent_object_index;
	byte unknown18[2];
	short placement_index;
	byte unknown1c[0x30 - 0x1c];
	point3f field_x221477;
	byte unknown3c[0xaa - 0x3c];
	char type;
	char placement_source;
	short unknownac;
	char bsp_index;
	byte unknownaf;
	char bsp_policy;
	byte unknownb1[0xd4 - 0xb1];
	long unknownd4;
};

struct s_placement_object_header
{
	byte unknown00[8];
	s_placement_object *object;
};

#define PLACEMENT_OBJECT(index) (((s_placement_object_header *)g_4e0300->data)[(index) & 0xffff].object)

struct s_object_placement_globals
{
	short unknown0;
	short unknown2;
};

extern s_object_placement_globals *g_4e0324;
/* the maximum number of each type's scenario objects */
long g_44074c[k_object_type_count] = { 0x80, 0x100, 0x80, 0x100, 0, 0, 0x7d0, 0x190, 0x64, 0x1f4, 0x100, 0x400, 0x80 };

#define OBJECT_TYPE_PLACEMENT(type) ((s_object_type_placement_view *)g_468630[type])
extern short g_4686c4;
struct s_object_list;
extern s_object_list *g_4de2f4;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
struct s_data_header_40;
extern s_data_header_40 *g_4de2ec;

#define SCENARIO_BLOCK(offset) ((s_scenario_block *)((byte *)g_4e0350 + (offset)))

#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))
#endif
#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

void __stdcall function_1e95d0(char const *status);
void function_1e9650();
void __stdcall function_b8600(long object_index, long unknown);
bool __stdcall function_beb30(long object_index);
struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
void function_11df60(vector3f const *rotation, vector3f *forward, vector3f *up);
color3f *unpack_color3f(dword pixel, color3f *color);
void function_b7300(long object_index);
void __stdcall function_b87b0(long object_index);
void __stdcall function_b83b0(long object_index, bool a);
void __stdcall function_b8460(long object_index, bool a);
bool function_11c470(long cluster_index, point3f const *point);
long function_bf0f0(s_type_4f0dcc const *datum, long type, long index, s_scenario_block *palette, bool a, bool b);
void function_bf380();
long function_bf760(long const *unique_id);
void loop_compact(s_loop_allocator *loop);

/* a type's scenario datums (and their size) and palette */
PRIVATE inline s_scenario_block *function_x71bcc9(void *scenario, long type, long *datum_size)
{
	s_object_type_placement_view *definition = OBJECT_TYPE_PLACEMENT(type);

	*datum_size = definition->scenario_datum_size;
	return (s_scenario_block *)((byte *)scenario + definition->scenario_datums_offset);
}

PRIVATE inline s_scenario_block *function_x6e4ccb(void *scenario, long type)
{
	return (s_scenario_block *)((byte *)scenario + OBJECT_TYPE_PLACEMENT(type)->scenario_palette_offset);
}

/* a scenario object of a type, NULL out of range */
// @retail 0xd5260
s_type_4f0dcc *function_d5260(long type, long index)
{
	s_object_type_placement_view *definition = OBJECT_TYPE_PLACEMENT(type);
	s_type_4f0dcc *result = NULL;

	if (definition->scenario_datums_offset != NONE && definition->scenario_palette_offset != NONE)
	{
		long size;
		s_scenario_block *block = function_x71bcc9(g_4e0350, type, &size);

		if (PIN(index, 0, block->count - 1) == index)
			result = (s_type_4f0dcc *)(block->elements + size * index);
	}
	return result;
}

/* whether an object of this type made from this tag can be placed by more
   than its name (it can be damaged, or is scenery with a damage model) */
// @retail 0xd5990
bool function_d5990(long type, long tag_index)
{
	dword type_mask = 1 << type;
	bool result = (type_mask & 0x199f) != 0;

	if (!result && (type_mask & 0x40))
	{
		byte *definition = g_4e3b44[tag_index & 0xffff].bytes;

		if (definition)
		{
			long model_index = *(long *)(definition + 0x38);

			if (model_index != NONE)
				result = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 0x60) > 0;
		}
	}
	return result;
}

/* a scenario object's palette entry */
// @retail 0xd58a0
s_scenario_palette_entry *function_d58a0(s_type_4f0dcc const *datum, void *scenario)
{
	s_scenario_palette_entry *result = NULL;

	if (datum->palette_index != NONE)
	{
		long palette_index = datum->palette_index;
		long type = datum->type;

		if (PIN(type, 0, 12) == type)
		{
			short offset = OBJECT_TYPE_PLACEMENT(type)->scenario_palette_offset;

			if (offset != NONE)
			{
				s_scenario_block *palette = (s_scenario_block *)((byte *)scenario + offset);

				if (PIN(palette_index, 0, palette->count - 1) == palette_index)
					result = &((s_scenario_palette_entry *)palette->elements)[palette_index];
			}
		}
	}
	return result;
}

/* how a scenario object picks its structure bsps: 0 any, 1 its own,
   2 those in its mask */
// @retail 0xd5a70
long function_d5a70(s_type_4f0dcc const *datum)
{
	long result = NONE;

	switch (datum->bsp_policy)
	{
	case 0:
		switch (datum->source)
		{
		case 0:
			result = 1;
			break;
		case 1:
			if (datum->origin_bsp_index == NONE || ((1 << datum->type) & 0x181f))
				result = 0;
			else
				result = 1;
			break;
		}
		break;
	case 1:
		result = 0;
		break;
	case 2:
		result = 2;
		break;
	}
	return result;
}

/* whether a scenario object is made with its map */
// @retail 0xd59f0
bool function_d59f0(s_type_4f0dcc const *datum)
{
	bool creatable = !(datum->flags & 1);
	short name_index = datum->name_index;
	bool persistent = false;

	if (name_index != NONE)
	{
		persistent = true;
	}
	else
	{
		s_scenario_palette_entry *entry = function_d58a0(datum, g_4e0350);

		if (entry && entry->tag_index != NONE)
			persistent = function_d5990(datum->type, entry->tag_index);
	}

	bool global = datum->name_index != NONE || function_d5a70(datum) != 1;

	return creatable && (persistent || global);
}

/* whether a scenario object belongs on the current structure bsp */
// @retail 0xd5910
bool function_d5910(s_type_4f0dcc const *datum, bool force)
{
	bool creatable = !(datum->flags & 1);
	bool result = false;

	if (creatable || (force & 1))
	{
		if (datum->bsp_policy != 2)
		{
			bool unplaced = datum->origin_bsp_index == NONE || datum->origin_bsp_index == g_4686c4;
			bool named = datum->name_index != NONE || function_d59f0(datum);

			if (unplaced || named)
				return true;
			return false;
		}
		result = (datum->bsp_mask & (1 << g_4686c4)) != 0;
	}
	return result;
}

/* the scenario's cluster references, sorted by their key (scenario
   +0x380), and their binary searches */
struct s_scenario_cluster_reference
{
	long key;
	short cluster_index;
	short unknown06;
};

typedef bool (__stdcall *t_cluster_reference_lower_compare)(s_scenario_cluster_reference const *, long);
typedef bool (__stdcall *t_cluster_reference_upper_compare)(long, s_scenario_cluster_reference const *);

// @retail 0xd5350
bool __stdcall cluster_reference_less_than_key(s_scenario_cluster_reference const *reference, long key)
{
	return reference->key < key;
}

// @retail 0xd5370
bool __stdcall cluster_reference_key_less_than(long key, s_scenario_cluster_reference const *reference)
{
	return reference->key < key;
}

// @retail 0xd5ac0
s_scenario_cluster_reference *cluster_reference_lower_bound(s_scenario_cluster_reference *first,
	s_scenario_cluster_reference *last, long const *key,
	t_cluster_reference_lower_compare compare)
{
	long count = last - first;

	while (count > 0)
	{
		long half = count / 2;
		s_scenario_cluster_reference *middle = first + half;

		if (compare(middle, *key))
		{
			first = middle + 1;
			count -= half + 1;
		}
		else
		{
			count = half;
		}
	}
	return first;
}

// @retail 0xd5b10
s_scenario_cluster_reference *cluster_reference_upper_bound(s_scenario_cluster_reference *first,
	s_scenario_cluster_reference *last, long const *key,
	t_cluster_reference_upper_compare compare)
{
	long count = last - first;

	while (count > 0)
	{
		long half = count / 2;
		s_scenario_cluster_reference *middle = first + half;

		if (!compare(*key, middle))
		{
			first = middle + 1;
			count -= half + 1;
		}
		else
		{
			count = half;
		}
	}
	return first;
}

/* whether an object is in one of the clusters a pair of structure bsps
   share (true when the scenario lists none) */
// @retail 0xd5390
bool function_d5390(long object_index, long key)
{
	s_scenario_block *references = (s_scenario_block *)((byte *)g_4e0350 + 0x380);
	bool result = false;
	bool listed = false;

	if (references->count > 0)
	{
		s_scenario_cluster_reference *first = (s_scenario_cluster_reference *)references->elements;
		s_scenario_cluster_reference *last = first + references->count;
		s_scenario_cluster_reference *lower =
			cluster_reference_lower_bound(first, last, &key, cluster_reference_less_than_key);
		s_scenario_cluster_reference *upper =
			cluster_reference_upper_bound(first, last, &key, cluster_reference_key_less_than);

		for (s_scenario_cluster_reference *reference = lower; !result && reference < upper; reference++)
		{
			short cluster_index = reference->cluster_index;

			if (cluster_index != NONE)
			{
				result = false;
				listed = true;
				if (object_index != NONE &&
					function_11c470(cluster_index, &PLACEMENT_OBJECT(object_index)->field_x221477))
				{
					result = true;
				}
			}
		}
		if (listed)
			return result;
	}
	return true;
}

/* whether an object placed on one structure bsp stays on another */
// @retail 0xd5460
bool function_d5460(short previous_bsp_index, short bsp_index, long object_index)
{
	bool result = false;

	if (bsp_index != NONE)
	{
		char object_bsp_index = PLACEMENT_OBJECT(object_index)->bsp_index;

		if (previous_bsp_index != object_bsp_index)
			previous_bsp_index = object_bsp_index;

		long b = bsp_index;
		long a = previous_bsp_index;
		long low = MIN(a, b);
		long high = MAX(a, b);

		result = object_bsp_index == bsp_index || object_bsp_index == NONE ||
			function_d5390(object_index, (high << 16) | (low & 0xffff));
	}
	return result;
}

/* whether an object stays after a structure bsp switch, by its policy */
// @retail 0xd54e0
bool function_d54e0(long object_index, s_type_4f0dcc const *datum, short bsp_index, short previous_bsp_index)
{
	bool result = false;

	switch (PLACEMENT_OBJECT(object_index)->bsp_policy)
	{
	case 0:
		result = function_d5460(previous_bsp_index, bsp_index, object_index);
		break;
	case 1:
		result = bsp_index == datum->origin_bsp_index;
		break;
	case 2:
		result = ((1 << bsp_index) & datum->bsp_mask) != 0;
		break;
	case NONE:
		result = false;
		break;
	}
	return result;
}

/* brings back an object the structure bsp switch had hidden */
__forceinline void object_placement_unhide(long object_index)
{
	s_placement_object *object = PLACEMENT_OBJECT(object_index);

	object->flags.hidden = false;
	if (!TEST_FIELD_BIT(object->flags.unknown8) && g_4de2f4 && *(byte *)g_4de2f4)
		function_b8600(object_index, 0);
}

/* hides an object that isn't on the current structure bsp, or deletes it */
// @retail 0xd52b0
void function_d52b0(long object_index)
{
	s_placement_object *object = PLACEMENT_OBJECT(object_index);

	if (object->unknownac != NONE || function_d5990(object->type, object->tag_index))
	{
		s_placement_object *current = PLACEMENT_OBJECT(object_index);

		function_b7300(object_index);
		if (TEST_FIELD_BIT(current->flags.unknown8))
			function_b87b0(object_index);
		current->flags.hidden = true;
		current->flags.hidden_by_bsp = true;
	}
	else if (g_4e6948->mode != 4 || object->unknownd4 == NONE)
	{
		function_b83b0(object_index, true);
		function_b8460(object_index, true);
	}
}

/* brings back an object hidden by a structure bsp switch */
// @retail 0xd4ff0
void function_d4ff0(long object_index)
{
	s_placement_object *object = PLACEMENT_OBJECT(object_index);

	if (TEST_FIELD_BIT(object->flags.hidden) && TEST_FIELD_BIT(object->flags.hidden_by_bsp))
	{
		object_placement_unhide(object_index);
		object->flags.hidden_by_bsp = false;
		object->bsp_index = (char)g_4686c4;
	}
}

/* keeps the placed objects in step with a structure bsp switch */
// @retail 0xd4e60
void function_d4e60(short bsp_index)
{
	short previous_bsp_index = g_4e0324->unknown0;

	if (previous_bsp_index != bsp_index)
	{
		s_type_f1af8e iterator;
		s_placement_object *object;

		function_bae80(&iterator, NONE, 0);
		for (object = (s_placement_object *)function_baeb0(&iterator); object;
			object = (s_placement_object *)function_baeb0(&iterator))
		{
			if (object->parent_object_index != NONE || object->placement_source == NONE)
				continue;

			s_type_4f0dcc *datum = ((1 << object->placement_source) & 3) ?
				function_d5260(object->type, object->placement_index) : NULL;
			bool hidden_by_switch = TEST_FIELD_BIT(object->flags.hidden_by_bsp);
			bool hidden = TEST_FIELD_BIT(object->flags.hidden);
			long object_index = iterator.object_index;
			bool stays = function_d54e0(object_index, datum, bsp_index, previous_bsp_index);
			bool keep = function_beb30(object_index) ? true : stays;

			if (hidden)
			{
				if (!keep)
					continue;
				if (hidden_by_switch)
				{
					object_placement_unhide(object_index);
					object->flags.hidden_by_bsp = false;
				}
			}
			else if (!keep)
			{
				function_d52b0(object_index);
				continue;
			}
			if (bsp_index != NONE)
				object->bsp_index = (char)bsp_index;
		}
	}
	g_4e0324->unknown0 = bsp_index;
}

/* fills in the data a scenario object is made from */
// @retail 0xd5060
bool function_d5060(s_scenario_block *palette, volatile long type, long unknown10,
	s_type_4f0dcc const *datum, bool unknown, s_object_placement_data *data)
{
	bool create = !((datum->flags >> 6) & 1);
	s_object_type_placement_view *definition = OBJECT_TYPE_PLACEMENT(type);
	bool result = false;

	if (type == 6 && g_4e6948->state == 2)
	{
		word multiplayer_flags = datum->multiplayer_flags;

		if (multiplayer_flags != 0 && multiplayer_flags != 0x7f)
		{
			switch (*(long *)((byte *)g_4e6948 + 0x180))
			{
			case 1:
				create = (byte)multiplayer_flags & 1;
				break;
			case 2:
				create = ((byte)multiplayer_flags >> 1) & 1;
				break;
			case 3:
				create = ((byte)multiplayer_flags >> 2) & 1;
				break;
			case 4:
				create = ((byte)multiplayer_flags >> 3) & 1;
				break;
			case 7:
				create = ((byte)multiplayer_flags >> 4) & 1;
				break;
			case 8:
				create = ((byte)multiplayer_flags >> 5) & 1;
				break;
			case 9:
				create = ((byte)multiplayer_flags >> 6) & 1;
				break;
			}
		}
	}

	if (!create || !definition || datum->palette_index == NONE)
		return result;
	if (PIN(datum->palette_index, 0, palette->count - 1) != datum->palette_index)
		return result;

	long tag_index = ((s_scenario_palette_entry *)palette->elements)[datum->palette_index].tag_index;

	if (tag_index != NONE)
	{
		function_b7930(data, tag_index, NONE, NULL);
		data->position = datum->position;
		data->scale = datum->scale > 0.0f ? datum->scale : 1.0f;
		function_11df60(&datum->rotation, &data->forward, &data->up);
		data->bsp_policy = (char)function_d5a70(datum);
		result = true;
		if (((1 << type) & 0x847) && (word)definition->scenario_datum_size >= 0x4c)
		{
			data->unknown0c = datum->unknown34;
			data->unknown74 = datum->unknown38;
			for (long i = 0; i < 4; i++)
				unpack_color3f(datum->field_3c[i], &data->field_3c[i]);
		}
	}
	data->unique_id = datum->unique_id;
	data->unknown08 = *(long *)&datum->origin_bsp_index;
	if (unknown && ((1 << datum->type) & 0x1883) || (datum->flags & 0x100))
		data->flags |= 8;
	else
		data->flags &= ~8;
	data->unknown10 = unknown10;
	return result;
}

/* makes the scenario's objects that come with the map */
// @retail 0xd5560
void function_d5560(bool skip_existing)
{
	void *scenario = g_4e0350;

	for (long type = 0; type < k_object_type_count; type++)
	{
		s_object_type_placement_view *definition = OBJECT_TYPE_PLACEMENT(type);

		if (definition->scenario_datums_offset != NONE && definition->scenario_palette_offset != NONE)
		{
			long size;
			s_scenario_block *block = function_x71bcc9(scenario, type, &size);
			s_scenario_block *palette = function_x6e4ccb(scenario, type);

			for (short i = 0; i < block->count; i++)
			{
				s_type_4f0dcc *datum = (s_type_4f0dcc *)(block->elements + i * size);
				bool create = function_d59f0(datum);

				if (skip_existing && function_bf760(&datum->unique_id) != NONE)
					continue;
				if (create)
				{
					function_bf0f0(datum, type, i, palette, true, true);
					function_bf380();
				}
			}
		}
	}
}

/* a bit for each scenario object of every type (5176 in all) */
struct s_scenario_object_bits
{
	dword bits[0xa2];
};

/* makes the scenario's objects that belong on a new structure bsp and
   don't exist yet */
// @retail 0xd5640
void function_d5640()
{
	void *scenario = g_4e0350;
	long first_bits[k_object_type_count];
	s_scenario_object_bits existing;
	long bit_count = 0;
	long type;

	memset(&existing, 0, sizeof(existing));
	for (type = 0; type < k_object_type_count; type++)
	{
		s_object_type_placement_view *definition = OBJECT_TYPE_PLACEMENT(type);

		if (definition->scenario_datums_offset != NONE && definition->scenario_palette_offset != NONE)
		{
			first_bits[type] = bit_count;
			bit_count += g_44074c[type];
		}
		else
		{
			first_bits[type] = NONE;
		}
	}

	s_type_f1af8e iterator;
	s_placement_object *object;

	function_bae80(&iterator, NONE, 0);
	for (object = (s_placement_object *)function_baeb0(&iterator); object;
		object = (s_placement_object *)function_baeb0(&iterator))
	{
		if ((1 << object->placement_source) & 3)
		{
			long bit = first_bits[object->type] + object->placement_index;

			existing.bits[bit >> 5] |= 1 << (bit & 0x1f);
		}
	}

	for (type = 0; type < k_object_type_count; type++)
	{
		s_object_type_placement_view *definition = OBJECT_TYPE_PLACEMENT(type);

		if (definition->scenario_datums_offset == NONE || definition->scenario_palette_offset == NONE)
			continue;

		long size = definition->scenario_datum_size;
		s_scenario_block *block = (s_scenario_block *)((byte *)scenario + definition->scenario_datums_offset);
		s_scenario_block *palette = (s_scenario_block *)((byte *)scenario + definition->scenario_palette_offset);
		long first_bit = first_bits[type];

		function_bf380();
		loop_compact((s_loop_allocator *)g_4de2ec);
		for (long i = 0; i < block->count; i++)
		{
			long bit = first_bit + i;

			if (existing.bits[bit >> 5] & (1 << (bit & 0x1f)))
				continue;

			s_type_4f0dcc *datum = (s_type_4f0dcc *)(block->elements + i * size);

			if (datum->name_index != NONE)
				continue;

			s_scenario_palette_entry *entry = function_d58a0(datum, g_4e0350);

			if (entry && entry->tag_index != NONE && function_d5990(datum->type, entry->tag_index))
				continue;
			if (datum->flags & 1)
				continue;
			if (datum->bsp_policy == 2)
			{
				if (!(datum->bsp_mask & (1 << (char)g_4686c4)))
					continue;
			}
			else
			{
				bool unplaced = datum->origin_bsp_index == NONE || datum->origin_bsp_index == g_4686c4;
				bool created = function_d59f0(datum) != 0;

				if (!unplaced && !created)
					continue;
			}
			function_bf0f0(datum, type, i, palette, true, true);
			function_bf380();
		}
	}
}

// @retail 0xd4e30
void function_d4e30(void)
{
	function_1e95d0("placing objects on bsp");
	if (!g_510c50 || !((byte *)g_510c50)[5] || !((byte *)g_510c50)[7])
		function_d5640();
	function_1e9650();
}
