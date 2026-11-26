// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_22F6CA.CPP: building a screen from its definition:
   its title and subtitle, its panes (their buttons, list, texts, bitmaps,
   models and widget groups) and the widgets of its widget set */

#include "unknown_11c920.h"
#include <string.h>
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "unknown_2312b4.h"

s_screen_definition *function_22f871(c_class_1473c9 *screen);
void function_08cc20(struct s_name_buffer *buffer, const wchar_t *name);
void function_253765(c_text_widget_45a5e0 *widget, short index, s_text_block const *definition);
void function_2538a6(c_text_widget_45a5e0 *widget, short value04, short font, short flags, color4f const *color, s_widget_bounds const *bounds);
void function_253bc9(c_text_widget_458940 *widget, long subtitle_type);
void function_253cff(c_class_19b8b1 *button);
void function_24bc12(c_class_1474e8 *list, bool remove_extra);
c_widget_45adf0 *function_2baf38(c_class_1a2c81 *parent, s_widget_block_24 *definition);

/* ---- the widgets a pane holds ---- */

// @retail 0x22f9bf
c_text_widget_45a5e0 *function_22f9bf(c_class_1473c9 *screen, long index, s_text_block *definition)
{
	c_text_widget_45a5e0 *text;

	if (definition->flags & 0x10)
	{
		text = new c_text_widget_32(screen->user_flags);
	}
	else
	{
		text = new c_text_widget_458940(screen->user_flags);
	}
	if (text)
	{
		text->m6c = true;
		screen->add_child(text);
		function_253765(text, (short)index, definition);
	}
	return text;
}

// @retail 0x22fa30
c_class_1a2c81 *function_22fa30(c_class_1a2c81 *parent, s_bitmap_block *definition)
{
	c_class_2b01eb *bitmap = new c_class_2b01eb(definition);

	if (bitmap)
	{
		bitmap->m6c = true;
		parent->add_child(bitmap);
	}
	return bitmap;
}

// @retail 0x22fa5b
c_class_1a2c81 *function_22fa5b(c_class_1a2c81 *parent, s_model_block *definition)
{
	c_class_2b0b5e *model = new c_class_2b0b5e(definition);

	if (model)
	{
		model->m6c = true;
		parent->add_child(model);
	}
	return model;
}

// @retail 0x22fa7d
c_widget_45ad18 *function_22fa7d(c_class_1473c9 *screen, long index, s_widget_block_18 *definition)
{
	c_widget_45ad18 *widget = new c_widget_45ad18(index, definition);

	if (widget)
	{
		s_widget_point origin;

		widget->m6c = true;
		screen->add_child(widget);
		memset(&origin, 0, sizeof(origin));
		widget->place(&origin);
	}
	return widget;
}

/* removes and deletes the screen's own widgets (not its title and
   subtitle), before its pane is built again */
// @retail 0x22fba9
void function_22fba9(c_class_1473c9 *screen)
{
	c_class_1a2c81 *widget = screen->child;

	while (widget)
	{
		c_class_1a2c81 *next_widget = widget->next;

		if (widget->type > 1 && widget->type > 5 &&
			(widget->type != 6 || widget != &screen->title && widget != &screen->subtitle))
		{
			screen->remove_child(widget);
			widget->v2();
			if (widget->m6c)
			{
				widget->~c_class_1a2c81();
				function_1a4826(widget);
			}
		}
		widget = next_widget;
	}
}

