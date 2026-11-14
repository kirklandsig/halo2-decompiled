// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_3dfe0.h"
#include "unknown_0259d0.h"
#include "object_tag_definitions.h"

extern s_record_pool *g_509434;
extern dword g_4ba034;
extern bool g_467464;
long function_d1850(long object_index, bool value, s_effect_color_query *query);
void function_d4080(long type, s_effect_color_query const *query, long flag, s_lighting_record *record);
void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
real magnitude3d(vector3f const *vector);
real distance3d(point3f const *first, point3f const *second);
struct s_object;
s_object *function_badc0(long object_index, dword mask);
void function_3e5a0(vector3f *current, vector3f const *target, real step);
void function_3e4e0(dword *current, dword const *target, real step);

// score 21 (was 21): the object pointer and force flag still swap stack homes (retail object 0x14 in ebx,
// force 0x10), the object is loaded into eax instead of ebx, and update's false store follows the object's home store.
// Earlier, 21 from 23: the tail's flag local is declared before the type local, so the zero
// store comes before the NONE store as in retail.
// MiniMax G (11-04): no new form, no lower score: the object pointer and force flag keep their stack homes, and the object stays in eax.
// Earlier at 23: the object pointer and force flag swap stack homes (retail object 0x14 in ebx,
// force 0x10), update's false store sits after the force test, and special takes esi for edi.
// update = false before force 218 (homes as retail, sample/update timestamps swap esi/edi);
// that plus update elapsed after the object 186; update before the object 216;
// object, update, update elapsed, force 186; an inline object getter for both lookups 27;
// force/object renamed (reset, stale; entity, body) 27; special declared before type 26 (tail
// order as retail, special still in esi); named sample/update timestamps 27, with update first
// 218; the other entry orders of update elapsed/object/force/update 169-240.
// Round 57: read-only byte object 27; the existing object type and flag field 27;
// object declaration hoisted 27; update declaration hoisted 27; object/update declarations
// hoisted 27; object/update/force declarations hoisted 27; special/type declarations split
// with original assignment order 27; a multi-use clamped sample interval 27. Body restored.
// Round 55: the object lookup written at each use 285 (873 bytes); `bool update = false;` as the first
// statement 227 (object loads straight into ebx as retail, but update leaves the parameter slot and
// the sample/update timestamps swap esi/edi), after the cache lookup 227, before the elapsed return
// 227, after it 216; force as a flag set in an if 55 (830 bytes); `update = force` with
// `if (!force && (object[0xc1] & 1)) return;` 228; force as `!(initialized && valid)` 27 (same code).
// Round 50 (lane 13, fast): entry object/update elapsed/update/force 186, force before update 214; timestamps named or a
// `now` local on that order 186; force byte or char 112; update elapsed below update 171; force one test + assign 82; force
// set in an if 55; update after the clamp 30; `update = force` 228, flag test first 246; update assigned in both arms 221
// (aligned 13; object and force homes as retail, the false store in the else arm); force renamed a_force, z_force 27.
// Lead, BLOCKED-BY 0xd4080's definition line (its file is another lane's this round): retail sets up `lea esi` before the
// push, so 0xd4080's last two parameters swap (flag, then record; 0x3d2c0 stays matched, 0xd4080 364 -> 363): 23, special
// declared first 21, with update assigned in both arms 222 (aligned 7: only the early false store and retail's object
// reload in the force arm remain). All 24 entry orders with either arm on that lead: 21 is the lowest count, aligned 7.
// Round 65 (m3 lane 10, fast): with 0xd4080 reordered to (type, query, flag, record) the call setup is retail's, 23;
// `special` declared before `type` 21 (NOT kept (review): a declaration order picked by score). Left: object/force homes and the early update store.
// Round 121 (fast; 0xd4080's definition is still in another lane's file): on `update = false` before force, sample and update
// timestamps named 218, only sample named 218, only update named 218, the update test's two sides flipped 218 (aligned 34, control 18;
// the swapped esi/edi follow 0xd1850's EDI and 0xd4080's ESI arguments, so the lead above comes first).
// Also round 121 (fast): the stationary test as one `!valid || !(dot < limit)` with the difference above it 120 (831 bytes); the
// lighting record through a named pointer 27, with special declared before type 26.
// Round 49 (m3 lane 2, fast = plain 23): the 0xd4080 lead above is applied (its definition and every declaration take
// (type, query, flag, record), and this call's setup is retail's), so that BLOCKED-BY no longer holds. Update assigned
// in both arms rebuilt: 221, aligned 9 (object/force homes and the object's EBX load as retail; left are the early
// false store, retail's EBX reload in the force arm and the special/type tail order). `if (!force && flag) return;`
// ahead of `if (force) update = true;` 27; the elapsed clamp right after the zero-elapsed return 32; the force test
// inverted with its arms swapped 27, with `update = false` before force as well 218; a 600 s permuter run (44 tries)
// none below 23.
// Round 137: the existing object getter for the initial, parent or both lookups 23 each; early update with the
// immediate clamp 246 (816 bytes), with unsigned timestamp values or field references 218 each, with the initial
// getter 218; the squared-length helper 34. Original body and declarations restored.
// Round 149 (m3 lane 6, fast; aligned counts, masked = registers and stack offsets ignored): `bool update = false;` first in the block leaves
// masked 13: retail's EBX reload in the force arm, object index in ESI with `mov edi, esi` at 0xd1850, and its reload on the skip path; the timestamps then swap
// esi/edi. Tried: `if (!force && flag) return;` ahead of the update test 25 (aligned 16); update first with braced arms 216 (26); the flag test as the first arm 218
// (33); named initialized read 218 (28); `char update` 218 (28); the clamp right after the zero return 246 (816 bytes); inline stale-cache helper 218 (28); sample
// set in an if/else on the NONE test 168 (28); update_time and elapsed as two tests 218 (28); sample declared with the entry locals 253 (836 bytes); update
// declared before force and assigned after it 21 (same code); update between update_elapsed and the object 216 (26); the object looked up again in the force arm
// 156 (27: a home store in the arm, not a reload), on the kept order 226 (813 bytes); separate flag-test and later object pointers 216 / 21; sample as one ternary
// chain 166 (26).
// Y596: Complementary interpolation result, force-arm/direct producers in bool/byte/short/long:
// 24/24/158/66/229/229/141/139. NOT kept: no equality; object/force spill homes and early update store remain; original whole body restored.
// Round 67 (m3 lane 13, fast): the body nested under `if (elapsed)` 21, under `if (cache->valid)` 21, both 21 (same code);
// one `now` read of the game time shared by both elapsed values 21; force renamed stale 21. Diagnostic rerun of update
// before force 218: object in ebx with retail's homes, but sample/update timestamps take edi/esi, the false store follows
// the home store and the force arm has no reload.
// Round 143 (m3 lane 8, fast): `update = false` before force re-measured as the base, 218: its only real differences are
// three allocator choices (no reload in the force arm, the false store after the object's home store, and the two
// timestamps in EDI/ESI for ESI/EDI); every other line is the four-byte shift. On that base: `if (force) update = true;`
// then `if (!force && flag) return;` 218 (threaded to the same code); the object through a named header-entry pointer
// 218; the elapsed clamp right after the zero return 246 (816 bytes); the existing ai object getter 218; force declared
// before update and assigned after it 218. A 600 s permuter run on the kept body (45 tries) found nothing below 21.
// Round 182 (m3 lane 4, fast = plain 21; masked aligned 12: object load register, the object/force homes, the false
// store's place and the reload's place): the cache as an element of the pool (`&((s_3dfe0_cache *)g_509434->data)[index]`,
// the matched 0x3dd10's form) 21 (aligned 12), on the update-first base 218 (aligned 28); `bool const force` 21;
// the distance test's object through a fresh lookup 255 (842 bytes: not merged across the calls); `(void)&object_index;`
// at the top (0x3dd10's idiom) 21, on the update-first base 218. Diagnostic: the update-first base (218) has retail's
// object/force homes and EBX load; left there are the timestamps in EDI/ESI, the object index loaded straight into EDI
// for 0xd1850 (retail ESI then `mov edi, esi`), the false store after the home store and no reload in the force arm.
// Round 114 (m3 lane 9, fast = plain 21; masked aligned 8 on the base, 15 on the update-first form): update-first re-measured 218 (homes
// and EBX load as retail, sample/update timestamps in EDI/ESI); the tail under `if (cache->valid)` in place of the early return 21 (same
// code), with update first 218; the special-flag tests as plain `& 0x2000` / `& 2` tests setting 1 48; the elapsed clamp as a ternary 21
// (same code); `update = update || cache->update_time == NONE || update_elapsed > 1` 205, with update first 121.
// m3 lane 15 r79 (fast with 0x3d2c0 and the callees = full 21; masked = aligned rows with registers and constants
// masked, 8 here, all in the entry block): the object local renamed body/zobject 21 (same code); `bool force;` declared
// before the object and assigned in place 21 (same code); no force local, its test written at both uses 161; the
// object through a named header-entry pointer 21 (same code); a named position pointer (object + 0x30) for its three
// uses 230; the elapsed clamp after the force arm 28. The parent's definition now reads the typed object definition
// (same code). On the update-first diagnostic (218, masked 15: retail's EBX object and homes, but the two timestamps
// take EDI/ESI and the force arm has no reload): renaming update, elapsed or update_elapsed, `type` before `special`
// and the update test's operands swapped all leave the first push EDI (218).
// m3 lane 14 r92 (fast with 0x3d2c0, control 21): the elapsed clamp just before the force flag 172; force as
// `initialized && valid ? false : true` 21 (same code); the update elapsed written at its test with no local 259
// (802 bytes); elapsed renamed sample_elapsed 21; the object as `byte *const` 21.
// @retail 0x3dfe0
void function_3dfe0(long cache_index, long object_index, real priority)
{
    s_3dfe0_cache *cache = (s_3dfe0_cache *)(g_509434->data + (cache_index & 0xffff) * 0x100);
    long elapsed = g_4ba034 - cache->sample_time;
    if (!elapsed) return;
    long update_elapsed = g_4ba034 - cache->update_time;
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    bool force = !cache->initialized || !cache->valid;
    bool update = false;
    if (elapsed < 0) elapsed = 1;
    if (force) update = true;
    else if (object[0xc1] & 1) return;
    bool sample = true;
    if (cache->sample_time != (dword)NONE)
    {
        if (priority > 400.0f) sample = elapsed > 3;
        else if (priority > 200.0f) sample = elapsed > 6;
        else if (priority > 100.0f) sample = elapsed > 9;
        else if (priority > 50.0f) sample = elapsed > 15;
        else if (priority > 10.0f) sample = elapsed > 45;
        else sample = elapsed > 60;
    }
    if (cache->update_time == (dword)NONE || update_elapsed > 1) update = true;
    if (sample)
    {
        bool stationary = false;
        if (cache->valid)
        {
            vector3f delta;
            vector3d_from_points3d(&cache->position, (point3f *)(object + 0x30), &delta);
            stationary = dot3f(&delta, &delta) < 0.002500000176951289f;
        }
        if (!stationary)
        {
            cache->sample_time = g_4ba034;
            if ((byte)function_d1850(object_index, force, &cache->target))
            {
                cache->valid = true;
                cache->position = *(point3f *)(object + 0x30);
            }
        }
    }
    if (!cache->valid) return;
    if (cache->initialized && g_467464 && !update)
    {
        s_effect_color_query *current = &cache->current;
        s_effect_color_query const *target = &cache->target;
        vector3f velocity;
        function_ba1d0(object_index, &velocity, 0);
        real blend = (magnitude3d(&velocity) - 0.15000000596046448f) * 0.03350083902478218f;
        if (blend < 0.0f) blend = 0.0f;
        else if (blend > 1.0f) blend = 1.0f;
        if (blend > 0.0f ||
            (distance3d(&cache->position, (point3f *)(object + 0x30)) > 0.05000000074505806f && blend > 0.0f) ||
            function_badc0(object_index, 0x80))
        {
            real half = blend * 0.5f;
            function_3e5a0((vector3f *)current, (vector3f const *)target, blend * 0.009999999776482582f);
            function_3e4e0(&current->color_b, &target->color_b, half);
            function_3e4e0(&current->color_a, &target->color_a, half);
        }
    }
    else
        cache->current = cache->target;
    long special = 0;
    long type = NONE;
    if (cache->parent != NONE)
    {
        byte *parent = ((s_3dd10_object_header *)g_4e0300->data)[cache->parent & 0xffff].object;
        s_object_definition *definition = (s_object_definition *)g_4e3b44[*(long *)parent & 0xffff].bytes;
        type = definition->object_type;
        if ((bool)(((*(dword *)(parent + 4)) >> 13) & 1)) special = 1;
        if ((bool)((definition->flags >> 1) & 1)) special |= 1;
    }
    function_d4080(type, &cache->current, special, &cache->lighting);
    cache->initialized = true;
}
