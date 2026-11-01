// @flags /O2 /Gr /arch:SSE
#include "object_masks_storage.h"
#include "unknown_11c920.h"
#include "props.h"
#include <float.h>
#include "unknown_26b230.h"
#include "globals.h"
#include "data_array.h"
#include "data_iterator.h"
#include "unknown_0259d0.h"
#include "slot_handler.h"
#include "unknown_20f040.h"
#include "unknown_2551c0.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "object_iterator.h"
#include "unknown_1e46c0.h"
#include "unknown_0d0690.h"

void function_1e2a90(long actor_index);
void __stdcall function_b83b0(long object_index, bool flag);
void __stdcall function_b8460(long object_index, bool flag);
void __stdcall function_b8540(long object_index);
void __stdcall swarm_delete(long index);
bool function_bba80(long object_index);
s_object *function_badc0(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *position);
void function_2691b0(long clump_index);
void function_2687a0(long clump_index);
void function_268900(long clump_index);
void function_2689d0();
__declspec(noinline) void function_268c90();
void function_268700(long clump_index);
void function_269870(long clump_index);
void function_26a210(long clump_index);
void function_26b290(long clump_index);
__declspec(noinline) void function_268840();

static __forceinline long real_to_long(real value);
PRIVATE __forceinline s_actor_view *clump_next_actor(s_iterator *iterator);

/* walks the clumps of g_502420, only while the ai is active (0x255d60's walk) */
struct s_clump_datum_walk
{
	s_data_datum_iterator iterator;
	long clump_index;
};

static inline void clump_datum_walk_new(s_clump_datum_walk *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->iterator.data = g_502420;
		iterator->iterator.index = NONE;
		iterator->iterator.datum_index = NONE;
	}
}

/* the next clump while the ai is active, or NULL */
PRIVATE __forceinline byte *clump_datum_walk_next(s_clump_datum_walk *iterator, bool active)
{
	byte *clump = NULL;

	if (active)
	{
		if (data_datum_iterator_next_inlined(&iterator->iterator))
			clump = iterator->iterator.datum;
		iterator->clump_index = iterator->iterator.datum_index;
	}
	return clump;
}

/* likewise, with the activity flag passed in and the index scan called */
PRIVATE __forceinline byte *clump_datum_walk_next_calling(s_clump_datum_walk *iterator, bool active)
{
	byte *clump = NULL;

	if (active)
	{
		if (data_datum_iterator_next_called(&iterator->iterator))
			clump = iterator->iterator.datum;
		iterator->clump_index = iterator->iterator.datum_index;
	}
	return clump;
}

/* likewise, calling the iterator step itself */
PRIVATE __forceinline byte *clump_datum_walk_step(s_clump_datum_walk *iterator, bool active)
{
	byte *clump = NULL;

	if (active)
	{
		if (data_datum_iterator_next(&iterator->iterator))
			clump = iterator->iterator.datum;
		iterator->clump_index = iterator->iterator.datum_index;
	}
	return clump;
}

// @retail 0x268510
void function_268510()
{
	short interval = (short)real_to_long((real)g_510c54->field_2_3);
	short slice_interval = (short)real_to_long((real)g_510c54->field_2_3 * 0.1f);
	s_clump_datum_walk clumps;
	byte *clump;

	clump_datum_walk_new(&clumps);
	while ((clump = clump_datum_walk_next(&clumps, g_4f55d0->active)) != NULL)
	{
		if (clump[0x1c])
			function_268700(clumps.clump_index);
	}
	long time = g_510c54->game_time;
	if (time % interval == 0)
		function_268840();
	else if (time % slice_interval == 0)
	{
		short maximum = 0;
		bool updated = false;

		clump_datum_walk_new(&clumps);
		while ((clump = clump_datum_walk_step(&clumps, g_4f55d0->active)) != NULL)
		{
			if (clump[0x1c])
			{
				if (*(short *)(clump + 0x1e) == *(long *)((byte *)g_4f55d0 + 0x18) && !updated)
				{
					function_269870(clumps.clump_index);
					function_26a210(clumps.clump_index);
					function_26b290(clumps.clump_index);
					*(short *)(clump + 0x1e) = 0;
					updated = true;
				}
				else
				{
					short count = ++*(short *)(clump + 0x1e);
					if (count > maximum)
						maximum = count;
				}
			}
			else
				*(short *)(clump + 0x1e) = 0;
		}
		*(long *)((byte *)g_4f55d0 + 0x18) = maximum;
	}
}

/* runs 0x2687a0 and 0x268900 for each clump whose byte at +0x1c is set, then
   0x2689d0 and 0x268c90 */
// @retail 0x268840
void function_268840()
{
	s_clump_datum_walk clumps;
	clump_datum_walk_new(&clumps);
	byte *clump;
	while ((clump = clump_datum_walk_next(&clumps, g_4f55d0->active)) != NULL)
	{
		if (clump[0x1c])
		{
			function_2687a0(clumps.clump_index);
			function_268900(clumps.clump_index);
		}
	}
	function_2689d0();
	function_268c90();
}

// @retail 0x268c90
void function_268c90()
{
	s_clump_datum_walk clumps;
	clump_datum_walk_new(&clumps);
	byte *clump;
	while ((clump = clump_datum_walk_next(&clumps, g_4f55d0->active)) != NULL)
	{
		if (clump[0x3c] && (*(long *)(clump + 0x38) == NONE ||
			*(long *)(clump + 0x38) + real_to_long((real)g_510c54->field_2_3 * 10.0f) < g_510c54->game_time))
		{
			clump[0x3c] = 0;
			s_iterator actors;
			actors.next = *(long *)(clump + 0x18);
			byte *actor;
			while ((actor = (byte *)clump_next_actor(&actors)) != NULL)
			{
				if (!actor[0xc]) continue;
				bool remove = true;
				if (actor[9])
				{
					bool keep = false;
					s_record_pool_iterator players;
					players.data = g_4e8c24;
					players.index = NONE;
					byte *player;
					while ((player = data_iterator_next_inlined(&players)) != NULL)
					{
						long unit = *(long *)(player + 0x2c);
						if (unit != NONE)
						{
							point3f point;
							function_b9dd0(unit, &point);
							real x = point.x - *(real *)(actor + 0x238);
							real y = point.y - *(real *)(actor + 0x23c);
							real z = point.z - *(real *)(actor + 0x240);
							if (225.0f > z * z + y * y + x * x) { keep = true; break; }
						}
					}
					if (!keep && *(long *)(actor + 0x18) != NONE)
						keep = function_bba80(*(long *)(actor + 0x18));
					remove = !keep;
				}
				if (remove)
				{
					long extra = NONE;
					if (*(long *)(actor + 0x26c) != NONE && *(long *)(actor + 0x26c) != *(long *)(actor + 0x18))
						extra = *(long *)(actor + 0x26c);
					s_actor_view *current = actor_get(actors.index);
					long unit = current->unknown018;
					long proxy = *(long *)((byte *)current + 0x1c);
					function_1e2a90(actors.index);
					if (unit != NONE)
					{
						function_b83b0(unit, true);
						function_b8460(unit, true);
					}
					else if (proxy != NONE) swarm_delete(proxy);
					if (extra != NONE)
					{
						byte *object = (byte *)function_badc0(extra, (dword)NONE);
						if (object)
						{
							s_object_child_iterator children;
							function_d0620(&children, extra);
							bool delete_extra = true;
							while (function_d0690(&children))
							{
								byte *child = *(byte **)(g_4e0300->data + (children.child_index & 0xffff) * 12 + 8);
								if (!child[0xaa])
								{
									delete_extra = false;
									break;
								}
							}
							if (delete_extra) function_b8540(extra);
						}
					}
				}
				else clump[0x3c] = 1;
			}
		}
		if (!*(short *)(clump + 0x10)) function_2691b0(clumps.clump_index);
	}
}


typedef bool (__stdcall *t_transition_test)(long, real *, real *);
typedef short (__stdcall *t_state_update)(long, real *, real *);

struct s_clump_state_entry
{
	long state;
	t_transition_test test;
	t_state_update update;
};

short __stdcall function_26b630(long clump_index, real *b, real *a);
short __stdcall function_26b660(long clump_index, real *b, real *a);
short __stdcall function_26b6b0(long clump_index, real *b, real *a);
bool __stdcall function_26b6e0(long clump_index, real *b, real *a);

short __stdcall function_26b700(long clump_index, real *b, real *a);
bool __stdcall function_26b750(long clump_index, real *b, real *a);

s_clump_state_entry g_470fac[4] =
{
	{0, 0, function_26b630},
	{1, 0, function_26b660},
	{2, function_26b6e0, function_26b6b0},
	{3, function_26b750, function_26b700},
};

// @retail 0x26b230
long function_26b230(long clump_index, long prop_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	long result = NONE;
	long index = clump->first_prop;

	while (index != NONE)
	{
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (index & 0xffff) * sizeof(s_clump_prop));
		if (prop->type == prop_index)
		{
			result = index;
			break;
		}
		index = prop->next;
	}

	return result;
}

PRIVATE __forceinline s_actor_view *clump_next_actor(s_iterator *iterator);

// @retail 0x26b290
void function_26b290(long clump_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	real a[13];
	real b[13];
	s_iterator objects;
	long i;

	for (i = 0; i < 13; i++)
	{
		b[i] = 0.0f;
		a[i] = 0.0f;
	}

	function_26bda0(clump_index, &objects);
	s_actor_view *actor;
	while ((actor = clump_next_actor(&objects)) != NULL)
	{
		short count = ((s_clump_object *)actor)->count;

		for (i = 0; i <= count; i++)
		{
			a[i] += 1.0;
		}
		b[count] += 1.0;
	}

	if (clump->divisor > 0)
	{
		for (i = 0; i < 13; i++)
		{
			b[i] /= clump->divisor;
			a[i] /= clump->divisor;
		}
	}

	long state;
	short new_state = NONE;
	for (state = 3; state > clump->state; state--)
	{
		t_transition_test test = g_470fac[state].test;
		if (test && test(clump_index, b, a))
		{
			new_state = state;
			break;
		}
	}
	if (new_state == NONE)
	{
		new_state = g_470fac[clump->state].update(clump_index, b, a);
	}

	if (new_state != NONE && new_state != clump->state)
	{
		clump->state_time = g_510c54->game_time;
		clump->state = new_state;
	}
}

// @retail 0x26b630
short __stdcall function_26b630(long clump_index, real *b, real *a)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	clump->unknown3e = 0;
	return NONE;
}

// @retail 0x26b660
short __stdcall function_26b660(long clump_index, real *b, real *a)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	if ((real)(g_510c54->game_time - clump->state_time) * g_510c54->rate > 30.0f)
	{
		clump->unknown30 = 0;
		return 0;
	}
	return NONE;
}

// @retail 0x26b6b0
short __stdcall function_26b6b0(long clump_index, real *b, real *a)
{
	short result = NONE;
	if (a[2] == 0.0f)
	{
		result = 1;
	}
	return result;
}

// @retail 0x26b6e0
bool __stdcall function_26b6e0(long clump_index, real *b, real *a)
{
	return a[3] > 0.0;
}

/* ---- whether a clump may use a prop ---- */
struct s_clump_prop_object
{
	byte unknown000[0x10a];
	word unknown10a_0 : 2;
	word flag10a_2 : 1;
	word unknown10a_3 : 13;
};

struct s_clump_prop_object_header
{
	byte unknown00[8];
	s_clump_prop_object *object;
};

s_ai_player *ai_player_get(long player_index);

PRIVATE __forceinline s_clump_node *clump_next_node(s_iterator *iterator)
{
	s_clump_node *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = (s_clump_node *)(g_502418->data + (index & 0xffff) * sizeof(s_clump_node));
		iterator->index = index;
		iterator->next = node->next;
	}
	return node;
}

// @retail 0x26b8a0
bool function_26b8a0(long prop_index, long actor_index)
{
	bool result = false;
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
	s_iterator iterator;
	iterator.next = prop->first_node;
	s_clump_node *node;

	while ((node = clump_next_node(&iterator)) != NULL)
	{
		if (node->state < 1 || node->state > 2 || node->actor_index == actor_index)
			continue;

		result = true;
		break;
	}

	return result;
}

struct s_clump_pool_iterator
{
	s_clump *current;
	s_record_pool_iterator pool;
};

PRIVATE __forceinline bool next_clump_in_pool(s_clump_pool_iterator *iterator)
{
	s_clump *result = NULL;
	if (g_4f55d0->active)
		result = (s_clump *)data_iterator_next_calling(&iterator->pool);
	iterator->current = result;
	return result != NULL;
}