/* the widgets of the current pane: bitmaps, groups, texts and models */
// score 23 (was 23): the text loop's offset reset is mov where retail has and, the model loop's
// the other way, and the model loop copies the screen to eax before reading its animation type
// (retail uses esi, the call's register argument, directly; both copy it in the text loop).
// Tried: the text definition as pointer arithmetic, the text loop as a while, a model
// definition local, a named animation type, the model helper taking the screen type: all 23.
// Round 45: own counters for the text loop or the model loop, the text result typed as the
// helper's return type, the text test nested, the inner counter declared with i: all 23.
// Round 11 (lane 18): the four alike loop bodies (bitmaps, block-24, block-18, models) through one
// shared inline helper 187 (kept out of line under /O1), as __forceinline 23 (identical code); the
// block-18 element read at each use (no block pointer) 61; `long i = 0;` declared before the pane
// lookup 25 (the text loop's reset becomes retail's `and`, but the zero moves above the pane call),
// with the model loop's own counter as well 25; the text result declared once before its loop 23;
// `animating` declared before the pane test 23.
// Round 101: the inner counter initialized at declaration 23; concrete block-24 or block-18 result
// types 23 each; implicit model index store 23; text or model preincrement 23 each; a forceinline
// setup helper for only the first three alike loops 23; both concrete group result types 23.
// Round 50 (lane 13, fast; aligned 4: the two zero stores and the model loop's copy): counter uninitialised with `for (i = 0`
// in the first loop 66; model animation without braces 23; model or text definition local 23; parent local for the model call
// 23; `if (!pane) return;` 23; model loop as a while 23; text test nested 23; text loop's own counter 23; counter before the
// pane lookup 25; screen pointer local in the model body or above its loop 23; animation type local 23, before the call 166.
// Round 121 (fast; aligned 17 throughout): one inner counter declared with i and reused for the text loop 23, the model loop 23, both
// 23; the old start_animation name for the model loop 23, the text loop 23, both 23; model declared apart from its assignment 23; declarations apart from assignments 23;
// pane != NULL with animating assigned apart 23; model loop as a while with its own reset 23.
// Round 51 (fast): `long i = 0;` above the pane lookup again 25 (text store `and`, model store still `and`); model loop's `if (!model) continue;` 23;
// own counters over the first, block-18, text and model loops in all 15 combinations 23-73; first loop resetting the counter with the zero above the if or above
// the call, `pane != NULL`, an extra `i = 0;` in the if 66 each; `long i;` above the lookup and `i = 0;` above the test 23; the model read through a base cast 23.
// Stores (block-18, text, model): counter zeroed above the if mov/mov/and, above the call mov/and/and, inside the if mov/mov/mov; retail mov/and/mov.
// Round 216 (m3 lane 3, fast): the model call and its animation read through one `c_class_1a2c81 *parent = screen;`
// (0x22fa5b takes the base pointer) 23 (same code: the EAX copy stays); the class's inline function_22e957 wrapper for
// the model loop's animation start 23; own text/model/both counters with the counter zeroed above the pane call 25/25/25,
// zeroed inside the if 66/66/66, above the if 23/23/23 (own counters never change the code). In retail the loops with a
// value0a store reset the offset with `mov` and the text loop (none) with `and`; ours swaps the text and model loops.
// Round 94 (lane 16, fast): text, model or block-18 element as pointer plus index 23/23/23; `bool const animating` 23;
// a const pane pointer 23.
// Round 73 (lane 14, fast): the text loop as `if (!text) continue;` then the animation test 23; that with the model loop
// as `if (!model) continue;` 23; the block-18 inner counter declared with i at the top 23; the model call through a base
// pointer declared in the loop 23; the text call assigned inside its test 23 (all the same code).
// Round 64 (m3 lane 2, fast): the text test as `animating && text` 26; the block-18 inner counter declared in its
// `for` 23; the model loop's start as `if (model && animating)` with the store under its own `if (model)` (0x22facd's
// shape) 23; the model pointer declared before its loop 23; a `*const` screen parameter 23 (all same code but the
// first); permuter (600 s, 55 tries) best 21 instructions, its baseline. The model loop's copy is the normal shape
// (matched 0x22facd copies the screen into ECX too); only retail's last loop reads [esi+0x34] directly.
// Round 85 (m3 lane 14, fast): the text helper 0x22f9bf taking a short index 23 (same code); the model helper 0x22fa5b as
// a member of the base widget (`this` the parent) 23, this function as a `__stdcall` member of the screen 23 (both same code,
// every caller still matches); one widget local shared by the five loops 23 (same code); the model loop's test as
// `pane->model_count > i` 23 (only the compare flips). Diagnostics: without the model loop's value0a store the model needs
// no callee-saved register and the screen is read as [esi+0x34] directly (54); one more loop after the model loop turns the
// model loop's reset into `mov` but keeps the EAX copy, so in our build the `and` comes from being the last loop and the copy
// from the model living in EDI.
// Round 86 (m3 lane 13, fast): the model loop in a force-inlined helper taking the screen, the pane and the flag 23
// (same code); `long i = 0;` above the pane lookup, re-measured, 25 (the text store becomes retail's `and`, the zero
// moves above the call, the model loop unchanged); an empty trailing loop over another pane block after the model
// loop 23 (deleted, same code); the text loop in such a helper 23, both loops in helpers 23 (same code). Diagnostics:
// without the second loop the stores stay mov/mov/and (text mov, model
// and), so the `and` follows the last loop, not a count of loops; the text loop placed after the model loop changes
// the whole allocation (screen kept in EBX). Reading: `and [slot], reg` is a constant-zero store whose immediate a late
// pass replaced by a zero register, `mov [slot], reg` a store the optimizer already tied to the counter; retail has
// the first in the text loop only.
// Round 157 (m3 lane 7, fast): the model loop walking a definition pointer (incremented in the call) 150 (421 bytes);
// the text loop walking one 46 (419 bytes); 0x22fa5b's parent parameter typed as the screen class, as 0x22f9bf's, 23 (same
// code). Stopped there: round 51's 15 counter combinations and round 86's diagnostics cover the counter and loop spellings.
// Round 122 (m3 lane 9, fast; control 23): 0-7 unused locals at the top 23 at every phase; the shared counter declared in the
// pane block after the flag 66; the model's animation start without braces 23; the text result typed as 0x22f9bf's return
// type 23. Diagnostic: the model's value0a store ahead of the animation start 11 at 414 bytes (the model stays in EAX, no
// EDI save or screen copy; the resets stay swapped), but retail stores after the call.
// Round 164 (m3 lane 7, fast): widgets and the flag given descriptive names 23, counters named index/child_index 23, the
// model result typed as the model widget (cast at the call) 23, the model animation start through the qualified base
// member 23 (all same code); the shared counter declared inside the pane block ahead of the flag 66.
// Round 166 (m3 lane 7, fast): one `long const &` to the screen's animation type read by all five loops 23; the screen as a
// base-widget parent pointer for the helper calls and every animation read 23; the model declared in its if condition 23;
// the text declared in its if condition with the animation test nested 23; both 23 (all the same code).
// Round 186 (m3 lane 6, fast): on `long i = 0;` above the pane lookup (25: the text reset becomes retail's `and`, the model
// reset stays `and` with the EAX copy): the model loop as a `while` 25, with `if (!model) continue;` 25, with `if (model &&
// animating)` and the store under its own `if (model)` 25, with its own counter declared in its `for` 25 or at the top 25 (all
// the same code); on main's body the text loop's own counter zeroed above the pane lookup 23 (same code).
// M3 lane 2 r90 (fast; all the same code): a `long const &` to the screen's animation type declared after the model call
// and read by the model's start 23, the same in the text loop 23; the model result as a `*const` local 23; the model
// definition as an `s_model_block &` element reference passed by address 23; the pane as a `*const &` bound to the getter's
// result 23. (The flag getter cannot take the screen without its cast: c_widget is not one of its bases.)
// M3 lane 13 r101 (fast): the definition moved to the end of the file (a prototype left in its place) 23; the implicit
// short conversion at all four value0a stores 23; the model's animation read through an explicit base-widget cast of the
// screen 23; all five animation starts through a forceinline (widget, screen) helper 23 (all the same code); `long i = 0;`
// above the pane call with `if (!pane) return;` 25 (the zero moves above the call). Permuter (seed 101, 600 s, fast,
// anneal): 76 tries, none below its reduced-image baseline 21 (full image 23).
// M3 lane 5 r193 (fast): a guard `if (pane->model_count > 0)` around the model loop 43 (422 bytes) and `if
// (pane->text_count > 0)` around the text loop 58 (418 bytes) (both emit their own test, nothing folds); the screen bound
// to a `c_class_1a2c81 &` before the model loop and used for its call and animation read 23; the model result as a
// `*const &` bound to the call 23; one widget local at function scope shared by the text and model loops 23 (all the
// same code). Retail's model loop alone reads `[esi + 0x34]` from the call's ESI; matched 0x22facd copies there too.
// M3 lane 12 r137 (fast; all the same code as main): a file-local inline accessor for the screen's animation type in the model
// loop 23, in the text loop 23, in all five loops 23; an inline value0a setter in the model loop 23, in the three value0a loops
// 23; accessor and setter together in the model loop 23; the model call and animation read through a `c_class_1473c9 *&`
// bound to the screen 23; the model bound to a `c_class_1a2c81 *&` over a plain local 23; `i = 0; if (i < count)` guarding the
// model loop 23, the text loop 23 (the guard folds away; it moves neither the resets nor the EAX copy).
// M3 lane 5 r198 (fast; control 23): the model loop on its own counter zeroed at its declaration with no reset in the
// `for`: declared at the function's top 150 (423 bytes), at the pane block's top 145, just before the text loop 74 (the
// counter then lives in memory across the earlier loops); `(short)i` at the text helper's call 53 (419 bytes); one widget
// pointer shared by the five loops and initialised NULL at the top 23; the flag declared false at the top, after or
// before the counter, and set in the pane block 23/23 (all three the same code). Diagnostics: a constant zero stored to
// a global after the model loop 31 (423 bytes: the model's reset stays `and`, the copy stays); the model loop deleted
// 131 (the text loop, then last, keeps `mov`); the model loop moved ahead of the text loop 158 (the model's reset is
// still the `and`), so in our build the `and` goes with the model loop wherever it stands, not with the last loop; a
// copy of the text loop after the model loop 108 and the text loop given a value0a store 57 (in both the model moves
// to ESI and the screen is reloaded from its home, no direct read); the model loop without its value0a store 54.
// Reading: `mov [slot], reg` is the zero shared with the counter's own register, `and [slot], reg` a literal zero the
// peephole pass rewrote; retail shares it in the model loop and not in the text loop.
// M3 lane 2 r99 (fast): `inline` on 0x22fa5b's definition 23 (stays out of line, same code); the model's value0a store
// written in both arms of the animation test 23; a separate widget local per loop declared at the pane block's top 23;
// `long i = 0;` above the pane lookup with the first loop also `for (i = 0;` 66, and the first loop `for (i = 0;` under
// main's `long i = 0;` 66 (both give one zero register, EBX, for the whole function; the model reset becomes `mov`).
// M3 lane 4 r214 (fast, with the file's 10 matched functions; aligned 4 / masked 3: the text and model resets
// swapped and the model loop's EAX copy of the screen): the flag as `char` 23; a `const` pane pointer 23; the text
// test as `text != NULL && animating` 23; the text loop's tests as `if (!text || !animating) continue;` 23; the text
// loop's counter reset as `i = 0;` ahead of `for (; ...)` 23; the same for the model loop 23 (all the same code).
// M3 lane 6 r197 (fast): `int i` 23 (same code); on the first loop's `for (i = 0;` under `long i = 0;` (66, re-measured): the
// text loop on its own `for (long t = 0;` counter 66, the model loop on its own 66, both 66 (every reset is then `mov` from
// the one zero register EBX, and the zero-register difference is not per loop).
// M3 lane 3 r266 (fast, control 23): the text loop's tests nested as `if (text) { if (animating) ... }` 23; the model
// call and its animation read through a `c_class_1a2c81 *parent = screen;` declared in the model loop's body 23; the
// text definition named before its call 23; the model definition named before its call 23; the text loop's animation
// read through an explicit base-widget cast 23 (all five the same code).
// M3 lane 4 r221 (fast, with the file's 10 matched functions): an `s_type_0cfb31 const &` to the screen's animation
// record read for the start in the model loop 23, in the text loop 23, in both 23; the text loop's start through the
// class's inline function_22e957 wrapper 23, both loops through it 23 (all the same code). Diagnostics: the text helper
// given a constant 0 for its index 47 (420 bytes: the text loop's counter moves to memory and the offset to ESI, the
// model loop unchanged, still `and` and the EAX copy); without the model's value0a store 54 (re-measured).
// M3 lane 2 r112 (fast with the file's 10 matched functions; control 23): `register long i` 23; the model's value0a
// through a `short index` named in its `if (model)` block 23; the model tests as `!= NULL` and `!= false` 23; the model
// loop as `i = 0; if (i < count) do ... while (++i < count);` 23 (all four the same code); the model loop walking a
// definition pointer stepped at the body's end 32 (412 B). Diagnostics: the text loop deleted (369 B) or without its
// animation start (395 B) moves the model to ESI and reloads the screen from its home, so the model loop's copy depends
// on the text loop. A compile-only build (no /GL) gives `and` resets in both loops and does not reproduce this function.
// M3 lane 8 r197 (fast with 0x22facd, 0x230427, 0x22f6ca): the screen parameter typed as the base widget (downcast for
// the pane getter and the two screen-typed helpers, the declaration in screen_widget_core.cpp changed to match) 23;
// the model definition as `pane->models + i` 23; the text and model loops stepping `++i` 23; the model's value0a
// store without the cast 23; the text call's definition as `pane->texts + i` 23 (all the same code). Compile-only (no
// LTCG conventions) gives both the text and model resets as `and` and reads the text loop's screen without a copy.
// M3 lane 3 r274 (fast, control 23): on main's body the text loop on its own counter declared in its `for` 23, the model
// loop so 23, both 23; the flag as `bool const` 23; a `continue;` closing the model loop's `if (model)` block 23 (all five
// the same code).
// M3 lane 3 r276 (fast with the file's 10 matched functions, control 23): the text loop on its own `int` counter 23; the
// model loop on its own `int` counter 23 (both the same code); `long i;` declared after the flag inside the pane block and
// zeroed by the first loop's `for (i = 0;` 66, that with the text loop's own `int` counter 66 (one zero register, EBX,
// for the whole function, every reset `mov`); `long i = 0;` above the pane call with the model loop on its own counter
// zeroed just ahead of it 25 (the text reset becomes retail's `and`, the model reset stays `and` with the EAX copy).
// M3 lane 6 r210 (fast): the model call given an explicit base-widget cast of the screen 23; the model loop as
// `if (!model) continue;` then the start and the store 23; the third loop's inner counter declared in its `for` 23; the
// text result typed as the text helper's return type 23, with the block-24 and block-18 results typed as theirs too 23
// (all the same code). Diagnostic: 1-8 unused initialised locals after `long i = 0;` 23 each (not a numbering effect).
// M3 lane 7 r199 (fast with 0x22facd/0x22fa5b, both MATCH): the model's animation type named in its animating block
// 23 (same code); the model's value0a store ahead of the start, re-measured, 11 at 414 B (the model then stays in EAX
// and is tested with `test eax, eax`; retail stores after the call); the model array named before its loop (`models =
// pane->models`) 38 (407 B), the text array so 41 (415 B), both 57 (406 B).
// M3 lane 16 r156 (fast with 0x22facd, 0x230427, 0x22f6ca; control 23): the text loop's test written
// `pane->text_count > i` 25 (check.py --fast agrees); the model's animation start reading the screen through a
// `c_class_1473c9 *const *` to the parameter 23 (same code, the EAX copy stays); a `continue;` after the model's value0a
// store 23 (same code). Diagnostics (not kept): the block-18 loop deleted 229 (327 B; the copy appears in the bitmap,
// block-24 and model loops), the bitmap loop deleted with the block-24 loop starting from the shared zero 194 (353 B; the
// model reset becomes `mov` as retail, but its EAX copy of the screen stays).
// M3 lane 5 r221 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): the model loop's animation read through a
// `c_class_1473c9 *const &` bound to the screen ahead of the loop 23, the text loop's read so 23, both 23; the flag returned into
// a `started` local read through a `bool const &` 23; a screen copy declared ahead of the model loop for its call and read 23
// (all five the same code: the reference and the copy fold, the EAX copy and the swapped resets stay).
// M3 lane 3 r285 (h2m fast, control 23): a second named local `c_class_1a2c81 *parent = screen;` at the pane block's top
// used only for the model call 23; a `c_class_1473c9 *owner = screen;` used only for the text call 23; the model loop's
// flag test through a `bool const &` to the flag 23; a redundant `i = 0;` ahead of the model loop's `for (i = 0;` 23, ahead
// of the text loop's 23 (all five the same code).
// M3 lane 16 r159 (h2m fast; control 23): diagnostics on the model loop's EAX copy of the screen: the model's animation
// started without the flag test 25 (410 B; the copy stays though only the null test's branch separates the call from the
// push); the model started with no null test or flag test 139 (403 B; the push then reads `[esi + 0x34]` directly, as
// retail, so our build keeps the call's ESI only inside one block); the animation type read ahead of the model call 166
// (431 B); the same in the text loop 84 (431 B); a dead self-store of the text's value0a 23 (folded away).
// M3 lane 16 r161 (h2m fast with 0x22facd, 0x230427, 0x22f6ca, 0x22fa5b, all MATCH; control 23): the model's animation
// type named right after the model call, ahead of the null test, 23 (same code, the EAX copy stays); a `bool const` copy
// of the flag for the model loop 23 (same code); the model call and its animation read through a `c_class_1473c9 *const
// &` bound to the screen at the pane block's top 23 (same code); the text test written `animating && text` 26; the model
// start under `animating && model` with the index store under its own null test 32 (420 B).
// M3 lane 6 r220 (h2m fast with 0x22facd, 0x230427, 0x22f6ca, 0x22fa5b, all MATCH; control 23): the model loop on its own
// `short` counter stored to value0a without a cast 152 (421 B); the text loop on its own `short` counter 55; both 153 (421
// B); the shared counter typed `short` 112 (411 B); the model result as a `*const` local 23 (same code).
// M3 lane 13 r136 (h2m fast with 0x22facd/0x22fa5b, both MATCH; control 23): the parameter copied into a `screen` local at the
// top 23; the model loop's counter through a `long &` bound to i 23; the text loop's animation type through a `long const &` in
// its start block 23 (all three the same code); 0x22fa5b's definition parameter typed const does not compile (the model
// constructor takes a non-const block).
// M3 lane 15 r125 (h2m fast with 0x22facd, 0x22fa5b, 0x22f9bf, 0x22f6ca, all MATCH; control 23): diagnostic, the counter's
// zero above the pane lookup plus the model index stored ahead of its start 13 at 414 B (the text reset becomes `and`, but
// the model reset stays `and` and the model loop still copies the screen, `mov ecx, esi`, with the model in EAX, so the copy
// does not come from the model's EDI); the text loop nested like the others with the zero above the pane lookup 25; the zero
// above the pane lookup with the model loop on its own `for` counter 25; the model loop on its own counter with the shared
// counter uninitialised and zeroed by the first loop's `for (i = 0;` 66; only the shared counter zeroed that way 66.
// M3 lane 4 r243 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23 at 416 B): the text loop on its own
// counter declared and zeroed above the pane lookup 144 (421 B: that zero is live across the call), with the model loop
// on its own counter as well 168 (433 B), with the shared counter declared bare and zeroed by the first loop's `for (i =
// 0;` 155 (423 B); the text loop's own counter declared at the pane block's top 137 (421 B); the text loop's tests nested
// together with the model loop in 0x22facd's shape (`if (model && animating)` start, the store under its own `if
// (model)`) 23 (same code).
// M3 lane 1 r150 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): the pane test as an early return
// (`if (!pane) return;`, and as `pane == NULL`) 23/23; the screen parameter declared `*const` 23; the model's animation
// type named inside its animating block 23; the model's `(short)i` named at the top of its `if (model)` block 23 (all five
// the same code: the swapped text/model zero stores and the model loop's EAX copy of the screen stay).
// M3 lane 6 r226 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): the text loop as a guarded do loop
// 23, the text and model loops both so 23; new trivial inline count accessors in the model loop's test, the text loop's
// test, both 23/23/23 (all five the same code).
// M3 lane 7 r213 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf/0x22f6ca, all MATCH; control 23; screened on a multi-object
// LTCG harness that reproduces this function exactly): 200 combinations of the counter scheme, early return, the
// `(short)` cast and nested or split guards in every loop give three codes only: main's; `long i = 0;` above the pane
// lookup (text reset retail's `and`, the zero rises above the call, the model loop keeps `and` and the screen copy) 25;
// a bare counter with `for (i = 0;` everywhere (every reset a `mov` from EBX). Forms: `long i;` in the pane block with
// `for (i = 0;` in every loop 66; `long i = 0;` in the pane block after the animating test 66; the model loop as `if
// (!model) continue;` 23; the model declared apart from its assignment 23; the model call or the type read through an
// explicit cast to the base type, both, or a qualified member: same code on the harness (the `mov eax, esi` copy stays).
// M3 lane 2 r137 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): on `long i = 0;` above the pane
// lookup (25): the model call and read through a base-typed local in the loop body 25; the model's null test as `if
// (!model) continue;` 25; the text tests nested 25; those two together 25; the base-typed local with the nested text
// tests 25 (all the 25 form's code). Retail has `and [frame slot], reg` in only four functions; in the matched ones it is
// a constant zero stored after a memset or compare left a zero register, so retail's text reset is a folded constant
// and its model reset a copy of the counter's register.
// M3 lane 14 r144 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf/0x22f6ca, all MATCH; control 23): the shared counter declared and
// zeroed at the pane block's top, ahead of the animating call 66; the model loop's test as `pane->model_count > i` 23 (same
// code); the text loop's test so 25, both 25; `long i = 0;` above the pane lookup with the model test swapped 25 (first
// +0xb). The matched sibling 0x22facd keeps a `mov ecx, esi` copy in its model loop, so the direct `[esi + 0x34]` read is
// particular to this loop.
// M3 lane 1 r157 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf/0x22f6ca, all MATCH each time; control 23). The screen's
// animation type read through a file-local inline accessor returning it as a one-long aggregate by value (the form that
// matched 0x151b80; such a copy is not merged with a plain read): in the model loop only 23, in the text loop only 23, in
// all five loops 23, in the text and model loops 23 (all four the same code as main); in the first four loops with the
// model loop plain 26 (417 B: the model takes ESI and the screen is reloaded from its home, as in the bitmap loop); in
// the bitmap loop only 160 (429 B, the shared zero moves to ESI); in the block-24 loop only 37 (415 B); in the block-18
// loop only 26 (417 B, the 26 code); in the first three loops 26. On `long i = 0;` above the pane lookup (25): plain 25
// re-measured, the accessor in the model loop 25, in the text loop 25 (same code each). The model loop's EAX copy and
// its `and` reset stay in every form whose model loop keeps the model in EDI.
// M3 lane 13 r145 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): one, two or three redundant
// parenthesis pairs around the model call's definition argument 23/23/23, around the text call's 23/23/23 (no phase
// effect on this integer code); 0x22fa5b returning the concrete model type, the model local typed so, 23; the text
// helper's index typed `short` with `(short)i` at the call 23 (same code); the pane declared in the `if` condition with
// `long i = 0;` above it 25 (the 25 form's code), with `long i = 0;` at the block's top 66. Diagnostic: the text loop
// without its flag test 46 (411 B; its reset stays `mov`, so the flag test does not set it).
// M3 lane 12 r179 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): the text loop's counter read through a
// `long &index = i;` bound in its body 23; the text and model loops both so 23 (both the same code); `long i = 0;` above
// the pane lookup with the text loop through that reference 25 (the 25 form's code); the block-18 inner counter declared
// `long j = 0;` 23 (same code); the pane test as an early `if (!pane) return;` with `long i = 0;` declared after it 66 (every
// reset then a `mov` from the one zero register).
// M3 lane 13 r150 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): the model count read once into a
// `long` local for the model loop's test 156 (424 B), the text count so 62 (421 B), both 163 (429 B); the model array bound
// to a `s_model_block *const &` ahead of its loop 23, the text array so 23 (both the same code). All callees are matched.
// M3 lane 15 r140 (h2m fast with 0x22facd/0x22fa5b/0x22f9bf, all MATCH; control 23): the text loop's NULL test as
// `if (!text) continue;` then the animation test 23; its animation test outside the NULL test 26; the model loop's start
// and store in one `model && animating` arm with an `else if (model)` store 23; `s_screen_pane *const pane` 23; explicit
// `!= NULL` / `!= false` tests in the text and model loops 23 (the 23s the same code). Reading: matched /O1 0x2b3e4d's
// `and [slot], eax` is a plain `field = 0;` store to a memory local after a `repe cmpsd` left EAX zero, so retail's text
// reset is a constant-zero store given the zero register late, its model reset a store of the counter's zero register.
// Roasted-Codes (#1508 comment, 2026-11-26; fast reduced-image scores): `long i = 0;` above the pane lookup plus the model
// loop as a guarded do/while over an explicit zero-initialised byte offset 24 (416 B); both resets are retail's (text `and`,
// model `mov`) and the earlier loops keep retail's registers; left are the `xor ebx, ebx` ahead of the pane call and the
// model loop's `mov eax, esi`. Without the byte offset 25 (model reset back to `and`); with the zero after the lookup 69 (418 B).
// @retail 0x22fc08
void __stdcall function_22fc08(c_class_1473c9 *screen)
{
	s_screen_pane *pane = screen->get_current_pane();
	long i = 0;

	if (pane)
	{
		bool animating = function_22f0ff((c_widget *)screen);

		for (; i < pane->bitmap_count; i++)
		{
			c_class_1a2c81 *bitmap = function_22fa30(screen, &pane->bitmaps[i]);

			if (bitmap)
			{
				if (animating)
				{
					bitmap->start_animation(screen->animation.type);
				}
				bitmap->value0a = (short)i;
			}
		}
		for (i = 0; i < pane->block_24_count; i++)
		{
			c_class_1a2c81 *widget = function_2baf38(screen, &pane->blocks_24[i]);

			if (widget)
			{
				if (animating)
				{
					widget->start_animation(screen->animation.type);
				}
				widget->value0a = (short)i;
			}
		}
		for (i = 0; i < pane->block_18_count; i++)
		{
			s_widget_block_18 *block = &pane->blocks_18[i];
			long j;

			for (j = 0; j < block->count; j++)
			{
				c_class_1a2c81 *widget = function_22fa7d(screen, j, block);

				if (widget)
				{
					if (animating)
					{
						widget->start_animation(screen->animation.type);
					}
					widget->value0a = (short)i;
				}
			}
		}
		for (i = 0; i < pane->text_count; i++)
		{
			c_class_1a2c81 *text = function_22f9bf(screen, i, &pane->texts[i]);

			if (text && animating)
			{
				text->start_animation(screen->animation.type);
			}
		}
		for (i = 0; i < pane->model_count; i++)
		{
			c_class_1a2c81 *model = function_22fa5b(screen, &pane->models[i]);

			if (model)
			{
				if (animating)
				{
					model->start_animation(screen->animation.type);
				}
				model->value0a = (short)i;
			}
		}
	}
}

