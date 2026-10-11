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
__declspec(noinline) void function_bb7b0(long object_index);
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
	c_turret_entity_definition *volatile self = this;
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
    c_turret_entity_definition const *volatile unused_this = this;
	bool valid = function_a6810(info, stream);
	if (function_1957d0(stream))
	{
		long index = function_1959c0(stream, 10);
		info->identifier = ((byte)function_1959c0(stream, 4) << 28) | index;
	}
	else
	{
		info->identifier = NONE;
	}
	if (stream->bit_position <= stream->size_in_bytes * 8 && valid)
    {
        return true;
    }
    return false;
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
	c_object_type_definition const *volatile saved_definition = this;
	if (function_a6d50(b, d, stream) && stream->bit_position <= stream->size_in_bytes * 8)
	{
		return true;
	}
	return false;
}

// @retail 0xa3d10
bool c_object_type_definition::v16(long a, long b, long c)
{
	c_object_type_definition *volatile definition = this;
	return function_a7180(a, b);
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
    c_object_type_definition *volatile self = this;
    (void)&self;
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
    long value = d;
    entity->object_index = v29(b, info, &value, e, f);
    if (entity->object_index != NONE)
    {
        if (!(OBJECT_HEADER(entity->object_index)->flags & 0x10))
        {
            OBJECT(entity->object_index)->field_d4 = entity->field0;
            function_108e10(entity->object_index);
            result = true;
            if (value)
            {
                v31(entity->object_index, value, e, f);
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
	c_object_type_definition const *volatile saved_definition = this;
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
	c_object_type_definition *volatile definition = this;
	long index = b & 0x3ff;
	if (index)
	{
		function_a6430(index, a, d);
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

#include "object_types_21_2.h"

class c_generic_type : public c_object_type_definition
{
public:
	virtual const char *v1();
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual bool v28(long index);
};

// @retail 0xa0690
const char *c_generic_type::v1()
{
	return "generic";
}

// @retail 0xa06b0
bool c_generic_type::v28(long index)
{
	s_object_view *object = OBJECT(index);
	bool result = false;
	if (object->field_ab != 2 || (((1 << object->type) & 2) && !TEST_FIELD_BIT(object->flag2)))
		result = true;
	return result;
}

// @retail 0xa0440
void c_item_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "item creation: relevance=%5.3f", relevance);
}

// @retail 0xa0890
void c_generic_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "generic creation: relevance=%5.3f", relevance);
}

// @retail 0xa0ee0
void c_projectile_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "projectile creation: relevance=%5.3f", relevance);
}

// @retail 0xa2310
void c_weapon_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "weapon creation: relevance=%5.3f", relevance);
}

// @retail 0xa40a0
void c_device_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "device creation: relevance=%5.3f", relevance);
}


// @retail 0xa02e0
long c_item_type::v27(long a, long b, long c, long d)
{
 // Preserve the local definition pointer stored in the retail stack frame.
 c_object_type_definition *volatile definition = this;
 byte *object = (byte *)OBJECT(a);
 long result = function_a5e70(a, b & 0x3ff, d);
 if (b & 0x400)
 {
  bool flag = (object[0xc1] & 1) != 0;
  if (flag != *(bool *)(d + 0x90))
  {
   *(bool *)(d + 0x90) = flag;
   result |= 0x400;
  }
 }
 return result;
}

// @retail 0xa0be0
long c_projectile_type::v27(long a, long b, long c, long d)
{
 // Preserve the local definition pointer stored in the retail stack frame.
 c_object_type_definition *volatile definition = this;
 long result = function_a5e70(a, b & 0x3ff, d);
 if (b & 0x400)
 {
  byte *object = (byte *)OBJECT(a);
  bool flag = (object[0xc1] & 1) != 0;
  if (flag != *(bool *)(d + 0x90))
  {
   *(bool *)(d + 0x90) = flag;
   result |= 0x400;
  }
 }
 return result;
}

class c_handle_table_450cd0;
bool function_99640(c_handle_table_450cd0 *self, long handle);

