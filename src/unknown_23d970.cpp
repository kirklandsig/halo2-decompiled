// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_23D970.CPP: a view state and its default direction, player and
   object searches, and the vertex shader constant table / full screen quad
   push buffer writers (batch 55-1) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <math.h>
#include <string.h>
#include "globals.h"
#include "data_array.h"
#include "object_iterator.h"
#include "effects.h"

long function_1366d0(short width, short height, short depth, short format, short alignment, long mipmap_count);

PRIVATE const dword g_4507c8[24] =
{
	31, 19, 27, 32, -1, -1, 17, -1, 16, 29, 30, 18,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, 23
};

typedef void *(__stdcall *texture_allocate_proc)(long size, long alignment);

// @retail 0x23e340
D3DTexture *function_23e340(short width, short height, short format,
	texture_allocate_proc allocate, long *size, void **data, D3DTexture *header)
{
	D3DTexture *result = 0;
	long bytes = function_1366d0(width, height, 1, format, 0x40, 0);
	bytes += -bytes & 0x7f;
	void *buffer = allocate(bytes, 0x80);
	if (header && buffer)
	{
		header->Lock = 0;
		header->Format = (g_4507c8[format] << 8) | 0x10029;
		header->Size = (((((dword)width * 4) >> 6) - 1) << 24) | ((height - 1) << 12) | (width - 1);
		header->Data = (dword)buffer & 0x0fffffff;
		header->Common = 0x40001;
		result = header;
	}
	else
	{
		buffer = 0;
	}
	*size = bytes;
	*data = buffer;
	return result;
}

/* ---- types ---- */

struct s_view_state
{
	real x;
	real y;
	real z;
	real yaw;
	real pitch;
	real unknown14;
	real unknown18;
};

struct s_view_globals
{
	s_view_state state;
	dword unknown1c;
	s_view_state output;
	byte valid;
};

struct s_player
{
	short identifier;
	byte unknown02[0x2a];
	long value2c;
	byte unknown30[0x90];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

struct s_object
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x14];
	short value2c;
};

struct s_input_state
{
	byte unknown00[0x10];
	byte values[0x38];
};

void __stdcall function_23d970(s_view_state *state);
bool function_015d00(long count, dword **out);

/* ---- globals ---- */

s_view_globals g_51ec40;
/* the camera the director flies (0x54 bytes) */
struct s_director_camera
{
	point3f position;
	byte unknown0c[0x20 - 0xc];
	vector3f forward;
	vector3f up;
	real value38;
	byte unknown3c[0x54 - 0x3c];
};

s_director_camera g_5022f8;
byte g_51ec11;
byte g_51ec13;
point3f g_51ec14;
real g_51ec20;
real g_51ec24;
short g_51ec3c;
byte g_470a20 = 2;
extern real g_54e854;
extern point3f *g_51ec28;
byte g_4e61b9;
s_input_state g_4e61dc[3];
s_input_state g_4e630c;
byte g_485af0;
real g_4856c4[9];
long g_470a3c[8];
byte g_470a38;

void __stdcall function_23d260(void *a, void *b, void *c);
void __stdcall function_23d790(void *a, void *b, void *c);
void __stdcall function_23d8e0(s_view_state *state);

/* the director's cameras by type (0 is the default, 1 flies freely): their
   updates, and the callbacks that hand their state over (.rdata 0x44ab7c
   and 0x44ab84) */
typedef void (__stdcall *camera_update_proc)(void *, void *, void *);
typedef void (__stdcall *camera_state_proc)(s_view_state *);

struct s_camera_state_procs
{
	camera_state_proc set_state;
	camera_state_proc report_state;
};

camera_update_proc const g_44ab7c[2] =
{
	function_23d260,
	function_23d790
};

s_camera_state_procs const g_44ab84[2] =
{
	{ 0, 0 },
	{ function_23d8e0, function_23d970 }
};

struct s_camera_start_23cea0
{
	point3f position;
	real yaw;
};

struct s_camera_scenario_23cea0
{
	byte unknown00[0x100];
	long count;
	s_camera_start_23cea0 *points;
};

