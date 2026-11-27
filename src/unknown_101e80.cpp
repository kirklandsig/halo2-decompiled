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

// score 31 (was 87): the link is read through the object at each use instead of through a `link` pointer local, so
// the link offset is made after the tag link's and loses the register tie to the object index (the index times 12
// now in ESI, the link offset in EDX, as retail). Left (709 B against 707): retail computes both link products ahead
// of the object lookup and forms the tag link in the 0xec product's register before copying it, where ours folds it
// into its first three reads; the two child lookups add the index first.
// At 87 (was 110): the two lookups of the holder's child now read the
// pool's data pointer again as retail does. Left: retail keeps the object
// index times 12 in esi and the link offset in edx (ours swaps them), forms
// the tag link's address once in ecx, and loads the object's tag index
// into a register before comparing it with the other object's.
// Tried: the getter also for the holder or the other object (92), the
// child read through a local copy of the pool pointer (110), a forced-inline
// slot-to-object helper (110), tag and tag link declared before the link
// (179, 209, 211), the tag index comparison reversed (87).
// Round 248: the object index times 12 and the link offset tie at register priority 76; the index wins the tie
// (later created) and takes EDX, so the link offset gets ESI; a forced-ranking search lifting the link offset to 77
// gives 24 (the tag link address and the tag index load still differ). Tried: a named minimum local for its test
// (87), the not-blocked test folded into one condition (87), the accumulator written through a fresh lookup (153,
// 740 B), the getter for the object itself (87), named flag9/flag10 bools (87), the two attachment indices as
// ternaries (159, 714 B), only the first (152), only the second (87, 711 B), the other index's first pick as a
// ternary (94), the holder's flag through the file's flag12c_0 bitfield, with or without TEST_FIELD_BIT (87, 87).
// Round 171: `real start = tag_link->start;` for the duration 180 (709 B), `real span = end - start` 87; the object's tag
// index read into a local ahead of the other object's lookup 87 (still folded into the compare); a pointer to the
// other object's link for its two tests 87; start local and tag index local together 180. Compile-only, the tag link
// is folded into its first three reads and formed afterwards (retail forms it first, then copies it) for `tag->links +
// link_index`, a links local, a reference, byte arithmetic, a long index, a const pointer and inline accessors taking
// a short or long index.
// Round 233: the tie is the object index (created later) against the link offset; one fewer candidate in any block of
// the attachment tail lifts the link offset to 77 and flips it, as the `other_index = NONE` start moved ahead of the
// second child lookup (74, tail code changed) or an else-if for the second pick (73, 710 B) show; the same flip
// comes from making the link offset after the tag link's: the `link` local declared after `tag_link` (31) or after
// `blocked` (31), or no `link` local (31, kept; same code); declared after `tag` 87, after `result` 87. On the kept
// form: the tag link at each use too 227 (737 B), tag_link declared ahead of `result` 31, the duration product
// written rate-first 31, the child lookups without the getter 117; the existing unit weapon-slot getter 0xcbd50 for
// the two picks 207 (it stays a call). Other probes on main: a second child local, the index compares written
// index-first and a named child index all leave the tie (87).
// GA23 (42 fast trials, 6 peers MATCH): child weapon slot lookup in a shared inline helper
// `unit_hand_weapon_102100(holder->child_index, 0/1)` (or index-first SIB in `object_get_102100`) 29 (al 24->22,
// fixes both `[ecx+eax+8]`/`[ecx+esi+8]` lines; NOT kept: 2-pt gain under keep bar). On 29/31: `real_bounds` +
// `real_bounds_interpolate` (inline / `__forceinline`) 31 (by-value 232), `tag_link` duration/rate inlines 31,
// `offset + base` 31, `tag_block` inline helpers 31, `long idx` at top 265, dead `tag_link` copy/helper 31,
// `inline_depth(2)` / no-`/Ob1` diag 31/29, `s_tag_link const &` / `+=` / decl at top / duration operand orders /
// `end` local / `weapon_get`+`weapon_tag_get` / `(long)link_index` / `__forceinline` ptr / decl reorder / 2-ptr
// duration / `101f20` holder inline / `long tag_index`+`.bytes` / `byte *links` / `dword flags` / ternary rate /
// `real_bounds *` local / `object_link_get` / duration-from-tag inline all 29 (same code: `tag_link` still folds into
// `[ecx+eax+8]`); `link` after `tag_link` 31; `rate = 0` no-else 197 (707 B); bools after duration 179 (708 B);
// `slots[2]` array 232; diag `s_tag_link *const &` 29 (still folds), `volatile` ptr 247. Body unchanged.
// M3 lane 14 r162 (fast). Reading: retail's entry block differs by register choice, and the schedule follows from it:
// retail keeps the tag table in EBP from the top (the tag then takes EBP too), the tag index in ECX and the 0xec product
// in EAX apart from the tag link (ECX), so the links pointer is read once as the add's memory operand; ours gives the
// product the tag link's register, reads the links pointer into EAX and forms the sum last, after its three reads
// (forced scheduler heights and ready times change nothing: 40 runs at 31). Tried: the object through
// object_get_102100 29 (only the object read's operand order moves), the object's header as a local with the object
// read from it 30, the duration and its test in a block of their own 31, the tag link taken from OBJECT_TAG(object)
// ahead of the tag local 31, an object link pointer declared after the tag link for the rate 31.
// M3 lane GA r27 (16 fast trials on base 31 / index-first SIB 29, all 6 file peers MATCH; body unchanged): param order
// `(object_index, link_index, ...)` 29 (caller 0xfdfb0 updated), tag-block operator[]/ref helpers 29/29,
// `s_real_bounds const &` duration helper 29, const tag/link 29, void* `tag_get` inline 29, `rate` local ahead of
// `tag_link` 29, single-exit tail 29, offset+base byte pointer 29, `g_4e3b44` byte arith/local first 29/29, `0x101ec0`
// selected-ref idiom 29 (all keep `[ecx+eax+8]` SIB fold and `g_4e3b44` in EAX); `link` ptr in `!blocked` 31;
// `object->links` ptr 265; dual-wield partner inline 298 (613 B, still folds `[eax+edx+8]`).
// M3 lane GB r29 (24 valid fast trials, all 6 file peers MATCH; body unchanged): no `flag9` local 31, `blocked` after
// duration 31, `real bounds[2]` 31, early `link_rate` / `tag_off` / both offsets 31, `rate *=` 31, `void *links` 31,
// `long tag_index` 31, a tag-block `void*` accessor 31, `tag->links[(long)link_index]` 31, `object->links[(long)link_index]`
// 31, `void *object` + `.bytes` 31, nested `rof` struct 31, no-`/Ob1` + byte-offset `object_get_102100` 31; `blocked` before
// `result` 33; `duration <= 0.0001f` 38; late `s_object_link *link` 172 (716 B); `rate = 0` + reordered duration 197 (707 B);
// late `tag_link` 203; `(word)` tag index 203 (704 B, breaks 3 peers); scalar interp helper 205 (711 B); late `result` 211;
// by-value `real_bounds rof` copy 232 (733 B, breaks `[ecx+eax+8]` fold only via 8-byte stack copy, still loads `[ebp+0x2d4]`
// first and keeps `g_4e3b44` in `EAX`).
// M3 lane GB r31 (31 valid fast trials + 1 diag, all 6 file peers MATCH; body unchanged): member methods
// `s_tag_link::interpolate` / `s_object_tag::get_link` / `s_bounds::interpolate`, ternary `rate`, `dword flags` +
// `TEST_FIELD_BIT`, `long barrel` after `tag`, `s_tag_link &`, `tag_link += link_index`, inline `rate` helper,
// byte-arith both, `!link_index`, `min_t` local, inline obj/tag getters, a tag block struct, `start + rate * span`,
// and diag `optimize("w")` all 31 (`29` on index-first SIB base with `span` or `real_bounds *rof`, `29` with tag-only
// `long barrel`); `long barrel = link_index` at top 30 (`al=23, mk=16`, lifts `movsx` above `tag` and matches `xorps`
// slot, -1 under keep bar; `30` with idx-first SIB / `tags` local / `span` / obj-only `barrel`); C89 top decls 136
// (707 B); `flag9` before `duration` 206; `accum` ptr 47; plain bitfields 211; explicit `obj_off` (and `tag_off`) at
// top covering all 6 link uses 39 (`mk=10`, first form putting `g_4e3b44` in `EBP` and emitting `imul 0xec` before
// `push edi; imul 0x34`, but `tag_off` still coalesces to `ECX` and keeps the `[ecx+eax+8]` SIB fold; after `object`
// 87 at 707 B with both `EBP` and `ECX` for `g_4e0300->data`; after `tag` 87; after `headers` 42; + late bools 211).
// @retail 0x102100
bool function_102100(short link_index, long object_index, bool *out_a, bool *out_b)
{
	s_object *object = OBJECT_HEADER(object_index)->object;
	s_object_tag *tag = OBJECT_TAG(object);
	bool result = false;
	s_tag_link *tag_link = &tag->links[link_index];
	bool blocked = false;
	real rate;

	real duration = (tag_link->end - tag_link->start) * object->links[link_index].rate + tag_link->start;
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
		if (!object->links[link_index].time || (real)object->links[link_index].timer + 1.0f >= rate)
			result = true;
	}

	if (tag_link->minimum && object->links[link_index].time < tag_link->minimum)
	{
		*out_a = false;
		*out_b = false;
	}
	else if (tag_link->maximum && object->links[link_index].time >= tag_link->maximum)
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

	if (object->links[link_index].flags & 0x40)
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
			object->links[link_index].accumulator += 1.0f / rate;
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
