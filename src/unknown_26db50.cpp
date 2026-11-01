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
// Round 52 (lane 10, fast): retail's store of the pathfinding data to a slot it never reads before the 0x26d100
// call is the argument LTCG drops from 0x26d100's unused last parameter (0x26d0e0 passes its third argument,
// 0x1fa7f0's result, the same way); passing 0x1fa7f0's result there replaces the volatile local at the same 90
// (same code; fast only: the plain check hit a Wine hang, body restored). Passing it at every call is BLOCKED-BY
// 0x270400 (contributor call lines); a defaulted parameter needs a ruling. On that form: the getter at the trace
// call as well 92, as a named local there 92 (only the rotation's XMM numbering moves); a plain radius 211 (the
// score still rests on the `real const &` radius); the ring term first in the angle 90; the distance product as
// radius times ring 90.
// Round 35 (lane 15, fast): the angle with the ring term first 90 (same code); sine/cosine computed before the
// direction copy 101 (1086 bytes); the direction copied after the angle 101 (1086); the origin pointer taken
// before the pathfinding lookup 90; the distance as `ring * radius + radius * 1.2` 90.
// Round 64 (lane 16, fast): the 96 form (j/k cross terms first, ring phase named) reproduces at 96 (1090 bytes); on it
// the angle, sine and cosine before the direction copy 246 (1104 bytes), the angle alone before the copy 246, the radius
// as a `real const` local without the reference 222 (1083 bytes); on the current body the angle and trig first 101,
// the `real const` radius 211 (1092 bytes).
// Round 150: distance terms swapped (ring product first) 90; angle terms swapped (ring term first) 90; both 90;
// the distance without the explicit double cast 90; the rotation's dot as dot3f(vector, axis) 90 (all the same
// code: the ring still converts with fild).
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
					entry->owner = function_26d100(g_4687b0, (long *)&entry->location, (s_collision_result_1697c0 *)&collision, &world);
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
