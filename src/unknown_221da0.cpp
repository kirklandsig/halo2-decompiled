// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_221DA0.CPP: sound transmission helpers: whether a listener hears
   a sound through the structure's clusters, the effect parameters of a
   sound's environment, and the conversions of its DSP parameters */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "local_cameras.h"
#include <math.h>
#include <string.h>

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

/* a tag's data block, as the function evaluator reads it */
struct s_tag_data_view
{
	long size;
	byte *address;
};

real function_13b390(void const *function, real input, real range);

/* rounds as the x87 does (unknown_0259d0's fld/fistp idiom) */
static __forceinline long real_to_long(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

struct s_structure_bsp_view;
real function_249d60(s_structure_bsp_view *bsp, long cluster_a, long cluster_b);

/* a sound as the transmission test reads it */
struct s_sound_transmission_view
{
	byte flags;
	byte unknown01[2];
	byte type : 4;
	byte unknown03_4 : 4;
	byte unknown04[0x28 - 4];
	long bsp_index;
	short cluster_index;
};

/* whether a listener may hear a sound: always without a listener, never
   when the sound is in another structure or cluster too far away */
PRIVATE __forceinline bool function_221da1(s_local_camera *arg_0,
    s_sound_transmission_view const *arg_1, real arg_2, byte arg_3)
{
    short local_0 = arg_1->cluster_index;
    bool local_1 = arg_3;
    if (local_0 != NONE)
    {
        short local_2 = arg_0->index;
        if (local_2 != NONE &&
            (*(long *)arg_0->unknown00 == arg_1->bsp_index || local_2 == local_0 ||
             function_249d60((s_structure_bsp_view *)g_4e0348, local_2, local_0) * arg_2 < 256.0f))
        {
            local_1 = false;
        }
    }
    return local_1;
}

// @retail 0x221da0
bool function_221da0(long listener_index, s_sound_transmission_view const *sound, real scale)
{
	bool result = listener_index == NONE;
	if (!result && !(sound->flags & 1))
	{
		byte type = sound->type;
		if (type == 1)
		{
            result = function_221da1(local_camera_get(listener_index), sound, scale, type);
		}
	}
	return result;
}

long function_2221d0(real value);

real function_30bf0(vector3f *v);
real function_192b90(point3f const *direction, long speaker, bool linear);

/* the gain of a speaker for a direction */
// @retail 0x222250
real function_222250(vector3f const *direction, long speaker)
{
	real result = 1.0f;

	if (PIN(speaker, 0, 4) == speaker)
	{
		vector3f normal = *direction;

		function_30bf0(&normal);
		result = function_192b90((point3f const *)&normal, speaker, true);
		result = PIN(result, 0.0f, 1.0f);
	}
	return result;
}

/* ---- the effect data blocks of a sound effect ---- */

/* a DSP effect data block (src/unknown_21e330.cpp reads them) */
struct s_effect_data_header
{
	dword effect_mask;
	byte flags;
	byte unknown05[3];
	word offset;
	word size;
};

struct s_effect_data_block
{
	s_effect_data_header header;
	long data[1];
};

/* a parameter of an effect component (16 bytes) */
struct s_effect_parameter
{
	long type;
	real default_value;
	real minimum;
	real maximum;
};

/* an effect component (0x18 bytes): the effect it drives and the data
   written to it, then its parameters */
struct s_effect_component
{
	long name;
	s_effect_data_header header;
	long parameter_count;
	s_effect_parameter *parameters;
};

/* the components of a sound effect definition */
struct s_effect_components
{
	long count;
	s_effect_component *components;
};

/* a function driving a parameter from the effect's inputs (16 bytes) */
struct s_effect_function
{
	short input;
	short range_input;
	s_tag_data_view function;
	real period;
};

/* how a template sets the parameters of one component (0x1c bytes) */
struct s_effect_component_overrides
{
	long function_count;
	s_effect_function *functions;
	long constant_count;
	real *constants;
	long source_count;
	char *sources;
	dword parameter_mask;
};

/* a sound effect template's settings of the components */
struct s_effect_overrides
{
	long count;
	s_effect_component_overrides *components;
	byte unknown08[8];
	dword component_mask;
};

/* the inputs of the effect functions: four values, then the direction of
   the sound */
struct s_effect_inputs
{
	real values[4];
	vector3f direction;
};

dword function_191660(long string_handle, long index);

/* fills the data block of one component of a sound effect */
// @retail 0x2222c0
void function_2222c0(s_effect_data_block *block, s_effect_component const *component, s_effect_component_overrides const *overrides,
	s_effect_inputs const *inputs, long speaker)
{
	long source_index = 0;
	long i;

	block->header = component->header;
	block->header.effect_mask = function_191660(component->name, speaker);
	if (component->header.flags & 1)
	{
		long effect = NONE;

		switch (component->name)
		{
		case 0xd00012e:
			effect = 10;
			break;
		case 0x1300012d:
			effect = 4;
			break;
		}
		block->data[0] = effect;
	}

	for (i = 0; i < component->parameter_count; i++)
	{
		s_effect_parameter const *parameter = &component->parameters[i];
		real value = parameter->default_value;

		if (overrides && (overrides->parameter_mask & (1 << i)))
		{
			char source = overrides->sources[source_index];

			if (source & 1)
			{
				s_effect_function const *function = &overrides->functions[source >> 1];
				short input = function->input;
				bool ranged = TEST_FIELD_BIT(function->function.address[1] & 1);
				real x = inputs->values[input];
				real y = ranged ? inputs->values[function->range_input] : 0.0f;

				if (input == 1)
				{
					x = (real)fmod((double)(x / function->period), 1.0);
				}
				if (speaker != NONE && input == 3)
				{
					x *= function_222250(&inputs->direction, speaker);
				}
				if (ranged)
				{
					short range_input = function->range_input;

					if (range_input == 1)
					{
						y = (real)fmod((double)(y / function->period), 1.0);
					}
					if (speaker != NONE && range_input == 3)
					{
						y *= function_222250(&inputs->direction, speaker);
					}
				}
				if (function->function.address && function->function.size > 0)
				{
					value = function_13b390(&function->function, x, y);
					byte const *field_30 = function->function.address;

					if (!(field_30[1] & 0xf0))
					{
						real lower = *(real const *)(field_30 + 4);
						real upper = *(real const *)(field_30 + 8);

						value = lower + (upper - lower) * PIN(value, 0.0f, 1.0f);
					}
				}
				else
				{
					value = 0.0f;
				}
			}
			else
			{
				value = overrides->constants[source >> 1];
			}
			source_index++;
		}

		value = PIN(value, parameter->minimum, parameter->maximum);
		if (component->header.flags & 1)
		{
			switch (parameter->type)
			{
			case 0:
				block->data[i + 1] = real_to_long(value);
				break;
			case 1:
				*(real *)&block->data[i + 1] = value;
				break;
			}
		}
		else
		{
			switch (parameter->type)
			{
			case 0:
				block->data[i] = real_to_long(value);
				break;
			case 1:
				block->data[i] = function_2221d0(value);
				break;
			}
		}
	}
}

/* writes the data blocks of a sound effect's components to a buffer: their
   count, then each block; the size in bytes goes to buffer_size */
// @retail 0x2225a0
void function_2225a0(s_effect_overrides const *overrides, s_effect_components const *components, s_effect_inputs const *inputs,
	long *buffer, long *buffer_size)
{
	long offset = 0;
	long override_index = 0;
	long i;

	buffer[0] = components->count;
	*buffer_size = 4;
	for (i = 0; i < components->count; i++)
	{
		s_effect_component const *component = &components->components[i];
		s_effect_component_overrides const *component_overrides = NULL;
		bool by_speaker;
		long speaker;
		long speaker_count;

		if (overrides->component_mask & (1 << i))
		{
			component_overrides = &overrides->components[override_index++];
		}
		by_speaker = false;
		if (component->name == 0x11000138)
		{
			by_speaker = true;
		}
		speaker_count = by_speaker ? 4 : 0;
		for (speaker = by_speaker ? 0 : NONE; speaker < speaker_count; speaker++)
		{
			function_2222c0((s_effect_data_block *)&buffer[offset + 1], component, component_overrides, inputs, speaker);
			*buffer_size += ((s_effect_data_block *)&buffer[offset + 1])->header.size + 0xc;
			offset += (((s_effect_data_block *)&buffer[offset + 1])->header.size + 0xf) >> 2;
		}
	}
}

/* the environment effects of a sound */
struct s_sound_effect_entry
{
	byte unknown00[8];
	long name;
};

struct s_sound_effect_definition
{
	byte unknown00[0x14];
	long entry_count;
	s_sound_effect_entry *entries;
};

struct s_sound_effect_template_view
{
	byte unknown00[4];
	long definition_tag_index;
};

struct s_sound_effect_source
{
	byte unknown00[0x2c];
	long template_count;
	s_sound_effect_template_view *templates;
};

struct s_sound_effect_request
{
	dword flags;
	byte unknown04[0x8e - 4];
	short voice_effect_index;
	char mixbin;
	byte unknown91[0x98 - 0x91];
	long handle;
};

/* the voice effect settings (include/network_voice.h) */
struct s_voice_effects;
extern s_voice_effects *g_510c90;

struct s_sound_class_definition;
void *function_18d090(long tag_index, long handle);
s_sound_class_definition *sound_get_class(long tag_index);
long function_1914f0(long string_handle);

static inline s_sound_effect_template_view *sound_effect_source_get_template(s_sound_effect_source *source)
{
	return source->template_count > 0 ? source->templates : NULL;
}

// @retail 0x2226b0
void function_2226b0(long tag_index, s_sound_effect_request *request)
{
	long handle = (request->flags & 0x100) ? request->handle : NONE;
	s_sound_effect_source *effect = (s_sound_effect_source *)function_18d090(tag_index, handle);
	s_sound_effect_source *sound_class = (s_sound_effect_source *)sound_get_class(tag_index);
	s_sound_effect_source *source = NULL;

	if (sound_class && sound_class->template_count > 0)
	{
		source = sound_class;
	}
	if (effect && effect->template_count > 0)
	{
		source = effect;
	}

	if (source)
	{
		long definition_tag_index = sound_effect_source_get_template(source)->definition_tag_index;

		if (definition_tag_index != NONE)
		{
			s_sound_effect_definition *definition = (s_sound_effect_definition *)g_4e3b44[definition_tag_index & 0xffff].bytes;

			if (definition->entry_count > 0)
			{
				s_sound_effect_entry *entry = definition->entries;

				if (entry->name == 0x1300012d)
				{
					short voice_effect_index = ((short *)g_510c90)[3];

					if (voice_effect_index != NONE)
					{
						request->voice_effect_index = voice_effect_index;
						request->flags |= 0x40;
					}
				}

				short mixbin = (short)function_1914f0(entry->name);
				if (mixbin != NONE)
				{
					request->mixbin = (char)mixbin;
					request->flags |= 0x200;
				}
			}
		}
	}
}

/* ---- the effects of a sound's playback ---- */

/* a sound effect of a playback (its template): the effect definition's tag
   and the template's settings */
struct s_playback_sound_effect
{
	byte unknown00[4];
	long definition_tag_index;
	byte unknown08[0x20 - 8];
	long override_count;
	s_effect_overrides *overrides;
};

/* a platform playback, or a sound class's (0x34 bytes) */
struct s_platform_playback_view
{
	byte unknown00[0x14];
	long filter_count;
	void *filters;
	long pitch_lfo_count;
	void *pitch_lfos;
	long filter_lfo_count;
	void *filter_lfos;
	long effect_count;
	s_playback_sound_effect *effects;
};

/* a sound effect tag: its components */
struct s_sound_effect_tag_view
{
	byte unknown00[0x14];
	long count;
	s_effect_components *components;
};

/* the effect data of a sound: data blocks and their size in bytes */
struct s_sound_effect_buffer
{
	long data[0x100];
	long size;
};

/* what a sound's playback applies to it */
struct s_sound_playback_effects
{
	byte unknown00[8];
	long unknown08;
	long unknown0c;
	void *filter;
	void *pitch_lfo;
	void *filter_lfo;
	s_sound_effect_buffer effect;
};

static inline void *tag_block_get_first(long count, void *elements)
{
	if (PIN(0, 0, count - 1) == 0)
	{
		return elements;
	}
	return NULL;
}

static inline bool playback_sound_effect_valid(s_playback_sound_effect const *effect)
{
	return effect && effect->definition_tag_index != NONE && effect->override_count > 0;
}

static inline s_effect_components *sound_effect_tag_get_components(long tag_index)
{
	return ((s_sound_effect_tag_view *)g_4e3b44[tag_index & 0xffff].bytes)->components;
}

/* the effects of a sound played through its class and its playback */
// @retail 0x222770
void function_222770(long tag_index, long handle, s_effect_inputs const *inputs, s_sound_playback_effects *effects)
{
	s_platform_playback_view *source = NULL;
	s_platform_playback_view *playback = (s_platform_playback_view *)function_18d090(tag_index, handle);
	s_platform_playback_view *class_playback = (s_platform_playback_view *)sound_get_class(tag_index);

	if (class_playback)
	{
		void *filter = tag_block_get_first(class_playback->filter_count, class_playback->filters);
		void *pitch_lfo = tag_block_get_first(class_playback->pitch_lfo_count, class_playback->pitch_lfos);
		void *filter_lfo = tag_block_get_first(class_playback->filter_lfo_count, class_playback->filter_lfos);
		s_playback_sound_effect *effect = (s_playback_sound_effect *)tag_block_get_first(class_playback->effect_count, class_playback->effects);

		effects->unknown08 = 0;
		effects->unknown0c = 0;
		effects->filter = filter;
		effects->pitch_lfo = pitch_lfo;
		effects->filter_lfo = filter_lfo;
		if (playback_sound_effect_valid(effect))
		{
			source = class_playback;
		}
	}
	if (playback && playback != class_playback)
	{
		void *filter = tag_block_get_first(playback->filter_count, playback->filters);
		void *pitch_lfo = tag_block_get_first(playback->pitch_lfo_count, playback->pitch_lfos);
		void *filter_lfo = tag_block_get_first(playback->filter_lfo_count, playback->filter_lfos);
		s_playback_sound_effect *effect = (s_playback_sound_effect *)tag_block_get_first(playback->effect_count, playback->effects);

		if (filter)
		{
			effects->filter = filter;
		}
		if (pitch_lfo)
		{
			effects->pitch_lfo = pitch_lfo;
		}
		if (filter_lfo)
		{
			effects->filter_lfo = filter_lfo;
		}
		if (playback_sound_effect_valid(effect))
		{
			source = playback;
		}
	}

	if (source)
	{
		/* retail reads the playback's effect here, whichever was chosen */
		s_playback_sound_effect *effect = playback->effect_count > 0 ? playback->effects : NULL;

		function_2225a0(effect->overrides, sound_effect_tag_get_components(effect->definition_tag_index), inputs,
			effects->effect.data, &effects->effect.size);
	}
	else
	{
		effects->effect.size = 0;
	}
}

/* the effect data of a platform playback */
// @retail 0x2228d0
void function_2228d0(long handle, s_sound_effect_buffer *buffer, s_effect_inputs const *inputs)
{
	s_platform_playback_view *playback = (s_platform_playback_view *)function_18d090(NONE, handle);

	if (playback)
	{
		s_playback_sound_effect *effect = (s_playback_sound_effect *)tag_block_get_first(playback->effect_count, playback->effects);

		if (playback_sound_effect_valid(effect))
		{
			function_2225a0(effect->overrides, sound_effect_tag_get_components(effect->definition_tag_index), inputs,
				buffer->data, &buffer->size);
		}
	}
}

/* ---- the sound environment a point hears ---- */

struct s_14b240_owner;
struct s_bsp3d_disk;
long function_18cfd0(long cluster_index, point3f const *point, real *distance);
real function_14b240(s_14b240_owner const *owner, s_bsp3d_disk const *disk, point3f const *point);

/* the structure's sound environments, as their disks (0x24 bytes) */
struct s_sound_environment_disk_view
{
	byte unknown00[0x24];
};

struct s_sound_environment_bsp_view
{
	byte unknown00[0x60];
	s_sound_environment_disk_view *environments;
};

/* how much a point is inside the sound environment nearest to a source in
   a cluster: 0 at its disk, approaching 1 far away; and a value of the
   source */
// @retail 0x222150
void function_222150(long cluster_index, point3f const *point, real const *values, point3f const *listener, long index, real *result)
{
	s_sound_environment_bsp_view *bsp = (s_sound_environment_bsp_view *)g_4e0348;
	real distance;
	long environment_index = function_18cfd0(cluster_index, point, &distance);

	if (environment_index != NONE)
	{
		s_sound_environment_disk_view *const volatile *local_2 = &bsp->environments;
		s_sound_environment_disk_view *local_0 = environment_index + *local_2;
		real value = ((volatile real const *)values)[index];
		real local_1 = function_14b240((s_14b240_owner const *)bsp, (s_bsp3d_disk const *)local_0, listener);
		result[1] = value;
		result[0] = (real)(1.0 - 1.0f / (local_1 + 1.0f));
	}
	else
	{
		result[0] = 1.0f;
		result[1] = 0.0f;
	}
}

struct s_221e30
{
	long field_0;
	point3f field_4, field_10;
	real field_1c, field_20;
	point3f field_24;
	real field_30, field_34, field_38;
	long field_3c, field_40;
	bool field_44, field_45, field_46, field_47;
};

struct s_221e31
{
	real field_0;
	byte field_4[0x418];
};

struct s_221e32
{
	byte field_0[0x7a];
	short field_7a;
	byte field_7c[0x34];
};

struct s_221e33
{
	union
	{
		struct { vector3f field_0, field_c; } field_0;
		real field_18[2];
	};
};

struct s_collision_bsp_test_vector_result;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
extern byte *g_4ed280;
bool function_1de630(dword arg_0, s_slot_entry_list *arg_1, s_collision_bsp_test_vector_result *arg_2,
	real arg_3, long arg_4, byte const *arg_5, point3f const *arg_6, vector3f const *arg_7);
void function_222150(long arg_0, point3f const *arg_1, real const *arg_2,
	point3f const *arg_3, long arg_4, real *arg_5);

// @retail 0x221e30
void function_221e30(s_221e30 *arg_0, real const *arg_1, s_local_camera const *arg_2)
{
	(void)&arg_1; (void)&arg_2;
	if (*(long const *)arg_2->unknown00 != arg_0->field_40)
	{
		s_221e33 local_10;
		s_structure_bsp_view *local_13 = (s_structure_bsp_view *)g_4e0348;
		real local_0 = 0.0f;
		if (!arg_0->field_47)
		{
			s_221e31 local_1, local_2;
			byte const *local_5 = g_4ed280 + g_4686c4 * 0x20 + 1;
			local_10.field_0.field_0.i = arg_0->field_24.x - arg_2->position.x;
			local_10.field_0.field_0.j = arg_0->field_24.y - arg_2->position.y;
			local_10.field_0.field_0.k = arg_0->field_24.z - arg_2->position.z;
			if (function_1de630(1, g_4e0340, (s_collision_bsp_test_vector_result *)&local_1,
				1.0f, 0x100, local_5, &arg_2->position, &local_10.field_0.field_0))
			{
				local_10.field_0.field_c.i = 0.0f - local_10.field_0.field_0.i;
				local_10.field_0.field_c.j = 0.0f - local_10.field_0.field_0.j;
				local_10.field_0.field_c.k = 0.0f - local_10.field_0.field_0.k;
				if (function_1de630(1, g_4e0340, (s_collision_bsp_test_vector_result *)&local_2,
					1.0f, 0x100, local_5, &arg_0->field_24, &local_10.field_0.field_c))
				{
					local_0 = PIN(1.0f - local_2.field_0 - local_1.field_0, 0.0f, 1.0f);
					if (local_0 > 0.0f)
					{
						arg_0->field_45 = true;
						arg_0->field_4.x = local_10.field_0.field_0.i * local_1.field_0 + arg_2->position.x;
						arg_0->field_4.y = local_10.field_0.field_0.j * local_1.field_0 + arg_2->position.y;
						arg_0->field_4.z = local_10.field_0.field_0.k * local_1.field_0 + arg_2->position.z;
						arg_0->field_1c = local_1.field_0;
						arg_0->field_10.x = local_10.field_0.field_c.i * local_2.field_0 + arg_0->field_24.x;
						arg_0->field_10.y = local_10.field_0.field_c.j * local_2.field_0 + arg_0->field_24.y;
						arg_0->field_10.z = local_10.field_0.field_c.k * local_2.field_0 + arg_0->field_24.z;
						arg_0->field_20 = local_2.field_0;
					}
				}
				else
					local_0 = 1.0f - local_1.field_0;
			}
		}
		else if (!arg_0->field_44 && arg_2->index != arg_0->field_3c)
		{
			real local_6 = function_249d60(local_13, arg_2->index, arg_0->field_3c);
			real local_7 = distance3d(&arg_0->field_24, &arg_2->position);
			local_0 = 1.0f - local_7 / ((local_7 + local_6) > 0.001f ? (local_7 + local_6) : 0.001f);
		}
		if (arg_2->index != NONE && arg_0->field_3c != NONE && arg_2->index != arg_0->field_3c)
		{
			if (arg_0->field_44)
			{
				arg_0->field_34 = 0.0f;
				arg_0->field_38 = arg_1[arg_0->field_3c];
			}
			else
			{
				function_222150(arg_0->field_3c, &arg_0->field_24, arg_1,
					&arg_2->position, arg_0->field_3c, local_10.field_18);
				s_221e32 const *local_9 = *(s_221e32 const **)((byte *)g_4e0348 + 0xa0);
				if (local_9[arg_2->index].field_7a == local_9[arg_0->field_3c].field_7a)
					local_10.field_18[0] = 0.0f;
				arg_0->field_34 = local_10.field_18[0];
				arg_0->field_38 = local_10.field_18[1];
			}
		}
		arg_0->field_30 += local_0;
	}
}

#include "unknown_249e20.h"

struct s_voice_position_entry
{
	long listener;
	byte unknown04[0x20];
	point3f position;
	vector3f result;
	long index, object;
	bool flag44, flag45, flag46, flag47;
};

struct s_voice_position_batch
{
	long count;
	s_voice_position_entry entries[0x48];
};

typedef void *(__fastcall *constructor_proc)(void *);
void __stdcall vector_constructor_iterator(void *arg_0, unsigned arg_1, int arg_2, constructor_proc arg_3);
void *__fastcall function_31bc70(void *arg_0);
real function_18cac0(long arg_0);

// @retail 0x221a70
void __stdcall function_221a70(long arg_0, s_voice_position_batch *arg_1)
{
	(void)&arg_0; (void)&arg_1;
	s_local_camera *local_0 = local_camera_get(arg_0);
	if (local_0 && local_0->index != NONE)
	{
		s_structure_bsp_view *local_1 = (s_structure_bsp_view *)g_4e0348;
		real local_2[0x200];
		memset(local_2, 0, sizeof(local_2));
		if (local_1->audibility_count > 0)
		{
			s_structure_audibility *local_3 = local_1->audibility;
			dword local_4[4] = {0};
			function_249d10(local_1, local_4, local_0->index);
			for (long local_5 = 0; local_5 < local_3->door_count; local_5++)
			{
				if (local_4[local_5 >> 5] & (1 << (local_5 & 31)))
				{
					long local_6 = local_1->audibility->clusters[local_5];
					if (local_6 != NONE)
					{
						real local_7 = function_18cac0(local_6);
						dword local_8[2][16];
						vector_constructor_iterator(local_8, 0x40, 2, function_31bc70);
						function_249c90(local_8[0], local_1, local_5, local_8[1]);
						long local_9;
						long local_10 = local_0->index;
						if (local_8[0][local_10 >> 5] & (1 << (local_10 & 31)))
							local_9 = 1;
						else if (local_8[1][local_10 >> 5] & (1 << (local_10 & 31)))
							local_9 = 0;
						else
							continue;
						for (long local_11 = 0; local_11 < local_1->cluster_count; local_11++)
						{
							if (local_11 != local_10 && (local_8[local_9][local_11 >> 5] & (1 << (local_11 & 31))))
								local_2[local_11] = local_2[local_11] > local_7 ? local_2[local_11] : local_7;
						}
					}
				}
			}
		}
		for (long local_12 = 0; local_12 < arg_1->count; local_12++)
		{
			if (arg_1->entries[local_12].listener == arg_0)
				function_221e30((s_221e30 *)&arg_1->entries[local_12], local_2, local_0);
		}
	}
}
