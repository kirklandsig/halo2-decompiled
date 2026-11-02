#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_markers.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include "unknown_2551c0.h"
#include "swarm.h"
#include "normalize3d.h"
#include <math.h>
#include <stddef.h>

// @flags /O2 /Gr /arch:SSE

/* the placement a spawned creature starts from (filled by its constructor, 0x200e60) */
struct s_actor_position_state
{
	s_actor_position_state();
	long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real facing;
	real field18;
	long field1c;
	byte unknown20[0xe];
	short field2e;
	byte unknown30[0x64 - 0x30];
};
typedef char assert_256bd0_placement_size[(sizeof(s_actor_position_state) == 0x64) ? 1 : -1];
typedef char assert_256bd0_placement_facing[(offsetof(s_actor_position_state, facing) == 0x14) ? 1 : -1];
typedef char assert_256bd0_placement_field2e[(offsetof(s_actor_position_state, field2e) == 0x2e) ? 1 : -1];
struct s_28dab2;
struct s_203780;
extern short g_470c40, g_470c44;
extern point2f *g_468778;
real function_30bf0(vector3f *vector);
real function_11cc90(vector2f const *a, vector2f const *b);
long function_1e4990(long index);
void swarm_iterator_new(s_290c50_iterator *iterator);
long function_28dbd0(long character_index, long perception_index, long definition_index, s_28dab2 const *placement, bool flag);
long __stdcall function_1e0850(long character_index, long squad_index, s_203780 const *placement, bool substitute);
bool function_1e11b0(long actor_index, bool active);
void __stdcall function_b77d0(long object_index, vector3f const *linear, vector3f const *angular);
void swarm_creatures_set_object_flag(long perception_index, bool flag);
void function_b7360(long object_index);
real distance_sq3f(point3f const *a, point3f const *b);

static inline byte *perception_iterator_next_256bd0(s_290c50_iterator *iterator)
{
	byte *entry = NULL;
	s_data_datum_iterator *records = (s_data_datum_iterator *)iterator;
	if (data_datum_iterator_next(records))
		entry = records->datum;
	iterator->object_index = records->datum_index;
	return entry;
}

