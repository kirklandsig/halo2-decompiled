// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_291EA0.CPP: the ai dialogue vocalizations an actor or an object
   speaks (outside functions lane A's script functions need) */

#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"
#include "unknown_272b70.h"
#include "unknown_29f5b0.h"

/* a variant of a vocalization, by the speaker's dialogue definition
   (0x10 bytes) */
struct s_vocalization_variant
{
	long dialogue_name;
	long unknown04;
	long sound_index;
	long name;
};

/* a vocalization of the ai dialogue globals (0x10 bytes) */
struct s_vocalization
{
	long name;
	long variant_count;
	s_vocalization_variant *variants;
	long default_name;
};

/* the ai dialogue globals tag */
struct s_ai_dialogue_globals
{
	long vocalization_count;
	s_vocalization *vocalizations;
};

struct s_tag_reference_view
{
	long group_tag;
	long index;
};

/* the scenario's reference to the ai dialogue globals (g_4e0350) */
struct s_scenario_ai_dialogue_view
{
	byte unknown000[0x3b0];
	long ai_dialogue_globals_count;
	s_tag_reference_view *field_3b4;
};

/* the globals tag (g_4e034c): the default name at +0x68 of its first block
   element */
struct s_globals_tag_dialogue_view
{
	byte unknown00[0xc8];
	long count;
	byte *elements;
};

/* the dialogue state of a unit, at the offset the unit's +0x342 gives */
struct s_unit_dialogue_view
{
	long dialogue_definition_index;
	long unknown04;
	long name;
};

/* the dialogue definition tag of a unit */
struct s_dialogue_definition_view
{
	byte unknown00[0x14];
	long name;
};

struct s_unit_291ea0
{
	byte unknown000[0x342];
	short dialogue_offset;
};

struct s_object_header_291ea0
{
	byte unknown00[8];
	s_unit_291ea0 *object;
};

inline void dialogue_name_override(long volatile *name, long value)
{
	if (value != 0 && value != NONE)
		*name = value;
}

#define TAG_GET_291EA0(type, index) ((type *)g_4e3b44[(index) & 0xffff].bytes)

/* plays the actor's variant of a vocalization; returns whether it found one */
// score 9 (was 9): the elements pointer and the object pool still land in EDX where retail
// keeps them in EAX, and the scaled object index goes to EAX where retail uses EDX; naming the
// elements, the pool or both does not move the allocation; the loop padding byte differs too.
// Round 47: typed default
// element 9; header pointer arithmetic 9; default if/else 48; cached unit index 129; direct globals
// access 9; existing object getter 9; first-element syntax 9; scaled header local 9; header-base local 9;
// pool-pointer local 9.
// Round 213: byte result 167; short result 165; long result 167; override sentinel first 18; override
// invalid return 9; override zero nesting 9; override sentinel return 18; named stack homes 9; original
// executable source restored.
// @retail 0x291ea0
bool function_291ea0(long arg_80f1d4, long actor_index, long script_index, real *duration)
{
	s_scenario_ai_dialogue_view *scenario = (s_scenario_ai_dialogue_view *)g_4e0350;
	bool result = false;

	if (duration)
		*duration = 0.0f;
	if (scenario->ai_dialogue_globals_count > 0)
	{
		s_actor_datum *actor = actor_datum_get(actor_index);
		long unit_index = actor->unit_index;
		if (unit_index != NONE)
		{
			long globals_index = scenario->field_3b4[0].index;
			if (globals_index != NONE)
			{
				s_tag_instance *tags = g_4e3b44;
				s_ai_dialogue_globals *globals = (s_ai_dialogue_globals *)tags[globals_index & 0xffff].bytes;
				for (short i = 0; i < globals->vocalization_count; i++)
				{
					s_vocalization *vocalization = &globals->vocalizations[i];
					if (vocalization->name == arg_80f1d4)
					{
						s_globals_tag_dialogue_view *globals_tag = (s_globals_tag_dialogue_view *)g_4e034c;
						long default_name = 0;
						if (globals_tag && globals_tag->count > 0)
							default_name = *(long *)(globals_tag->elements + 0x68);

						long local_dn = vocalization->default_name;
						long name;
						if (local_dn != 0)
							name = local_dn;
						if (local_dn == 0 || local_dn == NONE)
							name = default_name;

						s_unit_291ea0 *unit = ((s_object_header_291ea0 *)g_4e0300->data)[actor->unit_index & 0xffff].object;
						s_unit_dialogue_view *dialogue = (s_unit_dialogue_view *)((byte *)unit + unit->dialogue_offset);
						if (dialogue->dialogue_definition_index == NONE)
							break;

						long unit_name = dialogue->name;
						s_dialogue_definition_view *definition = (s_dialogue_definition_view *)tags[dialogue->dialogue_definition_index & 0xffff].bytes;
						dialogue_name_override(&name, unit_name);
						if (name == NONE || name == 0)
							name = unit_name;

						for (short j = 0; j < vocalization->variant_count; j++)
						{
							s_vocalization_variant *variant = &vocalization->variants[j];
							if (variant->dialogue_name == definition->name)
							{
								if (variant->sound_index == NONE)
									break;
								dialogue_name_override(&name, variant->name);
								dialogue_name_override(&name, variant->name);

								real seconds = function_2760a0(actor_index, script_index, name, variant->sound_index, 1.0f, 1.0f);
								if (duration)
									*duration = seconds;
								result = true;
								break;
							}
						}
						break;
					}
				}
			}
		}
	}
	return result;
}

/* plays a vocalization on an object; returns whether it found one */
// @retail 0x292080
bool function_292080(long object_index, long arg_80f1d4, real *duration)
{
	s_scenario_ai_dialogue_view *scenario = (s_scenario_ai_dialogue_view *)g_4e0350;
	bool result = false;

	if (duration)
		*duration = 0.0f;
	if (scenario->ai_dialogue_globals_count > 0)
	{
		long globals_index = scenario->field_3b4[0].index;
		if (globals_index != NONE)
		{
			s_ai_dialogue_globals *globals = TAG_GET_291EA0(s_ai_dialogue_globals, globals_index);
			for (short i = 0; i < globals->vocalization_count; i++)
			{
				s_vocalization *vocalization = &globals->vocalizations[i];
				if (vocalization->name == arg_80f1d4)
				{
					long name = vocalization->default_name;
					if (vocalization->variant_count > 0)
					{
						s_vocalization_variant *variant = &vocalization->variants[0];
						if (variant->name != 0 && variant->name != NONE)
							name = variant->name;
						function_189cd0(variant->sound_index, object_index, 1.0f, g_444ae0, g_444ae0, name, (long)duration);
						result = true;
					}
					break;
				}
			}
		}
	}
	return result;
}
