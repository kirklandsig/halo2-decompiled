#include "unknown_11c920.h"
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

		s_weighted_choice *choice = choices;

		for (long i = 0; i < count; i++, choice++)
		{
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
long function_15a090(short const *types, long type, volatile long count);
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
    long i = 0;
    if (globals->count > 0)
    {
        s_weapon_rule *rule = globals->rules;
        do
        {
            c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];
            long type = NONE;
            if (engine)
                type = engine->p0();
            if (function_15a090(rule->types, type, 4) == NONE)
            {
                rule++;
                i++;
                continue;
            }
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
            byte first_type = *((byte *)g_4e6948 + 0x212);
            if (first_type == 2)
            {
                weapons[1] = function_19eda0(weapons[1], *((char *)g_4e6948 + 0x213), NONE);
                weapons[0] = function_19eda0(weapons[0], *((char *)g_4e6948 + 0x212), weapons[1]);
            }
            else
            {
                weapons[0] = function_19eda0(weapons[0], (char)first_type, NONE);
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
        while (i < globals->count);
    }
}

#include <xtl.h>
#include <math.h>
#include "effects.h"
#include "flexible_surface_calls.h"
extern point3f g_4b9da0;
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

// @retail 0x19f680
void function_19f680(long tag, long group, long pass, long variant, void *context,
    point2f const *vertices, long count, point3f const *center, real radius,
    real perimeter, point3f const *color, real height)
{
    long const volatile *variant_reference = &variant;
    double delta_x = (double)center->x - (double)g_4b9da0.x;
    double delta_y = (double)center->y - (double)g_4b9da0.y;
    real distance = (real)(sqrt(delta_x * delta_x + delta_y * delta_y) - radius);
    real fade = distance < 0.0f ? 0.0f : distance > 1.5f * radius ? 1.5f * radius : distance;
    fade = 1.0f - fade / (1.5f * radius);
    if (fade > 0.0f)
    {
        real spread = fade * radius * 2.0f;
        real maximum_distance = radius - 0.0001f + spread;
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
        long variant_offset = *(word *)(polygon_pointer_19f(groups, 0xc) + (group_offset + (*variant_reference)) * 2) & 0x1ff;
        long shader = *(long *)(polygon_pointer_19f(groups, 0x14) + (variant_offset + pass) * 10 + 4);
        byte *attributes = polygon_pointer_19f(polygon_tag_19f(*(long *)(polygon_pointer_19f(
            polygon_pointer_19f(polygon_tag_19f(shader), 0x20), 4) + 0x100)), 8);
        long attribute_count = *(long *)(attributes + 4);
        short *types = *(short **)(attributes + 8);
        long uv = NONE;
        for (long i = 0; i < attribute_count; ++i)
        {
            if (types[i] == 3)
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
            real alpha = (real)((maximum_distance - sqrt(vertex_x * vertex_x + vertex_y * vertex_y)) /
                (maximum_distance - spread));
            alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
            point3f bottom = { vertices[index].x, vertices[index].y, center->z };
            point3f top = bottom;
            top.z += height;
            D3DDevice_SetVertexData4f(3, color->x, color->y, color->z, alpha);
            if (uv != NONE)
                D3DDevice_SetVertexData2f(uv, texture_position, 1.0f);
            D3DDevice_SetVertexData4f(0, bottom.x, bottom.y, bottom.z, 1.0f);
            if (uv != NONE)
                D3DDevice_SetVertexData2f(uv, texture_position, 0.0f);
            D3DDevice_SetVertexData4f(0, top.x, top.y, top.z, 1.0f);
            long previous = index == 0 ? count - 1 : step - 1;
            double edge_x = (double)vertices[previous].x - (double)vertices[index].x;
            double edge_y = (double)vertices[previous].y - (double)vertices[index].y;
            texture_position = (real)(texture_position - sqrt(edge_x * edge_x + edge_y * edge_y) * texture_scale);
        }
        D3DDevice_End();
        D3DDevice_SetRenderState(D3DRS_CULLMODE, old_cull);
    }
}

