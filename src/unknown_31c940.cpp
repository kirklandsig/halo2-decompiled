// @flags /O2 /GL-
// Iterative constraint solving, residual updates, and result writeback.
#include "solver_dispatch_31c940.h"
#include <math.h>
#include <string.h>
#include <stddef.h>
// Borrow the body-pointer word; individual operations capture their fields at
// the retail read/write boundaries rather than retaining a stale body pointer.
struct solver_scoped_body_fields_31c940
{
	solver_velocity_31c940 *const &body;
	__forceinline explicit solver_scoped_body_fields_31c940(
		solver_velocity_31c940 *const &value) : body(value) {}
};

// The residual pass carries the shared impulse cursor across row layouts.
struct solver_impulse_cursor_31c940
{
	float *&value;
	__forceinline explicit solver_impulse_cursor_31c940(float *&v) : value(v) {}
	__forceinline void advance(unsigned n) { value += n; }
};
typedef char solver_impulse_cursor_size_31c940[sizeof(solver_impulse_cursor_31c940) == 4 ? 1 : -1];
// Real scalar outputs and SIMD copies, ordered by their independently observed
// retail stack homes. Every member is consumed by a solver operation.
struct solver_scalar_arena_31c940
{
	float spin_first;
	float angular_limit;
	float normal_first_a_copy;
	float bounded_angular;
	float unilateral_linear;
	float angular_error;
	float normal_second_projection_b;
	float linear_error;
	float normal_second_b_copy;
	float linear_friction;
	float normal_first_projection_a;
	float linear_bounds;
	float normal_first_projection_b;
	float bounded_linear;
	float circular_first;
	float residual_normal_first;
	float angular_friction;
	float residual_normal_second;
	float normal_second_projection_a;
	float residual_bounded_first;
	float spin_angular;
	float residual_bounded_second;
	float spin_angular_copy;
	float residual_circular_first;
	float normal_first_b_copy;
	float residual_circular_second;
	float spin_second;
	float residual_spin;
	float normal_second_a_copy;
	float residual_linear;
	float circular_second;
	float residual_angular;
};
// These five words serve different scalar and pointer roles in separate operations.
// The rounded-input word has its own consumed scalar storage.
// This view borrows the currently active roles.
struct solver_work_prefix_31c940 {
 union { float pair_b; __m128 const *inverse_b; float const *writeback_time; };
 union { float first_error; __m128 *angular_b; };
 union { float fallback_scalar; float const *friction_scale; solver_linear_row_31e7b0 const *next_circular; };
};
struct solver_simd_word_31c940 {float value;};
struct solver_work_tail_31c940 {
 union {float pair_a;__m128 *linear_b;float *linear_floats_b;solver_linear_row_31e7b0 const *next_normal;};
};
struct solver_scalar_work_31c940 {
 float &pair_b;__m128 const *&inverse_b;float const *&writeback_time;
 float &first_error;__m128 *&angular_b;
 float &fallback_scalar;float const *&friction_scale;solver_linear_row_31e7b0 const *&next_circular;
 float &simd_scalar;
 float &pair_a;__m128 *&linear_b;float *&linear_floats_b;solver_linear_row_31e7b0 const *&next_normal;
 __forceinline solver_scalar_work_31c940(solver_work_prefix_31c940 &prefix,solver_simd_word_31c940 &simd_word,solver_work_tail_31c940 &tail):
 pair_b(prefix.pair_b),inverse_b(prefix.inverse_b),writeback_time(prefix.writeback_time),first_error(prefix.first_error),angular_b(prefix.angular_b),
 fallback_scalar(prefix.fallback_scalar),friction_scale(prefix.friction_scale),next_circular(prefix.next_circular),
 simd_scalar(simd_word.value),pair_a(tail.pair_a),linear_b(tail.linear_b),linear_floats_b(tail.linear_floats_b),next_normal(tail.next_normal){}
};


// Keep the SSE reduction in a scalar result object before x87 arithmetic.
struct solver_scalar_31c940 { float value; };


// Construct the scalar in its consumer's storage. This avoids the integer
// copies introduced by returning the plain scalar aggregate above.
struct solver_projection_31c940
{
	float value;
	__forceinline explicit solver_projection_31c940(__m128 const &v) { _mm_store_ss(&value, v); }
};


// Both normal pairs borrow the first error from its float home. The first
// coefficient is captured before the diagonal product; the second error
// retains its extended intermediate through the two coupled sums.
static __forceinline float first_coupled_impulse_31c940(double mass,
 float const &diagonal, double other, float coupling)
{
 return mass * diagonal + other * coupling;
}

static __forceinline float second_coupled_impulse_31c940(float mass,
 double diagonal, float const &other, float coupling)
{
 return mass * diagonal + other * coupling;
}
// Capture the row coefficient before forming its coupled product.
struct circular_mass_31c940
{
 float value;
 __forceinline explicit circular_mass_31c940(float input) : value(input) {}
};
// Keep the coupled product live while forming the diagonal product.
static __forceinline float circular_coupled_impulse_31c940(circular_mass_31c940 mass,
 float const &diagonal, double other, float coupling)
{
 double coupled = other * coupling;
 return mass.value * diagonal + coupled;
}
// Paired projections retain retail's separate loads of A's velocity fields.
static __forceinline void sum_three_to_arena_31c940(__m128 const &v, float &output)
{
 __m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
 __m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
 _mm_store_ss(&output, xyz);
}


static __forceinline __m128 project_first_linear_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, __m128 const &linear_axis, float &projection_output)
{
	// Retain the linear axis alongside the consumed B linear input. Reuse
	// that input value for A's angular contribution after the subtraction.
	struct { __m128 work; __m128 linear_axis; } projection;
	projection.work=b.body->linear;
	__m128 work_1=b.body->angular;
	__m128 work_2=a->linear;
	__m128 work_3=row->a;
	projection.linear_axis=linear_axis;
	work_2=_mm_sub_ps(work_2,projection.work);
	projection.work=a->angular;
	projection.work=_mm_mul_ps(projection.work,work_3);
	work_3=row->b;
	work_3=_mm_mul_ps(work_1,work_3);
	work_2=_mm_mul_ps(work_2,projection.linear_axis);
	work_1=_mm_add_ps(projection.work,work_3);
	work_3=_mm_add_ps(work_1,work_2);
	sum_three_to_arena_31c940(work_3,projection_output);
	return projection.linear_axis;
}


static __forceinline void project_second_linear_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float &projection_output)
{
	__m128 work_0=b.body->linear;
	__m128 work_1=a->linear;
	__m128 work_2=row->a;
	__m128 work_3=row->b;
	work_1=_mm_sub_ps(work_1,work_0);
	work_0=a->angular;
	work_0=_mm_mul_ps(work_0,work_2);
	work_2=b.body->angular;
	work_2=_mm_mul_ps(work_2,work_3);
	work_3=row->linear;
	work_1=_mm_mul_ps(work_1,work_3);
	work_0=_mm_add_ps(work_0,work_2);
	work_0=_mm_add_ps(work_0,work_1);
	sum_three_to_arena_31c940(work_0,projection_output);
}

static __forceinline __m128 project_linear_axis_view_31c940(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b,__m128 const &angular_b, __m128 const &axis_linear);
// Retain projection inputs before publishing the saved angular receiver.
static __forceinline void circular_first_projection_fields(
 solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a,
 solver_velocity_31c940 *b, __m128 *&angular_b, float &output)
{
 __m128 projected = project_linear_axis_view_31c940(
  row, a, b->linear, b->angular, row->linear);
 angular_b = &b->angular;
 _mm_store_ss(&output, projected);
}

static __forceinline void project_second_pair_second_row(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b, __m128 const &angular_b, __m128 const &axis_linear,float &projection_output);



static __forceinline void apply_linear(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float *result, float &simd_scratch,
	__m128 &weight_a, __m128 &weight_b, __m128 &temporary, __m128 &linear_change_b, __m128 &linear_axis)
{
	// The caller retains the projected linear axis in linear_axis.

	simd_scratch=impulse;
	temporary=a->inverse_inertia;

	weight_b=_mm_load_ss(&simd_scratch);
	weight_a=_mm_shuffle_ps(weight_b,weight_b,0);
	weight_a=_mm_mul_ps(weight_a,temporary);
	temporary=b.body->inverse_inertia;

	weight_b=_mm_shuffle_ps(weight_b,weight_b,0);
	weight_b=_mm_mul_ps(weight_b,temporary);
	temporary=_mm_shuffle_ps(weight_a,weight_a,0xff);
	temporary=_mm_mul_ps(temporary,linear_axis);
	linear_change_b=_mm_shuffle_ps(weight_b,weight_b,0xff);
	linear_change_b=_mm_mul_ps(linear_change_b,linear_axis);
	// Retain B through all four updates before A can overwrite its saved home.
	solver_velocity_31c940 *const update_b=b.body;
	linear_axis=a->linear;
	linear_axis=_mm_add_ps(linear_axis,temporary);
	a->linear=linear_axis;
	temporary=_mm_load_ps(update_b->linear.m128_f32);
	temporary=_mm_sub_ps(temporary,linear_change_b);
	update_b->linear=temporary;
	// Angular axes reload after both linear writes.
	temporary=row->a;
	weight_a=_mm_mul_ps(weight_a,temporary);
	temporary=row->b;
	weight_b=_mm_mul_ps(weight_b,temporary);
	temporary=a->angular;
	temporary=_mm_add_ps(temporary,weight_a);
	a->angular=temporary;
	weight_a=update_b->angular;
	weight_a=_mm_add_ps(weight_a,weight_b);
	update_b->angular=weight_a;
	*result+=impulse;

}

static __forceinline __m128 const *apply_circular_first_fields(float impulse,
 solver_linear_row_31e7b0 const *row,solver_velocity_31c940 *a,
 solver_scoped_body_fields_31c940 const &b,__m128 &linear_b,__m128 *const &angular_b,
 __m128 const *&inverse_home,solver_linear_row_31e7b0 const *&next_row_home,
 float *result,float &simd_scratch)
{
	__m128 w[5];
	w[2]=a->inverse_inertia;
	w[4]=row->linear;
	simd_scratch=impulse;
	w[1]=_mm_load_ss(&simd_scratch);
	w[0]=_mm_shuffle_ps(w[1],w[1],0);
	w[0]=_mm_mul_ps(w[0],w[2]);
	__m128 const *inverse_b=&b.body->inverse_inertia;
	w[2]=*inverse_b;
	w[1]=_mm_shuffle_ps(w[1],w[1],0);
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
	w[2]=_mm_mul_ps(w[2],w[4]);
	w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
	w[3]=_mm_mul_ps(w[3],w[4]);
	w[4]=a->linear;
	w[4]=_mm_add_ps(w[4],w[2]);
	a->linear=w[4];
	w[2]=linear_b;
	w[2]=_mm_sub_ps(w[2],w[3]);
	linear_b=w[2];
	w[2]=row->a;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=row->b;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=a->angular;
	w[2]=_mm_add_ps(w[2],w[0]);
	a->angular=w[2];
	inverse_home=inverse_b;
	w[0]=*angular_b;
	w[0]=_mm_add_ps(w[0],w[1]);
	*angular_b=w[0];
	// Read the old accumulation before publishing the next row; its store may redirect that row.
	// Read the result before publishing a next row that its store can redirect.
	float accumulated=*result+impulse;
	next_row_home=row+1;
	inverse_b=inverse_home;
	*result=accumulated;
	return inverse_b;
}
// This second row completes the shared two-impulse result stream.
static __forceinline void complete_circular_pair_31c940(float impulse,
 solver_linear_row_31e7b0 const *const &row_home,__m128 *&row_cursor,__m128 *&terms,
 solver_velocity_31c940 *a,__m128 &linear_b,__m128 *const &angular_b,
 __m128 const *inverse_b,float *&result_base,float &simd_scratch)
{
	__m128 w[5];
	w[2]=a->inverse_inertia;
	simd_scratch=impulse;
	w[3]=*inverse_b;
	terms+=6;
	w[1]=_mm_load_ss(&simd_scratch);
	// Form A's broadcast independently from the rounded input word.
	w[0]=_mm_load1_ps(&simd_scratch);
	w[1]=_mm_shuffle_ps(w[1],w[1],0);
	w[1]=_mm_mul_ps(w[1],w[3]);
	w[0]=_mm_mul_ps(w[0],w[2]);
	// Reuse the stream cursor only after the rounded input; retain this row through linear writes.
	row_cursor=(__m128 *)row_home;
	w[4]=*row_cursor++;
	w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
	w[2]=_mm_mul_ps(w[2],w[4]);
	w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
	w[3]=_mm_mul_ps(w[3],w[4]);
	w[4]=a->linear;
	w[4]=_mm_add_ps(w[4],w[2]);
	a->linear=w[4];
	w[2]=linear_b;
	w[2]=_mm_sub_ps(w[2],w[3]);
	linear_b=w[2];
	w[2]=*row_cursor++;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=*row_cursor;
	// This row captures B before writing A angular; the first row reloads it afterward.
	__m128 *angular_receiver=angular_b;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=a->angular;
	w[2]=_mm_add_ps(w[2],w[0]);
	a->angular=w[2];
	w[0]=*angular_receiver;
	w[0]=_mm_add_ps(w[0],w[1]);
	*angular_receiver=w[0];
	// Complete the pair cursor before publishing its final accumulation.
	float accumulated = result_base[1] + impulse;
	result_base += 2;
	result_base[-1] = accumulated;
}


// The first positive row retains its unshifted linear cursor while angular
// axes use the coupled-pair cursor. Keep this update separate from the second
// positive row so both existing cursor representations survive optimization.
// Capture B's angular receiver after the linear writes, before either angular write.


