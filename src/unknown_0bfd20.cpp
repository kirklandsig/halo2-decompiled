// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "object_iterator.h"

struct s_object_hdr
{
	dword tag_index;
	byte unknown04[0xa0];
	dword key;
	short subkey;
	byte byte_aa;
	byte byte_ab;
	byte unknownac[7];
	byte awake_count;
	byte unknownb4[0x1c];
	short match_index;
	byte unknownd2[0xf0 - 0xd2];
	real shield;
	byte unknownf4[0x104 - 0xf4];
	short value104;
};

/* the entries of the object header array (((s_object_header *)g_4e0300->data)) */
struct s_object_header
{
	byte unknown00[8];
	s_object_hdr *object;
};

/* the tag flags (g_4e3b44's flags pointer); only the bit tested here (108fd0
   has the fuller view) */
struct s_tag_flags
{
	byte unknown00[3];
	unsigned char flag0 : 1;
};

real g_547634;
real g_547638;

// @retail 0xbfd20
void function_0bfd20(long object_index)
{
	s_object_header *headers = (s_object_header *)g_4e0300->data;
	s_match_globals *globals = g_4e0348;
	long header_index = object_index & 0xffff;
	s_object_hdr *object = headers[header_index].object;
	if (TEST_FIELD_BIT(g_4e3b44[(object->tag_index & 0xffff)].flags->flag0))
	{
		for (long i = 0; i < globals->count; i++)
		{
			s_match_entry const *entry = &globals->match_entries[i];
			bool match = true;
			match &= entry->byte_06 == object->byte_aa;
			match &= entry->byte_07 == object->byte_ab;
			match &= entry->key == object->key;
			if (match && entry->byte_07 == 0)
				match &= entry->subkey == object->subkey;
			if (match)
			{
				object->match_index = (short)i;
				return;
			}
		}
		object->match_index = NONE;
	}
	else
	{
		object->match_index = NONE;
	}
}

// @retail 0xbfe20
inline void function_0bfe20(dword *flags, long bit, bool value)
{
	if (value)
		*flags |= 1 << bit;
	else
		*flags &= ~(1 << bit);
}

// @retail 0xbfe40
void function_0bfe40(word *flags, long bit, bool value)
{
	bool const *value_reference = &value;
	if (*value_reference)
		*flags |= (word)(1 << bit);
	else
		*flags &= (word)~(1 << bit);
}

/* retail never inlines this bit test */
__declspec(noinline) bool function_0bfe60(const dword *flags, long bit);

// @retail 0xbfe60
inline bool function_0bfe60(const dword *flags, long bit)
{
	return (flags[bit >> 5] & (1 << (bit & 31))) != 0;
}

struct s_bit_vector
{
	dword bits[8];

	__declspec(noinline) void function_0bfe80();
	__declspec(noinline) void function_0bfed0(long count);
	bool is_empty() const;
};

/* inverts the bits, keeping only the first 8 bits of the last word */
// @retail 0xbfe80
inline void s_bit_vector::function_0bfe80()
{
	for (long i = 0; i < 8; i++)
		bits[i] = ~bits[i];
	bits[7] &= 0xff;
}

/* clears every bit from count on */
// @retail 0xbfed0
inline void s_bit_vector::function_0bfed0(long count)
{
	long last = ((count + 31) >> 5) - 1;
	for (long i = last + 1; i < 8; i++)
		bits[i] = 0;
	long remainder = count & 31;
	dword mask = 0xffffffff;
	if (remainder > 0)
		mask >>= 32 - remainder;
	bits[last] &= mask;
}

// @retail 0xbff10
bool s_bit_vector::is_empty() const
{
	bool result = true;
	result &= bits[7] == 0;
	result &= (bits[6] == 0);
	result &= (bits[5] == 0);
	result &= (bits[4] == 0);
	result &= (bits[3] == 0);
	result &= (bits[2] == 0);
	result &= (bits[1] == 0);
	result &= (bits[0] == 0);
	return result;
}

// @retail 0xbff60
inline real function_0bff60(real a, real b)
{
	real d = a - b;
	if (d >= g_547638)
		d -= g_547634;
	if (-g_547638 >= d)
		d = g_547634 + d;
	return d;
}

// @retail 0xbfc90
bool function_bfc90(long object_index)
{
	bool result = false;
	if (object_index != NONE)
	{
		if (((s_object_header *)g_4e0300->data)[object_index & 0xffff].object->awake_count > 0)
			result = true;
	}
	return result;
}

static __forceinline void object_short_set_ab(long object_index, short value)
{
    long clamped = value;
    s_record_pool *objects = g_4e0300;
    s_object_hdr *object = ((s_object_header *)objects->data)[object_index & 0xffff].object;
    if (clamped < 0) clamped = 0;
    else if (clamped > 0x7ffe) clamped = 0x7ffe;
    object->value104 = (short)clamped;
}

// @retail 0xbf830
void function_bf830(long object_index, real shield, short value)
{
    s_record_pool *objects = g_4e0300;
    ((s_object_header *)objects->data)[object_index & 0xffff].object->shield = shield;
    object_short_set_ab(object_index, value);
}

// @retail 0xbfcc0
void function_bfcc0(void)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	function_bae80(&state.iterator, 0, 0);
	while ((state.object = function_baeb0(&state.iterator)) != 0)
		function_0bfd20(state.iterator.object_index);
}


struct s_object_model_definition_ab
{
    byte unknown00[0x38];
    long model_index;
};
struct s_model_pair_ab
{
    byte unknown00[4];
    long render_index;
    byte unknown08[4];
    long animation_index;
};
struct s_render_blocks_ab
{
    byte unknown00[0x74];
    long count_a;
    byte unknown78[4];
    long count_b;
};

// @retail 0xbf9a0
bool function_bf9a0(long object_index, long *render_index, long *animation_index)
{
    bool result = false;
    s_object_hdr *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    s_object_model_definition_ab *definition = (s_object_model_definition_ab *)g_4e3b44[object->tag_index & 0xffff].bytes;
    if (definition->model_index != NONE)
    {
        s_model_pair_ab *model = (s_model_pair_ab *)g_4e3b44[definition->model_index & 0xffff].bytes;
        long render = model->render_index;
        if (render != NONE && model->animation_index != NONE)
        {
            s_render_blocks_ab *data = (s_render_blocks_ab *)g_4e3b44[render & 0xffff].bytes;
            result = data->count_a > 0 && data->count_b > 0;
            *render_index = render;
            *animation_index = model->animation_index;
        }
    }
    return result;
}
