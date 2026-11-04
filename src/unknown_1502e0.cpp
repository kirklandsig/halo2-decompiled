// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1502E0.CPP: player helpers at the end of the players code (target
   candidates, the local players' view state, the players' census) */

#include "unknown_11c920.h"
#include "segment_sphere.h"
#include "globals.h"
#include "sound_sources.h"
#include "unknown_0259d0.h"
#include <string.h>
#include "object_iterator.h"
#include "unknown_06c1d0.h"

/* the players (g_4e8c24, 0x21c bytes each) */
struct s_tail_player
{
	short salt;
	word flags;
	byte unknown04[0x10 - 4];
	long leave_time;
	byte machine_address[6];
	short machine_index;
	short machine_user;
	byte unknown1e[2];
	long machine_controller;
	long controller_index;
	short local_index;
	byte unknown2a[2];
	long unit_index;
	long previous_unit_index;
	byte unknown34[0x174 - 0x34];
	long object_index174;
	byte unknown178[0x21c - 0x178];
};

/* the object header data (g_4e0300, 12 bytes each) */
struct s_tail_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	void *object;
};

/* the objects as the players code sees them */
struct s_tail_object
{
	byte unknown00[0xaa];
	byte type;
};

/* a candidate: its priority and its object */
struct s_target_candidate
{
	short priority;
	byte flags;
	byte unknown03;
	long object_index;
};

/* g_4e8c20 seen as the players' census */
struct s_player_census
{
	byte unknown00[4];
	bool all_without_units;
	bool any_available;
};

/* the view entries of g_4ed284 (0x94 bytes from +0x14) */
struct s_view_entry
{
	byte unknown00[0x28];
	short index28;
	byte unknown2a[0x8e - 0x2a];
	bool flag8e;
	bool flag8f;
	byte unknown90[0x94 - 0x90];

	__forceinline void clear_flags()
	{
		if (flag8e)
			flag8f = true;
		index28 = NONE;
	}
};

struct s_view_globals
{
	byte unknown00[0x14];
	s_view_entry entries[4];
};

struct s_object;
s_object *function_bae20(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);

static inline s_tail_player *tail_player_get(long player_index)
{
	return (s_tail_player *)g_4e8c24->data + (player_index & 0xffff);
}

