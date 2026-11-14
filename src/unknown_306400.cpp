// @flags /O2 /GL-
// Havok (third-party physics library): the second box's transform seen from
// the first box, and the first box's centre seen from the second.
#include <xmmintrin.h>

struct transform_306400
{
	__m128 columns[4];
};

__forceinline void __cdecl inverse_vectors_306400(transform_306400 const *transform, __m128 const *input, __m128 *output, int count)
{
	__m128 c0 = transform->columns[0];
	__m128 c1 = transform->columns[1];
	__m128 c2 = transform->columns[2];
	__m128 c3 = c2;
	_MM_TRANSPOSE4_PS(c0, c1, c2, c3);
	for (; --count >= 0;)
	{
		__m128 const d = *input;
		__m128 const z = _mm_mul_ps(c2, _mm_shuffle_ps(d, d, 0xaa));
		__m128 const y = _mm_mul_ps(c1, _mm_shuffle_ps(d, d, 0x55));
		__m128 const x = _mm_mul_ps(c0, _mm_shuffle_ps(d, d, 0x00));
		*output = _mm_add_ps(_mm_add_ps(x, y), z);
		output++;
		input++;
	}
}

struct vector_306400
{
	__m128 value;
	__forceinline void set_inverse_rotated(transform_306400 const *transform, __m128 const *direction)
	{
		inverse_vectors_306400(transform, direction, &value, 1);
	}
};

class box_pair_306400
{
public:
	char unknown0[0x14];
	transform_306400 const *first_transform;
	transform_306400 const *second_transform;
	char unknown1c[0x54];
	vector_306400 relative[4];
	vector_306400 relative_offset;
	vector_306400 first_centre;

	void function_306400();
};

// score 67 (was 67): the offset vector stores and single-vector registers still differ.
// Reference/pointer results 67; transform reference 67; count-first helper 67;
// indexed run 87; whole copies with changed result 136; consumed SSE/reference results 67.
// New: explicit vector copy assignment 68; SSE assignment 75;
// run input reference 67; in-place single rotation 75; source step at
// read 77; destination step at store 67; fixed links or method outside
// class 67. All executable forms restored.
// Aligned field stores or reads 89; library vector copies 136; member
// subtraction 74; typed stored transform with the run inline 67;
// scalar lane view 67; native SIMD temporary 67; result constructors 159.
// All restored.
// @retail 0x306400
void box_pair_306400::function_306400()
{
	inverse_vectors_306400(first_transform, second_transform->columns, &relative[0].value, 4);
	relative_offset.value = relative[3].value;
	vector_306400 first_position;
	first_position.set_inverse_rotated(first_transform, &first_transform->columns[3]);
	relative_offset.value = _mm_sub_ps(relative_offset.value, first_position.value);
	relative[3].value = relative_offset.value;
	first_centre.set_inverse_rotated((transform_306400 const *)relative, &relative_offset.value);
}

static __declspec(align(16)) const unsigned long g_455940[4] = {0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff};

// whether two boxes overlap, returned through a hidden pointer
class answer_306580
{
public:
	answer_306580() {}

	char value;
};

// a vector set from three floats, its last component cleared
struct vector_306580
{
	__m128 value;

	void set(float z, float y, float x)
	{
		float *components = (float *)&value;
		components[0] = x;
		components[1] = y;
		components[2] = z;
		components[3] = 0.0f;
	}
};

// which of x, y and z exceed the value, as bits (src/unknown_305900.cpp)
void __cdecl function_305900(const __m128 *vector, const float *value, int *mask);
// replaces x, y and z with the reciprocals of their square roots
// (src/unknown_305940.cpp)
void __cdecl function_305940(float *vector);
// replaces the components of a vector where another exceeds a value
// (src/unknown_305990.cpp)
void __cdecl function_305990(__m128 *vector, const __m128 *replacement, const __m128 *compare, const __m128 *limit);

// the separation along the cross product of two of the boxes' axes
// (src/unknown_305b30.cpp)
class box_305b30
{
public:
	void function_305b30(__m128 *out, long i, long j) const;
};

