/* DATA_ARRAY.H: the engine's data array (handle-addressed element pool;
   src/unknown_16b570.cpp). This is the one data-array type: g_4cf78c,
   g_4e8c24, g_4f55f0, g_502418, g_50241c, g_502420 and the object header data
   g_4e0300 are all views of it.

   A data array is a 0x4c byte header, followed (when built in place) by the
   element storage and an occupancy bitmap. A datum index is a 16-bit salt in
   the high half and the absolute slot index in the low half; every element
   starts with its own 16-bit salt (0 = free). */

#ifndef DATA_ARRAY_H
#define DATA_ARRAY_H

#include "unknown_11c920.h"

#define DATA_ARRAY_SIGNATURE 0x64407440

/* the memory allocator a data array was built through (vtable at [0]) */
class c_data_allocator
{
public:
	virtual void *allocate(long size) { return 0; }
	virtual void deallocate(void *block) { }
};

struct s_record_pool
{
	char name[0x20];
	long maximum_count;
	long size;
	byte alignment_bits;
	byte valid;
	word flag0 : 1;
	word flag1 : 1;
	word allocated : 1;
	word initialize_to_bad : 1;
	word unknown2a : 12;
	dword signature;
	c_data_allocator *allocator;
	long first_free_index;
	long high_water_index;
	long actual_count;
	short next_salt;
	word unknown42;
	byte *data;
	dword *bitmap;
};

struct s_record_pool_iterator
{
	s_record_pool *data;
	long datum_index;
	long index;
};

/* a datum index is a salt in the high half and the absolute index in the low half */
s_record_pool *data_new(const char *name, long maximum_count, long size, long alignment_bits, c_data_allocator *allocator);
void data_dispose(s_record_pool *data);
void function_16b5f0(s_record_pool *data, const char *name, long maximum_count, long size, long alignment_bits, c_data_allocator *allocator);
void data_array_construct(s_record_pool *data, const char *name, long maximum_count, long size, long alignment_bits, c_data_allocator *allocator, dword *bitmap);
void function_16b6b0(s_record_pool *data, long maximum_count, byte *elements);
void function_16b790(s_record_pool *data);
void record_pool_release_all(s_record_pool *data);
long record_pool_allocate(s_record_pool *data);
long datum_new_at_index_with_salt(s_record_pool *data, long datum_index);
long function_16b990(s_record_pool *data, long index);
void function_16ba00(s_record_pool *data, byte *datum);
void record_pool_release(s_record_pool *data, long datum_index);
byte *record_pool_lookup(s_record_pool *data, long datum_index);
byte *datum_get_absolute(s_record_pool *data, long index);
long index_to_datum_index(s_record_pool *data, long index);
byte *record_pool_iterator_step(s_record_pool_iterator *iterator);

/* an iteration over a data array that keeps a pointer to the current datum
   (data_iterator.cpp) */
struct s_data_datum_iterator
{
	byte *datum;
	s_record_pool *data;
	long datum_index;
	long index;
};

inline __declspec(noinline) bool data_datum_iterator_next(s_data_datum_iterator *iterator);
long record_pool_next_used(s_record_pool *data, long datum_index);
long function_16bc00(s_record_pool *data, long index);

/* retail inlines data_new into its callers via LTCG; the out-of-line original
   in unknown_16b570.cpp needs /Ob1, which stops LTCG inlining it, so callers
   that retail inlines it into use this copy. (data_dispose is already
   inlinable: it lives in its own file, unknown_16b5d0.cpp.) */
static inline s_record_pool *data_new_inlined(const char *name, long maximum_count, long size, long alignment_bits, c_data_allocator *allocator)
{
	long bitmap_size = ((maximum_count + 31) >> 5) * 4;
	s_record_pool *data = (s_record_pool *)allocator->allocate(sizeof(s_record_pool) + maximum_count * size + bitmap_size + (1 << alignment_bits) - 1);

	if (data)
	{
		function_16b5f0(data, name, maximum_count, size, alignment_bits, allocator);
		data->allocated = 1;
	}
	return data;
}
/* likewise function_16bc00 and record_pool_iterator_step
   (unknown_16b570.cpp), which retail inlines into callers built /Ob1 (ai.cpp,
   unknown_1cec30.cpp) */
static inline long data_next_absolute_index_inlined(s_record_pool *data, long index)
{
	long result = NONE;

	if (index >= 0)
	{
		for (; index < data->high_water_index; index++)
		{
			if (data->bitmap[index >> 5] & (1 << (index & 0x1f)))
			{
				result = index;
				break;
			}
		}
	}

	return result;
}

static inline byte *data_iterator_next_inlined(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = data_next_absolute_index_inlined(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	return result;
}

/* likewise record_pool_iterator_step itself, which retail inlines into callers that
   still call function_16bc00 */
static inline byte *data_iterator_next_calling(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	return result;
}

/* likewise record_pool_lookup, which retail inlines into callers such as
   unknown_26e370.cpp's */
static inline byte *datum_get_inlined(s_record_pool *data, long datum_index)
{
	byte *result = 0;

	if (datum_index != NONE)
	{
		long index = datum_index & 0xffff;

		if (index < data->high_water_index)
		{
			byte *datum = data->data + data->size * index;
			short salt = *(short *)datum;

			if (salt != 0 && salt == (datum_index >> 16))
			{
				result = datum;
			}
		}
	}

	return result;
}

/* likewise datum_get_absolute (the datum at an absolute index, or NULL when
   it is free), which retail inlines into callers such as unknown_1e9700.cpp's */
static inline byte *datum_get_absolute_inlined(s_record_pool *data, long index)
{
	byte *result = 0;

	if (index != NONE && index >= 0 && index < data->high_water_index)
	{
		byte *datum = data->data + data->size * index;

		if (*(short *)datum != 0)
		{
			result = datum;
		}
	}

	return result;
}

/* the inline copies of the datum index lookup and of the next used index
   search (retail also calls 0x16bc00) that LTCG places in callers */
static inline long data_datum_index(s_record_pool *array, long index)
{
	long datum = NONE;
	if (index != NONE)
		datum = (((short *)(array->data + array->size * index))[0] << 16) | index;
	return datum;
}

static inline long data_find_index(s_record_pool *array, long index)
{
	long result = NONE;
	if (index >= 0 && index < array->high_water_index)
	{
		do
		{
			if (array->bitmap[index >> 5] & (1 << (index & 0x1f)))
			{
				result = index;
				break;
			}
			index++;
		} while (index < array->high_water_index);
	}
	return result;
}


#endif