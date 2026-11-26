// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FA590.CPP: lookups in the structure's 2d trees and surfaces, and
   the surface a contact lies on */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1fa590.h"
#include <math.h>
#include "slot_handler.h"
#include "normalize3d.h"
#include "vector_products.h"

void rotate_direction(vector3f *vector, vector3f const *axis, real sine, real cosine); /* src/unknown_1eb8a0.cpp */
void function_11d180(vector3f const *in, vector3f const *up, vector3f *forward, vector3f *left, vector3f *out);

struct s_motion_adjustment
{
	byte unknown00[0x20];
	real scale;
	byte unknown24[0xf4 - 0x24];
	vector3f forward;
	byte unknown100[0x124 - 0x100];
	vector3f current;
	byte unknown130[8];
	vector3f target;
	real rate;
};

struct s_motion_adjustment_result
{
	byte unknown00[0xc];
	vector3f velocity;
};

// @retail 0x1fa810
void __stdcall function_1fa810(long *ticks, s_motion_adjustment_result *out, s_motion_adjustment const *state)
{
	vector3f left;
	vector3f up;
	vector3f forward;
	function_11d180(&state->forward, g_4687b0, &forward, &left, &up);
	vector3f target;
	vector3f_scale(&state->target, state->scale, &target);
	vector3f desired;
	vector3f_scale(&state->forward, target.i, &desired);
	desired.i += target.j * left.i;
	desired.j += target.j * left.j;
	desired.k += target.j * left.k;
	desired.i += target.k * up.i;
	desired.j += target.k * up.j;
	desired.k += target.k * up.k;
	vector3f offset;
	offset.i = desired.i - state->current.i;
	offset.j = desired.j - state->current.j;
	offset.k = desired.k - state->current.k;
	vector3f normalized = offset;
	real length = normalize3d(&normalized);
	real step = g_510c54->rate * state->rate;
	vector3f motion;
	if (length > step)
		vector3f_scale(&normalized, step, &motion);
	else
		motion = offset;
	vector3f_add(&motion, &state->current, &out->velocity);
	*ticks = *ticks - 1 < 0 ? 0 : *ticks - 1;
}

/* a node of a 2d tree: a line and the two children (negative: a leaf) */
struct s_tree2d_node
{
	real i;
	real j;
	real d;
	long front;
	long back;
};

struct s_tree2d
{
	byte unknown00[0x10];
	long leaf_count;
	long *leaves;
	long node_count;
	s_tree2d_node *nodes;
};

/* the leaf of the tree the point is in, or NONE */
// @retail 0x1fa590
long function_1fa590(long index, point2f const *point, s_tree2d const *tree)
{
	while (index != NONE)
	{
		s_tree2d_node *node;

		if (!(index & 0x80000000))
			return index;

		index &= 0x7fffffff;
		if (index < 0 || index >= tree->node_count)
			break;

		node = &tree->nodes[index];
		if (node->i * point->x + node->j * point->y - node->d > 0.0)
			index = node->front;
		else
			index = node->back;
	}

	return NONE;
}

// @retail 0x1fa5e0
long *function_1fa5e0(s_tree2d const *tree, long index, long unknown)
{
	long *result = 0;

	if (unknown == NONE && index >= 0 && index < tree->leaf_count)
		result = &tree->leaves[index];
	return result;
}

/* the unit vector of the first axis direction set in the flags */
// @retail 0x1fa600
void function_1fa600(vector3f *vector, word flags)
{
	if (flags & 0x10)
	{
		vector->i = 0.0f;
		vector->j = 0.0f;
		vector->k = 1.0f;
	}
	else if (flags & 0x20)
	{
		vector->i = 0.0f;
		vector->j = 0.0f;
		vector->k = -1.0f;
	}
	else if (flags & 0x40)
	{
		vector->i = 0.0f;
		vector->j = 1.0f;
		vector->k = 0.0f;
	}
	else if (flags & 0x80)
	{
		vector->i = 0.0f;
		vector->j = -1.0f;
		vector->k = 0.0f;
	}
	else if (flags & 0x100)
	{
		vector->i = 1.0f;
		vector->j = 0.0f;
		vector->k = 0.0f;
	}
	else if (flags & 0x200)
	{
		vector->i = -1.0f;
		vector->j = 0.0f;
		vector->k = 0.0f;
	}
}

