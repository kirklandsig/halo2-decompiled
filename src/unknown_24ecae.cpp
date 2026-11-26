// @flags /O1 /Oi /Gr /arch:SSE
#include "unknown_11c920.h"
#include "unknown_24ecae.h"
#include "globals.h"
#include "view_context_storage.h"

// score 49 (was 78): combining `sequence_index >= 0 && sequence_index < group->sequence_count` (dropping the separate
// `sequence_index < 0` continue) recovers retail's zero register in EDX for the sequence/bounds tests and `bitmap_index = 0`
// plus retail's exact 1102/1102 B length (78 -> 49, +0x11e); not kept at review (a statement order, match-only): setting `bounds` (0,1,0,1) ahead of `pivot` in both non-sprite
// branches (matching the sprite branch) schedules the bounds stores before `divss` as in retail (49 -> 45, mk 34 -> 17).
// Still differs: the second part's XMM4/XMM5 swap, `lea esi, [eax + 8]` at the sprite bounds copy, and the corner loop's
// counter/temp in ESI/EDI (retail EDI/ESI) with x stored before y is formed (retail computes both then stores x, y).
// M3 GB r23 (85 fast trials across 4 batches; plain confirmed 78 -> 45, 1102/1102 B, +0x11e, 0x1392c5/0x22a664/0x24e59f/0x24f0fc MATCH):
// Kept: combined `sequence_index >= 0 && sequence_index < group->sequence_count` (49); `bounds` before `pivot` (45) reverted at review (order).
// Also tried (not kept): on HEAD: side if/else 78/79, center fields 254, origin fields else 156, nested ge0 / bitmap_index=NONE /
// bitmap_index=seq 78, inline sprite helper 244, corner staged px/py 81 (52 on 49), point ptr 78, oy/ox 78, dx*=width 119,
// offset_point2f 80; on 49/45: bounds between pivots 45, one-use sprite local 47 (43 on 45, matches `lea esi, [eax+8]`),
// `sprites[0].bounds` 45, origin y-first 165 / rev-if 49 / copy-then-adjust 177, shift 0-x 49 / inline 252, alpha ternary/else 145,
// `0.5f *` pivot 49, volatile height before width 55, corner copy-assign 138 / origin before bools 121 / dx before dy 117 /
// y before x add 50 / inline dx/dy 117 / func-scope vertex 49 / point2f corner 49 / set_point2f 52 / add_scaled 55;
// Lead without volatile height: dropping `volatile` on 45 fixes 100% of XMM4/XMM5 (`al=26, mk=5`, 138 at 1089 B, frame 0x58);
// putting non-volatile width/height at top of the corner loop fixes EDI/ESI too (`al=13, mk=5`, 137 at 1089 B, leaving only
// the height spill and `lea esi,[eax+8]`); non-volatile width/height inside loop with y+= before x+= recovers exact `sub esp, 0x5c`
// + EDI/ESI + `movss [ebp-0x1c]` spill + `movaps xmm7` (45 at 1101 B, `al=21, mk=12`, spilling width instead of height), and
// dx before dy + x+= before y+= spills height to `[ebp-0x1c]` (`58`, 1101 B); volatile height inside loop fixes XMM4/XMM5
// and EDI/ESI (`+0x1c2`, 47/45/43) but stores `[ebp-0x1c]` inside the loop.
// Earlier (78, was 165): the hud globals read at each use (no `globals` local, no end-of-item reload), the list items
// read at each use (no item pointer: retail's iterator then points at the item's first color, the kind at -4) and the
// per-user alpha read as g_4e6950.user_values (the same global, so the load follows the two movsd copies as in retail).
// Left: the second part's XMM4/XMM5 swap (the allocator ranks center.y 9 above the side factor 8, retail the other way),
// the corner loop's counter in ESI (retail EDI, with its copy temporary in ESI) and its x sum stored before the y sum
// is formed (retail keeps the x sum in XMM7 until both are done), and retail's zero register in EDX for the sequence
// tests and the `bitmap_index = 0` store, where ours keeps sequence_count in EDX.
// Earlier (165): retail loads g_510c94 into EDI in the first loop's preheader and re-reads the global itself
// after each movsd copy and after the drawing calls; ours loads the `globals` local into EDX at entry and keeps the
// list count in EBX (retail EDX); retail also keeps a zero register in the second part. Round 158 (m3 lane 14, fast):
// g_510c94 read at each use (no local) 242 (retail's EDI loads and most of the second loop, shifted one
// instruction, but the zero register goes: `and [slot], 0`); the first loop through the global with a block-local in
// the second part 296; the local kept for the first loop only 242; the scaled height without its volatile 205; the
// kind read through `list->items[index].kind` 165.
// M3 lane 12 r192 (fast, plain 78): on the 78 form, the side factor written inline in the product 78; the origin sum
// spelled center first 78 (same code); the scaled width and height written inside the corner sums 131 (the counter
// then takes EDI and the copy temporary ESI, as retail, but the product is reassociated); the two scaled sizes as
// named locals inside the corner loop 150 (EDI/ESI as retail, but the height keeps a register: frame 0x58), with
// `dx * width` 150, with the differences written inline 162; the scaled height without its volatile 151; the
// differences inline on the volatile form 163; `scale * (real)width * dx` 131; `dx * ((real)width * scale)` 142.
// Ablations of the 78 form: no item pointer alone 156, with user_values 150; user_values alone 162; the global at
// each use with user_values but the item pointer kept 237.
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
