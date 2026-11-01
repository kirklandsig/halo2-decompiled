// @flags /O2 /GL- /GX
// Havok (third-party physics library): a filter over pairs of 32-bit keys.
// Two keys whose top 16 bits are equal and nonzero never pair; otherwise the
// pair is enabled by bit (second & 31) of word (first & 31) of a 32-word
// table. The filter answers through four interfaces: for two keys, and for
// two objects whose keys it reads.
#include <stddef.h>

#include "pair_filter_class.h"

class keyed_2df8f0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	// the object's kind, an index into a per-kind flags table
	virtual long slot_5() const { return 0; }
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	virtual void slot_13() {}
	virtual long key(long index) const { return 0; }
};

struct keyed_object_2df9d0
{
	char unknown0[0x1c];
	long key_1c;
	long key_20;
	long key_24;
};

// a pair's side seen as a chain: an object, the key of the part concerned
// (-1 for the whole), and the chain it belongs to; the outermost link carries
// the key at +0x1c
struct chain_2dfa20
{
	keyed_2df8f0 const *object;
	long part;
	long field_8;
	chain_2dfa20 const *parent;
	char unknown10[0xc];
	long key_1c;
};

// per-kind flags, bit 2: an object of that kind gives the key of its parts
struct kinds_2dfa20
{
	char unknown0[0x10c];
	unsigned long flags[1];
};

// the key of a chain's part: from the first link up whose kind gives keys
// for its parts, else 0
__forceinline long part_key_2dfa20(kinds_2dfa20 const *const *kinds, chain_2dfa20 const *chain)
{
	chain_2dfa20 const *link = chain->parent;
	do
	{
		if ((bool)(((*kinds)->flags[link->object->slot_5()] >> 2) & 1))
		{
			keyed_2df8f0 const *const object = link->object;
			long const part = chain->part;
			return object->key(part);
		}
		link = link->parent;
	}
	while (link);
	return 0;
}

struct pair_filter_settings_2dfb30
{
	long field_0;
	unsigned long enabled[32];
};
// @retail 0x2dfbc0 deleting c_2dfbf0
// @retail 0x2dfba0 adjustor c_2dfbf0 0xc
// @retail 0x2dfbb0 adjustor c_2dfbf0 0x10

// Retail 0x2dfbf0 installs four interface vptrs before field_18; its
// 32-word table ends at +0x9c. These assert storage only, not vtable identity.
typedef char assert_pair_filter_size[(sizeof(c_2dfbf0) == 0x9c) ? 1 : -1];
typedef char assert_pair_filter_field[(offsetof(c_2dfbf0, field_18) == 0x18) ? 1 : -1];
typedef char assert_pair_filter_table[(offsetof(c_2dfbf0, enabled) == 0x1c) ? 1 : -1];
typedef char assert_pair_reference_size[(sizeof(c_contact_rule_fallback) == 8) ? 1 : -1];
typedef char assert_pair_allocation_size[(offsetof(c_contact_rule_fallback, flags) == 4) ? 1 : -1];
typedef char assert_pair_reference_count[(offsetof(c_contact_rule_fallback, unknown06) == 6) ? 1 : -1];

// @retail 0x2df8a0
hkBool c_contact_rule_fallback::accepts(long a, long b)
{
	return static_cast<c_2dfbf0 &>(*this).test(a, b);
}

// @retail 0x2df8f0
hkBool c_2dfbf0::function_2df8f0(void const *arg_0, void const *arg_1, long arg_2)
{
	long b = ((keyed_2df8f0 const *)arg_1)->key(arg_2);
	return test(((keyed_object_2df9d0 const *)arg_0)->key_20, b);
}

// @retail 0x2df9d0
hkBool c_2dfbf0::function_2df9d0(void const *arg_0, void const *arg_1)
{
	return test(((keyed_object_2df9d0 const *)arg_0)->key_1c,
		((keyed_object_2df9d0 const *)arg_1)->key_1c);
}

// the key of a chain (its outermost link's for the whole, else its part's)
// against another object's key for one of its parts
// @retail 0x2dfa20
hkBool c_2dfbf0::function_2dfa20(kinds_2dfa20 const *const *kinds, chain_2dfa20 const *chain, long arg_2, keyed_2df8f0 const *other, long index)
{
	long const b = other->key(index);
	long a;
	if (chain->part == -1)
	{
		chain_2dfa20 const *root = chain;
		while (root->parent)
		{
			root = root->parent;
		}
		a = root->key_1c;
	}
	else
	{
		a = part_key_2dfa20(kinds, chain);
	}
	return test(a, b);
}

// @retail 0x2dfae0
hkBool c_2dfbf0::function_2dfae0(void const *arg_0, void const *arg_1)
{
	return test(((keyed_object_2df9d0 const *)arg_0)->key_24,
		((keyed_object_2df9d0 const *)arg_1)->key_1c);
}

// @retail 0x2dfb30
c_2dfbf0::c_2dfbf0(pair_filter_settings_2dfb30 const &settings)
{
	for (long i = 0; i < 32; i++)
	{
		enabled[i] = settings.enabled[i];
	}
	field_18 = settings.field_0;
}

// every pair enabled
// @retail 0x2dfbf0
c_2dfbf0::c_2dfbf0()
{
	for (long i = 0; i < 32; i++)
	{
		enabled[i] = 0xffffffff;
	}
	field_18 = 0;
}
