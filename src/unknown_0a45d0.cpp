// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "unknown_0a58d0.h"

class c_handler
{
public:
	virtual long function_xbc4d95() { return 0; }
	virtual bool handler1(long a) { return true; }
	virtual bool handler2(long a, long b, long c, long d);
	virtual bool handler3(long a, long b, long c, long d);
	virtual void handler4(dword *a);
	virtual bool handler5(long a, long b, long c, long d);
	virtual bool handler6(dword *a);
};

// @retail 0xa45d0
bool c_object_type_definition::v32(long a)
{
	return true;
}

// @retail 0xa45e0
bool c_handler::handler2(long a, long b, long c, long d)
{
	bool result = false;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long id = NONE;
	if (manager)
		id = manager->get_current_id();
	if (function_xbc4d95() == id)
	{
		g_55e4d0[g_4e9ae8->engine_index]->p44(c, d);
		result = true;
	}
	return result;
}

// @retail 0xa4650
bool c_handler::handler3(long a, long b, long c, long d)
{
	bool result = false;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long id = NONE;
	if (manager)
		id = manager->get_current_id();
	if (function_xbc4d95() == id)
	{
		g_55e4d0[g_4e9ae8->engine_index]->p45(b, c, d);
		result = true;
	}
	return result;
}

// @retail 0xa46c0
void c_handler::handler4(dword *a)
{
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long id = NONE;
	if (manager)
		id = manager->get_current_id();
	if (function_xbc4d95() == id)
	{
		if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
		{
			if (((*a ^ g_4e9ae8->value24) & 0x3ff) == 0)
				g_4e9ae8->value24 = *a;
		}
	}
}

// @retail 0xa4730
bool c_handler::handler5(long a, long b, long c, long d)
{
	bool result = false;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long id = NONE;
	if (manager)
		id = manager->get_current_id();
	if (function_xbc4d95() == id)
		result = g_55e4d0[g_4e9ae8->engine_index]->p46(b, c, d);
	return result;
}

// @retail 0xa47a0
bool c_handler::handler6(dword *a)
{
	bool result = false;
	c_engine_peer *manager = g_55e4d0[g_4e9ae8->engine_index];
	long id = NONE;
	if (manager)
		id = manager->get_current_id();
	if (function_xbc4d95() == id)
	{
		g_4e9ae8->value24 = *a;
		result = true;
	}
	return result;
}
