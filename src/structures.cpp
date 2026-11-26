// @flags /O2 /arch:SSE /Gr
/* STRUCTURES.CPP: queries of the structure bsp's render geometry: the
   lightmap triangle under a collision point */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "geometry_cache.h"

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
short function_10a0b0(long object_index);
bool function_bf9a0(long object_index, long *render_index, long *animation_index);
extern vector3f *g_4687bc;

/* a range of a section's triangles: the cluster (or instance definition)
   it is in, and its first triangle reference (8 bytes) */
struct s_structure_surface_range
{
	short cluster_index;
	short count;
	long first;
};

/* a triangle reference: its first index in the section's strip, its
   lightmap part, and its collision surface (8 bytes) */
struct s_structure_triangle_reference
{
	word first_index;
	word part_index;
	long surface_index;
};

/* a collision bsp's surfaces, as read here: the plane first (8 bytes) */
struct s_structure_collision_surface
{
	short plane_index;
	byte unknown02[6];
};

struct s_structure_collision_bsp
{
	long surface_count;
	s_structure_collision_surface *surfaces;
	byte unknown08[0x40 - 0x08];
};

/* a section's vertices: the count and the data, at an offset in the
   section's block (0x20 bytes) */
struct s_structure_vertex_block
{
	word unknown00;
	word count;
	byte unknown04[4];
	long offset;
	byte *data;
	byte unknown10[0x20 - 0x10];
};

/* a section of render geometry (0x44 bytes): its parts (the list
   function_16e1b0 searches), its strip indices and its vertices */
struct s_structure_section
{
	long part_count;
	void *parts;
	byte unknown08[0x20 - 0x08];
	long strip_index_count;
	word *strip_indices;
	byte unknown28[0x38 - 0x28];
	long vertex_block_count;
	s_structure_vertex_block *vertex_blocks;
	byte unknown40[4];
};

/* a cluster (0xb0 bytes) and an instanced geometry definition (0xc8 bytes)
   both start their geometry the same way */
struct s_structure_cluster_view
{
	byte unknown00[0x28];
	s_geometry_block_info field_28;
	byte unknown4c[4];
	s_structure_section *sections;
	byte unknown54[0xb0 - 0x54];
};

struct s_structure_instanced_geometry_definition
{
	byte unknown00[0x28];
	s_geometry_block_info field_28;
	byte unknown4c[4];
	s_structure_section *sections;
	byte unknown54[0x74 - 0x54];
	s_structure_collision_surface *surfaces;
	byte unknown78[0xb8 - 0x78];
	long surface_range_count;
	s_structure_surface_range *surface_ranges;
	byte unknownc0[4];
	s_structure_triangle_reference *triangle_references;
};

struct s_structure_instance
{
	transform4x3f matrix;
	short definition_index;
	byte unknown36[0x58 - 0x36];
};

struct s_structure_bsp_view
{
	byte unknown000[0x18];
	s_structure_collision_bsp *collision_bsps;
	byte unknown01c[0x30 - 0x1c];
	s_structure_surface_range *surface_ranges;
	byte unknown034[0x50 - 0x34];
	s_structure_triangle_reference *triangle_references;
	byte unknown054[0xa0 - 0x54];
	s_structure_cluster_view *clusters;
	byte unknown0a4[0x13c - 0xa4];
	s_structure_instanced_geometry_definition *instanced_geometry_definitions;
	byte unknown140[4];
	s_structure_instance *instances;
};

/* a collision result, as read here */
struct s_structure_collision_result
{
	long type;
	byte unknown04[4];
	point3f point;
	byte unknown14[0x3c - 0x14];
	long instance_index;
	byte unknown40[0x4c - 0x40];
	long surface_range_index;
	byte unknown50[4];
	long plane_index;
};

/* the lightmap triangle found */
struct s_structure_lightmap_triangle
{
	long cluster_index;
	long instance_index;
	byte unknown08[0x18 - 0x08];
	long unknown18;
	byte unknown1c[4];
	long part_index;
	long part_offset;
	long lightmap_part_index;
	byte unknown2c[4];
	real u;
	real v;
};

bool structure_get_lightmap_triangle(s_structure_collision_result const *collision,
    s_structure_lightmap_triangle *triangle);
bool __stdcall function_14a8e0(s_structure_collision_result const *collision,
    long model_index, s_structure_lightmap_triangle *triangle);

/* the collision a surface query starts from, passed by value */
struct s_surface_query_source
{
    void *collision;
};

/* whether the current structure bsp is loaded and its collision usable */
static __forceinline bool structure_bsp_collision_available()
{
    return g_4e0344 && g_4e0344->count > 0 && g_4e0348 &&
        *(long const *)((byte const *)g_4e0344->bsp + 0x1c) != NONE &&
        *(long const *)((byte const *)g_4e0344->bsp + 4) == *(long const *)((byte const *)g_4e0348 + 8);
}

