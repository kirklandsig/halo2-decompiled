// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"

/* slot type 0x23 */

struct s_slot_23
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	short unknown0e;
	byte unknown10[0x1b - 0x10];
	bool unknown1b;
	bool unknown1c;
	char unknown1d;
	byte unknown1e[2];
	s_type_c3b527 point;
	vector3f facing;
	byte unknown3c[0x40 - 0x3c];
};

struct s_slot_23_data
{
    bool unknown00;
    bool unknown01;
    short unknown02;
    real unknown04;
    byte unknown08[0x0f - 0x08];
    bool unknown0f;
    bool unknown10;
    char unknown11;
    byte unknown12[2];
    s_type_c3b527 point;
    vector3f facing;
    byte unknown30[0x34 - 0x30];
};

short __stdcall function_1b84a0(long actor_index);
bool __stdcall function_1b85a0(long actor_index, s_slot *slot);
void __stdcall function_1b89d0(long actor_index, s_slot *slot);
void __stdcall function_1b8ae0(long actor_index, s_slot *slot);
bool function_25da00(s_prop_node_view *node);

// @retail 0x1b84a0
short __stdcall function_1b84a0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && !actor->unknown223)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (view && node->unknown24 >= 3 && (view->unknown70 == 1 || !function_25da00(node)))
			result = 3;
	}
	return result;
}

// @retail 0x1b8540
bool __stdcall function_1b8540(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && prop_node_get(actor->prop_index)->unknown27 < 2)
	{
		s_slot_23 *state = (s_slot_23 *)slot;

		state->unknown0c = false;
		state->unknown0d = false;
		state->unknown0e = 0;
		result = true;
	}
	return result;
}

real function_1f8940(long actor_index);
void *function_1e4db0(long actor_index);

/* the block of the actor's character tag function_1e4db0 returns */
struct s_character_db0_flags
{
	byte flags;
	byte unknown01[0xc - 1];
	real unknown0c;
	real unknown10;
};

bool function_1f86f0(long index);
bool function_1f4f40(long actor_index, vector3f const *facing, short unknown, s_type_c3b527 const *point, bool face_prop);
void function_262800(long actor_index, s_reference reference, bool unknown);

/* slot type 0x23, update: once the actor stands at its prop's position, it
   turns to face it; then it lets the prop go */