// Borrow A's field addresses separately within each row update.
static __forceinline void apply_normal_positive_pair_31c940(float const &source_a,float const &source_b,
	__m128 const &first_axis,__m128 const *first_angular,
	solver_linear_row_31e7b0 const *second_row,solver_velocity_31c940 *body_a,
	solver_scoped_body_fields_31c940 const &b,float *results,
	float &copy_a,float &copy_b)
{
	__m128 second_scalar;
	__m128 w[5];
	{
		__m128 &a_linear = body_a->linear;
		__m128 &a_angular = body_a->angular;


		// Capture inertia before the SIMD copy can replace one of its lanes.
		// Use the stored float for accumulation and copy its bits for SIMD.

		w[2]=body_a->inverse_inertia;
		double impulse=source_a;
		memcpy(&copy_a,&source_a,sizeof(copy_a));
		// Reuse the consumed projection values through both body updates.

		w[1]=_mm_load_ss(&copy_a);
		w[0]=_mm_shuffle_ps(w[1],w[1],0);
		w[0]=_mm_mul_ps(w[0],w[2]);
		w[2]=b.body->inverse_inertia;
		w[1]=_mm_load_ss(&copy_a);
		w[1]=_mm_shuffle_ps(w[1],w[1],0);
		w[1]=_mm_mul_ps(w[1],w[2]);
		w[4]=first_axis;
		w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
		w[2]=_mm_mul_ps(w[2],w[4]);
		w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
		w[3]=_mm_mul_ps(w[3],w[4]);
		// Retain this receiver before A can replace the saved body pointer.
		solver_velocity_31c940 *const linear_body_b=b.body;
		w[4]=a_linear;
		w[4]=_mm_add_ps(w[4],w[2]);
		a_linear=w[4];
		w[2]=linear_body_b->linear;
		w[2]=_mm_sub_ps(w[2],w[3]);
		linear_body_b->linear=w[2];
		// Capture B after both linear writes, before either angular write.
		solver_velocity_31c940 *angular_body_b=b.body;
		w[2]=first_angular[0];
		w[0]=_mm_mul_ps(w[0],w[2]);
		w[2]=first_angular[1];
		w[1]=_mm_mul_ps(w[1],w[2]);
		w[2]=a_angular;
		w[2]=_mm_add_ps(w[2],w[0]);
		a_angular=w[2];
		w[0]=angular_body_b->angular;
		w[0]=_mm_add_ps(w[0],w[1]);
		angular_body_b->angular=w[0];
		// Capture the second SIMD input before the first result store
		// can replace its source or float copy.
		float accumulated=*results+impulse;
		memcpy(&copy_b,&source_b,sizeof(copy_b));
		w[1]=_mm_load_ss(&copy_b);
		*results=accumulated;
		second_scalar=w[1];
	}
	{
		__m128 &a_linear = body_a->linear;
		__m128 &a_angular = body_a->angular;

		// The previous update already captured the SIMD value. Accumulation reads
		// the stored second source after the first result is published.

		w[2]=body_a->inverse_inertia;
		float impulse = source_b;
		w[1]=second_scalar;
		w[4]=second_row->linear;
		w[0]=_mm_shuffle_ps(w[1], w[1], 0);
		w[0]=_mm_mul_ps(w[0], w[2]);
		w[2]=b.body->inverse_inertia;
		w[1]=_mm_shuffle_ps(w[1], w[1], 0);
		w[1]=_mm_mul_ps(w[1], w[2]);
		w[2]=_mm_shuffle_ps(w[0], w[0], 0xff);
		w[2]=_mm_mul_ps(w[2], w[4]);
		w[3]=_mm_shuffle_ps(w[1], w[1], 0xff);
		w[3]=_mm_mul_ps(w[3], w[4]);
		// Retain the linear receiver before A can replace the saved B body.
		solver_velocity_31c940 *const linear_body_b=b.body;
		w[4]=a_linear;
		w[4]=_mm_add_ps(w[4], w[2]);
		a_linear=w[4];
		w[2]=linear_body_b->linear;
		w[2]=_mm_sub_ps(w[2], w[3]);
		linear_body_b->linear=w[2];
		// The angular receiver is captured after the linear writes, before A angular.
		solver_velocity_31c940 *const angular_body_b=b.body;
		w[2]=second_row->a;

		w[0]=_mm_mul_ps(w[0], w[2]);
		w[2]=second_row->b;
		w[1]=_mm_mul_ps(w[1], w[2]);
		w[2]=a_angular;
		w[2]=_mm_add_ps(w[2], w[0]);
		a_angular=w[2];
		w[0]=angular_body_b->angular;
		w[0]=_mm_add_ps(w[0], w[1]);
		angular_body_b->angular=w[0];
		*(results+1)+=impulse;
	}
}
// Both fallbacks retain the pre-write B linear source. The first reloads its
// linear destination after A writes; the second retains that destination.
// B angular is captured after both linear writes, before A angular writes.


// The first fallback borrows its retained projection axis. The second borrows
// row storage, which is read after the rounded SIMD input has been published.
static __forceinline void apply_first_normal_fallback(float impulse, bool retain_linear_destination, __m128 const &linear_axis,
	__m128 const *angular_axes,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float *result, float &simd_scratch)
{
 __m128 w[5];
 w[2]=a->inverse_inertia;
 simd_scratch=impulse;
 w[1]=_mm_load_ss(&simd_scratch);
 w[0]=_mm_shuffle_ps(w[1],w[1],0);
 w[0]=_mm_mul_ps(w[0],w[2]);
 float *inverse_source=(float *)&b.body->inverse_inertia;
 w[2]=_mm_load_ps(inverse_source);
 solver_velocity_31c940 *linear_source=(solver_velocity_31c940 *)((char *)inverse_source - offsetof(solver_velocity_31c940,inverse_inertia));
 w[1]=_mm_shuffle_ps(w[1],w[1],0);
 w[1]=_mm_mul_ps(w[1],w[2]);
 w[4]=linear_axis;
 w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
 w[2]=_mm_mul_ps(w[2],w[4]);
 w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
 w[3]=_mm_mul_ps(w[3],w[4]);
 w[4]=a->linear;
 w[4]=_mm_add_ps(w[4],w[2]);
 a->linear=w[4];
 w[2]=linear_source->linear;
 w[2]=_mm_sub_ps(w[2],w[3]);
 if(retain_linear_destination)linear_source->linear=w[2];else b.body->linear=w[2];
 solver_velocity_31c940 *angular_receiver=b.body;
 w[2]=angular_axes[0];
 w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=angular_axes[1];
 w[1]=_mm_mul_ps(w[1],w[2]);
 w[2]=a->angular;
 w[2]=_mm_add_ps(w[2],w[0]);
 a->angular=w[2];
 w[0]=angular_receiver->angular;
 w[0]=_mm_add_ps(w[0],w[1]);
 angular_receiver->angular=w[0];
 *result+=impulse;
}
static __forceinline float &operation_float(unsigned char *op, long offset)
{
	return *(float *)(op + offset);
}

// Retail reloads the step pointer at these scalar reads. Return references so
// the x87 consumers retain their existing precision and evaluation boundaries.
static __forceinline float const &load_error_scale(solver_step_31c940 * const &step)
{
	return step->error_scale;
}

static __forceinline float const &load_velocity_scale(solver_step_31c940 * const &step)
{
	return step->velocity_scale;
}

static __forceinline float const &load_friction_scale(solver_step_31c940 * const &step)
{
	return step->friction_scale;
}

// The spin block retains B's field addresses across its projections and
// tangential updates. Keep the two projections' separate A-field loads.


// Keep the first projection reads ahead of publishing B's linear receiver.
struct spin_projection_output_31c940 {
 float *address; __m128 **linear_home; __m128 *linear_input;
 __forceinline spin_projection_output_31c940(float &output,__m128 *&home,__m128 *input):address(&output),linear_home(&home),linear_input(input){}
__forceinline spin_projection_output_31c940(spin_projection_output_31c940 const &v):address(v.address),linear_home(v.linear_home),linear_input(v.linear_input){}
 __forceinline void store(__m128 const &v) const {*linear_home=linear_input;_mm_store_ss(address,v);}
};
// The first reduction keeps its own consumed projection values. Capture B's
// angular receiver after those inputs and before publishing its linear receiver.
// Later projections retain their separate body reads and scalar publications.
static __forceinline __m128 project_linear_axis_view_31c940(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b,__m128 const &angular_b, __m128 const &axis_linear)
{
 struct projection_view {
  __m128 work;
  __m128 const &linear_axis;
  __forceinline projection_view(__m128 const &w, __m128 const &axis)
   :work(w),linear_axis(axis){}
 };
 projection_view projection(linear_b,axis_linear);
 __m128 work_1=angular_b;
 __m128 work_2=a->linear;
 __m128 work_3=row->a;

 work_2=_mm_sub_ps(work_2,projection.work);
 projection.work=a->angular;
 projection.work=_mm_mul_ps(projection.work,work_3);
 work_3=row->b;
 work_3=_mm_mul_ps(work_1,work_3);
 work_2=_mm_mul_ps(work_2,projection.linear_axis);
 work_1=_mm_add_ps(projection.work,work_3);
 work_3=_mm_add_ps(work_1,work_2);
 __m128 xy=_mm_add_ss(_mm_shuffle_ps(work_3,work_3,0x55),work_3);
 return _mm_add_ss(_mm_shuffle_ps(work_3,work_3,0xaa),xy);
}
static __forceinline void project_spin_contact_rows_31c940(solver_linear_row_31e7b0 const *rows,
	solver_velocity_31c940 *a,solver_velocity_31c940 *body_b,
	__m128 *const &linear_home,__m128 *&angular_home,
	spin_projection_output_31c940 output,solver_scalar_arena_31c940 &pool)
{
 {
  __m128 projected=project_linear_axis_view_31c940(rows,a,body_b->linear,
   body_b->angular,rows[0].linear);
  angular_home=&body_b->angular;
  output.store(projected);
 }
 {
 __m128 w[4];
 w[0]=*linear_home;
 w[1]=a->linear;
 w[2]=rows[1].a;
 w[3]=rows[1].b;
 w[1]=_mm_sub_ps(w[1],w[0]);
 w[0]=a->angular;
 w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=(*angular_home);
 w[2]=_mm_mul_ps(w[2],w[3]);
 w[3]=rows[1].linear;
 w[1]=_mm_mul_ps(w[1],w[3]);
 w[0]=_mm_add_ps(w[0],w[2]);
 w[0]=_mm_add_ps(w[0],w[1]);
 w[1]=_mm_shuffle_ps(w[0],w[0],0x55);
 w[1]=_mm_add_ss(w[1],w[0]);
 w[2]=_mm_shuffle_ps(w[0],w[0],0xaa);
 w[2]=_mm_add_ss(w[2],w[1]);
 _mm_store_ss(&pool.spin_second,w[2]);
 }
 {
 __m128 w[3];
 solver_angular_row_31e7b0 const *spin=(solver_angular_row_31e7b0 const *)(rows+2);
 w[1]=spin->a;
 w[0]=a->angular;
 w[2]=(*angular_home);
 w[0]=_mm_mul_ps(w[0],w[1]);
 w[1]=spin->b;
 w[2]=_mm_mul_ps(w[2],w[1]);
 w[0]=_mm_add_ps(w[0],w[2]);
 w[1]=_mm_shuffle_ps(w[0],w[0],0x55);
 w[1]=_mm_add_ss(w[1],w[0]);
 w[2]=_mm_shuffle_ps(w[0],w[0],0xaa);
 w[2]=_mm_add_ss(w[2],w[1]);
 _mm_store_ss(&pool.spin_angular,w[2]);
 }
}
static __forceinline __m128 const *apply_spin_first_fields(double impulse,
 solver_linear_row_31e7b0 const *row,solver_velocity_31c940 *a,
 __m128 inertia_a,__m128 linear_axis,__m128 *const &linear_home,__m128 &angular_b,
 __m128 const *const &inverse_home,solver_linear_row_31e7b0 const *&next_row_home,
 float *result,float &simd_copy)
{
	__m128 w[5];
	w[2]=inertia_a;
	w[4]=linear_axis;
	simd_copy=impulse;
	w[1]=_mm_load_ss(&simd_copy);
	w[0]=_mm_shuffle_ps(w[1],w[1],0);
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=*inverse_home;
	w[1]=_mm_shuffle_ps(w[1],w[1],0);
	w[1]=_mm_mul_ps(w[1],w[2]);
	__m128 *linear_receiver=linear_home;
	w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
	w[2]=_mm_mul_ps(w[2],w[4]);
	w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
	w[3]=_mm_mul_ps(w[3],w[4]);
	w[4]=a->linear;
	w[4]=_mm_add_ps(w[4],w[2]);
	a->linear=w[4];
	w[2]=*linear_receiver;
	w[2]=_mm_sub_ps(w[2],w[3]);
	*linear_receiver=w[2];
	w[2]=row->a;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=row->b;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=a->angular;
	w[2]=_mm_add_ps(w[2],w[0]);
	a->angular=w[2];
	w[0]=angular_b;
	w[0]=_mm_add_ps(w[0],w[1]);
	angular_b=w[0];
	float accumulated=*result+impulse;
	next_row_home=row+1;
	__m128 const *inverse_b=inverse_home;
	*result=accumulated;
	return inverse_b;
}
static __forceinline unsigned long apply_spin_second_fields(double impulse,
 solver_linear_row_31e7b0 const *const &row_home,__m128 *&row_cursor,
 solver_velocity_31c940 *a,__m128 *const &linear_home,__m128 &angular_b,
 __m128 const *inverse_b,float const &spin_source,float *result,float &simd_copy)
{
	__m128 w[5];
	w[2]=a->inverse_inertia;
	simd_copy=impulse;
	w[3]=*inverse_b;
	w[1]=_mm_load_ss(&simd_copy);
	w[0]=_mm_shuffle_ps(w[1],w[1],0);
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[1]=_mm_shuffle_ps(w[1],w[1],0);
	w[1]=_mm_mul_ps(w[1],w[3]);
	row_cursor=(__m128 *)row_home;
	w[4]=*row_cursor;
	__m128 *linear_receiver=linear_home;
	w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
	w[2]=_mm_mul_ps(w[2],w[4]);
	w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
	w[3]=_mm_mul_ps(w[3],w[4]);
	w[4]=a->linear;
	w[4]=_mm_add_ps(w[4],w[2]);
	a->linear=w[4];
	w[2]=*linear_receiver;
	w[2]=_mm_sub_ps(w[2],w[3]);
	*linear_receiver=w[2];
	// Linear writes can redirect the row used for the angular axes.
	row_cursor=(__m128 *)row_home+1;
	w[2]=*row_cursor++;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=*row_cursor;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=a->angular;
	w[2]=_mm_add_ps(w[2],w[0]);
	a->angular=w[2];
	w[0]=angular_b;
	w[0]=_mm_add_ps(w[0],w[1]);
	angular_b=w[0];
	double accumulated=*result+impulse;
	// Keep the SIMD bits even when the result store replaces the spin source.
	unsigned long spin_bits;
	memcpy(&spin_bits,&spin_source,sizeof(spin_bits));
	*result=accumulated;
	return spin_bits;
}