// @retail 0xa0e50
bool c_projectile_type::v8(long a, long b)
{
 bool result = true;
 long identifier = *(long *)(*(long *)(a + 0x14) + 0x14);
 if (identifier != NONE)
 {
  byte *table = *(byte **)(*(long *)(*(long *)b + 4) + 8);
  if (!function_99640((c_handle_table_450cd0 *)(table + 0x30), identifier))
   return false;
 }
 return result;
}

// @retail 0xa3df0
long c_device_type::v27(long a, long b, long c, long d)
{
 // Preserve the local definition pointer stored in the retail stack frame.
 c_object_type_definition *volatile definition = this;
 byte *object = (byte *)OBJECT(a);
 dword flags = b;
 long result = function_a5e70(a, flags & 0x3ff, d);
 if ((flags & 0x400) && *(real *)(d + 0x90) != *(real *)(object + 0x140))
 {
  *(real *)(d + 0x90) = *(real *)(object + 0x140);
  result |= 0x400;
 }
 if (flags & 0x800)
 {
  long index = *(long *)(object + 0x13c);
  if (index != NONE)
  {
   byte *entry = g_4e0328.groups->data + (index & 0xffff) * 12;
   if (*(real *)(d + 0x94) != *(real *)(entry + 4))
   {
    *(real *)(d + 0x94) = *(real *)(entry + 4);
    result |= 0x800;
   }
  }
 }
 return result;
}

#include "unknown_1946f0.h"
void simulation_write_position(long bits, s_bitstream *stream, real const *position, bool keep_inside);
void simulation_read_position(s_bitstream *stream, real *position, long bits);
void scenario_object_name_encode(long object_name, s_bitstream *stream);

struct s_z_impact_payload
{
 long object_name;
 real scale_a;
 real scale_b;
 vector3f direction;
 vector3f position;
 vector3f normal;
 word field_30;
 byte unknown32[2];
 bool field_34;
 dword field_38;
};

PRIVATE inline void z_write_impact_scale(s_bitstream *stream, real value)
{
 real scaled = value * 126.0f;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, 7);
}

PRIVATE inline real z_read_impact_scale(s_bitstream *stream)
{
 long value = function_1959c0(stream, 7);
 real result;
 if (value == 0) result = 0.0f;
 else if (value >= 126) result = 1.0f;
 else result = ((126 - value) * 0.0f + value * 1.0f) * (1.0f / 126.0f);
 return result;
}

struct s_z_impact_scenario
{
 byte unknown000[0x3d8];
 long count;
 long *names;
};

PRIVATE __forceinline long z_impact_read_name(s_bitstream *stream)
{
 long result = NONE;
 long index = function_1959c0(stream, 9) - 1;
 if (index != NONE)
 {
  s_z_impact_scenario *scenario = (s_z_impact_scenario *)g_4e0350;
  result = NONE;
  if (scenario && scenario->count > 0)
   if ((index < 0 ? 0 : (index > scenario->count - 1 ? scenario->count - 1 : index)) == index)
    result = scenario->names[index];
 }
 return result;
}

// @retail 0xa18c0
void c_projectile_impact_effect_event::v9(long a, void const *data, s_bitstream *stream)
{
 s_z_impact_payload const *event = (s_z_impact_payload const *)data;
 scenario_object_name_encode(event->object_name, stream);
 z_write_impact_scale(stream, event->scale_a);
 z_write_impact_scale(stream, event->scale_b);
 function_194bc0(stream, &event->direction);
 simulation_write_position(12, stream, (real const *)&event->position, true);
 function_194bc0(stream, &event->normal);
 function_1955d0(stream, &event->field_30, 16);
}

// @retail 0xa1ba0
void c_projectile_object_impact_effect_event::v9(long a, void const *data, s_bitstream *stream)
{
 s_z_impact_payload const *event = (s_z_impact_payload const *)data;
 scenario_object_name_encode(event->object_name, stream);
 z_write_impact_scale(stream, event->scale_a);
 z_write_impact_scale(stream, event->scale_b);
 function_194bc0(stream, &event->direction);
 simulation_write_position(12, stream, (real const *)&event->position, true);
 function_194bc0(stream, &event->normal);
 function_1955d0(stream, &event->field_30, 16);
 stream_write_checked(stream, event->field_38, 8);
 stream_write_bit(stream, event->field_34);
}

