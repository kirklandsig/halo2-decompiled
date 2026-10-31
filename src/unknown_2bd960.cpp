#include <string.h>
#include <math.h>
#include <wchar.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19ec40.h"
#include "engine_peer.h"
#include "unknown_1523c0.h"
#include "game_engine_events.h"
#include "unknown_13927e.h"
#include "data_array.h"
#include "unknown_07f720.h"

// @flags /O2 /arch:SSE /Gr
#include "view_context_storage.h"

/* ---- local views ---- */

/* a player (g_4e8c24 elements, 0x21c bytes) */
struct s_player_2bd
{
	byte unknown00[0x2c];
	long object_index;
	byte unknown30[0x1b8 - 0x30];
	short s1b8;
	byte unknown1ba[0x21c - 0x1ba];
};

/* the engine state (g_51ecc8): four points are set to a default */
struct s_state_2bd
{
	byte unknown00[0x1a0];
	long l1a0;
	long l1a4;
	short s1a8;
	byte unknown1aa[2];
	point3f p1ac;
	point3f p1b8;
	point3f p1c4;
	point3f p1d0;
};

/* the engine state in the multiplayer globals (g_51eccc, 0x118 bytes at
   g_4e9ae8 + 0xfc) */
struct s_state_2bf
{
	real f0[8];
	real f20[8];
	real f40[8];
	union
	{
		word w60[8];
		long l60[4];
	};
	long l70[8];
	/* each player's progress taking a territory: the time spent inside it
	   or outside, the territory left and the one inside (a), and the
	   progress shown (b) */
	struct
	{
		short time_inside;
		short time_outside;
		byte previous;
		byte a;
		byte b;
		byte unknown7;
	} entries90[16];
	word w110;
	word w112;
	word w114;
	byte unknown116[2];
};

struct s_spline_2bd
{
	long indices[24];
	long count;
};

/* the hill: a polygon of 32 vertices on a closed spline through up to 16
   markers, inside a circle and a height range */
struct s_polygon_2be
{
	/* the markers (the spline's points) */
	long ids[16];
	/* the vertices each spline segment gets */
	long steps[8];
	long count;
	point2f vertices[32];
	real center_x;
	real center_y;
	real center_z;
	real radius;
	real z_min;
	real z_max;
	real perimeter;
};

/* the settings an engine update copies, as the peer sees them */
struct s_settings_2bd
{
	byte unknown00[0x24];
	short s24;
	short s26;
};

/* the settings an engine update copies, as the territories' peer sees them */
struct s_settings_2c0
{
	byte unknown00[0x24];
	long holders[8];
	byte a[16];
	byte b[16];
};

struct s_stats_a
{
	long l[10];
};

struct s_stats_b
{
	long l[9];
	long b[8];
	long unknown44[8];
};

s_state_2bd *g_51ecc8;
s_state_2bf *g_51eccc;
point3f *g_468710;

/* callees */

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* ---- the engine classes at 0x45c8f0 and 0x45c9c0 ---- */

bool function_15eaf0();
void function_15b930(long player_index, bool by_team, long counter, long delta);
void function_1a0180(long tag_index, long string_handle, word *buffer);
int unicode_string_vsnprintf(word *buffer, long maximum_count, const word *format, ...);

class c_game_engine_a : public c_game_engine
{
public:
	virtual void v0(long, long, bool, long);
	virtual void v2(long, long);
	virtual bool v5(long, long);
	virtual bool v25();
	virtual void v32(long);
	virtual bool v38(long, long);
	virtual void v40();
	virtual void v43(long);
};

class c_game_engine_b : public c_game_engine
{
public:
	virtual bool v5(long, long);
	virtual long v20(long, long, long, long, long);
};

/* The complete retail table at 0x45c948 has its own slot numbering.
   The legacy classes above retain their existing shared-layout overrides. */
class c_game_engine_45c948
{
public:
	virtual long v0() { return NONE; }
	virtual bool v1();
	virtual void v2() {  }
	virtual bool v3() { return true; }
	virtual bool v4(long) { return true; }
	virtual void v5(long) {  }
	virtual void v6(long);
	virtual void v7(long) {  }
	virtual void v8(long) {  }
	virtual void v9(long, long) {  }
	virtual void v10(long) {  }
	virtual void v11() {  }
	virtual void v12();
	virtual void v13(long) {  }
	virtual void v14(long);
	virtual void v15(long) {  }
	virtual bool v16(long, long) { return true; }
	virtual void v17(long, long) {  }
	virtual void v18() {  }
	virtual real v19(long) { return 0.0f; }
	virtual long v20(long) { return 0; }
	virtual void v21(long) {  }
	virtual void v22(long) {  }
	virtual void v23(long, long) {  }
	virtual void v24(long, long) {  }
	virtual long v25(long, long, long) { return 0; }
	virtual long v26() { return NONE; }
	virtual bool v27(short, short) { return false; }
	virtual void v28(long) {  }
	virtual void v29() {  }
	virtual void v30(long, long, bool, long) {  }
	virtual long v31(long, long, long) { return 0; }
	virtual void v32(long player_index, long list_pointer);
	virtual bool v33(long, long) { return true; }
	virtual bool v34(long) { return false; }
	virtual bool v35(long, long) { return false; }
	virtual void v36(long) {  }
	virtual long v37(long, byte *) { return 0; }
	virtual bool v38(long) { return false; }
	virtual long v39() { return 0; }
	virtual void v40() {  }
	virtual void v41(s_engine_settings *) {  }
	virtual void v42(byte, dword *, s_engine_settings *) {  }
	virtual bool v43(byte, s_engine_settings *) { return false; }
	virtual void v44(long, s_stats *) {  }
	virtual void v45(long, long *, long) {  }
	virtual bool v46(long, long) { return false; }
	virtual void v47(long, long, real *) {  }
	virtual bool v48(long, long, long, long) { return false; }
	virtual bool v49(short, dword, long, s_player_update *) { return false; }
	virtual long v50(long, long, long, long, long) { return 0; }
};

// Retail stores this engine object at 0x47fc8c.
c_game_engine_45c948 g_47fc8c;

/* slots 14 and 15 of the first, slot 14 of the second: wrappers that run on
   the peer object (see unknown_2bcdd0.cpp) */
class c_engine_peer_a : public c_engine_peer
{
public:
	virtual void q0(long, s_stats_a *);
	virtual void q1(dword *, long, s_settings_2bd *);
	virtual bool q2(dword, long, s_settings_2bd *);
};

class c_engine_peer_b : public c_engine_peer
{
public:
	virtual void q0(long, s_stats_b *);
	virtual void q1(dword *, long, s_settings_2c0 *);
	virtual bool q2(dword, long, s_settings_2c0 *);
};

// @retail 0x2bd960
void function_2bd960()
{
	s_state_2bd *state = g_51ecc8;

	state->p1ac = *g_468710;
	state->p1b8 = *g_468710;
	state->p1c4 = *g_468710;
	state->p1d0 = *g_468710;
}

// @retail 0x2bd9d0
bool c_game_engine_a::v5(long a, long b)
{
	s_player_2bd *player = (s_player_2bd *)(g_4e8c24->data + (a & 0xffff) * 0x21c);
	bool result = false;

	if (player->s1b8 > 0)
	{
		if (b == 2)
			result = g_4e6948->flags22c_bits.bit2;
		else if (b == 3)
			result = g_4e6948->flags22c_bits.bit3;
		else if (b == 1)
			result = g_4e6948->flags22c_bits.bit4;
	}
	else
		result = c_game_engine::v5(a, b);
	return result;
}

// @retail 0x2bda60
void c_game_engine_a::v0(long player_index, long other_player_index, bool flag, long)
{
	if (((s_player_2bd *)(g_4e8c24->data + (other_player_index & 0xffff) * 0x21c))->s1b8 > 0 &&
		!flag && player_index != NONE && player_index != other_player_index)
	{
		function_15b930(player_index, function_15eaf0(), 0x1c, 1);
	}
	if (player_index != NONE && !flag && player_index != other_player_index &&
		((s_player_2bd *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c))->s1b8 > 0)
	{
		function_15b930(player_index, function_15eaf0(), 0x1b, 1);
	}
}
// @retail 0x2bdb40
void c_engine_peer_a::q0(long, s_stats_a *stats)
{
	memset(stats, 0, sizeof(s_stats_a));
	p41((s_stats *)stats);
}

