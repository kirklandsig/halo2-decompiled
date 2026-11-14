#include "unknown_11c920.h"
#include <string.h>
#include <xmmintrin.h>
#include "globals.h"
#include "data_array.h"
#include "physical_memory.h"

// @flags /O2 /arch:SSE /Gr

/* the physical memory allocator (physical_memory.h): a named, doubly linked
   list of blocks kept in a data array that follows the object */

// @retail 0x13d170
void function_13d170(s_physical_object *manager, const char *name, long page_count, long page_shift, long maximum_count, physical_block_delete_proc delete_proc, physical_block_busy_proc busy_proc, physical_block_state_proc state_proc, c_data_allocator *allocator)
{
	s_record_pool *data = (s_record_pool *)(manager + 1);

	function_16b5f0(data, name, maximum_count, sizeof(s_physical_block), 0, g_46875c);
	data->valid = 1;
	record_pool_release_all(data);

	memset(manager, 0, sizeof(s_physical_object));
	strncpy(manager->name, name, 0x20);
	manager->name[0x1f] = 0;
	manager->delete_proc = delete_proc;
	manager->busy_proc = busy_proc;
	manager->state_proc = state_proc;
	manager->page_count = page_count;
	manager->page_shift = page_shift;
	manager->first = NONE;
	manager->blocks = data;
	manager->time = 1;
	manager->signature = 0x77656565;
	manager->state = 0;
	manager->allocator = allocator;
	manager->last = NONE;
	for (long i = 0; i < 8; i++)
		manager->limits[i] = 0x7fffffff;
}

// @retail 0x13d230
void physical_memory_reset_time(s_physical_object *physical)
{
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = physical->blocks;
	physical->time = 0x101;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		dword age = 0x7fffffff - block->time;

		if (age > 0x100)
		{
			block->time = 0;
		}
		else
		{
			block->time = 0x100 - age;
		}
	}
}

// @retail 0x13d2b0
void physical_memory_flush(s_physical_object *physical)
{
	s_record_pool_iterator iterator;

	iterator.data = physical->blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		physical->block_delete(iterator.datum_index);
	}
}

/* frees a block: the owner's delete callback, then unlinks it */
// @retail 0x13d830
void s_physical_object::block_delete(long handle)
{
	s_physical_block *entry = &((s_physical_block *)blocks->data)[handle & 0xffff];

	if (delete_proc)
		delete_proc(handle);

	if (entry->previous != NONE)
		((s_physical_block *)blocks->data)[entry->previous & 0xffff].next = entry->next;
	else
		first = entry->next;

	if (entry->next != NONE)
		((s_physical_block *)blocks->data)[entry->next & 0xffff].previous = entry->previous;
	else
		last = entry->previous;

	record_pool_release(blocks, handle);
}

PRIVATE __forceinline byte *function_13d8b1(s_record_pool_iterator *arg_1)
{
	s_record_pool *local_1 = arg_1->data;
	long local_2 = data_find_index(local_1, arg_1->index + 1);
	byte *local_3;
	if (local_2 != NONE)
	{
		local_3 = local_1->data + local_1->size * local_2;
		arg_1->index = local_2;
		arg_1->datum_index = (*(short *)local_3 << 16) | local_2;
	}
	else
	{
		arg_1->index = local_1->maximum_count;
		arg_1->datum_index = NONE;
		local_3 = 0;
	}
	return local_3;
}

/* resizes the allocator to a number of pages, freeing the blocks past its end */
// @retail 0x13d8b0
void s_physical_object::method_13d8b0(long pages)
{
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)function_13d8b1(&iterator)) != NULL)
	{
		if (block->offset + block->pages > pages)
		{
			block_delete(iterator.datum_index);
		}
	}
	page_count = pages;
}

