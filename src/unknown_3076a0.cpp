// @flags /O2 /GL-
// Havok (third-party physics library): adding a contact for a pair of
// features to a manifold unless it is listed already, or found too far; and
// adding the contacts of a pair of box edges and their neighbours.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
static __declspec(align(16)) const unsigned long g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

// for each edge-edge axis (6 to 14), the axis of the first box's edge and of
// the second's
static const long g_4135c0[9] = {0, 1, 2, 0, 1, 2, 0, 1, 2};
static const long g_4135e4[9] = {0, 0, 0, 1, 1, 1, 2, 2, 2};

struct bool_3058b0
{
	bool value;

	bool_3058b0(bool value_)
	{
		value = value_;
	}
};

struct s_3058b0_pair;

class class_3058b0
{
public:
	bool_3058b0 contains(const s_3058b0_pair *pair) const;
};


struct key_305f60
{
	unsigned short index;
	unsigned short field_2;
	unsigned short id;
};

// the features a manifold holds already, and how many
class table_305f60
{
public:
	char unknown0[0x20];
	key_305f60 keys[8];
	long field_50;
	long count;
	bool full;
};

// the closest points of two features, their distance, and the features
struct feature_305f60
{
	__m128 first_point;
	__m128 second_point;
	char unknown20[0x30 - 0x20];
	float side;
	float distance;
	unsigned short kind;
	unsigned short field_3a;
};

struct face_306b90;
struct edge_pair_306e70;

class box_306b90
{
public:
	void function_306b90(face_306b90 *face, int axis) const;
	void function_306d00(face_306b90 *face, int axis) const;
};

class box_pair_306e70
{
public:
	void function_306e70(edge_pair_306e70 *out) const;
};

// the pair of boxes: each box's half extents, the bounds a face's contact
// point must lie within, and the separation found along each of the fifteen
// axes
struct transform_307840
{
	__m128 axes[3];
	__m128 position;
};

// a contact the pair keeps: its distance and normal
struct contact_307840
{
	char unknown0[0xc];
	float distance;
	__m128 normal;
	char unknown20[0x10];
};

struct contacts_307840
{
	contact_307840 *data;
	long size;
};

class manifold_305790;

class pair_305790
{
public:
	void function_305790(manifold_305790 *manifold, long first_flags, long second_flags);
};

// whether the contacts of the edges beside a face are added too
bool g_480399;

class pair_305f60
{
public:
	char unknown0[0x10];
	contacts_307840 **contacts;
	transform_307840 const *first_transform;
	transform_307840 const *second_transform;
	char unknown1c[0x20 - 0x1c];
	float first_extents[4];
	float second_extents[4];
	__m128 first_bounds;
	__m128 second_bounds;
	char unknown60[0xb0 - 0x60];
	float first_sides[4];
	float second_sides[4];
	float separations[15];

	void function_305f60(table_305f60 *table, feature_305f60 const *feature, key_305f60 *key);
	bool_3058b0 function_306f80(feature_305f60 *feature);
	void function_3076a0(table_305f60 *table, key_305f60 *key, float distance);
	void function_307720(table_305f60 *table, int first_axis, int second_axis, int first_edge, int second_edge,
		__m128 const *first_corner, __m128 const *second_corner, float distance);
	void function_3061c0(table_305f60 *table, key_305f60 key, unsigned short mask, float limit);
	void function_3062e0(table_305f60 *table, key_305f60 key, unsigned short mask, float limit);
	void function_307840(table_305f60 *table, key_305f60 key);
	int function_307c40(table_305f60 *table, key_305f60 *key, feature_305f60 *feature);
};

// @retail 0x3076a0
void pair_305f60::function_3076a0(table_305f60 *table, key_305f60 *key, float distance)
{
	if (((class_3058b0 *)table)->contains((s_3058b0_pair const *)key).value)
	{
		return;
	}
	feature_305f60 feature;
	feature.kind = key->index;
	feature.field_3a = key->field_2;
	if (!function_306f80(&feature).value)
	{
		return;
	}
	if (distance * 0.999f <= feature.distance)
	{
		function_305f60(table, &feature, key);
	}
}

// which of three edges a value names: 0, 1, or 2 for anything else
inline int edge_307720(int value)
{
	if (value == 0)
	{
		return value;
	}
	return value == 1 ? 1 : 2;
}

// the contacts of an edge of each box, named by the corners they start from
// with the edge's axis set, and of the edges beside them
// @retail 0x307720
void pair_305f60::function_307720(table_305f60 *table, int first_axis, int second_axis, int first_edge, int second_edge,
	__m128 const *first_corner, __m128 const *second_corner, float distance)
{
	__m128 first = *first_corner;
	__m128 second = *second_corner;
	first.m128_f32[first_axis] = 1.0f;
	second.m128_f32[second_axis] = 1.0f;
	key_305f60 key;
	key.index = (unsigned short)((((_mm_movemask_ps(first) & 7) + 0x400) << 4) + first_axis);
	key.id = 0;
	key.field_2 = (unsigned short)(((~_mm_movemask_ps(second) & 7) << 4) + second_axis);
	function_3076a0(table, &key, distance);
	unsigned short const first_flip = (unsigned short)(1 << (edge_307720(first_edge) + 4));
	int const second_bit = edge_307720(second_edge);
	key.index ^= first_flip;
	function_3076a0(table, &key, distance);
	key.field_2 ^= (unsigned short)(1 << (second_bit + 4));
	function_3076a0(table, &key, distance);
	key.index ^= first_flip;
	function_3076a0(table, &key, distance);
}