// @retail 0x23cea0
void function_23cea0(s_view_state *state, long temporary)
{
	if (!g_51ec13)
	{
		s_camera_scenario_23cea0 *scenario = (s_camera_scenario_23cea0 *)g_4e0350;
		if (scenario->count && scenario->points)
		{
			g_51ec14 = scenario->points->position;
			g_51ec20 = scenario->points->yaw;
		}
		else
		{
			g_51ec14.x = g_51ec14.y = g_51ec14.z = 0.f;
			g_51ec20 = g_51ec24 = 0.f;
		}
	}
	g_51ec13 = 1;
	double cosine = cos(g_51ec24);
	double x = cos(g_51ec20) * cosine;
	double y = sin(g_51ec20) * cosine;
	double z = sin(g_51ec24);
	state->y = 0.f;
	state->x = 0.f;
	state->yaw = 0.f;
	state->pitch = 0.f;
	state->unknown14 = 0.f;
	state->unknown18 = g_54e854;
	*(point3f *)state = g_51ec14;
	state->yaw = (real)atan2(y, x);
	state->pitch = (real)atan2(z, sqrt(y * y + x * x));
	if (!temporary)
		g_51ec28 = (point3f *)state;
	if (g_51ec3c)
		g_44ab84[g_51ec3c].report_state(state);
	g_51ec11 = g_470a20;
	/* Clear by value so this shared global's address does not escape. */
	static const s_director_camera cleared_23cea0 = {0};
	g_5022f8 = cleared_23cea0;
	g_5022f8.forward = *g_4687a8;
	g_5022f8.up = *g_4687b0;
}

/* the object the camera keeps its point relative to (unknown_23d030.cpp) */
extern long g_470a28;
void function_23d030(long object_index);

extern byte g_51ec10;
struct s_observer_command;
void __stdcall function_16c840(long user_index, long unused, s_observer_command *command);

// @retail 0x23d090
void __stdcall function_23d090(void *state, void *input, s_observer_command *command)
{
	if (g_51ec10)
	{
		if (!((byte *)input)[4])
		{
			function_16c840(0, (long)input, command);
			s_director_camera const *start = 0;
			if (g_4686c4 != NONE)
				start = (s_director_camera const *)&g_4e9bd4[0].state;
			g_5022f8 = *start;
			goto finished;
		}
		s_view_state *kept = (s_view_state *)g_51ec28;
		*(point3f *)kept = g_5022f8.position;
		kept->yaw = (real)atan2(g_5022f8.forward.j, g_5022f8.forward.i);
		kept->pitch = (real)atan2(g_5022f8.forward.k,
			sqrt(g_5022f8.forward.j * g_5022f8.forward.j + g_5022f8.forward.i * g_5022f8.forward.i));
		function_23d030(g_470a28);
		if (g_51ec3c)
			g_44ab84[g_51ec3c].report_state(kept);
	}
	g_44ab7c[g_51ec3c](state, input, command);
	if (g_51ec10)
	{
		dword flags = *(dword *)command | 9;
		*(real *)((byte *)command + 0x88) = 0.f;
		*(dword *)command = flags;
	}
	g_5022f8.position = *(point3f *)state;
	g_5022f8.forward = *(vector3f *)((byte *)command + 0x2c);
	g_5022f8.up = *(vector3f *)((byte *)command + 0x38);
	g_5022f8.value38 = *(real *)((byte *)command + 0x28);
finished:
	g_51ec14 = g_5022f8.position;
	g_51ec20 = (real)atan2(g_5022f8.forward.j, g_5022f8.forward.i);
	g_51ec24 = (real)atan2(g_5022f8.forward.k,
		sqrt(g_5022f8.forward.j * g_5022f8.forward.j + g_5022f8.forward.i * g_5022f8.forward.i));
}

#define PLAYER(array, index) ((s_player *)((array)->data + sizeof(s_player) * (index)))

/* ---- functions ---- */

/* takes the camera's state: remembers it, and places the camera where it
   was with its forward direction */
// @retail 0x23d8e0
void __stdcall function_23d8e0(s_view_state *state)
{
	s_view_globals *globals = &g_51ec40;

	globals->output = *state;
	globals->valid = true;
	*(point3f *)&state->x = g_5022f8.position;
	state->yaw = (real)atan2(g_5022f8.forward.j, g_5022f8.forward.i);
	state->pitch = (real)atan2(g_5022f8.forward.k, sqrt(g_5022f8.forward.j * g_5022f8.forward.j + g_5022f8.forward.i * g_5022f8.forward.i));
	function_23d030(g_470a28);
}

