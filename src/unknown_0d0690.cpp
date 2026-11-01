// @flags /O2 /Ob1 /arch:SSE /Gr
#include "view_context_storage.h"
/* UNKNOWN_0D0690.CPP: object child iteration, object state helpers and the
   render model triangle interpolation (positions, texture coordinates,
   normals and colors at barycentric coordinates) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "globals.h"
#include "unknown_0d0690.h"
#include "geometry_cache.h"

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result);
dword __cdecl pack_color3f(color3f const *color);

struct s_sky_surface_record
{
    vector3f direction;
    dword color0;
    dword color1;
};

/* the tag data of a scenario sky, or NULL when the index or its tag is NONE */
PRIVATE __forceinline byte *sky_surface_data(short index)
{
    byte *scenario = (byte *)g_4e0350;
    long tag_index = NONE;
    if (index >= 0)
    {
        long count = *(long *)(scenario + 8);
        if (index < count)
            tag_index = *(long *)(*(byte **)(scenario + 0xc) + index * 8 + 4);
    }
    byte *result = NULL;
    if (tag_index != NONE)
        result = g_4e3b44[tag_index & 0xffff].bytes;
    return result;
}

real function_30bf0(vector3f *vector);

/* the value limited to [minimum, maximum] */
PRIVATE __forceinline real pinned_real(real const &value, real minimum, real maximum)
{
	if (value < minimum) return minimum;
	if (value > maximum) return maximum;
	return value;
}

// score 30 (was 30): the frame, the sky/count homes, the trig temporaries, the x87 direction adds,
// the blend registers, the color0 store, the in-place strength add and the sete collision test match.
// Left: the i == 0 block loads the strength before the colour copy, so the copy advances ebp (retail
// copies through `lea eax, [ebp + 4]` first), and 0x131c20's `&color` is formed in edi and copied to ecx
// (retail: ecx, then edi). Round 96: a colour output pointer for blending and packing 30 (identical);
// consuming the returned blend pointer 194; colour first with the strength-sum operands reversed 121;
// the shared three-component setter for that copy 163; an entry-colour pointer for copy and blend 168;
// the output pointer for both in-place arguments, and also for the copy, 30 each (identical);
// the entry-colour pointer for the copy alone 168. Original copies and call restored.
// Round 36 (lane 1, fast; aligned = difflib count over the listing): colour copy
// first 121 (828 bytes, aligned 5: only the load-first strength add and the edi/ecx pair differ); colour
// first with the copy field by field 163, `0.0f < power` test 121, add re-read as `((real *)entry)[4]` 121,
// typed colour entry { color4f color; real strength; } 121, typed light record (yaw/pitch/count/colours)
// 121, `strength` declared inside the lights branch 139 or at the top of the function 162, without the
// `index` local / `hit` local / both 121/121/121, `strength` renamed to sort first or last 121/121;
// `if (i != 0) { if (power > 0) ... } else { copy; strength }` 38 (821 bytes: in-place add and retail's
// lea copy, but the i == 0 block lands after the blend block); a 600 s permuter run on the colour-first
// form, best 51 (it moved the strength back first).
// Round 39 (lane 1): compile-only screens reproduce the add's operand order: any non-local load or float
// arithmetic ahead of the strength read in the i == 0 arm gives the load-first add, and 200 combinations of
// declaration order, count/lights locals, entry placement, `0.0f <` test, `strength + power`, `!i`, named
// ratio and direct sky index, plus 63 mixes of `entry` and re-read entries pointers, all stay load-first.
// Real build: light count read once 34; lights base local 39; named blend ratio 30; entry read before the
// entry-count test 37; colour first with the light count local 125.
// Round 15 (m3 lane 2, LTCG env harness = file + depth-4 closure, positional/aligned; base 10/16 = full 30):
// colour copy first 105/5 (828 bytes: the copy is retail's, the strength add loads the entry first into
// xmm0 and copies back); on it two to four extra used locals, `strength = entry + strength`, `if (!i)`,
// a const source cast, a nested else-if, the fifth argument as `&color.red`: identical (105/5); strength
// declared first 108; a named power local in a nested test 84/46; the strength add before the blend
// call 182. On the strength-first body: the first argument as a const pointer or the fifth as
// `&color.red` 10/16 (identical): the ecx/edi pair follows the CSE of `&color`, not the spelling.
// Earlier (52): the frame, the sky/count homes, the trig temporaries, the x87 direction adds,
// the blend registers and the color0 store match. Left: the strength add loads the entry's strength,
// adds and copies back where retail adds it to xmm7 in place (its operands flip only when the strength
// load comes before the color copy in the i == 0 block, which then changes that block), 0x131c20's
// `&color` formed in edi and copied to ecx (retail: ecx, then edi), and retail's sete collision test.
// Round 30 (fast): strength first in the i == 0 block 97 (the add goes in place and the length is
// retail's, but the copy then advances ebp, `add ebp, 4`, since the entry pointer is dead after it;
// with memcpy for the copy also 97); `visible = hit && no object` 52 (folded); one entry-color
// pointer for the copy and the call 161 (821 bytes; it takes esi); the blend written in the call 52.
// Diagnostic: a store through `record` after the call keeps the load-first add, so it is the add's
// operand order, not a value reused across the call. Screened (compile-only): only a flag set in a
// test (`visible = false; if (hit) visible = no object;`) gives retail's sete.
// the frame, the sky/count homes, the trig temporaries, the x87 direction adds,
// the blend registers, the color0 store, the in-place strength add and the sete collision test match.
// Left: the i == 0 block loads the strength before the color copy, so the copy reuses ebp (retail copies
// through `lea eax, [ebp + 4]` first, a byte shorter), and 0x131c20's `&color` is formed in edi and
// copied to ecx (retail: ecx, then edi). The in-place add appears only when the strength read comes first.
// Round 27 (lane 11): the open test as `hit ? no object : false` gives retail's sete (alone 121, 828
// bytes); `hit && no object` or `call && no object` named bools fold it (52/52); with the strength
// assigned first in the i == 0 block 30 (kept). On it: typed light-entry struct 30; strength declared
// before direction 33; color declared before direction 36; blend computed in the call 30; const cast on
// the blend source 30; earlier, a const source pointer local 52 and `(color3f *)&color.red` 52.
// Compile-only, the explicit sum, `((real *)entry)[4]`, late or early strength declaration, field-wise or
// pointer color copy and `+=` in the i == 0 block all keep the load-first add with color first.
// Earlier (52): the strength add loaded the entry's strength, added and copied back where retail adds it
// to xmm7 in place (its operands flip only when the strength load comes before the color copy in the
// i == 0 block, which then changes that block), plus the `&color` registers and the sete.
// Round 38 (lane 9): the contribution through vector3f_assign and the sum through vector3f_add 52
// (kept: the j/k adds load the contribution as retail, since the contribution's fields are written in
// an inlined helper; replaces the hand-written double sums). On it: `strength = strength + power` 52;
// vector3f_add(&contribution, &direction, ...) 52; strength declared first 55; float `+=` and plain
// contribution-first sums 162/162; double sums 56; strength first in the i == 0 block 97 (821 bytes,
// strength add in place, color copy changes); a power pointer 52; `if (i != 0)` with the i == 0 block
// in the else 118 (816 bytes). Compile-only screens, strength order unchanged: direction_from_angles
// with vector3f_add (contribution-first adds, angles loaded once), the three declaration orders, a
// typed entry struct, a color pointer, memcpy, a named power local, `0.0f <` test, entry read late.
// Round 100 (lane 5): j/k sums without the cast 56; no-lights arm first 184; start copied field by
// field 198; `strength = power + strength` 56; strength updated before the blend call 191; strength
// declared before direction/color 59; named power local 171.
// Earlier (93): the frame, the sky/count homes, the trig temporaries, the blend registers and the
// the frame, the sky/count homes, the trig temporaries and the color0 store match.
// Left: the j and k direction adds load the direction where retail loads the contribution (the i add
// matches with the double-typed sum); the entry strength is loaded, added and copied back where retail
// adds it to the strength in place (`addss xmm7, [ebp + 0x10]`); 0x131c20's `&color` is formed in edi
// and copied to ecx (retail ecx, then edi); and retail's collision test materialises a sete before its
// branch. The strength add flips with the i add: every form that gives retail's i add gives the
// loaded strength, and the earlier 93 form had the strength but not the i add.
// Round 100 (on the 56 form): j/k sums without the double cast 56 (same code); the no-lights arm
// first (`count <= 0`) 184 (all three adds as retail, strength still loaded; the arm moves ahead of
// the loop); the zero-vector start copied field by field 198 (all adds as retail, the copy becomes
// SSE moves); `strength = power + strength` 56; strength updated before the blend call 191;
// strength declared first 59; a named power local for the test, divide and add 171.
// The direction adds follow first-assignment order: retail's form needs direction's fields assigned
// before the contribution's. Earlier (93 form and before):
// frame 0x8c and first trig/product sum agree; two x87 add operands, blend address ownership, strength-add destination/copy and collision normalization remain.
// Earlier: the frame, the sky/count homes, the trig temporaries, the blend registers and the
// color0 store match. Left: the three x87 direction adds load the direction first where retail adds
// it as the memory operand, 0x131c20's `&color` is formed in edi and copied to ecx where retail
// forms it in ecx and then edi, and the collision test's sete. The fast check also leaves two of
// retail's global references unresolved in the reduced image, so part of this diff may be noise.
// and the color0 store now match. Left: the three x87 direction adds load the direction first
// (retail adds it as the memory operand), 0x131c20's `&color` is formed in edi and copied to ecx
// (retail ecx, then edi), and the collision test's sete.
// the frame, the sky/count homes, the trig temporaries, the blend registers and the
// color0 store match. Left: the j and k x87 direction adds load the direction where retail loads the
// contribution (the adds follow which field is first assigned earlier in source: assigning direction's
// fields before the loop gives retail's adds), the strength is loaded, added and copied back where
// retail adds the entry's strength to it in place (it flips with the i add), 0x131c20's `&color` is
// formed in edi and copied to ecx where retail forms it in ecx and then edi, and the collision test's
// sete (only a bool set in a test and tested afterwards gives it).
// Round 99 (lane 6), on the double-cast adds: the cast on the other operand, on the direction, or
// `+= (double)` 56 each (identical code); contribution as a real[3] 56; existing direction_from_angles
// for the contribution 167 (the angles are loaded first); `strength = strength + entry` 56;
// `(double)strength + entry` 210.
// Earlier (round 93 form):
// Round 69: `direction.i = contribution.i + direction.i` written out, or contribution declared
// before direction, 93/93 (identical code: the add operands are reordered); the collision test as
// an inline/forceinline bool helper (call && no-object) 93/93 (folded); the per-light count test as
// an early `continue` 93.
// Lane 8 r64 (93 form = count read at each use, lights pointer in the loop, a contribution vector
// with double products cast once, the blend through the returning clamp, color0 first): three scalar
// terms 179/184 (sunk into the adds); terms written into the adds 186; contribution renamed/array/
// declared first, explicit `a = a + b` or `b + a` adds 95; existing direction_from_angles 171;
// aggregate initializer 162; adds in k/j/i order 109; color declared before direction 99 (frame
// slots swap); typed light-entry struct 162; strength spelled `entry + strength` or declared first
// 162/165 (float products); one bool from an `&&` collision test 95; the flag form gives the sete
// exactly (aligned 9) but 825 bytes, 160.
// Earlier at 150: the selector join and esi/edi saves match (the sky lookup helper returns the tag
// data, NULL by default, so the NONE join is not threaded).
// Lane 5 r77 on the kept form: count without volatile 191 (804 bytes); light contribution vector
// with fields read at each use 209 (837 bytes), with a plain count 166, plus the lights pointer
// read in the loop 161, plus the count read from the field at each use 159 (sky/count homes and the
// temporaries' homes then match retail; the adds still load the direction first, 4 bytes long),
// plus color0 stored first 160; direction_from_angles for the contribution 173; pin_real for the
// blend and colors 161 (same code); a pointer add helper and contribution array/initializer/
// declaration-order forms keep the direction-first adds (screened); color0 first alone 149.
// Earlier 213: frame 0x8c agrees; selector/count saves, sentinel join and pending trig/color homes remain.
// Frame 0x8c agrees; selector/count saves, sentinel join and pending trigonometric/color homes remain.
// Earlier 213 (was 213): frame 0x8c agrees; selector/count saves, sentinel join
// and pending trigonometric components differ. Native sky fields 213; native
// light/value fields 213; both views 213; frame pointer 296; status enum 213.
// selector complete validation 213; selector result join 213; collision value initialization 238; direct
// component lifetimes 231; component lifetimes ordinary count 225; native contribution vector 246; size
// preference 380; float consistency 302.
// Selector failing-range returns 213; explicit selector context 213; direct collision predicate 213;
// ordinary while light traversal 213; independent blend bounds 213; constant output first 213.
// Round 116: Direction initialization only with lights 213; color initialization only with lights 213; both 213; light-table field at each access 213; result from record validity 230. Opening code and inherited count qualifier restored.
// Round 122: Query-only collision lifetime 230; validated-record collision lifetime 217; byte status
// 217; strength lifetime inside light branch 211; parallel conditional channel clamps 213.
// Native contribution aggregate with cached/direct angles 236/235;
// native direct component fill 235; cached component orders ikj/jik/jki/kij/kji 219/212/221/216/219.
// Existing scalar color/blend/both 213/213/213; existing color helper and helper plus blend 213/213;
// entry strength snapshot alone/with scalar clamps/with color helper 225/225/225. All restored.
// Round 181 complete data-pointer selector, guard returns, full selector, const data, explicit context and definite pointer arms 213/213/213/213/213/213; ordinary inline 256; validated data output 228 (fast); all restored. Frame 0x8c agrees; selector/count saves and pending trigonometric/color homes remain.
// Native ray/fallback/both component loops with long ordinals 218/217/217; dword 218/217/217;
// both with short/byte ordinals 244/244. Original body restored; selector and pending homes remain.
// Round 246: direction seed/color seed/direction publication library-copy combinations,
// seven at 213; all three plus every whole-color replacement 213. Original copies restored.
// Round 191: Blend/pack/collision ordinary-inline declarations, singly, all pairs
// and together, each 213; all three separate noinline declarations 213.
// Original declarations, count qualification and executable body restored.
// Round 193: Scenario/data/both/value/data+value/scenario+value/all source pointers
// read at each use, then all plus light table: 213/192/192/213/231/213/231/247.
// Data-only plain 192 adds reloads and has a farther aligned sequence; original restored.
// Round 202: Native direction accumulation with long/dword/short/byte ordinals, direction/contribution/paired cursors, contribution-first indexed adds, complete updated-direction publication and the shared vector setter 106/106/192/192/205/195/171/106/182/93 fast.
// Original executable source restored. Frame 0x8c agrees; x87 accumulator operand order, blend-address reuse and collision predicate remain.
// Round 215: native light cursor 224; cursor with const light/value views 224;
// cursor with positive-light strength lifetime, mutable/const views 223/223;
// plain count with cursor, const views, scoped strength or both 233 each.
// Original indexed traversal, strength lifetime and inherited count qualifier restored.
// Round 360: Complete trig factors, existing contribution setter, returning color-result copy,
// all pairs and all three 162/162/179/162/180/180/180; original real-narrowed terms
// in double scalar carriers 186. Original contribution and color ownership restored.
// Round 183: shared vector add, either input owner, and unit add-scaled in place 93/93/93/93; separate native sum output, plain/scaled and either owner, 188/188/188/188. Original accumulation restored.
// @retail 0xd1a30
long __stdcall function_d1a30(long ignore_object_index, point3f const *point, s_sky_surface_record *record)
{
    byte collision[0x5c];
    *(short *)(collision + 0x24) = NONE;
    long result = 2;
    if (g_4b9ee9 && g_4b9eec != NONE)
    {
        long index = g_4b9eec;
        byte *sky = sky_surface_data((short)index);
        if (sky != NULL)
        {
            vector3f direction = *g_4687a4;
            color3f color = *(color3f *)g_468718;
            real strength = 0.0f;
            if (*(long *)(sky + 0x78) > 0)
            {
                for (long i = 0; i < *(long *)(sky + 0x78); ++i)
                {
                    byte *light = *(byte **)(sky + 0x7c) + i * 0x34;
                    if (*(long *)(light + 0x2c) > 0)
                    {
                        byte *entry = *(byte **)(light + 0x30);
                        vector3f contribution;
                        vector3f_assign(&contribution, (real)(cos(*(real *)(light + 0xc)) * cos(*(real *)(light + 0x10))),
                            (real)(sin(*(real *)(light + 0xc)) * cos(*(real *)(light + 0x10))), (real)sin(*(real *)(light + 0x10)));
                        vector3f_add(&direction, &contribution, &direction);
                        if (i == 0)
                        {
                            strength = *(real *)(entry + 0x10);
                            color = *(color3f *)(entry + 4);
                        }
                        else if (*(real *)(entry + 0x10) > 0.0f)
                        {
                            real blend = pinned_real(1.0f - strength / *(real *)(entry + 0x10) / ((real)i + 2.0f),
                                0.0f, 1.0f);
                            function_131c20(&color, (color3f *)(entry + 4), 0, blend, &color);
                            strength += *(real *)(entry + 0x10);
                        }
                    }
                }
                function_30bf0(&direction);
                vector3f ray;
                ray.i = direction.i * 100.0f;
                ray.j = direction.j * 100.0f;
                ray.k = direction.k * 100.0f;
                bool hit = function_1697c0(0x10000001, point, &ray, ignore_object_index, NONE,
                    (s_collision_result_1697c0 *)collision);
                bool open = hit ? *(short *)(collision + 0x5a) == NONE : false;
                if (!open)
                    color = *(color3f *)(sky + 0x34);
            }
            else
            {
                color = *(color3f *)(sky + 0x34);
                direction.i = direction.j = direction.k = -1.0f;
                function_30bf0(&direction);
            }
            record->color0 = 0xff555555;
            record->direction = direction;
            if (color.red < 0.0f) color.red = 0.0f;
            else if (color.red > 1.0f) color.red = 1.0f;
            if (color.green < 0.0f) color.green = 0.0f;
            else if (color.green > 1.0f) color.green = 1.0f;
            if (color.blue < 0.0f) color.blue = 0.0f;
            else if (color.blue > 1.0f) color.blue = 1.0f;
            record->color1 = pack_color3f(&color);
            result = 0;
        }
    }
    return result;
}

