/* PROPS.H: the ai's props (src/unknown_25d690.cpp): what an actor knows of
   the objects around it.

   - "prop" (g_50241c): 0x100 elements of 0xc4 bytes, one per object an
     actor team knows of; its state starts at +0x58.
   - "prop_ref" (g_502418): 0x400 elements of 0x3c bytes, one per actor and
     prop, chained from the actor's first_prop_index.
   - "tracking" (g_502414): 0x64 elements of 0x124 bytes for the props an
     actor follows closely: a state at +4 and a view at +0x70. */

#ifndef PROPS_H
#define PROPS_H

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "slot_handler.h"
#include "object_queries.h"

/* the state of a prop as an actor perceives it (0x6c bytes) */
struct s_type_5cfb45
{
	long unknown00;
	point3f position;
	byte unknown10[0x28 - 0x10];
	s_location location;
	byte unknown30[0x3c - 0x30];
	long unknown3c;
	long unknown40;
	long unknown44;
	point3f unknown48;
	short unknown54;
	byte unknown56[2];
	bool unknown58;
	byte unknown59[0x5e - 0x59];
	bool unknown5e;
	bool unknown5f;
	bool unknown60;
	bool unknown61;
	bool unknown62;
	bool unknown63;
	bool unknown64;
	bool unknown65;
	bool unknown66;
	bool unknown67;
	byte unknown68;
	bool unknown69;
	byte unknown6a[2];
};
typedef char assert_type_5cfb45_size[(sizeof(s_type_5cfb45) == 0x6c) ? 1 : -1];

/* the view of a tracked prop (0xb4 bytes) */
struct s_type_f95cd3
{
	byte unknown00[6];
	short unknown06;
	byte unknown08[8];
	long unknown10;
	long unknown14;
	byte unknown18[0x2a - 0x18];
	bool unknown2a;
	byte unknown2b[0x39 - 0x2b];
	char unknown39;
	byte unknown3a[2];
	real unknown3c;
	byte unknown40[0x4c - 0x40];
	bool unknown4c;
	byte unknown4d[3];
	long unknown50;
	real unknown54;
	real unknown58;
	real unknown5c;
	real unknown60;
	bool unknown64;
	byte unknown65;
	short unknown66;
	bool unknown68;
	bool unknown69;
	byte unknown6a[2];
	bool unknown6c;
	bool unknown6d;
	byte unknown6e[2];
	short unknown70;
	byte unknown72[0x88 - 0x72];
	bool unknown88;
	byte unknown89;
	short unknown8a;
	short unknown8c;
	short unknown8e;
	short unknown90;
	byte unknown92[2];
	vector3f unknown94;
	byte unknowna0[2];
	bool unknowna2;
	byte unknowna3;
	point3f unknowna4;
	long unknownb0;
};
typedef char assert_type_f95cd3_unknown3c[((long)&((s_type_f95cd3 *)0)->unknown3c == 0x3c) ? 1 : -1];

/* a "prop" element (0xc4 bytes) */
struct s_type_76cf92
{
	short salt;
	byte unknown02[2];
	short unknown04;
	byte unknown06[2];
	long object_index;
	byte unknown0c[4];
	long unknown10;
	long next_index;
	long first_node_index;
	long actor_index;
	short team;
	bool unknown22;
	bool unknown23;
	bool unknown24;
	bool unknown25;
	byte unknown26[0x32 - 0x26];
	bool unknown32;
	bool unknown33;
	bool unknown34;
	byte unknown35;
	bool unknown36;
	byte unknown37;
	long unknown38;
	bool unknown3c;
	byte unknown3d[0x58 - 0x3d];
	s_type_5cfb45 state;
};
typedef char assert_type_76cf92_team[((long)&((s_type_76cf92 *)0)->team == 0x20) ? 1 : -1];
typedef char assert_type_76cf92_size[(sizeof(s_type_76cf92) == 0xc4) ? 1 : -1];

/* a "prop_ref" element (0x3c bytes): an actor's reference to a prop */
struct s_prop_datum
{
	short salt;
	byte unknown02[2];
	long actor_index;
	long prop_index;
	byte unknown0c[4];
	real unknown10;
	long tracking_index;
	short type;
	short unknown1a;
	short unknown1c;
	byte unknown1e[2];
	long object_index;
	short state;
	char unknown26;
	char unknown27;
	real unknown28;
	long next_index;
	byte unknown30[0x3c - 0x30];
};

/* the same element as function_25d740 takes it */
struct s_prop_node : s_prop_datum
{
};

/* a "tracking" element (0x124 bytes) */
struct s_type_e5ff81
{
	short salt;
	byte unknown02[2];
	s_type_5cfb45 state;
	s_type_f95cd3 view;
};

/* the prop types (g_470f10), indexed by s_prop_datum::type */
struct s_prop_type_entry
{
	short id;
	short unknown2;
	short unknown4;
	short kind;
	short unknown8;
	short unknowna[3];
};

extern s_prop_type_entry g_470f10[9];

/* the actor fields the prop code reads (the actor of g_4f55f0, 0x888 bytes) */
struct s_actor_prop_view
{
	byte unknown000[9];
	bool unknown009;
	byte unknown00a[0x58 - 0xa];
	long first_prop_index;
	long tracked_prop_indices[8];
	long unknown07c;
	byte unknown080[0x684 - 0x80];
	short unknown684;
	short unknown686;
	short unknown688;
	byte unknown68a[2];
	long unknown68c;
	byte unknown690[0x888 - 0x690];
};

inline s_type_76cf92 *prop_get(long prop_index)
{
	return (s_type_76cf92 *)(g_50241c->data + (prop_index & 0xffff) * sizeof(s_type_76cf92));
}

inline s_prop_datum *prop_ref_get(long prop_ref_index)
{
	return (s_prop_datum *)(g_502418->data + (prop_ref_index & 0xffff) * sizeof(s_prop_datum));
}

inline s_type_e5ff81 *tracking_get(long tracking_index)
{
	return (s_type_e5ff81 *)(g_502414->data + (tracking_index & 0xffff) * sizeof(s_type_e5ff81));
}

inline s_actor_prop_view *actor_prop_view_get(long actor_index)
{
	return (s_actor_prop_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_prop_view));
}

s_type_5cfb45 *function_25d690(s_prop_datum *datum);
s_type_f95cd3 *function_25d700(long index);
s_type_f95cd3 *function_25d740(s_prop_node *node);

#endif
