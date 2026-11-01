/* EFFECTS.H: the effects, their locations and the particle systems they
   start (src/unknown_175bd0.cpp, src/unknown_173b90.cpp) */

#ifndef EFFECTS_H
#define EFFECTS_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "data_array.h"
#include "globals.h"
#include "object_queries.h"

/* a marker an effect is placed at: a point, a direction and the marker's
   name (a string id) */
struct s_effect_marker
{
	point3f position;
	vector3f forward;
	dword name;
};

/* who caused an effect */
struct s_effect_owner
{
	long unknown0;
	long unknown4;
	short unknown8;
};

/* what an effect is started near (s_effect_parameters::source) */
struct s_effect_source
{
	byte unknown00[8];
	point3f position;
	byte unknown14[0xc];
	short index;
};

/* what a new effect is made from (0x60 bytes) */
struct s_effect_parameters
{
	union
	{
		dword flags;
		struct
		{
			dword attached : 1;
			dword flag1 : 1;
			dword flag2 : 1;
			dword colors_set : 1;
			dword : 28;
		};
	};
	long tag_index;
	long object_index;
	s_effect_owner owner;
	short unknown18;
	s_effect_marker *markers;
	long marker_count;
	vector3f velocity;
	long unknown30;
	long unknown34;
	long unknown38;
	short unknown3c;
	real scale_a;
	real scale_b;
	point3f const *origin;
	vector3f const *direction;
	long const *unknown50;
	dword color_a;
	dword color_b;
	s_effect_source *source;
};

/* a particle system of an effect event (0x38 bytes) */
struct s_effect_particle_system_definition
{
	byte unknown00[4];
	long tag_index;
	long location_index;
	short unknown0c;
	short placement;
	short unknown10;
	word location_mode;
	byte unknown14[2];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word : 12;
	byte unknown18[0x30 - 0x18];
	long unknown30;
	byte unknown34[4];
};

/* the effect tag ('effe') */
struct s_effect_part
{
	short create_in;
	short create_in_mode;
	short location;
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word : 11;
	dword base_group_tag;
	dword group_tag;
	long tag_index;
	real velocity_lower;
	real velocity_upper;
	real velocity_cone_angle;
	real angular_velocity_lower;
	real angular_velocity_upper;
	real radius_lower;
	real radius_upper;
	dword a_scales;
	dword b_scales;
};

/* a beam of an effect event (0x3c bytes) */
struct s_effect_beam
{
	byte unknown00[8];
	short location;
	byte unknown0a[0x3c - 0xa];
};

/* an acceleration an effect event applies to its object (0x14 bytes) */
struct s_effect_acceleration
{
	short create_in;
	short create_in_mode;
	short location;
	byte unknown06[2];
	real acceleration;
	real inner_cone_angle;
	real outer_cone_angle;
};

struct s_effect_event
{
	byte unknown00[4];
	real skip_chance;
	real delay_lower;
	real delay_upper;
	real duration_lower;
	real duration_upper;
	long part_count;
	s_effect_part *parts;
	long beam_count;
	struct s_effect_beam *beams;
	long acceleration_count;
	s_effect_acceleration *accelerations;
	long particle_system_count;
	s_effect_particle_system_definition *particle_systems;
};

struct s_effect_definition
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword : 26;
	short restart_event_index;
	short location_flags_low;
	real unknown08;
	long location_count;
	dword *locations;
	long event_count;
	s_effect_event *events;
	byte unknown1c[4];
	long looping_sound_tag_index;
	short looping_sound_location;
	short location_flags_high;
	real distance_lower;
	real distance_upper;
};

struct s_effect_event_slot
{
	long unknown0;
	long unknown4;
};

/* the effects (g_4ea93c, 0x190 bytes each) */
struct s_effect_datum
{
	short salt;
	union
	{
		word flags;
		struct
		{
			word flag0 : 1;
			word flag1 : 1;
			word flag2 : 1;
			word flag3 : 1;
			word flag4 : 1;
			word flag5 : 1;
			word flag6 : 1;
			word flag7 : 1;
			word flag8 : 1;
			word flag9 : 1;
			word flag10 : 1;
			word : 5;
		};
	};
	long tag_index;
	long looping_sound_index;
	long unknown0c;
	long unknown10;
	long unknown14;
	short unknown18;
	short unknown1a;
	s_location location;
	point3f origin;
	vector3f velocity;
	union
	{
		vector3f direction;
		struct
		{
			real unknown3c;
			long unknown40;
			long unknown44;
		};
	};
	long object_index;
	s_effect_owner owner;
	long unknown58;
	short event_index;
	short unknown5e;
	real unknown60;
	real event_delay;
	real unknown68;
	real scale_a;
	real scale_b;
	real unknown74;
	real unknown78;
	long location_indices[32];
	long first_particle_system_index;
	long last_particle_system_index;
	dword color_a;
	dword color_b;
	byte unknown10c[4];
	s_effect_event_slot event_slots[16];
};

/* the effect locations (g_4ea938, 0x3c bytes each): chained per marker of
   an effect */
struct s_effect_location_datum
{
	short salt;
	short node_index;
	long next_index;
	transform4x3f matrix;
};

