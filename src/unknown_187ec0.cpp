// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_187EC0.CPP: queries on the first local player, and the
   lookups of the globals' block at +0x150 of g_4e034c (0xb4 byte elements) */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "impacts.h"
#include "main_messages.h"
#include <math.h>

struct s_player_datum
{
	byte unknown00[0x24];
	long index24;
	byte unknown28[4];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_player_record
{
	byte unknown00[0x1a];
	bool flag1a;
	byte unknown1b;
};

extern byte g_51ea18[];

struct s_pitch_object
{
	byte unknown00[0x18c];
	vector3f vector;
};

struct s_object_header
{
	byte unknown00[8];
	s_pitch_object *object;
};

static inline s_player_datum *player_get(long player_index)
{
	return (s_player_datum *)g_4e8c24->data + (player_index & 0xffff);
}

// score 3 (was 3): ours computes the record address in ecx and copies it into esi, retail computes it in esi
// directly; ours is padded with three int3s.
// Round 15: the record pointer computed before the index test 5 (retail's three instructions, ahead of the test);
// the index defaulted to NONE outside the player test 3; the player field read at each use with the pointer 3;
// the shared first-local-player helper 3 (kept: identical code); the pointer set under the test and tested 16.
// Every form without a pointer local (direct copy, memcpy, a typed table, value/out/flag lookups, a reused
// index) loads the index into esi (16).
// Round 12 (lane 18): the record with its real field layout (two reals, a 16-byte map, a short; 4-byte aligned) 3,
// that with the direct copy 16; `&table[index]` 3; byte arithmetic with sizeof 3, direct 16; the copy as the
// declaration's initializer 3; the record declared before the pointer 3; the index as dword (cast or local) 3,
// direct with the cast 16; a table-base pointer local at the top 3; the global declared with its size 3; the
// permuter (58 annealed tries) best 3.
// @retail 0x187ec0
bool function_187ec0(void)
{
	bool result = false;
	long index = local_player_first_index();

	if (index != NONE)
	{
		long record_index = player_get(g_4e8c20->entries[index])->index24;
		if (record_index != NONE)
		{
			s_player_record const *local_0 = (s_player_record *)g_51ea18 + record_index;
			s_player_record record;
			record = *local_0;
			result = record.flag1a;
		}
	}
	return result;
}

// @retail 0x187f30
real function_187f30(void)
{
	real result = 0.0f;
	long index = local_player_first_index();

	if (index != NONE)
	{
		long player_index = g_4e8c20->entries[index];
		bool const &valid = player_index != NONE;
		if (valid)
		{
			long unit_index = player_get(player_index)->unit_index;
			if (unit_index != NONE)
			{
				vector3f vector = ((s_object_header *)g_4e0300->data)[unit_index & 0xffff].object->vector;
				real const &i = vector.i;
				real const &j = vector.j;
				real const &k = vector.k;
				result = (real)atan2(k, sqrt(i * i + j * j));
			}
		}
	}
	return result;
}

/* the 0xb4 byte elements of the block at +0x150 of g_4e034c */
struct s_globals_element_value
{
	long value;
	byte unknown04[4];
};

struct s_globals_element
{
	long key;
	byte unknown04[4];
	short parent_index;
	byte unknown0a[0x3c - 0xa];
	s_globals_element_value first_values[7];
	s_globals_element_value second_values[7];
	byte unknownac[4];
	long tag_index;
};

struct s_globals_element_block_view
{
	byte unknown00[0x150];
	long count;
	s_globals_element *elements;
};

/* an index into a tag block, returned through memory */
class c_block_index
{
public:
	c_block_index(short index) : m_index(index) {}
	short m_index;
};

// @retail 0x1885f0
c_block_index function_1885f0(s_globals_element_block_view const *globals, long key)
{
	long count = globals->count;
	short result = NONE;
	for (long index = 0; index < count; ++index)
	{
		if (key == globals->elements[index].key)
		{
			result = (short)index;
			break;
		}
	}
	return c_block_index(result);
}

static inline s_globals_element *globals_element_get(s_globals_element_block_view const *globals, short index)
{
	s_globals_element *result = NULL;

	if (index != NONE && index >= 0 && index < globals->count)
	{
		result = &globals->elements[index];
	}
	return result;
}

inline s_globals_element *function_188640(long key);
// @retail 0x188640
s_globals_element *function_188640(long key)
{
	s_globals_element_block_view *globals = (s_globals_element_block_view *)g_4e034c;

	long const *local_0 = &key;
	c_block_index local_1 = function_1885f0(globals, *local_0);
	return globals_element_get(globals, local_1.m_index);
}

// @retail 0x188690
s_globals_element *function_188690(short index)
{
	short const *local_0 = &index;
	long local_1 = *(long const volatile *)local_0;
	return globals_element_get((s_globals_element_block_view *)g_4e034c, (short)local_1);
}
/* the entries of a seat's animations, by element of the globals block */
struct s_seat_animation_entry
{
	byte unknown00[4];
	long value04;
	byte unknown08[4];
	long value0c;
	byte unknown10[4];
	short element_index;
	byte type;
	byte unknown17;
};

struct s_seat_animation
{
	byte unknown00[8];
	long first_count;
	s_seat_animation_entry *first_entries;
	long second_count;
	s_seat_animation_entry *second_entries;
};

struct s_seat_animations
{
	long count;
	s_seat_animation *animations;
};

// score 55 (was 55): retail's ebp is not a frame pointer: it holds animation for the whole body, and result lives
// in the push ecx slot ([esp + 0x10]), stored on a hit and reloaded into edx after the inner loop. Ours keeps result
// in ebp and reloads animation from its parameter slot at every use; that register-or-spill choice is the whole
// difference. Retail's inner loop has no result test (a hit stores and breaks, then index = parent and the outer
// loop leaves on result), but every form with that shape builds the inner loop like retail while giving type eax
// instead of edx, which costs matched caller 0x188130. Tried: inner loop without the !result test and the pointer
// &entries[i] as a local 71 (0x188130 1); entries[i] indexed 66 (0x188130 1); while (!result && element) 71; a
// do-while 71; no first local 87; result declared first 71, last 79; the element_index parameter as the index 77;
// first ? &first_entries[i] : &second_entries[i] 80; count in the for condition 99; count inside the outer loop 77
// (0x188130 20); the search as an inline helper 101; a found index then the pointer 105; i < count && !result
// with that shape 63 (0x188130 20); animation before or after type in the parameter list 71.
// Count declared before result 65 (with the entry pointer and no guard 79: result in eax).
// The entry pointer, no guard and `if (result) break;` before `index = parent` gives retail's
// first 13 instructions (first in the animation's slot, result in the push ecx slot,
// animation in ebp) but 64 at 178 bytes: element_index then arrives in ecx (0x188130 20)
// and the loop tail is duplicated; with the parameter as the walking index 76, index
// declared first 64, element declared first 64; with element_index read once through its
// address 64 (element_index back on the stack, 0x188130 matches; left: count/element
// registers swapped, the 16-bit index load, the duplicated tail); that read with the
// parent step before the test 71 (type in eax).
// Round 116: the index as the material class (parameter and walking local) gives retail's 16-bit
// index load, 55 (c_block_index the same). On it: count before result 57; first/index/count/result
// order 55; no inner guard with count first 65; class + entry pointer + no guard + count first 71
// (158 bytes); that with `if (result) break;` before the parent step 63 (185 bytes: retail's
// animation/result homes, but the tail is duplicated and count/element swap EAX/EBX), with a short
// index local 64. Without the inner guard the first pass loads type into ESI unless count is
// declared before result.
// Round 118 (one-file harness, ~900 forms): renames, loop spellings, scopes, getter shapes, parameter
// orders and inline count/entry helpers change nothing. `entry = first ? &animation->first_entries[i] :
// &animation->second_entries[i]` with no guard gives retail's inner loop and tail (result spilled to the
// push ecx slot, reloaded into EDX) but allocates animation to EBX, so LTCG passes it there: 78 (157 bytes,
// 0x188130 17); with animation read once through its address 52 (retail's code with animation/count/element
// in EBX/EBP/EAX for retail's EBP/EAX/EBX; no declaration order, counter or flag type, compare order or loop
// spelling rotates them); with the guard 76 (0x188130 17), with count first 79, its if/else spelling 79.
// Round 120: ours computes the getter's element address in ECX (`add ecx, esi`) and copies it out; retail
// forms it in element's register (`lea ebx, [ecx + esi]; test ebx, ebx`), as in 0x1882d0. Getter shapes
// (byte offset, elements local, inverted early return or ternary, else arm, long index, index + elements)
// keep the add. `while (s_globals_element *element = ...)` with the per-arm entry and the guard 79
// (155 bytes: animation in EBP, element spilled, result in EAX); that with the address read and no guard
// 52; the getter in a for loop's init and step 164 (243 bytes); the while declaration with the entries
// pointer, no guard and the break before the parent step 63 (185 bytes); animation before type in the
// parameter list with the address read 52.
// Round 82: explicit entry byte offset 53; offset with compared-entry result 71 (0x188130 1); count before result,
// no inner guard, offset and compared entry 71 (158 bytes, caller matched); result declared first but initialized
// after count 71; per-arm entry 85 (187); single-exit label 71; element declared before result 71; pre-tested inner
// while 71. All restored; the offset's two-point gain was below the retention threshold.
// Round 44: native entry-index equality, reversed receiver, compared-entry result, count-first
// unguarded result, per-arm entry, true entry cursor and promoted traversal with/without inner
// result guard 55/55/61/71/78/64/80/66. Native type restored; result/animation allocation remains.
// Round 13 (fast): result tested in the outer loop's condition, `while (!result && (element = ...) != NULL)`,
// no inner guard and no break after the parent step 65 (type arrives in EAX and goes to ESI, result in EAX,
// 0x188130 1); with the entry pointer `&(first ? ... : ...)[i]` 63 (0x188130 12); result declared before first
// 65/64; with the index read once through its parameter's address 65/63 (same code).
// Round 93 (lane 8): without the inner guard, the entry as `&(first ? first_entries : second_entries)[i]`
// 63 at 172 bytes: the body is retail's instruction for instruction but for the register permutation,
// and LTCG then passes animation in EAX and type in ECX (ret 4; matched 0x188130 12). Without the guard
// and the per-arm entry pointer 78 (0x188130 17); that with `!result &&` in the while test instead of
// the break 78 (17); with the count read inside the outer loop 84 (0x188130 1); with `type == 0` at each
// use 88 (0x188130 18). Retail's EBP for animation is what keeps it a stack argument.
// Lane 16 r48 (fast): no inner guard 65 (type in EAX, result in EAX, count in EBP; 0x188130 1, 0x1882d0
// 23); no guard with `entry = &(first ? ... : ...)[i]` 63 (animation in EAX, type in ECX, two-local
// frame; 0x188130 12), with count declared before result 71 (0x188130 matches), result declared first
// 64 (0x188130 12), count computed from `type == 0` before first 80 (0x188130 1).
// Round 95 (lane 8), on round 118's 52 form (no inner guard, per-arm entry, animation read once
// through its address; reproduced 52, 0x188130 matches; its getter adds into ECX and copies to EAX
// where retail forms the element with `lea ebx, [ecx + esi]; test ebx, ebx`): element_index also
// read through its address as in 0x1882d0 52 (same code); result declared before first 69
// (0x188130 1); the array-select entry `&(first ? ... : ...)[i]` 62 (0x188130 3); `if (result)
// break;` before the parent step 66 (185 bytes); count declared before result 61.
// Round 38 (m3 lane 10, fast): the entries pointer selected once per outer pass, before the guarded inner
// loop 65 (0x188130 3), without the guard 67 (0x188130 3); `return &entries[i]` from the inner loop with a
// final `return NULL` 89 (145 bytes, 0x188130 17); animation as a const reference parameter 55 (same code);
// the inner guard written `!result && i < count` 66 (0x188130 12).
// Round 55 (lane 12): the compare through the class's operator== 55; the parent field typed as the
// class 55; no inner guard with the per-arm entry and `first = !type` 78 (0x188130 17); the array-select
// entry with count spelled from `type == 0` ahead of first 80 (0x188130 1); the getter as one ternary 65
// (0x188640 22, 0x188690 18). One-file harness (same code as the build): the parameter in either other
// order, getter early returns/nested test/forceinline/long index, the entry field as the class,
// `index(element_index)` or a default-constructed index assigned after, and all 120 declaration orders
// of the address-read form change nothing (the address-read form stays at its EBX/EBP/EAX rotation).
// Round 135 (lane 4, fast with 0x188130): the existing 0x188690 called through an `inline` declaration 79
// (186 bytes); 0x188690 taking the material class by value, called here with its other callers updated, 77
// (159 bytes; 0xd9110 10 -> 171); round 118's 52 form rebuilt 52, and on it the entries pointer per iteration
// with `&entries[i]` 65 (0x188130 1), result declared after count 61, element declared first 52 (same code),
// `i` declared at function scope 52 (same code).
// Round 65 (lane 12, fast with 0x188130/0x188640/0x188690), all on round 118's 52 form (rebuilt: 52,
// animation/count/element in EBX/EBP/EAX for retail's EBP/EAX/EBX): the count selected by if/else 52;
// the entry returned from the inner loop with a final `return NULL` 91 (143 bytes); `!result &&` in the
// outer loop's condition instead of the break 52; the entry pointer declared at function scope 52; the
// element_index parameter as the walking index 52. Retail's own 0x188690 forms the element with
// `add ecx, eax; mov eax, ecx`, so the lea/test in this body follows element's register, not the getter.
// Round 52 (lane 1, fast with 0x188130): the two count/entries pairs typed as a tag-block struct (count,
// entries) with the block selected per iteration and no inner guard 57 (175 bytes: retail's result spill and
// EDX reload, but animation is then register-passed in EAX, type in ECX, ret 4; 0x188130 12); that struct
// with retail's value selects and no guard 63 (172 bytes, registers permuted: animation EAX, count EBP,
// element ECX); the struct on the guarded form 55 (same code); one block reference selected once with its
// count in the loop test 98 (135 bytes); the value selects with `if (count > 0)` around the inner loop 57.
// @retail 0x187fe0
s_seat_animation_entry *function_187fe0(long type, s_seat_animation const *animation, c_type_47f957 element_index)
{
	bool first = type == 0;
	s_seat_animation_entry *result = NULL;
	long count = first ? animation->first_count : animation->second_count;
	c_type_47f957 index = element_index;
	s_globals_element *element;

	while ((element = globals_element_get((s_globals_element_block_view *)g_4e034c, index.m_index)) != NULL)
	{
		for (long i = 0; i < count && !result; i++)
		{
			s_seat_animation_entry *entries = first ? animation->first_entries : animation->second_entries;
			if (entries[i].element_index == index.m_index)
			{
				result = &entries[i];
				break;
			}
		}
		index = c_type_47f957(element->parent_index);
		if (result)
			break;
	}
	return result;
}

// @retail 0x188090
long function_188090(long tag_index, long index, long type)
{
	long result = NONE;

	if (index != NONE)
	{
		s_seat_animations *animations = (s_seat_animations *)g_4e3b44[tag_index & 0xffff].bytes;
		do
		{
			if (index < animations->count)
			{
				s_seat_animation *animation = &animations->animations[index];
				if (type == 0)
				{
					if (animation->first_count > 0)
					{
						goto local_0;
					}
				}
				else
				{
					if (animation->second_count > 0)
					{
						goto local_0;
					}
				}
				goto local_1;
local_0:
				result = index;
				break;
local_1:;
			}

			long next = NONE;
			switch (index)
			{
			case 16:
				next = 15;
				break;
			case 17:
				next = 15;
				break;
			case 19:
				next = 16;
				break;
			case 20:
				next = 19;
				break;
			case 13:
				next = 14;
				break;
			}
			index = next;
		} while (index != NONE);
	}
	return result;
}

static inline s_seat_animation_entry *seat_animation_entry_find(long tag_index, long index, long type, short element_index)
{
	long animation_index = function_188090(tag_index, index, type);
	s_seat_animation_entry *result = NULL;

	if (animation_index != NONE)
	{
		s_seat_animations *animations = (s_seat_animations *)g_4e3b44[tag_index & 0xffff].bytes;
		result = function_187fe0(type, &animations->animations[animation_index], c_type_47f957(element_index));
	}
	return result;
}

// @retail 0x188130
s_seat_animation_entry *function_188130(long tag_index, long index, long type, short element_index)
{
	return seat_animation_entry_find(tag_index, index, type, element_index);
}

// score 22 (was 22): the element index is loaded as a dword (retail a word), EBP holds the hoisted table instead of
// the requested entry index, and the element address is `add ecx, esi; mov ebx, ecx` (retail `lea ebx, [ecx+edx];
// test ebx, ebx`). BLOCKED-BY 0x1883e0: retail's convention needs the one-short index class on 0x1883e0's
// parameters too, whose declarations sit in held/contributor files.
// Native walk index 21, direct input 33;
// native parent assignment 21; native requested index 22; both indices 21, direct input 33; native hit return 21; result first 21.
// Native parent/request/both formals 22/14/14; register parameter second/third/last 22/22/22;
// native parent with register last 22; native indices with register second 14. All restored: unmatched index wrappers rejected.
// initial word load, requested-index/table register ownership and lookup schedule still differ.
// This round: success before parent 28; native carry with alias 30; initial negative guard 73; signed loop admission 79; walking formal 38; direct native carry 35; complete per-tag result 56; success inside tag arm 28.
// initial short load and global-table/entry-index register lifetimes differ.
// Earlier requested-index snapshot 22; parent snapshot 28; explicit lookup 22; found return 22;
// word carry 22; result first 22; one-element array 21; for parent walk 71; parameter walk 22;
// entry local, null break, shared return and memcpy copy 22. Direct short carry 33; native carry 31;
// result loop guard 22; tag workspace lifetime 22; ancestor local names 22; complete result 30;
// native carry with stack input 23; existing entry lookup 105.
// initial word load, EBP entry/table lifetime and lookup registers remain;
// native entry formal 22; native parent formal 22; both native formals 22;
// direct tag field 22; current-element traversal 141; separate inline declaration 22;
// separate noinline declaration 22; existing element query 59.
// Long requested index 22; both indices as a block-index class 11 (word load,
// requested index in EBP, the table reloaded per tag; only `lea ebx, [ecx+edx]; test ebx, ebx`
// left); with 0x1883e0's two material parameters of the same class too, 0x1883e0 matches and
// 0x188180 drops 89 -> 70, but 0x1883e0's callers in held/contributor files declare short/long;
// requested index only as the class 11; without the element reference 23; declaration orders,
// names, loop guard 11; early-return or ternary lookup helper 12 (0x188690/0x188130 lose).
// Round 15: walking index as the material class built through the parameter's address 21 (word load only);
// the class walk local built from the parameter 33; result declared first 22; tag field read at each use 22;
// parent step before the lookup 35; the entry index as a class local at the top 22.
// Round 108: both material parameters as the material class, read directly (no parameter reference), 14;
// 0x1883e0 stays 118 (its own parameters keep short/long). Not kept (unmatched index wrappers).
// Round 12 (lane 18): 1-7 unused initialised locals at the top 22 each (register choice, not operand numbering);
// the element getter as a ternary 23 (0x188640 22, 0x188690 18), with `elements + index` 22, with an else arm 22
// (0x188640/0x188690 lose), with the bounds test nested 22; the loop testing the assignment directly 22.
// @retail 0x1882d0
s_seat_animation_entry *function_1882d0(long type, short element_index, long index, short entry_element_index)
{
	short const *element_reference = &element_index;
	short local_0 = *element_reference;
	s_seat_animation_entry *result = NULL;
	s_globals_element *element;

	while ((element = globals_element_get((s_globals_element_block_view *)g_4e034c, local_0)) != NULL)
	{
		long tag_index = element->tag_index;
		if (tag_index != NONE)
		{
			result = seat_animation_entry_find(tag_index, index, type, entry_element_index);
		}
		local_0 = element->parent_index;
		if (result)
		{
			break;
		}
	}
	return result;
}

// @retail 0x188370
real function_188370(long type)
{
	real result = 10.0f;

	switch (type)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		result = 10.0f;
		break;
	case 11:
		result = 20.0f;
		break;
	case 12:
		result = 40.0f;
		break;
	case 13:
		result = 15.0f;
		break;
	case 14:
		result = 15.0f;
		break;
	case 15:
		result = 50.0f;
		break;
	case 16:
		result = 20.0f;
		break;
	case 17:
		result = 20.0f;
		break;
	case 18:
		result = 20.0f;
		break;
	case 19:
		result = 20.0f;
		break;
	case 20:
		result = 20.0f;
		break;
	}
	return result;
}