// MiniMax I (11-06): 45, not kept: the model query's result stored in `result` with a new `goto done` to the
// shared exit in place of the early return (retail's 286 bytes, same code otherwise); a goto main's body does
// not have is kept only with a byte match.
// score 53 (was 53): frame 0x60/prefix agree; exclusion BL and source-slot rewrite/reload remain absent; the byte result
// is still normalized with setne at both exits.
// Earlier at 53 (was 61): the 0xbf9a0 probe is inverted, so the 0x14a8e0 return is laid out after the requery as
// in retail. MiniMax E (10-31): with the type-6 test also setting a named exclusion flag 51 (a test-set flag,
// not kept without a match); VC still threads the flag on every edge (retail keeps it in BL) and the byte
// result is converted with setne.
// Earlier at 61 (was 61): VC threads the exclusion flag on every edge (retail keeps it in BL: `mov bl, 1;
// jmp` in the scenery arm, `xor bl, bl` before 0xbf9a0 and `test bl, bl` again after it, so
// &triangle->c takes EDI and the collision lives in its parameter slot: stored after the requery,
// reloaded after 0xbf9a0); the byte result is converted with setne. Retail 0x14af40 has the
// identical idiom. Tried: read-only object 68; unsigned object handle 68; word query buffer 68;
// native collision record 71; external triangle reference 68; source publication 68; byte result
// merge 68; query workspace names 68; native record retested with byte result storage; model
// inline/noinline 68/68; state inline 68; both contracts 68; probe result 68; model result 68;
// final found result 111; triangle inline 68; the collision re-read from the parameter at the join
// 68 (with a block-local flag 68); the flag as one && expression tested twice, with returns 68; the
// same with an else-if tail and a final else for the no-object case 71 (286 bytes); assigned with
// the && expression, tests unchanged 68; an inline helper for the scenery test stays out of line
// and is threaded 113/111; a switch on the object type with the scenery case falling into the
// general case 68, or the flag set in a one-case switch 68; the flag from the && expression with
// the model query nested under `if (!excluded)` 68; the requery in an inline helper that updates
// the collision through its address 90; found cleared in the scenery arm with no flag 68; bool
// result 78 (262 bytes: setne gone); the flag set in both arms with `if (excluded) goto done;` 68,
// and with a bool result 78; a pointer parameter passed by address to an inline probe helper 134;
// the by-value source passed by address to that helper 86/76; multi-consumer object policy,
// short/long and bool/byte found in object scope 117/117/116/116, at function scope 68/68/61/61
// (long policy reaches 286 bytes); `excluded = function_10a0b0(...) != 2` inside the type test
// with `if (excluded) goto done` or `if (!excluded)` guards 54/54 (286 bytes, setne and the
// threaded exit stay; a flag set inside a test, not kept); the object part in an inline helper
// returning `!excluded` 106 (keeps the flag in BL and the join test, adds a sete); the final test
// nested in the requery arm 77; `return function_14a8e0(...)` with an && flag 77 (263 bytes); a
// named `instance` bool from `!excluded && 0xbf9a0` 68; the exclusion clearing found with a
// found-only final test 68; the flag in two steps (type test, then `!= 2`) 92 (298 bytes: keeps
// the join test after 0xbf9a0, but as sete/setne); an inverted `lightmapped` flag from `||` 68; a
// single-exit `handled` flag also set after 0x14a8e0 68. A compile-only screen of ~190 flag,
// guard and tail forms threads the flag in every one; only a computed value in the join (a
// negation, a widening, a store, a setcc result) keeps retail's un-threaded flag test.
// retail keeps an exclusion flag in BL (`mov bl, 1` on the scenery arm, `xor bl,
// bl` before 0xbf9a0) and tests it at a join it does not thread, so &triangle->c takes EDI, the
// collision lives in its parameter slot (reloaded after the model query, stored after the requery)
// and the 0x14a8e0 return is laid out last; VC threads every flag form here, so the excluded arm now
// returns directly (no flag) and the model hit returns 0x14a8e0's result (286 bytes; the set-in-a-test
// flag and its goto are gone). Tried: read-only object 68; unsigned object handle 68; word query
// buffer 68; native collision record 71; external triangle reference 68; source publication 68; byte
// result merge 68; query workspace names 68; model inline/noinline 68/68; state inline 68; both
// contracts 68; probe result 68; model result 68; final found result 111; triangle inline 68; the
// collision re-read from the parameter at the join 68 (with a block-local flag 68); the flag as one &&
// expression tested twice, with returns 68; the same with an else-if tail and a final else for the
// no-object case 71 (286 bytes); assigned with the && expression, tests unchanged 68; an inline helper
// for the scenery test stays out of line and is threaded 113/111; a switch on the object type with the
// scenery case falling into the general case 68, or the flag set in a one-case switch 68; the flag
// from the && expression with the model query nested under `if (!excluded)` 68; the requery in an
// inline helper that updates the collision through its address 90; found cleared in the scenery arm
// with no flag 68; bool result 78 (262 bytes); the flag set in both arms with `if (excluded) goto
// done;` 68, with a bool result 78; a pointer parameter passed by address to an inline probe helper
// 134 (collision in EAX, 0xd2bb0 22); the by-value source passed by address to that helper 86/76
// (bool/byte result; 0xd2bb0 5); multi-consumer object policy, short/long and bool/byte found in
// object scope 117/117/116/116; function-scope policy 68/68/61/61 (long policy at 286 bytes, not
// kept); `excluded = function_10a0b0(...) != 2` inside the type test with `if (excluded) goto done`
// or `if (!excluded)` guards 54/54 (286 bytes, aligned worse: setne and the threaded exit stay); the
// object part in an inline helper returning `!excluded` 106 (keeps BL and the join test, adds a
// sete); the final test nested in the requery arm 77; `return function_14a8e0(...)` with an && flag
// 77 (263 bytes); 177 flag/guard/tail forms compile-only, all threaded; only a computed value in the
// join (a negation, a widening, a store) keeps retail's un-threaded flag test. Round 11 (relink
// screen, 66 = the old form there): the final test `found && !excluded` 113; the flag as long or
// byte 66; the collision read through the source at each use 66; on the kept form, a bool result 63
// (aligned 43 -> 41), `return false` on the excluded arm 58 (same code), `return` of the final query
// 128 (257 bytes). Lane 13 r14 (also: retail stores the requery's collision into the by-value
// source's slot, a dead store, and reads the source's collision from that slot after each call): a
// bool result 64; `return false` on the excluded arm 61; the flag from the && expression guarding the
// model query with `if (excluded) return result;` 65; the model result stored in `result` and the
// requery reading the source's collision 64; the flag set in the scenery arm, the model query in an
// else-if and the requery in `if (!excluded)` 82 (258 bytes).
// Round 44: existing availability producer result-first/guard returns/nested outcomes/
// structure once/settings once/both/both guarded/both with BSP receiver 61/98/98/61/78/78/98/61.
// Original helper/body restored; exclusion BL and source-slot join remain.
// Round 32 (lane 10, fast counts): `bool excluded = scenery && policy != 2;` with
// `if (!excluded && 0xbf9a0) { result = 0x14a8e0(...); return result; } else if (!excluded) requery; else
// return result;` 56 (286 bytes, threaded: no flag), with a bool result 71 (269 bytes); the requery as
// `if (!excluded) ... else found = false;` 72 (268 bytes), with a bool result 82 (258 bytes); the flag set
// in the scenery arm with the model query as its else-if and the collision re-read from the source at the
// join before `if (excluded) return result;` 55 (286 bytes, still threaded; a flag set in a test, not kept),
// with `if (!excluded) requery else found = false` 72; an inline exclusion test returning true/false
// (diagnostic) 95 (269 bytes).
// Round 11 (lane 2): ours threads the second flag test across the 0xbf9a0 call (the false edge knows
// the flag is clear); retail threads only the first. Real: the scenery test in a forceinline
// single-exit bool helper 112 (302 bytes: 0x10a0b0 inlined, flag threaded); the && flag tested
// twice (`!excluded && 0xbf9a0`, then `if (excluded) return result;`) 62; on it, `collision` reloaded
// from the source just before the second test 62 (dropped before threading; diagnostic); on it, a
// bool result 65; a function-scope flag cleared at entry, set in the scenery arm, 0xbf9a0 in an else-if,
// the requery under `if (!excluded)` with `found = false` otherwise 77 (263 bytes). The one-file LTCG
// harness passes the surface in EAX (the real build in ESI) even with 0xd2bb0's file and the other
// callers of 0x14ac60/0x14a8e0 linked, so it cannot screen this function.
// Round 97 (lane 8, fast with 0xd2bb0 and the callees): the flag as the out-parameter of an inline
// helper that also calls 0xbf9a0 (memory at threading time?) stays out of line 136 (251 bytes, ret 8,
// 0xd2bb0 lost); forced inline 112 (302 bytes: 0x10a0b0 inlined, flag still threaded), single-exit
// helper 112; a bool result with `return false` on the excluded arm 64; `result = 0x14a8e0(...);
// return result;` 54 (same code plus a setne on that path: the lengths line up, no structural gain),
// with a bool result 64, through `goto done` 56 (287 bytes); on it, `return false` on the excluded arm
// 54, the final call reading the source's collision 54, `found` set from the type test only when there
// is no object 92 (288 bytes, 0xd2bb0 5).
// Round 32 (lane 11, fast; 0x14af40 has the same BL flag and inlines 0x10a0b0 where this calls it, so
// a helper shared by the two was tried): the scenery test in a force-inline bool helper with
// `if (!excluded && 0xbf9a0)` storing the 0x14a8e0 result and `if (excluded) return` 118 (307 bytes),
// as plain inline 105 (245 bytes, out of line); the flag through a reference out-parameter 118; a helper
// returning the 0xbf9a0 result with the flag through a pointer 118; the flag cleared, then set from
// `!= 2` inside the type test 56 (288 bytes, setne and the direct exit stay; a flag set in a test).
// Compile-only (/O2 without /GL) all of these, plain/force-inline/single-exit helpers, thread the flag.
// Round 113 (lane 6, fast under the 0x14a8e0 reduced-image caveat): the flag as `type == 6 ? 0x10a0b0(...) != 2 : false` tested as `!excluded &&
// 0xbf9a0` then `if (excluded) return result;` 56 (288 bytes, setne in the type test, threaded); that with a
// bool result 59; plus the collision re-read from the source at the join 59; the 0x14af40 shape (`else if
// (!excluded)` requery with its own lightmap call, `else if (found)` for the no-object case) 86 (296 bytes);
// a function-scope flag cleared at entry and set in the type test, bool result 65. A scan of every matched
// retail function finds no other `mov r8, 1; jmp` to a `test r8, r8` past a call: only 0x14af40 shares it.
// N round 257: Complete native collision output on the inverted probe graph, byte/bool result and bool/byte found, raw/native triangle sentinel 53/53/53/53/63/63/63/63.
// Original record declarations, byte buffer and complete body restored.
// Round 108 (m3 lane 7, fast): a `modeled` flag set from 0xbf9a0 in the else arm of the scenery test, then
// `if (modeled) return 0x14a8e0(...)` and `if (!excluded)` requery 88 (found in BL, not the flag); `found = false`
// at the join before `if (!excluded)` 77 (263 bytes, threaded); a short policy defaulting to 2, tested twice
// 64; `excluded = false; if (type == 6) excluded = policy != 2;` with the model query and requery nested under
// `if (!excluded)`, else `found = false` 69 (268 bytes); a `lightmapped` flag tested around the model query
// and again around the requery 77 (263 bytes). Compile-only, all thread the join test. Compile-only probes:
// VC threads the flag through struct copies, inline accessors, address-taken references and cross-module
// (LTCG) inline helpers alike; only a side effect at the join (a store, even a redundant `result = false;`)
// leaves the test un-threaded, and that store stays in the code. Diagnostic (not kept): the flag set in both
// arms with `result = false;` before `if (excluded) return result;` 102 (302 bytes; flag in AL, not BL).
// Round 25 (m3 lane 2, fast; a compile-only /O2 screen of this file reproduces the threading): `found = false;`
// placed at the join before `if (!excluded)` around the requery, the && flag guarding 0xbf9a0, 77 (263 bytes:
// the store is sunk into the excluded arm and both tests thread); `result = false;` at the join with
// `if (excluded) return result;` 102 (302 bytes: the flag test stays un-threaded, `mov al, 1; jmp`, but the
// store stays, the flag is spilled and NONE takes EBP); `found = !excluded;` at the join with `if (found)`
// around the requery 86 (291 bytes: un-threaded, but sete and a spilled flag); that with the collision read
// through `source.collision` at every use 86 (same code); with the model result stored in `result` first
// 92 (296 bytes). The screen keeps the flag test only when the flag-test block's predecessor on the excluded
// edge is not the arm that sets the flag (a computed value or a memory test first); every such form that
// keeps the semantics adds code at the join, and the flag is then spilled where retail spills the collision.
// Round 61 (m3 lane 9, fast): the flag set in the scenery arm with 0xbf9a0 in its else-if and `point3f const
// *point = &collision->point;` named at the join before `if (excluded) return result;` 62 (the address sinks
// first; still threaded), with the requery under `if (!excluded)` and `else found = false;` 77 (263 bytes);
// the exclusion condition written out at both tests, no flag (looking for a CSE'd condition temp) 107 (309
// bytes: 0x10a0b0 called twice); a `done` flag set by the exclusion and by the model path (`result =
// 0x14a8e0(...); done = true;`) tested before the requery 68 (272 bytes); the positive test (`type != 6 ||
// 0x10a0b0 == 2`) holding the probe and requery with `else found = false;` 77 (263 bytes).
// Round 27 (m3 lane 2, fast): `found = false; if (!excluded) found = requery;` with the collision assigned after
// that if 79 (288 bytes: retail's `mov cl, 1; jmp` and an un-threaded `test cl, cl`, but `xor al, al` at the
// join, the flag spilled and NONE in EBP); with a bool result 81 (286 bytes); with the collision published
// through `source.collision` 79; `found = !excluded; if (found) found = requery;` with the assignment after the
// if 86 (291 bytes); an inline helper returning 0/1/2 (excluded/model/none) for the object part 160 (225 bytes,
// out of line); an inline `type == 6 && policy != 2` helper with `if (!excluded) requery else return result` 105
// (245 bytes, out of line). Compile-only: /O1, /Os, /G7, /Ob1, /Ob0, /Op and /Ox all thread the flag too, and so
// do helpers returning the exclusion (early returns, single-exit result, &&), `found = excluded ? ...`,
// `found = !excluded && requery` and an `__assume` at the join.
// Round 32 (m3 lane 2, fast): the && flag with the collision re-read from the source at the join 62 (the copy goes
// first; threaded); with a pointer to the query buffer named at the join 62; g_4687bc read into a local at
// the join 95 (296 bytes: the flag survives, `mov cl, 1; jmp` and the flag cleared just before 0xbf9a0 as in retail,
// but it is spilled, NONE takes EBX and the object index EDI); the source re-read through its address at the join
// 62 (folded); the join load with `if (!excluded) requery; else found = false;` 79 (286 bytes, same spill). A
// compile-only screen of 13 more flag forms (if-set, set after the probe, long, `== false`, struct-held, ternary
// requery, address-taken pointer parameter reassigned, parameter re-read at the join) threads every one; only a
// real load at the join keeps the flag, and it stays ahead of the test. The permuter's baseline link stalled.
// Round 39 (m3 lane 15, plain): the flag preset true and cleared in the not-excluded arm, `if (excluded) return
// result;` before the requery 62; the && flag with g_4687bc read into a local at the join and the requery and
// model reading the source's collision 95 (296 bytes: the flag survives but is spilled, &triangle->c takes EBX);
// that with every collision use through the source 95; with the model result stored in `result` and one `goto
// done` exit 89 (294 bytes, NONE in EBP); the preset-true flag with the join load 85 (293 bytes, still spilled).
// Compile-only, all threaded: preset-true in four placements, a positive requery flag, a negated or widened copy
// of the flag tested at the join, and the source as a class with a constructor or a destructor.
// Round 110 (m3 lane 7, fast): a compile-only screen keeps the join's flag test un-threaded (retail's `mov bl, 1;
// jmp`, `xor bl, bl` just before 0xbf9a0 and `test bl, bl` after it) when the flag reaches the test through a copy
// into a variable that has another live definition (as matched 0x1a0560's `result = loaded;`): a function-scope
// `excluded = false`, `excluded = scenery_excluded;` at the join and a final `found && !excluded`. Real builds of
// that: 77 (295 bytes: a second flag test, `found` spilled across the probe, the probe block laid out last); with
// the scenery test as if/else arms 77; with the probe nested under `if (!scenery_excluded)` 77; with `if
// (excluded) return result;` before the requery 62 (threaded again); `if (scenery_excluded) excluded = true; else
// { requery }` 75 (295 bytes).
// Round 113 (m3 lane 7, fast): the existing `object_get` for the object lookup 53 (same code); the object index
// read before `found` 97; the requery assigning `collision` only, 0x14a8e0 given `collision` 53 (same code); a failed
// requery returning `result` at once 108; the tail as `if (!found) return result; return lightmap(...)` 129 (257
// bytes); if/else exclusion arms (flag set true, or false with the model query returning 0x14a8e0) and `if (excluded)
// return result;` 62 (threaded).
// Round 286: Complete collision object-handle observations with cached/direct header,
// cached/per-use source and bool/byte found, eight forms 53 each. Original field
// snapshots and complete executable body restored; exclusion/source-slot join remains.
// Round 63 (m3 lane 16, fast): compile-only, the join test also threads as a switch on the flag, behind an
// `excluded || !0xbf9a0` guard, as a counter, as a parameter, and as `!!`, `== true`, `!= false`, `(char)`,
// `(long)`, `? true : false`, `| false`, `^ false`, `& 0xff`; the C front end and a constructed/destructed
// collision record thread it too. Only a bit-and join test keeps it: `if (excluded & 1)` 96 (297 bytes:
// retail's `mov r8, 1; jmp` and unthreaded join, but `test r8, 1`, the flag spilled into the collision's
// slot and NONE in EBX); that with the collision read through the source at every use 96; a plain
// non-inline exclusion helper 103 (240 bytes: LTCG keeps it out of line); the kept form with the collision
// read through the source at every use 53 (same code); a `short policy = 2` local set by 0x10a0b0 for
// scenery and tested `== 2` before 0xbf9a0 and `!= 2` before the requery 62.
// Round 221: coordinated byte return with native byte/bool result and direct/materialized model return
// 63/63/63/63; signed-byte return on those four graphs 63/63/63/63, all 269/286 bytes. The 0x60 frame
// and availability prefix remain correct; return normalization disappears but exclusion BL and
// source-slot publication do not appear. Every matched caller and file control survives. Original
// Boolean return, caller declaration, byte result, direct model return and complete body restored.
// Round 110 (m3 lane 7, fast): a compile-only screen keeps the join's flag test un-threaded (retail's `mov bl, 1;
// jmp`, `xor bl, bl` just before 0xbf9a0 and `test bl, bl` after it) when the flag reaches the test through a copy
// into a variable that has another live definition (as matched 0x1a0560's `result = loaded;`): a function-scope
// `excluded = false`, `excluded = scenery_excluded;` at the join and a final `found && !excluded`. Real builds of
// that: 77 (295 bytes: a second flag test, `found` spilled across the probe, the probe block laid out last); with
// the scenery test as if/else arms 77; with the probe nested under `if (!scenery_excluded)` 77; with `if
// (excluded) return result;` before the requery 62 (threaded again); `if (scenery_excluded) excluded = true; else
// { requery }` 75 (295 bytes).
// Round 294: Native query point assignment/memcpy/memmove 81/81/77; direction
// assignment/memcpy/memmove 80/80/75; both memcpy/memmove 113/137. Exact-only
// input snapshots restored; exclusion BL and source-slot publication/reload remain.
// Round 298: Finite acceptance/outcome/eligibility domains and pairs/all
// 52/56/53/74/52/56/74; all with complete finite model return 83.
// One-point leads rejected; original byte result and entire body restored.
// Exclusion BL and source-slot publication/reload remain.
// Round 300: Complete positive/rejection availability with common result exit and native
// short/long object policy, default/type-arm and conditional producers:
// 95/95/97/97/95/95/97/97. Original complete body restored; exclusion BL and source-slot join remain.
// Round 197 (lane 3, fast with the callees and 0xd2bb0): a plain collision pointer parameter in place of the by-value source
// 110 (frame 0x5c; 0xd2bb0 keeps its match); that with a `modeled` flag and nested tests 127 (262 bytes), with a bool result
// 137 (252 bytes); bool result alone 116 (271 bytes); requery split from the collision set with a second flag test 100
// (290 bytes; 0xd2bb0 21). Compile-only, the join test threads with a ternary, `&&`, do/while(false), `__assume` or `__noop`
// or an empty statement or goto label at the join, and a flag copied at the join into a variable with a second definition;
// only `found && !flag` (an extra test and an entry `xor`) and a store between the two tests keep it. Body restored.
// Round 519: Complete native collision-input assignment/memcpy/memmove after admission
// 156/156/154; after triangle resets 156/156/154; early assignment/memcpy with byte
// acceptance 156/156. Assignment/copy forms lose the matched caller; all restored.
// Exclusion BL and source-slot publication/reload remain.
// Round 343: Object-handle/header-producer/completed-classification last-use scope subsets
// 1..7 each 53; joint scopes with the model result at the shared exit 45 (286 bytes).
// Minimal model-result assignment + a new goto to a new label before the shared return 45 plain: NOT kept
// (goto, match only); assignment followed by its own return 46 NOT kept (gain seven, under the bar).
// Body restored at 53; exclusion/source-slot join remains.
// Round 138 (m3 lane 5): the model result stored in `result` with one `goto done` to the final return 45 plain: NOT kept
// (coordinator: the function keeps two other early returns, so the goto only moves this return, as round 343's; body
// restored at 53);
// on it, a bool result 56, the excluded arm also `goto done` 67 (272 bytes), the && flag guarding 0xbf9a0 with
// `if (excluded) return result;` 57 (287 bytes), a byte `found` 45 (same code), the 0xbf9a0 test positive with the
// requery after its block 56; before it, a byte flag `excluded |= 1` with the model query as its else-if 62 (threaded).
// Round 149 (m3 lane 5, fast with 0xd2bb0): 0x14af40's matching shape here (named `found`, requery in its then-arm, `!(excluded | found)`), the found arm
// storing the result 81 (291 bytes), `& 1` test 81, returning it 80 (286), store and return 79 (287): retail pushes EBX and EBP late here.
// Round 70 (m3 lane 14, fast with 0x14a8e0/0x14af40/0xd2bb0/0x14ac60/0x10a0b0/0xbf9a0): retail keeps source.collision in
// its stack slot (the requery's dead store to it stays, and only the model-query arm reloads it, before the un-threaded
// test), so forms that keep the parameter in memory: `(void)&source;` 53 (same code); the flag set in if/else arms with
// the collision re-read in the else arm and `if (excluded) return result;` 62, with `(void)&source;` 62, with the re-read
// at the join 62, the join re-read through `&source.collision` 62 (all threaded, 286 bytes).
// b11D (11-17) walk against retail, fast build on r843: fields, offsets, branch senses, argument order and requery
// arguments agree statement by statement (type 1/3 at 0x14b176-0x14b186, object 0x14b191-0x14b1a8, 0x10a0b0 `cmp ax, 2`
// 0x14b1b8, 0xbf9a0 stack pair +0xc/+0x10 0x14b1c2-0x14b1ce, 0x14a8e0 fed the unmodified source collision 0x14b225,
// 0x1697c0 args 0x14b1df-0x14b1f8, 0x14ac60 0x14b20b-0x14b20d): no behaviour difference found. Why the result sits in
// memory: BL is taken by the flag, a byte local then has no callee-saved byte register; a `bool` result drops the setne
// (the un-threaded forms below are 286 bytes with it). Un-threaded flag forms (declared `bool excluded`/`bool modeled`,
// `!(excluded | modeled)` as 0x14af40) all keep retail's `test` but allocate worse: NONE takes a callee-saved register
// (its pushes become `push reg`, `or reg, -1` after 0xbf9a0, retail uses `push -1`) and the flag is spilled into the
// dead source slot [esp+0x70]: object_index read before `found` 76, as is a function-scope flag or `modeled`, a
// literal -1 in the requery 76; found first 81; long flag 81; byte result 79-81 (290/291 bytes); collision only
// through source 81; flag by if-set 82; `!excluded` alone 93 (262 bytes); a plain collision pointer parameter 109.
// Best match-only recipe: none under 76. Needed: NONE in ECX only, flag in BL, source.collision spilled, not the flag.
// Round 228 (m3 lane 6, fast with 0xd2bb0): the three triangle NONE stores chained 55; the whole body nested under the
// availability test 99; that with an if/else flag (cleared in the else arm), the model result stored in `result` and
// g_4687bc read into a local at the join 131 (312 bytes); the 0xbf9a0 result tested through a `bool const &` 53 and the
// model-index field named as a `long *` for both of its uses 53 (same code each).
// Roasted-Codes (#1523 comment, 2026-11-26; fast reduced-image scores): after 0x14ac60 matched with its compiled-out
// `do {} while (false)`, that marker inside the arms instead of at the join (the notes have the join): after the
// requery's assignments, in the excluded early-return arm, and in a new empty found-miss else arm, 53 each (286 B, the
// body's code).
// @retail 0x14b120
bool function_14b120(s_surface_query_source source, void *surface)
{
    byte result = false;
    byte local_collision[0x5c];
    *(short *)(local_collision + 0x24) = NONE;
    if (!structure_bsp_collision_available())
        return false;
    s_structure_collision_result const *collision = (s_structure_collision_result const *)source.collision;
    s_structure_lightmap_triangle *triangle = (s_structure_lightmap_triangle *)surface;
    triangle->cluster_index = NONE;
    triangle->instance_index = NONE;
    *(long *)((byte *)triangle + 8) = NONE;
    bool found = collision->type == 1 || collision->type == 3;
    long object_index = *(long const *)((byte const *)collision + 0x40);
    if (object_index != NONE)
    {
        byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
        if (object[0xaa] == 6 && function_10a0b0(object_index) != 2)
            return result;
        if (!function_bf9a0(object_index, (long *)((byte *)triangle + 0xc),
            (long *)((byte *)triangle + 0x10)))
        {
            found = function_1697c0(0x4800005, &collision->point, g_4687bc, NONE, NONE,
                (s_collision_result_1697c0 *)local_collision);
            source.collision = local_collision;
            collision = (s_structure_collision_result const *)source.collision;
        }
        else
        {
            return function_14a8e0((s_structure_collision_result const *)source.collision,
                *(long *)((byte *)triangle + 0xc), triangle);
        }
    }
    if (found)
        result = structure_get_lightmap_triangle(collision, triangle);
    return result;
}

