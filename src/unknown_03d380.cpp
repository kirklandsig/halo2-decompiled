// @flags /O2 /Gr
#include "handles_storage.h"
/* UNKNOWN_03D380.CPP: the game module callbacks of the table at 0x46e320
   (batch 18-4); every entry takes one dword on the stack */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_03d380.h"
#include "object_iterator.h"

/* ---- types ---- */

struct s_object
{
	byte unknown00[0xcc];
	long unknownCC;
};

struct s_simulation_world
{
	byte unknown00[8];
	long state;
	byte unknown0c[0x22];
	byte flag2e;
};

struct s_4e6380
{
	byte unknown00[0x78];
	byte flag78;
	byte unknown79[2];
	byte flag7b;
	byte unknown7c;
	byte flag7d;
	byte unknown7e[2];
	long unknown80;
	long unknown84;
	byte unknown88[0x1f8 - 0x88];
	long unknown1f8;
	byte unknown1fc[4];
	long unknown200;
};

struct s_element_bc
{
	byte unknown00[2];
	byte kind;
	byte unknown03[9];
	long object_index;
	byte unknown10[0x7c];
	long unknown8c;
	byte unknown90[0x2c];
};

struct s_element_18_flags
{
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 8;
};

struct s_element_18
{
	byte unknown00[3];
	byte state;
	s_element_18_flags flags;
	byte unknown06[6];
	long value;
	byte unknown10[8];
};

struct s_element_40
{
	byte unknown00[4];
	long table_index;
	byte unknown08[0x30];
	long datum_index;
	byte unknown3c[4];
};

struct s_tag_iterator
{
	long unknown00;
	long unknown04;
	long unknown08;
	long index;
	dword signature;
};


struct s_unknown_5c;
s_unknown_5c *function_221810(short index);

/* ---- globals ---- */

byte g_4cf771;
byte g_4cf772;
byte g_4cf77b;
dword g_453498[1];
s_4e6380 *g_4e6380;
s_record_pool *g_4e637c;
s_looping_sound_globals *g_4ed288;

void decal_delete_group(long decal_index);

// @retail 0x43820
void __stdcall function_43820(long index)
{
	dword *entry = g_4c8798 + (index & 0xffff);
	if (*entry != NONE)
	{
		decal_delete_group(*entry);
		*entry = NONE;
	}
}

// @retail 0x43850
long __stdcall function_43850(long index)
{
	long handle = g_4c8798[index & 0xffff];
	byte *entry = (byte *)g_4ea950->data + (handle & 0xffff) * 0x40;
	word flags = entry[2];
	return (flags & 1) || (flags & 2);
}
byte g_4ea934;
byte g_4ea936;
byte g_4e6388;
s_record_pool *g_51ebfc;
s_record_pool *g_51ec00;
long g_47f04c;
byte g_47f059;
s_47f048_object *g_47f048;
void *g_51ecac;

#define GAME_MODE (g_4e6948->mode)
#define ELEMENT(array, type, datum) ((type *)((array)->data + sizeof(type) * ((datum) & 0xffff)))

/* ---- the table ---- */

void __stdcall function_1c3540(dword flags);
void __stdcall function_68090(dword flags);
void __stdcall function_16eff0(dword flags);
void __stdcall function_1e6af0(dword flags);
void __stdcall function_1c3590(dword flags);
void __stdcall function_123b00(dword flags);
void __stdcall function_1889d0(dword flags);
void __stdcall function_155f10(dword flags);
void __stdcall function_67fb0(dword flags);
void __stdcall function_124750(dword flags);
void __stdcall function_1264c0(dword flags);
void __stdcall function_188ac0(dword flags);
void __stdcall function_16f090(dword flags);
void __stdcall function_14b660(dword flags);
void __stdcall function_67fc0(dword flags);
void __stdcall function_43970(dword flags);
void __stdcall function_3d380(dword flags);
void __stdcall function_124620(dword flags);
void __stdcall function_155360(dword flags);
void __stdcall function_1c7f80(dword flags);
void __stdcall function_17d190(dword flags);
void __stdcall function_16f0c0(dword flags);
void __stdcall function_226030(dword flags);
void __stdcall function_24c829(dword flags);

