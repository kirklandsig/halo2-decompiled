// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_218ac0.h"

struct s_animation_state;
struct s_animation_ref;
struct s_packed_value;
struct s_set_ref;

long function_2193d0(s_animation_state *state, s_animation_ref *ref);
void function_219310(s_animation_state *state, s_animation_ref *ref, short value);
void function_2192b0(s_animation_state *state, s_animation_ref *ref, byte value);
long function_219370(s_animation_state *state, s_animation_ref *ref);
dword function_2194c0(s_animation_ref *ref, s_packed_value *value);
void function_219560(s_packed_value *value, s_animation_ref *ref, dword data);
short function_219110(s_set_ref *ref, short offset, dword *used_mask, short previous,
	dword *seed, bool *all_used, bool mirror);

// score 3: retail sets up the function_219110 argument registers (lea esi,
// mov ecx) before pushing function_219370's result; ours pushes it first.
// Manual variants could not change the ordering.
// @retail 0x219070
short function_219070(long set_index, s_sound_definition const *definition)
{
	s_permutation_set *set = &g_51ebd4->sets[definition->pitch_range_base + (short)set_index];
	s_animation_ref *ref = (s_animation_ref *)definition;
	if ((short)function_2193d0((s_animation_state *)set, ref) != NONE)
	{
		short permutation = (short)function_2193d0((s_animation_state *)set, ref);
		function_219310((s_animation_state *)set, ref, NONE);
		function_2192b0((s_animation_state *)set, ref, (byte)permutation);
		return permutation;
	}
	dword permutations = function_2194c0(ref, (s_packed_value *)set);
	short permutation = function_219110((s_set_ref *)definition, (short)set_index, &permutations,
		(short)function_219370((s_animation_state *)set, ref), &g_4e7408->seed, NULL, false);
	function_219560((s_packed_value *)set, ref, permutations);
	function_2192b0((s_animation_state *)set, ref, (byte)permutation);
	return permutation;
}