// @retail 0x2bdb80
void c_engine_peer_a::q1(dword *value, long, s_settings_2bd *settings)
{
	long result = 0;
	dword m = *value & 0x1f;

	if (m)
		p42(m, &result, (long)settings);
	s_state_2bd *state = g_51ecc8;
	if (*value & 0x20)
	{
		if (settings->s24 != state->l1a0)
		{
			settings->s24 = (short)state->l1a0;
			result |= 0x20;
		}
	}
	if (*value & 0x40)
	{
		if (settings->s26 != state->s1a8)
		{
			settings->s26 = state->s1a8;
			result |= 0x40;
		}
	}
	*value = result;
}

void hill_set(s_polygon_2be *hill, long index);

// @retail 0x2bdc00
bool c_engine_peer_a::q2(dword a, long, s_settings_2bd *settings)
{
	dword m = a & 0x1f;
	bool result = true;

	if (m)
		result = p43(m, (long)settings) != 0;
	s_state_2bd *state = g_51ecc8;
	if (a & 0x20)
	{
		long index = settings->s24;

		if (state->l1a0 != index)
		{
			state->l1a0 = index;
			hill_set((s_polygon_2be *)state, index);
		}
	}
	if (a & 0x40)
	{
		if (state->s1a8 != settings->s26)
		{
			state->s1a8 = settings->s26;
		}
	}
	return result;
}

// @retail 0x2bdd20
void function_2bdd20(s_spline_2bd *spline, point3f *points)
{
	for (long i = 0; i < spline->count; i++, points++)
	{
		s_palette_source_globals *globals = g_4e0350;
		s_marker_entry *entry = &globals->marker_entries[spline->indices[i]];

		if (points)
			*points = entry->position;
	}
}

// the four control points of segment n of a closed spline: the segment's two
// points and the midpoints with their outer neighbours
// score 43 (was 43): no frame; spline/index/point input ownership and first average schedule differ.
// native output-pointer result without index home 64 fast;
// ordinary inline declaration without index home 64 fast;
// pointer result and inline declaration 64 fast;
// inline direct output elements 65 fast;
// pointer result with inline direct elements 65 fast;
// conditional wrapped-index results 83 fast;
// wrapped indices with pointer result 83 fast;
// wrapped indices, pointer result and inline declaration 83 fast;
// spline/index/points entry convention and first source-address lifetimes remain.
// Indexed sums/scales/both 41/41/41; native sum/scale setters with index home 135/43;
// native middle copies 83; previous/both wrapped pointers from current 41/82. All restored.
// index remains stacked instead of spline; first average
// address/register schedule differs. With direct index: first/last/both shared
// scales, first/both point setters, source role names, late second-segment
// addresses and read-only source points 64/64/64/102/134/64/79/64.
// @retail 0x2bdd70
void function_2bdd70(s_spline_2bd *spline, point3f *points, long n, point3f *out)
{
	long i = n * 2;
	long previous = i - 1;
	long following = i + 1;
	long next = i + 2;

	if (previous < 0)
		previous += spline->count;
	if (next >= spline->count)
		next -= spline->count;

	point3f &first_output = out[0];
	point3f *a = &points[previous];
	point3f *b = &points[i];
	point3f *c = &points[following];
	point3f *d = &points[next];
	first_output.x = b->x + a->x;
	first_output.y = b->y + a->y;
	first_output.z = b->z + a->z;
	first_output.x *= 0.5f;
	first_output.y *= 0.5f;
	first_output.z *= 0.5f;
	out[1] = *b;
	out[2] = *c;
	point3f &last_output = out[3];
	last_output.x = d->x + c->x;
	last_output.y = d->y + c->y;
	last_output.z = d->z + c->z;
	last_output.x *= 0.5f;
	last_output.y *= 0.5f;
	last_output.z *= 0.5f;
}

/* the point at fraction t on the cubic Bezier segment through four control points */
// The parameters are listed as time, control points and output point so that the caller's
// arguments are written in the order retail evaluates them.
// @retail 0x2bde90
void function_2bde90(real t, point3f *points, point3f *out)
{
	real t3 = t * t * t;
	real t2 = t * t;
	real t1 = t;
	real m[4][4] =
	{
		{ -1.0f, 3.0f, -3.0f, 1.0f },
		{ 3.0f, -6.0f, 3.0f, 0.0f },
		{ -3.0f, 3.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f, 0.0f }
	};
	real r[4];
	long i = 0;

	do
	{
		r[i] = m[i][0] * t3 + m[i][1] * t2 + m[i][2] * t1 + m[i][3];
		i++;
	}
	while (i < 4);

	*out = *g_468788;
	out->x = points[0].x * r[0] + points[1].x * r[1] + points[2].x * r[2] + points[3].x * r[3];
	out->y = points[0].y * r[0] + points[1].y * r[1] + points[2].y * r[2] + points[3].y * r[3];
	out->z = points[0].z * r[0] + points[1].z * r[1] + points[2].z * r[2] + points[3].z * r[3];
}

/* ---- the hill's shape ---- */

/* builds the hill's polygon on the spline through its markers, and its
   center, radius, perimeter and height range (the height range also covers
   the hill's other markers) */
// score 18 (was 18): Sampling fraction/call setup and marker-loop registers remain; frame and retail length agree.
// Earlier at 25 (was 25): control/sampling conventions, radius base and marker registers remain.
// Earlier at 130 (was 134): control-call convention and scan induction differ;

