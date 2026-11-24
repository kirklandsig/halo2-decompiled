// @flags /O2 /GL-
// Iterative constraint solving, residual updates, and result writeback.
#include "solver_dispatch_31c940.h"
#include <math.h>
#include <string.h>
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
// One float word, with assignment and borrowing through its stored value.
struct stored_word_31c940 {
 float value;
 __forceinline stored_word_31c940 &operator=(float input) { value = input; return *this; }
 __forceinline operator float &() { return value; }
 __forceinline operator float const &() const { return value; }
};
typedef char stored_word_size_31c940[sizeof(stored_word_31c940)==4 ? 1 : -1];

// These five words serve different scalar and pointer roles in separate operations.
// The rounded-input word also serves as a linear cursor in other operations.
// Keep its native storage separate; this view borrows the currently active roles.
struct solver_work_prefix_31c940 {
 union { stored_word_31c940 pair_b; __m128 const *inverse_b; float const *writeback_time; };
 union { stored_word_31c940 first_error; __m128 *angular_b; };
 union { stored_word_31c940 fallback_scalar; float const *friction_scale; solver_linear_row_31e7b0 const *next_circular; };
};
union solver_simd_word_31c940 {float value;__m128 *linear_cursor;};
struct solver_work_tail_31c940 {
 union {stored_word_31c940 pair_a;__m128 *linear_b;float *linear_floats_b;};
};
struct solver_scalar_work_31c940 {
 stored_word_31c940 &pair_b;__m128 const *&inverse_b;float const *&writeback_time;
 stored_word_31c940 &first_error;__m128 *&angular_b;
 stored_word_31c940 &fallback_scalar;float const *&friction_scale;solver_linear_row_31e7b0 const *&next_circular;
 float &simd_scalar;__m128 *&linear_cursor;
 stored_word_31c940 &pair_a;__m128 *&linear_b;float *&linear_floats_b;
 __forceinline solver_scalar_work_31c940(solver_work_prefix_31c940 &prefix,solver_simd_word_31c940 &simd_word,solver_work_tail_31c940 &tail):
 pair_b(prefix.pair_b),inverse_b(prefix.inverse_b),writeback_time(prefix.writeback_time),first_error(prefix.first_error),angular_b(prefix.angular_b),
 fallback_scalar(prefix.fallback_scalar),friction_scale(prefix.friction_scale),next_circular(prefix.next_circular),
 simd_scalar(simd_word.value),linear_cursor(simd_word.linear_cursor),pair_a(tail.pair_a),linear_b(tail.linear_b),linear_floats_b(tail.linear_floats_b){}
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
	__m128 w[5];
	w[0]=b.body->linear;
	w[1]=a->linear;
	w[2]=row->a;
	w[3]=b.body->angular;
	w[4]=linear_axis;
	w[1]=_mm_sub_ps(w[1],w[0]);
	w[0]=a->angular;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=row->b;
	w[3]=_mm_mul_ps(w[3],w[2]);
	w[1]=_mm_mul_ps(w[1],w[4]);
	w[0]=_mm_add_ps(w[0],w[3]);
	w[0]=_mm_add_ps(w[0],w[1]);
	sum_three_to_arena_31c940(w[0],projection_output);
	return w[4];
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

static __forceinline __m128 project_first_spin_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b)
{
	__m128 w[5];
	w[0]=b->linear;
	w[1]=a->linear;
	w[3]=b->angular;
	w[2]=row->a;
	w[1]=_mm_sub_ps(w[1],w[0]);
	w[0]=a->angular;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=row->linear;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=row->b;
	w[3]=_mm_mul_ps(w[3],w[2]);
	w[0]=_mm_add_ps(w[0],w[3]);
	w[0]=_mm_add_ps(w[0],w[1]);
return w[0];
}
static __forceinline __m128 circular_first_projection_value(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b)
{
	__m128 w[5];
	w[0]=b->linear;
	w[1]=a->linear;
	w[3]=b->angular;
	w[2]=row->a;
	w[1]=_mm_sub_ps(w[1],w[0]);
	w[0]=a->angular;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=row->linear;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=row->b;
	w[3]=_mm_mul_ps(w[3],w[2]);
	w[0]=_mm_add_ps(w[0],w[3]);
	w[0]=_mm_add_ps(w[0],w[1]);
	w[1]=_mm_shuffle_ps(w[0],w[0],0x55);
	w[1]=_mm_add_ss(w[1],w[0]);
	w[2]=_mm_shuffle_ps(w[0],w[0],0xaa);
	w[2]=_mm_add_ss(w[2],w[1]);
	return w[2];
}
static __forceinline void circular_second_projection_fields(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b,__m128 *const &angular_b, float &projection_output)
{
	__m128 w[5];
	w[0]=linear_b;
	w[1]=a->linear;
	w[2]=row->a;
	w[3]=row->b;
	w[1]=_mm_sub_ps(w[1],w[0]);
	w[0]=a->angular;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=*angular_b;
	w[2]=_mm_mul_ps(w[2],w[3]);
	w[3]=row->linear;
	w[1]=_mm_mul_ps(w[1],w[3]);
	w[0]=_mm_add_ps(w[0],w[2]);
	w[0]=_mm_add_ps(w[0],w[1]);
	w[1]=_mm_shuffle_ps(w[0],w[0],0x55);
	w[1]=_mm_add_ss(w[1],w[0]);
	w[2]=_mm_shuffle_ps(w[0],w[0],0xaa);
	w[2]=_mm_add_ss(w[2],w[1]);
	_mm_store_ss(&projection_output,w[2]);
}






