// @flags /O2 /Gr
/* UNKNOWN_0E6900.CPP: sending a request to a unit (an outside function lane A's
   AI script functions need; its callers are in many regions) */

#include "cseries.h"
#include "globals.h"
#include "slot_handler.h"

/* a unit request type's handlers (0x467564..0x4677b4, 0x10 bytes each, in
   0xe7280..0xedf60): the first performs the request */
typedef bool (__stdcall *t_unit_request_proc)(long unit_index, s_unit_request *request);
typedef bool (__stdcall *t_unit_request_update_proc)(long unit_index, long type);
typedef void (__stdcall *t_unit_request_end_proc)(long unit_index, long type);

/* perform, update (each tick), finished and interrupted (the last three
   take the request type) */
struct s_unit_request_definition
{
	t_unit_request_proc perform;
	t_unit_request_update_proc update;
	t_unit_request_end_proc finished;
	t_unit_request_end_proc interrupted;
};

#define UNIT_REQUEST_DEFINITION(perform, update, finished, interrupted) \
	{ (t_unit_request_proc)(perform), (t_unit_request_update_proc)(update), (t_unit_request_end_proc)(finished), \
	(t_unit_request_end_proc)(interrupted) }

/* the callbacks (stubs in src/stubs/lane_i.cpp) */
bool __stdcall function_e7280(long unit_index, s_unit_request *request);
bool __stdcall function_e7320(long unit_index, long type);
void __stdcall function_e73c0(long unit_index, long type);
bool __stdcall function_e7440(long unit_index, s_unit_request *request);
bool __stdcall unit_action_throw_grenade(long unit_index, s_unit_request *request);
bool __stdcall unit_action_throw_grenade_update(long unit_index, long type);
void __stdcall unit_action_throw_grenade_interrupted(long unit_index, long type);
bool __stdcall unit_action_weapon_switch(long unit_index, s_unit_request *request);
bool __stdcall unit_action_weapon_switch_update(long unit_index, long type);
void __stdcall unit_action_weapon_switch_interrupted(long unit_index, long type);
void __stdcall unit_action_weapon_switch_finished(long unit_index, long type);
bool __stdcall unit_action_drop_weapon(long unit_index, s_unit_request *request);
bool __stdcall unit_action_pickup_weapon(long unit_index, s_unit_request *request);
bool __stdcall function_e9190(long unit_index, s_unit_request *request);
bool __stdcall function_e9370(long unit_index, s_unit_request *request);
bool __stdcall function_e9500(long unit_index, s_unit_request *request);
bool __stdcall function_e9690(long unit_index, s_unit_request *request);
bool __stdcall unit_action_melee(long unit_index, s_unit_request *request);
bool __stdcall unit_action_melee_attack(long unit_index, s_unit_request *request);
bool __stdcall unit_action_melee_attack_update(long unit_index, long type);
void __stdcall unit_action_melee_attack_interrupted(long unit_index, long type);
void __stdcall unit_action_vehicle_entry_finished(long unit_index, long type);
bool __stdcall unit_action_vehicle_entry_update(long unit_index, long type);
bool __stdcall unit_action_vehicle_entry(long unit_index, s_unit_request *request);
bool __stdcall unit_action_vehicle_exit(long unit_index, s_unit_request *request);
void __stdcall unit_action_vehicle_exit_finished(long unit_index, long type);
bool __stdcall unit_action_vehicle_exit_immediate(long unit_index, s_unit_request *request);
bool __stdcall unit_action_vehicle_exit_update(long unit_index, long type);
bool __stdcall unit_action_vehicle_board(long unit_index, s_unit_request *request);
void __stdcall unit_action_vehicle_board_finished(long unit_index, long type);
bool __stdcall unit_action_vehicle_board_update(long unit_index, long type);
bool __stdcall unit_action_vehicle_ejection(long unit_index, s_unit_request *request);
void __stdcall unit_action_vehicle_ejection_finished(long unit_index, long type);
bool __stdcall unit_action_vehicle_ejection_update(long unit_index, long type);
bool __stdcall unit_action_vehicle_flip(long unit_index, s_unit_request *request);
bool __stdcall function_eb7e0(long unit_index, s_unit_request *request);
void __stdcall function_eb960(long unit_index, long type);
bool __stdcall function_ebaa0(long unit_index, s_unit_request *request);
bool __stdcall function_ebb40(long unit_index, s_unit_request *request);
bool __stdcall function_ebd30(long unit_index, s_unit_request *request);
bool __stdcall function_ebf20(long unit_index, s_unit_request *request);
bool __stdcall function_ec050(long unit_index, s_unit_request *request);
void __stdcall function_ec2b0(long unit_index, long type);
bool __stdcall function_ec2d0(long unit_index, s_unit_request *request);
void __stdcall function_ec330(long unit_index, long type);
bool __stdcall function_ec380(long unit_index, s_unit_request *request);
bool __stdcall function_ec4f0(long unit_index, long type);
bool __stdcall function_ec940(long unit_index, s_unit_request *request);
bool __stdcall function_ec970(long unit_index, s_unit_request *request);
bool __stdcall function_ec9a0(long unit_index, s_unit_request *request);
void __stdcall function_ecb20(long unit_index, long type);
bool __stdcall function_ecb80(long unit_index, s_unit_request *request);
void __stdcall function_ecc70(long unit_index, long type);
bool __stdcall function_eccc0(long unit_index, s_unit_request *request);
bool __stdcall function_ecd50(long unit_index, s_unit_request *request);
void __stdcall function_ecdc0(long unit_index, long type);
bool __stdcall function_ecf30(long unit_index, s_unit_request *request);
void __stdcall function_ecfc0(long unit_index, long type);
bool __stdcall function_ecff0(long unit_index, s_unit_request *request);
bool __stdcall function_ed560(long unit_index, long type);
bool __stdcall function_ed680(long unit_index, s_unit_request *request);
bool __stdcall function_ed710(long unit_index, long type);
bool __stdcall function_ed800(long unit_index, s_unit_request *request);
bool __stdcall function_ed910(long unit_index, long type);
bool __stdcall function_eda30(long unit_index, s_unit_request *request);
bool __stdcall function_ede60(long unit_index, s_unit_request *request);
bool __stdcall function_edf10(long unit_index, long type);
void __stdcall function_edf60(long unit_index, long type);