game_module_proc g_46e320[30] =
{
	function_1c3540,
	function_68090,
	function_16eff0,
	function_1e6af0,
	function_1c3590,
	function_1c3540,
	function_123b00,
	function_1889d0,
	function_155f10,
	function_67fb0,
	function_124750,
	function_72c70,
	function_1264c0,
	function_188ac0,
	function_16f090,
	function_14b660,
	function_67fc0,
	function_43970,
	function_3d380,
	function_72c70,
	function_72c70,
	function_72c70,
	function_124620,
	function_155360,
	function_24c829,
	function_1c3590,
	function_1c7f80,
	function_17d190,
	function_16f0c0,
	function_226030,
};

// @retail 0x3d380
void __stdcall function_3d380(dword flags)
{
	struct { s_object *object; s_type_f1af8e iterator; } iteration;

	function_bae80(&iteration.iterator, 0, 0);
	for (iteration.object = function_baeb0(&iteration.iterator); iteration.object; iteration.object = function_baeb0(&iteration.iterator))
		iteration.object->unknownCC = NONE;
}

// @retail 0x43970
void __stdcall function_43970(dword flags)
{
	g_509448->proc20 = function_43820;
	g_509448->proc24 = function_43850;
}

// @retail 0x67fb0
void __stdcall function_67fb0(dword flags)
{
	g_4cf77b = 1;
}

struct s_67fc1
{
	byte field_0[0xc];
	signed long field_c : 8;
};

// @retail 0x67fc0
void __stdcall function_67fc0(dword flags)
{
	if (flags & 4)
	{
		function_593e0();
		s_simulation_world *world;
		if (GAME_MODE == 1 && (world = g_4cf77c)->state == 1)
		{
			function_6b040((c_class_6a600 *)world);
			g_4cf772 = 0;
			g_4cf771 = 0;
		}
		else
		{
			function_67f60();
			if (GAME_MODE == 4)
			{
				GAME_MODE = 5;
				void *proc = g_55e4d0[g_4e9ae8->engine_index];
				if (proc)
					function_162060(proc);
			}
			s_67fc1 *local_0 = (s_67fc1 *)g_4e6948;
			long local_1 = local_0->field_c;
			if (local_1 >= 4 && local_1 <= 5)
			{
				function_162420();
				function_bb7f0();
				function_183f10();
			}
			GAME_MODE = 1;
			function_67ee0();
		}
	}
	((c_class_6a600 *)g_4cf77c)->function_6a600();
	function_83370(g_4cf780, flags);
	function_6a770(g_4cf77c);
	g_4cf77b = 0;
}

// @retail 0x68090
void __stdcall function_68090(dword flags)
{
	if (g_4cf770 && !(flags & 1))
	{
		long state = g_4cf77c->state;
		if (state != 3 && state != 5)
			g_4cf77c->flag2e = 1;
	}
}

static __forceinline void function_123b01(byte *arg_0)
{
	XPhysicalProtect(arg_0, 0x3be000, PAGE_READWRITE);
	XPhysicalProtect(arg_0 + 0x3be000, 0x40000, PAGE_READWRITE | PAGE_WRITECOMBINE);
}

// @retail 0x123b00
void __stdcall function_123b00(dword flags)
{
	function_123b01(game_state_globals.base_address);
}

// @retail 0x124620
void __stdcall function_124620(dword flags)
{
	game_state_globals.game_time = g_510c54->game_time;
	g_510c54->unknown01 = 0;
}

// @retail 0x124750
void __stdcall function_124750(dword flags)
{
	if (g_510c2c)
		*(dword *)g_510c2c = (dword)g_453498;
}

void function_125a90(long value);