// Earlier at 25 (was 25): frame 0x110 agrees; control/sampling conventions, radius base and marker registers differ.
// Earlier: control-call convention and scan induction differ;
// control-call convention and scan induction differ;
// segment control scope 141 fast; sample point scope 141 fast; late marker
// array 141 fast; unsigned edges 142 fast; separate marker counters 141 fast.
// Control/sampling calls and scan induction remain; center/radius/both values 171/169/185;
// vertex memcpy 190; edge-length values 139; main height value 130 plain kept;
// shared point count 326; height plus count 345 (plain).
// Earlier at 130 (was 134): protected control/sampling calls and scan induction remain;
// center/radius/both/height/all endpoint scans 375/475/716/378/950f; frame/size/global 237/276/1164f.
// Earlier at 130 (was 130): Protected control/sampling conventions and scan induction remain. external center binding 130; external
// Earlier: protected control/sampling calls and scan induction remain;
// Earlier: Protected control/sampling conventions and scan induction remain. external center binding 130; external
// Earlier 130 (was 134): protected control/sampling calls and scan induction remain;
// Earlier 130 (was 130): Protected control/sampling conventions and scan induction remain. external center binding 130; external
// radius binding 131; center radius bindings 131; external height bindings 151; native scan ordinal 130;
// center library initialization 137; marker library input 130; independent point ordinals 158.
// Protected control/sampling conventions and scan induction still differ.
// shared edge magnitude 158p; native radius displacement 130p; center component loop 138p; scale
// component loop 130p; marker position reference 133p; marker array cursor 165p; perimeter vertex
// references 130p; edge magnitude with radius value 158p.
// protected control/sampling calls and scan induction remain; main scalar
// bounds 143; shared center height 143; count snapshot 274, with center height 274; selections 143;
// minimum/maximum alone 140/135; main plus marker bounds 171; initialized bounds 133;
// initialized bounds with direct height reads 128 (two-point gain rejected). All plain.
// Frame 0x110 and protected helper conventions remain; independent center publication
// xzy/yxz/yzx/zxy/zyx 134/134/134/134/134; center scale xzy/yxz/zyx 130/130/130
// (all plain). Original arithmetic, scans and helper calls restored.
// Earlier at 25 (was 130): the calls to 0x2bdd70 (index in a stack slot, retail EDX) and 0x2bde90
// Earlier: the calls to 0x2bdd70 (index in a stack slot, retail EDX) and 0x2bde90
// Earlier 25 (was 130): the calls to 0x2bdd70 (index in a stack slot, retail EDX) and 0x2bde90
// (t in XMM3, retail XMM2), the radius loop's pointer base and the marker loop's registers differ.
// Element pointer for both radius components 22 (gain 3, not kept); marker index zeroed before its
// guard 20 (not kept); marker loop as a for 582 (unrolled); globals local in/before the marker loop
// 20/22; callee 0x2bdd70 without its index address read 43 -> 64 (index in ECX); (spline, points,
// n, out) order with an indexed body 86 (points on the stack).
// Native XY aggregate copy 187f; marker height 160f; unsigned/short marker ordinal 74f/92f;
// copy plus height/index 220f/187f; height plus index 97f; copy/height/index 201f.
// Unchanged reduced preview 74f versus full 25; all new forms restored.
// shared sample/marker point 74 fast;
// step field at each use 187 fast;
// shared point and direct steps 187 fast;
// both point-array role names 74 fast;
// marker workspace declared with points 74 fast;
// immutable marker point 74 fast;
// one marker-entry array observation 81 fast;
// entry array with shared point 81 fast;
// Round 318, rolled-scan body: shared displacement 75f; main height 80f;
// marker field reads at every use 77f; displacement/height 81f, displacement/marker 78f,
// height/marker 97f, all three 98f; direct marker reads with a plain for 460f.
// Reduced control 74 versus full 25; helper conventions and radius/marker homes remain. All restored.
// Round 331: parallel radius squares 138f; native marker count/ordinal 74f; both 74f;
// shared center setter 179f, with squares/count/both 171f each; shared marker setter 160f.
// Reduced control 74f versus full 25. All eight bodies restored.
// Separate native radius-center input 179f; independent marker count 138f;
// conditional marker bound updates 83f; center/count 159f; center/updates 142f;
// count/updates 83f; all 142f; XY center with count/updates 116f. Reduced control 74, full 25.
// Round 166: Native center record with direct-center/radius/shared-point graphs
// 72/137/69/135/72/137/69/135 fast (unchanged control 72, full 23); restored.
// Exact control-helper orders spline/points/index/output 70, with radius receiver 67;
// spline/points/output/index 72, with radius 69. First order plus receiver kept, plain 18.
// Round 218: Native numerator/denominator/quotient/sample names on exact control graph
// 154/250/67/186/154/250/186/186 fast; pre-guard/shared versus independent
// marker ordinals 68/134/134/134 fast (control 67, full 18). All restored.
// Round 144: sampling output/fraction/points order 67f; inline original and five
// coordinated orders 219f each; separate noinline declaration 67f. Reduced control
// 67f versus full 18; first sampling input XMM3/XMM2 and final marker roles remain.
// The marker loop reads its marker id through a local, and the sampling call takes
// the control points before the output point, which puts the two address
// computations in retail's order.
// @retail 0x2be050
void hill_build_polygon(s_polygon_2be *hill, long index)
{
	long half;
	long vertex;
	long marker_ids[8];
	point3f control[4];
	point3f point;
	point3f points[16];
	long i;
	long count;

	half = hill->count / 2;
	vertex = 0;
	function_2bdd20((s_spline_2bd *)hill, points);
	for (i = 0; i < half; i++)
	{
		long steps = hill->steps[i];

		function_2bdd70((s_spline_2bd *)hill, points, i, control);
		for (long j = 0; j < steps; j++)
		{
			function_2bde90((real)j / (real)steps, control, &point);
			hill->vertices[vertex].x = point.x;
			hill->vertices[vertex].y = point.y;
			vertex++;
		}
	}
	hill->perimeter = 0.0f;
	for (i = 0; i < 32; i++)
	{
		long previous = (i != 0) ? i - 1 : 31;
		real dx = hill->vertices[previous].x - hill->vertices[i].x;
		real dy = hill->vertices[previous].y - hill->vertices[i].y;

		hill->perimeter += (real)sqrt(dy * dy + dx * dx);
	}
	count = hill->count;
	*(point3f *)&hill->center_x = *g_468788;
	if (count > 0)
	{
		for (i = 0; i != count; i++)
		{
			hill->center_x = points[i].x + hill->center_x;
			hill->center_y = points[i].y + hill->center_y;
			hill->center_z = points[i].z + hill->center_z;
		}
	}
	real scale = 1.0f / (real)count;
	point3f *center = (point3f *)&hill->center_x;
	center->x = center->x * scale;
	center->y = center->y * scale;
	center->z = center->z * scale;
	hill->radius = 0.0f;
	if (count > 0)
	{
		for (i = 0; i != count; i++)
		{
			point3f const *radius_point = &points[i];
			real dx = radius_point->x - center->x;
			real dy = radius_point->y - center->y;
			real distance_squared = dy * dy + dx * dx;

			hill->radius = hill->radius > distance_squared ? hill->radius : distance_squared;
		}
	}
	hill->radius = (real)sqrt(hill->radius);
	hill->z_min = hill->center_z;
	hill->z_max = hill->center_z;
	count = hill->count;
	if (count > 0)
	{
		for (i = 0; i != count; i++)
		{
			if (hill->z_min > points[i].z)
			{
				hill->z_min = points[i].z;
			}
			if (!(hill->z_max > points[i].z))
			{
				hill->z_max = points[i].z;
			}
		}
	}
	count = function_19ec40(0, 0.0f, (short)(index + 11), NONE, 1, 8, marker_ids, 0.0f);
	i = 0;
	if (count > 0)
	{
		do
		{
			long marker_id = marker_ids[i];
			point3f position = g_4e0350->marker_entries[marker_id].position;

			hill->z_min = hill->z_min > position.z ? position.z : hill->z_min;
			hill->z_max = hill->z_max > position.z ? hill->z_max : position.z;
			i++;
		}
		while (i < count);
	}
	hill->z_min -= 0.1f;
	hill->z_max += 0.8f;
}

/* sets up the hill of the index: its markers, the vertices each spline
   segment gets (32 in all), and its polygon */
// score 2 (was 2): Post-loop pointer/index restores remain reversed; eight compiled source trials restored.
// Earlier: the post-loop pointer/index restores remain reversed.
// Final count and entry divisor 2/2; low-bit clear/remainder 64/63; endpoint
// parts/start-first 2/2; step cursor 10; parity first 6; polygon/index names 2/2.
// NONE arm first 60; inline validity helper 2; count at the top 2; no half local 2;
// validity bool 2; loop index outside 2; parameter copies 2; callee parameters swapped 2;
// modulo parity 19; half before the count store 59; while loop 2; count stored after the loop 57.
// Local count/divisor/index role names singly, in pairs and together 9 fast each;
// unsigned ordinal 23 fast. Reduced baseline 9; full baseline 2.
// Consumed count 9 fast; initialized divisor 61; shared start numerator 9;
// compound step 9; nested validation 9; division normalization 56;
// common exit 9; parallel endpoint numerators 9. Fast baseline 9, full 2.
// Parameter names in either sort order 2; polygon test nested 9 fast; loop index
// declared at the top 9 fast; count stored as one expression and halved from the
// field 9 fast; while loop 9 fast (the restore pair unchanged in each).
// Round 117 (fast, the restore pair read in each): loop bound `i != half` 9; step products as
// `32 * (i + 1)` and `32 * i` 9; hill copied into a local at entry 9; `(short)index + 11` 9; a
// test of index ahead of the count test (diagnostic) 33 with the pair unchanged, so the pair does
// not follow the next use; const-qualified parameters 9; a 600 s permuter run, 54 variants, none below 9.
// @retail 0x2bdc70
void hill_set(s_polygon_2be *hill, long index)
{
	if (index != NONE)
	{
		long count = function_19ec40(0, 0.0f, (short)(index + 11), NONE, 0, 16, hill->ids, 0.0f);

		if (count >= 4)
		{
			long half;

			count -= count & 1;
			hill->count = count;
			half = count / 2;
			for (long i = 0; i < half; i++)
			{
				hill->steps[i] = (i + 1) * 32 / half - i * 32 / half;
			}
		}
		if (hill->count >= 4 && !(hill->count & 1))
		{
			hill_build_polygon(hill, index);
		}
	}
	else
	{
		hill->count = 0;
	}
}

/* ---- the hill's color ---- */

