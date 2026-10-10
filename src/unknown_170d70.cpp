#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "object_queries.h"
#include <math.h>
#include <string.h>
#include <float.h>

// @flags /O2 /arch:SSE /Gr

/* limits read by the range checks below (zero in the retail image; set at run time) */
real g_54e854;
real g_54e858;
real g_54e85c;
real g_54e860;

extern real g_4e9bd0;
short g_468d3c[6] = { 3, 3, 2, 1, 1, 3 };
short g_468d30[6] = { 3, 3, 2, 1, 1, 6 };
real const g_4449c0[6] = { 1500.0f, 1500.0f, 1500.0f, 100000.0f, 100000.0f, 100000.0f };

struct s_motion_channels_1701f0
{
	byte unknown00[8];
	dword flags;
	real target[16];
	byte unknown4c[0x94 - 0x4c];
	byte channel_flags[6];
	byte unknown9a[2];
	real times[6];
	byte unknownb4[0x10c - 0xb4];
	real current[16];
	byte unknown14c[0x180 - 0x14c];
	real first_derivative[13];
	real second_derivative[13];
	real fifth[13];
	real fourth[13];
	real third[13];
	real second[13];
	real first[13];
	real constant[13];
	real delta[13];
	byte unknown354[4];
};

// @retail 0x1701f0
void function_1701f0(long player_index)
{
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + player_index;
	real *out = state->second_derivative;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *time = state->times;
	for (long group = 0; group < 6; ++group, ++time)
	{
		real t = *time - g_4e9bd0;
		if (t > 0.0f)
		{
			real t2 = t * t;
			real t3 = t2 * t;
			for (short i = 0; i < g_468d3c[group]; ++i)
			{
				out[i] = fifth[i] * t3 * 20.0f + fourth[i] * t2 * 12.0f + third[i] * t * 6.0f + second[i] * 2.0f;
				if (out[i] > g_4449c0[group] || 0.0f - g_4449c0[group] > out[i])
				{
					for (long other = 0; other < 6; ++other)
						if (other != group && state->times[other] == *time)
							state->times[other] = 0.0f;
					*time = 0.0f;
				}
			}
		}
		else
			memset(out, 0, g_468d3c[group] * sizeof(real));
		long count = g_468d3c[group];
		out += count;
		fifth += count;
		fourth += count;
		third += count;
		second += count;
	}
}

// @retail 0x1703f0
void function_1703f0(long player_index)
{
	real inverse_dt = (real)(1.0 / g_4e9bd0);
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + player_index;
	real *out = state->first_derivative;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *first = state->first;
	real *delta = state->delta;
	for (long group = 0; group < 6; ++group)
	{
		real t = state->times[group] - g_4e9bd0;
		if (t > 0.0f)
		{
			real t2 = t * t;
			real t3 = t2 * t;
			real t4 = t3 * t;
			for (short i = 0; i < g_468d3c[group]; ++i)
				out[i] = fifth[i] * t4 * 5.0f + fourth[i] * t3 * 4.0f + third[i] * t2 * 3.0f + second[i] * t * 2.0f + first[i];
		}
		else if ((state->flags & 1) && ((state->channel_flags[group] & 2) || (state->flags & 8)))
			memset(out, 0, g_468d3c[group] * sizeof(real));
		else if (state->flags & 1)
		{
			for (short i = 0; i < g_468d3c[group]; ++i)
				out[i] = 0.0f - delta[i] * inverse_dt;
		}
		long count = g_468d3c[group];
		out += count;
		fifth += count;
		fourth += count;
		third += count;
		second += count;
		first += count;
		delta += count;
	}
}

vector3f *matrix4x3_rotation_between(transform4x3f const *a, transform4x3f const *b, vector3f *out);

PRIVATE inline void placement_basis_170c00(vector3f const *forward, vector3f const *up, transform4x3f *matrix)
{
	matrix->scale = 1.0f;
	matrix->forward = *forward;
	matrix->left.i = up->j * forward->k - forward->j * up->k;
	matrix->left.j = forward->i * up->k - up->i * forward->k;
	matrix->left.k = forward->j * up->i - forward->i * up->j;
	matrix->up = *up;
	matrix->position.x = matrix->position.y = matrix->position.z = 0.0f;
}

__declspec(noinline) vector3f *function_170c00(vector3f const *first_forward, vector3f const *first_up, vector3f const *second_forward, vector3f const *second_up, vector3f *out);

// @retail 0x170c00
vector3f *function_170c00(vector3f const *first_forward, vector3f const *first_up, vector3f const *second_forward, vector3f const *second_up, vector3f *out)
{
	transform4x3f first, second;
	placement_basis_170c00(first_forward, first_up, &first);
	placement_basis_170c00(second_forward, second_up, &second);
	return matrix4x3_rotation_between(&first, &second, out);
}

__declspec(noinline) void function_170bb0(real const *first, real const *second, real *out);

// @retail 0x170bb0
void function_170bb0(real const *first, real const *second, real *out)
{
	real const * *first_reference = &first;
	long count = 10;
	do
	{
		*out++ = *(*first_reference)++ - *second++;
	} while (--count);
	count = 1;
	do
	{
		function_170c00((vector3f const *)(*first_reference), (vector3f const *)((*first_reference) + 3),
			(vector3f const *)second, (vector3f const *)(second + 3), (vector3f *)out);
		(*first_reference) += 6;
		second += 6;
		out += 3;
	} while (--count);
}

// @retail 0x170d70
void function_170d70(vector3f const *rotation, vector3f *a, vector3f *b)
{
	vector3f axis = *rotation;
	real angle = normalize_inline(&axis);

	if (angle != 0.f)
	{
		real s = (real)sin(angle);
		real c = (real)cos(angle);
		real one_minus_c = 1.f - c;

		real ka = (a->i * axis.i + axis.k * a->k + axis.j * a->j) * one_minus_c;
		real pa1 = axis.i * a->k - a->i * axis.k;
		real pa2 = a->i * axis.j - axis.i * a->j;
		a->i = a->i * c + ka * axis.i - (axis.k * a->j - axis.j * a->k) * s;
		a->j = ka * axis.j + c * a->j - pa1 * s;
		a->k = ka * axis.k + c * a->k - pa2 * s;

		real kb = (b->k * axis.k + b->j * axis.j + b->i * axis.i) * one_minus_c;
		real pb1 = b->k * axis.i - b->i * axis.k;
		real pb2 = b->i * axis.j - b->j * axis.i;
		b->i = b->i * c + kb * axis.i - (b->j * axis.k - b->k * axis.j) * s;
		b->j = b->j * c + kb * axis.j - pb1 * s;
		b->k = b->k * c + kb * axis.k - pb2 * s;
	}
}

static inline bool real_is_finite(real value)
{
	return (*(long *)&value & 0x7f800000) != 0x7f800000;
}

