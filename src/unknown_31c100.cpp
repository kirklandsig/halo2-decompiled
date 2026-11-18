// @flags /O2 /GL-
// Assemble solver workspace, export motions, build rows, solve and import.
#include "lane_a_motion_leaves.h"
#include "lane_a_constraints.h"
#include "solver_rows_31e7b0.h"
#include <string.h>
#include "stack_array_30d740.h"

struct solver_step_31c940;
struct solver_velocity_31c940;
long __cdecl solve_operations_31c940(solver_step_31c940 *, unsigned char *, solver_velocity_31c940 *, float *);

struct scratch_31c100
{
	unsigned char unknown00[8];
	char *data;
	long size;
	unsigned char active;
};
scratch_31c100 *g_484b38;
char g_4153f4[32];
extern char *g_480154;
extern char *g_480158;

struct workspace_sizes_31c100
{
	unsigned char unknown00[8];
	long maximum_term_bytes, term_bytes, op_bytes, body_bytes, result_count;
};

static __forceinline class_3193c0 *motion_31c100(void const *entity)
{
	class_3193c0 *motion;
	memcpy(&motion, (char const *)entity + 0x3c, sizeof(motion));
	return motion;
}

// Lay the workspace out from the scratch buffer, moving to blocks from the
// allocator's stack until the terms, results and operations all fit.
static __forceinline void reserve_workspace_31c100(workspace_sizes_31c100 const *sizes, long result_bytes,
	char *initial, char *&base, char *&end, char *&allocated, char *&overflow, char *&term_limit,
	char *&terms, char *&results, char *&operations)
{
	char *cursor = initial;
	for (;;)
	{
		base = cursor;
		cursor += sizes->body_bytes + 0x90;
		terms = cursor;
		cursor += sizes->term_bytes;
		term_limit = cursor;
		long allocation_size;
		for (;;)
		{
			results = cursor;
			cursor += result_bytes;
			operations = cursor;
			cursor += sizes->op_bytes + 4;
			if (cursor <= end) return;
			if (terms >= end) break;
			if (term_limit >= end)
			{
				long spill = 2 * sizes->maximum_term_bytes + terms + sizes->term_bytes - end;
				allocation_size = spill + result_bytes + sizes->op_bytes + 4;
				allocated = (char *)stack_allocate_30d740((allocation_size + 15) & ~15);
				term_limit = end - sizes->maximum_term_bytes;
				end = allocated + allocation_size;
				overflow = allocated;
				cursor = allocated;
				cursor += spill;
			}
			else
			{
				allocation_size = result_bytes + sizes->op_bytes + 4;
				allocated = (char *)stack_allocate_30d740((allocation_size + 15) & ~15);
				end = allocated + allocation_size;
				cursor = allocated;
			}
		}
		allocation_size = cursor - initial;
		allocated = (char *)stack_allocate_30d740((allocation_size + 15) & ~15);
		end = allocated + allocation_size;
		cursor = allocated;
	}
}

// Review cleanup score 316: unmatched volatile/goto tuning removed.
// score 155 (was 262): the workspace is laid out with one cursor that is advanced past
// each part in turn (bodies, terms, results, operations) and replaced by the block the
// allocator returns (the block is assigned to `allocated` first and the cursor copied
// from it); the values are plain locals. The result count is read before the two zero
// stores and turned into the byte size after the scratch buffer is marked active. The
// constraint loop steps the list parameter itself (a reference to field_58), which
// frees esi for the cursors and leaves the base in edx with a home slot, as in retail.
// Left: the entity cursor of the first loop is kept in memory (retail: esi), retail
// saves the first body's address and the mode for the last loop, and the profiling
// record is written through the stepped pointer (fstp [ecx-4]) in retail.
// @retail 0x31c100
void __cdecl function_31c100(void *step_10, void *step_20, void *step_50, void *world_info,
	void *simulation, void *islands, long island_count, long field_58, long field_5c)
{
	workspace_sizes_31c100 *sizes = (workspace_sizes_31c100 *)simulation;
	long result_bytes = sizes->result_count;
	char *allocated = 0;
	char *overflow = 0;
	scratch_31c100 *scratch = g_484b38;
	scratch->active = 1;
	result_bytes = result_bytes * 4 + 8;
	char *initial = scratch->data;
	char *end = initial + g_484b38->size;
	char *base;
	char *term_limit;
	char *terms;
	char *results;
	char *operations;
	reserve_workspace_31c100(sizes, result_bytes, initial, base, end, allocated, overflow, term_limit,
		terms, results, operations);
	lane_a_motion_output *zero = (lane_a_motion_output *)base;
	zero->kind = 2;
	zero->rows[2] = _mm_setzero_ps();
	zero->rows[0] = _mm_setzero_ps();
	zero->rows[1] = _mm_setzero_ps();
	zero->rows[3] = _mm_setzero_ps();
	zero->rows[4] = _mm_setzero_ps();
	zero->rows[5] = _mm_setzero_ps();
	zero->rows[6] = _mm_setzero_ps();
	char *body = base + 0x80;
	void **entity = (void **)islands;
	void **entity_end = entity + island_count;
	for (; entity < entity_end; ++entity)
	{
		class_3193c0 *motion = motion_31c100(*entity);
		long offset = body - base;
		if (motion->field_8 != offset) motion->field_8 = offset;
		body = motion->export_state((long)step_10, (long)world_info, (lane_a_motion_output *)body);
	}
	*body = 3;
	lane_a_constraint **&constraint = *(lane_a_constraint ***)&field_58;
	lane_a_constraint **constraint_end = constraint + field_5c;
	solver_output_31e7b0 output;
	output.terms = (__m128 *)terms;
	output.operations = (unsigned long *)operations;
	for (; constraint < constraint_end; ++constraint)
	{
		void **pair = (void **)step_50;
		class_3193c0 *second = motion_31c100((*constraint)->body_b);
		pair[0] = base + motion_31c100((*constraint)->body_a)->field_8;
		pair[1] = base + second->field_8;
		if ((char *)output.terms >= term_limit)
		{
			output.terms = (__m128 *)overflow;
			term_limit = (char *)0xffffffff;
		}
		(*constraint)->build((lane_a_solver_input *)step_50, (lane_a_solver_output *)&output);
	}
	*output.operations = 0x400;
	for (long i = 0; i < sizes->result_count; ++i) ((float *)results)[i] = 0;
	long success = solve_operations_31c940((solver_step_31c940 *)step_20,
		(unsigned char *)operations, (solver_velocity_31c940 *)base, (float *)results);
	if (g_480154 < g_480158)
	{
		*(char const **)g_480154 = g_4153f4;
		*(float *)(g_480154 + 4) = (float)sizes->result_count;
		g_480154 += 8;
	}
	if (success == 1)
	{
		long mode = *(long *)((char *)step_20 + 0x20);
		body = base + 0x80;
		for (entity = (void **)islands; entity < entity_end; ++entity)
			body = motion_31c100(*entity)->integrate_output((lane_a_motion_step *)step_10,
				mode, (lane_a_motion_output *)body);
	}
	g_484b38->active = 0;
	if (allocated) g_memory_2d88a0->release_stack(allocated, (end - allocated + 15) & ~15);
}
