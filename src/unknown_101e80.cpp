#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_object_link
{
	signed char timer;
	byte state;
	byte unknown02[2];
	word flags;
	word time;
	byte unknown08[8];
	real rate;
	byte unknown14[0x10];
	real accumulator;
	byte unknown28[0xc];
};

struct s_object_slot
{
	signed char state;
	byte unknown01;
	short value;
	byte unknown04[8];
};

struct s_object_attachments
{
	byte unknown00[6];
	signed char first;
	signed char second;
	byte unknown08[4];
	long objects[4];
};

struct s_object
{
	dword tag_index;
	byte flags04;
	byte unknown05[3];
	byte unknown08[0xc];
	long parent_index;
	byte unknown18[0x114];
	union
	{
		byte flags12c;
		dword flag12c_0 : 1;
	};
	byte unknown130[0x24];
	long child_index;
	byte unknown158[0x20];
	long field_178;
	byte unknown17c[8];
	real field_184;
	byte unknown188[0x1c];
	s_object_link links[2];
	union
	{
		s_object_slot slots[2];
		s_object_attachments attachments;
	};
	byte unknown228[0x24];
	long field_24c;
};

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

struct s_tag_slot
{
	byte unknown00[0x1c];
	real duration;
	byte unknown20[0x20];
};

struct s_tag_link
{
	dword : 9;
	dword flag9 : 1;
	dword flag10 : 1;
	dword : 21;
	real start;
	real end;
	byte unknown0c[0x10];
	short minimum;
	short maximum;
	byte unknown20[0xcc];
};

struct s_object_tag
{
	byte unknown00[0x38];
	long field_38;
	byte unknown3c[0x80];
	dword : 26;
	dword flagbc_26 : 1;
	dword : 5;
	byte unknownc0[0x198];
	real field_258;
	byte unknown25c[0x6c];
	long slot_count;
	s_tag_slot *slots;
	long link_count;
	s_tag_link *links;
};

#define OBJECT_HEADER(index) (&((s_object_header *)g_4e0300->data)[(index) & 0xffff])
#define OBJECT_TAG(object) ((s_object_tag *)g_4e3b44[(object)->tag_index & 0xffff].data)

// @retail 0x101e80
long function_101e80(long object_index)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	if (object->slots[0].state || object->slots[1].state || object->field_178)
		return false;
	return true;
}

// @retail 0x101ec0
long function_101ec0(long object_index)
{
	s_object_header *header = OBJECT_HEADER(object_index);
	byte flags = header->flags;
	s_object *object = header->object;
	s_object_tag *tag = OBJECT_TAG(object);
	s_object_tag *const &selected = tag;

	if ((flags & 0x10) || (object->flags04 & 1) || selected->field_38 == NONE)
	{
		if (object->parent_index != NONE)
			object_index = object->parent_index;
	}
	return object_index;
}

// @retail 0x101f20
long function_101f20(long object_index)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	long result = NONE;

	if (TEST_FIELD_BIT(object->flag12c_0))
		result = object->child_index;
	return result;
}

// @retail 0x101f50
long function_101f50(long object_index)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	long child_index = NONE;

	if (TEST_FIELD_BIT(object->flag12c_0))
		child_index = object->child_index;

	long result = NONE;
	if (child_index != NONE)
	{
		s_object *child = OBJECT_HEADER(child_index)->object;
		long i = 0;
		do
		{
			if (child->attachments.objects[i] == object_index)
			{
				result = i;
				break;
			}
			i++;
		}
		while (i < 4);
	}
	return result;
}

s_object *function_badc0(long object_index, dword type_mask);

// @retail 0x101fb0
long function_101fb0(long object_index)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	long parent_index = object->parent_index;
	long result = NONE;

	if (parent_index != NONE)
	{
		s_object *parent = function_badc0(parent_index, 3);
		if (parent)
		{
			s_object_tag *tag = OBJECT_TAG(parent);
			if (!(TEST_FIELD_BIT(tag->flagbc_26) && parent->parent_index != NONE
				&& ((1 << OBJECT_HEADER(parent->parent_index)->type) & 3)))
			{
				result = parent->field_24c;
				if (result == NONE)
					result = parent_index;
			}
			else
			{
				parent_index = ((volatile s_object *)parent)->parent_index;
				s_object *ancestor = OBJECT_HEADER(parent_index)->object;
				s_object *const &selected = ancestor;
				result = selected->field_24c;
				if (result == NONE)
					result = parent_index;
			}
		}
	}
	return result;
}