// @retail 0xa1960
bool c_projectile_impact_effect_event::v10(long a, void *data, s_bitstream *stream)
{
 s_z_impact_payload *event = (s_z_impact_payload *)data;
 event->object_name = z_impact_read_name(stream);
 event->scale_a = z_read_impact_scale(stream);
 event->scale_b = z_read_impact_scale(stream);
 long packed_direction_1 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_1, &event->direction);
 simulation_read_position(stream, (real *)&event->position, 12);
 long packed_direction_2 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_2, &event->normal);
 function_195820(stream, &event->field_30, 16);
 return true;
}

// @retail 0xa1cc0
bool c_projectile_object_impact_effect_event::v10(long a, void *data, s_bitstream *stream)
{
 s_z_impact_payload *event = (s_z_impact_payload *)data;
 event->object_name = z_impact_read_name(stream);
 event->scale_a = z_read_impact_scale(stream);
 event->scale_b = z_read_impact_scale(stream);
 long packed_direction_3 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_3, &event->direction);
 simulation_read_position(stream, (real *)&event->position, 12);
 long packed_direction_4 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_4, &event->normal);
 function_195820(stream, &event->field_30, 16);
 event->field_38 = function_1959c0(stream, 8);
 event->field_34 = function_1957d0(stream);
 return true;
}

void __stdcall function_107520(long object_index);
void function_107370(long device_index, real value);
bool __stdcall function_1071e0(long group_index, real value);

// @retail 0xa3fd0
void function_a3fd0(long index, bool immediate, bool set_position, real position, bool set_power, real power)
{
 byte *object = (byte *)OBJECT(index);
 if (set_position && !(position < 0.0f || position > 1.0f))
 {
  *(real *)(object + 0x140) = position;
  *(real *)(object + 0x144) = 0.0f;
  function_107520(index);
  long group = *(long *)(object + 0x13c);
  if (group != NONE)
   *(real *)(g_4e0328.groups->data + (group & 0xffff) * 12 + 4) = position;
 }
 if (set_power && !(power < 0.0f || power > 1.0f))
 {
  long group = *(long *)(object + 0x13c);
  if (group != NONE)
  {
   if (immediate)
    function_107370(index, power);
   else
    function_1071e0(group, power);
  }
 }
}

void *function_122c10(long group_tag, long tag_index);
void function_fd0e0(long definition_index, real scale_a, real scale_b, vector3f const *direction,
 point3f const *point, vector3f const *normal, long index, bool attached, long object_index,
 short node_index, bool alternate);

// @retail 0xa1a80
bool c_projectile_impact_effect_event::v11(long a, long const *entities, long c, void const *data)
{
 s_z_impact_payload const *event = (s_z_impact_payload const *)data;
 if (event->object_name != NONE && function_122c10(0x70726f6a, event->object_name) &&
  !(event->scale_a < 0.0f || event->scale_a > 1.0f || event->scale_b < 0.0f || event->scale_b > 1.0f))
  function_fd0e0(event->object_name, event->scale_a, event->scale_b, &event->direction,
   (point3f const *)&event->position, &event->normal, event->field_30, false, NONE, NONE, false);
 return true;
}


bool function_100f00(long weapon_index);

