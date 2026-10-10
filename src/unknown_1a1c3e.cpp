// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_1A1C3E.CPP: the motion sensor. Each local player keeps a list of up
   to 16 objects (the nearby units first, then the other objects), and ten
   samples of up to 16 blips that fade out in turn. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "object_iterator.h"
#include <math.h>

/* ---- types ---- */

struct s_motion_sensor_blip
{
	char x;
	char y;
	char type;
	char height;
};

struct s_motion_sensor_sample
{
	s_motion_sensor_blip blips[16];
	long count;
};

struct s_motion_sensor_player
{
	void motion_sensor_add_other_object(long object_index);
	s_motion_sensor_sample samples[10];
	long objects[16];
	long nearby_count;
	long other_count;
};

struct s_motion_sensor_globals
{
	s_motion_sensor_player players[4];
	long sample_index;
	long update_ticks;
};

struct s_sensor_object
{
	long definition_index;
	byte unknown04[0x30 - 0x4];
	point3f position;
	byte unknown3c[0x70 - 0x3c];
	vector3f forward;
	byte unknown7c[0xaa - 0x7c];
	byte type;
	byte unknownab[0xc1 - 0xab];
	byte flagsc1;
	byte unknownc2[0x10a - 0xc2];
	byte flags10a_0 : 1;
	byte flags10a_1 : 1;
	byte flags10a_2 : 1;
	byte unknown10b[0x12c - 0x10b];
	union
	{
		long unknown12c;
		struct
		{
			short unknown12c_low;
			short team12e;
		};
	};
	byte unknown130[4];
	dword flags134_0 : 3;
	dword flags134_3 : 1;
	dword flags134_4 : 26;
	dword flags134_30 : 1;
	short team138;
	byte unknown13a[2];
	long unknown13c;
	byte unknown140[0x14a - 0x140];
	byte flags14a;
	byte unknown14b[0x248 - 0x14b];
	long unknown248;
	long unknown24c;
	byte unknown250[0x346 - 0x250];
	short zoom_index;
};

struct s_sensor_object_header
{
	byte unknown00[8];
	s_sensor_object *object;
};

struct s_sensor_block_element
{
	dword : 11;
	dword flag11 : 1;
	dword : 20;
	dword type;
	byte unknown08[0xb0 - 0x8];
};

struct s_sensor_definition
{
	byte unknown00[0xbc];
	dword : 6;
	dword flagbc_6 : 1;
	dword : 25;
	byte unknownc0[2];
	short heightc2;
	byte unknownc4[0x194 - 0xc4];
	short height194;
	byte unknown196[0x1c8 - 0x196];
	long block_count;
	s_sensor_block_element *block;
};

struct s_sensor_player
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

struct s_sensor_tag_globals
{
	byte unknown00[0x134];
	struct s_sensor_vehicle_globals
	{
		byte unknown00[0x3c];
		real speed;
	} *vehicle;
};

/* ---- globals ---- */

s_motion_sensor_globals *g_51e994;
real g_51e98c;
bool g_51e990;
real g_47ff98 = 1.1f;
real g_47ff9c[3] = { 0.0f, -0.75f, 1.0f };
short g_4b9dd0;
short g_4b9dd2;

extern color3f const g_445320[9] =
{
	{ 0.0f, 0.0f, 0.0f },
	{ 1.0f, 0.5f, 0.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 0.5f, 0.5f, 1.0f },
	{ 1.0f, 1.0f, 0.3f },
	{ 1.0f, 0.3f, 0.3f },
};

/* ---- callees ---- */

void *function_123d40(char const *name, char const *type_name, long size);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
real magnitude3d(vector3f const *v);
s_object *function_badc0(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);
bool function_1df560(short team_a, short team_b);
long function_155760(long index);
real function_30bf0(vector3f *v);
bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);
long function_e70e0(long unit_index);
bool function_53750(long index);
void function_cafc0(long unit_index, point3f *position);
long function_1469f0(real seconds);
void function_254200(void);
void __stdcall function_254490(point2f const *point, real scale, real alpha, color3f const *color, bool pulse);
void __stdcall function_2548f0(point2f const *center, real scale);

