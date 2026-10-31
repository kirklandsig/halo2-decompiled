// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
#include "globals.h"
#include "unknown_13fd90.h"
#include "unknown_19c1d0.h"
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <math.h>

#include "unknown_163110.h"

struct s_22bbca { short top, left, bottom, right; };
void function_22bbca(s_22bbca const *bounds, long tag_index, long first, long second,
	long text_index, real alpha);

// @retail 0x163400
void function_163400(s_text_widget_a const *widget)
{
	if (g_4e034c && g_4e034c->index != NONE && widget->valid)
	{
		s_22bbca bounds = *(s_22bbca const *)&widget->bounds;
		byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
		byte *data = *(byte **)(definition + 0xc);
		bounds.left += g_4b9dd2;
		bounds.right += g_4b9dd2;
		bounds.top += g_4b9dd0;
		bounds.bottom += g_4b9dd0;
		if (widget->text[0] == 0)
			function_22bbca((s_22bbca const *)&bounds, *(long *)(data + 0x54c),
				*(long *)(data + 0x554), 0, *(long *)widget->unknown34, widget->color_b.alpha);
		else if (widget->text[0] == 1)
			function_22bbca((s_22bbca const *)&bounds, *(long *)(data + 0x55c),
				*(long *)(data + 0x564), 0, *(long *)widget->unknown34, widget->color_b.alpha);
	}
}

// @retail 0x163110
s_text_buffer *text_buffer_format(s_text_buffer *buffer, const word *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnwprintf((wchar_t *)buffer->text, 0x4f, (const wchar_t *)format, arguments);
	buffer->text[0x4f] = 0;
	return buffer;
}

// @retail 0x163140
void s_text_widget_a::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

// @retail 0x1631f0
void s_text_widget_b::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

// @retail 0x1632a0
void s_text_widget_c::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

// @retail 0x163350
void s_text_widget_d::initialize(const s_short_rectangle *rectangle, const color4f *color_a, const color4f *color_b, const word *text, long text_length, bool flag)
{
	s_short_rectangle bounds = *rectangle;
	bounds.right--;
	bounds.bottom--;
	this->bounds = bounds;
	this->color_a = *color_a;
	this->color_b = *color_b;
	unicode_string_copy(this->text, text, text_length);
	this->flag = flag;
	text_bounds = bounds;
	text_bounds.top--;
	text_bounds.left += 2;
	text_bounds.right -= 2;
	valid = 1;
}

struct s_loading_map_view
{
	long state;
	byte unknown04[0x14 - 4];
	long position_a;
	long position_b;
	char name[256];
};

// @retail 0x163610
char *function_163610()
{
	char *result = 0;
	s_loading_map_view *map = (s_loading_map_view *)&g_4e6948->state;
	if (map && map->state == 1)
	{
		long key0 = map->position_a;
		long key1 = map->position_b;
		if (key0 == NONE)
		{
			s_entry_a *entry = function_19c320(map->name);
			key0 = NONE;
			if (entry)
				key0 = entry->key0;
			entry = function_19c320(map->name);
			key1 = NONE;
			if (entry)
				key1 = entry->key1;
		}
		if (key0 != NONE)
		{
			long next = function_19c440(key0, key1);
			if (next != NONE)
			{
				s_entry_a *entry = function_19c270(key0, next);
				if (entry)
					result = entry->name;
			}
		}
	}
	return result;
}

class c_entry_list
{
public:
	c_entry_list(long maximum_count);
	bool add(long a, short b, long c, short d);
	short find(long a);
	void swap(short index0, short index1);

private:
	long maximum_count;
	word count;
	short *shorts_b;
	long *longs_a;
	long *longs_c;
	short *shorts_d;
};

// @retail 0x163c40
c_entry_list::c_entry_list(long maximum_count)
{
	longs_a = new long[maximum_count];
	shorts_b = new short[maximum_count];
	this->maximum_count = maximum_count;
	longs_c = new long[maximum_count];
	shorts_d = new short[maximum_count];
}

// @retail 0x163c80
bool c_entry_list::add(long a, short b, long c, short d)
{
	bool result = false;

	if (count < maximum_count - 1)
	{
		longs_a[count] = a;
		shorts_b[count] = b;
		longs_c[count] = c;
		shorts_d[count] = d;
		count++;
		result = true;
	}
	return result;
}

// @retail 0x163d20
short c_entry_list::find(long a)
{
	for (long i = 0; i < count; i++)
	{
		if (longs_a[i] == a)
			return (short)i;
	}
	return NONE;
}

// @retail 0x163d50
void c_entry_list::swap(short index0, short index1)
{
	long a = longs_a[index0];
	longs_a[index0] = longs_a[index1];
	longs_a[index1] = a;
	short b = shorts_b[index0];
	shorts_b[index0] = shorts_b[index1];
	shorts_b[index1] = b;
	long c = longs_c[index0];
	longs_c[index0] = longs_c[index1];
	longs_c[index1] = c;
	short d = shorts_d[index0];
	shorts_d[index0] = shorts_d[index1];
	shorts_d[index1] = d;
}

struct s_view
{
	byte unknown00[4];
	transform4x3f matrix;
	byte unknown38[0x54 - 0x38];
	point3f point54;
	byte unknown60[0x84 - 0x60];
	byte flag84;
	byte flag85;
	byte unknown86[0xb0 - 0x86];
	real margin;
	vector3f vector_b4;
	byte unknownc0[0xe4 - 0xc0];
	vector3f vector_e4;
};

struct s_bounds3d
{
	real x0, x1, y0, y1, z0, z1;
};

s_bounds3d *g_4687e0;

static void transform_point(const transform4x3f *matrix, const point3f *point, point3f *out)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

