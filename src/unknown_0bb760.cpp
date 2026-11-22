#include "object_tag_definitions.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"
#include "object_tag_definitions.h"

// @flags /O2 /arch:SSE /Gr

/* local views of the object, its header and the object list (the shared
   header data is s_object_header_data in globals.h; 0bad50 and 108a90 keep
   their own views of the same objects) */
struct s_object
{
	byte unknown00[4];
	union
	{
		dword flags;
		struct
		{
			dword : 14;
			dword flag14 : 1;
			dword flag15 : 1;
			dword : 16;
		};
	};
	byte unknown08[0x14];
	long next_index;
	long next_time;
	byte export24;
	byte export25;
	byte export_flags;
	byte unknown27[0xad];
	long unknownd4;
	byte unknownd8;
};

struct s_object_header
{
	short identifier;
	byte flag0 : 1;
	byte : 7;
	byte type;
	byte unknown04[4];
	s_object *object;
};

struct s_object_list
{
	byte unknown00[4];
	short count;
	short unknown06;
	long first_index;
	byte unknown0c[8];
	long time14;

};

long *g_4de2d0;
s_object_list *g_4de2f4;

struct s_scenario_kind_ab
{
	byte unknown00[0x10];
	short type;
};

// @retail 0xbc970
bool function_bc970(long object_index, long id, real *value_out)
{
	real value = 0.0f;
	bool result = false;
	if (object_index != NONE)
	{
		s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		s_scenario_kind_ab *scenario = (s_scenario_kind_ab *)g_4e0350;
		if ((object->export_flags & 8) || !scenario || scenario->type != 0)
		{
			switch (id)
			{
			case 0x0d0005ff: value = 1.0f; break;
			case 0x170005fa: value = object->export24 * (1.0f / 64.0f); break;
			case 0x170005fb: value = object->export25 * (1.0f / 32.0f); break;
			case 0x180005fc: if (!(object->export_flags & 1)) value = 1.0f; break;
			case 0x1d0005fd: value = (object->export_flags & 2) ? 1.0f : -1.0f; break;
			case 0x1d0005fe: value = (object->export_flags & 4) ? 1.0f : -1.0f; break;
			}
			result = true;
		}
	}
	*value_out = value;
	return result;
}

struct s_local_time
{
	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;
};

void function_139030(s_local_time *time);
long function_139090();

// @retail 0xbca70
real function_bca70()
{
	long stamp = g_4de2f4->time14;
	real result = 1.0f;
	if (stamp && g_510c54->game_time > stamp)
	{
		real elapsed = (g_510c54->game_time - stamp) * g_510c54->rate;
		if (elapsed > 1.0f)
			result = 0.0f;
		else if (elapsed > 20.0f)
			result = 1.0f;
		else
			result = (elapsed - 15.0f) * 0.2f;
	}
	return result;
}

// @retail 0xbcad0
real function_bcad0()
{
	s_local_time time;
	function_139030(&time);
	long hour = time.hour;
	if (hour < 1)
		hour = 1;
	else if (hour > 24)
		hour = 24;
	return (hour % 12) / 12.0f;
}

// @retail 0xbcb10
real function_bcb10()
{
	s_local_time time;
	function_139030(&time);
	long minute = time.minute;
	if (minute < 0)
		minute = 0;
	else if (minute > 59)
		minute = 59;
	return minute / 60.0f;
}

// @retail 0xbcb60
real function_bcb60()
{
	s_local_time time;
	function_139030(&time);
	long second = time.second;
	if (second < 0)
		second = 0;
	else if (second > 59)
		second = 59;
	return second / 60.0f;
}

// @retail 0xbcbb0
real function_bcbb0()
{
	/* Retail retains the scratch value's stores, including the initial zero. */
	volatile real result = 0.0f;
	switch (function_139090())
	{
	case 1: result = 0.25f; return 0.25f;
	case 2: result = 0.5f; return 0.5f;
	case 3: result = 0.75f; return 0.75f;
	case 4: result = 1.0f; break;
	}
	return result;
}

void object_dispatch_attach_simulation_entity(long object_index);
void object_dispatch_detach_simulation_entity(long object_index);
void object_dispatch_object_removed(long object_index, long a);
void object_dispatch_player_removed(long object_index, long a);

// @retail 0xbb760
long function_bb760(short index)
{
	if (index >= 0 && index < 0x280)
		return g_4de2d0[index];
	return NONE;
}

// @retail 0xbb780
void function_bb780(long object_index, long value)
{
	((s_object_header *)g_4e0300->data)[object_index & 0xffff].object->unknownd4 = value;
	object_dispatch_attach_simulation_entity(object_index);
}

__declspec(noinline) void function_bb7b0(long object_index);

// @retail 0xbb7b0
void function_bb7b0(long object_index)
{
	s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	object_dispatch_detach_simulation_entity(object_index);
	object->unknownd4 = NONE;
	object->unknownd8 = 0;
}

PRIVATE __forceinline void object_clear_notification_inline(long object_index)
{
	s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	object_dispatch_detach_simulation_entity(object_index);
	object->unknownd4 = NONE;
	object->unknownd8 = 0;
}

// @retail 0xbb7f0
void function_bb7f0()
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		if (state.object->unknownd4 != NONE)
			object_clear_notification_inline(state.iterator.object_index);
	}
}

// @retail 0xbb880
void __stdcall function_bb880(long a)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;

	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		object_dispatch_object_removed(state.iterator.object_index, a);
	}
}

// @retail 0xbb8f0
void function_bb8f0(long a)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		object_dispatch_player_removed(state.iterator.object_index, a);
	}
}

// matches retail (was 4): the new deadline is the delta plus the game time, and the
// comparison reads the old deadline and that sum on both sides of it.
// score 4 (was 4): retail keeps the delta in the accumulator and adds the
// game time to it, ours keeps the game time in the accumulator and adds the
// delta to that, so the four instructions that build the time differ.
// Delta-first clock getter 4; old-deadline declaration before sum 4; branch-entry declaration homes 4;
// const old deadline 4; const delta 4; const object pointer 4; const header pointer 4; scoped favor-size 88.
// delta + game time 4; the deadline started as delta, the game time added 4; the old deadline
// read in the select 4; if-form max 4; the select as if/else stores 32; a conditional store 32.
// @retail 0xbb950
void function_bb950(long object_index, bool add, long delta)
{
	s_object_header *header = &((s_object_header *)g_4e0300->data)[object_index & 0xffff];
	s_object *object = header->object;

	if (add)
	{
		if (!TEST_FIELD_BIT(object->flag14) && !TEST_FIELD_BIT(object->flag15))
		{
			s_object_list *list = g_4de2f4;
			object->next_index = list->first_index;
			list->first_index = object_index;
			object->flag14 = 1;
			if (TEST_FIELD_BIT(header->flag0))
				list->count++;
		}
		object->next_time = (object->next_time > delta + g_510c54->game_time) ? object->next_time : delta + g_510c54->game_time;
	}
	else if (TEST_FIELD_BIT(object->flag14))
	{
		s_object_list *list = g_4de2f4;
		long *link = &list->first_index;
		while (*link != object_index)
			link = &((s_object_header *)g_4e0300->data)[*link & 0xffff].object->next_index;
		long &next = object->next_index;
		*link = next;
		next = NONE;
		object->flags &= ~0x4000UL;
		if (TEST_FIELD_BIT(header->flag0))
			list->count--;
	}
}

#include "unknown_0259d0.h"
#include <math.h>

transform4x3f *function_b8bd0(long object_index, short node_index);
real function_0bff60(real a, real b);
real function_c18e0();
real function_1588b0(long player_index, long type);

