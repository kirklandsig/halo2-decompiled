// @flags /O2 /GL- /GX
// Havok (third-party physics library): the object kept for a pair of small
// shapes, holding a manifold of up to eight contacts, and its query that
// grows both shapes by their radii.
#include "pair_agent_2ed170.h"
#include "timer_mark_2ebc80.h"

struct s_308180_entry
{
	unsigned short first;
	unsigned short second;
	unsigned short field_4;
};

// a manifold of up to eight contacts
class class_308180
{
public:
	char unknown0[0x20];
	s_308180_entry entries[8];
	long field_50;
	int count;
	bool changed;

	class_308180();
};

// a contact found between two sides, and the sides
struct contact_307f90
{
	__m128 point;
	__m128 normal;
	description_2ed170 const *a;
	description_2ed170 const *b;
	char unknown28[0x30 - 0x28];
};

// whether a contact was found
class answer_307f90
{
public:
	answer_307f90() {}

	char value;
};

// told of each contact found
class collector_2ef4b0
{
public:
	virtual void slot_0() {}
	virtual void slot_1(contact_307f90 const *contact) {}
};

// a query's input (constructor 0x2ef260)
class query_2ef260
{
public:
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	__m128 from;
	__m128 to;
	__m128 grown_from;
	__m128 grown_to;
	float radius;
	float tolerance;
	char unknown68[0x110 - 0x68];

	query_2ef260(long a, long b, long c, long d, long e, long f, __m128 const &from_, long g, __m128 const &to_, float radius_);
	void function_3080a0(class_308180 *manifold);
	void function_307f90(answer_307f90 *found, contact_307f90 *contact);
};

// a box: a radius it is grown by, and its half extents
struct box_2ef3b0
{
	char unknown0[0xc];
	float radius;
	__m128 half_extents;
};

struct input_2ef2f0
{
	long field_0;
	long field_4;
	float tolerance;
};

class manifold_pair_2ef2f0 : public sized_2ed170
{
public:
	long field_c;
	class_308180 manifold;
	long field_6c;

	__forceinline manifold_pair_2ef2f0(long value) : sized_2ed170(value) {}

	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void function_2ef3b0(description_2ed170 const *a, description_2ed170 const *b, input_2ef2f0 const *input, long arg_3);
};

void *__cdecl function_2ee390(long arg_0, long arg_1, long arg_2, long value);

// the manifold object when both boxes are large against the tolerance, the
// general object otherwise
// @retail 0x2ef2f0
void *__cdecl function_2ef2f0(long arg_0, long arg_1, long arg_2, long value)
{
	description_2ed170 const *a = (description_2ed170 const *)arg_0;
	description_2ed170 const *b = (description_2ed170 const *)arg_1;
	input_2ef2f0 const *input = (input_2ef2f0 const *)arg_2;
	box_2ef3b0 const *first_box = (box_2ef3b0 const *)a->shape;
	box_2ef3b0 const *second_box = (box_2ef3b0 const *)b->shape;
	if (!(first_box->half_extents.m128_f32[3] * 1.999f <= input->tolerance) &&
		!(second_box->half_extents.m128_f32[3] * 1.999f <= input->tolerance))
	{
		return new manifold_pair_2ef2f0(value);
	}
	return function_2ee390(arg_0, arg_1, arg_2, value);
}

