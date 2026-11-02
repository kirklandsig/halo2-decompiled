// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_223b60.h"

__int64 timing_counter_peek(timing_counter *c);

static inline __int64 read_tsc(void)
{
	volatile __int64 t = 0;
	__asm rdtsc
}

// @retail 0x223af0
timing_counter *timing_counter_start(timing_counter *c)
{
	((volatile timing_counter *)c)->total = 0;
	c->start = read_tsc();
	c->stopped = false;
	return c;
}

// @retail 0x223b20
__int64 timing_counter_resume(timing_counter *c)
{
	__int64 total = timing_counter_peek(c);

	if (c->stopped)
	{
		((volatile timing_counter *)c)->stopped = false;
		c->start = read_tsc();
	}
	return total;
}

// score 4 (was 4): identical body and length; our prologue stores the first delta
// zero before pushing esi, while retail pushes esi first and then stores both
// zero dwords. Every spelling of the reader and of the caller ties at 4, and the
// permuter found no better variant; only a barrier or a volatile anchor gives
// retail's order, and both are barred. The tick local zeroed by memset 50 (stores
// dropped); a volatile double 10; a volatile LARGE_INTEGER `= { 0 }` 50; total and
// stopped declared volatile in the struct 34 (push esi first, then a push edi);
// stopped alone 4; total alone 34.
// Round 41: reversed total addition 4; branch-duplicated totals 26; branch-duplicated
// total/stopped stores 24; explicit false guard 4; const pointer formal 4; entry total
// with reversed addition 29; delta-only rename 4; reversed clamp comparison 4.
// J199: existing shared timestamp producer 4; reader cdecl/stdcall 4/4; separate
// default-zero/exhaustive clamped outputs 40/4; inline peek accumulation with
// default/cdecl/stdcall reader 77/77/77 (resume loses its match). Original restored.
// Scoped stop stdcall/cdecl 4/4; unsigned result with default/stdcall/cdecl 4/4/4;
// only the queued signature and its existing caller declaration changed, then restored.
// Round 16: a non-volatile tick local 50 (stores dropped in all four); an inline clamped
// elapsed-ticks helper shared with peek 4; a forceinline reader 4; an early return when
// already stopped 4; the reader with external linkage 4.
// Round 61: timestamp then subtract 29; negated start then timestamp 43; LARGE_INTEGER delta 4;
// standalone clamp ternary 4; delta named t 4; stopped snapshot 4; inner delta/total scope 4;
// zero-initialized delta 4. Original body restored.
// Round 33 (lane 2, fast): `total = total + delta` 4; stopped stored before the total 6; the total
// added through a field pointer 4; the total returned through a local 4; `stopped == false` 4; an
// empty stopped arm with the work in else 4. CPU-target flags /G5, /G6, /G7 and /GB compile alike.
// @retail 0x223b60
__int64 timing_counter_stop(timing_counter *c)
{
	if (!c->stopped)
	{
		__int64 delta = read_tsc() - c->start;
		if (delta < 0) delta = 0;
		c->total += delta;
		c->stopped = true;
	}
	return c->total;
}

// @retail 0x223bb0
double timing_ticks_to_seconds(__int64 ticks)
{
	const __int64 frequency = 733333333;
	__int64 q = ticks / frequency;
	__int64 r = ticks - q * frequency;
	return (double)r / (double)frequency + (double)q;
}

// @retail 0x223c20
__int64 timing_counter_peek(timing_counter *c)
{
	__int64 total = c->total;
	if (!c->stopped)
	{
		__int64 delta = read_tsc() - c->start;
		if (delta < 0) delta = 0;
		total += delta;
	}
	return total;
}

// @retail 0x223e70
void function_223e70(const word *rgb, S3TC_COLOR *out)
{
	S3TC_COLOR color;
	word w = *rgb;
	color.b = (byte)w;
	w >>= 5;
	color.g = (byte)w;
	w >>= 6;
	color.r = (byte)w;
	color.b <<= 3;
	color.g <<= 2;
	color.r <<= 3;
	color.b |= color.b >> 5;
	color.g |= color.g >> 6;
	color.r |= color.r >> 5;
	*out = color;
}

PRIVATE inline s_fixup_element *fixup_entry_target(s_fixup_entry *entry, byte *base8)
{
	if (entry->target_index == 0xFFFF)
		return (s_fixup_element *)(base8 + entry->target_offset);
	return (s_fixup_element *)(*(byte **)(base8 + entry->target_offset + 4) + (entry->target_index << 5));
}

// @retail 0x223c70
void fixup_group_apply(s_fixup_group *group, byte *base)
{
	byte *base8 = base + 8;
	s_fixup_pointer *root = (s_fixup_pointer *)((byte *)group + group->pointer_offset);
	root->count = 1;
	root->pointer = base8;
	for (long i = 0; i < group->entry_count; i++)
	{
		s_fixup_entry *entry = &group->entries[i];
		if (entry->offset == NONE)
			continue;
		byte *address = base + group->data_offset + entry->offset + 8;
		switch (entry->type)
		{
		case 0:
		{
			s_fixup_pointer *target = (s_fixup_pointer *)(base8 + entry->target_offset);
			target->count = entry->value / entry->target_index;
			target->pointer = address;
			break;
		}
		case 1:
		{
			s_fixup_pointer *target = (s_fixup_pointer *)(base8 + entry->target_offset);
			target->count = entry->value;
			target->pointer = address;
			break;
		}
		default:
		{
			s_fixup_element *element = fixup_entry_target(entry, base8);
			short quotient = (short)((entry->value - element->base) / element->divisor);
			element->relative = (long)address - element->unknown4;
			element->quotient = quotient;
			s_fixup_trailer *trailer = &element->inline_trailer;
			element->trailer = trailer;
			trailer->count = 1;
			trailer->packed = (dword)address - element->unknown4;
			trailer->unknown8 = 0;
			trailer->packed &= 0xFFFFFFF;
			break;
		}
		}
	}
}

PRIVATE inline bool fixup_element_busy_223d80(s_fixup_element *element)
{
	D3DResource *resource = (D3DResource *)element->trailer;
	bool busy = false;
	if (resource)
		busy = resource->IsBusy() > 0;
	return busy;
}

// @retail 0x223d80
bool fixup_group_has_resource(s_fixup_group *group, byte *base)
{
	bool result = false;
	byte *base8 = base + 8;
	for (long i = 0; i < group->entry_count; i++)
	{
		s_fixup_entry *entry = &group->entries[i];
		if (entry->offset != NONE && entry->type == 2)
		{
			if (fixup_element_busy_223d80(fixup_entry_target(entry, base8)))
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

// @retail 0x223e00
void fixup_group_release_resources(s_fixup_group *group, byte *base)
{
	byte *base8 = base + 8;
	for (long i = 0; i < group->entry_count; i++)
	{
		s_fixup_entry *entry = &group->entries[i];
		if (entry->offset != NONE && entry->type == 2)
		{
			D3DResource *resource = (D3DResource *)fixup_entry_target(entry, base8)->trailer;
			if (resource)
				resource->BlockUntilNotBusy();
		}
	}
}
