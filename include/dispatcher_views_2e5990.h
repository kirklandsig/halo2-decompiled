#ifndef DISPATCHER_VIEWS_2E5990_H
#define DISPATCHER_VIEWS_2E5990_H
#include "agent_table_2e5d50.h"
#include "reference_counted_2d88a0.h"

typedef void (__cdecl *handler_2e5990)(void);

struct cell_2e5990
{
    char unknown0[2];
    char weight;
};

struct table_2e5990
{
    char unknown0[8];
    void *default_entry;
    char unknown_c[0x18c - 0xc];
    void *entry[32][32];
    handler_2e5990 first[32][32];
    handler_2e5990 second[32][32];
    handler_2e5990 third[32][32];
    cell_2e5990 cell[32][32];

    void reset();
};

// The same dispatcher as s_1c3a40, viewed through its reference table and
// alternate-type masks. These declarations are shared with the callers.
class table_2e5a20
{
public:
	char unknown0[0xc];
	referenced_2d88a0 *entry[8][8];
	unsigned long masks[32];
	char unknown18c[0x4d90 - 0x18c];
	alternate_types_2e5d50 alternate_types;

	void set_all(referenced_2d88a0 *object, long index);
	void function_2e5ce0(long target, long source, long depth);
};

#endif
