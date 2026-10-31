#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"
#include "unknown_08b110.h"
#include "unknown_096ed0.h"
#include <stdlib.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

long replication_table_get_chain(s_handle_peers *peers, long handle, long *handles);
long replication_table_find_in_chains(s_handle_peers *peers, long *handles, long handle);
void replication_table_release(s_handle_peers *peers, long handle);
void replication_table_release_chain(s_handle_peers *peers, long count, long const *handles);

// score 55 (was 55): frame 0x48, receiver EBP and pending counter ESI agree; selector,
// cursor/entry/query homes and load scheduling remain. Earlier 170: the peers table is read through a local; the
// index is read inside the enabled test, the cursor is declared under the index test, advanced
// before the stores and the size is one expression, so retail's prologue and output block match.
// Left: our cursor, entry pointer and item-priority slots sit 4 bytes below retail's and our
// chain loop keeps its counter in a stack slot instead of ESI.
// chain loop has one count test. Left: receiver EDI/EBP (ours keeps the index in ebx, retail spills it
// and reloads it each iteration), so every callee-saved register rotates.
// Lane 8 r64 (199 = output cursor local, no `<= 0` guards before the chain loops, `priority = 0.0f`
// for a blocked active chain, `entries[index].field` in full, for loop with continue): cursor plus
// guards 220 (output slot reused); plus active `priority = 0.0f` 216 (814 bytes); size as one
// expression with `(handles_count > 1 ? 2 : 0)` 202 (retail's setle/dec/and, but the cursor gets a
// +4 bias); output indexed by count 207 (same bias); `out = cursor++` per entry 239, declared before
// the loop 236 (aligned 134: no bias, cursor at retail's spot, registers rotated); all 23 other
// orders of the type/count/priority/size declarations 236 (identical code).
// Earlier baseline: Frame 0x48 agrees; receiver EDI/EBP and callback/output/blocked homes remain.
// Earlier 217 from 217: Frame 0x48 agrees; receiver and aggregate/blocked homes remain.
// Earlier 217 from 217: frame 0x48 agrees; receiver EDI/EBP and count,
// priority, size and blocked homes remain. Both redundant count guards removed
// 249; separate native arrays 249; distinct counter names 249; result-home names
// 249; arrays and counter names 249; pending-only guard 242; active-only 220.
// Earlier 217 from 217: Frame 0x48 agrees; receiver/output-slot/priority homes remain. Active priority result 228; mutable cursor 250; native entry/peer/both arrays 236/266/238; shared handle buffer 220; linked for 217; query enum 217; cursor plus priority 259; unmasked arm first 216.
// Frame 0x48 agrees; receiver EDI/EBP and query/blocked homes remain.
// Frame 0x48 agrees; receiver/output-slot/priority homes remain. Active priority result 228; mutable cursor 250; native entry/peer/both arrays 236/266/238; shared handle buffer 220; linked for 217; query enum 217; cursor plus priority 259; unmasked arm first 216.
// State snapshot 217; direct handled states 247; default chain count 238;
// invalid-priority-first arms 221; state/default/arm combination 238.
// Entry-scope guarded arrays 217; shared blocked workspace 217; shared chain
// index 217; shared callback outputs 224; all-ready predicate 220.
// Priority/size/both before pending lookup 221/218/221; active priority/size/both 217/217/217;
// both priorities/all four early 221/221.
// Remaining capacity 262; failed-query counts 239; size accumulation first 229;
// separate chain size 239; capacity/count/size combination 272.
// Pending/active/both counted while 217/217/294; pending/active unsigned chain index 217/217; shared dispatch and chain aggregation 340.
// Round 111: Positive pending/active/both equality termination 217/217/217; active state first 253; Boolean-OR blocked reduction 264.
// Pending/active/both separate chain lengths 219/219/221;
// pre-query peer flags 229; flags plus separate lengths 254.
// Pending/active/both callback outputs before traversal 217/217/217; both with guarded
// output cursor 250; both with emitted-count output indexing 250 (fast); all restored.
// Guarded aggregate priority/size/emitted-count/query-count names and all four together
// 217/217/217/217/217; receiver and callback-output homes remain.
// Round 115: Guarded distinct indices 217; separate blocked names 217; separate callback-output names 217; aggregate declarations first 217; guarded index/blocked names together 217. Opening body restored.
// Pending/active/both invalid-query continues 229/217/229; zero-state exit 217; output-failure exit 217; initialized handle arrays 297; pending/active selector after lookup 219/217. Receiver and callback/output/blocked homes remain.
// Disabled-entry exit 239; empty-list exit 239; both admission exits with later count 246; those
// exits with advancing cursor 244; those exits with active zero-priority join 243. All reach 814
// bytes but regress.
// Round 357: Simple pending/masked-active/both arms first 248/216/246; all query kinds
// after their queries 275; single-handle counts after callbacks 275. Opening body restored.
// Round 165: Pending/active/both byte blocked predicates 217/217/217;
// pending/active/both dword 247/228/247; mixed dword/byte and byte/dword 247/228.
// Original Boolean predicates, all guards and executable body restored.
// Round 165 follow-up: Unsigned handle/link/both domains 217/217/217; readonly
// entry/peer/both views 217/217/217; unsigned handle with both readonly views 217;
// all domains/views 217. Original signed locals and mutable views restored.
// Enabled/nonempty count initialization 230/219; narrow pending/active array names
// 217/217; pending-before-ready/active-before-waiting names 217/217; enabled count
// with pending-before-ready 230; nonempty count with active-before-waiting 219.
// Callback priority/size names before handle homes, pending/active combinations
// and after-handle query names: 217 each; receiver and callback homes remain.
// Round 234: Native state dispatch, either case order, with original pointers,
// entry reference, peer reference and both references: 244 each (816 bytes).
// Original guarded callback/output graph and executable body restored.
// Native state with direct entry/entry-plus-peer/peer fields 236/238/266; native flags
// with direct entry/peer 239/229; both snapshots with direct entry/entry-plus-peer/peer
// 239/239/229. Original entry/peer cursors, field reads and callback/output homes restored.
// Round 188: Chain-query inline/noinline/forceinline/inline-out-of-line declarations, then each
// with native word state observed once:
// 217,217,217,217,217,217,217,217 fast; reduced control 217, full 217. Original executable source restored.
// Round 191: Native two-use payload mask at active gate, with original/cursor/
// conditional size/both output forms 217/250/214/244; before state selection,
// consistently serving applicable callbacks, 217/250/214/244. Three-point best
// restored; original field observations, output graph and executable body restored.
// Round 153: Once-computed validity with readiness AND pending/active/both 233/220/240; blocked OR
// pending/active 249/232; AND-OR/OR-AND 249/251; both readiness byte/long 240/269. Original
// callback/reduction graph restored.
// Round 254: shared callback owner after peer/before peer/before handle 262/262/262;
// entry table alone/with owner 261/242; enabled/nonempty/entry-scope table with owner
// 246/244/248. Original live pointer observations and executable body restored.
// Round 193: Private pending/active/both chain maxima; each with private pending size;
// both maxima with conditional pending publication, without/with private size: 227/240/228/227/218/241/288/235. Original restored.
// Round 195: Pending/active reversed-relational and conditional-replacement maximum
// graphs, alone and in all pairs, 217 in eight forms. Equality/unordered choices retained;
// original body restored, with receiver and callback/output/blocked homes unresolved.
// Round 104: an entry pointer for the body's reads 216; also driving the loop step 216; size as one
// expression 202 (aligned count unchanged); count incremented first 209; the `index != NONE` guard that
// repeated the loop test removed 199 (identical code, kept).
// Round 31 lead (170 at 814 bytes, not kept: mixed reads and a repeated loop guard): a
// `s_handle_peers *peers = table;` local for the peer lookup and the three single-handle calls (the
// chain loops keep `table->`) puts every callee-saved register where retail has it (this EBP,
// peer/count EDI, table/index ESI, type EBX) 207; the output in 0x8b890's order (`out = cursor;
// count++; cursor++;`, size as one expression with `(handles_count > 1 ? 2 : 0)`) then gives retail's
// output block exactly 175; index read inside `if (unknown09)` and the cursor declared under `if
// (index != NONE)` with a `for (; ...)` loop gives retail's prologue 170. Left: in the first chain
// loop retail keeps the chain index in ESI, the vtable in EBX and spills type to [esp+0x34] (reloaded
// before the join); ours keeps type in EBX and spills the index, shifting the cursor/entry/item slots
// by 4. On that lead: type declared last/at function scope, one shared chain index, the first loop's
// item locals at block scope, the chain query through the local 170 each; `type = 3` after the loop
// 249 (782 bytes); type set only on the unblocked exit 228; the local in the loops too 216; a 600 s
// permuter run 158 only through a volatile read; an entry pointer for the reads 195. Without the local: the sibling output order 234,
// with the cursor inside the flag test 232; count renamed result 199 (identical).
// Round 250: two-use pending handle 199; pending array cursor 258; both 258;
// active array cursor 207; handle/active 207; both cursors 213; handle/both 213;
// all with cursors before chain queries 214. Original traversal restored.
// Round 169: Recovered receiver/output graph: selector, distinct chain-index and callback-output
// identities, all seven subsets 170; native dword selector 170. Pending selector/index
// interference and the four-byte cursor/entry/callback home shift remain.
// Conditional pending/active/both aggregate zero joins 170/170/170;
// post-query pending/active/both failure lifetimes 173/170/173; both joins with late/shared predicates 173/170.
// Complete per-use/table-local/traversal-local/owner-local roots on the EBP graph 231/228/228/222; original body restored.
// Round 173: native signed-byte/byte/short/word selector on the recovered EBP graph
// 216/216/200/200; complete post-query selector publication at those widths
// 278/278/266/266. Original selector, table observations and body restored; cursor/entry
// homes and pending selector/chain-index residency remain.
// Round 73: retail spills type around the first chain loop only (stored before the chain query, reloaded
// into EBX at the join) and keeps the counter in ESI and the vtable in EBX; ours keeps type in EBX and
// spills the counter. Renaming type (two names) or the chain counters (two names) 170 each; the first
// chain counter declared with its zero before `type = 3` 189.
// Round 396: real pending/active array cursors after/before lookup on the EBP graph
// 216,132,242,221,148,247,247,252; active post-lookup cursor kept at 132 plain, 823/814 bytes.
// Pending selector now spills at +0x20 versus +0x34; cursor/query homes remain four bytes low.
// Round 227: On the aligned receiver graph, early size reduction/conditional maxima/both
// 170/170/170; positive availability and its three combinations 171 each;
// all with a private pending total 218. Original reductions restored; selector/induction
// interference and the four-byte cursor/entry/callback shift remain.
// Round 175: On the recovered EBP graph, pending/active/both native chain-buffer names
// 170/170/170; two-use pending callback handle 170, with either/both buffers 170/170/170;
// complete graph with alternate buffer home order 170. Original executable body restored.
// Round 180: Pending/active selector constants after queries and single-item counts after
// callbacks, masks 1-7, then all with single-item selectors after callbacks:
// 172/170/172/229/230/229/230/229. Original executable body restored.
// Round 399: retained active-cursor graph, output/selector/input names in masks 1-7,
// all with callback-output names, native count/index names, those with selector/input,
// explicit remaining-count pointer walk and that walk with count/selector names: 132 each.
// Active remaining count still coalesces with selector at +0x20; complete body restored.
// Round 400: On the active-cursor graph, shared native array/blocked/callback outputs in masks
// 1-7, then all at traversal scope: 145/132/148/145/148/148/148/148. Original workspaces
// restored.
// Round 401: Active buffer endpoints with relational/equality bounds 153/153;
// pending endpoints 236/236; all four paired endpoint graphs 242. Each endpoint
// is formed after the signed positive-count test. Active endpoints recover cursor,
// selector and query homes but reverse the two buffers (834/814 bytes). Two native
// buffer-name orders with each active bound 153/153/153/153. Original restored.
// Round 182: Callback-argument cursor kept at 55; processed-count and endpoint forms regress; selector and query homes remain
// Trials 1-8: 55, 55, 216, 229, 208, 208, 101, 171.
// Trials 9-16: 57, 183, 185, 170, 185, 185, 185, 185.
// Trials 17-18: 153, 153.
// Lane 14 r6: the walking active cursor makes the compiler count the active chain down in a
// stack slot it shares with the pending chain's type spill, so that slot sorts first (more
// references); retail indexes the active chain (i in esi, count in edi), and its type spill is
// the last 4-byte slot below the handle arrays. Plain `handles[i]` in the active loop 170 (the
// pending loop then spills i and keeps type in ebx); type renamed kind/a_type/z_type 55 each;
// cursor renamed 55.
// Lane 14 r9: plain `handles[i]` in the active loop 170 (type then holds EBX through both chains, the
// pending counter spills); with one shared counter for both loops 170; with the pending type assigned
// after its loop 241; with the pending handle read once into a named local 170; that local on the
// cursor form 55 (same code).
// Round 95 (one-file harness, predictive here: indexed form 38 aligned = 170 in the build): the
// two chain counters trade places; whichever is in a register, the other loop loses. A short
// active index (spilled into the shared slot) gives back the pending loop's type spill, as the
// cursor does. With the active chain indexed: a pending item-handle local, an int pending
// index, type declared last, the blocked test first, the pending index declared at the top of
// its block 170 each; in the harness also int/dword/unsigned indices, swapped zero stores,
// pre-increment, blocked declared first, `size = size + item_size` and 36 random
// combinations of those, none with type spilled.
// Round 24 (lane 10): the pending loop's callback through the peers local 222 (822 bytes); the active
// loop's 253 (834 bytes); priority/size declared before handles_count/type 55 (same code); the active item
// priority at block scope 55; the active blocked flag declared before its handles 55.
// Round 45 (lane 9): with the active chain indexed (170) our allocator keeps type in EBX through the pending loop
// (vtable in ESI) and spills the pending counter; retail spills type around that loop only. On that form: the owner
// read into a loop-local 170; pending `type = 3` after the chain query 172; active `type = 4` after it 170; blocked
// declared before the handle arrays 170.
// Round 31: short active index 148; type declared at function scope 55 (identical); plain long active
// index 170 (814 bytes; the pending loop keeps type in EBX and spills its counter); pending loop also
// walked by a cursor 216; plain active index with function-scope type 170.
// Round 84: on the indexed active chain, separate pending-selector publication after the blocked
// test/always 226/241; peer callback 233; isolated control 170; owner before the loop 238;
// peer lookup, equality termination and iteration-local owner 170 each. Original body restored.
// Round 107: retail's slots follow the locals' names (cursor, entry, item locals, type last); ours put the
// active countdown's shared type slot first. The active counter renamed (z/u/a/aa/handle_index, one-file
// harness al 27 each; real handle_index 55); a named entry pointer for every field read 193 (831 bytes),
// with the active chain indexed 195 (815); a pending chain count assigned to handles_count after its loop
// 219 (814), with the active chain indexed 184 (796).
// Lane 14 r19 (fast `0x98150 0x89d20` reproduces the plain 55): indexed active loop with the item
// priority at the arm scope 170 (814 bytes; type keeps EBX through the pending chain, whose index
// spills); active cursor bounded by a remaining countdown 55 (same code); indexed loop bounded by its
// own count local 170; `type = 4` after the active chain query 170; blocked declared after `type = 4` 170.
// Round 14 (lane 2): retail keeps type in EBX and splits it only around the pending loop (stored to
// +0x34 before the chain query, reloaded at 0x98402 before the join) because that loop's call needs
// EBX for the vtable; with the active chain indexed ours keeps type in EBX, puts the vtable in ESI
// and spills the pending index. On the indexed form (fast, 0x89d20 with it): one function-scope
// `blocked` for both chains 170; `i != handles_count` in the pending/active loop 170/170; the pending
// item locals declared size first 170; on the cursor form, the same item order 55.
// Lane 16 r46: retail keeps type in EBX everywhere but the pending loop (stored to its home before the
// chain query, reloaded at the join), i.e. a split around that loop. On the indexed form: type renamed
// kind/z_type and the count renamed 170 each; a `short` pending index 198; the active owner in a
// loop-local 170; the pending size sum before the priority test 170. On the cursor form the cursor
// stepped in the `for` increment 132 (823 B).
// Round 135 (lane 4, fast with 0x89d20): `blocked` as a long 246 (821 bytes); the pending `blocked` declared
// after the chain query 61; the pending item locals declared before the loop 55 (same code); the active cursor
// non-const 55 (same code); the pending priority/size zeroed before `type = 3` and the chain query 65.
// Round 33 (m3 lane 11), fast with 0x89d20, on the cursor form: `type = 4` after the active chain
// query 58; `type = 3` after the pending chain query 60; the active cursor stepped in its own
// statement 61; the active item priority declared before its loop 55; the active size zeroed before
// its priority 54. Retail keeps type's home last (+0x34) with the cursor, entry and item slots 4 lower.
// Round 38 (m3 lane 10, fast): on the indexed active chain (170: active loop as retail; in the pending
// loop ours keeps type in EBX, the vtable in ESI and spills the index, where retail spills type and keeps
// the index in ESI and the vtable in EBX): the active callback through the peers local 218 (816 bytes);
// handles_count declared before type 170; type as a dword 170; pre-increment in both chain loops 170;
// the active arms as `if (item_priority <= 0.0f) blocked = true; else if (priority < item_priority)` 211.
// Round 55 (lane 12; a one-file LTCG harness with 0x89d20's file reproduces the build exactly): on the indexed
// active chain, the pending item locals at the arm scope, the pending counter declared before its `for`, the
// pending loop as a `while`, the active counter named j, both counters declared before their loops: 170 each
// (identical code: type keeps EBX, the pending counter spills). Diagnostic only (harness compile; no pragma in
// src): favour-size for this body 225/242 instructions, aligned 159 (much further from retail).
// Round 117 (fast with 0x89d20): the result count renamed `written` 55, the output cursor renamed `slot` 55, the
// active cursor renamed `a_cursor` 55 (names do not move the slots: same code each); the active cursor declared
// at the active arm's top 171 (832 bytes); pre-increment in the active loop 55 (same code).
// @retail 0x98150
long c_handle_table_450cd0::v1(long a1, long max_count, void *output)
{
	long count = 0;
	if (unknown09)
	{
		long index = unknown5024;
		if (index != NONE)
		{
			s_entry_450c94 *cursor = (s_entry_450c94 *)output;
			for (; index != NONE && count < max_count; index = entries[index].unknown0a)
			{
				if (entries[index].state)
				{
					long handle = entries[index].handle;
					s_handle_peers *peers = table;
					s_handle_peer *peer = &peers->peers[handle & 0x3ff];
					long type;
					long handles_count;
					real priority;
					long size;
					if (entries[index].state == 1)
					{
						if (peer->flags & 8)
						{
							long handles[4];
							bool blocked = false;
							type = 3;
							handles_count = replication_table_get_chain(table, handle, handles);
							priority = 0.0f;
							size = 0;
							for (long i = 0; i < handles_count; i++)
							{
								real item_priority;
								long item_size;
								table->owner->v3(handles[i], entries[handles[i] & 0x3ff].unknown04, a1, &item_priority, &item_size);
								if (item_priority > 0.0f)
									priority = priority > item_priority ? priority : item_priority;
								else
									blocked = true;
								size += item_size;
							}
							if (blocked)
								priority = 0.0f;
						}
						else
						{
							if (peer->flags & 0x10)
								continue;
							type = 1;
							handles_count = 1;
							peers->owner->v3(handle, entries[index].unknown04, a1, &priority, &size);
						}
					}
					else if (entries[index].state == 3)
					{
						if (peer->mask & (1 << shift))
						{
							if (peer->flags & 8)
							{
								long handles[4];
								bool blocked = false;
								type = 4;
								handles_count = replication_table_get_chain(table, handle, handles);
								priority = 0.0f;
								size = 0;
								long const *active_cursor = handles;
								for (long i = 0; i < handles_count; i++)
								{
									real item_priority;
									table->owner->v9(*active_cursor++, a1, &item_priority);
									if (item_priority > 0.0f)
										priority = priority > item_priority ? priority : item_priority;
									else
										blocked = true;
								}
								if (blocked)
									priority = 0.0f;
							}
							else
							{
								if (peer->flags & 0x10)
									continue;
								type = 2;
								handles_count = 1;
								peers->owner->v9(handle, a1, &priority);
								size = 0;
							}
						}
						else
						{
							if (!entries[index].unknown04)
								continue;
							type = 5;
							handles_count = 1;
							peers->owner->v8(handle, entries[index].unknown04, entries[index].unknown0c, a1, &priority,
								&size);
						}
					}
					else
						continue;
					if (priority > 0.0f)
					{
						s_entry_450c94 *out = cursor;
						count++;
						cursor++;
						out->unknown00 = *(long *)unknown04;
						out->index = index;
						out->unknown04 = priority;
						out->unknown0c = type;
						out->size = size + 14 * handles_count + 3 + (handles_count > 1 ? 2 : 0);
					}
				}
			}
		}
	}
	return count;
}

