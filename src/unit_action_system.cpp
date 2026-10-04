// @flags /O2 /Gr
/* UNIT_ACTION_SYSTEM.CPP: the requests a unit performs (its actions)

The functions follow unit_action_system.obj in Bungie's May 2003 builds
(halo-symbol-atlas): the handlers of the unit request table at 0x467564
(unknown_0e6900.cpp) and the helpers only they call. */

#include "cseries.h"
#include "globals.h"
#include "unit_requests.h"
