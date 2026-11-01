// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FB350.CPP: the recorded animations of objects (entries 55.. of the
   subsystem table at 0x441594) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1fb360.h"

s_record_pool *g_4f5724;

// @retail 0x1fb2f0
void function_1fb2f0(void)
{
	g_4f5724 = data_new_inlined("recorded animations", 1, sizeof(s_recorded_animation), 0, g_510c2c);
}

// @retail 0x1fb330
void function_1fb330(void)
{
	g_4f5724->valid = true;
	record_pool_release_all(g_4f5724);
}

// @retail 0x1fb350
void function_1fb350(void)
{
	g_4f5724->valid = false;
}

/* whether a recorded animation that has not finished plays on the object */
PRIVATE __forceinline byte *function_1fb762(s_record_pool_iterator *arg_0);

// score 12 (was 12): ours keeps the global pointer in ESI, retail keeps it in EDI.
// Round 34: early result return 54, also with one parameter address read 54; assignment-return 54;
// read-only or unsigned parameter 12 each; read-only record pointer 12; condition-scoped record 12;
// predicate-assigned result 29.
// Round 15: walk record {animation, iterator} 12; result declared after the iterator setup 13;
// body moved after its helpers 12; two continue tests 12; step result initialized first 12.
// Compile-only screens reproduce ours exactly; declaration/setup orders, nested tests, swapped
// compare, goto exit, stdcall, a pool local, pool read at each use in the step all keep ESI;
// only an object index held in EBX from entry (0x1fb760's case) moves the pool to EDI.
// Round 139: the step through the non-volatile shared iterator 41 (pool to EBX); the loop
// condition `!result && step` with the result assigned from the test 29; the result set from the
// flag inside the object test 30; the object index read through its address 12; the iterator
// aggregate-initialised 13; a 600 s permuter run (61 tries), none below 12.
// @retail 0x1fb6f0
bool recorded_animation_playing(long object_index)
{
	s_record_pool_iterator iterator;
	s_recorded_animation *animation;
	bool result = false;

	iterator.data = g_4f5724;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((animation = (s_recorded_animation *)function_1fb762(&iterator)) != 0)
	{
		if (animation->object_index == object_index && !(animation->flags & 1))
		{
			result = true;
			break;
		}
	}

	return result;
}

/* the recorded animation of an object, and its datum index */
PRIVATE __forceinline long function_1fb761(s_record_pool *arg_0, long arg_1)
{
	long local_0 = NONE;
	if (arg_1 >= 0)
	{
		for (; arg_1 < *(long const volatile *)&arg_0->high_water_index; arg_1++)
		{
			if (arg_0->bitmap[arg_1 >> 5] & (1 << (arg_1 & 0x1f)))
			{
				local_0 = arg_1;
				break;
			}
		}
	}
	return local_0;
}

PRIVATE __forceinline byte *function_1fb762(s_record_pool_iterator *arg_0)
{
	s_record_pool *local_0 = arg_0->data;
	long local_1 = function_1fb761(local_0, arg_0->index + 1);
	byte *local_2;
	if (local_1 != NONE)
	{
		local_2 = local_0->data + local_0->size * local_1;
		arg_0->index = local_1;
		arg_0->datum_index = (*(short *)local_2 << 16) | local_1;
	}
	else
	{
		arg_0->index = local_0->maximum_count;
		arg_0->datum_index = NONE;
		local_2 = 0;
	}
	return local_2;
}

// @retail 0x1fb760
s_recorded_animation *recorded_animation_find(long *datum_index, long object_index)
{
	s_record_pool_iterator iterator;
	s_recorded_animation *animation;
	s_recorded_animation *result = 0;
	long found_index = NONE;

	iterator.data = g_4f5724;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((animation = (s_recorded_animation *)function_1fb762(&iterator)) != 0)
	{
		if (animation->object_index == object_index)
		{
			result = animation;
			found_index = iterator.datum_index;
			break;
		}
	}

	if (datum_index)
		*datum_index = found_index;

	return result;
}