// Retain the first row's unshifted cursor alongside the second row's B axis.
// Their separate uses keep retail's cursor bias over the same row storage.


static __forceinline float sum_coupled_products_31c940(double diagonal,double cross){return diagonal+cross;}
static __forceinline void solve_contact_pair(solver_step_31c940 *const &step, unsigned char *op,
	__m128 *axis, __m128 *first_axis, solver_velocity_31c940 *a,
	solver_scoped_body_fields_31c940 const &b, float *result, solver_scalar_work_31c940 &scalar_work, solver_scalar_arena_31c940 &projection_pool)
{
	solver_step_31c940 *parameters = step;
	float target_a = axis[-5].m128_f32[3];
	__m128 retained_axis=project_first_linear_velocity(((solver_linear_row_31e7b0 *)(axis - 5)), a, b, *first_axis, projection_pool.normal_first_projection_a);
	// Retail stores this error to float; subsequent coupled expressions borrow
	// that stored value while the second error retains its x87 intermediate.

	project_second_linear_velocity(((solver_linear_row_31e7b0 *)(axis - 2)), a, b, projection_pool.normal_first_projection_b);
	scalar_work.first_error = target_a * parameters->error_scale
	- projection_pool.normal_first_projection_a * parameters->velocity_scale;
	float error_b = axis[-2].m128_f32[3] * parameters->error_scale
	- projection_pool.normal_first_projection_b * parameters->velocity_scale;
	float const &mass_a_source = axis[-3].m128_f32[3];
	scalar_work.pair_a = first_coupled_impulse_31c940(mass_a_source,
	(float const &)scalar_work.first_error, error_b, operation_float(op, 4));
	// Capture the second mass before its coupled product.
	struct {float diagonal,cross;} products;
	products.diagonal=axis[0].m128_f32[3];
	products.cross=scalar_work.first_error*operation_float(op,4);
	products.diagonal*=error_b;
	scalar_work.pair_b=sum_coupled_products_31c940(products.diagonal,products.cross);
	do
	{
		if (scalar_work.pair_a > 0.0f)
		{
			if (scalar_work.pair_b > 0.0f)
			{
				apply_normal_positive_pair_31c940(scalar_work.pair_a,scalar_work.pair_b,retained_axis,axis-4,(solver_linear_row_31e7b0 *)(axis-2),a,b,result,projection_pool.normal_first_a_copy,projection_pool.normal_first_b_copy);
				return;
			}
			break;
		}
		float candidate_b = error_b * axis[-1].m128_f32[3];
		// Unordered second candidates enter the update, as in retail.
		if(candidate_b<=0.0f)break;

		apply_first_normal_fallback(candidate_b, true,
		((solver_linear_row_31e7b0 *)(axis - 2))->linear, axis - 1, a, b, result + 1, scalar_work.fallback_scalar);
		return;
	} while (false);
	float candidate_a = scalar_work.first_error * axis[-4].m128_f32[3];
	if (candidate_a > 0.0f) apply_first_normal_fallback(candidate_a, false, retained_axis, axis - 4, a, b, result, scalar_work.fallback_scalar);
}

// The second unrolled pair keeps B field addresses and its first linear axis.
// Its two updates snapshot the angular-B address at different points.
// The first-row fallback keeps that destination through both linear writes.
static __forceinline void apply_second_pair_fallback(float impulse,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a,__m128 *linear_receiver,
	__m128 *const &angular_velocity_b_pointer, __m128 const &inverse_b, __m128 const &axis_linear, float *result, float &simd_scratch)
{
 __m128 w[5];w[2]=a->inverse_inertia;
 simd_scratch=impulse;w[1]=_mm_load_ss(&simd_scratch);
 w[0]=_mm_shuffle_ps(w[1],w[1],0);w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=inverse_b;w[1]=_mm_shuffle_ps(w[1],w[1],0);w[1]=_mm_mul_ps(w[1],w[2]);
 // Either body write may replace the saved angular pointer.
 __m128 *angular_receiver=angular_velocity_b_pointer;

 w[4]=axis_linear;
 w[2]=_mm_mul_ps(_mm_shuffle_ps(w[0],w[0],0xff),w[4]);
 w[3]=_mm_mul_ps(_mm_shuffle_ps(w[1],w[1],0xff),w[4]);
 w[4]=a->linear;w[4]=_mm_add_ps(w[4],w[2]);a->linear=w[4];
 w[2]=*linear_receiver;w[2]=_mm_sub_ps(w[2],w[3]);*linear_receiver=w[2];
 w[2]=row->a;w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=row->b;w[1]=_mm_mul_ps(w[1],w[2]);
 w[2]=a->angular;w[2]=_mm_add_ps(w[2],w[0]);a->angular=w[2];
 w[0]=*angular_receiver;w[0]=_mm_add_ps(w[0],w[1]);*angular_receiver=w[0];
 *result+=impulse;
}

// Read the stored float for x87 consumers; SIMD transfers can copy its bits.
static __forceinline float load_stored_impulse(float const &value)
{
	float const &stored=value;
	return stored;
}

static __forceinline unsigned long apply_second_pair_first_impulse(float const &source,
 solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a, __m128 *linear_b_pointer,
 __m128 *const &angular_b_home,
 __m128 const *&inverse_b_home,solver_linear_row_31e7b0 const *&next_row_home,
 __m128 const &linear_axis,float *result,float &simd_copy,float const &second_source)
{
 // Consume five vector values through the linear and angular updates.
 __m128 w[5];
 w[2]=a->inverse_inertia;
 float impulse=load_stored_impulse(source);
 memcpy(&simd_copy,&source,sizeof(simd_copy));
 w[1]=_mm_load_ss(&simd_copy);
 w[0]=_mm_shuffle_ps(w[1],w[1],0);
 w[0]=_mm_mul_ps(w[0],w[2]);
 __m128 const *inverse_b_pointer=linear_b_pointer+2;
 w[2]=*inverse_b_pointer;
 w[1]=_mm_shuffle_ps(w[1],w[1],0);
 w[1]=_mm_mul_ps(w[1],w[2]);
 w[4]=linear_axis;
 w[2]=_mm_mul_ps(_mm_shuffle_ps(w[0],w[0],0xff),w[4]);
 w[3]=_mm_mul_ps(_mm_shuffle_ps(w[1],w[1],0xff),w[4]);
 w[4]=a->linear;
 w[4]=_mm_add_ps(w[4],w[2]);
 a->linear=w[4];
 w[2]=*linear_b_pointer;
 w[2]=_mm_sub_ps(w[2],w[3]);
 *linear_b_pointer=w[2];
 w[2]=row->a;
 w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=row->b;
 w[1]=_mm_mul_ps(w[1],w[2]);
 w[2]=a->angular;
 w[2]=_mm_add_ps(w[2],w[0]);
 a->angular=w[2];
 inverse_b_home=inverse_b_pointer;
 __m128 *angular_b_pointer=angular_b_home;
 w[0]=*angular_b_pointer;
 w[0]=_mm_add_ps(w[0],w[1]);
 *angular_b_pointer=w[0];
 float accumulated=*result+impulse;
 next_row_home=row+1;
 // Snapshot only the SIMD bits before publishing the first result.
 // The second accumulation deliberately reloads its numeric source afterward.
 unsigned long second_bits;
 memcpy(&second_bits,&second_source,sizeof(second_bits));
 *result=accumulated;
 return second_bits;
}

static __forceinline void apply_second_pair_second_impulse(float const &source,unsigned long second_bits,
 solver_linear_row_31e7b0 const *const &row_home,__m128 *&row_cursor,solver_velocity_31c940 *a,
 __m128 *linear_b_pointer,__m128 *const &angular_b_home,__m128 const *const &inverse_b_home,
 float *result,float &simd_copy)
{
 // Consume five vector values through the linear and angular updates.
 __m128 w[5];
 w[2]=a->inverse_inertia;
 float impulse=load_stored_impulse(source);
 // Rebuild the rounded SIMD input from the pre-publication snapshot.
 memcpy(&simd_copy,&second_bits,sizeof(second_bits));
 w[3]=*inverse_b_home;
 w[1]=_mm_load_ss(&simd_copy);
 w[0]=_mm_shuffle_ps(w[1],w[1],0);
 w[0]=_mm_mul_ps(w[0],w[2]);
 w[1]=_mm_shuffle_ps(w[1],w[1],0);
 w[1]=_mm_mul_ps(w[1],w[3]);
 row_cursor=(__m128 *)row_home;
 w[4]=*row_cursor++;
 w[2]=_mm_mul_ps(_mm_shuffle_ps(w[0],w[0],0xff),w[4]);
 w[3]=_mm_mul_ps(_mm_shuffle_ps(w[1],w[1],0xff),w[4]);
 w[4]=a->linear;
 w[4]=_mm_add_ps(w[4],w[2]);
 a->linear=w[4];
 w[2]=*linear_b_pointer;
 w[2]=_mm_sub_ps(w[2],w[3]);
 *linear_b_pointer=w[2];
 w[2]=*row_cursor++;
 w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=*row_cursor;
 w[1]=_mm_mul_ps(w[1],w[2]);
 __m128 *angular_b_pointer=angular_b_home;
 w[2]=a->angular;
 w[2]=_mm_add_ps(w[2],w[0]);
 a->angular=w[2];
 w[0]=*angular_b_pointer;
 w[0]=_mm_add_ps(w[0],w[1]);
 *angular_b_pointer=w[0];
 *result+=impulse;
}


static __forceinline __m128 project_linear_axis(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b,__m128 const &angular_b, __m128 const &axis_linear)
{
 struct{__m128 work,linear_axis;}projection;
 projection.work=linear_b;
 __m128 work_1=angular_b;
 __m128 work_2=a->linear;
 __m128 work_3=row->a;
 projection.linear_axis=axis_linear;
 work_2=_mm_sub_ps(work_2,projection.work);
 projection.work=a->angular;
 projection.work=_mm_mul_ps(projection.work,work_3);
 work_3=row->b;
 work_3=_mm_mul_ps(work_1,work_3);
 work_2=_mm_mul_ps(work_2,projection.linear_axis);
 work_1=_mm_add_ps(projection.work,work_3);
 work_3=_mm_add_ps(work_1,work_2);
__m128 xy=_mm_add_ss(_mm_shuffle_ps(work_3,work_3,0x55),work_3);return _mm_add_ss(_mm_shuffle_ps(work_3,work_3,0xaa),xy);
}

static __forceinline void project_second_pair_second_row(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b, __m128 const &angular_b, __m128 const &axis_linear,float &projection_output)
{
struct{__m128 linear,angular;} inputs;__m128 velocity,axis;
inputs.linear=linear_b;
velocity=a->linear;
axis=row->a;
inputs.angular=row->b;

 velocity=_mm_sub_ps(velocity,inputs.linear);
 inputs.linear=a->angular;
 inputs.linear=_mm_mul_ps(inputs.linear,axis);
 axis=angular_b;
 axis=_mm_mul_ps(axis,inputs.angular);
 inputs.angular=axis_linear;
 velocity=_mm_mul_ps(velocity,inputs.angular);
 inputs.linear=_mm_add_ps(inputs.linear,axis);
 inputs.linear=_mm_add_ps(inputs.linear,velocity);
 sum_three_to_arena_31c940(inputs.linear,projection_output);
}


// The second-row fallback publishes its row after the A weight is captured.
// B inertia observes that publication; the row survives both linear writes.
static __forceinline void apply_second_normal_row_fallback_31c940(float impulse,
 solver_linear_row_31e7b0 const *next_row,solver_linear_row_31e7b0 const *&row_home,__m128 *&row_cursor,
 solver_velocity_31c940 *a,__m128 *linear_receiver,solver_scoped_body_fields_31c940 const &b,
 __m128 *const &angular_b_home,float *result,float &rounded)
{
 __m128 w[5];
 w[2]=a->inverse_inertia;
 rounded=impulse;
 w[1]=_mm_load_ss(&rounded);
 w[0]=_mm_shuffle_ps(w[1],w[1],0);
 w[0]=_mm_mul_ps(w[0],w[2]);
 // Word20 changes role from coupled scalar to the retained second row.
 row_home=next_row;
 w[2]=b.body->inverse_inertia;
 row_cursor=(__m128 *)row_home;
 w[4]=*row_cursor++;
 w[1]=_mm_shuffle_ps(w[1],w[1],0);
 w[1]=_mm_mul_ps(w[1],w[2]);
 w[2]=_mm_mul_ps(_mm_shuffle_ps(w[0],w[0],0xff),w[4]);
 w[3]=_mm_mul_ps(_mm_shuffle_ps(w[1],w[1],0xff),w[4]);

 w[4]=a->linear;w[4]=_mm_add_ps(w[4],w[2]);a->linear=w[4];
 w[2]=*linear_receiver;w[2]=_mm_sub_ps(w[2],w[3]);*linear_receiver=w[2];
 w[2]=*row_cursor++;w[0]=_mm_mul_ps(w[0],w[2]);
 w[2]=*row_cursor;w[1]=_mm_mul_ps(w[1],w[2]);
 // This row observes the new pointer after the linear writes, before A angular.
 __m128 *angular_receiver=angular_b_home;
 w[2]=a->angular;w[2]=_mm_add_ps(w[2],w[0]);a->angular=w[2];
 w[0]=*angular_receiver;w[0]=_mm_add_ps(w[0],w[1]);*angular_receiver=w[0];
 *result+=impulse;
}