static inline s_tail_object *tail_object_get(long object_index)
{
	return (s_tail_object *)((s_tail_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

void function_14cad0(long player_index, long unit_index);
void function_187a60(long datum_index);
void function_152340(void);

bool __stdcall function_e5470(long player_index, long object_index);
long __stdcall function_e5bc0(long unit_index, long object_index, long previous_index);
bool function_1df560(short team_a, short team_b);
bool function_151320(s_target_candidate *candidate, s_target_candidate *best, long player_index);

/* the object fields 0x1515e0 reads */
struct s_1515e0_object
{
	byte unknown00[0x10a];
	word unknown10a_0 : 1;
	word unknown10a_1 : 1;
	word unknown10a_2 : 1;
	word unknown10a_3 : 1;
	word unknown10a_4 : 1;
	word unknown10a_5 : 1;
	word unknown10a_6 : 1;
	word unknown10a_7 : 1;
	word unknown10b_0 : 8;
	byte unknown10c[0x138 - 0x10c];
	short team;
};

// @retail 0x1515e0
void function_1515e0(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_1515e0_object *object = (s_1515e0_object *)tail_object_get(object_index);
	byte type = ((s_tail_object_header *)g_4e0300->data)[player->unit_index & 0xffff].type;
	if (((1 << type) & 1) && !TEST_FIELD_BIT(object->unknown10a_2))
	{
		if (function_1df560((short)*((char *)player + 0xc0), object->team))
			*(long *)((byte *)*best_reference + 8) = function_e5bc0(player->unit_index, object_index, *(long *)((byte *)*best_reference + 8));
		else if (function_e5470(*player_reference, object_index))
		{
			s_target_candidate candidate;
			memset(&candidate, 0, sizeof(candidate));
			candidate.priority = 2;
			candidate.object_index = object_index;
			function_151320(&candidate, *best_reference, *player_reference);
		}
	}
}

bool function_d6af0(long unit_index);
bool function_d6790(long unit_index);
void player_speed_request_14ea20(long player_index);
void player_speed_request_14eb10(long player_index);
void player_speed_request_14ec00(long player_index);
bool function_152000(long player_index, short effect, short ticks);
void __stdcall function_191fab(long definition_index, long local_index);
void function_a89c0(long unit_index, long definition_index);
void function_f8110(long object_index);
void __stdcall function_b8540(long object_index);

/* game ticks in the given number of seconds, rounded */
static inline long game_seconds_to_ticks_round(real seconds)
{
	real ticks = g_510c54->field_2_3 * seconds;
	long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}

/* a flag field read as word bits: retail reads these flags with a load, a
   shift and a test of the low bit */
struct s_flag_word_bits
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

// score 114 (was 114): the walk found no behaviour difference. Retail tests flag 4 as `test dl, 0x10` on the
// header's flag byte; that faithful `header->flags & 0x10` (with the parameters read directly) gives 124 here,
// because with the early returns our link inlines 0x191fab (src/unknown_1916dc.cpp) at the straight-line tail
// call (364 bytes), where retail calls it. Nesting the body under a `proceed` flag instead of the early returns
// keeps the call out of line (105) but is a flag set inside a test, so it isn't kept; `/Ob1` on unknown_1916dc.cpp
// (106) is another file's flags. The word-bitfield view stays.
// Still differing: retail forms the player before it loads g_4e0300 (ours hoists that load above the pushes, so
// ecx/edx and the first lines are off by one register), loads the 0x130 float into xmm0 before the cvtsi2ss (ours
// folds it into the mulss), holds the ticks in edx (ours eax), and tests kind == 3 as `movsx eax, si; sub eax, 3; je`
// with the 0 and 1 stores on branches (ours branches on `cmp ax, 3`; the ternary and every if/else spelling give
// `setne` instead, 106). Tried: object/player through pointer locals (same), header first (128), header by
// tail_object_get (100), flag test before the ticks (108), the float in a local first (same), `seconds * field` or
// `(real)field * seconds` in the helper (same), an outer switch with `goto` into the shared 0x152000 call (jump
// table, 96 at 340 bytes), switch over 2/5/3 under `kind != 1` (binary tree, 109), `kind - 3 == 0` (102).
// Permuter (25 min on r654): best 86, only with a volatile header pointer; not kept.
// F352: Native header-byte mask producer with both existing stack aliases retained,
// bool/byte/short/long default-false arms 124/124/129/128; complete true/false
// arms 123/123/126/125. All restored; player-base/root scheduling, ticks/effect
// homes and the expanded tail remain. Original word-bitfield view retained.
// N341: direct unavailable producer 122; call-result admission flat/nontrivial-kind scopes 98/87;
// short ticks 114, full-width kind/effect 112/113, nested tick admission and sole exit 114 each.
// NOT kept: conditional success flags remain unmatched; width-only gains are at most two.
// Player/object load scheduling, native flag mask, effect dispatch and tail inlining remain.
// Round 51 (m3 lane 15): the two parameter-address locals dropped, parameters read directly 114 (same code, kept);
// on that: object through tail_object_get 120; `ticks <= 0 || unavailable` early return 114; player looked up after
// the ticks 120; the flag tested in the ticks condition (no `unavailable`) 122.
// DH319: Existing rounding producer duration/rate accumulators and named-rate
// products 114/114/114/114; the same with a short return 135/135/135/135.
// Original helper restored; player/root scheduling, duration XMM ownership,
// effect dispatch and expanded tail remain.
// Native definition/object/both field records 114/114/114; complete-definition
// pointer/reference rounding inputs 114/114; tick-result output pointer/reference
// 114/114; result-first named scalar homes 114. All restored; player/root,
// duration, tick/effect homes and expanded tail remain.
// B397: Duration-field pointer/reference inputs to the existing rounding producer,
// direct/duration/rate/named-rate graphs 114/114/114/114/114/114/114/114. All restored;
// scalar frame agrees, player/root ownership, separate SSE input and expanded tail remain.
// @retail 0x151ea0
void function_151ea0(long player_index, long object_index)
{
	s_tail_player *player = tail_player_get(player_index);
	s_tail_object_header *header = (s_tail_object_header *)g_4e0300->data + (object_index & 0xffff);
	byte *object = (byte *)header->object;
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	long ticks = game_seconds_to_ticks_round(*(real *)(definition + 0x130));
	bool unavailable = false;
	if (TEST_FIELD_BIT(((s_flag_word_bits *)&header->flags)->bit4))
		unavailable = true;
	if ((short)ticks > 0 && !unavailable)
	{
		short kind = *(short *)(definition + 0x12c);
		if (kind != 1)
		{
			if (kind == 2)
			{
				if (!function_d6af0(player->unit_index)) return;
				player_speed_request_14ea20(player_index);
			}
			else if (kind == 5)
			{
				if (!function_d6790(player->unit_index)) return;
				player_speed_request_14ec00(player_index);
			}
			else
			{
				short effect = kind == 3 ? 0 : 1;
				if (!function_152000(player_index, effect, (short)ticks)) return;
				if (effect == 0) player_speed_request_14eb10(player_index);
			}
		}
		long definition_index = *(long *)tail_object_get(object_index);
		function_191fab(definition_index, player->local_index);
		function_a89c0(player->unit_index, definition_index);
		if (player->local_index != NONE) function_f8110(object_index);
		function_b8540(object_index);
	}
}

// score 1 (was 1): only the jne of the equal-priority test differs: retail jumps to the epilogue after the copy
// block (al still holds the initial false) and keeps a separate `xor al, al` + epilogue for the not-closer
// arm; ours jumps to the epilogue after that `xor`. Tried (fast, base 1): `return false;` in the else arm
// (67, the register assignment changes from the prologue on), `return result;` there (67), `!(a < b)` test first
// (12), `a >= b` test first (13), copy arm's stores swapped (4), `return false` as an early exit before the
// copy label (67), copy label in the same arm with `return result` (69).
// Permuter (25 min on r654, about 1030 variants): nothing below 1.
// M337: Initial winner predicate with guarded/direct equality, equality-first result, and
// priority-range result, bool/byte carriers: 74/104, 112/102, 81/79, 82/91.
// All restored; the equal-priority jne still chooses the other existing epilogue.
// P294: whole-record memcpy 1;
// whole-record memmove 81;
// all named fields 37;
// readonly candidate 1;
// readonly memcpy 1;
// readonly memmove 81;
// readonly named fields 37;
// memcpy then success 4.
// Original executable body restored;
// no exact match from these ownership forms.
// Round 103: equality as an else-if 1; nested <=/== priority tests 12; an explicit not-equal
// else-if to done 1; separate distance-failure/copy/done labels 75; split result declaration 1;
// result declared last 1; byte result 32; a named closer flag 1. Original body restored.
// @retail 0x151320
bool function_151320(s_target_candidate *candidate, s_target_candidate *best, long player_index)
{
	bool result = false;
	long const *player_reference = &player_index;
	point3f player_position;
	point3f best_position;
	point3f candidate_position;
	vector3f to_candidate;
	vector3f to_best;
	real candidate_distance;
	real best_distance;

	if (candidate->priority > best->priority)
		goto copy;
	if (candidate->priority == best->priority)
	{
		function_b9dd0(tail_player_get(*player_reference)->unit_index, &player_position);
		function_b9dd0(best->object_index, &best_position);
		function_b9dd0(candidate->object_index, &candidate_position);
		vector3d_from_points3d(&player_position, &candidate_position, &to_candidate);
		vector3d_from_points3d(&player_position, &best_position, &to_best);
		candidate_distance = to_candidate.j * to_candidate.j + to_candidate.i * to_candidate.i + to_candidate.k * to_candidate.k;
		best_distance = length_sq3f(&to_best);
		if (candidate_distance < best_distance)
		{
copy:
			result = true;
			*best = *candidate;
		}
		else
		{
			result = false;
		}
	}
	return result;
}


static __forceinline bool player_candidate_has_type(s_target_candidate *candidate, dword type_mask)
{
	bool result = false;
	long object_index = candidate->object_index;

	if (function_bae20(object_index, NONE))
	{
		s_tail_object_header *header = (s_tail_object_header *)g_4e0300->data + (object_index & 0xffff);

		result = (type_mask & (1 << header->type)) != 0;
	}
	return result;
}

__declspec(noinline) bool function_1518f0(s_target_candidate *candidate, dword type_mask);

// @retail 0x1518f0
bool function_1518f0(s_target_candidate *candidate, dword type_mask)
{
	return player_candidate_has_type(candidate, type_mask);
}

PRIVATE __forceinline void local_player_view_clear(s_view_globals *views, short index)
{
	views->entries[index].clear_flags();
}

// @retail 0x1520f0
void function_1520f0(long player_index)
{
	s_tail_player const volatile *player = tail_player_get(player_index);
	short local_index = player->local_index;

	if (local_index != NONE)
		local_player_view_clear((s_view_globals *)g_4ed284, local_index);
}

// @retail 0x152340
void function_152340(void)
{
	s_player_census *census = (s_player_census *)g_4e8c20;
	s_record_pool_iterator iterator;
	s_tail_player *player;

	census->all_without_units = true;
	census->any_available = false;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		if (player->unit_index != NONE)
		{
			census->all_without_units = false;
		}
		else if (player->flags & 8)
		{
			census->all_without_units = false;
		}
		else
		{
			census->any_available = true;
		}
	}
}

/* the game options' players (16 of 0xe4 bytes at +0x2dc): an identifier
   and two sets of traits */
struct s_tail_traits
{
	byte unknown00[0x1c];
};

struct s_tail_options_player
{
	byte unknown00[0xe];
	byte identifier[12];
	byte unknown1a[0xac - 0x1a];
	s_tail_traits traits[2];
};

struct s_tail_options
{
	byte unknown00[0x2dc];
	s_tail_options_player players[16];
};

struct s_tail_scenario_entry
{
	byte unknown00[0x2c];
	short value2c;
};

struct s_tail_scenario
{
	byte unknown00[0x100];
	long entry_count;
	s_tail_scenario_entry *entries;
};

struct s_tail_player_identifier
{
	byte unknown00[4];
	byte identifier[12];
};

// score 31 (was 66): logic walk against retail found no behaviour difference (same player identifier
// at +4, scenario entry 0 value 0x2c == 1 selects the second traits set, 16 options players of 0xe4
// bytes at +0x2dc, 12-byte identifier compare at +0xe, traits at +0xac + 0x1c * set). Still differs:
// retail keeps the options pointer in ebx, the identifier in eax and the compare result in edx; ours
// puts the identifier in ebx, the options pointer in edx and the result in eax, and loads
// g_4e0350/g_4e6948 at different points. Tried: identifier hoisted/late (67), index form
// players[i] (66-67), do-while (31), memcmp() == 0 without the result variable (80), != 0 (80),
// scenario read without a local (67), flag as index vs ?: (43), find-then-test form (31).
// Permuter (25 min on r654): nothing below 31 (its renderer drops the `i++, entry++` increment, so it started
// from the indexed form).
// Round 45 (m3 lane 2, fast, base 31): retail loads the player first, the scenario next and g_4e6948 only after the
// scenario test (beside `push ebx`); the entry pointer assigned after the alternate test 68 (158 bytes, the loop's
// entry jump over the padding goes), that with the entry declared first 68; `difference` declared at function scope
// 31 (same code); the player declared before the entry 32; `alternate` declared first 67 (158 bytes).
// M337: Native range-end scan with early/delayed options and direct/common result
// [53, 45, 55, 46]; plain countdown scan with pointer step in the body [38, 30, 58, 26].
// NOT kept: 30 gains one; 26 uses a countdown unlike retail and depends on combined forms.
// Original ordinal scan restored; pointer/comparison roles and delayed saves remain.
// N345: Isolated entry/player/scenario/comparison/index/parameter/alternate role names
// and the CRT int comparison result: all eight 31. Original body restored; options
// acquisition, delayed saves and comparison register ownership still differ.
// F357: Native scenario count/list/selector operand lifetimes, seven subsets
// 31/31/31/31/31/31/31; complete operands with direct truth producer 68.
// Existing player/object lookup inline, forced and unhinted policies: eight 31.
// NOT kept: no gain; original observations, ordinal scan and full comparison restored.
// F353: Complete player record with native entry/options-root/traits-result/alternate-truth lifetimes
// 32/68/56/56/32/67/67/56.
// NOT kept: no gain; original full body/helpers restored.
// Y559: Complementary first-set storage bool/byte/short/long, branch-cleared then
// test-produced: 32/32/68/67/67/67/68/67. NOT kept: no byte match; native
// delayed saves and identifier/options/comparison allocation remain.
// N345: Isolated entry/player/scenario/comparison/index/parameter/alternate role names
// and the CRT int comparison result: all eight 31. Original body restored; options
// acquisition, delayed saves and comparison register ownership still differ.
// M348: Comparison transport unsigned-long/short/word/signed-byte/byte 31/67/67/67/67;
// equality carrier 31; negative continuation/explicit mismatch arms 32/32.
// NOT kept: no byte match; native options/identifier/comparison homes remain.
// M350: Immutable player/scenario/full-comparison storage, seven subsets, then
// fixed read-only player identity: 31 each. Original lookup phases and upward
// scan restored; options acquisition and identifier/comparison homes remain.
// @retail 0x1529a0
s_tail_traits *function_1529a0(long player_index)
{
	s_tail_options_player *entry = ((s_tail_options *)g_4e6948)->players;
	s_tail_player_identifier *player = (s_tail_player_identifier *)tail_player_get(player_index);
	s_tail_scenario *scenario = (s_tail_scenario *)g_4e0350;
	bool alternate = false;

	if (scenario->entry_count > 0 && scenario->entries[0].value2c == 1)
		alternate = true;
	for (long i = 0; i < 16; i++, entry++)
	{
		long difference = memcmp(entry->identifier, player->identifier, sizeof(entry->identifier));

		if (difference == 0)
			return &entry->traits[alternate ? 1 : 0];
	}
	return 0;
}

/* the units and their definitions as the census of 0x152580 reads them */
struct s_tail_unit
{
	long definition_index;
	byte unknown04[0x184 - 4];
	real value184;
	byte unknown188[0x22a - 0x188];
	short value22a;
	short value22c;
};

struct s_tail_seat
{
	byte unknown00[0x88];
	byte value88;
	byte unknown89[0x90 - 0x89];
	long tag90;
	byte unknown94[4];
	long tag98;
};

struct s_tail_unit_definition
{
	byte unknown00[0x1fc];
	byte value1fc;
	byte unknown1fd[0x2c0 - 0x1fd];
	long count2c0;
	byte unknown2c4[0x2d0 - 0x2c4];
	long seat_count;
	s_tail_seat *seats;
};

struct s_tail_seat_definition
{
	byte unknown00[0x128];
	byte value128;
};

struct s_tail_unit_state
{
	short value0;
	short value2;
	short value4;
	short value6;
};

// @retail 0x152580
void function_152580(long unit_index, s_tail_unit_state *state)
{
	s_tail_unit *unit = (s_tail_unit *)((s_tail_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
	s_tail_unit_definition *definition = (s_tail_unit_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;
	long value = 0;
	real scaled;
	long rounded;

	if (definition->seat_count > 0)
	{
		s_tail_seat *seat = definition->seats;

		if (seat->tag90 != NONE)
		{
			value = ((s_tail_seat_definition *)g_4e3b44[seat->tag90 & 0xffff].bytes)->value128;
		}
		else if (seat->tag98 != NONE)
		{
			value = seat->value88;
		}
	}
	if (!value)
	{
		value = definition->value1fc;
	}
	state->value0 = (short)value;
	scaled = (1.0f - unit->value184) * 100.0f;
	__asm
	{
		fld scaled
		fistp rounded
	}
	state->value6 = (short)rounded;
	if (definition->count2c0 > 0)
	{
		state->value2 = unit->value22a;
		state->value4 = unit->value22c;
	}
}

/* a horizontal forward vector and its left */
struct s_horizontal_axes
{
	vector3f forward;
	vector3f left;
};

real function_30bf0(vector3f *v);

static __forceinline void horizontal_left_set(real i, real j, vector3f *left)
{
	left->i = 0.0f - j;
	left->j = i;
	left->k = 0.0f;
}

// @retail 0x1502e0
void function_1502e0(vector3f const *forward, vector3f const *fallback, s_horizontal_axes *axes)
{
	axes->forward = *forward;
	axes->forward.k = 0.0f;
	if (function_30bf0(&axes->forward) == 0.0f)
	{
		axes->forward = *fallback;
		axes->forward.k = 0.0f;
		if (function_30bf0(&axes->forward) == g_45dbd8)
		{
			axes->forward = *g_4687a8;
		}
	}
	horizontal_left_set(axes->forward.i, axes->forward.j, &axes->left);
}

/* an object is being deleted: the players forget it (one of the object
   deletion callbacks, g_468664) */
// @retail 0x152cf0
void __stdcall function_152cf0(long object_index)
{
	if ((1 << tail_object_get(object_index)->type) & 3)
	{
		s_record_pool_iterator iterator;
		s_tail_player *player;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
		{
			if (player->unit_index == object_index)
			{
				s_tail_player *owner = tail_player_get(iterator.datum_index);

				owner->previous_unit_index = owner->unit_index;
				function_14cad0(iterator.datum_index, NONE);
				function_152340();
			}
			if (player->object_index174 == object_index)
			{
				player->object_index174 = NONE;
			}
			if (player->previous_unit_index == object_index)
			{
				player->previous_unit_index = NONE;
			}
		}
	}
	function_187a60(object_index);
}

short __stdcall function_cdeb0(long unit_index, long state_name, short slot, short direction);
long function_cbe60(long unit_index);
bool function_cd660(long unit_index);

// @retail 0x150dd0
bool function_150dd0(long player_index)
{
	s_tail_player *player = tail_player_get(player_index);
	long index = player->unit_index;
	if ((short)function_cdeb0(index, NONE, NONE, 0) == NONE)
		return true;
	else
	{
		if (g_4e6948->state != 2)
		{
			long unit_index = player->unit_index;
			if (function_cbe60(unit_index) == NONE && !function_cd660(unit_index))
				return true;
		}
		return false;
	}
}

struct s_player_motion_unit
{
	byte unknown00[0x150];
	vector3f vector150;
	vector3f vector15c;
	byte unknown168[0x180 - 0x168];
	vector3f vector180;
};

struct s_player_motion_control
{
	byte unknown00[0x28];
	vector3f vector28;
	vector3f vector34;
	vector3f vector40;
	byte unknown4c[0x7c - 0x4c];
};

struct s_unit_state_c6ef0;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);

// @retail 0x150790
void function_150790(long unit_index)
{
	s_player_motion_control control;
	s_player_motion_unit *unit = (s_player_motion_unit *)tail_object_get(unit_index);
	function_c6ef0((s_unit_state_c6ef0 *)&control);
	control.vector28 = unit->vector150;
	control.vector34 = unit->vector15c;
	control.vector40 = unit->vector180;
	function_c6de0(unit_index, &control);
}

struct s_player_target_object
{
	byte unknown00[0x30];
	point3f position;
	real radius;
	byte unknown40[0x168 - 0x40];
	vector3f direction;
};

void function_cafc0(long unit_index, point3f *position);
bool function_1078f0(long control_index, vector3f const *direction);
bool function_107870(long device_index);

// @retail 0x1516c0
void function_1516c0(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(player->unit_index);
	s_player_target_object *object = (s_player_target_object *)tail_object_get(object_index);
	point3f eye;
	function_cafc0(player->unit_index, &eye);
	if (function_11e5e0(&eye, &unit->direction, &object->position, object->radius) &&
		function_1078f0(object_index, &unit->direction) && function_107870(object_index))
	{
		s_target_candidate candidate;
		memset(&candidate, 0, sizeof(candidate));
		candidate.priority = 3;
		candidate.object_index = object_index;
		function_151320(&candidate, *best_reference, *player_reference);
	}
}

struct s_player_pickup_object
{
	long definition_index;
	byte unknown04[0x30 - 4];
	point3f position;
	byte unknown3c[0x12c - 0x3c];
	dword flags;
	byte unknown130[0x14c - 0x130];
	long field_14c;
};

struct s_player_pickup_definition
{
	byte unknown00[0x290];
	short type;
};

bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);

/* two of the four answers of 0xcd7b0 */
struct s_pickup_hands
{
	bool primary;
	bool secondary;
};

// @retail 0x151780
void function_151780(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(player->unit_index);
	s_player_pickup_object *object = (s_player_pickup_object *)tail_object_get(object_index);
	s_player_pickup_definition *definition = (s_player_pickup_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
	bool special = definition->type != 0;
	real radius = unit->radius + (special ? 0.1f : 0.0f) + 0.3f;
	if (!(object->flags & 1) && object->field_14c != player->unit_index)
	{
		vector3f delta;
		vector3d_from_points3d(&unit->position, &object->position, &delta);
		real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
		s_pickup_hands hands[2];
		if (distance < radius * radius && function_cd7b0(player->unit_index, object_index, &hands[0].primary))
		{
			s_pickup_hands first = hands[0];
			s_pickup_hands second = hands[1];
			long flags = 0;
			if (first.primary)
				flags = 1;
			else if (second.primary)
				flags = 2;
			if (first.secondary)
				flags |= 4;
			else if (second.secondary)
				flags |= 8;
			s_target_candidate candidate;
			memset(&candidate, 0, sizeof(candidate));
			candidate.object_index = object_index;
			candidate.flags = (byte)flags;
			candidate.priority = special ? 7 : 1;
			function_151320(&candidate, *best_reference, *player_reference);
		}
	}
}

struct s_player_removed_unit
{
	byte unknown00[0x10a];
	word flags;
	byte unknown10c[0x2a8 - 0x10c];
	long player_index;
};

struct s_player_census_counts
{
	long count;
	byte unknown04[4];
	short user_count;
	short controller_count;
	long users[4];
	long controllers[4];
	dword machine_valid_mask;
	byte machine_addresses[16][6];
	bool local_machine_valid;
	byte local_machine_address[6];
	byte unknown97;
	long local_machine_index;
};

extern byte g_4cf77b;

class c_class_6a600;
void function_69580(c_class_6a600 *world, long player_index);
void function_b7360(long object_index);
void player_control_set_unit(long user_index, long unit_index);
void function_1682bf(long unit_index, long user_index, long character_index);
void function_157de0(long player_index);

// @retail 0x152df0
void function_152df0(long player_index)
{
	long const *player_reference = &player_index;
	s_tail_player *player = tail_player_get(*player_reference);
	long unit_index = player->unit_index;
	if (unit_index != NONE)
	{
		s_player_removed_unit *unit = (s_player_removed_unit *)tail_object_get(unit_index);
		word *flags = &unit->flags;
		*flags |= 0x400;
		function_b7360(unit_index);
	}
	if (g_4e6948->mode == 4)
	{
		struct
		{
			s_player_removed_unit *unit;
			s_type_f1af8e iterator;
		} state;
		state.iterator.signature = 0x86868686;
		state.iterator.type_mask = 3;
		state.iterator.flags = 0;
		state.iterator.index = 0;
		state.iterator.object_index = NONE;
		while ((state.unit = (s_player_removed_unit *)function_baeb0(&state.iterator)) != 0)
		{
			if (state.unit->player_index == *player_reference)
				state.unit->player_index = NONE;
		}
	}
	function_14cad0(*player_reference, NONE);
	player->previous_unit_index = NONE;
	/* read through its address: retail loads the flag after the previous_unit_index store */
	if (*(byte const *)&g_4cf770 && !g_4cf77b)
		function_69580((c_class_6a600 *)g_4cf77c, *player_reference);
	player->machine_index = NONE;
	memset(player->machine_address, 0, sizeof(player->machine_address));
	player->machine_user = NONE;
	player->machine_controller = NONE;
	s_tail_player *local_player = tail_player_get(*player_reference);
	if (local_player->local_index != NONE)
	{
		player_control_set_unit(local_player->local_index, NONE);
		function_1682bf(NONE, local_player->local_index, NONE);
		s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
		globals->users[local_player->local_index] = NONE;
		globals->user_count--;
		local_player->local_index = NONE;
	}
	s_tail_player *controller_player = tail_player_get(*player_reference);
	if (controller_player->controller_index != NONE)
	{
		s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
		globals->controllers[controller_player->controller_index] = NONE;
		globals->controller_count--;
		controller_player->controller_index = NONE;
	}
	player->flags |= 2;
	player->leave_time = g_510c54->game_time;
	player->flags &= ~1;
	((s_player_census_counts *)g_4e8c20)->count--;
	function_157de0(*player_reference);
}

struct s_player_target_unit
{
	byte unknown00[0x30];
	point3f position;
	real radius;
	byte unknown40[0x10a - 0x40];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word : 13;
	byte unknown10c[0x248 - 0x10c];
	long parent_index;
	byte unknown24c[0x348 - 0x24c];
	word flag348_0 : 1;
	word flag348_1 : 1;
	word flag348_2 : 1;
	word flag348_3 : 1;
	word flag348_4 : 1;
	word flag348_5 : 1;
	word flag348_6 : 1;
	word : 9;
};

__declspec(noinline) bool function_f5dc0(long object_index);
bool function_cc0c0(long unit_index);
short __stdcall function_c8ef0(long unit_index, long object_index, long *target_index, short *seat_index);

void function_151430(long object_index, long player_index, s_target_candidate *best);

static __forceinline void player_object_target_candidate_evaluate(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_unit *object = (s_player_target_unit *)tail_object_get(object_index);
	if (!TEST_FIELD_BIT(object->flag2))
	{
		if (!function_f5dc0(object_index))
		{
			long unit_index = player->unit_index;
			if (!function_cc0c0(unit_index) && !(player->flags & 0x800))
			{
				long target_index = NONE;
				long seat_index = NONE;
				short type = function_c8ef0(unit_index, object_index, &target_index, (short *)&seat_index);
				if (type)
				{
					s_target_candidate candidate;
					memset(&candidate, 0, sizeof(candidate));
					switch (type)
					{
					case 1: candidate.priority = 4; break;
					case 2: candidate.priority = 5; break;
					case 3: candidate.priority = 6; break;
					}
					candidate.object_index = target_index;
					*(short *)&candidate.flags = (short)seat_index;
					function_151320(&candidate, *best_reference, *player_reference);
				}
			}
		}
		else if (player->unit_index != NONE)
		{
			s_player_target_unit *unit = (s_player_target_unit *)tail_object_get(player->unit_index);
			if (!TEST_FIELD_BIT(object->flag348_6) && object->parent_index == NONE)
			{
				vector3f delta;
				vector3d_from_points3d(&unit->position, &object->position, &delta);
				real radius = unit->radius + object->radius + 0.2f;
				real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				if (distance < radius * radius)
				{
					s_target_candidate candidate;
					memset(&candidate, 0, sizeof(candidate));
					candidate.priority = 8;
					candidate.object_index = object_index;
					function_151320(&candidate, *best_reference, *player_reference);
				}
			}
		}
	}
}

// @retail 0x151430
void function_151430(long object_index, long player_index, s_target_candidate *best)
{
	player_object_target_candidate_evaluate(object_index, player_index, best);
}

struct s_tail_transfer_options
{
	byte unknown00[0x2d4];
	s_tail_options_player players[16];
};

struct s_tail_transfer_unit
{
	byte unknown00[0x1fc];
	short seat_index;
	byte unknown1fe[0x212 - 0x1fe];
	char primary_slot;
	char secondary_slot;
	byte unknown214[4];
	long weapons[4];
	long weapon_times[4];
	byte unknown238[6];
	byte grenades[2];
};

struct s_tail_transfer_traits
{
	bool valid;
	byte unknown01;
	s_tail_unit_state weapons[3];
	byte grenades[2];
};

// score 153 (was 153): logic walked against retail, no behaviour difference found (loops, offsets, order of the three
// weapon slots, grenade copy and the memset path agree). Only allocation differs: retail keeps the player in ebx, the
// unit index in ebp and NONE in edi, and re-reads g_4e0300->data and the unit pointer in the no-seat branch (0x1528af,
// 0x1528d4) while ours reuses the unit pointer in esi. Tried: primary/secondary/other declared before the unit_index
// read (159), the three weapons as a long[3] (152), a held[2] array with the no-seat branch as a hand loop over
// held_slots[] (151, frame grows by 8), held[2] replacing primary/secondary (156), a store through a pointer to other (153).
// AC502: Failed-traits continuation/missing-unit completion/both 153/171/171; real
// identifier cursor/copy cursors/search plus continuation/both cursors/all admissions
// 217/258/217/270/283. Original traversals and body restored; selected-traits/unit/NONE homes remain.
// Round 552: existing shared current-weapon producer, all shared object getters, native selection
// do-while, admitted age, once/per-use clock, weapon-result home names and per-use second lookup:
// 154/154/154/180/165/165/154/154. All complete original source and existing helper bodies restored.
// NOT kept: no score gain or byte match.
// Round 51 (m3 lane 15): clock read once before the selection loop 164; alternate as one bool expression 175;
// primary through the existing unit_get_current_weapon 154; results declared other/secondary/primary 153; no-seat
// arm first 169. Retail's no-seat primary shares the selection counter's slot and lives in EDI in the seat arm.
// DH319: Last-match array ordinal long/dword/short/byte, range admission
// 189/189/196/197; unequal-sentinel admission 189/189/196/197. Original
// pointer selection restored; selected-traits/unit/NONE homes and pool reloads remain.
// Branch-owned primary/secondary/other default subsets 1/2/4/3/5/6/7:
// 157/158/162/162/152/160/151; complete conditional slot producers 151.
// NOT kept: at most two points and unmatched lifetime interaction. Original
// defaults and body restored; selected-traits/unit/NONE homes and reloads remain.
// B397: Native traits-element/player-identifier domains and complete inner choice
// eligibility subsets, then complete rejection complement 153/153/153/153/153/153/153/153.
// Original layouts/tests restored; traits/unit/NONE homes and pool reloads remain.
// @retail 0x152660
void function_152660(s_tail_transfer_options *options)
{
	bool alternate = false;
	for (long i = 0; i < 16; i++)
		memcpy(options->players[i].traits, ((s_tail_options *)g_4e6948)->players[i].traits, sizeof(options->players[i].traits));
	s_tail_scenario *scenario = (s_tail_scenario *)g_4e0350;
	if (scenario->entry_count > 0 && scenario->entries[0].value2c == 1)
		alternate = true;
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	s_tail_player *player;
	while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		s_tail_transfer_traits *traits = 0;
		for (long i = 0; i < 16; i++)
		{
			long difference = memcmp(((s_tail_player_identifier *)player)->identifier, options->players[i].identifier, 12);
			if (difference == 0)
				traits = (s_tail_transfer_traits *)&options->players[i].traits[alternate ? 1 : 0];
		}
		if (traits)
		{
			long unit_index = player->unit_index;
			if (unit_index != NONE)
			{
				s_tail_transfer_unit *unit = (s_tail_transfer_unit *)tail_object_get(unit_index);
				long primary = NONE;
				long secondary = NONE;
				long other = NONE;
				if (unit->seat_index != NONE)
				{
					for (long selection = 0; selection < 2; selection++)
					{
						long now = g_510c54->game_time;
						long chosen = NONE;
						long shortest = 0x7fffffff;
						for (long slot = 0; slot < 4; slot++)
						{
							long age = now - unit->weapon_times[slot];
							long weapon = unit->weapons[slot];
							if (weapon != NONE && (!selection || weapon != primary) && age < shortest)
							{
								shortest = age;
								chosen = weapon;
							}
						}
						if (!selection)
							primary = chosen;
						else
							other = chosen;
					}
				}
				else
				{
					s_tail_transfer_unit *first = (s_tail_transfer_unit *)tail_object_get(unit_index);
					short primary_slot = first->primary_slot;
					if (primary_slot != NONE)
						primary = first->weapons[primary_slot];
					s_tail_transfer_unit *second = (s_tail_transfer_unit *)tail_object_get(unit_index);
					short secondary_slot = second->secondary_slot;
					if (secondary_slot != NONE)
						secondary = second->weapons[secondary_slot];
					other = function_cbe60(unit_index);
				}
				if (primary != NONE)
					function_152580(primary, &traits->weapons[0]);
				else
					traits->weapons[0].value0 = 0;
				if (secondary != NONE)
					function_152580(secondary, &traits->weapons[2]);
				else
					traits->weapons[2].value0 = 0;
				if (other != NONE)
					function_152580(other, &traits->weapons[1]);
				else
					traits->weapons[1].value0 = 0;
				traits->grenades[0] = unit->grenades[0];
				traits->grenades[1] = unit->grenades[1];
				traits->valid = true;
			}
			else
				memset(traits, 0, sizeof(*traits));
		}
	}
}

struct s_player_pickup_unit
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x212 - 0x18];
	char held_slots[2];
	byte unknown214[2];
	char pending_slots[2];
	long weapons[4];
};

struct s_player_weapon_definition
{
	byte unknown00[0x12c];
	dword flags;
};

static __forceinline dword player_weapon_flags(long weapon_index)
{
	s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(weapon_index);
	return ((s_player_weapon_definition *)g_4e3b44[weapon->definition_index & 0xffff].bytes)->flags;
}

// score 284 (was 322):
// The logic matches retail block by block (reads, flag bits 3/4/22/23, stores, count >= 2 pass); only the
// register allocation differs. Retail keeps g_4e0300 in edi for the whole function and loads g_4e3b44 lazily
// into ecx, with the player in ebp; this build reloads g_4e0300 every iteration of the first loop and hoists
// g_4e3b44 into ebp, so the player and temporaries land in other registers. Retail's first loop is not
// aligned (no pad before it), ours is.
// Tried (scores): taking the parameters' addresses (322, the old form, 962 bytes), `if/else` instead of `?:`
// for the slot lookups (286), locals for held/pending weapon (295), inline helpers for the held/pending weapon
// lookups (295), writes through &held[hand] (287), a local copy of g_4e0300 passed to helpers (286, 1019
// bytes), one 4-entry array for held/pending (295), a struct for them (292), do-while with a hand<2 guard (284).
// AC502: Shared object getter at initial/held/pending/flag lookups, complete main/all
// receivers, immutable receivers and direct tag-field access: 284 each. Original getters,
// include set and body restored; pool/player/tag-table allocation remains.
// Round 552: original ternary producers with direct first-loop do/while, then native slot/final/both
// loops on each entry graph 284/636/284/636/284/636/284/636. All loops, observations, helper and complete body restored.
// NOT kept: no score gain or byte match.
// P295: Original ternary results with delayed held/both publication, completed held-slot scope,
// immutable results and readonly slot receivers: 284/281/284/281/284/281/284/281.
// NOT kept: three-point gain and one-use pending receiver; pool/player/tag carriers remain.
// Y559: Bounded slot count char/short/int/dword, entry then count-phase zero
// declaration: 287/283/284/284/305/293/294/294. NOT kept: one-point lead below
// the keep gate; retained pool/player and lazy tag-table ownership remain.
// Y561: Initial hand selector char/short/word/dword 345/345/345/284;
// final hand selector char/short/word/dword 283/282/282/284. NOT kept:
// two-point best below the gate; native full-width selectors/body restored.
// Y563: Count-slot rejection guards/held guard/Boolean accumulation/rejection plus
// accumulation, final guarded/early-exit tail: 284/284/318/318/284/284/318/318. NOT kept:
// no gain; original count admission, field observations and complete body restored.
// N353: Existing complete slot selector at held/pending/inventory subsets 1-7
// 288/294/295/337/337/351/346; all with direct slot arguments 346.
// NOT kept: no qualifying gain; original pool/player/tag acquisition and body restored.
// @retail 0x1509e0
void function_1509e0(long player_index, long weapon_index, bool *modes)
{
	s_tail_player *player = tail_player_get(player_index);
	s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
	long held[2];
	long pending[2];
	long count = 0;
	for (long hand = 0; hand < 2; hand++)
	{
		s_player_pickup_unit *current = (s_player_pickup_unit *)tail_object_get(player->unit_index);
		short slot = current->held_slots[hand];
		held[hand] = slot == NONE ? NONE : current->weapons[slot];
		s_player_pickup_unit *next = (s_player_pickup_unit *)tail_object_get(player->unit_index);
		short next_slot = next->pending_slots[hand];
		pending[hand] = next_slot == NONE ? NONE : next->weapons[next_slot];
		if (held[hand] != NONE && (bool)((player_weapon_flags(held[hand]) >> 3) & 1))
		{
			modes[hand] = false;
			modes[hand + 2] = false;
		}
	}
	if (unit->parent_index != NONE)
	{
		for (long hand = 0; hand < 2; hand++)
		{
			modes[hand] = false;
			modes[hand + 2] = false;
		}
	}
	for (long slot = 0; slot < 4; slot++)
	{
		long held_weapon = unit->weapons[slot];
		if (held_weapon != NONE && !(bool)((player_weapon_flags(held_weapon) >> 4) & 1))
		{
			dword flags = player_weapon_flags(weapon_index);
			if (!((bool)((flags >> 22) & 1) || (bool)((flags >> 23) & 1)) || held_weapon != held[1] || held_weapon != pending[1])
				count++;
		}
	}
	if (count >= 2)
	{
		for (long hand = 0; hand < 2; hand++)
		{
			if (hand == 1)
			{
				if (held[1] != NONE && !(bool)((player_weapon_flags(held[1]) >> 4) & 1))
					modes[1] = false;
			}
			else if (!(bool)((player_weapon_flags(weapon_index) >> 4) & 1))
			{
				modes[hand] = false;
				if (held[hand] != NONE && (bool)((player_weapon_flags(held[hand]) >> 4) & 1))
					modes[hand + 2] = false;
			}
		}
	}
}

#include "unit_requests.h"

void function_a90e0(long first, long second);
void function_a9030(long index, short slot);
void function_a8f80(long index, short slot);
bool __stdcall function_cd4e0(long unit_index, short hand, bool flag);
struct s_16e150_bits;
void function_16de20(short index, s_16e150_bits const *table, dword *output);

// @retail 0x151c10
bool function_151c10(long player_index, s_target_candidate *candidate)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (candidate->priority == 8 && function_1518f0(candidate, 2))
	{
		if (g_4e6948->mode == 4)
			function_a90e0(player->unit_index, candidate->object_index);
		else
		{
			s_unit_request request;
			request.type = 0x21;
			*(long *)request.arguments = player->unit_index;
			function_e6900(candidate->object_index, &request);
		}
		result = true;
	}
	return result;
}

