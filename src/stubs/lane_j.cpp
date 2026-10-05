// stubs for the game functions lane J's code (0x090000..0x09ffff) calls that
// are not decompiled yet
#include "unknown_11c920.h"

// @stub 0xa9120
void __stdcall function_a9120(long unit_index, long trick)
{
}

struct s_bitstream;
struct s_network_connection;

/* lane D's region */
// @stub 0x54810
void __stdcall function_054810(void const *data, long size)
{
}

/* lane D's region: a view's baseline update (a c_simulation_view method) */
class c_class_58d20;
class c_class_938e0;
struct s_session_id;
struct s_session_member_identity;
struct s_type_99af70;

/* lane D's region: the session handlers the message handler calls */
// @stub 0x61570
void __stdcall function_061570(c_class_58d20 *session, bool flag)
{
}

// @stub 0x5e3f0
bool __stdcall function_05e3f0(c_class_58d20 *session, s_type_99af70 const *address)
{
	return false;
}

// @stub 0x5cb80
bool __stdcall function_05cb80(c_class_58d20 *session, void const *message)
{
	return false;
}

// @stub 0x5d9e0
bool __stdcall function_05d9e0(c_class_58d20 *session, void const *message)
{
	return false;
}

/* kept out of the build in src/unknown_059ad0.cpp: built there, its session
   moves into a register and the matched 0x94700 no longer matches */
// @stub 0x5efd0
bool __stdcall network_session_handle_player_add(c_class_58d20 *session, long remote_index, void const *message)
{
	return false;
}

// @stub 0x5e7f0
bool __stdcall function_05e7f0(c_class_58d20 *session, s_type_99af70 const *address, void const *message, long *reason, bool *has_identity, s_session_member_identity *identity)
{
	return false;
}

/* lane D's region: a connection's update */
// @stub 0x883c0
void __stdcall function_0883c0(struct s_network_connection *connection)
{
}

/* lane D's region: a connection's reconnect */
/* lane J's, kept out of the build in src/unknown_075800.cpp (they
   change lane D's 0x5a520 convention): the session disband and boot handlers */
// @stub 0x94310
void __stdcall function_094310(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
}

// @stub 0x94330
void __stdcall function_094330(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
}

/* lane J's out-of-band handlers still to write, and the session disband
   counterparts kept out of the build (src/unknown_075800.cpp) */
// @stub 0x93fa0
void __stdcall function_093fa0(c_class_938e0 *handler, void const *message)
{
}

// @stub 0x94220
void __stdcall function_094220(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
}

// @stub 0x942d0
void __stdcall function_0942d0(c_class_938e0 *handler, s_session_id const *message, s_type_99af70 const *address)
{
}

struct s_network_message_session_query;

/* outside lane J: the system link reply and the out-of-band session
   handlers (lane D's region) */
// @stub 0xb2fc0
void __stdcall function_0b2fc0(s_network_message_session_query const *message)
{
}

// @stub 0x63080
void __stdcall function_063080(c_class_58d20 *session, s_network_message_session_query const *message, s_type_99af70 const *address)
{
}

// @stub 0x785d0
void __stdcall function_0785d0(void *unknown10, s_type_99af70 const *address, void const *message)
{
}

class c_simulation_view;
