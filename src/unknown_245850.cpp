#include "unknown_11c920.h"
#include "unknown_0259d0.h"

// @flags /O2 /arch:SSE /Gr /Ob1

struct s_bsp3d;
struct s_source_245400;
struct s_collection_245270;
struct s_line_list;
short function_1ddda0(s_bsp3d const *arg_0, long arg_1, point3f *arg_2);
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
void function_245270(real thickness, short count, point3f const *points, plane3f const *plane,
	real height, long field_00, long field_04, long field_08, byte field_0c,
	byte field_0d, short field_0e, s_collection_245270 *collection);
void function_245400(s_source_245400 const *source, long index,
	transform4x3f const *matrix, real height, real radius, long field_00,
	long field_04, s_line_list *list);
void function_245540(s_source_245400 const *arg_1, long arg_0,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7);

struct s_245850
{
	short field_00;
	short field_02;
	byte field_04;
	byte field_05;
	short field_06;
};

#pragma inline_depth(0)
// score 14 (was 14): the plane index is loaded with a 16 bit move where retail sign extends it
// (every retail caller of 0x14a300 does `movsx edx`), and two float sums emit their products in
// a different order: point->z (retail fk*x + uk*z first) and local_3.k (retail lk*j + fk*i first).
// Tried: all 12 orders/groupings of local_3.k (14 or 20); point->z grouped `a + (b + c)` (14-16),
// left to right (98: x is multiplied in place, one instruction fewer); a `long` plane index in
// bsp3d_get_plane and its five declarations (116 here, 573 bytes; 0x14a300 still matches,
// 0x10cf80 249 -> 247, 0x1fa3a0 80 -> 79). Round 15 (exact LTCG harness, ~700 forms): VC
// reassociates each sum by the definition order of its values, so text order and grouping
// of one row change nothing; the plane values defined k, i, j give retail's three normal rows
// but move the d sum (23; best d form 19); the point values defined y, z, x (or z first) give
// retail's point->z row but move the loads (17-18); all three point rows ungrouped give
// point->z but not x (21, real 26); point copy 139; scale local 126; named transformed
// normal 59. The pragma removed lets LTCG inline 0x14a300 (160).
// Round 28: every 0x14a300 call site loads the index with `mov dx` where retail sign-extends, so
// its index is a long in retail (0x14a300 still matches with one); a long index (harness, all
// declarations) gives the movsx but rotates the point registers 107; with it a flat x row 13, the
// z row in 120 shapes 13, the k row in 12 shapes 13, k,i,j plane values 22 (best d sum 18), other
// row orders 31, z,y,x point values 17. Real builds: flat x row 114 (575 bytes); z,y,x point
// values with the y row parenthesised left to right 14; that y row alone 18; k,i,j plane values
// 23; compound `*=` scale 14. Explicit parentheses change the code (fully parenthesised x rows
// 103+); all 264 spellings of the z row 14-96, the 18 of the k row 14-20, and the 264 of the x
// row with z,y,x point values and that y row 14-18.
// @retail 0x245850
void function_245850(long arg_0, s_source_245400 const *arg_1,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7)
{
	s_245850 const *local_0 = &(*(s_245850 const *const *)((byte const *)arg_1 + 0x2c))[arg_0];
	point3f local_1[8];
	short local_2 = function_1ddda0((s_bsp3d const *)arg_1, arg_0, local_1);
	plane3f local_3;
	bsp3d_get_plane((s_bsp3d const *)arg_1, local_0->field_00, &local_3);
	if (arg_2)
	{
		for (short local_4 = 0; local_4 < local_2; ++local_4)
		{
			point3f *point = &local_1[local_4];
			real local_5 = point->x;
			real local_6 = point->y;
			real local_7 = point->z;
			if (arg_2->scale != 1.f)
			{
				local_5 = arg_2->scale * local_5;
				local_6 = arg_2->scale * local_6;
				local_7 = arg_2->scale * local_7;
			}
			point->x = local_5 * arg_2->forward.i + (arg_2->up.i * local_7 + arg_2->left.i * local_6) + arg_2->position.x;
			point->y = arg_2->forward.j * local_5 + arg_2->up.j * local_7 + arg_2->left.j * local_6 + arg_2->position.y;
			point->z = arg_2->left.k * local_6 + (arg_2->forward.k * local_5 + arg_2->up.k * local_7) + arg_2->position.z;
		}
		real local_6 = local_3.j;
		real local_7 = local_3.k;
		real local_5 = local_3.i;
		local_3.i = arg_2->left.i * local_6 + (arg_2->forward.i * local_5 + arg_2->up.i * local_7);
		local_3.j = arg_2->left.j * local_6 + (arg_2->forward.j * local_5 + arg_2->up.j * local_7);
		local_3.k = arg_2->up.k * local_7 + (arg_2->forward.k * local_5 + arg_2->left.k * local_6);
		local_3.d = arg_2->position.x * local_3.i + (arg_2->position.z * local_3.k + arg_2->position.y * local_3.j) + arg_2->scale * local_3.d;
	}
	function_245270(arg_4, local_2, local_1, &local_3, arg_3, arg_5, arg_6, arg_0,
		local_0->field_04, local_0->field_05, local_0->field_06, arg_7);
}
#pragma inline_depth(255)

struct s_245aa0
{
	long field_00;
	long field_04[256];
	long field_404;
	long field_408[256];
	long field_808;
	long field_80c[256];
};

// @retail 0x245aa0
void function_245aa0(s_245aa0 const *arg_0, s_source_245400 const *arg_1,
	transform4x3f const *arg_2, real arg_3, real arg_4,
	long arg_5, long arg_6, s_collection_245270 *arg_7)
{
	for (long local_0 = 0; local_0 < arg_0->field_808; ++local_0)
		function_245400(arg_1, arg_0->field_80c[local_0], arg_2, arg_3, arg_4, arg_5, arg_6, (s_line_list *)arg_7);
	for (long local_1 = 0; local_1 < arg_0->field_404; ++local_1)
		function_245540(arg_1, arg_0->field_408[local_1], arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
	for (long local_2 = 0; local_2 < arg_0->field_00; ++local_2)
		function_245850(arg_0->field_04[local_2], arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7);
}