// @retail 0x1650a0
bool function_1650a0(const s_view *view, const point3f *point, s_bounds3d *bounds, real radius)
{
	bool result = true;
	point3f p;
	transform_point(&view->matrix, point, &p);
	if (p.z > radius)
		result = false;
	else
	{
	long i = 0;
	real z2 = p.z * p.z;
	s_bounds3d b;
	real *bn = &b.x0;
	for (; i < 2; i++)
	{
		real d = (real)sqrt(p.n[i] * p.n[i] + z2);
		if (radius >= d)
		{
			bn[i * 2] = -10000.0f;
			bn[i * 2 + 1] = 10000.0f;
		}
		else
		{
			real a = (real)asin(radius / d);
			real angle = (real)atan2(p.n[i], -p.z);
			real lo = angle - a;
			if (lo < -1.5707964f)
				bn[i * 2] = -10000.0f;
			else
				bn[i * 2] = (real)tan(lo);
			real hi = angle + a;
			if (hi > 1.5707964f)
				bn[i * 2 + 1] = 10000.0f;
			else
				bn[i * 2 + 1] = (real)tan(hi);
		}
	}
	real z_lo = p.z - radius;
	b.z0 = z_lo > -1.5258800e-5f ? -1.5258800e-5f : z_lo;
	real z_hi = p.z + radius;
	b.z1 = z_hi > -1.5258800e-5f ? -1.5258800e-5f : z_hi;
	*bounds = b;
	}
	return result;
}
struct s_plane
{
	vector3f normal;
	real d;
};

struct s_frustum_1648d0
{
	long identifier;
	byte unknown04[4];
	long plane_index;
	byte unknown0c[4];
	box2f rectangle;
	byte unknown20[4];
	point3f points[5];
	s_bounds3d bounds;
	s_plane planes[6];
	vector3f directions[4];
};

struct s_camera_163db0
{
	byte unknown00[0x38];
	transform4x3f matrix;
	bool has_plane;
	byte unknown6d[7];
	s_plane plane;
	byte unknown84[4];
	real depth;
};

struct box3f;
box3f *function_11f770(box3f *rectangle, long count, point3f const points[]);
int __stdcall function_1429d0(transform4x3f const *matrix, long count, point3f const *source, point3f *destination);
real function_30bf0(vector3f *vector);

bool g_47ff86 = true;

PRIVATE inline void transform_plane_163db0(transform4x3f const *matrix, s_plane const *source, s_plane *plane)
{
	real x = source->normal.i;
	real y = source->normal.j;
	real z = source->normal.k;
	plane->normal.i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	plane->normal.j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	plane->normal.k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	real scale = matrix->scale;
	real distance = matrix->position.y * plane->normal.j;
	distance += source->d * scale;
	distance += matrix->position.z * plane->normal.k;
	distance += matrix->position.x * plane->normal.i;
	plane->d = distance;
}