static __forceinline void apply_linear(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float *result, float &simd_scratch,__m128 (&w)[5])
{
	// The caller retains the projected linear axis in w[4].

	simd_scratch=impulse;
	w[2]=a->inverse_inertia;

	w[1]=_mm_load_ss(&simd_scratch);
	w[0]=_mm_shuffle_ps(w[1],w[1],0);
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=b.body->inverse_inertia;

	w[1]=_mm_shuffle_ps(w[1],w[1],0);
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
	w[2]=_mm_mul_ps(w[2],w[4]);
	w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
	w[3]=_mm_mul_ps(w[3],w[4]);
	// Retain B through all four updates before A can overwrite its saved home.
	solver_velocity_31c940 *const update_b=b.body;
	w[4]=a->linear;
	w[4]=_mm_add_ps(w[4],w[2]);
	a->linear=w[4];
	w[2]=update_b->linear;
	w[2]=_mm_sub_ps(w[2],w[3]);
	update_b->linear=w[2];
	// Angular axes reload after both linear writes.
	w[2]=row->a;
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=row->b;
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=a->angular;
	w[2]=_mm_add_ps(w[2],w[0]);
	a->angular=w[2];
	w[0]=update_b->angular;
	w[0]=_mm_add_ps(w[0],w[1]);
	update_b->angular=w[0];
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
static __forceinline void apply_circular_second_fields(float impulse,
 solver_linear_row_31e7b0 const *const &row_home,__m128 *&row_cursor,__m128 *&terms,
 solver_velocity_31c940 *a,__m128 &linear_b,__m128 *const &angular_b,
 __m128 const *inverse_b,float *result,float &simd_scratch)
{
	__m128 w[5];
	w[2]=a->inverse_inertia;
	simd_scratch=impulse;
	w[3]=*inverse_b;
	terms+=6;
	w[1]=_mm_load_ss(&simd_scratch);
	w[0]=_mm_shuffle_ps(w[1],w[1],0);
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
	*result+=impulse;
}



static __forceinline void apply_linear_spilled(float const &source, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float *result, float &simd_copy)
{
	// These normal-pair impulses have already been spilled to float. Load that
	// value for accumulation and copy its bits into the SIMD scalar.
	float impulse = source;

	memcpy(&simd_copy, &source, sizeof(simd_copy));
	__m128 work_a, work_b, work_c, work_d, work_e;
	work_c=a->inverse_inertia;
	work_b=_mm_load_ss(&simd_copy);
	work_e=row->linear;
	work_a=_mm_shuffle_ps(work_b, work_b, 0);
	work_a=_mm_mul_ps(work_a, work_c);
	work_c=b.body->inverse_inertia;
	work_b=_mm_shuffle_ps(work_b, work_b, 0);
	work_b=_mm_mul_ps(work_b, work_c);
	work_c=_mm_shuffle_ps(work_a, work_a, 0xff);
	work_c=_mm_mul_ps(work_c, work_e);
	work_d=_mm_shuffle_ps(work_b, work_b, 0xff);
	work_d=_mm_mul_ps(work_d, work_e);
	// Retain the linear receiver before A can replace the saved B body.
	// Retain this receiver before A can replace the saved body pointer.
	solver_velocity_31c940 *const linear_body_b=b.body;
	work_e=a->linear;
	work_e=_mm_add_ps(work_e, work_c);
	a->linear=work_e;
	work_c=linear_body_b->linear;
	work_c=_mm_sub_ps(work_c, work_d);
	linear_body_b->linear=work_c;
	// The angular receiver is captured after the linear writes, before A angular.
	solver_velocity_31c940 *const angular_body_b=b.body;
	work_c=row->a;

	work_a=_mm_mul_ps(work_a, work_c);
	work_c=row->b;
	work_b=_mm_mul_ps(work_b, work_c);
	work_c=a->angular;
	work_c=_mm_add_ps(work_c, work_a);
	a->angular=work_c;
	work_a=angular_body_b->angular;
	work_a=_mm_add_ps(work_a, work_b);
	angular_body_b->angular=work_a;
	*result+=impulse;
}

// The first positive row retains its unshifted linear cursor while angular
// axes use the coupled-pair cursor. Keep this update separate from the second
// positive row so both existing cursor representations survive optimization.
// Capture B's angular receiver after the linear writes, before either angular write.


static __forceinline void apply_first_normal_positive_impulse(float const &source, __m128 const &linear_axis,__m128 const *angular_axes,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float *result, float &simd_copy)
{

	// Capture inertia before the SIMD copy can replace one of its lanes.
	// Use the stored float for accumulation and copy its bits for SIMD.
	__m128 w[5];
	w[2]=a->inverse_inertia;
	float impulse=source;
	memcpy(&simd_copy,&source,sizeof(simd_copy));
	// Reuse the consumed projection values through both body updates.

	w[1]=_mm_load_ss(&simd_copy);
	w[0]=_mm_shuffle_ps(w[1],w[1],0);
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=b.body->inverse_inertia;
	w[1]=_mm_load_ss(&simd_copy);
	w[1]=_mm_shuffle_ps(w[1],w[1],0);
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[4]=linear_axis;
	w[2]=_mm_shuffle_ps(w[0],w[0],0xff);
	w[2]=_mm_mul_ps(w[2],w[4]);
	w[3]=_mm_shuffle_ps(w[1],w[1],0xff);
	w[3]=_mm_mul_ps(w[3],w[4]);
	// Retain this receiver before A can replace the saved body pointer.
	solver_velocity_31c940 *const linear_body_b=b.body;
	w[4]=a->linear;
	w[4]=_mm_add_ps(w[4],w[2]);
	a->linear=w[4];
	w[2]=linear_body_b->linear;
	w[2]=_mm_sub_ps(w[2],w[3]);
	linear_body_b->linear=w[2];
	// Capture B after both linear writes, before either angular write.
	solver_velocity_31c940 *angular_body_b=b.body;
	w[2]=angular_axes[0];
	w[0]=_mm_mul_ps(w[0],w[2]);
	w[2]=angular_axes[1];
	w[1]=_mm_mul_ps(w[1],w[2]);
	w[2]=a->angular;
	w[2]=_mm_add_ps(w[2],w[0]);
	a->angular=w[2];
	w[0]=angular_body_b->angular;
	w[0]=_mm_add_ps(w[0],w[1]);
	angular_body_b->angular=w[0];
	*result+=impulse;
}

// The first pair reloads B for inertia, then captures each update receiver.


static __forceinline void apply_first_normal_fallback(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, float *result, float &simd_scratch)
{
	// Keep accumulation wider than the float copy consumed by SIMD.
	__m128 inertia_a=a->inverse_inertia;
	float &rounded_impulse = simd_scratch;
	float &store = rounded_impulse;
	store = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
	__m128 inverse_b = b.body->inverse_inertia;
	solver_scoped_body_fields_31c940 const &linear_body_b = b;
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_body_b.body->linear = _mm_sub_ps(linear_body_b.body->linear, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	solver_scoped_body_fields_31c940 const &angular_body_b = b;
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	angular_body_b.body->angular = _mm_add_ps(angular_body_b.body->angular, angular_b);
	*result += impulse;
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
static __forceinline void reduce_into_spin_output(__m128 const &v,spin_projection_output_31c940 out) {
 __m128 xy=_mm_add_ss(_mm_shuffle_ps(v,v,0x55),v);
 __m128 xyz=_mm_add_ss(_mm_shuffle_ps(v,v,0xaa),xy);
 out.store(xyz);
}
static __forceinline void project_second_spin_fields(solver_linear_row_31e7b0 const *row,
 solver_velocity_31c940 *a,__m128 const &linear_b,__m128 const &angular_b,float &output)
{
 __m128 w[5];
 w[0]=linear_b;w[1]=a->linear;w[2]=row->a;w[3]=row->b;
 w[1]=_mm_sub_ps(w[1],w[0]);w[0]=a->angular;
 w[0]=_mm_mul_ps(w[0],w[2]);w[2]=angular_b;w[2]=_mm_mul_ps(w[2],w[3]);
 w[3]=row->linear;w[1]=_mm_mul_ps(w[1],w[3]);
 w[0]=_mm_add_ps(w[0],w[2]);w[0]=_mm_add_ps(w[0],w[1]);
 w[1]=_mm_shuffle_ps(w[0],w[0],0x55);w[1]=_mm_add_ss(w[1],w[0]);
 w[2]=_mm_shuffle_ps(w[0],w[0],0xaa);w[2]=_mm_add_ss(w[2],w[1]);
 _mm_store_ss(&output,w[2]);
}





// Keep the angular projection's A load distinct from the tangential rows.
static __forceinline void project_spin_angular_velocity(solver_angular_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &angular_b, float &projection_output)
{
	sum_three_to_arena_31c940(_mm_add_ps(_mm_mul_ps(_mm_load_ps((float const *)&(a->angular)), row->a), _mm_mul_ps(angular_b, row->b)), projection_output);
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
	float accumulated=*result+impulse;
	// Keep the SIMD bits even when the result store replaces the spin source.
	unsigned long spin_bits;
	memcpy(&spin_bits,&spin_source,sizeof(spin_bits));
	*result=accumulated;
	return spin_bits;
}

// Retain the first row's unshifted cursor alongside the second row's B axis.
// Their separate uses keep retail's cursor bias over the same row storage.


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
	float const &mass_b_source = axis[0].m128_f32[3];
	float mass_b = mass_b_source;
	scalar_work.pair_b = mass_b * error_b + scalar_work.first_error * operation_float(op, 4);
	// A rejected second-row candidate joins the same first-row fallback as a failed
	// positive pair. Keep the positive pair before the second-row fallback.
	do
	{
		if (scalar_work.pair_a > 0.0f)
		{
			if (scalar_work.pair_b > 0.0f)
			{
				apply_first_normal_positive_impulse(scalar_work.pair_a, retained_axis,axis-4, a, b, result, projection_pool.normal_first_a_copy);
				apply_linear_spilled(scalar_work.pair_b, ((solver_linear_row_31e7b0 *)(axis - 2)), a, b, result + 1, projection_pool.normal_first_b_copy);
				return;
			}
			break;
		}
		float candidate_b = error_b * axis[-1].m128_f32[3];
		// Unordered candidates enter the second-row update, as in retail.
		if (candidate_b <= 0.0f) break;
		apply_first_normal_fallback(candidate_b, ((solver_linear_row_31e7b0 *)(axis - 2)), a, b, result + 1, scalar_work.fallback_scalar);
		return;
	} while (false);
	float candidate_a = scalar_work.first_error * axis[-4].m128_f32[3];
	if (candidate_a > 0.0f) apply_first_normal_fallback(candidate_a, ((solver_linear_row_31e7b0 *)first_axis), a, b, result, scalar_work.fallback_scalar);
}

// The second unrolled pair keeps B field addresses and its first linear axis.
// Its two updates snapshot the angular-B address at different points.
static __forceinline void apply_second_pair_fallback(float impulse,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a,__m128 *const & linear_velocity_b_pointer,
	__m128 *const &angular_velocity_b_pointer, __m128 const &inverse_b, __m128 const &axis_linear, float *result, float &simd_scratch)
{
	// Keep accumulation wider than the float copy consumed by SIMD.
	__m128 inertia_a=a->inverse_inertia;
	float &rounded_impulse = simd_scratch;
	float &store = rounded_impulse;
	store = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	(*linear_velocity_b_pointer) = _mm_sub_ps((*linear_velocity_b_pointer), linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	(*angular_velocity_b_pointer) = _mm_add_ps((*angular_velocity_b_pointer), angular_b);
	*result += impulse;
}

// Read the stored float for x87 consumers; SIMD transfers can copy its bits.
static __forceinline float load_stored_impulse(float const &value)
{
	float const &stored=value;
	return stored;
}

static __forceinline void apply_second_pair_first_impulse(float const &source,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a,__m128 *const & linear_velocity_b_pointer,
	__m128 *const &angular_b_pointer, solver_velocity_31c940 *const &inertia_body_b, __m128 const *&inverse_b_home, __m128 const &axis_linear,
	float *result, float &simd_copy)
{
	// These normal-pair impulses have already been spilled to float. Load that
	// value for accumulation and copy its bits into the SIMD scalar.
	float impulse = load_stored_impulse(source);

	memcpy(&simd_copy, &source, sizeof(simd_copy));
	__m128 scalar = _mm_load_ss(&simd_copy);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 const *inverse_b_pointer = &inertia_body_b->inverse_inertia;
 inverse_b_home=inverse_b_pointer;
 __m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), (*inverse_b_home));
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	(*linear_velocity_b_pointer) = _mm_sub_ps((*linear_velocity_b_pointer), linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);

	__m128 *angular_b_address = angular_b_pointer;
	*angular_b_address = _mm_add_ps(*angular_b_address, angular_b);
	*result += impulse;
}

static __forceinline void apply_second_pair_second_impulse(float const &source,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a,__m128 *const & linear_velocity_b_pointer,
	__m128 *const &angular_b_pointer, __m128 const &inverse_b, float *result, float &simd_copy)
{
	// These normal-pair impulses have already been spilled to float. Load that
	// value for accumulation and copy its bits into the SIMD scalar.
	float impulse = load_stored_impulse(source);

	memcpy(&simd_copy, &source, sizeof(simd_copy));
	__m128 scalar = _mm_load_ss(&simd_copy);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	(*linear_velocity_b_pointer) = _mm_sub_ps((*linear_velocity_b_pointer), linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	__m128 *angular_b_address = angular_b_pointer;
	a->angular = _mm_add_ps(a->angular, angular_a);
	*angular_b_address = _mm_add_ps(*angular_b_address, angular_b);
	*result += impulse;
}


static __forceinline void project_linear_axis(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b, __m128 const &axis_linear, float &projection_output)
{
	__m128 relative = _mm_sub_ps(_mm_load_ps((float const *)&a->linear), b.body->linear);
	// Retain the linear and angular contributions before reduction.
	__m128 projection_contribution[3] =
	{
		_mm_mul_ps(relative, axis_linear),
		_mm_mul_ps(a->angular, row->a),
		_mm_mul_ps(_mm_load_ps((float const *)&b.body->angular), row->b)
	};
	__m128 angular = _mm_add_ps(projection_contribution[1], projection_contribution[2]);
	sum_three_to_arena_31c940(_mm_add_ps(angular, projection_contribution[0]), projection_output);
}

static __forceinline void project_second_pair_second_row(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b, __m128 const &angular_b, __m128 const &axis_linear, float &projection_output)
{
	__m128 relative = _mm_sub_ps(a->linear, linear_b);
	__m128 projection_contribution[2] =
	{
		_mm_mul_ps(a->angular, row->a),
		_mm_mul_ps(angular_b, row->b)
	};
	__m128 angular = _mm_add_ps(projection_contribution[0], projection_contribution[1]);
	sum_three_to_arena_31c940(_mm_add_ps(angular, _mm_mul_ps(relative, axis_linear)), projection_output);
}


static __forceinline void solve_second_contact_pair(solver_step_31c940 *const &step,
	unsigned char *op, __m128 *terms, __m128 *normal_axes, solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b,
	float *result, solver_scalar_work_31c940 &scalar_work, solver_scalar_arena_31c940 &projection_pool)
{

	solver_scoped_body_fields_31c940 const &body_b = b;
	scalar_work.linear_cursor=&body_b.body->linear;
	__m128 *const &linear_b_pointer=scalar_work.linear_cursor;
	__m128 *&angular_b_pointer = scalar_work.angular_b;
	angular_b_pointer = &body_b.body->angular;
	__m128 first_linear_axis = terms[0];
	float target_a = normal_axes[-5].m128_f32[3];
	// Complete both SIMD projections before the coupled x87 errors.
	project_linear_axis(((solver_linear_row_31e7b0 *)(normal_axes - 5)),
		a, body_b, first_linear_axis, projection_pool.normal_second_projection_a);
	project_second_pair_second_row(((solver_linear_row_31e7b0 *)(normal_axes - 2)),
		a, *linear_b_pointer, *angular_b_pointer, normal_axes[-2], projection_pool.normal_second_projection_b);
	solver_step_31c940 *parameters = step;
	// Retail spills the first error and both coupled impulses to 32-bit storage.
	scalar_work.pair_b = target_a * parameters->error_scale
	- projection_pool.normal_second_projection_a * parameters->velocity_scale;
	float error_b = normal_axes[-2].m128_f32[3] * parameters->error_scale
	- projection_pool.normal_second_projection_b * parameters->velocity_scale;
	float const &mass_a_source = normal_axes[-3].m128_f32[3];
	float mass_a = mass_a_source;
	scalar_work.pair_a = first_coupled_impulse_31c940(mass_a, (float const &)scalar_work.pair_b,
		error_b, operation_float(op, 4));
	// Capture the second mass before its coupled product.
	float const &mass_b_source = normal_axes[0].m128_f32[3];
	float mass_b = mass_b_source;
	scalar_work.fallback_scalar = second_coupled_impulse_31c940(mass_b, error_b,
		(float const &)scalar_work.pair_b, operation_float(op, 4));
	// Keep the first-row fallback in both source paths. The compiler merges
	// that tail while retaining retail's unordered second-row branch direction.
	// Nonpositive and unordered first impulses take the single-row path.
	if (load_stored_impulse(scalar_work.pair_a) > 0.0f)
	{
		if (load_stored_impulse(scalar_work.fallback_scalar) > 0.0f)
		{

			apply_second_pair_first_impulse(scalar_work.pair_a,
				((solver_linear_row_31e7b0 *)(normal_axes - 5)), a, linear_b_pointer, angular_b_pointer, b.body, scalar_work.inverse_b,
				first_linear_axis, result, projection_pool.normal_second_a_copy);
			apply_second_pair_second_impulse(scalar_work.fallback_scalar,
				((solver_linear_row_31e7b0 *)(normal_axes - 2)), a, linear_b_pointer, angular_b_pointer,
				*scalar_work.inverse_b, result + 1, projection_pool.normal_second_b_copy);

		}
		else
		{
			float candidate_a = scalar_work.pair_b * normal_axes[-4].m128_f32[3];
			if (candidate_a > 0.0f) apply_second_pair_fallback(candidate_a,
				((solver_linear_row_31e7b0 *)(normal_axes - 5)), a, linear_b_pointer, angular_b_pointer, b.body->inverse_inertia,
				first_linear_axis, result, scalar_work.fallback_scalar);

		}

	}
	else
	{
		float candidate_b = error_b * normal_axes[-1].m128_f32[3];
		if (candidate_b <= 0.0f)
		{
			float candidate_a = scalar_work.pair_b * normal_axes[-4].m128_f32[3];
			if (candidate_a > 0.0f) apply_second_pair_fallback(candidate_a,
				((solver_linear_row_31e7b0 *)(normal_axes - 5)), a, linear_b_pointer, angular_b_pointer, b.body->inverse_inertia,
				first_linear_axis, result, scalar_work.fallback_scalar);

		}
		else
		{
			apply_second_pair_fallback(candidate_b,
				((solver_linear_row_31e7b0 *)(normal_axes - 2)), a, linear_b_pointer, angular_b_pointer,
				b.body->inverse_inertia, normal_axes[-2], result + 1, scalar_work.fallback_scalar);

		}

	}
}


// Retain the coefficient and B inertia addresses across consecutive rows.
// Native scalar locals keep the projection between SSE and x87 without a copy.
static __forceinline void solve_linear_friction(float target, float const &limit,
	float const *const &friction_scale_pointer,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a,__m128 *const & angular_b_pointer, __m128 &linear_b, __m128 const *const &inverse_b_pointer,
	solver_step_31c940 *const &step, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	// Keep the angular products and reuse their work vectors for projection.
	__m128 work_a = _mm_load_ps((float const *)&(a->angular));
	__m128 work_b = _mm_mul_ps((*angular_b_pointer), _mm_load_ps((float const *)&(row->b)));
	work_a = _mm_mul_ps(work_a, row->a);
	work_a = _mm_add_ps(work_a, work_b);
	work_b = _mm_mul_ps(_mm_sub_ps(a->linear,
		_mm_load_ps((float const *)&(linear_b))), row->linear);
	work_a = _mm_add_ps(work_a, work_b);
	__m128 sum = _mm_add_ss(_mm_shuffle_ps(work_a, work_a, 0xaa),
		_mm_add_ss(_mm_shuffle_ps(work_a, work_a, 0x55), work_a));

	_mm_store_ss(&projection_pool.linear_friction, sum);
	float impulse = (target * (*friction_scale_pointer)
		- projection_pool.linear_friction * load_velocity_scale(step)) * row->a.m128_f32[3];
	float magnitude = (float)fabs(impulse);
	if (magnitude > limit)
	{
		float factor = limit / magnitude;
		impulse *= factor; row->linear.m128_f32[3] *= factor;
	}
	// Round the SIMD input once, retaining the extended impulse for accumulation.
	__m128 inertia_a=a->inverse_inertia;
	float &rounded = simd_scratch;
	rounded = impulse;
	__m128 scalar = _mm_load_ss(&rounded);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
	__m128 scalar_b = _mm_load_ss(&rounded);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar_b, scalar_b, 0), *inverse_b_pointer);
	// Retain B angular before either linear or angular write can redirect its home.
	__m128 *angular_receiver=angular_b_pointer;
	// Capture both linear contributions before either body write.
	__m128 linear_contribution[2] =
	{
		_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear),
		_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear)
	};
	a->linear = _mm_add_ps(a->linear, linear_contribution[0]);
	linear_b = _mm_sub_ps(linear_b, linear_contribution[1]);
	// Capture both angular contributions before either body write.
	__m128 angular_contribution[2] =
	{
		_mm_mul_ps(change_a, row->a),
		_mm_mul_ps(change_b, row->b)
	};
	a->angular = _mm_add_ps(a->angular, angular_contribution[0]);
	*angular_receiver = _mm_add_ps(*angular_receiver, angular_contribution[1]);
	*result += impulse;
}

