// @flags /O2 /GL- /GX
#include "constraint_helpers_314090.h"

// Allocate a motion-limit object with the serialized limits and radius.
// @retail 0x314380
c_314710 *__cdecl construct_motion_limits_314380(s_314320 const *description)
{
	return new c_314710(description);
}
