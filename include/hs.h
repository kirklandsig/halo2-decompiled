/* HS.H: the scripting system's value type, its types and the script
   function definitions */

#ifndef HS_H
#define HS_H

#include "unknown_11c920.h"

union script_value
{
	dword d;
	real r;
	short s;
	word w;
	byte b;
};

enum
{
	k_maximum_hs_function_parameters = 8
};

enum e_hs_type
{
	_hs_type_unparsed = 0,
	_hs_type_special_form,
	_hs_type_function_name,
	_hs_type_passthrough,
	_hs_type_void,
	_hs_type_boolean,
	_hs_type_real,
	_hs_type_short_integer,
	_hs_type_long_integer,
	_hs_type_string,
	_hs_type_script,
	_hs_type_string_id,
	_hs_type_unit_seat_mapping,
	_hs_type_trigger_volume,
	_hs_type_cutscene_flag,
	_hs_type_cutscene_camera_point,
	_hs_type_cutscene_title,
	_hs_type_cutscene_recording,
	_hs_type_device_group,
	_hs_type_ai,
	_hs_type_ai_command_list,
	_hs_type_ai_command_script,
	_hs_type_ai_behavior,
	_hs_type_ai_orders,
	_hs_type_starting_profile,
	_hs_type_conversation,
	_hs_type_structure_bsp,
	_hs_type_navpoint,
	_hs_type_point_reference,
	_hs_type_style,
	_hs_type_hud_message,
	_hs_type_object_list,
	_hs_type_sound,
	_hs_type_effect,
	_hs_type_damage,
	_hs_type_looping_sound,
	_hs_type_animation_graph,
	_hs_type_damage_effect,
	_hs_type_object_definition,
	_hs_type_bitmap,
	_hs_type_shader,
	_hs_type_render_model,
	_hs_type_structure_definition,
	_hs_type_lightmap_definition,
	_hs_type_game_difficulty,
	_hs_type_team,
	_hs_type_actor_type,
	_hs_type_hud_corner,
	_hs_type_model_state,
	_hs_type_network_event,
	_hs_type_object,
	_hs_type_unit,
	_hs_type_vehicle,
	_hs_type_weapon,
	_hs_type_device,
	_hs_type_scenery,
	_hs_type_object_name,
	_hs_type_unit_name,
	_hs_type_vehicle_name,
	_hs_type_weapon_name,
	_hs_type_device_name,
	_hs_type_scenery_name
};

typedef void (__stdcall *hs_evaluate_proc)(short function_index, long thread_index, bool initialize);

/* a script function's definition (.rdata 0x44b098 onwards; retail sizes
   each one to its parameters); the function table g_4744e0 points at one
   per script function */
struct s_type_f4462a
{
	short return_type;
	word flags;
	hs_evaluate_proc evaluate;
	void const *parse;
	short parameter_count;
	short parameter_types[k_maximum_hs_function_parameters];
};

/* the function table, defined beside script_return_store in unknown_209ae0.cpp */
extern s_type_f4462a *g_4744e0[];

inline s_type_f4462a *function_xca4acb(short function_index)
{
	return g_4744e0[function_index];
}

#endif
