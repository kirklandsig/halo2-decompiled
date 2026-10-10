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
#include "units.h"
#include "unit_requests.h"
#include "object_markers.h"
#include "unknown_1c62f0.h"
#include "unknown_0d0690.h"
#include "sound_sources.h"
#include "object_iterator.h"
#include "unknown_1cec30.h"
#include "object_queries.h"
#include "unknown_11cc90.h"
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
	bool unknown330[4];
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
bool function_10f630(long object_index, long *first, long *second);
void function_b7360(long object_index);
bool function_c9040(long vehicle_index, long unit_index, short seat_index, vector3f const *forward,
	point3f const *position);
long function_1e4a10(long index);
void function_c5740(long unit_index, bool start);
void function_1520f0(long player_index);
long __stdcall function_cdeb0(long unit_index, long state_name, long a, long b);
bool function_100f00(long weapon_index);
bool function_1e3370(long actor_index, void *unknown);
void function_10cec0(long unit_index, long weapon_index, long parent_marker_name, long marker_name);
void function_1c9c80(long object_index, long unknown2d0, word unknown2c8, real unknown2cc, long a, bool b);
bool unit_has_weapon_definition(long unit_index, long definition_index);
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode);
void function_10cd50(long weapon_index);
bool unit_action_active(long unit_index, long action_type);
byte function_10fcd0(long unit_index, long unknown, long state_name, long action_name);
bool function_100880(long weapon_index, long magazine_index);
void function_c98a0(point2f const *direction, long unit_index, long type, short value);
bool function_10f930(long object_index, real time, bool from_end);
/* who is responsible for damage (damage.cpp) */
struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
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
void function_d6800(long object_index, s_damage_owner const *owner, bool notify_parent, bool unknown);
void function_d6a70(long object_index);
void __stdcall function_caa60(long unit_index, long object_index, point3f const *point, bool knocked, bool force);
void __stdcall function_bd020(long object_index);
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
void __stdcall function_153d10(short team, long definition_index, void *a, void *b, long c, real d, real e, long f);
void object_get_damage_owner(long object_index, s_damage_owner *owner);
void function_107370(long device_index, real value);
void function_184060(long unknown3c, byte unknown59, s_type_1e6529 *data, long unknown50);
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
bool __stdcall function_bc1d0(long object_index, point3f *point);

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
struct s_unit_move_result;
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
void object_get_root_location(long object_index, s_location *location);
long function_d6c80(s_type_1e6529 *data, long ignore_object_index);
short *function_1886d0(long object_index, short *material_type);
struct s_globals_element;
s_globals_element *function_188690(short index);
bool function_101380(long weapon_index);
bool function_1013e0(long weapon_index);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector, long ignore_object_index,
	long ignore_unit_index, s_collision_result_1697c0 *result);
bool function_bc380(long object_index, long block_offset, long size, long a);
void __stdcall function_10f260(long unit_index);
void function_114ec0(long unit_index, long a);
bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);
void function_a8a30(long unit_index, long definition_index);
real normalize2d(point2f *v);
real function_11cc90(vector2f const *a, vector2f const *b);
void function_e70b0(long unit_index, short *value);
bool function_10ff40(long unit_index, long type, short side, short value, bool *flag, short *side_out, short *value_out);
void function_ba350(long object_index, real seconds);
bool function_101240(long weapon_index);
real function_10f7f0(long object_index);
real function_10f690(long object_index, real *duration);
bool function_a7670(long object_index);
void __stdcall function_d6bc0(long object_index);
void function_edfa0(long unit_index, point2f const *facing);
void __stdcall function_e4a20(point3f const *point, long arg_159e6d_2, long object_index, bool knocked);
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
void function_15cbf0(long attacker_index, long player_index, long flags);
bool function_1147e0(long unit_index, bool a, real b, real c, long definition_index, bool hard);
void function_c86e0(long unit_index, bool keep_weapon_zoom);
void function_1c95d0(long unit_index, long attacker_index, short type, real amount);
void function_1c9e10(long unit_index, vector3f const *direction, real shake);
void function_c7840(vector3f const *desired, vector3f *current, transform4x3f const *frame, real rate, vector3f *velocity,
	real const *limits, real arg_3097c5_2, real pitch_rate);
bool __stdcall function_10f430(long unit_index, long mode, long weapon_class, long weapon_type, long set, real blend,
	bool force, long flags);
bool function_1012c0(long weapon_index);
long function_baf80(long object_index);
struct s_location;
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	long unused, long effect_value, s_location const *location, real scale);
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction,
	long tag_index);


void function_d6660(s_type_1e6529 *data, long definition_index);
void function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);

struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

long function_189210(long object_index, long marker_name, s_sound_label_play const *play);

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
		function_189210(object_index, 0x4000095, &play);
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
	if (function_0c7070(unit_index))
	{
		return true;
	}
	long name = UNIT_ANIMATION(UNIT_GET(unit_index))->name;
	return name == 0x80000c4 || name == 0x400004a || name == 0x50000c3;
}

/* the last object of the chain the units' +0x24c links */
// @retail 0xc7100
long function_c7100(long unit_index)
{
	long *next = &UNIT_GET(unit_index)->unknown24c;
	long result = NONE;

	while (*next != NONE)
	{
		result = *next;
		next = &UNIT_GET(result)->unknown24c;
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
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

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
// @retail 0xcaf00
void function_caf00(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);

	unit->flags_10a &= ~4;
	function_b8b70(unit_index);
	function_bb950(unit_index, false, NONE);
	function_db5c0(unit_index);
	*((byte *)unit + unit->unknown33e) &= ~4;
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

	for (long i = 0; i < 4; i++)
	{
		if (i != unit->current_weapon_index && i != unit->next_weapon_index && unit->weapon_object_indices[i] != NONE)
		{
			return unit->weapon_object_indices[i];
		}
	}
	return NONE;
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

/* whether the unit's animation is 0x600008c */
// @retail 0xcc3c0
bool function_cc3c0(long unit_index)
{
	s_unit_animation *animation = UNIT_ANIMATION(UNIT_GET(unit_index));
	bool result = false;

	if (animation->unknown68 != NONE && animation->unknown00 != NONE && animation->unknown06 != NONE &&
		animation->name == 0x600008c)
	{
		result = true;
	}
	return result;
}

/* whether the unit's animation is 0xd000042 or 0xc000043 */
// @retail 0xcc410
bool function_cc410(long unit_index)
{
	s_unit_animation *animation = UNIT_ANIMATION(UNIT_GET(unit_index));

	if (animation->unknown68 != NONE && animation->unknown00 != NONE && animation->unknown06 != NONE &&
		(animation->name == 0xd000042 || animation->name == 0xc000043))
	{
		return true;
	}
	return false;
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
short function_cccd0(long unit_index, short grenade_type, char delta)
{
	UNIT_GET(unit_index)->grenade_counts[grenade_type] += delta;
	s_unit *unit = UNIT_GET(unit_index);
	unit->next_grenade_index = (char)grenade_type;
	unit->current_grenade_index = (char)grenade_type;
	return UNIT_GET(unit_index)->grenade_counts[grenade_type];
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

	for (long i = 0; i < 4; i++)
	{
		if (unit->weapon_object_indices[i] == NONE)
		{
			return (short)i;
		}
	}
	return NONE;
}

/* the unit's count of a grenade type */
// @retail 0xcdff0
short function_cdff0(long unit_index, short grenade_type)
{
	if (grenade_type == NONE)
	{
		return 0;
	}
	return UNIT_GET(unit_index)->grenade_counts[grenade_type];
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
	if (UNIT_GET(unit_index)->type != 0)
	{
		return false;
	}
	return function_e4050(unit_index);
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
	while (iterator->next_index != NONE)
	{
		long unit_index = iterator->next_index;
		s_unit *unit = UNIT_GET(unit_index);

		iterator->next_index = unit->next_sibling_index;
		if ((1 << unit->type) & 3)
		{
			iterator->unit_index = unit_index;
			iterator->seat_index = unit->parent_seat_index;
			return (s_damage_object *)unit;
		}
	}
	return 0;
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
		short counter = (UNIT_GET(unit_index)->unknown210 + 1) & 0xf;

		if (counter == NONE)
		{
			counter = 0;
		}
		UNIT_GET(unit_index)->unknown210 = counter;
		UNIT_GET(unit_index)->unknown214 = counter;
		if (UNIT_GET(unit_index)->unknown0d4 != NONE)
		{
			function_b58c0(UNIT_GET(unit_index)->unknown0d4, 0x2000);
		}
	}
}

/* the nearest seat 0xc8bb0 finds for the unit: its object and seat
   index; returns 0xc8bb0's third result */
// @retail 0xc8ef0
short __stdcall function_c8ef0(long unit_index, long a, long *object_index, short *seat_index)
{
	long found_object_index = NONE;
	short found_seat_index = NONE;
	short result = 0;
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
	short difficulty;

	if (g_4e6948->state == 1)
	{
		difficulty = g_4e6948->difficulty;
	}
	else
	{
		difficulty = 1;
	}
	switch (difficulty)
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
	s_unit *unit = UNIT_GET(unit_index);

	for (long i = 0; i < 4; i++)
	{
		long weapon_index = unit->weapon_object_indices[i];

		if (weapon_index != NONE && (*(dword *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x12c) >> 3) & 1)
		{
			return (short)i;
		}
	}
	return NONE;
}

/* the next grenade type the unit has, from the given one in a direction
   (0 keeps the given one when the unit has it) */
// @retail 0xcbec0
short function_cbec0(long unit_index, short grenade_type, short direction)
{
	s_unit *unit = UNIT_GET(unit_index);
	short result = NONE;
	short first = grenade_type == NONE ? 0 : grenade_type;
	short type = first;

	do
	{
		if (unit->grenade_counts[type] > 0)
		{
			result = type;
			if (type != first || direction == 0)
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
	} while (type != first);
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
		unit->unknown270 = matrix.position;
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
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->parent_index != NONE)
	{
		short seat_index = unit->parent_seat_index;

		if (seat_index != NONE)
		{
			s_unit *parent = UNIT_GET(unit->parent_index);

			if ((1 << parent->type) & 3)
			{
				return &UNIT_SEATS(UNIT_DEFINITION_GET(parent))[seat_index];
			}
		}
	}
	return 0;
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
	state->tail.unknown1c = 0.0f;
	state->tail.unknown20 = 0.0f;
	state->tail.unknown00 = NONE;
	state->tail.unknown04 = NONE;
	state->tail.unknown08 = NONE;
	state->tail.unknown18 = 0;
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
   fails 0x101640 */
// @retail 0xd1540
void __stdcall function_d1540(long unit_index)
{
	if (UNIT_GET(unit_index)->current_weapon_index != NONE && UNIT_GET(unit_index)->next_weapon_index != NONE)
	{
		s_unit *unit = UNIT_GET(unit_index);
		short index = unit->current_weapon_index;
		long weapon_index = index != NONE ? unit->weapon_object_indices[index] : NONE;
		s_unit *other = UNIT_GET(unit_index);
		short other_index = other->next_weapon_index;
		long other_weapon_index = other_index != NONE ? other->weapon_object_indices[other_index] : NONE;

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

/* creates a starting weapon for the unit; flags it at +0x12d when asked */
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
				*((byte *)object + 0x12d) |= 1;
			}
		}
	}
	return object_index;
}

/* whether either weapon the unit holds is in state 1 or 2 (+0x20c) */
// @retail 0xcc0c0
bool function_cc0c0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;
	short index = unit->current_weapon_index;

	if (index != NONE && unit->weapon_object_indices[index] != NONE)
	{
		char state = *((char *)UNIT_GET(unit->weapon_object_indices[index]) + 0x20c);

		result = state == 1 || state == 2;
	}
	index = unit->next_weapon_index;
	if (index != NONE && unit->weapon_object_indices[index] != NONE && !result)
	{
		char state = *((char *)UNIT_GET(unit->weapon_object_indices[index]) + 0x20c);

		result = state == 1 || state == 2;
	}
	return result;
}

/* gets rid of an object the unit held: deletes it (mode 1, or when the
   unit or the object's definition asks) or has 0xce6b0 drop it with an
   animation by mode */
// @retail 0xce470
void function_ce470(long mode, long object_index, long unit_index)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(object_index));

	if (UNIT_GET(unit_index)->flags_134 & 0x10000)
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
		return;
	}
	switch (mode)
	{
	case 2:
		function_ce6b0(unit_index, 0xa0005ad, object_index, 1.0f);
		break;
	case 3:
		function_ce6b0(unit_index, 0x9000536, object_index, 1.0f);
		break;
	default:
		function_ce6b0(unit_index, 0, object_index, 1.0f);
		break;
	}
}

/* the unit's next weapon state: its weapon's when it has rounds (+0x1fe),
   else the next of the globals' count after the given one */