// MiniMax lane E (11-01): no new form, no lower score; the named-condition form that gives retail's
// block order still needs 0x221810 kept out of line.
// score 36 (was 36): the address of the record is built here with an imul
// into ESI and an add; retail multiplies into EAX and forms the address with a
// lea, so the type byte that is tested afterwards is compared in another value.
// Retail also falls from the type test into the special-type arm, then the
// removal, then the kind test; ours puts the kind test first. Tried: the
// special arm as `type != 0x26 || a` 88; as `type == 0x26 && !a` 88; the kind
// tests joined with && 46; the type test negated, kind arm first 46; removal
// first with an if/else special arm 50.
// Round 411: Existing calling iterator with returned/salted receivers and remove/keep
// decisions 115/110/168/177; existing fully inlined iterator with the same four forms
// 115/110/168/177. Original traversal restored; the early policy load, ESI multiply
// and special/remove/ordinary/keep ordering remain.
// Lane 14 r9: the join as `if (remove) {removal} else keep` 51 (kind arm, keep, then the special arm and
// removal after the loop step); every action written in its own arm (no flag) 110; the flag declared
// before the element 47 (same code); the kind tests joined with && 47, plus the flipped join 51; the
// special arm as `remove = type == 0x26 ? !a : false` 47.
// Round 95 (one-file harness with 0x221810 pinned out of line; aligned 23 = 47 here): ours ends
// the kind arm with a jump into the special arm's `je` (tail merge), so the kind arm is the
// fall-through; that order survives the special arm cut down to `remove = false`, the arms
// swapped, removal first and every flag form tried. Real: kind arm `remove = f() != 1` with
// removal first 88 (417 bytes); arms swapped with removal first 51; kind arm as one `||`
// value with removal first 46; data pointer local 47; special arm as if/else with removal
// first 51. Harness only: the file's ELEMENT macro 37 (worse registers); element as
// `&((T *)data)[i]`, `data + i * sizeof`, `sizeof * i`, `i + (T *)data`, an elements
// pointer: the ESI multiply in each; `remove = type == 0x26 && !a` threads the sete away.
// Round 100 (real builds): the file's ELEMENT macro 62 (same early flag load, EBP/EDI swap);
// `remove` declared first 47; removal written first 51: both arm bodies then end as retail's
// (special arm `je keep` into the removal, kind arm `jne remove` into keep), but the kind arm
// stays the type test's fall-through and the special arm with the removal moves after the
// next-index code; with the type test negated and the kind arm first 51 (identical); with the
// kind tests joined by && 51.
// Round 11: `if (0x20) remove = false; else if (0x26) remove = !a; else if (kind)` with removal
// first 87 (retail's block order, but no re-test of 0x26 for 0x20); the condition named `bool
// special = 0x20 || 0x26` with removal first gives retail's whole block order in the compile-only
// harness, but LTCG then inlines 0x221810 (144 at 467 bytes; with the kind test as one `&&` 144,
// keep first 157 at 471 bytes); removal first alone 51.
// Round 13 (lane 13): the element written out at each use (`((T *)array->data)[index].field`)
// gives retail's `imul eax` + `lea esi` but sinks it below the b test 43; with `if (remove)`
// first as well 47; `if (remove)` first alone 51; ELEMENT(array, T, datum) 62, with index 94;
// `&((T *)data)[index]` 47; `(T *)(data + index * sizeof(T))` 47.
// Round 442: Live type observations with bool/byte/short/long removal 91/91/85/90; removal-
// first completion 91/91/85/90. Original type capture, comparison widths and complete body
// restored.
// Later (36, from 47): the special arm now assigns the removal flag itself
// (`remove = (type == 0x26) && !a`) and the kind arm assigns `remove = (byte != 1)`,
// so both arms branch to their own blocks instead of merging into the special arm's test;
// the removal arm is written before the keep arm, which puts retail's blocks in retail's order.
// Still differs: the b flag is loaded before the element address (retail loads it after) and
// the type lookup uses EAX where retail uses EDX.
// Round 109 (lane 5, fast): the special arm as `if (type == 0x26) remove = !a; else remove = false;`
// on the long type with the kind arm as `if (byte == 1) remove = false;` 51: both arm bodies are
// then retail's code (`cmp eax, 0x26`, the `sete` of !a, the fused kind arm), but the kind chain is
// still laid out first; that with keep written first 47 (tail-merged); the condition named `bool
// special = 0x20 || 0x26` with those arms 144 (retail's block order, but LTCG then inlines 0x221810,
// 467 bytes), with `if (!special)` and the kind arm first 158 (471 bytes, inlined), with the call's
// argument as `(short)type` 144. BLOCKED-BY 0x221810: the named-condition layout needs 0x221810 kept
// out of line (its file is outside this round).
// Round 44 (lane 16, fast counts; control 36): retail lays out the special arm first, falling
// into the removal with its `!a` materialized (`sete al; test al, al; je keep`), then the kind
// arm fused (`jne remove`) falling into keep; ours lays out the kind arm first (materialized
// `setne al; test al, al`) and fuses the special arm. Tried: the inner test on the long type
// (`type == 0x26 && !a`, retail's `cmp eax, 0x26`) 46, still kind arm first; that as a ternary
// `type == 0x26 ? !a : false` 88; the type test negated with the kind arm first and the special
// arm in the else 46 (same code as the long test); the kind arm as `remove = false` under one
// `kind && byte == 1` test 88; a two-case switch on the short type 86 (compares in ECX).
// Round 108 (lane 6): the type byte is the sound definition's class (the existing
// sound_definition_get(...)->promotion_index): read as a short 87 (`movsx ax`, retail widens to
// 32 bits), as a long through the getter 36 (same code as the direct read). Retail re-tests
// `cmp eax, 0x26` after `cmp ax, 0x26`, so its inner test is on the full-width value and is not
// threaded away (matched 0x18d360 tests the same pair on a short); that inner long test alone
// 46 gives retail's control flow but keeps the kind arm as the test's fall-through. `remove`
// assigned on every path with no initializer 36 and the negated test with the kind arm first 36
// both compile to the same code (VC normalises the arm order).
// Round 55 (fast): element computed before the index local 51; index as `(word)datum` 114; the special arm
// as `if (type == 0x26) remove = !a; else remove = false;` on the long type 88 (417 bytes); `remove` declared
// before the index and element 36; the element through a typed elements array local 36.
// Round 132 (lane 6, fast): the condition named as a bool 157 (471 bytes; LTCG still inlines
// 0x221810); the whole decision as one ternary 88 (both arms threaded, kind arm first); the file's
// ELEMENT macro with `(datum & 0xffff) + 1` and no index local 51; the special arm as `if (type ==
// 0x26) remove = !a; else remove = false;` on the long type 88 (aligned 31 against 35: the special arm
// is then materialised and falls into the join as in retail, but the kind arm is laid out first and
// jumps into the join with `setne`); that with the kind arm as one `kind && x == 1` test setting
// `remove = false` 51: both arm bodies are then retail's, but the kind arm stays the class test's
// fall-through and VC moves the special arm and the removal into the gap after the next-index
// ternary's `jmp`, where retail puts the kind arm in the gap after the removal's `jmp`.
// Round 71 (lane 12, fast): the special arm as `if (type == 0x26) remove = !a; else remove = false;` with the kind
// arm as `if (byte == 1) remove = false;` 51 (retail's arm bodies; the kind arm falls into keep and the special arm
// with the removal moves after the loop step); the condition named `bool special` with those arms 144 (467 bytes:
// retail's block order, special arm with `sete`, removal, kind arm, keep, but LTCG inlines 0x221810), with
// `remove = type == 0x26 && !a` and `!= 1` 157 (471), with the kind arm as one `&&` test 144 (467); the element
// pointer formed from `datum & 0xffff` before index 51. Still BLOCKED-BY 0x221810 (inlined whenever the layout is
// retail's).
// @retail 0x1264c0
void __stdcall function_1264c0(dword flags)
{
	bool b = (flags >> 5) & 1;
	bool a = (flags >> 6) & 1;


	/* neither pause (0) nor resume (1): only clears the paused flag */
	if (!b)
		function_125a90(2);

	if (g_4e6380->flag78)
	{
		long datum;

		function_21d4d0();
		s_record_pool *array = g_4e637c;
		datum = data_datum_index(array, function_16bc00(array, 0));
		while (datum != NONE)
		{
			long index = datum & 0xffff;
			s_element_bc *element = (s_element_bc *)array->data + index;
			bool remove = true;

			if (b)
			{
				long type = *((char *)g_4e3b44[element->object_index & 0xffff].flags + 2);

				if ((short)type == 0x20 || (short)type == 0x26)
					remove = ((short)type == 0x26) && !a;
				else if ((1 << element->kind) & 0x1e)
					remove = ((byte *)function_221810(type))[0x4c] != 1;
			}

			if (remove)
			{
				function_127320(datum, 10);
				array = g_4e637c;
			}
			else
				element->unknown8c = NONE;

			datum = data_datum_index(array, function_16bc00(array, datum == NONE ? 0 : index + 1));
		}

		if (!b)
			function_21f290();
		else
			function_125d60();
	}

	dword ticks = GetTickCount();
	s_4e6380 *globals = g_4e6380;
	globals->unknown80 = ticks;
	globals->unknown1f8 = NONE;
	globals->unknown200 = NONE;
	globals->unknown84 = 0;
	g_4ed288->value20 = 0;
	g_4ed288->value24 = 0;
	if (!b)
		function_21a1e0();
}

