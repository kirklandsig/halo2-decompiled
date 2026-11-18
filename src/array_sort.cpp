// @flags /O2 /arch:SSE /Gr
/* ARRAY_SORT.CPP: quicksort and binary search over arrays (after the C runtime's
   qsort), with specialised versions for 4-byte and 2-byte elements */

#include "unknown_11c920.h"

typedef bool (__stdcall *t_compare_function)(const void *, const void *, const void *);
typedef long (__stdcall *t_bsearch_compare_function)(const void *, const void *, const void *);
typedef long (__stdcall *t_bsearch_4byte_compare_function)(long, long, const void *);

enum
{
	k_sort_insertion_cutoff = 8,
	k_sort_stack_size = 30
};

#define SWAP_ELEMENTS(element_a, element_b, element_size, swap_dwords) \
{ \
	if (swap_dwords) \
	{ \
		int n = (element_size) >> 2; \
		char *a = (element_a); \
		char *b = (element_b); \
		do \
		{ \
			long t = *(long *)b; \
			long u = *(long *)a; \
			*(long *)a = t; \
			a += 4; \
			*(long *)b = u; \
			b += 4; \
			n--; \
		} while (n > 0); \
	} \
	else \
	{ \
		int n = (element_size); \
		char *a = (element_a); \
		char *b = (element_b); \
		do \
		{ \
			char t = *b; \
			char u = *a; \
			*a = t; \
			a++; \
			*b = u; \
			b++; \
			n--; \
		} while (n > 0); \
	} \
}

void sort_shortsort(char *lo, char *hi, unsigned int element_size, bool swap_dwords, t_compare_function compare, const void *context);

/* sorts count elements of element_size bytes; compare(a, b, context) says
   whether a goes after b */
// @retail 0x13da70
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_compare_function compare, const void *context)
{
	char *lo_stack[k_sort_stack_size];
	char *hi_stack[k_sort_stack_size];
	long stack_index;

	if (count < 2 || element_size == 0)
	{
		return;
	}

	stack_index = 0;
	bool swap_dwords = !(element_size & 3) && !((long)elements & 3);
	char *lo = (char *)elements;
	char *hi = (char *)elements + (count - 1) * element_size;

	for (;;)
	{
		unsigned long size = (unsigned long)(hi - lo) / element_size + 1;
		if (size <= k_sort_insertion_cutoff)
		{
			sort_shortsort(lo, hi, element_size, swap_dwords, compare, context);
		}
		else
		{
			char *middle = lo + (size / 2) * element_size;
			SWAP_ELEMENTS(middle, lo, element_size, swap_dwords);

			char *lo_guy = lo;
			char *hi_guy = hi + element_size;
			for (;;)
			{
				do
				{
					lo_guy += element_size;
				} while (lo_guy <= hi && !compare(lo_guy, lo, context));

				do
				{
					hi_guy -= element_size;
				} while (hi_guy > lo && !compare(lo, hi_guy, context));

				if (hi_guy < lo_guy)
				{
					break;
				}

				SWAP_ELEMENTS(lo_guy, hi_guy, element_size, swap_dwords);
			}

			SWAP_ELEMENTS(lo, hi_guy, element_size, swap_dwords);

			if (hi_guy - 1 - lo >= hi - lo_guy)
			{
				if (lo + element_size < hi_guy)
				{
					lo_stack[stack_index] = lo;
					hi_stack[stack_index] = hi_guy - element_size;
					stack_index++;
				}
				if (lo_guy < hi)
				{
					lo = lo_guy;
					continue;
				}
			}
			else
			{
				if (lo_guy < hi)
				{
					lo_stack[stack_index] = lo_guy;
					hi_stack[stack_index] = hi;
					stack_index++;
				}
				if (lo + element_size < hi_guy)
				{
					hi = hi_guy - element_size;
					continue;
				}
			}
		}

		stack_index--;
		if (stack_index < 0)
		{
			break;
		}
		lo = lo_stack[stack_index];
		hi = hi_stack[stack_index];
	}
}

/* the selection sort of the small partitions: moves the largest element to
   the end, until one is left */
// @retail 0x13dcd0
void sort_shortsort(char *lo, char *hi, unsigned int element_size, bool swap_dwords, t_compare_function compare, const void *context)
{
	while (hi > lo)
	{
		char *max = lo;
		char *p = lo + element_size;
		while (p <= hi)
		{
			max = compare(p, max, context) ? p : max;
			p += element_size;
		}

		if (swap_dwords)
		{
			SWAP_ELEMENTS(max, hi, element_size, true);
		}
		else
		{
			SWAP_ELEMENTS(max, hi, element_size, false);
		}

		hi -= element_size;
	}
}

/* the index of key in a sorted array of 4-byte values, or -1 */
// @retail 0x13dd70
long binary_search_4byte(long key, const long *base, long count, t_bsearch_4byte_compare_function compare, const long *context)
{
    const long *start = base;
    while (count != 0)
    {
        const long *middle = start + (count >> 1);
        long result = compare(key, *middle, context);
        if (result == 0)
        {
            return middle - base;
        }
        if (result > 0)
        {
            start = middle + 1;
            count--;
        }
        count >>= 1;
    }
    return -1;
}

/* the index of key in a sorted array of element_size-byte elements, or -1 */
// @retail 0x13ddd0
long function_13ddd0(const void *key, const void *base, long count, long element_size, t_bsearch_compare_function compare, const void *context)
{
    const char *start = (const char *)base;
    while (count != 0)
    {
        const char *middle = start + (count >> 1) * element_size;
        long result = compare(key, middle, context);
        if (result == 0)
        {
            return (middle - (const char *)base) / element_size;
        }
        if (result > 0)
        {
            start = middle + element_size;
            count--;
        }
        count >>= 1;
    }
    return -1;
}