// score 3 (was 3): Initial signed team load and EBP-save schedule remain; frame and the complete walk agree.
// Earlier forms:
// with a fifth word in the clump iterator (shared with 0x26bc60 and 0x269040;
// reverted in review, two points) the frame agrees at 66; retail sign-extends the
// first team argument (`movsx eax` after the clump address add, pushes after it) and zeroes the
// current-clump register (edi) at the loop top for the exhausted path, where ours zeroes ecx there.
// Round 80: `long team` local 92; current stored first in the step helper 92 (292 bytes);
// pointer-returning step 66; result set in both activity arms 66; iterator declared first 66.
// Earlier: retail sign-extends the first team argument and passes the second one
// narrow, and its frame is four bytes larger than ours, so the stack offsets and the register
// that holds the clump pointer differ as well.
// Pointer result/reference/both 68/68/68; current field first/pointer/reference 94/94/94.
// Promoted initial team 94 (sign extension recovered); result from initial test 127.
// Shared datum iterator 174; wide side 68; unsigned side 68; initial done 68; complete iterator 115; current
// initialized 68; prop type snapshot 93; prop reference 68.
// Both-arm step result/stored-current result/native step result/activity input/member step 68 each.
// Node query/prop query/both separate noinline declarations 68/68/68.
// Frame and nullable-current lifetime remain; current datum 68;
// success-controlled loop 68; selected datum 132; node/prop inline 68/68; forced node/prop
// expansion 246/158; datum plus success loop 68.
// frame, first team extension and nullable-current EDI lifetime remain;
// native clean success 102; optimistic result 113; initial success return 135; found success return 147;
// returned current pointer 68; existing record step 122; inline record step 119; entry side scope 68.
// frame 0x1c/0x20, first team sign extension and nullable current remain.
// Nullable value selection 68; inactive step exit 71; null step failure exit 94; word/byte
// success result 94/68; native step state value 123; shared step expansion 68; native selected
// index 68.
// a fifth iterator word (as the clump walk of 0x1c9830 keeps) 66, stored
// team in it 92, long team local 92, existing clump_get with long team 92, out-of-line pool step
// 122 (270 bytes), step through the current field 92 (292 bytes). A NULL-initialised result in the
// shared calling step moves current to edi (60, diagnostic): retail keeps the loop-top NULL live
// through the step's not-found arm and copies the found pointer from ecx.
// Existing complete calling walk bool/byte 3/3, long 128; complete walk calling the datum step
// bool/byte/long 133/133/165; inlined scan bool/long 136/122. Promoted initial team 31; actual
// pooled clump reference 3. Retained complete calling walk recovers frame 0x20, nullable current
// in EDI and exhausted-path layout.
// Initial getter 3; complete side result/comparison 15; per-use prop receiver 113;
// getter/complete side 15; getter/per-use receiver 113; complete side/per-use receiver 114;
// all three 114; typed initial pool array 3, on the recovered native calling walk.
// Round 247: early prop receiver 26; Boolean side 100; both 103; early receiver/promoted team 93; Boolean side/promoted team 102; all three 106; early receiver/entry walk 26; receiver/Boolean side/native handle 90. Initial extension and push schedule remain.
// Round 296: Side-query inline declaration alone/with predicate/lookup/both 3/3/3/3; with native short/promoted team 31/31; full side result 15; native team/full side 42. All executable source and declarations restored.
// Round 86: retail's unfolded add + movsx is the shape of a team read merged from two uses (as 0x201460's
// compare-then-pass); every promoted/short/int team local folds the add and sinks the EDI save 31; short
// const & / pointer to the field 25; clump_get named 3, in the argument 31; team or side getter by index 31,
// by clump pointer 3; prop between clump and side 26, before both 45.
// Round 92: read-only/immutable clump pointer, split clump/side assignments, side-before-clump
// declaration, const side, early iterator declaration, and inline predicate/lookup declarations each 3.
// Original body and declarations restored.
// Round 137 (lane 4): the walk reusing `clump` (no other_clump) with a long team local 31, the team passed
// directly 3; `function_20f040((long)clump->team)` 3; a long side compared as `(short)side` 3. The permuter matched
// only with a `volatile` clump pointer (not allowed): retail's first team read is an opaque load (movsx, address add
// not folded), the shape 0x26bc60 matched with its volatile read, so both likely share the original idiom.
// @retail 0x26b900
long function_26b900(long prop_index, long actor_index, long clump_index)
{
	bool result = function_26b8a0(prop_index, actor_index);
	if (!result)
	{
		s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
		short side = (short)function_20f040(clump->team);
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
		s_clump_datum_walk iterator;
		clump_datum_walk_new(&iterator);
		s_clump *other_clump;
		while ((other_clump = (s_clump *)clump_datum_walk_next_calling(&iterator, g_4f55d0->active)) != NULL)
		{
			if (clump_index != iterator.clump_index && side == (short)function_20f040(other_clump->team))
			{
				long other_prop_index = function_26b230(iterator.clump_index, prop->type);
				if (other_prop_index != NONE && function_26b8a0(other_prop_index, NONE))
				{
					result = true;
					break;
				}
			}
		}
	}
	return result;
}

// @retail 0x26ba60
bool function_26ba60(long prop_index, long actor_index, long clump_index)
{
	bool result = false;
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));

	if (!prop->unknown34)
	{
		if (TEST_FIELD_BIT(((s_clump_prop_object_header *)g_4e0300->data)[prop->type & 0xffff].object->flag10a_2))
			result = false;
		else if (clump_index != NONE)
		{
			s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
			if (!team_is_enemy(clump->team, 1))
			{
				s_game_time_globals *game_time_globals = g_510c54;
				long game_time = game_time_globals->game_time;
				s_data_datum_iterator players;

				/* true unless a player's time stamp is too recent or an actor's prop node is in state 1 or 2 */
				result = true;
				players.data = g_4e8c24;
				players.index = NONE;
				players.datum_index = NONE;
				while (data_datum_iterator_next(&players))
				{
					s_ai_player *player = ai_player_get(players.datum_index);
					if (player && game_time - player->unknown0c < game_time_globals->field_2_3)
						result = false;
				}

				if (result)
				{
					s_iterator actors;
					s_actor_view *actor;

					function_26bda0(clump_index, &actors);
					while ((actor = clump_next_actor(&actors)) != NULL)
					{
						if (actor->prop_index != NONE)
						{
							s_clump_node *node = (s_clump_node *)(g_502418->data + (actor->prop_index & 0xffff) * sizeof(s_clump_node));
							if (node->state < 1 || node->state > 2)
								continue;

							result = false;
							break;
						}
					}
				}
			}
			else
				result = !function_26b900(prop_index, actor_index, clump_index);
		}
	}

	return result;
}


/* Additional views of the same 0x50-byte group records and 0x3c-byte nodes. */
struct s_clump_activity_view
{
	byte unknown00[4];
	point3f center;
	short count;
	short team;
	long first_prop;
	long first_actor;
	bool active;
	byte unknown1d[3];
	long active_time;
	short update_state;
	short state;
	long state_time;
	byte unknown2c[0xc];
	long time38;
	byte unknown3c[4];
	long time40;
	byte unknown44[0xc];
};

struct s_clump_link_view
{
	byte unknown00[4];
	long actor_index;
	long prop_index;
	byte unknown0c[0x28];
	long next;
	long previous;
};

PRIVATE __forceinline s_actor_view *clump_next_actor(s_iterator *iterator)
{
	s_actor_view *actor = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		actor = actor_get(index);
		iterator->index = index;
		iterator->next = actor->next_index;
	}
	return actor;
}

PRIVATE __forceinline void clump_add_position(point3f const *a, point3f const *b, point3f *result)
{
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	result->z = a->z + b->z;
}

// @retail 0x2687a0
void function_2687a0(long clump_index)
{
	s_clump_activity_view *clump = &((s_clump_activity_view *)g_502420->data)[clump_index & 0xffff];
	if (clump->active)
	{
		s_iterator iterator;
		iterator.next = ((s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view)))->first_actor;
		s_actor_view *actor;
		while ((actor = clump_next_actor(&iterator)) != NULL)
		{
			if (actor->unknown009)
				break;
		}
		if (!actor)
		{
			clump->active = false;
			clump->active_time = g_510c54->game_time - 1;
		}
		else
		{
			clump->active_time = g_510c54->game_time;
		}
	}
}

// @retail 0x268810
void function_268810(long clump_index)
{
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->active = true;
	clump->update_state = 1;
}

// @retail 0x268900
void function_268900(long clump_index)
{
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->center.x = 0.0f;
	clump->center.y = 0.0f;
	clump->center.z = 0.0f;
	clump->count = 0;
	s_iterator iterator;
	iterator.next = ((s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view)))->first_actor;
	s_actor_view *actor;
	while ((actor = clump_next_actor(&iterator)) != NULL)
	{
		clump_add_position(&actor->position, &clump->center, &clump->center);
		clump->count++;
	}
	if (clump->count > 0)
	{
		real scale = (real)(1.0 / clump->count);
		clump->center.x = scale * clump->center.x;
		clump->center.y = scale * clump->center.y;
		clump->center.z = scale * clump->center.z;
	}
}

// @retail 0x268c60
void function_268c60(long clump_index)
{
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->time38 = g_510c54->game_time;
}

void function_1e2150(long actor_index, long object_index);

// @retail 0x26abd0
void function_26abd0(s_clump_link_view *node)
{
	s_actor_view *actor = actor_get(node->actor_index);
	long removed;
	if (*(long *)((byte *)node + 0x30) == NONE)
	{
		removed = *(long *)((byte *)actor + 0x58);
		*(long *)((byte *)actor + 0x58) = *(long *)((byte *)node + 0x2c);
	}
	else
	{
		s_clump_link_view *previous = (s_clump_link_view *)(g_502418->data + (*(long *)((byte *)node + 0x30) & 0xffff) * sizeof(s_clump_link_view));
		removed = *(long *)((byte *)previous + 0x2c);
		*(long *)((byte *)previous + 0x2c) = *(long *)((byte *)node + 0x2c);
	}
	if (*(long *)((byte *)node + 0x2c) != NONE)
	{
		s_clump_link_view *next = (s_clump_link_view *)(g_502418->data + (*(long *)((byte *)node + 0x2c) & 0xffff) * sizeof(s_clump_link_view));
		*(long *)((byte *)next + 0x30) = *(long *)((byte *)node + 0x30);
	}
	function_1e2150(node->actor_index, removed);
	node->actor_index = NONE;
	*(long *)((byte *)node + 0x30) = NONE;
	*(long *)((byte *)node + 0x2c) = NONE;
}

// @retail 0x26ac60
void function_26ac60(s_clump_link_view *node)
{
	if (node->previous == NONE)
	{
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (node->prop_index & 0xffff) * sizeof(s_clump_prop));
		prop->first_node = node->next;
	}
	else
	{
		s_clump_link_view *previous = (s_clump_link_view *)(g_502418->data + (node->previous & 0xffff) * sizeof(s_clump_link_view));
		previous->next = node->next;
	}
	if (node->next != NONE)
	{
		s_clump_link_view *next = (s_clump_link_view *)(g_502418->data + (node->next & 0xffff) * sizeof(s_clump_link_view));
		next->previous = node->previous;
	}
	node->prop_index = NONE;
	node->previous = NONE;
	node->next = NONE;
}

PRIVATE __forceinline s_prop_node_view *clump_member_prop_next(s_iterator *iterator);
PRIVATE __forceinline s_type_76cf92 *clump_next_prop(s_iterator *iterator);

/* a walk over a clump's props that keeps the current prop ahead of its iterator,
   as s_clump_pool_iterator keeps the current clump */
struct s_clump_prop_walk
{
	s_type_76cf92 *prop;
	s_iterator iterator;
};

// @retail 0x26b1d0
void function_26b1d0(long actor_index)
{
	s_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = clump_member_prop_next(&iterator)) != NULL)
	{
		if (node->unknown24 <= 0)
		{
			function_26abd0((s_clump_link_view *)node);
			record_pool_release(g_502418, iterator.index);
		}
	}
}