// score 11 (was 11): frame 0x134 and extent 244 agree; second box acquisition and radius
// carriers differ before growth. First extent at use 11; direct radii/all fields 13/13;
// radius/extent references 13/11; scalar broadcast loads 89; late second radius 13;
// staged vector broadcasts 64. NOT kept: tied or larger differences; original observations restored.
// First radius before second shape 11; both radii before extent 11; both
// extent snapshots 71; scalar-set/shuffle broadcasts 72; four-lane broadcasts
// 157; immutable output vectors 11; aligned extent loads 11; copy then add 72.
// NOT kept: no gain.
// @retail 0x2ef3b0
void manifold_pair_2ef2f0::function_2ef3b0(description_2ed170 const *a, description_2ed170 const *b, input_2ef2f0 const *input, long arg_3)
{
	timer_add_2ebc80("TtBoxBox");
	box_2ef3b0 const *first_box = (box_2ef3b0 const *)a->shape;
	box_2ef3b0 const *second_box = (box_2ef3b0 const *)b->shape;
	__m128 const first_half_extents = first_box->half_extents;
	float const first_radius = first_box->radius;
	float const second_radius = second_box->radius;
	__m128 grown_first = _mm_add_ps(first_half_extents, _mm_set_ps1(first_radius));
	__m128 grown_second = _mm_add_ps(second_box->half_extents, _mm_set_ps1(second_radius));
	query_2ef260 query((long)a, (long)b, (long)input, field_8, arg_3, (long)a->placement + 0x20, grown_first,
		(long)b->placement + 0x20, grown_second, input->tolerance);
	query.function_3080a0(&manifold);
	timer_add_2ebc80("Et");
}

class box_pair_2ef4b0
{
public:
	void function_2ef4b0(description_2ed170 const *a, description_2ed170 const *b, input_2ef2f0 const *input, collector_2ef4b0 *collector);
};

// the two boxes grown by their radii, and the contact found between them
// passed on
// @retail 0x2ef4b0
void box_pair_2ef4b0::function_2ef4b0(description_2ed170 const *a, description_2ed170 const *b, input_2ef2f0 const *input, collector_2ef4b0 *collector)
{
	timer_add_2ebc80("TtBoxBox");
	box_2ef3b0 const *first_box = (box_2ef3b0 const *)a->shape;
	box_2ef3b0 const *second_box = (box_2ef3b0 const *)b->shape;
	contact_307f90 contact;
	answer_307f90 found;
	{
		__m128 grown_first = _mm_add_ps(first_box->half_extents, _mm_set_ps1(first_box->radius));
		__m128 grown_second = _mm_add_ps(second_box->half_extents, _mm_set_ps1(second_box->radius));
		query_2ef260 query((long)a, (long)b, 0, 0, 0, (long)a->placement + 0x20, grown_first,
			(long)b->placement + 0x20, grown_second, input->tolerance);
		contact.a = a;
		contact.b = b;
		query.function_307f90(&found, &contact);
		if (found.value)
		{
			collector->slot_1(&contact);
		}
	}
	timer_add_2ebc80("Et");
}

// the same as a function
// @retail 0x2ef5d0
void __cdecl function_2ef5d0(void const *a_, void const *b_, void const *input_, void *collector_)
{
	description_2ed170 const *a = (description_2ed170 const *)a_;
	description_2ed170 const *b = (description_2ed170 const *)b_;
	input_2ef2f0 const *input = (input_2ef2f0 const *)input_;
	timer_add_2ebc80("TtBoxBox");
	box_2ef3b0 const *first_box = (box_2ef3b0 const *)a->shape;
	box_2ef3b0 const *second_box = (box_2ef3b0 const *)b->shape;
	contact_307f90 contact;
	answer_307f90 found;
	{
		__m128 grown_first = _mm_add_ps(first_box->half_extents, _mm_set_ps1(first_box->radius));
		__m128 grown_second = _mm_add_ps(second_box->half_extents, _mm_set_ps1(second_box->radius));
		query_2ef260 query((long)a, (long)b, 0, 0, 0, (long)a->placement + 0x20, grown_first,
			(long)b->placement + 0x20, grown_second, input->tolerance);
		contact.a = a;
		contact.b = b;
		query.function_307f90(&found, &contact);
		if (found.value)
		{
			((collector_2ef4b0 *)collector_)->slot_1(&contact);
		}
	}
	timer_add_2ebc80("Et");
}
// whether two boxes overlap, returned through a hidden pointer
class answer_306580
{
public:
	answer_306580() {}

	char value;
};

class box_pair_306400
{
public:
	void function_306400();
};

class class_306580
{
public:
	answer_306580 function_306580();
};

// told of each pair found overlapping
class pair_collector_2ef6e0
{
public:
	virtual void slot_0() {}
	virtual void slot_1(description_2ed170 const *a, description_2ed170 const *b) {}
};