/* an object, with its header's address formed as well although nothing
   reads it: that keeps a later lookup of the same object from reusing this
   one, so g_4e0300's data pointer and the object are read again for each
   lookup, as in retail (the idiom of 0xe5470's e5470_object_get) */
PRIVATE inline s_object *object_get_102100(long object_index)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	s_object_header *local_header = OBJECT_HEADER(object_index);

	return object;
}

// score 87 (was 110): the two lookups of the holder's child now read the
// pool's data pointer again as retail does. Left: retail keeps the object
// index times 12 in esi and the link offset in edx (ours swaps them), forms
// the tag link's address once in ecx, and loads the object's tag index
// into a register before comparing it with the other object's.
// Tried: the getter also for the holder or the other object (92), the
// child read through a local copy of the pool pointer (110), a forced-inline
// slot-to-object helper (110), tag and tag link declared before the link
// (179, 209, 211), the tag index comparison reversed (87).
// @retail 0x102100
bool function_102100(short link_index, long object_index, bool *out_a, bool *out_b)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	s_object_link *link = &object->links[link_index];
	s_object_tag *tag = OBJECT_TAG(object);
	bool result = false;
	s_tag_link *tag_link = &tag->links[link_index];
	bool blocked = false;
	real rate;

	real duration = (tag_link->end - tag_link->start) * link->rate + tag_link->start;
	if (duration > 0.0001f)
		rate = (real)g_510c54->field_2_3 / duration;
	else
		rate = 0.0f;

	if (tag->field_258 > 0.0f)
		rate = (object->field_184 * tag->field_258 + 1.0f) * rate;

	bool flag9 = TEST_FIELD_BIT(tag_link->flag9);
	if ((flag9 || TEST_FIELD_BIT(tag_link->flag10)) && tag->link_count > 1)
	{
		s_object_link *other = &object->links[link_index == 0];

		if (other->state == 1 && flag9)
			blocked = true;
		if ((other->state == 3 || other->state == 2) && TEST_FIELD_BIT(tag_link->flag10))
			blocked = true;
	}

	if (!blocked)
	{
		if (!link->time || (real)link->timer + 1.0f >= rate)
			result = true;
	}

	if (tag_link->minimum && link->time < tag_link->minimum)
	{
		*out_a = false;
		*out_b = false;
	}
	else if (tag_link->maximum && link->time >= tag_link->maximum)
	{
		*out_a = false;
		*out_b = true;
		result = false;
	}
	else
	{
		*out_a = true;
		*out_b = false;
	}

	if (link->flags & 0x40)
	{
		result = false;
		*out_b = true;
	}
	else if (result)
	{
		s_object *holder = OBJECT_HEADER(object_index)->object;
		if ((holder->flags12c & 1) && holder->child_index != NONE)
		{
			s_object *child = object_get_102100(holder->child_index);
			short first = child->attachments.first;
			long first_index = NONE;
			if (first != NONE)
				first_index = child->attachments.objects[first];

			child = object_get_102100(holder->child_index);
			short second = child->attachments.second;
			long second_index = NONE;
			if (second != NONE)
				second_index = child->attachments.objects[second];

			long other_index = NONE;
			if (object_index == first_index)
				other_index = second_index;
			if (object_index == second_index)
				other_index = first_index;

			if (other_index != NONE)
			{
				s_object *other = OBJECT_HEADER(other_index)->object;
				if (other->tag_index == object->tag_index && other->links[link_index].state == 1 && !other->links[link_index].timer)
					return false;
			}
		}

		if (1.0f > rate && rate > 0.0001f)
			link->accumulator += 1.0f / rate;
	}
	return result;
}

// @retail 0x102070
real function_102070(long object_index, short slot_index)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	s_object_tag *tag = OBJECT_TAG(object);
	real result = 0.0f;

	if (tag->slot_count > slot_index)
	{
		s_object_slot *slot = &object->slots[slot_index];
		switch (slot->state)
		{
		case 1:
			result = 1.0f - (real)slot->value / tag->slots[slot_index].duration * g_510c54->rate;
			break;
		case 2:
			result = 1.0f;
			break;
		}
	}
	return result;
}