// @retail 0x991d0
bool function_991d0(c_handle_table_450cd0 *self, long handle, long size, void const *data)
{
	long index = handle & 0x3ff;
	bool result = false;
	if (self->entries[index].state)
	{
		long old_handle = self->entries[index].handle;
		long difference = ((dword)handle >> 28) - ((dword)old_handle >> 28) + 16;
		if (difference > 8)
			difference -= 16;
		if ((difference >= 0 && difference < 4) || difference <= -4)
		{
			self->table->owner->v11(old_handle, handle, size, data);
			s_handle_peers *peers = self->table;
			byte flags = peers->peers[old_handle & 0x3ff].flags;
			if ((bool)((flags >> 3) & 1) || (flags & 0x10))
			{
				long handles[4];
				long count;
				if ((bool)((flags >> 3) & 1))
					count = replication_table_get_chain(peers, old_handle, handles);
				else
					count = replication_table_find_in_chains(peers, handles, old_handle);
				for (long i = 0; i < count; i++)
					function_99690(self, handles[i], 0);
				replication_table_release_chain(self->table, count, handles);
			}
			else
			{
				function_99690(self, old_handle, 0);
				replication_table_release(self->table, old_handle);
			}
			result = true;
		}
	}
	else
		result = true;
	return result;
}

