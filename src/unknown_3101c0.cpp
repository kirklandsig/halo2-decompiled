// @flags /O2 /GL- /GX
// Havok (third-party physics library): the constructor of the simulated
// world. It warns when the code using the library was built against another
// version, takes its settings from a description, and makes the objects it
// keeps: the table of empty slots, the object that dispatches pair queries
// (with the empty object it starts from), the settings the solver steps
// with, the fixed body every other body can be joined to, and the stepping
// values.
#include "reference_counted_2d88a0.h"
#include "stream_holder_2d8e70.h"
#include "tagged_objects_2e4ff0.h"
#include "owned_reference_318b90.h"
#include "motion_description_3115a0.h"
#include "slot_table_372a40.h"
#include "pair_notifications_315e70.h"
#include "havok_reference.h"
#include "unknown_1cec30.h"
#include <xmmintrin.h>
#include <new>

class_2e4ff0 *__cdecl function_2e4f70(long arg_0, long arg_1, int count);
struct object_4805a4;
object_4805a4 *__cdecl function_301a40();

// the game's error handler; its third slot reports a message
class error_handler_2d97d0
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void report(char const *title, unsigned long id, char const *message, char const *file, long line) {}
};

extern error_handler_2d97d0 *g_480178;

// the dispatcher of pair queries (0x2e5fd0)
class dispatcher_2e5fd0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x22)

	char unknown8[0x4da8 - 8];

	dispatcher_2e5fd0(void *default_entry_, referenced_2d88a0 *empty);
};

// the world's settings for the bodies it steps (0x30c770)
class class_30c770
{
public:
	void function_30c770();
};

// a one-byte boolean with a constructor
struct flag_3171a0
{
	char value;

	flag_3171a0(bool value_) : value((char)value_) {}
};

// the object 0x3171a0 builds for the world
class object_3171a0
{
public:
	REFERENCED_NEW_2D88A0(0x2d)

	char unknown0[0x24];
	long field_24;
	flag_3171a0 flag_28;
	flag_3171a0 flag_29;
	char unknown2a[2];
	flag_3171a0 flag_2c;
	char unknown2d[3];
	flag_3171a0 flag_30;
	char unknown31[0xb0 - 0x31];

	object_3171a0(long value);
};

// a body's description as it is built (0x311340)
struct s_311340 : description_3115a0
{
	s_311340();
};

// the description of the fixed body, which gives its pairs back when it goes
struct fixed_description_3101c0 : s_311340
{
	~fixed_description_3101c0()
	{
		if ((pairs.capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(pairs.data, (pairs.capacity_and_flags & 0x7fffffff) * sizeof(pair_312430), 0x12);
		}
	}
};

// a body built from a description (0x311ba0)
class c_311ba0
{
public:
	REFERENCED_NEW_2D88A0(0x28)

	char unknown0[0xa0];

	c_311ba0(s_311340 const *description);
};

// blocks from the allocator with a tag, not recording their size
#define BLOCK_NEW_3101C0(tag) \
	static void *operator new(unsigned int size) \
	{ \
		return g_memory_2d88a0->allocate_block(size, tag); \
	} \
	static void operator delete(void *block, unsigned int size) \
	{ \
		g_memory_2d88a0->release_block(block, size, tag); \
	}

// The world's pair notification handler for its special matrix entries.
class empty_entry_3101c0 : public dispatcher_315e70
{
public:
	BLOCK_NEW_3101C0(0x1c)
};

// the parts of the solver settings that answer queries
class settings_part_a_3101c0
{
public:
	virtual void slot_0() {}
};

class settings_part_b_3101c0
{
public:
	virtual void slot_0() {}
};

class settings_part_c_3101c0
{
public:
	virtual void slot_0() {}
};

class settings_part_d_3101c0
{
public:
	virtual void slot_0() {}
};

// the object the solver settings answer through
class settings_owner_3101c0 : public referenced_2d88a0, public settings_part_a_3101c0,
	public settings_part_b_3101c0, public settings_part_c_3101c0, public settings_part_d_3101c0
{
public:
	REFERENCED_NEW_2D88A0(0x22)
};

// the values the solver steps with
struct step_values_3101c0
{
	BLOCK_NEW_3101C0(0x1a)

