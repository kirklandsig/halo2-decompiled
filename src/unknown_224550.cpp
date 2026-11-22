#include "unknown_11c920.h"
#include "unknown_0259d0.h"
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
struct s_particle_value_cache
{
	real values[17];
	dword mask;
	void *field_48;
	void *field_4c;
	void const *field_50;

	void function_173c10(dword mask);
};
union s_draw_sprite_uv { box2f bounds; real values[4]; };
struct s_draw_sprite_definition
{
	dword flags;
	byte field_4[4];
	short first_sequence, sequence_count;
	byte field_c[0x1c-0xc];
	s_particle_property color, alpha, size, angle, frame;
	byte field_6c[0x98-0x6c];
	dword field_98;
	s_draw_sprite_uv geometry;
	box2f bounds;
};
struct s_draw_sprite_seed
{
	byte field_0[0x14];
	word field_14, field_16;
	byte field_18[0x3c-0x18];
	dword color;
};
struct s_draw_sprite_frame
{
	byte field_0[8];
	box2f bounds;
	point2f pivot;
};
struct s_draw_sprite_sequence
{
	byte field_0[0x34];
	long count;
	s_draw_sprite_frame const *frames;
};
struct s_draw_sprite_bitmap
{
	short type;
	byte field_2[0x3c-2];
	long count;
	s_draw_sprite_sequence const *sequences;
};
struct s_draw_sprite_context
{
	s_draw_sprite_definition const *definition;
	s_draw_sprite_bitmap const *bitmap;
	s_draw_sprite_sequence const *sequence;
	real width;
};
typedef char check_draw_sprite_mask[offsetof(s_draw_sprite_definition, field_98)==0x98?1:-1];
typedef char check_draw_sprite_frame[sizeof(s_draw_sprite_frame)==0x20?1:-1];

real function_246cd0(s_particle_property const *, real const *);
void function_246d80(s_particle_property const *, real const *, color3f *);
color3f *unpack_color3f(dword, color3f *);
dword __cdecl pack_color4f(color4f const *);
long function_016ae0(real);
void function_23ed30(point3f *,point3f *,real,real,dword,real,real *,real *,real *);

PRIVATE inline long sprite_flip(s_draw_sprite_definition const *definition,s_draw_sprite_seed const *seed)
{
	long flip=(definition->flags&2) && (seed->field_14&1)?1:0;
	if((definition->flags&4) && (seed->field_16&1)) flip|=2;
	else flip&=~2;
	return flip;
}
PRIVATE inline real box2f_width(box2f const *bounds)
{
	return bounds->x1-bounds->x0;
}
PRIVATE inline real box2f_height(box2f const *bounds)
{
	return bounds->y1-bounds->y0;
}
PRIVATE inline real interpolate_linear(real a,real b,real t)
{
	return (1.0f-t)*a+b*t;
}
PRIVATE inline void function_x0ec080(box2f const *bounds,long flip,real *out)
{
	real dy=(flip&2)?bounds->y0-bounds->y1:bounds->y1-bounds->y0;
	real dx=(flip&1)?bounds->x0-bounds->x1:bounds->x1-bounds->x0;
	real y=(flip&2)?bounds->y1:bounds->y0;
	real x=(flip&1)?bounds->x1:bounds->x0;
	out[0]=x; out[1]=y; out[2]=dx; out[3]=dy;
}