#define SENSOR_OBJECT(index) (((s_sensor_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define SENSOR_PLAYER(index) (&((s_sensor_player *)g_4e8c24->data)[(index) & 0xffff])
#define SENSOR_DEFINITION(index) ((s_sensor_definition *)g_4e3b44[(index) & 0xffff].bytes)

#define local_player_get_unit_index(local_player_index) \
	(function_14ddc0(local_player_index) ? SENSOR_PLAYER(function_14de70(local_player_index))->unit_index : NONE)

/* ---- the object lists ---- */

// @retail 0x1a1c3e
void motion_sensor_add_nearby_object(s_motion_sensor_player *sensor, long object_index)
{
	if (sensor->nearby_count < 16)
	{
		if (sensor->other_count)
		{
			if (sensor->other_count + sensor->nearby_count < 16)
				sensor->objects[sensor->other_count + sensor->nearby_count] = sensor->objects[sensor->nearby_count];
			else
				sensor->other_count--;
		}
		sensor->objects[sensor->nearby_count] = object_index;
		sensor->nearby_count++;
	}
}

// @retail 0x1a1c8e
void s_motion_sensor_player::motion_sensor_add_other_object(long object_index)
{
	s_motion_sensor_player *sensor = this;
	long *local_0 = &sensor->other_count;
	long count = sensor->nearby_count;
	long local_1 = *(volatile long *)local_0;
	count += local_1;
	if (count < 16)
	{
		sensor->objects[count] = object_index;
		(*local_0)++;
	}
}

// @retail 0x1a1cb5
void motion_sensor_remove_object(s_motion_sensor_player *sensor, long index)
{
	if (index >= 0)
	{
		long count = sensor->nearby_count + sensor->other_count;
		if (index < count)
		{
			if (index < sensor->nearby_count)
			{
				if (index != sensor->nearby_count - 1)
					sensor->objects[index] = sensor->objects[sensor->nearby_count - 1];
				if (sensor->other_count)
					sensor->objects[sensor->nearby_count - 1] = sensor->objects[sensor->nearby_count + sensor->other_count - 1];
				sensor->nearby_count--;
			}
			else
			{
				if (index != count - 1)
					sensor->objects[index] = sensor->objects[count - 1];
				sensor->other_count--;
			}
		}
	}
}

// @retail 0x1a1d2a
void motion_sensor_clear_nearby_objects(s_motion_sensor_player *sensor)
{
	for (long count = sensor->nearby_count; count; count--)
		motion_sensor_remove_object(sensor, 0);
}

// @retail 0x1a1d45
void motion_sensor_initialize(void)
{
	g_51e994 = (s_motion_sensor_globals *)function_123d40("motion sensor", NULL, sizeof(s_motion_sensor_globals));
}

// @retail 0x1a1d55
bool motion_sensor_object_moving(long object_index)
{
	s_sensor_object *object = SENSOR_OBJECT(object_index);
	real speed = 0.0f;
	bool result = false;

	if (!(object->flagsc1 & 1))
	{
		vector3f velocity;
		function_ba1d0(object_index, &velocity, NULL);
		velocity.k *= 0.33f;
		speed = magnitude3d(&velocity);
	}
	if (((1 << object->type) & 3) && object->unknown13c != NONE)
	{
		if (speed >= ((s_sensor_tag_globals *)g_4e034c)->vehicle->speed * 1.05f)
			result = true;
	}
	else
	{
		if (speed >= g_510c94->motion_sensor_minimum_speed)
			result = true;
	}
	return result;
}

// @retail 0x1a1dfe
void motion_sensor_blip_set_position(point2f const *point, s_motion_sensor_blip *blip)
{
	real x = point->x < 0.0f ? 0.0f : (point->x > 64.0f ? 64.0f : point->x);
	real y = point->y < 0.0f ? 0.0f : (point->y > 64.0f ? 64.0f : point->y);
	blip->x = (char)x;
	blip->y = (char)y;
}