/* a player, as the hill's color reads it */
struct s_player_2be
{
	byte unknown00[0x88];
	byte type;
	byte unknown89[0xc0 - 0x89];
	char team;
	byte unknownc1[0x1b8 - 0xc1];
	short time_inside;
	byte unknown1ba[0x21c - 0x1ba];
};

struct s_player_iterator_2be
{
	s_player_2be *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

bool player_iterator_next_with_unit(long *iterator);
extern color3f *g_468714;
color3f *function_7f720(color3f *color, short team_index);

static inline s_player_2be *player_get_2be(long index)
{
	return (s_player_2be *)(g_4e8c24->data + (index & 0xffff) * 0x21c);
}

/* whether the game is played in teams */
static inline bool game_is_team_game_2be()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		result = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
	}
	return result;
}

/* the local player's marker color */
static __forceinline void local_marker_color_2be(s_color_bits *color)
{
	s_color_bits *marker = (s_color_bits *)&g_468c80[0].red;

	if (g_4b9ed8 != NONE)
	{
		long local_player_index = g_4e8c20->entries[g_4b9ed8];

		if (local_player_index != NONE)
		{
			s_player_2be *player = (s_player_2be *)(g_4e8c24->data + (local_player_index & 0xffff) * 0x21c);

			if (player->type == 1 || player->type == 3)
			{
				marker = (s_color_bits *)&g_468c80[1].red;
			}
		}
	}
	*color = *marker;
}

/* the hill's color as the player sees it: neutral when nobody, or both the
   player's team and another, stand on it; else the color of the team on it */
// @retail 0x2be3b0
void hill_color(long player_index, s_color_bits *color)
{
	if (player_index != NONE)
	{
		s_player_2be *player = player_get_2be(player_index);
		bool friendly = false;
		bool enemy = false;
		long occupant = NONE;
		s_player_iterator_2be iterator;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			if ((1 << (byte)iterator.index) & (word)g_51ecc8->s1a8)
			{
				if (iterator.player->team == player->team)
				{
					occupant = iterator.index;
					friendly = true;
				}
				else
				{
					occupant = iterator.index;
					enemy = true;
				}
			}
		}
		if (friendly)
		{
			if (enemy)
			{
				*color = *(s_color_bits *)g_468714;
				return;
			}
		}
		else if (!enemy)
		{
			*color = *(s_color_bits *)g_468714;
			return;
		}
		if (game_is_team_game_2be())
		{
			color3f team_color;
			long team = player_get_2be(occupant)->team;

			*color = *(s_color_bits *)function_7f720(&team_color, team);
		}
		else
		{
			local_marker_color_2be(color);
		}
	}
	else
	{
		local_marker_color_2be(color);
	}
}

/* a player's unit, as the hill's marker reads it */
struct s_player_unit_2be
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

/* the hill's marker for the local player: in the hill's color, faded by how
   far the player stands outside it */
// @retail 0x2be6d0
void hill_marker(long local_player, s_polygon_2be *hill)
{
	if (hill->count >= 4 && !(hill->count & 1))
	{
		real fade = 1.0f;
		long player_index;
		s_color_bits color;
		s_marker_list list;

		if (local_player != NONE)
		{
			player_index = g_4e8c20->entries[local_player];
			if (player_index != NONE)
			{
				s_player_unit_2be *player = (s_player_unit_2be *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);

				if (player->unit_index != NONE)
				{
					point3f position;
					real dx;
					real dy;
					real distance;

					function_b9dd0(player->unit_index, &position);
					dx = hill->center_x - position.x;
					dy = hill->center_y - position.y;
					distance = dx * dx;
					distance += dy * dy;
					distance = (real)sqrt(distance) / hill->radius - 1.0f;
					if (0.0f > distance)
					{
						fade = 0.0f;
					}
					else if (distance > 1.0f)
					{
						fade = 1.0f;
					}
					else
					{
						fade = distance;
					}
				}
			}
		}
		player_index = NONE;
		if (local_player != NONE)
		{
			player_index = g_4e8c20->entries[local_player];
		}
		hill_color(player_index, &color);
		list.b0 = 1;
		list.b1 = 0;
		list.l4 = 1;
		list.position = *(point3f *)&hill->center_x;
		list.r14 = 0.0f;
		list.r18 = 0.1f;
		list.r1c = 0.0f;
		list.l20 = NONE;
		list.color24 = color;
		list.color30 = color;
		list.r3c = fade;
		list.r40 = 1.0f;
		list.count = 1;
		list.items[0].kind = 6;
		list.items[0].a = color;
		list.items[0].b = color;
		list.items[0].r = 1.0f;
		list.items[0].index = NONE;
		function_24e59f(&list);
	}
}

// @retail 0x2be880
bool function_2be880(long player_index, s_polygon_2be *polygon)
{
	bool result = false;

	if (player_index != NONE && polygon->count >= 4 && !(polygon->count & 1))
	{
		s_player_2bd *player = (s_player_2bd *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);

		if (player->object_index != NONE)
		{
			point3f position;

			function_b9dd0(player->object_index, &position);
			if (position.z >= polygon->z_min && polygon->z_max >= position.z)
			{
				real dx = polygon->center_x - position.x;
				real dy = polygon->center_y - position.y;

				real radius_squared = polygon->radius * polygon->radius;
				real center_distance_squared = dx * dx;
				center_distance_squared += dy * dy;
				if (center_distance_squared < radius_squared)
				{
					long crossings = 0;

					for (long i = 0; i < 32; i++)
					{
						long j = (i != 0) ? i - 1 : 31;
						real d = (polygon->vertices[i].y - polygon->vertices[j].y) * -100.0f;

						if (fabs(d) > 1e-4f)
						{
							real a = polygon->vertices[j].y - position.y;
							real t = a * 100.0f;
							real inv = 1.0f / d;
							t *= inv;
							real s =((polygon->vertices[i].x - polygon->vertices[j].x) * a - (polygon->vertices[i].y - polygon->vertices[j].y) * (polygon->vertices[j].x - position.x)) * inv;

							if (t >= 0.0f && t < 1.0f && s >= 0.0f && s < 1.0f)
								crossings++;
						}
					}
					result = crossings & 1;
				}
			}
		}
	}
	return result;
}

// @retail 0x2bf310
bool c_game_engine_45c948::v1()
{
	s_state_2bf *state = (s_state_2bf *)((byte *)g_4e9ae8 + 0xfc);

	memset(state, 0, 0x118);
	for (long i = 0; i < 8; ++i)
		state->w60[i] = (word)NONE;
	for (long j = 0; j < 8; j++)
		state->l70[j] = NONE;
	g_51eccc = state;

	short raw_n = g_4e6948->s22e;
    long n = raw_n < 1 ? 1 : raw_n;
	state->w110 = g_510c54->field_2_3 * n;

	short raw_m = g_4e6948->s230;
    long m = raw_m < 1 ? 1 : raw_m;
	state->w112 = m * g_510c54->field_2_3;
	state->w114 = g_4e6948->w22c;
	return true;
}

struct s_team_entry;
s_team_entry *function_15e410(short team);
void function_15e130(long object_index);
void __stdcall function_b8540(long a);
void function_b58c0(long index, dword mask);

/* a team's entry, as the territories read it */
struct s_team_entry_2bf
{
	long item_index;
};

/* the players' unit, as the territories read it */
struct s_player_unit_2bf
{
	byte unknown00[0x2c];
	long unit_index;
};

/* a player iterator: the current player before the iterator (0x19f240) */
struct s_player_iterator_2bf
{
	s_player_unit_2bf *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

bool player_iterator_next(long *iterator);

/* marks the parts of the engine's globals in the mask changed */
static inline void game_engine_globals_changed_mask_2bf(dword mask)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
	{
		function_b58c0(g_4e9ae8->value24, mask);
	}
}

/* marks the engine's globals changed */
static inline void game_engine_globals_changed_2bf()
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
	{
		function_b58c0(g_4e9ae8->value24, 0x20);
	}
}

/* drops the team's item at the territory */
static inline void territory_drop_team_item_2bf(long team)
{
	s_team_entry_2bf *entry = (s_team_entry_2bf *)function_15e410((short)team);

	if (entry)
	{
		long item_index = entry->item_index;

		if (item_index != NONE)
		{
			function_15e130(item_index);
			function_b8540(item_index);
		}
	}
}

