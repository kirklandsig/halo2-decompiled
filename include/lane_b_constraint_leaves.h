#ifndef LANE_B_CONSTRAINT_LEAVES_H
#define LANE_B_CONSTRAINT_LEAVES_H
#include "growable_array_2d91d0.h"
#include <xmmintrin.h>
#include "unknown_1efac0.h"
#include "lane_b_entity_listener.h"

struct lane_b_constraint_owner;
struct listener_world_315d20;

#include "lane_a_constraints.h"
typedef lane_a_constraint_description lane_b_constraint_description;
typedef lane_a_constraint lane_b_constraint_prefix;
struct object_480b90;
extern object_480b90 g_480b90;

// Native identity initialization called by several Havok aggregate constructors.
void *__fastcall function_31bc70(void *object);
struct lane_b_constraint_aggregate
{
	lane_b_constraint_aggregate();
};

struct lane_b_constraint_sizes
{
	long total;
	long term_bytes;
	long op_bytes;
	long results;
};
struct lane_b_contact_id_array
{
	unsigned char *data;
	long size;
	long capacity_and_flags;
	long allocate(unsigned char value);
};
struct lane_b_contact_constraint_prefix;
// Abstract interface; pure dispatch slots are not undefined external members.
struct lane_b_constraint_owner
{
	virtual void slot0(void *) = 0;
	virtual void slot1(void *) = 0;
	virtual void add(lane_b_contact_constraint_prefix *, const lane_b_constraint_sizes *) = 0;
	virtual void remove(lane_b_contact_constraint_prefix *, const lane_b_constraint_sizes *) = 0;
};
struct lane_b_contact_point
{
	__m128 position;
	__m128 normal;
};
struct lane_b_contact_property
{
	float impulse;
	float bias;
	unsigned char unknown_08[16];
	unsigned short flags;
	unsigned short unknown_1a;
};
struct lane_b_contact_solver_data
{
	unsigned long flags;
	unsigned char unknown_04[0x3c];
};
struct lane_b_contact_constraint_prefix
{
	char unknown_00[0xc];
	listener_world_315d20 *first_body;
	listener_world_315d20 *second_body;
	lane_b_constraint_owner *owner;
	char unknown_18[8];
	lane_b_contact_id_array ids;
	char unknown_2c[8];
	struct point_array
	{
		lane_b_contact_point *data;
		long size;
		long capacity_and_flags;
	} points;
	char unknown_40[0x10];
	lane_b_contact_solver_data solver_data;
	lane_b_contact_property *properties;
	long property_count;
	long property_capacity;
	void remove(unsigned short id);
	long add(lane_b_contact_point **point, lane_b_contact_property **property);
};
#endif