// the same comparison as function_305900, written in place
inline void exceeds_306580(const __m128 *vector, const float *value, int *mask)
{
	__m128 limit = _mm_set_ss(*value);
	limit = _mm_shuffle_ps(limit, limit, 0);
	*mask = _mm_movemask_ps(_mm_cmpgt_ps(*vector, limit)) & 7;
}

// two boxes, the second seen from the first, and how far apart they are
// along each of the fifteen axes that can separate them
class class_306580
{
public:
	char unknown0[0x20];
	__m128 first_extents;
	__m128 second_extents;
	char unknown40[0x20];
	float tolerance;
	char unknown64[0xc];
	// the second box's axes in the first box's frame
	__m128 axes[3];
	__m128 unknowna0;
	// the second box's centre in the first box's frame, and the first's in
	// the second's
	__m128 second_centre;
	__m128 first_centre;
	float first_faces[3];
	float second_faces[3];
	float edges[3][3];

	answer_306580 function_306580();
};

// whether the boxes overlap: no face or edge axis separates them by more
// than the tolerance
// score 295 (was 295): frame 0xb8 agrees; the initial mask load, vector
// homes, row reuse and later scalar scheduling differ. Return-mask or
// reference inputs 295; forced comparison or vector setter 295; four-lane
// construction 448; face memcpy 417; result from mask 413; broadcast load
// 451. All executable forms restored.
// Scalar lane view, independent typed columns or typed stored vectors 295;
// build all rows before squaring 293 NOT kept (two-point gain);
// false-first answer 408; shared face/edge workspace 294 NOT kept
// (one-point gain); scalar intrinsic read 444; setter reference result 295.
// All restored.
// @retail 0x306580
answer_306580 class_306580::function_306580()
{
	answer_306580 result;
	__m128 const parallel_limit = _mm_set_ss(999.99994f);
	__m128 const parallel_separation = _mm_set_ss(-1e17f);
	int separated;
	__m128 const mask = *(__m128 const *)g_455940;
	__m128 const a0 = _mm_and_ps(axes[0], mask);
	__m128 const a1 = _mm_and_ps(axes[1], mask);
	__m128 const a2 = _mm_and_ps(axes[2], mask);

	// the first box's faces
	__m128 separation;
	{
		__m128 const second = second_extents;
		__m128 const z = _mm_mul_ps(a2, _mm_shuffle_ps(second, second, 0xaa));
		__m128 const y = _mm_mul_ps(a1, _mm_shuffle_ps(second, second, 0x55));
		__m128 const x = _mm_mul_ps(a0, _mm_shuffle_ps(second, second, 0x00));
		__m128 const reach = _mm_add_ps(_mm_add_ps(_mm_add_ps(x, y), z), first_extents);
		separation = _mm_sub_ps(_mm_and_ps(second_centre, *(__m128 const *)g_455940), reach);
	}
	exceeds_306580(&separation, &tolerance, &separated);
	if (separated)
	{
		result.value = 0;
		goto done;
	}
	first_faces[0] = separation.m128_f32[0];
	first_faces[1] = separation.m128_f32[1];
	first_faces[2] = separation.m128_f32[2];

	// the second box's faces
	{
		__m128 const t0 = _mm_shuffle_ps(a0, a1, 0x44);
		__m128 const t2 = _mm_shuffle_ps(a0, a1, 0xee);
		__m128 const t1 = _mm_shuffle_ps(a2, a2, 0x44);
		__m128 const t3 = _mm_shuffle_ps(a2, a2, 0xee);
		__m128 const row_0 = _mm_shuffle_ps(t0, t1, 0x88);
		__m128 const row_1 = _mm_shuffle_ps(t0, t1, 0xdd);
		__m128 const row_2 = _mm_shuffle_ps(t2, t3, 0x88);
		__m128 const first = first_extents;
		__m128 const z = _mm_mul_ps(row_2, _mm_shuffle_ps(first, first, 0xaa));
		__m128 const y = _mm_mul_ps(row_1, _mm_shuffle_ps(first, first, 0x55));
		__m128 const x = _mm_mul_ps(row_0, _mm_shuffle_ps(first, first, 0x00));
		__m128 const reach = _mm_add_ps(_mm_add_ps(_mm_add_ps(x, y), z), second_extents);
		separation = _mm_sub_ps(_mm_and_ps(first_centre, *(__m128 const *)g_455940), reach);
	}
	exceeds_306580(&separation, &tolerance, &separated);
	if (separated)
	{
		result.value = 0;
		goto done;
	}
	second_faces[0] = separation.m128_f32[0];
	second_faces[1] = separation.m128_f32[1];
	second_faces[2] = separation.m128_f32[2];

	// the edges: the squares of each row of the axes, for the lengths of
	// the cross products
	vector_306580 squares_x;
	squares_x.set(axes[2].m128_f32[0], axes[1].m128_f32[0], axes[0].m128_f32[0]);
	squares_x.value = _mm_mul_ps(squares_x.value, squares_x.value);
	vector_306580 squares_y;
	squares_y.set(axes[2].m128_f32[1], axes[1].m128_f32[1], axes[0].m128_f32[1]);
	squares_y.value = _mm_mul_ps(squares_y.value, squares_y.value);
	vector_306580 squares_z;
	squares_z.set(axes[2].m128_f32[2], axes[1].m128_f32[2], axes[0].m128_f32[2]);
	squares_z.value = _mm_mul_ps(squares_z.value, squares_z.value);
	vector_306580 reach;
	__m128 lengths;
	__m128 edge;

	// the first box's x axis across each of the second's
	((box_305b30 const *)this)->function_305b30(&edge, 1, 2);
	reach.set(a2.m128_f32[2] * first_extents.m128_f32[1] + a2.m128_f32[1] * first_extents.m128_f32[2] + a1.m128_f32[0] * second_extents.m128_f32[0] + a0.m128_f32[0] * second_extents.m128_f32[1],
		a1.m128_f32[2] * first_extents.m128_f32[1] + a1.m128_f32[1] * first_extents.m128_f32[2] + a2.m128_f32[0] * second_extents.m128_f32[0] + a0.m128_f32[0] * second_extents.m128_f32[2],
		a0.m128_f32[2] * first_extents.m128_f32[1] + a0.m128_f32[1] * first_extents.m128_f32[2] + a1.m128_f32[0] * second_extents.m128_f32[2] + a2.m128_f32[0] * second_extents.m128_f32[1]);
	lengths = _mm_add_ps(squares_z.value, squares_y.value);
	function_305940((float *)&lengths);
	edge = _mm_mul_ps(lengths, _mm_sub_ps(edge, reach.value));
	exceeds_306580(&edge, &tolerance, &separated);
	if (separated)
	{
		result.value = 0;
		goto done;
	}
	function_305990(&edge, &parallel_separation, &lengths, &parallel_limit);
	edges[0][0] = edge.m128_f32[0];
	edges[1][0] = edge.m128_f32[1];
	edges[2][0] = edge.m128_f32[2];

	// the first box's y axis across each of the second's
	((box_305b30 const *)this)->function_305b30(&edge, 2, 0);
	reach.set(a1.m128_f32[1] * second_extents.m128_f32[0] + a2.m128_f32[0] * first_extents.m128_f32[2] + a0.m128_f32[1] * second_extents.m128_f32[1] + a2.m128_f32[2] * first_extents.m128_f32[0],
		a0.m128_f32[1] * second_extents.m128_f32[2] + a2.m128_f32[1] * second_extents.m128_f32[0] + a1.m128_f32[0] * first_extents.m128_f32[2] + a1.m128_f32[2] * first_extents.m128_f32[0],
		a1.m128_f32[1] * second_extents.m128_f32[2] + a0.m128_f32[0] * first_extents.m128_f32[2] + a0.m128_f32[2] * first_extents.m128_f32[0] + a2.m128_f32[1] * second_extents.m128_f32[1]);
	lengths = _mm_add_ps(squares_z.value, squares_x.value);
	function_305940((float *)&lengths);
	edge = _mm_mul_ps(lengths, _mm_sub_ps(edge, reach.value));
	exceeds_306580(&edge, &tolerance, &separated);
	if (separated)
	{
		result.value = 0;
		goto done;
	}
	function_305990(&edge, &parallel_separation, &lengths, &parallel_limit);
	edges[0][1] = edge.m128_f32[0];
	edges[1][1] = edge.m128_f32[1];
	edges[2][1] = edge.m128_f32[2];

	// the first box's z axis across each of the second's
	((box_305b30 const *)this)->function_305b30(&edge, 0, 1);
	reach.set(a1.m128_f32[2] * second_extents.m128_f32[0] + a2.m128_f32[0] * first_extents.m128_f32[1] + a0.m128_f32[2] * second_extents.m128_f32[1] + a2.m128_f32[1] * first_extents.m128_f32[0],
		a0.m128_f32[2] * second_extents.m128_f32[2] + a2.m128_f32[2] * second_extents.m128_f32[0] + a1.m128_f32[0] * first_extents.m128_f32[1] + a1.m128_f32[1] * first_extents.m128_f32[0],
		a1.m128_f32[2] * second_extents.m128_f32[2] + a0.m128_f32[0] * first_extents.m128_f32[1] + a0.m128_f32[1] * first_extents.m128_f32[0] + a2.m128_f32[2] * second_extents.m128_f32[1]);
	lengths = _mm_add_ps(squares_y.value, squares_x.value);
	function_305940((float *)&lengths);
	edge = _mm_mul_ps(lengths, _mm_sub_ps(edge, reach.value));
	function_305900(&edge, &tolerance, &separated);
	if (separated)
	{
		result.value = 0;
		goto done;
	}
	function_305990(&edge, &parallel_separation, &lengths, &parallel_limit);
	edges[0][2] = edge.m128_f32[0];
	edges[1][2] = edge.m128_f32[1];
	edges[2][2] = edge.m128_f32[2];
	result.value = 1;
done:
	return result;
}