/* the models and bitmaps of a widget set of the user interface globals */
// @retail 0x22facd
void function_22facd(c_class_1473c9 *screen, short set_index)
{
	bool animating = function_22f0ff((c_widget *)screen);

	if (set_index >= 0 && set_index < 0x20)
	{
		s_type_954545 *globals = ui_definition_get();

		if (globals && set_index < globals->widget_set_count)
		{
			s_widget_set *set = &globals->widget_sets[set_index];
			long i;

			for (i = 0; i < set->model_count; i++)
			{
				c_class_1a2c81 *model = function_22fa5b(screen, &set->models[i]);

				if (model && animating)
				{
					model->start_animation(screen->animation.type);
				}
			}
			for (i = 0; i < set->bitmap_count; i++)
			{
				c_class_1a2c81 *bitmap = function_22fa30(screen, &set->bitmaps[i]);

				if (bitmap)
				{
					if (animating)
					{
						bitmap->start_animation(screen->animation.type);
					}
					bitmap->value0a = (short)-i;
				}
			}
		}
	}
}

/* ---- the panes ---- */

/* builds the screen's current pane: its buttons or its list, then the rest
   of its widgets */
// @retail 0x22f6ca
void function_22f6ca(c_class_1473c9 *screen, s_screen_layout *layout, bool rebuild)
{
	if (rebuild || (screen->value5f0 >= 0 && screen->value5f0 < layout->count))
	{
		long i;
		s_screen_definition *definition = function_22f871(screen);
		s_screen_pane *pane = screen->get_current_pane();

		if (definition && pane)
		{
			screen->value68 = pane->value02 - 1;
			if (rebuild)
			{
				for (i = 0; i < pane->button_count; i++)
				{
					c_class_1a2c81 *button = layout->lists[screen->value5f0].widget[i];

					if (!button)
					{
						break;
					}
					screen->add_child(button);
					function_253cff((c_class_19b8b1 *)layout->lists[screen->value5f0].widget[i]);
				}
				if (pane->list_count > 0)
				{
					screen->add_child(layout->lists[screen->value5f0].list);
					function_24bc12(layout->lists[screen->value5f0].list, rebuild);
					goto configure_list;
				}
			}
			else if (layout->lists[screen->value5f0].type > 0)
			{
				long count = layout->lists[screen->value5f0].type;

				if (pane->button_count <= count)
				{
					count = pane->button_count;
				}
				for (i = 0; i < count; i++)
				{
					screen->add_child(layout->lists[screen->value5f0].widget[i]);
					function_253cff((c_class_19b8b1 *)layout->lists[screen->value5f0].widget[i]);
				}
			}
			else if (layout->lists[screen->value5f0].list)
			{
				screen->add_child(layout->lists[screen->value5f0].list);
				function_24bc12(layout->lists[screen->value5f0].list, false);
configure_list:
				if (definition->flag1)
				{
					layout->lists[screen->value5f0].list->notify_screen = true;
					s_list_head *head = &layout->lists[screen->value5f0].list->head80;
					s_list_head *const *head_reference = &head;
					delegate_register(*head_reference, (c_list_item_delegate *)&screen->delegate);
				}
			}
			function_22fc08(screen);
		}
	}
}

