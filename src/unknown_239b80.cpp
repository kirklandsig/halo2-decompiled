// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_239B80.CPP: two-dimensional polygon helpers (convex hulls,
   clipping against lines and point-in-polygon tests over point2f
   arrays) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <float.h>
#include <string.h>

#define k_real_epsilon 0.0001f
#define MAXIMUM_POLYGON_POINTS 48
#define FLAG(bit) (1 << (bit))
#define TEST_FLAG(flags, bit) (((flags) & FLAG(bit)) != 0)
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

struct vector2f { real i, j; };

/* a line in the plane: points p with dot(n, p) == d */
struct plane2f
{
	vector2f n;
	real d;
};

PRIVATE __forceinline bool polygon_clip_intersection_23a430(plane2f const *plane,
	point2f const *first, point2f const *second, long count, short capacity, point2f *output)
{
	real fraction = 0.0f;
	real dx = (first->x - second->x);
	real dy = first->y - second->y;
	real denominator = plane->n.i * dx + plane->n.j * dy;
	if ((denominator < -0.0001f || denominator > 0.0001f) && count < capacity)
	{
		real dot = (first->y * plane->n.j) + (first->x * plane->n.i);
		fraction = (dot - plane->d) / denominator;
		output->x = first->x - dx * fraction;
		output->y = first->y - dy * fraction;
		return true;
	}
	return false;
}

/* clips the polygon against the line, keeping the points on the side the normal points to; returns the new count */
// @retail 0x23a430
short function_23a430(short count, point2f const *points, plane2f const *plane,
	real epsilon, short capacity, point2f *output)
{
	point2f copy[128];
	long sides[128];
	if (points == output)
	{
		memcpy(copy, points, count * sizeof(point2f));
		points = copy;
	}
	long original_count = count;
	long positive = 0;
	long negative = 0;
	for (long i = 0; i < original_count; i++)
	{
		real distance = points[i].y * plane->n.j + points[i].x * plane->n.i - plane->d;
		if (distance > epsilon)
		{
			sides[i] = 1;
			positive++;
		}
		else if (0.f - epsilon > distance)
		{
			sides[i] = 0;
			negative++;
		}
		else
			sides[i] = NONE;
	}
	long result = 0;
	if (!negative)
	{
		memcpy(output, points, original_count * sizeof(point2f));
		return (short)original_count;
	}
	if (!positive)
		return 0;
	long previous = original_count - 1;
	for (long current = 0; current < original_count; previous = current++)
	{
		long side = sides[current];
		if (side == 0)
		{
			if (sides[previous] == 1)
			{
				point2f const *previous_point = &points[previous];
				if (polygon_clip_intersection_23a430(plane, &points[current], previous_point, result, capacity, &output[result]))
					result++;
			}
		}
		else
		{
			point2f const *current_point = &points[current];
			if (side == 1 && sides[previous] == 0)
			{
				point2f const *previous_point = &points[previous];
				if (polygon_clip_intersection_23a430(plane, previous_point, current_point, result, capacity, &output[result]))
					result++;
			}
			if (result < capacity)
				output[result++] = *current_point;
		}
	}
	return (short)result;
}


static inline real normalize2d(vector2f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j);
	if (fabs(m) < k_real_epsilon)
		return 0.f;
	real inv = 1.f / m;
	v->i = inv * v->i;
	v->j = v->j * inv;
	return m;
}

/* whether the displacement from one point to the other is longer than the epsilon on either axis */
static inline bool points2d_differ(point2f const *from, point2f const *to)
{
	return !(fabs(to->x - from->x) < k_real_epsilon && fabs(to->y - from->y) < k_real_epsilon);
}

/* two pi, set up at run time */
real g_55e470;

long function_239d50(long count, point2f const *points, point2f *hull);
short function_239e50(short count, point2f const *points, short *indices);

/* 0 when every point is the same, 1 when they are on one line, 2 otherwise */
// @retail 0x239b80
short function_239b80(short count, point2f const *points)
{
	point2f first;
	plane2f line;
	vector2f *normal = &line.n;
	short state = NONE;

	for (short i = 0; state < 2 && i < count; i++)
	{
		switch (state)
		{
		case NONE:
			first = points[i];
			state = 0;
			break;
		case 0:
			{
				bool valid = false;
				line.n.i = first.y - points[i].y;
				normal->j = points[i].x - first.x;
				if (normalize2d(normal) != 0.f)
				{
					line.d = normal->i * points[i].x + normal->j * points[i].y;
					valid = true;
				}
				else
				{
					line.d = 0.f;
				}
				if (valid)
					state = 1;
			}
			break;
		case 1:
			if (!(fabs(normal->i * points[i].x + normal->j * points[i].y - line.d) < k_real_epsilon))
				state = 2;
			break;
		}
	}

	return state;
}

// @retail 0x239cb0
long function_239cb0(long count, point3f const *points, point2f *hull)
{
	point2f points2d[MAXIMUM_POLYGON_POINTS];

	if (count > MAXIMUM_POLYGON_POINTS)
		count = MAXIMUM_POLYGON_POINTS;
	for (long i = 0; i < count; i++)
	{
		points2d[i].x = points[i].x;
		points2d[i].y = points[i].y;
	}

	return function_239d50(count, points2d, hull);
}

// @retail 0x239d50
long function_239d50(long count, point2f const *points, point2f *hull)
{
	point2f unique_points[MAXIMUM_POLYGON_POINTS];
	short indices[MAXIMUM_POLYGON_POINTS];

	short unique_count = (short)count;
	memcpy(unique_points, points, sizeof(point2f) * (count > MAXIMUM_POLYGON_POINTS ? MAXIMUM_POLYGON_POINTS : count));
	for (long i = 0; i < unique_count - 1; i++)
	{
		for (long j = i + 1; j < unique_count; j++)
		{
			if (fabs(unique_points[i].x - unique_points[j].x) < k_real_epsilon &&
				fabs(unique_points[i].y - unique_points[j].y) < k_real_epsilon)
			{
				unique_points[j] = unique_points[unique_count - 1];
				unique_count--;
			}
		}
	}

	short hull_count = function_239e50(unique_count, unique_points, indices);
	if (hull_count >= 3)
	{
		for (long i = 0; i < hull_count; i++)
			hull[i] = unique_points[indices[i]];
	}
	else
	{
		hull_count = 0;
	}
	return hull_count;
}

/* gift wrapping: the indices of the convex hull of the points, starting from
   the lowest */
