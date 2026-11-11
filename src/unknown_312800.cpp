// @flags /O2 /GL- /GX
// Havok (third-party physics library): an object placed by a transform, built
// from a description: its shape, its bounds not yet set, and the transform.
#include "placed_shape_312800.h"
#include "unknown_1cec30.h"

// score 2 (was 2): the bounds pointer store and receiver load are reversed.
// Native persistent pointer field as void, const void, bounds and const bounds 2 each;
// each with the existing clear forced inline 2. Original field type and helper restored.
// the receiver load precedes the bounds-field store;
// field store first 4, before clear 10; bounds pointer 2, store first 4;
// shape local late 2, early 6; clear returns pointer immediate 4, later 2.
// @retail 0x312800
placed_shape_312800::placed_shape_312800(description_315ba0 const *description)
	: overlapped_312800((referenced_2d88a0 *)description->field_4, 2)
{
	bounds.clear_312800();
	list.field_0 = description->field_0;
	body.field_8 = (long)&bounds;
	field_54 = ((shape_312800 *)body.shape)->slot_5();
	((hkTransform &)transform).set((hkTransform const &)description->transform);
}
