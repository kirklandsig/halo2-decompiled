// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object built from its description:
// the part object its kind asks for (kind 7 always gets 0x31a6b0's), given the
// description's matrix, vector and value, and the object activated through
// 0x30bc60 while that part's state is set.
#include <math.h>
#include <xmmintrin.h>
#include "derived_311920.h"
#include "tagged_objects_2e4ff0.h"
#include "motion_description_3115a0.h"

#include "motion_31ad80.h"

#include "lane_a_motion_leaves.h"

// an object's part, as the object reaches it
class part_311940
{
public:
	virtual ~part_311940() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	// the part's kind
	virtual int slot_6() { return 0; }
	virtual void slot_7(float value) {}
	virtual void slot_8() {}
	// its inertia matrix
	virtual void slot_9(void *matrix) {}
	virtual void slot_10() {}
	virtual void slot_11(void const *matrix) {}
	virtual void slot_12() {}
	virtual void slot_13() {}
	virtual void slot_14() {}
	virtual void slot_15(void const *vector) {}
	virtual void slot_16() {}
	virtual void slot_17() {}
	virtual void slot_18() {}
	virtual void slot_19() {}
	virtual void slot_20(void const *value) {}
	virtual void slot_21(void const *value) {}
	virtual void slot_22() {}
	virtual void slot_23() {}
	virtual void slot_24() {}
	virtual void slot_25() {}
	virtual void slot_26() {}
	virtual void slot_27() {}
	virtual void slot_28() {}
	virtual void slot_29() {}
	virtual void slot_30(part_311940 *other) {}
	virtual void slot_31(part_311940 *other) {}

	// destroyed through its table, known not to be null
	void delete_311e40() { delete this; }

	char unknown4[0x14 - 4];
	float field_14;
	float field_18;
	char unknown1c[0x20 - 0x1c];
	float field_20[4];
	float field_30[4];
	char unknown40[0xb0 - 0x40];
	float field_b0[4];
};

// a 3x3 matrix, one row to a vector
struct matrix_311940
{
	float rows[3][4];
};

// an object's description
struct s_311340
{
	char unknown0[0x20];
	__m128 field_20;
	__m128 field_30;
	__m128 field_40;
	__m128 field_50;
	matrix_311940 field_60;
	__m128 field_90;
	float field_a0;
	float field_a4;
	float field_a8;
	long field_ac;
	long field_b0;
	char kind;
	char field_b5;
};

// a part object for the given kind; for kind 1, 0x311890's when the matrix's
// diagonal terms are within a tenth of their sum of each other, else 0x31ad80's
// @retail 0x311940
static part_311940 *__cdecl function_311940(int kind, void const *first, void const *second,
	float value, matrix_311940 const *matrix, void const *vector)
{
	// retail keeps the kind on the stack, where the caller pushes it
	int const *kind_reference = &kind;
	part_311940 *part;
	switch (kind)
	{
	case 2:
		part = (part_311940 *)new class_311890(first, second);
		break;
	case 4:
		part = (part_311940 *)new class_31ad80(first, second);
		break;
	case 3:
		part = (part_311940 *)new class_311920(first, second);
		break;
	case 5:
		part = (part_311940 *)new class_31a870(first, second);
		break;
	case 1:
	{
		float const x = matrix->rows[0][0];
		float const y = matrix->rows[1][1];
		float const z = matrix->rows[2][2];
		float const sum = x + y + z;
		float const spread = (float)fabs(x - y) + (float)fabs(y - z) + (float)fabs(x - z);
		if (spread < sum * 0.1f)
		{
			part = (part_311940 *)new class_311890(first, second);
		}
		else
		{
			part = (part_311940 *)new class_31ad80(first, second);
		}
		break;
	}
	case 6:
		part = (part_311940 *)new class_3193c0(first, second);
		break;
	default:
		part = (part_311940 *)new class_31a6b0(first, second);
		break;
	}
	if (kind != 6)
	{
		part->slot_11(matrix);
		part->slot_15(vector);
		part->slot_7(value);
	}
	return part;
}