// @retail 0xc8960
short function_c8960(long unit_index, short value)
{
	s_unit *unit = UNIT_GET(unit_index);
	short index = unit->current_weapon_index;
	long weapon_index = index != NONE ? unit->weapon_object_indices[index] : NONE;
	short result = NONE;

	if (weapon_index != NONE)
	{
		byte *definition = UNIT_DEFINITION_GET(UNIT_GET(weapon_index));
		bool flag = *(definition + 0x12e) & 1;

		if (*(short *)(definition + 0x1fe) > 0)
		{
			return function_101010(weapon_index, value);
		}
		if (flag)
		{
			return result;
		}
	}
	long count = *(long *)(*(byte **)((byte *)g_4e034c + 0x134) + 0xb8);
	if (count > 0 && value + 1 < count)
	{
		result = value + 1;
	}
	return result;
}

/* sends request 0x17 when the unit's seat has bit 15 */
// @retail 0xd12b0
void function_d12b0(long unit_index, long seat_index, bool a, bool b)
{
	if (unit_index != NONE && seat_index != NONE &&
		(*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit_index)))[seat_index].flags >> 15) & 1)
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
		(*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit_index)))[seat_index].flags >> 15) & 1)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		request.type = 0x18;
		request.type17.unknown4 = a;
		request.type17.unknown5 = b;
		function_e6900(unit_index, &request);
	}
}

/* whether a biped faces along the direction: always while in animation
   0x6000084, else by the sign of its facing's dot product with it */
// @retail 0xcc010
bool function_cc010(long object_index, vector3f const *direction)
{
	s_unit *unit = (s_unit *)function_badc0(object_index, 3);
	bool result = false;

	if (unit && unit->type == 0)
	{
		long unknown;
		long name = NONE;

		function_10f630(object_index, &unknown, &name);
		if (!(*(UNIT_DEFINITION_GET(unit) + 0xbe) & 1))
		{
			if (name == 0x6000084)
			{
				return true;
			}
			return unit->unknown190 * direction->j + direction->i * unit->unknown18c > 0.0f;
		}
	}
	return result;
}

/* the unit's death outside mode 4: clears its flags, applies the globals'
   damage at +0x144 to it and, unless flag 2 at +0x10a, marks it */
// @retail 0xcffc0
void function_cffc0(long unit_index)
{
	if (g_4e6948->mode != 4)
	{
		s_unit *unit = UNIT_GET(unit_index);
		byte *globals = *(byte **)((byte *)g_4e034c + 0x144);

		unit->flags_134 &= ~0x200000;
		unit->flags_134 &= ~0x20;
		unit->flags_10a &= ~0x80;
		if (globals && *(long *)(globals + 0x48) != NONE)
		{
			s_type_1e6529 damage;

			function_d6660(&damage, *(long *)(globals + 0x48));
			damage.material_index = NONE;
			function_d7b80(&damage, unit_index, NONE, NONE, NONE, 0);
		}
		if (!((unit->flags_10a >> 2) & 1))
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

	for (long i = 0; i < count && !found; i++)
	{
		if (function_c9040(unit_index, a, seat_index, &markers[i].matrix.forward, &markers[i].matrix.position))
		{
			found = true;
		}
	}
	return found;
}

/* whether a rider of the unit sits in a seat with bit 11 whose +0x3e is
   the given value */
// @retail 0xc9200
bool __stdcall function_c9200(long unit_index, short value)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(unit_index));
	bool result = false;
	s_object_child_iterator iterator;

	function_d0620(unit_index, &iterator);
	while (function_d0690(&iterator) && !result)
	{
		if (iterator.child_value == unit_index && iterator.child_short != NONE)
		{
			s_unit_seat_definition *seat = &UNIT_SEATS(definition)[iterator.child_short];

			if ((*(dword *)&seat->flags >> 11) & 1 && *(short *)((byte *)seat + 0x3e) == value)
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

/* marks the unit's weapon slot changed: the object +0xd4 names gets the
   slot's two bits, then the counter steps */
static void unit_weapon_slot_changed(long unit_index, long slot_index)
{
	long index = UNIT_GET(unit_index)->unknown0d4;

	if (index != NONE)
	{
		function_b58c0(index, (1 << (slot_index + 0x12)) | (1 << (slot_index + 0xe)));
	}
	function_cea00(unit_index);
}

/* puts a weapon in the unit's slot, or empties the slot and gets rid of
   the weapon there by mode (mode 4 keeps it) */
// @retail 0xcea70
void __stdcall function_cea70(long mode, long weapon_index, short slot_index, long unit_index)
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
			function_ce470(mode, old_weapon_index, unit_index);
		}
	}
	unit_weapon_slot_changed(unit_index, slot_index);
}

/* attaches the weapon to the unit at its hand's marker (the unit
   definition's +0x158 or +0x15c), at the unit's marker +0x160 when the
   weapon has it, else at the weapon definition's +0x284 */
// @retail 0xd0870
void __stdcall function_d0870(long weapon_index, long unit_index, bool secondary)
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
	long marker_name = *(long *)(local_98b918_2 + 0x160);
	if (marker_name == NONE || marker_name == 0)
	{
		marker_name = *(long *)(local_67e06b + 0x284);
	}
	else
	{
		s_object_marker marker;

		if (!function_b8d30(weapon_index, marker_name, &marker, 1, true))
		{
			marker_name = *(long *)(local_67e06b + 0x284);
		}
	}
	function_10cec0(unit_index, weapon_index, parent_marker_name, marker_name);
}

