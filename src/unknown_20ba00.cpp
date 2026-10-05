#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Gr

struct s_audio_object_header
{
	byte unknown00[8];
	long *object;
};

struct s_audio_object_definition
{
	byte unknown000[0x1f0];
	short type;
};

struct s_audio_lookup_root
{
	byte unknown00[0xc8];
	long count;
	byte *entries;
};

struct s_audio_lookup_entry
{
	short value;
	short unknown02;
};

struct s_audio_lookup_table
{
	byte unknown00[0x24];
	long count;
	s_audio_lookup_entry *entries;
};

// @retail 0x20ba00
short function_20ba00(short index)
{
	short result = NONE;
	s_audio_lookup_root *root = (s_audio_lookup_root *)g_4e034c;
	if (root && root->count > 0)
	{
		long definition_index = *(long *)(root->entries + 0x64);
		if (definition_index != NONE)
		{
			s_audio_lookup_table *table = (s_audio_lookup_table *)g_4e3b44[definition_index & 0xffff].data;
			if (table && index >= 0 && index < table->count)
				result = table->entries[index].value;
		}
	}
	return result;
}

// @retail 0x20bb70
bool function_20bb70(long object_index)
{
	bool result = false;
	long definition_index = *((s_audio_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_audio_object_definition *definition = (s_audio_object_definition *)g_4e3b44[definition_index & 0xffff].data;
	if (definition->type == 6)
		result = true;
	return result;
}
