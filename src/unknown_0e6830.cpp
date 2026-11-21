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

// score 3 (was 3): only the register order in the request word's address differs: retail
// [esi + ebx + 4] (the byte offset as the base), ours [ebx + esi + 4], at the test, the reload
// and the store. Retail's explicit shl esi, 2 rather than a scaled index suggests the byte
// offset form is right. Tried, all 3: the sum's text order (block + offset + 4, block + 4 +
// offset, offset + 4 + block), block->active plus the offset either way, a bits pointer plus the
// offset either way, [1] off the word, the mask and offset declared at the top or the offset
// before the block, the loop variable at the top, the result before the block, the block
// through a byte pointer, an offset local or offset first, an unsigned offset, a signed mask.
// Worse: an array index (scaled address) 10/22/27; bit_vector_test and bit_vector_set 22/37;
// (type >> 5) << 2 5; the offset before the mask 37. 120 permuter variants: best 3.
// Also 3: the sum as integers either way, the offset cast to a byte pointer as the base, [1] off
// an offset-based pointer, sizeof(dword), a scaled dword pointer, and unused includes (string.h,
// math.h, stdio.h, stdlib.h, a repo header) or dummy declarations before the function; a word
// pointer or reference 27; an unsigned type with / and % 5.
// Matched in a full build with stand-in SDK headers (was 3): the request word is tested as `block->active[type >> 5]`
// and the byte offset local is declared only in the arm that clears the word. Named after the test, the offset gets a
// later id than the block pointer, which gives retail's [offset + block + 4] operand order at all three sites.
// @retail 0xe6830
bool __stdcall function_e6830(long unit_index)
{
    byte *object = ((s_e6830_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
    s_e6830_request_block *block = (s_e6830_request_block *)(object + *(short *)(object + 0x346));
    bool result = false;
    for (long type = 0; type < 60; type++)
    {
        dword mask = 1UL << (type & 31);
        if (block->active[type >> 5] & mask)
        {
            t_unit_request_update_proc update = g_4677c8[type]->update;
            if (update)
            {
                if (update(unit_index, type))
                    result = true;
                else
                {
                    long byte_offset = (type >> 5) * 4;
                    *(dword *)(byte_offset + (byte *)block + 4) &= ~mask;
                }
            }
        }
    }
    return result;
}
