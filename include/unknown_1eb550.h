/* UNKNOWN_1EB550.H: the globals of entry 25 (guessed layout), initialized by
   1eb550 and read by 211030 */

#ifndef UNKNOWN_1EB550_H
#define UNKNOWN_1EB550_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_unknown_1eb550
{
	real unknown0;
	real unknown4;
	real unknown8;
	vector3f vector;
	long unknown18;
};

extern s_unknown_1eb550 *g_51e9c4;

/* the world's gravity (the first value of entry 25's globals) */
inline real physics_gravity_get(void)
{
	return g_51e9c4->unknown0;
}

#endif
