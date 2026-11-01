#ifndef FORCE_MOTION_31ACD0_H
#define FORCE_MOTION_31ACD0_H
#include "motion_step_319f70.h"

// Layout read by the force callbacks and the swept-pose interpolation.
// This is an accessed prefix, not a complete callable virtual interface.
// Allocation sites establish the 0x110 extent.
class force_motion_31acd0 : public class_3114d0
{
public:
	__m128 saved_linear;
	__m128 saved_angular;
	__m128 inverse_inertia;
	__m128 force;
	__m128 torque;
	void add_force_at(__m128 const &value, __m128 const &point);
	void interpolate_sweep(float fraction, float delta_time);
};
#endif
