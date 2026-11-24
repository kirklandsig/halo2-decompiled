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
color3f *function_131c20(color3f *result, color3f const *a, color3f const *b, dword flags, real t);
color3f *function_131a00(hsv3f const *hsv, color3f *rgb);
void function_3dd10(long object_index, bool force);

// @retail 0xbe240
void __stdcall function_be240(long object_index, dword color_mask, color3f const *colors)
{

    byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    s_object_definition const *definition = (s_object_definition const *)g_4e3b44[*(long *)object & 0xffff].bytes;
    color3f *output = (color3f *)(object + *(short *)(object + 0x126));
    long count = (long)((dword)(long)*(short *)(object + 0x124) / 12) / 2;
    for (long i = 0; count > i; ++i)
    {
        s_colour_choices_ab const *entry = &((s_colour_choices_ab const *)definition->tint_sets.address)[i];
        if (color_mask & (1 << i))
            output[i] = colors[i];
        else
        {
            output[i] = *(color3f *)g_468710;
            long choice_count = entry->count;
            if (0 < choice_count)
            {
                long last = NONE;
                dword eligible = 0;
                long variant = 0;
                real weight = 0.0f;
                real index = (real)i;
                real seed = (real)fabs((*(real *)(object + 0x6c) * 744.1241455078125f + *(real *)(object + 0x64) * 315.89312744140625f + index * 431.1289367675781f) + *(real *)(object + 0x68) * 587.1294555664062f);
                real selection = (real)fmod(seed, 1.0f);
                seed = (real)fabs(index * 0.7121099829673767f + *(real *)(object + 0x68));
                real blend = (real)fmod(seed, 1.0f);
                char variant_index = *(char *)(object + 0xb1);

                if (variant_index != NONE && definition->model_tag_index != NONE)
                {
                    byte *model = g_4e3b44[definition->model_tag_index & 0xffff].bytes;
                    variant = *(long *)(*(byte **)(model + 0x54) + variant_index * 0x38);
                }

                for (long j = 0; choice_count > j; ++j)
                {
                    s_colour_choice_ab *choice = &entry->choices[j];
                    if (!choice->field_1c_4 || choice->field_1c_4 == variant)
                    {
                        last = j;
                        weight += choice->weight;
                        eligible |= 1 << j;
                    }
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
                                function_131c20(&output[i], &choice->lower, &choice->upper, 1, blend);
                                break;
                            }
                        }
                        ++j;
                    } while (j < choice_count);
                    /* a statement the release build compiles out: retail keeps the
                       selection loop's exit join as its own block (the 1.0 load on the
                       not-found path sits ahead of the count reload there) */
                    do {} while (false);
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