/* a list of ranges (0x48 bytes each), as unknown_16e290.cpp defines it */
struct s_16e1b0_range
{
	byte unknown00[6];
	word first;
	word count;
	byte unknown0a[0x48 - 0xa];
};

struct s_16e1b0_list
{
	long count;
	s_16e1b0_range *ranges;
};

void function_16e1b0(long value, s_16e1b0_list const *list, long *unknown, long *range_index, long *offset);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
bool function_11e800(point3f const *p, point3f const *a, point3f const *b, point3f const *c, real *u, real *v);

static inline bool structure_section_get_vertex(s_structure_section const *section, long index, point3f *point)
{
	s_structure_vertex_block const *block = &section->vertex_blocks[0];
	point3f const *vertices = (point3f const *)(block->data + block->offset);

	/* retail leaves the result unset on a miss: its three expanded calls share the
	   flag's byte and only ever store true to it */
	bool result;
	if (vertices && index >= 0 && index < block->count)
	{
		*point = vertices[index];
		result = true;
	}
	return result;
}
// @retail 0x14ac60
bool structure_get_lightmap_triangle(s_structure_collision_result const *collision, s_structure_lightmap_triangle *triangle)
{
	point3f point = collision->point;
	bool result = false;
	s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
	long surface_range_index = collision->surface_range_index;
	long plane_index = collision->plane_index & 0x7fff;
	s_structure_instanced_geometry_definition *definition = NULL;
	s_structure_section *section = NULL;
	long instance_index = collision->instance_index;
	s_structure_surface_range *range;

	if (collision->type == 1)
	{
		range = &bsp->surface_ranges[surface_range_index];
		s_structure_cluster_view *cluster = &bsp->clusters[range->cluster_index];

		if (function_12de70(&cluster->field_28, 3))
		{
			section = &cluster->sections[0];
		}
	}
	else if (collision->type == 3)
	{
		s_structure_instance *instance = &bsp->instances[instance_index];

		definition = &bsp->instanced_geometry_definitions[instance->definition_index];
		if (definition->surface_range_count == 0)
		{
			return false;
		}
		range = &definition->surface_ranges[surface_range_index];
		function_142700(&instance->matrix, &point, &point);
		if (function_12de70(&definition->field_28, 3))
		{
			section = &definition->sections[0];
		}
	}
	else
	{
		/* a statement the release build compiles out: retail keeps this arm's join as
		   its own block (the range is re-read there for the paths that skip type 1) */
		do {} while (false);
	}

	triangle->unknown18 = 0;
	if (section)
	{
		for (long i = range->first; i < range->first + range->count; i++)
		{
			s_structure_triangle_reference *reference = instance_index == NONE ?
				&bsp->triangle_references[i] :
				&definition->triangle_references[i];

			if (reference->surface_index == NONE)
			{
				continue;
			}

			s_structure_collision_surface *surface = instance_index == NONE ?
				&bsp->collision_bsps[0].surfaces[reference->surface_index] :
				&definition->surfaces[reference->surface_index];

			if (surface->plane_index != plane_index)
			{
				continue;
			}

			word *indices = &section->strip_indices[reference->first_index];
			long unknown;
			long part_index;
			long part_offset;
			point3f vertices[3];
			bool valid = true;

			function_16e1b0(reference->first_index, (s_16e1b0_list const *)section, &unknown, &part_index, &part_offset);
			for (long vertex = 0; vertex < 3; vertex++)
			{
				valid = valid && structure_section_get_vertex(section, indices[vertex], &vertices[vertex]);
			}
			if (valid && function_11e800(&point, &vertices[0], &vertices[1], &vertices[2], &triangle->u, &triangle->v))
			{
				triangle->part_offset = part_offset;
				triangle->lightmap_part_index = reference->part_index;
				triangle->unknown18 = unknown;
				triangle->part_index = part_index;
				triangle->cluster_index = range->cluster_index;
				triangle->instance_index = collision->instance_index;
				result = true;
				break;
			}
		}
	}
	return result;
}

