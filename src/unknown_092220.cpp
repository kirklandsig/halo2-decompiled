// @flags /O2 /Gr
/* UNKNOWN_092220.CPP: Xbox Live online tasks that follow the team
   balancing (0x92220..0x927b0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include <xtl.h>
#include <xonline.h>

HRESULT online_task_continue(s_type_9df9da *task);
long online_task_new_if_logged_on(void);
void function_6b640(long task_index);

static inline s_type_9df9da *online_task_try_and_get(long task_index)
{
	s_type_9df9da *task = 0;
	if (task_index != NONE)
	{
		s_record_pool *data = g_4cf78c;
		long absolute_index = task_index & 0xffff;
		if (absolute_index < data->high_water_index)
		{
			s_type_9df9da *candidate = (s_type_9df9da *)(data->data + data->size * absolute_index);
			if (candidate->salt != 0 && candidate->salt == (task_index >> 16))
				task = candidate;
		}
	}
	return task;
}

static __forceinline long function_925c2();

/* starts reading the given statistics */
// @retail 0x92220
long online_stats_read(word count, XONLINE_STAT_SPEC *specs)
{
	long result = NONE;
	for (long i = 0; i < count; i++)
	{
		dword flags = specs[i].xuidUser.dwUserFlags;
		if (flags == 0xbad00000 || (flags & 3))
			goto done;
	}
	{
		long task_index = function_925c2();
		s_type_9df9da *task = (s_type_9df9da *)datum_get_inlined(g_4cf78c, task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlineStatRead(count, specs, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
				result = task_index;
			}
			else
			{
				function_6b640(task_index);
				result = NONE;
			}
		}
		else
			result = task_index;
	}
done:
	return result;
}

/* retrieves the requested statistics */
// @retail 0x92330
bool online_stats_read_result(long task_index, XONLINE_STAT_SPEC *specs, word count, byte *extra_buffer, word extra_size)
{
	s_type_9df9da *task = (s_type_9df9da *)datum_get_inlined(g_4cf78c, task_index);
	bool result = false;
	if (task && online_logon_connected() && (task->flag_bits.running || task->flag_bits.finished))
	{
		if (SUCCEEDED(XOnlineStatReadGetResult((XONLINETASK_HANDLE)task->handle, count, specs, extra_size, extra_buffer)))
			result = true;
	}
	return result;
}

/* starts writing the given statistics */
// @retail 0x923c0
long online_stats_write(XONLINE_STAT_SPEC const *specs, word count)
{
	long task_index = online_task_new_if_logged_on();
	if (task_index != NONE)
	{
		s_record_pool *local_0 = g_4cf78c;
		long local_1 = task_index & 0xffff;
		if (local_1 < local_0->high_water_index)
		{
			byte *local_2 = local_0->data;
			long local_3 = local_0->size;
			s_type_9df9da *task = (s_type_9df9da *)(local_2 + local_3 * local_1);
			if (task->salt != 0 && task->salt == (task_index >> 16))
			{
				HRESULT status = XOnlineStatWrite(count, specs, NULL, (PXONLINETASK_HANDLE)&task->handle);
				if (SUCCEEDED(status))
				{
					task->flags = 1;
					task->type = 0x13;
					task->controller_index = NONE;
				}
				else
				{
					function_6b640(task_index);
					task_index = NONE;
				}
			}
		}
    }
    return task_index;
}

// @retail 0x92450
bool online_stats_write_succeeded(long task_index)
{
	s_type_9df9da *task = online_task_try_and_get(task_index);
	bool result = false;
	if (task && online_logon_connected() && (task->flags & 6))
	{
		HANDLE result_handle;
		PXONLINE_STAT_ATTACHMENT_REFERENCE references;
		DWORD count;
		if (SUCCEEDED(XOnlineStatWriteGetResult((XONLINETASK_HANDLE)task->handle, &result_handle, &references, &count)))
			result = true;
	}
	return result;
}