// score 7 (was 7): only argument temporaries differ: the 0x1f4f40 short argument is converted into DX
// and pushed after `mov eax, ebp` (retail converts AL in place into AX and pushes it first), and the
// 0x262800 reference is loaded into EAX (retail ECX).
// Earlier (47): the actor index, actor, view and state registers differ (retail EBP/EDI/EBX/ESI,
// ours EDI/EBX/EBP/ESI), and retail saves EBX late, giving the NONE path its own epilogue. The forms
// with one actor lookup at the top give index EBP and view EBX as retail, but actor ESI and the state
// EDI, loaded and pushed late. Tried: typed slot data block/current/entry 61/62; on that block actor
// once/at use 66/85; long/word/byte readiness 76/78/77; the readiness stored in the field and tested
// from it 47 (kept, retail's mov eax, 1); on it, one actor lookup at the top 52, plus the state
// declared first 52, a prop index local 52, the typed slot data block 52, node/view declared at the
// top 52, slot cast at each use 52, the last test as if/else-return 52; the NONE case as an early
// store and return 91 (actor EDI as retail, index and state then in scratch registers); getter in
// both NONE test and node lookup 47; actor before the view test 97; actual input/required/done byte
// fields, all subsets, 61 each; completion byte field 61; role names in all index/actor/view subsets
// 47; indexed data block with complete names 47; direct concrete callback input 47/52; native
// success/else-return gates, all subsets 47; native node reference with/without the data block
// 47/47; actor reference after the view 47; entry actor reference variants 52; the slot read once
// through its address 47, with the top actor 52, with the top actor and the state first 52; the
// getter at the 0x262800 call instead of the inner actor local 88 (276 bytes).
// Round 76: retail keeps one actor pointer across the calls (EDI, read at +0x418 at the end); a second
// lookup at the final call reloads the pool after the calls here 71, so the source looks the actor up
// once at the top. That form (52) gives retail's index EBP and view EBX and the shared final store
// (retail duplicates it for the NONE path); left are actor/state ESI/EDI swapped, EBX saved at entry and
// the 0x1f4f40/0x262800 argument temporaries. On it: the slot parameter cast at each use 52; the view
// computed first as NULL-or-view and tested once 52; the NONE case as an early store and return 91.
// Round 198: typed slot data block/current/entry 61/62; on that block actor once/at use 66/85; long/word/byte readiness 76/78/77; stored readiness 47 (exact-match-only, restored). Opening body restored.
// Round 238: actual input/required/done byte fields, all seven subsets, 61 each;
// completion byte field 61. Original Boolean field types and body restored.
// Round 89: the readiness stored in the field and tested from it, kept (retail's mov eax, 1) 47; on
// it, one actor lookup at the top 52 (index EBP and view EBX as retail, actor/state ESI/EDI swapped),
// plus the state declared first 52, a prop index local 52 or the typed slot data block 52; the
// NONE case as an early store and return 91 (actor EDI as retail, more pushes sink).
// Round 173: Role allocation names in all seven index/actor/view subsets 47 each; indexed data block with
// complete names 47. Direct concrete callback input, alone/one actor lookup/role names/both, 47/52/47/52.
// Original body, input declaration and table entry restored; late EBX save and register ownership remain.
// Round 270: native success/else-return gates for active, facing and release phases,
// all seven subsets 47; full gates with the slot data member 47. Original restored.
// Round 252: native node reference with/without the data block 47/47; actor reference
// after the view, alone/with node 47/47; entry actor reference alone/with node/with
// node and data block/with data block 52/52/52/52. Original complete body restored.
// Round 155: on the top actor lookup (index EBP, view EBX as retail; actor ESI/state EDI swapped): node/view
// declared at the top 52, prop-index local 52, slot cast at each use 52, last test as if/else-return 52, NONE
// as an early store and return 91; getter in both NONE test and node lookup 47; actor before the view test 97.
// Round 7 (lane 13): --fast frees this handler's convention (the table is dropped from the reduced image), so
// all trials ran plain. Retail's actor is a local (first load folded, then `add edi, edx`). On the top actor
// form, the completion store written again for the NONE case (if/else) gives retail's late EBX push and own
// NONE epilogue with state ESI and actor EDI, but the index then stays on the stack and the node takes EBX:
// 69; the same with the slot parameter typed `s_slot_23 *` (table cast) 69, node/view declared first 69,
// NONE arm first 91; that store on the current body 97.
// Round 429: complete readiness arms/path-owned completion/both/separate release/
// readiness-release/completion-release/all/all with one actor acquisition
// 104/104/94/47/104/104/94/101 fast; all restored.
// Round 106 (plain): top actor with the unknown0c test folded into the view test 52; the current body as
// single-exit nesting 132 (304 bytes); the top-actor body nested the same way 102 (302 bytes); readiness as
// `... ? true : false` 66; state declared first with both locals const 52.
// Round 238: named initial-completion/facing/release decisions, seven subsets 47 each;
// complete decisions with native slot data block 47. Original callback restored;
// initial one-use completion snapshot rejected without an exact match.
// Round 45 (lane 16): the actor looked up once at the top, the state typed as the slot's data block, a
// guard clause storing the completion for a null view, the completion stored after the final call and
// again in an else for the NONE prop index: 47 -> 7 (kept; retail's registers, both epilogues and the
// late EBX push). Steps: top actor with the NONE store in an else 69, plus state declared first/prop-index
// local/node and view at function scope 69 each; plus the data block 51 (head and NONE epilogue as retail,
// node EBX and view spilled), view renamed 51, view declared first 51, two actor lookups 92, NONE as an
// early store and return 79, view through the index getter after/before the node 74/88, one shared store
// 52; the guard clause 7. On 7: the final call under a positive test 7, `(short)` cast 7, the type byte
// declared signed char 7, a positive test around the 0x1f4f40 call 7.
// Round 23 (lane 15, plain): the type byte in a named char local, tested and passed 11; the 0x262800
// reference read into a local first 7 (its temporary moves EAX -> EDI, retail ECX); the state pointer
// declared before the actor 9; the readiness in a named bool stored and tested 22 (byte setcc); the
// completion store shared after an `if (view)` block 51 (276 bytes). At 0x1afb30's call to 0x1f4f40
// our build converts the byte in place as retail does; here EAX holds the actor index too early.
// Lane 16 r46 (plain): the 0x262800 reference copied into a named local first 7 (its temp moves to EDI).
// Round 213: inline facing callee 7; inline release callee 7; inline node test 7; both call contracts
// inline 7; short facing mode 44; short mode with inline facing callee 44; long facing mode 46; long mode
// with inline facing callee 46; release result used for completion 7; release result with short mode 44;
// release result with long mode 46; original executable source restored.
// Round 59: the facing mode through a field reference/pointer 7 each; the release reference through a field
// reference/pointer 7 each; one actor-index address read at the facing/release call 7 each; the data block
// through its array's address 7; one actor-index reference shared by all three calls 7. All neutral.
// Round 148 (fast): the release reference through a fresh actor getter 57 (286 bytes); the readiness as an
// if/else of two stores 78 (254 bytes); the null-view and already-complete tests merged into one guard 8;
// the facing call in an else of its guard 7; named facing/point pointers for the call 7 (same code); a 600 s
// permuter run (fast, anneal, 80 tries), none below 7.
// @retail 0x1b89d0
void __stdcall function_1b89d0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_23_data *state = (s_slot_23_data *)slot->unknownc;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);

		if (!view)
		{
			state->unknown00 = true;
			return;
		}
		if (state->unknown00)
			return;

		state->unknown00 = state->unknown01 && function_1f86f0(actor_index);
		if (state->unknown00)
		{
			if (!state->unknown0f || !state->unknown11 || state->unknown10)
				return;

			function_1f4f40(actor_index, &state->facing, state->unknown11, &state->point, true);
			state->unknown10 = true;
			state->unknown00 = false;
		}

		if (view->unknown70 != 0 || !function_25da00(node))
			return;

		function_262800(actor_index, actor->unknown418, false);
		state->unknown00 = true;
	}
	else
		state->unknown00 = true;
}

