// @flags /O2 /Ob1 /Gr
/* UNKNOWN_01E930.CPP: Bink movie playback

Bink allocates through function_156710 and
function_156810, which video_playback_setup (unknown_155ea0.cpp)
registers: the allocations come from a permanent block of physical memory
and are tracked in g_4e9148. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_01e930.h"
#include "unknown_03d380.h"
#include "physical_memory.h"
#include <xtl.h>
#include <d3d8.h>
#include <string.h>

enum
{
	k_maximum_bink_allocations = 16,
	k_bink_allocation_slack = 0x3000
};

/* the outstanding Bink allocations and their count */
void *g_4e9148[k_maximum_bink_allocations];
long g_510c64;
/* the available physical memory, in kilobytes */
long g_47ff80 = NONE;
dword g_51ebec;
extern dword g_54d5b8;
extern byte g_4e6388;

void __stdcall function_18f1c0(long a);
void function_12d520(long a);
bool function_12b3c0(void);
bool function_23e400(long button);
void function_156960(void);
void __stdcall function_35b90(void *material);
void __stdcall function_363a0(void *vertices);
void __stdcall function_1e930(long a);
int __stdcall function_3e2330(void *movie);
int __stdcall function_3e2870(void *movie);
void __stdcall function_3e2e50(void *movie);
int __stdcall function_3e2830(void *movie, void *destination, long pitch, long height, long x, long y, dword flags);

byte g_4c1a18;

/* the texture cache's physical memory (unknown_12c0d0.cpp) */
extern s_physical_object *g_4e6464;
byte g_4e6389;

/* the sound settings Bink plays through (unknown_01e930.h) */
s_bink_sound_settings *g_51ebe4;

/* the bitmap the movie's texture is drawn as */
struct s_bink_bitmap
{
	dword signature;
	short width;
	short height;
	byte depth;
	byte unknown09;
	short unknown0a;
	short format;
	short flags;
	byte unknown10[0x28 - 0x10];
	long unknown28;
	byte unknown2c[0x34 - 0x2c];
	long size;
	byte unknown38[0x4c - 0x38];
	long unknown4c;
	void *texture;
	long unknown54;
};

s_bink_bitmap g_55ece8;

long __stdcall function_12d2f0(long a, long b, long c, long d);
void function_12c600(void);
double timing_ticks_to_seconds(__int64 ticks);
bool function_2148b0(real *a);
struct D3DTexture *function_23e340(short width, short height, short format,
	void *(__stdcall *allocate)(long size, long alignment), long *size, void **data, struct D3DTexture *header);
short function_1358c0(short format);
void __stdcall function_3e0450(void *open, void *direct_sound);
void *__stdcall function_3e0af0(char const *name, dword flags);
int __stdcall function_3e2630(void *movie, dword track, dword *bins, dword count);
int __stdcall function_3e2680(void *movie, dword track, dword *bins, long *volumes, dword count);
void *__stdcall function_3e3c90(void *a);

/* the shared body of function_155e50, which retail inlines into
   the memory callbacks */
PRIVATE inline void bink_update_memory_available(void)
{
	MEMORYSTATUS status;

	memset(&status, 0, sizeof(status));
	status.dwLength = sizeof(status);
	GlobalMemoryStatus(&status);
	g_47ff80 = status.dwAvailPhys >> 10;
}

// @retail 0x155e50
void function_155e50(void)
{
	bink_update_memory_available();
}

// @retail 0x155f60
bool function_155f60(void)
{
	return g_4e9188.movie && g_4e9188.initialized;
}

// @retail 0x1565e0
void function_1565e0(void)
{
	if (g_4e9188.initialized && g_4e9188.permanent_memory)
	{
		function_12d520((long)g_4e9188.permanent_memory);
		g_4e9188.permanent_memory = NULL;
		g_4e9188.texture = NULL;
		g_4e9188.permanent_memory_size = 0;
	}
}

// @retail 0x156560
void function_156560(void)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = NULL;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = false;
			g_4e6388 = 0;
		}
	}
}

