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

// @retail 0x31c100
void __cdecl function_31c100(void *step_10, void *step_20, void *step_50, void *world_info,
	void *simulation, void *islands, long island_count, long field_58, long field_5c)
{
	// Keep buffer ownership and the cursor passed to row builders together.
	struct workspace_state
	{
		char *term_limit;
		char *overflow;
		char *volatile base;
		solver_output_31e7b0 output;
		char *allocated;
		char *volatile results;
	} workspace;

	workspace_sizes_31c100 *sizes = (workspace_sizes_31c100 *)simulation;
	workspace.allocated = 0;
	workspace.overflow = 0;
	long result_bytes = sizes->result_count * 4 + 8;
	scratch_31c100 *scratch = g_484b38;
	scratch->active = 1;
	char *initial = scratch->data;
	char *end = initial + g_484b38->size;
	workspace.base = initial;
	char *terms;
	char *operations;
reset_workspace:
	terms = workspace.base + sizes->body_bytes + 0x90;
	workspace.term_limit = terms + sizes->term_bytes;
	workspace.results = workspace.term_limit;
	for (;;)
	{
		operations = workspace.results + result_bytes;
		char *needed_end = operations + sizes->op_bytes + 4;
		if (needed_end <= end) break;
		long allocation_size;
		if (terms >= end)
		{
			allocation_size = needed_end - initial;
			// This helper caches the available count before changing the cursor.
			workspace.allocated = (char *)stack_allocate_30d740((allocation_size + 15) & ~15);
			end = workspace.allocated + allocation_size;
			workspace.base = workspace.allocated;
			goto reset_workspace;
		}
		else if (workspace.term_limit >= end)
		{
			long spill = 2 * sizes->maximum_term_bytes + terms + sizes->term_bytes - end;
			allocation_size = spill + result_bytes + sizes->op_bytes + 4;
			workspace.allocated = (char *)stack_allocate_30d740((allocation_size + 15) & ~15);
			workspace.term_limit = end - sizes->maximum_term_bytes;
			workspace.overflow = workspace.allocated;
			end = workspace.allocated + allocation_size;
			workspace.results = workspace.allocated + spill;
		}
		else
		{
			allocation_size = result_bytes + sizes->op_bytes + 4;
			workspace.allocated = (char *)stack_allocate_30d740((allocation_size + 15) & ~15);
			end = workspace.allocated + allocation_size;
			workspace.results = workspace.allocated;
		}
	}
	lane_a_motion_output *zero = (lane_a_motion_output *)workspace.base;
	zero->kind = 2;
	zero->rows[2] = _mm_setzero_ps();
	zero->rows[0] = _mm_setzero_ps();
	zero->rows[1] = _mm_setzero_ps();
	zero->rows[3] = _mm_setzero_ps();
	zero->rows[4] = _mm_setzero_ps();
	zero->rows[5] = _mm_setzero_ps();
	zero->rows[6] = _mm_setzero_ps();
	char *body = workspace.base + 0x80;
	void **entity = (void **)islands;
	void **entity_end = entity + island_count;
	for (; entity < entity_end; ++entity)
	{
		class_3193c0 *motion = motion_31c100(*entity);
		long offset = body - workspace.base;
		if (motion->field_8 != offset) motion->field_8 = offset;
		body = motion->export_state((long)step_10, (long)world_info, (lane_a_motion_output *)body);
	}
	*body = 3;
	workspace.output.terms = (__m128 *)terms;
	workspace.output.operations = (unsigned long *)operations;
	lane_a_constraint **constraint = (lane_a_constraint **)field_58;
	lane_a_constraint **constraint_end = constraint + field_5c;
	for (; constraint < constraint_end; ++constraint)
	{
		void **pair = (void **)step_50;
		class_3193c0 *second = motion_31c100((*constraint)->body_b);
		pair[0] = workspace.base + motion_31c100((*constraint)->body_a)->field_8;
		pair[1] = workspace.base + second->field_8;
		if ((char *)workspace.output.terms >= workspace.term_limit)
		{
			workspace.output.terms = (__m128 *)workspace.overflow;
			workspace.term_limit = (char *)0xffffffff;
		}
		(*constraint)->build((lane_a_solver_input *)step_50, (lane_a_solver_output *)&workspace.output);
	}
	*workspace.output.operations = 0x400;
	for (long i = 0; i < sizes->result_count; ++i) ((float *)workspace.results)[i] = 0;
	long success = solve_operations_31c940((solver_step_31c940 *)step_20,
		(unsigned char *)operations, (solver_velocity_31c940 *)workspace.base, (float *)workspace.results);
	if (g_480154 < g_480158)
	{
		*(char const **)g_480154 = g_4153f4;
		*(float *)(g_480154 + 4) = (float)sizes->result_count;
		g_480154 += 8;
	}
	if (success == 1)
	{
		long mode = *(long *)((char *)step_20 + 0x20);
		body = workspace.base + 0x80;
		for (entity = (void **)islands; entity < entity_end; ++entity)
			body = motion_31c100(*entity)->integrate_output((lane_a_motion_step *)step_10,
				mode, (lane_a_motion_output *)body);
	}
	g_484b38->active = 0;
	if (workspace.allocated) g_memory_2d88a0->release_stack(workspace.allocated, (end - workspace.allocated + 15) & ~15);
}