// a direction taken into a transform's frame
__forceinline __m128 rotate_inverse_307840(transform_307840 const *matrix, __m128 const &direction)
{
	__m128 const x = _mm_shuffle_ps(direction, direction, 0x00);
	__m128 const y = _mm_shuffle_ps(direction, direction, 0x55);
	__m128 const z = _mm_shuffle_ps(direction, direction, 0xaa);
	__m128 t0 = _mm_shuffle_ps(matrix->axes[0], matrix->axes[1], 0x44);
	__m128 t1 = _mm_shuffle_ps(matrix->axes[2], matrix->axes[2], 0x44);
	__m128 t2 = _mm_shuffle_ps(matrix->axes[0], matrix->axes[1], 0xee);
	__m128 t3 = _mm_shuffle_ps(matrix->axes[2], matrix->axes[2], 0xee);
	return _mm_add_ps(_mm_add_ps(
		_mm_mul_ps(_mm_shuffle_ps(t0, t1, 0x88), x),
		_mm_mul_ps(_mm_shuffle_ps(t0, t1, 0xdd), y)),
		_mm_mul_ps(_mm_shuffle_ps(t2, t3, 0x88), z));
}

// score 316 (was 316): retail computes the newest normal's three splats once,
// spills them and shares them between the two rotations, and keeps the count
// in edi; sharing the splats here scores 322 to 326.
// the faces of the two boxes most nearly facing along the normal of the
// newest contacts, when both face it closely: their contacts and, when asked,
// those of the edges around them, with the smallest distance among the
// contacts
// @retail 0x307840
void pair_305f60::function_307840(table_305f60 *table, key_305f60 key)
{
	int const count = table->count;
	if (count < 1)
	{
		return;
	}
	contacts_307840 const *contacts = *this->contacts;
	contact_307840 const *newest = &contacts->data[contacts->size - count];
	__m128 second = rotate_inverse_307840(second_transform, newest->normal);
	__m128 first = _mm_xor_ps(rotate_inverse_307840(first_transform, newest->normal), *(__m128 const *)g_4124a0);
	__m128 first_size = _mm_and_ps(first, *(__m128 const *)g_455940);
	__m128 second_size = _mm_and_ps(second, *(__m128 const *)g_455940);
	int first_axis;
	int first_mask;
	if (first_size.m128_f32[0] > first_size.m128_f32[1])
	{
		first_axis = 0;
		first_mask = 0x10;
	}
	else
	{
		first_axis = 1;
		first_mask = 0x20;
	}
	if (first_size.m128_f32[2] > first_size.m128_f32[first_axis])
	{
		first_axis = 2;
		first_mask = 0x40;
	}
	int second_axis;
	int second_mask;
	if (second_size.m128_f32[0] > second_size.m128_f32[1])
	{
		second_axis = 0;
		second_mask = 0x10;
	}
	else
	{
		second_axis = 1;
		second_mask = 0x20;
	}
	if (second_size.m128_f32[2] > second_size.m128_f32[second_axis])
	{
		second_axis = 2;
		second_mask = 0x40;
	}
	if (first_size.m128_f32[first_axis] < 0.98f || second_size.m128_f32[second_axis] < 0.98f)
	{
		return;
	}
	float first_side = first_sides[first_axis];
	key.field_2 = (unsigned short)(((_mm_movemask_ps(second) & 7) << 4) | ((*(unsigned long const *)&first_side >> 20) & 0x800));
	float second_side = second_sides[second_axis];
	key_305f60 second_key;
	second_key.field_2 = (unsigned short)(((_mm_movemask_ps(first) & 7) << 4) | ((*(unsigned long const *)&second_side >> 20) & 0x800));
	key.index = (unsigned short)first_axis;
	second_key.index = (unsigned short)(second_axis + 3);
	float distance = 1.0f;
	for (int i = 0; i < table->count; i++)
	{
		float const contact_distance = newest[i].distance;
		if (contact_distance < distance)
		{
			distance = contact_distance;
		}
	}
	function_3061c0(table, key, (unsigned short)second_mask, distance);
	second_key.id = 0;
	function_3062e0(table, second_key, (unsigned short)first_mask, distance);
	if (table->field_50 >= 4)
	{
		table->full = true;
		return;
	}
	if (g_480399)
	{
		int const first_edge_0 = g_4135c0[first_axis + 1];
		int const first_edge_1 = g_4135c0[first_axis + 2];
		int const second_edge_0 = g_4135c0[second_axis + 1];
		int const second_edge_1 = g_4135c0[second_axis + 2];
		second = _mm_xor_ps(second, *(__m128 const *)g_4124a0);
		function_307720(table, first_edge_0, second_edge_0, first_edge_1, second_edge_1, &first, &second, distance);
		function_307720(table, first_edge_0, second_edge_1, first_edge_1, second_edge_0, &first, &second, distance);
		function_307720(table, first_edge_1, second_edge_0, first_edge_0, second_edge_1, &first, &second, distance);
		function_307720(table, first_edge_1, second_edge_1, first_edge_0, second_edge_0, &first, &second, distance);
	}
	((pair_305790 *)this)->function_305790((manifold_305790 *)table, first_mask, second_mask);
}
// score 24 (was 24): Frame 0x44 and extent 847 agree; the returned byte is
// below the saved this instead of above the two side inputs. The remaining
// 4-byte stack homes are shifted by four. A common result and sole exit fix
// both distance-store/return orders, confirmed by the plain check.
// Earlier: a temporary or early flag 51; flag rename unchanged.
// SIMD buffers before count 24, before search 24; distinct side names 24;
// shared side 31; face-index captures 156; backward key pointers 204;
// named distances 24; common exit 19; direct byte test with it 47;
// count before result 19; outer returned object with it 47.
// the features of the axis of greatest separation, tried in turn up to eight
// times: 1 when the manifold lists them already, 2 when they give a new
// contact (its separation in the feature), 0 when none does
// Codex T round 87, NOT kept (coordinator 11-11): 24 -> 19 only from a result local and new `goto done` exits in place of the returns (a new goto is kept only with a byte match); body at 24.
// @retail 0x307c40
int pair_305f60::function_307c40(table_305f60 *table, key_305f60 *key, feature_305f60 *feature)
{
	int tries = 8;
	do
	{
		float best_separation = separations[0];
		int best = 0;
		for (int i = 1; i < 15; i++)
		{
			if (best_separation < separations[i])
			{
				best_separation = separations[i];
				best = i;
			}
		}
		if (best <= 2)
		{
			key->index = (unsigned short)best;
			((box_306b90 const *)this)->function_306b90((face_306b90 *)feature, best);
			float const side = feature->side;
			unsigned short const field_2 = (unsigned short)(((_mm_movemask_ps(feature->second_point) & 7) << 4) | ((*(unsigned long const *)&side >> 20) & 0x800));
			key->field_2 = field_2;
			for (int j = table->count - 1; j >= 0; j--)
			{
				if (table->keys[j].index == key->index && table->keys[j].field_2 == field_2)
				{
					return 1;
				}
			}
			if ((_mm_movemask_ps(_mm_cmplt_ps(_mm_and_ps(feature->first_point, *(__m128 const *)g_455940), first_bounds)) & 7) == 7)
			{
				int const axis = feature->kind;
				feature->distance = feature->first_point.m128_f32[axis] * feature->side - first_extents[axis];
				return 2;
			}
		}
		else if (best <= 5)
		{
			key->index = (unsigned short)best;
			((box_306b90 const *)this)->function_306d00((face_306b90 *)feature, best - 3);
			float const side = feature->side;
			unsigned short const field_2 = (unsigned short)(((_mm_movemask_ps(feature->first_point) & 7) << 4) | ((*(unsigned long const *)&side >> 20) & 0x800));
			key->field_2 = field_2;
			for (int j = table->count - 1; j >= 0; j--)
			{
				if (table->keys[j].index == key->index && table->keys[j].field_2 == field_2)
				{
					return 1;
				}
			}
			if ((_mm_movemask_ps(_mm_cmplt_ps(_mm_and_ps(feature->second_point, *(__m128 const *)g_455940), second_bounds)) & 7) == 7)
			{
				int const axis = feature->kind - 3;
				feature->distance = -(feature->second_point.m128_f32[axis] * feature->side) - second_extents[axis];
				return 2;
			}
		}
		else
		{
			feature->kind = (unsigned short)g_4135c0[best - 6];
			feature->field_3a = (unsigned short)g_4135e4[best - 6];
			((box_pair_306e70 const *)this)->function_306e70((edge_pair_306e70 *)feature);
			int const first_axis = feature->field_3a;
			int const second_axis = feature->kind;
			__m128 first = feature->first_point;
			__m128 second = feature->second_point;
			first.m128_f32[second_axis] = 1.0f;
			second.m128_f32[first_axis] = 1.0f;
			key->index = (unsigned short)((((_mm_movemask_ps(first) & 7) + 0x400) << 4) + second_axis);
			key->field_2 = (unsigned short)(((~_mm_movemask_ps(second) & 7) << 4) + first_axis);
			feature->kind = key->index;
			feature->field_3a = key->field_2;
			for (int j = table->count - 1; j >= 0; j--)
			{
				if (table->keys[j].index == key->index && table->keys[j].field_2 == key->field_2)
				{
					return 1;
				}
			}
			bool_3058b0 const found = function_306f80(feature);
			if (found.value)
			{
				return 2;
			}
		}
		separations[best] = -3.40282347e+38f;
	} while (--tries > 0);
	return 0;
}
