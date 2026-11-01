// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_131C20.CPP: colour interpolation, in rgb or hsv */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>

hsv3f *function_1318d0(const color3f *rgb, hsv3f *hsv);
color3f *function_131a00(const hsv3f *hsv, color3f *rgb);

enum
{
	_color_interpolation_hsv_bit = 0,
	_color_interpolation_long_way_bit
};

// @retail 0x131c20
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result)
{
	real one_minus_t = 1.0f - t;

	if (flags & (1 << _color_interpolation_hsv_bit))
	{
		hsv3f hsv_a;
		hsv3f hsv_b;
		hsv3f hsv;

		function_1318d0(a, &hsv_a);
		function_1318d0(b, &hsv_b);
		if ((fabs(hsv_a.hue - hsv_b.hue) > 0.5f) != ((flags >> _color_interpolation_long_way_bit) & 1))
		{
			if (hsv_b.hue > hsv_a.hue)
			{
				hsv_a.hue += 1.0f;
			}
			else
			{
				hsv_b.hue += 1.0f;
			}
		}
		hsv.hue = hsv_a.hue * one_minus_t + hsv_b.hue * t;
		if (hsv.hue > 1.0f)
		{
			hsv.hue -= 1.0f;
		}
		hsv.saturation = hsv_a.saturation * one_minus_t + hsv_b.saturation * t;
		hsv.value = hsv_a.value * one_minus_t + hsv_b.value * t;
		function_131a00(&hsv, result);
	}
	else
	{
		result->red = a->red * one_minus_t + b->red * t;
		result->green = a->green * one_minus_t + b->green * t;
		result->blue = a->blue * one_minus_t + b->blue * t;
	}

	return result;
}

__declspec(noinline) color3f *function_131d60(color4f const *a, color4f const *b, dword flags, color3f *result, color3f const *tint, real t);

// @retail 0x131d60
color3f *function_131d60(color4f const *a, color4f const *b, dword flags, color3f *result, color3f const *tint, real t)
{
	function_131c20((color3f const *)&a->red, (color3f const *)&b->red, flags, t, result);
	if (tint)
	{
		if (a->alpha > 0.0001f || b->alpha > 0.0001f)
		{
			real alpha = (1.0f - t) * a->alpha + b->alpha * t;
			real tint_weight = 1.0f - alpha;
			result->red = tint->red * tint_weight + result->red * alpha;
			result->green = tint->green * tint_weight + result->green * alpha;
			result->blue = tint->blue * tint_weight + result->blue * alpha;
		}
		else
		{
			result->red = result->red * tint->red;
			result->green = tint->green * result->green;
			result->blue = tint->blue * result->blue;
		}
	}
	return result;
}