/* releases every territory that is held */
// @retail 0x2bf3d0
bool c_game_engine_a::v25()
{
	for (long i = 0; i < (short)g_51eccc->w114; i++)
	{
		s_state_2bf *state = g_51eccc;

		if ((short)state->w60[i] != NONE && state->l70[i] != NONE)
		{
			state->l70[i] = NONE;
			game_engine_globals_changed_2bf();
			territory_drop_team_item_2bf(i);
		}
	}
	return true;
}

/* releases the territories the team holds */
// @retail 0x2bf4e0
void c_game_engine_a::v32(long team)
{
	if (g_4e6948->mode != 4)
	{
		for (long i = 0; i < (short)g_51eccc->w114; i++)
		{
			s_state_2bf *state = g_51eccc;

			if ((short)state->w60[i] != NONE && state->l70[i] == team && state->l70[i] != NONE)
			{
				state->l70[i] = NONE;
				game_engine_globals_changed_2bf();
				territory_drop_team_item_2bf(i);
			}
		}
	}
}

/* a point in the list below, with the values it is shown with */
struct s_point_list_item_2bf
{
	point3f position;
	real values[5];
};

/* a list of points to show (0x1004 bytes) */
struct s_point_list_2bf
{
	long count;
	s_point_list_item_2bf items[0x80];
};

/* the values the points get (unknown_157450.cpp) */
extern dword g_502258[0x27];

/* adds a point to the list, unless it is full */
static __forceinline void point_list_add_2bf(s_point_list_2bf *list, point3f position, real a, real b, real c, real d, real e)
{
	if (list->count < 0x80)
	{
		s_point_list_item_2bf *item = &list->items[list->count++];

		item->position = position;
		item->values[0] = a;
		item->values[1] = b;
		item->values[2] = c;
		item->values[3] = d;
		item->values[4] = e;
	}
}

/* adds the markers of the territories that the player or the player's team
   holds to the list */
// @retail 0x2bf740
void c_game_engine_45c948::v32(long player_index, long list_pointer)
{
	s_point_list_2bf *list = (s_point_list_2bf *)list_pointer;
	s_state_2bf *state = g_51eccc;

	for (long i = 0; i < (short)g_51eccc->w114; i++)
	{
		if ((short)state->w60[i] != NONE)
		{
			if (state->l70[i] == player_index ||
				(state->l70[i] != NONE && datum_get_inlined(g_4e8c24, state->l70[i]) &&
				 player_get_2be(state->l70[i])->team == player_get_2be(player_index)->team))
			{
				point3f const *source = &g_4e0350->marker_entries[(short)state->w60[i]].position;
				point_list_add_2bf(list, *source,
					*(real *)&g_502258[35], *(real *)&g_502258[36], *(real *)&g_502258[0], *(real *)&g_502258[1], *(real *)&g_502258[37]);
			}
		}
	}
}

PRIVATE __forceinline void territory_marker_extents_include(s_state_2bf *state, long index, point3f const *origin, point3f const *point)
{
    vector3f delta;
    vector3d_from_points3d(origin, point, &delta);
    real square = delta.j * delta.j + delta.i * delta.i;
    real distance = (real)sqrt(square);
    real below = origin->z - point->z;
    real above = point->z - origin->z + 0.8f;
    state->f0[index] = MAX(state->f0[index], distance);
    state->f20[index] = MAX(state->f20[index], below);
    state->f40[index] = MAX(state->f40[index], above);
}

PRIVATE __forceinline void territory_marker_position_copy(point3f *destination, point3f const *source)
{
    *destination = *source;
}

// @retail 0x2bf5b0
void c_game_engine_45c948::v12()
{
	s_state_2bf *state = g_51eccc;
	s_palette_source_globals *globals = g_4e0350;

	for (long i = 0; i < 8; i++)
	{
		short n = g_4e6948->w22c;

		if (n == 0 || n > i)
		{
			long ids[8];
			long count = function_19ec40(0, 0.0f, 10, NONE, i, 8, ids, 0.0f);

			if (count >= 1)
			{
				state->w60[i] = (word)ids[0];
				point3f p0;
				territory_marker_position_copy(&p0, &globals->marker_entries[ids[0]].position);
				state->f0[i] = 1.0f;
				state->f20[i] = 0.1f;
				state->f40[i] = 0.9f;
				for (long j = 1; j < count; j++)
				{
					point3f p;
					territory_marker_position_copy(&p, &globals->marker_entries[ids[j]].position);
					territory_marker_extents_include(state, i, &p0, &p);
				}
			}
		}
	}
}

/* retail fills this event field by field, as the other engines' event
   senders do, not through the shared initializer */
// @retail 0x2bf480
void c_game_engine_45c948::v6(long a)
{
	s_event event;

	event.type = 9;
	event.subtype = 0;
	event.cause_player_index = NONE;
	event.cause_team = NONE;
	event.effect_player_index = NONE;
	event.effect_team = NONE;
	event.f = 0;
	event.a = a;
	event.g = NONE;
	game_engine_event_send_inline(&event);
}

/* an object's header (12 bytes) and the object's definition, as v38 reads
   them */
struct s_object_header_2bf
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	long *object;
};

struct s_weapon_definition_2bf
{
	byte unknown000[0x290];
	short value290;
};

/* whether the object can be picked up: not a weapon whose definition says
   value290 is 1 */
/* each player's progress taking the territory they stand in, or leaving
   the one they stood in */
// @retail 0x2bfc40
void territories_update_players()
{
	s_player_iterator_2bf iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		s_state_2bf *state = g_51eccc;
		long player = iterator.index & 0xffff;

		if (iterator.player->unit_index != NONE)
		{
			point3f position;
			bool inside = false;

			function_b9dd0(iterator.player->unit_index, &position);
			for (long i = 0; i < (short)state->w114; i++)
			{
				short marker = (short)state->w60[i];

				if (marker != NONE)
				{
					point3f center;
					territory_marker_position_copy(&center, &g_4e0350->marker_entries[marker].position);
					real dx = position.x - center.x;
					real dy = position.y - center.y;
					real distance_squared = dy * dy + dx * dx;

					if (distance_squared < state->f0[i] * state->f0[i] &&
						state->f40[i] > position.z - center.z &&
						state->f20[i] > center.z - position.z)
					{
						inside = true;
						if ((char)state->entries90[player].a != i)
						{
							state->entries90[player].a = (byte)i;
							if ((char)state->entries90[player].previous != i)
							{
								state->entries90[player].time_inside = 0;
								state->entries90[player].time_outside = 0;
								state->entries90[player].b = 0;
							}
							state->entries90[player].previous = 0xff;
							game_engine_globals_changed_mask_2bf(1 << (player + 6));
						}
					}
				}
			}
			if (!inside)
			{
				long time;

				if (state->entries90[player].a != 0xff)
				{
					state->entries90[player].previous = state->entries90[player].a;
					state->entries90[player].a = 0xff;
					game_engine_globals_changed_mask_2bf(1 << (player + 6));
				}
				if (state->entries90[player].time_inside > 0)
				{
					time = MAX(state->entries90[player].time_inside - 3, 0);
					state->entries90[player].time_outside = 0;
					state->entries90[player].time_inside = (short)time;
					state->entries90[player].b = (byte)((short)time * 63 / (short)state->w110);
					game_engine_globals_changed_mask_2bf(1 << (player + 6));
				}
				else if (state->entries90[player].time_outside > 0)
				{
					time = MAX(state->entries90[player].time_outside - 3, 0);
					state->entries90[player].time_inside = 0;
					state->entries90[player].time_outside = (short)time;
					state->entries90[player].b = (byte)((short)time * 63 / (short)state->w112);
					game_engine_globals_changed_mask_2bf(1 << (player + 6));
				}
			}
		}
		else
		{
			state->entries90[player].previous = 0xff;
			if (state->entries90[player].a != 0xff)
			{
				state->entries90[player].a = 0xff;
				state->entries90[player].time_inside = 0;
				state->entries90[player].time_outside = 0;
				state->entries90[player].b = 0;
				game_engine_globals_changed_mask_2bf(1 << (player + 6));
			}
		}
	}
}
// @retail 0x2bfbe0
bool c_game_engine_a::v38(long player_index, long object_index)
{
	bool result = true;
	s_object_header_2bf *header = &((s_object_header_2bf *)g_4e0300->data)[object_index & 0xffff];

	if ((1 << header->type) & 4)
	{
		if (((s_weapon_definition_2bf *)g_4e3b44[*header->object & 0xffff].bytes)->value290 == 1)
		{
			result = false;
		}
	}
	return result;
}
// @retail 0x2c02e0
bool c_game_engine_b::v5(long a, long b)
{
	return c_game_engine::v5(a, b);
}

