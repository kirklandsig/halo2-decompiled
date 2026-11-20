#include "render_source_storage.h"
// @flags /O2 /arch:SSE /Gr
#include "material_records_storage.h"
/* UNKNOWN_03FDF0.CPP: cached light geometry and material pass dispatch */

#include "unknown_11c920.h"
#include "visibility_storage.h"
#include "view_context_storage.h"
#include "globals.h"
#include "index_cache_storage.h"
#include "unknown_0494b0.h"
#include "selected_source_pass.h"
#include <string.h>
#include <xtl.h>

struct s_planar_camera_source;
struct s_planar_camera;
struct s_timed_effect_globals;
void function_3d600(void);
void function_441b0(s_planar_camera_source const *source, s_planar_camera *state, byte const *view);
s_timed_effect_globals *function_01fd20(s_timed_effect_globals *result);
s_bit_vector_pool *function_1320f0(long mode, long pass, bool enabled, long extra,
	byte const *camera, byte const *plane, real distance, void *filter, real offset, long identifier);
void __stdcall function_2cbc0(bool enabled);
void function_44720(void);

// @retail 0x3f2c0
void function_3f2c0(s_planar_camera_source const *source, byte const *view, long object_index, long identifier)
{
	long const *identifier_reference = &identifier;
	long identifier_copy = *identifier_reference;
	(void)identifier_copy;
	byte camera[0x1bc]; // a whole s_planar_camera: function_441b0 fills 0x1bc bytes
	real offset = 0.0f;
	function_441b0(source, (s_planar_camera *)camera, view);
	byte *effect = (byte *)function_01fd20(0);
	bool enabled = effect && effect[0x3c];
	if (enabled)
	{
		offset = *(real *)(effect + 0x40) - *(real *)(camera + 0x70);
	}
	function_3d600();
	s_bit_vector_pool *data = function_1320f0(0, 1, enabled, 0, camera,
		camera + 0x60, 1024.0f, NULL, offset, g_4b9ee4);
	if (object_index != NONE)
	{
		struct s_entry_list_view
		{
			long maximum;
			word count;
			short *shorts_b;
			long *longs_a;
			long *longs_c;
			short *shorts_d;
		};
		s_entry_list_view *list = (s_entry_list_view *)data->lists[2];
		bool found = false;
		for (long i = 0; i < (short)list->count && !found; ++i)
			found = list->longs_a[(short)i] == object_index;
		if (!found && list->count < list->maximum - 1)
		{
			list->longs_a[list->count] = object_index;
			list->shorts_b[list->count] = 0;
			list->longs_c[list->count] = 0;
			list->shorts_d[list->count] = NONE;
			++list->count;
		}
	}
	function_2cbc0(true);
	function_44720();
}

struct s_record_source;
struct s_cache_record;
struct s_cache_record_state
{
    long count;
    dword unknown04;
    s_cache_record *records;
    void *buffer;
    byte unknown10;
    bool available;
    byte unknown12[2];
};
struct s_render_part_context
{
    long material;
    point3f position;
    real radius;
    byte active;
    byte unknown15[3];
    real values[4];
    real priority;
};
struct s_light_cone_ab { real values[5]; };
struct s_light_shape_ab
{
    long kind;
    union { struct { real radius_a, radius_b; } sphere; s_light_cone_ab cone; };
    bool ready;
    byte unknown19[3];
    vector3f left;
    real origin_offset;
    point3f origin;
    union
    {
        struct { real unused, radius_a, radius_b, radius; } sphere_render;
        struct
        {
            real unused;
            vector3f direction;
            real distance;
            real slope_x, slope_y, aspect;
            real width, height;
            real near_distance, far_distance;
            real far_width, far_height;
            point3f endpoint;
        } cone_render;
    };
};

/* The first cached record block precedes the existing metadata array.
   Later blocks occupy the preceding metadata entry's values storage. */

