#pragma once
/* The online result cache: the server list a stats query returns, and the
   one server whose key and address are registered. */
#include <xtl.h>

#pragma pack(push, 2)
struct s_online_cached_result
{
    unsigned __int64 entity;
    word unknown08;
    XNADDR address;
    word unknown2e;
    XNKID key_id;
    word unknown38;
    XNKEY key;
};
#pragma pack(pop)
typedef char online_cached_result_size[sizeof(s_online_cached_result) == 0x4a ? 1 : -1];

struct s_online_result_list
{
    s_online_cached_result results[8];
    dword count;
    long registered_index;
    IN_ADDR registered_address;
    long key_count;
};
typedef char online_result_list_size[sizeof(s_online_result_list) == 0x260 ? 1 : -1];

extern s_online_result_list g_510588;