// @retail 0x14b660
void __stdcall function_14b660(dword flags)
{
	struct
	{
		long a[2];
		long b;
		byte c[0xe40];
	} locals;

	if (flags & 8)
	{
		function_23654b(locals.c, locals.a, &locals.b);
		function_152f80(locals.a, locals.c);
	}
}

// @retail 0x155360
void __stdcall function_155360(dword flags)
{
	function_155380();
	function_155a30(*g_4e8c34);
}

// @retail 0x155f10
void __stdcall function_155f10(dword flags)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = 0;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = 0;
			g_4e6388 = 0;
		}
	}
}

// @retail 0x16eff0
void __stdcall function_16eff0(dword flags)
{
	if ((bool)((~(flags >> 4)) & 1))
	{
		long i;

		g_510c70[0] = g_4686c4;
		for (i = 0; i < 4; i++)
		{
			bool valid = false;

			g_510c70[i + 1] = NONE;
			if (i != NONE)
				valid = g_4e8c20->entries[i] != NONE;
			if (valid)
			{
				struct
				{
					byte unknown[4];
					short value;
				} result;

				function_11bed0((s_location *)&result, (point3f const *)&g_4e9bd4[i].state);
				g_510c70[i + 1] = result.value;
			}
		}
	}
}

// @retail 0x16f090
void __stdcall function_16f090(dword flags)
{
	g_4ea934 = 1;
	for (long i = 0; i < 4; i++)
		function_16f4b0(&g_4e9bd4[i]);
}