extern s_cache_record_state g_4b6280;
extern s_record_pool *g_4e030c;
extern dword g_4b843c;
extern long g_50943c, g_509440;
extern long g_467130, g_4858b4;
extern void *g_467134;
extern s_record_sources *g_467138;
extern byte g_485a75, g_485a76, g_4858bc;
extern long g_485898, g_4858b8;
extern real g_485adc, g_485ae0;
extern vector3f g_485950;
extern bool g_4ba022;
byte g_4ba018;

__declspec(noinline) void *function_449e0(short index, bool instance, bool load);
bool function_45ce0(short index, byte *out, short part, short transform_index);
bool function_460d0(dword const *mask, short index, long part_index);
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);
void function_14bc0(short target, short element, bool depth);
void function_15680(short mode);
void function_15370(short mode);
void function_44550(void);
void function_445d0(void);
void function_0496f0(void);
void __stdcall function_14b60(dword flags, dword color, real depth, byte stencil);
bool __stdcall function_44370(long mode);
void __stdcall function_174220(bool enabled);
bool function_c17f0(long light_index, s_light_shape_ab *shape, bool respect_engine);
void function_4a1e0(s_view_source *source, s_view_camera *camera, long resource_index,
    s_view_flags *flags, real unused_scale, bool enabled, real scale);

/* Retail keeps `first` in a frame slot of its own (shared later with the material) and gives the entry pointer the
   dead mode parameter's home: below the loop guard the entry pointer starts ahead of the index. So the entries are
   walked through a pointer stepped beside the counter, and the guard that repeats the loop's first test keeps the
   pointer's start below that test (a steering form, kept for the byte match; `&entries[n]` in a plain `for` leaves
   `first` in the mode parameter's home, two rows off). */
