#ifndef OWNED_REFERENCE_318B90_H
#define OWNED_REFERENCE_318B90_H
// Havok (third-party physics library): two reference-counted objects (vtables
// 0x415060 and 0x41506c) that keep the object they were made for; their
// blocks come from the allocator with tag 0x1d, and their deleting destructor
// is the one 0x30c3b0 shares.
#include "reference_base_318970.h"

class lane_a_simple_contact_manager;
class class_318e60;
struct lane_a_contact_body;

class class_318b90 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x1d)

	class_318b90(void *owner);
	virtual lane_a_simple_contact_manager *function_318bb0(
		lane_a_contact_body const *first, lane_a_contact_body const *second, void const *input);

	void *owner;
};

class class_319080 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x1d)

	class_319080(void *owner);
	virtual class_318e60 *create(lane_a_contact_body const *, lane_a_contact_body const *, void const *);
	virtual void slot_2() {}

	void *owner;
};

#endif