// the rest of a box pair's query: the contact manifold kept up to date, and
// the deepest contact found
class class_308180
{
public:
	char unknown0[0x58];
	bool changed;
	bool refresh;

	class_308180();
};

struct key_305f60
{
	unsigned short index;
	unsigned short field_2;
	unsigned short id;
};

struct feature_305f60
{
	__m128 first_point;
	__m128 second_point;
	char unknown20[0x38 - 0x20];
	unsigned short kind;
	unsigned short field_3a;
};

class table_305f60;

class pair_305f60
{
public:
	int function_307c40(table_305f60 *table, key_305f60 *key, feature_305f60 *feature);
	void function_305f60(table_305f60 *table, feature_305f60 const *feature, key_305f60 *key);
	void function_307840(table_305f60 *table, key_305f60 key);
};

struct entry_305a60;

// the answer of 0x305a60; the caller keeps it in a four-byte slot
struct bool_305a60
{
	bool value;
	char unknown1[3];
};

class class_305a60
{
public:
	bool_305a60 function_305a60(entry_305a60 *entry);
};

class box_pair_3072f0
{
public:
	void function_3072f0(class_308180 *manifold);
};

class class_307530
{
public:
	void function_307530(class_308180 *contacts);
};

struct feature_3059e0;

// a flag passed by value
struct flag_3059e0
{
	flag_3059e0(bool value_) : value(value_) {}
	flag_3059e0(flag_3059e0 const &other) : value(other.value) {}