// @retail 0xbcc20
bool __stdcall function_bcc20(long object_index, long id, real *out, bool *active)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    bool forced_active = false;
    bool result = true;
    real value = 0.0f;
    switch (id)
    {
    case 0x030005a6: value = 1.0f; break;
    case 0x040005a7: value = 0.0f; break;
    case 0x0500055b: value = TEST_FIELD_BIT(((s_object_flags_10a *)object)->bit2) ? 0.0f : 1.0f; break;
    case 0x0700055c:
        {
            transform4x3f *matrix = function_b8bd0(object_index, 0);
            if (fabs(matrix->forward.k) < 0.995f)
            {
                real angle = (real)atan2(matrix->forward.i, matrix->forward.j);
                value = function_0bff60(angle, *(real *)((byte *)g_4e0350 + 0x1c)) * 0.15915493667125702f + 0.5f;
            }
            else
                value = 1.0f;
        }
        break;
    case 0x0700055d:
        {
            byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
            byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
            if (*(long *)(model + 0x50) > 0)
            {
                long variant = *(char *)(object + 0xb1) + 1;
                variant = variant < 0 ? 0 : variant > *(long *)(model + 0x50) ? *(long *)(model + 0x50) : variant;
                value = (real)variant / *(long *)(model + 0x50);
            }
        }
        break;
    case 0x070006ba: value = function_bcbb0(); break;
    case 0x090006b5: value = function_bca70(); break;
    case 0x0a0006b6: value = function_bcad0(); break;
    case 0x0a0006b9: value = 1.0f; break;
    case 0x0c0006b7: value = function_bcb10(); break;
    case 0x0c0006b8: value = function_bcb60(); break;
    case 0x0d000558: value = *(real *)(object + 0xec); break;
    case 0x0f000559: value = *(real *)(object + 0xf0); break;
    case 0x0f0005aa: value = (((dword)object_index * 0x19660d + 0x3c6ef35f) >> 16) * 0.000015259021893143654f; break;
    case 0x0f0005ac:
        if ((g_4e6948->state != 2 || !((1 << object[0xaa]) & 3) || *(long *)(object + 0x13c) == NONE ||
            function_1588b0(*(long *)(object + 0x13c), 2) > 0.0f) && *(real *)(object + 0xf0) == 0.0f)
            value = 1.0f;
        else
            value = 0.0f;
        break;
    case 0x100006b4: value = 1.0f; break;
    case 0x13000556: value = *(real *)(object + 0xf8); break;
    case 0x130006bb: value = function_c18e0(); break;
    case 0x15000557: value = *(real *)(object + 0xf4); break;
    case 0x1800055a: value = *(real *)(object + 0xf0) - 1.0f; break;
    case 0x1a00071a: value = *(real *)((byte *)g_4de2f4 + 0x70); break;
    case 0x1a00071b: value = *(real *)((byte *)g_4de2f4 + 0x74); break;
    case 0x1a00071c: value = *(real *)((byte *)g_4de2f4 + 0x78); break;
    case 0x1a00071d: value = *(real *)((byte *)g_4de2f4 + 0x7c); break;
    case 0x0d0005ff: case 0x170005fa: case 0x170005fb:
    case 0x180005fc: case 0x1d0005fd: case 0x1d0005fe:
        forced_active = function_bc970(object_index, id, &value); break;
    default: result = false; break;
    }
    if (value < 0.0f)
        value = 0.0f;
    else if (value > 1.0f)
        value = 1.0f;
    *out = value;
    if (forced_active)
        *active = true;
    else
        *active = value > 0.0f;
    return result;
}

/* the base object type (0x4678e8): its name, group tag, datum size, its
   callbacks and its type list, which holds only itself */
struct s_object_type_definition_0bb760
{
    char const *name;
    long group_tag;
    short datum_size;
    short unknown0a;
    long unknown0c;
    void *functions[29];
    void *types[2];
    byte unknown8c[0xc8 - 0x8c];
};

void __stdcall function_be760(long index);
void __stdcall function_72c70(dword value);

extern s_object_type_definition_0bb760 g_4678e8;

s_object_type_definition_0bb760 g_4678e8 =
{
    "object",
    'obje',
    0x12c,
    NONE,
    NONE,
    {
        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL,
        NULL, NULL, (void *)function_be760, NULL,
        NULL, NULL, NULL, (void *)function_bcc20,
        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL,
        (void *)function_72c70
    },
    { &g_4678e8, NULL }
};

struct s_colour_choice_ab
{
    real weight;
    color3f lower;
    color3f upper;
    long field_1c_4;
};
struct s_colour_choices_ab
{
    long count;
    s_colour_choice_ab *choices;
    long function_count;
    struct s_colour_function_ab *functions;
};
hsv3f *function_1318d0(color3f const *rgb, hsv3f *hsv);
color3f *function_131a00(hsv3f const *hsv, color3f *rgb);
void function_3dd10(long object_index, bool force);

