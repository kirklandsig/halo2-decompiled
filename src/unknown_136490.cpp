// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_136490.CPP: bitmap sizes */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_13eeb0.h"
#include <xtl.h>
#include <string.h>

struct s_type_7ba8e9
{
	dword signature;
	short width;
	short height;
	char depth;
	byte more_flags;
	short type;
	short format;
	word flags;
	short registration_point[2];
	short mipmap_count;
	byte unknown16[0x50 - 0x16];
	D3DResource *resource;
	void *base_address;
	byte unknown58[0x74 - 0x58];
};

enum
{
	_enum_value_9146b9 = 2,
	_enum_value_e13d3d = 24
};

/* the bits per pixel of each format */
short g_453550[_enum_value_e13d3d] = { 8, 8, 8, 16, 0, 0, 16, 0, 16, 16, 32, 32, 0, 0, 4, 8, 8, 8, 8, 128, 96, 48, 16, 16 };

struct s_bitmap_data;
void texture_cache_bitmap_unload(s_bitmap_data *bitmap);

// @retail 0x1359d0
void function_1359d0(s_type_7ba8e9 *bitmap)
{
	if (bitmap)
	{
		texture_cache_bitmap_unload((s_bitmap_data *)bitmap);
		if (bitmap->resource)
		{
			D3DResource_Release(bitmap->resource);
			bitmap->resource = NULL;
		}
		if (bitmap->flags & 0x100)
		{
			if (bitmap->base_address && !VirtualFree(bitmap->base_address, 0, MEM_RELEASE))
			{
				GetLastError();
			}
			if (!VirtualFree(bitmap, 0, MEM_RELEASE))
			{
				GetLastError();
			}
		}
	}
}

// @retail 0x1358c0
short function_1358c0(short format)
{
	long bits = 0;
	if (format != NONE)
	{
		bits = g_453550[format];
	}
	return bits;
}

static inline short function_x48d32c(short format)
{
	return function_1358c0(format);
}

// @retail 0x1364e0
long function_1364e0(s_type_7ba8e9 const *bitmap, short mipmap_index)
{
	short width = bitmap->width >> mipmap_index > 1 ? bitmap->width >> mipmap_index : 1;
	if (bitmap->flags & 2)
	{
		width += -width & 3;
	}
	short height = bitmap->height >> mipmap_index > 1 ? bitmap->height >> mipmap_index : 1;
	if (bitmap->flags & 2)
	{
		height += -height & 3;
	}
	short depth = bitmap->depth >> mipmap_index > 1 ? bitmap->depth >> mipmap_index : 1;
	long pixels = depth * (width * height);
	if (bitmap->type == _enum_value_9146b9)
	{
		pixels *= 6;
	}
	return pixels;
}

// @retail 0x136490
long function_136490(s_type_7ba8e9 const *bitmap)
{
	long pixels = 0;
	for (short mipmap_index = 0; mipmap_index <= bitmap->mipmap_count; mipmap_index++)
	{
		pixels += function_1364e0(bitmap, mipmap_index);
	}
	short bits = function_x48d32c(bitmap->format);
	return bits * pixels / 8;
}

// @retail 0x1365a0
long function_1365a0(s_type_7ba8e9 const *bitmap, short mipmap_index)
{
	short width = bitmap->width >> mipmap_index > 1 ? bitmap->width >> mipmap_index : 1;
	if (bitmap->flags & 2)
	{
		width += -width & 3;
	}
	short bits = function_x48d32c(bitmap->format);
	return bits * width / 8;
}

PRIVATE __forceinline short function_136601(short arg_1, long arg_2)
{
	long local_1 = 0;
	if (arg_1 != NONE)
		local_1 = g_453550[arg_2];
	return local_1;
}

// @retail 0x136600
long function_136600(short width, short height, short mipmap_index, short depth, s_13eeb1 format, short alignment)
{
	short mipmap_width = width >> mipmap_index > 1 ? width >> mipmap_index : 1;
	short mipmap_height = height >> mipmap_index > 1 ? height >> mipmap_index : 1;
	short mipmap_depth = depth >> mipmap_index > 1 ? depth >> mipmap_index : 1;
	long local_3 = (short)format;
	if (local_3 >= 14 && local_3 <= 16)
	{
		mipmap_width += -mipmap_width & 3;
		mipmap_height += -mipmap_height & 3;
	}
	short row_size = function_136601((short)format, local_3) * mipmap_width / 8;
	if (alignment > 0)
	{
		row_size = (row_size + alignment - 1) & ~(alignment - 1);
	}
	return row_size * mipmap_depth * mipmap_height;
}