// Select the active angular limit before the shared body-update tail. The
// lower error is stored to float before multiplication; the upper error keeps
// the extended x87 value. The scalar result is rounded only for SIMD use.
static __forceinline float select_angular_limit(double velocity, float const &target,
	float const &lower, float const &upper, float const &gain, float const &mass,
	bool &active, __m128 &scalar, float &simd_scratch)
{
	double lower_error = (target - lower) * gain - velocity;
 simd_scratch = lower_error;
	if (lower_error > 0.0f)
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
		float impulse = error * mass;
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
 __m128 *&terms,solver_velocity_31c940 *a,__m128 *linear_a,float *const &linear_b,
 __m128 *const &angular_b,__m128 const *const &inverse_b,
 solver_step_31c940 *const &step,float *&result,unsigned char *&op,
 solver_scalar_arena_31c940 &projection_pool,float &simd_scratch)
{
	__m128 work[5];
	work[1]=*linear_a;
	work[2]=terms[1];
	work[3]=terms[2];
	work[0]=_mm_load_ps(linear_b);
	work[4]=*angular_b;
	work[1]=_mm_sub_ps(work[1],work[0]);
	work[0]=a->angular;
	work[0]=_mm_mul_ps(work[0],work[2]);
	work[2]=terms[0];
	work[1]=_mm_mul_ps(work[1],work[2]);
	work[4]=_mm_mul_ps(work[4],work[3]);
	work[0]=_mm_add_ps(work[0],work[4]);
	work[0]=_mm_add_ps(work[0],work[1]);
	work[1]=_mm_shuffle_ps(work[0],work[0],0x55);
	work[1]=_mm_add_ss(work[1],work[0]);
	work[3]=_mm_shuffle_ps(work[0],work[0],0xaa);
	work[3]=_mm_add_ss(work[3],work[1]);
	_mm_store_ss(&projection_pool.linear_error,work[3]);
	// Advance cursors while retaining this row's projected linear axis.
	op+=4; terms+=3; ++result;
	solver_step_31c940 *parameters=step;
	float impulse=mass*(target*parameters->error_scale-projection_pool.linear_error*parameters->velocity_scale);

	// A inertia precedes the rounded input; B inertia is read afterward.
	work[3]=a->inverse_inertia;
	simd_scratch=impulse;
	work[1]=_mm_load_ss(&simd_scratch);

	work[0]=_mm_shuffle_ps(work[1],work[1],0);
	work[0]=_mm_mul_ps(work[0],work[3]);
	work[3]=*inverse_b;
	work[1]=_mm_shuffle_ps(work[1],work[1],0);
	work[1]=_mm_mul_ps(work[1],work[3]);
	// Capture B linear before either linear write.
	float *linear_body_b=linear_b;
	work[3]=_mm_shuffle_ps(work[0],work[0],0xff);
	work[3]=_mm_mul_ps(work[3],work[2]);
	work[4]=_mm_shuffle_ps(work[1],work[1],0xff);
	work[4]=_mm_mul_ps(work[4],work[2]);
	work[2]=*linear_a;
	work[2]=_mm_add_ps(work[2],work[3]);
	*linear_a=work[2];
	work[2]=_mm_load_ps(linear_body_b);
	work[2]=_mm_sub_ps(work[2],work[4]);
	_mm_store_ps(linear_body_b,work[2]);
	work[2]=terms[-2];
	work[0]=_mm_mul_ps(work[0],work[2]);
	work[2]=terms[-1];
	work[1]=_mm_mul_ps(work[1],work[2]);
	__m128 *angular_a=&a->angular;
	work[2]=*angular_a;
	work[2]=_mm_add_ps(work[2],work[0]);
	*angular_a=work[2];
	// This row reloads the angular receiver after A's angular write.
	__m128 *angular_body_b=angular_b;
	work[0]=*angular_body_b;
	work[0]=_mm_add_ps(work[0],work[1]);
	*angular_body_b=work[0];
	result[-1]+=impulse;
}