// @retail 0x13d950
long physical_memory_used_pages(s_physical_object *physical, long age)
{
	long result = 0;
	s_record_pool_iterator iterator;
	s_physical_block *block;

	iterator.data = physical->blocks;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((block = (s_physical_block *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if ((dword)(block->time + age) >= (dword)physical->time || physical->busy_proc && physical->busy_proc(iterator.datum_index))
		{
			result += block->pages;
		}
	}
	return result;
}

struct s_physical_block_view;
bool physical_memory_candidate_is_better(s_physical_object const *physical, s_physical_block_view const *a, s_physical_block_view const *b);

struct s_block_candidate
{
	long previous;
	dword time;
	long offset;
	long pages;
};

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* a walk over the blocks, the current one ahead of the iterator */
struct s_physical_block_iterator
{
	s_physical_block *block;
	s_record_pool_iterator iterator;
};

// score 201 (was 201; round 95 m3 lane 11 near the end, m3 lane 2 r79 and m3 lane 8 r164 at the end): earlier 209, 347; case 0 is jump-threaded into the take test (retail materialises `xor al, al ... mov al, 1`
// MiniMax A (11-13): no new form, no lower score (201); the take test still jump-threads the case blocks past the
// loop and the second age's clamp still folds away, and the only form that reaches 98 needs a policy helper with one caller.
// Earlier 201 (was 201): earlier 209, 347; case 0 is jump-threaded into the take test (retail materialises `xor al, al ... mov al, 1`
// and falls into the join), the second age's clamp folds, the walk's EBX/EDI roles and the link order differ.
// M3 lane 8 r159 (fast, 0x13d320 MATCH each time; base 201, masked 56): the first candidate taken in an `if (!have_best)` arm
// with the switch and `if (better) best = *candidate;` in the else arm 224 (1216 bytes; case 0 still threaded); the block
// arm's offset advanced as `offset += block->pages` 368 (1197 bytes). Re-measured on the 201 base (forms listed below): the
// deletion walk on plain `s_record_pool_iterator iterator; s_physical_block *block;` locals, the pre-round-215 form, 212
// (first difference +0x1; the grouping aggregate is worth 11 here), with block declared first 212 (same code); the link
// section through the pool at each use (element macro, one best.previous test) 236 (1250 bytes); with the predecessor's
// successor in `long next` 223 (1239, masked 47); with an `after` pointer and a fresh predecessor pointer after the arms
// 214 (1228). No further natural form found that this note does not already list. All restored.
// MiniMax D (11-11): no new form, no lower score (201); the first difference is still the jump to retail's
// 0x13d679, and the code up to it matches retail instruction for instruction. What differs is the policy block
// behind it: retail's default arm clamps both ages with `cmp; sbb; and`, multiplies the best's pages by the
// candidate's age and the candidate's pages by the best's age, and keeps its two case blocks in place between
// that arm and the merge, while ours folds the second clamp away (`MAX(0, ...)` never holds for an unsigned
// difference), makes the two products in the other order and jump-threads case 0 and case 1 into the take test,
// so they sit past the loop. The documented forms for the clamp, the case 0 shapes and the take test were
// measured before and are worse on their own (209 to 284); the 93 form needs the policy helper, which is match-only.
// Status line before r159, 201 (was 201): earlier 201 (was 347): frame 0x1054 and entry/ring roles agree; weighted-age mask registers, detached policy cases and final linking differ.
// Match-only recipe status, 93 (was 201): the policy switch, the ring scan's successor and the block insertion now follow retail's shape; the walk's EBX/EDI roles, the overlap sum's register and the NONE register still differ.
// MiniMax E (11-11): 201 -> 93 (1213 bytes, retail's length, first difference +0x338): the case 0 policy
// goes through a small helper in 0x13d320's style, the second age is spelled MAX(age, 0), the before
// block is read again through a fresh pool base with its successor in a local, the insertion is in two
// stages with the first-block test repeated, and the scan writes its successor at both uses.
// MiniMax E (11-09): 209 -> 201 (1214 bytes against retail's 1213): the second age is now spelled
// MAX(0, physical->time - best.time), which for the unsigned difference is the same value as the
// clamped one, so our mask for it and the multiply that follows it go away. What still differs: the
// first age's difference rides in EDX where retail keeps it in EAX, ours keeps no mask for the second
// age where retail's does, our two products are made in the other order, and the switch's two case
// blocks sit at the end of our function where retail has them in place between the default arm and the
// merge. Measured this round: the mask for the second age restored is 209, the reversed comparison and
// the mask-free first age are 225, and a local for the clock, for the candidate's time and for the
// best's time all stay at 201.
// MiniMax G (11-06): no new form, no lower score: the first differing code is the MAX() clamp in the comparison
// block, where ours keeps the clamped value in ECX and the sign mask in EAX and retail the other way round, and
// our body is 15 bytes longer (1228 against 1213), the first divergent branch being the jump to retail's 0x13d679.
// MiniMax G (11-05): swapping the offset and previous declarations is the same code (347); retail keeps the offset in EBX and the current in EDI, ours the other way round, and the four-byte frame difference and the segment-arm layout stay unmatched; no new form, no lower score.
// MiniMax S (11-01): the result/limit declaration swap reaches retail's frame but scores worse; no lower score.
// Round 32: retail lays the segment arms out in source order (none, block, gap, then the scan), but
// ours moves the else arm past the scan; retail also uses the page offset (EBX) as its zero from
// the limit clamp on, so EBX is pushed first. Candidate stores offset before time 370; busy as
// `proc && proc(current)` 376; offset declared with the result at entry 400 (EBX/ESI prologue as
// retail, offset in EDI); arms inverted (`current != NONE` first) 367 at 1213 bytes (three-point
// gain dropped; the none arm then moves past the scan); inverted with early offset 382.
// tail and keeps page offset in EBX. Limit-first guard 385; oldest timestamp
// initialised 381; enclosing inner-next scope 370; short ring endpoints 392;
// candidate copy through memcpy 370.
// Bounded ring masks 387; unsigned-word endpoints 392; frame pointer 475;
// zero inline depth 418; size preference 643 (fast).
// Native busy result 370; native selection result 373; gap endpoint snapshot 370;
// page cursor published first 372; byte ring tail 432 (fast).
// Round 210: Word best-present/busy/policy states 372/373/374; byte ring head 393; function-
// scope deletion iterator 370. Frame 0x1050 versus 0x1054 and page/tail allocation remain.
// Round 212: Shared page shift 370; byte limit category 418; unsigned-word inner/insertion-next/successor cursors 374/375/383. Frame and page/tail homes remain.
// Frame 0x1050 versus 0x1054; retail spills the ring tail and keeps page offset in EBX.
// Limit-first guard 385; oldest timestamp initialised 381; enclosing inner-next scope 370; short
// ring endpoints 392; candidate copy through memcpy 370. Bounded ring masks 387; unsigned-word
// endpoints 392; frame pointer 475; zero inline depth 418; size preference 643 (fast). Native busy
// result 370; native selection result 373; gap endpoint snapshot 370; page cursor published first
// 372; byte ring tail 432 (fast). Tail reset 375; candidate while 370; short inner cursor 374;
// timestamp maximum 367 (three-point gain dropped); word limit index 386; maximum plus tail reset
// 373; maximum plus word limit 381; maximum plus all ring resets 368 (fast).
// Round 214: byte insertion successor 375; byte inner successor 383; byte inner ordinal 389; byte best-present state 370; byte policy state 370.
// Native pointer ring: indexed-source-equivalent for/while, mutable/immutable successor and branch/function
// array lifetimes 382/382/380/380/382/382/380/380. Original body and helpers restored.
// Negative best-candidate bool/byte/short/long in admitted scope 371/371/373/373;
// the same carriers from entry 379/379/380/380. Original positive best state and body restored.
// Cursor declaration orders 102435/032145/042315/052341 382/370/369/377
// (head, tail, offset, previous, current, oldest); four combined cursor names 370 each.
// Round 176: Unsigned previous/current/oldest, every pair/all three, and all
// three plus unsigned allocation result: 370 each (eight forms). Equality-first
// time policy bool/byte/short/long 385/385/381/383; non-strict admission
// 370/370/374/373. Frame/page-tail homes persist; all original code restored.
// Round 235: Complementary availability bool/byte/short/long at segment entry
// 376/376/387/385; constant default before ring insertion 385/385/399/397.
// Frame and page/ring allocation remain; original busy state and complete body restored.
// Round 237: Negative policy comparison bool/byte/short/long inside admission
// 376/376/374/375; defaults before admission 376/376/374/375. Original policy state restored.
// Round 194: Occupied-first short-circuit callback with initial/segment-owned defaults
// 379/379; conditional callback 366/366. Comparator inline/noinline on original
// 370/370, on occupied-first conditional form 366/366. Original-order callback
// forms 376/376/371/371. Four-point gain at retail length rejected; original code restored.
// Round 148: insertion candidate reference 370; scanned candidate reference 370; allocated block
// reference 370; both candidate references 370; insertion/allocated references 370;
// scan/allocated references 370; all object references 370; all references/readonly block/policy
// inline 370; isolated policy inline 370. All executable forms restored.
// Round 226: Native inserted candidate initializer 387; zero-initialized best 398; both 398;
// native deletion iterator initializer 370; insertion/iterator 387; best/iterator 398;
// all three 398; all with entry best initialization 417. Frame/page-tail homes remain.
// Round 162: Coordinated policy parameter orders 021/102/120/201/210 370 each; individual
// physical/candidate/best mutable-pointer input declarations 370 each. Policy body and all matched
// controls stay exact; original declarations and call restored. Frame 0x1050 versus 0x1054 and
// page/ring homes remain.
// Round 111: arms inverted 367 at 1213 B again (retail keeps the none arm first, so only a length
// coincidence); current and offset initialised at entry 399; 61 orders of the six cursor
// declarations (fast) 369-383; default case first 370 (our case 0/1 bodies stay at the function's
// end, retail keeps them in the loop); case 1 as `if (...) better = true;` 382. Retail keeps the
// offset in EBX through the scan and spills `next` (+0x4c); ours keeps `next` and the tail.
// DL190: Existing calling iterator with fields/aggregate 424/424; direct step 418/418; native datum iterator 409/404 (frame 0x1054, but out-of-line). Allocated lookup/absolute getter 367/367 at retail length (three-point gains rejected). Original iteration, allocation and body restored; page/ring carriers remain.
// Round 228: earlier head retirement/tail commitment/both 369/370/369; too-small
// candidate continue alone/with head/with tail/with both 370/381/370/386; all plus timestamp
// maximum 367 at retail length (three-point lead rejected). Original body restored.
// Round 237: Computed page-size long/dword, each with named-mask expression, original/entry constant result lifetime: 370/370/370/370/413/413/413/413.
// Original executable body restored.
// Round 27 (lane 14), aligned counts in brackets (control 370 [355]): busy handled at the end of the block arm
// (`if (busy) { head = tail; continue; }`, scan unconditional) 377 [252] at retail length 1213, retail's
// none/block/gap arm order; with busy = proc && proc(current) 377 [239]; that with the policy cases in
// default/1/0 source order 377 [239] (same code); with offset = 0 declared at entry 380 [257] (EBX pushed
// first as retail); with the allocation success returning directly 381 [244]. On the 239 form, retail still
// lays out the no-candidate limit update + return right after the loop guard and keeps the case 0/1 bodies in
// the loop (ours puts both at the end), and uses its zero EBX as offset (frame 0x1050 versus 0x1054).
// Round 242: Actual queued-candidate count long/dword/short/word, capacity-bound and
// not-full admission 407/407/406/418/407/407/406/418. All recover frame 0x1054 but
// grow code; original ring state and body restored, page-offset/tail homes remain.
// Round 8 (lane 17): the segment arms as an else-if chain 370 (same code); busy as a ternary on the callback 371;
// segment pages/time declared before the while 370; `if (!busy) scan else head = tail` 397 (1181 bytes); busy
// assigned in each arm without an initializer 369.
// Round 49 (m3 lane 1, fast): lane 14 round 27's busy form rebuilt (busy decided in the in-block arm,
// `if (busy) { head = tail; continue; }`, the scan unconditional) 377 at 1213 bytes (aligned 355 -> 240);
// with busy as `proc && proc(current)` 377 (aligned 238); on that: the new candidate's fields stored
// previous/offset/time/pages (retail's store order) 377 (on the original 370, same code), segment pages/
// time declared before the while 377, the scan as `for (...; i = next)` 377 (all aligned 238). Left there:
// retail holds offset in EBX from entry (its zero for the type clamp, so EBX is pushed first and `next` is
// spilled at +0x4c: frame 0x1054), stores the zero segment time in each arm and keeps current in EDI.
// Round 258: Native policy if/else dispatch 0/1/default and 1/0/default, each with
// bool/byte/short/long results, 385/385/384/381/400/400/396/394. Original switch, result domain and body
// restored; frame 0x1050 versus 0x1054 and page/ring homes remain.
// Round 35 (m3 lane 2, fast): lane 14's busy-in-arm form (`if (busy) { head = tail; continue; }` in the block
// arm) rebuilt 377 at 1213 bytes; on it, the allocation as a single exit (`if (result != NONE) link; else if (type
// == limit_index) clamp;`, no goto) 377 (same code; 370 on the original, same code), with the clamp arm first
// (`if (result == NONE)`) 393 (1197 bytes); on the original: the new candidate's fields written through
// `candidates[tail]` 371, the scan's candidate read through `candidates[i]` 397 (1244 bytes), the loop test as
// `page_count > offset` 370 (same code). Retail places the zero-trip limit update right after the loop guard
// (`jg` into the body, the update falling through); every form here keeps it at the end.
// Round 111 (lane 7, fast; the one-file harness does not reproduce this function: 1162 bytes): round 27's
// busy-continue form 377 (1213 bytes); with the none/gap arms storing the zero time first 377; with the
// success path returning directly (no goto) 381; with the outer test as an early `return result` 391 (1228).
// On the original: zero time first 370 (same code); success returning directly 376; early return 398 (1232).
// Retail lays the no-candidate tail (limit update, result reload, `pop edi`) and the shared epilogue right
// after the loop guard (`jg loop`, falling into the tail); allocation failure and the post-loop !have_best
// test jump back to it. Ours places the tail last and enters the loop through `jle tail; jmp body`.
// Round 131 (m3 lane 6, fast): `long offset = 0;` as the first statement 373 (push/pop order and the pages
// register become retail's, the zero moves to EDI, frame still 0x1050); offset declared after the result,
// before the clamp 400 (1231 bytes); the clamp as an if/else-if chain 370 (same code); both 373; offset
// first with the candidate stored previous/offset/time/pages 372.
// Round 275: Reused candidate pages/time/both/best fields/pages-best/time-best/all 369/367/367/370/367/367/367; all with immutable successor 367. One/three-point leads rejected.
// Original executable source restored; frame 0x1050 versus 0x1054, page/ring homes and policy blocks remain.
// Round 277: Existing private iterator with field/aggregate initialization and candidate
// assignment/memcpy 370 each; each with native successful-datum lookup 367 at 1213 bytes
// (three-point gains rejected). Original helper use, iterator, lookups and body restored;
// frame/page-tail homes and policy blocks remain.
// Round 131 (m3 lane 6, fast): `long offset = 0;` as the first statement 373 (push/pop order and the pages
// register become retail's, the zero moves to EDI, frame still 0x1050); offset declared after the result,
// before the clamp 400 (1231 bytes); the clamp as an if/else-if chain 370 (same code); both 373; offset
// first with the candidate stored previous/offset/time/pages 372.
// Round 47 (m3 lane 14, fast, kept form confirmed plain): round 27's busy form (busy = proc && proc(current)
// in the block arm, `if (busy) { head = tail; continue; }`, scan unconditional) 377 [aligned 239]; on it the
// limit update as `if (result == NONE && type == limit_index)` without the goto 377 (same code; on the old body
// 370, same code); offset = 0 declared first in the function 357 [262] (ebx/ebp pushed at entry and the no-best
// limit update laid out after the loop guard, as retail), after the page count 380, after the result 380, after
// the limit clamp 381; offset and previous = NONE both first 347 [260] (kept; either order, with or without a bare
// `current` declaration there 347); with head/tail there too 384, with oldest 386, with have_best 383, result
// moved after them 395 (1227 bytes); bare offset/previous declarations at the top assigned in place 377; offset
// initialised there with previous assigned in place 357. On the kept form: the policy ages computed and clamped
// one at a time, the products' operands swapped, the offset sum's operands swapped: 347 each (same code).
// Left: ours keeps offset in EDI and current in EBX (retail EBX/EDI), the mask in EBX (retail EDX), pushes EDI
// at entry, stores previous's NONE at entry, homes segment pages/time the other way round, and lays the case 0/1
// policy bodies at the function's end.
// Round 302: Scan cursor advanced before policy/prefetch with original/maximum timestamp and
// field/reused page total 380/378/378/374; enclosing cursor scope 380/378/378/374. Frame reaches 0x1054,
// but length grows to 1223-1232; original scan, values and body restored.
// Round 240: Inner-scan policy/time/segment-home names, seven subsets and all with reversed
// independent policy/time acquisition: 347/421/411/347/347/421/411/411. Policy-only field copy
// exact-only; policy/time reaches frame 0x1054 at 411. Original executable body restored.
// Round 267: page-mask node before/after quotient, expression/split decrement,
// original increment/direct conditional count: 347 in all eight forms. Complete
// opening body restored; 0x1050/0x1054 frame and page-offset/ring roles remain.
// Round 154 (lane 4, relink screen, exact at 347): all six policy case orders 347 (same code, bodies stay at the function's end);
// case 0 as `better = false` plus an if 347, through a one-caller inline 383 (1178), case 0 and default inline 379 (1177), the whole
// switch in an inline with returns or a result 343/343; offset first with previous in the if 357, offset after the page count 380,
// after the result 380, before the if 381 (aligned 195); head and tail as aggregate fields 347, with offset and previous inside 376;
// hill climb of 212 forms over those plus the mask form, candidate fill order, next-offset sum, busy form and segment store order
// 343 at best (candidate `time = pages = 0` with segment time first, 4 points, not kept). Retail's offset stays in EBX through the
// scan with tail in memory and next spilled; no form gave offset the higher register priority.
// Round 308: One post-callback block timestamp with direct/cached/conditional/guarded callback producer
// 347/347/356/358 for long and 347/347/356/358 for dword. Pre-callback segment time remained
// separate. The missing frame dword and page/ring register roles persist; original observations
// and callback expression restored.
// Round 248: Native indexed-ring best/array/deletion-iterator entry storage, seven scope subsets
// and all with coherent object-role names: 347 each. Original scopes, initialization and body restored.
// N304: Actual allocation/category/predecessor spill-name subsets 1..7 all 347;
// all names with limit-before-result 397 (1228 bytes, frame 0x1050). Original body
// restored; missing frame dword and page/ring ownership remain.
// N315: Final predecessor/offset/both/paired overlap-end ownership, admitted locals
// 351/347/351/360; bare entry declarations 351/347/351/360. Original selected record
// and full body restored; missing frame dword and page/ring roles remain.
// N317: Actual winning ordinal long/short, admitted/entry lifetime, after/before native
// candidate copy: 391/391/397/398/391/392/397/398. Frame 0x1054 recovered, but
// page/ring roles and policy placement regress; exact-only selection state restored.
// N319: Shared default-policy clock long/dword, paired/sequential age producers and original/conditional candidate time, all eight 347. Frame 0x1050/0x1054 and page/ring roles remain; original observations and full body restored.
// N325: Actual candidate cursor/physical-block cursor/ring successor reused across
// disjoint phases, seven subsets and all with bare entry declarations: 347 each.
// Original independent workspaces and full body restored; frame and page/ring homes remain.
// N329: Complete branch-owned failure-result defaults at admission/post-scan/no-selection/terminal paths,
// signed and unsigned handle bits: 399/389/391/393/399/389/391/393. All grow code; original entry NONE
// and full body restored.
// N333: Final linking backing/predecessor/successor direct-read subsets 1..7
// 353/347/381/359/367/355/391; complete direct reads with allocated native lookup 377.
// Original reads and full body restored; missing frame dword and page/ring roles remain.
// NOT kept: direct-read subsets tied or regressed and grew beyond retail.
// N335: Candidate decision initialized from not-have-best in bool/byte/short/long:
// 350/350/366/360; true-default with the existing guarded policy: 347/347/355/354. All tie or
// regress; complete original policy/result graph restored. Missing frame dword and page/ring
// homes remain.
// N337: Oldest-block replacement/retention expression results, bool/byte/short/long,
// all eight 347. Original oldest selection and complete body restored; frame and page/ring homes remain.
// m3 lane 11 r62 (fast): offset after the result with previous's NONE in the block 380; offset first with
// previous's NONE in the block 357 (retail stores that NONE in the block, at +0x48); the page shift in a twice-used
// local 347 (same code); head and tail as `head = tail = 0` 347 (same code); oldest declared first in the
// block 379. Retail keeps offset's zero in EBX from before the type clamp (it is the clamp's zero) and stores
// oldest's NONE from the result's register in the block.
// Round 122 (lane 7, fast; base 347 [aligned 260]; retail keeps offset in EBX, current in EDI spilled
// around the scan, previous on the stack and no entry stores for offset/previous): offset and previous
// declared at the top of the admitted block 376 [243] (entry stores gone, prologue then pushes three);
// offset after the result, previous in the block 380 [257]; previous alone in the block 357 [262];
// offset after the limit clamp 378 [241]; current declared first in the block 347 (same code).
// N339: Complete default-policy named products, staged products and sequential/paired
// weight accumulators, each with original/single clock observation: 347 each (eight forms).
// Original arithmetic, clock observations and full body restored; missing frame dword,
// page/ring ownership and policy placement remain.
// NOT kept: weighted storage/clock forms tie and retain the deficient frame.
// Y556: One busy-callback pointer observation at occupied/segment/admitted/entry scope,
// short-circuit/guarded result on original timestamp graph: 347/347/347/347/358/358/358/358.
// NOT kept: no qualifying gain or missing-frame recovery; original body restored.
// m3 lane 11 r62 (fast): offset after the result with previous's NONE in the block 380; offset first with
// previous's NONE in the block 357 (retail stores that NONE in the block, at +0x48); the page shift in a twice-used
// local 347 (same code); head and tail as `head = tail = 0` 347 (same code); oldest declared first in the
// block 379. Retail keeps offset's zero in EBX from before the type clamp (it is the clamp's zero) and stores
// oldest's NONE from the result's register in the block.
// M3 lane 9 r83 (fast): the type clamp as an if/else chain 347 (same code); the matched block's time read before its
// pages (retail reads pages first, ours reversed it) 344, with time-first in all three segment arms 344, plus the
// ring insertion stored previous/offset/time/pages as retail 344 (three-point gain, not kept); on that: offset
// declared at the top of the admitted block 384; switch cases ordered default/1/0 344 (same code: retail lays case 1
// and case 0 after the default arm, ours moves both past the scan whatever the order); previous declared in the
// block after current 357; the ring insertion indexed without the candidate pointer 345.
// M3 lane 6 r142 (fast): the type clamp as an if/else-if on a copy of type 385; result declared first 395 (1227
// bytes); oldest_time zero-initialised 392 (1228 bytes); best declared before the candidate ring 347 (same code);
// the page remainder mask as `~(-1 << shift)` 347 (other code, same count).
// Retail keeps result's NONE in EAX through the clamp and stores oldest's NONE from it. Original body restored.
// Round 69 (m3 lane 10, fast; aligned and register-masked counts in brackets, base 260/139): the 4 missing frame bytes are the next index's home: retail
// keeps offset in EBX, the forwarded candidate pages in EDI and spills next, tail and head, ours gives EDI to tail and EBX to next and keeps offset in memory. Pages, result, offset,
// clamp, with previous's NONE in the block 380 (257/139); the policy switch as an inline helper with a return per case 343 (260/145), on that head 375 (258/152). 240
// random declaration orders of the head, block and segment locals, compile-only: 289-295 against 295, frame never 0x1054. Diagnostics: unused
// pointers to tail or head 347/380 (same code); tail in a scratch global 376 (1228 bytes, 262/120) still leaves offset in memory.
// P305: Final predecessor/successor/both/all linked-record references, each with
// mutable/fixed backing receiver: eight at 347. Original complete body and native
// record accesses restored; missing frame dword and page/ring/policy allocation remain.
// P309: Initial page/category-limit admission as positive/complementary bool/byte/short/long
// producer outcomes: 347/347/347/347/347/347/347/347. NOT kept: one-use outcomes
// do not match; original complete body restored, missing frame dword and page/ring homes remain.
// M352: Final failure-limit observed/capped/branch-produced/complementary minimum,
// compound/nested independent failure-category guards, eight forms 347/347/347/347/347/347/347/347.
// All restored; frame 0x1050 versus 0x1054 and page/ring/policy ownership remain.
// DH333: Standard age/segment/category selections in seven subsets
// 393/388/419/384/433/366/435; final failure-limit minimum 410. The latter recovers
// frame 0x1054 but grows to 1243/1213 bytes. NOT kept: regressions; full original body restored.
// N348: Ring insertion admission in bool/byte/short/long, has-room/full truth,
// 347 each. NOT kept: one-use decisions do not match or recover the missing frame
// dword. Original ring insertion, page/tail allocation and complete body restored.
// N350: Final-link successor handle captured for all consumers, long/const long/
// dword/const dword, pointer/reference predecessor element: 347 each. NOT kept:
// unchanged frame/page-ring homes; original pooled observations and body restored.
// N352: Post-callback allocator-clock long/dword, occupied/segment/admitted/entry
// declarations: 347 each. NOT kept: one-use clock forms remain unmatched and retain
// frame 0x1050/0x1054 and page/ring homes; complete original body restored.
// N367: Joined callback/expiry busy result, expression bool/byte/short/long 360/360/365/365; branch
// producers before oldest admission 347/347/365/367. NOT kept: ties or regressions, wider forms grow beyond
// retail. Original busy/oldest graph restored; missing frame dword and page/ring homes remain.
// N371: Saved ring successor in the ordinary inner for increment, bare scan/segment/
// admitted/entry lifetimes, original then semantic cursor names: 347/347/347/347/347/347/347/347.
// NOT kept: unmatched successor-slot forms; full body restored; missing frame dword and page/ring homes remain.
// N374: Full-width unsigned insertion successor/scan ordinal/saved successor,
// masks 1..7 and immutable complete graph: 347/350/350/347/347/350/350/350.
// Original signed cursors, modulo and full body restored; NOT kept:
// no native signed-remainder/frame recovery or byte match.
// N376: Complete best-record memmove/range-copy/backward-copy contracts, with native
// zeroed/aggregate ring insertion, eight forms 370/369/369/372/391/392/392/404. NOT kept: no exact
// record/frame recovery; original transfers, signed ring and complete body restored.
// m3 lane 1 r71 (one-link harness, exact on control 347; al = register-inclusive aligned count, control 366): LEAD, retail's EDI in the scan is the candidate's running
// total: `long total = candidate->pages + segment_pages;` used by the store, the test and the policies 349/1212 (al 304), with one read of the clock in the default policy
// 348 (al 312). On total: offset first with previous in the block 371/1200 (302), offset after the result 393/1200 (299), with previous in the block 393/1200 (298), offset
// in the block 387/1211 (292), with previous there too 379/1211 (292). About 280 annealed forms of block declaration order, offset/previous position, candidate store order, case
// order, time/zero order and segment/next scope: best aligned 381/1211 (al 292), best positional 349/1212, none under 347. Register roles remain: retail gives offset EBX and current EDI and keeps tail in memory.
// Round 150 (m3 lane 5, fast; m = masked aligned, x = register-inclusive aligned, control 347 m97 x213): LEAD, the ring successor written twice (`i = (i + 1) % 256` in the for,
// `candidates[(i + 1) % 256]` at the prefetch, no next local) 385 m96 x196 gives retail's spilled successor store ahead of the jbe; plus total 385 m98 x185, offset after the result 382
// m97 x181, previous in the block 381 m96 x181; successor, offset, previous, no total 380 m94 x192. Early `return result` in the success path, alone 384 m99 x203, with all above 382 m97
// x187. LEAD, measured only since it repeats the loop test: `if (offset < count) do ... while` with the early return 392 (1196) m79 x193 gives retail's layout exactly (zero-trip `jg`,
// failure tail, first epilogue, loop). On it: candidate offset stored before time 390 m78, tail declared first 390 m78, no total 390 m76 x194; policy as a forced-inline helper with a
// return per case 390 m76, static inline 507 (1086), the helper with no guard 381 m95; empty do-while(false) at the policy join, in the block, both, in case 0 390 m76 each; true default
// 390 m76, `!have_best` default 396 (1200) m86, case 0 as an if 390, both 390; `continue` for the page test 390 m76, true default with the test reversed 390 m78, a prefetch pointer 390
// m76; no-selection arm first with its own return 393 (1229) m109. Left under the guard: case 0 and 1 bodies past the end of the function (retail in place, not threaded), the 4 missing
// frame bytes, and head not cached in EDX at the loop entry. Body unchanged at 347.
// Round 215 (lane 3, fast; x = register-inclusive aligned, m = masked aligned, control 347 x260 m151): kept: the walk's block pointer ahead of the iterator 341 (x249 m146), block-arm time first 338 (m137).
// Measured on that: offset after the result 377; previous in the block 352; candidate offset before time 338 (m135); total 339; none-arm time first 338; gap-arm time first 338; clock local 338; offset before the link update 338;
// time compare reversed 338; inner while 338; no ring local 338; ternary clamps in the case 339 (x215 m117); re-derived insertion 355 (1230).
// LEAD, unmatched and not kept (new helpers, a written-out successor): offset after the result + previous in the block + candidate/none/gap orders + successor written at both uses (`candidates[(i + 1) % 256]`, `i = (i + 1) % 256`)
// gives retail's roles (offset EBX, current EDI, tail in memory, successor in ECX with its spill): 378 (x224 m123). Plus an early `return result` in the success path, plus two forced-inline policies with one result each (time then pages; weighted age with
// `now - a->time > 0 ? now - a->time : 0`, the shape of 0x13d320, called from the switch or from a forced-inline switch in the `!have_best ||` test): 364 (1194, x121 m41), policy blocks in place. Plus the insertion re-derived per use (element macro, `next` local,
// `before` read after the arms; `first` as a pointer ahead of `entry->previous = NONE` in each arm of the first-block test): 353 (1213, x98 m30), NONE in EBX as retail. On that: guard around a do-while 417 (1181); total 344 (1195); a maximum helper 349; overlap helper 321 (1200, x139);
// duplicated tails in the after/none arms 352 (x134); limit test as an early return 398 (1224); head and tail as a local struct 361 (0x104c); head from tail, chained zeros, offset named in the limit tests 353; a local start 353.
// Left there: an early zero register at the limit clamp (retail's zero is offset's EBX, NONE in EAX, type in ECX; dropping any two real zero literals removes it, a threshold, so retail's source has fewer: not found), the skip path's `xor esi, esi` (retail literal zero in the none arm),
// late `push edi` (follows NONE in EAX), the walk's `movsx` before the add and `mov edx, edi`, the after arm past the epilogue, and the slot order.
// On the zero register: one real zero literal fewer (candidate time, one segment time, head, one clamp arm, the limit clamp's zero) removes it; copies of a zero (head from tail, chained stores, a helper's parameter) do not.
// Round 217 (lane 3; grid on fast, then plain; x = register-inclusive aligned, m = masked aligned): kept 338 -> 209 (1228 bytes): offset and previous in the admitted block, in the order oldest, have_best, head, offset, tail, current, previous; candidate, none and gap statement orders; `MAX(physical->time - x->time, 0)` for the default arm's two clamps. Same with `dword now` 209; on it `better` uninitialized 209, case 0 through a flag 209, an empty join 209, better preset true 209, written-out successor 209, direct insertion 230 (1246).
// RECIPE (match-only parts, not kept; 93, 1213 bytes, x 37, m 12, first difference +0x338: entry, clamp, late push edi and the loop all retail's): the above plus the successor written at both uses (`candidates[(i + 1) % 256]`, `i = (i + 1) % 256`), two forced-inline policies with one result each (time then pages; weighted age with the clamp as one ternary per age) called from a forced-inline switch in the `!have_best ||` test,
// and the insertion in two stages, the first test repeated after the arms: `if (best.previous == NONE) { if (physical->first == NONE) { entry->previous = NONE; physical->last = result; } else { element pointer ahead of entry->previous = NONE; element->previous = result; } } else { long next = element(best.previous)->next; if (next == NONE) {...} else {...} } if (best.previous == NONE) { entry->next = physical->first; physical->first = result; } else { before; entry->next = before->next; before->next = result; }`. The repeat is what sinks retail's after arm past the ret (without it the arm stays in line, 125 with offset at the top, x 45 m 24); written `!= NONE` 125 (m 58-60); direct first element 124 (m 19), `long first` 122 (m 16); offset at the top with that two-stage form 125 (x 27 m 10: only the entry's early zero and the walk differ).
// Left on the recipe: EBX and EDI swapped for the scan's maximum and bit and for result and NONE (`or edi, -1; cmp eax, edi` where retail has `cmp eax, -1` and `or ebx, -1` later), and the walk (retail's data base in EBX, `movsx ecx, [eax + ebx]` ahead of the add, `add ebx, edi` and a separate best.offset load copied from EDI for the second compare). No inner `return result`: with one, 353 (this was the block on round 215's lead). Offset: declared at the top 125, between the clamp and the limit test 244, first in the block 258, ahead of head 113, ahead of tail, current or previous 93, declared at the top and assigned there 93.
// On the recipe: block declarations best/iterator/oldest_time/segment/next_tail/i/candidate moved 93 each; overlap test with a start, end, block offset or block end local, sum swapped, inverted 93; reversed compare 93 (x 39); local iterator step with the salt read first 170 (x 79), with an offset local 387 (1216); result declared first 389 (1238), `long allocated` 93, in the test 93; `long oldest = result` 93 (same code). Dead volatile load probes (diagnostic, reverted): walk body 55 (1214), after the walk 81, insertion 97, at `if (have_best)` 98. Zero diagnostics on the 125 form (wrong values, reverted): head from pages 98 (x 31, m 20, retail's entry exactly), tail 127, candidate pages 384 (1191), gap time 347, second clamp 289 (1225), offset 327, `type < 1` 267, both limit zeros 305 (1224).
// T64: Native default-clamp producer boundaries on the entry-matching graph: zero-first maximum 225;
// sequential/paired once-produced ages [373, 373]; zero-first stages [225, 225];
// zero-default/explicit positive-zero/zero-test results [260, 209, 222]. NOT kept: no byte match;
// original macro, field observations, policy, ring, deletion and linking restored.
// P365: Complete selected-candidate scalar publications in eight field schedules: eight 359
// at 1217/1213 bytes, first +1. NOT kept: no native policy/copy equality; original
// record assignment, arithmetic, entry/frame and complete body restored.
// P367: Case-zero repeated candidate/best timestamp values, singly and in both
// declaration orders, mutable/const: eight 209s. NOT kept: no native policy
// placement or result equality; original field observations and body restored.
// P369: Actual candidate-age/selected-age/replacement-result identities, subsets 1/2/4/3/5/6/7 and immutable ages:
// eight 209. NOT kept: no native clamp-register or policy-placement repair; original whole body restored, frame
// 0x1054 retained.
// P371: Existing paired-age bare declarations at entry/ring/selected-record/outer-scan/
// candidate-scan/iteration/size-admission/policy-admission: eight 209s. NOT kept:
// no native clamp-register or policy-placement repair; original complete body restored.
// P373: Existing policy-result bare declaration at function/admission/record/outer-scan/
// outer-iteration/inner-scan/inner-iteration/size-admission boundaries: eight 209s. NOT kept:
// policy-placement/clamp homes unchanged; original whole body and false producer restored.
// T69: Entry-matching policy replacement signed char/byte/short/long results
// 209/209/212/211; retention-complement bool/byte/short/long 212/212/222/222.
// NOT kept: ties/regressions; original policy result, case bodies and complete body restored.
// P380: Natural nonempty-first first/predecessor/successor list-publication arms, seven branch subsets
// and complete topology with its actual predecessor outcome.
// Scores 210/212/213/215/215/215/215/215. NOT kept: no byte match; retained frame agrees but policy
// cases and weighted-age homes remain, and no publication form shortens toward native length; some grow.
// Original complete body restored.
// P387: Both age clamps through a standard explicit ordering-comparator maximum,
// declaration/sequential/paired/product producers with zero second/first:
// 439/439/456/456/422/422/411/411. NOT kept: every provider graph regresses, loses
// the native frame and grows to 1265-1311/1213 bytes. Original macro, policies,
// observations, ring scan, record copy and complete body restored.
// P397: Equality-first conditional/complete policy arms and earlier-time-first
// conditional/complete arms: 284/284/201/201; with one complete selection outcome:
// 227/227/222/222. NOT kept: no native case-placement or age-mask carrier repair;
// complete outcomes move extent away from retail. Original whole body restored.
// M3 lane 16 r97 (fast, 0x13d320 matched each time; body restored). Grid on the 201 body; M = second age as
// `MAX(time - best.time, 0)`, S = the comparison written `candidate * best_age < best * age`, T = `long total =
// candidate->pages + segment_pages` used by the store, the page test and both policies, N = one `dword now` for both
// ages: S 201 (same code); N 201 (same code); M 209 (1228 bytes); M+S 209; M+N 209; T 213 at retail's length 1213 (the
// default arm then multiplies from EDI as retail, but retail loads the segment pages into EAX ahead of the add and
// ours adds from memory); T with the sum's operands swapped 213 (same code); T+S 213; M+T 218 (1218 bytes); M+S+T 218;
// M+T+N 218; M+S+T+N 218. The policy result preset true with `if (better)` 201 (same code); `if (!have_best) better =
// true; else switch` 201 (same code); case 0 as an if / else-if pair that sets true 201 (same code).
// M3 lane 11 r81 (fast): the take test as `better || !have_best` 201 (same code); the walk's overlap conditions swapped 203 (+0xcc);
// the policy as an if/else-if chain 240 (1178 bytes: cases in line but in source order, case 0 still threaded, aligned 118 -> 109);
// case 0 as two `better = true` arms 201; default ages written inside the comparison 201. Retail's `sub eax, 0` chain is a switch.
// M3 lane 5 r167 (fast, 0x13d320 matched each time; base 201, aligned 87, masked 57): the second age's mask restored with case 0
// negated, `!(time > best || (time == best && pages >= best))`, 209 (1228 bytes); the two products named with the mask
// restored 209; the best's age computed first 203 (1213 bytes, masked 59), with named products 203; the mask restored
// with case 1 as `if (...) better = true;` 229 (1228 bytes, first difference +0x4e). Re-measured: one `dword now` with
// both clamps as ternaries 209 (lane 16's M+N), the named products alone 201 (same code). With both masks
// (209, masked 52) the default arm is retail's but for the first age in EAX where retail uses ECX, which folds the best
// pages into `imul eax, [mem]` where retail loads them first; the case 0/1 blocks still sit at the end. All restored.
// m3 lane 3 r227 (fast): our case 0 is jump-threaded into the take test (its true/false edges jump straight to the take
// and skip blocks), which is what moves cases 0 and 1 past the end; retail materialises the result (`xor al, al; ... mov al, 1`)
// and tests it at the join. Case 0 as `candidate->time == best.time ? pages < best.pages : time < best.time` 284 (1182 bytes:
// no threading, cases in place, but `setl`/`setb` values and every frame slot shifted, first difference +0x2f); as `if (time !=
// best.time) better = time < best.time; else better = pages < best.pages;` 284 (same code); negated (`!(time > best.time ||
// (time == best.time && pages >= best.pages))`) 222 (1220); the ternary with the second age's mask restored 279; the mask
// alone 209 (re-measure); `have_best = true` before the record copy 201 (same code).
// Round 149 (m3 lane 8, fast; aligned = register-masked difflib count, base 86): ours folds the second age's
// `MAX(0, ...)` away (unsigned, `0 > x` never holds) where retail computes both clamps (`cmp; sbb; and`), and our case 0
// is threaded into the take test, which moves cases 0/1 past the loop; retail keeps default, 1, 0 in place and case 0
// materialises `xor al, al ... mov al, 1` into the join. Case 0 as `better = candidate->time < best.time; if
// (candidate->time == best.time) better = candidate->pages < best.pages;` 235 (1178 bytes, aligned 65: cases in place
// as retail, but setb/setl), with the second age as `MAX(physical->time - best.time, 0)` 227 (1183 bytes, aligned 61);
// with that clamp: case 0 as a ternary on the time test 209 (1228), as an if / else-if pair setting true 209 (1228), the
// take test as `better || !have_best` 209 (1228), all aligned 84.
// Round 104 (m3 lane 9, fast): case 0 as `if (time < best time) better = true; else if (times equal) better =
// pages < best pages;` 235 (1178 bytes, aligned 118 -> 95: case 0 sits in place after case 1 as in retail, only its
// last test is a setl where retail branches to `mov al, 1`); that with the second age as `MAX(physical->time -
// best.time, 0)` 209 (1193, aligned 112); case 0 as a ternary on the times being unequal 284 (1182), with the clamp
// 279; as an if/else on the times being unequal 284, with the clamp 279.
// M3 lane 9 r109 (fast with 0x13d320, plain at the end): the default ages as `long` with no casts on the comparison 201 (same code,
// kept). LEAD (93 plain at 1213 bytes, masked aligned 18, first difference +0x338; patch build/r109/lead-0x13d370-93.patch;
// kept only with a match): case 0 through a forceinline helper in 0x13d320's style (`bool result = false; if (older, or as
// old and smaller) result = true; return result;`) 237 (1178: case 0 is then retail's `xor al, al ... mov al, 1` and the
// cases sit in place), plus both ages `MAX(x, 0)` 115 (1194), plus the before block read after the arms through a fresh
// `physical->blocks->data` with a `next` local for its successor 105 (1213), plus the insertion in two stages (round 217's
// repeated `best.previous == NONE`) 106, plus the scan's successor written at both uses 102, plus `entry->previous = NONE`
// in each arm with the first block's pointer formed ahead 93. Left on it: one EBX/EDI swap from the deletion walk on
// (high-water index and bit, then result and the NONE register, which ours also uses for the allocation test); it reads
// as retail picking EBX first for those late webs where ours picks EDI. On 93, each the same code: the helper `static
// inline`, the overlap test nested, the sum as `pages + best.offset`, the walk through this file's function_13d8b1, the
// link as one forceinline helper in block_delete's style, the whole switch as one forceinline helper, `dword now`,
// `better` declared at the loop top, copies of the oldest block's previous, the pool, the busy callback and the segment
// end; the overlap test's operands swapped 93, both swapped 93, its two tests in the other order 68 (positional only:
// masked 27, the outer loop's reload order flips); the walk's block as a plain local 116; the ages written inside the
// comparison 216; the permuter (64 tries) none below 93. Diagnostic volatile loads (removed): in the walk body 73, before
// the walk 115, neither flips the swap.
// M3 lane 11 r89 (fast with 0x13d320; main 201): r109's 93 lead reapplied 93 (1213 bytes, masked aligned 20). On it (kept
// only with a match): result declared after limit_index 236; the entry's offset/pages/time stored before the linking 94;
// `best.offset + pages` named once ahead of the walk 383 (1219 bytes); result declared first 389 (1238); the allocated
// index in its own local copied to result at the end 211 (1199); the final test as `type == limit_index && result == NONE`
// 364 (1230). Same code on 93: the walk's block offset named, the overlap end named inside the walk, best.offset copied to a
// local, the block end named, the block sum's operands swapped, the tail's block array renamed. The walk's EBX/EDI choice
// (retail: high-water EDI, bit and data EBX, sum into pages' register) and the tail's result/NONE pair stay swapped.
// M3 lane 13 r81 (fast with 0x13d320, which stays matched): lane 9 r109's 93 lead rebuilt is 93 again (1213 bytes, first
// difference +0x338, fully masked 14). What is left on it: the walk's count and bit (ours EBX/EDI, retail EDI/EBX); the
// overlap sum computed into best.offset's register with best.offset reloaded for the second test, where retail adds into the
// pages register and compares a copy (`mov edx, edi; cmp eax, edx`); the NONE register materialized before the allocation
// test (`or edi, -1; cmp eax, edi`), where retail tests the immediate and sets EBX to -1 inside the success arm; result in
// EBX at the return (retail EDI). On the lead: the oldest-block test with `oldest != NONE` first 91 (positional only: retail
// tests the count first); result declared in the candidate block with a separate `return NONE` 391 (1225 bytes, first
// difference +0x2f); same code (93): the deletion iterator at function scope, named `start`/`end` bounds for the overlap test,
// `end` alone, a named block offset, the iterator's fields initialized index first. On the kept body: the oldest test with
// `oldest != NONE` first 201 (different code); the default age clamped by `if (age < 0) age = 0;` 273 (first difference
// +0x1); the new candidate's pages cleared before its time 203; the lead's two-stage insertion alone 213 (1227 bytes).
// MiniMax E (11-11), NOT kept (coordinator 11-11): 201 -> 93 came from a new one-caller helper for the candidate test, `entry->previous = NONE` copied into both arms, a one-use pointer to the first block and the best.previous test split in two with the block data reloaded (forms kept only with a byte match; a one-caller helper never); body at 201. The split link order is the lead to explain in plain code.
// M3 lane 11 r95 (fast with 0x13d320, which stays matched; base 201, masked aligned 86). Retail's insertion reads the pool data
// once after the allocation for the entry, the first block, the predecessor's successor test and the successor block,
// keeps only the predecessor's scaled index (`shl esi, 3`), and after the successor arms reloads `physical->blocks->data`
// to update the predecessor's next. Forms: the second age as `MAX(physical->time - best.time, 0)` with the insertion through
// element expressions at each use and the successor in `long next` 230 (1247 B, masked 74: the scaled index and the join
// reload as retail, but the first arm reloads the data too); that clamp with the insertion in two stages (back links through
// the `blocks` local with the successor in `long next`, then `if (best.previous == NONE) {entry->next = first; first =
// result;} else` the predecessor taken afresh from `physical->blocks->data`) 221 (1237 B, masked 55: the insertion is then
// retail's but for the NONE register, `or ebx, -1; cmp esi, ebx`, and one store's place; a repeated test, match-only;
// diff saved as build/r95/lead-0x13d370-masked55.patch in m3-lane11's worktree); the two-stage insertion with main's
// clamp 208 (1221 B, masked 57); case 0 as `if (older) { better = true; break; } if (same time && fewer pages) better =
// true;` 201 (same code, still threaded); `bool better;` with no initializer 201 (same code). What keeps the count up on
// every form is case 0 threaded into the take test, which moves cases 0 and 1 past the loop. Body restored.
// M3 lane 2 r79 (fast with the file's functions and 0x13d320, all MATCH; base 201, register-masked aligned 88), case 0 spellings
// with the same truth table: `better = time <= best.time && (time < best.time || pages < best.pages)` 222 (1220 B); `if (time !=
// best.time) better = time < best.time; else if (pages < best.pages) better = true;` 200 (1213 B, retail's length, but case 0 still
// past the loop: setb, then its pages test threaded; masked 96; a one-point gain and a bool set by if/else-if, not kept), that with
// the second age as `MAX(physical->time - best.time, 0)` 200 (1222 B); nested `if (time <= best.time) { if (time < best.time ||
// pages < best.pages) better = true; }` 222; the whole test in an if/else with constant arms 201 (same code); equal time first
// (`if (time == best.time) { if (pages < best.pages) better = true; } else better = time < best.time;`) 232 (1180 B: the three
// cases then sit before the take test in retail's default/1/0 order, masked 69, but the pages test still jumps straight to the
// take and skip blocks); `if (time > best.time) break;` then the older-or-smaller test 222. Equal time first with both arms
// setting true (`if (time == best.time) { if (pages < best.pages) better = true; } else if (time < best.time) better = true;`)
// 206 (1219 B); equal time first with `better = pages < best.pages` there, else `if (time < best.time) better = true;` 200 (1213
// B, a different code from the 200 above); unequal time first, both arms setting true, 206 (1219 B). The nested and break forms
// are the same code as the first form (222); the constant-arm if/else the same code as main.
// m3 lane 8 r164 (fast with 0x13d320, MATCH each time; base 201). LEAD, match-only (a flag set by an if and copied): case 0 as a
// block with `bool older = false; if (older test) older = true; better = older;` 234 (1178 B): with no helper, case 0 is then
// retail's `xor al, al ... mov al, 1` in place after case 1, not threaded into the take test (the copy hides the constant
// from the threader). On it: the second age as `MAX(physical->time - best.time, 0)` 225 (1183 B; the default arm is then
// retail's but for the first age in EAX where retail has ECX, so the best pages fold into `imul eax, [mem]`); with the best's
// age first 232; the comparison written `candidate pages * best age < best pages * age` 225, both ages `dword` 225, the
// products' factors swapped 225 (each the same code); the insertion with the block data read at each use (no `blocks`
// local) 221 (1189 B); that with the predecessor also read at each use (no `before` local) 228 (1217 B: every frame slot moves,
// first difference +0x2f, and the data is reloaded after the entry's first store where retail keeps it). Left on 221: the
// walk's count/bit (ours EBX/EDI, retail EDI/EBX), the allocation result's register and the insertion's mixed data reload.
// P472: Case-zero older/equal-time/smaller Boolean outcomes, subsets 1/2/4/3/5/6/7, then all byte outcomes: 201/201/201/201/201/201/201/201.
// NOT kept: no native case-result/placement equality; original policies, observations, ring, iterator, linking and full body restored.
// @retail 0x13d370
long __stdcall function_13d370(s_physical_object *physical, long size, long type)
{
	long pages = size >> physical->page_shift;
	if (size & ((1 << physical->page_shift) - 1))
		pages++;
	long result = NONE;
	long limit_index = type < 0 ? 0 : (type > 8 ? 8 : type);
	if (pages < physical->limits[limit_index] || limit_index <= 0)
	{
		s_block_candidate candidates[256];
		s_block_candidate best;
		long oldest = NONE;
		bool have_best = false;
		long head = 0;
		long offset = 0;
		long tail = 0;
		long current = physical->first;
		long previous = NONE;
		dword oldest_time;
		while (offset < physical->page_count)
		{
			long next_tail = (tail + 1) % 256;
			if (next_tail != head)
			{
				s_block_candidate *candidate = &candidates[tail];
				candidate->previous = previous;
				candidate->offset = offset;
				candidate->time = 0;
				candidate->pages = 0;
				tail = next_tail;
			}
			long segment_pages;
			dword segment_time;
			if (current == NONE)
			{
				segment_time = 0;
				segment_pages = physical->page_count - offset;
				offset = physical->page_count;
			}
			else
			{
				s_physical_block *block = &((s_physical_block *)physical->blocks->data)[current & 0xffff];
				if (offset == block->offset)
				{
					segment_time = block->time;
					segment_pages = block->pages;
					bool busy = physical->busy_proc && physical->busy_proc(current);
					if ((dword)(block->time + type) >= (dword)physical->time)
						busy = true;
					else if (!busy && (oldest == NONE || (dword)block->time < oldest_time))
					{
						oldest = current;
						oldest_time = block->time;
					}
					previous = current;
					current = block->next;
					offset = block->pages + block->offset;
					_mm_prefetch((char *)&((s_physical_block *)physical->blocks->data)[current & 0xffff], _MM_HINT_T0);
					if (busy)
					{
						head = tail;
						continue;
					}
				}
				else
				{
					segment_time = 0;
					segment_pages = block->offset - offset;
					offset = block->offset;
				}
			}
			for (long i = head; i != tail; )
			{
				s_block_candidate *candidate = &candidates[i];
				long next = (i + 1) % 256;
				_mm_prefetch((char *)&candidates[next], _MM_HINT_T0);
				if (segment_time > candidate->time)
					candidate->time = segment_time;
				candidate->pages += segment_pages;
				if (candidate->pages >= pages)
				{
					bool better = false;
					if (have_best)
					{
						switch (physical->state)
						{
						case 0:
							better = candidate->time < best.time || (candidate->time == best.time && candidate->pages < best.pages);
							break;
						case 1:
							better = physical_memory_candidate_is_better(physical, (s_physical_block_view *)candidate, (s_physical_block_view *)&best);
							break;
						default:
							{
								long age = MAX(physical->time - candidate->time, 0);
								long best_age = MAX(0, physical->time - best.time);
								better = best.pages * age > candidate->pages * best_age;
							}
							break;
						}
					}
					if (!have_best || better)
					{
						best = *candidate;
						have_best = true;
					}
					head = (head + 1) % 256;
				}
				i = next;
			}
		}
		if (have_best)
		{
			s_physical_block_iterator blocks;
			blocks.iterator.data = physical->blocks;
			blocks.iterator.index = NONE;
			blocks.iterator.datum_index = NONE;
			while ((blocks.block = (s_physical_block *)data_iterator_next_inlined(&blocks.iterator)) != NULL)
			{
				if (blocks.block->offset < best.offset + pages && blocks.block->offset + blocks.block->pages > best.offset)
					physical->block_delete(blocks.iterator.datum_index);
			}
			if (physical->blocks->actual_count == physical->blocks->maximum_count && oldest != NONE)
			{
				if (best.previous == oldest)
					best.previous = ((s_physical_block *)physical->blocks->data)[oldest & 0xffff].previous;
				physical->block_delete(oldest);
			}
			result = record_pool_allocate(physical->blocks);
			if (result != NONE)
			{
				s_physical_block *blocks = (s_physical_block *)physical->blocks->data;
				s_physical_block *entry = &blocks[result & 0xffff];
				if (best.previous == NONE)
				{
					entry->previous = NONE;
					if (physical->first == NONE)
						physical->last = result;
					else
						blocks[physical->first & 0xffff].previous = result;
					entry->next = physical->first;
					physical->first = result;
				}
				else
				{
					s_physical_block *before = &blocks[best.previous & 0xffff];
					if (before->next == NONE)
					{
						entry->previous = physical->last;
						physical->last = result;
					}
					else
					{
						s_physical_block *after = &blocks[before->next & 0xffff];
						entry->previous = after->previous;
						after->previous = result;
					}
					entry->next = before->next;
					before->next = result;
				}
				entry->offset = best.offset;
				entry->pages = pages;
				entry->time = physical->time;
			}
		}
		if (result == NONE && type == limit_index)
			physical->limits[limit_index] = physical->limits[limit_index] > pages ? pages : physical->limits[limit_index];
	}
	return result;
}