// @retail 0x26aaf0
void function_26aaf0(s_clump *clump, long prop_index)
{
	s_clump *const *clump_reference = &clump;
	long const *prop_reference = &prop_index;
	long slot = prop_index & 0xffff;
	long index = *(long *)((byte *)prop_get(prop_index) + 0x18);
	while (index != NONE)
	{
		s_clump_link_view *node = (s_clump_link_view *)(g_502418->data + (index & 0xffff) * sizeof(s_clump_link_view));
		long next = node->next;
		function_26abd0(node);
		function_26ac60(node);
		long tracking = ((s_prop_datum *)node)->tracking_index;
		if (tracking != NONE) record_pool_release(g_502414, tracking);
		record_pool_release(g_502418, index);
		index = next;
	}
	long *link = &(*clump_reference)->first_prop;
	while (*link != NONE)
	{
		long current = *link & 0xffff;
		s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + current * sizeof(s_clump_prop));
		if (current == slot)
		{
			*link = prop->next;
			break;
		}
		link = &prop->next;
	}
	record_pool_release(g_50241c, *prop_reference);
}

void function_2694d0(long clump_index, long actor_index);

PRIVATE inline s_clump *clump_get(long clump_index)
{
	return (s_clump *)((clump_index & 0xffff) * 0x50 + g_502420->data);
}

// @retail 0x26a480
short function_26a480(long clump_index, short minimum)
{
	short const *minimum_reference = &minimum;
	s_iterator iterator;
	iterator.next = clump_get(clump_index)->first_prop;
	s_clump *clump = clump_get(clump_index);
	short count = 0;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		if (*(short *)((byte *)prop + 0xc) < *minimum_reference)
		{
			function_26aaf0(clump, iterator.index);
			count++;
		}
	}
	return count;
}

// @retail 0x268700
void function_268700(long clump_index)
{
	s_iterator iterator;
	iterator.next = clump_get(clump_index)->first_prop;
	s_clump *clump = clump_get(clump_index);
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		long object_index = *(long *)((byte *)prop + 8);
		byte *object = NULL;
		if (object_index != NONE && (object_index & 0xffff) < g_4e0300->high_water_index)
		{
			s_object_header_view *header = (s_object_header_view *)(g_4e0300->data + g_4e0300->size * (object_index & 0xffff));
			if (header->salt && header->salt == (object_index >> 16) && ((1 << header->type) & -1))
				object = header->object;
		}
		if (!object) function_26aaf0(clump, iterator.index);
	}
}

// @retail 0x2691b0
void function_2691b0(long clump_index)
{
	long const *clump_reference = &clump_index;
	s_iterator props;
	props.next = clump_get(clump_index)->first_prop;
	s_clump *clump = clump_get(clump_index);
	while (clump_next_prop(&props) != NULL)
		function_26aaf0(clump, props.index);
	s_iterator actors;
	actors.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_object;
	while (clump_next_actor(&actors) != NULL)
		function_2694d0(*clump_reference, actors.index);
	record_pool_release(g_502420, *clump_reference);
}

struct s_object;
point3f *function_b9dd0(long object_index, point3f *result);
bool function_bec70(long object_index);
bool function_26add0(long prop_index);

struct s_prop_candidate_view
{
	short type;
	short unknown02;
	long object_index;
	short priority;
	short unknown0a;
	long prop_index;
};

// @retail 0x269e60
short function_269e60(long object_index, s_clump_activity_view const *clump, long prop_index,
	s_prop_candidate_view *candidate, bool force)
{
	long const *object_reference = &object_index;
	s_clump_activity_view const *const *clump_reference = &clump;
	long const *prop_reference = &prop_index;
	s_prop_candidate_view *const *candidate_reference = &candidate;
	bool const *force_reference = &force;
	short type;
	short priority;
	bool unsupported;
	type = priority = 0;
	unsupported = false;
	s_slot_object_view *object = (s_slot_object_view *)function_badc0(object_index, (dword)NONE);
	if (object)
	{
		if (candidate)
		{
			long object_slot = object_index & 0xffff;
			if (g_4de300[object_slot] == g_4de2fc)
				goto done;
			g_4de300[object_slot] = g_4de2fc;
		}
		if (prop_index != NONE)
			force = prop_get(prop_index)->unknown10 > g_510c54->game_time;
		point3f point;
		function_b9dd0(*object_reference, &point);
		vector3f delta;
		delta.i = point.x - clump->center.x;
		delta.j = point.y - clump->center.y;
		delta.k = point.z - clump->center.z;
		real distance_squared = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
		if (*force_reference || distance_squared < 2500.0)
		{
			switch ((char)object->type)
			{
			case 1:
				if (object->actor_index == NONE)
				{
					priority = 1;
					type = 4;
					break;
				}
			case 0:
				{
					bool flag = (bool)((*(byte *)((byte *)object + 0x10a) >> 2) & 1);
					bool player = false;
					short team;
					if (object->actor_index != NONE)
						team = actor_get(object->actor_index)->unknown024;
					else if (object->player_index != NONE)
					{
						team = *(char *)(g_4e8c24->data + (object->player_index & 0xffff) * 0x21c + 0xc0);
						player = true;
					}
					else
						team = object->team;
					bool enemy = team == clump->team ? false : function_1df560(team, clump->team);
					if (flag)
					{
						priority = 1;
						type = enemy ? 6 : 7;
					}
					else if (enemy)
					{
						priority = 3;
						type = 1;
					}
					else
					{
						type = 2;
						if (player) priority = 3;
					}
				}
				break;
			case 12:
				if (*(long *)((byte *)object + 0x134) == 0)
				{
					byte *data = (byte *)object + *(short *)((byte *)object + 0x13a);
					if (function_1df560(*(short *)((byte *)object + 0x12e), clump->team))
					{
						type = 1;
						priority = 3;
					}
					else
					{
						type = 2;
						priority = 0;
					}
					long index = *(long *)(data + 8);
					if (index != NONE)
					{
						s_ai_object_iterator iterator;
						iterator.index = NONE;
						iterator.next_index = perception_get(index)->object_index;
						while (swarm_creature_iterator_next(&iterator))
							function_bec70(iterator.index);
					}
				}
				break;
			case 5:
				{
					byte *tag = g_4e3b44[object->tag_index & 0xffff].bytes;
					if (*(real *)(tag + 0xd0) > 0.0f &&
						(object->parent_index == NONE || (bool)((*(dword *)((byte *)object + 0x12c) >> 5) & 1)))
					{
						priority = 3;
						type = 3;
					}
				}
				break;
			default:
				unsupported = true;
				break;
			}
			if (!*force_reference && priority > 0)
			{
				real distance = (real)sqrt(distance_squared);
				if (*prop_reference != NONE && function_26add0(prop_index))
					priority -= (long)(distance / (*(real *)((byte *)*clump_reference + 0x2c) * 0.6666666865348816f));
				else
					priority -= (long)(distance / (*(real *)((byte *)*clump_reference + 0x2c) * 0.3333333432674408f));
				priority = priority > 0 ? priority : 0;
			}
		}
	}
done:
	if (*candidate_reference && !unsupported)
	{
		candidate->type = type;
		candidate->prop_index = prop_index;
		candidate->object_index = object_index;
		candidate->priority = priority;
	}
	return priority;
}

// @retail 0x269de0
void __stdcall function_269de0(long object_index, s_clump_activity_view const *clump, s_prop_candidate_view *candidates, short *count)
{
	long const *object_reference = &object_index;
	s_clump_activity_view const *const *clump_reference = &clump;
	s_prop_candidate_view *const *candidates_reference = &candidates;
	short *const *count_reference = &count;
	while (*object_reference != NONE)
	{
		if (**count_reference >= 100) break;
		s_slot_object_view *object = object_get(object_index);
		if (function_269e60(object_index, *clump_reference, NONE, &(*candidates_reference)[*count], false) >= (*clump_reference)->update_state)
			++*count;
		if (object->first_child_index != NONE)
			function_269de0(object->first_child_index, *clump_reference, candidates, count);
		object_index = object->next_object_index;
	}
}

// @retail 0x26add0
bool function_26add0(long prop_index)
{
	bool result = false;
	s_clump_prop *const props = (s_clump_prop *)g_50241c->data;
    s_clump_prop *const *props_reference = &props;
    s_clump_prop *prop = &(*props_reference)[prop_index & 0xffff];
	if (*(short *)((byte *)prop + 4) >= 1)
	{
		result = true;
	}
	else
	{
		s_iterator iterator;
		iterator.next = prop->first_node;
		s_clump_node *node;
		while ((node = clump_next_node(&iterator)) != NULL)
		{
			if (node->state >= 1)
			{
				result = true;
				goto done;
			}
		}
	}
done:
	return result;
}

// @retail 0x26b700
short __stdcall function_26b700(long clump_index, real *b, real *a)
{
	short result = NONE;
	if (a[2] == 0.0)
		result = 1;
	else if (a[5] == 0.0f)
		result = 2;
	return result;
}

// @retail 0x26b750
bool __stdcall function_26b750(long clump_index, real *b, real *a)
{
	return a[5] > 0.0;
}

// @retail 0x26b770
void function_26b770(long clump_index)
{
	if (clump_index != NONE)
	{
		s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
		clump->time40 = g_510c54->game_time;
	}
}

// @retail 0x26ba40
bool function_26ba40(long clump_index, long actor_index, long prop_index)
{
	bool result = true;
	if (clump_index != NONE)
		result = !function_26b900(prop_index, actor_index, clump_index);
	return result;
}

PRIVATE __forceinline s_prop_node_view *clump_member_prop_next(s_iterator *iterator)
{
	s_prop_node_view *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = prop_node_get(index);
		iterator->index = index;
		iterator->next = node->next_index;
	}
	return node;
}

// @retail 0x2694d0
void function_2694d0(long clump_index, long actor_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	long *link = &clump->first_object;
	while (*link != NONE)
	{
		long index = *link;
		s_actor_view *actor = actor_get(index);
		if (index == actor_index)
		{
			*link = actor->next_index;
			actor->unknown07c = NONE;
			s_iterator iterator;
			iterator.next = actor_get(actor_index)->first_prop_index;
			s_prop_node_view *node;
			while ((node = clump_member_prop_next(&iterator)) != NULL)
				function_26ac60((s_clump_link_view *)node);
			clump->divisor--;
			return;
		}
		link = &actor->next_index;
	}
}


struct s_prop_notice_view
{
	short fields[10];
};

struct s_prop_copy_view
{
	short salt;
	short type;
	short state;
	short unknown06;
	long object_index;
	short field0c;
	short unknown0e;
	long field10;
	long field14;
	long field18;
	long actor_index;
	short team;
	bool field22;
	bool field23;
	bool field24;
	bool field25;
	short unknown26;
	long field28;
	long field2c;
	byte field30;
	byte field31;
	byte field32;
	byte field33;
	byte field34;
	byte field35;
	byte field36;
	byte unknown37;
	long field38;
	short unknown3c;
	s_prop_notice_view notice;
	byte field52;
	byte field53;
	long field54;
	s_type_5cfb45 observed;
};

void prop_state_initialize(s_type_5cfb45 *state);

// @retail 0x26a310
void function_26a310(s_prop_copy_view *prop, short type, long object_index, s_clump *clump, short value)
{
	prop->type = type;
	prop->object_index = object_index;
	prop->field14 = NONE;
	prop->field0c = value;
	prop->state = g_470f10[type].unknown4 == 0;
	prop->field10 = NONE;
	prop->field18 = NONE;
	s_slot_object_view volatile *object = object_get(object_index);
	object->unknownb2 |= 2;
	unsigned long type_mask = 1 << object->type;
	if (type_mask & 3)
	{
		long actor_index = object->actor_index;
		prop->actor_index = actor_index;
		prop->team = object->team;
		prop->field23 = function_1df560(clump->team, prop->team);
		prop->field24 = !prop->field23;
		prop->field25 = NONE != object->player_index;
		prop->field22 = false;
	}
	else if (type_mask & 0x1000)
	{
		switch (*(long *)((byte *)object + 0x134))
		{
		case 0:
		{
			byte *extra = (byte *)object + *(short *)((byte *)object + 0x13a);
			prop->actor_index = *(long *)(extra + 4);
			prop->team = *(short *)((byte *)object + 0x12e);
			prop->field23 = function_1df560(clump->team, prop->team);
			prop->field24 = !prop->field23;
			prop->field25 = false;
			prop->field22 = *(long *)(extra + 8) != NONE;
			break;
		}
		case 1:
			prop->actor_index = NONE;
			prop->team = NONE;
			prop->field23 = false;
			prop->field24 = false;
			prop->field25 = false;
			prop->field22 = false;
			break;
		}
	}
	else
	{
		prop->actor_index = NONE;
		prop->team = NONE;
		prop->field23 = false;
		prop->field24 = false;
		prop->field25 = false;
		prop->field22 = false;
	}
	prop_state_initialize(&prop->observed);
	*(short *)&prop->field54 = NONE;
	*((short *)&prop->field54 + 1) = NONE;
	prop->field38 = NONE;
	*(real *)&prop->field2c = 0.0f;
	*(real *)&prop->field28 = 0.0f;
	prop->field36 = false;
	prop->field53 = false;
	prop->field52 = false;
	*(bool *)&prop->unknown3c = false;
	prop->field30 = false;
	prop->field32 = false;
	prop->field33 = false;
	prop->field34 = false;
	prop->field35 = false;
}

