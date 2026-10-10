// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1EB550.CPP: the lifecycle callbacks of entry 25 */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_1eb550.h"

s_unknown_1eb550 *g_51e9c4;

// @retail 0x1eb550
void function_1eb550(void)
{
	s_unknown_1eb550 *data = (s_unknown_1eb550 *)function_123d40("unknown", "unknown", sizeof(s_unknown_1eb550));

	data->unknown0 = 4.1712594f;
	data->unknown4 = 1.0f;
	data->unknown8 = 0.0011f;
	data->unknown18 = 0;
	g_51e9c4 = data;
	data->vector = *g_4687a4;
}

// @retail 0x1eb5e0
void function_1eb5e0(void)
{
	g_51e9c4 = 0;
}

// @retail 0x1eb5f0
void function_1eb5f0(void)
{
	s_unknown_1eb550 *reset_settings_block = g_51e9c4;

	reset_settings_block->unknown0 = 4.1712594f;
	reset_settings_block->unknown4 = 1.0f;
	reset_settings_block->unknown8 = 0.0011f;
	reset_settings_block->unknown18 = 0;
	reset_settings_block->vector = *g_4687a4;
}