// Retain the coefficient and B inertia addresses across consecutive rows.
// Native scalar locals keep the projection between SSE and x87 without a copy.
static __forceinline void solve_linear_friction(float target, float &limit,float next_limit,
	float const *const &friction_scale_pointer,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a,__m128 *const & angular_b_pointer, __m128 &linear_b, __m128 const *const &inverse_b_pointer,
	solver_step_31c940 *const &step, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	struct { __m128 first;
		__m128 second;
	} work;
	__m128 temporary;
	__m128 contribution;


	// Reuse the two consumed projection values for the body weights.
	work.first=linear_b;
	work.second=a->linear;
	temporary=row->a;
	work.second=_mm_sub_ps(work.second,work.first);
	work.first=a->angular;
	work.first=_mm_mul_ps(work.first,temporary);
	temporary=row->linear;
	work.second=_mm_mul_ps(work.second,temporary);

	// A projection inputs precede this publication; B angular inputs follow it.
	limit=next_limit;
	contribution=*angular_b_pointer;
	temporary=row->b;
	contribution=_mm_mul_ps(contribution,temporary);
	work.first=_mm_add_ps(work.first,contribution);
	work.first=_mm_add_ps(work.first,work.second);
	sum_three_to_arena_31c940(work.first,projection_pool.linear_friction);
	float impulse = (target * (*friction_scale_pointer)
	- projection_pool.linear_friction * load_velocity_scale(step)) * row->a.m128_f32[3];
	float magnitude = (float)fabs(impulse);
	if (magnitude > limit)
	{
		float factor = limit / magnitude;
		impulse *= factor;
		row->linear.m128_f32[3] *= factor;
	}

	__m128 update_0, update_1, update_2, update_3, update_4;
	// Retain the extended impulse for accumulation, rounding only its SIMD copy.
	update_2=a->inverse_inertia;
	simd_scratch=impulse;
	update_1=_mm_load_ss(&simd_scratch);
	update_0=_mm_shuffle_ps(update_1,update_1,0);
	update_0=_mm_mul_ps(update_0,update_2);
	update_2=*inverse_b_pointer;
	update_1=_mm_load_ss(&simd_scratch);
	update_1=_mm_shuffle_ps(update_1,update_1,0);
	update_1=_mm_mul_ps(update_1,update_2);

	// Capture B angular before either body can replace its saved home.
	__m128 *angular_receiver=angular_b_pointer;
	update_4=row->linear;
	update_2=_mm_mul_ps(_mm_shuffle_ps(update_0,update_0,0xff),update_4);
	update_3=_mm_mul_ps(_mm_shuffle_ps(update_1,update_1,0xff),update_4);
	update_4=a->linear;
	update_4=_mm_add_ps(update_4,update_2);
	a->linear=update_4;
	update_2=linear_b;
	update_2=_mm_sub_ps(update_2,update_3);
	linear_b=update_2;
	update_2=row->a;
	update_0=_mm_mul_ps(update_0,update_2);
	update_2=row->b;
	update_1=_mm_mul_ps(update_1,update_2);
	update_2=a->angular;
	update_2=_mm_add_ps(update_2,update_0);
	a->angular=update_2;
	update_0=*angular_receiver;
	update_0=_mm_add_ps(update_0,update_1);
	*angular_receiver=update_0;
	*result+=impulse;
}

// Select the active angular limit before the shared body-update tail. The
// lower error is stored to float before multiplication; the upper error keeps
// the extended x87 value. The scalar result is rounded only for SIMD use.
static __forceinline float select_angular_limit(double velocity, float const &target,
	float const &lower, float const &upper, float const &gain, float const &mass,
	bool &active, __m128 &scalar, float &simd_scratch)
{
	simd_scratch = (target - lower) * gain - velocity;
	if (simd_scratch > 0.0f)
	{
		float const &positive_error = simd_scratch;
		active = true;
		float impulse = positive_error * mass;
		simd_scratch = impulse;
		scalar = _mm_load_ss(&simd_scratch);
		return impulse;
	}
	float error = (target - upper) * gain - velocity;
	if (error < 0.0f)
	{
		active = true;
		double impulse = error * mass;
		simd_scratch = impulse;
		scalar = _mm_load_ss(&simd_scratch);
		return impulse;
	}
	active = false;
	return 0.0f;
}

// Convert only the SIMD update to float; retain the x87 impulse for results.
// Capture B's inverse inertia before either body update, including aliases.
// Capture target and mass before projection and retain its linear axis.
// Reload B inertia after the projection and rounded impulse stores. Angular
// axes reload after the linear writes; their receiver reloads after A angular.
static __forceinline void solve_linear_error_cursor_31c940(float target,float mass,
 __m128 *&terms,solver_velocity_31c940 *a,__m128 *linear_a,float *linear_b,
 __m128 *const &angular_b,__m128 const *const &inverse_b,
 solver_step_31c940 *const &step,float *&result,unsigned char *&op,
 solver_scalar_arena_31c940 &projection_pool,float &simd_scratch)
{
	// Retain the angular contribution and linear axis through publication;
 // reuse each consumed vector only after its final use.
 struct { __m128 angular, linear; } projection;
__m128 work_1; __m128 work_3; __m128 work_4;
	work_1=*linear_a;
	projection.linear=terms[1];
	work_3=terms[2];
	projection.angular=_mm_load_ps(linear_b);
	work_4=*angular_b;
	work_1=_mm_sub_ps(work_1,projection.angular);
	projection.angular=a->angular;
	projection.angular=_mm_mul_ps(projection.angular,projection.linear);
	projection.linear=terms[0];
	work_1=_mm_mul_ps(work_1,projection.linear);
	work_4=_mm_mul_ps(work_4,work_3);
	projection.angular=_mm_add_ps(projection.angular,work_4);
	projection.angular=_mm_add_ps(projection.angular,work_1);
	work_1=_mm_shuffle_ps(projection.angular,projection.angular,0x55);
	work_1=_mm_add_ss(work_1,projection.angular);
	work_3=_mm_shuffle_ps(projection.angular,projection.angular,0xaa);
	work_3=_mm_add_ss(work_3,work_1);
	_mm_store_ss(&projection_pool.linear_error,work_3);
	// Advance cursors while retaining this row's projected linear axis.
	op+=4; terms+=3; ++result;
	// Observe the current step after projection publication, which can alias a scale.
	solver_step_31c940 *parameters=step;
	float error_scale=parameters->error_scale;
	float velocity_scale=parameters->velocity_scale;
 float impulse=mass*(target*error_scale-projection_pool.linear_error*velocity_scale);

	// A inertia precedes the rounded input; B inertia is read afterward.
	work_3=a->inverse_inertia;
	simd_scratch=impulse;
	work_1=_mm_load_ss(&simd_scratch);

	projection.angular=_mm_shuffle_ps(work_1,work_1,0);
	projection.angular=_mm_mul_ps(projection.angular,work_3);
	work_3=*inverse_b;
	work_1=_mm_shuffle_ps(work_1,work_1,0);
	work_1=_mm_mul_ps(work_1,work_3);
	// Capture B linear before either linear write.
	float *linear_body_b=linear_b;
	work_3=_mm_shuffle_ps(projection.angular,projection.angular,0xff);
	work_3=_mm_mul_ps(work_3,projection.linear);
	work_4=_mm_shuffle_ps(work_1,work_1,0xff);
	work_4=_mm_mul_ps(work_4,projection.linear);
	projection.linear=*linear_a;
	projection.linear=_mm_add_ps(projection.linear,work_3);
	*linear_a=projection.linear;
	projection.linear=_mm_load_ps(linear_body_b);
	projection.linear=_mm_sub_ps(projection.linear,work_4);
	_mm_store_ps(linear_body_b,projection.linear);
	projection.linear=terms[-2];
	projection.angular=_mm_mul_ps(projection.angular,projection.linear);
	projection.linear=terms[-1];
	work_1=_mm_mul_ps(work_1,projection.linear);
	__m128 *angular_a=&a->angular;
	projection.linear=*angular_a;
	projection.linear=_mm_add_ps(projection.linear,projection.angular);
	*angular_a=projection.linear;
	// This row reloads the angular receiver after A's angular write.
	__m128 *angular_body_b=angular_b;
	projection.angular=*angular_body_b;
	projection.angular=_mm_add_ps(projection.angular,work_1);
	*angular_body_b=projection.angular;
	result[-1]+=impulse;
}


// Round once for the SIMD updates while retaining the scalar accumulation.
struct rounded_angular_input_31c940
{
	__m128 value;
	__forceinline rounded_angular_input_31c940(float &home, float input)
	{
		home = input;
		value = _mm_load_ss(&home);
	}
	__forceinline __m128 broadcast() const
	{
		return _mm_shuffle_ps(value, value, 0);
	}
};
// Capture target and limit before projection, retaining the coefficient address
// across consecutive rows. Keep B's inertia pointer through the clamp and read
// its value when forming the update. Only the SIMD copy rounds to float.
static __forceinline void solve_angular_friction_kernel(float target, float &limit,float next_limit,
	float const *const &friction_scale_pointer,
	solver_angular_row_31e7b0 *row, solver_step_31c940 *const &step,
	solver_velocity_31c940 *a, __m128 &velocity_b, __m128 const *const &inverse_b_pointer, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	{
		struct { __m128 axis, contribution; } input_a;
		__m128 work_c;
		// Retain both projection inputs before the limit can replace a row lane.
		input_a.axis=row->a;
		input_a.contribution=a->angular;
		work_c=velocity_b;
		input_a.contribution=_mm_mul_ps(input_a.contribution,input_a.axis);
		input_a.axis=row->b;
		work_c=_mm_mul_ps(work_c,input_a.axis);
		input_a.contribution=_mm_add_ps(input_a.contribution,work_c);
		// Publish the captured limit before storing the reduced projection.
		limit=next_limit;
		input_a.axis=_mm_shuffle_ps(input_a.contribution,input_a.contribution,0x55);
		work_c=_mm_shuffle_ps(input_a.contribution,input_a.contribution,0xaa);
		input_a.axis=_mm_add_ss(input_a.axis,input_a.contribution);
		work_c=_mm_add_ss(work_c,input_a.axis);
		_mm_store_ss(&projection_pool.angular_friction,work_c);
	}

	float impulse = (target * (*friction_scale_pointer)
	- projection_pool.angular_friction * load_velocity_scale(step)) * row->a.m128_f32[3];
	float magnitude = (float)fabs(impulse);
	if (magnitude > limit)
	{
		float factor = limit / magnitude;
		impulse *= factor;
		row->b.m128_f32[3] *= factor;
	}

	__m128 update[3];
	// Retain A inertia before the scalar store, then consume three SIMD values.
	update[2]=a->inverse_inertia;
	rounded_angular_input_31c940 scalar(simd_scratch, impulse);
	update[0]=scalar.broadcast();
	update[0]=_mm_mul_ps(update[0],update[2]);
	update[2]=*inverse_b_pointer;
	update[1]=scalar.broadcast();
	update[1]=_mm_mul_ps(update[1],update[2]);
	update[2]=row->a;
	update[0]=_mm_mul_ps(update[0],update[2]);
	update[2]=row->b;
	update[1]=_mm_mul_ps(update[1],update[2]);
	update[2]=a->angular;
	update[2]=_mm_add_ps(update[2],update[0]);
	a->angular=update[2];
	update[0]=velocity_b;
	update[0]=_mm_add_ps(update[0],update[1]);
	velocity_b=update[0];
	*result+=impulse;
}

// Capture the target independently from the coefficient that the kernel updates.
// Conversion to the kernel's float argument occurs after both inputs are captured.
struct bounded_target_31c940
{
 double value;
 __forceinline explicit bounded_target_31c940(float const &source) : value(source) {}
 __forceinline operator float() const { return value; }
};

// Keep B's field addresses across consecutive bounded angular rows. Capture
// both axes before either angular write; only the SIMD impulse rounds here.
static __forceinline void solve_bounded_angular(float mass, float target,
	solver_angular_row_31e7b0 *row, solver_velocity_31c940 *a,
	float *velocity_b, __m128 const *const &inverse_b_pointer,
	float const &upper, float const &lower, solver_step_31c940 *const &step, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	// Reuse the captured inputs after each projection value is consumed.
	struct { __m128 velocity_a, velocity_b, axis_a, axis_b; } inputs;
	__m128 work_4;
	inputs.velocity_a=a->angular;
	inputs.velocity_b=_mm_load_ps(velocity_b);
	inputs.axis_a=row->a;
	inputs.axis_b=row->b;
	inputs.velocity_b=_mm_mul_ps(inputs.velocity_b,inputs.axis_b);
	inputs.velocity_a=_mm_mul_ps(inputs.velocity_a,inputs.axis_a);
	inputs.velocity_a=_mm_add_ps(inputs.velocity_a,inputs.velocity_b);
	inputs.velocity_b=_mm_shuffle_ps(inputs.velocity_a,inputs.velocity_a,0x55);
	work_4=_mm_shuffle_ps(inputs.velocity_a,inputs.velocity_a,0xaa);
	inputs.velocity_b=_mm_add_ss(inputs.velocity_b,inputs.velocity_a);
	work_4=_mm_add_ss(work_4,inputs.velocity_b);
	_mm_store_ss(&projection_pool.bounded_angular,work_4);
// Keep the mass multiplication before the step scale, without an early
	// float spill. Both clamps retain their divide-then-multiply sequence.

	mass = (target - projection_pool.bounded_angular) * mass;
	float impulse = mass * load_error_scale(step);
	if (impulse > upper)
	{
		float ratio = upper / impulse;
		impulse = ratio * impulse;
	}
	else if (impulse < lower)
	{
		float ratio = lower / impulse;
		impulse = ratio * impulse;
	}

	work_4=a->inverse_inertia;
	simd_scratch=impulse;
	inputs.velocity_b=_mm_load_ss(&simd_scratch);
	inputs.velocity_a=_mm_shuffle_ps(inputs.velocity_b,inputs.velocity_b,0);
	inputs.velocity_a=_mm_mul_ps(inputs.velocity_a,work_4);
	work_4=*inverse_b_pointer;

	inputs.velocity_a=_mm_mul_ps(inputs.velocity_a,inputs.axis_a);
	inputs.axis_a=a->angular;
	inputs.axis_a=_mm_add_ps(inputs.axis_a,inputs.velocity_a);
 a->angular=inputs.axis_a;
	inputs.velocity_a=_mm_load_ps(velocity_b);
	inputs.velocity_b=_mm_shuffle_ps(inputs.velocity_b,inputs.velocity_b,0);
	inputs.velocity_b=_mm_mul_ps(inputs.velocity_b,work_4);
	inputs.velocity_b=_mm_mul_ps(inputs.velocity_b,inputs.axis_b);
	inputs.velocity_a=_mm_add_ps(inputs.velocity_a,inputs.velocity_b);
	_mm_store_ps(velocity_b,inputs.velocity_a);
	*result+=impulse;
}
static __forceinline void solve_unilateral_linear(float target,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b,
	__m128 &linear_b,__m128 *const & angular_b_pointer, solver_step_31c940 *const &step, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	// Reuse the five projection values for the weighted body updates.
	__m128 work_1 = a->linear;
	__m128 work_0 = linear_b;
	__m128 work_2 = row->a;
	__m128 work_3 = *angular_b_pointer;
	__m128 axis_linear = row->linear;
	work_1 = _mm_sub_ps(work_1, work_0);
	work_0 = a->angular;
	work_0 = _mm_mul_ps(work_0, work_2);
	work_2 = row->b;
	work_3 = _mm_mul_ps(work_3, work_2);
	work_1 = _mm_mul_ps(work_1, axis_linear);
	work_0 = _mm_add_ps(work_0, work_3);
	work_0 = _mm_add_ps(work_0, work_1);
	sum_three_to_arena_31c940(work_0, projection_pool.unilateral_linear);
	solver_step_31c940 *parameters = step;
	float error = target * parameters->error_scale - projection_pool.unilateral_linear * parameters->velocity_scale;
	if (error > 0.0f)
	{
		float impulse = error * row->a.m128_f32[3];
		work_2 = a->inverse_inertia;
		simd_scratch = impulse;
		work_1 = _mm_load_ss(&simd_scratch);
		work_0 = _mm_shuffle_ps(work_1, work_1, 0);
		work_0 = _mm_mul_ps(work_0, work_2);
		work_2 = b.body->inverse_inertia;
		work_1 = _mm_shuffle_ps(work_1, work_1, 0);
		work_1 = _mm_mul_ps(work_1, work_2);
		// Retain B angular before either body write can replace its home.
		__m128 *angular_b_address = angular_b_pointer;
		work_2 = _mm_shuffle_ps(work_0, work_0, 0xff);
		work_2 = _mm_mul_ps(work_2, axis_linear);
		work_3 = _mm_shuffle_ps(work_1, work_1, 0xff);
		work_3 = _mm_mul_ps(work_3, axis_linear);
		axis_linear = a->linear;
		axis_linear = _mm_add_ps(axis_linear, work_2);
		a->linear = axis_linear;
		work_2 = linear_b;
		work_2 = _mm_sub_ps(work_2, work_3);
		linear_b = work_2;
		// Angular axes reload after both linear writes.
		work_2 = row->a;
		work_0 = _mm_mul_ps(work_0, work_2);
		work_2 = row->b;
		work_1 = _mm_mul_ps(work_1, work_2);
		work_2 = a->angular;
		work_2 = _mm_add_ps(work_2, work_0);
		a->angular = work_2;
		work_0 = *angular_b_address;
		work_0 = _mm_add_ps(work_0, work_1);
		*angular_b_address = work_0;
		*result += impulse;
	}
}


