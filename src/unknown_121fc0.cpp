// @flags /O2 /arch:SSE /Gr
#include "font_loading_storage.h"
#include "unknown_11c920.h"
#include "font_loading.h"
#include "async.h"
#include "main_globals.h"
#include "unknown_058ee0.h"
#include <string.h>

extern bool g_4e3b40;
extern bool g_51ea00;
extern bool g_4ed39e;
extern bool g_4ed39d;
extern dword g_4ed3a0;
extern char const *g_4687f0;
bool function_121b00(void);
char *font_table_get_name(char *arg_0, long arg_1);
char *function_122810(char *arg_0, char const *arg_1);
void function_1223a0(long arg_0, char const *arg_1, bool arg_2);
void font_cache_initialize(void);
void font_cache_pixels_initialize(void);
void function_593e0(void);

/* the next token of a string separated by any of the delimiters (NULL when
   there is none); the token is terminated in place and *next set to the text
   after it */
static inline char *function_xe9ecc4(char *string, char const *delimiters, char **next)
{
	char *end = string;
	char *token;

	if (end)
	{
		end += strspn(end, delimiters);
		if (!*end)
			end = NULL;
	}
	token = end;
	if (end)
	{
		end = strpbrk(end, delimiters);
		if (end)
			*end++ = 0;
	}
	*next = end;
	return token;
}

/* loads the font table file named by the build's text path and maps its tokens to font files; the read
   runs on the async thread, which writes the size while this thread waits, and the failed read's else
   arm holds only a statement the release build compiles out */
// @retail 0x121fc0
void function_121fc0(void)
{
	for (long local_0 = 0; local_0 < 11; local_0++)
		g_4e28f4[local_0] = -2;
	if (!g_4e3b40)
	{
		memset(g_4e2920, 0, k_maximum_font_count * sizeof(s_font_cache_entry));
		if (!function_121b00())
		{
			g_51ea00 = true;
			main_globals.unknown29 = true;
			g_4ed39e = true;
			g_4ed39d = true;
			g_4ed3a0 = GetTickCount();
			if (g_527330.initialized && (g_527330.state == 3 || g_527330.state == 8))
				function_593e0();
		}
		char local_3[256];
		char local_4[256];
		char local_5[0x800];
		local_3[0] = 0;
		font_table_get_name(local_4, sizeof(local_4));
		strncpy(local_3, g_4687f0, sizeof(local_3));
		local_3[255] = 0;
		function_122810(local_3, local_4);
		bool local_6;
		bool local_7;
		dword volatile local_8;
		function_1a1480(local_3, local_5, sizeof(local_5) - 1, 7, 6, &local_8, &local_7, &local_6);
		function_120d50(&local_6, false);
		if (local_7)
		{
			long local_10 = 0;
			long local_11 = 0;
			char *local_9[11] = {0};
			local_8 = local_8 > sizeof(local_5) - 1 ? sizeof(local_5) - 1 : local_8;
			local_5[local_8] = 0;
			char *local_12;
			char *local_13 = function_xe9ecc4(local_5, "\t\n\r ", &local_12);
			while (local_13)
			{
				if (local_10 < 11)
				{
					long local_14 = NONE;
					local_9[local_10] = local_13;
					for (long local_15 = 0; local_15 < local_10; local_15++)
					{
						if (!strcmp(local_13, local_9[local_15]))
						{
							local_14 = local_15;
							break;
						}
					}
					if (local_14 != NONE)
						g_4e28f4[local_10] = g_4e28f4[local_14];
					else
					{
						g_4e28f4[local_10] = local_11;
						function_1223a0(local_11, local_13, true);
						local_11++;
					}
					local_10++;
				}
				local_13 = function_xe9ecc4(local_12, "\t\n\r ", &local_12);
			}
		}
		else
		{
			do {} while (false);
		}
		g_4e3b40 = true;
	}
	font_cache_initialize();
	font_cache_pixels_initialize();
}