	bool value;
};

class box_3059e0
{
public:
	void function_3059e0(__m128 *out, unsigned short const *index, feature_3059e0 const *feature, flag_3059e0 unused) const;
};

struct contact_305ca0
{
	__m128 point;
	__m128 normal;
	unsigned short id;
};

struct key_305ca0;
struct feature_305ca0;

class pair_305ca0
{
public:
	void function_305ca0(contact_305ca0 *out, key_305ca0 const *key, feature_305ca0 const *feature) const;
};

struct contact_305e10;
struct key_305e10;
struct feature_305e10;

class pair_305e10
{
public:
	void function_305e10(contact_305e10 *out, key_305e10 const *key, feature_305e10 const *feature) const;
};

struct contact_305eb0;
struct key_305eb0;
struct feature_305eb0;

class pair_305eb0
{
public:
	void function_305eb0(contact_305eb0 *out, key_305eb0 const *key, feature_305eb0 const *feature) const;
};

struct contact_307f90
{
	__m128 point;
	__m128 normal;
};

// whether a contact was found
class answer_307f90
{
public:
	answer_307f90() {}

	char value;
};

class query_2ef260
{
public:
	void function_3080a0(class_308180 *manifold);
	void function_307f90(answer_307f90 *found, contact_307f90 *contact);
};

