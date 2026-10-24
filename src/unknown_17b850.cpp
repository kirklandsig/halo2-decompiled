// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_17B850.CPP: the contrails (entry 39 of the lifecycle table) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_211060.h"
#include <string.h>

s_record_pool *g_4ea940;
s_record_pool *g_4ea944;

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
bool __stdcall function_bab40(long object_index, long name, real *value);
void function_17c0e0(long contrail_index, short count, bool force);
void __stdcall function_17c540(long contrail_index, real dt);

/* a contrail (g_4ea944, 0x48 bytes) */
struct s_contrail_datum
{
	short salt;
	word flag0 : 1;
	word : 15;
	long tag_index;
	long object_index;
	short marker_index;
	byte unknown0e[2];
	long marker_name;
	real scale;
	short sequence_index;
	short frame_index;
	real unknown1c;
	real unknown20;
	real point_delay;
	real frame_time;
	real unknown2c;
	short field_x92e145[4];
	long point_indices[4];
};

/* a contrail point (g_4ea940, 0x38 bytes) */
struct s_contrail_point_datum
{
	short salt;
	byte flags;
	char state_index;
	real state_time;
	real state_rate;
	real scale;
	byte unknown10[4];
	s_location location;
	point3f position;
	vector3f velocity;
	long next_index;
};

/* the contrail tag ('cont') */
struct s_contrail_definition
{
	byte unknown00[2];
	union
	{
		word flags;
		struct
		{
			byte flags02;
			byte flags03;
		};
	};
	real point_rate;
	real velocity_lower;
	real velocity_upper;
	real cone_angle;
	real inherited_velocity;
	byte unknown18[0x24 - 0x18];
	real unknown24;
	real unknown28;
	real frame_rate;
	byte unknown30[4];
	long bitmap_index;
	short sequence_first;
	short sequence_count;
	byte unknown3c[0xe8 - 0x3c];
	long state_count;
	struct s_contrail_point_state *states;
};

/* a state of the contrail points (0x40 bytes) */
struct s_contrail_point_state
{
	real duration_lower;
	real duration_upper;
	real transition_lower;
	real transition_upper;
	dword physics_group_tag;
	long physics_tag_index;
	real width;
	byte unknown1c[0x3c - 0x1c];
	dword flags;
};

struct s_contrail_bitmap_sequence
{
	byte unknown00[0x22];
	short frame_count;
	byte unknown24[0x3c - 0x24];
};

struct s_contrail_bitmap
{
	byte unknown00[0x3c];
	long sequence_count;
	s_contrail_bitmap_sequence *sequences;
};

struct s_contrail_object_marker
{
	byte unknown00[8];
	long marker_name;
	byte unknown0c[4];
	long name;
	byte unknown14[4];
};

struct s_contrail_object_definition
{
	byte unknown00[0x98];
	s_contrail_object_marker *markers;
};

struct s_contrail_object_header
{
	byte unknown00[8];
	long *object;
};

struct s_contrail_structure_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_contrail_structure_bsp
{
	byte unknown00[0x30];
	s_contrail_structure_leaf *leaves;
};

void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
void function_11bed0(s_location *location, point3f const *point);
void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle);

