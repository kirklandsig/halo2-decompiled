// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_10DC70.CPP: the vibration and trigger state of an object (decay,
   blending of the per-channel values, reset) and a few layout helpers */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>

real g_547648;
real g_54764c;

struct s_tag_data_1e8
{
	byte unknown00[0x1e8];
	real value;
};

struct s_datum
{
	short index;
	short salt;
};

static inline bool datum_is_valid(s_datum datum)
{
	return datum.salt != NONE;
}

static inline byte real_to_byte(real x)
{
	dword v = real_truncate(x);
	if (v > 0xFF)
	{
		return 0xFF;
	}
	return (byte)v;
}

/* the magnitude of a real by clearing its sign bit (retail compares with
   `and [esp+N], 0x7fffffff` here, not with fabs) */
static inline real real_sign_cleared(real x)
{
	*(dword *)&x &= 0x7fffffff;
	return x;
}

static inline signed char real_to_signed_byte(real x)
{
	long v = real_truncate(x);
	if (v < -128)
	{
		v = -128;
	}
	else if (v > 127)
	{
		v = 127;
	}
	return (signed char)v;
}

struct s_vibration_state
{
	byte flags;
	byte unknown01;
	byte countdown;
	byte unknown03;
	byte unknown04[0x32];
	short s36;
	long d38;
	byte unknown3c[4];
	real r40;
	byte unknown44[0xc];
	signed char c50[12];
	s_datum d5c;
	real r60;
	real r64;
	real r68;
	short s6c;
	byte b6e[13];
};

struct s_object
{
	dword tag_index;
	byte unknown04[0x126];
	short offset12a;
	byte unknown12c[0x212];
	short offset33e;
};

struct s_object_header
{
	byte unknown00[8];
	s_object *object;
};

#define OBJECT_FROM_INDEX(index) (((s_object_header *)g_4e0300->data)[(index) & 0xFFFF].object)
#define VIBRATION_STATE(object) ((s_vibration_state *)((byte *)(object) + (object)->offset33e))

static inline s_vibration_state *vibration_state(long object_index)
{
	return VIBRATION_STATE(OBJECT_FROM_INDEX(object_index));
}

// @retail 0x10dc70
bool function_10dc70(long object_index)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);
	s_vibration_state *state = VIBRATION_STATE(object);
	s_tag_data_1e8 *tag = (s_tag_data_1e8 *)g_4e3b44[object->tag_index & 0xFFFF].bytes;
	bool result = false;

	if (vibration_state(object_index)->d38 != NONE)
	{
		if (state->r40 < 0.0001f)
		{
			real value = state->r40 + 0.000033f;
			if (!(value > 0.0f))
			{
				value = 0.0f;
			}
			state->r40 = value;
		}
		else
		{
			real value = state->r40 + 0.15f;
			if (value > 1.0f)
			{
				value = 1.0f;
			}
			state->r40 = value;
		}
		result = true;
	}
	else
	{
		real value = state->r40 - 0.075f;
		if (!(value > 0.0f))
		{
			value = 0.0f;
		}
		state->r40 = value;
		for (long i = 0; i < 12; i++)
		{
			real scale = 0.9f;
			if (tag->value > 0.0f)
			{
				scale = tag->value;
			}
			signed char v = real_to_signed_byte((real)state->c50[i] * g_547648 * scale * 128.0f);
			state->c50[i] = v;
			if (v)
			{
				result = true;
			}
		}
		if (vibration_state(object_index)->s36 != NONE)
		{
			if (state->r40 != 0.0f)
			{
				state->countdown = 0xFF;
			}
			else if (!state->countdown)
			{
				g_4e7408->seed = g_4e7408->seed * 0x19660d + 0x3c6ef35f;
				state->countdown = (byte)(((g_4e7408->seed >> 16) * 225 >> 16) + 30);
			}
			else
			{
				state->countdown--;
			}
		}
	}
	return result;
}

struct s_vibration_channel
{
	short key_count;
	short key_offset;
};

struct s_vibration_key
{
	short time;
	short value;
};

struct s_vibration_curves
{
	short channel_count;
	short version;
	byte unknown04[8];
	real time_scale;
	s_vibration_channel channels[1];
};

struct s_vibration_curve_set
{
	dword size;
	s_vibration_curves *curves;
};

