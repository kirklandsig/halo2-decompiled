// @flags /O2 /GL- /GX
#include "constraint_helpers_314090.h"
#include <new>

// Retail inlines these constructors into the nullable placement callbacks.
// Keeping the definitions together allows /O2 to do that; /GX supplies the
// allocation-cleanup exception frames visible in those callbacks.
// @retail 0x3140c0
c_component_joint_snapshot_4::c_component_joint_snapshot_4()
{
	pivot_a = _mm_setzero_ps();
	pivot_b = _mm_setzero_ps();
}
// @retail 0x314860
c_component_joint_snapshot_3::c_component_joint_snapshot_3()
{
	pivot_a = _mm_setzero_ps();
	pivot_b = _mm_setzero_ps();
	length = 0.0f;
}
// @retail 0x314320
s_314320::s_314320()
{
	*(float *)&field_0 = -1.0f;
	field_4 = 0.01f;
	field_8 = 0.005f;
	field_c = 0.1f;
	field_10 = 0.2f;
}

// @retail 0x314140
void __cdecl construct_description_314140(void *storage)
{
	new (storage) c_component_joint_snapshot_4;
}

// @retail 0x3143e0
void __cdecl construct_description_3143e0(void *storage)
{
	new (storage) s_314320;
}

// @retail 0x3148e0
void __cdecl construct_description_3148e0(void *storage)
{
	new (storage) c_component_joint_snapshot_3;
}