typedef bool (__stdcall *t_sort_4byte_compare_function)(long, long, const void *);
typedef bool (__stdcall *t_sort_2byte_compare_function)(word, word, const void *);

/* sort_4byte's selection sort of the small partitions */
// @retail 0x13e0e0
PRIVATE void shortsort_4byte(long *lo, long *hi, t_sort_4byte_compare_function compare, const void *context)
{
	while (hi > lo)
	{
		long *max = lo;
		for (long *p = lo + 1; p <= hi; p++)
		{
			if (compare(*p, *max, context))
			{
				max = p;
			}
		}

		long swap = *max;
		*max = *hi;
		*hi = swap;

		hi--;
	}
}

/* sorts an array of 4-byte values */
// @retail 0x13de30
void sort_4byte(void *unused, long *elements, unsigned long count, t_sort_4byte_compare_function compare, const void *context)
{
	void **unused_reference = &unused;

	long *lo_stack[k_sort_stack_size];
	long *hi_stack[k_sort_stack_size];
	long stack_index;

	if (count < 2)
	{
		return;
	}

	stack_index = 0;
	long *lo = elements;
	long *hi = elements + (count - 1);

	for (;;)
	{
		unsigned long size = (hi - lo) + 1;
		if (size <= k_sort_insertion_cutoff)
		{
			shortsort_4byte(lo, hi, compare, context);
		}
		else
		{
			long *middle = lo + (size / 2);
			long swap = *middle;
			*middle = *lo;
			*lo = swap;

			long *lo_guy = lo;
			long *hi_guy = hi + 1;
			for (;;)
			{
				do
				{
					lo_guy++;
				} while (lo_guy <= hi && !compare(*lo_guy, *lo, context));

				do
				{
					hi_guy--;
				} while (hi_guy > lo && !compare(*lo, *hi_guy, context));

				if (hi_guy < lo_guy)
				{
					break;
				}

				swap = *lo_guy;
				*lo_guy = *hi_guy;
				*hi_guy = swap;
			}

			swap = *lo;
			*lo = *hi_guy;
			*hi_guy = swap;

			if (hi_guy - 1 - lo >= hi - lo_guy)
			{
				if (lo + 1 < hi_guy)
				{
					lo_stack[stack_index] = lo;
					hi_stack[stack_index] = hi_guy - 1;
					stack_index++;
				}
				if (lo_guy < hi)
				{
					lo = lo_guy;
					continue;
				}
			}
			else
			{
				if (lo_guy < hi)
				{
					lo_stack[stack_index] = lo_guy;
					hi_stack[stack_index] = hi;
					stack_index++;
				}
				if (lo + 1 < hi_guy)
				{
					hi = hi_guy - 1;
					continue;
				}
			}
		}

		stack_index--;
		if (stack_index < 0)
		{
			break;
		}
		lo = lo_stack[stack_index];
		hi = hi_stack[stack_index];
	}
}

/* sort_2byte's selection sort of the small partitions */
// @retail 0x13e140
PRIVATE void shortsort_2byte(word *lo, word *hi, t_sort_2byte_compare_function compare, const void *context)
{
	while (hi > lo)
	{
		word *max = lo;
		for (word *p = lo + 1; p <= hi; p++)
		{
			if (compare(*p, *max, context))
			{
				max = p;
			}
		}

		word swap = *max;
		*max = *hi;
		*hi = swap;

		hi--;
	}
}

/* sorts an array of 2-byte values; the scratch comes first, as in sort_4byte (retail's caller 0x1e370 sets it up
   before the count) */
// @retail 0x13df80
void sort_2byte(void *unused, word *elements, unsigned long count, t_sort_2byte_compare_function compare, const void *context)
{
	void **unused_reference = &unused;

	word *lo_stack[k_sort_stack_size];
	word *hi_stack[k_sort_stack_size];
	long stack_index;

	if (count < 2)
	{
		return;
	}

	stack_index = 0;
	word *lo = elements;
	word *hi = elements + (count - 1);

	for (;;)
	{
		unsigned long size = (hi - lo) + 1;
		if (size <= k_sort_insertion_cutoff)
		{
			shortsort_2byte(lo, hi, compare, context);
		}
		else
		{
			word *middle = lo + (size / 2);
			word swap = *middle;
			*middle = *lo;
			*lo = swap;

			word *lo_guy = lo;
			word *hi_guy = hi + 1;
			for (;;)
			{
				do
				{
					lo_guy++;
				} while (lo_guy <= hi && !compare(*lo_guy, *lo, context));

				do
				{
					hi_guy--;
				} while (hi_guy > lo && !compare(*lo, *hi_guy, context));

				if (hi_guy < lo_guy)
				{
					break;
				}

				swap = *lo_guy;
				*lo_guy = *hi_guy;
				*hi_guy = swap;
			}

			swap = *lo;
			*lo = *hi_guy;
			*hi_guy = swap;

			if (hi_guy - 1 - lo >= hi - lo_guy)
			{
				if (lo + 1 < hi_guy)
				{
					lo_stack[stack_index] = lo;
					hi_stack[stack_index] = hi_guy - 1;
					stack_index++;
				}
				if (lo_guy < hi)
				{
					lo = lo_guy;
					continue;
				}
			}
			else
			{
				if (lo_guy < hi)
				{
					lo_stack[stack_index] = lo_guy;
					hi_stack[stack_index] = hi;
					stack_index++;
				}
				if (lo + 1 < hi_guy)
				{
					hi = hi_guy - 1;
					continue;
				}
			}
		}

		stack_index--;
		if (stack_index < 0)
		{
			break;
		}
		lo = lo_stack[stack_index];
		hi = hi_stack[stack_index];
	}
}
