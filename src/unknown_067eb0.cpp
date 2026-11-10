// @flags /O2 /Gr
/* UNKNOWN_067EB0.CPP: lifecycle callbacks (entry 23) */

#include "unknown_11c920.h"
#include "globals.h"

long g_4cf784;

// @retail 0x67eb0
void function_067eb0(void)
{
	if (g_4cf770)
	{
		g_4cf77c = 0;
		g_4cf780 = 0;
		g_4cf784 = 0;
		g_4cf770 = false;
	}
}

#include <string.h>
#include "unknown_067e10.h"

struct s_simulation_definition_registry
{
	long entity_count;
	void *entities[32];
	long event_count;
	void *events[32];
};

c_class_6a600 g_544b20;
s_simulation_world_owner g_545d40;
s_simulation_definition_registry g_546974;

void __stdcall function_82240(s_simulation_definition_registry *registry,
	long *entity_count, long *event_count);

// The three pointer globals are written and read through their addresses, with their real
// types: retail treats them as address-taken (it stores them ahead of the clears and reloads the
// registry pointer on both sides of the call), and the registry is cleared through that pointer.
// @retail 0x67e30
void function_67e30(void)
{
	*(s_simulation_definition_registry **)&g_4cf784 = &g_546974;
	*(c_class_6a600 **)&g_4cf77c = &g_544b20;
	*(s_simulation_world_owner **)&g_4cf780 = &g_545d40;
	s_simulation_definition_registry *registry = *(s_simulation_definition_registry *const *)&g_4cf784;
	registry->entity_count = NONE;
	memset(registry->entities, 0, sizeof(registry->entities));
	registry->event_count = NONE;
	memset(registry->events, 0, sizeof(registry->events));
	long entity_count;
	long event_count;
	function_82240(*(s_simulation_definition_registry *const *)&g_4cf784, &entity_count, &event_count);
	registry = *(s_simulation_definition_registry *const *)&g_4cf784;
	registry->entity_count = entity_count;
	registry->event_count = event_count;
	g_4cf770 = true;
}
