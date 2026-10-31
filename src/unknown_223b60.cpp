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

// score 15 (was 28): each channel's raw bits read as the word shifts, then expanded, as retail;
// left: the three ORs put their result in the shift's copy register (retail keeps it in the
// channel's: `or dl, bl`, `or cl, dl`, `or al, cl`), the two slot loads come byte then dword
// (retail dword first), and the out pointer is loaded before the channel stores.
// Round 39: blue expanded in the colour field (`color.b = b << 3; color.b |= color.b >> 5;`)
// with green and red read already shifted and expanded in their locals 12 (blue then exact;
// not kept: the three rows written two ways); all three expanded in the fields 28 (the slot
// loads sink below the blue store, separate frame slots); the shifts first and the ORs in the
// stores 15; all shifts, then all ORs 15; field stores ORed with the shifted locals 15; every
// channel read already shifted 29; a field store per channel then a field OR 28; 180 statement
// orders and spellings of the shifted-word form 29.
// Before round 39, score 28 (was 28): the conversion is correct, but our compiler keeps the union local
// in registers (`movzx eax`; `shr ax`) while retail keeps it in memory
// (`xor ecx, ecx`; `mov cx, [eax]`; `shr word ptr [esp + 4], 5`) and puts the
// blue channel in dl. No source shape or annealed permuter run reproduced it.
// A plain word local shifted in place with a separate colour gives retail's load,
// in-memory shift and shared slot (29), but blue is then computed in the word's own
// register after the slot store; retail computes it in a copy (dl) first. Blue stored
// into the colour straight after its OR 26 (blue first, colour gets its own slot);
// blue from a byte copy in one expression 26 (89 bytes); long blue 28 (90 bytes);
// channels written into the colour fields 28; colour object through address casts 29;
// word copy for the shifts 42 (73 bytes); 20 statement orders and channel spellings 29.
// @retail 0x223e70
void function_223e70(const word *rgb, S3TC_COLOR *out)
{
	S3TC_COLOR color;
	word w = *rgb;
	byte b = (byte)w;
	w >>= 5;
	byte g = (byte)w;
	w >>= 6;
	byte r = (byte)w;
	b <<= 3;
	b |= b >> 5;
	g <<= 2;
	g |= g >> 6;
	r <<= 3;
	r |= r >> 5;
	color.b = b;
	color.g = g;
	color.r = r;
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
