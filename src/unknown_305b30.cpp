// @flags /O2 /GL-
// Havok (third-party physics library): a box placed by a transform: the
// absolute difference of two of its axes' rows scaled by each other's half
// extent, and one of its corners from a feature key.
#include "box_corners_413610.h"
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long absolute_mask_305b30[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

__declspec(align(16)) const float g_corner_signs_413610[8][4] = {
	{1.0f, 1.0f, 1.0f, 0.0f},
	{-1.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 1.0f, 0.0f},
	{-1.0f, -1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, -1.0f, 0.0f},
	{-1.0f, 1.0f, -1.0f, 0.0f},
	{1.0f, -1.0f, -1.0f, 0.0f},
	{-1.0f, -1.0f, -1.0f, 0.0f},
};

// a vector of four copies of a float
inline __m128 splat_305b30(float value)
{
	__m128 const single = _mm_set_ss(value);
	return _mm_shuffle_ps(single, single, 0);
}

struct vector_305b30
{
	__m128 value;

	void set(float x, float y, float z)
	{
		float *components = (float *)&value;
		components[0] = x;
		components[1] = y;
		components[2] = z;
		components[3] = 0.0f;
	}

	void set_product(__m128 const &a, __m128 const &b)
	{
		value = _mm_mul_ps(a, b);
	}
};

class box_305b30
{
public:
	char unknown0[0x70];
	__m128 axes[3];
	__m128 position;
	float half_extents[4];

	void function_305b30(__m128 *out, long i, long j) const;
};

// |half_extents[j] * row i - half_extents[i] * row j| of the axes
// @retail 0x305b30
void box_305b30::function_305b30(__m128 *out, long i, long j) const
{
	vector_305b30 row;
	row.set(axes[0].m128_f32[i], axes[1].m128_f32[i], axes[2].m128_f32[i]);
	__m128 const scaled_i = _mm_mul_ps(splat_305b30(half_extents[j]), row.value);
	row.set(axes[0].m128_f32[j], axes[1].m128_f32[j], axes[2].m128_f32[j]);
	__m128 const scaled_j = _mm_mul_ps(splat_305b30(half_extents[i]), row.value);
	*out = _mm_sub_ps(scaled_i, scaled_j);
	*out = _mm_and_ps(_mm_sub_ps(scaled_i, scaled_j), *(__m128 const *)absolute_mask_305b30);
}

// a box's corner key: an id, then a word whose low byte's high nibble picks
// the corner and whose high byte's low nibble is kept in the vertex
struct corner_key_305bf0
{
	unsigned short id;
	union
	{
		unsigned short word;
		struct
		{
			unsigned char corner;
			unsigned char flags;
		};
	};
};

struct corner_305bf0
{
	__m128 world;
	vector_305b30 local;
	char unknown20[0x10];
	float tagged_one;
	long field_34;
	unsigned short id;
};

class box_corner_305bf0
{
public:
	char unknown0[0x30];
	__m128 half_extents;
	char unknown40[0x30];
	__m128 axes[3];
	__m128 position;

	void function_305bf0(corner_305bf0 *out, corner_key_305bf0 const *key) const;

	// a point placed by the transform
	void place(__m128 *out, __m128 const *point) const
	{
		__m128 const p = _mm_load_ps((float const *)point);
		*out = _mm_add_ps(_mm_add_ps(
			_mm_mul_ps(axes[0], _mm_shuffle_ps(p, p, 0x00)),
			_mm_mul_ps(axes[1], _mm_shuffle_ps(p, p, 0x55))),
			_mm_add_ps(_mm_mul_ps(axes[2], _mm_shuffle_ps(p, p, 0xaa)), position));
	}
};

// out: the corner the key picks, in the box's frame and placed by its
// transform; 1.0 with the key's high nibble in its top four bits; the id
// @retail 0x305bf0
void box_corner_305bf0::function_305bf0(corner_305bf0 *out, corner_key_305bf0 const *key) const
{
	out->id = key->id;
	float tagged = 1.0f;
	*(long *)&tagged |= (key->word >> 8) << 28;
	out->tagged_one = tagged;
	out->local.set_product(half_extents, *(__m128 const *)((char const *)g_corner_signs_413610 + (key->corner & 0xf0)));
	place(&out->world, &out->local.value);
}

// Havok (third-party physics library): a contact from a feature of the
// second object: the feature's point in world space, and the normal made
// from the object's direction for the feature scaled by the reversed
// distance.
#include <xmmintrin.h>

struct transform_305ca0
{
	__m128 rotation[3];
	__m128 translation;
};

struct key_305ca0
{
	unsigned short index;
	unsigned short field_2;
	unsigned short id;
};

struct feature_305ca0
{
	char pad_0[0x10];
	__m128 point;
	char pad_20[0x10];
	float distance;
	long field_34;
};

struct contact_305ca0
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

class pair_305ca0
{
public:
	char pad_0[0x14];
	__m128 *directions;
	transform_305ca0 *transform;

	void function_305ca0(contact_305ca0 *out, key_305ca0 const *key, feature_305ca0 const *feature) const;
};

// @retail 0x305ca0
void pair_305ca0::function_305ca0(contact_305ca0 *out, key_305ca0 const *key, feature_305ca0 const *feature) const
{
	int const index = key->index;
	out->id = key->id;
	transform_305ca0 const *t = transform;
	__m128 const p = feature->point;
	out->point = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(t->rotation[0], _mm_shuffle_ps(p, p, 0)),
		_mm_mul_ps(t->rotation[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_add_ps(_mm_mul_ps(t->rotation[2], _mm_shuffle_ps(p, p, 0xaa)), t->translation));
	__m128 const scalar = _mm_set_ss(-feature->distance);
	__m128 const &direction = directions[index];
	out->normal = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), direction);
	((long *)&out->point)[3] = feature->field_34;
}