struct s_surface_flags
{
	byte unknown0[4];
	byte flags;
	byte bit;
	byte unknown6[2];
};

struct s_structure_view
{
	byte unknown00[0x2c];
	s_surface_flags *flags;
};

struct s_4e0348_view
{
	byte unknown00[0x18];
	s_structure_view *structure;
	byte unknown1c[0xc4 - 0x1c];
	long count;
	long first;
};

byte *function_183fc0(long index);

// @retail 0x1fa6b0
bool function_1fa6b0(s_pathfinding_data const *pathfinding, s_pathfinding_node const *node)
{
	bool result = false;
	long index;

	for (index = node->first_surface; index != NONE; index = pathfinding->surfaces[index].next)
	{
		s_pathfinding_surface *record = &pathfinding->surfaces[index];

		if (record->type == 7)
		{
			s_surface_flags *flags = &((s_4e0348_view *)g_4e0348)->structure->flags[record->index];

			if (flags->flags & 8)
			{
				dword bit = flags->bit;
				dword *bits = (dword *)function_183fc0(record->object_index);

				result = (bits[bit >> 5] & (1 << (bit & 31))) == 0;
			}
			break;
		}
	}

	return result;
}

// @retail 0x1fa720
s_pathfinding_edge *function_1fa720(s_edge_iterator *iterator)
{
	s_pathfinding_edge *edge = 0;

	while (iterator->surface_index != NONE)
	{
		s_pathfinding_surface *surface;

		iterator->previous_surface_index = iterator->surface_index;
		iterator->edge_index = (word)NONE;
		surface = &iterator->pathfinding->surfaces[iterator->previous_surface_index];
		iterator->surface_index = surface->next;
		if (surface->type == 0)
		{
			iterator->edge_index = (word)surface->index;
			edge = &iterator->pathfinding->edges[iterator->edge_index];
			iterator->forward = edge->surface == iterator->surface;
			goto found;
		}
	}

	if (iterator->next_edge_index != (word)NONE)
	{
		iterator->edge_index = iterator->next_edge_index;
		iterator->previous_surface_index = NONE;
		edge = &iterator->pathfinding->edges[iterator->edge_index];
		iterator->forward = edge->surface == iterator->surface;
		iterator->next_edge_index = edge->next_edges[iterator->forward];
		if (iterator->next_edge_index == iterator->first_edge_index)
			iterator->next_edge_index = (word)NONE;
found:
		if (iterator->forward)
			iterator->vertex = edge->vertices[0];
		else
			iterator->vertex = edge->vertices[1];
	}

	if (++iterator->count > 2000)
		edge = 0;
	return edge;
}

// @retail 0x1fa7f0
long function_1fa7f0(void)
{
	s_4e0348_view *globals = (s_4e0348_view *)g_4e0348;
	long result = 0;

	if (globals->count > 0)
		result = globals->first;
	return result;
}

/* the contact at a surface and its orientation (2..5 by the larger of i and j) */
struct s_contact
{
	byte unknown00[0xdc];
	vector3f normal;
};

struct s_contact_result
{
	long unknown0;
	long orientation;
};

// @retail 0x1faeb0
void function_1faeb0(s_contact const *contact, s_contact_result *result)
{
	result->orientation = 1;
	if (fabs(contact->normal.i) < fabs(contact->normal.j))
	{
		if (contact->normal.j < -0.0001f)
			result->orientation = 2;
		else if (contact->normal.j > 0.0001f)
			result->orientation = 3;
	}
	else
	{
		if (contact->normal.i < -0.0001f)
			result->orientation = 5;
		else if (contact->normal.i > 0.0001f)
			result->orientation = 4;
	}
}

/* a timer of 1.5 seconds and a zero vector */
struct s_1faf30_timer
{
	long ticks;
	vector3f vector;
};

