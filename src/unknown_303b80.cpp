// @flags /O2 /GL-
// Havok (third-party physics library): two features (a point, a segment or a
// triangle each) cut down to the parts that meet: a point inside the other
// side's triangle is kept alone; otherwise the closest pair of edges is
// found and each triangle loses the corner off its closest edge.
#include <xmmintrin.h>

// the corner before each corner of a triangle, then the one after (from +2)
static const signed char g_4136b0[5] = {2, 0, 1, 2, 0};

// the closest point on the first segment, the offset from the closest point
// on the second, and the square of that offset's length (0x308590)
struct closest_points_308590
{
	__m128 first_point;
	__m128 offset;
	float distance_squared;
};

int __cdecl function_308590(__m128 const *first_start, __m128 const *first_direction, __m128 const *second_start, __m128 const *second_direction, closest_points_308590 *result);

struct triangle_303360
{
	__m128 vertices[3];
	__m128 field_30;
	__m128 field_40;
	__m128 field_50;
};

class class_303360
{
public:
	// the corners of each side's feature, and their count
	long count_a;
	long count_b;
	char unknown8[0x20 - 0x8];
	__m128 corners_a[3];
	char unknown50[0xa0 - 0x50];
	__m128 corners_b[3];
	__m128 field_d0;
	__m128 sources_b[3];

	int function_303360(__m128 const *point, triangle_303360 *triangle, int orient);
	void function_303b80();
};

// @retail 0x303b80
void class_303360::function_303b80()
{
	int i;
	if (count_b == 3)
	{
		for (i = 0; i < count_a; i++)
		{
			if (function_303360(&corners_a[i], (triangle_303360 *)corners_b, 0) == 7)
			{
				corners_a[0] = corners_a[i];
				count_a = 1;
				return;
			}
		}
	}
	if (count_a == 3)
	{
		for (i = 0; i < count_b; i++)
		{
			if (function_303360(&corners_b[i], (triangle_303360 *)corners_a, 0) == 7)
			{
				_mm_store_ps(sources_b[0].m128_f32, sources_b[i]);
				corners_b[0] = corners_b[i];
				count_b = 1;
				return;
			}
		}
	}
	int const edges_a = (count_a == 3) * 2 + 1;
	int const edges_b = (count_b == 3) * 2 + 1;
	float best = 3.40282347e+38f;
	int best_a = 0;
	int best_b = 0;
	for (int a = 0; a < edges_a; a++)
	{
		for (int b = 0; b < edges_b; b++)
		{
			__m128 const direction_a = _mm_sub_ps(corners_a[g_4136b0[a + 2]], corners_a[a]);
			__m128 const direction_b = _mm_sub_ps(corners_b[g_4136b0[b + 2]], corners_b[b]);
			closest_points_308590 closest;
			function_308590(&corners_a[a], &direction_a, &corners_b[b], &direction_b, &closest);
			if (closest.distance_squared < best)
			{
				best = closest.distance_squared;
				best_a = a;
				best_b = b;
			}
		}
	}
	if (edges_a == 3)
	{
		count_a--;
		corners_a[g_4136b0[best_a]] = corners_a[count_a];
	}
	if (edges_b == 3)
	{
		count_b--;
		int const replace_b = g_4136b0[best_b];
		sources_b[replace_b] = sources_b[count_b];
		corners_b[replace_b] = corners_b[count_b];
	}
}
