// @flags /O1 /Oi /arch:SSE /Gr
#include "view_context_storage.h"
#include "hud_messages.h"
/* UNKNOWN_139296.CPP: per-player interface state (built for size) */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_07f720.h"
#include <string.h>
#include <math.h>
#include <xmmintrin.h>

struct s_player_view
{
	byte unknown00[0x28];
	short user_index;
};

struct s_user_interface_state
{
	real value00;
	byte unknown04[0x6c - 0x04];
};

struct s_510c4c;
extern s_510c4c *g_510c4c;

/* the interface state cleared each game: draw state, then a value per user */
struct s_4e6950
{
	byte unknown00[0x70];
	real user_values[4];
};

s_4e6950 g_4e6950;

struct s_4e69d0
{
	long indices[7];
	byte unknown1c[0x270 - 0x1c];
};

s_4e69d0 g_4e69d0[4];

// @retail 0x139296
void function_139296(long user_index, real value)
{
	if (user_index >= 0 && user_index < 4)
	{
		g_4e6950.user_values[user_index] = value;
	}
}

// @retail 0x1392a9
real function_1392a9(long user_index)
{
	if (user_index >= 0 && user_index < 4)
	{
		return g_4e6950.user_values[user_index];
	}
	return 1.0f;
}

// @retail 0x13a6e8
void function_13a6e8(long player_index, real amount)
{
	s_player_view *player = (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long user_index = player->user_index;
	if (user_index != NONE)
	{
		s_user_interface_state *state = (s_user_interface_state *)(user_index * sizeof(s_user_interface_state) + (byte *)g_510c4c);
		state->value00 = state->value00 - amount;
	}
}

// @retail 0x13ac42
void function_13ac42(long index)
{
	memset(&g_4e69d0[index], 0, sizeof(s_4e69d0));
	g_4e69d0[index].indices[0] = NONE;
	g_4e69d0[index].indices[1] = NONE;
	g_4e69d0[index].indices[2] = NONE;
	g_4e69d0[index].indices[3] = NONE;
	g_4e69d0[index].indices[4] = NONE;
	g_4e69d0[index].indices[5] = NONE;
	g_4e69d0[index].indices[6] = NONE;
}

// @retail 0x13ac30
void function_13ac30(void)
{
	for (long index = 0; index < 4; index++)
	{
		function_13ac42(index);
	}
}

void *function_123d40(char const *name, char const *type, long size);

/* the new hud's game state (g_510c4c, 0x1e4 bytes) */
struct s_new_hud_user
{
	real value00;
	real value04;
	long index08;
	byte unknown0c[0x50 - 0x0c];
	long unknown50[6];
	long unknown68;
};

struct s_new_hud_globals
{
	s_new_hud_user users[4];
	byte unknown1b0[4];
	long player_index;
	byte unknown1b8[0x1c0 - 0x1b8];
	s_player_appearance appearance;
	short unknown1d0;
	bool unknown1d2;
	bool unknown1d3;
	bool unknown1d4;
	byte unknown1d5[0x1d8 - 0x1d5];
	real current;
	real target;
	real rate;
};

struct s_13a720_status
{
	byte unknown00[0x1e];
	union
	{
		word flags;
		byte low_flags;
	};
	byte unknown20[8];
	real value28;
	real value2c;
	byte unknown30[0xad - 0x30];
	bool flagad;
	bool flagae;
};

struct s_interface_sound_block;
bool function_13cb40(void);
long function_14de70(long local_player_index);
bool function_15eb20(long player_index);
void function_22beb5(long local_player_index, dword state, s_interface_sound_block const *block,
	long *sounds, word *playing);

// @retail 0x13a720
void function_13a720(long user_index, s_13a720_status const *status)
{
	s_new_hud_user *user = &((s_new_hud_globals *)g_510c4c)->users[user_index];
	dword flags = 0;
	if (!(status->low_flags & 4) && !(0.0f >= status->value28) && !function_13cb40())
	{
		dword status_flags = 0;
		if (user->value00 != -1.0f && function_15eb20(function_14de70(user_index)))
		{
			if (status->flags & 0x200)
				status_flags |= 1;
			else
				status_flags &= ~1;
			if (user->value00 > status->value2c)
				status_flags |= 2;
			else
				status_flags &= ~2;
			if (status->value2c < 0.25f && status->value2c > 0.0f)
				status_flags |= 4;
			else
				status_flags &= ~4;
			if (status->value2c == 0.0f)
				status_flags |= 8;
			else
				status_flags &= ~8;
			if (status->flagad)
				status_flags |= 0x100;
			else
				status_flags &= ~0x100;
			if (status->flagae)
				status_flags |= 0x200;
			else
				status_flags &= ~0x200;
		}
		flags = status_flags;
		if (g_4e6948->state == 2 && status->value2c >= 1.0f)
			flags &= ~2;
	}
	function_22beb5(user_index, flags, (s_interface_sound_block *)((byte *)g_510c94 + 0x418),
		user->unknown50, (word *)&user->unknown68);
}

struct s_13b164_status
{
	byte unknown00[0xc];
	long index0c;
	long index10;
	byte unknown14[0x4c - 0x14];
	dword flags;
	byte unknown50[0xab - 0x50];
	bool flagab;
	byte unknownac[0xe3 - 0xac];
	bool flage3;
};

struct s_13b164_pair
{
	long object_index;
	long seat_index;
	byte unknown08[0x1c - 8];
	real value;
};

struct s_13b164_object
{
	long definition_index;
	byte unknown04[0xaa - 4];
	byte type;
	byte unknownab[0x138 - 0xab];
	short team;
	byte unknown13a[0x1c8 - 0x13a];
	s_13b164_pair pair;
};

struct s_13b164_header
{
	byte unknown00[8];
	s_13b164_object *object;
};

struct s_object;
struct s_entry_pair;
s_object *function_badc0(long object_index, dword type_mask);
bool function_106320(s_entry_pair *pair);
bool function_1df560(short team_a, short team_b);

// @retail 0x13b164
void function_13b164(long object_index, s_13b164_status *status)
{
	s_13b164_object *object = ((s_13b164_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_13b164_pair *pair = &object->pair;
	status->flags = 0;
	if (pair->object_index != NONE)
	{
		s_13b164_object *other = (s_13b164_object *)function_badc0(pair->object_index, NONE);
		if (other)
		{
			byte *definition = g_4e3b44[other->definition_index & 0xffff].bytes;
			if (((1 << other->type) & 3) && !function_1df560(object->team, other->team))
				status->flags |= 1;
			long model_index = *(long *)(definition + 0x38);
			if (model_index != NONE && pair->seat_index != NONE && pair->value >= 1.0f)
			{
				byte *model = g_4e3b44[model_index & 0xffff].bytes;
				if (pair->seat_index < *(long *)(model + 0x68))
				{
					byte *seat = *(byte **)(model + 0x6c) + pair->seat_index * 0x1c;
					if (seat[0x14] & 4)
						status->flags |= 0x10;
					if (seat[0x14] & 8)
						status->flags |= 4;
				}
			}
		}
		if (((status->index0c != NONE && status->flagab) || (status->index10 != NONE && status->flage3)) &&
			function_106320((s_entry_pair *)pair) && pair->value >= 1.0f)
			status->flags |= 8;
	}
}

struct s_13ad48_item
{
	byte unknown00[6];
	short value06;
	short value08;
	short value0a;
	short value0c;
	byte unknown0e[2];
	real value10;
	real value14;
	real value18;
	byte unknown1c[4];
	bool flag20;
	byte unknown21[3];
	bool flag24;
	bool flag25;
	bool flag26;
	byte unknown27;
	real value28;
};

// @retail 0x13ad48
void function_13ad48(long user_index, byte const *selectors, real *values, byte const *status, s_13ad48_item const *item)
{
	for (long i = 0; i < 4; i++)
	{
		real value = 0.0f;
		switch (selectors[i])
		{
		case 0: value = 0.0f; break;
		case 1: value = 1.0f; break;
		case 2: value = g_510c54->game_time * g_510c54->rate; break;
		case 3: value = function_1392a9(user_index); break;
		case 16: value = *(real const *)(status + 0x2c); break;
		case 17: value = *(real const *)(status + 0x28); break;
		case 18: value = *(real const *)(status + 0x20) >= 1.0f ? 1.0f : 0.0f; break;
		case 19: value = *(short const *)(status + 0x34) == 0 ? 1.0f : 0.0f; break;
		case 20: value = *(short const *)(status + 0x38); break;
		case 21: value = *(short const *)(status + 0x3a); break;
		case 22: value = *(real const *)(status + 0x2c) < 0.2f ? g_510c54->game_time * g_510c54->rate : 0.0f; break;
		case 24: value = 1.0f - *(real const *)(status + 0x44); break;
		case 32: value = 0.0f; break;
		case 33: value = 0.0f; break;
		case 48: value = item ? item->value06 : 0.0f; break;
		case 49: value = item ? item->value14 : 0.0f; break;
		case 50: value = item ? 100.0f - item->value10 * 100.0f : 0.0f; break;
		case 51: value = item ? item->value08 : 0.0f; break;
		case 52: value = item ? item->value18 : 0.0f; break;
		case 53: value = item && item->flag20 ? 1.0f : 0.0f; break;
		case 54: value = item && item->value0a ? (real)item->value06 / item->value0a : 0.0f; break;
		case 55: value = (item ? item->flag20 : false) ? g_510c54->game_time * g_510c54->rate : 0.0f; break;
		case 56: value = item ? 1.0f - item->value10 : 0.0f; break;
		case 57: value = item ? item->value28 : 0.0f; break;
		case 64: value = *(real const *)(status + 0x1cc); break;
		case 65: value = *(real const *)(status + 0x218); break;
		case 66: value = *(real *)((byte *)&((s_new_hud_globals *)g_510c4c)->users[user_index] + 0x28); break;
		case 67: value = *(real const *)(status + 0x220); break;
		}
		values[i] = value;
	}
}

bool function_22acb4(long player_index);

// score 165 (was 165): extra second-mask spill and persistent constant homes remain.
// Retail reads the mode word as NONE, 0, 1 in that order and the type bits as
// 1, 2, 3; either order alone scores 190-196, both 196, the 0x3c block written
// as clears 232-234, a dword second mask 288 and an unsigned flags word 192.
// Retail keeps the first mask in ebx and the second in bx; ours keeps the
// first in edi and spills the second to a second frame slot.
// Round 43: arithmetic NONE-seeded first 237; with third type order 123 240;
// dword/signed-word first 295/295; dword/signed-word second 291/288;
// OR-seeded first with third orders 123/231 281/280.
// Extra second-mask spill and persistent constant homes remain.
// extra second-mask slot and retained constant registers differ;
// third/second/fourth phases 265; fourth/second/third 261; third/fourth/second 246;
// shared item guard 241; descending selection cases 232. Original body retained.
// extra second-mask slot/constant registers remain;
// set-only all masks 529; first 381; second 293; third 262; fourth 294.
// extra second-mask slot and constant homes remain; second enum group kept.
// Second/fourth/third phases 287; fourth/third/second 297; first enum group 238;
// complete type-bit selection 227; second enum group 196; combined selections 201.
// second-mask spill and constant homes remain.
// Round 303: both enum groups 216; unified item predicate 256; multiplayer at use 257;
// shifted source flags 281; fourth enum selection 284; one-case bit first 190;
// one total enum-bit update 291. Final stores and query argument preserved.
// second-mask spill/constant-one retention remain.
// extra second-mask slot and retained constant registers differ;
// Earlier 232 (was 232): extra second-mask slot/constant registers remain;
// Earlier 196 (was 232): extra second-mask slot and constant homes remain; second enum group kept.
// Earlier 190 (was 196): second-mask spill and constant homes remain.
// Earlier 190 (was 190): second-mask spill/constant-one retention remain.
// Total low-bit updates from seeded accumulators: second/third/both/fourth/
// second-fourth/third-fourth/all 254,190,254,190,254,190,254; second seed 3 253.
// second-mask slot and constant homes remain; second-mask bit orders
// 16/2/4/8, 2/16/4/8, 2/4/16/8, 8/4/2/16 all 190;
// 2/8/4/16, 16/8/4/2, 8/16/2/4, 4/8/16/2 all 190 at 884 bytes.
// second-mask spill and constant homes remain.
// First enum order one/zero/NONE 189 (below gate), zero/NONE/one 195, zero/one/NONE 195,
// NONE/one/zero 197; native fourth on current enums 299; signed third/fourth/both 271/299/285.
// second-mask spill and persistent constant homes remain.
// Earlier 190 (was 190): second-mask spill and constant homes remain.
// Fourth-mask code/side/flag-a/flag-b/flag-c observations once: 190/260/209/209/209;
// all five/code plus flag-a/flag-b plus flag-c: 271/209/287. Second-mask spill remains.
// second-mask spill and persistent constant homes remain.
// Earlier 190 (was 190): second-mask spill and persistent constant homes remain.
// Individual redundant clears removed: first16 284; second16 281; both16 290;
// first2/4/8 286/286/290; first2+16 301; initial third128 249. All restored.
// False-first total setters: first/second/third/fourth 190/190/248/207;
// first-second/all 190/260. Clear-before-guarded-set second/all 259/340.
// Round 364: Native enum/source-flags/first-bytes/fourth-bytes reads 190/190/190/190;
// enum plus flags/first bytes 190/190; all scalar fields/all fields plus native items 190/190.
// Round 366: Joint item/type-mask selection 298; mask-first stores 298; zero result 300;
// initialized absent outputs 278; early first seed 298; signed third/native fourth/native second 316/308/285.
// First/second enum observations once 292/284; flags at each use 190; both enum observations 284; first/second/both with live flags 292/284/284; unsigned-word enums with live flags 284. Extra second-mask spill and persistent constant homes remain.
// Third-mask type order 132/213/231/312/321 190/273/165/251/190; item bits
// 132 196; item bits 213 196; item bits 321 196. Keep type order 231, plain 165.
// Round 206: On type order 231, total conditional result updates for first/second/third/fourth masks
// 165 each; first-second, second-third, second-fourth and all masks 165 each. Original setter restored.
// Round 166: Native source bits with Boolean conversion/total low-bit/false-first
// second updates 164/296/275/295/164/296/275/297 fast. The 164 form gains one
// length-penalty point only; extra slot remains. Original body and source fields restored.
// Round 331, retained third-type order 231: first-enum orders 248/284/428/482/842
// 192/193/284/283/189f; total second enum 234f; both enums 235f;
// all mask declarations at entry 165f. Extra spill remains; all eight restored.
// Round 145: Independent enum producers with native first/first-second/first-third/first-fourth/
// first-second-third/first-second-fourth/first-third-fourth/all results 290/299/295/291/311/314/307/323.
// Existing query inline declaration: retained producers 165; independent producers 232. All restored.
// Round 243: Signed source flags char/short 165/164; native second 204/203, third
// 270/269, both 287/286. The one-point length-only form is rejected; original body
// restored.
// our second mask is computed into ECX and spilled to an extra frame slot (retail keeps
// it in EBX and has one local), and the constant held in a register differs (retail: 1 in EDI; ours: 2 or a
// clear mask). Forms tried, by round:
// phases third/second/fourth 265, fourth/second/third 261, third/fourth/second 246, second/fourth/third 287,
// fourth/third/second 297; shared item guard 241; descending selection cases 232; set-only all masks 529,
// first 381, second 293, third 262, fourth 294; first enum group 238; complete type-bit selection 227; second
// enum group 196; combined selections 201; both enum groups 216; unified item predicate 256; multiplayer at use
// 257; shifted source flags 281; fourth enum selection 284; one-case bit first 190; one total enum-bit update 291;
// total low-bit updates from seeded accumulators second/third/both/fourth/second-fourth/third-fourth/all
// 254/190/254/190/254/190/254, second seed 3 253; second-mask bit orders 16/2/4/8, 2/16/4/8, 2/4/16/8, 8/4/2/16,
// 2/8/4/16, 16/8/4/2, 8/16/2/4, 4/8/16/2 all 190; first enum order one/zero/NONE 189, zero/NONE/one 195,
// zero/one/NONE 195, NONE/one/zero 197, NONE/zero/one 192; native fourth 299; signed third/fourth/both
// 271/299/285; fourth-mask code/side/flag-a/flag-b/flag-c observations once 190/260/209/209/209, all five 271,
// code plus flag-a/flag-b 209, plus flag-c 287; redundant clears removed first16 284, second16 281, both16 290,
// first2/4/8 286/286/290, first2+16 301, initial third128 249; false-first total setters first/second/third/
// fourth 190/190/248/207, first-second/all 190/260; clear-before-guarded-set second/all 259/340; native
// enum/source-flags/first-bytes/fourth-bytes reads 190 each; joint item/type-mask selection 298; mask-first
// stores 298; zero result 300; initialized absent outputs 278; early first seed 298; signed third/native
// fourth/native second 316/308/285; enum observations once first/second/both 292/284/284; flags at each use 190;
// third type order 132/213/231/312/321 190/273/165/251/190, 123 190; item bits 196; total conditional result
// updates 165; native source bits with boolean conversion 164 (one length point only); all mask declarations at
// entry 165; second enum as four set-or-clears 234, with the first enum NONE/zero/one 235.
// Round 57 (aligned listing counts, ours vs retail, in brackets): retail's own block order is the first enum
// NONE/zero/one, the second enum as four set-or-clears (0x100 twice) and the third type order 1/2/3; together
// 232 [aligned 84 -> 30]. On that body: short second 283 [19: 1 now in EDI as retail], long second 287 [18],
// (short)(char) second 232 [30], `& (word)1` 232, second split into a store and `&= 1` 283 [19], short first
// 268, all masks short 319, second declared at entry 232, flags read at each use 232, char flags local 233,
// `(word)(flags & 1)` 289. Only the second-mask register (and with it the frame slot) still differs there.
// Round 204: First-enum conditional seeds NONE/one/zero, each remaining order,
// 193/192/165/189/284/283; branch-produced NONE/one seeds 193/165. Retained type
// order 231; original first-mask producer restored. Extra second-mask spill remains.
// Round 206: On type order 231, signed first with word/short/dword/byte second
// 283/291/293/292; native first with the same second carriers 283/291/293/292. Original domains restored.
// Round 347: complete second result as unsigned byte on retained/first-enum/second-enum/both
// producer graphs 269/204/250/280; signed byte repeats 269/204/250/280.
// The second-enum byte form recovers the one-slot prologue, but later operations regress; body restored.
// Round 257: Plain four-entry source/destination mapping loops, original/source-bit order,
// byte fields 304/304; word fields 286/286; dword fields 292/292; int fields 292/292.
// Original scalar masks restored; extra second-mask spill and persistent constants remain.
// Round 210: First-mask high-byte orders 2134/1324/1243/4123/4321/3412/2341/3142
// all 165; original producers restored, extra second-mask spill remains.
// Round 60, on the 232 ordered control: long/short first 290/290;
// long first with short/long second 268/299; long third/fourth 229/229;
// fourth before second 261; third before second 265. Original 165 body restored.
// Round 205: Second/third/fourth domains short-word-word, word-long-word, short-long-word,
// word-word-long, short-word-long, word-long-long, short-long-long, short-short-long
// 202/270/270/295/300/285/297/297. On short-long-word, first enum reordered 296;
// plus independent second 287; plus third 123 292; plus native fourth 269. All restored.
// Round 86 (fast, aligned counts in brackets): retail's block order rebuilt 232 [37]; with a short second 283
// [20: 1 in EDI as retail; only the second mask in ECX with its own slot, from 0xFFFD taking EBX, remains, and
// that extra slot shifts every lockstep line]; on it the second as (short)(char)/(char)(x & 1)/(signed char)/
// & (short)1/1 & (char) 283 [20] each, without the char cast 287 [19]; word second stored then masked 232
// (as the rebuilt order; retail's `and ebx, 0xfffd` says word). Original body restored.
// Round 381: User-query bit update before enum tests/after first 8/2/4 updates 272/239/285/284; after
// 16/32/second-enum/first high 1024 updates 165 each. Original query placement restored; second-mask
// spill and constants remain.
// Round 242: Complete result phase orders 2134/3124/4123/2314/3214/2341/3421/4231
// 280/290/274/289/228/269/272/268. Original phase order, query and aliased output order restored.
// Round 388: One existing source-flags snapshot after first-mask checkpoints
// 8/2/4/16/32/256/512/1024: 281/281/278/275/271/268/268/266.
// Original late snapshot and every mask producer restored; second-mask spill remains.
// Round 399: Source-flags phase scope, multiplayer phase scope, readonly source values and all
// combinations, then all result-home names: 165 each. Original restored.
// Round 38: Complete second-mask computation after first-mask updates 8/2/4/16/32/256/512/1024:
// 272/168/288/165/278/278/280/280 fast, lengths 883/880/874/880/881/881/881/881. Extra
// second-mask slot remains; original phase order, observation and complete body restored.
// Round 424: physical item-selection orders 2143/2413/2431/3142/3412/3421/4132/4213
// all 165 at 884 bytes. Original order restored; extra second-mask spill and constant homes remain.
// N227: Set-first total updates in first/second/third/fourth/first-second/second-third/
// second-fourth/all result families 304/287/281/285/303/285/304/347. Original setter restored.
// Round 237: Complemented word result families first/second/third/fourth/first-second/second-third/second-fourth/all: On recovered complemented-second frame, first enum/independent second/third 123 subsets 1-7 and all with signed second: 178/244/252/250/242/274/245/289/230/234/271/247/228/230/267/262.
// Original executable body restored.
// Round 27 (lane 14): long flags 165 (same code). Retail CSEs the constant 1 into EDI (three cmp ax, di,
// and ebx, edi, cmp type, edi) so (char)flags & 1 stays movsx ebx, al / and ebx, edi; ours narrows it to
// and cl, 1 and CSEs 2 (EBX) and 0xffef (EDI) instead. On the retail block order (232, aligned 37): long
// flags 232, (word)((long)(char)flags & 1) 232, byte flags 233, long second 287 (aligned 19: 1 in EDI as
// retail, second still in ECX with its own slot, 0xfffd CSEd into EBX for the third/fourth clears).
// Round 242: Existing shared word-bit setter in first/second/first-second/third/first-third/
// second-third/all-three/all-four result families 165/299/299/165/165/299/299/299. Original macro,
// result domains, enum producers, query and aliased stores restored; second-mask spill remains.
// @retail 0x13a050
void function_13a050(byte const *status, long user_index, long type, s_13ad48_item const **item_out,
	word *first_out, word *second_out, word *third_out, word *fourth_out)
{
	s_13ad48_item const *item;
	switch (type)
	{
	case 1: item = (s_13ad48_item const *)(status + 0x88); break;
	case 2: item = (s_13ad48_item const *)(status + 0xc0); break;
	case 3: item = (s_13ad48_item const *)(status + 0xf8); break;
	case 4: item = (s_13ad48_item const *)(status + 0x50); break;
	default: item = NULL; break;
	}
	word first = 1;
#define SET_CONDITION(bits, mask, condition) if (condition) { bits |= mask; } else { bits &= ~mask; }
	SET_CONDITION(first, 8, *(short const *)(status + 0x36) == 1);
	SET_CONDITION(first, 2, *(short const *)(status + 0x36) == NONE);
	SET_CONDITION(first, 4, *(short const *)(status + 0x36) == 0);
	SET_CONDITION(first, 0x10, status[0x30]);
	SET_CONDITION(first, 0x20, status[0x31]);
	if (*(short const *)(status + 0x3c) == NONE)
		first |= 0x40;
	else if (*(short const *)(status + 0x3c) == 0)
		first |= 0x80;
	else if (*(short const *)(status + 0x3c) == 1)
		first |= 0x100;
	SET_CONDITION(first, 0x100, *(short const *)(status + 0x3c) == 1);
	SET_CONDITION(first, 0x200, status[0x32]);
	SET_CONDITION(first, 0x400, status[0x3e]);
	SET_CONDITION(first, 0x800, status[0x3f]);
	SET_CONDITION(first, 0x1000, status[0x40]);
	SET_CONDITION(first, 0x2000, function_22acb4(user_index));
	dword flags = *(dword const *)(status + 0x4c);
	word second = (char)flags & 1;
	SET_CONDITION(second, 2, flags & 8);
	SET_CONDITION(second, 4, flags & 0x10);
	SET_CONDITION(second, 8, flags & 4);
	SET_CONDITION(second, 0x10, flags & 2);
	word third = 0;
	SET_CONDITION(third, 2, type == 2);
	SET_CONDITION(third, 4, type == 3);
	SET_CONDITION(third, 1, type == 1);
	SET_CONDITION(third, 0x40, item ? item->flag20 : false);
	third &= ~0x80;
	if (item)
	{
		if (item->value0a && !item->value06 && item->value0c && !item->value08)
			third |= 0x80;
		if (item->value10 >= 1.0f)
			third |= 0x80;
	}
	SET_CONDITION(third, 0x100, item ? item->flag24 : false);
	SET_CONDITION(third, 0x200, item ? item->flag25 : false);
	SET_CONDITION(third, 0x400, item ? item->flag26 : false);
	bool multiplayer = g_4e6948->state == 2;
	word fourth = status[0x131] == 0 ? 1 : 0;
	SET_CONDITION(fourth, 2, status[0x131] == 1);
	SET_CONDITION(fourth, 4, multiplayer && !status[0x132]);
	SET_CONDITION(fourth, 8, multiplayer && status[0x132]);
	SET_CONDITION(fourth, 0x10, status[0x133]);
	SET_CONDITION(fourth, 0x20, !status[0x133]);
	SET_CONDITION(fourth, 0x40, status[0x174]);
	SET_CONDITION(fourth, 0x80, !status[0x174]);
	SET_CONDITION(fourth, 0x100, status[0x1d0]);
	SET_CONDITION(fourth, 0x200, !status[0x1d0]);
	SET_CONDITION(fourth, 0x400, status[0x21c]);
	SET_CONDITION(fourth, 0x800, status[0x224]);
#undef SET_CONDITION
	*item_out = item;
	*first_out = first;
	*second_out = second;
	*third_out = third;
	*fourth_out = fourth;
}

void new_hud_function_context_initialize(void);

// @retail 0x139152
void new_hud_initialize_for_new_map(void)
{
	s_new_hud_globals *globals = (s_new_hud_globals *)g_510c4c;

	memset(globals, 0, sizeof(s_new_hud_globals));
	globals->current = 1.0f;
	globals->target = 1.0f;
	globals->rate = 0.0f;
	globals->player_index = NONE;
	globals->unknown1d0 = NONE;
	globals->unknown1d2 = true;
	globals->unknown1d4 = true;
	globals->unknown1d3 = true;
	for (long i = 0; i < 4; i++)
	{
		memset(globals->users[i].unknown50, 0xff, sizeof(globals->users[i].unknown50));
		globals->users[i].value00 = -1.0f;
		globals->users[i].value04 = -1.0f;
		globals->users[i].index08 = NONE;
	}
	memset(&globals->appearance, 0, sizeof(globals->appearance));
	new_hud_function_context_initialize();
	function_13ac30();
}

dword __cdecl pack_color3f(const color3f *color);

dword g_502234[4];
byte g_502244;

// @retail 0x1392f8
void function_1392f8(s_player_appearance const *appearance)
{
	s_player_appearance const *const *appearance_reference = &appearance;
	color3f colors[4];

	function_7f790(NONE, false, appearance, colors);
	for (long i = 0; i < 4; i++)
	{
		g_502234[i] = pack_color3f(&colors[i]);
	}
	byte *local_1 = &g_502244;
	*local_1 = 4;
	((s_new_hud_globals *)g_510c4c)->appearance = **appearance_reference;
}

struct s_new_hud_player
{
	byte unknown00[0x84];
	s_player_appearance appearance;
};

// @retail 0x1392c5
void function_1392c5(long player_index)
{
	((s_new_hud_globals *)g_510c4c)->player_index = player_index;
	if (player_index != NONE)
	{
		s_new_hud_player *player = (s_new_hud_player *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
		function_1392f8(&player->appearance);
	}
}

/* the per-user interface state in the game state (called by function_19170f) */
// @retail 0x139130
void function_139130(void)
{
	g_510c4c = (s_510c4c *)function_123d40("new hud", NULL, 0x1e4);
	memset(&g_4e6950, 0, sizeof(g_4e6950));
	function_13ac30();
}

struct s_name_buffer
{
	wchar_t name[256];
};

void function_08cc20(s_name_buffer *buffer, const wchar_t *name);

struct s_510c4c_view
{
	byte unknown000[0x1bc];
	byte *strings;
	byte unknown1c0[0x1d2 - 0x1c0];
	bool flag1d2;
};

// @retail 0x13934d
void function_13934d(s_name_buffer *buffer, long string_handle)
{
	byte *strings = ((s_510c4c_view *)g_510c4c)->strings;
	if (strings)
	{
		wchar_t const *name;
		switch (string_handle)
		{
		case 0xe42d:
			name = (wchar_t const *)(strings + 0x1bc);
			break;
		case 0xe42e:
			name = (wchar_t const *)(strings + 0x208);
			break;
		case 0xe42f:
			name = (wchar_t const *)(strings + 0x134);
			break;
		case 0xe430:
			name = (wchar_t const *)(strings + 0x176);
			break;
		default:
			name = NULL;
			break;
		}
		if (name)
		{
			function_08cc20(buffer, name);
			return;
		}
	}
	buffer->name[0] = 0;
}

long function_155760(long index);
bool function_155d60(long index);

// @retail 0x13939b
bool function_13939b()
{
	long index = g_4b9ed8;
	if ((function_155760(index) != 3 || function_155d60(index)) &&
		function_155760(index) != 2 &&
		((s_510c4c_view *)g_510c4c)->flag1d2 && !function_2ac270_get(6))
		return true;
	return false;
}

byte function_016a90();

// @retail 0x13a690
long function_13a690(long mode)
{
	long result = 0;
	short top = g_4b9dd0;
	short bottom = g_4b9dd4;

	if ((short)(g_4b9dd6 - g_4b9dd2) < 640 || (short)(bottom - top) < 480)
	{
		if ((short)(g_4b9dd6 - g_4b9dd2) < 640 && (short)(bottom - top) < 480)
			result = 2;
		else
		{
			result = 1;
			if (function_016a90() && mode == 3)
				result = 2;
		}
	}

	return result;
}
struct s_510c4c_fade_view
{
	byte unknown000[0x1d8];
	real current;
	real target;
	real rate;
};

long function_1469f0(real seconds);

struct s_interface_pulse
{
	char delay;
	char ticks;
	char repeats;
	byte unknown03;
};

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// @retail 0x200891
real function_200891(s_interface_pulse const *pulse, bool rising)
{
	(void)&rising;
	real fraction;
	if (!pulse->ticks)
		fraction = 1.0f;
	else
		fraction = (real)pulse->ticks * g_510c54->rate * 1.5151515007019043f;
	if (rising)
		return PIN(fraction * 2.0, 0.0f, 1.0f);
	return PIN((fraction - 0.5f) * 2.0, 0.0f, 1.0f);
}

// @retail 0x2008fc
real function_2008fc(s_interface_pulse const *pulse, bool enabled)
{
	(void)&enabled;
	real result;
	if (enabled)
		result = function_200891(pulse, true);
	else
		result = 1.0f;
	return result;
}

PRIVATE __forceinline s_new_hud_user *pulse_user(long index)
{
	return &((s_new_hud_globals *)g_510c4c)->users[index];
}

// @retail 0x2003dc
void function_2003dc(long user_index)
{
	s_new_hud_user *user = pulse_user(user_index);
	for (long i = 0; i < 9; i++)
	{
		char *ticks = (char *)user + 0x2d + i * 4;
		if (ticks[-1] > 0)
		{
			if (--ticks[-1] == 0)
			{
				if (--ticks[1] > 0)
					ticks[-1] = (char)function_1469f0(0.5f);
				else
					ticks[0] = (char)function_1469f0(0.66f);
			}
			else if (ticks[0])
			{
				ticks[0] += 2;
				if ((real)ticks[0] * g_510c54->rate > 0.66f)
					ticks[0] = 0;
			}
		}
		else if (ticks[0] > 0)
			ticks[0]--;
	}
}

// @retail 0x13b306
void __stdcall function_13b306(real target, real seconds)
{
	if (seconds == 0.0f)
	{
		s_510c4c_fade_view *data = (s_510c4c_fade_view *)g_510c4c;
		data->current = target;
		data->target = target;
		data->rate = 0.0f;
	}
	else
	{
		long ticks = function_1469f0(seconds);
		if (ticks <= 1)
			ticks = 1;
		s_510c4c_fade_view *data = (s_510c4c_fade_view *)g_510c4c;
		data->target = target;
		data->rate = (target - data->current) / ticks;
	}
}

/* moves the value towards its target at its rate, stopping there */
// @retail 0x13b285
void function_13b285()
{
	s_510c4c_fade_view *data = (s_510c4c_fade_view *)g_510c4c;
	if (data->target > data->current)
	{
		data->current += (real)fabs(data->rate);
		if (data->current > data->target)
		{
			data->current = data->target;
			data->rate = 0.0f;
		}
	}
	else if (data->current > data->target)
	{
		data->current -= (real)fabs(data->rate);
		if (data->target > data->current)
		{
			data->current = data->target;
			data->rate = 0.0f;
		}
	}
}

/* the conditions of an interface element: masks of which one must match
   and none of the other may */
struct s_condition_masks
{
	word required[4];
	word excluded[4];
	byte minimum_value;
	byte minimum_a;
	byte minimum_b;
};

struct s_condition_subject
{
	byte unknown00[6];
	short a;
	short b;
	byte unknown0a[6];
	real value;
};

// @retail 0x13ac87
bool function_13ac87(s_condition_masks const *masks, word first, word second, word fourth, s_condition_subject const *subject, word third)
{
	if (subject)
	{
		if (masks->minimum_value > subject->value)
		{
			third |= 8;
		}
		else
		{
			third &= ~8;
		}
		if (subject->a < masks->minimum_a)
		{
			third |= 0x10;
		}
		else
		{
			third &= ~0x10;
		}
		if (subject->b < masks->minimum_b)
		{
			third |= 0x20;
		}
		else
		{
			third &= ~0x20;
		}
	}

	if ((masks->required[0] & first) || (masks->required[1] & second) || (masks->required[2] & third) || (masks->required[3] & fourth))
	{
		if (!(masks->excluded[0] & first) && !(masks->excluded[1] & second) && !(masks->excluded[2] & third) && !(masks->excluded[3] & fourth))
		{
			return true;
		}
	}
	return false;
}

struct s_ammunition_state
{
	long field_0;
	byte unknown04[2];
	short field_6;
	short rounds;
	short field_a;
	short magazine;
	byte unknown0e[2];
	real charge;
	byte unknown14[0xd];
	bool flag21;
	bool field_22;
};

struct s_ammunition_definition
{
	byte unknown00[0x18];
	short field_18;
	short maximum_rounds;
	real minimum_charge;
};

/* the state an ammunition counter shows */
static __forceinline long function_13b084(s_ammunition_state const *arg_1)
{
    return real_truncate(arg_1->charge * 100.0f);
}

// @retail 0x13b083
long function_13b083(long definition_index, s_ammunition_state const *state)
{
	long result = NONE;
	if (definition_index != NONE)
	{
		s_ammunition_definition *definition = (s_ammunition_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		if (state->magazine == 0 && 100 - PIN(function_13b084(state), 0, 100) == 0)
		{
			result = 4;
		}
		else if (state->magazine == 0 && definition->minimum_charge >= (1.0f - state->charge) * 100.0f)
		{
			result = 3;
		}
		else if (state->rounds == 0)
		{
			result = 2;
		}
		else if (state->rounds > definition->maximum_rounds || state->flag21)
		{
			result = 7;
		}
		else
			result = 1;
	}
	return result;
}

struct s_13aa27_record
{
	long field_0;
	byte field_4;
	byte field_5;
};

void function_2007b3(long arg_0, long arg_1, long arg_2, long arg_3);

// @retail 0x13aa27
void function_13aa27(long arg_1, byte const *arg_2, byte *arg_3, long arg_4, byte const *arg_5)
{
	byte const *const *local_0 = &arg_5;
	s_ammunition_state const *state = (s_ammunition_state const *)arg_2;
	s_13aa27_record *record = (s_13aa27_record *)arg_3;

	if (state->field_0 != record->field_0)
	{
		record->field_4 = 0;
		record->field_0 = state->field_0;
	}
	if (arg_4 != NONE)
	{
		s_ammunition_definition *definition = (s_ammunition_definition *)g_4e3b44[arg_4 & 0xffff].bytes;
		byte previous = record->field_4;
		byte local_1 = 0;
		if (state->field_a == 0)
		{
			if (definition->minimum_charge >= (1.0f - state->charge) * 100.0f)
				local_1 = 2;
		}
		else if (state->field_6 <= definition->field_18)
		{
			local_1 = 1;
		}
		if (local_1 != previous && local_1 != 0)
		{
			long local_2 = function_13b083(arg_4, state);
			if (local_2 != NONE)
				function_2007b3(arg_1, local_2, 3, NONE);
			record->field_5 = (byte)function_1469f0(5.0f);
		}
		if (local_1 != 0)
		{
			if (--record->field_5 == 0)
			{
				long local_3 = function_13b083(arg_4, state);
				record->field_5 = (byte)function_1469f0(5.0f);
				if (local_3 != NONE)
					function_2007b3(arg_1, local_3, 1, NONE);
			}
			if (state->field_a != 0 && state->field_22)
			{
				if (state->rounds == 0)
				{
					function_2007b3(arg_1, 2, 1, NONE);
					record->field_5 = (byte)function_1469f0(5.0f);
				}
				else if (state->rounds <= definition->maximum_rounds && !state->flag21)
				{
					function_2007b3(arg_1, 1, 1, NONE);
					record->field_5 = (byte)function_1469f0(5.0f);
				}
			}
			if (state->magazine == 0 && state->field_22)
			{
				if (100 - PIN(function_13b084(state), 0, 100) == 0)
				{
					function_2007b3(arg_1, 4, 1, NONE);
					record->field_5 = (byte)function_1469f0(5.0f);
				}
				else if (definition->minimum_charge >= (1.0f - state->charge) * 100.0f)
				{
					function_2007b3(arg_1, 3, 1, NONE);
					record->field_5 = (byte)function_1469f0(5.0f);
				}
			}
		}
		record->field_4 = local_1;
	}
}

void function_1a0180(long tag_index, long string_handle, word *buffer);

/* copies one of the HUD's message strings into a buffer of 0x100 characters */
// @retail 0x13925f
void function_13925f(long string_handle, word *buffer)
{
	s_hud_globals_definition *definition = g_510c94;

	buffer[0] = 0;
	if (definition && definition->string_list != NONE)
	{
		function_1a0180(definition->string_list, string_handle, buffer);
	}
}

struct s_weapon_status_magazine
{
	bool active;
	bool idle;
	short loaded;
	short loaded_maximum;
	short unloaded;
	short total_maximum;
};

struct s_weapon_status
{
	real value;
	real heat;
	bool flag8;
	bool flag9;
	byte unknown0a[2];
	real fraction;
	bool charging;
	bool target_available;
	bool target_charging;
	bool target_ready;
	real target_fraction;
	point3f target_position;
	short magazine_count;
	s_weapon_status_magazine magazines[2];
};


void function_100520(long weapon_index, s_weapon_status *status);

struct s_hud_weapon_record
{
	long object_index;
	bool valid;
	byte unknown05;
	short loaded;
	short unloaded;
	short loaded_maximum;
	short total_maximum;
	byte unknown0e[2];
	real heat;
	real value;
	real fraction;
	byte unknown1c[4];
	bool flag20;
	bool flag21;
	bool flag22;
	bool flag23;
	bool flag24;
	bool flag25;
	bool flag26;
	byte unknown27;
	real target_fraction;
	point3f target_position;
};

// @retail 0x13afa9
void function_13afa9(long object_index, long *definition_out, s_hud_weapon_record *record)
{
	s_13b164_object *object = ((s_13b164_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_weapon_status status;
	function_100520(object_index, &status);
	record->flag20 = status.flag8;
	record->loaded = status.magazines[0].loaded;
	record->loaded_maximum = status.magazines[0].loaded_maximum;
	record->unloaded = status.magazines[0].unloaded;
	record->total_maximum = status.magazines[0].total_maximum;
	record->flag21 = status.magazines[0].active;
	record->flag22 = status.flag9;
	record->flag23 = status.charging;
	record->flag24 = status.target_available;
	record->flag25 = status.target_charging;
	record->value = status.value;
	record->heat = status.heat;
	record->fraction = status.fraction;
	record->object_index = object_index;
	record->flag26 = status.target_ready;
	record->valid = true;
	record->target_fraction = status.target_fraction;
	record->target_position = status.target_position;
	*definition_out = *(long *)((byte *)g_4e3b44[(*(long *)object) & 0xffff].data + 0x2b4);
}


struct s_13a532
{
    byte field_0[8];
    long field_8;
    byte *field_c;
    long field_10;
    byte *field_14;
    byte field_18[8];
    long field_20;
    byte *field_24;
};

struct s_text_widget;
struct s_text_widget_state;
bool function_22a9bc(s_text_widget const *arg_1, long arg_2,
    s_text_widget_state const *arg_3, color4f const *arg_4);
bool function_22b7dd(byte const *arg_1, long arg_2);
struct s_bitmap_widget_22ae21;
bool function_22af8d(long arg_1, byte const *arg_2, s_bitmap_widget_22ae21 const *widget);
struct s_22b8e2;
void function_22b8e2(s_22b8e2 const *arg_0, long arg_1, color4f const *arg_2);
void function_22aff1(long arg_1, byte const *arg_2, byte const *arg_3, real const *arg_4);
void __stdcall function_22aa16(long arg_1, byte const *arg_2, byte const *arg_3, real const *arg_4);

// score 1 (was 1): first count still TEST EAX,EAX versus CMP EAX,EBX: retail's EBX is the
// hoisted mask zero, compared by the first guard and then kept as the first offset; ours starts
// the offset from it before the guard. The sibling form for the first loop (guard, then offset)
// makes loop 1 exact but keeps the zero live in EBX all function (118) unless a later loop
// starts its offset before its guard (one later loop in the for form 3, both 6).
// Offset-compared do/renamed-do 12/12; offset-guard for/nonzero-for/named admission/while 1/1/1/1;
// offset-plus-one admission 125; split first offset 42; direct-bound first do/for 118/118;
// direct-bound first with independent later loops 6; later count-first 6; captured later guards 116;
// independent second 3; captured/split-offset second 118/118; nonzero second admission 76;
// positive-nonzero/double-positive/combined second admissions 118/118/118.
// Earlier at 12: frame 0x28 and retail length agree.
// Earlier: retail keeps a success byte (set to 1 up front, overwritten with the
// call's result on failure) and returns that byte directly; ours keeps the failure flag, so the
// store and the negated return differ and the function is eight bytes longer.
// Earlier: Failure-byte initialization, stores and inverted return still differ.
// descriptor reference 38; immutable descriptor 38; single count reads 38; byte failure 38;
// signed-byte failure 38; wide failure 70; immutable callback results 38; shared callback-result
// home 38.
// native success still extends zero lifetime. Shared
// mask-zero producer with direct result 122; output-only masks 116; first,
// second, third or fourth default omitted 102/103/98/104; shared zeros with
// true or explicit false comparison 87/122. Original failure byte retained.
// Earlier: native success with first captured count 42; second/third 118;
// first plus second 45; count/offset names 42; offset before first count 12 plain,
// offset before setup 20; original count observations with early offsets 125/121;
// guard-assigned count 20; offset-produced mask zeros 125. Native-result 12 remains
// unmatched: first count takes EBX and offset spills; match-only result form restored.
// Earlier: the success byte is retail's (set, cleared after a failed draw, returned);
// the first loop's count takes EBX and its offset spills, where retail keeps the offset in EBX.
// Indexed loops over captured counts 118; their call result stored 118; first loop count declared
// before its offset 42; first offset alone outside the guard 118; one shared offset/count 41;
// first loop indexed 118; every count captured before its guard 20.
// First positive-count for/while 1/1; unsigned admitted remainder 1; unsigned first offset 12;
// admitted offset 42; shared color workspace 12; native color type 12; admitted offset plus shared color 42.
// Positive-count for kept after whole-program validation; all draw call tuples unchanged.
// Original positive guard before positive/nonzero counted walk 1/1; both redundant-guard diagnostics
// restored because they are nonexact. The ordinary positive-count for remains the plain-validated keeper.
// Repaired-countdown mask chain 5; offset mask producer 5; late result 1; subject,
// descriptor, input and mask lexical roles 1/1/1/1; mask chain with native roles 5.
// Partial mask chains 34/35/36/45/46/56/345/456: 5/4/5/3/4/3/5/3; first zero-test remains.
// First loop indexed over a captured count/the field 118/106; captured count guarded with the
// offset inside 42 (count takes EBX); guarded for/while 118/118; index counter with the offset
// as now 1; sibling form plus later loops captured/indexed/count-first/`> 0` do/guarded for
// 116/118/118/125/118; draw result as a ternary 158 or stored as the failed result 118 (also alone);
// one shared offset, with or without a shared count 127/127; offset declared before the mask
// zeros, before the setup call or first, with the sibling first loop 1/1/1.
// Round 20: an indexed first loop (`for (long i = 0; i < count; i++)`, element field_24 + i * 0x50) is
// exact once at most one later loop has a `> 0` guard: with the later loops in the current for form 6
// (both zero EBX before their guards); later sibling loops guarded `>= 1` 12 (`cmp eax, 1; jl`, the rest
// of both loops exact); all three indexed 118; chained mask zeroing with the indexed first loop 122;
// indexed first, sibling second, for-form third 3 (only the third xor-before-guard); indexed first and
// third with the sibling second 118. Two later zero guards (or indexed zero starts) keep the zero live.
// @retail 0x13a3c6
bool function_13a3c6(long arg_1, long arg_2, byte const *arg_3, long arg_4)
{
	bool success = true;
	s_13a532 const *local_2 = (s_13a532 const *)g_4e3b44[arg_1 & 0xffff].bytes;
    word local_3 = 0;
    word local_4 = 0;
    word local_5 = 0;
    word local_6 = 0;
    s_13ad48_item const *local_7;
    function_13a050(arg_3, arg_2, arg_4, &local_7, &local_3, &local_4, &local_5, &local_6);
    long offset = 0;
    long count = local_2->field_20;
    for (; count > 0; --count)
    {
        byte const *local_9 = local_2->field_24 + offset;
        if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
            local_6, (s_condition_subject const *)local_7, local_5))
        {
            real local_10[4];
            function_13ad48(arg_2, local_9 + 4, local_10, arg_3, local_7);
            if (!function_22b7dd(local_9, arg_2)) success = false;
        }
        offset += 0x50;
    }
    if (local_2->field_8 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_8;
        do
        {
            byte const *local_9 = local_2->field_c + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, (s_condition_subject const *)local_7, local_5))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, local_7);
                if (!function_22af8d(arg_2, arg_3, (s_bitmap_widget_22ae21 const *)local_9)) success = false;
            }
            local_8 += 0x64;
        }
        while (--local_11);
    }
    if (local_2->field_10 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_10;
        do
        {
            byte const *local_9 = local_2->field_14 + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, (s_condition_subject const *)local_7, local_5))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, local_7);
                if (!function_22a9bc((s_text_widget const *)local_9, arg_2, (s_text_widget_state const *)arg_3,
                    (color4f const *)local_10))
                    success = false;
            }
            local_8 += 0x54;
        }
        while (--local_11);
    }
	return success;
}