// @retail 0x26a7d0
void function_26a7d0(s_prop_copy_view *destination, s_prop_copy_view const *source)
{
	destination->state = source->state;
	destination->field2c = source->field2c;
	destination->field28 = source->field28;
	destination->observed = source->observed;
	destination->field52 = source->field52;
	destination->field53 = source->field53;
	destination->field54 = source->field54;
	destination->notice = source->notice;
	destination->field30 = source->field30;
	destination->field32 = source->field32;
	destination->field33 = source->field33;
	destination->field34 = source->field34;
	destination->field35 = source->field35;
	destination->field31 = source->field31;
	destination->field36 = source->field36;
	destination->field38 = source->field38;
}

// @retail 0x26a8d0
long function_26a8d0(long prop_index)
{
	s_prop_copy_view *prop = (s_prop_copy_view *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_prop_copy_view));
	long index = record_pool_allocate(g_502418);
	if (index != NONE)
	{
		s_prop_datum *node = prop_ref_get(index);
		node->unknown10 = 0.0f;
		*(short *)node->unknown0c = 0;
		node->tracking_index = NONE;
		node->unknown1c = NONE;
		node->object_index = *(long *)((byte *)prop + 8);
		node->type = prop->type;
		node->state = prop->state;
		node->prop_index = NONE;
		node->actor_index = NONE;
		node->unknown28 = FLT_MAX;
		node->unknown27 = 0;
		node->next_index = NONE;
		*(long *)((byte *)node + 0x30) = NONE;
		*(long *)((byte *)node + 0x34) = NONE;
		*(long *)((byte *)node + 0x38) = NONE;
	}
	return index;
}


point3f *function_b9dd0(long object_index, point3f *position);

// @retail 0x26aa40
void function_26aa40(long actor_index, long node_index)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_datum *node = prop_ref_get(node_index);
	if (actor->first_prop_index != NONE)
		*(long *)((byte *)prop_ref_get(actor->first_prop_index) + 0x30) = node_index;
	node->next_index = actor->first_prop_index;
	actor->first_prop_index = node_index;
	node->actor_index = actor_index;
	point3f position;
	function_b9dd0(node->object_index, &position);
	vector3f delta;
	vector3d_from_points3d(&position, &actor->position, &delta);
	node->unknown28 = (real)sqrt(delta.j * delta.j + (delta.i * delta.i + delta.k * delta.k));
}


struct s_clump_owner_object_view
{
	byte unknown00[0xaa];
	byte type;
	byte unknownab[0x134 - 0xab];
	long owner_kind;
	short unknown138;
	short owner_offset;
};

PRIVATE __forceinline long clump_object_actor_index(long object_index)
{
	s_clump_owner_object_view *object = (s_clump_owner_object_view *)((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	long result = NONE;
	if (object->type == 0xc)
	{
		byte *owner = object->owner_kind == 0 ? (byte *)object + object->owner_offset : NULL;
		if (owner)
			result = *(long *)(owner + 4);
	}
	return result;
}

PRIVATE __forceinline s_type_76cf92 *clump_next_prop(s_iterator *iterator)
{
	s_type_76cf92 *prop = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		prop = prop_get(index);
		iterator->index = index;
		iterator->next = *(long *)((byte *)prop + 0x14);
	}
	return prop;
}

// @retail 0x26b120
long function_26b120(long clump_index, long object_index)
{
	long result = NONE;
	long actor_index = clump_object_actor_index(object_index);
	s_iterator iterator;
	iterator.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		if (*(long *)((byte *)prop + 8) == object_index ||
			(prop->unknown22 && actor_index != NONE && actor_index == prop->actor_index))
		{
			result = iterator.index;
			break;
		}
	}
	return result;
}

// @retail 0x26b7a0
void function_26b7a0(long prop_index, long clump_index, bool immediate, bool *available, bool *first, bool *notify)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	s_type_76cf92 *prop = prop_get(prop_index);
	long time = g_510c54->game_time;
	*available = prop->unknown23;
	if (prop->unknown23 && !((s_prop_copy_view *)prop)->field30)
	{
		*first = !clump->unknown30;
		if (prop->unknown25 || immediate)
			*notify = true;
		else if (*(long *)((byte *)clump + 0x34) == NONE || (real)(time - *(long *)((byte *)clump + 0x34)) * g_510c54->rate > 5.0f)
			*notify = clump->state < 3;
		else
			*notify = false;
		*(long *)((byte *)clump + 0x34) = time;
		clump->unknown30 = true;
		((s_prop_copy_view *)prop)->field30 = true;
	}
	else
	{
		*notify = false;
		*first = false;
	}
}


// @retail 0x268bb0
real function_268bb0(s_clump_activity_view const *clump, s_actor_view const *actor)
{
	vector3f delta;
	vector3d_from_points3d(&actor->position, &clump->center, &delta);
	real distance = (real)sqrt(delta.k * delta.k + (delta.j * delta.j + delta.i * delta.i));
	if (distance > 6.0)
		return 0.0f;
	if (distance < 2.5)
		return 1.0f;
	double value = (short)(clump->count * 10.0 / distance);
	if (value > 1.0)
		value = 1.0;
	return (real)value;
}

// @retail 0x26a960
void function_26a960(s_clump *clump, long prop_index)
{
	s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
	prop->next = clump->first_prop;
	clump->first_prop = prop_index;
	s_iterator iterator;
	iterator.next = clump->first_object;
	while (clump_next_actor(&iterator))
	{
		long node_index = function_26a8d0(prop_index);
		if (node_index != NONE)
		{
			prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
			s_prop_datum *node = prop_ref_get(node_index);
			long previous = prop->first_node;
			if (previous != NONE)
				*(long *)((byte *)prop_ref_get(previous) + 0x38) = node_index;
			*(long *)((byte *)node + 0x34) = previous;
			prop->first_node = node_index;
			node->prop_index = prop_index;
			function_26aa40(iterator.index, node_index);
		}
	}
}


// @retail 0x26bc60
long function_26bc60(long clump_index)
{
	s_clump *clump = (s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump));
	struct
	{
		real count;
		real value;
		s_clump_pool_iterator iterator;
		long team;
	} state;
	state.count = 0.0f;
	state.value = 0.0f;
	state.team = ((s_clump volatile *)clump)->team;
	long side = function_20f040((short)state.team);
	if (g_4f55d0->active)
	{
		state.iterator.pool.data = g_502420;
		state.iterator.pool.index = NONE;
	}
	for (;;)
	{
		s_clump *other = NULL;
		if (g_4f55d0->active)
			other = (s_clump *)data_iterator_next_inlined(&state.iterator.pool);
		state.iterator.current = other;
		if (!other)
			break;
		long other_side = function_20f040(other->team);
		if ((short)other_side == (short)side)
		{
			state.count += other->divisor;
			state.value += (short)other->unknown3e;
		}
	}
	if (state.count * 0.2f > state.value)
		return 1;
	if (state.count * 0.8f > state.value)
		return 0;
	return 2;
}

/* the clump pool walk 0x269040 makes: like s_clump_pool_iterator with a trailing index slot,
   as s_ai_importance_squad_iterator (ai.cpp) has; retail's frame keeps the slot */
struct s_clump_walk_269040
{
	s_clump *clump;
	s_record_pool_iterator pool;
	long clump_index;
};

/* record_pool_iterator_step with its result cleared before the search */
PRIVATE __forceinline byte *clump_walk_step_269040(s_record_pool_iterator *iterator)
{
	byte *result = NULL;
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
	}
	return result;
}

PRIVATE __forceinline s_clump *clump_walk_advance_269040(s_clump_walk_269040 *walk)
{
	s_clump *clump = NULL;
	if (g_4f55d0->active)
		clump = (s_clump *)clump_walk_step_269040(&walk->pool);
	walk->clump = clump;
	return clump;
}

/* a new clump at the center for the team; when the pool is full, the clump
   whose active time is oldest is freed first */
// score 106 (was 127): the frame and the walk's slots now match; retail clears the clump
// register (ebp) before testing the active flag and keeps the result in ebx, the pool in esi
// and the index in edi; ours clears it just before the search call and keeps the result in
// ebp, NONE in ebx and the pool in edi. Tried: the stock data_iterator_next_calling in the
// loop body (139, the empty search jumps straight out), next_clump_in_pool (125),
// record_pool_iterator_step (164, not inlined), the step written into one helper with the
// clump cleared first (139), the stock step under this walk (124). Tried at 127 (lane): plain
// oldest-time/index storage separately 136/126; byte activity 127; native pool step 160; activity while 136.
// @retail 0x269040
long function_269040(point3f const *center, short team)
{
	long result = record_pool_allocate(g_502420);
	if (result == NONE)
	{
		s_clump_walk_269040 walk;
		if (g_4f55d0->active)
		{
			walk.pool.data = g_502420;
			walk.pool.index = NONE;
		}
		long oldest = NONE;
		long oldest_time = 0x7fffffff;
		s_clump_activity_view *clump;
		while ((clump = (s_clump_activity_view *)clump_walk_advance_269040(&walk)) != NULL)
		{
			if (clump->active_time < oldest_time)
			{
				oldest_time = clump->active_time;
				oldest = walk.pool.datum_index;
			}
		}
		if (oldest != NONE)
		{
			function_2691b0(oldest);
			result = record_pool_allocate(g_502420);
		}
	}
	if (result != NONE)
	{
		s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (result & 0xffff) * sizeof(s_clump_activity_view));
		clump->center = *center;
		clump->team = team;
		clump->count = 0;
		clump->first_actor = NONE;
		clump->first_prop = NONE;
		clump->active = true;
		*(short *)((byte *)clump + 0x1e) = 0;
		clump->active_time = g_510c54->game_time;
		clump->state = 0;
		clump->state_time = g_510c54->game_time;
		*(bool *)((byte *)clump + 0x30) = false;
		*(long *)((byte *)clump + 0x34) = NONE;
		clump->time38 = NONE;
		*(bool *)((byte *)clump + 0x3c) = false;
		*(real *)((byte *)clump + 0x2c) = 50.0f;
		clump->update_state = 1;
		*(short *)((byte *)clump + 0x3e) = 0;
		*(long *)((byte *)clump + 0x4c) = NONE;
	}
	return result;
}

// @retail 0x26a4f0
long function_26a4f0(s_clump_activity_view *clump, short priority)
{
	long result = NONE;
	if (priority >= clump->update_state)
	{
		result = record_pool_allocate(g_50241c);
		if (result == NONE)
		{
			long time = g_510c54->game_time;
			short removed = 0;
			long selected;
			do
			{
				s_clump_pool_iterator iterator;
				real best_score = 0.0f;
				selected = NONE;
				if (g_4f55d0->active)
				{
					iterator.pool.data = g_502420;
					iterator.pool.index = NONE;
				}
				for (;;)
				{
					iterator.current = NULL;
					if (g_4f55d0->active)
						iterator.current = (s_clump *)data_iterator_next_inlined(&iterator.pool);
					s_clump_activity_view *other = (s_clump_activity_view *)iterator.current;
					if (!other) break;
					if (other->update_state >= 4 || (other == clump && priority <= other->update_state + 1))
						continue;
					real score;
					if (!other->active)
					{
						score = (real)(time - other->active_time) * g_510c54->rate * 0.0033333334140479565f;
						if (score < 0.0f) score = 0.0f;
						else if (score > 1.0f) score = 1.0f;
						score = score * 10.100000381469727f + 8.0f;
					}
					else
					{
						score = (real)(3 - other->state) * 0.3333333432674408f;
						if (score < 0.0f) score = 0.0f;
						else if (score > 1.0f) score = 1.0f;
						score *= 2.0f;
					}
					score = (real)(3 - other->update_state) * 5.0f + score;
					if (score > best_score)
					{
						best_score = score;
						selected = iterator.pool.datum_index;
					}
				}
				if (selected != NONE)
				{
					s_clump_activity_view *other = (s_clump_activity_view *)(g_502420->data + (selected & 0xffff) * sizeof(s_clump_activity_view));
					other->update_state++;
					removed = function_26a480(selected, other->update_state);
				}
			} while (selected != NONE && removed <= 0);
			if (removed > 0) result = record_pool_allocate(g_50241c);
		}
	}
	return result;
}

