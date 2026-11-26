#ifndef SHADER_CACHE_H
#define SHADER_CACHE_H
#include "unknown_11c920.h"
#include "shader_storage.h"

struct s_shader_binding
{
	long field_00;
	long tag;
	long index;
	long field_0c;
};

struct s_shader_stream
{
	long field_00;
	long field_04;
	long field_08;
};

struct s_shader_cache
{
	s_shader_binding bindings[8];
	long active;
	long field_84;
	bool field_88;
	byte field_89[3];
	byte const *wanted[16];
	byte const *current[16];
	byte field_10c[0x100];
	bool descriptors_changed;
	byte field_20d[3];
	s_shader_stream streams[16];
	long stream_count;
	bool streams_changed;
	byte field_2d5[3];

	s_shader_cache();
};

extern byte const g_43f408[63][21];
void function_1c6b0(void *memory);
void __stdcall function_1c710(void *memory);

// Makes one of the immediate vertex formats the first stream's wanted
// descriptor, marks the descriptors changed when it differs from the current
// one, and applies the cache.
PRIVATE __forceinline void select_immediate_descriptor(long format)
{
	byte const *descriptor = g_43f408[format];
	function_1c6b0(g_51f0f0);
	s_shader_cache *state = (s_shader_cache *)g_51f0f0;
	state->wanted[0] = descriptor;
	if (state->current[0] != descriptor)
		state->descriptors_changed = true;
	function_1c710(g_51f0f0);
}
#endif
