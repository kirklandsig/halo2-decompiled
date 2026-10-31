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

// score 61 (was 61): VC threads the exclusion flag on every edge (retail keeps it in BL: `mov bl, 1;
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
        if (function_bf9a0(object_index, (long *)((byte *)triangle + 0xc),
            (long *)((byte *)triangle + 0x10)))
        {
            return function_14a8e0((s_structure_collision_result const *)source.collision,
                *(long *)((byte *)triangle + 0xc), triangle);
        }
        found = function_1697c0(0x4800005, &collision->point, g_4687bc, NONE, NONE,
            (s_collision_result_1697c0 *)local_collision);
        source.collision = local_collision;
        collision = (s_structure_collision_result const *)source.collision;
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
bool function_11e800(point3f const *a, point3f const *b, point3f const *c, point3f const *p, real *u, real *v);

static inline bool structure_section_get_vertex(s_structure_section const *section, long index, point3f *point)
{
	s_structure_vertex_block const *block = &section->vertex_blocks[0];
	point3f const *vertices = (point3f const *)(block->data + block->offset);

	if (vertices && index >= 0 && index < block->count)
	{
		*point = vertices[index];
		return true;
	}
	return false;
}

// score 172 (was 172): Frame 0x58 agrees; BSP/section carriers (ours keeps the section in EBX and
// the BSP in EBP, retail the BSP in EBX with the section in its stack home), range/plane homes and
// vertex joins remain. Retail never initialises its vertex flag (set only when a vertex exists,
// tested only after the first vertex); that uninitialised success-only flag gives 165 (163 with
// other declaration orders; 119 with an uninitialised definition), but it reads an uninitialised
// flag and drops the checks on the second and third vertices, so HEAD's body stays.
// Tried: output declarations reordered 185; outputs renamed 185; part index/unknown stores swapped
// 185; bool result 197; success-only flag 235, with bool result 221; instance index at each use
// 226/227/201; vertex loop with a break 287; inverted ternaries 165; surface range index at each use
// 191; flag initialised false 232; early return without a section 127 (an epilogue retail lacks;
// realignment only); BSP reads at use 217; conditional section selection 229; direct success returns
// 243; instance reads at use 213; relative triangle-range counter 206; reference table before
// traversal 221; instance case first 232; byte success result 185; result-controlled traversal 205;
// reference table and result traversal 216; index-first/output-first fetch contracts 185/185;
// filled-point return 185; native span inputs 238; actual validity publication 185; triangle query
// into result 217; validity-byte name 185; read-only section 185; shared residency 203; first-fetch
// validity 185; strip-input cursor 221; residency/first 203; residency/cursor 208; first/cursor 221;
// all three 208; both invariant geometry tables on the byte-result body 241; byte vertex validity 185;
// range index at use 199; cached range end 183 (below gate); later plane 211; native strip start 220;
// external section source 191; active section reference 185; surface table before walk 205; source
// and table 244; plane reads at use 225; collision record reference 185; long success/validity
// 192/185; native range 211; native reference 225; native section 216; native BSP 272; native
// collision 256; range and reference 218; section and range 227; collision and reference 248;
// BSP/range/block references 185/185/185; native block value 330; vertex count/memory copy 185/185;
// native strip-word input 257; range plus block references 185; failure accumulator bool/byte
// 185/185; signed byte/word fetch result 185/185; negative-index guard first 211; index test before
// data 208; failure plus early guard 211; failure accumulator plus signed-byte fetch 185; range/plane,
// range/instance, plane/instance and all metadata reads at every use 225/217/226/223, with BSP also
// read at every use 231/221/232/222; private point setter 230; dword index 258; both 256; dword fetch
// result 185; result with setter/index/both 230/258/256; complete input point captured before the
// shared setter 319; validity output pointer/reference/byte 185/185/185; predicate-produced bool/byte
// 185/185; predicate byte with native block 185; Boolean/byte outputs with native block 185/185;
// final failure bool/byte/short/long 182/182/192/194 (three-point best dropped); existing point setter
// 231; cluster reads at use 216; setter/cluster 245; instance receiver at use 188; setter/instance 228;
// both receivers 220; setter/both 251; native coordinate initializer 231; separate source/query points
// with entry/default initialization byte 233/233, bool 222/222, CRT byte 233/233, bool 222/222;
// eligible source-point return with section/block inputs and aggregate/CRT caller copies 266 each;
// validity assigned only on failure with those four forms 192 each; native-byte geometry,
// metadata/ordinal, query/triangle workspace and part-output lexical homes 185 each, all groups 185;
// two-use collision discriminator, reused reference surface ordinal and readonly BSP/instance inputs,
// all seven combinations 185 each; combined unsigned discriminator 185; part/residency/transform
// provider inline contracts, singles, pairs, all and all with separate noinline declarations 185 each
// (transform forms lose reduced controls 0x31590/0x168d60, also 0x23d030 without noinline); native/
// const fetch source capture with aggregate/library publication 185 each, each with query memmove
// 207; range declared before section 185; BSP declared last 186; range or BSP local renamed 185/185
// (spill slots unchanged); section local renamed 185 (names move neither slots nor registers);
// vertex fetches as one && chain with the triangle test 257 (677 bytes); vertex helper taking the
// block, read once per triangle 238; vertex getter returning the point or NULL 255; nested ifs in
// place of the continues 185 (same code), with the result in the loop test 221.
// Round 380: geometry-join byte/signed-byte result with source-owned definition and
// residency-owned section default subsets 208/172/216/217 each; original entry byte/bool
// result with source-owned definition 195/207. Branch-owned definition null default
// and geometry-join byte result 172 (733/726 bytes), plain confirmed; retained.
// Frame 0x58 agrees; BSP/section residence, range/plane homes and vertex joins remain.
// Round 384: strip receiver after readonly part lookup with original/readonly
// strip/reference/both 201/201/201/201; each with the two-use native first-index snapshot
// 230/230/230/230.
// Original strip acquisition, native byte result and complete defined-failure body restored;
// BSP/section and range/plane homes remain.
// Round 249: Complete pointer/lower/upper eligibility exits in all six orders
// 185/188/208/210/187/186; native bool/byte bounds predicates 254/254. Original
// defined-failure helper and complete query restored; BSP/section homes remain.
// Round 223: Signed/unsigned word masked-plane storage with original/reference-base/surface-base/both table selections 213/188/214/210/215/196/216/224. Original full-width plane, table selectors and defined vertex failures restored.
// Round 165: actual instance-table predicate shared at both selections, crossed with
// native byte fetch and the reused surface ordinal, 228 in all four forms; the same
// four with a canonical Boolean query result 232 each. Predicate shortens the frame;
// original long instance lifetime, helper, byte result and defined failures restored.
// Geometry-join Boolean result 182 (726 bytes); native byte fetch 172; both 182;
// initialized single-exit fetch 172, with Boolean result/byte fetch/both 182/172/182;
// complete result/fetch graph with readonly section 182. Original helper and result restored.
// Round 32 on branch-owned definition/result-at-join: all BSP observations 234;
// all collision metadata observations 228; CRT query snapshot 172; BSP/metadata 231;
// BSP/copy 234; metadata/copy 228; all 231, instance-first all 222. Original body restored.
// Round 251: bounded vertex byte/signed-byte/word/unsigned-long ordinals
// 259/259/259/172; each with a complete post-geometry range-end expression
// 269/269/269/196. Original ordinal and bounds restored; frame/BSP/section and
// defined vertex joins remain.
// Round 387: BSP/section carriers and range/plane homes remain. Native fetch output/section/both
// references 172 each; remaining index-output-section, output-index-section and section-output-
// index contracts 172 each; output reference with index-first and both references with output-
// first 172 each. Original query and complete defined-failure helper restored.
// Round 387: Three further native fetch reference contracts, index-output-section with section
// reference, output-index-section with point reference and section-output-index with both
// references: 172 each. Eight new reference contracts total; three pointer-only order observations
// are additional. Complete original query and defined-failure helper restored.
// Round 393: native external cluster/instance/triangle references in seven subsets
// 172 each; all with native surface reference 172. Original receivers and defined-failure body restored.
// Round 228 on the branch-owned geometry join: filled-output pointer success with native
// section/output references and index-first order, all eight contracts 172 each. Original
// Boolean fetch, defined failures and caller roles restored; BSP/section residence remains.
// Round 400: Existing vertex getter as a section member with bool/byte/word/long results
// 172/172/172/172; as a vertex-block member
// 172/172/172/172. Every defined failure and suppressed later fetch restored.
// Round 185: Direct collision-point transform input with aggregate/library/overlap-safe/shared-
// setter copies at entry 199/199/205/243; case-owned copies 251/251/257/296. Original in-place
// transform, point capture, defined vertex failures, complete body and include set restored.
// Round 187: Native byte-data/offset versus point-table fetch inputs, long/word count and
// early/result-first eligibility, all eight 235. Original fetch contract and body restored.
// Round 403: real triangle-reference cursor with readonly metadata/surface table/range
// bound/vertex cursor combinations 210/210/211/193/213/210/213; complete graph
// with Boolean result 224. Original traversal and defined-failure body restored.
// Round 207: native point-value getter with bool/byte/long/dword validity outputs,
// default-first result 284/284/278/278; eligibility-owned return 284/284/278/278.
// Frames 0x68/0x6c and shorter code regress; original Boolean fetch and defined failures restored.
// Round 426: captured instance key through publication with long/short/word/dword plane storage 232/180/197/232; Boolean results 242/187/194/242. Original collision observations and defined vertex failures restored.
// Round 229: failure-only in/out validity on the original point-output helper, bool/byte/short/long
// pointer outputs 172/172/172/172; references 172/172/172/172. Original helper, defined failures and query restored.
// @retail 0x14ac60
bool structure_get_lightmap_triangle(s_structure_collision_result const *collision, s_structure_lightmap_triangle *triangle)
{
	point3f point = collision->point;
	s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
	long surface_range_index = collision->surface_range_index;
	long plane_index = collision->plane_index & 0x7fff;
	long instance_index = collision->instance_index;
	s_structure_instanced_geometry_definition *definition;
	s_structure_section *section = NULL;
	s_structure_surface_range *range;

	if (collision->type == 1)
	{
		definition = NULL;
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
		definition = NULL;
	}

	byte result = false;
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
			if (valid && function_11e800(&vertices[0], &vertices[1], &vertices[2], &point, &triangle->u, &triangle->v))
			{
				triangle->part_offset = part_offset;
				triangle->lightmap_part_index = reference->part_index;
				triangle->part_index = part_index;
				triangle->unknown18 = unknown;
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

// score 70 (was 70): frame 0x58 and initial model/collision homes agree; bounds timing,
// lookup-address reuse and triangle-test registers differ.
// Earlier: with the part list read through its 0x48-byte range records and the part offset read
// at each use, the model/collision registers (EBX/EDI swapped), lookup address (retail keeps it
// in ESI for the second call) and BLOCKED-BY 0x11e800's argument registers differed.
// with the part list read through its 0x48-byte range records and the part
// offset read at each use, only the model/collision registers (EBX/EDI swapped), the lookup address
// (retail keeps it in ESI for the second call) and BLOCKED-BY 0x11e800's argument registers differ.
// Round 8 (lane 10): range-record list 95 (kept); part offset at each use 78 (kept); section groups
// after the node search 208; bounds read at its use in the vertex call 274; separate local for the
// second object lookup 78; lookup declared at the top 78; object lookup before the model 219;
// diagnostic: the bounds read before the geometry-block call gives 9 (registers as retail; left
// are the lookup address and 0x11e800's registers), held because it moves a one-use read.
// Round 10 (lane 10): model lookup before the triangle resets 83; model and object lookups there 88;
// point copied before the object re-read 78; section index local dropped 78 (same code).
// Earlier: region code and node search match; retail reads the model's bounds before
// the geometry-block call (that read moved there gives 143 with the section groups ahead of the node
// search, but it moves a definition read ahead of a call), keeps the vertex cursor in EBX and the
// mesh spilled, and BLOCKED-BY 0x11e800: its vertices/point registers (ours eax/esi/edx/ecx, retail
// eax/edx/ecx/esi) come from the contributor's body. Round 94: bool result 237 (886 bytes); signed
// count from sizeof (jle) 236; region pointer before the test with both 195; plus NONE-result node
// search 181 (kept); node count >= test 262/206; section groups after the surface lookup 254/214/177
// (swaps model and collision registers unless the bounds read moves too); for vertex loop 181; part
// range fields at their uses 180.
// Round 97: bounds before the geometry-block call with section groups before the node search 143
// (only BLOCKED-BY 0x11e800 registers, slots and the vertex cursor follow), same with a renamed
// section local 143 (names do not move slots), with a for vertex loop 143; bounds with the model
// lookup 221, plus early section groups 214.
// Frame 0x58 agrees; region scheduling and range/vertex homes remain. Split
// region/transform receivers 236; early section groups 254; both 254; byte return with
// split/groups/both 237/255/255; signed-byte result with split/groups 236/254.
// region scheduling and model/range/vertex homes remain.
// Region scheduling and model/range/vertex homes remain; staged region base
// 236; library point copy 236; both 236; false result at first failure 242; read-only
// reference/range 236/236; late false with reference 242; chained part resets 238.
// 0x58 frame; region scheduling/model spill remain.
// Retained mesh 258; unsigned node ordinal 240; masked plane snapshot 264;
// native surface-table view 240; relative range ordinal 252.
// Region scheduling and model/node storage still differ;
// direct surface-group expression 240; byte success result 244; word section ordinal 247;
// direct group and byte result 244; late bounds and byte result 236.
// cached reference end 250; native strip start 244; no search sentinel 248;
// shared point setter 238; positive parts scope 243; byte return 237.
// Region base at each use 268; surface lookup name 236; local point name 236; triangle vertex name
// 236; node/corner scalar names 236; node count at uses 236; native part-bound locals 236;
// section field at early uses 236.
// region scheduling and model/range/vertex homes remain. Fast trials: separate
// NONE result 269; separate count result 262; unsigned candidate and NONE result 269; unsigned
// candidate and count result 262; entry NONE-result lifetime 269; entry count-result lifetime 262;
// node pointer search 270; bounded mismatch scan 236. All restored.
// Model lookups at uses 298; refreshed model 284; read-only nodes 236; extraction-output lifetime 236;
// strip cursor 242; read-only strip 236; output lifetime plus strip 242; model uses plus strip 300.
// Cache inline/noinline, lookup inline/noinline, both inline/noinline, and
// mixed inline/noinline pairs 236 each; all declaration metadata restored.
// Safe signed transform count and direct region-record expression 236 each;
// section/range/reference/index/parts storage names and combined signed-count homes 236 each.
// Extraction-helper inline/noinline 236/236; unsigned/short/word vertex ordinal 236/248/248; initialized vertices/node/both 303/243/304. All helper declarations restored. Region schedule and model/range/vertex homes remain.
// Native surface-group/range/both roots through reused byte offsets 269/241/300;
// reference/group-reference/range-reference/all roots 247/294/265/329; all with bool result 320.
// Bounds/node/part table fields at every use 284/236/243; bounds-node/bounds-part/node-part
// graphs 258/286/243; all three/all three plus part-count field 239/239. All restored.
// Round 212: complete native range mutable/const 193/193; native reference mutable/const
// 194/194; both mutable/const 208/208; mesh field at every use 205; both const records with
// mesh field observations 214. Original records, observations and protected interfaces restored.
// DH round 189: Native lookup/extraction-array/node-output work-scope lifetimes, singles, pairs,
// all and reversed all 181 each; all original producer/read phases restored.
// Round 215: first transform in place 210; with model/section/both const views
// 210/210/210; const object/mesh views 181/181; in-place with const mesh/all
// four const views 210/210. Original input point, views and body restored.
// Round 62: section groups read right after the surface lookup, as retail does 177 (kept; model
// and collision swap EBX/EDI); after the node count/count test/nodes/sentinel 182/186/186/186;
// with the stored section index replaced by the local 206; object lookup before the model 202.
// Round 150: section test and geometry-block call in one condition 78; lookup pointer for the
// second call only 78; region index named for its four uses 254; the matched siblings' lookup
// pointer (`s_lookup *lookup_pointer = &lookup;` for both calls) 77 (retail's ESI reuse), and with
// the diagnostic bounds read 4: left are only 0x11e800's argument registers (ours second/third/point
// in esi/edx/ecx, retail edx/ecx/esi), so both forms wait on that contributor body.
// Round 202: Native mesh node/strip/parts fields, seven combinations, and all with const receiver 78/78/78/78/78/78/78/78 fast.
// Original executable source restored. Frame 0x58 agrees; model/collision roles, bounds acquisition, lookup-address reuse and triangle-test inputs remain.
// Round 76: the bounds read moved down to just before the lookup 70 (kept: model/collision
// take retail's EBX/EDI; left are the bounds read's place, the lookup-address reuse and
// BLOCKED-BY 0x11e800's registers); the bounds read after the object re-read 77, after the
// point copy/transform count 212, after the lookup initialize 105; const model/section/table
// views 78; bounds declared with the model, assigned in place 78; after the kept change: the
// surface lookup through an inline (lookup, position) helper 69, forced inline 262 (inlines
// 0x1efd80); lookup declared at the top 70; lookup declared before the bounds read 70;
// separate local for the transform object 70.
// Round 194: shared first/second/both object acquisitions, node-matrix accessor alone,
// either/both acquisitions with that accessor and shared headers with it 78 each.
// Model/collision homes, bounds timing, lookup reuse and triangle-call inputs remain.
// Round 209: Borrowed collision-point input with original/branch-owned point production, do/for vertex
// loop 70/70/70/70; corresponding bounded short vertex ordinals 102/102/102/102. Original body
// restored; bounds timing, lookup reuse and triangle-test registers remain.
// Round 78: the work after the lookup nested under `if (lookup.initialize(...))` 70; the lookup
// guard as `return result` 231; a const bounds pointer 70; the lookup and surface lookup in their
// own block 70; section groups from the section index local 157. Diagnostic: the bounds read
// before the geometry-block call still gives 9 (left: the lookup address, 0x11e800's registers).
// Round 81: the region permutations through the existing 0xba690 call, which LTCG inlines here as
// retail does (kept: identical code, its body no longer written out); with the bounds read before
// the geometry-block call 9, plus the lookup pointer 4 (only BLOCKED-BY 0x11e800's argument
// registers left, so neither form can be kept yet); lookup declared at the top 70; a region
// index local 262.
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
		s_vertex_bounds_23ac30 const *bounds = *(s_vertex_bounds_23ac30 const **)(model + 0x18);
		s_lookup lookup;
		if (!lookup.initialize(COLLISION_OBJECT_14A8E0)) goto done;
		byte *surface_set = (byte *)function_1efd80((s_table_holder *)&lookup,
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
			if (function_11e800(&vertices[0], &vertices[1], &vertices[2], &point, &triangle->u, &triangle->v))
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
// @retail 0x14af40
bool function_14af40(bool include_objects, point3f const *point, vector3f const *direction,
    long ignore_object_index, bool result, void *surface, point3f *probe)
{
    s_structure_lightmap_triangle *triangle = (s_structure_lightmap_triangle *)surface;
    s_structure_lightmap_triangle *const *triangle_reference = &triangle;
    struct { long flags; long attempts; byte collision[0x5c]; } state;
    state.flags = include_objects ? 0x4480000d : 0x4800c0d;
    result = false;
    *(short *)(state.collision + 0x24) = NONE;
    *probe = *point;
    if (!g_4e0344 || g_4e0344->count <= 0 || !g_4e0348) return false;
    byte *bsp = (byte *)g_4e0344->bsp;
    if (*(long *)(bsp + 0x1c) == NONE || *(long *)(bsp + 4) != *(long *)((byte *)g_4e0348 + 8)) return false;
    (*triangle_reference)->cluster_index = NONE;
    (*triangle_reference)->instance_index = NONE;
    *(long *)((byte *)*triangle_reference + 8) = NONE;
    state.attempts = 0;
    bool retry;
    do
    {
        retry = false;
        if (!function_1697c0(state.flags, probe, direction, ignore_object_index, NONE,
            (s_collision_result_1697c0 *)state.collision)) break;
        long object_index = *(long *)(state.collision + 0x40);
        if (object_index == NONE)
            result = structure_get_lightmap_triangle((s_structure_collision_result *)state.collision, *triangle_reference);
        else
        {
            byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            bool excluded = object[0xaa] == 6 && *(short *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0xc0) != 2;
            if (!excluded && function_bf9a0(object_index, (long *)((byte *)*triangle_reference + 0xc),
                (long *)((byte *)*triangle_reference + 0x10)))
                result = function_14a8e0((s_structure_collision_result *)state.collision,
                    *(long *)((byte *)*triangle_reference + 0xc), *triangle_reference);
            else if (!excluded)
            {
                state.flags = 0x4800005;
                if (function_1697c0(state.flags, probe, direction, NONE, NONE,
                    (s_collision_result_1697c0 *)state.collision))
                    result = structure_get_lightmap_triangle((s_structure_collision_result *)state.collision, *triangle_reference);
            }
        }
        if (!result && (state.collision[0x58] & 1) && ++state.attempts < 3)
        {
            probe->x += direction->i * 0.02f;
            probe->y += direction->j * 0.02f;
            probe->z += direction->k * 0.02f;
            retry = true;
        }
    } while (retry);
    return result;
}
