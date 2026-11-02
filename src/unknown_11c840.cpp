#include "unknown_11c920.h"
#include "globals.h"
// @flags /O2 /Gr

struct s_tag_iterator
{
    long unknown00;
    long unknown04;
    long datum_index;
    long next_index;
    long group_tag;
};

class c_part;
struct s_pair_element;
struct s_single_element;
struct s_part_table
{
    byte unknown00[0x20];
    long pair_a_count;
    s_pair_element *pairs_a;
    long single_count;
    s_single_element *singles;
    long pair_b_count;
    s_pair_element *pairs_b;
    void construct_shapes();
    c_part *get_shape(long index);
};

long function_122c70(s_tag_iterator *arg_0);
void function_2122b0(long arg_0);
void function_205550(void *arg_0);
void function_1ea9c0(long arg_0, bool arg_1);

// @retail 0x11c840
void function_11c840()
{
    s_tag_iterator local_0;
    local_0.next_index = 0;
    local_0.group_tag = NONE;
    long local_1 = function_122c70(&local_0);
    while (local_1 != NONE)
    {
        switch (*(dword *)&g_4e3b44[(short)local_1])
        {
        case 0x62697064:
            ((s_part_table *)(g_4e3b44[local_1 & 0xffff].bytes + 0x264))->construct_shapes();
            break;
        case 0x636f6c6c:
            function_2122b0(local_1);
            break;
        case 0x63726561:
            ((s_part_table *)(g_4e3b44[local_1 & 0xffff].bytes + 0xd4))->construct_shapes();
            break;
        case 0x70686d6f:
            function_1ea9c0(local_1, false);
            break;
        case 0x76656869:
            function_205550(g_4e3b44[local_1 & 0xffff].bytes + 0x2ac);
            break;
        }
        local_1 = function_122c70(&local_0);
    }
}