// @retail 0xa1f70
long c_weapon_type::v27(long a, long b, long c, long d)
{
 byte *object = (byte *)OBJECT(a);
 volatile long result = ((c_item_type *)this)->c_item_type::v27(a, b & 0x7ff, 0x94, d);
 dword requested = b;
 s_object_view *updated_object = OBJECT(a);
 s_tag_instance *tags = g_4e3b44;
 byte *definition = tags[updated_object->definition_index & 0xffff].bytes;
 if (*(short *)(definition + 0x290))
 {
  if (requested & 0x800)
  {
   dword source_flags = *(word *)(object + 0x16c);
   byte flags = (byte)((source_flags >> 8) & 1);
   if (source_flags & 0x200) flags |= 2; else flags &= ~2;
   if (source_flags & 0x400) flags |= 4; else flags &= ~4;
   if (source_flags & 0x800) flags |= 8; else flags &= ~8;
   if (*(byte *)(d + 0x94) != flags)
   {
    *(byte *)(d + 0x94) = flags;
    result |= 0x800;
   }
  }
  if ((requested & 0x1000) && *(short *)(d + 0x96) != *(short *)(object + 0x17e))
  {
   *(short *)(d + 0x96) = *(short *)(object + 0x17e);
   result |= 0x1000;
  }
 }
 if (requested & 0x2000)
 {
  definition = tags[*(long *)object & 0xffff].bytes;
  bool changed = false;
  bool force = !function_100f00(a);
  if (*(real *)(d + 0x9c) != *(real *)(object + 0x184) || force)
  {
   *(real *)(d + 0x9c) = *(real *)(object + 0x184);
   result |= 0x2000;
   changed = true;
  }
  if (*(long *)(definition + 0x2c0) > 0 &&
   (*(short *)(d + 0x98) != *(short *)(object + 0x22c) ||
    *(short *)(d + 0x9a) != *(short *)(object + 0x22a) || changed || force))
  {
   *(real *)(d + 0x9c) = *(real *)(object + 0x184);
   *(short *)(d + 0x98) = *(short *)(object + 0x22c);
   *(short *)(d + 0x9a) = *(short *)(object + 0x22a);
   return result | 0x2000;
  }
 }
 return result;
}


// @retail 0xa0640
bool function_a0640(c_object_type_definition *definition, long a, long *flags, long state, s_bitstream *stream)
{
 bool result = function_a6d50((long)flags, state, stream);
 if (function_1957d0(stream))
 {
  *(bool *)(state + 0x90) = function_1957d0(stream);
  *flags |= 0x400;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}

// @retail 0xa0520
bool c_item_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 return function_a0640(this, a, (long *)b, d, stream);
}

// @retail 0xa1350
bool c_projectile_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a6d50(b, d, stream);
 if (function_1957d0(stream))
 {
  *(bool *)(d + 0x90) = function_1957d0(stream);
  *(long *)b |= 0x400;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}

PRIVATE inline real z_read_object_fraction(s_bitstream *stream, long bits, long maximum)
{
 long value = function_1959c0(stream, bits);
 real result;
 if (!value) result = 0.0f;
 else if (value >= maximum) result = 1.0f;
 else result = ((maximum - value) * 0.0f + value * 1.0f) * (1.0f / maximum);
 return result;
}

// @retail 0xa4320
bool c_device_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a6d50(b, d, stream);
 if (function_1957d0(stream))
 {
  *(real *)(d + 0x90) = z_read_object_fraction(stream, 14, 16383);
  *(long *)b |= 0x400;
 }
 if (function_1957d0(stream))
 {
  *(real *)(d + 0x94) = z_read_object_fraction(stream, 14, 16383);
  *(long *)b |= 0x800;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}

// @retail 0xa26d0
bool c_weapon_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 bool result = function_a0640(this, a, (long *)b, d, stream) != false;
 if (function_1957d0(stream))
 {
  *(byte *)(d + 0x94) = (byte)function_1959c0(stream, 4);
  *(long *)b |= 0x800;
 }
 if (function_1957d0(stream))
 {
  *(short *)(d + 0x96) = (short)(function_1959c0(stream, 5) - 1);
  *(long *)b |= 0x1000;
  if (*(short *)(d + 0x96) != NONE)
   result = result && *(short *)(d + 0x96) >= 0 && *(short *)(d + 0x96) < 9;
 }
 if (function_1957d0(stream))
 {
  *(word *)(d + 0x98) = (word)function_1959c0(stream, 8);
  *(word *)(d + 0x9a) = (word)function_1959c0(stream, 11);
  *(real *)(d + 0x9c) = z_read_object_fraction(stream, 7, 126);
  *(long *)b |= 0x2000;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}


#include "flags_writer.h"
void function_194830(s_bitstream *stream, bool value);

