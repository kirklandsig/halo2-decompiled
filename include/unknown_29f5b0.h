/* UNKNOWN_29F5B0.H: what the script functions' evaluators share */

#ifndef UNKNOWN_29F5B0_H
#define UNKNOWN_29F5B0_H

#include "unknown_11c920.h"
#include "hs.h"

/* script_return_store: stores a script function's value in the calling frame */
void function_209ae0(long thread_index, long value);

/* evaluates a script function's arguments one per call; returns the
   arguments once they are all evaluated, NULL until then */
long *__stdcall function_209d50(long thread_index, short parameter_count, short const *parameter_types, bool initialize);

/* a constant 0 of code not decompiled yet, defined beside the stubs */
extern long const g_444ae0;

/* callees not decompiled yet (stubs in src/stubs/lane_a.cpp) */
void __stdcall function_29fe10(long index);
void function_159ac0(void);
long function_15e730(void);
void function_277380(void);
void function_13bff0(void);
void function_13ca80(void);
void function_1388e0(void);
bool function_226190(void);
void function_24cdaf(void);
bool function_187ec0(void);
void function_135750(void);
void function_135790(void);
void function_1915f0(void);
void function_29f5b0(short trigger_volume_index, short cutscene_flag_index);
long function_11c5f0(long trigger_volume_index, long type_mask);
void __stdcall function_29fd80(short name_index);
void __stdcall function_10af80(long object_index, real value, short ticks);
short __stdcall function_d88f0(long object_index, long name);
void __stdcall function_bbfc0(real a, real b, real c, real d, real e);
void __stdcall function_bc070(real a, real b, real c, real d, real e);
void __stdcall function_29ffb0(long object_index, short cutscene_flag_index, bool a, bool b);
long __stdcall function_bb670(short name_index, bool flag);
void __stdcall function_ba6f0(long object_index, long region_index, long state, bool flag);
bool __stdcall function_bbec0(long object_index, bool value);
void __stdcall function_ba410(long object_index, long a, long b);
bool __stdcall function_1071e0(long device_group_index, real value);
bool __stdcall function_107ed0(long device_index, long name, real value);
void __stdcall function_1e1a00(long index, long value);
void __stdcall function_2736c0(long ai_index);
void __stdcall function_273ac0(long ai_index, long other_ai_index);
long __stdcall function_272ea0(long ai_index);
short __stdcall function_274470(long ai_index);
long function_2958a0(long name);
void function_276860(long ai_index, short mode);
void function_2770c0(bool enable);
void function_277250(real seconds);
void __stdcall function_2772b0(long animation_graph_index, long name, real value, bool flag);
void __stdcall function_277680(real value);
void __stdcall function_16c740(real value, short count);
void __stdcall function_13c1e0(short title_index, real value);
void __stdcall function_189cd0(long sound_index, long object_index, real scale, long a, long b, long name, long flags);
void __stdcall function_18a430(long looping_sound_index, long object_index, real scale);
void __stdcall function_13b306(real a, real b);
bool function_14ed80(void);
bool function_14ece0(void);
bool function_225fe0(void);
void function_1e7800(void);
void function_1e78b0(void);
void function_1e7960(void);
bool __stdcall function_fa9a0(long *value);
void __stdcall function_24d7ac(short navpoint_index, short team_index, short is_object, long target, real value);
void __stdcall function_24d877(short team_index, short is_object, long target);
void __stdcall function_187df0(bool value);
void function_135820(void);
void __stdcall function_13c250(long object_index, long a, long b);
bool __stdcall function_beb30(long object_index);

/* defined in other files: unknown_01e930.cpp, and a stub in src/stubs/unknown_09a9f0.cpp */
long bink_playback_ticks_remaining(void);
void __stdcall function_b8540(long object_index);

#endif
