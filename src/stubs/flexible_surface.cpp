#include "flexible_surface_calls.h"


// @stub 0x4d0b0
void __stdcall function_4d0b0(long a, long b, long c, long d, long e, long f, void *record) { }

// @stub 0x423c0
void __stdcall function_423c0(void *payload) {}



// @stub 0x508d0
void __stdcall function_508d0(void *payload) {}

struct s_sort_record;
typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);

// @stub 0x40e30
void function_40e30(short group, long tag, real distance, long level, word kind,
    dword and_mask, dword or_mask, t_record_fill fill, dword value, void *context) {}