// score 144 (was 144): r149, r154 and r155 unchanged; earlier 144 (was 144), 144 (was 161), 161 (was 169); frame 0x18 against retail's 0x14; the count, the departure flag and the
// hull count still live in their own homes where retail keeps them in the dead argument homes. One note, the rounds' entries below.
// MiniMax E (11-20): no new form, no lower score (144): the frame is still 0x18 against retail's 0x14 because the
// departure flag is a dword local here and a byte in retail's dead indices home (our hull count then takes a dead
// argument home at +0x34 where retail has its own slot at +0x10), and the count stays in EBX across the candidate scan
// where retail re-reads it from its home before the index store. The body stands.
// MiniMax I (11-20): no new form, no lower score (144): the departure flag as a bool gives retail's frame of 0x14 and a byte flag in a dead argument home, but the home is then the count's (the count is kept in a register) where retail re-reads the count from its own home and keeps the flag in the indices home, and the score rises to 264 (762 bytes against retail's 784); the body stands.
// M3 lane 14 r112 (fast with 0x239d50; control 144, aligned 137, masked 65): retail's pass loop is the hull count itself,
// short, in its own cell at +0x10 (reloaded into EBX at the loop top, `cmp bx, word [count]`, top-tested, back edges from
// the two departure tests), the flag a byte in the dead indices home, the heading zeroed before the lowest scan, and the
// compaction calls memcpy. A body written that way (hull count as the while counter, early returns) 269 at 757 B
// (frame 0x14 and the byte flag in an argument home as in retail, 0x239d50 still matches, but the loop is rotated and
// points/indices swap ESI/EBP); on main's body: bool flag 264 (0x239d50 80), plus short hull count 252 (793 B), plus the
// heading first 251, plus memcpy 241 (785 B, 0x239d50 still 80); bool flag with memcpy alone 256 with 0x239d50 MATCHING
// (memmove is what breaks the caller with a byte flag).
// MiniMax E (11-17): no new form, no lower score (144): retail keeps the hull count in a frame slot of its own at +0x10
// with EBX holding the copy, re-reads the count from its argument home as a word for both the hull and the candidate
// bound, keeps the departure flag as a byte in the dead indices argument home and starts the previous heading at zero
// before the lowest scan; this build keeps the count in a register for the whole function, the flag as a dword of its
// own and the heading after the scan, so the frame stays 0x18 against retail's 0x14. The forms this round added to the
// earlier list: the previous heading initialised at the top of the block (215), the departure flag as a byte (the
// matched caller 0x239d50 drops to 80 with it), the hull count driving the hull loop on its own with the index
// written from it (260), and the count's bounds read from the argument home as words (the same code).
// MiniMax I (11-17): no new form, no lower score (144): a short hull count gives retail's 0x14 frame and puts the
// departure flag's byte in an argument home (236), but the flag has to stay a dword to keep the matched caller 0x239d50
// (a bool flag drops it to 80), so the frame stays 0x18; the one-counter while form (260), the two-continuation form
// (326) and the pass counter as a short (261) all move the loop shape or the homes away from retail's.
// MiniMax H (11-18): score 144 (was 144): no new form, no lower score; the frame stays 0x18 against retail's
// 0x14, retail keeps the departure flag as a byte in the dead indices argument home where this build keeps a
// dword of its own, and retail starts the previous heading at zero before the lowest scan where this build
// takes it after; the byte flag and the short hull count were measured again and they move the argument homes
// (236) or the loop shape (260). Seventeen further forms (the flag as a byte, bool, char, short and unsigned
// char, the hull count as a short and as an int, the hull count as the loop counter, the pass counter as a
// short) all measure 174 to 264, the compaction's copy as memcpy instead of memmove measures 145, and the
// permuter found nothing below 144.
// Earlier score line (144, was 161): earlier 161 (was 169); frame 0x18 against retail's 0x14; the count, the departure flag and the hull count still live
// in their own homes where retail keeps them in the dead argument homes.
// MiniMax G (11-12): score 161 (was 169): the departure update is now a short-circuit with the hull's first index
// passed to the difference helper first, and the compaction runs through memmove; the frame is still 0x18 against
// retail's 0x14, and the count, departure and hull-count homes still differ.
// MiniMax D (11-11): no new form, no lower score (169); the one-counter body still rotates the pass test to the loop's foot and keeps the count in a register where retail reloads it from its home, so the two-counter body is kept.
// Earlier score line (169, was 169): earlier 169 (was 170); at 170 (was 170) native output ordinal separates iteration from produced length;
// frame 0x18 versus 0x14, count/departure homes, equality and compaction still differ.
// MiniMax C (11-10): score 169 (was 169): no lower score this round. The frame is still 0x18 against
// retail's 0x14; retail's outer pass has one dword counter in EBX, stored at F-0x10, reloaded at the
// loop's head and compared as a word with the count home, it copies the count into EDX for the scan
// bound and reads the count home again for the current index where this build keeps the count in EBX
// and compares the scan index against it, its departure flag is a byte in the dead index argument's
// home where this build has a dword of its own, and its compaction is the CRT's memcpy where the
// reduced image has it inline. The single-counter forms were measured again: the while over the
// counter 242, the same with a short comparison 201 (this one matches retail's frame), a for over the
// same counter 259, the short comparison with the departure flag as a byte 262 (this one puts the
// flag in retail's index home and the counter in the frame), the departure flag as a byte on its own
// 256, and casting the count in the scan or in the current index 169 each. A 5400 second permuter run
// (3398 tries) reached 158, but every gain comes from forms that are not allowed: a pointer local used
// once (162 on its own), moved declarations, and the compaction with its source and destination
// swapped, which changes the copy. Measured apart, each of the permuter's other edits is 169 or worse
// (the comparison of the two points with the arguments swapped 171, the compaction bound as a word 176)
// and 176 or worse together, so the body stands.
// MiniMax C (11-10): score 169 (was 169): no lower score this round. One hull counter driving the outer
// pass (`while ((short)hull_count < count)` with `indices[(short)hull_count] = current; hull_count++;`)
// gives the frame 0x14 and retail's homes for the distance, the best angle, the previous angle and the
// angle, but the counter then sits in the dead index argument's home and the pass test rotates to the
// loop's foot: 203. The same counter with the departure flag as a byte puts that flag in retail's index
// home and the counter in the frame: 223. A short hull counter 275 (751 bytes); moving the heading's
// initialization ahead of the lowest scan 262 (757 bytes). Retail's shape is one dword counter at F-0x14
// compared as a word with the count home, the scan bound copied into EDX, the departure flag a byte in
// the index home, the pass test at the loop's head, and the compaction a CRT call where this body has it
// inline.
// MiniMax C (11-10): score 169 (was 170): the departure test is the negated difference predicate assigned
// straight to the flag, so the flag is written in the branches; the frame is 0x18 against retail's 0x14, and
// the count and departure homes, the scan bound and the compaction still differ.
// MiniMax S (11-08): no new form, no lower score (170); the frame is 0x18 against retail's 0x14, and the count and departure homes, the scan bound, the equality test and the compaction still differ.
// MiniMax G (11-06): a bool departure flag 248, the angle zeroed before the lowest scan 207, both together 204, the
// hull counter merged into the pass counter as a while loop with the increment at the top 272, and a 16-bit copy of
// the count as the scan bound 170 (the same code); no lower score.
// Earlier at 203: frame 0x14 agreed; count/departure homes, scan bound, equality and compaction differed.
// Round 132 (m3 lane 5, fast): index store and increment as two statements 203; `best_distance` declared
// after the index store 203; departure as `if (!left_start && !points2d_equal(...)) left_start = true;` 227
// (767 B); NONE exit tested before the current/heading updates 273 (753 B); the two equality exits joined
// with `||` 203. Retail's compaction is the CRT call 0x320890 and its hull loop has no entry guard.
// Round 27 (lane 9): memmove for the compaction (retail calls the CRT) with a bool flag and the heading set before the scan gives
// retail's count home +0x10, scan bound EDX and early heading/flag stores, but points takes EBP and the flag lands in points' home
// (retail: indices in EBP, the flag in its home) 265 (759 B); with the flag declared first 265; long flag 242; memcpy kept 250;
// `left_start = left_start || !points2d_equal(...)` 273.
// Frame 0x14 agrees; retail keeps `count` in memory (ebx only as temporaries:
// the call argument, then the hull count), ours gives `count` ebx for the whole body.
// Earlier: scan/count registers, equality scheduling and compaction inline boundary differ. Tried direct index output 261; plain lowest scan 250; full current index 299; full candidate counter 256; native completion returns 279; plain scan direct output 250; plain scan append then count 250.
// Full count tests 274; current-point snapshot 271; byte start state 250; float workspace names 250; still-at-origin state 264; inline classifier declaration 250; forced equality declaration 250.
// Unsigned word result 275; unsigned first/candidate ordinals 247/209 (caller lost); native compaction ordinals 285;
// monotonic departure 263; compaction shared exit 279; heading initialized after lowest scan 234.
// signed-word departure state 208; native selected-next index 273; departure before classifier 218; unsigned compaction ordinals 248; lowest constants after current index 234.
// Round 212: Unsigned-word/native-long departure 208/203; result lexical home 208; native lowest-scan ordinal 612; entry heading lifetime 237. Result/departure slots, bound and compaction boundary remain.
// Round 229: Equality-helper false-first/true-first/separate/early rejection results with long
// departure 260/260/260/212; with Boolean departure 249/249/249/234. Original helper/body restored.
// Departure/output/both/current/heading names 203 each; byte departure 234; late departure and
// lowest-coordinate order 203/203; heading before scan 220.
// Shared difference predicate early returns bool/byte/short/long 220/220/202/213;
// initialized difference result bool/byte/short/long 271/271/250/250. All restored.
// On the long departure state: signed-word/unsigned-word/unsigned-native/int result
// 257/257/203/203; native input count 259, with signed/unsigned word result
// 268/268; observed signed-word input count 203. Caller checked with each.
// Round 152: Classifier rejection with entry result, long/bool/byte/short departure
// 262/262/262/259; post-classification result 256/256/256/251. All restored.
// Equality input contracts (pointer order, first/second/both native references), each
// with long/Boolean departure: 203/227/203/234/203/234/203/234. Helper and body restored.
// Round 59: retail keeps the hull count at +0x10 and the departure flag as a byte in the index
// output's argument home, sets the heading and the flag before the lowest scan, copies the count for
// the scan bound, does not rotate the hull loop (test at its head, three back edges) and stores the
// flag as `x = 0; if (...) x = 1`. Tried: departure bool/char/short 234/234/208, each with the heading
// set before the scan 252/252/227 (bool: retail's two homes), long with it 220; on bool + heading
// first: flag set only when unequal 265 (751 B, aligned closest), loop test without the short cast 245,
// heading before the flag 250, flag after the current index 251.
// Round 102: the overlapping compaction copy is retail's CRT call (`memmove`, as matched
// memmove callers): 230 (759 B; `count` then leaves ebx, but `points`/`indices` move); with a
// bool flag and `left_start = left_start || !points2d_equal(...)` (retail's two direct stores) 247;
// plus heading/flag stored before the scan and a done flag in the hull condition (retail's
// unrotated loop and j-loop registers) 229; best declaration order of those 227; lowest_y
// declared first 203. One use of `count` fewer moves it to memory (diagnostic only).
// Round 105: on the done-flag form `count` keeps a callee-saved register whichever single use is
// removed (outer bound, scan bound, the current index's start, the call argument), so it is not one
// use over a threshold; renaming count/hull_count/left_start changes nothing. With `current = count`
// declared before the classifier call: 153 (784 B; done flag, kept only on a match), lowest_y before
// lowest_x 151; the hull-loop declarations after the index store 155; memcpy for the compaction 215;
// the increment at the end of the pass 176; the flag as `if (!left_start)` 246; the two final exits
// as an else-if 151. There points/current/count sit in EBX/ESI/EDI (retail: points ESI, current
// EDI, count in memory, the hull count cached in EBX).
// Round 16: count read through its address at the first-scan/hull bounds 203 each; explicit false/true
// departure branches with Boolean state and early heading, with memmove/memcpy 253/286; existing
// displacement helper 203; shared start-point pointer 253; read-once start index 265, with memmove 263.
// Original body restored.
// Round 239: separate lowest-scan/hull-pass/candidate-scan bound values, all
// seven combinations 203 each; all bounds with overlapping CRT compaction 230.
// Original bounds, equality helper, compaction and body restored; caller intact.
// Round 382: all four coordinate epsilon thresholds in four producer orders,
// with/without both candidate-angle thresholds, 204/204/204/204/204/204/204/204.
// Original threshold expressions and complete executable body restored.
// Round 385: Natural nonterminal continuation pairs with long/bool departure
// and memcpy 340/340/338/338, with memmove 323/340/307/307. Original terminal tests restored.
// Round 387: Parameter-derived current index live before classification, with Boolean departure, CRT overlap compaction and constant heading combinations 245/215/236/277/261/247/250/251; no added completion flag. Original body restored.
// Round 233: Complete current/candidate coordinate carriers at each candidate,
// squared-distance expression in seven subsets, then overlap CRT compaction:
// 253/287/278/203/253/287/278/289. All original coordinate reads, arithmetic and body restored.
// Round 182: native unsigned-word input count, long/bool departure and
// late/early heading crossed with memcpy/memmove 260/232/258/239/253/259/261/266;
// matched caller becomes 34 in each reduced trial. Original signature and body restored.
// Round 181: lowest/candidate-distance/ranking rejection subsets 203 each (seven forms); complete rejection graph with explicit stdcall 203. Original body and contract restored.
// Round 185: Coupled entry heading/departure bool/byte/short/long with memcpy/memmove
// 259/267/259/267/259/249/259/244; actual output cursor, long/bool departure and CRT combinations
// 249/266/277/264, with early heading 268/250/276/263. Cursor frame grows to 0x1c and four memcpy forms lose
// the caller. All state lifetimes, ordinals, original compaction and body restored; native result/departure
// homes and count register remain.
// Round 309: Advancing native lowest/candidate/both point cursors 203/200/200; duplicate
// index cursor alone/with lowest/candidate/both 231/230/229/228; all with overlap CRT 229.
// The 200 forms gain only three; original scans and full body restored. Count/departure homes remain.
// Round 171: retail's hull loop is tested at its head with the entry and all three back edges
// jumping to one `hull_count < count` test, and its three early exits skip the compaction: the
// shape of a completion flag in the loop condition (`while (!done && ...)`, compaction under
// `if (!done)`); an ordinary while, `for (;;)` with the test or with the compaction inside it at the
// head all compile to our rotated loop (203). On the flag form (match-only): 151; with the
// departure flag as `if (!left_start) left_start = ...` 246 (793 bytes); with memcpy 215. Without the
// flag: Boolean departure, heading first, memmove, `||` update and `current` at function scope 253
// (746 bytes); direct returns 262; short hull count 257; `current` at function scope 245, with the
// Boolean departure 215; the heading at function scope 231. Original body restored.
// Round 51 (lane 11, fast): on round 105's done-flag form (153; ours keeps count coalesced with current in
// ESI across the classifier call and reloads points into EBX after it, retail keeps points in ESI and
// reloads current = count from memory): short hull count without casts 249 (754 B, caller 0x239d50 80);
// the index store split from the increment 153; that store before best_distance 153; the hull test before
// the done flag 201. On the original: the departure flag declared first at function scope, long/bool,
// 224/218 (767 B; retail's flag sits in the index argument's home, ours gives that home to the hull count).
// Round 480: Shared native point-difference distance/angle/both producers
// 203/213/213; with squared-length sum 203/213/213; paired squared terms
// with/without the named sum 213/213. Original body restored; count/departure homes remain.
// Round 41 (lane 14): retail's first departure test compares the y term with `fxch; fcompi; jb` (the exit
// test does not). Boolean departure with heading first and the `||` update 264, with memmove 273 (748 B,
// points/indices in EBP/EBX); that with `|=` 295; long departure with `||` and memmove 236; done-flag
// diagnostic on the memmove form 229.
// Round 480: Shared native point-difference distance/angle/both producers
// 203/213/213; with squared-length sum 203/213/213; paired squared terms
// with/without the named sum 213/213. Original body restored; count/departure homes remain.
// Round 495: Standard duplicate search with long/short result and short/long outer
// ordinal 243/243/235/235; overlap CRT forms 261/251/268/268. Original body restored;
// count/departure homes and persistent bound allocation remain.
// Round 348: independent short/native output ordinal with long/bool departure and
// memcpy/memmove 261/250/260/262/170/259/248/264; native ordinal with original
// long departure and memcpy retained at 170, 785/784 bytes, plain confirmed.
// On that graph, short/word produced length 242/242; earlier current/pass lifetimes
// 249/260; one native count for all admitted phases 274. Follow-ups restored.
// Round 351: Heading/departure/current lifetimes on the retained native output-ordinal graph;
// 204/215/254/244/209/245/244/228. Original executable source restored.
// Round 275: Native current/next indices together, crossed with Boolean departure,
// early heading and overlap CRT compaction 269/272/269/281/275/262/273/284. Every form loses
// the matched caller; original word indices, helper and complete body restored.
// Round 362: Independent produced-length increment, crossed with short/native output ordinal, long/bool departure and memcpy/memmove.
// 280/258/275/261/239/234/211/249. Original executable source restored.
// Round 470: Distinct lowest/candidate/compaction ordinal identities, subsets
// 1/2/4/3/5/6/7, then all with output ordinal named: 170/170/170/170/170/170/170/170. Original body
// restored; extra frame word and count/departure/bound/compaction homes remain.
// F338: Produced length derived from the output ordinal only at terminal/exhausted exits,
// long/short ordinal, long/bool departure and memcpy/memmove: 259/266/283/268/270/277/231/274.
// All restored; the extra frame word and count/departure/compaction homes remain.
// N331: Completion on the native output-ordinal graph, bool/byte/short/long with memcpy
// 226/226/244/246, overlap CRT 279/279/281/283. NOT kept: conditional completion requires
// an exact match; every form regresses and loses the reduced caller (80). Original body restored.
// DH306: complete candidate-ranking decision as bool/byte/short/long 170 each;
// both complete lowest and candidate decisions 170 each. Original body restored;
// extra frame word, count/departure homes and compaction boundary remain.
// M330: Eager-length bound/output-index advance/native compaction-extent ownership,
// four forms with memcpy then memmove: 294/212/212/298/317/220/220/319. The 220
// forms recover frame 0x14 and 784 bytes but regress; original body restored.
// Round 543: Eager produced-length exits on the native output-ordinal graph, classifier/
// geometry/compaction returns and combinations: 256/185/275/256/254/222/254; all with
// overlap CRT copy 255. All regress; original stores, exits and complete body restored.
// F346: Completed lowest-coordinate, candidate-ranking and hull-geometry lifetimes in
// seven subsets 170 each; all with overlap CRT 259. NOT kept: 762/784 bytes, caller 80.
// Original complete executable body restored.
// Round 43 (m3 lane 2, fast): short hull count as `while (hull_count < count)` with
// `indices[hull_count++] = current`, bool departure and direct `return hull_count` exits 243 (753 B;
// frame 0x14 as retail, but the count stays in ESI and the departure byte lands in the points home);
// that with previous_angle declared before the lowest scan 273, with a long count 271; the original
// with previous_angle declared first 207; the original's pass loop as `while (hull_count < count)`
// with `indices[hull_count] = current; hull_count++;` 267 (756 B). Original body restored.
// B385: Existing complete 2D magnitude with component/aggregate displacement, scalar/native
// angle producer and memcpy/memmove: 170/170/180/180/259/259/259/259. No byte match;
// native vector grouping, shared header and complete original executable source restored.
// B391: Actual lowest/current/candidate external point references, subsets 1-7, then
// complete external coordinate references: 170/170/170/170/170/170/170/170. Original
// complete body restored; extra frame word, count/departure and compaction homes remain.
// DH316: Unsigned-word/signed-64-bit pass, long/bool departure and memcpy/memmove:
// 257/254/187/264/319/299/299/292. NOT kept: all regress; two word CRT forms lose the
// caller. Original pass, state, copy, helpers and entire executable body restored.
// AC508: Actual unrounded distance/angle/delta domains, wide subsets 1/2/4/3/5/6/7:
// 234/216/255/249/263/265/259; native float CRT pair 170. NOT kept: no exact precision match;
// original body restored. Extra frame word and count/departure/compaction homes remain.
// F356: Existing equality predicate with shared/native/scalar/absolute coordinate producers, original long then Boolean departure:
// 208/208/170/170/254/254/248/248. NOT kept: no qualifying match or gain; original complete body restored.
// Lane 13 r58 (fast): one retail-shaped body (short hull count, `while (hull_count < count)` with
// `indices[hull_count++] = current`, bool departure and heading declared ahead of the lowest scan, memmove
// compaction) 265 (759 B; frame 0x14 as retail, but points in EBP and indices in EBX where retail has ESI/EBP, and
// the zeroed count via a zero register); on it departure declared first 265, `current` declared first 264, a
// separate `hull_count++` 265, memcpy 275 (751 B), the `||` departure update 273 (748 B).
// N354: Existing epsilon equality at departure/completion/both 171/170/171;
// both with bool/byte/short/word departure 248/248/175/175, overlap CRT 259.
// NOT kept: tie/regression; original helper bindings, state and complete body restored.
// F363: Native point-owned predicate, instance/static with bool/byte/short/long
// results: 170/164/171/170/170/164/171/170. NOT kept: six-point byte gain below
// threshold; extra frame word remains. Original helper, point type and body restored.
// B399: Complete scalar distance producers on the native output-ordinal graph: double
// displacements 172; native/wide squared length 170/172; native/wide paired squares 170/172;
// double displacements with native/wide sum or wide squares 173/172/172. Original body
// restored; extra frame word, count/departure homes and compaction remain.
// P303: Native-width lowest/candidate/compaction ordinals on the retained output-ordinal graph,
// subsets 1/2/4/3/5/6/7: 654/262/180/642/651/259/635; unsigned native candidate 262.
// NOT kept: every form regresses, some lose caller 0x239d50; original scans, output ordinal and body restored.
// F365: Lowest/distance/ranking rejection on the retained native output ordinal, masks 1/2/4/3/5/6/7, then classifier early return:
// 170/170/170/170/170/170/170/256 fast. NOT kept: no qualifying byte match; complete original body and helpers restored.
// B418: Native lowest-point state, aggregate/component initialization and both
// current-declaration priorities: 170 each; all four with prior heading before scan
// 207/207/206/206. NOT kept: grouped state needs a match; original body restored.
// B420: One spatial equality result serves departure and geometric completion,
// bool/byte/short/long with long then Boolean departure: 301/301/298/301/313/313/297/297.
// NOT kept: no exact match; original complete body restored.
// B424: Exact endpoint termination of admitted lowest/pass/candidate scans,
// subsets 1/2/4/3/5/6/7 then all with duplicate outer scan: 170/170/170/170/170/170/170/170.
// NOT kept: no exact match; original signed bounds and complete body restored.
// B427: First-hit duplicate search completed before copying, short/native outer and
// inner ordinals with memcpy/memmove: 251/251/248/255/252/264/252/262. NOT kept: no match or gain;
// complete original body restored. Extra frame word and count/departure homes remain.
// B430: Complete coordinate-bound producers on the retained native output ordinal,
// four producer orders, without/with paired angle bounds: 179 each. NOT kept: no
// match or gain; original body restored. Frame remains 0x18 versus native 0x14.
// F393: Actual classifier result short/long/int/dword, then complementary admission bool/byte/short/long:
// 170/170/170/170/170/170/170/170. NOT kept: no byte match; original complete body/helpers restored;
// extra frame word and produced-length/departure/count/compaction homes remain.
// P345: Actual advancing output cursor retaining the independent pass bound/eager length,
// long/short pass, long/bool departure and memcpy/memmove: 251/267/251/276/299/258/253/266.
// NOT kept: no byte match; original complete body restored; extra frame word remains.
// B451: Actual output cursor supplies produced length from its difference to
// indices, plain for/while, long/bool departure and memcpy/memmove: 266/271/250/275/266/271/250/275.
// NOT kept: no eligible exact count/storage graph. Original full body restored;
// extra frame word and count/departure/compaction homes remain.
// T58: Whole-scan current-point pointer/reference, indexed/advancing candidate, original/overlap CRT:
// 228/228/259/259/222/222/249/249. NOT kept: no eligible match or gain; complete original source restored.
// Extra frame word, produced-length/departure homes and compaction remain.
// MiniMax A (11-05): retail's compaction copies the first (i - j) entries from `indices` to `indices + j` (`lea ecx, [ebp + eax*2]` is the destination) where this body copies them the other way round, a behaviour difference; changing the direction alone leaves the score at 170 because the copy is still expanded inline; one 16-bit hull counter driving the outer loop with the increment at the top of the body, a bool departure and that copy 243 (frame 0x14 and the first loop as retail, but the count argument takes EBP, the points argument drops out of a register and the code shortens to 753 bytes); the same counter with a long hull count 227 and with a long departure 255; the short hull count alone 242 and a bool departure alone 248; no lower score.
// MiniMax A (11-05): one 16-bit hull counter with `while (hull_count < count)` and a bool departure 245 (frame 0x14 and the first loop's `mov edx, ebx` as retail, but the points argument drops out of a register, the indices argument takes EBP and the code shortens); the same counters both 16-bit with the two-counter body 259 (the same rotation); a 16-bit pass counter with the long hull count 261 (the frame stays 0x18); no lower score.
// M3 lane 15 r60 (fast with 0x239d50): retail's compaction pushes the size, `&indices[j]`, then `indices` (EBP), so the copy's
// destination is `indices` as here (the direction claim in the MiniMax line above is backwards). Retail-shaped body (short hull
// count, `while (hull_count < count)` with `indices[hull_count++] = current`, bool departure and heading before the lowest scan,
// `return hull_count` exits, memmove) 271 (750 bytes: points take EBP, the hull count ESI, indices reload into EBX each pass, the
// loop test is duplicated at the bottom); that with points2d_equal `__forceinline` 271 (same code); points2d_equal
// `__forceinline` on this body 170 (same code); the retail-shaped body with `goto finished` exits 265 (759, as lane 13 r58), with
// a long departure 242 (as lane 9 r27); this body with a short hull count 242 (795, 0x239d50 then 80); `hull_count = pass + 1`
// stored before the index 210.
// P385: Candidate geometric-admission outcome bool/byte/short/long, positive then complementary:
// 170/170/170/170/170/170/170/170. NOT kept: no exact frame/count/departure or compaction repair;
// original magnitude, ranking, loops, copy and complete body restored.
// Lane 11 round 80 (fast with 0x239d50; m = register-masked aligned count, this body m 85). Retail's loop head is
// top-tested with no bottom copy, the hull count lives at +0x10 (reloaded into EBX at the head and after the scan), the
// departure flag sits in the indices argument's home and the count is compared from its argument home. Short hull count
// with `while (hull_count < count)`, bool departure, heading before the scan and goto exits 275 (751 bytes, m 86), with
// return exits 273 (753), with a top `if (hull_count >= count) break;` 275; a `done` flag loop with the pass in an if/else
// and the compaction in its else arm 271 (754, m 78). With memmove for the compaction: goto form 265 (759, m 65), `done`
// form 242 (791, m 56: retail's top-tested head, flag home and three epilogues, but the hull count in ESI and the count in
// EBX), return form 271 (750). On the 242 form: departure as `? false : true` or `== false` 242 (same code), written out
// with `!(<)` and `>=` 230 (764, caller 0x239d50 lost), as `if (!left && !equal) left = true` 236 (762, caller lost), as an
// if/else pair 236 (762, caller lost); a long hull count with short casts 243; a `short total` scan bound 242 (same code);
// the lowest-point scan as an inline helper 242 (same code), on the goto form 260 (762); the append as an inline helper
// taking the hull count by address or reference 242 (same code), as the `while` condition with goto exits 242 (same code
// as the `done` form), with return exits 256 (767). Grid of 20 over those choices: long departure 243-251, heading and
// flag declared after the scan 248-251, return exits 254-257, nothing below 242. All restored.
// Round 224 (m3 lane 3, fast with 0x239d50; aligned counts in brackets, this body 104): retail's departure update is
// direct byte stores (0 between the two coordinate tests, 1 on a difference). `if (points2d_equal(...)) left_start =
// false; else left_start = true;` gives them with no setcc: on this body 168 at retail's 784 bytes [97] (a two-point
// gain, not kept), with memmove as well 169 (784) [102]; with a bool flag 256 (767); bool flag, that update and the
// angle declared before the scan 204 (784) [93]. This body's registers are retail's (count EBX, indices EBP, points
// ESI reloaded in the inner loop), and they come only with the long pass against a widened count (the extra frame
// word): a short pass 276 (751, points EBP); short pass and short hull count 226 (767); short hull count 239 (0x239d50
// then 80); `(short)pass < count` 211 (765, frame 0x14 but points EBP), with a cast index 227; cast index only 227
// (767, frame 0x18). Retail-shaped body (short hull count as the while counter, bool flag, angle first, memmove):
// return exits 271 (750) [109], goto exits 265 (759) [98]; with the if/else update and gotos 273 (748) [93]; with
// the equality written out in `left_start = !(...)` and gotos 267 (757) [88], then a long hull count 267 [89], five
// declaration orders 266-267 or 238 [88-89], memcpy 275 [96], a long current 282 (0x239d50 lost); the top test
// inside `for (;;)` with the compaction under it compiles as the goto form (273/267). Each of these keeps points in
// EBP, the hull count in ESI and reloads indices from its argument home, and rotates the loop test to the bottom;
// retail tests at the top (three back edges) and reloads the hull count from +0x10 at each join. An inline
// duplicate-removal helper taking the count by pointer: not inlined 331 (663), forceinline 271 (same code).
// Lane 6 r167 (fast with 0x239d50, which stays matched; base 169, aligned 174, masked 86): the compaction through memmove
// (retail's CRT call 0x320890; the copy overlaps) 170 at 784 B but masked 65, the compaction block then as retail's; the
// three exits as `return hull_count;` with no label 186 (768 B); points2d_differ as `>= eps || >= eps` 173 (788 B), as
// `!(< eps) || !(< eps)` 170 (masked 85); memmove plus the heading declared ahead of the lowest scan 204 (masked 63). Our
// pass counter sits in the dead indices home and the departure flag in the frame, retail the other way round.
// Round 70 (m3 lane 15, fast with 0x239d50): the departure as `equal ? false : true` 171 (786 B); the if/else departure (the
// 168 form) with the flag declared first at function scope 252, with the left-start exit nested 168 (re-measure: the 168
// form's code), with `short next` declared first in the pass 170, with previous_angle declared beside the current index 206; `bool
// left_start` with the plain update and the nested exit 248. Lead: retail stores the departure byte directly (0 between the
// coordinate tests, 1 on a difference) and its y test is `fxch; fcompi; jb` where the exit test's is `fcompi; jbe`, so the
// two comparisons are not one expression in retail; no form here gives the fxch.
// M3 lane 16 r108 (fast with 0x239d50, which stays matched unless noted; m = register-masked aligned count, this body m 92):
// a retail-shaped completion-flag body (match-only: the flag is set inside tests): heading and a bool departure flag set
// before the lowest scan, one long hull counter in `while (!done && (short)hull_count < count)` with `indices[(short)
// hull_count] = current; hull_count++;`, the three exits setting the flag, the compaction under `if (!done)` through
// memmove 228 (763 B, m 34: frame 0x14, the flag in the indices home, the hull count at +0x10, the top-tested head and
// retail's compaction call; left: the count keeps EBX where retail gives EBX to the hull count, so the hull count is
// read from memory at each use and `current = count` copies EBX where retail reloads the count's home); on it `short
// current = count` before the classifier call 134 (784 B, m 35, points/current move to EBX/ESI), current declared first
// in the block 227, the lowest-point state declared first 230, a long `hull_count < count` 128 (784 B, m 54, frame 0x18
// again), a short hull count 227 (765 B, caller 80), the flag at function scope or as a long 228 (same code), a long
// departure 239 (793 B), `for (hull_count = 0; ...)` with `indices[(short)hull_count++]` 227, current before the call
// with the long compare 184 (792 B), `short next` outside the loop 228 (same code), the equality helper's parameters
// declared the other way round, `(b, a)` with the calls `(&points[indices[0]], &points[next])` and the same arithmetic,
// 217 (791 B, m 35: the departure block then has retail's registers, indices[0] in ECX and next in EAX, but the closure
// test gains an fxch retail lacks), a positive equality helper `(b, a)` negated at the departure 208 (setcc departure).
// The same flag alone on this body 258 (768 B, caller 80). Diagnostic (value-changing): bounding the candidate scan by a constant takes the count out
// of the callee-saved registers (scan bound copied `mov edx, ebx` as retail) and gives the hull count ESI, so the
// candidate scan's bound is what keeps the count in EBX here.
// Round 171 (m3 lane 5, fast with 0x239d50): the candidate scan (j loop) as a static inline helper returning the next
// index and writing the best angle through a pointer 519 (511 B, not inlined), as __forceinline 171; a short hull
// count with memmove 233 (0x239d50 80); the departure flag renamed to sort before hull_count (same-size name-order
// tip) 169 (same code). NOT kept (6 points, under the 8-point bar; coordinator 11-11): the departure update as `left_start = left_start ||
// points2d_differ(...)`, the direct form of the flag set under `if (!left_start)`, 163 (plain; 785 B; it gives retail's direct departure stores, 0
// between the coordinate tests and 1 on a difference, as dwords). With memmove for the compaction as well 162 (784 B,
// masked 51 -> 37, plain 162; the compaction block is then retail's apart from the hull count's home), not kept
// (memmove alone is 170). On the 162 form: the flag bool/byte/char 264 (762 B, the function then returns `ret 8`: a
// parameter moves to a register), a short hull count 255 (0x239d50 80), `return hull_count` for the three gotos 183
// (763 B), a short pass counter 250, `(short)pass < count` 240, the difference predicate as `>= eps || >= eps` 169, as
// `!(< eps) || !(< eps)` 169. Left on 162: the frame word (widened count), the rotated pass loop, the long flag's
// dword stores and the hull count in the dead index home.
// m3 lane 1 r98 (fast with the file's functions; 0x239d50 matched unless noted, 0x23a6f0 129 as in the full build; control 161).
// New, all fruitless: `short current = count;` at function scope before the classifier call (on this two-counter body) 264
// (762 B, 0x239d50 80); that with the heading set before the lowest scan 241 (762 B, 0x239d50 80); `indices[hull_count] =
// current; hull_count++;` keeping the pass loop 248 (794 B); `previous_angle = best_angle;` ahead of `current = next;` 163;
// the compaction bound without the cast (`i < hull_count`) 161 (784 B, different code, no gain). Re-measured on this base:
// the heading before the lowest scan 211 (784 B); `++hull_count;` for `hull_count = pass + 1;` 270 (810 B; lane 13 r58 ran
// a separate increment on its while body); the current-first form with the first departure call in retail's operand order
// 264 (same score). Retail computes both departure differences as next - first (`fld [next]; fsub [first]`): the first call's
// (first, next) order on main is not retail's, which costs 1-2 points here (round 171's 162/163).
// m3 lane 3 r239 (fast with the file's functions; 0x239d50 matched throughout; control 161). Retail's closure test has no
// fxch (`fld eps; fcompi st(1); jbe` on both coordinates, falling into the epilogue): a positive `fabs < eps && fabs < eps`
// test, where `!points2d_differ(...)` compiles the second coordinate as `fxch; fcompi; jae`. On this body the closure as a
// positive points2d_equal helper (N354's completion form re-measured on this base) 159 (784 B); `hull_count = pass + 1;`
// after the candidate scan 204; one positive equality helper for both tests with `left_start || !points2d_equal(...)` 168.
// Done-flag family (match-only; lane 16 r108's body re-measured 228 at 763 B): with the positive closure 229 (masked 45 ->
// 42, the fxch gone), plus `short current = count;` before the classifier call 150 (784 B), plus the departure as
// `left_start = left_start || points2d_differ(next, first)` 150, or lowest_y declared before lowest_x 148 (masked 47 each).
// Left in every family: retail caches the hull count in EBX at its top-tested loop head and reads the count from its home.
// P485: First departing index/one-based position owns hull departure history,
// short/word/long/dword 279/277/193/193 and 277/277/188/188. NOT kept: no byte
// match; original Boolean-valued history, eager length, loops and whole body restored.
// Native frame 0x14, count/departure argument reuse and scan bound remain.
// M3 lane 15 r83 (fast, 0x239d50 matched each time): `__assume` on points, on indices, on both, and `count <=
// MAXIMUM_POLYGON_POINTS` 161 each (same code); one hull counter (`while ((short)hull_count < count)`, `indices[(short)
// hull_count] = current; hull_count++;`) with the closure as `if (!left_start || points2d_differ(...)) continue; goto
// finished;` 331 (695 B). Compile-only: that closure on this body is the same code; the one-counter `while`, `for (; c; )`
// and `for (;;) { if (!c) break; ...}` all rotate the pass loop (test copied to its foot).
// m3 lane 1 r103 (fast with the file's functions, 0x239d50 matched in each; masked = register-masked aligned count; control
// 161). Completion-flag family (match-only): `while (!done && (short)hull_count < count)` with `indices[(short)hull_count] =
// current; hull_count++;`, heading and a bool departure flag set before the lowest scan, `short current = count;` at
// function scope before the classifier call, the departure as `if (!left_start) left_start = points2d_differ(&points[next],
// &points[indices[0]]);`, `if (next == NONE) done = true; else { departure; if (next == indices[0]) done = true; else if
// (left_start && <positive closure, both coordinates `< eps`>) done = true; }`, memmove compaction under `if (!done)`: 120
// (784 B, masked 38; the lowest score so far). Left: points EBX, current ESI, count EDI and the hull count ECX in the hull
// loop where retail has points ESI, current EDI, the count read from its home and the hull count EBX; the closure's success
// jumps to a shared exit where retail falls into its own epilogue. On it: current declared after the classifier call 215
// (791 B, masked 35: the prologue is then retail's exactly, but the count keeps EBX in the hull loop and the hull count takes
// ECX), declared just before the lowest scan 216, the departure flag declared first 215; the closure through
// `!points2d_differ(...)` 134 (masked 33: retail's closure layout with an fxch retail lacks), the same as `done = next ==
// indices[0] || left_start && !points2d_differ(...)` 134 (same code); the positive closure as `!(>= || >=)` 131 (791 B), as
// a separate `if` 149, constant first 120 (same code). Same code as the 120 form: current declared before the hull count;
// as the 215 form: the flag declared before the scan, `indices[(short)hull_count++]`, previous_angle after left_start.
// Without the flag, on this body: `if (!left_start) left_start = points2d_differ(&points[next], &points[indices[0]]);` 170
// (masked 61), with a bool flag 264 (762 B, 0x239d50 80).
// More r103 flag forms (on the 215 form, current declared after the call; all match-only, none lower): main's `left_start =
// left_start || points2d_differ(...)` 215 (same code), the flag declared at function scope 215, heading and departure flag
// after the lowest scan 244, the closure through `!points2d_differ` 227 (763 B), `for (hull_count = 0; ...; )` 243, `count >
// (short)hull_count` 215, memcpy for the compaction 234 (755 B, the copy inlined); the lowest-point scan as an inline helper
// returning the index (current from its result) 216 (791 B), forced inline with current declared first 244; as plain
// `static inline` it is not inlined (680 B, 343).
// P492: Float root/angle/equality-absolute API subsets 1/2/4/3/5/6/7,
// then standard float overloads: 161/161/161/161/161/161/161/161.
// NOT kept: extra frame word and count/departure/produced-length homes
// unchanged; original numeric APIs, predicate and complete body restored.
// m3 lane 4 r199 (fast with the file's functions; 0x239d50 matched throughout; control 161). Retail read in full: one dword
// hull counter at F+0x10, top-tested (`mov ebx, [F+0x10]; cmp bx, word [count]`), `indices[(short)hull_count] = current`
// then the increment stored back, all three pass exits and the compaction's `j < i` exit into one epilogue returning BX,
// the departure flag a byte in the indices home, `if (!left_start) left_start = differ(next, first)` (its y test with the
// fxch), the closure positive (both coordinates `fld eps; fcompi; jbe head`, no fxch), the heading zeroed and the flag
// cleared before the lowest scan, current reloaded from the count home after the classifier call. As plain C (one hull
// counter in `while ((short)hull_count < count)`, `return hull_count;` at the three exits, that departure and closure, bool
// flag and heading before the lowest scan): 262 (731 B: points EBP, count ESI, the pass test moves); current declared
// before the call 262 (same code); the closure as `!points2d_differ(...)` 269 (747 B); a long flag 290; the heading and
// flag after the lowest scan 289; the departure as `left_start = left_start || differ(next, first)` 262 (same code).
// On this body: the three gotos as `return hull_count;` with the positive closure 170 (763 B).
// M3 lane 11 r103 (fast with 0x239d50, matched each time). Lane 1 r103's completion-flag form re-measured 120 (784 B; current
// declared at function scope before the classifier call). On it: current assigned after the classifier call 215 (791 B,
// prologue exact but `count` takes EBX for the whole function), current assigned just before the lowest scan 216, the loop
// test as `(short)hull_count < count && !done` 211 (767 B), the candidate scan as a forceinline helper taking the count by
// value 189 (784 B; on the after-call form 212). Diagnostic only (volatile parameter reads, not keepable this round): with
// both `current = count` after the call and the loop test reading the count through a volatile view, 117 (767 B): retail's
// prologue, current reloaded from the count's home, points ESI, current EDI and the hull count cached in EBX; left were the
// lowest-point init order (lowest_y first), the loop entry `jmp` to a top test comparing with the count's home, the
// departure block's ECX/EAX naming and retail's separate epilogue for the classifier-fail path. Either read alone 216/215.
// So retail never gives `count` a register; a plain read through its address (`*&count` or a const pointer) is folded
// away (215).
// M3 lane 13 r96 (fast with 0x239d50/0x239b80, both MATCH unless noted; control 161; the 144 form plain-confirmed with the
// file's functions). Kept: the difference helper measures the displacement from its first point to its second (`to - from`)
// and both calls pass (first hull point, next): 161 -> 144 (784 B). Retail subtracts first from next in both the departure and
// the closure and loads the first index before next's (`movsx ecx, [ebp]` then `movsx eax, dx`), which this gives (main had
// `a - b` with the closure's arguments the other way round). Before it: the departure call alone with (next, first) 162
// (retail's subtraction, but next's index loads first). On the 144 form: a positive equality helper negated at the departure
// 161; the departure flag `bool` 264 (762 B, 0x239d50 80); `(short)pass < count` 240; one long hull counter in `while
// ((short)hull_count < count)` 236 (759 B); the heading zeroed before the lowest scan 213. Diagnostic (match-only): lane 1
// r103's completion-flag body with this helper convention, a positive written-out closure and `current` at function scope
// 116 (784 B; count EDI, points EBX, current ESI, hull count cached in ECX where retail has count in its home, points ESI,
// current EDI and the hull count cached in EBX).
// M3 lane 10 r110 (fast with 0x239d50 and 0x239b80, both matched each time; control 144). Reading: retail's hull loop is
// top-tested with every back edge jumping to its head, which is what jump threading of a completion flag gives (any plain
// loop here is rotated); retail never makes `count` a register variable (after the classifier call `current` is reloaded
// from count's home and the lowest scan bounds on an EDX copy of the call's EBX argument). Without a flag: one hull counter
// in `while ((short)hull_count < count)` with a bool departure flag, `if (!left_start) left_start = points2d_differ(...)`, the
// positive written-out closure and the three gotos 267 (757 B: frame 0x14 and the stack convention as retail, `count` in
// memory, but points takes EBP for the whole function and the flag the points home, indices reloaded into EBX; the loop is
// rotated); that with a long flag 247 (759 B), the closure as `!points2d_differ` 265, the departure as `left_start =
// left_start || ...` 267, `short current = count;` before the classifier call 251 (746 B); the compaction inside the loop's
// exit test (`for (;;) { if ((short)hull_count >= count) { compaction; break; } ... }`, breaks for the gotos) 267 and 251 (the
// same code: the compiler moves it out and rotates). On main's body a bool flag 264 (762 B: count passed in EAX, 0x239d50
// 80). Completion-flag family (match-only) re-measured: current before the call 116 (784 B), the flag declared after the
// heading 116 (same code), current after the call 216 (791 B; prologue, frame, flag slot, points ESI and indices EBP all
// retail's, but `count` keeps EBX for the whole function so the hull count only gets ECX); on that, `dword hull_count` 216,
// lowest_y declared first 216, the closure as `!points2d_differ` 217.
// P536: Explicit unrounded distance/heading/short-circuit absolute-result carriers,
// subsets 1/2/4/3/5/6/7 then complete with both coordinate differences named:
// 144/144/162/144/162/162/162/162. NOT kept: no exact result-transport/frame recovery;
// original real consumers, private predicate and complete body restored.
// P538: Actual missing-successor outcome at the terminal edge, positive/complementary
// bool/byte/short/long: 144/144/144/144/144/144/144/144. NOT kept: no native frame,
// count/departure-home or compaction repair; original producer, terminal edge and complete body restored.
// P547: Complete point2f displacement/opposite-heading producers, original/aggregate/shared
// difference combinations: 154/154/144/154/154/144/154/154. NOT kept: no native frame/count-home
// recovery; original scalars, closure helper, compaction and complete body restored.
// P549: Persistent multiuse selected-distance/angle/delta workspaces, seven subsets,
// then all with the existing successor ordinal retained across passes: eight at 144.
// NOT kept: extra frame word, count/departure/produced-length homes and loop head remain.
// Original complete body and declaration scopes restored.
// P551: Standalone complete lowest-point replacement/retention outcomes in
// bool/byte/short/long: 144/144/144/144/144/144/144/144. NOT kept: no exact frame/count-home repair;
// original body restored. Candidate decisions, all lazy observations and compaction unchanged.
// Round 252 (m3 lane 3, fast with 0x239d50/0x239b80, both matched; control 144). Main's candidate scan is retail's apart
// from frame offsets; the outer region differs in who holds EBX (ours the count, reloaded after the scan; retail the hull
// count, reloaded from +0x10) and in the widened count's frame slot. Main's closure as `if (!left_start ||
// points2d_differ(...)) continue; goto finished;` on the two-counter loop 144 (same code); `hull_count = pass + 1;` ahead
// of the index store 207; `dword hull_count` 144; `dword left_start` 144 (same code each); a `point2f const *origin =
// &points[current];` local for the candidate scan's four reads 216. Re-measured lane 15 r83's one-counter `continue` form
// 331 (695 B: the difference helper is then called out of line twice). Body unchanged.
// P583: Inclusive final-ordinal lowest/pass/candidate bounds, seven subsets, then
// one immutable shared final ordinal: 279/260/260/248/238/249/238/266. NOT kept: no exact native bound/frame
// recovery; original signed loops, observations and complete body restored.
// P586: Independent native real-home identities for the minimum pair, previous heading,
// selected distance/heading/turn and candidate distance/heading/turn: eight at 144.
// NOT kept: no frame/count/departure-home recovery; original complete body restored.
// b8H (11-17) walk against retail, fast build on r836: no logic or structure difference found, 144 unchanged.
// Walked 0x239e50-0x23a15d block by block against ours (same work on the same side of every jcc, NaN included):
// hull count 0 at 0x239e65 and the == 2 gate at 0x239e72/0x239e76; lowest scan 0x239eb0-0x239ef9 (branches at 0x239eba,
// 0x239ebf, 0x239ecc, 0x239ed8, 0x239ee5; FLT_MAX 0x44aed4, eps 0x45dbdc, -eps 0x45ddfc, -FLT_MAX 0x4409d4 read);
// candidate scan 0x239f55-0x23a016 (distance > 0.000244140625 from 0x45de54 at 0x239f7d-0x239f81, atan2(pj - cur) at
// 0x239f87-0x239f91, wrap by g_55e470 at 0x239fb0, accept/skip at 0x239fc4-0x23a009); departure 0x23a040-0x23a084 and
// closure 0x23a089-0x23a0d4 (both next - first, first index loaded before next's at 0x23a048); compaction
// 0x23a0e7-0x23a14f (2*(i - j) bytes from &indices[j] to indices, hull count = i - j as a dword at 0x23a13e).
// One codegen-only NaN difference, not a source one: our closure's y test compiles with fxch + jb (a NaN dy leaves the
// loop), retail's is fld eps; fcompi; jbe 0x239f10 at 0x23a0d4 (a NaN dy continues). The written-out positive closure
// gives retail's form but scores 159 (lane r239), so it is not applied. Still different: frame 0x18 against 0x14,
// the dword departure flag against retail's byte in the indices home, and the hull count's home.
// m3 lane 11 r110 (fast with 0x239d50 and 0x239b80, both matched each time; control 144): the count bound as
// `short const &limit = count;` for the hull and candidate-scan bounds 144 (same code: the binding folds, count keeps
// EBX); a redundant `if (0 < count)` guard around the candidate scan 149, around the hull loop 247; the first hull
// index bound as `short const &first = indices[0];` for its six reads 144 (same code); the departure as
// `if (!left_start) left_start = points2d_differ(...)` 162.
// m3 lane 7 r174 (fast with 0x239d50 and 0x239b80, both matched each time; control 144): `hull_count = pass + 1;` moved
// after the candidate scan (ahead of `current = next`) 187; the hull count indexing and incrementing itself beside the pass
// counter (`indices[hull_count] = current; hull_count++;`) 248 (794 B); best_distance declared last of the pass's four 144
// (same code); the pass loop tested on `(short)hull_count < count` with the pass counter kept for the index 250 (795 B); a
// `point2f const *point` for the lowest scan's six reads 144 (same code). Retail's outer region caches the hull count in EBX
// (its home +0x10 reloaded after the candidate scan, which leaves EBX unused); ours caches the count there instead.
// Duplicate scan from the second index 265, with cached key 265, search before
// the suffix move 222. A short suffix count 144, with a short hull-pass ordinal
// 249. Current after the successor transfer 144, heading published on
// continuation 177, both index and heading published on continuation 253.
// NOT kept: the 144 forms leave the extra frame word and the departure home.
// Input end-pointer bounds with the original short ordinals in the lowest/candidate/both scans 211/250/231;
// derive the ordinal at selection in the lowest/candidate/both scans 270/263/279; candidate/both with byte departure
// 249/297. NOT kept: the lowest-only bound gives frame 0x14 but changes the register allocation at +0x4;
// every candidate endpoint form regresses and loses the matched caller 0x239d50. The 0x18 frame and hull-count home remain.
// m3 lane 13 r109 (fast with 0x239d50 and 0x239b80, both MATCH unless noted; control 144). Re-measured: a bool departure
// flag alone 264 (762 B: the count then arrives in EAX, 0x239d50 80). New: the bool flag with the count read once through
// its address for the classifier call (`short const *const count_home = &count;`, a match-only form) 262 (762 B: the stack
// convention stays and 0x239d50 matches, the flag takes the indices home as retail, but the frame is 0x10 because the hull
// count moves into the count's dead home); every count read through that pointer 262 (same code: folded, the count keeps
// EBX). On the completion-flag body (match-only): bool departure and done flags, `current` at function scope, the positive
// written-out closure and the count_home call 117 (784 B; frame 0x14 and the flag byte in the indices home as retail; left:
// count EDI, points EBX, current ESI and the hull count cached in ECX where retail has the count in memory, points ESI,
// current EDI and the hull count in EBX); that with a short hull count 250 (749 B). Retail never caches the count in a
// register, even in the candidate scan where EBX is free, so the count is not a register candidate there at all.
// M3 lane 15 r99 (fast with the file's nine functions; 0x239d50 MATCH unless noted; [n] = aligned differing lines, control 144
// [137]). Diagnostic: count as a by-value one-short record parameter 144 (same code: the member is promoted like a scalar). A
// named copy of count for the classifier call and the first scan's bound 144 (same code). One short hull counter in `while
// (hull_count < count)` re-measured 236 (759 B) [147], with a bool flag 236 [122]: the frame, the count's EDX copy for the first
// scan and its word reads from the home in the hull loop are then retail's, but points takes EBP, the hull count ESI and indices
// only a per-pass EBX. A 24-form grid on that one-counter body (heading and flag before the lowest scan, the `if (!left_start)`
// departure, the positive written-out closure): exits as three returns, a completion flag or the gotos; a short or long
// counter; current declared before or after the classifier call; a bool or long departure flag. Returns: 261, 267, 261, 291
// (733 B) with the short counter and 262, 269, 262, 290 (731 B) with the long one [93-104]. Flag with a short counter 254, 254,
// 236, 265 (749-762 B; 0x239d50 80 in all four, the count arrives in EAX). Flag with a long counter: current before + bool 116
// (784 B) [109] (lane 1 r103's form), before + long 229 (794 B), after + bool 216 (791 B) [84: retail's prologue; left: count
// in EBX for the whole function with the hull count cached in ECX], after + long 238. Gotos: 251, 252, 267, 247 (746-759 B)
// [113-159], the same with the long counter 251, 252, 267, 247. The completion flag as direct conditions (`done = next ==
// NONE; if (!done) { departure; done = next == indices[0] || left_start && !points2d_differ(...); }`) 162 (797 B), the `||`
// departure 162, the written-out closure 159 with either departure; `if (next == NONE) done = true; else { departure; done =
// ...; }` 121 (791 B), with the written-out closure 116: the gain does not hold in the direct form, so the flag stays
// match-only. On the after + bool flag form: a named copy of count for the call and the scan 216 (same code), for the scan
// alone 216, current started at NONE 215, the candidate scan as a guarded do/while 264 (759 B, 0x239d50 lost). Reading: with
// one hull counter and no flag our build already splits the count as retail does (EBX for the call, EDX for the first scan,
// its home afterwards); with the flag it gives retail's points ESI, current EDI and indices EBP but keeps the count in EBX. No
// form gives both.
// M3 lane 14 r114 (fast with 0x239d50 and 0x239b80; control 144; one-object link screen first, 32 forms): retail's hull loop
// is not inverted (the `(short)hull count < count` test sits at the loop head, reached by a jmp over the alignment pad, and
// every back edge from the closure goes to it), the hull count is the only pass counter (reloaded into EBX from +0x10 at
// the head and after the candidate scan), and the departure flag is a byte in the indices home. On the one-counter
// `while` body: short counter, bool flag, heading and flag declared before the lowest scan, gotos 265 (759 B; screen best,
// frame 0x14 as retail, but the loop is inverted); the same with a long counter 264; the declarations after the lowest scan
// 266; the three exits as returns 265 (749 B); short counter with a long flag 242 (759 B). The loop's end written as
// `if (!left_start || points2d_differ(...)) continue; goto finished;` on that body (screen only) loses 25 rows.
// Still-at-start state as bool/byte/short/long with short-circuit update 264/264/265/265;
// explicit update 262/262/260/263. NOT kept: the 0x14 frame returns, but each form shortens
// the body by at least 21 bytes and changes the matched caller's convention.
// The pass bound still occupies a separate word. The result and departure homes differ. shared short ordinal for
// lowest/candidate 144, lowest/duplicate row 144, lowest/duplicate search 144, candidate/duplicate row 144,
// candidate/duplicate search 144, lowest/candidate/duplicate row 144, lowest/candidate/duplicate search 144,
// shared long lowest/candidate ordinal 651. NOT kept: The seven shared short scan ordinals tie without moving any
// stack home. The shared long ordinal lengthens the function to 1095 bytes.
// M3 lane 4 r219 (fast with 0x239d50 and 0x239b80, both MATCH each time; control 144): the count read once through
// `short const *const count_home = &count;` for the classifier, a bool flag and the compaction through memcpy (retail
// calls the CRT memcpy) 256 (767 B); count_home with memcpy 145; count_home alone 144 (same code). One hull counter
// `while ((short)hull_count < count)` with `indices[(short)hull_count] = current; hull_count++;`, the bool flag and the
// heading set before the lowest scan, memcpy and main's gotos 260 (757 B): frame 0x14, the hull count at +0x10, the flag
// byte in the indices home and the early heading are retail's; left: the pass loop is rotated (retail tests at its head,
// reached by a jmp, with the closure's back edges going there), and the candidate scan loses its `0 < count` guard, so our
// compiler knows count > 0 inside the pass loop while retail re-reads the count from its home every time. On that 260
// form: the pass bound or the classifier argument through a forceinline `short const &` accessor 260/260 (folds, the
// count stays in EBX); `for (;;)` with the bound test first 260 (rotated the same way).
// M3 lane 12 r148 (fast with 0x239d50 and 0x239b80, both MATCH each time; main 144). On lane 1 r103's completion-flag body
// (match-only; re-measured 116, frame 0x14, flag byte and hull-count home as retail): the hull append through a forced-inline
// helper taking the hull count by pointer 116 (same code: the pointer is scalar-replaced); the compaction through memcpy
// 217 (the copy is expanded inline as `rep movs`; it gives retail's prologue to +0x1d, points in ESI and count in EBX, but
// retail calls the CRT's memmove at 0x320890, as matched 0x18e470 does); `if (classifier != 2) return 0;` with the hull
// count declared after it 237; `if (done) return hull_count;` ahead of the compaction in place of `if (!done)` 169 (763 B);
// the heading declared and zeroed at function scope before the classifier call 121. Body unchanged.
// M3 lane 13 r123 (fast with 0x239d50 and 0x239b80, both MATCH each time; main 144, difflib-aligned 137, register-masked 58).
// What still keeps the count in EBX on the completion-flag form (match-only; `current = count` after the classifier call,
// re-measured 216 at 791 B, aligned 84, masked 37: only the count's EBX against retail's home reads with the hull count
// cached in EBX, the scan's EDX copy, and the exit and compaction epilogues differ). On it: the lowest scan through a
// forced-inline helper taking the count by value 216 (same code), the candidate scan through one returning the next index
// with the heading through a pointer 213 (784 B, aligned 95), both 213 (same code as the second: by-value copies of the
// count fold back into it); the hull count as a one-member aggregate local (diagnostic) 216 (same code, scalar-replaced);
// the count read through a `short const &` local for the pass test, the pass and candidate tests, every use after the
// call, or bound inside the block 216 each (same code: the reference folds). Without the flag, re-measured: one hull
// counter in `while ((short)hull_count < count)` with a bool departure flag and three returns 269 (747 B, aligned 143), with
// main's gotos 265 (759 B, aligned 118), with a long flag 252 (752 B), with `return (short)hull_count` 269 (same code).
// Reading: retail's hull count and count both behave as values the candidate scan's sqrt/atan2 may change (reloaded after
// it, never held across it), which no folded spelling reproduces. Body unchanged.
// M3 lane 2 r111 (fast with 0x239d50 and 0x239b80, both MATCH unless noted; control 144, aligned 84, masked 36): the
// completion-flag body (match-only; bool departure and done flags and the heading ahead of the lowest scan, one long
// counter in `while (!done && (short)hull_count < count)`, positive closure, compaction under `if (!done)`) rebuilt 210
// (784 B, frame 0x14, masked 19: left are the count in EBX, the hull count cached in ECX and the pass test repeated at the
// foot). A grid of {short, long with casts} counter x {`indices[hull_count++] = current`, store then increment} x {flag
// exits, three returns}: the append spelling never changes the code; long + flags 210, short + flags 236 (762 B, the count
// arrives in EAX, 0x239d50 80), long + returns 269 (747 B), short + returns 265 (749 B). Diagnostics, all removed: the
// `standard` marker on main's body and on the four flag/return bodies 221/231/292/265/295 with 0x239d50 at 34 each; the
// count's address stored to a global gives retail's memory compare in the candidate scan and a reload after it, but the
// count is still cached in EBX through the pass loop (flag body 171 at 794 B, masked 22; short counter 222 at 784 B; main's
// body 234); the hull count's address stored to a global gives retail's top-tested pass loop behind a jump (240 at 784 B)
// with other registers. Retail loads the count into EBX only as the classifier's argument, copies it to EDX for the lowest
// scan, reads its home again for `current = count` after the call and compares it from memory either side of the index
// store; EBX then caches the hull count.
// M3 lane 2 r115 (fast with 0x239d50 and 0x239b80, both MATCH each time; control 144, aligned 58 with registers and
// slots masked; body unchanged). Reading of retail's entry: EBX and EDI are only the classifier's argument registers;
// after the call the count is copied EBX -> EDX for the lowest scan, `current` is loaded again from the count's home
// (so `short current = count;` stands ahead of the call and is rematerialised after it), and from the pass loop on the
// count has no register at all (three compares against its home per pass) while the hull count owns EBX with its cell
// at +0x10. Re-measured: the one-counter body with main's gotos 265 (759 B); the completion-flag body (match-only)
// with `current` after the call 216 (791 B, aligned 84 unmasked), with `current` ahead of the call 116 (784 B). In both
// flag bodies ours gives the count a callee-saved register for the pass loop (EBX or EDI) and the hull count only ECX.
// Diagnostics, each reverted at once (an unreachable call taking the address, or the address stored to a global): the
// hull count's address taken unreachably on the one-counter body 230 (762 B: `inc dword [hull count]`, the scan's
// bound compared from memory, the loop still rotated); with the count's as well 238 (792 B); with all three parameters'
// too 252; the count's address taken unreachably on the flag body 216 (same code) and on the `current`-first flag body
// 212 (784 B: the count still cached in EDI); stored to a global on the flag body: both 240 (784 B, aligned 182), the
// hull count's alone 242 (768 B), the count's alone 178 (807 B, aligned 88), both on the `current`-first body 245. So
// neither an address-taken nor an escaping count or hull count gives retail's allocation; what ranks the hull count
// above the count for the fourth callee-saved register is still open.
// M3 lane 6 r214 (fast with 0x239d50, MATCH throughout; control 144): a retail-shaped body (one long hull counter as
// `while ((short)hull_count < count)`, bool flag, heading zeroed before the lowest scan, memmove, return exits) whose
// pass body ends in a `return` with the not-closed paths as `continue` (no latch block, to test for retail's top-tested
// head) 345 (655 B: the two difference tests merge into one); with a long flag 295 (683 B); with a short hull counter
// 343 (657 B); the plain return-exit form re-measured 269 (747 B); on it `short current = count;` at function scope
// ahead of the classifier call 271 (748 B). Body unchanged.
// Compaction after the complete walk uses input count 195, republished result 264, separate
// compacted count 250, or named input bound 195. Starting each scan at the first index with
// a predecessor gives 195/257/247/195. NOT kept: all retain the extra frame word and raise the score.
// Equality bounds in lowest scan, hull walk and candidate scan, all seven
// subsets 144. Direct post-test hull walk 267. NOT kept: equality leaves the
// 0x18 frame, and the post-test walk reaches 0x14 but shortens the body to
// 759 bytes and moves the caller to 80.
// Start index held for one output pass, each candidate, the lazy tie result, or the ranking rejection.
// Short and long start indices: 263/249/144/144/237/229/247/247.
// NOT kept: The short read before ranking reaches frame 0x14 but passes count in EAX and raises the score. The two short tie-only forms tie. The pass bound still adds a frame word, and the count and departure homes differ.
// Completed lowest-point scan extent as pass/candidate/compaction bound, seven subsets:
// 144/251/154/225/155/256/256. Signed-word pass with the completed extent at both traversal bounds 244.
// NOT kept: the two-bound form recovers frame 0x14 but raises the score and changes the caller.
// Round 499: sole produced length with equality bound, top and bottom in signed word, full width and unsigned word,
// then signed word with Boolean departure.
// Fast scores in order 251/260/253/249/273/246/228/224.
// NOT kept: Seven forms recover the 0x14 frame. The best scores 224 at retail length and changes the matched caller
// 0x239d50. Every form raises the target score.
// The wider pass bound still occupies the extra frame word, and retail’s count and departure homes differ.
// Separate signed-word loop bounds on pass, candidate, lowest/candidate,
// pass/candidate, lowest/pass or all three 144 each. A signed-word pass
// with its own bound, alone or with a signed-word candidate bound 250/250.
// NOT kept: the copies fold, and the short pass keeps the extra frame word.
// The result and departure homes remain.
// Independent append increment with long or short pass 270/249, pass advanced from published length 144/263,
// pass advanced from incremented length 242/237, short pass and short length incremented independently 249,
// short feedback with fixed compaction prefix 249. NOT kept: the forms reaching frame 0x14 shorten the
// function to 760–762 bytes and regress. The long published-length form ties at frame 0x18.
// M3 lane 14 r135 (fast with 0x239d50): one hull counter as the while variable, seven combinations of a bool/long flag,
// short/long hull count, the short compare and the heading first 233-265 (759-762 B, the loop rotated); main's body with a
// bool flag 264 (the count moves to EAX, `ret 8`, 0x239d50 80) and with the count read once through its address 262 (frame
// 0x10, caller matched; the hull count takes the count's home); the one-counter body with a bool flag, a long hull count, the
// heading first and `short current = count;` above the classifier call 156 at 784 B (count on the stack, frame 0x14, flag
// byte in the indices home and hull count at +0x10 as retail; left: the rotated pass loop, points/current/count in
// EBX/ESI/EDI, the closure's fxch); on it a positive two-axis equality helper for the closure 153, a short hull count 251,
// the short compare 251, the heading after the scan 222, return exits 248.
// M3 lane 16 r165 (fast with 0x239d50 and 0x239b80, both MATCH throughout; control 144): one long hull counter in a
// two-part loop condition `while ((short)hull_count < count && next != NONE)` (no flag; next started at current, the
// two closure exits under `if (next != NONE)`, the compaction skipped after a NONE exit) 251 (793 B: points and indices
// lose their registers); the departure flag declared at function scope ahead of the hull count 262 (764 B); the lowest
// scan's two coordinates in their own `{}` block 144; the classifier's result named before its test 144; a named point
// pointer in the lowest scan 144 (the last three the same code).
// M3 lane 1 r147 (fast with 0x239d50 and 0x239b80; control 144): loop indices widened to long: the lowest scan's 645
// (1095 B, the scan unrolled), the candidate scan's 251 (0x239d50 34), the compaction's two 288 (739 B, 0x239d50 80), the
// lowest and candidate scans' 651 (1095 B), all four 619 (1072 B).
// M3 lane 12 r167 (h2m fast with 0x239d50 and 0x239b80, MATCH in each unless noted; control 144): the pass loop as
// `while ((short)hull_count < count)` with `indices[hull_count] = current; hull_count++;` at its top 248 (759 B: frame 0x14
// and the scan bound copied into EDX as retail, but still rotated, points in EBP); with `indices[hull_count++] = current`
// 248 and with a bool flag 248 (same code); a long compare 260 (764 B); main's loop with `++hull_count` 270 (810 B), with
// `indices[hull_count] = current` 272 (807 B, 0x239d50 80). Body unchanged.
// The frame stays at 0x18 rather than 0x14. The departure flag and produced count still need the dead
// argument byte and EBX. Tried candidate decision from three ranking branches 144, three ranking
// branches with one start-index read 144, candidate rejection gates with one start-index read 144,
// distance and ranking rejection gates 144, three ranking branches with Boolean departure 264, three
// ranking branches and one start read with Boolean departure 264, candidate rejection gates with
// Boolean departure 264, distance and ranking rejection gates with Boolean departure 264. NOT kept: The
// four ranking forms tie with the original departure width. A Boolean departure shortens the body and
// changes the caller.
// M3 lane 2 r131 (H2M_FAST with 0x239d50 and 0x239b80, MATCH unless noted; control 144): the completion-flag body (lane 1
// r103 / lane 13 r96, compaction under `if (!done)`) re-measured 116 (784 B, masked 39); with the compaction unconditional 173
// (762 B); on 116, count read through a const volatile view of the parameter in the hull loop test 224 (794 B), there and in
// the candidate loop 229 (0x239d50 34), in all three loops 235 (0x239d50 34), in the candidate loop alone 242 (0x239d50 34).
// On 116 the homes left are points (retail ESI, ours EBX after the classifier call), current (retail EDI) and count (never
// a register in retail). Body unchanged.
// M3 lane 12 r174 (h2m fast with 0x239d50 and 0x239b80, both MATCH throughout; control 144): ours keeps a long copy of
// count in a frame cell for `pass < count` (the 0x18 frame's extra dword), holds count in EBX through the pass loop and
// gives the hull count the dead indices home, where retail's counter is the hull count itself in its own cell. The pass
// bound compared as a word, `(short)pass < count`, 240 (784 B, frame 0x14, points then EBP); the closure written `if
// (!left_start || points2d_differ(...)) continue; goto finished;` 144 (same code); both 240; the departure flag declared
// beside previous_angle after the lowest scan 240; `hull_count = pass + 1;` ahead of the index store 207. Body unchanged.
// M3 lane 4 r246 (H2M_FAST with 0x239d50 and 0x239b80, MATCH unless noted; control 144). Completion-flag family (match-only):
// lane 1 r103's body (`while (!done && (short)hull_count < count)`, `if (!left_start) left_start = differ(first, next)`, the
// positive written-out closure, memmove under `if (!done)`) with `current = count` after the classifier call and BOTH that
// read and the hull test through the parameter's volatile view 109 (767 B), with lowest_y declared before lowest_x 107: the
// prologue, lowest scan, candidate scan and departure are then retail's instruction for instruction. Left: the hull test
// (`movsx ecx, word [count]; movsx eax, bx; cmp eax, ecx` for retail's `cmp bx, word [count]`, and retail's `jmp` over the
// head's alignment), the compaction's scratch naming, retail's own epilogue for the compaction's normal end, the eps load
// ahead of the count load. On 107: the current read placed after the lowest-point declarations 107, the compaction compare
// written `indices[i] == indices[j]` 107; with only the current read volatile 217, with the hull test through a plain or
// cast address read instead 217 / 217, both through address reads 216, both through a `short const *const` to the count 216
// (the count then takes EBX for the whole body). Reading: the volatile views keep the count out of the register allocation
// (its lowest-scan bound then reuses the classifier's EBX argument, `mov edx, ebx`, as retail); one plain hull-test or
// current read gives it a register. A grid of 32 plain flag bodies (test `(short)hull_count < count` or `count >
// (short)hull_count`, long/short hull count, current set after the call or initialised at function scope, lowest_y/x
// first, bool/long flag) best 114 (784 B: current at function scope, lowest_y first, long hull count; the count in ESI,
// points EBX); the short hull count 235-254. The lowest scan as a forced-inline helper (returning the index from a passed
// start, starting from its count, or writing through a pointer) 216 / 216 / 216, through a pointer with current at
// function scope 114. Body unchanged.
// The widened append bound still holds an extra frame word. Use the minimum scan’s visited count as
// append bound, candidate bound, both, or both with one append counter: short 144/251/225/246,
// long 645/675/616/616. NOT kept: the short append-bound form ties. Other short forms raise
// the caller score. The long count unrolls the minimum scan and grows the body.
// M3 lane 1 r154 (H2M_FAST with 0x239d50 and 0x239b80, both MATCH each run; control 144): lane 1 r103's completion-flag
// body (match-only) rebuilt 118 (784 B, masked aligned 25); with lowest_y declared before lowest_x 116; that with memcpy for
// the compaction 218 (first +0x1d); on 116 the count read through a `short const &` bound at entry, at every read or only
// at the current/hull/candidate reads, 116 / 116, and through a `short const *` to the parameter 116 / 116 (all the same
// code: the view folds and the count keeps its register; only the volatile view of the earlier rounds moved it); the
// done flag replaced by three `return hull_count;` exits with the compaction unguarded 262 (731 B: the hull loop rotates).
// Retail leaves EBX free in the candidate loop yet still reads the count from its home there, so the count is memory
// resident by some property of its source (not register pressure). Body unchanged.
// M3 lane 15 r134 (h2m fast with 0x239d50 and 0x239b80, both MATCH; control 144, aligned 137): the bound written count
// first (an uncounted loop) on the pass loop, the candidate scan, the lowest scan, all three, and the compaction's outer
// loop 144 each (same code). The one-counter form (`while ((short)hull_count < count)`, bool flag, heading before the
// lowest scan, memcpy, main's gotos) re-measured 260 (757 B); with `while (count > (short)hull_count)` 210 at 784 B
// (aligned 128: frame 0x14 and the entry as retail, but `count` stays in EBX and the pass loop still rotates, the hull
// count in EAX); both with `current` and the hull test reading the count through a const volatile view of the parameter
// 216/216 (784 B, aligned 125). Body unchanged.
// M3 lane 13 r147 (h2m fast with 0x239d50 and 0x239b80, both MATCH unless noted; control 144, difflib-aligned 28 groups):
// the compaction's search as `for (j = 0; j < i && indices[j] != indices[i]; j++) {}` with the copy under `if (j < i)`
// 252 (frame 0x14, but the count then arrives in EAX with `ret 8` and 0x239d50 drops to 80); `if (j != i) break;` for
// `if (j < i)` 144 (same code); `if (classifier != 2) return 0;` with the hull count declared after it, on main's body,
// 261 (764 B, 0x239d50 80); the classifier result in a named short 144 (same code); the lowest scan bounded by a
// `short const` copy of the count 144 (same code: the copy folds, no EDX bound). Body unchanged.
// M3 lane 16 r178 (H2M_FAST with 0x239d50 and 0x239b80, both MATCH; control 144). Retail's pass loop is not rotated: the
// entry jumps over the header's alignment to a top test block (`mov ebx, [hull]; cmp bx, word [count]; jge`) and the
// three continuing paths of the closure tests jump back to it, so the closure's last compare falls through to the return.
// On main with one counter (`indices[(short)hull_count] = current; hull_count++;`, main's gotos): the loop as `for (;;)`
// with a top `if ((short)hull_count >= count) break;` 236 (759 B, frame 0x14 as retail; it is rotated all the same, the
// same code as) `while ((short)hull_count < count)` 236; that with `current` initialised at function scope ahead of the
// classifier call 236 (766 B); with a bool departure flag 236; with the candidate bound written `count > j` 258 (762 B).
// M3 lane 13 r149 (H2M_FAST with 0x239d50 and 0x239b80, both MATCH each run; control 144): a retail-shaped body (one long
// hull counter with short casts, a top-tested `while ((short)hull_count < count)`, the index store then `hull_count++`, a
// bool departure flag set under `if (!left_start)`, the three closure exits as `return (short)hull_count;`, the compaction
// after the loop) 248 (747 B: frame 0x14, but points in EBP, indices spilled, the hull loop still inverted); with a short
// hull count and `indices[hull_count++] = current;` 246 (749 B); with the departure flag declared ahead of the classifier
// call 249; with a long departure flag 241 (752 B); with the closure as `if (!left_start || differ) continue; return
// hull_count;` 360 (667 B). Body unchanged.
// M3 lane 4 r256 (H2M_FAST with 0x239d50 and 0x239b80, both MATCH unless noted; control 144, aligned 84; body unchanged).
// 1 to 4 unused initialised longs at the top of main's body (diagnostic) 144 each, 1 to 7 on the completion-flag body
// with `current` initialised at function scope 114 each, 1 to 3 on the flag body with `current` set after the call 216
// each (all the same code: this function does not follow the local count). The count declared `long` with `(short)count`
// at its five uses, on main 144 and on the flag body 216 (same code in both, and 0x239d50 drops to 11: the parameter is a
// short). On the flag body (216 at 791 B, aligned 50): memcpy for the compaction 235 (755 B: it expands in line, so
// retail's CRT call is the overlapping copy as written), the candidate bound through a `short point_count = count` copy
// declared in the pass loop 216 (same code: the copy folds), `current` initialised at function scope 114 (re-measure),
// that with memcpy 218. Reading of the 114 form against retail: the two are mirror images at entry. Retail loads points
// into its register (ESI) and copies it to the classifier's argument register, loads the count straight into the
// argument register and never gives it one of its own; ours loads the count into its own register and copies that to
// the argument, and loads points straight into the argument register. So the count outranks points here and ranks below
// both points and the hull count in retail; no spelling of the count's reads changes that rank.
// M3 lane 2 r145 (h2m fast with 0x239d50 and 0x239b80, both MATCH unless noted; control 144, aligned 84; body unchanged).
// Retail read again: one memory-resident hull counter ([esp+0x10], loaded into EBX at the loop head, `cmp bx, word
// [count]; jge` to the compaction), the index store then the increment at the head, the count never in a register
// after the lowest scan, the three closure tests returning EBX straight from the loop (the compaction runs only when
// the loop test fails), the departure flag a byte in the indices argument's home, previous_angle zeroed ahead of the
// lowest scan. The one-counter body (`while ((short)hull_count < count)`, `indices[(short)hull_count] = current;
// hull_count++;`, bool flag set under `if (!left_start)`, three `return (short)hull_count;` exits) re-measured 248 at 747
// B (the count stays in memory as in retail; points EBP, indices spilled, hull count ESI, where retail has ESI, EBP and
// EBX, and the loop rotated); the three exits as `goto finished;` to the one return 236 (759 B, rotated); the index
// store as `indices[(short)hull_count++] = current;` in a `for (; ...; )` head 248 (same code); previous_angle zeroed
// ahead of the lowest scan 270; the body ending in `return` with `if (!left_start) continue; if (differ) continue;` 357
// (669 B, 0x239d50 8). A forced-ranking search on the 236 form (40 single decisions) finds nothing below 236: the
// register exchange and the rotated loop are not one allocator decision away.
// M3 lane 8 r234 (H2M_FAST with 0x239d50 and 0x239b80, both MATCH unless noted; control 144, aligned 137, masked 56; body
// unchanged). Frame view: here the hull count (12 references) joins the dead indices argument home and the dword departure flag
// makes the sixth cell; in retail a one-byte flag is packed first into that argument home, so the hull count opens the frame
// (+0x10), and that needs the count's own home still live, which is the count staying in memory through the pass loop. Thirty
// forced single allocator decisions on main's body (colour picks, priorities, range cuts): none below 144. Forms: the departure
// update as `if (!left_start) left_start = points2d_differ(...)` 162; `current` as a long 281 (746 B, 0x239d50 34); the lowest
// scan's index as a long 645 (1095 B: the scan unrolls); `hull_count = pass + 1` ahead of the index store 207; the departure
// flag declared after the lowest scan 240.
// The result and departure homes still differ. Post-pass short/long hull counter with one terminal continuation
// 254/237, completion test 245/254, three terminal returns 256/239, one merged terminal return 256/239.
// NOT kept: the short counter reaches the frame size with different homes and every form raises the score.
// Minimum point with coordinate/whole-point updates 144/244, with heading initialized first 215/233,
// with the start index seeded before the classifier 264/224, and with both changes 242/216.
// NOT kept: the aggregate copy moves the frame away from retail. The early start index variants
// that reach frame 0x14 shorten the body to 762 bytes. Every early start variant changes the caller.
// Duplicate span as short, published before/after the copy with the nested breaks
// 144/146, and with the single exit 231/231. Long span with breaks 144/161,
// and with the single exit 230/230. NOT kept: the breaks keep frame 0x18.
// The single exit recovers 0x14 but shortens the body and loses the matched caller.
// M3 lane 14 r155 (H2M_FAST with 0x239d50 and 0x239b80, both MATCH each run; control 144; body unchanged). The closure
// through the file's existing two-axis equality helper (moved above this function) 152, with its arguments the other way
// 150; on lane 1 r103's completion-flag body (match-only; rebuilt here 119 at 791 B) 148 / 118 (that helper compares in
// double, `fld qword` for retail's single epsilon). On the flag body: the lowest scan's result in its own local copied into
// `current` before the hull loop 242 (784 B); `current` assigned after the classifier call 217 (791 B, exact to +0x28,
// points ESI and current EDI as retail); on that a named pointer per point in the lowest scan 217 (same code) and a named
// pointer to the current point for the candidate scan 238. Allocator reading of the 217 form: count, points and the hull
// count all start at priority <= 0 and are split; count's piece from the entry to the end of the hull loop (priority 21)
// takes EBX one pass before the hull count's hull-loop piece (76) is formed, so the hull count is left ECX. A forced-ranking
// search over priorities and colour picks finds nothing below 217; only splitting the hull count's range at the candidate
// scan (150-155 in that search) gives retail's EBX hull count with the count read from its home, so the split point is
// the cause.
// Duplicate search before compaction, with paired indices, outer cursor, stopped cursors, or saved span
// 222/221/215/251 for short ordinals and 266/279/276/278 for long. NOT kept: all increase
// the score and change the matched caller. The widened pass bound and departure home still differ.
// The separate pass-bound spill and the reported departure home still differ.
// Separate departure and closure arms with dword/bool state 233/230,
// index first and shared coordinate test 278/284, single coordinate result 278/275,
// and index first with distinct state continuations 293/279.
// NOT kept: the separate arms recover 0x14 and 784 bytes but raise the score.
// The other forms shorten the body and five change the matched caller.
// M3 lane 16 r190 (fast screen with 0x239d50 and 0x239b80, both MATCH; control 144, aligned 137, masked 58; body
// unchanged). Retail's pass-loop head read once more: after the index store it sets `next` to NONE in EDX, tests the
// candidate count against the count's home, stores the hull count, then the best angle (its cell) and the best delta
// (XMM1). Forms: the best distance declared last of the four bests 144 (same code); `next` and the three bests ahead of
// the index store and the hull count 146 (aligned 141); the bests in distance, delta, angle order with `next` after
// the hull count 146; the heading zeroed right after the departure flag, ahead of the lowest scan (retail's store
// order), 213 (aligned 135, masked 56); the candidate differences formed candidate minus current and passed to atan2
// 266 (760 B: the distance and the angle then share them, retail forms them twice).
// The extra frame word, persistent count and hull-count home still differ.
// Cursor-derived ordinals for the minimum/candidate/both scans 270/262/256;
// pointer-pair duplicate compaction alone and with minimum/candidate/both
// scans 286/269/267/265. Candidate identity compared as a pointer 254.
// Every cursor form raises the score and six change the matched hull caller.
// M3 lane 16 r191 (h2m fast with 0x239d50 and 0x239b80, both MATCH unless noted; control 144, aligned 137, masked 56).
// Ranking read: every piece of the hull count has a weighted cost at or below zero (stored each pass, read at the end),
// so the count's piece over the lowest scan keeps EBX. Forms: `current` declared at function scope (from the parameter)
// 264 (762 B, 0x239d50 80); the lowest point as a point2f (copied whole on a new low) 244 (796 B); a pointer to the
// scanned point in the lowest scan 144 (same code); the point2f with `current` at function scope 224 (784 B, aligned
// 171, 0x239d50 80); the candidate offset through this file's point-difference helper into a vector2f 144 (same code).
// M3 lane 4 r264 (h2m fast with 0x239d50 and 0x239b80, both MATCH each run; control 144; body unchanged): the pass
// loop under a redundant `if (0 < count)` guard 247 (frame 0x14 as retail, but points in EBP and the hull count in
// ESI); the classifier result named and tested through a const reference 144 (same code); the departure flag
// declared after the lowest scan, just ahead of the pass loop, 240; the hull count set through a reference local 144
// (same code); the three exits as `break` with the compaction under `if (pass == count)` 230.
// M3 lane 8 r244 (fast screen with 0x239d50, which MATCHES in every form below unless noted), at 144. Frame reading:
// objects are packed smallest first, then by falling reference count, trying the argument homes before new slots;
// retail's byte departure flag is packed first and takes the indices home because count's home stays live (count
// is re-read from it), so the hull count (12 references) gets a slot of its own; ours packs the dword hull count into
// the indices home and gives the flag a sixth slot (frame 0x18). Forms: the flag as bool 264 (762 B; 0x239d50 80),
// with memcpy for the compaction 256 (767 B); the pass loop as the hull count itself (`while ((short)hull_count <
// count)`, `indices[(short)hull_count] = current; hull_count++;`, no pass counter) with the bool flag and memcpy 204 at
// retail's 784 B and frame 0x14, every instruction through the classifier call as retail (lead: left are count kept
// in EBX, where retail re-reads it from its home for `current` and gives EBX to the hull count at the loop head, and
// the heading's zero store, which retail makes before the lowest scan); that with the heading declared before the
// flag 260 (757 B), after `current` 262 (757 B); the one-counter loop with the bool flag and memmove 236 (759 B), with
// the dword flag 236 (759 B).
// @retail 0x239e50
short function_239e50(short count, point2f const *points, short *indices)
{
	long hull_count = 0;

	if (function_239b80(count, points) == 2)
	{
		long left_start = false;
		real lowest_x = FLT_MAX;
		real lowest_y = FLT_MAX;
		short current = count;

		for (short i = 0; i < count; ++i)
		{
			if (lowest_y - k_real_epsilon > points[i].y ||
				lowest_y > points[i].y && lowest_x + k_real_epsilon > points[i].x ||
				lowest_y + k_real_epsilon > points[i].y && lowest_x - k_real_epsilon > points[i].x)
			{
				lowest_x = points[i].x;
				lowest_y = points[i].y;
				current = i;
			}
		}

		real previous_angle = 0.f;
		for (long pass = 0; pass < count; ++pass)
		{
			real best_distance = -FLT_MAX;
			indices[pass] = current;
			hull_count = pass + 1;
			short next = NONE;
			real best_angle = FLT_MAX;
			real best_delta = FLT_MAX;

			for (short j = 0; j < count; j++)
			{
				real dx = points[current].x - points[j].x;
				real dy = points[current].y - points[j].y;
				real distance = (real)sqrt(dx * dx + dy * dy);
				if (distance > 0.000244140625f)
				{
					real angle = (real)atan2(points[j].y - points[current].y, points[j].x - points[current].x);
					real delta = angle - previous_angle;
					while (delta < -k_real_epsilon)
						delta += g_55e470;
					if (delta < best_delta - k_real_epsilon ||
						!(delta > best_delta + k_real_epsilon) &&
						(j == indices[0] || next != indices[0] && distance > best_distance))
					{
						best_distance = distance;
						best_angle = angle;
						best_delta = delta;
						next = j;
					}
				}
			}

			current = next;
			previous_angle = best_angle;
			if (next == NONE)
				goto finished;
			left_start = left_start || points2d_differ(&points[indices[0]], &points[next]);
			if (next == indices[0])
				goto finished;
			if (left_start && !points2d_differ(&points[indices[0]], &points[next]))
				goto finished;
		}

		for (short i = 0; i < (short)hull_count; i++)
		{
			short j;
			for (j = 0; j < i; j++)
			{
				if (indices[j] == indices[i])
				{
					hull_count = i - j;
					memmove(indices, &indices[j], sizeof(short) * (short)hull_count);
					break;
				}
			}
			if (j < i)
				break;
		}
	}

finished:
	return hull_count;
}

