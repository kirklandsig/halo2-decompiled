// @flags /O2 /GL-
/* UNKNOWN_315340.CPP: sets the two mass scales of a constraint's scaling
   action (the scale_a and scale_b that 0x315280 applies and 0x3152f0 undoes),
   in the order of its two entities, and asks its listener for the action. */

#include "unknown_11c920.h"
#include <stddef.h>

class hkEntity;

/* the action's state values: set at run time */
dword g_480990;
dword g_480770;

struct s_mass_scale_request
{
	long unknown00;
	long unknown04;
	long size;
	long unknown0c;
};

class c_mass_scale_listener
{
public:
	virtual void v0();
	virtual void v1();
	virtual void request(void *action, s_mass_scale_request *request);
};

/* at +0x10 of the object the game hands in */
struct s_mass_scale_action
{
	dword state;
	byte unknown04[8];
	hkEntity *entity_a;
	byte unknown10[4];
	c_mass_scale_listener *listener;
	byte unknown18[0x40 - 0x18];
	real scale_a;
	real scale_b;
};

typedef char mass_scale_listener_offset[(offsetof(s_mass_scale_action, listener) == 0x14) ? 1 : -1];
typedef char mass_scale_offset[(offsetof(s_mass_scale_action, scale_a) == 0x40) ? 1 : -1];

// @retail 0x315340
void __cdecl function_315340(void *object, hkEntity *entity, hkEntity *other, real scale, real other_scale)
{
	s_mass_scale_action *action = (s_mass_scale_action *)((byte *)object + 0x10);
	if (action->state != g_480990)
		return;
	if (entity == action->entity_a)
	{
		action->scale_a = scale;
		action->scale_b = other_scale;
	}
	else
	{
		action->scale_a = other_scale;
		action->scale_b = scale;
	}
	s_mass_scale_request request;
	request.unknown00 = 0;
	request.unknown04 = 0;
	request.size = 0x88;
	request.unknown0c = 0;
	action->listener->request(action, &request);
	action->state = g_480770;
}
