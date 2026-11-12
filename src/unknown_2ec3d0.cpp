// @flags /O2 /GL-
// Havok (third-party physics library): bringing a keyed list of child objects
// up to date with its shape: a new object for each newly accepted child, the
// kept object told about each child still accepted, and the object of each
// child no longer accepted let go and taken out.
#include "keyed_list_2ec680.h"

// a child object: let go through its sixth slot, brought up to date through
// its eighth
class kept_child_2ec3d0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void release() {}
	virtual void slot_6() {}
	virtual void refresh(child_description_2edd00 const *child, typed_2edd00 **other, context_2edd00 const *context) {}
};

// score 141 (was 141): Frame 0x134 agrees; the saved object and search
// result use different registers, with an extra array address kept across calls.
// Decreasing count 141; one search exit 152; added-entry reference 164;
// buffer declared first 141; stored key inputs 141; existing branch first 146;
// direct table read 147; member copies 166; loop-index search result 166.
// @retail 0x2ec3d0
void keyed_list_2ec680::function_2ec3d0(description_2edd00 const *description, typed_2edd00 **other, context_2edd00 const *context)
{
	child_description_2edd00 child;
	child_buffer_2ec680 buffer;
	child.field_8 = description->field_8;
	child.parent = description;
	collection_2ec680 const *shape = (collection_2ec680 const *)description->shape;
	long count = shape->child_count();
	long key = shape->first_key();
	for (long n = 0; n < count; n++)
	{
		child.key = (long)shape->child(key, &buffer);
		child.index = key;
		long found = children.index_of_key(key);
		if (context->filter->accept(context, other, description, (shape_2edd00 const *)shape, key).value)
		{
			if (found == -1)
			{
				children.added_slot().key = key;
				long arg_3 = field_8;
				dispatcher_2edd00 *table = context->dispatcher;
				long first = ((typed_2edd00 *)child.key)->type();
				long second = (*other)->type();
				handler_2edd00 handler = table->handlers[first][second];
				children.last_slot().child = handler(&child, other, context, arg_3);
			}
			else
			{
				((kept_child_2ec3d0 *)children.data[found].child)->refresh(&child, other, context);
			}
		}
		else if (found != -1)
		{
			((kept_child_2ec3d0 *)children.data[found].child)->release();
			children.size--;
			children.data[found] = children.data[children.size];
		}
		key = shape->next_key(key);
	}
}