/* drops every weapon the unit holds (mode 1: deletes them) */
// @retail 0xccf20
void function_ccf20(long unit_index)
{
	long *slot = UNIT_GET(unit_index)->weapon_object_indices;

	for (long i = 0; i < 4; i++, slot++)
	{
		if (*slot != NONE)
		{
			s_unit *unit = UNIT_GET(unit_index);
			long weapon_index = unit->weapon_object_indices[(short)i];

			unit->weapon_object_indices[(short)i] = NONE;
			function_ce470(1, weapon_index, unit_index);
			unit_weapon_slot_changed(unit_index, (short)i);
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

/* creates the unit definition's starting weapons (+0x1c0, +0x1c4) and
   gives them to the unit; deletes those it cannot take (or, in state 2,
   already has) */
// @retail 0xccab0
void function_ccab0(long unit_index)
{
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(unit_index));

	for (long i = 0; i < *(long *)(definition + 0x1c0); i++)
	{
		long tag_index = *(long *)(*(byte **)(definition + 0x1c4) + i * 8 + 4);

		if (tag_index != NONE)
		{
			byte data[0xc4];

			function_b7930(data, tag_index, unit_index, 0);
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
	if ((unit->flags_134 >> 3) & 1 && 0.05f > unit->unknown2b0)
	{
		unit->unknown2b0 = 0.05f;
	}
	if (UNIT_GET(unit_index)->unknown0d4 != NONE)
	{
		function_b58c0(UNIT_GET(unit_index)->unknown0d4, 0x800000);
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
	s_unit *unit = UNIT_GET(unit_index);

	memset(hands, 0, sizeof(*hands));
	s_unit *current = UNIT_GET(unit_index);
	short index = current->current_weapon_index;
	if (index != NONE && current->weapon_object_indices[index] != NONE)
	{
		s_unit *next = UNIT_GET(unit_index);
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
		s_unit *parent = UNIT_GET(unit->parent_index);

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

	if (parent_index != NONE)
	{
		s_unit *parent = UNIT_GET(parent_index);

		if ((1 << parent->type) & 3)
		{
			short seat_index = unit->parent_seat_index;

			if (seat_index != NONE)
			{
				short other_seat_index = *(short *)((byte *)&UNIT_SEATS(UNIT_DEFINITION_GET(parent))[seat_index] + 0x3e);

				if (other_seat_index != NONE)
				{
					long occupant_index = function_c8f60(parent_index, other_seat_index);

					if (occupant_index != NONE && occupant_index != unit_index)
					{
						s_unit_request request;

						request.type = 0x20;
						request.type17.unknown4 = a;
						request.type17.unknown5 = b;
						if (function_e6900(occupant_index, &request))
						{
							function_cc810(parent_index);
						}
					}
				}
			}
		}
	}
	return false;
}

/* lowers the weapon in one of the unit's hands (request 8, or 0x12 for the
   second hand) */
// @retail 0xcd4e0
bool __stdcall function_cd4e0(long unit_index, short hand, bool flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	char *indices = &unit->current_weapon_index + hand;
	bool result = false;

	if (indices[0] != NONE && indices[4] != NONE)
	{
		s_unit_request request;

		memset(&request, 0, sizeof(request));
		indices[4] = NONE;
		request.type = hand != 0 ? 0x12 : 8;
		request.type17.unknown4 = flag;
		if (!function_e6900(unit_index, &request))
		{
			return false;
		}
		if (hand)
		{
			function_cea00(unit_index);
		}
		if (unit->unknown13c != NONE)
		{
			function_1520f0(unit->unknown13c);
		}
		return true;
	}
	return result;
}

/* the weapon the unit, or the first rider down its children with bit 27 of
   its definition's +0xbc, holds; which unit holds it */
// @retail 0xcbd80
long __stdcall function_cbd80(long object_index, long *holder_index)
{
	s_unit *unit = UNIT_GET(object_index);
	short index = unit->current_weapon_index;
	long weapon_index = index != NONE ? unit->weapon_object_indices[index] : NONE;

	if (weapon_index != NONE)
	{
		if (holder_index)
		{
			*holder_index = object_index;
		}
		return weapon_index;
	}
	for (long child_index = UNIT_GET(object_index)->first_child_index; child_index != NONE;
		child_index = UNIT_GET(child_index)->next_sibling_index)
	{
		s_unit *child = UNIT_GET(child_index);

		if ((1 << child->type) & 3 && (*(dword *)(UNIT_DEFINITION_GET(child) + 0xbc) >> 27) & 1)
		{
			weapon_index = function_cbd80(child_index, holder_index);
			if (weapon_index != NONE)
			{
				break;
			}
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
		function_cea70(mode, NONE, (short)slot_index, unit_index);
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

	if (!((unit->flags_134 >> 21) & 1))
	{
		real range;
		byte unknown[8];

		unit->flags_134 |= 0x200000;
		if (unit->actor_index != NONE && function_1e3370(unit->actor_index, unknown))
		{
			unit->unknown2d8 = 0.0f;
			range = 0.43633232f;
		}
		else
		{
			real angle = (real)atan2(unit->forward.j, unit->forward.i);

			if (angle > 3.1415927f)
			{
				angle -= 6.2831855f;
			}
			unit->unknown2d8 = angle;
			range = 1.7453293f;
		}
		unit->unknown2d8 += function_259d0(&g_4e7408->unknown0, 0, 0, -range, range);
	}
}

/* the object that answers for the unit: the vehicle when the unit's seat
   has bit 0 or 3, or a rider of that vehicle with bit 27 of its
   definition's +0xbc and its object flag 26; else the unit */
// @retail 0xcb6d0
long function_cb6d0(long unit_index)
{
	long result = unit_index;

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

				if (*(byte *)seat & 1 || (*(dword *)seat >> 3) & 1)
				{
					result = parent_index;
				}
				for (long child_index = parent->first_child_index; child_index != NONE;
					child_index = UNIT_GET(child_index)->next_sibling_index)
				{
					s_unit *child = (s_unit *)function_badc0(child_index, 3);

					if (child && (*(dword *)(UNIT_DEFINITION_GET(child) + 0xbc) >> 27) & 1 &&
						(child->object_flags >> 26) & 1)
					{
						return child_index;
					}
				}
			}
		}
	}
	return result;
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
			request.type1c.unknowna = unit_action_active(unit_index, 0x1f);
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
		seat_blocks = !((*(dword *)&seat->flags >> 5) & 1);
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
		result = function_10fcd0(unit_index, unknown, *(long *)(local_67e06b + 0x288),
			*(long *)(local_67e06b + 0x28c));
	}
	if (unit->unknown13c != NONE && (*(dword *)(local_67e06b + 0x12c) >> 29) & 1)
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
		unit->grenade_counts[0] += profile[0x40];
		unit->grenade_counts[1] += profile[0x41];
	}
}

/* whether the unit is free to act: not riding, without object flags 26
   and 27 in its animation state, no blocking action, and neither hand's
   weapon busy (0x100880) */
// @retail 0xc85c0
bool function_c85c0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = true;

	if (unit->parent_index != NONE ||
		(*(dword *)((byte *)unit + unit->unknown346 + 4) >> 26) & 1 ||
		(*(dword *)((byte *)unit + unit->unknown346 + 4) >> 27) & 1 ||
		unit_action_active(unit_index, 0x16) || unit_action_active(unit_index, 8) ||
		unit_action_active(unit_index, 0x12) || unit_action_active(unit_index, 0) ||
		unit_action_active(unit_index, 1) || unit_action_active(unit_index, 0xa) ||
		unit_action_active(unit_index, 0xb))
	{
		return false;
	}
	unit = UNIT_GET(unit_index);
	for (long hand = 0; hand < 2; hand++)
	{
		short index = (&unit->current_weapon_index)[hand];

		if (index != NONE && unit->weapon_object_indices[index] != NONE &&
			function_100880(unit->weapon_object_indices[index], NONE))
		{
			return false;
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
	byte *definition = UNIT_DEFINITION_GET(UNIT_GET(grenade_index));
	s_unit *unit = UNIT_GET(unit_index);
	byte *globals = (byte *)g_4e034c;

	if (!*(void **)(globals + 0x100))
	{
		return false;
	}
	short type = *(short *)(definition + 0x12e);
	short *maximum = (short *)(*(byte **)(globals + 0x104) + type * 0x2c);
	if (!maximum || unit->grenade_counts[type] >= *maximum)
	{
		return false;
	}
	if (unit->unknown13c != NONE)
	{
		function_1e9070(unit->unknown13c);
	}
	if (g_4e6948->mode == 4)
	{
		return false;
	}
	unit->grenade_counts[type]++;
	function_a7a30(unit_index, 0x400000);
	function_a8950(unit_index, UNIT_GET(grenade_index)->definition_index);
	s_unit *object = (s_unit *)function_badc0(unit_index, 3);
	if (object && object->unknown13c != NONE &&
		*(short *)((byte *)g_4e8c24->data + (unit_get_player_index(unit_index) & 0xffff) * 0x21c + 0x28) != NONE)
	{
		function_f8110(grenade_index);
	}
	function_b8540(grenade_index);
	return true;
}

/* finds a unit that is dying or down: object flags 22, 26 or 27 in its
   animation state, or in animation 0xc000043 or 0xd000042 without flag 2
   at +0x33e and bit 0 at its animation's +0x6c */
// @retail 0xcc170
bool __stdcall function_cc170(long *value)
{
	s_type_f1af8e iterator;
	s_unit *object;

	function_bae80(&iterator, 3, 1);
	while ((object = (s_unit *)function_baeb0(&iterator)) != 0)
	{
		long unit_index = iterator.object_index;
		s_unit *unit = UNIT_GET(unit_index);
		s_unit_animation *animation = UNIT_ANIMATION(unit);
		long name = NONE;

		if (animation->unknown68 != NONE && animation->unknown00 != NONE && animation->unknown06 != NONE)
		{
			name = animation->name;
		}
		if ((*(dword *)((byte *)unit + unit->unknown346 + 4) >> 22) & 1 ||
			(*(dword *)((byte *)unit + unit->unknown346 + 4) >> 26) & 1 ||
			(*(dword *)((byte *)unit + unit->unknown346 + 4) >> 27) & 1)
		{
			*value = unit_index;
			return true;
		}
		if ((name == 0xc000043 || name == 0xd000042) && !((*((byte *)object + object->unknown33e) >> 2) & 1) &&
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
long __stdcall function_cdeb0(long unit_index, long state_name, long a, long b)
{
	s_unit *unit = UNIT_GET(unit_index);
	short slot = (short)a;
	short direction = (short)b;
	short result = NONE;
	bool result_flag = false;
	long best = 0;
	short first = slot == NONE ? 0 : (slot + direction + 4) % 4;
	short i = first;

	do
	{
		long weapon_index = unit->weapon_object_indices[i];

		if (weapon_index != NONE && function_cd6a0(unit_index, state_name, weapon_index) &&
			i != unit->next_weapon_index)
		{
			long time = unit->unknown228[i];
			bool flag = (*(dword *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x12c) >> 3) & 1;

			if (result == NONE || (!result_flag && (flag || (direction == 0 && time > best))))
			{
				result = i;
				best = time;
				result_flag = flag;
			}
		}
		i = direction < 0 ? (i + 3) % 4 : (i + 1) % 4;
	} while (i != first);
	return result;
}

/* starts (state 1, its timer from 0xc5340, flag 3) or ends (state 2, ten
   seconds) the unit's state +0x2bc while its player is in state 1 at
   +0x88 (or the campaign flag allows it), with the globals' sounds */
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

				function_188180(point, forward, tag_index, unit_index, 0xf, 0, material_index, (long)(root + 0x70),
					(s_location const *)(root + 0x28), 1.0f);
			}
			return;
		}
	}
	byte *material = 0;
	if (material_index != NONE && material_index >= 0 && material_index < *(long *)((byte *)g_4e034c + 0x150))
	{
		material = *(byte **)((byte *)g_4e034c + 0x154) + material_index * 0xb4;
	}
	long tag_index = *(long *)(material + 0x64);
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
	long *slot = unit->weapon_object_indices;
	for (long i = 0; i < 4; i++, slot++)
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
			UNIT_GET(unit_index)->weapon_object_indices[(short)i] = NONE;
			unit_weapon_slot_changed(unit_index, (short)i);
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
		s_unit *unit = UNIT_GET(unit_index);
		short index = unit->current_weapon_index;
		bool first = weapon_index == (index != NONE ? unit->weapon_object_indices[index] : NONE);
		unit = UNIT_GET(unit_index);
		index = unit->next_weapon_index;
		bool second = weapon_index == (index != NONE ? unit->weapon_object_indices[index] : NONE);
		long action;

		if (first)
		{
			switch (type)
			{
			case 1:
				action = 2;
				break;
			case 2:
				action = 3;
				break;
			case 3:
				action = 4;
				break;
			case 4:
				action = 5;
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
			case 1:
				action = 0xc;
				break;
			case 2:
				action = 0xd;
				break;
			case 3:
				action = 0xe;
				break;
			case 4:
				action = 0xf;
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
		long rider_index = UNIT_GET(unit_index)->unknown24c;
		if (rider_index != NONE)
		{
			function_e68c0(action, rider_index);
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
			difference = total_a - total_b;
			motion = b;
		}
		else
		{
			return;
		}
		if (motion)
		{
			real x = (difference + motion->coast_time) * scale;
			real speed = (real)fabs(motion->field_10_4 * motion->acceleration + motion->velocity);
			real step = ((real)sqrt(x * x + 4.0f * speed * difference * scale) - x) / (scale + scale);
			real limit = motion->field_10_4 > motion->s_type_8c87de ? motion->s_type_8c87de :
				motion->field_10_4;

			if (step > limit)
			{
				step = motion->field_10_4 > motion->s_type_8c87de ? motion->s_type_8c87de :
					motion->field_10_4;
			}
			if (step > 0.0f)
			{
				real time = motion->field_10_4 - step;
				real velocity = time * motion->acceleration + motion->velocity;

				if (0.0001f > (real)fabs(velocity))
				{
					motion->done = true;
					motion->field_10_4 = 0.0f;
					motion->s_type_8c87de = 0.0f;
					motion->coast_time = 0.0f;
					return;
				}
				motion->field_10_4 = time;
				motion->s_type_8c87de -= step;
				motion->coast_time = (motion->acceleration * step + velocity * 2.0f) * step / velocity;
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

	if (unit->actor_index != NONE && g_4e6948->state == 1 && g_4f55dc[2])
	{
		short team = unit->unknown138;

		if (team == NONE || team < 0 || team >= 0x10 ||
			!function_0bfe60((dword const *)((byte *)g_4f55ec + 0xc4), team + 0x10))
		{
			unit->flags_134 |= 8;
		}
	}
	if ((unit->flags_134 >> 3) & 1)
	{
		unit->unknown2b0 += unit->unknown2b8 * g_510c54->rate;
		if (unit->unknown2b0 > 1.0f)
		{
			unit->unknown2b0 = 1.0f;
			if (UNIT_GET(unit_index)->unknown0d4 != NONE)
			{
				function_b58c0(UNIT_GET(unit_index)->unknown0d4, 0x800000);
			}
		}
	}
	else
	{
		unit->unknown2b0 -= unit->unknown2b8 * g_510c54->rate;
		if (0.0f > unit->unknown2b0)
		{
			unit->unknown2b0 = 0.0f;
			if (UNIT_GET(unit_index)->unknown0d4 != NONE)
			{
				function_b58c0(UNIT_GET(unit_index)->unknown0d4, 0x800000);
			}
		}
	}
	if ((unit->flags_134 >> 4) & 1)
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

/* drops a weapon of the unit: from its first hand at once (request 9's
   handler, then the player's controller hears of it), from its second
   hand by request 0x13, else from its slot (0xce920) */
// @retail 0xce520
void function_ce520(long unit_index, long weapon_index, bool flag)
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
		function_b7360(unit_index);
		bool result = g_4677c8[9]->perform(unit_index, &request);
		if (unit_index != NONE)
		{
			long player_index = UNIT_GET(unit_index)->unknown13c;

			if (player_index != NONE)
			{
				short controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

				if (controller != NONE)
				{
					function_1e6980((s_time_entry *)((byte *)g_51e9c0 + controller * 0x1b0 + 0x150),
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

/* where the unit's camera sits and its camera data: its seat's marker on
   the parent (when the parent has it), else the unit itself */
// @retail 0xd1080
bool function_d1080(long unit_index, transform4x3f *matrix, s_unit_camera_data *camera)
{
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;

	matrix->scale = 1.0f;
	if (unit->parent_index != NONE && unit->parent_seat_index != NONE)
	{
		long parent_index = unit->parent_index;
		s_unit_seat_definition *seat =
			&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(parent_index)))[unit->parent_seat_index];
		s_object_marker marker;

		if (!function_b8d30(parent_index, *(long *)((byte *)seat + 8), &marker, 1, false))
		{
			return result;
		}
		*matrix = marker.matrix;
		function_b9dd0(unit->parent_index, &matrix->position);
		*camera = *(s_unit_camera_data *)((byte *)seat + 0x24);
		return true;
	}
	byte *definition = UNIT_DEFINITION_GET(unit);
	function_b9dd0(unit_index, &matrix->position);
	matrix->forward = unit->forward;
	matrix->up = unit->up;
	*camera = *(s_unit_camera_data *)(definition + 0xf0);
	return true;
}

/* the state of a unit's weapon use: 6 or 5 for the flags when asked, 7
   past the definition's limit at +0x114, else 3; otherwise 0xc96b0's
   range (2 when forced, at most 1 when asked), held to 1 for players */
// @retail 0xc9770
long __stdcall function_c9770(long unit_index, bool flag, bool a, bool b, long unused, bool force, real c, real d)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	long range = 0;

	if (flag)
	{
		if (a)
		{
			return 6;
		}
		if (b)
		{
			return 5;
		}
		if (*(real *)(definition + 0x114) > 0.0f && unit->unknownf8 + d > *(real *)(definition + 0x114))
		{
			return 7;
		}
		return 3;
	}
	if ((unit->flags_10a >> 2) & 1)
	{
		return 0;
	}
	function_c96b0(unit_index, &range, c, d);
	long result;
	if (force)
	{
		result = 2;
	}
	else
	{
		result = range;
		if (b && result > 1)
		{
			result = 1;
		}
	}
	if ((unit->flags_134 >> 5) & 1 || (g_4e6948->state == 1 && g_4f55dc[7]) || unit->unknown13c != NONE)
	{
		if (result > 1)
		{
			return 1;
		}
	}
	return result;
}

/* whether the object's root, simulated by Havok and not keyframed, has
   something in the way (0x168f40 from its main rigid body's position) */
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
		(root->unknownc0 >> 6) & 1 && root->unknownb4 != NONE)
	{
		s_havok_component *component = havok_component_get(root->unknownb4);
		short rigid_body_index = function_0b67a0((s_small_index const *)component);

		if (rigid_body_index != NONE)
		{
			byte *rigid_body = (byte *)havok_component_rigid_body_get(rigid_body_index, component);

			if (!rigid_body[0x40] && !havok_component_rigid_body_keyframed(rigid_body_index, component))
			{
				point3f point = *(point3f *)(*(byte **)(rigid_body + 0x3c) + 0x70);

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

	if ((unit->flags_10a >> 2) & 1)
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

	function_d0620(unit_index, &iterator);
	if (function_d0690(&iterator))
	{
		do
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

				if ((*(dword *)&seat->flags >> 11) & 1)
				{
					other_team = rider->unknown138;
				}
				else
				{
					team = rider->unknown138;
				}
			}
		} while (function_d0690(&iterator));
		if (team != NONE && (team == other_team || other_team == NONE))
		{
			unit->unknown138 = team;
			return;
		}
	}
	unit->unknown138 = NONE;
}

/* the entry animation (of five) whose end point on the vehicle's seat is
   nearest the unit's center; the seat marker's position and that point */
// @retail 0xc7160
long __stdcall function_c7160(long unit_index, short seat_index, long a, long vehicle_index, long b)
{
	point3f *marker_position = (point3f *)a;
	point3f *position = (point3f *)b;
	s_unit *unit = UNIT_GET(unit_index);
	s_unit_seat_definition *seat = &UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(vehicle_index)))[seat_index];
	bool blocks = !((*(dword *)&seat->flags >> 5) & 1);
	real best_distance = 3.4028235e38f;
	long result = NONE;
	s_object_marker marker;

	if (function_b8d30(vehicle_index, *(long *)((byte *)seat + 8), &marker, 1, false))
	{
		if (marker_position)
		{
			*marker_position = marker.matrix.position;
		}
		for (long const *name = g_467430; name < g_467430 + 5; name++)
		{
			transform4x3f offset;

			if (function_10f9b0(unit_index, seat->label, *name, 1, &offset, blocks))
			{
				transform4x3f matrix;

				function_142a60(&marker.matrix, &offset, &matrix);
				real dx = matrix.position.x - unit->unknown030.x;
				real dy = matrix.position.y - unit->unknown030.y;
				real dz = matrix.position.z - unit->unknown030.z;
				real distance = dz * dz + dx * dx + dy * dy;

				if (best_distance > distance || result == NONE)
				{
					result = *name;
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
	char *count = UNIT_GET(unit_index)->grenade_counts;

	for (long type = 0; type < 2; type++, count++)
	{
		byte *globals = (byte *)g_4e034c;

		if (*(void **)(globals + 0x100))
		{
			byte *grenade = *(byte **)(globals + 0x104) + type * 0x2c;

			if (grenade && *count > 0)
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
							function_ce470(0, grenade_index, unit_index);
						}
					}
				} while (--*count > 0);
			}
		}
	}
}

/* puts a weapon of a definition (and a value at +0x17e) in the unit's slot,
   dropping the one there; sets the unit's +0x210 and +0x214 and, when the
   unit had no weapon, raises it (request 8) */
// @retail 0xd09c0
void __stdcall function_d09c0(long unit_index, long slot_index, long definition_index, short value, dword const *state)
{
	s_unit *unit = UNIT_GET(unit_index);
	long weapon_index = unit->weapon_object_indices[slot_index];
	long current_definition_index = NONE;
	short current_value = NONE;
	long count = 0;

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
		function_ce520(unit_index, weapon_index, false);
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
			function_cea70(NONE, weapon_index, (short)slot_index, unit_index);
			if (unit->unknown13c != NONE)
			{
				function_1520f0(unit->unknown13c);
			}
		}
	}
	s_unit *again = UNIT_GET(unit_index);
	dword new_state = *state;
	*(dword *)&again->unknown214 = new_state;
	again->unknown210 = (short)new_state;
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
		s_unit *object = (s_unit *)function_badc0(unit_index, 3);

		if (object && object->unknown13c != NONE)
		{
			object = (s_unit *)function_badc0(unit_index, 3);
			long player_index = object ? object->unknown13c : NONE;
			short controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

			if (controller != NONE)
			{
				vector3f direction;
				s_damage_owner owner;

				direction.i = 0.0f - unit->unknown168.i;
				direction.j = 0.0f - unit->unknown168.j;
				direction.k = 0.0f - unit->unknown168.k;
				owner.player_index = NONE;
				owner.object_index = NONE;
				owner.team = NONE;
				function_153d10(controller, definition_index, &owner, &direction, 0, 1.0f, 1.0f, 0);
			}
		}
		return;
	}
	s_type_1e6529 damage;

	function_d6660(&damage, definition_index);
	damage.material_index = NONE;
	damage.flags |= 8;
	damage.origin = unit->unknown030;
	damage.position = unit->unknown030;
	damage.direction.i = 0.0f - unit->unknown168.i;
	damage.direction.j = 0.0f - unit->unknown168.j;
	damage.direction.k = 0.0f - unit->unknown168.k;
	function_d7b80(&damage, unit_index, NONE, NONE, NONE, 0);
}

/* what a unit's melee hit: an object, or a surface */
struct s_unit_melee_hit
{
	long object_index;
	long unknown04;
	long unknown08;
	long unknown0c;
	short material_index;
	byte unknown12[2];
	real unknown14;
	bool unknown18;
};

/* applies the unit's melee damage to what it hit (a device of type 7 with
   bit 5 at +0x1cc is triggered first), then ends its state +0x2bc */
// @retail 0xcfc90
void __stdcall function_cfc90(long unit_index, long definition_index, s_unit_melee_hit const *hit)
{
	s_unit *unit = UNIT_GET(unit_index);
	s_type_1e6529 damage;
	s_object_marker marker;

	function_d6660(&damage, definition_index);
	damage.material_index = NONE;
	damage.unknown84 = hit->unknown18;
	damage.flags |= 1;
	damage.unknown18 = unit_index;
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
		s_unit *object = UNIT_GET(hit->object_index);

		if (object->type == 7 && (*((byte *)UNIT_GET(hit->object_index) + 0x1cc) & 0x20))
		{
			function_107370(hit->object_index, 1.0f);
		}
		function_d7b80(&damage, hit->object_index, NONE, NONE, NONE, 0);
	}
	else if (hit->unknown04 != NONE)
	{
		function_184060(hit->unknown04, (byte)hit->unknown08, &damage, hit->unknown0c);
	}
	function_c5740(unit_index, false);
}

/* whether a unit may enter the vehicle's seat from a marker: near enough
   (+0x98), facing it (+0xa0), the marker facing the unit (+0x9c), and
   moving with the vehicle (+0xa4) */
// @retail 0xc9040
bool function_c9040(long vehicle_index, long unit_index, short seat_index, vector3f const *forward,
	point3f const *position)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *seat = (byte *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(vehicle_index)))[seat_index];
	point3f center;
	vector3f direction;
	real distance;

	function_cafc0(unit_index, &center);
	direction.i = position->x - center.x;
	direction.j = position->y - center.y;
	direction.k = position->z - center.z;
	distance = (real)sqrt(direction.k * direction.k + direction.j * direction.j + direction.i * direction.i);
	if (0.0001f > (real)fabs(distance))
	{
		distance = 0.0f;
	}
	else
	{
		real scale = 1.0f / distance;

		direction.i = scale * direction.i;
		direction.j = direction.j * scale;
		direction.k = direction.k * scale;
	}
	if (*(real *)(seat + 0x98) > distance &&
		*(real *)(seat + 0xa0) >= function_11cf50(&direction, &unit->unknown168) &&
		*(real *)(seat + 0x9c) >= 3.1415927f - function_11cf50(&direction, forward))
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
			return true;
		}
	}
	return false;
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
	point3f position = unit->position;
	function_11bf90(unit_index, &position);
	vector3f velocity;
	velocity.i = unit->linear_velocity.i + direction.i;
	velocity.j = unit->linear_velocity.j + direction.j;
	velocity.k = unit->linear_velocity.k + direction.k;
	function_b75a0(unit_index, &position, 0, 0, 0, false);
	function_b77d0(unit_index, &velocity, g_4687a4);
	s_unit *again = UNIT_GET(unit_index);
	if (again->object_flags & 1)
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

	if (parent_index == NONE && !((unit->flags_10a >> 2) & 1))
	{
		if (unit->type == 0)
		{
			function_def60(point, unit_index, mode, origin, forward, offsets);
			return;
		}
		function_b9dd0(unit_index, &reference);
	}
	else
	{
		long location = NONE;

		if (unit->type == 0 && parent_index != NONE && UNIT_GET(parent_index)->type == 1)
		{
			function_f1070(parent_index, &location, 0, &reference, 0, 0);
		}
		if (location == NONE)
		{
			function_b9dd0(unit_index, &reference);
		}
	}
	if (mode > 0 && mode <= 3)
	{
		s_unit *holder = UNIT_GET(unit_index);
		short index = holder->current_weapon_index;

		if (mode != 3 || index == NONE || holder->weapon_object_indices[index] == NONE ||
			!function_101b80(holder->weapon_object_indices[index], 0, point))
		{
			s_object_marker marker;

			function_b8d30(unit_index, 0x4000095, &marker, 1, false);
			*point = marker.matrix.position;
		}
	}
	else
	{
		function_cafc0(unit_index, point);
	}
	real dx = origin->x - reference.x;
	real dy = origin->y - reference.y;
	real dz = origin->z - reference.z;
	point->x = point->x + dx;
	point->y = point->y + dy;
	point->z = point->z + dz;
}

