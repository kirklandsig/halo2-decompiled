#include "unknown_11c920.h"

// @flags /O2 /arch:SSE /Gr

typedef bool (__stdcall *t_compare_function)(const void *, const void *, const void *);
typedef long (__stdcall *t_bsearch_compare_function)(const void *, const void *, const void *);
typedef long (__stdcall *t_bsearch_4byte_compare_function)(long, long, const void *);

/* SORT: quicksort and binary search over arrays (after the C runtime's
   qsort), with specialised versions for 4-byte and 2-byte elements */

enum
{
	k_sort_insertion_cutoff = 8,
	k_sort_stack_size = 30
};

#define SWAP_ELEMENTS(element_a, element_b, element_size, swap_dwords) \
{ \
	char *a = (element_a); \
	char *b = (element_b); \
	if (swap_dwords) \
	{ \
		int n = (element_size) >> 2; \
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

void function_13dcd0(char *lo, char *hi, unsigned int element_size, bool swap_dwords, t_compare_function compare, const void *context);

// @retail 0x13da70
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_compare_function compare, const void *context)
{
	struct { char *field_0; char *field_4; char *field_8[k_sort_stack_size]; char *field_80[k_sort_stack_size]; } local_1;
	char **lo_stack = local_1.field_8;
	char **hi_stack = local_1.field_80;
	long stack_index;

	if (count < 2 || element_size == 0)
	{
		return;
	}

	stack_index = 0;
	bool swap_dwords = !(element_size & 3) && !((long)elements & 3);
	char *lo = (char *)elements;
	char *hi = (char *)elements + (count - 1) * element_size;
	char *volatile *local_2 = &local_1.field_0;
	*local_2 = lo;

recurse:
	unsigned long size = (unsigned long)(hi - lo) / element_size + 1;
	if (size <= k_sort_insertion_cutoff)
	{
		function_13dcd0(lo, hi, element_size, swap_dwords, compare, context);
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
				goto recurse;
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
				goto recurse;
			}
		}
	}

	stack_index--;
	if (stack_index >= 0)
	{
		lo = lo_stack[stack_index];
		hi = hi_stack[stack_index];
		goto recurse;
	}
}

// @retail 0x13dcd0
void function_13dcd0(char *lo, char *hi, unsigned int element_size, bool swap_dwords, t_compare_function compare, const void *context)
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

// @retail 0x13dd70
long function_13dd70(long key, const long *base, long count, t_bsearch_4byte_compare_function compare, const long *context)
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

// @retail 0x13de30
void sort_4byte(long *elements, unsigned long count, void *unused, t_sort_4byte_compare_function compare, const void *context)
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

recurse:
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
				goto recurse;
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
				goto recurse;
			}
		}
	}

	stack_index--;
	if (stack_index >= 0)
	{
		lo = lo_stack[stack_index];
		hi = hi_stack[stack_index];
		goto recurse;
	}
}

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

// @retail 0x13df80
void sort_2byte(word *elements, unsigned long count, void *unused, t_sort_2byte_compare_function compare, const void *context)
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

recurse:
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
				goto recurse;
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
				goto recurse;
			}
		}
	}

	stack_index--;
	if (stack_index >= 0)
	{
		lo = lo_stack[stack_index];
		hi = hi_stack[stack_index];
		goto recurse;
	}
}
