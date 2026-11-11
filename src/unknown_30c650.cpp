// @flags /O2 /GL- /GX
// Havok (third-party physics library): inserting another array's pointers
// into an array of pointers at an index, and gathering the pointers an
// object's members list into one array.
#include "growable_array_2d91d0.h"

struct pointers_30c650
{
	ELEMENTS_2D91D0_MEMBERS(void *)

	pointers_30c650() : data(0), size(0), capacity_and_flags(0x80000000) {}
	// a view of `count` pointers at `elements`, whose storage it does not own
	pointers_30c650(void **elements, long count, long capacity_flags)
	{
		size = count;
		data = elements;
		capacity_and_flags = capacity_flags;
	}
	~pointers_30c650()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(void *), 0x12);
		}
	}

	void function_30c650(long index, pointers_30c650 const *other);
};

// @retail 0x30c650
void pointers_30c650::function_30c650(long index, pointers_30c650 const *other)
{
	long count = other->size;
	long new_size = size + count;
	long tail = size - index;
	ensure_capacity(new_size);
	void **from = data + index;
	void **to = data + (count + index);
	long i;
	for (i = tail - 1; i >= 0; i--)
	{
		to[i] = from[i];
	}
	void *const *source = other->data;
	void **destination = data + index;
	for (i = count - 1; i >= 0; i--)
	{
		destination[i] = source[i];
	}
	size = new_size;
}

// an entry of the first gathered list, with a flag at +0xc
struct entry_30ced0
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

// an object a member lists at +0x58; slot 8 says its kind
class part_30cf70
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual int slot_8() { return 0; }
};

struct members_30d480_parts
{
	void **data;
	long size;
	long capacity_and_flags;
};

struct member_30d480
{
	char field_0[0x3c];
	pointers_30c650 field_3c;
	long field_48;
	pointers_30c650 field_4c;
	// part_30cf70 pointers
	members_30d480_parts parts;
};

struct members_30d480
{
	member_30d480 **data;
	long size;
	long capacity_and_flags;
};

struct owner_30d480
{
	long field_0;
	long field_4;
	members_30d480 first_members;
	members_30d480 second_members;
	long field_20;
	long field_24;
	long field_28;
	member_30d480 *field_2c;
	char field_30[0xd0 - 0x30];
	void *field_d0;
	char field_d4[0x120 - 0xd4];
	void **field_120;
	long field_124;
};

// the pointers in the +0x3c lists of the owner's +0x2c member and of every
// member of its two lists; the first is dropped (the last moved into its
// place) when its +0xc flag is clear
// @retail 0x30ced0
static void __cdecl function_30ced0(pointers_30c650 *out, owner_30d480 const *owner)
{
	if (owner->field_2c)
	{
		out->function_30c650(0, &owner->field_2c->field_3c);
	}
	if (((entry_30ced0 *)out->data[0])->field_c == 0)
	{
		out->size--;
		out->data[0] = out->data[out->size];
	}
	member_30d480 *const *member;
	for (member = owner->first_members.data; member != owner->first_members.data + owner->first_members.size; member++)
	{
		out->function_30c650(out->size, &(*member)->field_3c);
	}
	for (member = owner->second_members.data; member != owner->second_members.data + owner->second_members.size; member++)
	{
		out->function_30c650(out->size, &(*member)->field_3c);
	}
}

// the parts of every member of the owner's two lists, except those of kind 11
// @retail 0x30cf70
static void __cdecl function_30cf70(pointers_30c650 *out, owner_30d480 const *owner)
{
	for (member_30d480 *const *first = owner->first_members.data; first != owner->first_members.data + owner->first_members.size; first++)
	{
		members_30d480_parts const *parts = &(*first)->parts;
		long part_count = parts->size;
		for (long part = 0; part < part_count; part++)
		{
			if (((part_30cf70 *)parts->data[part])->slot_8() != 11)
			{
				out->append_copy(parts->data[part]);
			}
		}
	}
	for (member_30d480 *const *second = owner->second_members.data; second != owner->second_members.data + owner->second_members.size; second++)
	{
		members_30d480_parts const *parts = &(*second)->parts;
		long part_count = parts->size;
		for (long part = 0; part < part_count; part++)
		{
			if (((part_30cf70 *)parts->data[part])->slot_8() != 11)
			{
				out->append_copy(parts->data[part]);
			}
		}
	}
}

