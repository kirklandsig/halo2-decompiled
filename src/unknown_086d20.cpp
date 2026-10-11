// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_086D20.CPP: positions in the simulation's messages, quantized to
   a number of bits per axis over the world's bounds (or over -10..10) and
   written to or read from a bitstream (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "bitstream.h"
#include <string.h>

/* src/unknown_11eed0.cpp */
void function_11f4f0(long bits, real const *in, real_bounds const *ranges, long *out);
void function_11f580(long bits, real *out, real const *ranges, long const *in);

/* src/unknown_195720.cpp */
void function_195720(s_bitstream *stream, dword value, long count);
dword function_1959c0(s_bitstream *stream, long count);

/* src/unknown_14a280.cpp */
struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
extern s_bsp3d *g_4e033c;

/* the world's bounds, as the quantization reads them */
struct s_world_bounds_view
{
	byte unknown00[0x34];
	real_bounds bounds[3];
};

static inline real_bounds const *world_bounds(void)
{
	return ((s_world_bounds_view *)g_4e0348)->bounds;
}

/* the leaf of the world's bsp a point is in */
static inline long world_leaf(real const *point)
{
	return function_14a280(g_4e033c, 0, (point3f *)point);
}

/* the small range of the relative positions */
const real_bounds g_440214[3] = { { -10.0f, 10.0f }, { -10.0f, 10.0f }, { -10.0f, 10.0f } };

/* the steps tried, one quantum each way, when a quantized position falls
   outside the world */
const long g_47fe88[14][3] =
{
	{ 0, 0, 1 },
	{ 1, 1, 1 },
	{ -1, 1, 1 },
	{ 1, -1, 1 },
	{ -1, -1, 1 },
	{ 1, 1, -1 },
	{ -1, 1, -1 },
	{ 1, -1, -1 },
	{ -1, -1, -1 },
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ -1, 0, 0 },
	{ 0, -1, 0 },
	{ 0, 0, -1 },
};

/* a quantized position moved by one step, if it stays in range */
static __forceinline bool quantized_step(long *candidate, long const *quantized, long const *step, long bits)
{
	for (long axis = 0; axis < 3; axis++)
	{
		candidate[axis] = quantized[axis] + step[axis];
		if (candidate[axis] < 0 || candidate[axis] >= (1 << bits))
			return false;
	}
	return true;
}

/* writes a world position; when asked, a quantized position outside the
   world is moved by one quantum to a neighbour inside it */
// @retail 0x86d20
void simulation_write_position(long bits, s_bitstream *stream, real const *position, bool keep_inside)
{
	real_bounds const *ranges = world_bounds();
	long quantized[3];
	function_11f4f0(bits, position, ranges, quantized);
	if (keep_inside)
	{
		real dequantized[3];
		function_11f580(bits, dequantized, (real const *)ranges, quantized);
		if (world_leaf(dequantized) == NONE && world_leaf(position) != NONE)
		{
			for (long step = 0; step < sizeof(g_47fe88) / sizeof(g_47fe88[0]); step++)
			{
				long candidate[3];
				if (quantized_step(candidate, quantized, g_47fe88[step], bits))
				{
					function_11f580(bits, dequantized, (real const *)ranges, candidate);
					if (world_leaf(dequantized) != NONE)
					{
						quantized[0] = candidate[0];
						quantized[1] = candidate[1];
						quantized[2] = candidate[2];
						break;
					}
				}
			}
		}
	}
	for (long i = 0; i < 3; i++)
		function_195720(stream, quantized[i], bits);
}

/* reads a world position */
// @retail 0x86e40
void simulation_read_position(s_bitstream *stream, real *position, long bits)
{
	s_world_bounds_view *world = (s_world_bounds_view *)g_4e0348;
	long quantized[3];
	for (long i = 0; i < 3; i++)
		quantized[i] = function_1959c0(stream, bits);
	function_11f580(bits, position, (real const *)world->bounds, quantized);
}

/* whether two world positions quantize to within one quantum of each other */
// @retail 0x86e90
byte simulation_positions_close(long bits, real const *a, real const *b)
{
	s_world_bounds_view *local_0 = (s_world_bounds_view *)g_4e0348;
	long quantized_a[3];
	long quantized_b[3];
	function_11f4f0(bits, a, local_0->bounds, quantized_a);
	function_11f4f0(bits, b, local_0->bounds, quantized_b);
	if (abs(quantized_a[0] - quantized_b[0]) > 1 || abs(quantized_a[1] - quantized_b[1]) > 1 || abs(quantized_a[2] - quantized_b[2]) > 1)
		return false;
	return true;
}

/* writes a position relative to something, over -10..10 */
// @retail 0x86f10
void simulation_write_relative_position(s_bitstream *stream, long bits, real const *position)
{
	long quantized[3];
	function_11f4f0(bits, position, g_440214, quantized);
	for (long i = 0; i < 3; i++)
		function_195720(stream, quantized[i], bits);
}

/* reads a position relative to something */
#pragma optimize("s", on)
// @retail 0x86f50
void simulation_read_relative_position(long bits, real *position, s_bitstream *stream)
{
	long quantized[3];
	for (long i = 0; i < 3; i++)
		quantized[i] = function_1959c0(stream, bits);
	function_11f580(bits, position, (real const *)g_440214, quantized);
}
#pragma optimize("", on)


/* the machines of a game: a mask and their addresses */
struct s_simulation_machine_list
{
	dword mask;
	byte addresses[16][6];
};

/* writes the machines of a game */
// @retail 0x87c00
void simulation_write_machines(s_simulation_machine_list const *machines, s_bitstream *stream)
{
	stream_write_checked(stream, machines->mask, 16);
	for (long i = 0; i < 16; i++)
	{
		if (machines->mask & (1 << i))
			function_1955d0(stream, machines->addresses[i], 48);
	}
}

/* reads the machines of a game; false when the stream ran out */
// @retail 0x87c80
bool simulation_read_machines(s_bitstream *stream, s_simulation_machine_list *machines)
{
	machines->mask = function_1959c0(stream, 16);
	for (long i = 0; i < 16; i++)
	{
		if (machines->mask & (1 << i))
			function_195820(stream, machines->addresses[i], 48);
		else
			memset(machines->addresses[i], 0, sizeof(machines->addresses[i]));
	}
	if (!stream_overflowed(stream))
		return true;
	return false;
}