// Havok (third-party physics library): a corner of one box from a feature
// key, in its own frame and taken into the frame of the other box.
#include "box_corners_413610.h"
#include <xmmintrin.h>

// a box's corner key: an id, then a word whose low byte's high nibble picks
// the corner and whose high byte's low nibble is kept in the vertex
struct corner_key_305d40
{
	unsigned short id;
	union
	{
		unsigned short word;
		struct
		{
			unsigned char corner;
			unsigned char flags;
		};
	};
};

struct corner_305d40
{
	__m128 own;
	__m128 other;
	char unknown20[0x10];
	float tagged_one;
	long field_34;
	unsigned short id;
};

class box_pair_305d40
{
public:
	char unknown0[0x20];
	__m128 half_extents;
	char unknown30[0x40];
	__m128 axes[3];
	__m128 position;

	void function_305d40(corner_305d40 *out, corner_key_305d40 const *key) const;
};

// out: the corner the key picks, in the box's frame, and taken into the
// other frame by the inverse of the transform; 1.0 with the key's high nibble
// in its top four bits; the id
// @retail 0x305d40
void box_pair_305d40::function_305d40(corner_305d40 *out, corner_key_305d40 const *key) const
{
	out->id = key->id;
	float tagged = 1.0f;
	*(long *)&tagged |= (key->word >> 8) << 28;
	out->tagged_one = tagged;
	out->own = _mm_mul_ps(half_extents, *(__m128 const *)((char const *)g_corner_signs_413610 + (key->corner & 0xf0)));
	__m128 const d = _mm_sub_ps(out->own, position);
	__m128 row0 = axes[0];
	__m128 row1 = axes[1];
	__m128 row2 = axes[2];
	__m128 row3 = row2;
	_MM_TRANSPOSE4_PS(row0, row1, row2, row3);
	out->other = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(row0, _mm_shuffle_ps(d, d, 0x00)),
		_mm_mul_ps(row1, _mm_shuffle_ps(d, d, 0x55))),
		_mm_mul_ps(row2, _mm_shuffle_ps(d, d, 0xaa)));
}

// Havok (third-party physics library): a contact from a feature of the
// first object: the feature's point in world space, and the normal made from
// the other object's direction for the feature (numbered after the first
// three) scaled by the reversed distance.
#include <xmmintrin.h>

struct transform_305e10
{
	__m128 rotation[3];
	__m128 translation;
};

struct key_305e10
{
	unsigned short index;
	unsigned short field_2;
	unsigned short id;
};

struct feature_305e10
{
	__m128 point;
	char pad_10[0x20];
	float distance;
	long field_34;
};

struct contact_305e10
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

class pair_305e10
{
public:
	char pad_0[0x14];
	transform_305e10 *transform;
	__m128 *directions;

	void function_305e10(contact_305e10 *out, key_305e10 const *key, feature_305e10 const *feature) const;
};

// @retail 0x305e10
void pair_305e10::function_305e10(contact_305e10 *out, key_305e10 const *key, feature_305e10 const *feature) const
{
	int const index = key->index - 3;
	out->id = key->id;
	transform_305e10 const *t = transform;
	__m128 const p = feature->point;
	out->point = _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(t->rotation[0], _mm_shuffle_ps(p, p, 0)),
		_mm_mul_ps(t->rotation[1], _mm_shuffle_ps(p, p, 0x55))),
		_mm_add_ps(_mm_mul_ps(t->rotation[2], _mm_shuffle_ps(p, p, 0xaa)), t->translation));
	__m128 const scalar = _mm_set_ss(-feature->distance);
	__m128 const &direction = directions[index];
	out->normal = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), direction);
	((long *)&out->point)[3] = feature->field_34;
}

// Havok (third-party physics library): bringing a box pair's kept contacts
// up to date: each kept feature (a corner of one box against a face of the
// other, or two edges) is worked out again, and passed on as a contact while
// it still lies within the bounds; otherwise its id goes back to the owner
// and it leaves the manifold.
#include <xmmintrin.h>
#include "growable_array_2d91d0.h"

static __declspec(align(16)) const unsigned long absolute_mask_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

