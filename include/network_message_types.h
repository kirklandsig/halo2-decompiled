/* NETWORK_MESSAGE_TYPES.H: the network message type table and the functions
   that fill it in. Each message type has an encode and a decode callback and
   an optional third one, and a registration function per message family
   stores their addresses in the table. Being callbacks, the codecs keep the
   stdcall convention: an encoder or decoder takes the stream, the message
   size and the message on the stack.

   The families, in retail order, one source file per region:
     0xac490-0xacb10  discovery and connection    src/unknown_0ac490.cpp
     0xacc20-0xadab0  session protocol            src/unknown_0acc20.cpp
     0xadef0-0xaf680  session membership          src/unknown_0adef0.cpp
     0xaf890-0xb23d0  session parameters          src/unknown_0af890.cpp
     0xb2440-0xb2de0  view establishment, synchronous, results and test
                                                  src/unknown_0b2440.cpp */

#ifndef NETWORK_MESSAGE_TYPES_H
#define NETWORK_MESSAGE_TYPES_H

#include "unknown_11c920.h"
#include "bitstream.h"

/* the message types, in table order (the names are the registered ones) */
enum e_network_message_type
{
	_network_message_type_ping,
	_network_message_type_pong,
	_network_message_type_broadcast_search,
	_network_message_type_broadcast_reply,
	_network_message_type_connect_request,
	_network_message_type_connect_refuse,
	_network_message_type_connect_establish,
	_network_message_type_connect_closed,
	_network_message_type_join_request,
	_network_message_type_join_abort,
	_network_message_type_join_refuse,
	_network_message_type_leave_session,
	_network_message_type_leave_acknowledge,
	_network_message_type_session_disband,
	_network_message_type_session_boot,
	_network_message_type_host_handoff,
	_network_message_type_peer_handoff,
	_network_message_type_host_transition,
	_network_message_type_host_reestablish,
	_network_message_type_host_decline,
	_network_message_type_peer_reestablish,
	_network_message_type_peer_establish,
	_network_message_type_election,
	_network_message_type_election_refuse,
	_network_message_type_time_synchronize,
	_network_message_type_membership_update,
	_network_message_type_peer_properties,
	_network_message_type_delegate_leader,
	_network_message_type_boot_machine,
	_network_message_type_player_add,
	_network_message_type_player_refuse,
	_network_message_type_player_remove,
	_network_message_type_player_properties,
	_network_message_type_parameters_update,
	_network_message_type_parameters_request,
	_network_message_type_countdown_timer,
	_network_message_type_mode_acknowledge,
	_network_message_type_view_establishment,
	_network_message_type_player_acknowledge,
	_network_message_type_synchronous_update,
	_network_message_type_synchronous_actions,
	_network_message_type_synchronous_join,
	_network_message_type_synchronous_gamestate,
	_network_message_type_game_results,
	_network_message_type_test,

	k_network_message_type_count
};

typedef void (__stdcall *t_message_encode)(s_bitstream *stream, long size, void *message);
typedef bool (__stdcall *t_message_decode)(s_bitstream *stream, long size, void *message);
typedef bool (__stdcall *t_message_compare)(long size, void *a, void *b);

/* one entry of the message type table, 0x20 bytes */
struct s_message_type
{
	bool initialized;
	char const *name;
	long flags;                /* 1 for the variable size synchronous-gamestate */
	long minimum_size;
	long maximum_size;
	t_message_encode encode;
	t_message_decode decode;
	t_message_compare compare; /* the synchronous messages' comparison, or time-synchronize's reset */
};

class c_type_659ceb
{
public:
	void function_x5c51c9(e_network_message_type type, char const *name, long flags, long minimum_size, long maximum_size,
		t_message_encode encode, t_message_decode decode, t_message_compare compare)
	{
		s_message_type *definition = &m_types[type];
		definition->name = name;
		definition->flags = flags;
		definition->minimum_size = minimum_size;
		definition->maximum_size = maximum_size;
		definition->encode = encode;
		definition->decode = decode;
		definition->compare = compare;
		definition->initialized = true;
	}

	s_message_type m_types[k_network_message_type_count];
};

/* registers a message type of a fixed size with no third callback */
#define REGISTER_MESSAGE_TYPE(collection, type, name, size, encode, decode) \
	(collection)->function_x5c51c9(type, name, 0, size, size, (t_message_encode)(encode), (t_message_decode)(decode), NULL)

/* the registration functions, one per family */
void network_message_types_register_discovery(c_type_659ceb *collection);          /* 0xac800 */
void network_message_types_register_connection(c_type_659ceb *collection);         /* 0xacb10 */
void function_adab0(c_type_659ceb *collection);   /* 0xadab0 */
void network_message_types_register_session_membership(c_type_659ceb *collection); /* 0xaf680 */
void network_message_types_register_session_parameters(c_type_659ceb *collection); /* 0xb2220 */
void network_message_types_register_view_establishment(c_type_659ceb *collection); /* 0xb2680 */
void network_message_types_register_synchronous(c_type_659ceb *collection);        /* 0xb2b30 */
void network_message_types_register_game_results(c_type_659ceb *collection);       /* 0xb2cc0 */
void network_message_types_register_test(c_type_659ceb *collection);               /* 0xb2de0 */

/* the part of the parameters messages at 0x558 (parameters-request) and
   0x1488 (parameters-update), written by 0xb2330 and read by 0xb23d0 */
struct s_parameters_part
{
	long unknown00;
	byte unknown04[8];
	byte unknown0c[16];
	byte unknown1c[36];
	long unknown40;
};

void function_b2330(s_parameters_part *part, s_bitstream *stream);
bool function_b23d0(s_bitstream *stream, s_parameters_part *part);

/* callees of the codecs that are not decompiled yet (src/stubs/network.cpp).
   Apart from 0x63690 and 0x7cc50 (stdcall) and 0x87830 (fastcall), retail
   calls them with conventions LTCG chose (arguments in eax, ebx, esi or a
   mix of registers and the stack), which no stub can have, so their callers
   cannot match until they are decompiled */
struct s_player_action
{
	byte unknown00[0x5c];
};

struct s_session_id;
long network_session_time_since_start(const s_session_id *session_id); /* the time synchronize clock (unknown_075870.cpp) */
void __stdcall function_07ba10(s_bitstream *stream, void *session);    /* writes a session description */
bool __stdcall function_07c110(s_bitstream *stream, void *session);    /* reads a session description */
void function_07c5a0(s_bitstream *stream, void const *source);         /* writes a 0x90 byte sub-structure */
bool function_07ca70(s_bitstream *stream, void *destination);          /* reads it, true when it is valid */
void __stdcall function_063690(void *part, s_bitstream *stream);       /* writes the parameters' sub-structure at 0x3c / 0x8c */
byte function_063980(s_bitstream *stream, void *part);                 /* reads it */
void __stdcall function_07cc50(s_bitstream *stream, void *part);       /* writes the parameters' sub-structure at 0x3dc / 0x444 */
bool function_07d520(s_bitstream *stream, void *part);                 /* reads it */
void function_86f90(s_bitstream *stream, s_player_action *action);     /* the synchronous message helpers */
bool function_874c0(s_bitstream *stream, s_player_action *action);
bool function_87830(s_player_action *a, s_player_action *b);
void function_87d00(s_bitstream *stream, void *message);
bool function_87e90(s_bitstream *stream, void *message);
bool function_88060(void *a, void *b);
void function_197680(s_bitstream *stream, void *results);              /* the game results codecs */
byte function_197d80(s_bitstream *stream, void *results);

#endif