// @retail 0xa0540
bool function_a0540(c_object_type_definition *definition, long a, long requested, long *written, long state, s_bitstream *stream, long reserve)
{
 bool result = false;
 long const *reserve_reference = &reserve;
 long total_reserve = reserve + 1;
 if (function_a69a0(a, requested & 0x3ff, (long)written, state, (long)stream, definition->v33(), total_reserve))
 {
  s_flags_writer writer;
  flags_writer_initialize(&writer, stream, 10, 1, requested & 0x400, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 10, "at-rest-exists"))
    function_194830(stream, *(bool const *)(state + 0x90));
   flags_writer_end(&writer);
   *written |= writer.written;
   result = true;
  }
 }
 return result;
}

// @retail 0xa1250
bool c_projectile_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 bool result = false;
 long const *reserve_reference = &a8;
 long reserve = a8 + 1;
 if (function_a69a0(a1, a2 & 0x3ff, a3, a5, a7, v33(), reserve))
 {
  s_flags_writer writer;
  s_bitstream *stream = (s_bitstream *)a7;
  flags_writer_initialize(&writer, stream, 10, 1, a2 & 0x400, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 10, "at-rest-exists"))
    function_194830(stream, *(bool const *)(a5 + 0x90));
   flags_writer_end(&writer);
   *(long *)a3 |= writer.written;
   result = true;
  }
 }
 return result;
}

PRIVATE inline void z_write_device_fraction(s_bitstream *stream, real value)
{
 real scaled = value * 16383.0f;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, 14);
}

// @retail 0xa41b0
bool c_device_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 bool result = false;
 long const *reserve_reference = &a8;
 long reserve = a8 + 2;
 if (function_a69a0(a1, a2 & 0x3ff, a3, a5, a7, v33(), reserve))
 {
  s_flags_writer writer;
  s_bitstream *stream = (s_bitstream *)a7;
  flags_writer_initialize(&writer, stream, 10, 2, a2 & 0xc00, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 10, "position-exists"))
    z_write_device_fraction(stream, *(real const *)(a5 + 0x90));
   flags_writer_end(&writer);
   if (flags_writer_begin(&writer, 11, "position-group-position-exists"))
    z_write_device_fraction(stream, *(real const *)(a5 + 0x94));
   flags_writer_end(&writer);
   *(long *)a3 |= writer.written;
   result = true;
  }
 }
 return result;
}

// @retail 0xa4170
bool c_device_type::v13(long a, s_entity_info *info, s_bitstream *stream)
{
 c_object_type_definition const *volatile definition = this;
 bool valid = function_a6810(info, stream);
 if (stream->bit_position <= (stream->size_in_bytes << 3) && valid)
  return true;
 return false;
}

// @retail 0xa23a0
bool c_weapon_type::v13(long a, s_entity_info *info, s_bitstream *stream)
{
 c_object_type_definition const *volatile definition = this;
 bool valid = function_a6810(info, stream);
 valid = stream->bit_position <= (stream->size_in_bytes << 3) && valid;
 if (stream->bit_position <= (stream->size_in_bytes << 3) && valid)
  return true;
 return false;
}

// @retail 0xa04f0
bool c_item_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 return function_a0540(this, a1, a2, (long *)a3, a5, (s_bitstream *)a7, a8);
}

PRIVATE inline void z_write_weapon_fraction(s_bitstream *stream, real value)
{
 real scaled = value * 126.0f;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, 7);
}

// @retail 0xa2470
bool c_weapon_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 bool result = false;
 long const *reserve_reference = &a8;
 long reserve = a8 + 3;
 s_bitstream *stream = (s_bitstream *)a7;
 if (function_a0540(this, a1, a2 & 0x7ff, (long *)a3, a5, stream, reserve))
 {
  s_flags_writer writer;
  flags_writer_initialize(&writer, stream, 11, 3, a2 & 0x3800, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 11, "multiplayer-state-exists"))
    stream_write_checked(stream, *(byte const *)(a5 + 0x94), 4);
   flags_writer_end(&writer);
   if (flags_writer_begin(&writer, 12, "multiplayer-team-exists"))
    stream_write_checked(stream, *(short const *)(a5 + 0x96) + 1, 5);
   flags_writer_end(&writer);
   if (flags_writer_begin(&writer, 13, "weapon-ammo-exists"))
   {
    stream_write_checked(stream, *(short const *)(a5 + 0x98), 8);
    stream_write_checked(stream, *(short const *)(a5 + 0x9a), 11);
    z_write_weapon_fraction(stream, *(real const *)(a5 + 0x9c));
   }
   flags_writer_end(&writer);
   *(long *)a3 |= writer.written;
   result = true;
  }
 }
 return result;
}


