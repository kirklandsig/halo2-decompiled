// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_20FE20.CPP: object tables of game speed (a16 pad) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include "unknown_1efac0.h"
#include "object_iterator.h"
#include "unknown_1428b0.h"
#include "unknown_20fe20.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

void function_210e20(void);
void function_210f00(void);

// @retail 0x210df0
void function_210df0(void)
{
	memset(g_4f93a4, 0xff, 0x40);
	function_210f00();
	function_210e20();
}

struct s_audio_priority_record
{
	short salt;
	short type;
	long object_index;
	byte unknown08[0x14 - 8];
	long time;
	byte unknown18[4];
	short duration;
	byte unknown1e[4];
	short priority;
	byte unknown24[0x40 - 0x24];
	bool forced;
	byte unknown41[3];
	long parent;
};

struct s_audio_priority_definition
{
	byte unknown00[0x1a];
	short priority;
	byte unknown1c[0x60 - 0x1c];
};

struct s_audio_priority_table
{
	long count;
	s_audio_priority_definition *entries;
};

PRIVATE inline s_audio_priority_table *audio_priority_table(void)
{
	s_audio_priority_table *result = NULL;
	byte *globals = (byte *)g_4e034c;
	if (globals && *(long *)(globals + 0xc8) > 0)
	{
		long index = *(long *)(*(byte **)(globals + 0xcc) + 0x64);
		if (index != NONE)
			result = (s_audio_priority_table *)g_4e3b44[index & 0xffff].bytes;
	}
	return result;
}

// @retail 0x20f5a0
void function_20f5a0(s_audio_priority_record *current, s_audio_priority_record const *previous)
{
	s_audio_priority_table *table = audio_priority_table();
	if (current->time >= previous->time && current->time < previous->time + previous->duration)
	{
		bool close = abs(current->time - previous->time) < 3;
		s_audio_priority_definition *definition = &table->entries[current->type];
		if ((previous->forced && previous->object_index == current->object_index && current->priority < 11) ||
			(definition->priority < previous->priority &&
			 (previous->forced || current->priority <= previous->priority || !close) &&
			 (previous->forced || current->priority != previous->priority || !close || current->parent == NONE || previous->parent != NONE)))
		{
			real seconds = g_510c54->field_2_3 * 0.2f;
			long delay;
			__asm
			{
				fld seconds
				fistp delay
			}
			current->time = previous->time + previous->duration + delay;
		}
	}
}

struct s_audio_queue;
extern s_audio_queue *g_4f939c;
extern s_record_pool *g_4f9398;
long __declspec(noinline) function_20f040(short team);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

struct s_audio_object_state
{
	byte unknown000[0x10a];
	byte unused0 : 2;
	byte blocked : 1;
	byte unused1 : 5;
};

// @retail 0x20f960
long function_20f960(long index, s_audio_priority_table *table, bool *expired)
{
	(void)&expired;
	volatile bool result = false;
	byte *record = g_4f9398->data + (index & 0xffff) * 0x54;
	s_audio_priority_definition *definition = &table->entries[*(short *)(record + 2)];
	long time = g_510c54->game_time;
	if (expired)
		*expired = false;
	if (!*((byte *)g_4f55d0 + 0x20) || *(short *)((byte *)g_4f55d0 + 0x22) > 0)
		goto rejected;
	{
		real seconds = g_510c54->field_2_3 * *(real *)((byte *)definition + 0x20);
		long ticks;
		__asm
		{
			fld seconds
			fistp ticks
		}
		if (time - *(long *)(record + 0x10) > ticks)
			goto report_expiration;
	}
	{
		byte *object = *(byte **)(g_4e0300->data + (*(long *)(record + 4) & 0xffff) * 12 + 8);
		result = true;
		if (TEST_FIELD_BIT(((s_audio_object_state *)object)->blocked))
			goto rejected;
		long actor_index = *(long *)(object + 0x12c);
		if (actor_index == NONE)
			goto done;
		byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
		if (*(short *)(actor + 0x86) > *(short *)((byte *)definition + 0x16) ||
			time - *(long *)(actor + 0x4c) < g_510c54->field_2_3 * 3)
			goto rejected;
		short type = *(short *)(record + 0x24);
		if (type >= 0 && type < 200 && *(long *)(record + 0x28) != NONE)
		{
			switch (type)
			{
			case 138:
			case 142:
			case 150:
			case 159:
			case 194:
			case 196:
			case 198:
				{
					s_audio_object_state *target = (s_audio_object_state *)function_badc0(*(long *)(record + 0x28), NONE);
					if (!target || TEST_FIELD_BIT(target->blocked))
						goto rejected;
				}
				break;
			}
		}
		goto done;
	}
rejected:
	result = false;
report_expiration:
	if (expired)
		*expired = *(long *)(g_4f9398->data + (index & 0xffff) * 0x54 + 0x2c) != 0;
done:
	return result;
}

// @retail 0x20f1a0
real function_20f1a0(long object_index, short type)
{
	(void)&type;
	real result = 1.0f;
	s_audio_priority_table *table = audio_priority_table();
	if (table)
	{
		byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
		short team = *(short *)(object + 0x138);
		short side = (short)function_20f040(team);
		byte *queue = (byte *)g_4f939c + side * 0x7dc;
		long index = *(volatile long *)(queue + 0x7d0);
		long time = g_510c54->game_time;
		real seconds = g_510c54->field_2_3 * *(real *)((byte *)&table->entries[type] + 0x24);
		long delay;
		__asm
		{
			fld seconds
			fistp delay
		}
		time += delay;
		long found = NONE;
		for (; index != NONE; )
		{
			byte *node = g_4f9398->data + (index & 0xffff) * 0x54;
			if (*(long *)(node + 0x14) > time)
				break;
			found = index;
			index = *(long *)(node + 0x50);
		}
		if (found != NONE && *(long *)(g_4f9398->data + (found & 0xffff) * 0x54 + 4) == object_index)
			result = 0.5f;
	}
	return result;
}

long function_1caa10(long object_index);
long function_cbd50(long unit_index, short weapon_index);

PRIVATE __forceinline bool audio_object_type_matches(byte *object, long type)
{
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	return *(long *)(definition + 0x5c) > 0 && *(long *)(*(byte **)(definition + 0x60) + 4) == type;
}

// @retail 0x20c2f0
bool __stdcall function_20c2f0(long object_index, long type, real *scale)
{
	bool result = false;
	if (object_index != NONE)
	{
		byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
		if (audio_object_type_matches(object, type))
			goto found;
		if ((1 << *(byte *)(object + 0xaa)) & 3)
		{
			if (*(short *)(object + 0x1fc) != NONE)
			{
				long parent = *(long *)(object + 0x14);
				if (*(byte *)(g_4e0300->data + (parent & 0xffff) * 12 + 3) == 1)
				{
					long related = function_1caa10(parent);
					if (related != NONE)
					{
						byte *other = *(byte **)(g_4e0300->data + (related & 0xffff) * 12 + 8);
						if (audio_object_type_matches(other, type))
							goto found;
					}
				}
			}
			object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			long weapon = function_cbd50(object_index, *(char *)(object + 0x212));
			if (weapon != NONE)
			{
				byte *other = *(byte **)(g_4e0300->data + (weapon & 0xffff) * 12 + 8);
				if (audio_object_type_matches(other, type))
					goto found;
			}
		}
		return false;
		found:
		if (scale)
			*scale *= 2.0f;
		return true;
	}
	return result;
}

struct s_audio_weighted_entry
{
	byte unknown00[0x10];
	real weight;
};

// @retail 0x20f0a0
short function_20f0a0(s_audio_weighted_entry *entries, short count)
{
	short result = NONE;
	if (count == 1)
	{
		result = 0;
	}
	else
	{
		real maximum = 0.0f;
		real total = 0.0f;
		for (short i = 0; i < count; i++)
		{
			if (entries[i].weight > maximum)
				maximum = entries[i].weight;
		}
		for (short j = 0; j < count; j++)
		{
			if (entries[j].weight >= maximum * 0.8f)
				total += entries[j].weight;
			else
				entries[j].weight = 0.0f;
		}
		real random = function_x82e52f(&g_4e7408->unknown0, NULL, 0);
		real choice = total * random;
		real cumulative = 0.0f;
		for (short k = 0; k < count; k++)
		{
			if (entries[k].weight > 0.0f)
			{
				real weight = entries[k].weight;
				weight += cumulative;
				cumulative = weight;
				if (cumulative >= choice)
				{
					result = k;
					break;
				}
			}
		}
	}
	return result;
}

/* ---- shared views ---- */
struct s_header_view
{
	byte unknown00[8];
	byte *object;
};

#define OBJECT_FROM_INDEX(index) (((s_header_view *)g_4e0300->data)[(index) & 0xffff].object)

/* the object iterator of src/unknown_0bad50.cpp */
struct s_object;

/* ---- 0x4f9398: a pool of 0x54 byte nodes linked from an object ---- */
struct s_node
{
	byte unknown00[4];
	long object_index;
	byte unknown08[0x40];
	byte flag;
	byte unknown49[7];
	long next;
};

struct s_node_owner
{
	byte unknown00[0x7d0];
	long first;
	short count;
};

struct s_node_link
{
	byte unknown00[0x1c];
	long node_index;
};

struct s_node_object
{
	byte unknown00[0x342];
	short link_offset;
};

s_record_pool *g_4f9398;

#define NODE(index) ((s_node *)(g_4f9398->data + ((index) & 0xffff) * sizeof(s_node)))

// @retail 0x20fe20
void function_20fe20(s_node_owner *owner)
{
	long *link = &owner->first;

	while (*link != NONE)
	{
		long node_index = *link;
		s_node *node = NODE(node_index);

		if (node->flag)
		{
			long next = node->next;

			if (NODE(node_index)->object_index != NONE)
			{
				s_node_object *object = (s_node_object *)OBJECT_FROM_INDEX(NODE(node_index)->object_index);
				s_node_link *object_link = (s_node_link *)((byte *)object + object->link_offset);

				if (object_link->node_index == node_index)
				{
					object_link->node_index = NONE;
				}
			}

			record_pool_release(g_4f9398, node_index);
			*link = next;
			owner->count--;
		}
		else
		{
			link = &node->next;
		}
	}
}

