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
	long result = NONE;
	long location_index = NONE;
	s_type_c3b527 location;
	vector3f up, forward;
	function_26bfa0(object_index, &location_index, (s_location_view *)&location);
	if (location_index != NONE)
	{
		result = record_pool_allocate(g_51eca4);
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
	}
	return result;
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
			real x, y, z;
			switch (mode)
			{
			case 1:
				x = direction.i * 0.25f + motion->position.x;
				y = direction.j * 0.25f + motion->position.y;
				z = direction.k * 0.25f + motion->position.z;
				break;
			case 2:
				x = motion->position.x - direction.i * 0.25f;
				y = motion->position.y - direction.j * 0.25f;
				z = motion->position.z - direction.k * 0.25f;
				break;
			case 3:
				point = motion->position;
				x = point.x;
				y = point.y;
				z = point.z;
				break;
			default:
				mode = NONE;
				break;
			}
			if (mode != NONE)
			{
				vector3f side = motion->side;
				point.x = side.i * 0.15f + x;
				point.y = side.j * 0.15f + y;
				point.z = z;
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
					else
						goto failed;
				}
				else
					goto failed;
			}
		}
	}
failed:
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
	short sector;
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
			if (entry->sector == NONE)
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
long function_26d100(vector3f const *up, point3f const *point, long *location, s_collision_result_1697c0 *collision);

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
	real along = dot3f(axis, vector) * (1.0f - cosine);
	real i = vector->i, j = vector->j, k = vector->k;
	vector->i = axis->i * along + i * cosine - (axis->k * j - axis->j * k) * sine;
	vector->j = axis->j * along + j * cosine - (axis->i * k - axis->k * i) * sine;
	vector->k = axis->k * along + k * cosine - (axis->j * i - axis->i * j) * sine;
}