// @retail 0x924e0
long online_round_register(bool free_for_all, XNKID const *session_id, ULONGLONG const *round_key, long seconds)
{
	long task_index = online_task_new_if_logged_on();
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			XONLINE_ARB_ID id;
			id.SessionID = *session_id;
			id.qwRoundID = *round_key;
			dword flags = (free_for_all ? 0x10 : 0) | 0x0c;
			if (SUCCEEDED(XOnlineArbitrationRegister(&id, seconds, flags, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}
	return task_index;
}

/* submits the completed round's statistics */
static __forceinline long function_925c2()
{
    long local_0;
    if (online_logon_connected())
    {
        local_0 = record_pool_allocate(g_4cf78c);
        if (local_0 != NONE)
        {
            s_type_9df9da *local_1 = (s_type_9df9da *)g_4cf78c->data + (local_0 & 0xffff);
            local_1->handle = 0;
            local_1->type = NONE;
            local_1->controller_index = NONE;
            local_1->flags = 0;
        }
    }
    else
        local_0 = NONE;
    return local_0;
}

/* reports the round's statistics procedures for arbitration: every procedure's user must be
   a valid account that is not a guest, and at most 1000 procedures are accepted */
// MiniMax S (11-11): no new form, no lower score (14); the guard's `jle` jumps to the count check's own `jg` where
// retail jumps past it, because ours reuses the guard's flags to skip the compare, and this round's forms (an outer
// count guard, the count compared as < 1001, the count check turned into an early exit) measure 131, 16 and 14.
// MiniMax R (11-10): no new form, no lower score (14); the one difference left is the register the user flags load
// into (eax here, the dying user-pointer register ecx in retail), and every spelling of the two tests still leaves
// them in eax, so the body stands.
// MiniMax I (11-10): no new form, no lower score (14); the user flags are still loaded into eax where retail loads them into the dying user pointer register ecx, and every spelling of the two tests leaves the load in eax.
// MiniMax D (11-10): no new form, no lower score; the flags are still loaded into eax where retail
// loads them into the dying user pointer register ecx, and the sibling 0x92220 in this file matches
// with the same flags loaded into ecx, because there eax still holds the result and the allocator
// then takes the next free register; here eax is free, so our allocator takes it, while retail's
// allocator took ecx and reused the pointer's own register for the loaded value. This round: the
// flags in a named local, the flags assigned inside the condition, the flags through a pointer to
// the field, the flags through a pointer built with an explicit byte offset, the two tests split
// into two ifs, the condition written negated, the modulo form and the kind read directly with no
// local all leave the load in eax (14 each), the mask test first gives 15, the pointer assigned
// before the kind test with the test negated gives 115, and reading the field in the two kinds
// separately gives 149; every one was reverted, so the body stands. The two shorter encodings
// leave the function two bytes short, so every later field is reported as differing as well; the
// tail of the function matches retail instruction for instruction, and the two global loads in the
// reduced image are the check's unresolved placeholders, not differences of the code.
// separately gives 149; every one was reverted, so the body stands.
// MiniMax D (11-10): no new form, no lower score; retail loads the user flags into the pointer's own
// register (mov ecx, [ecx+8]) and compares them there, this build loads them into eax, and the two
// shorter encodings leave the function two bytes short, so every later field is reported as differing
// as well; the tail of the function matches retail instruction for instruction. The two global loads
// in the reduced image are the check's unresolved placeholders, not differences of the code.
// MiniMax E (11-09): no new form, no lower score; the user flags still load into eax where
// retail loads them into the dying user pointer register ecx.
// MiniMax R (11-09): no new form, no lower score; the user flags still load into eax where retail loads them into the dying user pointer register ecx, and the two tests in the other order (15), the flags compared as unsigned (14), the flags in an unsigned local (14) and the constants written unsigned (14) all leave the code as it was.
// MiniMax R (11-09): no new form, no lower score; the register the user flags load into is the only
// difference left, and every form of the test I tried leaves them in eax. This round: returning NONE
// at the bad-flags test instead of jumping to the end scores 98, so the body stands.
// MiniMax R (11-07): no new form, no lower score; the user flags still load into eax where retail loads them
// into the dying user pointer register ecx. This round: reading the flags once into a named dword, so the
// pointer is used only for that load, is byte-identical to the baseline, so the register choice does not come
// from the number of loads; the reduced image still leaves the 0x6cd50 call and two globals unresolved.
// MiniMax C (11-06): no new form, no lower score; the user flags still load into eax where retail
// loads them into the dying user pointer register ecx, and the block layout is the same in both
// builds, so every other difference the reduced image reports is a global address, a call target
// or a branch displacement; the permuter on this function was killed at its time limit and wrote
// nothing, so it searched nothing.
// MiniMax G (11-05, r3): no new form, no lower score; the flags still load into eax where retail
// loads them into the dying pointer register ecx.
// MiniMax G (11-05, r2): no new form, no lower score; the flags in a named local, the flags read
// through a pointer to the field and the precomputed user pointer still load the flags into eax.
// MiniMax G (11-05): no new form, no lower score; the user flags are still loaded into eax where
// retail loads them into the dying user-pointer register ecx.
// MiniMax B (11-05): no new form, no lower score.
// MiniMax I (11-01): the user flags are still loaded into eax where retail loads them into the dying user pointer register ecx; a conditional-expression user pointer (both polarities, with and without a record cursor), the flags and the pointer in function-scope locals and the mask test first all leave the code as it was.
// MiniMax A (11-03): no new form, no lower score.
// MiniMax G (11-04): no new form, no lower score.
// score 14 (was 14): this round: compiling the file without whole-program optimisation (/GL- in the
// file's flags) drops the register calling convention retail's code uses, so every function in the
// file stops matching; the flags in a named local and the two tests split into two ifs stay
// byte-identical to the baseline. Still the user flags are loaded into eax where retail loads them
// into the dying pointer register.
// Earlier measurement 14 (was 14): the plain (full) check agrees with the fast one: all 14 differences come from
// one register choice, the user flags loaded into eax where retail loads them into the dying user
// pointer register ecx; our two shorter encodings leave the function two bytes short, so every
// later field (the globals and the calls) is reported as differing as well.
// MiniMax H (11-04): no new form, no lower score; the flags still load into eax where retail
// loads them into the dying pointer register ecx (14 variants: the pointer non-const, the flags in
// dword/unsigned/short locals, the flags through a pointer or an index cast, a reference to the
// user, a void pointer, the two tests in the other order, the two tests split, the user pointer
// from a ternary, the kind in a switch, the mask test first: 14, 14, 14, 14, 14, 14, 14, 15,
// 125, 116, 15).
// This round: the flags in a const dword local 14, in an unsigned long local 14, a const kind 14,
// the kind declared before the user pointer 14, the mask test before the equality test 15 (all
// reverted).
// this round: the user flags are still loaded into eax where retail loads them
// into ecx; a named local for the flags, the record's field read directly, the flags read through
// an index cast, the two tests in the other order, the two tests split apart and a signed local
// all leave the code as it was.
// MiniMax C (11-08): forms tried, none lower: the flags in a local 14, the flags non-const 14, the
// two tests split 14, the mask first 15, the flags unsigned 14, the kind as a switch 116; the
// permuter's 55 tries kept the baseline 14.
// Earlier measurement 14 (was 14): the user flags are loaded into eax in ours and into ecx in retail.
// The block layout is the same in both builds (the branch that finds no user jumps to the
// shared load with ecx zero), so only the register the flags are loaded into differs.
// This round: the flags in a named local and the two tests split into two ifs 14 (byte-identical
// to the baseline).
// Earlier: the same register and padding remain. Shared task/datum lookups 14/14;
// enclosing update/conditional records 14/14; kind first 14; early flags carrier 14;
// inverse continue 14; report-id name 14; SDK guest test 14; existing allocator 183/317;
// field reference 14.
// Native procedure record reference 14; signed/unsigned full-width kind 129/129;
// request/task/both workspaces before result 14/14/14; named submitted count 14;
// conditional report-flag bit stores 63.
// MiniMax B (11-12): no new form, no lower score (14); the user flags still load into eax where retail loads
// them into the dying pointer register ecx; the record walked by a cursor pointer, the flags read through a
// dword index, the inverted guard with an empty body, the flags in an unsigned local, the flags split from the
// pointer, the record bound by reference, the kind read from the record directly, the flags in a
// function-scope local, the early return out of the loop and the break with a status flag
// (14, 14, 15, 14, 14, 14, 14, 14, 14, 98, 14) leave the code as it was.
// MiniMax B (11-11): no new form, no lower score (14); the user flags still load into eax where retail loads them
// into the dying pointer register ecx, and the permuter on this function was stopped at its time limit without a
// variant that beat it.
// MiniMax A (11-11): no new form, no lower score (14); the plain (full-image) check agrees with the fast one on all fourteen differences, so the register choice is not a reduced-image artifact, and no spelling of the flags load reaches retail's dying register.
// MiniMax G (11-11): no new form, no lower score (14); the user flags are still loaded into eax where retail loads
// them into the pointer register ecx, and our two shorter encodings leave the body two bytes short, so every later
// branch lands elsewhere; the flags in a named local, the two tests apart, the pointer from a conditional
// expression and the two tests in the other order (14, 14, 125, 15) leave the code as it was.
// P419: Separate real union arms 128; all five other equality-case orders
// 16/16/17/17/16; function-scope user pointer 14; request aggregate 14.
// All forms restored; flags still use EAX instead of the dying ECX pointer.
// @retail 0x925c0
long online_round_report(XNKID const *session_id, ULONGLONG const *round_key, long count, XONLINE_STAT_PROC const *procedures, bool flag0, bool flag1)
{
	long task_index = NONE;
	for (long i = 0; i < count; i++)
	{
		XUID const *user = NULL;
		if (procedures[i].wProcedureID == XONLINE_STAT_PROCID_UPDATE_REPLACE ||
			procedures[i].wProcedureID == XONLINE_STAT_PROCID_UPDATE_INCREMENT)
			user = &procedures[i].Update.xuid;
		else if (procedures[i].wProcedureID == XONLINE_STAT_PROCID_CONDITIONAL)
			user = &procedures[i].Conditional.xuid;
		if (user->dwUserFlags == 0xbad00000 || (user->dwUserFlags & 3))
			goto done;
	}
	if (count <= 1000)
	{
		task_index = function_925c2();
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			XONLINE_ARB_ID id;
			id.SessionID = *session_id;
			id.qwRoundID = *round_key;
			dword flags = (flag0 ? 1 : 0) | (flag1 ? 2 : 0);
			if (SUCCEEDED(XOnlineArbitrationReport(&id, flag1 ? 0 : count, procedures, NULL, flags, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}
done:
	return task_index;
}

/* whether the task failed because the service is not available */
// @retail 0x92750
bool online_task_service_unavailable(long task_index)
{
	bool result = false;
	s_type_9df9da *task = online_task_try_and_get(task_index);
	if (online_task_continue(task) == 0x8015b108)
		result = true;
	return result;
}

// @retail 0x927b0
long online_round_extend(XNKID const *session_id, ULONGLONG const *round_key, word seconds)
{
	long task_index = online_task_new_if_logged_on();
	long local_0 = task_index;
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			XONLINE_ARB_ID id;
			id.SessionID = *session_id;
			id.qwRoundID = *round_key;
			if (SUCCEEDED(XOnlineArbitrationExtendRound(&id, seconds, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				local_0 = NONE;
			}
		}
	}
	return local_0;
}