void function_20fda0(long node_index);

PRIVATE inline long audio_queue_delay(void)
{
	real seconds = (real)g_510c54->field_2_3 * 0.2f;
	long ticks;
	__asm
	{
		fld seconds
		fistp ticks
	}
	return ticks;
}

// @retail 0x20f6a0
bool function_20f6a0(s_node_owner *owner, long node_index)
{
	(void)&owner;
	(void)&node_index;
	volatile bool result = false;
	byte *record = (byte *)NODE(node_index);
	long filter = *(long *)((byte *)owner + 0x7d8);
	if (filter == NONE || *(long *)(record + 4) == filter || *(long *)(record + 0x28) == filter)
	{
		long previous = NONE;
		long next = NONE;
		for (long index = owner->first; index != NONE; )
		{
			byte *other = (byte *)NODE(index);
			function_20f5a0((s_audio_priority_record *)record, (s_audio_priority_record *)other);
			if (*(long *)(record + 0x14) <= *(long *)(other + 0x14))
			{
				next = index;
				break;
			}
			previous = index;
			index = *(long *)(other + 0x50);
		}
		if (*(long *)(record + 0x14) - *(long *)(record + 0x10) <= *(short *)(record + 0x1e) &&
			(!record[0xe] || *(long *)(record + 0x44) == NONE || *(long *)(record + 0x44) == previous))
		{
			s_node *before = previous == NONE ? NULL : NODE(previous);
			if (before)
				before->next = node_index;
			else
				owner->first = node_index;
			*(long *)(record + 0x50) = next;
			owner->count++;
			if (next != NONE)
			{
				byte *last = (byte *)NODE(node_index);
				do
				{
					byte *other = (byte *)NODE(next);
					long delay = audio_queue_delay();
					long time = *(long *)(other + 0x14);
					if (time < *(long *)(last + 0x14) + delay)
						time = *(long *)(last + 0x14) + audio_queue_delay();
					*(long *)(other + 0x14) = time;
					function_20f5a0((s_audio_priority_record *)other, (s_audio_priority_record *)last);
					if (!other[0x48])
					{
						if (*(long *)(other + 0x14) - *(long *)(other + 0x10) > *(short *)(other + 0x1e) ||
							(other[0xe] && *(long *)(other + 0x44) != NONE && *(long *)(other + 0x44) != previous))
							function_20fda0(next);
						else
						{
							last = other;
							previous = next;
						}
					}
					next = *(long *)(other + 0x50);
				} while (next != NONE);
			}
			result = true;
		}
	}
	function_20fe20(owner);
	return result;
}

/* ---- the tables of the match globals (0x4e0348) ---- */
struct s_flags8
{
	byte flags;
	byte unknown01[7];
};

struct s_pair_range
{
	dword unknown00;
	long lower;
	long upper;
	short value;
	short unknown0e;
};

struct s_pair_item
{
	short value;
	byte byte2;
	byte byte3;
};

struct s_pair_entry
{
	dword flag0 : 1;
	dword unknown : 31;
	long lower;
	long upper;
	long range_count;
	s_pair_range *ranges;
	long item_count;
	s_pair_item *items;
};

struct s_pair_table
{
	long count;
	s_flags8 *flags;
	byte unknown08[0x28];
	long entry_count;
	s_pair_entry *entries;
};

struct s_unknown_4e0348
{
	byte unknown00[0xc4];
	long count;
	s_pair_table *table;
};

struct s_object_with_pair
{
	byte unknown00[0xaa];
	byte type;
	byte unknownab[0x130 - 0xab];
	short value130;
	byte unknown132[0x1d4 - 0x132];
	short value1d4;
};

struct s_output_entry
{
	long object_index;
	short unknown04;
	short index;
	byte unknown08[2];
	byte byte0a;
	byte byte0b;
	byte unknown0c[4];
};

s_output_entry *g_4f93a0;

// @retail 0x2101d0
bool function_2101d0(s_pair_table *table, long object_index)
{
	s_object_with_pair *object = (s_object_with_pair *)OBJECT_FROM_INDEX(object_index);
	short value = NONE;

	if (object->type == 6)
	{
		value = object->value130;
	}
	else if (object->type == 7)
	{
		value = object->value1d4;
	}

	if (value >= 0)
	{
		if (value < table->entry_count)
		{
			s_pair_entry *entry = &table->entries[value];

			if (TEST_FIELD_BIT(entry->flag0))
			{
				short i = 0;

				if (entry->item_count > 0)
				{
					do
					{
						s_pair_item *item = &entry->items[i];
						s_output_entry *output = &g_4f93a0[item->value];

						output->index = i;
						output->object_index = object_index;
						output->byte0a = item->byte2;
						output->byte0b = item->byte3;
						i++;
					}
					while (i < entry->item_count);

					return false;
				}
			}
		}

		return false;
	}

	return false;
}

// @retail 0x210310
short function_210310(long object_index, long a)
{
	short result = NONE;
	s_unknown_4e0348 *globals = (s_unknown_4e0348 *)g_4e0348;

	if (globals->count > 0 && globals->table)
	{
		s_pair_table *table = globals->table;
		s_object_with_pair *object = (s_object_with_pair *)OBJECT_FROM_INDEX(object_index);
		short value = NONE;

		if (object->type == 6)
		{
			value = object->value130;
		}
		else if (object->type == 7)
		{
			value = object->value1d4;
		}

		if (value >= 0 && value < table->entry_count)
		{
			s_pair_entry *entry = &table->entries[value];

			if (TEST_FIELD_BIT(entry->flag0) && a != NONE)
			{
				s_lookup lookup;

				if (lookup.initialize(object_index))
				{
					short i = (short)(a & 0xff);

					if (i >= 0 && i < entry->item_count)
					{
						result = entry->items[i].value;
					}
				}
			}
		}
	}

	return result;
}
// @retail 0x2108a0
short function_2108a0(long index)
{
	short result = NONE;
	s_unknown_4e0348 *globals = (s_unknown_4e0348 *)g_4e0348;

	if (globals->count > 0 && globals->table)
	{
		s_pair_table *table = globals->table;

		if (index >= 0 && index < table->count)
		{
			if (table->flags[index].flags & 4)
			{
				short i;

				for (i = 0; i < table->entry_count; i++)
				{
					s_pair_entry *entry = &table->entries[i];

					if (TEST_FIELD_BIT(entry->flag0) && index >= entry->lower && index <= entry->upper)
					{
						short j;

						for (j = 0; j < entry->range_count; j++)
						{
							s_pair_range *range = &entry->ranges[j];

							if (index >= range->lower && index <= range->upper)
							{
								short value = range->value;

								if (value >= 0 && value < entry->item_count)
								{
									result = entry->items[value].value;
								}
								break;
							}
						}
						break;
					}
				}
			}
		}
	}

	return result;
}
/* ---- the axis permutation tables ---- */
extern short const g_440bb8[4][3] =
{
	{ 2, 1, 0 }, { 2, 0, 1 }, { 0, 2, 1 }, { 1, 2, 0 }
};

// @retail 0x210d10
point3f *function_210d10(short a, byte b, real *in, point3f *out)
{
	long index = a * 2 + b;
	real z = in[g_440b94[index][2]];
	real y = in[g_440b94[index][1]];

	out->x = in[g_440b94[index][0]];
	out->y = y;
	out->z = z;
	return out;
}

// @retail 0x210d60
inline point3f *function_210d60(short a, byte b, real *in, point3f *out)
{
	long index = a * 2 + b;
	real z = in[g_440bb8[index][2]];
	real y = in[g_440bb8[index][1]];

	out->x = in[g_440bb8[index][0]];
	out->y = y;
	out->z = z;
	return out;
}

/* ---- points relative to an object's node ---- */
struct s_node_matrix_object
{
	byte unknown00[0x114];
	short nodes_size;
	short nodes_offset;
};

#pragma inline_depth(0)
// @retail 0x2104b0
bool function_2104b0(short output_index, point3f const *point, point3f *out)
{
	bool success = false;

	if (output_index != NONE)
	{
		s_output_entry *output = &g_4f93a0[output_index];

		if (output->object_index != NONE)
		{
			s_node_matrix_object *object = (s_node_matrix_object *)OBJECT_FROM_INDEX(output->object_index);
			short node_index = output->index;

			if (node_index >= 0 && node_index < (long)(object->nodes_size / sizeof(transform4x3f)))
			{
				point3f local;

				function_210d60((char)output->byte0a, output->byte0b, (real *)point, &local);
				transform4x3f_apply_point((transform4x3f *)((byte *)object + object->nodes_offset) + node_index, &local, out);
				success = true;
			}
			else
			{
				*out = *point;
			}
		}
		else
		{
			*out = *point;
		}
	}
	else
	{
		*out = *point;
		success = true;
	}

	return success;
}
#pragma inline_depth(255)

// @retail 0x210850
point3f *function_210850(s_type_c3b527 const *point, point3f *out)
{
	if (point->output_index != NONE)
	{
		if (!function_2104b0(point->output_index, &point->point, out))
			*out = point->point;
	}
	else
	{
		*out = point->point;
	}

	return out;
}

/* ---- the node bits ---- */
struct s_match_node
{
	byte unknown00[0x18];
	byte flags;
	byte unknown19[0x24 - 0x19];
};

struct s_match_nodes
{
	byte unknown00[0x5c];
	long count;
	s_match_node *nodes;
};

// @retail 0x210e20
void function_210e20(void)
{
	s_match_nodes *globals = (s_match_nodes *)g_4e0348;
	long i;

	for (i = 0; i < globals->count; i++)
	{
		if (globals->nodes[i].flags & 8)
		{
			((dword *)g_4f93a4)[i >> 5] &= ~(1 << (i & 0x1f));
		}
	}
}

struct s_object_with_values
{
	byte unknown00[0x1da];
	short value1da;
	short value1dc;
};