// Capture target and limit before projection, retaining the coefficient address
// across consecutive rows. Keep B's inertia pointer through the clamp and read
// its value when forming the update. Only the SIMD copy rounds to float.
static __forceinline void solve_angular_friction_kernel(float target, float const &limit,
	float const *const &friction_scale_pointer,
	solver_angular_row_31e7b0 *row, solver_step_31c940 *const &step,
	solver_velocity_31c940 *a, __m128 &velocity_b, __m128 const *const &inverse_b_pointer, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(velocity_b, row->b));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);

	_mm_store_ss(&projection_pool.angular_friction, xyz);

	float impulse = (target * (*friction_scale_pointer)
		- projection_pool.angular_friction * load_velocity_scale(step)) * row->a.m128_f32[3];
	float magnitude = (float)fabs(impulse);
	if (magnitude > limit)
	{
		float factor = limit / magnitude;
		impulse *= factor; row->b.m128_f32[3] *= factor;
	}
	// Round the SIMD input once, then construct each body's broadcast separately.
	__m128 inertia_a=a->inverse_inertia;
	float &rounded_impulse = simd_scratch;
	rounded_impulse = impulse;
	__m128 input_a = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(input_a, input_a, 0), inertia_a);
	__m128 input_b = _mm_load_ss(&rounded_impulse);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(input_b, input_b, 0), (*inverse_b_pointer));
	__m128 axis_a = row->a, axis_b = row->b;
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, axis_a));
	velocity_b = _mm_add_ps(velocity_b, _mm_mul_ps(change_b, axis_b));
	*result += impulse;
}