// score 90 (was 90): Frame 0xd8 agrees; retail converts the ring in SSE then reloads
// it for the x87 radius product, while ours uses fild. Ring/spoke homes and the
// rotation's component spill, cross scheduling and register allocation remain.
// The body is 1085 bytes versus retail 1089.
// MiniMax G (11-14): no new form, no lower score (90, was 90): naming the ring as a float and hoisting its phase above the distance reproduces retail's cvtsi2ss, the slot store and the x87 reload, but the frame grows to 0xdc and the head's radius load and sector store move (143); the same hoist with the ring conversion written out (143), the phase product as a double (257), the sector read before the radius (109) and the radius without its reference (236) all score worse, the head's radius still arrives through a general register where retail uses movss, and the permuter found nothing below 87 in 25 tries.
// MiniMax H (11-12): no new form, no lower score (90): the ring is still converted with fild where retail
// converts it to float, keeps it in a stack slot and reloads it as a double for the radius product; the named
// ring float (119), the named ring phase (143) and the named double radius of the earlier rounds all score
// worse, and the rotation block still spills a component where retail keeps the three in registers.
// MiniMax H (11-11): the loop-invariant radius times 1.2 as a named double with the ring as a named float reaches 118
// in the reduced image but 146 in the full build, where the body returns to retail's own prologue, so it is not kept;
// the ring float shared with the angle term 119, the ring phase named in the outer loop 143 (the frame grows to 0xdc)
// and the cross-file rotation helper (the fast build cannot link it) all fail. The rotation's dot with the three
// products named 90 (same code: the compiler orders the terms itself).
// M3 lane 1 r96 (fast, the file's other 13 functions and 0x1ecf50 matched each time): the keepable spelling of the listed
// rotate_direction lead (the existing rotate_direction, the ring as one named float for the distance and the angle, the
// trace distance read at each use) 108 (1080 B, frame 0xd4: the ring stays an x87 fild without the one-use phase); the same
// without the ring float 108 (same code); sine and cosine through sinf/cosf 215 (1092 B); the direction declared once
// outside the loops 90 (same code); the spoke counter declared ahead of the distance 90 (same code).
// MiniMax I (11-10): no new form, no lower score (90).
// MiniMax D (11-09): no new form, no lower score; retail keeps the ring value as a float in a
// stack slot and reloads it as a double for the radius product where ours converts the counter
// directly, and retail's two global loads use swapped registers.
// MiniMax D (11-09) second pass: naming the ring as a float and computing the phase before the
// distance reproduces retail's cvtsi2ss, store, fld, mulss, fmul sequence, but the extra stack
// slot makes the frame 0xdc and copies the radius through an integer register, so it scores 234
// against the 90 body, which is kept.
// MiniMax G (11-04): ring block with a hoisted double scaled radius, the ring converted once
// into a named local and its offset named, plus the rotation with cross j, k named and a
// second name for axis (local_axis) mixed into the dot and rows: 76 (retail's cvtsi2ss ring
// conversion, SSE offset and an unspilled rotation). The second name is a register steer,
// kept only with a match; the same form without it 96. Not kept.
// Lane 15 r17: collision marker store before the entry pointer 90 (it then precedes `lea esi`
// as retail, but the point stores still come first); named ring phase 143 (ring conversion as
// retail: cvtsi2ss, store, fld; frame 0xdc, radius copied through integer registers); the
// same with a plain radius local 236; distance computed in the spoke loop at its use 243 (ring
// times radius moves to SSE); rotation through the existing cross3f of (vector, axis) 100.
// The phase's extra slot pairs with our rotation spill: retail keeps all three components in
// registers, so the ring conversion needs the rotation fixed first.
// Round 53: ring phase named in the outer loop before/after the distance 143/143 (SSE ring
// conversion as retail, but frame 0xdc and the radius copied through integer registers);
// all three cross terms named, j and k first 90 (forwarded back); a vector3f cross filled j, k, i
// 100, with the ring phase 149. Retail computes the j- and k-row cross subtrahends with the dot.
// Round 50: conditional sector store with the field read back as short (120 -> 109) and the
// sector index read after the radius (-> 90) kept. Rotation with cross j, k before i 100
// (no spill, frame 0xd4); with a named ring phase as well 96 (frame 0xd8, retail's SSE ring
// conversion and rotation core, but the radius copies through integer registers); statement
// orders of the cross/along locals 96-105; 0x2982f0/0x170d70/0x11d3b0 rotation forms 126-139;
// existing cross3f after/before the dot 134/255; distance inside the spoke loop 254; shared real
// ring 100; plain radius local 112; collision marker before the point 89; entry first 215.
// At 195: frame 0xdc versus 0xd8 and a rotation spill; shared cross
// 244; native component loop 244; cross before dot 257;
// signed-byte spoke 213; size optimization 395.
// rotation spill keeps frame 0xdc versus 0xd8; radius copy and sector-test registers differ.
// Current-allocation plain radius 209; scalar cross lifetimes 195; dot lifetime 195; unsigned-word spoke 195; depth one 195.
// One rotation spill still adds four frame bytes; axis component values 285;
// symmetric dot inputs 195; two/all cross subtrahends, minuends and projected-axis values 210;
// cosine-direction values 211; private rotation output last 195.
// At 184: frame 0xd8 now agrees; radius setup, rotation schedule and sector-test registers remain.
// Cosine first 197; shared direction/axis/both scales 210/213/261; cosine-first inputs 195;
// angle product lifetime 196; per-use ring phase 184 kept, 1087/1089 bytes; angle-result names 195.
// cosine complement 195; cosine first 197; output scalars 195; direct-input outputs 256;
// components before dot 195; shared rotation 195/1091; output scalars plus plain radius 209;
// shared rotation plus plain radius 208/1095. Rotation spill and frame 0xdc remain.
// At 163: Frame 0xd8 agrees; entry/ring conversion, rotation and sector registers
// remain. Shared real ring 182; ring plus phase 195; record-first 163 kept; with real ring 161
// rejected for two extra points; double base with ring/phase 182/195; axis value 317;
// record/ring/spoke values 163.
// At 125: entry-table/ring conversion and rotation allocation remain.
// Assigned sector word/short 161/161; default sector word 125 kept, short 128;
// missing-sector continue, sentinel-between-direction and sentinel-first 163 each;
// assigned short sector with interleaved publication 162.
// frame 0xd8 agrees; entry/ring conversion, rotation XMM and sector registers remain.
// Rotation output/axis/both object references 163/163/163; each with output last 163/163/163;
// cosine-first output/axis reference contracts 163/163. Original helper and body restored.
// At 125: entry-table/ring conversion and rotation allocation remain.
// Native direction value with output first/last, sine/cosine first 276/276/276/276;
// both vector values 328/328; rotated-vector return 281/281. Original rotation contract retained.
// At 120: entry-table load, ring conversion and rotation XMM allocation remain.
// Native radius member 125; shared ring with/without native radius 123/123; three-use trace distance 120 plain;
// shared trace displacement 225; elevation vector 245; collision point setter 125; collision-offset vector 189;
// trace distance with shared ring 120. Keep the smaller trace-distance form; frame 0xd8, 1090/1089 bytes.
// At 120: frame 0xd8 agrees; entry/ring conversion, rotation and sector publication differ.
// Named collision marker 120; marker before entry 120; entry before point 236;
// complete point components 144; promoted sector 120; conditional sector store 154;
// direct radius with trace scalar 239; sector eligibility result 120.
// Separated object before record/sector and early native radius 120/120/120;
// direction/world library copies 120/120; conditional sector/pathfinding 154/234; separate count advance 120.
// frame 0xd8 agrees; entry-table scheduling, ring conversion and rotation homes differ.
// Early object/record-pool producers 123/123 (exact-only); real ring/spoke 273/195;
// short remaining count 125; unsigned/double ring 248/261; real ring and spoke 201.
// Helper-owned angle/trigonometry 236; cosine first 237; early direction inputs 236;
// early projection or projection plus inputs 239; byte ring 161, remaining count 120,
// both byte states 161. Existing helper and all ring/trace inputs restored.
// Round 141: Native axis before/after projection 298/293; native direction before/after projection
// 253/259; both inputs before/after projection 306/312; axis with real ring 298; both inputs with
// double base distance 313. Original helper/body restored; SSE ring conversion and rotation spill
// remain.
// Round 141: Component, projection, combined input/projection, formal, complete rotation, radius,
// native-index and all-scalar lexical homes 90 each. All original names, input copies and statements
// restored.
// Sine-scaled cross 215; with projected-axis/cosine terms 215/215; all terms 90;
// projection plus cosine 215; projection/cross/cosine and cosine/cross/projection orders 90/90;
// all terms with cosine complement 90. Uniform rows; opening helper and body restored.
// Parallel displacement/elevation/both product parts 215/90/215; collision-offset parts 90;
// displacement with collision parts 215; elevation with collision parts 90; all parts 215;
// native point aggregate initializer 90. Original body and rotation helper retained.
// Round 242: fused add/scale at trace/elevation/collision 90/215/90; trace+elevation,
// trace+collision, elevation+collision, all three 215/90/215/215; all with ordinary radius 113.
// Original complete body and rotation helper restored; ring conversion and rotation spill remain.
// Round 244: existing rotation with distinct source/destination, output last/first,
// axis-source-output first and scalar inputs first, each with projection before/after original
// component snapshots, all 90. Original helper, in-place input copies and complete body restored.
// Round 247: native ring-distance product 90; with ring phase 142; double base 118; base/phase 142; shared ring product 90; shared ring/phase 142; shared ring/base 118; all three 144. SSE ring conversion and rotation spill remain.
// Round 250: existing staged-cross producer orders projection/components/cross, projection/cross/components,
// components/projection/cross, components/cross/projection, cross/projection/components and cross/components/projection
// 219/219/219/225/225/225; uniform projected-axis parts on first/last orders 102/237.
// Original helper, input snapshots and body restored; ring conversion and rotation spill remain.
// Round 253: Per-spoke query producer before direction/angle/sine/cosine, default-if versus
// conditional, 163/247/153/246/281/281/147/241. All exact-only; no match. Original query
// observation, rotation helper, input copies, inherited local qualifier and complete body restored.
// Round 255: unsigned-word ring with short/word/byte/char spokes 117/117/246/246; signed-byte ring with short/word/byte/char spokes 130/130/247/247. All restored; x87 ring conversion and rotation spill remain.
// Round 121: the missing frame slot is retail's SSE ring conversion temp (cvtsi2ss, a store, fld). The
// rotation helper in the form that took 0x29c860 to 2 (j and k cross terms in a vector first, the i
// cross term in the i row; in place) gives retail's rotation structure but 100 (frame 0xd4); with the
// ring phase named before the distance as well 96 (frame 0xd8, SSE ring conversion and rotation core as
// retail; the radius copy then moves to integer registers and the dot's sum order and integer register
// names differ); with 0x29c860's operand orders 100/96; on that: angle terms swapped, dot written j/i/k,
// phase after the distance 96 each; a named (real)ring 100/96 (with the phase); plain radius 222;
// sector index before the radius 263.
// Round 200: output-pointer rotation with four formal priorities 90/90/90/90;
// with native phase before distance 143/143/143/143 (frame 0xdc). Original void helper,
// input copies and complete body restored; output-result lifetime does not resolve the spill.
// Round 106: the existing 0x29c860 rotation helper (sin/cos of the angle inside, j/k cross terms in a vector,
// i in its row) in place 225 (frame 0xd4), on a copied source vector 124 (rotation core as retail up to register
// names, frame 0xe0); in place with the ring phase named before/after the distance 230/230 (SSE ring conversion,
// radius copied through integer registers); the ring loop as a plain `for` 266, with the named phase 262, with
// both 271 (1057-1066 bytes).
// Round 2 of lane 18, on the 96 form (j/k cross terms first, ring phase named before the distance;
// there only the direction components' registers, the radius copy and the sector load differ):
// plain radius 222 (repeats the listed form on this base); radius as a reference into the object 264;
// components read before the dot from locals 96; dot3f(vector, axis) 96; components before the dot 96.
// Round 457: shared vector scales and sum with ordinary/staged cross, projection/cosine
// scale order, original/named ring phase: 244/244/243/243/258/258/257/257.
// All inputs precede stores; original complete rotation and body restored.
// Round 104 (lane 8, fast): the named ring phase renamed angle_offset/phase/twist 143/143/143 (names
// move no slot); the dead pathfinding store dropped 204 (1056 bytes; retail has the store, sharing
// the sine's slot); the sector index read at the trace call 265; the rotation helper __forceinline
// 90 (identical); the pathfinding pointer as a ternary 217.
// Round 126 (m3 lane 6, fast with the file's 13 matched functions): the angle with the ring term first 90, the
// distance with the ring product first 90, the distance without the double cast 90, both term orders 90 (all the
// same code); a named ring phase written as the angle's first term 143 (1088 B; retail's SSE ring conversion, the
// rotation then spills).
// Round 109 (lane 8, fast; aligned = difflib count 60): the angle with the ring term first 90; the distance
// with the ring product first 90; both 90; the distance as `(double)radius * 1.2 + radius * (real)ring` 90;
// `(real)(radius * 1.2 + ring * radius)` 90 (all same code: VC canonicalises the operand orders, and the
// shared (real)ring stays an x87 fild with an fst for the SSE phase); the ring product in double 260 (1070
// bytes; not the same arithmetic).
// Round 26 (m3 lane 2, fast), on the 96 form (0x29c860's rotation in place, ring phase named before the distance;
// its residuals: the sector index read after `add ebp, ecx` and the radius copied through integer registers, the
// phase store before the spoke init): the radius read from the object at each use 262; the phase at function scope
// 96; the radius reference before the sector index 96; the sector index after the ring counters 97; the radius copy
// and reference after the ring counters 263.
// Lane 18 r14 (one-link LTCG env, exact for this function: env count 70 = check 90): rotation helper with the j/k
// cross terms first and the i term in its row, in place as a vector 75, as scalars 75, on component copies as a
// vector 75, as scalars 70 (same code as now); each with the ring phase named before the distance 78/78/78/125;
// the current helper with the named phase 125. On the vector form with the phase (rotation core and SSE ring
// conversion as retail, radius copied through integer registers): plain radius 213, sector index first with a
// plain radius 247 / with the reference 245, const radius 213. All 27 of current/vector-in-place/vector-on-copies
// helper x no phase/phase before/phase after the distance x reference/plain/reference-after-sector radius: none
// below 70 (vector forms without the phase 75, with it 78; the current helper with a phase 125).
// Round 233: signed-word ring with short/word/char/byte spokes 117/117/246/246;
// unsigned-byte ring with the same spokes 130/130/247/247. Original full-width
// ring, word spoke, helper and complete body restored; ring conversion/spill remain.
// Round 261: complete native rotated-vector publication with scalar/native input snapshots,
// split/original shared-dot projection and no/early ring phase 239/282/239/282/250/290/250/290.
// All restored; SSE ring conversion and the unspilled rotation remain.
// Round 52 (lane 10, fast): retail's store of the pathfinding data to a slot it never reads before the 0x26d100
// call is the argument LTCG drops from 0x26d100's unused last parameter (0x26d0e0 passes its third argument,
// 0x1fa7f0's result, the same way); passing 0x1fa7f0's result there replaces the volatile local at the same 90
// (same code; fast only: the plain check hit a Wine hang, body restored). Passing it at every call is BLOCKED-BY
// 0x270400 (contributor call lines); a defaulted parameter needs a ruling. On that form: the getter at the trace
// call as well 92, as a named local there 92 (only the rotation's XMM numbering moves); a plain radius 211 (the
// score still rests on the `real const &` radius); the ring term first in the angle 90; the distance product as
// radius times ring 90.
// Round 233: signed-word ring with short/word/char/byte spokes 117/117/246/246;
// unsigned-byte ring with the same spokes 130/130/247/247. Original full-width
// ring, word spoke, helper and complete body restored; ring conversion/spill remain.
// Round 261: complete native rotated-vector publication with scalar/native input snapshots,
// split/original shared-dot projection and no/early ring phase 239/282/239/282/250/290/250/290.
// All restored; SSE ring conversion and the unspilled rotation remain.
// Round 35 (lane 15, fast): the angle with the ring term first 90 (same code); sine/cosine computed before the
// direction copy 101 (1086 bytes); the direction copied after the angle 101 (1086); the origin pointer taken
// before the pathfinding lookup 90; the distance as `ring * radius + radius * 1.2` 90.
// Round 64 (lane 16, fast): the 96 form (j/k cross terms first, ring phase named) reproduces at 96 (1090 bytes); on it
// the angle, sine and cosine before the direction copy 246 (1104 bytes), the angle alone before the copy 246, the radius
// as a `real const` local without the reference 222 (1083 bytes); on the current body the angle and trig first 101,
// the `real const` radius 211 (1092 bytes).
// Round 194 (fast): the j/k cross terms in a vector re-measured 100 (1081 bytes), with the named ring phase 96
// (1090; there the dot sums k, j, i where retail sums j, i, k, the direction components sit in xmm5/4/3
// for retail's xmm4/3/5, and the radius copies through EDX); on that 96 form the cross terms before the dot
// 98, the component copies before the dot 96, the dot written on the copies 96 (without the phase 100),
// the radius read before the record pointer 108.
// Round 150: distance terms swapped (ring product first) 90; angle terms swapped (ring term first) 90; both 90;
// the distance without the explicit double cast 90; the rotation's dot as dot3f(vector, axis) 90 (all the same
// code: the ring still converts with fild).
// Round 233: signed-word ring with short/word/char/byte spokes 117/117/246/246;
// unsigned-byte ring with the same spokes 130/130/247/247. Original full-width
// ring, word spoke, helper and complete body restored; ring conversion/spill remain.
// Round 261: complete native rotated-vector publication with scalar/native input snapshots,
// split/original shared-dot projection and no/early ring phase 239/282/239/282/250/290/250/290.
// All restored; SSE ring conversion and the unspilled rotation remain.
// DH271: Byte/char/short/word remaining counts, each with original/named ring phase,
// 90/143/90/143/90/143/90/143. Native remaining-count widths do not free the SSE conversion/rotation slot;
// the named phase still grows the frame. Original complete body, rotation input snapshots, radius reference
// and inherited local qualifier restored; radius schedule, ring conversion and rotation spill remain.
// DH274: Native entry ordinal snapshots as short/word/long/dword, each with nested sector success or missing-sector continuation, 222/222/222/222/200/200/200/200. All exact-only forms restored; ring conversion and rotation spill remain.
// DH276: mutable scalar row/phase accumulators 215/215; native output row/phase
// accumulators 239/239; scalar accumulators with outer ring phase 233/233;
// native output accumulators with phase 250/252. All restored; SSE ring conversion
// and the rotation spill remain. Original helper and all input snapshots retained.
// DH280: Complete projected-axis/cosine-vector/sine-cross operand-priority subsets, then all with internal
// cross-product sides commuted [90, 90, 90, 90, 90, 90, 90, 90]. Original executable body restored.
// N301: Complete trace/distance/origin scope ends at point before collision, with
// original/real short marker and aggregate/library fallback transfer 90/90/90/90;
// each with the outer ring phase 143/143/143/143. All restored; ring conversion/spill remain.
// Round 29 (lane 17, fast), on the 96 form (register-masked aligned difference 20 against 37 for the current body; left are
// the radius copy through integer registers, which moves the sector load, the dot's (k + j) + i for retail's (j + i) + k,
// and i, j, k in xmm5/4/3 for retail's xmm4/3/5): dot as `(j + i)` then `+= k` 218 (1083 bytes); three accumulated terms 102
// (1086); i, j, k defined k first 96; dot written j, i, k with named components 96; radius declared then assigned 96; through
// a pointer local 96; through an inline getter 96; plain radius re-measured 222 and sector index first 263 (repeats, read
// for their heads: sector first also moves the object lookups, so it is not retail's source order); the direction copied
// field by field in all six orders 242/242/242/243/243/243 (1081), with the dot written j, i, k 237/237/238/238/238/238
// (1085); a named double base distance 96; the distance through an inline helper 96; a named (real)ring 96.
// The ring loop as a forceinline helper taking the radius by value (no reference; retail's movss-then-fld is the shape of a
// by-value float parameter elsewhere), on the 96 form 271 (1067 bytes), radius named in the caller 222 (1083), sector read in
// the helper 222 (1083), without the phase 243 (1074); on the current rotation with the phase 262 (1079), without 235 (1092).
// The radius still copies through integer registers in all six.
// DH282: Complete repeated scalar-input immutable declaration orders ijk/ikj/jik/jki/kij/kji, then kij/jik
// with the native outer ring phase [90, 90, 90, 90, 90, 90, 143, 143]. Original executable body restored.
// N323: Immutable underlying radius/ring distance/complete angle-trigonometric storage, seven subsets then immutable projection/component inputs: 90/90/90/90/90/90/90/90. Original complete helper/body restored; ring SSE conversion and rotation spill remain.
// Round 34 (lane 17, fast): trace.distance read at each use (no trace_distance local) 101 (1084 bytes); the
// pathfinding as a ternary 217 (1086); the origin pointer before the pathfinding lookup 90 (same code); the
// direction copy after sine and cosine 101 (1086); the half-axis offset added in the point's first statements 114.
// P284: Real sine/cosine API subsets 215/215/215; each with the outer ring phase
// 233/233/233; unrounded angle with original/real APIs 90/90. Complete original
// body, angle carrier and helper restored; ring SSE conversion, rotation spill
// and radius-save scheduling remain.
// Round 121 (fast; aligned count, control 93): named ring value 90 (92); with a named ring phase above the spoke loop 143 (144);
// direct sin/cos arguments 215 (81), with the named ring 218 (95), with the phase as well 231 (133).
// Also: origin declared with the sector index 90 (93); trace distance read at each row 101 (98); origin first in the point rows 90 (93).
// Round 205 (m3 lane 3, fast): distance computed in the spoke loop after the angle 240 (1082 B; cvtsi2ss as
// retail but ring times radius moves to SSE); that with the ring product in double 248; ring phase named before
// the distance 143 again (the ring block is then retail's instruction for instruction; left are frame 0xdc from
// the rotation spill at +0x40, the radius copied through EDX and the sector load after the radius); the collision
// marker and the entry pointer between the trace rows and the half-axis adds 216 (1092 B); the marker alone there
// 89 (retail stores it before the last point stores; a one-point gain, not kept); the trace, world and collision
// records declared at function scope 94; the trace record alone there 90 (same code).
// Round 64 (lane 10, fast; masked = aligned, registers and stack offsets ignored, 37 here): ring conversion as one in-place ring angle in the outer loop 142 (1088, cvtsi2ss as retail, radius through
// integer registers, masked 37); cross terms named, j and k first 90 (same code), with the angle 142; rows written j, k, i 189 (1088, masked 44); dot written (j + i) + k from the read components 93
// (masked 37), with the angle 144 (masked 33); the k*i and i*j subtrahends named before the dot 215 (1092, masked 36), with the angle 237 (1095, masked 35), named after the dot 237.
// Round 59 (m3 lane 13, fast): rebuilt the 96 form (j/k cross terms in a vector first, ring phase named before the
// distance; 1090 bytes). There the sector index is read after the radius, so the radius copies through EDX where
// retail reads the sector into EDX first and copies the radius through XMM0 (every general register is busy at that
// point), and the integer register names cascade from that. On the 96 form: the radius read before the record pointer 108;
// the ring counters declared first 97; the origin pointer at function scope 96; the record pointer as an array
// element 96; the ring phase named in the spoke loop 100 (1081 bytes, conversion back to fild). Original body restored.
// DH320: Existing bitmap activation ordinal char/signed-char/byte/short/word/int/
// unsigned-int/dword domains: 91/91/92/90/92/90/91/91. Both producer users checked;
// byte domains lose the matched peer. Original helper/body restored; ring conversion/spill remain.
// DH339: Trace/world/collision declared at native spoke scope, seven subsets then all
// with the actual point workspace, eight at 90. Original declaration scopes and complete
// body restored; radius schedule, SSE ring conversion and rotation spill remain.
// DH341: Bounded spoke !=8 / <=7 termination in short/long/word/byte storage:
// 90/91/232/232/91/92/232/232. NOT kept: no gain or native ring/rotation recovery.
// Original signed-short bound, two-ring traversal and complete body restored.
// DH344: Existing complete point setters at trace/elevation/collision and their pairs/all,
// then all with the outer ring phase:
// 92/90/90/92/92/90/92/144. NOT kept: no exact publication form; radius schedule, ring conversion and rotation spill remain.
// Original complete executable source restored.
// Round 147 (m3 lane 6, fast). The current body with a named object pointer read before the sector index 109 (1085 bytes). On
// the 96 form (rebuilt 96, 1090 bytes): that object pointer with the radius reference 263 (1074), with a plain radius 271 (1067),
// the sector index first with the reference 263 again; `1.0f - cosine` named 96; the dot named 96; both named 96; a const sector
// index 96; the i cross term named 96. The 96 form still reads the sector after `add ebp, ecx` and copies the radius through EDX.
// The one-file harness is not faithful here (227, 1103 bytes, with three callee files). Original body restored.
// DH348: Rotation scalar-input CRT captures in ijk/ikj/jik/jki/kij/kji order
// 275 each; ijk/kji with native radius CRT capture 276/276. NOT kept: frame/length
// regress and no exact boundary. Original helper, radius reference and full body restored;
// radius schedule, ring SSE conversion and rotation spill remain.
// Round 129 (m3 lane 7, fast, control 90; aligned/masked = difflib counts, control 58/37). On the 96 form (63/20): the helper reading vector-> directly in place, no
// component copies, 96 (same code; correct without copies because j and k cross terms are taken before the i row stores). What the named ring phase costs is the top block
// (helper alone 100 keeps retail's `mov edx, [ebp + ecx + 0x34]; add ebp, ecx` and the movss radius copy, phase alone 143, phase with the helper 96, phase after the distance 96);
// sector read first 263 (the sector then stores at once and the loop head padding goes, 1074 B), through its own record lookup before/after the radius 260/255, object pointer
// local first with sector before/after radius 263/96, radius before the record 108, ring value named and shared 90 (x87 fild stays). Dot order: retail's (j + i) + k follows
// component scalars declared k, i, j ahead of the dot, with scalar cross terms 143 (retail's dot loads, but the cross subtrahends sink and j spills, frame 0xdc); with the vector
// cross those declarations change nothing 96; dot terms written k,i,j or j,i,k or swapped sides 96 each (same code), accumulated from k or j 213/218 (1083 B); cross before the dot
// 98 (i in xmm4 as retail; (k + j) + i stays), j then k or k then j cross 96/105. The rotation written in the caller, not a helper, 233/243/232/240 (copy-first products).
// N382: Native record/origin/entry object references, seven ownership subsets, then all
// with outer ring phase: 90/90/90/90/90/90/90/143. NOT kept: no equality or gain;
// original complete body restored. Radius schedule, ring conversion and rotation spill remain.
// Round 138 (m3 lane 8, fast unless marked; control 90): `(real)(x * 1.f)` phase steps (diagnostic) 90/215/215 at depth 1/2/3, so no
// phase lead here; extra parameters on inline no-ops at seven sites compile the same (compile-only). Ring value and phase named, the
// angle adding the phase 143 (1088); sector read first 235, with the radius reference 109; the rotation with j/k cross terms named 90
// (same code), as a cross vector read without component copies 100 (1081), that with the named ring 96 (1090, the known 96 form, plain
// 96); a full cross vector 100, with the ring 149; dot written out 90, with the ring 142; components copied before an inline dot 145;
// the planar dot2d plus k term 96 (canonicalised like dot3f); a point-along helper at the trace point 96, at both point rows 113 (1086);
// the structure pointer read before the rotation 72 only together with the staged dot below, alone 166 (1096). On the 96 form the
// dot staged as `j*j + i*i; += k*k; *= 1 - cos` 72, plain 72 (1090, masked 20: retail's (j + i) + k and its rotation rows; left are the
// sector read before `add ebp, ecx`, the radius copied through EDX, the spoke counter's store and the 0x2104b0 argument setup). That
// writes out dot3f and splits a sum, so it is not kept; a natural source for retail's dot order is still open.
// M3 lane 11 r71 (fast, control 90 at 1085 bytes). Retail's rows subtract axis cross vector, with j and k ahead of
// the rows and i in its row: three named cross terms 100 (1087), with the ring phase 149 (1090), dot after the cross
// 231/251; j and k only 100 (1081), with the phase 96 (1090). On that 96, whose ring block and rotation are retail's
// sequence apart from register names: sector first 263 (1074), plain radius 222, plain radius with sector first 271,
// reference declared first 96, radius before the record 108, object pointer 263, const phase 96, ring counters
// swapped 96, double term on the plain local 96, ring term on it 96, named double at function scope 96 (222 without
// the reference), const local 96.
// Bisect, without the cross: phase after the distance 143, its operands swapped 143, ring product swapped 143, sum
// swapped 143, named double 142, phase from a named `(real)ring` 143. The phase in any position keeps `cvtsi2ss` and
// copies the radius through an integer register with the sector read late; a named `(real)ring` with no phase keeps
// `fild` and retail's XMM0 radius (90). The two never appeared together, and dead pointer chains of 1-8 links at
// three sites change neither (96). On the 96: trace.distance at each use 89 at retail's 1089 bytes (one point, and
// retail keeps the distance in a register, not kept), with direct sin/cos 228, direct sin/cos alone 230, along after
// the copies 96, scalar cross terms 143, with the trace form 148, angle written twice 96. On the 89: rotation reading
// `vector->` with no copies 89, three cross terms that way 142, a copy of the vector 263, written-out dot 89, dot
// arguments swapped 89.
// Round 133 (m3 lane 7, fast; aligned = difflib count, control 93): the two rings as a plain `for (long ring = 0; ring < 2;
// ring++)` 266 (1057 bytes, frame 0xd4: no countdown, so retail's countdown is real); in the rotation helper the component
// copies before the dot 90 (same code), the rows' first two terms swapped 90 (same code); the direction copied after the
// sine and cosine 101 (aligned 114); the pathfinding pointer as a ternary 217 (aligned 101). Body unchanged at 90.
// MiniMax B (11-06) round 2: no new form, no lower score (90); the ring value read into a float
// local inside the ring block and used for the distance gives the same code (90), the same local
// also used for the angle gives the same code (90), the angle's ring term named on its own gives
// the same code (90), the distance with the ring's float product widened by an inner cast gives
// the same code (90), the distance written with the ring and the radius both cast to double
// scores 266, and the float local used for the distance and the angle together scores 259; the
// ring is still converted with `fild` where retail converts it to float, keeps that float in a
// stack slot and reloads it as double: the compiler folds the float local back into the integer
// conversion, so the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (90); the ring value is still converted to
// double with `fild` where retail converts it to float, keeps that float in a stack slot and
// reloads it as double, and the rotation block still uses a different register set from retail's
// (retail keeps the three components in registers, ours spills one); naming the ring value
// once for both the distance and the angle scores 259 and the distance term written with the
// inner cast scores 259, so the body is unchanged.
// Round 141 (m3 lane 7, fast, caller 0x26dde0 matched): the record as `&records[result & 0xffff]` 90; the ring converted once into
// a named value for the distance and the angle 90 (the conversion stays `fild` then `fst`), with the array record 90; the rotation
// helper through cross3f into a cross vector 233 (1101 B), with the named ring 233; the entry direction stored j then i 93, the
// entry's field10 stored before the direction 90 (same code); the second pathfinding lookup read through the global at both uses 90;
// a named axis pointer for every g_4687b0 use 268 (1068 B); the trace test as two nested ifs 90 (same code). Body unchanged at 90.
// M3 lane 14 r74 (fast, base 90, aligned 58): in the rotation helper the j and k cross terms named ahead of the rows 90 (same code); only their
// subtrahends named (axis k times i, axis i times j, the two products retail forms beside the dot) 215 (1092 bytes, aligned 67); the ring phase named
// before or after the distance 143 (1088 bytes, aligned 113: retail's cvtsi2ss, store, fld, but the radius then copies through an integer register),
// with the named cross terms 143, with the named subtrahends 233 (1095); the ring as a named float used by the phase and the distance 143, with the
// named cross terms 143; the 1.2 product as a named double ahead of the loop 119 (1086, aligned 87), with the named phase 143; a float ring counter
// 238 (1103), with the named double 241 (1088); the distance without the (double) cast 90 (same code). Body unchanged.
// (The named phase re-measures rounds 17 and 53's 143; the named j, k cross terms give 90 here where round 50 records 100 for "cross j, k before
// i", so that round's form was a different one and was not rebuilt.)
// Lane 3 r230 (base 90): the existing rotate_direction (forceinline, matched in 0x1ecf50) for the rotation, plain 100
// (1081 bytes, frame 0xd4: the rotation no longer spills); with the ring phase named before the distance, plain 96 (1090
// bytes, frame 0xd8, retail's cvtsi2ss/store/fld ring block and an unspilled rotation; the radius still copies through
// EDX with the sector read after it, and the dot sums (k + j) + i); the private helper given that body with
// dot3f(axis, vector), plus the phase, 96 (same code); on that, the direction declared at function scope 96; a 600 s
// permuter run on it (47 tries) best 89, the listed trace.distance-at-each-use form.
// Round 148 (m3 lane 8, fast with the file's other functions and 0x26c590, 0x26d100, 0x2104b0, 0x14a280, 0x26d3f0; base aligned 58,
// masked 37): the rotation helper's dot through this file's record_dot3f 90; the origin as a `point3f const *` 90; the ring
// counters declared countdown first 90; the first pathfinding lookup as an if/else assignment 217 (1086 B, the same code as
// round 133's ternary); the trace point declared ahead of the trace distance 90; the second pathfinding lookup through its
// own structure local 90 (all 90s the same code).
// M3 lane 16 r100 (fast, 0x26dde0 stays matched). Lead: the existing cross-file rotate_direction (declared as bipeds.cpp does; list
// 0x1ecf50 so --fast inlines it) for record_rotate_vector gives retail's rotation shape with no spill: cross j and k form early, cross
// i sinks into the i row. Alone 100 (1081 B, frame 0xd4 for 0xd8); with the ring phase named in the outer loop (`real phase =
// (real)ring * 0.39269909262657166f;`, angle `spoke * 0.785.. + phase`) 96 (1090 B, frame 0xd8, retail's cvtsi2ss/store/fld/mulss/
// fmul ring sequence). Left at 96: the dot's terms (ours j, k, i; retail i, j, k) and an XMM3/4/5 permutation, the radius copied
// through EDX with the sector read late, the phase stored before the spoke's zero, the direction copy based in ECX. On it: a plain
// radius local 112, a const radius local 112, the sector read first 263, the phase after the distance 96 (same code); a named float
// ring value without the phase 100 (same code as alone). The cross-file rotate_vector_170d70 100, with the phase 96 (same code); the
// file helper in rotate_direction's shape (named cross terms read from the vector, dot3f(axis, vector)) 100 / 96 (same code: the dot
// order does not follow dot3f's argument order); cross terms named from the i/j/k copies 90 (same code as the body); crosses before
// the dot 103; crosses read from the vector with the copies kept for the rows 114 (either order; the dot then starts with i as
// retail but adds k before j; all six declaration orders of the copies 114, same code). The dot written out i, j, k or k, j, i, or as
// `(1.0f - cosine) * dot3f(..)`, compiles the same (VC orders the terms itself). Body unchanged.
// M3 lane 16 r102 (fast, 0x26dde0 stays matched, 0x1ecf50 listed): round 100's rotate_direction + named-phase lead re-measured with the
// phase after the distance and before it, 96 both (1090 B, registers masked 20 against 33; left: the radius copied through EDX where
// retail uses XMM0, the sector field read after it, then registers). On the lead: the ring as one named float for the distance and the
// phase 96, the angle as `phase + spoke * 0.785f` 96 (same code each); the direction copied after the sine and cosine 246 (1104 B); sine and cosine passed
// straight to rotate_direction 230 (1094 B). The `volatile` pathfinding copy removed 204 (1056 B; retail keeps the lookup), on the
// lead 199 (1061 B). Body unchanged at 90.
// M3 lane 12 r111 (fast, 0x26dde0 and 0x1ecf50 matched each time): lane 16 r100's lead rebuilt (rotate_direction declared as
// bipeds.cpp does, ring phase named in the outer loop) 96 (1090 B). On it: trace.distance read at each use (no local) 89 at retail's
// 1089 bytes (first +0x45; the entry still reads the sector after the radius and copies the radius through EDX); with the radius
// read as `object_get(object_index)->unknown03c` 89 (same code); the ring counters declared ahead of the radius 90; the origin
// pointer declared beside the sector index 89 (same code); `real const &radius = (real)object_get(..)->unknown03c` 264 (1077 B: VC
// binds the reference to the field). The named phase is a one-use value ahead of the spoke loop's calls, so none is kept. Restored.
// M3 lane 8 r152 (fast, 0x1ecf50 listed): re-measured the rotate_direction + named-phase lead 96 (masked 20: the radius copied
// through EDX with the sector read late, spoke-loop scheduling, the marker order) and it with trace.distance at each use 89
// (1089 B). New on the lead: a `long` spoke counter 239 (1080 B); the radius as a `real const &` bound straight to the object's
// field 263 (1078 B); the spoke's angle term named (`spoke_angle + phase`) 220 (1083 B); an early `return NONE` after the first
// lookup 255 (1090 B), the same on the body 255. Same code as the lead or the 89 form: the collision marker stored before the
// entry pointer. The 0.1-offset point as a copy of the collision point plus three `+=` 189 (1109 B, likely the listed
// collision-offset vector). Compile-only identical: `record->count++;` split from the 32 test, the trace failure as a guard
// `continue`, the trace or world declared in the outer scope, the entry flag set after its direction fields. Body unchanged at 90.
// M3 lane 15 r75 (fast, 0x26dde0 and 0x1ecf50 stay matched): the radius through the typed object field
// `object_get(object_index)->unknown03c` 90 (same code as the byte-offset cast). On the rotate_direction + named-phase lead
// (re-measured 96, 1090 bytes): the volatile copy replaced by the real form, the pathfinding passed as 0x26d100's last parameter
// (definition, this call and 0x26d0e0's) 96, the same code, but 0x26d3f0 stops matching; the spoke counter declared and zeroed
// before the phase 94 (1090 bytes; it moves only the spoke zero's store); the sector index read in the trace call (no local) 263
// (1066 bytes). Body unchanged at 90. I found no other untried natural form; the forms above cover the rest.
// m3 lane 3 r233 (fast with the file's functions and callees; control 90 at 1085 bytes, aligned 58, register-masked 33).
// Retail's rotation is this helper's formula term for term (rows a*along + d*cos - c*sin, the j and k cross subtrahends
// axis.k*d.i and axis.i*d.j scheduled beside the dot); what differs is scheduling and the spill. In the rotation helper:
// the dot named on its own line before `along` 90 (aligned 56, masked 38: other code, same count); the cross terms written
// with their sides flipped and added (`+ (axis->j * k - axis->k * j) * sine`, exactly the same values) 86 at 1087 bytes
// (aligned 56, masked 38: positional, and the subtraction order is retail's own); both 90 (masked 50). Re-measures of the
// listed object-reference contracts on this base: the axis taken by `const &` 90, the vector by reference as well 90 (same
// code each). In the entry tail: `record->count` read once into a named short for the activation pair 128 (1081 B: retail
// reads the count again after the word store, which may alias it); the 32 test as `>=` 91. Body unchanged.
// M3 lane 5 r177 (fast, 0x26dde0, 0x26d3f0 and 0x26d100 matched each time): the ring loop as `do { ... ring++; } while (ring < 2)`
// with no separate countdown 268 (1057 B, like the plain `for`); the distance computed at the top of the spoke loop (before
// the direction copy and the angle) 242 (1097 B); the rotation's cross terms written `sine * (..)` 90 (same code); the
// pathfinding lookup ahead of the direction copy 163 (1092 B); the 0.1-offset point in its own point local in place of
// reusing `point` 117 (1085 B: retail reuses the trace point's slot).
// M3 lane 9 r118: no trial run. Every lead in this note was checked against the round's listing (first difference +0x1a is
// the exit jump, shifted by the 4 missing bytes; then the x87 ring block and the rotation spill). Lane 15 r75 and lane 5
// r177 above found no untried natural form; the coupled causes (an SSE use of `(real)ring` ahead of the distance gives
// retail's cvtsi2ss but moves the radius copy into EDX and the sector read after `add ebp, ecx`) are covered by the 96/89
// lead's listed variants (sector first, radius placement, plain/const/reference radius, object pointer, own record lookup).
// @retail 0x26d570
long function_26d570(long object_index)
{
	long result = function_26d3f0(object_index, 0);
	if (result != NONE)
	{
		s_location_record_view *record = (s_location_record_view *)(g_51eca4->data + (result & 0xffff) * sizeof(s_location_record_view));
		real radius_value = *(real *)((byte *)object_get(object_index) + 0x3c);
		long sector_index = record->location_index;
		real const &radius = radius_value;
		long ring = 0;
		long rings_remaining = 2;
		do
		{
			real distance = (real)((double)radius * 1.2 + (real)ring * radius);
			for (short spoke = 0; spoke < 8; spoke++)
			{
				vector3f direction = *g_4687a8;
				real angle = (real)spoke * 0.7853981852531433f + (real)ring * 0.39269909262657166f;
				real sine = (real)sin(angle);
				real cosine = (real)cos(angle);
				record_rotate_vector(&direction, g_4687b0, sine, cosine);
				s_record_pathfinding_view *structure = (s_record_pathfinding_view *)g_4e0348;
				s_pathfinding_data *pathfinding = NULL;
				if (structure->count > 0)
					pathfinding = structure->pathfinding;
				s_sector_trace_result trace;
				point3f *origin = &record->point.point;
				if (!function_26c590(pathfinding, origin, sector_index, NONE, &direction, distance, NULL,
					(s_path_trace_result *)&trace) && !trace.blocked)
				{
					real trace_distance = trace.distance;
					point3f point;
					point.x = direction.i * trace_distance + origin->x;
					point.y = direction.j * trace_distance + origin->y;
					point.z = direction.k * trace_distance + origin->z;
					point.x += g_4687b0->i * 0.5f;
					point.y += g_4687b0->j * 0.5f;
					point.z += g_4687b0->k * 0.5f;
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
					entry->owner = function_26d100(g_4687b0, &world, (long *)&entry->location, (s_collision_result_1697c0 *)&collision);
					point.x = g_4687b0->i * 0.1f + collision.point.x;
					point.y = g_4687b0->j * 0.1f + collision.point.y;
					point.z = g_4687b0->k * 0.1f + collision.point.z;
					long sector = function_14a280((s_bsp3d *)g_4e0340, 0, &point);
					if (sector != NONE)
						entry->sector = ((s_record_sector_map *)g_4e0348)->entries[sector].sector;
					if (entry->sector != NONE)
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