// @retail 0x210e80
void function_210e80(void)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;

	function_bae80(&state.iterator, 0x80, 0);

	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		s_object_with_values *object = (s_object_with_values *)OBJECT_FROM_INDEX(state.iterator.object_index);

		object->value1da = NONE;
		object->value1dc = 0;
	}
}
struct s_object_mapping_entry
{
	long identifier;
	short index;
	byte type;
	byte source;
	short value1da;
	short value1dc;
};

struct s_object_mapping_block
{
	long count;
	s_object_mapping_entry *entries;
};

struct s_object_mapping_root
{
	byte unknown000[0x220];
	s_object_mapping_block *mappings;
};

struct s_mapped_object_view
{
	byte unknown000[0xa4];
	long identifier;
	short index;
	byte type;
	byte source;
	byte unknown0ac[0x1da - 0xac];
	short value1da;
	short value1dc;
};

// @retail 0x210f00
void function_210f00(void)
{
	function_210e80();
	s_object_mapping_block *mappings = ((s_object_mapping_root *)g_4e0348)->mappings;
	for (long i = 0; i < mappings->count; i++)
	{
		s_object_mapping_entry *entry = &mappings->entries[i];
		struct
		{
			s_object *object;
			s_type_f1af8e iterator;
		} state;
		function_bae80(&state.iterator, 0x80, 0);
		while ((state.object = function_baeb0(&state.iterator)) != NULL)
		{
			s_mapped_object_view *object = (s_mapped_object_view *)OBJECT_FROM_INDEX(state.iterator.object_index);
			bool matches = true;
			matches &= entry->identifier == object->identifier;
			matches &= entry->source == object->source;
			matches &= entry->type == object->type;
			if (matches)
			{
				if (entry->source == 0)
					matches &= entry->index == object->index;
				if (matches)
				{
					object->value1da = entry->value1da;
					object->value1dc = entry->value1dc;
					break;
				}
			}
		}
	}
}

/* ---- the particle direction cells and the noise table (0x4f93bc) ---- */

struct s_cell
{
	byte active;
	byte unknown01[3];
	real value4;
	real value8;
	real valuec;
	real interpolation;
	vector3f direction;
};

struct s_cell_source
{
	byte unknown00[0x50];
	long tag_index;
	vector3f direction;
	real scale;
	byte unknown64[0x88 - 0x64];
};

struct s_cell_tag
{
	real value0;
	real value4;
	real value8;
	real valuec;
};

struct s_cell_globals
{
	byte unknown00[0x84];
	long count;
	s_cell_source *sources;
};

long g_4fa0c0;
short g_4f9cbc;
s_cell g_4f9cc0[8];
vector3f g_4f93bc[24][8];

void function_211fd0(vector3f *out, real const *position, real time, real scale);

struct s_cell_effect_tag
{
	real unknown00[4];
	real noise_scale;
	real noise_period;
	real damping;
};

// @retail 0x211b70
void function_211b70(short index, real const *position, byte flags, vector3f *out)
{
	(void)&position;
	byte const *flags_reference = &flags;
	byte mode = *flags_reference;
	if (index >= 0 && index < g_4f9cbc)
	{
		s_cell *cell = &g_4f9cc0[index];
		if (cell->active)
		{
			s_cell_globals *globals = (s_cell_globals *)g_4e0348;
			s_cell_effect_tag *tag = (s_cell_effect_tag *)g_4e3b44[globals->sources[index].tag_index & 0xffff].bytes;
			real noise_scale = 0.0f;
			if (!(mode & 1))
				noise_scale = tag->noise_scale;
			vector3f noise;
			function_211fd0(&noise, position, tag->noise_scale * cell->interpolation, tag->noise_period);
			out->i = cell->direction.i * (1.0f - noise_scale) + noise.i;
			out->j = cell->direction.j * (1.0f - noise_scale) + noise.j;
			out->k = cell->direction.k * (1.0f - noise_scale) + noise.k;
			if (mode & 2)
			{
				out->i *= 1.0f - tag->damping;
				out->j *= 1.0f - tag->damping;
				out->k *= 1.0f - tag->damping;
			}
		}
		else
			*out = *g_4687a4;
	}
	else
		*out = *g_4687a4;
}

// @retail 0x211b30
bool function_211b30(byte const *location, real const *position, byte flags, vector3f *out)
{
	(void)&position;
	(void)&flags;
	short index = NONE;
	short cluster = *(short const *)(location + 4);
	if (cluster != NONE)
	{
		byte *clusters = *(byte **)((byte *)g_4e0348 + 0xa0);
		index = (word)*(short *)(clusters + cluster * 0xb0 + 0x76);
	}
	function_211b70((short)index, position, flags, out);
	return false;
}

PRIVATE real random_step(void)
{
	return random_index(&g_4e7408->seed, 2) ? 0.01f : -0.01f;
}

PRIVATE real pin_real(real x, real lower, real upper)
{
	if (x < lower)
	{
		x = lower;
	}
	else if (x > upper)
	{
		x = upper;
	}

	return x;
}

// @retail 0x2118c0
void function_2118c0(void)
{
	s_cell_globals *globals = (s_cell_globals *)g_4e0348;
	short i;

	g_4fa0c0++;

	for (i = 0; i < globals->count; i++)
	{
		s_cell_source *source = &globals->sources[i];
		s_cell *cell = &g_4f9cc0[i];

		if (source->tag_index != NONE)
		{
			s_cell_tag *tag = (s_cell_tag *)g_4e3b44[source->tag_index & 0xffff].bytes;
			real azimuth;
			real elevation;
			real scale;
			long j;

			cell->value4 += random_step();
			cell->value4 = pin_real(cell->value4, 0.0f, 1.0f);

			cell->valuec += random_step();
			cell->valuec = pin_real(cell->valuec, -1.0f, 1.0f);

			cell->value8 += random_step();
			cell->value8 = pin_real(cell->value8, -1.0f, 1.0f);

			cell->interpolation = (tag->value4 - tag->value0) * cell->value4 + tag->value0;

			azimuth = (real)atan2(source->direction.j, source->direction.i);
			elevation = (real)atan2(source->direction.k, sqrt(source->direction.j * source->direction.j + source->direction.i * source->direction.i));
			elevation += tag->valuec * cell->valuec * 0.5f;
			azimuth += cell->value8 * tag->value8 * 0.5f;

			cell->direction.i = (real)cos(azimuth) * (real)cos(elevation);
			cell->direction.j = (real)sin(azimuth) * (real)cos(elevation);
			cell->direction.k = (real)sin(elevation);

			scale = source->scale * cell->interpolation;
			for (j = 0; j < 3; j++)
			{
				cell->direction.n[j] *= scale;
			}

			cell->active = true;
		}
		else
		{
			cell->active = false;
		}
	}

	g_4f9cbc = (short)globals->count;
}
// @retail 0x211cc0
void function_211cc0(void)
{
	dword *seed = &g_4e7408->unknown0;
	short k = 0;
	short m;
	short b;

	do
	{
		b = 0;

		do
		{
			*seed = *seed * 0x19660d + 0x3c6ef35f;
			g_4f93bc[b * 8 + k][0] = g_4417f0[(short)(((*seed >> 16) * 0x402) >> 16)];
			b++;
		}
		while (b < 3);

		k++;
	}
	while (k < 8);

	k = 0;

	do
	{
		m = 1;

		do
		{
			real previous = (real)(k - 1);
			real t = (real)m * 0.125f + (real)k;
			real ta = t - (previous + 2.0f);
			real tb = t - (previous + 1.0f);
			real tc = t - previous;

			b = 0;

			do
			{
				vector3f *p3 = &g_4f93bc[b * 8 + ((k + 2) & 7)][0];
				vector3f *p2 = &g_4f93bc[b * 8 + ((k + 1) & 7)][0];
				vector3f *p1 = &g_4f93bc[b * 8 + k][0];
				vector3f *p0 = &g_4f93bc[b * 8 + ((k - 1) & 7)][0];
				long i;

				for (i = 0; i < 3; i++)
				{
					real s0 = p3->n[i] - p2->n[i];
					real s1 = p2->n[i] - p1->n[i];
					real s2 = p1->n[i] - p0->n[i];
					real d2 = s1 - s2;
					real d3 = (s0 - s1) - d2;

					g_4f93bc[b * 8 + k][m].n[i] = ((d3 * ta * (1.0f / 3.0f) + d2) * tb * 0.5f + s2) * tc + p0->n[i];
				}

				b++;
			}
			while (b < 3);

			m++;
		}
		while (m < 8);

		k++;
	}
	while (k < 8);
}

// @retail 0x211fd0
void function_211fd0(vector3f *out, real const *position, real time, real scale)
{
	real frequencies[3] = { 0.1f, 0.2f, 0.07f };
	short i = 0;

	*out = *g_4687a4;
	scale *= 1.0f / 3.0f;

	do
	{
		real value;
		short index;
		vector3f *noise;

		value = ((real)g_4fa0c0 * frequencies[i] * time + position[i]) * 8.0f;
		*(dword *)&value &= 0x7fffffff;
		value += 8388608.0f;
		index = *(byte *)&value;
		index &= 0x3f;
		noise = &g_4f93bc[i * 8][index];

		out->i += noise->i;
		out->j += noise->j;
		out->k += noise->k;
		i++;
	}
	while (i < 3);

	out->i *= scale;
	out->j *= scale;
	out->k *= scale;
}
// @retail 0x212100
void function_212100(point3f const *p3, point3f const *p2, point3f const *p1, point3f const *p0, point3f *out, real a, real b, real c)
{
	real two_b = b * 2.0f;
	real three_b = b * 3.0f;
	real inverse_b = 1.0f / b;
	real t0 = c - a;
	real t1 = c - (a + b);
	real t2 = c - (two_b + a);
	long i;

	for (i = 0; i < 3; i++)
	{
		real s0 = p3->n[i] - p2->n[i];
		real s1 = p2->n[i] - p1->n[i];
		real s2 = p1->n[i] - p0->n[i];
		real d2 = s1 - s2;
		real d3 = (s0 - s1) - d2;

		out->n[i] = ((d3 * t2 / three_b + d2) * t1 / two_b + s2) * inverse_b * t0 + p0->n[i];
	}
}