color3f *unpack_color3f(dword pixel, color3f *color);

struct s_object_tag
{
	short type;
	word flag0 : 1;
	word flag1 : 1;
	word : 14;
	byte unknown04[0x1d4 - 4];
	real value1d4;
	byte unknown1d8[4];
	real value1dc;
	byte unknown1e0[0x2c0 - 0x1e0];
	long count;
	struct s_object_range *ranges;
};

struct s_object_range
{
	byte unknown00[0xa];
	short maximum_a;
	short maximum_b;
};

struct s_object
{
	long tag_index;
	dword unknown04 : 13;
	dword flag13 : 1;
	dword unknown04a : 12;
	dword flag26 : 1;
	dword unknown04b : 5;
	byte unknown08[4];
	long next_sibling;
	long first_child;
	long value14;
	byte unknown18[0xaa - 0x18];
	byte type;
	byte unknownab[0x15c - 0xab];
	vector3f vector15c;
	vector3f vector168;
	byte unknown174[0x180 - 0x174];
	union
	{
		vector3f vector180;
		struct
		{
			real unknown180;
			real value184;
			real unknown188;
		};
	};
	byte unknown18c[0x1fa - 0x18c];
	byte counter1fa;
	byte counter1fb;
	short value1fc;
	byte unknown1fe[0x212 - 0x1fe];
	char slot212;
	byte unknown213[5];
	long children[1];
	byte unknown21c[0x22a - 0x21c];
	short value22a;
	short value22c;
	byte unknown22e[0x2a0 - 0x22e];
	long value2a0;
	short value2a4;
	byte unknown2a6[2];
	long value2a8;
	long value2ac;
	byte unknown2b0[0x334 - 0x2b0];
	real value334;
	real value338;
	byte unknown33c[2];
	short offset33e;
};

struct s_object_entry
{
	byte unknown00[8];
	long value;
};

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

#define OBJECT_FROM_INDEX(index) (((s_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define TAG_FROM_OBJECT(object) ((s_object_tag *)g_4e3b44[(object)->tag_index & 0xffff].bytes)


/* advances the child iterator to the next child object of type 0 or 1 */
// @retail 0xd0690
bool function_d0690(s_object_child_iterator *iterator)
{
	bool result = false;

	if (iterator->root == iterator->current)
	{
		while (iterator->next != NONE)
		{
			long index = iterator->next;
			s_object *child = OBJECT_FROM_INDEX(index);
			iterator->next = child->next_sibling;
			if ((1 << child->type) & 3)
			{
				result = true;
				iterator->child_index = index;
				iterator->child_short = child->value1fc;
				iterator->child_value = child->value14;
				break;
			}
		}
		if (!result)
		{
			long index = OBJECT_FROM_INDEX(iterator->root)->first_child;
			while (index != NONE)
			{
				s_object *child = OBJECT_FROM_INDEX(index);
				if ((1 << child->type) & 3)
				{
					if (TEST_FIELD_BIT(child->flag26))
						break;
				}
				index = child->next_sibling;
			}
			iterator->current = index;
			if (index != NONE)
				iterator->next = OBJECT_FROM_INDEX(index)->first_child;
		}
	}

	if (iterator->root != iterator->current)
	{
		while (iterator->current != NONE)
		{
			while (iterator->next != NONE)
			{
				long index = iterator->next;
				s_object *child = OBJECT_FROM_INDEX(index);
				iterator->next = child->next_sibling;
				if ((1 << child->type) & 3)
				{
					result = true;
					iterator->child_index = index;
					iterator->child_short = child->value1fc;
					iterator->child_value = child->value14;
					break;
				}
			}
			if (result)
				break;
			long index = OBJECT_FROM_INDEX(iterator->current)->next_sibling;
			while (index != NONE)
			{
				s_object *child = OBJECT_FROM_INDEX(index);
				if ((1 << child->type) & 3)
				{
					if (TEST_FIELD_BIT(child->flag26))
						break;
				}
				index = child->next_sibling;
			}
			iterator->current = index;
			if (index != NONE)
				iterator->next = OBJECT_FROM_INDEX(index)->first_child;
		}
	}

	return result;
}

// @retail 0xd0930
void function_d0930(long object_index, vector3f *vector)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);

	object->vector15c = *vector;
	object->vector180 = *vector;
}

// @retail 0xd0980
void __stdcall function_d0980(long object_index)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);

	object->value2a0 = NONE;
	object->value2a4 = NONE;
	object->value2a8 = NONE;
	object->value2ac = NONE;
}

// @retail 0xd0b70
void function_d0b70(long object_index, long a, long b, real value, long slot)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);
	long child_index = object->children[slot];

	if (child_index != NONE)
	{
		s_object *child = OBJECT_FROM_INDEX(child_index);
		s_object_tag *tag = TAG_FROM_OBJECT(child);

		child->value184 = value;
		if (tag->count > 0)
		{
			s_object_range *range = tag->ranges;

			child->value22c = (short)(a < 0 ? 0 : a > range->maximum_a ? range->maximum_a : a);
			child->value22a = (short)(b < 0 ? 0 : b > range->maximum_b ? range->maximum_b : b);
		}
	}
}

// @retail 0xd0dc0
void function_d0dc0(long object_index, long value)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);
	s_object_entry *entry = (s_object_entry *)((byte *)object + object->offset33e + 0x30);

	if (entry->value == value)
		entry->value = NONE;
}

// @retail 0xd1210
real function_d1210(long object_index)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);

	if (object->value334 > 0.0f)
	{
		s_object_tag *tag = TAG_FROM_OBJECT(object);

		if (object->value338 == 0.0f)
		{
			if (tag->value1d4 > object->value334)
				return object->value334 / tag->value1d4;
			return 1.0f;
		}
		if (object->value334 > object->value338)
			return (object->value334 - object->value338) / tag->value1dc;
	}
	return 0.0f;
}

// @retail 0xd1490
real function_d1490(long object_index, long child_index)
{
	real result = 1.0f;

	if (object_index != NONE && child_index != NONE)
	{
		s_object *object = OBJECT_FROM_INDEX(object_index);
		short slot = object->slot212;
		long current = NONE;
		byte counter;

		if (slot != NONE)
			current = object->children[slot];
		if (current == child_index)
		{
			counter = object->counter1fb;
			if (counter > 0)
			{
				real value = (real)counter * g_510c54->rate * (10.0f / 3.0f);
				if (0.0f > value)
					result = 0.0f;
				else if (value > 1.0f)
					result = 1.0f;
				else
					result = value;
			}
			else
				result = 0.0f;
			return result;
		}
		counter = object->counter1fa;
		if (counter > 0)
		{
			real value = (real)counter * g_510c54->rate * (10.0f / 3.0f);
			if (0.0f > value)
				result = 0.0f;
			else if (value > 1.0f)
				result = 1.0f;
			else
				result = value;
		}
		else
			result = 0.0f;
	}
	return result;
}

/* the callbacks of the unit object type's table at 0x4679c0 that are
   decompiled (entry 17 is function_d0980) */
void *g_4679c0[29] =
{
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, (void *)function_d0980, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0
};

struct s_vertex_block
{
	byte type;
	byte unknown01[3];
	long offset4;
	long offset;
	byte *data;
	byte unknown10[0x10];
};

struct s_triangle
{
	word a, b, c;
};

struct s_mesh
{
	byte unknown00[0x24];
	s_triangle *triangles;
	byte unknown28[0x38 - 0x28];
	long block_count;
	s_vertex_block *blocks;
};

struct s_rgb24
{
	byte r, g, b;
};