/* the release routine retail inlines here (allocation is handle_allocate in the header) */
static inline void free_block(void *block)
{
	long info;
	if (!g_4d87f8->allocator->get_info(block, &info))
		info = NONE;
	allocator_globals_release(g_4d87f8, block);
}

// @retail 0x99820
const char *function_99820()
{
	return "slayer-engine-globals";
}

__declspec(noinline) long function_98480(long handle, c_handle_table_450cd0 *self);

/* the next live handle after `handle` (NONE starts at the first), or NONE;
   retail keeps it out of line in has_pending_entity, which otherwise inlines it */
// @retail 0x98480
long function_98480(long handle, c_handle_table_450cd0 *self)
{
	long i;
	if (handle == NONE)
		i = 0;
	else
		i = (handle & 0x3ff) + 1;
	long result = NONE;
	for (; i < 0x400; i++)
	{
		if (self->entries[i].state == 1 || self->entries[i].state == 2)
		{
			result = self->entries[i].handle;
			break;
		}
	}
	return result;
}

__declspec(noinline) bool function_99640(c_handle_table_450cd0 *self, long handle);

// @retail 0x99640
bool function_99640(c_handle_table_450cd0 *self, long handle)
{
	long index = handle & 0x3ff;
	bool result = false;
	if (index >= 0 && (dword)index < 0x400)
	{
		if (self->entries[index].handle == handle)
			result = !self->table->peers[index].flag2 || (self->entries[index].state == 3 && !self->table->peers[index].flag1);
	}
	return result;
}

