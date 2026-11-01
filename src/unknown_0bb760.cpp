#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"

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

/* the word of flags at 0x10a of an object */
struct s_object_flags_10a
{
    byte unknown000[0x10a];
    word bit0 : 1;
    word bit1 : 1;
    word bit2 : 1;
    word : 13;
};

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

// Round 30 (lane D): the permuter found no better form (82 tries, best 84, the baseline). On this body
// the variant byte read before the blend value scores 171 and before the selection seed 190; with
// retail's found path (the inverse inside the found branch) it is 123, and the plain inverse form 132:
// retail's not-found exit reloads 1.0 into xmm5 while ours rejoins the block's load, so the tail sits
// 8 bytes short and every later branch target shifts. Original body kept.
// score 84 (was 84): the first scan's `!=` exits (retail's signed `<` loop unrolls by 4 in every
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
// @retail 0xbe240
void __stdcall function_be240(long object_index, dword color_mask, color3f const *colors)
{

    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    color3f *output = (color3f *)(object + *(short *)(object + 0x126));
    long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
    for (long i = 0; i < count; ++i)
    {
        s_colour_choices_ab *entry = &(*(s_colour_choices_ab **)(definition + 0xb0))[i];
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
                char variant_index = *(char *)(object + 0xb1);

                if (variant_index != NONE && *(long *)(definition + 0x38) != NONE)
                {
                    byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
                    variant = *(long *)(*(byte **)(model + 0x54) + variant_index * 0x38);
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
                    real inverse = 1.0f - blend;
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
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result);

// score 91 (was 91): retail keeps the destination pointer in EBX and
// the source pointer in its stack slot, while ours keeps the source in EBX and the destination in
// EDI (which also feeds the 0x131c20 result register), and retail counts the colours with a signed
// 32-bit division by 24 (magic 0xAAAAAAAB) where our (long) cast gives the cheaper 16-bit-range
// sequence. Round 31 (lane D): the multiply block written green, red, blue with the value first
// 90 (one point better, not kept), the same block with green and blue exchanged 105 each, the
// block with the value written after the component 91 each, the destination formed after the count
// test 99, the source in a separate variable 179, a byte-pointer destination 179, a temporary copy
// of the colour 91, the destination pointer advanced at the end of the body 93, an indexed
// destination 148 and a constant source 91; every one restored.
// Earlier: retail keeps the destination pointer in EBX and the source pointer
// only in its stack slot, while ours keeps the source pointer in EBX and the destination in EDI;
// retail also counts the colours with a signed 32-bit division by 24 (magic 0xAAAAAAAB), while our
// (long) cast lets MSVC use the cheaper 16-bit-range sequence, so the value agrees but the
// instruction sequence does not.
// Earlier: retail tests the count against zero before it computes
// the output pointer, ours computes the pointer first, and retail keeps the
// running offset in another register, so the test, the two leas and the
// loop's bookkeeping differ.
// Round 15 (m3 lane 2, LTCG env harness = file + depth-4 closure; positional/aligned, base 84/55 = full 91):
// plain `for (i = 0; i < count; i++)` with `current = &base[count + i]` and `result = true` at the body's
// top/end/after the copy 129/55 each (563 bytes: source kept in its stack slot as retail, but the false
// result takes BL, EBX is pushed at entry, the two false exits merge and the destination takes EDI);
// on it `&base[i + count]`, `base + count + i`, a source pointer, current declared first, the copy first,
// `colors` name, count declared before the array, a flags local 129/129/129/129/143/129/113/129; early
// `return false` 170; result declared in the block 95; `result = count > 0` after the loop 168; result set
// before the loop 91 (aligned 75); pointer increments in a plain for 97, inside the count test 108.
// Direct size/24 count 161; const base, entry and function views 91;
// unsigned count 171; component clamp loop 91; scale loop and both loops 90
// (one-count gain rejected).
// current takes edi (the 0x131c20 result register) where retail gives the
// blend function pointer edi and current ebx, base on the stack. Current declared first 91;
// count before base 174; current from base each pass 98; copy before the entry 133; memcpy copy 91.
// Shared count/inner-array/both snapshots 177/164/194; outer-array snapshot 206; outer plus
// count/inner/both 213/210/228; shared inner index with count and array 188.
// Round 212: complemented bool/byte/short/long return on plain pointer-body traversal 159/159/131/132;
// with admitted current lifetime 158/158/129/130 (fast control 91). Original return, pointer lifetimes
// and traversal restored; saved receiver roles remain.
// Complete native bounded-color publication 172; with actual output reference 164;
// with overlap-safe input transfer 214, with both 211. Complete scalar bound results 109;
// with output reference 116; overlap-safe input 155, with both 152. Original color
// operations, observations and pointer traversal restored; current/base register roles remain.
// Complete scalar bounds with overlap-safe input/output reference reach first +0x66
// at 152/580, but introduce an out-of-line transfer and worsen the total; rejected.
// Round 212: complemented bool/byte/short/long return on plain pointer-body traversal 159/159/131/132;
// with admitted current lifetime 158/158/129/130 (fast control 91). Original return, pointer lifetimes
// and traversal restored; saved receiver roles remain.
// Latest round: retail keeps the destination pointer in ebx and the source pointer only in the
// stack slot at E+0x0c, and gives the blend loop's scratch pointer and the second loop's byte
// offset to edi; ours keeps the source pointer in ebx and the destination in edi, and uses ebx
// for that scratch pointer and offset. The copy, the blend call and the scale and clamp
// accesses therefore name different registers.
// Retail also forms the destination after the count test, and scales red as value*red, green
// as green*value and blue as blue*value, while ours scales red as red*value, green as
// value*green and blue as value*blue.
// Destination formed after the count test 100; the same with the destination built from the
// object expression 119; the source declared after the destination and not used before the
// loop 172; the destination passed as a plain pointer 91; the scale multiplies written out 91;
// the destination taken as &base[count] 91; the two pointer increments swapped 91.
// Source pointer derived from the destination inside the loop, so that the source induction
// variable is never used in the preheader 186.
// Lane 14 r20: a plain `for (i = 0; i < count; i++)` with `&base[count + i]` and `result = true`
// in the body 149 (563 bytes: VC hoists the result store into the preheader and keeps the source
// pointer only in its stack slot, as retail, but the false result takes BL with EBX pushed at
// entry, so the two false exits merge, and the destination still takes EDI); the same inside the
// count guard 148; with `result = true` before that loop 107; a destination array `base + count`
// indexed by i 164; the destination pointer stepped at the end of the body 161; red scaled by `*=`
// and green and blue as `value *` 91 (identical code: VC picks the mulss operand order itself); the
// destination formed after the count test 99 (again).
// Round 86: scalar component copy 139; blend-only count 169; scale-only count 170;
// blend-only function array 173; scale-only function array 139; shared inner counter 105;
// shared function pointer 91; advancing outer entry pointer 203. Original body restored.
// Round 52: both value reads through the existing 0xbe8b0 91 (identical code, kept: it replaces that
// helper's body written out twice); color scale and clamp as inline helpers 91 (identical); the
// destination assigned first from the object 172 (575 bytes); one colour-array local split into
// destination and source 182 (586 bytes); a plain `for (i = 0; i < count; i++)` with a per-iteration
// destination pointer 186 (556 bytes). Screened: the destination declared before the source but
// assigned after it identical; the source derived from the destination 164; indexed `current[i]`
// accesses 135. The destination keeps EDI (the 0x131c20 result register) in every form.
// Round 38 (m3 lane 14, fast): the destination formed inside the count test, before/after `result = true`,
// 100/99 (576 bytes: retail's test-first order, but the source is stored before the branch and the
// destination keeps EDI); the blend output passed as `&base[count]` 209 (609 bytes); the source read as
// `base[i]` with no source increment 161; a plain `for (i = 0; i < count; i++)` with an indexed source and
// destination 107; that loop stepping only the destination 161.
// @retail 0xbe8e0
bool __stdcall function_be8e0(long object_index)
{
    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    bool result = false;
    if (definition[0x1c] & 1)
    {
        color3f *base = (color3f *)(object + *(short *)(object + 0x126));
        long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
        color3f *current = base + count;
        if (count > 0)
        {
            result = true;
            for (long remaining = count, i = 0; remaining > 0; --remaining, ++i, ++current, ++base)
            {
                s_colour_choices_ab *entry = &(*(s_colour_choices_ab **)(definition + 0xb0))[i];
                *current = *base;
                for (long j = 0; j < entry->function_count; ++j)
                {
                    s_colour_function_ab *function = &entry->functions[j];
                    if (function->blend_name)
                    {
                        function_131c20(&function->lower, &function->upper, function->flags, function_be8b0(object_index, function->blend_name), &*current);
                    }
                }
                for (long j = 0; j < entry->function_count; ++j)
                {
                    s_colour_function_ab *function = &entry->functions[j];
                    if (function->scale_name)
                    {
                        real value = function_be8b0(object_index, function->scale_name);
                        current->red = value * current->red;
                        current->green *= value;
                        current->blue *= value;
                    }
                }
                current->red = current->red < 0.0f ? 0.0f : current->red > 1.0f ? 1.0f : current->red;
                current->green = current->green < 0.0f ? 0.0f : current->green > 1.0f ? 1.0f : current->green;
                current->blue = current->blue < 0.0f ? 0.0f : current->blue > 1.0f ? 1.0f : current->blue;
            }
        }
    }
    return result;
}
