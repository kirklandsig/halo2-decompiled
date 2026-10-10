#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_xd56787.h"

// @flags /O2 /Gr

/* UNKNOWN_2C4D60.CPP: the channel decoders of the uncompressed animation
   codec (the definition at 0x47fb68 holds them: rotation, translation and
   scale, sampled at the current frame without interpolation) */

struct s_animation_data
{
	byte unknown00[0xc];
	long vector_offset;
	long scale_offset;
	long rotation_stride;
	long vector_stride;
	long scale_stride;
	byte rotations[1];
};

// @retail 0x2c4d60
void function_2c4d60()
{
	s_animation_data *data = g_sampling_settings.field_30;
	quaternionf *quaternions = (quaternionf *)(data->rotations + data->rotation_stride * g_5044b4);

	g_5044c0->rotation = quaternions[g_sampling_settings.frame_index];
}

// @retail 0x2c4da0
void function_2c4da0()
{
	s_animation_data *data = g_sampling_settings.field_30;
	vector3f *vectors = (vector3f *)((byte *)data + data->vector_stride * g_5044b8 + g_sampling_settings.frame_index * 12 + data->vector_offset);

	g_5044c0->vector = *vectors;
}

// @retail 0x2c4de0
void function_2c4de0()
{
	s_animation_data *scale_track_header = g_sampling_settings.field_30;

	g_5044c0->scale = *(real *)((byte *)scale_track_header + scale_track_header->scale_stride * g_5044bc + g_sampling_settings.frame_index * 4 + scale_track_header->scale_offset);
}