// score 77 (was 77): The pre-guard ordinal store, scan count and mask homes, and signed weight-scan exits still differ.
// Earlier comparison 97 (from 123). The object and tint entry exchange EDI and ESI.
// MiniMax A (11-20): no new form, no lower score (77): the first difference is still the index's zero store, which sinks after the outer guard's jle here and stands before it in retail; a form with the store in retail's place (the index declared before the guard, or no explicit guard) measures 97 and swaps the object's ESI/EDI role with the tint-bank pointer, and signed `<` comparisons in the first scan trigger VC's unrolling (636 and 237), so the kept 77 form has retail's registers with the store in the wrong place.
// the index's zero store sinks after the outer guard's jle; in the first scan the count is spilled
// Earlier header 77 (merged in r284): the index's zero store sinks after the outer guard's jle; in the first scan the count is spilled
// Earlier score line 77 (was 97): The 0x54 frame agrees. The ordinal zero sinks past the bank guard. Choice-count and mask homes still differ.
// Earlier score 77 (was 97): The 0x54 frame agrees. The ordinal zero sinks past the bank guard. Choice-count and mask homes still differ.
// Earlier detail: The pre-guard ordinal store, scan count and mask homes, and signed weight-scan exits still differ.
// Earlier score 77 (was 77): the index's zero store sinks after the outer guard's jle; in the first scan the count is spilled
// The index's zero store sinks after the outer guard's jle. In the first scan the count is spilled
// and the eligible mask holds EBP (retail: count EBP, mask and weight in stack slots), the scan leaves on `!=`, the
// blend's fstp precedes the variant byte compare and the not-found exit joins the clamp entry's 1.0 load.
// Earlier 97 (was 123): The object and tint entry exchange EDI and ESI.
// The choice count, mask and blend use other homes. The first scan exits on inequality.
// Earlier observation: The late variant byte restores the output/delta registers and keeps retail length.
// the object takes ESI and the bank/entry web EDI (retail EDI and ESI); in the first scan the count is
// the object takes EDI and the bank/entry web ESI as retail; in the first scan the count is
// the index's zero store sinks after the outer guard's jle; in the first scan the count is spilled
// and the eligible mask holds EBP (retail: count EBP, mask and weight in stack slots), the scan leaves on `!=`, the
// blend's fstp precedes the variant byte compare and the not-found exit joins the clamp entry's 1.0 load.
// Earlier 97 (was 123): The object and tint entry exchange EDI and ESI.
// Earlier 77 (was 97): the object takes EDI and the bank/entry web ESI as retail; in the first scan the count is
// The zero ordinal store falls after the guard in this build and before it in retail.
// Earlier (97, was 123): The object and tint entry exchange EDI and ESI.
// Earlier (77, was 97): the object takes EDI and the bank/entry web ESI as retail; in the first scan the count is
// spilled and the eligible mask holds EBP (retail: count EBP, mask and weight in stack slots), and the scan leaves on `!=`.
// The late variant byte restores the output/delta registers and keeps retail length.
// The blend store, weight-scan exits and fallback unity reload also differ.
// MiniMax S (11-19): the outer tint loop is now a redundant `if (0 < count)` guard around the for (the guide's form, as
// at 0x2e5590); it emits no extra code but moves the object into EDI and the tint web into ESI as retail has them, and
// the index's zero store now sinks after the guard's jle where retail stores it before.
// The object and tint-table saves still trade ESI and EDI. The blend store, weight-scan exits and fallback unity reload also differ.
// MiniMax I (11-17): the variant byte read before the blend value 123 (was 132): the byte is then held in BL across the fmod and the blend's store follows the call, so the compare moves behind it, but the colours pointer now takes EBX where retail uses EBP; the not-found exit still joins the clamp entry's 1.0 load and the first scan still leaves on inequality.
// MiniMax D (11-17): no new form, no lower score (84): the body is retail's 1034 bytes and the first difference is
// still the order of the 1.0f load and the zero xorps before the first clamp; the blend still sits at [esp+0x38]
// where retail keeps it at [esp+0x1c], the first scan still leaves on inequality where retail leaves on a signed
// less-than, and with the inverse computed inside the found block (132) the not-found path joins the clamp entry's
// load instead of carrying its own copy of the one.
// MiniMax S (11-16): no new form, no lower score (84): the first difference is still the order of the 1.0f load and the zero xorps before the first clamp, the blend keeps its frame slot at +0x38 where retail has +0x1c, and the first scan still exits on inequality where retail exits on less-than; the scan as a do-while measures 221, with a short counter 270, the inverse inside the found block 132, and dropping the seed variable 84 (the same code).
// MiniMax E (11-16): no new form, no lower score (84): the three differences are the same ones as before - the clamp entry still materialises the 1.0 before the zero where retail zeroes first and loads the 1.0 after, the blend still sits at [esp+0x38] where retail keeps it at [esp+0x1c] with the choices pointer in the other slot, and the first scan still leaves on inequality where retail leaves on a signed less-than; every spelling of that bound tried in earlier rounds makes the build unroll the scan by four, so the body stands.
// MiniMax E (11-15): no new form, no lower score (84): the blend's stack slot and the choices pointer's slot are still the other way round, and the 1.0 load still comes before the zero xorps where retail loads it after.
// MiniMax S (11-15): no new form, no lower score (84).
// MiniMax S (11-14): no new form, no lower score (84): the permuter's search over this body (60 tries, 610 s) found nothing below the baseline's 84; the first scan still leaves on != where retail leaves on a signed jl, the blend still sits at esp+0x38 where retail keeps it at esp+0x1c with the choices pointer in the other slot, and the clamp entry's constant order and the not-found path's own copy of the one still differ.
// MiniMax A (11-13): no new form, no lower score (84); with the inverse computed inside the found branch (the
// two conversions first, then the inverse) the blend takes retail's slot, the found block matches retail's
// code step by step and the clamp entry orders its zero and its one the way retail does, but the
// not-found path joins the clamp entry's load instead of carrying its own copy of the one (132) and the count
// test and the first scan's exit still differ, so the body stands; merging the second fabs into the fmod call
// scores 86 and leaves the blend in the seed's slot.
// MiniMax B (11-13): no new form, no lower score (84): aligned the two listings instruction by instruction: the first scan
// leaves on a signed less-than bound (retail's jle and jl against this build's je and jne), and writing that scan with
// the less-than bound unrolls it by four here (637); the blend keeps the seed's home (esp+0x38) where retail keeps it at
// esp+0x1c and the choices pointer the other way round; the clamp's entry materialises the one before the zero where
// retail materialises the zero first and lets its not-found path jump into the one-load; the five clamp spellings
// measured before are all worse, so the body stands; the permuter searched 45 forms for five minutes over the
// full image and the reduced image (which scores the same 84) and found nothing below 84.
// MiniMax B (11-12): no new form, no lower score (84); re-read retail's listing: the clamp entry's constant
// order, the blend's stack slot, the first scan's signed bound and the not-found exit's 1.0 reload are still
// the four differences, and the spellings of the scan's bound that keep it rolled (less-than, the bound read
// from the entry, a pointer bound, a do-while and the count compared the other way round) all land in a form
// that does not match, so the body stands.
// MiniMax H (11-11): no new form, no lower score (84); the first scan still leaves on `!=` where retail leaves on a signed `jl`, and every spelling of that bound makes the link-time build unroll the scan by four (637 for a less-than bound, 221 as a do-while), the blend still takes the index's old slot (105) and still sits at [esp+0x38] where retail keeps it at [esp+0x1c], so the body stands.
// MiniMax E (11-10, run 73): no new form, no lower score (84); the blend still takes the seed's stack slot where retail keeps it in the index's old slot and its fstp still lands before the variant byte read instead of after the compare, the choice scan still exits on inequality where retail exits on less-than (a less-than bound unrolls by four here at 637, a short counter at 270), the count test still skips on zero where retail skips on zero or less, and retail's own 1.0 reload at the selection loop's not-found exit is still missing.
// MiniMax D (11-11): no new form, no lower score (84): the first choices loop bounded by less than unrolls and scores 637 and the same loop as a do-while scores 221, the blend sharing the index's own home scores 105 (every slot shifts), so it still leaves on != where retail leaves on a signed jl, and the blend still sits at esp+0x38 where retail keeps it at esp+0x1c. Retail's entry guard for that scan is a test and a jle, so it skips the scan only when the count is negative, while ours skips it on zero and so admits no count of zero; the two guards and the two exits cannot be had from one source form.
// MiniMax G (11-11): no new form, no lower score (84): the first difference is at +0xcc - retail zeroes xmm7 before it reloads 1.0 into xmm5 where we load the constant first, retail keeps the blend at [esp+0x1c] where ours keeps it at [esp+0x38], and the variant byte test and the blend remainder follow in another order; the blend remainder reusing the index slot scores 105 and is reverted, and the permuter was stopped at its time limit with no result.
// MiniMax S (11-10): no new form, no lower score (84): the permuter (60 tries, 610 s) found nothing below the baseline's 84, the first scan still leaves on `!=` where retail leaves on a signed `jl` (a scan bounded by less than is unrolled here at 637, and as a do-while 221), and the blend still sits at esp+0x38 where retail keeps it at esp+0x1c with the choices pointer in the other slot (a local for that pointer scores 208).
// MiniMax D (11-10): no new form, no lower score; both builds run the same instructions, only four
// places differ, and this round's reading of the two listings pins them down. The first is the clamp
// entry: retail zeroes the float register and only then loads the 1.0 constant, ours loads the
// constant and only then zeroes. The second is the second fmod result: retail keeps it in the x87
// stack across the variant byte load and the compare and stores it afterwards, ours stores it at
// once, and retail stores it in the slot the loop counter's float home used while the chosen
// choice's pointer goes to the other slot, so ours reads the blend from the wrong slot. The third is
// the first scan's exit: retail leaves on a signed `jl`, ours leaves on `!=`, and every spelling of
// the signed bound makes the link-time build unroll the scan by four, which is much worse. The fourth
// is the not-found exit: retail reloads the 1.0 constant into the float register there and ours joins
// the clamp entry's load, so the tail is eight bytes short and every later branch target shifts. No
// source form of these four was found this round, so the body stands.
// MiniMax E (11-09): no new form, no lower score; the first scan still leaves on `!=` where
// retail's signed `<` loop unrolls by four, and every counter, bound and accumulator spelling
// keeps the unroll out of our build.
// MiniMax S (11-09): no new form, no lower score (84); the first scan's `!=` exits (retail's signed `<`
// loop unrolls by four), blend's stack slot, the inverse subtraction's home and the 1.0/0.0 load order
// still differ.
// MiniMax E (11-05): no new form, no lower score; the second fabs inlined into the fmod call
// gives 86 and a separate blend seed variable compiles the same as the reused one (84); both reverted.
// MiniMax H (11-04): the permuter was stopped before it reported any variant (no output in 240 s
// under the shared load); no lower score.
// Round 30 (lane D): the permuter found no better form (82 tries, best 84, the baseline). On this body
// the variant byte read before the blend value scores 171 and before the selection seed 190; with
// retail's found path (the inverse inside the found branch) it is 123, and the plain inverse form 132:
// retail's not-found exit reloads 1.0 into xmm5 while ours rejoins the block's load, so the tail sits
// 8 bytes short and every later branch target shifts. Original body kept.
// MiniMax R (11-01): no new form, no lower score; the inverse before the eligible test 208 and the
// constant order at the block entry stay as they are.
// MiniMax G (11-06): the inverse moved into the found branch 132; no lower score.
// MiniMax I (11-08): no new form, no lower score (84): the first scan's signed `<` (which unrolls by
// four), the blend's stack slot, the inverse subtraction in the blend block and the 1.0/0.0 load order
// still differ, and every counter, bound and accumulator spelling keeps the unroll.
// MiniMax A (11-14): no new form, no lower score (84): the first scan still leaves on inequality where retail
// leaves on a signed less-than; a `for` loop with `<` unrolls by four here (637 at 1310 bytes) and a do-while
// with `<` gives retail's `jle` and `jl` but moves every stack home four bytes down (221 at 1006 bytes).
// MiniMax D (11-14): no new form, no lower score (84): the first difference is still the order of the 1.0f load
// and the zero xorps before the first clamp (retail zeroes the register and then loads the constant, and it
// reads the variant byte before the blend's fstp), and the blend still sits at [esp+0x38] where retail keeps it
// at [esp+0x1c] with the choices pointer in the other slot.
// MiniMax R (11-15): no new form, no lower score (84): re-measured at retail's 1034 bytes, the first
// difference is still the order of the 1.0f load and the zeroing before the first clamp, the blend still
// sits at [esp+0x38] where retail keeps it at [esp+0x1c] with the choices pointer in the other slot, and
// the first scan's `!=` exits still differ from retail's signed `<` loop, which unrolls by four in every
// spelling measured so far; the body stands.
// MiniMax C (11-16): 84 (was 84): no new form, no lower score; at retail's 1034 bytes the first
// difference is still the 1.0f load before the zeroing before the first clamp (retail's clamp entry is
// xorps, movss, comiss), the blend still sits at [esp+0x38] where retail keeps it at [esp+0x1c] with the
// choices pointer in the other slot, and the first scan's `!=` exits still differ from retail's signed `<`
// loop; this round a separate blend-seed local, a do-while first scan, computing the inverse inside the
// found block (132, but retail's exact length) and reading the variant byte before the blend's fmod (123,
// but a changed prologue) all score worse, so the body stands.
// Earlier 97 (was 123): The late variant byte restores the output/delta registers and keeps retail length.
// Earlier score 97 (was 123): The late variant byte restores the output/delta registers and keeps retail length.
// Earlier opening state (was 84): frame 0x54 and length agree. The first-bank and input-delta registers are swapped,
// and the variant byte is read before the second remainder. The first scan's `!=` exits
// differ from retail's signed `<` loop (unrolled by 4 in every
// The 0x54 frame and 1034-byte extent agree. The first difference is an outer exit
// displacement. The build holds the early variant byte across the second fmod in BL and swaps the output and
// delta saved registers. Retail compares the byte in CL after the call and then stores the rounded blend.
// Earlier 123 (was 84): the first scan's `!=` exits (retail's signed `<` loop unrolls by 4 in every
// for/while/field/shared-counter form, 637-647; short counter 252; a compile-only probe shows the
// unroll comes from the float weight sum and survives every counter, bound and accumulator
// spelling), blend's stack slot, the inverse subtraction (retail's is in the blend block, with 1.0
// reloaded at the not-found exit) and the 1.0/0.0 load order.
// Round 102 (lane 7): a one-file LTCG harness (this file plus the HSV and 0x3dd10 files) reproduces 84
// exactly. First scan as `for (j = 0; j < choice_count; ++j)` 637 (1310 bytes, unrolled); bound read as
// entry->count 646; through a scan helper writing last/weight/mask through pointers, plain inline 354
// (called, 952 bytes), __forceinline 637, with entry->count 646; one `j` shared by both loops 637, with
// the `!=` scan 84; a walking choice pointer 563 (1245 bytes); the mask set through a SET_FLAG macro 637.
// Real --fast agrees for the `<` scan, shared j, walking pointer, forceinline helper and SET_FLAG forms.
// Round 54: moving the inverse into the found branch gives retail's 1.0/0.0 load order and the
// found-path subtraction (132f), but our not-found exit then rejoins the block's 1.0 load instead
// of reloading it, so the tail sits 8 bytes short of retail's and every later branch target
// shifts; a while-loop second scan adds an entry guard (196f, 1045 bytes). Original restored.
// Round 55: with the inverse inside the found branch (hsv3f a,b,mixed first, then the inverse, then
// the two conversions) our found path is byte-identical to retail's, the blend and choices stack
// homes match, and the 1.0/0.0 materialisation order matches; the one difference left there is
// retail's not-found exit reloading 1.0 into xmm5 (8 bytes) where ours rejoins the block's load, so
// the tail sits 8 bytes short and every later branch target shifts (132f). Putting the inverse
// after the two conversions also moves the subtraction (138f). Original restored.
// Round 53: the inverse at the top of the found block gives retail's blend slot, subtraction and
// 1.0/0.0 order but loses the 1.0 reload at the not-found exit (one instruction short) 132f; with
// the variant byte read before the blend value 123f, read at its uses 134f; with a pre-incremented
// selection test 132f; 30 declaration/clamp orders with it 120-134 (screened); seed names and a
// named choices pointer (screened) don't move the slots.
// Earlier 122 (was 138): second remainder store, signed scan exits and constant reload still differ.
// Blend and cached-choice homes, signed scan exits, inverse placement and constant reload still differ.
// Frame 0x54 agrees; remainder publication/home, signed scan exits and fallback one reload remain.
// Frame 0x54 agrees; blend/choice homes, signed scan exits, inverse placement and fallback one reload remain.
// Distinct seeds 138; last before weight 136; accumulator first 134;
// signed variant ordinal 138; const choices 138; unsigned variant key 138;
// skipped-choice loop 138; threshold first 144. Original full-length scan loops retained.
// Native blend remainder/immediate and delayed rounding 148/161; paired native remainders 196;
// variant read at uses 136; shared color interpolation alone/with remainder 376/374;
// initialized HSV outputs 158; separate RGB output 156. Second remainder publication, signed scan
// exits and one reload still differ; original arithmetic and scans restored.
// Selected choice result 134; selected ordinal 155; late variant key 229;
// variant between seed and remainder 122; late scan outputs 256;
// HSV output role names 134; immutable blend 134; immutable selection 134.
// native position pointer 137; position value snapshot 295; blend declaration first 134; variant
// declaration first 134; unsigned mask bits 134; unsigned last ordinal 134; component clamp loop
// 136; non-hue component blend loop 134.
// Variant between native remainder and rounding 213; variant assigned at metadata test 134;
// immutable early variant 122; output element reference 131; metadata entry reference 122;
// twice-used choice key 131; late mixed HSV workspace 122; selected choice reference 122.
// Earlier 122 (was 122): remainder publication, saved color roles and signed scan exits still differ.
// lower HSV reused 199f; upper HSV reused 203f.
// weighted selection reused 122f; cumulative weight reused 122f.
// shared scan ordinal 122f; short variant ordinal 263f.
// selection plus lower HSV 199f; weight plus upper HSV 203f.
// output/delta roles, second remainder publication and fallback one reload remain;
// input/output/both conversion inline declarations 122/122/122; actual lower/upper/both HSV
// returns 200/199/243; both returns plus input/both declarations 243/243.
// Saved output/count roles and second-remainder
// publication still differ. Early variant field binding 149; live count 122;
// both 149; early presence predicate 131; default/supplied/final color
// memcpy 213/213/213; base-color name 122; model-first guard 150; shared
// model handle with that guard 232. Original copies and observations retained.
// Earlier 122 (was 122): variant/remainder publication, saved roles and scan exits differ.
// Live choice count 122; choice-count field reference 213; signed weight bound 630;
// unequal-end for 122; inverse blend at uses 168; eligibility equality first 122;
// eligibility predicate value 213; actual object variant field 122. Opening body retained.
// Earlier 122 (was 122): output/delta roles, variant timing, scan exits and one reload differ.
// Full-width variant 273; eligibility first 122; early choice weight 131; complete clamp value
// with output reload/direct dual publication 341/341; separate wrapped hue 218; unsigned choice
// token declaration and receiver 122; weighted upper-channel scalar values 134.
// Earlier 84 (was 122): variant byte read after the blend value (registers now retail's), last
// before weight, inverse computed at the top of the blend branch. Still differs: the first scan's
// `!=` exits (retail's signed `<` loop unrolls by 4 in every for/while/field/shared-counter form,
// 637-647; short counter 252), blend's stack slot, the inverse subtraction (retail's is in the
// blend block, with 1.0 reloaded at the not-found exit) and the 1.0/0.0 load order.
// Second scan as for 196; found flag in its condition 403/406; renames 84; named choices pointer
// 208/238; select-then-blend 132, with the inverse after the loop 188.
// Round 186: One seed scope, output orders selection/blend and blend/selection, 84/84; both with blend-entry inverse 132/132; separate seed scopes in both orders 84/84, with inner inverse 132/132.
// Upper-first conditional/branches 201/244; lower-upper/upper-lower clamp stages 273/252; per-channel values lower/upper-first 240/243; all channel values lower/upper-first 245/254.
// Direct rounded seeds 84; shared double hue delta 156; both 156; symmetric six HSV products 84;
// products/direct seeds 84; products/delta 156; all three 156; positive hue arm on all three 156.
// HSV lower/upper/mixed buffers lifted individually, each pair and all to choice-count scope: seven
// forms 84; all three at function entry 84.
// Round 183: direct field observations in weight/selection/both scans 150/161/161; live count
// alone/with weight/selection/both 84/150/161/161; all with the signed first bound 619
// (unrolled). Original rolled scans, receiver graph and arithmetic restored.
// Round 212: carried eligibility bit with original/cursor first scan and independent second bit
// 224/225/228/229; signed bounds on those graphs 599/544/639/529 (fast control 84). Signed scans still
// unroll; original mask arithmetic and scans restored.
// Round 43: nested variant/model guards, staged outer inverse, early variant declaration and const
// variant 84 each; staged/const inverse in the found block 132 each; model-index snapshot 174;
// const choice count with the signed first bound 637 (unrolled). Original body restored.
// Lane 17 r1: first scan as a do-while (the choice count is already known positive) 221f; inverse in
// the found block (round 55's form) 132f, with the variant byte read before the blend value 123f, with
// the inverse declared at function entry 132f; outer count written at each use 294f (reloaded each
// pass). A volatile-weight diagnostic (reverted) stops the signed first scan unrolling (retail's
// jle/jl exits) but swaps the weight/mask homes, 102f. With the inverse in the found block only the
// not-found exit's own 1.0 reload (retail: movss xmm5 before the count reload) is missing.
// Round 439: complete supplied/default/converted color result, loop/entry lifetimes and
// field/indexed clamps with field/result dual publication: 261/261/291/286, repeated at entry.
// Frame grows to 0x60; original object-output pipeline, scans and all float trees restored.
// Lane 14 r20: the first scan as `for (j = 0; j < choice_count; j++)` with `continue` for an
// ineligible choice 637 (the compile-only build keeps it rolled with retail's jle/jl exits; the LTCG
// build unrolls it by 4, 1310 bytes), with the inverse in the found block 638; the selection loop
// breaking out and the blend after it under `j < choice_count`, the inverse between them, 176 (the
// test isn't threaded); the same under a found flag 280; the inverse in the found block (132, again)
// with the first scan's mask, weight and last stores reversed 134. A compile-only screen shows the
// unroll needs the float add: an integer sum or no sum leaves the `<` loop rolled. Calling the
// existing 0x131c20 (HSV flag) in the found block is not inlined, 374 (again).
// Round 128: inverse at the top of the found block again 132 (round 55's form: left are the not-found
// exit's own 1.0 reload, the variant byte read after the blend's fstp, and the first scan's exits);
// on it the first scan as a signed for loop 638 (unrolled, 1309 bytes), the same with the weight, the
// mask or the last index address-taken 638 each (taking an accumulator's address does not stop the
// unroll); the mixing moved after the second loop on a found choice pointer 145. The weight, mask and
// last index are memory-resident in both builds, so that is not what stops retail's unroll.
// Round 147 (m3 lane 4, fast = plain here): with the inverse at the top of the found block (round 55's form, 132)
// an aligned diff leaves only four instructions: blend's fstp after the variant byte compare, the first scan's
// two exits, and retail's `movss xmm5, 1.0` at the selection loop's not-found exit (retail rematerializes 1.0 on
// both exits; ours sends that exit through the clamp entry's load), plus 8 bytes of padding; blend also takes
// retail's slot there. A compile-only build without /GL packs blend into index's slot as retail and keeps the
// signed scan rolled, so the slot and the unroll are LTCG-build effects. Tried: seed renamed hash 84 (same
// code); the first scan as a signed for with the weight added first 616 (1294 bytes, unrolled); the inverse at
// the top of the selection loop body 84 (hoisted back); on the found-block form the target before the running
// sum 136 and the eligibility test as nested ifs 132. Diagnostic (reverted): size preference around the
// function 443 (879 bytes).
// Round 42 (m3 lane 13, fast): the inverse at the top of the found block 132 (again: retail's not-found exit
// reloads 1.0 into xmm5 before reloading the count into EAX, so it does not cross-jump into the mask path's
// `movss xmm5`; ours reloads the count first and merges, 8 bytes short); on it blend/variant declared first and
// assigned after 132 (same code), the variant byte read before the blend value 123 (held in BL across fmod); the
// found choice kept in a pointer with the inverse between the loop and an `if (selected)` blend 196 (frame 0x58);
// the inverse computed right after the blend seed 198 (frame 0x58).
// MiniMax R (11-10): no new form, no lower score; the choice scan still exits on inequality where retail exits on less-than (a scan bounded by less than is unrolled here at 637, and as a do-while 221), the choice count test still skips on zero where retail skips on zero or less, and the blend still sits at esp+0x38 where retail keeps it at esp+0x1c with the choices pointer in the other slot.
// MiniMax A (11-11): no new form, no lower score (84); the first difference is still the order of the 1.0f load and the zero xorps before the first clamp; the three clamps written as if/else, in the reversed test order or through locals measure 267, 201 and 245.
// MiniMax D (11-15): no new form, no lower score (84): the first difference is still the order of the 1.0f load and the zero xorps before the first clamp, the blend keeps its frame slot at +0x38 where retail has +0x1c, and the first scan still exits on inequality where retail exits on less-than; a 43-try permuter run (309 s) found nothing below 84.
// b10A (11-17) walk against retail, fast build on r841: every field offset (+0x64/0x68/0x6c, +0x124/0x126, +0xb1, def
// +0x38/+0xb0, model +0x54 stride 0x38), the float constants (0x45df34-0x45df44, 0x45dbbc 0.5, 0x45dbc0 1.0, fmod's
// double 1.0 at 0x45dcc0), the seed's sum order, every jcc sense (NaN included: 0xbe467 `jbe`, 0xbe49f `jae`, 0xbe509
// `jbe`, 0xbe532 `jbe`), the argument registers of 0x1318d0/0x131a00 and 0x3dd10(object, 1) agree; no behaviour
// difference. Retail computes `1.0 - blend` only on the found path (0xbe4ba-0xbe4d2: xmm5 = 1.0, xmm6 = blend, xmm4 =
// xmm5 - xmm6, after the break test and before the two 0x1318d0 calls), so `inverse` now sits there: retail's
// evaluation order, 132 (was 84 with the subtraction before the loop; that placement only schedules better). Left on
// it: retail's own `movss xmm5, 1.0` at the not-found exit 0xbe4a9 (ours joins the clamp entry's load, 8 bytes short),
// blend's fstp after the variant byte compare (0xbe3d0), its slot [esp+0x1c] (ours 0x38), and the first scan's `jle
// 0xbe408`/`jl 0xbe44d` exits. New: a redundant `if (choice_count > 0)` around the do-while folds (221, 1006 bytes); a
// volatile weight with a `<` scan gives retail's jle/jl exits and 1034 bytes but swaps the weight/mask homes
// ([esp+0x70]/[esp+0x10]) 102, with the inverse in the found block 145; declaration orders of the four scan locals
// 102-104/146-147; volatile last 131, eligible 248, j 259, last+eligible 283. Best match-only recipe: 102, not kept.
// Typed tint and model access 132; explicit variant outcomes/named predicate/direct conditional 227/227/227;
// separate invalid cases/nested conditional 231/231; separate blend seed with conditional/explicit outcomes 227/227.
// The typed fields retain the same code. The other forms move the saved registers and lengthen the body.
// Output/delta registers and the remainder store relative to the byte comparison still differ.
// Post-blend typed model variants 132, with the native object byte 132. Single checked model handle
// 146, with the object byte 146. Native variant block 132, with checked handle 146.
// Typed lookup under nested guards 132. Checked lookup with entry reference 147.
// NOT kept: the rounded result still stores before the byte comparison on the later read forms.
// Late variant with single-precision blend remainder 144, both remainders 129, blend absolute
// value and remainder 129, both absolute values and remainders 129. Object position members
// with original variant read 123, late read 132, late read and single-precision blend remainder 144,
// late read and both single-precision absolute values and remainders 129. Both remainder overloads
// recover the CL compare before the blend spill at +0x1c. NOT kept: overall score rises and the
// signed scan exits and fallback unity reload still differ.
// Signed weight pointer end with carried/derived ordinal 558/643; unequal end 283/313;
// signed selection pointer end 297/301 and both signed ranges 574/652.
// Signed weight ranges unroll. The unequal bound grows the frame and leaves the two output registers exchanged.
// Round 415. last eligible choice address 219, last address with signed bound 635, last address and weight cursor
// 229, last address with signed bound and weight cursor 576, last address and selection cursor 216, last address
// with both cursors 229, last address and const indexed choices 219, last address with early ineligible skip 219.
// Output/delta saved registers and blend publication still differ, followed by the signed scan and fallback one load.
// Separate indexed second bank, advancing second bank, fixed-distance first bank cursor, and paired bank cursors:
// 296/167/290/293. Variant byte after the blend remainder on the same four bank forms: 300/162/293/296.
// NOT kept: none recovers both retail output registers and the spill order.
// Round 628: current color reference/pointer and reference before entry 132 each; tint-first mask arm
// 249, tint entry on fallback only 144, first-bank cursor with indexed second bank 283,
// rounded blend at selection 198, const tint/choice references 123, shift supplied mask 179.
// Output/delta saved registers, blend publication and the signed weight-scan exits still differ.
// round blend after model lookup 159; round blend when choice is found 191; round blend at selection entry
// 182; variant presence before blend rounding 214; model variant presence before blend rounding 222; round
// blend after nested model lookup 159; conditional variant key before blend rounding 123; separate blend
// seed and round after model lookup 160. NOT kept: rounding after lookup recovers the output/delta saved
// registers and CL comparison, but puts the blend store after the entire lookup and shortens to 1019 bytes.
// The conditional key ties 123 at 1035 bytes and puts the blend in a different slot. The signed scan exits
// remain different. The opening executable source is restored.
// The object and tint-table saves still trade ESI and EDI, and the blend spill precedes the byte test.
// Tried unsigned variant byte before the remainder and signed index at lookup 147, unsigned variant byte
// after the remainder and signed index at lookup 97, shared vertical position input for both seeds 254,
// shared vertical seed input with the variant read after the remainder 230, shared vertical seed input
// with the unsigned late variant byte 230, ordinal conversion at each seed expression 123, ordinal
// conversion in both seeds with the late variant read 134, ordinal conversion in both seeds with the
// unsigned late variant byte 97.
// Kept the late unsigned byte with signed decoding at the index calculation. The byte compare gates the
// lookup and the index sign-extends at use as in retail. The 0x54 frame and 1034-byte length agree.
// Last ordinal, eligibility mask or weight initialized at scan entry 199/191/286;
// mask and weight 275; all three 312; all three with signed bound 562.
// Last ordinal gates selection 232; one choice-name read 106.
// NOT kept: regressions in the scan and saved-register layout.
// M3 lane 13 r113 (fast with 0x1318d0, 0x131a00, 0x3dd10, control 97): the first scan as a do-while like the second 230
// (995 bytes); a redundant `if (0 < count)` around the outer loop 77 (not kept, steering: the object then takes EDI and
// the bank/entry web ESI as retail, but the index's zero store sinks after the guard's jle, where retail stores it
// before; left are that store and the first scan's count/mask/weight homes); the index declared and zeroed ahead of
// that guard 97 (the guard's effect goes with it); the outer loop as a while 97; the count computed ahead of the
// output pointer 291 (1017 bytes).
// The 0x54 frame and 1034-byte extent agree. The object and tint entry exchange EDI and ESI.
// Lexical ranks for the object and tint entry before definition or after output score 97 each,
// alone and in all four pairs. Neither saved register moved. The blend spill and signed scan still differ.
// choice count named for the number of available choices 97; choice count named for the number of
// tint permutations 97; eligible flags named for the choice mask 97; count after the mask in lexical
// order 97; shared read-only choice array at the positive-count entry 262; shared read-only choice
// array at the first scan 117; shared array with an advancing weight-scan cursor and indexed
// selection 196; shared array with indexed weights and an advancing selection cursor 116.
// shared object datum lookup 97; shared object header lookup 97; shared slot object lookup 97; shared object
// datum with native tag index 106; native tint block size and displacement 97; native tint block and position
// fields 97; native tint block, position and variant fields 97; complete native tint object with shared datum
// lookup 97. All eight previews retain 1034 bytes and the same initial register mismatch. The native tag-
// index form adds spill differences. Opening executable source restored.
// M3 lane 8 r191 (fast with 0x1318d0/0x131a00/0x3dd10, control 97; the guard diagnostic 77 re-measured): the first scan
// bounded by `entry->count` read in its condition 647 (1316 B, unrolled), with the guard 645; `inline` on both HSV
// converter definitions 224 (they inline and three matched callers are lost), on the rgb-to-hsv one 237, on the
// hsv-to-rgb one 143. Compile-only (object in EDI as retail there, not exact): the signed scan still unrolls with the
// weight as an aggregate member, through an inline scan helper writing weight, mask and last through pointers or
// returning the weight, as a while, with a dword index, with `continue`, or with the choice pointer stepped. Body unchanged.
// M3 lane 9 r144 (fast with 0x1318d0/0x131a00/0x3dd10, control 97): a one-object screen (this file with the two
// callee files, frame 0x54 as retail) unrolls the signed first scan by four with or without /GL, so the unroll is
// not an LTCG effect; it stays rolled only with a volatile weight, last, eligible or variant (each of the four is
// memory-resident in retail's loop); taking their addresses (passed to an opaque call), a named eligibility bool,
// a named bit, nested or ternary eligibility tests, indexed field reads, byte-offset element access, the weight
// added first, `weight = weight + w` and a weight test all still unroll. Real trials: the scan locals declared
// ahead of the count test 315 (999 bytes); the first scan as a `for` with `!=` 97 (same code); a signed eligible
// mask 97 (same code); weight/eligible/last/variant declaration order 97; target before the running sum 100.
// Round 127 (m3 lane 10, fast with 0x131c20's file listed): the found block as a call to the existing 0x131c20
// (result, lower, upper, 1, blend) stays a call, 398 (838 bytes); with `inline` on 0x131c20's definition still a call
// 404 (0xd1a30 149, 0x131d60 52, 0xc0c80 319 lost); with a forceinline declaration of it in this file still a call 398;
// the choices read through an inline element accessor with a signed `<` first scan 638 (unrolled, 1309 bytes); the
// three clamps through a by-reference pin helper 197.
// indexed current bank 311, second-bank cursor 98, cursor with bank-end bound 191, ordered cursor bound 191,
// indexed bank-end bound 271, ordinal from cursor 278, unsigned bank ordinal 282, inclusive last choice 667.
// NOT kept: The second-bank cursor keeps 1034 bytes at 98. Other bank forms raise the score or move the frame.
// The inclusive last-choice bound unrolls the signed weight scan.
// M3 lane 15 r113 (fast with 0x3dd10/0x1318d0/0x131a00/0x14bfc0/0x2c0b70, all MATCH; control 77): the choice count read
// from the entry field at its test and both scan bounds 77 (same code); the three channel clamps through a forceinline
// by-reference pin helper with early returns (the form a matched neighbour uses) 189 (the running sum becomes a memory
// add); the scan count bound by `long const &` for the first scan's bound 86, the mask updated through a `dword &`
// 86, the weight summed through a `real &` 86 (all three the same code: only the found block's hue registers swap).
// Indexed tint entry after the white store 238; advancing tint entry 307;
// independent live choice-count and array lookup 180; guarded choice count 97;
// supplied-color test before the tint entry 106; signed scan with conditional mask accumulation 873;
// separate selection admission and threshold 219; positive weight predicate first 108.
// NOT kept: The saved object/tint roles, signed scan and blend publication still differ.
// Unequal bank bound with and without the guard 77/97. Named stored-color count or byte
// extent 86/86. Tag lookup after extent or after address 260/87. One supplied/default
// color assignment by value or by source address 298/298. The choice count and mask
// homes and the signed weight-scan exits still differ.
// One choice-name read with nonmatching skip, nested mismatch, selected-name first or
// complementary admission 86/86/189/86 at the unequal bound. All four signed forms 645.
// NOT kept: the signed bound unrolls the scan, and the other forms regress at retail length.
// The ordinal zero, choice count and mask homes still differ.
// M3 lane 3 r280 (fast with 0x1318d0/0x131a00/0x3dd10, all MATCH each time; control 77; [n] = aligned differing rows of
// 291, control [33]). Diagnostic lead (a volatile local, reverted at once, never kept): `long volatile variant` with the
// first scan as a plain `for (j = 0; j < choice_count; ++j)` 139 [5] at 1034 B: the scan stays rolled with retail's
// jle/jl exits, the count in EBP, the mask at [esp+0x70] through ESI and the weight at [esp+0x10]. On that base: without
// the outer `if (0 < count)` guard 131 [3] (the index's zero store is ahead of the jle as retail, registers unchanged);
// the blend through the float overload of fmod, `fmod(seed, 1.0f)`, 136 [3] (the blend's fstp follows the variant byte
// compare as retail); both 128 [1], both remainders through that overload 128 [1]: the one row left is retail's own
// `movss xmm5, 1.0` at the selection loop's not-found exit. Volatile last instead 141 [7], weight 82 [36], mask 265
// (1006 B); a volatile blend, selection or index outside the scan does not stop the unroll (649/638/683). So every
// difference but that one row follows from the first scan staying rolled, and no plain form keeps it rolled.
// Signed `for` first scan without a volatile (all unrolled by four, 1309-1384 B): alone 636, without the outer guard
// 638; the variant lookup in an inline helper writing through `long *` (forceinline or inline) 638/638, through `long &`
// 638, returning the name 649; the variant, last, weight, mask or count read through a reference bound to it 647 each;
// the mask, variant or last as a one-element array 647 each; the four scan values in one local aggregate 647; one more
// float compare, byte store or byte load inside the scan 737/638/712; with an opaque call after 0x131a00 641; without
// the two fmod calls 522; without the three converter calls 669; the weight zeroed just ahead of the scan 587. The scan
// as a do-while, with or without its own `if (choice_count > 0)`, 218 (1002 B: no pretest, the count spilled and the
// mask in EBP as with `!=`); as a `while` 647.
// For the not-found exit's 1.0 on the diagnostic base (control [1]): the inverse ahead of the selection loop 92 [18]; the
// blend after the loop with the inverse between them, on a selected-choice pointer 155 [33] (1049 B), on a found flag
// 171 [43] (1056 B), on `j < choice_count` 127 [65] (1039 B; none of the tests is threaded); the loop ended by a found
// flag in its condition as do-while 994/997 (1606 B) or `for` 358 (1105 B); the loop as a `for` with `break` 148 [29]
// (1034 B: same instructions, every 4-byte home one slot up, no pretest emitted, the 1.0 row still missing); that `for`
// with the blend after it on the index, a pointer or a flag 1272/931/955 (the selection loop unrolls).
// On main's body: the blend through the float overload of fmod 74 [31] (three points, not kept), both remainders 74
// (same code); those without the outer guard 95 [61].
// Later in r280: a compile-only sweep (30 spellings, then confirmed with the fast check) found the one signed spelling
// that stays rolled without a volatile: the bound written count first, `for (long j = 0; choice_count > j; ++j)` (the
// loop is then not counted and not unrolled; short, word, byte and char counters and `!=` are the other rolled forms,
// every `j < bound` spelling, dword counters, `j <= choice_count - 1`, a `while`, a stepped pointer, `continue`, `1u <<
// j`, `weight = weight + w` and narrower mask/last/variant types all unroll). With the enclosing test left as
// `choice_count > 0` its pretest folds away, 227 (1002 B), without the outer guard 220; with the enclosing test spelled
// `0 < choice_count` the pretest stays and the body is 1034 B with retail's jle/jl exits: 189 [34], with the float
// remainder 74 [30] (three points, not kept), without the outer guard as well 95 [60] (the object and tint entry swap
// ESI/EDI again), enclosing test `choice_count >= 1` 100, `entry->count > 0` 218 (1002 B). What is left on that form
// is only the count/mask fight for EBP: with `long volatile last` as a diagnostic 139 [5], with the float remainder
// 136 [3], without the outer guard as well 128 [1] (the 1.0 row); with a volatile variant instead 154 [8] (1019 B). So
// retail's scan is that uncounted signed loop, and `last` (or the variant) is out of the register fight in retail.
// For that fight, on the rolled form without the guard (95): the 24 declaration orders of last/mask/variant/weight 95
// each; no model local 95; no choice local in the scan 95; no index local 95; `last == j` 96; no target local 197
// (1037 B); last declared just ahead of the scan 202 (top registers as retail, output and delta swap EBX/EBP); the
// mask declared there 190 (1020 B); the variant declared at the byte read 125. For the 1.0 row on the diagnostic base
// ([1]): the selection loop as `for (j = 0; choice_count > j; ++j)` with `break` 150 [31] (`cmp ebp, ecx; jg`, homes one
// slot up), as do-while `while (choice_count > j)` 130 [3], as `for` with `<` 148 [29]; without the `break`, or ended by
// `j = choice_count` or `j = choice_count - 1`, 969-979 (1581 B).
// Pre-guard bank ordinal with signed/unequal do-while latch 97/97, guarded ordinal with signed
// do-while 77. Advance in latch 97, inclusive last bank ordinal 253, pretested unequal bank 97,
// ordinal-based guard 97, nonzero bank guard 97. NOT kept: the pre-guard forms lose the native
// object/tint register roles, and the inclusive bound shortens the body.
// Weight, selection or both read-only choice references 77/77/77. Shared choice array and both
// references, at scan entry or positive-count admission 89/272. Immutable scan-entry array 89.
// Signed weight bound with reference or shared array 636/582. NOT kept: the signed forms unroll.
// The reference and array forms leave the ordinal, weight and mask homes unresolved.
// M3 lane 4 r241 (H2M_FAST; control 77, aligned 28): the signed `j < choice_count` scan 636 (1309 B, unrolled); on lane 3's
// rolled `choice_count > j` form (189 with `0 < choice_count` as the enclosing test, 227 with the test left as is, 197 without
// the outer guard, 189 with the selection loop's bound written count first too): the choice's variant field named once and
// read at both eligibility tests 86 (the selection loop's add is retail's again, but the found block's two hue loads swap
// XMM0/XMM1), with no outer guard 106, with three last/mask/variant/weight declaration orders 86 each, with the blend through
// `fmod(seed, 1.0f)` 83 (both remainders 83, without the guard 104); the negated-and eligibility test 189, eligibility as a
// `continue` 189, the weight added before `last = j` 189. The rolled form with the float blend remainder 74 (aligned 26);
// on it `last` as a one-element array 74 (scalarised: same code). Diagnostic (reverted at once): `long volatile last` 136,
// aligned 9: only the guard's zero store and the not-found exit's 1.0 reload left. The positional score falls only when
// the count/mask fight, the zero store and the 1.0 row are all fixed together.
// Conditional last-ordinal, total-weight and mask updates on the rolled unequal scan,
// alone or grouped, and all three in the indexed loop 189 each. NOT kept: the
// pre-guard zero store and the count/mask homes stay different at retail length.
// Fixed second bank with guarded/pre-guard ordinal 311/311, with last-bounded selection 289.
// Pre-guard ordinal and stepped second bank 113. Last-bounded indexed or cursor selection
// 243/243, with pre-guard ordinal 251/251. NOT kept: the bank forms change the saved
// object/output allocation, and the last-bounded forms retain the different scan homes.
// M3 lane 3 r284 (h2m fast screen with 0x1318d0, 0x131a00, 0x3dd10, all MATCH; "rolled" = r280's `if (0 < choice_count)`
// around `for (long j = 0; choice_count > j; ++j)` with both remainders through `fmod(seed, 1.0f)`, 186 [33] at 1034 B:
// the count spilled to +0x10, the mask in EBP, the weight in the dead colours home): the selection loop bounded by
// `entry->count` rolled 186, main 77 (same code each); the selection loop testing its own copy of the mask rolled 186
// [42], main 86 [37]; the variant written through an inline helper's `long *` rolled 186 (same code); `last = j;
// eligible |= 1 << j;` through an inline helper taking both addresses rolled 186 (same code). Inline pointer helpers do
// not take `last` or the variant out of the count/mask register fight.
// Initialize the bank ordinal before object lookup, tag lookup, output resolution or bank count, 245 each.
// All four with a separate stored-color quotient also score 245. The pre-guard store uses the retail
// stack home relative to entry, and the earlier saved-register setup changes.
// Signed weight scan with live count 189, shared choice array 189, or both 189. Unsigned choice bits
// 189, ordinal-based bank guard 197, shared choice array and pre-guard bank ordinal 197, choice read
// at each use 77, shifted eligibility test at selection 194. The ordinal store and count and mask
// homes remain unresolved.
// Shared supplied/eligibility mask 77, with pre-guard bank ordinal 97, signed weight scan
// 189, explicit nonzero and positive tests 77, or advancing weight cursor 162. Selection bounded
// by last eligible or one past last 243/246. Renamed count and mask 77. Shared eligibility bit
// operations with unequal or signed weight bound 298/298. NOT kept: the helpers expand the
// frame and scan. The pre-guard ordinal and scan count and mask homes still differ.
// M3 lane 12 r166 (fast screen; control 77; the two stacked score lines merged into the first): the first scan as
// `for (j = 0; j < choice_count; ++j)` with the ineligible choice skipped by `continue` 636 (1309 B, unrolled again); the
// mask, weight and last updates in reverse order 77; the mask typed long 77; without the outer guard and the output
// pointer formed ahead of the definition 102; the first scan as a `for` on `!=` with j scoped to it 77 (same code).
// Body unchanged.
// M3 lane 15 r123 (h2m fast screen with 0x1318d0, 0x131a00, 0x3dd10, all MATCH each time; control 77, [34] aligned rows; "plain" =
// the first scan as `for (long j = 0; j < choice_count; ++j)` with both remainders through `fmod(seed, 1.0f)` and no outer
// guard, 638 at 1309 B (unrolled); "rolled" = r280's `0 < choice_count` test around `for (long j = 0; choice_count > j; ++j)`
// with the same remainders and no outer guard, 195 at 1034 B). Re-measured: rolled with the guard 186, the volatile variant
// diagnostic on plain 128 (one row left, the not-found exit's 1.0) and on rolled 143 (1019 B), volatile last on plain 130.
// Diagnostics on rolled (volatile probes, reverted at once): a volatile local touched only at the top of the arm 233, read in
// the scan body 304 (1081 B), written in the scan's found block 240 (1037 B), read in the selection loop 321 (1085 B), a
// volatile weight 99 (1035 B): none gives the count EBP, so it is the variant (or last) leaving the register candidates that
// does, not a volatile access in the loop. Source forms on plain: the variant as an anonymous union-typed local, as a one-member
// aggregate local (assigned or brace-initialised), filled by a 4-byte memcpy from the model's record, `last` as a union-typed or
// one-member aggregate: 638 each (same code as plain: the aggregates are scalar again before the loop is unrolled). The
// choice count read from the entry at every use with no local 638 (636 with the outer guard: one load, still unrolled), in
// the scan and the selection loop 638, in the scan only or the test and the scan 647 (1316 B). The variant lookup in a
// file-local helper: writing through `long *`, plain or `inline`, 638 on plain and 195 on rolled (same code each);
// returning the name 649 (1320 B) on plain and 125 on rolled; called where the scan compares it (no variant local) 1011
// (1612 B) on plain and 247 on rolled. The count is spilled and the mask keeps EBP in every rolled form. Body unchanged.
// Early ordinal with second-bank base 311, current color lvalue 106, both 311,
// or supplied color bound 303. Second-bank base with signed weight scan 302
// or one model-handle read 303. The same two forms with the opening guard 302/303.
// NOT kept: the early ordinal and current color retain 1034 bytes but score 106.
// The second-bank base forms shorten the body, and the mask and count homes still differ.
// Remaining-choice bounds for weight/selection/both 231/170/249. Completed-choice bounds
// 61/181/161. Direct counts in both 249/161. Visit ordinal before the test, advance at lookup,
// inclusive end with early visit, or signed cursor offset 224/293/219/250. NOT kept: the
// 61 form recovers the count and mask homes at retail length, but compares unsigned and
// adds an ordinal adjustment. The four signed follow-ups grow the frame to 0x58.
// M3 lane 5 r226 (h2m fast screen with 0x1318d0, 0x131a00, 0x3dd10, all MATCH each time; control 77). Reading: retail's
// scan is a signed `jl` loop that is not unrolled, with the count in EBP and `last`, the mask, the variant and the weight
// all loaded, updated and stored in memory each pass (the mask in the colours argument's home, the weight at +0x10);
// the natural form (both scans as `for (j < choice_count)`, no outer guard, 638 at 1309 B) already has the mask in
// that argument home, the weight at +0x10 and the variant in memory, but unrolls the scan four wide, which gives
// `last` EDI, the object EBP and the count EBX. The four accumulators declared in eight other orders 77 each (same
// code). The whole arm in a file-local helper: plain `inline` 918 (405 B, left out of line), forceinline 77 (same
// code), with the natural scan 636, with no outer guard 97, both 638. On the natural form, the scan writing through
// pointer locals to `last`, the mask and the weight (all three, each alone, the first two) 638/647/647/638/638 (the
// pointers fold, still unrolled); the scan in a helper with those three as out-parameters, `inline` 311 (950 B, a
// call), forceinline 638 (same code); one index shared by the scan and the selection loop 638, with the outer guard
// 636; a `short` scan index 262 (1049 B: the only form here that keeps the natural scan rolled, but every use
// sign-extends). What keeps retail's scan rolled is still not found.
// M3 lane 12 r170 (h2m fast with 0x1318d0/0x131a00/0x3dd10, all matched; control 77): the selection loop as a `for` on
// `j < choice_count` 199 (1045 B); one `j` for both loops 77 (same code); `target` declared ahead of `cumulative` 80; the
// choice guard as `0 < choice_count` 227 (1002 B); the scan bound as `choice_count != j` 227 (1002 B). Reading: retail
// keeps last, weight and the mask in memory through the scan (the mask loaded into EAX for the selection loop only) and
// gives the count EBP; ours gives the mask EBP and spills the count. Body unchanged.
// M3 lane 2 r133 (h2m fast with 0x1318d0/0x131a00/0x3dd10, all MATCH; control 77; N = the outer `if (0 < count)` guard
// removed, re-measured 97: retail's pre-guard index store and prologue, but the object takes ESI and the tint-entry
// pointer EDI, retail the reverse). On N: the position read through one `real const *` (x, y, z as [0], [1], [2])
// 108; the entry pointer formed inside the else arm 238 (object then EDI as retail, but the colours difference and
// the shift constant move: retail forms the entry at the loop top); the entry as `address + i` 97 (same code); the
// tint-set address in a function-scope local indexed per pass 311 (1001 B). Compile-only screens (the harness
// reproduces the unrolling): every natural `for`/`while` scan with `<` unrolls (pointer IV, `continue`, indexed
// fields, `entry->count` bound, reordered updates, a dword index); an escaped variant address (diagnostic call
// taking `&variant`) still unrolls; only a volatile variant read (diagnostic, not kept) or a short index keeps it rolled.
// Typed object view at the tag, position, bank fields or variant read 77 each.
// Typed position and variant, tag and bank, tag/bank/position, or all reads 77 each.
// NOT kept: the object view leaves the pre-guard ordinal and the scan homes unchanged.
// Round 463. Positive choice count via conditional, condition declaration, explicit negative floor, or
// empty-default assignment 237/237/267/227. With signed first-scan bounds 652/652/672/645. NOT kept:
// signed first scans unroll and the bank ordinal and choice-count homes remain different.
// Last ordinal name 77, total and variant names 77, all three 77, separate scan ordinal names 77,
// both sets 77. Signed completed-choice bound 61, count first 170, scoped remainder and variant
// scratch 77. NOT kept: the 61 form restores the count and mask homes, but compares a second ordinal
// one ahead of the scan index, and the ordinal store still follows the bank guard.
// @retail 0xbe240
void __stdcall function_be240(long object_index, dword color_mask, color3f const *colors)
{

    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    s_object_definition const *definition = (s_object_definition const *)g_4e3b44[*(long *)object & 0xffff].bytes;
    color3f *output = (color3f *)(object + *(short *)(object + 0x126));
    long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
    if (0 < count)
    {
        for (long i = 0; i < count; ++i)
        {
            s_colour_choices_ab const *entry = &((s_colour_choices_ab const *)definition->tint_sets.address)[i];
            if (color_mask & (1 << i))
                output[i] = colors[i];
            else
            {
                output[i] = *(color3f *)g_468710;
                long choice_count = entry->count;
                if (choice_count > 0)
                {
                    long last = NONE;
                    dword eligible = 0;
                    long variant = 0;
                    real weight = 0.0f;
                    real index = (real)i;
                    real seed = (real)fabs((*(real *)(object + 0x6c) * 744.1241455078125f + *(real *)(object + 0x64) * 315.89312744140625f + index * 431.1289367675781f) + *(real *)(object + 0x68) * 587.1294555664062f);
                    real selection = (real)fmod((double)seed, 1.0);
                    seed = (real)fabs(index * 0.7121099829673767f + *(real *)(object + 0x68));
                    real blend = (real)fmod((double)seed, 1.0);
                    byte variant_index = *(byte *)(object + 0xb1);

                    if (variant_index != 0xff && definition->model_tag_index != NONE)
                    {
                        byte *model = g_4e3b44[definition->model_tag_index & 0xffff].bytes;
                        variant = *(long *)(*(byte **)(model + 0x54) + (signed char)variant_index * 0x38);
                    }

                    long j = 0;
                    while (j != choice_count)
                    {
                        s_colour_choice_ab *choice = &entry->choices[j];
                        if (!choice->field_1c_4 || choice->field_1c_4 == variant)
                        {
                            last = j;
                            weight += choice->weight;
                            eligible |= 1 << j;
                        }

                        ++j;
                    }
                    if (eligible && weight > 0.0f)
                    {
                        real cumulative = 0.0f;
                        real target = selection * weight;
                        long j = 0;
                        do
                        {
                            s_colour_choice_ab *choice = &entry->choices[j];
                            if (eligible & (1 << j))
                            {
                                cumulative += choice->weight;
                                if (j == last || cumulative >= target)
                                {
                                    hsv3f a, b, mixed;
                                    real inverse = 1.0f - blend;
                                    function_1318d0(&choice->lower, &a);
                                    function_1318d0(&choice->upper, &b);
                                    if (fabs(a.hue - b.hue) > 0.5f)
                                    {
                                        if (b.hue > a.hue) a.hue += 1.0f;
                                        else b.hue += 1.0f;
                                    }
                                    mixed.hue = a.hue * inverse + b.hue * blend;
                                    if (mixed.hue > 1.0f) mixed.hue -= 1.0f;
                                    mixed.saturation = a.saturation * inverse + b.saturation * blend;
                                    mixed.value = a.value * inverse + b.value * blend;
                                    function_131a00(&mixed, &output[i]);
                                    break;
                                }
                            }
                            ++j;
                        } while (j < choice_count);
                    }
                }
            }
            output[i].red = output[i].red < 0.0f ? 0.0f : output[i].red > 1.0f ? 1.0f : output[i].red;
            output[i].green = output[i].green < 0.0f ? 0.0f : output[i].green > 1.0f ? 1.0f : output[i].green;
            output[i].blue = output[i].blue < 0.0f ? 0.0f : output[i].blue > 1.0f ? 1.0f : output[i].blue;
            output[i + count] = output[i];
        }
    }

    function_3dd10(object_index, true);
}

