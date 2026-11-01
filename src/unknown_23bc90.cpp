// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
/* UNKNOWN_23BC90.CPP: the cameras that follow a unit: the first person
   camera, the camera of a unit's seat and the following camera
   (0x23bc90..0x23cea0) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector_angles.h"
#include "globals.h"
#include "data_array.h"
#include "object_markers.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* a unit as its camera sees it */
struct s_camera_unit_view
{
	long tag_index;
	byte unknown04[0x10];
	long parent_index;
	byte unknown18[0x168 - 0x18];
	vector3f forward;
	byte unknown174[0x1fc - 0x174];
	short seat_index;
};

struct s_camera_object_view
{
	long tag_index;
};

/* a vehicle seat (0xb0 bytes) and the camera it gives its unit */
struct s_camera_seat_view
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword flag7 : 1;
	byte unknown04[0x60 - 4];
	byte camera[0xb0 - 0x60];
};

struct s_camera_vehicle_tag_view
{
	byte unknown000[0x1c8];
	long seat_count;
	s_camera_seat_view *seats;
};

struct s_camera_unit_tag_view
{
	byte unknown00[0xd4];
	byte camera[4];
};

struct s_camera_object_header_view
{
	byte unknown00[8];
	s_camera_unit_view *object;
};

/* the camera of the unit: its seat's when the seat has one, else its own */
// @retail 0x23c270
void *function_23c270(long unit_index)
{
	s_camera_unit_view *unit = ((s_camera_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;
	s_tag_instance *tags = g_4e3b44;
	void *camera = NULL;

	if (unit->parent_index != NONE)
	{
		s_camera_object_view *vehicle = (s_camera_object_view *)function_badc0(unit->parent_index, 2);

		if (vehicle)
		{
			s_camera_vehicle_tag_view *definition = (s_camera_vehicle_tag_view *)tags[vehicle->tag_index & 0xffff].flags;
			s_camera_seat_view *seat = &definition->seats[unit->seat_index];

			if (TEST_FIELD_BIT(seat->flag2) || TEST_FIELD_BIT(seat->flag0) || TEST_FIELD_BIT(seat->flag4))
			{
				camera = seat->camera;
			}
		}
	}
	if (!camera)
	{
		camera = ((s_camera_unit_tag_view *)tags[unit->tag_index & 0xffff].flags)->camera;
	}
	return camera;
}

void function_cafc0(long unit_index, point3f *position);

vector3f *function_11d090(vector3f const *vector, vector3f *out);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
point3f *function_b9dd0(long object_index, point3f *result);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
struct s_observer_command;
void function_172520(s_observer_command *command);
extern real g_54e854;

struct s_camera_command_23bd70
{
	dword flags;
	point3f position;
	vector3f offset;
	byte unknown1c[8];
	real distance;
	real field_of_view;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	byte unknown50[0x88 - 0x50];
	real timer;
	byte unknown8c;
	bool constrained_changed;
	byte unknown8e[0x91 - 0x8e];
	bool input_changed;
	byte unknown92[0x98 - 0x92];
	real constrained_timer;
	byte unknown9c[0xa8 - 0x9c];
	real input_timer;
};

PRIVATE __forceinline void camera_rotate_23bd70(transform4x3f const *matrix, vector3f const *direction, vector3f *result)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	result->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	result->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	result->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
}

PRIVATE __forceinline void camera_unrotate_23bd70(transform4x3f const *matrix, vector3f *direction)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	direction->i = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
	direction->j = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
	direction->k = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
}

/* the unit's camera command: its eye, facing and up, from its seat's camera
   marker or turned with the vehicle it rides */