/* the flags word at +0x10a of an object */
struct s_object_flags_256bd0
{
	word unknown0 : 6;
	word bit6 : 1;
};
// score 179 (was 201): the activity test is the loop's condition, so the frame is 0x110 and the
// activity byte is re-read at every pass as retail does; the first block's register homes differ.
// Round 34: the activity test moved into the while condition and the step helper no longer reads
// the global (it steps the iterator and copies its index): retail's frame 0x110 at 179/1074, the
// loop reloads the byte and takes retail's cmovne; the first block's register homes remain.
// Round 34 (lane A, machine 3): an early return of the initial g_470c44 (to leave EAX free for
// the b8d30 argument, as retail does) gives 277 with a 0x118 frame, and the returned global and
// the grouped case forms regress the same way, so the loop-condition form is kept.
// Round 15: shared iterator declaration 205; base-record view 208; conditional entry selection 263;
// existing point-distance helper 204; variant field reread 207; character field reread 202;
// both fields reread 281; character reread and base-record view on the helper graph 201 (kept).
// Earlier: frame 0x114 versus 0x110: our build hoists the ai globals' activity byte out of
// the iterator loop and spills it to a stack byte, where retail re-reads the globals pointer and its byte
// after every 0x6b380 call (the reload a program-wide exposed global address gives); every stack offset
// then differs by four. Also: the 0x28dbd0 call passes the character in EAX where retail passes the
// variant (BLOCKED-BY 0x28dbd0); retail selects the entry with cmovne; actor/point/selection homes.
// Frame 0x114/0x110, cached activity and actor/geometry/selection homes remain.
// Earlier baseline: Frame 0x114 versus 0x110, cached activity and geometry/actor homes remain.
// frame 0x114 versus 0x110, cached activity and actor/geometry homes remain.
// Earlier: frame 0x114 versus 0x110: our build hoists the swarm-activity byte out of
// frame 0x114 versus 0x110: our build hoists the swarm-activity byte out of
// the iterator loop into a stack byte (back edge to its reload) where retail re-reads the ai
// globals each pass; the 0x28dbd0 call passes the character in eax where retail passes the
// variant (BLOCKED-BY 0x28dbd0 for that call); retail selects the entry with cmovne.
// Native placement fields 213; actor getter 212; slot reference 211; native fields with getter
// 214; const entry result 211; condition-local entry 211; candidate skip 211; placement scope
// 211; marker position workspace 235; placement position reads 253; in-place velocity 212;
// marker/placement 225; marker/velocity 235; placement/velocity 253; all three 225; angle field
// before sign adjustment 230; helper state pointer in two orders 211/211; state reference
// 211/211; activity value 211/211; external activity reference 211/211; actual datum cursor
// with separate index output: pointer, void result, read-only result and reference output, each
// in both orders, 211. Round 72: the signed angle as a call to the existing 0x11cc90 (inlined
// as retail) 206, kept, with the placement written through named fields; entry selected by a
// ternary 263; the iterator step as a plain function in swarm.cpp 206 (still hoisted).
// Frame 0x114 versus 0x110, cached activity byte and actor/point/selection homes remain.
// Earlier: frame 0x114 versus 0x110 and hoisted active byte remain;
// native placement fields 213; actor getter 212.
// slot reference 211; native fields with getter 214.
// const entry result 211; condition-local entry 211.
// candidate skip 211; placement scope 211.
// frame 0x114 versus 0x110, activity caching and saved homes remain;
// marker position workspace 235; placement position reads 253; in-place velocity 212;
// marker/placement 225; marker/velocity 235; placement/velocity 253; all three 225;
// angle field before sign adjustment 230.
// Helper state pointer in two orders 211/211; state reference 211/211;
// activity value 211/211; external activity reference 211/211. Cached byte/frame remain.
// frame 0x114 versus 0x110 and cached activity byte remain.
// Actual datum cursor with separate index output: pointer in both orders, void result
// in both orders, read-only result in both orders, reference output in both orders all 211.
// Shared native datum result/position value/read-only result/read-only position value
// 211/260/211/260; caller-only native result/position value 211/260;
// complete native/read-only datum values 257/257.
// Earlier: frame 0x114 versus 0x110 and cached activity byte remain.
// Round 130: CRT position/direction/both copies 211/211/211; placement
// copy alone/with position/with direction/all copies 211/211/211/211; all copies
// with reused actor offset 254. Original copies and receiver are restored.
// Round 327: native selected/object/variant/character handles 211/211/211/211; selected/object and variant/character pairs
// 211/211; all four handles 211; all with double minimum comparison 306.
// Round 180: Shared point setters for source/placement/both 310/292/318; native direction
// initializer 251; with source/placement/both setters 275/310/288; selected point value 279.
// Round 344: unsigned-word status 206; per-arm object result 206; per-arm status 272;
// word status with object/status/both arms 206/272/272; both arms with signed status 272;
// full unsigned status with both arms 275. All creation-call bindings preserved.
// Round 243: Native actor/spawned-actor/unit metadata singly 206 each; all pairings 206
// each; complete native and read-only native records 206/206. Original body, iterator
// helper and includes restored.
// Earlier: frame 0x114 versus 0x110: our build hoists the ai globals' active byte out
// frame 0x114/0x110 from the hoisted ai-active byte (cause below), then
// scratch-register rotation, the direction copy store order and the angle helper operand pairing.
// Frame 0x114 versus 0x110: our build hoists the ai globals' active byte out
// of the iterator loop into a stack byte, where retail re-reads it after every 0x6b380 call (its
// compiler treats that call as possibly writing it; src/unknown_2003a0.cpp reaches the same reload
// only through a volatile read, which is no longer allowed); every stack offset then differs by 4.
// Also: the 0x28dbd0 call passes the character in EAX where retail passes the variant (BLOCKED-BY
// 0x28dbd0); retail selects the entry with cmovne; actor/point/selection register homes.
// Tried: native placement fields 213; actor getter 212; slot reference 211; native fields with getter
// 214; const entry result 211; condition-local entry 211; candidate skip 211; placement scope 211;
// before sign adjustment 230; helper state pointer in two orders 211/211; state reference 211/211;
// activity value 211/211; external activity reference 211/211; actual datum cursor with separate
// index output (pointer, void result, read-only result, reference output, each in both orders) 211;
// shared native datum result/position value/read-only result/read-only position value
// 211/260/211/260; caller-only native result/position value 211/260; complete native/read-only datum
// values 257/257; the signed angle as a call to the existing 0x11cc90 206 (kept), with the placement
// written through named fields; entry selected by a ternary 263; the iterator step as a plain
// function in swarm.cpp 206 (still hoisted); CRT position/direction/both copies 211/211/211;
// placement copy alone/with position/with direction/all copies 211 each; all copies with reused
// actor offset 254; native selected/object/variant/character handles 211 each, the pairs 211, all
// four 211, all with double minimum comparison 306; native actor/spawned-actor/unit metadata 206
// each, all pairings 206, complete native and read-only native records 206/206. Round 95: the step's
// two tests nested 206; the velocity through the existing vector3f_scale 206 (identical code, kept);
// the step selecting the entry with a ternary inside the active test 263 (frame 0x110 through a
// branchless select, the active byte still spilled); the next result in a named bool 206; the loop
// distance through vector3d_from_points3d and length_sq3f 208.
// Round 180: Shared point setters for source/placement/both 310/292/318; native direction
// initializer 251; with source/placement/both setters 275/310/288; selected point value 279.
// Round 344: unsigned-word status 206; per-arm object result 206; per-arm status 272;
// word status with object/status/both arms 206/272/272; both arms with signed status 272;
// full unsigned status with both arms 275. All creation-call bindings preserved.
// Round 153: Character/variant/both native admission exits 206/206/206; variant/character/both
// fields at every use 205/202/273; both exits with variant/both fields 205/273. On the fresh
// character-field graph, nested step tests/native entry select/shared displacement 202/274/202.
// Four-point 202 rejected; body/helper restored.
// Round 187: placement angle first 253; angle after position 249; angle last 251;
// constants first 250; descending fields 243; angle then descending fields 269;
// short stores first 215; position then descending constants 251. Original order restored.
// Displacement producer orders xyz/xzy/yzx/zxy/zyx 206 each; all three named squares
// after the deltas/interleaved with each delta 205/205; successive z-y-x additions 208.
// One-count forms rejected; original arithmetic and helper restored.
// Round 63: object/selected handles declared the other way 205; result read after the timer pointer 205;
// minimum declared before the flags query 210; entry position through a named point pointer 205; the
// variant creation and the spawn as if/else after the flags block 208.
// Round 102: the cause of the hoist: our link proves the iterator's address never escapes, so the
// iterator stores cannot reach g_4f55d0->active; retail reloads it each pass. A diagnostic
// `s_290c50_iterator *const *` read of the step helper's parameter (stores &iterator) gives
// retail's frame 0x110 and loop: 187 fast (aligned 112 -> 67), not kept. The object index copied
// only while active (retail skips the copy when inactive) 209; with the diagnostic 187; the step
// helper forceinline 205; both 209.
// Round 27: initializer inline/noinline [201,201]; those boundaries
// with ordinary/inline/noinline datum step [217,201,217,194,201,194].
// Noinline initializer plus ordinary step reaches frame 0x110 at 194/1070;
// seven-point form restored. Original declarations, bodies and calls restored;
// activity spill remains.
// Round 245: overlap-safe selected-position/direction/both transfers 280/256/279;
// placement transfer alone/with position/with direction/all three 280/282/283/283;
// fallback-direction transfer alone 208. Original copies and iterator restored.
// Round 208: actual entry-result output through pointer/reference, each formal order and bool/byte
// success, 201 each. All original step and caller contracts restored; frame 0x114/0x110, cached
// activity and geometry/selection homes remain.
// Round 377: real datum-member storage, byte/native entry 201/201;
// separate actual cursor/index outputs 202/202; conditional-entry byte/native
// and split outputs 274/274/274/274. All helper/header/body changes restored; activity spill remains.
// Round 211: Placement constructed before marker/after position/after direction 219/210/210; each
// with a separate refreshed actor 219/210/210; refreshed actor alone 201; early
// placement/refreshed actor/shared displacement 237. Original constructor, receiver, calls and
// iterator restored; activity spill and native geometry homes remain.
// Live tag payload reference 219, with spawned/perception/both shared getters 225/219/225; live
// tag-instance record 218, with spawned/perception/both getters 226/218/226. Activity spill and
// native geometry/selection homes remain; original receivers and call values restored.
// Round 171: noinline initializer/ordinary datum step with minimum/selection/both lifetimes
// 195/195/196; later object result 194, with minimum/selection/both 195/195/196; complete graph
// with early uninitialized cursor 196. Frame 0x110 recovered at 1070 bytes, but the best seven-point
// gain is below threshold. Original declarations, lifetimes, helper and complete body restored.
// Round 238: actual slot countdown/unit tag link/variant tag field declarations,
// all seven subsets 201 each; all three with read-only tag receivers 201.
// Original slot, tag views, field observations and iterator restored.
// Round 216: real iterator zero aggregate/full aggregate/CRT clear/index defaults
// 243/244/242/231; before the flags lookup 247/253/246/223. Original iterator input,
// initializer and private step restored; frame 0x114/0x110, cached activity and homes remain.
// Round 72: active test outside with the next result named, index copied, then the entry
// selected 288 (frame 0x110 but the entry block reorders); select then copy inside the active
// test 201; iterator before the minimum 205; reversed minimum compare 201; selection stored
// before the minimum 201.
// Full native position/direction/placement copies on noinline-initializer/ordinary-datum
// boundaries, all seven subsets 194 each; all copies with selected-before-minimum publication
// 194. Frame 0x110 recovered, but seven-point gains rejected; all declarations and bodies restored.
// Round 72 (lane 8): retail's step tests the activity byte on its own and assigns the entry inside
// it (`if (active) { if (next) entry = datum; object_index = datum_index; }`): that gives retail's
// `cmovne ebp` and the index store ahead of the exit test (loop body as retail, 1075 B) but stays 201,
// since the hoisted activity byte still adds the frame word; with the entry as a ternary 279; on
// that shape `distance < minimum`, a forceinline step and the iterator declared first 201 each.
// The hoist is not ours to fix: the matched 0x268510 reloads the same global only because its loop
// calls an out-of-line function; here 0x6b380 is the only call.
// Round 191: out-of-line initializer/ordinary datum declarations with nested/branch-result/
// active-ternary/combined-ternary entries, byte/native records each 194/262/290/262.
// Nested forms recover frame 0x110 at 1070 bytes but gain seven only; all declarations,
// iterator body, native receivers and complete function restored.
// Round 211: complete iterator reference input with byte/native entry, default/complete test
// result and original/recovered initializer-datum boundaries, scores
// 201/262/201/262/194/262/194/262. Original executable form restored.
// Round 230: actual timer-field reference, native twice-used actor ordinal and exact double minimum,
// seven subsets; all with word status, scores [179, 287, 287, 302, 302, 284, 284, 284].
// Original executable source restored. Initial marker-argument and position/direction homes remain.
// @retail 0x256bd0
short __stdcall function_256bd0(long actor_index, s_slot *slot, bool active)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	short result = g_470c44;
	short *timer = (short *)((byte *)slot + 0xc);
	--*timer;
	if (*timer <= 0)
	{
		s_object_marker marker;
		point3f position;
		if (function_b8d30(*(long *)(actor + 0x18), 0xf0005b4, &marker, 1, false) > 0)
			position = marker.matrix.position;
		else
			position = *(point3f *)(actor + 0x22c);
		vector3f direction = *(vector3f *)(actor + 0x290);
		if (direction.k != 0.0f)
		{
			direction.k = 0.0f;
			if (function_30bf0(&direction) == g_45dbd8)
				direction = *g_4687a8;
		}
		s_actor_position_state placement;
		placement.position = position;
		placement.field10 = NONE;
		placement.facing = function_11cc90((vector2f const *)g_468778, (vector2f const *)&direction);
		placement.field18 = 0.0f;
		placement.field1c = 4;
		placement.field2e = 1;
		byte *unit = *(byte **)(g_4e0300->data + (*(long *)(actor + 0x18) & 0xffff) * 12 + 8);
		byte *definition = g_4e3b44[*(long *)unit & 0xffff].bytes;
		if (*(long *)(definition + 0x30c) != NONE)
		{
			long variant = *(long *)(g_4e3b44[*(long *)(definition + 0x30c) & 0xffff].bytes + 0x18);
			if (variant != NONE)
			{
				long object_index = NONE;
				long selected = NONE;
				byte *flags = (byte *)function_1e4990(*(long *)(definition + 0x30c));
				if (flags && (flags[0] & 1))
				{
					real minimum = 25.0f;
					s_290c50_iterator iterator;
					swarm_iterator_new(&iterator);
					byte *entry;
					while (g_4f55d0->active && (entry = perception_iterator_next_256bd0(&iterator)) != NULL)
					{
						real distance = distance_sq3f((point3f const *)(entry + 8), &position);
						if (minimum > distance)
						{
							minimum = distance;
							selected = iterator.object_index;
						}
					}
					if (selected != NONE)
						object_index = function_28dbd0(variant, selected, *(long *)(definition + 0x30c), (s_28dab2 const *)&placement, false);
				}
				if (selected == NONE)
				{
					long spawned = function_1e0850(*(long *)(definition + 0x30c), *(long *)(actor + 0x30), (s_203780 const *)&placement, false);
					if (spawned != NONE)
					{
						byte *other = g_4f55f0->data + (spawned & 0xffff) * 0x888;
						if (*(long *)(actor + 0x30) == NONE)
							function_1e11b0(spawned, true);
						long perception = *(long *)(other + 0x1c);
						if (perception != NONE)
							object_index = *(long *)(g_5044c8->data + (perception & 0xffff) * 0x34 + 0x18);
					}
				}
				if (object_index != NONE)
				{
					vector3f velocity;
					vector3f_scale(&direction, 0.03f, &velocity);
					function_b77d0(object_index, &velocity, 0);
				}
			}
		}
		actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
		if (actor[7])
		{
			long perception = *(long *)(actor + 0x1c);
			if (perception != NONE)
				swarm_creatures_set_object_flag(perception, true);
		}
		else
		{
			long unit_index = *(long *)(actor + 0x18);
			s_object_flags_256bd0 *flags = (s_object_flags_256bd0 *)(*(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8) + 0x10a);
			flags->bit6 = true;
			function_b7360(*(long *)(actor + 0x18));
		}
		result = g_470c40;
	}
	return result;
}