struct s_vibration_output
{
	byte unknown00[0xc];
	real time;
	byte unknown10[4];
	signed char indices[12];
	signed char values[12];
};

inline real function_10e8e0(real t);

#define VIBRATION_VERSION 0x96c

/* a key's time and value, converted only for the current curve version */
static inline void vibration_key_read(s_vibration_curves const *curves, s_vibration_key const *key, real *time, real *value)
{
	if (curves->version == VIBRATION_VERSION)
	{
		*time = (real)key->time * curves->time_scale;
		*value = (real)key->value * 0.0001f;
	}
}

/* blends each channel's curve value at the time into the previous output and keeps the twelve largest */
// @retail 0x10de40
void function_10de40(s_vibration_curve_set *set, s_vibration_output *output, real time, real blend, bool skip_special)
{
	output->time = time;
	if (set->size > 0x10)
	{
		s_vibration_curves *curves = set->curves;
		if (curves && curves->version == VIBRATION_VERSION)
		{
			long used = 0;
			real previous[24];
			memset(previous, 0, sizeof(previous));
			for (long i = 0; i < 12; i++)
			{
				long index = output->indices[i];
				if (index >= 0 && index < curves->channel_count && index < 24)
				{
					previous[index] = (real)output->values[i] * g_547648;
				}
			}

			real out_values[12];
			long out_indices[12];
			memset(out_indices, 0, sizeof(out_indices));
			memset(out_values, 0, sizeof(out_values));
			memset(output->indices, 0, sizeof(output->indices));
			memset(output->values, 0, sizeof(output->values));

			long channel_count = curves->channel_count;
			s_vibration_channel *channels = curves->channels;
			s_vibration_key *keys_base = (s_vibration_key *)(channels + channel_count);
			real t_first, v_first, t_last, v_last, t_key, t0, v0, t1, v1;
			for (long c = 0; c < channel_count; c++)
			{
				s_vibration_channel *channel = &channels[c];
				real *prior = &previous[c];
				s_vibration_key *keys = keys_base + channel->key_offset;
				real result = 0.0f;
				bool skip = skip_special && (c == 0x12 || c == 0x13 || c == 0x14);
				short key_count = channel->key_count;
				if (key_count > 0 && !skip)
				{
					s_vibration_key *last = key_count > 1 ? keys + key_count - 1 : keys;
					vibration_key_read(curves, keys, &t_first, &v_first);
					vibration_key_read(curves, last, &t_last, &v_last);
					if (t_first >= time || key_count == 1)
					{
						result = v_first;
					}
					else if (time >= t_last)
					{
						result = v_last;
					}
					else
					{
						long prev = NONE;
						s_vibration_key *key = keys;
						for (long k = 0; k < key_count; k++)
						{
							if (curves->version == VIBRATION_VERSION)
							{
								t_key = (real)key->time * curves->time_scale;
							}
							key++;
							if (t_key > time)
							{
								prev = k - 1;
								break;
							}
						}
						if (prev >= 0)
						{
							if (curves->version == VIBRATION_VERSION)
							{
								t0 = (real)keys[prev].time * curves->time_scale;
								v0 = (real)keys[prev].value * 0.0001f;
								t1 = (real)keys[prev + 1].time * curves->time_scale;
								v1 = (real)keys[prev + 1].value * 0.0001f;
							}
							if (t1 - t0 > 0.0001f)
							{
								real fraction = (time - t0) / (t1 - t0);
								real smooth = function_10e8e0(fraction);
								result = (v1 - v0) * smooth + v0;
								if (fabs(result) < 0.0001f)
								{
									result = 0.0f;
								}
							}
							else
							{
								result = v0;
							}
						}
					}

					result = (result - *prior) * blend + *prior;
					real magnitude = (real)fabs(result);
					if (!(magnitude < 0.0001f))
					{
						if (used < 12)
						{
							out_values[used] = result;
							out_indices[used] = c;
							used++;
						}
						else
						{
							real smallest = 3.4028235e38f;
							long smallest_index = 0;
							for (long i = 0; i < 12; i++)
							{
								if (smallest > real_sign_cleared(out_values[i]))
								{
									smallest = (real)fabs(out_values[i]);
									smallest_index = i;
								}
							}
							if (magnitude > smallest)
							{
								out_values[smallest_index] = result;
								out_indices[smallest_index] = c;
							}
						}
					}
				}
			}

			if (used > 0)
			{
				for (long i = 0; i < used; i++)
				{
					output->indices[i] = (signed char)out_indices[i];
					output->values[i] = real_to_signed_byte(out_values[i] * 128.0f);
				}
			}
		}
	}
}