// the deepest contact between the boxes, when they overlap; here, in the file
// that compiles 0x306400 first, so the call to 0x306580 reuses this in ecx as
// retail's does
// score 9 (was 11): the point's fourth component is a named `w_component`
// local (lane 3). Output stores and the fresh failure zero still differ.
// score 11 (was 69): retail stores the contact's normal before its point, and
// writes the not-found answer from a fresh zero in cl instead of bl.
// Output stores and fresh failure zero still differ.
// Overlap outcome 74; lookup outcome 62; both 84; common failures 102/69/106;
// both outcomes as bool 84; bool with common failure 106.
// Pointer return 28; pointer return with class assignment 28; pointer return
// with aligned stores 26; empty destructor 28; destructor and mutable-source
// copy 28; destructor and aligned stores 26; mutable-source copy 28;
// class local with empty destructor 11. NOT kept: no improvement.
// @retail 0x307f90
void query_2ef260::function_307f90(answer_307f90 *found, contact_307f90 *contact)
{
	char result = 0;
	long w_component = 3;
	class_308180 manifold;
	key_305f60 key;
	feature_305f60 feature;
	key.id = 0;
	((box_pair_306400 *)this)->function_306400();
	if (((class_306580 *)this)->function_306580().value)
	{
		if (((pair_305f60 *)this)->function_307c40((table_305f60 *)&manifold, &key, &feature) == 2)
		{
		result = 1;
		contact_305ca0 deepest;
		int const kind = feature.kind;
		if (kind <= 2)
		{
			((pair_305ca0 *)this)->function_305ca0(&deepest, (key_305ca0 const *)&key, (feature_305ca0 const *)&feature);
		}
		else if (kind <= 5)
		{
			((pair_305e10 *)this)->function_305e10((contact_305e10 *)&deepest, (key_305e10 const *)&key, (feature_305e10 const *)&feature);
		}
		else
		{
			((pair_305eb0 *)this)->function_305eb0((contact_305eb0 *)&deepest, (key_305eb0 const *)&key, (feature_305eb0 const *)&feature);
		}
		if (key.index > 2)
		{
			float const back = -deepest.point.m128_f32[w_component];
			__m128 along = _mm_set_ss(back);
			along = _mm_shuffle_ps(along, along, 0);
			deepest.point = _mm_add_ps(deepest.point, _mm_mul_ps(along, deepest.normal));
		}
		contact->normal = deepest.normal;
		contact->point = deepest.point;
		contact->point.m128_f32[w_component] = deepest.point.m128_f32[w_component];
		}
		else
		{
			found->value = 0;
			return;
		}
	}
	found->value = result;
}

// the manifold of contacts between the boxes brought up to date
// @retail 0x3080a0
void query_2ef260::function_3080a0(class_308180 *manifold)
{
	((box_pair_306400 *)this)->function_306400();
	((box_pair_3072f0 *)this)->function_3072f0(manifold);
	bool_305a60 const unchanged = ((class_305a60 *)this)->function_305a60((entry_305a60 *)manifold);
	if (manifold->changed && !unchanged.value)
	{
		return;
	}
	if (!((class_306580 *)this)->function_306580().value)
	{
		return;
	}
	key_305f60 key;
	feature_305f60 feature;
	key.id = 0;
	int const found = ((pair_305f60 *)this)->function_307c40((table_305f60 *)manifold, &key, &feature);
	if (unchanged.value && found)
	{
		((box_3059e0 *)this)->function_3059e0((__m128 *)manifold, &key.index, (feature_3059e0 const *)&feature, flag_3059e0(found == 2));
		if (manifold->refresh)
		{
			((class_307530 *)this)->function_307530(manifold);
		}
	}
	if (found == 2)
	{
		((pair_305f60 *)this)->function_305f60((table_305f60 *)manifold, &feature, &key);
		((pair_305f60 *)this)->function_307840((table_305f60 *)manifold, key);
	}
}
