// @flags /O1 /Oi /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_24ecae.h"
#include "globals.h"
#include "view_context_storage.h"

// score 165 (was 165): retail loads g_510c94 into EDI in the first loop's preheader and re-reads the global itself
// after each movsd copy and after the drawing calls; ours loads the `globals` local into EDX at entry and keeps the
// list count in EBX (retail EDX); retail also keeps a zero register in the second part. Round 158 (m3 lane 14, fast):
// g_510c94 read at each use (no local) 242 (retail's EDI loads and most of the second loop, shifted one
// instruction, but the zero register goes: `and [slot], 0`); the first loop through the global with a block-local in
// the second part 296; the local kept for the first loop only 242; the scaled height without its volatile 205; the
// kind read through `list->items[index].kind` 165.
// @retail 0x24ecae
long __stdcall function_24ecae(s_marker_list const *list, s_24ecae_placement const *placement, real offset)
{
	dword mask = 0;
	long count = 0;
	s_24ecae_globals *globals = (s_24ecae_globals *)g_510c94;
	long index;
	for (index = 0; index < list->count; ++index)
	{
		long kind_index = list->items[index].kind;
		if (globals && kind_index >= 0 && kind_index < globals->image_count)
		{
			s_24ecae_kind const *kind = &globals->images[kind_index];
			if (kind->bitmap_index != NONE && kind->shader_index != NONE &&
				(!placement->hidden || kind->hidden_sequence != NONE) &&
				(!placement->alternate || kind->alternate_sequence != NONE) &&
				kind->sequence_index != NONE)
			{
				mask |= 1UL << index;
				++count;
			}
		}
	}
	if (count != 0 && count <= 2)
	{
		point2f center = placement->position;
		long drawn = 0;
		if (placement->hidden)
			offset = (real)(sqrt(512.0) * placement->scale + offset);
		else
			offset = placement->scale * 16.0f + offset + 1.0f;
		real shift_x = -(placement->forward_i * offset);
		real shift_y = -(placement->forward_j * offset);
		center.x += shift_x;
		center.y += shift_y;
		for (index = 0; index < list->count; ++index)
		{
			if (mask & (1UL << index))
			{
				s_list_item const *item = &list->items[index];
				s_24ecae_kind const *kind = &globals->images[item->kind];
				s_24ecae_bitmap_group const *group = (s_24ecae_bitmap_group *)g_4e3b44[kind->bitmap_index & 0xffff].bytes;
				long sequence_index = placement->hidden ? kind->hidden_sequence :
					placement->alternate ? kind->alternate_sequence : kind->sequence_index;
				if (sequence_index < 0)
					continue;
				long bitmap_index;
				box2f bounds;
				point2f pivot;
				if (sequence_index < group->sequence_count)
				{
					s_24ecae_sequence const *sequence = &group->sequences[sequence_index];
					bitmap_index = sequence->first_bitmap;
					s_24ecae_bitmap const *bitmap = &group->bitmaps[bitmap_index];
					if (sequence->sprite_count > 0)
					{
						bounds = sequence->sprites->bounds;
						pivot.x = (bounds.x0 + bounds.x1) * 0.5f;
						pivot.y = (bounds.y0 + bounds.y1) * 0.5f;
					}
					else
					{
						pivot.x = (real)bitmap->field_10 / bitmap->width;
						pivot.y = (real)bitmap->field_12 / bitmap->height;
						bounds.x0 = 0.0f;
						bounds.x1 = 1.0f;
						bounds.y0 = 0.0f;
						bounds.y1 = 1.0f;
					}
				}
				else if (sequence_index == 0 && group->sequence_count == 0 && group->bitmap_count > 0)
				{
					bitmap_index = 0;
					s_24ecae_bitmap const *bitmap = group->bitmaps;
					pivot.x = (real)bitmap->field_10 / bitmap->width;
					pivot.y = (real)bitmap->field_12 / bitmap->height;
					bounds.x0 = 0.0f;
					bounds.x1 = 1.0f;
					bounds.y0 = 0.0f;
					bounds.y1 = 1.0f;
				}
				else
					continue;
				if (bitmap_index == NONE)
					continue;
				s_24ecae_bitmap const *bitmap = &group->bitmaps[bitmap_index];
				real alpha = 1.0f;
				if (placement->fading && kind->hidden_sequence == NONE)
					alpha = 1.0f - placement->fade;
				point2f origin;
				if (!placement->hidden && count != 1)
				{
					real side = drawn != 0 ? 0.5f : -0.5f;
					origin.x = placement->scale * side * 32.0f + center.x;
					origin.y = center.y;
				}
				else
					origin = center;
				real width = (real)bitmap->width * placement->scale;
				// volatile: retail stores the scaled height to its own stack slot once and reloads
				// it for every corner, while the scaled width stays in a register
				real volatile height = (real)bitmap->height * placement->scale;
				point2f points[4];
				for (long vertex = 0; vertex < 4; ++vertex)
				{
					bool top = vertex == 0 || vertex == 1;
					bool right = vertex == 1 || vertex == 2;
					points[vertex] = origin;
					real dy = (top ? bounds.y0 : bounds.y1) - pivot.y;
					real dx = (right ? bounds.x1 : bounds.x0) - pivot.x;
					points[vertex].x += width * dx;
					points[vertex].y += height * dy;
				}
				g_4e6950.field_0 = *(color3f const *)&item->a;
				g_4e6950.field_c = *(point3f const *)&item->b;
				g_4e6950.value18 = 0.0f;
				g_4e6950.value1c = g_4e69c0[g_4b9ed8] * list->r3c * alpha;
				g_4e6950.value20 = item->r;
				g_4e6950.value3c = 1.0f;
				function_1392c5(item->index);
				function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
					kind->bitmap_index, bitmap_index, kind->shader_index);
				++drawn;
				globals = (s_24ecae_globals *)g_510c94;
			}
		}
	}
	return count;
}