// @retail 0x13a532
void function_13a532(long arg_1, long arg_2, byte const *arg_3, long arg_4)
{
	s_13a532 const *local_2 = (s_13a532 const *)g_4e3b44[arg_1 & 0xffff].bytes;
    word local_3 = 0;
    word local_4 = 0;
    word local_5 = 0;
    word local_6 = 0;
    s_13ad48_item const *item;
    function_13a050(arg_3, arg_2, arg_4, &item, &local_3, &local_4, &local_5, &local_6);
    if (local_2->field_20 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_20;
        do
        {
            byte const *local_9 = local_2->field_24 + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, (s_condition_subject const *)item, local_5))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, item);
                function_22b8e2((s_22b8e2 const *)local_9, arg_2, (color4f const *)local_10);
            }
            local_8 += 0x50;
        }
        while (--local_11);
    }
    if (local_2->field_8 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_8;
        do
        {
            byte const *local_9 = local_2->field_c + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, (s_condition_subject const *)item, local_5))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, item);
                function_22aff1(arg_2, arg_3, local_9, local_10);
            }
            local_8 += 0x64;
        }
        while (--local_11);
    }
    if (local_2->field_10 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_10;
        do
        {
            byte const *local_9 = local_2->field_14 + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, (s_condition_subject const *)item, local_5))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, item);
                function_22aa16(arg_2, arg_3, local_9, local_10);
            }
            local_8 += 0x54;
        }
        while (--local_11);
    }
}