// @retail 0x995c0
void function_995c0(c_handle_table_450cd0 *self, long handle)
{
	long index = handle & 0x3ff;
	s_handle_peer *peer = &self->table->peers[index];
	s_handle_entry *entry = &self->entries[index];
	if (self->entries[index].state == 1 && !(self->entries[index].unknown10 & self->entries[index].state))
	{
		function_99690(self, handle, 0);
		self->unknown5038++;
		return;
	}
	if (self->entries[index].state == 1)
		function_99690(self, handle, 3);
	peer->mask |= (word)(1 << self->shift);
	if (entry->state == 3)
	{
		if (entry->unknown04 != 0)
			self->unknown5034--;
		self->unknown503c++;
	}
}
// @retail 0x98ac0
void function_98ac0(c_handle_table_450cd0 *self, long handle)
{
	s_handle_item *item = new s_handle_item;
	long index = handle & 0x3ff;
	if (item != 0)
	{
		item->next = self->node->items;
		self->node->items = item;
		item->handle = handle;
		item->kind = 1;
	}
	else
		self->unknown0a = 1;
	self->entries[index].unknown10 |= 1;
	function_99690(self, handle, 2);
}

// @retail 0x98b60
void function_98b60(c_handle_table_450cd0 *self, long handle)
{
	s_handle_item *item = new s_handle_item;
	if (item != 0)
	{
		item->next = self->node->items;
		self->node->items = item;
		item->handle = handle;
		item->kind = 2;
	}
	else
		self->unknown0a = 1;
	function_99690(self, handle, 4);
}

