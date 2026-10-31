#include "unknown_11c920.h"
#include "frame_storage.h"
#include "globals.h"
#include <xtl.h>
#include <math.h>
#include <stddef.h>

// @flags /O2 /arch:SSE /Gr

struct s_tag_data { long size; byte *address; };
struct s_particle_property
{
	short input_index, range_index;
	word modifier;
	short modifier_index;
	s_tag_data function;
};
struct s_draw_value_cache
{
	real values[17];
	dword field_44;
	void *field_48;
	void *field_4c;
	void const *field_50;
};
struct s_draw_index_span
{
	byte field_0[4];
	word first, count;
};
struct s_draw_mesh_definition
{
	long field_0;
	long orientation;
	byte field_8[0x20-8];
	s_particle_property scale_x, scale_y, scale_z, angle;
	byte field_60[0x80-0x60];
	long span_count;
	s_draw_index_span *spans;
	byte field_88[0x94-0x88];
	word *indices;
	byte field_98[0xd8-0x98];
	dword field_d8;
};
struct s_draw_mesh_context { s_draw_mesh_definition const *definition; long constant_index; };
struct s_draw_mesh_seed { byte field_0[0x1a]; word field_1a; };

typedef char check_draw_mesh_mask[offsetof(s_draw_mesh_definition, field_d8)==0xd8?1:-1];

void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values);
real function_246cd0(s_particle_property const *property, real const *values);
real function_30bf0(vector3f *v);
vector3f *function_11d000(vector3f const *v, vector3f *out);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
void function_1cf50();

PRIVATE inline void draw_cross(vector3f const *a, vector3f const *b, vector3f *out)
{
	out->i = a->j*b->k-a->k*b->j;
	out->j = a->k*b->i-a->i*b->k;
	out->k = a->i*b->j-a->j*b->i;
}

PRIVATE __forceinline void draw_rotation(vector3f const *axis, real sine, real cosine, matrix3x3 *out)
{
	real k2=axis->k*axis->k;
	real i2=axis->i*axis->i;
	real j2=axis->j*axis->j;
	real ij=(1.0f-cosine)*axis->i*axis->j;
	real ik=(1.0f-cosine)*axis->k*axis->i;
	real jk=(1.0f-cosine)*axis->k*axis->j;
	out->forward.i=(1.0f-i2)*cosine+i2;
	out->left.i=ij-sine*axis->k;
	out->forward.j=ij+sine*axis->k;
	out->left.j=(1.0f-j2)*cosine+j2;
	out->forward.k=ik-sine*axis->j;
	out->up.i=ik+sine*axis->j;
	out->up.k=(1.0f-k2)*cosine+k2;
	out->up.j=jk-sine*axis->i;
	out->left.k=jk+sine*axis->i;
}

