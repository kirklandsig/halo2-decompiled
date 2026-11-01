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
void function_1ea9c0(long arg_0, long arg_1);

// score 19 (was 19): retail passes function_1ea9c0's second argument as a stack byte, ours
// passes it in ECX; the callee's convention is set by its own (stubbed) file. BLOCKED-BY
// 0x1ea9c0 (its retail convention takes the tag index in eax and pushes the byte flag; every
// other line here only follows the 2-byte shift). Round 24: the flag declared bool 19 (xor dl,
// dl), a named group tag 19, an explicit default 19, `false` 19, a guarded do-while 19.
// Round 28: descriptive names 19; the lookup assigned in the loop condition 19; a for loop over
// the iterator 19; a named part table 19; the matched 0x12c640 iterator idiom 19. Round 100:
// iterator stores swapped 21; aggregate initialization 58; memset 62; the masked index cached 51
// (216 bytes); an instance-base local 19; an if chain 33 (216); a deferred preprocessing flag 19;
// a definition pointer 65 (199).
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
            function_1ea9c0(local_1, 0);
            break;
        case 0x76656869:
            function_205550(g_4e3b44[local_1 & 0xffff].bytes + 0x2ac);
            break;
        }
        local_1 = function_122c70(&local_0);
    }
}