bool __stdcall function_bab40(long object_index, long name, real *value);

// @retail 0xbe8b0
real __stdcall function_be8b0(long object_index, long name)
{
    real value;
    if (!function_bab40(object_index, name, &value))
        value = 0.0f;
    return value;
}

// @retail 0xbe6d0
real function_be6d0(long object_index, long attachment_index)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    byte *attachment = *(byte **)(definition + 0x98) + attachment_index * 24;
    real value = 1.0f;
    long name = *(long *)(attachment + 0x10);
    if (name)
        function_bab40(object_index, name, &value);
    if ((bool)(((dword)*(word *)(definition + 2) >> 9) & 1))
        return *(real *)(object + 0xa0) * value;
    return value;
}

struct s_colour_function_ab
{
    long unused;
    dword flags;
    color3f lower;
    color3f upper;
    long scale_name;
    long blend_name;
};
color3f *function_131c20(color3f *result, color3f const *a, color3f const *b, dword flags, real t);

// @retail 0xbe8e0
bool __stdcall function_be8e0(long object_index)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    s_object_definition const *definition =
        (s_object_definition const *)g_4e3b44[*(long *)object & 0xffff].bytes;
    bool result = false;
    if (definition->runtime_flags & 1)
    {
        color3f *base = (color3f *)(object + *(short *)(object + 0x126));
        long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
        for (long i = 0; i < count; i++)
        {
            s_colour_choices_ab *entry = &((s_colour_choices_ab *)definition->tint_sets.address)[i];
            color3f *current = &base[count + i];
            *current = base[i];
            for (long j = 0; j < entry->function_count; ++j)
            {
                s_colour_function_ab *function = &entry->functions[j];
                if (function->blend_name)
                {
                    function_131c20(current, &function->lower, &function->upper, function->flags, function_be8b0(object_index, function->blend_name));
                }
            }
            for (long j = 0; j < entry->function_count; ++j)
            {
                s_colour_function_ab *function = &entry->functions[j];
                if (function->scale_name)
                {
                    real value = function_be8b0(object_index, function->scale_name);
                    current->red *= value;
                    current->green *= value;
                    current->blue *= value;
                }
            }
            current->red = current->red < 0.0f ? 0.0f : current->red > 1.0f ? 1.0f : current->red;
            current->green = current->green < 0.0f ? 0.0f : current->green > 1.0f ? 1.0f : current->green;
            current->blue = current->blue < 0.0f ? 0.0f : current->blue > 1.0f ? 1.0f : current->blue;
            result = true;
        }
    }
    return result;
}
