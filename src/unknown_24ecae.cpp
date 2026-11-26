// @flags /O1 /Oi /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_24ecae.h"
#include "globals.h"
#include "view_context_storage.h"

// Draws the icons of a marker list's usable kinds beside a placement (only when one or two qualify);
// returns how many qualify.
// @retail 0x24ecae
long __stdcall function_24ecae(s_marker_list const *list, s_24ecae_placement const *placement, real offset)
{
	dword mask = 0;
	long count = 0;
	long index;
	for (index = 0; index < list->count; ++index)
	{
		long kind_index = list->items[index].kind;
		if (g_510c94 && kind_index >= 0 && kind_index < ((s_24ecae_globals *)g_510c94)->image_count)
		{
			s_24ecae_kind const *kind = &((s_24ecae_globals *)g_510c94)->images[kind_index];
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
				s_24ecae_kind const *kind = &((s_24ecae_globals *)g_510c94)->images[list->items[index].kind];
				s_24ecae_bitmap_group const *group = (s_24ecae_bitmap_group *)g_4e3b44[kind->bitmap_index & 0xffff].bytes;
				long sequence_index = placement->hidden ? kind->hidden_sequence :
					placement->alternate ? kind->alternate_sequence : kind->sequence_index;
				long bitmap_index;
				box2f bounds;
				point2f pivot;
				if (sequence_index >= 0 && sequence_index < group->sequence_count)
				{
					s_24ecae_sequence const *sequence = &group->sequences[sequence_index];
					bitmap_index = sequence->first_bitmap;
					s_24ecae_bitmap const *bitmap = &group->bitmaps[bitmap_index];
					if (sequence->sprite_count > 0)
					{
						s_24ecae_sprite const *sprite = sequence->sprites;
						bounds = sprite->bounds;
						pivot.x = (bounds.x0 + bounds.x1) * 0.5f;
						pivot.y = (bounds.y0 + bounds.y1) * 0.5f;
					}
					else
					{
						bounds.x0 = 0.0f;
						bounds.x1 = 1.0f;
						bounds.y0 = 0.0f;
						bounds.y1 = 1.0f;
						pivot.x = (real)bitmap->field_10 / bitmap->width;
						pivot.y = (real)bitmap->field_12 / bitmap->height;
					}
				}
				else if (sequence_index == 0 && group->sequence_count == 0 && group->bitmap_count > 0)
				{
					bitmap_index = 0;
					s_24ecae_bitmap const *bitmap = group->bitmaps;
					bounds.x0 = 0.0f;
					bounds.x1 = 1.0f;
					bounds.y0 = 0.0f;
					bounds.y1 = 1.0f;
					pivot.x = (real)bitmap->field_10 / bitmap->width;
					pivot.y = (real)bitmap->field_12 / bitmap->height;
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
				point2f points[4];
				for (long vertex = 0; vertex < 4; ++vertex)
				{
					bool top = vertex == 0 || vertex == 1;
					bool right = vertex == 1 || vertex == 2;
					points[vertex] = origin;
					real dy;
					if (top)
						dy = bounds.y0 - pivot.y;
					else
						dy = bounds.y1 - pivot.y;
					real dx;
					if (right)
						dx = bounds.x1 - pivot.x;
					else
						dx = bounds.x0 - pivot.x;
					real width = (real)bitmap->width * placement->scale;
					real height = (real)bitmap->height * placement->scale;
					points[vertex].x += width * dx;
					points[vertex].y += height * dy;
				}
				g_4e6950.field_0 = *(color3f const *)&list->items[index].a;
				g_4e6950.field_c = *(point3f const *)&list->items[index].b;
				g_4e6950.value18 = 0.0f;
				g_4e6950.value1c = g_4e6950.user_values[g_4b9ed8] * list->r3c * alpha;
				g_4e6950.value20 = list->items[index].r;
				g_4e6950.value3c = 1.0f;
				function_1392c5(list->items[index].index);
				function_22a664((s_widget_quad_2b11 const *)points, (s_float_rect const *)&bounds,
					kind->bitmap_index, bitmap_index, kind->shader_index);
				++drawn;
			}
		}
	}
	return count;
}
