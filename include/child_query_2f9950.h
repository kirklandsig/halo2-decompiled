#ifndef CHILD_QUERY_2F9950_H
#define CHILD_QUERY_2F9950_H
// Havok (third-party physics library): one side of a query passed on as the
// child shape of a wrapping shape, at the same key and placement, with the
// wrapping side as its parent; and a collector of the nearest result.

struct input_2ea940;

// a shape that wraps a child shape
struct wrapper_2f9950
{
	char field_0[0xc];
	void const *child;
};

// The shape and key are copied together while a child keeps its placement
// and parent unchanged.
struct geometry_2ea640
{
	void const *shape;
	long key;
};

// one side of a query: a shape, the key of the part concerned, where it sits,
// and the side it is a part of
struct body_2ea940 : geometry_2ea640
{
	void const *placement;
	body_2ea940 const *parent;

	body_2ea940() {}
	// the wrapped child of a side, in the same place
	body_2ea940(body_2ea940 const *parent_, void const *placement_, wrapper_2f9950 const *wrapper)
	{
		placement = placement_;
		parent = parent_;
		void const *child = wrapper->child;
		key = parent->key;
		shape = child;
	}
};

void __cdecl function_2ea940(body_2ea940 const *a, body_2ea940 const *b, input_2ea940 const *input, void *collector);

// a collector that starts with no result: the largest float as its distance
class nearest_2f9950
{
public:
	nearest_2f9950() : distance(3.40282347e+38f) {}
	virtual ~nearest_2f9950() {}

	float distance;
};

class nearest_value_2f9950 : public nearest_2f9950
{
public:
	nearest_value_2f9950(long value_) : value(value_) {}

	long value;
};

#endif