PRIVATE inline real plane_error_163db0(s_plane const *plane, point3f const *point)
{
	return (real)fabs(plane->normal.k * point->z + plane->normal.j * point->y + plane->normal.i * point->x - plane->d);
}
// score 39 (was 39): at retail's length (2833 bytes). Left: retail stores the identifier before it loads
// the copied rectangle (ours loads x0 first); the top and bottom planes sum the distance as z, y,
// d * scale, x and schedule the k row's first add before its third product, where the other planes
// sum y, d * scale, z, x (one helper gives the same order everywhere); and the x-term operand order
// (n.i or position.x loaded first) in plane 1, the top/bottom planes and three diagnostic tests.
// Round 51 (lane 16): retail's has-plane arm jumps past the plane-4 transform, so the shared
// transform after the if/else was a behaviour difference: the transform back in the else arm 99
// (2818 bytes, the stored unit normal forwarded), with that normal set through vector3f_assign
// 53 (2833 bytes, read back as retail does); the transform helper taking the source plane and the
// output plane (distance read from the source) 39 (the distance load stays after the normal
// stores); the k row as one expression 39 (kept, drops the `nk +=` split). x0 read through the
// rectangle pointer 95; all four products `depth * rect->` 95, all through result->rectangle 98,
// depth read at each product 162; the d * scale operand spellings 55/55/53; compile-only screens:
// all 24 one-expression distance sums 103 (canonicalised, a different order per plane), 168
// grouped sums best 79, dot3f distance forms 88-107, the camera/rectangle/points aliases or the
// negated-depth local dropped 39/52/42/41 (harness units), the error test written x first 35 (same).
// The rectangle pointer declared after the identifier store, x0 through it or the result, or the
// pointer first with depth assigned after the identifier 83/83/83 (2832 bytes).
// A second helper for the top and bottom planes (one-expression k row, z, y, d, x) gives them
// retail's code (harness 25) but is not kept without a match.
// Earlier score 96 (was 68, review): the top and bottom planes' own transform helper (a copy of
// transform_plane_163db0 that sums the distance as z, y, d * scale, x) is removed, since a copied
// helper body is kept only with a match; with that helper the function scored 68.
// Earlier score 68 (was 74): the near corner's z value is computed into a named local before the corner
// is stored and the corner is then stored as x, y, z, which is the order retail stores it in;
// the identifier store, the plane-5 normal stores and the distance product operands still differ.
// Earlier score 74 (was 76): the k component of the transformed plane normal is now summed through a
// named local, which reproduces retail's order of the three products and the two adds;
// the identifier store, the plane-5 normal stores and the distance product operands still differ.
// Earlier score 76 (was 78): the plane-4 normal is stored as k, i, j in the branch that sets it to the
// unit z normal, which is the order retail stores it in; the identifier store, points[2] store
// order, the plane-5 normal stores and the distance product operands still differ.
// Earlier score 78 (was 99): the plane-4 transform moved out of the else arm so both paths share it,
// and the plane-4 and plane-5 normals are set with vector3f_assign, which reproduces retail's
// read-back of the normals; the identifier store, points[2] store order and product operands differ.
// Earlier score 99 (was 124): retail sums the plane distance of the top and bottom planes in a different
// order than the other four, so those planes use their own transform helper and their distance
// products now line up; the identifier store, the last corner stores and the plane-4 and plane-5
// normals (which retail reads back from memory) still differ.
// Earlier score 124 (was 363): the error pointer is now cleared once before the plane checks and the
// per-group else branch is gone, so the diagnostic branches line up; the identifier store, the
// plane-normal products and the branch polarity of the last check in each group still differ.
// Earlier score 363 (was 363): frameless admission and EDI output agree; camera/corner roles, initial stores and normal/distance scheduling differ.
// Earlier: frameless guard and EDI output agree; camera/corners EBX/EBP and initial store scheduling differ.
// camera in EBP and the corner array in EBX (here exchanged); retail reads the copied rectangle
// through the copy's address (`[eax+4]`...) after the identifier store, where ours reads
// `[edi+0x14]`... ahead of it; plane input/product residency and the first store schedule.
// Round 137: rectangle pointer/reference for the copy and the four products 415/415 (reads move
// behind the identifier store as in retail, but the pointer takes EBP); corner-array pointer
// for the corner stores and both 0x1429d0 arguments 655; depth read at each product 666; plain
// plane test in place of the double negation 404 (same code); returns in place of the result
// flag 688.
// Earlier: YZX normal inputs 442; scalar-first normal rows 423; running distance 412 kept;
// diagnostic disjunctions 423; native float error with distance 412; corner loop 432;
// side-plane loop 1154; reverse direction loop 423; default plane first 423; direct
// first-diagnostic expression 423; native matrix snapshot 778; input-rectangle products 446;
// coordinate-pair product initialization 425; unsigned direction ordinal 423; optional-plane
// copy 647; normal setter 610; bounds copy block 423; position copy block 423; directions after
// bounds 578; complete planes before normalize 515; identifier at geometry end 767; corners in
// index order 426; diagnostic result chain 423; plane tests result 671; camera plane predicate
// 669; far depth snapshot 646; corner point setters 622; rectangle CRT copy 423; plane array
// receiver 761; native plane return 700; late right/top products 682; scaled distance 469;
// normal inputs KJI 433; normal names 423; helper order 422; scaled distance/KJI 474; first
// slope before constants 478; all side slopes 618; last corner XYZ 424; complete-normal
// xyz/yxz/xzy/zxy 512/512/511/512 and with distance products 512 (exact only);
// distance-products xyz/yxz/zxy 412/426/430; immutable scalar inputs 412; native/immutable
// native normal 789/789; complete row accumulators 410; immutable inputs with row accumulators
// 410; native normal with rows 790; parallel normal products 410; native normal/products 789;
// separate normal pointer/reference/value 404/404/835, with matrix reference 404/404/835, with
// output-address return 404/404 (separate normal pointer kept); separate distance input 611;
// whole-plane pointer/reference/value 403/403/883; rotation-scale-position inputs 684; with
// distance/plane pointer/reference 582/717/717; shared triple-test expression/success-result/
// failure-result/early-failures, point first 1182/1170/1170/1182, point last
// 1182/1168/1168/1182; scalar normal arguments in all six orders 697; scalar XYZ/ZYX with matrix
// reference 697/697; complete depth snapshot 733; all depth fields 667; identifier before
// rectangle 431; input rectangle value 686; depth snapshot/identifier 733; fields/identifier
// 682; rectangle/snapshot 699; rectangle/fields 683; on separate-normal input, parallel row
// accumulators 404, all row products named 407, initial two-term sums 407, final forward terms
// 455, scalar-first counterparts 404/407/407/455; three-input transform formal orders
// M/O/N, N/M/O, N/O/M, O/M/N, O/N/M 404/404/403/404/403; transform/diagnostic/both forced
// expansion 404/404/404.
// Frameless admission/output EDI agree; camera/point saved registers and initial stores remain.
// Camera/points saved registers and initial stores remain; input distance/whole-plane ownership does not qualify.
// Earlier: Camera/point saved registers and initial stores remain; separate normal input improves plane scheduling.