// @retail 0x1faf30
void function_1faf30(s_1faf30_timer *timer)
{
	real seconds = (real)g_510c54->field_2_3 * 1.5f;
	long ticks;

	__asm
	{
		fld seconds
		fistp ticks
	}
	timer->ticks = ticks;
	timer->vector = *g_4687a4;
}

struct s_contact_surface_range
{
    long key;
    long first;
    long last;
    long unknown0c;
};

struct s_contact_surface_owner
{
    long unknown00;
    long first;
    long last;
    long range_count;
    s_contact_surface_range *ranges;
    byte unknown14[8];
};

struct s_contact_surface_table
{
    long count;
    byte unknown04[0x30 - 4];
    long owner_count;
    s_contact_surface_owner *owners;
    byte unknown38[8];
    long mapping_count;
    short (*mapping)[2];
};

struct s_bsp3d;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, long plane_index, plane3f *plane);
short function_120850(vector3f const *v);
real *function_120810(real const *point, short axis, byte side, real *out);
short function_296230(long object_index);

// Matched in a full build with stand-in SDK headers (was 63): the two file-local callees take their parameters in
// the order (tree, index, unknown) and (index, point, tree), which gives the caller's register-argument moves
// retail's order; the range test of case 3 is split into its own arms; the leaf value is read through a
// reference-bound local.
// Before:
// score 63 (was 79): the plane lookup is the argument of the axis call, so the plane and the
// projected point no longer share a stack slot and the frame is retail's 0x18. Left: retail has
// the result in edi and the structure index in ebx (here reversed), and passes the projected
// point's address with a fresh lea where ours forwards function_120810's return value.
// @retail 0x1fa3a0
long function_1fa3a0(long structure_index, long surface_index, long object_index,
    long node_index, point3f const *point)
{
    long const *node_reference = &node_index;
    long result = NONE;
    long type = object_index != NONE ? 4 : (structure_index != NONE ? 3 : 1);
    byte *globals = (byte *)g_4e0348;
    long table_count = *(long *)(globals + 0xc4);
    s_contact_surface_table *table = NULL;
    if (table_count > 0)
        table = *(s_contact_surface_table **)(globals + 0xc8);
    if (table)
    {
        switch (type)
        {
        case 4:
            {
                short owner_index = function_296230(object_index);
                if (owner_index >= 0 && owner_index < table->owner_count)
                {
                    s_contact_surface_owner *owner = &table->owners[owner_index];
                    for (short i = 0; i < owner->range_count; i++)
                    {
                        s_contact_surface_range *range = &owner->ranges[i];
                        if (range->key == *node_reference)
                        {
                            result = range->first;
                            if (result == NONE || surface_index < 0 || surface_index > range->last - result)
                                result = NONE;
                            else
                            {
                                result += surface_index;
                                if (result < 0 || result >= table->count)
                                    result = NONE;
                            }
                            break;
                        }
                    }
                }
            }
            break;
        case 3:
            if (structure_index >= 0 && structure_index < table->mapping_count)
            {
                short owner_index = table->mapping[structure_index][0];
                if (owner_index >= 0 && owner_index < table->owner_count)
                {
                    s_contact_surface_owner *owner = &table->owners[owner_index];
                    long first = owner->first;
                    if (first == NONE)
                        result = NONE;
                    else if (surface_index < 0 || surface_index > owner->last - first)
                        result = NONE;
                    else
                    {
                        result = first + surface_index;
                        if (result < 0 || result >= table->count)
                            result = NONE;
                    }
                }
            }
            break;
        case 1:
            {
                s_slot_entry_list *bsp = g_4e0340;
                if (bsp && surface_index != NONE)
                {
                    long *leaf = function_1fa5e0((s_tree2d *)table, surface_index, object_index);
                    if (leaf)
                    {
                        byte *surfaces = *(byte **)((byte *)bsp + 0x2c);
                        plane3f plane;
                        short axis = function_120850((vector3f *)bsp3d_get_plane((s_bsp3d *)bsp,
                            *(short *)(surfaces + surface_index * 8), &plane));
                        byte side = ((real *)&plane)[axis] > 0.0f;
                        point2f projected;
                        function_120810((real const *)point, axis, side, (real *)&projected);
                        long value = *leaf;
                        long const &kept = value;
                        result = function_1fa590(kept, &projected, (s_tree2d *)table);
                    }
                }
            }
            break;
        }
    }
    return result;
}

