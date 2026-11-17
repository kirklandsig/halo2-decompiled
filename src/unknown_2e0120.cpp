// @flags /O2 /GL-
// Havok (third-party physics library): a step of an iterative solver: asks
// for the response against the reversed direction, accumulates it, offsets
// the accumulated value by the reciprocal scales, and returns the response
// reversed.
#include <xmmintrin.h>

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

class solver_2e0120
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void respond(__m128 const *direction, __m128 *out, void *context) {}

	__m128 accumulated; // at 0x10: the vtable pointer is padded to the vector alignment
	__m128 offset;
	float scale[3];

	void function_2e0120(__m128 const *direction, __m128 *out, void *context);
};

// @retail 0x2e0120
void solver_2e0120::function_2e0120(__m128 const *direction, __m128 *out, void *context)
{
	__m128 const sign = *(__m128 const *)g_4124a0;
	__m128 reversed = _mm_xor_ps(*direction, sign);
	__m128 response;
	respond(&reversed, &response, context);
	_mm_store_ps((float *)&accumulated, _mm_add_ps(accumulated, response));
	__m128 reciprocal;
	((float *)&reciprocal)[0] = 1.0f / scale[0];
	((float *)&reciprocal)[1] = 1.0f / scale[1];
	((float *)&reciprocal)[2] = 1.0f / scale[2];
	((float *)&reciprocal)[3] = 0.0f;
	offset = _mm_add_ps(accumulated, reciprocal);
	*out = _mm_xor_ps(response, *(__m128 const *)g_4124a0);
}

// an entry sorted by its 16-bit key
struct entry_2e0250
{
	unsigned short key;
	unsigned short value;
};

// the order the sort compares by, an empty object passed by value
struct order_2e01c0
{
};

// sorts the 16-bit values from low to high (both included)
// @retail 0x2e01c0
void __cdecl function_2e01c0(unsigned short *entries, int low, int high, order_2e01c0 order)
{
	int j = high;
	int i = low;
	unsigned short pivot = entries[(low + high) >> 1];
	do
	{
		while (entries[i] < pivot)
		{
			i++;
		}
		while (pivot < entries[j])
		{
			j--;
		}
		if (j >= i)
		{
			if (j != i)
			{
				__declspec(align(16)) unsigned short swap = entries[j];
				entries[j] = entries[i];
				entries[i] = swap;
			}
			j--;
			i++;
		}
	} while (i <= j);
	if (low < j)
	{
		function_2e01c0(entries, low, j, order);
	}
	if (i < high)
	{
		function_2e01c0(entries, i, high, order);
	}
}

struct order_2e0250
{
};

// sorts the entries from low to high (both included) by key
// @retail 0x2e0250
void __cdecl function_2e0250(entry_2e0250 *entries, int low, int high, order_2e0250 order)
{
	int j = high;
	int i = low;
	entry_2e0250 pivot = entries[(low + high) >> 1];
	do
	{
		while (entries[i].key < pivot.key)
		{
			i++;
		}
		while (pivot.key < entries[j].key)
		{
			j--;
		}
		if (j >= i)
		{
			if (j != i)
			{
				__declspec(align(16)) entry_2e0250 swap = entries[j];
				entries[j] = entries[i];
				entries[i] = swap;
			}
			j--;
			i++;
		}
	} while (i <= j);
	if (low < j)
	{
		function_2e0250(entries, low, j, order);
	}
	if (i < high)
	{
		function_2e0250(entries, i, high, order);
	}
}

// sorts the entries from low to high (both included) by key
// @retail 0x2e02d0
void __cdecl function_2e02d0(entry_2e0250 *entries, int low, int high, order_2e0250 context)
{
	int j = high;
	int i = low;
	entry_2e0250 pivot = entries[(low + high) >> 1];
	do
	{
		while (entries[i].key < pivot.key)
		{
			i++;
		}
		while (pivot.key < entries[j].key)
		{
			j--;
		}
		if (j >= i)
		{
			if (j != i)
			{
				__declspec(align(16)) entry_2e0250 swap = entries[j];
				entries[j] = entries[i];
				entries[i] = swap;
			}
			j--;
			i++;
		}
	} while (i <= j);
	if (low < j)
	{
		function_2e02d0(entries, low, j, context);
	}
	if (i < high)
	{
		function_2e02d0(entries, i, high, context);
	}
}
