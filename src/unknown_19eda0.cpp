#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "engine_peer.h"

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_19EDA0.CPP: the multiplayer weapon choices (a weapon type names
   one of the multiplayer globals' weapon references, or a random one) and
   weighted random choices from a tag's list */

struct s_tag_reference_view
{
	dword group_tag;
	long index;
};

/* the multiplayer globals' weapon references */
struct s_multiplayer_weapons
{
	s_tag_reference_view references[18];
};

struct s_multiplayer_runtime_globals
{
	byte unknown00[0x6c];
	s_multiplayer_weapons *weapons;
};

struct s_multiplayer_globals_definition
{
	byte unknown00[0xc];
	s_multiplayer_runtime_globals *runtime;
};

/* the weapon of a weapon type: none, a random one other than the excluded
   one, one of the globals' weapons, or the default */
// @retail 0x19eda0
long __stdcall function_19eda0(long default_weapon, char type, long excluded)
{
	long result = default_weapon;
	long index = g_4e034c->index & 0xffff;
	s_multiplayer_runtime_globals *runtime = ((s_multiplayer_globals_definition *)g_4e3b44[index].bytes)->runtime;
	switch (type)
	{
	case 1:
		result = NONE;
		break;
	case 2:
	{
		do
		{
			result = function_19eda0(default_weapon, (char)(random_index(&g_4e7408->unknown0, 16) + 3), excluded);
		} while (result == excluded && excluded != NONE || result == NONE);
		break;
	}
	case 3:
		result = runtime->weapons->references[3].index;
		break;
	case 4:
		result = runtime->weapons->references[0].index;
		break;
	case 5:
		result = runtime->weapons->references[1].index;
		break;
	case 6:
		result = runtime->weapons->references[6].index;
		break;
	case 7:
		result = runtime->weapons->references[7].index;
		break;
	case 8:
		result = runtime->weapons->references[5].index;
		break;
	case 9:
		result = runtime->weapons->references[2].index;
		break;
	case 10:
		result = runtime->weapons->references[4].index;
		break;
	case 11:
		result = runtime->weapons->references[12].index;
		break;
	case 12:
		result = runtime->weapons->references[9].index;
		break;
	case 13:
		result = runtime->weapons->references[10].index;
		break;
	case 14:
		result = runtime->weapons->references[13].index;
		break;
	case 15:
		result = runtime->weapons->references[11].index;
		break;
	case 16:
		result = runtime->weapons->references[15].index;
		break;
	case 17:
		result = runtime->weapons->references[14].index;
		break;
	case 18:
		result = runtime->weapons->references[8].index;
		break;
	case 19:
		result = runtime->weapons->references[16].index;
		break;
	case 20:
		result = runtime->weapons->references[17].index;
		break;
	}
	return result;
}

struct s_weighted_choice
{
	real weight;
	byte unknown04[4];
	long value;
	long extra;
};

struct s_weighted_choice_block
{
	long count;
	s_weighted_choice *choices;
};

// @retail 0x19f130
real weighted_choices_total(s_weighted_choice_block *block)
{
	long count = block->count;
	s_weighted_choice *choices = block->choices;
	real total = 0.0f;

	for (long i = 0; i < count; i++)
		total = choices[i].weight + total;

	return total;
}

// @retail 0x19f1a0
long weighted_choice_random(long tag_index, long *extra)
{
	s_weighted_choice_block *block = (s_weighted_choice_block *)g_4e3b44[tag_index & 0xffff].bytes;
	long count = block->count;
	s_weighted_choice *choices = block->choices;
	long result = NONE;
	real total = weighted_choices_total(block);

	if (count > 1)
	{
		real value = (real)random_index(&g_4e7408->unknown0, (short)(long)total);

		for (long i = 0; i < count; i++)
		{
			s_weighted_choice *choice = &choices[i];

			value -= choice->weight;
			if (value <= 0.0f)
			{
				result = choice->value;
				if (extra)
					*extra = choice->extra;
				break;
			}
		}
	}
	else if (count > 0)
	{
		result = choices[0].value;
		if (extra)
			*extra = choices[0].extra;
	}

	return result;
}

/* the same code as above for the vehicle collections, which the linker folded into 0x19f1a0
   (retail calls it from a separate block in 0x15aad0); keep the two bodies identical */
long weighted_choice_random_15aad0(long tag_index, long *extra)
{
	s_weighted_choice_block *block = (s_weighted_choice_block *)g_4e3b44[tag_index & 0xffff].bytes;
	long count = block->count;
	s_weighted_choice *choices = block->choices;
	long result = NONE;
	real total = weighted_choices_total(block);

	if (count > 1)
	{
		real value = (real)random_index(&g_4e7408->unknown0, (short)(long)total);

		for (long i = 0; i < count; i++)
		{
			s_weighted_choice *choice = &choices[i];

			value -= choice->weight;
			if (value <= 0.0f)
			{
				result = choice->value;
				if (extra)
					*extra = choice->extra;
				break;
			}
		}
	}
	else if (count > 0)
	{
		result = choices[0].value;
		if (extra)
			*extra = choices[0].extra;
	}

	return result;
}


struct s_weapon_rule_choice
{
    long tag_index;
    long unknown04;
};

struct s_weapon_rule
{
    byte flags;
    byte unknown01[3];
    short types[4];
    byte unknown0c[0x40 - 0x0c];
    s_weapon_rule_choice choices[5];
    byte unknown68[0x9c - 0x68];
};

struct s_weapon_rule_globals
{
    byte unknown000[0x128];
    long count;
    s_weapon_rule *rules;
};

struct s_weapon_rule_header
{
    byte unknown00[8];
    byte *object;
};

struct s_effect_owner;
long function_15a090(long type, long count, short const *types);
void __stdcall function_ccff0(long unit_index);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *creation);
bool unit_has_weapon_definition(long unit_index, long definition_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
void __stdcall function_b8540(long object_index);

// @retail 0x19ef40
void __stdcall function_19ef40(long unit_index, long *first_count, long *second_count)
{
    s_weapon_rule_globals *globals = (s_weapon_rule_globals *)g_4e0350;
    for (long i = 0; i < globals->count; i++)
    {
        s_weapon_rule *rule = &globals->rules[i];
        long type = NONE;
        c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
        if (engine)
            type = engine->p0();
        if (function_15a090(type, 4, rule->types) != NONE)
        {
            function_ccff0(unit_index);
            long weapons[2];
            weapons[0] = NONE;
            weapons[1] = NONE;
            for (long j = 0; j < 5; j++)
            {
                long tag_index = rule->choices[j].tag_index;
                if (tag_index != NONE)
                {
                    long extra;
                    long weapon = weighted_choice_random(tag_index, &extra);
                    if (weapon != NONE)
                    {
                        if (weapons[0] == NONE)
                            weapons[0] = weapon;
                        else if (weapon != weapons[0])
                        {
                            weapons[1] = weapon;
                            break;
                        }
                    }
                }
            }
            if (*((byte *)g_4e6948 + 0x212) == 2)
            {
                weapons[1] = function_19eda0(weapons[1], *((char *)g_4e6948 + 0x213), NONE);
                weapons[0] = function_19eda0(weapons[0], *((char *)g_4e6948 + 0x212), weapons[1]);
            }
            else
            {
                weapons[0] = function_19eda0(weapons[0], *((char *)g_4e6948 + 0x212), NONE);
                weapons[1] = function_19eda0(weapons[1], *((char *)g_4e6948 + 0x213), weapons[0]);
            }
            for (long k = 0; k < 2; k++)
            {
                if (weapons[k] != NONE)
                {
                    byte creation[0xc4];
                    function_b7930(creation, weapons[k], NONE, NULL);
                    *(long *)(creation + 0x0c) = 0;
                    long weapon_index = function_b7b40(creation);
                    if (weapon_index != NONE)
                    {
                        byte *weapon = ((s_weapon_rule_header *)g_4e0300->data)[weapon_index & 0xffff].object;
                        if (!unit_has_weapon_definition(unit_index, *(long *)weapon))
                            function_cd0c0(unit_index, weapon_index, 1);
                        else
                            function_b8540(weapon_index);
                    }
                }
            }
            if (rule->flags & 1)
            {
                *first_count = 0;
                *second_count = 0;
            }
            if (rule->flags & 2)
            {
                *second_count += *first_count;
                *first_count = 0;
            }
            break;
        }
    }
}

#include <xtl.h>
#include <math.h>
#include "effects.h"
#include "flexible_surface_calls.h"
extern byte *g_485a80;
void function_1ccb0(long format);
void function_1cd90();
void function_1cf50();

class c_polygon_material_reference_19f
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void slot4() = 0;
    virtual void slot5() = 0;
    virtual void slot6() = 0;
    virtual void slot7() = 0;
    virtual void slot8() = 0;
    virtual void slot9() = 0;
    virtual byte *reference() = 0;
};