// frameless entry agrees; camera/points EBX/EBP and initial store scheduling differ.
// Frameless guard agrees; camera/points register ownership, plane reloads and first stores remain.
// Earlier 412 (was 412): frameless entry agrees; camera/points EBX/EBP and initial store scheduling differ.
// YZX normal inputs 442; scalar-first normal rows 423; running distance 412 kept plain; diagnostic disjunctions 423;
// Camera/point saved registers and initial stores differ; complete buffered normal rows require an exact match.
// Earlier: Camera/point EBX/EBP ownership and initial store schedule remain. YZX normal
// Camera/point EBX/EBP ownership and initial store schedule remain. YZX normal
// inputs 442; scalar-first normal rows 423; running distance 412 kept plain; diagnostic disjunctions 423;
// native float error with distance 412.
// Earlier 423 (was 423): camera/points use EBX/EBP rather than EBP/EBX;
// corner loop 432; side-plane loop 1154; reverse direction loop 423;
// default plane first 423; direct first-diagnostic expression 423.
// Native matrix snapshot 778; input-rectangle products 446; coordinate-pair
// product initialization 425; unsigned direction ordinal 423; optional-plane copy 647.
// j90: camera/points saved registers remain exchanged; native copy boundaries tie and geometry/store lifetime changes regress.
// normal setter 610; bounds copy block 423; position copy block 423; directions after bounds 578; complete
// planes before normalize 515; identifier at geometry end 767
// j92: camera/points saved registers remain exchanged; corner order and diagnostic/lifetime forms do not improve the frameless entry.
// corners in index order 426; diagnostic result chain 423; plane tests result 671; camera plane predicate 669;
// far depth snapshot 646.
// Earlier 423 (was 423): Camera/points saved registers and initial store schedule remain.
// corner point setters 622; rectangle CRT copy 423; plane array receiver 761;
// native plane return 700; late right/top products 682 (fast).
// Earlier 423 (was 423): camera/point homes and plane input/product residency remain.
// Scaled distance 469; normal inputs KJI 433; normal names 423; helper order 422;
// scaled distance/KJI 474; first slope before constants 478; all side slopes 618; last corner XYZ 424.
// complete-normal-xyz 512 (exact only); distance-products-xyz 412.
// complete-normal-distance-products-xyz 512 (exact only); complete-normal-yxz 512 (exact only).
// complete-normal-xzy 511 (exact only); complete-normal-zxy 512 (exact only).
// distance-products-yxz 426; distance-products-zxy 430.
// Immutable scalar inputs 412; native/immutable native normal 789/789; complete row
// accumulators 410; immutable inputs with row accumulators 410; native normal with rows 790;
// parallel normal products 410; native normal/products 789. Two-point reductions rejected.
// Round 129: separate normal pointer/reference/value 404/404/835; with matrix reference
// 404/404/835; separate normal pointer/reference with output-address return 404/404.
// Only separate normal pointer retained, plain 404; all original input copies precede stores.
// Round 132: separate distance input 611; whole-plane pointer/reference/value 403/403/883;
// rotation-scale-position inputs 684; with distance/plane pointer/reference 582/717/717.
// One-point whole-plane reductions rejected; original in-place input copies retained.
// Round 134: shared triple-test expression/success-result/failure-result/early-failures,
// point first 1182/1170/1170/1182; point last 1182/1168/1168/1182.
// Scalar normal arguments XYZ/XZY/YXZ/YZX/ZXY/ZYX 697/697/697/697/697/697;
// scalar XYZ/ZYX with matrix reference 697/697.
// Complete depth snapshot 733; all depth fields 667; identifier before rectangle 431; input rectangle
// value 686; depth snapshot/identifier 733; fields/identifier 682; rectangle/snapshot 699;
// rectangle/fields 683.
// Round 145: On separate-normal input, parallel row accumulators 404; all row products named 407;
// initial two-term sums 407; final forward terms 455. Scalar-first counterparts 404/407/407/455;
// all restored.
// Three-input transform formal orders M/O/N, N/M/O, N/O/M, O/M/N, O/N/M
// 404/404/403/404/403; transform/diagnostic/both forced expansion 404/404/404.
// One-point formal-order leads rejected; original helper contracts and all normal copies restored.
// Computed distance returned for immediate caller store: normal input orders
// XYZ/XZY/YXZ/YZX/ZXY/ZYX 486/490/490/492/491/489; forced expansion 486;
// out-of-line boundary 1758. All input copies and original helper restored.
// Round 160: diagnostic point/plane/both native inputs 1229/1363/1898;
// scalar point/plane/both inputs 654/755/755; forced scalar point/both
// expansion 654/755. Original diagnostic and transform helpers restored.
// Round 149: Split native normal/distance outputs with all matrix, normal-input and distance-output
// pointer/reference contracts [476, 476, 476, 476, 476, 476, 476, 476]; all restored.
// Round 236: conditional diagnostic pointer updates for right/top/tip/all
// corner/right+tip/top+tip/all/all including initial result 404 each; all restored.
// Round 244: rectangle/position/both/bounds/rectangle-bounds/position-bounds/all
// memmove copies 721/691/716/712/721/710/740; all three memcpy copies 404.
// Original native copies and separated normal inputs restored; camera/corner homes remain.
// Round 218: CRT output-address rectangle products/all, memcpy 415/417,
// memmove 721/592; each with copied-bounds output consumed 415/417/721/592. All restored.
// Copy output identity does not recover camera EBP/corners EBX or the first store schedule.
// Round 254: diagnostic parallel products/accumulator/both/absolute-result/products-result/
// accumulator-result/all/projected-point part 400/404/400/400/404/400/404/400. Four-point
// reductions rejected; original diagnostic helper and complete body restored.
// Round 239: parallel up/left/up+left/forward/up+forward/left+forward
// product operand ownership, complete distance products, all products: 404 each. All restored.
// Round 300: the third plane's distance store moved up beside the second plane's own zero store,
// with the same value reaching that plane's transform 363 (was 399). The separate guard tests and
// the result flag's declaration order were left alone.
// Round 412: distance zeros before their normalizers, plane subsets 0/1/01/2/02/12/012, 548/400/523/523/551/536/526; all three before normal component stores 435. Original body, retained fourth-distance phase and every helper restored.
// Round 215: identifier after depth/x0/x1/y0/y1 acquisition 368 each; after x0/x1/y1 with the real
// copied-rectangle receiver 386 each. Original publication and receiver restored; camera/corner
// register exchange and initial/normal-distance scheduling remain.
// Round 432: sixteen-use diagnostic Boolean success/failure returns, direct/signed/absolute/predicate
// storage 416/416/432/381 and 416/416/432/771; original real-return helper restored.
// Round 91: camera alias dropped 368; depth local for the corner z stores 720; fourth distance zero
// back after its normalizer 399 (aligned 103 against 143, 2814 bytes); plane 0 normal k stored first
// 574; planes 0-3 normals through the existing vector3f_assign 590 (aligned 123), with the fourth
// zero back 636 (aligned 83, 2807 bytes: retail's float loads and pointer stores for the normals),
// plus planes 4/5 through it 615 (2837 bytes, aligned 100), plus a cleared error first 692, error
// cleared first alone on the aligned-83 form 677, all with the camera-plane branch too 697.
// Round 43 (one-file LTCG link of this file and its callee objects, equal to the full listing; current
// form 39 there): any one-expression distance sum is reordered by the compiler (all 24 term orders 73);
// the existing matrix_transform_vector + dot3f for the transform 43 (dot3f(normal, position) 51, the
// rows kept with dot3f 65, the source plane's d read back 62); the existing plane_distance_to_point for
// the error tests 43, dot3f in them 43; camera alias dropped 39 and `if (camera->has_plane)` 39 (same
// code). Retail's planes 2 and 3 alone sum z, y, d * scale, x: the separate top/bottom helper gives 17;
// with a position pointer local (`point3f const *position = &matrix->position;` before the sum) in that
// helper 15, in the side helper 21, in both 42; with a normal pointer in the error helper as well 11
// (left: the identifier load order and the x-term operand order of planes 1 and 3; fast agrees, 11).
// The x-term and error-test operand orders follow the compiler's temporary numbering (dead copies of
// depth, diagnostic only, rotate the error tests with period 8: 7 extra gives 9 on the 17 form), the
// identifier load order does not (dead copies before it change nothing); x0/x1/y0/y1 through rect,
// the copy through a non-const rect, depth read before the copy or before the identifier 22/24/22/29/18.
// On the 11 form, x0 through rect gives retail's identifier store and load order and all four error-test
// orders, but x0 is then multiplied as `depth * [x0]` (16; the four corner lines in any order 16-22, the
// copy field by field 678, depth read at the product 651); one dead temporary ahead of the copy
// (diagnostic only) restores the x0 product, and the floor over 128 dead-temporary placements is 6:
// plane 1's x term and the plane 2/point 3 and plane 3/point 0 error tests keep the other operand first.
// Round 53: x0 as `depth * rect->x0` 83; x0 through the pointer declared after x1/y0/y1 659;
// products in x0, y0, x1, y1 order through the pointer 87; the copy through a non-const rectangle
// pointer with x0 through it 83, with x0 through the result 39 (same code). Compile-only: the x term
// written `n.i * position.x` still loads position.x first in every plane; one plane pointer reused
// for planes 0-3 49 (harness, forwards the zero distance).
// Round 139: the identifier stored before the rectangle copy 135 (2831 bytes); depth read before the
// copy 51; x0 read from the input rectangle 679; on the pointer-copy form (83: the identifier store,
// depth and rectangle loads are retail's, only x0's product folds the load depth-first and is one
// byte short) x0 written depth-first 83 (same code) and x0 computed last 659; the in-place plane
// transform of 0x184440's file (one-expression z, y, scale * d, x distance) for planes 2 and 3 730.
// @retail 0x163db0
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result)
{
	bool valid = false;
	if (rectangle->x1 > rectangle->x0 && rectangle->y1 > rectangle->y0)
	{
		point3f *points = &result->points[0];
		s_camera_163db0 const *cam = camera;
		box2f const *rect = &result->rectangle;
		result->rectangle = *rectangle;
		result->identifier = identifier;
		real depth = cam->depth;
		real x0 = result->rectangle.x0 * depth;
		real x1 = rect->x1 * depth;
		real y0 = rect->y0 * depth;
		real y1 = rect->y1 * depth;
		result->points[1].z = 0.0f - cam->depth;
		result->points[1].x = x0;
		result->points[1].y = y0;
		result->points[3].z = 0.0f - cam->depth;
		result->points[3].x = x1;
		result->points[3].y = y0;
		points->z = 0.0f - cam->depth;
		points->x = x0;
		points->y = y1;
		real negated_depth = 0.0f - cam->depth;
		result->points[2].x = x1;
		result->points[2].y = y1;
		result->points[2].z = negated_depth;
		function_1429d0(&cam->matrix, 4, points, points);
		result->points[4] = cam->matrix.position;
		result->bounds = *g_4687e0;
		function_11f770((box3f *)&result->bounds, 5, result->points);
		vector3f_assign(&result->planes[0].normal, -1.0f, 0.0f, 0.0f - result->rectangle.x0);
		function_30bf0(&result->planes[0].normal);
		result->planes[0].d = 0.0f;
		transform_plane_163db0(&cam->matrix, &result->planes[0], &result->planes[0]);
		vector3f_assign(&result->planes[1].normal, 1.0f, 0.0f, result->rectangle.x1);
		function_30bf0(&result->planes[1].normal);
		result->planes[1].d = 0.0f;
		transform_plane_163db0(&cam->matrix, &result->planes[1], &result->planes[1]);
		vector3f_assign(&result->planes[2].normal, 0.0f, -1.0f, 0.0f - result->rectangle.y0);
		function_30bf0(&result->planes[2].normal);
		result->planes[2].d = 0.0f;
		transform_plane_163db0(&cam->matrix, &result->planes[2], &result->planes[2]);
		vector3f_assign(&result->planes[3].normal, 0.0f, 1.0f, result->rectangle.y1);
		function_30bf0(&result->planes[3].normal);
		result->planes[3].d = 0.0f;
		transform_plane_163db0(&cam->matrix, &result->planes[3], &result->planes[3]);
		if (!(!(cam->has_plane)))
		{
			result->planes[4].normal.i = 0.0f - cam->plane.normal.i;
			result->planes[4].normal.j = 0.0f - cam->plane.normal.j;
			result->planes[4].normal.k = 0.0f - cam->plane.normal.k;
			result->planes[4].d = 0.0f - cam->plane.d;
		}
		else
		{
			vector3f_assign(&result->planes[4].normal, 0.0f, 0.0f, 1.0f);
			result->planes[4].d = 0.0f;
			transform_plane_163db0(&cam->matrix, &result->planes[4], &result->planes[4]);
		}
		vector3f_assign(&result->planes[5].normal, 0.0f, 0.0f, -1.0f);
		result->planes[5].d = cam->depth;
		transform_plane_163db0(&cam->matrix, &result->planes[5], &result->planes[5]);
		vector3d_from_points3d(&result->points[4], &result->points[0], &result->directions[0]);
		vector3d_from_points3d(&result->points[4], &result->points[1], &result->directions[1]);
		vector3d_from_points3d(&result->points[4], &result->points[2], &result->directions[2]);
		vector3d_from_points3d(&result->points[4], &result->points[3], &result->directions[3]);
		char const *error = 0;
		if (!(plane_error_163db0(&result->planes[0], &result->points[1]) < 0.01f &&
			plane_error_163db0(&result->planes[2], &result->points[1]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[1]) < 0.01f))
			error = "_pyramid_bottom_left world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[1], &result->points[3]) < 0.01f &&
			plane_error_163db0(&result->planes[2], &result->points[3]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[3]) < 0.01f))
			error = "_pyramid_bottom_right world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[0], &result->points[0]) < 0.01f &&
			plane_error_163db0(&result->planes[3], &result->points[0]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[0]) < 0.01f))
			error = "_pyramid_top_left world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[1], &result->points[2]) < 0.01f &&
			plane_error_163db0(&result->planes[3], &result->points[2]) < 0.01f &&
			plane_error_163db0(&result->planes[5], &result->points[2]) < 0.01f))
			error = "_pyramid_top_right world-vertex off planes";
		if (!(plane_error_163db0(&result->planes[0], &result->points[4]) < 0.01f &&
			plane_error_163db0(&result->planes[1], &result->points[4]) < 0.01f &&
			plane_error_163db0(&result->planes[2], &result->points[4]) < 0.01f &&
			plane_error_163db0(&result->planes[3], &result->points[4]) < 0.01f))
			error = "_pyramid_tip world-vertex off planes";
		if (g_47ff86 && error)
			g_47ff86 = false;
		valid = true;
	}
	return valid;
}