// @retail 0x16f0c0
void __stdcall function_16f0c0(dword flags)
{
	long mode = g_4e6948->mode;

	if (mode < 2 || mode > 5)
		g_4ea936 = 1;
}

/* the next used index at or after index, or NONE */
static __forceinline long record_pool_next_used_index(s_record_pool *array, long index)
{
	if (index >= 0 && index < array->high_water_index)
	{
		long count = array->high_water_index;
		dword *bits = array->bitmap;

		do
		{
			if (bits[index >> 5] & (1 << (index & 0x1f)))
				return index;
			index++;
		} while (index < count);
	}
	return NONE;
}

/* steps the iterator to the next used datum; its address, or NULL at the end */
static __forceinline byte *record_pool_iterator_next_used(s_record_pool_iterator *iterator)
{
	s_record_pool *array = iterator->data;
	long index = record_pool_next_used_index(array, iterator->index + 1);

	if (index == NONE)
		return NULL;

	byte *datum = array->data + array->size * index;

	iterator->index = index;
	iterator->datum_index = (*(short *)datum << 16) | index;
	return datum;
}

// @retail 0x17d190
void __stdcall function_17d190(dword flags)
{
	if (g_4ea950->valid)
	{
		s_record_pool_iterator iterator;

		iterator.data = g_4ea950;
		iterator.index = NONE;
		while (record_pool_iterator_next_used(&iterator))
		{
			s_element_40 *element = ELEMENT(g_4ea950, s_element_40, iterator.datum_index);

			if (element->datum_index == iterator.datum_index)
				g_4c8798[element->table_index & 0xffff] = iterator.datum_index;
		}
	}
}