// the pointers in the +0x4c lists of every member of the owner's two lists
// @retail 0x30d0c0
static void __cdecl function_30d0c0(pointers_30c650 *out, owner_30d480 const *owner)
{
	member_30d480 *const *member;
	for (member = owner->first_members.data; member != owner->first_members.data + owner->first_members.size; member++)
	{
		out->function_30c650(out->size, &(*member)->field_4c);
	}
	for (member = owner->second_members.data; member != owner->second_members.data + owner->second_members.size; member++)
	{
		out->function_30c650(out->size, &(*member)->field_4c);
	}
}

// appends everything the owner lists to `out`: the +0x3c lists, the owner's
// own +0x120 list, the parts, the +0x4c lists, then its +0xd0 pointer
// The temporary-view setup and cleanup still differ in instruction scheduling
// and register allocation; the ordinary checker reports score 56.
// appends everything the owner lists to `out`: the +0x3c lists, the owner's
// own +0x120 list, the parts, the +0x4c lists, then its +0xd0 pointer
// score 152 (was 183): retail keeps the zero in eax and the gathered array's
// data in ebx and computes the freed size with lea, ours keeps the zero in
// ebx, reloads the data pointer and computes the size with shl.
// Explicit capacity 152; distinct array types 183, with capacity 152;
// capacity word 181; object names 152; size-first view stores 201;
// empty-array stores, a view base and pointer-first view stores 152;
// unsigned capacity 183.
// score 72 (was 152): the first EDI/ESI setup is reversed; the ownership
// word is formed before the size store, and release bytes overwrite ESI.
// Capacity snapshot 152; named byte count 199; const locals, a storage pointer,
// copy initialization of the locals and a forced inline release 152; split
// empty initialization 154. Signed byte counts, capacity plus byte count,
// ordered stores plus byte count and const byte count 199; split empty
// initialization plus byte count 201. Complete flags plus byte count 80;
// argument orders 199; complete flags with ordered stores 76;
// the capacity parameter renamed 76. Kept: complete flags with ordered stores
// and the byte count written in the call 72 (the named byte count, 76, is a
// one-use local and not kept).
// Further trials: signed ownership 183; view member initializers 181;
// release function defined separately, input/output object references, unsigned
// complete flags and distinct gathered types 72; cleanup resets 122;
// signed release multiplication and a full-flags snapshot 72.
// Review cleanup score 72: removed unsupported volatile/one-use tuning;
// the prior scored trials above remain available for future matching work.
// @retail 0x30d480
void __cdecl function_30d480(owner_30d480 const *owner, pointers_30c650 *out)
{
	{
		pointers_30c650 gathered;
		function_30ced0(&gathered, owner);
		pointers_30c650 view(gathered.data, gathered.size, gathered.size | 0x80000000);
		out->function_30c650(out->size, &view);
	}
	if (owner->field_124 > 0)
	{
		pointers_30c650 view(owner->field_120, owner->field_124, owner->field_124 | 0x80000000);
		out->function_30c650(out->size, &view);
	}
	{
		pointers_30c650 gathered;
		function_30cf70(&gathered, owner);
		pointers_30c650 view(gathered.data, gathered.size, gathered.size | 0x80000000);
		out->function_30c650(out->size, &view);
	}
	{
		pointers_30c650 gathered;
		function_30d0c0(&gathered, owner);
		pointers_30c650 view(gathered.data, gathered.size, gathered.size | 0x80000000);
		out->function_30c650(out->size, &view);
	}
	void *last = owner->field_d0;
	if (last)
	{
		out->append_copy(last);
	}
}
