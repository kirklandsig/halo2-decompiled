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
extern byte g_4ba022;
byte g_4ba018;

__declspec(noinline) void *function_449e0(short index, bool load, bool instance);
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

// score 54 (was 54): count, mode and first-index homes conflict across the inline copy. Retail
// multiplies the index into edx and clears the globals before the loop, ours
// computes the index in eax and stores the table pointer later. Without the block pointer (count and
// entries indexed at each use) the head is retail's (scaled index in edx, spilled) and so is the loop
// tail, but count lands in cx/edx (retail ax/eax, zero in ecx) and first stays in eax where retail
// spills it across the inline memcpy; 144 (count through the metadata view or the block, 492 bytes);
// entries indexed by the counter 164; separate cursor pointer 54; with the current stored first 140;
// block pointer declared inside the if 151; index head with the block pointer only for the loop 155;
// count read once into a short local 177 (480 bytes). Round 35, all on the indexed form: the cursor
// set after the selection with the copy source indexed 164 (retail's loop, saves and reload at the
// join, but both parameters on the stack); one variable for first and the loop index 159 (retail's
// head exactly: index in edx, count in ax/eax, zero in ecx, entries in edx; the index then lives in
// ebp and mode is spilled, where retail spills first and keeps mode in ebp); `first + n` with
// `entries[n]` 158; first by a chained assignment 158; the copy's destination through the stored
// current 164; the loop index set before the selection 162 (cache index in ecx); the selection as
// if/else or as the reversed ternary 164/164; selection, n/index, cursor or first declared early 164
// each; the cursor before the copy with one index variable 163; the count in a short local read before
// the clears 164. The two g_4c1a80 statements (the same field as g_4c1a48[2].flags) removed: same
// code. Round 37: in the indexed form (144, 492 bytes) the loop body, its stack slots (scaled index at
// +0x28 above geometry) and the frame are retail's; left are the head (count in cx then edx where
// retail coalesces ax/eax, zero in eax, first loaded into eax before the pushes where retail spills it
// from edi, entries spilled at once) and the cursor reload at the loop head. On that form: first by a
// chained assignment 140; current set from previous and first read back 140; entries declared before
// first 145; a for loop over n 144 (141 with the chained first). On the block form: each entry taken
// as `&block->entries[n]` 146 (494 bytes).
// Count, mode and first-index homes conflict across the inline copy.
// Metadata pointer plus entry-table expression and early index 154; shared-scale entries 159;
// direct metadata with early index 159; const first 146; direct copy with late entries 163;
// first address direct/reference 163 each; early selection 169. Original block body restored.
// Round 100: the block local renamed (records, zone) 54 (it is a spilled register variable, so the
// name does not order its slot); indexed form through g_4c6b00[cache_index].count 144; on it, no first
// local (previous read for the copy and the loop index) 157 (count ax/eax, zero ecx and first edi as
// retail, but the index arrives in EAX, the frame grows to 0x4c and previous is re-read); a named long
// count before first 140 (count in ax/eax as retail, index in ECX, zero EDX); on the block form, no
// first local 162. Retail indexes the blocks (count read once as a short, scaled index spilled); the
// block form stays the closest.
// retail multiplies the index into edx and clears the globals before the
// loop, ours computes the index in eax and stores the table pointer later. Without the block
// pointer (count and entries indexed at each use) the head is retail's (scaled index in edx,
// spilled) and so is the loop tail, but
// count lands in cx/edx (retail ax/eax, zero in ecx) and first stays in eax where retail
// spills it across the inline memcpy; 144 (count through the metadata view or the block,
// 492 bytes); entries indexed by the counter 164; separate cursor pointer 54; with the
// current stored first 140; block pointer declared inside the if 151; index head with the
// block pointer only for the loop 155; count read once into a short local 177 (480 bytes).
// Round 35, all on the indexed form: the cursor set after the selection with the copy source
// indexed 164 (retail's loop, saves and reload at the join, but both parameters on the stack);
// one variable for first and the loop index 159 (retail's head exactly: index in edx, count in
// ax/eax, zero in ecx, entries in edx; the index then lives in ebp and mode is spilled, where
// retail spills first and keeps mode in ebp); `first + n` with `entries[n]` 158; first by a
// chained assignment 158; the copy's destination through the stored current 164; the loop
// index set before the selection 162 (cache index in ecx); the selection as if/else or as the
// reversed ternary 164/164; selection, n/index, cursor or first declared early 164 each; the
// cursor before the copy with one index variable 163; the count in a short local read before
// the clears 164. The two g_4c1a80 statements (the same field as g_4c1a48[2].flags) removed:
// same code. Count, mode and first-index homes conflict across the inline copy: metadata
// pointer plus entry-table expression and early index 154; shared-scale entries 159; direct
// metadata with early index 159; const first 146; direct copy with late entries 163; first
// address direct/reference 163 each; early selection 169. Original block body restored.
// Round 16 (all indexed, no block pointer): first and index separate, entries pointer after the
// copy 164 (cache_index moves to the stack); one index variable, entries after 160 (retail's
// head to the clears, then the index takes EBP and mode stays on the stack, where retail loads
// mode into EBP at entry and spills first); first and index separate, entries pointer set before
// the copy 144 (mode in EBP at entry as retail, but the count lands in CX with the zero in EAX,
// so first stays in EAX across the copy and the entries pointer is spilled at once); one index
// variable with entries before the copy 163 (cache_index in EAX); a short count local for the
// guard, copy size and advance 177 (cache_index in ECX).
// Round 10 (lane 13): the indexed form is now 144 (492 bytes) but aligned 30 against the block form's 55:
// only the head and the cursor's reload at the loop head differ. On it: a separate cursor set before
// the loop 164 and the cursor defined after the copy 164 (the scaled index then takes EBP in LTCG's
// first pass and both parameters go to the stack); one variable for first and the loop index 163
// (stack convention too). The block as a reference 54 (the same code).
// Also: metadata pointer plus entry-table expression and early index 154; shared-scale entries 159;
// first address direct/reference 163 each; early selection 169.
// Round 99 (lane 6), indexed form (block field read at each use): plain 144 (492 bytes; 30 aligned
// differences against 55 for the block form: the param/zero/count registers rotate, first is kept
// in a register where retail loads it into EDI and spills it, and the entries cursor is spilled at
// once); a long count named at the block top 140 (29 aligned), before the `if` 165, as a short
// before the `if` 177 (480 bytes), named after first 166.
// takes cache_index in EDX (its caller 0x404b0 passes it there), multiplies it in place, reads the
// count once as a short into AX (zero in ECX) and clears the globals before the loop; ours computes
// the index in EAX and stores the table pointer later. Without the block pointer (count and entries
// indexed at each use) the head is retail's (scaled index in EDX, spilled) and so is the loop tail,
// but count lands in CX/EDX (retail AX/EAX, zero in ECX) and first stays in EAX where retail loads
// it into EDI and spills it across the inline memcpy; the entries cursor is spilled at once.
// Tried: indexed form 144 (count through the metadata view or the block, 492 bytes; aligned 30
// against the block form's 55); entries indexed by the counter 164; separate cursor pointer 54;
// with the current stored first 140; block pointer declared inside the if 151; index head with the
// block pointer only for the loop 155; count read once into a short local 177 (480 bytes); the
// block local renamed (records, zone) or taken as a reference 54; on the block form: each entry as
// `&block->entries[n]` 146 (494 bytes), no first local 162; metadata pointer plus entry-table
// expression and early index 154; shared-scale entries 159; direct metadata with early index 159;
// const first 146; direct copy with late entries 163; first address direct/reference 163 each;
// early selection 169. The two g_4c1a80 statements (the same field as g_4c1a48[2].flags) removed:
// same code. On the indexed form: the cursor set after the selection with the copy source indexed
// 164 (retail's loop, saves and reload at the join, but both parameters on the stack); the cursor
// defined after the copy or set before the loop 164 (the scaled index takes EBP, both parameters on
// the stack); one variable for first and the loop index 159-163 (retail's head exactly: index in
// EDX, count in AX/EAX, zero in ECX, entries in EDX; the index then lives in EBP and mode is
// spilled, where retail spills first and keeps mode in EBP), with entries after the copy 160, with
// entries before it 163 (cache_index in EAX); `first + n` with `entries[n]` 158; first by a
// chained assignment 140/158; current set from previous and first read back 140; the copy's
// destination through the stored current 164; the loop index set before the selection 162 (cache
// index in ECX); the selection as if/else or as the reversed ternary 164/164; selection, n/index,
// cursor or first declared early 164 each; the cursor before the copy with one index variable 163;
// the count in a short local read before the clears 164, for the guard, copy size and advance 177
// (cache_index in ECX); entries declared before first 145; a for loop over n 144 (141 with the
// chained first); entries pointer after the copy 164 (cache_index moves to the stack); no first
// local (previous read for the copy and the loop index) 157 (count AX/EAX, zero ECX and first EDI
// as retail, but the index arrives in EAX, the frame grows to 0x4c and previous is re-read); a long
// count named at the block top 140 (aligned 29: count in AX/EAX as retail, index in ECX, zero EDX),
// before the `if` 165, as a short before the `if` 177 (480 bytes), named after first 166. Round 41
// (fast, indexed form): the named long count with one first/index variable 163; the short count
// local with one first/index variable 112 (508 bytes but aligned 68); the field re-read for the
// copy and advance with one first/index variable 163; with the named long count, entries declared
// before first or before the count 140/140, the copy source indexed and entries set after the
// advance 155. The block form stays the closest.
// Plain group reset loops count/first/flags, count/flags/first, first/count/flags,
// flags/count/first with block and indexed receivers 54/54/54/54/144/144/144/144. Scaled-index
// and copy/count homes remain; original body restored.
// Lane 15 r19: retail's caller 0x404b0 passes the index in EDX and pushes mode; retail spills its
// scaled-index temporary above the loop locals where ours spills the named block pointer below them
// (every loop slot 4 bytes off). The per-iteration entry copy removed 54 (identical, kept); block
// pointer before the clears 148 (492 bytes); early return on `count <= 0` 54 (same code); indexed form
// without the entry copy 144 (492 bytes); entries before first 54 (same code).
// Round 6 (lane 17), indexed form (144; loop exactly retail's, head only): the index, count and zero
// always take EAX/ECX/EDX in a rotation (block EAX/ECX/EDX, indexed EDX/ECX/EAX, long count first
// ECX/EAX/EDX; retail EDX/EAX/ECX). On it: a short count read before the clears with the loop re-reading
// the field 177 (480 bytes); the clears in both arms of the guard 145; the guard as a bool computed before
// the clears 144; a long count before the clears 165; entries declared before the clears 146; `0 < count`
// 144; `count >= 1` 145; the clears through a group-reset helper 144.
// Round 107 (fast; the block form's entries cursor is biased +0xc, the second non-base read): indexed form
// 144 (492 bytes; index in EDX as retail, count CX/EDX and zero EAX where retail has AX/EAX and ECX, first
// in EAX across the copy where retail loads it into EDI and spills it); on it, current and first both
// read from previous 140 (495), `&g_4ba138[first]` as the copy destination 144, a for loop over n 144,
// the selection computed before the copy 170 (490).
// Round 13 (fast; aligned = difflib count): the indexed form again 144 (492 bytes, aligned 23 against the block
// form's 39: count CX/zero EAX where retail has AX/ECX, first in EAX where retail loads it into EDI and spills it,
// no loop-entry jump over the pad); on it an early return on `count <= 0` 144 (aligned 23), the copy size as
// `sizeof * count` 144 (23), `count >= 1` 145 (26); the block form with the geometry pointer renamed 54 (same code:
// the slot order does not follow that name); on the indexed form the loop index declared before n 145 (23).
// Round 112 (fast): the parameters as (mode, cache_index) at the definition and call 54 (same code: the one
// register argument stays EAX). The arrival register follows the colouring of cache_index, the count and the
// clears' zero: block form index EAX, count CX, zero EDX; indexed form EDX/CX/EAX 144; a named long count
// at the block top ECX/AX/EDX 140 (retail EDX/AX/ECX; with retail's zero in ECX the six clears are 6-byte
// stores, which is most of the indexed forms' 13-16 missing bytes). On that named count: the copy source as
// the field 140, entries set after the advance 155, entries set before the loop 158, index declared before n
// 141, the count as a short local 177 (480 bytes). A /Gd compile-only first pass loads cache_index into EBX
// (block form) or EBP (indexed), so it does not predict the arrival register here.
// Round 28 (lane 14), `--fast`: the indexed form rebuilt 144 (492 bytes, aligned 30: count CX and zero EAX where
// retail has AX/ECX, so the six clears take the 5-byte EAX stores; first kept in EAX where retail loads it into
// EDI and spills it; the cursor stored to mode's home before the copy where retail keeps entries in EDX and
// stores the cursor at the loop head); the clears as two chained assignments per group or one chain of six 54/54
// (same code) and on the indexed form 144/144 (same code); on the block form the cursor advanced before the
// counters 58, a for loop over n 60, `0 < block->count` 54 (same code).
// Round 99 (lane 8), indexed forms (index in EDX as retail; count CX and zero EAX where retail has AX and ECX):
// rebuilt 144 (492 bytes); guard as `0 < count` 144; a `block_index` local for every block access 164; first read back
// from the stored current 140 (495); `memcpy(&g_4ba138[first], entries, sizeof * count)` 144; entries declared after the
// copy 168 (479); a long count named before the clears 165, at the top of the `if` 140 (495), with first after it 140.
// Round 21 (m3 lane 2, fast), indexed form with `long count = field;` for the copy size and advance and the guard and
// loop still on the field: retail's count handling exactly (short load to AX, `cmp ax, <zero>`, `movsx eax, ax` in
// place), but the index takes ECX and the zero EDX, so first keeps EDX across the copy and entries is spilled: 140
// (495); the count declared after `current = first` 166; before the clears 139 (508 bytes, frame 0x4c, zero ESI);
// entries then count first in the block 140; copy source as the field address, entries after the advance 155; the
// copy as `sizeof * count` into `&g_4ba138[first]` 140; through the metadata view 140.
// Round 40 (reduced-relink screen, base 49 on its scale): 36 combinations of the block pointer or indexed
// blocks, the count as the field, a long at the top of the `if` or a short before the clears, the entries
// cursor before the copy or after the advance, a separate index or `first + n`, and the block pointer before
// or after the clears: nothing below the current body; indexed with a short count before the clears and the
// cursor after the advance 60 (508 bytes; the index arrives in EAX, mode stays on the stack); every
// `first + n` form 114-124 (aligned 37); indexed with the field at each use 123-146.
// Round 120 (lane 6, fast): on the block form, `current` set from `previous` and `first` reading
// `previous` again 130; the block as `blocks + cache_index + 1` 54 (same code); `current` advanced
// before the copy 167 (490 bytes); the loop index starting from `previous` read again 162 (493);
// non-const block and entries pointers 54 (same code). Indexed form rebuilt 144 (count CX, zero EAX,
// first EAX kept across the copy, entries ESI spilled at once); with a named long count 140 (count
// AX/EAX as retail, but the index arrives in ECX with the zero in EDX, first in EDX); that with the copy
// source as the field and entries set after the advance 155 (index arrives in EAX, scaled in EBX).
// Round 34 (lane 13, LTCG harness through 0x404b0, harness positional; base 49 = 54, indexed form 125 at
// 492 bytes, aligned 30): on the indexed form geometry declared at the loop-body top 125, `geometry !=
// NULL` 125 (both identical), material as one ternary 112 (494 bytes, aligned 33), weight after the
// material index 125 (aligned 40), shape before weight 124 (aligned 46); on the block form geometry
// declared first 49 (identical, 54).
// Round 24 (m3 lane 2), on the indexed form (count read through blocks[cache_index + 1] at each use,
// re-measured 144 at 492 bytes: the cache index in EDX as retail, but the clears' zero in EAX and the
// count in CX, so the first index keeps EAX where retail loads it into EDI and spills it): the two
// clears as a loop over entries 1 and 2 144 (unrolled to the same code); the loop index declared
// before the counter 145; the selection mask as a long 144; a named long count with one variable
// for the first and loop index 166 (cache index back in EAX). The caller 0x404b0 reloads the cache
// index from its stack slot for the call, so the register is this function's choice. On the block
// form, the clears as that loop 54 (same code).
// Round 107 (lane 8, fast; aligned = difflib count, block form 46): indexed form re-measured 144 (index EDX,
// count CX, zero EAX); with a long count named at the `if` top 140 (index ECX, count AX, zero EDX). On the
// block form: a long count named at the `if` top for the copy and advance 130 (508 bytes, aligned 41), with
// the loop on it too 181 (479 bytes); the indexed named count with the loop on it 178 (479); a long count
// named before the guard and used for it, the copy and the advance 161 (495); the guard as `>= 1` 54 (same
// code). Retail's `first` lives in EDI (the copy's destination register) and is spilled at once; every form
// here keeps it in a scratch register across the copy.
// Round 36 (m3 lane 14, fast), on the indexed form with a long count at the block top (rebuilt 140, 495 bytes:
// retail's count handling, but cache_index ECX and the zero EDX where retail has EDX/ECX, so first is loaded
// into EDX before the pushes and kept across the copy where retail loads it into EDI and spills it, and the
// entries pointer goes to mode's home before the copy where retail keeps it in EDX): entries set before first
// 140 (same code); the copy into `&g_4ba138[first]` with current set to `first + count` after it 164; a
// separate walking cursor initialised from entries before or after n/index 158/158 (both parameters move to
// the stack, the scaled index takes EBP, the zero takes ECX as retail); current stored after the copy 164;
// first read before count 166.
// Round 124 (lane 5): an LTCG env link of this file plus the callee files' objects, with an entry calling
// 0x404b0, gives the full build's listing instruction for instruction (2 s a form). On it (positional/aligned):
// block form with `current` read back for first 125/37 (153 instructions); a short count before the `if` on
// the block 60/53; indexed forms 125-145 (aligned 29-30 at 148-150 instructions: index ECX and zero EDX where
// retail has EDX/ECX, first kept in EDX across the copy where retail loads it into EDI and spills it, entries
// spilled to the cursor home at once where retail builds it in EDX after the copy; the indexed form with a
// long count in the `if` is 140 in the real build); indexed with a short count and the entries pointer after
// the copy 60/75-80; a separate loop cursor copied from the entries pointer (retail's `mov ebx, edx`), 32
// combinations of count/first/cursor/copy-source/loop forms, 129-130/47-48.
// Round 126 (lane 5; env link positional, 49 = 54): unsigned cache_index, first, index or first and index on
// the block and indexed forms 49/125 (same code each); 60 orders of the `if` head statements (first,
// current, entries, a long count, copy, advance, selection) on the block form 49-146 (best the current order),
// 60 on the indexed form 125-145; a short count inside the `if` with the loop re-reading the field on the block
// 143 (164 real), indexed 145; a dword count at the `if` top indexed 126 (140 real, 495 bytes), block 125; real
// `--fast`: unsigned cache_index 54, current/count/first/entries/copy/selection/advance on the block 66, first/
// entries/current/copy/selection/advance 66.
// Round 47 (lane 11, fast): the block pointer renamed to sort after or before the other locals 54/54
// (same code: its slot stays between n and the weight, where retail has the weight and the scaled
// index is a temporary after geometry); the count read at each use through the caller's metadata
// view (`g_4c6b00[cache_index].count`) with the entries from the record view 144 (492 bytes: the
// indexed form's index EDX, count CX, zero EAX, first EAX, entries ESI spilled at once, where retail
// has count AX/EAX, zero ECX, first EDI and entries EDX); that with a short count local at the `if`
// or before the clears 177/177 (480 bytes).
// Round 70 (lane 12, fast): indexed form 144 (492 bytes; count CX and zero EAX where retail has AX and ECX,
// first in EAX where retail loads it into EDI); a short count local before or after the clears 177/177 (480
// bytes: the short stays in BX, is extended into EDX for the copy and tested as `test bx, bx` at the loop
// guard, where retail extends it once into EAX and tests EAX); on the indexed form the parameters as (mode,
// cache_index) at the definition and the call 144 (same code), first as `current = first = previous` 144.
// @retail 0x402b0
void function_402b0(long cache_index, long mode)
{
    g_4c1a48[1].count = 0;
    g_4c1a48[1].first = 0;
    g_4c1a48[1].flags = 0;
    g_4c1a48[2].count = 0;
    g_4c1a48[2].first = 0;
    g_4c1a48[2].flags = 0;
    s_render_index_cache_block const *block = &g_4c62f8.records.blocks[cache_index + 1];
    if (block->count > 0)
    {
        long first = g_4c0b78.previous;
        g_4c0b78.current = first;
        s_44940_entry const *entries = block->entries;
        memcpy(g_4ba138 + first, entries, block->count * sizeof(s_44940_entry));
        g_4c0b78.current += block->count;
        dword selection = !mode ? ~0UL : mode == 2 ? 0x10 : 0x20;
        long n = 0;
        long index = first;
        while (n < block->count)
        {
            if (entries->unknown00 & selection & 0x1fffff &&
                entries->unknown00 & 1 && entries->unknown10[0xa] > 0 && (entries->flags & 0x1f0) == 0x1f0)
            {
                byte weight = g_4c0b78.weights[index];
                long shape = entries->flags & 0xf;
                byte material_index = g_4c0b78.material_indices[(short)index];
                long material = NONE;
                if (material_index != 0xff)
                    material = g_4c0b78.materials[material_index];
                byte *geometry = (byte *)function_449e0((short)index, false, (bool)((entries->unknown00 >> 12) & 1));
                if (geometry)
                {
                    for (long part = 0; part < *(long *)geometry; ++part)
                    {
                        s_render_part_context context;
                        if (function_45ce0((short)index, (byte *)&context, (short)part, (short)shape) &&
                            function_460d0(*(dword const **)(entries->unknown10 + 4), (short)index, part))
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
            ++n;
            ++index;
            ++entries;
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

// score 193 (was 193): BLOCKED-BY 0x44370. Frame 0x48 and geometry agree. Round 81 (fast; aligned
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
        byte *geometry = (byte *)function_449e0((short)index_bits, false, (bool)((entry->unknown00 >> 12) & 1));
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
    bool selected;
    if (single_pass)
    {
        selected = (bool)((g_4c1a48[0].flags >> 16) & 1);
        if (selected)
        {
            g_467130 = 16; g_467134 = 0; g_467138 = &g_4c1a48[0];
            g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true); function_15680(0);
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
    }
    else
    {
        selected = (bool)((g_4c1a48[0].flags >> 1) & 1);
        if (selected)
        {
            g_467130 = 1; g_467134 = 0; g_467138 = &g_4c1a48[0]; g_4858b4 = 1;
            function_14bc0(1, 0, true);
            if (!g_485a75 && !g_485a76) function_14b60(1, 0, 0.0f, 0);
            bool stipple_enabled = g_485898 < 4 || g_485898 > 7;
            function_15680(stipple_enabled);
        }
        else g_467130 = 0;
        function_15370(1);
        if (selected) { function_44550(); function_445d0(); }
        selected = (bool)((g_4c1a48[0].flags >> 3) & 1);
        if (selected)
        {
            g_467130 = 3; g_467134 = 0; g_467138 = &g_4c1a48[0]; g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true);
            bool stipple_enabled = g_485898 < 4 || g_485898 > 7;
            function_15680(stipple_enabled);
        }
        else g_467130 = 0;
        function_15370(1);
        if (selected) { function_44550(); function_445d0(); }
        selected = (bool)((g_4c1a48[0].flags >> 8) & 1) && g_4ba022;
        if (selected)
        {
            g_467130 = 8; g_467134 = 0; g_467138 = &g_4c1a48[0]; g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true); function_15680(0);
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
        selected = (bool)((g_4c1a48[0].flags >> 20) & 1);
        if (selected)
        {
            g_467130 = 20; g_467134 = 0; g_467138 = &g_4c1a48[0]; g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true); function_15680(0);
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
        selected = (bool)((g_4c1a48[0].flags >> 7) & 1);
        if (selected)
        {
            g_467130 = 7; g_467134 = 0; g_467138 = &g_4c1a48[0]; g_4858b4 = g_4858b8;
            function_14bc0((short)g_4858b8, 0, true); function_15680(0);
        }
        else g_467130 = 0;
        function_15370(0);
        if (selected) { function_44550(); function_445d0(); }
        if (function_44370(2)) { function_44550(); function_445d0(); }
    }
    g_4c1a48[0].flags = g_4c1a48[0].unknown10;
    g_4c1a48[0].first = 0;
    return true;
}