struct s_surface_colour_bucket
{
	word unknown00;
	word resource_index;
	byte unknown04[4];
	word *first_vertices;
};

struct s_surface_colour_resource
{
	byte flags;
	byte unknown01[0xc - 1];
	s_geometry_block_info block;
	byte unknown30[4];
	struct s_surface_colour_runtime *runtime;
};

struct s_surface_colour_runtime
{
	long count;
	struct s_surface_colour_data
	{
		byte unknown00[0x20];
		s_vertex_block block;
	} *data;
};

struct s_surface_colour_group
{
	byte unknown00[0x44];
	s_surface_colour_resource *resources;
	byte unknown48[0x54 - 0x48];
	s_surface_colour_bucket *buckets_by_instance;
	byte unknown58[0x64 - 0x58];
	s_surface_colour_bucket *other_buckets;
};

// @retail 0xd35a0
s_vertex_block *function_d35a0(long index, s_surface_colour_group *group, bool instance, long vertex_index)
{
	s_vertex_block *result = NULL;
	s_surface_colour_bucket *bucket;
	word *first_vertex;
	if (instance)
	{
		bucket = &group->buckets_by_instance[index];
		first_vertex = bucket->first_vertices;
	}
	else
	{
		bucket = &group->other_buckets[index];
		first_vertex = &bucket->first_vertices[vertex_index];
	}
	s_surface_colour_resource *resource = &group->resources[bucket->resource_index];
	if ((resource->flags & 2) && function_12de70(&resource->block, 3))
	{
		result = &resource->runtime->data->block;
		long offset;
		if (result->type == 0x2e)
			offset = *first_vertex * 4;
		else
			offset = *first_vertex * 3;
		result->offset4 = offset;
		result->offset = offset;
	}
	return result;
}

struct s_structure_lightmap_triangle
{
	long cluster_index;
	long instance_index;
	byte unknown08[0x18 - 0x08];
	long unknown18;
	byte unknown1c[4];
	long part_index;
	long part_offset;
	long lightmap_part_index;
	byte unknown2c[4];
	real u;
	real v;
};

struct s_surface_geometry_resource
{
	byte unknown00[0x28];
	s_geometry_block_info block;
	byte unknown4c[4];
	s_mesh *sections;
};

struct s_16e290_resource;
long function_16e290(s_16e290_resource *resource);

// @retail 0xd1d70
bool function_d1d70(s_structure_lightmap_triangle const *triangle, s_mesh **out)
{
	bool result = false;
	byte *bsp = (byte *)g_4e0348;
	if (triangle->instance_index != NONE)
	{
		byte *instances = *(byte **)(bsp + 0x144);
		short definition_index = *(short *)(instances + triangle->instance_index * 0x58 + 0x34);
		byte *definitions = *(byte **)(bsp + 0x13c);
		s_surface_geometry_resource *resource = (s_surface_geometry_resource *)(definitions + definition_index * 0xc8);
		if (function_12de70(&resource->block, 3))
		{
			result = true;
			*out = resource->sections;
		}
	}
	else if (*(long const *)triangle->unknown08 == NONE)
	{
		byte *clusters = *(byte **)(bsp + 0xa0);
		s_surface_geometry_resource *resource = (s_surface_geometry_resource *)(clusters + triangle->cluster_index * 0xb0);
		if (function_12de70(&resource->block, 3))
		{
			result = true;
			*out = (s_mesh *)function_16e290((s_16e290_resource *)resource);
		}
	}
	return result;
}

#define MESH_BLOCK_UV_FLOAT 0x18
#define MESH_BLOCK_UV_A 0x1e
#define MESH_BLOCK_UV_B 0x1f
#define MESH_BLOCK_COLOR_A 0x2e
#define MESH_BLOCK_COLOR_B 0x2f
#define MESH_BLOCK_NORMAL 0x30

static inline real dequantize16(short value)
{
	return ((real)value * 2.0f + 1.0f) * (1.0f / 65535.0f);
}

static inline void unpack_point_at(short *p, point2f *point)
{
	point->x = dequantize16(p[0]);
	point->y = dequantize16(p[1]);
}

static inline void unpack_point(byte *data, long index, point2f *point)
{
	short *p = (short *)data + index * 2;

	point->x = dequantize16(p[0]);
	point->y = dequantize16(p[1]);
}

// score 4 (was 4): in the compressed branch retail reads the triangle's b and
// c indices in the other order, so the two unpacked points land in the other
// pair of SSE registers and u and v swap registers.
// @retail 0xd2bf0
void function_d2bf0(s_mesh *mesh, long triangle_index, real u, real v, real *out_a, real *out_b)
{
	long i = 3;
	bool compressed = false;

	if (mesh->block_count > i)
	{
		s_vertex_block *block = mesh->blocks + i;
		s_vertex_block *found;

		do
		{
			found = block;
			if (block->type == MESH_BLOCK_UV_A)
				break;
			if (block->type == MESH_BLOCK_UV_B)
			{
				compressed = true;
				break;
			}
			i++;
			block++;
		}
		while (i < mesh->block_count);

		s_triangle *triangle = mesh->triangles + triangle_index;

		if (!compressed)
		{
			byte *data = found->data;
			point2f *pa = (point2f *)data + triangle->a;
			point2f *pb = (point2f *)data + triangle->b;
			point2f *pc = (point2f *)data + triangle->c;

			real ax = pa->x, bx = pb->x, cx = pc->x;
			*out_a = (cx - ax) * v + (bx - ax) * u + ax;
			real ay = pa->y, by = pb->y, cy = pc->y;
			*out_b = (by - ay) * u + (cy - ay) * v + ay;
		}
		else
		{
			byte *data = found->data;
			point2f a, b, c;

			unpack_point(data, triangle->a, &a);
			unpack_point(data, triangle->b, &b);
			unpack_point(data, triangle->c, &c);

			real ax = a.x, ay = a.y;
			real bx = b.x, by = b.y;
			real cx = c.x, cy = c.y;
			*out_a = (cx - ax) * v + (bx - ax) * u + ax;
			*out_b = (cy - ay) * v + (by - ay) * u + ay;
		}
	}
}

static inline void pin_real(real *value, real minimum, real maximum)
{
    if (minimum > *value)
        *value = minimum;
    else if (*value > maximum)
        *value = maximum;
}

// @retail 0xd2dc0
void function_d2dc0(s_mesh *mesh, long triangle_index, real u, real v, real *out_a, real *out_b)
{
	s_vertex_block *block = mesh->blocks + 1;
	s_triangle *triangle = mesh->triangles + triangle_index;
	byte *data = block->data + block->offset;

	if (block->type == MESH_BLOCK_UV_FLOAT)
	{
		point2f *pa = (point2f *)data + triangle->a;
		point2f *pb = (point2f *)data + triangle->b;
		point2f *pc = (point2f *)data + triangle->c;

		real ax = pa->x, bx = pb->x, cx = pc->x;
		*out_a = (cx - ax) * v + (bx - ax) * u + ax;
		real ay = pa->y, by = pb->y, cy = pc->y;
		*out_b = (by - ay) * u + (cy - ay) * v + ay;
	}
	else
	{
		short *pa = (short *)data + triangle->a * 2;
		short *pb = (short *)data + triangle->b * 2;
		short *pc = (short *)data + triangle->c * 2;
		real ax = (real)pa[0], ay = (real)pa[1];
		real bx = (real)pb[0], by = (real)pb[1];
		real cx = (real)pc[0], cy = (real)pc[1];
		point2f a, b, c;
		a.x = (ax * 2.0f + 1.0f) * (1.0f / 65535.0f);
		a.y = (ay * 2.0f + 1.0f) * (1.0f / 65535.0f);
		b.x = (bx * 2.0f + 1.0f) * (1.0f / 65535.0f);
		b.y = (by * 2.0f + 1.0f) * (1.0f / 65535.0f);
		c.x = (cx * 2.0f + 1.0f) * (1.0f / 65535.0f);
		c.y = (cy * 2.0f + 1.0f) * (1.0f / 65535.0f);

		*out_a = (c.x - a.x) * v + (b.x - a.x) * u + a.x;
		*out_b = (c.y - a.y) * v + (b.y - a.y) * u + a.y;
	}
	*out_a = pinned_real(*out_a, -1000.0f, 1000.0f);
	*out_b = pinned_real(*out_b, -1000.0f, 1000.0f);
}

static inline void unpack_normal(dword value, vector3f *vector)
{
	vector->i = ((real)(long)(value << 21) * (1.0f / 1048576.0f) + 1.0f) * (1.0f / 2047.0f);
	value >>= 11;
	vector->j = ((real)(long)(value << 21) * (1.0f / 1048576.0f) + 1.0f) * (1.0f / 2047.0f);
	value >>= 11;
	vector->k = ((real)(long)(value << 22) * (1.0f / 2097152.0f) + 1.0f) * (1.0f / 1023.0f);
}

// @retail 0xd31c0
bool function_d31c0(s_mesh *mesh, real u, real v, long triangle_index, vector3f *out)
{
	long i = 3;
	bool result = false;

	if (mesh->block_count > i)
	{
		s_vertex_block *block = mesh->blocks + i;
		s_vertex_block *found;

		do
		{
			found = block;
			if (block->type == MESH_BLOCK_NORMAL)
				break;
			i++;
			block++;
		}
		while (i < mesh->block_count);

		s_triangle *triangle = mesh->triangles + triangle_index;
		dword *data = (dword *)found->data;
		vector3f a, b, c;

		unpack_normal(data[triangle->a], &a);
		unpack_normal(data[triangle->b], &b);
		unpack_normal(data[triangle->c], &c);

		out->i = (b.i - a.i) * u + (c.i - a.i) * v + a.i;
		out->j = (b.j - a.j) * u + (c.j - a.j) * v + a.j;
		out->k = (b.k - a.k) * u + (c.k - a.k) * v + a.k;
		result = true;
	}
	return result;
}

// @retail 0xd33a0
bool function_d33a0(s_mesh *mesh, long triangle_index, real u, real v, color3f *out)
{
	long i = 3;
	bool result = false;
	bool compressed = false;

	if (mesh->block_count > i)
	{
		s_vertex_block *block = mesh->blocks + i;
		s_vertex_block *found;
		color3f a, b, c;

		do
		{
			found = block;
			if (block->type == MESH_BLOCK_COLOR_A)
				break;
			if (block->type == MESH_BLOCK_COLOR_B)
			{
				compressed = true;
				break;
			}
			i++;
			block++;
		}
		while (i < mesh->block_count);

		s_triangle *triangle = mesh->triangles + triangle_index;

		if (!compressed)
		{
			dword *data = (dword *)found->data;

			unpack_color3f(data[triangle->a], &a);
			unpack_color3f(data[triangle->b], &b);
			unpack_color3f(data[triangle->c], &c);
		}
		else
		{
			s_rgb24 *data = (s_rgb24 *)found->data;
			s_rgb24 pa = data[triangle->a];
			s_rgb24 pb = data[triangle->b];
			s_rgb24 pc = data[triangle->c];

			a.red = (real)pa.r * (1.0f / 255.0f);
			a.green = (real)pa.g * (1.0f / 255.0f);
			a.blue = (real)pa.b * (1.0f / 255.0f);
			b.red = (real)pb.r * (1.0f / 255.0f);
			b.green = (real)pb.g * (1.0f / 255.0f);
			b.blue = (real)pb.b * (1.0f / 255.0f);
			c.red = (real)pc.r * (1.0f / 255.0f);
			c.green = (real)pc.g * (1.0f / 255.0f);
			c.blue = (real)pc.b * (1.0f / 255.0f);
		}

		out->red = (c.red - a.red) * v + (b.red - a.red) * u + a.red;
		out->green = (c.green - a.green) * v + (b.green - a.green) * u + a.green;
		out->blue = (c.blue - a.blue) * v + (b.blue - a.blue) * u + a.blue;
		result = true;
	}
	return result;
}

// @retail 0xd3630
bool function_d3630(s_vertex_block *block, long triangle_index, s_mesh *mesh, real u, real v, color3f *out)
{
	bool result = false;

	if (block)
	{
		s_triangle *triangle = mesh->triangles + triangle_index;
		byte *data = block->data + block->offset4;
		color3f a, b, c;

		if (block->type == MESH_BLOCK_COLOR_A)
		{
			unpack_color3f(((dword *)data)[triangle->a], &a);
			unpack_color3f(((dword *)data)[triangle->b], &b);
			unpack_color3f(((dword *)data)[triangle->c], &c);
		}
		else
		{
			s_rgb24 *colors = (s_rgb24 *)data;
			s_rgb24 pa = colors[triangle->a];
			s_rgb24 pb = colors[triangle->b];
			s_rgb24 pc = colors[triangle->c];

			a.red = (real)pa.r * (1.0f / 255.0f);
			a.green = (real)pa.g * (1.0f / 255.0f);
			a.blue = (real)pa.b * (1.0f / 255.0f);
			b.red = (real)pb.r * (1.0f / 255.0f);
			b.green = (real)pb.g * (1.0f / 255.0f);
			b.blue = (real)pb.b * (1.0f / 255.0f);
			c.red = (real)pc.r * (1.0f / 255.0f);
			c.green = (real)pc.g * (1.0f / 255.0f);
			c.blue = (real)pc.b * (1.0f / 255.0f);
		}

		out->red = (c.red - a.red) * v + (b.red - a.red) * u + a.red;
		out->green = (c.green - a.green) * v + (b.green - a.green) * u + a.green;
		out->blue = (c.blue - a.blue) * v + (b.blue - a.blue) * u + a.blue;
		result = true;
	}
	return result;
}

