#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"

// @flags /O2 /arch:SSE /Gr

/* the vehicle type (vtable at 0x4520f0) and the base-class methods whose
   retail copy is first reached through it; the hierarchy is in
   unknown_0a58d0.h */

/* the object header array of g_4e0300 (12 bytes each, the object pointer at
   +8), read as bytes */
#define OBJECT_FROM_INDEX(index) \
	((byte *)((s_object_header *)g_4e0300->data)[(index) & 0xffff].object)

/* the tag data of g_4e3b44 (data pointer at +8) */
#define TAG_DATA_FROM_INDEX(index) \
	(g_4e3b44[(index) & 0xffff].bytes)

struct s_object_data_view
{
	byte unknown00[8];
	long unknown08;
	byte unknown0c[0xb8];
};

struct s_object_creation
{
	c_vehicle_type *owner;
	s_object_data_view data;
};

static void object_state_clear(s_entity_state *state)
{
	state->field0 = 0;
	state->field4 = 0;
	state->field8_long = 0;
	state->fieldc = 0;
	state->field10 = 0;
}

// @retail 0x9fe30
long c_vehicle_type::v0()
{
	return 12;
}

// @retail 0xa09a0
const char *c_vehicle_type::v1()
{
	return "vehicle";
}

// @retail 0xa0880
void c_object_type_definition::v9(long a, long b, long *result)
{
	*result = 0x85;
}

// @retail 0xa0930
void c_object_type_definition::v21(s_entity *link)
{
	if (link->object_index != NONE)
	{
		byte *object = OBJECT_FROM_INDEX(link->object_index);
		*(long *)(object + 0xd4) = link->field0;
	}
}

// @retail 0xa0700
void c_object_type_definition::v26(long object_index, long unused, s_entity_state *state)
{
	byte *first = OBJECT_FROM_INDEX(object_index);
	byte *object;
	byte *definition;

	object_state_clear(state);

	object = OBJECT_FROM_INDEX(object_index);
	state->field4 = *(long *)object;
	state->field0 = *(short *)(object + 0x1a);
	state->field8 = object[0xaf];
	state->fieldc = *(long *)(object + 0x24);

	definition = TAG_DATA_FROM_INDEX(*(long *)first);
	if (first[0xb1] != 0xff && *(long *)(definition + 0x38) != NONE)
	{
		byte *tag = TAG_DATA_FROM_INDEX(*(long *)(definition + 0x38));
		state->field10 = *(long *)(*(byte **)(tag + 0x54) + (char)first[0xb1] * 0x38);
	}
	else
	{
		state->field10 = 0;
	}
}
// @retail 0xa07f0
long c_object_type_definition::v29(long a, s_entity_info *info, long *c, long d, long e)
{
	s_object_creation creation;
	long index;

	creation.owner = (c_vehicle_type *)this;
	function_a5d90(&creation.data, info, c, e);
	creation.data.unknown08 = info->identifier;
	if (info->field0 != NONE)
		index = function_a73b0(info);
	else
		index = function_b7b40(&creation);
	if (index != NONE)
	{
		byte *object = OBJECT_FROM_INDEX(index);
		object[0xaf] = info->byte8;
		function_b9b90(index, true);
	}
	return index;
}

// @retail 0xa0900
void c_object_type_definition::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
	function_a6660(info);
	function_1955d0(stream, info->vehicle_data, 0x20);
}

// @retail 0xa0a60
bool c_object_type_definition::v13(long a, s_entity_info *info, s_bitstream *stream)
{
	bool ok = function_a6810(stream);

	function_195820(stream, info->vehicle_data, 0x20);
	return stream->bit_position <= (stream->size_in_bytes << 3) && ok;
}

// @retail 0xa09f0
void c_vehicle_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];

	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "vehicle creation: relevance=%5.3f", relevance);
}

// @retail 0x2bcc80
bool c_object_type_definition::v30(long a)
{
	return false;
}