// @retail 0x1393f3
bool function_1393f3(long arg_1, byte *arg_2)
{
	bool local_1 = true;
	((s_510c4c_view *)g_510c4c)->strings = arg_2;
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x8) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x8), arg_1, arg_2, 4)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0xc) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0xc), arg_1, arg_2, 1)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x10) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x10), arg_1, arg_2, 2)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x18) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x18), arg_1, arg_2, 0)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x0) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x0), arg_1, arg_2, 0)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x4) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x4), arg_1, arg_2, 0)) local_1 = false;
	}
	return local_1;
}

// @retail 0x1394b9
void function_1394b9(long arg_1, byte *arg_2)
{
	((s_510c4c_view *)g_510c4c)->strings = arg_2;
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3);
	}
	if (*(long const *)(arg_2 + 0x8) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x8), arg_1, arg_2, 4);
	}
	if (*(long const *)(arg_2 + 0xc) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0xc), arg_1, arg_2, 1);
	}
	if (*(long const *)(arg_2 + 0x10) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x10), arg_1, arg_2, 2);
	}
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3);
	}
	if (*(long const *)(arg_2 + 0x18) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x18), arg_1, arg_2, 0);
	}
	if (*(long const *)(arg_2 + 0x0) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x0), arg_1, arg_2, 0);
	}
	if (*(long const *)(arg_2 + 0x4) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x4), arg_1, arg_2, 0);
	}
}