// @retail 0x210a30
real function_210a30(s_type_c3b527 const *a, s_type_c3b527 const *b)
{
	real local_0;
	real local_1;
	real local_2;

	if (a->output_index == b->output_index)
	{
		local_0 = b->point.x - a->point.x;
		local_1 = b->point.y - a->point.y;
		local_2 = b->point.z - a->point.z;
	}
	else
	{
		point3f pa;
		point3f pb;

		function_210850(a, &pa);
		function_210850(b, &pb);
		local_1 = pb.x - pa.x;
		local_2 = pb.y - pa.y;
		local_0 = pb.z - pa.z;
	}

	return local_0 * local_0 + local_2 * local_2 + local_1 * local_1;
}

// @retail 0x210b60
real function_210b60(s_type_c3b527 const *a, point3f const *b)
{
	real local_0;
	real local_1;
	real local_2;

	if (a->output_index == NONE)
	{
		local_0 = b->x - a->point.x;
		local_1 = b->y - a->point.y;
		local_2 = b->z - a->point.z;
	}
	else
	{
		point3f point;

		function_210850(a, &point);
		local_1 = b->x - point.x;
		local_2 = b->y - point.y;
		local_0 = b->z - point.z;
	}

	return local_0 * local_0 + local_2 * local_2 + local_1 * local_1;
}

/* ---- vectors and distances between node points ---- */
static inline real node_point_magnitude3d(vector3f const *v)
{
	return (real)sqrt(v->j * v->j + (v->i * v->i + v->k * v->k));
}

#pragma inline_depth(0)
// @retail 0x2105b0
bool function_2105b0(short output_index, vector3f const *vector, vector3f *out)
{
	bool success = false;

	if (output_index != NONE)
	{
		s_output_entry *output = &g_4f93a0[output_index];

		if (output->object_index != NONE)
		{
			s_node_matrix_object *object = (s_node_matrix_object *)OBJECT_FROM_INDEX(output->object_index);
			short node_index = output->index;

			if (node_index >= 0 && node_index < (long)(object->nodes_size / sizeof(transform4x3f)))
			{
				vector3f local;

				function_210d60((char)output->byte0a, output->byte0b, (real *)vector, (point3f *)&local);
				function_142640((transform4x3f *)((byte *)object + object->nodes_offset) + node_index, &local, out);
				success = true;
			}
			else
			{
				*out = *vector;
			}
		}
		else
		{
			*out = *vector;
		}
	}
	else
	{
		*out = *vector;
		success = true;
	}

	return success;
}
#pragma inline_depth(255)

// @retail 0x210690
bool function_210690(short output_index, point3f const *point, point3f *out)
{
	bool success = false;

	if (output_index != NONE)
	{
		s_output_entry *output = &g_4f93a0[output_index];

		if (output->object_index != NONE)
		{
			s_node_matrix_object *object = (s_node_matrix_object *)OBJECT_FROM_INDEX(output->object_index);
			short node_index = output->index;

			if (node_index >= 0 && node_index < (long)(object->nodes_size / sizeof(transform4x3f)))
			{
				point3f local;

				function_142700((transform4x3f *)((byte *)object + object->nodes_offset) + node_index, point, &local);
				function_210d10((char)output->byte0a, output->byte0b, (real *)&local, out);
				success = true;
			}
			else
			{
				*out = *point;
			}
		}
		else
		{
			*out = *point;
		}
	}
	else
	{
		*out = *point;
		success = true;
	}

	return success;
}

// @retail 0x210770
bool function_210770(short output_index, vector3f const *vector, vector3f *out)
{
	bool success = false;

	if (output_index != NONE)
	{
		s_output_entry *output = &g_4f93a0[output_index];

		if (output->object_index != NONE)
		{
			s_node_matrix_object *object = (s_node_matrix_object *)OBJECT_FROM_INDEX(output->object_index);
			short node_index = output->index;

			if (node_index >= 0 && node_index < (long)(object->nodes_size / sizeof(transform4x3f)))
			{
				vector3f local;

				function_1427f0((transform4x3f *)((byte *)object + object->nodes_offset) + node_index, vector, &local);
				function_210d10((char)output->byte0a, output->byte0b, (real *)&local, (point3f *)out);
				success = true;
			}
			else
			{
				*out = *vector;
			}
		}
		else
		{
			*out = *vector;
		}
	}
	else
	{
		*out = *vector;
		success = true;
	}

	return success;
}

PRIVATE __forceinline real function_210ac1(real arg_0)
{
	__asm
	{
		fld arg_0
		fsqrt
	}
}

// @retail 0x210970
real function_210970(s_type_c3b527 const *a, s_type_c3b527 const *b)
{

	if (a->output_index == b->output_index)
	{
		vector3f v;
		vector3d_from_points3d(&a->point, &b->point, &v);
		return function_210ac1((v.k * v.k + v.i * v.i) + v.j * v.j);
	}
	else
	{
		vector3f v;
		point3f pa;
		point3f pb;

		function_210850(a, &pa);
		function_210850(b, &pb);
		vector3d_from_points3d(&pa, &pb, &v);
		return function_210ac1((v.j * v.j + v.k * v.k) + v.i * v.i);
	}

}


// @retail 0x210ac0
real function_210ac0(s_type_c3b527 const *a, point3f const *b)
{
	vector3f v;
	real local_0;
	if (a->output_index == NONE)
	{
		vector3d_from_points3d(&a->point, b, &v);
		local_0 = v.k * v.k + v.i * v.i;
	}
	else
	{
		point3f point;
		function_210850(a, &point);
		vector3d_from_points3d(&point, b, &v);
		local_0 = v.i * v.i + v.k * v.k;
	}
	return function_210ac1(local_0 + v.j * v.j);
}


// @retail 0x210be0
void function_210be0(s_type_c3b527 const *a, s_type_c3b527 const *b, vector3f *out)
{
	if (a->output_index == b->output_index)
	{
		vector3d_from_points3d(&a->point, &b->point, out);
		if (a->output_index != NONE)
			function_2105b0(a->output_index, out, out);
	}
	else
	{
		point3f pa;
		point3f pb;

		function_210850(a, &pa);
		function_210850(b, &pb);
		vector3d_from_points3d(&pa, &pb, out);
	}
}

// @retail 0x210c90
void function_210c90(point3f const *b, s_type_c3b527 const *a, vector3f *out)
{
	if (a->output_index == NONE)
	{
		vector3d_from_points3d(b, &a->point, out);
	}
	else
	{
		point3f point;

		function_210850(a, &point);
		vector3d_from_points3d(b, &point, out);
	}
}


struct s_2960e0_counts;
long function_2960e0(s_2960e0_counts *counts, long *objects, long maximum_count);

// @retail 0x210280
void function_210280(void)
{
    long objects[2000];
    long count = function_2960e0((s_2960e0_counts *)g_4e0350, objects, 2000);
    s_pair_table *table = NULL;
    s_unknown_4e0348 *globals = (s_unknown_4e0348 *)g_4e0348;
    if (globals->count > 0)
        table = globals->table;
    for (long i = 0; i < 100; i++)
        g_4f93a0[i].object_index = NONE;
    if (table)
    {
        for (short i = 0; i < count; i++)
        {
            long object_index = objects[i];
            if (object_index != NONE)
                function_2101d0(table, object_index);
        }
    }
}


point3f *function_b9dd0(long object_index, point3f *result);
real function_30bf0(vector3f *vector);
long function_25d810(long object_index, long actor_index, bool create);

// @retail 0x20e190
real function_20e190(long object_index)
{
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    long actor_index = *(long *)(object + 0x12c);
    real result = 0.0f;
    if (actor_index != NONE)
    {
        byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
        if (actor[0x2e0])
            return *(real *)(actor + 0x2e4);
        real maximum = 0.0f;
        bool found = false;
        s_record_pool *players = g_4e8c24;
        long index = NONE;
        for (;;)
        {
            index = record_pool_next_used(players, index + 1);
            if (index == NONE)
                break;
            byte *player = players->data + index * players->size;
            if (!player)
                break;
            long unit_index = *(long *)(player + 0x2c);
            if (unit_index != NONE)
            {
                real weight = 0.0f;
                found = true;
                point3f position;
                function_b9dd0(unit_index, &position);
                point3f *actor_position = (point3f *)(actor + 0x238);
                real y = position.y - actor_position->y;
                real x = position.x - actor_position->x;
                real z = position.z - actor_position->z;
                real squared = z * z + y * y + x * x;
                if (squared > 64.0f)
                    result = 0.0f;
                else
                {
                    byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
                    vector3f forward = *(vector3f *)(unit + 0x168);
                    vector3f direction;
                    direction.i = actor_position->x - position.x;
                    direction.j = actor_position->y - position.y;
                    direction.k = actor_position->z - position.z;
                    if (function_30bf0(&direction) > 0.0f)
                    {
                        weight = 1.0f - squared * 0.015625f;
                        real dot = forward.k * direction.k + forward.j * direction.j + forward.i * direction.i;
                        if (dot < 0.0f)
                        {
                            real fraction = dot + 1.0f;
                            fraction = fraction < 0.0f ? 0.0f : fraction > 1.0f ? 1.0f : fraction;
                            weight = (fraction + 1.0f) * weight * 0.5f;
                        }
                        long reference_index = function_25d810(unit_index, *(long *)(object + 0x12c), false);
                        if (reference_index != NONE && (signed char)g_502418->data[(reference_index & 0xffff) * 0x3c + 0x27] < 1)
                            weight *= 0.5f;
                    }
                    result = weight * 0.3f + 0.7f;
                }
                if (result > maximum)
                    maximum = result;
            }
        }
        if (!found)
            maximum = 1.0f;
        result = maximum;
        *(real *)(actor + 0x2e4) = result;
        actor[0x2e0] = true;
    }
    return result;
}


long const g_4458f0[12] =
{
    0, 0x0900077e, 0x05000768, 0x0e00002a, 0x0d00002b, 0x05000769,
    0x0500076a, 0x0500076b, 0x0400076c, 0x0400076d, 0x0700076e, 0x0800076f
};

bool function_1f3610(long actor_index, short value);
bool function_1f57f0(long actor_index, long animation, long const *target);
bool __stdcall function_110ab0(long unit_index);
long function_1469f0(real seconds);
void function_20d870(long actor_index, short channel, short value);