// @retail 0x26a780
long function_26a780(s_clump_activity_view *clump, s_prop_candidate_view const *candidate)
{
	long result = function_26a4f0(clump, candidate->priority);
	if (result != NONE)
	{
		s_prop_copy_view *prop = (s_prop_copy_view *)(g_50241c->data + (result & 0xffff) * sizeof(s_prop_copy_view));
		function_26a310(prop, candidate->type, candidate->object_index, (s_clump *)clump, candidate->priority);
	}
	return result;
}

// @retail 0x26a860
long function_26a860(s_clump_activity_view *clump, s_prop_copy_view const *source)
{
	s_prop_copy_view copy = *source;
	long result = function_26a4f0(clump, source->field0c);
	if (result != NONE)
	{
		s_prop_copy_view *prop = (s_prop_copy_view *)(g_50241c->data + (result & 0xffff) * sizeof(s_prop_copy_view));
		function_26a310(prop, copy.type, copy.object_index, (s_clump *)clump, copy.field0c);
		function_26a7d0(prop, &copy);
	}
	return result;
}

// @retail 0x26a740
long function_26a740(long object_index, s_clump_activity_view *clump)
{
	s_prop_candidate_view candidate;
	g_4de2fc++;
	g_4de2f8 = true;
	function_269e60(object_index, clump, NONE, &candidate, true);
	g_4de2f8 = false;
	return function_26a780(clump, &candidate);
}

// score 3 (was 3): the array cursor LEA precedes the initial JLE/MOVZX instead of following them.
// Earlier: retail caches the generation in ebx (reloaded after each mark store and call) and reloads the prop pool at each use; ours caches the prop pool in ebx and loads the generation at each use.
// frame 0xd8; entry generation scheduling, child-next register and count/end traversal differ; fresh destination head 171f; short plus fresh head 167 plain; short/end 216f; typed object views 192f; delayed source base 219f; source first 211f; masked slots 198f.
// Round 280: remaining-count traversal 128 plain; current-child handle 164; collection write cursor 172; array scope 167; early active flag 199.
// Child/countdown 125; short remaining 125; unsigned remaining 125 (each only three below the retained countdown).
// Frame pointer 198; depth zero 313; later generation increment 130; native source slot 175; size preference 360; first-walk generation snapshot 178.
// generation after destination 221; generation after head 126; read-only source 128; source reference 128.
// read-only copy 128; read-only first walk 128; ordinary prop inline 128; separate copy noinline 128.
// Entry group helper 128; fresh-head group helper 128; native prop field 128;
// prop handle at use 128; child extension at use 128; owner extension at use 130;
// native entry slots 219; direct populated-array index 160.
// Round 69: existing inline datum lookup in the collected pass 288 (generation then cached as retail, but the
// object query inlines there); short/long index loops 161/160; child next into the walking index 165;
// child next with an else arm 167.
// Round 247: generation comparison reversed / child for-header update / both 128/128/128; validation and cursor at latch, alone or with either/both 166/166/166/166; complete form with nullable child result 172. Generation/pool and child/countdown homes remain.
// Round 263: shared datum validation with object-query inline/global-noinline/scoped-noinline
// declarations 288/288/288; each with ordinary-inline prop iterator 288/288/288;
// inline/global-noinline query with positive validation blocks 288/288. Frame 0xdc; all restored.
// Round 299: shared member iterator, entry workspace/native prop-field subsets 130 each;
// generation after destination/source/head on that graph 224/120/138; source timing
// with complete twice-used mark slots 84 plain. Short/unsigned-long/word remaining
// 81/84/81; positive validation with latch cursor and long/short/unsigned-long/word
// remaining 80/3/80/3. Shared members, source timing, mark slots and short latch kept,
// 3 plain at 737/737 bytes; counter/cursor declaration placements, const cursor and
// joint copy-role names 3 each. All other forms restored.
// Round 304: collected-count consumption/postdecrement 3/3; cursor consumption alone/with count 81/81;
// word/long collection 4/12; populated-end traversal 85; collector write cursor 32.
// Original body restored; cursor LEA still precedes the positive-count branch and extension.
// Round 84: guarded signed/word do loops 40/93; comma and split for-initializers 3/3/3;
// equality countdown 3; plain while 3; guarded signed inequality do 40. Original body restored.
// @retail 0x269580
void function_269580(long destination_index, long source_index)
{
	s_clump_activity_view *destination = (s_clump_activity_view *)(g_502420->data + (destination_index & 0xffff) * sizeof(s_clump_activity_view));
	s_clump_activity_view *source = (s_clump_activity_view *)(g_502420->data + (source_index & 0xffff) * sizeof(s_clump_activity_view));
	g_4de2fc++;
	g_4de2f8 = true;
	s_iterator iterator;
	iterator.next = ((s_clump_activity_view *)(g_502420->data + (destination_index & 0xffff) * sizeof(s_clump_activity_view)))->first_prop;
	s_type_76cf92 *prop;
	while ((prop = clump_next_prop(&iterator)) != NULL)
	{
		long object_index = *(long *)((byte *)prop + 8);
		byte *object = (byte *)function_badc0(object_index, (dword)NONE);
		if (!object) continue;
		if (*(char *)(object + 0xaa) == 12 && *(long *)(object + 0x134) == 0)
		{
			byte *extra = object + *(short *)(object + 0x13a);
			if (extra && *(long *)(extra + 8) != NONE)
			{
				s_ai_object_iterator members;
				members.next_index = perception_get(*(long *)(extra + 8))->object_index;
				while (swarm_creature_iterator_next(&members) != NULL)
				{
					long member_slot = members.index & 0xffff;
					if (g_4de300[member_slot] != g_4de2fc)
						g_4de300[member_slot] = g_4de2fc;
				}
				continue;
			}
		}
		long object_slot = object_index & 0xffff;
		if (g_4de300[object_slot] != g_4de2fc)
			g_4de300[object_slot] = g_4de2fc;
	}
	long indices[50];
	short count = 0;
	iterator.next = ((s_clump_activity_view *)(g_502420->data + (source_index & 0xffff) * sizeof(s_clump_activity_view)))->first_prop;
	while (clump_next_prop(&iterator)) indices[count++] = iterator.index;
	short remaining = count;
	if (remaining > 0)
	{
		long *cursor = indices;
		for (; remaining > 0; --remaining)
		{
			long index = *cursor;
			if (index != NONE && (index & 0xffff) < g_50241c->high_water_index)
			{
				s_prop_copy_view *copy = (s_prop_copy_view *)(g_50241c->data + g_50241c->size * (index & 0xffff));
				if (copy->salt && copy->salt == (index >> 16))
				{
					long object_index = copy->object_index;
					if (function_badc0(object_index, (dword)NONE))
					{
						long object_slot = object_index & 0xffff;
						if (g_4de300[object_slot] != g_4de2fc)
						{
							g_4de300[object_slot] = g_4de2fc;
							long added = function_26a860(destination, copy);
							if (added != NONE) function_26a960((s_clump *)destination, added);
						}
					}
				}
			}
			++cursor;
		}
	}
	g_4de2f8 = false;
	if (*(real *)((byte *)source + 0x2c) > *(real *)((byte *)destination + 0x2c))
		*(real *)((byte *)destination + 0x2c) = *(real *)((byte *)source + 0x2c);
	*(byte *)((byte *)destination + 0x30) |= *(byte *)((byte *)source + 0x30);
	if (*(long *)((byte *)source + 0x34) > *(long *)((byte *)destination + 0x34))
		*(long *)((byte *)destination + 0x34) = *(long *)((byte *)source + 0x34);
	if (source->state > destination->state)
	{
		destination->state = source->state;
		destination->state_time = source->state_time;
	}
}

// @retail 0x26ace0
long function_26ace0(long object_index, long actor_index, short type)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;
	if (actor->unknown07c != NONE)
	{
		s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (actor->unknown07c & 0xffff) * sizeof(s_clump_activity_view));
		long prop_index = function_26b120(actor->unknown07c, object_index);
		if (prop_index == NONE)
		{
			prop_index = function_26a740(object_index, clump);
			if (prop_index != NONE)
			{
				((s_prop_copy_view *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_prop_copy_view)))->field0c = type;
				function_26a960((s_clump *)clump, prop_index);
			}
			else
			{
				return NONE;
			}
		}
		s_iterator nodes;
		nodes.next = ((s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop)))->first_node;
		s_clump_node *node;
		while ((node = clump_next_node(&nodes)) != NULL)
		{
			if (node->actor_index == actor_index)
			{
				result = nodes.index;
				break;
			}
		}
	}
	return result;
}

void function_269250(long clump_index, long actor_index);

