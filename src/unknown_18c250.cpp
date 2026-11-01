// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_18C250.CPP: the sound source callbacks built with /arch:SSE
   (the tables are in unknown_18c810.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "unknown_249e20.h"
#include "local_cameras.h"
#include "unknown_11a4d0.h"
#include "unknown_1428b0.h"
#include "sound_promotions.h"
#include <math.h>
#include <string.h>

/* an object, as the sound source code reads it */
struct s_sound_object_view
{
	byte unknown00[0x10a];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word unknown10a : 13;
	byte unknown10c[0x1da - 0x10c];
	short machine_index;
};

struct s_sound_object_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_sound_object_view *object;
};

struct s_sound_class_flags
{
	byte unknown00[0xa];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word unknown0a : 4;
};

struct s_sound_tag_class
{
	byte unknown00[2];
	char class_index;
};

struct s_unknown_5c;
s_object *function_badc0(long object_index, dword type_mask);
s_unknown_5c *function_221810(short index);
dword vector3d_compress(vector3f const *vector);
void function_11bed0(s_location *location, point3f const *point);
char function_18d4b0(long tag_index, char audible, long object_index, long *local_player_index);

static inline s_sound_object_header *sound_object_header(long object_index)
{
	return (s_sound_object_header *)g_4e0300->data + (object_index & 0xffff);
}

