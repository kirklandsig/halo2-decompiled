// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "projection_storage.h"
/* UNKNOWN_23D970.CPP: a view state and its default direction, player and
   object searches, and the vertex shader constant table / full screen quad
   push buffer writers (batch 55-1) */

#include "unknown_11c920.h"
#include "vector_angles.h"
#include "vector_products.h"
#include "frame_storage.h"
#include <xtl.h>
#include <math.h>
#include <string.h>
#include "globals.h"
#include "data_array.h"
#include "object_iterator.h"
#include "effects.h"
#include "slot_handler.h"

__declspec(noinline) long function_1366d0(short width, short height, short depth, short format, short alignment, long mipmap_count);

PRIVATE const dword g_4507c8[24] =
{
	31, 19, 27, 32, -1, -1, 17, -1, 16, 29, 30, 18,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, 23
};

typedef void *(__stdcall *texture_allocate_proc)(long size, long alignment);

// @retail 0x23e340
D3DTexture *function_23e340(long width, long height, short format,
	texture_allocate_proc allocate, long *size, void **data, D3DTexture *header)
{
	long bytes = function_1366d0((short)width, (short)height, 1, format, 0x40, 0);
	long aligned = (-bytes & 0x7f) + bytes;
	D3DTexture *result = header;
	void *buffer = allocate(aligned, 0x80);
	if (header && buffer)
	{
		dword texture_format = g_4507c8[format];
		header->Lock = 0;
		header->Size = (((((dword)width * 4) >> 6) - 1) << 24) | ((height - 1) << 12) | (width - 1);
		header->Format = (texture_format << 8) | 0x10029;
		header->Data = (dword)buffer & 0x0fffffff;
		header->Common = 0x40001;
	}
	else
	{
		buffer = 0;
		result = 0;
	}
	*size = aligned;
	*data = buffer;
	return result;
}

/* ---- types ---- */

/* where a camera is and which way it faces */
struct s_view_point
{
	point3f position;
	real yaw;
	real pitch;
};

/* the yaw and pitch that face along a direction */
static __forceinline void view_face_direction(s_view_point *view, vector3f const *direction)
{
	view->yaw = (real)atan2(direction->j, direction->i);
	view->pitch = (real)atan2(direction->k, sqrt(direction->j * direction->j + direction->i * direction->i));
}

struct s_view_state : s_view_point
{
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
bool function_015d00(dword **out, long count);

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
/* the free camera's start */
s_view_point g_51ec14;
short g_51ec3c;
byte g_470a20 = 2;
extern real g_54e854;
extern point3f *g_51ec28;
byte g_4e61b9;
s_input_state g_4e61dc[3];
s_input_state g_4e630c;
byte g_485af0;
long g_470a3c, g_470a40, g_470a44, g_470a48, g_470a4c, g_470a50, g_470a54, g_470a58;
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
			s_camera_start_23cea0 const *start = scenario->points;
			g_51ec14.position = start->position;
			g_51ec14.yaw = start->yaw;
		}
		else
			memset(&g_51ec14, 0, sizeof(g_51ec14));
	}
	g_51ec13 = 1;
	vector3f forward;
	direction_from_angles(&forward, g_51ec14.yaw, g_51ec14.pitch);
	state->position.y = 0.f;
	state->position.x = 0.f;
	state->yaw = 0.f;
	state->pitch = 0.f;
	state->unknown14 = 0.f;
	state->unknown18 = g_54e854;
	state->position = g_51ec14.position;
	view_face_direction(state, &forward);
	if (!temporary)
		g_51ec28 = (point3f *)state;
	if (g_51ec3c)
		g_44ab84[g_51ec3c].report_state(state);
	g_51ec11 = g_470a20;
	memset(&g_5022f8, 0, sizeof(g_5022f8));
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
		kept->position = g_5022f8.position;
		view_face_direction(kept, &g_5022f8.forward);
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
	g_51ec14.position = g_5022f8.position;
	view_face_direction(&g_51ec14, &g_5022f8.forward);
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
	state->position = g_5022f8.position;
	view_face_direction(state, &g_5022f8.forward);
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

	state->position.x = 0.f;
	state->position.y = 1.f;
	state->position.z = 0.f;
	view_face_direction(state, &g_5022f8.forward);
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

PRIVATE __forceinline s_player *observer_next_player_23dba0(s_record_pool *players, long *index, long *datum)
{
	if (!observer_next_index_23dba0(players, index) || *index == NONE)
		return NULL;
	s_player *player = (s_player *)(g_4e8c24->data + players->size * *index);
	*datum = (player->identifier << 16) | *index;
	return player;
}

