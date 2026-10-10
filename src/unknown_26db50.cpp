// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_26c380.h"
#include "slot_handler.h"
#include "unknown_20fe20.h"
#include "unknown_11cc90.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_2626b0.h"
#include "object_markers.h"

/* Location records share one owner and type, with a bitmap of active entries. */
struct s_location_record_view
{
	short salt;
	short type;
	long owner;
	short count;
	short users;
	s_type_c3b527 point;
	vector3f up;
	vector3f forward;
	long location_index;
	bool valid;
	byte unknown39[3];
	long time;
	dword active[1];
	s_262b40_result entries[32];
	byte unknown444[0x40];
};

struct s_location_record_actor_view
{
	byte unknown000[0x3f4];
	long record_index;
	long owner_index;
	short update_count;
	byte unknown3fe[0x888 - 0x3fe];
};

void function_26bfa0(long object_index, long *location_index, s_location_view *location);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
void function_1e22d0(long actor_index, bool conditional);

// @retail 0x26dc20
void function_26dc20(long actor_index)
{
	s_location_record_actor_view *actor = (s_location_record_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_location_record_actor_view));
	long record_index = actor->record_index;
	function_1e22d0(actor_index, true);
	actor->record_index = NONE;
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	if (--record->users == 0)
		record_pool_release(g_51eca4, record_index);
}

// @retail 0x26def0
void function_26def0(long actor_index, long owner_index)
{
	s_location_record_actor_view *actor = (s_location_record_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_location_record_actor_view));
	if (actor->owner_index != owner_index)
	{
		if (actor->record_index != NONE)
			function_26dc20(actor_index);
		actor->owner_index = owner_index;
		actor->update_count = 0;
	}
}

// @retail 0x26d3f0
long function_26d3f0(long object_index, short type)
{
	long const volatile *object_reference = &object_index;
	long location_index = NONE;
	s_type_c3b527 location;
	vector3f up, forward;
	function_26bfa0(object_index, &location_index, (s_location_view *)&location);
	if (location_index != NONE)
	{
		long result = record_pool_allocate(g_51eca4);
		if (result != NONE)
		{
			s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (result & 0xffff) * sizeof(s_location_record_view));
			record->owner = object_index;
			record->count = 0;
			record->users = 0;
			*(volatile short *)&record->type = type;
			record->active[0] = 0;
			function_b9fc0(*object_reference, &forward, &up);
			record->point = location;
			record->location_index = location_index;
			record->up = up;
			record->forward = forward;
			record->valid = true;
			record->time = g_510c54->game_time;
		}
		return result;
	}
	return NONE;
}

// @retail 0x26d500
long function_26d500(long object_index)
{
	byte *tag = g_4e3b44[object_get(object_index)->tag_index & 0xffff].bytes;
	long result = NONE;
	if (*(long *)(tag + 0x5c) > 0 && (**(byte **)(tag + 0x60) & 4))
	{
		result = function_26d3f0(object_index, 1);
		if (result != NONE)
		{
			s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (result & 0xffff) * sizeof(s_location_record_view));
			record->count = 32;
		}
	}
	return result;
}

PRIVATE inline real record_dot3f(vector3f const *a, vector3f const *b)
{
	return a->i * b->i + a->j * b->j + a->k * b->k;
}

// @retail 0x26dc90
bool function_26dc90(long record_index)
{
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	bool result;
	if (g_510c54->game_time > record->time)
	{
		long owner = record->owner;
		record->valid = false;
		if (owner != NONE)
		{
			vector3f up, forward;
			function_b9fc0(owner, &forward, &up);
			if (record->type != 1 || (!(record_dot3f(&record->up, &up) < 0.9f) && !(record_dot3f(&record->forward, &forward) < 0.99f)))
			{
				real threshold = record->type == 1 ? 0.002500000176951289f : 0.04000000283122063f;
				long location_index;
				s_type_c3b527 location;
				function_26bfa0(owner, &location_index, (s_location_view *)&location);
				if (location_index != NONE && function_210a30(&location, &record->point) <= threshold)
				{
					record->valid = true;
					result = true;
					goto update_time;
				}
			}
		}
		result = false;
update_time:
		record->time = g_510c54->game_time;
		return result;
	}
	return record->valid;
}