/* the scenario's string list, as v20 reads it */
struct s_scenario_2c0
{
	byte unknown000[0x3a4];
	long string_list_tag_index;
};

/* a string of the scenario's string list */
inline void scenario_get_string(long string_handle, word *buffer)
{
	long tag_index = ((s_scenario_2c0 *)g_4e0350)->string_list_tag_index;

	buffer[0] = 0;
	if (tag_index != NONE)
	{
		function_1a0180(tag_index, string_handle, buffer);
	}
}

/* writes the name of the event's territory for the #territory_name token */
// @retail 0x2c02f0
long c_game_engine_b::v20(long token, long token_length, long event_pointer, long destination_pointer, long remaining)
{
	s_event *event = (s_event *)event_pointer;
	word *destination = (word *)destination_pointer;
	long result = 0;
	word text[0x100];

	if (!wcsncmp(L"#territory_name", (const wchar_t *)token, token_length) && event && event->g != NONE)
	{
		long string_handle;

		switch (event->g)
		{
		case 0:
			string_handle = 0x1a0006c5;
			break;
		case 1:
			string_handle = 0x1a0006c6;
			break;
		case 2:
			string_handle = 0x1a0006c7;
			break;
		case 3:
			string_handle = 0x1a0006c8;
			break;
		case 4:
			string_handle = 0x1a0006c9;
			break;
		case 5:
			string_handle = 0x1a0006ca;
			break;
		case 6:
			string_handle = 0x1a0006cb;
			break;
		case 7:
			string_handle = 0x1a0006cc;
			break;
		}
		scenario_get_string(string_handle, text);
		result = unicode_string_vsnprintf(destination, remaining, L"%s", text);
		if (result < 0)
		{
			result = remaining;
		}
	}
	return result;
}
// @retail 0x2c0410
void c_engine_peer_b::q0(long, s_stats_b *stats)
{
	memset(stats, 0, sizeof(s_stats_b));
	memset(stats->b, 0xff, sizeof(stats->b));
	p41((s_stats *)stats);
}

/* copies the territories' holders and the entries the mask selects into
   the settings, and returns the mask of what changed */
// @retail 0x2c0450
void c_engine_peer_b::q1(dword *value, long unused, s_settings_2c0 *settings)
{
	long result = 0;
	dword m = *value & 0x1f;

	if (m)
		p42(m, &result, (long)settings);
	s_state_2bf *state = g_51eccc;
	if (*value & 0x20)
	{
		bool changed = false;

		for (long i = 0; i < (short)state->w114; i++)
		{
			if ((short)state->w60[i] != NONE)
			{
				long holder = state->l70[i] == NONE ? NONE : state->l70[i] & 0xffff;

				if (settings->holders[i] != holder)
				{
					settings->holders[i] = holder;
					changed = true;
				}
			}
		}
		if (changed)
			result |= 0x20;
		else
			result &= ~0x20;
	}
	for (long j = 0; j < 16; j++)
	{
		dword bit = 1 << (j + 6);

		if (*value & bit)
		{
			if (settings->a[j] != state->entries90[j].a || settings->b[j] != state->entries90[j].b)
			{
				settings->a[j] = state->entries90[j].a;
				settings->b[j] = state->entries90[j].b;
				result |= bit;
			}
		}
	}
	*value = result;
}

struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
void __stdcall function_a7870(long object_index);
void __stdcall function_a7810(dword mask);
void __stdcall function_be240(long object_index, dword color_mask, color3f const *colors);
void function_15e050(long object_index, short value);
bool function_15f330(s_player_appearance const *appearance, short team_index, color3f *colors);
bool function_15b7c0(long delta, long player_index);
long game_engine_get_player_score(dword player_index);
void player_announce_points_to_win(long player_index, long score);

/* The territory item uses its holder's four appearance colors. */
struct s_territory_placement
{
	byte unknown00[0x1c];
	point3f position;
	byte unknown28[0x74 - 0x28];
	dword color_mask;
	color3f colors[4];
	long value_a8;
	byte unknownac[0xc4 - 0xac];
};

struct s_territory_item
{
	byte unknown00[0x17e];
	short territory;
};

struct s_territory_tag_data
{
	long unknown00;
	long item_tag;
};

struct s_territory_tag
{
	byte unknown00[0xc];
	s_territory_tag_data *data;
};