// score 17 (was 17):
// Logic and structure match; only the callee-saved registers differ: retail keeps player->unit_index in
// edi and the object data pointer in esi, this build uses esi and edi. Tried (scores): unit_index in a local
// (45), `hand` before the unit_index test (27), unit before hand (20), the held-weapon test through an
// inline helper (17), `short` hand (38), `long` slot (38), `!primary` inline in the index (44).
// Permuter (25 min on r654): best 12, only with a volatile read of the unit index; not kept.
// F352: Complete rejection returns / sole-exit rejection / exhaustive outcomes / retained-false
// dispatch outcomes, saved player receiver: 50/35/48/78; existing lookup at every player
// field: 46/46/46/71. All restored; saved unit/table EDI/ESI still reversed.
// M337: Returned availability owns unit and held-slot admission; bool/byte/short/long
// states with the original short slot [48, 68, 85, 80]; native signed-byte slot [46, 66, 81, 76].
// All restored; the saved unit-index and table/slot registers remain exchanged.
// Round 51 (m3 lane 15): mode arms swapped 27; `primary ? 0 : 1` hand 17; result declared before the player 35;
// e68c0 code from hand 60; unit index local inside the dispatch block 17; dispatch outcome in its own success flag
// copied to result (0x152bf0's matching form) 17. Diagnostic: 1-8 unused entry locals 17 each (not symbol numbering).
// Round 104: unit declared at entry 17; slot declared at entry 17; unit then result before player 35;
// read-only player/unit receivers 17 each; nested slot/weapon tests 17; held-slot array pointer 17;
// weapons array pointer 17. NOT kept: no byte match or gain; original body restored, EDI/ESI still exchanged.
// @retail 0x151b80
bool function_151b80(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
		short slot = unit->held_slots[hand];
		if (slot != NONE && unit->weapons[slot] != NONE)
		{
			if (g_4e6948->mode == 4)
			{
				function_a9030(player->unit_index, hand);
				result = true;
			}
			else
				result = function_e68c0(primary ? 9 : 0x13, player->unit_index);
		}
	}
	return result;
}