// @retail 0xa03d0
void c_item_type::v31(long a, long b, long c, long d)
{
 c_object_type_definition *volatile definition = this;
 if (b & 0x400)
  function_b9b90(a, *(bool const *)(d + 0x90));
 long mask = b & 0x3ff;
 if (mask)
  function_a6430(mask, a, d);
}

// @retail 0xa0e10
void c_projectile_type::v31(long a, long b, long c, long d)
{
 c_object_type_definition *volatile definition = this;
 long mask = b & 0x3ff;
 if (mask)
  function_a6430(mask, a, d);
 if (b & 0x400)
  function_b9b90(a, *(bool const *)(d + 0x90));
}


// @retail 0xa27f0
bool c_weapon_type::v16(long a, long b, long c)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a7180(a, b) &&
  fabs(*(real const *)(b + 0x9c) - *(real const *)(a + 0x9c)) < 0.007936508394777775f;
 *(real *)(b + 0x9c) = 0.0f;
 *(real *)(a + 0x9c) = 0.0f;
 return result;
}

// @retail 0xa3f70
void c_device_type::v31(long a, long b, long c, long d)
{
 c_object_type_definition *volatile definition = this;
 long mask = b & 0x3ff;
 if (mask)
  function_a6430(mask, a, d);
 if (b & 0xc00)
  function_a3fd0(a, false, (bool)(((dword)b >> 10) & 1), *(real const *)(d + 0x90),
   (bool)(((dword)b >> 11) & 1), *(real const *)(d + 0x94));
}

// @retail 0xa4410
bool c_device_type::v16(long a, long b, long c)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a7180(a, b) &&
  fabs(*(real const *)(b + 0x90) - *(real const *)(a + 0x90)) < 6.103888154029846e-05f;
 *(real *)(b + 0x90) = 0.0f;
 *(real *)(a + 0x90) = 0.0f;
 result = result &&
  fabs(*(real const *)(b + 0x94) - *(real const *)(a + 0x94)) < 6.103888154029846e-05f;
 *(real *)(b + 0x94) = 0.0f;
 *(real *)(a + 0x94) = 0.0f;
 return result;
}


void function_15e130(long object_index);
void function_15e050(long object_index, short value);

struct s_z_weapon_update_object
{
 long definition_index;
 byte unknown004[0x16c - 4];
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
 word flag11 : 1;
 word flag12 : 1;
 word flag13 : 1;
 word flag14 : 1;
 word flag15 : 1;
 byte unknown16e[0x17e - 0x16e];
 short field17e;
 byte unknown180[4];
 long field184;
 byte unknown188[0x22a - 0x188];
 short field22a;
 short field22c;
};

// @retail 0xa2140
void c_weapon_type::v31(long a, long b, long c, long d)
{
 c_object_type_definition *volatile definition = this;
 long mask = b & 0x7ff;
 s_z_weapon_update_object *object = (s_z_weapon_update_object *)OBJECT(a);
 if (mask)
 {
  if (mask & 0x400)
   function_b9b90(a, *(bool const *)(d + 0x90));
  mask &= 0x3ff;
  if (mask)
   function_a6430(mask, a, d);
 }
 long state_mask = b & 0x800;
 if (state_mask || (b & 0x1000))
 {
  byte *tag = *(byte **)((byte *)g_4e3b44 + (OBJECT(a)->definition_index & 0xffff) * 16 + 8);
  if (*(short const *)(tag + 0x290))
  {
   if (state_mask)
   {
    if (*(byte const *)(d + 0x94) & 1) object->flag8 = true;
    else object->flag8 = false;
    if (*(byte const *)(d + 0x94) & 2) object->flag9 = true;
    else object->flag9 = false;
    if (*(byte const *)(d + 0x94) & 4) object->flag10 = true;
    else object->flag10 = false;
    if (*(byte const *)(d + 0x94) & 8) object->flag11 = true;
    else object->flag11 = false;
   }
   if ((b & 0x1000) && object->field17e != *(short const *)(d + 0x96))
   {
    if (TEST_FIELD_BIT(object->flag6))
     function_15e130(a);
    word value = *(word const *)(d + 0x96);
    if (value != 0xffff)
     function_15e050(a, value);
   }
  }
 }
 if (b & 0x2000)
 {
  byte *tag = *(byte **)((byte *)g_4e3b44 + (object->definition_index & 0xffff) * 16 + 8);
  object->field184 = *(long const *)(d + 0x9c);
  if (*(long const *)(tag + 0x2c0) > 0)
  {
   byte *entry = *(byte **)(tag + 0x2c4);
   short first = *(short const *)(d + 0x98);
   short second = *(short const *)(d + 0x9a);
   if (first <= *(short const *)(entry + 0xa) && second <= *(short const *)(entry + 0xc))
   {
    object->field22c = first;
    object->field22a = *(short const *)(d + 0x9a);
   }
  }
 }
}