/* the leaf and cluster of the structure bsp a point is in */
static inline void contrail_location_from_point(s_location *location, point3f *point)
{
	long cluster_index;

	if (g_4686c4 == NONE)
	{
		location->leaf_index = NONE;
		cluster_index = NONE;
	}
	else
	{
		long leaf_index = function_14a280(g_4e033c, 0, point);

		location->leaf_index = leaf_index;
		cluster_index = leaf_index != NONE ? ((s_contrail_structure_bsp *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
	}
	location->cluster_index = (short)cluster_index;
	location->bsp_index = g_4686c4;
}

void function_17b850(void);
real function_17bef0(dword flags, real scale, real lower, real upper, long bit);
void function_17bf80(s_contrail_datum *contrail);
short function_17c040(long contrail_index, real dt);
void function_17c880(long contrail_index);

#define CONTRAIL(index) ((s_contrail_datum *)g_4ea944->data + ((index) & 0xffff))
#define CONTRAIL_POINT(index) ((s_contrail_point_datum *)g_4ea940->data + ((index) & 0xffff))
#define CONTRAIL_DEFINITION(index) ((s_contrail_definition *)g_4e3b44[(index) & 0xffff].bytes)

static inline void contrails_make_valid(s_record_pool *data)
{
	data->valid = true;
	record_pool_release_all(data);
}

// @retail 0x17b7b0
void function_17b7b0(void)
{
	g_4ea944 = data_new_inlined("contrail", 0x100, sizeof(s_contrail_datum), 0, g_510c2c);
	g_4ea940 = data_new_inlined("contrail point", 0x400, sizeof(s_contrail_point_datum), 0, g_510c2c);
	if (!g_4ea944 || !g_4ea940)
		function_17b850();
}

// @retail 0x17b850
void function_17b850(void)
{
	if (g_4ea940)
	{
		g_4ea940 = 0;
	}
	if (g_4ea944)
	{
		g_4ea944 = 0;
	}
}

// @retail 0x17b870
void function_17b870(void)
{
	contrails_make_valid(g_4ea944);
	contrails_make_valid(g_4ea940);
}

// @retail 0x17b8a0
void function_17b8a0(void)
{
	g_4ea940->valid = false;
	g_4ea944->valid = false;
}

PRIVATE __forceinline long data_scan_17b8c0(s_record_pool *data, long index)
{
	if (index >= 0)
	{
		long count = data->high_water_index;
		if (index < count)
		{
			dword const *bits = data->bitmap;
			do
			{
				if (bits[index >> 5] & (1 << (index & 31)))
					return index;
				++index;
			} while (index < count);
		}
	}
	return NONE;
}

PRIVATE __forceinline s_contrail_datum *data_step_17b8c0(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = data_scan_17b8c0(data, iterator->index + 1);
	if (index == NONE)
		return 0;
	s_contrail_datum *result = (s_contrail_datum *)(data->data + data->size * index);
	iterator->index = index;
	iterator->datum_index = (result->salt << 16) | index;
	return result;
}

// @retail 0x17b8c0
void contrails_delete_all(void)
{
	struct
	{
		s_contrail_datum *current;
		s_record_pool_iterator cursor;
	} iterator;
	iterator.cursor.data = g_4ea944;
	iterator.cursor.index = NONE;

	for (;;)
	{
		iterator.current = data_step_17b8c0(&iterator.cursor);
		if (!iterator.current)
			break;

		if (iterator.current->object_index != NONE)
		{
			function_17c880(iterator.cursor.datum_index);
		}
		else
		{
			function_17c880(iterator.cursor.datum_index);
			record_pool_release(iterator.cursor.data, iterator.cursor.datum_index);
		}
	}
}

// @retail 0x17b940
void contrails_update_locations(void)
{
	long contrail_index = data_datum_index(g_4ea944, function_16bc00(g_4ea944, 0));

	while (contrail_index != NONE)
	{
		s_contrail_datum *contrail = CONTRAIL(contrail_index);

		for (long i = 0; i < 4; i++)
		{
			for (long point_index = contrail->point_indices[i]; point_index != NONE; )
			{
				s_contrail_point_datum *point = CONTRAIL_POINT(point_index);

				if (point->location.cluster_index != NONE)
				{
					if (g_4686c4 == NONE)
					{
						point->location.leaf_index = NONE;
						point->location.cluster_index = NONE;
					}
					else
					{
						long leaf_index = function_14a280(g_4e033c, 0, &point->position);

						point->location.leaf_index = leaf_index;
						point->location.cluster_index = leaf_index != NONE ? ((s_contrail_structure_bsp *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
					}
					point->location.bsp_index = g_4686c4;
				}
				point_index = point->next_index;
			}
		}
		contrail_index = data_datum_index(g_4ea944, data_find_index(g_4ea944, contrail_index == NONE ? 0 : (contrail_index & 0xffff) + 1));
	}
}

// @retail 0x17bab0
long function_17bab0(long object_index, short marker_index, long tag_index)
{
	long contrail_index = NONE;

	if (tag_index != NONE && CONTRAIL_DEFINITION(tag_index)->bitmap_index != NONE)
	{
		contrail_index = record_pool_allocate(g_4ea944);
		if (contrail_index != NONE)
		{
			s_contrail_datum *contrail = CONTRAIL(contrail_index);
			long *object = ((s_contrail_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			s_contrail_object_marker *marker = &((s_contrail_object_definition *)g_4e3b44[*object & 0xffff].bytes)->markers[marker_index];

			*(word *)((byte *)contrail + 2) = 0;
			contrail->tag_index = tag_index;
			contrail->object_index = object_index;
			contrail->marker_index = marker_index;
			contrail->marker_name = marker->name;
			contrail->sequence_index = NONE;
			function_17bf80(contrail);
			contrail->unknown1c = 0.0f;
			contrail->unknown20 = 0.0f;
			for (long i = 0; i < 4; i++)
			{
				contrail->field_x92e145[i] = 0;
				contrail->point_indices[i] = NONE;
			}
			if (function_bab40(contrail->object_index, contrail->marker_name, &contrail->scale))
			{
				contrail->flag0 = true;
				function_17c0e0(contrail_index, 1, true);
			}
		}
	}
	return contrail_index;
}

// @retail 0x17bbd0
void contrail_update(long contrail_index, bool detach, real dt)
{
	s_contrail_datum *contrail = CONTRAIL(contrail_index);

	if (detach)
	{
		contrail->flag0 = false;
		contrail->object_index = NONE;
	}
	if (contrail->flag0)
	{
		short count = function_17c040(contrail_index, dt);

		function_17c0e0(contrail_index, count < 1 ? 1 : count, false);
	}
	contrail->unknown2c += dt;
}

// @retail 0x17bc40
void function_17bc40(real dt)
{
	long contrail_index = data_datum_index(g_4ea944, function_16bc00(g_4ea944, 0));

	while (contrail_index != NONE)
	{
		s_contrail_datum *contrail = CONTRAIL(contrail_index);
		real elapsed = dt - contrail->unknown2c;
		s_contrail_definition *definition = CONTRAIL_DEFINITION(contrail->tag_index);

		contrail->unknown2c = 0.0f;
		if (contrail->object_index != NONE)
		{
			bool active = function_bab40(contrail->object_index, contrail->marker_name, &contrail->scale);

			if (active != contrail->flag0)
			{
				real scale = contrail->scale;

				contrail->scale = 0.0f;
				function_17c0e0(contrail_index, 1, true);
				contrail->scale = scale;
			}
			if (active)
				contrail->flag0 = true;
			else
				contrail->flag0 = false;
			if (active)
				function_17c0e0(contrail_index, function_17c040(contrail_index, dt), true);
		}
		if (elapsed > 0.0f && (definition->unknown24 > 0.0f || definition->unknown28 > 0.0f))
		{
			real remaining = elapsed;
			real rate = definition->frame_rate;

			if (rate > 0.0f)
			{
				if (definition->flags02 & 0x20)
					rate = contrail->scale * rate;
				do
				{
					real step = 1.0f / rate - contrail->frame_time;

					if (!(remaining >= step))
					{
						contrail->frame_time += remaining;
						break;
					}
					function_17bf80(contrail);
					remaining -= step;
				} while (remaining > 0.0f);
			}
			else
			{
				contrail->frame_time += elapsed;
			}

			real a = definition->unknown24;
			if (definition->flags03 & 1)
				a = contrail->scale * a;
			contrail->unknown1c -= a * elapsed;

			real b = definition->unknown28;
			if (definition->flags03 & 2)
				b = contrail->scale * b;
			contrail->unknown20 += b * elapsed;
		}
		function_17c540(contrail_index, dt);

		short i;
		for (i = 0; i < 4; i++)
		{
			if (contrail->point_indices[i] != NONE)
				break;
		}
		if (i == 4 && contrail->object_index == NONE)
		{
			function_17c880(contrail_index);
			record_pool_release(g_4ea944, contrail_index);
		}
		contrail_index = data_datum_index(g_4ea944, data_find_index(g_4ea944, contrail_index == NONE ? 0 : (contrail_index & 0xffff) + 1));
	}
}

__declspec(noinline) real function_17bef0(dword flags, real scale, real lower, real upper, long bit);

// @retail 0x17bef0
real function_17bef0(dword flags, real scale, real lower, real upper, long bit)
{
	/* only the low byte of the bit index is read, once */
	byte b = (byte)*(long const *)&bit;
	real base = lower;
	if (flags & (1 << b))
		base = scale * lower;
	real range = upper - lower;
	if (flags & (1 << (byte)(b + 1)))
		range = range * scale;
	return base + range * ((real)_random(&g_4e7408->seed, __FILE__, __LINE__) * (1.f / 65535.f));
}

// @retail 0x17bf80
void function_17bf80(s_contrail_datum *contrail)
{
	s_contrail_definition *definition = CONTRAIL_DEFINITION(contrail->tag_index);

	contrail->frame_time = 0.0f;
	if (definition->bitmap_index != NONE)
	{
		s_contrail_bitmap *bitmap = (s_contrail_bitmap *)g_4e3b44[definition->bitmap_index & 0xffff].bytes;
		short sequence_index;
		short frame_index;

		contrail->frame_index++;
		sequence_index = contrail->sequence_index;
		frame_index = contrail->frame_index;
		if (sequence_index < 0 || sequence_index >= bitmap->sequence_count || frame_index < 0 || frame_index >= bitmap->sequences[sequence_index].frame_count)
		{
			short first = definition->sequence_first;
			short last = first + definition->sequence_count;

			contrail->sequence_index = (short)(first + (((last - first) * random_next(&g_4e7408->seed)) >> 16));
			contrail->frame_index = 0;
		}
	}
	else
	{
		contrail->frame_index = 0;
		contrail->sequence_index = 0;
	}
}

// @retail 0x17c040
short function_17c040(long contrail_index, real dt)
{
	s_contrail_datum *contrail = CONTRAIL(contrail_index);
	s_contrail_definition *definition = CONTRAIL_DEFINITION(contrail->tag_index);
	real rate = definition->point_rate;
	short count = 0;

	if (definition->flags02 & 1)
		rate = contrail->scale * rate;

	real interval = 1.0f / rate;

	while (dt != 0.0f)
	{
		if (dt >= contrail->point_delay)
		{
			dt -= contrail->point_delay;
			count++;
			contrail->point_delay = interval;
		}
		else
		{
			contrail->point_delay -= dt;
			break;
		}
	}
	return count;
}

static inline real contrail_scale_if_17c0e0(word flags, word mask, real scale, real value)
{
	if (flags & mask)
		value = scale * value;
	return value;
}

// @retail 0x17c0e0
void function_17c0e0(long contrail_index, short count, bool force)
{
	s_contrail_datum *contrail = CONTRAIL(contrail_index);
	s_contrail_definition *definition = CONTRAIL_DEFINITION(contrail->tag_index);

	if (count)
	{
		s_object_marker markers[4];
		long marker_name = ((s_contrail_object_definition *)g_4e3b44[*((s_contrail_object_header *)g_4e0300->data)[contrail->object_index & 0xffff].object & 0xffff].bytes)->markers[contrail->marker_index].marker_name;
		short marker_count = function_b8d30(contrail->object_index, marker_name, markers, 4, false);

		if (marker_count > 0)
		{
			real speed = function_17bef0(definition->flags, contrail->scale, definition->velocity_lower, definition->velocity_upper, 1);
			real cone_angle = contrail_scale_if_17c0e0(definition->flags, 8, contrail->scale, definition->cone_angle);
			real inherited_velocity = contrail_scale_if_17c0e0(definition->flags, 0x10, contrail->scale, definition->inherited_velocity);

			for (short i = 0; i < marker_count; i++)
			{
				s_object_marker *marker = &markers[i];
				s_contrail_point_datum *previous;
				short point_count;

				if (contrail->point_indices[i] != NONE)
				{
					previous = CONTRAIL_POINT(contrail->point_indices[i]);
					point_count = count;
				}
				else
				{
					previous = 0;
					point_count = 1;
				}
				if (previous && !memcmp(&marker->matrix.position, &previous->position, sizeof(point3f)) && !force)
					continue;

				for (short j = 1; j <= point_count; j++)
				{
					long point_index = record_pool_allocate(g_4ea940);

					if (point_index != NONE)
					{
						s_contrail_point_datum *point = CONTRAIL_POINT(point_index);
						vector3f direction;
						vector3f velocity;

						point->flags = 3;
						point->state_index = NONE;
						point->state_time = 0.0f;
						point->state_rate = 0.0f;
						point->scale = contrail->scale;
						random_vector_in_cone(&marker->matrix.forward, &direction, &g_4e7408->seed, 0.0f, cone_angle);
						point->position = marker->matrix.position;
						contrail_location_from_point(&point->location, &point->position);
						function_ba1d0(contrail->object_index, &velocity, 0);
						point->velocity.i = inherited_velocity * velocity.i + direction.i * speed;
						point->velocity.j = inherited_velocity * velocity.j + direction.j * speed;
						point->velocity.k = inherited_velocity * velocity.k + direction.k * speed;
						if (j < point_count)
						{
							real t = (real)j / (real)point_count;
							real s = 1.0f - t;
							point3f position;

							point->scale = previous->scale * s + t * point->scale;
							position.x = previous->position.x * s + point->position.x * t;
							position.y = previous->position.y * s + point->position.y * t;
							position.z = previous->position.z * s + point->position.z * t;
							function_11bed0(&point->location, &position);
							point->position = position;
							point->velocity.i = previous->velocity.i * s + point->velocity.i * t;
							point->velocity.j = previous->velocity.j * s + point->velocity.j * t;
							point->velocity.k = previous->velocity.k * s + point->velocity.k * t;
						}
						point->next_index = contrail->point_indices[i];
						contrail->field_x92e145[i]++;
						contrail->point_indices[i] = point_index;
					}
				}
			}
		}
	}
}

// @retail 0x17c540
void __stdcall function_17c540(long contrail_index, real dt)
{
	s_contrail_datum *contrail = CONTRAIL(contrail_index);
	s_contrail_definition *definition = CONTRAIL_DEFINITION(contrail->tag_index);
	long point_list[1024];

	for (short i = 0; i < 4; i++)
	{
		long point_index = contrail->point_indices[i];
		short count = 0;

		while (point_index != NONE)
		{
			s_contrail_point_datum *point = CONTRAIL_POINT(point_index);

			if (!(point->flags & 4))
			{
				point->state_time += point->state_rate * dt;
				while (point->state_rate == 0.0f || point->state_time > 1.0f)
				{
					if (point->flags & 2)
					{
						s_contrail_point_state *state = &definition->states[point->state_index + 1];

						point->state_index++;
						point->state_time = 0.0f;
						real duration = function_17bef0(state->flags, point->scale, state->duration_lower, state->duration_upper, 0);

						point->state_rate = duration;
						if (duration != 0.0f)
							point->state_rate = 1.0f / duration;
						point->flags &= ~2;
					}
					else
					{
						long state_index = point->state_index;

						if (state_index + 1 >= definition->state_count)
						{
							point->flags |= 4;
							break;
						}

						s_contrail_point_state *state = &definition->states[state_index];

						point->state_time = 0.0f;
						real duration = function_17bef0(state->flags, point->scale, state->transition_lower, state->transition_upper, 2);

						point->state_rate = duration;
						if (duration != 0.0f)
							point->state_rate = 1.0f / duration;
						point->flags |= 2;
					}
				}
			}
			if (point->flags & 1)
			{
				point->flags &= ~1;
			}
			else if (!(point->flags & 4))
			{
				s_contrail_point_state *state = &definition->states[point->state_index];

				if (state->physics_tag_index != NONE)
					function_211060(&point->velocity, 0, g_4e3b44[state->physics_tag_index & 0xffff].bytes, &point->location, NONE, &point->position, 0, 0, 0, state->width * 0.5f, dt);
			}
			point_list[count++] = point_index;
			point_index = point->next_index;
		}
		while (count > 1)
		{
			count--;

			long last_index = point_list[count];
			s_contrail_point_datum *last = CONTRAIL_POINT(last_index);
			s_contrail_point_datum *previous = CONTRAIL_POINT(point_list[count - 1]);

			if (!(last->flags & 4) || !(previous->flags & 4) || last->next_index != NONE)
				break;
			previous->next_index = NONE;
			contrail->field_x92e145[i]--;
			record_pool_release(g_4ea940, last_index);
		}
		if (contrail->field_x92e145[i] == 1 && (CONTRAIL_POINT(contrail->point_indices[i])->flags & 4))
		{
			record_pool_release(g_4ea940, contrail->point_indices[i]);
			contrail->point_indices[i] = NONE;
			contrail->field_x92e145[i] = 0;
		}
	}
}

// @retail 0x17c880
void function_17c880(long contrail_index)
{
	s_contrail_datum *contrail = CONTRAIL(contrail_index);
	s_record_pool *points = g_4ea940;

	for (long i = 0; i < 4; i++)
	{
		long point_index = contrail->point_indices[i];

		while (point_index != NONE)
		{
			long next_index = ((s_contrail_point_datum *)points->data + (point_index & 0xffff))->next_index;

			record_pool_release(points, point_index);
			point_index = next_index;
		}
		contrail->field_x92e145[i] = 0;
		contrail->point_indices[i] = NONE;
	}
}