// @retail 0x23d970
void __stdcall function_23d970(s_view_state *state)
{
	g_51ec40.state = *state;
	if (g_51ec40.valid)
	{
		*state = g_51ec40.output;
		return;
	}

	state->x = 0.f;
	state->y = 1.f;
	state->z = 0.f;
	state->yaw = (real)atan2(g_5022f8.forward.j, g_5022f8.forward.i);
	state->pitch = (real)atan2(g_5022f8.forward.k, sqrt(g_5022f8.forward.j * g_5022f8.forward.j + g_5022f8.forward.i * g_5022f8.forward.i));
}

PRIVATE __forceinline bool observer_next_index_23dba0(s_record_pool *players, long *index)
{
	++*index;
	if (*index >= 0 && *index < players->high_water_index)
	{
		long limit = players->high_water_index;
		dword *bits = players->bitmap;
		do
		{
			if (bits[*index >> 5] & (1 << (*index & 0x1f)))
				return true;
			++*index;
		} while (*index < limit);
	}
	return false;
}

// @retail 0x23dba0
bool function_23dba0(long player_index)
{
	s_record_pool *players = g_4e8c24;
	long team = PLAYER(players, player_index & 0xffff)->team;
	long index = NONE;

	for (;;)
	{
		if (!observer_next_index_23dba0(players, &index))
			break;
		if (index == NONE)
			break;
		s_player *player = (s_player *)(players->data + players->size * index);
		long datum = (player->identifier << 16) | index;
		if (datum != player_index && player->team == team)
			return true;
	}
	return false;
}

// @retail 0x23dc40
long function_23dc40(long player_index, long last_index, bool same_team)
{
	s_record_pool *players = g_4e8c24;
	long index = NONE;
	long team;

	if (same_team)
		team = PLAYER(players, player_index & 0xffff)->team;
	else
		team = NONE;
	long result = NONE;

	for (;;)
	{
		if (!observer_next_index_23dba0(players, &index))
			break;
		if (index == NONE)
			break;
		s_player *player = (s_player *)(players->data + players->size * index);
		long datum = (player->identifier << 16) | index;
		if (datum != player_index && player->value2c != NONE && (!same_team || player->team == team))
		{
			if (result == NONE)
				result = datum;
			else if ((datum & 0xffff) > (last_index & 0xffff))
			{
				result = datum;
				break;
			}
		}
	}

	if (result == NONE)
		result = last_index;
	return result;
}
// @retail 0x23dd30
long function_23dd30(void)
{
	struct
	{
		s_object *object;
		s_type_f1af8e iterator;
	} state;
	long result = NONE;

	state.iterator.signature = 0x86868686;
	state.iterator.type_mask = 0x40;
	state.iterator.flags = 0;
	state.iterator.index = 0;
	state.iterator.object_index = NONE;
	while ((state.object = function_baeb0(&state.iterator)) != 0)
	{
		if (state.object->parent_index == NONE && state.object->value2c != NONE)
			result = state.iterator.object_index;
	}
	return result;
}

/* the state of the observer camera that follows another player */
struct s_observer_state
{
	point3f position;
	byte unknown0c[0x18 - 0xc];
	real yaw;
	real pitch;
	real distance;
	real field_of_view;
	real timer;
	long player_index;
	long target_player_index;
	long target_unit_index;
	real delay;
	byte reset;
	char local_index;
};

void function_23dda0(s_observer_state *observer);

PRIVATE inline real camera_random_23da00(real minimum, real maximum)
{
	real random = function_x82e52f(&g_4e7408->seed, 0, 0);
	return minimum + (maximum - minimum) * random;
}