// @retail 0xa1120
bool c_projectile_type::v13(long a, s_entity_info *info, s_bitstream *stream)
{
    c_object_type_definition const *volatile definition = this;
    bool valid = (unsigned char)function_a6810(info, stream) != 0;
    long *fields = (long *)info->vehicle_data;
    if (function_1957d0(stream))
    {
        fields[0] = function_1959c0(stream, 4);
        valid = valid && fields[0] >= 0 && fields[0] < 16;
    }
    else
        fields[0] = NONE;
    if (function_1957d0(stream))
    {
        long index = function_1959c0(stream, 10);
        byte salt = (byte)function_1959c0(stream, 4);
        fields[1] = index | ((dword)salt << 28);
        if (function_1957d0(stream))
            fields[2] = function_1959c0(stream, 5);
        else
            fields[2] = NONE;
    }
    else
    {
        fields[1] = NONE;
        fields[2] = NONE;
    }
    info->vehicle_data[12] = function_1957d0(stream);
    info->vehicle_data[13] = function_1957d0(stream);
    if (valid && stream->bit_position <= (stream->size_in_bytes << 3))
        return true;
    return false;
}


// Disabled: serialization/initial-state callees still omit the required stream/output in protected callers.
#if 0
struct s_z_initial_state;
void __stdcall function_a6660(s_entity_info const *, s_bitstream *);
bool function_a5bd0(s_entity_info const *, s_z_initial_state *);

// Retail 0xa2380
void c_item_type::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
    c_item_type *volatile self = this;
    function_a6660(info, stream);
}

// Retail 0xa3db0
bool c_device_type::v19(long a, long info, long c, s_entity_data *data)
{
    c_device_type *volatile self = this;
    memset(data, 0, 0x98);
    bool result = false;
    if (function_a5bd0((s_entity_info *)info, (s_z_initial_state *)data)) result = true;
    return result;
}

// Retail 0xa02a0
bool c_item_type::v19(long a, long info, long c, s_entity_data *data)
{
    c_item_type *volatile self = this;
    memset(data, 0, 0x94);
    bool result = false;
    if (function_a5bd0((s_entity_info *)info, (s_z_initial_state *)data))
    {
        ((byte *)data)[0x90] = 0;
        result = true;
    }
    return result;
}

// Retail 0xa1f10
bool c_weapon_type::v19(long a, long info, long c, s_entity_data *data)
{
    c_weapon_type *volatile self = this;
    memset(data, 0, 0xa0);
    memset(data, 0, 0x94);
    bool result = false;
    if (function_a5bd0((s_entity_info *)info, (s_z_initial_state *)data))
    {
        ((byte *)data)[0x90] = 0;
        ((byte *)data)[0x94] = 0;
        *(short *)((byte *)data + 0x96) = NONE;
        result = true;
    }
    return result;
}
#endif