// @retail 0x150820
void function_150820(dword *bits, bool local_only)
{
    s_match_globals *table = g_4e0348;
    s_record_pool_iterator iterator;
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    s_tail_player *player;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        if (local_only && player->local_index == NONE) continue;
        short cluster = *(short *)player->unknown2a;
        if (cluster != NONE)
        {
            dword cluster_bits[16];
            function_16de20(cluster, (s_16e150_bits const *)table, cluster_bits);
            for (long word_index = ((table->list_count + 31) >> 5) - 1; word_index >= 0; --word_index)
                bits[word_index] |= cluster_bits[word_index];
        }
    }
}

// score 43 (was 43): the walk found no difference in logic (same stores, calls, argument order, held_slots/weapons
// offsets, mode == 4 byte test, result kept in bl with its stack copy). Only the callee-saved pair is swapped:
// retail keeps the player in esi and the hand in edi (edi pushed late), ours the reverse, and the unit index and the
// `primary` temporary sit in ecx/edx and edx/al instead of edx and cl/eax. Tried: hand declared before the player
// (43), result declared first (43), `primary ? 0 : 1` / `primary == 0` (43), `primary ^ 1` (75), bool hand (89),
// hand after the unit lookup (70), unit_index cached in a local (43), modes declared at the top (43), the weapon and
// mode tests nested (47).
// Permuter (25 min on r654): best 16, only with a volatile read of the unit index and the held slot read as a
// char (a behaviour change); not kept.
// Round 45 (m3 lane 2, fast, base 43): `else if (function_cd4e0(...)) result = true;` 47; `!primary` at each use with
// no hand local 117 (236 bytes); the player fetched at each use 89 (220 bytes); the unit fetched at each use 43
// (same code); hand declared at function scope 43.
// Round 552: guarded command success/direct command/short-circuit result 47/95/87; all admission
// returns 108, absent-unit return 78, branch-owned result 106, success-default result 88, admission
// result before dispatch 101. All original calls, observations and executable body restored.
// NOT kept: no score gain or byte match.
// N345: Isolated player/hand/unit/held-slot/held-weapon/selector/parameter role names
// and the int hand-negation result: all eight 43. Original body and calls restored;
// ESI/EDI player/hand ownership still differs.
// F357: Canonical shared object/header lookup with mutable or read-only
// player/unit receivers: eight 43; existing player/object inline, forced and
// unhinted expansion policies: eight 43. Every other user keeps its score.
// NOT kept: no gain; original helpers, includes, receivers and complete body restored.
// F353: Native hand arm/default producers, original then actual mode/result storage names
// 43/45/45/43/43/45/45/43.
// NOT kept: no gain; original full body/helpers restored.
// Y559: Complementary final failure bool/byte/short/long, conditional then
// guarded command clearing: 85/85/87/89/73/73/87/89. NOT kept: no byte match;
// native ESI player/EDI hand and temporary entry registers remain.
// N345: Isolated player/hand/unit/held-slot/held-weapon/selector/parameter role names
// and the int hand-negation result: all eight 43. Original body and calls restored;
// ESI/EDI player/hand ownership still differs.
// M348: Hand unsigned-long/word/byte/signed-byte 43/81/89/89; positive result
// byte/signed-byte/short/long 92/92/90/91. NOT kept: no byte match;
// native player/hand homes remain. Original body and call tuples restored.
// M350: Immutable captured hand/slot/weapon values, seven subsets: 43 each;
// initialized four-byte mode output 86. Original body and calls restored;
// native ESI player/EDI hand ownership remains.
// @retail 0x151aa0
bool function_151aa0(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
		short slot = unit->held_slots[hand];
		if (slot != NONE)
		{
			long weapon_index = unit->weapons[slot];
			bool modes[4];
			if (weapon_index != NONE && function_cd7b0(player->unit_index, weapon_index, modes) && modes[hand])
			{
				if (g_4e6948->mode == 4)
				{
					function_a8f80(player->unit_index, hand);
					result = true;
				}
				else
					result = function_cd4e0(player->unit_index, hand, false) ? true : result;
			}
		}
	}
	return result;
}