// @retail 0xd0620
void function_d0620(s_object_child_iterator *iterator, long object_index)
{
	s_object *object = OBJECT_FROM_INDEX(object_index);
	bool done = false;

	while (!done)
	{
		if (object->value14 != NONE && TEST_FIELD_BIT(object->flag26) && ((1 << OBJECT_FROM_INDEX(object->value14)->type) & 3))
		{
			object_index = object->value14;
			object = OBJECT_FROM_INDEX(object_index);
		}
		else
			done = true;
	}

	iterator->root = object_index;
	iterator->current = object_index;
	iterator->next = object->first_child;
}

struct s_effect_color_query
{
	real unknown00;
	real unknown04;
	real unknown08;
	dword color_a;
	dword color_b;
};

long __stdcall function_d2a50(long flags, long object_index, bool value, s_effect_color_query *query,
    long unused, point3f const *point);
long function_e5670(long unit_index);

bool function_14af40(bool value, point3f const *point, vector3f const *vector, long object_index,
    bool a, void *surface, point3f *hit_point);
long __stdcall function_d1e10(void const *surface, s_effect_color_query *query, long flags, bool value);
/* the collision a surface query starts from, passed by value */
struct s_surface_query_source
{
    void *collision;
};

bool function_14b120(s_surface_query_source source, void *surface);

extern vector3f const g_4406e4 = { 0.0f, 0.0f, -10.0f };
extern vector3f const g_4406f0[5] =
{
    { -10.0f, 0.0f, 0.0f }, { 10.0f, 0.0f, 0.0f },
    { 0.0f, -10.0f, 0.0f }, { 0.0f, 10.0f, 0.0f }, { 0.0f, 0.0f, -40.0f }
};

// @retail 0xd2a50
long __stdcall function_d2a50(long flags, long object_index, bool value, s_effect_color_query *query,
    long supplied_direction, point3f const *point)
{
    long result = 2;
    vector3f const *directions;
    long count;
    if (flags & 1) { directions = g_4406f0; count = 5; }
    else
    {
        directions = (flags & 4) ? (vector3f const *)supplied_direction : &g_4406e4;
        count = 1;
    }
    if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
    {
        byte *bsp = (byte *)g_4e0344->locations;
        if (*(long *)(bsp + 0x1c) != NONE && *(long *)(bsp + 4) == *(long *)((byte *)g_4e0348 + 8))
        {
            vector3f const *direction = directions;
            struct { long i; point3f start, hit_point; byte surface[0x38]; } frame;
            for (frame.i = 0; frame.i < count;)
            {
                if (result != 2) break;
                point3f &start = frame.start;
                point3f &hit_point = frame.hit_point;
                byte *surface = frame.surface;
                start.x = point->x - direction->i * 0.0010000000474974513f;
                start.y = point->y - direction->j * 0.0010000000474974513f;
                start.z = point->z - direction->k * 0.0010000000474974513f;
                if (function_14af40((bool)value, &start, direction, object_index, true, surface, &hit_point))
                    result = function_d1e10(surface, query, flags, value);
                long next = frame.i + 1;
                frame.i = next;
                direction++;
            }
        }
    }
    return result;
}

// @retail 0xd2bb0
long function_d2bb0(void *source, s_effect_color_query *query)
{
    byte surface[0x38];
    s_effect_color_query **query_reference = &query;
    long result = 2;
    s_surface_query_source query_source = { source };
    if (function_14b120(query_source, surface))
        result = function_d1e10(surface, *query_reference, 0, 0);
    return result;
}

PRIVATE __forceinline bool surface_status_failed(long status)
{
    return status == 1 || status == 2 || status == 3;
}

// score 2 (was 2): the retry call computes `flags & ~1` after pushing the object index; retail
// computes it before (`flags ^ 1` keeps the same order, so the operator is not the cause).
// Earlier, with volatile tag pointer and flags locals (44): zero then OR 148; const tag 44;
// byte tag bitfield 135; dword object bitfield 44; named query failure 44;
// explicit final exits 181; inverse failure result 44; retry flag update 44.
// Status predicate as byte 65, short 69, long 68; arm-assigned result 129;
// signed status bounds 130; selected predicate 161; staged result 58;
// ordinary predicate inline 139.
// Now (no volatiles, object bit 13 and the tag's word bit 1 as bitfields, flags = 0 then
// |= 1 twice) 2: retry flags snapshot 2; `flags &= ~1` before the retry 2; retry flags in
// a local 2; dword flags 2; flags declared before the tag pointer 4; point declared first 24;
// failed set after the retry 104.
// Current bitfield forms (fast): nonzero retry 85; exact retry flag 131; word flags 117;
// short flags 117; bool option 115; first failure workspace 120; combined bits 147;
// initial object option 140. XOR 2; subtract 18; clear extracted bit 2; subtract extracted
// bit 133; retry status scope 2; one failure store 98; long failure 83; entry status 2;
// mask assignment in the call 2; nested retry status predicate 2; direct status
// initialization 2; two-call index local 2; read-only point 2; byte failure 2; negated
// return 2; guarded mask assignment 87. Round 44: in-place clear with `failed = failed(status)
// || failed` 93; forceinline query wrapper for both calls 2; failure assigned from the
// predicate then tested 120; sky decision as a per-type bool 62; dword flags parameter in
// the query and its callers 2. Round 55: the query's point parameter second, third, fourth
// or fifth in its list (definition, declarations and calls) 2 each, first 2 (0x17b5d0 2); byte
// flags parameter 2 (0xd2a50 12); its fifth parameter as a direction pointer passed NULL 2; the
// retry test as a bool conversion 2, as `(flags & 1) == 1` 2.
// @retail 0xd1850
long function_d1850(long object_index, bool value, s_effect_color_query *query)
{
    bool failed = false;
    if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
    {
        byte *bsp = (byte *)g_4e0344->locations;
        if (*(long *)(bsp + 0x1c) != NONE && *(long *)(bsp + 4) == *(long *)((byte *)g_4e0348 + 8))
        {
            s_object *object = OBJECT_FROM_INDEX(object_index);
            s_object_tag *definition = TAG_FROM_OBJECT(object);
            long flags = 0;
            if (TEST_FIELD_BIT(object->flag13)) flags |= 1;
            if (TEST_FIELD_BIT(definition->flag1)) flags |= 1;
            point3f *point = (point3f *)((byte *)object + 0x30);
            long status = function_d2a50(flags, object_index, value, query, 0, point);
            if (surface_status_failed(status))
            {
                failed = true;
                if (flags & 1)
                {
                    status = function_d2a50(flags & ~1, object_index, value, query, 0, point);
                    if (surface_status_failed(status)) failed = true;
                }
                if (status != 3)
                {
                    short type = definition->type;
                    if (type == 1)
                    {
                        byte *unit_tag_bytes = g_4e3b44[OBJECT_FROM_INDEX(object_index)->tag_index & 0xffff].bytes;
                        if (!((1 << unit_tag_bytes[0x1f0]) & 0x28)) goto no_sky;
                    }
                    else if (type == 0) { if (function_e5670(object_index) != 2) goto no_sky; }
                    else if (type == 0xc)
                    {
                        byte *unit_tag_bytes = g_4e3b44[OBJECT_FROM_INDEX(object_index)->tag_index & 0xffff].bytes;
                        if (!(bool)((*(dword *)(unit_tag_bytes + 0xd4) >> 4) & 1)) goto no_sky;
                    }
                    else goto no_sky;
                    failed = surface_status_failed(function_d1a30(object_index, point, (s_sky_surface_record *)query));
                no_sky:;
                }
            }
        }
    }
    return failed ? 0 : 1;
}

struct s_lighting_record
{
	color3f color0;
	vector3f direction0;
	real length;
	real value1c;
	color3f color1;
	vector3f direction1;
	color3f color2;
	vector3f direction2;
	dword unknown50;
};

struct s_tag_data
{
	long size;
	byte *address;
};

struct s_lighting_parameters
{
	dword flags;
	real bias;
	color3f lower1;
	color3f upper1;
	real angle1;
	s_tag_data scale1;
	color3f lower2;
	color3f upper2;
	color3f lower_base;
	color3f upper_base;
	real angle2;
	s_tag_data scale2;
	color3f lower0;
	color3f upper0;
	s_tag_data scale0;
	s_tag_data value1c;
};

struct s_lighting_parameter_block
{
	long count;
	s_lighting_parameters *entries;
};

void function_117c80(vector3f const *input, vector3f *output, real angle);
real function_13b390(void const *function, real input, real range);

#define lighting_pin(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

PRIVATE __forceinline void lighting_pin_color(color3f const *input, color3f const *lower, color3f const *upper, color3f *out)
{
	out->red = lighting_pin(input->red, lower->red, upper->red);
	out->green = lighting_pin(input->green, lower->green, upper->green);
	out->blue = lighting_pin(input->blue, lower->blue, upper->blue);
}

PRIVATE __forceinline real lighting_function(s_tag_data const *function, real input)
{
	real value = function_13b390(function, input, 1.0f);
	byte *header = function->address;
	if (!(header[1] & 0xf0))
	{
		real lower = *(real *)(header + 4);
		real upper = *(real *)(header + 8);
		value = lighting_pin(value, 0.0f, 1.0f);
		value = (upper - lower) * value + lower;
	}
	return value;
}

PRIVATE __forceinline void lighting_negate(vector3f *vector)
{
	vector->i = 0.0f - vector->i;
	vector->j = 0.0f - vector->j;
	vector->k = 0.0f - vector->k;
}

PRIVATE __forceinline void lighting_rotate(vector3f const *input, real angle, vector3f *out)
{
	real sine = (real)sin(angle);
	real cosine = (real)cos(angle);
	vector3f axis = *g_4687b0;
	vector3f perpendicular;
	cross3f(input, &axis, &perpendicular);
	real projection = (axis.k * input->k + axis.j * input->j + axis.i * input->i) * (1.0f - cosine);
	out->i = input->i * cosine + axis.i * projection - perpendicular.i * sine;
	out->j = input->j * cosine + axis.j * projection - perpendicular.j * sine;
	out->k = input->k * cosine + axis.k * projection - perpendicular.k * sine;
}

PRIVATE __forceinline real lighting_normalize_ordered(vector3f *v, real square)
{
    real magnitude = (real)sqrt(square);
    if (!(fabs(magnitude) < 0.0001f))
    {
        real inverse = 1.0f / magnitude;
        v->i = inverse * v->i;
        v->j = v->j * inverse;
        v->k = v->k * inverse;
        return magnitude;
    }
    return 0.0f;
}