// score 24 (was 24): the tag's data pointer is loaded into ecx where retail
// keeps it in eax, the marker index is kept in edx where retail uses eax, and
// the colour mask is read at the top of the function instead of just before
// it is changed.
// Tried: the player lookup before the position copy 29; the mask update first
// 50, or between the copy and the player lookup 32; the tag chain as byte
// offsets 24; a const player pointer 24; a 600 s permuter run, none below 24.
// Round 16: the existing position-copy helper 43; game_engine_get() 24; the tag index as
// a ternary 41; placement declared first 24; tag index at function scope 48; placement at
// function scope 24; the test without the short cast 51; player declared at the block top
// 24; the created object in its own local 24; an early NONE return 62.
// Round 56: inline definition 24; copy helper with masked player index 43, with
// marker-globals/tag-table/tag-header locals 43/43/43; direct copy with masked player
// index 24; helper with tag reference 43; helper/else/tag reference 65 (271 bytes).
// Round 11 (lane 18), fast: the copy helper with the tag index set per arm 65 (271 bytes; every line
// after the 0xb7930 call is retail's, the NONE lands after the test); the helper with the initialised
// index 43 (the chain's middle pointer takes ECX and every later scratch register rotates by one); the
// chain in a small inline getter 43; the condition as a conditional 0xb7930 argument 65; the chain
// walked through one reassigned byte pointer 43; the chain's last step as data[0].item_tag 43; the
// marker index read once into a short local for the test and the copy 84 (281 bytes).
// @retail 0x2c0980
long territory_create_item(long player_index, long territory)
{
	long result = NONE;

	if ((short)g_51eccc->w60[territory] != NONE)
	{
		long tag_index = NONE;
		s_territory_placement placement;

		if (g_55e4d0[g_4e9ae8->engine_index])
		{
			tag_index = ((s_territory_tag *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->data->item_tag;
		}
		function_b7930(&placement, tag_index, NONE, 0);
		placement.position = g_4e0350->marker_entries[(short)g_51eccc->w60[territory]].position;
		s_player_2be *player = player_get_2be(player_index);
		placement.color_mask |= 15;
		function_15f330((s_player_appearance const *)((byte *)player + 0x84), player->team, placement.colors);
		placement.value_a8 = *(long *)((byte *)player + 0x89);
		result = function_b7b40(&placement);
		if (result != NONE)
		{
			function_a7870(result);
			function_15e050(result, (short)territory);
		}
	}
	return result;
}

// @retail 0x2c0b70
void territory_update_item_colors(long object_index)
{
	s_territory_item *item = (s_territory_item *)((s_object_header_2bf *)g_4e0300->data)[object_index & 0xffff].object;
	long territory = item->territory;
	color3f colors[4];

	if (territory >= 0 && territory < (short)g_51eccc->w114)
	{
		long holder = g_51eccc->l70[territory];

		if (holder != NONE)
		{
			s_player_2be *player = (s_player_2be *)record_pool_lookup(g_4e8c24, holder);

			if (player)
			{
				function_15f330((s_player_appearance const *)((byte *)player + 0x84), player->team, colors);
				function_be240(object_index, 15, colors);
			}
		}
	}
}

// @retail 0x2c0740
void territory_set_holder(long holder, long territory, bool silent)
{
	long previous = g_51eccc->l70[territory];

	if (previous != holder)
	{
		g_51eccc->l70[territory] = holder;
		game_engine_globals_changed_2bf();
		territory_drop_team_item_2bf(territory);
		if (!silent)
		{
			if (previous != NONE && record_pool_lookup(g_4e8c24, previous))
			{
				function_15b930(previous, function_15eaf0(), 0x2c, 1);
			}
			if (holder != NONE && record_pool_lookup(g_4e8c24, holder))
			{
				function_15b930(holder, function_15eaf0(), 0x2b, 1);
			}
		}
		if (holder != NONE)
		{
			territory_create_item(holder, territory);
		}
		if (!silent)
		{
			s_event event;

			if (previous != NONE && record_pool_lookup(g_4e8c24, previous))
			{
				game_engine_event_initialize_inline(&event, 9, function_15eaf0() ? 5 : 2);
				event.cause_player_index = holder;
				event.effect_player_index = previous;
				if (function_15eaf0())
				{
					if (holder != NONE)
					{
						event.cause_team = player_get_2be(holder)->team;
					}
					event.effect_team = player_get_2be(previous)->team;
				}
				event.g = (short)territory;
				function_19eb90(&event);
			}
			if (holder != NONE)
			{
				game_engine_event_initialize_inline(&event, 9, function_15eaf0() ? 4 : 1);
				event.cause_player_index = holder;
				if (function_15eaf0())
				{
					event.cause_team = player_get_2be(holder)->team;
				}
				event.g = (short)territory;
				game_engine_event_send_inline(&event);
			}
		}
	}
}

// @retail 0x2c0a90
void territories_update_scores()
{
	bool tick = g_510c54->game_time % g_510c54->field_2_3 == 0;

	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->w6c == 1 &&
		(g_4e6948->mode == 4 || g_4e9ae8->lc04 == 1) && tick)
	{
		for (long i = 0; i < (short)g_51eccc->w114; i++)
		{
			s_state_2bf *state = g_51eccc;

			if ((short)state->w60[i] != NONE && state->l70[i] != NONE)
			{
				long holder = state->l70[i];
				long old_score = game_engine_get_player_score(holder);

				function_15b7c0(1, holder);
				long score = game_engine_get_player_score(holder);
				if (score != old_score)
				{
					player_announce_points_to_win(holder, score);
				}
			}
		}
	}
}

PRIVATE inline void advance_territory_counter(short *reset, short *counter)
{
	*reset = 0;
	++*counter;
}

// @retail 0x2bff80
void territories_update_holders()
{
	dword changed = 0;
	s_player_iterator_2bf iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	bool active = player_iterator_next((long *)&iterator);
	while (active)
	{
		long player = iterator.index & 0xffff;
		long territory = (char)g_51eccc->entries90[player].a;

		if (territory != NONE && iterator.player->unit_index != NONE)
		{
			long holder = g_51eccc->l70[territory];

			if (holder != NONE)
			{
				holder &= 0xffff;
				if (holder != player)
				{
					short team = ((s_player_2be *)iterator.player)->team;
					short holder_team = ((s_player_2be *)g_4e8c24->data)[holder].team;
					c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
					if (engine)
					{
						bool opposing = engine->p27(holder_team, team);
						if (opposing)
						{
							if ((char)g_51eccc->entries90[holder].a == territory)
							{
								g_51eccc->entries90[player].time_inside = 0;
								g_51eccc->entries90[player].time_outside = 0;
								g_51eccc->entries90[player].b = 0;
								function_a7810(1 << (player + 6));
							}
							else
							{
								advance_territory_counter(&g_51eccc->entries90[player].time_outside, &g_51eccc->entries90[player].time_inside);
								if (g_51eccc->entries90[player].time_inside < (short)g_51eccc->w110)
								{
									g_51eccc->entries90[player].b = (byte)(g_51eccc->entries90[player].time_inside * 63 / (short)g_51eccc->w110);
									function_a7810(1 << (player + 6));
								}
								else
								{
									territory_set_holder(NONE, territory, false);
									changed |= 1 << territory;
								}
							}
						}
					}
				}
			}
			else
			{
				advance_territory_counter(&g_51eccc->entries90[player].time_inside, &g_51eccc->entries90[player].time_outside);
				if (g_51eccc->entries90[player].time_outside < (short)g_51eccc->w112)
				{
					g_51eccc->entries90[player].b = (byte)(g_51eccc->entries90[player].time_outside * 63 / (short)g_51eccc->w112);
					game_engine_globals_changed_mask_2bf(1 << (player + 6));
				}
				else
				{
					territory_set_holder(iterator.index, territory, false);
					changed |= 1 << territory;
				}
			}
		}
		active = player_iterator_next((long *)&iterator);
	}
	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		long player = iterator.index & 0xffff;
		long territory = (char)g_51eccc->entries90[player].a;

		if (territory != NONE && iterator.player->unit_index != NONE && (changed & (1 << territory)))
		{
			g_51eccc->entries90[player].time_outside = 0;
			g_51eccc->entries90[player].time_inside = 0;
			g_51eccc->entries90[player].b = 0;
			game_engine_globals_changed_mask_2bf(1 << (player + 6));
		}
	}
}

// @retail 0x2c0230
void c_game_engine_a::v40()
{
	if (g_4e6948->mode != 4 && !g_4e6948->flag1128)
	{
		territories_update_players();
		for (long i = 0; i < (short)g_51eccc->w114; i++)
		{
			s_state_2bf *state = g_51eccc;

			if ((short)state->w60[i] != NONE && state->l70[i] != NONE)
			{
				byte *player = record_pool_lookup(g_4e8c24, state->l70[i]);

				if (!player)
				{
					territory_set_holder(NONE, i, false);
				}
				else if (player[2] & 2)
				{
					territory_set_holder(NONE, i, false);
				}
			}
		}
		territories_update_holders();
		territories_update_scores();
	}
}

struct s_spawn_influence_list;
void function_23ba10(long type, point3f const *point, s_spawn_influence_list *list);

// @retail 0x2bdb00
void c_game_engine_a::v2(long unused, long list_pointer)
{
	long count = ((s_polygon_2be *)g_51ecc8)->count;

	if (count >= 4 && !(count & 1))
	{
		point3f const *point = (point3f const *)&((s_polygon_2be *)g_51ecc8)->center_x;

		function_23ba10(8, point, (s_spawn_influence_list *)list_pointer);
		function_23ba10(7, point, (s_spawn_influence_list *)list_pointer);
	}
}

// @retail 0x2c0400
void c_game_engine_a::v43(long object_index)
{
	territory_update_item_colors(object_index);
}

void function_15fe20(long team_index, color3f *color);

// @retail 0x2bf8d0
void territory_marker_color(long player_index, long territory, s_color_bits *color)
{
	if (player_index != NONE && g_51eccc->l70[territory] != NONE &&
		record_pool_lookup(g_4e8c24, g_51eccc->l70[territory]))
	{
		long holder = g_51eccc->l70[territory];

		if (function_15eaf0())
		{
			function_15fe20(player_get_2be(holder)->team, (color3f *)color);
		}
		else
		{
			*color = *function_13927e(g_4b9ed8);
		}
	}
	else
	{
		*color = *(s_color_bits *)g_468714;
	}
}

