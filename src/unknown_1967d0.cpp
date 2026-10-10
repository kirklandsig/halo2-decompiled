// @flags /O2 /Ob1 /Gr
/* UNKNOWN_1967D0.CPP: game speed (the input recording: counter clamps, the
   snapshot of the input state into a record, and the packet codecs that
   write a record's changes into a bit stream and merge them back) */

#include "unknown_11c920.h"
#include "globals.h"
#include "bitstream.h"
#include "input_record.h"
#include <string.h>
#include <time.h>

s_flagged_value g_511020;
s_flagged_value g_511028;
s_counter_bits g_46ddc0[86];
s_input_address g_51e8d4[16];

// @retail 0x001967d0
void function_1967d0(long a, long c, long b, long delta)
{
	if (g_510ca0 && !g_510ca1)
	{
		long minimum = g_46ddc0[b].minimum;
		long maximum = g_46ddc0[b].maximum;
		if (a != NONE)
		{
			s_input_counter *counter = &g_511bf4.all[a * 0x1b5 + b];
			word raw = *(word const *)counter;
			long value = raw & 0x7fff;
			value += delta;
			if (value < minimum)
				value = minimum;
			else if (value > maximum)
				value = maximum;
			*(word *)counter = raw ^ ((raw ^ value) & 0x7fff);
		}
		if (c != NONE)
		{
			s_input_counter *counter = &g_511bf4.counters[0][c * 0x2d + b];
			word raw = *(word const *)counter;
			long value = raw & 0x7fff;
			value += delta;
			if (value < minimum)
				value = minimum;
			else if (value > maximum)
				value = maximum;
			*(word *)counter = raw ^ ((raw ^ value) & 0x7fff);
		}
	}
}

// @retail 0x001968b0
void function_1968b0(long c, long a, long b, long value)
{
	if (g_510ca0 && !g_510ca1)
	{
		long minimum = g_46ddc0[b].minimum;
		long maximum = g_46ddc0[b].maximum;
		if (a != NONE)
		{
			s_input_counter *counter = &g_511bf4.all[a * 0x1b5 + b];
			long clamped;
			if (value < minimum)
				clamped = minimum;
			else
				clamped = value > maximum ? maximum : value;
			counter->value = clamped;
		}
		if (c != NONE)
		{
			s_input_counter *counter = &g_511bf4.counters[0][c * 0x2d + b];
			long clamped;
			if (value < minimum)
				clamped = minimum;
			else
				clamped = value > maximum ? maximum : value;
			counter->value = clamped;
		}
	}
}

// @retail 0x00196960
long function_196960(long a, long b, long c)
{
	long result = NONE;
	if (a != NONE)
		result = g_511bf4.all[a * 0x1b5 + b].value;
	if (c != NONE)
		result = g_511bf4.counters[0][c * 0x2d + b].value;
	return result;
}

// @retail 0x00197360
void function_197360(s_input_record *record)
{
	memset(record, 0, sizeof(*record));
	record->flag1 = g_511020.flag;
	if (record->flag1)
		record->value4 = g_511020.value;
	record->flag8 = g_511028.flag;
	if (g_511028.flag)
		record->valuec = g_511028.value;
	record->flag0 = g_510cb1;
	memcpy(record->devices, input_device(0), sizeof(record->devices));
	memcpy(record->groups, &g_511bf4, sizeof(g_511bf4));
	memcpy(record->entries, g_511a74, sizeof(record->entries));
	memcpy(record->addresses, g_51e8d4, sizeof(record->addresses));
}

// @retail 0x00197480
void function_197480(s_bitstream *stream, s_input_counter *counters, long count, s_counter_bits *ranges)
{
	long i;
	for (i = 0; i < count; i++)
	{
		stream_write_bit(stream, counters[i].flag);
		if (counters[i].flag)
		{
			long value = counters[i].value - ranges[i].minimum;
			stream_write_checked(stream, value, ranges[i].bits);
		}
	}
}

// @retail 0x00197590
bool function_197590(s_bitstream *stream, s_input_counter *counters, long count, s_counter_bits *ranges)
{
	bool result = true;
	long i;
	for (i = 0; i < count; i++)
	{
		counters[i].flag = stream_read_bit(stream);
		if (counters[i].flag)
		{
			long value = function_1959c0(stream, ranges[i].bits) + ranges[i].minimum;
			result = result && value <= ranges[i].maximum;
			counters[i].value = value;
		}
	}
	return result;
}