// @retail 0x23da00
void function_23da00(s_observer_state *observer, long local_index, long target)
{
	point3f const *position = 0;
	if (local_index != NONE && g_4686c4 != NONE)
		position = &g_4e9bd4[local_index].state.position;
	observer->position = *position;
	observer->field_of_view = g_54e854;
	observer->distance = camera_random_23da00(2.f, 6.f);
	observer->yaw = camera_random_23da00(0.f, 6.2831855f);
	observer->pitch = (real)(-(double)camera_random_23da00(0.47123894f, 1.0995574f));
	observer->timer = 3.f;
	real delay = 3.f;
	if (target != NONE)
		delay = 3.402823466e+38f;
	else if (g_4e6948->state == 2)
		delay = 15.f;
	observer->delay = delay;
	long player_index = NONE;
	if (local_index != NONE)
		player_index = g_4e8c20->entries[local_index];
	observer->player_index = player_index;
	observer->target_player_index = player_index;
	observer->target_unit_index = NONE;
	observer->reset = 0;
	observer->local_index = (char)local_index;
	if (player_index != NONE)
	{
		s_player *player = PLAYER(g_4e8c24, player_index & 0xffff);
		if (player->value2c != NONE)
			observer->target_unit_index = player->value2c;
		else if (*(long *)player->unknown30 != NONE)
			observer->target_unit_index = *(long *)player->unknown30;
		else
			function_23dda0(observer);
	}
}

struct s_game_options_flags_view
{
	byte unknown00[0x184];
	byte flag0 : 1;
};

PRIVATE inline bool observer_team_mode_23dda0(s_game_options_view *options)
{
	bool result = false;
	if (g_55e4d0[g_4e9ae8->engine_index])
		result = TEST_FIELD_BIT(((s_game_options_flags_view *)options)->flag0);
	return result;
}

// @retail 0x23dda0
void function_23dda0(s_observer_state *observer)
{
	s_game_options_view *options = g_4e6948;
	long unit_index = NONE;
	bool same_team;

	if (options->state == 2)
	{
		same_team = observer_team_mode_23dda0(options);
	}
	else
	{
		same_team = function_23dba0(observer->player_index);
	}

	observer->target_player_index = function_23dc40(observer->player_index, observer->target_player_index, same_team);
	if (observer->target_player_index != NONE)
		unit_index = PLAYER(g_4e8c24, observer->target_player_index & 0xffff)->value2c;

	real delay = 3.f;
	if (unit_index != observer->target_unit_index && unit_index != NONE)
	{
		observer->timer = delay;
		observer->target_unit_index = unit_index;
		observer->reset = false;
	}
	if (options->state == 2)
		delay = 15.f;
	observer->delay = delay;
}

s_object *function_badc0(long object_index, dword type_mask);
__declspec(noinline) s_player_state *function_16f3a0(long index);
real function_30bf0(vector3f *vector);
vector3f *function_11d090(vector3f const *vector, vector3f *out);
void function_172520(s_observer_command *command);

struct s_follow_output_23de50
{
	dword flags;
	point3f position;
	vector3f offset;
	byte unknown1c[8];
	real distance;
	real field_of_view;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	byte unknown50[0x88 - 0x50];
	real timer;
	byte modes[8];
	real value94;
	byte unknown98[8];
	real valuea0;
};

PRIVATE inline long camera_round_23de50(real value)
{
	long result;
	__asm fld value
	__asm fistp result
	return result;
}

