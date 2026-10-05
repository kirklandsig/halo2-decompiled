// @flags /O2 /Gr
/* UNKNOWN_059670.CPP: the current and the other session (the session
   manager's session_a and session_b) as the menus query them (outside lane H's region, decompiled
   by lane H because its menus call these with register arguments) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"

struct s_network_session_membership;

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

// @retail 0x592f0
bool function_592f0(void)
{
	bool result = false;
	if (g_527330.initialized)
	{
		c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
		long state = session->state;
		if (state && SESSION_STATE_IS_LIVE(state))
		{
			result = session->current_member == session->value50;
		}
	}
	return result;
}

// @retail 0x5a680
s_network_session_membership *function_5a680(c_class_58d20 *session, long *current_member, long *member_index)
{
	if (current_member)
	{
		*current_member = session->current_member;
	}
	if (member_index)
	{
		*member_index = session->member_index;
	}
	return (s_network_session_membership *)&session->value4c;
}
