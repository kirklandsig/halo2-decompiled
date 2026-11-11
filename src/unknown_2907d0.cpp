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

// score 19 (was 19): earlier start 21. Ours keeps the transform pointer in EBX and the point's y component in
// EBP, retail keeps them the other way round; retail also stores the last field with a
// direct word store where ours moves it through EAX. Tried: the last field through the
// plain output pointer 34, with field1c too 35; field2e as word 21; field2e stored before
// field1c 22, right after field10 34 (the constant 1 stays shared with the result); every
// store plain with arg_2 read directly 49 (arg_2 then arrives in ecx); local_5 not volatile
// 35; only field1c/field2e through local_5 35; the angles through an inlined helper taking
// real pointers 113; arg_2 read once through a volatile cast instead of local_4 35; the
// result in a local also stored to field2e 21.
// Round 99 (lane 16): the angle pair is the existing 0x11df30 (as matched 0x203780 fills the same
// record), with field14 a vector2f: 21 -> 19, and the record's flags member is no longer volatile.
// Left: the transform in EBX and the point's y in EBP (retail the other way) and the constant 1
// shared between the result and field2e. Without the two steering locals that form reads 33 (464
// bytes: the transform arrives in ECX; the stores then sit in retail's order on their own); seven
// parameter orders 33; field2e as word 33; local_1 declared first 33. The existing 0x142640 for the
// private rotate 312 (stays a call). With the angle function: no volatile pointer 113 (453 bytes);
// only field1c or only field2e through it 19; no address-of local 124; a byte result 19; a result
// local with field2e = true 19; field10 stored before the position 19.
// Round 69 (m3 lane 2, fast, plain-confirmed): the node lookup through the existing 0xb8c00
// (the count through its out-parameter, the returned pointer advanced past the bounds test)
// 19, the same code: kept, as the count/offset reads were that getter's body written out;
// on it the record no longer needs its volatile pointer (19, same code: removed). Also on it:
// no address-of local and no volatile 124 (447 bytes); the getter's pointer plus the node
// index before the bounds test 119; the getter's pointer in its own local 19. On the old
// base: local_1 declared first 19 (same code), the no-entity copies swapped 68, one exit with
// a `placed` flag 136; no steering locals re-measured 33 (the transform arrives in ECX and
// goes to EBX: in the stack first pass it must load into EBP to stay a stack argument).
// @retail 0x2907d0
bool function_2907d0(long arg_0, short arg_1, transform4x3f const *arg_2, s_actor_position_state *arg_3)
{
	transform4x3f const *const *local_4 = &arg_2;
	point3f local_0 = *g_468788;
	vector3f local_1;
	if (arg_0 != NONE)
	{
		long node_count;
		transform4x3f const *local_3 = function_b8c00(arg_0, &node_count);
		if (arg_1 < 0 || arg_1 >= node_count)
			return false;
		local_3 += arg_1;
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
	function_11df30(&arg_3->field14, &local_1);
	arg_3->field1c = 4;
	arg_3->field2e = 1;
	return true;
}