PRIVATE inline long audio_response_ticks(real seconds)
{
    real ticks = g_510c54->field_2_3 * seconds;
    long result;
    __asm
    {
        fld ticks
        fistp result
    }
    return result;
}

// @retail 0x20d220
void function_20d220(long record_index, long object_index, short type, long const *target)
{
    (void)&object_index;
    (void)&type;
    (void)&target;
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    byte *other = NULL;
    byte *actor = NULL;
    byte *record = NULL;
    long target_index = target[1];
    if (target_index != NONE)
        other = (byte *)function_badc0(target_index, NONE);
    if (record_index != NONE)
        record = g_4f9398->data + (record_index & 0xffff) * 0x54;
    long actor_index = *(long *)(object + 0x12c);
    if (actor_index != NONE)
        actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
    if (other && actor && target_index != object_index)
    {
        real seconds = 1.5f;
        if (record)
            seconds = *(short *)(record + 0x1c) * g_510c54->rate + 1.0f;
        if (*(short *)(actor + 0x684) <= 0)
        {
            *(short *)(actor + 0x686) = (short)audio_response_ticks(seconds);
            *(short *)(actor + 0x688) = 6;
            *(long *)(actor + 0x68c) = target[1];
            *(short *)(actor + 0x684) = 0;
        }
    }
    if (type != NONE)
    {
        s_audio_priority_table *table = audio_priority_table();
        if (table)
        {
            byte *definition = (byte *)&table->entries[type];
            if (actor && *(real *)(definition + 0x38) > 0.0f)
                function_1f3610(*(long *)(object + 0x12c), (short)audio_response_ticks(*(real *)(definition + 0x38)));
            if (*(real *)(definition + 0x3c) > 0.0f && other &&
                ((1 << other[0xaa]) & 3) && *(long *)(other + 0x12c) != NONE)
            {
                long delay = function_1469f0(*(real *)(definition + 0x3c));
                function_1f3610(*(long *)(other + 0x12c), (short)delay);
            }
            if (actor && !function_110ab0(object_index))
            {
                short animation = *(short *)(definition + 0x18);
                if (animation >= 0 && animation < 12)
                {
                    switch (animation)
                    {
                    case 1: case 3: case 4: case 5: case 6: case 7: case 9: case 10:
                        if (other)
                        {
                            if (*(short *)(actor + 0x684) <= 1)
                            {
                                *(short *)(actor + 0x686) = (short)audio_response_ticks(2.0f);
                                *(short *)(actor + 0x688) = 6;
                                *(long *)(actor + 0x68c) = target[1];
                                *(short *)(actor + 0x684) = 1;
                            }
                            *(long *)(actor + 0x6c8) = g_4458f0[*(short *)(definition + 0x18)];
                            *(bool *)(actor + 0x6cc) = true;
                            break;
                        }
                    case 2: case 8: case 11:
                        function_1f57f0(*(long *)(object + 0x12c), g_4458f0[animation], NULL);
                        break;
                    }
                }
            }
            if (record && actor)
            {
                *(short *)(actor + 0x620) = *(short *)(record + 0x1c) / 2;
                short channel = *(short *)(definition + 0x40);
                if (channel > 0)
                    function_20d870(*(long *)(object + 0x12c), channel - 1,
                        g_510c54->field_2_3 + *(short *)(record + 0x1c));
            }
        }
    }
}

struct s_game_allegiance_globals;
extern s_game_allegiance_globals *g_4f55ec;
bool function_0bfe60(dword const *flags, long bit);
bool function_15e020(short a, short b);
short function_1a6fe0(long owner_index, short type);
dword function_1a7ad0(long owner_index);
long function_114440(long unit_index);
bool function_1144a0(long unit_index, short entry_index, long *tag_index);
long function_25d770(long actor_index, long object_index);
real function_1c9e50(short index);
short function_272ab0(long actor_index);
struct s_squad_definition_entry;
struct s_squad_definition_view;
s_squad_definition_entry *function_203330(s_squad_definition_view const *squad);

PRIVATE __forceinline bool audio_team_is_hostile(short team)
{
    bool result = true;
    if (team != NONE)
    {
        long mode = g_4e6948->state;
        if (mode == 1)
        {
            if (team >= 0 && team < 16)
                result = !function_0bfe60((dword const *)((byte *)g_4f55ec + 0xc4), team * 16 + 1);
        }
        else if (mode == 2)
            result = function_15e020(team, 1);
        else
            result = team != 1;
    }
    return result;
}

PRIVATE __forceinline bool audio_target_is_known(long actor_index, long object_index)
{
    if (object_index != NONE)
    {
        long index = function_25d770(actor_index, object_index);
        if (index != NONE)
        {
            short state = *(short *)(g_502418->data + (index & 0xffff) * 0x3c + 0x24);
            if (state >= 1 && state <= 2)
                return true;
        }
    }
    return false;
}

PRIVATE __forceinline bool audio_target_is_current(byte *actor, long object_index)
{
    if (object_index != NONE)
    {
        long index = *(long *)(actor + 0x338);
        if (index != NONE)
            return *(long *)(g_502418->data + (index & 0xffff) * 0x3c + 0x20) == object_index;
    }
    return false;
}

// @retail 0x20d8c0
short function_20d8c0(long source_index, s_audio_priority_table *table, byte const *request,
    long object_index, long target_index, long previous_index, long *tag_index, real *weight)
{
    (void)&table;
    (void)&request;
    (void)&object_index;
    (void)&target_index;
    (void)&previous_index;
    (void)&tag_index;
    (void)&weight;
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    byte *actor = NULL;
    byte *source = NULL;
    byte *target = NULL;
    short type = *(short const *)(request + 2);
    bool eligible = true;
    real scale = 1.0f;
    long source_unit_index = NONE;
    if (target_index != NONE)
        target = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
    if (source_index != NONE)
    {
        byte *candidate = *(byte **)(g_4e0300->data + (source_index & 0xffff) * 12 + 8);
        if ((1 << candidate[0xaa]) & 3)
        {
            source_unit_index = source_index;
            source = candidate;
        }
    }
    if (type >= 0 && type < table->count)
    {
        byte *definition = (byte *)&table->entries[type];
        short priority = *(short *)(definition + 0xa);
        long time = g_510c54->game_time;
        short team = *(short *)(object + 0x138);
        byte *queue = (byte *)g_4f939c + (short)function_20f040(team) * 0x7dc;
        long actor_index = *(long *)(object + 0x12c);
        if (actor_index != NONE)
        {
            actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
            if (actor && (actor[0x223] ||
                ((actor[0x225] || function_1a6fe0(actor_index, 0x38) != NONE) && priority < 13)))
                eligible = false;
        }
        if ((bool)((*(dword *)(object + 0x134) >> 3) & 1))
            eligible = false;
        if ((!object[0xaa] && object[0x39d]) || !eligible)
            goto failed;
        if (previous_index != NONE && function_114440(object_index) ==
            *(long *)(g_4f9398->data + (previous_index & 0xffff) * 0x54 + 0x4c))
            goto failed;
        if (actor)
        {
            dword mask = function_1a7ad0(actor_index);
            if ((*(dword const *)(request + 0x3c) & mask) != mask)
            {
                type = NONE;
                eligible = false;
                goto checked;
            }
            long action = *(long *)(actor + 0x858);
            if (action != NONE)
                eligible = *(bool *)(g_502408->data + (action & 0xffff) * 0xd4 + 0x82);
        }
        if (!eligible)
            goto checked;
        if ((request[0xa] & 1) && object_index != target_index && actor &&
            !audio_target_is_known(actor_index, target_index))
            goto invalid;
        if ((request[0xa] & 2) && object_index != source_unit_index && actor &&
            !audio_target_is_known(*(long *)(object + 0x12c), source_unit_index))
            goto invalid;
        {
            bool friendly = false;
            if (!audio_team_is_hostile(*(short *)(object + 0x138)))
                friendly = true;
            else if (actor && *(long *)(actor + 0x7c) != NONE)
            {
                long clump = *(long *)(actor + 0x7c);
                friendly = *(short *)(g_502420->data + (clump & 0xffff) * 0x50 + 0x10) > 1;
            }
            if ((bool)(((dword)request[0xa] >> 2) & 1))
            {
                if (!friendly)
                    goto invalid;
            }
            else if (friendly)
                scale = 0.6f;
        }
        {
            word flags = *(word const *)(request + 0xa);
            if ((flags & 8) && actor)
            {
                if (!audio_target_is_current(actor, target_index))
                    goto invalid;
                scale *= 1.1f;
            }
            if ((flags & 0x10) && actor)
            {
                if (!audio_target_is_current(actor, source_unit_index))
                    goto invalid;
                scale *= 1.1f;
            }
            if ((flags & 0x20) && (!source || *(long *)(source + 0x13c) == NONE) &&
                audio_team_is_hostile(*(short *)(object + 0x138)))
                goto ineligible;
            flags = *(word const *)(request + 0xa);
            if ((flags & 0x40) && (!actor ||
                (function_1a6fe0(*(long *)(object + 0x12c), 0x22) == NONE &&
                 function_1a6fe0(*(long *)(object + 0x12c), 0x1b) == NONE)))
                goto ineligible;
            if (flags & 0x80)
            {
                eligible = false;
                if (!actor || *(long *)(actor + 0x30) == NONE)
                    goto checked;
                byte *squad = g_51e9d8->data + (*(long *)(actor + 0x30) & 0xffff) * 0x98;
                byte *local_b3aa32_2 = (byte *)function_203330((s_squad_definition_view const *)squad);
                if (local_b3aa32_2)
                    eligible = (bool)((*(dword *)(local_b3aa32_2 + 0x24) >> 4) & 1);
            }
            if (!eligible)
                goto checked;
            if (flags & 0x100)
            {
                eligible = false;
                if (!source || *(long *)(source + 0x13c) == NONE)
                    goto checked;
                team = *(short *)(object + 0x138);
                if (team == 1)
                    eligible = true;
                else
                {
                    if (audio_team_is_hostile(team))
                        goto checked;
                    byte *player = g_4e8c24->data + (*(long *)(source + 0x13c) & 0xffff) * 0x21c;
                    switch (team)
                    {
                    case 2: eligible = player[0x88] == 0; break;
                    case 3: case 7: eligible = player[0x88] == 1; break;
                    }
                }
            }
            if (!eligible)
                goto checked;
        }
        if (*(short const *)(request + 0x18) > 0 &&
            function_1c9e50(*(short const *)(request + 0x18)) > *(real *)(actor + 0x3d8))
            goto invalid;
        if (actor)
        {
            short current = function_272ab0(*(long *)(object + 0x12c));
            short required = *(short const *)(request + 0x1a);
            if (required != current)
            {
                if (required)
                    goto invalid;
                scale *= 0.6f;
            }
        }
        else if (*(short const *)(request + 0x1a))
            goto invalid;
        goto checked;
invalid:
        type = NONE;
ineligible:
        eligible = false;
checked:
        actor_index = *(long *)(object + 0x12c);
        if (actor_index != NONE &&
            *(short *)(g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 0x86) > *(short *)(definition + 0x16))
        {
            type = NONE;
            goto failed;
        }
        if (eligible)
        {
            real result_weight = function_20f1a0(object_index, type) * scale;
            if (*(long *)(object + 0x13c) != NONE)
            {
                type = NONE;
                goto failed;
            }
            real probability = *(real *)(definition + 0x48);
            if ((target && *(long *)(target + 0x13c) != NONE) ||
                (source && *(long *)(source + 0x13c) != NONE))
                probability = *(real *)(definition + 0x44);
            g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
            dword random = g_4e7408->unknown0 >> 16;
            if (probability > (real)random * (1.0f / 65535.0f))
                goto failed;
            while (type != NONE && definition)
            {
                long tag = NONE;
                if (((long *)queue)[type] <= time && function_1144a0(object_index, type, &tag))
                {
                    *weight = result_weight;
                    *tag_index = tag;
                    return type;
                }
                type = *(short *)(definition + 8);
                result_weight *= 0.8f;
                if (type != NONE)
                    definition = (byte *)&table->entries[type];
            }
        }
    }
    else
        type = NONE;
failed:
    *weight = 0.0f;
    *tag_index = NONE;
    return type;
}