// @retail 0x10e8e0
inline real function_10e8e0(real t)
{
	real result = t * t * 3.0f - t * t * t * 2.0f;

	return result < 0.0f ? 0.0f : result > 1.0f ? 1.0f : result;
}

// @retail 0x10e9f0
void function_10e9f0(long object_index, short channel, real value, real time)
{
	s_vibration_state *state = VIBRATION_STATE(OBJECT_FROM_INDEX(object_index));
	if (datum_is_valid(state->d5c))
	{
		if (channel != NONE && channel < 13)
		{
			state->s6c = channel;
			state->r68 = value;
		}
		else
		{
			state->s6c = NONE;
			state->r68 = 0.0f;
		}

		if (time > 0.0f)
		{
			state->r60 = 1.0f / ((real)g_510c54->field_2_3 * time);
			state->r64 = 0.0f;
		}
		else
		{
			time = 0.0f;
			for (long i = 0; i < 13; i++)
			{
				state->b6e[i] = real_to_byte(time);
			}
			time = state->r68 * 128.0f;
			state->b6e[channel] = real_to_byte(time);
			state->r60 = 0.0f;
			state->r64 = 0.0f;
		}
	}
}

// @retail 0x10eaf0
bool function_10eaf0(long object_index)
{
	s_vibration_state *state = VIBRATION_STATE(OBJECT_FROM_INDEX(object_index));
	bool result = false;

	if (state->r60 > 0.0f)
	{
		state->r64 += 1.0f;
		real blend = state->r60 * state->r64;
		if (0.0f > blend)
		{
			blend = 0.0f;
		}
		else if (blend > 1.0f)
		{
			blend = 1.0f;
		}

		real inverse = 1.0f - blend;
		for (long i = 0; i < 13; i++)
		{
			real v = (real)state->b6e[i] * g_54764c;
			if (i == state->s6c)
			{
				v = (state->r68 - v) * blend + v;
			}
			else
			{
				v = v * inverse;
			}
			state->b6e[i] = real_to_byte(v * 128.0f);
		}

		if (fabs(blend - 1.0f) < 0.0001f)
		{
			state->r60 = 0.0f;
			state->r64 = 0.0f;
		}
		result = true;
	}
	return result;
}

// @retail 0x10edd0
bool function_10edd0(long object_index)
{
	s_vibration_state *state = VIBRATION_STATE(OBJECT_FROM_INDEX(object_index));
	bool result = false;

	if (datum_is_valid(state->d5c))
	{
		long i = 0;
		while (!result)
		{
			if (i >= 13)
			{
				break;
			}
			if (state->b6e[i])
			{
				result = true;
			}
			i++;
		}
	}
	return result;
}

struct s_bit_layout
{
	signed char b0;
	byte unknown01[5];
	short w6;
	byte unknown08[4];
	long d0c;
};

struct s_bit_owner
{
	byte *data;
	s_bit_layout *layout;
	byte bit_count;
};

// @retail 0x10ee70
byte *function_10ee70(s_bit_owner *owner)
{
	s_bit_layout *layout = owner->layout;
	return owner->data + (((owner->bit_count + 0x1F) >> 3) & ~3) + layout->w6 + layout->d0c + layout->b0;
}

// @retail 0x10eea0
bool function_10eea0(s_bit_owner *owner, short bit)
{
	s_bit_layout *layout = owner->layout;
	long offset = (((owner->bit_count + 0x1F) >> 3) & ~3) + (bit >> 5) * 4 + layout->w6 + layout->d0c + layout->b0;
	return (*(dword *)(owner->data + offset) & (1 << (bit & 0x1F))) != 0;
}

struct s_trigger_state
{
	long unknown00;
	short unknown04;
	short unknown06;
	long unknown08;
	char unknown0c;
	char unknown0d;
	short unknown0e;
	byte unknown10;
	byte unknown11;
	struct
	{
		word flag0 : 1;
	} flags12;
	short unknown14;
	short unknown16;
	real unknown18;
	real unknown1c;
};

