// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include "unknown_293070.h"
#include <math.h>

struct s_293130
{
	short field_0;
	short field_2;
	byte field_4[0xc];
	short field_10;
	byte field_12[0x16];
};

struct s_293131
{
	point3f field_0;
	real field_c;
	byte field_10[4];
	real field_14;
	real field_18;
};

struct s_293132
{
	byte field_0[0xc];
	long field_c;
	s_293131 *field_10;
	long field_14;
	byte field_18[8];
	real field_20;
	real field_24;
	byte field_28[4];
	long field_2c;
	byte field_30[0x54];
};

struct s_293133
{
	byte field_0[0x350];
	long field_350;
	s_293132 *field_354;
};

struct s_293134
{
	byte field_0[0x1c];
	point3f field_1c;
	vector3f field_28;
	byte field_34[0x24];
	real field_58;
	byte field_5c[0x58];
	word field_b4;
	word field_b6;
	byte field_b8[0xc];
};

struct s_295600_state
{
	short duration;
	short elapsed;
	vector3f current;
	vector3f previous;
};

struct s_293135
{
	long field_0;
	short field_4;
	byte field_6[6];
	bool field_c;
	bool field_d;
	bool field_e;
	byte field_f;
	short field_10;
	short field_12;
	long field_14;
	s_295600_state field_18;
};

struct s_effect_owner;
extern s_record_pool *g_51ecb4;
void function_b7930(void *arg_0, long arg_1, long arg_2, s_effect_owner const *arg_3);
long __stdcall function_b7b40(void *arg_0);
void __stdcall function_b8540(long arg_0);
bool function_2936b0(long arg_0, long arg_1);

__forceinline void function_293133(real arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = (real)sin(arg_0);
	real local_1 = (real)cos(arg_0);
	real vector_i = arg_2->i;
	real vector_j = arg_2->j;
	real vector_k = arg_2->k;
	real one_minus_cosine = 1.0f - local_1;
	real local_2 = dot3f(arg_1, arg_2) * one_minus_cosine;
	real local_3 = arg_1->k * vector_j - vector_k * arg_1->j;
	real local_4 = vector_k * arg_1->i - arg_1->k * vector_i;
	real local_5 = vector_i * arg_1->j - arg_1->i * vector_j;
	real cosine_term_i = vector_i * local_1;
	real axis_term_i = arg_1->i * local_2;
	real cross_term_i = local_3 * local_0;
	real projected_term_i = cosine_term_i + axis_term_i;
	arg_2->i = projected_term_i - cross_term_i;
	real cosine_term_j = vector_j * local_1;
	real axis_term_j = arg_1->j * local_2;
	real cross_term_j = local_4 * local_0;
	real projected_term_j = cosine_term_j + axis_term_j;
	arg_2->j = projected_term_j - cross_term_j;
	real cosine_term_k = vector_k * local_1;
	real axis_term_k = arg_1->k * local_2;
	real cross_term_k = local_5 * local_0;
	real projected_term_k = cosine_term_k + axis_term_k;
	arg_2->k = projected_term_k - cross_term_k;
}

