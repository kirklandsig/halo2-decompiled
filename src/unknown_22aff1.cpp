// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>
#include <stddef.h>

struct s_widget_quad_2b11;
struct s_float_rect;
struct s_bitmap_widget_22ae21;
struct s_widget_transform_block;
struct s_510c4c;
extern s_510c4c *g_510c4c;
/* the interface draw inputs: the color at 0x30 and the three colors after it */
struct s_4e6950
{
	byte field_0[0x30];
	color4f color;
	color3f field_40, field_4c, field_58;
	byte field_64[0xc];
};
extern s_4e6950 g_4e6950;
typedef char check_22aff1_color_offset[offsetof(s_4e6950, color) == 0x30 ? 1 : -1];
typedef char check_22aff1_last_color_offset[offsetof(s_4e6950, field_58) == 0x58 ? 1 : -1];
long function_13a690(long);
void function_22ae21(long, s_bitmap_widget_22ae21 const *, long *, long *, long *, box2f *);
void function_1396c7(long, point2f *);
void function_22bcaf(s_widget_transform_block const *, point2f *, point2f *, point2f *);
double __stdcall function_1d6d0(real);
double __cdecl function_2b460(real);
void function_22a664(s_widget_quad_2b11 const *, s_float_rect const *, long, long, long);
color3f *unpack_color3f(dword, color3f *);
bool function_22acb4(long);
void function_1392c5(long);
inline void point2f_set(point2f *point, real x, real y);

PRIVATE __forceinline real function_22aff2(real arg_0)
{
	return arg_0 < 0.0f ? 0.0f : arg_0 > 1.0f ? 1.0f : arg_0;
}

struct s_22aff3
{
    point2f field_0;
    real field_8;
    point2f field_c[4];
    union
    {
        struct { byte field_2c[4]; point2f field_30; };
        color3f field_2c_1;
    };
    union { long field_38; real field_38_1; };
    point2f field_3c;
    long field_44;
    point2f field_48;
    union { long field_50; real field_50_1; dword const *field_50_2; };
    box2f field_54;
    byte field_64[3];
    bool field_67;
};

PRIVATE __forceinline real function_22aff5(real arg_0)
{
    real local_0 = arg_0;
    if (0.0f > arg_0)
        local_0 = 0.0f;
    else if (arg_0 > 1.0f)
        local_0 = 1.0f;
    return local_0;
}

