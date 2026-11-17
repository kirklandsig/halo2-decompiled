// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "unknown_059ad0.h"

// @retail 0x5b040
byte c_class_58d20::get_value_49c4()
{
	byte result = 0;

	if (state > 2 && state <= 8)
	{
		result = value49c4;
	}

	return result;
}

// @retail 0x5b060
long c_class_58d20::get_value_49c8()
{
	long result = NONE;

	if (state > 2 && state <= 8)
	{
		result = value49c8;
	}

	return result;
}

// @retail 0x5b080
byte *c_class_58d20::get_data_4a00()
{
	byte *result = 0;

	if (state > 2 && state <= 8)
	{
		if (flag49fd)
		{
			result = data4a00;
		}
	}

	return result;
}

// @retail 0x5b0b0
long c_class_58d20::get_value_5e20()
{
	long result = NONE;

	if (state > 2 && state <= 8)
	{
		result = value5e20;
	}

	return result;
}

// @retail 0x5b0d0
bool c_class_58d20::get_values_4d08(long *a, long *b, byte **c)
{
	bool result = false;

	if (state > 2 && state <= 8)
	{
		if (value4d0c != NONE && flag4d10)
		{
			*a = value4d08;
			*b = value4d0c;
			*c = &flag4d10;
			result = true;
		}
	}

	return result;
}

/* an 8-byte struct result is not kept by the stand-in caller (the call is
   dropped and the function folded away), so the pair is returned as an __int64 */
// @retail 0x5b120
__int64 c_class_58d20::get_values_4da0()
{
	__int64 values = NONE;

	if (state > 2 && state <= 8)
		values = value4da0_pair;

	return values;
}