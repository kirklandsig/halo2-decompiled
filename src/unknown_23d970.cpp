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

__declspec(noinline) long function_1366d0(short width, short height, short depth, short format, short alignment, long mipmap_count);

PRIVATE const dword g_4507c8[24] =
{
	31, 19, 27, 32, -1, -1, 17, -1, 16, 29, 30, 18,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, 23
};

typedef void *(__stdcall *texture_allocate_proc)(long size, long alignment);

// score 28 (was 28): retail reads width and height as full dwords (its only caller sign-extends
// both), so its prototype has 32-bit dimensions; the caller's declaration is short. Retail also
// spills the header register into the dead width slot before the allocation call and makes the null
// result only on the failure path. Tried: the dimensions used directly 46; each path storing the
// outputs and returning 52 (retail's tests, no spill); `if (!result) buffer = 0;` 28; a named
// validity bool 28; a dword aligned size 28; long width/height here and in the caller's
// prototype 27 (176 bytes: the entry and the caller's movsx pushes as retail; the spill, the
// early null and the field order still differ); with that, result starting as the header and cleared with the buffer 42 (failure
// path as retail), result = header at the top 69 (header moves to eax), result set in both arms
// or the test inverted 69, the lock store first 31 (43 with the header result).
// Round 12 (fast): ours makes the null result in ECX before the tests (`xor ecx, ecx; cmp edi,
// ecx`), and that extra instruction stands where retail's dead spill is, so the positions after it
// line up. Result set in both arms 69; result = header just before the allocation call, cleared
// in the else 69 (copy coalesced into the header's register, header moves to eax); `return buffer
// ? header : 0` with no result 68 (branchless sbb, but the Lock store then takes EBX's zero);
// result = 0 at the top 83 (own stack slot); `result = header` first in the success arm and
// result = 0 in the else 51 (both tails and the tests exactly retail's, ECX then stays reserved
// through the arm); that with the Lock store first 51 (180 bytes, Lock from EBX as retail); that
// with long width/height here and in the caller's prototype 52 (176 bytes; caller 244 -> 246);
// with long width/height, the header itself cleared in the failure arm 68, result set at the end
// of the arm 68. Retail's result is set late yet not coalesced with the header, and its spill
// stores the header register: a copy of the header defined after width's read and live across
// the allocation call would give both; no source form tried keeps that copy apart.
// The failure result rewritten 69 (181 bytes); the result declared before allocation 28; the buffer
// before aligned 28; all declarations at entry 28; the result before Lock/Data 33/35; a promoted
// height with the failure result 66 (182), with Lock first 31. Body restored.
// @retail 0x23e340
D3DTexture *function_23e340(short width, short height, short format,
	texture_allocate_proc allocate, long *size, void **data, D3DTexture *header)
{
	long w = width;
	long bytes = function_1366d0((short)w, height, 1, format, 0x40, 0);
	long aligned = (-bytes & 0x7f) + bytes;
	void *buffer = allocate(aligned, 0x80);
	D3DTexture *result = 0;
	if (header && buffer)
	{
		header->Format = (g_4507c8[format] << 8) | 0x10029;
		header->Size = (((((dword)w * 4) >> 6) - 1) << 24) | ((height - 1) << 12) | (w - 1);
		header->Lock = 0;
		header->Data = (dword)buffer & 0x0fffffff;
		header->Common = 0x40001;
		result = header;
	}
	else
	{
		buffer = 0;
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
