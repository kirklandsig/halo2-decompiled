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

s_counter_bits g_46ddc0[86];
s_input_address g_51e8d4[16];

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

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
			counter->value = PIN(counter->value + delta, minimum, maximum);
		}
		if (c != NONE)
		{
			s_input_counter *counter = &g_511bf4.counters[0][c * 0x2d + b];
			counter->value = PIN(counter->value + delta, minimum, maximum);
		}
	}
}

// @retail 0x001968b0
void function_1968b0(long a, long c, long b, long value)
{
	if (g_510ca0 && !g_510ca1)
	{
		long minimum = g_46ddc0[b].minimum;
		long maximum = g_46ddc0[b].maximum;
		if (a != NONE)
		{
			s_input_counter *counter = &g_511bf4.all[a * 0x1b5 + b];
			counter->value = value < minimum ? minimum : (value > maximum ? maximum : value);
		}
		if (c != NONE)
		{
			s_input_counter *counter = &g_511bf4.counters[0][c * 0x2d + b];
			counter->value = value < minimum ? minimum : (value > maximum ? maximum : value);
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
	byte flag8 = g_511028.flag;
	record->flag8 = flag8;
	if (flag8)
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
bool function_1984e0(s_input_counter *current, long count, s_input_counter *previous, s_input_counter *out)
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
		update->groups[i].flag0 = function_1984e0((s_input_counter *)current->groups[i], 45, (s_input_counter *)previous->groups[i], update->groups[i].first);
		update->groups[i].flag1 = function_1984e0((s_input_counter *)&current->groups[i][45], 32, (s_input_counter *)&previous->groups[i][45], update->groups[i].second);
		for (j = 0; j < 45; j++)
		{
			update->groups[i].entries[j].flag = function_1984e0((s_input_counter *)&current->groups[i][78 + j * 8], 7, (s_input_counter *)&previous->groups[i][78 + j * 8], update->groups[i].entries[j].counters);
		}
	}

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			update->pairs[i][j].flag = function_1984e0((s_input_counter *)current->pairs[i][j], 2, (s_input_counter *)previous->pairs[i][j], update->pairs[i][j].counters);
		}
	}

	for (i = 0; i < 16; i++)
	{
		update->counters[i].flag = function_1984e0((s_input_counter *)current->counters[i], 45, (s_input_counter *)previous->counters[i], update->counters[i].counters);
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

/* copies the counters an update flags into the record's counters */
static inline void counters_apply_1988e0(s_input_counter *counters, long count, s_input_counter const *update)
{
	long i;

	for (i = 0; i < count; i++)
	{
		if (update[i].flag)
			counters[i].value = update[i].value;
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

		if (group->flag0)
			counters_apply_1988e0(record->groups[i], 45, group->first);
		if (group->flag1)
			counters_apply_1988e0(&record->groups[i][45], 32, group->second);
		for (j = 0; j < 45; j++)
		{
			if (group->entries[j].flag)
				counters_apply_1988e0(&record->groups[i][78 + j * 8], 7, group->entries[j].counters);
		}
	}

	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			s_pair_update *pair = &update->pairs[i][j];

			if (pair->flag)
			{
				for (k = 0; k < 2; k++)
					counters_apply_1988e0(record->pairs[i][j], 2, pair->counters);
			}
		}
	}

	for (i = 0; i < 16; i++)
	{
		s_counters_update *group = &update->counters[i];

		if (group->flag)
			counters_apply_1988e0(record->counters[i], 45, group->counters);
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
void sort_4byte(void *unused, long *elements, unsigned long count, t_sort_4byte_compare_function compare, const void *context);

/* lists the active devices and the active entries, each sorted */
// @retail 0x1990b0
void function_1990b0(long *devices, long *device_count, long *entry_count, long *entries)
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
		{
			long temporary;

			sort_4byte(&temporary, devices, *device_count, function_199010, 0);
		}

		*entry_count = 0;
		for (i = 0; i < 16; i++)
		{
			if (g_511a74[i].active)
				entries[(*entry_count)++] = i;
		}
		{
			long temporary;

			sort_4byte(&temporary, entries, *entry_count, function_199060, 0);
		}
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
long function_1969a0(long a, long c, long b)
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

#include "unknown_157450.h"

/* the results' flag: the game has teams (0x510ddb, inside the results at 0x510cb0) */
byte g_510ddb;

struct s_score_display;
void function_23f3e0(s_score_display *display, long mode, bool fallback);
long function_1587f0(long team);
void function_199460(void);
struct s_network_observer;
struct s_simulation_world_owner;
struct s_session_machine_address;
struct s_machine_address;
long simulation_watcher_find_machine(s_simulation_world_owner const *watcher, s_machine_address const *address);
long network_observer_find_channel_by_machine(s_network_observer *observer, long owner, s_session_machine_address const *address);

/* the players and teams in the order of their ranks (0x6c bytes, filled by 0x23f3e0) */
struct s_results_ranking_195ed0
{
	long players[16];
	short teams[8];
	char player_ranks[16];
	char team_ranks[8];
	short player_count;
	short team_count;
};

/* a player's record in the game results (0xa4 bytes, sixteen from 0x511034) */
struct s_results_player_195ed0
{
	bool active;
	byte address_index;
	byte unknown02[0xe];
	struct
	{
		byte unknown00[0x7c];
		char team_index;
		byte unknown7d[0x13];
	} configuration;
	char rank;
	byte unknowna1[3];
};

/* inline copies of game_engine_team_is_active (0x161e60) and of the team
   score (0x1587f0), which retail inlines here */
static inline bool team_is_active_195ed0(long team)
{
	bool result = false;

	if (function_x340af0() && team >= 0 && team < 8)
	{
		result = (function_xaee93d()->team_mask & (1 << team)) != 0;
	}
	return result;
}

static inline long team_score_195ed0(long team)
{
	long result = NONE;

	if (game_engine_statborg_inline() && function_x340af0() && team_is_active_195ed0(team))
	{
		result = game_engine_statborg_inline()->teams[team].score;
	}
	return result;
}

/* updates the game results: the active teams, the players' configurations,
   the ranks and scores, and which machines are still connected */
// score 100 (was 100): the teams flag is read before the configuration copy and address-index
// store, and the third team-score read expands where retail calls 0x1587f0.
// Earlier score 191.
// store, and the third team-score read expands where retail calls 0x1587f0. Earlier score 191.
// Shared properties type 191, typed record copy 191, conditional team index 191, nullable team
// entry 314, combined team guard 333, team read before flag 309, record reference 191,
// native counter field 191, and branch-local score stores 192.
// The flag read precedes the configuration copy, and the final score query expands. Tried:
// existing score clamp 192; separate clamp stores 147; inline score clamp 100; existing score on lower/upper
// test 223/216; statistics pointer retained 191; shared active-team query 138; reversed clamp tests 210. NOT
// kept: 100 and 138 differ at the score or active-team call boundary; 147 grows farther from retail.
// Configuration memmove 720; existing input-device view with memcpy/memmove 191/720;
// native ranking record 191; ranked-team/team-rank/player/player-rank pointer walks 223/243/209/195.
// The flag read still leads the copy in the tied forms, and the final score query still expands.
// P608: Empty-slot continue, while-tail/lookup/skip, do-while-tail/latch/skip and terminal
// inequality: 191/320/886/886/902/902/902/716. NOT kept: tied flag schedule or lost unrolling.
// Round 416: Shared absolute-slot lookup with a machine-player or byte view,
// direct or retained pool, and a retained destination each score 191. The same
// four forms with the destination getter at each access score 732. NOT kept:
// the getter forms roll the player loop and the flag read stays ahead of the copy.
// M3 lane 16 r135 (fast, plain confirmed): the clamp as the file's own PIN over team_score_195ed0 191 -> 100
// (1211 B, aligned 74 -> 67): the third expansion now runs out of inline budget at its team test, which then calls
// 0x161e60, and the per-rank g_4e9ae8 reload is retail's; retail instead calls 0x1587f0 for the whole third read.
// The flag order needs the results block at 0x510cb0 as one global (flag +0x12b, player records +0x384, address
// table +0xdc24): a compile-only screen with the flag and the records as fields of one global puts the flag load
// after the copy and the 0xff store, as retail; split globals let VC schedule it first (an escaped flag address
// does not). Also tried (fast, old base 191): the clamp as if/else statements into a long 191 (same code); the
// score helper's statistics read through 0x162030 223 (1257 B); the third call given `ranking.teams[rank]` 195; the
// rank loop's team as `short` 240 (1269 B); re-measured: PIN over 0x1587f0 192 (each read inlined, 0x161e60 and
// 0x162030 called), the shared active-team query 138 (aligned 103). Diagnostic: the file with /arch:SSE 191 (no
// change: our link inlines 0x1587f0 here and nowhere else, 0x64d70 and 0x19e1c0 call it).
// On 100: a named statistics pointer in the score helper 100 (same code); the rank counter `short` 222 (1255 B);
// the rank declared in each `for` 100 (same code); the entry fields written through the table (no entry pointer)
// 105; the team helper `__forceinline` 322 (1051 B).
// The team-mode load still precedes the player copy, and the final score call still expands.
// Statborg across both tests 107; shared mode 214; shared mask 149; statborg/mode 202; engine
// receiver 202; statborg/mask 160; statborg/mode/mask 178; shared eligibility 165; engine receiver and
// mask with both mode guards 210/215. NOT kept: the 107 form grows the frame to 0x78. The mask
// and shared-eligibility gains omit retail guards, and the final score call still expands.
// M3 lane 10 r120 (fast; control 100): the flag order was tested with the globals joined. One object for the whole
// range (0x510ca0 to 0x51e984: the kept bytes, the record's head, the players' entries, the teams' entries, the
// counters, the samples and the addresses as fields, the old names as macros) 76 here (each unrolled pass reads the
// teams flag after the copy and the 0xff store, as retail), but matched 0x196470 goes to 158: its second player loop
// then tests the NONE index on the entry pointer (`cmp eax, <entries - 0xa4>`), where retail keeps a separate
// index * 0xa4 counter in a stack slot for that test, which our compiler does only while that address lies below the
// start of the entries' own global. So retail's players' entries are not in an object that starts at or below
// 0x510fa0, and the flag (0x510ddb) cannot share their object: not kept. The head alone as one object (0x510cb0 to
// 0x511000, its address already taken here) 100 (same code; 0x196470 and the 44 other users stay matched). On that
// head object: the record pointer formed from the head's address (`&g_510cb0 + 0x384 + index * 0xa4`, diagnostic) 333
// (aligned 58: the flag after the copy as retail, but the first loop then keeps the flag in BL); the record pointer
// through a `long` cast of input_device(0) 100 (same code). Diagnostic on main's globals: the flag read as
// `*((byte *)&g_511000 - 0x225)` 373 (the flag after the copy, the address not folded). Third score read: the two
// clamp tests through the file's inline copy and the value through the existing 0x1587f0 191 (1237 B: the link
// inlines 0x1587f0 there and calls 0x161e60 and 0x162030 from it). build/r120/storage-lead holds the head-only header.
// M3 lane 5 r203 (fast with 0x196470, 0x1587f0, 0x161e60 and 0x162030, matched throughout; control 100): the first loop's
// team test through the file's inline copy 442 (1301 B: it expands there, so the inline budget is not spent before the
// rank loop); the inline team test as one `return a && b && c && d` 151 (1210 B; the second clamp read then drops its team
// test); the score helper with an early `return NONE` 170 (1225 B); the score helper `__forceinline` 100; the clamp as an
// if / else if / else chain into a long 100 (the 100s the same code). Reading: the one extra byte is the rank loop's entry
// test, where retail compares the team count's word in memory with BP (`cmp word ptr [esp + 0x82], bp`) and ours
// sign-extends it first; retail's second clamp read repeats the range and mask test (only the mask load is shared), ours
// reuses the first read's bool in BL.
// One sampled score 225; lower sample reused 190; upper sample reused 175; local bound tests
// with native final score and separate stores 192, or admitted-arm selection 191; paired team/player
// rank cursors 186/165 with indices or 186/174 with remaining counts. The final query still expands
// on the native-call forms, and the post-copy flag order remains.
// Short score through explicit bounds 191, named lower test 191, nested score validation 100,
// early unavailable score return 170, static score query 322, eligibility from the mask tests 151,
// one receiver for the score table 204, signed rank bound retained at entry 100. The teams flag
// still precedes the copy, and the in-range score call remains expanded.
// Unsigned player/team ordinals 254/153, direct guarded team-table stores 246,
// installed-team and active-team references 100/100, count-first rank comparison 100,
// admitted clamp with a shared result 177, unsigned terminal player inequality 732.
// The flag still leads the configuration copy and the final score call still expands.
// Clamp failures with rank continues and native or inline final score 192/128.
// Team ordinal read at each use 216; signed count snapshots 100; rank before the
// ranking query 104; team/rank cursors 337; signed rank sample at the store or
// before the entry lookup 100/100. The early flag read and expanded final score remain.
// M3 lane 4 r219 (fast with 0x196470, 0x1587f0, 0x161e60 and 0x162030, all MATCH each time; control 100): the score
// helper reading its result through a private inline copy of the statistics getter, as 0x1587f0's own source does, 100;
// the score helper `__forceinline` 100 (the third read still calls 0x161e60 from inside it); the team test with an early
// `return false` 100; the score helper as three nested ifs 100; the teams flag declared `bool` 100 (all the same code);
// re-measured the early-return score helper 170. Reading: retail's second expansion repeats the range and mask tests
// (`shl eax, cl; test edx, eax; setne al`) where ours reuses the first expansion's setne result in BL, and the inline
// budget runs out one level deeper in ours (the team test) than in retail (the whole third read).
// Indexed configuration records 181, indexed configuration and player ranks 156,
// combined copied-team admission 301, rank division before team entry 100,
// rank and score through indexed team 105, short team rank ordinal 205,
// named player rank and slot 100, native in-range score with short selection 191.
// NOT kept: the early flag load remains and the native score call still inlines.
// M3 lane 11 r125 (fast with 0x196470 and 0x199310, both MATCH throughout; control 100): the records through the
// results view of g_510cb0 (players at +0x384, as 0x199310 reads them) 100 (same code: the flag is another symbol); the
// flag and the records through that view 333 (aligned 58: each pass reads the flag after the copy and the 0xff store as
// retail, but the first loop then keeps the flag in BL across the 0x161e60 calls); the same through the cast at each use
// 333 (same code); the flag, the records and the team entries (+0xdc4) through the view 328 (aligned 59: the first loop
// reloads the flag as retail, but counts down from 16 in EBX because the entries' end lies past the one-byte g_510cb0);
// the score helper reading the score through the engine getter's statborg 116 (frame 0x78); the score helper taking
// `long const &team` 296 (1077 B). Diagnostic (not keepable, an unmapped name): the flag, the records and the team
// entries as fields of one 0x1044-byte object of the view type 73 (aligned 25): everything up to the rank loop body is
// then retail's, the rank loop's `cmp word ptr [esp + 0x82], bp` included. So retail addresses the flag, the records and
// the team entries off one symbol that runs past 0x511bf4, while matched 0x196470 needs the records' symbol to start
// above 0x510fa0 (its NONE test keeps an index * 0xa4 counter); only the rank loop's inline budget is then left.
// Diagnostic on that object: 0x196470's second player loop indexing its player array as a real array member (not the
// cast entry pointer) 158 (751 B, the NONE test on the pointer again): the compiler forms `players - 0xa4` whenever it
// lies inside the symbol, array member or not, so one object for both functions does not reproduce retail.
// M3 lane 16 r147 (fast with 0x196470, 0x1587f0, 0x161e60 and 0x162030, all MATCH; control 100): the clamp over the real
// 0x1587f0 re-measured 192 (1241 B: all three reads inlined, their nested calls kept); the score helper's team test
// through the real 0x161e60 180 (1225 B); the team helper plain `static` 180 (1225 B, the same code: its copy folds into
// 0x161e60); the score helper plain `static` 322 (1051 B: no read inlined, each calls the file's copy); a `0 <
// ranking.team_count` guard around the rank loop 161 (1210 B; the count tested as a word, but twice).
// Public score query at the lower bound, upper bound, both bounds, lower bound with
// selected value, or upper bound with selected value gives 217/203/135/223/216.
// Short return, short result local, or both gives 101/147/147.
// NOT kept: the provider substitutions grow the body. Narrow result storage reaches
// 1210 bytes but raises the score, and the teams flag still precedes the copy.
// Counter bound beside the record or after publishing its address index 100/100,
// native counter value with low-bit mask 100, bool team-mode sample 100,
// full-width lower default 177, nested inclusive score selection 165,
// inclusive upper selection with local or native interior query 100/191.
// The tied forms still load the teams flag before the copy, and the native
// interior query still inlines.
// Machine-player configuration byte span 100, typed source member 100, typed assignment or memcpy
// into the results record 100/100, returned typed copy destination 204, and shared device-view
// copies from the byte or typed member or through assignment 100/100/100. NOT kept: the
// copy and address-index write still follow the teams-flag read in the tied forms.
// m3 lane 4 r248 (fast with 0x196470, 0x1587f0, 0x161e60 and 0x162030, all MATCH): the counter store written as the
// counter's own 15-bit field (`.value = 1`, as the file's other counter writers do) for the hand mask through a `word *`
// cast 100, the same code (`and ecx, 0x8001; or ecx, 1`): kept; with a named counter pointer 100. Both helpers with
// external `inline` linkage, alone or together, 100; the clamp's `(short)` cast dropped 100 (all the same code); the team
// test's bit folded into its condition with `result = true;` 151 (1210 B). Diagnostic (behaviour change, reverted): an
// extra guarded 0x199460 call with a team test ahead of the team loop 381 (1253 B), and the third score read still
// expands with its team test called, so the third read's expansion does not follow the inline work spent before it.
// M3 lane 2 r138 (h2m fast with 0x1587f0, 0x159460, 0x161e60, 0x162030, 0xd9640, 0x15b3a0, 0x15c550, 0x15ca10, 0x15ce70,
// 0x162060, 0x196780, 0x64d70): this file and the file of the real team test and statistics getter are both /Ob1, so
// neither can expand here; with PIN over the real 0x1587f0 (re-measured 192 at 1241 B): `inline` on 0x161e60's definition
// 489 (1338 B; 0x159460 lost), on 0x162030's 180 (1225 B; 0x1587f0, 0x159460, 0x15b3a0, 0x15c550, 0x162060 lost), on
// both 442 (1301 B); `inline` on 0x1587f0's own definition 192 and a forced-inline declaration of it here 192 (both the
// same code as the plain call). Body unchanged.
// M3 lane 2 r140 (h2m fast with 0x196470, 0x1587f0, 0x161e60, 0x162030, all MATCH; control 100): the team loop's counter
// `short` in its own `for` with the player loop's `long` 205 (1242 B); the reverse 175 (1223 B); both `short` 222 (1255
// B); the clamp as an if / else-if chain over one sampled score, the value read again, 220 (1257 B); the first team loop's
// entry named once for the test and the store 100 (same code). Diagnostic (width cast, not keepable): `(short)rank <
// ranking.team_count` 149 (1210 B: the entry test is retail's `cmp word ptr [esp + 0x82], bp`, but the latch compares
// words too where retail sign-extends). Retail's own 0x1587f0 calls 0x161e60 and 0x162030, so its two full expansions
// here and the called third read fit one score call expanded by site frequency. Body unchanged.
// m3 lane 4 r251 (H2M_FAST with 0x196470, 0x1587f0, 0x161e60 and 0x162030, all MATCH; control 100): the rank loop's test
// written count first 100 (same code); the score store ahead of the rank store 109; a `short` counter for the team-rank
// loop alone 205 (1242 B, a pointer walk); the counter initialised at its declaration 104 (its zero then serves the
// ranking call's `push 0`, the guard still sign-extends where retail compares the word with the zeroed counter); the
// ranking aggregate in the outer block 100 (same code).
// M3 lane 11 r159 (h2m fast with 0x196470, 0x1587f0, 0x161e60 and 0x162030, all MATCH; control 100): the team-rank
// loop's bound as a named `short` or `long` local for its test 100/100 (same code: the copy folds, the entry test still
// sign-extends where retail compares the word with BP); that loop as a `while` with the increment last 163 (the counter
// moves to EDI, the entry test unchanged); as `if (0 < count) do ... while (++rank < count)` 156 (1210 B: the entry test
// becomes a word test of AX but the count is spilled for the body); the team test reading the team mask into a named
// local 100 (same code); re-measured the team read at each use (no `team` local) 216. Body unchanged.
// M3 lane 2 r144 (h2m fast with 0x196470, 0x1587f0, 0x161e60 and 0x162030, all MATCH unless noted; control 100, aligned
// 58). Mixed views, the first loop left on g_510ddb and g_511a74: in the player loop the flag and the record through a
// view of the results record at g_510cb0 (flag +0x12b, players +0x384) 333 (aligned 55: all four unrolled passes read
// the flag after the copy and the 0xff store as retail, but the first loop then holds the flag in BL, loaded ahead of
// the loop and stored after it); the flag alone through the view 333 (aligned 63, the same first loop); the record
// alone through the view 100 (same code: the copy does not reach g_510ddb); flag, record and that loop's team entry
// through the view 333 (aligned 55). So the first loop reloads the flag only while the player loop reads the same
// symbol. Diagnostic, reverted (an undefined stand-in object of 0x510cb0's record with the flag, the players and the
// team entries as fields, used by this function and by 0x196470 in place of g_510ddb, input_device() and g_511a74):
// 76 here, 0x196470 158 (751 B), as lane 10 r120 measured; under it 0x196470 differs from retail only in the player
// loop's NONE test (`cmp eax, players - 0xa4` for retail's index * 0xa4 counter in a stack cell), and its device
// pointer formed by byte arithmetic from the record's address, through a file-local inline taking the record pointer,
// through a `dword` cast of the record's address, or per pass from the players member 158 each (same code), through a
// file-local const pointer 133 (760 B, the pointer is loaded). No form of 0x196470 found keeps its counter once the
// players lie more than 0xa4 bytes into their symbol, so the one-object record stays a lead. Diagnostic sweep of the
// local count (1 to 8 unused initialised longs at the top) 100 each, the same code.
// Early results return 100; address cursor 155; unused-address continue 100; local-address continue 100;
// shared world receiver 100; connectivity from the channel predicate 165; selected channel 141;
// shared connectivity store 131. NOT kept: the flag still precedes the copy, and the shared store
// joins branches that have distinct stores in retail.
// Separate rank ordinals 100; team count difference or both count differences 207/225;
// typed rank receivers 100; short team with direct rank stores 229; nonnegative team count 188;
// inactive-team continue 100; copied member and typed team receiver 301. The teams byte still
// precedes the copy in the tied forms, and the selected score query still expands.
// Copied team held as char, signed char, short or int 299/299/331/100;
// widened after admission from char or short 299/331; field at both uses 301;
// char with named presence 299. NOT kept: the copy still follows the flag read.
// Combined sentinel and inactive-team decision with char, short or long
// 274/295/329. NOT kept: all grow the player loop and retain the early flag read.
// M3 lane 4 r260 (h2m fast with 0x196470, 0x1587f0, 0x161e60, 0x162030 and 0x199310, all MATCH unless noted; control 100,
// aligned 45; body unchanged). Retail's rank loop, read row by row: the first score read expands 0x1587f0 with the team
// test and the statistics getter inside it; the second expands it again and repeats the team's range and mask tests
// (only the engine pointer, the running flag and the mask word are reused); the third is `call 0x1587f0`; the entry test
// is `cmp word ptr [count], bp` with the zeroed counter. Diagnostic (a build setting, reverted): the callee file of
// 0x161e60 and 0x162030 without /Ob1 and the clamp over the real 0x1587f0 442 (1301 B), and 0x1587f0 37, 0x159460 198,
// 0x15b3a0 143, 0x15c550 155, 0x64d70 220: the link then expands the two everywhere, so retail's are not plain
// expandable functions. The third read follows the site's estimated frequency: with one more enclosing test around
// the rank loop it becomes retail's `call 0x1587f0` (the clamp's tests through the file's copies and its value through
// the real function, in `if (0 < ranking.team_count)`: 173 at 1197 B on main, 149 on lane 12's results view; the
// counter zeroed ahead of the guard 150; the guard as `rank < count` 144, aligned 39; the clamp over the real function
// in the guard 177 / 154; the file's copies in the guard 161 / 148), but the second read's team test then becomes a
// call as well, where retail expands it. Without the guard both expand (the mixed clamp 191 at 1237 B, 167 on the
// view). A guard with a do-while inside (`if (0 < count) do ... while (++rank < count)`) keeps the plain loop's level:
// the third read expands again (160, and 132 at 1210 B with the file's copies). Diagnostic guards that halve less
// (`a || b`, `!(a && b && c)` of folding tests) leave the third read expanded (184, 194). So retail's sites need the
// third read below the real function's level while the second read's nested team test stays above its own, a window
// between the plain loop and one full extra test that none of these forms hits. Lane 12's results view re-measured 76
// (aligned 37).
// Separate score-table and mode admission 163, admitted score returned directly 170,
// native short score field 100, native mask and bit locals 100. Nested connectivity
// guards 165, initial machine-admission result 141, shared local-address marker 136,
// connectivity assigned by guard arm 141. The teams flag still leads the player copy.
// Read the source team inside mode guard, after address publication, after copy or before copy:
// 295/297/297/150 with either memcpy or typed assignment. NOT kept: all regress.
// The teams flag still leads the copy and the final score query still expands.
// M3 lane GB round 5 (fast with 0x196470, 0x1587f0, 0x161e60, 0x162030 and 0x199310 all MATCH; control 100, al=71, mask=49):
// nested outer guards 100, with native value arm 191; nested if inside team_is_active_195ed0 100, 3-level nested if 151 (1210 B);
// team range before function_x340af0 102; function_x340af0 before statborg 169 (1221 B); inner-flipped <= 0x7fff PIN
// inline/mixed/native 100/191/192; if/else-if/else stores with native else 192; nested >=0/<=0x7fff with native innermost 211;
// statborg local with native value arm 191; team_count != 0 guard inline/mixed 161 (1210 B)/173 (1197 B). Diagnostics (reverted):
// dropping /Ob1 on this TU (native/current) 192/100 (same code), #pragma inline_depth(1) 100. Body unchanged.
// Configuration receiver from the existing entry's active member 100, word-sized team rank
// with/without signed count sample 160/160, one-based comparison 187. Inclusive negative/upper/
// both score boundaries 101/100/101, both with the native query on admission 191.
// NOT kept: the ties leave the flag read early and the other forms grow the body or raise the score.
// Signed-byte team/player/both rank ordinals 181/165/196, with matching signed
// count snapshots 181/165/196. Separate unsigned bytes 198, shared signed byte 196.
// NOT kept: narrow team ordinals compare to an immediate and add a byte spill
// where retail uses BP. The early teams load and final score call still differ.
// @retail 0x195ed0
void function_195ed0(void)
{
	if (g_510ca0 && !g_510cb1)
	{
		if (!g_511020.flag)
		{
			function_199460();
		}
		for (long team = 0; team < 16; team++)
		{
			if (game_engine_team_is_active(team))
			{
				if (!g_511a74[team].active)
				{
					g_511a74[team].active = true;
				}
				if (!g_510ddb)
				{
					g_510ddb = true;
				}
			}
		}
		for (long index = 0; index < 16; index++)
		{
			s_machine_player *player = machine_player_try_get(index);

			if (player)
			{
				s_results_player_195ed0 *record = (s_results_player_195ed0 *)input_device(index);

				memcpy(&record->configuration, (byte *)player + 0x44, sizeof(record->configuration));
				record->address_index = 0xff;
				if (g_510ddb)
				{
					long team = record->configuration.team_index;

					if (team != NONE)
					{
						s_input_entry *entry = &g_511a74[team];

						if (!entry->active)
						{
							entry->active = true;
							entry->value = 0;
							entry->unknown01 = 0xff;
						}
					}
				}
				g_511bf4.groups[index][0].value = 1;
			}
		}
		if ((bool)((function_xaee93d()->flags >> 5) & 1))
		{
			s_results_ranking_195ed0 ranking;
			long rank;

			function_23f3e0((s_score_display *)&ranking, 1, false);
			for (rank = 0; rank < ranking.team_count; rank++)
			{
				long team = ranking.teams[rank];
				s_input_entry *entry = &g_511a74[team];

				entry->unknown01 = ranking.team_ranks[rank] / 2;
				entry->value = (short)PIN(team_score_195ed0(team), 0, 0x7fff);
			}
			for (rank = 0; rank < ranking.player_count; rank++)
			{
				((s_results_player_195ed0 *)input_device(ranking.players[rank] & 0xffff))->rank = ranking.player_ranks[rank] / 2;
			}
		}
		function_199310(&g_510cb0);
		for (long index = 0; index < 16; index++)
		{
			s_input_address *address = &g_51e8d4[index];

			if (address->data[6])
			{
				if (!memcmp(g_4cf7cc, address->data, 6))
				{
					address->data[8] = true;
					address->data[7] = true;
				}
				else
				{
					s_network_observer *observer;
					bool connected;

					address->data[8] = false;
					observer = *(s_network_observer **)((byte *)g_4cf780 + 8);
					connected = false;
					if (observer && simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
						(s_machine_address const *)address->data) != NONE)
					{
						long channel = network_observer_find_channel_by_machine(observer, 3,
							(s_session_machine_address const *)address->data);

						if (channel != NONE && *(long *)((byte *)observer + channel * 0x528 + 0xa8) == 7)
						{
							connected = true;
						}
					}
					address->data[7] = connected;
				}
			}
		}
	}
}


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
long network_observer_find_channel_by_machine(s_network_observer *observer, long owner, s_session_machine_address const *address);