// @retail 0x1366d0
long function_1366d0(short width, short height, short depth, short format, short alignment, long mipmap_count)
{
	long local_1 = *(long volatile *)&mipmap_count;
	long total = 0;
	for (short mipmap_index = 0; mipmap_index <= (short)local_1; mipmap_index++)
	{
		s_13eeb1 local_2 = { *(long const *)&format };
		total += function_136600(width, height, mipmap_index, depth, local_2, alignment);
	}
	return total;
}

// @retail 0x1358e0
s_type_7ba8e9 *function_1358e0(short width, short height, short mipmap_count, short format, word flags)
{
	s_type_7ba8e9 *bitmap = (s_type_7ba8e9 *)VirtualAlloc(NULL, sizeof(s_type_7ba8e9), MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
	if (!bitmap)
	{
		GetLastError();
	}
	else
	{
		memset(bitmap, 0, sizeof(*bitmap));
		bitmap->type = 0;
		bitmap->mipmap_count = mipmap_count;
		bitmap->width = width;
		bitmap->signature = 'bitm';
		bitmap->height = height;
		bitmap->depth = 1;
		bitmap->format = format;
		bitmap->flags = flags | 0x100;
		if (!(width & (width - 1)) && !(height & (height - 1)))
		{
			bitmap->flags |= 1;
		}
		if (format >= 14 && format <= 16)
		{
			bitmap->flags |= 2;
		}
		if (format == 17)
		{
			bitmap->flags |= 4;
		}
		bitmap->base_address = NULL;
		if (!(flags & 0x800))
		{
			long size = function_136490(bitmap);
			void *base_address = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
			if (!base_address)
			{
				GetLastError();
			}
			bitmap->base_address = base_address;
		}
	}
	return bitmap;
}

/* the address of a pixel of a 2d texture's mipmap */
// @retail 0x135a30
void *function_135a30(s_type_7ba8e9 const *bitmap, short mipmap_index, short x, short y)
{
	short width = bitmap->width;
	long offset = 0;
	short minimum = (bitmap->flags & 2) ? 4 : 1;
	short height = bitmap->height;
	long bits = function_x48d32c(bitmap->format);

	for (short i = 0; i < mipmap_index; i++)
	{
		offset += width * height;
		width = minimum > width >> 1 ? minimum : width >> 1;
		height = minimum > height >> 1 ? minimum : height >> 1;
	}

	return (byte *)bitmap->base_address + bits * (x + y * width + offset) / 8;
}

/* the address of a pixel of a 3d texture's mipmap */
// @retail 0x135af0
void *function_135af0(s_type_7ba8e9 const *bitmap, short x, short y, short z, short mipmap_index)
{
	short width = bitmap->width;
	short minimum = (bitmap->flags & 2) ? 4 : 1;
	short height = bitmap->height;
	short depth = bitmap->depth;
	long offset = 0;
	long bits = function_x48d32c(bitmap->format);

	for (short i = 0; i < mipmap_index; i++)
	{
		offset += width * height * depth;
		width = minimum > width >> 1 ? minimum : width >> 1;
		height = minimum > height >> 1 ? minimum : height >> 1;
		depth = 1 > depth >> 1 ? 1 : depth >> 1;
	}

	return (byte *)bitmap->base_address + bits * (x + width * (y + height * z) + offset) / 8;
}

/* the address of a pixel of a cube map face's mipmap */
// @retail 0x135c00
void *function_135c00(s_type_7ba8e9 const *bitmap, short x, short y, short face, short mipmap_index)
{
	long offset = 0;
	short minimum = (bitmap->flags & 2) ? 4 : 1;
	short size = bitmap->width;
	long bits = function_x48d32c(bitmap->format);

	for (short i = 0; i < mipmap_index; i++)
	{
		offset += size * size * 6;
		size = minimum > size >> 1 ? minimum : size >> 1;
	}

	return (byte *)bitmap->base_address + bits * (x + size * (y + size * face) + offset) / 8;
}

/* Packed normal palette indexed by eight-bit bitmap samples. */
dword g_468848[256] =
{
	0xff7e7eff, 0xff7e7fff, 0xff7e80ff, 0xff7e81ff, 0xff7f7eff, 0xff7f7fff, 0xff7f80ff, 0xff7f81ff,
	0xff807eff, 0xff807fff, 0xff8080ff, 0xff8081ff, 0xff817eff, 0xff817fff, 0xff8180ff, 0xff8181ff,
	0xff7f82ff, 0xff837fff, 0xff7d7fff, 0xff8183ff, 0xff817cff, 0xff7c82ff, 0xff8481ff, 0xff7d7cff,
	0xff7f85ff, 0xff847dff, 0xff7a80ff, 0xff8484ff, 0xff807aff, 0xff7c85ff, 0xff877fff, 0xff7a7cff,
	0xff8288ff, 0xff8479ff, 0xff7883ff, 0xff8884ff, 0xff7c77ff, 0xff7d89ff, 0xff897bff, 0xff767dff,
	0xff8689ff, 0xff8275ff, 0xff7787ff, 0xff8c81ff, 0xff7877ff, 0xff808dff, 0xff8977ff, 0xff7381ff,
	0xff8b88ff, 0xff7e72ff, 0xff788cff, 0xff8e7cff, 0xff7379ff, 0xff858eff, 0xff8671ff, 0xff7187fe,
	0xff9085fe, 0xff7871fe, 0xff7c91fe, 0xff8e76fe, 0xff6e7efe, 0xff8c8efe, 0xff816dfe, 0xff728efe,
	0xff937ffe, 0xff7173fe, 0xff8394fe, 0xff8c6ffe, 0xff6b85fe, 0xff938bfe, 0xff796bfe, 0xff7794fe,
	0xff9577fd, 0xff6a78fd, 0xff8b95fd, 0xff8669fd, 0xff6c8dfd, 0xff9884fd, 0xff716cfd, 0xff7e99fd,
	0xff936ffd, 0xff6680fd, 0xff9392fd, 0xff7e65fd, 0xff6f96fd, 0xff9b7bfc, 0xff6871fc, 0xff879bfc,
	0xff8d67fc, 0xff658afc, 0xff9b8bfc, 0xff7365fc, 0xff779dfc, 0xff9b71fc, 0xff6279fc, 0xff929afc,
	0xff8460fb, 0xff6795fb, 0xffa181fb, 0xff6969fb, 0xff81a1fb, 0xff9666fb, 0xff5e84fb, 0xff9c94fb,
	0xff785efb, 0xff6e9ffb, 0xffa275fa, 0xff5f71fa, 0xff8ea2fa, 0xff8d5dfa, 0xff5f91fa, 0xffa48afa,
	0xff6c60fa, 0xff79a6fa, 0xff9f68f9, 0xff597df9, 0xff9b9df9, 0xff8058f9, 0xff659ef9, 0xffa97cf9,
	0xff5f67f9, 0xff87a9f8, 0xff975cf8, 0xff578bf8, 0xffa694f8, 0xff7157f8, 0xff6fa8f8, 0xffa86df8,
	0xff5673f7, 0xff96a7f7, 0xff8a54f7, 0xff5b9af7, 0xffae86f7, 0xff625cf7, 0xff7eaff7, 0xffa25ef6,
	0xff5082f6, 0xffa59ff6, 0xff7a50f6, 0xff64a8f6, 0xffb075f6, 0xff5567f5, 0xff8fb0f5, 0xff9552f5,
	0xff5194f5, 0xffb092f5, 0xff6852f4, 0xff72b2f4, 0xffac64f4, 0xff4c77f4, 0xffa1aaf4, 0xff854af4,
	0xff58a5f3, 0xffb780f3, 0xff575bf3, 0xff85b7f3, 0xffa254f3, 0xff498af2, 0xffb09ef2, 0xff7149f2,
	0xff65b3f2, 0xffb66cf2, 0xff4a6af1, 0xff99b5f1, 0xff9248f1, 0xff4c9ef1, 0xffbb8df0, 0xff5d4ff0,
	0xff78bcf0, 0xffaf59f0, 0xff427df0, 0xffacacef, 0xff7d42ef, 0xff58b0ef, 0xffbf78ef, 0xff4c5cee,
	0xff8ebfee, 0xffa048ee, 0xff4294ee, 0xffbb9ced, 0xff6743ed, 0xff69beed, 0xffbb61ed, 0xff3f6fed,
	0xffa4b9ec, 0xff8c3dec, 0xff4aaaec, 0xffc486eb, 0xff514deb, 0xff80c5eb, 0xffaf4deb, 0xff3a86ea,
	0xffb8abea, 0xff743aea, 0xff5abcea, 0xffc56de9, 0xff405fe9, 0xff99c4e9, 0xff9c3de9, 0xff3e9fe8,
	0xffc696e8, 0xff5b40e8, 0xff70c9e7, 0xffbd55e7, 0xff3576e7, 0xffb1bae7, 0xff8334e6, 0xff4ab6e6,
	0xffcd7de6, 0xff454ee5, 0xff8acde5, 0xffad40e5, 0xff3391e4, 0xffc4a7e4, 0xff6834e4, 0xff5ec8e3,
	0xffca61e3, 0xff3465e3, 0xffa5c8e3, 0xff9531e2, 0xff3bace2, 0xffd18ee2, 0xff4e3fe1, 0xff79d3e1,
	0xffbd48e1, 0xff2c80e0, 0xffbeb9e0, 0xff792ce0, 0xff4cc3df, 0xffd471df, 0xff3852df, 0xff96d3de,
	0xffa833de, 0xff2f9edd, 0xffd1a1dd, 0xff5b31dd, 0xff66d4dc, 0xffcc54dc, 0xff296ddc, 0xffb3c9db,
	0xff8c27db, 0xff3bbadb, 0xffda84da, 0xff4040da, 0xff84dbd9, 0xffbb3ad9, 0xff258cd9, 0xffcbb5d8,
	0xff6c26d8, 0xff52d0d8, 0xffd964d7, 0xff2b59d7, 0xffa4d7d6, 0xffa027d6, 0xff2cacd6, 0x008080ff,
};

dword __cdecl pack_color4f(color4f const *color);
dword __cdecl pack_color3f(color3f const *color);
real function_135880(word value);

static __forceinline real bitmap_display_component(real value)
{
	value *= 100.0f;
	return value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
}

// @retail 0x135ca0
dword function_135ca0(void const *pixels, short format, long index)
{
	switch (format)
	{
	case 6:
		{
			dword value = ((word const *)pixels)[index];
			dword high = (((value & 0xfffff800) | 0xffff0000) << 3) | (value & 0x7e0);
			high = (high << 2) | (value & 0xffffe01f);
			return (high << 3) | ((((value >> 1) & 0xe) | (value & 0x600)) >> 1);
		}
	case 8:
		{
			dword value = ((word const *)pixels)[index];
			dword high = ((value & 0x7c00) << 3) | (value & 0x3e0);
			high = (high << 2) | (value & 0x7000);
			high = (high << 1) | (value & 0x1f);
			high = (high << 2) | (value & 0x380);
			return (high << 1) | ((value >> 2) & 7) | (-(value >> 15) << 24);
		}
	case 9:
		{
			dword value = ((word const *)pixels)[index];
			dword red = (value >> 8) & 15;
			dword green = (value >> 4) & 15;
			dword blue = value & 15;
			dword high = ((((value >> 8) & 0xfffffff0) << 12) | value) & 0xfffff000;
			high |= ((red << 4) | red) << 4;
			return (((((high | green) << 4 | green) << 4 | blue) << 4) | blue);
		}
	case 10:
	case 11:
		return ((dword const *)pixels)[index];
	case 0:
		return ((byte const *)pixels)[index] << 24;
	case 1:
		{
			dword value = ((byte const *)pixels)[index];
			return (((value | 0xffffff00) << 8 | value) << 8) | value;
		}
	case 2:
		{
			dword value = ((byte const *)pixels)[index];
			return (((value << 8 | value) << 8 | value) << 8) | value;
		}
	case 3:
		{
			dword value = ((word const *)pixels)[index];
			dword intensity = value & 0xff;
			return (((value << 8) | intensity) << 8) | intensity;
		}
	case 17:
		return g_468848[((byte const *)pixels)[index]];
	case 18:
		return ((byte const *)pixels)[index];
	case 19:
		{
			color4f color = ((color4f const *)pixels)[index];
			color.alpha = bitmap_display_component(color.alpha);
			color.red = bitmap_display_component(color.red);
			color.green = bitmap_display_component(color.green);
			color.blue = bitmap_display_component(color.blue);
			return pack_color4f(&color);
		}
	case 20:
		{
			color3f color = ((color3f const *)pixels)[index];
			color.red = bitmap_display_component(color.red);
			color.green = bitmap_display_component(color.green);
			color.blue = bitmap_display_component(color.blue);
			return pack_color3f(&color);
		}
	case 21:
		{
			struct s_half_color { word red, green, blue; };
			s_half_color color = ((s_half_color const *)pixels)[index];
			color4f converted;
			converted.alpha = 1.0f;
			converted.red = bitmap_display_component(function_135880(color.red));
			converted.green = bitmap_display_component(function_135880(color.green));
			converted.blue = bitmap_display_component(function_135880(color.blue));
			return pack_color4f(&converted);
		}
	case 22:
		{
			dword value = ((word const *)pixels)[index];
			return ((value | 0xffffff00) << 16) | (value & 0xffffff00);
		}
	case 23:
		return ((word const *)pixels)[index] | 0xff000000;
	default:
		return 0;
	}
}

#include "unknown_223b60.h"
struct S3TCBlockRGB;
struct S3TCBlockRGBA_explicit;
struct S3TCBlockRGBA_interpolated;
void function_223ed0(S3TCBlockRGB const *block, S3TC_COLOR *out, short x, short y);
void DecodeBlockRGBA_explicit__single_pixel(S3TCBlockRGBA_explicit const *block, S3TC_COLOR *out, short x, short y);
void DecodeBlockRGBA_interpolated__single_pixel(S3TCBlockRGBA_interpolated const *block, S3TC_COLOR *out, short x, short y);
void function_358d0(short width, short x, short y, short height, dword *out);
bool g_55e72c;

union s_136141
{
	void const *field_0;
	dword field_1;
};

// @retail 0x136140
dword __fastcall function_136140(long arg_1, long arg_2, s_136141 arg_3, long size, void const *pixels, short x, short height, short format, word flags)
{
	void const *base = arg_3.field_0;
	short y = (short)arg_1;
	short width = (short)arg_2;
	short const *x_reference = &x;
	x = *x_reference;
	if (flags & 2)
	{
		long bits = 0;
		if (format != NONE)
			bits = g_453550[format];
		short block_bytes = (short)((short)bits * 16 / 8);
		byte const *block = (byte const *)pixels + ((short)(y / 4) * width / 4 + (short)(x / 4)) * block_bytes;
		x &= 3;
		y &= 3;
		if (block < base || block >= (byte const *)base + size)
		{
			if (!g_55e72c)
				g_55e72c = true;
			block = (byte const *)base;
			x = y = 0;
		}
		dword result;
		switch (format)
		{
		case 14: function_223ed0((S3TCBlockRGB const *)block, (S3TC_COLOR *)&result, x, y); break;
		case 15: DecodeBlockRGBA_explicit__single_pixel((S3TCBlockRGBA_explicit const *)block, (S3TC_COLOR *)&result, x, y); break;
		case 16: DecodeBlockRGBA_interpolated__single_pixel((S3TCBlockRGBA_interpolated const *)block, (S3TC_COLOR *)&result, x, y); break;
		default: return 0;
		}
		return result;
	}
	if (flags & 8)
	{
		dword offsets[2];
		function_358d0(width, x, y, height, offsets);
		return function_135ca0(pixels, format, offsets[0] | offsets[1]);
	}
	return function_135ca0(pixels, format, y * width + x);
}

__forceinline long bitmap_round(real value)
{
	long result;
	__asm { fld value }
	__asm { fistp result }
	return result;
}

// @retail 0x1362b0
dword function_1362b0(s_type_7ba8e9 const *bitmap, point2f const *uv, real detail)
{
	if (!bitmap->base_address)
		return 0xffffffff;
	long mipmap = 0;
	if (detail < 1.0f && bitmap->mipmap_count > 0)
		mipmap = bitmap_round((1.0f - detail) * bitmap->mipmap_count);
	short width = bitmap->width >> (short)mipmap > 1 ? bitmap->width >> (short)mipmap : 1;
	if (bitmap->flags & 2)
		width += (byte)(-(byte)width) & 3;
	short height = bitmap->height >> (short)mipmap > 1 ? bitmap->height >> (short)mipmap : 1;
	if (bitmap->flags & 2)
		height += (byte)(-(byte)height) & 3;
	real value = width * uv->x - 0.5f;
	long x;
	if (!(width & (width - 1)))
		x = bitmap_round(value) & (width - 1);
	else
		x = (bitmap_round(value) % width + width) % width;
	value = height * uv->y - 0.5f;
	long y;
	if (!(height & (height - 1)))
		y = bitmap_round(value) & (height - 1);
	else
		y = (bitmap_round(value) % height + height) % height;
	void *pixels;
	switch (bitmap->type)
	{
	case 0: pixels = function_135a30(bitmap, (short)mipmap, 0, 0); break;
	case 1: pixels = function_135af0(bitmap, 0, 0, 0, (short)mipmap); break;
	default: pixels = function_135c00(bitmap, 0, 0, 0, (short)mipmap); break;
	}
	s_136141 local_1;
	local_1.field_0 = bitmap->base_address;
	return function_136140((short)y, width, local_1, *(long *)((byte *)bitmap + 0x34), pixels, (short)x, height, bitmap->format, bitmap->flags);
}
