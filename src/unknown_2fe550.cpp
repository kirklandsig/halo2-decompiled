// @flags /O2 /GL-
// Havok (third-party physics library): casting a segment against a box
// centred on the origin, clipping it against each face it crosses.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// the one-byte answer of a cast, returned through a hidden pointer
struct answer_2fe550
{
	char value;

	answer_2fe550(bool value_) : value((char)value_) {}
};

struct segment_2fe550
{
	__m128 start;
	__m128 end;
};

// where a cast hit: the surface normal, the part hit and the fraction of the
// segment travelled
struct hit_2fe550
{
	__m128 normal;
	long part;
	float fraction;
};

class box_2fe550
{
public:
	char unknown0[0x10];
	__m128 half_extents;

	answer_2fe550 function_2fe550(segment_2fe550 const *segment, hit_2fe550 *hit) const;
};

// a segment starting inside the box misses; otherwise the faces are taken
// first on the positive side, then on the negative side, each one the start
// lies outside of moving the entry and each other one moving the exit
// score 42 (was 73): the axis loop counts one byte offset (8, 4, 0) that every access uses, and
// steps it in both paths (after an axis that is not crossed, and at the end of the body), which
// keeps it the only counter; the limit is read once per axis and the leaving test is
// `!(leave < fraction)`; `leave` is declared last. Left: retail fuses the step with the sign test
// (`sub ecx, 4; jns`), ours tests again, two bytes longer.
// Before, 73 (was 141): retail walks the three axes by their byte offset, so the
// offset is the loop's one counter and every access is a plain displacement;
// ours still derives the axis from the offset and scales it, and our four
// masks and the frame's slots for the normal and the negated extents sit in
// other registers and other places than retail's.
// normal name and early declaration 73; component loop with renamed normal 141;
// shared face limit with component loop 202, exit operands 206; early normal
// with byte counter and shared limit 124, exit operands 125, limit first 124;
// scalar normal lanes 108, component loop 148, extent lanes 118, both 160.
// endpoint scalar arrays 73, component loop 141; endpoint lane pointers
// 129, component loop 154; shared axis bit 196, endpoint arrays 196, common
// limit 123; later bit table 75; earlier sign 73, later exit load 71 (dropped);
// explicit endpoint vector loads 73; shared bit with byte counter 73.
// Reference arguments 73; SIMD/scalar vector unions 141; vector references
// with one extent load 202; one result exit 110; post-decrement axis 140;
// long axis and guarded body with one extent load 202; outside mask
// initialized earlier 104; unsigned masks and table 73.
// @retail 0x2fe550
answer_2fe550 box_2fe550::function_2fe550(segment_2fe550 const *segment, hit_2fe550 *hit) const
{
	__m128 const extents = half_extents;
	__m128 const negative_extents = _mm_xor_ps(extents, *(__m128 const *)g_4124a0);
	int const start_above = _mm_movemask_ps(_mm_cmple_ps(extents, segment->start)) & 7;
	int const start_below = _mm_movemask_ps(_mm_cmple_ps(segment->start, negative_extents)) & 7;
	int const end_above = _mm_movemask_ps(_mm_cmple_ps(extents, segment->end)) & 7;
	int const end_below = _mm_movemask_ps(_mm_cmple_ps(segment->end, negative_extents)) & 7;
	if ((start_above & end_above) != 0 || (start_below & end_below) != 0 || (start_below | start_above) == 0)
	{
		return answer_2fe550(false);
	}
	__m128 normal;
	int const axis_bits[3] = {1, 2, 4};
	float enter = 0.0f;
	float side = 1.0f;
	int crossed = end_above | start_above;
	int outside = start_above;
	float leave = hit->fraction;
	for (int pass = 1; pass >= 0; pass--)
	{
		for (int offset = 8; offset >= 0;)
		{
			if ((crossed & *(int const *)((char const *)&axis_bits + offset)) == 0)
			{
				offset -= 4;
				continue;
			}
			float const limit = *(float const *)((char const *)&negative_extents + offset);
			float const start_distance = side * *(float const *)((char const *)&segment->start + offset) + limit;
			float const end_distance = side * *(float const *)((char const *)&segment->end + offset) + limit;
			float const fraction = start_distance / (start_distance - end_distance);
			if ((outside & *(int const *)((char const *)&axis_bits + offset)) != 0)
			{
				if (fraction >= enter)
				{
					enter = fraction;
					normal = _mm_setzero_ps();
					*(float *)((char *)&normal + offset) = side;
				}
			}
			else if (!(leave < fraction))
			{
				leave = fraction;
			}
			if (leave < enter)
			{
				return answer_2fe550(false);
			}
			offset -= 4;
		}
		side = -1.0f;
		crossed = end_below | start_below;
		outside = start_below;
	}
	hit->normal = normal;
	hit->fraction = enter;
	hit->part = -1;
	return answer_2fe550(true);
}
