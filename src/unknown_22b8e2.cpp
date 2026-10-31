// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "bitmap_sequences.h"

struct s_4e6950
{
	byte field_0[0x28];
	long field_28, field_2c;
	color4f field_30;
	byte field_40[0x40];
};
extern s_4e6950 g_4e6950;
struct s_filter_vector;

struct s_22b8e2
{
	byte field_0[0x1c];
	word field_1c;
	byte field_1e[6];
	long field_24;
	byte field_28[4];
	long field_2c;
	byte field_30[4];
	long field_34;
	byte field_38[4];
	long field_3c;
	signed char field_40[3];
	byte field_43;
	short field_44[6];
};

long function_13a690(long);
bool function_22acb4(long);
void function_1396c7(long, point2f *);
void function_2d140(long, real const *, long, long, s_filter_vector const *);

// @retail 0x22b8e2
void function_22b8e2(s_22b8e2 const *arg_0, long arg_1, color4f const *arg_2)
{
	long local_0 = function_13a690(arg_0->field_1c);
	long local_1;
	point2f local_4;
	long local_2;
	switch (local_0)
	{
	case 0: local_1 = arg_0->field_2c; break;
	case 1: local_1 = arg_0->field_34; break;
	case 2: local_1 = arg_0->field_3c; break;
	default: return;
	}
	if (local_1 == NONE)
		return;
	local_2 = NONE;
	long local_3 = 0;
	switch (local_0)
	{
	case 0: local_3 = arg_0->field_40[0]; break;
	case 1: local_3 = arg_0->field_40[1]; break;
	case 2: local_3 = arg_0->field_40[2]; break;
	}
	if (arg_0->field_24 != NONE)
	{
		s_bitmap_tag_22ae21 const *tag = (s_bitmap_tag_22ae21 const *)g_4e3b44[arg_0->field_24 & 0xffff].bytes;
		if (local_3 >= 0 && local_3 < tag->sequence_count)
		{
			s_bitmap_sequence_22ae21 const *sequence = &tag->sequences[local_3];
			if (sequence->sprite_count > 0)
			{
				long index = 0;
				if (sequence->sprite_count > 1 && function_22acb4(arg_1))
					index = 1;
				local_2 = sequence->sprites[index].bitmap_index;
			}
			else if (sequence->bitmap_count > 0)
			{
				long index = 0;
				if (sequence->bitmap_count > 1 && function_22acb4(arg_1))
					index = 1;
				local_2 = sequence->first_bitmap + index;
			}
		}
		else if (local_3 == 0 && tag->sequence_count == 0 && tag->bitmap_count > 0)
			local_2 = 0;
	}
	function_1396c7(arg_0->field_1c, &local_4);
	switch (local_0)
	{
	case 0: local_4.x += arg_0->field_44[0]; local_4.y += arg_0->field_44[1]; break;
	case 1: local_4.x += arg_0->field_44[2]; local_4.y += arg_0->field_44[3]; break;
	case 2: local_4.x += arg_0->field_44[4]; local_4.y += arg_0->field_44[5]; break;
	}
	real local_15 = 0.5f / (g_4b9dd6 - g_4b9dd2);
	real local_16 = 0.5f / (g_4b9dd4 - g_4b9dd0);
	real local_11 = (real)g_4b9dd2;
	real local_13 = (real)g_4b9dd6;
	real local_21[9][4];
	local_21[5][0] = local_21[2][0] = local_13;
	real local_14 = (real)g_4b9dd4;
	local_21[8][0] = local_13;
	local_21[0][0] = local_11;
	real local_12 = (real)g_4b9dd0;
	local_21[6][0] = local_21[3][0] = local_11;
	local_21[7][0] = local_21[4][0] = local_21[1][0] = local_4.x;
	local_21[8][1] = local_21[7][1] = local_21[6][1] = local_14;
	local_21[2][1] = local_21[1][1] = local_21[0][1] = local_12;
	local_21[5][1] = local_21[4][1] = local_21[3][1] = local_4.y;
	real local_17 = 1.0f - (local_4.x - local_11) * local_15;
	real local_19 = 1.0f - (local_4.y - local_12) * local_16;
	real local_18 = 1.0f - (local_13 - local_4.x) * local_15;
	real local_20 = 1.0f - (local_14 - local_4.y) * local_16;
	local_21[0][2] = local_17;
	local_21[0][3] = local_19;
	local_21[1][2] = 1.0f;
	local_21[1][3] = local_19;
	local_21[2][2] = local_18;
	local_21[2][3] = local_19;
	local_21[3][2] = local_17;
	local_21[3][3] = 1.0f;
	local_21[4][2] = 1.0f;
	local_21[4][3] = 1.0f;
	local_21[5][2] = local_18;
	local_21[5][3] = 1.0f;
	local_21[6][2] = local_17;
	local_21[6][3] = local_20;
	local_21[7][2] = 1.0f;
	local_21[7][3] = local_20;
	local_21[8][2] = local_18;
	local_21[8][3] = local_20;
	g_4e6950.field_28 = arg_0->field_24;
	g_4e6950.field_2c = local_2;
	g_4e6950.field_30 = *arg_2;
	function_2d140(local_1, (real const *)arg_2, 3, 3, (s_filter_vector const *)local_21);
}
