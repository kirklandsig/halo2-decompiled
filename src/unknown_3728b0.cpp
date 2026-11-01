// @flags /O2 /GL-
#include "slot_table_372a40.h"

// @retail 0x3728b0
slot_table_372a40::~slot_table_372a40() {}

// @retail 0x372a40
slot_table_372a40::slot_table_372a40()
{
	for (long row = 0; row < 8; ++row)
		for (long column = 0; column < 8; ++column)
			slots[row][column] = &empty;
}