PRIVATE inline byte *polygon_tag_19f(long index)
{
    return g_4e3b44[index & 0xffff].bytes;
}

PRIVATE inline byte *polygon_pointer_19f(byte *base, long offset)
{
    return *(byte **)(base + offset);
}

PRIVATE inline real polygon_distance_19f(real x, real y)
{
    return (real)sqrt(x * x + y * y);
}

PRIVATE __forceinline long polygon_round_19f(real value);
PRIVATE inline long polygon_round_19f(real value)
{
    long result;
    __asm
    {
        fld value
        fistp result
    }
    return result;
}

// MiniMax R (11-01): no new form, no lower score (the retail-shaped fade gives 390, the double
// comparison 316; the call to 0x4b2d0 keeps three stack arguments where retail's takes two).
// score 316 (was 379): the distance to the centre is now computed and compared in double, so the
// the distance to the centre is now computed and compared in double, so the
// float copy lands in the dead radius slot and the fade block keeps retail's x87 shape; the fade
// bound is still one product where retail has two and the frame is 0x34 versus 0x30.
// Earlier at 379 (was 379): frame 0x34 versus 0x30; fade/radius homes and render-call transport differ.
// the distance to the centre is now computed and compared in double, so the
// Earlier 379: frame 0x34 versus 0x30; fade/radius homes and render-call transport differ.
// Earlier at 393: the fade clamp now computes a single radius*1.5 product, so the
// radius keeps retail's xmm3; our fade block is still one instruction shorter than
// retail's, and the call to function_4b2d0 still passes three stack arguments.
// Round 194: Named real radial denominator at division/loop boundary, with ternary/explicit
// alpha bounds and long/dword UV ordinals, all 366; without the denominator local 416.
// Ordinary 366 loses both callers at two differences each; all trial forms restored.
// Round 184: native attribute header with captured/live signed count and short/word
// elements: actual cursor 379 each; direct array indexing 374 each (five-point
// gain rejected). Frame 0x34 versus 0x30 and protected call transport remain.
// Round 226: ordinary variant/body pointer advance with native material/group/shader records,
// every pair/all three, and all plus native context byte 432 each at 1480/1518 bytes.
// Both matched callers become 2; every record, input observation and original body restored.
// Round 32: BLOCKED-BY 0x4b2d0 (in a contributor's file): retail passes it the variant in EAX, the
// group in ECX and the tag in EDX with two pushes; ours pushes three. Retail also parks the fade
// denominator and alpha in dead argument slots (0x5c/0x58), which frees 0x14 for the colour
// temporaries (frame 0x30). The fade bound as two products 402 (retail's fst and two mulss, but
// xmm numbering rotates); a plain address read of the variant 423 and one volatile read 422 (both
// frame 0x30, but the two matched callers lose); a named fade width before the loop 374 (our
// compiler then multiplies by a reciprocal where retail divides; not kept); a vertex pointer 397.
// Round 66: existing double-input distance helper at entry, entry+vertex and all three sites
// 379/379/399; pre-loop counter, entry counter, entry wrapped index, entry predecessor and
// a plain while loop 379 each. Both callers stayed matched; all executable forms restored.
// Round 409: one emitted-position workspace 403; native tint copy 393; both 416; current-vertex
// address 379; with position/tint/both 403/393/416; all with ordinary variant 466 and both
// matched callers lose. Original complete body and output/parameter homes restored.
// Round 272: ordinary variant and load-stepped cursor with entry declarations for
// fade limit/spread/outer distance, every subset and complete role names
// 416/416/416/416/416/416/416/416. Original parameter home, observations and body restored.
// Round 415: ordinary variant/body cursor step with native current/previous vertices and
// top-first declarations, seven subsets 429/416/396/432/429/416/396; all with CRT input copies 396.
// Both matched callers lose two in every trial; complete executable body restored.
// Round 132 (lane 6, fast): the variant parameter read directly, without the volatile reference,
// 401 (both matched callers lose, 2 each); read once through a plain const address 400 (frame 0x30,
// aligned 128 against 157, but pass moves to EDI and the callers lose, 8 and 11); the double distance
// with `1.5f * radius` written at both uses 391; the attribute scan indexed as `types[i]` 395; the
// material default as an if/else 316 (same code). Retail's colour temporary reuses the spread slot
// (0x14) because its radial denominator sits in the dead perimeter slot; ours keeps a reciprocal in
// the spread slot through the loop, which is the extra frame dword.
// Round 122 (fast): retail also inlines 0x1ccb0(59)/0x1cd90 here (both in an /Ob1 contributor file; inline
// declarations here 379). Ours keeps &vertices[index].y in EBP, so the step and fade denominator lose retail's EBP and
// perimeter slot. uv loop indexing the block array each pass 374 (count in the test 374); fade if/else 436; inner
// radius at its use 430; on the uv form: bottom by fields 374, bottom right after the index 375 (step in EBP,
// pointer spilled instead), edge with one cast 374, top from the vertex 377, top.z assigned 375.
// Round 42 (lane 14, fast; "aligned" = differing lines after a sequence alignment, base 157): retail's 1.5 is
// not a literal but a .data float at 0x47ff94 (between g_47ff90 and g_47ff98, read only here). With
// `real g_47ff94 = 1.5f;`, the distance as `real distance = (real)sqrt(...) - radius;` and the bound written
// as `g_47ff94 * radius` in the clamp and the divisor, the fade block is retail's (the global loaded once,
// fst into the dead radius slot, the second product in place) but for the xmm numbering and the clamp's
// arm order: 384 (aligned 144, 1501 bytes); with the radial denominator written in the division,
// `/ (maximum_distance - spread)`, our compiler divides by the hoisted value as retail does (a named
// denominator, in the loop or before it, becomes a reciprocal multiply): 389 (aligned 136, 1495 bytes).
// Neither is kept: the base's longer x87 fade block happens to offset the missing inline of 0x1ccb0(59)
// (retail inlines it; BLOCKED-BY 0x1ccb0 in a contributor's /Ob1 file), so every shorter fade block
// raises the positional count. Also: the float distance with one bound local 374 (1501 bytes); with
// `1.5f * radius` written twice 388, as `radius * 1.5f` 388 (same code: the literal is commuted);
// the global with the original double distance 390; the inline denominator on the base 370 (aligned
// 155); the alpha divided in double, on the base/on the global form 399/350 (hoisted in double);
// diagnostic float-consistency pragma 500 (1605 bytes). Next: keep the global, float distance and
// inline denominator once 0x1ccb0 inlines here.
// Round 214: mixed-precision real result before tests 421; zero-first result 395;
// separate distance/factor 316, with zero-first 395; upper-first result 316;
// factor expression 316; explicit distance arms then factor 316; scalar role names 316.
// @retail 0x19f680
void function_19f680(long tag, long group, long pass, long variant, void *context,
    point2f const *vertices, long count, point3f const *center, real radius,
    real perimeter, point3f const *color, real height)
{
    long const volatile *variant_reference = &variant;
    long variant_value = *variant_reference;
    real inner_radius = radius - 0.0001f;
    double delta_x = (double)center->x - (double)g_4b9da0.x;
    double delta_y = (double)center->y - (double)g_4b9da0.y;
    double distance = sqrt(delta_x * delta_x + delta_y * delta_y) - radius;
    real fade_limit = 1.5f * radius;
    real fade = distance < 0.0 ? 0.0f : ((real)distance > fade_limit ? fade_limit : (real)distance);
    fade = 1.0f - fade / fade_limit;
    if (fade > 0.0f)
    {
        real spread = fade * radius * 2.0f;
        real maximum_distance = inner_radius + spread;
        function_1bd50(0);
        function_1cdd0(0, 0);
        function_4b2d0(tag, group, (*variant_reference), ((byte *)context)[4], pass);
        long material = tag == NONE ? *(long *)(g_485a80 + 0xd0) : tag;
        byte *reference;
        dword kind = *(dword *)&g_4e3b44[(short)material];
        if (kind == 0x5052544d || kind == 0x70727433)
            reference = ((c_polygon_material_reference_19f *)function_137bd0(material))->reference();
        else
            reference = polygon_pointer_19f(polygon_tag_19f(material), 0x24);
        byte *groups = polygon_pointer_19f(polygon_tag_19f(*(long *)reference), 0x5c);
        long group_offset = *(word *)(polygon_pointer_19f(groups, 4) + group * 10) & 0x1ff;
        long variant_offset = *(word *)(polygon_pointer_19f(groups, 0xc) + (group_offset + variant_value) * 2) & 0x1ff;
        long shader = *(long *)(polygon_pointer_19f(groups, 0x14) + (variant_offset + pass) * 10 + 4);
        byte *attributes = polygon_pointer_19f(polygon_tag_19f(*(long *)(polygon_pointer_19f(
            polygon_pointer_19f(polygon_tag_19f(shader), 0x20), 4) + 0x100)), 8);
        long attribute_count = *(long *)(attributes + 4);
        short *types = *(short **)(attributes + 8);
        long uv = NONE;
        for (long i = 0; i < attribute_count; ++i, ++types)
        {
            if (*types == 3)
            {
                uv = i;
                break;
            }
        }
        DWORD old_cull;
        D3DDevice_GetRenderState(D3DRS_CULLMODE, &old_cull);
        function_1ccb0(59);
        function_1cd90();
        function_1cf50();
        D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
        real repetitions = perimeter / (height * 1.2f);
        long rounded = polygon_round_19f(repetitions);
        real texture_scale = (real)(rounded < 2 ? 2 : polygon_round_19f(repetitions)) / perimeter;
        long period = g_510c54->field_2_3 * 2;
        real texture_position = (real)(g_510c54->game_time % period) / (real)period;
        for (long step = 0; step <= count; ++step)
        {
            long index = step == count ? 0 : step;
            double vertex_x = (double)vertices[index].x - (double)g_4b9da0.x;
            double vertex_y = (double)vertices[index].y - (double)g_4b9da0.y;
            real radial_denominator = maximum_distance - spread;
            real alpha = (maximum_distance - (real)sqrt(vertex_x * vertex_x + vertex_y * vertex_y)) /
                radial_denominator;
            if (alpha < 0.0f)
                alpha = 0.0f;
            else if (alpha > 1.0f)
                alpha = 1.0f;
            point3f bottom = { vertices[index].x, vertices[index].y, center->z };
            point3f top = bottom;
            top.z += height;
            D3DDevice_SetVertexData4f(11, color->x, color->y, color->z, alpha);
            if (uv != NONE)
                D3DDevice_SetVertexData2f(uv, texture_position, 1.0f);
            D3DDevice_SetVertexData4f(0, bottom.x, bottom.y, bottom.z, 1.0f);
            if (uv != NONE)
                D3DDevice_SetVertexData2f(uv, texture_position, 0.0f);
            D3DDevice_SetVertexData4f(0, top.x, top.y, top.z, 1.0f);
            long previous = index != 0 ? step - 1 : count - 1;
            double edge_x = (double)vertices[previous].x - (double)vertices[index].x;
            double edge_y = (double)vertices[previous].y - (double)vertices[index].y;
            texture_position = texture_position - (real)sqrt(edge_x * edge_x + edge_y * edge_y) * texture_scale;
        }
        D3DDevice_End();
        D3DDevice_SetRenderState(D3DRS_CULLMODE, old_cull);
    }
}

