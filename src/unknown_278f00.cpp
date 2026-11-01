// @flags /O2 /Ob1 /Gr /arch:SSE
#include "unknown_1cec30.h"

struct s_278f00
{
	byte field_0[0x3c];
	hkRigidBody **field_3c;
	long field_40;
	byte field_44[0x6a];
	bool field_ae;
};

struct s_278f01
{
	s_278f00 **field_0;
	long field_4;
	long field_8;
};

struct s_278f02
{
	byte field_0[3];
	byte field_3;
	byte field_4[4];
	s_havok_object *field_8;
};

struct s_278f03
{
	byte field_0[0xc0];
	word : 5;
	word field_c0_5 : 1;
	word field_c0 : 1;
	word : 9;
	byte field_c2[0x12];
	long field_d4;
};

extern hkWorld *g_51e9a4;
extern bool g_55e4fc;
extern bool g_47f05b;
void __stdcall function_1c3770(long arg_0, dword arg_1);
void function_146bf0();
void function_1d1260(s_havok_component *arg_0);
void __stdcall function_b8540(long arg_0);
void __stdcall function_b8890(long arg_0);

bool function_a7670(long object_index);

// score 181 (was 181): frame 0x424 and retail's length agree; left are the scratch-register phase at entry
// (retail xor eax), the homes of the list/island/index/count slots and the inner-loop induction, the
// admission flag (retail BL, ours a stack byte) and the cursor cleanup loop (retail indexed).
// list reference 212; island reference 212.
// body reference 212; header reference 212.
// unsigned object handle 212; list and island references 212.
// first standard search 244; second standard search 282.
// existing component-delete helper 341; existing detach helper 464; inline declarations
// 341/464; category byte comparisons 266; signed-byte category 212, membership 212, both 212.
// Outer selector byte/short/word/dword 219/266/266/212; byte/short with branched
// selection 245/291; byte/short with indexed cleanup 226/269. Original restored.
// Distinct first/second membership ordinals: byte/long, short/long, word/long 281 each;
// long/byte 278; long/short and long/word 267; byte/byte 289; short/short 290.
// Round 225: First/second array list/island ownership pairs 01/02/10/11/12/20/21/22: 212 each.
// Round 227: Remove redundant first/second/both pre-retry initializations 212/212/212; split
// first/second/both insertion increments 212/212/212; both removals with second/both split increments
// 212/212. Original resets and body restored.
// Compound-termination first/second index 266/212; both indices 266; first/second
// begin-end cursors 291/279; both cursors 319; index/cursor 271; cursor/index 291.
// Round 330: Full-width unsigned first/second/search domains, seven combinations 212 each; all three with
// both processing ordinals 212. Signed body/list bounds, retry and call cycle retained; original restored.
// Round 178: Reprocess islands with explicit advance on unflagged slots, for-loop selector/membership
// combinations 270/270/241/239; while-loop combinations 270/270/241/239. All reach 954 bytes but regress;
// original induction restored. List/index/second-count homes and cleanup scheduling remain.
// Round 235: Force-inline declaration at component-delete/detach boundaries 341/464;
// with cleanup helper 411/532; with branched list selection 350/469; both boundaries
// with branched selection 404/538. All helpers stay out of line; original body restored.
// Complementary category bool/byte/short/long with signed second count 212/212/271/273;
// unsigned second-count counterparts 212/212/271/273. Wide category frame 0x428; all restored.
// Round 194: Shared array/header/group alone, all three pairs and all three together 212 each; all native
// records with branched list selection 244. Original body and views restored.
// Unsigned first/second/both handle arrays 212/212/212; first/second/both
// arrays with unsigned classification 212/212/212; complete unsigned processing 212;
// unsigned arrays/processing with original signed classification 212. Original storage restored.
// Round 246: category after first/both membership scans with early header 252/291;
// move header too 306/302; indexed-cleanup counterparts 257/296/312/311.
// Native byte flag unit with unchanged/bit-5/bit-6/both writes 212 each;
// signed-byte flag unit with those four write forms 255 each. Original restored.
// Lane 4 r98: list chosen per arm (global read in each, as retail) 244; descriptive local names give
// identical code (212); plain indexed cleanup loop (retail's inc/cmp count form) 217 (964 bytes); word
// bit-5/bit-6 fields in s_278f03 fuse both flag writes into retail's `or/and byte ptr` but 939 bytes, 253;
// a named bool for the mode-4 test folds away (212). Retail evaluates that test's second half as
// `setne/test` (as 0x1c4040 does): an inline bool helper shared with 0x1c4040 is untried.
// Round 224: First-list processing cursors, read-only/mutable and consumed/body-end steps,
// original/branched selection 283/221/283/221/272/266/272/266. Original indexed processing and call cycle restored.
// Header-byte-offset induction long/dword 276 each, short/word/byte 280 each;
// direct descending list selector long/short/signed-byte 251/265/252. Restored.
// Round 102: the mode-4 test through the existing 0xa7670 (inlined with retail's setne/test) 281 alone;
// with word bit-5/bit-6 fields written through them 250 (954 B); with the list chosen per arm 181,
// kept. On it: descriptive names 181; list declared at function scope 181; indexed cleanup loop 196;
// found flag declared before the admission flag 274; the inner bound read from the field 248.
// Round 63: list chosen by if/else arms (long selector) 244; ternary of the two list addresses 244;
// descriptive names for the outer locals 212; second scan with its own found flag 269 (flag materialized,
// not threaded); index comparison before the first-scan flag 213. Diagnostics, not kept: the predicted-copy
// test as a one-line inline bool helper 212, as a single-exit helper 281 (gives the setne/test).
// Round 14 (lane 10): the second scan setting the same found flag (`if (!found)` then jump-threads to the
// iteration end, and the admission flag takes BL, both as retail) 260; with the inner bound read from
// local_5->field_40 (no count local; the retry head becomes retail's) 231; plus the plain indexed cleanup
// loop 233 (957 bytes). Lockstep counts rise, but aligned differences fall from 133 to 60; what remains is
// spill-slot order (retail: cursor, second count, counter, list, body, selector, index upward), the outer
// index stored at each definition (retail keeps it in ECX on the skip path), the record address sunk below
// the `local_9 > 0` branch and loop padding. On that base: body in an if-block 233; index declared outside
// the loop 233; the category as two comparisons 235.
// Round 104: retail tests the second scan's hit first (threaded straight past the insert), then the first
// found flag (CL), then the admission flag (BL). Index compare first 244; a second-scan found flag tested
// first 260 (retail's scan shape and BL/CL exactly, 83 aligned differences against 93, but the header address
// is folded after the `local_9 > 0` branch where retail computes it before, so the lockstep count rises);
// with the header from object_header_get 260 (identical); with the type compared to 0 and 7 249 (address now
// before the branch, but no bit test); the type comparisons alone 255. Slot order ignores local names.
// Round 55: swapped count declarations 181; counts before the body-count snapshot 181;
// retry before that snapshot 181; a fresh indexed cleanup counter 196; outer selector declared
// after the arrays 181; scan-wide body pointer 181; existing header getter 181; const body count 181.
// Retry-head body-count snapshot before/after the local resets, with original/first/second/both
// list-count declarations at function scope: eight at 181. Original snapshot and scopes restored.
// Round 262: function-entry selector/index/island storage, all seven combinations, 181 each;
// all three with entry body-count storage 181. Every initializer stayed at its original use;
// original declarations restored. Scratch phase, scalar homes and indexed cleanup remain.
// Round 218: First/second scratch arrays at island/function, function/island, island/island,
// list/function, function/list, list/list, island/list and list/island scopes:
// 181/181/181/181/181/181/181/181 (fast control 181). Original restored.
// Round 368: first/second scratch counts long/short, long/word, short/long, short/short, short/word,
// word/long, word/short, word/word 245/245/280/272/272/280/272/272. Original count domains and executable
// body restored.
// Round 24: shared processing ordinal 183; island membership ordinal 181; both at function
// scope 183; eager second scan 181; eager first/both scans 259/259; staged category 259;
// direct body-pointer countdown 237. Original restored.
// Round 244: twice-used pending-field pointer, with original/immutable-count/early-count/
// found-first lifetimes 181/181/181/274; field-reference counterparts
// 181/181/181/274. Original field observations, count and flag order restored.
// Round 218: Indexed 0x81 membership with bool/byte/short/long admission 182/182/182/260;
// with fresh body bound 249/249/246/245. Original classification and body restored.
// Frame 0x424 agrees; scratch phase, scalar homes, admission BL and cleanup induction remain.
// Round 206: Chained initial list-count/list-and-retry resets, with separate/chained retry resets
// [181, 273, 181, 273]; flat entry rejection on the same four graphs [181, 273, 181, 273]. All restored.
// Round 378: Existing component-index getter with ordinary/inline/forced/noinline
// caller declarations 299 each; each with indexed cleanup 294. Query remains out
// of line; original property scan, cleanup cursor, declarations and body restored.
// Round 383: saved release-state truth byte/signed-byte/word/long with consumed cleanup
// cursor 181/181/242/207; with cursor advanced after release 263/263/233/273.
// Original saved truth, cursor timing and complete body restored.
// Round 170: Recursive inline/forced-inline/noinline/inline-noinline declarations, then each with indexed
// cleanup 181/181/181/181/196/196/196/196. Original recursive declaration and cursor restored.
// Round 33: Complementary first membership result bool/byte/short/long with cursor cleanup,
// then indexed cleanup: 181/181/182/260/196/196/197/264. Original result, both eager scans and body restored.
// Round 176: Real pending field at +0xae as byte/signed-byte, each with Boolean/byte admission and
// first-membership carriers, eight at 181. Native record offsets and all truth values preserved;
// original pending field, flag carriers, signed bounds, both eager scans, retry and cleanup restored.
// Round 249: Header pool/data observed at island/retry boundaries 181/281/181/281;
// with indexed cleanup 194/288/194/288. Original observations and complete body restored.
// Round 428: live list-array/list-bound/body-array field references, seven nonempty
// combinations 181/181/181/181/181/181/181; all three by field pointers 181. Exact-only forms
// restored; original field observations, both eager scans, retries and release graph retained.
// Round 211: completed first-membership long/dword ordinal, direct/named result, cursor/indexed
// cleanup, scores 182/197/259/263/182/197/259/263. Original executable form restored.
// DL204: reverse first/second/both membership indices 260/256/245; reverse first cursor
// alone/with reverse second 310/328; indexed-cleanup counterparts 241/336/252. Original restored.
// Round 97 (lane 7): on the per-arm list base, second scan setting the shared found flag + inner bound
// from the field + plain indexed cleanup 233 (957 bytes; aligned differences 93 -> 52: retail's scans, retry
// head, cleanup and BL admission; left: outer index EDX, shifted spill slots, the body index stored at its
// definition and loop tail where retail keeps it and the list in ECX/EDX on the unflagged path, the header
// address folded after the `local_9 > 0` branch, loop padding); with a nested if for the flagged body 233; with
// a counted retry loop 233; with the admission flag assigned in if/else arms 249; the shared found flag alone
// 260. A renamed outer index moves no spill slot. Lead: on that base the outer selector is zeroed into EDX
// (retail EAX), so the list lands in EAX (retail EDX) and every spill slot shifts by one; the one real extra
// instruction is the body-index spill at loop entry (retail spills it only on the flagged path after `dec`).
// Round 249: Real two-list parent record, indexed/branched arrays and branched/conditional
// named members 281/181/181/181; those four with indexed cleanup 276/196/196/196.
// Original parent observations, native child types, cleanup and complete body restored.
// Round 267: reached-end comparisons for fixed selector, first/second membership, first processing,
// cleanup, both searches, both processing loops and all five together each score 181.
// Original loop tests restored; scratch EDX/EAX, list/index/count homes and cleanup induction remain.
// Round 191 (lane 3, fast): retail's outer loop is the selector counter (selector at esp+0x28 after the
// four pushes, body at +0x24, list +0x20). The outer locals renamed to sort in retail's slot order 181
// (same code); the first scan ending on its found flag in the loop condition 260; the admission flag
// preset true and cleared in a test 179 (a flag set in a test: only with a match), that with the plain
// indexed cleanup 194 (957 bytes, plain check); the admission as if/else arms 255; the record type read
// into a byte before the admission 181 (same code); the index decrement after the pending-flag clear
// 181, the body count read before that clear 181 (same code each).
// @retail 0x278f00
void function_278f00()
{
	long local_0[128];
	long local_1[128];
	if (g_55e4fc && !g_47f05b)
	{
		g_47f05b = true;
		for (long local_2 = 0; local_2 < 2; local_2++)
		{
			s_278f01 *local_3;
			if (local_2 == 0)
				local_3 = (s_278f01 *)((byte *)g_51e9a4 + 8);
			else
				local_3 = (s_278f01 *)((byte *)g_51e9a4 + 0x14);
			for (long local_4 = 0; local_4 < local_3->field_4; local_4++)
			{
				s_278f00 *local_5 = local_3->field_0[local_4];
				if (!local_5->field_ae)
					continue;
				local_4--;
				local_5->field_ae = false;
				long local_6 = local_5->field_40;
				long local_7 = 0;
				long local_8 = 0;
				long local_9 = 0;
				do
				{
					local_7 = 0;
					local_8 = 0;
					for (long local_10 = 0; local_10 < local_6; local_10++)
					{
						hkRigidBody *local_11 = local_5->field_3c[local_10];
						if (!local_11->m_fixed)
						{
							long local_12 = havok_component_get(havok_entity_property_get((hkEntity *)local_11, 0x2001))->object_index;
							s_278f02 *local_13 = &((s_278f02 *)g_4e0300->data)[local_12 & 0xffff];
							bool local_14 = local_9 > 0 || !((1 << local_13->field_3) & 0x81);
							bool local_15 = false;
							long local_16;
							for (local_16 = 0; local_16 < local_7; local_16++)
							{
								if (local_0[local_16] == local_12)
								{
									local_15 = true;
									break;
								}
							}
							for (local_16 = 0; local_16 < local_8; local_16++)
							{
								if (local_1[local_16] == local_12)
									break;
							}
							if (!local_15 && local_16 == local_8)
							{
								if (local_14)
									local_0[local_7++] = local_12;
								else
									local_1[local_8++] = local_12;
							}
						}
					}
					if (local_7 > 0)
						break;
					local_9++;
				} while (local_9 < 2);
				for (long local_10 = 0; local_10 < local_7; local_10++)
				{
					long local_11 = local_0[local_10];
					s_havok_object *local_12 = havok_object_get(local_11);
					if (function_a7670(local_11))
					{
						((s_278f03 *)local_12)->field_c0_5 = true;
						function_1c3770(local_11, 0);
					}
					else
					{
						bool local_13 = TEST_FIELD_BIT(((s_278f03 *)local_12)->field_c0);
						if (local_13)
							function_146bf0();
						local_12 = havok_object_get(local_11);
						long local_14 = local_12->havok_component_index;
						if (local_14 != NONE)
						{
							s_havok_component *local_15 = havok_component_get(local_14);
							long local_16 = local_15->object_index;
							local_15->~s_havok_component();
							record_pool_release(g_51e9b8, local_14);
							s_havok_object *local_17 = havok_object_get(local_16);
							if (*((byte *)local_17 + 0xc0) & 1)
							{
								local_17->havok_flag = false;
								(*g_51e9a0)--;
							}
							local_12->havok_component_index = NONE;
						}
						local_12 = havok_object_get(local_11);
						if (*((byte *)local_12 + 0xc0) & 1)
						{
							local_12->havok_flag = false;
							(*g_51e9a0)--;
						}
						if (local_13)
						{
							function_278f00();
							function_146bf0();
						}
						function_b8540(local_11);
					}
				}
				long const *cleanup_cursor = local_1;
				for (long local_10 = 0; local_10 < local_8; local_10++)
				{
					long local_11 = *cleanup_cursor++;
					s_havok_object *local_12 = havok_object_get(local_11);
					if (local_12->havok_component_index != NONE)
						function_1d1260(havok_component_get(local_12->havok_component_index));
					((s_278f03 *)local_12)->field_c0 = false;
					function_b8890(local_11);
				}
			}
		}
		g_47f05b = false;
		g_55e4fc = false;
	}
}
