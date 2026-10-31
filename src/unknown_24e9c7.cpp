// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "view_context_storage.h"

struct s_float_rect;
struct s_widget_quad_2b11;
void function_22a664(s_widget_quad_2b11 const *arg_0, s_float_rect const *arg_1,
	long arg_2, long arg_3, long arg_4);
extern real g_4e69c0[4];


struct s_4e6950
{
	color3f field_0;
	point3f field_c;
	real value18;
	real value1c;
	real value20;
	real value24;
	byte unknown28[0x70 - 0x28];
	real user_values[4];
};
extern s_4e6950 g_4e6950;

/* the hud globals' marker kinds (+0x408) */
struct s_24e9c7_kind
{
	long unknown00;
	long bitmap_index;
	long unknown08;
	long shader_index;
	short sequence_index;
	byte unknown12[0x24 - 0x12];
};

struct s_24e9c7_kinds_view
{
	byte unknown00[0x408];
	long count;
	s_24e9c7_kind *entries;
};

struct s_24e9c7_sprite
{
	byte unknown00[8];
	real bounds[4];
};

struct s_24e9c7_sequence
{
	byte unknown00[0x20];
	short first_bitmap_index;
	byte unknown22[0x34 - 0x22];
	long sprite_count;
	s_24e9c7_sprite *sprites;
};

struct s_24e9c7_bitmap
{
	byte unknown00[4];
	short width;
	short height;
	byte unknown08[0x74 - 8];
};

struct s_24e9c7_bitmap_group
{
	byte unknown00[0x3c];
	long sequence_count;
	s_24e9c7_sequence *sequences;
	long bitmap_count;
	s_24e9c7_bitmap *bitmaps;
};

struct s_24e9c7_marker
{
	bool use_full_alpha;
	byte unknown01[3];
	long kind;
	byte unknown08[0x24 - 8];
	color3f color;
	point3f point;
	real alpha;
	real value40;
};

struct s_24e9c7_placement
{
	real forward_i;
	real forward_j;
	real x;
	real y;
	real size;
	bool hidden;
	byte unknown15[3];
	real fade;
	real unknown1c;
	real scale;
};

struct s_24e9c7_point
{
	real x, y;
};

struct s_24e9c7_points
{
	s_24e9c7_point n[4];
};

struct s_24e9c7_bounds
{
	real x0, x1, y0, y1;
};