// @retail 0x1889d0
void __stdcall function_1889d0(dword flags)
{
	s_record_pool *array = g_4ed28c;
	long datum = data_datum_index(array, function_16bc00(array, 0));

	while (datum != NONE)
	{
		s_element_18 *element = ELEMENT(array, s_element_18, datum);

		if (function_18d1c0(element->value) == datum)
		{
			long value = element->value;
			long other = function_18d1c0(value);
			if (other != NONE)
			{
				s_element_18 *other_element = ELEMENT(array, s_element_18, other);
				if (other_element->value == value)
					other_element->flags.flag5 = 0;
			}
		}

		datum = data_datum_index(array, data_find_index(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}
}

/* releases the deleted looping sounds and the active ones a reset ends (with flags bit 6 the
   ones whose tag has an effect of type 0x20 or 0x26 stay), clears the tags' entries at +0x4c of
   g_51ebd4 and the sound globals, and with flags bit 5 updates the listeners once */
// @retail 0x188ac0
void __stdcall function_188ac0(dword flags)
{
	bool a = (flags >> 6) & 1;
	long datum = data_datum_index(g_4ed28c, function_16bc00(g_4ed28c, 0));
	s_tag_iterator iterator;
	long tag_datum;
	s_looping_sound_globals *globals;

	while (datum != NONE)
	{
		s_element_18 *element = ELEMENT(g_4ed28c, s_element_18, datum);

		if (element->state == 2)
			record_pool_release(g_4ed28c, datum);
		else
		{
			word flags5 = *(word const *)&element->flags;
			if ((bool)((flags5 >> 5) & 1))
			{
				long value = element->value;
				if (!(function_18d360(value) && a))
				{
					if ((bool)((flags5 >> 6) & 1) || (*(byte *)g_4e3b44[value & 0xffff].flags & 2))
						record_pool_release(g_4ed28c, datum);
					else
						function_18d290(value, datum);
				}
			}
		}

		datum = data_datum_index(g_4ed28c, data_find_index(g_4ed28c, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}

	iterator.index = 0;
	iterator.signature = 0x736e6421;
	while ((tag_datum = function_122c70(&iterator)) != NONE)
	{
		byte *definition = (byte *)g_4e3b44[tag_datum & 0xffff].flags;

		if (definition[0xc] != 0xff)
		{
			long *entry = (long *)(g_51ebd4->entries + (char)definition[0xc] * 0x1c);

			if (entry)
			{
				entry[4] = NONE;
				entry[5] = 0;
				entry[6] = 0;
			}
		}
	}

	globals = g_4ed288;
	function_220fd0((s_sound_driver_volumes const *)globals->gains);
	memset(globals->slots, 0xff, 0x100);
	function_225ab0();
	if ((flags & 0x20) && !g_4ed288->flag241)
		function_18bb80(3.4028235e38f);
	g_4ed288->flag241 = 0;
}

// @retail 0x1c3540
void __stdcall function_1c3540(dword flags)
{
	if (!(flags & 1))
	{
		function_1c2b10();
		record_pool_release_all(g_51ebfc);
		record_pool_release_all(g_51ec00);
		g_47f058 = false;
	}
	if (flags & 2)
	{
		function_1c29d0();
		function_1c2890();
		g_47f059 = 0;
	}
	g_47f04c++;
}

// @retail 0x1c3590
void __stdcall function_1c3590(dword flags)
{
	g_47f04c--;
	if (flags & 2)
	{
		g_47f059 = 1;
		function_1c2910();
		function_1c39c0(g_47f048, 0);
		g_47f048->function_30be40((long)g_51ecac);
	}
	if (!(flags & 1))
	{
		g_47f058 = true;
		function_1c2a10();
	}
}

// @retail 0x1c7f80
void __stdcall function_1c7f80(dword flags)
{
	g_557c6c->proc2c = function_25dd20;
	g_557c6c->proc30 = function_25dd30;
}

// @retail 0x1e6af0
void __stdcall function_1e6af0(dword flags)
{
	function_1e75d0(0);
}

// @retail 0x226030
void __stdcall function_226030(dword flags)
{
	if (!(flags & 0x80))
		g_4701ec.stage = 0;
}


// Disabled: shared decal handles currently hold 256 entries; retail initializes 1024.
#if 0
#include "crc.h"
#include "physical_memory.h"
extern D3DResource *g_509444;

// Retail 0x43890
void function_43890()
{
    byte *state = (byte *)VirtualAlloc(0, 12, 0x101000, PAGE_READWRITE);
    if (!state) GetLastError();
    *(long *)state = 1;
    long size = 0x14000;
    game_state_globals.unknown8 += size;
    byte *buffer = (byte *)game_state_globals.base_address + 0x3fe000 - game_state_globals.unknown8;
    g_509444 = (D3DResource *)state;
    function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
    *(byte **)(state + 4) = buffer;
    *(long *)(state + 8) = 0;
    if ((*(long *)state & 0x70000) != 0x20000) buffer = (byte *)((dword)buffer & 0x0fffffff);
    *(byte **)(state + 4) = buffer;
    c_data_allocator *allocator = g_510c2c;
    void *table = allocator->allocate(0x613c);
    function_13d170((s_physical_object *)table, "decal vertex cache", 0x500, 6, 1024,
        function_43820, (physical_block_busy_proc)function_43850, 0, allocator);
    memset(g_4c8798, 0xff, 1024 * sizeof(dword));
    g_509448 = (s_game_proc_table_509448 *)table;
}
#endif


s_handles_storage g_handles_storage;

#include "crc.h"
#include "physical_memory.h"
extern D3DResource *g_509444;

// @retail 0x43890
void function_43890()
{
    byte *state = (byte *)VirtualAlloc(0, 12, 0x101000, PAGE_READWRITE);
    if (!state) GetLastError();
    *(long *)state = 1;
    g_509444 = (D3DResource *)state;
    long size = 0x14000;
    game_state_globals.unknown8 += size;
    byte *buffer = (byte *)game_state_globals.base_address + 0x3fe000 - game_state_globals.unknown8;
    function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
    *(byte **)(state + 4) = buffer;
    *(long *)(state + 8) = 0;
    if ((*(long *)state & 0x70000) != 0x20000) buffer = (byte *)((dword)buffer & 0x0fffffff);
    c_data_allocator *allocator = g_510c2c;
    *(byte **)(state + 4) = buffer;
    void *table = g_510c2c->allocate(0x613c);
    function_13d170((s_physical_object *)table, "decal vertex cache", 0x500, 6, 1024,
        function_43820, (physical_block_busy_proc)function_43850, 0, allocator);
    memset(g_4c8798, 0xff, 1024 * sizeof(dword));
    g_509448 = (s_game_proc_table_509448 *)table;
}
