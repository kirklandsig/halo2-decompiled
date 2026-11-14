// @flags /O2 /Ob1 /arch:SSE /Gr
/* TEAM_BALANCING.CPP: the assignment of players to teams. Players (with a
   weight) are grouped into parties, parties are dealt to the team with the
   fewest players, and pairs of parties are then swapped between teams for as
   long as that makes the teams more even. */

#include "unknown_11c920.h"
#include <string.h>
#include <new>

#define MAXIMUM_PLAYERS 16
#define MAXIMUM_PARTIES 16
#define MAXIMUM_TEAMS 4

struct s_balance_player
{
	long weight;
	long index;

	s_balance_player();
	bool has_index(long player_index) const
	{
		bool result = false;
		if (index == player_index)
			result = true;
		return result;
	}
};

struct s_balance_party
{
	bool flagged;
	long id;
	long count;
	long total;
	s_balance_player players[MAXIMUM_PLAYERS];

	s_balance_party();
	void reset()
	{
		count = 0;
		total = 0;
		flagged = false;
		id = NONE;
	}
	/* whether slot i holds a player; the loops that walk a party test it
	   at the top of each pass */
	bool slot_in_use(long i) const
	{
		bool result = false;
		if (i < count)
			result = true;
		return result;
	}
	bool same_id(s_balance_party const *other) const
	{
		bool result = false;
		if (id == other->id)
			result = true;
		return result;
	}
	void add_player(s_balance_player const *player);
	void remove_player(s_balance_player const *player);
};

struct s_balance_team
{
	long party_count;
	long flagged_count;
	s_balance_party parties[MAXIMUM_PARTIES];
	s_balance_party players;

	s_balance_team();
	void reset()
	{
		party_count = 0;
		flagged_count = 0;
		players.reset();
		players.id = 0;
	}
	void add_party(s_balance_party const *party);
	void remove_party(s_balance_party const *party);
	void remove_largest_party(s_balance_party *party);
};

s_balance_team *__stdcall function_91620(s_balance_team *team);

// The stored teams have trivial copy and destruction.
union s_balance_team_array
{
	long alignment;
	byte data[sizeof(s_balance_team) * MAXIMUM_TEAMS];
	s_balance_team &operator[](long i) { return ((s_balance_team *)data)[i]; }
	s_balance_team const &operator[](long i) const { return ((s_balance_team const *)data)[i]; }
};

struct s_balance_solution
{
	long team_count;
	s_balance_team_array teams;

	s_balance_solution()
	{
		for (long i = 0; i < MAXIMUM_TEAMS; ++i)
			function_91620(&teams[i]);
		team_count = 0;
	}
	void reset(long count);
	long total_spread() const;
	long count_spread() const;
	long rank_spread() const;
	long smallest_team() const;
	long cost() const
	{
		return 7 * total_spread() + 3 * rank_spread();
	}
};

struct s_balance_problem
{
	bool success;
	s_balance_solution solution;
	long maximum_team_size;

	s_balance_problem();
};

struct s_balance_optimizer
{
	s_balance_solution current;
	bool improved;
	s_balance_solution best;

	s_balance_optimizer();
	void try_swap(long team_a, long team_b);
	bool improve();
	bool optimize(s_balance_solution const *solution);
};

// @retail 0x91530
inline s_balance_player::s_balance_player()
{
	weight = NONE;
	index = NONE;
}

// @retail 0x91540
inline s_balance_party::s_balance_party()
{
	reset();
}

// @retail 0x91580
void s_balance_party::add_player(s_balance_player const *player)
{
	long i = count;
	while (i > 0)
	{
		if (players[i - 1].weight >= player->weight)
			break;
		players[i] = players[i - 1];
		i--;
	}
	players[i] = *player;
	count++;
	total += player->weight;
}

// @retail 0x915d0
void s_balance_party::remove_player(s_balance_player const *player)
{
	long index = player->index;
	long i = 0;
	while (!players[i].has_index(index))
		i++;
	count--;
	for (; i < count; i++)
		players[i] = players[i + 1];
	total -= player->weight;
}

__forceinline s_balance_team::s_balance_team()
{
	reset();
}

// @retail 0x91620
s_balance_team *__stdcall function_91620(s_balance_team *team)
{
	__assume(team != 0);
	s_balance_team *const *team_slot = &team;
	return new (*team_slot) s_balance_team;
}

// @retail 0x91690
void s_balance_team::add_party(s_balance_party const *party)
{
	parties[party_count] = *party;
	party_count++;
	for (long i = 0; party->slot_in_use(i); i++)
		players.add_player(&party->players[i]);
	if (party->flagged)
		flagged_count++;
}

// @retail 0x916e0
void s_balance_team::remove_party(s_balance_party const *party)
{
	long i = 0;
	while (!parties[i].same_id(party))
		i++;
	party_count--;
	for (; i < party_count; i++)
		parties[i] = parties[i + 1];
	for (long j = 0; party->slot_in_use(j); j++)
		players.remove_player(&party->players[j]);
	if (party->flagged)
		flagged_count--;
}

// @retail 0x91760
void s_balance_team::remove_largest_party(s_balance_party *party)
{
	s_balance_party *largest = &parties[0];
	for (long i = 1; i < party_count; i++)
	{
		if (parties[i].count > largest->count ||
			(parties[i].count == largest->count && parties[i].total > largest->total))
		{
			largest = &parties[i];
		}
	}
	*party = *largest;
	remove_party(party);
}

// @retail 0x917c0
void s_balance_solution::reset(long count)
{
	team_count = count;
	for (long i = 0; i < count; i++)
		teams[i].reset();
}

PRIVATE __forceinline long function_91801(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.total;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.total;
		if (result <= value) result = value;
	}
	return result;
}

PRIVATE __forceinline long function_91802(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.total;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.total;
		if (result > value) result = value;
	}
	return result;
}

// @retail 0x91800
long s_balance_solution::total_spread() const
{
	long maximum = function_91801(this);
	long minimum = function_91802(this);
	return maximum - minimum;
}

PRIVATE __forceinline long function_91861(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.count;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.count;
		if (result <= value) result = value;
	}
	return result;
}

