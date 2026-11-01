/* UNKNOWN_249E20.H: the structure bsp's audibility data (g_4e0348) */
#ifndef UNKNOWN_249E20_H
#define UNKNOWN_249E20_H

#include "unknown_11c920.h"

struct s_structure_audibility
{
	long door_count;
	real distance_lower;
	real distance_upper;
	byte unknown0c[4];
	dword *bits;
	byte unknown14[4];
	dword *door_bits;
	byte unknown1c[4];
	dword *cluster_pair_bits;
	byte unknown24[4];
	byte *cluster_pair_distances;
	long cluster_count;
	char *clusters;
};

struct s_structure_bsp_view
{
	byte unknown00[0x60];
	short *sound_clusters;
	byte unknown64[0x9c - 0x64];
	long cluster_count;
	byte unknowna0[0xe8 - 0xa0];
	byte *cluster_pair_values;
	byte unknownec[0x220 - 0xec];
	struct s_structure_cluster_map
	{
		byte unknown00[0xc];
		short *indices;
	} *cluster_map;
	long audibility_count;
	s_structure_audibility *audibility;
};

long function_249e20(s_structure_bsp_view *bsp, long index);
long function_249e60(long cluster_index, s_structure_bsp_view *bsp, long index);
bool function_249c20(s_structure_bsp_view *bsp, long cluster_a, long cluster_b);
void function_249c90(s_structure_bsp_view *bsp, dword *bits_a, long index, dword *bits_b);
void function_249d10(s_structure_bsp_view *bsp, dword *bits, long index);
real function_249d60(s_structure_bsp_view *bsp, long cluster_a, long cluster_b);

#endif
