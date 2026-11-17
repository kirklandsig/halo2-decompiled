// @flags /O2 /GL-
// Havok (third-party physics library): a reference-counted object's
// constructor: it takes a reference to the object it is given, and points
// its list's owner at itself.
#include "world_object_30b340.h"

// score 12 (was 12): retail stores the vtable and reference count first and the
// fields out of offset order (+0x14 and +0x18 before +0x10, +0x24 before +0x1c),
// and the list's own stores last of all; the compiler emits the member stores
// in declaration order whatever order the initializer list is in, and a list
// with a constructor is built before the body, so retail's order needs the
// header's list to be a plain member, which is out of scope
// tried and no better: the initializer list in retail's store order (the same
// bytes), the stores in the body instead of the initializer list (16: the base
// class's stores then come first as in retail, but the list's own stores move
// ahead of the fields), the list's fields assigned in the body as well (44: the
// list's constructor's stores are not eliminated as dead, so they appear
// twice)
// public bases declared before the polymorphic base: four prefix-first forms 14,
// four forms with both preceding it 11; each used body or initializer stores.
// Further trials: staged public bases 5; a separate zero-word base 5;
// that reset after the type store 5; outer tail setup 5; a two-word reset 5;
// outer world store 5; base constructors with no body 5; chained clears 8. These
// setup helpers are rejected; the original constructor and layout remain.
// further trials: public input/handle bases 13; reference prefix 13;
// full payload base 18; handle tail initialized together 18; zero word
// assigned with the handle 18; prefix/input base 13, initializers 11;
// reference/input base 14. Public bases delay the final vtable store.
// member input record 4; member input and handle records 0, but existing
// descendants require direct object/placement/id fields and list.field_0;
// base input record with the original exposed fields 12
// aggregate tail and body stores 23; first two tail stores in constructor
// body 17, first only 17, owner only 16; two public tail bases with list/body
// constructors in all four combinations 12
// @retail 0x30b340
class_30b340::class_30b340(referenced_2d88a0 *object_, long value)
	: field_8(0), body(object_), handle(value)
{
	list.owner = this;
	if (object_)
	{
		object_->reference_count++;
	}
}
