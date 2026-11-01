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

// score 29 (was 29): the handler and gateway blocks still differ: retail keeps a
// copy of the slot pointer in edx and esi and clears the two back-pointers through
// materialized field addresses (add eax, 0xc and add eax, 8); ours stores both
// through [eax+N]. Everything before the handler block matches (the gateway flag
// cleared with a one-byte memset; a plain store gives 54).
// Tried: pointer-to-field locals (s_gateway_slot **) 67, and 29 on this form (same
// code); static dispose helpers, inline member dispose() on both slots and static
// detach helpers taking T ** 67; the globals read again inside the tests 67;
// volatile initialized/back-pointer fields 65; typed handler/gateway members 67;
// byte flags on either or both slots 67; initialization values 67; inactive return
// 67; byte release ordinal 112; short/byte session ordinals 125/125; shared index
// scope, session reference, session array receiver and link reference 67;
// handler/gateway/both native clears 58/59/54; session clears 71/68/67/59; all
// three 47; full pointer ranges 57; reciprocal clears 47/59/59; trailing pointer
// 47; all scalar clears 72; separate field stores in retail's order in both slots
// 59 (the gateway's alone 51, the handler's alone 55); one-pointer memsets of the
// two back-pointers 29 (same code).
// the materialized field addresses (add eax, 0xc and add eax, 8), ours stores both
// through [eax+N]; everything before the handler block now matches (the
// gateway flag cleared with a one-byte memset; a plain store gives 54).
// Earlier 68 (was 67): retail keeps the node pointer in eax and adds 0xc to it before the
// compare, ours copies it into ecx and reads [eax+0xc] instead.
// Earlier 67 (was 67): field-address carriers and post-clear link load still differ;
// typed handler/gateway/both members 67/67/67; gateway/handler/both byte flags 67/67/67;
// initialization values 67; inactive return 67.
// everything before the handler block matches. In the handler and gateway
// blocks retail keeps the tested-and-cleared fields as pointers (handler copied to edx,
// eax += 0xc for &handler->gateway; gateway copied to esi, lea edx [eax+4] for
// &gateway->initialized, eax += 8 for &gateway->link); ours stores through [eax+N].
// Tried (all 67): pointer-to-field locals (s_gateway_slot **), static dispose helpers,
// inline member dispose() on both slots, static detach helpers taking T **, reading the
// globals again inside the tests; volatile initialized/back-pointer fields 65 (other shape).
// Earlier 47 (was 67): Handler/gateway field carriers and post-clear link load still differ.
// byte release ordinal 112; short session ordinal 125; byte session ordinal 125; shared index
// scope 67; session reference 67; session array receiver 67; link reference 67; handler/gateway
// references 67.
// session clear schedule, field-address carriers and final link load differ;
// handler/gateway/both native clears 58/59/54; session 71; handler/session 68;
// gateway/session 67; all three 47 plain; full pointer ranges 57.
// Earlier 47 (was 47): session schedule, field addresses and late link observation remain; native
// handler/gateway reciprocal clears 47/59, both 59; gateway/handler byte clears 72/47; separate
// session clears 59; trailing pointer 47; all scalar clears 72.
// reverse chained handler/gateway/both pair clears 55/51/59; those forms with
// complete reciprocal-slot and active-field lifetimes 55/51/59; both reversed
// pairs in phase scopes 59; reversed pairs/field lifetimes/native handler byte 59.
// Round 95 (lane 8): retail's `mov ecx, [eax + 0xc]; add eax, 0xc` is the shape of a
// pointer-to-pointer variable (the matched list walks 0x984d0/0x89df0 have it), with the
// address shared by the test load and the store. The handler flag as a one-byte memset 29
// (same code); that with one-pointer memsets of both back-pointers 29 (same code); the
// back-pointer memsets alone 29; the link/messages pairs as plain stores in retail's order
// (handler 0x14, 8, 4; gateway 0x10, 0xc) with the handler flag memset, with or without the
// back-pointer memsets 59/59 (retail's store order and EBX zero, but no materialised
// back-pointer address: 8 bytes short there); plain pairs with the back-pointer memsets 59;
// the back-pointers as one-member structs with an inline detach() on the handler, the
// gateway or both 29/29/29 (same code), both with plain pairs 59.
// KIMI1 r94: guarded gateway 12-byte clear 73; gateway/handler/both 12-byte
// ranges 29/29/29 (same code); base plus back-pointer field aliases handler/
// gateway/both 29/29/29 (same code); base plus active-field alias 54;
// scoped link local 29 (same code); direct first link global read 29 (same code).
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
		s_handler_slot *handler = (s_handler_slot *)g_51056c;
		if (handler->initialized)
		{
			if (handler->gateway)
			{
				handler->gateway->handler = 0;
				handler->gateway = 0;
			}
			handler->unknown14 = 0;
			memset(&handler->link, 0, sizeof(handler->link) + sizeof(handler->messages));
			handler->initialized = false;
		}
		s_gateway_slot *gateway = (s_gateway_slot *)g_510568;
		if (gateway->initialized)
		{
			if (gateway->link)
			{
				gateway->link->out_of_band_consumer = 0;
				gateway->link = 0;
			}
			memset(&gateway->messages, 0, sizeof(gateway->messages) + sizeof(gateway->handler));
			memset(&gateway->initialized, 0, sizeof(gateway->initialized));
		}
		memset(g_510564, 0, 0x5a0);
		c_class_93590 *link = g_510560;
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
