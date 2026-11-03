/* DATA_ITERATOR.H: the data iterator that remembers its current datum
   (0x6b380, src/data_iterator.cpp), as retail inlines it into its callers:
   with the next-index scan inlined too (0x1406b0, 0xc1720), and calling the
   scan (0x140b80) */

#ifndef DATA_ITERATOR_H
#define DATA_ITERATOR_H

#include "data_array.h"

/* makes the datum at an absolute index the iterator's current datum */
static inline void data_datum_iterator_set_current(s_data_datum_iterator *iterator, s_record_pool *data, long index)
{
	iterator->datum = data->data + data->size * index;
	iterator->index = index;
	iterator->datum_index = (*(short *)iterator->datum << 16) | index;
}

static inline bool data_datum_iterator_next_inlined(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = data_next_absolute_index_inlined(data, iterator->index + 1);
	byte *datum;

	if (index != NONE)
	{
		datum = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)datum << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		datum = 0;
	}
	iterator->datum = datum;
	return datum != 0;
}

static inline bool data_datum_iterator_next_called(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);

	if (index != NONE)
	{
		data_datum_iterator_set_current(iterator, data, index);
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		iterator->datum = 0;
	}
	return iterator->datum != 0;
}

#endif
