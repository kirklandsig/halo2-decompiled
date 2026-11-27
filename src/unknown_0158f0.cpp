#include "surface_resource_pairs.h"
// @flags /O2 /Gr
#include "palette_cache_storage.h"
#include "palette_storage.h"
/* UNKNOWN_0158F0.CPP: D3D texture and palette creation (the D3D header
   allocation is inlined from the SDK's D3DDevice_CreateTexture2) */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"

namespace D3D
{
	struct CDevice
	{
		void LazySetStateVB(dword value);
		static dword m_PushSegmentSize;
	};

	dword volatile *WINAPI MakeRequestedSpace(dword fallback_size, dword requested_size);
}

// Retail has two 24-entry format rows; unrelated data starts at 0x450828.
// Values map bitmap formats to hardware formats, with -1 for unsupported ones.
long g_450768[2][24] = {
    {0x19, 0, 1, 0x1a, -1, -1, 5, -1, 2, 4, 7, 6,
     -1, -1, 0xc, 0xe, 0xf, 0xb, 0xb, -1, -1, -1, 0x28, 0x28},
    {0x1f, 0x13, 0x1b, 0x20, -1, -1, 0x11, -1, 0x10, 0x1d, 0x1e, 0x12,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x17, 0x17}
};
dword g_43e8a8[12];
long g_43e8d8[6];

s_surface_resource_pairs g_surface_resource_pairs;
D3DResource *g_485ae4;
D3DResource *g_485ae8;
D3DResource *g_485aec;

// @retail 0x14a60
void function_014a60(void)
{
	if (g_509378)
	{
		D3DResource_Release(g_509378);
		g_509378 = NULL;
	}
	if (g_509380)
	{
		D3DResource_Release(g_509380);
		g_509380 = NULL;
	}
	if (g_509374)
	{
		D3DResource_Release(g_509374);
		g_509374 = NULL;
	}
	if (g_50937c)
	{
		D3DResource_Release(g_50937c);
		g_50937c = NULL;
	}
}

// @retail 0x1d2a0
void function_01d2a0(void)
{
	if (g_485ae4)
	{
		D3DResource_Release(g_485ae4);
		g_485ae4 = NULL;
	}
	if (g_485ae8)
	{
		D3DResource_Release(g_485ae8);
		g_485ae8 = NULL;
	}
	if (g_485aec)
	{
		D3DResource_Release(g_485aec);
		g_485aec = NULL;
	}
}

/* the palette tag data: a list of palette groups, each pointing at 256-color
   palettes (0x400 bytes apart) that are loaded into D3D palette headers */
struct s_palette_group
{
	byte unknown00[8];
	long palette_count;
	dword palettes;
	byte unknown10[0x58];
};

struct s_palette_tag_data
{
	byte unknown00[0x80];
	long group_count;
	s_palette_group *groups;
};

/* one entry of the global array at +0x214; the palette tag index is at +0x1c */
struct s_palette_source
{
	byte unknown00[0x1c];
	long tag_index;
	byte unknown20[0x24];
};


long g_484dc0[4];
long g_467000;
long g_5093ac;
dword **g_407488;


// score MATCH (was 21; fast build 24):
// b13B (11-17) walk against retail, fast build on r846: no logic difference. Format is a word (movsx at 0x158f4),
// the flag byte as movzx (0x158f1), the table loads (0x158fe, 0x15909), the pushes into 0x3fde00 (0x1591e-0x1592f),
// the 0x10000 test (0x15936), the header fill (0x1597c-0x159a2) and ret 0x10 match. Caller 0x1d000 sign-extends before
// each push (0x1d0a5-0x1d0b5) and `out` travels in ebx (0x1d0b9), so the declared types stand: no type change.
// Left: the failure tail. Retail makes the NULL in a register (xor ecx,ecx at 0x15971, mov [ebx],ecx at 0x15976) where
// ours stores an immediate (the 2 bytes of size). Same tail in retail 0x15b10 (0x15b4c-0x15b51). Tried, fast scores:
// the store as `*out = texture`, a result flag or pointer, a flag branch, a store helper (24); store before the test,
// `return *out != NULL`, the SDK's HRESULT CreateTexture (35-53, `out` then leaves ebx: ret 0x14); no store on failure
// (23, drops retail's store at 0x15976: behaviour change, rejected).
// b16K (11-17) fast build on r852: the tail as 0x15a60's (b16A below), the result pointer and the flag assigned in
// both arms of one if/else, then one store and one return: 184 bytes, retail's xor ecx,ecx (0x15971) and mov [ebx],ecx
// (0x15976); only the 3 library-call targets differ in the fast image (they differ in its 24 as well). The parameter
// order stands: 0x1d0e0 sets ebx, al, ecx (0x1d0ec-0x1d0f3), last to first in declared order. Caller 0x14850 MATCH.
// @retail 0x158f0
bool function_0158f0(
	short b,
	byte a,
	long width,
	long height,
	long levels,
	long usage_index,
	D3DTexture **out)
{
	D3DTexture *texture = D3DDevice_CreateTexture2(width, height, 1, levels + 1, g_43e8a8[usage_index], (D3DFORMAT)g_450768[a][b], D3DRTYPE_TEXTURE);
	D3DTexture *result;
	bool created;
	if (texture == NULL)
	{
		result = NULL;
		created = false;
	}
	else
	{
		result = texture;
		created = true;
	}
	*out = result;
	return created;
}

