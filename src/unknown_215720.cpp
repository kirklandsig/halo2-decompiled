// @flags /O2 /Gr
/* UNKNOWN_215720.CPP: a lifecycle callback (entry 9, field_10_2) */

#include "unknown_11c920.h"
#include "saved_game_storage.h"

void function_216760(void);

// @retail 0x215720
void function_215720(void)
{
	g_55c158 = NONE;
}

// @retail 0x2157c0
void function_2157c0(long selection)
{
	if (selection != NONE)
		g_55c158 = selection;
	if (g_51ea14)
	{
		*(long *)g_51ea14 = 0;
		function_216760();
	}
}


#include <xtl.h>

void function_125d60(void);
void function_215810(void);
void function_120d50(bool volatile *done, bool idle);

// @retail 0x215730
void function_215730(void)
{
    if (g_55c14d)
    {
        function_120d50(&g_55c14c, true);
        memset(&g_51e9f8, 0, sizeof(g_51e9f8));
        g_55c14d = false;
        g_55c158 = NONE;
    }
    function_215810();
}