/* the frames (at 30 a second) the object's recorded animation has played */
// @retail 0x1fb4c0
long recorded_animation_get_frames(long object_index)
{
	real frames;
	s_recorded_animation *animation = recorded_animation_find(0, object_index);
	long result = 0;

	if (animation && animation->object_index == object_index)
	{
		frames = (real)animation->ticks * g_510c54->rate * 30.0f;

		long local_0;
		__asm
		{
			fld frames
			fistp local_0
		}
		result = local_0;
	}

	return result;
}

struct s_type_339e8b;
struct playback_unit_control_view;

/* the event-stream codecs (unknown_29ed40.cpp and
   unknown_29f3e0.cpp) */
void __stdcall function_29f080(s_type_339e8b *controller,
	playback_unit_control_view *control, byte const **cursor, byte version);
bool __stdcall function_29f0c0(s_type_339e8b *controller,
	playback_unit_control_view *control, long *remaining_ticks, byte const **cursor);
void __stdcall function_29f3e0(s_type_339e8b *controller,
	playback_unit_control_view *control, byte const **cursor, byte version);
bool __stdcall function_29f400(s_type_339e8b *controller,
	playback_unit_control_view *control, long *remaining_ticks, byte const **cursor);

struct s_recorded_animation_version
{
	void (__stdcall *initialize)(s_type_339e8b *controller,
		playback_unit_control_view *control, byte const **cursor, byte version);
	bool (__stdcall *apply)(s_type_339e8b *controller,
		playback_unit_control_view *control, long *remaining_ticks, byte const **cursor);
};

/* retail's g_46fd54 and g_46fd4c: unknown_29f3e0.cpp and
   unknown_29ed40.cpp define these pairs as const objects with
   internal linkage, so this file keeps its own copies until they are shared */
static s_recorded_animation_version const recorded_animation_version_v1 =
{
	function_29f3e0,
	function_29f400
};

static s_recorded_animation_version const recorded_animation_version_current =
{
	function_29f080,
	function_29f0c0
};

/* the codecs of each recording version (1..4) */
s_recorded_animation_version const *const g_46fd5c[4] =
{
	&recorded_animation_version_v1,
	&recorded_animation_version_v1,
	&recorded_animation_version_v1,
	&recorded_animation_version_current
};

/* a cutscene recording of the scenario (0x34 bytes) */
struct s_scenario_recorded_animation
{
	byte unknown00[0x20];
	byte version;
	byte unknown21;
	byte control_version;
	byte unknown23;
	short length_ticks;
	byte unknown26[0x30 - 0x26];
	byte const *field_30_2;
};

struct s_scenario_recorded_animations_view
{
	byte unknown000[0x110];
	long recorded_animation_count;
	s_scenario_recorded_animation *field_114;
};

struct s_recorded_animation_object
{
	byte unknown000[0x134];
	dword flags;
};

struct s_recorded_animation_object_header
{
	byte unknown00[8];
	s_recorded_animation_object *object;
};

#define RECORDED_ANIMATION_OBJECT(index) (((s_recorded_animation_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

bool function_cbf40(long unit_index);
void __stdcall function_cbf60(long unit_index, bool active);

inline void recorded_animation_object_set_flag(long object_index, long bit, bool value)
{
	dword *flags = &RECORDED_ANIMATION_OBJECT(object_index)->flags;
	*flags = value ? (*flags | FLAG(bit)) : (*flags & ~FLAG(bit));
}

