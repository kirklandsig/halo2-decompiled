#ifndef __UNKNOWN_0D0690_H__
#define __UNKNOWN_0D0690_H__

/* iterating the units riding an object and its riders (unknown_0d0690.cpp) */
struct s_object_child_iterator
{
	long root;
	long current;
	long next;
	long child_value;
	long child_index;
	short child_short;
};

void function_d0620(s_object_child_iterator *iterator, long object_index);
bool function_d0690(s_object_child_iterator *iterator);

#endif
