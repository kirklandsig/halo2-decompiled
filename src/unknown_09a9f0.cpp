// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_09A9F0.CPP: the turret simulation entity definition (vtable at
   0x452788) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include <string.h>

#define OBJECT_HEADER(index) (&((s_object_header *)g_4e0300->data)[(index) & 0xFFFF])
#define OBJECT(index) (OBJECT_HEADER(index)->object)

void function_108e10(long object_index);
void function_108e80(long object_index);
void function_bb7b0(long object_index);
// @retail 0x9f9d0
long c_turret_entity_definition::v0()
{
	return 0xf;
}

// @retail 0xa3980
const char *c_turret_entity_definition::v1()
{
	return "turret";
}

// @retail 0xa06a0
long c_object_type_definition::v2()
{
	return 0x90;
}

// @retail 0x9fce0
long c_object_type_definition::v3()
{
	return 0x14;
}

// @retail 0xa0250
long c_object_type_definition::v4()
{
	return 0xa;
}

// @retail 0x9a9f0
long c_object_type_definition::v5()
{
	return 0x3f;
}

// @retail 0xa59d0
bool c_object_type_definition::v6(s_entity *entity)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		result = v28(entity->object_index);
	}
	return result;
}

// @retail 0xa5bb0
bool c_object_type_definition::v7(s_entity *entity)
{
	if (entity->object_index != NONE && entity->field_6)
	{
		return true;
	}
	return false;
}

// @retail 0xa5750
bool c_object_type_definition::v8(long a, long b)
{
	return true;
}

// @retail 0xa3b10
void c_turret_entity_definition::v9(long a, long b, long *size)
{
	*size = 0x74;
}

// @retail 0xa3b20
void c_turret_entity_definition::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
	{
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	}
	function_11c9c0(buffer, size, "turret creation: relevance=%5.3f", relevance);
}

// @retail 0xa0ab0
void c_object_type_definition::v11(long a, long b, long c)
{
	((s_flags_a6900 const *)b)->function_a6900((long *)c, this);
}

// @retail 0xa3b90
void c_turret_entity_definition::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
	function_a6660(info);
	if (stream->size_in_bytes * 8 - stream->bit_position >= 1 && info->identifier != NONE)
	{
		stream->data[stream->bit_position / 8] |= (byte)(1 << (stream->bit_position % 8));
	}
	stream->bit_position++;
	if (info->identifier != NONE)
	{
		function_b5650(info->identifier, stream);
	}
}

// @retail 0xa3c00
bool c_turret_entity_definition::v13(long a, s_entity_info *info, s_bitstream *stream)
{
	bool valid = function_a6810(stream);
	if (function_1957d0(stream))
	{
		long index = function_1959c0(stream, 10);
		info->identifier = ((byte)function_1959c0(stream, 4) << 28) | index;
	}
	else
	{
		info->identifier = NONE;
	}
	return stream->bit_position <= stream->size_in_bytes * 8 && valid;
}

// @retail 0xa3cd0
bool c_object_type_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	bool result = false;
	if (function_a69a0(a1, a2 & 0x3ff, a3, a5, a7, v33(), a8))
	{
		result = true;
	}
	return result;
}

// @retail 0xa0960
bool c_object_type_definition::v15(long a, long b, long c, long d, s_bitstream *stream)
{
	if (function_a6d50(b, d, stream) && stream->bit_position <= stream->size_in_bytes * 8)
	{
		return true;
	}
	return false;
}

// @retail 0xa3d10
void c_object_type_definition::v16(long a, long b, long c)
{
	function_a7180(a, b);
}

// @retail 0xa4870
bool c_object_type_definition::v17(long a, long b, long c)
{
	return true;
}

// @retail 0xa59f0
void c_object_type_definition::v18(s_entity *entity, long b, s_entity_state *state)
{
	v26(entity->object_index, b, state);
}

// @retail 0xa07b0
bool c_object_type_definition::v19(long a, long b, long c, s_entity_data *data)
{
	bool result = false;
	memset(data, 0, sizeof(s_entity_data));
	if (function_a5bd0(b))
	{
		result = true;
	}
	return result;
}

// @retail 0xa5a00
bool c_object_type_definition::v20(s_entity *entity, long *index, long c, long d)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		long value = *index;
		if (OBJECT(entity->object_index)->type == 1)
		{
			value &= 0x3ff;
		}
		*index = v27(entity->object_index, value, c, d);
		result = true;
	}
	return result;
}