// @retail 0x98bf0
void function_98bf0(long handle, c_handle_table_450cd0 *self, dword mask)
{
	s_handle_record *record = new s_handle_record;
	long index = handle & 0x3ff;
	if (record != 0)
	{
		record->next = self->node->records;
		self->node->records = record;
		s_handle_record *found = 0;
		for (s_handle_node *node = self->head; node != 0; node = node->next)
		{
			for (s_handle_record *other = node->records; other != 0; other = other->next)
			{
				if (other->handle == handle)
				{
					found = other;
					break;
				}
			}
		}
		if (found != 0)
			found->link = record;
		record->handle = handle;
		record->mask = mask;
		record->link = 0;
	}
	else
		self->unknown0a = 1;
	self->entries[index].unknown04 &= ~mask;
	self->entries[index].unknown0c = time_now();
	if (self->entries[index].state == 3 && self->entries[index].unknown04 == 0)
		self->unknown5034--;
}
/* ---- the sender state of 0x450d1c (see c_vtable_450d1c in unknown_08b110.h) ---- */
struct s_sender_node;

class c_sender_manager
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3(s_sender_node *node) = 0;
};

struct s_sender_node
{
	byte unknown00[0x18];
	void *block;
	byte unknown1c[4];
	dword active_mask;
	dword done_mask;
	s_sender_node *next;
};

