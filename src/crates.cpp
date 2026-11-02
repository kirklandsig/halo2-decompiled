// @flags /O2 /Gr
/* CRATES.CPP: crate creation and update callbacks.
   Filename and routine names are inferred from retail's named crate type;
   see docs/crates.md for the mapping and shared dependency scope. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1cec30.h"

struct s_crate_object_view
{
	byte unknown000[0xb4];
	long havok_component_index;
	byte unknown0b8[4];
	long time_bc;
	struct
	{
		word : 8;
		word flag8 : 1;
		word : 3;
		word flag12 : 1;
		word : 3;
	} state;
	byte unknown0c2[0x12c - 0xc2];
	dword crate_flags;
};

struct s_crate_header_view
{
	byte unknown00[8];
	s_crate_object_view *object;
};

struct s_crate_placement_view
{
	long definition_index;
};

struct s_crate_definition_view
{
	byte unknown00[0x38];
	long model_index;
	byte unknown3c[0xbc - 0x3c];
	dword flags_bc;
};

struct s_crate_model_view
{
	byte unknown00[0x24];
	long physics_model_index;
};

struct s_crate_rigid_body_view
{
	byte unknown00[0x1e];
	short motion_type;
	byte unknown20[0x90 - 0x20];
};

struct s_crate_physics_model_view
{
	byte unknown00[0x38];
	long rigid_body_count;
	s_crate_rigid_body_view *rigid_bodies;
};

struct s_crate_model_info_view
{
	byte unknown00[0x48];
	s_crate_physics_model_view *physics_model;
	byte unknown4c[8];
};

/* Keep PR #21's dependency declarations unchanged. Its matrix-result type
   remains opaque here; this callback reads a different field of the result. */
struct s_type_1a7926;
bool function_20a9a0(s_type_1a7926 *matrices, long object_index);
void __stdcall function_1d24a0(s_havok_component *component, real position);

#define CRATE_GET(index) (((s_crate_header_view *)g_4e0300->data)[(index) & 0xffff].object)
#define CRATE_TAG_GET(type, index) ((type *)g_4e3b44[(index) & 0xffff].bytes)

// score 6 (was 77): ours folds the cleared flags into the first set (`mov dword ptr [edi+0x12c], 2`)
// where retail keeps `or dword ptr [edi+0x12c], 2`. Tried: the bit as a dword bitfield sharing the
// word 6; the clear as memset 54 (EDI becomes a field pointer); the crate read at each use 70; a
// flags pointer local 42; the field as long 6.
// Library clear with first result / first model workspace / both 54/54/54; repeated receiver
// lookup 54; receiver acquired after clear 81; genuine body cursor 51;
// complete positive body-classification result 54; exhaustive final result with library clear 88.
// Original body and include restored.
// flags pointer local 42; the field as long 6; shared object getter 6; tag helper inline/forceinline
// 6/6; shared setter inline/forceinline 6/6; definition at function scope 6; a 32-bit flags bitfield 6;
// definition flag bitfield 6; crate re-read at the first set 75.
// @retail 0x11bbf0
bool __stdcall function_11bbf0(long object_index, s_crate_placement_view const *placement, long creation_argument2)
{
	s_crate_object_view *crate = CRATE_GET(object_index);
	dword *crate_flags_ptr = &crate->crate_flags;
	bool result = false;
	*crate_flags_ptr = 0;
	if (placement->definition_index != NONE)
	{
		s_crate_definition_view const *definition =
			CRATE_TAG_GET(s_crate_definition_view, placement->definition_index);
		if (definition->flags_bc & 1)
			crate->crate_flags |= 2;
		if (definition->model_index != NONE)
		{
			s_crate_model_view const *model = CRATE_TAG_GET(s_crate_model_view, definition->model_index);
			if (model->physics_model_index != NONE)
			{
				s_crate_physics_model_view const *physics_model =
					CRATE_TAG_GET(s_crate_physics_model_view, model->physics_model_index);
				bool has_other_motion_type = false;
				for (long i = 0; i < physics_model->rigid_body_count; i++)
				{
					short motion_type = physics_model->rigid_bodies[i].motion_type;
					if (motion_type != 1 && motion_type != 2)
						has_other_motion_type = true;
				}
				if (!has_other_motion_type)
					crate->crate_flags |= 2;
			}
		}
	}
	s_crate_model_info_view info;
	if (function_20a9a0((s_type_1a7926 *)&info, object_index) && info.physics_model->rigid_body_count > 0)
		result = true;
	return result;
}

// @retail 0x11bce0
bool __stdcall function_11bce0(long object_index)
{
	s_crate_object_view *crate = CRATE_GET(object_index);
	bool result = false;
	if (TEST_FIELD_BIT(crate->state.flag12) && crate->havok_component_index != NONE)
	{
		function_1d24a0(havok_component_get(crate->havok_component_index), 1.0f);
		result = true;
	}
	if (!crate->state.flag8)
	{
		crate->time_bc = g_510c54->game_time;
		result = true;
	}
	return result;
}

/* Actual crate type-definition prefix at 0x468180, through update.
   The shared machine/crate callback at +0x4c and later fields are outside
   this view and implementation claim. */
struct s_crate_type_definition_view
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[7];
	bool (__stdcall *create)(long, s_crate_placement_view const *, long);
	void *unknown30[4];
	bool (__stdcall *update)(long);
};

s_crate_type_definition_view g_468180 =
{
	"crate", 'bloc', 0x130, 0x328, 0x330, 0x4c,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	function_11bbf0,
	{ NULL, NULL, NULL, NULL },
	function_11bce0
};