struct s_short_pair
{
	short a;
	short b;
};

struct s_weapon_state
{
	struct
	{
		byte flag0 : 1;
		byte flag1 : 1;
	} flags;
	byte unknown01[2];
	byte unknown03;
	byte unknown04[0x90];
	long unknown94;
	long unknown98;
	s_trigger_state triggers[3];
	s_short_pair pairs0fc[7];
	s_short_pair pairs118[3];
};

#define WEAPON_STATE(object) ((s_weapon_state *)((byte *)(object) + (object)->offset33e))

static inline void trigger_head_initialize(s_trigger_state *trigger)
{
	trigger->unknown00 = NONE;
	trigger->unknown04 = NONE;
	trigger->unknown06 = NONE;
}

static inline void trigger_body_initialize(s_trigger_state *trigger)
{
	trigger->unknown1c = 0.0f;
	trigger->unknown18 = 1.0f;
	trigger->unknown08 = NONE;
	trigger->unknown0c = NONE;
	trigger->unknown0d = NONE;
	trigger->unknown0e = NONE;
	trigger->unknown10 = 0;
	trigger->unknown11 = 0;
	*(word *)&trigger->flags12 = 0;
	trigger->unknown14 = 0;
	trigger->unknown16 = 0;
}

// @retail 0x10f040
void function_10f040(long object_index)
{
	s_weapon_state *state = WEAPON_STATE(OBJECT_FROM_INDEX(object_index));

	state->unknown98 = NONE;
	state->unknown94 = NONE;
	trigger_head_initialize(&state->triggers[0]);
	trigger_body_initialize(&state->triggers[0]);
	trigger_head_initialize(&state->triggers[1]);
	trigger_body_initialize(&state->triggers[1]);
	trigger_head_initialize(&state->triggers[2]);
	trigger_body_initialize(&state->triggers[2]);
	state->unknown03 = 0;
	s_short_pair *pair = &state->pairs0fc[0];
	for (long i = 0; i < 7; i++)
	{
		pair->a = NONE;
		pair->b = NONE;
		pair++;
	}
	pair = &state->pairs118[0];
	long count = 3;
	do
	{
		pair->a = NONE;
		pair->b = NONE;
		pair++;
		count--;
	}
	while (count);
	state->flags.flag1 = 0;
}
struct s_weapon_handles
{
	long unknown00;
	short unknown04;
	short unknown06;
	byte unknown08[0x60];
	long unknown68;
	byte unknown6c[4];
	long unknown70;
	byte unknown74[8];
	long unknown7c;
};

#define WEAPON_HANDLES(object) ((s_weapon_handles *)((byte *)(object) + (object)->offset12a))

// @retail 0x10f5f0
long function_10f5f0(long object_index)
{
	s_weapon_handles *handles = WEAPON_HANDLES(OBJECT_FROM_INDEX(object_index));
	long result = NONE;

	if (handles->unknown68 != NONE && handles->unknown00 != NONE && handles->unknown06 != NONE)
	{
		result = handles->unknown7c;
	}
	return result;
}

// @retail 0x10f630
bool function_10f630(long object_index, long *first, long *second)
{
	s_weapon_handles *handles = WEAPON_HANDLES(OBJECT_FROM_INDEX(object_index));
	bool result = false;

	if (handles->unknown68 != NONE && handles->unknown00 != NONE && handles->unknown06 != NONE)
	{
		*first = handles->unknown70;
		*second = handles->unknown7c;
		result = true;
	}
	else
	{
		*first = NONE;
		*second = NONE;
	}
	return result;
}

// @retail 0x10f820
bool function_10f820(long object_index)
{
	s_weapon_state *state = WEAPON_STATE(OBJECT_FROM_INDEX(object_index));
	s_trigger_state *trigger0 = &state->triggers[0];
	s_trigger_state *trigger1 = &state->triggers[1];
	bool result = false;

	if ((trigger0->unknown00 != NONE && trigger0->unknown06 != NONE && TEST_FIELD_BIT(trigger0->flags12.flag0) && !(trigger0->unknown11 & 9)) ||
		(trigger1->unknown00 != NONE && trigger1->unknown06 != NONE && TEST_FIELD_BIT(trigger1->flags12.flag0) && !(trigger1->unknown11 & 9)))
	{
		result = true;
	}
	return result;
}
