#include "unknown_11c920.h"
#include "unknown_1a1940.h"
#include <string.h>

// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_ring_buffer
{
	void write_wrapped(long count, long offset, const void *src);
	void read_wrapped(long offset, long count, void *dst);
	long write(long count, const void *src);

	long size;
	byte *data;
	long start;
	long used;
};

// @retail 0x1a1940
void s_ring_buffer::write_wrapped(long count, long offset, const void *src)
{
	long first = size - offset > count ? count : size - offset;
	long local_0 = count - first;
	if (first > 0)
		memcpy(data + offset, src, first);
	if (local_0 > 0)
		memcpy(data, (const byte *)src + first, local_0);
}

// @retail 0x1a19a0
void s_ring_buffer::read_wrapped(long offset, long count, void *dst)
{
	long first = size - offset > count ? count : size - offset;
	long local_0 = count - first;
	if (first > 0)
		memcpy(dst, data + offset, first);
	if (local_0 > 0)
		memcpy((byte *)dst + first, data, local_0);
}

// @retail 0x1a1900
long s_ring_buffer::write(long count, const void *src)
{
	long result = NONE;

	if (used + count <= size)
	{
		result = (start + used) % size;
		if (count > 0)
			write_wrapped(count, result, src);
		used += count;
	}
	return result;
}

class c_page_heap : public c_page_allocator
{
public:
	virtual void free_pages(byte *address, long size, long unused);
	virtual long get_page_size();
	virtual void *allocate_pages(long alignment_mask, long size, long unused);

private:
	byte *base;
	long unknown08;
	long page_count;
	dword bits[1019];
	long *used_pages;
};

// @retail 0x1a1a00
void c_page_heap::free_pages(byte *address, long size, long unused)
{
	long offset = address - base;
	long start = offset / 0x1000;
	long end = (offset + size + 0xfff) / 0x1000;
	for (long page = start; page < end; page++)
		bits[page >> 5] &= ~(1 << (page & 0x1f));
	*used_pages += start - end;
}

// @retail 0x1a1a80
long c_page_heap::get_page_size()
{
	return 0x1000;
}

// @retail 0x1a1a90
void *c_page_heap::allocate_pages(long alignment_mask, long size, long unused)
{
	void *result = NULL;
	long shift;

	switch (alignment_mask)
	{
	case 0xfff: shift = 0; break;
	case 0x1fff: shift = 1; break;
	case 0x3fff: shift = 2; break;
	case 0x7fff: shift = 3; break;
	case 0xffff: shift = 4; break;
	default: goto local_0;
	}
	long pages = (size + 0xfff) / 0x1000;
	long step_mask = (1 << (shift + 12)) - 1;
	long index = ((((long)base + step_mask) & ~step_mask) - (long)base) / 0x1000;
	long first = index;
	long end = index + pages;
	while (end <= page_count)
	{
		bool free_run = true;
		long checked = index;
		long page = index;
		while (page < end && free_run)
		{
			free_run = !(bits[page >> 5] & (1 << (page & 0x1f)));
			page++;
			checked++;
		}
		if (free_run)
			break;
		if (index + 1 <= checked)
			index = checked;
		else
			index++;
		alignment_mask = (1 << shift) - 1;
		index = first + ((index - first + alignment_mask) & ~alignment_mask);
		end = index + pages;
	}
	if (index + pages <= page_count)
	{
		for (long p = index; p < index + pages; p++)
			bits[p >> 5] |= 1 << (p & 0x1f);
		result = base + (index << 12);
		*used_pages += pages;
	}
local_0:
	return result;
}

// @retail 0x1a1c20 deleting c_page_heap
