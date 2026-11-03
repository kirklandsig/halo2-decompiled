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

/* a compiler barrier ahead of the time stamp read, as 0x194260 has before its timer mark */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x223b60
__int64 timing_counter_stop(timing_counter *c)
{
	if (!c->stopped)
	{
		_ReadWriteBarrier();
		__int64 delta = read_tsc() - c->start;
		if (delta < 0) delta = 0;
		c->total += delta;
		c->stopped = true;
	}
	return c->total;
}
#pragma function(_ReadWriteBarrier)

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
