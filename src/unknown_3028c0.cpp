// @flags /O2 /GL-
// Havok (third-party physics library): starting a walk of a segment through
// a grid, the segment taken into the grid's units; and handing back the ids
// of a list's entries from the last, emptying the list.
#include <xmmintrin.h>

// a grid: its origin, with its cell size in the fourth lane
struct grid_3028c0
{
	__m128 origin;
	char unknown10[0x30 - 0x10];
	char cells[1];
};

struct grid_holder_3028c0
{
	char unknown0[0x10];
	grid_3028c0 const *grid;
};

class counted_3028c0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual long count() { return 0; }
};

// a segment and the direction it sweeps
struct input_3028c0
{
	__m128 start;
	__m128 end;
	__m128 direction;
	char unknown30[0x34 - 0x30];
	counted_3028c0 **counted;
	grid_holder_3028c0 **holder;
};

struct segment_3028c0
{
	__m128 start;
	__m128 end;
};

// the walk's state
struct walk_3028c0
{
	__m128 field_0;
	__m128 direction;
	float length;
	long field_24;
	float scale;
	long field_2c;
	long field_30;
};

// a vector of four copies of a float
inline __m128 splat_3028c0(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

// the sum of a vector's first three terms
inline float sum_3028c0(__m128 const &v)
{
	float sum;
	_mm_store_ss(&sum, _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v)));
	return sum;
}

class class_301a60
{
public:
	void function_301a60(walk_3028c0 *walk, char const *cells, segment_3028c0 const *segment);
};

class walker_3028c0
{
public:
	char unknown0[0x10];
	grid_3028c0 const *grid;
	float inverse_cell_size;
	long count;
	float field_1c;
	float field_20;
	long field_24;
	long field_28;
	input_3028c0 const *input;

	void function_3028c0(input_3028c0 const *input_, long arg_1, long arg_2);
};

// score 0 (was 42): matched with the cell array bound by a constant reference
// (retail forms its address early in its own register), the scale splatted
// first in each product, both differences taken before either product, and the
// three clears made just before the call.
// Before, 42 (was 42): header/argument registers and early zero carrier still differ.
// Earlier allocation: the walk's last field is cleared before the direction
// the walk's last field is cleared before the direction
// is built, the direction and the segment's start are multiplied with the
// vector first, and the grid's origin is read once for both ends. What is
// left is the two call arguments, which retail pushes late and ours still
// pushes at the top.
// Further trials from 42: ordered scalar clears 45; scale-first direction
// 45; endpoint deltas 45; reversed declarations 45; stored scale 45;
// shared splat 60; renamed locals 45; both pointers 45; staged segment
// members 45; inline/force-inline byte accessor diagnostics 42/45 (removed).
// Further trials: collector reference/pointer fields 42; input reference 42;
// broadcast scalar reference 42; referenced scalar load 55; whole-block
// memset 74; zero aggregate 92; value-parameter scalar load 91; inherited
// walk method 42. Opening body retained.
// Further trials: sum by value 40; staged sum 42; vector sum result 56;
// direct splat 65; forced inline math 42; whole query initializer 48;
// sum through output 44; size preference 121; value sum with late clears 43.
// Further trials: segment before direction 48; translations before length
// 47; length after segment 48; shared direction value 48; shared origin
// value 64; direction with early translations 50 or late length 46; both
// shared values with late length 63. Header/cells registers and late pushes
// still differ; the opening zero-store and arithmetic forms are retained.
// Late final clear 45; late clear chains 45/45; paired library clear 44;
// member-one chain 44; both chains 47; scale before reciprocal 51;
// scale first plus late clear chain 54.
// Cell-size value 49; origin field reference 57; both 65; segment initializer 42;
// cell/segment 49; origin/segment 57; cell/origin/segment 65; cell-size reference 42.
// @retail 0x3028c0
void walker_3028c0::function_3028c0(input_3028c0 const *input_, long arg_1, long arg_2)
{
	input = input_;
	grid = (*input_->holder)->grid;
	field_24 = arg_1;
	field_28 = arg_2;
	count = (*input_->counted)->count();
	field_1c = 1.0f;
	field_20 = 1.0f;
	walk_3028c0 walk;
	walk.field_0 = _mm_setzero_ps();
	inverse_cell_size = 1.0f / grid->origin.m128_f32[3];
	float const scale = grid->origin.m128_f32[3] * 1.52587890625e-05f;
	walk.scale = scale;
	walk.direction = _mm_mul_ps(splat_3028c0(scale), input_->direction);
	char const *const &cells = grid->cells;
	walk.length = sum_3028c0(input_->direction) * 3.0f;
	segment_3028c0 segment;
	__m128 const origin = grid->origin;
	segment.start = _mm_sub_ps(input_->start, origin);
	segment.end = _mm_sub_ps(input_->end, origin);
	segment.start = _mm_mul_ps(splat_3028c0(scale), segment.start);
	segment.end = _mm_mul_ps(splat_3028c0(scale), segment.end);
	walk.field_2c = 0;
	walk.field_24 = 0;
	walk.field_30 = 0;
	((class_301a60 *)this)->function_301a60(&walk, cells, &segment);
}

class class_3029d0_owner
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void release_id(unsigned short id) = 0;
};

struct s_3029d0_entry
{
	char unknown0[0x1c];
	unsigned short id;
	char unknown1e[0x12];
};

// @retail 0x3029d0
void __cdecl function_3029d0(s_3029d0_entry *entries, int *count, class_3029d0_owner *owner)
{
	while (*count != 0)
	{
		--*count;
		owner->release_id(entries[*count].id);
	}
}
