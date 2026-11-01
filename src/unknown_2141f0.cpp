#include "unknown_11c920.h"
// @flags /O2 /Gr
#include "async.h"
#include "unknown_122870.h"
#include <string.h>

struct s_cache_copy_request
{
	char map_name[256];
	long priority;
};

struct s_cache_copy_progress
{
	long completed;
	volatile dword total;
};

struct s_copy_header_view
{
	long signature;
	long version;
	long size;
	byte unknown00c[0x20 - 0xc];
	char path[256];
	byte unknown120[0x20];
	short type;
	byte unknown142[0x14c - 0x142];
	FILETIME last_used;
	byte unknown154[0x174 - 0x154];
	bool dependencies_required[3];
	byte unknown177;
	FILETIME source_time;
	FILETIME dependency_times[3];
	byte unknown198[0x2d0 - 0x198];
	dword checksum;
	byte unknown2d4[0x800 - 0x2d4];
};

struct s_cache_file
{
	s_file_handle file;
	s_copy_header_view header;
};

extern s_cache_file g_557c90[3];
extern s_cache_copy_request g_55be24[2];
extern s_cache_copy_progress g_55bcf8;
extern long g_55bd04;
extern long g_55bd08;
extern bool g_55bd0c;
extern long g_55bd18;
extern long g_55bd1c;
extern bool g_55bd20;
extern char g_55bd21[0x103];

s_copy_header_view g_55b4f0;
dword g_55bcf0;
long g_55bcf4;
long g_55bd00;
long g_55bd10;
s_file_handle g_55bd14;
dword g_55c02c;

long const g_448fd4[7] = {0, 2, 2, 2, 2, 2, 2};
long const g_448ff0[7] = {0x40000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000, 0x100000};

bool cache_header_verify(s_cache_header const *header);
void file_ref_clock_time(FILETIME *time);
dword __stdcall checksum_buffer_mmx(void const *buffer, dword buffer_size);
bool cache_copy_complete(char const *map_name);
long map_location_get(char const *map_name);
void map_file_path_get(char const *map_name, char *path);
long cache_file_slot_from_type(long type);
long cache_file_choose_slot(short type, dword size);
void cache_copy_buffer_resize(long size);
void cache_copy_buffer_release(void);
void cache_copy_forget(char const *map_name);
bool map_copy_request(char const *map_name, long priority);
void __stdcall cache_copy_record_use(char const *map_name);