void function_a8f10(long index, long target, short value);
bool function_138880(void);

// @retail 0x151940
bool function_151940(long player_index, s_target_candidate *candidate, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if ((candidate->priority == 1 || candidate->priority == 7) && player_candidate_has_type(candidate, 4))
	{
		long weapon_index = candidate->object_index;
		s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(weapon_index);
		bool modes[4];
		if (*(long *)((byte *)weapon + 0x154) == NONE && function_cd7b0(player->unit_index, weapon_index, modes))
		{
			short mode;
			if (primary)
			{
				if (modes[0]) mode = 3;
				else if (modes[2]) mode = 4;
				else goto done;
			}
			else
			{
				if (modes[1]) mode = 5;
				else if (modes[3]) mode = 6;
				else goto done;
			}
#pragma inline_depth(0)
			if (function_138880())
#pragma inline_depth()
			{
				function_a8f10(player->unit_index, weapon_index, mode);
				result = true;
			}
			else
			{
				s_unit_request request;
				memset(&request, 0, sizeof(request));
				request.type = 0x14;
				*(long *)request.arguments = weapon_index;
				*(short *)(request.arguments + 4) = mode;
				if (function_e6900(player->unit_index, &request)) result = true;
			}
		}
	}
 done:
	return result;
}

void function_a9180(long first, long second);
void function_a9340(long first, long second, short seat);
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *blocker_index, bool *grouped);
void function_107840(long device_index, long unit_index);
bool function_1c9500(long unit_index, long actor_index, bool notify);
void function_1e8f60(long player_index);