// score MATCH (was 21; fast build 24):
// b13B (11-17) walk against retail, fast build on r846: no logic difference. The edge arrives in eax (0x159e0), the
// format in cx (movsx at 0x159c9), usage index and levels are dword stack reads (0x159b0, 0x159d8), the cube type
// and pushes into 0x3fde00 (0x159bd-0x159de), the header fill (0x15a2d-0x15a49) and ret 8 match. Caller 0x1d000 loads
// eax with the sign-extended width and ecx with the format (0x1d06b, 0x1d071): no type change. Left: the failure tail
// (xor ecx,ecx at 0x15a22, mov [ebx],ecx at 0x15a27, ours an immediate store); forms tried are listed at 0x158f0.
// b16K (11-17) fast build on r852: the tail as 0x15a60's, the result pointer and the flag assigned in both arms of one
// if/else, then one store and one return: 169 bytes, only the 3 library-call targets differ in the fast image. The
// parameter order stands: 0x1d0e0 sets ebx, ecx, eax (0x1d12a-0x1d134), last to first in declared order.
// @retail 0x159b0
bool function_0159b0(
	long edge,
	short b,
	long levels,
	long usage_index,
	D3DCubeTexture **out)
{
	D3DCubeTexture *texture = (D3DCubeTexture *)D3DDevice_CreateTexture2(edge, edge, 1, levels + 1, g_43e8a8[usage_index], (D3DFORMAT)g_450768[0][b], D3DRTYPE_CUBETEXTURE);
	D3DCubeTexture *result;
	bool created;
	if (texture == NULL)
	{
		result = NULL;
		created = false;
	}
	else
	{
		result = texture;
		created = true;
	}
	*out = result;
	return created;
}

// score MATCH (was 21; fast build 24):
// b13B (11-17) walk against retail, fast build on r846: no logic difference. The format arrives in ax (movsx at
// 0x15a79), the usage index in ecx (0x15a61), width/height/depth/levels are dword stack reads (0x15a6e-0x15a93), the
// volume type, the header fill (0x15ae2-0x15afe) and ret 0x10 match. Caller 0x1d000 loads ebx/eax/ecx at
// 0x1d093-0x1d097: the declared order and types stand, no change. Left: the failure tail (xor ecx,ecx at 0x15ad7,
// mov [ebx],ecx at 0x15adc, ours an immediate store); forms tried are listed at 0x158f0.
// b16A (11-17) walk against retail, fast build on r852: two real forms, both with retail evidence. (1) Parameter order:
// retail's caller 0x1d0e0 loads ebx, then eax (mov eax,9 at 0x1d112), then ecx (xor ecx,ecx at 0x1d117), and 0x1d000
// does the same (mov ebx,edi 0x1d093, mov eax,ebp 0x1d095, xor ecx,ecx 0x1d097); callers set register arguments last
// to first in declared order, so usage_index (ecx) is declared before the format (eax); the stack words and the
// callee's bytes are unchanged, both callers and the declaration in unknown_01cf50.cpp follow. (2) The tail: retail
// keeps the NULL pointer and the false flag as two values (xor ecx,ecx 0x15ad7, xor al,al 0x15ada, mov [ebx],ecx
// 0x15adc) stored/returned once after an if/else pair, which gives 174 bytes with only the 3 library-call targets
// differing in the fast image (they differ in the 24 as well). Tried: `*out = texture` in the failure branch, success
// branch first (24), flag set inside the test (24), result and flag preset then overridden (31), `return *out != NULL`,
// ternary store (47), bool from the compare then null the pointer (35), flag + store (41); `out` leaves ebx in those.
// 0x158f0, 0x159b0 and 0x15b10 share this tail and were not touched (out of this pair's scope): lead for them.
// @retail 0x15a60
bool function_015a60(
	long usage_index,
	short a,
	long width,
	long height,
	long depth,
	long levels,
	D3DVolumeTexture **out)
{
	D3DVolumeTexture *texture = (D3DVolumeTexture *)D3DDevice_CreateTexture2(width, height, depth, levels + 1, g_43e8a8[usage_index], (D3DFORMAT)g_450768[0][a], D3DRTYPE_VOLUMETEXTURE);
	D3DVolumeTexture *result;
	bool created;
	if (texture == NULL)
	{
		result = NULL;
		created = false;
	}
	else
	{
		result = texture;
		created = true;
	}
	*out = result;
	return created;
}