// @retail 0x402b0
void function_402b0(long cache_index, long mode)
{
    g_4c1a48[1].count = 0;
    g_4c1a48[1].first = 0;
    g_4c1a48[1].flags = 0;
    g_4c1a48[2].count = 0;
    g_4c1a48[2].first = 0;
    g_4c1a48[2].flags = 0;
    if (g_4c62f8.records.blocks[cache_index + 1].count > 0)
    {
        long first = g_4c0b78.current = g_4c0b78.previous;
        memcpy(g_4ba138 + g_4c0b78.current, g_4c62f8.records.blocks[cache_index + 1].entries,
            g_4c62f8.records.blocks[cache_index + 1].count * sizeof(s_44940_entry));
        g_4c0b78.current += g_4c62f8.records.blocks[cache_index + 1].count;
        dword selection = !mode ? ~0UL : mode == 2 ? 0x10 : 0x20;

        long n = 0;
        if (n < g_4c62f8.records.blocks[cache_index + 1].count)
        {
            for (s_44940_entry const *entry = g_4c62f8.records.blocks[cache_index + 1].entries;
                n < g_4c62f8.records.blocks[cache_index + 1].count; ++n, ++entry)
            {
                long index = first + n;
                if (entry->unknown00 & selection & 0x1fffff &&
                    entry->unknown00 & 1 && entry->unknown10[0xa] > 0 && (entry->flags & 0x1f0) == 0x1f0)
                {
                    byte weight = g_4c0b78.weights[index];
                    long shape = entry->flags & 0xf;
                    byte material_index = g_4c0b78.material_indices[(short)index];
                    long material = NONE;
                    if (material_index != 0xff)
                        material = g_4c0b78.materials[material_index];
                    byte *geometry = (byte *)function_449e0((short)index, (bool)((entry->unknown00 >> 12) & 1), false);
                    if (geometry)
                    {
                        for (long part = 0; part < *(long *)geometry; ++part)
                        {
                            s_render_part_context context;
                            if (function_45ce0((short)index, (byte *)&context, (short)part, (short)shape) &&
                                function_460d0(*(dword const **)(entry->unknown10 + 4), (short)index, part))
                            {
                                long chosen = context.material;
                                if (material != NONE)
                                    chosen = material;
                                if (chosen != NONE)
                                    function_41040((short)index, part, 1, weight, &context, chosen, true, ~0UL, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

// score 155 (was 182): Frame 0x84/0x88, cache/draw transport and state/API expansion remain.
// Round 81: the last pass's flag test without the bool cast, `((g_4ba014 >> 1) & 1)`, 155 at
// retail's length (kept; `g_4ba014 & 2` gives the same code; retail still tests the byte in memory
// and reloads the dword for `shr 1`), with `bool selected` 220; on it the four selections without
// their bool casts 155, the last pass's tests as nested ifs 155, 0x402b0's parameters as
// (mode, cache_index) 155 (all the same code). 0x496f0 is now
// inlined here (retail calls it at all five call sites): a separate noinline declaration at its
// definition 314 (868 bytes); bool selection 222 (975 bytes, aligned 62 -> 32: retail's stack byte),
// with the noinline declaration 285 (892 bytes); the flag test's arms swapped 182 (same code).
// Earlier: frame 0x84/0x88, callee registers and render-state expansion differ;
// dword selection 183; word setup flags 234; combined mask 222;
// brightness component loop 238; redundant bit-clear removal 217.
// Word selection 238; byte setup flags 189; joined parent cursor 183;
// zero inline depth on dword selection 367; raw selection masks 198 (plain).
// j93: Frame 0x84/0x88 and helper conventions/expansion remain; original API values preserved.
// first pass byte 202; second pass byte 201; third pass byte 195; typed parent lookup 183; direct tag lookup
// 226.
// Earlier: Frame is four bytes small; cache/draw contracts and render expansion
// remain. first pass result arms 205; second pass result arms 196; third pass result arms 195;
// fourth pass result arms 195; all pass result arms 219; native cache record binding 183; native
// setup field updates 183; cache inline contract 183; signed setup word 183; native table
// traversal 266.
// Earlier: Frame is four bytes small; callee transport and graphics expansion remain. shared first pass 315; shared
// second pass 377; shared third pass 364; shared final pass 370; shared all passes 785; forced shared all
// passes 785; forced shared boolean results 785; forced shared byte results 785; constant table shared all
// passes 749; constant table forced shared all passes 749; constant table forced shared boolean results
// 749; constant table forced shared byte results 749; typed constant table shared all passes 280; typed
// constant table forced shared all passes 280; typed constant table forced shared boolean results 280;
// typed constant table forced shared byte results 280.
// frame 0x84/0x88, helper conventions and API expansion remain;
// independent final-pass byte 199; with first/second/third byte 209/208/202;
// with first-second/first-third/second-third bytes 218/214/213; final-pass short 200 (plain).
// Earlier: frame 0x84/0x88 and helper/API contracts remain; independent
// first/second/third/final/all native pass results 202f each; signed native setup flags, unsigned native
// selection and both 202f each; unchanged reduced preview 202f, full 183.
// Entry native selection/flags+selection/parent/ordinal/all declarations 202f each;
// counter+selection/flags+parent+resource/entry coordinated roles 202f each, unchanged control 202f.
// Native actual light definition/parent/node/camera fields 202f each; all fields 202f;
// external native record reference 202f; native definition receiver 202f; both receivers 202f.
// Unchanged reduced control 202f/946, ordinary baseline 183/947; frame and helper/API gaps remain.
// Light-pool receiver at every use 253f; with direct tag 242f; Boolean selection
// 283f; tag plus Boolean 275f; byte setup 256f; tag plus byte 242f;
// Boolean plus byte 286f; all three 276f (control 202f, full 183).
// Boolean receiver forms recover frame 0x88 but regress; original pooled observations restored.
// Native pass flags first/second/third/final 215/214/214/215; first pair 220; first three 227; all
// four 233; all with byte result 231. Fast control 202. Native flag header restored.
// Round 199: Plain shared 12..13 pass loop, long ordinal with long/bool/byte/short result
// 296/282/282/287; short ordinal with those results 293/279/279/284 (fast control 202). All restored.
// Round 161: Actual low-word shape kind with shared long/bool/signed-byte/short
// pass carriers 202f/231f/231f/248f; separate pass lifetimes 202f/231f/231f/248f.
// Unchanged reduced control 202f; full baseline 183. Actual shape type and body restored.
// Inline/noinline state-helper declaration with shared long/bool/byte/short pass
// carriers 319f/290f/290f/285f; independent carriers 319f/290f/290f/285f.
// Reduced control 202f; full baseline 183. All declarations and body restored.
// Round 140: Repeated sample count observed once for passes 11/12/13, their pairs and all three; then all with
// native Boolean selection 202/202/202/202/202/202/202/231 (fast control 202, full 183). Original observations
// and selection restored; frame, cache/draw and state/API expansion remain.
// Round 168: All normalized mask producers: long shared/split 202f each; bool shared/split 231f
// each; byte setup with long shared/split 203f each, bool shared/split 234f each. Fast control
// 202f; full 183. All source restored.
// Round 145: Stable cache bound with long/bool/byte/short results 210/253/253/251f;
// same bound with native cursor and ordinal 236/253/253/260f (control 202f, full 183).
// Bound-only long recovers frame 0x88 but regresses. All original traversal and results restored.
// frame 0x84 versus 0x88, cache/draw transport and state/API expansion remain.
// Retail keeps each pass's selection in a stack byte (+0x13; ours a long in ESI); 0x4a1e0 (machine 1's,
// in a contributor file) takes the camera in EAX, the light index in EDX and the definition on the
// stack in retail, ours the light index in EAX, the definition in EDX and the camera on the stack:
// BLOCKED-BY 0x4a1e0. Fixed: the depth clip planes are the vertex-program planes (retail writes the
// +0x1c10 pair and bit 2, as 0x13450 does), 183 -> 182.
// Tried: dword selection 183; word setup flags 234; combined mask 222; brightness component loop 238;
// redundant bit-clear removal 217; word selection 238; byte setup flags 189; joined parent cursor 183;
// zero inline depth on dword selection 367; raw selection masks 198; first/second/third pass byte
// 202/201/195; typed parent lookup 183; direct tag lookup 226; first/second/third/fourth/all pass result
// arms 205/196/195/195/219; native cache record binding 183; native setup field updates 183; cache inline
// contract 183; signed setup word 183; native table traversal 266; shared first/second/third/final/all
// passes 315/377/364/370/785, forced 785, forced Boolean/byte results 785/785; constant table shared all
// passes 749 (forced and result widths 749); typed constant table shared all passes 280 (forced and result
// widths 280); independent final-pass byte 199, with first/second/third byte 209/208/202, with pairs
// 218/214/213; final-pass short 200; independent native pass results 202f each; signed native setup
// flags, unsigned native selection and both 202f each; entry native selection/flags+selection/parent/
// ordinal/all declarations 202f each; counter+selection/flags+parent+resource/entry coordinated roles
// 202f each; native light definition/parent/node/camera fields 202f each, all 202f; external native
// record reference 202f; native definition receiver 202f, both receivers 202f; light-pool receiver at
// every use 253f, with direct tag 242f; Boolean selection 283f; tag plus Boolean 275f; byte setup 256f;
// tag plus byte 242f; Boolean plus byte 286f; all three 276f (Boolean receiver forms recover frame
// 0x88); native pass flags first/second/third/final 215/214/214/215, first pair 220, first three 227,
// all four 233, with byte result 231; plain shared 12..13 pass loop, long ordinal with long/bool/byte/
// short result 296/282/282/287, short ordinal 293/279/279/284; low-word shape kind with shared long/
// bool/signed-byte/short pass carriers 202f/231f/231f/248f, separate lifetimes the same;
// inline/noinline state-helper declaration with shared long/bool/byte/short carriers 319f/290f/290f/
// 285f, independent the same; repeated sample count observed once for passes 11/12/13, pairs and all
// three, with native Boolean selection 202f..231f; all normalized mask producers long shared/split 202f,
// bool 231f, byte setup with long 203f, with bool 234f ("f": reduced image, which builds this function
// differently: 202 there for 183 here). Round 75 (plain checks): Boolean selection 222 (975 bytes:
// 0x496f0 is then inlined, aligned 87 -> 44); with the vertex-program planes 222 (aligned 42); plus a
// noinline redeclaration of 0x496f0 here 222 (ignored); plus a noinline declaration at its definition
// 285 (892 bytes).
// Round 352: native cache-body graph with consistent reversed/unsigned/both mode contracts
// 201f each; four explicit cdecl combinations 211f each; unsigned reversed stdcall 201f.
// Fast control 201/946 versus plain 182/947. Original helper signature/call and body restored.
// Round 64: retail's passes are 0x44370 inlined (contributor file) and its per-light reset is 0x4a780
// inlined (same file); our LTCG keeps both out of line: the reset as a 0x4a780 call 305 (828 bytes), with
// a forceinline declaration 305; 0x402b0 parameters (mode, cache_index) 182 (still EAX); flags assigned to
// g_4ba014 directly 182 (identical); flag arms swapped by a negated test 182 (same layout).
// Lane 9 r23 (plain): retail's four passes are 0x44370's body inlined with the mode constant and
// its mode table folded (mode 14's target 1, the culling argument's dead g_485898 load, case 11's
// `enabled = true` re-store, the stack enable byte at +0x13): BLOCKED-BY 0x44370, which sits in a
// /Ob1 contributor file with a writable table. Calls to 0x44370 607 (569 bytes, not inlined), with
// a forceinline declaration here 607 (ignored); passes written as the folded inline 231 (frame 0x88
// and the stack byte agree, 0x496f0 inlines, 980 bytes); 0x402b0 with its block indexed at each use
// takes the cache index in EDX as retail's call does, 182 (0x402b0 itself 144).
// Lane 9 r29 (plain): SET_FLAG ternary for the geometry bit 155 (same code); byte selection 220
// (973 bytes, frame 0x88 and the stack byte as retail, 0x496f0 still inlined); SDK class
// SetDepthClipPlanes 155 (same); byte selection with a noinline 0x496f0 declaration 287 (890 bytes).
// Diagnostic: the passes as an inlined copy of 0x44370 with a const mode table 282 (895 bytes: stack
// byte, pass-11 re-store and the 0x496f0 call as retail; only the dead g_485898 loads and frame
// 0x88 missing); the same copy on the real writable table 782 (1389 bytes).
// r136 lane 7 (plain): the four passes through draw_light_source_pass (moved into include/selected_source_pass.h,
// with the stipple cull and case 11's `enabled = true`) 157 at 946/946 bytes (LCS-aligned 63 -> 20; the passes then
// equal retail, but ours keeps the constant 1 in EBX where retail uses immediates); not kept (positional rise).
// @retail 0x404b0
bool function_404b0(long mode)
{
    for (long cache_index = 0; cache_index < g_509440; ++cache_index)
    {
        if (g_4c6b00[cache_index].count <= 0) continue;
        long light_index = g_4c6b00[cache_index].index;
        byte *light = g_4e030c->data + (light_index & 0xffff) * 0x110;
        byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
        s_light_shape_ab shape;
        function_c17f0(light_index, &shape, true);
        if (*(short *)(light + 0x54) != NONE)
        {
            long parent = *(long *)(light + 0x4c);
            while (parent != NONE)
                parent = *(long *)(*(byte **)(g_4e0300->data + (parent & 0xffff) * 12 + 8) + 0x14);
        }
        function_402b0(cache_index, mode);
        function_4a1e0((s_view_source *)&shape, (s_view_camera *)(light + 0x84), light_index,
            (s_view_flags *)definition, 1024.0f, false, 1.0f);
        g_4ba018 = true;
        dword flags = (short)shape.kind == 0 ? 1 : 0;
        if (g_485950.i > 0.0f || g_485950.j > 0.0f || g_485950.k > 0.0f) flags |= 2;
        else flags &= ~2UL;
        g_4ba014 = flags & ~0x3cUL;
        long selected = (bool)((g_4c1a48[1].flags >> 11) & 1);
        if (selected)
        {
            g_467130 = 11; g_467134 = (void *)1; g_467138 = &g_4c1a48[1];
            g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true);
            function_15680(0);
            function_0496f0();
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
        selected = (bool)((g_4c1a48[1].flags >> 14) & 1);
        if (selected)
        {
            g_467130 = 14; g_467134 = (void *)1; g_467138 = &g_4c1a48[1];
            g_4858b4 = 1;
            function_14bc0(1, 0, true); function_15680(0);
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
        selected = (bool)((g_4c1a48[1].flags >> 12) & 1);
        if (selected)
        {
            g_467130 = 12; g_467134 = (void *)1; g_467138 = &g_4c1a48[1];
            g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true); function_15680(0);
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
        if ((g_4ba014 & 1) && ((g_4ba014 >> 1) & 1))
        {
            selected = (bool)((g_4c1a48[1].flags >> 13) & 1);
            if (selected)
            {
                g_467130 = 13; g_467134 = (void *)1; g_467138 = &g_4c1a48[1];
                g_4858b4 = g_4858b8;
                function_14bc0((short)g_4858b8, 0, true); function_15680(0);
            }
            else g_467130 = 0;
            function_15370(0);
            if (selected) { function_44550(); function_445d0(); }
        }
        g_4858bc = false; g_4b843c = 0;
        D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
        D3DDevice_SetScissors(0, FALSE, 0);
        D3DDevice_SetDepthClipPlanes(g_485adc, g_485ae0, D3DSDCP_SET_VERTEXPROGRAM_PLANES);
        function_14bc0((short)g_4858b8, 0, true);
        g_4ba018 = false; g_4ba014 = 0;
    }
    return true;
}

// score 85 (was 193): r136 lane 7: the passes go through the shared pass helper (include/selected_source_pass.h, moved
// there from src/unknown_0350e0.cpp unchanged), which follows 0x44370's body for a constant mode (gate by mode,
// the two 0x15680 calls for the cull, late `case 3: case 16: enabled = true;`). With the cull test in a one-use `bool
// stipple_enabled` (76; kept only with a match, coordinator 11-08): 1215/1215 bytes, every inlined pass equal to retail's except the 0x14b60 call (flags pushed, retail passes them in eax): BLOCKED-BY 0x14b60.
// Earlier 193 (was 193): BLOCKED-BY 0x44370. Frame 0x48 and geometry agree. Round 81 (fast; aligned
// count 24, so the 21 missing bytes cause most of the 193): the entry byte test as `<= 0` 192 (retail's
// `jbe`); the selections or the geometry flag without their bool casts 193/193 (same code); pass 8 as
// `selected = bit; if (selected) selected = g_4ba022;` 176 (aligned 16, not kept: a flag set inside a
// test; retail copies the global byte unnormalised, which needs g_4ba022 declared bool where a
// contributor file defines it as byte); the same as a ternary 170 (1215 bytes only through padding,
// aligned 36, not kept). Retail's passes 16/1/3/8/20/7
// are 0x44370 inlined with its mode table folded (that table lies in retail's .rdata; ours is a writable
// byte array defined in a contributor's file), so in every pass that culls with 0 retail still loads
// g_485898 (`mov eax, [g_485898]; xor eax, eax`, 20 of the 21 missing bytes: the table's cull mode became
// constant only after layout) and re-stores the pass's selected byte as 1 after the culling call in
// passes 16 and 3. Also differ: the entry byte test (retail `jbe`), the stub 0x14b60 (retail passes its
// flags in eax) and the pass-8 enable (retail copies the global byte).
// Forms tried: common final flush 226, with noinline query 226, inline query 397; short/long/byte result
// 254/300/226; noinline with short/byte 254/226; word material index 298; unsigned weight 309; typed geometry
// header 250; entry observations reordered 326; native local names 250; early weight with context-first material
// 226; depth zero 250, one 250; size policy 462; shared tail-pass loop 476; shared single pass 275, first
// multipass 269, second multipass 281, conditional pass 273, twentieth pass 269, seventh pass 269, all passes 291,
// forced shared all passes 291; shared final result two consumers 291; native final result two consumers 291;
// conditional cull calls first/third/both 226 each; native Boolean cull tests first/third/both 226 each;
// signed/unsigned short pass result 255/255; inline cull/stencil/first-flush/second-flush/state-pair/flush-pair/
// all-four declarations 226 each, noinline all-four 226; late single/third/both success publication 220/212/210;
// single/third/conditional result arms 232/214/226; all result arms 204 (1217 B); ordered conditional result 221;
// native masks for passes 16/1/3/8/20/7 226/226/226/230/226/226, all masks 230, with byte lower-bound admission
// 229; entry tests selection/bit0/byte/flags in orders 2134/1324/1243/2143/1342/1423/2314/1432
// 237/230/233/244/235/308/239/308; two-use setup setting captured in single/third/conditional/twentieth/seventh
// passes 226 each, all five 226, cull-setting observations once 226, all setting captures 226; formal-address
// removal, pass-phase false result, entry result and entry false result 226 each, removal with pass false/entry/
// entry false 226 each; isolated unsigned-byte lower-bound admission 225; independent pass outcomes bool/byte/
// short/long 226/226/255/297; complete stipple choice bool/byte/short/long 193/193/226/226 (bool kept); Boolean
// role name, negated range, direct outside range and byte outside range 193 each.
// Round 145: Shared culling setting/predicate/split comparisons at multipass entry
// 275/264/264; setting/predicate at pass phase 272/274; independent outcomes with
// those three multipass observations 275/264/264. Original observations and body restored.
// Round 243: Seven ordered admission partitions 225 each; positive geometry arm 192 at
// 1194 bytes, first +0x1b3. One-point gain rejected; original body restored.
// Round 57 (aligned listing counts in brackets, baseline 24): stipple argument `cond ? 1 : 0` in both culling
// passes 226 [25] (1181 B); one forced-inline pass helper (bit, extra condition, depth, culling, target) with a
// bool `&&` enable 205 [38] (1215 B), with a byte `(flags >> mode) & extra` enable 230 [16]; the helper returning
// whether the pass started (the caller flushes): early-return bool 199 [32], byte enable with early returns 203
// [29], bool enable 199 [32], culling test order flipped 203 [29], byte enable returning `enabled != 0` 235 [14],
// ternary byte enable 205 [35], bool `&&` enable returned 211 [38], ternary returned as byte 205 [35], the byte
// helper with a stipple local before the culling call 235 [14]. None gives retail's dead g_485898 loads.
// Round 252: Both window-predicate parts named at both stipple producers, bool/byte/short/long terms:
// Boolean result 193 each; full-width result 226 each. Original predicates restored; unsigned-zero
// branch, pass joins, missing setting loads and clear-call transport remain.
// Round 205: Four admission-mask result producers after setup 350/218/353/218;
// the same per-entry producers 309/364/366/365. Original mask lifetime and body
// restored; byte-zero admission, pass joins and clear-call transport remain.
// Context before enable/walk/object/part boundaries 193 each; the same
// four scopes with every entry field read directly 310 each. Original context
// scope and cached-entry pointer restored; pass joins and call transport remain.
// Round 61: every pass as a call to 0x44370 928 (618 B, kept out of line), with a forceinline
// declaration of it 928; one forced-inline pass helper in 0x44370's form (mode, target, clear flags,
// cull mode; `cond ? 0 : cull_mode`; modes 3/16 re-marked enabled) 203 (1198 B; the re-store and
// the word argument then as retail, the stipple arms swapped), the cull arm written first 203, with
// 0x44370's mask-gated enable 203. Original body restored.
// Lane 9 r29 (fast): entry byte `<= 0` 192 again (one point, not kept); stipple as
// `g_485898 >= 4 && g_485898 <= 7 ? 0 : 1` 226; pass-8 `g_4ba022 != 0` 193; material as a
// ternary 269; `(void)&single_pass` removed 193 (identical code, plain; kept). 0x14b60 written
// from the disabled draft with a noinline declaration (plain): 255, and matched 0x14ac0 drops to 2.
// Round 12 (lane 10, fast): the real 0x14b60 body in this file (stub removed) breaks matched 0x14ac0
// (2), builds 93 bytes against 91 and takes this to 245, as the disabled contributor copy warns;
// culling argument as `g >= 4 && g <= 7 ? 0 : 1` 226 (1181 B), the call written in both arms in either
// condition sense 226/226 (two call sites as retail, but our 0 arm falls through where retail's 1 arm
// does: that arm was still the table's cull-mode load when retail laid the blocks out); pass 8 as
// `selected = bit; if (selected) selected = g_4ba022 != 0;` 176 (1196 B; a flag set inside a test,
// match-only; retail stores the raw byte, so the global is a bool in retail). All restored.
// @retail 0x3fdf0
bool function_3fdf0(long mode, bool single_pass)
{
    g_4b6280.unknown04 = g_4b6280.count;
    dword selection = !mode ? ~0UL : mode == 2 ? 0x10 : 0x20;
    g_4c1a48[0].unknown10 = g_4c1a48[0].flags;
    g_4c1a48[0].flags = 0;
    g_4c1a48[0].first = g_4c1a48[0].count;
    function_174220(true);
    for (long n = 0; n < g_50943c; ++n)
    {
        short index_bits = ((short const *)g_4c6b00[7].values)[n];
        if (index_bits == NONE) continue;
        long index = (short)index_bits;
        s_44940_entry const *entry = &g_4ba138[index];
        if (!((entry->unknown00 & selection) & 0x1fffff) || !(entry->unknown00 & 1) ||
            !entry->unknown10[0xa] || (entry->flags & 0x1f0) != 0x1f0) continue;
        byte weight = g_4c0b78.weights[index];
        long transform_index = entry->flags & 0xf;
        byte material_index = g_4c0b78.material_indices[index];
        long material = NONE;
        if (material_index != 0xff)
            material = g_4c0b78.materials[material_index];
        byte *geometry = (byte *)function_449e0((short)index_bits, (bool)((entry->unknown00 >> 12) & 1), false);
        if (!geometry) continue;
        for (long part = 0; part < *(long *)geometry; ++part)
        {
            s_render_part_context context;
            if (function_45ce0((short)index_bits, (byte *)&context, (short)part, (short)transform_index) &&
                function_460d0(*(dword const **)(entry->unknown10 + 4), index, part))
            {
                long chosen = context.material;
                if (material != NONE) chosen = material;
                if (chosen != NONE) function_41040(index, part, 0, weight, &context, chosen, false, ~0UL, 0);
            }
        }
    }
    if (single_pass)
        draw_selected_source_pass(0, 16, false, false, g_4858b8, false);
    else
    {
        draw_selected_source_pass(0, 1, true, true, 1, false);
        draw_selected_source_pass(0, 3, true, true, g_4858b8, false);
        draw_selected_source_pass(0, 8, false, false, g_4858b8, false);
        draw_selected_source_pass(0, 20, false, false, g_4858b8, false);
        draw_selected_source_pass(0, 7, false, false, g_4858b8, false);
        if (function_44370(2)) { function_44550(); function_445d0(); }
    }
    g_4c1a48[0].flags = g_4c1a48[0].unknown10;
    g_4c1a48[0].first = 0;
    return true;
}
