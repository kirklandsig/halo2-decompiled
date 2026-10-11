// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_23BC90.CPP: the cameras that follow a unit: the first person
   camera, the camera of a unit's seat and the following camera
   (0x23bc90..0x23cea0) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
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
};

PRIVATE __forceinline void camera_rotate_23bd70(transform4x3f const *matrix, vector3f *direction)
{
	real x = direction->i;
	real y = direction->j;
	real z = direction->k;
	direction->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	direction->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	direction->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
}

// @retail 0x23bd70
void function_23bd70(long unit_index, vector3f const *forward, s_camera_command_23bd70 *command)
{
    /* Retail passes the output pointer on the stack. */
    s_camera_command_23bd70 **command_reference = &command;
	vector3f &command_forward = command->forward;
	vector3f &command_up = command->up;
	command->timer = 0.f;
	command->flags = 0;
	command->offset = *g_4687a4;
	command->distance = 0.f;
	command_forward = *forward;
	command->field_of_view = g_54e854;
	function_11d090(&command_forward, &command_up);
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
						command_forward = marker.matrix.forward;
						command_up = marker.matrix.up;
					}
				}
				else
				{
					point3f position;
					transform4x3f matrix;
					function_b9dd0(parent_index, &position);
					function_1420f0(&matrix, &position, (vector3f *)((byte *)parent + 0x70), (vector3f *)((byte *)parent + 0x7c));
					real x = command_forward.i;
					real y = command_forward.j;
					real z = command_forward.k;
					command_forward.i = matrix.forward.k * z + matrix.forward.j * y + matrix.forward.i * x;
					command_forward.j = matrix.left.k * z + matrix.left.j * y + matrix.left.i * x;
					command_forward.k = matrix.up.k * z + matrix.up.j * y + matrix.up.i * x;
					function_11d090(&command_forward, &command_up);
					transform4x3f const *const volatile matrix_cursor = &matrix;
					transform4x3f const *reached_matrix = matrix_cursor;
					camera_rotate_23bd70(reached_matrix, &command_forward);
					camera_rotate_23bd70(reached_matrix, &command_up);
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

void function_212100(point3f const *p3, point3f const *p2, point3f const *p1, point3f const *p0,
	point3f *out, real start, real step, real time);

PRIVATE __forceinline void camera_key_direction_23c300(quaternionf const *q, vector3f *out)
{
	vector3f v;
	v.i = 1.f;
	v.j = v.k = 0.f;
	real dot = (q->i * v.i + q->j * v.j + q->k * v.k) * 2.f;
	real w2 = q->w * 2.f;
	real s = q->w * q->w * 2.f - 1.f;
	vector3f cross;
	cross.i = q->j * v.k - q->k * v.j;
	cross.j = q->k * v.i - q->i * v.k;
	cross.k = q->i * v.j - q->j * v.i;
	out->i = v.i * s + q->i * dot + cross.i * w2;
	out->j = v.j * s + q->j * dot + cross.j * w2;
	out->k = v.k * s + q->k * dot + cross.k * w2;
}

PRIVATE __forceinline void camera_direction_curve_23c300(vector3f const *p3, vector3f const *p2,
	vector3f const *p1, vector3f const *p0, vector3f *out, real start, real step, real time)
{
	real two_step = step * 2.f;
	real three_step = step * 3.f;
	real inverse_step = 1.f / step;
	real t0 = time - start;
	real t1 = time - (start + step);
	real t2 = time - (two_step + start);
	for (long i = 0; i < 3; i++)
	{
		real s0 = p3->n[i] - p2->n[i];
		real s1 = p2->n[i] - p1->n[i];
		real s2 = p1->n[i] - p0->n[i];
		real d2 = s1 - s2;
		real d3 = (s0 - s1) - d2;
		out->n[i] = ((d3 * t2 / three_step + d2) * t1 / two_step + s2) * inverse_step * t0 + p0->n[i];
	}
}

// @retail 0x23c300
void function_23c300(bool alternate, s_camera_tracks_23c300 const *camera,
	vector3f *direction, real time, point3f *position)
{
	long track = alternate ? 1 : 0;
	long tag_index = NONE;
	if (camera->count)
	{
		track = track > camera->count - 1 ? camera->count - 1 : track;
		s_camera_track_reference_23c300 *reference = &camera->tracks[track];
		if (reference)
			tag_index = reference->index;
	}
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
			camera_key_direction_23c300(&definition->keys[first].rotation, &a);
			camera_key_direction_23c300(&definition->keys[first + 1].rotation, &b);
			camera_key_direction_23c300(&definition->keys[first + 2].rotation, &c);
			camera_key_direction_23c300(&definition->keys[first + 3].rotation, &d);
			camera_direction_curve_23c300(&d, &c, &b, &a, direction, first * step, step, time);
		}
		if (position)
			function_212100(&definition->keys[first + 3].position, &definition->keys[first + 2].position,
				&definition->keys[first + 1].position, &definition->keys[first].position,
				position, first * step, step, time);
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
    real lower = 0.f;
    real upper = 1.f;
    real time;
    do
    {
        vector3f direction;
        time = (upper + lower) * 0.5f;
        function_23c300(alternate, camera, &direction, time, 0);
        if (angle > atan2(direction.k, sqrt(direction.j * direction.j + direction.i * direction.i)))
            lower = time;
        else
            upper = time;
    } while (upper - lower > 0.001f);
    return time;
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

// @retail 0x23c110
void __stdcall function_23c110(void *state, void *input, s_observer_command *command)
{
    long user = *(long *)input;
    long unit_index = NONE;
    s_record_pool *players = g_4e8c24;
    s_index_table *indices = g_4e8c20;
    if (user != NONE && indices->entries[user] != NONE)
    {
        long player = user != NONE ? indices->entries[user] : NONE;
        unit_index = ((s_camera_player_23c110 *)players->data)[player & 0xffff].unit_index;
    }
    long player = user != NONE ? indices->entries[user] : NONE;
    s_unknown_185ab0_entry *view = &g_4ed284->entries[user];
    vector3f forward;
    real horizontal = (real)cos(view->pitch);
    forward.i = horizontal * (real)cos(view->yaw);
    forward.j = horizontal * (real)sin(view->yaw);
    forward.k = (real)sin(view->pitch);
    long current_unit = ((s_camera_player_23c110 *)players->data)[player & 0xffff].unit_index;
    if (current_unit != NONE)
        function_cb810(current_unit, &forward);
    function_23bd70(unit_index, &forward, (s_camera_command_23bd70 *)command);
    byte *settings = *(byte **)((byte *)g_4e034c + 0xf4);
    volatile real limits[2];
    limits[0] = *(real *)(settings + 0x18);
    limits[1] = *(real *)(settings + 0x1c);
    *(real *)((byte *)command + 0x1c) = limits[0];
    *(real *)((byte *)command + 0x20) = limits[1];
    s_camera_command_23bd70 *camera = (s_camera_command_23bd70 *)command;
    camera->field_of_view = function_187130(*(long *)input) * g_47136c;
    camera->field_of_view = camera->field_of_view < g_55e484 ? g_55e484 :
        camera->field_of_view > g_55e488 ? g_55e488 : camera->field_of_view;
    if (*(real *)state != camera->field_of_view)
    {
        *(real *)((byte *)command + 0xa4) = 0.18f;
        *((byte *)command + 0x90) = 1;
        *(real *)state = camera->field_of_view;
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
    vertical = vertical < -1.f ? -1.f : vertical > 1.f ? 1.f : vertical;
    real time = function_23c980(camera, (real)asin(vertical), alternate);
    vector3f direction;
    point3f offset;
    function_23c300(alternate, camera, &direction, time, &offset);
    direction.i = forward->i;
    direction.j = forward->j;
    real length = (real)sqrt(direction.i * direction.i + direction.j * direction.j);
    if (!(fabs(length) < 0.0001f))
    {
        real inverse = 1.f / length;
        direction.i *= inverse;
        direction.j *= inverse;
    }
    real y = (offset.x * direction.j + offset.y * direction.i) + position->y;
    real x = (offset.x * direction.i - offset.y * direction.j) + position->x;
    real z = position->z + offset.z;
    position->y = y;
    position->x = x;
    position->z = z;
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
        bool constrained = (*(dword *)(unit + 0x148) & 3) != 0;
        if (constrained != values->constrained)
        {
            *((byte *)output + 0x8d) = 1;
            real *timer = (real *)((byte *)output + 0x98);
            *timer = 0.5f > *timer ? 0.5f : *timer;
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
            *((byte *)output + 0x91) = 1;
            real *timer = (real *)((byte *)output + 0xa8);
            *timer = 0.4f > *timer ? 0.4f : *timer;
        }
        else if (values->yaw != 0.f || values->pitch != 0.f)
        {
            values->pitch = 0.f;
            values->yaw = 0.f;
        }
        s_unknown_185ab0_entry *view = &g_4ed284->entries[controls->user];
        real yaw = view->yaw;
        real pitch = values->pitch + view->pitch;
        pitch = pitch < -1.57079637f ? -1.57079637f : pitch > 1.57079637f ? 1.57079637f : pitch;
        real time = function_23c980((s_camera_tracks_23c300 *)camera.camera, pitch, alternate);
        vector3f direction;
        point3f offset;
        function_23c300(alternate, (s_camera_tracks_23c300 *)camera.camera, &direction, time, &offset);
        real final_yaw = yaw + values->yaw;
        real cosine = (real)cos(pitch);
        real sine = (real)sin(pitch);
        command->forward.i = (real)cos(final_yaw) * cosine;
        command->forward.j = (real)sin(final_yaw) * cosine;
        command->forward.k = sine;
        real distance = (real)sqrt(offset.z * offset.z + offset.y * offset.y + offset.x * offset.x);
        command->distance = distance;
        command->offset.i = (cosine * distance + offset.x) * values->scale;
        command->offset.j = -values->scale * offset.y;
        command->offset.k = (sine * distance + offset.z) * values->scale;
        function_ba1d0(camera.unit_index, &command->velocity, 0);
        command->flags |= 1;
        function_11d090(&command->forward, &command->up);
    }
    function_172520(output);
    values->initialized = true;
}
