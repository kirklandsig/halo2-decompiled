#include "unknown_4cf790.h"
// @flags /O2 /Gr
/* UNKNOWN_08DD70.CPP: the network dispose: releases the pending messages,
   the online tasks, the sessions, the observer, the message handler, the
   gateway and the link, then clears the network globals. */
#include "unknown_11c920.h"
#include "unknown_075870.h"
#include <string.h>
#include <xtl.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_0662e0.h"

struct s_system_link_globals
{
	bool active;
	byte unknown01[7];
	s_session_id session_id;
};
#include "system_link_globals.h"

class c_class_93590;
class c_class_938e0;
struct s_network_message_gateway;
class c_type_659ceb;
struct s_session_interface_globals;
struct s_search_value_globals;

extern bool g_4d8ba0;
extern long g_510584;
extern bool g_510580;

extern long g_510518;
extern long g_510530;
extern s_search_value_globals g_509450;
extern s_session_interface_globals g_4cd868;
extern c_class_93590 *g_510560;
extern s_network_message_gateway *g_510568;
extern s_network_observer *g_510570;
extern c_class_58d20 *g_510574;

/* the message handlers' block (0x5a0 bytes) and the message handler */
c_type_659ceb *g_510564;
c_class_938e0 *g_51056c;

void __stdcall function_8e0f0(long index, long result);
void pending_messages_reset(void);
void online_result_registration_clear(void);
void function_6b640(long task_index);
void online_tasks_dispose(void);
void network_session_manager_dispose(void);
void function_b3670(void);
void function_b31b0(void);
void network_session_close(c_class_58d20 *session);
void network_observer_dispose_channels(s_network_observer *observer);
void function_81f80(void);
void network_link_close_connections(c_class_93590 *link);
void network_link_close(c_class_93590 *link);

/* the fields of a session that the dispose resets */
struct s_session_slot
{
	byte unknown00[4];
	void *unknown04;
	s_network_observer *observer;
	long *owners;
	long owner_index;
	long value14;
	long value18;
	byte unknown1c[0x741c - 0x1c];
	long state;
	byte unknown7420[0x78b8 - 0x7420];
};

struct s_link_slot
{
	bool initialized;
	byte unknown01[0x1c - 1];
	void *out_of_band_consumer;
};

struct s_gateway_slot
{
	void *vtable;
	bool initialized;
	byte unknown05[3];
	s_link_slot *link;
	void *messages;
	void *handler;
};

struct s_handler_slot
{
	bool initialized;
	byte unknown01[3];
	void *link;
	void *messages;
	s_gateway_slot *gateway;
	void *observer;
	void *unknown14;
};

// @retail 0x8dd70
void function_8dd70(void)
{
	if (g_4d8ba0)
	{
		for (long index = 0; index < 32; index++)
			function_8e0f0(index, 13);
		pending_messages_reset();
		online_result_registration_clear();
		if (g_510584 != NONE)
		{
			function_6b640(g_510584);
			g_510584 = NONE;
		}
		g_510580 = false;
		online_tasks_dispose();
		g_4cf8e0 = false;
		if (g_510518 && g_510530 != NONE)
			function_6b640(g_510530);
		memset(&g_509450, 0, 0x70f8);
		memset(&g_4cd868, 0, 0x7d8);
		network_session_manager_dispose();
		function_b3670();
		function_b31b0();
		memset(&g_4d8ea8, 0, sizeof(g_4d8ea8));
		for (long index = 0; index < 3; index++)
		{
			s_session_slot *session = &((s_session_slot *)g_510574)[index];
			if (session->owners)
				session->owners[session->owner_index] = 0;
			if (session->state != NONE)
				network_session_close((c_class_58d20 *)session);
			session->state = NONE;
			session->value18 = NONE;
			session->value14 = 0;
			session->unknown04 = 0;
			session->observer = 0;
		}
		network_observer_dispose_channels(g_510570);
		function_81f80();
		/* read through their addresses: retail treats these pointers as ones a store may alias */
		s_handler_slot *handler = (s_handler_slot *)*(c_class_938e0 *const *)&g_51056c;
		if (((s_handler_slot *)g_51056c)->initialized)
		{
			if (((s_handler_slot *)g_51056c)->gateway)
			{
				((s_handler_slot *)g_51056c)->gateway->handler = 0;
				handler->gateway = 0;
			}
			handler->unknown14 = 0;
			handler->messages = 0;
			handler->link = 0;
			handler->initialized = false;
		}
		s_gateway_slot *gateway = (s_gateway_slot *)*(s_network_message_gateway *const *)&g_510568;
		if (((s_gateway_slot *)g_510568)->initialized)
		{
			if (((s_gateway_slot *)g_510568)->link)
			{
				((s_gateway_slot *)g_510568)->link->out_of_band_consumer = 0;
				gateway->link = 0;
			}
			gateway->handler = 0;
			gateway->messages = 0;
			gateway->initialized = false;
		}
		memset(g_510564, 0, 0x5a0);
		c_class_93590 *link = *(c_class_93590 *const *)&g_510560;
		network_link_close_connections(link);
		network_link_close(link);
		((s_link_slot *)link)->out_of_band_consumer = 0;
		((s_link_slot *)link)->initialized = false;
		memset(&g_network_configuration, 0, sizeof(g_network_configuration));
		g_4d8ba0 = false;
		g_510560 = 0;
		g_510564 = 0;
		g_510568 = 0;
		g_51056c = 0;
		g_510570 = 0;
		g_510574 = 0;
	}
}