static __forceinline void update_linear_limit_values(float impulse, solver_linear_row_31e7b0 const *row,__m128 const &linear_axis,
 solver_velocity_31c940 *a, __m128 const &inverse_b,
 float *linear_b,__m128 *const & angular_b_pointer, float *result, float &simd_scratch)
{
	__m128 inertia_a=a->inverse_inertia;
	simd_scratch = impulse;
	__m128 scalar = _mm_load_ss(&simd_scratch);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	// Capture B's angular receiver before either body is written.
	__m128 *angular_receiver=angular_b_pointer;
	// Capture both linear contributions before either body write.
	__m128 linear_contribution[2] =
	{
		_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), linear_axis),
		_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), linear_axis)
	};
	a->linear = _mm_add_ps(a->linear, linear_contribution[0]);
	_mm_store_ps(linear_b, _mm_sub_ps(_mm_load_ps(linear_b), linear_contribution[1]));
	// Capture both angular contributions before either body write.
	__m128 angular_contribution[2] =
	{
		_mm_mul_ps(change_a, row->a),
		_mm_mul_ps(change_b, row->b)
	};
	a->angular = _mm_add_ps(a->angular, angular_contribution[0]);
	(*angular_receiver) = _mm_add_ps((*angular_receiver), angular_contribution[1]);
	*result += impulse;
}

// Residual rows retain A's angular contribution before the linear product.
static __forceinline void project_residual_linear_velocity(
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a,
	solver_velocity_31c940 *b, __m128 const &linear_axis, float &projection_output)
{
	// Capture the angular input before its axis, then reuse each vector after
	// its final contribution. Both body inputs precede the reduction store.
	__m128 angular_a=a->angular;
	__m128 angular_axis=row->a;
	__m128 linear_b=b->linear;
	__m128 angular_b=b->angular;
	angular_a=_mm_mul_ps(angular_a,angular_axis);
	angular_axis=a->linear;
	angular_axis=_mm_sub_ps(angular_axis,linear_b);
	linear_b=linear_axis;
	angular_axis=_mm_mul_ps(angular_axis,linear_b);
	linear_b=row->b;
	angular_b=_mm_mul_ps(angular_b,linear_b);
	angular_a=_mm_add_ps(angular_a,angular_b);
	angular_a=_mm_add_ps(angular_a,angular_axis);
	sum_three_to_arena_31c940(angular_a,projection_output);

}


static __forceinline void subtract_angular_residual(solver_angular_row_31e7b0 *row,solver_velocity_31c940 *a,solver_velocity_31c940 *b, float &projected)
{
	__m128 work_b=row->a;
	__m128 work_a=a->angular;
	__m128 work_c=b->angular;
	work_a=_mm_mul_ps(work_a,work_b);
	work_b=row->b;
	work_c=_mm_mul_ps(work_c,work_b);
	work_a=_mm_add_ps(work_a,work_c);
	work_b=_mm_shuffle_ps(work_a,work_a,0x55);
	work_b=_mm_add_ss(work_b,work_a);
	work_c=_mm_shuffle_ps(work_a,work_a,0xaa);
	work_c=_mm_add_ss(work_c,work_b);
	_mm_store_ss(&projected,work_c);
	row->b.m128_f32[3]=row->b.m128_f32[3]-((float const &)projected);

}


// Solver state stays together through the iterations. Result publication then
// reuses its storage for the result stride and dispatch kind.
union solver_phase_state_31c940
{
	struct
	{
		solver_velocity_31c940 *a_home;
		float saved_velocity;
		long iteration;
		float saved_error;
		float spin_limit;
		float linear_limit;
		float angular_limit;
	} solving;
	struct
	{
		long stride;
		long kind;
	} writing;
};

static __forceinline float const &capture_writeback_time(solver_step_31c940 *const &step)
{
	return step->inverse_time;
}
// Body B retains one pointer word throughout the solving phases.
struct solver_pointer_phase_31c940
{
	solver_velocity_31c940 *body_b;
};
struct solver_complete_scalars_31c940
{
	solver_work_tail_31c940 work;
	solver_phase_state_31c940 phase;
	solver_scalar_arena_31c940 projections;
};

// The saved body pointer and scalar roles occupy 45 four-byte words.
// Keep this record word-aligned so their observed retail homes stay contiguous.
struct __declspec(align(4)) solver_scalar_owner_31c940
{
 solver_pointer_phase_31c940 pointers;
 solver_work_prefix_31c940 prefix;
 solver_simd_word_31c940 simd;
 solver_complete_scalars_31c940 scalars;
};
typedef char solver_owner_alignment_31c940[__alignof(solver_scalar_owner_31c940) == 4 ? 1 : -1];
typedef char solver_scalar_owner_size_31c940[sizeof(solver_scalar_owner_31c940)==180 ? 1 : -1];

typedef char solver_work_size_31c940[(sizeof(solver_work_prefix_31c940)+4+sizeof(solver_work_tail_31c940)) == 20 ? 1 : -1];
typedef char solver_phase_size_31c940[sizeof(solver_phase_state_31c940) == 28 ? 1 : -1];
typedef char solver_projection_pool_size_31c940[sizeof(solver_scalar_arena_31c940) == 128 ? 1 : -1];
typedef char solver_scalar_frame_size_31c940[(sizeof(solver_work_prefix_31c940)+4+sizeof(solver_complete_scalars_31c940)) == 176 ? 1 : -1];

// Stack allocation is 0xB4, as in retail.
// Still substantially unmatched; no exact solver match is claimed.
// The pre-review score 1675 used volatile tuning that is not restored here.
// SSE scheduling, scalar homes, repeated broadcasts and control flow still differ.
// The second normal pair walks its active row through a local cursor; friction
// can dispatch directly into the next body header.
// Publish the header inputs, then advance the shared stream cursor.
static __forceinline void decode_solver_header_31c940(unsigned char *&op,
 __m128 *&terms, solver_velocity_31c940 *&a, solver_velocity_31c940 *&b,
 solver_velocity_31c940 *&a_home)
{
	solver_header_31c940 *header = (solver_header_31c940 *)op;
	// Capture the row input before publishing pointer homes; the header can overlap them.
	a = header->a;
	solver_velocity_31c940 *next_b = header->b;
	terms = header->terms;
	a_home = a;
	b = next_b;
	op += sizeof(*header);
}