// @retail 0x1a1e49
char motion_sensor_object_type(long local_player_index, long object_index)
{
	char result = 0;
	long player_index = function_14de70(local_player_index);

	if (player_index != NONE)
	{
		s_sensor_player *player = SENSOR_PLAYER(player_index);
		long team = player->team;

		if (object_index == NONE)
		{
			result = 6;
		}
		else if (object_index == player->unit_index)
		{
			result = 1;
		}
		else
		{
			s_sensor_object *object = SENSOR_OBJECT(object_index);
			long mask = 1 << object->type;

			if (mask & 3)
			{
				if (mask & 2)
				{
					if (object->unknown12c == NONE || !function_1df560(object->team138, team))
					{
						long rider_index = object->unknown24c;
						if (rider_index == NONE)
							rider_index = object->unknown248;
						if (rider_index != NONE)
						{
							result = function_1df560(SENSOR_OBJECT(rider_index)->team138, team) ? 5 : 4;
						}
						else
						{
							s_sensor_definition *definition = SENSOR_DEFINITION(object->definition_index);
							result = 4;
							if (definition->block_count > 1 && definition->block[0].type == 0xa000082)
								result = 5;
						}
					}
					else
					{
						result = 5;
					}
				}
				else
				{
					result = function_1df560(object->team138, team) ? 3 : 2;
					if (g_4e6948->state == 2 && object->unknown13c != NONE && function_53750(object->unknown13c & 0xffff))
						result = result == 3 ? 8 : 7;
				}
			}
			else if (mask & 0x1000)
			{
				result = function_1df560(object->team12e, team) ? 3 : 2;
			}
		}
	}
	if (!result)
		result = 2;
	return result;
}

// @retail 0x1a1faf
void motion_sensor_blip_set_type(long local_player_index, long object_index, s_motion_sensor_blip *blip)
{
	s_motion_sensor_blip *volatile *local_0 = (s_motion_sensor_blip *volatile *)&blip;
	char type = motion_sensor_object_type(local_player_index, object_index);
	s_motion_sensor_blip *local_1 = *local_0;
	volatile char local_2 = type;

	local_1->type = type;
	if (object_index == NONE)
	{
		local_1->height = 0;
	}
	else
	{
		s_sensor_object *object = SENSOR_OBJECT(object_index);
		long mask = 1 << object->type;

		if (mask & 3)
		{
			short height = SENSOR_DEFINITION(object->definition_index)->height194;
			if (height < 0)
				height = 0;
			else if (height > 3)
				height = 3;
			local_1 = *local_0;
			local_1->height = (char)height;
			if (g_4e6948->state == 2 && (local_2 == 7 || local_2 == 8))
				local_1->height = 2;
		}
		else if (mask & 0x1000)
		{
			short height = SENSOR_DEFINITION(object->definition_index)->heightc2;
			if (height < 0)
				height = 0;
			else if (height > 3)
				height = 3;
			local_1 = *local_0;
			local_1->height = (char)height;
		}
	}
}

// @retail 0x1a2084
bool motion_sensor_object_valid(long object_index)
{
	s_sensor_object *object = (s_sensor_object *)function_badc0(object_index, NONE);
	bool result = false;
	long mask;
	if (!object)
		goto local_0;
	mask = 1 << object->type;
	if (mask & 3)
	{
		if (object->unknown13c != NONE)
			goto local_0;
	}
	else
	{
		if (!(mask & 0x1000))
			goto local_0;
		if (TEST_FIELD_BIT(SENSOR_DEFINITION(object->definition_index)->flagbc_6))
			goto local_0;
	}
	if (!TEST_FIELD_BIT(object->flags10a_2))
		result = true;
local_0:
	return result;
}

// @retail 0x1a20e6
long __stdcall motion_sensor_object_visible(long object_index)
{
	long *local_0 = &object_index;
	s_sensor_object *object = SENSOR_OBJECT(*(volatile long *)local_0);
	bool always = false;
	bool unhidden = true;
	bool flag30 = false;

	if ((1 << object->type) & 3)
	{
		long zoom = function_e70e0(*(volatile long *)local_0);
		if ((object->flags14a & 0x21) || (zoom != 0 && zoom != 3))
			always = true;
		flag30 = TEST_FIELD_BIT(object->flags134_30);
		unhidden = g_4e6948->state == 2 || !TEST_FIELD_BIT(object->flags134_3);
		if (always)
			goto local_0;
	}
	bool moving = motion_sensor_object_moving(*(volatile long *)local_0);
	if (unhidden && (moving || flag30))
		goto local_0;
	if (!g_51e990)
		return false;
local_0:
	return true;
}