struct s_clip_1648d0
{
	byte unknown00[0x10];
	real x0, x1, y0, y1;
	real margin;
};

struct s_projection_1648d0
{
	s_bounds3d bounds;
	s_bounds3d unknown18;
	point2f hull[12];
	short hull_count;
};

struct s_plane_block_1648d0
{
	byte unknown00[0xc];
	s_plane *planes;
};

struct s_bsp_1648d0
{
	byte unknown00[0x18];
	s_plane_block_1648d0 *block;
};

/* how far along the ray the plane lies, in units of the direction (0 when they are parallel) */
PRIVATE inline real ray_plane_distance_1648d0(point3f const *origin, vector3f const *direction,
	s_plane const *plane)
{
	real dot = direction->j * plane->normal.j;
	dot += direction->k * plane->normal.k;
	dot += plane->normal.i * direction->i;
	real distance;
	if (dot != 0.0f)
	{
		real offset = origin->y * plane->normal.j;
		offset += origin->z * plane->normal.k;
		offset += plane->normal.i * origin->x;
		distance = (offset - plane->d) * (-1.0f / dot);
	}
	else
		distance = 0.0f;
	return distance;
}

bool function_1652e0(s_view *view, real margin, const point3f *points, long point_count, point3f *projected_points, short *projected_count, s_bounds3d *bounds, bool use_plane0, bool use_plane1);
long function_239cb0(long count, point3f const *points, point2f *hull);