PRIVATE __forceinline void polygon_difference_23a220(point2f const *a, point2f const *b, vector2f *result)
{
	result->i = a->x - b->x;
	result->j = a->y - b->y;
}

// The polygon's edges and the point's offset are both differences of two vertices: the edge
// runs from the current vertex to the next one (wrapping round the end), the offset from the
// current vertex to the tested point. A point that lies farther from an edge than the radius
// is outside the polygon, so the function returns false for it.
// @retail 0x23a160
bool function_23a160(point2f const *point, real radius, short count, point2f const *points)
{
	short total = count;
	bool result = true;
	real radius_squared = radius * radius;
	for (short i = 0; i < count; i++)
	{
		long next = (i + 1 >= total) ? 0 : i + 1;
		point2f const *current = &points[i];
		vector2f edge;
		polygon_difference_23a220(&points[next], current, &edge);
		vector2f offset;
		polygon_difference_23a220(point, current, &offset);
		real edge_j_value = edge.j;
		real edge_i_value = edge.i;
		real length_squared = edge_i_value * edge_i_value + edge_j_value * edge_j_value;
		if (length_squared != 0.f)
		{
			real cross = offset.i * edge.j - offset.j * edge.i;
			if (cross > 0.f && cross * cross > length_squared * radius_squared)
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

/* true when the point lies on the inner side of every edge of the polygon,
   to within epsilon */
// @retail 0x23a220
bool function_23a220(point2f const *points, short count, point2f const *point, real epsilon)
{
	bool result = true;
	for (short i = 0; i < count; i++)
	{
		point2f const *current = &points[i];
		long next = (i + 1 >= count) ? 0 : i + 1;
		vector2f offset;
		polygon_difference_23a220(point, current, &offset);
		vector2f edge;
		polygon_difference_23a220(&points[next], current, &edge);
		if (edge.i * offset.j - edge.j * offset.i < -epsilon)
		{
			result = false;
			break;
		}
	}
	return result;
}

PRIVATE __forceinline bool clip_points_equal_23a6f0(point2f const *a, point2f const *b, real epsilon)
{
	return fabs((double)a->x - b->x) < epsilon && fabs((double)a->y - b->y) < epsilon;
}

// MiniMax S (11-20): no new form, no lower score (129): retail reads the count argument as a dword home (mov ecx, [ebp+8]; movsx edi, cx) and keeps it in ECX to the loop guard, where this build reads a word home and keeps the previous side in CL instead of a byte home; the count's width comes from the caller's declaration in src/unknown_17d9a0.cpp, and an int copy of the count measures 320.
// MiniMax G (11-19): no new form, no lower score (129): retail reads the count's home as a dword and narrows it
// afterwards (`mov ecx, dword [ebp+8]` then `movsx edi, cx`), so retail's count parameter is a four-byte type while this
// declaration's `short count` is read as a word home and sign-extended at once; that prototype is also declared in
// src/unknown_17d9a0.cpp, which this lane may not edit, so the count's width is BLOCKED-BY: the zero constant follows
// the count into XMM4 where retail builds it in XMM2, and the two side bytes keep their homes. Body unchanged.
// MiniMax G (11-19): no new form, no lower score (129): retail keeps both side bytes in memory and loads the count into
// ECX at the alias join where our build narrows it to a sign-extended word, and the count's width comes from the
// `short count` prototype in src/unknown_17d9a0.cpp, which this lane may not edit; the previous side computed
// straight from the distance expression, the side test as `previous_inside != inside`, the side updated before the
// previous point, `inside` declared once ahead of the loop and the count named once all score 129 or worse.
// Body unchanged.
// MiniMax S (11-17): no new form, no lower score (129): retail reads the count argument as a dword home and keeps the low word in cx, where this declaration's short count is read as a word home and the previous side is kept in cl; the parameter's width is declared in another file this lane may not edit, so the zero and epsilon registers and the side bytes stay out of step.
// MiniMax E (11-14): no new form, no lower score (129): retail still reads the count argument as a dword home and keeps
// it in CX, and that parameter's type is declared in another file this lane may not edit; the zero constant still takes
// xmm4 where retail creates it in xmm2, and the previous side byte still sits at [esp+0xd] where retail has it at
// [esp+0xc]; the count guard in front of the loop (92) is not taken, as it repeats the loop's own i < count test.
// MiniMax E (11-14): no new form, no lower score (129): retail still reads the count argument as a dword home and
// sign-extends the low word, where this declaration's short count is read as a word home, and that parameter's type is
// declared in src/unknown_17d9a0.cpp, which this lane may not edit; retail also keeps the previous side in memory where
// this build keeps it in CL.
// MiniMax B (11-14), NOT kept: 92 (was 129) with a `count > 0` guard in front of the loop (retail's count load
// into ECX and its `test cx, cx` loop guard at 0x23a78b), the fraction as an if/else (retail's else arm that
// materialises the zero at 0x23a887) and the products spelled `current->x * plane->n.i`. The guard repeats the
// loop's own `i < count` test, so it is kept only with a byte match: without it the other two changes score 249,
// and the guard around the `previous` setup and the loop together (where it would also protect the
// points[count - 1] read) scores 271 (coordinator). At 92 what was left: the zero constant in XMM1 late where retail
// creates it in XMM2 early, so the loop's float homes are one register out; 890 bytes against retail's 887.
// MiniMax B (11-12): no new form, no lower score (129, was 129): retail reads the count argument as a dword home
// (`mov ecx, [ebp + 8]; movsx edi, cx`) where this declaration's short count is read as a word home, and that type
// is declared in src/unknown_17d9a0.cpp, so the first difference stays; retail also keeps the previous side in
// memory where this build keeps it in CL.
// MiniMax G (11-13): no new form, no lower score (129): the SIMD homes carry the bulk of the score and
// they are a pure renaming: retail holds the zero constant in xmm2 and the difference dy in xmm0 (the
// dead loop temp), where this build holds dy in xmm2 and the zero in xmm4, so every instruction of the
// clipped branch differs while the arithmetic is the same. The count's ECX home is reproducible only by
// a third use of the count: the `count > 0` guard (211, wrong control flow) or moving
// `previous_inside = inside;` to just before `if (inside)` (157); that second form reproduces retail's
// general registers exactly (`mov ecx,[ebp+8]`, `movsx edi,cx`, `test cx,cx`, the loop head's
// `mov ecx,[ebp+0xc]` and the `mov edi,edi` pad) but swaps the two side bytes (this build keeps the
// previous side at [esp+0xc] and the current at [esp+0xd], retail the other way round) and keeps the
// SIMD renaming. Declaring the side flags in retail's order (current, previous, positive, negative)
// changes no code (157, same bytes).
// MiniMax G (11-13): no new form, no lower score (129): retail materialises the count in ECX at the top of
// the main path (mov ecx,dword ptr [ebp+8]; movsx edi,cx; test cx,cx), so the previous side has no register
// and lives in the byte at [esp+0xd]; this build re-reads the count from its home (movsx edi,word ptr
// [ebp+8]; cmp word ptr [ebp+8],0) and keeps the previous side in CL, and the loop head takes EDI where
// retail takes ECX. The count stays a short in retail (the alias branch loads word ptr [ebp+8] and the
// guard compares ax with word ptr [ebp+8]), so this is the count's lifetime inside the body and not its
// width; the declaration that fixes its width lives in src/unknown_17d9a0.cpp, which is not mine to edit.
// MiniMax I (11-15): no new form, no lower score (129): retail loads the count's four-byte home and keeps the low word
// in CX for the loop guard, so the previous-side bool is spilled to esp+0xd, while this build loads the sixteen-bit home
// and keeps the bool in CL; the home is read as a dword only when the parameter is a thirty-two-bit type, and that width
// is fixed by the declaration in src/unknown_17d9a0.cpp, which is not this lane's file.
// MiniMax D (11-16): no new form and no lower score (129): a `count > 0` guard in front of the loop does give retail's count load (`mov ecx, dword ptr [ebp + 8]`, `movsx edi, cx`) and its `test cx, cx` guard, but the flow then differs elsewhere - `i = 0` is stored after the guard where retail stores it before the `jle`, the zero constant stays in XMM4 against retail's XMM2 and the function grows to 893 bytes (211; 92 only together with retail's fraction if/else and `current->x * plane->n.i`); the guard repeats the loop's own `i < count` test, so it may be kept only with a byte match and is restored here; the permutation search over the body (44 tries, 409 s, annealed, one job) reached 128, one point better than the baseline.
// MiniMax C (11-19): no new form, no lower score (129): retail reads the count argument as a dword and narrows it to a short for the indexing (`mov ecx, dword ptr [ebp + 8]`, `movsx edi, cx`, `test cx, cx`) where this build reads it as a word; the count's width is this function's signature and the caller in src/unknown_17d9a0.cpp declares the same short parameter, so widening it here is a signature change this lane may not make (a measurement with a dword count and a narrowed short scored 332 and 826 bytes).
// MiniMax I (11-20): no new form, no lower score (129): retail reads the count's home as a dword and narrows it afterwards, while this declaration's short count is read as a word and sign-extended at once; the prototype is also declared in a file this lane may not edit, so the count's width is BLOCKED-BY.
// score 129 (was 129): Retail length and the mask update agree; retail keeps the previous side in memory
// aligned frame and stride/prior homes agree; prior-side byte and SIMD homes differ.
// MiniMax D (11-11): no new form, no lower score (129); retail reads the count argument as a dword home (`mov ecx, [ebp+8]; movsx edi, cx`) where this declaration's short count is read as a word, and that parameter type is declared in src/unknown_17d9a0.cpp, so the first difference stays.
// M3 lane 1 r96 (fast, no guard): the two side-presence flags in one declaration 129; the crossing deltas as a vector2f
// (j then i) 129; the crossing outputs y first 131; dx written at its two uses 241 (880 B); the clamp as if/else-if 129.
// Widening the count is BLOCKED-BY 0x17dd80 (its only caller, a contributor's, pushes the short raw).
// MiniMax S (11-08): no new form, no lower score (129); retail keeps the previous side in memory and this build keeps it in CL across the back edge, and the prior-side byte and SIMD homes differ.
// MiniMax G (11-06): no new form, no lower score; retail's count parameter is a dword (mov ecx, [ebp + 8]; movsx edi, cx;
// test cx, cx, and mov ebx, [ebp + 8] for the returned count) where this declaration takes a short, retail keeps the current
// side at [esp + 0xc] and the previous side at [esp + 0xd] where this build keeps the previous side in CL.
// Round 132 (m3 lane 5, fast): the previous side's register is the one cause of the entry and loop-head
// differences. `previous_inside = inside;` moved from the loop end to just before `if (inside)` spills it
// (its life then crosses the append's `shl cl` shifts): count is then loaded once into ECX (`mov ecx,[count];
// movsx edi,cx ... test cx,cx`), `points` is loaded into ECX at the loop head and the pad is `mov edi,edi`, all
// as retail, but the copy merges into the inside test (157); with the fraction as if/else as well, retail's
// XMM4/XMM5 constants and the shared zero-fraction block (164; zero XMM1, dy/fraction XMM2/XMM0 remain).
// No form with the copy at the loop end spilled it: `points[i]` at each use (no `current`) 129 (same code);
// the previous side declared uninitialised with the flags 129; both sides declared first 129; on the if/else
// fraction those and `inside` at function scope 249 each; a separate source pointer for the alias copy
// 170/172 (points arrives in ECX); zero-denominator arm first 198, on the moved copy 223; if/else-if clamp
// 249, on the moved copy 164. Diagnostics: a volatile previous side 246; a two-element side array 147
// (scalar-replaced, back in CL).
// Earlier: Retail length and the mask update agree; retail keeps the previous side in memory
// (ours CL across the back edge, one instruction short from the side test on), loads count once into ECX
// for the scaled index and the guard, and keeps zero in XMM2. Earlier: count/stride/prior homes remained
// (retail keeps the copy size at +0x18, the previous point at +0x1c and both side flags in memory).
// Earlier: count/stride homes and distance/intersection lifetimes differ. Tried capacity break common result 245; shared mask return 254; existing plane dot 243; plain previous side 265; previous side branches 259; byte side presence 243; named distance products 243; native clipping role homes 243.
// Native current ordinal 249; long side states 264; prior-point ordinal 288; ordered fraction clamp 243; clear-mask arm first 243; shared normalized count 275; existing point difference 243.
// Explicit source bit 243; unsigned input ordinal 237; word side states 263; retained signed distance 273; parallel edge scales 243.
// signed side-presence words 248; signed current-side byte 256; double equality tolerance 302; native equality references 237; ordinary unsigned previous-side byte 278.
// Round 212: Current-side unsigned word/native long 257/250; input ordinal name 237; true-first current side 341; positive-first final policy 236. Count/stride/prior homes and shared publication remain.
// Round 229: Equality helper native first/second/both point arguments 356/357/444;
// result-first with those arguments 356/357/444; pointer result-first/early rejects 237/237.
// Original helper/body restored.
// Previous-point, distance, positive-state, negative-state and result names 237 each;
// status declaration order 264; previous-point address 237; displacement order 237.
// Actual NONE failure/common publication unsigned/signed ordinal 212/252; ordinary previous side 227/254;
// positive-first policies 211/251/256/253. Keep only the minimal 212 form; one-point tail edit restored.
// Native previous/plane/both observations 378/370/354; both native points 355;
// all native inputs 376; complete plane/previous/both scalar observations 347/359/368.
// Original inputs, copies and body restored.
// Fraction default between/before deltas, before clipped publication and at iteration entry:
// unsigned ordinal 211/211/209/209; signed ordinal 259/259/263/275. All restored;
// the best three-point change does not qualify. Count/stride/prior and SIMD homes remain.
// On the common failure-result graph: signed word/native count at entry or after
// alias handling, each without/with shared copy extent: 212/289/211/211/303/225/212/212.
// One-point forms rejected; original count observations, helpers and body restored.
// Round 143: retail materialises the point-mask test as a 0/1 value (`neg; sbb; neg; je`) before
// the set/clear; SET_FLAG with TEST_FLAG / with a `(bool)` test 212/212, a named bool 212, `!!` 212,
// `? true : false` 212 (all simplified to `test`); SET_FLAG plus signed short index 252, plus the
// previous side not volatile 227, plus both 254.
// Round 59: retail keeps the index as a word in memory (+0x14, tested `cmp ax, [count]`), the
// previous point at +0x1c, the copy size at +0x18 and the plane reloaded from its argument home.
// Tried: the previous side's volatile removed 227; signed short index 252 (869 B; retail's loop
// tail, but the previous point stays in ESI); both 254 (frame +8, the widened index spilled);
// long index without the volatile 272; the distance sums with the terms swapped (loop, fraction,
// both) 212 (identical).
// Common-failure graph mask production: conditional result 212/252; clear then set
// 275/266; named output bit 222/252; named bit with conditional result 222/252
// (unsigned/signed word ordinal). Original ordinal, mask and body restored.
// Round 62: without the volatile the plane sits in ESI and the previous point spills as in retail,
// but the previous side stays in CL and the frame grows 8 (227); with a signed short index too it
// gives retail's loop tail but spills the widened index separately (254, 879 B); signed short alone
// 252; `(bool)` on the point-mask bit 212 (identical; retail materialises it with neg/sbb/neg),
// with no volatile and short index 254.
// Round 102: with the volatile: previous side as `= false; if (...) = true` 255, long index 266;
// without it: that if/else 250, the current side declared beside the previous one 227, the previous
// point through a named `count - 1` index 229 (harness: `points + count - 1` and a word-cast index
// 220/277, unchanged or worse; without the volatile the previous side lives in CL, retail's in memory).
// Round 105: the mask update as SET_FLAG(mask, result, point_mask ? TEST_FLAG(...) : false) (retail's
// materialised bit, `neg; sbb; neg`, with the NULL mask threaded to the clear arm; `&&` folds it to `test`)
// with a signed short index and the previous side's volatile removed 133, plus the previous distance
// named like the loop's 129 (kept); the bit as `(*point_mask >> i) & 1` alone 177; on 129: the fraction as
// an if/else 249 (XMM4/XMM5 constants as retail, flag still in CL); side flags byte/char/long/short,
// function-scope sides, swapped side test or loop-end copies, dot_product2d distances, a named
// `count - 1` or copy size, the current point after the distance: no gain.
// Round 17: entry previous-side direct assignment with signed/unsigned ordinal 254/227; entry previous
// pointer with signed ordinal 254; false-first entry side with signed/unsigned ordinal 272/246;
// previous-point lexical name 254; shared copy extent 265; output-mask lexical name with that extent
// 265. Original declarations, qualifier, ordinal, names and body restored.
// Round 267: On the native mask graph, prior true-first/false-first side results 132/130;
// current true-first/false-first 284/284; paired true/true, true/false, false/true,
// false/false results 284 each. Count ECX and prior-side byte home remain; original body restored.
// Round 366: previous-distance carry 156; two-use intersection pointer 129; both 156; shared 2-D difference 129, with distance/pointer/both 156/129/156; joint point cursor 262. All restored.
// Round 369: prior/current/both outside values 257/268/325; outside values with test-assigned fraction 253/266/316; both with negative accumulation 344; complementary side presence 134. All restored.
// Round 109: retail loads count once into ECX after the alias copy (`mov ecx, [count]; movsx edi, cx`,
// guard `test cx, cx`), so ECX is busy at entry and the previous side spills (+0xd, current +0xc);
// ours reads count from memory twice and keeps the previous side in CL. The alias test reversed 130;
// the clipped reset after the alias copy 220; mask declared first 132; plane n.j * dy 129; the
// previous side as `0 <= distance` 129 (compile-only screens of nine more orders and spellings: none).
// Round 262: equality-producer byte/short/long result domains and ordinary inline
// metadata 129 each; single-exit bool/byte/short/long equality results on the
// native-mask/signed-ordinal graph 129 each. Original helper and body restored.
// Round 256: Native short/native long point counts at entry/alias join, each with default/test-
// assigned fraction 129/249/133/251/296/298/254/259. All restored.
// Round 217: shared final-copy predicate with original/positive-first policy and copy
// after/before normalization 279/299/287/291; with reused byte extent 292/308/296/300. Original
// body restored; count ECX, prior-side byte and SIMD homes remain.
// DL196: coordinated equality parameter priorities 129/129/129/129/129;
// complete intersection and two priority interactions 255/255/255.
// Original executable body and all supporting contracts restored.
// Round 171: retail loads count once after the alias copy (`mov ecx,[ebp+8]` for the stride and
// `test cx,cx` for the guard), so ECX is busy at entry, the previous side stays in memory and the
// loop head gets ECX for `points`; ours keeps the previous side in CL (loop end one store short).
// Every operand order of both distances, the deltas, the denominator, the numerator and the outputs
// compiles identically (120-form screen); mask first 132; side flags before the result 132; the
// fraction default before the deltas 240 (880 bytes); one named copy size 129; one inline
// append-and-dedupe helper for both tails 129; `points + count - 1` / `&points[count] - 1` 129.
// Round 453: Plain current-point cursor with native/memcpy/memmove publication 280/280/306;
// original indexed walk with point memcpy/memmove 129/337. Native intersection on the cursor
// graph with native/memcpy append 235/235; complete memmove publications 298.
// All original indexed traversal, native stores, copies and helper contracts restored.
// Round 54 (lane 16, fast): the previous side computed straight from its distance expression (no previous
// distance local) 133; the previous side stored before the previous point at the loop end 129 (same code); the
// result declared after the flags and mask 132; the side test as `previous_inside != inside` 129 (same code); the
// current side `bool const` 129 (same code). Diagnostic only: an `if (count > 0)` guard around the loop 211.
// Round 46 (lane 1, fast): the previous side as `!(distance < 0.f)` 129 (same code); the previous point as
// `points + (count - 1)` 129; a named `count - 1` index local 273; the loop test `count > i` 130; the clipped
// reset after the alias copy 220; the fraction negated with unary minus 129 (same code).
// Round 56 (lane 16, fast): the previous side as `= false; if (distance >= 0) = true` 130; the inside test written
// as `distance >= 0.f` at the append 265 (874 bytes); the side flags renamed (side/previous_side, last_inside,
// was_inside) 129 each (same code and slots: the names do not move the byte flags here).
// The NONE result arm written first (`if (result == NONE) memcpy ...; else {...}`) 129 (same code).
// Round 63 (lane 12, fast; an LTCG env link with the stand-in callers and 0x17dd80 is exact): the negative test as
// `-epsilon > distance` 129 and as `distance < 0.f - epsilon` 129 (same code); the current point as `points + i` 129;
// the entry distance with the x term first 129 (same code); the inside test after the epsilon tests 136. Retail loads
// `points` into ECX at the loop head (ours into EDI, coalesced with the current point), so ECX is never free across
// the loop there and the previous side stays in memory; a `(void)&` on the previous side, both sides or count changes
// nothing (env).
// Round 51 (lane 11, fast): `if (count > 0)` directly around the for loop 211 (893 B): count is then loaded once
// into ECX (`movsx edi, cx` stride, `test cx, cx` guard), the previous side goes to memory and the loop head
// takes ECX, all as retail (aligned 85 -> 38); with the fraction as if/else (or a ternary) 92 (890 B, aligned 32,
// eps/one in XMM4/XMM5): left are the zero in XMM1 (retail XMM2), the intersection registers permuted (retail
// dy XMM0, denominator/fraction XMM1), `i = 0` stored after the guard (retail before `jle`) and count hoisted
// into ECX for `result = count`. The guard repeats the loop's first test, so it needs a match. On that graph:
// dx written at its uses 147 (887 B, aligned 27, zero XMM2); dy at its uses 300; both 289; dx first 95;
// outputs y first 97; clamp as if/else-if 92; denominator terms swapped 92; `fraction = 0.f` default before the
// deltas 97; fraction/dx/dy declaration orders 92/95/92/92; vector2f delta j-i/i-j/initializer 91/94/94;
// polygon_difference_23a220 deltas 94; clipped store after denominator/clamp/deltas 286/250/99; `result =
// count` after the copy 264; NONE arm first 96; index `= 0` before the guard 249 (129, the original code, without the else).
// Without the guard: `if (count > 0)` around the previous point too 274; `if (count != 0)` 265; index at
// function top with the guard 263; guarded do/while 128; while loop / function-scope index 129 each; previous
// distance from points[count - 1] 268; count read through its address 129; 1-6 unused locals 129 (diagnostic);
// previous side volatile 149 (diagnostic, count still read twice; reverted).
// Round 480: Unified polygon side-presence mask, byte/word/dword/long, branched
// 278/278/278/278 and conditional collection 302/278/278/278. All restored;
// native count ECX, prior-side byte and SIMD homes remain.
// Round 41 (lane 14): no matched game function has retail's `mov ecx,[count]; movsx edi,cx ... test cx,cx`
// entry (retail scan). On the if/else fraction (249, 880 B): if/else-if clamp 249; clamp inside the nonzero
// arm 249; `short last = count - 1` 273; previous side declared before the previous point 249; loop test
// `i != count` 249; `points + count - 1` 249. Zero-denominator arm first (`== 0.f`) 198 (887 B).
// Round 294: uninitialized current-point/distance/both declarations before the loop
// or at entry, 129 each; both with the current-side declaration at those scopes
// 129/129. Original declaration scopes and body restored; count ECX/prior-side byte remain.
// Round 480: Unified polygon side-presence mask, byte/word/dword/long, branched
// 278/278/278/278 and conditional collection 302/278/278/278. All restored;
// native count ECX, prior-side byte and SIMD homes remain.
// Round 27 (lane 17, harness exact): lead. `if (count > 0)` before the for (repeats the loop's test, match-only)
// gives retail's entry: count once in ECX, both side flags in memory, points in ECX at the loop head (211, aligned
// 36); with the fraction as if/else 92 (890 bytes, aligned 29, constants XMM4/XMM5 as retail). Left on that: zero,
// lateral and denominator in other XMMs (ours zero XMM1 set after the entry distance, retail XMM2 set first), `i = 0`
// after the jump (retail before it), a count reload at the tail. On it: zero arm first 228; clamp in the nonzero arm 96;
// `0 <= distance` for either side and denominator spellings 92; dx before dy 95. Without the guard: `i` before a guarded
// for/while/do-while 129/129/128; if/else fraction 249 (880 bytes); current after the distance through points[i] 249; named
// size 249; `short last` 273; sides in a `bool[2]` 259 (frame +8, element 1 still CL); volatile previous side (diagnostic,
// removed) 246. What it takes is a third read of count near the loop; no natural source for it found.
// Round 507: Complete paired coordinate differences or absolute errors, xy/yx and
// first/neighbor peer priorities, 129/143/147/151 each. Count ECX, prior-side byte and
// SIMD homes remain; original equality helper and complete body restored.
// Round 300: independent positive/negative Boolean accumulators with logical/
// bitwise OR pairs, positive-test bool 288/304/318/315 and byte
// 288/304/318/321. Original conditional collectors restored; count ECX,
// prior-side byte and SIMD homes remain.
// Round 359: Prior signed-distance state with test-assigned fraction, nonzero/zero first,
// native/full-decision clamp and both denominator product parts: 270/270/270/270/255/255/255/255.
// Original state and arithmetic graph restored; count/prior-side and SIMD homes remain.
// Round 362: End-bounded input traversal, crossed with short/native mask ordinal, default/test-assigned fraction and loop-end/pre-append prior side.
// 326/321/310/304/176/176/273/273. Original executable source restored.
// Round 364: Coordinated long/int input-count contracts: direct 283/283; reused signed-word
// count at entry 198/198 or alias join 199/199; entry word-normalized parameter 296/296.
// All restored; count ECX, prior-side byte and SIMD homes remain.
// Round 366: Actual crossing result before the epsilon collectors, bool/byte/short/long
// 247/247/300/299; with prior state advanced at that producer 232/232/311/310.
// Advanced bool/byte state recovers count ECX and both side-byte homes, but adds
// crossing-result work and leaves SIMD homes different. Original complete body restored.
// Actual output cursor at alias join: native/memcpy with rollback 286/286, deferred commit 342/342; at
// entry: rollback 285/285, deferred commit 325/325. Count/prior-side and SIMD homes remain; original
// indexed output body restored.
// M339: Actual current/intersection/last-output element references, subsets 1/2/4/3/5/6/7 then previous
// endpoint: 129/129/129/129/129/129/129/129. Original pointer/output forms and full body restored.
// Round 202 (lane 3, harness exact, confirmed --fast): retail keeps both side flags in memory (current +0xc,
// previous +0xd, the loop end copies through CL); ours keeps the previous side in CL, so count is not loaded once into
// ECX. `long last = count - 1` index 129; `points + count` then `previous--` 129; the result/flags/mask declared after
// the clipped reset 137; the current side as `= false; if (...) = true` 284 (923 bytes); the previous side as an
// if/else 129; a `long original_count` copy for the previous point and the loop 293 (865); a forceinline side helper
// writing the flag through a bool pointer, for both sides 131, previous only 131, loop only 129 (the flags are
// promoted after inlining). Restored.
// DH313: Crossing XOR/current-conditional/prior-conditional/complement predicates, original
// fraction 184/268/279/250; denominator-produced zero 112/255/277/247. NOT kept:
// 112 is a nonmatching predicate/fraction interaction; all exact-only forms restored. Count ECX,
// prior-side byte and SIMD homes remain.
// M339: Actual current/intersection/last-output element references, subsets 1/2/4/3/5/6/7 then previous
// endpoint: 129/129/129/129/129/129/129/129. Original pointer/output forms and full body restored.
// Lane 13 r58 (fast): a `short total = count` copy (as matched 0x23a160) for the previous point and the loop 133;
// for the loop only 129 (same code); the previous side re-derived from `distance >= 0.f` at the loop end 258; the
// previous distance folded into the side's initializer 133; `!(distance < 0.f)` for the side 130.
// B395: Actual NONE result terminates the input walk, short/long ordinal, failure/success
// capacity arms and tail/post-crossing prior-state publication: 282/282/284/284/250/309/257/311.
// NOT kept: all regress; original capacity breaks, indexed walk and complete body restored.
// F364: Native point-owned equality, instance/static and ordinary/forced boundaries,
// original/recovered count-fraction graphs: 129/129/129/129/281/92/281/92. NOT kept: repeated loop
// guard requires a match. Original helper and body restored; count/prior-side and SIMD homes remain.
// B399: Pre-collector crossing with immediate prior-side publication plus four complete
// denominator/fraction/clamp decisions, bool then byte: 239/241/239/241/239/241/239/241. NOT kept:
// every exact-only combination regresses; original state, fraction and complete body restored.
// Count/prior-side and SIMD homes remain.
// B401: Crossing-only prior-side updates, inside/outside polarity, entry/intersection-completion
// and copy/Boolean-transition owners: 163/223/209/219/219/224/214/220. NOT kept: unmatched
// conditional state forms regress; original unconditional tail, complete body and helper restored.
// B403: Indexed-output prospective accepted-count decisions, reject/accept/conditional/negated
// rejection, native short then signed-short-normalized long: 217/255/255/255/238/260/236/260.
// NOT kept: all regress; original incremental count, indexed outputs and complete body restored.
// B416: Entry-distance/crossing-math/current-distance last-consumer regions, seven
// subsets then all with complete geometric role names: 129 each. Original body restored;
// count ECX, prior-side byte and SIMD homes remain.
// B418: Normal pointer/reference, complete coefficient references and plane
// reference before/after alias copying: 131/131/200/130/131/131/200/130. NOT kept:
// no match or gain; original field observations and complete body restored.
// B420: Wrapped predecessor from the real input ordinal, four natural address
// producers with tail then completed-crossing prior-side publication: 326/326/386/333/318/318/381/324.
// NOT kept: no exact match; original complete body restored.
// B424: Standard min/max interpolation clamp, nested lower/upper, sequential lower/upper,
// named lower/upper and single standard lower/upper boundary: 357/357/371/372/371/372/376/383.
// NOT kept: no exact match; original fraction clamp, include set and complete body restored.
// B448: Complete closed-edge write provider, output pointer/reference and plane pointer/reference, forced/ordinary inline:
// 198/198/198/198/945/945/945/945. NOT kept: no byte match; complete original body restored.
// Round 62 (m3 lane 14, fast; each with the `if (count > 0)` guard, match-only, restored): if/else fraction and the
// previous side folded into its initializer 93 (889 bytes, aligned 36); if/else with the fraction declared at function
// scope `= 0.f` 151 (887; zero set first but in XMM3); if/else, folded side, dx at its uses 154 (887, aligned 33, zero
// XMM2 as retail, first difference +0x9e); the same with dy at its uses instead 300 (926). Without the guard: `short i
// = 0; if (count > 0) do ... while (i < count);` 235. Compile-only screen (900 forms, base aligned 81): declaration
// orders of dx/dy/fraction/denominator, statement orders with a fraction default and every product operand order:
// none below aligned 25; retail's x output copies the fraction and its y output multiplies dy in place, and no spelling
// gives that split; `i = 0` before a guard folds the guard away, so the store stays after the `jle`.
// Round 166 (m3 lane 4, one-file LTCG harness exact at 129; no source change): random search, 1500 forms over the loop
// guard, fraction (default, if/else, zero arm first, ternary), clamp (ternary, if/else-if, in the nonzero arm), delta order,
// dx or dy at their uses, x/y output operand orders, output order, denominator terms, fraction declaration, previous and
// current side spellings, tail order and declaration order: no guard best 131 (887); `if (count > 0)` around the for
// 95 (890), around a for with an outer index 98 (890); guarded do-while 102 (891), and 131 (887) where the guard folds.
// B448: Complete closed-edge write provider, output pointer/reference and plane pointer/reference, forced/ordinary inline:
// 198/198/198/198/945/945/945/945. NOT kept: no byte match; complete original body restored.
// F396: Complete scalar coordinate-pair equality input, real/double epsilon, bool/byte result, forced/ordinary inline: 129/129/244/244/129/129/244/244.
// NOT kept: no byte equality; count ECX, prior-side byte and SIMD homes remain. Original complete body and helpers restored.
// MiniMax G (11-18): no new form, no lower score (129): the count's width is still the first difference and the
// parameter's declared type is not ours to change; the pre-loop side still stays in a register byte where retail
// keeps it at F-0xd, the loop's side still takes F-0xd where retail has it at F-0xc, and the zero constant
// still lands in XMM4 against retail's XMM2. This round's forms: the two sides in a two-element holder (the
// loop's side takes a home of its own, 149), the count copied into a local for the previous point and the loop
// test (133), the pre-loop side as a conditional assignment (133) and the entry distance with the x term first
// (129, the same code); all were restored.
// M3 lane 15 r60 (fast): the side carried in one variable, the previous side copied from it at the loop top 288 (922 bytes);
// the side-change test as `previous_inside ? !inside : inside` 279 (915); the epsilon side tests moved to the loop end 264 (877);
// one `short point_count = count` copy at all six uses 129 (same code); the current side declared before the loop 129 (same
// code, as round 294); the `*clipped = false` reset moved just before the loop 221.
// MiniMax C (11-10): no lower score this round (129, was 129). The count's 32-bit form is what
// the first difference is: retail loads the whole home into ECX and narrows it (`mov ecx, dword ptr
// [ebp + 8]; movsx edi, cx`), this build sign-extends the word home directly, and every int-typed use
// of the count folds back into that word load: an int index 129, a long index 129, an int count used
// for the index and the loop test 293. The zero also lands in XMM4 against retail's XMM2 and the
// epsilon in XMM5 against XMM4, which follows from the same allocation.
// MiniMax C (11-10): no lower score this round (129, was 129). Retail's count argument is 32 bits
// wide: it loads the whole home (`mov ecx, dword ptr [ebp + 8]`) and narrows it (`movsx edi, cx`) for the
// previous point's address, while the copy size reads the same home as a word (`movsx ecx, word ptr
// [ebp + 8]`); with this declaration's short count the address reads the word directly, so every
// instruction after the first difference is out of step. Widening the parameter to 32 bits and narrowing
// the copy sizes back to words reproduces retail's frame (0x1014), its flag homes (F-0xe, F-0xf, F-0x10)
// and its count load, but the points argument then takes EAX instead of ESI and the score rises to 163;
// the parameter's declared type is not ours to change.
// MiniMax A (11-05): retail reads the count argument as a dword and narrows it (`mov ecx, [ebp + 8]; movsx edi, cx`) where this declaration's short count is read as a word (`movsx edi, word ptr [ebp + 8]`), so the parameter type, not the body, keeps the rest of the function out of step; no lower score.
// Lane 11 round 80 (fast; al/m = difflib-aligned and register-masked aligned counts, this body al 85 m 20): previous side
// as byte 129 (same code), char 242 (892 bytes), long 266 (893); `previous_inside = inside;` between the two appends (round
// 132's form) 157 with al 47 m 9; that with an if/else fraction 164 (m 13), a ternary fraction 164 (same code), the loop's
// distance reused in the fraction 248 (857 bytes); alone, the if/else or ternary fraction 249 (880), the reused distance 256
// (853); the two loop-end statements swapped 129 (same code); `previous_inside != inside` 129 (m 19), on the moved copy 158;
// on the moved copy five renames of the side locals 157 each (same code: names move no slot here), a byte previous side 157,
// with the if/else fraction 164. The parameter is read as a dword wherever it is enregistered in this body too (`mov ebx,
// [ebp + 8]` at the tail), so the count's declared type is not the cause. All restored.
// Round 224 (m3 lane 3, fast): none of these loads count once into ECX (only the repeated `if (count > 0)` guard
// does). The four flags as members of one local aggregate 198 (scalar-replaced, 887 bytes), with the if/else
// fraction 127 (880); the two side flags as one aggregate 147, with the if/else fraction 259; the fraction as one
// ternary initialiser 249 (880); a named `long size` after the alias copy for both tail copies 129 (same code),
// with the if/else fraction 249; the entry distance read through points[count - 1] with the if/else fraction 256
// (893); `count ? &points[count - 1] : points` for the previous point 293 (903), with the if/else fraction 288;
// `points + count` then a separate `previous--` statement 129 (same code). Guard with the if/else fraction and
// the four-flag aggregate 99 (890).
// Lane 6 r167 (fast, base 129, aligned 85, masked 20: count read from memory twice where retail loads it once into ECX
// and tests `test cx, cx` at the loop entry, so retail's previous side has no byte register and lives at +0xd; ours
// keeps it in CL): `short const count` in the definition 129, `real const previous_distance` 129, the previous side as
// `>= 0.f ? true : false` 129, the crossing's mask bit through SET_FLAG(mask, result, true) 129 (all same code); the
// `*clipped = true` store moved to the end of the crossing block 277 (856 B); the loop end's previous point as
// `&points[i]` 129 (same code); re-measure on this body: the loop test `i != count` 130 (listed at 249 on the if/else
// fraction).
// Round 70 (m3 lane 15, fast): both dedupe tests nested under `if (result != 1)` 129, the alias copy size as
// `sizeof(point2f) * count` 129, the crossing bit as `SET_FLAG(mask, result, true)` 129 (same code); `points = (point2f
// const *)memcpy(copy, ...)` 372 (975 B); re-measured: the deltas through polygon_difference_23a220 129 (round 366's shared
// 2-D difference). No untried natural form targets the cause: the one lever is the count's ECX at the alias join, which only
// the repeated `if (count > 0)` guard gives (match-only, best 92 with the if/else fraction).
// M3 lane 12 r113: no unlisted natural form, no trial: retail holds count in ECX from the alias join to the loop guard
// (`mov ecx, [ebp + 8]; movsx edi, cx ... test cx, cx`), so the previous side gets no byte register (+0xd) and the loop
// head takes ECX for `points`; ours reads count from memory twice and keeps the previous side in CL. Body unchanged.
// Round 80 (m3 lane 13, fast with the file; no caveat): retail's only caller 0x17dd80 pushes the short count raw
// (`mov ecx, dword ptr [esp + 0xa0]; push ecx`, no movsx) and passes the output in EDX, so the parameter is retail's
// short and a 32-bit count would also change that contributor's caller. The inside arm's copy written per member 129,
// `previous = &points[i]` at the loop end 129 (same code each); the crossing point built in a point2f initializer and
// stored whole 255 (899 bytes). Body unchanged at 129.
// MiniMax H (11-18): no new form, no lower score (129): what is left is allocation only - retail reads the count as
// a dword into ECX and keeps it there (`mov ecx, dword ptr [ebp + 8]`, `movsx edi, cx`, `test cx, cx` at the loop
// entry) while this build reads it as a word and keeps the previous side in CL where retail has it in a byte home at
// +0xd, and the zero constant is in XMM4 here against XMM2 in retail; the count's width is fixed by declarations
// outside this lane's files, the only lever for it (a repeated `if (count > 0)` guard) is match-only, and the
// recorded way to spill the previous side (moving its assignment to just before the inside test, which does give
// retail's count in ECX) merges the alias copy into the inside test at 157, so the body stands.
// MiniMax G (11-14): no new form, no lower score (129): the count's width is still the first difference - retail reads
// the argument as a dword home (`mov ecx, dword ptr [ebp + 8]; movsx edi, cx`) where this declaration's `short count`
// is read as a word home - and that width is declared by the caller in src/unknown_17d9a0.cpp, so it cannot be changed
// from this lane; the permuter gain recorded in the lines above needed `volatile` reads, so it stays discarded.
// b8H (11-17) walk against retail, fast build on r836: no logic or structure difference found, 129 unchanged.
// Walked 0x23a6f0-0x23aa65 block by block (ours does the same work on the same side of every jcc, NaN included):
// *clipped cleared 0x23a716-0x23a718; alias copy of count * 8 bytes into the 0x1014 frame 0x23a71d-0x23a741; previous
// point &points[count - 1] and its side (distance >= 0, jae 0x23a784) 0x23a743-0x23a786; loop guard 0x23a78b-0x23a796;
// current side (jae 0x23a7e0), positive (jbe 0x23a7ea) and negative (0 - eps > d, jbe 0x23a7fd) tests; crossing
// 0x23a812-0x23a91a (capacity test on the result word, deltas previous - current, denominator ni*dx + dy*nj, divide
// skipped only for == 0 by ucomiss/test ah,0x44/jnp 0x23a85a, fraction 0 - d/den with the lower then the upper clamp
// against 1.0 at 0x45dbc0, outputs fraction*d + current, mask |= 1 << result, then the dedupe against the first point
// and result - 2); inside arm 0x23a91b-0x23a9d9 (mask bit from *point_mask when non-null, else cleared); tail
// 0x23a9f9-0x23aa65 (NONE -> raw copy, < 3 -> 0, no positive -> 0, no negative -> count and raw copy, *point_mask =
// mask). The difference left is allocation only: previous side in CL against [esp+0xd], zero in XMM4 against XMM2.
// M3 lane 1 r118 (fast, the linked build; control 129, aligned 85, register-masked 20): the three plane distances
// (previous point, loop, fraction numerator) through one new forced-inline distance helper, in either x-product order,
// or the first two only 129/129/129 (same code); the two outputs as `fraction * dx` / `dy * fraction` in the three other
// operand-order pairs 129 each (same code: the products are canonicalised in the linked build too); the fraction as an
// if/else with the clamp inside (retail's shared zero block) 249 (880 B; aligned 49, masked 21), the same with each of
// the three other output operand-order pairs 249 each (same code). Retail's split (the x output from a copy of the
// fraction times dx, the y output as dy times the fraction in dy's own register, and `dy` copied before its product
// with the normal's j) puts dy first in both of its products; no spelling here moves dy's rank. Body unchanged.
// 463: crossing normal components shared by denominator and numerator, before/after deltas
// and in x/y or y/x order, 263 each; with named numerator products 270 each. NOT kept:
// longer crossing code leaves the count load and prior-side home different.
// Round 182 (m3 lane 7, fast; body unchanged at 129): with every register masked the kept body is 28 aligned lines
// from retail (the count load, the previous side's home, the crossing's product operands). Retail holds count in ECX
// only from the alias join to the entry guard (the loop's bottom test and `result = count` read its home), so it is the
// stack parameter enregistered over that stretch, which takes ECX before the previous side is defined. Distance from
// `points[i].y` / `points[i].x` with `current` named after it 129 (same code: this build already loads indexed and
// forms the pointer afterwards, from EDI where retail uses ECX); the entry distance from `points[count - 1]` at both
// reads 268 (896 B, the stride goes to ECX and the previous side is reloaded into CL ahead of the loop); the side test
// as `inside ^ previous_inside` 184. Re-measured on this tree, guard forms (match-only): `if (count > 0)` around the
// loop 211 (893 B, aligned 38); with the fraction as if/else and the clamp inside 96 (890 B, aligned 35), the same with
// the denominator's terms swapped 96 (same code); with dx written at its two uses 151 (887 B, aligned 30, first
// difference +0x9e: zero XMM2, entry and loop head as retail); on that, the clamp as if/else-if 151 and the x output
// as `fraction * (...)` 151 (same code each), the clamp as if/else-if after the if/else 147 (aligned 27). Left on that
// form: `i = 0` stored after the guard's branch (retail before it), dy/dx/denominator in XMM3/XMM1/XMM0 against
// retail's XMM0/XMM3/XMM1 with dy first in both its products, retail's zero-fraction arm sitting between the two
// clamp tests, and count hoisted into ECX for `result = count` where retail reads its home.
// M3 lane 12 r148 (fast with 0x239e50/0x239d50/0x239b80; control 129). Reading: retail keeps both side bytes in memory
// (inside +0xc, previous +0xd), so the count it loads into ECX at the alias join is still there for the loop guard; ours
// keeps the previous side in CL and so reads the count twice. Tried: the previous side computed straight from the
// distance expression (no previous_distance local) 133; the side test as `previous_inside != inside` 129; the previous
// side updated before the previous point at the loop end 129; `inside` declared once ahead of the loop 129; the count
// named once as `short total = count;` for every use (0x23a160's spelling) 129 (same code each). Body unchanged.
// M3 lane 3 r274 (fast with 0x239e50; control 129): the previous side declared, then assigned 129; recomputed from the
// distance at the loop end 258; the crossing written through a pointer to its output element 129; the previous point as
// `points + count - 1` with the inside copy written per member 129; the epsilon test as `epsilon < distance` 129.
// Diagnostic (reverted): a volatile previous side 149 (the frame moves, the entry is not retail's).
// M3 lane 8 r202 (fast; control 129, aligned 89, register-masked 28): on the repeated-guard graph (match-only), re-measured guard +
// if/else fraction 92 (890 B, aligned 36: the zero made late in XMM1, `i = 0` after the guard's branch, the crossing's dy
// copy, the tail's count in ECX) and guard + clamp inside + dx at its uses 151; new on the 92 form: `inside` after the epsilon
// tests 101, the entry side as an if/else assignment 92 and `fraction` declared ahead of the deltas 92 (same code each), the
// guard spelled `0 < count` 251 (880 B) and `count >= 1` 271 (906 B).
// Ordinal before point/side, between them, before side/point, or between
// side/point 133/132/133/130. Publishing the side in each input-acceptance
// branch, before or after the point copy, after the mask update, or after duplicate
// suppression 216/216/166/135. Early publication recovers count in ECX, swaps
// the side slots and grows the body to 895 bytes. Late publication leaves the side in CL.
// M3 lane 6 r214 (fast with 0x239e50; control 129, aligned 81, masked 16). Retail keeps both side bytes in memory (the
// copy at the loop end goes through CL into +0xd), so the count holds ECX from the top; ours keeps the previous side in
// CL. The listed copy moved ahead of `if (inside)` re-measured 157 (masked 9: left the two side-byte slots swapped, the
// copy's place, the XMM renaming); on it the previous side renamed to sort between the two others, the current side
// renamed to sort last, and both renamed: 157 each (same code: the byte slots do not follow names); the current side
// renamed on main 129 (same code); the crossing result named ahead of the copy (`bool crossed = inside !=
// previous_inside; previous_inside = inside; if (crossed)`) 226 (894 B). Body unchanged.
// MiniMax E (11-20): score 129 (was 129): no new form and no lower score; the first difference is the count's load width -
// retail reads the count slot as a dword (`mov ecx, dword ptr [ebp + 8]`) and takes its low word (`movsx edi, cx`), this
// build reads a word directly (`movsx edi, word ptr [ebp + 8]`). Retail's dword load means its count parameter is 32-bit,
// and the declaration may not change here. Behind that: retail keeps the previous-side flag in memory at [esp + 0xd] where
// this build keeps it in CL, and retail's zero is XMM2 where this build uses XMM4.
// M3 lane 12 r171 (H2M fast; control 129): the loop test as `count > i` 130; the crossing deltas declared dx then dy 129;
// the denominator's terms swapped (`dy * n.j + n.i * dx`) 129; the alias test as `output == points` 130 (first +0x2e); the
// crossing's mask update ahead of the two output stores 131. Re-measured: the copy moved ahead of `if (inside)` 157 (count in
// ECX and retail's loop head; retail's copy is at the loop end through CL, so the moved copy is not retail's statement order:
// at the loop head retail holds `points` in ECX, which is what leaves the previous side without a byte register). Body unchanged.
// M3 lane 2 r141 (h2m fast with 0x239e50; control 129): both dedupe tests as `result > 1` 131; the dedupe through a named
// `last` output pointer (`last - 1` for the one before) 269 (891 B); the y output as `current->y + fraction * dy` 129 (same
// code); the kept-point bit as `point_mask && TEST_FLAG(...)` 252 (880 B); `result = 0` set just ahead of the loop 255.
// Reading: the count's dword load is a cross-block CSE of `count` that only appears when the guard is in the source (the
// rotated loop test is formed after CSE here), which is why the repeated `count > 0` guard reproduces it. Body unchanged.
// M3 lane 8 r234 (H2M_FAST with 0x239e50, 0x239d50 and 0x239b80; control 129, aligned 85, masked 20; body unchanged). Allocator
// reading: the count's candidate over the entry, the loop guard and the tail has three references and a weighted cost of exactly
// 0, so it is not coloured; the previous side (cost 12, priority 10) then takes ECX. One more reference of the count in that
// range makes its cost positive, it takes ECX first and the previous side has no byte register left, which is retail's entry
// (and why the repeated `count > 0` guard reproduces it). A forced decision that leaves the previous side uncoloured gives the
// closest structure (209 positional, 5 structural rows from 19); changing either candidate's priority alone changes nothing.
// Forms: the previous side started true and cleared under `if (!(previous_distance >= 0.f))` 132; started false and set 130;
// as `previous_distance >= 0.f ? true : false` 129 and `!(previous_distance < 0.f)` 129 (same code each); one `inside` at
// function scope set at entry and copied into the previous side 200.
// M3 lane 5 r245 (h2m fast with 0x239e50, 0x239d50 and 0x239b80; control 129, sequence-aligned 85 rows exact, 20 with
// registers and stack offsets masked; body unchanged), each looking for a natural extra use of the count between the
// entry and the loop's zero-trip test: the previous point's distance read through `points[count - 1]` 268 (896 B); the
// alias copy reading from `output` 205; `sizeof(point2f) * count` 129 (same code); the previous distance's second
// product written `plane->n.i * previous->x` as in the loop 129 (same code); `points + count - 1` 129 (same code).
// M3 lane 14 r155 (H2M_FAST with 0x239e50, 0x239d50 and 0x239b80; control 129; body unchanged). Allocator reading: the
// count's piece from the alias join to the tail (references at the join, the loop guard and `result = count`) has a
// weighted cost of exactly 0, so it is skipped and the previous side takes ECX; main's tail is retail's one instruction
// late. In the `!negative` arm the copy written before `result = count` 207 (891 B) but aligned 85 -> 34: the count is
// then in ECX at the join and both side bytes stay in memory as retail (statement order, match-only). LEAD (match-only,
// needs that order): with it, the fraction as `if (denominator != 0.f) fraction = ...; else fraction = 0.f;` (retail's
// zero block shared with the low clamp) 42 at 888 B, aligned 28; left the zero in XMM1 made late (retail XMM2 at the
// join), the crossing's dy and fraction in XMM2/XMM0 (retail XMM0/XMM1: the allocator ranks the fraction, 148, above dy,
// 50) and one extra `movaps`. On that 42 form: the clamp inside the if 46, the denominator's terms swapped 42 (same code),
// the deltas declared dx first 45, `real fraction;` ahead of the deltas 42 (same code); the reordered tail with
// `fraction = 0.f` ahead of the deltas 49. Without the reorder: the if/else fraction 249 (880 B); the tail copy sized
// from the result 129 (same code); the NONE copy as the first arm 129.
// M3 lane 5 r252 (H2M_FAST with 0x239e50, 0x239d50 and 0x239b80; control 129). Re-built lane 14 r155's LEAD (the tail
// copy ahead of `result = count`, the fraction as if/else): 42 at 888 B. Allocator reading of it: the float values
// colour fraction (148) > denominator (92) > distance (76) > zero (54) > dy (50) = dx (50), so the zero takes XMM1
// and dy XMM2 where retail has dy XMM0, fraction XMM1, zero XMM2. A forced-ranking search: dy ranked first 13 (left:
// the dy * n.j and dy * fraction operand orders, which make ours one byte long, and the plane reload ahead of the
// previous y load); dy one step above the zero 22; the zero one step below dy 29; lowering the fraction or the
// denominator alone changes nothing. On the lead: the clamp into a separate `clamped` local 42, the clamp as
// if/else-if 42, the outputs as `fraction * dx` / `fraction * dy` 42, the deltas declared at function scope, at the
// top of the crossing arm, or dy alone at function scope 42 each, `real fraction;` at function scope 42 (same code
// each: neither declarations nor spelling move these ranks); the deltas through the file's point-difference helper
// 45 and through vector2d_from_points2d 45. Body unchanged.
// M3 lane 15 r154 (H2M_FAST with 0x239e50, 0x239d50 and 0x239b80; control 129; on the 42 lead, 888 B): the allocator's
// float ledger on the lead: the fraction 148 (its divide block 36, five clamp blocks 8 each, the two output products 72),
// the denominator 92, the zero 54, dy and dx 50 each (their block 64, the output 12, -26 over the six clamp blocks
// where they are live and unread); retail's colours need dy above all three, about 100 more, which no spelling of the
// crossing gives. Forms on the lead: the y output written first 47; the denominator written `dx * n.i + dy * n.j` 42,
// `dy * n.j + dx * n.i` 42; the outputs as `fraction * dx + x` 42 and `x + dx * fraction` 42 (same code each). Body
// unchanged.
// @retail 0x23a6f0
short function_23a6f0(point2f *output, short count, point2f const *points,
	plane2f const *plane, short capacity, dword *point_mask, bool *clipped, real epsilon)
{
	point2f copy[512];
	short result = 0;
	bool positive = false;
	bool negative = false;
	dword mask = 0;
	if (clipped)
		*clipped = false;
	if (points == output)
	{
		memcpy(copy, points, count * sizeof(point2f));
		points = copy;
	}
	point2f const *previous = &points[count - 1];
	real previous_distance = previous->y * plane->n.j + previous->x * plane->n.i - plane->d;
	bool previous_inside = previous_distance >= 0.f;
	for (short i = 0; i < count; i++)
	{
		point2f const *current = &points[i];
		real distance = current->y * plane->n.j + plane->n.i * current->x - plane->d;
		bool inside = distance >= 0.f;
		if (distance > epsilon)
			positive = true;
		else if (0.f - epsilon > distance)
			negative = true;
		if (inside != previous_inside)
		{
			if (result == capacity)
			{
				result = NONE;
				break;
			}
			if (clipped)
				*clipped = true;
			real dy = previous->y - current->y;
			real dx = previous->x - current->x;
			real denominator = plane->n.i * dx + dy * plane->n.j;
			real fraction = 0.f;
			if (denominator != 0.f)
				fraction = 0.f - (current->y * plane->n.j + plane->n.i * current->x - plane->d) / denominator;
			fraction = fraction < 0.f ? 0.f : fraction > 1.f ? 1.f : fraction;
			output[result].x = dx * fraction + current->x;
			output[result].y = dy * fraction + current->y;
			mask |= FLAG(result);
			result++;
			if (result != 1 && (clip_points_equal_23a6f0(&output[result - 1], output, epsilon) ||
				clip_points_equal_23a6f0(&output[result - 1], &output[result - 2], epsilon)))
				result--;
		}
		if (inside)
		{
			if (result == capacity)
			{
				result = NONE;
				break;
			}
			output[result] = *current;
			SET_FLAG(mask, result, point_mask ? TEST_FLAG(*point_mask, i) : false);
			result++;
			if (result != 1 && (clip_points_equal_23a6f0(&output[result - 1], output, epsilon) ||
				clip_points_equal_23a6f0(&output[result - 1], &output[result - 2], epsilon)))
				result--;
		}
		previous = current;
		previous_inside = inside;
	}
	if (result != NONE)
	{
		if (result < 3)
			result = 0;
		if (!positive)
			result = 0;
		else if (!negative)
		{
			result = count;
			memcpy(output, points, count * sizeof(point2f));
		}
	}
	else
		memcpy(output, points, count * sizeof(point2f));
	if (point_mask)
		*point_mask = mask;
	return result;
}

// @retail 0x23a2b0
short function_23a2b0(short count, point2f const *points, short clip_count,
    point2f const *clip, short capacity, point2f *output, real epsilon)
{
    point2f buffers[2][512];
    for (short i = 0; i < clip_count && count > 0; i++)
    {
        short previous = i ? i - 1 : clip_count - 1;
        point2f *destination = i == clip_count - 1 ? output : buffers[i & 1];
        plane2f plane;
        plane.n.i = clip[previous].y - clip[i].y;
        plane.n.j = clip[i].x - clip[previous].x;
        real length = (real)sqrt(plane.n.i * plane.n.i + plane.n.j * plane.n.j);
        if (!(fabs(length) < 0.0001f))
        {
            real inverse = 1.f / length;
            plane.n.i *= inverse;
            plane.n.j *= inverse;
        }
        else
            length = 0.f;
        if (length != 0.f)
        {
            plane.d = plane.n.i * clip[i].x + plane.n.j * clip[i].y;
            count = function_23a430(count, points, &plane, epsilon, capacity, destination);
            if (count == NONE)
                return NONE;
        }
        else
        {
            plane.d = 0.f;
            memcpy(destination, points, count * sizeof(point2f));
        }
        points = destination;
    }
    return count;
}
