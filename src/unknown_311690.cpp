// @flags /O2 /GL- /GX
// Havok (third-party physics library): an entity set to a kind keeps it when
// it already has that kind; kind 1 takes the shared object of that kind, and
// kind 2 a new object (0x314710) built for the entity.
#include "havok_reference.h"
#include "kind_object_3192b0.h"
#include "reference_counted_2d88a0.h"
#include "constraint_helpers_314090.h"

class hkEntity;

// the object an entity keeps for its kind, with the query of that kind
class kind_311690 : public c_havok_reference_counted
{
public:
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual long kind() { return 0; }
};

// the entity as the game's 0x1d8df0 sees it: it takes a reference to the new
// object and drops the old one
struct s_1d8df0
{
	byte field_0[0x54];
	c_havok_reference_counted *field_54;
	void function_1d8df0(c_havok_reference_counted *arg_0);
};

class c_library_311690 : public s_1d8df0
{
public:
	void update(long value);
};

// @retail 0x311690
void c_library_311690::update(long value)
{
	kind_311690 *kind = (kind_311690 *)field_54;
	if (kind && kind->kind() == value)
	{
		return;
	}
	switch (value)
	{
	case 1:
		g_480bb8.reference_count++;
		if (field_54)
		{
			havok_reference_remove(field_54);
		}
		field_54 = (c_havok_reference_counted *)&g_480bb8;
		break;
	case 2:
		{
			c_314710 *object = new c_314710((hkEntity *)this);
			function_1d8df0((c_havok_reference_counted *)object);
			havok_reference_remove((c_havok_reference_counted *)object);
		}
		break;
	}
}