real g_557c70;
real function_30bf0(vector3f *v);
void function_c7840(transform4x3f const *frame, vector3f *current, vector3f const *desired, real rate,
    vector3f *velocity, real const *limits, real max_speed, real acceleration);

/* facing comes second: both retail callers (0x1197b0, 0xe01d0) form the facing address only after the settings and
   control addresses, the order a right-to-left evaluation of (object_index, facing, settings, control, ...) gives.
   With facing that early its i component outranks the test's forward.i read unless two named values come ahead of
   that read (speed_squared, limit_squared) and the cross product reads forward.i through the forward pointer, with
   the up pointer the older symbol; the operand orders below are retail's only in that arrangement */
// @retail 0x1faf80
bool function_1faf80(long object_index, vector3f *facing, void const *settings_pointer, vector3f const *control,
    real threshold, real *turn)
{
    s_slot_object_view *object = object_get(object_index);
    real const *settings = (real const *)settings_pointer;
    vector3f desired;
    real speed_squared = object->velocity.i * object->velocity.i + object->velocity.j * object->velocity.j +
        object->velocity.k * object->velocity.k;
    real limit_squared = g_557c70 * g_557c70;
    if (speed_squared < 0.25f &&
        ((vector3f *)object->unknown094)->i * ((vector3f *)object->unknown094)->i + ((vector3f *)object->unknown094)->j * ((vector3f *)object->unknown094)->j + ((vector3f *)object->unknown094)->k * ((vector3f *)object->unknown094)->k < limit_squared &&
        control->i * control->i + control->j * control->j + control->k * control->k < 0.0001f &&
        object->forward.k * facing->k + object->forward.j * facing->j + object->forward.i * facing->i > threshold)
        desired = object->forward;
    else
    {
        real vertical = settings[3] * control->k;
        desired = *facing;
        if (vertical != 0.0f)
        {
            desired.k += vertical;
            if (function_30bf0(&desired) == 0.0f)
                desired = *facing;
        }
    }
    vector3f const *up = (vector3f const *)((byte *)object + 0x7c);
    vector3f *forward = &object->forward;
    vector3f left;
    left.i = forward->j * up->k - *(real const *)((byte *)object + 0x80) * forward->k;
    left.j = up->i * forward->k - up->k * forward->i;
    left.k = up->j * forward->i - up->i * forward->j;
    real value = (facing->k * left.k + facing->j * left.j + facing->i * left.i) * 3.3333332538604736f;
    value *= control->i;
    value -= control->j;
    if (value > 1.5f)
        value = 1.5f;
    real target = settings[0] * value;
    real fraction;
    if (*turn * target <= 0.0f)
        fraction = 1.0f;
    else if ((target >= 0.0f ? target : -target) > 0.001f)
    {
        real ratio = *turn / target;
        fraction = 1.0f - (ratio > 1.0f ? 1.0f : ratio);
    }
    else
        fraction = 0.0f;
    real duration = ((1.0f - fraction) * settings[2] + settings[1] * fraction) * (real)g_510c54->field_2_3;
    if (duration > 0.0f)
        target = (target - *turn) / duration + *turn;
    *turn = target;
    real limits[4] = { -3.1415927410125732f, 3.1415927410125732f, -1.5707963705062866f, 1.5707963705062866f };
    if (settings[9] == 0.0f)
        *forward = desired;
    else
        function_c7840(NULL, forward, &desired, g_510c54->rate, (vector3f *)object->unknown094, limits, settings[8], settings[9]);
    return true;
}