// @retail 0x1b8ae0
void __stdcall function_1b8ae0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));
		s_slot_23_data *state = (s_slot_23_data *)slot->unknownc;

		actor->unknown4a2 = true;
		if (view->unknown06 != 2)
			state->unknown02 = g_510c54->field_2_3;
		else if (state->unknown02 > 0)
			state->unknown02--;

		s_actor_view *current = actor_get(actor_index);
		if (current->unknown50c && current->unknown504 == 1 &&
			!(function_1f8940(actor_index) < 2.0f) && state->unknown02 <= 0)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 0;
		}
		else if (view->unknown70 == 0)
		{
			actor->unknown420 = 2;
			actor->unknown41c = 4;
		}
		else if (view->unknown70 == 1)
		{
			point3f point;

			function_210850(&view->unknown78, &point);
			actor->unknown420 = 3;
			actor->unknown424.point = point;
			actor->unknown41c = 4;
		}

		if (actor->unknown229)
			actor->unknown482 = true;

		s_character_db0_flags *character = (s_character_db0_flags *)function_1e4db0(actor_index);

		if (character && (character->flags & 2) && actor->unknown086 <= 3)
			actor->unknown483 = true;
	}
}

// @retail 0x1b8c60
short __stdcall function_1b8c60(long actor_index, s_slot *slot, bool active)
{
	s_slot_23 *state = (s_slot_23 *)slot;
	short result = g_46fbe8;

	if (state->unknown0c)
		result = g_46fbe4;
	return result;
}