static inline bool real_in_world_range(real value)
{
	return real_is_finite(value) && value >= -50000.0f && value <= 50000.0f;
}

PRIVATE inline bool motion_near_zero_170630(real value)
{
	return real_is_finite(value) && fabs(value) < 0.001f;
}

PRIVATE inline void motion_normalize_170630(vector3f *vector)
{
	real magnitude = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real inverse = 1.0f / magnitude;
		vector->i *= inverse;
		vector->j *= inverse;
		vector->k *= inverse;
	}
}

// @retail 0x170630
void function_170630(long player_index)
{
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + player_index;
	real values[13];
	real *value = values;
	real *current = state->current;
	real *target = state->target;
	real *velocity = state->first_derivative;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *first = state->first;
	real *constant = state->constant;
	for (short group = 0; group < 6; ++group)
	{
		real t = state->times[group] - g_4e9bd0;
		if (!(t > 0.0f) && (state->flags & 1))
		{
			for (short i = 0; i < g_468d30[group]; ++i)
				current[i] = target[i];
		}
		else
		{
			if (t > 0.0f)
			{
				real t2 = t * t;
				real t3 = t2 * t;
				real t4 = t3 * t;
				real t5 = t4 * t;
				for (short i = 0; i < g_468d3c[group]; ++i)
					value[i] = fifth[i] * t5 + fourth[i] * t4 + third[i] * t3 + second[i] * t2 + first[i] * t + constant[i];
			}
			else
			{
				for (short i = 0; i < g_468d3c[group]; ++i)
					value[i] = 0.0f - velocity[i] * g_4e9bd0;
			}
			if (group < 5)
			{
				for (short i = 0; i < g_468d3c[group]; ++i)
					current[i] = value[i] + current[i];
			}
			else
				function_170d70((vector3f const *)value, (vector3f *)current, (vector3f *)(current + 3));
		}
		current += g_468d30[group];
		target += g_468d30[group];
		long count = g_468d3c[group];
		value += count;
		velocity += count;
		fifth += count;
		fourth += count;
		third += count;
		second += count;
		first += count;
		constant += count;
	}
	vector3f *forward = (vector3f *)(state->current + 10);
	vector3f *up = (vector3f *)(state->current + 13);
	if (!(motion_near_zero_170630(length_sq3f(forward) - 1.0f) &&
		motion_near_zero_170630(length_sq3f(up) - 1.0f) &&
		motion_near_zero_170630(up->k * forward->k + forward->i * up->i + up->j * forward->j)))
	{
		vector3f left;
		left.i = up->j * forward->k - up->k * forward->j;
		left.j = up->k * forward->i - forward->k * up->i;
		left.k = forward->j * up->i - up->j * forward->i;
		up->i = forward->j * left.k - left.j * forward->k;
		up->j = left.i * forward->k - forward->i * left.k;
		up->k = forward->i * left.j - forward->j * left.i;
		motion_normalize_170630(forward);
		motion_normalize_170630(up);
	}
}

// @retail 0x172350
bool function_172350(point3f const *point)
{
	if (real_in_world_range(point->x) && real_in_world_range(point->y) && real_in_world_range(point->z))
		return true;
	return false;
}

// @retail 0x1723e0
bool function_1723e0(real value)
{
	if (real_is_finite(value) && value >= g_54e858 && value <= g_54e85c)
		return true;
	return false;
}

// @retail 0x172420
bool function_172420(real value)
{
	if (real_is_finite(value) && value >= g_45dbd8 && value <= g_54e860)
		return true;
	return false;
}

// @retail 0x172460
bool function_172460(real value)
{
	if (real_is_finite(value) && value >= g_45dbd8 && value <= 3600.0f)
		return true;
	return false;
}

bool function_a74c0(vector3f const *forward, vector3f const *up);
bool function_a7570(vector3f const *vector);

struct s_checked_placement_1724a0
{
	dword flags;
	point3f position;
	point3f target;
	byte unknown1c[8];
	real scale;
	real radius;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	byte unknown50[0x38];
	real duration;
};

// @retail 0x1724a0
bool function_1724a0(s_checked_placement_1724a0 const *placement)
{
	if (placement && (!(placement->flags & 1) ||
		(function_a74c0(&placement->forward, &placement->up) &&
		function_172350(&placement->position) && function_172350(&placement->target) &&
		function_a7570(&placement->velocity) && function_172420(placement->scale) &&
		function_1723e0(placement->radius) && function_172460(placement->duration))))
		return true;
	return false;
}

struct s_observer_command;
struct s_16f460;
void function_16f460(s_16f460 *state);

PRIVATE inline real placement_clamp_172520(real const &value, real minimum, real maximum)
{
	return value < minimum ? minimum : value > maximum ? maximum : value;
}

// @retail 0x172520
void function_172520(s_observer_command *command)
{
	s_checked_placement_1724a0 *placement = (s_checked_placement_1724a0 *)command;
	if (!function_1724a0(placement))
	{
		if (!function_a74c0(&placement->forward, &placement->up))
		{
			placement->forward = *g_4687a8;
			placement->up = *g_4687b0;
		}
		placement->position.x = placement_clamp_172520(placement->position.x, -50000.0f, 50000.0f);
		placement->position.y = placement_clamp_172520(placement->position.y, -50000.0f, 50000.0f);
		placement->position.z = placement_clamp_172520(placement->position.z, -50000.0f, 50000.0f);
		placement->target.x = placement_clamp_172520(placement->target.x, -50000.0f, 50000.0f);
		placement->target.y = placement_clamp_172520(placement->target.y, -50000.0f, 50000.0f);
		placement->target.z = placement_clamp_172520(placement->target.z, -50000.0f, 50000.0f);
		if (!function_a7570(&placement->velocity))
			placement->velocity = *g_4687a4;
		placement->radius = placement_clamp_172520(placement->radius, g_54e858, g_54e85c);
		placement->scale = placement_clamp_172520(placement->scale, 0.0f, g_54e860);
		if (!function_1724a0(placement))
			function_16f460((s_16f460 *)command);
	}
}

/* a table of 512 short lists: a bit per list that is in use, the first value
   of each list and the next value after each value */
struct s_short_list_table
{
	dword used[16];
	short first[512];
	short next[256];
};

PRIVATE __forceinline bool short_list_used_173b00(dword const *used, long index)
{
	return (used[index >> 5] & (1 << (index & 31))) != 0;
}

PRIVATE __forceinline void short_list_mark_used_173b00(dword *used, long index)
{
	used[index >> 5] |= 1 << (index & 31);
}

__declspec(noinline) void function_173b00(s_short_list_table *table, short list_index, short value);

// @retail 0x173b00
void function_173b00(s_short_list_table *table, short list_index, short value)
{
	if (!short_list_used_173b00(table->used, list_index))
	{
		table->next[value] = NONE;
		table->first[list_index] = value;
		short_list_mark_used_173b00(table->used, list_index);
	}
	else
	{
		table->next[value] = table->first[list_index];
		table->first[list_index] = value;
	}
}