// score 380 (was 380): Frame 0x64 versus 0x6c; rotation copies and product homes differ.
// Round 34 (lane A, machine 3): retail's rotation switch has two case bodies (the g_4687ac and
// the g_4687b0 axis) and one shared tail; grouping the five cases by axis gives 435 and 430
// (reversed groups), so the five-case form stays; a 300 s permuter run reached 321 differing
// instructions, its only new suggestion is a wrong swap of the row-13 components.
// Scalar axis inputs 393 plain (391 fast, frame 0x6c); all axis snapshots 754; third component
// snapshot 820; first two 900; camera record snapshot 615. Scalar-axis factor names, product names
// and independent square order all 391 (fast). Basis scope ends after composition 380;
// diagonal-first stores 610; cross destination return 380; second-axis cases first 380; scales
// after rotation 802 (fast).
// Frame 0x64 versus 0x6c; independently scheduled axis expansions and cosine/product spills remain.
// Scalar axis inputs 393 plain (391 fast, frame 0x6c); all axis snapshots 754;
// third component snapshot 820; first two 900; camera record snapshot 615.
// Scalar-axis factor names, product names and independent square order all 391 (fast).
// Reverse scalar inputs 441; trig-first scalar inputs 441; reversed inputs with orientation snapshot 441; reversed inputs with shared axis cases 400; reversed inputs with separate trig declarations 441.
// Round 121: Both axis pointers retained 470; complete local matrix then publication 1142; second axis pointer across switches 501; double angle 393; all three scale products factor-first 380. Original body and both helper bodies restored.
// Complete rows forward-left-up 683; forward-up-left 1012; left-forward-up 664;
// left-up-forward 1188; up-forward-left 1324; up-left-forward 1259; complete columns
// jki 1056 and kij 1369. Original interleaved stores and helper bodies restored.
// Complete native axis input copy 424; copy with complement/sine products/both
// 430/470/418; diagonal-first copy with the same four forms 428/468/461/515.
// All recover frame 0x6c and first +0xaf but regress; original helper restored.
// Round 235: Real-rounded trigonometric results in double storage: sine/cosine/both
// 465/654/764; with native double angle 448/451/576; both results with scalar axis
// inputs 783, and with angle 449. Original real storage, helper interface and body restored.
// Round 237: Parallel diagonal remainders with sine/complement sharing
// 410/443/573/518; parallel diagonal cosine products with the same sharing
// 410/443/573/518. Original helper expressions restored.
// Rotation case orders 43210/32104/12340/02143/13042/31024/20413/41230:
// 449/450/454/454/450/450/449/449 fast; original order restored.
// Basis case groups 021/102/120/201/210 380/381/383/384/383; basis 102
// with rotation orders 01243/01324/10423 393/451/393. Original order restored.
// Round 160: native destination/both receivers 380/380; immutable product/all producer
// lifetimes 380/380; complete cross inputs alone/with receiver 380/380; uniform
// context-definition observations 449; combined cross/receiver/producers 380.
// Round 76 rotation helper forms: products in i,j,k order with a named complement 429; forward/left/up row
// stores with a named complement 1293 (frame 0x6c, cases expanded); sine products named first 921; named
// complement alone 429; axis cases grouped 0,1,3 / 2,4 435 (aligned listing 374 -> 323).
// Round 162: Shared row setters in FLU/FUL/LFU/LUF/UFL/ULF order 1041/1550/704/715/985/706;
// complete complement 398, with all sine products 983. Other complemented row orders
// 828/414/701/1561/764; complemented scalar/native axis inputs 1111/1357. The 398 form has frame
// 0x68 versus 0x6c but still regresses. Original helper bodies, fields, arithmetic and calls
// restored.
// DL175: Three independent/shared-counter row scaling loops [1630, 1631]; full-width unsigned
// count/first/both [395, 396, 402]; both with independent rows 1619; immutable word span carriers
// alone/with rows [380, 1630]. Original helper, scaling and span carriers restored.
// Round 309: Existing cross producer, scalar/native-vector inputs and pointer/native-result
// outputs 468/924/705/500; with scalar rotation inputs 389/617/840/517. The 389 form
// recovers frame 0x6c, first +0xaf, but regresses; original helpers and full body restored.
// Scaled rotation producer with native input/complement/sine-product subsets
// 464/639/799/979/765/605/627/612; original helper and post-switch scaling restored.
// Round 249: Shared staged cross-product families 0 / 1-2 / 3-4, all seven
// nonempty subsets 380 each; all with shared in-place row scaling 380. Frame
// remains 0x64/0x6c; original helpers, include set and complete body restored.
// @retail 0x224fd0
void __stdcall function_224fd0(s_draw_mesh_seed const *seed, s_draw_value_cache *values,
	point3f const *position, vector3f const *direction, real scale, color4f const *color,
	s_draw_mesh_context const *context)
{
	s_draw_mesh_definition const *definition=context->definition;
	s_draw_index_span const *span=&definition->spans[seed->field_1a%definition->span_count];
	dword mask=definition->field_d8&0xf80f;
	function_173ba0(~values->field_44&mask,values->field_48,values->field_4c,values->field_50,values->values);
	values->field_44|=mask;
	real angle=function_246cd0(&definition->angle,values->values)*6.2831854820251465f;
	real sine=(real)sin(angle), cosine=(real)cos(angle);
	real scale_i=function_246cd0(&definition->scale_x,values->values)*scale;
	real scale_j=function_246cd0(&definition->scale_y,values->values)*scale;
	real scale_k=function_246cd0(&definition->scale_z,values->values)*scale;
	matrix3x3 rotation, basis;
	switch(definition->orientation)
	{
	case 0: draw_rotation(g_4687ac,sine,cosine,&rotation); break;
	case 1: draw_rotation(g_4687ac,sine,cosine,&rotation); break;
	case 2: draw_rotation(g_4687b0,sine,cosine,&rotation); break;
	case 3: draw_rotation(g_4687ac,sine,cosine,&rotation); break;
	case 4: draw_rotation(g_4687b0,sine,cosine,&rotation); break;
	default: __assume(0);
	}
	rotation.forward.i*=scale_i; rotation.forward.j*=scale_i; rotation.forward.k*=scale_i;
	rotation.left.i*=scale_j; rotation.left.j*=scale_j; rotation.left.k*=scale_j;
	rotation.up.i*=scale_k; rotation.up.j*=scale_k; rotation.up.k*=scale_k;
	switch(definition->orientation)
	{
	case 0:
		basis.forward.i=-g_485618.forward.i;
		basis.forward.j=-g_485618.forward.j;
		basis.forward.k=-g_485618.forward.k;
		basis.up=g_485618.up;
		draw_cross(&basis.up,&basis.forward,&basis.left);
		break;
	case 1:
	case 2:
		basis.up=*direction;
		function_30bf0(&basis.up);
		function_11d000(&basis.up,&basis.forward);
		draw_cross(&basis.up,&basis.forward,&basis.left);
		break;
	case 3:
	case 4:
		basis.up=*g_4687b0;
		vector3f facing=g_485618.forward;
		draw_cross(&basis.up,&facing,&basis.left);
		draw_cross(&basis.left,&basis.up,&basis.forward);
		break;
	}
	function_142eb0(&basis,&rotation,&rotation);
	word *indices=definition->indices;
	D3DDevice_SetVertexData4f(12,rotation.forward.i,rotation.left.i,rotation.up.i,position->x);
	D3DDevice_SetVertexData4f(13,rotation.forward.j,rotation.left.j,rotation.up.j,position->y);
	D3DDevice_SetVertexData4f(14,rotation.forward.k,rotation.left.k,rotation.up.k,position->z);
	if(context->constant_index!=NONE)
		D3DDevice_SetVertexData4f(context->constant_index,color->red,color->green,color->blue,color->alpha);
	word count=span->count, first=span->first;
	function_1cf50();
	D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST,count,indices+first);
}