s_slot_handler_2 g_47e908 =
{
	{
		0x23, 2, 0, -2, 0,
		function_1b84a0, function_1b8c60, function_1b8540, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b85a0, function_1b89d0, function_1b8ae0
};

void *function_1e4be0(long actor_index);
bool __stdcall function_1ac610(long actor_index, point3f const *point,
    s_object_marker *markers, short *indices);
short __stdcall function_272af0(s_match_globals *structure, point3f const *point);
struct s_1f4a20_entry;
struct s_1f4a20_source;
bool function_1f4a20(s_1f4a20_entry const *entries, long actor_index, s_reference reference, s_1f4a20_source const *source,
    bool alternate, s_type_c3b527 *point, vector3f *facing,
    signed char *type, bool *available);

// score 113 (was 113): the search tail's three exits now fall to one `return result` (retail's
// reloads); left: frame 0x1634 against 0x162c (the result byte has its own dword, retail packs it at
// 0xf beside has_markers/available at 0xe, so later homes sit 8 higher), the result set with setne
// (retail `mov al, 1` before the branch), 0x25d690 called after the marker call's first pushes, and
// the tail's actor index/reference/slot in EBX/ESI/EDI (retail ESI/EDI/EBX).
// Earlier 147 (was 172): frame 0x162c agrees; the actor offset is in ESI as retail. Left: the result
// is set with setne (retail `mov al, 1` before the branch, `xor al, al` on the missing-prop exit),
// the view/actor-offset/character/actor spill homes rotate (retail 0x14/0x18/0x1c/0x20, ours
// 0x1c/0x14/0x20/0x18), 0x25d690 is called after the first pushes, and in the search tail retail
// keeps the actor index in ESI, the reference in EDI and the slot in EBX with scratch spilled (ours
// scratch EBX, reference ESI, slot EDI, the index reloaded at each use).
// Earlier 172 (was 193): frame 0x162c agrees since round 91; the actor offset (retail ESI), slot
// frame 0x162c agrees since round 91; the actor offset (retail ESI), slot
// (retail rematerialized into EBX) and scratch (retail in node's slot) homes remain.
// Earlier: aligned frame 0x1634 versus 0x162c and actor/slot/scratch homes remain.
// Round 16: retail takes the actor index from its first stack argument into EBX, keeps the
// scaled actor offset in ESI and the actor pointer in EDI and stores them at [esp+0x18] and
// [esp+0x20]; ours keeps the slot index in ESI and spills the actor offset.
// Earlier note (score 212): aligned frame 0x1634 versus 0x162c and actor/slot/scratch homes remain.
// Earlier: frame 0x1634 versus 0x162c; scaled actor offset and
// slot/scratch homes remain, with a 14-byte length deficit.
// Round 198: payload/native input/read-only input/input scope 323 each; clamp 322;
// typed preparation/both scopes 257/257; scopes plus clamp 259; early payload receiver 257;
// ordinary result/receiver with ordinary result 325/325; search outputs first 257.
// Typed payload/native input and preparation scope kept at 257 plain; all follow-ups restored.
// Round 256: native geometry/bounds/marker-reference request fields, all seven subsets,
// and complete fields with movement scope 257 each; request header and body restored.
// Round 171: native search request at current/entry/guard/array declaration boundaries 257 each;
// complete zero aggregates at those boundaries 312/314/312/312. Original request, initialization
// and executable source restored; aligned frame and actor/slot/scratch homes remain.
// Round 213: character/payload/scratch home names and named scalar outputs [257, 257, 257,
// 257]; shared guard/node prop index with original/renamed actor and entry/later result [257,
// 257, 257, 257]. Frame and actor/slot/scratch allocation remain.
// Round 216: late marker default 323; result initialized after actor acquisition 257; shared
// scratch cleanup 257; late marker/result, marker/cleanup, result/cleanup and all 323/323/257/323;
// marker readiness directly from tests 315. Original early marker default and every exit restored;
// frame 0x1634/0x162c and actor/slot/scratch homes remain.
// Round 395: actor at use 303; invalid-search guard 212 (plain kept); complete
// success test 305; guard/actor 300, actor/test 296, guard/test 240 (retail length);
// all three and search-entry availability 305/305. Original actor and success flow kept.
// Round 398: marker-index preparation/call scopes crossed with search-phase ending and movement-input scope
// 212/212/258/258/212/212/258/258; all restored.
// Round 400: Character/distance scope before scratch, entry payload pointer and readonly
// character in masks 1-7, then complete movement scope: 212/212/212/212/212/212/212/212.
// Original lifetimes restored.
// DH round 257: Marker readiness byte/short/long 212/279/283; selection result byte/short/long
// 212/240/243; long-ready with byte-result 283, byte-ready with long-result 243; concrete slot
// input/native payload member 212. All restored.
// Round 429: ordinary result/location-index value/both/actor-reference value/result-reference/
// location-reference/all/direct true with both values 289/214/289/258/294/257/294/294.
// Separate invalid-search graph preserved; every new form restored.
// Round 91: the search's other_actor/unknown and the code using them in their own block (entry and
// scratch outside it) 212 -> 193 (kept: retail reuses node/view/actor-offset slots for them, frame
// 0x162c); the whole search tail in a block 217 (frame 0x15ec); entry declared before scratch 193;
// has_markers cleared right before its test 284; reference copied before its flag 242 (one load,
// 1048 bytes); state declared at the top 193; state in the node block and again before its
// unknown04 test 294; minimum as distance < limit 193.
// Round 45 (lane 16): retail calls 0x25d690 before pushing the marker call's arguments; the node
// state read into a block local first gives that block exactly, 173 (one-use call result, not kept);
// on it: early/late state pointers 294, existing 0x1f86f0 for the moving test 295 (stays a call),
// named marker-list local 173; `return true` at the not-ready exit 288 (1068 bytes, own `mov al, 1`
// epilogue; retail stores the result through AL and that exit returns AL unreloaded); non-volatile
// result 287 (folded); scratch declared in the search block 193; `limit < distance ? limit :
// distance` 191; limit read from the field at each use 189 (retail's select with distance in XMM0);
// with that, the reference read once into a local inside the nested moving test 239 (1048 bytes).
// Retail's `mov al, 1; mov [esp+0xf], al` with the not-ready exit returning AL unreloaded is the shape
// of a plain bool result kept in memory (matched 0x1bcd00 shows it for a result assigned false on one
// path); a plain result assigned false for the missing prop (single exit, body nested in the ready
// test) folds to constants 293; the result's address taken as a no-op 287 (folded too).
// Round 109 (lane 6): 0x1f4a20's fourth argument is the marker list (0x70-byte records, NULL-tested;
// retail passes the slot holding `has_markers ? markers : NULL`), not the scratch buffer: a named
// marker-list local for the request field and that call 225 (1068 bytes = retail's length); kept
// with the actor's +0x418 reference read once into a local for its compare and its store (retail
// loads it once) 172. On 172: the movement limit read at each use 169; `limit < distance ? limit :
// distance` 170; has_markers cleared after the state resets (retail's store order) 273; `return true`
// at the not-ready exit 282 (1069 bytes); the state as `(s_slot_23 *)slot` 172 (same code). Retail
// still keeps the scaled actor index in ESI from entry and reloads `slot` into EBX late; ours keeps
// `slot` in ESI from the 0x1e4be0 call and spills the index.
// Round 17 (m3 lane 2, fast): retail's tail has ESI = actor_index (reloaded for the search) and
// EBX = slot (reloaded at the unknown04 test); ours keeps EBX = actor_index and ESI = slot
// throughout. view renamed fields 172 and node renamed prop_node 172 (same code: the 4-byte spill
// slots node/view and character/offset stay swapped); the node state read into a block local
// before the marker call 153 (that block becomes retail's; one-use call result, not kept), with
// state assigned at the top 153, with the three clears through slot->unknownc and state declared
// at the movement block 285 (1055 bytes); the prop index tested through a fresh actor_get 272.
// Round 25 (fast): the state read from `slot` at every use 172 (CSE'd, same code); `view` renamed
// `fields`, `node` renamed `prop`, `character` renamed `traits` 172 each (same code: names don't move
// the node/view or index/character slots); prop index tested through actor_get before the actor
// pointer 272; has_markers cleared before its test re-measured 273 (aligned 130 vs 133). Retail's
// early state stores go through the `&state->unknown11` temporary ([eax - 2]), so its `slot` is not
// live across the middle. Opening body restored.
// Round 8 (lane 18): the result computed as `actor->prop_index != NONE` and tested for the missing-prop
// exit, in place of the volatile result: 194 (actor offset in ESI, setne), with `if (!result) return
// result` 194, as a ternary 194, both exits in one test 287 (1050 bytes), preset true then false for
// the missing prop 278 (folds); with `has_markers = false` right before its test 147 (kept, plain).
// On 147: three declaration orders of the function-scope locals 147 each; scratch declared in the
// search block 147; the limit select as `distance < limit ? distance : limit` 147, `limit <= distance ?
// limit : distance` 146, `distance > limit ? limit : distance` 145.
// Round 32 (m3 lane 14): matched sibling 0x1b3fd0 gets retail's result byte (`mov al, 1; mov [esp+..], al;
// jne; xor al, al`, a reload at each final epilogue) from `bool result = true; if (... == NONE) result =
// false; else if (...) { body } return result;` with every body path reaching that return. Here: the
// whole body in that shape 182 (result byte, frame 0x162c and the tail's reference in EDI as retail, but
// the actor offset in EAX and the slot held in ESI); only the search tail single exit 113 (kept). On
// 113: wrapped openings (sibling/`result && ready`/`if (!result) return result`/`return false` then
// ready/constant true with `return false`) 182/274/267/267/287; `return result` for the missing prop
// 113; `return true` when not ready 288 (1076 bytes); result declared then assigned 113; sibling opening
// with early returns 287 (folds); scratch declared in the search block 113, with entry 141, entry alone
// 141; available/success declared at the search block's top, available alone, available at function
// scope 113 each; has_markers declared after result or first 113 each (slots unchanged).
// Round 61 (lane 16, fast): the node state read into a block local before the marker call 101 (0x25d690 then
// runs before the pushes as retail; a one-use call result, not kept); the existing function_25d690 called
// directly in the argument (no prop_node_state) 113 (same code). The sibling single exit (`bool result =
// true; if (prop == NONE) result = false; else if (ready) { body } return result;`) 182, with the state local
// 161 (aligned 116 -> 91: frame 0x162c, the result byte at 0xf beside has_markers, retail's `mov al, 1` and
// 0x25d690 order, but the actor offset in EAX spilled where retail keeps it in ESI, and the slot held in
// ESI); on that: the state pointer at the function top, at the ready block's top, or written out at each use
// as `slot->unknownc` 161 each (same code); `result` declared before the actor 175; the two tests nested
// (`if (prop != NONE) { if (ready) {...} } else result = false;`) 277; `else { if (ready) {...} }` 161;
// the direct 0x25d690 call 182.
// Round 477: Comparison-produced entry result signed-byte/byte/short/long on aligned and native search requests 116/116/275/276 each.
// Narrow results fold later returns to constants despite recovering frame 0x162c; original Boolean result, aligned request and complete body restored.
// Round 46 (m3 lane 10, fast): lane 14's sibling single exit rebuilt (`bool result = true; if (prop_index ==
// NONE) result = false; else if (ready) { body } return result;`) 182: frame 0x162c, the result byte at 0xf
// beside has_markers and retail's `mov al, 1; mov [esp+0xf], al; jne; xor al, al` opening, but the actor
// offset in EAX (spilled at 0x20; retail ESI at 0x18) and the view/character homes swapped (ours 0x1c/0x14,
// retail 0x14/0x1c). On it: result declared before the actor 195; the ready test nested inside a
// prop-index test with `else result = false` 277; the body's locals declared at function scope 182, with
// has_markers first 182.
// Round 14 (lane 17, fast): on this body the node state read into a block local before the marker call 101
// (that block becomes retail's; one-use call result, not kept); `has_markers` renamed 113 (same code). The
// whole body in the sibling result shape again 182; on it the node-state local 161 (frame 0x162c, result
// byte, marker block and the tail's EDI reference as retail; left: the actor offset in EAX and the node
// table base in ESI where retail has them the other way round, then the view/offset/character/actor spill
// homes rotate and the slot stays in ESI); on that, the result declared before the actor 175; the tests as
// nested ifs with the false result in an else 277.
// Round 13 (lane 18, fast): retail's frame slots follow reference counts: its result byte (two defs in
// the sibling `bool result = true; if (... == NONE) result = false; else if (ready) { body } return result;`
// shape) sits beside has_markers at 0xe/0xf, so the 113 form's single-def result spills to its own dword
// and the frame grows by 8. The movement limit read at each use gives retail's select block exactly
// (distance in XMM0) 110 (aligned 113 against 116); on the sibling shape (182, aligned 105) it gives 178
// (aligned 101: frame and result byte as retail; the actor offset in EAX, not ESI, and the slot
// parameter enregistered in ESI from the state stores, where retail spills the offset around the view
// branch and reloads the slot into EBX at the unknown04 test); the state pointer formed at entry from the
// parameter 113 / sibling 182 (same code); sibling + limit read + result declared before the actor 191.
// Retail also inlines matched 0x1f86f0 (the 504 test), which sits in an /Ob1 file (a flags lead).
// Round 264: False-first single result, library movement minimum/two-use location index/
// explicit marker-result masks 0..7 281/274/281/274/271/271/271/271. The first restores frame
// 0x162c/result +0xf but regresses; original complete body and request producers restored.
// Round 150: 0x25d690 called directly in the 0x1ac610 argument (no inline wrapper) 113 (same code: the call
// still follows the first two pushes); has_markers typed byte 113 (same code); sibling result shape 182, with
// has_markers byte 182, with the missing-prop path returning false directly 287 (folds); the first two state
// clears as one chained assignment 115. The compile-only harness gives frame 0x162c here but other registers.
// Round 111 (lane 8, fast): the sibling single exit rebuilt 182 (frame 0x162c, result byte at 0xf beside
// has_markers and retail's opening; left: the actor offset in EAX spilled at 0x20 with the actor at 0x18,
// where retail keeps the offset in ESI at 0x18 and the actor at 0x20, and the slot parameter held in ESI);
// on it a `long prop_index` local for the NONE test and the node lookup 182 (same code); the same local on
// the kept form 113 (same code); the kept form's computed result nested as `if (result && ready) { body }`
// with one `return result` 274; `bool result = false;` set true inside a prop-index block that holds the
// ready test 281 (1056 bytes); 0x25d690 called directly in the marker call's argument (no wrapper) 113
// (same code: the call still follows the first two pushes).
// Round 291: result identity before readiness/availability, all readiness/availability/
// selection identity subsets, 113/113/113/113/113/113/113/113. Original body restored.
// Round 133 (m3 lane 6, fast with the file and its callees listed): the node state read into a block local before the
// marker call together with the movement limit read at each use 97 (aligned 116 -> 102; the 0x25d690 call, the marker
// block and the select become retail's, the frame stays 0x1634 with the result byte in its own dword; one-use call
// result, not kept); the sibling single exit with both 156 (aligned 86: frame 0x162c and retail's result byte, but the
// actor offset in EAX and `slot` held in ESI), with the result declared before the actor 170 (aligned 89), the sibling
// exit with the node state alone 161 (aligned 91); `result` renamed `has_prop` (sorts beside has_markers) 113 and with
// both pieces 97 (identical code: names don't move the byte slots).
// Round 222: contiguous three-byte state memset with whole-body result-first,
// movement field reads and native request masks 0..7: 248/249/248/249/248/249/248/249.
// Result-first forms recover frame 0x162c but worsen state-base/register homes.
// Original scalar state resets, result, request, field snapshot and complete body restored.
// @retail 0x1b85a0
bool __stdcall function_1b85a0(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    bool result = actor->prop_index != NONE;
    if (!result)
        return false;
    if (!*((bool *)actor + 0x40))
        return result;

    byte *character;
    s_slot_23_data *state;
    bool has_markers;
    s_object_marker markers[32];
    short marker_indices[32];
    s_2605d0_request request;
    real distance;
    {
        s_prop_node_view *node = prop_node_get(actor->prop_index);
        s_prop_view_fields *view = prop_node_view(node);
        character = (byte *)function_1e4be0(actor_index);
        state = (s_slot_23_data *)slot->unknownc;
        state->unknown0f = false;
        state->unknown10 = false;
        state->unknown11 = 0;
        has_markers = false;
        if (!*((bool *)actor + 0x3c) && actor->unknown227)
            has_markers = function_1ac610(actor_index, &prop_node_state(node)->position,
                markers, marker_indices);

        memset(&request, 0, sizeof(request));
        request.type = 3;
        if (view && view->unknown70 == 1)
        {
            function_210850(&view->unknown78, (point3f *)((byte *)&request + 0x24));
            short sector = *(short *)((byte *)view + 0x84) == NONE ?
                *(short *)((byte *)view + 0x72) :
                function_272af0(g_4e0348, (point3f *)((byte *)&request + 0x24));
            *((bool *)&request + 0x20) = true;
            *(s_type_c3b527 *)((byte *)&request + 0x30) = view->unknown78;
            *(long *)((byte *)&request + 0x40) = *(long *)((byte *)view + 0x74);
            *(short *)((byte *)&request + 0x44) = sector;
            distance = function_210ac0(&view->unknown78, &actor->position);
        }
        else
        {
            *((bool *)&request + 0x52) = false;
            distance = node->unknown28;
        }
    }
    s_character_db0_flags const *movement = (s_character_db0_flags const *)function_1e4db0(actor_index);
    long minimum;
    if (movement)
    {
        real limit = movement->unknown0c;
        minimum = (long)(limit > distance ? distance : limit);
        *(long *)((byte *)&request + 0x684) = minimum;
        *(long *)((byte *)&request + 0x688) = (long)movement->unknown10;
    }
    else
    {
        minimum = 0;
        *(long *)((byte *)&request + 0x684) = 0;
        *(long *)((byte *)&request + 0x688) = 100;
    }
    *(real *)((byte *)&request + 0x48) = 2.0f;
    *((bool *)&request + 0x46) = true;
    *(real *)((byte *)&request + 0x4c) = (real)minimum;
    *((bool *)&request + 0x50) = true;
    *((bool *)&request + 0x59) = character && (*character & 1);
    s_object_marker *request_markers = has_markers ? markers : NULL;
    *(s_object_marker **)((byte *)&request + 0x64) = request_markers;
    if (state->unknown04 != 0.0f)
        *(real *)((byte *)&request + 0x1c) = state->unknown04;
    if (actor->unknown50c && actor_get(actor_index)->unknown504 != 2)
    {
        s_reference actor_reference = actor->unknown418;
        if (!REFERENCE_EQUAL(actor_reference, g_470fa0))
        {
            *((bool *)&request + 0x692) = true;
            *(s_reference *)((byte *)&request + 0x694) = actor_reference;
        }
    }

    byte *scratch = ai_scratch_buffer_get();
    s_261d20_entry entry;
    {
        long other_actor;
        bool unknown;
        s_reference reference = function_261280((s_prop_search *)&request, actor_index,
            &entry, &other_actor, scratch, &unknown);
        if (REFERENCE_EQUAL(reference, g_470fa0) || *(short *)((byte *)&entry + 8) > 1)
        {
            state->unknown00 = true;
            g_46eeb8[0x23]->unknown8 = g_46f348;
            ai_scratch_buffer_release(scratch);
        }
        else
        {
            s_262b40_result *location = function_262b40(reference);
            bool available = false;
            bool success = false;
            if (*(short *)((byte *)&entry + 0x5c) != 0 &&
                *(long *)((byte *)location + 0x14) != NONE &&
                *(long *)((byte *)location + 0x14) != 0xffff)
            {
                success = function_1f4a20((s_1f4a20_entry const *)&entry, actor_index, reference,
                    (s_1f4a20_source const *)request_markers, true, &state->point, &state->facing,
                    (signed char *)&state->unknown11, &available);
                if (success)
                {
                    state->unknown0f = true;
                    state->unknown10 = false;
                }
                else if (available)
                    reference = g_470fa0;
            }
            if (!available && !success)
                reference = function_2626b0(actor_index, reference, other_actor, scratch, unknown, true);
            if (!REFERENCE_EQUAL(reference, g_470fa0))
            {
                state->unknown01 = true;
                actor->unknown4ae = true;
                ai_scratch_buffer_release(scratch);
            }
            else
            {
                state->unknown00 = true;
                g_46eeb8[0x23]->unknown8 = g_46f348;
                ai_scratch_buffer_release(scratch);
            }
        }
    }
    return result;
}