class box_pair_2ef6e0
{
public:
	void function_2ef6e0(description_2ed170 const *a, description_2ed170 const *b, input_2ef2f0 const *input, pair_collector_2ef6e0 *collector);
};

// the two boxes grown by their radii, and the pair passed on when they
// overlap
// score 6 (was 6): the second radius load still follows the first vector store.
// Second vector first 32; declarations before assignments 6; memory broadcasts 78;
// shared broadcaster 72; first/second/both shape values 99/103/127;
// four-lane value broadcasts 137.
// Second-radius load remains late; second radius first 24, both radii first 24, first
// extent with radii 24, separate splats 29, mutable extents 76, const extents 6, native scalar vector
// load 62, extent role names 9.
// Delayed second shape 25; direct shape reads 25; box references 6;
// radius references 6; extent references 6; vector-return growth 6;
// output-reference growth 6; aligned extent loads 6. NOT kept: no gain.
// @retail 0x2ef6e0
void box_pair_2ef6e0::function_2ef6e0(description_2ed170 const *a, description_2ed170 const *b, input_2ef2f0 const *input, pair_collector_2ef6e0 *collector)
{
	timer_add_2ebc80("TtBoxBox");
	box_2ef3b0 const *first_box = (box_2ef3b0 const *)a->shape;
	box_2ef3b0 const *second_box = (box_2ef3b0 const *)b->shape;
	__m128 grown_first = _mm_add_ps(first_box->half_extents, _mm_set_ps1(first_box->radius));
	__m128 grown_second = _mm_add_ps(second_box->half_extents, _mm_set_ps1(second_box->radius));
	query_2ef260 query((long)a, (long)b, 0, 0, 0, (long)a->placement + 0x20, grown_first,
		(long)b->placement + 0x20, grown_second, input->tolerance);
	((box_pair_306400 *)&query)->function_306400();
	answer_306580 overlap = ((class_306580 *)&query)->function_306580();
	if (overlap.value)
	{
		collector->slot_1(a, b);
	}
	timer_add_2ebc80("Et");
}

// the same as a function
// score 6 (was 6): the second radius load still follows the first vector store.
// Second vector first 32; declarations before assignments 6; memory broadcasts 78;
// shared broadcaster 72; first/second/both shape values 99/103/127;
// four-lane value broadcasts 137.
// Second-radius load remains late; second radius first 24, both radii first 24, first
// extent with radii 24, separate splats 29, mutable extents 76, const extents 6, native scalar vector
// load 63, extent role names 9.
// Delayed second shape 25; direct shape reads 25; box references 6;
// radius references 6; extent references 6; vector-return growth 6;
// output-reference growth 6; aligned extent loads 6. NOT kept: no gain.
// @retail 0x2ef7f0
void __cdecl function_2ef7f0(void const *a_, void const *b_, void const *input_, void *collector_)
{
	description_2ed170 const *a = (description_2ed170 const *)a_;
	description_2ed170 const *b = (description_2ed170 const *)b_;
	input_2ef2f0 const *input = (input_2ef2f0 const *)input_;
	timer_add_2ebc80("TtBoxBox");
	box_2ef3b0 const *first_box = (box_2ef3b0 const *)a->shape;
	box_2ef3b0 const *second_box = (box_2ef3b0 const *)b->shape;
	__m128 grown_first = _mm_add_ps(first_box->half_extents, _mm_set_ps1(first_box->radius));
	__m128 grown_second = _mm_add_ps(second_box->half_extents, _mm_set_ps1(second_box->radius));
	query_2ef260 query((long)a, (long)b, 0, 0, 0, (long)a->placement + 0x20, grown_first,
		(long)b->placement + 0x20, grown_second, input->tolerance);
	((box_pair_306400 *)&query)->function_306400();
	answer_306580 overlap = ((class_306580 *)&query)->function_306580();
	if (overlap.value)
	{
		((pair_collector_2ef6e0 *)collector_)->slot_1(a, b);
	}
	timer_add_2ebc80("Et");
}