bool function_172750(long mode, point3f const *point, real radius);

/* element_index starts the parent walk of 0x1882d0's fallback lookup, and
   entry_element_index (the fifth argument, which retail reads into esi) is
   the element whose values and seat entries are looked up */
// score 118 (was 118): frame 0x1c agrees; tag/remapped-index ownership and the dead-argument output home remain
// (ret 0x28 against 0x30: the entry index arrives in eax, the tag index on the stack; retail passes only the
// tag index, in ebx).
// Earlier: frame 0x1c agrees; tag/remapped-index registers and output homes remain.
// Earlier forms and observations:
// Defaults before element 175; index before element 165; both before element 151;
// read-only element 118; narrowed element index 118; shared special-index test 167;
// explicit output defaults 118; read-only element and entries 118.
// tag and remapped-index register/stack roles and result homes still differ;
// index at use 160, branch index 118, update index 118, guarded index 124, entry scope 172,
// named outputs 118, entry fields first 176, conditional second lookup 156.
// This round:
// First/second/both/all default chains 119/119/120/120;
// first/second/both tag-test results 150/162/162; both results and default chain 160.
// Round 187: field-pair default orders 021/102/120/201/210 gave 119/117/119/119/120;
// first-side/second-side defaults 118/119; immutable remapped index 118. All restored.
// Round 147: existing index getter 210; distance-reject exit 118; variant-first guards 188; equality
// membership 144; reverse cases 118; nested guards 118; selected first-table index 188; second-case
// branches 151.
// Round 186 initial/fallback/both conditional lookup results 162/118/162; guarded payload
// selection alone/with initial/with fallback/with both 219/230/219/230; shared getter force-inline 118.
// Round 135: the entry index read once through its address 128 (no register parameter left,
// ret 0x34), with the element index too 128, passed from its address 128; the entry index
// read once into a short local 118; first index after the output defaults 165. With the
// existing material class for the 4th/5th parameters (and 0x1882d0's) it matches (fast;
// tag index in EBX, first index on the stack): BLOCKED-BY the declarations in src/impacts.cpp
// and src/projectiles.cpp, which name the old parameter types.
// Round 21: in the all-stack first pass the entry index is loaded once at entry into a free register (so
// LTCG makes it the register argument) in every source order tried: element lookup moved to its use 191;
// both entry pointers declared at the top 170; second entry declared first 118; first index as an if
// statement 118; first index computed inside the distance arm 124.
// Round 420: first payload defaults 193; second payload defaults 151; both payload defaults 176; variant
// defaults 184; first payload and variants 175; second payload and variants 175; all defaults 193; all
// defaults and type outcomes 172. Original body restored.
// Round 49: still BLOCKED-BY the declarations in src/impacts.cpp (held change) and src/projectiles.cpp
// (another lane's this round), which name the old parameter types the round-135 match changes.
// Round 104: second value index as `index == 13 ? 4 : 3` 161; NULL entries set in an else of the tag test 162;
// element looked up after the first index 165; `index == 13 || index == 14` 121; unbraced fallback lookups 118.
// The material-class parameters stay BLOCKED-BY the declarations in src/impacts.cpp and src/projectiles.cpp.
// Round 108: 0x1882d0's two material parameters as the material class (no parameter reference) 118 here.
// Round 474: Complete first/second/both conditional variant results 165/168/170;
// whole first/second/both entry values 178/167/205; second/both payload pairs
// before variants 131/159. Original body restored; tag/index and output homes remain.
// Round 52 (lane 1, fast): the entry index wrapped once in a material-class local 118 (same code);
// element_index read once through its address for the fallback lookups 118 (same code); the element
// through the existing 0x188690 210 (472 bytes); each tag lookup as `tag_index != NONE ? ... : NULL` 162;
// the second tag lookup first 172. Re-measured: the 4th/5th parameters as the material class (0x188180's
// call adapted) still match here (fast), 0x188180 56; still BLOCKED-BY the declarations in
// src/impacts.cpp and src/projectiles.cpp.
// @retail 0x1883e0
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, short element_index, long entry_element_index, long index, long variant,
	long *first_value04, long *second_value04, long *first_value, long *second_value, long *first_value0c, long *second_value0c)
{
	s_globals_element *element = globals_element_get((s_globals_element_block_view *)g_4e034c, (short)entry_element_index);
	long first_index = index == 13 ? 14 : index;
	long first04 = NONE;
	long second04 = NONE;
	long first = NONE;
	long second = NONE;
	long first0c = NONE;
	long second0c = NONE;

	if (ignore_distance || function_172750(0, point, function_188370(index)))
	{
		s_seat_animation_entry *first_entry = NULL;
		s_seat_animation_entry *second_entry = NULL;

		if (tag_index != NONE)
		{
			first_entry = function_188130(tag_index, first_index, 0, (short)entry_element_index);
			second_entry = function_188130(tag_index, index, 1, (short)entry_element_index);
		}
		if (!first_entry)
		{
			first_entry = function_1882d0(0, element_index, first_index, (short)entry_element_index);
		}
		if (!second_entry)
		{
			second_entry = function_1882d0(1, element_index, index, (short)entry_element_index);
		}

		if (first_entry)
		{
			if (first_entry->type == 1 && variant != NONE)
			{
				if (index >= 13 && index <= 14)
				{
					first = element->first_values[3].value;
				}
				else
				{
					first = element->first_values[variant].value;
				}
			}
			first04 = first_entry->value04;
			first0c = first_entry->value0c;
		}
		if (second_entry)
		{
			if (second_entry->type == 1 && variant != NONE)
			{
				if (index >= 13 && index <= 14)
				{
					switch (index)
					{
					case 13:
						second = element->second_values[4].value;
						break;
					case 14:
						second = element->second_values[3].value;
						break;
					}
				}
				else
				{
					second = element->second_values[variant].value;
				}
			}
			second04 = second_entry->value04;
			second0c = second_entry->value0c;
		}
	}

	if (first_value04)
	{
		*first_value04 = first04;
	}
	if (second_value04)
	{
		*second_value04 = second04;
	}
	if (first_value)
	{
		*first_value = first;
	}
	if (second_value)
	{
		*second_value = second;
	}
	if (first_value0c)
	{
		*first_value0c = first0c;
	}
	if (second_value0c)
	{
		*second_value0c = second0c;
	}
}