// Accumulate all four lanes through the aligned body-field storage.
static __forceinline void accumulate_velocity_field_31c940(__m128 &sum, __m128 const &value)
{
	__m128 input = _mm_load_ps(value.m128_f32);
	__m128 accumulated = _mm_load_ps(sum.m128_f32);
	accumulated = _mm_add_ps(accumulated, input);
	_mm_store_ps(sum.m128_f32, accumulated);
}
// The deferred normal path keeps the second error in its native float
// expression while borrowing the first error from its published home.
static __forceinline float first_pending_normal_impulse_31c940(double mass,
 float const &diagonal, float other, float const &coupling)
{
 return mass * diagonal + other * coupling;
}
// Preserve the captured normal-row coefficient through the coupled sum.
struct normal_mass_31c940
{
 float value;
 __forceinline explicit normal_mass_31c940(float input) : value(input) {}
};
static __forceinline float second_pending_normal_impulse_31c940(normal_mass_31c940 mass,
 float diagonal, float const &other, float coupling)
{
 return mass.value * diagonal + other * coupling;
}
// Save the scalar fields without passing their encodings through x87.
static __forceinline void save_solver_scales_31c940(float const &error,
 float const &velocity, float &saved_error, float &saved_velocity)
{
 memcpy(&saved_error, &error, sizeof(saved_error));
 memcpy(&saved_velocity, &velocity, sizeof(saved_velocity));
}
// Carry the current kind across each run of dynamic bodies. The next kind
// is captured only after both accumulated fields have been published.
static __forceinline void save_velocity_run_31c940(
 solver_velocity_31c940 *body, signed char &kind)
{
 for (;;)
 {
  switch (kind)
  {
  case 3: return;
  case 2:
   do
   {
    body->accumulated_angular = body->angular;
    body->accumulated_linear = body->linear;
    ++body;
    kind = (signed char)body->kind;
   } while (kind == 2);
   break;
  }
 }
}
static __forceinline void add_velocity_run_31c940(
 solver_velocity_31c940 *body, signed char &kind)
{
 for (;;)
 {
  switch (kind)
  {
  case 3: return;
  case 2:
   do
   {
    body->accumulated_angular = _mm_add_ps(body->accumulated_angular, body->angular);
    accumulate_velocity_field_31c940(body->accumulated_linear, body->linear);
    ++body;
    kind = (signed char)body->kind;
   } while (kind == 2);
   break;
  }
 }
}
static __forceinline void process_velocity_epoch_31c940(
 long iteration, solver_velocity_31c940 *bodies)
{
 if (iteration == 0)
 {
  solver_velocity_31c940 *velocity_body = bodies;
  signed char velocity_kind = (signed char)velocity_body->kind;
  save_velocity_run_31c940(velocity_body, velocity_kind);
 }
 else
 {
  solver_velocity_31c940 *velocity_body = bodies;
  signed char velocity_kind = (signed char)velocity_body->kind;
  add_velocity_run_31c940(velocity_body, velocity_kind);
 }
}
// @retail 0x31c940
long __cdecl solve_operations_31c940(solver_step_31c940 *step, unsigned char *operations,
	solver_velocity_31c940 *bodies, float *impulses)
{
	solver_scalar_owner_31c940 owned;
 solver_work_prefix_31c940 &work_prefix=owned.prefix;
 solver_simd_word_31c940 &simd_word=owned.simd;
solver_complete_scalars_31c940 &frame=owned.scalars;
solver_scalar_work_31c940 scalar_work(work_prefix,simd_word,frame.work);
	solver_phase_state_31c940 &state = frame.phase;
	solver_scalar_arena_31c940 &projection_pool = frame.projections;
	solver_pointer_phase_31c940 &pointer_phase=owned.pointers;

	if (!g_480c60)
	{
		function_31c7d0();
		if (!g_480c60) return 0;
	}
	// Capture the first count before publishing zero. At each iteration end,
 // retain the current limit before publishing the incremented counter.
 long iteration_count=step->iterations;
	for (state.solving.iteration = 0; state.solving.iteration < iteration_count;)
	{
		solver_step_31c940 *initial_parameters = step;
		// Save the step scales as raw words for the residual updates.
		save_solver_scales_31c940(initial_parameters->error_scale,initial_parameters->velocity_scale,state.solving.saved_error,state.solving.saved_velocity);
		unsigned char *op = operations;
		__m128 *terms;
		solver_velocity_31c940 *a;

		solver_velocity_31c940 *&b = pointer_phase.body_b;
		float *result = impulses;
		__m128 *normal_axes;
		bool dispatch_done=false;
		do
		{
			decode_solver_header_31c940(op,terms,a,b,state.solving.a_home);
			normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
			bool header_complete=false;
			while (!header_complete)
			{

				// Retail forms this address before dispatch, including for empty streams.

				solver_scoped_body_fields_31c940 scoped_b(b);
				__m128 first_linear_axis;
                float error_b;
                __m128 *linear_input;
                __m128 *update_cursor;
                __m128 *&angular_b_pointer = scalar_work.angular_b;
                bool first_positive;
                enum { not_normal, normal_complete, normal_fallback }
                    normal_completion = not_normal;
                switch (*(signed char *)op)
                {
                case 0: dispatch_done=true;header_complete=true;continue;
                // Process up to two coupled pairs, then join the spinning record.
                case 3:
                {
                    solve_contact_pair(step, op, normal_axes, terms, a, scoped_b,
                        result, scalar_work, projection_pool);
                    signed char next_pair_kind = *((signed char *)op + 8);
                    op += 8; terms += 6; normal_axes += 6; result += 2;
                    if (next_pair_kind != 3) continue;

                    solver_scoped_body_fields_31c940 const &body_b = scoped_b;
                    linear_input = &body_b.body->linear;
                    first_linear_axis = terms[0];
                    float target_a = terms[0].m128_f32[3];
                    // Derive angular input from the retained linear address.
                    // An independent saved pointer would introduce another home.
                    __m128 first_projection = project_linear_axis(
                        ((solver_linear_row_31e7b0 *)terms), a, *linear_input,
                        linear_input[1], first_linear_axis);
                    angular_b_pointer = linear_input + 1;
                    _mm_store_ss(&projection_pool.normal_second_projection_a, first_projection);
                    project_second_pair_second_row(((solver_linear_row_31e7b0 *)terms) + 1,
                        a, *linear_input, *angular_b_pointer, terms[3],
                        projection_pool.normal_second_projection_b);
                    solver_step_31c940 *parameters = step;
                    scalar_work.pair_b = target_a * parameters->error_scale
                        - projection_pool.normal_second_projection_a * parameters->velocity_scale;
                    error_b = terms[3].m128_f32[3] * parameters->error_scale
                        - projection_pool.normal_second_projection_b * parameters->velocity_scale;
                    float const &mass_a_source = terms[2].m128_f32[3];
                    float mass_a = mass_a_source;
                    scalar_work.pair_a = first_pending_normal_impulse_31c940(mass_a,
                        (float const &)scalar_work.pair_b, error_b, operation_float(op, 4));
                    float const &mass_b_source = terms[5].m128_f32[3];
                    float mass_b = mass_b_source;
                    scalar_work.fallback_scalar = second_pending_normal_impulse_31c940(normal_mass_31c940(
                        mass_b), error_b, (float const &)scalar_work.pair_b, operation_float(op, 4));
                    first_positive = load_stored_impulse(scalar_work.pair_a) > 0.0f;
                    if ((first_positive) && load_stored_impulse(scalar_work.fallback_scalar) > 0.0f)
                    {
                        __m128 *linear_receiver = linear_input;
                        unsigned long second_bits = apply_second_pair_first_impulse(
                            scalar_work.pair_a, ((solver_linear_row_31e7b0 *)terms),
                            a, linear_receiver, angular_b_pointer, scalar_work.inverse_b,
                            scalar_work.next_normal, first_linear_axis, result,
                            projection_pool.normal_second_a_copy, scalar_work.fallback_scalar);
                        apply_second_pair_second_impulse(scalar_work.fallback_scalar, second_bits,
                            scalar_work.next_normal, update_cursor, a, linear_receiver,
                            angular_b_pointer, scalar_work.inverse_b, result + 1,
                            projection_pool.normal_second_b_copy);
                        normal_completion = normal_complete;
                    }
                    else
                    {
                        normal_completion = normal_fallback;
                    }
                }
                // The normal fallback follows the unilateral loop. Both normal
                // outcomes then use the same stream and result advancement.
                case 2:
                {
                    if (normal_completion == not_normal)
                    {
                        solver_scoped_body_fields_31c940 const &body_b = scoped_b;
                        __m128 *linear_b_pointer = &body_b.body->linear;
                        __m128 *&angular_b_pointer = scalar_work.angular_b;
                        angular_b_pointer = &body_b.body->angular;
                        do
                        {
                            solve_unilateral_linear(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3],
                                ((solver_linear_row_31e7b0 *)terms), a, scoped_b,
                                *linear_b_pointer, angular_b_pointer, step, result,
                                projection_pool, scalar_work.fallback_scalar);
                            terms += 3; ++result; op += 4;
                        } while (*op == 2);
                        normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
                        continue;
                    }
                    if (normal_completion == normal_fallback)
                    {
                        bool choose_first = first_positive;
                        float candidate_b;
                        if (!first_positive)
                        {
                            candidate_b = error_b * terms[4].m128_f32[3];
                            // Unordered second candidates enter the update.
                            choose_first = candidate_b <= 0.0f;
                        }
                        if (choose_first)
                        {
                            float candidate_a = scalar_work.pair_b * terms[1].m128_f32[3];
                            if (candidate_a > 0.0f)
                                apply_second_pair_fallback(candidate_a,
                                    ((solver_linear_row_31e7b0 *)terms), a, linear_input,
                                    angular_b_pointer, scoped_b.body->inverse_inertia,
                                    first_linear_axis, result, scalar_work.fallback_scalar);
                        }
                        else
                        {
                            apply_second_normal_row_fallback_31c940(candidate_b,
                                ((solver_linear_row_31e7b0 *)terms) + 1,
                                scalar_work.next_normal, update_cursor, a, linear_input,
                                scoped_b, angular_b_pointer, result + 1, scalar_work.fallback_scalar);
                        }
                    }
                    // Observe the following kind after all row writes, before advancing.
                    if (*((signed char *)op + 8) != 8)
                    {
                        op += 8; terms += 6; result += 2;
                        normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
                        continue;
                    }
                    op += 8; terms += 6; result += 2;
                    break;
                }
                case 8: break;

				case 1:
					header_complete = true;
					continue;
				// Two tangential rows share a circular friction limit.
				case 7:
				{
					float target_a = ((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
					__m128 *linear_b=&b->linear;
					circular_first_projection_fields(((solver_linear_row_31e7b0*)terms),a,b,scalar_work.angular_b,projection_pool.circular_first);
					project_second_pair_second_row(((solver_linear_row_31e7b0*)terms)+1,a,*linear_b,*scalar_work.angular_b,((solver_linear_row_31e7b0*)terms)[1].linear,projection_pool.circular_second);
					// Both projections finish before these shared parameter reads.
					solver_step_31c940 *parameters = step;
					scalar_work.fallback_scalar = target_a * parameters->friction_scale - projection_pool.circular_first * parameters->velocity_scale;
					// Retain the row target before its scale product.
					float target_b = ((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3];
					scalar_work.pair_a = target_b * parameters->friction_scale
						- projection_pool.circular_second * parameters->velocity_scale;
					float maximum = operation_float(op, 12);
					// Round the circular limit to its float home before comparison and
					// division; the reads can then use that home directly.
					float &maximum_squared = scalar_work.pair_b;
					float &maximum_store = maximum_squared;
					maximum_store = maximum * maximum;
					// Capture each mass before evaluating the coupled row.
					float const &mass_a_source = ((solver_linear_row_31e7b0 *)terms)[0].b.m128_f32[3];
					circular_mass_31c940 mass_a(mass_a_source);
					float impulse_a = circular_coupled_impulse_31c940(mass_a,
						scalar_work.fallback_scalar, scalar_work.pair_a, operation_float(op, 8));
					float const &mass_b_source = ((solver_linear_row_31e7b0 *)terms)[1].b.m128_f32[3];
					circular_mass_31c940 mass_b(mass_b_source);
					float impulse_b = circular_coupled_impulse_31c940(mass_b,
						scalar_work.pair_a, scalar_work.fallback_scalar, operation_float(op, 8));
					// Retain the first tangential square before adding the second.
					float norm = impulse_a * impulse_a;
					norm += impulse_b * impulse_b;
					if (norm > maximum_squared)
					{
						float factor = sqrtf(maximum_squared / norm);
						impulse_a *= factor; impulse_b *= factor;
						((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3] *= factor;
						((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3] *= factor;
					}
					__m128 const *inverse_b=apply_circular_first_fields(impulse_a,((solver_linear_row_31e7b0*)terms),a,scoped_b,*linear_b,scalar_work.angular_b,scalar_work.inverse_b,scalar_work.next_circular,result,scalar_work.simd_scalar);
complete_circular_pair_31c940(impulse_b,scalar_work.next_circular,normal_axes,terms,a,*linear_b,scalar_work.angular_b,inverse_b,result,scalar_work.simd_scalar);

					op += 20;
					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Angular friction with a scalar limit.
				case 9:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					// Publish the friction-scale home before the captured inertia address.
					float const *&friction_scale_pointer = scalar_work.friction_scale;
					friction_scale_pointer = &step->friction_scale;
					__m128 const *&inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					__m128 *velocity_b_pointer = &body_b.body->angular;
					do
					{
						// The target precedes the limit store, including when they share storage.
						float target=((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3];
						solve_angular_friction_kernel(target,state.solving.angular_limit,operation_float(op,4),friction_scale_pointer,
							((solver_angular_row_31e7b0 *)terms), step, a, *velocity_b_pointer, inverse_b_pointer, result, projection_pool, scalar_work.simd_scalar);
						terms += 2; ++result; op += 8;
					} while (*op == 9);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Angular limits.
				case 10:
				{
					__m128 &velocity_b = scoped_b.body->angular;
					do
					{

						// Retain the two projection inputs with the first angular axis.
						// Reuse them for the consumed body-update values.
						struct { __m128 velocity_a, velocity_b, axis_a; } work;
						__m128 axis_b;
						__m128 inertia;
						work.velocity_a = a->angular;
						work.velocity_b = velocity_b;
						work.axis_a = ((solver_angular_row_31e7b0 *)terms)->a;
						axis_b = ((solver_angular_row_31e7b0 *)terms)->b;
						work.velocity_b = _mm_mul_ps(work.velocity_b, axis_b);
						work.velocity_a = _mm_mul_ps(work.velocity_a, work.axis_a);
						work.velocity_a = _mm_add_ps(work.velocity_a, work.velocity_b);
						work.velocity_b = _mm_shuffle_ps(work.velocity_a, work.velocity_a, 0x55);
						inertia = _mm_shuffle_ps(work.velocity_a, work.velocity_a, 0xaa);
						work.velocity_b = _mm_add_ss(work.velocity_b, work.velocity_a);
						inertia = _mm_add_ss(inertia, work.velocity_b);
						_mm_store_ss(&projection_pool.angular_limit, inertia);
						bool active;
						float impulse = select_angular_limit(projection_pool.angular_limit * load_velocity_scale(step),
						    ((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3], operation_float(op, 4), operation_float(op, 8),
						    operation_float(op, 12), ((solver_angular_row_31e7b0 *)terms)->a.m128_f32[3], active, work.velocity_b, scalar_work.simd_scalar);
						if (active)
						{
						    inertia = a->inverse_inertia;
						    work.velocity_a = _mm_shuffle_ps(work.velocity_b, work.velocity_b, 0);
						    work.velocity_a = _mm_mul_ps(work.velocity_a, inertia);
						    inertia = scoped_b.body->inverse_inertia;
						    work.velocity_a = _mm_mul_ps(work.velocity_a, work.axis_a);
						    work.axis_a = a->angular;
						    work.axis_a = _mm_add_ps(work.axis_a, work.velocity_a);
						    a->angular = work.axis_a;
						    work.velocity_a = velocity_b;
						    work.velocity_b = _mm_shuffle_ps(work.velocity_b, work.velocity_b, 0);
						    work.velocity_b = _mm_mul_ps(work.velocity_b, inertia);
						    work.velocity_b = _mm_mul_ps(work.velocity_b, axis_b);
						    work.velocity_a = _mm_add_ps(work.velocity_a, work.velocity_b);
						    velocity_b = work.velocity_a;
						    *result += impulse;
						}
						terms += 2; ++result; op += 16;
					} while (*op == 10);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// A bounded angular impulse.
				case 4:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					__m128 const * &inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					float *velocity_b_pointer = (float *)&body_b.body->angular;
					do
					{
						float const &mass_source = ((solver_angular_row_31e7b0 *)terms)->a.m128_f32[3];
						double mass = mass_source;
						bounded_target_31c940 target(((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3]);
						solve_bounded_angular(mass, target, ((solver_angular_row_31e7b0 *)terms), a, velocity_b_pointer, inverse_b_pointer,
							operation_float(op, 4), operation_float(op, 8), step, result, projection_pool, scalar_work.simd_scalar);
						terms += 2; ++result; op += 16;
					} while (*op == 4);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Angular error correction.
				case 12:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					scalar_work.angular_b = &body_b.body->angular;
					__m128 const * &inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					__m128 *velocity_a_pointer = &a->angular;
					do
					{

						// Retain this row's receiver; the next row observes the published home.
						__m128 *velocity_b_pointer = scalar_work.angular_b;
						__m128 w[5];
						w[2]=terms[0];
						float mass=terms[0].m128_f32[3];
						w[3]=terms[1];
						float target=terms[1].m128_f32[3];
						w[0]=*velocity_a_pointer;
						w[1]=*velocity_b_pointer;
						w[0]=_mm_mul_ps(w[0],w[2]);
						w[1]=_mm_mul_ps(w[1],w[3]);
						w[0]=_mm_add_ps(w[0],w[1]);
						w[1]=_mm_shuffle_ps(w[0],w[0],0x55);
						w[1]=_mm_add_ss(w[1],w[0]);
						w[4]=_mm_shuffle_ps(w[0],w[0],0xaa);
						w[4]=_mm_add_ss(w[4],w[1]);
						_mm_store_ss(&projection_pool.angular_error,w[4]);
						solver_step_31c940 *parameters=step;
						w[4]=a->inverse_inertia;
						float impulse=mass*(target*parameters->error_scale-projection_pool.angular_error*parameters->velocity_scale);
						op+=4;terms+=2;++result;
						scalar_work.simd_scalar=impulse;
						w[1]=_mm_load_ss(&scalar_work.simd_scalar);
						w[0]=_mm_shuffle_ps(w[1],w[1],0);
						w[0]=_mm_mul_ps(w[0],w[4]);
						w[4]=*inverse_b_pointer;
						__m128 *receiver=velocity_b_pointer;
						w[0]=_mm_mul_ps(w[0],w[2]);
						w[2]=*velocity_a_pointer;
						w[2]=_mm_add_ps(w[2],w[0]);
						*velocity_a_pointer=w[2];
						w[0]=*receiver;
						w[1]=_mm_shuffle_ps(w[1],w[1],0);
						w[1]=_mm_mul_ps(w[1],w[4]);
						w[1]=_mm_mul_ps(w[1],w[3]);
						w[0]=_mm_add_ps(w[0],w[1]);
						*receiver=w[0];
						result[-1]+=impulse;
} while (*op == 12);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Linear error correction.
				case 13:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					float * &linear_b_pointer = scalar_work.linear_floats_b;
					linear_b_pointer = (float *)&body_b.body->linear;
					__m128 * &angular_b_pointer = scalar_work.angular_b;
					angular_b_pointer = &body_b.body->angular;
					__m128 const *&inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					__m128 *linear_a_pointer = &a->linear;
					// Retain this row receiver, then refresh it after result publication.
					float *linear_b_live = linear_b_pointer;
					do
					{
						solve_linear_error_cursor_31c940(((solver_linear_row_31e7b0*)terms)->linear.m128_f32[3],
						((solver_linear_row_31e7b0*)terms)->a.m128_f32[3],terms,a,linear_a_pointer,linear_b_live,
						angular_b_pointer,inverse_b_pointer,step,result,op,projection_pool,scalar_work.simd_scalar);
						linear_b_live = linear_b_pointer;
					} while (*op == 13);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Linear friction with a scalar limit.
				case 6:
    {
     solver_scoped_body_fields_31c940 const &body_b = scoped_b;
     // Publish the scale home before capturing the body's field addresses.
     float const *&friction_scale_pointer = scalar_work.friction_scale;
     friction_scale_pointer = &step->friction_scale;
     solver_velocity_31c940 *captured_b = body_b.body;
     __m128 const *&inverse_b_pointer = scalar_work.inverse_b;
     inverse_b_pointer = &captured_b->inverse_inertia;
     __m128 *linear_b_pointer = &captured_b->linear;
     __m128 *&angular_b_pointer = scalar_work.angular_b;
     angular_b_pointer = &captured_b->angular;

					do
					{
						float target=((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
						solve_linear_friction(target, state.solving.linear_limit,operation_float(op,4), friction_scale_pointer,
							((solver_linear_row_31e7b0 *)terms), a, angular_b_pointer, *linear_b_pointer, inverse_b_pointer, step, result, projection_pool, scalar_work.simd_scalar);
						terms += 3; ++result; op += 8;
					} while (*op == 6);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Independent lower and upper linear limits.
				case 11:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					// Retain the linear field address across both bound updates.
					float * linear_b_pointer = (float *)&body_b.body->linear;
					__m128 * &angular_b_pointer = scalar_work.angular_b;
					angular_b_pointer = &body_b.body->angular;
					do
					{
						solver_linear_row_31e7b0 const *row = (solver_linear_row_31e7b0 const *)terms;

						// Reuse consumed projection values while retaining the first bound axis.
						struct
						{
							__m128 first;
							__m128 second;
						} projection;
						__m128 temporary;
						__m128 contribution;
						__m128 first_linear_axis;
						projection.first=_mm_load_ps(linear_b_pointer);
						projection.second=a->linear;
						temporary=row->a;
						contribution=*angular_b_pointer;
						first_linear_axis=row->linear;
						projection.second=_mm_sub_ps(projection.second,projection.first);
						projection.first=a->angular;
						projection.first=_mm_mul_ps(projection.first,temporary);
						temporary=row->b;
						contribution=_mm_mul_ps(contribution,temporary);
						projection.second=_mm_mul_ps(projection.second,first_linear_axis);
						projection.first=_mm_add_ps(projection.first,contribution);
						projection.first=_mm_add_ps(projection.first,projection.second);
						sum_three_to_arena_31c940(projection.first,projection_pool.linear_bounds);
						// Capture the step pointer for velocity and the first bound. The
						// second bound reloads it after any first-bound result write.
						solver_step_31c940 *parameters = step;
						float velocity = projection_pool.linear_bounds * parameters->velocity_scale;
						float low = (((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3] - operation_float(op, 4)) * parameters->error_scale - velocity;
						if (low > 0.0f)
							update_linear_limit_values(low * ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), first_linear_axis, a, scoped_b.body->inverse_inertia, linear_b_pointer, angular_b_pointer, result, scalar_work.simd_scalar);
						float high = (((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3] - operation_float(op, 8)) * load_error_scale(step) - velocity;
						if (high < 0.0f)
						{
							// Capture the axis before the rounded input can replace its W lane.
							__m128 high_axis = _mm_load_ps(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32);
							update_linear_limit_values(high * ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), high_axis, a, scoped_b.body->inverse_inertia, linear_b_pointer, angular_b_pointer, result, scalar_work.simd_scalar);
						}
						terms += 3; ++result; op += 12;
					} while (*op == 11);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// A bounded linear impulse.
				case 5:
				{
					do
					{
						solver_scoped_body_fields_31c940 const &body_b = scoped_b;
						// Capture mass before the target and retain the target in the
						// x87 scalar path until the impulse is formed.
						// This local also carries the mass-scaled error through the step scale.
						float mass = ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3];
						double target = ((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
						__m128 *linear_b_pointer = &body_b.body->linear;
						__m128 *angular_b_pointer = &body_b.body->angular;
						// Match the same mass-before-step-scale order as the angular path.

						// Reuse the consumed projection vectors through the bounded update.
						struct
						{
							__m128 contribution;
							__m128 temporary;
						} work;
						__m128 relative;
						__m128 angular_b;
						__m128 linear_axis;
						work.contribution=*linear_b_pointer;
						relative=a->linear;
						work.temporary=((solver_linear_row_31e7b0*)terms)->a;
						angular_b=*angular_b_pointer;
						linear_axis=((solver_linear_row_31e7b0*)terms)->linear;
						relative=_mm_sub_ps(relative,work.contribution);
						work.contribution=a->angular;
						work.contribution=_mm_mul_ps(work.contribution,work.temporary);
						work.temporary=((solver_linear_row_31e7b0*)terms)->b;
						angular_b=_mm_mul_ps(angular_b,work.temporary);
						relative=_mm_mul_ps(relative,linear_axis);
						work.contribution=_mm_add_ps(work.contribution,angular_b);
						work.contribution=_mm_add_ps(work.contribution,relative);
						sum_three_to_arena_31c940(work.contribution,projection_pool.bounded_linear);
						mass = (target - projection_pool.bounded_linear) * mass;
						float impulse = mass * load_error_scale(step);
						// Keep the divide followed by multiply on both clamp paths;
						// reassociating them changes rounding at 24-bit x87 precision.
						if (impulse > operation_float(op, 4))
						{
							float ratio = operation_float(op, 4) / impulse;
							impulse = ratio * impulse;
						}
						else if (impulse < operation_float(op, 8))
						{
							float ratio = operation_float(op, 8) / impulse;
							impulse = ratio * impulse;
						}
						// Reload the saved A receiver at the bounded update boundary.
						apply_linear(impulse, ((solver_linear_row_31e7b0 *)terms), state.solving.a_home, scoped_b, result, scalar_work.simd_scalar,
							work.contribution,relative,work.temporary,angular_b,linear_axis);
						terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(1);
						a = state.solving.a_home; ++result; op += 16;
					} while (*op == 5);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				case 15:
				{
					solver_step_31c940 *parameters = ((solver_step_31c940 *const &)step);
					state.solving.saved_velocity = parameters->velocity_scale;
					state.solving.saved_error = parameters->error_scale;
					parameters->velocity_scale = operation_float(op, 8);
					parameters->error_scale = operation_float(op, 4);
					op += 12;
					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				case 16:
					step->error_scale = state.solving.saved_error; step->velocity_scale = state.solving.saved_velocity;
					op += 4;
					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				case 14:
					(*(void (__cdecl **)(__m128 *, void *))(op + 4))(terms, op + 8);
					op += op[1];
				a = state.solving.a_home;
					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;

				default:
					// Redispatch unknown kinds through the current row-axis address.
					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}

				// Both entry paths consume the same three-axis friction record.
				{

					solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(((solver_linear_row_31e7b0 *)terms) + 2);
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					__m128 * &linear_b_pointer = scalar_work.linear_b;

					__m128 *angular_b_pointer;
					float spin_mass = spin->a.m128_f32[3], spin_target = spin->b.m128_f32[3];

					spin_projection_output_31c940 projection_output(projection_pool.spin_first,linear_b_pointer,&body_b.body->linear);
					project_spin_contact_rows_31c940((solver_linear_row_31e7b0 const *)terms,a,body_b.body,linear_b_pointer,angular_b_pointer,projection_output,projection_pool);
scalar_work.first_error = (spin_target * load_friction_scale(step) - projection_pool.spin_angular * load_velocity_scale(step)) * spin_mass;
					// Observe each target before multiplying by the current step scale.
					float captured_target_a = (float const &)((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3];
					scalar_work.pair_b = captured_target_a * load_friction_scale(step) - projection_pool.spin_first * load_velocity_scale(step);
					double captured_target_b = (float const &)((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3];
					// Complete the second error before publishing the limit, then round it to its float home.
					double spin_error = captured_target_b * load_friction_scale(step)
                        - projection_pool.spin_second * load_velocity_scale(step);
                    state.solving.spin_limit = operation_float(op, 12);
                    scalar_work.fallback_scalar = spin_error;
					// Keep limited impulses in native x87 intermediates until their stores.
					// The norm and ratio use native scalar locals; SIMD rounds separately.
					double impulse_a = ((solver_linear_row_31e7b0 *)terms)[0].b.m128_f32[3] * scalar_work.pair_b + scalar_work.fallback_scalar * operation_float(op, 8);
					double impulse_b = ((solver_linear_row_31e7b0 *)terms)[1].b.m128_f32[3] * scalar_work.fallback_scalar + scalar_work.pair_b * operation_float(op, 8);
					// Reload after the tangential impulses to retain spin-squared first
					// in the x87 norm accumulation.
					double impulse_spin = scalar_work.first_error;
					float norm = impulse_spin * impulse_spin + impulse_b * impulse_b + impulse_a * impulse_a;
					if (norm > state.solving.spin_limit * state.solving.spin_limit)
					{
						// The norm comparison uses the saved limit; division reloads the
						// live record after the tangential error was published.
						float factor = operation_float(op, 12) / sqrt(norm);
						impulse_a *= factor; impulse_b *= factor; impulse_spin *= factor;
						((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3] *= factor;
						((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3] *= factor;
						spin->b.m128_f32[3] *= factor;
					}
					// Scale and round spin before either linear update can change ((solver_linear_row_31e7b0 *)terms) storage.
					// Capture both values before spin and inverse-pointer publication.
__m128 first_inertia_a=a->inverse_inertia;
__m128 first_linear_axis=((solver_linear_row_31e7b0 *)terms)->linear;
solver_velocity_31c940 *inverse_body_b=body_b.body;
scalar_work.first_error = impulse_spin * operation_float(op, 20);
					__m128 const * &inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &inverse_body_b->inverse_inertia;
					__m128 const *second_inverse_b=apply_spin_first_fields(impulse_a,((solver_linear_row_31e7b0 *)terms),a,
 first_inertia_a,first_linear_axis,linear_b_pointer,*angular_b_pointer,inverse_b_pointer,scalar_work.next_circular,result,scalar_work.fallback_scalar);
unsigned long spin_bits=apply_spin_second_fields(impulse_b,scalar_work.next_circular,normal_axes,a,
 linear_b_pointer,*angular_b_pointer,second_inverse_b,scalar_work.first_error,result+1,scalar_work.simd_scalar);
{
						// Accumulation sees the new source; SIMD consumes the pre-store snapshot.
						__m128 w[5];
						w[2]=a->inverse_inertia;
						float impulse=scalar_work.first_error;
						memcpy(&projection_pool.spin_angular_copy,&spin_bits,sizeof(projection_pool.spin_angular_copy));
						w[1]=_mm_load_ss(&projection_pool.spin_angular_copy);
						w[0]=_mm_shuffle_ps(w[1],w[1],0);
						w[0]=_mm_mul_ps(w[0],w[2]);
						w[2]=*inverse_b_pointer;
						w[1]=_mm_shuffle_ps(w[1],w[1],0);
						w[1]=_mm_mul_ps(w[1],w[2]);
						w[2]=spin->a;
						w[0]=_mm_mul_ps(w[0],w[2]);
						w[2]=spin->b;
						w[1]=_mm_mul_ps(w[1],w[2]);
						w[2]=a->angular;
						w[2]=_mm_add_ps(w[2],w[0]);
						a->angular=w[2];
						w[0]=(*angular_b_pointer);
						w[0]=_mm_add_ps(w[0],w[1]);
						(*angular_b_pointer)=w[0];
						*(result+2)+=impulse;
					}
					terms += 8; result += 3;
					op += 24;

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
				}
			}

		} while (!dispatch_done);


		process_velocity_epoch_31c940(state.solving.iteration, bodies);
// The third operation byte classifies the residual row layout.
		{

			// This pass keeps B in a register across consecutive residual rows.
			solver_velocity_31c940 *b;
			op = operations;
			// Retail advances the impulse cursor with each residual row, while
			// this pass writes the remaining errors back into the row stream.
			result = impulses;

			solver_impulse_cursor_31c940 result_cursor(result);
			signed char residual_kind;
			solver_header_31c940 *first_residual_header = (solver_header_31c940 *)op;
			a = first_residual_header->a; b = first_residual_header->b;
			terms = first_residual_header->terms; op += 24;
			// Carry the class through grouped rows; class zero finishes this pass.

			
			// The outer entry fetches a class. The inner entry reuses the class
   // captured after the first pair, before any next-pair publication.
   bool residual_done = false;
   do
   {
    residual_kind = (signed char)op[2];
    for (;;)
			{
				switch (residual_kind)
				{
				case 0:
     residual_done = true;
     break;
				case 1:
				{
					solver_header_31c940 *header = (solver_header_31c940 *)op;
					a = header->a; b = header->b; terms = header->terms; op += 24;
					break;
				}
				case 5:
				{
					// Consume up to two paired records, then join the friction path.
					{
						__m128 first, second;
						{
       __m128 work_0, work_1, work_2, work_3, work_4, work_5, work_6;
								solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
								// Retain the shared inputs across both projections.
								// Reuse each input only after its final multiplication.
								work_3 = a->angular;
								work_1 = row[0].a;
								work_2 = b->linear;
								work_5 = row[0].b;
								work_0 = work_3;
								work_0 = _mm_mul_ps(work_0, work_1);
								work_1 = row[1].a;
								work_3 = _mm_mul_ps(work_3, work_1);
								work_1 = a->linear;
								work_1 = _mm_sub_ps(work_1, work_2);
								work_2 = b->angular;
								work_6 = work_1;
								work_4 = work_2;
								work_4 = _mm_mul_ps(work_4, work_5);
								work_5 = row[1].b;
								work_2 = _mm_mul_ps(work_2, work_5);
								work_3 = _mm_add_ps(work_3, work_2);
								work_5 = row[0].linear;
								work_0 = _mm_add_ps(work_0, work_4);
								work_6 = _mm_mul_ps(work_6, work_5);
								work_0 = _mm_add_ps(work_0, work_6);
								work_4 = row[1].linear;
								work_1 = _mm_mul_ps(work_1, work_4);
								work_3 = _mm_add_ps(work_3, work_1);
								work_2 = _mm_shuffle_ps(work_3, work_3,0xaa);
								work_1 = _mm_shuffle_ps(work_3, work_3,0x55);
								work_1 = _mm_add_ss(work_1, work_3);
								work_3 = work_2;
								work_2 = work_0;
								work_2 = _mm_shuffle_ps(work_2, work_2,0xaa);
								work_3 = _mm_add_ss(work_3, work_1);

								work_1 = _mm_shuffle_ps(work_0, work_0,0x55);
								work_1 = _mm_add_ss(work_1, work_0);
								work_2 = _mm_add_ss(work_2, work_1);
								first = work_2;
second = work_3;
      }
						_mm_store_ss(&projection_pool.residual_normal_first, first);
						terms[0].m128_f32[3] -= projection_pool.residual_normal_first;
						_mm_store_ss(&projection_pool.residual_normal_second, second);
						terms[3].m128_f32[3] -= projection_pool.residual_normal_second;
						terms += 6;
					}
					result_cursor.advance(2); op += op[1];
					// Redispatch the classification already read for this pair.
					residual_kind = (signed char)op[2];
					if (residual_kind != 5) continue;
					{
						__m128 first, second;
						{
       __m128 work_0, work_1, work_2, work_3, work_4, work_5, work_6;
								solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
								// Retain the shared inputs across both projections.
								// Reuse each input only after its final multiplication.
								work_3 = a->angular;
								work_1 = row[0].a;
								work_2 = b->linear;
								work_5 = row[0].b;
								work_0 = work_3;
								work_0 = _mm_mul_ps(work_0, work_1);
								work_1 = row[1].a;
								work_3 = _mm_mul_ps(work_3, work_1);
								work_1 = a->linear;
								work_1 = _mm_sub_ps(work_1, work_2);
								work_2 = b->angular;
								work_6 = work_1;
								work_4 = work_2;
								work_4 = _mm_mul_ps(work_4, work_5);
								work_5 = row[1].b;
								work_2 = _mm_mul_ps(work_2, work_5);
								work_3 = _mm_add_ps(work_3, work_2);
								work_5 = row[0].linear;
								work_0 = _mm_add_ps(work_0, work_4);
								work_6 = _mm_mul_ps(work_6, work_5);
								work_0 = _mm_add_ps(work_0, work_6);
								work_4 = row[1].linear;
								work_1 = _mm_mul_ps(work_1, work_4);
								work_3 = _mm_add_ps(work_3, work_1);
								work_2 = _mm_shuffle_ps(work_3, work_3,0xaa);
								work_1 = _mm_shuffle_ps(work_3, work_3,0x55);
								work_1 = _mm_add_ss(work_1, work_3);
								work_3 = work_2;
								work_2 = work_0;
								work_2 = _mm_shuffle_ps(work_2, work_2,0xaa);
								work_3 = _mm_add_ss(work_3, work_1);

								work_1 = _mm_shuffle_ps(work_0, work_0,0x55);
								work_1 = _mm_add_ss(work_1, work_0);
								work_2 = _mm_add_ss(work_2, work_1);
								first = work_2;
second = work_3;
      }
						_mm_store_ss(&projection_pool.residual_bounded_first, first);
						terms[0].m128_f32[3] -= projection_pool.residual_bounded_first;
						_mm_store_ss(&projection_pool.residual_bounded_second, second);
						terms[3].m128_f32[3] -= projection_pool.residual_bounded_second;
						terms += 6;
					}
					result_cursor.advance(2); op += op[1];
					if (op[2] != 6) break;
				}
				case 6:
				{
					{
						// Own the pair temporaries until both projections are reduced.
						// Advance the pair and angular row together after publishing both errors.
						{
							__m128 first;
							{
								__m128 work_0, work_1, work_2, work_3, work_4, work_5, work_6;
								solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
								// Retain the shared inputs across both projections.
								// Reuse each input only after its final multiplication.
								work_3 = a->angular;
								work_1 = row[0].a;
								work_2 = b->linear;
								work_5 = row[0].b;
								work_0 = work_3;
								work_0 = _mm_mul_ps(work_0, work_1);
								work_1 = row[1].a;
								work_3 = _mm_mul_ps(work_3, work_1);
								work_1 = a->linear;
								work_1 = _mm_sub_ps(work_1, work_2);
								work_2 = b->angular;
								work_6 = work_1;
								work_4 = work_2;
								work_4 = _mm_mul_ps(work_4, work_5);
								work_5 = row[1].b;
								work_2 = _mm_mul_ps(work_2, work_5);
								work_3 = _mm_add_ps(work_3, work_2);
								work_5 = row[0].linear;
								work_0 = _mm_add_ps(work_0, work_4);
								work_6 = _mm_mul_ps(work_6, work_5);
								work_0 = _mm_add_ps(work_0, work_6);
								work_4 = row[1].linear;
								work_1 = _mm_mul_ps(work_1, work_4);
								work_3 = _mm_add_ps(work_3, work_1);
								work_2 = _mm_shuffle_ps(work_3, work_3,0xaa);
								work_1 = _mm_shuffle_ps(work_3, work_3,0x55);
								work_1 = _mm_add_ss(work_1, work_3);
								work_3 = work_2;
								work_2 = work_0;
								work_2 = _mm_shuffle_ps(work_2, work_2,0xaa);
								work_3 = _mm_add_ss(work_3, work_1);

								work_1 = _mm_shuffle_ps(work_0, work_0,0x55);
								work_1 = _mm_add_ss(work_1, work_0);
								work_2 = _mm_add_ss(work_2, work_1);
								first = work_2;

								_mm_store_ss(&projection_pool.residual_circular_first, first);
								terms[0].m128_f32[3] -= projection_pool.residual_circular_first;
								_mm_store_ss(&projection_pool.residual_circular_second, work_3);
								terms[3].m128_f32[3] -= projection_pool.residual_circular_second;
								terms += 8;
							}
						}
						solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(terms-2);
						{
							// Retain the first axis with A's consumed angular contribution.
							struct { __m128 axis, contribution; } angular_inputs;
							angular_inputs.axis=spin->a;
							angular_inputs.contribution=a->angular;
							__m128 second_contribution=b->angular;
							angular_inputs.contribution=_mm_mul_ps(angular_inputs.contribution,angular_inputs.axis);
							angular_inputs.axis=spin->b;
							second_contribution=_mm_mul_ps(second_contribution,angular_inputs.axis);
							angular_inputs.contribution=_mm_add_ps(angular_inputs.contribution,second_contribution);
							angular_inputs.axis=_mm_shuffle_ps(angular_inputs.contribution,angular_inputs.contribution,0x55);
							angular_inputs.axis=_mm_add_ss(angular_inputs.axis,angular_inputs.contribution);
							second_contribution=_mm_shuffle_ps(angular_inputs.contribution,angular_inputs.contribution,0xaa);
							second_contribution=_mm_add_ss(second_contribution,angular_inputs.axis);
							_mm_store_ss(&projection_pool.residual_spin,second_contribution);
							spin->b.m128_f32[3]=spin->b.m128_f32[3]-((float const &)projection_pool.residual_spin);
						}
						result_cursor.advance(3);
						op += op[1];
					}
					// The header opcode takes precedence over its classification byte.
					if (*op == 1)
					{
						solver_header_31c940 *header = (solver_header_31c940 *)op;
						a = header->a; b = header->b; terms = header->terms; op += 24;
					}
					break;
				}
				case 3:
				{
					solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
					do
					{
						project_residual_linear_velocity(row, a, b, row->linear, projection_pool.residual_linear);
						row->linear.m128_f32[3] -= projection_pool.residual_linear;
						++row; result_cursor.advance(1); op += op[1];
					} while (op[2] == 3);
					terms = (__m128 *)row;
					break;
				}
				case 4:
				{
					solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
					do
					{
						subtract_angular_residual(row,a,b, projection_pool.residual_angular);
						++row; result_cursor.advance(1); op += op[1];
					} while (op[2] == 4);
					terms = (__m128 *)row;
					break;
				}
				case 2: op += op[1]; break;

				default: break;
				}
				break;

			}
   } while (!residual_done);

		}
		// The limit may partly overlap the counter home. Retain both inputs
		// before publishing the increment, as in the retail epilogue.
		struct { long counter, limit; } next;
		next.counter = state.solving.iteration;
		next.limit = step->iterations;
		++next.counter;
		state.solving.iteration = next.counter;
		iteration_count = next.limit;
	}

	// Publish impulses and remaining error using each header's destination and
	// result stride. Contact friction records carry a separate result destination.
	float *result = impulses;
	unsigned char *op = operations;
	__m128 *terms;
	unsigned char *destination;
	for (;;)
	{

		solver_header_31c940 *first_output_header = (solver_header_31c940 *)op;
		long captured_stride = first_output_header->result_stride;
		terms = first_output_header->terms;
		destination = first_output_header->results;
		state.writing.stride = captured_stride;
		op += 24;
		for (;;)
		{
			// Retain the dispatch value while publishing its home for repeated rows.
			int current_kind = *(signed char *)op;
			state.writing.kind = current_kind;
			switch (current_kind)
			{
			case 1: break;

			case 3:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;

				// Keep each stride read in retail order and retain the coefficient
				// address through both outputs, which may overlap the input buffers.
				unsigned long first_impulse_bits;
				memcpy(&first_impulse_bits, result, sizeof(first_impulse_bits));
				long first_stride = state.writing.stride;
				memcpy(destination, &first_impulse_bits, sizeof(first_impulse_bits));
				float const &inverse_time = capture_writeback_time(step);
				*(float *)(destination + 4) = row[0].linear.m128_f32[3] * inverse_time;
				destination += first_stride;
				float *next_result = result + 1;
				*(float *)destination = *next_result;
				long const &stride_again = state.writing.stride;
				long second_stride = stride_again;
				*(float *)(destination + 4) = row[1].linear.m128_f32[3] * inverse_time;
				destination += second_stride;
				terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
				result = next_result + 1; op += 8; continue;
			}
			case 2: case 5: case 6: case 11: case 13:
			{
				// Keep the coefficient address across the cursor calls.
				float const *&inverse_time_pointer = scalar_work.writeback_time;
				inverse_time_pointer = &step->inverse_time;
				do
				{
					// Retain this row's time address before the impulse can replace it.
					unsigned long output_impulse_bits;
					memcpy(&output_impulse_bits, result, sizeof(output_impulse_bits));
					float const *output_time = inverse_time_pointer;
					memcpy(destination, &output_impulse_bits, sizeof(output_impulse_bits));
					// Retain stride after the impulse, before the error can replace it.
					long output_stride = state.writing.stride;
					*(float *)(destination + 4) = terms[0].m128_f32[3] * *output_time;
					destination += output_stride;
					terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(1);
					++result; op += op[1];
				} while (*(signed char *)op == state.writing.kind);
				continue;
			}
			case 4: case 9: case 10: case 12:
			{
				float const *&inverse_time_pointer = scalar_work.writeback_time;
				inverse_time_pointer = &step->inverse_time;
				do
				{
					// Capture anew each row, retaining the address through its outputs.
					unsigned long output_impulse_bits;
					memcpy(&output_impulse_bits, result, sizeof(output_impulse_bits));
					float const *output_time = inverse_time_pointer;
					memcpy(destination, &output_impulse_bits, sizeof(output_impulse_bits));
					// Capture between the two outputs, including overlapping stack homes.
					long output_stride = state.writing.stride;
					*(float *)(destination + 4) = terms[1].m128_f32[3] * *output_time;
					destination += output_stride;
					terms = (__m128 *)((s_angular_cursor_31c520 *)terms)->offset(1);
					++result; op += op[1];
				} while (*(signed char *)op == state.writing.kind);
				continue;
			}
			case 7:
			{
				unsigned char *friction_result = *(unsigned char **)(op + 4);
				// Earlier output may replace this word; read it after each store.
				long const &friction_stride = *(long const *)(op + 16);
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				*(float *)friction_result = result[0];
				// Capture the error before publishing the pointer its input can alias.
				float first_error = row[0].linear.m128_f32[3] * step->inverse_time;
				float const *&inverse_time_pointer = scalar_work.writeback_time;
				inverse_time_pointer = &step->inverse_time;
				// The error output may replace the saved pointer for later rows.
				*(float *)(friction_result + 4) = first_error;
				friction_result += friction_stride;
				*(float *)friction_result = result[1];
				*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * *inverse_time_pointer;
				terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
				result += 2; op += 20; continue;
			}
			case 8:
			{
				unsigned char *friction_result = *(unsigned char **)(op + 4);
				// Earlier output may replace this word; read it after each store.
				long const &friction_stride = *(long const *)(op + 16);
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				*(float *)friction_result = result[0];
				// Capture the error before publishing the pointer its input can alias.
				float first_error = row[0].linear.m128_f32[3] * step->inverse_time;
				float const *&inverse_time_pointer = scalar_work.writeback_time;
				inverse_time_pointer = &step->inverse_time;
				// The error output may replace the saved pointer for later rows.
				*(float *)(friction_result + 4) = first_error;
				destination = 0;
				friction_result += friction_stride;
				*(float *)friction_result = result[1];
				*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * *inverse_time_pointer;

				friction_result += friction_stride;

				*(float *)friction_result = result[2];
				*(float *)(friction_result + 4) = terms[7].m128_f32[3] * *inverse_time_pointer;
				terms = (__m128 *)((s_angular_cursor_31c520 *)(terms + 6))->offset(1);

				result += 3; op += 24;
				continue;
			}
			case 0: return 1;
			case 14: case 15: case 16: op += op[1]; continue;

			default: continue;
			}
			break;
		}
	}
}