// @retail 0x151c90
bool function_151c90(long player_index, s_target_candidate *candidate)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	switch (candidate->priority)
	{
	case 3:
		if (function_1518f0(candidate, 0x380))
		{
			if (g_4e6948->mode == 4)
				function_a9180(player->unit_index, candidate->object_index);
			else
				function_107840(candidate->object_index, player->unit_index);
			function_1e8f60(player_index);
			result = true;
		}
		break;
	case 4:
	case 5:
	case 6:
		if (function_1518f0(candidate, 3))
		{
			short seat = *(short *)&candidate->flags;
			if (seat >= 0)
			{
				long target = candidate->object_index;
				long definition = *(long *)tail_object_get(target);
				byte *tag = g_4e3b44[definition & 0xffff].bytes;
				if (seat < *(long *)(tag + 0x1c8))
				{
					long blocker = NONE;
					bool grouped;
					if (function_c92c0(player->unit_index, target, seat, &blocker, &grouped))
					{
						if (function_138880())
							function_a9340(player->unit_index, candidate->object_index, seat);
						else
						{
							s_unit_request request;
							request.type = 0x1c;
							request.type1c.object_index = candidate->object_index;
							request.type1c.seat_index = seat;
							request.type1c.unknowna = false;
							request.type1c.unknownb = false;
							function_e6900(player->unit_index, &request);
						}
						result = true;
					}
					else if (blocker != NONE)
					{
						byte *unit = (byte *)tail_object_get(blocker);
						if (!function_138880() && *(long *)(unit + 0x12c) != NONE)
							function_1c9500(player->unit_index, *(long *)(unit + 0x12c), true);
						result = true;
					}
				}
			}
		}
		break;
	case 2:
		if (g_4e6948->mode != 4)
		{
			s_unit_request request;
			request.type = 0x15;
			*(long *)request.arguments = player_index;
			result = function_e6900(candidate->object_index, &request);
		}
		break;
	}
	return result;
}

struct s_player_rebind_option
{
    bool active;
    byte unknown01;
    short local_index;
    long controller_index;
    byte unknown08[6];
    byte identifier[12];
    byte unknown1a[0xe4 - 0x1a];
};
struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;
void __stdcall function_167e86(long user_index, long hand);

void __stdcall player_set_local_user(long player_index, long user_index);
void __stdcall function_14f270(long player_index, long controller_index);

// score 285 (was 587): frame 0xe4, 78 bytes short; mask/index homes, release expansion and late binding homes remain.
// The existing controller iterator restores the native memory cursor and pool reload (plain 285).
// Earlier 587 (was 628): the unbind in the second-to-last loop follows retail's expansion of 0x14f190/0x14f270's
// NONE arms (through freshly fetched player pointers), the machine fields come from s_player_census_counts, the
// iterator sits in a struct after a pointer (retail's frame 0xe4: a free slot at +0x24), and the arrays are filled
// by loops (the preamble matches retail instruction for instruction). Retail also expands 0x1682bf(NONE, user,
// NONE) here (0x1535b8-0x1535f2); ours calls it, as unknown_1682bf.cpp builds with /Ob1. Measured trial, not kept
// (a written-out copy stays only with a byte match): 0x1682bf's NONE arm written out here as a forceinline
// helper gives 497, so that inlined shape is the right one. Still differs: retail keeps iterator.index in memory
// in the second (controller/user choice) loop, ours keeps it in a register (retail +0x177: 8 more
// instructions there, so everything after is shifted); retail compares with immediate -1, ours sometimes keeps
// -1 in a register; the rebind arm passes `user` (short) from a register.
// Tried: iterator beside the free slot as separate locals (545), explicit `= {NONE...}` controllers (556),
// the three masks in all six declaration orders (543-559), `index` at function scope (545), the loop
// over two weapons as `for` with an index (555 unrolled) or as a helper (510, unrolled).
// N341: retained local-index short, controller/user loop admission and failure guards 587 each;
// real datum-iterator API 934; absolute cursor subscripts 586; subscript name 587.
// NOT kept: one-point cursor reduction; native mask/index homes and missing inline expansion remain.
// N343: Six semantic mask-home naming forms and independent traversal/pass home names 587 each.
// Original names and complete body restored; native mask/index homes, memory cursor and
// missing inline expansion remain. NOT kept: unmatched storage-only renaming.
// P295: Existing option/user/controller vector setters 618/543/663/576;
// complete test/set families 629/536/644/435. NOT kept: scalar masks become
// indexed word addresses with sar 5, absent in retail; original mask code restored.
// DH323: Actual controller/user/both output-array search accumulators, decoded
// then absolute ordinal 587/587/578/588/587/443; controller/user lvalues 587/587.
// NOT kept: native searches retain EAX/EBP results before final array stores;
// array workspaces shift mask/iterator homes despite 443 at 1898/1992 bytes.
// M344: Complete default-owned acceptance/rejection pass outcomes in bool/byte/short/long
// storage 668 each (1850/1992 bytes). NOT kept: outcome flags do not match and shorten
// the body; original switch, masks, traversal state, helpers and complete body restored.
// Y563: Eight existing pass-switch case orders, including 2/1/default/0: 587 each.
// Original Boolean result and mask/index homes persist; complete source restored.
// NOT kept: no gain or byte match; native memory cursor and inline expansion remain.
// N353: Existing controller iterator/user release/controller release subsets 1-7
// 285/587/285/587/285/587/285; complete release graph with a bounded controller loop 604.
// Keep only the existing controller iterator (plain 285, 1914/1992 bytes); release calls and bounded loop restored.
// @retail 0x152f80
void __stdcall function_152f80(void *address, void *option_data)
{
    byte const *machine_address = (byte const *)address;
    s_player_rebind_option const *options = (s_player_rebind_option const *)option_data;
    long controllers[16];
    long users[16];
    long assigned[16];
    dword options_used = 0;
    dword users_used = 0;
    dword controllers_used = 0;
    for (long i = 0; i < 16; i++)
        controllers[i] = NONE;
    for (long i = 0; i < 16; i++)
    {
        assigned[i] = NONE;
        users[i] = NONE;
    }
    struct
    {
        s_tail_player *player;
        s_record_pool_iterator iterator;
    } state;
    state.iterator.data = g_4e8c24;
    for (long pass = 0; pass < 4; pass++)
    {
        state.iterator.index = NONE;
        while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
        {
            long index = (short)state.iterator.datum_index;
            if (assigned[index] != NONE) continue;
            for (long option = 0; option < 16; option++)
            {
                s_player_rebind_option const *entry = &options[option];
                if (!entry->active || (options_used & (1 << option))) continue;
                bool match;
                switch (pass)
                {
                case 0:
                    match = state.player->local_index != NONE && !memcmp(entry->identifier, state.player->unknown04, 12);
                    break;
                case 1:
                    match = state.player->local_index != NONE && entry->controller_index == state.player->controller_index;
                    break;
                case 2:
                    match = state.player->local_index != NONE;
                    break;
                default:
                    match = true;
                    break;
                }
                if (!match) continue;
                users[index] = entry->local_index;
                assigned[index] = option;
                controllers[index] = entry->controller_index;
                options_used |= 1 << option;
                users_used |= 1 << entry->local_index;
                controllers_used |= 1 << entry->controller_index;
                break;
            }
        }
    }
    state.iterator.index = NONE;
    while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
    {
        long index = (short)state.iterator.datum_index;
        if (assigned[index] != NONE || state.player->local_index == NONE) continue;
        long controller = NONE;
        long user = NONE;
        for (long pass = 0; pass < 3; pass++)
        {
            if (controller != NONE) break;
            for (long candidate = 0; candidate != NONE; candidate = controller_index_next(candidate))
            {
                if (!(controllers_used & (1 << candidate)) &&
                    (pass > 0 || candidate == state.player->controller_index) &&
                    (pass > 1 || g_4e61cc[(short)candidate]))
                {
                    controller = candidate;
                    break;
                }
            }
        }
        for (long pass = 0; pass < 2; pass++)
        {
            if (user != NONE) break;
            for (long candidate = 0; candidate < 4; candidate++)
            {
                if (!(users_used & (1 << candidate)) && (pass > 0 || candidate == state.player->local_index))
                {
                    user = candidate;
                    break;
                }
            }
        }
        controllers[index] = controller;
        users[index] = user;
        users_used |= 1 << user;
        controllers_used |= 1 << controller;
    }
    s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
    if (globals->local_machine_index != NONE)
    {
        state.iterator.index = NONE;
        while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
            if (state.player->local_index != NONE) memcpy(state.player->machine_address, machine_address, 6);
        memcpy(globals->local_machine_address, machine_address, 6);
        memcpy(globals->machine_addresses[globals->local_machine_index], machine_address, 6);
    }
    else
    {
        globals->local_machine_valid = true;
        memcpy(globals->local_machine_address, machine_address, 6);
        globals->local_machine_index = 0;
        globals->machine_valid_mask |= 1;
        memcpy(globals->machine_addresses[0], machine_address, 6);
    }
    state.iterator.data = g_4e8c24;
    state.iterator.index = NONE;
    while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
    {
        long index = (short)state.iterator.datum_index;
        if (state.player->local_index != NONE && (state.player->local_index != users[index] || state.player->controller_index != controllers[index]))
        {
            s_tail_player *unbound = tail_player_get(state.iterator.datum_index);
            if (unbound->local_index != NONE)
            {
                player_control_set_unit(unbound->local_index, NONE);
                function_1682bf(NONE, unbound->local_index, NONE);
                ((s_player_census_counts *)g_4e8c20)->users[unbound->local_index] = NONE;
                ((s_player_census_counts *)g_4e8c20)->user_count--;
                unbound->local_index = NONE;
            }
            unbound = tail_player_get(state.iterator.datum_index);
            if (unbound->controller_index != NONE)
            {
                ((s_player_census_counts *)g_4e8c20)->controllers[unbound->controller_index] = NONE;
                ((s_player_census_counts *)g_4e8c20)->controller_count--;
                unbound->controller_index = NONE;
            }
            state.player->machine_user = NONE;
            state.player->machine_controller = NONE;
        }
    }
    state.iterator.data = g_4e8c24;
    state.iterator.index = NONE;
    while ((state.player = (s_tail_player *)data_iterator_next_inlined(&state.iterator)) != 0)
    {
        long index = (short)state.iterator.datum_index;
        if (users[index] == NONE) continue;
        memcpy(state.player->machine_address, machine_address, 6);
        state.player->machine_index = (short)((s_player_census_counts *)g_4e8c20)->local_machine_index;
        if (state.player->local_index == NONE)
        {
            short user = (short)users[index];
            state.player->machine_user = user;
            state.player->machine_controller = controllers[index];
            player_set_local_user(state.iterator.datum_index, user);
            function_14f270(state.iterator.datum_index, state.player->machine_controller);
        }
        if (TEST_FIELD_BIT(g_54e8e0[state.player->controller_index].flag4)) *(long *)g_54e8e0[state.player->controller_index].unknown004 = state.player->local_index;
    }
}