#include "unknown_272b70.h"

// @retail 0x20e460
long function_20e460(long ai_index, s_audio_priority_table *table, byte const *request,
    long previous_index, long *tag_index, short *type, real *weight, real *distance_weight)
{
    (void)&table; (void)&request; (void)&previous_index;
    (void)&tag_index; (void)&type; (void)&weight; (void)&distance_weight;
    real best = 0.0f;
    long chosen_tag = NONE;
    long chosen_type = NONE;
    long chosen = NONE;
    real chosen_weight = 0.0f;
    real chosen_distance = 0.0f;
    s_ai_actor_iterator iterator;
    ai_actor_iterator_new(ai_index, &iterator);
    s_actor_datum *actor;
    while ((actor = ai_actor_iterator_next(&iterator)) != NULL)
    {
        long tag = NONE;
        real candidate_weight = 0.0f;
        if (actor->unit_index != NONE)
        {
            short candidate = function_20d8c0(NONE, table, request, actor->unit_index,
                NONE, previous_index, &tag, &candidate_weight);
            if (candidate != NONE && candidate_weight > 0.0f)
            {
                real distance = function_20e190(actor->unit_index);
                real score = distance * candidate_weight;
                if (score > best)
                {
                    best = score;
                    chosen_weight = candidate_weight;
                    chosen_distance = distance;
                    chosen = actor->unit_index;
                    chosen_type = candidate;
                    chosen_tag = tag;
                }
            }
        }
    }
    *tag_index = chosen_tag;
    *type = (short)chosen_type;
    *weight = chosen_weight;
    *distance_weight = chosen_distance;
    return chosen;
}

// @retail 0x20eeb0
long function_20eeb0(s_audio_priority_table *table, byte const *request, long target_index,
    long source_index, long previous_index, long *tag_index, short *type, real *weight, real *distance_weight)
{
    (void)&table; (void)&request; (void)&target_index; (void)&source_index; (void)&previous_index;
    (void)&tag_index; (void)&type; (void)&weight; (void)&distance_weight;
    real best = 0.0f;
    long chosen = NONE;
    real chosen_weight = 0.0f;
    real chosen_distance = 0.0f;
    long chosen_type = NONE;
    long chosen_tag = NONE;
    if (target_index != NONE)
    {
        byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
        long actor_index = *(long *)(object + 0x12c);
        if (actor_index != NONE)
        {
            byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
            long clump_index = *(long *)(actor + 0x7c);
            if (clump_index != NONE)
            {
                long index = *(long *)(g_502420->data + (clump_index & 0xffff) * 0x50 + 0x18);
                while (index != NONE)
                {
                    actor = g_4f55f0->data + (index & 0xffff) * 0x888;
                    long unit_index = *(long *)(actor + 0x18);
                    index = *(long *)(actor + 0x80);
                    if (unit_index != NONE)
                    {
                        real candidate_weight;
                        long tag;
                        short candidate = function_20d8c0(source_index, table, request, unit_index,
                            target_index, previous_index, &tag, &candidate_weight);
                        if (candidate != NONE && candidate_weight > 0.0f)
                        {
                            real distance = function_20e190(*(long *)(actor + 0x18));
                            real score = distance * candidate_weight;
                            if (score > best)
                            {
                                best = score;
                                chosen_weight = candidate_weight;
                                chosen_distance = distance;
                                chosen_type = candidate;
                                chosen_tag = tag;
                                chosen = *(long *)(actor + 0x18);
                            }
                        }
                    }
                }
            }
        }
    }
    *tag_index = chosen_tag;
    *type = (short)chosen_type;
    *weight = chosen_weight;
    *distance_weight = chosen_distance;
    return chosen;
}
#include "unknown_1e46c0.h"
bool function_1df560(short team_a, short team_b);

// @retail 0x20e580
long function_20e580(long ai_index, s_audio_priority_table *table, byte const *request,
    long previous_index, long *tag_index, short *type, real *weight, real *distance_weight)
{
    (void)&table; (void)&request; (void)&previous_index;
    (void)&tag_index; (void)&type; (void)&weight; (void)&distance_weight;
    real best = 0.0f;
    long chosen = NONE;
    long chosen_tag = NONE;
    long chosen_type = NONE;
    real chosen_weight = 0.0f;
    real chosen_distance = 0.0f;
    struct
    {
        s_actor_iterator field_0;
        s_ai_actor_iterator field_18;
    } local_0;
    ai_actor_iterator_new(ai_index, &local_0.field_18);
    s_actor_datum *first = ai_actor_iterator_next(&local_0.field_18);
    if (first)
    {
        
        function_x66da2b(&local_0.field_0, true);
        short team = *(short *)((byte *)first + 0x24);
        s_actor_datum *actor;
        while ((actor = (s_actor_datum *)function_1e46c0(&local_0.field_0)) != NULL)
        {
            short candidate_team = *(short *)((byte *)actor + 0x24);
            if (team == candidate_team || !function_1df560(team, candidate_team))
                continue;
        long tag = NONE;
        real candidate_weight = 0.0f;
        if (actor->unit_index != NONE)
        {
            short candidate = function_20d8c0(NONE, table, request, actor->unit_index,
                NONE, previous_index, &tag, &candidate_weight);
            if (candidate != NONE && candidate_weight > 0.0f)
            {
                real distance = function_20e190(actor->unit_index);
                real score = distance * candidate_weight;
                if (score > best)
                {
                    best = score;
                    chosen_weight = candidate_weight;
                    chosen_distance = distance;
                    chosen = actor->unit_index;
                    chosen_type = candidate;
                    chosen_tag = tag;
                }
            }
        }
    }
    }
    *tag_index = chosen_tag;
    *type = (short)chosen_type;
    *weight = chosen_weight;
    *distance_weight = chosen_distance;
    return chosen;
}


#include "units.h"
#include "slot_handler.h"
long function_1b8c80(long object_index);
bool function_277790(long actor_index, long other_actor_index);

// @retail 0x20ea60
long function_20ea60(s_audio_priority_table *table, byte const *request, long target_index,
    long source_index, long previous_index, long *tag_index, short *type, real *weight, real *distance_weight)
{
    (void)&table; (void)&request; (void)&target_index; (void)&source_index; (void)&previous_index;
    (void)&tag_index; (void)&type; (void)&weight; (void)&distance_weight;
    byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
    long chosen = NONE;
    real best = 0.0f;
    real chosen_weight = 0.0f;
    real chosen_distance = 0.0f;
    short chosen_type = NONE;
    long chosen_tag = NONE;
    long parent = *(long *)(object + 0x14);
    if (parent != NONE && *(short *)(object + 0x1fc) != NONE &&
        g_4e0300->data[(parent & 0xffff) * 12 + 3] == 1)
    {
        s_object_seat seats[64];
        union { long field_0; real field_00; } local_0;
        local_0.field_0 = 0;
        function_c8a40(function_1b8c80(parent), seats, (short *)&local_0.field_0, 64);
        if ((short)local_0.field_0 > 0)
        {
            s_object_seat *seat = seats;
            long remaining = (word)local_0.field_0;
            do
            {
                long candidate_index = function_c8f60(seat->object_index, seat->seat_index);
                if (candidate_index != target_index && candidate_index != source_index && candidate_index != NONE)
                {
                    byte *candidate_object = *(byte **)(g_4e0300->data + (candidate_index & 0xffff) * 12 + 8);
                    if (!function_1df560(*(short *)(candidate_object + 0x138), *(short *)(object + 0x138)) &&
                        *(long *)(candidate_object + 0x12c) != NONE)
                    {
                        long tag;
                        
                        short candidate = function_20d8c0(source_index, table, request, candidate_index,
                            target_index, previous_index, &tag, &local_0.field_00);
                        if ((short)candidate != NONE && local_0.field_00 > 0.0f)
                        {
                            real distance = function_20e190(candidate_index);
                            real score = distance * local_0.field_00;
                            if (score > best)
                            {
                                best = score;
                                chosen_weight = local_0.field_00;
                                chosen_distance = distance;
                                chosen_type = candidate;
                                chosen_tag = tag;
                                chosen = candidate_index;
                            }
                        }
                    }
                }
                ++seat;
            } while (--remaining);
        }
    }
    *tag_index = chosen_tag;
    *type = (short)chosen_type;
    *weight = chosen_weight;
    *distance_weight = chosen_distance;
    return chosen;
}

