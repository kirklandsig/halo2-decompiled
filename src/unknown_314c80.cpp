// @flags /O2 /GL- /GX
#include "constraint_helpers_314090.h"
#include <new>

// Nullable placement construction used by the prismatic description table.
// @retail 0x314c80
void __cdecl construct_description_314c80(void *storage)
{
	new (storage) c_component_joint_snapshot_5;
}
