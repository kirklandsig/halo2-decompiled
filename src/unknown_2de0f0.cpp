// @flags /O2 /GL-
// Havok (third-party physics library): the bounding box of a segment with a
// radius, placed by a transform.
#include <xmmintrin.h>

struct transform_2de0f0
{
	__m128 axes[3];
	__m128 position;
};

// a vector updated in place
struct vector_2de0f0
{
	__m128 value;

	void set_max(__m128 const &a, __m128 const &b) { value = _mm_max_ps(a, b); }
	void set_min(__m128 const &a, __m128 const &b) { value = _mm_min_ps(a, b); }
	void add(__m128 const &a) { value = _mm_add_ps(value, a); }
	void subtract(__m128 const &a) { value = _mm_sub_ps(value, a); }
};

struct bounds_2de0f0
{
	vector_2de0f0 lower;
	vector_2de0f0 upper;
};

class rounded_segment_2de0f0
{
public:
	long field_0;
	long field_4;
	long field_8;
	float radius;
	__m128 ends[2];

	void function_2de0f0(transform_2de0f0 const &transform, float tolerance, bounds_2de0f0 &out) const;
};

// both ends placed by the transform, the box around them grown by the radius
// and the tolerance
// score 7 (was 7): frame 0x40 and the point loop agree; upper/lower result registers,
// output-pointer load and stores differ. Value inputs to maximum/minimum/both, growth/both
// operations 7/7/7/7/7; indexed endpoints 39; minimum first 7; late growth 52; local
// minimum/maximum/both 61/88/135; vector point records 7. NOT kept: ties or larger code.
// @retail 0x2de0f0
void rounded_segment_2de0f0::function_2de0f0(transform_2de0f0 const &transform, float tolerance, bounds_2de0f0 &out) const
{
	__m128 const grow = _mm_set1_ps(tolerance + radius);
	__m128 points[2];
	__m128 const *end = ends;
	__m128 *point = points;
	for (int i = 1; i >= 0; i--)
	{
		__m128 const p = *end;
		*point = _mm_add_ps(_mm_add_ps(
			_mm_mul_ps(transform.axes[0], _mm_shuffle_ps(p, p, 0x00)),
			_mm_mul_ps(transform.axes[1], _mm_shuffle_ps(p, p, 0x55))),
			_mm_add_ps(_mm_mul_ps(transform.axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform.position));
		end++;
		point++;
	}
	out.upper.set_max(points[0], points[1]);
	out.lower.set_min(points[0], points[1]);
	out.lower.subtract(grow);
	out.upper.add(grow);
}