// @retail 0x23de50
void __stdcall function_23de50(void *state, void *input, s_observer_command *output)
{
	s_observer_state *observer = (s_observer_state *)state;
	s_follow_output_23de50 *command = (s_follow_output_23de50 *)output;
	if (observer->target_unit_index != NONE && !function_badc0(observer->target_unit_index, -1))
		observer->target_unit_index = NONE;
	if (observer->target_unit_index == NONE)
	{
		function_23dda0(observer);
		if (observer->target_unit_index == NONE)
		{
			if (g_4e6948->state == 2)
				function_23dda0(observer);
			else
				observer->target_unit_index = function_23dd30();
		}
	}
	if (observer->target_unit_index != NONE)
	{
		byte *object = (byte *)function_badc0(observer->target_unit_index, -1);
		if (object)
		{
			struct s_header { byte unknown00[8]; byte *object; };
			byte *target = ((s_header *)g_4e0300->data)[observer->target_unit_index & 0xffff].object;
			command->position = *(point3f *)(target + *(short *)(target + 0x116) + 0x28);
			if ((1 << object[0xaa]) & 1)
			{
				if ((bool)(((dword)*(word *)(object + 0x348) >> 15) & 1) || observer->reset > 0)
				{
					long maximum = camera_round_23de50((real)g_510c54->field_2_3 * 0.33f);
					if (observer->reset < maximum)
						observer->reset++;
				}
				if (observer->reset == camera_round_23de50((real)g_510c54->field_2_3 * 0.33f))
					*(point3f *)observer->unknown0c = function_16f3a0(observer->local_index)->position;
			}
		}
		else
		{
			observer->reset = 0;
			command->position = observer->position;
		}
	}
	else
		command->position = observer->position;
	if (observer->reset == camera_round_23de50((real)g_510c54->field_2_3 * 0.33f))
	{
		vector3f previous_up = *(vector3f *)((byte *)function_16f3a0(observer->local_index) + 0x2c);
		point3f *kept = (point3f *)observer->unknown0c;
		vector3f direction;
		direction.i = command->position.x - kept->x;
		direction.j = command->position.y - kept->y;
		direction.k = command->position.z - kept->z;
		command->distance = function_30bf0(&direction);
		command->forward = direction;
		vector3f right;
		right.i = direction.k * previous_up.j - direction.j * previous_up.k;
		right.j = direction.i * previous_up.k - direction.k * previous_up.i;
		right.k = direction.j * previous_up.i - direction.i * previous_up.j;
		command->up.i = right.k * direction.j - right.j * direction.k;
		command->up.j = right.i * direction.k - right.k * direction.i;
		command->up.k = right.j * direction.i - right.i * direction.j;
		function_30bf0(&command->up);
		command->field_of_view = observer->field_of_view;
		command->offset = *g_4687a4;
		command->velocity = *g_4687a4;
		command->flags = 0x19;
		command->timer = 0.f;
	}
	else
	{
		command->distance = observer->distance;
		real cosine = (real)cos(observer->pitch);
		command->forward.i = (real)cos(observer->yaw) * cosine;
		command->forward.j = (real)sin(observer->yaw) * cosine;
		command->forward.k = (real)sin(observer->pitch);
		function_11d090(&command->forward, &command->up);
		command->field_of_view = observer->field_of_view;
		command->offset = *g_4687a4;
		command->velocity = *g_4687a4;
		command->flags = 1;
		command->timer = 0.f > observer->timer ? 0.f : observer->timer;
	}
	command->value94 = 0.f;
	command->modes[0] = 3;
	if (observer->timer == 3.f)
	{
		command->distance = 0.5f;
		command->valuea0 = 0.f;
		command->modes[3] = 3;
	}
	real delta = ((real *)input)[2];
	observer->timer -= delta;
	observer->delay = 0.f > observer->delay - delta ? 0.f : observer->delay - delta;
	bool advance = false;
	if (observer->player_index != NONE)
	{
		s_player *player = PLAYER(g_4e8c24, observer->player_index & 0xffff);
		long controller = *(long *)((byte *)player + 0x24);
		if (controller != NONE && g_4e61cc[(short)controller])
		{
			byte pressed = g_4e61b9 ? g_4e630c.values[0] : g_4e61dc[(short)controller].values[0];
			if (pressed == 1)
			{
				real delay = g_4e6948->state == 2 ? 15.f : 3.f;
				if (delay > observer->delay + 1.f)
					advance = true;
			}
		}
	}
	if ((observer->delay == 0.f && (!g_510c54->active || !g_510c54->unknown01)) || advance)
		function_23dda0(observer);
	function_172520(output);
}

static inline long next_index(long index, long maximum)
{
	long result = NONE;
	if (index >= 0 && index < maximum)
		result = index + 1;
	return result;
}

// @retail 0x23e400
bool function_23e400(long index)
{
	bool result = false;

	for (long i = 0; i != NONE; i = next_index(i, 3))
	{
		short slot = (short)i;
		if (g_4e61cc[slot])
		{
			s_input_state *state;
			if (g_4e61b9)
				state = &g_4e630c;
			else
				state = &g_4e61dc[slot];
			if (state && state->values[index])
				result = true;
		}
	}
	return result;
}

