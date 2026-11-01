// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_23AA70.CPP: a pass over the structure's clusters */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

/* the structure's clusters (0xb0 bytes each), as this file reads them */
struct s_cluster_23aa
{
	byte unknown00[0x7c];
	short value7c;
	word count7e;
	byte unknown80[0xb0 - 0x80];
};

struct s_cluster_spawn_23aa;

struct s_structure_23aa
{
	byte unknown00[0x9c];
	long cluster_count;
	s_cluster_23aa *clusters;
	byte unknowna4[0xf4 - 0xa4];
	long valuef4;
	s_cluster_spawn_23aa *entries;
};

void function_17d710(short cluster_index);

/* calls 0x17d710 on every cluster that has a value7c and a count */
// @retail 0x23aa70
void function_23aa70(void)
{
	s_structure_23aa *structure = (s_structure_23aa *)g_4e0348;

	if (structure->valuef4)
	{
		for (short i = 0; i < structure->cluster_count; i++)
		{
			s_cluster_23aa *cluster = &structure->clusters[i];

			if (cluster->value7c != NONE && cluster->count7e > 0)
			{
				function_17d710(i);
			}
		}
	}
}

struct s_cluster_spawn_23aa
{
	point3f position;
	byte palette_index;
	byte unknown0d;
	char yaw;
	char pitch;
};

struct s_cluster_palette_23aa
{
	long unused;
	long tag_index;
};

struct s_cluster_palette_globals_23aa
{
	byte unknown00[0x144];
	s_cluster_palette_23aa *types;
};

struct s_effect_source;
void function_17e670(s_effect_source *source, point3f const *point, long tag_index,
	vector3f const *direction, real radius, long first, long second, long third);

/* a cluster's bit in a mask with one bit per cluster */
#define CLUSTER_MASK_TEST(mask, index) ((mask)[(index) >> 5] & (1 << ((index) & 0x1f)))

/* Starts and stops cluster effects when the active cluster masks change. */
// score 16 (was 16): frame and prefix agree; palette/entry register ownership differs from +0xba.
// BLOCKED-BY 0x17e670: its body takes the effect source in edx and the point in ecx here,
// retail passes the source in ecx and the point in edx (also at 0x17a8a0).
// Shared entry 17; entry plus palette 17; shared index 32; const shared entry 17;
// direct tag 52; tag after angles 28; tag after direction 52; shared direction helper 17.
// Further trials: entry/palette ownership and contributor effect-call convention remain.
// uniform structure 125; cluster at use 92.
// spawn at use 82; structure/cluster 105.
// structure/spawn 121; cluster/spawn 136.
// all three owners 149; all owners/tag at call 150.
// Position pointer 16; aggregate direction 16; direction helper with position 16;
// that combination with shared entry 17; entry pointer arithmetic 16;
// full-width first entry 32; short first entry 16; short spawn cursor 78.
// Round 70: direct mask casts 16; mask pointers at function scope 16 and loop scope 16;
// a const cluster 16; const structure and cluster 16; the entry declared before the tag 16;
// shared entry with the tag after the angles 28 and after the direction 52.
// @retail 0x23aad0
void __stdcall function_23aad0(long previous_address, long current_address, long count)
{
	s_structure_23aa *structure = (s_structure_23aa *)g_4e0348;
	if (structure->valuef4)
	{
		for (short i = 0; i < count; i++)
		{
			s_cluster_23aa *cluster = &structure->clusters[i];
			if (cluster->value7c != NONE && cluster->count7e > 0)
			{
				dword const *previous_mask = (dword const *)previous_address;
				dword const *current_mask = (dword const *)current_address;
				if (CLUSTER_MASK_TEST(previous_mask, i) && !CLUSTER_MASK_TEST(current_mask, i))
				{
					function_17d710(i);
				}
				else if (!CLUSTER_MASK_TEST(previous_mask, i) && CLUSTER_MASK_TEST(current_mask, i))
				{
					for (long j = 0; j < cluster->count7e; j++)
					{
						long tag_index = ((s_cluster_palette_globals_23aa *)g_4e0350)->types[structure->entries[cluster->value7c + j].palette_index].tag_index;
						s_cluster_spawn_23aa *entry = &structure->entries[cluster->value7c + j];
						real yaw = entry->yaw * 0.02473695f;
						real pitch = entry->pitch * 0.012368475f;
						vector3f direction;
						direction.i = (real)(cos(pitch) * cos(yaw));
						direction.j = (real)(cos(pitch) * sin(yaw));
						direction.k = (real)sin(pitch);
						function_17e670(0, &entry->position, tag_index, &direction, 1.0f, 1, NONE, 0);
					}
				}
			}
		}
	}
}

