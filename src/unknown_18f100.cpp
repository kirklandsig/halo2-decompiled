// @flags /O2 /Gr /GL-
#include "unknown_11c920.h"
#include "unknown_12b400.h"

extern long g_4ed294;
void texture_cache_initialize_for_new_map(void);
void texture_cache_dispose_from_old_map(void);
bool __stdcall function_163890(char const *map_name, long mode);

// @retail 0x18f100
bool __stdcall function_18f100(char const *map_name)
{
	function_xe0ae94();
	texture_cache_initialize_for_new_map();
	g_4ed294 = 5;
	bool result = function_163890(map_name, 2);
	texture_cache_dispose_from_old_map();
	g_global_f9ae07.field_0--;
	g_4ed294 = 0;
	return result;
}