// @retail 0x293130
long __stdcall function_293130(long arg_0)
{
	s_293134 local_13;
	long flock_absolute_index = arg_0 & 0xffff;
	s_293130 *local_2 = &((s_293130 *)g_51ecb4->data)[flock_absolute_index];
	s_293133 *definition = (s_293133 *)g_4e0350;
	short source_index = local_2->field_2;
	s_293132 *local_3 = &definition->field_354[source_index];
	long local_0 = NONE;
	if (local_3->field_2c != NONE && local_3->field_c > 0 && local_2->field_10 > 0)
	{
		real local_5 = 0.0f;
		real total_weight = 0.0f;
		short local_6 = 0;
		short local_7;
		short local_17;
		short local_8 = NONE;
		for (local_7 = 0; local_7 < local_3->field_c; local_7++)
		{
			if (local_2->field_10 & (1 << local_7))
			{
				s_293131 *entry = &local_3->field_10[local_7];
				total_weight += entry->field_18;
				local_6++;
			}
		}
		if (total_weight > 0.0f)
		{
			real random_fraction = _random(&g_4e7408->unknown0, NULL, 0) * (1.0f / 65535.0f);
			real selection_weight = total_weight * random_fraction;
			for (local_7 = 0; local_7 < local_3->field_c; local_7++)
			{
				if (local_2->field_10 & (1 << local_7))
				{
					s_293131 *entry = &local_3->field_10[local_7];
					local_5 += entry->field_18;
					if (local_5 >= selection_weight)
					{
						local_8 = local_7;
						break;
					}
				}
			}
		}
		else if (local_6 > 0)
		{
			short local_9 = random_index(&g_4e7408->unknown0, local_6);
			short local_10 = 0;
			for (local_17 = 0; local_17 < local_3->field_c; local_17++)
			{
				if (local_2->field_10 & (1 << local_17))
				{
					if (local_9 == local_10)
					{
						local_8 = local_17;
						break;
					}
					local_10++;
				}
			}
		}
		if (local_8 != NONE)
		{
			s_293131 *local_11 = &local_3->field_10[local_8];
			long tag_absolute_index = local_3->field_2c & 0xffff;
			byte *tag = g_4e3b44[tag_absolute_index].bytes;
			dword tag_flags = *(dword *)(tag + 0xd4);
			bool local_12 = !((tag_flags >> 4) & 1);
			function_b7930(&local_13, local_3->field_2c, NONE, NULL);
			local_13.field_b4 = 1;
			local_13.field_b6 = 0x34;
			if (local_3->field_20 > 0.0f && local_3->field_24 > 0.0f)
			{
				real scale = function_259d0(&g_4e7408->unknown0, NULL, 0, local_3->field_20, local_3->field_24);
				local_13.field_58 = scale;
			}
			local_13.field_1c.x = function_259d0(&g_4e7408->unknown0, NULL, 0, -local_11->field_14, local_11->field_14) + local_11->field_0.x;
			local_13.field_1c.y = function_259d0(&g_4e7408->unknown0, NULL, 0, -local_11->field_14, local_11->field_14) + local_11->field_0.y;
			if (local_12)
				local_13.field_1c.z = local_11->field_0.z;
			else
				local_13.field_1c.z = function_259d0(&g_4e7408->unknown0, NULL, 0, -local_11->field_14, local_11->field_14) + local_11->field_0.z;
			real local_18 = local_11->field_c;
			local_13.field_28 = *g_4687a8;
			function_293133(local_18, g_4687b0, &local_13.field_28);
			local_0 = function_b7b40(&local_13);
			if (local_0 != NONE)
			{
				long object_absolute_index = local_0 & 0xffff;
				s_flock_object_header *header = &((s_flock_object_header *)g_4e0300->data)[object_absolute_index];
				s_flock_object *object = header->object;
				s_293135 *local_15 = (s_293135 *)flock_object_get_member(object);
				if (local_15)
				{
					local_15->field_0 = NONE;
					local_15->field_4 = NONE;
					local_15->field_12 = NONE;
					local_15->field_14 = NONE;
					local_15->field_c = false;
					local_15->field_d = false;
					local_15->field_e = true;
					local_15->field_10 = 0;
					s_295600_state *state = &local_15->field_18;
					state->duration = NONE;
					state->elapsed = 0;
					state->previous = *(vector3f *)g_468788;
					function_2936b0(arg_0, local_0);
					if (local_3->field_14 > 0)
					{
						short drawn_index = random_index(&g_4e7408->unknown0, (short)local_3->field_14);
						local_15->field_4 = drawn_index;
					}
				}
				else
				{
					function_b8540(local_0);
					local_0 = NONE;
				}
			}
		}
	}
	return local_0;
}
