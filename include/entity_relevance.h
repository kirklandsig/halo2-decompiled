/* ENTITY_RELEVANCE.H: the update relevance and period of a simulation entity
   (0xabac0, src/unknown_0aa4d0.cpp), which the entity definitions print */

#ifndef ENTITY_RELEVANCE_H
#define ENTITY_RELEVANCE_H

#include "unknown_11c920.h"
#include "unknown_0a58d0.h"

/* what an entity's update was last sent with */
struct s_update_state;

real function_abac0(real *relevance_out, s_creation_request const *request, s_update_state const *state, long *period_out);

#endif
