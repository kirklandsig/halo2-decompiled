#ifndef FLEXIBLE_SURFACE_CALLS_H
#define FLEXIBLE_SURFACE_CALLS_H
#include "unknown_11c920.h"

typedef bool (__stdcall *surface_render_test)(long, long, long, long, long, long, void *);
typedef void (__stdcall *surface_render_draw)(long, long, long, long, long, long, void *);
void function_40f60(void *, surface_render_test, surface_render_draw);
bool __stdcall function_d4bc0(long, long, long, long, long, long, void *);
void function_1bd50(void *);
void function_1cdd0(long, long);
void function_4b2d0(long, long, long, long, long);
void function_1c6b0(void *);
void __stdcall function_1c710(void *);
#endif