struct s_vertex_bounds_23ac30
{
	real x0, x1, y0, y1, z0, z1;
};

struct s_vertex_buffer_23ac30
{
	byte format;
	byte unknown01[7];
	long offset;
	byte const *data;
};

struct s_vertex_source_23ac30
{
	byte unknown00[0x3c];
	s_vertex_buffer_23ac30 const *buffer;
};

struct s_vertex_with_node_23ac30
{
	point3f position;
	byte node;
	byte unknown0d[3];
};

struct s_packed_vertex_23ac30
{
	short x, y, z;
};

struct s_packed_vertex_with_node_23ac30
{
	short x, y, z;
	byte node;
	byte unknown07;
};

PRIVATE __forceinline void vertex_decode_ranges_23ac30(short x, short y, short z,
	s_vertex_bounds_23ac30 const *bounds, vector3f const *range, point3f *point)
{
	point3f result;
	result.x = (((real)x * 2.0f + 1.0f) * 0.000007629510946571827f + 0.5f) * range->i + bounds->x0;
	result.y = (((real)y * 2.0f + 1.0f) * 0.000007629510946571827f + 0.5f) * range->j + bounds->y0;
	result.z = (((real)z * 2.0f + 1.0f) * 0.000007629510946571827f + 0.5f) * range->k + bounds->z0;
	*point = result;
}

PRIVATE __forceinline void vertex_decode_23ac30(short x, short y, short z,
	s_vertex_bounds_23ac30 const *bounds, point3f *point)
{
	vector3f range = { bounds->x1 - bounds->x0, bounds->y1 - bounds->y0, bounds->z1 - bounds->z0 };
	vertex_decode_ranges_23ac30(x, y, z, bounds, &range, point);
}

/* reads one vertex position (and its node, for the formats that store one) from a
   buffer in one of four formats; packed formats decode through the bounds */