// Keep B's field addresses across consecutive bounded angular rows. Capture
// both axes before either angular write; only the SIMD impulse rounds here.
static __forceinline void solve_bounded_angular(float mass, float target,
	solver_angular_row_31e7b0 *row, solver_velocity_31c940 *a,
	float *velocity_b, __m128 const *const &inverse_b_pointer,
	float const &upper, float const &lower, solver_step_31c940 *const &step, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{
	__m128 axis_a = row->a, axis_b = row->b;
	__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, axis_a), _mm_mul_ps(_mm_load_ps(velocity_b), axis_b));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);

	_mm_store_ss(&projection_pool.bounded_angular, xyz);
	// Keep the mass multiplication before the step scale, without an early
	// float spill. Both clamps retain their divide-then-multiply sequence.
	double mass_error = (target - projection_pool.bounded_angular) * mass;
	float impulse = mass_error * load_error_scale(step);
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
	// Capture A inertia before the SIMD input can overwrite its lanes.
	__m128 inertia_a=a->inverse_inertia;
	// Only SIMD consumes this rounded copy; accumulation retains the x87 impulse.
	float &rounded_impulse = simd_scratch;
	rounded_impulse = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	// Retain both body weights before either angular velocity write.
	__m128 weights[2];
	weights[0] = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
	weights[1] = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), *inverse_b_pointer);
	// Reload A after projection for the bounded angular update.
	a->angular = _mm_add_ps(_mm_load_ps((float const *)&a->angular), _mm_mul_ps(weights[0], axis_a));
	_mm_store_ps(velocity_b, _mm_add_ps(_mm_load_ps(velocity_b),
		_mm_mul_ps(weights[1], axis_b)));
	*result += impulse;
}

// Preserve the captured linear axis through projection and update. The angular
// axes reload after the linear stores, as required when rows overlap bodies.


static __forceinline void solve_unilateral_linear(float target,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a, solver_scoped_body_fields_31c940 const &b,
	__m128 &linear_b,__m128 *const & angular_b_pointer, solver_step_31c940 *const &step, float *result, solver_scalar_arena_31c940 &projection_pool, float &simd_scratch)
{// Reuse the five real projection values; retain the linear axis for the update.
	__m128 work[5];
	work[0]=linear_b;
	work[1]=a->linear;
	work[2]=row->a;
	work[3]=*angular_b_pointer;
	work[4]=row->linear;
	work[1]=_mm_sub_ps(work[1],work[0]);
	work[0]=a->angular;
	work[0]=_mm_mul_ps(work[0],work[2]);
	work[2]=row->b;
	work[3]=_mm_mul_ps(work[3],work[2]);
	work[1]=_mm_mul_ps(work[1],work[4]);
	work[0]=_mm_add_ps(work[0],work[3]);
	work[0]=_mm_add_ps(work[0],work[1]);
	sum_three_to_arena_31c940(work[0],projection_pool.unilateral_linear);
	__m128 axis_linear=work[4];
	solver_step_31c940 *parameters = step;
	float error = target * parameters->error_scale - projection_pool.unilateral_linear * parameters->velocity_scale;
	if (error > 0.0f)
	{
		float impulse = error * row->a.m128_f32[3];
		__m128 inertia_a=a->inverse_inertia;
		simd_scratch = impulse;
		__m128 scalar = _mm_load_ss(&simd_scratch);
		__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
		__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b.body->inverse_inertia);
		// Capture B before either body write can replace its saved address.
		__m128 *const angular_receiver = angular_b_pointer;

		// Form both linear contributions before reloading and writing either body.
		__m128 linear_delta[2] =
		{
			_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear),
			_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear)
		};
		// Reload A for the update instead of retaining its projection load.
		a->linear = _mm_add_ps(_mm_load_ps((float const *)&a->linear), linear_delta[0]);
		linear_b = _mm_sub_ps(linear_b, linear_delta[1]);
		// Both linear deltas are consumed before the angular axes reload.
		// Reuse their first vector through the angular contributions and A write.
		linear_delta[0]=row->a;
		change_a=_mm_mul_ps(change_a,linear_delta[0]);
		linear_delta[0]=row->b;
		change_b=_mm_mul_ps(change_b,linear_delta[0]);
		linear_delta[0]=a->angular;
		linear_delta[0]=_mm_add_ps(linear_delta[0],change_a);
		a->angular=linear_delta[0];
		change_a=*angular_receiver;
		change_a=_mm_add_ps(change_a,change_b);
		*angular_receiver=change_a;
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

