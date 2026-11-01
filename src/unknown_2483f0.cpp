// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_2483F0.CPP: the particle locations (g_51ec8c) and the particle
   emitters chained to each (g_51ec88) */

#include "unknown_11c920.h"
#include "frame_storage.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include "globals.h"
#include "vector_products.h"

extern s_record_pool *g_51ec84;
extern s_record_pool *g_51ec88;
extern s_record_pool *g_51ec8c;

/* the particle emitters (0x4c bytes each): the particles each one owns
   (g_51ec84) are chained from it */
struct s_particle_emitter_datum
{
	short salt;
	word particle_count;
	long first_particle_index;
	long next_index;
	real unknown0c;
	byte unknown10[0x4c - 0x10];
};

/* the particle locations (0x34 bytes each) */
struct s_particle_location_datum
{
	short salt;
	byte unknown02;
	byte random;
	union
	{
		struct
		{
			long first_emitter_index;
			long last_emitter_index;
			long next_index;
		};
		long link_indices[3];
	};
	point3f position;
	real unknown1c;
	vector3f vector;
	real unknown2c;
	dword unknown30;
};

dword g_4ba034;

/* the particles (0x40 bytes each) */
struct s_particle_datum
{
	short salt;
	word unknown02;
	long next_index;
	byte unknown08[0x40 - 0x08];
};

/* deletes the particles of an emitter */
// @retail 0x2483b0
void function_2483b0(s_particle_emitter_datum *emitter)
{
	long index = emitter->first_particle_index;
	if (index != NONE)
	{
		long next;
		do
		{
			next = ((s_particle_datum *)g_51ec84->data)[index & 0xffff].next_index;
			record_pool_release(g_51ec84, index);
			index = next;
		} while (next != NONE);
	}
	emitter->particle_count = 0;
	emitter->first_particle_index = NONE;
}

PRIVATE __forceinline void function_2483f1(s_particle_emitter_datum *arg_0, long *arg_1, long *arg_2, s_record_pool *arg_3, long arg_4)
{
	long const &ordinal = (arg_0 - (s_particle_emitter_datum *)arg_3->data);
	long local_0 = (arg_4 << 16) | ordinal;
	arg_0->next_index = NONE;
	if (*arg_1 == NONE)
		*arg_1 = local_0;
	if (*arg_2 != NONE)
		((s_particle_emitter_datum *)arg_3->data)[*arg_2 & 0xffff].next_index = local_0;
	*arg_2 = local_0;
}

PRIVATE __forceinline void function_248d91(s_particle_location_datum *arg_0, long *arg_1, long *arg_2, s_record_pool *arg_3, long arg_4)
{
	long const &ordinal = (arg_0 - (s_particle_location_datum *)arg_3->data);
	long local_0 = (arg_4 << 16) | ordinal;
	arg_0->next_index = NONE;
	if (*arg_1 == NONE)
		*arg_1 = local_0;
	if (*arg_2 != NONE)
		((s_particle_location_datum *)arg_3->data)[*arg_2 & 0xffff].next_index = local_0;
	*arg_2 = local_0;
}

#pragma inline_depth(1)
// @retail 0x2483f0
void function_2483f0(s_particle_emitter_datum *emitter, long *first_index, long *last_index)
{
	long local_0 = *(short const volatile *)&emitter->salt;
	function_2483f1(emitter, first_index, last_index, g_51ec88, local_0);
}
#pragma inline_depth(255)

#pragma inline_depth(1)
// @retail 0x248d90
void function_248d90(s_particle_location_datum *particle_location, long *first_index, long *last_index)
{
	long local_0 = *(short const volatile *)&particle_location->salt;
	function_248d91(particle_location, first_index, last_index, g_51ec8c, local_0);
}
#pragma inline_depth(255)

/* the frame a particle system is drawn in */
struct s_particle_frame
{
	byte unknown00[0x10];
	matrix3x3 rotation;
	point3f position;
};

/* the offset of first person particles: a point, then a forward and an up
   vector */



matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);