/* a lifecycle callback with the body of function_156560 */
// @retail 0x1566c0
void __stdcall function_1566c0(dword a, dword b)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = NULL;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = false;
			g_4e6388 = 0;
		}
	}
}

// @retail 0x1565a0
void bink_playback_end(void)
{
	if (g_4e9188.initialized)
	{
		function_156560();
		if (g_4e9188.flags & 0x20)
			function_18f1c0(0);
		g_4e9188.flags = 0;
		g_51ebec = g_54d5b8;
	}
}

// @retail 0x155ed0
void function_155ed0(void)
{
	if (g_4e9188.initialized)
	{
		function_156560();
		if (g_4e9188.flags & 0x20)
			function_18f1c0(0);
		g_51ebec = g_54d5b8;
		memset(&g_4e9188, 0, sizeof(g_4e9188));
	}
}

/* takes size bytes, aligned, from the top of the permanent block (the texture
   allocator 0x23e340 calls) */
// @retail 0x156620
void *__stdcall function_156620(long size, long alignment)
{
	byte *result = g_4e9188.permanent_memory + (g_4e9188.permanent_memory_size - size);

	if (alignment)
	{
		unsigned long mask = (unsigned long)alignment - 1;

		if ((long)result & mask)
		{
			size += (alignment - (long)result) & mask;
			result -= (alignment - 1) & (alignment - (long)result);
		}
	}
	g_4e9188.permanent_memory_size -= size;
	return result;
}

// @retail 0x156690
bool function_156690(void)
{
	bool result = true;

	for (short i = 0; i < g_510c64; i++)
	{
		if (g_4e9148[i])
			result = false;
	}
	return result;
}

// @retail 0x156710
void *__stdcall function_156710(unsigned long size)
{
	void *result = NULL;

	bink_update_memory_available();

	if (g_510c64 > 0 && !g_4e9148[0] && function_156690())
	{
		g_510c64 = 0;
		g_4e9188.permanent_memory_used = 0;
	}

	if (g_4e9188.permanent_memory_used + size <= (unsigned long)g_4e9188.permanent_memory_size &&
		g_510c64 < k_maximum_bink_allocations && g_4e9188.permanent_memory)
	{
		result = g_4e9188.permanent_memory + g_4e9188.permanent_memory_used;
		g_4e9188.permanent_memory_used += size;
		XPhysicalProtect(result, size, PAGE_READWRITE);
		g_4e9148[g_510c64++] = result;
		function_155e50();
		g_4e9188.permanent_memory_used += k_bink_allocation_slack;
		if (g_4e9188.permanent_memory_used > g_4e9188.permanent_memory_size)
			g_4e9188.permanent_memory_used = g_4e9188.permanent_memory_size;
	}
	return result;
}

// @retail 0x156810
void __stdcall function_156810(void *block)
{
	bink_update_memory_available();
	for (long i = 0; i < g_510c64; i++)
	{
		if (g_4e9148[i] == block)
		{
			g_4e9148[i] = NULL;
			break;
		}
	}
	bink_update_memory_available();
}

