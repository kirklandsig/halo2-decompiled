// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object that listens to a world
// through two interfaces, and detaches both from the world when the world
// lets it go or when it is destroyed; through one of them it slows bodies
// down on contacts the game asks it to.
#include "reference_counted_2d88a0.h"
#include "unknown_1cec30.h"
#include <math.h>
#include <xmmintrin.h>

bool __cdecl function_279740(long value);

class listener_30c420;
class hkWorld;

class class_30caf0
{
public:
	void function_30caf0(void *pointer);
};

class c_2784c4
{
public:
	void function_30cb30(void *arg_0);
};

class c_library_30c470
{
public:
	void detach(void *callback);
};

class class_30c420
{
public:
	void function_30c420(listener_30c420 *listener);
};

class deletable_2da9f0
{
public:
	virtual void destroy(unsigned int flags) = 0;
};

class interface_b_2da9f0
{
public:
	// a contact between two bodies
	virtual void contact_2daae0(void *contact) {}
	virtual void slot_1() {}
	virtual void slot_2() {}
};

class interface_c_2da9f0
{
public:
	virtual ~interface_c_2da9f0() {}
	virtual void released(void *world) = 0;
};

class class_2da9f0 : public referenced_2d88a0, public interface_b_2da9f0, public interface_c_2da9f0
{
public:
	REFERENCED_NEW_2D88A0(0xb)

	void *world;

	class_2da9f0(void *world_);
	~class_2da9f0();
	void released(void *world_);
	void contact_2daae0(void *contact);
};

// @retail 0x2da9f0
void class_2da9f0::released(void *world_)
{
	((c_library_30c470 *)world)->detach(static_cast<interface_c_2da9f0 *>(this));
	((class_30c420 *)world)->function_30c420((listener_30c420 *)static_cast<interface_b_2da9f0 *>(this));
	world = 0;
	referenced_2d88a0 *object = this;
	object->reference_count--;
	if (object->reference_count == 0)
	{
		if (function_279740((long)object))
		{
			object->reference_count = 1;
		}
		else
		{
			((deletable_2da9f0 *)object)->destroy(1);
		}
	}
}

// attaches itself to the world through both interfaces
// @retail 0x2dacf0
class_2da9f0::class_2da9f0(void *world_)
{
	world = world_;
	((c_2784c4 *)world_)->function_30cb30(static_cast<interface_c_2da9f0 *>(this));
	((class_30caf0 *)world_)->function_30caf0(static_cast<interface_b_2da9f0 *>(this));
}

// @retail 0x2dad80 deleting class_2da9f0

// makes a new listener on the world
// @retail 0x2dadb0
void __cdecl function_2dadb0(hkWorld *arg_0)
{
	new class_2da9f0(arg_0);
}

// @retail 0x2dace0 adjustor class_2da9f0 0xc

// @retail 0x2daa60
class_2da9f0::~class_2da9f0()
{
	if (world)
	{
		((c_library_30c470 *)world)->detach(static_cast<interface_c_2da9f0 *>(this));
		((class_30c420 *)world)->function_30c420((listener_30c420 *)static_cast<interface_b_2da9f0 *>(this));
	}
}

// the world's collision filter (game code): slot 1 says whether two
// collidables may collide
class filter_2daae0
{
public:
	virtual void slot_0() {}
	virtual hkBool slot_1(void const *first, void const *second) { return hkBool(); }
};

// the world as 0x30c190 sees it
class c_278b60_filter;
class c_library_30c190
{
public:
	c_278b60_filter *filter();
};

// a collidable's shape: slot 5 says its kind
class shape_2daae0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual int slot_5() { return 0; }

	char unknown4[0x24 - 4];
	float field_24;
	float field_28;
};

// a collidable, held by its parent when it is part of another
struct collidable_2daae0
{
	shape_2daae0 *shape;
	char unknown4[0xc - 4];
	collidable_2daae0 *parent;
	char unknown10[0x20 - 0x10];
	hkEntity *entity;
};

struct contact_state_2daae0
{
	char unknown0[0xc];
	long field_c;
};

// a contact the world reports: its two collidables
struct contact_event_2daae0
{
	char unknown0[8];
	collidable_2daae0 *first;
	collidable_2daae0 *second;
	char unknown10[0x18 - 0x10];
	contact_state_2daae0 *state;
	float field_1c;
};

