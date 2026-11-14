// @flags /O2 /arch:SSE /Gr
/* UNIT_OBJECT_TYPE.CPP: the unit object type

The unit object type's callbacks (its definition at 0x4679b0) and the unit
functions around them: unit placement, creation, update, seats, inventory,
weapons and grenades, melee, aiming, custom animations and scripting. The
functions upstream already has stay in their files (units.cpp,
unknown_0c7070.cpp, unknown_0c86e0.cpp, unknown_0c8880.cpp,
unknown_0cafc0.cpp, unknown_0cbd50.cpp, unknown_0cc2b0.cpp,
unknown_0cd660.cpp, unknown_0d0690.cpp, unknown_0d0e00.cpp). */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "bit_vector.h"
#include "units.h"
#include "unit_requests.h"
#include "object_markers.h"
#include "unknown_1c62f0.h"
#include "unknown_0d0690.h"
#include "sound_sources.h"
#include "object_iterator.h"
#include "unknown_1cec30.h"
#include "object_queries.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include "vector_products.h"
#include "object_tag_definitions.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

/* the last 0x24 bytes of a unit state (0xc6ef0), kept at the unit's +0x1c8 */
struct s_unit_state_tail
{
	long unknown00;
	long unknown04;
	long unknown08;
	byte unknown0c[0x18 - 0xc];
	short unknown18;
	byte unknown1a[2];
	real unknown1c;
	real unknown20;
};

/* clears a unit state tail: its floats, NONE indices and +0x18 */
inline void unit_state_tail_clear(s_unit_state_tail *tail)
{
	tail->unknown1c = 0.0f;
	tail->unknown20 = 0.0f;
	tail->unknown00 = NONE;
	tail->unknown04 = NONE;
	tail->unknown08 = NONE;
	tail->unknown18 = 0;
}

/* the unit (the object fields, then the unit's own; the fields read here) */
struct s_unit
{
	long definition_index;
	dword object_flags;
	byte unknown008[4];
	long next_sibling_index;
	long first_child_index;
	long parent_index;
	byte unknown018[0x28 - 0x18];
	long unknown028;
	long unknown02c;
	point3f unknown030;
	byte unknown03c[0x64 - 0x3c];
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	byte unknown094[0xaa - 0x94];
	byte type;
	byte unknownab[0xb4 - 0xab];
	long unknownb4;
	byte unknownb8[0xc0 - 0xb8];
	word unknownc0;
	short unknownc2;
	long unknownc4;
	long unknownc8;
	byte unknowncc[0xd4 - 0xcc];
	long unknown0d4;
	byte unknownd8[0xec - 0xd8];
	real unknownec;
	real unknownf0;
	real unknownf4;
	real unknownf8;
	real unknownfc;
	real unknown100;
	byte unknown104[0x10a - 0x104];
	byte flags_10a;
	byte unknown10b[0x12a - 0x10b];
	short animation_offset;
	long actor_index;
	long unknown130;
	dword flags_134;
	short unknown138;
	byte unknown13a[2];
	long unknown13c;
	long unknown140;
	byte unknown144[4];
	long unknown148;
	long unknown14c;
	vector3f unknown150;
	vector3f unknown15c;
	vector3f unknown168;
	vector3f unknown174;
	vector3f unknown180;
	real unknown18c;
	real unknown190;
	byte unknown194[4];
	vector3f unknown198;
	vector3f unknown1a4;
	vector3f unknown1b0;
	char unknown1bc;
	byte unknown1bd[3];
	long unknown1c0;
	long unknown1c4;
	s_unit_state_tail unknown1c8;
	long unknown1ec;
	long unknown1f0;
	byte unknown1f4;
	byte unknown1f5;
	char unknown1f6;
	char unknown1f7;
	byte unknown1f8;
	byte unknown1f9;
	byte unknown1fa;
	byte unknown1fb;
	short parent_seat_index;
	byte unknown1fe[0x208 - 0x1fe];
	real unknown208;
	byte unknown20c[0x210 - 0x20c];
	short unknown210;
	char current_weapon_index;
	char next_weapon_index;
	short unknown214;
	char unknown216;
	char unknown217;
	long weapon_object_indices[4];
	long unknown228[4];
	long unknown238;
	char current_grenade_index;
	char next_grenade_index;
	char grenade_counts[2];
	char unknown240;
	char unknown241;
	byte unknown242;
	byte unknown243;
	long unknown244;
	long unknown248;
	long unknown24c;
	long unknown250;
	long unknown254;
	byte unknown258;
	byte unknown259;
	short unknown25a;
	real unknown25c[2];
	real unknown264;
	real unknown268;
	real unknown26c;
	point3f unknown270;
	vector3f unknown27c;
	vector3f unknown288;
	vector3f unknown294;
	long unknown2a0;
	short unknown2a4;
	byte unknown2a6[2];
	long unknown2a8;
	long unknown2ac;
	real unknown2b0;
	real unknown2b4;
	real unknown2b8;
	char unknown2bc;
	byte unknown2bd;
	short unknown2be;
	byte unknown2c0[0x2c4 - 0x2c0];
	real unknown2c4;
	word unknown2c8;
	short unknown2ca;
	real unknown2cc;
	long unknown2d0;
	long unknown2d4;
	real unknown2d8;
	real unknown2dc;
	long unknown2e0;
	real unknown2e4;
	short unknown2e8;
	short unknown2ea;
	long unknown2ec;
	byte unknown2f0[0x330 - 0x2f0];
	byte unknown330[4];
	real unknown334;
	real unknown338;
	short unknown33c;
	short unknown33e;
	short unknown340;
	short unknown342;
	short unknown344;
	short unknown346;
	byte flags_348;
};

/* the 0x14 bytes of camera data a seat (+0x24) or a unit definition
   (+0xf0) holds */
struct s_unit_camera_data
{
	real unknown00;
	real unknown04;
	real unknown08;
	real unknown0c;
	real unknown10;
};

/* the field offsets the retail code reads */
#define UNIT_OFFSET_CHECK(field, offset) typedef char unit_offset_check_##field[offsetof(s_unit, field) == (offset) ? 1 : -1]
UNIT_OFFSET_CHECK(unknown030, 0x30);
UNIT_OFFSET_CHECK(position, 0x64);
UNIT_OFFSET_CHECK(linear_velocity, 0x88);
UNIT_OFFSET_CHECK(type, 0xaa);
UNIT_OFFSET_CHECK(unknown25c, 0x25c);
UNIT_OFFSET_CHECK(unknown208, 0x208);
UNIT_OFFSET_CHECK(unknown243, 0x243);
UNIT_OFFSET_CHECK(unknown2c4, 0x2c4);
UNIT_OFFSET_CHECK(unknown268, 0x268);
UNIT_OFFSET_CHECK(unknown26c, 0x26c);
UNIT_OFFSET_CHECK(unknown174, 0x174);
UNIT_OFFSET_CHECK(unknownc4, 0xc4);
UNIT_OFFSET_CHECK(unknown100, 0x100);
UNIT_OFFSET_CHECK(unknown198, 0x198);
UNIT_OFFSET_CHECK(unknown1fa, 0x1fa);
UNIT_OFFSET_CHECK(unknown1f8, 0x1f8);
UNIT_OFFSET_CHECK(unknownb4, 0xb4);
UNIT_OFFSET_CHECK(unknownf8, 0xf8);
UNIT_OFFSET_CHECK(unknown0d4, 0xd4);
UNIT_OFFSET_CHECK(flags_10a, 0x10a);
UNIT_OFFSET_CHECK(actor_index, 0x12c);
UNIT_OFFSET_CHECK(unknown13c, 0x13c);
UNIT_OFFSET_CHECK(unknown180, 0x180);
UNIT_OFFSET_CHECK(unknown1c8, 0x1c8);
UNIT_OFFSET_CHECK(unknown1ec, 0x1ec);
UNIT_OFFSET_CHECK(parent_seat_index, 0x1fc);
UNIT_OFFSET_CHECK(weapon_object_indices, 0x218);
UNIT_OFFSET_CHECK(unknown238, 0x238);
UNIT_OFFSET_CHECK(grenade_counts, 0x23e);
UNIT_OFFSET_CHECK(unknown2a0, 0x2a0);
UNIT_OFFSET_CHECK(unknown2bc, 0x2bc);
UNIT_OFFSET_CHECK(unknown2d8, 0x2d8);
UNIT_OFFSET_CHECK(unknown2dc, 0x2dc);
UNIT_OFFSET_CHECK(unknown294, 0x294);
UNIT_OFFSET_CHECK(unknown2ec, 0x2ec);
UNIT_OFFSET_CHECK(unknown330, 0x330);
UNIT_OFFSET_CHECK(unknown140, 0x140);
UNIT_OFFSET_CHECK(unknown1f6, 0x1f6);
UNIT_OFFSET_CHECK(unknown25a, 0x25a);
UNIT_OFFSET_CHECK(unknown2e0, 0x2e0);
UNIT_OFFSET_CHECK(unknown342, 0x342);
UNIT_OFFSET_CHECK(unknown2e8, 0x2e8);
UNIT_OFFSET_CHECK(unknown346, 0x346);
UNIT_OFFSET_CHECK(flags_348, 0x348);

/* the unit's animation state, at the offset +0x12a holds */
struct s_unit_animation
{
	long unknown00;
	byte unknown04[2];
	short unknown06;
	byte unknown08[0x68 - 0x8];
	long unknown68;
	byte unknown6c[0x7c - 0x6c];
	long name;
};

struct s_unit_header
{
	byte unknown00[8];
	s_unit *unit;
};

#define UNIT_GET(index) (((s_unit_header *)g_4e0300->data)[(index) & 0xffff].unit)
#define UNIT_DEFINITION_GET(unit) (g_4e3b44[(unit)->definition_index & 0xffff].bytes)
#define UNIT_SEAT_COUNT(definition) (*(long *)((definition) + 0x1c8))
#define UNIT_SEATS(definition) (*(s_unit_seat_definition **)((definition) + 0x1cc))
#define UNIT_ANIMATION(unit) ((s_unit_animation *)((byte *)(unit) + (unit)->animation_offset))

/* UNIT_GET with the index masked before the header array is read: the
   lookups that address the header as offset + array (0xcea00, 0xce0c0) */
static inline s_unit *unit_at_masked_index(long unit_index)
{
	long index = unit_index & 0xffff;
	s_unit_header *headers = (s_unit_header *)g_4e0300->data;

	return headers[index].unit;
}

/* a float rounded to an integer as the x87 does (fld, fistp) */
__forceinline long unit_round(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

/* the units among an object's children, with their seats (as damage.cpp) */
struct s_unit_child_iterator
{
	long object_index;
	long unit_index;
	short seat_index;
	long next_index;
};

struct s_damage_object;

void function_ccab0(long unit_index);
void __stdcall function_cc810(long vehicle_index);
bool function_c48f0(long unit_index);
bool __stdcall function_c49b0(long unit_index);
bool __stdcall function_c58f0(long unit_index);
bool __stdcall function_c60c0(long unit_index);
bool function_c5eb0(long unit_index);
bool function_c6740(long unit_index);
void function_114c60(long unit_index);
void function_c6990(long unit_index);
void function_c6810(long unit_index);
void function_c50a0(long unit_index);
bool function_0c7070(long unit_index);
void function_b8b70(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_db5c0(long object_index);
void function_e4bd0(long arg_159e6d_2);
void __stdcall function_b8540(long object_index);
void function_ce920(long unit_index, long slot_index, long mode, bool flag);
bool function_e4050(long object_index);
bool function_e6900(long unit_index, s_unit_request *request);
void function_ccf20(long unit_index);
void function_b58c0(long index, dword mask);
void function_c8bb0(long unit_index, long vehicle_index, long *object_index, short *seat_index, short *priority,
	real *distance, bool *flag);
bool function_d1080(long unit_index, transform4x3f *matrix, s_unit_camera_data *camera);
void function_e69c0(long unit_index, long type);
void function_114240(long unit_index);
void function_a94b0(long unit_index);
bool function_a9440(long unit_index, long player_index);
void function_a7bc0(long unit_index);
bool function_a9500(long unit_index, long index);
bool function_101640(long weapon_index);
bool function_e68c0(long type, long unit_index);
struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *data);
void function_101980(long weapon_index, short rounds_loaded, short magazine_index, short rounds_total);
void function_10cdf0(long object_index);
void function_10ca80(long object_index, long a);
void function_ce6b0(long unit_index, long name, long object_index, real scale);
short function_101010(long weapon_index, short field_240);
bool function_100fd0(long weapon_index);
bool function_10f630(long object_index, long *first, long *second);
void function_b7360(long object_index);
bool function_c9040(long vehicle_index, long unit_index, short seat_index, point3f const *position,
	vector3f const *forward);
long function_1e4a10(long index);
void function_c5740(long unit_index, bool start);
void function_1520f0(long player_index);
short __stdcall function_cdeb0(long unit_index, long state_name, short slot, short direction);
bool function_100f00(long weapon_index);
bool function_1e3370(long actor_index, void *unknown);
void function_10cec0(long weapon_index, long unit_index, long parent_marker_name, long marker_name);
void function_1c9c80(long object_index, long unknown2d0, word unknown2c8, real unknown2cc, long a, bool b);
bool unit_has_weapon_definition(long unit_index, long definition_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
void function_10cd50(long weapon_index);
bool unit_action_active(long unit_index, long action_type);
byte function_10fcd0(long unit_index, long unknown, long state_name, long action_name);
bool function_100880(long weapon_index, long magazine_index);
bool function_10f930(long object_index, real time, bool from_end);
/* who is responsible for damage (damage.cpp) */
struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
	short unknown0a;
};

/* a damage event (0x88 bytes, damage.cpp) */
struct s_type_1e6529
{
	long definition_index;
	union
	{
		byte unknown04[4];
		dword flags;
	};
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	long unknown1c;
	short unknown20;
	short unknown22;
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real unknown54;
	real unknown58;
	real unknown5c;
	real distance;
	real distance_scale;
	bool in_unknown_radius;
	byte unknown69[3];
	vector3f cone_direction;
	real unknown78;
	short material_index;
	short unknown7e;
	byte unknown80[4];
	bool unknown84;
	byte unknown85[3];
};

/* what damage did to an object (damage.cpp) */
struct s_damage_report
{
	byte unknown00;
	byte unknown01[3];
	dword flags;
	long definition_index;
	s_damage_owner owner;
	vector3f direction;
	point3f origin;
	byte unknown30[4];
	real scale;
	real unknown38;
	long unknown3c;
	short unknown40;
	byte unknown42[2];
	real unknown44;
	real unknown48;
	real distance;
	long unknown50;
};

extern s_damage_owner const *g_467420;
void __stdcall function_d6800(long object_index, s_damage_owner const *owner, bool notify_parent, bool unknown);
void function_d6a70(long object_index);
void __stdcall function_caa60(long unit_index, long object_index, point3f const *point, bool knocked, bool force);
void __stdcall function_bd020(long object_index);
bool __stdcall function_c98a0(point2f const *direction, long unit_index, long type, short value);
void function_1e9070(long player_index);
void function_a7a30(long object_index, dword mask);
void function_a8950(long unit_index, long definition_index);
long unit_get_player_index(long unit_index);
void function_f8110(long equipment_index);
extern bool g_4f55dc[16];
struct s_game_allegiance_globals;
extern s_game_allegiance_globals *g_4f55ec;
bool function_0bfe60(const dword *flags, long bit);
struct s_time_entry;
struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;
void function_1e6980(s_time_entry *entries, short a, byte b);

/* a unit request type's handlers (unknown_0e6900.cpp) */
typedef bool (__stdcall *t_unit_request_proc)(long unit_index, s_unit_request *request);
typedef bool (__stdcall *t_unit_request_update_proc)(long unit_index, long type);
typedef void (__stdcall *t_unit_request_end_proc)(long unit_index, long type);

struct s_unit_request_definition
{
	t_unit_request_proc perform;
	t_unit_request_update_proc update;
	t_unit_request_end_proc finished;
	t_unit_request_end_proc interrupted;
};

extern s_unit_request_definition *g_4677c8[60];
point3f *function_b9dd0(long object_index, point3f *result);
struct s_small_index;
short function_0b67a0(const s_small_index *data);
bool havok_component_rigid_body_keyframed(long rigid_body_index, s_havok_component *component);
struct s_vehicle_ray;
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray, long ignore_object_index, long ignore_unit_index);
extern const long g_467430[5];
bool function_10f9b0(long unit_index, long state_name, long action_name, long a, transform4x3f *matrix, bool flag);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_15e050(long object_index, short value);
void __stdcall function_153d10(long team, long definition_index, void *a, void *b, long c, real d, real e, bool f);
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner);
void function_107370(long device_index, real value);
void function_184060(long unknown3c, long unknown59, s_type_1e6529 *data, long unknown50);
void function_cafc0(long unit_index, point3f *position);
real function_11cf50(vector3f const *a, vector3f const *b);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
void function_b9a90(long object_index);
long function_11bf90(long object_index, point3f *point);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
	s_location const *location, bool unknown);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, bool d);
real function_30bf0(vector3f *v);
void __stdcall function_def60(point3f *point, long arg_159e6d_2, short mode, point3f const *origin,
	vector3f const *forward, real const *offsets);
void function_f1070(long vehicle_index, long *location, long *unknown3c0, point3f *point, long *unknown3c8,
	long *unknown3cc);
bool function_101b80(long weapon_index, short barrel_index, point3f *point);
void function_ce0c0(long unit_index);
struct s_damage_report;
bool function_10f340(long unit_index, long mode, long set);
void function_e3f00(long arg_159e6d_2);
real function_d1210(long object_index);
bool __stdcall function_ff5f0(long weapon_index, long name, real *value, bool *active);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
void matrix4x3_from_forward_and_up(transform4x3f *out, vector3f const *forward, vector3f const *up);
extern vector3f *g_4687b8;
extern vector3f *g_4687bc;
void __stdcall function_b8ee0(long parent_index, long marker_name, long object_index, long a);
void function_10b360(long object_index);
void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle);
void function_10cf80(vector3f const *impulse, long item_index, bool flag);
point3f *function_b9ef0(long object_index, point3f *result);
bool __stdcall function_bc1d0(long object_index, point3f const *point, long ignore_unit_index);

/* the globals of 0x5107e8 (unknown_29f5b0.cpp) */
struct s_5107e8
{
	bool flag0;
	byte unknown01[3];
	long time4;
	bool flag8;
};

extern s_5107e8 *g_5107e8;
void function_1bbdf0(long vehicle_index);
long havok_component_new(long object_index);
void havok_component_delete(long component_index);
void function_1cf120(long component_index);
void function_1d1540(s_havok_component *component);
void __stdcall function_b9b90(long object_index, bool disable);
bool function_1d48f0(point3f *arg_0, s_havok_component *component, long rigid_body_index, long type, point3f const *target,
	vector3f const *offset, s_location *location, real a5, real radius, bool a7, point3f const *root_point,
	long root_index);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
vector3f *function_11d000(vector3f const *v, vector3f *out);
long function_baf40(long object_index);
bool function_16a7c0(point3f const *from, point3f const *to, long ignore_object_index, long ignore_unit_index,
	point3f *result);
bool function_1cb920(void *data, long mode);
bool function_1c9500(long unit_index, long actor_index, bool notify);
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *a, bool *b);
bool function_1df560(short team_a, short team_b);
bool function_138880();
void *function_1e5240(long actor_index);
long function_cbd50(long unit_index, short weapon_index);
struct s_collision_result_1697c0;
bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
	long ignore_unit_index, s_collision_result_1697c0 *result);
s_location *object_get_root_location(long object_index, s_location *location);
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index);
short *function_1886d0(long object_index, short *material_type);
struct s_globals_element;
s_globals_element *function_188690(short index);
bool function_101380(long weapon_index);
bool function_1013e0(long weapon_index);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector, long ignore_object_index,
	long ignore_unit_index, s_collision_result_1697c0 *result);
bool function_bc380(long object_index, long block_offset, short size, long a);
void __stdcall function_10f260(long unit_index);
void function_114ec0(long unit_index, long a);
bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);
void function_a8a30(long unit_index, long definition_index);
real normalize2d(point2f *v);
real function_11cc90(vector2f const *a, vector2f const *b);
/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
s_unit_action_state function_e70b0(long unit_index);
bool function_10ff40(long unit_index, long type, short side, short value, bool *flag, short *side_out, short *value_out);
void function_ba350(long object_index, real seconds);
bool function_101240(long weapon_index);
real function_10f7f0(long object_index);
real function_10f690(long object_index, real *duration);
bool function_a7670(long object_index);
void __stdcall function_d6bc0(long object_index);
void function_edfa0(long unit_index, point2f const *facing);
void __stdcall function_e4a20(long arg_159e6d_2, long object_index, point3f const *point, bool knocked);
void function_14cad0(long player_index, long unit_index);
void function_152340(void);
void function_1e2a90(long actor_index);
void function_d0e00(long unit_index, real rate);
void function_114e80(long unit_index);
void function_100430(long weapon_index, long value, real amount);
bool function_106030(long weapon_index);
bool function_101d20(long weapon_index);
void function_1509e0(long player_index, long weapon_index, bool *modes);
void function_1060a0(long weapon_index, long unit_index);
struct s_juggernaut_globals;
extern s_juggernaut_globals *g_510c9c;
long function_113da0(long unit_index);
void function_113d20(long unit_index, real time);
long function_113df0(long unit_index);
void function_113d60(long unit_index, real time);
bool __stdcall function_110ab0(long unit_index);
real function_11ce20(vector3f const *a, vector3f const *b);
real function_1201a0(vector3f *v, vector3f const *fallback);
void function_11f0d0(vector3f *position, vector3f *forward, vector3f const *target, real rate, real max_angle, real scale);
void function_bba20(long object_index);
long function_176780(long object_index, s_effect_owner const *owner, real scale_a, long tag_index, real scale_b,
	point3f const *origin, vector3f const *direction);
void function_15cbf0(long attacker_index, long player_index, bool flags);
bool function_1147e0(long unit_index, bool a, real b, real c, long definition_index, bool hard);
void function_c86e0(long unit_index, bool keep_weapon_zoom);
void function_1c95d0(long unit_index, long attacker_index, short type, real amount);
void function_1c9e10(long unit_index, vector3f const *direction, real shake);
void function_c7840(transform4x3f const *frame, vector3f *current, vector3f const *desired, real rate, vector3f *velocity,
	real const *limits, real arg_3097c5_2, real pitch_rate);
bool __stdcall function_10f430(long unit_index, long mode, long weapon_class, long weapon_type, long set, real blend,
	bool force, long flags);
bool function_1012c0(long weapon_index);
long function_baf80(long object_index);
struct s_location;
#include "impacts.h"
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	c_type_47f957 material, long effect_value, s_location const *location, real scale);
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction,
	long tag_index);


void function_d6660(s_type_1e6529 *data, long definition_index);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);

struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

long function_189210(long object_index, s_sound_label_play const *play, long marker_name);

/* sends the unit request 8 between two updates of the unit */
// @retail 0xc4820
void __stdcall function_c4820(long unit_index)
{
	s_unit_request request;

	function_ccab0(unit_index);
	memset(&request, 0, sizeof(request));
	request.type = 8;
	function_e6900(unit_index, &request);
	function_cc810(unit_index);
}

/* whether nothing holds the unit back: false when +0x13c is set and the
   unit has no parent, or is in a third-person seat of it */
// @retail 0xc4970
bool function_c4970(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = true;

	if (unit->unknown13c != NONE && (unit->parent_index == NONE || function_c48f0(unit_index)))
	{
		result = false;
	}
	return result;
}

/* plays one of the globals' sounds at the object's marker 0x4000095 */
// The last two instructions to match were the marker immediate (eax) and the play record's address (edi):
// callers set up register arguments last to first, so 0x189210 takes the play record before the marker name.
// @retail 0xc5890
void function_c5890(long object_index, short index)
{
	long tag_index = *(long *)(*(byte **)((byte *)g_4e034c + 0x134) + index * 8 + 0xe8);

	if (tag_index != NONE)
	{
		s_sound_label_play play;

		play.label = NONE;
		play.tag_index = tag_index;
		play.scale = 1.0f;
		play.variant = 0;
		function_189210(object_index, &play, 0x4000095);
	}
}

/* the unit's per-tick checks: whether any of them changed something */
// @retail 0xc6b30
bool __stdcall function_c6b30(long unit_index)
{
	bool changed = function_c49b0(unit_index);

	changed |= function_c58f0(unit_index);
	changed |= function_c60c0(unit_index);
	changed |= function_c5eb0(unit_index);
	changed |= function_c6740(unit_index);
	function_114c60(unit_index);
	function_c6990(unit_index);
	function_c6810(unit_index);
	function_c50a0(unit_index);
	return changed;
}

// @retail 0xc6f80
void __stdcall function_c6f80(long unit_index, long a, long b)
{
	s_unit *unit = UNIT_GET(unit_index);

	unit->unknown1ec = a;
	unit->unknown1f0 = b;
}

/* whether the unit's animation is one of three (or 0x0c7070 holds) */
// @retail 0xc70b0
bool function_c70b0(long unit_index)
{
	bool result = function_0c7070(unit_index);

	if (!result)
	{
		long name = UNIT_ANIMATION(UNIT_GET(unit_index))->name;

		if (name == 0x80000c4 || name == 0x400004a || name == 0x50000c3)
		{
			result = true;
		}
	}
	return result;
}

/* the last object of the chain the units' +0x24c links. The link is tested
   through the unit and the result read through the pointer, as retail reloads it */
// @retail 0xc7100
long function_c7100(long unit_index)
{
	long result = NONE;
	s_unit *unit = UNIT_GET(unit_index);
	long *next = &unit->unknown24c;
	long value = *next;

	while (value != NONE)
	{
		result = *next;
		unit = UNIT_GET(result);
		value = unit->unknown24c;
		next = &unit->unknown24c;
	}
	return result;
}

// @retail 0xc8860
short function_c8860(long unit_index)
{
	return UNIT_GET(unit_index)->unknown240;
}

/* whether the unit, a biped, has bit 4 of +0x348 set */
// @retail 0xc8a10
bool function_c8a10(long unit_index)
{
	bool result = false;
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->type == 0)
	{
		result = (unit->flags_348 >> 4) & 1;
	}
	return result;
}

/* the number of seats function_c8a40 lists for the object */
// @retail 0xc8b80
short __stdcall function_c8b80(long object_index, s_object_seat *seats, short maximum_count)
{
	short count = 0;

	function_c8a40(object_index, seats, &count, maximum_count);
	return count;
}

/* clears the unit's flag 2 at +0x10a and its state that depends on it */
// score 13 (was 13): the final call receives the unit in EAX rather than retail ESI and becomes a tail
// jump, and `1 << type` lands in EDX (retail EAX). BLOCKED-BY 0xe4bd0, which inlines 0xe5930 where retail
// calls it (BLOCKED-BY 0xe5930: only a noinline declaration beside its definition keeps it out). Existing indexed/both byte helpers 22/22; state pointer 22; subscript 23;
// explicit state assignment 23; native state flags via pointer/at use 22/23;
// native flags with masked lookup 22. Round 89: the animation flags cleared as the word they are
// (bipeds.cpp's `word const *animation_flags`) 13, kept (retail's `and byte [eax+edi], 0xfb` and dead
// `add eax, edi`); a `word *animation_flags` local 10 (matches once 0xe4bd0 is called as retail does,
// see 0xe4bd0's note); on the word clear, a word subscript/`0xfffb`/offset first/short 13 each, dword 23.
// Round 94: `unit->type == 0` or `!unit->type` for the mask test 12 (retail's length, not its shl/test);
// 0xe4bd0's flag test as a field bit 13; a noinline declaration of 0xe4bd0 13; inline depth 0 around
// 0xe4bd0 (diagnostic) gives it retail's 61 bytes but inlines it here (86).
// Round 37 (fast): the mask test written `!= 0`, `(long)` mask, `1 & mask`, `1u << type` 13 each; the
// unit re-read for the test 58. Still BLOCKED-BY 0xe4bd0 (src/bipeds.cpp, held by another lane this
// round), whose retail ESI convention needs 0xe5930 called out of line.
// Round 281: masked lookup with native word clear 13; existing word setter 10; both native setters 10;
// computed type mask 13; boolean predicate 10; word setter with early guard/void return 10/10;
// masked lookup, word setter and predicate 9; minimal word setter and predicate 9 (both 92 bytes).
// Both forms scoring 9 lose 0xc2d00 (0 -> 2) in ordinary full links; rejected and original body restored.
// Round 38 (lane 17, plain): 0xe4bd0 declared first among the callees 13; declared just before this function
// 13; the mask test `== 1` 13; flag 2 cleared through the file's s_unit_flags_10a_view bit (struct moved up)
// 13; an early return before the 0xe4bd0 call 13. Still BLOCKED-BY 0xe4bd0 (its file held by other lanes).
// Round 97 (fast): existing bitfield view for the first clear 13; literal 0xfb 13; explicit first clear 13;
// animation flag reference 10 (NOT kept: one-use reference and gain below four); named type predicate 10
// (NOT kept: narrow shift and gain below four); explicit return 13; early guard 13; const unit pointer 13;
// final parameter read through its address 13. Original body restored.
// Round 38 (lane 17, plain): 0xe4bd0 declared first among the callees 13; declared just before this function
// 13; the mask test `== 1` 13; flag 2 cleared through the file's s_unit_flags_10a_view bit (struct moved up)
// 13; an early return before the 0xe4bd0 call 13. Still BLOCKED-BY 0xe4bd0 (its file held by other lanes).
// Round 82 (lane 11, fast): 0xe4bd0 declared after 0xbd020 (the move that gave 0xc98a0 retail's convention) 13; after
// 0xdb5c0 13; 0xe5930 declared before it 13; the word pointer local with the move 10 (not kept: one-use local, no match);
// the unit re-read for the type test 58 (repeat). BLOCKED-BY 0xe4bd0 (src/bipeds.cpp held this round) and 0xe5930.
// Round 112 (m3 lane 16, plain with 0xe4bd0 and 0xc2d00): `inline` on 0xe4bd0's definition 13 (same code); the existing
// word_flags_set for the +0x33e clear with the mask test plain or `!= 0` 10/10; with unit_at_masked_index 10; with
// byte_flags_set for the first clear too 10; all three existing helpers 10 (each 95 bytes: the mask temp takes EAX as in
// retail and only the 0xe4bd0 tail call is left; 0xc2d00 flips to 2, the accepted link flip). Round 281's 9 adds a
// named bool used once for the type test, which is kept only on a match. Still BLOCKED-BY 0xe4bd0 / 0xe5930.
// M3 lane 11 r97 (fast with 0xe4bd0, 91 throughout): the type in a named local for the mask test 13; the unit as a byte
// pointer with raw offsets 13; the mask spelled `(1 << 0)` 13; the animation-state offset in a named short 13; the type
// test inverted with an early return ahead of the 0xe4bd0 call 13 (all the same code). Still BLOCKED-BY 0xe4bd0
// (src/bipeds.cpp, held by another lane this round) and BLOCKED-BY 0xe5930: retail calls 0xe5930 from all six of its
// callers, ours expands it inside 0xe4bd0 (113 bytes against 61), which gives 0xe4bd0 the EAX convention.
// @retail 0xcaf00
void function_caf00(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	unit->flags_10a &= ~4;
	function_b8b70(unit_index);
	function_bb950(unit_index, false, NONE);
	function_db5c0(unit_index);
	*(word *)((byte *)unit + unit->unknown33e) &= ~4;
	if ((1 << unit->type) & 1)
	{
		function_e4bd0(unit_index);
	}
}

/* the position of the unit's marker 0x4000095 */
// @retail 0xcaf60
void function_caf60(long unit_index, point3f *position)
{
	s_object_marker marker;

	function_b8d30(unit_index, 0x4000095, &marker, 1, false);
	*position = marker.matrix.position;
}

/* the position of the unit's marker 0x40000bd */
// @retail 0xcaf90
void function_caf90(long unit_index, point3f *position)
{
	s_object_marker marker;

	function_b8d30(unit_index, 0x40000bd, &marker, 1, false);
	*position = marker.matrix.position;
}

// @retail 0xcb7e0
void function_cb7e0(long unit_index, vector3f *vector)
{
	*vector = UNIT_GET(unit_index)->unknown168;
}

/* the first weapon the unit holds that is neither its current nor its next */
// @retail 0xcbe60
long function_cbe60(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	long result = NONE;

	for (long i = 0; i < 4; i++)
	{
		if (i != unit->current_weapon_index && i != unit->next_weapon_index && unit->weapon_object_indices[i] != NONE)
		{
			result = unit->weapon_object_indices[i];
			break;
		}
	}
	return result;
}

/* whether the unit (or what +0x248 names) has an actor */
// @retail 0xcc380
bool function_cc380(long object_index)
{
	s_unit *unit = UNIT_GET(object_index);

	if (unit->unknown248 != NONE)
	{
		unit = UNIT_GET(unit->unknown248);
	}
	return unit->actor_index != NONE;
}

/* the name of the unit's current animation, or NONE when it has none */
inline long unit_animation_name(long unit_index)
{
	long result = NONE;
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_animation *animation = UNIT_ANIMATION(unit);

	if (animation->unknown68 != NONE && animation->unknown00 != NONE && animation->unknown06 != NONE)
	{
		result = animation->name;
	}
	return result;
}

/* whether the unit's animation is 0x600008c */
// @retail 0xcc3c0
bool function_cc3c0(long unit_index)
{
	bool result = false;

	if (unit_animation_name(unit_index) == 0x600008c)
	{
		result = true;
	}
	return result;
}

/* whether the unit's animation is 0xd000042 or 0xc000043 */
// @retail 0xcc410
bool function_cc410(long unit_index)
{
	long name = unit_animation_name(unit_index);

	return name == 0xd000042 || name == 0xc000043;
}

/* whether the unit's seat has bit 2 of its flags */
// @retail 0xcc750
bool function_cc750(long unit_index, short seat_index)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(unit_index));
	bool result = false;

	if (seat_index >= 0 && seat_index < UNIT_SEAT_COUNT(definition))
	{
		result = UNIT_SEATS(definition)[seat_index].flags.bit2;
	}
	return result;
}

/* whether the unit's seat has bit 3 of its flags */
// @retail 0xcc7b0
bool function_cc7b0(long unit_index, short seat_index)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(unit_index));
	bool result = false;

	if (seat_index >= 0 && seat_index < UNIT_SEAT_COUNT(definition))
	{
		result = UNIT_SEATS(definition)[seat_index].flags.bit3;
	}
	return result;
}

// @retail 0xccb80
long function_ccb80(long unit_index)
{
	return UNIT_GET(unit_index)->unknown238;
}

/* adds to the unit's count of a grenade type and makes it current */
// @retail 0xcccd0
short function_cccd0(long unit_index, short grenade_type, short delta)
{
	s_unit *unit = UNIT_GET(unit_index);

	unit->grenade_counts[grenade_type] += delta;
	s_unit *again = UNIT_GET(unit_index);
	again->next_grenade_index = (char)grenade_type;
	again->current_grenade_index = (char)grenade_type;
	return unit->grenade_counts[grenade_type];
}

/* deletes the object +0x238 names */
// @retail 0xccd20
void function_ccd20(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->unknown238 != NONE)
	{
		function_b8540(unit->unknown238);
		unit->unknown238 = NONE;
	}
}

/* the unit's first empty weapon slot, or NONE */
// @retail 0xcd620
short function_cd620(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	short result = NONE;

	for (long i = 0; i < 4; i++)
	{
		if (unit->weapon_object_indices[i] == NONE)
		{
			result = (short)i;
			break;
		}
	}
	return result;
}

/* the unit's count of a grenade type */
// @retail 0xcdff0
short function_cdff0(long unit_index, short grenade_type)
{
	if (grenade_type != NONE)
	{
		return UNIT_GET(unit_index)->grenade_counts[grenade_type];
	}
	return 0;
}

// @retail 0xce020
short function_ce020(long unit_index)
{
	return UNIT_GET(unit_index)->current_grenade_index;
}

/* runs 0xce920 on each weapon slot but the current and the next */
// @retail 0xcece0
void function_cece0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	for (long i = 0; i < 4; i++)
	{
		if (i != unit->current_weapon_index && i != unit->next_weapon_index)
		{
			function_ce920(unit_index, i, 0, false);
		}
	}
}

/* forgets the object +0x2a8 names when it is this one */
// @retail 0xceea0
void __stdcall function_ceea0(long unit_index, long object_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->unknown2a8 == object_index)
	{
		unit->unknown2a8 = NONE;
	}
}

/* whether the unit is a biped that 0xe4050 holds for */
// @retail 0xd03b0
bool function_d03b0(long unit_index)
{
	bool result = false;

	if (UNIT_GET(unit_index)->type == 0)
	{
		result = function_e4050(unit_index);
	}
	return result;
}

/* starts listing the units among the object's children */
// @retail 0xd0590
void function_d0590(s_unit_child_iterator *iterator, long object_index)
{
	s_unit *object = UNIT_GET(object_index);

	iterator->object_index = object_index;
	iterator->unit_index = NONE;
	iterator->seat_index = NONE;
	iterator->next_index = object->first_child_index;
}

/* the next unit among the object's children, or 0 */
// @retail 0xd05c0
s_damage_object *function_d05c0(s_unit_child_iterator *iterator)
{
	s_damage_object *result = NULL;

	while (iterator->next_index != NONE)
	{
		long unit_index = iterator->next_index;
		s_unit *unit = UNIT_GET(unit_index);

		iterator->next_index = unit->next_sibling_index;
		if ((1 << unit->type) & 3)
		{
			iterator->unit_index = unit_index;
			iterator->seat_index = unit->parent_seat_index;
			result = (s_damage_object *)unit;
			break;
		}
	}
	return result;
}

/* clears the unit's weapon indices, then its weapons and +0x238 */
// @retail 0xc4880
void __stdcall function_c4880(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	unit->unknown216 = NONE;
	unit->unknown217 = NONE;
	unit->current_weapon_index = NONE;
	unit->next_weapon_index = NONE;
	function_ccf20(unit_index);
	function_ccd20(unit_index);
}

/* outside mode 4: steps the unit's 4-bit counter at +0x210 and marks the
   object +0xd4 names */
// @retail 0xcea00
void function_cea00(long unit_index)
{
	if (g_4e6948->mode != 4)
	{
		s_unit *unit = unit_at_masked_index(unit_index);
		word counter = ((byte)unit->unknown210 + 1) & 0xf;

		if (counter == (word)NONE)
		{
			counter = 0;
		}
		unit->unknown210 = counter;
		unit->unknown214 = counter;
		unit = unit_at_masked_index(unit_index);
		if (unit->unknown0d4 != NONE)
		{
			function_b58c0(unit->unknown0d4, 0x2000);
		}
	}
}

/* the nearest seat 0xc8bb0 finds for the unit: its object and seat
   index; returns 0xc8bb0's third result */
// @retail 0xc8ef0
short __stdcall function_c8ef0(long unit_index, long a, long *object_index, short *seat_index)
{
	short result = 0;
	long found_object_index = NONE;
	short found_seat_index = NONE;
	real distance = 3.4028235e38f;
	bool flag = false;

	function_c8bb0(unit_index, a, &found_object_index, &found_seat_index, &result, &distance, &flag);
	*object_index = found_object_index;
	*seat_index = found_seat_index;
	return result;
}

/* a number of ticks that shrinks with the difficulty: eight seconds' worth
   on easy and normal, six on heroic, four on legendary */
// @retail 0xc5340
short function_c5340()
{
	real scale = 1.0f;
	long difficulty;

	if (g_4e6948->state == 1)
	{
		difficulty = g_4e6948->difficulty;
	}
	else
	{
		difficulty = 1;
	}
	switch ((short)difficulty)
	{
	case 2:
		scale = 0.75f;
		break;
	case 3:
		scale = 0.5f;
		break;
	}
	return (short)unit_round(g_510c54->field_2_3 * (scale * 8.0f));
}

/* the first of the unit's weapons whose definition has bit 3 at +0x12c */
// @retail 0xcfe40
short function_cfe40(long unit_index)
{
	short result = NONE;
	s_unit *unit = UNIT_GET(unit_index);

	for (long i = 0; i < 4; i++)
	{
		long weapon_index = unit->weapon_object_indices[i];

		if (weapon_index != NONE && TEST_FIELD_BIT((*(dword *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x12c) >> 3) & 1))
		{
			result = (short)i;
			break;
		}
	}
	return result;
}

/* the next grenade type the unit has, from the given one in a direction
   (0 keeps the given one when the unit has it) */
// @retail 0xcbec0
short function_cbec0(long unit_index, short grenade_type, short direction)
{
	s_unit *unit = UNIT_GET(unit_index);
	short result = NONE;

	if (grenade_type == NONE)
	{
		grenade_type = 0;
	}
	short type = grenade_type;
	do
	{
		if (unit->grenade_counts[type] > 0)
		{
			result = type;
			if (type != grenade_type || direction == 0)
			{
				break;
			}
		}
		if (direction < 0)
		{
			type = type == 0 ? 1 : type - 1;
		}
		else
		{
			type = type == 1 ? 0 : type + 1;
		}
	} while (type != grenade_type);
	return result;
}

/* whether the unit rides in a seat of its parent unit with bit 6 */
// @retail 0xc48f0
bool function_c48f0(long unit_index)
{
	bool result = false;

	if (unit_index != NONE)
	{
		s_unit *unit = UNIT_GET(unit_index);

		if (unit->parent_index != NONE)
		{
			s_unit *parent = UNIT_GET(unit->parent_index);

			if ((1 << parent->type) & 3)
			{
				result = (*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(parent))[unit->parent_seat_index].flags >> 6) & 1;
			}
		}
	}
	return result;
}

/* stores the position 0xd1080 finds for the unit at +0x270 and clears the
   vector at +0x27c */
// @retail 0xd1000
void function_d1000(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_camera_data camera;
	transform4x3f matrix;

	if (function_d1080(unit_index, &matrix, &camera))
	{
		real_point3d_set(&unit->unknown270, matrix.position.x, matrix.position.y, matrix.position.z);
	}
	unit->unknown27c.i = 0.0f;
	unit->unknown27c.j = 0.0f;
	unit->unknown27c.k = 0.0f;
}

/* how far the unit's animation 0x700005c (rising) or 0x700005d (falling)
   has run, as 0 to 1; 1 otherwise */
// @retail 0xd1410
real function_d1410(long unit_index)
{
	real result = 1.0f;

	if (unit_index != NONE)
	{
		s_unit_animation *animation = UNIT_ANIMATION(UNIT_GET(unit_index));

		if (animation->name == 0x700005c)
		{
			result = 0.0f;
			if (animation->unknown00 != NONE && animation->unknown06 != NONE)
			{
				result = ((c_animation_channel *)animation)->get_frame_ratio();
			}
		}
		else if (animation->name == 0x700005d)
		{
			real ratio = 0.0f;

			if (animation->unknown00 != NONE && animation->unknown06 != NONE)
			{
				ratio = ((c_animation_channel *)animation)->get_frame_ratio();
			}
			result = 1.0f - ratio;
		}
	}
	return result;
}

/* the definition of the seat the unit rides in, or 0 */
// @retail 0xc8fc0
s_unit_seat_definition *function_c8fc0(long unit_index)
{
	s_unit_seat_definition *result = 0;
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->parent_index != NONE)
	{
		short seat_index = unit->parent_seat_index;

		if (seat_index != NONE)
		{
			s_unit *parent = UNIT_GET(unit->parent_index);

			if ((1 << parent->type) & 3)
			{
				result = &UNIT_SEATS(UNIT_DEFINITION_GET(parent))[seat_index];
			}
		}
	}
	return result;
}

/* stops five of the unit's request types, sends request 0x13 and runs
   0x114240 */
// @retail 0xce040
void function_ce040(long unit_index)
{
	s_unit_request request;

	function_e69c0(unit_index, 8);
	function_e69c0(unit_index, 0x12);
	function_e69c0(unit_index, 0);
	function_e69c0(unit_index, 0xa);
	function_e69c0(unit_index, 0x1b);
	memset(&request, 0, sizeof(request));
	request.type = 0x13;
	function_e6900(unit_index, &request);
	function_114240(unit_index);
}

/* a unit state set to its defaults: the name 0x6000086, NONE indices and
   the default vector three times */
struct s_unit_state_c6ef0
{
	long name;
	short unknown04;
	short unknown06;
	char unknown08;
	char unknown09;
	short unknown0a;
	short unknown0c;
	byte unknown0e[2];
	long unknown10;
	vector3f unknown14;
	long unknown20;
	long unknown24;
	vector3f unknown28;
	vector3f unknown34;
	vector3f unknown40;
	vector3f unknown4c;
	s_unit_state_tail tail;
};

// score 2 (was 2): the name store and shared NONE setup are exchanged; name
// after NONE 7, name before the zero 2, zero after NONE 2, zero removed 46,
// cached default pointer 6, later default pointer 2, tail NONE grouped 45,
// first vector early 16, zero floats swapped 4.
// @retail 0xc6ef0
void function_c6ef0(s_unit_state_c6ef0 *state)
{
	memset(state, 0, sizeof(*state));
	state->unknown04 = 0;
	state->name = 0x6000086;
	state->unknown06 = NONE;
	state->unknown08 = NONE;
	state->unknown09 = NONE;
	state->unknown0a = NONE;
	state->unknown0c = NONE;
	state->unknown28 = *g_4687a8;
	state->unknown34 = *g_4687a8;
	state->unknown40 = *g_4687a8;
	unit_state_tail_clear(&state->tail);
}

/* how far the unit's timer at +0x2be has run, by its kind at +0x2bc: kind 1
   counts down from 0xc5340's ticks, kind 2 up to ten seconds */
// @retail 0xc53c0
real function_c53c0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	switch (unit->unknown2bc)
	{
	case 1:
		return 1.0f - (real)unit->unknown2be / (real)function_c5340();
	case 2:
		return (real)unit->unknown2be / (real)unit_round(g_510c54->field_2_3 * 10.0f);
	}
	return 0.0f;
}

/* in mode 4, tells the player and actor code when the unit's +0x13c and
   +0x130 no longer match the ones it last saw (+0x2a8, +0x2ac) */
// @retail 0xd0d20
void function_d0d20(long unit_index)
{
	if (g_4e6948->mode == 4)
	{
		s_unit *unit = UNIT_GET(unit_index);

		if (unit->unknown13c != unit->unknown2a8)
		{
			if (unit->unknown13c != NONE)
			{
				function_a94b0(unit_index);
			}
			long player_index = unit->unknown2a8;
			if (player_index != NONE && record_pool_lookup(g_4e8c24, player_index))
			{
				function_a9440(unit_index, player_index);
			}
		}
		if (unit->unknown130 != unit->unknown2ac)
		{
			if (unit->unknown130 != NONE)
			{
				function_a7bc0(unit_index);
			}
			if (unit->unknown2ac != NONE)
			{
				function_a9500(unit_index, unit->unknown2ac);
			}
		}
	}
}

/* sends request 0x13 when the unit holds two weapons and either of them
   fails 0x101640; each weapon is read through its own copy of the object
   pool, so the guard and both reads look the unit up again, as retail does */
// @retail 0xd1540
void __stdcall function_d1540(long unit_index)
{
	if (UNIT_GET(unit_index)->current_weapon_index != NONE && UNIT_GET(unit_index)->next_weapon_index != NONE)
	{
		s_record_pool *objects = g_4e0300;
		s_record_pool *other_objects = g_4e0300;

		short index = ((s_unit_header *)objects->data)[unit_index & 0xffff].unit->current_weapon_index;
		long weapon_index = NONE;

		if (index != NONE)
		{
			weapon_index = ((s_unit_header *)objects->data)[unit_index & 0xffff].unit->weapon_object_indices[index];
		}
		short other_index = ((s_unit_header *)other_objects->data)[unit_index & 0xffff].unit->next_weapon_index;
		long other_weapon_index = NONE;

		if (other_index != NONE)
		{
			other_weapon_index = ((s_unit_header *)other_objects->data)[unit_index & 0xffff].unit->weapon_object_indices[other_index];
		}

		if (!function_101640(weapon_index) || !function_101640(other_weapon_index))
		{
			function_e68c0(0x13, unit_index);
		}
	}
}

/* a weapon the unit starts with: its tag and rounds */
struct s_unit_starting_weapon
{
	byte unknown00[4];
	long tag_index;
	short rounds_loaded;
	word rounds_total;
};

/* creates a starting weapon for the unit; when asked, sets bit 8 of the
   weapon's item flags (the word at +0x12c) */
// @retail 0xccd60
long function_ccd60(s_unit_starting_weapon const *weapon, long unit_index, bool flag)
{
	long object_index = NONE;

	if (weapon->tag_index != NONE)
	{
		byte data[0xc4];

		function_b7930(data, weapon->tag_index, unit_index, 0);
		object_index = function_b7b40(data);
		if (object_index != NONE)
		{
			s_unit *object = UNIT_GET(object_index);

			if (*(long *)(UNIT_DEFINITION_GET(object) + 0x2c0) > 0)
			{
				function_101980(object_index, weapon->rounds_loaded, 0, weapon->rounds_total);
			}
			if (flag)
			{
				*(word *)((byte *)object + 0x12c) |= 0x100;
			}
		}
	}
	return object_index;
}

/* whether either weapon the unit holds is in state 1 or 2 (+0x20c) */
// @retail 0xcc0c0
bool function_cc0c0(long unit_index)
{
	s_record_pool *objects = g_4e0300;
	bool result = false;

	for (long hand = 0; hand < 2; hand++)
	{
		s_unit *holder = UNIT_GET(unit_index);
		short index = (&holder->current_weapon_index)[hand];
		long weapon_index = index != NONE ? holder->weapon_object_indices[index] : NONE;

		if (weapon_index != NONE)
		{
			char *weapon;

			result = result ||
				(weapon = (char *)((s_unit_header *)objects->data)[weapon_index & 0xffff].unit)[0x20c] == 1 ||
				weapon[0x20c] == 2;
		}
	}
	return result;
}

/* gets rid of an object the unit held: deletes it (mode 1, or when the
   unit or the object's definition asks) or has 0xce6b0 drop it with an
   animation by mode */
// @retail 0xce470
void function_ce470(long mode, long unit_index, long object_index)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(object_index));

	if (TEST_FIELD_BIT((UNIT_GET(unit_index)->flags_134 >> 16) & 1))
	{
		mode = 1;
	}
	if (*(long *)(definition + 0x38) == NONE)
	{
		mode = 1;
	}
	function_10cdf0(object_index);
	function_10ca80(object_index, NONE);
	if (mode == 1)
	{
		function_b8540(object_index);
	}
	else
	{
		long name = 0;

		switch (mode)
		{
		case 2:
			name = 0xa0005ad;
			break;
		case 3:
			name = 0x9000536;
			break;
		}
		function_ce6b0(unit_index, name, object_index, 1.0f);
	}
}

/* the unit's next zoom level after the given one: its weapon's (0x101010)
   when the weapon has zoom levels (0x100fd0), else, unless the weapon's
   flag 0 at +0x12e forbids it, the next of the globals' count (NONE past
   the last) */
// @retail 0xc8960
short function_c8960(long unit_index, short value)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool flag = false;
	long weapon_index = function_cbd50(unit_index, unit->current_weapon_index);
	short result = NONE;

	if (weapon_index != NONE)
	{
		byte *definition = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));

		flag = *(definition + 0x12e) & 1;
		if (function_100fd0(weapon_index))
		{
			result = function_101010(weapon_index, value);
			goto done;
		}
	}
	if (!flag)
	{
		long count = *(long *)(*(byte **)((byte *)g_4e034c + 0x134) + 0xb8);

		if (count > 0 && value + 1 < count)
		{
			result = value + 1;
		}
	}
done:
	return result;
}

/* sends request 0x17 when the unit's seat has bit 15 */
// @retail 0xd12b0
void function_d12b0(long unit_index, long seat_index, bool a, bool b)
{
	if (unit_index != NONE && seat_index != NONE &&
		TEST_FIELD_BIT((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit_index)))[seat_index].flags >> 15) & 1))
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 0x17;
		request.type17.unknown4 = a;
		request.type17.unknown5 = b;
		function_e6900(unit_index, &request);
	}
}

/* sends request 0x18 when the unit's seat has bit 15 */
// @retail 0xd1360
void __stdcall function_d1360(long unit_index, long seat_index, bool a, bool b)
{
	if (unit_index != NONE && seat_index != NONE &&
		TEST_FIELD_BIT((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit_index)))[seat_index].flags >> 15) & 1))
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 0x18;
		request.type17.unknown4 = a;
		request.type17.unknown5 = b;
		function_e6900(unit_index, &request);
	}
}

/* the dot product of two vectors' horizontal (i, j) parts */
static inline real unit_dot_product2d(vector3f const *a, vector3f const *b)
{
	return a->i * b->i + a->j * b->j;
}

/* whether a biped faces along the direction: always while in animation
   0x6000084, else by the sign of its facing's dot product with it */
// @retail 0xcc010
bool __stdcall function_cc010(long object_index, vector3f const *direction)
{
	s_unit *unit = (s_unit *)function_badc0(object_index, 3);
	bool result = false;

	if (unit && unit->type == 0)
	{
		long unknown;
		long name = NONE;

		function_10f630(object_index, &name, &unknown);
		if (!(*(UNIT_DEFINITION_GET(unit) + 0xbe) & 1))
		{
			if (name == 0x6000084)
			{
				result = true;
			}
			else
			{
				result = unit_dot_product2d((vector3f const *)&unit->unknown18c, direction) > 0.0f;
			}
		}
	}
	return result;
}

/* the byte at +0x10a as bits */
struct s_unit_flags_10a_view
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word unknown3 : 4;
	word bit7 : 1;
};

/* the unit's death outside mode 4: clears its flags, applies the globals'
   damage at +0x144 to it and, unless flag 2 at +0x10a, marks it */
// @retail 0xcffc0
void function_cffc0(long unit_index)
{
	if (g_4e6948->mode != 4)
	{
		s_unit *unit = UNIT_GET(unit_index);
		byte *globals = *(byte **)((byte *)g_4e034c + 0x144);

		bit_vector_set(&unit->flags_134, 21, false);
		bit_vector_set(&unit->flags_134, 5, false);
		((s_unit_flags_10a_view *)&unit->flags_10a)->bit7 = false;
		if (globals && *(long *)(globals + 0x48) != NONE)
		{
			s_type_1e6529 damage;

			damage.material_index = NONE;
			function_d6660(&damage, *(long *)(globals + 0x48));
			function_d7b80(&damage, unit_index, NONE, NONE, NONE, 0);
		}
		if (!TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2))
		{
			unit->flags_10a |= 0x20;
			function_b7360(unit_index);
		}
	}
}

/* whether 0xc9040 holds for any of the seat's markers on the unit */
// @retail 0xc6fb0
bool __stdcall function_c6fb0(long unit_index, long a, short seat_index)
{
	long marker_name = *(long *)((byte *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit_index)))[seat_index] + 0xc);
	bool found = false;
	s_object_marker markers[4];
	long count = function_b8d30(unit_index, marker_name, markers, 4, false);

	for (long i = 0; !found && i < count; i++)
	{
		if (function_c9040(unit_index, a, seat_index, &markers[i].matrix.position, &markers[i].matrix.forward))
			found = true;
	}
	return found;
}

/* whether a rider of the unit sits in a seat with bit 11 whose +0x3e is
   the given value */
// score 2 (was 2): only the order of the two argument loads before function_d0620 differs
// (retail moves unit_index into eax before taking the iterator's address). Flag 11 read as a field
// bit gives the rest. Iterator first, result first or an iterator pointer local 2; result after the call 77.
// Round 94: matches with 0xd0620's formals as (iterator, object_index), held for src/damage.cpp's callers.
// Round 410: Initializer inline/noinline, next inline/noinline, both inline and both noinline 2
// each; natural for initializer 2; predicate-defined result 49. All restored; initializer
// argument-load order remains.
// @retail 0xc9200
bool __stdcall function_c9200(long unit_index, short value)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(unit_index));
	bool result = false;
	s_object_child_iterator iterator;

	function_d0620(&iterator, unit_index);
	while (function_d0690(&iterator) && !result)
	{
		if (iterator.child_value == unit_index && iterator.child_short != NONE)
		{
			s_unit_seat_definition *seat = &UNIT_SEATS(definition)[iterator.child_short];

			if (TEST_FIELD_BIT((*(dword *)&seat->flags >> 11) & 1) && *(short *)((byte *)seat + 0x3e) == value)
			{
				result = true;
			}
		}
	}
	return result;
}

/* how far out of range two values are for the unit: 0 within, 1 past the
   first limits, 2 past the second (its actor's, else its definition's) */
// @retail 0xc96b0
void function_c96b0(long unit_index, long *result, real a, real b)
{
	s_unit *unit = UNIT_GET(unit_index);

	*result = 0;
	if (unit->actor_index != NONE)
	{
		byte *limits = (byte *)function_1e4a10(*(long *)((byte *)g_4f55f0->data + (unit->actor_index & 0xffff) * 0x888 + 0x54));

		if (limits)
		{
			if (a > *(real *)(limits + 0x24) || b > *(real *)(limits + 0x28))
			{
				*result = 2;
			}
			else if (a > *(real *)(limits + 0x18) || b > *(real *)(limits + 0x1c))
			{
				*result = 1;
			}
			return;
		}
	}
	byte *definition = UNIT_DEFINITION_GET(unit);
	if (b > *(real *)(definition + 0x10c))
	{
		*result = 2;
	}
	else if (b > *(real *)(definition + 0x104) || a > *(real *)(definition + 0x104))
	{
		*result = 1;
	}
}

/* puts a weapon in the unit's slot, or empties the slot and gets rid of
   the weapon there by mode (mode 4 keeps it) */
// @retail 0xcea70
void __stdcall function_cea70(long mode, short slot_index, long weapon_index, long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (weapon_index != NONE)
	{
		function_10ca80(weapon_index, unit_index);
		function_10cd50(weapon_index);
		unit->weapon_object_indices[slot_index] = weapon_index;
		unit->unknown228[slot_index] = 0;
	}
	else
	{
		long old_weapon_index = unit->weapon_object_indices[slot_index];

		unit->weapon_object_indices[slot_index] = NONE;
		if (mode != 4)
		{
			function_ce470(mode, unit_index, old_weapon_index);
		}
	}
	unit = UNIT_GET(unit_index);
	if (unit->unknown0d4 != NONE)
	{
		function_b58c0(unit->unknown0d4, (1 << (slot_index + 0x12)) | (1 << (slot_index + 0xe)));
	}
	function_cea00(unit_index);
}

/* attaches the weapon to the unit at its hand's marker (the unit
   definition's +0x158 or +0x15c), at the unit's marker +0x160 when the
   weapon has it, else at the weapon definition's +0x284 */
// @retail 0xd0870
void __stdcall function_d0870(long weapon_index, long unit_index, long secondary)
{
	byte *local_98b918_2 = UNIT_DEFINITION_GET(UNIT_GET(unit_index));
	byte *local_67e06b = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));
	long parent_marker_name;

	if (!secondary)
	{
		parent_marker_name = *(long *)(local_98b918_2 + 0x158);
	}
	else
	{
		parent_marker_name = *(long *)(local_98b918_2 + 0x15c);
	}
	s_object_marker marker;
	long marker_name = *(long *)(local_98b918_2 + 0x160);
	if (marker_name == NONE || marker_name == 0 ||
		!function_b8d30(weapon_index, marker_name, &marker, 1, true))
	{
		marker_name = *(long *)(local_67e06b + 0x284);
	}
	function_10cec0(weapon_index, unit_index, parent_marker_name, marker_name);
}

/* drops every weapon the unit holds (mode 1: deletes them) */
// @retail 0xccf20
void function_ccf20(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	for (long i = 0; i < 4; i++)
	{
		if (unit->weapon_object_indices[i] != NONE)
		{
			function_cea70(1, (short)i, NONE, unit_index);
		}
	}
}

/* counts down the unit's timers: +0x2e8 (clearing +0x2e4), +0x2ca (then
   runs 0x1c9c80 with +0x2c8..+0x2d0) and +0x1f4 (then 0xcffc0); whether
   any ran */
// @retail 0xc6740
bool function_c6740(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

	if (unit->unknown2e8 > 0)
	{
		if (--unit->unknown2e8 == 0)
		{
			unit->unknown2e4 = 0.0f;
		}
		result = true;
	}
	if (unit->unknown2ca > 0)
	{
		if (--unit->unknown2ca == 0)
		{
			function_1c9c80(unit_index, unit->unknown2d0, unit->unknown2c8, unit->unknown2cc, 0, true);
			unit->unknown2c8 = 0;
			unit->unknown2d0 = NONE;
			unit->unknown2cc = 0.0f;
		}
		result = true;
	}
	if (unit->unknown1f4 > 0)
	{
		if (--unit->unknown1f4 == 0)
		{
			function_cffc0(unit_index);
		}
		return true;
	}
	return result;
}

/* one of a unit definition's weapons (+0x1c4, 8 bytes each) */
struct s_unit_definition_weapon
{
	long unknown0;
	long tag_index;
};

/* creates the unit definition's starting weapons (+0x1c0, +0x1c4) and
   gives them to the unit; deletes those it cannot take (or, in state 2,
   already has) */
// @retail 0xccab0
void function_ccab0(long unit_index)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(unit_index));

	for (long i = 0; i < *(long *)(definition + 0x1c0); i++)
	{
		s_unit_definition_weapon *entry = &(*(s_unit_definition_weapon **)(definition + 0x1c4))[i];

		if (entry->tag_index != NONE)
		{
			byte data[0xc4];

			function_b7930(data, entry->tag_index, unit_index, 0);
			long weapon_index = function_b7b40(data);
			if (weapon_index != NONE)
			{
				if ((g_4e6948->state == 2 &&
					unit_has_weapon_definition(unit_index, UNIT_GET(weapon_index)->definition_index)) ||
					!function_cd0c0(unit_index, weapon_index, 1))
				{
					function_b8540(weapon_index);
				}
			}
		}
	}
}

/* lowers both of the unit's weapons (requests 8 and 0x12), then drops them */
// @retail 0xccff0
void __stdcall function_ccff0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_request request;

	unit->unknown216 = NONE;
	if (unit->current_weapon_index != NONE)
	{
		memset(&request, 0, sizeof(request));
		request.type = 8;
		request.type17.unknown4 = true;
		function_e6900(unit_index, &request);
	}
	unit->unknown217 = NONE;
	if (unit->next_weapon_index != NONE)
	{
		memset(&request, 0, sizeof(request));
		request.type = 0x12;
		request.type17.unknown4 = true;
		function_e6900(unit_index, &request);
	}
	function_ccf20(unit_index);
}

/* takes an amount from the unit's +0x2b0 and caps +0x2b8 (a quarter by
   default), unless its state +0x2bc is set (then 0xc5740 runs) */
// @retail 0xd0e60
void function_d0e60(long unit_index, real amount, real limit)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->unknown2bc)
	{
		function_c5740(unit_index, 0);
		return;
	}
	if (limit == 0.0f)
	{
		limit = 0.25f;
	}
	if (limit > unit->unknown2b8)
	{
		limit = unit->unknown2b8;
	}
	unit->unknown2b8 = limit;
	unit->unknown2b0 -= amount;
	if (TEST_FIELD_BIT((unit->flags_134 >> 3) & 1) && 0.05f > unit->unknown2b0)
	{
		unit->unknown2b0 = 0.05f;
	}
	s_unit *current = UNIT_GET(unit_index);

	if (current->unknown0d4 != NONE)
	{
		function_b58c0(current->unknown0d4, 0x800000);
	}
}

/* which hands the unit holds weapons in, and whether its seat lacks bit 5 */
struct s_unit_weapon_hands
{
	byte unknown00[8];
	bool one;
	bool two;
	bool seat_allows;
	byte unknown0b;
};

// @retail 0xcb430
void function_cb430(long unit_index, s_unit_weapon_hands *hands)
{
	s_record_pool *objects = g_4e0300;
	s_unit *unit = UNIT_GET(unit_index);

	memset(hands, 0, sizeof(*hands));
	if (function_cbd50(unit_index, UNIT_GET(unit_index)->current_weapon_index) != NONE)
	{
		s_unit *next = ((s_unit_header *)objects->data)[unit_index & 0xffff].unit;
		short next_index = next->next_weapon_index;

		if (next_index != NONE && next->weapon_object_indices[next_index] != NONE)
		{
			hands->two = true;
		}
		else
		{
			hands->one = true;
		}
	}
	if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
	{
		s_unit *parent = ((s_unit_header *)objects->data)[unit->parent_index & 0xffff].unit;

		hands->seat_allows = !((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(parent))[unit->parent_seat_index].flags >> 5) & 1);
	}
}

/* asks the unit sharing the seat group of the unit's seat to get out
   (request 0x20); returns false */
// @retail 0xd0f30
bool __stdcall function_d0f30(long unit_index, bool a, bool b)
{
	s_unit *unit = UNIT_GET(unit_index);
	long parent_index = unit->parent_index;
	if (parent_index == NONE)
		return false;
	s_unit *parent = UNIT_GET(parent_index);
	if (!((1 << parent->type) & 3))
		return false;
	short seat_index = unit->parent_seat_index;
	if (seat_index == NONE)
		return false;
	s_unit_seat_definition *seat = &UNIT_SEATS(UNIT_DEFINITION_GET(parent))[seat_index];
	long occupant_index = NONE;
	if (*(short *)((byte *)seat + 0x3e) != NONE)
		occupant_index = function_c8f60(parent_index, *(short *)((byte *)seat + 0x3e));
	if (occupant_index != NONE && occupant_index != unit_index)
	{
		s_unit_request request;
		request.type = 0x20;
		request.type17.unknown4 = a;
		request.type17.unknown5 = b;
		if (function_e6900(occupant_index, &request))
			function_cc810(parent_index);
	}
	return false;
}

/* lowers the weapon in one of the unit's hands (request 8, or 0x12 for the
   second hand) */
// @retail 0xcd4e0
bool __stdcall function_cd4e0(long unit_index, short hand, bool flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

	if ((&unit->current_weapon_index)[hand] != NONE && (&unit->unknown216)[hand] != NONE)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		(&unit->unknown216)[hand] = NONE;
		request.type = hand != 0 ? 0x12 : 8;
		request.type17.unknown4 = flag;
		if (function_e6900(unit_index, &request))
		{
			if (hand)
			{
				function_cea00(unit_index);
			}
			if (unit->unknown13c != NONE)
			{
				function_1520f0(unit->unknown13c);
			}
			result = true;
		}
	}
	return result;
}

/* the weapon the unit, or the first rider down its children with bit 27 of
   its definition's +0xbc, holds; which unit holds it */
// @retail 0xcbd80
long __stdcall function_cbd80(long object_index, long *holder_index)
{
	s_record_pool *objects = g_4e0300;
	long weapon_index = function_cbd50(object_index, UNIT_GET(object_index)->current_weapon_index);

	if (weapon_index != NONE)
	{
		if (holder_index)
		{
			*holder_index = object_index;
		}
	}
	else
	{
		for (long child_index = ((s_unit_header *)objects->data)[object_index & 0xffff].unit->first_child_index; child_index != NONE;)
		{
			s_unit *child = UNIT_GET(child_index);

			if ((1 << child->type) & 3 && TEST_FIELD_BIT((*(dword *)(UNIT_DEFINITION_GET(child) + 0xbc) >> 27) & 1))
			{
				weapon_index = function_cbd80(child_index, holder_index);
				if (weapon_index != NONE)
				{
					break;
				}
			}
			child_index = child->next_sibling_index;
		}
	}
	return weapon_index;
}

/* empties a weapon slot of the unit (deleting the weapon in mode 4, when
   it is used up, or in state 2 when it is neither kept nor fit), and
   resets the hands that would have taken it */
// @retail 0xce920
void function_ce920(long unit_index, long slot_index, long mode, bool flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	long weapon_index = unit->weapon_object_indices[(short)slot_index];

	if (weapon_index != NONE)
	{
		if (g_4e6948->mode == 4 || *(real *)((byte *)UNIT_GET(weapon_index) + 0x184) >= 1.0f ||
			(g_4e6948->state == 2 && !function_100f00(weapon_index) && !function_101640(weapon_index)))
		{
			mode = 1;
		}
		function_cea70(mode, (short)slot_index, NONE, unit_index);
		for (long hand = 0; hand < 2; hand++)
		{
			if ((short)slot_index == (&unit->unknown216)[hand])
			{
				if (flag)
				{
					(&unit->unknown216)[hand] = (char)function_cdeb0(unit_index, NONE, (&unit->current_weapon_index)[hand], 0);
				}
				else
				{
					(&unit->unknown216)[hand] = NONE;
				}
			}
		}
	}
}

/* a motion in three phases: an acceleration, a coast and a deceleration */
struct s_unit_motion
{
	bool done;
	byte unknown01[3];
	real position;
	real velocity;
	real acceleration;
	real field_10_4;
	real coast_time;
	real deceleration;
	real s_type_8c87de;
};

/* advances a position and velocity along the motion for a time; whether
   time remains after its last phase */
// MiniMax A (11-14): no new form, no lower score (104): retail moves the two incoming floats into xmm0 and xmm1 and
// compares them there, while this build compares them in xmm1 and xmm3 and keeps a third value in xmm5, and the body
// is 265 bytes against retail's 227 (BLOCKED-BY 0xc7840's call sites).
// MiniMax H (11-14): no new form, no lower score (104): the accumulator spelling still measures 28, but this build then inlines it into 0xc7840 and loses the matched 0x1e5af0 and 0x1faf80, so it cannot be kept.
// MiniMax B (11-13): no new form, no lower score (104): retail moves the two incoming floats into xmm0 and xmm1 and compares them there, while this build compares them in xmm1 and xmm3 and keeps a third value in xmm5, and the body is 265 bytes against retail's 227.
// MiniMax H (11-12): no new form, no lower score (104): the accumulator form that gives retail's 227 bytes and 28 differences still makes LTCG inline this body into 0xc7840, which is in this lane's list, so it is not kept.
// MiniMax C (11-10): score 104 (was 104): no lower score this round. The accumulator form re-measured 28 with
// 227 bytes, retail's length: every instruction then matches retail's except that the time argument arrives in
// XMM2 and the phase step sits in XMM3, where retail has time in XMM3 and the step in XMM2. The three float
// arguments are promoted to XMM5/XMM4/XMM2 here and to XMM5/XMM4/XMM3 in retail, so the callee's argument
// registers follow the call site's context - and retail reaches the calls out of line, which this build does not.
// The change was dropped: LTCG inlines the callee into 0xc7840 (563 -> 1104) and 0x1faf80 (matched) flips to 16.
// MiniMax S (11-11): no new form, no lower score (104); retail's register convention still needs the body out of line, which only a pragma or a new noinline twin can do, so the body stands as found.
// MiniMax G (11-12): no new form tried this round, no lower score (104): the out-of-line convention still needs the
// body to stay out of line, which only a pragma or a twin copy could give, so the body stands as found.
// MiniMax H (11-13): no new form, no lower score (104); retail moves the two incoming floats into xmm0 and xmm1 and compares them there, while this build compares them in xmm1 and xmm3 and keeps a third value in xmm5, and the body is 265 bytes against retail's 227.
// score 104 (was 104): this body keeps position/velocity on the stack (ret 0xc); retail takes them in
// XMM5/XMM4, time in XMM3 and returns with ret 4. #515's body (output accumulators initialised from the inputs,
// and each step as `time > motion->field ? motion->field : time` reading the field again) gives retail's
// convention and the same 227 bytes at 28; only time/step are swapped (time in XMM2, step in XMM3). But then
// 0x1e5af0 (matched) flips to 42 through 0x1faa20 -> 0xc7840 -> this function (its ebx/edi choice moves) and
// 0xc7840 goes 723 -> 989, so it is not kept. Same convention at 28 with a single step variable, a time copy,
// the compare operands swapped; 67 with the three durations cached in locals; 69/70 with `if (time <= step)
// step = time` or `step = time > step ? step : time` on a step loaded first. Separate output accumulators
// alone 69; clamps, phase steps and completion test variants on the stack form 104-126. Retail's time in XMM3
// probably follows 0xc7840's call site, so look there first.
// Round 57 (m3 lane 9): the accumulator form with ternary steps 28 (227 bytes) is small enough that LTCG inlines it
// into both 0xc7840 calls (2786 bytes, 995: the "723 -> 989" above); with the three steps through a forceinline
// step helper 67 (221 bytes), still inlined. Retail keeps it out of line, so its body costs more than this form.
// Round 197 (lane 3, fast): the accumulator form re-measured 28 (227 bytes) with every parameter order (same code); time copied
// 28; named phase values or one step variable 70 (218 bytes); one-use named products and outputs 28 (still inlined at
// 2871 bytes); in-place position and velocity 121 (277 bytes, stack again, not inlined); `if (time <= step)` steps 66 (221
// bytes); defined after 0xc7840 28 (still inlined). Only a test ahead of the calls in 0xc7840 keeps it out of line (see its note).
// Round 2 of #1049 (fast): statement-by-statement walk against retail finds no logic difference: same done test and
// copies, same three phases (accelerate 0x10/0xc, coast 0x14, decelerate 0x1c/0x18), same clamp (`time <= step` -> time),
// same final `time > 0`. The 38 extra bytes are only the stack arguments. The accumulator form (28, 227 bytes) differs from
// retail in one thing: time arrives in XMM2 and step is XMM3 (retail time XMM3, step XMM2); variants that keep 28 and the
// same swap: result declared after the accumulators, one function-scope step, a `remaining` copy of time, products written
// out in each use, `!result &&` repeated at each phase test, `!motion->done` in the first test, an unused-product block.
// Inlining into 0xc7840 does not change with any of these (2870 bytes, 1103); `bool result = false` with `!motion->done`
// tested (not equivalent: a done motion would return false) is 233 bytes and stays out of line (0xc7840 2442, 645), so the
// choice looks size-bound (227 inlined, 233 not) and retail's identical 227 bytes being kept out of line points at 0xc7840.
// Round 144 (m3 lane 7): LTCG's inline cost follows the callee's final code (compile-only /d2inlT probes: every form with
// the accumulator form's code costs 72; the default threshold sits at 74-75; this stack form needs 78), so retail's
// identical 227 bytes cost the same and stay out of line only because retail reaches its calls on a colder path: an
// `if (rate <= 0.0f) return;` before the calls raises the needed threshold to 82, while else-arm speed zeros, the `||`
// speed test and normalize3d for the speed leave 72. With the accumulator form kept out of line (pragma, diagnostic only)
// 28, but 0xc7840 563 -> 648 and 0x1e5af0/0x1faf80 lost: any XMM-convention body changes 0xc7840's own convention.
// A `start_velocity` copy in the first phase (velocity updated before position) puts step in XMM2 as retail (24 out of
// line, time XMM1 / velocity XMM3, still inlined); in both accelerating phases 58 (233 bytes, out of line, same losses);
// on this stack form 103. BLOCKED-BY 0xc7840 (call-site frequency and its convention).
// MiniMax G (11-11): no new form, no lower score kept: output accumulators with the ternary steps give 70
// (218 bytes) in retail's XMM5/XMM4 convention, but LTCG then inlines the callee into 0xc7840 (563 -> 1095)
// and 0x1faf80 (matched -> 16); the ternary clamp alone leaves the body on the stack (104), and in that
// form the third phase keeps the change alone where retail adds the old velocity. The body was restored.
// MiniMax R (11-11): no new form tried this round, no lower score (104): the body is 265 bytes against retail's 227 - 38 bytes of extra expansion - and the same body inlined into 0xc7840 is what holds that caller at 563.
// MiniMax A (11-13): no new form tried this round, no lower score (104): retail moves the two incoming floats
// into xmm0 and xmm1 and compares them there, while this build compares them in xmm1 and xmm3 and keeps a third
// value in xmm5, and the body is 265 bytes against retail's 227; the accumulator form that gives retail's 227
// bytes at 28 still makes LTCG inline this body into 0xc7840 and flips the matched 0x1faf80: BLOCKED-BY
// 0xc7840's call sites.
// @retail 0xc7750
bool function_c7750(s_unit_motion const *motion, real position, real velocity, real time, real *out_velocity,
	real *out_position)
{
	bool result = motion->done;

	if (!result && time > 0.0f)
	{
		real step = motion->field_10_4;

		if (step > 0.0f)
		{
			if (time <= step)
			{
				step = time;
			}
			real change = motion->acceleration * step;
			position = (change * 0.5f + velocity) * step + position;
			velocity = change + velocity;
			time -= step;
		}
		if (time > 0.0f)
		{
			step = motion->coast_time;
			if (step > 0.0f)
			{
				if (time <= step)
				{
					step = time;
				}
				position = step * velocity + position;
				time -= step;
			}
			if (time > 0.0f)
			{
				step = motion->s_type_8c87de;
				if (step > 0.0f)
				{
					if (time <= step)
					{
						step = time;
					}
					real change = motion->deceleration * step;
					position = (change * 0.5f + velocity) * step + position;
					velocity = change + velocity;
					time -= step;
				}
				if (time > 0.0f)
				{
					result = true;
				}
			}
		}
	}
	*out_position = position;
	*out_velocity = velocity;
	return result;
}

/* once: sets the unit's flag 21 and a random angle at +0x2d8 around its
   actor's direction (or its own facing) */
// @retail 0xcfec0
void function_cfec0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (!TEST_FIELD_BIT((unit->flags_134 >> 21) & 1))
	{
		real actor_vector[2];
		real range;

		bit_vector_set(&unit->flags_134, 21, true);
		if (unit->actor_index != NONE && function_1e3370(unit->actor_index, actor_vector))
		{
			unit->unknown2d8 = 0.0f;
			range = 0.43633232f;
		}
		else
		{
			/* the angle keeps the vector's first component, as in retail; the
			   named double gives retail's 8-byte aligned frame */
			double angle = atan2(unit->forward.j, unit->forward.i);
			actor_vector[0] = (real)angle;

			if (actor_vector[0] > 3.1415927f)
			{
				actor_vector[0] -= 6.2831855f;
			}
			unit->unknown2d8 = actor_vector[0];
			range = 1.7453293f;
		}
		unit->unknown2d8 += function_259d0(&g_4e7408->unknown0, 0, 0, -range, range);
	}
}

/* the object that answers for the unit: the vehicle when the unit's seat
   has bit 0 or 3, or a rider of that vehicle with bit 27 of its
   definition's +0xbc and its object flag 26; else the unit */
// MiniMax G (11-13): no lower score (2): the only remaining difference is the last exit, where the value for
// unit_index == NONE is returned as the constant (or eax, 0xffffffff) and retail moves the register copy
// (mov eax, ebx), so our body is 258 bytes against retail's 257. This round tried fifteen more forms: a result local
// returned at every level (113, 47 and 58, according to where it is assigned and returned), a copy of the value in the
// exit as int, long, unsigned or as an index (2 each), one return per level through that local (47), the exits shared
// by two paths (12, 32 and 39) and the local tested against itself (47 twice); every one of them scores 2 or worse,
// because the local's value merges the exits and drops the copy in ebx. The body stands as found.
// MiniMax H (11-12): no new form, no lower score (2): the exit for unit_index == NONE still returns the constant (or eax, 0xffffffff) where retail moves ebx; a result local (46) and a single final return (62) both change the frame and the register allocation, and this round's unsigned comparison (0xffffffffu) and the copy of the parameter that the test uses instead of the parameter itself both keep the fold, so the body stands as found.
// MiniMax H (11-11): no new form, no lower score (2): the exit for unit_index == NONE returns the constant (or eax, 0xffffffff) where retail moves ebx (mov eax, ebx), so the body is one byte longer; the value is kept in the register only where an exit is shared with another path, and merging the exits moves the edi push into the prologue.
// MiniMax S (11-11): no new form, no lower score (2); the copy propagation from the compared value into the returned one cannot be broken without a form review rejects, so the body stands as found.
// MiniMax G (11-11): no new form, no lower score: a result variable returned at each level 47 (the copy in ebx is
// dropped, so the prologue changes) and one final return 113 (the exits merge into one epilogue); a probe shows VC7
// folds the -1 into every separate exit, for a parameter, a copy or a cast alike, and keeps the register copy only
// when one return is reached from two paths, which merges the exits and moves the EDI push.
// MiniMax G (11-13): no lower score (2): the last exit still returns the constant where retail moves the register copy;
// dropping the unit-level return so that the NONE exit shares the final return gives retail's register copy but pushes
// EDI ahead of the branch (12, 252 B), so the per-level epilogues are what keep the lazy pushes.
// MiniMax G (11-12): no new form tried this round, no lower score (2): the only remaining difference is the last
// exit, which returns the constant where retail moves the register copy, and no spelling of the copy avoids the fold.
// score 2 (was 113): a return after each closing brace gives retail's per-level epilogues, lazy pushes and
// exit order. Left: the last exit (unit_index == NONE) returns the constant (or eax, -1); retail moves ebx
// (mov eax, ebx), so it did not fold the value there. Tried: a result variable returning at each level 47;
// early-return chain 99; nested ifs with one final return and the parameter reassigned 62 (one shared epilogue,
// all four registers pushed first); early return for NONE at the top 92; a result variable with one final
// return 113. Round 1 of #1049, all still 2 (or eax, -1 kept): result variable / copy of the parameter
// (`original`) returned at the last exit; `valid = unit_index != NONE` as the test; (dword) casts on the compare;
// while instead of if. Failing differently: `~unit_index != 0` 96; early return of unit_index for the NONE case
// at the top 99 (layout moves the exit first). The optimizer substitutes -1 on the NONE edge whatever the
// spelling of the returned copy.
// Permuter (25 min on #1049, about 900 variants): nothing below its own baseline (1 in its render).
// Round 82 (lane 11, fast): `NONE != unit_index` 2; `!(unit_index == NONE)` 2; the seat-level and parent-level returns
// merged into the final one 39 (246 B); the parent-level return merged into the final one 12 (252 B: the shared exit
// returns `mov eax, ebx` as retail, but VC then pushes EDI before the first test); the seat-level return merged into
// the parent-level one 32. So retail's NONE exit is a separate epilogue without the edge constant.
// m3 lane 16 r104 (fast, callers 0x1a4900/0x1a4c00 checked): the parent index through a ternary on the NONE test with the
// seat read through a second lookup 12 (252 B: every exit returns `mov eax, ebx` as retail, but EDI is pushed before the
// first test); `unit_index != NONE && UNIT_GET(unit_index)->parent_index != NONE` 99 (the argument moves to the stack and
// both callers lose their match); the unit as `unit_index != NONE ? UNIT_GET(unit_index) : NULL` 97 (same loss); one early
// return for `unit_index == NONE || parent == NONE` 99 (same loss); parent and seat tested together with && 33.
// Compile-only probes: VC puts the constant into every NONE exit that is its own block, for a parameter, a call result or a
// load alike; the 11 matched functions whose NONE edge returns the register all reach a return shared with another path.
// Hypothesis (inferred from that scan, not measured): retail's NONE exit was a shared return split again before the pushes
// were placed.
// MiniMax R (11-11): no new form tried this round, no lower score (2): the body is 258 bytes against retail's 257 and the last exit is the only difference left; every exit form measured before moves retail's lazy pushes into the prologue.
// Round 430: child result assigned before break 28; typed child flags 2;
// typed parent table 2; existing unit/parent/sibling getter 2/2/2;
// plain child while 2; null-child continue 2. No lower score.
// MiniMax B (11-13): no new form, no lower score (2): the body is 258 bytes against retail's 257 and the last exit is
// still the only difference left - it returns the constant where retail moves the register copy (mov eax, ebx); every
// spelling of that copy folds back into the constant, and the per-level epilogues of the earlier forms move retail's
// lazy pushes into the prologue.
// MiniMax A (11-14): still no lower score (2): one exit for the whole body (a result variable with a
// break out of the child search) gives 97, and letting the parent-level path fall through to the last
// return so that it shares the exit with the NONE path gives 12, because the shared path moves
// `push edi` ahead of the branch on the NONE edge. Retail's NONE exit at 0xcb7cc has its own epilogue
// (pop ebx, pop ecx, ret, no pop edi), so it cannot be tail-shared with the other exits, and the
// shared-return observation from the earlier scan has to be about the value at that exit being a merge
// rather than about a shared block: the source would return a variable with more than one definition
// while the tested variable stays a separate copy in EBX (retail's `mov ebx, eax; cmp ebx, -1`). Every
// form tried that gives the returned variable a second definition also loses the EBX copy, because the
// test moves back onto the parameter in EAX, so the two requirements pull against each other.
// MiniMax A (11-14): no new form, no lower score (2): a plain full-build check of this body gives the
// same single difference as the reduced image (the NONE exit returns `or eax, 0xffffffff` where retail
// moves the register copy), so the reduced image's unresolved globals are not the cause; a result
// variable compared, assigned and returned through all four exits keeps our order (47), because VC's
// copy propagation drops the copy and takes the test on the parameter in EAX, which loses retail's
// `mov ebx, eax` prologue.
// P455: Explicit rejected else-return arms at unit/parent/seat levels, seven
// subsets 2/2/2/2/2/2/2; signed-range non-sentinel admission 92.
// NOT kept: no exact separate NONE exit; original body and lazy saves restored.
// @retail 0xcb6d0
long function_cb6d0(long unit_index)
{
	if (unit_index != NONE)
	{
		s_unit *unit = UNIT_GET(unit_index);
		long parent_index = unit->parent_index;

		if (parent_index != NONE)
		{
			short seat_index = unit->parent_seat_index;

			if (seat_index != NONE)
			{
				s_unit *parent = UNIT_GET(parent_index);
				s_unit_seat_definition *seat = &UNIT_SEATS(UNIT_DEFINITION_GET(parent))[seat_index];

				if (*(byte *)seat & 1 || TEST_FIELD_BIT((*(dword *)seat >> 3) & 1))
				{
					unit_index = parent_index;
				}
				for (long child_index = parent->first_child_index; child_index != NONE;
					child_index = UNIT_GET(child_index)->next_sibling_index)
				{
					s_unit *child = (s_unit *)function_badc0(child_index, 3);

					if (child && TEST_FIELD_BIT((*(dword *)(UNIT_DEFINITION_GET(child) + 0xbc) >> 27) & 1) &&
						TEST_FIELD_BIT((child->object_flags >> 26) & 1))
					{
						return child_index;
					}
				}
				return unit_index;
			}
			return unit_index;
		}
		return unit_index;
	}
	return unit_index;
}

/* applies a unit state (0xc6ef0's) to the unit */
// @retail 0xc6de0
void function_c6de0(long object_index, void *control)
{
	s_unit *unit = UNIT_GET(object_index);
	s_unit_state_c6ef0 *state = (s_unit_state_c6ef0 *)control;

	unit->unknown1b0 = state->unknown14;
	unit->unknown1c0 = state->unknown20;
	unit->unknown1c4 = state->unknown24;
	unit->unknown1bc = (char)state->unknown04;
	if (state->unknown06 == unit->unknown214)
	{
		*(dword *)&unit->unknown214 = *(dword *)&state->unknown06;
	}
	if (state->unknown0a != NONE)
	{
		unit->next_grenade_index = (char)state->unknown0a;
	}
	unit->unknown241 = (char)state->unknown0c;
	unit->unknown148 = state->unknown10;
	unit->unknown180 = state->unknown40;
	unit->unknown15c = state->unknown34;
	unit->unknown150 = state->unknown28;
	unit->unknown14c = state->name;
	unit->unknown1a4 = state->unknown4c;
	unit->unknown1c8 = state->tail;
}

/* in mode 4: follows the unit's seat changes with requests: leaving the
   seat it last had (+0x2a0, +0x2a4) and, when it has no parent, going back
   to it (request 0x1c) */
// @retail 0xd0c10
void function_d0c10(long unit_index)
{
	if (g_4e6948->mode == 4)
	{
		s_unit *unit = UNIT_GET(unit_index);

		if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
		{
			long last_parent_index = unit->unknown2a0;

			if (unit->parent_index != last_parent_index || unit->parent_seat_index != unit->unknown2a4)
			{
				if (unit_action_active(unit_index, 0x1f) && last_parent_index == NONE)
				{
					function_e69c0(unit_index, 0x1f);
				}
				if (!unit_action_active(unit_index, 0x1d) && !unit_action_active(unit_index, 0x1e) &&
					!unit_action_active(unit_index, 0x20))
				{
					function_e68c0(0x1e, unit_index);
				}
			}
		}
		if (unit->parent_index == NONE && unit->unknown2a0 != NONE)
		{
			s_unit_request request;

			request.type = 0x1c;
			request.type1c.object_index = unit->unknown2a0;
			request.type1c.seat_index = unit->unknown2a4;
			request.type1c.unknowna = unit_action_active(unit_index, 0x1f) != false;
			request.type1c.unknownb = true;
			function_e6900(unit_index, &request);
		}
	}
}

/* whether the unit may ready the weapon in a state: always in a vehicle,
   never from a seat without bit 5, else by 0x10fcd0; never while +0x13c
   is set and the weapon has bit 29 at +0x12c */
// @retail 0xcd6a0
bool __stdcall function_cd6a0(long unit_index, long unknown, long weapon_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *local_67e06b = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));
	long seat_state_name;
	bool seat_blocks;
	bool result;

	if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
	{
		s_unit_seat_definition *seat =
			&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit->parent_index)))[unit->parent_seat_index];

		seat_state_name = seat->label;
		seat_blocks = !(TEST_FIELD_BIT((*(dword *)&seat->flags >> 5) & 1));
	}
	else
	{
		seat_state_name = 0x7000101;
		seat_blocks = false;
	}
	if (unknown == NONE || unknown == 0x7000101)
	{
		unknown = seat_state_name;
	}
	if ((1 << unit->type) & 2)
	{
		result = true;
	}
	else if (seat_blocks)
	{
		result = false;
	}
	else
	{
		byte available = function_10fcd0(unit_index, unknown, *(long *)(local_67e06b + 0x288),
			*(long *)(local_67e06b + 0x28c));

		result = *(bool *)&available;
	}
	if (unit->unknown13c != NONE && TEST_FIELD_BIT((*(dword *)(local_67e06b + 0x12c) >> 29) & 1))
	{
		result = false;
	}
	return result;
}

/* gives the unit a starting profile (the globals' +0xfc, 0x44 bytes each):
   resets it first when asked, then its two weapons, its two reals and
   its grenades */
// @retail 0xcce00
void function_cce00(long unit_index, short starting_profile_index, bool reset, bool flag)
{
	long const *reference = &unit_index;

	if (*reference != NONE && starting_profile_index != NONE)
	{
		byte *profile = *(byte **)((byte *)g_4e0350 + 0xfc) + starting_profile_index * 0x44;
		s_unit *unit = UNIT_GET(unit_index);

		if (reset)
		{
			function_ccff0(unit_index);
			unit->unknownf0 = 1.0f;
			unit->unknownec = 1.0f;
			*(word *)unit->grenade_counts = 0;
		}
		s_unit_starting_weapon *weapon = (s_unit_starting_weapon *)(profile + 0x28);
		for (long i = 2; i != 0; i--, weapon++)
		{
			if (weapon->tag_index != NONE)
			{
				long weapon_index = function_ccd60(weapon, unit_index, flag);

				if (weapon_index != NONE && !function_cd0c0(unit_index, weapon_index, 1))
				{
					function_b8540(weapon_index);
				}
			}
		}
		unit->unknownf0 -= *(real *)(profile + 0x24);
		unit->unknownec -= *(real *)(profile + 0x20);
		for (long j = 0; j < 2; j++)
		{
			unit->grenade_counts[j] += profile[0x40 + j];
		}
	}
}

/* whether the unit is free to act: not riding, performing none of the
   actions 0x1a, 0x1b, 0x16, 8, 0x12, 0, 1, 0xa and 0xb (0xe6fe0), and
   neither hand's weapon busy (0x100880); the hands are read through a
   copy of the object pool, which looks the unit up again after the calls */
// @retail 0xc85c0
bool function_c85c0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = true;
	s_record_pool *objects = g_4e0300;

	if (unit->parent_index != NONE || unit_action_active(unit_index, 0x1a) || unit_action_active(unit_index, 0x1b) ||
		unit_action_active(unit_index, 0x16) || unit_action_active(unit_index, 8) ||
		unit_action_active(unit_index, 0x12) || unit_action_active(unit_index, 0) ||
		unit_action_active(unit_index, 1) || unit_action_active(unit_index, 0xa) ||
		unit_action_active(unit_index, 0xb))
	{
		result = false;
	}
	if (result)
	{
		for (long hand = 0; hand < 2; hand++)
		{
			short index = (&((s_unit_header *)objects->data)[unit_index & 0xffff].unit->current_weapon_index)[hand];
			long weapon_index = NONE;

			if (index != NONE)
			{
				weapon_index = ((s_unit_header *)objects->data)[unit_index & 0xffff].unit->weapon_object_indices[index];
			}
			if (weapon_index != NONE && function_100880(weapon_index, NONE))
			{
				result = false;
				break;
			}
		}
	}
	return result;
}

/* a unit's scenario placement data (the part the unit reads) */
struct s_unit_placement
{
	real unknown00;
	byte flags;
};

/* places the unit from the scenario: its +0xec and flags, and, when the
   placement says so, starts it dead (animation, damage, no weapons or
   grenades) */
// @retail 0xc42e0
void __stdcall function_c42e0(long unit_index, void const *placement)
{
	s_unit_placement const *data = (s_unit_placement const *)placement;
	s_unit *unit = UNIT_GET(unit_index);

	if (data->unknown00 > 0.0f)
	{
		unit->unknownec = data->unknown00;
	}
	if (data->flags & 4)
	{
		unit->flags_134 |= 0x1000;
	}
	unit->object_flags |= 0x8000;
	if (data->flags & 1)
	{
		unit->flags_134 |= 0x40;
		function_c98a0(0, unit_index, 3, NONE);
		s_unit_animation *animation = UNIT_ANIMATION(UNIT_GET(unit_index));
		if (animation->unknown68 != NONE && animation->unknown00 != NONE && animation->unknown06 != NONE &&
			animation->name == 0xc000043)
		{
			function_10f930(unit_index, 0.1f, true);
		}
		unit->unknownec = 0.0f;
		unit->unknownf0 = 0.0f;
		function_d6800(unit_index, g_467420, false, false);
		function_d6a70(unit_index);
		function_ccff0(unit_index);
		function_ccd20(unit_index);
		*(word *)unit->grenade_counts = 0;
		function_caa60(unit_index, NONE, 0, true, false);
		function_bd020(unit_index);
	}
}

/* picks up a grenade: when the globals allow its type (+0x104, 0x2c bytes
   each) and the unit carries fewer than the type's maximum, counts it,
   tells the player code, and deletes it */
// @retail 0xccba0
bool __stdcall function_ccba0(long unit_index, long grenade_index)
{
	s_unit *grenade = UNIT_GET(grenade_index);
	byte *definition = UNIT_DEFINITION_GET(grenade);
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

	if (*(void **)((byte *)g_4e034c + 0x100))
	{
		short *maximum = (short *)(*(byte **)((byte *)g_4e034c + 0x104) + *(short *)(definition + 0x12e) * 0x2c);

		if (maximum && unit->grenade_counts[*(short *)(definition + 0x12e)] < *maximum)
		{
			if (unit->unknown13c != NONE)
			{
				function_1e9070(unit->unknown13c);
			}
			if (g_4e6948->mode != 4)
			{
				unit->grenade_counts[*(short *)(definition + 0x12e)]++;
				function_a7a30(unit_index, 0x400000);
				function_a8950(unit_index, grenade->definition_index);
				long controller = unit_get_player_index(unit_index) == NONE ? NONE :
					*(short *)((byte *)g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c + 0x28);
				if (controller != NONE)
				{
					function_f8110(grenade_index);
				}
				function_b8540(grenade_index);
				result = true;
			}
		}
	}
	return result;
}

/* an iterator over the units (as 0xfa9a0's over the projectiles): the
   current one, then the object iterator */
struct s_unit_list_iterator
{
	s_unit *datum;
	s_type_f1af8e iterator;
};

/* finds a unit that is dying or down: performing action 0x16, 0x1a or
   0x1b (0xe6fe0), or in animation 0xc000043 or 0xd000042 without flag 2
   at +0x33e and bit 0 at its animation's +0x6c */
// @retail 0xcc170
bool __stdcall function_cc170(long *value)
{
	s_unit_list_iterator iterator;

	function_bae80(&iterator.iterator, 3, 1);
	while ((iterator.datum = (s_unit *)function_baeb0(&iterator.iterator)) != 0)
	{
		long unit_index = iterator.iterator.object_index;
		long name = unit_animation_name(unit_index);
		s_unit *object = iterator.datum;

		if (unit_action_active(unit_index, 0x16) || unit_action_active(unit_index, 0x1a) ||
			unit_action_active(unit_index, 0x1b))
		{
			*value = unit_index;
			return true;
		}
		if ((name == 0xc000043 || name == 0xd000042) && !TEST_FIELD_BIT((*(word const *)((byte *)object + object->unknown33e) >> 2) & 1) &&
			!(*((byte *)object + object->animation_offset + 0x6c) & 1))
		{
			*value = unit_index;
			return true;
		}
	}
	return false;
}

/* the next weapon slot from the given one in a direction whose weapon the
   unit may ready in the state (0xcd6a0), not its second hand's; prefers
   weapons with bit 3 at +0x12c, then (direction 0) the newest */
// @retail 0xcdeb0
short __stdcall function_cdeb0(long unit_index, long state_name, short slot, short direction)
{
	s_unit *unit = UNIT_GET(unit_index);
	short result = NONE;
	bool result_flag = false;
	long best = 0;
	slot = slot != NONE ? (slot + direction + 4) % 4 : 0;
	short i = slot;

	do
	{
		if (unit->weapon_object_indices[i] != NONE &&
			function_cd6a0(unit_index, state_name, unit->weapon_object_indices[i]) && i != unit->next_weapon_index)
		{
			long weapon_index = unit->weapon_object_indices[i];
			long time = unit->unknown228[i];
			bool flag = (*(dword *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x12c) >> 3) & 1;
			bool better;

			if (result == NONE)
			{
				better = true;
			}
			else if (result_flag)
			{
				better = false;
			}
			else if (flag)
			{
				better = true;
			}
			else if (direction == 0)
			{
				better = time > best;
			}
			else
			{
				better = false;
			}
			if (better)
			{
				result = i;
				best = time;
				result_flag = flag;
			}
		}
		i = direction < 0 ? (i + 3) % 4 : (i + 1) % 4;
	} while (i != slot);
	return result;
}

/* starts (state 1, its timer from 0xc5340, flag 3) or ends (state 2, ten
   seconds) the unit's state +0x2bc while its player is in state 1 at
   +0x88 (or the campaign flag allows it), with the globals' sounds */
// score 15 (was 15): ours keeps unit_index in edi and the player data pointer in esi, retail the
// reverse: unit_index reaches 0xc5890 in the register that function hands on to 0x189210, which
// takes the object in edi here and in esi in retail. BLOCKED-BY 0x189210. Tried: a named
// player-state condition 15; the start arm's tests inverted 15; a player index local 15; the
// stop arm first 60; the state byte read in the test 15.
// @retail 0xc5740
void function_c5740(long unit_index, bool start)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->unknown13c != NONE)
	{
		byte state = *((byte *)g_4e8c24->data + (unit->unknown13c & 0xffff) * 0x21c + 0x88);

		if (state == 1 || (g_4e6948->state == 1 && g_4f55dc[0]))
		{
			if (start)
			{
				if (unit->parent_seat_index == NONE && !unit->unknown2bc)
				{
					unit->unknown2bc = 1;
					unit->unknown2be = function_c5340();
					unit->flags_134 |= 8;
					unit->unknown2b8 = g_510c54->field_2_3 * 0.05f;
					function_c5890(unit_index, 0);
				}
				else
				{
					function_c5890(unit_index, 2);
				}
			}
			else if (unit->unknown2bc == 1)
			{
				unit->unknown2bc = 2;
				unit->unknown2be = (short)unit_round(g_510c54->field_2_3 * 10.0f);
				unit->flags_134 &= ~8;
				unit->unknown2b8 = g_510c54->field_2_3 * 0.1f;
				function_c5890(unit_index, 1);
			}
		}
	}
}

/* plays the sounds of a unit's impact: its weapon's (+0x58) when it holds
   a weapon that allows it (0x1012c0), else the globals' material sound
   (+0x154, 0xb4 bytes each) and the definition's +0xac */
// MiniMax A (11-14): no new form, no lower score (97): retail keeps the definition index in ebp and the object globals
// pointer in edi and re-reads the unit index and the material from the stack, while this build keeps the unit index in
// ebx and the material in bp; the effect helper's convention in src/unknown_187ec0.cpp is not this lane's to change.
// MiniMax H (11-14): no new form, no lower score (97): the sound call still takes the point in ECX and the forward on the stack where retail takes the point in EAX and the forward in ECX; that convention belongs to 0x188180, in a file this lane may not edit.
// MiniMax B (11-13): no new form, no lower score (97): retail keeps the definition index in EBP and the object globals pointer in EDI and re-reads the unit index and the material from the stack, while this build keeps the unit index in EBX and the material in BP; the callers' setup fixes that outside this file.
// MiniMax H (11-12): no new form, no lower score (97): retail keeps the definition index in ebp and reads the unit index and the material from the stack at each use where ours keeps the unit index in ebx and the material in bp; the effect helper's parameter convention in src/unknown_187ec0.cpp is not this lane's to change.
// MiniMax S (11-11): no new form, no lower score (90): the body is retail's length and the first difference is at
// +0x0, where retail keeps the definition index in EBP and reads the unit index and the material from the stack at
// each use, while ours keeps the unit index in EBX and the material in BP; the effect helper's parameter convention
// (src/unknown_187ec0.cpp) is not this lane's: BLOCKED-BY 0x188180.
// MiniMax I (11-11): no new form, no lower score (90); the prologue differs - retail keeps the definition
// index in EBP and reads the unit index and the material from the stack at each use, ours keeps the unit
// index in EBX and the material in BP, and the call to 0x188180 is unmatched in another lane's file.
// MiniMax R (10-31): no new form, no lower score: retail passes the point in ECX and again on the stack and
// never the forward to 0x188180, whose parameter types live in src/unknown_187ec0.cpp.
// MiniMax G (11-12): no new form tried this round, no lower score (90): the call to 0x188180 still needs its
// parameter types fixed in another lane's file, so the call setup cannot be matched from here.
// MiniMax H (11-13): no new form, no lower score (97); retail keeps the definition index in EBP and the object globals pointer in EDI and re-reads the unit index and the material from the stack, while this build keeps the unit index in EBX and the material in BP.
// score 94 (was 94): Frameless length agrees; definition/material/pool register ownership and call setup remain.
// Pool snapshot 92 (two-point gain rejected); headers 94; shared tag lifetime 94; pool/tag 92;
// direct unit observations 94; conditional material result 94; weapon getter 121; pool/getter 135;
// existing masked-index getter 94. Original body restored.
// Round 178: retail passes all five on the stack (ret 0x14) and pushes the material dword unextended;
// every 0xcbd50 form here makes our LTCG pass two in registers (ret 0xc), inline 0x1012c0 and break
// matched 0xcf040 (11): 0xcbd50 alone 121, with a unit local 121, with if/else arms and a short-
// parameter material lookup 121, the same with a long material parameter 125 (0xcf040 228); a char
// slot index in the original body 99.
// Round 180: the material lookup through the existing global_material_get 94 (identical code, kept), as a
// named material local 94; with 0xcbd50 for the slot weapon 121 (ret 0xc again); a pool snapshot 94; the
// unit or material parameter read once through its address 94/94. Diagnostics: no material argument to
// 0x188180 flips the convention (material in a register, 331 bytes, 115); a (word) argument 94. Retail
// pushes the material slot's raw dword to 0x188180 (its seventh parameter is a short there, a long in
// our declaration and definition), reloads the pool's data after 0x1012c0 (our LTCG treats that matched
// callee as read-only) and keeps the definition index in EBP.
// Round 37: the weapon's definition and root lookups through a block pool local used twice 96 (retail's
// data reload after 0x1012c0 and its EDI/ESI roles, but the prologue shifts); the slot reads through it instead 96;
// the local first at function scope 96, after the slot read 96. Diagnostic only, on a link of this file with a
// scratch copy of 0x188180's definition (not kept): its seventh parameter as a short, alone and with the pool local,
// moves no closer (the material then sits in EBP; our 0x188180 still takes point, forward and the effect value in
// other registers than retail's EAX, ECX and stack). BLOCKED-BY 0x188180 (parameter type and register convention).
// Round 57 (m3 lane 9): retail's 0xcf3d0 also pushes the material's raw dword, so the short parameter here is right;
// 0x188180's seventh parameter is declared in two held files. Retail holds definition_index in EBP, the tag in EBX,
// the weapon in ESI and the object array in EDI, re-reading unit and material from the stack. A NONE-initialised
// weapon index with if/else exits 121 (333 bytes, 0xcf040 11); both sound plays through an inline helper 94; the
// material path as an inline helper 260 (kept out of line, 0xcf040 6); an early return on a NONE definition 94;
// `goto done` for the weapon exit 94. Diagnostic (reverted): a global store in 0x1012c0 gives the pool reload but
// 96 (351 bytes), so that reload is not the main cause.
// Lane 18 round 24 (fast): separate material/definition tag locals 94 (same code); a NONE-initialised weapon index with
// the early return kept 121 (333 bytes, 0xcf040 11); the definition tag as a ternary 94 (same code); `if (tag_index ==
// NONE) return;` before the 0x188180 call 94 (same code); the weapon slot field read at each use 99. Retail sets EAX
// (point) before ECX (forward) for 0x188180. Still BLOCKED-BY 0x188180 (parameter type and register convention).
// Round 281: full-width material local 106; full-width slot 110; both 118; early receiver declarations 94;
// nested weapon gates 94; named admission result 94; early receivers with full-width material 106;
// immutable definition-argument lifetime 94. Full-width material forms also lose matched 0xcf040 (232).
// Round 350: Single-exit weapon/fallback with guarded/conditional handle production, short/char slot and
// original/pool receivers: 121/121/124/124/123/123/123/123. Definition/material/pool ownership and
// effect convention remain.
// Round 144 (m3 lane 5, fast; retail's 0x188180 call takes the point in EAX and the forward in ECX, ours the effect
// value in ECX, so the first differences are that callee's convention, BLOCKED-BY 0x188180): the definition's tag read
// before the material play 103; the unit read at each use 94; a weapon unit local 94; a named root index 94; the
// material tag read at entry 106. Original body restored.
// M341: Native root fields through mutable/const pointer/reference, each with expression
// or in-place final definition index: 94/94/94/94/94/94/94/94. Original root/index body
// restored; definition/material/pool ownership and effect-call convention remain.
// Round 1 of #1049 (fresh walk against retail 0xceee0-0xcf03b, fast, still 94): no logic difference left: weapon
// slot, 0x1012c0 gate, definition tag +0x58, root through 0xbaf80, the 0x188180 argument list (point in EAX,
// forward in ECX, tag, unit, 0xf, 0, raw material dword, root+0x70, root+0x28, 1.0), the material record's
// +0x64 and the definition record's +0xac sounds all agree. What differs is only the prologue and the 0x188180
// call setup: retail keeps definition_index in EBP (loaded first), the object globals pointer in EDI (reloaded
// through [edi+0x44] after 0x1012c0), and reads unit_index and the material from the stack at each use, while
// ours puts unit_index in EBX and the material in BP; and 0x188180 is still unmatched (bipeds.cpp, items.cpp
// and unknown_187ec0.cpp), so which registers carry its first arguments is not ours to decide.
// MiniMax R (11-11): no new form tried this round, no lower score (90): the body is retail's length and the first difference is at +0x0, so what is left is the register home of the two index arguments and the width of the material argument, which the callers' setup fixes outside this file.
// Coordinator (11-12, lane 13 r85's held change): 0x188180's seventh parameter now carries the material as c_type_47f957,
// the type its callee 0x1883e0 already takes (0xe4d90 168 -> 138, 0x10c880 69 -> 39, 0xec4f0 23 -> 8, 0x247070 30 -> 6);
// this function rose 90 -> 97 with it. Its own parameter as that type gives 114 fast and needs contributor 0xcf040's call line.
// MiniMax A (11-13): no new form tried this round, no lower score (97): retail keeps the definition index in EBP
// and the object globals pointer in EDI and re-reads the unit index and the material from the stack, while this
// build keeps the unit index in EBX and the material in BP; the callers' setup fixes the convention outside this
// file: BLOCKED-BY 0x188180 (src/unknown_187ec0.cpp).
// @retail 0xceee0
void __stdcall function_ceee0(long unit_index, long definition_index, short material_index, point3f const *point,
	vector3f const *forward)
{
	s_unit *unit = UNIT_GET(unit_index);
	short index = unit->current_weapon_index;

	if (index != NONE)
	{
		long weapon_index = unit->weapon_object_indices[index];

		if (weapon_index != NONE && function_1012c0(weapon_index))
		{
			long tag_index = *(long *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x58);

			if (tag_index != NONE)
			{
				byte *root = (byte *)UNIT_GET(function_baf80(weapon_index));

				function_188180(point, forward, tag_index, unit_index, 0xf, 0, c_type_47f957(material_index), (long)(root + 0x70),
					(s_location const *)(root + 0x28), 1.0f);
			}
			return;
		}
	}
	long tag_index = *(long *)(global_material_get(material_index) + 0x64);
	if (tag_index != NONE)
	{
		function_189060(unit_index, NONE, 1.0f, g_468788, g_4687a8, tag_index);
	}
	if (definition_index != NONE)
	{
		tag_index = *(long *)(g_4e3b44[definition_index & 0xffff].bytes + 0xac);
		if (tag_index != NONE)
		{
			function_189060(unit_index, NONE, 1.0f, g_468788, g_4687a8, tag_index);
		}
	}
}

/* forgets an object everywhere the unit refers to it: +0x248, +0x24c, its
   weapon slots (and hands), +0x238, the seat it last had and its
   animation state's targets */
// @retail 0xced30
void __stdcall function_ced30(long unit_index, long object_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->unknown248 == object_index)
	{
		unit->unknown248 = NONE;
	}
	if (unit->unknown24c == object_index)
	{
		unit->unknown24c = NONE;
	}
	long *slot;
	long i;

	for (i = 0, slot = unit->weapon_object_indices; i < 4; i++, slot++)
	{
		if (*slot == object_index)
		{
			if (i == unit->current_weapon_index)
			{
				unit->current_weapon_index = NONE;
			}
			if (i == unit->next_weapon_index)
			{
				unit->next_weapon_index = NONE;
			}
			function_cea70(4, (short)i, NONE, unit_index);
		}
	}
	if (unit->current_weapon_index == NONE)
	{
		unit->unknown216 = (char)function_cdeb0(unit_index, NONE, NONE, 0);
	}
	if (unit->unknown238 == object_index)
	{
		unit->unknown238 = NONE;
	}
	if (unit->unknown2a0 == object_index)
	{
		unit->unknown2a0 = NONE;
		unit->unknown2a4 = NONE;
	}
	s_unit *again = UNIT_GET(unit_index);
	byte *state = (byte *)again + again->unknown346;
	if (*(long *)(state + 0x10) == object_index)
	{
		*(long *)(state + 0x10) = NONE;
		*(short *)(state + 0x14) = NONE;
	}
	if (*(long *)(state + 0x18) == object_index)
	{
		*(long *)(state + 0x18) = NONE;
	}
}

/* the request a weapon event sends the unit holding the weapon (and the
   unit +0x24c names): by the event type, for its first or second hand */
// @retail 0xc9d00
void __stdcall function_c9d00(long unit_index, long weapon_index, long type)
{
	if (type)
	{
		s_record_pool *objects = g_4e0300;
		short index = ((s_unit_header *)objects->data)[unit_index & 0xffff].unit->current_weapon_index;
		long current_weapon_index = NONE;

		if (index != NONE)
		{
			current_weapon_index = ((s_unit_header *)objects->data)[unit_index & 0xffff].unit->weapon_object_indices[index];
		}
		bool first = weapon_index == current_weapon_index;
		bool second = weapon_index == function_cbd50(unit_index, UNIT_GET(unit_index)->next_weapon_index);
		long action;

		if (first)
		{
			switch (type)
			{
			case 3:
				action = 4;
				break;
			case 4:
				action = 5;
				break;
			case 1:
				action = 2;
				break;
			case 2:
				action = 3;
				break;
			case 7:
				action = 6;
				break;
			case 8:
				action = 7;
				break;
			default:
				return;
			}
		}
		else if (second)
		{
			switch (type)
			{
			case 3:
				action = 0xe;
				break;
			case 4:
				action = 0xf;
				break;
			case 1:
				action = 0xc;
				break;
			case 2:
				action = 0xd;
				break;
			case 7:
				action = 0x10;
				break;
			case 8:
				action = 0x11;
				break;
			default:
				return;
			}
		}
		else
		{
			return;
		}
		function_e68c0(action, unit_index);
		s_unit *unit = UNIT_GET(unit_index);
		if (unit->unknown24c != NONE)
		{
			function_e68c0(action, unit->unknown24c);
		}
	}
}

/* evens out two motions: the one that would finish first gives up part of
   its acceleration so both end together (or stops when its speed would
   vanish) */
// @retail 0xc75d0
void function_c75d0(s_unit_motion *a, s_unit_motion *b, real scale)
{
	if (!a->done && !b->done)
	{
		real total_a = a->s_type_8c87de + a->coast_time + a->field_10_4;
		real total_b = b->s_type_8c87de + b->coast_time + b->field_10_4;
		real difference;
		s_unit_motion *motion;

		if (a->field_10_4 > 0.0f && total_b > total_a)
		{
			difference = total_b - total_a;
			motion = a;
		}
		else if (b->field_10_4 > 0.0f && total_a > total_b)
		{
			motion = b;
			difference = total_a - total_b;
		}
		else
		{
			return;
		}
		if (motion)
		{
			real x = (difference + motion->coast_time) * scale;
			real speed = (real)fabs(motion->field_10_4 * motion->acceleration + motion->velocity);
			real step = ((real)sqrt(x * x - (0.0f - speed * difference) * scale * 4.0f) - x) / (scale + scale);
			real limit = motion->field_10_4 > motion->s_type_8c87de ? motion->s_type_8c87de :
				motion->field_10_4;

			if (step > limit)
			{
				step = motion->field_10_4 > motion->s_type_8c87de ? motion->s_type_8c87de :
					motion->field_10_4;
			}
			if (step > 0.0f)
			{
				real velocity = (motion->field_10_4 - step) * motion->acceleration + motion->velocity;

				if (0.0001f > (real)fabs(velocity))
				{
					motion->done = true;
					motion->field_10_4 = 0.0f;
					motion->s_type_8c87de = 0.0f;
					motion->coast_time = 0.0f;
					return;
				}
				motion->field_10_4 -= step;
				motion->s_type_8c87de -= step;
				motion->coast_time = ((motion->acceleration * step + velocity * 2.0f) * step) / velocity;
			}
		}
	}
}

/* moves the unit's two fades toward their targets each tick: +0x2b0 by
   +0x2b8 per second up to 1 while flag 3 is set (set here when its team
   +0x138 is not at peace with the campaign's team 16), else down to 0;
   +0x2b4 by a third per second by flag 4 */
// @retail 0xc6810
void function_c6810(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool clamped = false;

	if (unit->actor_index != NONE && g_4e6948->state == 1 && g_4f55dc[2] && team_is_enemy(1, unit->unknown138))
	{
		unit->flags_134 |= 8;
	}
	if (TEST_FIELD_BIT((unit->flags_134 >> 3) & 1))
	{
		unit->unknown2b0 += unit->unknown2b8 * g_510c54->rate;
		if (unit->unknown2b0 > 1.0f)
		{
			unit->unknown2b0 = 1.0f;
			clamped = true;
		}
	}
	else
	{
		unit->unknown2b0 -= unit->unknown2b8 * g_510c54->rate;
		if (0.0f > unit->unknown2b0)
		{
			unit->unknown2b0 = 0.0f;
			clamped = true;
		}
	}
	if (clamped)
	{
		s_unit *current = UNIT_GET(unit_index);

		if (current->unknown0d4 != NONE)
		{
			function_b58c0(current->unknown0d4, 0x800000);
		}
	}
	if (TEST_FIELD_BIT((unit->flags_134 >> 4) & 1))
	{
		unit->unknown2b4 += g_510c54->rate * 0.33333334f;
		if (unit->unknown2b4 > 1.0f)
		{
			unit->unknown2b4 = 1.0f;
		}
	}
	else
	{
		unit->unknown2b4 -= g_510c54->rate * 0.33333334f;
		if (0.0f > unit->unknown2b4)
		{
			unit->unknown2b4 = 0.0f;
		}
	}
}

/* drops a weapon of the unit: from its first hand by request 9, from its
   second hand by request 0x13 (both through 0xe6900), else from its slot
   (0xce920); retail has 0xe6900 expanded in the first branch only, so that
   branch is written out and inline_depth(0) keeps the second a call */
#pragma inline_depth(0)
// MATCH (was 150, 60). The request-9 branch is written out (0xe6900 expanded there only) and the player's
// controller is read twice through one player pointer: retail does not fold the +0x28 into the address, which a
// single read (60) or a pointer to the field (60, 30) does. Tried: both branches calling 0xe6900 179; a noinline
// declaration of 0xe6900 150 (not honoured); the call sites reordered 179; a shared request local 179; the
// requester unit and its player index re-read 60.
// @coldspear (#104, 2026-10-30): reading the request definition g_4677c8[9] through a volatile pointer gives 26,
// not kept: retail re-reads the definition there.
// @retail 0xce520
void function_ce520(long weapon_index, long unit_index, bool flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	long slot = NONE;

	for (long i = 0; i < 4; i++)
	{
		if (unit->weapon_object_indices[i] == weapon_index)
		{
			slot = i;
			break;
		}
	}
	if (slot == unit->current_weapon_index)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 9;
		request.type17.unknown4 = !flag;
		s_unit_request_definition *definition = g_4677c8[9];

		function_b7360(unit_index);
		bool result = definition->perform(unit_index, &request);
		if (unit_index != NONE)
		{
			s_unit *requester = UNIT_GET(unit_index);

			if (requester->unknown13c != NONE)
			{
				byte *player = (byte *)g_4e8c24->data + (requester->unknown13c & 0xffff) * 0x21c;

				/* two reads of the controller: retail keeps the player pointer and loads +0x28 from it */
				if (*(short *)(player + 0x28) != NONE)
				{
					function_1e6980((s_time_entry *)((byte *)g_51e9c0 + *(short *)(player + 0x28) * 0x1b0 + 0x150),
						(short)request.type, result);
				}
			}
		}
	}
	else if (slot == unit->next_weapon_index)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 0x13;
		request.type17.unknown4 = !flag;
		function_e6900(unit_index, &request);
	}
	else
	{
		function_ce920(unit_index, slot, 0, flag);
	}
}

#pragma inline_depth()

/* where the unit's camera sits and its camera data: its seat's marker on
   the parent (when the parent has it), else the unit itself */
// MiniMax G (11-13): no new form tried this round, no lower score (91): the seat index's multiply
// is scheduled before the parent lookup, the seats array comes back in ecx where retail uses esi,
// the unit spill is read after the matrix copy, and the camera data is read at [ebp+0x24] where
// retail copies the base up by 0x24.
// MiniMax H (11-13): no new form, no lower score (91): the seat index's multiply is still scheduled before the parent
// lookup, the seats array still comes back in ecx where retail uses esi, the unit spill is still read after the matrix copy
// and the camera data is still read at [ebp+0x24] where retail copies the seat base up by 0x24; this round a named parent
// definition (91, and the seat macro's 0x1cc follows the pointer's type, so the offset it adds changed) and a seat pointer
// advanced by 0x24 (91, the compiler folds the offset back into the accesses) were both measured and restored; the
// permuter over this body tried 54 rewrites in its window and none of them lowered its 86 differing instructions.
// MiniMax G (11-11): no new form, no lower score (113); retail keeps the false result as a stack byte and loads the camera
// argument only inside each branch with the seat in ebp, while this build folds the result to xor al,al, hoists the camera into
// ebp at entry and spills the seat; every form that names the camera twice keeps the camera in ebp.
// MiniMax G (11-11): later in the same round, still no lower score (113); the one-return forms (the result set on both
// paths with a single return at the end) score 116 to 131 depending on where the result byte is written, and a second
// view of the unit pointer (the seat read through it) scores 113 like the standing body, so the camera's home and the
// seat's home still cannot both match.
// MiniMax B (11-12): no new form, no lower score (113): a second view of the unit pointer for the parent lookup (the idiom that matches 0x13992a) reproduces retail's parent split (the compare in memory and the reload) and puts the seat in ebp, but the frame then loses four bytes (0x74 against retail's 0x78, the seat needs no spill) and scores 117; the single-return form that keeps retail's result byte promotes the camera into ebp instead of the seat (125 alone, 127 with the second view).
// MiniMax B (11-12): score 91 (was 113): copying the camera data field by field instead of one struct assignment keeps the camera argument out of ebp (the struct assignment takes its address, so the pointer is hoisted into ebp at entry), and with the parent index read at the test and at the call the parent test compares in memory and the parent reloads into ebx as retail does; the frame is retail's 0x78 and the result is retail's stack byte. Left: the seat's index multiply is scheduled before the parent lookup, the seats array is in ecx rather than esi, the unit spill is read after the matrix copy, and the camera data is read at [ebp+0x24] rather than the base being copied up by 0x24.
// score 91 (was 113; earlier 116): copying the camera data field by field (five float assignments) instead of one
// struct assignment keeps the camera argument off ebp - the struct assignment takes its address, so the pointer is
// hoisted into a callee-saved register at entry - and reading the parent index at the test and at the call (no local)
// makes the parent test compare in memory and the parent reload into ebx as retail does. The frame is retail's 0x78 and
// the result is retail's stack byte. Left: the seat's index multiply is scheduled before the parent lookup instead of
// after it, the seats array comes back in ecx instead of esi, the unit spill is read after the matrix copy instead of
// before it, and the camera data is read at [ebp+0x24] instead of the base being copied up by 0x24.
// Earlier at 113: false folded to xor al,al, camera held in ebp and seat spilled; block order agreed.
// Tried (all 113 or worse): result passed as the flag argument of 0xb8d30 (113), result set true on both paths
// with one return (125), volatile result (121), marker declared at the top (113), camera through a local (113).
// Round 5 of #1049: no logic difference in a fresh walk. The single-return form (`result = true;` after each copy,
// `return result;`) is what gives retail's stack byte (`mov [esp+0x13],0` at entry, `mov al,[esp+0x13]` on the
// failed-marker path) and the three-epilogue layout, but it scores 125 because the camera pointer still takes EBP at
// entry (retail leaves it on its stack slot and loads it into EAX for each copy, so the seat stays in EBP and only
// the unit and result spill) and the seat spills an extra slot (frame 0x7c versus 0x78). Also tried with that form:
// `result = true` before the copy 126; one `byte *source` for the seat and the definition 125; memcpy for the camera
// copy 125. 66 callers; none was changed. Unfixed: nothing found that lowers the camera's register priority.
// Round 144 (m3 lane 7): single-return form 125 again; with a `short seat_index` local 125; `unit->parent_index` read at
// the call 127. One copy after the if/else from a function-scope `camera_data` pointer (`if (result) *camera =
// *camera_data;`) 152 (342 bytes) gives retail's registers (camera read at the copy, seat or definition in EBP, the result
// a stack byte) but no second copy of the tail; NULL-initialised pointer tested 148; memcpy 152; `return true` inside 170;
// early failure return 174; two copies from the shared pointer 130 (the camera is hoisted whenever it is named twice).
// M3 lane 2 r72 (fast; masked = registers/slots-masked aligned count, base 32): `unit->parent_index` read at the seat
// lookup and the call (no parent local) 117 (masked 30); that with the single-return form 127 at 403 bytes, masked 18
// (the parent test reads memory and the parent loads into EBX as retail; only the camera/seat swap for EBP is left).
// On that form: result declared first 126; marker declared at the top or first in the block 127 (same code); one
// function-scope `byte *source` for the seat and the definition 127; a named parent unit, parent definition or seat
// array 127 each; the test inverted with the plain arm first 127; the copies through a two-use inline copy helper
// (forceinline or inline) 127; the camera's address taken (`(void)&camera`, or both copies through a const reference
// to it) 127, all the same code: the camera keeps EBP in every form that names it twice.
// MiniMax A (11-11), NOT kept (coordinator 11-11): 113 -> 78 with the camera parameter and the result made volatile; volatile is reverted, body at 113.
// Typed seats/acceleration fields 113; marker-result producer 115; CRT matrix transfer 113;
// shared axis setters 112 (NOT kept: gain one); CRT camera/axis transfers 113/113;
// plain axis traversals 128; sole-return failure jump with shared camera transfer 152.
// NOT kept: subthreshold setter gain and tied or larger differences; original control and copies restored.
// MiniMax A (11-12): no new form, no lower score (113): retail compares the parent index in memory and reloads it into ebx
// where ours keeps it in ecx, and the camera copy still takes ours's register home instead of retail's ebp.
// P434: current entry has native parent reload, camera residency and frame 0x78; first +0x41 is
// the arm span, then signed seat-index scheduling and copy/base registers. Complete camera record
// assignment 132; actual acceleration fields, camera/acceleration record references, seat lifetime,
// parent-marker name, actual-source lifetime and separate seat indexing all 91. NOT kept: ties or
// regressions; original body restored. Current copy tails remain six bytes longer than retail.
// Uniformly readonly tag receivers 91. Required parent seat-table access through the existing
// unit.seats.address field 91 in fast/plain checks, retained with no code-size or score change.
// P456: native 0x78 frame and parent reload retained. Direct owner camera reads 91; existing vector
// setter/complete vector prefix 106/111; plain five-component ordinal/cursors 91/91; result after
// scale 91; byte result 85 at native 396 bytes (NOT kept: gain six below eight); marker before seat on
// scalar-copy graph 91. Original copies/result restored; seat scheduling and native copy-base
// advancement remain.
// MiniMax A (11-14): no new form, no lower score (91): the seat camera copy still folds +0x24 into
// each of the five member reads where retail advances the seat pointer once and copies the five
// dwords from it; copying the whole five-field camera record (with and without the const cast)
// grows the frame to 0x7c and scores 142, so it is not kept.
// @retail 0xd1080
bool function_d1080(long unit_index, transform4x3f *matrix, s_unit_camera_data *camera)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

	matrix->scale = 1.0f;
	if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
	{
		s_unit_seat_definition *seat =
			&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit->parent_index)))[unit->parent_seat_index];
		s_object_marker marker;

		if (function_b8d30(unit->parent_index, *(long *)((byte *)seat + 8), &marker, 1, false))
		{
			*matrix = marker.matrix;
			function_b9dd0(unit->parent_index, &matrix->position);
			s_unit_camera_data const *source = (s_unit_camera_data const *)((byte *)seat + 0x24);
			camera->unknown00 = source->unknown00;
			camera->unknown04 = source->unknown04;
			camera->unknown08 = source->unknown08;
			camera->unknown0c = source->unknown0c;
			camera->unknown10 = source->unknown10;
			result = true;
		}
	}
	else
	{
		s_unit_definition *definition = (s_unit_definition *)UNIT_DEFINITION_GET(unit);
		function_b9dd0(unit_index, &matrix->position);
		matrix->forward = unit->forward;
		matrix->up = unit->up;
		s_unit_camera_data const *source = (s_unit_camera_data const *)&definition->unit.acceleration;
		camera->unknown00 = source->unknown00;
		camera->unknown04 = source->unknown04;
		camera->unknown08 = source->unknown08;
		camera->unknown0c = source->unknown0c;
		camera->unknown10 = source->unknown10;
		result = true;
	}
	return result;
}

/* the state of a unit's weapon use: 6 or 5 for the flags when asked, 7
   past the definition's limit at +0x114, else 3; otherwise 0xc96b0's
   range (2 when forced, at most 1 when held), clamped to 1 for players */
// @retail 0xc9770
long __stdcall function_c9770(long unit_index, bool flag, bool a, bool b, bool held, bool force, real c, real d)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	long result = 0;
	long range = 0;

	if (flag)
	{
		if (a)
		{
			result = 6;
		}
		else if (b)
		{
			result = 5;
		}
		else if (*(real *)(definition + 0x114) > 0.0f && unit->unknownf8 + d > *(real *)(definition + 0x114))
		{
			result = 7;
		}
		else
		{
			result = 3;
		}
	}
	else if (!TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2))
	{
		function_c96b0(unit_index, &range, c, d);
		if (force)
		{
			result = 2;
		}
		else if (held && range > 1)
		{
			result = 1;
		}
		else
		{
			result = range;
		}
		if (TEST_FIELD_BIT((unit->flags_134 >> 5) & 1) || (g_4e6948->state == 1 && g_4f55dc[7]) ||
			unit->unknown13c != NONE)
		{
			if (result > 1)
			{
				result = 1;
			}
		}
	}
	return result;
}

/* whether the object's root, simulated by Havok and not keyframed, has
   something in the way (0x168f40 from its main rigid body's position) */
// MiniMax G (11-13): no lower score (12): the body is retail's length and the only difference is the pair of saved
// registers - the root index sits in edi here and in ebp in retail, with the rigid body's byte offset in the other.
// This round tried six more forms (the result local first, the NONE constant written as -1, a while loop for the root
// walk, the byte offset in a local, the root read first and the result local last) and all of them score 12, so the
// choice between the two registers does not follow from the source.
// The sixteen-bit rigid-body index reaches retail's saved registers and its address order (mov ecx, dword ptr [edx +
// edi + 0x40]) but the NONE test then narrows to sixteen bits (cmp ax, 0xffff) where retail sign-extends (movsx eax,
// ax; cmp eax, -1), while the wide index keeps the thirty-two-bit test and swaps the registers again; a permuter run
// (736 tries, 612 s) found only a one-use local for the pool data and a redundant test, which are not kept.
// MiniMax H (11-12): no new form, no lower score (12): the root index still sits in edi where retail keeps it in ebp and the rigid body's byte offset in the other register; the short root index that gives retail's registers tests at 16 bits before extending.
// MiniMax H (11-11): no new form, no lower score (12): the body is retail's length and the only difference is the
// pair of saved registers: the root index sits in edi here and in ebp in retail, and the rigid body's byte offset
// takes the other; the rigid body pointer kept in a local scores 52 and the same call kept in a local scores 12.
// MiniMax S (11-11): no new form, no lower score (12): the body is retail's length and the only difference is the
// pair that tests the rigid-body index against -1, which sits at +0xe in ours and in retail's own registers, while
// the root index and the NONE constant are kept in EBP where retail puts them in EDI; the sixteen-bit form of the
// root index gives retail's registers but tests at 16 bits before extending, so the two cannot be had together.
// MiniMax I (11-11): no new form, no lower score (12); the root index and the NONE constant sit in EBP
// where retail puts them in EDI and keeps the byte offset there, and the short root index that would give
// retail's registers breaks the sixteen-bit semantics of the index.
// MiniMax G (11-12): no new form tried this round, no lower score (12): the root index and the rigid-body offset
// are still in the opposite registers from retail, and the short index that would swap them changes the width test.
// score 12 (was 12): root and rigid-body offset saved registers differ (a short index gives
// retail's registers but tests at 16 bits before extending: 3 aligned differences, 293 bytes);
// entry result 89; native short body index 12 at 293 bytes; root cursor/seed 110/96;
// parent while, existing keyframe method and point first 12; point value copy 46;
// 0xbaf80 call for the root 121 at 258 bytes (our link keeps the call); one combined
// index/fixed/keyframed test 12; root getter before the header 12; point read x, y, z 22.
// This round: a short root index 62 and an unsigned short one 77 (the saved registers do not
// change); int, unsigned int and unsigned long roots 12; a short rigid-body index 12 at 293
// bytes and an unsigned short one 13; a short loop variable 83 and an unsigned short one 18;
// the root loop over the root itself 22; the loop variable outside the `for` 12; the root index
// in a nested block 12; the rigid-body index declared before the root index 12.
// MiniMax R (10-31): no new form, no lower score.
// Round 94: the existing root helper inlined by dropping its noinline declaration 12, with an inline
// definition 12; a typed header view 18; root and body-index role renames 12/12; root and component
// predeclarations 12/12; a guarded do/while root traversal 12; a short body result copied to a
// widened body index 12. Original body and helper declaration restored.
// MiniMax R (11-11): no new form tried this round, no lower score (12): the body is retail's length and the first difference is at +0xe, in the pair that tests the rigid-body index against -1; every form that changes that test also changes which registers the whole function keeps, so the EBP against EDI choice and the test cannot be made separately.
// Round 430: direct query result 31; absent-body exit 102; typed fixed
// field 12; separate body tests 12; root getter after the header 12;
// branch result initialization 104; fixed-body exit 99; typed header
// with the salt at four 12. All executable forms restored.
// MiniMax B (11-13): no new form, no lower score (12): the body is retail's length and the only difference left is the
// pair of saved registers - the root index sits in edi here and in ebp in retail, and the rigid body's byte offset takes
// the other; the sixteen-bit root index that gives retail's registers changes the width of the NONE test, so the two
// cannot be had together.
// P455: Captured/direct point aggregate 12/22; captured/direct shared setter
// 12/12; standard point copy 46; entry point storage 12; setter result as
// query input 12; post-root point storage 12. NOT kept: no match; wide
// body comparison and original body restored; root/body saved-register pair remains.
// M3 lane 11 r97 (fast; control 12): cause narrowed. The pair is a ranking of two saved-register values: the root
// index (two definitions, one inside the walk) ranks above the rigid-body element offset here and below it in retail.
// Diagnostics that give retail's pair (root EBP, offset EDI), all with other code changes: root left uninitialised 87
// (298 bytes); a third element read after the query call 36 (305 bytes); the keyframed test ahead of the fixed test 48
// (293 bytes, the index itself kept across the call); two extra early tests with their own loads 91, three 101; two
// float tests before the query call 62; no loop, one conditional definition 102. Same pair as main (EDI/EBP): x through
// a third lookup 38; short root 62; one extra early test 74 or 81; two more tests on the loaded flags byte 85; one, two,
// six extra early stores 83/94/119; 1 to 8 unused locals 12 each (not a symbol-count phase). Natural forms: the masked
// root in a named local 12; header flags byte in a named local 25 (294 bytes); component index assigned in the test 12;
// typed `rigid_bodies[i].rigid_body->m_fixed` / `->m_motion` with or without `.data` 12/12; a named element offset
// (index * sizeof element) 12; a reference to the array 12; an element reference for the first read only 12; a named
// reference to the element for both reads 21 at 292 bytes (retail's pair, first difference +0x52, but the element
// address is kept across the keyframed call where retail keeps the offset and reloads the array); the loop's parent
// through unit_at_masked_index 12; the header through object_header_get 12; both 12; a local forceinline copy of the
// walk 12. Splitting the root from the walk: a walk-local current index with the root assigned once after it, as a
// while with a parent local 113, a guarded do/while 91, a guarded for 91 (retail's pair, but the walk's index takes EDX,
// the pool pointer ESI and a `mov ebp, edx` follows the loop); a walk-local last index under no guard 12 (folds away);
// the root's NONE in an else arm 95 / 86 (retail's pair, `or ebp, -1` after the loop). Permuter (fast, anneal, 600 s,
// 71 tries): best 12.
// @retail 0xcc460
bool __stdcall function_cc460(long object_index)
{
	long root_index = NONE;

	for (long index = object_index; index != NONE; index = UNIT_GET(index)->parent_index)
	{
		root_index = index;
	}
	byte *header = (byte *)g_4e0300->data + (root_index & 0xffff) * 0xc;
	s_unit *root = ((s_unit_header *)header)->unit;
	bool result = false;

	if (*(short *)(header + 4) != NONE && (header[2] & 1) && (header[2] & 0x40) &&
		TEST_FIELD_BIT((root->unknownc0 >> 6) & 1) && root->unknownb4 != NONE)
	{
		s_havok_component *component = havok_component_get(root->unknownb4);
		long rigid_body_index = function_0b67a0((s_small_index const *)component);

		if (rigid_body_index != NONE)
		{
			if (!((byte *)havok_component_rigid_body_get(rigid_body_index, component))[0x40] &&
				!havok_component_rigid_body_keyframed(rigid_body_index, component))
			{
				point3f const *position =
					(point3f const *)(*(byte **)((byte *)havok_component_rigid_body_get(rigid_body_index, component) + 0x3c) + 0x70);
				real z = position->z;
				real x = position->x;
				real y = position->y;
				point3f point;

				point.x = x;
				point.y = y;
				point.z = z;
				if (function_168f40(0x14800005, (s_vehicle_ray const *)&point, root_index, object_index))
				{
					result = true;
				}
			}
		}
	}
	return result;
}

/* the unit's team (+0x138): its player's, else its actor's, else for a
   vehicle the team its riders share (riders in seats with bit 11 count
   apart; any disagreement leaves it NONE) */
// @retail 0xc6990
void function_c6990(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2))
	{
		return;
	}
	if (unit->unknown13c != NONE)
	{
		unit->unknown138 = *((char *)g_4e8c24->data + (unit->unknown13c & 0xffff) * 0x21c + 0xc0);
		return;
	}
	if (unit->actor_index != NONE)
	{
		unit->unknown138 = *(short *)((byte *)g_4f55f0->data + (unit->actor_index & 0xffff) * 0x888 + 0x24);
		return;
	}
	if (unit->type != 1)
	{
		return;
	}
	short team = NONE;
	short other_team = NONE;
	s_object_child_iterator iterator;

	function_d0620(&iterator, unit_index);
	while (function_d0690(&iterator))
	{
		if (team != NONE && other_team != NONE)
		{
			break;
		}
		s_unit *rider = UNIT_GET(iterator.child_index);

		if (iterator.child_short != NONE && iterator.child_value != NONE)
		{
			s_unit_seat_definition *seat =
				&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(iterator.child_value)))[iterator.child_short];

			if (TEST_FIELD_BIT((*(dword *)&seat->flags >> 11) & 1))
			{
				other_team = rider->unknown138;
			}
			else
			{
				team = rider->unknown138;
			}
		}
	}
	if (team == NONE)
	{
		unit->unknown138 = NONE;
	}
	else if (team == other_team || other_team == NONE)
	{
		unit->unknown138 = team;
	}
	else
	{
		unit->unknown138 = NONE;
	}
}

/* the entry animation (of five) whose end point on the vehicle's seat is
   nearest the unit's center; the seat marker's position and that point */
// @retail 0xc7160
long __stdcall function_c7160(long unit_index, short seat_index, long vehicle_index, long a, long b)
{
	point3f *marker_position = (point3f *)a;
	point3f *position = (point3f *)b;
	long result = NONE;
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_seat_definition *seat = &UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(vehicle_index)))[seat_index];
	bool blocks = !((*(dword *)&seat->flags >> 5) & 1);
	long marker_name = *(long *)((byte *)seat + 8);
	real best_distance = 3.4028235e38f;
	s_object_marker marker;

	if (function_b8d30(vehicle_index, marker_name, &marker, 1, false))
	{
		if (marker_position)
		{
			*marker_position = marker.matrix.position;
		}
		for (long i = 0; i < 5; i++)
		{
			transform4x3f offset;

			if (function_10f9b0(unit_index, seat->label, g_467430[i], 1, &offset, blocks))
			{
				transform4x3f matrix;

				vector3f delta;

				function_142a60(&marker.matrix, &offset, &matrix);
				delta.i = matrix.position.x - unit->unknown030.x;
				delta.j = matrix.position.y - unit->unknown030.y;
				delta.k = matrix.position.z - unit->unknown030.z;
				real distance = (delta.k * delta.k + delta.i * delta.i) + delta.j * delta.j;

				if (best_distance > distance || result == NONE)
				{
					result = g_467430[i];
					best_distance = distance;
					if (position)
					{
						*position = matrix.position;
					}
				}
			}
		}
	}
	return result;
}

/* throws away all the unit's grenades: outside mode 4 each one becomes a
   grenade object that is dropped (0xce470) */
// @retail 0xceb30
void __stdcall function_ceb30(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	for (long type = 0; type < 2; type++)
	{
		byte *globals = (byte *)g_4e034c;
		byte *grenade = *(void **)(globals + 0x100) ? *(byte **)(globals + 0x104) + type * 0x2c : NULL;

		if (grenade && unit->grenade_counts[type] > 0)
		{
			do
			{
				if (g_4e6948->mode != 4)
				{
					byte data[0xc4];

					function_b7930(data, *(long *)(grenade + 0x20), unit_index, 0);
					long grenade_index = function_b7b40(data);
					if (grenade_index != NONE)
					{
						function_10ca80(grenade_index, unit_index);
						function_10cd50(grenade_index);
						long drop_mode = 0;
						byte *grenade_definition = UNIT_DEFINITION_GET(UNIT_GET(grenade_index));

						if (TEST_FIELD_BIT((UNIT_GET(unit_index)->flags_134 >> 16) & 1))
						{
							drop_mode = 1;
						}
						if (*(long *)(grenade_definition + 0x38) == NONE)
						{
							drop_mode = 1;
						}
						function_10cdf0(grenade_index);
						function_10ca80(grenade_index, NONE);
						if (drop_mode == 1)
						{
							function_b8540(grenade_index);
						}
						else
						{
							long name = 0;

							switch (drop_mode)
							{
							case 2:
								name = 0xa0005ad;
								break;
							case 3:
								name = 0x9000536;
								break;
							}
							function_ce6b0(unit_index, name, grenade_index, 1.0f);
						}
					}
				}
			} while (--unit->grenade_counts[type] > 0);
		}
	}
}

/* puts a weapon of a definition (and a value at +0x17e) in the unit's slot,
   dropping the one there; sets the unit's +0x210 and +0x214 and, when the
   unit had no weapon, raises it (request 8) */
// score 4 (was 124): retail sets 0xcea70's NONE mode (EAX) before the weapon (ECX), and 0xce520's
// weapon (EDX) before the unit (EAX). The count declared first keeps the unit index in EBX from entry,
// so LTCG passes it there as retail (with the NONE initialisers first: NONE in EBX, stack argument);
// the unit through unit_at_masked_index and the definition/value declared before the weapon index
// give the rest. Other orders 8-60; NONE in an else arm 117. MATCH with 0xcea70 declared (slot, weapon,
// mode, unit) AND 0xce520 (unit, weapon, flag) at every call (the 0xce520 order alone 2, the 0xcea70
// order alone 2); both held: the retail setup orders are reg-to-reg copies or differ per caller, so they
// are not retail evidence, and 0xcea70 is matched code in @Banshee64's range. The 0xce520 order also
// changes its call in 0x15e360 (src/unknown_157450.cpp), which stays matched either way.
// @retail 0xd09c0
void __stdcall function_d09c0(long unit_index, long slot_index, long definition_index, short value, dword const *state)
{
	long count = 0;
	s_unit *unit = unit_at_masked_index(unit_index);
	long current_definition_index = NONE;
	short current_value = NONE;
	long weapon_index = unit->weapon_object_indices[slot_index];

	if (weapon_index != NONE)
	{
		s_unit *weapon = UNIT_GET(weapon_index);

		current_definition_index = weapon->definition_index;
		current_value = *(short *)((byte *)weapon + 0x17e);
	}
	if (unit->weapon_object_indices[0] != NONE)
	{
		count = 1;
	}
	if (unit->weapon_object_indices[1] != NONE)
	{
		count++;
	}
	if (unit->weapon_object_indices[2] != NONE)
	{
		count++;
	}
	if (unit->weapon_object_indices[3] != NONE)
	{
		count++;
	}
	if (current_definition_index == definition_index && current_value == value)
	{
		return;
	}
	if (weapon_index != NONE)
	{
		function_ce520(weapon_index, unit_index, false);
		weapon_index = NONE;
	}
	if (definition_index != NONE)
	{
		byte data[0xc4];

		function_b7930(data, definition_index, unit_index, 0);
		weapon_index = function_b7b40(data);
		if (weapon_index != NONE)
		{
			if (value != NONE)
			{
				function_15e050(weapon_index, value);
			}
			function_cea70(NONE, (short)slot_index, weapon_index, unit_index);
			if (unit->unknown13c != NONE)
			{
				function_1520f0(unit->unknown13c);
			}
		}
	}
	s_unit *again = UNIT_GET(unit_index);
	*(dword *)&again->unknown214 = *state;
	again->unknown210 = again->unknown214;
	if (count == 0 && weapon_index != NONE && unit->unknown216 == slot_index)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 8;
		request.type17.unknown4 = true;
		request.type17.unknown5 = true;
		function_e6900(unit_index, &request);
	}
}

/* damages the unit with a damage definition from its center against its
   velocity +0x168 (in mode 4, through its player's controller) */
// @retail 0xd03e0
void function_d03e0(long unit_index, long definition_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (g_4e6948->mode == 4)
	{
		if (unit_get_player_index(unit_index) != NONE)
		{
			long player_index = unit_get_player_index(unit_index);
			long controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

			if (controller != NONE)
			{
				/* retail also writes a zero short after the team */
				struct { long player_index; long object_index; short team; short padding; } owner = { NONE, NONE, NONE, 0 };
				vector3f direction;

				/* retail zeroes .j and .k before filling the vector */
				memset(&direction.j, 0, 2 * sizeof(real));

				direction.i = 0.0f - unit->unknown168.i;
				direction.j = 0.0f - unit->unknown168.j;
				direction.k = 0.0f - unit->unknown168.k;
				function_153d10(controller, definition_index, &owner, &direction, 0, 1.0f, 1.0f, 0);
			}
		}
	}
	else
	{
		s_type_1e6529 damage;

		damage.material_index = NONE;
		function_d6660(&damage, definition_index);
		damage.flags |= 8;
		damage.direction.i = 0.0f - unit->unknown168.i;
		damage.direction.j = 0.0f - unit->unknown168.j;
		damage.direction.k = 0.0f - unit->unknown168.k;
		damage.origin = unit->unknown030;
		damage.position = unit->unknown030;
		function_d7b80(&damage, unit_index, NONE, NONE, NONE, 0);
	}
}

/* what a unit's melee hit: an object, or a surface */
#include "unit_hit.h"

/* applies the unit's melee damage to what it hit (a device of type 7 with
   bit 5 at +0x1cc is triggered first), then ends its state +0x2bc */
// @retail 0xcfc90
void __stdcall function_cfc90(long unit_index, long definition_index, s_unit_melee_hit const *hit)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_type_1e6529 damage;
	s_object_marker marker;

	damage.material_index = NONE;
	function_d6660(&damage, definition_index);
	damage.unknown84 = hit->unknown18;
	damage.unknown18 = unit_index;
	damage.flags |= 1;
	damage.unknown1c = unit->unknown028;
	*(long *)&damage.unknown20 = unit->unknown02c;
	object_get_damage_owner(unit_index, &damage.owner);
	function_b8d30(unit_index, 0x4000095, &marker, 1, false);
	damage.position = marker.matrix.position;
	damage.origin = unit->unknown030;
	damage.direction = unit->unknown168;
	damage.node_direction = unit->unknown168;
	damage.material_index = hit->material_index;
	damage.unknown54 = hit->unknown14;
	if (hit->object_index != NONE)
	{
		if (UNIT_GET(hit->object_index)->type == 7)
		{
			long device_index = hit->object_index;

			if (*((byte *)UNIT_GET(device_index) + 0x1cc) & 0x20)
			{
				function_107370(device_index, 1.0f);
			}
		}
		function_d7b80(&damage, hit->object_index, NONE, NONE, NONE, 0);
	}
	else if (hit->unknown04 != NONE)
	{
		function_184060(hit->unknown08, hit->unknown04, &damage, hit->unknown0c);
	}
	function_c5740(unit_index, false);
}

/* v multiplied by the scale, component by component */
static __forceinline void vector_scale_in_place(vector3f *v, real scale)
{
	v->i *= scale;
	v->j *= scale;
	v->k *= scale;
}

/* scales v to unit length and returns its old length, or 0 for a vector
   too short to normalize (items.cpp expands the same normalization) */
static inline real vector_normalize_in_place(vector3f *v)
{
	real length = (real)sqrt(length_sq3f(v));

	if (!(fabs(length) < 0.0001f))
	{
		vector_scale_in_place(v, 1.0f / length);
	}
	else
	{
		length = 0.0f;
	}
	return length;
}

/* whether a unit may enter the vehicle's seat from a marker: near enough
   (+0x98), facing it (+0xa0), the marker facing the unit (+0x9c), and
   moving with the vehicle (+0xa4) */
// @retail 0xc9040
bool function_c9040(long vehicle_index, long unit_index, short seat_index, point3f const *position,
	vector3f const *forward)
{
	bool result = false;
	s_unit *unit = UNIT_GET(unit_index);
	byte *seat = (byte *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(vehicle_index)))[seat_index];
	point3f center;
	vector3f direction;
	real distance;

	function_cafc0(unit_index, &center);
	vector3d_from_points3d(&center, position, &direction);
	distance = vector_normalize_in_place(&direction);
	if (*(real *)(seat + 0x98) > distance &&
		*(real *)(seat + 0xa0) >= function_11cf50(&unit->unknown168, &direction) &&
		*(real *)(seat + 0x9c) >= 3.1415927f - function_11cf50(forward, &direction))
	{
		vector3f unit_velocity;
		vector3f vehicle_velocity;

		function_ba1d0(unit_index, &unit_velocity, 0);
		function_ba1d0(vehicle_index, &vehicle_velocity, 0);
		real dx = unit_velocity.i - vehicle_velocity.i;
		real dy = unit_velocity.j - vehicle_velocity.j;
		real dz = unit_velocity.k - vehicle_velocity.k;
		if (*(real *)(seat + 0xa4) >= (real)sqrt(dz * dz + dy * dy + dx * dx))
		{
			result = true;
		}
	}
	return result;
}

/* pushes the unit off its parent (when it is not seated; seated units
   get request 0x1e): detaches it with a push away from the parent's
   center, and wakes it */
// @retail 0xcc590
void function_cc590(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->parent_index == NONE)
	{
		return;
	}
	if (unit->parent_seat_index != NONE)
	{
		function_e68c0(0x1e, unit_index);
		return;
	}
	point3f parent_center;
	point3f center;
	vector3f direction;

	function_b9dd0(unit->parent_index, &parent_center);
	function_b9dd0(unit_index, &center);
	direction.i = center.x - parent_center.x;
	direction.j = center.y - parent_center.y;
	direction.k = center.z - parent_center.z;
	if (function_30bf0(&direction) == 0.0f)
	{
		direction = unit->forward;
	}
	direction.i *= 0.6f;
	direction.j *= 0.6f;
	direction.k *= 0.6f;
	function_b9a90(unit_index);
	{
		point3f position = unit->position;

		function_11bf90(unit_index, &position);
		vector3f velocity;
		velocity.i = unit->linear_velocity.i + direction.i;
		velocity.j = unit->linear_velocity.j + direction.j;
		velocity.k = unit->linear_velocity.k + direction.k;
		function_b75a0(unit_index, &position, 0, 0, 0, false);
		function_b77d0(unit_index, &velocity, g_4687a4);
	}
	s_unit *again = UNIT_GET(unit_index);
	if (TEST_FIELD_BIT(again->object_flags & 1))
	{
		again->object_flags &= ~1;
		if (function_b9d20(unit_index))
		{
			function_bef30(unit_index, 0, 1, 0, false);
		}
		function_b8b70(unit_index);
	}
}

/* where a unit aims from, moved by the offset between a reference point
   and the unit's own point (its vehicle's, else its center): a free biped
   leaves it to 0xdef60; otherwise mode 3 uses its weapon's barrel, modes
   1-3 its marker 0x4000095, and other modes its center */
// @retail 0xcb500
void __stdcall function_cb500(long unit_index, short mode, point3f const *origin, vector3f const *forward,
	real const *offsets, point3f *point)
{
	s_unit *unit = UNIT_GET(unit_index);
	long parent_index = unit->parent_index;
	point3f reference;

	if (parent_index == NONE && !(TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2)))
	{
		if (unit->type == 0)
		{
			function_def60(point, unit_index, mode, origin, forward, offsets);
			return;
		}
	}
	else if (unit->type == 0 && parent_index != NONE && UNIT_GET(parent_index)->type == 1)
	{
		long location;

		function_f1070(parent_index, &location, 0, &reference, 0, 0);
		if (location != NONE)
		{
			goto have_reference;
		}
	}
	function_b9dd0(unit_index, &reference);
have_reference:
	switch (mode)
	{
	case 3:
	{
		s_unit *holder = UNIT_GET(unit_index);
		short index = holder->current_weapon_index;

		if (index != NONE && holder->weapon_object_indices[index] != NONE &&
			function_101b80(holder->weapon_object_indices[index], 0, point))
		{
			break;
		}
	}
	// fall through
	case 1:
	case 2:
	{
		s_object_marker marker;

		function_b8d30(unit_index, 0x4000095, &marker, 1, false);
		*point = marker.matrix.position;
		break;
	}
	default:
		function_cafc0(unit_index, point);
		break;
	}
	vector3f delta;

	delta.i = origin->x - reference.x;
	delta.j = origin->y - reference.y;
	delta.k = origin->z - reference.z;
	point->x += delta.i;
	point->y += delta.j;
	point->z += delta.k;
}

/* fades the unit's two definition-timed values (+0x25c, the definition's
   +0x1b8 entries) toward 1 while their condition holds, else toward 0:
   the first while +0x248 is set (or flag 1), the second while +0x24c is
   set apart from it; whether any moved */
// MATCH (was 110; round 1-2 of #1049 129 -> 110). Round 3 of #1049: the draft set `active = false` in a
// `default:` arm; retail clears it (`xor al, al`) BEFORE the switch, so `bool active = false;` is declared
// ahead of the switch and the switch has no default. That freed the registers: the -1 constant is no longer
// kept in EBX (retail compares the loop's NONE tests with an immediate) and the time globals pointer lands in
// EBX. The whole body inside `if (!flag)` with one `return result` gives retail's shared early exit, flag 2 of
// +0x10a through the word bitfield view and the flag clear through bit_vector_set give retail's loads and
// in-place AND. No effect (before the fix): parent index in a local, parent and seat tests nested, seat in
// a local (110 each); the time globals pointer cached in a local (108).
// @retail 0xc5eb0
bool function_c5eb0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	bool result = false;

	if (!TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 11) & 1))
	{
		function_d0c10(unit_index);
		function_ce0c0(unit_index);
		if (TEST_FIELD_BIT((unit->flags_134 >> 26) & 1) && g_4e6948->mode != 4 && unit->parent_index != NONE &&
			unit->parent_seat_index != NONE)
		{
			bit_vector_set(&unit->flags_134, 26, false);
			function_e68c0(0x1e, unit_index);
		}
		for (long i = 0; i < *(long *)(definition + 0x1b8); i++)
		{
			real const *time = (real const *)(*(byte **)(definition + 0x1bc) + i * 8);
			bool active = false;

			switch (i)
			{
			case 0:
				active = unit->unknown248 != NONE || TEST_FIELD_BIT((unit->flags_134 >> 1) & 1);
				break;
			case 1:
				active = unit->unknown24c != NONE && unit->unknown24c != unit->unknown248;
				break;
			}
			if (!(TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2)) && active)
			{
				if (unit->unknown25c[i] != 1.0f)
				{
					if (*time > 0.0f)
					{
						unit->unknown25c[i] += 1.0f / (g_510c54->field_2_3 * *time);
					}
					else
					{
						unit->unknown25c[i] = 1.0f;
					}
					if (unit->unknown25c[i] > 1.0f)
					{
						unit->unknown25c[i] = 1.0f;
					}
					result = true;
				}
			}
			else
			{
				if (unit->unknown25c[i] != 0.0f)
				{
					if (*time > 0.0f)
					{
						unit->unknown25c[i] -= 1.0f / (g_510c54->field_2_3 * *time);
					}
					else
					{
						unit->unknown25c[i] = 0.0f;
					}
					if (0.0f > unit->unknown25c[i])
					{
						unit->unknown25c[i] = 0.0f;
					}
					result = true;
				}
			}
		}
	}
	return result;
}

/* turns a vector into the frame of a player's seat (seats without bit 4):
   built from the vehicle's up and the world's axes. The forward axis's j and
   k are written through axis: written as matrix fields, the transform at the
   end loads the input's k before j and multiplies its forward terms the other
   way round */
// @retail 0xcb810
bool function_cb810(long unit_index, vector3f *vector)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

	if (unit->unknown13c != NONE)
	{
		short seat_index = unit->parent_seat_index;

		if (seat_index != NONE)
		{
			long parent_index = unit->parent_index;

			if (!TEST_FIELD_BIT((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(parent_index)))[seat_index].flags >> 4) & 1))
			{
				vector3f forward;
				vector3f up;
				transform4x3f matrix;
				vector3f *axis = &matrix.forward;

				function_b9fc0(parent_index, &forward, &up);
				matrix.forward.i = g_4687b8->k * up.j - g_4687b8->j * up.k;
				axis->j = g_4687b8->i * up.k - g_4687b8->k * up.i;
				axis->k = g_4687b8->j * up.i - g_4687b8->i * up.j;
				if (function_30bf0(axis) == 0.0f)
				{
					matrix.forward.i = g_4687bc->k * up.j - g_4687bc->j * up.k;
					axis->j = g_4687bc->i * up.k - g_4687bc->k * up.i;
					axis->k = g_4687bc->j * up.i - g_4687bc->i * up.j;
					function_30bf0(axis);
				}
				matrix4x3_from_forward_and_up(&matrix, axis, &up);
				real i = vector->i;
				real j = vector->j;
				real k = vector->k;
				vector->i = matrix.up.i * k + matrix.left.i * j + i * matrix.forward.i;
				vector->j = matrix.up.j * k + matrix.left.j * j + matrix.forward.j * i;
				vector->k = matrix.up.k * k + matrix.left.k * j + matrix.forward.k * i;
				result = true;
			}
		}
	}
	return result;
}

/* how the unit reacts to damage it took: sets the report's reaction
   (+0x50) from 0xc9770, after the biped's knockdown checks */
// MATCH (was 109): memory fields read as field bits (TEST_FIELD_BIT) give retail's load-and-shift, the locals (type
// bits, flags) are plain mask tests, and the +0x1f0 bit is read through a fresh UNIT_DEFINITION_GET(unit) (retail does
// after 0xe4050, which keeps unit in ebp). Round 2 of #1049: the statements were already retail's; two spellings fixed
// the allocation: the 0x3dc byte is read through UNIT_GET(unit_index) (not through the reassigned `header`), which puts
// the type-bit pointer sum in esi like retail (109 -> 3), and ignore/held/force are declared in that order before the
// +0x50 store (retail zeroes them as ignore, held, force). Tried before: held/force before the store, ignore after (128).
// @retail 0xc9e70
void function_c9e70(long unit_index, dword flags, s_type_1e6529 const *data, s_damage_report const *report)
{
	byte *bytes = (byte *)report;
	s_unit *unit = UNIT_GET(unit_index);
	byte *header = (byte *)g_4e0300->data + (unit_index & 0xffff) * 0xc;
	byte *local_98b918_2 = UNIT_DEFINITION_GET(unit);
	bool is_biped = (1 << header[3]) & 1;
	byte *damage = g_4e3b44[*(long *)(bytes + 8) & 0xffff].bytes + 0x10;
	bool hard = bytes[4] & 1;
	bool knocked_down = false;
	bool strong = hard && *(real *)(damage + 0x30) >= 10.0f;

	if (hard)
	{
		knocked_down = function_10f340(unit_index, 0x7000101, 0xd000042) && is_biped && function_e4050(unit_index);
	}
	if (*(short *)damage == 3 && is_biped && TEST_FIELD_BIT((*(dword *)(UNIT_DEFINITION_GET(unit) + 0x1f0) >> 10) & 1))
	{
		function_e3f00(unit_index);
	}
	bool ignore = false;
	bool held = false;
	bool force = false;
	*(long *)(bytes + 0x50) = 0;
	if ((data->flags & 0x10) || TEST_FIELD_BIT((damage[4] >> 4) & 1) || TEST_FIELD_BIT((unit->flags_134 >> 19) & 1))
	{
		ignore = true;
	}
	header = (byte *)g_4e0300->data + (unit_index & 0xffff) * 0xc;
	dword type_bit = 1 << header[3];
	if (((type_bit & 1) && *((byte *)UNIT_GET(unit_index) + 0x3dc) == 2) || (type_bit & 2))
	{
		held = true;
	}
	else if ((flags & 2) || (flags & 0x100))
	{
		force = true;
	}
	else
	{
		if (unit->unknown13c != NONE)
		{
			held = true;
		}
		if (TEST_FIELD_BIT((*(dword *)(local_98b918_2 + 0xbc) >> 7) & 1) && !TEST_FIELD_BIT((damage[4] >> 2) & 1))
		{
			held = true;
		}
		if (TEST_FIELD_BIT((unit->flags_134 >> 5) & 1))
		{
			held = true;
		}
		if (unit->unknown1f4 > 0)
		{
			held = true;
		}
	}
	if (*(dword *)(damage + 4) & 0x4000)
	{
		force = true;
	}
	if (!ignore)
	{
		*(long *)(bytes + 0x50) = function_c9770(unit_index, hard, strong, knocked_down, held, force,
			unit->unknownf4 + *(real *)(bytes + 0x48), unit->unknownf8 + *(real *)(bytes + 0x44));
	}
}

// MiniMax G (11-13): no lower score (139): the bit-field spelling of the 0x90005a1 pair reaches retail's shift-and-test
// form but scores only 137, under the keep threshold, so the literal mask stays; the constant 1 that retail holds in bl
// for the whole body still needs a local that holds nothing else.
// MiniMax H (11-13): score 139 (was 163): the three binary cases now fall out of the switch instead of jumping over
// the clamp, so the 0.0 and 1.0 results reach one shared store tail as retail does, and the body is 574 bytes against
// retail's 578. Left: retail holds the constant 1 in bl for the whole body (its `test bl, al` bit tests and its
// `mov al, bl` returns) while this build uses immediates and no ebx, and the 0x90005a1 bit test is a direct mask
// test where retail shifts the byte first.
// MiniMax A (11-12): no new form, no lower score (163): retail still holds the constant 1 in a register for the whole body (saved at entry, used by the bit tests and returned with mov al, bl) and sends every case to one shared store tail, while this build returns an immediate and clones the store into the 0.0 case (602 bytes against retail 578); a local that only holds that constant is a barred form, so the shared tail is not reachable from here and the body stands.
/* the unit's exported function values by name (0 to 1), and whether each
   is active; two names come from its weapon (0xff5f0) */
// MiniMax H (11-12): no new form, no lower score (163); retail holds the constant 1 in a register for the whole body
// (saved at entry, used by the bit tests and returned with `mov al, bl`) and sends every case to one shared store
// tail, while this build returns an immediate and clones the store into the 0.0 case; a local holding only that
// constant is a barred form, so the shared tail is not reachable from here and the body stands.
// MiniMax G (11-11): no new form, no lower score (163); retail holds the constant 1 in a register for the whole
// body (saved at entry, used by the bit tests and returned with `mov al, bl`) and sends every case to one shared
// store tail, while this build returns an immediate and clones the store into the 0.0 case (602 bytes against
// retail's 578); a local holding only that constant is a banned form, so the shared tail is not reachable from
// here and the body stands.
// score 139 (was 163; earlier 168): every case, constant, clamp, store and weapon call matches retail's statements; what differs is
// Earlier at 163 (was 163; earlier 168): every case, constant, clamp, store and weapon call matches retail's statements; what differs is
// layout. Retail keeps the constant 1 in bl for the whole function (saved at entry, used by the `test bl, al` bit tests
// and returned with `mov al, bl`) and sends every case to one shared store tail; ours still returns an immediate and
// holds no ebx (574 bytes, retail 578). Tried: bit tests as field bits and flags_10a through its byte
// and returned with `mov al, bl`) and sends every case to one shared store tail; ours returns an immediate and clones
// the store into the 0.0 case (608 bytes, retail 578). Tried: bit tests as field bits and flags_10a through its byte
// view 173/180 (retail's shr/test forms, but larger); a function-level `found` bool set true and returned 168 (folded
// to an immediate).
// Round 4 of #1049 (logic walk: no behaviour difference; every case's field, constant, clamp, active test and the
// weapon names/index test agree with retail): the weapon default as one `if (name == a || name == b)` around the lookup with a
// single `return found` (retail has one shared pop esi/ret; ours had two copies; 168 -> 163). Not kept: `result = 1.0f;`
// before the 5a1 test instead of its else arm 159 (a value set before a test, without a match). Tried and dropped: tail as `if (result > 0.0f) { *active = true;
// return true; } *active = false; return true;` 156 (retail's two return copies; kept plain, no match); field-bit
// tests for the 5a1 pair 162, plus the 5a2 test 164 (`shr al, 2` forms but still no bl: 605-608 bytes); the 5a1 else
// arm with field bits 172; a `bool ok = true` returned from both tails and the default path 164 (folded, no bl);
// cases reordered to retail's address order (no change: the switch is laid out in value order); the two byte scales
// (0xd00059d and 0x1800059e) through one `goto scale_byte` label 163 (retail merges their mulss tail, ours does not).
// Round 144 (m3 lane 7): the 0x90005a1 pair as one ternary 163; the clamp as a PIN-style ternary 163 (both spellings,
// same code); both 163; field-bit tests for the 0x90005a1 pair and the 0x140005a2 bit with a function-scope `found =
// true` returned at the store tail 170 (613 bytes, no BL constant). Retail's shared-bound `movaps xmm0, xmm1` clamp
// and its BL constant 1 (`test bl, al`, `mov al, bl`) did not appear in any form.
// Round 234 (m3 lane 3): retail's weapon query is function_cbd50 inlined (`movsx cx, byte [unit+0x212]; cmp cx, -1`);
// calling it as the sibling 0xc8960 does 169 (it stays a call here). A function-level `bool found = true` that the
// default case assigns before jumping to the one final `return found` after the store tail 170 (620 bytes): retail's
// `push ebx; mov bl, 1` and `mov al, bl` returns appear (masked 52 -> 48), but the store tail is still copied into the
// 0.0 path; with field-bit tests for the 0x90005a1 pair 172 (masked 45), and for the 0x140005a2 bit too 177 (masked
// 43); the 0x90005a1 pair as a ternary on those 170 and 177 (same code). The weapon name as a ternary with the index
// initialised at its declaration 174 (619 bytes); that on the single-exit form 191 (640 bytes).
// P434: complete bounded carrier 163; direct delegated returns 154 (597 bytes; NOT kept:
// tail jump replaces retail call/return and removes its saved weapon-name register); complete
// delegated name admission 168; bound defaults 154 (599 bytes; NOT kept: one-use bound copy
// adds a nonnative transfer); branch-owned found 159 (NOT kept: gain four below eight); index
// lifetime, uniformly const unit and property-value name 163/163/163. Original body restored.
// MiniMax B (11-13): no new form, no lower score (163): our build returns an immediate and clones the store into the 0.0 case (608 bytes against retail's 578), where retail keeps the constant 1 in bl and sends every case to one shared store tail.
// P456: on the shared-clamp 574-byte graph, sole early/late found result 132/142; shared state with
// original returns 139; existing field-bit forms 136; successive/upper-first clamps 139/143; identity-
// plus-alias delegated name 142; delegated result from tests 143. Sole early found recovers BL but is
// 582/578 bytes, gain seven: NOT kept below eight. Field-bit gain three also rejected. Original
// executable body restored; native truth/shift-test and saved-name carriers remain.
// MiniMax A (11-14): no new form, no lower score (139): retail still carries the constant 1 in bl
// (the mask of both bit tests and the returned value) where this build folds the tests into memory
// masks and returns 1 in eax; a 300 s permuter run over the body moved the reduced image's count by
// one (136 to 135) with local constants and a swapped call argument, so it is not kept.
// @retail 0xc6b90
bool __stdcall function_c6b90(long unit_index, long name, real *value, bool *active)
{
	s_unit *unit = UNIT_GET(unit_index);
	real result;

	switch (name)
	{
	case 0x90005a1:
		if ((unit->flags_10a >> 2) & 1 || (unit->flags_134 >> 18) & 1)
		{
			result = 0.0f;
		}
		else
		{
			result = 1.0f;
		}
		break;
	case 0x6000087:
		result = unit->unknown2c4;
		break;
	case 0x5000581:
		result = function_d1210(unit_index);
		break;
	case 0x90006aa:
		result = function_d1410(unit_index);
		break;
	case 0xb0006ab:
		result = 1.0f - function_d1410(unit_index);
		break;
	case 0xd00059d:
		result = (byte)unit->unknown243 * (1.0f / 255.0f);
		break;
	case 0xe00059f:
		result = unit->unknown208;
		break;
	case 0xf0006bf:
		result = unit->unknown1b0.k;
		break;
	case 0x11000088:
		result = unit->unknown2b0;
		break;
	case 0x1100059b:
		result = unit->unknown25c[0];
		break;
	case 0x1100059c:
		result = unit->unknown25c[1];
		break;
	case 0x140005a2:
		result = unit->unknown248 != NONE || (unit->flags_134 >> 1) & 1 ? 1.0f : 0.0f;
		break;
	case 0x140005a3:
		result = unit->unknown24c != NONE ? 1.0f : 0.0f;
		break;
	case 0x160005a0:
		result = unit->unknown264;
		break;
	case 0x1800059e:
		result = unit->unknown1f8 * (1.0f / 255.0f);
		break;
	default:
	{
		bool found = false;

		if (name == 0xb0005a9 || name == 0x130005a8)
		{
			long weapon_name;
			short index;

			if (name == 0xb0005a9)
			{
				weapon_name = 0xb0005a9;
			}
			else
			{
				weapon_name = 0xc000569;
			}
			index = unit->current_weapon_index;
			if (index != NONE && unit->weapon_object_indices[index] != NONE)
			{
				found = function_ff5f0(unit->weapon_object_indices[index], weapon_name, value, active);
			}
		}
		return found;
	}
	}
	if (0.0f > result)
	{
		result = 0.0f;
	}
	else if (result > 1.0f)
	{
		result = 1.0f;
	}
store:
	*value = result;
	*active = result > 0.0f;
	return true;
}

/* drops an object the unit held: detaches it (at a marker, 0x4000023 when
   the unit has it, else 0xa000798, unless named), throws it at a random
   speed in a cone around the unit's aim on top of the unit's velocity
   (capped at 4 unless the unit's +0xc0 bit 1 is set), and deletes it in
   state 1 when it has nowhere to go and is not a predicted copy (0xa7670) */
// MiniMax A (11-14): no new form, no lower score (4): only the four instructions that set up the random_vector_in_cone
// call are in the other order - retail spills the sampled speed first and then forms the forward, result and seed
// pointers, this build forms the pointers first - and the callee's parameter list is in src/unknown_146240.cpp:
// BLOCKED-BY 0x146320.
// MiniMax H (11-14): no new form, no lower score (4): the cone call's four setup instructions are still in the other order, and that order is the cone helper's own convention in a file this lane may not edit; the eight source shapes tried before all give these four bytes.
// MiniMax B (11-13): no new form, no lower score (4): the cone call's four setup instructions are still in the other order: retail spills the sampled speed first, then the forward pointer, then the result pointer and the seed, while this build forms the pointers first.
// MiniMax G (11-13): no new form tried this round, no lower score (4): the frame is retail's and the four
// instructions that set up the cone call keep their order - the sampled speed's spill, the forward pointer, the output
// pointer and the seed copy - while retail spills the speed first and copies the seed last; the callee 0x146320 and
// its parameter list live in src/unknown_146240.cpp, which this lane may not edit, so the setup stays as it is.
// MiniMax H (11-12): no new form, no lower score (4): the frame is retail's and every instruction agrees except the four that set up the cone call; that callee 0x146320 lives in src/unknown_146240.cpp, which this lane may not edit.
// MiniMax S (11-11): no new form, no lower score (4): the frame is retail's and every instruction agrees except the
// four that set up the cone call - retail spills the sampled speed first and then forms the forward pointer, the
// output pointer and the seed copy, ours forms the forward pointer before the spill and the seed before the output
// pointer; eight source shapes of that setup (pointer locals, a float local, speed first, the seed as a global or a
// pointer) all leave the same four instructions in a different order, and the callee's parameter list lives in
// src/unknown_146240.cpp:
// BLOCKED-BY 0x146320.
// MiniMax I (11-11): no new form, no lower score (4); the cone call's argument setup is scheduled
// differently - retail spills the sampled speed first and then forms the forward pointer, the output
// pointer and the seed copy, ours forms the forward pointer before the spill and the seed before the
// output pointer, and the named forms of that setup all compile to the same four instructions.
// MiniMax G (11-12): no new form tried this round, no lower score (4): the cone call's argument setup still has
// the sampled-speed spill and the seed copy in the other order, and the callee itself lives in another lane's file.
// MiniMax H (11-13): no new form, no lower score (4); the cone call's four setup instructions are in the other order: retail spills the sampled speed first, then the forward pointer, then the result pointer and the seed, while this build forms the pointers first.
// score 4 (was 4): cone setup differs: retail spills sampled speed before forming the forward
// pointer, then forms the output pointer before copying the seed. Ours swaps both pairs.
// The 0x98 frame and every other instruction agree.
// existing length/dot helpers, direction lifetime/name and named angle 13;
// existing random-real helper 111; scale/add component loops 80/13; seed global at each
// use, compound adds, direction-first adds, speed-first scaling 13 each; speed scaled
// before the cone call 120; every squared-length operand order compiles the same.
// cone result pointer local, separate thrown-speed local, two-use scale snapshot, seed renamed,
// velocity at function scope and forward pointer local 4 each; existing scale helper 80;
// inline-product scale helper 93; direction pointer used throughout 48.
// Round 204: existing random-range call 38; raw generator, force-inline step declaration,
// named normalized fraction, shared velocity addition and first/second/both library copies 4 each.
// Frame and all other instructions agree; the cone argument setup remains.
// MiniMax R (10-31): no new form, no lower score.
// Round 1 of #1049, all 4: scale * 0.3926991f in a named max_angle local (before or after the seed local);
// speed and direction declared up front with a separate assignment; forward and result pointers as named locals;
// `speed = speed * scale`. Retail's order (fstp, forward, result, seed) is the source order of the call's
// arguments after the spill; ours emits the forward pointer before the spill and the result after the seed.
// Permuter (25 min on #1049, about 450 variants): nothing below 4.
// MiniMax R (11-11): no new form tried this round, no lower score (4): the body is retail's length and the frame agrees; the four differences stay the two swapped pairs, and the named forms of that setup compile to the same four instructions.
// MiniMax A (11-13): no new form tried this round, no lower score (4): only the four instructions that set up
// the random_vector_in_cone call are in the other order - retail spills the sampled speed first and then forms
// the forward, result and seed pointers, this build forms the pointers first - and the callee's parameter list
// is in src/unknown_146240.cpp: BLOCKED-BY 0x146320.
// @retail 0xce6b0
void function_ce6b0(long unit_index, long name, long object_index, real scale)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_unit *object = UNIT_GET(object_index);

	if (name == NONE || name == 0)
	{
		s_object_marker marker;

		name = function_b8d30(unit_index, 0x4000023, &marker, 1, false) ? 0x4000023 : 0xa000798;
	}
	function_b8ee0(unit_index, name, object_index, 0);
	function_b9a90(object_index);
	function_10b360(object_index);
	dword *seed = &g_4e7408->unknown0;
	real speed = 0.8f + (1.2f - 0.8f) * ((real)random_next(seed) * (1.0f / 65535.0f));
	object->linear_velocity = *g_4687a4;
	*(vector3f *)((byte *)object + 0x94) = *g_4687a4;
	vector3f direction;
	random_vector_in_cone(&unit->unknown168, &direction, seed, 0.0f, scale * 0.3926991f);
	speed *= scale;
	direction.i *= speed;
	direction.j *= speed;
	direction.k *= speed;
	{
		vector3f velocity;

		function_ba1d0(unit_index, &velocity, 0);
		direction.i = velocity.i + direction.i;
		direction.j = velocity.j + direction.j;
		direction.k = velocity.k + direction.k;
	}
	if (!TEST_FIELD_BIT((unit->unknownc0 >> 1) & 1))
	{
		real length_squared = direction.j * direction.j;
		length_squared += direction.k * direction.k;
		length_squared += direction.i * direction.i;

		if (length_squared > 16.0f)
		{
			real factor = 4.0f / (real)sqrt(length_squared);

			direction.i *= factor;
			direction.j *= factor;
			direction.k *= factor;
		}
	}
	object->unknown14c = unit_index;
	function_10cf80(&direction, object_index, false);
	{
		point3f point;

		if (!function_bc1d0(object_index, function_b9ef0(unit_index, &point), unit_index))
		{
			if (!function_a7670(object_index) && g_4e6948->state == 1)
			{
				function_b8540(object_index);
			}
		}
	}
}

/* the player's unit's toggle on control bit 2 (+0x148): in campaign it
   flips flag 29 with its sounds and fades +0x264 in (a fifth of a second)
   or out (0.8 s) by it; in state 1 or with the campaign flag it runs the
   state +0x2bc and its timer instead */
// @retail 0xc50a0
void function_c50a0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	long player_index = unit->unknown13c;

	if (player_index == NONE || g_4e6948->state != 1)
	{
		return;
	}
	if (*((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x88) == 1 || g_4f55dc[0])
	{
		if ((byte)unit->unknown148 & 4)
		{
			if (unit->unknown2bc != 1)
			{
				function_c5740(unit_index, true);
			}
			unit->flags_134 ^= 0x8000;
		}
		short timer = unit->unknown2be;
		if (timer > 0)
		{
			unit->unknown2be = --timer;
			if (timer == 0)
			{
				switch (unit->unknown2bc)
				{
				case 1:
					function_c5740(unit_index, false);
					break;
				case 2:
					function_c5890(unit_index, 3);
					unit->unknown2bc = 0;
					break;
				}
			}
		}
		return;
	}
	if ((byte)unit->unknown148 & 4)
	{
		if (TEST_FIELD_BIT((unit->flags_134 >> 29) & 1))
		{
			function_c5890(unit_index, 4);
			bit_vector_set(&unit->flags_134, 29, false);
		}
		else
		{
			function_c5890(unit_index, 5);
			bit_vector_set(&unit->flags_134, 29, true);
			unit->unknown268 = 4.0f;
		}
	}
	if (TEST_FIELD_BIT((unit->flags_134 >> 29) & 1))
	{
		if (unit->unknown268 > 0.0f)
		{
			unit->unknown268 -= g_510c54->rate;
			if (0.0f >= unit->unknown268)
			{
				unit->unknown268 = 0.0f;
			}
		}
		if (unit->parent_index != NONE || TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2) ||
			(!g_5107e8->flag0 && unit->unknown268 == 0.0f))
		{
			bit_vector_set(&unit->flags_134, 29, false);
		}
		if (unit->unknown264 != 1.0f)
		{
			unit->unknown264 += 1.0f / (real)unit_round(g_510c54->field_2_3 * 0.2f);
			if (unit->unknown264 > 1.0f)
			{
				unit->unknown264 = 1.0f;
			}
		}
	}
	else if (unit->unknown264 != 0.0f)
	{
		unit->unknown264 -= 1.0f / (real)unit_round(g_510c54->field_2_3 * 0.8f);
		if (0.0f > unit->unknown264)
		{
			unit->unknown264 = 0.0f;
		}
	}
}

// MiniMax G (11-13): no new form, no lower score (166): the diff re-read this round confirms the first difference is still the register plan around the object pool pointer, which none of the source spellings listed above moves, so the body stands.
// MiniMax A (11-12): no new form, no lower score (166): the register plan still differs - retail keeps the vehicle pointer in esi, the -1 and the child index in ebp and the object pool pointer in edx, ours keeps the pool in esi and the vehicle in ebp - and the load of the old driver index sits before the definition lookup in ours and after it in retail; none of the source spellings tried earlier moves that plan, so the body stands.
/* finds the vehicle's driver (+0x248) and gunner (+0x24c) among its
   riders, in two passes (plain seats, then seat groups whose rider is in
   place), recursing into ridden units; tells 0x1bbdf0 when the driver
   changed */
// MiniMax B (11-13): no new form, no lower score (166): re-measured with the plain build (665 bytes against retail 661, first difference at +0x7); what is left is the register allocation of the vehicle and the object pool pointer, which the recursive call register plan decides.
// MiniMax H (11-12): no new form, no lower score (166); the register plan still differs - retail keeps the vehicle
// pointer in ESI, the -1 and the child index in EBP and the object pool pointer in EDX (reloaded after the recursive
// call), ours keeps the pool in ESI and the vehicle in EBP - and the load of the old driver index sits before the
// definition lookup in ours and after it in retail; the sibling read from the held child pointer and the child as a
// block local, both recorded below, move that plan no closer.
// MiniMax G (11-11): no new form, no lower score (166); what still differs is the register plan - retail keeps the
// vehicle pointer in ESI, the -1 and the child index in EBP and the object pool pointer in EDX (reloaded after the
// recursive call), ours keeps the pool in ESI and the vehicle in EBP - and the load of the old driver index sits
// before the definition lookup in ours and after it in retail; none of the source spellings tried earlier moves
// that plan, so the body stands.
// MiniMax S (11-11): no lower score (166): the vehicle's definition index in a named local with the definition
// read through it gives the same code (166, 665 bytes); retail loads the object pool into EDX before the
// callee-saved pushes and keeps the vehicle in ESI where ours loads the pool into ESI after the push and keeps
// the vehicle in EBP, and that register plan does not follow from the source spelling.
// MiniMax I (11-11): no new form, no lower score (166); retail keeps the vehicle pointer in ESI and the
// NONE constant in EBP where ours keeps the vehicle in EBP, and the load of the old driver index sits
// before the definition lookup in ours and after it in retail.
// MiniMax E (11-11): no new form tried, no lower score (166): what still differs is the register
// allocation - retail keeps the -1 and the child index in EBP, the vehicle in ESI and the object pool
// pointer in EDX, ours keeps the pool in ESI and the vehicle in EBP - and the load of the old driver
// index sits before the definition lookup in ours and after it in retail; the forms listed below (the
// bool local for bit 13, the next sibling read from the held child pointer, a `none` local, the short
// pass, the driver preset false) all score the same or worse, so the body stands.
// score 166 (was 166; earlier 188): block layout now retail's (the rider that has a seat is the fall-through side, as an
// `if (seat_index != NONE)` with the no-seat gunner flag in its else: 179), the driver, extra gunner and
// partner flag tests read as field bits (172, 661 bytes), and so does the grouped-seat assignment bit (166,
// 665 bytes). What remains is register allocation: retail keeps -1/the child index in ebp, the vehicle in
// esi and the object pool pointer in edx (reloaded after the recursive call), ours keeps the pool in esi
// and the vehicle in ebp; bit 13 of the seat stays `test ah, 0x20` (field-bit forms of it grow the code to
// 688-700 bytes, 196-213). Tried and dropped: a bool local for bit 13 (206); next sibling read from the
// held child pointer (222); a `none` local (166); short pass (177); driver preset false (166); field bit
// on the no-seat gunner flag (166).
// Round 4 of #1049: logic walk against retail found no behaviour difference (type mask, seat group flags 11/13/2/3/18,
// pass-0 and pass-1 assign tests, other-seat merge at +0x3e, driver/gunner stores, final driver-changed call all agree).
// Retail reads the next sibling through the child pointer it saved across the recursive call; retried as a
// function-scope `child` read in the loop increment: 222 (694 bytes, whole register plan changes), dropped again.
// Round 232 (m3 lane 3): the two action-bit tests through the existing unit_action_active(child_index, 0x20 / 0x1f)
// (retail inlines 0xe6fe0 here) and pass 0's test as `assign = !grouped && !unit_action_active(...)` without the
// check_driver goto: 166 (same code, kept). No gotos at all (seat arm nested under `!grouped || bit 13`, if/else-if
// driver/gunner choice, gunner test in both arms) 259 at 728 bytes; a named seat pointer for the flags and the +0x3e
// read 177 (671 bytes, masked aligned 29 -> 26); that with `child` declared before the inner loop and the next sibling
// read from it 173 (653 bytes); `child` before the inner loop alone, re-measured, 222.
// P435: readonly tag/child, immutable previous driver, seated-only driver scope, immutable
// seat flags, conditional pass assignment, outer pass workspace and traversal role names all 166;
// typed unit flag fields 166, plain 166. Entry pool/vehicle allocation still differs.
// MiniMax B (11-13): no new form, no lower score (166): retail keeps the vehicle pointer in esi and the header table pointer in edx and pushes its registers after the index arithmetic, ours keeps the vehicle in ebp and pushes first; the body is four bytes longer than retail's.
// T101: Complete extra-gunner/partner-driver/partner-gunner predicate updates,
// sets 1..7: 247/195/262/177/241/196/268; compound Boolean accumulation 201.
// NOT kept: all regress and miss equality; native pool/vehicle/child ownership
// remains different. Original complete body, accessors and activity calls restored.
// MiniMax A (11-14): no new form, no lower score (166): the prologue still keeps the global in esi
// and the vehicle in ebp with the loads after the pushes, where retail keeps the global in edx and
// the vehicle in esi and shrink-wraps the two global loads above push ebx; the register choice
// follows the frame's shape, which the recursion and the loop's children fix.
// @retail 0xcc810
void __stdcall function_cc810(long vehicle_index)
{
	s_unit *vehicle = UNIT_GET(vehicle_index);
	s_unit_definition *definition = (s_unit_definition *)UNIT_DEFINITION_GET(vehicle);
	long old_driver_index = vehicle->unknown248;

	vehicle->unknown248 = NONE;
	vehicle->unknown24c = NONE;
	if (TEST_FIELD_BIT((definition->unit.flags >> 26) & 1) && vehicle->parent_index != NONE && vehicle->actor_index == NONE &&
		(1 << ((byte *)g_4e0300->data)[(vehicle->parent_index & 0xffff) * 0xc + 3]) & 3)
	{
		vehicle->unknown24c = vehicle->parent_index;
	}
	for (long pass = 0; pass < 2; pass++)
	{
		for (long child_index = vehicle->first_child_index; child_index != NONE;
			child_index = UNIT_GET(child_index)->next_sibling_index)
		{
			s_unit *child = UNIT_GET(child_index);
			bool driver;
			bool gunner;

			if (!((1 << child->type) & 3))
			{
				continue;
			}
			short seat_index = child->parent_seat_index;
			if (seat_index != NONE)
			{
				s_unit_seat_definition *seats = UNIT_SEATS((byte *)definition);
				dword flags = *(dword *)&seats[seat_index].flags;
				bool grouped = (flags >> 11) & 1;
				bool assign;

				if (grouped && !((flags >> 13) & 1))
				{
					goto recurse;
				}
				driver = (flags >> 2) & 1;
				gunner = (flags >> 3) & 1;
				if (TEST_FIELD_BIT((flags >> 18) & 1) && child->unknown13c != NONE)
				{
					gunner = true;
				}
				if (pass == 0)
				{
					assign = !grouped && !unit_action_active(child_index, 0x20);
				}
				else
				{
					assign = grouped && unit_action_active(child_index, 0x1f);
				}
				if (grouped)
				{
					short other_seat_index = *(short *)((byte *)&seats[seat_index] + 0x3e);

					if (other_seat_index != NONE)
					{
						dword other_flags = *(dword *)&seats[other_seat_index].flags;

						if (TEST_FIELD_BIT((other_flags >> 2) & 1))
						{
							driver = true;
						}
						if (TEST_FIELD_BIT((other_flags >> 3) & 1))
						{
							gunner = true;
						}
					}
				}
				if (!assign)
				{
					goto recurse;
				}
				if (driver && !TEST_FIELD_BIT((vehicle->flags_134 >> 1) & 1) && vehicle->unknown248 == NONE)
				{
					vehicle->unknown248 = child_index;
					if (gunner && vehicle->unknown24c == NONE)
					{
						vehicle->unknown24c = child_index;
					}
					goto recurse;
				}
			}
			else
			{
				gunner = (((s_unit_definition *)UNIT_DEFINITION_GET(child))->unit.flags >> 25) & 1;
			}
			if (gunner && (vehicle->unknown24c == NONE || vehicle->unknown24c == vehicle->unknown248))
			{
				vehicle->unknown24c = child_index;
			}
		recurse:
			function_cc810(child_index);
		}
	}
	if (old_driver_index != vehicle->unknown248)
	{
		function_1bbdf0(vehicle_index);
	}
}

/* plans a motion over a distance from a velocity: accelerate, coast and
   decelerate (by the acceleration) without passing the speed limit when
   one is given; negative distances are planned mirrored */
// MiniMax G (11-13): no new form tried this round, no lower score (162): the one-use discriminant
// is still evaluated on the x87 stack where retail keeps it in SSE, so the frame stays eight
// bytes, and retail keeps stop_time*0.5f*velocity*0.5f unfolded where this build folds it into one
// multiply by 0.25.
// MiniMax H (11-13): no new form, no lower score (162): the one-use discriminant is still evaluated on the x87 stack where
// retail keeps it in SSE, so the frame stays eight bytes and retail's dead argument slots go unused; retail also keeps
// stop_time*0.5f*velocity*0.5f unfolded where this build folds it into one multiply by 0.25, and it reads the acceleration
// from the argument at each use where ours loads it into a register early; this round the stop position with stop_time
// first ties at 162, the same product split over two statements scores 181 and the done flag as a long scores 212, all
// restored, and the order the two builds multiply the stop distance in does not follow the order the source gives it.
// MiniMax G (11-11): no new form, no lower score (162); retail keeps no local frame (its spills land in dead
// argument slots, ours reserves eight bytes) and computes the quadratic's discriminant b*b - a*stop_position*4 in
// SSE, while ours chains it on the x87 stack through spills of a and b, which is what needs the frame; retail also
// reads the acceleration from the argument at each use where ours loads it early into a register, and the forms
// tried earlier (the discriminant in float locals, a double cast, the negation written out) all score 165 or more,
// so the body stands.
// MiniMax B (11-12): no new form, no lower score (162): a named float discriminant local, an explicit (double)
// cast on the sqrt argument, the file's flags line without /Gr and the same line with /fp:strict all score 162 and
// leave the discriminant on the x87 double stack; /Od rewrites the body (1343 bytes, 932), which shows the flags
// line is applied, so the widening comes from the expression tree and not from the build settings.
// score 162 (was 162; earlier 167): the logic agrees with retail statement by statement (re-walked in round 4 of #1049).
// Retail keeps no local frame (its spills go in dead argument slots, ours reserves 8 bytes), computes the
// quadratic's discriminant b*b - a*stop_position*4 in SSE and stores only for the fsqrt (ours chains it on the
// x87 stack through spills of a and b, which is what needs the frame), and keeps stop_time*0.5*velocity*0.5
// unfolded (ours folds to *0.25 and shares the product with stop_position); ours also loads the acceleration
// early (xmm7) where retail reads it from the argument at each use. Position/velocity stored before the done
// test (retail's order) 167. Round 4 of #1049: the acceleration and deceleration stores before the root choice
// 162 (the load still hoisted); the negated acceleration written out in the discriminant 165, kept in `negative`
// before the branch 166 (708 bytes, frame stays). No effect: stop_position written out at its three uses 167; the
// discriminant, b*b and a*stop_position*4 in float locals or a (double) cast 167; the first test's product orders
// (velocity first, distance first, paren pairs 210), stop_position before the first test (183), midpoint local;
// a/b swapped, product orders in the discriminant, two_a before or after the root, a+a, v+v, sqrtf or a
// real-parameter helper for the roots (all 167), a and b written out in the sums (200). Worse: named
// half_time/half_velocity temps 207-212.
// Round 144 (m3 lane 7): inverse and stop time before moving_away 162 (aligned better, same length); `motion->done =
// ...; if (motion->done)` 182 (722 bytes, retail's `mov eax, 1`); the acceleration/deceleration stores after the speed
// limit (where retail stores them) 167; all three 180; plus a `half_time` part in the first test (retail's unfolded
// st*0.5*v*0.5) 215; an inline discriminant helper 162 (with the three 180/215). The frame comes from the single-use
// discriminant going to x87 (compile-only: every one-use spelling, sqrtf included, does; a second use or a `> 0` guard
// keeps it in SSE and drops the frame, as retail, but retail shows neither).
// Round 234 (m3 lane 3): `negative` and its two stores after the speed limit (three store orders) 167 (same code as the
// stores alone); the else branch using `negative` in place of `a` 167 (703 bytes); `a` read from motion->acceleration
// 167; `peak` by if/else 164; the coast stored in each branch 167 (713 bytes); the limit clamp as a ternary 162 (same
// code). Compile-only: every discriminant spelling (constant first or last, named parts, `root` reused, `b = v + v`)
// stays on x87 with the frame; a `< 0` clamp of a named discriminant moves it to SSE and drops the frame.
// Root choice: the second root's clamp as an else-if chain 162 and the test as `second < 0.0f || first < second` 162
// (both same code); the test negated with the arms swapped 162 (other code); second computed before first 175 (716
// bytes); `time` defaulted to the clamped second root and overridden by the first 174 (715 bytes).
// T91: completed admission in a long 212; explicit bool/long producers 162/212;
// success-default bool/long with complementary failed tests 177/206; unfinished
// planning in the completion's else 162; moving-away begun after both early
// planning exits 185; chained phase clears 161. NOT kept: the chained clears gain
// only one; every other form ties or regresses. The x87 discriminant/frame remains,
// including the 708-byte success-default bool form. Original complete body restored.
// MiniMax A (11-12): no new form, no lower score (162): our build folds the first test's stop_time * 0.5f * velocity * 0.5f
// into one multiply by 0.25 where retail keeps three multiplies, and it evaluates the one-use discriminant on the x87 stack
// where retail keeps it in SSE with a spilled root; the done flag is also a byte store here and a dword store in retail.
// P434: immutable coefficient pair/root, entry-scope coefficients/root/time, descriptive
// coefficient/root-result names, and complete single-completion outcomes each 162. NOT kept:
// all tie; the discriminant stays on x87, frame 8 remains, and the first difference is +7.
// MiniMax B (11-13): no new form, no lower score (162): the one-use discriminant is still evaluated on the x87 stack where retail keeps it in SSE, and the done flag is still a byte store here against retail's dword store.
// T101: Existing shared square-root boundary at quadratic/positive/both 162/162/162;
// uniform fabsf 178, uncast fabs 162; reversed completion admissions 191;
// complete/default-false moving-away producers 162/186. NOT kept: no gain or match;
// the x87 discriminant and excess frame remain; original complete body restored.
// MiniMax A (11-14): no new form, no lower score (162): the one-use discriminant still lands on the
// x87 stack with an 8-byte frame where retail keeps it in SSE with a spilled root, and the first
// test's three multiplies are still folded into one multiply by 0.25; a named float discriminant
// local and the square root without the (real) cast compile to the same bytes, so the promotion to
// double is not source-controllable by those spellings.
// @retail 0xc7300
void __stdcall function_c7300(real distance, real velocity, real speed_limit, real acceleration, s_unit_motion *motion)
{
	motion->position = distance;
	motion->velocity = velocity;
	bool done = (real)fabs(distance) < 0.001f && (real)fabs(velocity) < 0.001f;

	motion->done = done;
	if (done)
	{
		motion->acceleration = 0.0f;
		motion->field_10_4 = 0.0f;
		motion->deceleration = 0.0f;
		motion->s_type_8c87de = 0.0f;
		motion->coast_time = 0.0f;
		return;
	}
	bool moving_away = velocity > 0.0f;
	real inverse_acceleration = 1.0f / acceleration;
	real stop_time = (real)fabs(velocity) * inverse_acceleration;
	if (0.0f > stop_time * 0.5f * velocity * 0.5f + distance)
	{
		function_c7300(0.0f - distance, 0.0f - velocity, speed_limit, acceleration, motion);
		motion->position *= -1.0f;
		motion->velocity *= -1.0f;
		motion->acceleration *= -1.0f;
		motion->deceleration *= -1.0f;
		return;
	}
	real stop_position = velocity * 0.5f * stop_time + distance;
	if (0.0f > stop_position)
	{
		real deceleration = velocity * velocity / (distance * 2.0f);

		motion->acceleration = 0.0f;
		motion->field_10_4 = 0.0f;
		motion->deceleration = deceleration;
		motion->s_type_8c87de = 0.0f - velocity / deceleration;
		motion->coast_time = 0.0f;
		return;
	}
	real negative = 0.0f - acceleration;
	motion->deceleration = acceleration;
	motion->acceleration = negative;
	real time;
	if (moving_away)
	{
		time = (real)sqrt(inverse_acceleration * stop_position);
	}
	else
	{
		real a = 0.0f - acceleration;
		real b = velocity * 2.0f;
		real root = (real)sqrt(b * b - a * stop_position * 4.0f);
		real first = (0.0f - b - root) / (a * 2.0f);
		real second = (root - b) / (a * 2.0f);

		if (first >= 0.0f && (0.0f > second || second > first))
		{
			time = first;
		}
		else
		{
			time = 0.0f > second ? 0.0f : second;
		}
	}
	real peak = time;
	if (speed_limit > 0.0f)
	{
		real limit_time = moving_away ? inverse_acceleration * speed_limit : (velocity + speed_limit) * inverse_acceleration;

		if (0.0f > limit_time)
		{
			limit_time = 0.0f;
		}
		peak = time > limit_time ? limit_time : time;
	}
	if (moving_away)
	{
		motion->field_10_4 = peak + stop_time;
		motion->s_type_8c87de = peak;
	}
	else
	{
		motion->field_10_4 = peak;
		motion->s_type_8c87de = peak + stop_time;
	}
	real coast = 0.0f;
	if (time > peak)
	{
		real left = time - peak;
		real speed = negative * motion->field_10_4 + velocity;

		coast = (left * speed * 2.0f - left * left * acceleration) / speed;
	}
	motion->coast_time = coast;
}

/* moves an object to a point (its own center by default) while the
   physics allow it: finds a clear spot near the target with its Havok
   component (made for the check when it has none, tried with a doubling
   radius), then places it there; without physics it just places it */
// score 225 (was 225): same size and the same code in the same order, with one exit (the no-physics branch is
// the else, so it ends the function as in retail) and the center kept in the move's position (retail's frame
// holds both in one slot, 0x48). What differs is register allocation: retail keeps root_index, root_point, the
// a5 value and the component in ebx, ebp, esi and edi through the loop and leaves the header, the rigid body
// index and the object index on the stack (reloaded at each use); ours keeps the header, the root index and the
// rigid body index in registers and spills the root point. Retail also pushes ebp and edi only on the physics
// path; ours pushes all four first. a7 is a bool, which retail passes on to 0x1d48f0 unchanged (as a long it
// needed a setne conversion there, 238 at 745 bytes).
// Tried: block scope for the center (251, no frame change); `long` rigid body index (231); the a5 value in a
// local before the loop (225, same code).
// Round 5 of #1049: the no-parent flag test `(unknownc0 >> 6) & 1` is now TEST_FIELD_BIT, which gives retail's
// `mov cl,[..]; shr cl,6; test cl,1` (it was `test byte [..],0x40`; the function is 1 byte longer, 734, and the score
// the same 225). Logic re-walked statement by statement against retail: no difference. Retail's `moved` is in BL
// on every path except inside the physics block (stored at the top, reloaded into BL after the loop, returned as
// `mov al,bl`), and a5 is loaded into ESI just before the loop (the only integer loop invariant besides root_index,
// root_point and the component; the header and the rigid body index stay on the stack). Not kept (all 225-238):
// a5 declared `real` (the call then pushes it from memory, 225) or `long` with the 0x1d48f0 declaration changed to
// long (227); `moved` split into an outer result (BL) plus an inner flag copied after the loop (233); the header
// expression written out at each use instead of a `header` local (230); declaration order of root_index/root_point/
// root_origin (225).
// MATCH (applied 11-10 by the m3 coordinator from lane 11 r85's held patch; full check: lost [], gained [0xc5460]).
// Round 85 (m3 lane 11), held then: byte match in the plain check (0xe0070, 0xe18d0 and 0x14e200 stay
// MATCH) with a5 a `real` passed straight to 0x1d48f0 (which reads it with movss; 0x14e200 passes a float's bits), which
// needs this function's declarations in src/bipeds.cpp (a contributor's file) and src/unknown_14e0f0.cpp retyped, plus a
// `long` rigid body index (retail stores and compares it 32-bit), the location and the moved position as two plain locals
// in place of the move aggregate (retail's frame sorts the 8-byte location below the 12-byte points), root_point declared
// before root_index and `ignore_index == function_baf80(ignore_index)`. Patch: m3-lane11 build/r85/held-0xc5460.patch.
// The `&a5` reinterpretation is what keeps a5 out of ESI and gives the header EBX. Steps: long index 238 (754 bytes); with
// the two plain locals 237 (every stack slot as retail); then `moved` declared first, the location declared last,
// root_point first 237 each; `real a5` 129 (733 bytes); then root_point first 1, the compare swapped 128, both MATCH.
// All but `real a5` together 237.
// @retail 0xc5460
bool __stdcall function_c5460(long object_index, long ignore_index, point3f const *position, point3f *result,
	real a5, real radius, bool a7)
{
	byte *header = (byte *)g_4e0300->data + (object_index & 0xffff) * 0xc;
	s_unit *object = ((s_unit_header *)header)->unit;
	bool moved = false;

	if (g_47f058)
	{
		bool detached = *(short *)(header + 4) == NONE;
		bool had_component = object->unknownb4 != NONE;
		if (detached && !had_component)
		{
			object->unknownb4 = havok_component_new(object_index);
			function_1cf120(object->unknownb4);
		}
		if (object->parent_index == NONE && object->unknownb4 != NONE)
		{
			s_havok_component *component = havok_component_get(object->unknownb4);
			long rigid_body_index = havok_component_main_rigid_body_index_get(component);

			if (rigid_body_index != NONE)
			{
				s_location location;
				point3f center;
				point3f target;
				vector3f offset;
				point3f *root_point = 0;
				long root_index = NONE;
				point3f root_origin;

				function_b9dd0(object_index, &center);
				function_b9ef0(object_index, &target);
				offset.i = target.x - center.x;
				offset.j = target.y - center.y;
				offset.k = target.z - center.z;
				target = position ? *position : center;
				if (object->unknown13c != NONE && ignore_index != NONE && ignore_index == function_baf80(ignore_index))
				{
					root_index = ignore_index;
					root_point = function_b9ef0(ignore_index, &root_origin);
				}
				for (short attempt = 0; attempt < 3; attempt++)
				{
					if (function_1d48f0(&center, component, rigid_body_index, header[3] ? 0xc : 9, &target, &offset, &location, a5,
						radius, a7, root_point, root_index))
					{
						if (result)
						{
							*result = center;
						}
						moved = true;
						break;
					}
					radius *= 2.0f;
				}
				if (detached && !had_component)
				{
					if (*(short *)(header + 4) == NONE)
					{
						havok_component_delete(object->unknownb4);
						object->unknownb4 = NONE;
					}
					else if (TEST_FIELD_BIT((object->unknownc0 >> 6) & 1))
					{
						function_1d1540(havok_component_get(object->unknownb4));
					}
				}
				if (moved)
				{
					function_b75a0(object_index, &center, 0, 0, &location, false);
					function_b9b90(object_index, false);
				}
			}
		}
	}
	else if (position)
	{
		function_b75a0(object_index, position, 0, 0, 0, false);
		if (result)
		{
			*result = *position;
		}
	}
	return moved;
}

/* clamps a direction to the unit's yaw and pitch limits for aiming (or,
   when asked, for looking, relative to its aim) in its own frame; whether
   it changed */
// MiniMax A (11-14): no new form, no lower score (169): the first difference is inside the shared cross-product helper,
// whose own file this lane may not edit, so the two products land in other registers; retail loads up.k and up.j before
// up.i where this build preloads all three.
// MiniMax H (11-14): no new form, no lower score (169): the first difference still falls inside the shared cross-product helper, whose file this lane may not edit, and the staged spelling of that helper measures 169 again; the body is one byte longer, so the branch after the helper shifts.
// MiniMax G (11-13): no new form, no lower score (169): the diff re-read this round puts the first difference inside the shared cross-product helper, whose own file this lane may not edit, and the staged spelling of that helper measures 169 again, so the body stands.
// MiniMax B (11-13): no new form, no lower score (169): re-measured with the plain build (768 bytes against retail 767, first difference at +0x4a); what is left is the x87 stack use in the three direction sums and the order retail loads up.k and up.j in.
// MiniMax H (11-12): no new form, no lower score (169): retail loads up.k and up.j before up.i where this build preloads all three, retail keeps the clamped yaw and pitch in the frame where this build reuses the two incoming argument slots, and the local vector sits at +0xc where retail's is at +0x18; the cross-product helper's own file stays as it is.
// MiniMax G (11-11): no new form, no lower score (169); the argument orders that reach 127 (the second cross product, the
// frame call and the two angle readings with their operands exchanged) compute other values than retail - retail's bytes
// give the up vector as forward x (up x forward), the frame's forward as the forward vector and its up as the up vector,
// and the angles as atan2(left, forward) and atan2(up, sqrt) - so the body stands as found. What still differs: retail
// loads up.k and up.j before up.i while this build preloads all three, the forward scale's x87 term is scheduled
// differently, retail keeps the clamped yaw and pitch in the frame (+0xc and +0x10) where this build reuses the two
// incoming argument slots, and the local vector sits at +0xc where retail's is at +0x18.
// MiniMax D (11-11): no new form, no lower score (169).
// MiniMax G (11-12): no new form tried this round, no lower score (169): the cross-product helper's own file stays
// as it is, so the two component loads and the vector home cannot be brought into retail's order from here.
// score 169 (was 169; earlier 179): the logic agrees with retail statement by statement (re-walked in round 4 of #1049).
// Round 4 of #1049: the two cross products through the repo's cross3f_staged / cross3f (retail's left vector is
// never stored and all three components are formed before the up stores) give 169 here, but including
// vector_products.h in this file costs matched 0x41640 (src/unknown_0350e0.cpp, two stack slots swapped) in the
// full build, so they are not kept; the staged form written out in place 184; plain cross3f for both 169; yaw and pitch in one local aggregate put them over `up` at +0x14/+0x18 as retail
// does, but scores the same 169 (not kept). What still differs: in the three direction sums' last term retail loads the matrix
// component and multiplies by forward_scale on the x87 stack (fld [m]; fmul st(n)) and keeps forward_scale and
// cosine until two final pops; ours copies the scale (fld st(3)) and consumes it at its last use. A double
// forward_scale (no cast) gives retail's forms (aligned differences 12 -> 9) but then cosine is rounded through a
// float spill (fstp/fmul [mem]), score 189; double on cosine, left_scale or up_scale as well 185-200. Also one
// load/multiply order in left.j. No effect: product operand orders in the cross products and sums; the order of
// the six cross product statements (36 permutations, only worse); order of the scale statements (176-183);
// scalars or an array for the local vector (189/179); the local over `forward` itself (183); other scopes or
// declaration orders for up/forward/local/yaw/pitch; one temporary per product (180); a component-wise copy of
// the aim into forward (172, different code).
// Round 85 (m3 lane 11): both cross products through cross3f (vector_products.h is included in this file now, and
// 0x41640 stays MATCH) 179 -> 169 at 768 bytes, kept; cross3f_staged for the second or both 169. On that: yaw and pitch
// as the existing s_view_angles (retail's slots exactly: plain-aligned 56 -> 34) 169; the clamped direction through
// vector_angles.h's direction_from_angles 169, with s_view_angles 169; s_view_angles with cross3f_staged second 169; the
// rotation through this file's unit_vector_from_frame (forward-declared) 169. Left: the cross products' SSE schedule
// (retail loads up.k and up.j first and up.i later; ours preloads all three) and the x87 forward_scale term.
// Round 232 (m3 lane 3): the local frame through dot3f(direction, &matrix.forward / left / up) 255 (690 bytes); the
// forward and left scales as `(real)(cos(yaw) * cosine)` 169 (same code); sinf/cosf for the four scales 169 (same
// code); s_view_angles re-measured 169 (aligned 34, register-masked 13: only the first cross product's load order and
// copies and the x87 forward term are left); yaw and pitch through atan2f/sqrtf 169 (same code); the four clamp
// comparisons with their operands reversed 169 (same code).
// P435: angle/vector/frame role names, result first, CRT forward copy, readonly
// receivers and immutable scales 169; direct projection field reads 262; alias-safe
// in-place staged crosses 188. NOT kept: no gain; native cross loads and scalar homes remain.
// MiniMax B (11-13): no new form, no lower score (169): the three direction sums' last term still multiplies the matrix component by forward_scale on the x87 stack where retail keeps the scale in a register until two final pops, and retail loads up.k and up.j first where ours preloads all three.
// T101: Existing vector setter at basis-copy/projection/reconstruction publications,
// sets 1..7: 179/183/178/169/174/183/178; all with disabled early result 185.
// NOT kept: no match or gain; delayed up.i, native scalar/vector homes and x87
// forward-scale residence remain. Original complete body and shared helpers restored.
// @retail 0xcba50
bool __stdcall function_cba50(long unit_index, vector3f *direction, bool looking)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *state = (byte *)unit + unit->unknown33e;
	bool clamped = false;
	bool enabled;
	real const *limits;

	if (looking)
	{
		enabled = (state[0] >> 6) & 1;
		limits = (real const *)(state + 0x14);
	}
	else
	{
		enabled = (state[0] >> 5) & 1;
		limits = (real const *)(state + 4);
	}
	if (enabled)
	{
		vector3f up;
		vector3f forward;

		function_b9fc0(unit_index, &forward, &up);
		if (looking)
		{
			vector3f left;

			forward = unit->unknown168;
			cross3f(&up, &forward, &left);
			cross3f(&forward, &left, &up);
			if (function_30bf0(&up) == 0.0f)
			{
				function_30bf0(function_11d000(&forward, &up));
			}
		}
		transform4x3f matrix;
		function_1420f0(&matrix, g_468788, &forward, &up);
		real i = direction->i;
		real j = direction->j;
		real k = direction->k;
		vector3f local;

		local.i = matrix.forward.k * k + matrix.forward.j * j + matrix.forward.i * i;
		local.j = matrix.left.k * k + matrix.left.j * j + matrix.left.i * i;
		local.k = matrix.up.k * k + matrix.up.j * j + matrix.up.i * i;
		real yaw = (real)atan2(local.j, local.i);
		real pitch = (real)atan2(local.k, (real)sqrt(local.j * local.j + local.i * local.i));

		if (limits[0] > yaw)
		{
			yaw = limits[0];
			clamped = true;
		}
		else if (yaw > limits[1])
		{
			yaw = limits[1];
			clamped = true;
		}
		if (limits[2] > pitch)
		{
			pitch = limits[2];
			clamped = true;
		}
		else if (pitch > limits[3])
		{
			pitch = limits[3];
			clamped = true;
		}
		if (clamped)
		{
			real cosine = (real)cos(pitch);
			real forward_scale = (real)cos(yaw) * cosine;
			real left_scale = (real)sin(yaw) * cosine;
			real up_scale = (real)sin(pitch);

			direction->i = matrix.up.i * up_scale + left_scale * matrix.left.i + matrix.forward.i * forward_scale;
			direction->j = matrix.up.j * up_scale + left_scale * matrix.left.j + matrix.forward.j * forward_scale;
			direction->k = matrix.up.k * up_scale + left_scale * matrix.left.k + matrix.forward.k * forward_scale;
		}
	}
	return clamped;
}

/* where a unit's view or weapon is: its aim (when asked, the unit's own),
   a point projected onto the aim line from its center, moved by offsets
   along the aim and its left and up, then held clear of the world from
   its center; and its root object's velocity */
// score 2 (was 184; earlier 202, 218): frame 0x24 and 808 bytes agree; the first cross
// product's j minuend loads aim.i before world-up.k. Earlier history follows.
// Earlier 184 (was 202; earlier 218): the frame, convention (aim and point in registers, the other six arguments on the stack,
// ret 0x18), branch shapes and call order agree; what differs is xmm register allocation and operand order.
// Retail loads the dot product's seven operands first (aim.i in xmm0, the center in xmm4-6) and multiplies in a
// register; ours folds the aim into memory operands. In each offset step retail loads the vector component and
// multiplies it by the scale; ours copies the scale and multiplies it by the component for the first term.
// The offsets pointer only stays on the stack (as retail's does) when it is read three times, not nine, so the
// three steps go through an inline helper. The root loop must keep its own index: counting down unit_index
// itself leaves ebp live past retail's early pops (215 with the helper, but a different tail).
// Round 4 of #1049: a statement-by-statement walk found no logic difference (the project/clip/use_aim tests,
// the cross products, the 0.0 compare, the clip call and the root loop all agree); the 45 bytes are loads retail
// does not fold (about ten). Reading aim.i/j/k once into locals for the projection (dot product and the three
// stores) makes the compiler load them unfolded, as retail does: 202 and 776 bytes. Retail re-reads them after
// each store to point (it cannot rule out aliasing), so the locals are a spelling that only matters if aim
// overlaps point, which no caller does. Retail also keeps aim in esi and point in edi (ours the reverse).
// Tried (all 218): dx/dy/dz locals; `(point - center) * aim` operand order (it also reorders y before z); the
// scale first in the helper; the clip call's unit read through a local (its call order then matches retail);
// the helper's three products as locals before the adds; the dot product as three statements (220); aim.i alone
// in a local (220). Locals for the three offsets instead of the helper (229) fuse the three steps into one sum.
// Round 232 (m3 lane 3): 202 -> 184 through the existing helpers: both cross products through cross3f (186, 792
// bytes); the offset steps through vector3f_add_scaled with the point cast as bipeds.cpp's eye point does, in place of
// the file's own move-along helper (alone 187, aligned 65 -> 44); aim read at each use in the projection, no ai/aj/ak
// locals (187; on cross3f alone 204); the projection's offset and dot product through vector3d_from_points3d + dot3f
// (184, aligned 25); the projected point through vector3f_add_scaled (184, same code). Same 184: the dot written in
// retail's (i + k) + j grouping (aligned 29); dot3f(&offset, aim); the root loop as a while loop; the clip call taking
// function_baf40() directly (aligned 35). Left (16 bytes short): aim in EDI and point in ESI (retail ESI/EDI); the dot
// product folds aim.i and sums j first; the k*j cross terms load the other operand; the first add_scaled product copies
// the scale; the root loop head gets a 2-byte pad (retail inlines 0xbaf80, which is noinline here).
// Round 234 (m3 lane 3): 184 -> 12 (808 bytes) by declaring point before aim: retail's callee keeps aim from EAX and point
// from ECX, and its one caller (0x104150) sets EAX, then ECX; ours had them the other way round (definition, the
// weapons.cpp declaration and the call changed together). Left at 12: the dot sums (i + k) + j in retail, ours
// (i + j) + k; and in the first cross product retail loads a->k for the j minuend where ours loads b->i. On 12: the
// dot written in i, k, j order, named or inline, 12 (the compiler reorders it); dot3f(&offset, aim) 12; the sum inline
// as j + k + i 2, j + i + k 2, dot3f's own body inline (i + j + k) 2, offset-first orders 2 (all hand-written dots,
// so not kept without a match); on those, cross3f_staged 2, the first cross product written out 91 (796 bytes), with
// the j minuend's operands swapped 91; the permuter on the inline-dot form found nothing below 2.
// Then 12 -> 2 with both cross products through the existing cross3f_staged (the dot then sums as retail, dot3f kept);
// only the second staged 12, only the first 12; on 2 the inline dot forms and dot3f(&offset, aim) 2 (same code). Left:
// the first cross product's j minuend `a->k * b->i` loads b->i first (retail a->k); it does so even without the
// projection block (diagnostic), so it is local to that product. The world-up global in a local first 6; the first
// cross product written out (staged order) 108 (796 bytes), with that minuend's operands swapped 108.
// Also on 2: `up` declared before `left` 2, `up` declared after the zero test 2, `0.0f == function_30bf0(&left)` 121.
// T91: existing scalar-input setter for aim/fallback/aim+fallback/clip/aim+clip/fallback+clip/all
// publications 216/131/201/62/202/122/217; world-up copied into left before an alias-safe
// in-place staged cross 179. NOT kept: no match; original copies and helper calls restored.
// First +0x12a still exchanges the first cross j minuend's load and memory multiplier.
// P432: CRT aim/fallback/clip copies 2/2/2; basis/projection role names 2/2;
// function-scope projection/vertical workspaces 2/2; normalization result 21.
// NOT kept: no match; first cross j load and memory multiplier still exchange.
// @retail 0xc8290
void __stdcall function_c8290(point3f *point, vector3f *aim, long unit_index, vector3f *velocity, real const *offsets,
	bool project, bool use_aim, bool clip)
{
	s_unit *unit = UNIT_GET(unit_index);
	point3f center;

	if (project || clip)
	{
		function_cafc0(unit_index, &center);
	}
	if (use_aim)
	{
		*aim = unit->unknown168;
	}
	if (project)
	{
		vector3f offset;

		vector3d_from_points3d(&center, point, &offset);
		vector3f_add_scaled((vector3f const *)&center, aim, dot3f(aim, &offset), (vector3f *)point);
	}
	if (offsets)
	{
		vector3f left;
		vector3f up;

		cross3f_staged(g_4687b0, aim, &left);
		if (function_30bf0(&left) == 0.0f)
		{
			left = *g_4687ac;
		}
		cross3f_staged(aim, &left, &up);
		function_30bf0(&up);
		vector3f_add_scaled((vector3f const *)point, aim, offsets[0], (vector3f *)point);
		vector3f_add_scaled((vector3f const *)point, &left, offsets[1], (vector3f *)point);
		vector3f_add_scaled((vector3f const *)point, &up, offsets[2], (vector3f *)point);
	}
	if (clip)
	{
		point3f clipped;
		long ignore_index = function_baf40(unit_index);

		if (function_16a7c0(&center, point, ignore_index, NONE, &clipped))
		{
			*point = clipped;
		}
	}
	long root_index = NONE;
	for (long index = unit_index; index != NONE; index = UNIT_GET(index)->parent_index)
	{
		root_index = index;
	}
	function_ba1d0(root_index, velocity, 0);
}

void rotate_direction(vector3f *vector, vector3f const *axis, real sine, real cosine); /* forced inline in src/unknown_1eb8a0.cpp */

/* the unit's wandering direction: its actor's (else the default vector)
   turned about the world's up by the angle +0x2d8, which drifts by a
   random turn rate (+0x2dc) kept away from its limits */
// score 58 (was 91): earlier 99, was 120; only register numbering differs. Retail keeps the clamps in xmm4/xmm5 and the pi/4 and
// 4.244 constants share xmm3; ours puts them in xmm3/xmm6 and 4/5, then needs one more register in the rotation.
// Reading direction->k directly in the three terms (instead of a cached z) gave 120 -> 99.
// Tried (all 99): operand order of the angle and rate products, swapped high/low declaration order, low copied
// from high, conditional-expression clamps; reading i, j directly in the first terms (99); rotated vector built
// in a temporary and copied (122).
// Round 1 of #1049 (fast, kept at 99): a fresh walk found no logic difference (clamps, the two branches' scale and
// random-turn formulas, the 0x45e240/0x45e244 constants and the Rodrigues rotation d*cos + a*along - (d x a)*sin
// all agree, the x87 tail matches byte for byte). Two structural facts for whoever continues. (1) Retail takes the
// sums angle + pi/4 and rate + 2pi into the first block but multiplies them by the constant only after the
// preceding clamp (`a`, test, `b`, test; `c`, test, `d`, test): that source shape, with the sums as locals
// declared first, gives retail's 810 bytes and its c/d block, but the clamps land in xmm3/xmm4 instead of
// retail's xmm4/xmm5 and the score rises to 170 (171 with either high/low order), so it was not kept; low/high
// swapped, high = low and low = high behave the same. (2) Retail reads the direction's i and j from memory at
// each use and forms (d x a) as three early registers; named locals for those terms or for x/y/z moved the
// direction pointer from ESI to EDI (181) or gave 800 bytes (174).
// Round 102 (m3 lane 9): retail forms the dot product and all three cross terms of the unchanged direction
// before any store, so the rotation now goes through the shared cross3f(direction, axis) and dot3f(axis,
// direction): 99 -> 91 plain at retail's 810 bytes, kept. On that: the dot product line before the cross product
// 93; cross3f_staged 91 (same code); each clamp product followed by its own test 92; both random turns through the
// existing function_259d0 101; the axis read from its global at each use 91 (same code). Left: the 1.0 constant
// and the clamps take xmm5 and xmm2/xmm6 where retail has xmm2 and xmm4/xmm5, and retail interleaves the cross
// terms with the dot product.
// Round 66 (m3 lane 2, plain 91 -> 58, kept): the rotation is the existing rotate_direction (forced inline in
// src/unknown_1eb8a0.cpp, matched in 0x1ecf50 and already called from other files through a declaration): its dot
// first, then the three cross terms, is retail's order, and 1.0 now takes xmm2 as retail's does. Left: high/low take
// xmm3/xmm6 and the a/b constants xmm4/xmm5 where retail has xmm4/xmm5 and one shared xmm3 (retail forms the second
// sum before the first product, so pi/4 dies and its scale reuses xmm3), and three cross products take their
// operands the other way round. On 58 (fast): the sums as locals declared first (re-measure) 170 (masked 19, 1.0 in
// xmm2 but high/low in xmm3/xmm4); the sums with below declared first 170; sums in the a/b block only 168; in the
// c/d block only 61; each product right before its test (re-measure) 74; `high = low = 1.0f` 58 (same code); one
// declaration for both 58 (same code). Also tried, not kept: the same helper copied privately here 58 (same code).
// T91: current angle/rate read once for both bound products, separately/both,
// 80/82/97; complete actor-query result 168; default vector through the existing
// setter 178; success-default actor result 183; coherent bound-producer names 58;
// updated angular velocity named for its store and integration 58. NOT kept:
// no equality or gain; original observations, copies, result and rotation restored.
// P405 fast: direct angular limits, complete rate/angle clamps, separate actor query admission, named upper bound, random-first high branch, named trig results, float trig calls; scores 142, 58, 175, 142, 57, 96, 58, 62.
// NOT kept: the upper-bound local gives only one point (57), below the retention threshold;
// other calibrated forms tie or regress. Original body and existing rotation restored.
// P432: normalized random helper, upper/lower range helpers, actor observation,
// CRT fallback, actor byte result, entry bounds, integrated angle result, both range helpers;
// fast scores 58/43/43/58/58/58/266/105/61.
// NOT kept: no permitted confirmed gain; original executable body restored.
// Individual range helpers confirm 43/43; uniform use 61. NOT kept: selecting one
// parallel producer only changes later rotation registers; native samplers already agree.
// @retail 0xd0080
void function_d0080(long unit_index, vector3f *direction)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool from_actor = false;

	if (unit->actor_index != NONE && function_1e3370(unit->actor_index, direction))
	{
		from_actor = true;
	}
	else
	{
		*direction = *g_4687a8;
	}
	real high = 1.0f;
	real low = 1.0f;
	if (from_actor)
	{
		real a = (0.7853982f - unit->unknown2d8) * 4.2441316f;
		real b = (unit->unknown2d8 + 0.7853982f) * 4.2441316f;

		if (1.0f > a)
		{
			high = a;
		}
		if (1.0f > b)
		{
			low = b;
		}
	}
	real c = (6.2831855f - unit->unknown2dc) * 0.53051645f;
	real d = (unit->unknown2dc + 6.2831855f) * 0.53051645f;
	if (high > c)
	{
		high = c;
	}
	if (low > d)
	{
		low = d;
	}
	real turn;
	if (low > high)
	{
		if (-1.0f > high)
		{
			turn = -0.62831855f;
		}
		else
		{
			real scale = 1.0f > high ? high : 1.0f;
			real random = (real)random_next(&g_4e7408->unknown0) * (1.0f / 65535.0f);

			turn = random * (scale * 0.62831855f - -0.62831855f) - 0.62831855f;
		}
	}
	else if (-1.0f > low)
	{
		turn = 0.62831855f;
	}
	else
	{
		real scale = 1.0f > low ? low : 1.0f;
		real lower = scale * -0.62831855f;
		real random = (real)random_next(&g_4e7408->unknown0) * (1.0f / 65535.0f);

		turn = random * (0.62831855f - lower) + lower;
	}
	unit->unknown2dc += turn;
	unit->unknown2d8 += g_510c54->rate * unit->unknown2dc;
	if (-3.1415927f > unit->unknown2d8)
	{
		unit->unknown2d8 += 6.2831855f;
	}
	else if (unit->unknown2d8 > 3.1415927f)
	{
		unit->unknown2d8 -= 6.2831855f;
	}
	rotate_direction(direction, g_4687b0, (real)sin(unit->unknown2d8), (real)cos(unit->unknown2d8));
}

/* the best seat for the unit to enter among the object's and its riders'
   seats (0xc8a40): one it can reach (0xc6fb0) with an entry animation
   (0xc7160) its animation graph has, ranked by 0xc92c0's priority and
   then distance (non-driver seats count half again as far once a
   driver's seat is found) */
// @retail 0xc8bb0
void function_c8bb0(long unit_index, long vehicle_index, long *object_index, short *seat_index, short *priority,
	real *distance, bool *flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_unit *vehicle = UNIT_GET(vehicle_index);
	s_object_seat seats[0x40];
	short count = 0;

	function_c8a40(vehicle_index, seats, &count, 0x40);
	if (unit->unknown13c != NONE && TEST_FIELD_BIT((vehicle->flags_134 >> 12) & 1))
	{
		return;
	}
	short entry_count = count;
	for (short i = 0; i < entry_count; i++)
	{
		s_object_seat *entry = &seats[i];
		long candidate_index = entry->object_index;
		s_unit *candidate = UNIT_GET(candidate_index);

		if (unit->unknown13c != NONE && TEST_FIELD_BIT((candidate->flags_134 >> 12) & 1))
		{
			continue;
		}
		s_unit_seat_definition *definition = entry->definition;
		short seat = entry->seat_index;
		if (!(unit->unknown13c == NONE && !TEST_FIELD_BIT((*(dword *)&definition->flags >> 17) & 1) ||
			unit->unknown13c != NONE && !TEST_FIELD_BIT((*(dword *)&definition->flags >> 16) & 1)))
		{
			continue;
		}
		if (!function_c6fb0(candidate_index, unit_index, seat))
		{
			continue;
		}
		point3f marker_position;
		point3f entry_position;
		if (function_c7160(unit_index, seat, candidate_index, (long)&marker_position, (long)&entry_position) == NONE)
		{
			continue;
		}
		real dx = unit->unknown030.x - entry_position.x;
		real dy = unit->unknown030.y - entry_position.y;
		real dz = unit->unknown030.z - entry_position.z;
		real entry_distance = (real)sqrt(dz * dz + dy * dy + dx * dx);
		dx = unit->unknown030.x - marker_position.x;
		dy = unit->unknown030.y - marker_position.y;
		dz = unit->unknown030.z - marker_position.z;
		real marker_distance = (real)sqrt(dz * dz + dy * dy + dx * dx);
		real nearest = entry_distance > marker_distance ? marker_distance : entry_distance;

		if (TEST_FIELD_BIT((*(dword *)&definition->flags >> 9) & 1) && candidate->unknown248 == NONE)
		{
			continue;
		}
		long label = definition->label;
		if (label == NONE || !function_1cb920(UNIT_ANIMATION(UNIT_GET(unit_index)), label))
		{
			continue;
		}
		long blocker_index = NONE;
		bool blocked_by_driver = false;
		short rank;
		if (function_c92c0(unit_index, candidate_index, seat, &blocker_index, &blocked_by_driver))
		{
			rank = blocked_by_driver ? 3 : 2;
		}
		else
		{
			if (blocker_index == NONE)
			{
				continue;
			}
			s_unit *blocker = UNIT_GET(blocker_index);
			if (blocker->actor_index == NONE || !function_1c9500(unit_index, blocker->actor_index, 0))
			{
				continue;
			}
			rank = 1;
		}
		if (rank <= 0)
		{
			continue;
		}
		bool driver = TEST_FIELD_BIT((*(dword *)&definition->flags >> 2) & 1);
		real scale = 1.0f;
		if (*flag && !driver)
		{
			scale = 1.5f;
		}
		if (*seat_index == NONE || rank > *priority || *distance > scale * nearest)
		{
			*object_index = candidate_index;
			*seat_index = seat;
			*priority = rank;
			*distance = nearest;
			*flag = driver;
		}
	}
}

/* one of the unit's four recent damage sources (+0x2f0, 0x10 bytes each) */
struct s_unit_damage_source
{
	long time;
	real amount;
	long object_index;
	long player_index;
};

#define UNIT_DAMAGE_SOURCES(unit) ((s_unit_damage_source *)((byte *)(unit) + 0x2f0))

/* records damage the unit took from a player or object (adding to its
   entry, else filling a free one or replacing the least recent of those
   but the largest); when asked, counts a hit from an ally against the
   unit or vehicle seat responsible (betrayal, reset after four seconds) */
// MiniMax H (11-12): the threshold test now compares the short count against a short limit, so it is a
// 16-bit compare as in retail (31 -> 13, 855 bytes against retail's 854); what was left was the first loop's
// add (ours loads the source amount and adds the incoming amount, retail copies the incoming amount and
// adds the source amount) and the two branch targets that follow it. Nine forms of that add (the two
// operand orders, the addend in a local, the pointer to the amount field, the time store before the amount
// store and the cast) all compile to the same bytes here. The order is reached by taking the damage source
// entry itself through a pointer - `s_unit_damage_source *source = &UNIT_DAMAGE_SOURCES(unit)[i];` with the
// amount read and written as `source->amount` - so the entry's amount is added to the incoming amount with
// retail's operand order and the flag load lands where retail puts it: the function matches retail byte for
// byte.
// score 0 (was 31; earlier 129, 140, 313): matches retail: the first loop takes the damage source entry
// through its own pointer, so the amount is added to the entry's stored amount with retail's operand order
// (the incoming amount copied into the register and the entry's amount added to it) and the flag load falls
// where retail puts it. Earlier loop and responsible-source differences (all closed by that pointer form):
// score 31 (was 31; earlier 129, 140, 313): 857/854 bytes; the first accumulation's load order
// and the final short comparison remain. Earlier loop and responsible-source differences:
// retail keeps the largest source's index in EDX and the slot in EAX where ours has them the other
// way round (it widens the index once, `movsx edx, dx`; ours copies it to a long, `kept`, which
// gives that widening but in the other register), and retail holds NONE in EBP for the team, player,
// unit and seat compares (`or ebp, -1`) where ours compares against -1. The datum lookup of the
// object also differs by one register. Behaviour fixes in this version: the largest amount is
// kept, not the smallest (retail's comiss order), and the team test takes the unit's team first.
// Tried: largest declared first 200 (index lands in EDI); slot not reset to NONE before the
// slot-0 test 231; largest tested directly as a short 191; `slot = largest != 0 ? 0 : NONE` 211;
// the seat NONE test inside each type arm 146; the seat index set by an if/else 140.
// Round 3 of #1049 (retail walk: no logic difference found; the callee, its arguments and every field agree):
// what keeps the score is the constant -1. Retail hoists NONE into EBP for the second half (`or ebp, -1` after
// the notify test; the team, player, player-unit and both seat tests compare against EBP; the object test and
// the two tail tests use an immediate where EBP is reused), puts the objects pool in EDI and the header in EBP,
// and keeps the seat test in each type arm (two `cmp ecx, ebp`) with one shared lookup. Ours compares every
// test with an immediate, merges the two seat tests after the ternary, and takes the pool in EBP. Nothing
// triggered the hoist: seat test in each arm of an if/else (the compiler merges them again) 146, an extra
// `responsible_index != NONE` 150 (858 bytes), `responsible_index` uninitialised 140, the whole betrayal part
// nested under `if (notify && team != NONE && ...)` 140. Every second-half change below also flips the first
// loop's `total += amount` to `movss xmm1, [src]; addss xmm1, amount` (retail: movaps then addss with the
// source), so its score is not comparable: `largest` kept as a short with no `kept` (16-bit compares) 191;
// `(long)largest` casts 191; `kept` declared before `largest` 140; the threshold compare cast to short
// (`cmp cx, bx` as retail, instead of ours movsx + 32-bit) 146; the pool cached in a local, `objects` (lands in
// EDI as retail) 146; that with the object path's unit in its own `s_unit *object_unit = 0` assigned to
// `responsible` after the header test (gives retail's `xor esi, esi` / `test esi, esi` shape) 146 at 848 bytes,
// also 146 and 854 bytes without the pool local. Not kept: none lowers the score. Largest-block residual: retail
// keeps `slot` live in EAX across the block so `largest` takes EDX and widens in place (`movsx edx, dx`).
// Round 144 (m3 lane 7): 140 -> 129 with the largest search and the oldest-other choice as two plain loops over the four
// sources (main's `slot = NONE;` kept between them): VC hoists the loop-invariant widening of `largest` into EDX, and
// the whole first half now matches retail. Without the `slot = NONE;` 229; short counters 342. On the loops: object
// index assigned before the header lookup 135; seat test and lookup in each type arm 135 (our compiler merges both
// tests); short threshold 135; both of the first two 143; the objects pool in a local at the top of the second half 135
// (pool in EDI as retail, still no NONE register), after the player lookup 140. Left: retail's NONE in EBP (`or ebp, -1`
// before the team test and again before the seat tests), the header in EBP, the seat test per arm, `cmp cx, bx`.
// Round 232 (m3 lane 3): 129 -> 31 with three changes together: the object path through the existing type-checked
// getter function_badc0 (alone 142 at 846 bytes, aligned 41 -> 33); the seat test and lookup in each type arm (with it
// 137); the unit lookups through the field read at each use, seat arms (135, 854 bytes, arms no longer merged) and player
// path (31: NONE now in EBP, pool EDI, header EBP as retail). Left (31, 857 bytes): the threshold compare, ours
// `movsx ecx, cx; cmp ecx, ebx`, retail `cmp cx, bx`; and the first loop's add, ours `movss xmm1, [eax]; addss xmm1,
// xmm0`, retail `movaps xmm1, xmm0; addss xmm1, [eax]`. The limit as a one-use named short before the test gives the
// 16-bit compare: 13 (855 bytes), not kept (one-use local, width only). The add order follows whole-function numbering:
// the compile-only harness reproduces it (main's body and the per-arm form without the field reads give retail's order,
// each later change flips it). Measured under LTCG, same code: `+=` in place of the total copy; the limit inside an
// `if (g_4f55d0->active)` block. Harness screens only, the add stays ours: `total = source + amount`, `amount + source`,
// a pointer to the source entry, the time store first, found declared before now, `last` read at each use, the player
// as a typed player datum, the notify return without braces (found stored first changes other code). Also: the
// player's unit index assigned alone then looked up after its own NONE test 142.
// Round 234 (m3 lane 3), each on the one-use short limit (13): the damage sources as a real member array of s_unit
// in place of the byte pad and cast 13 (same code); the time global read at each use 54; `(short)(c ? 5 : 3)` 13;
// `c ? (short)5 : (short)3` 13; `short const limit` 13; `c == NONE ? 3 : 5` 13; `short limit = 3; if (c) limit = 5;`
// before the active test 59 and as if/else 71; inside `if (g_4f55d0->active)` with `limit = 3; if (c) limit = 5;` 24
// (the add takes retail's order, but the limit is a branch where retail has setne/lea), with `limit <= count` 24,
// if/else 60, `limit = 5; if (c == NONE) limit = 3;` 36, `limit += 2` 36, `long limit` 31. Re-measured under LTCG:
// `total = amount + source` 13, found declared first 13, `last` read at each use 13. In the harness only the second
// half's code moves the add (spellings, operand sides, nested early returns and dummy stores do not).
// Native source/free-slot cursors 35/35; updated count used for store/test 34;
// complete expiry/admission predicates 31/31; selected threshold tests 52;
// separate three/five-hit cases 49; separate second clock value 31.
// NOT kept: ties or regressions; original body restored.
// @retail 0xca700
void __stdcall function_ca700(long unit_index, real amount, short type, bool notify, long player_index, short team,
	long object_index, byte flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	long now = g_510c54->game_time;
	bool found = false;

	for (long i = 0; i < 4; i++)
	{
		if ((player_index != NONE && UNIT_DAMAGE_SOURCES(unit)[i].player_index == player_index) ||
			(object_index != NONE && UNIT_DAMAGE_SOURCES(unit)[i].object_index == object_index))
		{
			s_unit_damage_source *source = &UNIT_DAMAGE_SOURCES(unit)[i];
			real total = amount;

			total += source->amount;
			source->amount = total;
			UNIT_DAMAGE_SOURCES(unit)[i].time = now;
			unit->unknown330[i] = flag;
			found = true;
		}
	}
	if (!found)
	{
		short slot = NONE;

		for (long i = 0; i < 4; i++)
		{
			if (UNIT_DAMAGE_SOURCES(unit)[i].time == NONE)
			{
				slot = (short)i;
				break;
			}
		}
		if (slot == NONE)
		{
			short largest = 0;

			for (long i = 1; i < 4; i++)
			{
				if (UNIT_DAMAGE_SOURCES(unit)[i].amount > UNIT_DAMAGE_SOURCES(unit)[largest].amount)
				{
					largest = (short)i;
				}
			}
			slot = NONE;
			for (long i = 0; i < 4; i++)
			{
				if (i != largest && (slot == NONE || (dword)UNIT_DAMAGE_SOURCES(unit)[i].time < (dword)UNIT_DAMAGE_SOURCES(unit)[slot].time))
				{
					slot = (short)i;
				}
			}
		}
		UNIT_DAMAGE_SOURCES(unit)[slot].object_index = object_index;
		UNIT_DAMAGE_SOURCES(unit)[slot].player_index = player_index;
		UNIT_DAMAGE_SOURCES(unit)[slot].amount = amount;
		UNIT_DAMAGE_SOURCES(unit)[slot].time = now;
		unit->unknown330[slot] = flag;
	}
	if (!notify || team == NONE || !function_1df560(unit->unknown138, team))
	{
		return;
	}
	long responsible_index = NONE;
	s_unit *responsible = 0;
	byte *player = datum_get_inlined(g_4e8c24, player_index);
	if (player && *(long *)(player + 0x2c) != NONE)
	{
		responsible_index = *(long *)(player + 0x2c);
		responsible = UNIT_GET(*(long *)(player + 0x2c));
	}
	if (!responsible)
	{
		responsible_index = object_index;
		responsible = (s_unit *)function_badc0(object_index, 3);
		if (!responsible)
		{
			return;
		}
	}
	if (type == 9)
	{
		if (responsible->unknown248 != NONE)
		{
			responsible_index = responsible->unknown248;
			responsible = UNIT_GET(responsible->unknown248);
		}
	}
	else if (responsible->unknown24c != NONE)
	{
		responsible_index = responsible->unknown24c;
		responsible = UNIT_GET(responsible->unknown24c);
	}
	if (TEST_FIELD_BIT(((s_unit_flags_10a_view *)&responsible->flags_10a)->bit2))
	{
		return;
	}
	long last = responsible->unknown2ec;
	now = g_510c54->game_time;
	if (last == NONE || (now - last) * g_510c54->rate > 4.0f)
	{
		responsible->unknown2ea = 0;
	}
	responsible->unknown2ea++;
	responsible->unknown2ec = now;
	if (g_4f55d0->active &&
		responsible->unknown2ea >= (short)(UNIT_GET(responsible_index)->unknown13c != NONE ? 5 : 3))
	{
		responsible->unknown2ea = 0;
	}
}

/* the unit's seat acceleration: from where its camera sits (0xd1080) this
   tick and the last, in the camera's frame scaled by its data; sets flag
   25 (and, seated, flag 26) when it passes the data's limits; then eases
   the smoothed copy at +0x288 toward it */
// @retail 0xce0c0
void function_ce0c0(long unit_index)
{
	s_unit *unit = unit_at_masked_index(unit_index);
	transform4x3f matrix;
	s_unit_camera_data camera;

	bit_vector_set(&unit->flags_134, 26, false);
	bit_vector_set(&unit->flags_134, 25, false);
	bool still = TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2) &&
		(unit_at_masked_index(unit_index)->type != 0 || !function_e4050(unit_index));
	if (function_d1080(unit_index, &matrix, &camera) && !still)
	{
		real ticks = g_510c54->field_2_3;
		vector3f velocity;
		vector3f acceleration;
		vector3f left;

		cross3f_staged(&matrix.up, &matrix.forward, &left);
		velocity.i = (matrix.position.x - unit->unknown270.x) * ticks;
		velocity.j = (matrix.position.y - unit->unknown270.y) * ticks;
		velocity.k = (matrix.position.z - unit->unknown270.z) * ticks;
		acceleration.i = (velocity.i - unit->unknown27c.i) * ticks;
		acceleration.j = (velocity.j - unit->unknown27c.j) * ticks;
		acceleration.k = (velocity.k - unit->unknown27c.k) * ticks;
		unit->unknown294.i = dot3f(&matrix.forward, &acceleration) * camera.unknown00;
		unit->unknown294.j = dot3f(&left, &acceleration) * camera.unknown04;
		unit->unknown294.k = dot3f(&matrix.up, &acceleration) * camera.unknown08;
		real forward = (real)fabs(unit->unknown294.i);
		real sideways = (real)fabs(unit->unknown294.j);
		real vertical = (real)fabs(unit->unknown294.k);
		real limit = camera.unknown0c;

		if (limit > 0.0f && (forward > limit || sideways > limit || vertical > limit))
		{
			bit_vector_set(&unit->flags_134, 25, true);
		}
		limit = camera.unknown10;
		if (limit > 0.0f && unit->parent_index != NONE && unit->parent_seat_index != NONE &&
			(g_4e6948->state != 2 || !function_138880()) &&
			(forward > limit || sideways > limit || vertical > limit))
		{
			bit_vector_set(&unit->flags_134, 26, true);
		}
		unit->unknown270 = matrix.position;
		unit->unknown27c = velocity;
	}
	else
	{
		unit->unknown294.k = 0.0f;
		unit->unknown294.j = 0.0f;
		unit->unknown294.i = 0.0f;
	}
	real keep = 0.7f;
	real take = 0.3f;
	if (TEST_FIELD_BIT((unit->flags_134 >> 28) & 1))
	{
		keep = 0.95f;
		take = 0.05f;
		bit_vector_set(&unit->flags_134, 28, false);
	}
	unit->unknown288.i = unit->unknown288.i * keep + unit->unknown294.i * take;
	unit->unknown288.j = take * unit->unknown294.j + keep * unit->unknown288.j;
	unit->unknown288.k = unit->unknown288.k * keep + unit->unknown294.k * take;
}

/* outside mode 4, damages around the unit by type: 1 and 2 use the
   definition's +0x180 and +0x188; otherwise its +0x168, an actor's
   override, a seat group's +0x170 or the weapon's +0x190; from its
   marker 0x500000a (type 0, when clear of the world), slightly above its
   center (type 1) or its center; then plays the impact sounds of what it
   hit */
// @retail 0xcf040
void function_cf040(long unit_index, short type)
{
	/* a volatile read of the mode: retail loads it into a register first, which sets the operand order
	   of the unit lookup below */
	char mode = *(char volatile *)&g_4e6948->mode;

	if (mode == 4)
	{
		return;
	}
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	long damage_definition_index;

	switch (type)
	{
	case 1:
		damage_definition_index = *(long *)(definition + 0x180);
		break;
	case 2:
		damage_definition_index = *(long *)(definition + 0x188);
		break;
	default:
		damage_definition_index = *(long *)(definition + 0x168);
		if (unit->actor_index != NONE)
		{
			byte *actor_definition = (byte *)function_1e5240(unit->actor_index);

			if (actor_definition && *(long *)(actor_definition + 0xc8) != NONE)
			{
				damage_definition_index = *(long *)(actor_definition + 0xc8);
			}
		}
		if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
		{
			s_unit_seat_definition *seat =
				&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit->parent_index)))[unit->parent_seat_index];

			if (TEST_FIELD_BIT((*(dword *)&seat->flags >> 11) & 1))
			{
				damage_definition_index = *(long *)(definition + 0x170);
			}
		}
		else
		{
			long weapon_index = function_cbd50(unit_index, UNIT_GET(unit_index)->current_weapon_index);

			if (weapon_index != NONE)
			{
				byte *local_67e06b = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));

				if (TEST_FIELD_BIT((*(dword *)(local_67e06b + 0x12c) >> 15) & 1))
				{
					damage_definition_index = *(long *)(local_67e06b + 0x190);
				}
			}
		}
		break;
	}
	if (damage_definition_index == NONE)
	{
		return;
	}
	vector3f forward = unit->forward;
	point3f point;
	switch (type)
	{
	case 1:
		function_b9dd0(unit_index, &point);
		point.x += g_4687b0->i * 0.1f;
		point.y += g_4687b0->j * 0.1f;
		point.z += g_4687b0->k * 0.1f;
		break;
	case 0:
	{
		s_object_marker marker;

		if (function_b8d30(unit_index, 0x500000a, &marker, 1, false) == 1)
		{
			byte collision[0x5c];

			point = marker.matrix.position;
			forward = marker.matrix.forward;
			*(short *)(collision + 0x24) = NONE;
			if (function_16a040(0x3480000f, &unit->unknown030, &point, unit_index, NONE,
				(s_collision_result_1697c0 *)collision))
			{
				point = unit->unknown030;
			}
			break;
		}
	}
	/* fall through */
	default:
		point = unit->unknown030;
		break;
	}
	{
		s_type_1e6529 damage;

		damage.material_index = NONE;
		function_d6660(&damage, damage_definition_index);
		object_get_root_location(unit_index, (s_location *)&damage.unknown1c);
		object_get_damage_owner(unit_index, &damage.owner);
		damage.position = point;
		damage.origin = unit->unknown030;
		*(byte *)&damage.unknown84 = 3;
		damage.unknown18 = unit_index;
		damage.cone_direction = forward;
		long hit_index = function_d6c80(&damage, unit_index);
		if (hit_index != NONE)
		{
			short material_type;

			damage.material_index = *function_1886d0(hit_index, &material_type);
		}
		long material_index = *(long *)&damage.material_index;
		if (function_188690((short)material_index))
		{
			function_ceee0(unit_index, damage_definition_index, (short)material_index, &point, g_4687b0);
		}
	}
}

// MiniMax G (11-13): no lower score (1): the whole difference is still the base and index order of the header read
// inside the weapon loop (ours [eax+edi+8], retail [edi+eax+8]). This round added five more spellings of that read -
// the unit in a local, the byte-pointer sum, the header array in a local, the index term first as an integer sum and
// the unit in a local together with the sum - and they score 303, 1, 1, 1 and 303; the three that keep the body at
// retail's length also keep our operand order. A 300 second permuter run over this body (2 tries) found nothing
// below 1, so the body stands.
// MiniMax A (11-12): no new form, no lower score (1): the single difference is still the operand order of the weapon index read (ours [eax + edi + 8] against retail [edi + eax + 8]); retail own matched 0xe9370 emits both orders for the same unit-header lookup, so the order is not a fixed convention of one macro and the spellings tried earlier do not reach it. The body stands.
/* whether the unit may enter the vehicle's seat: not its own, allowed for
   players or AIs (bits 16, 17), its weapons fit (bits 2, 3), the seat
   free, its seat group's partner friendly and willing, no enemy riding
   (outside state 2), and for players a clear line to the vehicle; also
   the seat's occupant and whether the seat is grouped */
// MiniMax H (11-12, third pass): no new form, no lower score (1); the only difference left in the whole body is the
// base and index order of the header read inside the weapon loop (ours [eax+edi+8], retail [edi+eax+8]). This round
// added the index term first and the pool first as integer sums, the byte-pointer sum, the element pointer with the
// unit field, the element pointer cast, the pool in a local at the top of the function (at both lookups and at the loop
// lookup only), the masked index in a local at the top, the element pointer in a local at the top, and the same
// spellings on the first lookup: the two integer sums make the compiler materialise the address (lea eax, [eax+edi+8])
// instead of folding it into the load, and the locals at the top of the function move the whole register plan (238 to
// 292), so the base and index order still follows neither the spelling nor the site.
// MiniMax H (11-12, second pass): no new form, no lower score (1); the only difference left in the whole body is the
// base and index order of the header read inside the weapon loop (ours [eax+edi+8], retail [edi+eax+8]). This round
// added eight more spellings of that read (the element pointer, the pool in a local, the byte-pointer arithmetic, the
// element pointer then the byte field, the slot in a local, the scaled slot in a local, the address of the field and
// the cast of the element address) and all of them compile to the same bytes. The one form that does come out with
// retail's operand order is a cast deref of the element pointer at offset zero, which reads as [edi+eax] - the same
// registers as retail but without the +8 displacement - so the address shape retail uses is not reachable from that
// source. Retail's own entry reads the same unit twice, once as [ecx+edx*4+8] with the index scaled by four and once
// as [edi+ecx+8], and only the read inside the loop differs here. The permuter started this round was stopped at the
// run's time limit without a result (the earlier run's 26 tries found nothing below 1).
// MiniMax H (11-12): no new form, no lower score (1); the only difference left in the whole body is the base and
// index order of the header read inside the weapon loop (ours [eax+edi+8], retail [edi+eax+8]). This round added the
// header-pointer form, the byte-pointer arithmetic, the offset in a local, the pool in a local and the word-index
// forms of that lookup, and every one of them compiles to the same bytes, so the order does not follow from the
// source, and the permuter (26 tries, 305 s, full image 924 s) found nothing below 1 either.
// MiniMax G (11-11): no new form, no lower score (1); the only difference left is the base and index order of the
// header read inside the weapon loop (ours [eax+edi+8], retail [edi+eax+8]); the lookup macro, the data-first
// pointer arithmetic, the same address spelled as an integer and the masked-index helper all compile to the same
// bytes here, and an annealed permuter run over this file (26 tries, best 1) left the score as it was, so the swap
// does not follow from the source and the body stands.
// MiniMax S (11-11): no lower score (1): the weapon loop's holder through unit_at_masked_index 1 (same code), the
// lookup as the pool data plus the masked index times twelve plus eight 1 (same code), the same address as an
// integer with the offset first and the data first 1, the data read untyped at the pool's 0x44 1 and the offset
// as a pointer cast 1 - every spelling keeps our [eax+edi+8] against retail's [edi+eax+8].
// MiniMax I (11-11): no new form, no lower score (1); the only difference left is the base and index
// order of the header lookup inside the weapon loop - retail addresses it with the byte offset as the
// base and the data pointer as the index, ours has them the other way round, and writing that lookup as
// the offset plus the data pointer compiles to the same bytes here.
// MiniMax E (11-11): no new form tried, no lower score (1): the only difference left is the base and
// index order of the weapon loop's header read (ours [eax+edi+8], retail [edi+eax+8]); the lookup macro,
// a data-first pointer arithmetic, a local header array, a plain integer sum and the masked-index
// helper all compile to the same bytes here, so the swap is not reachable from the source and the body
// stands.
// score 1 (was 274): what is left is the header lookup of the weapon loop, whose address operands are
// swapped (ours [eax+edi+8], retail's [edi+eax+8]). Fixes made: the team test takes the unit's team first,
// the bit 16 and 17 tests are two separate ifs, the seat flags are read in place, the group flag is
// declared after the result (274 -> 24), the root walk is a guarded do-while with its index declared
// first (24 -> 17), a blocked line of sight clears the result instead of returning false (17 -> 1).
// Tried for the lookup: unit_at_masked_index, the holder declared before the loop, a local header
// array, two lookups, a volatile read of the weapon index (all 1); a short hand counter 180, a char
// weapon slot 71. Before the tail change: a while or a for for the root walk 24, a copy of the vehicle
// index 24, an explicit offset for the header 24, the two tail conditions swapped 20.
// @coldspear (#104, 2026-10-30): the remaining difference is the order of an address's two registers on a byte array
// (ours [index + pointer], retail [pointer + index]), as in 0x4c450, 0x181cf0, 0x23600, 0x1988e0 and 0x194710. About 20
// forms tried on two of them (index spelling, declaration order, pointer formation, char and const types): none changed it.
// Round 82 (lane 11, fast): the weapon loop through the existing 0xcbd50 getter 1, with the
// holder kept for its slot argument 1; the first unit lookup through unit_at_masked_index 1, both lookups through it 1;
// the unit looked up before the vehicle 139. Diagnostic: 1-32 initialised unused locals at the top or before the loop
// leave the order unchanged (not symbol numbering here).
// m3 lane 16 r104 (fast, callers unchanged): the root walk's lookup through unit_at_masked_index 1; the rider lookup
// through it 1; both 1; the rider read through a named child index (also stored as the occupant) 1; the weapon loop's
// slot and weapon reads through the existing unit_weapon_view_get 1; the slot through unit_weapon_view_get with the
// weapon through 0xcbd50 1 (a variant of round 82's 0xcbd50 form). Lead: matched 0xe9370 shows both orders in one body,
// its lookups by the parameter as [data + offset] and its loop lookups by child_index as [offset + data], whether the
// offset is fresh or reused, so the order seems to follow the index variable the offset came from.
// Permuter (r104, 10 minutes, 53 variants): nothing below 1.
// Round 430: shared low-word slot 263; shared header offset 2; direct
// loop fields 1; hand while 1; outer hand declaration 1; const holder 1;
// empty-hand continue 1; typed seat-table field 1. No lower score.
// MiniMax G (11-13): no lower score (1): the weapon loop's header lookup still takes the data pointer as the address
// base where retail takes the byte offset; this round tried the offset-first integer sum, the element-pointer form and
// the pool copy inside the loop, and a standalone compile of the same lookup puts the offset in the base slot only when
// the base pointer is a global's own value, which this lookup is not (it reads the pool's data field).
// MiniMax B (11-13): no new form, no lower score (1): one instruction is left, the SIB byte of the weapon loop's header
// lookup, whose address operands retail orders the other way round; the same lookup written as a pointer to the header
// or through the masked index keeps our order.
// P455: Actual holder/input/occupant/group/result/iterator/unit/origin role names,
// eight isolated forms 1/1/1/1/1/1/1/1. NOT kept: no byte match;
// original executable body restored; frame and lone lookup operand exchange remain.
// MiniMax A (11-14): still no lower score (1): the index the loop looks the header up with, copied into
// a local of its own, and the same lookup written as a plain integer sum over the header array, both
// compile to exactly the same bytes as the form above; retail's straight-line lookup in this function
// orders the SIB with the offset as the base as we do, and its loop lookup keeps the pool data in the
// same register we use, so the base/index order is a register-allocation artefact rather than
// something the source can spell.
// MiniMax A (11-14): no new form, no lower score (1): the weapon loop's header lookup keeps our
// SIB order (base eax, index edi) where retail has the offset as the base; the lookup written as
// pointer arithmetic on the header array, with the offset term first, and with the header's own
// offset folded into the offset, all compile to the same bytes.
// MiniMax A (11-14): still no lower score (1): a full build gives the same single difference, so
// the reduced image is not the cause; the matched sibling 0xc85c0 in this file reaches retail's
// order (base esi, the offset, index eax, the pool data) for the same pool lookup, but it holds the
// pool pointer in a register while retail reloads the pool inside this loop; the lookup through the
// existing helper, through a named header pointer, with the offset added to the data as a long in
// both orders, with the offset as a pointer added to the data as a long, with a byte offset local,
// with the loop's hand variable as an int and with the hand variable declared outside the loop all
// compile to the same bytes; a 280 s permuter run (28 tries) stays at 1.
// @retail 0xc92c0
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *blocker_index, bool *grouped)
{
	s_unit *vehicle = UNIT_GET(vehicle_index);
	s_unit_seat_definition *seat = &UNIT_SEATS(UNIT_DEFINITION_GET(vehicle))[seat_index];
	s_unit *unit = UNIT_GET(unit_index);
	bool occupied = false;
	long occupant_index = NONE;
	bool result = true;
	bool group = TEST_FIELD_BIT((*(dword *)&seat->flags >> 11) & 1);
	long other_seat_index = group ? *(short *)((byte *)seat + 0x3e) : NONE;
	bool partner_friendly = false;
	bool partner_willing = false;
	bool enemy_aboard = false;

	if (unit_index == vehicle_index)
	{
		result = false;
	}
	if (unit->unknown13c != NONE && TEST_FIELD_BIT((vehicle->flags_134 >> 12) & 1))
	{
		result = false;
	}
	if (unit->unknown13c == NONE && TEST_FIELD_BIT((*(dword *)&seat->flags >> 17) & 1))
	{
		result = false;
	}
	if (unit->unknown13c != NONE && TEST_FIELD_BIT((*(dword *)&seat->flags >> 16) & 1))
	{
		result = false;
	}
	s_object_child_iterator iterator;
	function_d0620(&iterator, vehicle_index);
	while (function_d0690(&iterator))
	{
		s_unit *rider = UNIT_GET(iterator.child_index);

		if (iterator.child_short == seat_index && iterator.child_value == vehicle_index)
		{
			occupied = true;
			occupant_index = iterator.child_index;
		}
		else if (group && rider->parent_seat_index == other_seat_index && iterator.child_value == vehicle_index)
		{
			if (!function_1df560(unit->unknown138, rider->unknown138))
			{
				partner_friendly = false;
			}
			else
			{
				partner_friendly = true;
				partner_willing = unit->actor_index == NONE && function_c9200(vehicle_index, (short)other_seat_index);
			}
		}
		else if (rider->parent_seat_index != NONE && unit->unknown138 != NONE &&
			function_1df560(unit->unknown138, rider->unknown138))
		{
			enemy_aboard = true;
		}
	}
	for (long hand = 0; hand < 2; hand++)
	{
		s_unit *holder = UNIT_GET(unit_index);
		short index = (&holder->current_weapon_index)[hand];

		if (index != NONE)
		{
			long weapon_index = holder->weapon_object_indices[index];

			if (weapon_index != NONE &&
				((TEST_FIELD_BIT((*(dword *)&seat->flags >> 2) & 1) && function_101380(weapon_index)) || (TEST_FIELD_BIT((*(dword *)&seat->flags >> 3) & 1) && function_1013e0(weapon_index))))
			{
				result = false;
			}
		}
	}
	if (result)
	{
		if (!group)
		{
			if (occupied || (enemy_aboard && g_4e6948->state != 2))
			{
				result = false;
			}
		}
		else if (occupied)
		{
			result = false;
		}
		else if (other_seat_index == NONE)
		{
			if (TEST_FIELD_BIT((*(dword *)&seat->flags >> 14) & 1) && !enemy_aboard)
			{
				result = false;
			}
		}
		else if (!partner_friendly || partner_willing)
		{
			result = false;
		}
	}
	if (blocker_index)
	{
		*blocker_index = occupant_index;
	}
	if (grouped)
	{
		*grouped = group;
	}
	if (result && unit->unknown13c != NONE)
	{
		long index = vehicle_index;
		long root_index = NONE;
		if (vehicle_index != NONE)
		{
			do
			{
				root_index = index;
				index = UNIT_GET(index)->parent_index;
			} while (index != NONE);
		}
		point3f origin;
		point3f target;
		vector3f direction;
		byte collision[0x5c];

		*(short *)(collision + 0x24) = NONE;
		function_b9ef0(unit_index, &origin);
		function_b9ef0(root_index, &target);
		direction.i = target.x - origin.x;
		direction.j = target.y - origin.y;
		direction.k = target.z - origin.z;
		if (function_1697c0(0x80080d, &origin, &direction, unit_index, vehicle_index,
			(s_collision_result_1697c0 *)collision))
		{
			result = false;
		}
	}
	return result;
}

/* the unit object type's creation callback: resets the unit's fields,
   allocates its three state blocks (+0x33c, +0x340, +0x344; out of memory
   otherwise), takes its team from the creation data (in campaign, the
   definition's when unset), starts its animation and finds its two
   animation graph entries */
// MiniMax G (11-13): no new form tried this round, no lower score (59): the first header read
// takes the scaled offset as the address base where retail takes the data pointer, and the success
// arm's scratch registers stay swapped.
// MiniMax G (11-11): no new form, no lower score (59); the two header reads still take the byte offset as the base
// where retail takes the data pointer (ours [esi+ecx+8], retail [ecx+esi+8]), and the success arm keeps the copy
// source in EDX and the destination in ECX where retail has them the other way round; every spelling of the lookup
// tried earlier (a named offset local, the data expression inline, a named header pointer) compiles to the same
// bytes, so the body stands.
// MiniMax S (11-11): no lower score (59): the header read as the pool data plus the masked index times twelve plus
// eight through a named offset local 59 (same code), the same with the data expression inline 59 (same code) and the
// header through a named header pointer at that offset 59 (same code) - every spelling keeps our [esi+ecx+8] against
// retail's [ecx+esi+8], and the success arm keeps the copy source in edx and the destination in ecx where retail has
// ecx and edx.
// MiniMax I (11-11): no new form, no lower score (59); the two header reads still take the byte offset
// as the base where retail takes the data pointer, and the success arm's scratch registers (the g_4e0300
// reload, the +0x33e offset, the creation pointer and the two vector copies) are ours EDX, EAX and ECX
// where retail has EAX, ECX and EDX in the other roles.
// MiniMax C (11-10): score 59 (was 202): the out-of-memory arm sets the return value before its store while the body sits in the else arm, which gives retail's three return blocks and its 1040 bytes. NOT kept (coordinator, 11-11): the header read as the pool data plus the masked index times twelve plus eight through a named offset local (57, retail's [ecx + esi + 8] amode), a hand-expanded getter with a shared local, kept only with a match.
// MiniMax E (11-11): no new form tried, no lower score (59): the out-of-memory arm's form is retail's
// (1040 bytes, three return blocks); what is left is the success arm's scratch registers - ours EDX, EAX
// and ECX where retail has EAX, ECX and EDX for the g_4e0300 reload, the +0x33e offset, the creation
// pointer and the vector copies - and the two header reads' base and index order (ours [esi+ecx+8],
// retail [ecx+esi+8]); a 300 s permuter run over this body (21 tries) stays at 59, so it stands.
// score 59 (was 202; 253, 282 before that): the out-of-memory arm sets the return value before its store
// and the body sits in the else arm, which gives retail's three return blocks and its 1040 bytes; the
// logic agrees with retail statement by statement (re-walked in round 5 of #1049).
// Earlier score 202 (was 202; 253, 282 before that): the logic agrees with retail statement by statement (re-walked in round 5 of #1049).
// Round 5 of #1049: the four weapon slots are cleared with memset(.., 0xff, 16), which gives retail's lea/pointer fill
// (definition spilled first, EDX the pointer, ECX the value; 253 -> 202). What still differs: (1) retail's early
// `return false` after the animation check keeps its own epilogue (jne over `pop edi/esi/ebp; xor al,al; pop ebx;
// ret 0xc`) and so does the out-of-memory return, whose store sits after `xor al, al` (`mov ecx,[esp+0x1c]` before
// the pops, `mov [ecx],1` after the xor); ours merges the two into one tail (the check jumps over the store block;
// 16 bytes shorter, 1024) and every instruction after it shifts. Giving the first return a store of its own
// (`*out_of_memory = false`, NOT behaviour-preserving, not kept) makes the epilogues separate and gives 1040 bytes
// and score 172, so the merge is the rest of the score; (2) the three unit header reads are `[esi+ecx+8]` in
// ours for the first two, retail `[ecx+esi+8]` (base first; the third already matches); (3) the registers of the
// reads after the zero-fill loop (eax, ecx, edx in retail) and of the vector copies (retail copies through ECX and
// EDX in the other roles); (4) `push ebp` before `xorps`. Earlier fixes, in order: a failed-allocation flag
// (`failed`) in place of the || chain gives retail's layout (282 -> 266), the slot fill as a short-counter loop
// (262), the two graph lookups reading the definition's +0x38 afresh and keeping a long index (257), the
// damage-source loop through the UNIT_DAMAGE_SOURCES view (253), the memset (202). Tried for (1): `return
// *out_of_memory = true, false`, `return !failed`, a `result` local returned from both exits, the store as a
// separate local flag (all 253 before the memset, same merge). Tried for the fill, before the memset: the slot fill
// as a long or dword loop, a pointer loop, `*slot++`, four stores through a pointer local, an inline fill helper
// (281-299 or no change). Tried for (2): unit_at_masked_index, the header written as byte offset + array, base + 8
// + offset (no change); the animation check held in a bool 257; the amount stored first in the source loop 266.
// m3 lane 12 r106 (fast, plain where noted): (1) is solved when the out-of-memory arm sets the return value before
// its store: `bool result;`, the early `return false` (or the whole body single-exit with `result = false` in the
// check's arm), then `if (failed) { result = false; *out_of_memory = true; } else { ...; result = true; } return
// result;` gives retail's three return blocks exactly at 1040 bytes, 59 (plain 59). Not kept: with the store first
// it is 202 again (statement order by score). Left at 59: the success arm's scratch registers (ours edx, eax, ecx
// where retail has eax, ecx, edx: the g_4e0300 reload, the +0x33e offset, the creation pointer, the vector copies)
// and (2). Without the flag (the || chain in the if, or an else-if chain) the success arm gets retail's registers
// but the check and out-of-memory blocks move to the end 193; `if (allocated) {...} else {...}` 193; nested single
// exit with result false at entry 198; result started true and cleared in the arm 305 (stack slot), declared
// after the check and started false 305; `return !failed` 317; `result = a && b && c` 312; on the 59 form result
// declared first, no data local, an `allocated` flag, `!(a && b && c)`, `failed = false; if (...) failed = true;`
// 59 each (same code). A 600 s permuter run on the 59 form (--fast --anneal), 45 tries, none below 59.
// Typed unit-tag fields 59; source cursor 68; sequential/case allocation failure 326/301;
// model-entry cursors 76; complete shared setters 200; CRT vector copies 59; shared forward address 60.
// NOT kept: no gain; original body retained apart from the required typed tag fields.
// P405 fast: long/short/word source ordinals, success result at admission, post-allocation unit local, allocation result, both model counts, multi-use team; scores 60, 208, 208, 305, 59, 330, 59, 59.
// NOT kept: no confirmed permitted gain; original body and all callee interfaces restored.
// MiniMax A (11-14): still no lower score (59): the first header read takes the scaled offset as the address
// base where retail takes the data pointer. The pool pointer named in a local of its own rebuilds the head
// (the pool load moves to the top and is spilled, 287), the masked index named in a local for the first read
// alone gives the bytes we already have (59) and for both reads gives 116, so the base and index order of
// that read is not reachable from the source spelling.
// MiniMax A (11-12): no new form, no lower score (59): the first header read takes the scaled offset as the address base where
// retail takes the data pointer, and the success arm's scratch registers stay swapped; the early-return form of the
// out-of-memory block, tried here, scores 193 (59 with the failed flag kept).
// P432: chained long/zero clears, fixed source bound, source step at store,
// creation view scope, marker byte results, grenade count, model observation;
// fast scores 59/59/58/59/59/62/119/80.
// NOT kept: no permitted confirmed gain; original executable body restored.
// Fixed source termination 58 moves first to +0x1a2 (native header addressing,
// jne versus jb at the loop); NOT kept: gain one, below the retention threshold.
// MiniMax B (11-13): no new form, no lower score (59): the first header read still takes the scaled offset as the address
// base where retail takes the data pointer, and the success arm's scratch registers stay swapped; the early-return form
// of the out-of-memory block scores 193, and the fixed source termination moves the first difference but gains only one.
// P456: native extent/unsigned source loop and three exits retained. Wide/byte failure carriers 59/59;
// wide result 166; failure/ordinal/paired marker-result declaration lifetimes 59/59/59; nested
// grenade/team admissions 59/59. NOT kept: ties or regression; header SIB and success scratch rotation
// remain.
// @retail 0xc4410
bool __stdcall function_c4410(long unit_index, void const *creation, bool *out_of_memory)
{
	byte const *data = (byte const *)creation;
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_definition *definition = (s_unit_definition *)UNIT_DEFINITION_GET(unit);

	unit->unknown238 = NONE;
	memset(unit->weapon_object_indices, 0xff, sizeof(unit->weapon_object_indices));
	unit->unknown214 = 0;
	unit->unknown210 = 0;
	unit->next_weapon_index = NONE;
	unit->current_weapon_index = NONE;
	unit->unknown217 = NONE;
	unit->unknown216 = NONE;
	unit->current_grenade_index = NONE;
	unit->next_grenade_index = NONE;
	unit->unknown240 = NONE;
	unit->unknown241 = NONE;
	unit->unknown13c = NONE;
	unit->unknown140 = NONE;
	unit->actor_index = NONE;
	unit->unknown130 = NONE;
	unit->parent_seat_index = NONE;
	unit->unknown248 = NONE;
	unit->unknown24c = NONE;
	unit->unknown2a0 = NONE;
	unit->unknown2a4 = NONE;
	unit->unknown2a8 = NONE;
	unit->unknown2ac = NONE;
	unit->unknown1ec = 0;
	unit->unknown2c8 = 0;
	unit->unknown2ca = 0;
	unit->unknown2cc = 0.0f;
	unit->unknown2d0 = NONE;
	unit->unknown2e0 = NONE;
	unit->unknown250 = NONE;
	unit->unknown254 = NONE;
	unit->unknown258 = 0;
	unit->unknown25a = NONE;
	unit->unknown1f4 = 0;
	unit->unknown2d4 = NONE;
	unit->unknown2ea = 0;
	unit->unknown2ec = NONE;
	unit->unknown1f6 = NONE;
	unit->unknown1f7 = NONE;
	unit->unknown334 = 0.0f;
	unit->unknown338 = 0.0f;
	unit_state_tail_clear(&unit->unknown1c8);
	for (dword i = 0; i < 4; i++)
	{
		UNIT_DAMAGE_SOURCES(unit)[i].time = NONE;
		UNIT_DAMAGE_SOURCES(unit)[i].amount = 0.0f;
		UNIT_DAMAGE_SOURCES(unit)[i].object_index = NONE;
		UNIT_DAMAGE_SOURCES(unit)[i].player_index = NONE;
		unit->unknown330[i] = 0;
	}
	if (UNIT_GET(unit_index)->animation_offset == NONE)
	{
		return false;
	}
	bool result;
	bool failed = !function_bc380(unit_index, 0x33c, 0x124, 0) || !function_bc380(unit_index, 0x340, 0x58, 0) ||
		!function_bc380(unit_index, 0x344, 0x44, 0);
	if (failed)
	{
		result = false;
		*out_of_memory = true;
	}
	else
	{
		unit = UNIT_GET(unit_index);
		byte *state = (byte *)unit + unit->unknown346;
		*(long *)(state + 0x10) = NONE;
		*(short *)(state + 0x14) = NONE;
		*(long *)(state + 0x18) = NONE;
		*(long *)(state + 0x2c) = NONE;
		*(long *)((byte *)unit + unit->unknown33e + 0x2c) = NONE;
		function_10f260(unit_index);
		byte *weapon_state = (byte *)unit + unit->unknown342;
		*(long *)weapon_state = NONE;
		*(long *)(weapon_state + 0x44) = NONE;
		function_114ec0(unit_index, *(long *)(data + 0xc));
		*(vector3f *)&unit->unknown18c = unit->forward;
		unit->unknown180 = unit->forward;
		unit->unknown168 = unit->forward;
		unit->unknown15c = unit->forward;
		unit->unknown150 = unit->forward;
		unit->unknown14c = 0x6000086;
		short grenade_type = definition->unit.grenade_type;
		if (grenade_type >= 0 && grenade_type < 2 && definition->unit.grenade_count >= 0)
		{
			unit->grenade_counts[grenade_type] = (char)definition->unit.grenade_count;
		}
		unit->object_flags |= 0x1000;
		unit->unknown138 = *(short *)(data + 0x64);
		if (g_4e6948->state == 1 && (unit->unknown138 == 0 || unit->unknown138 == NONE))
		{
			unit->unknown138 = definition->unit.default_team;
		}
		function_10f430(unit_index, 0x7000001, 0x7000101, 0x7000101, 0x7000001, 0.0f, false, 2);
		long first_index = NONE;
		if (definition->object.model_tag_index != NONE)
		{
			byte *graph = g_4e3b44[definition->object.model_tag_index & 0xffff].bytes;

			for (long i = 0; i < *(long *)(graph + 0x78); i++)
			{
				if (*(long *)(*(byte **)(graph + 0x7c) + i * 0x5c) == 0x80001a2)
				{
					first_index = i;
					break;
				}
			}
		}
		unit->unknown1f6 = (char)first_index;
		long second_index = NONE;
		if (definition->object.model_tag_index != NONE)
		{
			byte *graph = g_4e3b44[definition->object.model_tag_index & 0xffff].bytes;

			for (long i = 0; i < *(long *)(graph + 0x78); i++)
			{
				if (*(long *)(*(byte **)(graph + 0x7c) + i * 0x5c) == 0x90001a3)
				{
					second_index = i;
					break;
				}
			}
		}
		unit->unknown1f7 = (char)second_index;
		result = true;
	}
	return result;
}

/* gives the unit a weapon (unless it is held or flagged, or the unit may
   not ready it) by mode: 0 and 1 into a free slot (1 readies it), 2 after
   dropping all, 3 and 4 into the first hand after lowering the second
   (4 also changing weapons), 5 to 7 into the second hand beside a
   matching first (6 lowering it first, 7 readying it); tells the player
   code */
// MiniMax A (11-14): score 272 (was 280): the two weapon flag tests and the readiness flag now live in the first byte of the
// four-byte hand-modes buffer that 0xcd7b0 fills, so the flag values are stored into that byte as retail stores them
// (`and cl,1` into the buffer) and the buffer is four bytes wide, as 0xcd7b0 writes four; the mode 3/4 buffer is four bytes
// too, which puts the unit pointer at 0x14 and the weapon pointer at 0x18 as in retail. What still differs: retail keeps the
// readiness byte at 0x13 and the buffer at 0x64, ours keeps them at 0x64 and 0x10, and the branch targets and the tail differ.
// MiniMax A (11-14): score 280 (was 300): the hand index is set to 0 and then to 1 in the mode 5-7 test, which gives retail's
// register homes (weapon EDI, mode BX) and retail's length (1052 bytes); what still differs is retail's flag-7 bit materialised
// with `and cl,1` and stored in the ready byte, retail's frame order (unit 0x14, weapon 0x18, hands 0x1c), the mode-2 path's
// `xor esi,esi` and the duplicated epilogue.
// score 280 (was 333): the single exit with `default: __assume(0)` gives retail's stack convention
// score 300 (was 333; 280 only with the unkept `hand = 0; if (...) hand = 1;` form below): the single exit with `default: __assume(0)` gives retail's stack convention
// (all three arguments pushed), which matches 0xc4820, 0xccab0, 0xcce00 and 0x19ef40; current frame 0x50
// agrees (earlier 0x54); weapon/mode in EBX/EDI versus EDI/EBX,
// and the two weapon flag tests are not stored in the ready byte as in retail.
// Object flag 7 read as a field bit (TEST_FIELD_BIT) gives retail's load-and-shift (294 -> 279).
// Retail's ready byte and the main hands array share one slot (unit_index's stack home, which also takes the
// two flag tests' stores and the slot index spill): `ready` written as hands[0] with the flag tests stored into it
// gives frame 0x50, the same size and 300 (293 with hands[1] cleared), but LTCG then passes the unit in ESI (ret 8),
// which breaks the callers, so it is not kept; separately named hands and ready stay 333.
// Two request homes 313; result first 313; shared exit 301; secondary modes first 294; branch-local requests 314.
// With a trial `standard` marker (removed): plain body 320; single exit 320; modes 3/4 tested first 308;
// `default: __assume(0)` 302; blocked local for the flag tests 320; one ready check after the mode
// chain 329. Without it: either request renamed 333; hands declared at the top 333; hands in a bare
// block 332 (frame 0x50).
// Round 6 of #1049 (332 -> 300): most of the 32 extra bytes were the hand index: with `hand = ...` assigned in the
// mode-2 branch and the mode 3-7 branch, LTCG threaded the mode tests and duplicated the chain (and put mode/weapon in
// EDI/EBX, 333). One `hand = mode == 5 || mode == 6 || mode == 7;` after the mode branches gives 300 (1056 bytes).
// `hand = 0; if (mode == 5 || mode == 6 || mode == 7) hand = 1;` there gives retail's register homes (weapon EDI,
// mode BX) and size (280, 1052 bytes) but is not kept: a value set before a test, without a match. The three request locals are block-
// scoped (modes 4, 6, 7 then share retail's [esp+0x40]; the case-1 one is [esp+0x20]), as is retail's frame.
// Still differing: retail threads only mode 2 (`xor esi, esi; jmp` past the chain; `hand = 0` in the mode-2 branch
// does it but then the chain is duplicated and mode/weapon swap registers: 318/312), retail's mode-6 request joins
// mode 4's shared `test al, al; mov [ready], al; je done` (ours has its own, so the done block is laid out early
// and the epilogue duplicated, the final `result = true` stored at [esp+0xb]); the frame order (ours: hands at
// 0x14, unit 0x18, weapon 0x1c; retail: unit 0x14, weapon 0x18, hands 0x1c) and retail's two flag tests stored to
// the shared ready/hands slot (ready as a scalar drops them; `hands[0]` as ready keeps them but takes the param
// home for `result`, 274, 1056 bytes: not kept). Tried: ternary `hand` after the branches (300, as the plain form); `second_hands`
// or `ready_request` declared at the top (no change); hand chain inside the else (318).
// Walk against retail (b4M, 2026-11-06, statement by statement, 0xcd0c0-0xcd4b8): no logic difference found. Checked: the
// object lookups (0xcd0d3-0xcd0f2), the flag-7 and +0x12c tests (0xcd0f6-0xcd123), cd6a0 args (0xcd129-0xcd12e), mode 2
// (0xcd14b), the 3/4 and 5-7 chains (0xcd158-0xcd17c), mode 6's request type 0x13 (0xcd1d1-0xcd206), the 3/4 second hand
// block (0xcd210-0xcd27b), mode 4's three paths (0xcd280-0xcd2f6), the hand index (0xcd300-0xcd316), cd7b0/hands[hand]
// (0xcd327-0xcd344), the slot test and parent_index (+0x14) detach (0xcd34c-0xcd374), cea70 (0xcd37c-0xcd382), the
// no-range-check jump table (0xcd38f, 0xcd4bc) with its cases (0xcd396-0xcd47b: types 8/0x12, +0x216/+0x217 stores, the
// mode 1 slot compare at 0xcd3bd), the +0x13c test (0xcd47f-0xcd48a) and a8a30 for modes other than 1 and 7 (0xcd48f-0xcd4a3).
// Every field, callee, argument, test direction and mode set equals retail's; only frame order, the weapon/mode registers and
// the duplicated epilogue differ. The `(char)` casts on the two +0x216/+0x217 stores were dropped (same bytes). `weapon` is an
// item, not a unit: s_unit's +0x12c is actor_index, so the flag read at +0x12c is cast (items.cpp calls it flags_12c).
// Also tried: hand as a `switch (mode)` 283 (1052 bytes, but compiles the 5-7 test as a range compare, not retail's cmp
// chain: not kept), `if/else` and `?:` for hand 300.
// P457: main/secondary hand-output names and unit/weapon receiver names 300 each;
// byte/wide function result 316/318; byte readiness and existing bit predicate 300/300.
// NOT kept: ties or regressions. Native frame 0x50 confirmed; saved-register schedule,
// object/hand homes, dead flag stores and shared request completion still differ.
// MiniMax A (11-14): no new form, no lower score (272): the readiness flag and the four-byte hand
// buffer still take each other's frame slots (ours: the flag at 0x64 and the buffer at 0x10; retail:
// the flag at 0x13 and the buffer at 0x64), and swapping the two declarations changes nothing.
// NOT kept (coordinator screen, MiniMax A 11-14): a goto the function on main does not have (kept only with a byte match): `goto done;`; the body is main's again.
// @retail 0xcd0c0
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode)
{
	s_unit *weapon = UNIT_GET(weapon_index);
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;
	bool ready;
	short hand;

	if (TEST_FIELD_BIT((weapon->object_flags >> 7) & 1) || *((byte *)weapon + 0x12c) & 1 ||
		!function_cd6a0(unit_index, NONE, weapon_index))
	{
		goto done;
	}
	ready = true;
	if (mode == 2)
	{
		function_ccff0(unit_index);
	}
	else
	{
		if (mode == 3 || mode == 4)
		{
			long second_index = function_cbd50(unit_index, UNIT_GET(unit_index)->next_weapon_index);

			if (second_index != NONE && (!function_101640(weapon_index) || !function_101640(second_index)))
			{
				bool hands[2];

				if (!function_cd7b0(unit_index, second_index, hands) || !hands[1] || !function_cd4e0(unit_index, 1, true))
				{
					function_e68c0(0x13, unit_index);
				}
			}
			if (mode == 4)
			{
				if (function_cbe60(unit_index) != NONE)
				{
					s_unit_request request;

					memset(&request, 0, sizeof(request));
					request.type = 9;
					request.type17.unknown4 = true;
					ready = function_e6900(unit_index, &request);
				}
				else if (second_index != NONE)
				{
					ready = function_cd4e0(unit_index, 0, true);
				}
				else
				{
					ready = function_e68c0(9, unit_index);
				}
				if (!ready)
				{
					goto done;
				}
			}
		}
		else if (mode == 5 || mode == 6 || mode == 7)
		{
			long first_index = function_cbd50(unit_index, UNIT_GET(unit_index)->current_weapon_index);

			if (first_index == NONE || !function_101640(first_index) || !function_101640(weapon_index))
			{
				goto done;
			}
			ready = true;
			if (mode == 6)
			{
				s_unit_request request;

				memset(&request, 0, sizeof(request));
				request.type = 0x13;
				request.type17.unknown4 = true;
				ready = function_e6900(unit_index, &request);
				if (!ready)
				{
					goto done;
				}
			}
		}
	}
	hand = mode == 5 || mode == 6 || mode == 7;
	if (!ready)
	{
		goto done;
	}
	bool hands[2];
	if (!function_cd7b0(unit_index, weapon_index, hands) || !hands[hand])
	{
		goto done;
	}
	short slot_index = function_cd620(unit_index);
	if (slot_index == NONE)
	{
		goto done;
	}
	if (weapon->parent_index != NONE)
	{
		function_b9a90(weapon_index);
	}
	function_cea70(NONE, slot_index, weapon_index, unit_index);
	switch (mode)
	{
	case 0:
	case 1:
		unit->unknown216 = function_cdeb0(unit_index, NONE, unit->current_weapon_index, 0);
		if (mode == 1 && unit->unknown216 == slot_index)
		{
			s_unit_request request;

			memset(&request, 0, sizeof(request));
			request.type = 8;
			request.type17.unknown4 = true;
			request.type17.unknown5 = true;
			function_e6900(unit_index, &request);
		}
		break;
	case 2:
	case 3:
	case 4:
		unit->unknown216 = slot_index;
		function_e68c0(8, unit_index);
		break;
	case 5:
	case 6:
		unit->unknown217 = slot_index;
		function_e68c0(0x12, unit_index);
		break;
	case 7:
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		unit->unknown217 = slot_index;
		request.type = 0x12;
		request.type17.unknown4 = true;
		request.type17.unknown5 = true;
		function_e6900(unit_index, &request);
		break;
	default:
		__assume(0);
	}
	if (unit->unknown13c != NONE)
	{
		function_1520f0(unit->unknown13c);
	}
	if (mode != 1 && mode != 7)
	{
		function_a8a30(unit_index, weapon->definition_index);
	}
	result = true;
done:
	return result;
}

/* the unit's death by a damage type, from a direction: picks the side it
   fell toward (front, back, left, right), plays its death (0x10ff40) and,
   from type 3, sets its ragdoll timer +0x1f5, its flags and (when the
   death had no animation) its deadness; a free biped turns to face it */
// MiniMax A (11-14): no new form, no lower score (250): the zero constant rides in xmm0 here and in xmm2 there, and
// retail's scale block multiplies the x component in place and the y component into a fresh register, which no single
// expression form gives.
// MiniMax B (11-14), NOT kept (coordinator: the 250 -> 209 rests on the one-use `bool bit9` test, a form kept only
// with a byte match; B's body without it measures 252, and without the `__assume(0)` default too 247):
// lower score (209, was 250): the face switch's `default: __assume(0)` drops the jump table's range
// check (retail has none) and realigns the two instruction streams; the nested `length > 0.0f` test (retail defers the
// jbe past the multiplies) and the bit-9 bool local (retail's shr/and form) are kept. Still differing: the y scale
// multiply - retail keeps the scale's register live and does `movss xmm3,[y]; mulss xmm3,xmm0`, ours folds the scale
// into the multiply (`mulss xmm0,[y]`) and sinks both stores, which shifts every later instruction. The dword facing
// copy (`facing = *(point2f const *)&unit->forward`) gives retail's four dword moves but leaves the body 12 bytes short
// (215), and the side chain with `side = 2` hoisted into a register also scores 215. This round: both components with
// the scale on the left, both with the scale on the right, the two mixed compound assignments and the dword copy
// together with the left-hand spelling all fold the same way (209); `/fp:precise` changes nothing, `/GL-` is worse
// (298), and the permuter (300 s, 7 tries) finds nothing below the 209 baseline. Later this round: the x multiply
// already matches retail (`movaps xmm3,xmm0; mulss xmm3,[x]`) and only the y multiply differs, so the scale's register
// has to stay live across it; the y multiply first (210), the y value through a temporary, both components through a
// scaled copy (307), the compound assignments, the scale on the left of both and the multiplies inside the length test
// (210) all fold the same way. The scale's register is therefore still live in retail's tree, and no third use of it
// appears in retail's bytes, so the ingredient is still unidentified.
// MiniMax B (11-13): no new form, no lower score (250): retail's scale block multiplies the x component in place and the y
// component into a fresh register, which no single expression form gives; the facing copy, the split length test, the
// __assume and the duration forms were measured again on the committed convention and none of them reached the 250
// baseline, and the permuter (300 s, 7 tries) found 243 against a 244 reduced-image baseline, below the eight-point bar,
// so the body is the committed one.
// B's form, not kept: score 209 (was 250): Frame 0x24 agrees; retail takes the direction in ECX (round 82: now ours too) and keeps
// the unit pointer in EBX, the definition on the stack. Round 5 of #1049: retail's epilogue clears AL (`xor al,
// al` on every path, including the early return), so the function returns bool false (256 -> 254). The
// convention is coupled to 0xd6800's: declaring this function after 0xbd020 (forward declarations) WITHOUT
// __stdcall gives retail's ECX-only convention and a head that agrees to the scale step (250), but 0xd6800
// then takes ECX/EDX and the MATCHED 0xd6bc0 goes to 63 (and 0xc42e0 5 -> 32), so that is not kept. On that
// convention a fresh walk found these retail forms (each measured there, none kept without a match):
// (a) the scale block runs before the `length > 0.0f` test (`if (!(0.0001f > fabs(length))) { scale; toward
// *= scale; if (length > 0.0f && normalize2d(&facing) > 0.0f) ...`; the test's comiss is scheduled above the
// divss in the same block), 255; `facing = *(point2f const *)&unit->forward` (dword copy as retail) and
// `default: __assume(0)` in the face switch (retail's jump table has no range check); (b) retail stores each
// definition-flag bool to a dead frame byte exactly when the test is a __forceinline helper returning a one-
// byte struct (`s_flag { bool value; }`, as 0x1c32d0's hkBool does): both bit tests then agree and the
// hoisted `mov ecx, 2` appears; (c) `bool front = side == 3;` gives retail's sete/test/je; (d) the weapon
// timing's `!(time >= 0.0f)` with the 0x10f690 output in a separate local (retail never stores the first
// time); (e) the ticks stored to the field, then re-stored 1 when `<= 1` (store, `cmp al, 1`, `jg`).
// Still differing on that convention: the y multiply of the scale (retail loads y into xmm3, ours multiplies
// in xmm0); the state pointer's spill slot (+0x14 retail, +0x1c ours); retail keeps the first weapon index in
// a frame slot and looks the unit table up again for the second (ours merges the two lookups; a pool
// pointer local, `holder` re-read: no change); `or byte [mem], imm` flag updates (ours load/or/store through
// AL); the tail loading ESI = unit_index at the join. On the committed (stdcall) convention these forms
// score worse (facing copy 310, scale split 263, __assume 271, duration 270), so they are not applied.
// Round 82 (lane 11): the declaration moved after 0xbd020's (still __stdcall) gives retail's convention, the
// direction in ECX and the rest pushed: 254 -> 250 (aligned 198 -> 96), and matches 0xc42e0.
// Round 151: __stdcall in place of the file's /Gr
// fastcall (which passed the unit in EDX) 258, kept; the unit read through its address 258; the
// direction declared last/second 255/256 (no retail evidence, not kept). Round 89: 0x11cc90 takes its
// operands as retail passes them (toward in EDI, facing in ESI) 258; both definition tests through
// TEST_FIELD_BIT 278, the bit-9 one alone 285 (retail stores each test's bool to a dead stack byte);
// angle/turn/flag declared first 256; `!(time >= 0.0f)` 258 (gives retail's comiss order).
// Round 3: the two weapon indices as short locals assigned under `if` (retail compares them as words, movsx
// dx/cx, and stores NONE to the first one's slot before the test) 256, ternaries on the char fields 258;
// a separate unit lookup for each index (retail re-reads the table for the second one; ours merges the two) 256;
// the bit-9 test through a bool local 285 (no store appears); the direction copied field by field 258.
// P435: typed tag flags 250 (plain 250); CRT direction/facing copies 250/253;
// immutable angle magnitude, decision role names, entry state workspace, entry facing
// output and one-completion animation admission 250; native case order 3,0,1,2 262.
// NOT kept: no tuning gain; native zero/flag storage, state slot and facing tail remain.
// P457: shared magnitude/bit-vector predicates 254/258; existing state-byte setter 250;
// successful-turn/final-identity library point copies 250/250; point-role names,
// entry side workspace and byte turn result 250 each. NOT kept: ties or regressions;
// native zero SIMD carrier, dead flag stores and state home remain.
// @retail 0xc98a0
bool __stdcall function_c98a0(point2f const *direction, long unit_index, long type, short value)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_definition *definition = (s_unit_definition *)UNIT_DEFINITION_GET(unit);
	real angle = 0.0f;
	bool turn = false;
	bool flag = false;
	point2f facing;
	point2f toward;

	if (direction)
	{
		toward = *direction;
		facing.x = unit->forward.i;
		facing.y = unit->forward.j;
		real length = (real)sqrt(toward.x * toward.x + toward.y * toward.y);

		if (!(0.0001f > (real)fabs(length)) && length > 0.0f)
		{
			real scale = 1.0f / length;

			toward.x = scale * toward.x;
			toward.y = toward.y * scale;
			if (normalize2d(&facing) > 0.0f)
			{
				angle = function_11cc90((vector2f const *)&toward, (vector2f const *)&facing);
				if (!((definition->unit.flags >> 9) & 1) && unit->type == 0 && unit->parent_index == NONE)
				{
					if (function_e70b0(unit_index).value == NONE)
					{
						facing = toward;
						turn = true;
					}
				}
			}
		}
	}
	short side;
	real magnitude = (real)fabs(angle);
	if (0.7853982f > magnitude)
	{
		side = 3;
	}
	else if (magnitude > 2.159845f)
	{
		side = 0;
	}
	else
	{
		side = angle > 0.0f ? 1 : 2;
	}
	if (g_4e6948->state == 2 && value == 2)
	{
		if (type == 7)
		{
			side = 1;
		}
	}
	else if (value == NONE)
	{
		value = 0;
	}
	bool animated = function_10ff40(unit_index, type, side, value, &flag, &side, &value);
	if (!animated && type < 3)
	{
		return false;
	}
	byte *state = (byte *)unit + unit->unknown33e;
	if (flag)
	{
		function_ba350(unit_index, 0.1335f);
		*state |= 1;
	}
	if (type >= 3)
	{
		s_unit *holder = UNIT_GET(unit_index);
		long first_index = NONE;
		long second_index = NONE;
		short current_weapon = holder->current_weapon_index;
		short next_weapon = holder->next_weapon_index;

		if (current_weapon != NONE)
		{
			first_index = holder->weapon_object_indices[current_weapon];
		}
		if (next_weapon != NONE)
		{
			second_index = holder->weapon_object_indices[next_weapon];
		}

		if (flag && type != 6 && (first_index == NONE || !function_101240(first_index)) &&
			(second_index == NONE || !function_101240(second_index)))
		{
			real time = function_10f7f0(unit_index);

			if (time < 0.0f)
			{
				function_10f690(unit_index, &time);
				time = function_259d0(&g_4e7408->unknown0, 0, 0, time * 0.25f, time * 0.75f);
			}
			char ticks = (char)unit_round(g_510c54->field_2_3 * time);
			unit->unknown1f5 = ticks > 1 ? ticks : 1;
		}
		else
		{
			unit->unknown1f5 = 0;
		}
		if (side == 3)
		{
			*state |= 8;
		}
		else
		{
			*state &= ~8;
		}
		if (!animated)
		{
			*((byte *)unit + unit->animation_offset + 0x6c) |= 1;
			*state |= 4;
			if ((definition->unit.flags >> 1) & 1 && !function_a7670(unit_index))
			{
				function_d6bc0(unit_index);
			}
		}
	}
	if (turn)
	{
		point2f face;

		switch (side)
		{
		case 0:
			face.x = 0.0f - facing.x;
			face.y = 0.0f - facing.y;
			break;
		case 1:
			face.x = 0.0f - facing.y;
			face.y = facing.x;
			break;
		case 2:
			face.x = facing.y;
			face.y = 0.0f - facing.x;
			break;
		case 3:
			face = facing;
			break;
		}
		function_edfa0(unit_index, &face);
	}
	return false;
}

/* sends a unit request straight to its handler and tells the player's
   controller how it went (as 0xce520 does for request 9) */
static __forceinline void unit_perform_request(long unit_index, long type)
{
	s_unit_request request;
	s_unit_request_definition *definition = g_4677c8[type];

	memset(&request, 0, sizeof(request));
	request.type = type;
	function_b7360(unit_index);
	bool result = definition->perform(unit_index, &request);
	if (unit_index != NONE)
	{
		long player_index = UNIT_GET(unit_index)->unknown13c;

		if (player_index != NONE)
		{
			short controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

			if (controller != NONE)
			{
				function_1e6980((s_time_entry *)((byte *)g_51e9c0 + controller * 0x1b0 + 0x150), (short)request.type,
					result);
			}
		}
	}
}

/* the unit dies: the biped's death (0xe4a20), its player and actor let
   go, its weapons and grenades dropped, it leaves its seat, its requests
   end (unless a ragdoll timer runs, when forced) and its actions stop */
// score 224 (was 243): Round 3 of #1049. Same size as retail (1175 bytes) and the same 312 instructions in the same
// order; only registers, two address folds and the hand loop's second argument differ. The bit-21 clear is
// bit_vector_set (retail loads the flags, adds 0x134 to the pointer, ANDs, stores: +15 bytes, 243 -> 229); each
// request handler is read before the request is cleared (retail keeps it in EBX across 0xb7360: 226); the bit-1
// clear as bit_vector_set puts the flag load after the 1.0f load as retail (224). Still differs: (1) retail
// passes 0x100430's value from a stack word (mov word [esp+0x44],0 in the dead
// `knocked` slot, reloaded as a dword; the callee uses only its low word, so behaviour is the same) where ours
// pushes 0; (2) retail adds the base explicitly before the 16-bit loads at +0xc (weapon state) and +0x28
// (controller, twice): `add eax, base; mov ax, [eax+d]`, ours folds them into the address; (3) the flag-clear
// temp is EBX (ours EDX); the request zeroing uses EAX in the 0x1e block and its first/second copies (ours
// ECX/EDX); the interrupted-callback state pointer is EBP, value EBX (ours the reverse). Callers: 0xc42e0 stays 5
// (blocked by 0xd6800's convention, not by this), 0xca0b0 stays 344.
// Tried: inline request helper 628; forced expansion 323; two-hand do/while 323; entry request workspace 324;
// zero aggregate 327; word local, volatile word local (219 but movzx and a size of 1176, not kept), word_flags_set
// local for the value 229 (folded to push 0); player pointer local, data base local, forceinline player helper
// for the controller read, weapon block pointer local 226/224 (folded, no change).
// @retail 0xcaa60
void __stdcall function_caa60(long unit_index, long object_index, point3f const *point, bool knocked, bool force)
{
	s_unit *unit = UNIT_GET(unit_index);

	if ((1 << unit->type) & 1)
	{
		function_e4a20(unit_index, object_index, point, knocked);
	}
	if (unit->unknown13c != NONE)
	{
		unit->flags_134 &= ~0x20000000;
		unit->unknown264 = 0.0f;
		unit->unknown2bc = 0;
		unit->unknown2be = 0;
	}
	function_bb950(unit_index, true, g_510c54->field_2_3 * 10);
	long player_index = unit->unknown13c;
	if (player_index != NONE)
	{
		byte *player = (byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c;

		*(long *)(player + 0x30) = *(long *)(player + 0x2c);
		/* the player lets go of its unit: retail passes NONE in ebx */
		function_14cad0(player_index, NONE);
		function_152340();
	}
	if (unit->unknown130 != NONE)
	{
		function_a7bc0(unit_index);
	}
	long actor_index = unit->actor_index;
	if (actor_index != NONE)
	{
		unit->unknown25a = *(short *)((byte *)g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 0x30);
		function_1e2a90(actor_index);
		unit->actor_index = NONE;
	}
	unit->unknown2e0 = g_510c54->game_time;
	bit_vector_set(&unit->flags_134, 1, false);
	function_d0e00(unit_index, 1.0f);
	unit->unknown148 = 0;
	s_unit *again = UNIT_GET(unit_index);
	short weapon_state = *(short *)((byte *)again + again->unknown342 + 0xc);
	if (weapon_state > 0 && weapon_state < 0xf)
	{
		function_114e80(unit_index);
	}
	for (long hand = 0; hand < 2; hand++)
	{
		if ((&unit->current_weapon_index)[hand] != NONE)
		{
			s_unit *holder = UNIT_GET(unit_index);
			short index = (&holder->current_weapon_index)[hand];

			function_100430(index != NONE ? holder->weapon_object_indices[index] : NONE, 0, 0.0f);
		}
	}
	bit_vector_set(&UNIT_GET(unit_index)->flags_134, 21, false);
	if (unit->parent_index != NONE)
	{
		if (unit->parent_seat_index != NONE)
		{
			s_unit_request request;

			memset(&request, 0, sizeof(request));
			request.type = 0x1e;
			function_e6900(unit_index, &request);
		}
		else
		{
			function_cc590(unit_index);
		}
	}
	function_cece0(unit_index);
	s_unit *holder = UNIT_GET(unit_index);
	if (holder->unknown238 != NONE && g_4e6948->mode != 4)
	{
		function_ce470(0, unit_index, holder->unknown238);
		holder->unknown238 = NONE;
	}
	function_ceb30(unit_index);
	if (!unit->unknown1f5 || force)
	{
		unit_perform_request(unit_index, 0x13);
		unit_perform_request(unit_index, 9);
	}
	byte *state = (byte *)UNIT_GET(unit_index) + UNIT_GET(unit_index)->unknown346;
	if (g_4677c8[0x1a]->interrupted)
	{
		g_4677c8[0x1a]->interrupted(unit_index, 0x1a);
	}
	*(dword *)(state + 4) &= ~0x4000000;
	state = (byte *)UNIT_GET(unit_index) + UNIT_GET(unit_index)->unknown346;
	if (g_4677c8[0x1b]->interrupted)
	{
		g_4677c8[0x1b]->interrupted(unit_index, 0x1b);
	}
	*(dword *)(state + 4) &= ~0x8000000;
	state = (byte *)UNIT_GET(unit_index) + UNIT_GET(unit_index)->unknown346;
	if (g_4677c8[0x16]->interrupted)
	{
		g_4677c8[0x16]->interrupted(unit_index, 0x16);
	}
	*(dword *)(state + 4) &= ~0x400000;
	function_114240(unit_index);
}

/* the engine object's vtable as far as the weapon pickup check reads it */
class c_unit_engine_view
{
public:
	virtual void v00() = 0;
	virtual void v01() = 0;
	virtual void v02() = 0;
	virtual void v03() = 0;
	virtual void v04() = 0;
	virtual void v05() = 0;
	virtual void v06() = 0;
	virtual void v07() = 0;
	virtual void v08() = 0;
	virtual void v09() = 0;
	virtual void v10() = 0;
	virtual void v11() = 0;
	virtual void v12() = 0;
	virtual void v13() = 0;
	virtual void v14() = 0;
	virtual void v15() = 0;
	virtual bool may_pick_up_weapon(long player_index, long weapon_index) = 0;
};

/* which of the unit's hands (0, 1) and slots (2, 3) may take the weapon:
   free hands, a second hand for dual-wieldable weapons, a matching weapon
   replaced only by a fresher one, an empty weapon only when it adds
   rounds, not a fully used one for AIs; the player code and the engine
   have the last word, then the weapon learns its holder. A weapon's rounds
   are its magazines' (+0x22a, 0x10 bytes each), or its definition's
   (+0x2c4, 0x5c bytes each) for weapons 0x106030 says refill and, when
   counting a held weapon in a multiplayer engine mode 7 with option 0x20,
   for the dropped weapon of a player on a flagged team; retail expands both
   counts in place */
// score 492 (was 530): Frame 0x3c and length 1785 now agree. Early object/tag
// spills, magazine flags, later register/slot choices and the expanded
// 0x106030 call remain. The indexed magazine pass gives native pointer steps
// and a countdown, confirmed by the ordinary whole-file check.
// Earlier 530 from 1032: Round 2 of #1049. The draft's logic agreed; the drop came from
// shape. Retail expands the round counts in place (the draft called a static helper): the slot loop's magazine loop
// reloads the unit and the definition count each pass and calls 0x106030 per magazine, the held-weapon loop reads
// the engine mode/team words each pass; the three flag tests
// are TEST_FIELD_BIT (load, shift, test dl,1) and the second-hand bit is read from the definition again; `empty`
// and `loaded` are declared with the other flags (registers DL/BL, as retail); the body sits in `if (0xcd6a0)` with
// one `return result` so the early exit shares the epilogue (prologue now identical); the definition-match tests are
// repeated, not a bool. Still differs: register and spill-slot choices only (retail keeps the weapon and definition
// pointers in EDI/ESI until `push ebp`, spills them at 0x18/0x24 and the unit offset at 0x2c; ours spills early to
// 0x14/0x18, so most later [esp+N] differ), the slot loop's two induction registers (retail EBP=0x5c step, EBX=0x10;
// ours the reverse), the order of two loads in the match loop, the modes pointer in EBX versus ESI, and 0x106030,
// which retail calls and ours inlines. A file-scope inline_depth(0) pair around this function keeps it a call
// (482) but is not kept, as it doesn't give a match.
// Tried: weapon looked up before the unit 510; first_slot or total declared elsewhere 483/482; state word as
// short-array index, branch order swapped in the magazine loop, magazine pointer arithmetic 482 each; `break`
// replaced by `return result` inside and `return false` after the loop 490 (separate early epilogue); helper static
// function for both loops 780-1032.
// Round 87: the first magazine pass uses its ordinary index for both real
// strides, which compile to pointer steps and a countdown as in retail.
// Body pointer steps 530; slot indexing 530; tag-pointer rename 530;
// shared initial headers 512/1809 (NOT kept: mixed cached/direct pool reads);
// indexed magazines 492/1785; per-arm held sums 510/1809; direct empty answer
// 506/1767; library zero initialization 530. NOT kept: the per-arm held sums
// and direct empty answer; the indexed pass is confirmed by the plain check.
// @retail 0xcd7b0
bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes)
{
	bool result = false;

	if (function_cd6a0(unit_index, NONE, weapon_index))
	{
		s_unit *unit = UNIT_GET(unit_index);
		s_unit *weapon = UNIT_GET(weapon_index);
		byte *local_67e06b = UNIT_DEFINITION_GET(weapon);
		long magazine_count = *(long *)(local_67e06b + 0x2c0);
		long first_slot = NONE;
		bool weapon_empty = false;
		bool second_hand_only;
		bool has_rounds = false;
		bool dual_wieldable;
		long total = 0;
		bool empty = false;
		bool loaded = false;

		if (magazine_count > 0)
		{
			byte *magazine = *(byte **)(local_67e06b + 0x2c4) + 8;
			short *state = (short *)((byte *)weapon + 0x22a);

			for (long i = 0; i < magazine_count; i++)
			{
				if (*(short *)(magazine + i * 0x5c) > 0)
				{
					if (state[i * 8 + 1] == 0 && state[i * 8] == 0)
					{
						empty = true;
					}
					else
					{
						loaded = true;
					}
				}
			}
			if (empty && !loaded)
			{
				weapon_empty = true;
			}
		}
		byte *local_67e06b_4 = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));
		dword weapon_flags = *(dword *)(local_67e06b_4 + 0x12c);
		dual_wieldable = TEST_FIELD_BIT((weapon_flags >> 22) & 1) || TEST_FIELD_BIT((weapon_flags >> 23) & 1);
		second_hand_only = (*(dword *)(local_67e06b_4 + 0x12c) >> 23) & 1;
		long *slot = unit->weapon_object_indices;
		for (long i = 0; i < 4; i++, slot++)
		{
			if (*slot != NONE)
			{
				long held_definition_index = UNIT_GET(*slot)->definition_index;

				if (weapon->definition_index == held_definition_index ||
					*(long *)(local_67e06b + 0x304) == held_definition_index)
				{
					if (first_slot == NONE)
					{
						first_slot = i;
					}
					for (long m = 0; m < *(long *)(local_67e06b + 0x2c0); m++)
					{
						s_unit *slot_weapon = UNIT_GET(*slot);
						long rounds;

						if (function_106030(*slot))
						{
							byte *slot_definition = UNIT_DEFINITION_GET(slot_weapon);

							if (*(long *)(slot_definition + 0x2c0) <= m)
							{
								continue;
							}
							rounds = *(short *)(*(byte **)(slot_definition + 0x2c4) + m * 0x5c + 0xc);
						}
						else
						{
							rounds = *(short *)((byte *)slot_weapon + 0x22a + m * 0x10);
						}
						if (rounds > 0)
						{
							has_rounds = true;
							total += rounds;
						}
					}
				}
			}
		}
		long held[2];
		*(dword *)modes = 0;
		for (long hand = 0; hand < 2; hand++)
		{
			s_unit *holder = UNIT_GET(unit_index);
			short index = (&holder->current_weapon_index)[hand];
			long held_index = index != NONE ? holder->weapon_object_indices[index] : NONE;

			modes[hand] = true;
			modes[hand + 2] = true;
			held[hand] = held_index;
			if (held_index != NONE)
			{
				s_unit *held_weapon = UNIT_GET(held_index);
				char state = *((char *)held_weapon + 0x20c);

				if (TEST_FIELD_BIT((*(dword *)(UNIT_DEFINITION_GET(held_weapon) + 0x12c) >> 3) & 1) || state == 1 || state == 2)
				{
					modes[hand] = false;
				}
			}
			else
			{
				modes[hand + 2] = false;
			}
		}
		bool dual[2];
		*(word *)dual = 0;
		if (dual_wieldable)
		{
			if (held[0] == NONE)
			{
				modes[1] = false;
				modes[3] = false;
			}
			else
			{
				dword held_flags = *(dword *)(UNIT_DEFINITION_GET(UNIT_GET(held[0])) + 0x12c);

				if (!TEST_FIELD_BIT((held_flags >> 22) & 1) && !TEST_FIELD_BIT((held_flags >> 23) & 1))
				{
					modes[1] = false;
					modes[3] = false;
				}
			}
			if (second_hand_only)
			{
				modes[0] = false;
				modes[2] = false;
			}
			dual[0] = held[1] != NONE;
			dual[1] = held[0] != NONE;
		}
		else
		{
			modes[1] = false;
			modes[3] = false;
		}
		if (first_slot != NONE)
		{
			for (long hand = 0; hand < 2; hand++)
			{
				bool hand_dual = dual[hand];

				if (!hand_dual)
				{
					modes[hand] = hand_dual;
				}
				if (modes[hand + 2])
				{
					s_unit *held_weapon = UNIT_GET(held[hand]);
					long held_definition_index = held_weapon->definition_index;

					if ((weapon->definition_index == held_definition_index ||
						*(long *)(local_67e06b + 0x304) == held_definition_index) &&
						*(real *)((byte *)held_weapon + 0x184) > *(real *)((byte *)weapon + 0x184))
					{
						continue;
					}
					if (weapon->definition_index == held_definition_index ||
						*(long *)(local_67e06b + 0x304) == held_definition_index || !hand_dual)
					{
						modes[hand + 2] = false;
						modes[hand] = false;
					}
				}
			}
		}
		if (weapon_empty)
		{
			for (long hand = 0; hand < 2; hand++)
			{
				if (!has_rounds)
				{
					dual[hand] = false;
				}
				else if (dual[hand] && held[hand] != NONE && UNIT_GET(held[hand])->definition_index == weapon->definition_index)
				{
					s_unit *held_weapon = UNIT_GET(held[hand]);
					long held_count = *(long *)(UNIT_DEFINITION_GET(held_weapon) + 0x2c0);
					long held_total = 0;

					for (long m = 0; m < held_count; m++)
					{
						long rounds;
						bool by_definition = false;

						if (g_4e6948->state == 2 && *((byte *)held_weapon + 0x12c) & 1)
						{
							long owner_index = *(long *)((byte *)held_weapon + 0x154);

							if (owner_index != NONE)
							{
								long player_index = UNIT_GET(owner_index)->unknown13c;

								if (player_index != NONE && g_55e4d0[g_4e9ae8->engine_index] && *(long *)((byte *)g_4e6948 + 0x180) == 7 &&
									((1 << player_index) & *(word *)g_510c9c) && *((byte *)g_4e6948 + 0x22c) & 0x20)
								{
									by_definition = true;
								}
							}
						}
						if (by_definition)
						{
							byte *held_definition = UNIT_DEFINITION_GET(held_weapon);

							rounds = *(long *)(held_definition + 0x2c0) > m ?
								*(short *)(*(byte **)(held_definition + 0x2c4) + m * 0x5c + 0xc) : 0;
						}
						else
						{
							rounds = *(short *)((byte *)held_weapon + 0x22a + m * 0x10);
						}
						held_total += rounds;
					}
					if (held_total == total)
					{
						dual[hand] = false;
					}
				}
				if (!dual[hand])
				{
					modes[hand] = false;
					modes[hand + 2] = false;
				}
			}
		}
		if (unit->actor_index != NONE &&
			((function_101d20(weapon_index) && *(real *)((byte *)weapon + 0x184) == 1.0f) || weapon_empty))
		{
			modes[0] = false;
			modes[1] = false;
			modes[2] = false;
			modes[3] = false;
		}
		if (unit->unknown13c != NONE)
		{
			function_1509e0(unit->unknown13c, weapon_index, modes);
		}
		for (long hand = 0; hand < 2; hand++)
		{
			if (modes[hand] || modes[hand + 2])
			{
				result = true;
				long player_index = unit->unknown13c;
				if (player_index != NONE)
				{
					c_unit_engine_view *engine = (c_unit_engine_view *)g_55e4d0[g_4e9ae8->engine_index];

					if (engine)
					{
						result = engine->may_pick_up_weapon(player_index, weapon_index);
					}
				}
				if (result)
				{
					function_1060a0(weapon_index, unit_index);
				}
				break;
			}
		}
	}
	return result;
}

// MiniMax A (11-12): no new form, no lower score (253): the frame is still 0x24 against retail 0x28 (retail keeps the second loop first in a stack slot, the flags word in memory and the hand index spilled), the unit is still held in edx, and retail still looks the unit up again after 0x110ab0 and reloads the state offset and word for each of the three state-bit tests where ours reads them once; the byte compares of the 0x113da0 and 0x113df0 results reach retail test al, al for two points only, which is under the keep bar, so the body stands.
/* the unit's control update: counts down its ragdoll (+0x1f5) and two
   short timers, crouches or stands by the control bits, keeps its weapon
   and grenade choices, zoom (with the weapon's or the globals' zoom
   sounds), and passes each hand's trigger state to its weapon; whether
   anything changed */
// MiniMax H (11-12): no new form, no lower score (253); the frame is still 0x24 against retail's 0x28 (retail keeps
// the second loop's `first` in a stack slot, the flags word in memory and the hand index spilled), the unit is still
// held in EDX, and retail still looks the unit up again after 0x110ab0 and reloads the state offset and word for each
// of the three state-bit tests where ours reads them once.
// MiniMax G (11-11): no new form, no lower score (253); the frame is still 0x24 against retail's 0x28 (retail keeps
// the second loop's `first` in a stack slot, the flags word in memory and the hand index spilled), the unit is still
// held in EDX, and retail still looks the unit up again after 0x110ab0 and reloads the state offset and word for
// each of the three state-bit tests where ours reads them once; the byte compares of the 0x113da0/0x113df0 results
// reach retail's `test al, al` for two points only, which is under the keep bar, so the body stands.
// MiniMax S (11-11): no lower score (253): the trigger flags' first bit with the word cast moved in front of the
// shift 253 (same code, still a movzx cx,cl and a 32-bit shl where retail has a 16-bit shl cx,1) and the same with
// an explicit word cast around the bit 257 (1446 bytes, worse); the frame stays 0x24 against retail's 0x28.
// MiniMax I (11-11): no new form, no lower score (253); the frame is still 0x24 against retail's 0x28
// (retail keeps the second loop's `first` in a stack slot, the flags word in memory and the hand index
// spilled), the unit is still held in EDX, and retail still looks the unit up again after 0x110ab0 and
// reloads the state offset and word for each of the three state-bit tests where ours reads them once.
// MiniMax C (11-10): NOT kept (coordinator, 11-11): 232 from the trigger flags as a word in a word/dword union passed as its dword (a single local wrapped in a union) together with byte compares of the 0x113da0/0x113df0 results; retail has the 16-bit flag ops and the 0x28 frame. Try the byte compares alone, or a 16-bit flags local passed widened, before any aggregate.
// MiniMax E (11-11): no new form kept, no lower score (253): the coordinator's byte compares of the
// 0x113da0/0x113df0 results do reach retail's `test al, al` (251, two points, under the bar, so the body
// is restored), the hand loop's values as chars give 267, the flags local as a dword 278 and the second
// loop's hand index as a char 255 (253 with the byte compares); what is left is the frame, 0x24 against
// retail's 0x28, with the changed flag at [esp+0x13] where retail has [esp+0x12] - retail keeps the
// second loop's `first` in a stack slot, the flags word in memory and the hand index spilled, so retail
// carries four more bytes of locals - and the unit is still held in EDX while retail looks the unit up
// again after 0x110ab0 and reloads the state offset and word for each of the three state-bit tests.
// MiniMax B (11-14), NOT kept (coordinator: the 244 -> 15 rests on steering forms: `(byte)` casts on the two call
// results, `(real)(char)zoom`, a word/long union for the trigger flags, and UNIT_GET written out by hand through two
// copies of the pool pointer; the natural if-form of the zoom weapon alone measures 274. LEAD: B's form is 15 from a
// match): score 15 (was 23): the changed flag is a bool, so the return is not normalised and the extent
// matches retail's 1470 bytes; the zoom's scale takes the zoom's low byte (`(real)(char)zoom`), which gives retail's
// `movsx edx, bl`; the first flag bit keeps retail's byte test and word widening (`and cl, 1` and `movzx cx, cl`).
// Still differs: retail's first shift of the flags is 16-bit (`shl cx, 1`) where ours shifts 32-bit (`shl ecx, 1`)
// (MSVC6 narrows a shift only on a 16-bit load, as retail 0x43740's `short count = source->point_count[slot] * 2;`),
// and three address operands are ordered the other way round (`[ecx + eax + 8]` against retail's `[eax + ecx + 8]`).
// Tried for the 16-bit shift: the word cast on the bit, a word local, the multiply by two, the compound assignment,
// the two-step assignment, a signed member, a char or unsigned char cast or local, a bool local, a word-only chain
// variable and a word variable read as a dword; each keeps the 32-bit shift or scores worse, and the three pool
// lookups keep our SIB order under the macro, byte-arithmetic and header-pointer spellings. This round again: the
// shifted operand is the only lever and VC6 folds the flag bit back into it every time - an unsigned short, short, byte
// or unsigned char cast on the bit, a word or byte local for it, the multiply by two, the ternary `? 2 : 0`, the int
// expression without the bool cast, the two-step through the flags storage (plain, cast and compound), a second word
// view of that storage and a word cast on the int operand all keep the 32-bit shift, and a plain word variable for the
// whole chain makes the chain's OR and AND 32-bit as well, so the 16-bit loads of the flags storage are what keep
// them 16-bit.
// B's form, not kept: score 15 (was 23): the changed flag is a bool and the two animation-slot results are compared as bytes; the
// trigger flags keep both views of their storage (the word is built with retail's 16-bit operations and the call
// takes the wide view of the same slot), which gives retail's 0x28 frame and the dword the call reads; the zoom
// block's two header lookups each read the pool's data through a pool local of their own, and the zoom weapon is
// NONE then an `if`, which gives retail's hoisted `or eax, -1`. Still differs: retail's first shift of the flags
// is 16-bit (`shl cx, 1`) where ours shifts 32-bit (`shl ecx, 1`), and three address operands are ordered the
// other way round (`[ecx + eax + 8]` against retail's `[eax + ecx + 8]`).
// +0x10a goes through s_unit_flags_10a_view (load, shift, test), the zoom is a short compared against
// the stored field (cmp bx; the field is then read, not the local, for the NONE tests), the zoom sound is read
// before the level count, and the second loop's weapon index is NONE then an `if`. Still differs: frame 0x24 versus 0x28 (retail keeps the second loop's `first`
// at [esp+0x13], the flags word in memory with the call reading it as a dword, and the hand index spilled),
// and register choices (holder in EDX, first in BL/memory, a few scheduling swaps); and retail looks the unit
// up again after 0x110ab0 and reloads the state offset and word for each of the three state-bit tests, which
// ours reads once. Volatile reads of the state offset and word reproduce those reloads (249) but are not kept:
// volatile goes only on parameters and locals here.
// Tried: the flags passed as *(long *)&flags 252; with the volatile reads: bit mask computed before the pointer
// 249, the pointer read volatile 254.
// Round 179: the four action-bit tests as unit_action_active calls (retail inlines 0xe6fe0 there, reloading +0x346
// per test) 310 -> 254, kept; the second loop's weapon through the existing 0xcbd50 254 (same code), kept. Still
// differs: frame 0x24 versus 0x28 (retail writes the flags word once from CX into a stack slot and pushes it as a
// dword, builds it with 16-bit ops, spills `first` and uses EBX/EDX where ours uses EDX/EDI); 0x113da0/0x113df0
// results tested as AL in retail (ours EAX); 0xd1540 takes the unit on the stack in retail. Tried: 0x100430's
// `value` as word (definition and declaration) 257; the zoom block's weapon as NONE then if, with the scales from
// the stored byte field 293; only the scales from the byte field `(real)unit->unknown240` 253; the zoom block's
// weapon through 0xcbd50 293 (retail re-reads the header data pointer there, ours keeps it).
// Round 5 of #1049: no logic difference found in a fresh walk (every test, store and call agrees; behaviour is the
// same except that retail passes the flags dword with its upper half unwritten, see below). The 20 missing bytes
// are the flags slot (4: retail keeps the flags in a stack dword, writes the word `mov [esp+0x20],cx` and
// passes `mov eax,[esp+0x20]`), `first` stored at [esp+0x13] (4; pushes `changed` to [esp+0x12]), the loop-head
// alignment (jmp over 8 bytes of padding, follows from the size) and the zoom block's second header lookup.
// Tried (not kept; layout-only forms count only on a MATCH): flags as `union { word w; long l; }` built through .w
// and passed as .l: 253 -> 238 (frame 0x28 as retail and `or cx,4` / `and cx,0xfffb` become retail's 16-bit ops;
// a plain word local or short gives 32-bit `ecx` ops), with `last` as dword `(control >> 30) & 1` 234; the same
// with `last = control >> 30` tested `& 1` 259; word-typed bitfield struct built bit by bit 344; the `flags |= n`
// / `flags &= ~n` if/else form of the ternaries (same code); `short flags` 283; the zoom weapon as NONE then an if
// 284, through 0xcbd50 292 (that one matches retail's `or eax,-1` shape but ours takes the idx in DI, because retail
// holds the header global in EDI for its second data load; ours keeps the data pointer and loads the global once).
// Still differs after the union: `first` is not spilled (retail needs EBX as a third bit-test temp because it keeps
// `control` in EDX where ours uses EDI), `shl ecx,1` for `shl cx,1`, `test dl,dl` after `and dl,1` for `test bl,1`.
// Round 85 (m3 lane 11, fast; header before this round: score 254 (was 310)): 0x113da0/0x113df0 returning bool (definitions and declarations; retail's callers test AL)
// 251 but both callees stop matching (5, 8); the flag block reading unit->unknown148 at each use 253 (`sar`, control
// still in EDI); `last` a dword shift tested `& 1` (no union) 261, with the field reads 261; flags/last declared at the
// loop top 253, the order last, flags, control 253 (same code).
// Round 175 (m3 lane 6, fast then plain): kept 253 -> 244: bit 7 (0x80) set inside each hand's arm from its own
// control bit (30 / 31), with no `last` local; retail tests the shifted control after the arms join (`test bl, 1`),
// which gives retail's second-loop registers (control in EDX, EBX as the bit temp, `first` spilled to [esp+0x13]).
// The definition flags read as `definition->unit.flags` (same code). On 244: the zoom weapon through 0xcbd50 274
// (masked 27: retail's `or eax, -1` shape, but retail keeps the header global in EDI and reloads the data pointer
// for the weapon's lookup, ours keeps the data pointer); the NONE-then-if zoom weapon 274 (re-measured); the zoom
// block's unit read at each use 244; the weapon definition through object_get 244 (same code); `word flags = 0;`
// with bit 1 set by the same ternary as the others 246 (`movzx ecx, cl; and ecx, 1` for retail's `shl cx, 1`); the
// second loop's body under `if (current != NONE)` 244; the rate written into the call 244; `hand == 0` at each use
// for `first` 282. Diagnostic: the listed word/dword overlay for the flags on 244 gives 229 (frame 0x28, first
// difference +0xed; aligned with registers masked only the zoom block's reload and the loop pad remain; unmasked
// also the two `test al, al`, `movsx edx, bl` for the scale, `[ecx + edx + 8]` at the second loop's two unit
// lookups where ours has `[edx + ecx + 8]`, and `shl cx, 1`); with the 0xcbd50 zoom weapon too 266.
// MiniMax B (11-13): no new form, no lower score (253): retail keeps the flags in a stack dword and spills `first`, ours keeps the control word in edi and does not spill `first`; the 0x113da0 and 0x113df0 results are tested as al in retail and as eax here.
// P457: existing word setter for updates/all bits 244/246; zero-based set-only flags 362;
// wide/byte changed result 398/237 (byte gives native 1470 bytes, gain seven below eight);
// held countdown as complete outcome 239; sound-request/flag-word role names 244/244.
// NOT kept: ties, regressions or subthreshold gains; native flag dword transfer remains.
// @retail 0xc58f0
bool __stdcall function_c58f0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_definition *definition = (s_unit_definition *)UNIT_DEFINITION_GET(unit);
	bool changed = false;

	if (!(TEST_FIELD_BIT((definition->unit.flags >> 11) & 1)))
	{
		if ((char)unit->unknown1f5 > 0)
		{
			if (--unit->unknown1f5 == 0)
			{
				function_e68c0(0x13, unit_index);
				function_e68c0(9, unit_index);
			}
			changed = true;
		}
		if (unit->unknown1fa > 0)
		{
			unit->unknown1fa--;
			changed = true;
		}
		if (unit->unknown1fb > 0)
		{
			unit->unknown1fb--;
			changed = true;
		}
		if (TEST_FIELD_BIT((unit->flags_134 >> 1) & 1))
		{
			dword control = unit->unknown148;

			if (!(control & 0x8000000) && !((1 << unit->type) & 1 && control & 0x4000))
			{
				if (!function_113da0(unit_index))
				{
					function_113d20(unit_index, 0.5f);
					changed = true;
				}
			}
			else if (!function_113df0(unit_index))
			{
				function_113d60(unit_index, 0.5f);
				changed = true;
			}
		}
	}
	if (!(TEST_FIELD_BIT((definition->unit.flags >> 10) & 1)) && !(TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2)))
	{
		if (*((byte *)unit + 0x19) & 8)
		{
			function_e68c0(0x13, unit_index);
			function_e68c0(9, unit_index);
		}
		else
		{
			for (long hand = 0; hand < 2; hand++)
			{
				long current = (&unit->current_weapon_index)[hand];
				long wanted = (&unit->unknown216)[hand];

				if (current != NONE)
				{
					unit->unknown228[current] = g_510c54->game_time;
				}
				if (wanted != current)
				{
					if (hand == 0)
					{
						function_e68c0(8, unit_index);
					}
					else if (hand == 1)
					{
						function_e68c0(0x12, unit_index);
					}
				}
			}
		}
		if (unit->unknown148 & 0x10000000)
		{
			function_e68c0(0, unit_index);
			function_e68c0(0xa, unit_index);
		}
		if (unit->next_grenade_index != unit->current_grenade_index && !function_110ab0(unit_index))
		{
			short grenade_type = function_cbec0(unit_index, unit->next_grenade_index, 0);

			if (grenade_type != NONE)
			{
				unit->current_grenade_index = (char)grenade_type;
			}
		}
		short zoom = function_c85c0(unit_index) ? unit->unknown241 : NONE;
		if (unit->unknown240 != zoom)
		{
			unit->unknown240 = zoom;
			if (unit->unknown240 == NONE)
			{
				unit->unknown26c = 0.0f;
			}
			s_unit *holder = UNIT_GET(unit_index);
			short index = holder->current_weapon_index;
			long weapon_index = index != NONE ? holder->weapon_object_indices[index] : NONE;
			real scale = 1.0f;
			long sound;

			if (weapon_index != NONE && *(short *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x1fe) > 0)
			{
				byte *local_67e06b = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));

				if (unit->unknown240 == NONE)
				{
					sound = *(long *)(local_67e06b + 0x278);
				}
				else
				{
					sound = *(long *)(local_67e06b + 0x270);
					short levels = *(short *)(local_67e06b + 0x1fe);

					if (levels > 1)
					{
						scale = (real)zoom / (real)(levels - 1);
					}
				}
			}
			else
			{
				byte *globals = *(byte **)((byte *)g_4e034c + 0x134);

				if (unit->unknown240 == NONE)
				{
					sound = *(long *)(globals + 0xd0);
				}
				else
				{
					sound = *(long *)(globals + 0xc8);
					long levels = *(long *)(globals + 0xb8);

					if (levels > 1)
					{
						scale = (real)zoom / (real)(levels - 1);
					}
				}
			}
			if (sound != NONE)
			{
				s_sound_label_play play;

				play.label = NONE;
				play.tag_index = sound;
				play.scale = scale;
				play.variant = 0;
				function_189210(unit_index, &play, 0x4000095);
			}
		}
	}
	if (!(TEST_FIELD_BIT((definition->unit.flags >> 11) & 1)))
	{
		for (long hand = 0; hand < 2; hand++)
		{
			bool first = hand == 0;

			if ((&unit->current_weapon_index)[hand] == NONE)
			{
				continue;
			}
			long weapon_index = function_cbd50(unit_index, (&UNIT_GET(unit_index)->current_weapon_index)[hand]);
			bool held = function_110ab0(unit_index);

			if (unit->unknown1ec > 0)
			{
				held = false;
			}
			if (unit_action_active(unit_index, 0x16) || unit_action_active(unit_index, 0x1a) ||
				unit_action_active(unit_index, 0x1b) || unit_action_active(unit_index, first ? 8 : 0x12))
			{
				held = true;
			}
			dword control = unit->unknown148;
			word flags;

			if (first)
			{
				flags = (word)((TEST_FIELD_BIT((control >> 16) & 1)) << 1);
				flags = TEST_FIELD_BIT((control >> 17) & 1) ? flags | 4 : flags & ~4;
				flags = TEST_FIELD_BIT((control >> 18) & 1) ? flags | 8 : flags & ~8;
				flags = TEST_FIELD_BIT((control >> 19) & 1) ? flags | 0x10 : flags & ~0x10;
				flags = TEST_FIELD_BIT((control >> 20) & 1) ? flags | 1 : flags & ~1;
				flags = TEST_FIELD_BIT((control >> 30) & 1) ? flags | 0x80 : flags & ~0x80;
			}
			else
			{
				flags = (word)((TEST_FIELD_BIT((control >> 21) & 1)) << 1);
				flags = TEST_FIELD_BIT((control >> 22) & 1) ? flags | 4 : flags & ~4;
				flags = TEST_FIELD_BIT((control >> 23) & 1) ? flags | 8 : flags & ~8;
				flags = TEST_FIELD_BIT((control >> 24) & 1) ? flags | 0x10 : flags & ~0x10;
				flags = TEST_FIELD_BIT((control >> 25) & 1) ? flags | 1 : flags & ~1;
				flags = TEST_FIELD_BIT((control >> 31) & 1) ? flags | 0x80 : flags & ~0x80;
			}
			flags = TEST_FIELD_BIT((control >> 29) & 1) ? flags | 0x100 : flags & ~0x100;
			flags = held ? flags | 0x20 : flags & ~0x20;
			flags = unit->unknown240 != NONE ? flags | 0x40 : flags & ~0x40;
			real rate = first ? *(real *)&unit->unknown1c0 : *(real *)&unit->unknown1c4;
			function_100430(weapon_index, flags, rate);
		}
		function_d1540(unit_index);
	}
	return changed;
}

/* the angle a turn covered in a tick as a share of its rate, 0 to 1 */
static real unit_turn_share(real angle, real step)
{
	real share = angle / step;

	if (0.0f > share)
	{
		return 0.0f;
	}
	if (share > 1.0f)
	{
		return 1.0f;
	}
	return share;
}

/* the first byte of the unit's state record as bits: whether the aim and the look turn within limits */
struct s_unit_turn_state_view
{
	word unknown0 : 5;
	word aim_limits_on : 1;
	word look_limits_on : 1;
	word unknown7 : 1;
};

// MiniMax A (11-12): no new form, no lower score (395 in the plain build, 453 in the reduced one): the moved flag still sits in a register where retail keeps it in a stack byte, the state and aim pointers still take the other register roles, and the two divisions by the stored step still share one reciprocal; the forms for each of those are in the note below and none is kept, so the body stands.
/* turns the unit's aim (+0x168) toward its desired aim (+0x15c) and its
   look (+0x18c) toward +0x180 at the definition's rates (scaled while it
   is in state 1 at +0x1bc), within its aim and look limits when they are
   on (0xc7840); records the aim's turn speeds (+0x243, +0x1f8); whether
   either moved */
// MiniMax H (11-14): no new form, no lower score (395): re-measured with the plain build (1660 bytes, as retail, first difference at +0x2); the flag home still moves and the shared step's single reciprocal is still where retail does not have it, and that step helper lives in another file.
// MiniMax B (11-13): no new form, no lower score (395): re-measured with the plain build (1660 bytes, as retail, first difference at +0x2); what is left is the moved flag home and the single reciprocal of the shared step.
// MiniMax H (11-12): no new form, no lower score (395 in the plain build, 453 in the reduced one, which builds
// 0x11f0d0 differently); the moved flag still sits in a register where retail keeps it in a stack byte, the state and
// aim pointers still take the other register roles, and the two divisions by the stored step still share one
// reciprocal; the forms tried earlier for each of those are in the note below and none is kept, so the body stands.
// MiniMax G (11-11): no new form, no lower score (395 in the plain build, 453 in the reduced one, which builds
// 0x11f0d0 differently); the moved flag still sits in a register where retail keeps it in a stack byte, the state
// and aim pointers still take the other register roles, and the two divisions by the stored step still share one
// reciprocal; the forms tried earlier for each of those are in the note below and none is kept, so the body stands.
// MiniMax S (11-11): no new form, no lower score (395 in the plain build, 453 in the reduced one, which builds
// 0x11f0d0 differently): the three differences left are the moved flag's stack byte, the state and aim pointer's
// register roles and the one reciprocal the two divisions share, and the forms for each of them are in the note
// below.
// MiniMax I (11-11): no new form, no lower score (395 in the plain build, 453 in the reduced one, which
// builds 0x11f0d0 differently); the moved flag still sits in a register where retail keeps it in a stack
// byte, the state pointer and the aim pointer still take the other register roles, and the two divisions
// by the stored step still share one reciprocal.
// MiniMax C (11-10): no new form tried, no lower score (the plain build reads 395, the reduced build 453 because 0x11f0d0 is built differently there); what is left is the moved flag's stack byte, the definition and aim pointer's register roles and the two divisions by the stored step, and the forms for each of those are in the note below.
// MiniMax E (11-11): no new form tried, no lower score (395 in the plain build, 453 in the reduced one,
// which builds 0x11f0d0 differently): the moved flag still sits in a register where retail keeps it in a
// stack byte, the state and aim pointers still take the other register roles, and the two divisions by
// the stored step still share one reciprocal, so the body stands as found.
// score 395 (was 395): built the turn frame in place (b9fc0 writes frame.forward/up, left and up cross products as
// retail), result as a variable, the state byte's bits through s_unit_turn_state_view, +0x10a through
// s_unit_flags_10a_view, the unit/definition bit tests nested. Still differs: retail keeps the result `moved`
// in a stack slot ([esp+0xb], 0 stored up front, reloaded on the all-close path, BL=1 on the moved path)
// where ours folds it to constants (frame 0x78 versus 0x7c, every later local 4 bytes off), the scale/rate
// loads and edi/ebx assignment (state pointer in EDI, &aim in EBX; ours puts state in EDX) and a few
// schedulings. Tried: early returns instead of the nesting (411, the nested form 384 before the state view),
// `volatile bool moved` (397: stack slot and frame match, but stores/loads are not retail's), scale*rate
// operand order (no change), declaring old_flat/new_flat in the other order or at function scope (no change;
// 0x11ce20 takes the new aim first, as retail passes it in ECX at 0xc63be; the other order also scores 401).
// Round 7 of #1049 (401 -> 395): both 0x11f0d0 calls had the vectors in the wrong parameter order (aim, desired,
// velocity). Retail's callee writes the zero vector through its first register (EBX), copies the target into
// the second (ESI) and reads the third (EDI), and retail's call sites load EBX = &velocity (0x174 / 0x198),
// ESI = &aim (0x168 / 0x18c), EDI = &desired (0x15c / 0x180): the arguments are (velocity, aim, desired), so
// the old source zeroed the aim and copied the velocity over the desired vector (a behaviour bug). Remaining:
// `moved` sits in EBX for the whole function (xor bl,bl) where retail keeps it in a stack byte ([esp+0xb],
// reloaded at the all-close return; EBX holds &aim), the reciprocal ours makes of the shared `step` division
// (retail divides twice by the stored step), and schedulings. Tried (0xc60c0 with callers and callees in one
// check): all-close as the single `return moved` after `if (!(close)) { moved = true; ... }` 419 (slot and
// register assignment as retail but every callee-saved register is pushed at entry, frame 0x80); the same
// with an early return for the two start tests 440; early return for the start tests alone 410; moved as a
// one-field struct / one-element array / through a pointer 395 (all fold the same); `return false` at the
// all-close point 395; byte *state declared before the rates 397; both turn shares written out in place
// (no helper) 399; &aim as a named pointer (no change).
// Round 85 (m3 lane 11, plain; header before this round: score 395 (was 401)): the shares as in-place clamps on
// yaw_share/pitch_share (retail stores the quotient and then clamps that slot; a re-measure of the written-out shares)
// 399 (1664), with the single exit 423; no `step` local, the rate product at each
// division: through the helper 404 (1648), with the in-place clamps 400 (1660, retail's two `divss` by the stored
// product and no reciprocal: ours makes the reciprocal because both divisions share one named divisor), plus the
// single exit 410 (1691; masked aligned 83 stack / 52 register against 103 / 68 here, the lead), plus an early
// `return false;` or `return moved;` for the two start tests 431. Every single-exit form saves all four registers
// at entry with an unused bottom slot (frame 0x80); retail saves EBX inside the `if`, EDI around the state pointer.
// MiniMax B (11-13): no new form, no lower score (395): `moved` still sits in ebx for the whole function where retail keeps it in a stack byte, and ours still makes one reciprocal of the shared `step` where retail divides twice by the stored step.
// MiniMax G (11-13): no new form, no lower score (395): the moved flag still sits in a register where retail
// keeps it in a stack byte, and this build still makes one reciprocal of the shared step where retail divides
// twice by the stored step; the single-exit and in-place-clamp forms in the note below score 419 and 400.
// MiniMax A (11-13): no new form tried this round, no lower score (395): the frame is 0x78 here and 0x7c in
// retail, `moved` still sits in ebx for the whole function where retail keeps it in a stack byte, and this build
// still makes one reciprocal of the shared step where retail divides twice by the stored step.
// @retail 0xc60c0
bool __stdcall function_c60c0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	bool moved = false;

	if (!(TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 10) & 1)) && !(TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2)))
	{
		vector3f old_aim = unit->unknown168;
		vector3f old_look = *(vector3f *)&unit->unknown18c;
		real scale = unit->unknown1bc == 1 ? *(real *)(definition + 0x14c) : 1.0f;
		real arg_3097c5_2 = *(real *)(definition + 0x144) * scale;
		real pitch_rate = *(real *)(definition + 0x148) * scale;
		byte *state = (byte *)unit + unit->unknown33e;

		if (arg_3097c5_2 == 0.0f && pitch_rate == 0.0f)
		{
			unit->unknown168 = unit->unknown15c;
			if (function_c4970(unit_index))
			{
				function_cba50(unit_index, &unit->unknown168, false);
			}
			unit->unknown174 = *g_4687a4;
		}
		else if (TEST_FIELD_BIT(((s_unit_turn_state_view *)state)->aim_limits_on))
		{
			transform4x3f frame;

			frame.scale = 1.0f;
			function_b9fc0(unit_index, &frame.forward, &frame.up);
			frame.position = *g_468788;
			frame.left.i = frame.up.j * frame.forward.k - frame.up.k * frame.forward.j;
			frame.left.j = frame.up.k * frame.forward.i - frame.up.i * frame.forward.k;
			frame.left.k = frame.up.i * frame.forward.j - frame.up.j * frame.forward.i;
			function_c7840(&frame, &unit->unknown168, &unit->unknown15c, g_510c54->rate, &unit->unknown174,
				(real const *)(state + 4), arg_3097c5_2, pitch_rate);
		}
		else
		{
			function_11f0d0(&unit->unknown174, &unit->unknown168, &unit->unknown15c, g_510c54->rate, arg_3097c5_2, pitch_rate);
		}
		real yaw_share;
		real pitch_share;
		if (*(real *)(definition + 0x144) == 0.0f)
		{
			yaw_share = 0.0f;
			pitch_share = 0.0f;
		}
		else
		{
			real step = g_510c54->rate * *(real *)(definition + 0x144);
			vector3f new_flat;
			vector3f old_flat;

			yaw_share = unit_turn_share(function_11ce20(&old_aim, &unit->unknown168), step);
			new_flat = unit->unknown168;
			old_flat = old_aim;
			old_flat.k = 0.0f;
			new_flat.k = 0.0f;
			function_1201a0(&old_flat, g_4687b0);
			function_1201a0(&new_flat, g_4687b0);
			pitch_share = unit_turn_share(function_11ce20(&new_flat, &old_flat), step);
		}
		unit->unknown243 = (byte)(long)(yaw_share * 255.0f);
		unit->unknown1f8 = (byte)(long)(pitch_share * 255.0f);
		real look_yaw_rate = *(real *)(definition + 0x150) * scale;
		real look_pitch_rate = *(real *)(definition + 0x154) * scale;
		if (look_yaw_rate == 0.0f && look_pitch_rate == 0.0f)
		{
			*(vector3f *)&unit->unknown18c = unit->unknown180;
			function_cba50(unit_index, (vector3f *)&unit->unknown18c, true);
			unit->unknown198 = *g_4687a4;
		}
		else if (TEST_FIELD_BIT(((s_unit_turn_state_view *)state)->look_limits_on))
		{
			transform4x3f frame;

			frame.scale = 1.0f;
			function_b9fc0(unit_index, &frame.forward, &frame.up);
			frame.forward = unit->unknown168;
			frame.position = *g_468788;
			frame.left.i = frame.up.j * frame.forward.k - frame.up.k * frame.forward.j;
			frame.left.j = frame.up.k * frame.forward.i - frame.up.i * frame.forward.k;
			frame.left.k = frame.up.i * frame.forward.j - frame.up.j * frame.forward.i;
			frame.up.i = frame.forward.j * frame.left.k - frame.forward.k * frame.left.j;
			frame.up.j = frame.forward.k * frame.left.i - frame.forward.i * frame.left.k;
			frame.up.k = frame.forward.i * frame.left.j - frame.forward.j * frame.left.i;
			function_c7840(&frame, (vector3f *)&unit->unknown18c, &unit->unknown180, g_510c54->rate, &unit->unknown198,
				(real const *)(state + 0x14), look_yaw_rate, look_pitch_rate);
		}
		else
		{
			function_11f0d0(&unit->unknown198, (vector3f *)&unit->unknown18c, &unit->unknown180, g_510c54->rate,
				look_yaw_rate, look_pitch_rate);
		}
		if (0.0001f > (real)fabs(old_aim.i - unit->unknown168.i) && 0.0001f > (real)fabs(old_aim.j - unit->unknown168.j) &&
			0.0001f > (real)fabs(old_aim.k - unit->unknown168.k) &&
			0.0001f > (real)fabs(old_look.i - ((vector3f *)&unit->unknown18c)->i) &&
			0.0001f > (real)fabs(old_look.j - ((vector3f *)&unit->unknown18c)->j) &&
			0.0001f > (real)fabs(old_look.k - ((vector3f *)&unit->unknown18c)->k))
		{
			return moved;
		}
		moved = true;
		function_bba20(unit_index);
	}
	return moved;
}

/* a direction from a yaw and a pitch */
static void unit_direction_from_angles(vector3f *direction, real yaw, real pitch)
{
	real cosine = (real)cos(pitch);

	direction->i = (real)cos(yaw) * cosine;
	direction->j = (real)sin(yaw) * cosine;
	direction->k = (real)sin(pitch);
}

/* a world vector in a frame's axes, and back */
static void unit_vector_to_frame(vector3f *out, transform4x3f const *frame, vector3f const *v)
{
	out->i = frame->forward.i * v->i + frame->forward.k * v->k + frame->forward.j * v->j;
	out->j = frame->left.j * v->j + frame->left.i * v->i + frame->left.k * v->k;
	out->k = frame->up.i * v->i + frame->up.k * v->k + frame->up.j * v->j;
}

static void unit_vector_from_frame(vector3f *out, transform4x3f const *frame, vector3f const *v)
{
	out->i = frame->up.i * v->k + frame->left.i * v->j + frame->forward.i * v->i;
	out->j = frame->up.j * v->k + frame->left.j * v->j + frame->forward.j * v->i;
	out->k = frame->up.k * v->k + frame->left.k * v->j + frame->forward.k * v->i;
}

real function_50650(real cosine);

/* turns a direction toward a desired one within yaw and pitch limits (in
   a frame, when given), keeping an angular velocity: plans both angles'
   motions under a speed limit and an acceleration (0xc7300), evens them
   (0xc75d0), steps them (0xc7750), snapping when both are done */
// MiniMax A (11-14): no new form, no lower score (563): the frame is 0x8c against retail's 0x80, retail tests its
// second pointer at the entry and leaves the body when it is null, while this build stores two zeroed speeds first and
// only then branches; the two 0xc7750 calls still push position and velocity on the stack where retail passes them in
// xmm5 and xmm4 (BLOCKED-BY 0xc7750's inlining).
// MiniMax H (11-14): no new form, no lower score (563): re-measured with the plain build (2474 bytes against retail's 2483); the second pointer's entry test and the frame size still differ, and the cheaper 0xc7750 body that would take the call out of line would be inlined here and lose the matched 0x1e5af0 and 0x1faf80.
// MiniMax B (11-13): no new form, no lower score (563): re-measured with the plain build (2474 bytes against retail 2483, first difference at +0x2); what is left is the entry test of the second pointer and the frame size.
// MiniMax H (11-12): no new form, no lower score (563): the frame is still 0x8c against retail's 0x80, retail tests its second pointer at the entry while this build stores two zeroed speeds first, and the two 0xc7750 calls still push position and velocity on the stack where retail passes them in xmm5 and xmm4; the accumulator form of 0xc7750 that gives 28 also inlines into this body, so neither change is kept.
// MiniMax S (11-11): no new form, no lower score (563): the frame is still 0x8c against retail's 0x80 - retail tests
// its second pointer at the entry and leaves when it is null, while this build stores two zeroed speeds first and
// branches afterwards, and retail reuses the current angle slots for the new speeds while ours gives them their own;
// moving the zero stores into an else arm removes the entry stores but the frame becomes 0x88 and the accumulators
// become xmm5 and xmm6 (614), and parenthesising the frame helper's sums gives 565, so neither is kept:
// BLOCKED-BY 0xc7750's inlining.
// MiniMax G (11-11): no new form, no lower score: the two 0xc7750 calls still push position and velocity on the
// stack where retail passes them in xmm5 and xmm4 with the time in xmm3, and 0xc7750's accumulator form (28,
// retail's convention) is inlined into both calls (1103 here) and flips the matched caller 0x1faf80 to 16.
// MiniMax G (11-12): no new form tried this round, no lower score (563): the frame is still 0x8c against retail's
// 0x80, and the two 0xc7750 calls still pass their arguments on the stack instead of in xmm registers.
// score 563 (was 563): frame 0x8c versus retail 0x80; input/vector homes and prediction/call scheduling remain.
// MiniMax D (11-01): no new form tried, no lower score.
// Earlier 700 (was 723): frame 0x94 versus retail 0x80, input staging, vector homes and prediction/call scheduling remain.
// Paired role names 723; angular-velocity value 700 (kept); complete direction values 653; shared axes rotation 697;
// shared final cross 723; direction plus velocity values 628 (rejected: lose matched 0x1e5af0), with names 628 (unconfirmed);
// parallel predicted-angle results 723. The angular-velocity value alone preserves all matches.
// Round 178: both frame-to-world rotations through the existing matrix_transform_vector (retail rotates
// target in place with its components preloaded; that block then has retail's exact instructions) 717,
// 2363 bytes (offset-masked aligned misses 599 -> 547: the lead to build on); with the world-to-frame
// helper reading v's components into locals first (i, j, k or i, k, j) 669/671, but our LTCG then moves
// a parameter to the stack and matched 0x1e5af0 (43) and 0x1faf80 (4 -> 211) break; plus the prediction
// as an in-place unit-axis rotation of a copy (0x2551f0's helper shape) 721; plus named predicted angles 718.
// Round 180: the direction helper replaced by the shared direction_from_angles 700 (identical code, but not
// kept: the include and the dropped static helper move image-dependent 0x24cf66 from 114 to 115); the
// world-to-frame rows as dot3f calls 700 (identical), both 700; the rows computed into locals before the stores
// 700, the same in both helpers 700 (VC still folds the vector's loads). Reading v's components first (i, j, k)
// 650 and copying v into a local vector first 618 (2479 bytes) both make our LTCG pass `current` on the stack
// (ret 0x18), which breaks matched 0x1e5af0 (43, plain) and 0x1faf80 (4 -> 211): rejected. Retail keeps
// `current` in EDI with the components preloaded; find what keeps it a register parameter.
// Round 117: parameters reordered to (frame, current, desired, ...): retail sets up EAX (desired), EDI (current), ESI
// (frame) in that order at both 0xc60c0 calls, EAX before ESI at the other two; 700 (same), and 0x1faf80 4 -> 2.
// Round 37: the round-180 copy form rechecked 618 (2479 bytes, `current` on the stack); components read first 650
// (2435, stack); with the desired vector rotated first 634 (2445) but `current` still a stack argument (ret 0x18);
// both frame-to-world rotations through matrix_transform_vector in place 717 (2363); components first with the
// in-place rotations 669 (2411, stack). Every preloading form frees EDI, which then takes the velocity pointer
// (retail EBX), and `current` is spilled, so LTCG passes it on the stack. Retail keeps `wraps` in a stack byte
// copied to BL.
// Round 57 (m3 lane 9): retail reuses slots: the desired-in-frame vector also holds the clamped target and the final
// direction, and the velocity copy's slot later holds a copy of the current vector for the prediction. The clamped
// target computed into the desired-in-frame vector 700 (frame still 0x94); also the final direction there and the
// next direction in the velocity copy 700 (frame 0xa0); the velocity copied into the axis and normalized by the
// existing normalize_inline (retail's epsilon test, divide and `jnp` order) 706 (frame 0x88, the copy's slot now
// shared; sum and scale operand orders differ), by normalize3d 708 (0x88). 0xc7750 in retail's XMM convention
// (accumulators, ternary steps) is inlined here (2786 bytes, 995). Next: the prediction through 0x2551f0's
// static axis-rotation helper once it can be shared, on top of the axis normalization. The first atan2 as a named
// double (0xcfec0's fix) 694 (frame 0x98, plain) but caller 0xc60c0 401 -> 447.
// Round 282: motion/prediction scopes, their pair and common-output pair 700/700/700/700;
// complete planning scopes without/with motion or common output 653/653/653/653; planning with
// original speed initialization 700; only both speed results initialized at entry 653 (plain kept).
// Frame 0x94 and direction staging remain; every original vector, arithmetic tree and call is retained.
// Round 288: Complete frame-input value/ijk/ikj/kji staging on entry-zero speed graph, each
// without/with complete from-frame input value, 613/685/647/625/647/625/647/625. Best value-only form
// reaches retail 2483 bytes at 613, but all eight lose exact callers (43/210); all original helpers,
// body, parameter delivery and entry speed zeros restored.
// Round 343: Complete input value/scalar staging with early wrap bool/byte/word/long:
// 648/648/637/645/673/673/655/676; all lose exact callers 43/210.
// Early wrap alone bool/word 725/719. Entire helper, frame inputs, arithmetic,
// parameter delivery and entry speed zeros restored.
// DH274: existing clamped arccosine 601; direct axis 600, with early reciprocal 591;
// native copied axis normalized in place 574, with early reciprocal 563 (plain kept);
// either axis scaled before trigonometry 600/574; reciprocal without native axis 591;
// native axis/reciprocal without clamped producer 615. Original transform helpers retained.
// DH277: complete prediction field/scalar accumulators and radial/cosine terms 566/566/563/556; with shared
// cross 585/585/585/595. Seven-point cosine-term lead at 2479/2483 rejected below eight; original body and
// transform helpers restored.
// Round 515: Complete prediction through the existing vector setter, with magnitude-squared
// part, paired frame/motion names and phase-entry result declarations: 563/567/563/567/563/567/563/567.
// Frame/input homes remain; complete original body and every shared helper restored.
// Round 316: Native prediction input copy mutable/const, before-angle/after-trig
// 614/614/614/614; component aggregate counterparts 563/563/563/563.
// Early native copy reserves 0x98, late copy 0x8c; both matched callers stay exact.
// Original prediction input, axis, reciprocal and complete body restored.
// Round 286: Current native-axis graph with target/current-direction storage reuse and joint
// motion-record lexical order, seven subsets then all with complete cosine parts: 563/563/563/563/563/563/563/556. Original
// storage lifetimes and body restored; frame, vector homes and prediction/motion scheduling remain.
// Round 309: Plain axis/final-scale loops and accumulated actual completion result, subsets 1..7, then
// all unsigned component counters: 1090/576/1090/565/1091/577/1091/1091. Axis loops also lose caller 0x1faf80 at 16.
// Original scaling, completion results and body restored; frame/input/vector homes and
// prediction/call scheduling remain.
// Round 197 (lane 3, fast with 0x1e5af0, 0x1faf80): the stack `current` traces to the callee. 0xc7750 in its accumulator form
// takes retail's XMM5/XMM4/XMM3 convention (28, its note) and then, kept out of line, `current` stays in EDI with the
// components preloaded and both matched callers still match. Only a test ahead of the two 0xc7750 calls keeps them out of
// line (LTCG's inline choice follows the call's nesting): `if (&yaw_motion)` 648 (2441 bytes), on the second call only 844
// (2677), `if (sizeof(s_unit_motion))` (same code as the first on the kept form), float tests 639/653. That is a steering
// test, so nothing here is kept: BLOCKED-BY 0xc7750's inlining. Forms with the test, aligned count with stack slots and XMM
// numbers hidden in brackets (this body 292): `target` reusing the desired vector with matrix_transform_vector in place, a
// rotated copy `predicted` after normalize_inline(&axis), speeds zeroed in an else arm, late `inverse_rate` 642 (2440) [225]
// and with cross terms named 657 (2429) [211]; plus components read first in the to-frame helper 597 (2443) [151], without
// named crosses 585 (2454) [165]; read as i, j, k with named predicted angles 585 (2442) [134]; other read orders 599/599/605/
// 606; plus yaw and pitch as vector2f pairs (current, desired, speed, offset, new speed from 0xc7750's out argument, final
// angles) 534 (2437) [103]: limits then takes ECX as retail and speed and angles overlay the frame-local current as retail.
// Left: frame 0x84 against 0x80 (retail's order: offset sharing the lowest slot with the root and cosine temporaries, frame-
// local current, desired, current, axis, target, motions), scheduling. On it: the to-frame current written in the arm 533
// (2438) [107]; as a value-returning helper 617 (2475) [189]; scalar angles through pointer helpers 571 (2455) [145]; scalar
// new speeds 586 (2442) [134]; an angle-limit helper for both clamps 585 (same code); flag declaration orders 671/585/643;
// normalize3d 588 (2439); early offsets 585 (same code); to-frame and from-frame row orders 534 (same code); renames and
// to-angles, direction-from-pair and dot3f helpers: same frame order. Without the test: a plain scope, do/while(false) 1050
// (2825); the second call in the condition 1049 (2827); `!= false` 1065 (2844); forceinline or inline wrapper 1049/1049
// (2824); the rewrite on the old 0xc7750 624 (2441), `current` on the stack and both callers lost. Earlier steps without the
// test on the old 0xc7750: `target` reuse 563 (same code); in place 580 (2450); else-arm zeros 614 (2470), with both 638
// (2446); direction in `target` 565, with in place 582 (2453), all 640 (2449); prediction rewrite 644/669/621; preloaded
// components 578 (2451) and 646 (2508), ret 0x18. Body restored.
// M341: Complete current/desired/predicted double angle-result ownership, seven phase
// subsets then all const: 565/565/565/563/563/563/565/565. Exact-only result copies restored;
// frame 0x8c/0x80, input/vector homes and prediction/call scheduling remain.
// Round 84 (m3 lane 9, fast control 563): the calls now line up with retail's (same ten segments, near-equal
// instruction counts); at the 0xc7750 calls ours pushes position and velocity where retail passes them in XMM5/XMM4
// with time in XMM3, and so keeps yaw_done in a stack byte where retail copies it to BL. 0xc7750's accumulator form
// (28, retail's convention) is still inlined into both calls against today's body: 1103 (2870 bytes, 0x1faf80 0 ->
// 16); with both frame-to-world rotations through the existing matrix_transform_vector 1067 (2846), plus the
// to-frame rows through dot3f 1067 (2846): extra inline levels in this body do not use up the inliner's budget. On
// the current 0xc7750: both rotations through matrix_transform_vector 566 (2450 bytes); the target's in-place
// rotation alone 566 (2450). All restored; still BLOCKED-BY 0xc7750's inlining.
// Round 7 of #1049 (full statement walk of all 612 retail lines against this body, fast 563 unchanged): no logic
// difference. Verified: the to-frame rows (i,k,j / i,j,k / i,k,j orders and frame offsets 4/8/0xc, 0x10/0x14/0x18,
// 0x1c/0x20/0x24), wraps (limits[1]-limits[0]-2pi > -0.0001), the yaw/pitch clamp tails (retail jumps straight to the
// target construction when the pitch clamp fires, same as `unchanged = false`), cos(p)cos(y) / cos(p)sin(y) / sin(p),
// the in-place frame rotation of target then 0x30bf0, the speed test (|speed| < 0.0001 or == 0 zeroes both speeds,
// retail only on that path), the prediction rows (all three cross terms and operand orders), both atan2/sqrt
// pitch terms, `(x - current) * (1/rate)` with 1/rate divided late, the +-pi offset wrap, the five c7300 arguments
// (offset, speed, max_speed, acceleration, &motion), 0xc75d0's (yaw, pitch, acceleration), 0xc7750's arguments
// (motion in ECX, new speed pointer in EDX, offset pointer on the stack, XMM3 rate / XMM4 speed / XMM5 offset),
// both calls running before the `yaw_done && pitch_done` test, the done path (current = target, velocity =
// *0x4687a4), the yaw/pitch clamps without the `unchanged` flag, direction and next (speed * rate + angle),
// clamped dot in operand order i,k,j, the cross product signs, 0x30bf0, acos(dot) / rate capped at max_speed
// (0x322bd8 behind function_50650), the velocity scaling and the frame/no-frame tail. Constants checked in the XBE:
// 2pi 0x43f17c, pi 0x45dddc, -pi 0x45df50, 1.0 0x45dbc0, -1.0 0x45dbcc, +-0.0001 0x45dbdc/0x45ddfc. The 9 missing
// bytes are encodings (ours folds [edi] loads into mulss memory operands where retail preloads XMM0-2; stack
// layout 0x8c vs 0x80), not a statement. Retail's slot map (offsets after its `push ebx`): wraps byte 7; offsets 8/0xc;
// current-in-frame 0x10..0x18 reused for the speeds and later yaw/pitch; desired angles 0x1c/0x20; current angles
// 0x24/0x28 reused for the new speeds; velocity copy 0x2c..0x34 reused for next; desired-in-frame 0x38..0x40 reused for
// target then direction; motions 0x44/0x64.
// Inlining probes (accumulator 0xc7750, 28, 227 bytes): `static` on 0xc7750 1103 (same); 40 dead statements
// before the calls 1098 (2866, still inlined); 60 live statements (4059 bytes, still inlined: the caller's size does not
// matter); file flag /Ob1 1204 (1889 bytes: the static helpers and trig stop inlining); the c7300..c7750 sequence
// duplicated into the wraps and else arms (no inlining, 2575 bytes, the compiler does not merge them) 718. The inliner
// keeps the calls out of line only when they sit in a conditional block (round 197's tests), never from the size
// of the caller or the callee's linkage.
// MiniMax R (11-11): no new form tried this round, no lower score (563): retail tests its second pointer at the entry and leaves the body when it is null, this build stores two zeroed speeds first and only then branches, and the frame is 0x8c against retail's 0x80.
// MiniMax B (11-13): no new form, no lower score (563): retail tests its second pointer at the entry and leaves the body when it is null, ours stores two zeroed speeds first and only then branches; the frame is 0x8c against retail's 0x80.
// MiniMax G (11-13): no new form, no lower score (563): the frame is still 0x8c against retail's 0x80, and the
// two 0xc7750 calls still pass their arguments on the stack where retail passes them in xmm registers; the
// accumulator form of 0xc7750 that gives 28 is inlined into this body, so neither change is kept.
// MiniMax A (11-13): no new form tried this round, no lower score (563): retail tests its second pointer at the
// entry and leaves the body when it is null, this build stores two zeroed speeds first and only then branches,
// and the frame is 0x8c against retail's 0x80; the two 0xc7750 calls still push position and velocity on the
// stack where retail passes them in xmm5 and xmm4: BLOCKED-BY 0xc7750's inlining.
// @retail 0xc7840
void function_c7840(transform4x3f const *frame, vector3f *current, vector3f const *desired, real rate, vector3f *velocity,
	real const *limits, real max_speed, real acceleration)
{
	real yaw_speed = 0.0f;
	real pitch_speed = 0.0f;
	vector3f local_263186;
	vector3f local_0588e7;

	if (frame)
	{
		unit_vector_to_frame(&local_263186, frame, current);
		unit_vector_to_frame(&local_0588e7, frame, desired);
	}
	else
	{
		local_263186 = *current;
		local_0588e7 = *desired;
	}
	bool wraps = limits[1] - limits[0] - 6.2831855f > -0.0001f;
	real current_yaw = (real)atan2(local_263186.j, local_263186.i);
	real current_pitch = (real)atan2(local_263186.k,
		(real)sqrt(local_263186.j * local_263186.j + local_263186.i * local_263186.i));
	real desired_yaw = (real)atan2(local_0588e7.j, local_0588e7.i);
	real desired_pitch = (real)atan2(local_0588e7.k,
		(real)sqrt(local_0588e7.j * local_0588e7.j + local_0588e7.i * local_0588e7.i));
	bool unchanged = true;

	if (wraps)
	{
		if (limits[0] > desired_yaw)
		{
			desired_yaw += 6.2831855f;
		}
		else if (desired_yaw > limits[1])
		{
			desired_yaw -= 6.2831855f;
		}
	}
	else if (limits[0] > desired_yaw)
	{
		desired_yaw = limits[0];
		unchanged = false;
	}
	else if (desired_yaw > limits[1])
	{
		desired_yaw = limits[1];
		unchanged = false;
	}
	if (limits[2] > desired_pitch)
	{
		desired_pitch = limits[2];
		unchanged = false;
	}
	else if (desired_pitch > limits[3])
	{
		desired_pitch = limits[3];
		unchanged = false;
	}
	vector3f target;
	if (unchanged)
	{
		target = *desired;
	}
	else
	{
		unit_direction_from_angles(&target, desired_yaw, desired_pitch);
		if (frame)
		{
			vector3f world;

			unit_vector_from_frame(&world, frame, &target);
			target = world;
			function_30bf0(&target);
		}
	}
	vector3f angular_velocity = *velocity;
	real speed = (real)sqrt(angular_velocity.i * angular_velocity.i + angular_velocity.k * angular_velocity.k + angular_velocity.j * angular_velocity.j);
	if (!(0.0001f > (real)fabs(speed)) && speed != 0.0f)
	{
		real inverse_rate = 1.0f / rate;
		real inverse = 1.0f / speed;
		vector3f axis = angular_velocity;
		vector3f predicted;
		real angle = speed * rate;
		real sine = (real)sin(angle);
		real cosine = (real)cos(angle);

		axis.i = inverse * axis.i;
		axis.j = axis.j * inverse;
		axis.k = axis.k * inverse;
		real along = (axis.i * local_263186.i + axis.k * local_263186.k + axis.j * local_263186.j) * (1.0f - cosine);
		predicted.i = local_263186.i * cosine + along * axis.i -
			(local_263186.j * axis.k - local_263186.k * axis.j) * sine;
		predicted.j = axis.j * along + local_263186.j * cosine - (local_263186.k * axis.i - axis.k * local_263186.i) * sine;
		predicted.k = axis.k * along + local_263186.k * cosine - (axis.j * local_263186.i - axis.i * local_263186.j) * sine;
		yaw_speed = ((real)atan2(predicted.j, predicted.i) - current_yaw) * inverse_rate;
		pitch_speed = ((real)atan2(predicted.k, (real)sqrt(predicted.i * predicted.i + predicted.j * predicted.j)) -
			current_pitch) * inverse_rate;
	}
	real yaw_offset = current_yaw - desired_yaw;
	real pitch_offset = current_pitch - desired_pitch;
	if (wraps)
	{
		if (yaw_offset > 3.1415927f)
		{
			yaw_offset -= 6.2831855f;
		}
		else if (-3.1415927f > yaw_offset)
		{
			yaw_offset += 6.2831855f;
		}
	}
	s_unit_motion yaw_motion;
	s_unit_motion pitch_motion;
	function_c7300(yaw_offset, yaw_speed, max_speed, acceleration, &yaw_motion);
	function_c7300(pitch_offset, pitch_speed, max_speed, acceleration, &pitch_motion);
	function_c75d0(&yaw_motion, &pitch_motion, acceleration);
	bool yaw_done = function_c7750(&yaw_motion, yaw_offset, yaw_speed, rate, &yaw_speed, &yaw_offset);
	bool pitch_done = function_c7750(&pitch_motion, pitch_offset, pitch_speed, rate, &pitch_speed, &pitch_offset);
	if (yaw_done && pitch_done)
	{
		*current = target;
		*velocity = *g_4687a4;
		return;
	}
	real yaw = yaw_offset + desired_yaw;
	real pitch = pitch_offset + desired_pitch;
	if (wraps)
	{
		if (limits[0] > yaw)
		{
			yaw += 6.2831855f;
		}
		else if (yaw > limits[1])
		{
			yaw -= 6.2831855f;
		}
	}
	else if (limits[0] > yaw)
	{
		yaw = limits[0];
	}
	else if (yaw > limits[1])
	{
		yaw = limits[1];
	}
	if (limits[2] > pitch)
	{
		pitch = limits[2];
	}
	else if (pitch > limits[3])
	{
		pitch = limits[3];
	}
	vector3f direction;
	vector3f next;
	unit_direction_from_angles(&direction, yaw, pitch);
	unit_direction_from_angles(&next, yaw_speed * rate + yaw, pitch_speed * rate + pitch);
	real dot = next.i * direction.i + next.k * direction.k + next.j * direction.j;
	if (-1.0f > dot)
	{
		dot = -1.0f;
	}
	else if (dot > 1.0f)
	{
		dot = 1.0f;
	}
	velocity->i = direction.j * next.k - direction.k * next.j;
	velocity->j = direction.k * next.i - direction.i * next.k;
	velocity->k = direction.i * next.j - direction.j * next.i;
	function_30bf0(velocity);
	real turn = function_50650(dot) / rate;
	if (turn > max_speed)
	{
		turn = max_speed;
	}
	velocity->i *= turn;
	velocity->j *= turn;
	velocity->k *= turn;
	if (frame)
	{
		unit_vector_from_frame(current, frame, &direction);
		function_30bf0(current);
	}
	else
	{
		*current = direction;
	}
}

/* the unit's reaction to a damage report: the campaign's explosive
   grenade chain, the player controller's report, its shield recharge
   delay and stun, the timer +0x2bc, its death direction, the player and
   betrayal bookkeeping, the hit animation, the actor's perception of the
   attacker, its camera shake and, when it was a killing blow, its death */
// score 116 (was 129; earlier 383, 399): only register allocation is left. Retail keeps unit_index in ebx (reloaded after the grenade
// score 129 (was 383; earlier 399): only register allocation is left. Retail keeps unit_index in ebx (reloaded after the grenade
// block), report in ebp and the unit in esi, and spills the definition to [esp+0x10]; ours spills unit_index and the
// definition and uses ebx and edi as temporaries, so the prologue order, the stack slots (retail: definition 0x10,
// total 0x18, unit 0x1c, hard 0x20) and the byte re-load of flags for the sign test differ. Retail passes the bit of
// flags to function_15cbf0 masked with 0xffffff01, as a bool (its parameter and c_engine_peer::p29's are bool, 379 -> 370).
// Tried: definition written out at every use (605), unit declared after the other locals (353), total as one sum
// expression (367; it loads +0x44 first, so total is built with += to load +0x48 first), a (bool) cast on the
// function_15cbf0 argument (343, no change).
// Round 6 of #1049 (full statement walk against retail: the 19 calls, their order, every field offset, constant,
// condition direction and the switch table (cases 0-4, 6, 8, 9) agree; the 12 missing bytes are encodings, not a
// statement: 3 for `and edx, 0xffffff01` vs ours `and edx, 1` on the function_15cbf0 argument, the rest register moves).
// Two real differences. (1) Fixed (behaviour): retail passes function_ca700's last argument as the raw byte
// `report->unknown00` (`xor ecx, ecx; mov cl, [ebp]`; ca700 stores that byte at unit+0x330+i unchanged), where the
// source converted it to a bool (`test cl, cl; setne cl`), storing a nonzero byte other than 1 as 1. ca700's `flag` is
// a `byte` and `unknown330` a `byte[4]` now (ca700 stays 140, d9640 stays MATCH); this function's score goes 344 -> 379
// because the call is one instruction shorter and the positional score shifts (without the array change ca700 grows to
// 517). (2) Retail's function_15cbf0 takes a bool (`and 0xffffff01`): fixed, 379 -> 370 (0x15cbf0 still matches).
// What decides the rest: retail enregisters unit_index in ebx from entry (`mov ebx, [esp+0xc0]`; the non-grenade path
// reaches the badc0 call with it, only the grenade path reloads it) and spills the definition (reloaded from [esp+0x10]
// in five places, edi holds attacker_index); ours loads the argument straight into eax, uses bl for the hard temp,
// reloads ebx at the join and keeps the definition in edi. Retail also loads the flags byte twice (hard, then the sign
// test) where ours shares one load. Tried: all 24 orders of the unit/definition/hard/total declarations (with the byte
// flag: 379-390; 379 for unit first with definition before hard), `hard` as `(flags & 1) != 0` (344) or `== 1` (442).
// Round 85 (m3 lane 11, fast and plain; header before this round: score 370 (was 413)): the player index through the existing unit_get_player_index for both the test
// and the controller lookup (retail's NONE-first `or ecx, -1` shape of the inlined helper; matched 0xd03e0's form) 399 ->
// 438 (1586) alone; the sign test as the mask `report->flags & 0x80` (retail reads the byte again and tests it with
// `test dl, dl; jns`) 400 alone, both together 383 (1610): kept. Then (no gain, same code unless noted): the owner's
// object index stored before the player index (projectiles.cpp's order; retail stores +0xc8 first) 383; the switch on
// `report->unknown00 & 0x3f` with cases 0x1d.. 383, with the owner order 383; the 0x1147e0 block reading report->flags at
// each use 383; `hard` through TEST_FIELD_BIT 383. Left: retail gives unit_index EBX (reloaded after the grenade
// block) and keeps the definition in memory; ours gives the definition EBX and unit_index EDI there; slot order.
// Round 232 (m3 lane 3): 383 -> 129 (1602 bytes, retail's length) with the shake's range as a named local, read once
// under the other three tests and used for its own test, the subtraction and the division (`real range = ...0x58;
// if (range > report->distance) {...}`): the definition then stays in memory as in retail (masked aligned 56 -> 15).
// The same local with the test still reading the field (a mix) 127. Before it: the grenade owner through the existing
// function_bc190(unit_index, &owner) 478 (1546 bytes: it stays a call, its file is /Ob1); the shield maximum as
// `if (unit->unknown2cc > shield)` 383 (same code); a diagnostic only (reverted): the definition as a volatile local 268;
// the type word read once at the top into a short for its three uses 366 (aligned worse, a declaration moved up).
// On 129: the owner stored in function_bc190's order (object, player, team) 125, not kept (statement order); the
// shake's minimum named 75 positional only (aligned 47 unchanged, masked 15 -> 18), not kept; the add rate named before
// its test 129; `add < 0.0f` / `cap < 0.0f` 129; the cap's upper test without else 129; the shake passed straight to
// 0x1c9e10 129; `minimum > unit->unknown2e8` 129. Left: retail reads the shake's fields through a second copy of the
// definition pointer (`mov eax, edx`, no shared loads); the late blocks' ECX/EDX roles; the 0xc98a0 call sets its ECX
// argument before the pushes (a parameter-order lead for 0xc98a0, whose matched caller 0xc42e0 would have to follow);
// the owner store order; the damage NONE/byte store order. BLOCKED-BY 0xbc190's /Ob1 file for the owner fill.
// Round 234 (m3 lane 3): 129 -> 116 by the damage's NONE material store before function_b9dd0/function_d6660, where
// retail stores it (d6660 then clears the record and sets its own default, so retail's damage keeps that default);
// between the two calls 120. On 116: `inline` on 0xbc190's definition with the owner through function_bc190 141 (1598
// bytes; the grenade block matches, but 0xfaa60 inlines it too and rises 1220 -> 1275); the owner in 0xbc190's order
// (re-measure) 112; a named `distance` for the shake 61 positional only (aligned 46 unchanged, masked 14 -> 15, an extra
// register load); the stun's globals read at each use 93 (1606 bytes, masked 16); the shake test with its sides swapped,
// `scale * def[0x34]`, `x = x + add`, the minimum first in the shake sum and the grenade globals at each use 116 (same
// code); a named fraction 125 (1598 bytes, masked 12) and the shake in two steps 125.
// Combinations: distance + fraction 63 (aligned 45, masked 14, still the extra distance load), with the owner order 59
// (masked 10); globals at each use + fraction 138, + distance 122 (1610 bytes), all three 93; distance + owner order
// 57 (masked 11), fraction + owner order 121 (1598 bytes, masked 8), globals + owner order 89 (1606 bytes); the cap's
// upper test as `cap > 1.0f` 116 (other code, masked 15). The low counts are positional; only the owner order (a
// statement order, kept only with a match) and the fraction's stun multiply order move masked lines.
// @retail 0xca0b0
void __stdcall function_ca0b0(long unit_index, s_damage_report const *report)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = g_4e3b44[report->definition_index & 0xffff].bytes;
	bool hard = report->flags & 1;
	real total = report->unknown48;

	/* retail loads +0x48 first; a single sum expression is emitted with +0x44 first */
	total += report->unknown44;

	if (report->flags & 0x80)
	{
		unit->flags_134 |= 0x8000000;
		if (g_4e6948->state == 1 && g_4f55dc[1])
		{
			byte *globals = (byte *)g_4e034c;
			byte *grenade = *(void **)(globals + 0x100) ? *(byte **)(globals + 0x104) + 0x2c : 0;
			byte *grenade_definition = g_4e3b44[*(long *)(grenade + 0x20) & 0xffff].bytes;
			long damage_definition_index = *(long *)(grenade_definition + 0x110);
			s_damage_owner owner;

			/* retail re-reads the unit from the object table here instead of using its register */
			owner.player_index = UNIT_GET(unit_index)->unknownc4;
			owner.object_index = UNIT_GET(unit_index)->unknownc8;
			owner.team = UNIT_GET(unit_index)->unknownc2;
			if (damage_definition_index != NONE)
			{
				point3f center;
				s_type_1e6529 damage;

				/* retail sets the material before the record is initialized, which then replaces it with its default */
				damage.material_index = NONE;
				function_b9dd0(unit_index, &center);
				function_d6660(&damage, damage_definition_index);
				damage.owner = owner;
				*(byte *)&damage.unknown84 = 4;
				damage.unknown54 = 1.0f;
				object_get_root_location(unit_index, (s_location *)&damage.unknown1c);
				damage.origin = center;
				damage.position = center;
				function_d6c80(&damage, NONE);
			}
			function_176780(unit_index, (s_effect_owner const *)&owner, 0.0f, *(long *)(grenade_definition + 0x128), 0.0f, 0,
				0);
		}
	}
	if (unit_get_player_index(unit_index) != NONE)
	{
		long player_index = unit_get_player_index(unit_index);
		long controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

		if (controller != NONE)
		{
			function_153d10(controller, report->definition_index, (void *)&report->owner, (void *)&report->direction, 0,
				report->scale, report->unknown38, (report->flags >> 3) & 1);
		}
	}
	real shield = unit->unknown100 + unit->unknownfc;
	if (shield > 0.0f)
	{
		unit->unknown2c8 = *(word *)(definition + 0x12);
		unit->unknown2ca = (short)unit_round(g_510c54->field_2_3 * 1.5f);
		if (shield < unit->unknown2cc)
		{
			shield = unit->unknown2cc;
		}
		unit->unknown2cc = shield;
		if (report->owner.object_index != NONE)
		{
			unit->unknown2d0 = report->owner.object_index;
		}
	}
	if (report->unknown44 > 0.0f || report->unknown48 > 0.0f)
	{
		function_d0e60(unit_index, *(real *)(definition + 0x30), 0.0f);
	}
	if (report->unknown50 > 0)
	{
		function_c98a0((point2f const *)&report->direction, unit_index, report->unknown50, (word)report->unknown3c);
	}
	if (report->owner.player_index != NONE && unit->unknown13c != NONE &&
		report->unknown48 + report->unknown44 > 0.0001f)
	{
		function_15cbf0(report->owner.player_index, unit->unknown13c, (report->flags >> 4) & 1);
	}
	if (report->owner.player_index != NONE || report->owner.object_index != NONE)
	{
		function_ca700(unit_index, total, *(short *)(definition + 0x12), hard, report->owner.player_index,
			report->owner.team, report->owner.object_index, report->unknown00);
	}
	dword flags = report->flags;
	if (!(flags & 0x20) && (flags & 1 || report->unknown44 > 0.0f || report->unknown48 > 0.0f))
	{
		function_1147e0(unit_index, (flags >> 6) & 1, unit->unknownf8, unit->unknownf4, report->definition_index, hard);
	}
	if (report->unknown44 > 0.0f || report->unknown48 > 0.0f)
	{
		function_c86e0(unit_index, false);
	}
	long attacker_index = report->owner.object_index;
	s_unit *attacker = (s_unit *)function_badc0(attacker_index, 3);
	if (unit->actor_index != NONE || (attacker && attacker->actor_index != NONE))
	{
		short type = *(short *)(definition + 0x12);

		switch ((report->unknown00 & 0x3f) - 0x1d)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
		case 8:
		case 9:
			if ((report->unknown00 & 0xc0) == 0xc0)
			{
				type = 9;
			}
			break;
		}
		if (hard)
		{
			function_1c95d0(unit_index, attacker_index, type, total);
		}
		else if (!TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2))
		{
			function_1c9c80(unit_index, attacker_index, type, total, (long)&report->direction, false);
		}
	}
	if (g_4e6948->mode != 4 && unit->actor_index != NONE && report->flags & 4)
	{
		real range = *(real *)(definition + 0x58);

		if (range > report->distance)
		{
			real shake = (range - report->distance) / range *
				(*(real *)(definition + 0x60) - *(real *)(definition + 0x5c)) + *(real *)(definition + 0x5c);

			function_1c9e10(unit_index, &report->direction, shake);
		}
	}
	if (unit->unknown13c != NONE && *(real *)(definition + 0x34) > 0.0f && g_4e6948->state == 2)
	{
		byte *globals = *(byte **)((byte *)g_4e034c + 0x134);
		real add = *(real *)(definition + 0x34) * report->scale;
		real cap = *(real *)(definition + 0x38) * report->scale;

		if (0.0f > add)
		{
			add = 0.0f;
		}
		if (0.0f > cap)
		{
			cap = 0.0f;
		}
		else if (cap >= 1.0f)
		{
			cap = 1.0f;
		}
		if (cap > unit->unknown2e4)
		{
			unit->unknown2e4 += add;
			if (unit->unknown2e4 > cap)
			{
				unit->unknown2e4 = cap;
			}
		}
		short ticks = (short)unit_round(g_510c54->field_2_3 * *(real *)(definition + 0x3c));
		short minimum = (short)unit_round(g_510c54->field_2_3 * *(real *)(globals + 0x84));
		short maximum = (short)unit_round(g_510c54->field_2_3 * *(real *)(globals + 0x88));
		if (unit->unknown2e8 < minimum)
		{
			unit->unknown2e8 = minimum;
		}
		unit->unknown2e8 += ticks;
		if (unit->unknown2e8 > maximum)
		{
			unit->unknown2e8 = maximum;
		}
	}
	if (hard)
	{
		function_caa60(unit_index, report->unknown40, (point3f const *)&report->direction, false,
			(report->flags >> 10) & 1);
	}
}

typedef char unit_state_size_check[sizeof(s_unit_state_c6ef0) == 0x7c ? 1 : -1];
typedef char unit_motion_offset_check[offsetof(s_unit_motion, s_type_8c87de) == 0x1c ? 1 : -1];
typedef char unit_motion_position_check[offsetof(s_unit_motion, position) == 0x4 ? 1 : -1];
typedef char unit_damage_size_check[sizeof(s_type_1e6529) == 0x88 ? 1 : -1];
typedef char unit_damage_report_check[offsetof(s_damage_report, unknown50) == 0x50 ? 1 : -1];

long function_10f5f0(long object_index);
long function_10f720(long object_index, bool first);
bool function_f42f0(long vehicle_index);
bool function_1143d0(long unit_index);
bool function_a76b0(long unit_index, long flag);
void __stdcall function_a9260(long unit_index, struct s_unit_melee_hit const *hit, long damage_index);
extern short g_47d8e0;

/* the unit's control input for a tick: its wandering direction (flag 21)
   or its facing (through its parent's node) as its desired aim and look;
   scripted controls (+0x1ec, +0x1f0); for a vehicle, its driver's
   controls and its gunner's aim and triggers when their animations allow;
   then its boost charge (+0x334, +0x338); whether anything changed */
// MATCH (was 204; 419 before round 4 of #1049). Round 5 of #1049: the gunner block's copy_controls in the
// else arm is `!leaving && !entering` evaluated again, not a copy of copy_aim (retail recomputes the
// two tests, which also put `leaving` in bl and the gunner pointer on the stack as retail), and entering is
// declared before leaving (retail zeroes entering's byte first). Earlier attempts at the allocation by
// declaration order (leaving/entering swapped, state before the gunner, copy flags first) were 204-209 while
// the copy was still there.
// @retail 0xc49b0
bool __stdcall function_c49b0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	bool changed = false;

	function_d0d20(unit_index);
	if (TEST_FIELD_BIT((unit->flags_134 >> 21) & 1))
	{
		function_d0080(unit_index, &unit->unknown150);
		unit->unknown15c = unit->unknown150;
		unit->unknown180 = unit->unknown150;
		unit->unknown1b0 = *g_4687a8;
		changed = true;
		unit->unknown148 = 0;
	}
	else if (TEST_FIELD_BIT((unit->flags_134 >> 1) & 1))
	{
		changed = true;
	}
	else
	{
		s_unit *object = UNIT_GET(unit_index);
		vector3f direction;

		if (object->parent_index == NONE)
		{
			direction = object->forward;
		}
		else
		{
			s_unit *parent = UNIT_GET(object->parent_index);
			transform4x3f const *node = (transform4x3f const *)((byte *)parent + *(short *)((byte *)parent + 0x116) +
				*((char *)object + 0x18) * 0x34);

			real i = object->forward.i;
			real j = object->forward.j;
			real k = object->forward.k;

			direction.i = node->up.i * k + node->left.i * j + node->forward.i * i;
			direction.j = node->up.j * k + node->left.j * j + node->forward.j * i;
			direction.k = node->up.k * k + node->left.k * j + node->forward.k * i;
		}
		unit->unknown180 = direction;
		unit->unknown15c = direction;
		unit->unknown150 = direction;
		unit->unknown1b0 = *g_4687a4;
		unit->unknown148 = 0;
	}
	if (!(TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 11) & 1)))
	{
		bool frozen = TEST_FIELD_BIT((unit->flags_134 >> 22) & 1);

		if (unit->unknown1ec > 0)
		{
			real fire;

			unit->unknown148 |= unit->unknown1f0;
			if (unit->unknown1f0 & 0x10000)
			{
				long period = unit_round(g_510c54->field_2_3 * 0.25f);

				if (unit->unknown1ec % period == 0)
				{
					unit->unknown148 |= 0x10000;
				}
				else
				{
					unit->unknown148 &= ~0x10000;
				}
				fire = 1.0f;
			}
			else
			{
				fire = 0.0f;
			}
			*(real *)&unit->unknown1c0 = fire;
			if (--unit->unknown1ec == 0)
			{
				unit->unknown1f0 = 0;
			}
			changed = true;
		}
		if (!frozen)
		{
			long driver_index = unit->unknown248;

			if (driver_index != NONE && !(TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2)))
			{
				s_unit *driver = UNIT_GET(driver_index);
				long state = function_10f5f0(driver_index);
				bool copy;

				if (function_0c7070(driver_index))
				{
					long phase = function_10f720(driver_index, true);

					if (driver->unknown13c != NONE && !*((byte *)g_4e8c20 + 6))
					{
						copy = phase != 2;
					}
					else
					{
						copy = phase == 1;
					}
				}
				else if (state == 0x400004a || state == 0x80000c4)
				{
					copy = function_10f720(driver_index, true) == 2;
				}
				else
				{
					copy = true;
				}
				if (copy)
				{
					unit->unknown148 |= driver->unknown148 & 0xffff;
					unit->unknown150 = driver->unknown150;
					unit->unknown1b0 = driver->unknown1b0;
				}
				changed = true;
			}
			if (unit->unknown24c != NONE && !(TEST_FIELD_BIT(((s_unit_flags_10a_view *)&unit->flags_10a)->bit2)) && function_1143d0(unit_index))
			{
				long gunner_index = unit->unknown24c;
				s_unit *gunner = UNIT_GET(gunner_index);
				long state = function_10f5f0(gunner_index);
				bool copy_aim;
				bool copy_controls;
				bool entering = false;
				bool leaving = false;

				if (function_0c7070(gunner_index))
				{
					long phase = function_10f720(gunner_index, true);

					entering = phase == 2 || phase == 0;
				}
				else if (state == 0x400004a || state == 0x80000c4)
				{
					long phase = function_10f720(gunner_index, true);

					leaving = phase == 1 || phase == 0;
				}
				if (gunner->unknown13c != NONE && !*((byte *)g_4e8c20 + 6))
				{
					copy_aim = !leaving;
					copy_controls = !leaving && !entering;
				}
				else
				{
					copy_aim = !leaving && !entering;
					copy_controls = !leaving && !entering;
				}
				if (copy_aim)
				{
					unit->unknown15c = gunner->unknown15c;
					unit->unknown180 = gunner->unknown15c;
				}
				if (copy_controls)
				{
					unit->unknown148 |= gunner->unknown148 & 0xffff0000;
					unit->unknown148 |= gunner->unknown148 & 0x20;
					unit->unknown1c0 = gunner->unknown1c0;
				}
				changed = true;
			}
			if (unit->unknown148 & 0xffff0000)
			{
				unit->unknown244 = g_510c54->game_time;
				changed = true;
			}
		}
	}
	bool boosting = (unit->type == 1 && function_f42f0(unit_index)) || unit->unknown148 & 0x800;
	if (TEST_FIELD_BIT((*(dword *)(definition + 0xbc) >> 28) & 1) && (boosting || unit->unknown334 > 0.0f))
	{
		if (unit->unknown338 == 0.0f)
		{
			real full = *(real *)(definition + 0x1d8) + *(real *)(definition + 0x1d4);
			if (boosting)
			{
				unit->unknown334 += g_510c54->rate;
				if (full > unit->unknown334)
				{
					return true;
				}
			}
			real recover;
			real cooldown;
			if (full > unit->unknown334)
			{
				if (*(real *)(definition + 0x1d4) > unit->unknown334)
				{
					recover = unit->unknown334 / *(real *)(definition + 0x1d4) * *(real *)(definition + 0x1dc);
				}
				else
				{
					recover = *(real *)(definition + 0x1dc);
				}
				cooldown = (unit->unknown334 + recover) * *(real *)(definition + 0x1e0) / (*(real *)(definition + 0x1dc) + full);
			}
			else
			{
				recover = *(real *)(definition + 0x1dc);
				cooldown = *(real *)(definition + 0x1e0);
			}
			if (function_cc380(unit_index))
			{
				cooldown = 0.0001f;
			}
			unit->unknown334 = cooldown + recover;
			unit->unknown338 = cooldown;
			return true;
		}
		unit->unknown334 -= g_510c54->rate;
		if (0.0f >= unit->unknown334)
		{
			unit->unknown334 = 0.0f;
			unit->unknown338 = 0.0f;
		}
		return true;
	}
	return changed;
}

/* the collision test result 0x1697c0 fills (unknown_1689b0.cpp's layout) */
struct s_unit_melee_collision
{
	long type;
	real t;
	point3f point;
	byte start_location[8];
	byte end_location[8];
	short material_type;
	byte unknown26[2];
	vector3f normal;
	real distance;
	long instance_index;
	long unknown3c;
	long object_index;
	byte unknown44[0x4c - 0x44];
	long bsp_surface_reference[3];
	byte bsp_surface_flags[2];
	short bsp_surface_index;
};

/* the unit's melee strike: casts a 5 by 5 fan of rays over its weapon's
   melee box along its aim, keeping the best hit (a biped first, else the
   nearest object; the first surface otherwise), picks the melee damage by
   the attack's name (or the seat's, for a seat group), damages what it
   hit (through the player code when it asks), plays the impact and
   applies the second damage to the striker */
// score 493 (was 504; 507 before): the melee damage and self damage are set only when the melee table entry exists
// (retail reaches the weapon's defaults with NONE still in both), hit_t is not initialised (retail has no store
// for it), and the damage data's material_index is set to NONE before 0xd6660 (retail order). Retail's tail
// shapes (the controller as a long, the player pointer taken before the rounding) cost 7 through slot offsets
// (511) at the time; they are kept since round 6 (below), when they stopped costing.
// Still differs: register allocation (retail: EBX unit, ESI weapon definition, EDI &aim, EBP scratch, the
// unit index read from the stack each time; ours keeps the unit index in EDI and the unit in EBP), short
// locals copied as words by retail (material_index) but as dwords by ours, and the callee-saved registers
// retail pops before the player-timer tail. Tried: aim read as unit->unknown168 (548 with range inlined, 518
// otherwise), range read at each use (+30), hit_t = 0.0f (+6).
// Round 180: the impact's material test through the existing global_material_get 504 (2209 bytes, kept).
// Round 6 of #1049 (504 -> 493, 2220 bytes vs retail 2229): a statement walk found these logic differences:
// (1) the biped-preference test is `type == 0 && hit_type == 0 && hit_t > t` OR `type == 0 && hit_type != 0`
// (retail tests the type twice and the hit_t compare comes first), with hit_type taken as a signed char (movsx);
// (2) s_damage_owner has a zero short after the team that retail writes (`owner.unknown0a = 0`, as the
// damage.cpp layout has); (3) the melee hit's seat group byte is stored as a plain byte (the header types it bool,
// so a plain assignment adds a setne); (4) the rounding tail's controller is a long (movsx) and the player
// pointer is taken first (the tail's instructions now equal retail's). Not kept, to avoid raising the score:
// TEST_FIELD_BIT around the two seat-flag tests (retail loads the dword, shr 11, test 1 at both: those exact
// instructions, but the score goes 493 -> 526, 2229 bytes; only one of the two: 504/525); `long weapon_index =
// NONE; if (index != NONE) weapon_index = ...` (retail does set eax = ebp = NONE before its je; 514); position
// before origin (493, same); aim read from the unit for ray.j/.k as retail's addressing does ([ebx+0x16c] while
// i goes through edi=&aim; 533-543, with unit in ebx but j in a register and -1 no longer in ebp); aim declared
// before the weapon definition (543); an unknown26 short in the collision struct (no change).
// Still differs (register allocation, frame 0x180 vs 0x17c): retail keeps the unit in ebx, the weapon definition
// in esi, &aim in edi and -1 in ebp with both loop counters on the stack; ours spills the unit and keeps j in ebx.
// Retail also loads g_4e3b44 and g_4e0300 into esi/edi up front and compares the weapon slot with `cmp cx, bp`.
// Round 85 (m3 lane 11, fast): the self-damage controller's player index through the existing unit_get_player_index
// (test and lookup, the form that helped 0xca0b0) 523 (2218).
// @retail 0xcf3d0
void __stdcall function_cf3d0(long unit_index, long name, byte flags, real scale)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *local_98b918_2 = UNIT_DEFINITION_GET(unit);
	short index = unit->current_weapon_index;
	long weapon_index = index != NONE ? unit->weapon_object_indices[index] : NONE;
	short material_index = g_47d8e0;
	point3f point = unit->position;
	vector3f normal = *g_4687b0;
	long hit_index = NONE;
	char hit_seat = 3;
	long self_damage_index = NONE;
	short surface_type = NONE;
	long surface_a = NONE;
	long surface_b = NONE;
	long damage_index = NONE;

	if (weapon_index != NONE)
	{
		byte *local_67e06b = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));
		short hit_type = NONE;
		real hit_t;
		point3f center;
		vector3f across;
		vector3f up;
		vector3f const *aim = &unit->unknown168;

		hit_seat = local_67e06b[0x1fc] & 0x3f;
		function_cafc0(unit_index, &center);
		function_30bf0(function_11d000(aim, &across));
		up.i = aim->j * across.k - aim->k * across.j;
		up.j = aim->k * across.i - aim->i * across.k;
		up.k = aim->i * across.j - aim->j * across.i;
		real width = *(real *)(local_67e06b + 0x1b0) * 0.5f;
		real height = *(real *)(local_67e06b + 0x1b4) * 0.5f;
		real range = *(real *)(local_67e06b + 0x1b8);
		across.i *= width;
		across.j *= width;
		across.k *= width;
		up.i *= height;
		up.j *= height;
		up.k *= height;
		for (long i = -2; i <= 2; i++)
		{
			for (long j = -2; j <= 2; j++)
			{
				vector3f ray;
				s_unit_melee_collision collision;

				ray.i = (real)i * across.i + (real)j * up.i + aim->i * range;
				ray.j = (real)j * up.j + (real)i * across.j + aim->j * range;
				ray.k = (real)j * up.k + (real)i * across.k + aim->k * range;
				collision.material_type = NONE;
				if (!function_1697c0(0x2480000f, &center, &ray, unit_index, NONE, (s_collision_result_1697c0 *)&collision))
				{
					continue;
				}
				point = collision.point;
				normal = collision.normal;
				switch (collision.type)
				{
				case 1:
				case 3:
					if (hit_index == NONE)
					{
						material_index = collision.material_type;
						if (collision.bsp_surface_flags[0] & 8)
						{
							surface_type = collision.bsp_surface_flags[1];
							surface_a = collision.unknown3c;
							surface_b = collision.bsp_surface_reference[1];
						}
					}
					break;
				case 4:
				{
					long object_index = collision.object_index;
					s_unit *object = UNIT_GET(object_index);

					if (object->type == 2 && object->parent_index != NONE)
					{
						object_index = object->parent_index;
						object = UNIT_GET(object_index);
					}
					if (hit_index == NONE || (object->type == 0 && hit_type == 0 && hit_t > collision.t) ||
						(object->type == 0 && hit_type != 0))
					{
						hit_index = object_index;
						hit_type = (char)object->type;
						material_index = collision.material_type;
						hit_t = collision.t;
					}
					break;
				}
				}
			}
		}
		byte *melee = 0;
		switch (name)
		{
		case 0x500000a:
		case 0xc000073:
		case 0xe000607:
		case 0xe000608:
		case 0xe000609:
		case 0xe00060a:
			melee = local_67e06b + 0x1bc;
			break;
		case 0xc000075:
			melee = local_67e06b + 0x1cc;
			break;
		case 0xc000077:
			melee = local_67e06b + 0x1dc;
			break;
		case 0xb0005b2:
		case 0x140005b3:
			melee = local_67e06b + 0x1ec;
			break;
		}
		if (melee)
		{
			damage_index = *(long *)(melee + 4);
			self_damage_index = *(long *)(melee + 0xc);
		}
		if (damage_index == NONE)
		{
			damage_index = *(long *)(local_67e06b + 0x190);
		}
		if (self_damage_index == NONE)
		{
			self_damage_index = *(long *)(local_67e06b + 0x198);
		}
		if (damage_index == NONE)
		{
			damage_index = *(long *)(local_98b918_2 + 0x168);
		}
	}
	else
	{
		hit_index = unit->parent_index;
		if (hit_index == NONE || unit->parent_seat_index == NONE)
		{
			goto impact;
		}
		byte *parent_definition = UNIT_DEFINITION_GET(UNIT_GET(hit_index));
		if (!((*(dword *)&UNIT_SEATS(parent_definition)[unit->parent_seat_index].flags >> 11) & 1))
		{
			goto impact;
		}
		damage_index = *(long *)(local_98b918_2 + 0x170);
		self_damage_index = *(long *)(local_98b918_2 + 0x178);
		long graph_index = *(long *)(parent_definition + 0x38);
		if (graph_index != NONE)
		{
			byte *graph = g_4e3b44[graph_index & 0xffff].bytes;

			if (*(long *)(graph + 0x60))
			{
				material_index = *(short *)(*(byte **)(graph + 0x64) + 0xce);
			}
		}
	}
	if (damage_index == NONE || (hit_index == NONE && surface_type == NONE))
	{
		goto impact;
	}
	{
		s_unit_melee_hit hit;

		hit.object_index = hit_index;
		hit.unknown04 = surface_type;
		hit.unknown08 = surface_a;
		hit.unknown0c = surface_b;
		hit.material_index = material_index;
		/* the header types this byte as a bool, but retail stores the seat group byte unchanged */
		*(char *)&hit.unknown18 = hit_seat;
		hit.unknown14 = 0.0f > scale ? 0.0f : (scale > 1.0f ? 1.0f : scale);
		if (unit->parent_index != NONE && unit->parent_seat_index != NONE &&
			!((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit->parent_index)))[unit->parent_seat_index].flags >>
				11) & 1))
		{
			goto impact;
		}
		if (!flags)
		{
			if (function_a76b0(unit_index, 0))
			{
				function_a9260(unit_index, &hit, damage_index);
			}
			else if (g_4e6948->mode != 4)
			{
				function_cfc90(unit_index, damage_index, &hit);
			}
		}
	}
impact:
	if (global_material_get(material_index))
	{
		function_ceee0(unit_index, damage_index, material_index, &point, &normal);
		if (self_damage_index != NONE)
		{
			s_type_1e6529 damage;

			damage.material_index = NONE;
			function_d6660(&damage, self_damage_index);
			damage.flags |= 8;
			damage.origin = unit->unknown030;
			damage.position = unit->unknown030;
			damage.direction.i = 0.0f - unit->unknown168.i;
			damage.direction.j = 0.0f - unit->unknown168.j;
			damage.direction.k = 0.0f - unit->unknown168.k;
			if (g_4e6948->mode != 4)
			{
				function_d7b80(&damage, unit_index, NONE, NONE, NONE, 0);
			}
			else
			{
				s_unit *object = (s_unit *)function_badc0(unit_index, 3);

				if (object && object->unknown13c != NONE)
				{
					object = (s_unit *)function_badc0(unit_index, 3);
					long player_index = object ? object->unknown13c : NONE;
					long controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

					if (controller != NONE)
					{
						s_damage_owner owner;

						owner.player_index = NONE;
						owner.object_index = NONE;
						owner.team = NONE;
						owner.unknown0a = 0;
						function_153d10(controller, self_damage_index, &owner, &damage.direction, 0, 1.0f, 1.0f, 0);
					}
				}
			}
		}
	}
	if (unit->unknown13c != NONE)
	{
		byte *player = (byte *)g_4e8c24->data + (unit->unknown13c & 0xffff) * 0x21c;

		player[0x40] = (byte)unit_round(g_510c54->field_2_3 * 0.5f);
	}
}

/* the unit object type's definition: its datum size and the callbacks it
   dispatches to (only this file's callbacks are filled in) */
struct s_unit_type_definition_view
{
	char const *name;
	long group_tag;
	short datum_size;
	short unknown0a;
	long unknown0c;
	void *functions[29];
	void *parent;
	void *self;
	byte unknown8c[0xc8 - 0x8c];
};

extern s_unit_type_definition_view g_4679b0;
s_unit_type_definition_view g_4679b0 =
{
	"unit",
	'unit',
	0x348,
	NONE,
	NONE,
	{
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, (void *)function_c4410,
		NULL, NULL, (void *)function_c4820, (void *)function_c4880,
		(void *)function_c6b30, NULL, NULL, (void *)function_c6b90,
		NULL, NULL, (void *)function_ced30, (void *)function_ceea0,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL
	},
	NULL,
	&g_4679b0
};