// @retail 0x23bd70
void function_23bd70(long unit_index, vector3f const *forward, s_camera_command_23bd70 *command)
{
	command->timer = 0.f;
	command->flags = 0;
	command->offset = *g_4687a4;
	command->distance = 0.f;
	command->forward = *forward;
	command->field_of_view = g_54e854;
	function_11d090(&command->forward, &command->up);
	if (unit_index != NONE)
	{
		s_camera_unit_view *unit = ((s_camera_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;
		function_cafc0(unit_index, &command->position);
		function_ba1d0(unit_index, &command->velocity, 0);
		long parent_index = unit->parent_index;
		if (parent_index != NONE)
		{
			s_camera_object_view *parent = (s_camera_object_view *)function_badc0(parent_index, 2);
			if (parent)
			{
				s_camera_vehicle_tag_view *definition = (s_camera_vehicle_tag_view *)g_4e3b44[parent->tag_index & 0xffff].bytes;
				if (TEST_FIELD_BIT(definition->seats[unit->seat_index].flag7))
				{
					s_object_marker marker;
					if (function_b8d30(parent_index, 0xf0000db, &marker, 1, false))
					{
						command->position = marker.matrix.position;
						command->forward = marker.matrix.forward;
						command->up = marker.matrix.up;
					}
				}
				else
				{
					point3f position;
					transform4x3f matrix;
					function_b9dd0(parent_index, &position);
					function_1420f0(&matrix, &position, (vector3f *)((byte *)parent + 0x70), (vector3f *)((byte *)parent + 0x7c));
					camera_unrotate_23bd70(&matrix, &command->forward);
					function_11d090(&command->forward, &command->up);
					camera_rotate_23bd70(&matrix, &command->forward, &command->forward);
					camera_rotate_23bd70(&matrix, &command->up, &command->up);
				}
			}
		}
		command->flags = 1;
	}
	function_172520((s_observer_command *)command);
}

/* the first person camera of the unit: its eye and facing, or its seat's
   camera marker when the seat says so */
// @retail 0x23bc90
void function_23bc90(long unit_index, point3f *position, vector3f *forward)
{
	s_camera_unit_view *unit = ((s_camera_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;

	function_cafc0(unit_index, position);
	*forward = unit->forward;

	long parent_index = unit->parent_index;
	if (parent_index != NONE)
	{
		s_camera_object_view *vehicle = (s_camera_object_view *)function_badc0(parent_index, 2);

		if (vehicle)
		{
			s_camera_vehicle_tag_view *definition = (s_camera_vehicle_tag_view *)g_4e3b44[vehicle->tag_index & 0xffff].flags;

			if (TEST_FIELD_BIT(definition->seats[unit->seat_index].flag7))
			{
				s_object_marker marker;

				if (function_b8d30(parent_index, 0xf0000db, &marker, 1, false))
				{
					*position = marker.matrix.position;
					*forward = marker.matrix.forward;
				}
			}
		}
	}
}

struct s_camera_track_reference_23c300
{
	dword group;
	long index;
};

struct s_camera_tracks_23c300
{
	byte unknown00[0x14];
	long count;
	s_camera_track_reference_23c300 *tracks;
};

struct s_camera_key_23c300
{
	point3f position;
	quaternionf rotation;
};

struct s_camera_track_23c300
{
	dword flags;
	long count;
	s_camera_key_23c300 *keys;
};

void function_212100(point3f *out, point3f const *p0, point3f const *p1, point3f const *p2,
	point3f const *p3, real start, real step, real time);

/* the forward vector (1, 0, 0) rotated by a key's unit quaternion */
PRIVATE __forceinline void camera_key_direction_23c300(quaternionf q, vector3f *out)
{
	vector3f v;
	v.i = 1.f;
	v.j = v.k = 0.f;
	real w2 = q.w * 2.f;
	real m = q.w * q.w * 2.f - 1.f;
	real d = (q.j * v.j + q.k * v.k + q.i * v.i) * 2.f;
	real cx = q.j * v.k - v.j * q.k;
	real cy = v.i * q.k - q.i * v.k;
	real cz = q.i * v.j - q.j * v.i;

	out->i = cx * w2 + v.i * m + q.i * d;
	out->j = q.j * d + cy * w2 + m * v.j;
	out->k = v.k * m + cz * w2 + d * q.k;
}

PRIVATE __forceinline void camera_direction_curve_23c300(vector3f *out, vector3f const *p0, vector3f const *p1,
	vector3f const *p2, vector3f const *p3, real start, real step, real time)
{
	for (long i = 0; i < 3; i++)
	{
		real x3 = p3->n[i];
		real x2 = p2->n[i];
		real x1 = p1->n[i];
		real x0 = p0->n[i];
		x3 -= x2;
		x2 -= x1;
		x1 -= x0;
		x3 -= x2;
		x2 -= x1;
		x3 -= x2;
		out->n[i] = ((x3 * (time - (step * 2.f + start)) / (step * 3.f) + x2) * (time - (start + step)) / (step * 2.f) + x1) *
			(1.f / step) * (time - start) + x0;
	}
}

/* the tag index of a camera's track: the clamped track's, NONE when there is none */
PRIVATE __forceinline long camera_track_index_23c300(s_camera_tracks_23c300 const *camera, long track)
{
	long tag_index;
	if (camera->count)
	{
		track = track > camera->count - 1 ? camera->count - 1 : track;
		s_camera_track_reference_23c300 *reference = &camera->tracks[track];
		tag_index = reference ? reference->index : NONE;
	}
	else
		tag_index = NONE;
	return tag_index;
}

// @retail 0x23c300
void function_23c300(s_camera_tracks_23c300 const *camera, real time, point3f *position, vector3f *direction, bool alternate)
{
	long tag_index = camera_track_index_23c300(camera, alternate ? 1 : 0);
	if (tag_index == NONE)
		tag_index = (*(s_camera_track_reference_23c300 **)((byte *)g_4e034c + 0xec))->index;
	s_camera_track_23c300 *definition = (s_camera_track_23c300 *)g_4e3b44[tag_index & 0xffff].bytes;
	if (definition && definition->count >= 4)
	{
		short frame = (short)(time * (definition->count - 1));
		real step = 1.f / (real)(definition->count - 1);
		short first = frame;
		while (first > 0 && (first + 4 > definition->count || first > frame - 1))
			first--;
		if (direction)
		{
			vector3f a, b, c, d;
			camera_key_direction_23c300(definition->keys[first].rotation, &a);
			camera_key_direction_23c300(definition->keys[first + 1].rotation, &b);
			camera_key_direction_23c300(definition->keys[first + 2].rotation, &c);
			camera_key_direction_23c300(definition->keys[first + 3].rotation, &d);
			camera_direction_curve_23c300(direction, &a, &b, &c, &d, first * step, step, time);
		}
		if (position)
			function_212100(position, &definition->keys[first].position, &definition->keys[first + 1].position,
				&definition->keys[first + 2].position, &definition->keys[first + 3].position, first * step, step, time);
	}
	else
	{
		*direction = *g_4687a8;
		*(vector3f *)position = *g_4687a4;
	}
}

struct s_observer_command;
// @retail 0x23c0e0
void function_23c0e0(long object_index, s_observer_command *command)
{
    s_camera_unit_view *unit = ((s_camera_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
    function_23bd70(object_index, &unit->forward, (s_camera_command_23bd70 *)command);
}

// @retail 0x23c980
real function_23c980(s_camera_tracks_23c300 const *camera, real angle, bool alternate)
{
    /* Keep the selector among the stack parameters. */
    bool const *alternate_reference = &alternate;
    struct
    {
        real time;
        real upper;
        real lower;
    } interval;
    interval.lower = 0.f;
    interval.upper = 1.f;
    do
    {
        vector3f direction;
        interval.time = (interval.upper + interval.lower) * 0.5f;
        function_23c300(camera, interval.time, 0, &direction, alternate);
        if (angle > atan2(direction.k, sqrt(direction.j * direction.j + direction.i * direction.i)))
            interval.lower = interval.time;
        else
            interval.upper = interval.time;
    } while (interval.upper - interval.lower > 0.001f);
    return interval.time;
}

bool function_cb810(long unit_index, vector3f *vector);
real function_187130(long player_index);
real g_47136c = 1.f;
real g_55e484;
real g_55e488;
struct s_camera_player_23c110
{
    byte unknown00[0x2c];
    long unit_index;
    byte unknown30[0x21c - 0x30];
};

bool function_14ddc0(long local_player_index);
long function_14de70(long local_player_index);

// @retail 0x23c110
void __stdcall function_23c110(void *state, void *input, s_observer_command *command)
{
    long unit_index;
    s_record_pool *players = g_4e8c24;
    if (function_14ddc0(*(long *)input))
    {
        long player = function_14de70(*(long *)input);
        unit_index = ((s_camera_player_23c110 *)players->data)[player & 0xffff].unit_index;
    }
    else
        unit_index = NONE;
    long player = function_14de70(*(long *)input);
    s_view_angles *angles = &g_4ed284->entries[*(long *)input].angles;
    vector3f forward;
    real horizontal = (real)cos(angles->pitch);
    s_camera_player_23c110 const *current_player = &((s_camera_player_23c110 *)players->data)[player & 0xffff];
    forward.i = horizontal * (real)cos(angles->yaw);
    forward.j = horizontal * (real)sin(angles->yaw);
    forward.k = (real)sin(angles->pitch);
    if (current_player->unit_index != NONE)
        function_cb810(current_player->unit_index, &forward);
    function_23bd70(unit_index, &forward, (s_camera_command_23bd70 *)command);
    byte *settings = *(byte **)((byte *)g_4e034c + 0xf4);
    real_bounds limits = *(real_bounds *)(settings + 0x18);
    *(real *)((byte *)command + 0x1c) = limits.lo;
    *(real *)((byte *)command + 0x20) = limits.hi;
    s_camera_command_23bd70 *camera = (s_camera_command_23bd70 *)command;
    camera->field_of_view = function_187130(*(long *)input) * g_47136c;
    camera->field_of_view = camera->field_of_view < g_55e484 ? g_55e484 :
        camera->field_of_view > g_55e488 ? g_55e488 : camera->field_of_view;
    if (*(real *)state != camera->field_of_view)
    {
        *(real *)((byte *)command + 0xa4) = 0.18f;
        *((byte *)command + 0x90) = 1;
        *(dword *)state = *(dword *)&camera->field_of_view;
    }
}

// @retail 0x23ca20
void function_23ca20(long unit_index, point3f *position, vector3f *forward)
{
    s_camera_unit_view *unit = ((s_camera_object_header_view *)g_4e0300->data)[unit_index & 0xffff].object;
    s_camera_tracks_23c300 *camera = (s_camera_tracks_23c300 *)function_23c270(unit_index);
    function_cafc0(unit_index, position);
    *forward = unit->forward;
    bool alternate = false;
    byte *object = (byte *)function_badc0(unit_index, 3);
    if (object)
        alternate = (bool)((*(dword *)(object + 0x134) >> 17) & 1);
    real vertical = forward->k;
    real clamped = vertical < -1.f ? -1.f : vertical > 1.f ? 1.f : vertical;
    real time = function_23c980(camera, (real)asin(clamped), alternate);
    vector3f direction;
    point3f offset;
    function_23c300(camera, time, &offset, &direction, alternate);
    struct { real i, j; } planar;
    planar.i = forward->i;
    planar.j = forward->j;
    real length = (real)sqrt(planar.i * planar.i + planar.j * planar.j);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.f / length;
        planar.i *= inverse;
        planar.j *= inverse;
    }
    position->x += offset.x * planar.i - offset.y * planar.j;
    position->y += offset.x * planar.j + offset.y * planar.i;
    position->z += offset.z;
}

struct s_player_control_camera
{
    long unit_index;
    short seat_index;
    byte unknown06[2];
    void *camera;
    point3f position;
};
void player_control_get_camera(long player_index, s_player_control_camera *camera);

struct s_follow_state_23cbb0
{
    bool initialized;
    byte unknown01;
    bool constrained;
    byte unknown03;
    bool alternate;
    byte unknown05[3];
    long unit_index;
    short seat_index;
    byte unknown0e[2];
    real yaw;
    real pitch;
    real scale;
};
struct s_follow_input_23cbb0
{
    long user;
    bool active;
    byte unknown05[7];
    real yaw;
    real pitch;
};

// @retail 0x23cbb0
void __stdcall function_23cbb0(void *state, void *input, s_observer_command *output)
{
    s_player_control_camera camera;
    s_follow_state_23cbb0 *values = (s_follow_state_23cbb0 *)state;
    s_follow_input_23cbb0 *controls = (s_follow_input_23cbb0 *)input;
    s_camera_command_23bd70 *command = (s_camera_command_23bd70 *)output;
    player_control_get_camera(controls->user, &camera);
    command->position = camera.position;
    command->field_of_view = g_54e854;
    command->timer = 0.f;
    command->flags = 0;
    if (values->initialized && (camera.unit_index != values->unit_index || camera.seat_index != values->seat_index))
        command->timer = 0.5f;
    values->unit_index = camera.unit_index;
    values->seat_index = camera.seat_index;
    if (camera.camera)
    {
        byte *unit = (byte *)((s_camera_object_header_view *)g_4e0300->data)[camera.unit_index & 0xffff].object;
        bool constrained = (*(dword *)(unit + 0x148) & 1) || (*(dword *)(unit + 0x148) & 2);
        if (constrained != values->constrained)
        {
            command->constrained_changed = true;
            command->constrained_timer = 0.5f > command->constrained_timer ? 0.5f : command->constrained_timer;
            values->constrained = constrained;
        }
        bool alternate = false;
        byte *object = (byte *)function_badc0(camera.unit_index, 3);
        if (object)
            alternate = (bool)((*(dword *)(object + 0x134) >> 17) & 1);
        if (alternate != values->alternate)
        {
            command->timer = 0.5f > command->timer ? 0.5f : command->timer;
            values->alternate = alternate;
        }
        if (controls->active)
        {
            values->yaw += controls->yaw;
            values->pitch += controls->pitch;
            command->input_changed = true;
            command->input_timer = 0.4f > command->input_timer ? 0.4f : command->input_timer;
        }
        else if (values->yaw != 0.f || values->pitch != 0.f)
        {
            values->pitch = 0.f;
            values->yaw = 0.f;
        }
        s_unknown_185ab0_entry *view = &g_4ed284->entries[controls->user];
        s_view_angles angles = view->angles;
        real total_pitch = values->pitch + angles.pitch;
        real pitch = total_pitch < -1.57079637f ? -1.57079637f : total_pitch > 1.57079637f ? 1.57079637f : total_pitch;
        real time = function_23c980((s_camera_tracks_23c300 *)camera.camera, pitch, alternate);
        vector3f direction;
        point3f offset;
        function_23c300((s_camera_tracks_23c300 *)camera.camera, time, &offset, &direction, alternate);
        real final_yaw = angles.yaw + values->yaw;
        vector3f *forward = &command->forward;
        direction_from_angles(forward, final_yaw, pitch);
        real cosine = (real)cos(pitch);
        real sine = (real)sin(pitch);
        real distance = (real)sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
        command->distance = distance;
        command->offset.i = (cosine * distance + offset.x) * values->scale;
        command->offset.j = -values->scale * offset.y;
        command->offset.k = (sine * distance + offset.z) * values->scale;
        function_ba1d0(camera.unit_index, &command->velocity, 0);
        command->flags |= 1;
        function_11d090(forward, &command->up);
    }
    function_172520(output);
    values->initialized = true;
}