struct s_sender_owner
{
	byte unknown00[4];
	c_sender_manager *manager;
	byte unknown08[0x40];
	long count;
	s_sender_node *head;
};

struct s_sender_link
{
	s_sender_node *node;
	s_sender_link *next;
};

struct s_sender_request
{
	long key;
	s_sender_link *links;
	s_sender_request *next;
};

struct s_sender
{
	byte unknown00[0xc];
	long player;
	s_sender_request *requests;
	long request_count;
	s_sender_owner *owner;
	long unknown1c;
	long pending;
	long unknown24;
};

static __forceinline void function_96ed1(void *arg_0)
{
	long local_0;
	if (!g_4d87f8->allocator->get_info(arg_0, &local_0))
		local_0 = NONE;
	c_allocator *local_1 = g_4d87f8->allocator;
	s_allocator_globals *local_2 = g_4d87f8;
	local_1->release(arg_0, NONE);
	if (arg_0 != 0)
		local_2->count--;
}

// @retail 0x96ed0
void function_96ed0(s_sender *self)
{
	s_sender_request *request = self->requests;
	self->requests = 0;
	if (request != 0)
	{
		do
		{
			s_sender_link *link = request->links;
			s_sender_request *next_request = request->next;
			if (link != 0)
			{
				do
				{
					s_sender_node *node = link->node;
					s_sender_link *next = link->next;
					node->done_mask &= ~(1 << self->player);
					free_block(link);
					link = next;
				}
				while (link != 0);
			}
			free_block(request);
			request = next_request;
		}
		while (request != 0);
	}

	s_sender_node *node = self->owner->head;
	if (node != 0)
	{
		do
		{
			s_sender_node *next = node->next;
			dword mask = 1 << self->player;
			if (node->active_mask & mask)
			{
				s_sender_owner *owner = self->owner;
				node->active_mask &= ~mask;
				if (node->active_mask == 0)
				{
					owner->manager->v3(node);

					/* 0x89e70 and 0x89eb0, which retail inlines here */
					s_sender_node **link = &owner->head;
					if (*link != 0)
					{
						do
						{
							s_sender_node *current = *link;
							if (current == node)
							{
								*link = node->next;
								break;
							}
							link = &current->next;
						}
						while (*link != 0);
					}
					owner->count--;
					if (node->block != 0)
						function_96ed1(node->block);
					free_block(node);
				}
			}
			node = next;
		}
		while (node != 0);
	}
	self->pending = 0;
	self->unknown1c = 0;
	self->unknown24 = 0;
}
/* ---- the sorting of 0x97ab0 ---- */
struct s_priority_entry_raw { long v[5]; };