/* folded in retail with the other empty callbacks of two arguments */
static void __stdcall unit_request_ignore(long unit_index, long type)
{
}

s_unit_request_definition g_467564 = UNIT_REQUEST_DEFINITION(function_e7280, function_e7320, 0, function_e73c0);
s_unit_request_definition g_467574 = UNIT_REQUEST_DEFINITION(function_e7440, 0, 0, 0);
s_unit_request_definition g_467584 = UNIT_REQUEST_DEFINITION(unit_action_weapon_switch, unit_action_weapon_switch_update, unit_action_weapon_switch_finished, unit_action_weapon_switch_interrupted);
s_unit_request_definition g_467594 = UNIT_REQUEST_DEFINITION(unit_action_drop_weapon, 0, 0, 0);
s_unit_request_definition g_4675a4 = UNIT_REQUEST_DEFINITION(unit_action_pickup_weapon, 0, 0, 0);
s_unit_request_definition g_4675b4 = UNIT_REQUEST_DEFINITION(function_e9190, 0, 0, 0);
s_unit_request_definition g_4675c4 = UNIT_REQUEST_DEFINITION(unit_action_throw_grenade, unit_action_throw_grenade_update, 0, unit_action_throw_grenade_interrupted);
s_unit_request_definition g_4675d4 = UNIT_REQUEST_DEFINITION(function_e9370, 0, 0, 0);
s_unit_request_definition g_4675e4 = UNIT_REQUEST_DEFINITION(function_e9500, 0, 0, 0);
s_unit_request_definition g_4675f4 = UNIT_REQUEST_DEFINITION(function_e9690, 0, 0, 0);
s_unit_request_definition g_467604 = UNIT_REQUEST_DEFINITION(unit_action_melee, 0, 0, 0);
s_unit_request_definition g_467614 = UNIT_REQUEST_DEFINITION(unit_action_melee_attack, unit_action_melee_attack_update, 0, unit_action_melee_attack_interrupted);
s_unit_request_definition g_467624 = UNIT_REQUEST_DEFINITION(unit_action_vehicle_entry, unit_action_vehicle_entry_update, unit_action_vehicle_entry_finished, 0);
s_unit_request_definition g_467634 = UNIT_REQUEST_DEFINITION(unit_action_vehicle_exit, unit_action_vehicle_exit_update, unit_action_vehicle_exit_finished, 0);
s_unit_request_definition g_467644 = UNIT_REQUEST_DEFINITION(unit_action_vehicle_exit_immediate, 0, 0, 0);
s_unit_request_definition g_467654 = UNIT_REQUEST_DEFINITION(unit_action_vehicle_board, unit_action_vehicle_board_update, unit_action_vehicle_board_finished, 0);
s_unit_request_definition g_467664 = UNIT_REQUEST_DEFINITION(unit_action_vehicle_ejection, unit_action_vehicle_ejection_update, unit_action_vehicle_ejection_finished, unit_request_ignore);
s_unit_request_definition g_467674 = UNIT_REQUEST_DEFINITION(unit_action_vehicle_flip, 0, 0, 0);
s_unit_request_definition g_467684 = UNIT_REQUEST_DEFINITION(function_eb7e0, 0, function_eb960, 0);
s_unit_request_definition g_467694 = UNIT_REQUEST_DEFINITION(function_ebaa0, 0, function_ec2b0, 0);
s_unit_request_definition g_4676a4 = UNIT_REQUEST_DEFINITION(function_ebb40, 0, 0, 0);
s_unit_request_definition g_4676b4 = UNIT_REQUEST_DEFINITION(function_ebd30, 0, 0, 0);
s_unit_request_definition g_4676c4 = UNIT_REQUEST_DEFINITION(function_ebf20, 0, 0, 0);
s_unit_request_definition g_4676d4 = UNIT_REQUEST_DEFINITION(function_ec050, 0, function_ec2b0, 0);
s_unit_request_definition g_4676e4 = UNIT_REQUEST_DEFINITION(function_ec2d0, 0, function_ec330, 0);
s_unit_request_definition g_4676f4 = UNIT_REQUEST_DEFINITION(function_ec380, function_ec4f0, 0, 0);
s_unit_request_definition g_467704 = UNIT_REQUEST_DEFINITION(function_ec940, 0, function_ec2b0, 0);
s_unit_request_definition g_467714 = UNIT_REQUEST_DEFINITION(function_ec970, 0, function_ec2b0, 0);
s_unit_request_definition g_467724 = UNIT_REQUEST_DEFINITION(function_ec9a0, 0, function_ecb20, 0);
s_unit_request_definition g_467734 = UNIT_REQUEST_DEFINITION(function_eccc0, 0, 0, 0);
s_unit_request_definition g_467744 = UNIT_REQUEST_DEFINITION(function_ecd50, 0, function_ecdc0, 0);
s_unit_request_definition g_467754 = UNIT_REQUEST_DEFINITION(function_ecb80, 0, function_ecc70, 0);
s_unit_request_definition g_467764 = UNIT_REQUEST_DEFINITION(function_ecf30, 0, function_ecfc0, 0);
s_unit_request_definition g_467774 = UNIT_REQUEST_DEFINITION(function_ecff0, function_ed560, 0, 0);
s_unit_request_definition g_467784 = UNIT_REQUEST_DEFINITION(function_ed680, function_ed710, 0, 0);
s_unit_request_definition g_467794 = UNIT_REQUEST_DEFINITION(function_ed800, function_ed910, 0, 0);
s_unit_request_definition g_4677a4 = UNIT_REQUEST_DEFINITION(function_eda30, 0, 0, 0);
s_unit_request_definition g_4677b4 = UNIT_REQUEST_DEFINITION(function_ede60, function_edf10, function_edf60, 0);

