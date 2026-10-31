// @flags /O2 /GL-
// Havok (third-party physics library): the name of a file without the
// folders in its path, whichever separator they use.
#include "string_helpers_2d8c90.h"
#include "stream_holder_2d8e70.h"
#include "error_handler_2d8030.h"

struct class_2d9070;
class_2d9070 *__cdecl function_2d9070(class_2d9070 *writer);

class reporter_2ff210;
extern reporter_2ff210 *g_48017c;

class reporter_2d7fb0
{
public:
	virtual void slot_0() = 0;
	virtual void slot_1() = 0;
	virtual stream_holder_2d8e70 *slot_2() = 0;
};

#include "service_streams_2d8780.h"

// @retail 0x2d7f70
char const *path_2d7f70::function_2d7f70(char const *path) const
{
	for (int i = 0; i < 2; i++)
	{
		char const *separator = function_2d8da0(path, "/\\"[i]);
		if (separator)
		{
			path = separator + 1;
		}
	}
	return path;
}

// @retail 0x2d7fb0
void path_2d7f70::function_2d7fb0(char const *text) const
{
	reporter_2d7fb0 *reporter = (reporter_2d7fb0 *)g_48017c;
	function_2d9070((class_2d9070 *)reporter->slot_2()->function_2d8f10(text));
}

// @retail 0x2d7fe0
flag_2d8e70 error_handler_2d8030::function_2d7fe0(long id)
{
	return flag_2d8e70(silenced.find(id) == 0);
}