class hkBool
{
public:
	hkBool() {}

	char m_bool;
};

// the object as 0x30bc40 and 0x30bc60 see it
class hkRigidBody
{
public:
	hkBool body_is_simulating(void) const;
	void activate(void);
};

// the object as 0x311690 sees it
class c_library_311690
{
public:
	void update(long value);
};

// the object as 0x3117b0 sees it
class bounded_shape_3117b0
{
public:
	void function_3117b0();
};

// the object: built on the one 0x30beb0 builds
class c_311ba0 : public class_30bf50
{
public:
	c_311ba0(s_311340 const *description);

	char unknown4[8 - 4];
	void *world;
	void *shape;
	long unknown10;
	char *field_14;
	char unknown18[0x3c - 0x18];
	part_311940 *part;
	bool kind_7;
	char unknown41[3];
	void *group;
	char unknown48[4];
	long field_4c;
	long field_50;
	char unknown54[0xa0 - 0x54];

	// 0x30bc60 called when 0x30bc40 says false and field_44 is set
	void activate_if_inactive_311ba0()
	{
		if (!((hkRigidBody const *)this)->body_is_simulating().m_bool && group != 0)
		{
			((hkRigidBody *)this)->activate();
		}
	}
};

// @retail 0x311ba0
c_311ba0::c_311ba0(s_311340 const *description) : class_30bf50(description)
{
	bool const is_kind_7 = description->kind == 7;
	if (is_kind_7)
	{
		kind_7 = true;
		part = (part_311940 *)new class_31a6b0(&description->field_20, &description->field_30);
		field_14 = (char *)part + 0x60;
	}
	else
	{
		part = function_311940(description->kind, &description->field_20, &description->field_30,
			description->field_a0, &description->field_60, &description->field_90);
		activate_if_inactive_311ba0();
		part->slot_20(&description->field_40);
		activate_if_inactive_311ba0();
		part->slot_21(&description->field_50);
		part->field_14 = description->field_a4;
		part->field_18 = description->field_a8;
		field_14 = (char *)part + 0x60;
		((c_library_311690 *)this)->update(description->field_b5);
		if (shape != 0)
		{
			((bounded_shape_3117b0 *)this)->function_3117b0();
		}
	}
	field_4c = description->field_ac;
	field_50 = description->field_b0;
}

// the scale a part keeps (0x183df0, game code)
struct s_scale_holder
{
	float get_inverse_scale() const;
};

// the part a part was made from, if any (0x30c180)
struct class_30c180
{
	long function_30c180() const;
};

// the world's bookkeeping for a body that becomes or stops being kind 7
class entity_316000;
struct constraints_30fb40;
class class_30e140
{
public:
	void function_30f960(entity_316000 *entity);
	void function_30fb40(entity_316000 *entity, constraints_30fb40 *removed);
};

struct member_30c8e0;
struct constraints_30f380;
class owner_30c8e0
{
public:
	void function_30f340(member_30c8e0 *member);
	void function_30f380(member_30c8e0 *member, constraints_30f380 const *added);
};

// an array of four-byte values whose storage comes from the allocator's stack
// area (src/unknown_311d10.cpp)
struct array_311d10
{
	long *data;
	long size;
	unsigned long capacity_and_flags;