// @retail 0x20ec80
long function_20ec80(long target_index, s_audio_priority_table *table, byte const *request,
    long source_index, long previous_index, bool require_related, long *tag_index,
    short *type, real *weight, real *distance_weight)
{
    (void)&table; (void)&request; (void)&source_index; (void)&previous_index; (void)&require_related;
    (void)&tag_index; (void)&type; (void)&weight; (void)&distance_weight;
    long chosen = NONE;
    real best = 0.0f;
    real chosen_weight = 0.0f;
    real chosen_distance = 0.0f;
    long chosen_type = NONE;
    long chosen_tag = NONE;
    if (target_index != NONE)
    {
        byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
        long actor_index = *(long *)(object + 0x12c);
        if (actor_index != NONE)
        {
            byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
            short slot = *(short *)(actor + 0x190);
            while (slot >= 0)
            {
                short handler_type = *(short *)(actor + 0x90 + slot * 0x40);
                if (handler_type != NONE)
                {
                    s_slot_handler *handler = g_46eeb8[handler_type];
                    if ((handler->kind == 2 || handler->kind == 1) && *(byte *)&handler->unknown3c)
                        break;
                }
                --slot;
            }
            if (slot >= 0)
            {
                long action_index = *(long *)(actor + 0xa0 + slot * 0x40);
                byte *entry = g_502424->data + (action_index & 0xffff) * 0xbc + 4;
                long remaining = 10;
                do
                {
                    long index = *(long *)entry;
                    if (index != NONE && *(short *)(entry + 4) == 3 && index != *(long *)(object + 0x12c) &&
                        (!require_related || function_277790(index, *(long *)(object + 0x12c))))
                    {
                        byte *candidate_actor = g_4f55f0->data + (index & 0xffff) * 0x888;
                        long candidate_index = *(long *)(candidate_actor + 0x18);
                        if (candidate_index != NONE)
                        {
                            long tag;
                            real candidate_weight;
                            short candidate = function_20d8c0(source_index, table, request, candidate_index,
                                target_index, previous_index, &tag, &candidate_weight);
                            if (candidate != NONE && candidate_weight > 0.0f)
                            {
                                real distance = function_20e190(*(long *)(candidate_actor + 0x18));
                                real score = distance * candidate_weight;
                                if (score > best)
                                {
                                    best = score;
                                    chosen_weight = candidate_weight;
                                    chosen_distance = distance;
                                    chosen_type = candidate;
                                    chosen_tag = tag;
                                    chosen = *(long *)(candidate_actor + 0x18);
                                }
                            }
                        }
                    }
                    entry += 0xc;
                } while (--remaining);
            }
        }
    }
    *tag_index = chosen_tag;
    *type = (short)chosen_type;
    *weight = chosen_weight;
    *distance_weight = chosen_distance;
    return chosen;
}

struct s_actor_point_request
{
    byte field_0[0x14];
    s_record_pool *pool;
    long iterator_index;
    long datum_index;
    long group_index;
    long actor_index;
    long next_actor_index;
    short type;
    short mode;
    real radius;
    real radius_squared;
    long index;
    point3f point;
    long result_index;
    real squared_distance;
};

struct s_actor_moving;
void function_1e4770(s_actor_point_request *request, short mode, point3f const *point, short type, real radius);
s_actor_moving *function_1e47d0(s_actor_point_request *request);
point3f *function_b9dd0(long object_index, point3f *result);

// @retail 0x20e720
long function_20e720(bool same_team, long ai_index, s_audio_priority_table *table, byte const *request,
    long target_index, long source_index, long previous_index, bool require_related, bool same_type,
    long *tag_index, short *type, real *weight, real *distance_weight)
{
    (void)&table; (void)&request; (void)&target_index; (void)&source_index; (void)&previous_index;
    (void)&require_related; (void)&same_type;
    (void)&tag_index; (void)&type; (void)&weight; (void)&distance_weight;
    long chosen = NONE;
    if (target_index == NONE)
    {
        if (ai_index != NONE)
        {
            if (same_team)
                chosen = function_20e460(ai_index, table, request, previous_index, tag_index, type, weight, distance_weight);
            else
                chosen = function_20e580(ai_index, table, request, previous_index, tag_index, type, weight, distance_weight);
        }
    }
    else
    {
        byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
        real best = 0.0f;
        real chosen_weight = 0.0f;
        real chosen_distance = 0.0f;
        long chosen_type = NONE;
        long chosen_tag = NONE;
        byte *actor = NULL;
        long actor_index = *(long *)(object + 0x12c);
        if (actor_index != NONE)
            actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
        point3f position;
        function_b9dd0(target_index, &position);
        s_actor_point_request iterator;
        function_1e4770(&iterator, *(short *)(object + 0x138), &position, !same_team, 8.0f);
        byte *candidate_actor;
        while ((candidate_actor = (byte *)function_1e47d0(&iterator)) != NULL)
        {
            long candidate_index = *(long *)(candidate_actor + 0x18);
            if (candidate_index == target_index || candidate_index == source_index || candidate_index == NONE)
                continue;
            if (actor)
            {
                if (require_related)
                {
                    if (!function_277790(iterator.result_index, *(long *)(object + 0x12c)))
                        continue;
                }
                else if (same_type && *(short *)(actor + 4) != *(short *)(candidate_actor + 4))
                    continue;
            }
            long tag;
            real candidate_weight;
            short candidate = function_20d8c0(source_index, table, request, candidate_index,
                target_index, previous_index, &tag, &candidate_weight);
            if (candidate != NONE && candidate_weight > 0.0f)
            {
                real distance = function_20e190(*(long *)(candidate_actor + 0x18));
                real x = position.x - *(real *)(candidate_actor + 0x238);
                real y = position.y - *(real *)(candidate_actor + 0x23c);
                real z = position.z - *(real *)(candidate_actor + 0x240);
                real squared = z * z + x * x + y * y;
                real score = distance * candidate_weight;
                if (squared < 0.0f)
                    squared = 0.0f;
                else if (squared > 64.0f)
                    squared = 64.0f;
                score *= (1.0f - squared * 0.015625f) * 0.5f + 1.0f;
                if (score > best)
                {
                    best = score;
                    chosen_weight = candidate_weight;
                    chosen_distance = distance;
                    chosen = *(long *)(candidate_actor + 0x18);
                    chosen_type = candidate;
                    chosen_tag = tag;
                }
            }
        }
        *tag_index = chosen_tag;
        *type = (short)chosen_type;
        *weight = chosen_weight;
        *distance_weight = chosen_distance;
    }
    return chosen;
}

bool __stdcall function_20bbb0(short condition, long object_index, real *weight);

struct s_audio_selection
{
    short type;
    short unknown02;
    long object_index;
    long target_index;
    long tag_index;
    real weight;
};