struct s_record_motion_view
{
	byte unknown00[0x48];
	vector3f direction;
	vector3f side;
	point3f position;
};

real function_30bf0(vector3f *vector);
bool function_26d290(point3f const *origin, point3f const *target, long sector_index, long *output_sector);

// @retail 0x26e180
bool function_26e180(long record_index, s_record_motion_view const *motion, short mode, point3f *reported_point,
	s_type_c3b527 *output, long *output_sector)
{
	bool result = false;
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	if (record->type == 1)
	{
		vector3f direction = motion->direction;
		direction.k = 0.0f;
		if (function_30bf0(&direction) > 0.0f)
		{
			point3f point;
			real point_z;
			switch (mode)
			{
			case 1:
				point_z = direction.k * 0.25f + motion->position.z;
				point.x = direction.i * 0.25f + motion->position.x;
				point.y = direction.j * 0.25f + motion->position.y;
				break;
			case 2:
				point_z = motion->position.z - direction.k * 0.25f;
				point.x = motion->position.x - direction.i * 0.25f;
				point.y = motion->position.y - direction.j * 0.25f;
				break;
			case 3:
				point = motion->position;
				point_z = point.z;
				break;
			default:
				mode = NONE;
				break;
			}
			if (mode != NONE)
			{
				vector3f side = motion->side;
				point.x += side.i * 0.15f;
				point.y += side.j * 0.15f;
				point.z = point_z;
				short index = record->point.output_index;
				if (function_210690(index, &point, &output->point))
				{
					output->output_index = index;
					if (function_26d290(&record->point.point, &output->point, record->location_index, output_sector))
					{
						if (reported_point)
							*reported_point = point;
						result = true;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x26db50
long function_26db50(long owner, short type)
{
	long result = NONE;
	long const *owner_reference = &owner;
	s_record_pool_iterator iterator;
	iterator.data = g_51eca4;
	iterator.index = NONE;
	s_location_record_view *record;
	while ((record = (s_location_record_view *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (record->owner == *owner_reference && record->type == type)
		{
			result = iterator.datum_index;
			break;
		}
	}
	return result;
}

// @retail 0x26dbd0
void function_26dbd0(long record_index, long actor_index)
{
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	s_location_record_actor_view *actor = (s_location_record_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_location_record_actor_view));
	if (actor->record_index != record_index)
	{
		actor->record_index = record_index;
		record->users++;
	}
}

// @retail 0x26e030
s_262b40_result *__stdcall function_26e030(s_reference reference)
{
	s_262b40_result *result = NULL;
	long index = reference.unknown0;
	s_location_record_view *record = &((s_location_record_view *)g_51eca4->data)[reference.unknown2 & 0x7fff];
	if (index >= 0 && index < record->count && (record->active[index >> 5] & (1 << (index & 0x1f))))
		result = &record->entries[index];
	return result;
}

struct s_record_object_motion_view
{
	byte unknown00[0x30];
	point3f position;
	byte unknown3c[0x88 - 0x3c];
	vector3f velocity;
};

long function_baf80(long object_index);

// @retail 0x26df40
bool function_26df40(long actor_index, long object_index, bool ignore_speed)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	if (object_index != NONE)
	{
		long root_index = function_baf80(object_index);
		s_record_object_motion_view *object = (s_record_object_motion_view *)((s_object_header_view *)g_4e0300->data)[root_index & 0xffff].object;
		if (ignore_speed || sqrt(length_sq3f(&object->velocity)) < 0.1f)
		{
			point3f position = object->position;
			vector3f delta;
			vector3d_from_points3d(&position, &actor->position, &delta);
			if (sqrt(delta.j * delta.j + (delta.i * delta.i + delta.k * delta.k)) < 15.0f)
				result = true;
		}
	}
	return result;
}


struct s_location_entry_view
{
	union
	{
		s_type_c3b527 location;
		struct
		{
			byte unknown00[0xe];
			word flags;
		};
	};
	short field10;
	word sector;
	long owner;
	vector2f direction;
};

struct s_record_sector_map
{
	byte unknown00[0x30];
	struct s_sector_entry
	{
		word sector;
		byte unknown02[6];
	} *entries;
};

struct s_bsp3d;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
vector2f *function_11df30(vector2f *angles, vector3f const *vector);

PRIVATE __forceinline bool location_entry_active(dword const *bits, long index)
{
	return (bits[index >> 5] & (1UL << (index & 31))) != 0;
}

PRIVATE __forceinline void location_entry_activate(dword *bits, long index)
{
	bits[index >> 5] |= 1UL << (index & 31);
}

// @retail 0x26d9c0
bool function_26d9c0(long record_index, s_type_c3b527 const *point, short entry_index, short type, long owner, vector3f const *direction)
{
	s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (record_index & 0xffff) * sizeof(s_location_record_view));
	bool result = false;
	if (entry_index >= 0 && entry_index < 32)
	{
		if (!location_entry_active(record->active, entry_index))
		{
			s_location_entry_view *entry = (s_location_entry_view *)&record->entries[entry_index];
			point3f position;
			function_210850(point, &position);
			position.x = g_4687b0->i * 0.05f + position.x;
			position.y = g_4687b0->j * 0.05f + position.y;
			position.z = g_4687b0->k * 0.05f + position.z;
			long sector_index = function_14a280((s_bsp3d *)g_4e0340, 0, &position);
			if (sector_index == NONE)
				goto done;
			entry->sector = ((s_record_sector_map *)g_4e0348)->entries[sector_index].sector;
			if (entry->sector == (word)NONE)
				goto done;
			location_entry_activate(record->active, entry_index);
			entry->owner = owner;
			entry->location = *point;
			entry->flags = 0;
			entry->field10 = NONE;
			if (direction)
				function_11df30(&entry->direction, direction);
			else
			{
				entry->direction.i = 0.0f;
				entry->direction.j = 0.0f;
			}
			entry->flags |= 0x40;
			long type_value = type;
			dword flags = entry->flags;
			if (type_value > 0 && type_value <= 3)
				entry->flags = (word)(flags | 0x90);
			((short *)record->unknown444)[entry_index] = type;
		}
		result = true;
	}
done:
	return result;
}

// @retail 0x26e090
short __stdcall function_26e090(long object_index, s_object_marker *markers, short *types, short capacity)
{
	short count = function_b8d30(object_index, 0x100006c0, markers, capacity, false);
	if (types)
	{
		for (short i = 0; i < count; ++i)
			types[i] = 2;
	}
	if (count < capacity)
	{
		short added = function_b8d30(object_index, 0x110006c1, markers + count, capacity - count, false);
		if (types)
		{
			for (short i = count; i < count + added; ++i)
				types[i] = 1;
		}
		count += added;
		if (count < capacity)
		{
			added = function_b8d30(object_index, 0x0b0006c2, markers + count, capacity - count, false);
			if (types)
			{
				for (short i = count; i < count + added; ++i)
					types[i] = 3;
			}
			count += added;
		}
	}
	return count;
}

struct s_pathfinding_data;
struct s_path_trace_result;
struct s_collision_result_1697c0;
bool function_26c590(s_pathfinding_data const *pathfinding, point3f const *origin, long sector_index,
	long target_sector_index, vector3f const *direction, real distance, s_path_location const *location,
	s_path_trace_result *trace);
long function_26d100(vector3f const *up, long *location, s_collision_result_1697c0 *collision, point3f const *point);

struct s_record_pathfinding_view
{
	byte field_0[0xc4];
	long count;
	s_pathfinding_data *pathfinding;
};

struct s_record_collision_view
{
	long type;
	real fraction;
	point3f point;
	byte field_14[0x5c - 0x14];
};

PRIVATE inline void record_rotate_vector(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real along = record_dot3f(axis, vector) * (1.0f - cosine);
	real i = vector->i, j = vector->j, k = vector->k;
	vector->i = axis->i * along + i * cosine - (axis->k * j - axis->j * k) * sine;
	vector->j = axis->j * along + j * cosine - (axis->i * k - axis->k * i) * sine;
	vector->k = axis->k * along + k * cosine - (axis->j * i - axis->i * j) * sine;
}

// @retail 0x26d570
long function_26d570(long object_index)
{
	long result = function_26d3f0(object_index, 0);
	if (result != NONE)
	{
		s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (result & 0xffff) * sizeof(s_location_record_view));
		vector3f const *up = g_4687b0;
		long sector_index = record->location_index;
		real radius = *(real *)((byte *)object_get(object_index) + 0x3c);
		long ring = 0;
		long rings_remaining = 2;
		do
		{
			real distance = (real)((double)radius * 1.2 + (real)ring * radius);
			real phase = (real)ring * 0.39269909262657166f;
			for (short spoke = 0; spoke < 8; spoke++)
			{
				vector3f direction = *g_4687a8;
				real angle = (real)spoke * 0.7853981852531433f + phase;
				real sine = (real)sin(angle);
				real cosine = (real)cos(angle);
				record_rotate_vector(&direction, up, sine, cosine);
				s_record_pathfinding_view *structure = (s_record_pathfinding_view *)g_4e0348;
				s_pathfinding_data *pathfinding = NULL;
				if (structure->count > 0)
					pathfinding = structure->pathfinding;
				s_sector_trace_result trace;
				point3f *origin = &record->point.point;
				if (!function_26c590(pathfinding, origin, sector_index, NONE, &direction, distance, NULL,
					(s_path_trace_result *)&trace) && !trace.blocked)
				{
					point3f point;
					point.x = direction.i * trace.distance + origin->x;
					point.y = direction.j * trace.distance + origin->y;
					point.z = direction.k * trace.distance + origin->z;
					point.x += up->i * 0.5f;
					point.y += up->j * 0.5f;
					point.z += up->k * 0.5f;
					s_location_entry_view *entry = (s_location_entry_view *)&record->entries[record->count];
					point3f world;
					s_record_collision_view collision;
					*(short *)((byte *)&collision + 0x24) = NONE;
					if (!function_2104b0(record->point.output_index, &point, &world))
						world = point;
					s_pathfinding_data *volatile saved_pathfinding = NULL;
					structure = (s_record_pathfinding_view *)g_4e0348;
					if (structure->count > 0)
						saved_pathfinding = structure->pathfinding;
					entry->owner = function_26d100(up, (long *)&entry->location, (s_collision_result_1697c0 *)&collision, &world);
					up = g_4687b0;
					point.x = up->i * 0.1f + collision.point.x;
					point.y = up->j * 0.1f + collision.point.y;
					point.z = up->k * 0.1f + collision.point.z;
					long sector = function_14a280((s_bsp3d *)g_4e0340, 0, &point);
					if (sector != NONE)
						entry->sector = ((s_record_sector_map *)g_4e0348)->entries[sector].sector;
					if (entry->sector != (word)NONE)
					{
						entry->flags |= 0x40;
						entry->direction.i = 0.0f;
						entry->direction.j = 1.5707963705062866f;
						entry->field10 = NONE;
						((short *)record->unknown444)[record->count] = 0;
						location_entry_activate(record->active, record->count);
						if (++record->count == 32)
							break;
					}
				}
			}
			ring++;
		}
		while (--rings_remaining);
	}
	return result;
}


// @retail 0x26dde0
void __stdcall function_26dde0(long actor_index)
{
 long const *input_reference = &actor_index;
 byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
 long owner = *(long *)(actor + 0x3f8);
 if (owner != NONE)
 {
  if (*(long *)(actor + 0x26c) != NONE)
  {
   function_26def0(actor_index, NONE);
   return;
  }
  bool has_record = *(long *)(actor + 0x3f4) != NONE;
  bool valid = function_26df40(*input_reference, owner, has_record);
  if (valid)
  {
   long record_index = *(long *)(actor + 0x3f4);
   *(short *)(actor + 0x3fc) = 0;
   if (record_index == NONE)
   {
    record_index = function_26db50(*(long volatile *)(actor + 0x3f8), 0);
    if (record_index == NONE)
     record_index = function_26d570(*(long volatile *)(actor + 0x3f8));
    if (record_index != NONE)
    {
     function_26dbd0(record_index, actor_index);
     *(short *)(actor + 0x2c) += g_510c54->field_2_3 * 8;
    }
    else
     goto failure;
   }
   else
    valid = function_26dc90(record_index);
   if (valid)
    return;
  }
 failure:
  if (*(long *)(actor + 0x3f4) != NONE)
   function_26dc20(actor_index);
  ++*(short *)(actor + 0x3fc);
  if ((real)*(short *)(actor + 0x3fc) * g_510c54->rate > 5.0f)
  {
   function_26def0(actor_index, NONE);
   *(short *)(actor + 0x2c) = 0;
  }
 }
}