// Both reduced vectors are consumed by the subsequent residual writes.
struct solver_residual_pair_value_31c940
{
	__m128 first;
	__m128 second;
	__forceinline solver_residual_pair_value_31c940(__m128 *cursor,
		solver_velocity_31c940 *a, solver_velocity_31c940 *b)
	{


		solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)cursor;
		// Retain both rows' inputs, reusing each vector after its last use.
		__m128 work_a = row[0].a;
		__m128 work_b = b->angular;
		__m128 work_c = a->angular;
		__m128 work_d = _mm_load_ps((float const *)&(row[1].linear));
		__m128 work_e = _mm_load_ps((float const *)&(row[0].b));
		__m128 work_f = row[1].a;
		work_a = _mm_mul_ps(work_c, work_a);
		__m128 work_g = _mm_load_ps((float const *)&(a->linear));
		work_g = _mm_sub_ps(work_g, _mm_load_ps((float const *)&(b->linear)));
		work_c = _mm_add_ps(_mm_mul_ps(work_c, work_f),
		_mm_mul_ps(work_b, _mm_load_ps((float const *)&(row[1].b))));
		work_d = _mm_add_ps(work_c, _mm_mul_ps(work_g, work_d));
		work_c = _mm_add_ss(_mm_shuffle_ps(work_d, work_d, 0x55), work_d);
		work_e = _mm_add_ps(work_a, _mm_mul_ps(work_b, work_e));
		work_e = _mm_add_ps(work_e,
		_mm_mul_ps(work_g, _mm_load_ps((float const *)&(row[0].linear))));
		work_b = _mm_add_ss(_mm_shuffle_ps(work_d, work_d, 0xaa), work_c);
		second=work_b;
		work_a = _mm_shuffle_ps(work_e, work_e, 0x55);
		work_d = _mm_add_ss(_mm_shuffle_ps(work_e, work_e, 0xaa),
		_mm_add_ss(work_a, work_e));
		// Keep the second reduced vector live until the first residual is written.