	bool field_0;
	float field_4;
	float field_8;
	long field_c;
	long field_10;
	bool field_14;
	float field_18;
	long field_1c;

	step_values_3101c0()
	{
		field_0 = true;
		field_4 = 0.1f;
		field_8 = 0.1f;
		field_c = 0;
		field_14 = true;
		field_18 = 0.01f;
		field_1c = 20;
	}
};

// the solver's settings
struct settings_values_3101c0
{
	void *dispatcher;
	settings_part_b_3101c0 *owner;
	long field_8;
	step_values_3101c0 *values;
};

struct solver_settings_3101c0 : settings_values_3101c0
{
	BLOCK_NEW_3101C0(0x1a)
};

// a reference-counted object the world keeps last
class last_object_3101c0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(4)
};

// an array that starts empty, without storage of its own
struct array_3101c0
{
	void *data;
	long size;
	unsigned long capacity_and_flags;

	array_3101c0() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_3101c0()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(long), 0x12);
		}
	}
};

struct s_30cb70
{
	unsigned char field_0;
};

// the world's description
struct s_3111d0
{
	__m128 field_0;
	bool field_10;
	char unknown11[3];
	long field_14;
	char unknown18[8];
	__m128 field_20;
	__m128 field_30;
	long field_40;
	float field_44;
	float field_48;
	long field_4c;
	bool field_50;
	s_30cb70 field_51;
	char unknown52[2];
	float field_54;
	float field_58;
	long field_5c;
	long field_60;
	bool field_64;
	char unknown65[3];
	float field_68;
	float field_6c;
	long field_70;
	long field_74;
	bool field_78;
	char unknown79[3];
	float field_7c;
	long field_80;
	bool field_84;
	char unknown85[3];
	long field_88;
	long field_8c;
};

struct s_steps_3101c0
{
	float field_0;
	float field_4;
	float field_8;
	float field_c;
	char unknown10[8];
	float field_18;
	long field_1c;
	float field_20;
};

struct s_pair_3101c0
{
	float field_0;
	float field_4;
};

enum state_3101c0 { state_3101c0_clear = 0 };

class c_3101c0 : public referenced_2d88a0
{
public:
	array_3101c0 array_8;
	array_3101c0 array_14;
	array_3101c0 array_20;
	object_3171a0 *field_2c;
	long field_30;
	long field_34;
	c_311ba0 *fixed_body;
	array_3101c0 array_3c;
	array_3101c0 array_48;
	array_3101c0 array_54;
	array_3101c0 array_60;
	array_3101c0 array_6c;
	array_3101c0 array_78;
	array_3101c0 array_84;
	array_3101c0 array_90;
	array_3101c0 array_9c;
	array_3101c0 array_a8;
	array_3101c0 array_b4;
	long field_c0;
	class_2e4ff0 *field_c4;
	dispatcher_2e5fd0 *dispatcher;
	solver_settings_3101c0 *settings;
	settings_owner_3101c0 *settings_owner;
	char unknownd4[0xe0 - 0xd4];
	__m128 field_e0;
	array_3101c0 array_f0;
	array_3101c0 array_fc;
	array_3101c0 array_108;
	array_3101c0 array_114;
	array_3101c0 array_120;
	state_3101c0 field_12c : 8;
	state_3101c0 field_12d : 8;
	long field_130;
	long field_134;
	slot_table_372a40 *slots;
	empty_entry_3101c0 *empty_entry;
	settings_values_3101c0 settings_copy;
	long field_150;
	char unknown154[0x160 - 0x154];
	s_steps_3101c0 field_160;
	char unknown184[0x1a8 - 0x184];
	s_pair_3101c0 field_1a8;
	char unknown1b0[0x210 - 0x1b0];
	__m128 field_210;
	__m128 field_220;
	long field_230;
	long field_234;
	long field_238;
	bool field_23c;
	bool field_23d;
	bool field_23e;
	char unknown23f;
	bool field_240;
	char unknown241[3];
	float field_244;
	float field_248;
	long field_24c;
	last_object_3101c0 *field_250;
	long field_254;