/* the particle systems (g_510c74, 0x54 bytes each) */
struct s_particle_system_datum
{
	short salt;
	short unknown02;
	real unknown04;
	real unknown08;
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word : 5;
	word definition_index;
	long tag_index;
	long event_index;
	long effect_index;
	s_location location;
	long unknown24;
	real random[2];
	long location_index;
	long unknown34;
	long next_index;
	long previous_index;
	long first_child_index;
	long last_child_index;
	s_particle_system_datum *parent;
	long unknown4c;
	dword color;

	void set_location(s_location const *location);
	struct s_effect_particle_system_definition *function_1751d0();
};

/* the particle locations (g_51ec8c, 0x34 bytes each) */
struct s_particle_location_datum
{
	short salt;
	short unknown02;
	byte unknown04[8];
	long next_index;
	point3f position;
	byte unknown1c[0x34 - 0x1c];
};

extern s_record_pool *g_4ea93c;
extern s_record_pool *g_4ea938;
extern s_record_pool *g_510c74;
extern s_record_pool *g_51ec84;
extern s_record_pool *g_51ec88;
extern s_record_pool *g_51ec8c;

#define DATUM(array, type, index) ((type *)((array)->data) + ((index) & 0xffff))

/* the definition (and the group) of a tag */
#define TAG_GET(type, index) ((type *)g_4e3b44[(index) & 0xffff].bytes)
#define TAG_GROUP(index) (*(dword *)&g_4e3b44[(short)(index)])

/* the datum of an index that may be stale (the salt is checked) */
static inline byte *record_pool_lookup_checked(s_record_pool *data, long datum_index)
{
	byte *result = 0;

	if (datum_index != NONE)
	{
		long index = datum_index & 0xffff;

		if (index < data->high_water_index)
		{
			byte *datum = data->data + data->size * index;

			if (*(short *)datum != 0 && *(short *)datum == (datum_index >> 16))
				result = datum;
		}
	}
	return result;
}

/* function_16b790 (0x16b790), which retail inlines into the lifecycle
   callbacks */
static inline void data_make_valid_inlined(s_record_pool *data)
{
	data->valid = true;
	record_pool_release_all(data);
}

/* the particle systems' lifecycle steps, which retail inlines into the
   effects' */
static inline void function_x496c75(void)
{
	data_make_valid_inlined(g_510c74);
	data_make_valid_inlined(g_51ec84);
	data_make_valid_inlined(g_51ec88);
	data_make_valid_inlined(g_51ec8c);
}

static inline void function_x3bc618(void)
{
	g_51ec8c->valid = false;
	g_51ec88->valid = false;
	g_51ec84->valid = false;
	g_510c74->valid = false;
}

/* the particle system tags: the objects at 0x479868 and 0x479874 that
   function_137bd0 (unknown_0e4050.cpp) picks by the group of a tag */
class c_type_4e7709
{
public:
	virtual void v0() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5() {}
	virtual void v6() {}
	virtual struct s_effect_particle_system_definition *function_1751d0(word index) { return 0; }
	virtual void v8() {}
	virtual void v9() {}
	virtual long const *function_x947334() { return 0; }
	virtual void render(s_particle_system_datum *system, void *a, void *b, void *c) {}
	virtual void v12() {}
	virtual void v13() {}
	virtual void v14() {}
	virtual void v15() {}
	virtual void v16() {}
	virtual bool multiplied() { return false; }
	virtual bool tinted() { return false; }
	virtual void v19() {}
	virtual void initialize(long tag_index) {}
};

c_type_4e7709 *function_137bd0(long tag_index);

extern c_type_4e7709 g_479868;
extern c_type_4e7709 g_479874;

/* the particle system object for a tag's group, pointed at the tag (null for
   other groups); function_137bd0 is its out-of-line copy */
inline c_type_4e7709 *particle_object_for_tag(long tag_index)
{
	c_type_4e7709 *result = 0;

	switch (TAG_GROUP(tag_index))
	{
	case 'prt3':
		result = &g_479868;
		break;
	case 'PRTM':
		result = &g_479874;
		break;
	}
	if (result)
		result->initialize(tag_index);
	return result;
}

/* what a particle system spawns from (function_178c80) */
struct s_particle_system_spawn
{
	real scale;
	real unknown;
	long location_index;
};

/* particles (src/unknown_173b90.cpp) */
void function_173de0(void);
void particle_systems_update_locations(void);
void __stdcall function_174180(long particle_system_index);
void particle_system_unlink(s_particle_system_datum *particle_system, long *first_index, long *last_index);
void particle_system_link(s_particle_system_datum *particle_system, long *first_index, long *last_index);
long function_173fd0(struct s_effect_particle_system_definition *definition, long effect_index, long tag_index, short definition_index, long event_index);
void function_175a80(dword color_a, dword color_b, s_particle_system_datum *particle_system, bool multiplied, bool tinted);
void function_175270(s_particle_system_datum *particle_system, s_particle_system_spawn *spawn, transform4x3f const *matrix, bool field_b4);

/* effects (src/unknown_175bd0.cpp) */
void function_1774f0(long effect_index);
void effect_remove_event_slot(long effect_index, long value);
long function_176780(s_effect_owner const *owner, long object_index, real scale_a, long tag_index, real scale_b, point3f const *origin, vector3f const *direction);
/* the object-first argument order; the definition takes the owner first, as retail passes it */
inline long function_176780(long object_index, s_effect_owner const *owner, real scale_a, long tag_index, real scale_b, point3f const *origin, vector3f const *direction)
{
	return function_176780(owner, object_index, scale_a, tag_index, scale_b, origin, direction);
}

#endif