		first=work_d;
	}
};
typedef char solver_residual_pair_value_size_31c940[sizeof(solver_residual_pair_value_31c940) == 32 ? 1 : -1];
static __forceinline solver_residual_pair_value_31c940 project_residual_pair_31c940(__m128 *cursor,solver_velocity_31c940 *a,solver_velocity_31c940 *b)
{
	return solver_residual_pair_value_31c940(cursor,a,b);

}
static __forceinline void update_residual_pair(__m128 *&cursor,solver_velocity_31c940 *a,solver_velocity_31c940 *b, float &scalar_a, float &scalar_b)
{
	solver_residual_pair_value_31c940 projected = project_residual_pair_31c940(cursor, a, b);
	_mm_store_ss(&scalar_a, projected.first);
	cursor[0].m128_f32[3] -= scalar_a;
	cursor += 6;

	_mm_store_ss(&scalar_b, projected.second);
	cursor[-3].m128_f32[3] -= scalar_b;
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

typedef char solver_work_size_31c940[(sizeof(solver_work_prefix_31c940)+4+sizeof(solver_work_tail_31c940)) == 20 ? 1 : -1];
typedef char solver_phase_size_31c940[sizeof(solver_phase_state_31c940) == 28 ? 1 : -1];
typedef char solver_projection_pool_size_31c940[sizeof(solver_scalar_arena_31c940) == 128 ? 1 : -1];
typedef char solver_scalar_frame_size_31c940[(sizeof(solver_work_prefix_31c940)+4+sizeof(solver_complete_scalars_31c940)) == 176 ? 1 : -1];

// Ordinary reduced-check score 2077. Stack allocation is 0xB4, as in retail.
// Still substantially unmatched; no exact solver match is claimed.
// The pre-review score 1675 used volatile tuning that is not restored here.
// SSE scheduling, scalar homes, repeated broadcasts and control flow still differ.
// The second normal pair retains a biased row cursor; friction can dispatch
// directly into the next body header.
// Publish the header inputs, then advance the shared stream cursor.
static __forceinline void decode_solver_header_31c940(unsigned char *&op,
 __m128 *&terms, solver_velocity_31c940 *&a, solver_velocity_31c940 *&b,
 solver_velocity_31c940 *&a_home)
{
	solver_header_31c940 *header = (solver_header_31c940 *)op;
	// Capture the row input before publishing pointer homes; the header can overlap them.
	a = header->a;
	terms = header->terms;
	b = header->b;
	a_home = a;
	op += sizeof(*header);
}

// @retail 0x31c940
long __cdecl solve_operations_31c940(solver_step_31c940 *step, unsigned char *operations,
	solver_velocity_31c940 *bodies, float *impulses)
{
	solver_work_prefix_31c940 work_prefix;
solver_simd_word_31c940 simd_word;
solver_complete_scalars_31c940 frame;
solver_scalar_work_31c940 scalar_work(work_prefix,simd_word,frame.work);
	solver_phase_state_31c940 &state = frame.phase;
	solver_scalar_arena_31c940 &projection_pool = frame.projections;
	solver_pointer_phase_31c940 pointer_phase;

	if (!g_480c60)
	{
		function_31c7d0();
		if (!g_480c60) return 0;
	}
	for (state.solving.iteration = 0; state.solving.iteration < step->iterations; ++state.solving.iteration)
	{
		solver_step_31c940 *initial_parameters = step;
		state.solving.saved_error = initial_parameters->error_scale;
		state.solving.saved_velocity = initial_parameters->velocity_scale;
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
				switch (*(signed char *)op)
				{
				case 0: dispatch_done=true;header_complete=true;continue;
				// Retail groups up to two coupled normal pairs, then checks directly
				// for the three-axis friction record. Longer runs redispatch here.
				case 3:
				{
					// Read the following opcode after the row writes and before advancing.
					signed char next_pair_kind;
					{
						solve_contact_pair(step, op, normal_axes, terms, a, scoped_b, result, scalar_work, projection_pool);
						next_pair_kind = *((signed char *)op + 8);
						terms += 6; normal_axes += 6; result += 2; op += 8;

					}
					if (next_pair_kind != 3) continue;
					{
						solve_second_contact_pair(step, op, terms, normal_axes, a, scoped_b, result, scalar_work, projection_pool);
						next_pair_kind = *((signed char *)op + 8);
						terms += 6;  result += 2; op += 8;

					}

					if (next_pair_kind != 8) { normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128)); continue; }
					break;
				}
				// Continue below the switch for the shared spinning-friction path.
				case 8: break;

				// A single unilateral linear row.
				case 2:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					__m128 *linear_b_pointer = &body_b.body->linear;
					__m128 * &angular_b_pointer = scalar_work.angular_b;
					angular_b_pointer = &body_b.body->angular;
					do
					{
						solve_unilateral_linear(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), a, scoped_b,
							*linear_b_pointer, angular_b_pointer, step, result, projection_pool, scalar_work.fallback_scalar);
						terms += 3; ++result; op += 4;
					} while (*op == 2);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}

				case 1:
					header_complete = true;
					continue;
				// Two tangential rows share a circular friction limit.
				case 7:
				{
					float target_a = ((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
					__m128 *linear_b=&b->linear;
					__m128 first_projection=circular_first_projection_value(((solver_linear_row_31e7b0*)terms),a,b);
					// Publish this receiver after reduction, before the scalar output.
					scalar_work.angular_b=&b->angular;
					_mm_store_ss(&projection_pool.circular_first,first_projection);
					circular_second_projection_fields(((solver_linear_row_31e7b0*)terms)+1,a,*linear_b,scalar_work.angular_b,projection_pool.circular_second);
					// Both projections finish before these shared parameter reads.
					solver_step_31c940 *parameters = step;
					scalar_work.fallback_scalar = target_a * parameters->friction_scale - projection_pool.circular_first * parameters->velocity_scale;
					// Retain the row target before its scale product.
					float const &target_b = ((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3];
					scalar_work.pair_a = target_b * parameters->friction_scale
						- projection_pool.circular_second * parameters->velocity_scale;
					float maximum = operation_float(op, 12);
					// Round the circular limit to its float home before comparison and
					// division; the reads can then use that home directly.
					float &maximum_squared = scalar_work.pair_b;
					float &maximum_store = maximum_squared;
					maximum_store = maximum * maximum;
					// Capture each mass before its coupled product. These product locals
					// retain the extended x87 value until addition to the diagonal term.
					float const &mass_a_source = ((solver_linear_row_31e7b0 *)terms)[0].b.m128_f32[3];
					float mass_a = mass_a_source;
					float coupled_a = scalar_work.pair_a * operation_float(op, 8);
					float impulse_a = mass_a * scalar_work.fallback_scalar + coupled_a;
					float const &mass_b_source = ((solver_linear_row_31e7b0 *)terms)[1].b.m128_f32[3];
					float mass_b = mass_b_source;
					float coupled_b = scalar_work.fallback_scalar * operation_float(op, 8);
					float impulse_b = mass_b * scalar_work.pair_a + coupled_b;
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
apply_circular_second_fields(impulse_b,scalar_work.next_circular,normal_axes,terms,a,*linear_b,scalar_work.angular_b,inverse_b,result+1,scalar_work.simd_scalar);
result+=2;
					op += 20;
					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Angular friction with a scalar limit.
				case 9:
				{
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					float const * &friction_scale_pointer = scalar_work.friction_scale;
	 friction_scale_pointer = &step->friction_scale;
					__m128 *velocity_b_pointer = &body_b.body->angular;
					__m128 const * &inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					do
					{
						state.solving.angular_limit = operation_float(op, 4);
						solve_angular_friction_kernel(((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3], state.solving.angular_limit, friction_scale_pointer,
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
						__m128 axis_a = ((solver_angular_row_31e7b0 *)terms)->a, axis_b = ((solver_angular_row_31e7b0 *)terms)->b;
						__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, axis_a), _mm_mul_ps(velocity_b, axis_b));
						__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
						__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);

						_mm_store_ss(&projection_pool.angular_limit, xyz);
						bool active;
						__m128 scalar;
						float impulse = select_angular_limit(projection_pool.angular_limit * load_velocity_scale(step),
							((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3], operation_float(op, 4), operation_float(op, 8),
							operation_float(op, 12), ((solver_angular_row_31e7b0 *)terms)->a.m128_f32[3], active, scalar, scalar_work.simd_scalar);
						if (active)
						{
							__m128 inertia_a = a->inverse_inertia;
							__m128 inertia_second = scoped_b.body->inverse_inertia;
							__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
							a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, axis_a));
							__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_second);
							velocity_b = _mm_add_ps(velocity_b, _mm_mul_ps(change_b, axis_b));
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
						float mass = mass_source;
						float target = ((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3];
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
					__m128 * velocity_b_pointer = &body_b.body->angular;
					scalar_work.angular_b = velocity_b_pointer;
					__m128 const * &inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					__m128 *velocity_a_pointer = &a->angular;
					do
					{

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
						velocity_b_pointer=scalar_work.angular_b;
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
					do
					{
						solve_linear_error_cursor_31c940(((solver_linear_row_31e7b0*)terms)->linear.m128_f32[3],
						((solver_linear_row_31e7b0*)terms)->a.m128_f32[3],terms,a,linear_a_pointer,linear_b_pointer,
						angular_b_pointer,inverse_b_pointer,step,result,op,projection_pool,scalar_work.simd_scalar);
					} while (*op == 13);

					normal_axes = (__m128 *)((unsigned long)terms + 5 * sizeof(__m128));
					continue;
				}
				// Linear friction with a scalar limit.
				case 6:
				{
					float const * &friction_scale_pointer = scalar_work.friction_scale;
	 friction_scale_pointer = &step->friction_scale;
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					__m128 const * &inverse_b_pointer = scalar_work.inverse_b;
					inverse_b_pointer = &body_b.body->inverse_inertia;
					__m128 *linear_b_pointer = &body_b.body->linear;
					__m128 * &angular_b_pointer = scalar_work.angular_b;
					angular_b_pointer = &body_b.body->angular;
					do
					{
						state.solving.linear_limit = operation_float(op, 4);
						solve_linear_friction(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3], state.solving.linear_limit, friction_scale_pointer,
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
						// Retain the angular inputs, then reuse their work vectors for reduction.
						__m128 work_a = _mm_load_ps((float const *)&(a->angular));
						__m128 work_b = *angular_b_pointer;
						work_a = _mm_mul_ps(work_a, row->a);
						__m128 work_c = row->b;
						work_c = _mm_add_ps(work_a, _mm_mul_ps(work_b, work_c));
						// The lower update keeps this axis; the upper update reads the row again.
						__m128 first_linear_axis=row->linear;
						work_b = _mm_add_ps(work_c,
							_mm_mul_ps(_mm_sub_ps(_mm_load_ps((float const *)&(a->linear)),
								*(__m128 const *)linear_b_pointer), first_linear_axis));
						sum_three_to_arena_31c940(work_b, projection_pool.linear_bounds);
						// Capture the step pointer for velocity and the first bound. The
						// second bound reloads it after any first-bound result write.
						solver_step_31c940 *parameters = step;
						float velocity = projection_pool.linear_bounds * parameters->velocity_scale;
						float low = (((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3] - operation_float(op, 4)) * parameters->error_scale - velocity;
						if (low > 0.0f)
							update_linear_limit_values(low * ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), first_linear_axis, a, scoped_b.body->inverse_inertia, linear_b_pointer, angular_b_pointer, result, scalar_work.simd_scalar);
						float high = (((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3] - operation_float(op, 8)) * load_error_scale(step) - velocity;
						if (high < 0.0f)
							update_linear_limit_values(high * ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), ((solver_linear_row_31e7b0 *)terms)->linear, a, scoped_b.body->inverse_inertia, linear_b_pointer, angular_b_pointer, result, scalar_work.simd_scalar);
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
						// Capture the mass before projection, retaining its x87 value.
						float const &mass_source = ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3];
						float mass = mass_source;
						__m128 *linear_b_pointer = &body_b.body->linear;
						__m128 *angular_b_pointer = &body_b.body->angular;
						float target = ((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
						// Match the same mass-before-step-scale order as the angular path.

						// Reuse the consumed projection vectors through the bounded update.
						__m128 bounded_work[5];
						bounded_work[0]=*linear_b_pointer;
						bounded_work[1]=a->linear;
						bounded_work[2]=((solver_linear_row_31e7b0*)terms)->a;
						bounded_work[3]=*angular_b_pointer;
						bounded_work[4]=((solver_linear_row_31e7b0*)terms)->linear;
						bounded_work[1]=_mm_sub_ps(bounded_work[1],bounded_work[0]);
						bounded_work[0]=a->angular;
						bounded_work[0]=_mm_mul_ps(bounded_work[0],bounded_work[2]);
						bounded_work[2]=((solver_linear_row_31e7b0*)terms)->b;
						bounded_work[3]=_mm_mul_ps(bounded_work[3],bounded_work[2]);
						bounded_work[1]=_mm_mul_ps(bounded_work[1],bounded_work[4]);
						bounded_work[0]=_mm_add_ps(bounded_work[0],bounded_work[3]);
						bounded_work[0]=_mm_add_ps(bounded_work[0],bounded_work[1]);
						sum_three_to_arena_31c940(bounded_work[0],projection_pool.bounded_linear);
						double mass_error = (target - projection_pool.bounded_linear) * mass;
						float impulse = mass_error * load_error_scale(step);
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
						apply_linear(impulse, ((solver_linear_row_31e7b0 *)terms), state.solving.a_home, scoped_b, result, scalar_work.simd_scalar, bounded_work);
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

				default: continue;
				}

				// Both entry paths consume the same three-axis friction record.
				{

					solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(((solver_linear_row_31e7b0 *)terms) + 2);
					solver_scoped_body_fields_31c940 const &body_b = scoped_b;
					__m128 * &linear_b_pointer = scalar_work.linear_b;

					__m128 *angular_b_pointer = &body_b.body->angular;
					float spin_mass = spin->a.m128_f32[3], spin_target = spin->b.m128_f32[3];

					spin_projection_output_31c940 projection_output(projection_pool.spin_first,linear_b_pointer,&body_b.body->linear);
					reduce_into_spin_output(project_first_spin_velocity(((solver_linear_row_31e7b0*)terms),a,body_b.body),projection_output);
					project_second_spin_fields(((solver_linear_row_31e7b0*)terms)+1,a,*linear_b_pointer,*angular_b_pointer,projection_pool.spin_second);
					// Round the initial spin before the coupled tangential calculation.
					project_spin_angular_velocity(spin, a, *angular_b_pointer, projection_pool.spin_angular);
					scalar_work.first_error = (spin_target * load_friction_scale(step) - projection_pool.spin_angular * load_velocity_scale(step)) * spin_mass;
					// Observe each target before multiplying by the current step scale.
					float captured_target_a = (float const &)((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3];
					scalar_work.pair_b = captured_target_a * load_friction_scale(step) - projection_pool.spin_first * load_velocity_scale(step);
					float captured_target_b = (float const &)((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3];
					scalar_work.fallback_scalar = captured_target_b * load_friction_scale(step) - projection_pool.spin_second * load_velocity_scale(step);
					// Keep the norm, ratio and limited impulses wider until their stores.
					// SIMD consumption below has its own explicit float conversion.
					double impulse_a = ((solver_linear_row_31e7b0 *)terms)[0].b.m128_f32[3] * scalar_work.pair_b + scalar_work.fallback_scalar * operation_float(op, 8);
					double impulse_b = ((solver_linear_row_31e7b0 *)terms)[1].b.m128_f32[3] * scalar_work.fallback_scalar + scalar_work.pair_b * operation_float(op, 8);
					// Reload after the tangential impulses to retain spin-squared first
					// in the x87 norm accumulation.
					double impulse_spin = scalar_work.first_error;
					double norm = impulse_spin * impulse_spin + impulse_b * impulse_b + impulse_a * impulse_a;
					state.solving.spin_limit = operation_float(op, 12);
					if (norm > state.solving.spin_limit * state.solving.spin_limit)
					{
						double factor = state.solving.spin_limit / sqrt(norm);
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


		// Save the first iteration's velocities, then accumulate later ones.
		// Retail consumes consecutive kind-2 bodies before checking the sentinel.
		solver_velocity_31c940 *body = bodies;
		if (state.solving.iteration == 0)
		{
			while ((signed char)body->kind != 3)
			{
				switch ((signed char)body->kind)
				{
				case 2:
					do
					{
						body->accumulated_angular = body->angular;
						body->accumulated_linear = body->linear;
						++body;
					} while (body->kind == 2);
					break;
				}
			}
		}
		else
		{
			while ((signed char)body->kind != 3)
			{
				switch ((signed char)body->kind)
				{
				case 2:
					do
					{
						body->accumulated_angular = _mm_add_ps(body->accumulated_angular, body->angular);
						body->accumulated_linear = _mm_add_ps(body->accumulated_linear, body->linear);
						++body;
					} while (body->kind == 2);
					break;
				}
			}
		}
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

			residual_kind = (signed char)op[2];
			for (;; residual_kind = (signed char)op[2])
			{
				switch (residual_kind)
				{
				case 0: break;
				case 1:
				{
					solver_header_31c940 *header = (solver_header_31c940 *)op;
					a = header->a; b = header->b; terms = header->terms; op += 24;
					continue;
				}
				case 5:
				{
					// Consume up to two paired records, then join the friction path.
					update_residual_pair(terms, a, b, projection_pool.residual_normal_first, projection_pool.residual_normal_second); result_cursor.advance(2); op += op[1];
					// Redispatch the classification already read for this pair.
					residual_kind = (signed char)op[2];
					if (residual_kind != 5) continue;
					update_residual_pair(terms, a, b, projection_pool.residual_bounded_first, projection_pool.residual_bounded_second); result_cursor.advance(2); op += op[1];
					if (op[2] != 6) continue;
				}
				case 6:
				{
					{
						update_residual_pair(terms, a, b, projection_pool.residual_circular_first, projection_pool.residual_circular_second);
						solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)terms;
						subtract_angular_residual(spin, a, b, projection_pool.residual_spin);
						terms += 2;
						result_cursor.advance(3);
						op += op[1];
					}
					// The header opcode takes precedence over its classification byte.
					if (*op == 1)
					{
						solver_header_31c940 *header = (solver_header_31c940 *)op;
						a = header->a; b = header->b; terms = header->terms; op += 24;
					}
					continue;
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
					continue;
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
					continue;
				}
				case 2: op += op[1]; continue;

				default: continue;
				}
				break;

			}

		}
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
				float first_impulse = result[0];
				long const &stride_source = state.writing.stride;
				long first_stride = stride_source;
				*(float *)destination = first_impulse;
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
					*(float *)destination = *result;
					*(float *)(destination + 4) = terms[0].m128_f32[3] * *inverse_time_pointer;
					destination += state.writing.stride;
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
					*(float *)destination = *result;
					*(float *)(destination + 4) = terms[1].m128_f32[3] * *inverse_time_pointer;
					destination += state.writing.stride;
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