static inline void particle_cross_product3d(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - b->j * a->k;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

/* the rotation and position a particle system is drawn at: its own, or
   moved by the first person offset */
// @retail 0x248450
void function_248450(matrix3x3 *rotation, s_particle_frame const *frame, point3f *position, matrix3x3 const **rotation_result, point3f const **position_result, bool field_b4)
{
	if (field_b4)
	{
		matrix3x3 offset;

		*rotation = frame->rotation;
		*position = frame->position;
		offset.forward = g_485618.forward;
		particle_cross_product3d(&g_485618.up, &g_485618.forward, &offset.left);
		offset.up = g_485618.up;
		function_142eb0(&offset, rotation, rotation);

		vector3f_rotate_into_axes(&offset, (vector3f const *)position, (vector3f *)position);
		position->x += g_485618.position.x;
		position->y += g_485618.position.y;
		position->z += g_485618.position.z;

		*rotation_result = rotation;
		*position_result = position;
	}
	else
	{
		*rotation_result = &frame->rotation;
		*position_result = &frame->position;
	}
}

/* a new particle location at the origin */
// Matches once g_4ba034 is treated as address-taken (0x117dd0 reads it through its address, as retail
// reloads it there): the frame-count load then stays below the float stores.
// @retail 0x248620
long function_248620()
{
	long location_index = record_pool_allocate(g_51ec8c);

	if (location_index != NONE)
	{
		s_particle_location_datum *particle_location = &((s_particle_location_datum *)g_51ec8c->data)[location_index & 0xffff];

		for (long link_index = 0; link_index < 3; link_index++)
			particle_location->link_indices[link_index] = NONE;
		particle_location->position = *g_468788;
		*(point3f *)&particle_location->vector = *g_468788;
		particle_location->unknown2c = 0.5f;
		particle_location->unknown1c = 1.0f;
		particle_location->unknown30 = g_4ba034;
		particle_location->random = (byte)random_index(&g_4e7408->seed, 0xff);
	}

	return location_index;
}

/* deletes a particle location with its emitters */
// @retail 0x2486e0
void __stdcall function_2486e0(long particle_location_index)
{
	s_particle_location_datum *particle_location = &((s_particle_location_datum *)g_51ec8c->data)[particle_location_index & 0xffff];
	long emitter_index = particle_location->first_emitter_index;

	if (emitter_index != NONE)
	{
		s_record_pool *data = g_51ec88;
		long next_index;

		do
		{
			next_index = ((s_particle_emitter_datum *)data->data)[emitter_index & 0xffff].next_index;
			function_2483b0(&((s_particle_emitter_datum *)data->data)[emitter_index & 0xffff]);
			record_pool_release(data, emitter_index);
			emitter_index = next_index;
		} while (next_index != NONE);
	}

	record_pool_release(g_51ec8c, particle_location_index);
}

/* the particles of a location's emitters */
// @retail 0x248d50
long function_248d50(s_particle_location_datum *particle_location)
{
	long emitter_index = particle_location->first_emitter_index;
	long count = 0;

	if (emitter_index != NONE)
	{
		s_particle_emitter_datum *emitters = (s_particle_emitter_datum *)g_51ec88->data;

		do
		{
			s_particle_emitter_datum *emitter = &emitters[emitter_index & 0xffff];

			count += emitter->particle_count;
			emitter_index = emitter->next_index;
		} while (emitter_index != NONE);
	}

	return count;
}

/* the fade of something drawn between two distances: in over the first
   range, out over the last */
struct s_distance_fade
{
	byte unknown00[0x18];
	real near_distance;
	real near_fade_range;
	real near_fade_scale;
	real far_distance;
	real far_fade_range;
	real far_fade_scale;
};

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_510c50_fade_view
{
	byte unknown00[5];
	bool fade_disabled;
};

// @retail 0x249bb0
real function_249bb0(s_distance_fade const *fade, real distance)
{
	s_510c50_fade_view *globals = (s_510c50_fade_view *)g_510c50;
	real result = 0.0f;

	if (globals && globals->fade_disabled)
	{
		result = 1.0f;
	}
	else if (distance > fade->near_distance && fade->far_distance > distance)
	{
		if (fade->near_fade_range + fade->near_distance > distance)
			result = (distance - fade->near_distance) * fade->near_fade_scale;
		else if (distance > fade->far_distance - fade->far_fade_range)
			result = (fade->far_distance - distance) * fade->far_fade_scale;
		else
			result = 1.0f;
	}

	return result;
}