static inline vector3f *transform4x3f_apply_normal(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

static inline vector3f *cross3f(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = b->k * a->j - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
	return result;
}

/* the axis and the vector are taken into locals in that order: it sets the order of the cross
   product's operand loads (0x18c250) */
static inline vector3f *function_x84d9e8(vector3f *vector_in, vector3f const *axis_in, real sine, real cosine)
{
	vector3f cross;
	vector3f const *axis = axis_in;
	vector3f *vector = vector_in;

	cross3f(axis, vector, &cross);
	vector->i = cosine * vector->i + sine * cross.i;
	vector->j = cosine * vector->j + sine * cross.j;
	vector->k = cosine * vector->k + sine * cross.k;
	return vector;
}

// @retail 0x18c250
bool __stdcall function_18c250(long object_index, long tag_index, s_sound_marker const *marker, s_type_99c531 *location)
{
	bool result = object_index == NONE || function_18c3b0(object_index, tag_index, marker, location);

	if (object_index == NONE || result)
	{
		s_sound_tag *sound = (s_sound_tag *)((s_tag_instance_view *)g_4e3b44)[tag_index & 0xffff].data;
		register long local_0 = sound->class_index;
		s_sound_class_view *local_1 = ((s_sound_globals_view *)g_51ebd4)->classes;
		s_sound_class_spatialization *class_spatialization = sound_class_get_spatialization(
			&local_1[local_0]);

		if (class_spatialization && (class_spatialization->flags & 1))
		{
			location->flags |= 1;
			location->spatial.velocity = *g_4687a4;
			location->spatial.position = *(point3f *)g_4687a8;
			function_x84d9e8((vector3f *)&location->spatial.position, g_4687b0, (real)sin(class_spatialization->angle), (real)cos(class_spatialization->angle));
			result = true;
		}
	}
	return result;
}


// score 23 (was 23): with 0xba300 now returning its location (retail's convention) a bool result set after the
// audible store gives retail's entry (`xor bl, bl`), EBX/EBP roles and split success/failure returns. Left: the
// machine object pointer loads into EAX (retail reuses EBP), the cluster lookup's ECX/EDX roles and base-load
// order, and the final location copy (retail stores its second dword first, interleaved with the call setup).
// Round 34: bool result set before the audible store 68; with it after: the lookup index named 23, as
// `18 * index + row` 23, the cluster table typed as rows of 18 23, memcpy for the location copy 23, a named
// header pointer for the machine read 23. Earlier forms (under the old 0xba300 convention):
// Byte result first 150; Boolean early-success result 152; long result 135; independent type tests 170;
// excluded-class exit 165; long result first 134 kept (plain).
// Root output at entry 134; forward at entry 134; direct successful completion 185; machine arm first 134; CRT location publication 134; existing normal helper input values 55 kept (plain).
// Word/unsigned long/signed char result 55/55/73; named location fields 65; invalid-cluster common exit 55;
// bool result 76 (xor bl, bl agrees; the object index moves to ebp), set before the location copy 76, set after the
// audible store 181; long result set after the audible store 53; cluster index sum operands swapped 55.
// frame 0x18 and normal-transform block agree; entry result/object, root-location and final copy carriers differ.
// Word result 55; unsigned long result 55; signed char result 73; named location fields 65; invalid-cluster common exit
// 55.
// Recovered normal body: Boolean first 76; byte first 73; Boolean after lookup 78; long after lookup 56;
// direct class receiver 55; direct class with Boolean 76; native workspace names 55; success before
// publication 55.
// native byte type after normal gain 113; readonly object 55; readonly matrix 55; named machine row 55; native
// row first 55; high location fields first 65; cluster-derived result 197; eligibility-derived result 199.
// One shared header across type/machine consumers, cached/per-use type, long/bool result,
// native/CRT location publication 144/144/181/181/178/178/186/186. Header lifetime adds receiver pressure; restored.
// Round 431: common word/long datum slots 151/23; with machine receiver reacquired 151/23;
// root-location returned output original/word/long slot 172/108/172; long slot with overlap-safe location copy 53.
// Frame and normal block agree; machine receiver, cluster-map registers and final publication order remain.
// Round 7 (m3 lane 2): `object` reassigned from the header for the machine read 23; local names sorted
// first/last 23/23; `indices[...] * 18 + row` 23; the requested-audible nibble read into a char before the
// location copy 17 (after-call DL as retail; one-use read ahead of a store, kept only on a match), the same
// read after the copy 22; with the early read, a 64-bit copy 17; the copy's high dword written first
// (cast form, diagnostic) 21, retail's first loads and stores, so the copy order follows source order.
// Round 42: named machine object 23; header local in the machine arm 23; indices local 23; object local
// reassigned from the header 23; one header local for the type and machine reads 137. Only dropping the (word)
// cast gives retail's cluster lea/base order, but retail zero-extends the index.
// Round 43 (lane 10): `machine_index & 0xffff` for the cast 23 (same code); the location local at function
// scope 23; `forward` declared before `matrix` 23; a byte object type 110; a cluster table pointer local
// 23; 1-3 unused locals at the top (diagnostic, numbering) 23 each. Retail reuses the dying register in
// both the machine read (EBP) and the cluster sum (EAX holds the row, then the table).
// @retail 0x18c3b0
bool __stdcall function_18c3b0(long object_index, long tag_index, s_sound_marker const *marker, s_type_99c531 *location)
{
	bool result = false;
	s_sound_object_view *object = (s_sound_object_view *)function_badc0(object_index, NONE);

	if (object && !object_or_parent_hidden(object_index))
	{
		s_sound_tag_class *sound = (s_sound_tag_class *)g_4e3b44[tag_index & 0xffff].bytes;
		if (!TEST_FIELD_BIT(object->flag2) || !TEST_FIELD_BIT(((s_sound_class_flags *)function_221810(sound->class_index))->flag11))
		{
			s_location object_location;
			object_get_root_location(object_index, &object_location);

			if (object_location.cluster_index != NONE)
			{
				long object_type = sound_object_header(object_index)->type;
				transform4x3f *matrix = function_b8bd0(object_index, marker->node_index < 0xff ? marker->node_index : 0);
				vector3f forward;

				transform4x3f_apply_point(matrix, &marker->position, &location->spatial.position);
				location->spatial.compressed_forward = vector3d_compress(transform4x3f_apply_normal(matrix, &marker->forward, &forward));
				function_ba1d0(object_index, &location->spatial.velocity, NULL);

				if ((1 << object_type) & 3)
				{
					if (function_11b930(object_index))
					{
						function_11bed0(&object_location, &location->spatial.position);
					}
				}
				else if (object_type == 7)
				{
					short machine_index = sound_object_header(object_index)->object->machine_index;
					if (machine_index != NONE)
					{
						s_local_camera *camera = function_125360();
						if (camera && camera->active)
						{
							short camera_index = camera->index;
							if (camera_index != NONE)
							{
								s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
								long audibility_index = function_249e20(bsp, machine_index);
								if (audibility_index != NONE)
								{
									long row = function_249e60(camera_index, bsp, audibility_index);
									if (row != NONE)
									{
										object_location.cluster_index = bsp->sound_clusters[row + 18 * bsp->cluster_map->indices[(word)machine_index]];
									}
								}
							}
						}
					}
				}

				location->spatial.location = object_location;
				location->audible = function_18d4b0(tag_index, location->requested_audible, object_index, NULL);
				result = true;
			}
		}
	}
	return result;
}

struct s_sound_unit_view
{
	byte unknown00[0x208];
	real sound_value;
};

// @retail 0x18c9b0
void function_18c9b0(long object_index, real target)
{
	if (g_4ed28c->valid && function_badc0(object_index, 3))
	{
		real *value = &((s_sound_unit_view *)sound_object_header(object_index)->object)->sound_value;
		real delta = target - *value;

		if (delta < -0.3f)
		{
			delta = -0.3f;
		}
		else if (delta > 0.3f)
		{
			delta = 0.3f;
		}
		*value += delta;
	}
}

struct s_sound_cluster_view
{
	byte unknown00[0x8c];
	long sound_count;
	short *sounds;
	byte unknown94[0xb0 - 0x94];
};

struct s_sound_environment_view
{
	byte unknown00[8];
	point3f position;
	byte unknown14[0x24 - 0x14];
};

struct s_sound_bsp_view
{
	byte unknown00[0x60];
	s_sound_environment_view *environments;
	byte unknown64[0xa0 - 0x64];
	s_sound_cluster_view *clusters;
};

// @retail 0x18cfd0
long function_18cfd0(long cluster_index, point3f const *point, real *distance)
{
	long result = NONE;
	*distance = 3.4028235e38f;

	s_sound_cluster_view *cluster = &((s_sound_bsp_view *)g_4e0348)->clusters[cluster_index];

	for (long i = 0; i < cluster->sound_count; i++)
	{
		short index = cluster->sounds[i];
		point3f *position = &((s_sound_bsp_view *)g_4e0348)->environments[index].position;
		vector3f vector;
		vector3d_from_points3d(position, point, &vector);
		real length = (real)sqrt(vector.j * vector.j + (vector.k * vector.k + vector.i * vector.i));
		if (*distance > length)
		{
			*distance = length;
			result = index;
		}
	}
	return result;
}

struct s_sound_class_play_bits
{
	byte unknown00[8];
	word flag0 : 1;
	word flag1 : 1;
	word unknown08 : 14;
};

struct s_vibration_curve_set;
void function_10e480(long object_index, long tag_index, s_vibration_curve_set *curves, real time);

static inline s_sound_promotion *sound_promotion_get(s_sound_promotion_tag const *sound)
{
	short promotion_index = sound->promotion_index;
	s_sound_promotion *result = NULL;

	if (promotion_index != NONE)
	{
		result = &((s_sound_globals_promotion_view *)g_51ebd4)->promotions[promotion_index];
	}
	return result;
}

#pragma inline_depth(0)
// @retail 0x18c630
void __stdcall function_18c630(long object_index, long tag_index, long a, long b)
{
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;

	if (function_badc0(object_index, 3) && TEST_FIELD_BIT(((s_sound_class_play_bits *)function_221810(sound->class_index))->flag1))
	{
		short local_0 = sound->promotion_index;
		s_sound_promotion *promotion = NULL;
		if (local_0 != NONE)
			promotion = &((s_sound_globals_promotion_view *)g_51ebd4)->promotions[local_0];
		if (promotion)
		{
			function_12de70(&promotion->block, 2);
		}
	}
}
#pragma inline_depth(255)

// score 7 (was 7): the tail still loads the entry offset into EDX and the samples pointer into ESI, where
// retail loads the samples into EDX and the offset into the dying EDI, and the object index is still loaded
// into ECX before the pushes where retail loads it after them, with the curves address computed by lea eax
// where retail uses lea ecx. This round: the front end canonicalises the sum, so every spelling gives the
// same two loads with the offset first and as the add's destination: the pointer first, both operands as
// dwords in either order, a dword-typed view of the promotion data, the offset cast to unsigned, int or
// size_t, the sample pointer in a local before or inside the block and the offset hoisted before the block
// all give the same bytes (7); reading the scale's bits without the volatile pointer gives the same bytes
// (7), and passing the scale itself gives retail's lea ecx and its late object index but loads the scale
// last (8). A named local for the curve set's address gives the same bytes (7). The permuter ran past its
// 900-second budget on a loaded machine and was stopped by the harness's background limit; it wrote nothing.
// score 7 (was 7): the tail still loads the entry offset into EDX and the samples pointer into ESI, where
// retail loads the samples into EDX and the offset into the dying EDI; the object index is still loaded into
// ECX before the pushes and the curves address with lea eax, where retail uses lea ecx and loads the object
// index after the pushes. This round: the sum with both operands as dwords in either order, the sum through
// an address local, the pointer local with += and the index form all compile to the same two loads (7); with
// the scale read as a plain dword the tail gets retail's lea ecx and late object index but loads the scale
// last (8).
// score 7 (was 7): sample-base and offset registers plus object call setup remain; readonly data
// input 7; readonly entry input 7; native count identity 7; output count lifetime 52; existing
// scale storage lifetime 7; input pointer home names 7; existing scale home names 7; inline matched
// output callee 7; final object address read 7; split construction around count 9; split
// construction count last 7; actual descriptor type 7; nested query guard 7; object snapshot 7;
// mutable permutation input 7; scalar scale copy 8; scalar copy interleaved 11; plain scale
// pointer 7; scalar copy split count last 8; direct scale bits 7; union real assignment 8;
// real assignment after stores 8; scalar copy declared first 8; late object snapshot 7.
// native data/entry/both record copies 56/58/74; direct entry/data/both fields 22/23/65;
// native admission predicate 7; nested availability predicate 7.
// @retail 0x18ca20
void __stdcall function_18ca20(long object_index, long tag_index, s_sound_permutation const *permutation, real scale)
{
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;
	s_sound_promotion_data *data = ((s_sound_globals_promotion_view *)g_51ebd4)->promotions[sound->promotion_index].data;
	s_sound_promotion_entry *entry = &data->entries[permutation->entry_index];
	long count = entry->data_count;

	if (count > 0 && g_4ed28c->valid && function_badc0(object_index, 3))
	{
		s_sound_promotion_state curves;
		union { dword field_0; real field_4; } local_0;
		dword const volatile *local_1 = (dword const volatile *)&scale;
		local_0.field_0 = *local_1;
		curves.data = entry->data_offset + data->samples;
		curves.count = count;
		function_10e480(object_index, tag_index, (s_vibration_curve_set *)&curves, local_0.field_4);
	}
}

static __forceinline long float_to_int_nearest(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

#pragma inline_depth(0)
// @retail 0x18c720
void function_18c720(long tag_index, long object_index, long set_index, long permutation_index, real scale)
{
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;

	if (!(sound->flags & 2) && TEST_FIELD_BIT(((s_sound_class_play_bits *)function_221810(sound->class_index))->flag1))
	{
		short local_0 = sound->promotion_index;
		s_sound_promotion *promotion = NULL;
		if (local_0 != NONE)
			promotion = &((s_sound_globals_promotion_view *)g_51ebd4)->promotions[local_0];
		if (promotion && (function_12de70(&promotion->block, 2) || promotion->count > 0))
		{
			s_sound_permutation_set *set = &((s_sound_globals_promotion_view *)g_51ebd4)->sets[sound->permutation_base + set_index];

			real local_1 = scale * 30.0f;
			long local_2;
			__asm
			{
				fld local_1
				fistp local_2
			}
			function_18c9b0(object_index, function_218e50(tag_index, (short)set_index, (short)permutation_index, local_2));
			function_18ca20(object_index, tag_index, &((s_sound_globals_promotion_view *)g_51ebd4)->permutations[set->first_permutation + permutation_index], scale);
		}
	}
}
#pragma inline_depth(255)

// @retail 0x18c6a0
void __stdcall function_18c6a0(long object_index, long unused, long tag_index, long set_index, long permutation, long scale)
{
	if (object_index != NONE && permutation && function_badc0(object_index, 3))
	{
		s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;
		s_sound_permutation_set *set = &((s_sound_globals_promotion_view *)g_51ebd4)->sets[sound->permutation_base + set_index];
		s_sound_permutation *first = &((s_sound_globals_promotion_view *)g_51ebd4)->permutations[set->first_permutation];

		function_18c720(tag_index, object_index, set_index, (s_sound_permutation *)permutation - first, *(real *)&scale);
	}
}

real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference); /* unknown_20b3c0.cpp */

/* replays the impulse sounds in the slots of g_4ed288 on their objects and
   frees the slots whose sounds are over or whose objects are gone */
#pragma inline_depth(0)
// @retail 0x18bf90
void function_18bf90(void)
{
	for (long i = 0; i < 16; i++)
	{
		s_looping_sound_slot *slot = &g_4ed288->slots[i];

		if (slot->active)
		{
			if (function_badc0(slot->source_index, 3))
			{
				real elapsed = (real)(g_510c54->game_time - slot->end_time) * g_510c54->rate;
				real duration = sound_permutation_reference_duration(slot->datum_index, &slot->permutation);

				function_18c720(slot->datum_index, slot->source_index, slot->permutation.pitch_range_index, slot->permutation.permutation_index, elapsed);
				if (!(elapsed > g_510c54->rate * 10.0f + duration))
				{
					continue;
				}
			}
			slot->datum_index = NONE;
			slot->active = false;
			slot->source_index = NONE;
		}
	}
}
#pragma inline_depth(255)
/* a listener of the sound system (0x48 bytes) */
struct s_sound_listener_view
{
	long leaf_index;
	short cluster_index;
	bool active;
	byte unknown07[0x48 - 7];
};

struct s_sound_system_listeners_view
{
	byte unknown00[0x88];
	s_sound_listener_view listeners[4];
};

struct s_4e6380;
extern s_4e6380 *g_4e6380;

/* the clusters within earshot of a listener */
dword g_54e8a0[16];

// @retail 0x18cb10
void sound_audible_clusters_update(void)
{
	s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
	s_sound_listener_view *listener;
	long listener_index;

	memset(g_54e8a0, 0, ((bsp->cluster_count + 31) >> 5) * sizeof(dword));
	for (listener_index = 0, listener = ((s_sound_system_listeners_view *)g_4e6380)->listeners; listener_index < 4; listener_index++, listener++)
	{
		if (listener->active && listener->cluster_index != NONE)
		{
			for (long cluster_index = 0; cluster_index < bsp->cluster_count; cluster_index++)
			{
				if (function_249d60(bsp, cluster_index, listener->cluster_index) < 256.0f)
				{
					g_54e8a0[cluster_index >> 5] |= 1 << (cluster_index & 31);
				}
			}
		}
	}
}
#include "object_markers.h"
#include "object_queries.h"

extern long const g_444ae0;
extern s_record_pool *g_4ea93c;
extern s_record_pool *g_51ebfc;
struct s_impact;
struct s_impact_sound_location;
void impact_sound_location_get(s_impact const *impact, s_impact_sound_location *location, real *scale);
bool function_17b2d0(long effect_index, point3f *point, vector3f *forward, real *scale);
bool __stdcall function_bab40(long object_index, long name, real *value);
long function_b8c40(long object_index, short entry_index);
short function_188790(real angle);
void function_18d4f0(long object_index, char *audible, long *local_player_index);

struct s_loop_location_datum
{
	short salt;
	byte field_2;
	char type;
	union
	{
		word flags;
		struct { word flag0 : 1; word flag1 : 1; word field_4_2 : 3; word flag5 : 1; };
	};
	short marker;
	real scale;
	long tag;
	long owner;
	union { long name; struct { char source; char listener; short cluster; }; };
};

struct s_loop_location_object
{
	byte field_0[0x12c];
	byte field_12c;
	byte field_12d[3];
	long field_130;
	real field_134;
	real field_138;
	real field_13c;
	real field_140;
	real field_144;
};

PRIVATE inline long loop_location_object_index(s_loop_location_datum const *sound)
{
	return sound->type == 1 ? sound->owner : NONE;
}

PRIVATE __forceinline void loop_location_clear(s_type_99c531 *location)
{
	location->flags |= 2;
	location->scale = 0.0f;
	memset(&location->spatial, 0, sizeof(location->spatial));
	function_11bed0(&location->spatial.location, g_468788);
	location->spatial.compressed_forward = vector3d_compress(g_4687b0);
}

// score 279 (was 279): frame 0x8c versus 0x80; datum/tag homes, constant-one spill and output-address formation differ.
// Earlier: frame 0x8c versus 0x80; datum/tag homes and root-output address formation differ.
// Round 30 (lane D): assigning the root-location result to the destination, and keeping it in a local
// instead, both score 279; a local copy of the sound type for the type-1 test scores 289, the flag test
// through the flag0 bitfield and the type mask shifted as an unsigned 1 both score 279 with the same
// code, and the mask shifted the other way scores 285. A build without the type shift has frame 0x88,
// so the extra dword is the spill of the literal 1 that the flag test, the type-1 test and the shift
// share. What still differs: the frame stays 0x8c against retail's 0x80 and the datum/tag homes differ.
// Original body kept.
// score 279 (was 279): frame 0x8c versus 0x80; datum/tag homes and root-output address formation differ.
// frame 0x8c versus 0x80; datum/tag homes, constant-one spill and output-address formation differ.
// Earlier: frame 0x8c versus 0x80; datum/tag homes and root-output address formation differ.
// Earlier 280 from 280: Frame 0x8c versus 0x80; datum/tag and protected root-location convention remain. BLOCKED-BY 0xba300.
// Frame 0x8c versus 0x80; datum/tag and protected root-location convention remain.
// Round 9 (lane 10): retail's case-3 failure byte is a dead store into the slot the audibility output
// reuses. Forward vector at function scope 280; cluster flag as a word mask 280; case-0 fields read
// without the type test 307; one function-scope status byte for failure and audibility 280 (no store);
// case-2 spatial field passed directly 280. BLOCKED-BY 0xba300 (contributor) still.
// Earlier baseline: Frame 0x84 versus 0x80; datum/tag allocation and protected root-location convention remain.
// marker outer scope 309; direct failure predicate 293 kept (plain); stored-scale arm first 314; ascending cases 311; failure exit 314.
// Long failure result 293; marker consumption scope 293; shared scale object index 339;
// force-inline object-index query 293; marker/axis/audibility role names 293.
// Tag-first declaration 293; explicit failure arms 309; word cluster 294; CRT velocity copy 293; owner value conditional 301.
// Effect axis at entry 293; audibility output at entry 293; direct late tag flag 323 (frame 0x80);
// readonly datum 293; CRT marker position 293.
// signed-word effect failure 293; unsigned native cluster 293; unsigned marker handle 293; immutable datum pointer 293; native clear-output reference 293; unsigned-byte effect failure 293.
// Named location fields 293; failure-first common return 296; cases two/three/zero 296; entry marker-name workspace
// 293; native flag0 view 293.
// Tag acquisition after returning kinds 305; conditional failure 309; conditional marker selection 293;
// audibility-first guard 292 (one-point gain dropped); ordinary clear inline 412.
// Datum/tag lexical names 293; direct success predicate 293; cluster selection tests 293;
// object property base reread 560; const retained tag bytes 293.
// Cases zero/three/two 293, two/zero/three 296, three/zero/two 293;
// direct effect/impact flag masks 345; final native flag mask 300.
// Round 214: direct selected cluster 312; unsigned category mask 293; nested tag/audibility guards 293; conditional final flag 297; force-inline header lookup 293.
// External datum reference 293; late three-query handle 317; marker position setter 303; complete kind if/else chain 322; readonly object properties 293 (fast); all restored.
// Round 213: object-index default result/complete arms/nonobject return/object return
// 325/293/324/293; with direct late tag observation 345/323/343/323. Original helper and body restored.
// Staged failure bool/byte/short/long 323/323/327/327; staged success 265/265/325/322 (match-only, restored);
// complete conditional failure/success 309/309/307/308/293/293/293/293. All restored.
// Round 96: BLOCKED-BY 0xba300: retail passes it the location in EAX and the object in ECX; ours passes the
// location in ESI, so ESI cannot hold the datum and the definition pointer spills (frame 0x84). Without
// that call (diagnostic) the frame is 0x80 with retail's ESI datum / EBP definition. Failure byte in a
// function-scope char shared with the audibility output 293; address-taken failure flag 293.
// Round 152: Cluster/scale/paired/spatial/location+spatial/scale+spatial/all/late-all receivers
// 309/345/336/309/280/331/337/337; genuine location+spatial pair kept at plain 280, 1034 bytes.
// Direct/reversed-direct/entry pair 311/311/282; late pair 278 (two-point gain rejected);
// position/location and spatial/world receiver names 280/280.
// Round 208: existing output bit members 2/3/4/5/6 individually, all five,
// with bit 10, and with bits 10/0 all 293. Original output masks and complete
// body restored; frame 0x84, datum/tag homes and root-location convention remain.
// Round 66: addresses written at each use (no spatial/location pointers) 293; without the root-location call
// (diagnostic) that form gives retail's ESI datum and EBP definition (frame 0x84), the pointer pair does not
// (frame 0x88): retail passes 0xba300 the object in ECX and the location in EAX, ours the location in ESI.
// Failure flag from the bit test, then from the call 333.
// Round 150 (fast): without the root-location call the frame is 0x88, and 0x84 with the addresses
// written at each use (297/316); the 4 bytes left are a spill of the constant 1, which ours holds in
// EBX from the datum flag test on (`test bl, al`, every type compare, `1 << type`) where retail tests
// with immediates and loads `mov bl, 1` only in the marker part. The flag test as the flag0 bitfield
// 280; as `== 0` 280; the object index local dropped from the type test 280 (all identical code).
// Round 231: spatial/cluster/both/flags/spatial-flags/cluster-flags/all field references and all
// plus pooled-datum reference each 280. Original body restored.
// Round 233: cluster/spatial/both receivers bounded to their consuming kind phases 309/293/311;
// marker declared at object-phase entry 280; marker plus those receivers 309/293/311;
// audible output declared at object-phase entry, initialized at its query 280. All restored.
// Round 360: tag pointer acquired in the object phase after admission/name/marker/
// position/forward/velocity/root-location/audibility boundaries 308/316/312/314/316/321/318/320.
// Original acquisition and receiver pair restored; root-location convention remains.
// Round 396: complete datum snapshot with pair/cluster-direct/spatial-direct/both-direct
// 339,339,322,315; typed pool fields at each use with those receivers
// 517,503,463,451. Original datum observations and receiver pair restored.
// Round 180: Entry forward/marker/audibility outputs, masks 1-7, then all in reverse declaration
// order: 280/280/280/280/280/280/280/280. Original executable body restored.
// Round 401: Category membership read from a shifted signed/unsigned mask
// 286/286; with complete two-use negative adjustment 282/282, distance pair
// 305/305, angle pair 310/310. Frame 0x8c and protected root-location destination
// remain; all original field observations and the receiver pair restored.
// Round 178: distance pair read once 306; angle pair once 314; negative adjustment
// once 276 (four-point gain below the gate); distance+angle 324; distance+adjustment
// 307; angle+adjustment 315; all five properties 324; all with readonly receiver 324.
// Original observations and body restored; protected root-location convention remains.
// Round 186: Complete source-kind char/byte/short/long with address/value owner selection
// 346/346/338/351/358/358/350/363. Original field observations, multi-use handle helper and complete
// body restored.
// DL176: Native distance/angle/both property-array loops with long ordinals 280/331/331; both with
// short/word/unsigned-long ordinals 367/367/331; shared long/short ordinal 331/367. Original
// scalar fields, property tests, receiver pair and full body restored; protected root-location and
// datum/tag homes remain.
// Round 217: native root output at query/entry scope via aggregate/library copy 305 each; with
// native velocity output 337 each. Frame 0x8c and datum/tag homes remain. Root-location callee
// now matches and uses object ECX/output EAX; original body restored.
// DL201: effect/audibility ordinary, inline and force-inline contracts, all eight pairs
// 279/556/279/492/279/492/556/756. Ordinary inline forms tie; forced audibility expansion
// loses seven matched callers. All original declarations, definitions and body restored.
// Root-location query now matches; datum/tag homes and the constant-one spill remain.
// Round 247: complete object-phase spatial output at phase/entry scope, with native/
// library marker-position and full-output transfers in all eight combinations, 371
// each (frame 0xb0; reduced 0x21db80 loses). Original direct fields, receiver pair,
// provider calls, output publication and complete body restored.
// @retail 0x18cbc0
void function_18cbc0(long looping_sound_index, s_type_99c531 *location)
{
	s_loop_location_datum *sound = &((s_loop_location_datum *)g_4ed28c->data)[looping_sound_index & 0xffff];
	byte *definition = g_4e3b44[sound->tag & 0xffff].bytes;
	s_sound_position *spatial = &location->spatial;
	s_location *cluster_location = &spatial->location;
	location->flags = 0;
	location->unknown08 = *(sound->type == 0 ? &sound->owner : &g_444ae0);
	location->unknown02 = 0;
	switch (sound->type)
	{
	case 3:
		{
			location->audible = 1;
			vector3f forward;
			bool failed = TEST_FIELD_BIT(sound->flag1) || !function_17b2d0(sound->owner, &spatial->position, &forward, &location->scale);
			if (!failed)
			{
				spatial->compressed_forward = vector3d_compress(&forward);
				spatial->velocity = *(vector3f *)((byte *)g_4ea93c->data + (sound->owner & 0xffff) * 0x190 + 0x30);
				function_11bed0(cluster_location, &spatial->position);
			}
			else loop_location_clear(location);
			return;
		}
	case 2:
		location->audible = 1;
		if (!TEST_FIELD_BIT(sound->flag1))
			impact_sound_location_get((s_impact *)((byte *)g_51ebfc->data + (sound->owner & 0xffff) * 0xa0),
				(s_impact_sound_location *)spatial, &location->scale);
		else loop_location_clear(location);
		return;
	case 0:
	{
		long cluster = sound->type == 0 ? sound->cluster : NONE;
		if (cluster != NONE)
		{
			*(short *)((byte *)cluster_location + 4) = (short)cluster;
			*(long *)cluster_location = NONE;
			*(short *)((byte *)cluster_location + 6) = sound->type == 0 ? sound->listener : NONE;
			((byte *)&location->flags)[1] |= 0x10;
		}
		break;
	}
	}
	if (!(sound->flags & 1))
	{
		long object_index = loop_location_object_index(sound);
		if ((1 << sound_object_header(object_index)->type) & 0x400)
		{
			s_loop_location_object *object = (s_loop_location_object *)sound_object_header(sound->owner)->object;
			location->unknown02 = object->field_12c;
			*(long *)(location->unknown30) = object->field_130;
			if (object->field_134 > 0.0f) { location->flags |= 4; *(real *)(location->unknown30 + 4) = object->field_134; }
			if (object->field_138 > 0.0f) { location->flags |= 8; *(real *)(location->unknown30 + 8) = object->field_138; }
			if (object->field_13c > 0.0f) { location->flags |= 0x10; *(short *)(location->unknown30 + 0xc) = function_188790(object->field_13c); }
			if (object->field_140 > 0.0f) { location->flags |= 0x20; *(short *)(location->unknown30 + 0xe) = function_188790(object->field_140); }
			if (object->field_144 < 0.0f) { location->flags |= 0x40; *(real *)(location->unknown30 + 0x10) = object->field_144; }
		}
		function_bab40(loop_location_object_index(sound), sound->name, &location->scale);
	}
	else location->scale = sound->scale;
	if (sound->type == 1)
	{
		long marker_name = 0;
		if (sound->marker != NONE) marker_name = function_b8c40(sound->owner, sound->marker);
		s_object_marker marker;
		function_b8d30(sound->owner, marker_name, &marker, 1, false);
		spatial->position = marker.matrix.position;
		spatial->compressed_forward = vector3d_compress(&marker.matrix.forward);
		function_ba1d0(loop_location_object_index(sound), &spatial->velocity, NULL);
		object_get_root_location(loop_location_object_index(sound), cluster_location);
		char audible = 1;
		function_18d4f0(loop_location_object_index(sound), &audible, NULL);
		location->audible = audible;
		if ((*definition & 8) && !location->audible)
		{
			location->audible = 1;
			location->flags |= 1;
		}
	}
	else location->audible = 0;
	if (TEST_FIELD_BIT(sound->flag5)) location->flags |= 0x400;
}
