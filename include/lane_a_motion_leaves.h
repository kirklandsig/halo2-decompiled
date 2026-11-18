#ifndef LANE_A_MOTION_LEAVES_H
#define LANE_A_MOTION_LEAVES_H
#include "derived_311920.h"

struct lane_a_motion_step { float delta_time; };
struct lane_a_motion_output
{
	unsigned char kind;
	unsigned char metadata[11];
	unsigned char enabled;
	__m128 rows[7];
	float &component(long row, long lane) { return rows[row].m128_f32[lane]; }
};
// Retail table415130 has32 entries; following table4151b0 is separately
// installed by31a6b0. Empty slots preserve their observed stack cleanup.
class class_3193c0 : public class_3114d0
{
public:
	class_3193c0(void const *position, void const *orientation);
	OBJECT_NEW_3114B0
	class_3193c0 *source;
	// slot0 inherited genuine virtual destructor; its chain remains an audit.
	virtual void integrate(lane_a_motion_step const *step); //1
	virtual void slot2(void *, void *) {} //24dc50
	virtual char *export_state(long, long, lane_a_motion_output *out) const; //3
	virtual char *integrate_output(lane_a_motion_step const *, float, lane_a_motion_output *); //4
	virtual long output_size() { return 0x80; } //5:31aac0
	virtual long kind() { return 6; } //6:09ac70
	virtual void set_mass(float value); //7
	virtual void function_319740(float value); //8
	virtual void inertia(__m128 *out) const { out[0]=out[1]=out[2]=_mm_setzero_ps(); } //9
	virtual void inverse_inertia(__m128 *out) const { out[0]=out[1]=out[2]=_mm_setzero_ps(); } //10
	virtual void slot11(void *) {} //72c70
	virtual void slot12(void *) {} //72c70
	virtual void inertia_world(__m128 *out) const { out[0]=out[1]=out[2]=_mm_setzero_ps(); } //13
	virtual void inverse_inertia_world(__m128 *out) const { out[0]=out[1]=out[2]=_mm_setzero_ps(); } //14
	virtual void set_local_centre(__m128 const *value); //15
	virtual void store_position(__m128 const *value); //16
	virtual void set_rotation(__m128 const *value); //17
	virtual void set_position_rotation(__m128 const *position, __m128 const *orientation); //18
	virtual void store_transform(transform_312630 const *value); //19
	virtual void store_linear_velocity(__m128 const *value); //20
	virtual void store_angular_velocity(__m128 const *value); //21
	virtual void velocity_at(__m128 const *point, __m128 *out) const; //22
	virtual void slot23(void *) {} //72c70
	virtual void slot24(void *,void *) {} //24dc50
	virtual void slot25(void *) {} //72c70
	virtual void slot26(void *,void *) {} //24dc50
	virtual void slot27(void *) {} //72c70
	virtual void slot28(void *) {} //72c70
	virtual void slot29(void *,void *) {} //24dc50
	virtual void snapshot_into(class_3193c0 *out) const; //30
	virtual void set_source(class_3193c0 *value); //31
};
class class_31a6b0 : public class_3193c0
{
public:
	class_31a6b0(void const *position, void const *orientation);
};
#endif
