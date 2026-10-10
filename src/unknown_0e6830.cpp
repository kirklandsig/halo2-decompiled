// @flags /O2 /Gr
// Update the active unit requests, retaining changes made by callbacks.
#include "unknown_11c920.h"
#include "globals.h"

struct s_unit_request;
typedef bool (__stdcall *t_unit_request_proc)(long unit_index, s_unit_request *request);
typedef bool (__stdcall *t_unit_request_update_proc)(long unit_index, long type);
typedef void (__stdcall *t_unit_request_end_proc)(long unit_index, long type);

// Same definition as the existing table in unknown_0e6900.cpp.
struct s_unit_request_definition
{
    t_unit_request_proc perform;
    t_unit_request_update_proc update;
    t_unit_request_end_proc finished;
    t_unit_request_end_proc interrupted;
};
extern s_unit_request_definition *g_4677c8[60];

struct s_e6830_object_header
{
    byte unknown00[8];
    byte *object;
};

struct s_e6830_request_block
{
    dword unknown00;
    dword active[2];
};

// @retail 0xe6830
bool __stdcall function_e6830(long unit_index)
{
    byte *object = ((s_e6830_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
    s_e6830_request_block *block = (s_e6830_request_block *)(object + *(short *)(object + 0x346));
    bool result = false;
    for (long type = 0; type < 60; type++)
    {
        dword mask = 1UL << (type & 31);
        long byte_offset = (type >> 5) * 4;
        if (*(dword *)(byte_offset + (byte *)block + 4) & mask)
        {
            t_unit_request_update_proc update = g_4677c8[type]->update;
            if (update)
            {
                if (update(unit_index, type))
                    result = true;
                else
                    *(dword *)(byte_offset + (byte *)block + 4) &= ~mask;
            }
        }
    }
    return result;
}