// @retail 0x1648d0
bool function_1648d0(s_frustum_1648d0 const *source, s_view *view,
	s_clip_1648d0 const *clip, s_projection_1648d0 *result, bool make_hull)
{
	bool success = false;
	s_clip_1648d0 const * *clip_reference = &clip;
	s_view * *view_reference = &view;
	long pyramid_edges[8][2] = { {0, 1}, {1, 3}, {3, 2}, {2, 0}, {4, 0}, {4, 1}, {4, 2}, {4, 3} };
	long box_edges[12][2] = { {0, 1}, {1, 3}, {3, 2}, {2, 0}, {4, 5}, {5, 7}, {7, 6}, {6, 4}, {4, 0}, {5, 1}, {6, 2}, {7, 3} };
	point3f points[8];
	long (*edges)[2];
	long edge_count;
	if (source->plane_index != NONE)
	{
		s_plane const *plane = &((s_bsp_1648d0 *)g_4e0348)->block->planes[source->plane_index];
		for (long i = 0; i < 4; i++)
		{
			vector3f const *direction = &source->directions[i];
			real distance = ray_plane_distance_1648d0(&source->points[4], direction, plane);
			points[i] = source->points[i];
			points[i + 4].x = direction->i * distance + source->points[4].x;
			points[i + 4].y = direction->j * distance + source->points[4].y;
			points[i + 4].z = direction->k * distance + source->points[4].z;
		}
		edge_count = 12;
		edges = box_edges;
	}
	else
	{
		memcpy(points, source->points, 5 * sizeof(point3f));
		edge_count = 8;
		edges = pyramid_edges;
	}
	long point_count = 0;
	real margin = 1.52588e-5f > (*clip_reference)->margin ? 1.52588e-5f : (*clip_reference)->margin;
	s_bounds3d bounds = *g_4687e0;
	point3f segments[24];
	point3f projected[24];
	short projected_count;
	for (long i = 0; i < edge_count; ++i)
	{
		segments[point_count++] = points[edges[i][0]];
		segments[point_count++] = points[edges[i][1]];
	}
	function_1652e0((*view_reference), margin, segments, point_count, projected, &projected_count, &bounds, true, true);
	result->bounds = *g_4687e0;
	result->unknown18 = *g_4687e0;
	result->hull_count = 0;
	if (projected_count >= 4)
	{
		box2f clipped;
		clipped.x0 = bounds.x0 > (*clip_reference)->x0 ? bounds.x0 : (*clip_reference)->x0;
		clipped.x1 = bounds.x1 > (*clip_reference)->x1 ? (*clip_reference)->x1 : bounds.x1;
		clipped.y0 = bounds.y0 > (*clip_reference)->y0 ? bounds.y0 : (*clip_reference)->y0;
		clipped.y1 = bounds.y1 > (*clip_reference)->y1 ? (*clip_reference)->y1 : bounds.y1;
		if (!(clipped.x0 > clipped.x1 || clipped.y0 > clipped.y1))
		{
			*(box2f *)&result->bounds = clipped;
			if (make_hull)
				result->hull_count = (short)function_239cb0(projected_count, projected, result->hull);
			result->bounds.z0 = bounds.z0;
			result->bounds.z1 = bounds.z1;
			success = true;
		}
	}
	return success;
}

