// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <wchar.h>
struct short_rectangle2d;
void function_1fa50(short_rectangle2d const *bounds, word const *text, void const *clip, void const *position, long line_gap, real scale, long color, void const *shadow);

struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

struct s_33a0b_default;
struct short_rectangle2d;
extern s_33a0b_default *g_4686d4;
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)

class c_1fa50
{
public:
	void function_1fb10(short_rectangle2d const *, real) const;
};

void function_13edb0(long, long, long, dword, color4f const *, color4f const *);
wchar_t const *loading_text_get(void);
wchar_t *ustrnzcatf(wchar_t *, wchar_t const *, ...);

// @retail 0x2233a0
void __stdcall function_2233a0(real const *arg_0)
{
	real const *const *local_5 = &arg_0;
	long local_4 = (long)(**local_5 * 100.0f);
	color4f local_0 = *g_4686cc;
	color4f local_1 = *(color4f const *)g_4686d4;
	wchar_t local_2[64];
	c_1fa50 const *local_6 = (c_1fa50 const *)local_2;
	short_rect local_3;
	(void)&arg_0;
	local_0.red = 1.0f;
	local_0.green = 1.0f;
	local_0.blue = 1.0f;
	local_1.red = 0.01568627543747425f;
	local_1.green = 0.0235294122248888f;
	local_2[0] = 0;
	local_1.blue = 0.03921568766236305f;
	function_13edb0(3, NONE, 2, 0, &local_0, &local_1);
	g_4e73a0.tab_stop_count = 0;
	wchar_t const *local_7 = loading_text_get();
	wcsncpy((wchar_t *)local_6, local_7, 62);
	local_2[62] = 0;
	ustrnzcatf((wchar_t *)local_6, L" %d%%", local_4);
	local_3 = g_485a8a.b;
	local_3.v0 += 200;
	function_1fa50(
		(short_rectangle2d const *)&local_3, (word const *)local_6, 0, 0, 0, 1.0f, 0, 0);
}