#pragma intrinsic(memcpy)
// score 3 (was 3): Spacing-scalar-before-vertex loading remains; frame 0x88 and every home agree.
// Earlier: Repeated-translation scalar/vertex input-load order remains; frame homes, rounded offsets and spacing XMM roles agree.
// Earlier: repeated-draw spacing XMM roles and vertex-translation
// input-load scheduling remain; frame homes and rounded-offset memory operands agree.
// Earlier at 121: frame homes differ: retail keeps vertical_scale at +0x08
// (vertices at +0x0c), horizontal_scale in height's slot, the vertical offset in
// width's and the horizontal offset in arg_3's; ours swaps the two axes and the
// height/width homes and puts vertical_scale in arg_3's slot. Retail also reads both
// offsets from memory at every product where ours caches each in a register (4
// bytes short). Offsets as one point2f 121 (plain locals 146, arg_3 reference 141);
// declarations reordered (axes, height/width, vertical_scale before the vertices)
// 121 each; locals renamed 121; scales computed inside the loop 120, as a point2f
// 138; offset x before y 466; offset at loop or function scope 121; vertex pointer 121.
// Round 72: one (real) conversion after each ternary 154 (vertex sums go x87); offsets
// assigned in if/else 121; origin-first, offset-first sums 121; edge-offset helper
// inline/forceinline 509/121; plain scalars 146 and an inline transform taking the
// offsets by address 146 (both load the offsets into xmm as ours does); the vertex loop in
// a forceinline helper 146. Every SSE form loads the rounded offsets into registers.
// Native offset vector 121; destination cursor 423; vector plus cursor 423;
// vertical/horizontal/both full-width side predicates 447/138/450; bounded short
// vertex ordinal 431; native vector plus short ordinal 431. Original body retained.
// complete vertex value before array store 446; both completed coordinate scalars before stores 121;
// uniform named axis products at each row 121; uniform origin addition stages at each row 119; constant
// axis initialization at declaration 136; explicit offset branches with native point workspace 121; width
// used directly by repeated draw spacing 121; immutable palette with body cursor step 121.
// Uniform two-row horizontal/vertical product and pair-sum operand orders 121 each;
// complete input-color component publication 518.
// Matched selection/transform separate declaration states default/inline/noinline,
// all eight nondefault pairs 121 each; caller frame homes and rounded-offset memory/XMM
// residency remain. Original helper declarations and complete body retained.
// Entry validity, bitmap admission and completed-draw terminal guard graphs: eight forms
// all 121; original nested admission lifetimes restored.
// Round 78: point pointer stores/loads 450; read-only point pointer 121; indexed real offset 121;
// destination vertical staging 480; axis initialization order 122; with axis declarations 122;
// component order 120; plus early vertical-scale declaration 120. Original body restored.
// Complete transformed horizontal/vertical/both input values 473/470/489; native origin 499;
// horizontal+origin/vertical+origin/all transform inputs/all plus anchor 518/515/541/560.
// Input copies enlarge the frame and do not recover offset memory operands; original inputs restored.
// Round 315: Existing horizontal/vertical/both axis setters 116/119/68; vertex combinations
// 121/116/119/68 and all anchors 68. On the recovered memory-offset graph, independent rounded scalars
// 14; dimension/scale declaration combinations 68 or 14; rounded-point setter 143. Body-stepped
// palette 14; constant ordinal between conversions 12; spacing/translation commutations 12; completion
// before palette initialization 7; original arithmetic restored 7. Keep both existing axis setters,
// independent twice-used offsets, the constant ordinal lifetime and completion publication. Existing
// helper body and every marked call input unchanged.
// Existing centering/repeat/construction setters, subsets 1-7: 369/342/384/7/369/342/384; all with half-spacing 384.
// Producer-local real/integer repeat-count parts 3/7, isolated producer scopes 3/7; real part retained after width conversion.
// Half-spacing and repeated-add input commutations 7; early real-span variants 3, restored. Coordinate pointer/completed sum/fixed do loop 3 each; array cursor 74.
// Keep only the real repeat-count part at its original product; every arithmetic group, helper and marked call input remains original.
// Round 46: Two-stage spacing/vertex accumulation 3/3; repeated-translation unsigned-long/short/word/byte ordinals 4/74/74/74; immutable spacing/spacing role name 3/3. Original body restored.
// draws a bitmap widget: its quad from the anchored bounds, then the mode's palette, repeated or mirrored passes
// @retail 0x22aff1
void function_22aff1(long arg_0, byte const *arg_1, byte const *arg_2, real const *arg_3)
{
	long local_0 = function_13a690(*(word const *)(arg_2 + 0x1c));
	if (*(long const *)(arg_2 + 0x2c) != NONE && *(long const *)(arg_2 + 0x24) != NONE)
	{
		point2f horizontal_axis;
		point2f vertices[4];
		point2f vertical_axis;
		long height;
		point2f anchor;
		long bitmap_index;
		point2f origin;
		long width;
		box2f coordinates;
		bool drawn;
		function_22ae21(arg_0, (s_bitmap_widget_22ae21 const *)arg_2,
			&bitmap_index, &width, &height, &coordinates);
		if (bitmap_index != NONE)
		{
			drawn = false;
			function_1396c7(*(word const *)(arg_2 + 0x1c), &origin);
			point2f_set(&horizontal_axis, 1.0f, 0.0f);
			point2f_set(&vertical_axis, 0.0f, 1.0f);
			switch (local_0)
			{
			case 0:
				origin.x += *(short const *)(arg_2 + 0x34);
				origin.y += *(short const *)(arg_2 + 0x36);
				anchor.x = coordinates.x0 + (coordinates.x1 - coordinates.x0) * *(real const *)(arg_2 + 0x40);
				anchor.y = coordinates.y0 + (coordinates.y1 - coordinates.y0) * *(real const *)(arg_2 + 0x44);
				break;
			case 1:
				origin.x += *(short const *)(arg_2 + 0x38);
				origin.y += *(short const *)(arg_2 + 0x3a);
				anchor.x = coordinates.x0 + (coordinates.x1 - coordinates.x0) * *(real const *)(arg_2 + 0x48);
				anchor.y = coordinates.y0 + (coordinates.y1 - coordinates.y0) * *(real const *)(arg_2 + 0x4c);
				break;
			case 2:
				origin.x += *(short const *)(arg_2 + 0x3c);
				origin.y += *(short const *)(arg_2 + 0x3e);
				anchor.x = coordinates.x0 + (coordinates.x1 - coordinates.x0) * *(real const *)(arg_2 + 0x50);
				anchor.y = coordinates.y0 + (coordinates.y1 - coordinates.y0) * *(real const *)(arg_2 + 0x54);
				break;
			}
			g_4e6950.color = *(color4f const *)arg_3;
			function_22bcaf((s_widget_transform_block const *)(arg_2 + 0x58), &origin, &horizontal_axis, &vertical_axis);
			real vertical_scale = (real)height;
			long local_13 = 0;
			real horizontal_scale = (real)width;
			for (; local_13 < 4; ++local_13)
			{
				bool local_14 = local_13 > 1;
				bool local_15 = local_13 == 1 || local_13 == 2;
				real offset_y;
				real offset_x;
				offset_y = local_14 ? (real)function_1d6d0((coordinates.y1 - anchor.y) * vertical_scale) :
					(real)-function_2b460((anchor.y - coordinates.y0) * vertical_scale);
				offset_x = local_15 ? (real)function_1d6d0((coordinates.x1 - anchor.x) * horizontal_scale) :
					(real)-function_2b460((anchor.x - coordinates.x0) * horizontal_scale);
				vertices[local_13].x = horizontal_axis.x * offset_x + vertical_axis.x * offset_y + origin.x;
				vertices[local_13].y = horizontal_axis.y * offset_x + vertical_axis.y * offset_y + origin.y;
			}
			if (*(word const *)(arg_2 + 0x1e) & 1)
			{
				real local_18 = coordinates.x0;
				coordinates.x0 = coordinates.x1;
				coordinates.x1 = local_18;
			}
			if (*(word const *)(arg_2 + 0x1e) & 2)
			{
				real local_19 = coordinates.y0;
				coordinates.y0 = coordinates.y1;
				coordinates.y1 = local_19;
			}
			switch (*(word const *)(arg_2 + 0x60))
			{
			case 1:
				function_1392c5(*(long const *)(arg_1 + 0x188));
				g_4e6950.field_40 = *(color3f const *)(arg_1 + 0x18c);
				break;
			case 2:
				function_1392c5(*(long const *)(arg_1 + 0x1d4));
				g_4e6950.field_40 = *(color3f const *)(arg_1 + 0x1d8);
				break;
			case 3:
				g_4e6950.field_40 = *(color3f const *)(arg_1 + 0x18c);
				g_4e6950.field_4c = *(color3f const *)(arg_1 + 0x18c);
				break;
			case 4:
				g_4e6950.field_40 = *(color3f const *)(arg_1 + 0x1d8);
				g_4e6950.field_4c = *(color3f const *)(arg_1 + 0x1d8);
				break;
			case 5:
			{
				byte const *local_20 = (byte const *)g_510c4c + arg_0 * 0x6c;
				drawn = true;
				dword local_21[5] = {0, 0xff0000, 0xff00, 0xffff00, 0x7f00ff};
				dword const *palette = local_21;
				for (long local_22 = 0; local_22 <= 4; ++local_22)
				{
					bool local_47 = local_22 == 0;
					real local_23 = function_22aff2(*(real const *)(arg_1 + 0x2c) - local_22);
					real local_24 = function_22aff2(*(real const *)local_20 - local_22);
					bool local_25 = local_24 > local_23;
					real local_26 = local_25 ? *(real const *)(local_20 + 4) : -1.0f;
					real local_27 = local_26 < 0.0f ? 0.0f : function_22aff2(1.0f - local_26);
					color3f local_28;
					local_28 = *(color3f const *)g_468710;
					local_28.red *= local_27;
					local_28.green *= local_27;
					local_28.blue *= local_27;
					if (!local_25)
						local_24 = local_23;
					if (local_23 <= 0.0f && local_24 <= 0.0f)
						break;
					g_4e6950.color.alpha = local_24;
					g_4e6950.color.red = local_23;
					g_4e6950.field_40 = local_28;
					if (local_47)
					{
						if (function_22acb4(arg_0))
						{
							unpack_color3f(0x45059a, &g_4e6950.field_4c);
							unpack_color3f(0x9c46c1, &g_4e6950.field_58);
						}
						else
						{
							unpack_color3f(0x55aa, &g_4e6950.field_4c);
							unpack_color3f(0x78f0, &g_4e6950.field_58);
						}
					}
					else
					{
						unpack_color3f(*palette, &g_4e6950.field_4c);
						unpack_color3f(*palette, &g_4e6950.field_58);
					}
					function_22a664((s_widget_quad_2b11 const *)vertices, (s_float_rect const *)&coordinates,
						*(long const *)(arg_2 + 0x24), bitmap_index, *(long const *)(arg_2 + 0x2c));
					++palette;
				}
				break;
			}
			case 7:
			{
				drawn = true;
				long local_29 = *(long const *)(arg_1 + 0x22c);
				if (local_29 > 0)
				{
					real repeat_span = (real)(local_29 - 1);
					real local_31 = repeat_span * ((real)(width + 1) * 0.5f);
					for (long local_32 = 0; local_32 < 4; ++local_32)
						vertices[local_32].x -= local_31;
					g_4e6950.field_4c = *(color3f const *)g_468718;
					for (long local_33 = 0; local_33 < *(long const *)(arg_1 + 0x22c); ++local_33)
					{
						unpack_color3f(*(dword const *)(arg_1 + 0x230 + local_33 * 4), &g_4e6950.field_40);
						g_4e6950.color.alpha = *(real const *)(arg_1 + 0x250 + local_33 * 4);
						function_22a664((s_widget_quad_2b11 const *)vertices, (s_float_rect const *)&coordinates,
							*(long const *)(arg_2 + 0x24), bitmap_index, *(long const *)(arg_2 + 0x2c));
						for (long local_34 = 0; local_34 < 4; ++local_34)
							vertices[local_34].x += (real)(width + 1);
					}
				}
				break;
			}
			}
			if (*(word const *)(arg_2 + 0x1e) & 0x10)
			{
				real local_35 = vertices[1].x - vertices[0].x;
				real local_36 = vertices[2].y - vertices[1].y;
				for (long local_37 = 0; local_37 < 4; ++local_37)
				{
					bool local_46 = local_37 > 1;
					if (local_37 != 1 && local_37 != 2)
						vertices[local_37].x -= local_35 * 10.0f;
					if (!local_46)
						vertices[local_37].y -= local_36 * 10.0f;
				}
				coordinates.x0 -= (coordinates.x1 - coordinates.x0) * 10.0f;
				coordinates.y0 -= (coordinates.y1 - coordinates.y0) * 10.0f;
			}
			if (!drawn)
			{
				function_22a664((s_widget_quad_2b11 const *)vertices, (s_float_rect const *)&coordinates,
					*(long const *)(arg_2 + 0x24), bitmap_index, *(long const *)(arg_2 + 0x2c));
				real local_38 = vertices[1].x - vertices[0].x;
				real local_39 = vertices[2].y - vertices[1].y;
				point2f local_40[4];
				memcpy(local_40, vertices, sizeof(local_40));
				if (*(word const *)(arg_2 + 0x1e) & 4)
				{
					for (long local_41 = 0; local_41 < 4; ++local_41)
						if (local_41 != 1 && local_41 != 2)
							vertices[local_41].x += local_38 * 2.0f;
					function_22a664((s_widget_quad_2b11 const *)vertices, (s_float_rect const *)&coordinates,
						*(long const *)(arg_2 + 0x24), bitmap_index, *(long const *)(arg_2 + 0x2c));
				}
				memcpy(vertices, local_40, sizeof(local_40));
				if (*(word const *)(arg_2 + 0x1e) & 8)
				{
					for (long local_42 = 0; local_42 < 4; ++local_42)
						if (local_42 <= 1)
							vertices[local_42].y += local_39 * 2.0f;
					function_22a664((s_widget_quad_2b11 const *)vertices, (s_float_rect const *)&coordinates,
						*(long const *)(arg_2 + 0x24), bitmap_index, *(long const *)(arg_2 + 0x2c));
				}
				memcpy(vertices, local_40, sizeof(local_40));
				if ((*(word const *)(arg_2 + 0x1e) & 4) && (*(word const *)(arg_2 + 0x1e) & 8))
				{
					for (long local_43 = 0; local_43 < 4; ++local_43)
					{
						bool local_46 = local_43 > 1;
						if (local_43 != 1 && local_43 != 2)
							vertices[local_43].x += local_38 * 2.0f;
						if (!local_46)
							vertices[local_43].y += local_39 * 2.0f;
					}
					function_22a664((s_widget_quad_2b11 const *)vertices, (s_float_rect const *)&coordinates,
						*(long const *)(arg_2 + 0x24), bitmap_index, *(long const *)(arg_2 + 0x2c));
				}
			}
		}
	}
}
#pragma function(memcpy)