/* fades the unit's two definition-timed values (+0x25c, the definition's
   +0x1b8 entries) toward 1 while their condition holds, else toward 0:
   the first while +0x248 is set (or flag 1), the second while +0x24c is
   set apart from it; whether any moved */
// @retail 0xc5eb0
bool function_c5eb0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	bool result = false;

	if ((*(dword *)(definition + 0xbc) >> 11) & 1)
	{
		return false;
	}
	function_d0c10(unit_index);
	function_ce0c0(unit_index);
	if ((unit->flags_134 >> 26) & 1 && g_4e6948->mode != 4 && unit->parent_index != NONE &&
		unit->parent_seat_index != NONE)
	{
		unit->flags_134 &= ~0x4000000;
		function_e68c0(0x1e, unit_index);
	}
	for (long i = 0; i < *(long *)(definition + 0x1b8); i++)
	{
		real const *time = (real const *)(*(byte **)(definition + 0x1bc) + i * 8);
		bool active;

		switch (i)
		{
		case 0:
			active = unit->unknown248 != NONE || (unit->flags_134 >> 1) & 1;
			break;
		case 1:
			active = unit->unknown24c != NONE && unit->unknown24c != unit->unknown248;
			break;
		default:
			active = false;
			break;
		}
		if (!((unit->flags_10a >> 2) & 1) && active)
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
	return result;
}

/* turns a vector into the frame of a player's seat (seats without bit 4):
   built from the vehicle's up and the world's axes */
// @retail 0xcb810
bool function_cb810(long unit_index, vector3f *vector)
{
	s_unit *unit = UNIT_GET(unit_index);

	if (unit->unknown13c == NONE)
	{
		return false;
	}
	short seat_index = unit->parent_seat_index;
	if (seat_index == NONE)
	{
		return false;
	}
	long parent_index = unit->parent_index;
	if ((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(parent_index)))[seat_index].flags >> 4) & 1)
	{
		return false;
	}
	vector3f forward;
	vector3f up;
	transform4x3f matrix;

	function_b9fc0(parent_index, &forward, &up);
	matrix.forward.i = g_4687b8->k * up.j - g_4687b8->j * up.k;
	matrix.forward.j = g_4687b8->i * up.k - g_4687b8->k * up.i;
	matrix.forward.k = g_4687b8->j * up.i - g_4687b8->i * up.j;
	if (function_30bf0(&matrix.forward) == 0.0f)
	{
		matrix.forward.i = g_4687bc->k * up.j - g_4687bc->j * up.k;
		matrix.forward.j = g_4687bc->i * up.k - g_4687bc->k * up.i;
		matrix.forward.k = g_4687bc->j * up.i - g_4687bc->i * up.j;
		function_30bf0(&matrix.forward);
	}
	matrix4x3_from_forward_and_up(&matrix, &matrix.forward, &up);
	real i = vector->i;
	real j = vector->j;
	real k = vector->k;
	vector->i = matrix.up.i * k + matrix.left.i * j + i * matrix.forward.i;
	vector->j = matrix.up.j * k + matrix.left.j * j + matrix.forward.j * i;
	vector->k = matrix.up.k * k + matrix.left.k * j + matrix.forward.k * i;
	return true;
}

/* how the unit reacts to damage it took: sets the report's reaction
   (+0x50) from 0xc9770, after the biped's knockdown checks */
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
	if (*(short *)damage == 3 && is_biped && (*(dword *)(local_98b918_2 + 0x1f0) >> 10) & 1)
	{
		function_e3f00(unit_index);
	}
	*(long *)(bytes + 0x50) = 0;
	bool ignore = false;
	bool held = false;
	bool force = false;
	if ((data->flags & 0x10) || (damage[4] & 0x10) || (unit->flags_134 >> 19) & 1)
	{
		ignore = true;
	}
	header = (byte *)g_4e0300->data + (unit_index & 0xffff) * 0xc;
	dword type_bit = 1 << header[3];
	if (((type_bit & 1) && *((byte *)((s_unit_header *)header)->unit + 0x3dc) == 2) || (type_bit & 2))
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
		if ((*(dword *)(local_98b918_2 + 0xbc) >> 7) & 1 && !(damage[4] & 4))
		{
			held = true;
		}
		if ((unit->flags_134 >> 5) & 1)
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