// Round 16: retail loads the three `view->vector_e4` coefficients into xmm0, xmm3 and xmm4
// before it copies the point and then reloads the point's components from that copy, ours reloads
// the copied normal's x into xmm4; retail keeps the near distance in xmm5 and the zero in xmm4,
// ours keeps the near distance in xmm6 and the zero in xmm1.
// score 115 (was 115): coefficient XMM roles, classification operands and projection staging still differ.
// Intersection/projected point setters 119/119; existing in-place vector scale 115; long/byte result 216/119.
// Forward value copy 474; direct projected element 246; scaled direct element 247; separate scaled vector 115.
// Second-plane-first 425; reuse first negated normal 463; indexed plane selection 375.
// Frame pointer 574; zero inline depth 609.
// First/second normal memcpy 115/115; negated aggregate 115;
// native endpoint classification 387; default-zero intersection ratio 252.
// Earlier: Coefficient/zero XMM roles, classification products and projection staging remain.
// Existing scale for first/second/source/all normal negations 425/410/433/446f; reused projected
// workspace 249f; per-use outside bytes 249f; completed projection total 214f; native short total 115f.
// Earlier: coefficient/zero XMM roles, classification operands and projection homes remain.
// Native camera vector member 115; camera/bounds references 251/133; clipped/camera/projected point names 115/115/115;
// paired projection names 115; native vector plus three point names 115.
// First/second input-vector value copies 463/474; external vector references 251/279;
// both copies 454; shared scaled difference 390; conditional plane enable 474;
// projection cursor before its guard 115. Arithmetic grouping and count guards retained.
// Earlier: Normal/zero XMM roles, classification and projection homes remain; offset
// accumulator 269; shared segment count 468; count result 459; projected/intersection values 402/323;
// bool/second endpoint values 259/362; completed cursor 196.
// The locals as plane3f 115; plane_distance_to_point for the classification 265, plus the
// intersection distance 269, plus dot3f for the denominator 216; the distance alone 117;
// dot3f for the first plane's d 278 (either argument order), for both planes' d 155.
// Round 199: matching named xz subtotal, y product, x/z products and all products in both plane
// rows 305/267/280/269; with existing component assignment for every normal negation
// 306/301/174/285. All restored; original grouping, scale helper and guards retained; coefficient
// preload and projection homes remain.
// Round 419: complete near plane 496; complete far plane 460; complete near plane/complete far plane 564;
// complete near plane/immutable completed planes 496; complete far plane/immutable completed planes 460;
// complete near plane/complete far plane/immutable completed planes 564; complete near plane/complete far
// plane/parallel full offset producers 567; complete near plane/complete far plane/immutable completed
// planes/parallel full offset producers 567. Coefficient/zero XMM roles, classification operands and
// projection staging remain. Original body restored.
// Round 184: logical second-plane enable 459; implied pass retest removed 370;
// classification/clipping-pair/projection-pair source cursors 327/255/117;
// classification with clipping/projection cursors 193/327; all cursors 195.
// Native byte/Boolean culling-array initialization 489/489 (frame grows sixteen bytes).
// All restored; coefficient preload, classification operands and projection staging remain.
// Round 80: forward declaration first 196; const view 115; first-plane receiver 285,
// with j-k-i offset order 285; inner do/while and condition increment 115/115;
// const x value/reference 141/119. Original body restored.
// Lane 15 r19: retail loads all three vector_e4 components before the forward copy (ours loads e4.i
// after it, which swaps XMM3/XMM4 from there on). Both planes through one forceinline plane helper
// 281 (1867 bytes); plane 0's d as one negated expression 143; near distance after the copy 141;
// forward pointer removed 264 (1867 bytes); clip denominator / distance / both as single
// expressions 247/251/117.
// Round 91: first plane's d in i, j, k order 146, in j, k, i order 133; classification in i, j, k
// order 115, point-first products 115; the intersection scaled through a value-first scale helper
// (i = s * i, j = j * s, k = k * s) 115. Retail's first-plane block loads camera.i early and the
// normal's i late (ours the reverse); its denominator copies delta for i and k and folds the normal.
// Round 243: Existing normal-copy setters and overlap-safe bounds transfer,
// subsets 1..7 460/502/526/495/508/565/569; all with native negated input 569.
// Original transfers and complete body restored; coefficient preload and projection homes remain.
// Round 100 (lane 8, fast): plane 0's d in every term order with camera-first or normal-first products: operand order
// never changes the code; i/j/k 146, i/k/j normal-first 115 (same code), j/i/k 149, j/k/i 133, k/i/j 144, k/j/i 133.
// @retail 0x1652e0
bool function_1652e0(s_view *view, real margin, const point3f *points, long point_count, point3f *projected_points, short *projected_count, s_bounds3d *bounds, bool use_plane0, bool use_plane1)
{
	point3f local_points[32];
	byte outside[32];
	byte segment_culled[16];
	s_plane planes[2];
	bool culled = true;

	use_plane1 &= (view->flag84 || view->flag85);
	*bounds = *g_4687e0;

	memset(segment_culled, 0, sizeof(segment_culled));

	real near_sum = view->margin + margin;
	real near_distance = 1.52588e-5f > near_sum ? 1.52588e-5f : near_sum;

	vector3f const *forward = (vector3f const *)&view->point54;
	planes[0].normal = *forward;
	planes[0].d = view->vector_e4.i * planes[0].normal.i + view->vector_e4.k * planes[0].normal.k + view->vector_e4.j * planes[0].normal.j;
	planes[0].normal.i = -planes[0].normal.i;
	planes[0].normal.j = -planes[0].normal.j;
	planes[0].normal.k = -planes[0].normal.k;
	planes[0].d = -planes[0].d;
	planes[0].d += near_distance;

	vector3f negated;
	negated.i = -forward->i;
	negated.j = -forward->j;
	negated.k = -forward->k;
	planes[1].normal = negated;
	planes[1].d = view->vector_b4.i * planes[1].normal.i + view->vector_b4.k * planes[1].normal.k + view->vector_b4.j * planes[1].normal.j;
	planes[1].normal.i = -planes[1].normal.i;
	planes[1].normal.j = -planes[1].normal.j;
	planes[1].normal.k = -planes[1].normal.k;
	planes[1].d = -planes[1].d;

	memcpy(local_points, points, point_count * sizeof(point3f));

	for (long pass = 0; pass < 2; pass++)
	{
		s_plane *plane;
		if (pass == 0)
		{
			if (!use_plane0)
				continue;
			plane = &planes[0];
		}
		else
		{
			if (pass == 1 && !use_plane1)
				continue;
			plane = &planes[1];
		}

		long i;
		for (i = 0; i < point_count; i++)
		{
			outside[i] = plane->normal.i * local_points[i].x + plane->normal.k * local_points[i].z + plane->normal.j * local_points[i].y - plane->d > -0.0001f ? 1 : 0;
		}

		long segment_count = point_count / 2;
		long segment = 0;
		if (segment_count > 0)
		{
		do
		{
			byte outside0 = outside[segment * 2];
			segment_culled[segment] |= (!outside0 && !outside[segment * 2 + 1]);
			if (!segment_culled[segment] && outside0 != outside[segment * 2 + 1])
			{
				point3f *p0 = &local_points[segment * 2];
				point3f *p1 = &local_points[segment * 2 + 1];
				vector3f delta;
				vector3d_from_points3d(p0, p1, &delta);
				real denominator = delta.i * plane->normal.i;
				denominator += delta.k * plane->normal.k;
				denominator += plane->normal.j * delta.j;
				real t;
				if (denominator != 0.0f)
				{
					real distance = plane->normal.j * p0->y;
					distance += p0->z * plane->normal.k;
					distance += plane->normal.i * p0->x;
					t = (distance - plane->d) * (-1.0f / denominator);
				}
				else
					t = 0.0f;
				vector3f_scale(&delta, t, &delta);
				point3f intersection;
				intersection.x = delta.i + p0->x;
				intersection.y = delta.j + p0->y;
				intersection.z = delta.k + p0->z;
				if (!outside0)
					*p0 = intersection;
				else
					*p1 = intersection;
			}
			segment++;
		} while (segment < segment_count);
		}
	}

	long segment = 0;
	long segment_count = point_count / 2;
	long total = 0;
	if (segment_count > 0)
	{
	point3f *cursor = projected_points;
	do
	{
		if (!segment_culled[segment])
		{
			culled = false;
			total += 2;
			point3f *output = cursor;
			cursor += 2;
			for (long k = 0; k < 2; k++)
			{
				point3f t;
				point3f *out = &output[k];
				transform_point(&view->matrix, &local_points[segment * 2 + k], &t);
				real w = t.z > -1.52588e-5f ? -1.52588e-5f : t.z;
				real factor = -1.0f / w;
				point3f s;
				s.x = t.x * factor;
				s.y = t.y * factor;
				s.z = t.z;
				if (projected_points)
					*out = s;
				if (bounds->x0 > s.x)
					bounds->x0 = s.x;
				if (s.x > bounds->x1)
					bounds->x1 = s.x;
				if (bounds->y0 > s.y)
					bounds->y0 = s.y;
				if (s.y > bounds->y1)
					bounds->y1 = s.y;
				if (bounds->z0 > s.z)
					bounds->z0 = s.z;
				if (s.z > bounds->z1)
					bounds->z1 = s.z;
			}
		}
		segment++;
	} while (segment < segment_count);
	}

	if (projected_count)
		*projected_count = (short)total;
	return !culled;
}