// @retail 0x1fb360
bool function_1fb360(long unit_index, short recording_index, long flags)
{
	bool result = false;

	if (unit_index != NONE && recording_index != NONE)
	{
		s_scenario_recorded_animations_view *scenario = (s_scenario_recorded_animations_view *)g_4e0350;

		if (recording_index < scenario->recorded_animation_count)
		{
			long datum_index;
			s_recorded_animation *animation = recorded_animation_find(&datum_index, unit_index);
			s_scenario_recorded_animation *recording = &scenario->field_114[recording_index];

			if (!recorded_animation_playing(unit_index))
			{
				if (!animation)
				{
					long index = record_pool_allocate(g_4f5724);

					if (index != NONE)
						animation = &((s_recorded_animation *)g_4f5724->data)[index & 0xffff];
				}

				if (animation)
				{
					animation->object_index = unit_index;
					animation->remaining_ticks = 0;
					animation->ticks = recording->length_ticks;
					animation->cursor = recording->field_30_2;
					animation->version = recording->version - 1;
					animation->flags &= ~1;
					g_46fd5c[animation->version]->initialize((s_type_339e8b *)animation->controller,
						(playback_unit_control_view *)animation->control, &animation->cursor, recording->control_version);
					function_cbf60(unit_index, true);
					SET_FLAG(animation->flags, 2, function_cbf40(unit_index));
					long object_offset = (unit_index & 0xffff) * sizeof(s_recorded_animation_object_header);
					dword *object_flags = &((s_recorded_animation_object_header *)(g_4e0300->data + object_offset))->object->flags;
					*object_flags &= ~1;
					object_flags = &((s_recorded_animation_object_header *)(g_4e0300->data + object_offset))->object->flags;
					*object_flags |= 1 << 22;
					animation->flags |= (word)flags;
					result = true;
				}
			}
			goto done;
		}
	}

done:
	return result;
}

struct s_recorded_animation_object_datum
{
	short salt;
	byte unknown02;
	byte type;
	byte unknown04[4];
	s_recorded_animation_object *object;
};

/* the object, if it is a unit (types 0 and 1) */
static inline s_recorded_animation_object *recorded_animation_unit_try_and_get(long object_index)
{
	s_recorded_animation_object_datum *datum = (s_recorded_animation_object_datum *)datum_get_inlined(g_4e0300, object_index);
	s_recorded_animation_object *result = 0;

	if (datum && ((1 << datum->type) & 3))
		result = datum->object;

	return result;
}

bool __stdcall function_beb30(long object_index);
void __stdcall function_b8540(long a);
void function_c6de0(long object_index, void *control);
void function_f0fd0(long vehicle_index, bool set);

/* advances every recorded animation by a tick; finished ones restore the
   unit's flags and are deleted */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// Round 17: a _ReadWriteBarrier() between the datum lookup and the null test makes MSVC materialise
