// @flags /O2 /Oi /arch:SSE /Gr
/* UNKNOWN_2056E0.CPP: applying a vehicle's physics step: the forces of its
   contacts and the caller's force and torque, a braking that pulls its
   velocities back (capped), and the flags of which contacts touch */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_1428b0.h"
#include "vector_products.h"
#include <math.h>

struct s_vehicle_physics_state;

/* the physics state as this step reads it (vehicles.cpp) */
struct s_vehicle_step_definition
{
	byte unknown00[0x44];
	long contact_count;
};

struct s_vehicle_step_contact
{
	byte unknown00[0x80];
	bool touching;
	bool lifted;
	byte unknown82[0xd8 - 0x82];
};

struct s_vehicle_step_state
{
	byte unknown000[4];
	s_vehicle_step_definition *definition;
	real scale;
	matrix3x3 rotation;
	byte unknown030[0x6c - 0x30];
	real mass;
	byte unknown070[0xb10 - 0x70];
	s_vehicle_step_contact contacts[16];
};

/* the vehicle object as this step reads it */
struct s_vehicle_step_object
{
	byte unknown000[0xb4];
	long havok_component_index;
	byte unknown0b8[0x38e - 0xb8];
	word contact_flags;
};

struct s_vehicle_step_header
{
	byte unknown00[8];
	s_vehicle_step_object *object;
};

/* the motion's virtual slot that gives its local inverse inertia */
class c_vehicle_step_motion
{
public:
	virtual void slot00(void) {}
	virtual void slot01(void) {}
	virtual void slot02(void) {}
	virtual void slot03(void) {}
	virtual void slot04(void) {}
	virtual void slot05(void) {}
	virtual void slot06(void) {}
	virtual void slot07(void) {}
	virtual void slot08(void) {}
	virtual void slot09(void) {}
	virtual void slot0a(void) {}
	virtual void slot0b(void) {}
	virtual void slot0c(void) {}
	virtual void inverse_inertia_local(hkRotation &inertia) const {}
};

bool __stdcall function_206090(s_vehicle_physics_state *state, vector3f *linear, vector3f *angular);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);

/* scales a vector down to the given length when it is longer; the double
   scale goes straight to the helper (a named double local here would make
   LTCG keep every caller's stack 8-byte aligned, padding their frames) */
PRIVATE __forceinline void vector_limit_length(vector3f *v, real maximum)
{
	real length_squared = v->k * v->k + v->j * v->j + v->i * v->i;
	if (length_squared > maximum * maximum)
	{
		scale_vector_by_double(v, maximum / sqrt(length_squared), v);
	}
}

/* the same cap with the scale kept in a real, as the linear brake's cap is */
PRIVATE __forceinline void vector_limit_length_real_scale(vector3f *v, real maximum)
{
	real length_squared = v->k * v->k + v->j * v->j + v->i * v->i;
	if (length_squared > maximum * maximum)
	{
		real scale = maximum / sqrt(length_squared);
		v->i = scale * v->i;
		v->j = scale * v->j;
		v->k = scale * v->k;
	}
}

/* the motion's local inverse inertia as a matrix3x3; the 16-byte aligned
   Havok rotation it reads lives in this inlined helper, not in the caller's
   own locals (written in the caller, it changes the conventions LTCG gives
   the caller's callees) */
PRIVATE __forceinline void motion_inverse_inertia_local(c_vehicle_step_motion *motion, matrix3x3 *out)
{
	hkRotation inertia;
	motion->inverse_inertia_local(inertia);
	out->forward.i = inertia.m_col0(0);
	out->forward.j = inertia.m_col0(1);
	out->forward.k = inertia.m_col0(2);
	out->left.i = inertia.m_col1(0);
	out->left.j = inertia.m_col1(1);
	out->left.k = inertia.m_col1(2);
	out->up.i = inertia.m_col2(0);
	out->up.j = inertia.m_col2(1);
	out->up.k = inertia.m_col2(2);
}

