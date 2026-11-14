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
					s_sound_object_view *machine = sound_object_header(object_index)->object;
					if (machine->machine_index != NONE)
					{
						s_local_camera *camera = function_125360();
						if (camera && camera->active)
						{
							short camera_index = camera->index;
							if (camera_index != NONE)
							{
								s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
								long audibility_index = function_249e20(bsp, machine->machine_index);
								if (audibility_index != NONE)
								{
									long row = function_249e60(camera_index, bsp, audibility_index);
									if (row != NONE)
									{
										object_location.cluster_index = bsp->sound_clusters[row + 18 * bsp->cluster_map->indices[(word)machine->machine_index]];
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

// score 2 (was 2): only the two loads' registers differ: ours loads the samples address into EDI (the data
// pointer's dying register) and the entry's data offset into EDX, retail the samples into EDX and the offset into EDI.
// MiniMax E (11-09): 2 (was 7): the samples pointer is copied into a dword and the scale is passed
// sample-base/offset registers and final object setup remain;
// MiniMax E (11-09): score 2 (was 7): the samples pointer is copied into a dword and the scale is passed
// straight to the callee, so the scale is loaded early and the sum's first operand is the samples pointer,
// which matches retail's load order and its call setup (the curves address by lea ecx and the object index
// loaded after the pushes); what is left is the two loads' destination registers, ours putting the samples
// pointer in EDI (the data pointer's own register) and the entry's data offset in EDX, where retail puts
// the samples pointer in EDX and the offset in the dying EDI.
// MiniMax I (11-08): no new form, no lower score (7); the sum spelled with the samples pointer first gives the same bytes.
// MiniMax R (11-06/07): the samples' memcpy form (2; H 11-04, C 11-06) again, not kept; a memcpy copy of the entry's data offset 3
// (also a field's bits read through its address), every plain spelling of the sum 8, the offset through the
// array expression 23, a short cast of the offset 17.
// MiniMax C (11-06): no new form, no lower score; copying the samples' address into a dword with memcpy
// and passing the scale straight to the callee scores 2, and copying the entry's data offset instead scores 3,
// but both read a field's bits through its address and are not kept; passing the scale straight to the callee
// gives retail's lea ecx and its late object index but loads the scale last (8), and the sum still loads the
// entry offset into EDX as the add's destination in every spelling (the source order, the offset cast to
// unsigned, int or size_t, the samples as an integer, the offset as a pointer and both as dwords all give the
// same bytes); keeping the scale's volatile read and passing the scale as well loads the scale twice; a union
// overlay of the samples' pointer and its address, the index form (&samples[data_offset]) and the sum with
// both operands cast to size_t all score 8, so the original body is restored.
// MiniMax H (11-04): no new form, no lower score; the tail still loads the entry offset into EDX and the samples into ESI where retail loads the samples into EDX and the offset into the dying EDI, and the object index still loads into ECX before the pushes; every spelling of the sum gives the same bytes (7).
// MiniMax D (11-01): no new form, no lower score; the sample sum still puts the entry offset in EDX and the
// samples in ESI where retail has the samples in EDX and the offset in the dying EDI, the object index still
// loads before the pushes, and the permuter's 1806 tries never beat 7.
// Earlier 7 (was 7): the tail still loads the entry offset into EDX and the samples pointer into ESI, where
// Earlier (7, was 7): the tail still loads the entry offset into EDX and the samples pointer into ESI, where
// retail loads the samples into EDX and the offset into the dying EDI, and the object index is still loaded
// into ECX before the pushes where retail loads it after them, with the curves address computed by lea eax
// where retail uses lea ecx. This round: the front end canonicalises the sum, so every spelling gives the
// same two loads with the offset first and as the add's destination: the pointer first, both operands as
// dwords in either order, a dword-typed view of the promotion data, the offset cast to unsigned, int or
// size_t, the sample pointer in a local before or inside the block and the offset hoisted before the block
// all give the same bytes (7), as do the sum through an address local, the pointer local with += and the
// index form; reading the scale's bits without the volatile pointer gives the same bytes
// (7), and passing the scale itself gives retail's lea ecx and its late object index but loads the scale
// last (8). A named local for the curve set's address gives the same bytes (7). The permuter ran past its
// 900-second budget on a loaded machine and was stopped by the harness's background limit; it wrote nothing.
// MiniMax E (11-05): no new form, no lower score; the seven differences stay the sample base against the
// entry's data offset, the object index load and the curve pointer setup; passing the scale directly gives 8.
// MiniMax S (11-09): the 2-diff memcpy form still differs only in the two loads' registers (retail keeps the
// sample base in EDX and the offset in EDI, ours the other way round); a round trip of the base through a dword
// folds back to the pointer form at 7, and the index form, a pointer local and an unsigned index leave the offset
// in the add's destination, at 7 or 8.
// Earlier 2 (was 7): sample-base and offset registers plus object call setup remain; readonly data
// Earlier (7, was 7): sample-base and offset registers plus object call setup remain; readonly data
// Earlier (2, was 7): sample-base and offset registers plus object call setup remain; readonly data
// input 7; readonly entry input 7; native count identity 7; output count lifetime 52; existing
// scale storage lifetime 7; input pointer home names 7; existing scale home names 7; inline matched
// output callee 7; final object address read 7; split construction around count 9; split
// construction count last 7; actual descriptor type 7; nested query guard 7; object snapshot 7;
// mutable permutation input 7; scalar scale copy 8; scalar copy interleaved 11; plain scale
// pointer 7; scalar copy split count last 8; direct scale bits 7; union real assignment 8;
// real assignment after stores 8; scalar copy declared first 8; late object snapshot 7.
// native data/entry/both record copies 56/58/74; direct entry/data/both fields 22/23/65;
// native admission predicate 7; nested availability predicate 7.
// MiniMax H (11-04): the curve address built from the samples' address copied into a dword with memcpy, plus
// the entry's data offset, with the scale passed straight to the callee: 7 -> 2. NOT kept: the memcpy reads the
// field's bits through its address (kept only with a byte match); without it the same body is 8.
// T80: Count-presence/missing results at the original comparison, bool/byte/short/long:
// 7/7/7/7/7/7/7/7. Sample-base/offset register reuse and final object setup remain.
// NOT kept: no byte equality or qualifying gain; original complete body restored.
// MiniMax R (11-09): no new form, no lower score (7): the curve set's address is built from the
// entry's data offset plus the samples' address with the operands in the other order here (ours adds
// the samples to the offset, retail the offset to the samples), the curve pointer is computed in eax
// here and in ecx in retail, and the scale is loaded before the pointer's push in retail and after it
// here; the memcpy form that reads the field's bits through its address is the only one that reaches
// 2 and it is kept out of this lane's changes.
// Lane 9 r99 (fast): a file-local helper returning a tag data's address plus an offset, over the promotion data's head
// read as that tag data (diagnostic cast), 8, with the count stored first 10, the helper indexing `&address[offset]` 8;
// the count stored before the address 4; on the memcpy body the offset in a named local before or after the copy 2/2,
// the sum as an integer sum either way round 2/2, the offset written first 2 (all the same code); `samples_address +=
// offset` then the cast 8; the copy made straight into the curve set's address field, then `+=` 8.
// Round 156 (m3 lane 8, fast): 1-16 unused initialised locals at the top, 1-8 between data and entry, 1-8
// before the copy all 2 (no numbering tie: VC ranks the add's operands by tree depth); a promotion pointer
// with `promotion->data->samples + offset` 22 (the samples become the add's destination, as retail, but
// the plain-sum schedule returns), with the copy from that read 23; the samples through the full promotion
// expression 25; the count stored first on this body 4, on the plain sum 10; an inline helper filling the
// set through a pointer 8 (count first 10), returning it by value 10/10; both reads copied with memcpy 9/9.
// Round 77 (m3 lane 15, fast with every function in this file): the copy into a `byte const *` local then `samples +
// offset` 2, `&samples[offset]` 2, the dword indexed as `&((byte const *)samples_address)[offset]` 2, the entry as
// `data->entries + entry_index` 2, no count local (the count read at the test and the store) 2, all the same code;
// re-measured: the offset read into a local at the top 2 (same code), the offset through `data->entries[...]` at the
// sum 23. The permuter on this memcpy body (600 s, anneal): 68 tries, best 2 on both images.
// Round 160 (m3 lane 8, fast): in retail both reads are plain temporaries of one sum evaluated while the scale sits in
// EAX: the samples go to EDX (the destination) and the offset to EDI, free once the samples are read. The curve set
// built as a temporary object in the call's argument list 8 (the plain sum's code: the sum is still evaluated ahead
// of the scale); the plain sum with 1-3 unused locals 8 each (no count effect); the entry's data offset copied with
// memcpy instead of the samples 3 (the samples become the EDX destination as in retail, but the offset is read first,
// into ESI in place), with a samples local before or after it 3/3, with the count read again from the entry 3; the
// address field assigned the samples and then advanced by the offset 8, by the memcpy'd offset 3 (also with the samples
// copied straight into the field); the set as an aggregate initializer 10 (the count stored first frees EBX for the
// second read; with a samples local or the index form 10/10); the field assigned, the count stored, then the field
// advanced 11, with the memcpy'd offset 10.
// Round 195 (m3 lane 4, fast): the curve set as the existing two-field tag-data aggregate (size, address) 2; the object
// test nested in its own if 2; the curve address through an inline helper taking the data pointer and the offset 8 (the
// plain sum's schedule: the add ahead of the scale load); the curve set and the copy declared at function scope 2; the
// guard as an early return 2 (all the 2s the same code).
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
		dword samples_address;

		memcpy(&samples_address, &data->samples, sizeof(samples_address));
		curves.data = (byte const *)samples_address + entry->data_offset;
		curves.count = count;
		function_10e480(object_index, tag_index, (s_vibration_curve_set *)&curves, scale);
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

// score 184 (was 246; earlier 264, 279, 280, 293): the top field as a value ternary (round 170, plain). HELD (needs a
// ruling): `inline` on 0x188790's definition, 113 alone and 49 with the ternary (build/r170/held-0x18cbc0.patch in the m3-lane6
// worktree); round 170's line at the end of this note has the forms.
// MiniMax E (11-10): no new form, no lower score (246); the literal 1 is still pooled in EBX, so the frame stays 0x84 against retail's 0x80 (a byte flag field 324, word bitfields 246 and the flag-0 bitfield test 324 all score the same or worse).
// MiniMax E (11-06), at 246 (was 264): the type cases now run 0, 2, 3 as retail does, with the type test
// falling through to case 3; case 3 keeps its own failure byte and calls the position routine only when
// flag1 is clear; the audibility byte is declared at the audibility call. Not kept (coordinator): case 0's listener
// as a one-use local read before the 0x1000 flag store (244; 246 without it). Still different: our frame is 0x84 against retail's 0x80, so every stack home is
// off by four; retail merges the audibility byte into the 4-bit field where ours assigns it; retail's clear
// path sets the flag byte with `or byte ptr [edi], 2` where ours folds it into a word store.
// MiniMax C (11-04): no new form, no lower score: the flag-0 mask is the same literal 1 as the type tests and the shift, so the compiler hoists it into EBX and the frame stays 0x84 (a unique mask value folds to `test byte ptr [esi+4], imm`); a byte flags field with byte or word bitfield views, the mask spelled byte, unsigned, long, short, true or character, `== 0`, `!= 0` and `% 2`, the type test as a switch, and the audibility form that reproduces retail's neg/sbb/inc store all score 265 or worse.
// Earlier 264 (was 264): frame 0x84 versus 0x80: our build keeps the constant 1 in EBX from the datum flag test
// (`test bl, al`, the type tests, the shift, b8d30's `push ebx`) and leaves a dead store of it in a stack slot,
// so the object pointer goes to ECX; retail tests with immediates and loads `mov bl, 1` only after the b8d30
// call. Retail also stores case 3's failure byte (`neg/sbb/inc`, slot shared with the audibility output) and
// sets the case-2 clear flag with `or byte ptr [edi], 2` where ours folds it to `mov word ptr [edi], 2`.
// Earlier (score 279, was 279): frame 0x8c versus 0x80; datum/tag homes, constant-one spill and output-address formation differ.
// Earlier: frame 0x8c versus 0x80; datum/tag homes and root-output address formation differ.
// Round 30 (lane D): assigning the root-location result to the destination, and keeping it in a local
// instead, both score 279; a local copy of the sound type for the type-1 test scores 289, the flag test
// through the flag0 bitfield and the type mask shifted as an unsigned 1 both score 279 with the same
// code, and the mask shifted the other way scores 285. A build without the type shift has frame 0x88,
// so the extra dword is the spill of the literal 1 that the flag test, the type-1 test and the shift
// share. What still differs: the frame stays 0x8c against retail's 0x80 and the datum/tag homes differ.
// Original body kept.
// Earlier: frame 0x8c versus 0x80; datum/tag homes and root-output address formation differ.
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
// Round 267: all five other flags/owner/byte entry-publication orders and three native shared-zero
// chains each score 279. Original initial stores restored; frame 0x8c, datum/tag homes and one spill remain.
// Round 38 (lane 15, fast; aligned = LCS-aligned differing instructions, 207 at 279): with the
// root-location callee matching, the spatial/cluster addresses written at each use (no pointer locals)
// give retail's ESI datum and EBP definition at retail's 1029 bytes but frame 0x84, 288 (aligned 236; the
// extra dword is the constant 1 kept in EBX from the flag test and spilled, where retail keeps the object
// in EBX); on that form: the object type read into a local 288; the flag test through TEST_FIELD_BIT 288;
// the object pointer read before the type test 307 (frame 0x80 as retail, but case 2 is then laid out
// before case 3 and the case-3 failure byte is no longer stored). The object pointer read early on the
// original body 309. Original body restored.
// Round 26 (lane 17): spatial/cluster addresses written at each use (no pointer locals) 288 alone (retail's ESI
// datum and EBP definition, register-blind differences 91 -> 82), with the audibility output declared at the top
// of the marker block 264 (kept; same structure, its store now precedes the marker lookup). On that: forward
// vector at function scope 288 (before the audibility move); the clear's flag through the flag1 bitfield 264
// (same code); one function-scope char for the case-3 failure and the audibility 264 (no store); the audibility
// move on the pointer-local body 293. Diagnostics: flag test on another bit 290, the type test's object index
// without its type compare 331 (frame 0x80), the shift on another constant 286: the early EBX constant stays.
// On the kept form: audibility declared just before the marker 267; marker declared before its name 264; case 3's
// forward vector declared before the audible store 264.
// Y499: retained direct-output/audibility graph, failure-first/common exit/both
// 261/260/261; reversed membership mask 257; with failure/common/both 263/264/263;
// all with byte failure 263. Seven-point best is below the keep bar; original body restored.
// Frame 0x84/0x80 and early constant-one spill remain.
// M305: Two-use negative adjustment 265; with reversed membership 258 (six-point lead, restored);
// late-tag/two-distance interactions 307/337/345/303/328/336.
// AC545: Owning raw byte with signed-byte/byte/signed-word/word bitfields
// 311/311/311/264; raw word with the first three 311/311/311; byte owning
// declaration plus direct bit-zero admission 311. NOT kept: no equality or
// gain; original datum layout and body restored, native one-spill/failure byte remain.
// Round 154 (m3 lane 6, fast with 0x188790 exact): `inline` on 0x188790's definition gives retail's 0x80 frame, xmm0
// re-zeroed after the calls and the object in EBX but scores 301; on it case 3's failure as `bool failed = true` set
// from the call only when flag1 is clear 162, audible declared right before 0x18d4f0 134, the object index named
// before it 125, the top field as `sound->type == 0 ? sound->owner : g_444ae0` 91, cases in order 0, 2, 3 40, the
// flags cleared by memset 22, a long listener named ahead of the 0x1000 flag 20 (aligned 15). Without the `inline`
// and the match-only forms (ternary, case order, memset, audible placement kept) 287. NOT kept: the 20 rests on a
// compiler hint on another function and on match-only forms; the chain is held as build/held/l6-1106-18cbc0-inline-chain.patch.
// AC555: Effect-provider char/byte/signed-char returns with original Boolean
// and native failure receivers, eight forms 264 each; provider stays exact. NOT
// kept: no gain; original declarations/body restored; one-spill/failure byte remain.
// Y604: Existing clear pointer/reference input and complete location result,
// ignored/rebound at both actual calls: 264/264/264/264/264/264/264/264.
// NOT kept: no equality; complete clear ownership leaves the early constant-one
// spill, frame and failure-byte differences. Original helper/body restored.
// N396: Selected owner-word memcpy/memmove through conditional/branch/selected-source/negative
// sources: 264/307/280/324/264/307/267/310. NOT kept: no equality or gain; original owner transfer
// and body restored, constant-one spill and native failure-byte store remain.
// b5F (11-06) statement walk against retail 0x18cbc0-0x18cfc4, no logic or precision difference found: every field, offset, test
// direction, callee and argument matches (flag0 0x18cdab, type 1 owner/NONE selects 0x18cdb5/0x18cecd, object-type mask 0x400
// 0x18cddf, field tests 0x134/0x138/0x13c/0x140 are > 0 and 0x144 is < 0 at 0x18ce14-0x18ce86, comiss only, no rounding steps;
// case 0 cluster/leaf/bsp stores 0x18cd88-0x18cda7; case 2 impact stride 0xa0 at 0x18cd2f; case 3 effect stride 0x190 and velocity at +0x30
// at 0x18cc83; audible merge 0x18cf7b-0x18cf9f; flag5 -> 0x400 at 0x18cfac). Spelled the case-0 location stores through
// s_location's leaf_index/cluster_index/bsp_index (same bytes, 246) and dropped the (short) cast on the cluster store (246).
// Tried: flags |= 0x1000 or unknown0c |= 1 in place of the byte-wide or 249 (word or); the case-3 failure as a block-local
// without the true default (call path only, size 1029 as retail) 285 (loses the failure byte retail stores at 0x18cc60 on the
// call path only); cluster as short 250. Not matched: frame 0x84/0x80, the constant 1 in EBX, case 2's `or byte [edi], 2` (ours folds to a word store).
// Round 170 (m3 lane 6; fast unless marked, 0x188790 and 0x21db80 MATCH in every row; al/m = aligned / register-masked
// aligned counts). Kept: the top field as `sound->type == 0 ? sound->owner : g_444ae0` 184 (plain, 1034 B, al 82, m 48; it
// no longer takes the address of the shared const g_444ae0). Held: `inline` on 0x188790's definition 113 (0x188790 stays out
// of line and exact; ours then re-zeroes xmm0 after its calls and keeps the object in EBX as retail, frame 0x80), with the
// ternary 49 (plain, 1031 B). On the held 49, round 154's match-only chain: the object index named before audible 40, flags
// cleared by memset 22 (1029 B), the listener named ahead of the 0x10 flag store 20 (al 15, m 9). Left at 20: retail selects
// the top value through a pointer (`lea eax, [esi + 0x10]; ...; mov eax, 0x444ae0; mov eax, [eax]`, main's earlier spelling),
// and its marker position copy runs edx/eax/ecx where ours runs eax/ecx/edx. On the 20 chain: main's pointer select 98 (head
// exact as retail's, but the load lands in EDX where retail reuses EAX and every scratch register after it is one step off;
// al 171, m 13), without the object index 107, memset only 109; the select written `sound->type != 0 ? g_444ae0 :
// sound->owner` 305 (al 14, m 8: everything but those two spots agrees, one instruction short), `*(sound->type != 0 ?
// &g_444ae0 : &sound->owner)` 101, as an if/else 317; the type read once into a char for the select and the switch 157;
// the datum as pointer arithmetic 98 (same code); the three type-0 ternaries through small inline accessors 20/98 (same
// code as the written ternaries). On the kept 184 (no inline): the object index named before audible 184 (9 lines differ),
// memset 177, the listener named 182, all three 175; the select with swapped arms 307; a char type for the select and the
// switch 302. On the 175 chain: the marker name as a ternary 174 (alone on 184: 183), the 0xbab40 call through the object
// block's index 199 (1005 B); on 174: the audibility test as `!location->audible && (*definition & 8)` 173; on 173: the
// 0x400 flag as `|= ... ? 0x400 : 0` 185 (186 on 174), case 2's `audible = 1` after its if/else 373 (0x21db80 lost),
// the 0xba1d0 call through a named object index 183 (1020 B), case 3's failure test with the clear first 218. Same code:
// a char failure flag, `marker` or case 3's forward vector at function scope, braces on the last else, case 0 as an
// early break, case 0's 0x10 flag as a word or.
// MiniMax B (11-11): score 184 (was 246): the location's second dword now takes the owner's value instead of
// the address of the owner field, which keeps the address out of the load; still different: the frame is 0x84
// against retail's 0x80, the literal 1 is still pooled in EBX (retail tests with immediates and materialises it
// only for the shift), and the audibility byte and the flag stores still take other registers.
// @retail 0x18cbc0
void function_18cbc0(long looping_sound_index, s_type_99c531 *location)
{
	s_loop_location_datum *sound = &((s_loop_location_datum *)g_4ed28c->data)[looping_sound_index & 0xffff];
	byte *definition = g_4e3b44[sound->tag & 0xffff].bytes;
	location->flags = 0;
	location->unknown08 = sound->type == 0 ? sound->owner : g_444ae0;
	location->unknown02 = 0;
	switch (sound->type)
	{
	case 0:
	{
		long cluster = sound->type == 0 ? sound->cluster : NONE;
		if (cluster != NONE)
		{
			location->spatial.location.cluster_index = cluster;
			location->spatial.location.leaf_index = NONE;
			location->spatial.location.bsp_index = sound->type == 0 ? sound->listener : NONE;
			((byte *)&location->flags)[1] |= 0x10;
		}
		break;
	}
	case 2:
		location->audible = 1;
		if (!TEST_FIELD_BIT(sound->flag1))
			impact_sound_location_get((s_impact *)((byte *)g_51ebfc->data + (sound->owner & 0xffff) * 0xa0),
				(s_impact_sound_location *)&location->spatial, &location->scale);
		else loop_location_clear(location);
		return;
	case 3:
		{
			location->audible = 1;
			vector3f forward;
			bool failed = true;
			if (!TEST_FIELD_BIT(sound->flag1))
				failed = !function_17b2d0(sound->owner, &location->spatial.position, &forward, &location->scale);
			if (!failed)
			{
				location->spatial.compressed_forward = vector3d_compress(&forward);
				location->spatial.velocity = *(vector3f *)((byte *)g_4ea93c->data + (sound->owner & 0xffff) * 0x190 + 0x30);
				function_11bed0(&location->spatial.location, &location->spatial.position);
			}
			else loop_location_clear(location);
			return;
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
		location->spatial.position = marker.matrix.position;
		location->spatial.compressed_forward = vector3d_compress(&marker.matrix.forward);
		function_ba1d0(loop_location_object_index(sound), &location->spatial.velocity, NULL);
		object_get_root_location(loop_location_object_index(sound), &location->spatial.location);
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