// MiniMax S (11-06): no new form, no lower score: the idle arm still pairs and adds its products in a different order from retail's, and the sine, the cosine and the axis keep different homes; the body is still 15 bytes longer than retail.
// Statement-by-statement walk against retail (b6E, 11-06; every score here is a fast check, 221 as committed, the
// full build 214): no logic or structure difference found. Active arm (0x1faa38-0x1fac9a): up and forward copies from
// state+0x100 and +0xf4, c7840 call with {-pi, pi, -pi, pi} and settings+0x88/+0x8c, angle stored as a float at
// 0x1faae7 and reloaded for both fcos (0x1faaf7) and fsin (0x1fab4e), cos and sin rounded to float (fstp at
// 0x1fab40 and 0x1fab6d), dot summed j, k, i, then Rodrigues terms (a + b) - c, projection summed k, j, i, the
// zero-length reset at 0x1fac5d and the final angle == 0 clear at 0x1fac87 all agree in order and direction.
// Idle arm (0x1facad-0x1faea6): both crosses, the zero reset to g_4687a8/g_4687ac, the two reloads of state+0x28
// (0x1fad9c, 0x1fadaf), base scaled by cos, axis scaled after the trig, up = axis * sin + base, projection and
// the three normalisations agree too. Remaining difference is scheduling only (retail loads axis.j and axis.k
// before fcos and reloads the angle for fsin; ours dups it with `fld st(0)`). Also tried: cross3f before the
// trig 235 (1161 bytes, the retail size), cross3f between sin and dot 223, cross3f after cos+dot and before sin 221.
// score 132 (was 214): the x87 trig sequence, the angle/trig homes and the active rotation are retail's; left: the active
// arm's sine store and one product two slots late, the idle first cross loading g before forward->i in two products, and
// the idle second cross and final projection loading forward first where retail copies the register operand (`movaps reg;
// mulss mem`), 12 bytes in all. Earlier at 214: frame 0x28 agrees; angle/trig homes, the x87 angle copy (ours `fld st(0)`,
// retail reloads the angle from its home for fsin in both arms) and projection/product scheduling remain. Earlier, 214 from 251:
// trig homes and projection registers differ;
// shared trig scope 254; sine declared first 251; forward projection loop 235;
// rotation loop 246; idle base loop 235; projection plus base loops 235; all loops 246.
// pointer scale loop 239; fixed do scale 237; idle reconstruction loop 235;
// two-component idle projection loop 235; native bounds pair 235.
// global optimization off 721; size optimization 409; inline depth zero 362;
// active rotation scope 235; idle sine after scaling 235.
// active/idle/both real trig 233/235/233; shared weighted axis 214 plain kept;
// weighted cross 218; both weights 258; output cosine scale 235; real trig plus axis 226.
// Earlier at 214: frame 0x28 agrees; angle/trig homes and projection scheduling remain. Active angle
// name 214; external up reference 212; forward reference 216; both output references
// 178 (held: reference aliases of the two parameters); external velocity reference 214.
// Round 67 (m3 lane 13, plain; fast is unreliable here): the existing rotate_direction (forced inline in
// src/unknown_1eb8a0.cpp) for the hand rotation with `(real)sin(angle), (real)cos(angle)` 249 (1182 bytes, aligned
// 178 -> 154), with named cosine and sine 250 (1182, aligned 137); the existing rotate_vector_170d70 250 (same code);
// sine after the dot 214 (same code); the idle angle field read at each trig call 214 (loads merged). Kept: the
// shared cross3f in place of a file-local copy of it, 214 (same code). The active arm is 4 bytes and the idle arm
// 11 bytes longer than retail's; the idle crosses differ in product operand order (`movss mem; mulss reg` against
// retail's `movaps reg; mulss mem`).
// Round 228 (m3 lane 6, fast with 0xc7840 listed, confirmed plain): kept 132 = the idle trig reading the angle field at
// each call (both arms then reload for fsin as retail) with the base scaled by the existing vector3f_scale, the existing
// rotate_direction for the active rotation, both projections as vector3f_scale(.., dot3f(up, forward), &offset) then three
// subtractions (every component read before the first store, as retail), and cosf/sinf for the four trig calls.
// Steps: cosf/sinf alone 226 (1161 bytes); the field read at each call alone 214, with the loop 214, the sine after the
// scaling 214; the cosine inside the base loop 236 (the active arm's x87 becomes retail's); three scaling statements 251,
// with the field read 252; rotate_direction on that 248 (sin/cos arguments 247); the offset projections 193 (active only
// 204, idle only 245); active cosf/sinf 186 (either alone 230, all four 188); vector3f_scale for the base 130 (plain
// cos/sin in the active arm then 202; all four cosf/sinf 132, kept for one spelling). Same code: dot term orders,
// cross3f_staged or a plain-order cross for either idle cross, dot3f either way round, base/axis declaration order,
// renamed cosine, the projection named or passed straight in. Worse: vector3f_add_scaled for the idle up rows 142-163;
// sin/cos as rotate_direction arguments 141; main's idle projection statements 186; a normalize call between the idle
// crosses 163 (diagnostic: the second cross still loads forward first). Dead-local phase scans 0-7: no effect.
// Permuter (900 s, 94 tries): best 103 by its own count only through a swapped cross3f argument order (a sign change,
// not usable) plus constant and volatile temporaries.
// M3 lane 15 r135 (the coordinator's rotation-family screen, h2m fast): the rotation through include/axis_rotation.h's helper in place of rotate_direction 214 (1175 B). Restored.
// M3 lane 16 r181 (h2m fast with 0xc7840, 0x30bf0 and 0x1eb8a0 listed, which reproduces 132; without 0x1eb8a0 the
// rotation is a call, 962 bytes): aligned, 36 instructions differ. Retail's commutative operands put forward->i first
// against every leaf (both dots' i products, the scale's i) and forward->j/k last (the second cross, the scale's j/k);
// ours puts all of forward first in the idle arm and up->i first in both dots. The matched 0x109660 (scale plus a
// subtract helper) gets retail's forward->i-first dot, so the keys are context. Redundant paren pairs anywhere
// give two states only (count 0 mod 4 = this code; 1, 2, 3 or 5 pairs = one other listing, 130): the first cross's
// i row becomes retail's but its j/k second products flip. Parameter renames 132; casts on either cross or the
// projection inputs 132; a pointer local for forward in the idle arm at any one use 130 (the paren state); the
// sine declared first 143; the rate first in the angle product 132; the angle call result in parens 170 (1164
// bytes, the rotation reschedules); double sin/cos in the active arm 204, in the idle arm 130, both 202.
// M3 lane 8 r230 (h2m fast with 0xc7840, 0x30bf0 and 0x1eb8a0 listed, base 132): both projections' subtractions through
// a file-local a - b helper (0x109660's scale-then-subtract shape) 188; the global up pointer named for the first idle
// cross 132; both idle cross outputs declared ahead of the first cross 132 (same code each); the idle axis normalized
// ahead of the base scaling 127 (1149 bytes: the call moves ahead of the scaling, retail calls it after; not kept);
// the active offset declared ahead of the angle 132 (same code). Left as before: the active sine store and the dot's
// i term (retail forward->i first), the idle first cross's i row (retail loads g first) and its j/k rows.
// M3 lane 1 r164 (h2m fast with 0xc7840, 0x30bf0 and 0x1eb8a0, base 132): the 12 extra bytes are the idle arm's products
// that load the forward component and multiply by the register value (`movss reg, mem; mulss reg, reg`, 9 bytes) where
// retail copies the register value and multiplies by memory (8 bytes). Forms: the idle base's zero test as
// `0.0f == function_30bf0(&base)` 131; the angle test as `0.0f == angle` 82 (1169 B; not kept: the compare then folds
// the angle's load, `ucomiss xmm0, [mem]`, unlike retail's `movss xmm1, [mem]; ucomiss xmm1, xmm0`, and the gain is only
// the idle arm realigning behind 4 fewer bytes); the active zero test as `0.0f == function_30bf0(forward)` 133; the
// tick test as `0 < *ticks` 132 (same code); the idle sine computed ahead of the cosine 132 (same code).
// M3 lane 7 r227 (fast with 0xc7840, 0x30bf0 and 0x1eb8a0, base 132; caller 0x1e5af0 listed). Reading: retail passes
// state in EAX, up in ECX and forward in EBX (0x1e5af0 passes its own register arguments straight through, so its
// call shows no setup order). A component read at offset 0 through a pointer parameter ranks by the parameter's
// number from the end of the list, so forward.i ranks below up.i here (2 against 4), where retail's active dot needs
// it above; the idle crosses also need the global's j/k loads ranked above forward's j/k, which follow the address
// temporaries' numbering, ours the other way round. Parameter orders, definition + declaration + call together: forward,
// up, state, ticks 130 (0x1e5af0 MATCH); forward, up, ticks, state 130 (MATCH); state, forward, up, ticks 296 (0x1e5af0
// 47); forward, state, up, ticks 295 (47); state, ticks, forward, up 296 (47). On the first: the first idle cross through
// cross3f_staged 130, the second 130, both 146. Not kept (an order picked by score without a match).
// M3 lane 1 r170 (h2m fast with 0xc7840, 0x30bf0, 0x1eb8a0 and 0x1e5af0, base 132; aligned counts by a masked line
// diff). The active arm's angle expression is the lever: each extra node on it (a redundant paren pair, a cast of the
// product, the length in a named local, or `angle *= rate` as a second statement) moves the whole function, and the
// effect adds up: one 170 (1164 B), two 206 (1175 B), three 16 at retail's 1161 B (aligned 16: left only the active
// sine store two instructions late and the offset-0 loads of both dots and the first idle cross, where retail takes
// forward's component first). The same three nodes anywhere else (the active dot, the active zero test, the idle
// cosine) do nothing; 0-7 unused initialised locals change nothing at three nodes. Forms: the length named 170, the
// product cast 170, `angle *= rate` 170, `angle = angle * rate` 170, the named length with the product cast 206, the
// rate cast 132 (no node), the rate in a named local for the 0xc7840 call and the angle 290 (1181 B), the angle test
// as `!angle` 130. LEAD (steering, match-only): three redundant paren pairs on the angle 16, or the named length with
// two pairs 16; with the parameters ordered forward, up, state, ticks (definition, declaration and 0x1e5af0's call
// together; 0x1e5af0 MATCH) 12 (only the sine store and the first idle cross's i products left); forward, up, ticks,
// state 12; the other 20 orders 16..284 (state or ticks first breaks 0x1e5af0, 47).
// @retail 0x1faa20
void function_1faa20(vector3f *up, byte const *state, vector3f *forward, long *ticks)
{
    if (*ticks > 0)
    {
        *up = *(vector3f const *)(state + 0x100);
        *forward = *(vector3f const *)(state + 0xf4);
        byte *settings = *(byte **)(state + 8);
        real limits[4] = { -3.1415927410125732f, 3.1415927410125732f, -3.1415927410125732f, 3.1415927410125732f };
        vector3f *velocity = (vector3f *)(ticks + 1);
        function_c7840(NULL, up, g_4687b0, g_510c54->rate, velocity, limits,
            *(real *)(settings + 0x88), *(real *)(settings + 0x8c));
        vector3f axis = *velocity;
        real angle = function_30bf0(&axis) * g_510c54->rate;
        real cosine = cosf(angle);
        real sine = sinf(angle);
        rotate_direction(up, &axis, sine, cosine);
        vector3f offset;
        vector3f_scale(up, dot3f(up, forward), &offset);
        forward->i -= offset.i;
        forward->j -= offset.j;
        forward->k -= offset.k;
        if (function_30bf0(forward) == 0.0f)
        {
            *up = *g_4687b0;
            *forward = *g_4687a8;
        }
        if (angle == 0.0f)
            *ticks = 0;
    }
    else
    {
        vector3f axis;
        cross3f(forward, g_4687b0, &axis);
        vector3f base;
        cross3f(&axis, forward, &base);
        if (function_30bf0(&base) == 0.0f)
        {
            base = *g_4687a8;
            axis = *g_4687ac;
        }
        real cosine = cosf(*(real const *)(state + 0x28));
        real sine = sinf(*(real const *)(state + 0x28));
        vector3f_scale(&base, cosine, &base);
        function_30bf0(&axis);
        up->i = axis.i * sine + base.i;
        up->j = axis.j * sine + base.j;
        up->k = axis.k * sine + base.k;
        function_30bf0(forward);
        vector3f offset;
        vector3f_scale(forward, dot3f(up, forward), &offset);
        up->i -= offset.i;
        up->j -= offset.j;
        up->k -= offset.k;
        function_30bf0(up);
    }
}
