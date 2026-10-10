#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "impacts.h"
// @flags /O2 /arch:SSE /Gr

struct s_277ec0;
struct s_havok_cd_body;
struct s_havok_impact_contact;
struct s_1cf520;
class c_1cf520;
extern short g_54e898;
extern s_record_pool *g_51ebfc;
void function_277ec0(real *, real *, short *, short *, s_277ec0 *, s_277ec0 *, long, long);
long havok_cd_body_shape_key_get(s_havok_cd_body const *);
long havok_entity_property_2002_get(hkEntity const *);
long __stdcall function_1d0400(s_havok_component *, short, void const *volatile,
    long, long, long, char, hkEntity *, hkEntity *, real, real, short, short);
void function_1cf520(s_1cf520 *, c_1cf520 *, bool, bool, long, long);
long havok_component_impact_find(s_havok_component *, s_havok_impact_contact const *, bool);
bool havok_component_impact_make_room(long, s_havok_component *, real);
bool impact_components_valid(long, long);
real impact_distance_squared_to_nearest_player(point3f const *, long);
long impact_new(s_impact_data const *, long);
void havok_component_impact_add(s_havok_component *, long);

class c_277b80
{
public:
    virtual void function_277b81() = 0;
    virtual void function_277b82() = 0;
    virtual void function_277b83() = 0;
    virtual void function_277b84() = 0;
    virtual void function_277b85() = 0;
    virtual long function_277b86() = 0;
    byte field_4[0xc];
    long field_10;
    byte field_14[6];
    char field_1a;
};

PRIVATE __forceinline void function_277b87(hkEntity *arg_0, long *arg_1, long *arg_2)
{
    *arg_1 = havok_entity_property_get(arg_0, 0x2001);
    *arg_2 = havok_entity_property_get(arg_0, 0x2002);
}

// @retail 0x277b80
long __stdcall function_277b80(void *arg_0, bool arg_1, real arg_2,
    c_1cf520 *arg_3, short arg_4, s_277ec0 *arg_5, s_277ec0 *arg_6,
    hkEntity *arg_7, hkEntity *arg_8, c_277b80 **arg_9)
{
    
    long local_0;
    long local_1;
    long local_2;
    long local_3;
    function_277b87(arg_7, &local_0, &local_1);
    function_277b87(arg_8, &local_2, &local_3);
    s_havok_component *local_4 = havok_component_get(local_0);
    long local_5 = NONE;
    if (!TEST_FIELD_BIT(local_4->flags12 & 1))
    {
        s_havok_component *local_6 = local_2 == NONE ? NULL : havok_component_get(local_2);
        long local_7 = local_6 ? local_6->object_index : NONE;
        short local_8 = g_54e898;
        short local_9 = g_54e898;
        real local_10 = 0.0f;
        real local_11 = 1.0f;
        char local_12 = 0;
        long local_13 = NONE;
        function_277ec0(&local_11, &local_10, &local_9, &local_8,
            arg_5, arg_6, local_0, local_2);
        if ((*arg_9)->function_277b86() == 0x18)
        {
            local_13 = (*arg_9)->field_10;
            local_12 = (*arg_9)->field_1a;
        }
        long local_14 = havok_cd_body_shape_key_get((s_havok_cd_body const *)arg_6);
        if (local_4->unknown04 & 1)
            local_5 = function_1d0400(local_4, arg_4, *(void const **)&arg_2, local_14, local_13,
                local_7, local_12, arg_7, arg_8, local_11, local_10, local_8, local_9);
        if (local_5 != NONE)
        {
            s_havok_component_element48 *local_15 = &local_4->unknown88.data[local_5];
            function_1cf520((s_1cf520 *)local_15, arg_3, true, arg_1, local_0,
                havok_entity_property_2002_get(arg_7));
            s_impact_data local_16;
            impact_data_set(&local_16, true, local_0, local_1, c_type_47f957(local_9),
                local_2, local_3, c_type_47f957(local_8), &local_15->position,
                &local_15->normal, NONE, NULL);
            long local_17 = havok_component_impact_find(local_4, (s_havok_impact_contact const *)&local_16, true);
            if (local_17 == NONE)
            {
                if ((!local_6 || havok_component_impact_find(local_6,
                    (s_havok_impact_contact const *)&local_16, false) == NONE)
                    && impact_components_valid(local_16.component_a, local_16.component_b))
                {
                    real local_18 = impact_distance_squared_to_nearest_player(&local_16.position, 1);
                    if (havok_component_impact_make_room(local_16.unknown08, local_4, local_18))
                    {
                        local_17 = impact_new(&local_16, 1);
                        havok_component_impact_add(local_4, local_17);
                    }
                }
            }
            if (local_17 != NONE)
            {
                byte *local_19 = (byte *)g_51ebfc->data + (local_17 & 0xffff) * 0xa0;
                local_15->impact_index = local_17;
                ++*(short *)(local_19 + 8);
            }
        }
    }
    void **local_20 = &arg_0;
    (void)*(void *volatile *)local_20;
    return local_5;
}