/* a block index of NONE, read where a function takes a block index */
short g_47d8e0 = NONE;

dword vector3d_compress(vector3f const *vector);
#include "unknown_1765e0.h"
long function_189400(s_sound_position const *position, long object_index, long tag_index, real scale);

// score 77 (was 77): retail's own clone takes the point in EAX and the forward vector in ECX and
// pops eight stack arguments, ours takes the forward in EDX and pops seven; that split comes
// from the call sites, which are in other files (unit_object_type.cpp, unknown_0a76b0.cpp,
// items.cpp, unknown_246f10.cpp). Retail also calls 0x1883e0 with twelve stack arguments where
// ours passes thirteen: BLOCKED-BY 0x1883e0 (another file).
// Earlier: frame 0x48 agrees; first counter spill recovered; effect/forward inputs and second scan differ
// (BLOCKED-BY 0x1883e0, see below). Earlier forms and observations:
// Point setter 89; component loop 317; origin value 150; description position 105;
// late description 89; result-array names 89; query-owned defaults 122; facing value 149.
// frame and array homes agree; input registers and the first scan counter differ;
// paired initialization 326, effect cursor 91, sound cursor 100, both cursors 103,
// paired/effect cursor 328, paired/sound cursor 335, paired/both cursors 344, shared scan index 89.
// Earlier counter-width forms:
// Narrow first short/word/byte 120/120/120; narrow second short/word/byte 77/77/77;
// both short/word 126/126; signed-short second scan 77 plain, 337/336 bytes.
// First/second/both array library fills 99/102/101;
// local defaults/library defaults before point 77/101;
// compression before point copy 75; location before velocity 76;
// both sound copy orders 74 (three-point gain, rejected).
// Exact-end sound scans dword/long/short 89/89/77; exact-end effect scan 84;
// shared complete offset scale 125; sound point setter 106;
// native velocity/location field copies 89/96.
// Round 147: two-phase offset 125; origin accumulation 142; all named products 77; origin-first adds 77;
// effect/sound/both continues 77/77/77; signed short bound 77.
// direction alias 89; point memcpy 89; velocity memcpy 89; location memcpy 89; three copies 89;
// signed effect index 89; signed sound index 89; signed indices 89; own-parameter input 87;
// inline effect callee 89; inline sound callee 89; inline dispatch callees 89.
// BLOCKED-BY 0x1883e0: with its 4th/5th parameters as the one-short index class this takes
// retail's eax/ecx inputs and only the second scan differs (short/word 56, dword/long 70).
// Callee entry index through its address 77; volatile effect read 90 (retail inputs);
// aggregate effect point 77; typed effect-direction local 77; point and description first 77.
// Round 94 (lane 6): with 0x1883e0's parameters as c_type_47f957 (held to 2026-11-02) and the
// second scan on a 32-bit index, 77 -> 46.
// Round 36: query inline 77; query and target inline 77; target inline 77; array-decay outputs 77;
// function-scope counters 77; aggregate sound position 77; compression last 76 (rejected); vector assign 77.
// Round 116: named effect index 77; named sound index 77; both 77; dword second scan with the named
// sound index 89; description filled before the effect scan 98.
// Round 118: effect point through the existing vector3f_add_scaled 77; first array filled first 77;
// the fills paired by index 76; velocity copied first 77; effect point after the query 96.
// Round 52 (lane 1, fast): the sound position as an aggregate initializer 77 (same code); the scan index
// types exchanged (short first, dword second) 120 (364 bytes); the six NONE defaults as one chained
// assignment 77; one chained assignment per array 77; the locals declared position, effect point, second,
// first 77; the fills paired by index re-measured 76. With 0x1883e0's 4th/5th parameters as the material
// class (diagnostic) 56 (346 bytes): this takes retail's inputs, so the input split follows 0x1883e0's
// convention; left are the second scan (retail: dword index in EDI, scale in EBP) and the `or esi, -1` slot.
// @retail 0x188180
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	long entry_element_index, long effect_value, s_location const *location, real scale)
{
	long first_values[3];
	long second_values[3];
	point3f effect_point;
	s_sound_position position;

	effect_point.x = forward->i * 0.01f + point->x;
	effect_point.y = forward->j * 0.01f + point->y;
	effect_point.z = forward->k * 0.01f + point->z;
	second_values[0] = NONE;
	second_values[1] = NONE;
	second_values[2] = NONE;
	first_values[0] = NONE;
	first_values[1] = NONE;
	first_values[2] = NONE;
	function_1883e0(tag_index, false, point, g_47d8e0, entry_element_index, index, variant,
		&first_values[0], &second_values[0], &first_values[1], &second_values[1], &first_values[2], &second_values[2]);

	for (dword i = 0; i < sizeof(second_values) / sizeof(second_values[0]); i++)
	{
		if (second_values[i] != NONE)
		{
			function_1765e0(&effect_point, (vector3f const *)effect_value, forward, second_values[i], 0, 0);
		}
	}

	position.position = effect_point;
	position.compressed_forward = vector3d_compress(forward);
	position.velocity = *g_4687a4;
	position.location = *location;
	for (short j = 0; j < sizeof(first_values) / sizeof(first_values[0]); j++)
	{
		if (first_values[j] != NONE)
		{
			function_189400(&position, object_index, first_values[j], scale);
		}
	}
}