// @retail 0x156ab0
bool function_156ab0(void)
{
	long buttons[] = { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13 };
	bool result = false;

	for (dword i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
	{
		if (function_23e400(buttons[i]))
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x1568d0
void function_1568d0(void)
{
	D3DLOCKED_RECT locked;

	function_3e2870(g_4e9188.movie);
	function_3e2e50(g_4e9188.movie);
	g_4e9188.texture->LockRect(0, &locked, NULL, 0);
	if (locked.pBits)
		function_3e2830(g_4e9188.movie, locked.pBits, locked.Pitch, g_4e9188.height, 0, 0, g_4e9188.copy_flags | 0x80000000);
}

// @retail 0x155f80
void function_155f80(bool synchronous)
{
	if (g_4e9188.initialized && g_4e9188.movie)
	{
		if (function_12b3c0())
		{
			if (!synchronous)
				g_4e9188.unknown02 = !function_3e2330(g_4e9188.movie);
		}
		else if (synchronous)
		{
			while (function_3e2330(g_4e9188.movie))
				;
			g_4e9188.unknown02 = true;
		}

		if ((g_4e9188.flags & 2) && (!(g_4e9188.flags & 0x100) || g_4e9188.unknown03) && function_156ab0() ||
			g_4e9188.finished)
			bink_playback_end();

		s_bink_movie *movie = (s_bink_movie *)g_4e9188.movie;
		if ((!movie || movie->frame_number == movie->frames) && !(g_4e9188.flags & 1))
			g_4e9188.finished = true;
	}
}

// @retail 0x156040
void function_156040(void)
{
	if (g_4e9188.initialized && g_4e9188.movie)
	{
		if (function_12b3c0())
			g_4e9188.unknown02 = true;
		if (g_4e9188.unknown02)
		{
			function_1568d0();
			g_4e9188.unknown02 = false;
		}
		function_156960();
		function_155f80(true);
	}
}

PRIVATE inline __int64 bink_read_tsc(void)
{
	volatile __int64 t = 0;
	__asm rdtsc
}
// score 244 (was 244): frame 0x3c versus 0x40, allocation homes and SSE scheduling differ.
// MiniMax S (10-31): no new form, no lower score. Retail zeroes its homes with ebx after an early
// `xor ebx, ebx` and builds the allocation size as ((flags & 0x100) | 0x40) << 16; ours derives the same
// size through a `large` bool and a ternary and zeroes the homes with eax. Our frame is one dword short.
// the retry ordinal as a function-scope local 239 (MiniMax E run 12; under the keep bar, restored).
// Round 231: shared retry helper/native byte block/both/shared audio arrays/helper-arrays/byte-
// arrays/all/Boolean large-block selection 313/287/313/339/364/339/364/347; separate noinline
// format-size query declaration 287. Original body and declaration restored.
// Frame 0x3c versus 0x40, timestamp and output homes,
// retry expansion, float comparison and material scheduling still differ.
// Earlier retry/native block/shared audio forms 313/287/313/339/364/339/364/347;
// format-size noinline 287. Native Boolean allocation choice 256 (plain).
// Round 219: early texture outputs, output names, timestamp names and both names
// 287 each; earlier large decision 286 (rejected); earlier size plus decision 295;
// common void exit 287; SSE ownership original/Boolean 287/266, restored.
// Round 212: allocation result first 244 (plain kept); retry ordinal/clock declaration
// first 256/256; result+ordinal/result+clock/ordinal+clock/all 260/244/256/260;
// direct skip result 286; full-width mask/Boolean decisions 289/290, with result
// first 273/274. The frame remains 0x3c versus 0x40.
// Round 223: two-use sound-interface value 274; allocation-size branches 256;
// both 274; consistent movie-global reads 256; sound/movie 274; size/movie 256;
// all three 274; per-branch volume arrays before channel arrays 256. All restored.
// Round 165: Skip-result byte/short/long/dword at original scope 244/274/274/274; entry true
// initialization 258/287/287/287. SSE policy diagnostic 250, restored. Frame 0x3c versus 0x40
// and clock/output homes remain; original body, helpers and flags restored.
// Round 246: complementary small-allocation bool/byte/short/long 270/270/289/289; the same
// parameter-derived decision at entry 306/306/305/298. Original large decision restored.
// Round 225: native long/dword allocation result, with ordinary/role-named audio arrays,
// early/late zero lifetime: 244/244/256/256/244/244/256/256. Original body restored;
// frame 0x3c, clock/output homes and instruction policy remain.
// Round 251: bounded retry short/word/byte/dword carriers 263/263/259/245;
// the same four with a scoped timeout else phase 263/263/259/245. Frame stays
// 0x3c; original ordinal, continue graph and full executable body restored.
// Round 212: native status zero/length aggregate initialization, original/entry texture outputs
// and bins/volumes declaration orders [286, 286, 286, 286, 285, 285, 285, 285]. Original shared
// producer and body restored; frame and clock/output homes remain.
// Round 217: initialized texture size/data/both at native phase 246/246/249;
// the same private output defaults at entry 284/284/295; both outputs in reverse
// declaration order at native/entry phase 249/295. Original body restored.
// Round 269: Successful allocation/movie/texture phase nesting, seven subsets,
// and all with post-protection acceptance nested: 245 each at 1104 bytes.
// Original early cleanup returns, helpers and full body restored; frame stays 0x3c.
// Timestamp producer original with inclusive/selected clamp 245/244; forced
// with original/inclusive/selected clamp 244/245/244; outlined with all three
// clamps 310 each. Original helper contract, clamp and full body restored.
// Round 274: Common false-default skippability across both admission tests, bool/byte/short/long
// at the original phase 267/267/276/276, at entry 285/285/287/287. Original result,
// allocation/clock/audio/output phases and full body restored; frame remains 0x3c.
// Separate native constant audio arrays at entry: surround bins/volumes, stereo bins/volumes,
// surround pair/stereo pair/both bins/all 290/300/289/299/307/296/297/306. All restored.
// Round 411: Allocation result produced on both admission arms, void/byte pointers and
// long/dword carriers with entry/post-clock declarations, all eight 285; each carrier
// with false-default skippability at admitted/entry phase, all eight 281. All are 1098
// bytes with frame 0x3c; original entry allocation zero and complete body restored.
// Round 214: native start/elapsed/both clock records at original or entry declaration phases,
// including independent mixed phases, 244 each in eight forms. Exact-only records restored;
// frame 0x3c/0x40 and status/clock/output homes and instruction policy remain.
// Round 431: Allocation-clock/memory-decoder/texture-output consumption scopes, seven subsets; all with
// read-only movie input: 244/244/244/244/244/244/244/244. Original consumption scopes, helpers and
// complete executable body restored.
// Round 21 (lane 13): retail builds this body with SSE (movss stores of 1.0f, `fld; fcompi` for the
// timeout) while this file has no /arch:SSE; no other function here has float code. Retail's size is
// `((flags & 0x100) | 0x40) << 16` with the mask spilled to a dword slot (the 0x40 frame) and the
// start time's low half in EDI. `dword large = flags & 0x100` 273; the mask test written at both
// uses (retail's or/shl size) 269; the allocation result declared after the start time 256; both
// 287; `(real)` on the seconds before the 0.1f compare 244 (float compare, still fcomp/fnstsw).
// Round 104: retail is SSE code (movss stores, fcompi), so the body belongs with src/unknown_1564e0.cpp's SSE
// Bink functions (moved there, with the memory helper in unknown_01e930.h). There: the ternary testing
// `(flags & 0x100)` itself gives retail's and/or 0x40/shl 0x10 (a `large` local gives neg/sbb); the movie open as
// an if/else with a call per arm gives retail's branch and spills the mask; `size` read back from
// permanent_memory_size after the three stores gives retail's ebp size/esi block/edi start; the retry loop as a
// do/while is unrotated (for (;;) and do-while(true) rotate); `!((real)timing_ticks_to_seconds(elapsed) < 0.1f)`
// gives fld dword/fcompi/jbe; `function_1358c0(g_55ece8.format)` stays a call; the success path inside a positive
// last texture test puts the shared failure exit last; one bool for both loading queries gives cmp al, bl.
// Together aligned 109 -> 74 but 322 at 1061 bytes: frame 0x44 (retail packs start with the first clock temporary
// and the second clock temporary with the memory status, ours the other way), two zero stores to the status slot
// at entry unexplained, D3DDevice_IsBusy out of line in every unrotated loop form, the size constant 10 in ebp.
// Lane 18 round 6: the calls line up by position through SwitchToThread; after it ours trails by 13 instructions
// (the rotated loop's second allocation call and the failure exit placed after the loop). Size query called with
// `g_55ece8.format` (stays a call, 10 not in EBP) 245; that plus long width/height in 0x23e340 and its
// declaration 252; do/while retry loop 315 (1061 bytes, unrotated, D3DDevice_IsBusy a call); the same with the
// attempts branch ending in `continue` 315; do/while plus the format-field query 311. Original body restored.
// Round 109 (lane 6, fast; the body re-checked against retail field by field, no value difference): the size as
// `(flags & 0x100) ? ...` with the movie flags still from `large` 275; both through the mask, no `large` 269; the
// retry counter declared after the block pointer 260, before it 260; `!(seconds < 0.1f)` 244 (same code). The x87
// `fcomp qword`/fnstsw compare and the 1.0f stores stay non-SSE while the file has no /arch:SSE.
// Round 7 (lane 17, fast, in this file): the retry loop as `do { block = ...; if (!block) { retry or timeout
// arm } } while (!block);` 315 at 1061 bytes (aligned 109 -> 123); the movie open as an if/else with a call
// per arm 251 (aligned 120); the format's bits read as `function_1358c0(g_55ece8.format)` 245 at 1100 bytes
// (aligned 109 -> 96: the call stays out of line as retail's, where `function_1358c0(10)` folds to a table
// load; lead); that with the sound interface read once into a local 251 (aligned 107), with
// `dword large = flags & 0x100` 278 (aligned 99).
// Round 9 (lane 17, fast; aligned base 109): the mask tested at both uses with the movie open as an if/else
// with a call per arm 278 (1102 bytes, frame 0x44, aligned 136); that with the format-field size query 286
// (1112 bytes, aligned 119); a dword mask local for both uses with the per-arm open 283 (frame 0x44); a dword
// mask local for the open and the size ternary on `flags & 0x100` 269 (frame 0x3c, aligned 111; diagnostic, a
// local/direct mix), with the per-arm open 278. Retail's movss 1.0 stores and fld/fcompi timeout compare need
// /arch:SSE, which this file's flags lack (build flags are the user's decision).
// Round 256: Shared width/height/both short locals 255/254/257; promoted long locals 256/255/259;
// both short/long at entry 257/259. Original dimension reads and complete body restored; frame
// 0x3c and clock/output homes remain.
// Round 475: Plain surround-volume/stereo-volume/stereo-bin producer subsets
// 1/2/4/3/5/6/7: 254/243/262/245/264/243/245; all with adjacent material loops 297.
// Stereo volume loop 243 at 1098 bytes; both stereo loops 243 at 1099: one-point
// leads below threshold. Entire body restored; frame 0x3c versus 0x40 remains.
// Round 49 (lane 11, fast): the first loading query in a `bool loading` local 239 (its test becomes retail's
// `cmp al, bl`); the same local also holding the second query 238 (both tests retail's; 6 points, under the keep
// bar); skippable as `!query` 273; on the 238 form: size and movie flags tested on `flags & 0x100` directly 267
// (retail's and/or/shl size, but the mask lands in EDI where retail spills it to the 0x40 frame's extra dword);
// `dword large = flags & 0x100` 271 (1102 bytes); the retry counter at function scope 260; the format-field size
// query 245 (1100 bytes); the allocation result declared after the start time 256 (the last two are the listed
// scores of those forms alone, so the loading local's gain cancels in them). Lead: the loading local is retail's
// for both queries, so it may count once the frame's mask slot is found.
// @retail 0x156090
void function_156090(char const *name, dword flags)
{
	void *block = NULL;

	bink_update_memory_available();

	if (!g_4e9188.initialized)
		return;
	if (function_2148b0(0) && !(flags & 0x100))
		return;

	g_4e9188.finished = false;
	if (flags & 0x200)
	{
		g_4e9188.unknown03 = false;
	}
	else
	{
		bool skippable = true;

		if (function_2148b0(0))
			skippable = false;
		g_4e9188.unknown03 = skippable;
	}

	bool large = (flags & 0x100) != 0;
	long size = large ? 0x1400000 : 0x400000;

	g_4e9188.permanent_memory_size = size;
	g_4e9188.permanent_memory_used = 0;
	g_4e9188.permanent_memory = NULL;

	__int64 start = bink_read_tsc();

	if (size > 0 && g_4e6464->page_count > 0)
	{
		long attempts = 0;

		while (!(block = (void *)function_12d2f0(size, 0, 0, (long)function_1566c0)))
		{
			if (attempts < 30)
			{
				attempts++;
				function_12c600();
				continue;
			}

			__int64 elapsed = bink_read_tsc() - start;
			if (elapsed < 0)
				elapsed = 0;
			if (timing_ticks_to_seconds(elapsed) >= 0.1f)
				break;
			D3DDevice_KickPushBuffer();
			D3DDevice_IsBusy();
			SwitchToThread();
		}
	}

	g_4e9188.permanent_memory = (byte *)block;
	if (!block)
	{
		function_1565e0();
		return;
	}

	XPhysicalProtect(block, g_4e9188.permanent_memory_size, PAGE_READONLY);
	function_155e50();
	if (g_51ebe4->direct_sound)
	{
		function_155e50();
		function_3e0450(function_3e3c90, g_51ebe4->direct_sound);
		function_155e50();
	}

	g_4e9188.movie = function_3e0af0(name, large ? 0x8002000 : 0);
	function_155e50();
	if (!g_4e9188.movie)
	{
		function_1565e0();
		return;
	}

	if (g_51ebe4->surround)
	{
		dword bins[4] = { 0, 1, 4, 5 };
		long volumes[4] = { 0x8000, 0x8000, 0x5472, 0x5472 };

		function_3e2630(g_4e9188.movie, 0, bins, 4);
		function_3e2680(g_4e9188.movie, 0, bins, volumes, 4);
	}
	else
	{
		dword bins[2] = { 0, 1 };
		long volumes[2] = { 0x8000, 0x8000 };

		function_3e2630(g_4e9188.movie, 0, bins, 2);
		function_3e2680(g_4e9188.movie, 0, bins, volumes, 2);
	}

	s_bink_movie *movie = (s_bink_movie *)g_4e9188.movie;
	long texture_size;
	void *texture_data;

	g_4e9188.width = (short)movie->width;
	g_4e9188.height = (short)movie->height;
	g_4e9188.copy_flags = 3;
	g_4e9188.texture = function_23e340(g_4e9188.width, g_4e9188.height, 10, function_156620, &texture_size, &texture_data,
		(struct D3DTexture *)g_4e9188.texture_header);
	if (!g_4e9188.texture)
	{
		function_1565e0();
		return;
	}

	function_155e50();
	XPhysicalProtect(texture_data, texture_size, PAGE_READWRITE | PAGE_WRITECOMBINE);
	function_155e50();
	if (!g_4e9188.texture)
	{
		function_1565e0();
		return;
	}

	memset(g_4e9188.material, 0, sizeof(g_4e9188.material));
	((real *)g_4e9188.material)[0x10] = 1.0f;
	((real *)g_4e9188.material)[0x11] = 1.0f;
	((real *)g_4e9188.material)[0xa] = 1.0f;
	((real *)g_4e9188.material)[0xb] = 1.0f;
	((long *)g_4e9188.material)[0] = 0;
	g_4e9188.material[0x96] = 0;
	*(short *)&g_4e9188.material[0x94] = 7;

	g_55ece8.signature = 'bitm';
	g_55ece8.width = g_4e9188.width;
	g_55ece8.height = g_4e9188.height;
	g_55ece8.depth = 1;
	g_55ece8.unknown0a = 0;
	g_55ece8.format = 10;
	g_55ece8.flags = 0x10;
	g_55ece8.size = function_1358c0(10) * g_4e9188.height * g_4e9188.width / 8;
	g_55ece8.unknown4c = NONE;
	g_55ece8.unknown28 = NONE;
	g_55ece8.unknown54 = NONE;
	g_55ece8.texture = g_4e9188.texture;
	*(s_bink_bitmap **)&g_4e9188.material[0xc] = &g_55ece8;
	g_4e9188.flags = flags;
	if (flags & 4)
	{
		function_155e50();
		function_155e50();
	}
	function_1568d0();
	g_4e6389 = 0;
	g_4e9188.flag1 = true;
	g_4e6388 = 1;
}