// @retail 0x2bf960
void c_game_engine_45c948::v14(long local_player)
{
	long player_index = NONE;
	if (local_player != NONE)
		player_index = g_4e8c20->entries[local_player];
	s_state_2bf *state = g_51eccc;
	for (long i = 0; i < (short)state->w114; ++i)
	{
		if ((short)state->w60[i] != NONE)
		{
			s_color_bits color;
			territory_marker_color(player_index, i, &color);
			s_marker_list list;
			s_marker_entry const &marker = g_4e0350->marker_entries[(short)state->w60[i]];
			list.b0 = 0;
			list.b1 = 0;
			list.l4 = 1;
			list.position = marker.position;
			long holder = state->l70[i];
			list.r14 = 0.4f;
			list.r18 = 0.1f;
			list.r1c = 0.0f;
			list.l20 = NONE;
			if (!datum_get_inlined(g_4e8c24, holder))
				list.r14 = 0.0f;
			list.r3c = 1.0f;
			list.r40 = 1.0f;
			list.count = 1;
			list.items[0].kind = 7;
			list.items[0].a = color;
			list.items[0].b = color;
			list.items[0].r = 1.0f;
			list.items[0].index = NONE;
			list.color24 = color;
			list.color30 = color;
			function_24e59f(&list);
			state = g_51eccc;
		}
	}
	if (player_index != NONE)
	{
		s_player_2be *player = player_get_2be(player_index);
		if (*(long *)((byte *)player + 0x2c) != NONE)
		{
			s_player_iterator_2bf iterator;
			iterator.data = g_4e8c24;
			iterator.absolute_index = NONE;
			iterator.index = NONE;
			while (player_iterator_next((long *)&iterator))
			{
				long other = iterator.index;
				if (other != player_index)
				{
					short own_team = player->team;
					short other_team = ((s_player_2be *)iterator.player)->team;
					c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
					bool friendly = false;
					if (engine)
						friendly = engine->p27(other_team, own_team);
					if (!friendly)
					{
						s_marker_list list;
						if (function_162550(other, &list))
							function_24e59f(&list);
					}
				}
			}
		}
	}
}

static inline byte *territory_player_from_slot(s_record_pool *data, long index)
{
	byte *result = 0;

	if (index != NONE && index >= 0 && index < data->high_water_index)
	{
		byte *player = data->data + data->size * index;

		if (*(short *)player != 0)
		{
			result = player;
		}
	}
	return result;
}

// @retail 0x2c05f0
bool c_engine_peer_b::q2(dword mask, long unused, s_settings_2c0 *settings)
{
	dword base_mask = mask & 0x1f;
	bool result = true;

	if (base_mask)
	{
		result = p43(base_mask, (long)settings) != 0;
	}
	s_state_2bf *state = g_51eccc;
	if (mask & 0x20)
	{
		for (long i = 0; i < (short)g_51eccc->w114; i++)
		{
			if ((short)state->w60[i] != NONE)
			{
				long slot = settings->holders[i];
				byte *player = territory_player_from_slot(g_4e8c24, slot);
				long holder = NONE;

				if (player)
				{
					holder = data_datum_index(g_4e8c24, slot);
				}
				state->l70[i] = holder;
				s_team_entry_2bf *entry = (s_team_entry_2bf *)function_15e410((short)i);
				if (entry && entry->item_index != NONE)
				{
					territory_update_item_colors(entry->item_index);
				}
			}
		}
	}
	for (long j = 0; j < 16; j++)
	{
		if (mask & (1 << (j + 6)))
		{
			state->entries90[j].a = settings->a[j];
			state->entries90[j].b = settings->b[j];
		}
	}
	return result;
}

#include "flexible_surface_calls.h"
struct s_sort_record;
typedef bool (__stdcall *t_record_fill_2be)(long, void *, long, long, long, void *, s_sort_record *);
void function_41490(short group, long tag, short kind, real distance, t_record_fill_2be fill,
    dword value, void (__stdcall *callback)(void *), void *context, point3f const *position);
void __stdcall function_2be5d0(long, long, long, long, long, long, void *);

// @retail 0x2be650
void __stdcall function_2be650(void *submission)
{
    function_40f60(submission, function_d4bc0, function_2be5d0);
}

// @retail 0x2be670
void function_2be670(s_polygon_2be *hill)
{
    byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
    byte *data = *(byte **)(definition + 0xc);
    byte *item = *(byte **)(data + 0x534);
    function_41490(0, *(long *)(item + 0xc4), NONE, 640.f,
        (t_record_fill_2be)function_d4bc0, (dword)function_2be5d0,
        function_2be650, (void *)NONE, (point3f *)&hill->center_x);
}

bool function_15b2f0();
bool function_15b7c0(long delta, long player_index);
void function_1972a0(long player_index, long value14, long value10);
long game_engine_get_player_score(dword player_index);
void player_announce_points_to_win(long player_index, long score);

// @retail 0x2beee0
void function_2beee0()
{
    long players = 0;
    long teams = 0;
    s_player_iterator_2be iterator;
    iterator.data = g_4e8c24;
    iterator.absolute_index = NONE;
    iterator.index = NONE;
    while (player_iterator_next_with_unit((long *)&iterator))
    {
        if (iterator.player->time_inside)
        {
            players |= 1 << (byte)iterator.index;
            if (game_is_team_game_2be() && iterator.player->team != NONE)
                teams |= 1 << (byte)iterator.player->team;
        }
    }
    if ((g_4e6948->flags22c & 1))
    {
        if (game_is_team_game_2be())
        {
            if (!teams || ((teams - 1) & teams))
                return;
        }
        else if (!players || ((players - 1) & players))
            return;
    }
    if (!function_15b2f0())
        return;
    if (game_is_team_game_2be())
    {
        for (long i = 0; i < 8; ++i)
        {
            if (teams & (1 << i))
                ++((long *)((byte *)g_51ecc8 + 0x180))[i];
            else
                ((long *)((byte *)g_51ecc8 + 0x180))[i] = 0;
        }
        for (long team = 0; team < 8; ++team)
        {
            long last = NONE;
            if ((teams & (1 << team)) &&
                ((long *)((byte *)g_51ecc8 + 0x180))[team] % g_510c54->field_2_3 == 0)
            {
                s_player_iterator_2be members;
                members.data = g_4e8c24;
                members.absolute_index = NONE;
                members.index = NONE;
                while (player_iterator_next_with_unit((long *)&members))
                {
                    if (members.player->team == team && members.player->time_inside)
                    {
                        bool award = true;
                        if (!(g_4e6948->flags22c & 2))
                        {
                            iterator.data = g_4e8c24;
                            iterator.absolute_index = NONE;
                            iterator.index = NONE;
                            while (player_iterator_next_with_unit((long *)&iterator))
                            {
                                if (iterator.index != members.index && iterator.player->team == members.player->team &&
                                    iterator.player->time_inside &&
                                    (iterator.player->time_inside > members.player->time_inside ||
                                    (iterator.player->time_inside == members.player->time_inside &&
                                     (members.index & 0xff) < (iterator.index & 0xff))))
                                    award = false;
                            }
                        }
                        if (award)
                        {
                            long score = game_engine_get_player_score(members.index);
                            function_15b7c0(1, members.index);
                            long updated = game_engine_get_player_score(members.index);
                            last = members.index;
                            if (score != updated)
                                player_announce_points_to_win(members.index, updated);
                        }
                    }
                }
                if (last != NONE)
                    player_get_2be(last)->time_inside = 1;
            }
        }
    }
    else
    {
        s_player_iterator_2be members;
        members.data = g_4e8c24;
        members.absolute_index = NONE;
        members.index = NONE;
        while (player_iterator_next_with_unit((long *)&members))
        {
            if (members.player->time_inside && members.player->time_inside % g_510c54->field_2_3 == 0)
            {
                long score = game_engine_get_player_score(members.index);
                function_1972a0(members.index, 4, NONE);
                function_15b7c0(1, members.index);
                long updated = game_engine_get_player_score(members.index);
                if (score != updated)
                    player_announce_points_to_win(members.index, updated);
            }
        }
    }
}

void function_19f680(long tag, long group, long pass, long variant, void *context,
    point2f const *vertices, long count, point3f const *center, real radius,
    real perimeter, point3f const *color, real height);

// @retail 0x2be5d0
void __stdcall function_2be5d0(long arg_0, long arg_1, long arg_2, long arg_3,
    long arg_4, long arg_5, void *arg_6)
{
    s_polygon_2be *hill = (s_polygon_2be *)g_51ecc8;
    if (hill->count >= 4 && !(hill->count & 1))
    {
        long player = NONE;
        s_color_bits color;
        if (g_4b9ed8 != NONE)
            player = g_4e8c20->entries[g_4b9ed8];
        hill_color(player, &color);
        function_19f680(arg_0, arg_2, arg_4, arg_3, arg_6, hill->vertices, 32,
            (point3f *)&hill->center_x, hill->radius, hill->perimeter, (point3f *)&color, 0.8f);
    }
}