/* the title: its text and size come from the definition */
// @retail 0x22fda6
void __stdcall function_22fda6(c_class_1473c9 *screen)
{
	s_type_954545 *globals = ui_definition_get();
	s_screen_definition *definition = function_22f871(screen);
	word name[0x100];
	short pane_value;
	color4f color;
	long font;
	s_widget_bounds bounds;

	name[0] = 0;
	pane_value = screen->get_first_pane_value();
	color.alpha = 1.0f;
	color.red = 0.7f;
	color.green = 0.7f;
	color.blue = 0.7f;
	font = 1;
	if (definition ? (bool)(definition->no_title == 0) : true)
	{
		bounds.left = -100;
		bounds.right = 100;
		bounds.top = 100;
		bounds.bottom = 80;
		screen->add_child(&screen->title);
		if (globals)
		{
			color = globals->title_color;
			if (definition)
			{
				if (definition->flags & 1)
				{
					font = globals->title_fonts[3];
					bounds = globals->title_bounds[3].title;
				}
				else if (definition->flags & 8)
				{
					font = globals->title_fonts[2];
					bounds = globals->title_bounds[2].title;
				}
				else if (definition->flags & 0x10)
				{
					font = globals->title_fonts[1];
					bounds = globals->title_bounds[1].title;
				}
				else
				{
					font = globals->title_fonts[0];
					bounds = globals->title_bounds[0].title;
				}
				screen->title.function_253b1a(definition->title_string_id);
				function_08cc20((s_name_buffer *)name, (const wchar_t *)screen->title.function_22f52e()->function_22f52e());
			}
		}
		function_2538a6(&screen->title, pane_value, (short)font, 1, &color, &bounds);
		c_class_22cc8e *text = screen->title.function_22f52e();
		c_class_22cc8e *const *text_reference = &text;
		(*text_reference)->value14 |= 2;
		screen->title.function_22f52e()->set_text(name);
		screen->title.value6a = 0x7fff;
	}
}