// score 529 (was 529): Frame 0x2c versus 0x28, selection-result spill and block/query homes remain.
// Round 36 (lane 1, fast, with matched 0x3db00; aligned/shape = difflib counts over the listing, base 527/271):
// `record` stays a stack argument only while our allocator gives it EBP; when it gets ESI this function
// returns `ret 8`, 0xd4080's convention moves and 0x3db00 is lost. Loop without the outer count test 581
// (526/267); plus `bool result = false ... = true` 589 (record in esi, 0x3db00 lost); plus early
// `return false` 564 (frame 0x28); plus `if (parameters) { ... return true; } return false;` 580 (505/262,
// frame 0x28) = base B; B + struct-copy normal after the length 564 (lost); B + lightmap copied into
// `adjusted` with the bias added in place 572 (lost); B + lightmap clamp before base clamp 567 (489/218);
// B + direction0 as `0 - normal` 608; B + both length scales before either is applied 542 (412/257,
// 2189 bytes); B + fade named before the last luminance 586 (440/244); B + clamps + scales + fade 552
// (351/195, 2189 bytes, frame 0x28) = C; C + struct-copy normal / lightmap copy / both 565/592/593 (lost;
// block in eax and query in edx as retail, but record in esi); both + direction0 `0 - normal` 558
// (442/174, frame 0x2c) = D; D + `real bias = parameters->bias` read once (retail loads it once into
// xmm2) 552 (439/156); that + the direction0.k clamp through `pinned_real` 562 (lost); that with
// (block, query, type) declared order 552 (identical); with `dword mask` named before the loop 559
// (mask in eax as retail); with the result variable 554 (lost: block/query take edi/ebx and
// parameters/record ebp/esi, retail's allocation exactly permuted).
// Round 39 (lane 1, fast, 0x3db00 kept unless noted): the incoming register follows the callee-saved home
// (ebx <- eax, edi <- edx), so the convention is decided by which web gets ebx. The existing input_normal
// alias declared at the top 529 (identical); D9 + named mask + result variable 565 (0x3db00 lost); D13 with
// the result declared at the top/after parameters/late, `result = true` first or last, mask named or not:
// 554/565/553/549, 554/565/553/549, 559/557/526/525; on D9, result (top/late/none) x true first/last x
// named mask x length from the copy x struct-copy normal (40 forms): best `bool result = false;` declared
// right before the found test with `result = true;` first in the block 520 (2189 bytes, shape 271 -> 187,
// block eax and query edx as retail, 0xd4080's call setup becomes retail's; it uses D9's one-use fade and
// direction0 negated per component as 0xd4080 writes it), without fade 526, with copy + lighting_negate 598, both 605;
// that 526 form with the base pin first 550, scale2 at its use 529, both 575, `result = true` last 559;
// result at the top with true first: 549/545 (retail's ebp/esi/edi homes, result in bl, 0x3db00 kept).
// Diagnostics: the normal copied before the loop (before or after the count read) gives block eax /
// query edx; one more block read after the loop makes block a stack argument.
// Selected ordinal 587; combined admission mask 570; unsigned scan 529; shared bias local 559
// (caller lost); common blend weight 533.
// frame 0x2c versus 0x28, stored result and block/query registers differ.
// Late success return 577; short result 585; direct primary output 598; early square 579;
// negate normal before copy 546; late return plus parameter order 577; ordered countdown
// traversal 560 (retail length); countdown with tested result 566.
// second scale starts at its application; retail length, extra frame word remains.
// Early lightmap workspace 579 (caller lost); lightmap clamp first 587; primary copy/clamp 598;
// early final fade 529/2196; second scale at use 529/2189 kept.
// Final fade with late second scale 541/2175 (restored).
// Complete RGB sample/lower/both/upper/sample-upper/lower-upper/all captures 634/655/774/664/810/730/913 (upper
// forms lose caller); read-only selected parameters 529. Original helper and body restored.
// Captured normal with existing squared-length helper 508 plain (rejected: caller 442); final selected-pointer result
// 570; captured normal plus final pointer 561; completion-assigned result 572 and with normal 573
// (both lose caller); early color declarations 529; declaration/completion forms 572/573.
// Native normal copy and its name/scope combinations 613 (caller 510; matched caller lost);
// captured-normal role names, entry color declarations and entry length 508 (caller 442);
// inline/noinline declarations 508 (caller 442). Restore original body to preserve caller 364.
// Round 222: native query-vector field 529; field at every use 567; native query copy 578; original-query squared-length helper 529; native-query length before copy 528; all RGB first products 530; all RGB illumination terms 577; normal memcpy 578.
// Negative selection bool/byte/short/long 534/534/567/565; with uniform live block count
// 561/561/531/533. Original selection, count snapshot and every math/helper form restored.
// Incoming lightmap value before normal: native/component/library copy 551/546/551;
// after length: native/component/library copy 556/554/556;
// reused channel scalars 555; native adjusted-value construction 551. All restored.
// Curve helper per-use header/bounds/both 534/545/529; clamp in mapping expression 529;
// with per-use header/bounds/both 534/576/564; header guard return 529. All restored.
// Round 161: Native clamp/rotation/negation results 716/568/611; clamp-rotation/clamp-
// negation/rotation-negation/all 728/808/649/831; all with ordinary clamp inline 831. Negation
// result forms lose the matched caller; original helpers and body restored.
// Paired scalar-output recovery: numeric/vector results 578/578; helper guard 574;
// parallel products 577; early numeric returns/input copy 577/586; entry/root defaults
// 570/573; captured length/default plus capture 577/560; coordinated order/order plus capture 577/577.
// Every paired helper, input observation, signature and call restored.
// Round 165: Positive selection result byte/word/long/dword at original declaration
// 531/561/559/559; entry declarations 531/561/559/559. Original bool result and every
// vector/color/rotation operation restored; frame 0x2c versus 0x28 and block/query/result homes
// remain. Both caller levels preserve their original measurements.
// Round 245: Ordinary normalization/clamp/curve inline availability, each and all
// seven combinations, 529 each; all three plus ordinary negation 529. All restored.
// Round 384: channel-local sample/lower/upper/sample-lower/sample-upper/lower-upper/all
// clamp captures 591/582/569/581/566/562/569; plain clamp branch stores 572.
// All eight lose the matched caller; original complete clamp and target body restored.
// Round 220: Indexed entry-base snapshot 563; repeated flag snapshot 530; both 563;
// default-first with base/flags/both 562/531/562; live count with base/base-flags 564/563.
// Base snapshots recover frame 0x28 but regress; original scan and complete body restored.
// Round 390: conditional last-selection publication with native entry pointer,
// combined admission mask and live-count masks 0-7: 529/565/570/567/567/561/574/570.
// Original scan, positive result, complete math body and all helper/caller contracts restored.
// Round 392: dead first-clamp workspace reused for either later clamp, native/CRT
// normal copies before/after intrinsic length: 567/581/566/588 for each workspace.
// CRT forms lose a matched caller; original distinct color objects and body restored.
// Round 247: current-frame normal/color/both role names, entry declarations and their
// three naming combinations, bounded-color consumption scope with both names: 529 each.
// Original vector/color homes, declaration phases, selection and complete body restored.
// Round 253: Native RGB bias/reflected/scales/bias-reflected/bias-scales/
// reflected-scales/all/all-short component loops 541/537/530/532/578/530/577/572.
// Original selection, all private helpers, complete body and shared color type restored.
// Existing curve mapper scalar-first/persistent-record-reference/both contracts,
// forced or ordinary inline, 529 each; scalar-first or record-reference outlined
// contracts 849 (1903 bytes; caller lost). Original mapper, all calls and body restored.
// Round 257: Mapper lower-independent/upper-independent/upper-exclusive bounds
// 529/551/551; separate bounded input with lower-exclusive/lower-independent/
// upper-independent/upper-exclusive tests 529/529/551/551; existing returning pin 529.
// Original mapper, complete body and every caller contract restored.
// Round 207: reverse ordinal last-applicable search with current-entry receiver,
// complete admission mask and final-pointer result subsets 0-7:
// 568/562/525/563/576/582/576/592. Combined-mask 525 is only four lower, dropped;
// original forward scan, positive result and complete body/helper contracts restored.
// Round 209: Captured-normal squared-length helper with entry-base/flag/both/live-count/base-
// count/flag-count/all/all-readonly forms 554/508/550/544/555/508/554/554. Entry-base combinations
// recover frame 0x28 and first +8, but argument homes remain. All leave the caller 441/442 versus
// 364; plain confirms 508/442, a target gain of 21 against caller regression 78. Original scan,
// input observations, math, helpers, interfaces and complete body restored.
// Complete reused scalar normal captures ijk/ikj/jik/jki/kij/kji:
// 555/555/553/553/553/553; immutable/native component-publication 555/555.
// Frame remains 0x2c; every form puts caller at 442 versus 364. Original
// input observations, native vector, length expression and complete body restored.
// Round 268: shared final color-scale scalar pairs 1-2/1-0/2-0/all, original luminance
// workspace 567 each; split luminance producers 529/529/529/567. Frame stays 0x2c;
// caller 364 and exact caller survive all eight. Original scalar graph and body restored.
// Round 232: Ordered normalizer single-result zero/magnitude defaults, direct/shared
// scale, forced/ordinary inline: 530 each (caller 457/413 versus 364). Original
// returned-length contract, helper, all inputs and full body restored.
// Round 235: uniform published-length field consumers with combined admission/entry-base/
// late-result subsets 0-7: 577/562/569/564/571/571/577/570. All regress both caller
// levels; original local length, selection, helper graph and complete body restored.
// @retail 0xd37f0
bool function_d37f0(s_lighting_parameter_block *block, long type, s_effect_color_query const *query,
	s_lighting_record *record, color3f const *base, color3f const *lightmap)
{
	s_lighting_parameters *parameters = NULL;
	long count = block->count;
	if (count > 0)
	{
		dword mask = 1 << (type + 1);
		for (long i = 0; i < count; i++)
		{
			if ((block->entries[i].flags & mask) || (block->entries[i].flags & 1))
				parameters = &block->entries[i];
		}
	}
	bool result = parameters != NULL;
	if (result)
	{
		vector3f const *input_normal = (vector3f const *)query;
		vector3f normal = { input_normal->i, input_normal->j, input_normal->k };
		real length = (real)sqrt(input_normal->i * input_normal->i + input_normal->j * input_normal->j + input_normal->k * input_normal->k);
		record->length = length;
		lighting_normalize_ordered(&normal, (normal.k * normal.k + normal.j * normal.j) + normal.i * normal.i);
		color3f adjusted;
		adjusted.red = lightmap->red + parameters->bias;
		adjusted.green = lightmap->green + parameters->bias;
		adjusted.blue = lightmap->blue + parameters->bias;
		color3f primary;
		lighting_pin_color(&adjusted, &parameters->lower1, &parameters->upper1, &primary);
		record->color1 = primary;
		function_117c80(&normal, &record->direction1, parameters->angle1 * 0.01745329238474369f);
		lighting_negate(&record->direction1);
		color3f limited_base;
		color3f limited_lightmap;
		lighting_pin_color(base, &parameters->lower_base, &parameters->upper_base, &limited_base);
		lighting_pin_color(&adjusted, &parameters->lower2, &parameters->upper2, &limited_lightmap);
		real square = length * length;
		record->color2.red = ((limited_base.red * limited_lightmap.red) * square) * 2.0f + record->color1.red * (1.0f - square);
		record->color2.green = ((limited_base.green * limited_lightmap.green) * square) * 2.0f + record->color1.green * (1.0f - square);
		record->color2.blue = ((limited_base.blue * limited_lightmap.blue) * square) * 2.0f + record->color1.blue * (1.0f - square);
		record->direction2 = record->direction1;
		lighting_negate(&record->direction2);
		lighting_rotate(&record->direction2, parameters->angle2 * 0.01745329238474369f, &record->direction2);
		lighting_pin_color(&adjusted, &parameters->lower0, &parameters->upper0, &record->color0);
		record->direction0 = normal;
		lighting_negate(&record->direction0);
		if (record->direction0.k > -0.75f)
		{
			record->direction0.k = lighting_pin(record->direction0.k, -1.0f, -0.75f);
			lighting_normalize_ordered(&record->direction0,
			(record->direction0.i * record->direction0.i + record->direction0.j * record->direction0.j) +
			record->direction0.k * record->direction0.k);
		}
		real luminance = 2.0f * (0.11f * lightmap->blue + 0.59f * lightmap->green + 0.3f * lightmap->red);
		record->value1c = lighting_function(&parameters->value1c, lighting_pin(luminance, 0.0f, 1.0f));
		real scale1 = lighting_function(&parameters->scale1, length);
		record->color1.red *= scale1;
		record->color1.green *= scale1;
		record->color1.blue *= scale1;
		real scale2 = lighting_function(&parameters->scale2, length);
		record->color2.red *= scale2;
		record->color2.green *= scale2;
		record->color2.blue *= scale2;
		luminance = 0.114f * adjusted.blue + 0.587f * adjusted.green + 0.299f * adjusted.red;
		real scale0 = (lighting_function(&parameters->scale0, luminance) * (1.0f - (0.1f * length + 0.6f))) * 1.4f;
		record->color0.red *= scale0;
		record->color0.green *= scale0;
		record->color0.blue *= scale0;
	}
	return result;
}

real g_467484 = 15.0f;
real g_467494 = 5.0f;
real function_30bf0(vector3f *vector);