// @retail 0xa3c70
void c_turret_entity_definition::v21(s_entity *entity)
{
	s_entity_info *info = entity->info;
	if (entity->object_index != NONE)
	{
		OBJECT(entity->object_index)->field_d4 = entity->field0;
	}
	if (info->identifier != NONE)
	{
		long identifier = info->identifier;
		byte salt = (byte)(((long)((dword)identifier >> 28) + 1) % 16);
		info->identifier = (salt << 28) | (identifier & 0x3ff);
	}
}

// @retail 0xa5a60
bool c_object_type_definition::v22(s_entity *entity, long b, s_entity_info *info, long d, long e, long f)
{
	bool result = false;
	entity->object_index = v29(b, info, &d, e, f);
	if (entity->object_index != NONE)
	{
		if (!(OBJECT_HEADER(entity->object_index)->flags & 0x10))
		{
			OBJECT(entity->object_index)->field_d4 = entity->field0;
			function_108e10(entity->object_index);
			result = true;
			if (d)
			{
				v31(entity->object_index, d, e, f);
			}
		}
		else
		{
			entity->object_index = NONE;
		}
	}
	return result;
}

// @retail 0xa5b10
bool c_object_type_definition::v23(s_entity *entity, long b, long c, long d)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		v31(entity->object_index, b, c, d);
		result = true;
	}
	return result;
}

// @retail 0xa5b40
bool c_object_type_definition::v24(s_entity *entity)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		if (!v30(entity->object_index))
		{
			function_bb7b0(entity->object_index);
			function_b8540(entity->object_index);
		}
		entity->object_index = NONE;
		result = true;
	}
	return result;
}

// @retail 0xa5b80
bool c_object_type_definition::v25(s_entity *entity)
{
	long index = entity->object_index;
	bool result = false;
	if (index != NONE)
	{
		result = v32(entity->object_index);
	}
	return result;
}

// @retail 0xa3990
void c_turret_entity_definition::v26(long index, long b, s_entity_state *state)
{
	s_object_view *object = OBJECT(index);
	memset(state, 0, sizeof(*state));
	s_object_view *current = OBJECT(index);
	state->field4 = current->definition_index;
	state->field0 = current->field1a;
	state->field8 = current->field_af;
	state->fieldc = current->field24;
	state->field10 = function_a5930(object->field14);
}

// @retail 0xa3a00
long c_object_type_definition::v27(long a, long b, long c, long d)
{
	return function_a5e70(a, b & 0x3ff, d);
}

// @retail 0xa09b0
bool c_object_type_definition::v28(long index)
{
	s_object_view *object = OBJECT(index);
	bool result = false;
	if (object->field_ab != 2 || !TEST_FIELD_BIT(object->flag2))
	{
		result = true;
	}
	return result;
}

// @retail 0xa3a20
long c_turret_entity_definition::v29(long a, s_entity_info *info, long *c, long d, long e)
{
	long result = NONE;
	long found = function_a58d0(info->identifier);
	if (found != NONE)
	{
		long child = OBJECT(found)->first_child;
		if (child != NONE)
		{
			long definition_index = info->definition_index;
			do
			{
				s_object_view *object = OBJECT(child);
				if (object->definition_index == definition_index && object->field_d4 == NONE && result == NONE)
				{
					result = child;
				}
				child = object->next_sibling;
			}
			while (child != NONE);
		}
	}
	return result;
}

// @retail 0xa3a90
bool c_turret_entity_definition::v30(long index)
{
	bool result = false;
	s_object_view *object = OBJECT(index);
	long field14 = object->field14;
	if (field14 != NONE)
	{
		function_108e80(index);
		object->field_d4 = NONE;
		object->field_d8 = 0;
		result = true;
	}
	return result;
}

// @retail 0xa3ae0
void c_object_type_definition::v31(long a, long b, long c, long d)
{
	long index = b & 0x3ff;
	if (index)
	{
		function_a6430(a, index, d);
	}
}

// @retail 0x9bed0
bool c_object_type_definition::v33()
{
	return false;
}

// @retail 0xa0410
bool c_object_type_definition::v34(long a, s_entity_data *source, long *block)
{
	memcpy(block, source->block, sizeof(source->block));
	return true;
}

// @retail 0x9dcd0
void c_object_type_definition::v35(long a, s_entity_data *data, long *block)
{
	memcpy(data->block, block, sizeof(data->block));
}