struct s_component_contact_pair;
// whether the game slows the moving entity on this contact (game code)
bool __cdecl function_279410(s_component_contact_pair const *pair, hkEntity *moving,
	hkEntity *opposing, real limit, real reduction, real approach);

// sets a rigid body's linear velocity (game code)
struct s_1d8e40 : hkRigidBody
{
	void function_1d8e40(hkVector4 const &arg_0);
};

// on a contact the filter lets through, a body whose shape is of kind 0x17
// gets its speed set to its shape's two values' difference, when the game
// asks for it
// score 112 (was 112): retail keeps the first body in edi and the second in
// ebx and takes 1.0f over the distance before the shape's difference; ours
// swaps the two bodies and divides the difference by the distance.
// Root names 112; earlier second declaration 112; double reciprocal 112;
// named inverse 125; scalar extraction unavailable; second shape first 112;
// for parent walks 112; one positive filter guard 112; sum lane extraction 143.
// Direct rigid-body view 112; velocity reference 106; float sqrt 112;
// scale updated in two steps 125; four-lane broadcast 135; output vector
// initialization 112; direct contact observations 112; named speed 112;
// velocity reference with staged reciprocal 31; motion pointer 120;
// velocity value 112; threshold captured 177; subtraction captured 125;
// both callback scalars captured 152; sum vector extraction 117.
// NOT kept: the velocity reference gains six, below threshold; the 31-point
// combination requires an exact match. Its input loads and scalar scratch
// still differ. All executable text restored; serial search found no match.
// @retail 0x2daae0
void class_2da9f0::contact_2daae0(void *contact_)
{
	contact_event_2daae0 *contact = (contact_event_2daae0 *)contact_;
	collidable_2daae0 *first = contact->first;
	while (first->parent)
	{
		first = first->parent;
	}
	collidable_2daae0 *second = contact->second;
	while (second->parent)
	{
		second = second->parent;
	}
	if (((c_library_30c190 *)world)->filter() == 0)
	{
		return;
	}
	if (!((filter_2daae0 *)((c_library_30c190 *)world)->filter())->slot_1(first, second).m_bool)
	{
		return;
	}
	if (contact->first->shape->slot_5() == 0x17)
	{
		hkEntity *entity = first->entity;
		shape_2daae0 *shape = contact->first->shape;
		__m128 const squares = _mm_mul_ps(((hkRigidBody *)entity)->m_motion->m_linear_velocity.m_quad, ((hkRigidBody *)entity)->m_motion->m_linear_velocity.m_quad);
		float speed_squared;
		_mm_store_ss(&speed_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
		if (function_279410((s_component_contact_pair const *)contact, entity, second->entity,
			shape->field_24, shape->field_24 - shape->field_28, contact->field_1c))
		{
			float const scale = 1.0f / (float)sqrt(speed_squared) * (shape->field_24 - shape->field_28);
			__m128 const single = _mm_set_ss(scale);
			hkVector4 velocity;
			velocity.m_quad = _mm_mul_ps(_mm_shuffle_ps(single, single, 0), ((hkRigidBody *)entity)->m_motion->m_linear_velocity.m_quad);
			((s_1d8e40 *)entity)->function_1d8e40(velocity);
			contact->state->field_c = 0;
		}
	}
	if (contact->second->shape->slot_5() == 0x17)
	{
		hkEntity *entity = second->entity;
		shape_2daae0 *shape = contact->second->shape;
		__m128 const squares = _mm_mul_ps(((hkRigidBody *)entity)->m_motion->m_linear_velocity.m_quad, ((hkRigidBody *)entity)->m_motion->m_linear_velocity.m_quad);
		float speed_squared;
		_mm_store_ss(&speed_squared, _mm_add_ss(_mm_shuffle_ps(squares, squares, 0xaa), _mm_add_ss(_mm_shuffle_ps(squares, squares, 0x55), squares)));
		if (function_279410((s_component_contact_pair const *)contact, entity, first->entity,
			shape->field_24, shape->field_24 - shape->field_28, contact->field_1c))
		{
			float const scale = 1.0f / (float)sqrt(speed_squared) * (shape->field_24 - shape->field_28);
			__m128 const single = _mm_set_ss(scale);
			hkVector4 velocity;
			velocity.m_quad = _mm_mul_ps(_mm_shuffle_ps(single, single, 0), ((hkRigidBody *)entity)->m_motion->m_linear_velocity.m_quad);
			((s_1d8e40 *)entity)->function_1d8e40(velocity);
			contact->state->field_c = 0;
		}
	}
}