#include "data_array.h"
void __stdcall function_14be90(long player_index, dword const *configuration);

struct s_player_configuration_copy
{
    dword words[0x24];
};

// @retail 0x1523c0
void function_1523c0()
{
    s_record_pool *players = g_4e8c24;
    long index = NONE;
    for (;;)
    {
        long next = data_find_index(players, index + 1);
        if (next == NONE)
            break;
        byte *player = players->data + players->size * next;
        long datum = ((long)*(short const volatile *)player << 16) | next;
        s_player_configuration_copy configuration;
        index = next;
        memcpy(&configuration, player + 0xd4, sizeof(configuration));
        function_14be90(datum, configuration.words);
    }
}


struct s_tag_iterator
{
    long unknown00, unknown04, datum_index, next_index, group_tag;
};
struct s_loadout_state_152a50 { short kind, rounds, total, depleted; };
struct s_effect_owner;
long function_122c70(s_tag_iterator *iterator);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);

// @retail 0x152a50
long function_152a50(s_loadout_state_152a50 const *state, long owner_index, bool *failed)
{
    long const *owner_reference = &owner_index;
    bool *const *failed_reference = &failed;
    long result = NONE;
    if (state->kind != 0)
    {
        s_tag_iterator iterator;
        iterator.next_index = 0;
        iterator.group_tag = 0x77656170;
        long tag;
        while ((tag = function_122c70(&iterator)) != NONE)
        {
            byte *definition = g_4e3b44[tag & 0xffff].bytes;
            if (*(long *)(definition + 0x2d0) > 0)
            {
                byte *seat = *(byte **)(definition + 0x2d4);
                long ammunition = *(long *)(seat + 0x90);
                if (ammunition != NONE)
                {
                    if (state->kind == (short)g_4e3b44[ammunition & 0xffff].bytes[0x128]) break;
                }
                else if (*(long *)(seat + 0x98) != NONE && state->kind == (short)seat[0x88]) break;
            }
            if (state->kind == (short)definition[0x1fc]) break;
        }
        if (tag != NONE)
        {
            byte creation[0xc4];
            function_b7930(creation, tag, *owner_reference, NULL);
            result = function_b7b40(creation);
            if (result != NONE)
            {
                byte *object = (byte *)tail_object_get(result);
                byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
                if (*(long *)(definition + 0x2c0) > 0)
                {
                    *(short *)(object + 0x22c) = state->total;
                    *(short *)(object + 0x22a) = state->rounds;
                }
                *(real *)(object + 0x184) = 1.0f - (real)state->depleted * 0.01f;
            }
            else **failed_reference = true;
        }
        else if (state->kind != 0) **failed_reference = true;
    }
    return result;
}



#include <math.h>
byte function_150380(long player_index);
void function_e64e0(vector3f const *reference, vector3f const *direction, vector3f const *fallback,
    bool three_dimensional, vector3f *result, short *axis_index);

struct s_xy_150400 { real i; real j; };

// @retail 0x150400
void function_150400(long player_index, vector3f const *forward, vector3f const *movement,
    vector3f *new_forward, vector3f *new_movement)
{
    vector3f const *const *local_41f0cc_2 = &forward;
    vector3f *const *new_forward_reference = &new_forward;
    vector3f *const *new_movement_reference = &new_movement;
    s_tail_player *player = tail_player_get(player_index);
    vector3f facing = *(*local_41f0cc_2);
    s_xy_150400 xy = *(s_xy_150400 const *)movement;
    if (function_150380(player_index))
    {
        byte *unit = (byte *)tail_object_get(player->unit_index);
        s_horizontal_axes axes;
        function_1502e0(*local_41f0cc_2, (vector3f const *)(unit + 0x70), &axes);
        vector3f const *axis_forward = &axes.forward;
        vector3f const *axis_left = &axes.left;
        real move_i = movement->i;
        vector3f world;
        world.i = axis_forward->i * move_i;
        world.j = axis_forward->j * move_i;
        world.k = axis_forward->k * move_i;
        real move_j = movement->j;
        world.i += axis_left->i * move_j;
        world.j += axis_left->j * move_j;
        world.k += axis_left->k * move_j;
        function_e64e0(&world, *local_41f0cc_2, (vector3f const *)(unit + 0x70), false, &facing, NULL);
        function_1502e0(&facing, (vector3f const *)(unit + 0x70), &axes);
        xy.i = axes.forward.j * world.j + axes.forward.k * world.k + axes.forward.i * world.i;
        xy.j = axes.left.k * world.k + axes.left.j * world.j + axes.left.i * world.i;
        real square = xy.j * xy.j + xy.i * xy.i;
        if (square > 1.0f)
        {
            real inverse = 1.0f / (real)sqrt(square);
            xy.i *= inverse;
            xy.j *= inverse;
        }
    }
    **new_forward_reference = facing;
    (*new_movement_reference)->j = xy.j;
    (*new_movement_reference)->i = xy.i;
    (*new_movement_reference)->k = 0.0f;
}



void __stdcall function_ccff0(long unit_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);

// @retail 0x152bf0
bool function_152bf0(byte const *loadout, long unit_index)
{
    bool result = false;
    if (loadout[0])
    {
        byte *unit = (byte *)tail_object_get(unit_index);
        function_ccff0(unit_index);
        long primary, special, secondary;
        bool success;
        {
            bool failed = false;
            primary = function_152a50((s_loadout_state_152a50 *)(loadout + 2), unit_index, &failed);
            special = function_152a50((s_loadout_state_152a50 *)(loadout + 0x12), unit_index, &failed);
            secondary = function_152a50((s_loadout_state_152a50 *)(loadout + 0xa), unit_index, &failed);
            success = !failed;
        }
        for (long i = 0; i < 2; ++i)
        {
            unit[0x23e + i] = 0;
            unit[0x23e + i] = loadout[0x1a + i];
        }
        if (primary != NONE && !function_cd0c0(unit_index, primary, 1))
        { function_b8540(primary); success = false; }
        if (secondary != NONE && !function_cd0c0(unit_index, secondary, 1))
        { function_b8540(secondary); success = false; }
        if (special != NONE && !function_cd0c0(unit_index, special, 7))
        { function_b8540(special); return false; }
        result = success;
    }
    return result;
}

bool function_cb810(long unit_index, vector3f *vector);
// @retail 0x1505f0
void function_1505f0(long player_index, long unit_index, byte const *input)
{
    (void)&unit_index; (void)&input;
    s_tail_player *player = tail_player_get(player_index);
    long unit = player->unit_index;
    long current_unit = player->unit_index;
    real cos_second_angle = (real)cos(*(real const *)(input + 8));
    vector3f facing;
    facing.i = (real)(cos(*(real const *)(input + 4)) * cos_second_angle);
    facing.j = (real)(sin(*(real const *)(input + 4)) * cos_second_angle);
    facing.k = (real)sin(*(real const *)(input + 8));
    if (current_unit != NONE) function_cb810(unit, &facing);
    vector3f forward_copy, projected_movement;
    function_150400(player_index, &facing, (vector3f const *)(input + 0xc), &forward_copy, &projected_movement);
    byte control[0x7c];
    function_c6ef0((s_unit_state_c6ef0 *)control);
    *(dword *)(control + 0x10) = *(dword const *)input;
    *(vector3f *)(control + 0x34) = facing;
    *(vector3f *)(control + 0x40) = facing;
    *(vector3f *)(control + 0x28) = forward_copy;
    *(vector3f *)(control + 0x14) = projected_movement;
    *(real *)(control + 0x20) = *(real const *)(input + 0x14);
    *(real *)(control + 0x24) = *(real const *)(input + 0x18);
    *(dword *)(control + 6) = *(dword const *)(input + 0x1e);
    *(word *)(control + 0xa) = *(word const *)(input + 0x22);
    *(word *)(control + 0xc) = *(word const *)(input + 0x24);
    *(dword *)control = 0x06000086;
    *(word *)(control + 4) = 0;
    memcpy(control + 0x58, input + 0x34, 0x24);
    dword *flags = (dword *)((byte *)tail_object_get(unit_index) + 0x134);
    if (input[0x58]) *flags |= 0x40000000;
    else *flags &= ~0x40000000;
    function_c6de0(unit_index, control);
}