// @retail 0x1a2197
void motion_sensor_draw_blip(char type, point2f const *point, real scale, real intensity, char height)
{
	color3f const *color = &g_445320[type];
	real alpha = 1.0f;
	real offset = g_47ff9c[height];

	if (type == 6)
		alpha = ((real)sin(g_510c54->game_time * g_510c54->rate * 3.14159265f) + 1.0f) * 0.033333335f + 1.0f;
	function_254490(point, scale, alpha * intensity + offset, color, type == 6);
}

// @retail 0x1a221d
void motion_sensor_reset(long local_player_index)
{
	s_motion_sensor_globals *globals = g_51e994;
	s_motion_sensor_player *sensor = &globals->players[local_player_index];

	globals->update_ticks = 0;
	sensor->other_count = 0;
	sensor->nearby_count = 0;
	for (long i = 0; i < 10; i++)
		sensor->samples[i].count = 0;
}

// @retail 0x1a224e
void motion_sensor_update_pulse(void)
{
	real time = (real)fmod((double)g_510c54->game_time * g_510c54->rate, (double)2.1f);

	if (time < 2.0375f)
		g_51e98c = 1.0f / ((time + 0.0625f) * g_47ff98);
	else
		g_51e98c = 0.4f;
}

// @retail 0x1a2336
void motion_sensor_update_nearby_objects(void)
{
	point3f positions[4];
	real range = g_510c94->motion_sensor_range * 1.5f;
	real range_squared = range * range;
	bool valid[4];
	long i;

	for (i = 0; i < 4; i++)
	{
		valid[i] = false;
		if (function_14ddc0(i))
		{
			s_motion_sensor_player *sensor = &g_51e994->players[i];
			long unit_index = local_player_get_unit_index(i);
			if (unit_index != NONE)
			{
				valid[i] = true;
				motion_sensor_clear_nearby_objects(sensor);
				function_cafc0(unit_index, &positions[i]);
			}
		}
	}

	s_data_datum_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.datum_index = NONE;
	iterator.index = NONE;
	while (data_datum_iterator_next(&iterator))
	{
		s_sensor_player *player = (s_sensor_player *)iterator.datum;
		if (player->unit_index != NONE)
		{
			s_sensor_object *unit = SENSOR_OBJECT(player->unit_index);
			bool room = false;
			s_motion_sensor_player *sensor = g_51e994->players;
			for (i = 0; i < 4; i++, sensor++)
			{
				if (valid[i] && sensor->nearby_count < 16)
				{
					real dy = unit->position.y - positions[i].y;
					real dx = unit->position.x - positions[i].x;
					room = true;
					if (range_squared >= dx * dx + dy * dy)
						motion_sensor_add_nearby_object(sensor, player->unit_index);
				}
			}
			if (!room)
				break;
		}
	}
}

// @retail 0x1a24a5
void motion_sensor_clear_objects(void)
{
	s_motion_sensor_player *sensor = g_51e994->players;

	for (long i = 0; i < 4; i++, sensor++)
	{
		if (function_14ddc0(i) && local_player_get_unit_index(i) != NONE)
		{
			sensor->other_count = 0;
			motion_sensor_clear_nearby_objects(sensor);
		}
	}
}

