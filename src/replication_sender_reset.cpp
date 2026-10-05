// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"

struct s_sender;
void function_96ed0(s_sender *self);

struct s_sender_tables
{
	byte unknown00[0xc];
	s_sender *tables[15];
};

// @retail 0x89dc0
void replication_table_clear_senders(s_sender_tables *senders)
{
	for (long i = 0; i < 15; i++)
	{
		if (senders->tables[i])
			function_96ed0(senders->tables[i]);
	}
}
