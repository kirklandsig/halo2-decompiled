// @flags /O2 /GL-
// Havok (third-party physics library): the contact between a sphere and a
// box, bracketed by two marks in the timing stream: the sphere's centre is
// carried into the box's frame; outside the box, the contact is along the
// line from the nearest point of the box, when it is within the tolerance;
// inside, it is along the axis whose face is nearest. The contact keeps an
// id the owner hands out once; a miss hands the id back.
#include "timer_mark_2ebc80.h"
#include "growable_array_2d91d0.h"
#include "pair_agents_2eab70.h"
#include "sphere_box_2f1ea0.h"

// a contact: where the two sides touch, and its id
struct contact_2f1b80
{
	contact_point_2f1ea0 where;
	unsigned short id;
};

ELEMENTS_2D91D0_ALIGNED(contacts_2f1b80, contact_2f1b80);

struct results_2f1b80
{
	contacts_2f1b80 *contacts;
};

class contact_owner_2f1b80
{
public:
	virtual void slot_0() {}
	virtual unsigned short add_contact(void const *a, void const *b, void const *input, contact_2f1b80 *contact) { return 0; }
	virtual void release_id(unsigned short id) {}
};

// the contact of a sphere with a box, added to the output with the id the
// owner hands out; a miss drops it and hands the id back
// Matched: see 0x2f1ea0 in unknown_2f1ea0.cpp (the shared contact helper's face arms).
// Before, 37 (was 37): the centre and body B take edi and ebx where retail has
// ebx and edi, and three 4-byte stack slots are packed differently.
// Further trials: output element reference 37; native signed or unsigned
// held id 64; direct id consumers 37; common held-id lifetime 37; common
// native id 64; element reference with common or direct id 37/37.
// This/scratch homes and centre/body-B register ownership still differ.
// Further trials: typed output/body pair/query/all inputs 37/37/37/37;
// miss first 59; miss to one exit 59; id rejection first 42;
// output-array member reference 37.
// Further trials: output element/record/both role names 37 each; output work
// scope alone/element name 37/37; id role name 37; output receiver before
// the mark alone/element name 37/37. Shared geometry retained unchanged.
// @retail 0x2f1b80
void class_2f1b80::function_2f1b80(void const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TtSphereBox");
	results_2f1b80 *results = (results_2f1b80 *)collector;
	contact_2f1b80 *contact = &results->contacts->next_slot();
	if (sphere_box_contact_2f1ea0(a, b, input, contact->where))
	{
		if (id == 0xffff)
		{
			id = owner->add_contact(a, b, input, contact);
		}
		unsigned short held = id;
		if (held != 0xffff)
		{
			contact->id = held;
		}
		else
		{
			results->contacts->size--;
		}
	}
	else
	{
		results->contacts->size--;
		unsigned short held = id;
		if (held != 0xffff)
		{
			owner->release_id(held);
			id = 0xffff;
		}
	}
	timer_add_2ebc80("Et");
}