// MiniMax D (11-01): no new form tried, no lower score.
// score 364 (was 364): Frame 0x4c versus 0x40; normal-copy and limited/rotation/color homes remain.
// Round 36 (lane 1, fast, 0x3db00 kept; aligned/shape base 320/203): in-place direction2 rotation 435
// (292/143, frame 0x40, 1870 bytes, first +0x57); with the square right after the direction1 negate 462
// (288/125); in-place with the (k, i, j) squared length 429 (284/143, first +0x74); plus the colour2
// positive checks as stored selects `x > 0 ? x : 0` (retail stores each one) 441 (271/123); plus the early
// square 446 (254/117); plus the shared rotation reading the axis through `g_4687b0` 429 (0xd37f0's
// record becomes a register argument, 0x3db00 lost); (k, i, j) + selects without the in-place
// rotation 434 (314/170), with the early square 448 (282/178).
// The 0xd37f0 call is set up for its current convention (block in EDX, query in EAX; retail block EAX,
// query EDX): BLOCKED-BY 0xd37f0's body, swapping its two parameters moves only the setup order.
// Round 39 (lane 1, fast): the normalize helper through the existing in/out vector3f_scale 364 (identical);
// (k, i, j) squared length 358 (as round 90); that plus the colour2 selects 434; with 0xd37f0 in its
// late-result form (retail's convention) round 36's F6 452 (first difference +0x9c, 1837 bytes, aligned
// 320 -> 219) and F6 without the axis pointer 445; under that convention the block lookup lands in eax
// and the call setup is retail's, leaving the tag table load retail hoists above the NONE test.
// Round 90: normal's squared length spelled k, i, j 358 (x87 runs i, k, j as retail; not kept: gain under 8); turned
// vector rotated in place in direction2 429 (frame 0x40, 1870 bytes); squared length before the
// clamps 458, with in-place rotation 456; 0xd37f0 block/query parameters swapped 436; tag fallback
// as a select 383.
// Frame 0x4c versus 0x40; normal-copy and limited/rotation/color homes remain.
// Earlier 364 (was 364): frame 0x4c versus 0x40; normal-copy and native color/rotation homes remain.
// frame 0x4c versus 0x40; normal/color scheduling and rotation scratch differ.
// Frame 0x4c versus 0x40; normal-copy and limited/rotation/color homes remain.
// frame 0x4c versus 0x40 and normal/color scheduling still differ;
// copied limited color kept; early squared length 428, decoded-color names 429,
// in-place rotation 464, normal-square scalar 429, copy plus early square 453 (fast).
// Base clamped in place 444; outer limited-color lifetime 364; copied color plus
// in-place rotation 435 (retail length); scales at first use 381; shared surface clamp 481;
// shared axis view 515, with in-place rotation 463, with in-place base clamp 542
// (all three axis views lose the matched caller). Original helper and bodies restored.
// rotation workspace still adds twelve frame bytes; intrinsic normal copy 364;
// shared negate then native output 460; scoped turned vector 364; single-result normalizer 380;
// ordinary rotation inline 364.
// Normal consumer before rotation 476; before limited color 508; early primary record 500; direct mobile result 480.
// Complete color result 440; blend complement 451; early normal with complement 479, with complete color result 500.
// Shared dot/length-square/primary RGB scale 364/364/364; secondary/reflected/all RGB scales 452/450/465; dot/all and square/all 465/465.
// Earlier: rotation workspace still adds twelve frame bytes; intrinsic normal copy 364;
// Native rotation input snapshot 490; cosine-first evaluation 364; live axis fields 528 (caller lost);
// input snapshot with cosine order/in-place output/live axis 490/487/487 (live-axis caller lost);
// input snapshot with bounded-color home/radians producer 490/490.
// Native limited/turned/both/normal homes 364/364/364/364;
// limited-normal/turned-normal/all homes 364/364/364; outer rotation workspace 364.
// All original normal, copied color and rotation values and helper boundaries retained.
// Round 152: All rotation-row partial sums/products 364/438; all normalizer products 430 (retail frame/length); normalizer with rotation partials/products 430/449; all RGB blend products 429 (retail frame/length); blend/rotation partials 443; all parallel products 429.
// Independent lower/upper bound records at entry, before fallback and original final phase:
// all eight non-original phase pairs 364; original final-bound lifetimes restored.
// Color clamp orders 102/210/201/012/021 386/377/393/393/387; first color scale orders rbg/grb/gbr 364/363/362.
// Round 158: Genuine length-only primary/secondary scale factors produced independently or jointly at entry,
// after decoding, fallback admission and rotation admission. Scores [544, 525, 543, 515, 503, 485, 482,
// 499]. Original executable body restored.
// Round 184: direct-field normal initializer 473; early normal snapshot/native initializer
// 488/507; native query vector view 364; view with shared snapshot/direct reads/library copy
// 488/364/364; early normal plus direct magnitude reads 517. Original observations restored.
// Round 217: Repeated primary direction/secondary direction/both/reflected color/secondary+color/
// both+color/primary+color/secondary+all-colors views 367/468/468/368/471/471/370/471 fast.
// In-place direction views recover frame 0x40 but regress; original fields, scratch and helpers restored.
// Round 161: Optional magnitude output with void/numeric/vector result 411/411/411; guard 493;
// native clamp/rotation/input graph 555/454/999; all normalizer products 321 (callee 577,
// rejected). Original numeric returns/input copy 469/467; entry/root default 489/471; captured
// length/default plus capture 439/476; coordinated block-query order/order plus capture 455/439.
// All original helper contracts and bodies restored.
// Round 245: Rotation trig/axis/cross/projection schedules ACXSP/AXCPS/AXSCP/CAXPS/
// SAXCP/ASXCP/ACSXP/AXCSP 364/364/430/364/430/430/364/364.
// Cosine after cross with sine first recovers frame 0x40 and length 1870 at 430; all restored.
// Round 385: Plain negate/rotate/normalize component loops and pairs/all,
// long ordinals 422/486/426/505/404/503/485; all with short ordinals 509. Original helpers restored.
// Round 395: decoded-base/query/blend phase with one final pin exit, limited/
// turned scopes and normal declaration priority in all eight combinations: 364 each.
// Original phase graph, native lifetimes, helpers and inherited homes restored.
// Round 253: Plain native RGB loops limited/reflected-blend/scales/limited-reflected/
// limited-scales/reflected-scales/all/all-short 431/436/419/450/419/438/447/660.
// Several recover frame 0x40 but regress; original complete body and color type restored.
// Entry-only uninitialized length/square/three-scale declarations, seven subsets,
// then all with luminance, 364 each. All value producers and complete body restored;
// frame 0x4c and normal/limited-color/rotation homes remain.
// Round 257: Projection-before-cross orders SCAPX/SACPX/CSAPX/CAPSX/CAPXS/ACPSX/
// ACSPX/ASCPX 474/474/474/442/442/442/474/474. The three 442 forms recover frame
// 0x40 and length 1870; all regress. Original helper and whole body restored.
// Round 207: staged cross/complete rotation setter/both 364/364/364; complete
// normalizer setter with neither/cross/rotation/both 430 each (retail frame and length);
// all three with native-value rotation input 490. Original helper/output graph restored.
// @retail 0xd4080
void function_d4080(long type, s_effect_color_query const *query, s_lighting_record *record, long flag)
{
	long const *const flag_reference = &flag;
	long flag_value = *flag_reference;
	(void)flag_value;
	byte *context = (byte *)g_4e0350;
	vector3f const *input_normal = (vector3f const *)query;
	real length = (real)sqrt(input_normal->i * input_normal->i + input_normal->j * input_normal->j + input_normal->k * input_normal->k);
	volatile bool mobile = false;
	if (type == 0 || type == 1 || type == 2)
		mobile = true;
	vector3f normal = *input_normal;
	record->length = lighting_normalize_ordered(&normal, (normal.i * normal.i + normal.k * normal.k) + normal.j * normal.j);
	color3f base, lightmap;
	unpack_color3f(query->color_a, &base);
	unpack_color3f(query->color_b, &lightmap);
	long tag_index = *(long *)(context + 0x33c);
	if (tag_index == NONE)
		tag_index = *(long *)((byte *)g_4e034c + 0x184);
	s_lighting_parameter_block *block = (s_lighting_parameter_block *)g_4e3b44[tag_index & 0xffff].bytes;
	if (!block || !function_d37f0(block, type, query, record, &base, &lightmap))
	{
		record->color1 = lightmap;
		function_117c80(&normal, &record->direction1, g_467484);
		lighting_negate(&record->direction1);
		color3f limited = base;
		limited.red = limited.red < 0.25 ? 0.25 : limited.red > 0.75 ? 0.75 : limited.red;
		limited.green = limited.green < 0.25 ? 0.25 : limited.green > 0.75 ? 0.75 : limited.green;
		limited.blue = limited.blue < 0.25 ? 0.25 : limited.blue > 0.75 ? 0.75 : limited.blue;
		record->color2.red = lighting_pin((lightmap.red * limited.red) * 2.0f, 0.0f, 1.0f);
		record->color2.green = lighting_pin((lightmap.green * limited.green) * 2.0f, 0.0f, 1.0f);
		record->color2.blue = lighting_pin((lightmap.blue * limited.blue) * 2.0f, 0.0f, 1.0f);
		if (!(record->color2.red > 0.0f)) record->color2.red = 0.0f;
		if (!(record->color2.green > 0.0f)) record->color2.green = 0.0f;
		if (!(record->color2.blue > 0.0f)) record->color2.blue = 0.0f;
		real square = length * length;
		record->color2.red = (1.0f - square) * record->color1.red + record->color2.red * square;
		record->color2.green = (1.0f - square) * record->color1.green + record->color2.green * square;
		record->color2.blue = (1.0f - square) * record->color1.blue + record->color2.blue * square;
		vector3f turned = record->direction1;
		lighting_negate(&turned);
		lighting_rotate(&turned, g_467494 * 0.01745329238474369f, &record->direction2);
		record->color0 = lightmap;
		record->direction0.i = 0.0f - normal.i;
		record->direction0.j = 0.0f - normal.j;
		record->direction0.k = 0.0f - normal.k;
		record->direction0.k *= 2.0f;
		function_30bf0(&record->direction0);
		real k = mobile ? 1.18f : 0.75f;
		real luminance = 0.114f * lightmap.blue + 0.587f * lightmap.green + 0.299f * lightmap.red;
		real scale0 = (1.0f - (0.1f * length + 0.6f)) * ((k - 0.7f) * luminance + 0.7f);
		real scale1 = 0.05f * length + 0.4f;
		real scale2 = 0.4f - 0.05f * length;
		record->color0.red *= scale0;
		record->color0.green *= scale0;
		record->color0.blue *= scale0;
		record->color1.red *= scale1;
		record->color1.green *= scale1;
		record->color1.blue *= scale1;
		record->color2.red *= scale2;
		record->color2.green *= scale2;
		record->color2.blue *= scale2;
		record->value1c = 1.0f;
	}
	color3f lower = { 0.0f, 0.0f, 0.0f };
	color3f upper = { 1.0f, 1.0f, 1.0f };
	lighting_pin_color(&record->color1, &lower, &upper, &record->color1);
	lighting_pin_color(&record->color2, &lower, &upper, &record->color2);
	lighting_pin_color(&record->color0, &lower, &upper, &record->color0);
}

#undef lighting_pin

#include "unknown_03bcb0.h"

PRIVATE inline D3DTexture *surface_bitmap_cached(s_bitmap_data *bitmap)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = NULL;
    if (view->last_frame > g_4e6488)
        texture = view->texture;
    return texture;
}

PRIVATE __forceinline D3DTexture *surface_bitmap_texture(s_bitmap_data *bitmap, dword flags, real bias)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = surface_bitmap_cached(bitmap);
    if (!texture)
    {
        _mm_prefetch((char const *)&view->flags, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset1, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset2, _MM_HINT_T0);
        _mm_prefetch((char const *)view->texture, _MM_HINT_T0);
        texture = texture_cache_bitmap_get_texture(bitmap, flags, bias);
        if (!texture)
            texture = function_12ce00(bitmap, flags, bias);
    }
    return texture;
}

// @retail 0xd15e0
D3DTexture *function_d15e0(s_bitmap_data *bitmap, real bias)
{
    return surface_bitmap_texture(bitmap, 0, bias);
}

// @retail 0xd1630
D3DTexture *function_d1630(s_bitmap_data *bitmap)
{
    return surface_bitmap_texture(bitmap, 6, 0.0f);
}

// @retail 0xd1680
D3DTexture *function_d1680(s_bitmap_data *bitmap)
{
    return surface_bitmap_texture(bitmap, 4, 0.0f);
}

struct s_type_7ba8e9;
struct s_bitmap_view;
struct D3DSurface;
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);
D3DTexture *function_12360(s_bitmap_view *bitmap, real bias);
D3DTexture *function_1cfb0(s_bitmap_view *bitmap);
bool __stdcall function_1d2f0(D3DSurface *surface, byte *bitmap);
dword function_1362b0(s_type_7ba8e9 const *bitmap, point2f const *uv, real detail);
dword function_015cd0(long index, dword entry);