/* the colors of the connection bars: the frame, then good, fair and poor */
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;
color4f *g_4686d8;
color4f *g_4686dc;
color4f *g_4686e8;

void function_36880(s_short_rectangle const *rectangle, color4f const *color);

/* draws a connection quality widget: a frame and one bar per quality step */
// @retail 0x1634d0
void connection_widget_draw(s_text_widget_c const *widget)
{
	if (widget->valid)
	{
		long bar_count;
		color4f color;
		s_short_rectangle bounds;
		color4f const *bar_color;
		long i;

		if (widget->text[0] < 1)
		{
			bar_count = 1;
		}
		else if (widget->text[0] > 6)
		{
			bar_count = 6;
		}
		else
		{
			bar_count = widget->text[0];
		}
		color = *(color4f *)g_4686d4;
		bounds.left = widget->bounds.left + 1;
		bounds.right = widget->bounds.right - 1;
		color.alpha = widget->color_b.alpha;
		bounds.top = widget->bounds.bottom - 0x12;
		bounds.bottom = widget->bounds.bottom - 1;
		function_36880(&bounds, &color);

		bounds.left = widget->bounds.left + 1;
		bounds.right = widget->bounds.right - 1;
		bounds.top = widget->bounds.bottom - 3;
		bounds.bottom = widget->bounds.bottom - 1;
		if (bar_count >= 4)
		{
			color = *g_4686dc;
		}
		else if (bar_count >= 2)
		{
			color = *g_4686e8;
		}
		else
		{
			color = *g_4686d8;
		}
		color.alpha = widget->color_b.alpha;
		for (i = bar_count; i > 0; i--)
		{
			function_36880(&bounds, &color);
			bounds.top -= 3;
			bounds.bottom -= 3;
		}
	}
}