/* the unit's exported function values by name (0 to 1), and whether each
   is active; two names come from its weapon (0xff5f0) */
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
		goto store;
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
		goto store;
	case 0x140005a3:
		result = unit->unknown24c != NONE ? 1.0f : 0.0f;
		goto store;
	case 0x160005a0:
		result = unit->unknown264;
		break;
	case 0x1800059e:
		result = unit->unknown1f8 * (1.0f / 255.0f);
		break;
	default:
	{
		bool found = false;
		long weapon_name;

		if (name == 0xb0005a9)
		{
			weapon_name = 0xb0005a9;
		}
		else if (name == 0x130005a8)
		{
			weapon_name = 0xc000569;
		}
		else
		{
			return found;
		}
		short index = unit->current_weapon_index;
		if (index != NONE && unit->weapon_object_indices[index] != NONE)
		{
			found = function_ff5f0(unit->weapon_object_indices[index], weapon_name, value, active);
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
   speed in a cone around the unit's aim, and deletes it in state 1 when
   it has nowhere to go */
// @retail 0xce6b0
void function_ce6b0(long unit_index, long name, long object_index, real scale)
{
	s_unit *object = UNIT_GET(object_index);

	if (name == NONE || name == 0)
	{
		s_object_marker marker;

		name = function_b8d30(unit_index, 0x4000023, &marker, 1, false) ? 0x4000023 : 0xa000798;
	}
	function_b8ee0(unit_index, name, object_index, 0);
	function_b9a90(object_index);
	function_10b360(object_index);
	real speed = (real)random_next(&g_4e7408->unknown0) * (1.0f / 65535.0f) * (1.2f - 0.8f) + 0.8f;
	object->linear_velocity = *g_4687a4;
	*(vector3f *)((byte *)object + 0x94) = *g_4687a4;
	vector3f direction;
	random_vector_in_cone(&UNIT_GET(unit_index)->unknown168, &direction, &g_4e7408->unknown0, 0.0f, scale * 0.3926991f);
	speed *= scale;
	vector3f velocity;
	function_ba1d0(unit_index, &velocity, 0);
	velocity.i += direction.i * speed;
	velocity.j += direction.j * speed;
	velocity.k += direction.k * speed;
	if (!((object->unknownc0 >> 1) & 1))
	{
		real length_squared = velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i;

		if (length_squared > 16.0f)
		{
			real factor = 4.0f / (real)sqrt(length_squared);

			velocity.i *= factor;
			velocity.j *= factor;
			velocity.k *= factor;
		}
	}
	object->unknown14c = unit_index;
	function_10cf80(&velocity, object_index, false);
	point3f point;
	if (!function_bc1d0(object_index, function_b9ef0(unit_index, &point)))
	{
		if (g_4e6948->mode == 4 && UNIT_GET(object_index)->unknown0d4 != NONE)
		{
			return;
		}
		if (g_4e6948->state == 1)
		{
			function_b8540(object_index);
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
		if ((unit->flags_134 >> 29) & 1)
		{
			function_c5890(unit_index, 4);
			unit->flags_134 &= ~0x20000000;
		}
		else
		{
			function_c5890(unit_index, 5);
			unit->flags_134 |= 0x20000000;
			unit->unknown268 = 4.0f;
		}
	}
	if ((unit->flags_134 >> 29) & 1)
	{
		if (unit->unknown268 > 0.0f)
		{
			unit->unknown268 -= g_510c54->rate;
			if (0.0f >= unit->unknown268)
			{
				unit->unknown268 = 0.0f;
			}
		}
		if (unit->parent_index != NONE || (unit->flags_10a >> 2) & 1 ||
			(!g_5107e8->flag0 && unit->unknown268 == 0.0f))
		{
			unit->flags_134 &= ~0x20000000;
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

/* finds the vehicle's driver (+0x248) and gunner (+0x24c) among its
   riders, in two passes (plain seats, then seat groups whose rider is in
   place), recursing into ridden units; tells 0x1bbdf0 when the driver
   changed */
// @retail 0xcc810
void __stdcall function_cc810(long vehicle_index)
{
	s_unit *vehicle = UNIT_GET(vehicle_index);
	byte *definition = UNIT_DEFINITION_GET(vehicle);
	long old_driver_index = vehicle->unknown248;

	vehicle->unknown248 = NONE;
	vehicle->unknown24c = NONE;
	if ((*(dword *)(definition + 0xbc) >> 26) & 1 && vehicle->parent_index != NONE && vehicle->actor_index == NONE &&
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
			if (seat_index == NONE)
			{
				gunner = (*(dword *)(UNIT_DEFINITION_GET(child) + 0xbc) >> 25) & 1;
				goto check_gunner;
			}
			{
				s_unit_seat_definition *seats = UNIT_SEATS(definition);
				dword flags = *(dword *)&seats[seat_index].flags;
				bool grouped = (flags >> 11) & 1;
				bool assign;

				if (grouped && !((flags >> 13) & 1))
				{
					goto recurse;
				}
				driver = (flags >> 2) & 1;
				gunner = (flags >> 3) & 1;
				if ((flags >> 18) & 1 && child->unknown13c != NONE)
				{
					gunner = true;
				}
				if (pass == 0)
				{
					if (!grouped && !(*((byte *)child + child->unknown346 + 8) & 1))
					{
						goto check_driver;
					}
					assign = false;
				}
				else
				{
					assign = grouped && (*(dword *)((byte *)child + child->unknown346 + 4) >> 31) & 1;
				}
				if (grouped)
				{
					short other_seat_index = *(short *)((byte *)&seats[seat_index] + 0x3e);

					if (other_seat_index != NONE)
					{
						dword other_flags = *(dword *)&seats[other_seat_index].flags;

						if ((other_flags >> 2) & 1)
						{
							driver = true;
						}
						if ((other_flags >> 3) & 1)
						{
							gunner = true;
						}
					}
				}
				if (!assign)
				{
					goto recurse;
				}
			}
		check_driver:
			if (driver && !((vehicle->flags_134 >> 1) & 1) && vehicle->unknown248 == NONE)
			{
				vehicle->unknown248 = child_index;
				if (gunner && vehicle->unknown24c == NONE)
				{
					vehicle->unknown24c = child_index;
				}
				goto recurse;
			}
		check_gunner:
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
// @retail 0xc7300
void __stdcall function_c7300(real distance, real velocity, real speed_limit, real acceleration, s_unit_motion *motion)
{
	bool done = (real)fabs(distance) < 0.001f && (real)fabs(velocity) < 0.001f;

	motion->position = distance;
	motion->velocity = velocity;
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
	real negative = 0.0f - acceleration;
	motion->deceleration = acceleration;
	motion->acceleration = negative;
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

/* where a move by Havok's phantom check (0x1d48f0) ends */
struct s_unit_move_result
{
	s_location location;
	point3f position;
};

/* moves an object to a point (its own center by default) while the
   physics allow it: finds a clear spot near the target with its Havok
   component (made for the check when it has none, tried with a doubling
   radius), then places it there; without physics it just places it */
// @retail 0xc5460
bool __stdcall function_c5460(long object_index, long ignore_index, point3f const *position, point3f *result,
	long a5, real radius, long a7)
{
	byte *header = (byte *)g_4e0300->data + (object_index & 0xffff) * 0xc;
	s_unit *object = ((s_unit_header *)header)->unit;
	bool moved = false;

	if (!g_47f058)
	{
		if (position)
		{
			function_b75a0(object_index, position, 0, 0, 0, false);
			if (result)
			{
				*result = *position;
			}
		}
		return moved;
	}
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
		short rigid_body_index = havok_component_main_rigid_body_index_get(component);

		if (rigid_body_index != NONE)
		{
			s_unit_move_result move;
			point3f center;
			point3f target;
			vector3f offset;
			long root_index = NONE;
			point3f *root_point = 0;
			point3f root_origin;

			function_b9dd0(object_index, &center);
			function_b9ef0(object_index, &target);
			offset.i = target.x - center.x;
			offset.j = target.y - center.y;
			offset.k = target.z - center.z;
			target = position ? *position : center;
			if (object->unknown13c != NONE && ignore_index != NONE && function_baf80(ignore_index) == ignore_index)
			{
				root_index = ignore_index;
				root_point = function_b9ef0(ignore_index, &root_origin);
			}
			for (short attempt = 0; attempt < 3; attempt++)
			{
				if (function_1d48f0(&move.position, component, rigid_body_index, header[3] ? 0xc : 9, &target, &offset, &move.location, *(real *)&a5,
					radius, (bool)a7, root_point, root_index))
				{
					if (result)
					{
						*result = move.position;
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
				else if ((object->unknownc0 >> 6) & 1)
				{
					function_1d1540(havok_component_get(object->unknownb4));
				}
			}
			if (moved)
			{
				function_b75a0(object_index, &move.position, 0, 0, &move.location, false);
				function_b9b90(object_index, false);
			}
		}
	}
	return moved;
}

/* clamps a direction to the unit's yaw and pitch limits for aiming (or,
   when asked, for looking, relative to its aim) in its own frame; whether
   it changed */
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
	if (!enabled)
	{
		return clamped;
	}
	vector3f forward;
	vector3f up;

	function_b9fc0(unit_index, &forward, &up);
	if (looking)
	{
		vector3f left;

		up = unit->unknown168;
		left.i = up.k * forward.j - forward.k * up.j;
		left.j = forward.k * up.i - up.k * forward.i;
		left.k = forward.i * up.j - forward.j * up.i;
		forward.i = left.k * up.j - left.j * up.k;
		forward.j = up.k * left.i - left.k * up.i;
		forward.k = left.j * up.i - left.i * up.j;
		if (function_30bf0(&forward) == 0.0f)
		{
			function_30bf0(function_11d000(&up, &forward));
		}
	}
	transform4x3f matrix;
	function_1420f0(&matrix, g_468788, &forward, &up);
	real x = matrix.forward.k * direction->k + matrix.forward.j * direction->j + matrix.forward.i * direction->i;
	real y = matrix.left.k * direction->k + matrix.left.j * direction->j + matrix.left.i * direction->i;
	real z = matrix.up.k * direction->k + matrix.up.j * direction->j + matrix.up.i * direction->i;
	real yaw = (real)atan2(y, x);
	real pitch = (real)atan2(z, (real)sqrt(y * y + x * x));

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
	return clamped;
}

/* where a unit's view or weapon is: its aim (when asked, the unit's own),
   a point projected onto the aim line from its center, moved by offsets
   along the aim and its left and up, then held clear of the world from
   its center; and its root object's velocity */
// @retail 0xc8290
void __stdcall function_c8290(vector3f *aim, point3f *point, long unit_index, vector3f *velocity, real const *offsets,
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
		real along = aim->i * (point->x - center.x) + aim->k * (point->z - center.z) + aim->j * (point->y - center.y);

		point->x = aim->i * along + center.x;
		point->y = aim->j * along + center.y;
		point->z = aim->k * along + center.z;
	}
	if (offsets)
	{
		vector3f left;
		vector3f up;

		left.i = aim->k * g_4687b0->j - g_4687b0->k * aim->j;
		left.j = g_4687b0->k * aim->i - aim->k * g_4687b0->i;
		left.k = aim->j * g_4687b0->i - aim->i * g_4687b0->j;
		if (function_30bf0(&left) == 0.0f)
		{
			left = *g_4687ac;
		}
		up.i = aim->j * left.k - aim->k * left.j;
		up.j = aim->k * left.i - aim->i * left.k;
		up.k = aim->i * left.j - aim->j * left.i;
		function_30bf0(&up);
		point->x += aim->i * offsets[0];
		point->y += aim->j * offsets[0];
		point->z += aim->k * offsets[0];
		point->x += left.i * offsets[1];
		point->y += left.j * offsets[1];
		point->z += left.k * offsets[1];
		point->x += up.i * offsets[2];
		point->y += up.j * offsets[2];
		point->z += up.k * offsets[2];
	}
	if (clip)
	{
		point3f clipped;

		if (function_16a7c0(&center, point, function_baf40(unit_index), NONE, &clipped))
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

/* the unit's wandering direction: its actor's (else the default vector)
   turned about the world's up by the angle +0x2d8, which drifts by a
   random turn rate (+0x2dc) kept away from its limits */
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
	real sine = (real)sin(unit->unknown2d8);
	real cosine = (real)cos(unit->unknown2d8);
	vector3f const *axis = g_4687b0;
	real along = (axis->i * direction->i + axis->j * direction->j + axis->k * direction->k) * (1.0f - cosine);
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;

	direction->i = axis->i * along + x * cosine - (y * axis->k - z * axis->j) * sine;
	direction->j = axis->j * along + y * cosine - (z * axis->i - axis->k * x) * sine;
	direction->k = z * cosine + axis->k * along - (axis->j * x - axis->i * y) * sine;
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
	if (unit->unknown13c != NONE && (vehicle->flags_134 >> 12) & 1)
	{
		return;
	}
	for (long i = 0; i < count; i++)
	{
		s_object_seat *entry = &seats[i];
		long candidate_index = entry->object_index;
		s_unit *candidate = UNIT_GET(candidate_index);

		if (unit->unknown13c != NONE && (candidate->flags_134 >> 12) & 1)
		{
			continue;
		}
		short seat = entry->seat_index;
		dword flags = *(dword *)&entry->definition->flags;
		if (unit->unknown13c == NONE ? (flags >> 17) & 1 : (flags >> 16) & 1)
		{
			continue;
		}
		if (!function_c6fb0(candidate_index, unit_index, seat))
		{
			continue;
		}
		point3f marker_position;
		point3f entry_position;
		if (function_c7160(unit_index, seat, (long)&marker_position, candidate_index, (long)&entry_position) == NONE)
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

		if ((flags >> 9) & 1 && candidate->unknown248 == NONE)
		{
			continue;
		}
		long label = entry->definition->label;
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
			long actor_index;

			if (blocker_index == NONE || (actor_index = UNIT_GET(blocker_index)->actor_index) == NONE ||
				!function_1c9500(unit_index, actor_index, 0))
			{
				continue;
			}
			rank = 1;
		}
		if (rank <= 0)
		{
			continue;
		}
		bool driver = (flags >> 2) & 1;
		real scale = *flag && !driver ? 1.5f : 1.0f;
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

/* records damage the unit took from a player or object (adding to its
   entry, else filling a free one or replacing the least recent of the
   weaker ones); when asked, counts a hit from an ally against the unit
   or vehicle seat responsible (betrayal, reset after four seconds) */
// @retail 0xca700
void __stdcall function_ca700(long unit_index, real amount, short type, bool notify, long player_index, short team,
	long object_index, bool flag)
{
	s_unit *unit = UNIT_GET(unit_index);
	long now = g_510c54->game_time;
	s_unit_damage_source *sources = (s_unit_damage_source *)((byte *)unit + 0x2f0);
	bool found = false;

	for (long i = 0; i < 4; i++)
	{
		if ((player_index != NONE && sources[i].player_index == player_index) ||
			(object_index != NONE && sources[i].object_index == object_index))
		{
			sources[i].amount += amount;
			sources[i].time = now;
			unit->unknown330[i] = flag;
			found = true;
		}
	}
	if (!found)
	{
		short slot;

		for (slot = 0; slot < 4; slot++)
		{
			if (sources[slot].time == NONE)
			{
				break;
			}
		}
		if (slot == 4)
		{
			short weakest = 0;

			if (sources[1].amount < sources[weakest].amount)
			{
				weakest = 1;
			}
			if (sources[2].amount < sources[weakest].amount)
			{
				weakest = 2;
			}
			if (sources[3].amount < sources[weakest].amount)
			{
				weakest = 3;
			}
			slot = weakest != 0 ? 0 : NONE;
			for (short i = 1; i < 4; i++)
			{
				if (weakest != i && (slot == NONE || (dword)sources[i].time < (dword)sources[slot].time))
				{
					slot = i;
				}
			}
		}
		sources[slot].object_index = object_index;
		sources[slot].player_index = player_index;
		sources[slot].amount = amount;
		sources[slot].time = now;
		unit->unknown330[slot] = flag;
	}
	if (!notify || team == NONE || !function_1df560(team, unit->unknown138))
	{
		return;
	}
	long responsible_index = NONE;
	s_unit *responsible = 0;
	byte *player = datum_get_inlined(g_4e8c24, player_index);
	if (player && *(long *)(player + 0x2c) != NONE)
	{
		responsible_index = *(long *)(player + 0x2c);
		responsible = UNIT_GET(responsible_index);
	}
	if (!responsible)
	{
		byte *header = datum_get_inlined(g_4e0300, object_index);

		responsible_index = object_index;
		if (header && (1 << header[3]) & 3)
		{
			responsible = ((s_unit_header *)header)->unit;
		}
		if (!responsible)
		{
			return;
		}
	}
	long seat_index = type == 9 ? responsible->unknown248 : responsible->unknown24c;
	if (seat_index != NONE)
	{
		responsible_index = seat_index;
		responsible = UNIT_GET(seat_index);
	}
	if ((responsible->flags_10a >> 2) & 1)
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
		responsible->unknown2ea >= (UNIT_GET(responsible_index)->unknown13c != NONE ? 5 : 3))
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
	s_unit *unit = UNIT_GET(unit_index);
	bool still = false;
	transform4x3f matrix;
	s_unit_camera_data camera;

	unit->flags_134 &= ~0x4000000;
	unit->flags_134 &= ~0x2000000;
	if ((unit->flags_10a >> 2) & 1 && (UNIT_GET(unit_index)->type != 0 || !function_e4050(unit_index)))
	{
		still = true;
	}
	if (function_d1080(unit_index, &matrix, &camera) && !still)
	{
		real ticks = g_510c54->field_2_3;
		vector3f velocity;
		vector3f acceleration;
		vector3f left;

		left.i = matrix.forward.k * matrix.up.j - matrix.up.k * matrix.forward.j;
		left.j = matrix.up.k * matrix.forward.i - matrix.forward.k * matrix.up.i;
		left.k = matrix.forward.j * matrix.up.i - matrix.up.j * matrix.forward.i;
		velocity.i = (matrix.position.x - unit->unknown270.x) * ticks;
		velocity.j = (matrix.position.y - unit->unknown270.y) * ticks;
		velocity.k = (matrix.position.z - unit->unknown270.z) * ticks;
		acceleration.i = (velocity.i - unit->unknown27c.i) * ticks;
		acceleration.j = (velocity.j - unit->unknown27c.j) * ticks;
		acceleration.k = (velocity.k - unit->unknown27c.k) * ticks;
		unit->unknown294.i = (matrix.forward.k * acceleration.k + matrix.forward.j * acceleration.j +
			matrix.forward.i * acceleration.i) * camera.unknown00;
		unit->unknown294.j = (left.k * acceleration.k + left.j * acceleration.j + left.i * acceleration.i) *
			camera.unknown04;
		unit->unknown294.k = (matrix.up.k * acceleration.k + matrix.up.j * acceleration.j +
			matrix.up.i * acceleration.i) * camera.unknown08;
		real forward = (real)fabs(unit->unknown294.i);
		real sideways = (real)fabs(unit->unknown294.j);
		real vertical = (real)fabs(unit->unknown294.k);
		real limit = camera.unknown0c;

		if (limit > 0.0f && (forward > limit || sideways > limit || vertical > limit))
		{
			unit->flags_134 |= 0x2000000;
		}
		limit = camera.unknown10;
		if (limit > 0.0f && unit->parent_index != NONE && unit->parent_seat_index != NONE &&
			(g_4e6948->state != 2 || !function_138880()) &&
			(forward > limit || sideways > limit || vertical > limit))
		{
			unit->flags_134 |= 0x4000000;
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
	if ((unit->flags_134 >> 28) & 1)
	{
		keep = 0.95f;
		take = 0.05f;
		unit->flags_134 &= ~0x10000000;
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
	if (g_4e6948->mode == 4)
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

			if ((*(dword *)&seat->flags >> 11) & 1)
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

				if ((*(dword *)(local_67e06b + 0x12c) >> 15) & 1)
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
		}
		else
		{
			point = unit->unknown030;
		}
		break;
	}
	case 1:
		function_b9dd0(unit_index, &point);
		point.x += g_4687b0->i * 0.1f;
		point.y += g_4687b0->j * 0.1f;
		point.z += g_4687b0->k * 0.1f;
		break;
	default:
		point = unit->unknown030;
		break;
	}
	s_type_1e6529 damage;

	function_d6660(&damage, damage_definition_index);
	damage.material_index = NONE;
	object_get_root_location(unit_index, (s_location *)&damage.unknown1c);
	object_get_damage_owner(unit_index, &damage.owner);
	damage.position = point;
	damage.origin = unit->unknown030;
	damage.cone_direction = forward;
	*(byte *)&damage.unknown84 = 3;
	damage.unknown18 = unit_index;
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

/* whether the unit may enter the vehicle's seat: not its own, allowed for
   players or AIs (bits 16, 17), its weapons fit (bits 2, 3), the seat
   free, its seat group's partner friendly and willing, no enemy riding
   (outside state 2), and for players a clear line to the vehicle; also
   the seat's occupant and whether the seat is grouped */
// @retail 0xc92c0
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *blocker_index, bool *grouped)
{
	s_unit *vehicle = UNIT_GET(vehicle_index);
	s_unit_seat_definition *seat = &UNIT_SEATS(UNIT_DEFINITION_GET(vehicle))[seat_index];
	s_unit *unit = UNIT_GET(unit_index);
	dword flags = *(dword *)&seat->flags;
	bool group = (flags >> 11) & 1;
	bool occupied = false;
	long occupant_index = NONE;
	bool result = true;
	long other_seat_index = group ? *(short *)((byte *)seat + 0x3e) : NONE;
	bool partner_friendly = false;
	bool partner_willing = false;
	bool enemy_aboard = false;

	if (unit_index == vehicle_index)
	{
		result = false;
	}
	if (unit->unknown13c != NONE && (vehicle->flags_134 >> 12) & 1)
	{
		result = false;
	}
	if (unit->unknown13c == NONE ? (flags >> 17) & 1 : (flags >> 16) & 1)
	{
		result = false;
	}
	s_object_child_iterator iterator;
	function_d0620(vehicle_index, &iterator);
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
			if (!function_1df560(rider->unknown138, unit->unknown138))
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
			function_1df560(rider->unknown138, unit->unknown138))
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
				(((flags >> 2) & 1 && function_101380(weapon_index)) || ((flags >> 3) & 1 && function_1013e0(weapon_index))))
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
			if ((flags >> 14) & 1 && !enemy_aboard)
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
		long root_index = NONE;
		for (long index = vehicle_index; index != NONE; index = UNIT_GET(index)->parent_index)
		{
			root_index = index;
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
			return false;
		}
	}
	return result;
}

/* the unit object type's creation callback: resets the unit's fields,
   allocates its three state blocks (+0x33c, +0x340, +0x344; out of memory
   otherwise), takes its team from the creation data (in campaign, the
   definition's when unset), starts its animation and finds its two
   animation graph entries */
// @retail 0xc4410
bool __stdcall function_c4410(long unit_index, void const *creation, bool *out_of_memory)
{
	byte const *data = (byte const *)creation;
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);

	unit->unknown238 = NONE;
	unit->weapon_object_indices[0] = NONE;
	unit->weapon_object_indices[1] = NONE;
	unit->weapon_object_indices[2] = NONE;
	unit->weapon_object_indices[3] = NONE;
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
	unit->unknown1c8.unknown1c = 0.0f;
	unit->unknown1c8.unknown20 = 0.0f;
	unit->unknown1c8.unknown00 = NONE;
	unit->unknown1c8.unknown04 = NONE;
	unit->unknown1c8.unknown08 = NONE;
	unit->unknown1c8.unknown18 = 0;
	s_unit_damage_source *sources = (s_unit_damage_source *)((byte *)unit + 0x2f0);
	for (dword i = 0; i < 4; i++)
	{
		sources[i].time = NONE;
		sources[i].amount = 0.0f;
		sources[i].object_index = NONE;
		sources[i].player_index = NONE;
		unit->unknown330[i] = false;
	}
	if (UNIT_GET(unit_index)->animation_offset == NONE)
	{
		return false;
	}
	if (!function_bc380(unit_index, 0x33c, 0x124, 0) || !function_bc380(unit_index, 0x340, 0x58, 0) ||
		!function_bc380(unit_index, 0x344, 0x44, 0))
	{
		*out_of_memory = true;
		return false;
	}
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
	short grenade_type = *(short *)(definition + 0x1b4);
	if (grenade_type >= 0 && grenade_type < 2 && *(short *)(definition + 0x1b6) >= 0)
	{
		unit->grenade_counts[grenade_type] = (char)definition[0x1b6];
	}
	unit->object_flags |= 0x1000;
	unit->unknown138 = *(short *)(data + 0x64);
	if (g_4e6948->state == 1 && (unit->unknown138 == 0 || unit->unknown138 == NONE))
	{
		unit->unknown138 = *(short *)(definition + 0xc0);
	}
	function_10f430(unit_index, 0x7000001, 0x7000101, 0x7000101, 0x7000001, 0.0f, false, 2);
	char index = NONE;
	long graph_index = *(long *)(definition + 0x38);
	if (graph_index != NONE)
	{
		byte *graph = g_4e3b44[graph_index & 0xffff].bytes;

		for (long i = 0; i < *(long *)(graph + 0x78); i++)
		{
			if (*(long *)(*(byte **)(graph + 0x7c) + i * 0x5c) == 0x80001a2)
			{
				index = (char)i;
				break;
			}
		}
	}
	unit->unknown1f6 = index;
	index = NONE;
	if (graph_index != NONE)
	{
		byte *graph = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;

		for (long i = 0; i < *(long *)(graph + 0x78); i++)
		{
			if (*(long *)(*(byte **)(graph + 0x7c) + i * 0x5c) == 0x90001a3)
			{
				index = (char)i;
				break;
			}
		}
	}
	unit->unknown1f7 = index;
	return true;
}

/* gives the unit a weapon (unless it is held or flagged, or the unit may
   not ready it) by mode: 0 and 1 into a free slot (1 readies it), 2 after
   dropping all, 3 and 4 into the first hand after lowering the second
   (4 also changing weapons), 5 to 7 into the second hand beside a
   matching first (6 lowering it first, 7 readying it); tells the player
   code */
// @retail 0xcd0c0
bool __stdcall function_cd0c0(long unit_index, long weapon_index, short mode)
{
	s_unit *weapon = UNIT_GET(weapon_index);
	s_unit *unit = UNIT_GET(unit_index);
	bool result = false;
	bool ready;
	short hand;
	s_unit_request request;

	if ((weapon->object_flags >> 7) & 1 || *((byte *)weapon + 0x12c) & 1 ||
		!function_cd6a0(unit_index, NONE, weapon_index))
	{
		return result;
	}
	ready = true;
	if (mode == 2)
	{
		function_ccff0(unit_index);
		hand = 0;
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
					return result;
				}
			}
		}
		else if (mode == 5 || mode == 6 || mode == 7)
		{
			long first_index = function_cbd50(unit_index, UNIT_GET(unit_index)->current_weapon_index);

			if (first_index == NONE || !function_101640(first_index) || !function_101640(weapon_index))
			{
				return result;
			}
			ready = true;
			if (mode == 6)
			{
				memset(&request, 0, sizeof(request));
				request.type = 0x13;
				request.type17.unknown4 = true;
				ready = function_e6900(unit_index, &request);
				if (!ready)
				{
					return result;
				}
			}
		}
		hand = mode == 5 || mode == 6 || mode == 7 ? 1 : 0;
	}
	if (!ready)
	{
		return result;
	}
	bool hands[2];
	if (!function_cd7b0(unit_index, weapon_index, hands) || !hands[hand])
	{
		return result;
	}
	short slot_index = function_cd620(unit_index);
	if (slot_index == NONE)
	{
		return result;
	}
	if (weapon->parent_index != NONE)
	{
		function_b9a90(weapon_index);
	}
	function_cea70(NONE, weapon_index, slot_index, unit_index);
	switch (mode)
	{
	case 0:
	case 1:
		unit->unknown216 = (char)function_cdeb0(unit_index, NONE, unit->current_weapon_index, 0);
		if (mode == 1 && unit->unknown216 == slot_index)
		{
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
		unit->unknown216 = (char)slot_index;
		function_e68c0(8, unit_index);
		break;
	case 5:
	case 6:
		unit->unknown217 = (char)slot_index;
		function_e68c0(0x12, unit_index);
		break;
	case 7:
		memset(&request, 0, sizeof(request));
		unit->unknown217 = (char)slot_index;
		request.type = 0x12;
		request.type17.unknown4 = true;
		request.type17.unknown5 = true;
		function_e6900(unit_index, &request);
		break;
	}
	if (unit->unknown13c != NONE)
	{
		function_1520f0(unit->unknown13c);
	}
	if (mode != 1 && mode != 7)
	{
		function_a8a30(unit_index, weapon->definition_index);
	}
	return true;
}

/* the unit's death by a damage type, from a direction: picks the side it
   fell toward (front, back, left, right), plays its death (0x10ff40) and,
   from type 3, sets its ragdoll timer +0x1f5, its flags and (when the
   death had no animation) its deadness; a free biped turns to face it */
// @retail 0xc98a0
void function_c98a0(point2f const *direction, long unit_index, long type, short value)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
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
				angle = function_11cc90((vector2f const *)&facing, (vector2f const *)&toward);
				if (!((*(dword *)(definition + 0xbc) >> 9) & 1) && unit->type == 0 && unit->parent_index == NONE)
				{
					short state;

					function_e70b0(unit_index, &state);
					if (state == NONE)
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
		return;
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
		long first_index = holder->current_weapon_index != NONE ? holder->weapon_object_indices[holder->current_weapon_index] : NONE;
		long second_index = holder->next_weapon_index != NONE ? holder->weapon_object_indices[holder->next_weapon_index] : NONE;

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
			if ((*(dword *)(definition + 0xbc) >> 1) & 1 && !function_a7670(unit_index))
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
}

/* sends a unit request straight to its handler and tells the player's
   controller how it went (as 0xce520 does for request 9) */
static void unit_perform_request(long unit_index, long type)
{
	s_unit_request request;

	memset(&request, 0, sizeof(request));
	request.type = type;
	function_b7360(unit_index);
	bool result = g_4677c8[type]->perform(unit_index, &request);
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
// @retail 0xcaa60
void __stdcall function_caa60(long unit_index, long object_index, point3f const *point, bool knocked, bool force)
{
	s_unit *unit = UNIT_GET(unit_index);

	if ((1 << unit->type) & 1)
	{
		function_e4a20(point, unit_index, object_index, knocked);
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
		function_14cad0(player_index, unit_index);
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
	unit->flags_134 &= ~2;
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
	UNIT_GET(unit_index)->flags_134 &= ~0x200000;
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
		function_ce470(0, holder->unknown238, unit_index);
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

/* the rounds a weapon holds in its magazines (+0x22a, 0x10 bytes each), or
   its definition's (+0x2c4, 0x5c bytes each) for weapons 0x106030 says
   refill; in a multiplayer engine mode 7 with option 0x20, the dropped
   weapon of a player on a flagged team counts by its definition too */
static long unit_weapon_rounds(long weapon_index, long magazine_count, bool check_owner)
{
	s_unit *weapon = UNIT_GET(weapon_index);
	long total = 0;

	for (long i = 0; i < magazine_count; i++)
	{
		bool by_definition;
		short rounds;

		if (check_owner)
		{
			by_definition = false;
			if (g_4e6948->state == 2 && *((byte *)weapon + 0x12c) & 1)
			{
				long owner_index = *(long *)((byte *)weapon + 0x154);

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
		}
		else
		{
			by_definition = function_106030(weapon_index);
		}
		if (by_definition)
		{
			byte *definition = UNIT_DEFINITION_GET(weapon);

			if (*(long *)(definition + 0x2c0) <= i)
			{
				continue;
			}
			rounds = *(short *)(*(byte **)(definition + 0x2c4) + i * 0x5c + 0xc);
		}
		else
		{
			rounds = *(short *)((byte *)weapon + 0x22a + i * 0x10);
		}
		if (check_owner || rounds > 0)
		{
			total += rounds;
		}
	}
	return total;
}

/* which of the unit's hands (0, 1) and slots (2, 3) may take the weapon:
   free hands, a second hand for dual-wieldable weapons, a matching weapon
   replaced only by a fresher one, an empty weapon only when it adds
   rounds, not a fully used one for AIs; the player code and the engine
   have the last word, then the weapon learns its holder */
// @retail 0xcd7b0
bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes)
{
	bool result = false;

	if (!function_cd6a0(unit_index, NONE, weapon_index))
	{
		return result;
	}
	s_unit *unit = UNIT_GET(unit_index);
	s_unit *weapon = UNIT_GET(weapon_index);
	byte *local_67e06b = UNIT_DEFINITION_GET(weapon);
	long magazine_count = *(long *)(local_67e06b + 0x2c0);
	long first_slot = NONE;
	bool weapon_empty = false;
	bool has_rounds = false;
	long total = 0;

	if (magazine_count > 0)
	{
		bool empty = false;
		bool loaded = false;
		byte *magazine = *(byte **)(local_67e06b + 0x2c4) + 8;
		short *state = (short *)((byte *)weapon + 0x22a);

		for (long i = magazine_count; i != 0; i--, magazine += 0x5c, state += 8)
		{
			if (*(short *)magazine > 0)
			{
				if (state[1] == 0 && state[0] == 0)
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
	dword weapon_flags = *(dword *)(UNIT_DEFINITION_GET(UNIT_GET(weapon_index)) + 0x12c);
	bool dual_wieldable = (weapon_flags >> 22) & 1 || (weapon_flags >> 23) & 1;
	bool second_hand_only = (weapon_flags >> 23) & 1;
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
				long rounds = unit_weapon_rounds(*slot, magazine_count, false);
				if (rounds > 0)
				{
					has_rounds = true;
					total += rounds;
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

			if ((*(dword *)(UNIT_DEFINITION_GET(held_weapon) + 0x12c) >> 3) & 1 || state == 1 || state == 2)
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

			if (!((held_flags >> 22) & 1) && !((held_flags >> 23) & 1))
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
				bool matches = weapon->definition_index == held_definition_index ||
					*(long *)(local_67e06b + 0x304) == held_definition_index;

				if (matches && *(real *)((byte *)held_weapon + 0x184) > *(real *)((byte *)weapon + 0x184))
				{
					continue;
				}
				if (matches || !hand_dual)
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
				long held_count = *(long *)(UNIT_DEFINITION_GET(UNIT_GET(held[hand])) + 0x2c0);

				if (unit_weapon_rounds(held[hand], held_count, true) == total)
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
			return result;
		}
	}
	return false;
}

/* the unit's control update: counts down its ragdoll (+0x1f5) and two
   short timers, crouches or stands by the control bits, keeps its weapon
   and grenade choices, zoom (with the weapon's or the globals' zoom
   sounds), and passes each hand's trigger state to its weapon; whether
   anything changed */
// @retail 0xc58f0
bool __stdcall function_c58f0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	bool changed = false;

	if (!((*(dword *)(definition + 0xbc) >> 11) & 1))
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
		if ((unit->flags_134 >> 1) & 1)
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
	if (!((*(dword *)(definition + 0xbc) >> 10) & 1) && !((unit->flags_10a >> 2) & 1))
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
		char zoom = function_c85c0(unit_index) ? unit->unknown241 : NONE;
		if (unit->unknown240 != zoom)
		{
			unit->unknown240 = zoom;
			if (zoom == NONE)
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

				if (zoom == NONE)
				{
					sound = *(long *)(local_67e06b + 0x278);
				}
				else
				{
					short levels = *(short *)(local_67e06b + 0x1fe);

					sound = *(long *)(local_67e06b + 0x270);
					if (levels > 1)
					{
						scale = (real)zoom / (real)(levels - 1);
					}
				}
			}
			else
			{
				byte *globals = *(byte **)((byte *)g_4e034c + 0x134);

				if (zoom == NONE)
				{
					sound = *(long *)(globals + 0xd0);
				}
				else
				{
					long levels = *(long *)(globals + 0xb8);

					sound = *(long *)(globals + 0xc8);
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
				function_189210(unit_index, 0x4000095, &play);
			}
		}
	}
	if (!((*(dword *)(definition + 0xbc) >> 11) & 1))
	{
		for (long hand = 0; hand < 2; hand++)
		{
			bool first = hand == 0;

			if ((&unit->current_weapon_index)[hand] == NONE)
			{
				continue;
			}
			s_unit *holder = UNIT_GET(unit_index);
			short index = (&holder->current_weapon_index)[hand];
			long weapon_index = index != NONE ? holder->weapon_object_indices[index] : NONE;
			bool held = function_110ab0(unit_index);

			if (unit->unknown1ec > 0)
			{
				held = false;
			}
			dword const *bits = (dword const *)((byte *)holder + holder->unknown346 + 4);
			long bit = first ? 8 : 0x12;
			if ((bits[0] >> 22) & 1 || (bits[0] >> 26) & 1 || (bits[0] >> 27) & 1 || (bits[bit >> 5] & (1 << (bit & 0x1f))))
			{
				held = true;
			}
			dword control = unit->unknown148;
			word flags;
			bool last;

			if (first)
			{
				flags = (word)(((control >> 16) & 1) << 1);
				flags = (control >> 17) & 1 ? flags | 4 : flags & ~4;
				flags = (control >> 18) & 1 ? flags | 8 : flags & ~8;
				flags = (control >> 19) & 1 ? flags | 0x10 : flags & ~0x10;
				flags = (control >> 20) & 1 ? flags | 1 : flags & ~1;
				last = (control >> 30) & 1;
			}
			else
			{
				flags = (word)(((control >> 21) & 1) << 1);
				flags = (control >> 22) & 1 ? flags | 4 : flags & ~4;
				flags = (control >> 23) & 1 ? flags | 8 : flags & ~8;
				flags = (control >> 24) & 1 ? flags | 0x10 : flags & ~0x10;
				flags = (control >> 25) & 1 ? flags | 1 : flags & ~1;
				last = (control >> 31) & 1;
			}
			flags = last ? flags | 0x80 : flags & ~0x80;
			flags = (control >> 29) & 1 ? flags | 0x100 : flags & ~0x100;
			flags = held ? flags | 0x20 : flags & ~0x20;
			flags = unit->unknown240 != NONE ? flags | 0x40 : flags & ~0x40;
			real rate = first ? *(real *)&unit->unknown1c0 : *(real *)&unit->unknown1c4;
			function_100430(weapon_index, flags, rate);
		}
		function_d1540(unit_index);
	}
	return changed;
}

/* a frame from a forward and an up: left = up x forward, at the default
   point (the frame 0xc7840 limits a turn in) */
static void unit_turn_frame(transform4x3f *frame, vector3f const *forward, vector3f const *up)
{
	frame->scale = 1.0f;
	frame->forward = *forward;
	frame->left.i = up->j * forward->k - up->k * forward->j;
	frame->left.j = up->k * forward->i - up->i * forward->k;
	frame->left.k = up->i * forward->j - up->j * forward->i;
	frame->up = *up;
	frame->position = *g_468788;
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

/* turns the unit's aim (+0x168) toward its desired aim (+0x15c) and its
   look (+0x18c) toward +0x180 at the definition's rates (scaled while it
   is in state 1 at +0x1bc), within its aim and look limits when they are
   on (0xc7840); records the aim's turn speeds (+0x243, +0x1f8); whether
   either moved */
// @retail 0xc60c0
bool __stdcall function_c60c0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);

	if ((*(dword *)(definition + 0xbc) >> 10) & 1 || (unit->flags_10a >> 2) & 1)
	{
		return false;
	}
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
	else if ((state[0] >> 5) & 1)
	{
		vector3f forward;
		vector3f up;
		transform4x3f frame;

		function_b9fc0(unit_index, &forward, &up);
		unit_turn_frame(&frame, &forward, &up);
		function_c7840(&unit->unknown15c, &unit->unknown168, &frame, g_510c54->rate, &unit->unknown174,
			(real const *)(state + 4), arg_3097c5_2, pitch_rate);
	}
	else
	{
		function_11f0d0(&unit->unknown168, &unit->unknown15c, &unit->unknown174, g_510c54->rate, arg_3097c5_2, pitch_rate);
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
		vector3f old_flat;
		vector3f new_flat;

		yaw_share = unit_turn_share(function_11ce20(&old_aim, &unit->unknown168), step);
		old_flat = old_aim;
		new_flat = unit->unknown168;
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
	else if ((state[0] >> 6) & 1)
	{
		vector3f forward;
		vector3f up;
		vector3f left;
		transform4x3f frame;

		function_b9fc0(unit_index, &forward, &up);
		forward = unit->unknown168;
		left.i = up.j * forward.k - up.k * forward.j;
		left.j = up.k * forward.i - up.i * forward.k;
		left.k = up.i * forward.j - up.j * forward.i;
		up.i = forward.j * left.k - forward.k * left.j;
		up.j = forward.k * left.i - forward.i * left.k;
		up.k = forward.i * left.j - forward.j * left.i;
		unit_turn_frame(&frame, &forward, &up);
		function_c7840(&unit->unknown180, (vector3f *)&unit->unknown18c, &frame, g_510c54->rate, &unit->unknown198,
			(real const *)(state + 0x14), look_yaw_rate, look_pitch_rate);
	}
	else
	{
		function_11f0d0((vector3f *)&unit->unknown18c, &unit->unknown180, &unit->unknown198, g_510c54->rate,
			look_yaw_rate, look_pitch_rate);
	}
	if (0.0001f > (real)fabs(old_aim.i - unit->unknown168.i) && 0.0001f > (real)fabs(old_aim.j - unit->unknown168.j) &&
		0.0001f > (real)fabs(old_aim.k - unit->unknown168.k) &&
		0.0001f > (real)fabs(old_look.i - ((vector3f *)&unit->unknown18c)->i) &&
		0.0001f > (real)fabs(old_look.j - ((vector3f *)&unit->unknown18c)->j) &&
		0.0001f > (real)fabs(old_look.k - ((vector3f *)&unit->unknown18c)->k))
	{
		return false;
	}
	function_bba20(unit_index);
	return true;
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

/* turns a direction toward a desired one within yaw and pitch limits (in
   a frame, when given), keeping an angular velocity: plans both angles'
   motions under a speed limit and an acceleration (0xc7300), evens them
   (0xc75d0), steps them (0xc7750), snapping when both are done */
// @retail 0xc7840
void function_c7840(vector3f const *desired, vector3f *current, transform4x3f const *frame, real rate, vector3f *velocity,
	real const *limits, real max_speed, real acceleration)
{
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
	real yaw_speed = 0.0f;
	real pitch_speed = 0.0f;
	real speed = (real)sqrt(velocity->i * velocity->i + velocity->k * velocity->k + velocity->j * velocity->j);
	if (!(0.0001f > (real)fabs(speed)) && speed != 0.0f)
	{
		real inverse = 1.0f / speed;
		vector3f axis;
		vector3f predicted;
		real angle = speed * rate;
		real sine = (real)sin(angle);
		real cosine = (real)cos(angle);

		axis.i = inverse * velocity->i;
		axis.j = velocity->j * inverse;
		axis.k = velocity->k * inverse;
		real along = (axis.i * local_263186.i + axis.k * local_263186.k + axis.j * local_263186.j) * (1.0f - cosine);
		predicted.i = local_263186.i * cosine + along * axis.i -
			(local_263186.j * axis.k - local_263186.k * axis.j) * sine;
		predicted.j = axis.j * along + local_263186.j * cosine - (local_263186.k * axis.i - axis.k * local_263186.i) * sine;
		predicted.k = axis.k * along + local_263186.k * cosine - (axis.j * local_263186.i - axis.i * local_263186.j) * sine;
		real inverse_rate = 1.0f / rate;
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
	real turn = (real)acos(dot) / rate;
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
// @retail 0xca0b0
void __stdcall function_ca0b0(long unit_index, s_damage_report const *report)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = g_4e3b44[report->definition_index & 0xffff].bytes;
	bool hard = report->flags & 1;
	real total = report->unknown48 + report->unknown44;

	if ((char)report->flags < 0)
	{
		unit->flags_134 |= 0x8000000;
		if (g_4e6948->state == 1 && g_4f55dc[1])
		{
			byte *globals = (byte *)g_4e034c;
			byte *grenade = *(void **)(globals + 0x100) ? *(byte **)(globals + 0x104) + 0x2c : 0;
			byte *grenade_definition = g_4e3b44[*(long *)(grenade + 0x20) & 0xffff].bytes;
			long damage_definition_index = *(long *)(grenade_definition + 0x110);
			s_damage_owner owner;

			owner.player_index = unit->unknownc4;
			owner.object_index = unit->unknownc8;
			owner.team = unit->unknownc2;
			if (damage_definition_index != NONE)
			{
				point3f center;
				s_type_1e6529 damage;

				function_b9dd0(unit_index, &center);
				function_d6660(&damage, damage_definition_index);
				damage.material_index = NONE;
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
	s_unit *object = (s_unit *)function_badc0(unit_index, 3);
	if (object && object->unknown13c != NONE)
	{
		object = (s_unit *)function_badc0(unit_index, 3);
		long player_index = object ? object->unknown13c : NONE;
		short controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

		if (controller != NONE)
		{
			function_153d10(controller, report->definition_index, (void *)&report->owner, (void *)&report->direction,
				*(long *)&report->scale, report->unknown38, 0.0f, (report->flags >> 3) & 1);
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
		else if (!((unit->flags_10a >> 2) & 1))
		{
			function_1c9c80(unit_index, attacker_index, type, total, (long)&report->direction, false);
		}
	}
	if (g_4e6948->mode != 4 && unit->actor_index != NONE && report->flags & 4 &&
		*(real *)(definition + 0x58) > report->distance)
	{
		real shake = (*(real *)(definition + 0x58) - report->distance) / *(real *)(definition + 0x58) *
			(*(real *)(definition + 0x60) - *(real *)(definition + 0x5c)) + *(real *)(definition + 0x5c);

		function_1c9e10(unit_index, &report->direction, shake);
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
void function_a9260(long damage_index, struct s_unit_melee_hit const *hit);
extern short g_47d8e0;

/* the unit's control input for a tick: its wandering direction (flag 21)
   or its facing (through its parent's node) as its desired aim and look;
   scripted controls (+0x1ec, +0x1f0); for a vehicle, its driver's
   controls and its gunner's aim and triggers when their animations allow;
   then its boost charge (+0x334, +0x338); whether anything changed */
// @retail 0xc49b0
bool __stdcall function_c49b0(long unit_index)
{
	s_unit *unit = UNIT_GET(unit_index);
	byte *definition = UNIT_DEFINITION_GET(unit);
	bool changed = false;

	function_d0d20(unit_index);
	if ((unit->flags_134 >> 21) & 1)
	{
		function_d0080(unit_index, &unit->unknown150);
		unit->unknown15c = unit->unknown150;
		unit->unknown180 = unit->unknown150;
		unit->unknown1b0 = *g_4687a8;
		changed = true;
		unit->unknown148 = 0;
	}
	else if ((unit->flags_134 >> 1) & 1)
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

			direction.i = node->up.i * object->forward.k + node->left.i * object->forward.j + node->forward.i * object->forward.i;
			direction.j = node->up.j * object->forward.k + node->left.j * object->forward.j + node->forward.j * object->forward.i;
			direction.k = node->up.k * object->forward.k + node->left.k * object->forward.j + node->forward.k * object->forward.i;
		}
		unit->unknown180 = direction;
		unit->unknown15c = direction;
		unit->unknown150 = direction;
		unit->unknown1b0 = *g_4687a4;
		unit->unknown148 = 0;
	}
	if (!((*(dword *)(definition + 0xbc) >> 11) & 1))
	{
		bool frozen = (unit->flags_134 >> 22) & 1;

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

			if (driver_index != NONE && !((unit->flags_10a >> 2) & 1))
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
			if (unit->unknown24c != NONE && !((unit->flags_10a >> 2) & 1) && function_1143d0(unit_index))
			{
				long gunner_index = unit->unknown24c;
				s_unit *gunner = UNIT_GET(gunner_index);
				long state = function_10f5f0(gunner_index);
				bool entering = false;
				bool leaving = false;
				bool copy_aim;
				bool copy_controls;

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
					copy_controls = copy_aim;
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
	if ((*(dword *)(definition + 0xbc) >> 28) & 1 && (boosting || unit->unknown334 > 0.0f))
	{
		if (unit->unknown338 != 0.0f)
		{
			unit->unknown334 -= g_510c54->rate;
			if (0.0f >= unit->unknown334)
			{
				unit->unknown334 = 0.0f;
				unit->unknown338 = 0.0f;
			}
			return true;
		}
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
// @retail 0xcf3d0
void __stdcall function_cf3d0(long unit_index, long name, long flags, real scale)
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
		real hit_t = 0.0f;
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
					if (hit_index == NONE || (object->type == 0 && (hit_type != 0 || hit_t > collision.t)))
					{
						hit_index = object_index;
						hit_type = object->type;
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
		damage_index = melee ? *(long *)(melee + 4) : NONE;
		self_damage_index = melee ? *(long *)(melee + 0xc) : NONE;
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
		hit.unknown14 = 0.0f > scale ? 0.0f : (scale > 1.0f ? 1.0f : scale);
		hit.unknown18 = hit_seat;
		if (unit->parent_index != NONE && unit->parent_seat_index != NONE &&
			!((*(dword *)&UNIT_SEATS(UNIT_DEFINITION_GET(UNIT_GET(unit->parent_index)))[unit->parent_seat_index].flags >>
				11) & 1))
		{
			goto impact;
		}
		if (!(byte)flags)
		{
			if (function_a76b0(unit_index, 0))
			{
				function_a9260(damage_index, &hit);
			}
			else if (g_4e6948->mode != 4)
			{
				function_cfc90(unit_index, damage_index, &hit);
			}
		}
	}
impact:
	if (material_index != NONE && material_index >= 0 && material_index < *(long *)((byte *)g_4e034c + 0x150) &&
		*(byte **)((byte *)g_4e034c + 0x154) + material_index * 0xb4)
	{
		function_ceee0(unit_index, damage_index, material_index, &point, &normal);
		if (self_damage_index != NONE)
		{
			s_type_1e6529 damage;

			function_d6660(&damage, self_damage_index);
			damage.material_index = NONE;
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
					short controller = *(short *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

					if (controller != NONE)
					{
						s_damage_owner owner;

						owner.player_index = NONE;
						owner.object_index = NONE;
						owner.team = NONE;
						function_153d10(controller, self_damage_index, &owner, &damage.direction, 0, 1.0f, 1.0f, 0);
					}
				}
			}
		}
	}
	if (unit->unknown13c != NONE)
	{
		*((byte *)g_4e8c24->data + (unit->unknown13c & 0xffff) * 0x21c + 0x40) =
			(byte)unit_round(g_510c54->field_2_3 * 0.5f);
	}
}
