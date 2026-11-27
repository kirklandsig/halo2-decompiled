#ifndef __UNKNOWN_20FE20_H__
#define __UNKNOWN_20FE20_H__

#include "unknown_0259d0.h"

/* a point, either in the world or (output_index not NONE) relative to the
   node of an object that an output entry of g_4f93a0 names */
struct s_type_c3b527
{
	point3f point;
	short output_index;
};

bool function_2104b0(short output_index, point3f const *point, point3f *out);
point3f *function_210850(s_type_c3b527 const *point, point3f *out);
real function_210a30(s_type_c3b527 const *a, s_type_c3b527 const *b);
inline real function_210b60(s_type_c3b527 const *a, point3f const *b);
/* to world space (2104b0 points, 2105b0 vectors) and back (210690, 210770) */
bool function_2105b0(short output_index, vector3f const *vector, vector3f *out);
bool function_210690(short output_index, point3f const *point, point3f *out);
bool function_210770(short output_index, vector3f const *vector, vector3f *out);
/* the distance between two points, and the vectors between them */
real function_210970(s_type_c3b527 const *a, s_type_c3b527 const *b);
real function_210ac0(s_type_c3b527 const *a, point3f const *b);
void function_210be0(s_type_c3b527 const *a, s_type_c3b527 const *b, vector3f *out);
void function_210c90(point3f const *b, s_type_c3b527 const *a, vector3f *out);

#endif
