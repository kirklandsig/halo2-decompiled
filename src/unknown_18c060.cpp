// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "local_cameras.h"
#include "object_queries.h"

struct s_unknown_13bf00;
struct s_object;
extern s_unknown_13bf00 *g_510c50;
extern s_record_pool *g_4ed28c;
extern dword g_54e8a0[16];

struct s_18c060
{
	short field_0;
	byte field_2;
	byte field_3;
	union
	{
		word flags;
		struct
		{
			byte field_4;
			byte field_5;
		};
	};
	short field_6;
	real field_8;
	long field_c;
	long field_10;
	long field_14;
};

struct s_18c061
{
	byte field_0[0x78];
	bool field_78;
	bool field_79;
	bool field_7a;
	byte field_7b[3];
	bool field_7e;
};

struct s_18c062
{
	byte field_0[5];
	bool field_5;
};

void function_127400(bool arg_0);
void sound_set_ambience(long arg_0);
void sound_audible_clusters_update(void);
void __stdcall function_18bb80(real arg_0);
void __stdcall function_225b60(real arg_0);
void function_18bf90(void);
void sound_effects_update(void);
void __stdcall function_18a7b0(long arg_0);
s_object *function_badc0(long arg_0, dword arg_1);
long __stdcall function_18d1c0(long arg_0);
bool function_b9d20(long arg_0);

#pragma inline_depth(0)
// @retail 0x18c060
void __stdcall function_18c060(real arg_0)
{
	if (((s_18c061 *)g_4e6380)->field_78 &&
		((s_18c061 *)g_4e6380)->field_79 &&
		((s_18c061 *)g_4e6380)->field_7a)
		function_127400(false);

	sound_audible_clusters_update();
	function_18bb80(arg_0);
	function_225b60(arg_0);
	function_18bf90();
	long local_0 = NONE;
	if (g_510c50 && ((s_18c062 *)g_510c50)->field_5)
		local_0 = 0;
	else if (g_4ed288->value24 >= g_510c54->game_time)
		local_0 = 1;
	sound_set_ambience(local_0);

	s_data_datum_iterator local_2;
	local_2.data = g_4ed28c;
	local_2.index = NONE;
	s_18c060 *local_3;
	for (;;)
	{
		long local_9 = local_2.index + 1;
		long local_10 = NONE;
		s_record_pool *local_11 = local_2.data;
		if (local_9 >= 0 && local_9 < local_11->high_water_index)
		{
			long local_12 = local_11->high_water_index;
			dword *local_13 = local_11->bitmap;
			do
			{
				if (local_13[local_9 >> 5] & (1 << (local_9 & 0x1f)))
				{
					local_10 = local_9;
					break;
				}
				local_9++;
			} while (local_9 < local_12);
		}
		if (local_10 == NONE)
			break;
		local_3 = (s_18c060 *)(local_11->data + local_11->size * local_10);
		local_2.datum = (byte *)local_3;
		local_2.index = local_10;
		local_2.datum_index = (local_3->field_0 << 16) | local_10;
		if (local_3->field_3 == 4 && local_3->field_10 != NONE &&
			local_3->field_10 <= g_510c54->game_time)
		{
			((s_18c060 *)g_4ed28c->data)[local_2.datum_index & 0xffff].flags |= 2;
		}
		if (local_3->field_3 != 1)
		{
			function_18a7b0(local_2.datum_index);
		}
		else if ((local_3->field_4 & 1) && !function_badc0(local_3->field_10, (dword)NONE))
		{
			if (function_18d1c0(local_3->field_c) == local_2.datum_index)
			{
				long local_5 = local_3->field_c;
				long local_6 = function_18d1c0(local_5);
				if (local_6 != NONE)
				{
					s_18c060 *local_7 = (s_18c060 *)g_4ed28c->data + (local_6 & 0xffff);
					if (local_7->field_c == local_5)
						local_7->field_4 &= ~0x20;
				}
			}
			record_pool_release(g_4ed28c, local_2.datum_index);
		}
		else if (function_b9d20(local_3->field_10))
		{
			s_location local_8;
			object_get_root_location(local_3->field_10, &local_8);
			if (local_8.cluster_index != NONE &&
				(g_54e8a0[local_8.cluster_index >> 5] & (1 << (local_8.cluster_index & 0x1f))))
			{
				function_18a7b0(local_2.datum_index);
			}
			else
				local_3->field_5 &= ~1;
		}
	}
	((s_18c061 *)g_4e6380)->field_7e = true;
	sound_effects_update();
}
#pragma inline_depth(255)