// score 4 (was 4): the conversion destination uses ECX rather than EAX; the UV address uses EDX rather than ECX.
// Scoped/entry two-use conversion pointers 4/4; multiuse UV pointer with and without it 4/4;
// direct success return 93; UV/storage names 4/4; getter noinline 4; both/sample noinline 4/4.
// Round 39: bsp pointer local 4; palette ternary at the colour call 18; flag test nested in the
// bitmap block 83; zero-result arm first 68; named first texture 4; inverted 0xd1680 test 19.
// Neither register moves with dead locals added, the flag block, the request arm or the palette
// call removed, or the two functions' definitions swapped: the cause looks outside this body.
// Round 118: the buffer declared in the conversion block 4; a short result 19; the bsp globals
// pointer in a local 4; the bitmap tested before the status 4; the result declared after the
// buffer 4 (a compile-only build keeps the same two registers in every form).
// @retail 0xd2f90
long function_d2f90(long flags, short bitmap_index, long palette_index, real u, real v, color3f *out)
{
    long result = 1;
    byte temporary[0x74];
    if (g_4e0344 && g_4e0344->count > 0)
    {
        long tag = *(long *)((byte *)g_4e0344->locations + 0x1c);
        if (tag != NONE)
        {
            s_bitmap_data *bitmap = (s_bitmap_data *)function_137550(tag, bitmap_index);
            if ((flags & 2) && bitmap && !function_d1680(bitmap))
                function_1cfb0((s_bitmap_view *)bitmap);
            if (bitmap)
            {
                if (!function_d15e0(bitmap, 0.0f))
                {
                    if (!*(void **)((byte *)bitmap + 0x54))
                    {
                        function_12360((s_bitmap_view *)bitmap, 0.0f);
                        result = 1;
                    }
                    else
                        result = 0;
                }
                else if (function_d1680(bitmap))
                    result = 0;
                else
                {
                    function_d1630(bitmap);
                    if (function_1d2f0((D3DSurface *)function_d15e0(bitmap, 0.0f), temporary))
                    {
                        result = 3;
                        bitmap = (s_bitmap_data *)temporary;
                    }
                    else
                        bitmap = NULL;
                }
                if (result != 1 && bitmap)
                {
                    point2f uv = { u, v };
                    dword pixel = function_1362b0((s_type_7ba8e9 const *)bitmap, &uv, 1.0f);
                    if (*(short *)((byte *)bitmap + 0xc) == 0x12)
                        pixel = function_015cd0(palette_index, pixel);
                    unpack_color3f(pixel, out);
                    goto done;
                }
            }
            out->blue = 0.0f;
            out->green = 0.0f;
            out->red = 0.0f;
        }
    }
done:
    return result;
}

// @retail 0xd30d0
bool function_d30d0(long tag, long flags, real u, real v, color3f *out)
{
    bool result = true;
    byte temporary[0x74];
    s_bitmap_data *sample_bitmap = NULL;
    s_bitmap_data *bitmap = NULL;
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        if (definition && *(long *)(definition + 0x44) > 0)
            bitmap = *(s_bitmap_data **)(definition + 0x48);
    }
    if ((flags & 2) && bitmap && !function_d1680(bitmap))
        function_1cfb0((s_bitmap_view *)bitmap);
    if (bitmap)
    {
        if (!function_d15e0(bitmap, 0.0f))
        {
            if (*(void **)((byte *)bitmap + 0x54))
                sample_bitmap = bitmap;
            else
                function_12360((s_bitmap_view *)bitmap, 0.0f);
        }
        else if (function_1d2f0((D3DSurface *)function_d15e0(bitmap, 0.0f), temporary))
            sample_bitmap = (s_bitmap_data *)temporary;
    }
    if (sample_bitmap)
    {
        point2f uv = { u, v };
        unpack_color3f(function_1362b0((s_type_7ba8e9 const *)sample_bitmap, &uv, 1.0f), out);
    }
    else
    {
        out->blue = 0.0f;
        out->green = 0.0f;
        out->red = 0.0f;
        result = false;
    }
    return result;
}

struct s_33a0b_default
{
    dword unknown0;
    vector3f vector;
};
extern s_33a0b_default *g_4686d4;
extern color3f *g_468714;

/* an instance's or cluster's lightmap bitmap (4 bytes) */
struct s_d16d0_bitmap_entry
{
    short bitmap_index;
    char palette_index;
    byte unknown3;
};

// @retail 0xd16d0
bool function_d16d0(s_structure_lightmap_triangle const *triangle, color3f *out, real *weight)
{
    bool result = false;
    /* never assigned: retail hands back whatever this slot holds (the one
       caller ignores it) */
    real sample_weight;
    if (*(long const *)triangle->unknown08 == NONE)
    {
        s_mesh *mesh = NULL;
        if (function_d1d70(triangle, &mesh))
        {
            byte *bsp = (byte *)g_4e0344->locations;
            long index = triangle->instance_index;
            real v = 0.0f, u = 0.0f;
            long palette_index;
            long bitmap_index;
            if (index != NONE)
            {
                s_d16d0_bitmap_entry const *entry = &(*(s_d16d0_bitmap_entry const **)(bsp + 0x4c))[index];
                palette_index = entry->palette_index;
                bitmap_index = entry->bitmap_index;
            }
            else
            {
                s_d16d0_bitmap_entry const *entry = &(*(s_d16d0_bitmap_entry const **)(bsp + 0x2c))[triangle->cluster_index];
                palette_index = entry->palette_index;
                bitmap_index = entry->bitmap_index;
            }

            color3f color = *(color3f *)g_468714;
            function_d2bf0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &u, &v);
            if (function_d2f90(0, bitmap_index, palette_index, u, v, &color) == 0)
            {
                result = true;
                *weight = sample_weight;
                if (0.0f > color.red)
                    color.red = 0.0f;
                else if (color.red > 1.0f)
                    color.red = 1.0f;
                if (0.0f > color.green)
                    color.green = 0.0f;
                else if (color.green > 1.0f)
                    color.green = 1.0f;
                if (0.0f > color.blue)
                    color.blue = 0.0f;
                else if (color.blue > 1.0f)
                    color.blue = 1.0f;
                *out = color;
            }
        }
    }
    return result;
}

struct s_cluster_query;
struct s_structure_collision_result;
bool structure_get_lightmap_triangle(s_structure_collision_result const *query, s_structure_lightmap_triangle *triangle);

// @retail 0xd47d0
bool __stdcall function_d47d0(s_cluster_query const *query, color3f *out)
{
    s_structure_lightmap_triangle triangle;
    *out = *(color3f *)&g_4686d4->vector;
    if (g_4e0344 && structure_get_lightmap_triangle((s_structure_collision_result const *)query, &triangle))
    {
        real weight;
        function_d16d0(&triangle, out, &weight);
    }
    return false;
}

struct s_sample_point
{
    real index;
    real weight[4];
};
struct s_24490_definition;
struct s_13da30_owner;
bool function_0241c0(long key, long block, vector3f *scale_out, long index,
    s_sample_point const *a, s_sample_point const *b, s_sample_point const *c,
    real u, real v, vector3f *result);
void *function_24490(long key, s_24490_definition *definition, long entry_key);
real magnitude3d(vector3f const *vector);
bool function_13d9f0(long type);
long function_13da30(s_13da30_owner const *owner);
extern color3f const *g_468734;
extern color3f *g_46872c;

PRIVATE __forceinline void surface_pin_color(color3f *color)
{
    pin_real(&color->red, 0.0f, 1.0f);
    pin_real(&color->green, 0.0f, 1.0f);
    pin_real(&color->blue, 0.0f, 1.0f);
}

PRIVATE __forceinline void surface_sample_unpack(short const *source, s_sample_point *point)
{
    point->index = (real)source[0];
    point->weight[0] = dequantize16(source[1]);
    point->weight[1] = dequantize16(source[2]);
    point->weight[2] = dequantize16(source[3]);
    point->weight[3] = dequantize16(source[4]);
}

