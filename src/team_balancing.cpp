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
	long maximum = function_91861(this);
	long minimum = function_91862(this);
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

// score 67 (was 67): the count delta now matches retail (the delta named, then the two new team
// counts); what remains is the order of the spread queries: retail calls the candidate's
// count_spread first in both comparisons (and the reference's cost first), ours calls the
// reference's first whatever the operand order, which also adds a `mov ebx, esi` (514 bytes).
// Round 37 (on the named delta): every comparison with the reference on the left 65; only the
// equality/only the less-than so 66/66; cost operands swapped 67; candidate spread named after the
// selection 65; the selection as a reference 65; `bool better` chain 81; candidate spread named
// before the selection 74. Compile-only screens of ~60 one-tree delta spellings all reassociate
// (they do not show the named-delta effect, which only the LTCG build gives).
// Earlier (score 80/116): count homes, signed party delta and pure-query ownership remain.
// frame 0x2694 agrees; the full build now scores as the reduced image (80). Left:
// our build reassociates the new count difference into 2*(b-a) - B + A (retail keeps (A - 2*(a-b)) - B
// with `lea ecx, [eax+eax]`), so the inner party pointer takes EAX (retail ECX), the A/B count-address
// homes swap (+0x34/+0x2c against +0x2c/+0x34), and every later line is offset by one; retail also
// calls the candidate spread before the reference spread.
// Tried (reduced image unless noted): count fields A/B/both 80/80/80; early roots 173; players
// A/B/both 78/75/80; reverse count roots 173; B players root before A/first/between views 82/88/82;
// immutable B root 75 (B players plain 115 in the full build); team/party/both lexical priority
// 80/80/80; short outer/inner/both ordinals 142/142/155; scope and comparison names 80/80;
// candidate/reference/both spread values 135/127/137; spread delta 132; candidate A/B/both team roots
// 80/150/150; outer party cursor 91; first spread order 80; equal spread order 80; spread orders 80;
// cost order 78; first spread/cost order 78; equal spread/cost order 78; query orders 78; inner party
// cursor 132; cursor/spread orders 132; reused signed team delta 161; reused party delta 80; reused
// team and party deltas 161; A/B/both count references 80/80/80; early count references 173; inline
// count spread 225; inline total spread 80; inline spread queries 225. Full build (when it scored 116):
// first party pointer in the outer loop 116; nested ifs instead of continues 116; teams read in full
// at each use 146 (loses 0x91dd0); comparison operands swapped 116; explicit outer party pointer
// stepping 116. No-link screens: every delta spelling folds to the same 2*(b-a) sequence.
// Round 82 (full build 80): all three comparisons with the reference on the left (cost compare as
// retail) 78; that with the reference chosen by if/else 78; party pointer A in the outer loop 78;
// count-after-swap totals, a named party delta subtracted from both, `- 2 * delta` 78/78/78;
// `+ (b-a) * 2` 78; `b - a + (a-b) * 2` 78 (A/B count-address homes then agree; the reassociation
// stays); an inline absolute-difference helper for both differences 78.
// frame 0x2694 and entry agree; from +0x3e the A/B count-address homes are
// swapped, the inner party pointer takes EAX (retail ECX) so the count delta is built as 2*(b-a),
// and the two count_spread queries run reference first (retail candidate first). The full and the
// reduced build now agree at 80 (earlier rounds saw 116 in the full build).
// Tried: count fields A/B/both 80/80/80; early roots 173; players A/B/both 78/75/80; reverse
// count roots 173; B players root before A/first/between views 82/88/82; immutable B root 75
// (B players plain 115, an ineligible one-count gain); team/party/both lexical priority 80/80/80;
// short outer/inner/both ordinals 142/142/155; scope and comparison names 80/80;
// candidate/reference/both spread values 135/127/137; spread delta 132; candidate A/B/both team
// roots 80/150/150; outer party cursor 91; first/equal/both spread orders 80/80/80; cost order 78;
// first/equal spread with cost order 78/78; query orders 78; inner party cursor 132;
// cursor/spread orders 132; reused signed team delta 161; reused party delta 80; reused team and
// party deltas 161; A/B/both count references 80/80/80; early count references 173; inline count
// spread 225; inline total spread 80; inline spread queries 225.
// Full-build forms (when it scored 116): first party pointer in the outer loop, nested ifs
// instead of continues, comparison operands swapped (the call order follows the operands, not
// their position), explicit outer party pointer stepping 116 each; teams read in full at each use
// 146 (loses 0x91dd0). No-link screens: every delta spelling (named, doubled, parenthesised,
// count-after-swap, abs forms) folds to the same 2*(b-a) sequence; reference/local/method forms
// keep reference first.
// Round 9: party elements at each use, no party pointers 140; cost test written candidate first
// 78 (the cost order form again); delta grouped a - b - 2 * (pa - pb) 80; named a_count/b_count
// pointers 80 (the count references again); b declared before a 87; both absolute values as
// ternaries 80; the reference picked by `= &current; if (improved) = &best;` 80. Compile-only
// screen: the spread queries' order ignores the operands' order.
// Round 11: outer party pointer declared in the outer loop 80; the new team counts as two named
// locals 80; nested tests instead of the continues 80; candidate declared at function scope 184
// (558 bytes); the candidate's spread named before the comparison 80 (the reference's query still
// runs first: LTCG treats count_spread as pure and reorders the calls); permuter 58 tries, best 80.
// count homes, signed party delta and pure-query ownership remain.
// Earlier: count-address homes, party registers, delta and query order differ.
// Reduced control 80 (full 116); count fields A/B/both 80/80/80; early roots 173;
// players A/B/both 78/75/80; reverse count roots 173; B players root before A/first/
// between views 82/88/82; immutable B root 75. B players plain 115 is an ineligible
// one-count gain; all subobject forms restored.
// frame 0x2694 agrees; team-pointer homes, delta and query ownership differ.
// Reduced baseline 80 (full 116); team/party/both lexical priority 80/80/80;
// short outer/inner/both ordinals 142/142/155; accepted scope and comparison names 80/80.
// Earlier: count homes, signed delta and query ownership remain.
// Reduced candidate/reference/both spread values 135/127/137; spread delta 132;
// candidate A/B/both team roots 80/150/150; outer party cursor 91 (reduced baseline 80).
// Reduced trials (opening 80; full 116):
// first spread order 80; equal spread order 80; spread orders 80; cost order 78;
// first spread/cost order 78; equal spread/cost order 78; query orders 78; inner party cursor 132;
// cursor/spread orders 132; reused signed team delta 161; reused party delta 80;
// reused team and party deltas 161; A count reference 80; B count reference 80; count references 80;
// early count references 173; inline count spread 225; inline total spread 80;
// inline spread queries 225.
// the inner party pointer takes EAX (retail ECX), so the count delta is
// built as 2*(b-a) and the A/B count-address homes swap; the full build also calls each pair of
// queries in the other order (reference spread first, candidate cost first; retail the reverse).
// The reduced image scores 80 and orders the cost calls as retail: trust only the plain check.
// Plain-checked: first party pointer in the outer loop 116; nested ifs instead of continues 116;
// teams read in full at each use 146 (loses 0x91dd0); comparison operands swapped 116 (the call
// order follows the operands, not their position); explicit outer party pointer stepping 116.
// No-link screens: every delta spelling (named, doubled, parenthesised, count-after-swap, abs
// forms) folds to the same 2*(b-a) sequence; reference/local/method forms keep reference first.
// Earlier (reduced-image) forms: control 80; count fields A/B/both 80/80/80; early roots 173;
// players A/B/both 78/75/80; reverse count roots 173; B players root before A/first/between views
// 82/88/82; immutable B root 75 (B players plain 115, an ineligible one-count gain);
// team/party/both lexical priority 80/80/80; short outer/inner/both ordinals 142/142/155;
// scope and comparison names 80/80; candidate/reference/both spread values 135/127/137; spread
// delta 132; candidate A/B/both team roots 80/150/150; outer party cursor 91.
// Round 295: separate noinline declarations for count/total/rank query methods,
// all seven nonempty subsets, 80 each; rank-only inline declaration 80. Every
// declaration and body restored; count homes, inner party and query ownership remain.
// Round 47: with the reference on the left (65) only the four count_spread calls differ (aligned
// 8). Compile-only screens: a local candidate against a reference pointer calls the reference
// first in every operand order, as LTCG does; only the candidate's spread named first calls it
// first there (LTCG reorders that too, 74/80 above). Reference selected before the candidate copy
// 128 (moves try_swap's register parameters, 0x91dd0 then 2), with the reference on the left 128;
// selected at the top of the inner loop 121; `(improved ? best : current)` at each use 112.
// Round 230: solution selection at every query, alone, with A/B/both team observations,
// outer-party lifetime, both candidate-team roots, shared candidate spread and equality-first
// acceptance: 112/179/150/186/112/182/107/115. Original selection, named post-swap counts and body restored.
// Lane 14 r20: the first test as `!(candidate >= reference)` 67; the reference pointer declared
// `const` 67; the condition rewritten as `<= && (!= || cost)` 100 (512 bytes but reference spread
// still first and the frame 0x268c); the reference selected right after the candidate copy, before
// the swaps, 151 (521 bytes; 0x91dd0 9); the cost test written candidate first 67.
// Round 127 (lane 4): the reference selected right after the candidate copy 151 (0x91dd0 9; the
// reference is still queried first); the reference on the left of both comparisons 65 (reference
// still first); the comparison as an in-class solution method called on the candidate 188 (not
// inlined under /Ob1, 420 bytes); the improvement as an if / else-if pair 150 (0x91dd0 8); the
// reference pointer declared `const` 67 (same code).
// Round 46 (lane 9): the cost compare written candidate first (`candidate.cost() < reference->cost()`) 67, with
// both spread compares reference first 65 (the cost compare is then retail's; only the four count_spread calls
// differ: ours reference first in every operand order, retail candidate first), with only the first spread
// compare reference first 66; `candidate - reference spread < 0` for the first test 67; the selection written
// at each use as `(improved ? &best : &current)->` 112 (not CSE'd; still reference first);
// `!(candidate.count_spread() >= reference->count_spread())` 67; the selection written `!improved ? &current :
// &best` 70; the selection moved after the candidate copy (before the swaps) 151; `best = candidate;` before
// `improved = true;` 71. No other function calls count_spread, total_spread or rank_spread out of line.
// Round 14 (lane 2): aligned the only cause is the count_spread order (and the `mov ebx, esi` it
// adds); a compile-only screen reproduces it even with count_spread's body defined after this
// function, so it is not purity: VC queries the stack-local candidate (`lea ebx, [esp+0x38]`) after
// any pointer or member solution, in every operand order (probes against `best` and `current`
// directly do the same). Real builds, 67 each (same code): the comparison as a forceinline
// `better_than` method on the candidate, as a forceinline `worse_than` method on the reference, as a
// forceinline free function of both pointers, the selection as an if/else, the reference declared at
// the top of the inner loop and assigned after the swaps. Compile-only, same order: `this->` inside
// the method, the selection passed as the call's argument, `(&candidate)->`, a reference variable,
// the selection written at each use, `!(reference <= candidate)`.
// Lane 16 r46: the reference declared at the top 67; count_spread as a free function on a pointer 67; the
// test in a forceinline (candidate, reference) helper 67; renamed trial/baseline 67; `candidate(current)`
// 67. Compile-only (no /GL, extern methods): the call on the stack copy runs after the call through the
// pointer in every operator, operand order, helper, free/__stdcall and naming form; with two stack copies
// the right operand of `<`/`==` runs first. So retail's candidate-first order is not a reorder that only
// LTCG's readonly knowledge makes.
// Round 135 (lane 4): a compile-only screen shows the order of two count_spread calls ranks their `this`
// expressions, not the operands: `&best` (this + 0x2668) before the reference variable before `this` (current)
// before the stack candidate, in both operand orders; a pointer local to the candidate (declared before or after
// the reference), a storage-array candidate, a cast to const and forceinline helpers taking (reference,
// candidate) or (candidate, reference) still query the candidate last (its address is propagated first). Real
// builds: the reference pointer non-const 67; that with `candidate.cost() < reference->cost()` 67; with both
// spread tests reference-left 65; count_spread non-const 67; the selection as a reference variable 67.
// Round 33 (m3 lane 11), fast: the test as a forceinline solution method on the candidate
// (`candidate.better_than(reference)`) with its comparisons this-first 67, this-first with the cost
// written other-first 67, all other-first 65; the candidate declared in the outer loop and copied in
// the inner one 209 (570 bytes); the reference as `&(improved ? best : current)` 67 (same code).
// Round 38 (m3 lane 10): compile-only micro tests: VC calls a method on a stack local after the same
// call through a register pointer, a member, a global or a pointer parameter, whatever the operand
// order; between two stack locals the right operand runs first; named one-use results keep their
// statement order only while the callee's body is unseen (an extern declaration), not with it
// defined in the file, and not at LTCG with it defined anywhere. Real builds (fast): the three
// spread definitions moved below this function 67 (same code), with both spreads named 67;
// `continue` on `candidate > reference` and on equal spreads with `candidate cost >= reference
// cost` 100 (512 bytes; first difference +0x7), the same with the reference on the left 100; the
// candidate spread named and used in both tests (three queries) 80 (512 bytes).
// Round 55 (lane 12; a one-file LTCG harness of this file reproduces the build exactly): the test in a
// forceinline (candidate, reference) helper written `b > a || (b == a && cost)` 65, `a < b || (b == a && cost)`
// 66; the qualified call `candidate.s_balance_solution::count_spread()` 67; `a - b < 0 || (a - b == 0 &&
// cost)` 67; the test as two `continue`s 100 (512 bytes). Harness probes: the stack candidate's call runs
// last against any pointer, even `&best` or a parameter; against a second stack copy the older copy runs
// first in either operand order; nested `<=` then `<` 55 aligned, else-if pair 66 aligned (both worse).
// Round 27 (m3 lane 15, plain): pointers to the candidate's two teams for the swaps 150 (518 bytes,
// 0x91dd0 then 9); the first test as `!(reference->count_spread() <= candidate.count_spread())` 66; the
// condition named `bool better` 67; only the equality written reference first 66; the equality-and-cost
// clause tested before the `<` 107 (536 bytes). The candidate's count_spread still runs last.
// Round 32 (m3 lane 14, fast): not purity: count_spread made to look writing (a store through a copy
// of `this`, diagnostic, 0x91860 then 38) still queries the reference first, 97 (it takes EDX then);
// that with the reference on the left 97; the spreads passed as arguments of forceinline `less`/`equal`
// helpers (candidate first) 67 (same code); a `const &` alias of the candidate in the test 67; a const
// pointer to the candidate for all six queries 67.
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
				(candidate.count_spread() == reference->count_spread() && reference->cost() > candidate.cost()))
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

/* whether the players and the extra players joining them can be balanced
   into the teams within the given imbalance; each extra player is a party
   of its own */
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
// Round 32 (m3 lane 14, fast): the clamp as `if (excess < 0) excess = 0;` 46; `0 > excess ? 0 : excess` 34;
// the next party value named in the copy loop 34; the party value named, stored first, then the maximum
// 75; pre-increment in the fill 34. Compile-only with two dummy leading parameters (all-stack first
// pass): the total first/extra-first/`+=`/open slots first/next party first all load the player count
// into EBX.
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