	c_3101c0(s_3111d0 const &description, long version);
	void function_30cb70(s_30cb70 arg_0);
};

// @retail 0x3101c0
c_3101c0::c_3101c0(s_3111d0 const &description, long version)
{
	if (version != 0x4f4c)
	{
		char buffer[0x200];
		stream_holder_2d8e70 stream(buffer, sizeof(buffer), flag_2d8e70(true));
		stream.function_2d8f10("** Havok libs built with version [")
			->function_2d8f60(0x4f4c)
			->function_2d8f10("], used with code built with [")
			->function_2d8fb0(version)
			->function_2d8f10("]. **");
		g_480178->report("Error", 0x53c94b42, buffer, ".\\world\\hkWorld.cpp", 0xa74);
	}
	field_130 = 1;
	field_134 = 0xfffffff;
	field_e0 = description.field_0;
	field_23c = description.field_10;
	field_23d = description.field_84;
	field_30 = description.field_88;
	field_34 = description.field_8c;
	field_210 = description.field_20;
	field_220 = description.field_30;
	field_230 = description.field_40;
	field_c4 = function_2e4f70((long)&field_210, (long)&field_220, description.field_40);
	field_234 = description.field_14;
	field_238 = field_234 / 2;
	slots = new slot_table_372a40();
	empty_entry = new empty_entry_3101c0();
	slots->store_slot_372a40(10, empty_entry);
	slots->store_slot_372a40(17, empty_entry);
	slots->store_slot_372a40(18, empty_entry);
	class_319080 *empty = new class_319080(this);
	dispatcher = new dispatcher_2e5fd0(function_301a40, (referenced_2d88a0 *)empty);
	havok_reference_remove((c_havok_reference_counted *)empty);
	settings_owner = new settings_owner_3101c0();
	settings = new solver_settings_3101c0();
	settings->dispatcher = dispatcher;
	settings->field_8 = description.field_60;
	settings->values = new step_values_3101c0();
	settings->values->field_4 = description.field_68;
	settings->values->field_8 = description.field_6c;
	settings->values->field_c = description.field_70;
	settings->values->field_10 = description.field_74;
	settings->values->field_0 = description.field_64;
	settings->values->field_14 = description.field_78;
	settings->values->field_18 = description.field_7c;
	settings->values->field_1c = description.field_80;
	settings->owner = settings_owner;
	((class_30c770 *)this)->function_30c770();
	field_2c = new object_3171a0((long)this);
	field_2c->field_24 = -1;
	field_2c->flag_29.flag_3171a0::flag_3171a0(false);
	field_2c->flag_30.flag_3171a0::flag_3171a0(true);
	{
		fixed_description_3101c0 fixed;
		fixed.field_b4 = 7;
		fixed.field_a0 = 0.0f;
		fixed_body = new c_311ba0(&fixed);
		((hkWorld *)this)->entity_world_insert((hkEntity *)fixed_body);
	}
	settings_copy = *(settings_values_3101c0 *)settings;
	s_steps_3101c0 &steps = field_160;
	steps.field_0 = 1.0f;
	steps.field_4 = description.field_44;
	steps.field_8 = description.field_48;
	steps.field_c = description.field_44 * 0.5f;
	steps.field_1c = description.field_4c;
	steps.field_20 = 1.0f / description.field_4c;
	steps.field_18 = 0.99f;
	s_pair_3101c0 &pair = field_1a8;
	pair.field_4 = steps.field_20;
	pair.field_0 = steps.field_18;
	field_150 = 0;
	field_254 = 0;
	field_23e = false;
	field_240 = false;
	if (description.field_50)
	{
		function_30cb70(description.field_51);
	}
	field_248 = (field_244 = description.field_54) * description.field_58;
	field_24c = description.field_5c;
	field_250 = new last_object_3101c0();
	field_c0 = 0;
	field_12c = state_3101c0_clear;
	field_12d = state_3101c0_clear;
}
