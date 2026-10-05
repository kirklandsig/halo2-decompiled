#include "unknown_11c920.h"

// @flags /O1 /Gr

class c_session_user_state
{
public:
	c_session_user_state();
	bool active;
	byte unknown01;
	short index;
};

// @retail 0x24fa12
c_session_user_state::c_session_user_state()
{
	index = NONE;
	active = false;
}

struct s_session_screen_state
{
	byte unknown00[0x814];
	c_session_user_state users[4];
};

// @retail 0x24fa23
long function_24fa23(s_session_screen_state const *screen)
{
	return screen->users[0].active || screen->users[1].active ||
		screen->users[2].active || screen->users[3].active;
}
