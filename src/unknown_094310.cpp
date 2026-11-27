// @flags /O2 /Gr
/* UNKNOWN_094310.CPP: the handler for a session's disband message: looks
   the session up in the handler's session manager and passes the message
   on. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_0820f0.h"

/* the message handler (src/unknown_075800.cpp): only the member this file needs */
struct s_network_session_list;
class c_class_938e0
{
public:
	void handle_session_disband(const s_session_id *message, const s_type_99af70 *address);

	byte unknown00[0x14];
	s_network_session_list *session_manager;
};

c_class_58d20 *network_session_manager_find_session(s_network_session_list *manager, const s_session_id *session_id);
bool network_session_handle_session_disband(c_class_58d20 *session, const s_type_99af70 *address);

// @retail 0x94310
void c_class_938e0::handle_session_disband(const s_session_id *message, const s_type_99af70 *address)
{
	c_class_58d20 *session = network_session_manager_find_session(session_manager, message);
	if (session)
		network_session_handle_session_disband(session, address);
}