// Disabled: the contiguous results object still has split shared bindings.
// Retail 0x195ed0; the storage parameter is only an analysis binding.
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
						long channel = network_observer_find_channel_by_machine(observer, 3,
							(s_session_machine_address const *)address->data);
						if (channel != NONE && *(long *)((byte *)observer + channel * 0x528 + 0xa8) == 7)
							connected = true;
					}
					address->data[7] = connected;
				}
			}
		}
	}
}

extern dword g_510ca4, g_510ca8;
struct s_surface_description;
s_surface_description *function_192e60(long index);
void utf8_string_to_utf16_string(char const *source, word *destination, long destination_count);

// Disabled: the contiguous 0xdcd8-byte results object still has split shared bindings.
// Retail 0x195a40; results exposes the missing contiguous storage binding.
void function_195a40(byte *results)
{
    memset(results, 0, 0xdcd8);
    g_510ca4 = 0;
    g_510ca8 = (long)time(NULL);
    if (g_55e4d0[g_4e9ae8->engine_index])
    {
        byte *options = (byte *)g_4e6948;
        *(long *)(results + 0x130) = *(long *)(options + 0x10);
        *(long *)(results + 0x134) = *(long *)(options + 0x14);
        *(long *)(results + 4) = *(long *)(options + 0x138);
        if (*(long *)(options + 0x138) != NONE)
        {
            byte *variant = (byte *)function_192e60(*(long *)(options + 0x138));
            if (variant)
            {
                long mode = *(long *)variant;
                results[0x12a] = mode == 5;
                results[0x128] = mode == 5 || mode == 2 || mode == 4;
                results[0x129] = mode == 5 || mode == 3 || mode == 4;
                for (long i = 0; i < 9; ++i)
                    utf8_string_to_utf16_string((char const *)(variant + 0xc + i * 0x10),
                        (word *)(results + 8 + i * 0x20), 0x10);
            }
        }
        *(long *)(results + 0x130) = *(long *)(options + 0x10);
        *(long *)(results + 0x134) = *(long *)(options + 0x14);
        memcpy(results + 0x138, options + 0x13c, 0x130);
        *(long *)(results + 0x268) = *(long *)(options + 0x20);
        memcpy(results + 0x26c, options + 0x24, 0x104);
        for (long i = 0; i < 16; ++i)
        {
            byte *source = options + 0x2dc + i * 0xe4;
            byte *record = results + 0x384 + i * 0xa4;
            record[0] = source[0];
            if (record[0])
            {
                memcpy(record + 2, source + 0xe, 12);
                memcpy(record + 0x10, source + 0x1c, 0x90);
                record[1] = function_199250((s_address_table *)results, source + 8);
                if (record[1] == 0xff) record[1] = function_1991f0((s_address_table *)results, source + 8);
            }
        }
        results[0x12b] = results[0x180] & 1;
        if (results[0x12b])
        {
            for (long i = 0; i < 16; ++i)
            {
                byte *record = results + 0x384 + i * 0xa4;
                long team = *(char *)(record + 0x8c);
                if (!record[0] || team == NONE) continue;
                byte *entry = results + 0xdc4 + team * 0x18;
                if (entry[0]) continue;
                entry[0] = 1;
                entry[1] = 0xff;
                *(short *)(entry + 2) = 0;
                *(short *)(entry + 4) = NONE;
                *(long *)(entry + 8) = NONE;
                memset(entry + 0xc, 0, 12);
                if (results[0x12a] && memcmp(record + 0x80, g_440070, 12))
                {
                    memcpy(entry + 0xc, record + 0x80, 12);
                    *(long *)(entry + 8) = *(long *)(record + 0x9c);
                    *(short *)(entry + 4) = *(short *)(record + 0x9a);
                }
            }
        }
        byte *session = *(byte **)((byte *)g_4cf780 + 0xc);
        bool local_host = false;
        if (session && simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
            (s_machine_address const *)g_4cf7cc) != NONE)
            local_host = !memcmp(session + 0x62 + *(long *)(session + 0x40) * 0x10c, g_4cf7cc, 6);
        for (long i = 0; i < 16; ++i)
        {
            byte *address = results + 0xdc24 + i * 11;
            if (!address[6]) continue;
            session = *(byte **)((byte *)g_4cf780 + 0xc);
            bool remote_host = session && simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
                (s_machine_address const *)address) != NONE &&
                !memcmp(session + 0x62 + *(long *)(session + 0x40) * 0x10c, address, 6);
            address[8] = remote_host;
            address[9] = remote_host;
            if (local_host && remote_host) address[7] = 1;
            else if (local_host || remote_host)
            {
                s_network_observer *observer = *(s_network_observer **)((byte *)g_4cf780 + 8);
                long channel = NONE;
                if (observer && simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780,
                    (s_machine_address const *)address) != NONE)
                    channel = network_observer_find_channel_by_machine(observer, 3, (s_session_machine_address const *)address);
                address[7] = channel != NONE && *(long *)((byte *)observer + channel * 0x528 + 0xa8) == 7;
            }
            else address[7] = 0;
        }
        results[0] = 1;
        if (g_4e6948->mode != 4)
        {
            g_510ca0 = true;
            g_510ca1 = false;
            function_196470();
            function_196780();
        }
    }
}

#endif
