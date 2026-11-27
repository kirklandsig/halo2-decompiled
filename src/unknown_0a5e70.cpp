// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A5E70.CPP: object state synchronization (compares an object's state
   with the previous state and returns the mask of what changed) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>

#define k_real_epsilon 0.0001f

/* the position bounds in the match globals (g_4e0348) */
struct s_position_bounds
{
	byte unknown00[0x34];
	real x_min;
	real x_max;
	real y_min;
	real y_max;
	real z_min;
	real z_max;
};

/* the previous state the object is compared with */
struct s_object_state
{
	point3f position;
	vector3f forward;
	vector3f up;
	real scalar;
	vector3f vector_a;
	vector3f vector_b;
	real bounded_a;
	bool bounded_a_flag;
	byte unknown45[3];
	real bounded_b;
	bool bounded_b_flag;
	bool flag;
	byte unknown4e[2];
	byte count;
	byte bytes[16];
	byte tag_value;
	word words_a;
	word words_b;
};

struct s_object_a5e70
{
	dword tag_index;
	byte unknown04[0x10];
	long parent_index;
	byte unknown18[0x4c];
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f vector_a;
	vector3f vector_b;
	real scalar;
	byte unknownac[0x3c];
	word words_a;
	word words_b;
	byte unknowne4[8];
	real bounded_a;
	real bounded_b;
	byte unknownf4[0x10];
	short selector_b;
	short selector_a;
	byte unknown108[2];
	byte flags10a;
	byte unknown10b[0xd];
	short count118;
	short offset11a;
};

struct s_tag_definition_a5e70
{
	byte unknown00[0x38];
	long child_tag_index;
};

struct s_child_definition_a5e70
{
	byte unknown00[0x60];
	long count;
	byte *entries;
};

#define OBJECT(index) (*(s_object_a5e70 **)((byte *)g_4e0300->data + 8 + ((index) & 0xffff) * 12))

#define PIN_REAL(value, minimum, maximum) ((minimum) > (value) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

static void scale_vector(vector3f *v, real limit_squared, real limit)
{
	real length_squared = v->k * v->k + v->j * v->j + v->i * v->i;
	if (length_squared > limit_squared)
	{
		real scale = limit / (real)sqrt(length_squared);
		v->i = v->i * scale;
		v->j = v->j * scale;
		v->k = v->k * scale;
	}
}

class c_object_type_definition;

#define CLOSE(a, b) (fabs((a) - (b)) < k_real_epsilon)

/* the definition is not read here; each of the five callers (the object types' v27) stores its `this`
   to a stack slot ahead of the call, the way the callers of 0xa4950 store theirs */