struct s_ring_buffer
{
	long unknown00;
	long index;
	short values[256];
};

// @retail 0x173b60
void function_173b60(s_ring_buffer *buffer, short value)
{
	long index = buffer->index++;

	if (buffer->index == 256)
		buffer->index = 0;
	buffer->values[index] = value;
}

struct s_flag_holder
{
	byte unknown00[0x16];
	byte flag16_0 : 1;
	byte flag16_1 : 1;
	byte flag16_2 : 1;
};

// @retail 0x173b80
long function_173b80(s_flag_holder *holder)
{
	return holder->flag16_2;
}

/* ---- the local players ---- */

static inline long local_player_next(long index)
{
	long i = (index == NONE) ? 0 : index + 1;
	long result = NONE;

	for (; i < 4; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x172750
bool function_172750(long mode, point3f const *point, real radius)
{
	real limit = radius * radius;

	for (long i = local_player_next(NONE); i != NONE; i = local_player_next(i))
	{
		s_player_state *player = NULL;
		real distance_squared;

		if (i != NONE && g_4686c4 != NONE)
			player = &g_4e9bd4[i].state;
		real dx = player->position.x - point->x;
		real dy = player->position.y - point->y;
		real dz = player->position.z - point->z;
		distance_squared = dx * dx + dy * dy + dz * dz;
		if (mode == 1)
			distance_squared = player->radius * player->radius * distance_squared;
		if (limit > distance_squared)
			return true;
	}
	return false;
}

/* ---- the rectangle pool ---- */

struct s_rect4
{
	real x0, x1, y0, y1;
};

struct s_polygon_clip_173380
{
	s_rect4 bounds;
	short count;
	short unknown12;
	point2f *points;
};

short function_23a2b0(short count, point2f const *points, short clip_count,
	point2f const *clip, short capacity, point2f *output, real epsilon);

// @retail 0x173380
long function_173380(s_polygon_clip_173380 const *polygon, s_polygon_clip_173380 const *clip, s_polygon_clip_173380 *output)
{
	output->count = function_23a2b0(polygon->count, polygon->points, clip->count,
		clip->points, 64, output->points, 0.0001f);
	if (output->count == NONE)
	{
		output->count = clip->count;
		memcpy(output->points, clip->points, clip->count * sizeof(point2f));
	}
	return output->count >= 3;
}

struct s_pool_entry
{
	long key;
	short next;
	short unknown06;
	short value08;
	short unknown0a;
	s_rect4 rect;
	short count;
	short unknown1e;
	real *data;
	real unknown24;
	long unknown28;
	byte unknown2c[0x40];
};

struct s_pool
{
	long count;
	s_pool_entry entries[256];
	long data_used;
	real data[10240];
};

struct s_pool_source
{
	byte unknown00[0xa0];
	s_rect4 rect;
	byte unknown_b0[0x198 - 0xb0];
	long count;
	real data[1];
};

// @retail 0x172bb0
short function_172bb0(s_pool *pool, s_pool_source *source, short value, long key)
{
	long count = pool->count++;
	short index = (short)count;
	s_pool_entry *entry = &pool->entries[index];

	entry->key = key;
	entry->next = NONE;
	entry->unknown06 = NONE;
	entry->unknown0a = NONE;
	entry->unknown28 = NONE;
	entry->value08 = value;
	entry->rect = source->rect;

	real *data = &pool->data[pool->data_used * 2];
	pool->data_used += source->count;
	entry->data = data;
	memcpy(data, source->data, source->count * 8);
	entry->count = (short)source->count;
	entry->unknown24 = 0.0f;
	memset(entry->unknown2c, 0, 0x40);
	return index;
}

// @retail 0x173350
short function_173350(s_pool *pool, short index, short value)
{
	while (index != NONE)
	{
		s_pool_entry *entry = &pool->entries[index];

		if (entry->unknown06 == value)
			break;
		index = entry->next;
	}
	return index;
}

/* ---- merging the rectangles ---- */

/* lists of pool entries: bit i of the first 16 words says whether list i
   has a head (heads[i]); the entries are chained through next[] */
struct s_pool_lists
{
	dword valid[16];
	short heads[512];
	short next[256];
};

static inline short list_head(s_pool_lists const *lists, short list)
{
	if (lists->valid[list >> 5] & (1 << (list & 31)))
		return lists->heads[list];
	return NONE;
}

static inline short list_next(s_pool_lists const *lists, short list, short index)
{
	short current = lists->heads[list];

	while (current != NONE && current != index)
		current = lists->next[current];
	return lists->next[index];
}

struct s_view_cluster_1733e0
{
	short index;
	short counts[6];
	short heads[6];
};

struct s_view_volume_1733e0
{
	long identifier;
	long cluster_index;
	long plane_index;
	byte unknown0c[0x20 - 0xc];
	real distance;
	byte unknown24[0x108 - 0x24];
};

struct s_view_collection_1733e0
{
	short active;
	byte unknown02[2];
	byte camera[0x1bc];
	byte unknown1c0[0xa6c - 0x1c0];
	short cluster_count;
	s_view_cluster_1733e0 clusters[128];
	byte unknown176e[2];
	long order[128];
	short volume_count;
	byte unknown1972[2];
	s_view_volume_1733e0 volumes[512];
};

struct s_frustum_1648d0;
struct s_camera_163db0;
struct s_16e150_bits;
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result);
void function_16de20(short index, s_16e150_bits const *table, dword *output);
bool __stdcall function_134950(long a, long b, void const *context);
void sort_4byte(long *elements, unsigned long count, void *unused, bool (__stdcall *compare)(long, long, void const *), void const *context);

// @retail 0x1733e0
void function_1733e0(s_view_collection_1733e0 *collection, long cluster_index, byte const *camera)
{
	memcpy(collection->camera, camera, sizeof(collection->camera));
	collection->active = 1;
	collection->cluster_count = 0;
	collection->volume_count = 0;
	if (cluster_index != NONE)
	{
		dword visible[16];
		function_16de20(cluster_index, (s_16e150_bits const *)g_4e0348, visible);
		for (long i = 0; i < g_4e0348->list_count; ++i)
		{
			if ((visible[i >> 5] & (1 << (i & 31))) && collection->cluster_count < 128)
			{
				long index = collection->cluster_count;
				s_view_cluster_1733e0 *cluster = &collection->clusters[index];
				collection->order[index] = index;
				cluster->index = (short)i;
				cluster->counts[0] = 0;
				cluster->heads[0] = NONE;
				++collection->cluster_count;
				short volume_index = collection->volume_count;
				if (volume_index < 512)
				{
					s_view_volume_1733e0 *volume = &collection->volumes[volume_index];
					collection->volume_count = volume_index + 1;
					function_163db0((s_camera_163db0 const *)camera, (box2f const *)(camera + 0xa0), 0, (s_frustum_1648d0 *)volume);
					++cluster->counts[0];
					if (cluster->heads[0] == NONE)
						cluster->heads[0] = volume_index;
				}
			}
		}
		sort_4byte(collection->order, collection->cluster_count, &camera, function_134950, collection);
	}
}

// @retail 0x172ee0
bool function_172ee0(long *cluster_map, s_pool_lists const *lists, s_pool *pool, byte const *cameras,
	long group, s_view_collection_1733e0 *collection, long entry_index, long cluster_index)
{
	long cluster_slot = cluster_map[cluster_index];
	s_view_cluster_1733e0 *cluster;
	if (cluster_slot == NONE)
	{
		short count = collection->cluster_count;
		if (count >= 128)
			return false;
		cluster_slot = count;
		collection->cluster_count = count + 1;
		collection->order[cluster_slot] = cluster_slot;
		cluster_map[cluster_index] = cluster_slot;
		cluster = &collection->clusters[cluster_slot];
		cluster->index = (short)cluster_index;
		memset(cluster->counts, 0, sizeof(cluster->counts));
		memset(cluster->heads, 0xff, sizeof(cluster->heads));
	}
	else
		cluster = &collection->clusters[cluster_slot];
	do
	{
		short volume_index = collection->volume_count;
		if (volume_index >= 512)
			return false;
		s_pool_entry *entry = &pool->entries[entry_index];
		if (entry->unknown28 == NONE)
		{
			s_view_volume_1733e0 *volume = &collection->volumes[volume_index];
			collection->volume_count = volume_index + 1;
			if (function_163db0((s_camera_163db0 const *)(cameras + entry->key * 0x1bc), (box2f const *)&entry->rect, entry->key, (s_frustum_1648d0 *)volume))
			{
				volume->cluster_index = cluster_slot;
				volume->distance = entry->unknown24;
				entry->unknown0a = volume_index;
				short portal_index = entry->unknown06;
				volume->plane_index = portal_index != NONE
					? *(long *)(*(byte **)((byte *)g_4e0348 + 0x60) + portal_index * 0x24 + 4) : NONE;
				if (cluster->heads[group] == NONE)
					cluster->heads[group] = volume_index;
				++cluster->counts[group];
			}
		}
		entry_index = list_next(lists, (short)cluster_index, (short)entry_index);
	} while (entry_index != NONE);
	return true;
}

PRIVATE __forceinline long list_head_index_1730a0(s_pool_lists const *lists, short index)
{
	if (short_list_used_173b00(lists->valid, index))
		return lists->heads[index];
	return NONE;
}

// @retail 0x1730a0
void function_1730a0(s_pool *pool, s_pool_lists const *lists, long *cluster_map,
	byte const *cameras, long group, s_view_collection_1733e0 *collection)
{
	long count = pool->count;
	s_pool_entry *entry = pool->entries;
	for (long index = 0; index < count; ++index, ++entry)
	{
		long cluster_index = entry->value08;
		if (index == (short)list_head_index_1730a0(lists, (short)cluster_index))
		{
			if (!function_172ee0(cluster_map, lists, pool, cameras, group, collection, index, cluster_index))
				break;
		}
	}
}

static inline real real_minimum(real a, real b)
{
	return (a > b) ? b : a;
}

static inline real real_maximum(real a, real b)
{
	return (a > b) ? a : b;
}

static inline void rect4_union(s_rect4 *rect, s_rect4 const *other)
{
	if (rect->x0 > other->x0)
		rect->x0 = other->x0;
	if (!(rect->x1 > other->x1))
		rect->x1 = other->x1;
	if (rect->y0 > other->y0)
		rect->y0 = other->y0;
	if (!(rect->y1 > other->y1))
		rect->y1 = other->y1;
}

static inline real rect4_area(s_rect4 const *rect)
{
	return (rect->y1 - rect->y0) * (rect->x1 - rect->x0);
}

// @retail 0x172c70
void function_172c70(s_pool_lists *lists, s_pool *pool)
{
	for (long list = 0; list < g_4e0348->list_count; list++)
	{
		long index = list_head(lists, (short)list);

		while (index != NONE)
		{
			s_pool_entry *entry = &pool->entries[index];
			long next = list_next(lists, (short)list, (short)index);

			while (next != NONE)
			{
				s_pool_entry *other = &pool->entries[next];
				s_rect4 merged = other->rect;

				rect4_union(&merged, &entry->rect);
				if ((rect4_area(&other->rect) + rect4_area(&entry->rect)) * 1.5f > rect4_area(&merged))
				{
					entry->unknown28 = next;
					other->unknown24 = real_minimum(entry->unknown24, other->unknown24);
					other->rect = merged;
					break;
				}
				next = list_next(lists, (short)list, (short)next);
			}
			index = list_next(lists, (short)list, (short)index);
		}
	}
}

/* ---- the view setup ---- */

struct s_rotation_matrix
{
	real m00, m01, m02;
	real m10, m11, m12;
	real m20, m21, m22;
};

struct s_view_setup
{
	point3f position;
	s_location location;
	byte unknown14[0x20 - 0x14];
	vector3f forward;
	vector3f up;
	real field_of_view;
	real aspect;
	real scale_x;
	real scale_y;
	real deviation;
	real vertical_field_of_view;
	real ratio;
};

static inline void rotation_matrix_from_axis_angle(s_rotation_matrix *m, vector3f const *axis, real angle)
{
	real s = (real)sin(angle);
	real c = (real)cos(angle);
	real i2 = axis->i * axis->i;
	real j2 = axis->j * axis->j;
	real k2 = axis->k * axis->k;
	real one_minus_c = 1.f - c;
	real tij = one_minus_c * axis->i * axis->j;
	real tik = one_minus_c * axis->k * axis->i;
	real tjk = one_minus_c * axis->k * axis->j;

	m->m00 = (1.f - i2) * c + i2;
	m->m01 = tij - s * axis->k;
	m->m10 = tij + s * axis->k;
	m->m11 = (1.f - j2) * c + j2;
	m->m20 = tik - s * axis->j;
	m->m02 = tik + s * axis->j;
	m->m22 = (1.f - k2) * c + k2;
	m->m12 = tjk - s * axis->i;
	m->m21 = tjk + s * axis->i;
}

static inline void rotation_matrix_transform(s_rotation_matrix const *m, vector3f const *in, vector3f *out)
{
	vector3f copy;

	if (in == out)
	{
		copy = *in;
		in = &copy;
	}
	out->i = in->i * m->m00 + m->m02 * in->k + m->m01 * in->j;
	out->j = in->k * m->m12 + m->m11 * in->j + in->i * m->m10;
	out->k = in->j * m->m21 + in->k * m->m22 + in->i * m->m20;
}

// @retail 0x171d90
void function_171d90(s_view_setup *view)
{
	vector3f original = view->forward;
	s_rotation_matrix m;

	view->ratio = view->field_of_view / g_54e854;
	view->vertical_field_of_view = (real)(atan2((double)(tan(view->field_of_view * 0.5f) / view->aspect), 1.0) * 2.0);

	real half = view->vertical_field_of_view * 0.5f;
	real angle_a = (real)atan(tan(half) * view->scale_x);
	real angle_b = (real)atan(tan(half) * view->scale_y);

	rotation_matrix_from_axis_angle(&m, g_4687b0, angle_a);
	rotation_matrix_transform(&m, &view->forward, &view->forward);
	rotation_matrix_transform(&m, &view->up, &view->up);

	vector3f axis;
	axis.i = view->forward.j * view->up.k - view->up.j * view->forward.k;
	axis.j = view->forward.k * view->up.i - view->up.k * view->forward.i;
	axis.k = view->up.j * view->forward.i - view->forward.j * view->up.i;
	rotation_matrix_from_axis_angle(&m, &axis, angle_b);
	rotation_matrix_transform(&m, &view->forward, &view->forward);
	rotation_matrix_transform(&m, &view->up, &view->up);

	real dot = view->forward.j * original.j + view->forward.k * original.k + view->forward.i * original.i;
	real limit = 0.0001f;
	if (!(dot > limit))
		dot = limit;
	real inv = 1.f / dot;
	real dz = original.k * inv - view->forward.k;
	real dy = original.j * inv - view->forward.j;
	real dx = inv * original.i - view->forward.i;
	view->deviation = (real)sqrt(dz * dz + dy * dy + dx * dx);
}

void function_11bed0(s_location *location, point3f const *point);

// @retail 0x1726d0
void function_1726d0(s_view_setup *view, point3f const *position, vector3f const *forward, vector3f const *up)
{
	memset(view, 0, sizeof(*view));
	view->position = *position;
	view->forward = *forward;
	view->up = *up;
	view->field_of_view = g_54e854;
	view->aspect = 1.3333334f;
	function_11bed0(&view->location, position);
	function_171d90(view);
}

struct s_polygon_173910
{
	long field_0;
	long plane_index;
	byte unknown08[0x14];
	long count;
	point3f const *points;
};

struct s_polygon_context_173910
{
	long field_0;
	struct s_planes
	{
		byte unknown00[0xc];
		plane3f const *planes;
	} *geometry;
	struct s_origin
	{
		byte unknown00[0x60];
		point3f position;
	} *origin;
};

short function_120850(vector3f const *v);
bool function_23a220(point2f const *point, short count, point2f const *points, real epsilon);

// @retail 0x173910
bool function_173910(s_polygon_context_173910 const *context, s_polygon_173910 const *polygon)
{
	point2f points[128];
	plane3f const *plane = &context->geometry->planes[polygon->plane_index];
	long axis = function_120850(&plane->n);
	long positive = ((real const *)plane)[axis] > 0.0f ? 1L : 0L;
	point3f const *origin = &context->origin->position;
	real distance = 0.0f - plane_distance_to_point(plane, origin);
	point3f projected;
	projected.x = plane->n.i * distance + origin->x;
	projected.y = distance * plane->n.j + origin->y;
	projected.z = plane->n.k * distance + origin->z;
	short const *axes = g_440b94[axis * 2 + positive];
	point2f point;
	point.x = ((real const *)&projected)[axes[0]];
	point.y = ((real const *)&projected)[axes[1]];
	for (long i = 0; i < polygon->count; ++i)
	{
		points[i].x = ((real const *)&polygon->points[i])[axes[0]];
		points[i].y = ((real const *)&polygon->points[i])[axes[1]];
	}
	return function_23a220(&point, (short)polygon->count, points, 0.0001f) != false;
}

// @retail 0x173890
long function_173890(s_polygon_context_173910 const *context, s_polygon_173910 const *polygon, long state, bool *inside)
{
	plane3f const *plane = &context->geometry->planes[polygon->plane_index];
	point3f const *origin = &context->origin->position;
	real distance = origin->x * plane->i + plane->j * origin->y + plane->k * origin->z - plane->d;
	if (fabs(distance) > 0.015625)
	{
		*inside = false;
		if (state <= 0)
		{
		zero:
			return 0;
		}
		if (state >= 3)
		{
			if (distance < 0.0f)
				return 1;
			return 2;
		}
	}
	else
	{
		*inside = function_173910(context, polygon);
		if (!*inside && state <= 0)
			goto zero;
	}
	return 3;
}

struct s_projected_polygon_173520
{
	long field_0;
	byte state;
	byte unknown05;
	short field_6;
	short field_8;
	short unknown0a;
	real distance;
	box2f bounds;
	short count;
	short unknown22;
	point2f *points;
};

struct s_polygon_cache_173520
{
	struct s_geometry
	{
		byte unknown00[0x60];
		s_polygon_173910 *polygons;
	} *geometry;
	s_polygon_context_173910::s_planes *planes;
	byte *view;
	plane3f field_c_8;
	dword visited[16];
	s_projected_polygon_173520 polygons[512];
	long point_count;
	point2f points[5120];
};

box2f g_5476cc;
box2f *g_4687dc = &g_5476cc;
int __stdcall function_1429d0(transform4x3f const *matrix, long count, point3f const *source, point3f *destination);
long function_11fc80(plane3f const *plane, bool keep_inside, real tolerance, long point_count, point3f const *points, long maximum_count, point3f *out);

// @retail 0x173520
s_projected_polygon_173520 *function_173520(long polygon_index, s_polygon_cache_173520 *cache)
{
	point3f transformed[64];
	point3f clipped[64];
	s_projected_polygon_173520 *result = &cache->polygons[polygon_index];
	dword bit = 1 << (polygon_index & 31);
	long word_index = polygon_index >> 5;
	if (!(cache->visited[word_index] & bit))
	{
		if (5120 - cache->point_count >= 64)
		{
			s_polygon_173910 const *polygon = &cache->geometry->polygons[polygon_index];
			function_1429d0((transform4x3f const *)(cache->view + 4), polygon->count, polygon->points, transformed);
			long count = function_11fc80(&cache->field_c_8, true, 0.0078125f, polygon->count, transformed, 64, clipped);
			bool inside;
			result->state = (byte)function_173890((s_polygon_context_173910 const *)cache, polygon, count, &inside);
			result->field_6 = ((short const *)polygon)[0];
			result->field_8 = ((short const *)polygon)[1];
			result->distance = FLT_MAX;
			box2f &bounds = result->bounds;
			bounds = *g_4687dc;
			result->points = cache->points + cache->point_count;
			cache->point_count += 64;
			result->count = (short)count;
			result->field_0 = *(long const *)((byte const *)polygon + 0x18);
			if (result->state)
			{
				long index = (result->state == 1 ? count - 1 : 0) * (long)sizeof(point2f);
				long volatile step = (result->state == 1 ? -1 : 1) * (long)sizeof(point2f);
				point3f const *local_cb4759 = clipped;
				for (long remaining = count; remaining > 0; --remaining, ++local_cb4759, index += step)
				{
					point2f *point = (point2f *)((byte *)result->points + index);
					real inverse = -1.0f / local_cb4759->z;
					point->x = local_cb4759->x * inverse;
					point->y = local_cb4759->y * inverse;
					if (bounds.x0 > point->x)
						bounds.x0 = point->x;
					if (point->x > bounds.x1)
						bounds.x1 = point->x;
					if (bounds.y0 > point->y)
						bounds.y0 = point->y;
					if (point->y > bounds.y1)
						bounds.y1 = point->y;
					real distance = 0.0f - local_cb4759->z;
					result->distance = result->distance > distance ? distance : result->distance;
				}
				if (result->state == 3)
				{
					if (inside)
					{
						result->distance = (real)(result->distance > 0.015625 ? 0.015625 : result->distance);
						bounds = *(box2f *)(cache->view + 0xa0);
						result->count = *(short *)(cache->view + 0x198);
						memcpy(result->points, cache->view + 0x19c, *(long *)(cache->view + 0x198) * sizeof(point2f));
					}
					else
					{
						bounds.x0 -= 0.00390625f;
						bounds.x1 += 0.00390625f;
						bounds.y0 -= 0.00390625f;
						bounds.y1 += 0.00390625f;
						result->points[0].x = bounds.x0;
						result->points[0].y = bounds.y0;
						result->points[1].x = bounds.x1;
						result->points[1].y = bounds.y0;
						result->points[2].x = bounds.x1;
						result->points[2].y = bounds.y1;
						result->points[3].x = bounds.x0;
						result->points[3].y = bounds.y1;
						result->count = 4;
					}
				}
				if (cache->view[0x84] && result->distance > *(real *)(cache->view + 0x88))
					result->state = 0;
				cache->point_count = (result->points - cache->points) + result->count;
			}
			cache->visited[word_index] |= bit;
		}
		else
			return 0;
	}
	return result;
}


struct s_cluster_portals_172840
{
	byte unknown00[0x8c];
	long portal_count;
	short const *portals;
	byte unknown94[0xb0 - 0x94];
};

void function_11f5f0(box2f *bounds, point2f const *points, long count);

// @retail 0x172840
bool __stdcall function_172840(long key, s_pool *pool, s_short_list_table *lists,
	s_ring_buffer *queue, s_polygon_cache_173520 *cache)
{
	bool result = true;
	bool pool_full = false;
	bool queue_full = false;
	bool projection_full = false;
	while (queue->unknown00 != queue->index)
	{
		long head = queue->unknown00++;
		if (queue->unknown00 == 256)
			queue->unknown00 = 0;
		long index = queue->values[head];
		s_pool_entry *entry = &pool->entries[(short)index];
		word cluster_index = entry->value08;
		s_cluster_portals_172840 const *cluster =
			&(*(s_cluster_portals_172840 **)((byte *)g_4e0348 + 0xa0))[(short)cluster_index];
		for (long portal = 0; portal < cluster->portal_count; ++portal)
		{
			long portal_index = cluster->portals[portal];
			s_projected_polygon_173520 *polygon = function_173520(portal_index, cache);
			if (!polygon)
			{
				projection_full = true;
				continue;
			}
			dword bit = 1 << (portal_index & 31);
			long word_index = portal_index >> 5;
			if (((dword *)entry->unknown2c)[word_index] & bit)
				continue;
			if (!(((dword *)g_4f93a4)[word_index] & bit))
				continue;
			if ((polygon->state == 1 && (polygon->field_0 & 2)) ||
				(polygon->state == 2 && (polygon->field_0 & 0x10)))
				continue;
			if (!((polygon->state == 1 && polygon->field_6 == (short)cluster_index) ||
				(polygon->state == 2 && polygon->field_8 == (short)cluster_index) ||
				(polygon->state == 3 && function_173350(pool, (short)index, (short)portal_index) == NONE)))
				continue;
			if (!(polygon->bounds.x1 >= entry->rect.x0 && entry->rect.x1 >= polygon->bounds.x0 &&
				polygon->bounds.y1 >= entry->rect.y0 && entry->rect.y1 >= polygon->bounds.y0))
				continue;
			long new_index = pool->count;
			if (new_index == 256 || 5120 - pool->data_used < 64)
			{
				pool_full = true;
				continue;
			}
			++pool->count;
			s_pool_entry *next = &pool->entries[(short)new_index];
			next->data = (real *)((point2f *)pool->data + pool->data_used);
			pool->data_used += 64;
			if ((byte)function_173380((s_polygon_clip_173380 *)&polygon->bounds,
				(s_polygon_clip_173380 *)&entry->rect, (s_polygon_clip_173380 *)&next->rect))
			{
				short other_cluster = polygon->field_6;
				if (other_cluster == (short)cluster_index)
					other_cluster = polygon->field_8;
				next->key = key;
				next->next = (short)index;
				next->value08 = other_cluster;
				next->unknown06 = (short)portal_index;
				next->unknown28 = NONE;
				next->unknown0a = NONE;
				next->rect = *(s_rect4 *)g_4687dc;
				next->unknown24 = polygon->distance;
				memcpy(next->unknown2c, entry->unknown2c, sizeof(next->unknown2c));
				((dword *)next->unknown2c)[word_index] |= bit;
				pool->data_used = ((point2f *)next->data - (point2f *)pool->data) + next->count;
				function_11f5f0((box2f *)&next->rect, (point2f *)next->data, next->count);
				function_173b00(lists, other_cluster, (short)new_index);
				if ((queue->index + 1) % 256 != queue->unknown00)
					function_173b60(queue, (short)new_index);
				else
					queue_full = true;
			}
			else
			{
				pool->data_used = (point2f *)next->data - (point2f *)pool->data;
				--pool->count;
			}
		}
	}
	if (pool_full)
		result = false;
	if (projection_full)
		result = false;
	if (queue_full)
		return false;
	return result;
}

// @retail 0x173130
void __stdcall function_173130(long camera_count, byte *cameras, long cluster_index,
	s_view_collection_1733e0 *collection)
{
	s_pool pool;
	s_polygon_cache_173520 cache;
	long cluster_map[512];
	s_short_list_table lists;
	s_ring_buffer queue;
	collection->active = (short)camera_count;
	if (cameras != collection->camera)
		memcpy(collection->camera, cameras, camera_count * 0x1bc);
	collection->cluster_count = 0;
	collection->volume_count = 0;
	if (cluster_index != NONE)
	{
		memset(cluster_map, 0xff, sizeof(cluster_map));
		byte *geometry = (byte *)g_4e0348;
		byte *camera = cameras;
		for (long group = 0; group < camera_count; ++group, camera += 0x1bc)
		{
			cache.geometry = (s_polygon_cache_173520::s_geometry *)geometry;
			cache.planes = *(s_polygon_context_173910::s_planes **)(geometry + 0x18);
			cache.view = camera;
			memset(cache.visited, 0, ((*(long *)(geometry + 0x5c) + 31) >> 5) * sizeof(dword));
			memset(lists.used, 0, ((*(long *)(geometry + 0x9c) + 31) >> 5) * sizeof(dword));
			pool.count = 0;
			pool.data_used = 0;
			cache.point_count = 0;
			cache.field_c_8.i = 0.0f;
			cache.field_c_8.j = 0.0f;
			cache.field_c_8.k = -1.0f;
			cache.field_c_8.d = 0.0f;
			queue.unknown00 = 0;
			short index = function_172bb0(&pool, (s_pool_source *)camera, (short)cluster_index, group);
			function_173b00(&lists, (short)cluster_index, index);
			queue.index = 1;
			queue.values[0] = index;
			function_172840(group, &pool, &lists, &queue, &cache);
			function_172c70((s_pool_lists *)&lists, &pool);
			function_1730a0(&pool, (s_pool_lists *)&lists, cluster_map, cameras, group, collection);
		}
	}
	sort_4byte(collection->order, collection->cluster_count, &camera_count, function_134950, collection);
}

struct s_segment_collision
{
    long kind;
    real fraction;
    point3f point;
    byte field_14[0x24 - 0x14];
    short material;
    byte field_26[0x5c - 0x26];
};
struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

// @retail 0x171d00
bool function_171d00(point3f const *start, point3f const *end, real *fraction, bool narrow)
{
    bool result = false;
    (void)&fraction;
    s_segment_collision collision;
    collision.material = NONE;
    long flags = 0x808c0f;
    if (narrow) flags = 0x808c0d;
    vector3f direction;
    direction.i = end->x - start->x;
    direction.j = end->y - start->y;
    direction.k = end->z - start->z;
    if (function_1697c0(flags, start, &direction, NONE, NONE,
        (s_collision_result_1697c0 *)&collision))
    {
        *fraction = collision.fraction;
        result = true;
    }
    return result;
}

struct s_camera_cluster_171830
{
	byte field_0[0x70];
	byte reference;
	byte field_71[0xb0 - 0x71];
};

struct s_camera_reference_171830
{
	short field_0;
	short index;
	plane3f plane;
	byte field_14[4];
};

// @retail 0x171830
void function_171830(point3f const *point, vector3f const *forward,
	vector3f const *up, real *distance, real offset)
{
	(void)&point;
	(void)&distance;
	(void)&offset;
	real center_fraction = 1.0f;
	s_location location;
	function_11bed0(&location, point);
	bool narrow = false;
	if (location.cluster_index != NONE)
	{
		byte *geometry = (byte *)g_4e0348;
		s_camera_cluster_171830 *clusters = *(s_camera_cluster_171830 **)(geometry + 0xa0);
		byte reference = clusters[location.cluster_index].reference;
		if (reference != 0xff)
		{
			s_camera_reference_171830 *entry =
				&(*(s_camera_reference_171830 **)(geometry + 0x68))[reference & 0x7f];
			if (entry->index != NONE)
			{
				if (!(reference & 0x80) ||
					entry->plane.k * point->z + entry->plane.j * point->y + point->x * entry->plane.i - entry->plane.d < 0.0f)
					narrow = true;
			}
		}
	}
	real depth = *distance + offset;
	point3f center;
	center.x = point->x + (0.0f - depth * up->i);
	center.y = point->y + (0.0f - depth * up->j);
	center.z = point->z + (0.0f - depth * up->k);
	function_171d00(point, &center, &center_fraction, narrow);
	vector3f side;
	side.i = forward->j * up->k - up->j * forward->k;
	side.j = up->i * forward->k - forward->i * up->k;
	side.k = up->j * forward->i - forward->j * up->i;
	real best_fraction = center_fraction;
	real radius = *distance * 0.174f;
	vector3f offsets[2];
	offsets[0] = *forward;
	offsets[0].i *= radius;
	offsets[0].j *= radius;
	offsets[0].k *= radius;
	offsets[1].i = side.i * radius;
	offsets[1].j = side.j * radius;
	offsets[1].k = side.k * radius;
	vector3f const *best_offset = NULL;
	real best_sign;
	for (short sample = 0; sample < 4; ++sample)
	{
		s_segment_collision collision;
		collision.material = NONE;
		real sign = (real)((sample & 2) ? 1 : -1);
		vector3f const *displacement = &offsets[sample & 1];
		vector3f direction;
		direction.i = displacement->i * sign + center.x - point->x;
		direction.j = displacement->j * sign + center.y - point->y;
		direction.k = displacement->k * sign + center.z - point->z;
		long flags = 0x808c0f;
		if (narrow) flags = 0x808c0d;
		if (function_1697c0(flags, point, &direction, NONE, NONE,
			(s_collision_result_1697c0 *)&collision) && collision.fraction < best_fraction)
		{
			best_fraction = collision.fraction;
			best_offset = displacement;
			best_sign = sign;
		}
	}
	real fraction;
	if (best_offset)
	{
		real lower = 0.0f;
		real upper = best_sign;
		real lower_fraction = center_fraction;
		real upper_fraction = best_fraction;
		for (long remaining = 10; remaining; --remaining)
		{
			real middle = (upper + lower) * 0.5f;
			bool hit = false;
			s_segment_collision collision;
			collision.material = NONE;
			long flags = 0x808c0f;
			if (narrow) flags = 0x808c0d;
			vector3f direction;
			direction.i = best_offset->i * middle + center.x - point->x;
			direction.j = best_offset->j * middle + center.y - point->y;
			direction.k = best_offset->k * middle + center.z - point->z;
			real found_fraction;
			if (function_1697c0(flags, point, &direction, NONE, NONE,
				(s_collision_result_1697c0 *)&collision))
			{
				found_fraction = collision.fraction;
				hit = true;
				if (fabs(found_fraction - upper_fraction) < 0.1f)
				{
					upper = middle;
					upper_fraction = found_fraction;
					continue;
				}
			}
			lower = middle;
			lower_fraction = hit ? found_fraction : 1.0f;
		}
		real direction;
		if (upper_fraction > lower_fraction)
			direction = lower;
		else
			direction = (real)(upper >= 0.0f);
		real blend = upper_fraction > lower_fraction ? lower : upper;
		if (direction != 0.0f)
			blend = 0.0f - blend;
		fraction = (1.0f - blend) * best_fraction + blend * center_fraction;
	}
	else
	{
		fraction = center_fraction;
	}
	*distance *= fraction;
}

void function_3f500(long cluster_index);
double g_45e4c8 = -3.4028234663852886e+38;

// @retail 0x170fd0
void __stdcall function_170fd0(long user_index)
{
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + user_index;
	s_view_setup *view = (s_view_setup *)((byte *)state + 0xb8);
	point3f position = *(point3f *)&state->current[0];
	real distance;
	if (0.0f > state->current[8]) distance = 0.0f;
	else if (state->current[8] > 3.4028235e38f) distance = 3.4028235e38f;
	else distance = state->current[8];
	vector3f *forward = (vector3f *)&state->current[10];
	vector3f *up = (vector3f *)&state->current[13];
	if (!function_a74c0(forward, up))
	{
		forward->i = g_4687a8->i;
		forward->j = g_4687a8->j;
		forward->k = g_4687a8->k;
		up->i = g_4687b0->i;
		up->j = g_4687b0->j;
		up->k = g_4687b0->k;
	}
	real field_of_view = state->current[9];
	if (g_54e858 > field_of_view) field_of_view = g_54e858;
	else if (field_of_view > g_54e85c) field_of_view = g_54e85c;
	state->current[9] = field_of_view;
	if (-50000.0f > position.x) position.x = -50000.0f;
	else if (position.x > 50000.0f) position.x = 50000.0f;
	if (-50000.0f > position.y) position.y = -50000.0f;
	else if (position.y > 50000.0f) position.y = 50000.0f;
	if (-50000.0f > position.z) position.z = -50000.0f;
	else if (position.z > 50000.0f) position.z = 50000.0f;
	if (0.0f > distance) distance = 0.0f;
	else if (distance > g_54e860) distance = g_54e860;
	real direction_x = forward->i;
	real direction_y = forward->j;
	real magnitude = (real)sqrt(direction_y * direction_y + direction_x * direction_x);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real inverse = 1.0f / magnitude;
		direction_x *= inverse;
		direction_y *= inverse;
	}
	position.x += state->current[4] * direction_y + state->current[3] * direction_x;
	position.y = state->current[3] * direction_y - state->current[4] * direction_x + position.y;
	position.z = state->current[5] + position.z;
	view->forward.i = forward->i;
	view->forward.j = forward->j;
	view->forward.k = forward->k;
	view->up.i = up->i;
	view->up.j = up->j;
	view->up.k = up->k;
	vector3f *offset = (vector3f *)((byte *)state + 0xcc);
	offset->i = 0.0f - state->first_derivative[0];
	offset->j = 0.0f - state->first_derivative[1];
	offset->k = 0.0f - state->first_derivative[2];
	view->field_of_view = field_of_view;
	view->aspect = 1.33333337f;
	view->scale_x = state->current[6];
	view->scale_y = state->current[7];
	function_171d90(view);
	if (*((byte *)state + 0xb6))
	{
		transform4x3f const *matrix = (transform4x3f *)((byte *)state + 0x14c);
		point3f scaled = position;
		if (matrix->scale != 1.0f)
		{
			scaled.x *= matrix->scale;
			scaled.y *= matrix->scale;
			scaled.z *= matrix->scale;
		}
		position.x = matrix->up.i * scaled.z + matrix->left.i * scaled.y + matrix->forward.i * scaled.x + matrix->position.x;
		position.y = matrix->up.j * scaled.z + matrix->left.j * scaled.y + matrix->forward.j * scaled.x + matrix->position.y;
		position.z = matrix->up.k * scaled.z + matrix->left.k * scaled.y + matrix->forward.k * scaled.x + matrix->position.z;
		vector3f source;
		source.i = offset->i; source.j = offset->j; source.k = offset->k;
		if (matrix->scale != 1.0f)
		{
			source.i *= matrix->scale; source.j *= matrix->scale; source.k *= matrix->scale;
		}
		offset->i = matrix->up.i * source.k + matrix->left.i * source.j + matrix->forward.i * source.i;
		offset->j = matrix->up.j * source.k + matrix->left.j * source.j + matrix->forward.j * source.i;
		offset->k = matrix->up.k * source.k + matrix->left.k * source.j + matrix->forward.k * source.i;
		source.i = view->forward.i; source.j = view->forward.j; source.k = view->forward.k;
		if (matrix->scale != 1.0f)
		{
			source.i *= matrix->scale; source.j *= matrix->scale; source.k *= matrix->scale;
		}
		view->forward.i = matrix->up.i * source.k + matrix->left.i * source.j + matrix->forward.i * source.i;
		view->forward.j = matrix->up.j * source.k + matrix->left.j * source.j + matrix->forward.j * source.i;
		view->forward.k = matrix->up.k * source.k + matrix->left.k * source.j + matrix->forward.k * source.i;
		source.i = view->up.i; source.j = view->up.j; source.k = view->up.k;
		if (matrix->scale != 1.0f)
		{
			source.i *= matrix->scale; source.j *= matrix->scale; source.k *= matrix->scale;
		}
		view->up.i = matrix->up.i * source.k + matrix->left.i * source.j + matrix->forward.i * source.i;
		view->up.j = matrix->up.j * source.k + matrix->left.j * source.j + matrix->forward.j * source.i;
		view->up.k = matrix->up.k * source.k + matrix->left.k * source.j + matrix->forward.k * source.i;
		vector3f const *translation = (vector3f const *)((byte *)state + 0x4c);
		offset->i += translation->i;
		offset->j += translation->j;
		offset->k += translation->k;
	}
	if (!(state->flags & 0x10) && distance == 0.0f)
		function_171830(&position, &view->forward, &view->up, &distance, 0.02f);
	view->position.x = position.x - view->forward.i * distance;
	view->position.y = position.y - view->forward.j * distance;
	view->position.z = position.z - view->forward.k * distance;
	if (-50000.0f > view->position.x) view->position.x = -50000.0f;
	else if (view->position.x > 50000.0f) view->position.x = 50000.0f;
	if (-50000.0f > view->position.y) view->position.y = -50000.0f;
	else if (view->position.y > 50000.0f) view->position.y = 50000.0f;
	if (-50000.0f > view->position.z) view->position.z = -50000.0f;
	else if (view->position.z > 50000.0f) view->position.z = 50000.0f;
	s_location location;
	function_11bed0(&location, &view->position);
	if (location.cluster_index != NONE)
	{
		if (location.cluster_index != view->location.cluster_index && location.cluster_index >= 0 &&
			location.cluster_index < *(long *)((byte *)g_4e0348 + 0x9c))
			function_3f500(location.cluster_index);
		view->location = location;
	}
	long player_index = user_index == NONE ? NONE : g_4e8c20->entries[user_index];
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	if (*(long *)(player + 0x2c) == NONE && !(player[2] & 8) && *g_510c70 == g_4686c4)
	{
		long cluster_index = g_510c70[user_index + 1];
		if (cluster_index >= 0 && cluster_index < *(long *)((byte *)g_4e0348 + 0x9c))
			function_3f500(cluster_index);
	}
	if (fabs(g_45e4c8) < 0.05f)
		view->position.z -= 3.4028235e38f;
}