// score 37 (was 37): state 0 is blocked by 0x214db0 (map_file_path_get, src/unknown_213d20.cpp):
// ours keeps EDI across the call, so the compiler folds the address of the pending request into
// one register and the block is three bytes short, while retail's callee clobbers EDI. Left:
// state 0 copies the request before the path call, states 7 and 9 keep the handle in another
// register, state 11 swaps the completed and limit registers, state 16 sets the state before the
// memset, and state 19 clears the slot before the buffer release.
// Earlier, 37 (was 39): state 11 writes the chunk size to g_55bcf4 before the positioned-copy call
// and reads the slot into a named local, which puts the buffer in ESI as retail has it. Left:
// state 0 copies the request before the path call, states 7 and 9 keep the handle in another
// register, state 11 swaps the completed and limit registers, state 16 sets the state before the
// memset, and state 19 clears the slot before the buffer release.
// Earlier, 39 (was 47): state 14 reads the slot's file handle into a named local before the write
// call, which puts that load right after the clock call where retail has it; states 13 and 14
// now match retail exactly. Left: state 0 still copies the request before the path call, states 7
// and 9 keep the handle in another register, state 11's registers differ (buffer in EDX, limit in
// ESI, chunk in ESI, slot in EAX), state 16 sets the state before the memset, and state 19 clears
// the slot before the buffer release.
// Earlier, 47 (was 114): state 11 keeps the chunk size in a local and writes it to g_55bcf4 after the
// positioned-copy call, which makes that block exactly retail's length (99 bytes) so every later
// block lines up again. Left: state 11's registers differ (buffer in EDX, slot in EAX, file in ECX,
// handle in EDX), state 0 still copies the request before the path call, and states 7/9/13/14/19
// differ in register choice and statement order.
// Earlier, 114 (was 161): state 11's no-buffer path now assigns state = 11 explicitly, so the compiler
// materialises `mov esi, 0xb` and merges the exit into state 10's tail (retail's short je); the body is
// retail's length (1720) again and every case offset, the jump table and its entries match. Left:
// state 0 still copies the request before the path call (retail calls 0x214db0 first and re-forms
// 0x55bf28), and states 7/9/11/12/14/16/19 still differ in register choice and statement order.
// Earlier, 161 (was 367): state 0 copies the request before the path call (retail calls 0x214db0 first and
// re-forms 0x55bf28 after it, BLOCKED-BY 0x214db0: ours keeps the constant in EDI, 3 bytes short; copying first is
// the only order whose block has retail's length, which lets round 85's retail forms below count). Left: state 7/9/14
// file-handle registers (ECX/EDX), state 11's buffer in EDI (retail ESI) and argument order, state 10/16/19 constant
// scheduling, 1724 bytes against 1720.
// Round 122 (fast/plain): copy-first alone 335; with round 85's split states 8/15, explicit write-flag temporaries,
// named size_known and indexed dependency time 161 (kept; the four without copy-first 430). On that: state 10/16
// statement orders 161/161/161; state 7 without the file local 341; state 9 without it 203; slot published after
// the memset 520; state 11 file local before/after the clamp 290/274; state 1 after the copy 382; pending pointer 367.
// Earlier: aligned frame 0x110 agrees; request-source reuse, dependency bound and write-option scheduling remain.
// Round 74, case-block sizes against retail's jump table: the two 0x1a1050 writes with explicit
// `s_file_write_flags(0)` temporaries (retail pushes the zero from EAX; the conversion builds it in place)
// fix states 7 and 14; states 8 and 15 split with case 15 written after case 14 (retail's block order)
// and state 6 through a named size_known bool give retail's length (1720) and every case offset from
// state 12 on: 432 (fast). Still off: state 0's `mov esi, edi` after 0x214db0 (retail re-forms
// 0x55bf28, 3 bytes; a pending-request pointer and a memcpy copy change nothing) and state 11, 3 bytes
// long, whose no-buffer exit retail merges into state 10's `state = 11` tail. Flags temporaries alone
// 465 (1688 bytes); split alone 468; split + temporaries 430 (case 15 after case 8); memcpy request
// copy 367. All restored: the lockstep count only falls once state 0 is fixed.
// Round 85: retail keeps states 8 and 15 as separate cases (`state = 9;` / `state = 16;`); with them split, the
// writes' flags as `s_file_write_flags(0)` temporaries, state 6's first test through a named `size_known` bool
// and the dependency time indexed (`&g_55b4f0.dependency_times[type]`, retail's pointer exit test) the body is
// retail's length and order (aligned differences 111 -> 53), but the score is 461/434/432/430: the one
// remaining shift is `mov esi, edi` after the 0x214db0 call where retail re-forms the constant (3 bytes), which
// adds a loop pad and misaligns every later line. BLOCKED-BY 0x214db0 (callers act as if it had no register
// summary). The three local forms on the merged-case body 453.
// Request library copies 471/471; state increments 471; shared flush states 472; shared write-completion
// states 433.
// Signed-word dependency index 446; unsigned index 446; explicit existing write flags 453;
// terminal snapshot assignment 433; adjacent write-header cases 434.
// Completed-request/result names 433; direct selected-file calls 440; direct size predicate 374;
// header-result branches 413; terminal disposition tests 434; adjacent state clears 439;
// direct size predicate plus header-result branches 382.
// Direct initial slot 393; failed publication first 473; terminal case first 467; signed-word
// state 572; dependency-time cursor 367 kept (1712/1720 bytes); cursor-end bound 375.
// Cursor/count while 469; unsigned cursor ordinal 367; native unsigned state 367; dependency file pointer
// 445; pointer plus while 479.
// Dependency Boolean reduction 536; immutable terminal snapshot 367; header-copy case beside request-copy
// case 495; result-first declarations 367; byte cursor ordinal 367.
// Paired dependency cursors 367; signed-word cursor ordinal 367; terminal memmove 568; separate name/priority
// locals 383; header failure-first result 430.
// Positioned write inline/noinline 367/367; positioned copy 367/367; flush 367/367; write+copy/all inline
// 367/367. All existing declarations restored.
// Cursor/count while 469; unsigned cursor ordinal 367; native unsigned state 367; dependency file pointer 445; pointer plus while 479.
// Dependency Boolean reduction 536; immutable terminal snapshot 367; header-copy case beside request-copy case 495; result-first declarations 367; byte cursor ordinal 367.
// Paired dependency cursors 367; signed-word cursor ordinal 367; terminal memmove 568; separate name/priority locals 383; header failure-first result 430.
// Native file record references in states 7/9/13/14, paired 7+9/9+13/7+14 and all
// 367 (eight forms); original receivers restored; request source and write options remain.
// Round 342: active request field copy 499; completed-size operand order 367;
// checksum-result-first assignment 367; slot publication before file pointer 364 (three-point
// gain rejected); isolated direct final file 435.
// priority observations 568; copy size result 367; completed byte result 367; accumulated checksum 448; retry
// disposition first 367; staged request admission 367; active request memmove 433; no slot case exit 367.
// Out-of-line path/checksum/both/clock/all call summaries 367 each; completion states first 491;
// destination states first 451; path summary with completion-first states 491 (fast).
// Priority word/byte 506/516; retained-cursor do 469; operation-group/reverse cases 479/504;
// sole time-array traversal 415; staged size result on cursor body 448; checksum state first 367.
// Individual case swaps 1/2, 3/4, 5/6, 7/8, 9/10, 11/12, 13/14,
// 17/18: 386/390/365/367/367/367/371/381. Original order restored.
// Round 377: Separate incoming/next-state long/dword/short/byte 367/367/543/524;
// arm-produced next-state results 528/528/593/530 (fast control 367). Complete
// transition-result ownership does not recover request-source or write-option
// scheduling; byte arm results reach retail length but regress. All forms restored.
// Round 176: Case 19 after 0 / 12 after 4 / 16 after 19: 461/444/431;
// first+second / first+third / second+third / all: 486/492/447/500;
// case 18 after 19: 410. All case bodies and original order restored.
// Defining-unit inline path/checksum/path+checksum/path with separate noinline/clock/path+clock/
// clock+checksum/all call declarations 367 each; all declarations and bodies restored.
// Complete failure result bool/byte/short/long 661/661/654/657;
// the same result initialized first 661/661/654/657. Original success restored.
// Round 223: Repeated transfer buffer/count/limit/buffer+count/buffer+limit/count+limit/all
// inputs 366/362/367/362/367/362/362; all with named size 367; count and buffer+count
// with earlier slot publication 367/367. Five-point exact-length gains restored.
// Round 148: admission after validation 458; retirement after validation 412; both request cases
// after validation 502; validation/retirement first 501; publication phases first 498;
// publication phases after validation 517; header phases first 520;
// admission/retirement/publication together 492. All executable forms restored.
// Round 152: Actual inline definitions plus all declarations, path/checksum/both 367/367/367;
// with separate noinline boundaries 367/367/367; completed-count combinations 362/362.
// All supporting declarations and the original body restored.
// Round 23: true-default failure-only handle/size/shared-write 480/400/444; handle+size/
// handle+write/size+write 464/479/354; all three/four 462/461. The 354 conditional flag
// form is match-only and rejected. Complete size+write/size/write/nested value producers
// 426/426/367/426; original completion producers and all observations restored.
// Round 217: Header-validator inline/noinline/inline-out-of-line declarations 367/367/367;
// with cursor-end bounds 375/375/375; with repeated completed-count value 362/362
// fast. Five-point exact-length forms are below the retention threshold; all declarations and body restored.
// Round 240: One shared full-width priority ordinal, signed/unsigned, with state
// first and/or constant true result first: eight forms 367. Request source,
// dependency bound and write options remain; all original input reads restored.
// Round 162: State-arm success results bool/byte/short/long 531/559/627/637; the same declarations
// first 531/559/627/637. All shrink the aligned frame to 0x108 versus 0x110. Original success
// lifetime, all state arms and observations restored.
// Round 38: Native pending request reused by path/copy, conditional value/assignment/const/memcpy
// and case assignment/memcpy 511 each fast at 1728 bytes; entry assignment/memcpy 507 each
// at 1732 bytes. Original request flow, descriptor lifetimes and complete body restored.
// Round 50: pending request as path input 161; call-first pending input 430; transfer buffer local
// after/before resize 165/519; one-use handle locals in states 7/9/14 151/317/153 and states 7+14
// 143 (one-use field copies, not kept without a match). Opening body restored.
// Round 25 (lane 17, fast): state 19's buffer release before the slot clear 43 (the -1 store then sinks to the
// handle store); state 0 call-first 432, with a memcpy copy 432, and with 0x214db0's file name as a ternary
// (still matching) 432: `mov esi, edi` stays; state 11's clamp as min-ternaries in three operand orders 37/37/37,
// `chunk = limit; if (chunk > remaining)` 37, `chunk = remaining; if (remaining > limit)` 38 and `if (limit < chunk)`
// 38 (chunk shares remaining's register as in retail, the scratch registers rotate by one); a state 7 header
// pointer local 37, without the file local 334; state 16's copy as a struct assignment 37. Lead: with the header
// and the slot as members of one struct (diagnostic) and the release first, 35: the slot store stays ahead of
// the header clear as in retail, so 0x55b4f0..0x55c02c look like one struct (its users span five files).
// Single-variable clamps `chunk = total - completed; if (...) chunk = limit;` with `chunk > limit` 38,
// `limit <= chunk` 37 (retail's `cmp limit, chunk; jg` shape, registers still rotated: ours colours the limit
// first) and `limit < chunk` 38.
// Round 20 (lane 18, fast): state 19 with the buffer release before the slot clear (retail's order) 43
// (ours then sinks the NONE store past both clears to share the handle's NONE, where retail keeps it right
// after the call; retail's cache-copy globals 0x55b4f0..0x55c02c look like one structure, which would keep
// that store in place); state 11's clamp as `remaining < limit ? remaining : limit` 37 (same code), as the
// chunk set to the remaining bytes and clamped in place 38 (the chunk stays in ECX where retail keeps it in
// EAX); state 16 with the flag store before the header clear 37; the header copy as a struct assignment 37.
// @retail 0x2141f0
bool function_2141f0(void)
{
	long priority = g_448fd4[g_55be24[0].priority];
	long state = g_55bd04;
	bool success = true;

	switch (state)
	{
	case 0:
		if (g_55be24[1].map_name[0] &&
			!cache_copy_complete(g_55be24[1].map_name) &&
			map_location_get(g_55be24[1].map_name) != 3)
		{
			g_55be24[0] = g_55be24[1];
			map_file_path_get(g_55be24[0].map_name, g_55bd21);
			state = 1;
		}
		memset(&g_55be24[1], 0, sizeof(g_55be24[1]));
		g_55bd08 = 0;
		break;
	case 1:
		g_55bd00 = function_1a0b40(g_55bd21, 1, 0, 1, 1, priority, &g_55bd14, &g_55bd0c);
		state = 2;
		break;
	case 2:
		success = g_55bd14.handle != INVALID_HANDLE_VALUE;
		state = 3;
		break;
	case 3:
		memset(&g_55b4f0, 0, sizeof(g_55b4f0));
		g_55bd00 = function_1a0f10(g_55bd14, &g_55b4f0, 0x800, 0, 1, priority, &g_55bcf0, &g_55bd0c);
		state = 4;
		break;
	case 4:
		{
			file_ref_clock_time(&g_55b4f0.last_used);
			strncpy(g_55b4f0.path, g_55bd21, 0x100);
			g_55b4f0.path[0xff] = 0;
			success = g_55bcf0 == 0x800 && cache_header_verify((s_cache_header const *)&g_55b4f0);
			if (success)
			{
				for (long type = 0; type < 3; type++)
				{
					if (g_55b4f0.dependencies_required[type])
					{
						long slot = cache_file_slot_from_type(type);

						if (!g_557c90[slot].header.path[0] ||
							CompareFileTime(&g_557c90[slot].header.source_time, &g_55b4f0.dependency_times[type]) != 0)
						{
							success = false;
						}
					}
				}
			}
			state = 5;
		}
		break;
	case 5:
		g_55bd00 = function_1a15f0(g_55bd14, 1, priority, (dword *)&g_55bcf8.total, &g_55bd0c);
		state = 6;
		break;
	case 6:
		{
			bool size_known = g_55bcf8.total != (dword)NONE;
			success = size_known && g_55bcf8.total > 0x800;
		}
		g_55c02c = 0;
		state = 7;
		break;
	case 7:
		{
			long slot = cache_file_choose_slot(g_55b4f0.type, g_55b4f0.size);
			if (slot != NONE)
			{
				s_cache_file *file = &g_557c90[slot];

				g_55bd10 = slot;
				memset(&file->header, 0, sizeof(file->header));
				g_55bd00 = function_1a1050(file->file, &file->header, 0x800, 0, s_file_write_flags(0), 1, priority, &g_55bcf0, &g_55bd0c);
				state = 8;
			}
		}
		break;
	case 8:
		success = g_55bcf0 == 0x800;
		state = 9;
		break;
	case 9:
		{
			s_cache_file *file = &g_557c90[g_55bd10];

			g_55bd00 = async_flush_file(file->file, 1, priority, &g_55bd0c);
		}
		state = 10;
		break;
	case 10:
		g_55bcf0 = 0;
		g_55bcf8.completed = 0x800;
		state = 11;
		break;
	case 11:
		cache_copy_buffer_resize(g_448ff0[g_55be24[0].priority]);
		if (g_55bd18)
		{
			long remaining = g_55bcf8.total - g_55bcf8.completed;
			long chunk = g_55bd1c > remaining ? remaining : g_55bd1c;
			long slot = g_55bd10;
			g_55bcf4 = chunk;
			g_55bd00 = async_copy_position(g_55bd14, g_557c90[slot].file, (void *)g_55bd18,
				chunk, g_55bcf8.completed, g_55bcf8.completed, 1, priority, &g_55bcf0, &g_55bd0c);
			state = 12;
		}
		else
		{
			state = 11;
		}
		break;
	case 12:
		success = (dword)g_55bcf4 == g_55bcf0;
		g_55c02c ^= checksum_buffer_mmx((void const *)g_55bd18, g_55bcf0);
		if (success)
		{
			g_55bcf8.completed += g_55bcf0;
			if ((dword)g_55bcf8.completed >= g_55bcf8.total)
			{
				success = g_55c02c == g_55b4f0.checksum;
				state = 13;
			}
			else
			{
				state = 11;
			}
		}
		if (g_55bd20)
		{
			cache_copy_buffer_release();
			g_55bd20 = false;
		}
		break;
	case 13:
		{
			s_cache_file *file = &g_557c90[g_55bd10];

			g_55bd00 = async_flush_file(file->file, 1, priority, &g_55bd0c);
		}
		state = 14;
		break;
	case 14:
		{
			s_cache_file *file = &g_557c90[g_55bd10];
			file_ref_clock_time(&g_55b4f0.last_used);
			s_file_handle handle = file->file;
			g_55bd00 = function_1a1050(handle, &g_55b4f0, 0x800, 0, s_file_write_flags(0), 1, 0, &g_55bcf0, &g_55bd0c);
			state = 15;
		}
		break;
	case 15:
		success = g_55bcf0 == 0x800;
		state = 16;
		break;
	case 16:
		memcpy(&g_557c90[g_55bd10].header, &g_55b4f0, sizeof(g_55b4f0));
		memset(&g_55b4f0, 0, sizeof(g_55b4f0));
		g_55bd08 = 1;
		state = 17;
		break;
	case 17:
		state = 18;
		break;
	case 18:
		if (g_55bd14.handle != INVALID_HANDLE_VALUE)
		{
			g_55bd00 = function_1a1550(g_55bd14, 0, 6, &g_55bd0c);
		}
		state = 19;
		break;
	case 19:
		{
			s_cache_copy_request request = g_55be24[0];

			g_55bd10 = NONE;
			cache_copy_buffer_release();
			memset(&g_55b4f0, 0, sizeof(g_55b4f0));
			memset(&g_55be24[0], 0, sizeof(g_55be24[0]));
			g_55bd21[0] = 0;
			g_55bd14.handle = INVALID_HANDLE_VALUE;
			switch (g_55bd08)
			{
			case 1:
				cache_copy_forget(request.map_name);
				break;
			case 3:
				if (!cache_copy_complete(request.map_name))
				{
					map_copy_request(request.map_name, request.priority);
				}
				break;
			}
			state = 0;
		}
		break;
	default:
		__assume(0);
	}
	if (success)
	{
		g_55bd04 = state;
	}
	else
	{
		cache_copy_record_use(g_55bd21);
		g_55bd04 = 17;
		g_55bd08 = 3;
	}
	return success;
}
