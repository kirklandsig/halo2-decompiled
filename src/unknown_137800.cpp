#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "geometry_cache.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

extern dword g_4e6494;

struct s_node_matrix
{
	real rows[3][4];
};

struct s_137800
{
	byte field_0[4];
	byte field_4;
	byte field_5[0x17];
	long field_1c;
	byte field_20[8];
	byte *field_28;
	byte field_2c[0xc];
	byte *field_38;
	char field_3c[0xc];
	long field_48;
};

struct s_137801
{
	short field_0;
	short field_2;
	short field_4[16][2];
	s_node_matrix field_44[256];
};

long function_1376c0(long model_index, s_node_matrix *matrices, transform4x3f const *nodes);
transform4x3f const *__stdcall function_bd610(long arg_1, byte const *arg_2, transform4x3f const *arg_3);

/* builds the node matrices for 0x137800. The 16-byte aligned array lives here because a function that declares one itself
   moves the register arguments of the functions it reaches to the stack, and LTCG sets those conventions before inlining */
PRIVATE __forceinline void node_matrices_build(long arg_1, transform4x3f const *arg_3, byte const *arg_4, s_137801 *arg_7,
	s_137800 *local_2, long local_3)
{
	__declspec(align(16)) s_node_matrix local_1[255];
	s_node_matrix *local_4 = (local_2->field_4 & 4) ? local_1 : arg_7->field_44;
	arg_7->field_2 = (short)local_2->field_1c;
	long local_5 = function_1376c0(arg_1, local_4, arg_3);
	byte *local_6 = local_2->field_38 + local_3 * 12;
	for (long local_7 = 0; local_7 < *(long *)(local_6 + 4); local_7++)
	{
		byte *local_8 = *(byte **)(local_6 + 8) + local_7 * 16;
		s_node_matrix local_9;
		memset(&local_9, 0, sizeof(local_9));
		real local_10 = 0.0f;
		for (dword local_11 = 0; local_11 < 4; local_11++)
		{
			dword local_12 = local_8[local_11];
			if (local_12 == 0xff)
				break;
			s_node_matrix *local_13 = &local_4[local_12];
			real local_14 = local_11 < 3 ? ((real *)(local_8 + 4))[local_11] : 1.0f - local_10;
			local_9.rows[0][0] = local_13->rows[0][0] * local_14 + local_9.rows[0][0];
			local_9.rows[0][1] = local_13->rows[0][1] * local_14 + local_9.rows[0][1];
			local_9.rows[0][2] = local_13->rows[0][2] * local_14 + local_9.rows[0][2];
			local_9.rows[0][3] = local_13->rows[0][3] * local_14 + local_9.rows[0][3];
			local_9.rows[1][0] = local_13->rows[1][0] * local_14 + local_9.rows[1][0];
			local_9.rows[1][1] = local_13->rows[1][1] * local_14 + local_9.rows[1][1];
			local_9.rows[1][2] = local_13->rows[1][2] * local_14 + local_9.rows[1][2];
			local_9.rows[1][3] = local_13->rows[1][3] * local_14 + local_9.rows[1][3];
			local_9.rows[2][0] = local_13->rows[2][0] * local_14 + local_9.rows[2][0];
			local_9.rows[2][1] = local_13->rows[2][1] * local_14 + local_9.rows[2][1];
			local_9.rows[2][2] = local_13->rows[2][2] * local_14 + local_9.rows[2][2];
			local_9.rows[2][3] = local_13->rows[2][3] * local_14 + local_9.rows[2][3];
			local_10 = local_14 + local_10;
		}
		local_4[local_5++] = local_9;
	}
	if (local_2->field_4 & 4)
	{
		arg_7->field_44[0] = local_4[0];
		local_5 = 1;
	}
	arg_7->field_0 = (short)local_5;
	for (long local_15 = 0; local_15 < local_2->field_1c; local_15++)
	{
		byte local_16 = arg_4[local_15];
		if (local_16 != 0xff)
		{
			byte *local_17 = local_2->field_28 + local_16 * 0x5c;
			byte *local_18 = NULL;
			if (!g_4e6494 || function_12de70((s_geometry_block_info *)(local_17 + 0x38), 0))
				local_18 = *(byte **)(local_17 + 0x34);
			if (*(long *)(local_18 + 0x64) > 0)
			{
				byte *local_19 = *(byte **)(local_18 + 0x68);
				arg_7->field_4[local_15][0] = (short)local_5;
				arg_7->field_4[local_15][1] = *(short *)(local_18 + 0x64);
				for (long local_20 = 0; local_20 < *(long *)(local_18 + 0x64); local_20++)
					arg_7->field_44[local_5++] = local_4[local_19[local_20]];
			}
			else
			{
				arg_7->field_4[local_15][0] = 0;
				arg_7->field_4[local_15][1] = local_17[0x24];
				if (!arg_7->field_4[local_15][1])
					arg_7->field_4[local_15][1] = (short)(local_2->field_48 < 40 ? local_2->field_48 : 40);
			}
		}
		else
		{
			arg_7->field_4[local_15][0] = 0;
			arg_7->field_4[local_15][1] = 0;
		}
	}
}

// @retail 0x137800
void function_137800(long arg_2, long arg_1, transform4x3f const *arg_3, byte const *arg_4, long arg_5, bool arg_6, s_137801 *arg_7)
{
	s_137800 *local_2 = (s_137800 *)g_4e3b44[arg_1 & 0xffff].bytes;
	long local_3 = local_2->field_3c[arg_5];
	if (!arg_6 && arg_2 != NONE)
		arg_3 = function_bd610(arg_2, arg_4, arg_3);
	node_matrices_build(arg_1, arg_3, arg_4, arg_7, local_2, local_3);
}
