/* TAG_GROUPS.H: the two ways a tag's data points elsewhere, as the Xbox map
   lays them out (8 bytes each) */

#ifndef TAG_GROUPS_H
#define TAG_GROUPS_H

#include "unknown_11c920.h"

/* a tag reference of a tag block */
struct s_tag_reference
{
	dword group_tag;
	long tag_index;
};

/* a tag block: its element count and, in a loaded map, the first element
   (the element type is named where the block is declared) */
struct s_tag_block
{
	long count;
	void *address;
};

typedef char assert_tag_reference_size[(sizeof(s_tag_reference) == 0x8) ? 1 : -1];
typedef char assert_tag_block_size[(sizeof(s_tag_block) == 0x8) ? 1 : -1];

#endif