short __stdcall function_bb050(long, dword, void const *, point3f const *, real, long *, short);

// @retail 0x151110
void __stdcall function_151110(long player_index, void *output)
{
    s_tail_player *player = tail_player_get(player_index);
    memset(output, 0, 12);
    memset(output, 0, 8);
    *(long *)((byte *)output + 4) = NONE;
    *(long *)((byte *)output + 8) = NONE;
    long unit_index = player->unit_index;
    if (unit_index == NONE) return;
    byte *unit = (byte *)tail_object_get(unit_index);
    if (*(long *)(unit + 0x14) != NONE) return;
    struct s_region { real radius; point3f *center; } region;
    region.radius = *(real *)(unit + 0x3c) + 0.4f;
    region.center = (point3f *)(unit + 0x30);
    long objects[64];
    long count = function_bb050(0, 0xffffebcf, unit + 0x28, region.center, region.radius + 3.0f, objects, 64);
    for (long i = 0; i < count; ++i)
    {
        long object_index = objects[i];
        byte *object = (byte *)tail_object_get(object_index);
        long type = *(signed char *)(object + 0xaa);
        dword mask = 1 << type;
        if (mask & 0x1430) continue;
        if (mask & 0x184)
        {
            real reach = *(real *)(object + 0x3c) + region.radius;
            real x = *(real *)(object + 0x30) - region.center->x;
            real y = *(real *)(object + 0x34) - region.center->y;
            real z = *(real *)(object + 0x38) - region.center->z;
            real distance = x * x;
            distance += z * z;
            distance += y * y;
            if (!(distance <= reach * reach)) continue;
        }
        s_target_candidate *best = (s_target_candidate *)output;
        switch (type)
        {
        case 2: function_151780(object_index, player_index, best); break;
        case 8: function_1516c0(object_index, player_index, best); break;
        case 1: function_151430(object_index, player_index, best); break;
        case 0: function_1515e0(object_index, player_index, best); break;
        default:
            {
                long child_index = *(long *)(object + 0x10);
                while (child_index != NONE && (unsigned long)count < 64)
                {
                    objects[count++] = child_index;
                    child_index = *(long *)((byte *)tail_object_get(child_index) + 0xc);
                }
            }
            break;
        }
    }
}

bool __stdcall function_ffc60(long weapon_index, long other_index, long unit_index, long local_user, short *rounds);
bool __stdcall function_ccba0(long unit_index, long grenade_index);
long function_ccb80(long unit_index);
void function_15e8e0(long object_index, long player_index);
void __stdcall function_191ec4(long definition_index, long player_index, short count);
void __stdcall function_191e51(long definition_index, long player_index);

// score 202 (was 202): Frame 0x28 agrees; player data uses ECX versus EDX,
// the object uses EBP versus ESI and the scaled header offset spills instead of staying in EBP.
// Earlier: the prologue's registers differ: retail keeps the player base in ebx
// and the object index*12 in ebp, ours gives ebp to the player and spills the index; the
// argument convention of the callee 0xccba0 (src/unit_object_type.cpp) decides this. Logic walked against retail: no
// behaviour difference found. Retail re-reads [ebx+0x2c] at 0x150ec0 and g_4e0300->data at 0x150e90 (edx still holds
// g_4e0300) where ours reuses edx/ecx, which costs ours the ebp register. Tried: a local unit_index (202), the 0x14c compare
// through a second tail_player_get (238), g_4e0300 in a local (242), an explicit object offset kept in a local (202),
// the parent test without the bool (227).
// AC500: Native unit/object field views and existing complete difference producer, masks 1-7,
// 202/202/202/203/203/203/203; complete graph with byte header kind 203. Original real layouts,
// fields, helper use and complete body restored; frame agrees, early lookup ownership and offset spill remain.
// AC502: Positive unit/parent/owner/radius/all admissions and independent final eligibility
// 202 each; common notification alone/with complete admissions 232/232 (688 bytes).
// Original body restored; entry player/header-offset ownership and spill remain.
// Round 51 (m3 lane 15): rounds scoped in the loop body 202 (modes still takes the object_index argument home;
// retail shares rounds' slot); parent retest through parent_index 227; weapon_index declared in the loop 202; final
// dispatch as if/else with one notification 232; offsets declared z, x, y 203.
// Y557: Native signed-short scan as for/while 242/202; counter live from
// entry 263/225; iteration-owned weapon 242/202; both lifetimes 263/225.
// Original do-loop, counter position and weapon lifetime restored. Frame 0x28
// agrees; player-data ownership and the EBP header-offset spill remain.
// Entry-scope rounds/mode/request output subsets 1/2/4/3/5/6/7:
// 202/202/204/202/204/204/204; all with role-based output names 204.
// Original declarations, producers and body restored; player-data/header-offset
// ownership and the shared rounds/mode output home remain.
// B397: Complete four-slot transfer admission in bool/byte/short/long, then
// rejection complement 202/202/202/202/202/202/202/202. Original call/rounds/loop graph restored;
// player-data ownership and the scaled header-offset spill remain.
// @retail 0x150e30
void __stdcall function_150e30(long player_index, long object_index)
{
    s_tail_player *player = tail_player_get(player_index);
    if (player->unit_index == NONE) return;
    byte *unit = (byte *)tail_object_get(player->unit_index);
    byte *object = (byte *)tail_object_get(object_index);
    long parent_index = *(long *)(object + 0x14);
    bool parent_ok = false;
    if (parent_index != NONE)
    {
        long parent_type = ((s_tail_object_header *)g_4e0300->data)[parent_index & 0xffff].type;
        parent_ok = parent_type == 11 || parent_type == 7;
    }
    if (*(long *)(object + 0x14) != NONE && !parent_ok) return;
    if (*(long *)(object + 0x14c) == player->unit_index) return;
    real y = *(real *)(object + 0x34) - *(real *)(unit + 0x34);
    real x = *(real *)(object + 0x30) - *(real *)(unit + 0x30);
    real z = *(real *)(object + 0x38) - *(real *)(unit + 0x38);
    real radius = *(real *)(unit + 0x3c) + 0.3f;
    if (!(z * z + x * x + y * y < radius * radius)) return;
    long weapon_index = NONE;
    short rounds;
    short i = 0;
    do
    {
        long current = *(signed char *)(unit + 0x212);
        current = current < 0 ? 0 : current;
        long slot = (current + i) % 4;
        weapon_index = *(long *)(unit + 0x218 + slot * 4);
        if (weapon_index != NONE && function_ffc60(weapon_index, object_index, player->unit_index, player->local_index, &rounds))
        {
            if (rounds > 0)
                function_191ec4(*(long *)tail_object_get(weapon_index), player->local_index, rounds);
            break;
        }
        ++i;
    } while (i < 4);
    s_object *pickup = function_badc0(object_index, 8);
    if (pickup)
    {
        byte *definition = g_4e3b44[*(long *)pickup & 0xffff].bytes;
        short kind = *(short *)(definition + 0x12c);
        if (kind == 6)
        {
            if (function_ccba0(player->unit_index, object_index))
                function_191e51(*(long *)pickup, player->local_index);
        }
        else if (kind != 0 && function_ccb80(player->unit_index) == NONE && !function_138880())
            function_151ea0(player_index, object_index);
    }
    byte type = ((s_tail_object_header *)g_4e0300->data)[object_index & 0xffff].type;
    if ((1 << type) & 4)
    {
        bool modes[4];
        if (function_cd7b0(player->unit_index, object_index, modes) && modes[0] && function_150dd0(player_index))
        {
            if (function_138880())
            {
                function_a8f10(player->unit_index, object_index, 3);
                function_15e8e0(object_index, player_index);
                return;
            }
            s_unit_request request;
            memset(&request, 0, sizeof(request));
            request.type = 0x14;
            *(long *)request.arguments = object_index;
            *(short *)(request.arguments + 4) = 3;
            function_e6900(player->unit_index, &request);
        }
    }
    function_15e8e0(object_index, player_index);
}


// @retail 0x1508e0
void __stdcall function_1508e0(long player_index)
{
    s_tail_player *player = tail_player_get(player_index);
    long objects[16];
    if (player->unit_index != NONE)
    {
        byte *unit = (byte *)tail_object_get(player->unit_index);
        if (*(long *)(unit + 0x14) == NONE)
        {
            long count = function_bb050(0, 0x89c, unit + 0x28, (point3f *)(unit + 0x30),
                *(real *)(unit + 0x3c) + 0.3f, objects, 16);
            for (long i = 0; i < count; ++i)
            {
                long index = objects[i];
                byte *object = (byte *)tail_object_get(index);
                long type = *(signed char *)(object + 0xaa);
                if (type >= 2 && type <= 3) function_150e30(player_index, index);
                if (object[0xaa] == 11 || object[0xaa] == 7)
                {
                    long child = *(long *)(object + 0x10);
                    while (child != NONE && (unsigned long)count < 16)
                    {
                        objects[count++] = child;
                        child = *(long *)((byte *)tail_object_get(child) + 0xc);
                    }
                }
            }
        }
    }
}

