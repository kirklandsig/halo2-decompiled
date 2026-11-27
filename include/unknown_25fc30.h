/* UNKNOWN_25FC30.H: firing position evaluation structures (offsets taken from
the retail code; only the fields the evaluators touch are named) */

#ifndef UNKNOWN_25FC30_H
#define UNKNOWN_25FC30_H

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"

struct s_type_d4fbfa
{
	byte unknown00[0xc];
	short unknown0c;
	union
	{
		struct
		{
			byte flags;
			byte unknown0f;
		};
		short flags0e;
	};
	short unknown10;
	short unknown12;
	long unknown14;
};

/* geometry of a firing position being evaluated (+0x0c..+0x4b, 0x40 bytes) */
struct s_firing_position_geometry
{
	point3f position;
	real unknown18;
	vector3f unknown1c;
	real unknown28;
	real unknown2c;
	real unknown30;
	vector3f unknown34;
	vector3f unknown40;
};

/* a firing position being evaluated (0x78 bytes) */
struct s_type_b36ac5
{
	s_type_d4fbfa *definition;
	s_reference reference;
	short type;
	byte unknown0a[2];
	union
	{
		s_firing_position_geometry geometry;
		struct
		{
			point3f position;
			real unknown18;
			vector3f unknown1c;
			real unknown28;
			real unknown2c;
			real unknown30;
			vector3f unknown34;
			vector3f unknown40;
		};
	};
	bool unknown4c;
	bool unknown4d;
	byte unknown4e[2];
	real unknown50;
	real score;
	bool unknown58;
	bool unknown59;
	bool unknown5a;
	byte unknown5b;
	short unknown5c;
	byte unknown5e[2];
	point3f unknown60;
	vector3f unknown6c;
};

struct s_type_967e20
{
	byte type;
	byte unknown01[3];
	union
	{
		struct
		{
			bool unknown04;
			byte unknown05[3];
			long unknown08;
			byte unknown0c[4];
			bool unknown10;
			bool unknown11;
			byte unknown12[2];
			bool unknown14;
			byte unknown15[3];
			real unknown18;
		};
		struct
		{
			byte unknown04_[4];
			real range08;
			real range0c;
			real range10;
			byte unknown14_[4];
			real range18;
		};
	};
	byte unknown1c[0x51 - 0x1c];
	bool unknown51;
	byte unknown52[2];
	bool unknown54;
	bool unknown55;
	bool unknown56;
	byte unknown57;
	bool unknown58;
	byte unknown59;
	bool unknown5a;
	bool unknown5b;
	bool unknown5c;
	byte unknown5d[3];
	real unknown60;
	void *unknown64;
	byte unknown68[0x70 - 0x68];
	long sphere_count;
	struct
	{
		real radius;
		point3f center;
	} spheres[0x20];
	short line_count;
	short unknown276;
	short unknown278;
	byte unknown27a[2];
	struct
	{
		short type;
		byte unknown02[2];
		point3f point;
		vector3f direction;
	} lines[0x20];
	byte unknown5fc[0x60c - 0x5fc];
	byte unknown60c[0x618 - 0x60c];
	bool unknown618;
	byte unknown619[3];
	real unknown61c;
	byte unknown620[0x24];
	bool unknown644;
	byte unknown645[0x65c - 0x645];
	short unknown65c;
	byte unknown65e[0x668 - 0x65e];
	bool unknown668;
	byte unknown669[3];
	vector3f unknown66c;
	real unknown678;
	byte unknown67c[4];
	real unknown680;
	long unknown684;
	long unknown688;
	bool unknown68c;
	byte unknown68d;
	short unknown68e;
	short unknown690;
	bool unknown692;
	byte unknown693;
	long unknown694;
	bool unknown698;
	byte unknown699;
	short unknown69a;
	byte unknown69c[0x6a0 - 0x69c];
	long unknown6a0;
};

#endif