// @retail 0x23e460
void function_23e460(void)
{
	if (g_485af0)
	{
		real constants[22][4] =
		{
			{ 53.f, 15.f, 0.f, 0.15915494f },
			{ (real)sin(0.f), (real)cos(0.f), 0.f, 0.f },
			{ (real)sin(0.41887903f), (real)cos(0.41887903f), 0.f, 0.f },
			{ (real)sin(0.83775806f), (real)cos(0.83775806f), 0.f, 0.f },
			{ (real)sin(1.2566371f), (real)cos(1.2566371f), 0.f, 0.f },
			{ (real)sin(1.6755161f), (real)cos(1.6755161f), 0.f, 0.f },
			{ (real)sin(2.0943952f), (real)cos(2.0943952f), 0.f, 0.f },
			{ (real)sin(2.5132742f), (real)cos(2.5132742f), 0.f, 0.f },
			{ (real)sin(2.9321532f), (real)cos(2.9321532f), 0.f, 0.f },
			{ (real)sin(3.3510323f), (real)cos(3.3510323f), 0.f, 0.f },
			{ (real)sin(3.7699113f), (real)cos(3.7699113f), 0.f, 0.f },
			{ (real)sin(4.1887903f), (real)cos(4.1887903f), 0.f, 0.f },
			{ (real)sin(4.6076694f), (real)cos(4.6076694f), 0.f, 0.f },
			{ (real)sin(5.0265484f), (real)cos(5.0265484f), 0.f, 0.f },
			{ (real)sin(5.4454274f), (real)cos(5.4454274f), 0.f, 0.f },
			{ (real)sin(5.8643064f), (real)cos(5.8643064f), 0.f, 0.f },
			{ (real)sin(6.2831855f), (real)cos(6.2831855f), 0.f, 0.f },
			{ g_4856c4[0], g_4856c4[3], g_4856c4[6], 0.f },
			{ g_4856c4[1], g_4856c4[4], g_4856c4[7], 0.f },
			{ g_4856c4[2], g_4856c4[5], g_4856c4[8], 0.f },
			{ 1.f, 0.f, 0.99f, 42.f },
			{ 2.f, 3.051851e-05f, 1.f, 42.f }
		};

		for (long i = 1; i < 17; i++)
		{
			long next = (i == 16) ? 1 : i + 1;
			constants[i][2] = constants[next][0] - constants[i][0];
			constants[i][3] = constants[next][1] - constants[i][1];
		}

		D3DDevice_SetVertexShaderConstant(52, constants, 22);
		g_485af0 = 0;
	}
}

// @retail 0x23ecd0
void function_23ecd0(void)
{
	g_470a3c[0] = 1;
	g_470a3c[1] = 2;
	g_470a3c[2] = 3;
	g_470a3c[3] = 4;
	g_470a3c[4] = 5;
	g_470a3c[5] = 6;
	g_470a3c[6] = 7;
	g_470a3c[7] = 8;
}

struct s_shader_cache;
void function_1bbf0(long tag, long first, long second, long third, long fourth, real scale);
void function_1cf50(void);
void function_1c590(s_shader_cache *state, long tag, long index);
void function_1c6b0(void *state);
void __stdcall function_1c710(void *state);
void function_1ccb0(long format);
extern byte g_51f0f0[0x2d8];
PRIVATE byte const g_43f77a[20] =
{
	0x2a, 0x00, 0x03, 0x01, 0x02, 0x02, 0x00, 0x03, 0x03, 0x04,
	0x02, 0x05, 0x01, 0x06, 0x03, 0x07, 0x03, 0x09, 0x11, 0xff
};
extern dword g_4b8348;
byte g_470a2c;
byte g_470a39 = 1;