// @retail 0xa5e70
long function_a5e70(long object_index, long flags, long state_pointer, c_object_type_definition const *definition)
{
	s_object_state *state = (s_object_state *)state_pointer;
	s_object_a5e70 *object = OBJECT(object_index);
	long changed = 0;

	if (flags & 0x1)
	{
		bool value = (object->flags10a >> 2) & 1;
		if (state->flag != value)
		{
			state->flag = value;
			changed = 1;
		}
	}

	if (object->parent_index == NONE)
	{
		if (flags & 0x2)
		{
			s_position_bounds *bounds = (s_position_bounds *)g_4e0348;
			point3f position;
			position.x = PIN_REAL(object->position.x, bounds->x_min, bounds->x_max);
			position.y = PIN_REAL(object->position.y, bounds->y_min, bounds->y_max);
			position.z = PIN_REAL(object->position.z, bounds->z_min, bounds->z_max);
			if (!(CLOSE(state->position.x, position.x) && CLOSE(state->position.y, position.y) && CLOSE(state->position.z, position.z)))
			{
				state->position = position;
				changed |= 2;
			}
		}

		if (flags & 0x4)
		{
			if (!(CLOSE(state->forward.i, object->forward.i) && CLOSE(state->forward.j, object->forward.j) && CLOSE(state->forward.k, object->forward.k)
				&& CLOSE(state->up.i, object->up.i) && CLOSE(state->up.j, object->up.j) && CLOSE(state->up.k, object->up.k)))
			{
				state->forward = object->forward;
				state->up = object->up;
				changed |= 4;
			}
		}
	}

	if (flags & 0x8)
	{
		real scalar = PIN_REAL(object->scalar, 0.f, 10.f);
		if (state->scalar != scalar)
		{
			state->scalar = scalar;
			changed |= 8;
		}
	}

	if (flags & 0x10)
	{
		vector3f vector = object->vector_a;
		scale_vector(&vector, 122500.f, 350.f);
		if (!(CLOSE(state->vector_a.i, vector.i) && CLOSE(state->vector_a.j, vector.j) && CLOSE(state->vector_a.k, vector.k)))
		{
			state->vector_a = vector;
			changed |= 0x10;
		}
	}

	if (flags & 0x20)
	{
		vector3f vector = object->vector_b;
		scale_vector(&vector, 900.f, 30.f);
		if (!(CLOSE(state->vector_b.i, vector.i) && CLOSE(state->vector_b.j, vector.j) && CLOSE(state->vector_b.k, vector.k)))
		{
			state->vector_b = vector;
			changed |= 0x20;
		}
	}

	if (flags & 0x40)
	{
		real bounded = PIN_REAL(object->bounded_a, -1.f, 1.f);
		bool selector = object->selector_a == 0;
		if (state->bounded_a != bounded || state->bounded_a_flag != selector)
		{
			state->bounded_a = bounded;
			state->bounded_a_flag = selector;
			changed |= 0x40;
		}
	}

	if (flags & 0x80)
	{
		real bounded = PIN_REAL(object->bounded_b, 0.f, 3.f);
		bool selector = object->selector_b == 0;
		if (state->bounded_b != bounded || state->bounded_b_flag != selector)
		{
			state->bounded_b = bounded;
			state->bounded_b_flag = selector;
			changed |= 0x80;
		}
	}

	if (flags & 0x100)
	{
		s_object_a5e70 *current = OBJECT(object_index);
		signed char *entry = (signed char *)current + *(short volatile *)&current->offset11a;
		long count = *(short volatile *)&current->count118 / 10;
		if (state->count == count && count > 0)
		{
			byte *bytes = state->bytes;
			entry += count * 2 + 1;
			for (long n = count; n > 0; n--)
			{
				if (*bytes != *entry)
				{
					*bytes = *entry;
					changed |= 0x100;
				}
				entry += 8;
				bytes++;
			}
		}
	}

	if (flags & 0x200)
	{
		s_tag_definition_a5e70 *definition = (s_tag_definition_a5e70 *)g_4e3b44[object->tag_index & 0xffff].bytes;
		long value = 0;
		if (definition->child_tag_index != NONE)
		{
			s_child_definition_a5e70 *child = (s_child_definition_a5e70 *)g_4e3b44[definition->child_tag_index & 0xffff].bytes;
			if (child->count > 0)
			{
				value = *(long *)(child->entries + 0xe0);
			}
		}

		if (state->tag_value == value)
		{
			word state_words = state->words_a;
			word object_words = object->words_a;
			if (state_words != object_words || state->words_b != object->words_b)
			{
				state->words_a = object_words | state_words;
				state->words_b |= object->words_b;
				changed |= 0x200;
			}
		}
	}

	return changed;
}

byte simulation_positions_close(long bits, real const *a, real const *b);
bool function_195560(vector3f const *up_a, vector3f const *a, vector3f const *up_b, vector3f const *b);
bool __fastcall function_195370(vector3f const *a, vector3f const *b, real lo, real hi, long bits);

// @retail 0xa7180
bool function_a7180(long state_a, long state_b)
{
    s_object_state *a = (s_object_state *)state_a;
    s_object_state *b = (s_object_state *)state_b;
    bool result = simulation_positions_close(16, (real const *)&a->position, (real const *)&b->position);
    b->position = *g_468788;
    a->position = b->position;
    result = result && function_195560(&a->forward, &a->up, &b->forward, &b->up);
    b->forward = *g_4687a4;
    a->forward = *g_4687a4;
    b->up = *g_4687a4;
    a->up = *g_4687a4;
    result = result && fabs(b->scalar - a->scalar) < 0.07874015718698502f;
    b->scalar = 0.0f;
    a->scalar = 0.0f;
    result = result && function_195370(&a->vector_a, &b->vector_a, 0.03f, 350.0f, 10);
    b->vector_a = *g_4687a4;
    a->vector_a = *g_4687a4;
    result = result && function_195370(&a->vector_b, &b->vector_b, 0.03f, 30.0f, 8);
    b->vector_b = *g_4687a4;
    a->vector_b = *g_4687a4;
    result = result && fabs(b->bounded_a - a->bounded_a) < 0.007874015718698502f;
    b->bounded_a = 0.0f;
    a->bounded_a = 0.0f;
    result = result && fabs(b->bounded_b - a->bounded_b) < 0.011811023578047752f;
    b->bounded_b = 0.0f;
    a->bounded_b = 0.0f;
    memset((byte *)a + 0x68, 0, 0x28);
    memset((byte *)b + 0x68, 0, 0x28);
    return result;
}