/* the subtitle: the user's gamertag */
// @retail 0x22ff53
void __stdcall function_22ff53(c_class_1473c9 *screen)
{
	s_type_954545 *globals = ui_definition_get();
	s_screen_definition *definition = function_22f871(screen);
	short pane_value = screen->get_first_pane_value();
	word name[0x100];
	color4f color;
	s_widget_bounds bounds;

	name[0] = 0;
	color.alpha = 1.0f;
	color.red = 0.7f;
	color.green = 0.7f;
	color.blue = 0.7f;
	wcsncpy((wchar_t *)name, L"", 0xff);
	name[0xff] = 0;
	bounds.left = -100;
	bounds.right = 100;
	bounds.top = -100;
	bounds.bottom = -120;
	screen->add_child(&screen->subtitle);
	if (globals)
	{
		color = globals->title_color;
		if (definition)
		{
			long subtitle_type = definition->value06;
			color = definition->subtitle_color;
			if (definition->flags & 1)
			{
				bounds = globals->title_bounds[3].subtitle;
			}
			else if (definition->flags & 8)
			{
				bounds = globals->title_bounds[2].subtitle;
			}
			else if (definition->flags & 0x10)
			{
				bounds = globals->title_bounds[1].subtitle;
			}
			else
			{
				bounds = globals->title_bounds[0].subtitle;
			}
			function_253bc9(&screen->subtitle, subtitle_type);
			function_08cc20((s_name_buffer *)name, (const wchar_t *)screen->subtitle.function_22f52e()->function_22f52e());
		}
	}
	function_2538a6(&screen->subtitle, pane_value, 1, 2, &color, &bounds);
	screen->subtitle.function_22f52e()->set_text(name);
	screen->subtitle.value6a = 0x7fff;
}

/* builds the screen from its definition: its title and subtitle, its panes
   (each pane a child of the layout's container when there are several) and
   its widget set */
// @retail 0x22f8df
void c_class_1473c9::build(s_screen_layout *layout)
{
	s_screen_definition *definition = function_22f871(this);

	function_22fda6(this);
	function_22ff53(this);
	if (definition)
	{
		bool rebuild = layout->count == NONE;

		value5f4 = TEST_FIELD_BIT(definition->flag5);
		if (definition->pane_count > 0)
		{
			s_screen_pane *first_pane = definition->panes;

			if (definition->pane_count > 1 && layout->container)
			{
				c_class_1473c9 *pane = (c_class_1473c9 *)layout->container->child;
				short i;

				value5f0 = 0;
				value68 = first_pane->value02 - 1;
				add_child(layout->container);
				for (i = 0; i < definition->pane_count; i++)
				{
					pane->value5f0 = i;
					function_22f6ca(pane, layout, rebuild);
					pane = (c_class_1473c9 *)pane->next;
				}
			}
			else if (definition->pane_count == 1 || definition->flag1)
			{
				value5f0 = 0;
				function_22f6ca(this, layout, rebuild);
			}
			function_22facd(this, definition->widget_set - 1);
		}
	}
}