// score 555 (was 555): frame 0x90 agrees; status/mesh/color/sample homes and selective helper expansion remain.
// Earlier: frame 0x90 agrees and the first arm matches retail instruction for instruction
// except its stack homes (lightmap +0x20 versus +0x34, mesh +0x2c versus +0x20, result/part, normal,
// u/v/bsp/material slots: retail packs them jointly with the second arm's colours and normals); the
// direct sample pointers are pushed as built (retail builds a, b, c in order, sharing vertices[0] * 5
// with the unpack arm), the zero constant is not held in ebx for the result/valid stores, and one of
// the two 0x24590 lengths is inlined. per_vertex is assigned in each arm because retail stores its
// false (`mov byte [esp+0x13], 0`) after the two entry reads and threads the second index test; the
// single bool expression stored the false first and re-tested the index (without it the rest scores 788).
// Round 36 (lane 1; aligned/shape = difflib counts over the listing, base 1007/355): triangle u/v/index
// read at each call, as retail and 0xd16d0, 782 (986/289, frame 0x90); plus material lookup before the
// default copies 782 (951/270); plus `if (section_index != NONE) {...}` for the early return 786
// (944/261); plus 0x241c0 called in each sample-format arm 788 (953/205); plus per_vertex assigned in
// both arms of the index/word test (threads retail's second index test) 623; plus palette read before
// bitmap 617 (kept); plus the colours stored before the -1.0 614; plus the zeroed outputs declared
// before bsp 616, both 613, declared base_v, base_u, v, u 612 (kept); plus `if (dynamic_valid) result = 0;
// else result = 1;` 595 (kept). On the 612/595 forms: the direct sample pointers as one-use locals 626
// (aligned 489; not kept); bsp renamed, u/v names swapped: identical; final rescale through
// vector3f_scale 619 (3142 bytes), all three scalings 648 (3081 bytes, a pin block tail-merges); with
// the result branch 614/624; lightmap declared first, second-arm base/normal declared with the dynamic
// outputs 595/595 (identical); `result = 1` before the block lookup, cleared on success 601.
// Round 39 (lane 1, fast): retail calls 0x24590 for both lengths and 0x13da30 in both arms (27 calls);
// ours inlines the second length and the second-arm material lookup (25 calls), so its scaled normals stay
// in registers where retail stores and reloads them. Sample strides as `vertices[k] * 5` on real/short
// pointers 595 (identical); the colour pair cleared as two statements 595; base_valid set inside an `if`
// on the first call 555 (3130 bytes, but aligned 874 -> 1011, the flag held in bl; a flag set in a test);
// the 0.5 fallback as three statements 596; normal declared before base in the second arm 595.
// Earlier (761): Frame 0x98 versus 0x90; status +0x40 versus +0x24, mesh
// +0x24 versus +0x20, sample homes and material/magnitude expansion remain.
// Earlier: Frame 0x90 agrees; result +0x3c versus +0x24, mesh +0x2c versus +0x20, and helper expansion
// remain. Existing scalar clamp output pointer 811; weight loop 819; native sample table 819;
// block-success samples 831; parallel running luminance 818; clamp plus narrow samples 799; with
// Boolean/word/byte capability 790/802/790.
// the long per-vertex flag and the sample points declared at the top of
// the long per-vertex flag and the sample points declared at the top of
// their scope; mesh, UV and color homes and helper expansion still differ (frame 0xa8 versus 0x90).
// Native bitmap word 835; palette byte 852; both 852; UV role names 835; inline depth zero 2009;
// metadata dispatch 847; long per-vertex alone 831; dynamic entry lifetime 835; model sample
// lifetime 825; depth one 835; material noinline 870/866.
// shared cross-path normal 820; shared cross-path base color 825; native scale helper 853;
// texture carrier name 819; first sample lexical position 819.
// Dynamic validity long 863; base validity short 819; accumulated sampled color 828;
// weighted normal loop 892; both lengths before scaling 837; one broad sample scope
// with the other two narrowed (first/second/third) 831/831/831.
// frame 0xa8 versus 0x90; mesh, UV and sample homes remain.
// Only third/second/first sample narrowed 828/828/828 (frame 0x9c);
// byte per-vertex capability 825; function-scope samples 819. Original body retained.
// frame 0xa8 versus 0x90, mesh/sample/UV homes and helper expansion remain.
// Presence-branch samples 831 (frame 0x90); later vertex pointer 853; common base/normal outputs 825;
// direct static material use 859; base-only fallback first 818. Original body retained.
// Native packed input record 819; unpack output first 819; decoded sample value return 945;
// color value contract 1350; native color reference output 819. Frame/mesh/sample/color/UV
// homes and helper expansion remain; original complete body and both helpers restored.
// Native part/index/section records 790/790/790; part/index, part/section, index/section 790/790/790;
// all three 790; all three with result declared first 790. Real metadata layouts canonicalize.
// Decoder output reference 790; decoder output pointer result 823; ordinary decoder/color/both
// 1143/1084/1478; separate noinline decoder/color/both 1140/1597/1958.
// Frame agrees; status/mesh/sample homes and shared external call boundaries remain. All restored.
// Late model texture outputs/sample color/combined color at model entry 790/790/796;
// texture plus sample/texture plus combined/sample plus combined/all three 790/796/796/796;
// all three at function entry 793. Frame/result/mesh and external helper boundaries remain.
// Round 181 material noinline 807; native status/mesh, vector, color, barycentric sample and combined role homes 790/790/790/790/790; single result exit 834; combined homes with material noinline 807 (fast); all restored. Frame 0x90 agrees; result/mesh/sample/UV homes and material/magnitude expansion remain.
// Round 327: status tests 800; phase failure default 796; current group/index/data fields
// 801/811/819; status tests with group/index 802/816; phase default with group and indices 815.
// Round 331: magnitude noinline/inline/force-inline caller declarations, alone and
// with material noinline, plus magnitude noinline/material inline: all seven 790f;
// requested magnitude boundary with all shared vector scales 824f (3069 bytes).
// Caller-local contracts do not resolve the expansion; all declarations and scales restored.
// Cross-path shared mesh/texture coordinates/material/mesh-texture/mesh-material/texture-material/all
// 790/787/790/787/790/787/787; all with result first 787. Shared texture coordinates with
// short/word/signed-byte/byte status 790/790/820/820. Three-point gain rejected; all restored.
// Round 251: separate per-path long/dword/short/word status lifetimes
// 809/809/812/812; the same lifetimes after admission 809/809/812/812. All use
// frame 0x8c; original shared status, frame 0x90 and complete body restored.
// Round 255: native short component counters base/dynamic/output 784/789/782;
// base-dynamic/base-output/dynamic-output/all 784/797/784/804; all byte 804 (fast).
// Only the final rescale counter is short, range 0..3; ordinary 782 at 3133/3130.
// Round 391: Existing color-sum, special default-vector setter and weighted-vector
// scale/add helpers in seven subsets 782/782/782/828/828/828/828;
// all three with chained default colors 827. Frame 0x90 and displaced status/mesh homes
// remain; original body, all loops, sample lifetimes and helper bodies restored.
// Round 395: Nested/failure-first luminance decisions 782/810; dynamic-first
// validity with original/nested/failure-first luminance 782/782/786; base-first
// validity with those decisions 783/783/797. Original native output graph restored.
// Round 177: Native vector setters base/dynamic/output/weighted 784/781/790/782;
// base+dynamic/all scales/weighted+output/all four 795/824/790/824. One-point gain
// rejected; original loops and outputs restored; status/mesh homes remain in frame 0x90.
// Round 268: native existing vertex-buffer, actual model-input record and native three-word
// triangle views in all seven subsets 782 each; all with read-only buffer/data 782.
// Original views restored; frame 0x90, displaced status/mesh homes and external expansion remain.
// Round 253: Direct component-array cursor loops base/dynamic/output/base-dynamic/
// base-output/dynamic-output/all/all with ordered end tests 856/841/785/809/840/819/810/911.
// Original ordinal loops, short final counter, frame and complete body restored.
// Native interpolation pointers with decoded/raw defaults at producer, before-samples,
// dynamic-phase and function declaration scopes: 784/796 in each pair. Original
// two-arm source selection, independent sample objects and complete body restored.
// Guarded pointer values after decoding at producer/before-samples/dynamic-phase/
// function declaration scopes 806 each (3072 bytes). Raw-default diagnostics are
// rejected; original format-dominated address arithmetic and full body restored.
// Round 257: Scalar color-pin lower/upper independent, upper exclusive and lower/upper
// guard returns 782 each; native lower/upper scalar results 828/828; existing returning pin
// 843. Original conditional stores, helper and complete body restored.
// Round 207: structure/model UV, ordinal, both UV, structure-ordinal/model-ordinal/all/entry
// captures 838/790/787/866/773/776/803/807. One-path UV caching is match-only; restored.
// Complete captures with late-structure/early-model/both phases 761/803/761; early-structure
// with original/early model 772/772; readonly late-structure/early-model/both 761/803/761.
// Consistent reused UV and ordinal inputs at the natural structure phase kept, plain 761.
// Round 420: Cross-path shared UV inputs/ordinal/both/mesh/UV-mesh/ordinal-mesh/all/all with status first:
// 807/761/807/760/807/760/807/807. UV sharing grows frame to 0x9c; mesh sharing gains one only. Original
// independent captures, mesh homes and status order restored.
// Round 44: complete default normal/base/lightmap initializations after material, individually/
// paired/all, then all after native input captures 761/761/761/759/759/759/759/759.
// Original default observations and body restored; frame 0x98/0x90 and homes remain.
// Reused magnitude results before RGB construction, after pin/before packing and after
// packed publication, base/dynamic or dynamic/base: 628/628/622/622/622/622;
// base-only/dynamic-only before RGB 614/628. Frames 0x8c versus 0x90; original
// producer phases, calls, complete body and matched caller controls restored.
// Round 473: all three five-value sample element offsets shared across real/short formats; long/dword,
// ascending/reverse declarations and model/entry validity lifetime: 603/821/555/773/603/821/555/773.
// sample_offset names 555; plain 555 kept. Each offset has both format consumers; all sample objects,
// helper bodies, call roles and arithmetic grouping stay fixed.
// Round 231: structure/model/both admission guards 817/618/642; the same guards with
// test-produced shared status 817/798/776; both guards with special-material single
// completion and test-produced/original status 816/797. Original admission and body restored.
// Round 236: Three reused sample-offset storage lifetimes at entry/model/section/dynamic scope,
// native long/dword at each: 555/555/555/555/555/555/555/555. Assignments stayed at their original
// producer; original body restored. Frame agrees; status/mesh/color/sample homes and helper expansion remain.
// Round 278: Native complete default library copies normal/base/lightmap in all seven subsets, then all
// with the complete normal-publication family: 555 each. Original aggregate transfers and entire body
// restored; frame 0x90, displaced result/mesh/UV/color/sample homes and selective helper expansion remain.
// @retail 0xd1e10
long __stdcall function_d1e10(void const *surface, s_effect_color_query *query, long flags, bool value)
{
    s_structure_lightmap_triangle const *triangle = (s_structure_lightmap_triangle const *)surface;
    long result = 2;
    if (*(long const *)triangle->unknown08 == NONE)
    {
        s_mesh *mesh = NULL;
        if (function_d1d70(triangle, &mesh))
        {
            byte *part = *(byte **)((byte *)mesh + 4) + triangle->part_index * 0x48;
            if (function_13d9f0(*(word *)part))
            {
                if ((byte)value)
                {
                    query->unknown00 = 0.0f;
                    query->unknown04 = 0.0f;
                    query->color_a = 0xff404040;
                    query->color_b = 0xff404040;
                    query->unknown08 = -1.0f;
                }
                return 0;
            }
            real base_v = 0.0f, base_u = 0.0f;
            real v = 0.0f, u = 0.0f;
            byte *bsp = (byte *)g_4e0344->locations;
            long index = triangle->instance_index;
            long bitmap_index = NONE;
            long palette_index = NONE;
            bool per_vertex;
            if (index != NONE && *(word *)(*(byte **)((byte *)g_4e0348 + 0x144) + index * 0x58 + 0x56) != 0)
                per_vertex = true;
            else
            {
                byte *entry = index != NONE ? *(byte **)(bsp + 0x4c) + index * 4 :
                    *(byte **)(bsp + 0x2c) + triangle->cluster_index * 4;
                palette_index = (signed char)entry[2];
                bitmap_index = *(short *)entry;
                per_vertex = false;
            }
            long material = function_13da30((s_13da30_owner const *)(
                *(byte **)((byte *)g_4e0348 + 0xa8) + *(short *)(part + 4) * 0x20));
            vector3f normal = *g_4687b0;
            color3f base = *g_468734;
            color3f lightmap = *g_46872c;
            if (!per_vertex)
            {
                function_d2bf0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &u, &v);
                result = function_d2f90(flags, bitmap_index, palette_index, u, v, &lightmap);
            }
            else
            {
                s_vertex_block *block = function_d35a0(index, (s_surface_colour_group *)bsp, true, NONE);
                if (block)
                {
                    function_d3630(block, triangle->lightmap_part_index, mesh, triangle->u, triangle->v, &lightmap);
                    result = 0;
                }
                else
                    result = 1;
            }
            function_d2dc0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &base_u, &base_v);
            if (!function_d30d0(material, flags, base_u, base_v, &base))
                base.red = base.green = base.blue = 0.5f;
            function_d31c0(mesh, triangle->u, triangle->v, triangle->lightmap_part_index, &normal);
            surface_pin_color(&base);
            surface_pin_color(&lightmap);
            query->color_a = pack_color3f(&base);
            query->color_b = pack_color3f(&lightmap);
            *(vector3f *)query = normal;
        }
    }
    else
    {
        byte const *record = (byte const *)surface;
        long section_index = *(long const *)(record + 0x14);
        if (section_index != NONE)
        {
            byte *definition = g_4e3b44[*(long const *)(record + 0xc) & 0xffff].bytes;
            s_mesh *mesh = *(s_mesh **)(*(byte **)(definition + 0x28) + section_index * 0x5c + 0x34);
            word const *vertices = (word const *)((byte *)mesh->triangles + triangle->part_offset * 2);
            bool dynamic_valid = false;
            color3f dynamic_color;
            vector3f dynamic_normal;
            if (*(long *)(definition + 0x74) > 0)
            {
                byte *block = (byte *)function_24490(*(long const *)(record + 8),
                    *(s_24490_definition **)(definition + 0x78), section_index);
                if (block)
                {
                    byte *data = *(byte **)(block + 0xc) + *(long *)(block + 4);
                    // Each format stores five values per sample.
                    long sample_offset_c = vertices[2] * 5;
                    long sample_offset_b = vertices[1] * 5;
                    long sample_offset_a = vertices[0] * 5;
                    if (block[0] != 0x39)
                    {
                        dynamic_valid = function_0241c0(*(long const *)(record + 8), *(long const *)triangle->unknown2c,
                            &dynamic_normal, *(long const *)(record + 0x1c), (s_sample_point *)(data + sample_offset_a * sizeof(real)),
                            (s_sample_point *)(data + sample_offset_b * sizeof(real)), (s_sample_point *)(data + sample_offset_c * sizeof(real)),
                            triangle->u, triangle->v, (vector3f *)&dynamic_color);
                    }
                    else
                    {
                        s_sample_point point_a;
                        s_sample_point point_b;
                        s_sample_point point_c;
                        surface_sample_unpack((short *)(data + sample_offset_a * sizeof(short)), &point_a);
                        surface_sample_unpack((short *)(data + sample_offset_b * sizeof(short)), &point_b);
                        surface_sample_unpack((short *)(data + sample_offset_c * sizeof(short)), &point_c);
                        dynamic_valid = function_0241c0(*(long const *)(record + 8), *(long const *)triangle->unknown2c,
                            &dynamic_normal, *(long const *)(record + 0x1c), &point_a, &point_b, &point_c,
                            triangle->u, triangle->v, (vector3f *)&dynamic_color);
                    }
                }
                if (dynamic_valid)
                    result = 0;
                else
                    result = 1;
            }
            color3f base;
            vector3f normal;
            bool base_valid = function_d33a0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &base)
                && function_d31c0(mesh, triangle->u, triangle->v, triangle->lightmap_part_index, &normal);
            if (dynamic_valid && base_valid)
            {
                real dynamic_weight = dynamic_color.blue * 0.11f + dynamic_color.green * 0.59f + dynamic_color.red * 0.3f;
                real base_weight = base.blue * 0.11f + base.green * 0.59f + base.red * 0.3f;
                if (dynamic_weight > 0.0001f && base_weight > 0.0001f)
                {
                    color3f combined;
                    combined.red = base.red + dynamic_color.red;
                    combined.green = base.green + dynamic_color.green;
                    combined.blue = base.blue + dynamic_color.blue;
                    surface_pin_color(&combined);
                    query->color_b = pack_color3f(&combined);
                    real base_length = magnitude3d(&normal);
                    if (base_length > 0.0001f)
                    {
                        real scale = 1.0f / base_length;
                        for (long component = 0; component < 3; ++component)
                            normal.n[component] *= scale;
                    }
                    real dynamic_length = magnitude3d(&dynamic_normal);
                    if (dynamic_length > 0.0001f)
                    {
                        real scale = 1.0f / dynamic_length;
                        for (long component = 0; component < 3; ++component)
                            dynamic_normal.n[component] *= scale;
                    }
                    vector3f *out = (vector3f *)query;
                    out->i = dynamic_normal.i * dynamic_weight + normal.i * base_weight;
                    out->j = dynamic_normal.j * dynamic_weight + normal.j * base_weight;
                    out->k = dynamic_normal.k * dynamic_weight + normal.k * base_weight;
                    function_30bf0(out);
                    real length = dynamic_length > base_length ? dynamic_length : base_length;
                    for (short component = 0; component < 3; ++component)
                        out->n[component] *= length;
                }
                else if (dynamic_weight > 0.0001f)
                {
                    surface_pin_color(&dynamic_color);
                    query->color_b = pack_color3f(&dynamic_color);
                    *(vector3f *)query = dynamic_normal;
                }
                else
                {
                    surface_pin_color(&base);
                    query->color_b = pack_color3f(&base);
                    *(vector3f *)query = normal;
                }
            }
            else if (dynamic_valid)
            {
                surface_pin_color(&dynamic_color);
                query->color_b = pack_color3f(&dynamic_color);
                *(vector3f *)query = dynamic_normal;
            }
            else if (base_valid)
            {
                surface_pin_color(&base);
                query->color_b = pack_color3f(&base);
                *(vector3f *)query = normal;
            }
            else
            {
                query->color_a = query->color_b = 0;
                *(vector3f *)query = *g_4687b0;
            }
            real u, v;
            function_d2dc0(mesh, triangle->lightmap_part_index, triangle->u, triangle->v, &u, &v);
            byte *part = *(byte **)((byte *)mesh + 4) + triangle->part_index * 0x48;
            long material = function_13da30((s_13da30_owner const *)(
                *(byte **)(definition + 0x64) + *(short *)(part + 4) * 0x20));
            color3f sampled;
            if (function_d30d0(material, 0, u, v, &sampled))
            {
                surface_pin_color(&sampled);
                query->color_a = pack_color3f(&sampled);
            }
            else
                query->color_a = 0xffc0c0d0;
        }
    }
    return result;
}
