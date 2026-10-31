#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "globals.h"
#include "font_loading.h"
// @flags /O2 /arch:SSE /Gr
struct short_rectangle2d;
void function_1fa50(short_rectangle2d const *bounds, word const *text, void const *clip, void const *position, long line_gap, real scale, long color, void const *shadow);

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
struct s_display_text;
real display_text_get_alpha(s_display_text const *arg_1);
bool function_13cbf0(void);
void function_13ccf0(short_rectangle2d *arg_2, short_rectangle2d *arg_3, short_rectangle2d *arg_1);
void function_13e8a0(void);
void function_1a0180(long arg_1, long arg_2, word *arg_3);
bool function_13ee20(word const *arg_1, long arg_2);
void function_13e9c0(word const *arg_1, short_rectangle2d const *arg_2, short_rectangle2d *arg_3, short_rectangle2d *arg_4, real arg_5);
void function_13ec70(color4f const *arg_1);

long function_122540(long font);

// @retail 0x13cf00
void function_13cf00(void)
{
	if (function_13cbf0())
	{
		byte *local_1 = (byte *)g_4e0350;
		long local_2 = 0;
		long local_4 = function_122540(9);
		function_13e8a0();
		font_get(g_4e28f4[9]);
		g_4e73a0.font = 9;
		g_4e73a0.style = NONE;
		g_4e73a0.justification = 2;
		g_4e73a0.flags = 1;
		if (local_1 && *(long *)(local_1 + 0x35c) != NONE)
		{
			byte *local_5 = (byte *)g_510c50 + 0x18;
			short_rectangle2d local_6, local_7, local_8;
			function_13ccf0(&local_6, &local_7, &local_8);
			if (local_6.left <= 48)
				local_6.left = 48;
			if (local_6.right > 592)
				local_6.right = 592;
			if (local_6.top <= 36)
				local_6.top = 36;
			if (local_6.bottom > 444)
				local_6.bottom = 444;
			long local_9 = *(long *)local_5;
			bool local_10 = false;
			word local_11[256];
			if (local_9)
			{
				local_11[0] = 0;
				function_1a0180(*(long *)(local_1 + 0x35c), local_9, local_11);
				local_10 = function_13ee20(local_11, g_4e73a0.font);
				if (local_10)
				{
					short_rectangle2d local_14, local_15;
					function_13e9c0(local_11, &local_6, &local_14, &local_15, 1.0f);
					local_2 = (local_15.bottom - local_6.top) / local_4;
				}
			}
			short local_12 = local_6.bottom;
			local_6.top = (short)(local_12 - local_4 * local_2);
			local_9 = *(long *)local_5;
			if (local_9 && local_10)
			{
				local_11[0] = 0;
				color4f local_13;
				local_13.alpha = display_text_get_alpha((s_display_text *)local_5);
				function_1a0180(*(long *)(local_1 + 0x35c), local_9, local_11);
				short_rectangle2d local_16;
				local_16.left = local_6.left + 1;
				local_16.right = local_6.right + 1;
				local_16.top = local_6.top + 2;
				local_16.bottom = (short)(local_12 + 2);
				local_13.red = 0.0f;
				local_13.green = 0.0f;
				local_13.blue = 0.0f;
				function_13ec70(&local_13);
				function_1fa50(&local_16, (word const *)local_11, NULL, NULL, 0, 1.0f, 0, NULL);
				local_13.red = 0.76078433f;
				local_13.green = 0.92941177f;
				local_13.blue = 1.0f;
				function_13ec70(&local_13);
				function_1fa50(&local_6, (word const *)local_11, NULL, NULL, 0, 1.0f, 0, NULL);
			}
		}
	}
}

void interface_rectangle_fill(short_rectangle2d const *arg_1, dword arg_2);
bool function_147d13(void);
color3f *unpack_color3f(dword arg_1, color3f *arg_2);
void function_13ed50(color4f const *arg_1);

struct s_13c780
{
	long field_0;
	short_rectangle2d field_4;
	short field_c;
	short field_e;
	dword field_10;
	dword field_14;
	real field_18;
	real field_1c;
	real field_20;
};

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

// @retail 0x13c780
void function_13c780(void)
{
	byte *local_1 = (byte *)g_510c50;
	if (local_1)
	{
		if (*(real *)local_1 > 0.0f && !function_147d13())
		{
			dword local_2 = (dword)(real_to_long(*(real *)local_1 * 255.0f) < 0 ? 0 :
				real_to_long(*(real *)local_1 * 255.0f) > 255 ? 255 : real_to_long(*(real *)local_1 * 255.0f)) << 24;
			short_rectangle2d local_3, local_4, local_5;
			function_13ccf0(&local_3, &local_4, &local_5);
			interface_rectangle_fill(&local_4, local_2);
			interface_rectangle_fill(&local_5, local_2);
		}
		for (long local_6 = 0; local_6 < 4; local_6++)
		{
			local_1 = (byte *)g_510c50;
			short *local_7 = (short *)(local_1 + 8 + local_6 * 4);
			if (local_7[0] != NONE)
			{
				byte *local_8 = (byte *)g_4e0350;
				s_13c780 *local_9 = &(*(s_13c780 **)(local_8 + 0x1f4))[local_7[0]];
				long local_10 = *(long *)(local_8 + 0x204);
				if (local_10 != NONE)
				{
					word local_11[256];
					local_11[0] = 0;
					function_1a0180(local_10, local_9->field_0, local_11);
					if (function_13ee20(local_11, g_4e73a0.font))
					{
						real local_13 = 1.0f;
						short_rectangle2d const *local_12 = &local_9->field_4;
						if (local_12->right == local_12->left || local_12->bottom == local_12->top)
							local_12 = (short_rectangle2d *)((byte *)g_510c94 + 0x29c);
						real local_14 = local_7[1] * g_510c54->rate;
						if (local_9->field_18 > local_14)
							local_13 = local_14 / local_9->field_18;
						else if (local_14 > local_9->field_1c)
							local_13 = 1.0f - (local_14 - local_9->field_1c) / local_9->field_20;
						if (0.0f > local_13) local_13 = 0.0f;
						else if (local_13 > 1.0f) local_13 = 1.0f;
						color4f local_15, local_16;
						unpack_color3f(local_9->field_10, (color3f *)&local_15.red);
						if (fabs(local_15.red - 1.0f) < 0.0001f && fabs(local_15.green - 1.0f) < 0.0001f && fabs(local_15.blue - 1.0f) < 0.0001f)
						{
							if (local_15.red > 0.8f) local_15.red = 0.8f;
							if (local_15.green > 0.8f) local_15.green = 0.8f;
							if (local_15.blue > 0.8f) local_15.blue = 0.8f;
						}
						unpack_color3f(local_9->field_14, (color3f *)&local_16.red);
						local_15.alpha = local_13;
						local_16.alpha = local_13;
						function_13e8a0();
						long local_17 = local_9->field_e;
						font_get(g_4e28f4[local_17]);
						g_4e73a0.font = local_17;
						long local_18 = local_9->field_c;
						g_4e73a0.style = NONE;
						g_4e73a0.justification = local_18;
						g_4e73a0.flags = 1;
						function_13ec70(&local_15);
						function_13ed50(&local_16);
						function_1fa50(local_12, (word const *)local_11, NULL, NULL, 0, 1.0f, 0, NULL);
					}
				}
			}
		}
	}
	function_13cf00();
}