/* Sets the material pass and matrix constants for an interface draw. */
// @retail 0x23ea80
bool function_23ea80(long tag, transform4x3f const *matrix, long stage, long pass, long variant)
{
	long *tag_index_ptr = &tag;
	(void)&stage;
	(void)&pass;
	(void)&variant;
	bool result = false;
	dword kind = ((dword *)g_4e3b44)[(short)(*tag_index_ptr) * 4];
	long const *reference;
	if (kind == 0x5052544d || kind == 0x70727433)
		reference = function_137bd0((*tag_index_ptr))->function_x947334();
	else
		reference = *(long const **)(g_4e3b44[(*tag_index_ptr) & 0xffff].bytes + 0x24);
	byte *groups = *(byte **)(g_4e3b44[*reference & 0xffff].bytes + 0x5c);
	long first = **(word **)(groups + 4) & 0x1ff;
	long second = (*(word **)(groups + 0xc))[first + stage] & 0x1ff;
	long shader_tag = *(long *)(*(byte **)(groups + 0x14) + (second + pass) * 10 + 4);
	byte *shader = *(byte **)(*(byte **)(g_4e3b44[shader_tag & 0xffff].bytes + 0x20) + 4);
	g_470a2c = 1;
	g_4b8348 = 0;
	D3DDevice_SetRenderState(D3DRS_STIPPLEENABLE, 0);
	real constants[12];
	constants[0] = matrix->forward.i;
	constants[1] = matrix->left.i;
	constants[2] = matrix->up.i;
	constants[3] = matrix->position.x;
	constants[4] = matrix->forward.j;
	constants[5] = matrix->left.j;
	constants[6] = matrix->up.j;
	constants[7] = matrix->position.y;
	constants[8] = matrix->forward.k;
	constants[9] = matrix->left.k;
	constants[10] = matrix->up.k;
	constants[11] = matrix->position.z;
	D3DDevice_SetVertexShaderConstant(78, constants, 3);
	function_1bbf0((*tag_index_ptr), 0, pass, variant, 0, 10000.0f);
	function_1cf50();
	long binding_tag = *(long *)(shader + 0x100);
	long entry = stage + 7;
	if (binding_tag != NONE)
	{
		byte *bindings = g_4e3b44[binding_tag & 0xffff].bytes;
		if ((dword)entry < *(dword *)(bindings + 4) &&
			(dword)(*(long *)(*(byte **)(bindings + 8) + entry * 0x1c + 0xc) >> 4) > 0)
		{
			result = true;
			function_1c590((s_shader_cache *)g_51f0f0, binding_tag, entry);
			if (g_470a38 && g_470a39)
				function_1ccb0(43);
			else
			{
				function_1c6b0(g_51f0f0);
				*(byte const **)(g_51f0f0 + 0x8c) = g_43f77a;
				if (*(byte const **)(g_51f0f0 + 0xcc) != g_43f77a)
					g_51f0f0[0x20c] = 1;
				function_1c710(g_51f0f0);
			}
			function_1c710(g_51f0f0);
			function_23e460();
			function_23ecd0();
		}
	}
	return result;
}

static inline dword *push_vertex_data4f(dword *push, long slot, real a, real b, real c, real d)
{
	push[0] = D3DPUSH_ENCODE(0x1a00 + slot * 16, 4);
	((real *)push)[1] = a;
	((real *)push)[2] = b;
	((real *)push)[3] = c;
	((real *)push)[4] = d;
	return push + 5;
}

static inline dword *push_vertex_data2f(dword *push, long slot, real a, real b)
{
	push[0] = D3DPUSH_ENCODE(0x1880 + slot * 8, 2);
	((real *)push)[1] = a;
	((real *)push)[2] = b;
	return push + 3;
}

static inline dword *push_vertex_data4ub(dword *push, long slot, dword value)
{
	push[0] = D3DPUSH_ENCODE(0x1940 + slot * 4, 1);
	push[1] = value;
	return push + 2;
}
// @retail 0x23ed30
void function_23ed30(point3f *a, point3f *b, real c, real d, dword color, real e, real *f, real *g, real *h)
{
	dword *push;

	if (g_470a38 && function_015d00(0x2e, &push))
	{
		push = push_vertex_data4f(push, g_470a3c[0], b->x, b->y, b->z, 1.f);
		push = push_vertex_data2f(push, g_470a3c[1], e, 0.f);
		push = push_vertex_data4f(push, g_470a3c[3], a->x, a->y, a->z, 1.f);
		push = push_vertex_data2f(push, g_470a3c[4], c, d);
		push = push_vertex_data4ub(push, g_470a3c[7], color);
		push = push_vertex_data4f(push, g_470a3c[2], f[0], f[1], f[2], f[3]);
		push = push_vertex_data4f(push, g_470a3c[5], g[0], g[1], g[2], g[3]);
		push = push_vertex_data4f(push, g_470a3c[6], h[0], h[1], h[2], h[3]);
		*push++ = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
		*push++ = 8;
		*push++ = D3DPUSH_ENCODE(D3DPUSH_INLINE_ARRAY, 8) | D3DPUSH_NOINCREMENT_FLAG;
		*push++ = 0;
		*push++ = 0;
		*push++ = 1;
		*push++ = 0;
		*push++ = 0x10001;
		*push++ = 0;
		*push++ = 0x10000;
		*push++ = 0;
		*push++ = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
		*push++ = 0;
		D3DDevice_EndPush(push);
	}
}