// @retail 0x1a2505
void motion_sensor_update_other_objects(void)
{
	struct
	{
		point3f field_0[4];
		s_sensor_object *volatile field_30;
		s_type_f1af8e field_34;
		bool field_44[4];
		real field_48;
		volatile long field_4c;
	} local_0;
	point3f (&positions)[4] = local_0.field_0;
	bool (&valid)[4] = local_0.field_44;
	real range = g_510c94->motion_sensor_range * 1.5f;
	real &range_squared = local_0.field_48;
	range_squared = range * range;
	long i;

	for (i = 0; i < 4; i++)
	{
		valid[i] = false;
		if (function_14ddc0(i))
		{
			long unit_index = local_player_get_unit_index(i);
			if (unit_index != NONE)
			{
				valid[i] = true;
				function_cafc0(unit_index, &positions[i]);
			}
		}
	}

	s_type_f1af8e &iterator = local_0.field_34;
	function_bae80(&iterator, 0x1003, 1);
	s_sensor_object *object;
	while ((object = (s_sensor_object *)function_baeb0(&iterator), local_0.field_30 = object, object) != NULL)
	{
		if (motion_sensor_object_valid(iterator.object_index))
		{
			volatile bool room = false;
			s_motion_sensor_player *sensor = g_51e994->players;
			point3f *local_1 = positions;
			for (*(long *)&local_0.field_4c = 0; local_0.field_4c < 4; (*(long *)&local_0.field_4c)++, sensor++, local_1++)
			{
				if (valid[local_0.field_4c] && sensor->other_count + sensor->nearby_count < 16)
				{
					real dy = object->position.y - local_1->y;
					real dx = object->position.x - local_1->x;
					room = true;
					if (range_squared >= dx * dx + dy * dy)
						sensor->motion_sensor_add_other_object(iterator.object_index);
				}
			}
			if (!room)
				break;
		}
	}
}

// @retail 0x1a264e
void motion_sensor_build_sample(long local_player_index)
{
	s_motion_sensor_player *sensor = &g_51e994->players[local_player_index];
	s_motion_sensor_sample *sample = &sensor->samples[g_51e994->sample_index];
	long unit_index = local_player_get_unit_index(local_player_index);

	sample->count = 0;
	if (unit_index != NONE)
	{
		volatile real angle = 0.0f - (g_4ed284->entries[local_player_index].yaw + 1.5707964f);
		real sine = (real)sin(angle);
		real scale = 1.0f / g_510c94->motion_sensor_range;
		real cosine = (real)cos(angle);
		point3f origin;
		function_cafc0(unit_index, &origin);

		long count = sensor->nearby_count + sensor->other_count;
		for (long i = 0; i < count && sample->count < 16; i++)
		{
			long object_index = sensor->objects[i];
			s_sensor_object *object = (s_sensor_object *)function_badc0(object_index, NONE);
			if (object && (byte)motion_sensor_object_visible(object_index))
			{
				real dx = object->position.x - origin.x;
				real dy = object->position.y - origin.y;
				real range = g_510c94->motion_sensor_range;
				if (range * range > dx * dx + dy * dy)
				{
					s_motion_sensor_blip *blip = &sample->blips[sample->count++];
					point2f point;
					point.x = ((dy * sine - dx * cosine) * scale + 1.0f) * 32.0f;
					point.y = ((dy * cosine + dx * sine) * scale + 1.0f) * 32.0f;
					motion_sensor_blip_set_position(&point, blip);
					motion_sensor_blip_set_type(local_player_index, object_index, blip);
				}
			}
		}
	}
}

// @retail 0x1a2823
void motion_sensor_render(long local_player_index, short const *origin)
{
	short mode = (short)function_155760(local_player_index);
	point2f center;

	center.x = (real)(origin[0] + g_4b9dd2);
	center.y = (real)(origin[1] + g_4b9dd0);
	if (mode != 3 && mode != 2)
	{
		s_motion_sensor_player *sensor = &g_51e994->players[local_player_index];
		function_254200();
		for (long i = 0, age = 10; age > 0; i++, age--)
		{
			s_motion_sensor_sample *sample = &sensor->samples[(g_51e994->sample_index + i) % 10];
			real fraction = (real)age * 0.1f;
			real scale = fraction * fraction;
			fraction = pow(1.0f - fraction, 3.5f) * 7.0f + 1.0f;
			for (long j = 0; j < sample->count; j++)
			{
				s_motion_sensor_blip *blip = &sample->blips[j];
				if (blip->type)
				{
					point2f point;
					point.x = (real)blip->x;
					point.y = (real)blip->y;
					motion_sensor_draw_blip(blip->type, &point, scale, fraction, blip->height);
				}
			}
		}
		function_2548f0(&center, g_51e98c);
	}
}

