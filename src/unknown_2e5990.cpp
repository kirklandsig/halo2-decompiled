// @flags /O2 /GL-
// Havok (third-party physics library): resets a 32x32 table of handlers to
// its defaults. The three default handlers are empty functions; retail's
// linker folded them (with the game's own empty functions) into 0x175f40.

#include "dispatcher_views_2e5990.h"

static void __cdecl default_handler_a(void)
{
}

static void __cdecl default_handler_b(void)
{
}

static void __cdecl default_handler_c(void)
{
}

// @retail 0x2e5990
void table_2e5990::reset()
{
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            entry[i][j] = default_entry;
            first[i][j] = default_handler_a;
            second[i][j] = default_handler_b;
            third[i][j] = default_handler_c;
            cell[i][j].weight = 100;
        }
    }
}

// Sets the flag at offset 0x4d8d from its one-byte argument (passed by value
// through a copy constructor, as the game code that calls it declares it).
#include "agent_table_2e5d50.h"

// @retail 0x2e5a10
void s_1c3a40::function_2e5a10(s_2e5a10 arg_0)
{
    field_4d8d = arg_0.field_0;
}