struct s_unit_material_object
{
	long definition_index;
	byte unknown04[0x10a - 4];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word unknown10a : 13;
};

struct s_unit_material_object_header
{
	byte unknown00[8];
	s_unit_material_object *object;
};

struct s_unit_material_definition
{
	byte unknown00[0x38];
	long model_index;
	byte unknown3c[0x280 - 0x3c];
	short material_type;
	short alternate_material_type;
};

struct s_unit_material_model
{
	byte unknown00[0x60];
	long count;
	struct
	{
		byte unknown00[0xce];
		short material_type;
	} *entries;
};

// @retail 0x1886d0
short *function_1886d0(long object_index, short *material_type)
{
	struct s_1886d4 { signed short field_0 : 16; };
	s_1886d4 local_1;
	local_1.field_0 = g_47d8e0;
	s_unit_material_object *object = ((s_unit_material_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_unit_material_definition *definition = (s_unit_material_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;

	if (function_badc0(object_index, 1))
	{
		if (TEST_FIELD_BIT(object->flag2))
		{
			local_1.field_0 = definition->alternate_material_type;
		}
		else
		{
			local_1.field_0 = definition->material_type;
		}
		if (local_1.field_0 != g_47d8e0)
		{
			goto local_0;
		}
	}
	if (definition->model_index != NONE)
	{
		s_unit_material_model *model = (s_unit_material_model *)g_4e3b44[definition->model_index & 0xffff].bytes;
		if (model->count)
		{
			local_1.field_0 = model->entries->material_type;
		}
	}
local_0:
	*material_type = local_1.field_0;
	return material_type;
}