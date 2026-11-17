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
			byte *state = (byte *)function_16f3a0(g_4b9ed8);
			point3f feet;
			feet.x = (0.0f - *(real *)(state + 0x48)) * g_4b9db8.i + eye.x;
			feet.y = (0.0f - *(real *)(state + 0x48)) * g_4b9db8.j + eye.y;
			real const &height = 0.0f - *(real *)(state + 0x48);
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
