// @flags /O2 /GL-
// Havok (third-party physics library): a sphere against a box: the contact
// between them, from the box's nearest point to the sphere's centre, or from
// the face nearest the centre when the centre is inside; and the pair
// reported when the centre is closer to the box than the sum of their radii.
// Each is bracketed by two marks in the timing stream.
#include "timer_mark_2ebc80.h"
#include "sphere_box_2f1ea0.h"

// a point placed by a transform
struct placed_point_2f23c0
{
	__m128 value;

	void set_transformed(c_query_transform const *transform, __m128 const *point)
	{
		__m128 p = *point;
		__m128 z = _mm_add_ps(_mm_mul_ps(transform->axes[2], _mm_shuffle_ps(p, p, 0xaa)), transform->position);
		__m128 y = _mm_mul_ps(transform->axes[1], _mm_shuffle_ps(p, p, 0x55));
		__m128 x = _mm_mul_ps(transform->axes[0], _mm_shuffle_ps(p, p, 0x00));
		value = _mm_add_ps(_mm_add_ps(x, y), z);
	}
};

class collector_2f23c0
{
public:
	virtual void slot_0() {}
	virtual void add_pair(body_2f23c0 const *a, body_2f23c0 const *b) {}
};

// a contact: where the two sides touch, and the two sides
struct contact_2f2130
{
	contact_point_2f1ea0 where;
	body_2f23c0 const *a;
	body_2f23c0 const *b;
};

class contact_collector_2f2130
{
public:
	virtual void slot_0() {}
	virtual void add_contact(contact_2f2130 const *contact) {}
};

// the contact of a sphere with a box, passed on when they are within the
// tolerance or the centre is inside the box
__forceinline void pass_sphere_box_contact_2f1ea0(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtSphereBox");
	contact_2f2130 contact;
	contact.a = (body_2f23c0 const *)a;
	contact.b = (body_2f23c0 const *)b;
	if (sphere_box_contact_2f1ea0(a, b, input, contact.where))
	{
		((contact_collector_2f2130 *)collector)->add_contact(&contact);
	}
	timer_add_2ebc80("Et");
}

class class_2f1ea0
{
public:
	void function_2f1ea0(void const *a, void const *b, void const *input, void *collector);
};

// Matched: in the shared contact helper's inside branch each face arm sets its
// normal last and reads it through `b->motion` (no motion local). The four arms then
// begin with the same load in the same register, which the compiler merges into the
// one load retail has above the branch, and `b` stays live through the arms.
// Before, 20 (was 20): body A and the sphere take edi here and ebx in retail
// (the centre the other way round), body B's motion in the inside branch
// takes esi where retail has edx, and the collector's table and the record's
// address (and the end mark's time) have eax and edx swapped.
// Body record values A/B/both 178/193/204; contact-pointer/native status 20/20;
// short/unsigned axis 62/20; rotation transform reference 20.
// Record B/A input alone 25/25; abstract collector 20; typed outer A/B/all
// inputs 20/20/20; ordinary/private outer inline 812/20.
// score 20 (was 20): body/centre and inside-motion registers still differ.
// Box at uses 20; sphere at uses 146; box and sphere at uses 146; centre at uses 173;
// box and centre at uses 173; box-radius value 38; both radius values 174;
// box radius at uses with extent receiver 20. Restored.
// Complete box frame 207; box motion 214; sphere motion 206; both motions 239;
// late box frame 221; both frames 240; box value 217; sphere value 202.
// @retail 0x2f1ea0
void class_2f1ea0::function_2f1ea0(void const *a, void const *b, void const *input, void *collector)
{
	pass_sphere_box_contact_2f1ea0(a, b, input, collector);
}

// the same as a function
// Matched: see 0x2f1ea0 (the shared contact helper's face arms).
// Before, 20 (was 20): as 0x2f1ea0. Inside-first dispatch 124; centre value
// 188; default face first 93; byte result 20; first/half product parts 20/20;
// point displacement/distance parts 20/20; default normal value 54.
// as 0x2f1ea0.
// Signed/unsigned byte axis 69/69; long axis 20; native broadcast 45;
// uniform refinement constants 155; scalar selection first 20; refined-lane
// broadcast 167; reversed outer input order 20.
// as 0x2f1ea0.
// score 20 (was 20): same body/centre, motion and tail register differences.
// Sphere before transform 24; box before transform 175; aligned axis loads 20;
// aligned normal stores 133; aligned point store 22; aligned centre load 20;
// opaque record sides 20; shared completion exit 20. Restored.
// as 0x2f1ea0.
// Vector face depths 179; selected face pointer 68; sign operand order 24;
// rotation accumulation/product order 20/20; vector root/sum 156/33; direction steps 20.
// @retail 0x2f2130
void __cdecl function_2f2130(void const *a, void const *b, void const *input, void *collector)
{
	pass_sphere_box_contact_2f1ea0(a, b, input, collector);
}

// @retail 0x2f23c0
void __cdecl function_2f23c0(body_2f23c0 const *a, body_2f23c0 const *b, void const *input, collector_2f23c0 *collector)
{
	timer_add_2ebc80("TtSphereBox");
	sphere_2f23c0 const *sphere = (sphere_2f23c0 const *)a->shape;
	box_2f23c0 const *box = (box_2f23c0 const *)b->shape;
	__m128 const *centre = &a->motion->transform.position;
	c_query_point local;
	local.inverse_transform(&b->motion->transform, centre);
	__m128 const sign = _mm_and_ps(local.value, *(__m128 const *)g_sign_mask_4124a0);
	__m128 const magnitude = _mm_and_ps(local.value, *(__m128 const *)g_magnitude_mask_455940);
	placed_point_2f23c0 nearest;
	nearest.value = _mm_xor_ps(_mm_min_ps(magnitude, box->half_extents), sign);
	placed_point_2f23c0 world;
	world.set_transformed(&b->motion->transform, &nearest.value);
	__m128 const offset = _mm_sub_ps(world.value, *centre);
	__m128 const squares = _mm_mul_ps(offset, offset);
	float distance_squared;
	_mm_store_ss(&distance_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
	float const radius = box->radius + sphere->radius;
	if (distance_squared < radius * radius)
	{
		collector->add_pair(a, b);
	}
	timer_add_2ebc80("Et");
}
