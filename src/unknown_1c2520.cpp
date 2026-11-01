// @flags /O2 /Ob1 /Gr /GL- /GX
#include "pair_filter_class.h"

// Emit the interface deleting destructors and the implicit complete
// destructors in their own exception-enabled translation unit.
// @retail 0x1c2520 deleting pair_collection_filter
// @retail 0x1c2540 deleting pair_ray_collection_filter
// @retail 0x1c3450 destructor pair_filter_base
// @retail 0x1c3440 destructor c_2dfbf0
