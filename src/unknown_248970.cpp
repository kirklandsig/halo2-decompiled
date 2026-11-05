#include "unknown_11c920.h"
#include "effects.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

#define MAX(a, b) ((a) > (b) ? (a) : (b))

real magnitude3d(vector3f const *vector);
struct s_247fd0;
struct s_object_2470e0;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_248970
{
	short field_0;
	byte field_2;
	byte field_3;
	long field_4;
	long field_8;
	long field_c;
	point3f field_10;
	real field_1c;
	byte field_20[0x34 - 0x20];
};

struct s_248971
{
	byte field_0[0x18];
	real field_18;
	real field_1c;
	real field_20;
	real field_24;
	real field_28;
	real field_2c;
	long field_30;
	byte *field_34;
};

class c_247470
{
public:
	short field_0;
	word field_2;
	long field_4;
	long field_8;
	real field_c;
	matrix3x3 field_10;
	point3f field_34;
	point3f field_40;
	void function_247470(real arg_0, s_particle_system_datum *arg_1, s_object_2470e0 *arg_2,
		s_247fd0 *arg_3, transform4x3f const *arg_4, dword arg_5);
};

/* updates a particle location: its position, its fade over the viewers, then its chain of emitters,
   allocating and linking any it lacks */
// @retail 0x248970
void function_248970(s_particle_location_datum *arg_0, bool arg_5, real arg_1, s_particle_system_datum *arg_2,
	real *arg_3, transform4x3f const *arg_4)
{
	s_248970 *local_20 = (s_248970 *)arg_0;
	long head = local_20->field_4;
	s_248971 *local_1 = (s_248971 *)arg_2->function_1751d0();
	long child_count = local_1->field_30;
	local_20->field_10 = arg_4->position;
	local_20->field_2 = arg_5;
	if (!arg_5)
	{
		local_20->field_1c = 0.f;
		for (long local_3 = 0; local_3 < *(short *)((byte *)g_4e8c20 + 8); ++local_3)
		{
			s_player_state const *local_6 = 0;
			if (local_3 != NONE && g_4686c4 != NONE)
				local_6 = (s_player_state const *)((byte const *)(g_4e9bd4 + local_3) + 0xb8);
			vector3f offset;
			vector3d_from_points3d(&local_6->position, &local_20->field_10, &offset);
			real local_10 = magnitude3d(&offset) * local_6->radius;
			real local_11 = 0.f;
			if (g_510c50 && ((byte *)g_510c50)[5])
				local_11 = 1.f;
			else if (local_10 > local_1->field_18 && local_1->field_24 > local_10)
			{
				if (local_1->field_1c + local_1->field_18 > local_10)
					local_11 = (local_10 - local_1->field_18) * local_1->field_20;
				else if (local_10 > local_1->field_24 - local_1->field_28)
					local_11 = (local_1->field_24 - local_10) * local_1->field_2c;
				else
					local_11 = 1.f;
			}
			local_20->field_1c = MAX(local_20->field_1c, MAX(local_11, TEST_FIELD_BIT(arg_2->flag10) ? 1.f : 0.f));
		}
	}
	else
		local_20->field_1c = 1.f;
	*(real *)((byte *)arg_3 + 0x1c) = local_20->field_1c;
	for (long local_14 = 0; local_14 < child_count; ++local_14)
	{
		s_record_pool *local_15 = g_51ec88;
		if (head == NONE)
		{
			long child_index = record_pool_allocate(local_15);
			if (child_index != NONE)
			{
				c_247470 *local_16 = DATUM(local_15, c_247470, child_index);
				local_16->field_8 = NONE;
				local_16->field_2 = 0;
				local_16->field_4 = NONE;
				local_16->field_c = 0.f;
			}
			head = child_index;
			if (head == NONE)
				break;
			c_247470 *local_17 = DATUM(local_15, c_247470, head);
			/* the salt is read ahead of the division, as 0x2483f0 does */
			long const &salt = local_17->field_0;
			long const &ordinal = local_17 - (c_247470 *)local_15->data;
			long local_18 = (salt << 16) | ordinal;
			local_17->field_8 = NONE;
			if (local_20->field_4 == NONE)
				local_20->field_4 = local_18;
			if (local_20->field_8 != NONE)
				DATUM(local_15, c_247470, local_20->field_8)->field_8 = local_18;
			local_20->field_8 = local_18;
		}
		c_247470 *local_19 = DATUM(local_15, c_247470, head);
		local_19->function_247470(arg_1, arg_2, (s_object_2470e0 *)(local_1->field_34 + local_14 * 0xb8), (s_247fd0 *)arg_3, arg_4, arg_2->color);
		head = local_19->field_8;
	}
}