struct s_priority_entry
{
	long child;
	real priority;
	long size;
	long unknown0c;
	long index;
};

// @retail 0x97a80
int __cdecl function_97a80(const void *a, const void *b)
{
	real difference = ((const s_priority_entry *)b)->priority - ((const s_priority_entry *)a)->priority;
	if (difference > 0.0f)
		return 1;
	if (difference < 0.0f)
		return -1;
	return 0;
}
/* retail inlines the stream initialization here */
static inline void stream_initialize(s_bitstream *stream, void *buffer, long size, long mode)
{
	stream->mode = mode;
	stream->data = (byte *)buffer;
	stream->size_in_bytes = size;
	stream->unknown08 = 1;
	memset(stream->data, 0, stream->size_in_bytes);
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (stream->mode == 1)
	{
		stream->unknown2c = 0;
		stream->unknown30 = 0;
	}
	else if (stream->mode == 3 || stream->mode == 4)
	{
		if (function_1959c0(stream, 0x20) == 0x64656267)
			stream->error = true;
		else
		{
			stream->bit_position = 0;
			stream->error = false;
		}
	}
}

/* retail inlines the end of a stream here: its size is rounded up to the
   alignment, and its bits are appended to the output */
static __forceinline void function_97ab1(s_bitstream *arg_0, s_bitstream *arg_1)
{
	long *local_0 = &arg_0->size_in_bytes;
	long local_1 = local_0[3];
	long local_2 = local_0[1];
	long local_3 = (local_1 + 7) / 8;
	local_0[0] = local_3;
	long local_4 = local_3 % local_2;
	if (local_4)
		local_0[0] = local_3 - local_4 + local_2;
	local_0[2] = 2;
	function_1955d0(arg_1, (void *)local_0[-1], local_1);
	arg_1->unknown2c += local_0[10];
	arg_1->unknown30 += local_0[11];
}

// @retail 0x97ab0
bool c_aggregate_450cf4::v4(long a1, s_bitstream *out, long a3, long reserved_bits)
{
	s_priority_entry_raw list_raw[2000];
	s_priority_entry *list = (s_priority_entry *)list_raw;
	dword buffers[3][0x180];
	s_bitstream streams[3];
	long total = 0;
	bool overflowed = false;
	long source_value = source->v0();
	for (long i = 0; i < 3; i++)
	{
		c_handle_table_450cd0 *child = children[i];
		if (child != 0 && child->v0())
			total += child->v1(source_value, 2000 - total, &list[total]);
	}
	qsort(list, total, sizeof(s_priority_entry), function_97a80);
	if (total > 0x100)
		total = 0x100;

	long budget = (out->size_in_bytes << 3) - out->bit_position - reserved_bits;
	long k = 0;
	do
	{
		streams[k].mode = 0;
		streams[k].bit_position = 0;
		streams[k].checkpoint_count = 0;
		streams[k].error = false;
		k++;
	}
	while (k < 3);
	budget -= unknown24;
	for (long i = 0; i < 3; i++)
		stream_initialize(&streams[i], buffers[i], 0x600, 1);

	if (a1 != NONE)
	{
		for (long i = 0; i < total; i++)
		{
			long *local_0 = &list_raw[i].v[1];
			long which = local_0[-1];
			if (local_0[1] <= budget)
			{
				c_handle_table_450cd0 *child = children[which];
				s_bitstream *stream = &streams[which];
				if (child != 0)
				{
					long before = stream->bit_position;
					child->v3(local_0[3], local_0[2], source_value, a1, (long)stream, (stream->size_in_bytes << 3) - before - budget);
					budget += before - stream->bit_position;
					if (budget <= 0)
						break;
				}
			}
			else
				overflowed = true;
		}
	}

	for (long i = 0; i < 3; i++)
	{
		c_handle_table_450cd0 *local_1 = children[i];
		if (local_1)
			local_1->v4(a1, &streams[i]);
	}

	for (long i = 0; i < 3; i++)
	{
		if (children[i] != 0)
			function_97ab1(&streams[i], out);
	}	return overflowed;
}
/* the part of 0x992d0 that points the records of earlier nodes, which referred
   to a record about to be freed, at what that record referred to */
static inline void records_relink(s_handle_node *head, s_handle_node *stop, long handle, s_handle_record *record)
{
	s_handle_node *other = head;
	while (other != 0 && other != stop)
	{
		s_handle_record *candidate = other->records;
		other = other->next;
		for (; candidate != 0; candidate = candidate->next)
		{
			if (candidate->handle == handle && candidate->link == record)
			{
				candidate->link = record->link;
				return;
			}
		}
	}
}