// @retail 0x1a2965
bool motion_sensor_enemy_vehicle_ahead(long local_player_index)
{
	long player_index = function_14de70(local_player_index);
	bool result = false;

	if (player_index != NONE)
	{
		s_sensor_player *player = SENSOR_PLAYER(player_index);
		if (((s_sensor_player volatile *)player)->unit_index != NONE)
		{
			long unit_index = player->unit_index;
			s_motion_sensor_player *sensor = &g_51e994->players[local_player_index];
			s_sensor_object *unit = SENSOR_OBJECT(unit_index);
			point3f unit_position;
			function_b9dd0(unit_index, &unit_position);
			for (long i = 0; i < sensor->other_count; i++)
			{
				long object_index = sensor->objects[sensor->nearby_count + i];
				if (object_index != NONE)
				{
					s_sensor_object *object = (s_sensor_object *)function_badc0(object_index, 3);
					if (object && ((1 << object->type) & 2) && motion_sensor_object_type(local_player_index, object_index) == 5)
					{
						point3f position;
						function_b9dd0(object_index, &position);
						real dx = position.x - unit_position.x;
						real dy = position.y - unit_position.y;
						real dz = position.z - unit_position.z;
						if (16.0f > dz * dz + dy * dy + dx * dx)
						{
							vector3f velocity;
							function_ba1d0(object_index, &velocity, NULL);
							if (1.5f > magnitude3d(&velocity))
							{
								vector3f direction;
								direction.i = object->position.x - unit->position.x;
								direction.j = object->position.y - unit->position.y;
								direction.k = object->position.z - unit->position.z;
								function_30bf0(&direction);
								if (direction.k * unit->forward.k + direction.j * unit->forward.j + direction.i * unit->forward.i < cos(0.5235987715423107))
								{
									s_sensor_definition *definition = SENSOR_DEFINITION(object->definition_index);
									for (long j = 0; j < definition->block_count; j++)
									{
										if (TEST_FIELD_BIT(definition->block[j].flag11))
										{
											result = true;
											break;
										}
									}
									if (result)
										break;
								}
							}
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1a2b6b
bool motion_sensor_enemy_nearby(long local_player_index)
{
	long player_index = function_14de70(local_player_index);
	bool result = false;

	if (player_index != NONE)
	{
		s_sensor_player *player = SENSOR_PLAYER(player_index);
		if (((s_sensor_player volatile *)player)->unit_index != NONE)
		{
			real range = g_510c94->motion_sensor_range;
			s_motion_sensor_player *sensor = &g_51e994->players[local_player_index];
			real range_squared = range * range;
			s_sensor_object *unit = SENSOR_OBJECT(player->unit_index);
			for (long i = 0; i < sensor->other_count; i++)
			{
				long object_index = sensor->objects[sensor->nearby_count + i];
				if (object_index != NONE)
				{
					s_sensor_object *object = (s_sensor_object *)function_badc0(object_index, 3);
					if (object)
					{
						long type = motion_sensor_object_type(local_player_index, object_index);
						if (type == 5 || type == 3)
						{
							real dx = object->position.x - unit->position.x;
							real dy = object->position.y - unit->position.y;
							if (range_squared >= dy * dy + dx * dx)
							{
								result = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1a22b4
void function_1a22b4(void)
{
	motion_sensor_update_pulse();
	if (g_51e994->update_ticks == 0)
	{
		motion_sensor_clear_objects();
		motion_sensor_update_nearby_objects();
		motion_sensor_update_other_objects();
		g_51e994->update_ticks = function_1469f0(0.5f);
	}
	else
	{
		motion_sensor_update_nearby_objects();
		g_51e994->update_ticks--;
	}
	if (g_51e994->sample_index == 0)
		g_51e994->sample_index = 9;
	else
		g_51e994->sample_index--;
	for (long i = 0; i < 4; i++)
	{
		if (function_14ddc0(i))
			motion_sensor_build_sample(i);
	}
}