// @retail 0x20c440
void function_20c440(byte const *request, s_audio_priority_table *table, long target_index,
    long source_index, long previous_index, long ai_index, short category, short first_state,
    short second_state, short other_category, s_audio_selection *results, short *count)
{
    long const *source_reference = &source_index;
    (void)&table; (void)&target_index; (void)&(*source_reference); (void)&previous_index; (void)&ai_index;
    (void)&category; (void)&first_state; (void)&second_state; (void)&other_category;
    (void)&results; (void)&count;
    long chosen = NONE;
    short chosen_type = NONE;
    long tag = NONE;
    real base_weight = 0.0f;
    real scale = 1.0f;
    real weight = 0.0f;
    real distance = 0.0f;
    long source_unit = NONE;
    if ((*source_reference) != NONE && ((1 << g_4e0300->data[((*source_reference) & 0xffff) * 12 + 3]) & 3))
        source_unit = (*source_reference);
    short entry = *(short const *)(request + 2);
    if (entry < 0 || entry >= table->count)
        return;
    byte *definition = (byte *)&table->entries[entry];
    base_weight = *(real *)(definition + 0x34);
    short required = *(short const *)(request + 0x14);
    if (!required)
    {
        if (category != 0 && category != NONE)
            scale = 0.8f;
    }
    else if (required != category)
        return;
    required = *(short const *)(request + 0x16);
    if (!required)
    {
        if (other_category != NONE && other_category != 0)
            scale *= 0.8f;
    }
    else if (required != other_category)
        return;
    required = *(short const *)(request + 0x20) - 1;
    short other_required = *(short const *)(request + 0x22) - 1;
    if (first_state != NONE && required == NONE)
        scale *= 0.6f;
    else if (first_state != required)
        return;
    if (second_state != NONE && other_required == NONE)
        scale *= 0.6f;
    else if (second_state != other_required)
        return;
    bool eligible = function_20bbb0(*(short const *)(request + 0x24), (*source_reference), &scale);
    if (eligible)
        eligible = function_20bbb0(*(short const *)(request + 0x26), target_index, &scale);
    if (eligible && *(long const *)(request + 0x28))
        eligible = function_20c2f0((*source_reference), *(long const *)(request + 0x28), &scale);
    if (eligible && *(long const *)(request + 0x30))
        eligible = function_20c2f0(target_index, *(long const *)(request + 0x30), &scale);
    if (eligible && *(short const *)(request + 0x2c))
    {
        eligible = false;
        if (target_index != NONE && (*source_reference) != NONE)
        {
            point3f target_position;
            point3f source_position;
            function_b9dd0(target_index, &target_position);
            function_b9dd0((*source_reference), &source_position);
            real y = source_position.y - target_position.y;
            real z = source_position.z - target_position.z;
            real x = source_position.x - target_position.x;
            real squared = z * z + y * y + x * x;
            switch (*(short const *)(request + 0x2c))
            {
            case 1: eligible = squared <= 1.0f; break;
            case 2: eligible = squared <= 6.25f && squared > 1.0f; break;
            case 3: eligible = squared <= 25.0f && squared > 6.25f; break;
            case 4: eligible = squared <= 100.0f && squared > 25.0f; break;
            case 5: eligible = squared > 100.0f; break;
            case 6:
                if (squared < 9.0f)
                {
                    byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
                    real dot = *(real *)(object + 0x78) * z + *(real *)(object + 0x74) * y + *(real *)(object + 0x70) * x;
                    eligible = dot > 0.0f;
                }
                break;
            case 7:
                if (squared < 9.0f)
                {
                    byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
                    real dot = *(real *)(object + 0x78) * z + *(real *)(object + 0x74) * y + x * *(real *)(object + 0x70);
                    eligible = dot < 0.0f;
                }
                break;
            case 8: eligible = z > 1.0f; break;
            case 9: eligible = z < -1.0f; break;
            }
            if (eligible)
                scale *= 1.5f;
        }
    }
    base_weight = scale * base_weight;
    if (eligible && base_weight > 0.0f && definition)
    {
        long response_target = NONE;
        switch (*(short const *)(request + 0xc))
        {
        case 0: response_target = target_index; break;
        case 1: response_target = (*source_reference); break;
        }
        switch (*(short const *)(request + 8))
        {
        case 0:
            if (target_index != NONE)
            {
                chosen = target_index;
                chosen_type = function_20d8c0((*source_reference), table, request, target_index, target_index, previous_index, &tag, &weight);
                if (weight > 0.0f)
                    distance = function_20e190(target_index);
            }
            break;
        case 1:
            if (source_unit != NONE)
            {
                chosen = source_unit;
                chosen_type = function_20d8c0((*source_reference), table, request, source_unit, target_index, previous_index, &tag, &weight);
                if (weight > 0.0f)
                    distance = function_20e190(source_unit);
            }
            break;
        case 2:
            chosen = function_20e720(true, ai_index, table, request, target_index, (*source_reference), previous_index,
                false, false, &tag, &chosen_type, &weight, &distance);
            break;
        case 3:
            break;
        case 4:
            chosen = function_20e720(false, ai_index, table, request, target_index, (*source_reference), previous_index,
                false, false, &tag, &chosen_type, &weight, &distance);
            break;
        case 5:
            if (target_index != NONE)
                chosen = function_20ea60(table, request, target_index, (*source_reference), previous_index, &tag, &chosen_type, &weight, &distance);
            break;
        case 6:
            if (target_index != NONE)
                chosen = function_20ec80(target_index, table, request, (*source_reference), previous_index, false, &tag, &chosen_type, &weight, &distance);
            break;
        case 7:
            if (ai_index != NONE)
                chosen = function_20e460(ai_index, table, request, previous_index, &tag, &chosen_type, &weight, &distance);
            break;
        case 8:
            chosen = function_20e720(true, ai_index, table, request, target_index, (*source_reference), previous_index,
                true, false, &tag, &chosen_type, &weight, &distance);
            break;
        case 9:
            chosen = function_20ec80(target_index, table, request, (*source_reference), previous_index, true, &tag, &chosen_type, &weight, &distance);
            break;
        case 10:
            chosen = function_20eeb0(table, request, target_index, (*source_reference), previous_index, &tag, &chosen_type, &weight, &distance);
            break;
        case 11:
            chosen = function_20e720(true, ai_index, table, request, target_index, (*source_reference), previous_index,
                false, true, &tag, &chosen_type, &weight, &distance);
            break;
        }
        real final_weight = distance * weight * base_weight;
        if (chosen != NONE && final_weight > 0.0f)
        {
            results[*count].object_index = chosen;
            results[*count].target_index = response_target;
            results[*count].type = chosen_type;
            results[*count].tag_index = tag;
            results[*count].weight = final_weight;
            ++*count;
        }
    }
}

bool function_1df640(short team_a, short team_b);

// @retail 0x20cbd0
void function_20cbd0(short group_index, s_audio_priority_table *table, long target_index,
    long source_index, long previous_index, long ai_index, short category,
    s_audio_selection *results, short maximum_count, short *count)
{
    (void)&table; (void)&target_index; (void)&source_index; (void)&previous_index; (void)&ai_index;
    (void)&category; (void)&results; (void)&maximum_count; (void)&count;
    long target_team = NONE;
    long source_team = NONE;
    long relation = 2;
    short source_type = NONE;
    short target_type = NONE;
    byte *data = (byte *)table;
    if (group_index >= 0 && group_index < *(long *)(data + 0x1c))
    {
        short *group = *(short **)(data + 0x20) + group_index * 2;
        if (target_index != NONE)
        {
            byte *object = *(byte **)(g_4e0300->data + (target_index & 0xffff) * 12 + 8);
            target_team = *(short *)(object + 0x138);
            long actor_index = *(long *)(object + 0x12c);
            if (actor_index != NONE)
                target_type = *(short *)(g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 4);
        }
        if (source_index != NONE)
        {
            byte *object = *(byte **)(g_4e0300->data + (source_index & 0xffff) * 12 + 8);
            if ((1 << object[0xaa]) & 3)
            {
                source_team = *(short *)(object + 0x138);
                long actor_index = *(long *)(object + 0x12c);
                if (actor_index != NONE)
                    source_type = *(short *)(g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 4);
            }
        }
        if (source_team == NONE && target_team == NONE)
            relation = 0;
        else if (source_team != NONE && source_team == target_team)
            relation = 3;
        else if (function_1df640((short)target_team, (short)source_team))
            relation = 5;
        else if (!function_1df560((short)target_team, (short)source_team))
            relation = 3;
        else if (function_1df560((short)target_team, (short)source_team))
            relation = 4;
        if (group[1] > 0)
        {
            for (short index = group[0]; index < group[1] + group[0]; ++index)
            {
                if (*count >= maximum_count)
                    break;
                function_20c440(*(byte **)(data + 0xc) + index * 0x40, table, target_index, source_index, previous_index,
                    ai_index, (short)relation, target_type, source_type, category, results, count);
            }
        }
    }
    else
        *count = 0;
}

void __stdcall function_20d570(long arg_0, short arg_1, long arg_2, void *arg_3);
void function_20fda0(long arg_0);

// @retail 0x20fec0
void function_20fec0(long arg_0, long arg_1)
{
    if (arg_1 != NONE)
    {
        byte *local_0 = *(byte **)(g_4e0300->data + (arg_0 & 0xffff) * 12 + 8);
        long local_1 = (short)function_20f040(*(short *)(local_0 + 0x138));
        s_node_owner *local_2 = (s_node_owner *)((byte *)g_4f939c + local_1 * 0x7dc);
        byte *local_3 = g_4f9398->data + (arg_1 & 0xffff) * 0x54;
        if (!local_3[0x41])
        {
            if (*(long *)(local_3 + 0x2c))
                function_20d570(NONE, NONE, *(long *)(local_3 + 4), local_3 + 0x24);
            function_20fda0(arg_1);
            function_20fe20(local_2);
        }
        else
        {
            local_3[0x48] = true;
            function_20fe20(local_2);
        }
    }
}

void function_114680(long arg_0, void const *arg_1);

struct s_20fb30
{
    short field_0;
    short field_2;
    long field_4;
    short field_8;
    short field_a;
    short field_c;
    byte field_e[2];
    long field_10;
    byte field_14[0x1c];
};

// @retail 0x20fb30
void function_20fb30(void)
{
    s_audio_priority_table *local_0 = audio_priority_table();
    long local_1 = g_510c54->game_time;
    long local_2 = 0;
    long local_3 = 2;
    do
    {
        s_node_owner *local_4 = (s_node_owner *)((byte *)g_4f939c + local_2);
        for (long local_5 = local_4->first; local_5 != NONE; )
        {
            byte *local_6 = g_4f9398->data + (local_5 & 0xffff) * 0x54;
            if (!local_6[0x48])
            {
                if (*(long *)(local_6 + 0x14) >= local_1)
                    break;
                if (!local_6[0x40])
                {
                    s_audio_priority_definition *local_7 = &local_0->entries[*(short *)(local_6 + 2)];
                    bool local_8 = false;
                    if (function_20f960(local_5, local_0, &local_8))
                    {
                        byte *local_9 = *(byte **)(g_4e0300->data + (*(long *)(local_6 + 4) & 0xffff) * 12 + 8);
                        byte *local_10 = local_9 + *(short *)(local_9 + 0x342);
                        short local_11 = *(short *)(local_6 + 0x22);
                        if (TEST_FIELD_BIT(((s_audio_object_state *)local_9)->blocked) && local_11 != 15)
                            goto local_18;
                        short local_12 = *(short *)(local_10 + 0xc);
                        if (local_12)
                        {
                            bool local_13;
                            switch ((long)local_11)
                            {
                            case 12:
                                if (local_12 >= 10 && !local_10[0x49])
                                    goto local_18;
                                local_13 = local_11 > local_12;
                                break;
                            case 13:
                            case 14:
                                local_13 = local_11 >= local_12;
                                break;
                            default:
                                local_13 = local_11 > local_12;
                                break;
                            }
                            if (!local_13)
                                goto local_18;
                        }
                        s_20fb30 local_14;
                        local_14.field_0 = *(short *)((byte *)local_7 + 0xa);
                        local_14.field_2 = *(short *)(local_6 + 2);
                        local_14.field_4 = *(long *)(local_6 + 8);
                        local_14.field_8 = *(short *)(local_6 + 0x18);
                        local_14.field_a = *(short *)(local_6 + 0x1a);
                        local_14.field_10 = local_5;
                        memcpy(local_14.field_14, local_6 + 0x24, sizeof(local_14.field_14));
                        local_14.field_c = *(short *)(local_6 + 0x20);
                        function_114680(*(long *)(local_6 + 4), &local_14);
                        local_6[0x40] = true;
                    }
                    else
                    {
                        if (local_8)
                            function_20d570(NONE, NONE, *(long *)(local_6 + 4), local_6 + 0x24);
                        function_20fda0(local_5);
                    }
                }
            }
local_18:
            local_5 = *(long *)(local_6 + 0x50);
        }
        function_20fe20(local_4);
        local_2 += 0x7dc;
    }
    while (--local_3);
}
