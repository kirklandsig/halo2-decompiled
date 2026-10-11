#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1428b0.h"
#include "object_queries.h"
#include "slot_handler.h"
#include <math.h>
#include <new>
// @flags /O2 /arch:SSE /Gr

struct s_actor_position_state
{
	s_actor_position_state();
	volatile long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real field14;
	real field18;
	long field1c;
	short field20;
	short field22;
	short field24;
	byte unknown26[2];
	short field28;
	short field2a;
	short field2c;
	short field2e;
	long field30;
	long field34;
	real field38;
	short field3c;
	short field3e;
	byte field40;
	byte unknown41[0x60 - 0x41];
	short field60;
};

__forceinline void function_290852(transform4x3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	transform4x3f const volatile *local_3 = arg_0;
	real local_0 = arg_1->i;
	real local_1 = arg_1->j;
	real local_2 = arg_1->k;
	if (arg_0->scale != 1.0f)
	{
		local_0 = arg_0->scale * local_0;
		local_1 = local_3->scale * local_1;
		local_2 = local_3->scale * local_2;
	}
	arg_2->i = arg_0->up.i * local_2 + arg_0->left.i * local_1 + arg_0->forward.i * local_0;
	arg_2->j = arg_0->up.j * local_2 + arg_0->left.j * local_1 + arg_0->forward.j * local_0;
	arg_2->k = arg_0->up.k * local_2 + arg_0->left.k * local_1 + arg_0->forward.k * local_0;
}

// @retail 0x2907d0
bool function_2907d0(long arg_0, short arg_1, transform4x3f const *arg_2, s_actor_position_state *arg_3)
{
	transform4x3f const *const *local_4 = &arg_2;
	s_actor_position_state volatile *local_5 = arg_3;
	point3f local_0 = *g_468788;
	vector3f local_1;
	if (arg_0 != NONE)
	{
		byte *local_2 = (byte *)object_get(arg_0);
		if (arg_1 < 0 || arg_1 >= (long)((unsigned long)*(short *)(local_2 + 0x114) / sizeof(transform4x3f)))
			return false;
		transform4x3f const *local_3 = (transform4x3f *)(local_2 + *(short *)(local_2 + 0x116)) + arg_1;
		transform4x3f_apply_point(local_3, &(*local_4)->position, &local_0);
		function_290852(local_3, &(*local_4)->forward, &local_1);
	}
	else
	{
		local_0 = (*local_4)->position;
		local_1 = (*local_4)->forward;
	}
	arg_3->s_actor_position_state::s_actor_position_state();
	arg_3->position = local_0;
	arg_3->field10 = NONE;
	local_5->field14 = (real)atan2(local_1.j, local_1.i);
	local_5->field18 = (real)atan2(local_1.k, sqrt(local_1.j * local_1.j + local_1.i * local_1.i));
	local_5->field1c = 4;
	local_5->field2e = 1;
	return true;
}
