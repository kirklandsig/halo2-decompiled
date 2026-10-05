#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_13927e.h"

// @flags /O1 /Gr

struct s_marker_kind_entry
{
	byte unknown00[0x10];
	short value10;
	short value12;
	byte unknown14[4];
};

struct s_marker_kinds_view
{
	byte unknown00[0x410];
	long count;
	s_marker_kind_entry *entries;
};

/* Whether any marker uses different values for the two display states. */
// @retail 0x24e554
bool function_24e554(s_marker_list const *list)
{
	bool result = false;
	if (list->count > 0)
	{
		long count = list->count;
		s_list_item const *item = list->items;
		do
		{
			s_marker_kinds_view *globals = (s_marker_kinds_view *)g_510c94;
			long index = item->kind;
			if (globals && index >= 0 && index < globals->count)
			{
				s_marker_kind_entry const *entry = &globals->entries[index];
				if (entry->value12 != entry->value10)
					result = true;
			}
			item++;
		} while (--count);
	}
	return result;
}