__declspec(noinline) bool function_015b10(long index, D3DPalette **out);

// score 99 (was 103): D3DDevice_CreatePalette2 stays a call; retail inlines it (b13A below).
// b13A (11-17) walk against retail, fast build on r846: logic matches statement by statement. Retail inlines the SDK's
// D3DDevice_CreatePalette2: alloc 0xc / 0x64800000 (0x15b20), size read from D3D::g_PaletteSize (0x15b2b, its only
// reference: retail holds no out-of-line CreatePalette2), data alloc 0xb6800000 (0x15b38), free 0x24800000 (0x15b47),
// Common = size << 30 | 0x1030001 stored before Data = ptr & 0xfffffff (0x15b63, 0x15b65), no store to Lock; `out`
// arrives in ebx. Here the library's CreatePalette2 stays a call (103; via the IDirect3DDevice8_CreatePalette wrapper
// 86), and writing its body out in this file as a static __forceinline helper (reading the library's
// D3D::g_PaletteSize) gives 20 here and 0x1d3f0 76 -> 64, but DECOMPILING.md says a function that calls the public D3D
// API must call it, so the body still calls D3DDevice_CreatePalette2; why LTCG keeps this one call out of line is open.
// Left: retail keeps the NULL in ecx (xor ecx, ecx at 0x15b4c, mov [ebx], ecx at 0x15b51), ours does pop edi; mov
// [ebx], 0 - the same difference 0x158f0/0x159b0/0x15a60 show with the real inlined body. With the written-out helper:
// body in the function 25, one result variable 23, *out stored before the test 39, a local for the table read 18.
// b16K (11-17) fast build on r852: the tail as 0x15a60's (result and flag assigned in both arms, one store and one
// return) gives 99 (was 103); the rest is still D3DDevice_CreatePalette2 kept out of line, as above.
// GA14 (18 real + 2 diag, fast with 0x14a60..0x15cd0, 0x1d3f0): confirmed 0x15b10 is the sole caller of
// D3DDevice_CreatePalette2 in our build (the library copy, 84 B, 0 data refs); removing __declspec(noinline)
// (inlines 0x15b10 into 0x1d3f0, still calls CreatePalette2), size local, forceinline wrapper, direct return,
// early return, const/enum g_43e8d8, param swap, direct *out, /Ob2, /arch:SSE, /Ox, and diag #pragma inline_depth(255)
// all stay 99 (30 B); diag constant (D3DPALETTESIZE)0 still keeps CreatePalette2 out of line (104, 25 B). Wrappers
// D3DDevice_CreatePalette (86-87), D3DDevice::CreatePalette (81) and IDirect3DDevice8_CreatePalette (76) only lower
// score via spurious HRESULT (0x8007000e) length padding and worsen 0x1d3f0 (77-102), NOT kept. Body restored at 99.
// GB r16: the D3DDevice_CreatePalette2 call wrapped in do { ... } while (0) MATCHES (95/95 B; 0x1d3f0 76 -> 65), not kept:
// a dead loop whose only job is to change the compiler's inlining (the same class as a dead if (0) steer).
// Round 159 (m3 lane 14, fast, 0x1d3f0 in brackets): the call assigned inside the NULL test 99 (76); `return (*out =
// call) != NULL;` 99 (91); the pointer declared in the if condition with two returns 99 (76); a bool from the compare
// and one store 103 (91); the success arm first 99 (76). The SDK call stays out of line in every straight-line form.
// M3 lane 8 r243 (H2M_FAST with 0x1d3f0, its caller; control 99 at 30 B, 0x1d3f0 76): the result and flag preset with
// one `if (palette != NULL)` arm 90 (38 B; 0x1d3f0 80); the palette pointer started NULL and assigned by the call 99
// (76); a bool from the compare and a ternary store 93 (36 B; 0x1d3f0 91); `*out = palette; return palette != NULL;` 99
// (0x1d3f0 91); `*out` stored in each arm of the NULL test 103 (26 B). The SDK call stays out of line in each. Reading:
// the link-time inliner weighs a call by its block's estimated frequency against one link-wide threshold, so only a
// loop around the call (GB r16's dead loop) lifts it over; retail's call sits in the entry block of a function with one
// caller, so retail's threshold or the library body's cost differed from this build's (a link-wide setting, not this
// source).
// GB26 (12 real + 1 flag diag fast trials with 0x1d3f0 and file peers; control 99 at 30 B, 0x1d3f0 76): `extern const`
// `g_43e8d8` with retail values {3,2,1,0,32,64} 99 (0x1d3f0 76), + no `noinline` 99 (0x1d3f0 75); `result = palette;
// created = true; if (!palette)` 99; `bool created` branch `*out` 93 (36 B, 0x1d3f0 91); `*out = NULL` before call 99
// (+ no `noinline` 99, 0x1d3f0 110); `if (D3DPalette *palette = ...)` 99; ternary `*out` 99 (0x1d3f0 91); no `noinline`
// + direct return 99 (0x1d3f0 69); `dest = out` alias 99; `*(g_43e8d8 + index)` 99; 4-case `switch` on size 40 (95 B,
// jump table still calling `CreatePalette2` out of line, 0x1d3f0 76); diag `/O1` 94 (35 B, 0x1d3f0 91). Body unchanged.
// @retail 0x15b10
bool function_015b10(
	long index,
	D3DPalette **out)
{
	D3DPalette *palette = D3DDevice_CreatePalette2((D3DPALETTESIZE)g_43e8d8[index]);
	D3DPalette *result;
	bool created;
	if (palette == NULL)
	{
		result = NULL;
		created = false;
	}
	else
	{
		result = palette;
		created = true;
	}
	*out = result;
	return created;
}

