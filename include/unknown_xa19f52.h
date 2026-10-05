/* UNKNOWN_XA19F52.H: the simulation world's entity database, as
   src/unknown_0a58d0.cpp (an entity's object) and src/unknown_0aa4d0.cpp
   (an entity's relevance to the observers) see it */

#ifndef UNKNOWN_XA19F52_H
#define UNKNOWN_XA19F52_H

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"

/* the entity definitions (g_4cf784, src/unknown_067eb0.cpp): a count, then
   one definition per entity type */
extern long g_4cf784;

struct s_entity_definitions
{
	long count;
	c_object_type_definition *definitions[1];
};

/* an entity in the database (0x20 bytes) */
struct s_simulation_entity
{
	long identifier;
	short type;
	bool field_6;
	byte unknown07;
	long object_index;
	byte unknown0c[0x14];
};

struct s_simulation_entity_table
{
	byte unknown00[0x14];
	s_simulation_entity entities[0x400];
};

struct s_simulation_entity_database
{
	byte unknown00[0x2098];
	s_simulation_entity_table table;
};

struct s_simulation_world_view
{
	byte unknown00[4];
	s_simulation_entity_database *database;
};

/* the entity an index stands for, or none when its salt is stale
   (src/unknown_0a58d0.cpp) */
s_simulation_entity *simulation_entity_try_get(s_simulation_entity_table *table, long entity_index);

#endif