PRIVATE __forceinline long function_91862(s_balance_solution const *solution)
{
	long result = solution->teams[0].players.count;
	for (long i = 1; i < solution->team_count; ++i)
	{
		long value = solution->teams[i].players.count;
		if (result > value) result = value;
	}
	return result;
}

// @retail 0x91860
long s_balance_solution::count_spread() const
{
	/* the results are written through references: a store through a pointer here is what
	   makes try_swap (0x91bd0) query the candidate's spread ahead of the reference's */
	long extremes[2];
	long &maximum = extremes[1];
	long &minimum = extremes[0];
	maximum = function_91861(this);
	minimum = function_91862(this);
	return maximum - minimum;
}

// @retail 0x918c0
long s_balance_solution::rank_spread() const
{
	long largest = teams[0].players.count;
	long i;
	for (i = 1; i < team_count; i++)
	{
		if (largest <= teams[i].players.count)
			largest = teams[i].players.count;
	}
	long sum = 0;
	for (long rank = 0; rank < largest; rank++)
	{
		long maximum = rank < teams[0].players.count ? teams[0].players.players[rank].weight : 0;
		long minimum = maximum;
		for (i = 1; i < team_count; i++)
		{
			long weight = rank < teams[i].players.count ? teams[i].players.players[rank].weight : 0;
			if (maximum <= weight)
				maximum = weight;
			if (minimum > weight)
				minimum = weight;
		}
		sum += maximum - minimum;
	}
	return sum;
}

// @retail 0x919a0
long s_balance_solution::smallest_team() const
{
	long smallest = 0;
	for (long i = 1; i < team_count; i++)
	{
		if (teams[i].players.count < teams[smallest].players.count ||
			(teams[i].players.count == teams[smallest].players.count && teams[i].players.total < teams[smallest].players.total))
		{
			smallest = i;
		}
	}
	return smallest;
}

// @retail 0x91a00
s_balance_problem::s_balance_problem()
{
	success = false;
}