void function_13992a(byte *arg_1, long arg_2);
void function_2003dc(long arg_1);
void function_13aa27(long arg_1, byte const *arg_2, byte *arg_3, long arg_4, byte const *arg_5);
bool function_14ddc0(long arg_1);
void function_13b285(void);
long players_first_active_local_player(void);
long function_14de10(long arg_1);

__forceinline long function_13a83a(long arg_1)
{
    long local_1;
    if (function_14ddc0(arg_1))
    {
        long local_2 = function_14de70(arg_1);
        local_1 = *(long *)(g_4e8c24->data + (local_2 & 0xffff) * 0x21c + 0x2c);
    }
    else
        local_1 = NONE;
    return local_1;
}

// @retail 0x13a838
void __stdcall function_13a838(long arg_1)
{
    long const *local_0 = &arg_1;
    byte *local_1 = (byte *)g_510c4c + arg_1 * 0x6c;
    byte local_2[0x270];
    function_2003dc(arg_1);
    function_13992a(local_2, arg_1);
    function_13a720(arg_1, (s_13a720_status const *)local_2);
    if (local_2[0x130])
    {
        real local_3 = local_2[0x133] ? 1.0f : 0.0f;
        real *local_15 = (real *)(local_1 + 0x28);
        if (*local_15 != local_3)
        {
            real local_4 = 2.0f / g_510c54->field_2_3;
            if (local_3 > *local_15)
            {
                real local_5 = *local_15 + local_4;
                *local_15 = local_5 > 1.0f ? 1.0f : local_5;
            }
            else
            {
                real local_5 = *local_15 - local_4;
                *local_15 = local_5 > 0.0f ? local_5 : 0.0f;
            }
        }
    }
    long local_6 = function_13a83a(arg_1);
    if (local_6 != NONE)
    {
        byte const *local_8 = *(byte **)(g_4e0300->data + (local_6 & 0xffff) * 12 + 8);
        if (*(real *)local_1 > *(real *)(local_8 + 0xf0))
        {
            if (*(real *)(local_1 + 4) < 0.0f || *(real *)(local_1 + 4) > 1.0f)
                *(long *)(local_1 + 8) = g_510c54->game_time;
            long local_10 = *(long *)(local_1 + 8);
            if (0.5f > (g_510c54->game_time - local_10) * g_510c54->rate)
                *(real *)(local_1 + 4) = 0.0f;
            else
            {
                *(real *)local_1 = *(real *)(local_8 + 0xf0);
                *(real *)(local_1 + 4) += (g_510c54->game_time - local_10) * g_510c54->rate;
                *(long *)(local_1 + 8) = g_510c54->game_time;
            }
        }
        else
        {
            if (*(real *)(local_8 + 0xf0) > *(real *)local_1)
            {
                *(real *)local_1 = *(real *)(local_8 + 0xf0);
                *(real *)(local_1 + 4) = -1.0f;
            }
            else
            {
                *(real *)local_1 = *(real *)(local_8 + 0xf0);
                if (*(real *)(local_1 + 4) > 0.0f)
                    *(real *)(local_1 + 4) += (g_510c54->game_time - *(long *)(local_1 + 8)) * g_510c54->rate;
            }
            *(long *)(local_1 + 8) = g_510c54->game_time;
        }
    }
    long local_12 = *(long *)(local_2 + 0xc);
    if (local_12 != NONE)
        function_13aa27(arg_1, local_2 + 0x88, local_1 + 0x18, local_12, local_2);
    local_12 = *(long *)(local_2 + 0x10);
    if (local_12 != NONE)
        function_13aa27(arg_1, local_2 + 0xc0, local_1 + 0x20, local_12, local_2);
}

// @retail 0x1391ed
void function_1391ed(void)
{
    word local_1 = 0;
    function_13b285();
    for (long local_2 = players_first_active_local_player(); local_2 != NONE; local_2 = function_14de10(local_2))
    {
        local_1 |= 1 << local_2;
        function_13a838(local_2);
    }
    for (long local_3 = 0; local_3 < 4; local_3++)
    {
        if (!(local_1 & (1 << local_3)))
        {
            s_new_hud_user *local_4 = &((s_new_hud_globals *)g_510c4c)->users[local_3];
            function_22beb5(local_3, 0, (s_interface_sound_block *)((byte *)g_510c94 + 0x418),
                local_4->unknown50, (word *)&local_4->unknown68);
        }
    }
}