// @retail 0x23dba0
bool function_23dba0(long player_index)
{
	s_record_pool *players = g_4e8c24;
	long team = PLAYER(players, player_index & 0xffff)->team;
	long index = NONE;
	bool result = false;

	long datum;
	s_player *player;
	while ((player = observer_next_player_23dba0(players, &index, &datum)) != NULL)
	{
		if (datum != player_index && player->team == team)
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x23dc40
long function_23dc40(long player_index, long last_index, bool same_team)
{
	s_record_pool *players = g_4e8c24;
	long team;

	if (same_team)
		team = PLAYER(players, player_index & 0xffff)->team;
	else
		team = NONE;
	long result = NONE;
	long index = NONE;

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
void function_1f087b(vector3f const *a, vector3f const *b, vector3f *result); /* inline in src/unknown_1f03e0.cpp */

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

PRIVATE __forceinline long camera_round_23de50(real value);

PRIVATE inline long camera_round_23de50(real value)
{
	long result;
	__asm fld value
	__asm fistp result
	return result;
}

PRIVATE inline s_player_state *local_player_state(long local_index)
{
	s_player_state *result = NULL;

	if (local_index != NONE && g_4686c4 != NONE)
	{
		s_player_4e9bd4 *player = &g_4e9bd4[local_index];
		result = &player->state;
	}
	return result;
}

// score 45 (was 45): the second product through the existing staged helper of src/unknown_1f03e0.cpp
// (function_1f087b, every component computed before the stores, i first) gives retail's previous_up
// loads (j, i, then k into xmm0 after the distance store) and folds forward's loads in the second
// product. Left: the first product's right.i/right.j colours (ours xmm1/xmm2, retail xmm2/xmm1), the
// second product's schedule (retail computes k, j, i) and the else branch's forward stores (through
// edi here, ecx in retail).
// MiniMax H (11-12): no new form, no lower score (45): what is left is the register colour of the first
// product's operands (ours xmm1/xmm2, retail xmm2/xmm1), the second product's schedule and the else arm's
// forward stores, all of them inside the shared helpers (include/vector_products.h and
// src/unknown_1f03e0.cpp), whose bodies are not mine to change.
// MiniMax R (11-11): no new form, no lower score (45); the reduced image the fast check builds compiles
// this function differently from the full program (255 with 1224 bytes against the full build's 45 with
// 1262 bytes), so the fast number no longer agrees with the full build and the plain check is the one to
// trust when ranking variants here. A permuter run (198 tries in 2407 s, 10 sources in its reduced image)
// confirms it: it scored the original 227 on the reduced image and 44 in the full image, and none of its
// 198 rewrites beat 44 in the full image.
// MiniMax E (11-10): no new form, no lower score (45); the first cross product loads its two operands into the other pair of registers than retail (here xmm1/xmm2, there xmm2/xmm1), which moves the result of every later product of the second cross, and in the else arm retail keeps the three forward components on one pointer where this build stores the last two from the command pointer.
// MiniMax C (11-09): no new form, no lower score (45); the first cross product still takes its two temporaries in the other pair of registers (ours xmm1/xmm2, retail xmm2/xmm1) and retail keeps the k term in xmm4 with a movaps where this build reloads it from memory.
// MiniMax C (11-06): no new form, no lower score.
// MiniMax R (11-04): no new form, no lower score.
// MiniMax C (11-09): no new form, no lower score (45); the first cross product still takes its two temporaries in the other pair of registers (ours xmm1/xmm2, retail xmm2/xmm1) and retail keeps the k term in xmm4 with a movaps where this build reloads it from memory.
// MiniMax C (11-06): no new form, no lower score.
// MiniMax R (11-04): no new form, no lower score.
// Before round 127: camera-basis register allocation differed. Retail loads previous_up.k into
// xmm0 after storing the distance and holds right.i/j/k in xmm2/xmm1/xmm0 (ours loads it before
// the store, into xmm1, and right lands in xmm2/xmm0/xmm1); retail computes the second cross
// product staged (all products, then the stores through eax), copying right's components
// (movaps, then a memory multiply). The else branch stores forward.j/k through ecx in retail and
// through edi here. Kept: cross3f for the second product 57 (was written out by hand).
// Tried: cross3f_staged for the second product 217 (1270 bytes; aligned 37 from 44, retail's
// reads and stores, u.k/right.i colours swapped); both products staged 221; previous_up read
// after the forward copy 268, before the distance 241; direction_from_angles in the else branch
// 224 (angles read once), with cross3f_staged 220; a forward pointer local in the else branch 55
// (the else branch is then retail's), 225 aligned 43 in an earlier build, with the staged
// product 217 [35: only previous_up.k's register is left], in both branches with the staged
// product 221, on that right or direction declared first 217/217, both products staged 221, the
// first product reading direction 223; staged first product 222; direction declared before
// previous_up 57; the first product taken from the direction local 217 (1261 bytes); the
// distance in a named local stored after the forward copy or after the first product 227/233,
// the same on the staged/pointer form 221/232; previous_up copied after the direction
// difference 99, on the staged/pointer form 241.
// Round 22 (one-file harness with the callee files, exact: 58 for the build's 57): the first
// product's operand order in each component, all 8 orders, 58 each and 217 each with the staged
// second product (identical code: the products are canonicalised); first product from the
// direction local with the staged second product 223, with both products from direction (cross3f
// or staged second) 229/229; up staged into a local, then copied 253 (1290 bytes).
// Lane 17 r1: right declared first 57; const previous_up 57; a forward pointer in both branches with
// cross3f 325 (1268 bytes); previous_up copied after the direction difference 98; staged second product
// with the else-branch forward pointer 217 (1270 bytes): retail's staged order (k, j, i, then the stores)
// but right's colours follow previous_up.k, which ours loads into a free register before the distance
// store where retail loads it into xmm0 after that store.
// Lane 13 round 18 (base: staged second product + forward pointer in the else branch, 217 at
// 1270 bytes, 35 aligned): both products staged 221; previous_up copied after the direction 241;
// first product from the direction local with a plain second product 217 (1261 bytes, 47
// aligned); the distance stored after the products 224 (it then holds xmm0 across them). A
// compile-only build of the file reproduces the first product's colours (u.k in xmm2 even with
// the distance on x87); there u.k takes retail's xmm0 only when no distance store sits between
// the call and the products (dropped or moved after them: 19-21 differing SSE lines against 33);
// none of 48 helper/operand/copy-placement combinations does better than the base.
// Round 322: Complete scalar up snapshot i/j/k permutations all 107; memcpy snapshot 57; memmove 327 at retail length. Original aggregate copy, products and body restored; up K/distance scheduling remains.
// Round 127 (lane 4): cross3f_staged for the second product 217 again (1270 bytes; aligned, only
// the first product's colours (right.i/k swapped), the second product's operand order (ours loads
// forward's component and multiplies right's register, retail copies right's register and
// multiplies forward from memory) and the else branch's stores remain); on it: a force-inline
// helper reading the angles through the observer and writing through the output pointer (as the
// matched 0x23d790 builds its forward vector) 221, alone 222; a forward pointer local for the copy
// and both products 221; the distance in a named local 217 (same code). Diagnostic only: one no-op
// product on the distance before the staged products 55 at 1262 bytes (the staged code shrinks by
// 14 bytes), two/three 203/223: the operand orders follow VC's tuple numbering, as in 0x293130.
// Round 46 (lane 9): a forward pointer in the camera branch only (copy and both products through it) 221
// (1273 bytes), with the staged second product 221 (1272); the second product reading the direction local 224
// (1268), staged 224; the player state named before the previous_up copy 57 (identical). On the base the first
// product is retail's sequence with up.k in xmm1 (loaded before the distance store) where retail loads it into
// xmm0 after the store, so right's j/k colours swap and the second product is not staged.
// Round 111 (lane 6, fast): command declared before observer 57; the staged second product reading forward through
// a second view of the command (`((s_follow_output_23de50 *)output)->forward`) 55 (1262 B: the second product then has
// retail's operand order and stores), with the view for the first product only 55; that plus a forward pointer in
// the else branch 53 (else branch retail's; only previous_up.k in xmm2 loaded before the distance store, retail xmm0
// after it, swapping right.i/right.k, remains); on 53: previous_up copied after the direction difference 96, after
// the forward copy 128, between the distance and the copy 100, right declared first 55, const previous_up 53, staged
// first product 54 (1264 B), first product from direction 220, the up output through the view 53, five declaration
// orders of previous_up/direction/right 53 each.
// Round 33 (lane 10): retail's second product is exactly cross3f_staged's order (k, j, i, then the stores),
// 217 at 1270 bytes again (up.k then in xmm2); the field-of-view copy moved to the start of the camera
// branch 248 (1261 bytes), with the staged product 114 (1262); the else-branch forward pointer 55 again,
// with the staged product 217; the distance in a named local stored at once 57 (identical). A compile-only
// /FAs build reproduces our camera colouring but returns the distance on x87 (xmm0 stays free there).
// Round 114 (lane 6, fast): right or direction declared at function scope 57/57; the second product's
// output through the output view (plain cross3f) 57; the forward copy as memcpy 57; the else branch's
// cosine declared at function scope 57 (all identical code).
// Round 133 (fast): the direction through the existing scaled_difference3f with a 1.0f scale 223 (1266
// bytes); the forward copy field by field 232 (1274 bytes), as memcpy 57 (identical); memcpy with
// cross3f_staged for the second product 217 (1270 bytes), for both products 221 (1272). The 3 extra bytes are
// the second product's three `movss x, [forward]; mulss x, right` pairs, where retail copies right's register
// and multiplies forward from memory (one byte shorter each). A one-file harness with the callee files gives
// 362 at 1246 bytes here (not exact).
// Round 137 (lane 4): else branch through a named forward pointer 55 (else branch retail's); the second product as
// cross3f_staged through a `vector3f const *forward` local 221 (1272 bytes), with the else pointer 221; on the else
// pointer: a named up pointer for the second product and its normalize 222 (1273), the first product staged 222.
// Round 59 (lane 12, fast): the first product reading the direction local with the forward copy
// after it 217 (1261 bytes, every product register moves), with cross3f_staged for the second
// product 223 (1266); either with a forward pointer in the else branch 217/223; the else branch
// without the cosine local (cos(pitch) at both uses) 238 (1284 bytes).
// Round 28 (m3 lane 2, fast): the right local renamed to sort first 57 (names do not move the
// colours); previous_up declared at function scope 57, with cross3f_staged for the second product
// 217 (1270 bytes: our colours are then retail's with xmm0 and xmm2 exchanged, right.i in xmm0 and
// previous_up.k/right.k in xmm2); the first product written into the up output, then cross3f_staged
// in place 252 (1291 bytes, the first product's stores stay); the second product into an up local
// then copied 253 (as listed); right, both products and the up normalize in a force-inline helper
// (staged second product) 221 (1272 bytes).
// Round 42 (lane 11, fast): the side vector built in command->up and crossed in place with
// cross3f_staged (cross3f/staged first product) 252/243 (1291/1289 bytes: the first product's stores
// stay and the second reloads them); the staged second product with each product's operands swapped
// (b * a) 217 (1270, canonicalised like cross3f_staged); the first product from the direction local
// 217 (1261, again); a forward pointer for the copy and the first product with the staged second
// product reading command->forward 217 (1270), the pointer taken after the copy 217 (propagated).
// Retail's second product treats forward's loads as values newer than right's (as the listed second
// view of the command gives), so something between the products separates them; no plain form did.
// Round 65 (lane 12, fast): the second product in place on the output (first product into command->up,
// then cross3f_staged from it into it, no right local) 252 (1291 bytes); a cross returning its output
// pointer passed straight to the normalize 57 (same code); the first product as a cross returning the
// vector by value 303 (1332 bytes); the second product staged in place on right, then copied to the
// output 260 (1296 bytes); the distance stored through a pointer to its field 57 (same code).
// Round 58 (lane 16, fast): both products cross3f_staged through command->up (first into up, second
// in place) 243 (1289 bytes: up stored and reloaded); `cross3f(&forward, &previous_up, &right)` then
// `cross3f(&right, &forward, &up)` (the same up) 57: previous_up's loads become retail's (j xmm3, i
// xmm4, k xmm0 after the distance store), but the first product's subtractions are then negated;
// that with cross3f_staged for the second product 59 (1262 bytes); the cross3f body behind a (b, a)
// parameter order (diagnostic) 57 (identical: only the formula's load order moves previous_up.k);
// right negated after the forward-first product (diagnostic) 256 (the negation is not folded); on the
// forward-first form: the first product as cross3f_staged 57, previous_up copied after the direction
// difference 249 (1272 bytes).
// Round 15 (lane 17, fast): a cross helper returning its result pointer, the second product passed
// straight to the normalize 57 (same code), both products through it nested with staged components
// 221 (1272 bytes), plain components nested 57; a player-state pointer local for the previous_up copy
// 57; previous_up declared at function scope 57. Diagnostics: the first product's components in j, i,
// k order (up.k named first) with the staged second product, written out 218 (forward read through
// edi), through a pointer helper 219 (up.k then in xmm2, loaded before the distance store).
// Round 17 (lane 17, fast): both products staged with the first in place into the previous_up copy 222
// (1270 bytes); the else branch through a force-inline helper reading the angles through the observer and
// writing through its forward pointer (0x23d790's form) 222 (1273 bytes: the else branch is retail's, but
// the first product recolours, up.k xmm2, and the second loses its memory operands), with cross3f_staged
// for the second product 221 (1272); the camera basis as a force-inline helper with pointer parameters 296
// (1267, frame differs); `right` renamed `left` 57 and previous_up/direction renamed up/a_direction 57
// (identical code: local names do not move these registers).
// Round 127 (lane 5, fast): diagnostic, round 111's second-view staged product with the else-branch
// forward pointer re-measured 53 (1262 bytes; up.k and right.i swap xmm0/xmm2), and on it the first
// product written out in each statement order ijk/ikj/jik/jki/kij/kji 54/54/216/217/56/217; `advance`
// declared at its block 340 (1261 bytes; retail clears it as a stack byte at entry); the gamepad delay
// as a ternary 57 (same code); the delay update through a named remaining local 111 (1261 bytes).
// Then (plain unless noted), existing out-of-file cross helpers for the second product: function_1f087b
// 45 (kept), scripted_cross_product 57, cross_11e130 57, function_25a131 229 (1277), vector_basis_cross
// (staged k first) 217 (1270); for both products scripted_cross_product 57, function_1f087b 49 (1264);
// with function_1f087b second, the first through scripted_cross_product 45, cross_11e130 45,
// function_25a131 215, vector_basis_cross second with function_1f087b/scripted first 221/217. Same-file
// staged second products by statement order (diagnostic, fast = plain): ijk/jik/jki 45, ikj/kij/kji 217
// (computing k before j recolours the first product); on 45 the first product written out in the five
// other plain orders 56-63 (1261-1262), staged in any order 49 (1264); on 45 right declared first,
// direction declared first, const previous_up 45 each; a forward pointer in the else branch 43 (the
// else branch is then retail's; one-use pointer, kept only on a match).
// Round 129 (lane 5, fast; `--fast` keeps the cross-file function_1f087b out of line (1224 B), so these use a
// same-file copy, which gives the base's 45): the existing direction_from_angles in the else branch 156 (1264 B).
// Second product as a staged helper, each row one expression (E) or accumulated (`x = p; x -= q;`, A), all six row
// orders x eight styles: ijk EEE/AEE, jik EEE/EAE, jki EEE/EEA 45; k-first or k-before-j orders with the k row
// accumulated 55 (1262 B: retail's own k, j, i order no longer recolours to 217, but previous_up.k loads into
// xmm2 before the distance store and right.i/right.k swap xmm0/xmm2); the rest 217/222. First product as a
// helper with per-row styles: on the base, direct ijk EEE 45, staged EEE 49, any accumulated row 214-225; jik
// EEE 59 (1261 B), jik AAE/AAA 61, jki EEE/EEA/EAE 63 (1261 B); on the kji/k-accumulated second product,
// direct EEE 55, staged EEE 56, the rest 203-221.
// Round 127 (lane 6, fast): the second product into a local up, then copied before the normalize 253 (1290
// bytes), normalized in the local and then copied 254 (1292); previous_up through the out-of-line player state
// getter 367 (1244: retail inlines it here); diagnostic: the distance not stored 218 at retail's 1261 bytes (the
// three bytes go, but the second product keeps our operand order: the store does not set it).
// Round 151 (lane 4, fast with 0x1f0870 listed so the helper inlines; without it fast builds 1224 bytes): the
// one extra byte is the second product's second `movss xmm, [forward.k]` load (retail loads forward.k once and
// copies right's registers). Products (first, second): function_1f087b then cross3f 222 (1273 bytes), then
// cross3f_staged 221 (1272); forward-first cross3f (forward x previous_up) then function_1f087b(right, forward)
// 224 (1270); the first product from the direction local with function_1f087b second 43 (1261 bytes, first
// difference +0x200, but the first product then reads the direction's stack copy and interleaves with the
// forward copy where retail reads command->forward after it: a different shape, not kept); both products from
// the direction local with function_1f087b second 229. On the 43 form: right declared first 43, const
// previous_up 43 (same code each); the second product through scripted_cross_product 217, through cross_11e130
// 217 (1261 bytes each).
// Round 24 (lane 17, fast with 0x1f0870's file, which the reduced image needs to inline function_1f087b; it then
// agrees with the full build at 45): diagnostic dead temps at the top (period 8, in pairs) on this body 45/45/49/49/
// 219/219/49/49 for 0-7 (219 at 1261 bytes is a shifted allocation that also loses previous_up's load order), on
// cross3f_staged for the second product 217/217/221/221/223/223/221/221, on the else-branch forward pointer 43/47/
// 47/219/219/47/47/43 (the pointer counts one), real-typed dead temps 45/49/49: the temp count has four states here
// and none gives retail's right.i/right.j colours. function_1f087b reading forward through a second view of the
// output 216 (1265 bytes). No existing direction-from-angles helper takes this observer through a pointer.
// Round 50 (lane 14, fast with 0x1f0870/0x23d790): `right` in a local two-field aggregate or as element 0 of a
// two-element array 45/45, element 1 216 (1265 bytes), the same with the staged second product 217/217/217; both
// products through named component locals in all 36 row orders 228 (1272 bytes each: the order does not reach the
// code); both products staged re-measured 221 (1272); the second product into a local vector copied to the output
// field by field, or a local two-field struct, through cross3f, staged or function_1f087b, 232 each (1272); the first
// product into a local copied to `right` field by field, through any of the three, 49 (1264), with the staged second
// product 221 (1272). A force-inline difference-of-products helper taking its operands by const reference: second
// product rows i, j, k 58 (1264), staged rows in either order 211 (1261 bytes, retail's length, aligned 35, but up.k
// goes to xmm2 and the rows interleave with the stores); first product through it with function_1f087b 219 (1261,
// aligned 53), with the staged second product 228 (1272).
// Round 510: whole-function forward/up receivers and native trig setter, seven subsets
// 354/349/354/159/354/346/354; all with position receiver 372. Original complete body restored;
// product colors, forward.k reuse and angle receiver stores remain.
// Round 512: Common field-of-view/offset/velocity output tail 228/208/207;
// field-of-view+offset/field-of-view+velocity/offset+velocity 219/219/238; all
// common stores, separate/chained vector copies 246/246. Original complete body
// restored; right i/j colors, forward.k reuse and ordinary-angle bases remain.
// Round 348: complete normalized-forward/offset/velocity setter subsets
// 231/233/265/197/243/244/279; all with prior-up setter capture 313.
// Original aggregate copies and products restored; right i/j carriers and forward.k reload remain.
// M297: Completed direction workspace reused for the first product, direct/staged; prior-up
// workspace reused with the existing staged producers; direct forward normalization with
// direct/staged/k-first first products and prior-up reuse [49, 45, 63, 56, 360, 360, 360, 364]. Original workspaces restored.
// Round 482: Native typed prior-up capture with unchanged/k-first/i-first first product,
// k-first second/both products, direction first input, forward setter and angle receiver
// gives 45/49/49/217/221/43/231/43. Typed layout and complete body restored; two-point
// forms rejected. Right i/j carriers, forward.k reuse and ordinary-angle stores remain.
// J239: prior-up setter 252; published-forward snapshot 226; both captures 258; forward setter 231; paired setters 245; paired aggregate capture with k-first product 258; paired setters with k-first product 245; paired setters with i-first product 245.
// Original input snapshots, products and body restored.
// M320: First product native/const/library/overlap-safe result copies 289/289/289/391;
// each with the existing staged second producer 301/301/301/400. NOT kept: native copies require a match.
// Original frame, product inputs and complete body restored; colors and forward.k reuse remain.
// Round 43 (m3 lane 2, fast with 0x1f0870): the second product from the direction local 224 (1268 B);
// previous_up copied after the normalize 94; right from the direction before the forward copy 45 (1261 B,
// both products then read stack copies), with the else-branch forward pointer 43; forward copy first, right
// from direction, else pointer 41 (1261 B; products read the stack copy where retail reads command->forward).
// None kept (gains under 3, different shape).
// Round 205 (m3 lane 3, fast with 0x1f0870): previous_up copied straight into `right` and crossed in place with
// cross3f_staged 56 (1262 B); in place with function_1f087b 63 at retail's 1261 bytes: the second product then has
// retail's form (copies of right's registers, forward multiplied from memory) but the first product starts with
// the j row and previous_up.k loads before the distance store; that copy made after the forward copy 130 (frame
// moves); the in-place first product with cross3f for the second 217; the in-place form with the else-branch
// forward pointer 216 (the camera colours move again).
// Round 64 (lane 10, plain): a forward pointer in the else arm 43 (retail's ecx-based j and k stores; two points, not kept without a match). On that form the second product k first, as retail schedules it:
// cross3f_staged 217 (1270, masked aligned 9), the global force-inline k-first helpers of src/unknown_2900b0.cpp and src/impacts.cpp 217 (1270, masked 9), the same with 0x1f087b's or the k-first helper for the first
// product 221 (1272, masked 13); written out k, j, i with retail's operand sides 225 (1270, masked 17), through a forward pointer 221 (1272, masked 15). All move up.k's load ahead of the distance store and rotate the first product's colours.
// Round 68 (m3 lane 10, fast with 0x1f0870 and each helper's own file listed): untried out-of-file cross helpers.
// Second product through 0x206090's k-first helper (copies the second operand, stores k, j, i as it goes) 218 (1270
// bytes: previous_up.k loads before the distance store again), through 0x441b0's staged k-first helper 217 (1270),
// through 0x2922b0's staged i-first helper 45 (same code as function_1f087b). First product (with function_1f087b
// second) through 0x1f2a80's, 0x1703f0's or 0xf4800's plain inline cross 45/45/45 (same code), through 0x441b0's
// staged helper 49 (1264). Original calls restored.
// Round 82 (m3 lane 16, fast with 0x1f0870): the distance named before its store 45 (same code); one function-scope
// forward pointer for both arms 322 (1266 bytes, prologue changes); named yaw/pitch locals in the else arm 157 (1264);
// right declared at function scope 45 (same code); the cosine computed before the distance store 49.
// DL258: Actual output-basis pair, saved-point overlay and input-step fields,
// all seven nonempty families, 45 each; direct native saved-point member alone 45.
// NOT kept: no gain. Right I/J colors, repeated forward K and ordinary-angle store
// bases remain. Original record layouts, snapshots, products and complete body restored.
// B417: Prior-up/normalized-direction/right last-consumer regions, seven subsets then complete
// geometric workspace names: 45/45/45/45/45/45/45/45 at 1262/1261. Original body restored;
// right i/j carriers, second-product forward.k reuse and ordinary-angle store bases remain.
// B419: Native geometric-basis ownership of prior-up/normalized-forward/perpendicular-left,
// seven role subsets then complete forward-setter graph: 70/77/45/70/70/77/70/249. NOT kept: grouped
// workspace does not match. Original scalar/vector owners, helpers and complete body restored.
// B423: Native reset-completion decision at the geometric branch, ready/waiting
// bool/byte/short/long, 45 each at 1262/1261. NOT kept: no exact match;
// original direct comparison, staged helper and complete body restored.
// B426: Existing reference direct/staged and pointer direct/staged cross producers,
// each at the first/second product: 45/57/49/45/45/57/49/217. Original shared producers and
// complete body restored; right i/j colors, forward.k reuse and angle stores remain.
// B428: Complementary input-result lifetime, bool/byte/short/long with conditional/total
// predicate producers: 74/75/74/75/95/96/89/103. NOT kept: no exact match; stack initialization
// and lengths regress. Original complete body restored; right i/j colors, forward.k reuse
// and ordinary-angle store bases remain.
// B430: Counted component-array prior-up/forward/default-vector transfers, masks
// 1/2/3/4/5/6/7: 106/233/272/259/275/254/286; all with chained equal defaults 300.
// NOT kept: no byte match; original aggregate transfers, products and body restored.
// M391: Normalized-distance/delay-subtraction/transition-clamp double receivers, seven subsets, then complete real receivers: 227/114/240/135/245/150/256/103.
// NOT kept: No exact product-color, forward-K reuse or angle-store-base repair; nonmatching result lifetimes are discarded. Original complete body restored.
// B432: Caller-visible inline declarations for the existing staged cross, normalizer
// and perpendicular producers, seven subsets, then staged-cross forceinline: eight 45s.
// Original declarations and callback restored; right i/j, forward.k and angle bases remain.
// B434: Missing-object/missing-target/ordinary-orientation arms first, seven
// subsets 361/219/222/250/367/333/337; nullable acquisition with common fallback 358.
// NOT kept: all regress; original branches, products and full body restored.
// F393: Masked target ordinal long/dword/int/word, then actual reset-capability decision bool/byte/short/long:
// 45/45/45/185/55/55/55/45. NOT kept: no byte match; original complete body/helpers restored;
// right i/j carriers, forward.k reuse and ordinary-angle store bases remain.
// Round 54 (m3 lane 15; a one-link harness with 0x30bf0, 0x11d090, 0x16f3a0, 0xbadc0, 0x172520, 0x1f0870's files and an address-taken
// entry is exact, 45/43/53/217 as listed, 4.6 s a form): first product written out 219 (1261 bytes), with `(real)` on row i's second, first
// or both products 219/216/224, on the whole row 219, with parentheses on all rows 224, row i as a negated sum or difference 219/232;
// first product through a local helper by row order ikj 57, jik 59 (1261), jki 63 (1261), kij 56, kji 56; no command local, no observer local,
// neither, crossed with function_1f087b, staged and plain second products and the else pointer: 43 to 222, none below 45 without that
// pointer. Diagnostic: 0-7 identity locals before the first product by 0-7 before the second, with function_1f087b or staged: best 45,
// best at 1261 bytes 64.
// B432: Caller-visible inline declarations for the existing staged cross, normalizer
// and perpendicular producers, seven subsets, then staged-cross forceinline: eight 45s.
// Original declarations and callback restored; right i/j, forward.k and angle bases remain.
// B434: Missing-object/missing-target/ordinary-orientation arms first, seven
// subsets 361/219/222/250/367/333/337; nullable acquisition with common fallback 358.
// NOT kept: all regress; original branches, products and full body restored.
// M391: Normalized-distance/delay-subtraction/transition-clamp double receivers, seven subsets, then complete real receivers: 227/114/240/135/245/150/256/103.
// NOT kept: No exact product-color, forward-K reuse or angle-store-base repair; nonmatching result lifetimes are discarded. Original complete body restored.
// Round 151 (harness, exact at 45): grid of 200 forms, second product (function_1f087b, cross3f_staged, staged through a view of the output, k accumulated, k accumulated through a view) by
// first product (cross3f, function_1f087b, written jik, kji, ikj) by the else-arm forward pointer by 0-3 dead temps ahead of the first product: best 43 at 1262 bytes (the else pointer alone), none lower.
// Random grid of 143 natural forms (those products, the else pointer, a named up or forward pointer, declaration orders, a named distance, advance and command placement): none below 45.
// Y610: Paired angle/forward/up field lvalue identities,
// subsets 1/2/4/3/5/6/7, then all with command-record identity:
// 230/354/349/354/348/354/377/377 (fast control 45).
// NOT kept: no exact identity-form match; original complete body restored.
// Round 57 (m3 lane 2, `--fast 0x23de50 0x1f0870`, 45 as the full check): a camera-arm forward pointer for the copy and
// both products 223 (1272 bytes), with the else-arm forward pointer too 223; an up pointer for the second product and its
// normalize 45 (same code), with the else-arm forward pointer 47 (1264 bytes); the else-arm forward pointer with direction
// declared before previous_up 43 (as the pointer alone).
// P386: Default offset / velocity / both via memcpy or std::copy; both memmove; complete std::copy forward:
// 45/45/45/197/192/238/249/259. NOT kept: ties or regressions; no exact native transfer result.
// Right i/j carriers, forward.k reuse and angle store bases remain; original body/includes restored.
// P398: Existing first-cross/difference complete scalar source ownership,
// prior-up/forward/difference subsets then immutable complete inputs:
// 49/226/223/226/222/220/70/70.
// NOT kept: all regress; no exact input-copy match. Original shared providers and body restored;
// right i/j carriers, forward.k reuse and ordinary-angle store bases remain.
// Round 100 (m3 lane 12, `--fast 0x23de50 0x1f0870`, 45 as the full check): forward copied first and normalized in place for the distance 331; the
// direction difference written out k, j, i 56 (1264 bytes); the velocity default copied before the offset in the camera arm 47; direction declared
// first with previous_up assigned after its declaration 45 (same code); the second product reading the direction local 224 (1268 bytes). All restored.
// Round 84 (m3 lane 1, `--fast 0x23de50 0x1f0870`, 45 as the full check; aligned count in brackets, base [20], else pointer 43 [18]):
// the second product's rows as the three by-value arguments of the existing vector3f_assign 223 (1265 bytes) [58], with the else
// pointer 80 (1264) [55] (the rows interleave as retail's do, each right register destroyed at its last use, but forward is read
// through edi and the first product recolours), the same with retail's operand sides 80; the first product through that setter
// with the else pointer 49 (1264) [23], both products 79 [55]; the setter rows read through a forward pointer local 221 (1272),
// with the else pointer 221 (the code of r137's const forward pointer form); a pointer-parameter cross built on that setter for the
// second product 217 (1270) [27], with the else pointer 217 [25], with retail's operand sides 217/217, for both products 221/221
// (1272) (cross3f_staged's code each time), for the first product only 49/47 (1264). Re-measured r205's in-place prior-up copy
// (one local for both products) 63 (1261) [28] and with the else pointer 216 (1261) [30]; new on it: the in-place first product
// staged with the else pointer 57 (1264) [28], through the setter cross 56 (1262) [28] (r205's staged 56), with the else pointer
// 57, with retail's operand sides and the else pointer 57. All restored.
// Round 87 (m3 lane 1, `--fast 0x23de50 0x1f0870`, 45 as the full check, aligned/masked 20/8): an inline identity function wrapped
// 1, 2 or 3 times round the target index before the products 45/45/45 (diagnostic, same code: the inline-expansion count does not
// move the product colours here); the target read through the existing object_get 45 (same code); previous_up copied after the
// distance store with cross3f_staged (retail's k, j, i schedule) for the second product 245 (1270 bytes, masked 25); both products
// through cross3f_staged 221 (1272, re-measure of round 84). No untried natural form of the
// two products is left; both differences sit in them (right.i/right.j colours, retail's k-first second product) and in the else
// arm's forward stores.
// Round 164 (m3 lane 6): no untried natural form found. What differs now: the first product's two temporaries swapped
// (right.i/right.j in xmm1/xmm2, retail xmm2/xmm1), the second product's rows scheduled j, k, i where retail does k, j, i with
// copies of right's registers (the extra byte), the else arm's forward.j/k stores through EDI (retail ECX), and address
// fields shifted by the length. Every form for these above is listed with its score. Body unchanged.
// Round 166 (m3 lane 6, `--fast 0x23de50 0x1f0870`; masked = register-masked aligned count, base 12): the else arm's facing
// through vector3f_assign 159 (round 510's trig setter again), through vector3f_assign on a named facing pointer that the
// up call also takes 159 (1270 bytes); the else-arm facing pointer with the first product read from the direction local 41
// at 1261 bytes (first difference +0x200), but masked 35, aligned 51: the first product then reads the stack copy ahead
// of the facing copy, so the lower count is positional only; on it a named up pointer for the second product and its
// normalize 215, the distance in a named local stored after the facing copy 215; a const facing pointer for both products
// (not the copy) 223 (1272 bytes), with the else-arm facing pointer 223; one up pointer ahead of the arms for the second
// product, its normalize and the else arm's up call 255 (1264 bytes).
// M3 lane 9 r109 (`--fast 0x23de50 0x1f0870`, 45 as the full check): no untried natural form of the products left (left: forward.k/forward.j
// in xmm1/xmm2 where retail has xmm2/xmm1, so right.i/right.j and the second product's schedule, and the else arm's forward
// stores through edi where retail forms the forward pointer in ecx first); diagnostic: the second product replaced by a copy of
// right 244 (1234 bytes; it reshapes the arm, so it does not isolate the colours). Kept: the target's position through the existing
// object_get and object_node_matrix (include/slot_handler.h) in place of the getter written out with a header type made up in the
// body 45 (same code; the file's other functions stay matched).
// M3 lane 11 r89 (`--fast 0x23de50 0x1f0870`, 45 as the full check): the first product through the existing contact_cross
// (src/unknown_1f03e0.cpp, cross3f's formula as an external inline) 45 (same code). No other untried natural form found: the
// product rows, staging, helper choice, pointers, copies, declaration orders and dead-temp periods are all listed above.
// M3 lane 13 r81 (fast with 0x1f0870, 45 as the full check; aligned exact/fully masked 88/8): of the 45, about 24 lines
// are calls and global loads after the products flagged only by the 1-byte shift; the masked diff is the first product's
// two temporaries (ours f.k -> xmm1 = right.i and f.j -> xmm2, retail xmm2/xmm1; right.k xmm0 in both), the second
// product's resulting reload of forward.k (`movss xmm4, [ecx+8]`, 5 bytes, where retail copies right.i with a 3-byte
// `movaps xmm4, xmm2`), and the else arm's j/k stores through edi. Retail's second product is cross3f_staged's k, j, i
// order; re-measured cross3f_staged there 217 (1270 bytes: previous_up.k then loads into xmm2 and every colour rotates).
// Retail reads input[2] three times and recomputes the reset bound three times, as this source does, so no form outside
// the products and the else arm keeps the rest of the body; every product and else-arm form found is listed above. Camera
// arm statement orders: the field of view copied before the products 216 (1265 bytes), after the offset/velocity copies 67;
// the timer cleared before the flags 47; velocity copied before offset 47; after the arms, modes[0] before value94 47, and in
// the timer-3 block valuea0 before the distance 47. Same code: velocity copied from command->offset 45; up through the
// existing normalize3d 44 (same instructions; it calls a local copy instead of 0x30bf0, so not kept).
// M3 lane 12 r115 (fast with 0x1f0230): the first product through function_1f087b too 49 (1264 bytes); the else arm's
// forward pointer re-measured 43; on it: the pointer also for the copy and both products 223 (1272), an up pointer
// for the second product, its normalize and the else arm's call 47 (1264), the forward copy through memcpy (listed) 43
// (same code); the else arm's distance copy after the 0x11d090 call 62, after the pitch cosine 49. 0xe3910 shows the same two-cross pattern: retail's second cross reads forward apart from the first.
// M3 lane 4 r190 (`--fast 0x23de50 0x1f0230`, 45 = the full check here): the one-use reset bound `maximum` written into
// its test 45 (same code); 0xe3910's two-cross pattern (Round 58's listed form, re-measured) 59,
// with the else arm's forward pointer 57; a grid of first product (previous_up x forward / forward x previous_up) x second
// (function_1f087b / cross3f_staged, operands to match) x the else pointer x `right` declared first: 45/43/43, 217 x4,
// 224 x4, 59/59/57/57 (best the listed else pointer, 43); the else pointer with the bound written in its test 43; the
// else pointer plus an up pointer for the 0x11d090 call 47 (1264 bytes; near r115's up pointer). New forms: the delay and
// timer clamps through the SDK's `max` 45 (same code); the controller index named once for its test and the slot 337
// (1263 bytes); `advance = delay > observer->delay + 1.f;` assigned directly 74 (1270 bytes); `observer->delay == 0.f` 46.
// Body unchanged.
// M3 lane 2 r77 (`--fast 0x23de50 0x1f0870`, control 45): no unlisted natural form found for what is left (the first
// product's colours, the second product's schedule and the else arm's stores through the forward pointer): product rows,
// staging, helper choices, pointers, copies, declaration orders and the existing angle helpers (by value, and the pointer
// form 0x23d790 builds its forward with) are all listed above. Body unchanged.
// Round 175 (m3 lane 6): no new trial. The else arm's forward stores through ECX are the listed forward-pointer form
// (43, one-use pointer); the existing direction_from_angles there is listed (156/224), as is 0x23d790's helper shape
// (221/222). The first product's colours sit in cross3f (shared include/vector_products.h) and function_1f087b
// (src/unknown_1f03e0.cpp, on this round's avoid list). Body unchanged.
// M3 lane 13 r89 (`--fast 0x23de50 0x1f0870`, control 45): the product colours and the second product's schedule sit in
// cross3f (shared include/vector_products.h) and function_1f087b (src/unknown_1f03e0.cpp, on lane 2's avoid list this
// round), so only the code around them was tried: the camera arm's flags set before the field of view 63, its timer
// cleared first 65; the else arm's k row first 56, its distance copied after the three rows (before 0x11d090) 64, the
// pitch cosine written at each use 169 (1275 bytes). Re-measured: the common field of view/offset/velocity stores once
// after the arms 246 (1211 bytes, Round 512's listed 246). Body unchanged.
// M3 lane 9 r120 (plain): the second product through cross3f 57 (1264 B); both products through function_1f087b 49
// (1264 B); the second through cross3f_staged 217 (1270 B); the first through function_1f087b with the second through
// cross3f_staged 221 (1272 B) or cross3f 222 (1273 B).
// P479: Standard prior-up/displacement/target/fallback/saved-position coordinate producers
// 322/250/329/324/333; paired angle multiply, complete time subtraction and reset multiply
// kernels 258/300/417. NOT kept: all regress; native right i/j, forward.k reuse and angle bases
// remain. Original copies, arithmetic, shared products and complete body restored.
// MiniMax H (11-14): no new form, no lower score (45): what still differs is the register colour of the first product's operands and the else arm's forward stores, both of them inside the shared helpers (include/vector_products.h and src/unknown_1f03e0.cpp), whose bodies this lane may not change.
// @retail 0x23de50
void __stdcall function_23de50(void *state, void *input, s_observer_command *output)
{
	s_observer_state *observer = (s_observer_state *)state;
	s_follow_output_23de50 *command = (s_follow_output_23de50 *)output;
	bool advance = false;
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
			command->position = object_node_matrix(object_get(observer->target_unit_index), 0)->position;
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
		vector3f previous_up = *(vector3f *)((byte *)local_player_state(observer->local_index) + 0x2c);
		vector3f direction;
		vector3d_from_points3d((point3f *)observer->unknown0c, &command->position, &direction);
		command->distance = function_30bf0(&direction);
		command->forward = direction;
		vector3f right;
		cross3f(&previous_up, &command->forward, &right);
		function_1f087b(&command->forward, &right, &command->up);
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
	observer->timer -= ((real *)input)[2];
	observer->delay = 0.f > observer->delay - ((real *)input)[2] ? 0.f : observer->delay - ((real *)input)[2];
	if (observer->player_index != NONE)
	{
		s_player *player = PLAYER(g_4e8c24, observer->player_index & 0xffff);
		if (*(long *)((byte *)player + 0x24) != NONE)
		{
			short slot = (short)*(long *)((byte *)player + 0x24);
			if (g_4e61cc[slot])
			{
				s_input_state *gamepad = g_4e61b9 ? &g_4e630c : &g_4e61dc[slot];
				if (gamepad && gamepad->values[0] == 1)
				{
					real delay = 3.f;
					if (g_4e6948->state == 2)
						delay = 15.f;
					if (delay > observer->delay + 1.f)
						advance = true;
				}
			}
		}
	}
	if ((0.f == observer->delay && (!g_510c54->active || !g_510c54->unknown01)) || advance)
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

/* retail calls it out of line */
__declspec(noinline) void function_23ecd0(void);

// @retail 0x23ecd0
void function_23ecd0(void)
{
	g_470a3c = 1;
	g_470a40 = 2;
	g_470a44 = 3;
	g_470a48 = 4;
	g_470a4c = 5;
	g_470a50 = 6;
	g_470a54 = 7;
	g_470a58 = 8;
}

struct s_shader_cache;
void function_1bbf0(long tag, long first, long second, long third, long fourth, real scale);
void function_1cf50(void);
void function_1c590(s_shader_cache *state, long tag, long index);
void function_1c6b0(void *state);
void __stdcall function_1c710(void *state);
void function_1ccb0(long format);
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
bool function_23ea80(long tag, transform4x3f const *matrix, long variant, long stage, long pass)
{
	bool result = false;
	dword kind = ((dword *)g_4e3b44)[(short)tag * 4];
	long const *reference;
	if (kind == 0x5052544d || kind == 0x70727433)
		reference = function_137bd0(tag)->function_x947334();
	else
		reference = *(long const **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
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
	function_1bbf0(tag, 0, stage, pass, 0, 10000.0f);
	function_1cf50();
	long entry = variant + 7;
	long binding_tag = *(long *)(shader + 0x100);
	result = binding_tag != NONE &&
		(dword)entry < *(dword *)(g_4e3b44[binding_tag & 0xffff].bytes + 4) &&
		(dword)(*(long *)(*(byte **)(g_4e3b44[binding_tag & 0xffff].bytes + 8) + entry * 0x1c + 0xc) >> 4) > 0;
	if (result)
	{
		{
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

static inline void push_vertex_data4f(dword *&push, long slot, real a, real b, real c, real d)
{
	push[0] = D3DPUSH_ENCODE(0x1a00 + slot * 16, 4);
	((real *)push)[1] = a;
	((real *)push)[2] = b;
	((real *)push)[3] = c;
	((real *)push)[4] = d;
	push += 5;
}

static inline void push_vertex_data2f(dword *&push, long slot, real a, real b)
{
	push[0] = D3DPUSH_ENCODE(0x1880 + slot * 8, 2);
	((real *)push)[1] = a;
	((real *)push)[2] = b;
	push += 3;
}

static inline void push_vertex_data4ub(dword *&push, long slot, dword value)
{
	push[0] = D3DPUSH_ENCODE(0x1940 + slot * 4, 1);
	push[1] = value;
	push += 2;
}
static inline void push_begin(dword *&push, dword primitive, dword count)
{
	push[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
	push[1] = primitive;
	push[2] = D3DPUSH_ENCODE(D3DPUSH_INLINE_ARRAY, count) | D3DPUSH_NOINCREMENT_FLAG;
	push += 3;
}

static inline void push_end(dword *&push)
{
	push[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
	push[1] = 0;
	push += 2;
}

// @retail 0x23ed30
void function_23ed30(point3f *a, point3f *b, real c, real d, dword color, real e, real *f, real *h, real *g)
{
	dword *push;

	if (g_470a38 && function_015d00(&push, 0x2e))
	{
		push_vertex_data4f(push, g_470a3c, b->x, b->y, b->z, 1.f);
		push_vertex_data2f(push, g_470a40, e, 0.f);
		push_vertex_data4f(push, g_470a48, a->x, a->y, a->z, 1.f);
		push_vertex_data2f(push, g_470a4c, c, d);
		push_vertex_data4ub(push, g_470a58, color);
		push_vertex_data4f(push, g_470a44, f[0], f[1], f[2], f[3]);
		push_vertex_data4f(push, g_470a50, g[0], g[1], g[2], g[3]);
		push_vertex_data4f(push, g_470a54, h[0], h[1], h[2], h[3]);
		push_begin(push, 8, 8);
		*push++ = 0;
		*push++ = 0;
		*push++ = 1;
		*push++ = 0;
		*push++ = 0x10001;
		*push++ = 0;
		*push++ = 0x10000;
		*push++ = 0;
		push_end(push);
		D3DDevice_EndPush(push);
	}
}
