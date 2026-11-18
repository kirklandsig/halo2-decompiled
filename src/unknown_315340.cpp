// @flags /O2 /GL-
#include "contact_modifiers_315340.h"
#include <string.h>

void *g_480990;
void *g_480770;
void *g_480880;

static __forceinline unsigned long modifier_scale_bits(float const &value)
{
	unsigned long bits;
	memcpy(&bits, &value, sizeof(bits));
	return bits;
}

static __forceinline void install_mass_scales(contact_modifier_view_315340 *contact,
	unsigned long const &a, unsigned long const &b)
{
	// Retail copies integer words through the scalar parameters' home slots.
	// The compiler copies a float read from memory as a word, so nothing goes
	// through x87.
	contact->value.scale_a = *(float const *)&a;
	lane_b_constraint_sizes sizes;
	sizes.total = 0;
	sizes.term_bytes = 0;
	sizes.results = 0;
	contact->scale_b = *(float const *)&b;
	sizes.op_bytes = 0x88;
	contact->owner->add((lane_b_contact_constraint_prefix *)contact, &sizes);
	contact->table = g_480770;
}

// Matched (was 11): both words reach the helper by reference, and the other-body arm
// reads the first scale before the second as the matching arm does (b, then a). The
// two stores of `a` then end both arms alike and merge after the join, in the dead
// manager argument home, and `b` keeps the frame cell.
// @retail 0x315340
void __cdecl function_315340(void *manager, hkEntity *first, hkEntity *second, float scale_first, float scale_second)
{
	contact_modifier_view_315340 *contact = (contact_modifier_view_315340 *)((unsigned char *)manager + 0x10);
	if (contact->table == g_480990)
	{
		unsigned long a, b;
		if (first == contact->body_a)
		{
			a = modifier_scale_bits(scale_first);
			b = modifier_scale_bits(scale_second);
		}
		else
		{
			b = modifier_scale_bits(scale_first);
			a = modifier_scale_bits(scale_second);
		}
		install_mass_scales(contact, a, b);
	}
}

static __forceinline void store_modifier_sign(float *out, float value)
{
	// The selected sign is spilled to its parameter slot, then copied as a word.
	*(unsigned long *)out = *(unsigned long const volatile *)&value;
}

// @retail 0x3153c0
void __cdecl function_3153c0(void *manager, void *body, void const *velocity)
{
	contact_modifier_view_315340 *contact = (contact_modifier_view_315340 *)((unsigned char *)manager + 0x10);
	float sign = 1.0f;
	if (contact->body_a == body) sign = -1.0f;
	if (contact->table == g_480990)
	{
		contact->value.velocity = velocity;
		store_modifier_sign(&contact->scale_b, sign);
		lane_b_constraint_sizes sizes;
		sizes.total = 0;
		sizes.term_bytes = 0;
		sizes.results = 0;
		sizes.op_bytes = 0x28;
		contact->owner->add((lane_b_contact_constraint_prefix *)contact, &sizes);
		contact->table = g_480880;
	}
}

// Apply a velocity projection to the fourth component of two consecutive
// linear solver rows. The input words need not have vector alignment.
// @retail 0x315440
void __cdecl add_velocity_bias_315440(void *, velocity_record_315440 *record)
{
	contact_term_315440 *terms = record->terms;
	__m128 velocity, first, second;
	for (int i = 0; i < 4; ++i) velocity.m128_f32[i] = record->velocity[i];
	for (int i = 0; i < 4; ++i) first.m128_f32[i] = terms[0].linear[i];
	for (int i = 0; i < 4; ++i) second.m128_f32[i] = terms[1].linear[i];
	__m128 a = _mm_mul_ps(first, velocity);
	__m128 ay = _mm_shuffle_ps(a, a, 0x55);
	__m128 az = _mm_shuffle_ps(a, a, 0xaa);
	float first_bias;
	_mm_store_ss(&first_bias, _mm_add_ss(az, _mm_add_ss(ay, a)));
	__m128 b = _mm_mul_ps(second, velocity);
	__m128 by = _mm_shuffle_ps(b, b, 0x55);
	__m128 bz = _mm_shuffle_ps(b, b, 0xaa);
	float second_bias;
	_mm_store_ss(&second_bias, _mm_add_ss(bz, _mm_add_ss(by, b)));
	terms[0].linear[3] += first_bias;
	terms[1].linear[3] += second_bias;
}