// score 56 (was 56): retail keeps the palette count in a register through both loops and stores it to g_5093ac
// once after the outer loop (the zero-trip path skips that store), compares the group count in memory against the
// zero register, and keeps the palette definition in EAX (spilled, reloaded after the data store) with 0x30001 in EBP.
// Lead: the count written as g_5093ac itself (no local, `g_5093ac++` in the inner arm, no copy after the loop) gives
// all of that structure, 56 at 280 B with only register choice left (ours: EBP definition, EBX zero then 0x30001, ESI
// count, EDI NONE then index; retail: EAX definition, EBP 0x30001, EDI zero and count, EBX NONE then index, ESI the
// 0x400 step). On it the allocator ranks the palette pointer (288), the group (152) and the 0x400 step (96) above the
// tag index (48); a forced-ranking search gives a byte match with the tag index ranked first (the definition then
// claims EAX before the loop values pick) and with no other single decision. Forms on the global count: the definition
// read through the tag table at each use 86 (283 B, reloaded every outer pass); the group pointer local dropped 97
// (259 B); the palette pointer local dropped 65; the two global stores ahead of the tag read 56 (same code). With the
// local count: the definition read at each use 79 (299 B).
// M3 lane 8 r243 (H2M_FAST with 0x15b10, 0x15c90 and 0x15cd0; control 56): an early return on the NONE tag 56 (same
// code); the global-count form with the palette as `g_484dd0 + g_5093ac` 56, with the step as `i << 10` 56, with a
// walking palette-data local stepped by 0x400 61; the group as `groups + group_index` 56; the local count declared at
// the global's zero store 56 (the global form's code); the definition declared at its load 56. Allocator reading
// (global form): the inner loop body (weight 4, eight candidates referenced) gives the palette pointer 256 of its 288;
// the tag index's two blocks give it 48; the definition's first web is never coloured (-29, live through the inner loop
// unreferenced), so its split part takes the one register left (EBP) where retail's takes EAX. With no tag-index local
// (the two global stores first, the source's tag index read in the test and again for the definition) 56 on the local
// count and 56 on the global count.
// M3 GB r23 (30 real + 3 flag diag fast trials with 0x15c90/0x15cd0/0x14a60/0x1d2a0 MATCH; control 56 at 280 B): on
// global-count (56, al=59, mk=5), store orders Common/Data/Lock 56, Data/Lock/Common 56, Common/Lock/Data 65,
// Lock/Data/Common 65, Lock/Common/Data 65; early return / scoped decls / continue bounds / inline helpers / source
// ptr / top decls / rename locals / dword*256 / /Ob1 / /arch:SSE / /Oi all 56; reverse bounds 57; stores before loop1
// 76; D3DPalette_Register 113; (word)tag_index 65; slot local 67; hoisted base 61; no-ptr Common first 65; post-inc
// `&g_484dd0[g_5093ac++]` 86 (285 B, al=27; Common first 84, al=26; flips all registers to retail's exact EAX/ECX/EDX/
// ESI/EBX/EDI layout but adds early pointer copy); inc before Lock 83 / after Data 84; local palette_total with
// `if (group_count > 0)` 56, store in outer 67, store in inner 63, post-inc 87. Body unchanged at 56.
// GB26 (27 fast trials; control 56 at 280 B): 13 global-count forms (`D3DResource *`, `while` loops, a `dword` stage,
// post-inc/pre-inc, nested/pointer bounds, a walking palette pointer, `if (group_count > 0)`, a temporary `D3DPalette`,
// `const` pointers) 56-99. Calling the SDK's palette-header setter (the 256-entry size, the palette slot, the masked
// palette address) in place of the three header stores matches byte for byte (280/280 B, plain-confirmed, seven
// variants; unmasked 50; a `palette_total` local 3; post-inc 71; `D3DPalette_Register` 82), held as a patch: linking
// that SDK routine into the image flips the matched 0x24cf66 (src/unknown_24c7c1.cpp, 0 -> 2, two loads swapped at
// +0x3bd) in all seven variants.
// @retail 0x15b70
void function_015b70(void)
{
	long stage;
	long group_index;
	long palette_total = 0;
	s_palette_tag_data *field_4_7;

	for (stage = 0; stage < 4; stage++)
	{
		D3DDevice_SetPalette(stage, g_484dbc);
		g_484dc0[stage] = NONE;
	}

	long tag_index = g_4e0350->sources[g_4686c4].tag_index;
	g_467000 = NONE;
	g_5093ac = 0;
	if (tag_index != NONE)
	{
		field_4_7 = g_4e3b44[tag_index & 0xffff].palette;
		for (group_index = 0; group_index < field_4_7->group_count; group_index++)
		{
			s_palette_group *group = &field_4_7->groups[group_index];
			for (long i = 0; i < group->palette_count; i++)
			{
				if (palette_total >= 0 && palette_total < 32)
				{
					D3DPalette *palette = &g_484dd0[palette_total];
					palette->Data = (group->palettes + i * 0x400) & 0x7fffffff;
					palette->Common = 0x30001;
					((D3DResource *)palette)->Lock = 0;
					palette_total++;
				}
			}
		}
		g_5093ac = palette_total;
	}
}

// @retail 0x15c90
void function_015c90(long stage, long index)
{
	if (index != g_467000 && index >= 0 && index < g_5093ac && index != g_484dc0[stage])
	{
		D3DDevice_SetPalette(stage, &g_484dd0[index]);
		g_484dc0[stage] = index;
		g_467000 = index;
	}
}

// @retail 0x15cd0
dword function_015cd0(long index, dword entry)
{
	dword result = 0xff4060;

	if (index >= 0 && index < g_5093ac)
	{
		dword *palette = (dword *)(g_484dd0[index].Data | 0x80000000);
		if (palette)
		{
			result = palette[entry & 0xff];
		}
	}
	return result;
}

s_palette_storage g_palette_storage;

s_palette_cache_storage g_palette_cache_storage;
