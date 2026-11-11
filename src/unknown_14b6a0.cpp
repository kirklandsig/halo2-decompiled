#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "data_iterator.h"
#include "unknown_067e10.h"
#include "vehicle_ray.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"

struct s_player_creation_record;
class c_class_6a600;
extern byte g_4cf77b;
extern byte g_4cf77a;

#pragma pack(push, 2)
struct s_player_creation_record
{
	bool active;
	bool field_2_2;
	short controller_index;
	long value04;
	s_machine_address machine;
	dword key[3];
	byte unknown1a[2];
	dword configuration[0x24];
	byte unknownac[0xe4 - 0xac];
};
#pragma pack(pop)

void __stdcall function_14c090(long index, bool reuse, s_player_creation_record const *record);
void function_694c0(c_class_6a600 *world, long index);
void ai_player_add(long index);
void function_157be0(dword index);
void function_157ed0(long index);
void function_152df0(long index);
point3f *function_b9dd0(long object_index, point3f *point);
struct s_location;
void function_b75a0(long object_index, point3f const *position, vector3f const *forward,
	vector3f const *up, s_location const *location, bool update);
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray, long ignore, long ignore2);
void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
void function_1c7e70(long player_index, short value);
void __stdcall function_14e0f0(long player_index, long target_index, point3f const *position,
	point3f const *alternate);
void function_152340();

// @retail 0x14bf80
void __stdcall function_14bf80(long player_index, s_player_creation_record const *record)
{
	function_14c090(player_index, false, record);
	++*(long *)g_4e8c20;
	if (g_4cf770 && !g_4cf77b)
		function_694c0((c_class_6a600 *)g_4cf77c, player_index);
	function_157ed0(player_index);
}

// @retail 0x14bc00
long function_14bc00(long player_index, s_player_creation_record const *record)
{
	long index = function_16b990(g_4e8c24, player_index);
	if (index != NONE)
	{
		function_14c090(index, true, record);
		*(short *)(g_4e8c24->data + (index & 0xffff) * 0x21c + 0x218) = 0;
		if (!*((byte const *)record + 1))
		{
			++*(long *)g_4e8c20;
			if (g_4cf770 && !g_4cf77b)
				function_694c0((c_class_6a600 *)g_4cf77c, index);
			ai_player_add(index);
			function_157be0(index);
		}
	}
	return index;
}

void function_14bac0();

static __forceinline void players_pending_creation_process(s_record_pool_iterator * iterator_reference)
{
	long absolute_index;
	while ((absolute_index = data_next_absolute_index_inlined(iterator_reference->data, iterator_reference->index + 1)) != NONE)
	{
		byte *player = iterator_reference->data->data + iterator_reference->data->size * absolute_index;
		long index = (*(short *)player << 16) | absolute_index;
		iterator_reference->index = absolute_index;
		if (!(player[2] & 2))
		{
			s_player_creation_record record;
			memset(&record, 0, sizeof(record));
			record.machine = *(s_machine_address *)(player + 0x14);
			record.controller_index = *(short *)(player + 0x1c);
			record.value04 = *(long *)(player + 0x20);
			memcpy(record.key, player + 4, sizeof(record.key));
			record.active = true;
			memcpy(record.configuration, player + 0xd4, sizeof(record.configuration));
			function_152df0(index);
			function_14c090(index, false, &record);
			++*(long *)g_4e8c20;
			if (g_4cf770 && !g_4cf77b)
				function_694c0((c_class_6a600 *)g_4cf77c, index);
			function_157ed0(index);
		}
		if (!g_4cf77a)
			player[2] |= 8;
	}
}

// @retail 0x14bac0
void function_14bac0()
{
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	
	s_record_pool_iterator *iterator_reference = &iterator;
	players_pending_creation_process(iterator_reference);
}
/* a pending request: moves every player's root object by the offset between two
   markers, then points the other players at a destination marker (or the requester
   unit's centre) and an optional alternate; then clears +0x2a of every player */