/* Draws one marker's sprite at its placement; returns the sprite's height. */
// score 125 (was 125): the frame size, the bounds stores and the marker copy agree; what is
// left is the result, kind and bitmap homes eight bytes above retail's, the 1.0 constant in
// xmm4 where retail uses xmm5, and the geometric SSE schedule — no source form found this round.
// score 125 (was 125): reassociating the corner sums and splitting them into two steps both
// canonicalise back to retail's association, so what is left is the SSE schedule itself, the
// eight-byte-higher stack homes and the sprite pointer staging; no source form found.
// score 125 (was 125): result/kind/bitmap homes sit 8 bytes above retail's (retail keeps a spill at -8
// and an unused slot at -0xc above them), the 1.0 constant is in xmm4 where retail uses xmm5, and the
// geometric SSE schedule differs. Tried: table read at each use 125 plain (743/743), real half width
// 135f, direct height 135f, sprite pointer 133f, global optimization off 716f. Truncation helper at
// height 194, width 167, both 198; side before upper 139; both sides before upper 138; direct alpha
// stores 125; const metadata 125; native sprite rectangle 125. Round 280: plain quad passes 198; faded
// alpha first 125; lookup scope ended 125; shared bounds value 125; x then y coordinates 130. Upper
// components 125; side components 125; reverse components 133; all components 133; earlier upper
// displacement 123 plain (two points, dropped), with components 123; direct reverse uses 167. shader
// snapshot 174; sequence count snapshot 160; sequence field at use 125; unscaled height/width/both 125;
// shared forward values 161; external bitmap reference 125. upper/side/reverse/all value initialization
// 125; origin/sequence references 125; speed policy 279; frame omission 125. Native input
// direction/center/both 125/125/125; direct quad array 125; span locals 125; early basis / scale /
// basis with spans 125/125/125 (fast). Basis i/j/both product input orders 125; upper or side inputs
// 125; basis with upper / side / both input orders 125. Point origin library copies 252; native
// bitmap/placement/sequence values 215/176/205; bitmap plus sequence 230; placement plus copies 176;
// opacity snapshot 139; placement plus bitmap 183. Origin / upper-copy / upper / positive-side passes
// 188/220/240/204; origin-upper 195; origin-side 206; upper-side 214; lower-copy pass 193. Scalar
// origin x/y/both 194/194/194; scaled height/width/both accumulation 125/125/125; side-product updates
// 125; y capture with side updates 194. upper / negative-side / positive-side addition inputs
// 125/125/125; upper-negative / upper-positive / both sides / all additions 125/125/125/125; all
// additions with constant-first size 125. selected bitmap word 153; bitmap after bounds 125; word
// bitmap/long sequence 169; late word bitmap 153; shared opacity tail 142; tail with word/late bitmap
// 161/142; branch-set tail 142. external group/live count/live shader 125/125/125; live scale/group
// with scale 141/141; count with shader/size accumulator/across component lifetimes 125/125/138. Round
// 132: across as a point struct 144; 0 - forward_j 138; size inline 143; first_bitmap declared before
// bounds 125; result assigned after its declaration 125.
// Entry declarations with in-place assignments: across carriers 125; metadata carriers 125;
// both pairs 125; bounds workspace 125; three displacement workspaces 125; all geometry
// workspaces 125; reversed geometry declarations 125; coupled scalar role names 125.
// Round 193: complete per-use kind/bitmap/group observations 207/182/205;
// kind+bitmap/kind+group/bitmap+group/all 225/223/201/225;
// all with per-use sequence field 326. All original observations restored.
// Round 303: early outer/texture/selected-bitmap admissions 128/127/149;
// outer+texture/outer+bitmap/texture+bitmap/all 130/149/149/183;
// all with returned height declared after admission 183. Original scopes and body restored.
// Round 41 (fast): retail computes size * (0 - forward_j) and size * forward_i before the height
// block, spills the second at -8 and reuses forward_i for the upper vector; ours folds the
// single-use across terms into the side products (computed late, forward_i read twice). Side
// vector scaled in place by the half width 144, with the half width first 144, negated forward
// copied then scaled in place 139 (gives retail's 0 - forward_j); named perpendicular components
// both/one 138/138. Original body restored.
// Round 46: retail computes the across pair right after size as size * (0 - forward_j) and spills
// its j part at -8, the same home later used for the scaled side j (ours sinks both products to the
// side and negates after multiplying). Side vector built then scaled in place 144; perpendicular
// then in-place size scale 139; scale field at each use 141; negated operand first 125; marker
// global stores before the geometry 149. Original body restored.
// Round 313: Shared real half-width, with direct/named full pixel width: current basis 125/125, early
// upper 123/123, deferred basis 129/129, both lifetimes 128/128. Two-count gain rejected; original
// geometry and body restored; early spill and scalar homes remain.
// Round 48: VC rewrites `size * -forward_j` as `-(forward_j * size)` and sinks it (ours); `0.0f -
// forward_j` or a negated-component local keeps retail's early `size * (0 - forward_j)`, yet the
// pair's home stays below result: across built in the side vector and scaled in place by the half
// width, with `0.0f -` 139; with unary minus 144; `0.0f -` alone (a listed form, as a base) 138;
// negated-component locals scaled by size 138; scale read before size with `0.0f -` 137.
// Round 322: Shared upper-origin point copy/value/twice-used coordinates/first vertex at assembly 168/147/190/147; at the height producer 153/146/189/147. Original corner sums, geometry and body restored; early across spill and shifted scalar homes remain.
// Round 454: Complete center point on the live-table entry graph, native copy at corner
// assembly 125, with all corner origins initialized first 129; library move 240/240.
// Basis-boundary counterparts 125/129/195/196. Original point reads, copies and geometry
// restored; the early across spill and shifted scalar homes remain.
// @retail 0x24e9c7
real function_24e9c7(s_24e9c7_marker const *marker, s_24e9c7_placement const *placement)
{
	real result = 0.0f;
	if (g_510c94 && marker->kind >= 0 && marker->kind < ((s_24e9c7_kinds_view *)g_510c94)->count)
	{
		s_24e9c7_kind *kind = &((s_24e9c7_kinds_view *)g_510c94)->entries[marker->kind];
		long bitmap_index = kind->bitmap_index;
		if (bitmap_index != NONE && kind->shader_index != NONE)
		{
			s_24e9c7_bitmap_group *group = (s_24e9c7_bitmap_group *)g_4e3b44[bitmap_index & 0xffff].bytes;
			short sequence_index = kind->sequence_index;
			s_24e9c7_bounds bounds;
			long first_bitmap = NONE;
			bounds.x0 = 0.0f;
			bounds.x1 = 1.0f;
			bounds.y0 = 0.0f;
			bounds.y1 = 1.0f;
			if (sequence_index >= 0 && sequence_index < group->sequence_count)
			{
				s_24e9c7_sequence *sequence = &group->sequences[sequence_index];
				first_bitmap = sequence->first_bitmap_index;
				if (sequence->sprite_count)
					bounds = *(s_24e9c7_bounds *)sequence->sprites->bounds;
			}
			else if (sequence_index == 0 && group->sequence_count == 0 && group->bitmap_count > 0)
			{
				first_bitmap = 0;
			}
			if (first_bitmap != NONE)
			{
				s_24e9c7_bitmap *bitmap = &group->bitmaps[first_bitmap];
				real size = placement->size + 1.0f;
				real across_i = size * -placement->forward_j;
				real across_j = size * placement->forward_i;
				real scale = placement->scale;
				long height = (long)((bounds.y1 - bounds.y0) * bitmap->height * scale);
				result = (real)height;
				long half_width = (long)((bounds.x1 - bounds.x0) * bitmap->width * scale) / 2;
				s_24e9c7_point up;
				up.x = -(result * placement->forward_i);
				up.y = -(placement->forward_j * result);
				s_24e9c7_point side;
				side.x = (real)half_width * across_i;
				side.y = (real)half_width * across_j;
				s_24e9c7_point back;
				back.x = -side.x;
				back.y = -side.y;
				s_24e9c7_points points;
				points.n[0] = *(s_24e9c7_point const *)&placement->x;
				points.n[0].x += up.x;
				points.n[0].y += up.y;
				points.n[0].x += back.x;
				points.n[0].y += back.y;
				points.n[1] = *(s_24e9c7_point const *)&placement->x;
				points.n[1].x += up.x;
				points.n[1].y += up.y;
				points.n[1].x += side.x;
				points.n[1].y += side.y;
				points.n[2] = *(s_24e9c7_point const *)&placement->x;
				points.n[2].x += side.x;
				points.n[2].y += side.y;
				points.n[3] = *(s_24e9c7_point const *)&placement->x;
				points.n[3].x += back.x;
				points.n[3].y += back.y;
				g_4e6950.field_0 = marker->color;
				g_4e6950.field_c = marker->point;
				g_4e6950.value18 = 0.0f;
				g_4e6950.value24 = marker->value40;
				real alpha;
				if (marker->use_full_alpha)
					alpha = g_4e69c0[g_4b9ed8] * marker->alpha;
				else if (!placement->hidden)
					alpha = (1.0f - placement->fade) * g_4e69c0[g_4b9ed8] * marker->alpha;
				else
					alpha = 0.0f;
				g_4e6950.value1c = alpha;
				function_22a664((s_widget_quad_2b11 const *)&points, (s_float_rect const *)&bounds,
					bitmap_index, first_bitmap, kind->shader_index);
			}
		}
	}
	return result;
}