// b7M (11-17) walk against retail, fast build on r830: walked all of 0x224550 statement by statement (field offsets,
// branch senses, callees with their register arguments, constants): no behaviour difference found. Checked: mask
// refresh 0x22457b-0x224598; angle/alpha/colour products and the lower-first four-channel clamp 0x224653-0x2246cb
// (alpha is not seed-scaled, 0x2245d5-0x224625); pack 0x2246d6; 0x10000 arm 0x2246e0-0x2247eb (bounds to uv_a, geometry
// copied raw, flip, in-place encode, uv_b=uv_a); bitmap arm 0x224846-0x2249bf (type 3, the 0x10 pick: word seed modulo
// and clamp 0x224879-0x224885, 0x8 interval 0x22489f, count>0 0x2248ac, 0x20 blend 0x224907); aspect and inverse sums
// 0x224a7f-0x224b24; flip and encode of both sets 0x224b2a-0x224bdf; geometry 0x224bf1-0x224c6f; quarter turn 0x224c75;
// final call 0x224caa-0x224cd2 (size*scale, blend, uv_a, uv_b, geometry in ebx). Constants read, all equal ours: 2pi
// 0x2245a0, 2.0 0x2245ed, 1.0 0x22465a, 0.5 0x2247fc, -1.0 0x224c05, pi/2 0x224c80. Caller 0x2243c0 and the seven
// callees MATCH. Existing-helper check: the index clamp as PIN_126660 (needs #include "unknown_124f90.h", not added)
// 194 (same code). Score stays 194 (fast, r830): what remains is the known tint/uv_b home swap and the blend's XMM5
// residence.
// @retail 0x224550
void __stdcall function_224550(s_draw_sprite_seed const *seed,s_particle_value_cache *values,
	point3f *position,point3f *direction,real scale,color4f const *color,s_draw_sprite_context *context)
{
	real blend=0.0f;
	s_draw_sprite_definition const *definition=context->definition;
	values->function_173c10(definition->field_98&0xf80f);
	real angle=function_246cd0(&definition->angle,values->values)*6.2831854820251465f;
	color4f tint;
	function_246d80(&definition->color,values->values,(color3f *)&tint.red);
	tint.alpha=function_246cd0(&definition->alpha,values->values)*color->alpha;
	if(definition->flags&0x180)
	{
		color3f seed_color;
		unpack_color3f(seed->color,&seed_color);
		tint.red=seed_color.red*tint.red*2.0f;
		tint.green=seed_color.green*tint.green*2.0f;
		tint.blue=seed_color.blue*tint.blue*2.0f;
	}
	tint.red=color->red*tint.red;
	tint.green=color->green*tint.green;
	tint.blue=color->blue*tint.blue;
	real *components=&tint.alpha;
	for(long channel=0;channel<4;++channel)
	{
		if(components[channel]<0.0f) components[channel]=0.0f;
		else if(components[channel]>1.0f) components[channel]=1.0f;
	}
	dword packed=pack_color4f(&tint);
	s_draw_sprite_uv geometry;
	s_draw_sprite_uv uv_a,uv_b;
	if(definition->flags&0x10000)
	{
		uv_a.bounds=definition->bounds;
		geometry=definition->geometry;
		function_x0ec080(&uv_a.bounds,sprite_flip(definition,seed),uv_a.values);
		uv_b=uv_a;
	}
	else
	{
		s_draw_sprite_bitmap const *bitmap=context->bitmap;
		uv_a.bounds.x0=0.0f; uv_a.bounds.x1=1.0f; uv_a.bounds.y0=0.0f; uv_a.bounds.y1=1.0f;
		box2f bounds_b;
		bounds_b.x0=0.0f; bounds_b.x1=1.0f; bounds_b.y0=0.0f; bounds_b.y1=1.0f;
		point2f pivot={0.5f,0.5f};
		if(bitmap && bitmap->type==3)
		{
			if(definition->flags&0x10)
			{
				long count=definition->sequence_count<1?1:definition->sequence_count;
				long index=seed->field_16%count+definition->first_sequence;
				if(index<0) index=0; else if(index>bitmap->count-1) index=bitmap->count-1;
				context->sequence=&bitmap->sequences[index];
			}
			if(context->sequence)
			{
				long intervals=context->sequence->count;
				if(definition->flags&8)
					intervals--;
				if(context->sequence->count>0)
				{
					real frame=function_246cd0(&definition->frame,values->values)*(real)intervals;
					long index=function_016ae0(frame);
					s_draw_sprite_frame const *first=&context->sequence->frames[index%context->sequence->count];
					s_draw_sprite_frame const *next=&context->sequence->frames[(index+1)%context->sequence->count];
					if(!(definition->flags&0x20)) blend=frame-(real)index;
					uv_a.bounds=first->bounds; bounds_b=next->bounds;
					pivot.x=interpolate_linear(first->pivot.x,next->pivot.x,blend);
					pivot.y=interpolate_linear(first->pivot.y,next->pivot.y,blend);
				}
			}
		}
		real inv_ax=1.0f/box2f_width(&uv_a.bounds),inv_ay=1.0f/box2f_height(&uv_a.bounds),inv_bx=1.0f/box2f_width(&bounds_b),inv_by=1.0f/box2f_height(&bounds_b);
		real aspect_x=1.0f,aspect_y=1.0f;
		long flip=sprite_flip(definition,seed);
		if(box2f_width(&uv_a.bounds)>box2f_height(&uv_a.bounds)) aspect_y=box2f_height(&uv_a.bounds)*inv_ax; else aspect_x=box2f_width(&uv_a.bounds)*inv_ay;
		if(box2f_width(&bounds_b)>box2f_height(&bounds_b)) aspect_y=interpolate_linear(aspect_y,box2f_height(&bounds_b)*inv_bx,blend);
		else aspect_x=interpolate_linear(aspect_x,box2f_width(&bounds_b)*inv_by,blend);
		real inv_x=interpolate_linear(inv_ax,inv_bx,blend);
		real inv_y=interpolate_linear(inv_ay,inv_by,blend);
		real width=context->width*aspect_x;
		function_x0ec080(&uv_a.bounds,flip,uv_a.values); function_x0ec080(&bounds_b,flip,uv_b.values);
		real x=inv_x*pivot.x*2.0f-1.0f,y=inv_y*pivot.y*2.0f-1.0f;
		real x0=(-1.0f-x)*width, y0=(-1.0f-y)*aspect_y;
		real x1=(1.0f-x)*width, y1=(1.0f-y)*aspect_y;
		geometry.values[0]=x0; geometry.values[1]=y0; geometry.values[2]=x1-x0; geometry.values[3]=y1-y0;
	}
	if(definition->flags&0x2000) angle+=1.5707963705062866f;
	function_23ed30(position,direction,function_246cd0(&definition->size,values->values)*scale,
		angle,packed,blend,uv_a.values,uv_b.values,geometry.values);
}
