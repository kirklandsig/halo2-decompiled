#include "unknown_11c920.h"
#include "frame_storage.h"
#include "globals.h"
#include "vector_products.h"
#include "normalize3d.h"
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

/* the rotation by an angle (given as its sine and cosine) about a unit axis */
PRIVATE __forceinline void draw_rotation(vector3f const *axis, real sine, real cosine, matrix3x3 *out)
{
	vector3f axis_squared, axis_sine;
	axis_squared.i=axis->i*axis->i;
	axis_squared.j=axis->j*axis->j;
	axis_squared.k=axis->k*axis->k;
	vector3f_scale(axis,sine,&axis_sine);
	out->forward.i=(1.0f-axis_squared.i)*cosine+axis_squared.i;
	real t=1.0f-cosine;
	real tij=(axis->j*t)*axis->i;
	out->left.i=tij-axis_sine.k;
	out->forward.j=tij+axis_sine.k;
	out->left.j=(1.0f-axis_squared.j)*cosine+axis_squared.j;
	real tk=t*axis->k;
	real tki=tk*axis->i;
	out->up.i=tki+axis_sine.j;
	out->forward.k=tki-axis_sine.j;
	out->up.k=(1.0f-axis_squared.k)*cosine+axis_squared.k;
	real tkj=tk*axis->j;
	out->up.j=tkj-axis_sine.i;
	out->left.k=tkj+axis_sine.i;
}

/* the same rotation with the i-j term as (t * i) * j, the text of the matched 0x1544e0: retail's
   copy for the g_4687b0 axis forms the term that way and its other copy as (j * t) * i above;
   VC keeps a parenthesised float product's order and reschedules an unparenthesised chain,
   so function_224fd0 needs both texts (byte match) */
PRIVATE __forceinline void draw_rotation_up(vector3f const *axis, real sine, real cosine, matrix3x3 *out)
{
	vector3f axis_squared, axis_sine;
	axis_squared.i=axis->i*axis->i;
	axis_squared.j=axis->j*axis->j;
	axis_squared.k=axis->k*axis->k;
	vector3f_scale(axis,sine,&axis_sine);
	out->forward.i=(1.0f-axis_squared.i)*cosine+axis_squared.i;
	real t=1.0f-cosine;
	real tij=(t*axis->i)*axis->j;
	out->left.i=tij-axis_sine.k;
	out->forward.j=tij+axis_sine.k;
	out->left.j=(1.0f-axis_squared.j)*cosine+axis_squared.j;
	real tk=t*axis->k;
	real tki=tk*axis->i;
	out->up.i=tki+axis_sine.j;
	out->forward.k=tki-axis_sine.j;
	out->up.k=(1.0f-axis_squared.k)*cosine+axis_squared.k;
	real tkj=tk*axis->j;
	out->up.j=tkj-axis_sine.i;
	out->left.k=tkj+axis_sine.i;
}

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
	case 0:
	case 1:
	case 3:
		draw_rotation(g_4687ac,sine,cosine,&rotation);
		break;
	case 2:
	case 4:
		draw_rotation_up(g_4687b0,sine,cosine,&rotation);
		break;
	default: __assume(0);
	}
	vector3f_scale(&rotation.forward,scale_i,&rotation.forward);
	vector3f_scale(&rotation.left,scale_j,&rotation.left);
	vector3f_scale(&rotation.up,scale_k,&rotation.up);
	switch(definition->orientation)
	{
	case 0:
		basis.up=g_485618.up;
		vector3f_negate_ab(&g_485618.forward,&basis.forward);
		cross3f(&basis.up,&basis.forward,&basis.left);
		break;
	case 1:
	case 2:
		basis.up=*direction;
		function_30bf0(&basis.up);
		function_11d000(&basis.up,&basis.forward);
		cross3f(&basis.up,&basis.forward,&basis.left);
		break;
	case 3:
	case 4:
		basis.up=*g_4687b0;
		cross3f(&basis.up,&g_485618.forward,&basis.left);
		cross3f(&basis.left,&basis.up,&basis.forward);
		break;
	}
	function_142eb0(&basis,&rotation,&rotation);
	word *indices=definition->indices;
	D3DDevice_SetVertexData4f(12,rotation.forward.i,rotation.left.i,rotation.up.i,position->x);
	D3DDevice_SetVertexData4f(13,rotation.forward.j,rotation.left.j,rotation.up.j,position->y);
	D3DDevice_SetVertexData4f(14,rotation.forward.k,rotation.left.k,rotation.up.k,position->z);
	/* the colour through the device's inline method: the basis crosses above then take retail's operand order (byte match) */
	if(context->constant_index!=NONE)
		D3DDevice::SetVertexData4f(context->constant_index,color->red,color->green,color->blue,color->alpha);
	word count=span->count, first=span->first;
	function_1cf50();
	D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST,count,indices+first);
}