#include "unknown_1efac0.h"

struct s_vertex_source_23ac30;
struct s_vertex_bounds_23ac30;
struct s_table_holder;
void *function_1efd80(s_table_holder *holder, dword position);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);
void function_23ac30(s_vertex_source_23ac30 const *source, long index,
	s_vertex_bounds_23ac30 const *bounds, point3f *point, long *node);

/* the collision record's object index, re-read at each use as retail does (no local copy) */
#define COLLISION_OBJECT_14A8E0 (*(long *)((byte const *)collision + 0x40))

// The vertex-bounds read sits before the geometry-block call (retail reads it there and spills it) and the lookup's
// address is a named pointer (retail keeps it in ESI for the second call); matched once 0x11e800 took the point first.
// @retail 0x14a8e0
bool __stdcall function_14a8e0(s_structure_collision_result const *collision,
	long model_index, s_structure_lightmap_triangle *triangle)
{
	bool result = false;
	{
		triangle->unknown18 = NONE;
		*(long *)((byte *)triangle + 8) = NONE;
		*(long *)((byte *)triangle + 0x14) = NONE;
		triangle->part_index = NONE;
		triangle->part_offset = NONE;
		triangle->lightmap_part_index = NONE;
		*(long *)((byte *)triangle + 0xc) = NONE;
		*(long *)((byte *)triangle + 0x10) = NONE;
		byte *model = g_4e3b44[model_index & 0xffff].bytes;
		byte *regions;
		long region_count;
		function_ba690(COLLISION_OBJECT_14A8E0, &region_count, &regions, NULL, NULL);
		byte *region_data = *(byte **)(model + 0x20) + (*(short *)((byte const *)collision + 0x44)) * 16;
		if (regions[(*(short *)((byte const *)collision + 0x44))] != 0xff)
		{
			*(long *)((byte *)triangle + 0x14) =
				*(short *)(*(byte **)(region_data + 0xc) + (char)regions[(*(short *)((byte const *)collision + 0x44))] * 16 + 0xc);
		}
		else
			*(long *)((byte *)triangle + 0x14) = 0xff;
		long section_index = *(long *)((byte *)triangle + 0x14);
		if (section_index == 0xff) goto done;
		byte *section = *(byte **)(model + 0x28) + section_index * 0x5c;
		s_vertex_bounds_23ac30 const *bounds = *(s_vertex_bounds_23ac30 const **)(model + 0x18);
		if (!function_12de70((s_geometry_block_info *)(section + 0x38), 3)) goto done;
		byte *object = *(byte **)(g_4e0300->data + (COLLISION_OBJECT_14A8E0 & 0xffff) * 12 + 8);
		point3f point = collision->point;
		long transform_count = *(short *)(object + 0x114) / sizeof(transform4x3f);
		if (transform_count > 0)
		{
			transform4x3f *matrix = (transform4x3f *)(object + *(short *)(object + 0x116)) + (*(short *)((byte const *)collision + 0x46));
			function_142700(matrix, &collision->point, &point);
			matrix = (transform4x3f *)(*(byte **)(model + 0x4c) + (*(short *)((byte const *)collision + 0x46)) * 0x60 + 0x28);
			function_142700(matrix, &point, &point);
		}
		s_lookup lookup;
		s_lookup *lookup_pointer = &lookup;
		if (!lookup_pointer->initialize(COLLISION_OBJECT_14A8E0)) goto done;
		byte *surface_set = (byte *)function_1efd80((s_table_holder *)lookup_pointer,
			*(dword *)((byte const *)collision + 0x48));
		byte *section_groups = *(byte **)(model + 0x80) + *(long *)((byte *)triangle + 0x14) * 8;
		byte *section_mesh = *(byte **)(section + 0x34);
		long node_count = *(long *)(section_mesh + 0x64);
		if (node_count <= 0) goto done;
		byte *nodes = *(byte **)(section_mesh + 0x68);
		long node_index = NONE;
		for (long node = 0; node < node_count; ++node)
		{
			if (nodes[node] == (*(short *)((byte const *)collision + 0x46)))
			{
				node_index = node;
				break;
			}
		}
		if (node_index == NONE) goto done;
		byte *surface_group = *(byte **)(section_groups + 4) + node_index * 16;
		if (*(long *)surface_group != *(long *)(surface_set + 0x10)) goto done;
		s_structure_surface_range *range = (s_structure_surface_range *)*(byte **)(surface_group + 4)
			+ collision->surface_range_index;
		for (long reference_index = range->first; reference_index < range->first + range->count; ++reference_index)
		{
			if (reference_index < 0 || reference_index >= *(long *)(surface_group + 8)) continue;
			s_structure_triangle_reference *reference =
				(s_structure_triangle_reference *)*(byte **)(surface_group + 0xc) + reference_index;
			long surface = reference->surface_index;
			if (surface == NONE) goto done;
			if (surface >= *(long *)surface_set) continue;
			byte *surfaces = *(byte **)(surface_set + 4);
			if (*(word *)(surfaces + surface * 8) != (collision->plane_index & 0x7fff)) continue;
			section_mesh = *(byte **)(section + 0x34);
			word *indices = *(word **)(section_mesh + 0x24) + reference->first_index;
			point3f vertices[3];
			long vertex_node;
			long vertex = 0;
			do
			{
				function_23ac30((s_vertex_source_23ac30 const *)section_mesh, indices[vertex],
					bounds, &vertices[vertex], &vertex_node);
				++vertex;
			} while (vertex < 3);
			if (function_11e800(&point, &vertices[0], &vertices[1], &vertices[2], &triangle->u, &triangle->v))
			{
				triangle->unknown18 = NONE;
				*(long *)((byte *)triangle + 8) = COLLISION_OBJECT_14A8E0;
				triangle->part_index = 0;
				result = true;
				s_16e1b0_list const *parts = (s_16e1b0_list const *)section_mesh;
				for (long part = 0; part < parts->count; ++part)
				{
					s_16e1b0_range const *part_range = &parts->ranges[part];
					if (triangle->part_offset >= part_range->first &&
						triangle->part_offset < part_range->first + part_range->count)
					{
						triangle->part_index = part;
						break;
					}
				}
				triangle->part_offset = reference->first_index;
				triangle->lightmap_part_index = reference->part_index;
				*(long *)((byte *)triangle + 0xc) = model_index;
				*(long *)((byte *)triangle + 0x1c) = (*(short *)((byte const *)collision + 0x44));
				*(long *)((byte *)triangle + 0x2c) = vertex_node;
				goto done;
			}
		}
	}
done:
	return result;
}