// retail's 0-or-pointer phi, and a second one between iterator.index = index and the object-index
// load keeps the object load after the store; with both the found path is instruction for instruction
// retail's and the function measures 473 bytes, retail's length, but the second barrier makes MSVC
// hoist g_4e0300 into EBP and wrecks the prologue (139). Without it the best surviving form is the
// committed body with the datum index computed from the element's salt instead of the shared helper:
// 99, a gain of 2, below the threshold, so the body was restored. The tail's element expression
// (pool->size * index + pool->data), the order of the store and the datum index, and the object
// index in a local are all insensitive (99, 99, 164, 99). What is left is the register allocation:
// retail keeps the datum index in EDI, the element base in EBP and the object pool in EBX, and its
// body is 7 bytes longer than ours, so the internal jump displacements differ as well.
// Not kept (101, no match): the iterator step written out in the body (data_find_index, the
// element and datum index by hand, a four-field datum iterator), a hand copy of function_1fb762.
// With it the search matches retail byte for byte (the shared data_find_index scan
// snapshots the high water in ESI and the four-field datum iterator gives retail's 0x14 frame and its
// two pool homes). What still differs is register allocation: retail keeps the datum index in EDI and
// the element base in EBP, ours keeps the datum index in EBX and the element base in ECX, so the
// object-pool load lands in EBP instead of EBX and the unit mask in EAX instead of EDX.
// score 122 (was 122): our frame is eight bytes smaller and the base pointer is kept in EDI where
// retail keeps it in ECX. Retail iterates with the four-field datum iterator (data and index stored,
// the datum index kept in EDI), snapshots the high water per search and computes the element from a
// second spilled copy of the pool. Shared step with/without the barrier 162/152; datum iterator 152
// (frame 0x10); plus a pool local for the iterator and release 150 (frame 0x14); iterator from the
// global, local only for the release 127; that local assigned after the iterator 124 (retail's
// prologue; retail reads the copy for the element and releases through the global).
// Round 406: shared absolute element getter, ordinary three/four-field iterator,
// early/late saved pool and shared datum-index producer 166/157/166/157/163/156/166/157.
// Four-field early pool reaches frame 0x14; getter checks and later allocation regress.
// All unqualified/barrier-free trials restored.
// Round 14: ours also keeps g_4e0300 in EBP across the loop (reloaded after each call) where
// retail re-reads it at each use, and passes function_f0fd0's flag in DL where retail uses CL
// (BLOCKED-BY 0xf0fd0 for that call). Barrier removed alone 161; finished arm first 149; if/else
// finished flag 123 (same code); early continue to the release 162; deletion test in one
// condition 127; the codec pairs as writable globals (diagnostic) change nothing.
// DL192: Native word flag views for admission/publication/cleanup subsets 1-7 on ordinary traversal:
// 152/154/154/156/161/160/160; complete view with native four-field iterator 160. All target changes and the
// external flag declaration restored; pool homes remain.
// Round 298: ordinary shared traversal without the target barrier, unit/flag producer
// inline/plain/forceinline declaration pairs except inline/inline: eight times 152.
// All bodies and declarations restored. Current flag call uses retail CL; two pool homes,
// object-pool reloads and frame 0x14 remain unresolved.
// DL192: Native word flag views for admission/publication/cleanup subsets 1-7 on ordinary traversal:
// 152/154/154/156/161/160/160; complete view with native four-field iterator 160. All target changes and the
// external flag declaration restored; pool homes remain.
// the datum index kept in EDI), holds the high water in ESI for each search, re-reads the iterator's
// pool from memory at the loop head and computes the element from a second spilled copy of the pool;
// ours keeps the pool in EDI and g_4e0300 in EBP across the loop (reloaded after each call) where
// retail re-reads it at each use; function_f0fd0's flag goes in DL where retail uses CL
// (BLOCKED-BY 0xf0fd0 for that call).
// Tried: shared step with/without the barrier 162/152; datum iterator 152 (frame 0x10); plus a pool
// local for the iterator and release 150 (frame 0x14); iterator from the global, local only for the
// release 127; that local assigned after the iterator 124 (retail's prologue). Round 406: shared
// absolute element getter, ordinary three/four-field iterator, early/late saved pool and shared
// datum-index producer 166/157/166/157/163/156/166/157. Round 14: barrier removed alone 161;
// finished arm first 149; if/else finished flag 123 (same code); early continue to the release 162;
// deletion test in one condition 127; codec pairs as writable globals (diagnostic) no change.
// DL192: native word flag views, subsets 1-7, 152-161; complete view with native four-field iterator
// 160. Round 298: unit/flag producer inline/plain/forceinline declaration pairs 152 each.
// Round 17: the shared search's volatile high-water read removed 162 (ESI holds the high water as
// retail, but 0x1fb760 and 0x1fb4c0 stop matching and 0x1fb6f0 rises 12 -> 41, so the volatile is
// load-bearing for them); this function alone on the non-volatile data_iterator_next_inlined 162
// (high water in ESI, pool still in EDI), the same without the barrier 152; the out-of-line
// record_pool_iterator_step 205 (stays a call, 422 bytes); animation declared before the iterator 122.
// DL199: Completion byte/word/short/long on ordinary three-field and four-field iterators: 152
// each. Four-field frame reaches 0x10, still below retail 0x14; the second pool home remains
// missing. All callback values and original body restored.
// DL209: Absolute-index scan/element API pairs 318/278/318/278/201/167; salted-handle APIs 174/185;
// single condition-step scan/element pairs 203/180/203/180. All restored.
// Round 250: Ordinary shared traversal with remaining-tick/control receiver subsets on three/four-
// field iterators 151/152/151/151/152/151; complete receivers with positive callback completion
// 166/166. Four-field frame 0x10 still lacks the second pool home. All matched controls survive;
// original body, include and private helpers restored.
// Earlier (122, was 122): this round: the permuter on this function finished with no candidate (10
// tries in 605.9 s, best 122, the baseline, no confirming build); retail's frame is 0x14 with the
// pool pointer in two stack homes and the current index in a third, ours is 0xc with the pool
// pointer in EDI, and the fast check's reduced image leaves g_4e0300 and g_4f5724 unresolved, so
// every line that loads one of them is counted as a difference.
// Earlier (122, was 122): this round: the listing was re-read against retail's and no new source
// shape was found. Retail's frame is 0x14 with the pool pointer in two stack homes and the
// current index in a third, ours is 0xc with the pool pointer in EDI. Note that the fast check
// builds a reduced image in which g_4e0300 and g_4f5724 are missing, so the global loads differ
// from retail's and the score overstates the real gap. A permuter run on this function was
// started at the end of the round.
// Earlier (122, was 122): this round: the listing was re-read to place the first difference; retail
// loads the pool once in the prologue and keeps it in two stack homes (one at the loop head, one
// for the element lookup) with the iteration index in a third, ours keeps the pool in EDI with one
// home, so the frame is 0xc against retail's 0x14 and every later offset differs. Still retail's
// second home for the pool pointer and its register-held datum index; BLOCKED-BY 0xf0fd0 for the
// flag register in the release path.
// Earlier (122, was 122): computing the datum index in the body from the element's salt and the
// iteration index, as retail does, gives 150 and 148 against 122, and a separate local for the
// pool pointer used by the release call gives 158; the length stays 473 in all of them. Still
// the frame of 0xc against retail's 0x14, retail's second home for the pool pointer and its
// register-held datum index.
// Round 250: Ordinary shared traversal with remaining-tick/control receiver subsets on three/four-
// field iterators 151/152/151/151/152/151; complete receivers with positive callback completion
// 166/166. Four-field frame 0x10 still lacks the second pool home. All matched controls survive;
// original body, include and private helpers restored.
// Round 57 (lane 16, fast with the file's iterator users): the barrier before the unit lookup 161 (479 bytes);
// the finished arm as the else of the tick arm with its own release 137 (486 bytes); `if (!unit) { release;
// continue; }` first 162 (482 bytes); the deletion test reading the object index at each use 127; `bool finished`
// declared at its assignment 122 (same code).
// @retail 0x1fb510
void function_1fb510(void)
{
	s_record_pool_iterator iterator;
	s_recorded_animation *animation;

	iterator.data = g_4f5724;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((animation = (s_recorded_animation *)function_1fb762(&iterator)) != 0)
	{
		s_recorded_animation_object *unit = recorded_animation_unit_try_and_get(animation->object_index);
		_ReadWriteBarrier();
		if (unit)
		{
			if (!(animation->flags & 1))
			{
				bool finished;

				animation->ticks--;
				finished = !g_46fd5c[animation->version]->apply((s_type_339e8b *)animation->controller,
					(playback_unit_control_view *)animation->control, &animation->remaining_ticks, &animation->cursor);
				animation->remaining_ticks++;
				function_c6de0(animation->object_index, animation->control);
				SET_FLAG(animation->flags, 0, finished);
				continue;
			}

			recorded_animation_object_set_flag(animation->object_index, 0, (animation->flags & 4) != 0);
			recorded_animation_object_set_flag(animation->object_index, 22, false);
			function_cbf60(animation->object_index, false);
			if (animation->flags & 8)
			{
				long object_index = animation->object_index;

				if (object_index != NONE && !function_beb30(object_index))
					function_b8540(object_index);
			}
			if (animation->flags & 0x10)
				function_f0fd0(animation->object_index, true);
		}

		record_pool_release(g_4f5724, iterator.datum_index);
	}
}
#pragma function(_ReadWriteBarrier)