	array_311d10() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_311d10()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct stack_array_311d10 : array_311d10
{
	long *stack_block;
	long stack_capacity;

	stack_array_311d10(long capacity);
	~stack_array_311d10();
};

// an inertia matrix as a part gives it
struct inertia_311e40
{
	__m128 rows[3];
};

// whether a kind of part moves (kinds 6 and 7 do not)
static inline bool kind_moves_311e40(int kind)
{
	return !(kind == 7 || kind == 6);
}

// score 103 (was 103): ours merges the delete that ends the kind-change
// branch into the delete that ends the non-moving branch, where retail keeps
// both (one through edx, one through eax); retail also allocates the
// 0xd0-byte part before testing for kind 7 (a ternary for the two news scores
// the same).
// the body's part changed to one of another kind: a moving part is replaced
// by one of the new kind built from its state, a part that does not move
// gets a part of its own over the moving one, and leaving kind 6 or 7 goes
// back to the part underneath (when there is one); the world is told when
// the body stops or starts being of kind 7
// @retail 0x311e40
void class_3115a0::function_311e40(int kind)
{
	c_311ba0 *body = (c_311ba0 *)this;
	int const current = body->part->slot_6();
	if (current == kind)
	{
		return;
	}
	bool const moves = kind_moves_311e40(kind);
	bool const moved = kind_moves_311e40(current);
	if (moves && !moved && ((class_30c180 *)body->part)->function_30c180() == 0)
	{
		return;
	}
	void *const world = body->world;
	stack_array_311d10 constraints(0x10);
	if (world)
	{
		if (current == 7)
		{
			((class_30e140 *)world)->function_30fb40((entity_316000 *)this, (constraints_30fb40 *)&constraints);
		}
		if (kind == 7)
		{
			((class_30e140 *)world)->function_30f960((entity_316000 *)this);
		}
	}
	if (moves)
	{
		if (moved)
		{
			if (kind != 1)
			{
				part_311940 *old = body->part;
				inertia_311e40 inertia;
				old->slot_9(&inertia);
				part_311940 *created = function_311940(kind, old->field_b0, old->field_30,
					((s_scale_holder const *)old)->get_inverse_scale(), (matrix_311940 const *)&inertia, old->field_20);
				old->slot_30(created);
				created->field_14 = old->field_14;
				created->field_18 = old->field_18;
				body->part = created;
				old->delete_311e40();
			}
		}
		else
		{
			part_311940 *old = body->part;
			body->part = (part_311940 *)((class_30c180 *)old)->function_30c180();
			if (body->part->slot_6() != kind && kind != 1)
			{
				part_311940 *underneath = body->part;
				inertia_311e40 inertia;
				underneath->slot_9(&inertia);
				part_311940 *created = function_311940(kind, underneath->field_b0, underneath->field_30,
					((s_scale_holder const *)underneath)->get_inverse_scale(), (matrix_311940 const *)&inertia, underneath->field_20);
				created->field_14 = underneath->field_14;
				created->field_18 = underneath->field_18;
				body->part = created;
				underneath->delete_311e40();
			}
			old->slot_30(body->part);
			old->slot_31(0);
			old->delete_311e40();
		}
	}
	else
	{
		part_311940 *created;
		if (kind == 7)
		{
			created = (part_311940 *)new class_31a6b0(body->part->field_b0, body->part->field_30);
		}
		else
		{
			created = (part_311940 *)new class_3193c0(body->part->field_b0, body->part->field_30);
		}
		if (moved)
		{
			created->slot_31(body->part);
			body->part = created;
		}
		else
		{
			part_311940 *old = body->part;
			if (((class_30c180 *)old)->function_30c180())
			{
				old->slot_30((part_311940 *)((class_30c180 *)old)->function_30c180());
				created->slot_31((part_311940 *)((class_30c180 *)old)->function_30c180());
			}
			body->part = created;
			delete old;
		}
	}
	body->field_14 = (char *)body->part + 0x60;
	body->kind_7 = kind == 7;
	if (world)
	{
		if (current == 7)
		{
			((owner_30c8e0 *)world)->function_30f380((member_30c8e0 *)this, (constraints_30f380 const *)&constraints);
		}
		if (kind == 7)
		{
			((owner_30c8e0 *)world)->function_30f340((member_30c8e0 *)this);
		}
	}
}