#undef COLLISION_OBJECT_14A8E0

/* excluded holds whether the hit object is left out of the model query. Nothing reads its incoming
   value (the only caller passes true); retail keeps the flag in BL and leaves the argument's slot alone. */
// MiniMax G (11-04): the result as a local with the parameter kept alive by an unused pointer 125 and 127;
// the entry order none/probe/flags/result 127; the exclusion flag assigned in the type-6 arm with the
// requery after the join 149; the join test as !(excluded & 1) 138. Body restored to the original.
// score 125 (was 125): frame 0x64 agrees; exclusion BL, NONE/EBX and spilled result allocation still differ.
// Lane 15 r17: the pointers to the direction, ignored object and surface parameters removed (each
// identical code; only the triangle pointer holds the convention: without it 158 and matched caller
// 0xd2a50 lost). Object branch first (`object_index != NONE`, retail's fall-through with `je` to the
// shared lightmap call) 130; with the exclusion set by an if 130; result as a local with the fourth
// parameter unread 131 (0xd2a50 lost); every parameter used plainly 156 (0xd2a50 lost). Retail gives
// ESI to the direction (reloaded after the calls that reuse ESI) and leaves the result in its
// parameter slot; ours gives the result EBX and NONE ESI.
// Earlier: retail keeps the exclusion flag in BL (`mov bl, 1` / `xor bl, bl`, tested again
// after the failed lightmap query), NONE in EBX and the result in its parameter slot; VC threads our
// flag. Flag as a helper out-parameter 168 (kept in memory); early-outs through the availability
// helper 149 (440 bytes, aligned closer); scenery getter call 146 (out of line); function-scope flag
// 125 (identical); early-out helper with an if/else flag 149.
// Existing model and collision probe declarations ordinary/inline/noinline, independently and
// jointly, 125 in all eight forms. Both original declarations restored; their definitions,
// arguments and the entire body are unchanged.
// Round 207: actual skipped/failed/found model state, signed byte/word/long/dword
// in object scope 134/136/133/133; function-scope declarations 134/136/133/133.
// Byte state reaches retail length; prologue/result/NONE allocation remains. All restored.
// Round 426: native byte/signed-byte/short/long retry state 125 each; all four with the existing point setter at retry advance 125 each. Original body restored; exclusion/result/direction allocation remains.
// Round 229: collision object observed at each use 125; complete eligibility observations 216,
// both 216; query success in the actual result 158, with object/eligibility/both 158/233/233;
// all with admitted-geometry result lifetime 267. Full original body restored.
// Lane 15 r19: retail's caller pushes the constant 1 for the third stack parameter and retail never
// reads that slot; retail's result byte lives in the surface parameter's slot (dead once the surface
// pointer is in EBP). Result as a local with the parameter unread 126 (LTCG drops the parameter,
// `ret 0xc`, matched caller 0xd2a50 56), plus `(void)&parameter` 126 (0xd2a50 56); the parameter read in
// the retry test 124 (471 bytes, 0xd2a50 kept; LTCG keeps the read); the two exclusion tests as one
// nested if 125 (same code), with the object branch first 130.
// Round 34 (m3 lane 14, fast): the exclusion flag homed in the unread bool parameter (result a local) as an &&
// expression or if-set 126/126 (LTCG drops the parameter, `ret 0xc`, 0xd2a50 lost; flag still threaded); the
// policy through the existing `function_10a0b0(object_index) != 2` 162 (450 bytes, 0xd2a50 lost); `if
// (!excluded) { if (!0xbf9a0) requery; else 0x14a8e0; }` 130 at 470 bytes (0x14a8e0 laid out last as retail,
// but the structure lookup call is duplicated where retail shares one between the no-object and requery
// paths); both 151; one shared structure lookup under a `found` flag, model query inverted or not 163/164 (448
// bytes, 0xd2a50 lost).
// Round 185 (lane 3, fast): the grouped state replaced by plain flags/attempts/collision locals 157
// (456 bytes, matched caller 0xd2a50 97); one 0x14ac60 call reached through a `found` flag 164 (0xd2a50
// 72); a compiled-out `do {} while (false);` at the requery join 125 (same code: VC drops it before
// threading); the existing 0x10a0b0 called, with `inline` on its definition, 162 (stays out of line,
// 0xd2a50 99); the flag and a `modeled` result set in if/else arms (false and the 0xbf9a0 result in
// the eligible arm) 125 (same code).
// Round 108 (m3 lane 7, fast): the flag as `type == 6 ? policy != 2 : false` 133 (470 bytes); `excluded = false;
// if (type == 6) { short policy = ...; excluded = policy != 2; }` 133 (470 bytes); the scenery test as an empty
// if-arm with the model query and requery as its else-ifs 125 (same code); the attempt limit as a `break` inside
// `if (!result && (bit & 1))` 125 (same code); a named `modeled` bool from `!excluded && 0xbf9a0` 125 (same
// code). Compile-only none of these keeps the flag in BL (VC threads it; a store at the join stops that, see
// 0x14b120's note).
// Round 25 (m3 lane 2, fast): the exclusion named in two parts (`bool scenery = ...; bool excluded = scenery &&
// ...`) 125 (same code); as `object[0xaa] == 6 ? policy != 2 : false` 133 (470 bytes, setne/test, still
// threaded); the retry test as `if (result || !(flags & 1) || ++attempts >= 3) break;` 125; the flag inverted
// (`bool lightmapped = type != 6 || policy == 2`) 125; no flag, the model query and requery nested under
// `if (type != 6 || policy == 2)` 125 (same code: the source's flag is threaded away completely). Diagnostic
// only (it requeries after a failed model lookup, so not keepable): `if (!result && !excluded)` in front of the
// requery keeps retail's `mov bl, 1; jmp` / `test bl, bl`, NONE in EBX with the back edge's `or ebx, -1` and
// the result in its parameter slot in a compile-only screen, but 157 (488 bytes) in the real build.
// Round 61 (m3 lane 9, fast): the object branch first with the probe inverted (`if (!excluded && !0xbf9a0)`
// requery, `else if (!excluded)` 0x14a8e0, the no-object lightmap in the else) 145 (456 bytes); the same
// inverted probe with the no-object case first 130 (470 bytes, retail's length; the 0x14a8e0 arm stays
// inside the loop where retail lays it out after it); the exclusion condition written out at both tests, no
// flag 216 (534 bytes: the policy loads repeat after 0xbf9a0; 0xd2a50 101); one lightmap call through `bool
// found = true;` (cleared for objects, set from the requery) with the model and requery under `if
// (!excluded)` 164 (448 bytes; 0xd2a50 72); the result parameter written through `bool *const
// result_reference = &result;` 125 (same code).
// Round 27 (m3 lane 2, fast): a `found` local (`object_index == NONE`, set by the requery) with one shared
// lightmap call after the object part 157 (455 bytes, matched caller 0xd2a50 67); that with the excluded arm
// clearing `found` 159 (453 bytes, 0xd2a50 72); the flag declared at function scope and cleared before the loop
// 125 (same code); the object part as an if/else-if chain with an empty excluded arm 125 (same code);
// `excluded = false; if (scenery) excluded = policy != 2;` 133 (470 bytes, setne, threaded).
// Round 32 (m3 lane 2, fast; 0xd2a50 in brackets): the surface parameter read once through its address in place
// of the pointer to the triangle local 125 (identical code [kept]); result as a local with the unread parameter
// read into it (a dead read) 126, or dead-written, 126 (LTCG drops the parameter either way, ret 0xc [56]); the
// result reset moved to just before the loop 136; the probe copy before the flags computation 128. Retail never
// reads that stack parameter (orig+0xc, the caller pushes 1) and spills the result into the surface pointer's
// dead slot; only 0xd2a50 calls it.
// Round 39 (m3 lane 15, plain): the flag preset true and cleared by the positive test 125 (same code); the flag set
// from the unread third stack parameter in the scenery arm with result a local 133 (471 bytes: the parameter is
// read and threaded on, no constant propagation); `direction` read once through its address at the join 125
// (folded); a long flag 125; the probe result in a `probed` flag tested before `else if (!excluded)` 125.
// Diagnostic (volatile read of `direction` at the join, reverted): 152 at 480 bytes, but the flag then takes
// retail's shape exactly (`mov bl, 1; jmp`, `xor bl, bl` before 0xbf9a0, &triangle->c in ESI with the direction
// reloaded, result in memory), so the threading of the flag is the one blocker.
// Round 110 (m3 lane 7, fast): the flag through a function-scope `excluded` copied from a `scenery_excluded` &&
// local inside the requery's else-if test (diagnostic: an assignment in a test) 125 (same code); `*probe = *point;`
// first at entry 128; `result = false;` first 130; the collision's NONE store first 130; the probe step through the
// existing `vector3f_add_scaled(probe, direction, 0.02f, probe)` 125 (same code).
// Round 113 (m3 lane 7, fast): the existing `object_get` for the object lookup 125 (same code); the probe copy
// first, then flags, the NONE short and `result = false` 128; the result parameter typed `byte` at the definition only
// 119 at 470 bytes (the caller's `bool` declaration no longer resolves, 0xd2a50 96; result still in BL: an artefact);
// a shared force-inline exclusion helper (object lookup + 0x10a0b0) for this and 0x14b120 125 (0x14b120 106); the
// requery's result as `result = query && lightmap(...)` 174 (505 bytes).
// Round 286: Native collision storage 125; unsigned flags 125; bounded short/byte attempts
// 124/129; native handle observations 125; separate flags/all scalar workspace 157/157
// (caller lost); direct triangle 158 (caller lost). Original storage and executable body restored.
// Round 63 (m3 lane 16, fast): a bit-and join test (`else if (!(excluded & 1))`, diagnostic) keeps the flag
// unthreaded and gives retail's allocation (flag in BL with `mov bl, 1; jmp` and `xor bl, bl` before 0xbf9a0,
// the direction reloaded into ESI, NONE in EBX with `or ebx, -1` at the back edge, the result in its
// parameter slot) but `test bl, 1` and a different layout: 149 (477 bytes); with one lightmap call reached
// through a `found` flag 139 (471 bytes); either with the existing availability helper 148 (470 bytes: the
// failure exit inline as retail's, but ESI pushed after it) / 150; the plain form with `*probe = *point`
// before the flags computation 128.
// Round 221: coordinated byte result parameter and caller declaration 119; independent retry count 126;
// existing initial point setter 142; both 143. Signed-byte parameter on those four graphs
// 119/126/142/143. Byte and signed-byte forms retain the caller at 119, but gain six and fail the
// eight-point gate; setter forms lose the matched caller. Original Boolean parameter, caller
// declaration, grouped storage, point copy and entire body restored. Frame 0x64 agrees; exclusion BL,
// result stack home, direction ESI and NONE ownership remain.
// DH round 264: Result-owned pretested retries with independent flags/long-short-word-byte attempts
// and native collision fields 157/156/156/160; with the existing initial point setter
// 144/145/145/157. All regress; original record layout, grouped state, triangle access and
// body restored; result/NONE/exclusion allocation and direction reloads remain.
// Round 294: Initial probe memmove/three-long triangle memset/two-byte material memset
// subsets 1/2/4/3/5/6/7 give 140/144/125/138/140/144/138; all with the availability
// producer 158. Original body restored; exclusion/result/NONE and direction homes remain.
// Round 298: Finite eligibility/retry/outcome domains and pairs/all
// 125/125/131/125/131/131/131; full finite graph with three-outcome model 164.
// Original Boolean parameter, grouped storage and complete body restored;
// exclusion/result/NONE ownership and direction residency remain.
// Round 300: Complete positive-admission/done-exit availability graphs with native short/long
// policy merges, default/type-arm and conditional producers:
// 136/136/147/147/136/136/147/147. Original complete body restored; exclusion/result/NONE and direction homes remain.
// Round 197 (lane 3, fast with 0xd2a50): a `modeled` flag set in the scenery test's else arm 125; a false probe arm with the
// flag set after 0x14a8e0 125 (same code), as an else-if chain 125; `requery` preset true and cleared in both arms 125, or
// from `!excluded` with the 0x14a8e0 arm clearing it 161 (485 bytes); the excluded result cleared in a nested else 137 (471
// bytes). The join test stays threaded in all of them. Body restored.
// Round 519: Complete direction value shared by both probes and retry advance,
// admitted/per-pass samples with original eligibility 177/165; positive eligibility
// 177/165; existing availability producer 142/147; both changes 142/147.
// All lose the matched caller; original direction observations and complete body restored.
// Round 343: BSP/header/object-handle last-consumer scope subsets 1..7 each 125;
// joint object-phase scopes with the existing availability helper 149 (440 bytes).
// Original availability, workspace, exclusions and body restored; NONE/result/exclusion homes remain.
// Round 125 (lane 8, fast with 0xd2a50): the guards as one positive test around the body with `return false`
// after it 125; `retry = true` before the probe advance 125; `bool retry = true; while (retry)` 125; the
// direction copied to a local for every use (diagnostic) 125; the exclusion preset false and set by an if 125;
// an empty excluded arm with the model query and requery as its else-if/else 125 (same code each: the flag is
// still threaded, the result keeps EBX and the direction no register). Body unchanged.
// Round 138 (m3 lane 5, fast): availability through the existing inline helper 149 (440 bytes), with `result = false`
// after it 169; `result = false` after the inline availability tests 136; the requery flags assigned in the call
// argument 125 (same code); diagnostic: the requery's lightmap call sharing the no-object call through a label 165
// (0xd2a50 lost).
// Round 123 (m3 lane 7, fast with 0xd2a50): both availability guards through the existing inline
// structure_bsp_collision_available() 149 (440 bytes, a repeat re-measured); the flag declared beside `retry` and
// assigned in the object arm 125; both flag tests as `excluded == false` 125; the direction read once into a
// loop-top pointer for both queries and the step 125; the flag set in if/else arms 125; preset false and set
// true under a nested policy test 125 (all the same code: VC still threads the flag).
// DH315: One result exit on original exclusion/storage graph; shared sentinel,
// entry attempt initialization and availability producer masks 0..7 give
// 136/136/144/144/136/136/144/144, all 452/470 bytes. Original exits,
// reset phases, exclusion, grouped workspace and complete body restored.
// Round 81 (m3 lane 9, fast with 0xd2a50): retail holds NONE and then the exclusion flag in EBX (`mov bl, 1` /
// `xor bl, bl`, re-set by `or ebx, -1` at the back edge), the direction in ESI and the result only in its parameter
// slot; ours threads the flag, keeps NONE in ESI and the result in BL. The triangle used directly without the
// reference pointer 158 (454 bytes, 0xd2a50 72); that with the state struct split into three locals 157 (456,
// 0xd2a50 97); the split alone 157 (0xd2a50 97); a named cluster pointer for the 0xbf9a0 output and its read 138
// (473); the exclusion preset false and assigned the definition test inside `if (type == 6)` 133 (470 bytes, setne).
// DL255: Fixed-three forward attempt counters, long/dword/short/word: 122/122/118/118;
// reverse remaining counters: 123/140/119/139. Extents 470/470/470/470/470/460/470/461.
// NOT kept: gains at most seven; narrow counter/layout forms require equality. Frames
// stay 0x64, first +3 and exclusion/result/NONE ownership remain. Original complete
// body, grouped workspace, triangle reference, retry state and query operands restored.
// Zero/one exclusion in signed-byte/short/word compound producers 125/125/125;
// complete branch producers 133/133/133; short/word conditional producers 133/133.
// Original Boolean exclusion, result parameter, grouped storage and complete body restored.
// DH335: collision-selection input char/signed-char/byte 125 each; short/word 126,
// int/long/dword 130. Wider inputs lose the matched caller. Original Boolean
// contract and whole body restored; exclusion/result/NONE and direction homes remain.
// N351: Native output long fields +08/+0c/+10, subsets 1/2/4/3/5/6/7, then model-output pointer:
// 125/125/125/125/125/125/125/124. NOT kept: no exact match or qualifying gain; complete original body restored.
// Remaining: exclusion/result/NONE homes and direction reload.
// Round 162 (lane 4, fast with 0xd2a50). LEAD, not kept (diagnostic forms): one lightmap call reached through a
// `hit` flag (true, cleared in the object arm, set from the requery, `if (hit) result = lightmap`) gives retail's shared
// `test al, al; mov [slot], al` tail and the result in memory, 164 at 448 bytes while the flag threads (0xd2a50 lost,
// probe in ESI). With the join test as `!(excluded & 1)` (or `excluded ^ 1`) 139 at 471 bytes and retail's allocation;
// plus the existing structure_bsp_collision_available() 150 at 461, entry order result, NONE short, probe copy, flags 112
// (n,p,f,r 112; p,n,r,f 113; p,f,r,n 130; f,p,r,n 150); plus result a local and the unread parameter kept by an unused
// `bool const *flag_reference = &parameter` 59 at 470 bytes, aligned 28, 0xd2a50 matching (a plain address cast, or an
// empty inline taking it, drops the parameter: ret 0xc, 114). Only three things then differ: the NONE short's store
// before the result's, `test bl, 1` for `test bl, bl`, and the found arm, inline in ours and after the step in retail.
// So what is left is the join test staying unthreaded with a plain `test bl, bl`, and the found arm's place.
// Threaded on that 59 form (167, 432 bytes; aligned 155): join test as `&` or `|` of itself, `&& itself`, `(long)` == 0, > 0,
// a reference, a pointer or a copy of the flag at the join, `while`, `do/while(false)`, `for` or `switch` around
// the requery, an empty inline before it, a known-false `hit` or `retry` as the compound's first element or second;
// `-(long)excluded == 0` stays unthreaded 65 [24]. On the kept body: a statement in the else before the test 125; result
// local 126 (parameter dropped, 0xd2a50 lost); the flag in the unread parameter 126 (dropped); `found` set in arms or
// from the && test, then if/else-if, 125 each; the requery and found call in an inline taking the flag 155 at 486 bytes
// (first test stays, second threads); negative dispatch with the found arm in the else 125, with `hit` 164; 16-128
// unused locals 164 (no tracking limit); the found arm at a label after the step (goto, diagnostic) 59 (ours moves it
// back inline); `found` tested after the lightmap call with `!found && !excluded` 182 at 428 (found arm after the
// lightmap, jumped over; the flag still threads), that with found before lightmap 174, as else-if 182, the flag set by if 183;
// for(;;) with breaks on the 112 form 144 (505 bytes).
// N369: Pass-owned false-result initialization with independent availability,
// generation and retry scopes, masks 0..7: 146/146/146/171/146/146/146/171.
// NOT kept: all lose the matched caller (14), regress and miss native extent;
// original entry-result lifetime, guards and complete body restored.
// Lane 5 r149 (held, coordinator): MATCH with the parameter renamed `unused`, a fresh `bool result`, a dead
// `bool const *unused_reference = &unused;` (retail never reads it, but its slot is address-taken), surface read once
// through its address, the BSP test through structure_bsp_collision_available, and the object path as a `hit` flag
// with the requery under `!(excluded | found)`. Without the dead address line 109 (fast). Not kept: a parameter-address
// line that is never read is reverted even with a match (10-19 ruling). Patch build/held/lane5-r149-14af40-unused-address.patch.
// N378: Complete object completion chain with eligibility/model/fallback results in the actual return result:
// 181/133/181/133/200/147/200/147. NOT kept: no byte equality or qualifying native-shape gain;
// original executable source restored; exclusion/result/NONE and direction homes remain.
// Lane 12 r92: matches (0) with `bool const *unused_reference = &unused; __assume(*unused_reference);` plus
// `__assume(!found)` in the !found arm, hit/found flags and plain locals; held, not kept: the assume on the
// unread parameter states a value nobody checks (patch build/held/lane12-r92-14af40-assume-unused.patch).
// T60: Overwritten result-parameter char/short/word/int/long/dword/unsigned-int domains
// 119/115/115/118/118/118/118; independent long result 120 (caller lost). NOT kept:
// wider result operations lack native byte storage/equality; other gains below margin.
// Original Boolean contract/body restored; exclusion/result/NONE and direction homes remain.
// Round 216 (m3 lane 3, fast with 0xd2a50): the existing availability helper with the entry order NONE short, probe
// copy, flags, result 151 (440 bytes); that entry order alone 127; the flag in two steps (type test, then `excluded =
// policy != 2` under it) 155 (480 bytes, sete/setne); the first query as the `while` condition with one `break` retry
// test 155 (480 bytes, 0xd2a50 lost); the flag as a byte member of the state aggregate 152 (483 bytes). Body restored.
// @retail 0x14af40
bool function_14af40(bool include_objects, point3f const *point, vector3f const *direction,
    long ignore_object_index, bool excluded, void *surface, point3f *probe)
{
    s_structure_lightmap_triangle *triangle = (s_structure_lightmap_triangle *)surface;
    long flags;
    long attempts;
    byte collision[0x5c];
    *(short *)(collision + 0x24) = NONE;
    bool result = false;
    *probe = *point;
    flags = include_objects ? 0x4480000d : 0x4800c0d;
    if (!structure_bsp_collision_available()) return false;
    triangle->cluster_index = NONE;
    triangle->instance_index = NONE;
    *(long *)((byte *)triangle + 8) = NONE;
    attempts = 0;
    bool retry;
    do
    {
        retry = false;
        if (!function_1697c0(flags, probe, direction, ignore_object_index, NONE,
            (s_collision_result_1697c0 *)collision)) break;
        long object_index = *(long *)(collision + 0x40);
        bool hit = true;
        if (object_index != NONE)
        {
            byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            excluded = object[0xaa] == 6 && *(short *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0xc0) != 2;
            hit = false;
            bool found = !excluded && function_bf9a0(object_index, (long *)((byte *)triangle + 0xc),
                (long *)((byte *)triangle + 0x10));
            if (!found)
            {
                if (!(excluded | found))
                {
                    flags = 0x4800005;
                    hit = function_1697c0(flags, probe, direction, NONE, NONE,
                        (s_collision_result_1697c0 *)collision);
                }
            }
            else
                result = function_14a8e0((s_structure_collision_result *)collision,
                    *(long *)((byte *)triangle + 0xc), triangle);
        }
        if (hit)
            result = structure_get_lightmap_triangle((s_structure_collision_result *)collision, triangle);
        if (!result && (collision[0x58] & 1) && ++attempts < 3)
        {
            probe->x += direction->i * 0.02f;
            probe->y += direction->j * 0.02f;
            probe->z += direction->k * 0.02f;
            retry = true;
        }
    } while (retry);
    return result;
}
