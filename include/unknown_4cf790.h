#pragma once

#include "unknown_11c920.h"
#include <stddef.h>

struct s_record_pool;
struct s_network_observer;

struct s_4cf790
{
	byte field_0;
	bool field_1;
	bool field_2;
	byte field_3[36];
	byte field_27;
	byte field_28[20];
	byte field_3c[6];
	byte field_42[2];
	byte field_44[256];
	bool field_144;
	byte field_145[3];
	s_record_pool *field_148;
	long field_14c;
	bool field_150;
	byte field_151[3];
	s_network_observer *field_154;
	real *field_158;
	bool field_15c;
	byte field_15d[3];
	long field_160[27];
	bool field_1cc;
	bool field_1cd;
	byte field_1ce[2];
	long field_1d0;
	bool field_1d4;
	byte field_1d5[3];
	long field_1d8;
	long field_1dc;
};

typedef char assert_4cf790_layout[(
    offsetof(s_4cf790, field_44) == 0x44 &&
    offsetof(s_4cf790, field_148) == 0x148 &&
    offsetof(s_4cf790, field_1d8) == 0x1d8 &&
    sizeof(s_4cf790) == 0x1e0) ? 1 : -1];

extern s_4cf790 g_4cf790_storage;

/* the cache of recently met players at 0x4cf970 (src/unknown_0805e0.cpp): its own
   object, the 0x8e4c bytes that 0x7fa40 reads from its file and 0x7fb50 writes back
   (a version, the checksum of the 0x8e44 bytes from the count on, the count, four
   list heads, then the entries) */
struct s_4cf970
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
	long field_10;
	long field_14;
	long field_18;
	byte field_1c[350 * 104];
};

typedef char assert_4cf970_layout[(
    offsetof(s_4cf970, field_8) == 0x8 &&
    offsetof(s_4cf970, field_1c) == 0x1c &&
    sizeof(s_4cf970) == 0x8e4c) ? 1 : -1];

extern s_4cf970 g_4cf970_storage;

#define g_4cf790 (g_4cf790_storage.field_0)
#define g_4cf791 (g_4cf790_storage.field_1)
#define g_4cf792 (g_4cf790_storage.field_2)
#define g_4cf793 (*(XNADDR *)g_4cf790_storage.field_3)
#define g_4cf7b8 (*(s_type_99af70 *)g_4cf790_storage.field_28)
#define g_4cf7cc (g_4cf790_storage.field_3c)
#define g_4cf7d4 (*(s_xnet_registry_entry (*)[8])g_4cf790_storage.field_44)
#define g_4cf8d4 (g_4cf790_storage.field_144)
#define g_4cf8d8 (g_4cf790_storage.field_148)
#define g_4cf8e0 (g_4cf790_storage.field_150)
#define g_4cf8e4 (g_4cf790_storage.field_154)
#define g_4cf8e8 (g_4cf790_storage.field_158)
#define g_4cf8ec (g_4cf790_storage.field_15c)
#define g_4cf8f0 (g_4cf790_storage.field_160)
#define g_4cf95c (g_4cf790_storage.field_1cc)
#define g_4cf95d (g_4cf790_storage.field_1cd)
#define g_4cf960 (g_4cf790_storage.field_1d0)
#define g_4cf964 (g_4cf790_storage.field_1d4)
#define g_4cf968 (g_4cf790_storage.field_1d8)
#define g_4cf96c (g_4cf790_storage.field_1dc)
#define g_4cf970 (g_4cf970_storage.field_0)
#define g_4cf974 (g_4cf970_storage.field_4)
#define g_4cf978 (g_4cf970_storage.field_8)
#define g_4cf97c (g_4cf970_storage.field_c)
#define g_4cf980 (g_4cf970_storage.field_10)
#define g_4cf984 (g_4cf970_storage.field_14)
#define g_4cf988 (g_4cf970_storage.field_18)
#define g_4cf98c (*(s_player_configuration_cache_entry (*)[350])g_4cf970_storage.field_1c)
