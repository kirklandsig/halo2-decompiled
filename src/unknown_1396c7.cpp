// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_1396C7.CPP: a hud anchor's screen point: a corner or the centre
   of the view's bounds, or a projected world point */

#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "camera_storage.h"
#include "unknown_030290.h"
#include "globals.h"

struct s_510c4c;
extern s_510c4c *g_510c4c;

/* the parts of the new hud's game state (g_510c4c) read here */
struct s_1396c7_hud_view
{
	byte unknown000[0x1bc];
	byte *anchor;
};

/* the anchor record: two flags, then a world point at +0xb4 */
struct s_1396c7_anchor
{
	byte unknown00[0xad];
	bool flag_ad;
	bool flag_ae;
	byte unknown0af[0xb4 - 0xaf];
	point3f position;
};

point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);
real function_30bf0(vector3f *v);
bool function_30710(s_camera const *camera, short_rectangle2d const *rect, vector3f const *v, point2f *out, s_view const *view);
double __cdecl function_2b460(real value);
bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);
long function_155760(long index);
s_player_state *function_16f3a0(long index);

// score 28 (was 28): in case 4 retail multiplies a copy of the negated height by memory for x and y
// (`movaps; mulss [g]`) but loads the z component and multiplies by the height for z; the named
// height gives memory-first for all three (two bytes longer). Tried: the field through a reference
// and `(0.0f - height)` at each use 39; a state pointer and the negated read at each use 57 (x/y
// right, z multiplied in place); that with a named height for z only 39 (CSE folds it); z first 38;
// a displacement vector 46; the state pointer first 67; eye sums forward-first, feet declared
// first, z written temp-first 57; the height as a unary minus 28; the feet set through a
// point-along-vector helper 28; y row before x 37; the height before the eye sums 38; every
// product written memory-first 28. Retail multiplies a register by a writable global only
// here and at 0x224fd0 (no matched model); compile-only screens give x/y right only when
// the negation is an expression at each use, which then multiplies z in place.
// Round 31: the named height plus the negated read written at the x and y uses 101 (one
// subtraction, all three products copy the height, z in place; compile-only keeps z right
// there but not in the real build); the named height defined after y for z only 101; a const
// pointer to the camera storage for every camera read 66; the forward vector copied to a local
// 142 (copied to the stack); feet seeded with the eye sums and updated with `+=` 66.
// Round 69: shared vector3f_add_scaled with separate / in-place output 28/28; expression x/y
// with z read through the height's address 57, through the existing point setter 57; the union's
// array view 28; in-place eye construction 28; immutable height 28; split negation 28.
// @retail 0x1396c7
void function_1396c7(long a, point2f *point)
{
	switch (a)
	{
	case 0:
	{
		real y = (real)g_4b9dd8.top;
		point->x = (real)g_4b9dd8.right;
		point->y = y;
		break;
	}
	case 1:
	{
		real y = (real)g_4b9dd8.top;
		point->x = (real)g_4b9dd8.left;
		point->y = y;
		break;
	}
	case 2:
	{
		real y = (real)g_4b9dd8.bottom;
		point->x = (real)g_4b9dd8.left;
		point->y = y;
		break;
	}
	case 3:
	{
		real y = (real)g_4b9dd8.bottom;
		point->x = (real)g_4b9dd8.right;
		point->y = y;
		break;
	}
	case 4:
	{
		long unit_index;
		if (function_14ddc0(g_4b9ed8))
			unit_index = *(long *)(g_4e8c24->data + (function_14de70(g_4b9ed8) & 0xffff) * 0x21c + 0x2c);
		else
			unit_index = NONE;
		point->x = (real)((g_4b9dd8.left + g_4b9dd8.right) / 2);
		point->y = (real)((g_4b9dd8.top + g_4b9dd8.bottom) / 2);
		if (unit_index != NONE && !function_155760(g_4b9ed8))
		{
			point3f eye;
			eye.x = g_4b9da0.x + g_4b9dac.i;
			eye.y = g_4b9da0.y + g_4b9dac.j;
			eye.z = g_4b9da0.z + g_4b9dac.k;
			real height = 0.0f - *(real *)((byte *)function_16f3a0(g_4b9ed8) + 0x48);
			point3f feet;
			feet.x = height * g_4b9db8.i + eye.x;
			feet.y = height * g_4b9db8.j + eye.y;
			feet.z = g_4b9db8.k * height + eye.z;
			point3f transformed;
			transform4x3f_apply_point((transform4x3f const *)&g_4b9e14, &feet, &transformed);
			point2f screen;
			if (function_30710(&g_4b9e14, 0, (vector3f const *)&transformed, &screen, (s_view const *)&g_camera_storage))
				point->y = screen.y;
		}
		point->x = (real)function_2b460(point->x + 0.5f);
		point->y = (real)function_2b460(point->y + 0.5f);
		break;
	}
	default:
	{
		s_1396c7_anchor *anchor = (s_1396c7_anchor *)((s_1396c7_hud_view *)g_510c4c)->anchor;
		if (anchor->flag_ad || anchor->flag_ae)
		{
			point3f transformed;
			transform4x3f_apply_point((transform4x3f const *)&g_4b9e14, &anchor->position, &transformed);
			function_30bf0((vector3f *)&transformed);
			point2f screen;
			if (function_30710(&g_4b9e14, 0, (vector3f const *)&transformed, &screen, (s_view const *)&g_camera_storage))
			{
				point->x = (real)function_2b460(screen.x + 0.5f);
				point->y = (real)function_2b460(screen.y + 0.5f);
				break;
			}
		}
		point->x = 10000.0f;
		point->y = 10000.0f;
		break;
	}
	}
}
