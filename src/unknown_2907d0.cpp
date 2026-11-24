#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1428b0.h"
#include "object_queries.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include <math.h>
#include <new>
// @flags /O2 /arch:SSE /Gr

struct s_actor_position_state
{
	s_actor_position_state();
	long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	vector2f field14;
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
	real local_0 = arg_1->i;
	real local_1 = arg_1->j;
	real local_2 = arg_1->k;
	if (arg_0->scale != 1.0f)
	{
		local_0 = arg_0->scale * local_0;
		local_1 = arg_0->scale * local_1;
		local_2 = arg_0->scale * local_2;
	}
	arg_2->i = arg_0->up.i * local_2 + arg_0->left.i * local_1 + arg_0->forward.i * local_0;
	arg_2->j = arg_0->up.j * local_2 + arg_0->left.j * local_1 + arg_0->forward.j * local_0;
	arg_2->k = arg_0->up.k * local_2 + arg_0->left.k * local_1 + arg_0->forward.k * local_0;
}

vector2f *function_11df30(vector2f *angles, vector3f const *v);
transform4x3f *function_b8c00(long object_index, long *node_count);

/* Where an actor stands and faces: a node of the object moved by the given transform, or the
   transform itself when there is no object. A node index outside the object's nodes fails. */
// @retail 0x2907d0
bool function_2907d0(long arg_0, short arg_1, transform4x3f const *arg_2, s_actor_position_state *arg_3)
{
	bool result = true;
	point3f local_0 = *g_468788;
	vector3f local_1;
	if (arg_0 != NONE)
	{
		long node_count;
		transform4x3f const *local_3 = function_b8c00(arg_0, &node_count);
		if (arg_1 < 0 || arg_1 >= node_count)
		{
			result = false;
		}
		else
		{
			local_3 += arg_1;
			transform4x3f_apply_point(local_3, &arg_2->position, &local_0);
			function_290852(local_3, &arg_2->forward, &local_1);
		}
	}
	else
	{
		local_0 = arg_2->position;
		local_1 = arg_2->forward;
	}
	if (result)
	{
		arg_3->s_actor_position_state::s_actor_position_state();
		arg_3->position = local_0;
		arg_3->field10 = NONE;
		function_11df30(&arg_3->field14, &local_1);
		arg_3->field1c = 4;
		arg_3->field2e = 1;
	}
	return result;
}