/* the definition of each request type (60 types, in .data) */
s_unit_request_definition *g_4677c8[60] =
{
	&g_467564, &g_467564, &g_467574, &g_467574, &g_467574, &g_467574,
	&g_467574, &g_467574, &g_467584, &g_467594, &g_467564, &g_467564,
	&g_467574, &g_467574, &g_467574, &g_467574, &g_467574, &g_467574,
	&g_467584, &g_467594, &g_4675a4, &g_4675b4, &g_4675c4, &g_4675d4,
	&g_4675e4, &g_4675f4, &g_467604, &g_467614, &g_467624, &g_467634,
	&g_467644, &g_467654, &g_467664, &g_467674, &g_467684, &g_467694,
	&g_4676a4, &g_4676b4, &g_4676c4, &g_4676d4, &g_4676e4, &g_4676f4,
	&g_467704, &g_467714, &g_467724, &g_467734, &g_467744, &g_467754,
	&g_467764, &g_467774, &g_467784, &g_467794, &g_4677a4, &g_4677b4,
	&g_467574, &g_467574, &g_467574, &g_467574, &g_467574, &g_467574
};

void function_b7360(long object_index);
void function_1e77c0(long player_index, long type, byte result);

// @retail 0xe6900
bool function_e6900(long unit_index, s_unit_request *request)
{
	s_unit_request_definition *definition = g_4677c8[request->type];
	bool result;

	function_b7360(unit_index);
	result = definition->perform(unit_index, request);

	if (unit_index != NONE)
	{
		s_slot_object_view *unit = object_get(unit_index);

		if (unit->player_index != NONE)
			function_1e77c0(unit->player_index, request->type, result);
	}

	return result;
}