// score 87 (was 87): retail hoists the walk's pool load into ebx and keeps the selection
// only on the stack (stored from esi, best stored in a tail shared with the initialisation);
// ours keeps the pool in memory and the selection in edi.
// Earlier: retained frame pointer 187; depth zero 304; selected-result reuse 134; team snapshot 133;
// size preference 338; double candidate/both scores 157/158; activity while 160; role-based scalar
// names 134; inline depth two 134; the clump datum walk with a cached activity flag 126 (retail's frame);
// selection stored before the best score 126; selection from the iterator's datum index 128 (selection
// on the stack and pool hoisted, but the null-current join is threaded); creation arm first 130;
// walk declared after the selection locals 129; actor index local 126; early continue on a
// non-improving score 128; declaration orders, scopes, cached or global activity flag, team test as
// continue or nested if, and inline/forceinline walk helpers all compile identically.
// Common score path: zero default with branch/selection-first 92/92, ternary 125/125;
// best default with branch/selection-first 89/89 fast, ternary 128/128. Retained
// branch score initialized from best, 87 plain; pool/selection and x87/SSE homes remain.
// Round 247: common-graph scalar homes 89; wide best/names 175/175; wide candidate/names 146/146; both wide/names 169/169; homes with existing center setter 111. Pool and scalar/x87 allocation remain.
// Round 84: score computed and compared inside the team arm 126 (retail's three homes and x87 compare,
// selection still split into EDI); with the selection stored first or names best_index/best_score 126
// (same code); selection from the walk's datum index 128 (selection memory-only, pool hoisted, but the
// null-current join threads away and the homes reverse); common path with the datum index 146.
// Round 422: persistent score 128; native selection handle 146; persistent score/native handle
// 152; actor handle snapshot 89; persistent score/actor handle 128; native/actor handles 146;
// all scan lifetimes 152; function-scope candidate 128. Opening body restored.
// Round 31: in-arm compare with datum-index selection 128 (pool hoisted into EDI, selection memory-only,
// but the exhaustion null test threads away and the homes rotate); with best declared first 128; score
// local renamed 128 (slots do not follow names here); in-arm compare with clump-index selection 126;
// each with the walk at function scope 128/126.
// Round 83 (fast; unchanged control 89): two-arm score assignment 128; zero-initialized two-arm
// assignment 128; reversed strict comparison 89; separate best publication 89; old-clump local
// after/before the scan 141/147; flattened selection/creation tests 89; byte activity snapshot 89.
// Round 129: selection stored before the best score (retail cross-jumps the best store with the
// initialisation's) 89 (aligned 54 -> 58); in-arm compare with that order 126 (480 B, aligned 60;
// retail's x87 compare, but the found block is laid out after the improve block and the selection
// stays in EDI after the loop); with the iterator's datum index 128 (pool hoisted and the shared best
// store as retail, but the not-found path threads to the exit and the score/best/selection homes are
// 0x14/0x18/0x1c against retail's 0x1c/0x18/0x14 reversed); best, score, selection declared in that
// order, either selection 128/126 (homes unchanged).
// Round 107 (fast, with 0x269580/0x269040 in the build; without them 0x269580's arguments swap):
// the team-arm form gives retail's homes (selection 0x14, score 0x18, best 0x1c) and x87 compare,
// 126; a memory-only selection (diagnostic, reverted) shows the pool then takes EBX. On the
// team-arm form: selection locals before the walk 129; activity read in the walk call 126; team
// compare reversed 126; the datum step reading the pool field at each use (data_iterator.h) 126,
// identical. Zero-default score with the common compare 90.
// Round 29 (plain checks): the in-arm compare 126 (480 B; plain agrees with fast), with the selection
// from the iterator's datum index 128, with the selection stored first 126; the walk as 0x26bc60's
// s_clump_pool_iterator (current cleared and set each step, selection from pool.datum_index) with the
// in-arm compare 144, with the common compare 138; the created clump through the existing clump_get
// 87 (same code), with the in-arm compare 126. A record-pool walk without the datum field is ruled out:
// retail's 0x268510 passes this walk's iterator to the out-of-line 0x6b380.
// Round 250: Native actor/group output ownership, then native selected-handle combinations:
// 104/146/147/147/146/147; both complete-output forms with team-local scoring 129/129.
// Pool/selection and x87/SSE allocation remain; all matched controls survive.
// Round 31 (lane 15, fast): in-arm compare with the selection stored first, with `continue` on a team mismatch,
// with `best < score`: 126 each (480 B); `(void)&selected;` on either form identical (87/126). Diagnostics:
// the score call replaced by a constant (143) or the post-loop selection use removed (220) both hoist the pool
// into a register and leave the selection in memory as retail: the selection wins the callee-saved register
// only through the call in the walk plus its post-loop uses.
// Round 116 (lane 8, fast with 0x269580/0x269040; the unchanged body gives 89 there): in-arm x87 compare with the
// selection stored first 126; with the selection from `groups.iterator.datum_index` 128 (pool promoted to EDI and the
// selection memory-only as retail, but the walk's join loses the clump-index copy, so the not-found path threads to
// the exit and the homes rotate); the walk struct declared once outside the actor loop 126 / 128; selection and best
// declared before the walk 129.
// Round 51 (lane 10, fast): one `clump_index` local for both the selected and the created clump 89 (482 bytes), with
// the in-arm compare 126 (480); that with the selection stored before the best score 89, in-arm 126; the in-arm
// compare with the score declared at the top of the walk body 126; the group declared in the while condition 89.
// Retail never gives the selection a register (stored from ESI in the walk, reloaded after the post-loop calls,
// unlike the created index in EDI), so the walk's pool field is scalarised into EBX; ours gives EDI to the selection.
// Round 193 (lane 3, plain): the in-arm compare gives retail's 480 bytes and homes (126), but our found
// block is laid out after the improve block (0x26ae30's identical walk keeps retail's order); a
// memory-only selection (diagnostic, reverted) gives the pool EBX but promotes the walk index into EBP
// and sends the actor to the stack, 145. On the in-arm form: the walk reading g_4f55d0->active at each
// step 126 (same code); score declared per actor 126; activity snapshot before the walk init 126; the
// creation arm assigning the selection 126 (same code); creation arm first 130.
// Round 34 (plain checks): in-arm compare with the selection stored first 126; on it `actor->unknown07c != selected`
// 126, best declared and stored first 126 (both same code), the team read once into a `short` before the scan 168
// (497 bytes), creation arm first 130.
// Round 18 (m3 lane 18, fast with 0x269580/0x269040/0x269250/0x2694d0/0x268bb0; this body is 89 fast): the walk's
// scan (data_next_absolute_index_inlined, data_array.h) returning from inside its loop: this body 89, the in-arm
// compare 126, the in-arm compare with the iterator's datum index 128 (all three unchanged code: the walk already
// threads as retail); in-arm compare with `if (score <= best) continue;` 128, with the team test as a `continue`
// too 128. Re-measured: in-arm with the selection stored first 126 (480 B; found block placed after the loop body,
// selection in EDI, best stored in the improve block), with the iterator's datum index 128 (pool hoisted and the
// selection memory-only as retail, but the not-found path threads to the exit and the homes reverse). Plain: the
// in-arm search as a forceinline function returning the selection 126 (480 B); the in-arm compare with the selection
// and best declared once for the whole function and reset per actor 126. The fast image swaps 0x269580's two
// argument registers (0x269580 matches only in the full image): read this function's call setup from plain checks.
// @retail 0x2689d0
void function_2689d0(void)
{
	s_actor_iterator actors;
	function_x66da2b(&actors, true);
	s_actor_view *actor;
	while ((actor = (s_actor_view *)function_1e46c0(&actors)) != NULL)
	{
		s_clump_datum_walk groups;
		clump_datum_walk_new(&groups);
		bool active = g_4f55d0->active;
		long selected = NONE;
		real best = 0.0f;
		s_clump_activity_view *group;
		while ((group = (s_clump_activity_view *)clump_datum_walk_next(&groups, active)) != NULL)
		{
			real score = best;
			if (group->team == actor->unknown024)
				score = function_268bb0(group, actor);
			if (score > best)
			{
				best = score;
				selected = groups.clump_index;
			}
		}
		if (selected != NONE)
		{
			if (selected != actor->unknown07c)
			{
				if (actor->unknown07c != NONE)
				{
					function_269580(selected, actor->unknown07c);
					function_2694d0(actor->unknown07c, actors.actor_index);
				}
				function_269250(selected, actors.actor_index);
			}
		}
		else
		{
			long created = function_269040(&actor->position, actor->unknown024);
			if (created != NONE)
			{
				if (actor->unknown07c != NONE)
				{
					s_clump_activity_view *group = (s_clump_activity_view *)(g_502420->data + (created & 0xffff) * sizeof(s_clump_activity_view));
					group->center = actor->position;
					function_269580(created, actor->unknown07c);
					function_2694d0(actor->unknown07c, actors.actor_index);
				}
				function_269250(created, actors.actor_index);
			}
		}
	}
}

/* adds an actor to a clump: links it into the clump's actors, moves the actor's prop
   nodes onto the clump's props and gives it a node for each clump prop it lacks */
