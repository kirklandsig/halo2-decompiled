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

inline void dialogue_name_override(long *name, long value)
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
// the default-element and object-pool temporaries land in EDX where retail keeps
// them in EAX, and the scaled object index goes to EAX where retail uses EDX; the loop padding
// follows (our pool load is a byte longer, so the pad before the variant loop is a byte shorter).
// Naming the elements, the pool or both does not move the allocation. Round 47: typed default
// pool-pointer local 9. Round 16: the default name from an inline helper with a result local 9, the
// helper as a ternary 48; the vocalization's default name read at each use 9; a fresh actor lookup
// for the unit's index 129; the vocalization default declared ahead of the default name 9; the unit
// index read from the actor at both tests 9.
// Round 213 (m3 lane 3, fast): retail's two loads reuse the dying register (`mov eax, [eax + 0xcc]`, matched 0x1c9e50 likewise); `globals_tag` reassigned
// to the element 9; an element local set in the arm 9; both reads kept as two statements 9; an inline helper returning the name 9; g_4e034c read at each
// use 9; an inline element getter 97 (467 bytes). Dead `volatile` load probes (reverted) before the tag, the test, the name or the unit lookup 84-110
// (440-448 bytes: the name leaves the stack and the frame shrinks), so no rotation step could be located. Retail's lookup shape (scaled index EDX, pool
// reuses EAX) also matches 0x28f8d0's difference; both shapes occur in matched functions with the same inline.
// Round 75 (m3 lane 11, fast): the override helper's pointer parameter without `volatile` 9 (same code, kept); `name`
// declared first in the found block 9; the default name at function scope, zeroed at entry, 146 (450 bytes); the unit
// lookup through the `unit_index` local 129 (the listed cached unit index, re-measured); the default name from a
// force-inline helper with early returns 48; the tag table global read at each use, no `tags` local, 9 (same code).
// Round 78 (lane 10, fast): the vocalization's name now takes the unit stage's override-then-fallback shape
// (`name = NONE`, dialogue_name_override, then the default while still invalid) 9 with byte-identical code
// (kept; it replaces a generic local tested twice). Also: `long name = vocalization->default_name` with one
// fallback test 177 (438 bytes). On the kept form: the search as its own loop with an `i < count` test after
// it 124 (468 bytes); the one-use unit_index local dropped 9 and default_name declared before the globals tag
// pointer 9 (same code each). Diagnostics: one variant override in place of two 56 (451 bytes) and a cut-down
// default block leave the elements/pool registers where they are.
// Round 160 (m3 lane 6, fast): a 600 s permuter run (59 tries) none below 9; the unit's dialogue state through a
// force-inline getter taking the unit index 9; the globals-tag pointer declared at function scope 9; the override
// helper as __forceinline 9; the actor through the byte-offset actor getter 9; the scenario global read at each use 16.
// Round 187 (m3 lane 4, fast): `name` declared at function scope 9 (same code); `return result` at the definition and
// sound NONE exits 87 (478 bytes); the masked unit index in a named local 9 (same code); the definition NONE test as a
// nested block 9 (same code); the variant mismatch as `continue` 9 (same code). Re-measured on the round-78 override
// form: round 16's vocalization default read ahead of the default name, now 126 (440 bytes, the name leaves the stack).
// Round 96 (m3 lane 10, fast): the two differing loads are one cause each: retail's elements and pool loads reuse
// EAX (the pool's `mov eax, [mem]` is a byte shorter, which is the jump and pad differences). Parameters ordered
// (actor, name, script, duration) at the definition, declarations and all three calls 9 (same code); the default
// name and the unit's dialogue state both through inline helpers 9; the override helper by reference 9; result
// declared before the scenario pointer 9; the tag table pointer declared at function scope 9. All restored.
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

						long name = NONE;
						dialogue_name_override(&name, vocalization->default_name);
						if (name == NONE || name == 0)
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