// @retail 0x91b30
s_balance_optimizer::s_balance_optimizer()
{
	improved = false;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x91a30
bool balance_teams_greedy(s_balance_problem *problem, long team_count, s_balance_team const *pool, long maximum_team_size)
{
	problem->maximum_team_size = maximum_team_size;
	problem->success = false;
	s_balance_solution *solution = &problem->solution;
	solution->reset(team_count);
	s_balance_team remaining = *pool;
	while (remaining.party_count > 0)
	{
		s_balance_party party;
		remaining.remove_largest_party(&party);
		long target = solution->smallest_team();
		_ReadWriteBarrier();
		problem->solution.teams[target].add_party(&party);
	}
	long largest = solution->teams[0].players.count;
	for (long i = 1; i < solution->team_count; i++)
	{
		if (largest <= solution->teams[i].players.count)
			largest = solution->teams[i].players.count;
	}
	bool success = largest <= maximum_team_size;
	problem->success = success;
	return success;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x91bd0
void s_balance_optimizer::try_swap(long team_a, long team_b)
{
	s_balance_team const *a = &current.teams[team_a];
	s_balance_team const *b = &current.teams[team_b];
	long difference = a->players.count - b->players.count;
	if (difference < 0)
		difference = -difference;
	for (long i = 0; i < a->party_count; i++)
	{
		for (long j = 0; j < b->party_count; j++)
		{
			s_balance_party const *party_a = &a->parties[i];
			s_balance_party const *party_b = &b->parties[j];
			if (party_a->flagged != party_b->flagged)
				continue;
			long delta = party_a->count - party_b->count;
			long new_count_a = a->players.count - delta;
			long new_count_b = b->players.count + delta;
			long new_difference = new_count_a - new_count_b;
			if (new_difference < 0)
				new_difference = -new_difference;
			if (new_difference > difference)
				continue;
			s_balance_solution candidate = current;
			candidate.teams[team_a].remove_party(party_a);
			candidate.teams[team_b].remove_party(party_b);
			candidate.teams[team_a].add_party(party_b);
			candidate.teams[team_b].add_party(party_a);
			s_balance_solution const *reference = improved ? &best : &current;
			if (candidate.count_spread() < reference->count_spread() ||
				(candidate.count_spread() == reference->count_spread() && candidate.cost() < reference->cost()))
			{
				improved = true;
				best = candidate;
			}
		}
	}
}

// @retail 0x91dd0
bool s_balance_optimizer::improve()
{
	improved = false;
	for (long i = 0; i < current.team_count - 1; i++)
	{
		for (long j = i + 1; j < current.team_count; j++)
			try_swap(i, j);
	}
	if (improved)
		current = best;
	return improved;
}

// @retail 0x91b90
bool s_balance_optimizer::optimize(s_balance_solution const *solution)
{
	bool result = false;
	current = *solution;
	if (improve())
	{
		result = true;
		while (improve());
	}
	return result;
}

static __forceinline void function_91fc2(s_balance_party *arg_0, s_balance_player const *arg_1)
{
    long local_0 = arg_0->count;
    while (local_0 > 0)
    {
        if (arg_0->players[local_0 - 1].weight >= arg_1->weight)
            break;
        arg_0->players[local_0] = arg_0->players[local_0 - 1];
        --local_0;
    }
    arg_0->players[local_0] = *arg_1;
    arg_0->count++;
    arg_0->total += arg_1->weight;
}

static __forceinline void function_91fc1(s_balance_team *arg_0, s_balance_party const *arg_1)
{
    arg_0->parties[arg_0->party_count] = *arg_1;
    arg_0->party_count++;
    for (long i = 0; arg_1->slot_in_use(i); i++)
        function_91fc2(&arg_0->players, &arg_1->players[i]);
    if (arg_1->flagged)
        arg_0->flagged_count++;
}

// @retail 0x91fc0
bool balance_teams(long team_count, long maximum_team_size, bool flag_parties, long flag_minimum, long flag_maximum,
	bool use_parties, long player_count, long const *party_indices, long const *weights, bool optimize, long *team_indices)
{
	s_balance_party parties[MAXIMUM_PARTIES];
	long i;
	for (i = 0; i < player_count; i++)
	{
		long party_index = use_parties ? party_indices[i] : i;
		s_balance_player player;
		player.index = i;
		player.weight = weights[i];
		parties[party_index].add_player(&player);
	}

	long pool_storage[sizeof(s_balance_team) / sizeof(long)];
	s_balance_team &pool = *(s_balance_team *)pool_storage;
	function_91620(&pool);
	for (i = 0; i < MAXIMUM_PARTIES; i++)
	{
		if (parties[i].count > 0)
		{
			s_balance_party *party = &parties[i];
			party->id = i;
			if (flag_parties && party->count >= flag_minimum && party->count <= flag_maximum)
				party->flagged = true;
			function_91fc1(&pool, party);
		}
	}

	s_balance_problem problem;
	bool result = false;
	if (balance_teams_greedy(&problem, team_count, &pool, maximum_team_size))
	{
		long optimizer_storage[sizeof(s_balance_optimizer) / sizeof(long)];
		s_balance_optimizer &optimizer = *(s_balance_optimizer *)optimizer_storage;
		new (optimizer_storage) s_balance_optimizer;
		s_balance_solution const *solution = &optimizer.current;
		if (!optimize || !optimizer.optimize(&problem.solution))
			solution = &problem.solution;
		for (long team = 0; team < solution->team_count; team++)
		{
			s_balance_party const *members = &solution->teams[team].players;
			for (long j = 0; members->slot_in_use(j); j++)
				team_indices[members->players[j].index] = team;
		}
		result = true;
	}
	return result;
}


/* balances the players without their weights and returns the difference
   between the largest and the smallest team */
// @retail 0x91e30
bool balance_teams_by_count(long player_count, long const *party_indices, long team_count, long maximum_team_size,
	bool flag_parties, long flag_minimum, long flag_maximum, bool use_parties, long *imbalance)
{
	long weights[MAXIMUM_PLAYERS];
	long team_indices[MAXIMUM_PLAYERS];
	memset(weights, 0, sizeof(weights));
	bool result = balance_teams(team_count, maximum_team_size, flag_parties, flag_minimum, flag_maximum,
		use_parties, player_count, party_indices, weights, false, team_indices);
	if (result)
	{
		long team_sizes[MAXIMUM_PLAYERS];
		memset(team_sizes, 0, sizeof(team_sizes));
		for (long i = 0; i < player_count; i++)
			team_sizes[team_indices[i]]++;
		long smallest = team_sizes[0];
		long largest = team_sizes[0];
		for (long team = 1; team < team_count; team++)
		{
			smallest = smallest > team_sizes[team] ? team_sizes[team] : smallest;
			largest = largest <= team_sizes[team] ? team_sizes[team] : largest;
		}
		*imbalance = largest - smallest;
	}
	return result;
}

// MiniMax A (11-12): no new form, no lower score (34): the player count still arrives in edx where retail only caller 0x7e210 (src/unknown_07a9a0.cpp) loads it into eax, and the copy loop, the rep stosd fill, the call setup and the final clamp all follow from that one exchange, so the body stands. BLOCKED-BY 0x7e210.
/* whether the players and the extra players joining them can be balanced
   into the teams within the given imbalance; each extra player is a party
   of its own */
// MiniMax A (11-14): no new form, no lower score (34): the player count is tested in edx here and in eax in retail, and
// the copy, the fill, the call setup and the clamp all follow from that one register; retail's caller 0x7e210
// (src/unknown_07a9a0.cpp) hands over the player count in eax, the extra count in edx and the parties pointer in
// ecx: BLOCKED-BY the caller.
// MiniMax H (11-14): no new form, no lower score (34): writing the total as extra_count + player_count compiles to the same bytes, so the count's register is not the body's to choose; retail's caller 0x7e210 (another file, itself at 766) hands the player count over in EAX.
// MiniMax B (11-13): no new form, no lower score (34): the player count is tested in EDX here and in EAX in retail, and the copy, the fill, the call setup and the clamp all follow from that one register, which the caller's setup in another file decides.
// MiniMax H (11-12): no new form, no lower score (34); whole-program optimisation passes the player count in EDX
// where retail's only caller (0x7e210, src/unknown_07a9a0.cpp) loads it into EAX, and the copy loop, the rep stosd
// fill, the call setup and the final clamp all follow from that one exchange, so the body stands.
// BLOCKED-BY 0x7e210 (another file).
// MiniMax G (11-11): no new form, no lower score (34); the player count still arrives in EDX where retail's only
// caller 0x7e210 (src/unknown_07a9a0.cpp, out of this lane) loads it into EAX, and the copy loop, the `rep stosd`
// fill, the call setup and the final clamp all follow from that one exchange; this round re-read the diff and the
// count's register is decided by the caller and the body, not by the parameter order or the declarations, so the
// body stands.
// MiniMax R (11-11): no new form, no lower score (34); the player count still arrives in EDX where retail's
// only caller 0x7e210 (src/unknown_07a9a0.cpp, out of this lane) loads it into EAX, and the copy loop, the
// `rep stosd` fill, the call setup and the final clamp all follow from that one exchange; the body, not the
// parameter order or the declarations, decides which register carries the count.
// MiniMax S (11-10): no new form, no lower score (34); the `(short)` fill still moves the player count
// into EAX (7) and with the extra count read first in the sum it still measures 7, so the operand
// order of the sum is not what decides the base register: what is left there is the lea's operand
// order and the narrowing load (191 bytes against retail's 190). Retail's own fill is a dword load
// (`mov eax, dword ptr [esp+0xc]`), so the narrowing form is left out. The permuter run on this
// function was killed by its own time limit and produced no result.
// MiniMax h (11-05): the count roles stay as they are in a local player count used by the sum and
// the copy loop (34), in the sum as `extra + player` (34), as `total = extra; total += player` (34)
// and through a local extra or players (34); only the value-changing `(short)` fill moves the player
// count to EAX (7, and 5 with the clamp's subtraction written in place), which the note below keeps out.
// Retail's sum reads the extra count first and the player count second, and the operand order of the
// sum follows the order the two counts are declared in, so retail's declaration has the extra count
// first: swapping the two declarations here gives 35 and would need the call site in
// src/unknown_07a9a0.cpp to pass (count, groups, player count) instead of its current order.
// MiniMax G (11-06): no new form, no lower score: the register that carries the player count into this
// function is chosen at the call site, and retail's caller 0x7e210 (src/unknown_07a9a0.cpp, out of this
// lane) puts the player count in EAX while our build puts it in EDX; the copy loop, the `rep stosd` fill,
// the argument rotation and the final clamp all follow from that one difference.
// MiniMax H (11-13): no new form, no lower score (34); the player count is tested in EDX here and in EAX in retail, and the copy, the fill, the call setup and the clamp all follow from that one register, so the body stands.
// score 34 (was 34): frame 0x48 agrees; from +0x14 the player-count EAX/EDX roles are exchanged
// (retail player count in EAX, extra count in EDX) and flow through the copy, fill, call setup and
// the final clamp's register: our loop temporaries take EDI/EAX (retail EDX/EDI), leaving EDX for the
// player count. The body, not the parameter order or the caller, decides them.
// Round 22: retail's only caller 0x7e210 (src/unknown_07a9a0.cpp) loads the player count
// into EAX and the loop count into EDX before the call; the call to 0x91e30 then keeps its
// arguments in ECX/EDX/EAX (retail) where ours keeps them in EDX/EAX/ECX, the same rotation
// as the two counts, so the exchange reaches the call setup from that out-of-lane caller.
// Tried: result first 34; guarded countdown 45; source/output cursors 67; count parameter reference
// 54; shared existing count 34; staged total 34; capacity lifetime first 34; independent loop
// indices 34; named candidate party 34; descriptive scalar homes 34; independent indices with
// candidate 34; earlier imbalance output 65; standard party maximum in either order 100; standard
// excess maximum in either order 61; both maxima 118; guarded standard fill 55; maxima with fill 119;
// total after copy 74; phase-accumulated total 74; shared party value 73; strict maximum update 35;
// early imbalance 66; explicit excess clamp 46; result from combined tests 67; separate noinline
// declaration 34; with the caller updated, orders (extra, parties, player) 35, (player, extra,
// parties) 34, (parties, player, extra) 34, (parties, extra, player) 35, (extra, player,
// parties) 35; extra-first sum 35; copy before the maximum update 82; parties read back for
// the maximum 75; fill as `while (i < total) parties[i++] = next;` 34; clamp as
// `excess >= 0 ? excess : 0` 34; array and index declared first 34.
// Round 130 (m3 lane 8, fast): the player-count parameter itself as the fill index 34; the maximum
// update as a ternary 34; no total local (sums at each use) 74; open_slots computed after the call
// 87 (170 bytes); 1-7 unused probe locals 34 each
// (diagnostic: the player-count register does not follow the symbol count).
// Round 116: open slots from the sum before the total 71; `party_indices[i] + 1 >= next_party` 35;
// open slots written at their use 87 (170 bytes); next_party declared first 34; capacity as
// `maximum_team_size * team_count` 34. In the all-stack first pass extra count is always loaded
// into EAX (player count into EBX or EDX), which LTCG keeps; retail's roles need player count there.
// parties) 34, (parties, player, extra) 34, (parties, extra, player) 35, (extra, player, parties) 35;
// extra-first sum 35 (34 in round 82); copy before the maximum update 82; parties read back for the
// maximum 75; fill as `while (i < total) parties[i++] = next;` 34; clamp as `excess >= 0 ? excess : 0`
// 34; array and index declared first 34. Round 94: copy then max from the copy 75; fill counted by
// extra count 62 (moves extra count to the stack); clamp as `excess > 0 ? excess : 0` 43; open slots
// from the sum written out 71; `party >= next_party` 81. Screened without a role change: all 60
// declaration orders, local and parameter names, another caller, inline fill/copy helpers, clamp
// spellings. Round 82: total as `extra_count` then `+= player_count` 34; next party declared first
// 34; fill counted down from the top 72 (193 bytes); maximum as a ternary
// (`next > p + 1 ? next : p + 1`) 34, with the operands reversed 35.
// declaration 34; with the caller updated, parameter orders (extra, parties, player) 35, (player,
// extra, parties) 34, (parties, player, extra) 34, (parties, extra, player) 35, (extra, player,
// parties) 35; extra-first sum 35. Round 94: copy then max from the copy 75; fill counted by extra
// count 62 (moves extra count to the stack); clamp as `excess > 0 ? excess : 0` 43; open slots from
// the sum written out 71; `party >= next_party` 81; copy before the maximum update 82; parties read
// back for the maximum 75; fill as `while (i < total) parties[i++] = next;` 34; clamp as
// `excess >= 0 ? excess : 0` 34; array and index declared first 34. Screened without a role change:
// all 60 declaration orders, local and parameter names, another caller, inline fill/copy helpers,
// clamp spellings.
// Round 9: early `return false` when the balance fails 63; `return result && clamp <= maximum` 63;
// fill bound written `player_count + extra_count` 34; open slots computed at the clamp 87; the copy
// as a `while` loop 34 (the listed copy-before-maximum form reran at 82).
// declaration 34; parameter orders (extra, parties, player) 35, (player, extra, parties) 34,
// (parties, player, extra) 34, (parties, extra, player) 35, (extra, player, parties) 35;
// extra-first sum 35; copy before the maximum update 82; parties read back for the maximum 75.
// Round 94: copy then max from the copy 75; fill counted by extra count 62 (moves extra count to
// the stack); clamp as `excess > 0 ? excess : 0` 43, as `excess >= 0 ? excess : 0` 34; open slots
// from the sum written out 71; `party >= next_party` 81; fill as `while (i < total) parties[i++] =
// next;` 34; array and index declared first 34. Screened without a role change: all 60
// spellings. Round 13 (lane 13): retail's clamp tail (`and eax, ecx`: the excess register is the
// destination) comes from the subtraction written in the clamp, e.g. `MAX(0, imbalance -
// open_slots)` (compile-only); in the build it loads the maximum into ECX early and keeps EDX for
// the mask: `MAX(0, ...) <= maximum` 45 (192 bytes), the same through a local 45, as `maximum >=
// MAX(...)` 45, as an `if` clearing the result 45, the `< 0 ? 0 :` form written out 45.
// Separate maximum reduction and guarded memcpy, copy before/after scan, indexed/advancing
// source and if/conditional maximum 93/93/83/83/79/79/88/88. Count register exchange and clamp
// homes remain; original body restored.
// Round 14 (lane 2; fast with 0x91e30): imbalance declared with the other locals at the top and set
// to NONE before the call 34; declared there with its NONE initializer 65 (190 bytes); open slots
// computed just before the call 87 (170 bytes), with imbalance at the top too 87; next_party declared
// after the array and index 34.
// Round 38 (m3 lane 10, fast): the player count arrives in EDX in every form, whatever its parameter
// position; an all-stack compile-only build loads extra count into EAX and player count into EBX.
// Trials: parameters (extra, parties, player) with the caller updated 35 (player count still EDX); the
// fill continuing the copy loop's index 70 (now: player count in ECX, party indices on the stack,
// 0x7e210 781), with that parameter order 70; `result = false;` with the call as the test 82 (204
// bytes); `player_count > i` 34; open slots computed after next_party 34; pre-increment in both
// loops 34.
// Round 55 (lane 12; the one-file LTCG harness of this file reproduces the build exactly): total and open slots
// computed after the copy loop 96 (162 bytes), with the sum written extra first 96; total after the copy loop and
// open slots just before the call 87 (172 bytes); open slots after the fill 87 (170 bytes); `open_slots =
// team_count * maximum_team_size;` then `open_slots -= total;` 34 (same code). In the compile-only all-stack
// first pass player count always takes a callee-saved register (EBX, or EDX/EBP when the sums move) and extra
// count EAX or ECX, never EDX, so LTCG gives player count the leftover EDX.
// Round 135 (lane 4, fast with 0x91e30): `bool result = false; if (balance_teams_by_count(...)) {...}` 82
// (204 bytes); fill bound written `player_count + extra_count` 34 (same code); the party index read once into a
// named local 73; fill as `parties[player_count + i] = next_party` over extra_count 62; no total local (the sum
// written at the open slots, the fill and the call) 74.
// Round 23 (m3 lane 2, fast): the function moved ahead of 0x91e30 34; the maximum updated through a
// forceinline `long *` helper 34 (the address is promoted); `unsigned long` total 35 (`jae`); the fill value
// copied into its own local 34 (propagated); the fill as a pointer walk 63 (199 bytes, no rep stosd);
// `(dword)` on the fill value 34. A permuter run (42 tries) reached 8 only with a value-changing
// `(short)next_party` fill and swapped call arguments. Diagnostic, not keepable: `(short)next_party` in the
// fill alone gives 7 with retail's roles (player count EAX, extra count EDX, every loop register as
// retail); adding the clamp written as `(imbalance - open_slots < 0 ? 0 : imbalance - open_slots)` gives 5
// (left: `lea ebx, [eax + edx]` against `[edx + eax]`, the movsx and its byte). So the roles follow whether
// the rep stosd value is the maximum variable itself (ours) or a separate value loaded at the fill (retail).
// Round 32 (m3 lane 14, fast): the clamp as `if (excess < 0) excess = 0;` 46; `0 > excess ? 0 : excess` 34;
// the next party value named in the copy loop 34; the party value named, stored first, then the maximum
// 75; pre-increment in the fill 34. Compile-only with two dummy leading parameters (all-stack first
// pass): the total first/extra-first/`+=`/open slots first/next party first all load the player count
// into EBX.
// Round 125 (lane 6, fast with 0x91e30): open slots computed first from the sum written out, total after
// (real build) 71, with the sum extra first 71 (player count still EDX; next party then in EAX with a
// cmovle); total and open slots `const` 34; the locals as `int` 34; the excess as `imbalance -=
// open_slots` in place 34 (all three identical code).
// Round 25 (m3 lane 2, fast): extra-first sum with the clamp's subtraction written in place 45 (192 bytes); a
// `short` next_party local 76 (197 bytes); open slots as `team_count * maximum_team_size - player_count -
// extra_count` 71; the last party index tracked from NONE and the fill writing `last_party + 1` 82 (173 bytes);
// the clamp compare as `maximum_imbalance >= (excess < 0 ? 0 : excess)` 35.
// Round 61 (m3 lane 9, fast; the caller's file was on another lane's list, so no parameter orders): the
// clamp as `excess = imbalance > open_slots ? imbalance - open_slots : 0;` then `result = excess <=
// maximum_imbalance;` 65 (205 bytes); the copy bound `i != player_count` 35; open slots as `team_count *
// maximum_team_size - player_count - extra_count` 71; `long i = 0;` with `for (; i < player_count; i++)` 34
// (same code); next_party first, then the total, then `open_slots = team_count * maximum_team_size;
// open_slots -= total;` 34 (same code).
// Round 34 (m3 lane 14, fast): 1-8 initialized unused locals at the top and 1/2/3/5 before the imbalance 34 each
// (same code: the count register exchange is no numbering tie); the fill through a walking `long *slot` 63 (199
// bytes); the clamp named `clamped` 34; the call given `player_count + extra_count` 34; the imbalance declared
// bare and set to NONE before the call 34; the party index read once into `party` 73 (frame 0x40, cmovle).
// Lane 12 r66 (fast, with 0x91e30 matched): the result as `excess < 0 || excess <= maximum_imbalance` 48
// (193 bytes); the maximum as one ternary assignment per party 34 (same code); the fill as
// `while (extra_count-- > 0) parties[i++] = next_party;` 70 (the party list moves to the stack); the
// excess clamped in place in the imbalance output 46; `maximum_imbalance >= (excess < 0 ? 0 : excess)`
// 35. Re-measured: copy before the maximum 82, open
// slots from the sum written out 71, fill counted by the extra count 62, next_party after the array 34.
// Round 114 (lane 8; fast, and a one-file LTCG harness of this file that reproduces 34): the fill as `while
// (player_count < total) parties[player_count++] = next_party;` 34 (same code); parameters (extra_count,
// party_indices, player_count) with the call updated 35, with the clamp's subtraction in place 46, with the
// fill advancing player_count / next_party first / extra-first sum / an independent fill index / the maximum
// as a ternary 35 each, the clamp as `if (excess < 0) excess = 0;` 47, the fill over extra_count 62; fill
// values `next_party + 0`, `| 0`, `-(-next_party)`, `*&next_party`, through a `long const *`, through a
// const-reference fill helper, an `int` or `dword` next_party, a one-field struct: 34 each (same code).
// Diagnostics: `(short)` on the fill value with the clamp's subtraction in place 5, and with extra_count
// declared before player_count (any of three orders) 4, only the `movsx` left: the `lea ebx, [edx + eax]`
// operand order follows the parameter declaration order, not the sum's spelling, and the fill value has to
// be evaluated after the count and destination, which here only a non-leaf value does (a load through a
// register such as `party_indices[0]` is placed last; every local, global or folded form loads first).
// Round 21 (lane 17, fast): `(void)&next_party;` 34 (same code); next_party `volatile` (diagnostic, reverted) 59 with
// the player count still in EDX; total written `extra_count + player_count` with the clamp's subtraction in place
// 45 (192 bytes); a dead `next_party = NONE;` after the fill (diagnostic) 34; the party index named once in the
// copy loop with the array declared first 73; the fill counted down from `total - 1` 72 (193 bytes); open slots
// as the capacity then `-= total` 34; the fill bound `i != total` 35; 1-7 dead temporaries (diagnostic) 34 each.
// Round 276: standard count-based fill with difference/extra count, alone 62/60,
// with excess maximum 82/82, party maximum 118/115, both maxima 116/113.
// Combined scan, original fill and clamp restored; EAX/EDX count ownership remains.
// Round 127 (lane 5, fast with 0x91e30): the copy stored before the maximum update 82 (170 bytes);
// next_party set to 0 just before the copy loop 34; the fill through a block-scoped element pointer 34;
// the clamp as `if (excess < 0) excess = 0;` 46; the fill through a pointer to next_party 34
// (propagated); diagnostics: next_party as a memory-resident one-element array 34 (same code, so
// residency is not what flips the roles), a volatile read at the fill 58 (no rep stosd; reverted).
// Round 48 (m3 lane 10, fast): retail's only caller passes the player count in EAX, the party list in ECX
// and the extra count in EDX. `int next_party` 34; one variable serving as next party and then as the
// call's imbalance output (address-taken, sharing retail's frame-0 slot) 36 (the fill value is still
// loaded first); the extra-first sum with the clamp's subtraction written in place 45 (192 bytes); the fill
// bound `i != total` 35; the maximum test as `party_indices[i] + 1 >= next_party` 35. Re-measured
// (diagnostic, value-changing): `(short)next_party` in the fill 7 with retail's roles.
// Round 279: standard maximum element selection, reduction/copy first, maximum/reverse-minimum
// comparator, if/conditional lower bound: 72/80/84/94/78/87/84/90.
// Original combined scan, fill and clamp restored; count ownership remains.
// Round 21 (lane 17, fast): `(void)&next_party;` 34 (same code); next_party `volatile` (diagnostic, reverted) 59 with
// the player count still in EDX; total written `extra_count + player_count` with the clamp's subtraction in place
// 45 (192 bytes); a dead `next_party = NONE;` after the fill (diagnostic) 34; the party index named once in the
// copy loop with the array declared first 73; the fill counted down from `total - 1` 72 (193 bytes); open slots
// as the capacity then `-= total` 34; the fill bound `i != total` 35; 1-7 dead temporaries (diagnostic) 34 each.
// M292: Raw maximum-party index with strict/named-fill/conditional/inclusive producers,
// original/expression-owned clamp:
// 82/85/82/82/82/85/82/82. Original executable body restored.
// M296: Sixteen-slot shared short/word/byte/char ordinals, then separate short/long,
// long/short, short/short and word/word [79, 79, 78, 78, 79, 76, 79, 79]. Original long induction restored.
// Round 56 (m3 lane 10; one-file LTCG harness, exact here; the first form confirmed --fast): `int next_party` 34; `int
// parties[]` 34; `dword parties[]` 34; `(long)next_party` in the fill 34; next_party declared after the index 34; a
// forceinline fill helper (first, last, value) 34, plain inline 34, (pointer, count, value) 56; a static next_party 60
// (diagnostic); a one-element array 34; next_party read through its own address in the fill 34. Diagnostics
// (value-changing): the fill value `(short)` or `(unsigned short)` 4 (retail roles; left the movsx, `lea ebx, [edx +
// eax]` and the clamp register), `next_party + 1` 38 (retail roles), `(char)` 63, `-next_party` 48, `^ 1` 48, constant
// 5 31, no maximum update 66 (retail roles): the player count gets EAX only when the rep stosd value is a temporary
// computed after the count and destination; a plain variable (register, memory, static, element, inline parameter)
// keeps it in EDX. The all-stack compile-only pass shows the player count in EBX for the plain and the (short) forms
// alike, so it is no proxy for these roles.
// Round 289: One ordered union-domain copy/fill traversal, preserving signed iteration ranges,
// with conditional/logical bounds, maximum comparison directions and named/expression excess:
// 87/73/87/73/87/72/87/72. Original separate loops, full-width values and body restored.
// F321: complementary failure result in bool/byte/word/long storage, with
// conditional-value and branch-owned clamp consumers: 63/63/73/68/90/90/102/96.
// Original combined scan/fill and success-result graph restored; count ownership remains.
// Round 75 (m3 lane 9; one-file LTCG harness, control 34, positional counts): new fill values: pointer cast `(long)(char *)next_party` 34, `(long)(__int64)next_party`
// 34, union member 34, read through its own address 34, 32-bit bitfield 34, one-field struct elements copied whole 34, next_party and the imbalance as a two-element array or two-field
// struct 43/43 (aligned 37), `next_party > 0 ? next_party : 0` 65, a counted walking pointer 43, a copy declared before the loop 34, a copy per iteration 34; loop shapes: braced body 34, while 34,
// guarded do-while (two spellings) 64 (no rep stos), walking pointer to an end 54, `slot != end` 59; next_party declared after the index 34. Parameters on the register three, with the
// entry updated: (extra, list, player) 35, (player, extra, list) 34, (extra, player, list) 35, (list, player, extra) 34, (list, extra, player) 35. The player count stays EDX in all of them.
// Retail's caller loads EAX, ECX, EDX in that order; the 35s are `lea ebx, [edx + eax]` with no role change.
// Round 38 (lane 17, fast with 0x91e30): local names as a tie-break for the roles (same-size slots are
// ordered by name): extra_count renamed `requested_count` (sorting after player_count) 34; next_party
// `unused_party` 34; parties `party_list` 34; total `combined_count` 34; i `index` 34 (same code each).
// Round 144 (m3 lane 6, fast with 0x91e30): the fill through `long *extra_parties = &parties[player_count]` over
// extra_count 67 (191 bytes); the clamp as an if/else of two results (`0 <= maximum` / `excess <= maximum`) 55 (199
// bytes); `open_slots >= imbalance ? 0 : imbalance - open_slots` 65 (205 bytes); the imbalance lifted to the open
// slots (`if (imbalance < open_slots) imbalance = open_slots;`) then `imbalance - open_slots <= maximum` 45;
// re-measured: total and open slots after the copy loop 96 (162 bytes).
// Round 200 (m3 lane 3, fast): clamp `(excess > 0 ? excess : 0)` 43, `(excess >= 0 ? excess : 0)` 34,
// `maximum_imbalance >= (...)` 35; `extra_count + player_count` alone 34; per-loop index declarations
// (`long i` / `long j`) 34. All restored.
// M354: Standard forward/backward input copy before/after maximum reduction,
// branch/conditional maxima 76/76/77/77/76/76/77/77.
// NOT kept: split library-copy ownership misses the native combined scan/fill allocation.
// M356: Reverse combined copy/maximum pass, indexed/cursor, direct/reused sample,
// branch/conditional maximum: 41/41/73/73/64/64/64/64. NOT kept: all regress;
// sampled/cursor forms also raise the caller. Original forward combined pass restored.
// M358: Inclusive maximum-update admission, positive/complementary bool/byte/short/long:
// 34 each. NOT kept: one-use predicates do not match. Original combined scan/fill restored;
// player-count EAX/EDX, late fill-value load and clamp homes remain.
// M364: Combined copy/maximum selection by input/output element pointer or slot ordinal,
// branch/conditional producers: 116/116/96/96/120/120/98/98. NOT kept: no exact native
// fill/count ownership; original scalar maximum, loops and complete body restored.
// Y581: Standard trivial construction fill, difference/extra count and total/extra
// endpoints, original excess 67/66/66/66; expression excess 70/69/69/69.
// NOT kept: all regress and grow to 196/198 versus native 190 bytes; original
// combined scan, assignment fill, clamp, includes and full body restored.
// B447: Complete copy/maximum/fill boundary, pointer/actual-array reference,
// forced inline consuming total 34/34, returning computed total 71/71; ordinary
// inline 144/144/143/143. NOT kept: no exact match; ordinary providers stay called.
// Original complete body restored; frame/count/fill-value and clamp roles remain.
// Round 91 (m3 lane 9, fast with 0x91e30): open slots from `team_count * maximum_team_size - (player_count +
// extra_count)` with the total declared after 71; that with the fill bound and the call written `player_count +
// extra_count` (no total local) 74; the copy through a `long const *` to the element 34 (same code); the tail as
// `imbalance -= open_slots; if (imbalance < 0) imbalance = 0;` 46; the fill under `if (extra_count > 0)` 74 (198
// bytes); a `long const` of the index plus one named for the maximum and the copy 87 (169 bytes).
// Round 75 (m3 lane 11, fast with 0x91e30): the subtraction in place, `imbalance -= open_slots;` with the clamp
// on `imbalance`, 34; parameters (extra_count, party_indices, player_count) as the caller's setup order shows,
// definition, declaration and the call in 0x7e210 changed together, on that 35 (the `lea` operands then as retail,
// player count still EDX; 0x7e210 stays 766); diagnostics on that: `(short)` on the fill value 6 (191 bytes, roles
// as retail), then the clamp as an excess local 6, `if (imbalance < 0) imbalance = 0;` 19, `imbalance >= 0 ?
// imbalance : 0` 6, the clamp assigned back, `imbalance = imbalance < 0 ? 0 : imbalance;` then the compare, 4 (only
// the movsx: the `and eax, ecx` order comes from assigning the clamp to the value's own variable); the same without
// the cast 46 (192 bytes); that clamp assignment alone with the original order 45 (192 bytes); an early `return
// false` for a failed count balance 63 (203 bytes). The real build agrees with round 114's harness: the open
// question is still a natural fill value that is evaluated after the count and destination.
// P382: Positive terminal balance result in char/signed-char/byte/short/word/int/long/
// dword: 47/47/47/64/64/61/61/61. NOT kept: all lengthen to 195-205 versus 190,
// count-register exchange and first +0x14 remain; original Boolean result and full body restored.
// MiniMax H (11-11): no new form, no lower score (34); the player count is tested in EDX here and in EAX in retail, and the register it arrives in is fixed by the caller's parameter order: retail's caller 0x7e210 hands over the player count in EAX, the extra count in EDX and the groups pointer in ECX, so retail's parameter order is (player count, extra count, parties pointer) and the call site is in another file.
// MiniMax E (11-08): no new form, no lower score: the player count is tested in edx here and in eax in retail, and which register holds it is fixed by the caller's parameter order.
// P391: Limit-based acceptance (nonnegative maximum and excess within it), both test
// priorities with bool/byte/short/long results: 53/69/82/78/53/69/82/78. NOT kept:
// all lengthen to 197-217/190; original clamp, Boolean result and full body restored.
// Round 139 (m3 lane 7, fast with 0x91e30): the fill loop written after `long imbalance = NONE;` 37 (the NONE store moves
// ahead of the fill's test; the fill value is still loaded first); `register long next_party` 34 (same code); `long const`
// player and extra count parameters 34 (same code); the excess as `long excess = imbalance; excess -= open_slots;` 34 (same
// code); the fill counted down over the extra count, `parties[total - i]`, 72. Re-measured: the maximum as one ternary
// assignment 34, the fill through a walking pointer to an end 63 (199 bytes), the clamp assigned back to the imbalance 45
// (192 bytes). The player count stays EDX in each.
// P400: Repeated live input-cell reference/pointer with inclusive/strict branch and
// inclusive/complementary selection maxima: 34/35/34/34/34/35/34/34. NOT kept:
// no exact count/fill/clamp allocation repair; native 0x48 frame and 190-byte extent
// remain. Original complete body, direct input reads and scalar maximum restored.
// M3 lane 5 r167 (fast with 0x91e30; base 34, aligned 23, masked 2: only register roles are left): the copy as a
// `while` loop with the maximum as one ternary 34; the extra-first sum with the clamp as `excess >= 0 ? excess : 0` 34;
// per-loop `long i` declarations with the extra-first sum 34; the maximum as one ternary with the fill bound written
// `player_count + extra_count` 34; `long const` total and open slots with the copy as a `while` loop 34. Re-measured:
// the clamp as `if (excess < 0) excess = 0;` 46; next_party first with open slots as the capacity then `-= total` 34;
// an early `return false` (here with the clamp assigned back to the imbalance) 63 (203 bytes). All restored.
// m3 lane 3 r227: both builds keep next_party at frame+0; the only difference is the entry convention (player
// count EAX in retail, EDX here), which follows the first pass's order for the rep stosd operands: ours loads the fill value
// before the count and destination (`mov eax, [esp + 0xc]` first), retail after them, so retail's player count can live in EAX
// until `sub ecx, eax`. Every natural fill/maximum/clamp form is listed above at 34 or worse; only value-changing fill values
// move the load, and the caller 0x7e210 (src/unknown_07a9a0.cpp) is outside this round's files. New (fast): next_party scoped to a
// block holding both loops 34; next_party and the index in that block 34 (same code). No other untried natural form is left.
// Round 149 (m3 lane 8, fast with 0x91e30/0x7e210): the copy loop forms `party + 1` once per pass (`lea edi, [edx + 1]`)
// in retail as here, so a running largest index filled with `largest + 1` is not retail's. Combinations, all 34 with the
// player count still in EDX: the extra-first sum with next_party declared first, with the fill as `while (i < total)
// parties[i++] = next_party;`, with the subtraction in place (`imbalance -= open_slots;`); next_party declared first with
// the subtraction in place; the `while` fill with the subtraction in place.
// Round 76 (m3 lane 14, fast): the fill counted down over the extra count 70 (diagnostic: the extra count then goes
// to the stack); the clamp assigned back to the imbalance with an extra-first sum 45 (192 bytes); next_party as a
// dword 35 (same roles); the copy bound as `player_count > i` 34 (same code); open slots computed just before the call
// 87 (170 bytes). A /Gd compile-only harness of this body (all-stack first pass) loads the extra count into EAX in
// all 13 spellings screened (sums, total/open-slot placement, fill forms, signature orders), but it does not show the
// role change the `(short)` fill gives in the real build, so it cannot screen this cause. The countdown fill, the clamp
// assigned back, the dword next_party and the open slots before the call re-measure listed forms; new: the copy bound
// form above and one loop over the total that copies below the player count and fills above it 83 (174 bytes). No
// untried natural form left in this note's lists.
// Round 105 (m3 lane 16, compile-only `/O2 /Ob1 /arch:SSE /Gd` first pass, the all-stack pass LTCG takes register
// arguments from): next_party takes EAX there because the `rep stosd` fill wants its value in EAX, so the player count
// goes to callee-saved EBX and LTCG then gives it the leftover EDX, with the extra count (loaded into EAX first)
// keeping EAX. A `while` copy loop, the imbalance declared first and an early `return false` keep that (EBX); a
// named party value moves it to EDX and the fill counted over the extra count to EBP (both listed, 73 and 62). Retail
// needs the player count loaded into EAX in that pass, which only the value-changing `(short)` fill gives.
// Round 184 (m3 lane 4, fast): the copy through an output cursor (`*party++ = party_indices[i]`) 78 (175 bytes);
// through an input cursor alone 77 (172 bytes); that cursor with the fill through a walking pointer 64; the output
// cursor continued by the fill over the extra count 86 (173 bytes); the total as `player_count` then `+= extra_count`
// 34 (same code); re-measured: the fill through a walking pointer 63 (199 bytes). No untried natural form is left in
// this note's lists; the player count still arrives in EDX.
// M3 lane 16 r108 (fast with 0x91e30; register-masked aligned 4: the copy loop's store/counter-reload order and the
// fill value's load, both from the player count's EDX/EAX role): the party array declared first 34 (same code; the
// listed "array and index declared first"). Every other candidate checked against this note was already listed; no
// untried natural form found.
// Round 154 (m3 lane 8): round 114's one-file LTCG harness refreshed from the current file reproduces 34 (190/190
// bytes). Grepped this note for the remaining spellings (strict and inclusive maximum updates, the copy through
// memcpy, fill helpers, the fill continuing the copy loop, declaration orders): all are listed, and no existing
// dword-fill helper exists to call.
// No untried natural form left; the cause stays the fill value loaded before the count and destination.
// MiniMax A (11-13): no new form tried this round, no lower score (34): the player count is still tested in EDX
// here and in EAX in retail, and the copy, the fill, the call setup and the clamp all follow from that one
// register; retail's caller 0x7e210 hands over the player count in EAX, the extra count in EDX and the parties
// pointer in ECX, so the register that decides this is fixed by the caller's setup in src/unknown_07a9a0.cpp:
// BLOCKED-BY the caller.
// @retail 0x91f00
bool balance_teams_can_add(long player_count, long const *party_indices, long extra_count, long team_count,
	long maximum_team_size, bool flag_parties, long flag_minimum, long flag_maximum, bool use_parties,
	long maximum_imbalance)
{
	long total = player_count + extra_count;
	long open_slots = team_count * maximum_team_size - total;
	long next_party = 0;
	long parties[MAXIMUM_PLAYERS];
	long i;
	for (i = 0; i < player_count; i++)
	{
		if (next_party <= party_indices[i] + 1)
			next_party = party_indices[i] + 1;
		parties[i] = party_indices[i];
	}
	for (i = player_count; i < total; i++)
		parties[i] = next_party;
	long imbalance = NONE;
	bool result = balance_teams_by_count(total, parties, team_count, maximum_team_size, flag_parties,
		flag_minimum, flag_maximum, use_parties, &imbalance);
	if (result)
	{
		long excess = imbalance - open_slots;
		result = (excess < 0 ? 0 : excess) <= maximum_imbalance;
	}
	return result;
}