// @retail 0x269250
void function_269250(long clump_index, long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	s_clump_activity_view *clump = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
	clump->count++;
	function_26b1d0(actor_index);
	actor->unknown07c = clump_index;
	actor->next_index = clump->first_actor;
	clump->first_actor = actor_index;
	if (actor->unknown009 && !clump->active)
	{
		s_clump_activity_view *updated = (s_clump_activity_view *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump_activity_view));
		updated->active = true;
		updated->update_state = 1;
	}
	if (*(bool *)((byte *)actor + 0xc))
		*(bool *)((byte *)clump + 0x3c) = true;
	g_4de2fc++;
	g_4de2f8 = true;
	s_iterator nodes;
	nodes.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = clump_member_prop_next(&nodes)) != NULL)
	{
		long object_slot = node->object_index & 0xffff;
		if (g_4de300[object_slot] != g_4de2fc)
			g_4de300[object_slot] = g_4de2fc;
		long prop_index = function_26b120(clump_index, node->object_index);
		if (prop_index != NONE)
		{
			s_clump_prop *prop = (s_clump_prop *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_clump_prop));
			s_clump_link_view *link = (s_clump_link_view *)prop_ref_get(nodes.index);
			long first = prop->first_node;
			if (first != NONE)
				((s_clump_link_view *)prop_ref_get(first))->previous = nodes.index;
			link->next = first;
			prop->first_node = nodes.index;
			link->prop_index = prop_index;
		}
		else
		{
			function_26abd0((s_clump_link_view *)node);
			if (node->view_index != NONE) record_pool_release(g_502414, node->view_index);
			record_pool_release(g_502418, nodes.index);
		}
	}
	s_clump_prop_walk props;
	props.iterator.next = ((s_clump *)(g_502420->data + (clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
	while ((props.prop = clump_next_prop(&props.iterator)) != NULL)
	{
		long slot = *(long *)((byte *)props.prop + 8) & 0xffff;
		if (g_4de300[slot] != g_4de2fc)
		{
			g_4de300[slot] = g_4de2fc;
			long index = function_26a8d0(props.iterator.index);
			if (index != NONE)
			{
				s_clump_prop *current = (s_clump_prop *)(g_50241c->data + (props.iterator.index & 0xffff) * sizeof(s_clump_prop));
				s_clump_link_view *link = (s_clump_link_view *)prop_ref_get(index);
				long first = current->first_node;
				if (first != NONE)
					((s_clump_link_view *)prop_ref_get(first))->previous = index;
				link->next = first;
				current->first_node = index;
				link->prop_index = props.iterator.index;
				function_26aa40(actor_index, index);
			}
		}
	}
	g_4de2f8 = false;
}

// score 89 (was 89): frame 0x24 agrees; category-byte homes and deletion completion differ. With the removed
// prop cleared and tested (3) only the hoisted prop/node pool registers differ: retail loads the prop pool
// first into edi, ours always loads the node pool first; an extra prop lookup (93) or an index node loop (76)
// gives retail's registers but keeps the node-first load order.
// Earlier: The 3 form clears the removed prop and tests it before the node scan (a set-and-test
// frame 0x24 agrees; category-byte homes, removal completion and pool carriers remain.
// Earlier rejected pointer-clear diagnostic: The 3 form clears the removed prop and tests it before the node scan (a set-and-test
// flag, kept only on a match; build/r74/26b230.best3.cpp in lane 5): there only the two pool registers
// differ: retail keeps the prop pool in edi and the node pool in ebp; ours swaps them. Start there.
// Round 74 (from 189): tail as a record-pool iterator walk, clump loop through the shared datum walk,
// members through swarm_creature_iterator_next, unit set only when the mask test passes, the removed
// prop cleared and the node scan guarded by it, flags/replacement declared first, together 3.
// Then: first-node lookup through prop_get 3; node body through prop_ref_get 180; node step through
// prop_ref_get 3; prop step reading the pool directly 3; swarm flag tested before the prop 185;
// `continue` member loop 346; changed/unit in the prop loop condition 181; renamed swarm flag 89 (at 89);
// a 600 s permuter run stopped by the harness before reporting.
// Before round 74 (old body): frame 0x1c versus 0x24; current/replacement/object homes and iterator exit differed.
// Byte type 195; membership result earlier 189; existing group lookup 189; removal arm first 189; guarded pointer-result loop 194.
// Complete workspace scope 189; depth zero 391; member-link result first 222; replacement-result loop 206; promoted category 195.
// Depth one 189; outer datum API 290; word unit 218; word group 192; calling scan 237.
// Direct pool step 254; activity-controlled loop 194; existing prop lookup 189; short/signed byte change 244/189; cached-bound scan 189.
// node datum type 189; shared object header 189; forced existing scan 188; self-member skip 358;
// final tracking guard 189; complete unit arms 193; unsigned replacement 189; word category mask 192.
// Read-only object/external object reference/read-only member/extensions 189/189/189/189.
// Ordinary prop/node/both iterator inline and separate removal noinline 189/189/189/189.
// Frame/current/object/replacement homes remain; unchanged result 189;
// changed-reference count 246; category at uses 195; member header getter 189; removal
// inline/forced expansion 189/460; complete unit predicate 243; count plus removal boundary 246.
// frame/current/object/replacement homes remain.
// Earlier: Frame/current/object/replacement homes remain; unchanged result 189;
// Earlier: frame/current/object/replacement homes remain.
// Shared datum/calling walks 216/188; with group getter 216/188; native iterator index 189/237;
// current-head/read-only object 241/216. Shared walk recovers frame 0x24 at 216/765 bytes.
// On that frame: member while 216; tested category 219; both 219; category names 216;
// names with while/test/both 216/219/219. One-point calling-walk gain rejected; body restored.
// Round 368: non-unit bool/byte 219/219; non-group bool/byte 190/190;
// joint absence domains bool/bool, byte/byte, bool/byte, byte/bool 220/220/220/220.
// Round 373: shared member walk ordinary/inline/forced 183/183/183; entry member
// workspace with the same boundaries 183/183/183; uniform original-object reads
// with ordinary/forced member walk 243/243. Six-point gain restored.
// Current-member identity before lookup 204; successor published on continuation 200;
// original-object accessor at every use 262; identity/successor, identity/accessor, successor/accessor,
// all three 200/261/262/262; all with conditional unit admission 263. Original body restored.
// Round 211: change-result gate before each prop lookup, on the shared-member/frame-0x24 body,
// with original/reversed category declarations and native node/getter subsets: 111 four times /
// 109 four times; both reach 741 bytes. Original traversal and completion tests restored.
// DL159: single iteration-completion label 3; separate completion guards 3; reversed category declarations 5; category name/native node datum/node iterator/both iterator names 3/3/3/3; native node with reversed categories 5. Labels rejected as branch layout.
// Structured unit continuation, with separate completion guards and native byte unit/change domains in all eight combinations, 89 each. Original body restored.
// Round 104, on the 3 form: group flag before the cleared prop 185; a removed flag for the cleared prop 3;
// prop step defined before the node step 3; pool struct fields swapped (diagnostic) 3; prop-loop body
// reading the prop at its top 136; 600 s permuter, 66 tries, none below 3. Original body restored.
// Round 276: Unchanged-result polarity on the recovered frame, crossed with byte result, native node
// Round 276: datum and complete category identities, 94 in all eight forms. Original result, traversal
// Round 276: and body restored; category homes, removal completion and pool-register roles remain.
// Native repeated state pointer/reference, crossed with the existing node-head getter
// and chained adjacent flag clears: all eight 89. Native object fields retain the same offsets.
// Round 249: Complete deletion/live-prop arms with common completion, ownership/replacement
// comparison priority and original/reversed category declarations 196/144/197/146;
// changed published after deletion 196/178/197/180. All exact-only forms restored.
// Round 299: chained category initialization in both assignment orders, with
// invariant-first completion and native node-datum subsets: 87/89/87/89/87/89/87/89.
// Two-count forms restored; category homes and deletion/pool scheduling remain.
// Round 433: Typed current clump and changed-reference count on frame 0x24, signed/unsigned with
// original/invariant-first completion and direct/shared head; fast
// 230/230/229/229/230/230/229/229.
// Original body and existing helper restored.
// Round 107: the pool registers follow how g_502418/g_50241c are declared. With the 3 form and the
// two pools as plain separate globals (either declaration order, or one of them in a one-field
// struct) this function matches byte for byte, but 0x25d770 (0 -> 2) and 0x25d810 (0 -> 91) then
// load the node pool first (the other 717 functions of the 46 pool files unchanged); with
// s_prop_pool_storage those two match and this one keeps the node pool in edi. Also 3: the struct
// with the tracking pool (0x502414) or the clump pool (0x502420) added, the struct defined in a
// /GL- file, the prop step reading the pool twice. The swarm flag renamed (slot order) 89, identical.
// Round 31: on the 3 form, prop loop as an explicit index loop with the lookup in its body 189 (the pools
// are no longer hoisted); target-clear arm through prop_get at each use 220; the existing prop walk
// struct 3; node iterator declared beside the prop iterator 3. On the opening body the deletion arm as
// `if (unit) continue; break;` 89 (identical code).
// Lane 16 r48 (fast): the 3 form under s_prop_pool_storage leaves only the pool registers (ours node pool
// EDI / prop pool EBP; retail the other way). Under separate pool globals the 3 form matches but
// 0x25d770 2 and 0x25d810 91 (335 B; result/actor registers permute): 0x25d770's walk as a for loop 114
// (208 B, the step is duplicated); the prop pool defined in this file instead of globals.cpp 2/91; prop
// declared before datum in 0x25d770 2/91; both pools as one two-element array 252 (709 B: the pools are
// no longer hoisted; 0x26aaf0 0 -> 70). Ours gives the pool the loop references first the first
// callee-saved register (reverse order with the struct); retail gives the prop pool it in all three.
// Lane 14 r22: unit flag renamed to sort before the group flag 89 (slots unchanged); the completion
// test as the prop loop's condition (`(!changed || unit) && next`, the removal arm `continue`) 111
// (741 B, but the entry is not threaded past the test); the removed flag rechecked 3 (both flag slots
// then agree; only the prop/node pool roles in EDI/EBP differ, set by s_prop_pool_storage, which
// matched 0x25d770 needs); with the removed test last 190; set before the removal call 3.
// Round 29 (fast, 3 form with separate pools, which matches here): a probe reading the prop pool first
// at the top of unknown_25d690.cpp 2/91 (0x25d770/0x25d810); separate pools named to sort prop first or
// last 2/91 each (names do not rank promoted globals); 0x25d770's prop lookup at each of its three uses
// 55 (the prop pool then takes EDI and loads first as retail, but the lookup sinks under the tests: the
// promoted pools rank by reference count, ties by first reference); the final actor compare through a
// second prop_get in both 49/115; both actor reads through it in 0x25d770 55/91.
// Round 250: Foreign-state clear before owned-prop handling: replacement-first with category-
// order/native-node subsets 123/123/121/121; removal-first 98/98/96/96. Category homes and
// deletion/pool scheduling remain; all matched controls survive.
// Round 116 (lane 8, fast): the 3 form (cleared prop) re-checked 3: the hoisted loads are in place and only which
// pool gets EDI/EBP differs (ours node EDI / prop EBP). Separate pool globals with the 3 form: this function matches,
// 0x25d770 2 and 0x25d810 91, unchanged by the definition order in globals.cpp or the extern order in globals.h. Under
// separate globals the first-referenced pool takes the first register (the node pool in 0x25d770/0x25d810); under the
// struct the other one; retail gives the prop pool the first register in all three. The node walk through the out-of-
// line 0x26be30 4 (wrong link field, +0x2c for +0x34; not a valid form).
// Round 193 (lane 3, fast): the 3 form under s_prop_pool_storage 3; with separate pool globals it
// matches but 0x25d770 2 and 0x25d810 91, also with the prop pool defined first in globals.cpp, declared
// first in globals.h, grouped with the clump pool (one struct, node separate) or the node pool grouped with
// the tracking pool: 0/2/91 each. Whenever the two pools are separate symbols 0x25d770/0x25d810 give the
// node pool the first register (one use each, node referenced first); as one symbol 0x26ae30 does (node
// referenced last); retail gives the prop pool the first register in all three. The 3 form's cleared prop
// is a flag set and tested, kept only on a match: a shared-declaration conflict with matched 0x25d770 and
// 0x25d810, not a body problem. The 3 form with the node scan started from the prop pointer 208 (709 B).
// Round 482: Independent category false-initializer lifetimes at entry/object/type
// boundaries, eight nonoriginal pairs 91/91/90/91/91/90/91/91. Original entry
// declarations restored; category-byte homes and deletion/pool completion remain.
// @retail 0x26ae30
void function_26ae30(long object_index)
{
	bool group_object = false;
	bool unit = false;
	long replacement = NONE;
	s_slot_object_view *object = object_get(object_index);
	short type = (char)object->type;
	if (type == 12)
	{
		if (*(long *)((byte *)object + 0x134) == 0)
		{
			byte *data = (byte *)object + *(short *)((byte *)object + 0x13a);
			if (data)
			{
				long perception = *(long *)(data + 8);
				group_object = true;
				if (perception != NONE)
				{
					s_ai_object_iterator members;
					members.next_index = perception_get(perception)->object_index;
					while (swarm_creature_iterator_next(&members) != NULL)
					{
						if (members.index != object_index)
						{
							replacement = members.index;
							break;
						}
					}
				}
			}
		}
	}
	else if ((1 << type) & 3)
		unit = true;
	s_clump_datum_walk groups;
	byte *group;
	clump_datum_walk_new(&groups);
	while ((group = clump_datum_walk_next(&groups, g_4f55d0->active)) != NULL)
	{
		bool changed = false;
		s_iterator props;
		props.next = ((s_clump *)(g_502420->data + (groups.clump_index & 0xffff) * sizeof(s_clump)))->first_prop;
		s_type_76cf92 *prop;
		while ((prop = clump_next_prop(&props)) != NULL)
		{
			if (*(long *)((byte *)prop + 8) == object_index)
			{
				changed = true;
				if (replacement != NONE)
					*(long *)((byte *)prop + 8) = replacement;
				else
				{
					function_26aaf0((s_clump *)group, props.index);
					if (!unit) break;
					continue;
				}
			}
			else if (prop->state.unknown3c == object_index)
			{
				prop->state.unknown3c = NONE;
				prop->state.unknown66 = false;
				prop->state.unknown65 = false;
			}
			if (group_object && prop->unknown22)
			{
				s_iterator nodes;
				nodes.next = ((s_clump_prop *)(g_50241c->data + (props.index & 0xffff) * sizeof(s_clump_prop)))->first_node;
				s_clump_node *node;
				while ((node = clump_next_node(&nodes)) != NULL)
				{
					if (((s_prop_datum *)node)->object_index == object_index)
					{
						((s_prop_datum *)node)->object_index = replacement;
						changed = true;
					}
				}
			}
			if (changed && !unit) break;
		}
	}
	if (object->type == 1)
	{
		s_record_pool_iterator trackings;
		trackings.data = g_502414;
		trackings.index = NONE;
		byte *tracking;
		while ((tracking = data_iterator_next_inlined(&trackings)) != NULL)
		{
			if (*(long *)(tracking + 0x40) == object_index)
				*(long *)(tracking + 0x40) = NONE;
		}
	}
}

struct s_2640c0;
bool function_25ccd0(s_type_5cfb45 *state, long object_index, short value, s_2640c0 *motion);

// @retail 0x26a210
void function_26a210(long clump_index)
{
    s_iterator iterator;
    iterator.next = clump_get(clump_index)->first_prop;
    s_type_76cf92 *prop;
    while ((prop = clump_next_prop(&iterator)) != NULL)
    {
        long object_index = *(long *)((byte *)prop + 8);
        s_object *object = function_badc0(object_index, 3);
        if (object)
        {
            prop->unknown26[0] = (bool)((*(dword *)((byte *)object + 0x134) >> 7) & 1);
            if (prop->unknown23)
                *((bool *)prop + 0xbf) = (bool)((*(dword *)((byte *)object + 0x134) >> 8) & 1);
        }
        if (g_470f10[*(short *)((byte *)prop + 2)].kind == 0 && prop->unknown04 == 1)
            function_25ccd0(&prop->state, object_index, NONE, NULL);
    }
}

int __cdecl function_269da0(void const *a, void const *b);
void bitset_or(dword const *a, dword const *b, dword *destination, long bit_count);
void *actor_get_character_situation_entry(long actor_index);

// score 71 (was 71): the cluster loop (hoisted reset word in SI, NONE in EDI), the marking loop and the
// second selection loop now equal retail; left: the prop-loop head (ours loads the count before the prop
// pool and keeps the pool in EDI where retail has EDX) and the first selection loop's `add ebx, 0x10`,
// where retail re-derives the candidate from the cursor (doing that here drops the reset hoist, 301).
// Plain checks only: --fast drops the reset hoist that the plain build keeps.
// Round 29: the existing bitset_or for the hand-written OR loop 252 (aligned 139 -> 90); with the marking
// slots as 0x269580/0x269250 write them 178; the first loop as `while (selected < 50 && cursor < count)`
// with the second loop's candidate re-derived from the cursor 71 (kept). On 178: that condition with the
// candidate re-derived after `++cursor` in both loops 301, in the first only 308, the condition alone 316
// (1310 B); the second loop's re-derivation alone 173; the condition with the candidates taken at the tops
// of both loops 297; re-derivation in both with the first loop's break kept 180 (1334 B). On 71:
// s_clump_prop_walk for the prop loop 71 (same code); the first loop's step as `candidate = candidates
// + cursor` 301 (the reset hoist drops as with `&candidates[cursor]`); the second loop indexing
// `candidates[cursor].prop_index` with no pointer 81. Fast diagnostics (reverted): an early reset local
// as long stored as (short) 26 on the both-loop form (aligned 17), as short/word 275; the structure
// global at the compare only 302; a local s_location copied in the arm 336.
// Earlier: Frame 0x6a4 agrees; retail hoists the reset word into SI and NONE into EDI for
// the cluster loop and reads the cluster count twice (ours keeps one read live), recomputes each
// candidate from the short cursor (`movsx ebx, di; shl ebx, 4; lea ebx, [esp+ebx+0x74]`) and leaves the
// first selection loop unrotated; visibility/marking registers follow.
// frame 0x6a4 agrees; reset-word/cursor allocation and selection/cleanup placement differ.
// frame 0x6a4 agrees; reset-word/cursor allocation and selection/cleanup layout differ; typed cluster view 322f; isolated cluster iterator 322f; unsigned cursor 328f; unsigned selected 322f; case-result assignment 391f; existing first 347f; indexed cleanup 351f.
// Round 280: full-width reset snapshot 373; current-child handle 322; late cursor 350; late selected total 335; ascending word merge 374.
// Round 156, caller context: depth zero 505; depth one 322; target size 517;
// wide cluster selectors 356; retained generation slot 322. Original body retained.
// round 312: reset/cursor allocation remains; selected byte 332; generation publication first 338; short visibility count 355; cluster-count snapshot 374; shared iterator initializer 322.
// Frame 0x6a4 agrees; reset-word SI, marking registers and selection/cleanup placement remain. Existing creation helper 322; exhaustive successor 324; native clump heads 373; generation value 332; shared state fallthrough 322; visibility pointers 374.
// Scoped first distance iterator 322; two-use candidate-distance sample 326; readonly
// settings view 322; first scope plus sample 326; separate marking result 322; first
// scope plus marking result 322; chained reset fields 322, plus marking result 322.
// Live clear/admission/both 322/315/315; live row/clear-row/admission-row/all 315/383/322/356;
// candidate address before sort 356. Live admission bound 315 (the global re-read at one of four uses; not kept);
// early reset-word SI load recovered; all call tuples and existing merge/selection loops retained.
// Round 342: candidate-count lifetime at collection 383; invalid-cluster reset first 322;
// fallback initialization first 322; observed-state CRT copy 322; shared visibility test 322.
// Readonly candidate 322; priority/type/both samples 331/357/369; readonly samples 369; selected threshold first 372; threshold with samples 361; readonly structure 322.
// Round 368: joint selection-phase counter lifetime, short/long cursor and total
// 360/352/361/352; the same four with indexed candidate advancement 382/372/365/371.
// Round 373: shared marking-chain walk ordinary/inline/forced 296/296/296;
// entry chain workspace with the same boundaries 296/296/296; uniform structure
// observations with ordinary/forced walk 331/331. Ordinary shared walk kept.
// Round 144: existing complete actor/location/both views 296f each; cluster field
// observations alone or with actor/location/both 296f each; both views and readonly
// visibility row 296f. All restored; early reset-word and later allocation remain.
// Round 183: Backward visibility source/destination/both pointer induction 331/322/358;
// four default-position permutations 296/296/296/296; both pointers with default first 358.
// Original merge and dispatch restored; reset-word SI and later cursor/selection homes remain.
// Round 351: native short count/cursor/selected home names in all seven combinations
// 296 each; all scalar-home names with structure/distance roles 296. Original names restored.
// Round 192: actor/prop/node iterator lexical homes in all seven combinations 296 each;
// complete native iterator/array homes 296. Frame 0x6a4, early reset-word SI and later
// visibility/selection allocation unchanged; original names and all code restored.
// DL round 156: native member/object generation ordinals word-word, word-long, long-word,
// long-long, dword-long, long-dword, dword-word, word-dword
// 311/323/323/299/299/299/323/323. Shared walk and original observations restored.
// Round 353: on the shared marking walk, reset snapshots short/long/word
// 357/356/357; short/long with chained sentinels 357/356; short/long
// with long actor cluster 359/362; all three 359. Original observations restored.
// Native word dispatch zero-first/direct field 283/283; one-first 354; unsigned
// eligibility 342. Early state result with those forms 323/323/325/329. Guarded
// nonzero word dispatch 283 retained; original state-result lifetime kept.
// Round 27: actor-step/getter expansion boundaries, ordinary/inline/forced
// in all eight non-original pairs [283,283,283,283,283,283,283,283]. Original
// declarations and bodies restored; early reset SI and later homes remain.
// Round 167: existing absent-leaf location initializer 283; visibility cluster,
// row-count and row-pointer scalar homes in all seven combinations 283 each.
// Original reset and names restored; early reset SI and later allocation remain.
// Round 250: existing native structure count 283; with per-iteration candidate,
// short/long cursor and selected carriers 384/336/383/337;
// corresponding direct indexed fields 374/336/375/337. Original body restored.
// Round 29: Real distance member and entry cluster/candidate array declarations in all seven combinations, then the entry distance workspace: 283 each. Original publication and declaration lifetimes restored.
// Round 383: shared existing location reset ordinary/inline/forced/outlined 283/283/283/346;
// with consistently retained clump heads 391/391/391/411. Original helper linkage, both original
// helper callers, root observations and complete executable body restored.
// Round 386: real actor/prop/node traversal workspace reuse for each pair/all,
// then the same four with the actual uninitialized query workspace at entry
// 283/283/283/283/283/283/283/283; original independent workspaces and body restored.
// Round 244: Five alternate location-reset field orders 283 each; reset-word-first with either sentinel
// chain 283/283; reset between sentinels with invalid-cluster-first merge 283. Early reset SI and
// visibility/selection homes remain; original field publications restored.
// Round 90: `while (selected < 50 && cursor < count)` for the first selection loop 423 (retail's
// unrotated head, but the frame drops to 0x6a0: the clump offset gets EBX instead of a spill); with
// `&candidates[cursor]` per iteration 396; with the candidate reassigned after `++cursor` in both loops
// 372 (aligned 202 -> 161, 1308 bytes); that reassignment alone on the original loops 388 (frame 0x6a0);
// on the 372 form with the structure global read at every use 368. Retail's extra pressure is the
// hoisted reset word/NONE pair; ours never hoists them.
// Round 391: Loop-local produced object pointers in plain for initializers/steps,
// cluster/marking/prop in seven subsets 397/389/439/401/478/458/573;
// all three with the object query 653. All step returns tested; original body restored.
// Round 74: live admission 293; entry reset 348; post-clear reset 284; both-loop
// recomputation with live admission 318; selection-only 325 and cleanup-only 287 with live
// admission; both-loop recomputation with entry reset 353; adding live admission 316.
// Original executable body restored.
// Round 84: reset word named before the distance store 351 (the load moves to retail's spot, registers
// shift); one iterator per actor loop 283 (same code); structure global at each use 318; named stamp
// index in both marking arms 319 (retail's direct marking addresses, later loops misaligned by padding);
// with `while (selected < 50 && cursor < count)` as well 326.
// DL round 206: candidate evaluator/collector inline modes 1/0, 0/1, 1/1, 2/0,
// 0/2, 2/1, 1/2, 2/2 (ordinary, inline, forced) 283/283/283/1749/459/1749/459/3173.
// Forced evaluator expansion also loses matched neighbours; all declarations and body restored.
// Round 250: Complete candidate values by construction/assignment/memcpy/memmove 317/317/317/426;
// with native cleanup-entry copy 388/388/388/445. Early reset-word and visibility/selection
// allocation remain; all matched controls survive.
// Round 116 (lane 8, plain): first loop re-deriving `&candidates[cursor]` 301 (re-check); that plus a short local
// of g_4686c4 before the distance store (diagnostic, one-use local of a global) 275: the cluster loop is then retail's
// except `mov esi, dword ptr` for retail's `mov si, word ptr`, and the marking loop swaps EBX/EDI; cursor declared
// first 301; both selection loops indexing `candidates[cursor]` with no pointer 233 (1365 bytes); the second loop as
// `for (; cursor < count; ++cursor)` over the array 293.
// Round 193 (lane 3, plain; 283 -> 71 before this round): both selection loops re-deriving the candidate
// after `++cursor` give retail's selection loops exactly but 301: our allocator then promotes g_502420 into
// EDI across the cluster loop and drops the bsp-word/NONE hoists, and swaps the marking loop's walker/object
// table registers. Adding g_4686c4 read into a long local before the distance store 26 (not kept: a one-use
// early read; its movsx can never equal retail's compiler-hoisted 16-bit `mov si`), as short 275 (a 32-bit
// load). On the both-loop form: candidate declared at the top 301; bitset/candidate arrays declared at the
// top 301; one iterator per actor loop 301; the cluster loop's first actor through the clump pointer 372.
// Round 482: Stable selection-count snapshots short/long after sort 197/313, before sort
// 203/315; with first-loop indexed candidate advancement 314/171/313/184.
// Caller-inclusive fast control 71; every snapshot regresses. Original escaped count restored.
// @retail 0x269870
void function_269870(long clump_index)
{
    s_clump_activity_view *clump = (s_clump_activity_view *)clump_get(clump_index);
    byte *structure = (byte *)g_4e0348;
    short count = 0, cursor = 0, selected = 0;
    real distance = 50.0f;
    s_iterator actors;
    actors.next = clump->first_actor;
    while (clump_next_actor(&actors) != NULL)
    {
        byte *settings = (byte *)actor_get_character_situation_entry(actors.index);
        if (settings && *(real *)(settings + 4) > distance)
            distance = *(real *)(settings + 4);
    }
    *(real *)((byte *)clump + 0x2c) = distance;
    dword clusters[16];
    memset(clusters, 0, ((*(long *)(structure + 0x9c) + 31) >> 5) * 4);
    actors.next = clump_get(clump_index)->first_object;
    s_actor_view *actor;
    while ((actor = clump_next_actor(&actors)) != NULL)
    {
        short cluster = *(short *)((byte *)actor + 0x254);
        if (cluster >= 0 && cluster < *(long *)(structure + 0x9c))
        {
            bitset_or(clusters, *(dword **)(structure + 0x58) + ((*(long *)(structure + 0x9c) + 31) >> 5) * cluster,
                clusters, *(long *)(structure + 0x9c));
        }
        else
        {
            *(long *)((byte *)actor + 0x250) = NONE;
            *(short *)((byte *)actor + 0x254) = NONE;
            *(short *)((byte *)actor + 0x256) = g_4686c4;
        }
    }
    ++g_4de2fc;
    g_4de2f8 = true;
    actors.next = clump_get(clump_index)->first_object;
    while ((actor = clump_next_actor(&actors)) != NULL)
    {
        if (*((bool *)actor + 7))
        {
            s_ai_object_iterator members;
            members.next_index = perception_get(*(long *)((byte *)actor + 0x1c))->object_index;
            while (swarm_creature_iterator_next(&members) != NULL)
            {
                long member_slot = members.index & 0xffff;
                if (g_4de300[member_slot] != g_4de2fc)
                    g_4de300[member_slot] = g_4de2fc;
            }
        }
        else
        {
            long object_slot = actor->unknown018 & 0xffff;
            if (g_4de300[object_slot] != g_4de2fc)
                g_4de300[object_slot] = g_4de2fc;
        }
    }
    s_prop_candidate_view candidates[100];
    s_iterator props;
    props.next = clump_get(clump_index)->first_prop;
    s_type_76cf92 *prop;
    while ((prop = clump_next_prop(&props)) != NULL)
    {
        if (function_269e60(*(long *)((byte *)prop + 8), clump, props.index, &candidates[count], false) < clump->update_state)
            function_26aaf0((s_clump *)clump, props.index);
        else
            ++count;
    }
    s_type_f1af8e iterator;
    iterator.signature = 0x86868686;
    iterator.type_mask = 0x1023;
    iterator.flags = 0;
    iterator.index = 0;
    iterator.object_index = NONE;
    s_slot_object_view *object;
    while ((object = (s_slot_object_view *)function_baeb0(&iterator)) != NULL)
    {
        short cluster = *(short *)((byte *)object + 0x2c);
        if (clusters[cluster >> 5] & (1 << (cluster & 31)))
            function_269de0(iterator.object_index, clump, candidates, &count);
    }
    g_4de2f8 = false;
    qsort(candidates, count, sizeof(*candidates), function_269da0);
    s_prop_candidate_view *candidate = candidates;
    while (selected < 50 && cursor < count)
    {
        if (candidate->priority < 3 && selected >= 20)
            break;
        if (candidate->prop_index == NONE)
        {
            long index = function_26a4f0(clump, candidate->priority);
            if (index != NONE)
            {
                s_prop_copy_view *new_prop = (s_prop_copy_view *)prop_get(index);
                function_26a310(new_prop, candidate->type, candidate->object_index, (s_clump *)clump, candidate->priority);
                function_26a960((s_clump *)clump, index);
                ++selected;
            }
        }
        else
        {
            s_prop_copy_view *existing = (s_prop_copy_view *)datum_get_inlined(g_50241c, candidate->prop_index);
            if (existing)
            {
                existing->field0c = candidate->priority;
                if (existing->type != candidate->type)
                {
                    s_type_5cfb45 *state = NULL;
                    short operation = g_470f10[candidate->type].unknown4;
                    if (operation != 0)
                    {
                        if (operation != 1)
                            goto type_ready;
                        s_iterator nodes;
                        nodes.next = ((s_clump_prop *)prop_get(candidate->prop_index))->first_node;
                        s_clump_node *node;
                        while ((node = clump_next_node(&nodes)) != NULL)
                        {
                            if (node->state >= 1)
                            {
                                if (*(long *)((byte *)node + 0x14) != NONE)
                                    state = function_25d690((s_prop_datum *)node);
                                break;
                            }
                        }
                        if (!node)
                            goto type_ready;
                    }
                    existing->state = 1;
                    if (state)
                        existing->observed = *state;
                    else
                        function_25ccd0(&existing->observed, existing->object_index, NONE, NULL);
                type_ready:
                    existing->type = candidate->type;
                }
                ++selected;
            }
        }
        ++cursor;
        ++candidate;
    }
    while (cursor < count)
    {
        long index = candidate->prop_index;
        if (index != NONE && datum_get_inlined(g_50241c, index))
            function_26aaf0((s_clump *)clump, index);
        ++cursor;
        candidate = &candidates[cursor];
    }
}

// @retail 0x269da0
int __cdecl function_269da0(void const *first, void const *second)
{
    s_prop_candidate_view const *a = (s_prop_candidate_view const *)first;
    s_prop_candidate_view const *b = (s_prop_candidate_view const *)second;
    if (a->priority > b->priority)
        return -1;
    if (a->priority >= b->priority && a->prop_index != NONE && b->prop_index == NONE)
        return -1;
    return 1;
}