struct s_308180_entry
{
	unsigned short first;
	unsigned short second;
	unsigned short field_4;
};

// a manifold of up to eight contacts (0x308180)
class class_308180
{
public:
	char unknown0[0x20];
	s_308180_entry entries[8];
	long field_50;
	int count;
	bool changed;

	void remove(int index, long a, long b);
};

// the closest points of two features, a scale, their distance, and the
// features' keys
struct feature_3072f0
{
	__m128 first_point;
	__m128 second_point;
	__m128 normal;
	float scale;
	float distance;
	unsigned short kind;
	unsigned short field_3a;
};

struct bool_3058b0
{
	bool value;
};

struct corner_305bf0;
struct corner_key_305bf0;
struct corner_305d40;
struct corner_key_305d40;
struct contact_305ca0;
struct key_305ca0;
struct feature_305ca0;
struct contact_305e10;
struct key_305e10;
struct feature_305e10;
struct contact_305eb0;
struct key_305eb0;
struct feature_305eb0;
struct feature_305f60;

class pair_305eb0
{
public:
	void function_305eb0(contact_305eb0 *out, key_305eb0 const *key, feature_305eb0 const *feature) const;
};

class pair_305f60
{
public:
	bool_3058b0 function_306f80(feature_305f60 *feature);
};

struct contact_3072f0
{
	char unknown0[0x30];
};

ELEMENTS_2D91D0_ALIGNED(contacts_3072f0, contact_3072f0);

struct results_3072f0
{
	contacts_3072f0 *contacts;
};

class owner_3072f0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void release_id(unsigned short id) {}
};

// a pair of boxes: their half extents and the bounds a kept feature must
// stay within
class box_pair_3072f0
{
public:
	long field_0;
	long field_4;
	long field_8;
	owner_3072f0 *owner;
	results_3072f0 *results;
	char unknown14[0x20 - 0x14];
	__m128 first_extents;
	__m128 second_extents;
	__m128 first_bounds;
	__m128 second_bounds;

	void function_3072f0(class_308180 *manifold);
};

// Matched (was 12): compiled in one file after the four functions that receive the feature's
// address (0x305bf0, 0x305ca0, 0x305d40, 0x305e10), the distance store sinks to just before the
// branch; the entry is addressed from the index at the top of each pass, and each arm that keeps
// its entry steps to the next one itself.
// @retail 0x3072f0
void box_pair_3072f0::function_3072f0(class_308180 *manifold)
{
	for (int i = 0; i < manifold->count;)
	{
		s_308180_entry *entry = &manifold->entries[i];
		unsigned short const kind = entry->first;
		int const type = kind;
		if (type <= 2)
		{
			feature_3072f0 feature;
			((box_corner_305bf0 const *)this)->function_305bf0((corner_305bf0 *)&feature, (corner_key_305bf0 const *)entry);
			if ((_mm_movemask_ps(_mm_cmplt_ps(_mm_and_ps(feature.first_point, *(__m128 const *)absolute_mask_455940), first_bounds)) & 7) != 7)
			{
				goto drop;
			}
			int const axis = feature.kind;
			feature.distance = feature.scale * feature.first_point.m128_f32[axis] - first_extents.m128_f32[axis];
			contact_3072f0 *contact = &results->contacts->next_slot();
			((pair_305ca0 const *)this)->function_305ca0((contact_305ca0 *)contact, (key_305ca0 const *)entry, (feature_305ca0 const *)&feature);
			i++;
			continue;
		}
		else if (type <= 5)
		{
			feature_3072f0 feature;
			((box_pair_305d40 const *)this)->function_305d40((corner_305d40 *)&feature, (corner_key_305d40 const *)entry);
			if ((_mm_movemask_ps(_mm_cmplt_ps(_mm_and_ps(feature.second_point, *(__m128 const *)absolute_mask_455940), second_bounds)) & 7) != 7)
			{
				goto drop;
			}
			int const axis = feature.kind - 3;
			feature.distance = -(feature.scale * feature.second_point.m128_f32[axis]) - second_extents.m128_f32[axis];
			contact_3072f0 *contact = &results->contacts->next_slot();
			((pair_305e10 const *)this)->function_305e10((contact_305e10 *)contact, (key_305e10 const *)entry, (feature_305e10 const *)&feature);
			i++;
			continue;
		}
		else
		{
			feature_3072f0 feature;
			feature.kind = kind;
			feature.field_3a = entry->second;
			if (((pair_305f60 *)this)->function_306f80((feature_305f60 *)&feature).value)
			{
				contact_3072f0 *contact = &results->contacts->next_slot();
				((pair_305eb0 const *)this)->function_305eb0((contact_305eb0 *)contact, (key_305eb0 const *)entry, (feature_305eb0 const *)&feature);
				i++;
				continue;
			}
			else
			{
				goto drop;
			}
		}
	drop:
		if (entry->first <= 5)
		{
			manifold->field_50--;
		}
		owner->release_id(manifold->entries[i].field_4);
		manifold->remove(i, field_0, field_4);
	}
}