// @retail 0x14b6a0
void function_14b6a0()
{
	byte *globals = (byte *)g_4e8c20;
	short request = *(short *)(globals + 0xa4);
	if (request != NONE)
	{
		long player_index = *(long *)(globals + 0xa8);
		byte *scenario = (byte *)g_4e0350;
		byte *record = *(byte **)(scenario + 0x134) + request * 14;
		byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
		if (*(short *)(record + 8) != NONE)
		{
			byte *markers = *(byte **)(scenario + 0x1e4);
			point3f const *destination = (point3f *)(markers + *(short *)(record + 8) * 0x38 + 0x24);
			point3f const *origin;
			if (*(short *)(record + 6) != NONE)
				origin = (point3f *)(markers + *(short *)(record + 6) * 0x38 + 0x24);
			else
				origin = (point3f *)(*(byte **)(scenario + 0x10c) + *(short *)record * 0x44 + 0x24);
			vector3f offset;
			offset.i = destination->x - origin->x;
			offset.j = destination->y - origin->y;
			offset.k = destination->z - origin->z;
			++g_4de2fc;
			g_4de2f8 = true;
			s_data_datum_iterator player_walk;
			player_walk.data = g_4e8c24;
			player_walk.index = NONE;
			while (data_datum_iterator_next_called(&player_walk))
			{
				long unit = *(long *)(player_walk.datum + 0x2c);
				if (unit != NONE)
				{
					byte *headers = g_4e0300->data;
					long root;
					do
					{
						root = unit;
						byte *object = *(byte **)(headers + (unit & 0xffff) * 12 + 8);
						unit = *(long *)(object + 0x14);
					} while (unit != NONE);
					long slot = root & 0xffff;

					if (g_4de300[slot] != g_4de2fc)
					{
						g_4de300[slot] = g_4de2fc;
						point3f translated;
						function_b9dd0(root, &translated);
						translated.x += offset.i;
						translated.y += offset.j;
						translated.z += offset.k;
						function_b75a0(root, &translated, NULL, NULL, NULL, false);
					}
				}
			}
			g_4de2f8 = false;
		}
		if (*(long *)g_4e8c20 > 1)
		{
			bool have_destination = false;
			bool have_alternate = false;
			point3f destination, alternate;
			short target_marker = *(short *)(record + 0xa);
			if (target_marker != NONE)
			{
				byte *marker = *(byte **)(scenario + 0x1e4) + target_marker * 0x38;
				point3f point = *(point3f *)(marker + 0x24);
				while (function_168f40(0x800005, (s_vehicle_ray const *)&point, NONE, NONE)) {}
				have_destination = true;
				destination = point;
			}
			short alternate_marker = *(short *)(record + 0xc);
			if (alternate_marker != NONE)
			{
				byte *marker = *(byte **)(scenario + 0x1e4) + alternate_marker * 0x38;
				point3f point = *(point3f *)(marker + 0x24);
				while (function_168f40(0x800005, (s_vehicle_ray const *)&point, NONE, NONE)) {}
				have_alternate = true;
				alternate = point;
				alternate.z += 1.0f;
			}
			{
				point3f center;
				real height, radius;
				function_df5f0(*(long *)(player + 0x2c), &center, &height, &radius);
				if (have_destination)
					destination.z += radius;
				else if (!function_168f40(0x800005, (s_vehicle_ray const *)&center, NONE, NONE))
				{
					destination = center;
					have_destination = true;
				}
			}
			if (have_destination)
			{
				s_data_datum_iterator iterator;
				iterator.data = g_4e8c24;
				iterator.index = NONE;
				iterator.datum_index = NONE;
				while (data_datum_iterator_next(&iterator))
				{
					if (iterator.datum_index != *(long *)((byte *)g_4e8c20 + 0xa8))
					{
						function_1c7e70(iterator.datum_index, g_4686c4);
						function_14e0f0(iterator.datum_index, *(long *)(player + 0x2c), &destination,
							have_alternate ? &alternate : NULL);
					}
				}
			}
		}
		*(short *)((byte *)g_4e8c20 + 0xa4) = NONE;
	}
	s_record_pool_iterator players;
	byte *player;

	players.data = g_4e8c24;
	players.index = NONE;
	while ((player = data_iterator_next_inlined(&players)) != NULL)
		*(short *)(player + 0x2a) = NONE;
	function_152340();
}