// score 0 (was 1): the linear brake's cap keeps its scale in a real, so retail's float zero
// constant multiplies the scale (fmul dword) where the shared cap's double scale produced a
// double zero (fmul qword); the angular cap keeps the shared double scale and is unchanged.
// double 0.0 constant (`fmul qword`), retail by the float 0.0f (`fmul dword`).
// Earlier: Frame 0x94 versus 0xc4; query/rotation overlap, motion/reciprocal and product/brake allocation remain.
// Aligned frame remains 0x94 versus retail 0xc4; motion/reciprocal, product and braking scheduling still differ.
// aligned query/rotation overlap, motion/reciprocal and cap-result allocation remain.
// Earlier: Aligned frame remains 0x94 versus retail 0xc4; motion/reciprocal, product and braking scheduling still differ.
// double cap parameter 249; read-only motion input 184; read-only step view 184; read-only
// component view 184; native contact receiver 184; complete contact mask result 304; separate
// braking result 274; linear brake used after its cap 223.
// Output reference 184; motion reference 184; both references 184; limiter reference 184; both helper
// contracts 184; native column values 382; column library copies 220; copies with output reference 220.
// Frame overlap, motion/reciprocal and brake allocation remain.
// rotation memcpy 184; complete limiter input 351; both 351;
// broad rotation plus limiter input 366; norm accumulation 184; capped stores reversed 184;
// separate integration loops 182 (dropped); torque coordinate loop 286. Frame gap remains.
// rotation/inertia frame sharing and reciprocal/product scheduling still differ.
// Earlier: rotation memcpy 184; complete limiter input 351; both 351;
// Earlier: rotation/inertia frame sharing and reciprocal/product scheduling still differ.
// separate first matrix result 194f; separate final matrix result 213f.
// both matrix results 194f; torque snapshot 271f.
// first result plus torque snapshot 286f; linear change reused 184f.
// angular acceleration reused 184f; both brake workspaces reused 184f.
// first result plus shared force scale 194f; first result plus acceleration output 194f.
// aligned frame overlap and product/brake carriers remain;
// native inertia row/reverse-column/middle-first/reverse-component/reverse-row
// publication 184/184/184/184/184; forward/left/up column memcpy 193/191/195.
// frame overlap and motion/reciprocal, product and brake allocation remain.
// Separate transpose output via returned/completed output 222/195; with first product 201/240;
// with final product 243/240; with both products 201/240. Opening matrices and helpers retained.
// aligned frame overlap, motion/reciprocal and brake allocation remain.
// Query workspace in caller alone/with broad rotation 234/234; live rotation input 232;
// first-product output pointer 224; pointer with separate transpose 236; native square-root
// result, limiter output pointer and both 184/184/184. All helper contracts restored.
// query/rotation overlap and scalar-product allocation remain;
// query output pointer ignored/retained/ordinary/ordinary ignored 184/184/184/184;
// new pointer flow with broad rotation/first product/final product/transpose 221/194/213/222.
// Joint query/rotation output with rotation copy after 0/1/3/4/6/7/8/9 native publications gives 219/219/219/219/219/220/221/221. Frame 0xc4 and prefix through +0xfd recover, but all fast trial scores regress; original query boundary and scoped rotation restored.
// Query-source-only matrix with early/late world declarations 213/213; broad rotation 216;
// distinct first world output 194; completed transpose alone/with first output 277/240;
// late world and broad rotation 216; first world and broad rotation 194. All restored.
// Shared double-factor cap scaling in place/through completed output 227/302 fast;
// with broad rotation 235/310, first matrix output 239/314, completed transpose 240/315.
// Query/rotation frame fixes still worsen cap scheduling; original helpers and matrices restored.
// Round 160: Complete force products, linear integration results, angular integration results, both
// result sets, norm squares, squared cap, all cap products and force-plus-cap products all 184. Every
// expression tree, input observation, existing helper, matrix role and signed contact loop restored.
// Earlier: query/rotation overlap and scalar-product allocation remain;
// Round 131: existing length norm 184; shared cap scale with real first-product
// output 194; shared cap scale with length/dot norms 184/184; shared double cap
// scale 227, with first product 239, with length/dot norms 227/227. Alternate
// norm and double expansion required an exact match; all restored.
// Virtual output pointer/mutable receiver/both 184/184/184; uniform angular terms alone/with
// pointer/receiver/both 184 each; new query/product contract with distinct first result 194.
// Aligned query/rotation overlap remains.
// Wide rotation with query publication columns 021/102/120/201/210 219/221/221/219/221;
// rows 012/021/102 221/219/217. Frame recovers but scores regress; original retained.
// Coordinated body-scalar/limiter-scalar/query-output names and combinations
// 184/184/184/184/184/184/184/184; last form also makes repeated body scalars const. All restored.
// Round 168: Complete native converted matrix by fields/aggregate 270/270; with broad rotation
// 268/268, distinct first product 290/290, both 273/273. Converter and body restored.
// Combined scale/cap ownership: pointer forced/ordinary, reference and native inputs
// 292/292/292/241; with broad rotation 274/274/274/256. Original limiter and scopes restored.
// Round 333: Ordinary limiter/cap-first/both contracts 184/184/184; cap-first/both with broad rotation 221/221; ordinary limiter/cap-first/both with ordinary query expansion 184/184/184. Broad forms recover frame 0xc4 but regress; all original helpers, matrix scopes and calls restored.
// Complete world-inertia producer with matrix/state pointer/reference inputs, forced 233 each;
// ordinary matrix/state pointer/reference 462/468/462/468. Original query boundary restored.
// Round 153: native virtual query value/const-value initialization 184/184; assignment 275/275; each with broad rotation 221/287/221/287. Initializers retain the 0x94 frame; broad initialization recovers 0xc4 but regresses. Original virtual output, helpers and scoped rotation restored.
// Round 76: existing vector3f_scale for the inverse mass, the angular brake, both 184/184/184 (identical
// code: the reciprocal stays the product destination); angular product as three rows 221, with component
// locals 221, with that and the brake helper 221. Aligned difference 143 of 289 (frame and float schedule).
// Round 155: Native inverse-inertia/rotation matrix alignments 16/4,4/16,16/16,8/4,
// 4/8,8/8,16/8,8/16: 240/224/240/241/224/241/240/241. Original native matrix alignments and scoped rotation restored.
// Lane 4 r98: `0.0f - braking / dt` builds as braking * (-1 / dt); `-(braking / dt)` gives retail's
// zero-minus-quotient and its 1265 bytes but 221. With broad rotation 232 (frame 0xc4, first difference
// +0xfd); with it, divisions by the mass, reciprocal-first products and an inline vector-scale helper for
// the mass and angular-brake products 232 each: the products still copy the scalar where retail scales
// the component in place.
// Round 224: Existing query producer owns preceding inverse-mass scaling; scalar/state inputs
// and vector pointer/reference 184/184/184/184; broad-rotation counterparts 221/221/221/221. Original boundary and scopes restored.
// Round 28 (m3 lane 12): `0.0f - braking / dt` compiles to a product with -1/dt; `-(braking / dt)` gives
// retail's subtraction from the zero register and retail's length (1265) but 221 fast; with function-scope
// rotation (frame 0xc4, rotation at +0xac) 232; with one integration loop 221; acceleration folded into the
// angular loop 270; inverse-mass products reversed 221 (no change). Float register allocation then differs.
// Round 66 (fast): the inverse-mass products follow definition order: with the reciprocal defined before the
// force block they scale each component in place as retail and every later SSE register then agrees (aligned
// 105 -> 58) but 170 is not kept (it moves a memory read past a call, and retail divides late, after the motion
// load); defined at the top 263 (spilled across the state query); before the velocity call 200; before the
// motion load 184; written at each use 184; `x = x * reciprocal` 184; `+=` force/torque adds 184.
// Lane 9 r25: rotation declared in a block around the inertia query and assigned after it 221 (frame
// 0xc4, raw inertia +0x70, rotation +0xac as retail's; first difference +0xfd: retail loads the motion
// before the reciprocal and scales each component in place, ours copies the scalar); with the
// reciprocal before the motion 221; with `-(braking / dt)` 232; with `x = x * s` mass products and an
// angular brake copied then scaled 267; with the limiter's products as `*=` 221.
// Round 30 (m3 lane 12): inverse-mass products as `x = x * inverse_mass`, reciprocal before the motion pointer,
// force sum as `+=`, reciprocal written at each use 184 each (identical code: the reciprocal stays the
// product destination); function-scope rotation 221 (frame 0xc4, rotation +0xac, float registers rename).
// Lane 4 r106: the force/torque sums and the four velocity integrations through pointer helpers
// (vector3f_add, vector3f_add_scaled) give retail's division after the motion load and the in-place
// products (184 -> 123 -> 72 with function-scope rotation and `-(braking / dt)`); the limit's scaling
// through scale_vector_by_double 1. For the last difference: brake vectors through vector3f_scale, an
// aggregate initializer or real_point3d_set, `= 0`, zero k first 1/1/1/1/4; double-scale local 7; real
// scale into the double helper 1; limit inline/no inline keyword 1/1; the existing 0xa75d0 for both/linear
// only 72/2 (float scale: retail's zero constant but not its reload order); explicit double conversions 7.
// Round 272: CRT planar/both brake initialization 172/172; complete velocity copy 190;
// both native vector setters 1; consumed limiter output 146, explicit input/output 146;
// native k-j-i norm loop 149; CRT planar zero with consumed limiter 183. All restored; zero-product width remains.
// Existing double-scale force-inline declaration 1
// complete limiter split pointer/reference outputs 1/1
// native value input with pointer/reference outputs 325/325
// complete native returns from value/reference inputs 345/283
// returned output pointer 1. Zero product remains wide
// original limiter and shared-helper declaration restored.
// @retail 0x2056e0
void __stdcall function_2056e0(long vehicle_index, s_vehicle_physics_state *state, real braking,
	vector3f const *force, vector3f const *torque)
{
	real dt = g_510c54->rate;
	s_vehicle_step_object *vehicle = ((s_vehicle_step_header *)g_4e0300->data)[vehicle_index & 0xffff].object;
	s_havok_component *component = havok_component_get(vehicle->havok_component_index);
	s_vehicle_step_state *step = (s_vehicle_step_state *)state;
	vector3f linear_change;
	vector3f angular_change;
	if (!function_206090(state, &linear_change, &angular_change))
		braking = 0.0f;
	if (force)
		vector3f_add(force, &linear_change, &linear_change);
	if (torque)
		vector3f_add(torque, &angular_change, &angular_change);
	vector3f linear_velocity;
	vector3f angular_velocity;
	function_ba1d0(vehicle_index, &linear_velocity, &angular_velocity);
	c_vehicle_step_motion *motion = (c_vehicle_step_motion *)component->rigid_bodies.data[0].rigid_body->m_motion;
	real inverse_mass = 1.0f / step->mass;
	linear_change.i *= inverse_mass;
	linear_change.j *= inverse_mass;
	linear_change.k *= inverse_mass;
	matrix3x3 inverse_inertia;
	motion_inverse_inertia_local(motion, &inverse_inertia);
	matrix3x3 rotation = step->rotation;
	function_142eb0(&rotation, &inverse_inertia, &inverse_inertia);
	function_142eb0(&inverse_inertia, function_142b80(&rotation, &rotation), &inverse_inertia);
	vector3f local_461a4b_2;
	for (long axis = 0; axis < 3; axis++)
		local_461a4b_2.n[axis] = inverse_inertia.up.n[axis] * angular_change.k +
			inverse_inertia.left.n[axis] * angular_change.j + inverse_inertia.forward.n[axis] * angular_change.i;
	vector3f_add_scaled(&linear_velocity, &linear_change, dt, &linear_velocity);
	vector3f_add_scaled(&angular_velocity, &local_461a4b_2, dt, &angular_velocity);
	if (braking > 0.0f)
	{
		real scale = -(braking / dt);
		vector3f linear_brake;
		linear_brake.i = linear_velocity.i * scale;
		linear_brake.j = linear_velocity.j * scale;
		linear_brake.k = 0.0f;
		vector_limit_length_real_scale(&linear_brake, 270.0f);
		vector3f angular_brake;
		angular_brake.i = angular_velocity.i * scale;
		angular_brake.j = angular_velocity.j * scale;
		angular_brake.k = angular_velocity.k * scale;
		vector_limit_length(&angular_brake, 18.0f);
		vector3f_add_scaled(&linear_velocity, &linear_brake, dt, &linear_velocity);
		vector3f_add_scaled(&angular_velocity, &angular_brake, dt, &angular_velocity);
	}
	function_b77d0(vehicle_index, &linear_velocity, &angular_velocity);
	vehicle->contact_flags = 0;
	for (long i = 0; i < step->definition->contact_count; i++)
	{
		if (i >= 0x10000)
			break;
		if (!step->contacts[i].lifted && step->contacts[i].touching)
			vehicle->contact_flags &= ~(1 << i);
		else
			vehicle->contact_flags |= 1 << i;
	}
}
