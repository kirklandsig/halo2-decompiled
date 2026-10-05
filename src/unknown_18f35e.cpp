// @flags /O1 /Oi /Gr /GL-
#include "unknown_11c920.h"
#include "screen_widgets.h"
#include <string.h>

/* The four entries constructed at +0x2a8 by 0x18f372 have stride 0x248.
   That array construction passes this constructor to an iterator. The
   separate translation unit preserves its standard thiscall convention
   until the caller's array construction is present. */
struct s_profile_list_entry
{
	s_profile_list_entry();
	long profile_index;
	s_player_profile_settings settings;
	byte unknown1e4[0x248 - 0x1e4];
};

// @retail 0x18f35e
s_profile_list_entry::s_profile_list_entry()
{
	profile_index = NONE;
	memset(&settings, 0, sizeof(settings));
}
