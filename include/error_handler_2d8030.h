#ifndef LANE_B_ERROR_2D8030_H
#define LANE_B_ERROR_2D8030_H
// Havok (third-party physics library): the bookkeeping of its default error
// handler (vtable 0x411da4): counts of reports per source location, and the
// report ids switched off.
#include "growable_array_2d91d0.h"
#include "sized_reference_a9ef0.h"
#include "stream_holder_2d8e70.h"

// compares two strings ignoring the case of ASCII letters: <0, 0 or >0
int __cdecl compare_ignoring_case_2d8ce0(char const *left, char const *right);
// formats into `buffer`
int __cdecl format_2d8cc0(char *buffer, char const *format, ...);

// a source location: a file name and a line in it
struct location_2d8280
{
	char const *file;
	long line;
};

struct location_count_2d8280
{
	char const *file;
	long line;
	long count;
};

// the count kept for each location
struct location_counts_2d8280
{
	ELEMENTS_2D91D0_MEMBERS(location_count_2d8280)

	~location_counts_2d8280()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(location_count_2d8280), 0x12);
		}
	}

	// the count of `location`, or none
	long *find(location_2d8280 const &location);
	// the count of `location`, added (uncounted) when missing
	long *get(location_2d8280 const &location);
	// removes `location`: 0 when it was there, 1 when it was not
	int remove(location_2d8280 const &location);
};

struct id_value_2d8410
{
	long id;
	long value;
};

// a value kept for each report id
struct id_values_2d8410
{
	ELEMENTS_2D91D0_MEMBERS(id_value_2d8410)

	~id_values_2d8410()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(id_value_2d8410), 0x12);
		}
	}

	__forceinline long *find(long id)
	{
		for (long i = 0; i < size; i++)
		{
			if (data[i].id == id)
			{
				return &data[i].value;
			}
		}
		return 0;
	}
	// removes `id`: 0 when it was there, 1 when it was not
	int remove(long const &id);
	// the value of `id`, added (unset) when missing
	long *get(long const &id);
};

// the default error handler (vtable 0x411da4). Its vtable is not resolved
// here: the slots this file does not define are left pure, so a vtable this
// build may emit differs from retail's (2d7fe0, 2d7fb0, 2d7f70 and the
// repeated 2d8640/2d86a0 slots).
class error_handler_2d8030 : public c_sized_reference_a9ef0
{
public:
	REFERENCED_NEW_2D88A0(0x13)

	location_counts_2d8280 counts;
	id_values_2d8410 silenced;

	virtual ~error_handler_2d8030();
	// reports at three levels, unless silenced
	virtual void report_2d8030(char const *title, long id, char const *message, char const *file, long line);
	virtual void report_2d82e0(char const *title, long id, char const *message, char const *file, long line);
	virtual void report_2d8330(char const *title, long id, char const *message, char const *file, long line);
	// switches report `id` on or off
	virtual void set_enabled(long id, bool enabled);
	virtual flag_2d8e70 function_2d7fe0(long id);
	// counts a silenced location up, or down (forgotten at zero)
	virtual void count_location(char const *file, long line);
	virtual void slot_7(char const *file, long line) = 0;
	virtual void slot_8(char const *file, long line) = 0;
	virtual void uncount_location(char const *file, long line);
	virtual void slot_10(char const *file, long line) = 0;
	virtual void slot_11(char const *file, long line) = 0;
	virtual void slot_12(char const *text) const = 0;
	// the name kept for a source file
	virtual char const *file_name(char const *path) const = 0;

	// whether a report from `file` at `line` with `id` is silenced
	flag_2d8e70 is_silenced(char const *file, long line, long id);
	// writes and shows one report
	void show(long level, char const *title, long id, char const *message, char const *file, long line, flag_2d8e70 flag);
};

#endif