/* writes an update (198540) into the bit stream */
// @retail 0x197680
void function_197680(s_bitstream *stream, void *results)
{
	s_input_update *update = (s_input_update *)results;
	long i;
	long j;

	stream_write_bit(stream, update->flag0);
	if (update->flag0)
		function_1955d0(stream, &update->value4, 32);
	stream_write_bit(stream, update->flag8);
	if (update->flag8)
		function_1955d0(stream, &update->valuec, 32);

	for (i = 0; i < 16; i++)
	{
		s_device_update *device = &update->devices[0][i];

		stream_write_bit(stream, device->changed);
		if (device->changed)
		{
			stream_write_bit(stream, device->full);
			if (device->full)
			{
				function_1955d0(stream, &device->view, sizeof(device->view) * 8);
			}
			else
			{
				stream_write_bit(stream, device->view.active);
				stream_write_checked(stream, device->view.button + 1, 5);
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_entry_update *entry = &update->entries[0][i];

		stream_write_bit(stream, entry->changed);
		if (entry->changed)
		{
			stream_write_bit(stream, entry->full);
			if (entry->full)
			{
				function_1955d0(stream, &entry->entry, sizeof(entry->entry) * 8);
			}
			else
			{
				stream_write_bit(stream, entry->entry.active);
				stream_write_checked(stream, (char)entry->entry.unknown01 + 1, 5);
				function_195720(stream, entry->entry.value & 0xffff, 16);
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counter_group_update *group = &update->groups[i];

		stream_write_bit(stream, group->flag0);
		if (group->flag0)
			function_197480(stream, group->first, 45, g_46ddc0);
		stream_write_bit(stream, group->flag1);
		if (group->flag1)
			function_197480(stream, group->second, 32, &g_46ddc0[54]);
		for (j = 0; j < 45; j++)
		{
			stream_write_bit(stream, group->entries[j].flag);
			if (group->entries[j].flag)
				function_197480(stream, group->entries[j].counters, 7, &g_46ddc0[45]);
		}
	}

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			s_pair_update *pair = &update->pairs[i][j];

			stream_write_bit(stream, pair->flag);
			if (pair->flag)
				function_197480(stream, pair->counters, 2, &g_46ddc0[52]);
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counters_update *counters = &update->counters[i];

		stream_write_bit(stream, counters->flag);
		if (counters->flag)
			function_197480(stream, counters->counters, 45, g_46ddc0);
	}

	for (i = 0; i < 16; i++)
	{
		s_address_update *address = &update->addresses[0][i];

		stream_write_bit(stream, address->flag);
		if (address->flag)
		{
			function_1955d0(stream, address->address.data, 48);
			stream_write_bit(stream, address->address.data[6]);
			stream_write_bit(stream, address->address.data[7]);
			stream_write_bit(stream, address->address.data[9]);
			stream_write_bit(stream, address->address.data[8]);
			stream_write_bit(stream, address->address.data[10]);
		}
	}
}

/* reads a 16 bit signed value */
static inline short stream_read_short(s_bitstream *stream)
{
	long value = function_1959c0(stream, 16);

	if (value & 0x8000)
		value |= 0xffff0000;
	return (short)value;
}

/* reads an update (197680) from the bit stream; false when a counter is out
   of its range or the stream ran out */
// @retail 0x197d80
byte function_197d80(s_bitstream *stream, void *results)
{
	s_input_update *update = (s_input_update *)results;
	bool success = true;
	long i;
	long j;

	memset(update, 0, sizeof(*update));
	update->flag0 = function_1957d0(stream);
	if (update->flag0)
		function_195820(stream, &update->value4, 32);
	update->flag8 = function_1957d0(stream);
	if (update->flag8)
		function_195820(stream, &update->valuec, 32);

	for (i = 0; i < 16; i++)
	{
		s_device_update *device = &update->devices[0][i];

		device->changed = stream_read_bit(stream);
		if (device->changed)
		{
			device->full = stream_read_bit(stream);
			if (device->full)
			{
				function_195820(stream, &device->view, sizeof(device->view) * 8);
			}
			else
			{
				device->view.active = stream_read_bit(stream);
				device->view.button = (char)function_1959c0(stream, 5) - 1;
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_entry_update *entry = &update->entries[0][i];

		entry->changed = stream_read_bit(stream);
		if (entry->changed)
		{
			entry->full = stream_read_bit(stream);
			if (entry->full)
			{
				function_195820(stream, &entry->entry, sizeof(entry->entry) * 8);
			}
			else
			{
				entry->entry.active = stream_read_bit(stream);
				entry->entry.unknown01 = (char)function_1959c0(stream, 5) - 1;
				entry->entry.value = stream_read_short(stream);
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counter_group_update *group = &update->groups[i];

		group->flag0 = stream_read_bit(stream);
		if (group->flag0)
			success = success && function_197590(stream, group->first, 45, g_46ddc0);
		group->flag1 = stream_read_bit(stream);
		if (group->flag1)
			success = success && function_197590(stream, group->second, 32, &g_46ddc0[54]);
		for (j = 0; j < 45; j++)
		{
			group->entries[j].flag = stream_read_bit(stream);
			if (group->entries[j].flag)
				success = success && function_197590(stream, group->entries[j].counters, 7, &g_46ddc0[45]);
		}
	}

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			s_pair_update *pair = &update->pairs[i][j];

			pair->flag = stream_read_bit(stream);
			if (pair->flag)
				success = success && function_197590(stream, pair->counters, 2, &g_46ddc0[52]);
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counters_update *counters = &update->counters[i];

		counters->flag = stream_read_bit(stream);
		if (counters->flag)
			success = success && function_197590(stream, counters->counters, 45, g_46ddc0);
	}

	for (i = 0; i < 16; i++)
	{
		s_address_update *address = &update->addresses[0][i];

		address->flag = stream_read_bit(stream);
		if (address->flag)
		{
			function_195820(stream, address->address.data, 48);
			address->address.data[6] = stream_read_bit(stream);
			address->address.data[7] = stream_read_bit(stream);
			address->address.data[9] = stream_read_bit(stream);
			address->address.data[8] = stream_read_bit(stream);
			address->address.data[10] = stream_read_bit(stream);
		}
	}

	return success && !stream_overflowed(stream);
}

// @retail 0x001984e0
bool function_1984e0(s_input_counter *current, long count, s_input_counter *out, s_input_counter *previous)
{
	bool changed = false;
	long i;
	for (i = 0; i < count; i++)
	{
		if (current[i].value != previous[i].value)
		{
			out[i].flag = 1;
			out[i].value = current[i].value;
			changed = true;
		}
		else
		{
			out[i].flag = 0;
		}
	}
	return changed;
}

/* builds the update that turns the previous record into the current one: the
   changed values, and the whole device or entry when more than its active
   flag and button (or value) changed */
// @retail 0x198540
void __stdcall function_198540(s_input_record const *previous, s_input_record const *current, s_input_update *update)
{
	long i;
	long j;

	memset(update, 0, sizeof(*update));
	update->flag0 = current->flag1;
	if (update->flag0)
		update->value4 = current->value4;
	update->flag8 = current->flag0;
	if (update->flag8)
		update->valuec = current->valuec;

	for (i = 0; i < 16; i++)
	{
		s_device_update *local_354463 = &update->devices[0][i];
		s_input_device_view const *device = &current->devices[0][i];
		s_input_device_view const *previous_device = &previous->devices[0][i];

		if (memcmp(device, previous_device, sizeof(*device)) != 0)
		{
			s_input_device_view view;

			local_354463->changed = true;
			local_354463->view.active = device->active;
			local_354463->view.button = device->button;
			view = *previous_device;
			view.active = device->active;
			view.button = device->button;
			if (memcmp(&view, device, sizeof(view)) != 0)
			{
				local_354463->view = *device;
				local_354463->full = true;
			}
		}
		else
		{
			local_354463->changed = false;
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_input_entry const *entry = &current->entries[0][i];
		s_input_entry const *local_d96427 = &previous->entries[0][i];
		s_entry_update *entry_update = &update->entries[0][i];

		if (memcmp(entry, local_d96427, sizeof(*entry)) != 0)
		{
			s_input_entry value;

			entry_update->changed = true;
			entry_update->entry.active = entry->active;
			entry_update->entry.unknown01 = entry->unknown01;
			entry_update->entry.value = entry->value;
			value = *local_d96427;
			value.active = entry->active;
			value.unknown01 = entry->unknown01;
			value.value = entry->value;
			if (memcmp(&value, entry, sizeof(value)) != 0)
			{
				entry_update->entry = *entry;
				entry_update->full = true;
			}
		}
		else
		{
			entry_update->changed = false;
		}
	}

	for (i = 0; i < 16; i++)
	{
		update->groups[i].flag0 = function_1984e0((s_input_counter *)current->groups[i], 45, update->groups[i].first, (s_input_counter *)previous->groups[i]);
		update->groups[i].flag1 = function_1984e0((s_input_counter *)&current->groups[i][45], 32, update->groups[i].second, (s_input_counter *)&previous->groups[i][45]);
		for (j = 0; j < 45; j++)
		{
			update->groups[i].entries[j].flag = function_1984e0((s_input_counter *)&current->groups[i][78 + j * 8], 7, update->groups[i].entries[j].counters, (s_input_counter *)&previous->groups[i][78 + j * 8]);
		}
	}

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			update->pairs[i][j].flag = function_1984e0((s_input_counter *)current->pairs[i][j], 2, update->pairs[i][j].counters, (s_input_counter *)previous->pairs[i][j]);
		}
	}

	for (i = 0; i < 16; i++)
	{
		update->counters[i].flag = function_1984e0((s_input_counter *)current->counters[i], 45, update->counters[i].counters, (s_input_counter *)previous->counters[i]);
	}

	for (i = 0; i < 16; i++)
	{
		s_input_address const *address = &current->addresses[0][i];
		s_address_update *address_update = &update->addresses[0][i];

		if (memcmp(address, &previous->addresses[0][i], sizeof(*address)) != 0)
		{
			address_update->address = *address;
			address_update->flag = true;
		}
		else
		{
			address_update->flag = false;
		}
	}
}

// @retail 0x001988e0
void function_1988e0(s_input_record *record, s_input_update *update)
{
	long i;
	long j;
	long k;

	record->flag1 = update->flag0;
	if (record->flag1)
		record->value4 = update->value4;
	record->flag0 = update->flag8;
	if (record->flag0)
		record->valuec = update->valuec;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			s_device_update *device = &update->devices[i][j];
			if (device->changed)
			{
				s_input_device_view *d = &record->devices[i][j];
				d->active = device->view.active;
				d->button = device->view.button;
				if (device->full)
					*d = device->view;
			}
		}
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			s_entry_update *entry = &update->entries[i][j];
			if (entry->changed)
			{
				s_input_entry *e = &record->entries[i][j];
				e->active = entry->entry.active;
				e->unknown01 = entry->entry.unknown01;
				e->value = entry->entry.value;
				if (entry->full)
					*e = entry->entry;
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counter_group_update *group = &update->groups[i];
		s_input_counter *counters = record->groups[i];
		
		if (group->flag0)
		{
			for (j = 0; j < 45; j++)
			{
				if (group->first[j].flag)
					counters[j].value = group->first[j].value;
			}
		}
		if (group->flag1)
		{
			for (j = 0; j < 32; j++)
			{
				if (group->second[j].flag)
					counters[45 + j].value = group->second[j].value;
			}
		}		for (j = 0; j < 45; j++)
		{
			if (group->entries[j].flag)
			{
				for (k = 0; k < 7; k++)
				{
					if (group->entries[j].counters[k].flag)
						counters[78 + j * 8 + k].value = group->entries[j].counters[k].value;
				}
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			s_pair_update *pair = &update->pairs[i][j];
			s_input_counter *counters = record->pairs[i][j];
			if (pair->flag)
			{
				k = 2;
				do
				{
					if (pair->counters[0].flag)
						counters[0].value = pair->counters[0].value;
					if (pair->counters[1].flag)
						counters[1].value = pair->counters[1].value;
					k--;
				}
				while (k);
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counters_update *group = &update->counters[i];
		s_input_counter *counters = record->counters[i];
		if (group->flag)
		{
			for (j = 0; j < 45; j++)
			{
				if (group->counters[j].flag)
					counters[j].value = group->counters[j].value;
			}
		}
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			s_address_update *address = &update->addresses[i][j];
			s_input_address *d = &record->addresses[i][j];
			if (address->flag)
				*d = address->address;
		}
	}
}

// @retail 0x00199010
bool __stdcall function_199010(long a, long b, const void *context)
{
	long result = input_device(a)->button - input_device(b)->button;
	if (result == 0)
		result = input_device(b)->axis - input_device(a)->axis;
	return result > 0;
}

// @retail 0x00199060
bool __stdcall function_199060(long a, long b, const void *context)
{
	long result = (char)g_511a74[a].unknown01 - (char)g_511a74[b].unknown01;
	if (result == 0)
		result = (short)g_511a74[b].value - (short)g_511a74[a].value;
	return result > 0;
}

/* the sort routines (src/unknown_13dcd0.cpp); the third parameter is never read */
typedef bool (__stdcall *t_sort_4byte_compare_function)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_sort_4byte_compare_function compare, const void *context);

/* lists the active devices and the active entries, each sorted */
// @retail 0x1990b0
void function_1990b0(long *device_count, long *devices, long *entry_count, long *entries)
{
	if (g_510cb0 && g_510cb1)
	{
		long i;

		*device_count = 0;
		for (i = 0; i < 16; i++)
		{
			if (input_device(i)->active)
				devices[(*device_count)++] = i;
		}
		sort_4byte(devices, *device_count, &entries, function_199010, 0);

		*entry_count = 0;
		for (i = 0; i < 16; i++)
		{
			if (g_511a74[i].active)
				entries[(*entry_count)++] = i;
		}
		sort_4byte(entries, *entry_count, &entries, function_199060, 0);
	}
	else
	{
		*device_count = 0;
		*entry_count = 0;
	}
}
struct s_address_table
{
	byte unknown00[0xdc24];
	struct
	{
		byte address[6];
		byte used;
		byte unknown07[4];
	} entries[16];
};

// @retail 0x001991f0
long function_1991f0(s_address_table *table, byte const *address)
{
	long index = NONE;
	dword i;
	for (i = 0; i < 16; i++)
	{
		if (!table->entries[i].used)
		{
			index = i;
			break;
		}
	}
	if (index != NONE)
	{
		memset(&table->entries[index], 0, sizeof(table->entries[index]));
		table->entries[index].used = 1;
		memcpy(table->entries[index].address, address, 6);
	}
	return index;
}

// @retail 0x00199250
long function_199250(s_address_table *table, byte const *address)
{
	long result = NONE;
	dword i;
	for (i = 0; i < 16 && result == NONE; i++)
	{
		if (table->entries[i].used && memcmp(table->entries[i].address, address, 6) == 0)
			result = i;
	}
	return result;
}


// @retail 0x1969a0
long function_1969a0(long a, long b, long c)
{
	long result = NONE;
	if (g_510cb0 && g_510cb1)
	{
		result = function_196960(a, b, c);
	}
	return result;
}

/* the results globals at 0x510cb0 hold the sixteen addresses of g_51e8d4 */
struct s_results_globals_view
{
	byte unknown0000[0xdc24];
	s_input_address addresses[16];
};

// @retail 0x199290
long function_199290(byte *results)
{
	s_results_globals_view *view = (s_results_globals_view *)results;
	long result = NONE;
	dword index;

	for (index = 0; index < 16; index++)
	{
		if (result != NONE)
		{
			break;
		}
		if (view->addresses[index].data[6] && view->addresses[index].data[8])
		{
			result = index;
		}
	}
	return result;
}

/* the results' view of the players: their machine's address slot */
struct s_results_player_slot
{
	bool active;
	byte address_index;
	byte unknown02[0xa4 - 2];
};

struct s_results_players_view
{
	byte unknown0000[0x384];
	s_results_player_slot players[16];
};

/* the machines' addresses (g_4e8c20 + 0x30) and the players' machines */
struct s_machine_addresses
{
	byte addresses[16][6];
};

struct s_machine_table_view
{
	byte unknown00[0x30];
	s_machine_addresses machines;
};

struct s_machine_player
{
	short salt;
	byte unknown02[0x1a - 2];
	short machine_index;
};

static inline s_machine_player *machine_player_try_get(long index)
{
	s_record_pool *data = g_4e8c24;

	if (index != NONE && index >= 0 && index < data->high_water_index)
	{
		s_machine_player *player = (s_machine_player *)(data->data + data->size * index);

		if (player->salt != 0)
			return player;
	}
	return 0;
}

// @retail 0x199310
void function_199310(byte *results)
{
	s_machine_addresses machines = ((s_machine_table_view *)g_4e8c20)->machines;
	s_results_players_view *view = (s_results_players_view *)results;
	s_address_table *table = (s_address_table *)results;
	dword used = 0;
	long i;
	dword j;

	for (i = 0; i < 16; i++)
	{
		s_machine_player *player = machine_player_try_get(i);

		if (player && player->machine_index != NONE)
			view->players[i].address_index = (byte)function_199250(table, machines.addresses[player->machine_index]);
		if (view->players[i].active && view->players[i].address_index != 0xff)
			used |= 1 << view->players[i].address_index;
	}

	for (j = 0; j < 16; j++)
	{
		if (!(used & (1 << j)))
			memset(&table->entries[j], 0, sizeof(table->entries[j]));
	}

	for (j = 0; j < 16; j++)
	{
		s_machine_player *player = machine_player_try_get(j);

		if (player && player->machine_index != NONE && view->players[j].address_index == 0xff)
		{
			view->players[j].address_index = (byte)function_199250(table, machines.addresses[player->machine_index]);
			if (view->players[j].address_index == 0xff)
				view->players[j].address_index = (byte)function_1991f0(table, machines.addresses[player->machine_index]);
		}
	}
}

byte g_510cb2;
long g_510de0;
long g_510de4;
extern long g_510518;
extern bool g_51051c;
extern bool g_51051d;

long __stdcall function_73b10(long a, long b);
void function_73ca0(byte *results);
void __stdcall function_b3e90(byte *results);
void function_232d77(void);

/* starts the game results: the host records its own address and the
   results go to the session's link */
// @retail 0x196390
void function_196390(void)
{
	*(volatile bool *)&g_510cb1 = true;
	if (2 == g_4e6948->state)
	{
		long index;
		index = function_199290(&g_510cb0);
		bool local = false;

		if (index != NONE)
		{
			local = function_199250((s_address_table *)&g_510cb0, g_4cf7cc) == index;
		}
		if (function_73b10(g_510de0, g_510de4) == 2)
		{
			function_73ca0(&g_510cb0);
		}
		else if (g_510518)
		{
			g_51051c = true;
			g_51051d = false;
		}
		if (local)
		{
			if (!g_510cb2)
			{
				function_b3e90(&g_510cb0);
			}
		}
	}
	function_232d77();
}

// @retail 0x195e90
void function_195e90(void)
{
	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		if (g_510ca0)
		{
			g_510ca0 = false;
		}
		if (!g_510cb1)
		{
			g_510cb2 = true;
			function_196390();
		}
	}
}

/* plays the record of one tick back into the input globals */
// @retail 0x1973f0
void function_1973f0(s_input_record *record)
{
	if (!g_510cb1)
	{
		g_511020.flag = record->flag1;
		g_511020.value = record->value4;
		g_511028.flag = record->flag8;
		g_511028.value = record->valuec;
		memcpy(input_device(0), record->devices, sizeof(record->devices));
		memcpy(&g_511bf4, record->groups, sizeof(g_511bf4));
		memcpy(g_511a74, record->entries, sizeof(record->entries));
		memcpy(g_51e8d4, record->addresses, sizeof(record->addresses));
		if (record->flag0)
		{
			function_196390();
		}
	}
}

// @retail 0x199460
void function_199460(void)
{
	long index = function_199290(&g_510cb0);

	g_511020.flag = true;
	g_511020.value = time(NULL);
	if (index != NONE)
	{
		g_51e8d4[index].data[9] = true;
	}
}

// @retail 0x1994a0
void function_1994a0(long index)
{
	long i;

	for (i = 0; i < 16; i++)
	{
		if (g_51e8d4[i].data[6])
		{
			g_51e8d4[i].data[8] = i == index;
		}
	}
}

/* retail calls function_1994a0 here; LTCG inlines it in this build */
// @retail 0x196430
void function_196430(void)
{
	long index = function_199290(&g_510cb0);

	g_511028.flag = true;
	g_511028.value = time(NULL);
	if (index == NONE)
	{
		index = function_199250((s_address_table *)&g_510cb0, g_4cf7cc);
		function_1994a0(index);
	}
}

/* a player as the input record sees it: its identifier and the settings
   the record keeps of it */
struct s_record_player
{
	short salt;
	byte unknown02[2];
	byte identifier[12];
	byte unknown10[0x44 - 0x10];
	byte settings[0x90];
};

/* a device entry as the players' identifiers and settings fill it */
struct s_record_device
{
	bool active;
	char unknown01;
	byte identifier[12];
	byte unknown0e[2];
	byte settings[0x90];
	char unknowna0;
	byte unknowna1[3];
};

/* the two counters of a pair of players */
struct s_counter_pair
{
	s_input_counter counters[2];
};

/* the samples' two player indices (unknown_196d20.cpp) */
struct s_196d20_sample
{
	dword data[9];
};

struct s_record_sample
{
	byte unknown00;
	char players[2];
	byte unknown03[0x24 - 3];
};

extern s_196d20_sample g_515c34[];

static inline s_record_player *record_player_try_get(long index)
{
	s_record_player *result = 0;

	if (index != NONE && index >= 0 && index < g_4e8c24->high_water_index)
	{
		s_record_player *player = (s_record_player *)(g_4e8c24->data + g_4e8c24->size * index);

		if (player->salt != 0)
			result = player;
	}
	return result;
}

static inline bool record_identifier_equal(void const *a, void const *b)
{
	return memcmp(a, b, 12) == 0;
}

/* the players changed: moves each player's devices, counters and samples
   from the slot of its identifier to its own index */
// @retail 0x196470
void function_196470(void)
{
	if (g_510ca0 && !g_510cb1)
	{
		long player_slots[16];
		long slot_players[16];
		s_counter_pair pairs[16][16];
		s_record_device devices[16];
		s_input_counter groups[16][0x1b5];
		s_record_device *current_devices = (s_record_device *)input_device(0);
		s_counter_pair (*current_pairs)[16] = (s_counter_pair (*)[16])g_511bf4.pairs;
		s_record_sample *sample = (s_record_sample *)g_515c34;
		s_record_device *device;
		long player_index;
		long slot;
		long i;
		long j;

		for (i = 0; i < 16; i++)
		{
			player_slots[i] = NONE;
			slot_players[i] = NONE;
		}
		for (player_index = 0; player_index < 16; player_index++)
		{
			s_record_player *player = record_player_try_get(player_index);

			if (player)
			{
				for (slot = 0; slot < 16; slot++)
				{
					if (input_device(slot)->active && record_identifier_equal(player->identifier, input_device(slot)->unknown02))
					{
						if (slot_players[slot] == NONE)
						{
							slot_players[slot] = player_index;
							player_slots[player_index] = slot;
						}
						break;
					}
				}
			}
		}

		memcpy(devices, current_devices, sizeof(devices));
		player_index = 0;
		do
		{
			s_record_player *player = record_player_try_get(player_index);

			if (player)
			{
				device = &current_devices[player_index];

				if (player_slots[player_index] == NONE)
				{
					device->active = true;
					memcpy(device->identifier, player->identifier, sizeof(device->identifier));
					memcpy(device->settings, player->settings, sizeof(device->settings));
					device->unknowna0 = NONE;
					device->unknown01 = NONE;
				}
				else
				{
					*device = devices[player_slots[player_index]];
				}
			}
			player_index++;
		}
		while (player_index < 16);

		memcpy(groups, g_511bf4.groups, sizeof(groups));
		for (player_index = 0; player_index < 16; player_index++)
		{
			s_input_counter *group = g_511bf4.groups[player_index];

			if (player_slots[player_index] == NONE)
				memset(group, 0, sizeof(g_511bf4.groups[player_index]));
			else
				memcpy(group, groups[player_slots[player_index]], sizeof(g_511bf4.groups[player_index]));
		}

		memcpy(pairs, current_pairs, sizeof(pairs));
		for (i = 0; i < 16; i++)
		{
			long first_slot = player_slots[i];

			for (j = 0; j < 16; j++)
			{
				if (player_slots[j] != NONE && first_slot != NONE)
					current_pairs[i][j] = pairs[first_slot][player_slots[j]];
				else
					*(dword *)&current_pairs[i][j] = 0;
			}
		}

		for (i = 0; i < 1000; i++, sample++)
		{
			char *player = sample->players;

			for (j = 0; j < 2; j++, player++)
			{
				long index = *player;

				if (index != NONE && index >= 0 && index < 16)
					*player = (char)slot_players[index];
				else
					*player = NONE;
			}
		}
		function_199310(&g_510cb0);
	}
}

/* the players (0x21c bytes each): the unit and the dead unit */
struct s_results_player
{
	byte unknown00[0x2c];
	long unit_index;
	long dead_unit_index;
	byte unknown34[0x21c - 0x34];
};

point3f *function_b9dd0(long object_index, point3f *result);

inline long results_player_unit(s_results_player const *player)
{
	long result = NONE;
	if (player->unit_index != NONE)
		result = player->unit_index;
	else if (player->dead_unit_index != NONE)
		result = player->dead_unit_index;
	return result;
}



/* where a player's unit, or its dead unit, is. Retail passes position on the
   stack: reading it through its address keeps it there */
// @retail 0x1994d0
bool function_1994d0(long player_index, point3f *position)
{
	bool result = false;
	s_results_player *player = (s_results_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_results_player));
	long unit_index = results_player_unit(player);
	point3f *const *position_reference = &position;

	if (unit_index != NONE)
	{
		function_b9dd0(unit_index, *position_reference);
		result = true;
	}

	return result;
}
bool game_engine_team_is_active(long team);

/* marks every active team as in the game */
// @retail 0x196780
void function_196780(void)
{
	if (g_510ca0 && !g_510cb1)
	{
		long team;

		for (team = 0; team < 16; team++)
		{
			if (game_engine_team_is_active(team) && !g_511a74[team].active)
			{
				g_511a74[team].active = true;
			}
		}
	}
}

// The no-argument retail entry needs a single shared results object.
// The current split globals do not hold its sixteen player records.
// This core body remains disabled until that storage has one valid binding.
#if 0
#include "unknown_157450.h"

struct s_results_record_195ed0
{
	byte active;
	byte address_index;
	byte unknown02[0xe];
	byte appearance[0x90];
	char rank;
	byte unknowna1[3];
};

struct s_results_update_storage
{
	byte unknown000[0x12b];
	byte teams;
	byte unknown12c[0x370 - 0x12c];
	s_flagged_value started;
	s_flagged_value finished;
	byte unknown380[4];
	s_results_record_195ed0 players[16];
	s_input_entry entries[16];
	s_input_counters counters;
	byte unknown4f84[0xdc24 - 0x4f84];
	s_input_address addresses[16];
};

struct s_results_ranking_195ed0
{
	long players[16];
	short teams[8];
	char player_ranks[16];
	char team_ranks[8];
	short player_count;
	short team_count;
};

void function_23f3e0(s_results_ranking_195ed0 *ranking, long mode, bool fallback);
long function_1587f0(long team);
void function_199460();
struct s_network_observer;
struct s_machine_address;
struct s_session_machine_address;
struct s_simulation_world_owner;
long simulation_watcher_find_machine(s_simulation_world_owner const *watcher, s_machine_address const *address);
long network_observer_find_channel_by_machine(s_network_observer *observer, s_session_machine_address const *address, long owner);

// Core of retail 0x195ed0; the storage parameter is only an analysis binding.
void refresh_results_195ed0(s_results_update_storage *results)
{
	if (g_510ca0 && !g_510cb1)
	{
		if (!g_511020.flag)
			function_199460();
		for (long team = 0; team < 16; ++team)
		{
			if (game_engine_team_is_active(team))
			{
				if (!results->entries[team].active)
					results->entries[team].active = true;
				if (!results->teams)
					results->teams = true;
			}
		}
		for (long index = 0; index < 16; ++index)
		{
			s_machine_player *player = machine_player_try_get(index);
			if (player)
			{
				s_results_record_195ed0 *record = &results->players[index];
				memcpy(record->appearance, (byte *)player + 0x44, sizeof(record->appearance));
				record->address_index = 0xff;
				long team = (signed char)record->appearance[0x7c];
				if (results->teams && team != NONE && !results->entries[team].active)
				{
					results->entries[team].active = true;
					results->entries[team].value = 0;
					results->entries[team].unknown01 = 0xff;
				}
				word *flags = (word *)results->counters.groups[index];
				*flags = (*flags & 0x8001) | 1;
			}
		}
		if ((bool)((function_xaee93d()->flags >> 5) & 1))
		{
			s_results_ranking_195ed0 ranking;
			function_23f3e0(&ranking, 1, false);
			for (long rank = 0; rank < ranking.team_count; ++rank)
			{
				long team = ranking.teams[rank];
				s_input_entry *entry = &results->entries[team];
				entry->unknown01 = ranking.team_ranks[rank] / 2;
				entry->value = (short)(function_1587f0(team) < 0 ? 0 :
					function_1587f0(team) > 0x7fff ? 0x7fff : function_1587f0(team));
			}
			for (long rank = 0; rank < ranking.player_count; ++rank)
			{
				long player = ranking.players[rank] & 0xffff;
				results->players[player].rank = ranking.player_ranks[rank] / 2;
			}
		}
		function_199310((byte *)results);
		for (long index = 0; index < 16; ++index)
		{
			s_input_address *address = &results->addresses[index];
			if (address->data[6])
			{
				if (!memcmp(address->data, g_4cf7cc, 6))
				{
					address->data[8] = true;
					address->data[7] = true;
				}
				else
				{
					address->data[8] = false;
					s_network_observer *observer = *(s_network_observer **)((byte *)g_4cf780 + 8);
					bool connected = false;
					if (observer && simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
						(s_machine_address const *)address->data) != NONE)
					{
						long channel = network_observer_find_channel_by_machine(observer,
							(s_session_machine_address const *)address->data, 3);
						if (channel != NONE && *(long *)((byte *)observer + channel * 0x528 + 0xa8) == 7)
							connected = true;
					}
					address->data[7] = connected;
				}
			}
		}
	}
}
#endif