// @retail 0x992d0
void c_handle_table_450cd0::v8(long handle, bool flag)
{
	s_handle_node **head_link = &head;
	s_handle_node **link = &head;
	s_handle_node *node = *link;
	while (node != 0)
	{
		if (node->handle == handle)
		{
			s_handle_record *record = node->records;
			if (record != 0)
			{
				do
				{
					long record_handle = record->handle;
					s_handle_record *next_record = record->next;
					long index = record_handle & 0x3ff;
					s_handle_peer *peer = &table->peers[index];
					if (!flag)
					{
						dword mask = record->mask;
						for (s_handle_node *other = node->next; other != 0; other = other->next)
						{
							for (s_handle_record *candidate = other->records; candidate != 0; candidate = candidate->next)
							{
								if (candidate->handle == record_handle)
									mask &= ~candidate->mask;
							}
						}
						if (mask != 0 && entries[index].unknown04 == 0 && entries[index].state == 3)
						{
							if (!(peer->mask & (1 << shift)))
								unknown5034++;
						}
						entries[index].unknown04 |= mask;
					}
					else
						unknown5030++;
					records_relink(*head_link, node, record_handle, record);
					free_block(record);
					record = next_record;
				}
				while (record != 0);
			}

			s_handle_item *item = node->items;
			if (item != 0)
			{
				do
				{
					long item_handle = item->handle;
					s_handle_item *next_item = item->next;
					long offset = (item_handle & 0x3ff) * sizeof(s_handle_peer);
					s_handle_peer *peer = (s_handle_peer *)((byte *)table->peers + offset);
					if (item->kind == 1)
					{
						if (!flag)
						{
							if (peer->mask & (1 << shift))
								function_99690(this, item_handle, 3);
							else
								function_99690(this, item_handle, 1);
						}
						else
						{
							function_99690(this, item_handle, 3);
							unknown5028++;
						}
					}
					else if (!flag)
						function_99690(this, item_handle, 3);
					else
					{
						function_99690(this, item_handle, 0);
						peer->mask &= ~(1 << shift);
						s_handle_peers *peers = table;
						if (((s_handle_peer *)((byte *)peers->peers + offset))->mask == 0)
						{
							peers->owner->v12(item_handle);
							byte *flags = (byte *)peers->peers + offset;
							*flags &= 0xfe;
						}
						unknown5038++;
					}
					free_block(item);
					item = next_item;
				}
				while (item != 0);
			}

			*link = node->next;
			free_block(node);
			return;
		}
		link = &node->next;
		node = *link;
	}
}
// @retail 0x98cf0
long c_handle_table_450cd0::v5(dword a1, s_bitstream *stream, long max_blocks, s_handle_block *blocks, long *count_out)
{
	long error = 0;
	long count = 0;
	if (unknown0a)
		error = 2;
	else
	{
		s_handle_block *cursor = blocks;
		do
		{
			dword type = function_1959c0(stream, 3);
			long n;
			if (type >= 6)
			{
				error = 3;
				break;
			}
			if (type != 0)
			{
				if (type > 2 && type <= 4)
				{
					n = function_1959c0(stream, 2) + 2;
					if (n < 1 || n > 4)
					{
						error = 3;
						break;
					}
				}
				else
					n = 1;
			}
			else
				n = 0;
			if (error != 0)
				break;
			if (count >= max_blocks && n > 0)
			{
				error = 3;
				break;
			}
			if (n == 0)
				break;

			s_handle_block_info info;
			s_handle_block_data data;
			memset(&info, 0, sizeof(info));
			info.count = n;
			long produced = 0;
			for (long i = 0; i < n; i++)
			{
				if (error != 0)
					break;
				dword low = function_1959c0(stream, 10);
				dword item = low | ((byte)function_1959c0(stream, 4) << 28);
				long index = item & 0x3ff;
				info.items[i] = item;
				switch (type)
				{
				case 1:
				case 3:
					error = table->owner->v1(item, &info.b[i], &info.c[i], 8, &produced, data.v, stream);
					break;
				case 2:
				case 4:
					break;
				case 5:
					if (entries[index].handle == (long)item && entries[index].state == 3)
					{
						s_handle_peers *peers = table;
						error = peers->owner->v6(item, info.c, 8, &produced, data.v, stream);
					}
					else
						error = 2;
					break;
				default:
					error = 3;
					break;
				}
			}

			if (error == 0)
			{
				count++;
				s_handle_block *block = cursor;
				cursor++;
				block->type = type;
				block->index = info.items[0];
				block->info = info;
				block->count = produced;
				block->data = data;
			}
			else
			{
				for (long k = 0; k < produced; k++)
				{
					if (data.v[k * 2 + 1] != 0)
					{
						free_block(data.v[k * 2 + 1]);
						data.v[k * 2 + 1] = 0;
					}
				}
			}
		}
		while (error == 0);
	}
	*count_out = count;
	return error;
}
