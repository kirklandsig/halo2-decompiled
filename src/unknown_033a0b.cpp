// @flags /O2 /arch:SSE /Gr
#include "view_context_storage.h"
/* UNKNOWN_033A0B.CPP: vector getters */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_33a0b_view
{
	byte unknown00[0x10];
	vector3f vector10;
	byte unknown1c[0x14];
	vector3f vector30;
	vector3f vector3c;
	vector3f vector48;
	vector3f vector54;
};

struct s_33a0b_default
{
	dword unknown0;
	vector3f vector;
};

struct s_dwords3 { dword a, b, c; };

inline PRIVATE void copy_vector(vector3f *dest, vector3f const *src)
{
	s_dwords3 *d = (s_dwords3 *)dest;
	s_dwords3 const *s = (s_dwords3 const *)src;
	d->a = s->a;
	d->b = s->b;
	d->c = s->c;
}

vector3f g_4859dc;
vector3f g_4859ec;
s_33a0b_view *g_485a58;
s_33a0b_default *g_4686d4;
real g_485a6c;
real g_485a70;

inline PRIVATE real pin_unit(real value)
{
	if (value < 0.0f)
	{
		value = 0.0f;
	}
	else if (value > 1.0f)
	{
		value = 1.0f;
	}
	return value;
}

color3f *unpack_color3f(dword pixel, color3f *color);
real function_33670(long selector);

struct s_4e6950
{
	byte unknown00[0x70];
	real user_values[4];
};
extern s_4e6950 g_4e6950;

// score 393 (was 393): retail reads g_485a6c and g_485a70 from memory for each component in cases 23, 35
// and 24, as if the stores through out could change them; ours keeps them in registers. A full test build
// in which another function in this file returned their addresses matched retail, which is consistent with
// different alias assumptions in the original program (the cause is unknown). That is not repeated here
// (docs/DECOMPILING.md). Writing the cases as a loop over the components scores 401.
// @retail 0x33980
void function_33980(long selector, vector3f *out)
{
	switch (selector)
	{
	case 0:
		out->i = g_485618.forward.i;
		out->j = g_485618.forward.j;
		out->k = g_485618.forward.k;
		break;
	case 3:
	case 4:
	case 21:
	case 22:
	{
		long index;
		switch (selector)
		{
		case 3: index = 0; break;
		case 4: index = 1; break;
		case 21: index = 2; break;
		case 22: index = 3; break;
		default: __assume(0);
		}
		byte *view = (byte *)g_485a58;
		if (view && index >= 0 && index < view[0x74])
			unpack_color3f(((dword *)(view + 0x64))[index], (color3f *)out);
		else *out = (*(vector3f *)g_468710);
		break;
	}
	case 5:
		copy_vector(out, &g_4859dc);
		break;
	case 6:
		copy_vector(out, &g_4859ec);
		break;
	case 14:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector3c;
		copy_vector(out, vector);
		break;
	}
	case 15:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector30;
		copy_vector(out, vector);
		break;
	}
	case 16:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector54;
		copy_vector(out, vector);
		break;
	}
	case 17:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector48;
		copy_vector(out, vector);
		break;
	}
	case 18:
		copy_vector(out, &g_485768);
		break;
	case 19:
		copy_vector(out, &g_4857ec);
		break;
	case 23:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector30;
		*out = *vector;
		out->i = pin_unit(out->i * g_485a6c);
		out->j = pin_unit(out->j * g_485a6c);
		out->k = pin_unit(out->k * g_485a6c);
		break;
	}
	case 35:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector30;
		*out = *vector;
		out->i = pin_unit(out->i * g_485a6c * 0.5f);
		out->j = pin_unit(out->j * g_485a6c * 0.5f);
		out->k = pin_unit(out->k * g_485a6c * 0.5f);
		break;
	}
	case 24:
	{
		vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector10;
		*out = *vector;
		out->i = pin_unit((g_485a70 + 1.0f) * out->i);
		out->j = pin_unit((g_485a70 + 1.0f) * out->j);
		out->k = pin_unit((g_485a70 + 1.0f) * out->k);
		break;
	}
	case 25:
		copy_vector(out, &g_485768);
		break;
	case 26:
		out->i = g_485768.i * g_485774;
		out->j = g_485768.j * g_485774;
		out->k = g_485768.k * g_485774;
		break;
	case 27:
		copy_vector(out, &g_485780);
		break;
	case 28:
		out->i = g_485780.i * g_48578c;
		out->j = g_485780.j * g_48578c;
		out->k = g_485780.k * g_48578c;
		break;
	case 29:
		copy_vector(out, &g_485798);
		break;
	case 30:
		out->i = g_485798.i * g_4857a4;
		out->j = g_485798.j * g_4857a4;
		out->k = g_485798.k * g_4857a4;
		break;
	case 31:
		copy_vector(out, &g_4857ec);
		break;
	case 32:
		out->i = g_4857ec.i * g_4857f8;
		out->j = g_4857ec.j * g_4857f8;
		out->k = g_4857ec.k * g_4857f8;
		break;
	case 33: copy_vector(out, (vector3f *)(g_4e6950.unknown00 + 0x00)); break;
	case 34: copy_vector(out, (vector3f *)(g_4e6950.unknown00 + 0x0c)); break;
	case 36: copy_vector(out, (vector3f *)(g_4e6950.unknown00 + 0x40)); break;
	case 37: copy_vector(out, (vector3f *)(g_4e6950.unknown00 + 0x4c)); break;
	case 38: copy_vector(out, (vector3f *)(g_4e6950.unknown00 + 0x58)); break;
	case 39: copy_vector(out, (vector3f *)(g_4e6950.unknown00 + 0x64)); break;
	case 40:
	{
		real value = function_33670(*(long *)g_485a58);
		out->i = value;
		out->j = value;
		out->k = value;
		break;
	}
	case 41:
		if (g_4b9f5c != NONE && g_4b9f84 > g_4b9f80)
		{
			real inverse = 1.0f / (g_4b9f84 - g_4b9f80);
			out->i = 0.0f - g_4b9dac.i * inverse;
			out->j = 0.0f - g_4b9dac.j * inverse;
			out->k = 0.0f - g_4b9dac.k * inverse;
		}
		else *out = g_4686d4->vector;
		break;
	default: __assume(0);
	}
}