// score 62 (was 62): format 3 loads the buffer data into EBP where retail uses EBX, and
// format 4 forms a vertex pointer where retail addresses every field as base + index * 8
// and keeps the index for the late node read. Indexing format 4 gives exactly retail's
// format-4 code but the allocator then swaps EBX/EDI (node arrives in EBX, the source on
// the stack): 175. Tried: indexed format 4 or both packed arrays 175, format 2 62, format 3
// 60; node formats/ordinary index/range producer 175; concrete packed pointer 62; concrete
// reference 62; native value 173; coordinate-array pointer 62; coordinate-array reference 62;
// returned point 62; native input/output 62; concrete indexed input 175; indexed native
// decoder 582; ordinary index 582; native input reference/value 578/549; native axis bounds
// 62; range value input 267; range output reference 62; natural format case order 127;
// static indexed/input templates 582/578; shared point constructor 228; aggregate point 62;
// range stages 62; range decoder/producer inline 62/537; buffer fields at use 190; ordinary
// scalar index 62; packed cases together 111; node/plain/both point memcpy 62/62/62;
// node/packed/both record references 62/62/62; native coordinate loop 62; native range and
// coordinate loops 62; format-3 sum written offset first 62; formats 3 and 4 indexed 175;
// format-4 pointer for the coordinates with the node read by index 175; the index read once
// into a local for every case with format 4 indexed 175; indexed format 4 with node declared
// before point (definition, declaration and call) 175; node before point alone 62 (caller +2);
// formats 2, 3 and 4 all indexed 175; format-4 record as a packed position member plus node,
// indexed/pointer 175/62; with format 4 indexed, the format read into a local 175, one node
// store after the switch 187, the index read directly 175; the buffer pointer read at each use
// 190; the index read through its own address (index_reference) dropped 62 (kept); a shared
// node store after the switch with formats 3/4 returning 172, with format 4 indexed 175.
// One-file LTCG harness: only the format-4 form decides the convention (removing any other case
// never flips it); format 4 keeps retail's EDI node and EDX source only when its node read
// re-derives the buffer base or comes before the decode, instead of keeping base and index live.
// Round 99 (harness, then real build): every one of the 120 parameter orders flips the same way
// with formats 3/4 indexed (the signature is not the lever); a caller-shaped harness entry flips
// too; 256 combinations of formats 2/3 indexed, an inline node-store helper, default first/last,
// break/return, buffer at each use, `!= NULL` and data+offset order all flip; formats 3/4
// indexed with the node-store helper 175 (real); the node stored before the decode 73 (real,
// convention kept, harness aligned 19).
// format-4 code but the allocator then swaps EBX/EDI (node arrives in EBX, the switch index and
// base take EDI, the source moves to the stack): 175. Tried: indexed format 4 or both packed
// arrays 175, format 2 62, format 3 60; node formats/ordinary index/range producer 175; concrete
// packed pointer 62; concrete reference 62; native value 173; coordinate-array pointer 62;
// coordinate-array reference 62; returned point 62; native input/output 62; concrete indexed
// input 175; indexed native decoder 582; ordinary index 582; native input reference/value
// 578/549; native axis bounds 62; range value input 267; range output reference 62; natural
// format case order 127; static indexed/input templates 582/578; shared point constructor 228;
// aggregate point 62; range stages 62; range decoder/producer inline 62/537; buffer fields at use
// 190; ordinary scalar index 62; packed cases together 111; node/plain/both point memcpy
// 62/62/62; node/packed/both record references 62/62/62; native coordinate loop 62; native range
// and coordinate loops 62; format-3 sum written offset first 62; formats 3 and 4 indexed 175;
// format-4 pointer for the coordinates with the node read by index 175; the index read once into
// a local for every case with format 4 indexed 175; indexed format 4 with node declared before
// point (definition, declaration and call) 175; node before point alone 62 (caller +2); formats
// 2, 3 and 4 all indexed 175; format-4 record as a packed position member plus node,
// indexed/pointer 175/62; with format 4 indexed, the format read into a local 175, one node store
// after the switch 187, the index read directly 175; the index read through its own address
// (index_reference) dropped 62 (index stays on the stack: kept); a shared node store after the
// switch with formats 3/4 returning 172, with format 4 indexed 175. One-file LTCG harness: only
// the format-4 form decides the convention (removing any other case never flips it); format 4
// keeps retail's EDI node and EDX source only when its node read re-derives the buffer base.
// Round 23 (lane 10): with format 4 indexed: the coordinates read into short locals first 175; a
// const node parameter 175; the switch on a format-minus-one local 175; the full base expression
// at each format-4 access 107 (636 bytes, retail convention), with format 3 indexed too 105.
// store after the switch 187, the index read directly 175 (the source still arrives on the
// stack, the switch index in EDI, the node in EBX); the buffer pointer read at each use 190;
// the index read through its own address (index_reference) dropped 62 (index stays on the
// stack: kept); format 3 indexed 60 (retail's format-3 code exactly, EBX for the offset); a
// shared node store after the switch with formats 3/4 returning 172, with format 4 indexed
// 175; one vertex-base local shared by every case, format 4 indexed 201; format 4 indexed with
// the node stored before the decode 75 (keeps retail's convention, EBX base, EDI node, EDX
// source: only the node store's position differs, but 0x14a8e0 168); the node read through the
// re-derived base 192 (buffer fields reloaded after the point stores); the full base
// expression at each format-4 access 107 (keeps the convention, but reloads the buffer after the
// point stores); the array local named to sort before `node` 175 (names do not break the tie). One-file LTCG harness: only the
// format-4 form decides the convention (removing any other case never flips it); format 4
// keeps retail's EDI node and EDX source only when its node read re-derives the buffer base
// (the full base expression at each access, or the node read through it) instead of keeping
// base and index live.
// indexed/pointer 175/62; with format 4 indexed, the format read into a local 175, one node store after the switch 187, the index read directly 175 (the source still arrives
// on the stack, the switch index in EDI, the node in EBX); the buffer pointer read at each use 190;
// the index read through its own address (index_reference) dropped 62 (index stays on the stack: kept);
// format 3 indexed 60 (retail's format-3 code exactly, EBX for the offset); a shared node store after
// the switch with formats 3/4 returning 172, with format 4 indexed 175. One-file LTCG harness: only the
// format-4 form decides the convention (removing any other case never flips it); format 4 keeps
// retail's EDI node and EDX source only when its node read re-derives the buffer base (the full base
// expression at each access, or the node read through it) instead of keeping base and index live.
// Round 38 (lane 9), format 4 indexed through a `vertices` local (175: switch temp/base in EDI, node
// in EBX, source on the stack): the base read through `source->buffer` 97 (node back in EDI, source
// and buffer then in EBP/ESI); a byte base pointer cast per access 175; the node read through a
// re-derived `buffer` base 192 (636 bytes) or `source->buffer` base 175.
// Round 120 (format 3 indexed in each): format 4 with the full base expression at each access 105 (636
// bytes: the base is re-derived after the point stores); a byte data local cast to the record array
// 175; the node read with a long cast 175; `node != NULL` 175 (each with the node in EBX and the source
// on the stack); formats 2 and 3 indexed with the format-4 pointer 60. Parameter reorders need the
// caller in src/structures.cpp.
// Round 19 (lane 13), format 4 indexed through a `vertices` local: with a const index parameter 175;
// with format 3 indexed too 175 (registers masked, only 13 aligned differences: the convention flip is
// the whole gap); with the format read into a long local for the switch 175; the coordinates through a
// coordinate pointer and the node through the full base expression 107 (636 bytes); the decode written
// in both arms of the node test 261.
// Round 6 (lane 17): formats 3 and 4 indexed with format 4 ending `if (!node) break;` 175, with format 4
// returning after its node store 175, with format 4's node byte read before the decode 190 (636 bytes);
// format 4's pointer declared `const *const` 62; formed as data + offset + index * sizeof in bytes 62.
// Round 11 (m3 lane 2), one-file LTCG harness (base convention reproduced; format 4 indexed flips it:
// the switch temp and every case's base take EDI and the node EBX, so the node is coloured before
// the format-4 base once that base stays live to the late node read): formats 2/3/4 pointer or
// indexed in all 12 combinations, format 2 indexed gives the same code as its pointer; `if (node !=
// NULL)` and braces on the indexed form flip. Real, format 3 indexed with format 4 indexed and its
// node stored before the decode: 73; with format 2 indexed too 73; with `node != NULL` 73; with
// `return` for `break` 73; with the default case first 73.
// Round 177, all with format 4 indexed through a `vertices` local (175: the source moves to the stack, the
// node to EBX, the switch temp and every case's base to EDI; otherwise exactly retail's code): the node
// parameter renamed to sort after the array local (vertex_node, a_node) 175/175 (same code); `if (!node)
// break;` before the format-4 node store 175; a `wants_node` bool for the five node tests 193 (636 bytes);
// `switch (buffer->format - 1)` with cases 0-3 175; the decode helper `inline` instead of `__forceinline` 521
// (kept out of line).
// Round 112 (fast; 0x14a8e0 70 with the convention kept, 269 when it flips): format 4 through a `vertices`
// local with the node read through a pointer formed after the decode 175 (624 bytes, flipped); the node test
// around the decode (decode and node store in one arm, decode alone in the else) 261 (696 bytes); format 3
// indexed with the format-4 pointer 60 (as before: two points); `vertices + index` as the pointer 62 (same
// code); the node byte read through a byte cast of the indexed record 175. A /Gd compile-only first pass
// reloads every parameter per case here, so it shows nothing about the flip.
// Round 8 (lane 17), formats 3 and 4 indexed through `vertices` locals (175, 624 bytes: exactly retail's code
// with EBX and EDI swapped, the node in EBX unsaved, the switch temp and every base in EDI, the source on the
// stack, ret 8; 0x14a8e0 269): format 4's coordinates as a `short position[3]` member 175; the default case
// written first 175 (same code); the decode helpers taking long coordinates 175; the switch on
// `source->buffer->format` 175; `source->buffer` at every use 186 (632 bytes, 0x14a8e0 99).
// Round 99 (lane 8): the buffer read once inside each case 62; format 1 through a vertex pointer 62; a const buffer
// pointer 62; format 3's position copied member by member 102; the default case's node test on its own line 62.
// Round 20, all with formats 3 and 4 indexed through `vertices` locals (175, flipped: node EBX,
// switch temp and bases EDI, source on the stack, `ret 8`): both array locals declared at the
// function top 175; formats 1, 2 and default zeroing the node once after the switch with 3/4
// returning 175; an unsigned index (definition and declaration) 175; format 4's node byte read into a
// local before the decode (stored after) 174 (0x14a8e0 283); formats 3/4 returning after their node
// stores 175. Only the node store's position (before the decode, 73) keeps the convention.
// Round 10 (lane 17), formats 3 and 4 indexed through `vertices` locals (175, 624 bytes, caller 269): the index
// parameter as `word` (definition, declaration, call) 154 (628 bytes: retail's convention and EDI node kept,
// caller 158, but the index is loaded into BX and re-extended per case, so format 4 forms a pointer again); as
// `dword` 175; the decode written as a macro (no parameter temporaries) 175; the index read once through its own
// address into a local used by every case 175; the bounds as three lo/hi pairs 175.
// Round 57 (lane 16, fast with 0x14a8e0), formats 3 and 4 indexed unless noted (175, flipped): one function-scope
// byte `vertex_data` assigned in every case and cast per access 175; the cases written in reverse order 176 (same
// layout); format 4's decode and node store as one forceinline helper taking the records and index 175; format 4
// through `data`/`offset` locals re-forming the base at each access 89 (retail's convention and format 3's EBX
// offset kept, but format 4 re-associates to `offset + index*8` plus data and forms a pointer again, and the EBP
// push sinks into format 2), with `offset + data` 89. Diagnostic: format 2's node store before its decode with 3/4
// indexed 194 (still flipped): only format 4 keeping base and index live across its decode flips the node to EBX.
// Round 43 (lane 10), format 4 indexed through `vertices` (175, flipped): case 4 written before case 3
// 170 (620 bytes, still flipped: node EBX, switch temp/bases EDI, source on the stack); with format 3
// indexed too 170; with format 2 indexed too 170; case 4 written first of all 175; the index parameter
// renamed (vertex_index) 175. Format 3 indexed alone 60 again (gain under 3, not kept).
// Round 143 (fast, faithful here), format 4 indexed through `vertices`: the range built in the case and
// the ranges decoder called directly 175; the decode helper taking bounds and point first 175; a shared
// inline data + offset accessor in all four cases 175; decoded into a case-local point stored before the
// node 175. Diagnostics: without format 2's zero store 177, without all three 173 (node on the stack).
// Round 146 (fast), formats 3 and 4 indexed: format 4's record copied into a local first 197 (640 bytes);
// a decode helper returning the point by value (`*point = decode(...)`, formats 2 and 4) 173; the range
// vector built first in each packed case and the ranges decoder called 175; one function-scope `data`
// local assigned in every case and cast per access 175; a coordinate-array decode helper given
// `&vertices[index].x` with the node read by index 175. Each still flips (node EBX, switch temp EDI,
// source on the stack, `ret 8`); retail's constant only appears in this function.
// Round 122 (lane 6), formats 3 and 4 indexed through `vertices` locals: the decode returning the point by
// value 175; an axis loop over the three packed coordinates 184 (632 bytes); bounds as the decode's first
// parameter 175; a per-axis decode helper (value, lower, upper) 177 (628 bytes); the result as an aggregate
// initialiser 175. One-file LTCG harness (definition held out of line, reproduces both conventions): format 4
// indexed with `*node = 0` (nothing live after the decode) still flips, so the late node read is not the
// cause; a node read through the buffer pointer (buffer live across the decode) keeps retail's convention;
// build flags do not move the convention.
// Round 107 (lane 7): one `vertices` byte base (data + offset) before the switch for every case 201 (588 bytes,
// flipped, 0x14a8e0 269); formats 3 and 4 indexed with a trailing `return;` 175; format 4 indexed with the buffer
// data in its own local 175, with a braced node store and format 1 as pointer + index 175, with the node byte read
// into a long before the decode 174 (0x14a8e0 283). All flipped as before: the node takes EBX and the switch
// temp/case-4 base EDI, where retail gives that web EBX and the node EDI.
// @retail 0x23ac30
void function_23ac30(s_vertex_source_23ac30 const *source, long index,
	s_vertex_bounds_23ac30 const *bounds, point3f *point, long *node)
{
	s_vertex_buffer_23ac30 const *buffer = source->buffer;
	switch (buffer->format)
	{
	case 1:
		{
			point3f const *vertices = (point3f const *)(buffer->data + buffer->offset);
			*point = vertices[index];
			if (node) *node = 0;
		}
		break;
	case 2:
		{
			s_packed_vertex_23ac30 const *vertices = (s_packed_vertex_23ac30 const *)(buffer->data + buffer->offset);
			vertex_decode_23ac30(vertices[index].x, vertices[index].y, vertices[index].z, bounds, point);
			if (node) *node = 0;
		}
		break;
	case 3:
		{
			s_vertex_with_node_23ac30 const *vertices = (s_vertex_with_node_23ac30 const *)(buffer->data + buffer->offset);
			*point = vertices[index].position;
			if (node) *node = vertices[index].node;
		}
		break;
	case 4:
		{
			s_packed_vertex_with_node_23ac30 const *vertices = (s_packed_vertex_with_node_23ac30 const *)(buffer->data + buffer->offset);
			vertex_decode_23ac30(vertices[index].x, vertices[index].y, vertices[index].z, bounds, point);
			if (node) *node = vertices[index].node;
		}
		break;
	default:
		*point = *g_468788;
		if (node) *node = 0;
		break;
	}
}