// Disabled: required stream cannot be supplied through a6660's current protected one-argument interface.
#if 0
// Retail 0xa0f50
void c_projectile_type::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
    c_projectile_type *volatile self = this;
    function_a6660(info, stream);
    byte const *data = (byte const *)info;
    long first = *(long const *)(data + 0x10);
    stream_write_bit(stream, first != NONE);
    if (first != NONE) stream_write_checked(stream, first, 4);
    long second = *(long const *)(data + 0x14);
    stream_write_bit(stream, second != NONE);
    if (second != NONE)
    {
        function_b5650(second, stream);
        long third = *(long const *)(data + 0x18);
        stream_write_bit(stream, third != NONE);
        if (third != NONE) stream_write_checked(stream, third, 5);
    }
    stream_write_bit(stream, data[0x1c] != 0);
    stream_write_bit(stream, data[0x1d] != 0);
}
#endif



// @retail 0xa0350
long c_item_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    c_item_type *volatile self = this;
    byte creation[0xc4];
    function_a5d90(creation, info, flags, state);
    long index;
    if (info->field0 != NONE) index = function_a73b0(info);
    else index = function_b7b40(creation);
    if (index != NONE) ((byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object)[0xaf] = info->byte8;
    return index;
}

// @retail 0xa3eb0
long c_device_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    c_device_type *volatile self = this;
    byte creation[0xc4];
    function_a5d90(creation, info, flags, state);
    long index;
    if (info->field0 != NONE) index = function_a73b0(info);
    else index = function_b7b40(creation);
    if (index != NONE)
    {
        ((byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object)[0xaf] = info->byte8;
        if (*flags & 0xc00)
        {
            function_a3fd0(index, true, (*flags & 0x400) != 0, *(real *)(state + 0x90),
                (*flags & 0x800) != 0, *(real *)(state + 0x94));
            *flags &= ~0xc00;
        }
    }
    return index;
}

struct s_projectile_target
{
    long object_index;
    short node_index;
    byte unknown06[2];
};
void projectile_set_target(long index, s_projectile_target const *target);

// @retail 0xa0c50
long c_projectile_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    c_projectile_type *volatile self = this;
    byte creation[0xc4];
    function_a5d90(creation, info, flags, state);
    byte *fields = (byte *)info;
    long player = *(long *)(fields + 0x10);
    if (player != NONE && player >= 0 && player < g_4e8c24->high_water_index)
    {
        s_record_pool *players = g_4e8c24;
        byte *records = players->data;
        byte *record = records + player * players->size;
        if (*(short *)record)
        {
            long player_index = player | (*(short volatile *)(records + player * players->size) << 16);
            *(long *)(creation + 0x60) = *(long *)(record + 0x2c);
            *(long *)(creation + 0x5c) = player_index;
            *(long *)(creation + 0x64) = (signed char)record[0xc0];
            *(long *)(creation + 0x6c) = *(long *)(record + 0x2c);
            *(long *)(creation + 0x68) = player_index;
            *(short *)(creation + 0x70) = (signed char)record[0xc0];
        }
    }
    long index;
    if (info->field0 != NONE) index = function_a73b0(info);
    else index = function_b7b40(creation);
    if (index != NONE)
    {
        s_record_pool *objects = g_4e0300;
        long offset = (index & 0xffff) * 12;
        byte *object = *(byte **)(objects->data + offset + 8);
        object[0xaf] = info->byte8;
        if (fields[0x1d])
        {
            object = *(byte **)(objects->data + offset + 8);
            *(dword *)(object + 0x12c) |= 0x100;
        }
        long target = *(long *)(fields + 0x14);
        if (target != NONE)
        {
            byte *entry = *(byte **)((byte *)g_4cf77c + 4) + 0x2098 + (target & 0x3ff) * 0x20 + 0x14;
            if (*(long *)entry == target && *(long *)(entry + 8) != NONE)
            {
                s_projectile_target request;
                request.object_index = *(long *)(entry + 8);
                request.node_index = *(short *)(fields + 0x18);
                projectile_set_target(index, &request);
            }
        }
        if (!fields[0x1c])
        {
            object = *(byte **)(objects->data + offset + 8);
            *(dword *)(object + 0x12c) &= ~2;
        }
    }
    return index;
}

// @retail 0xa2120
long c_weapon_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    return ((c_item_type *)this)->c_item_type::v29(0x10, info, flags, 0x94, state);
}

