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
// m3 lane 14 r97 (fast with 0x3d2c0, 0xd1850 and 0xd4080, control 21): update-first re-measured 228 (homes and EBX load as
// retail, the timestamps in EDI/ESI, the false store at the very top); a 1200 s permuter run from that form (77 tries) best
// 167 (volatile and constant locals); `update` started as `force` after a combined `!force && flag` return 246 (804 bytes);
// force as `!(initialized && valid)` with the update time in a named local used twice 21 (same code); the force arm
// inverted with the flag return nested in its own braces 25; the false store and the object's flag read (a named bool)
// ahead of force 246 (at retail's length).
// m3 lane 14 r99 (fast with 0x3d2c0, 0xd4080 and 0xd1850, control 21), all on the update-first form (`bool update = false;`
// ahead of force, 218: the object's EBX load and homes as retail; left the timestamps in EDI/ESI, the false store after the
// home store and no reload in the force arm): both timestamp NONE tests compared as signed longs 218; force declared bare
// ahead of update and assigned after the false store 218; force as `!(initialized && valid)` 218; the elapsed clamp ahead
// of force 239 (816 B); the object as `byte *const` 218.
// m3 lane 16 r124 (fast with 0x3d2c0 and 0xd1850, control 21; update-first re-measured 218): on update-first, the force arm
// inverted (not-force flag test first) 218; force and update set inside an `initialized && valid` if/else with the clamp in
// both arms 218 (829 B); the hidden-object flag in a named bool 218; `initialized` read into a local for the force test 218.
// On the kept order, `update` declared bare and set false in the not-force arm 222.
// Round 169 (m3 lane 7, fast with 0x3d2c0, control 21): `update` as char 21; `update` declared bare at the top and set
// false after force 21 (same code each); the initialized flag named for the force test and the tail's test 212 (829 B);
// the masked object index named 21 (same code); the update-time test moved to just after the force arm 240. Probe stores
// of 0, 1, -1, 2 on the update-first form (diagnostic, reverted) leave the timestamps in EDI/ESI (194, 834 B).
// Round 192 (m3 lane 5, fast with 0x3d2c0/0x3db00/0xd1850/0x3e4e0, control 21, aligned 12, masked 8, all in the entry block): the
// hidden-object return as its own `if (!force && flag) return;` after `if (force) update = true;` 21 and the rest of the body under
// `if (force || !flag)` 21 (both the base's code); update elapsed computed before the zero-elapsed return 235; the sample decision as
// an inline helper returning each threshold test 312 (753 bytes: the shared setg goes); a stale-cache helper for force 21 (same code);
// the hidden bit read as the word flags' 0x100 bit 21 (same code). On update-first (218, masked 15): inline accessors for the update
// time at its NONE test or for the sample time at its subtraction 218 each (same code), update elapsed before the zero return 192,
// the sample helper 303 (768 bytes), the word-flags bit 218 (same code), the hidden return ahead of the clamp and the force arm 214.
// M3 lane 4 r209 (fast with 0x3d2c0 and 0xd1850, both matched; control 21; update-first re-measured 218): update first
// with the object looked up in each arm of the force test (no lookup ahead of it) 232 (the lookup is not hoisted); the
// kept order with the object looked up in each arm 248 (813 bytes); update first with update elapsed computed at the
// update test 263 (802 bytes); update first with the update-time test moved ahead of the sample decision 228; update
// first with the object bound through a `byte *const &` to its lookup copy 218 (the update-first code).
// Round 137 (m3 lane 9, fast with 0x3d2c0 and 0xd1850, control 21, aligned 12, masked 8; aligned = rows that differ once the two
// listings are aligned): `bool update = false;` right after the zero-elapsed return re-measured 216 (aligned 26, masked 13): the entry
// block is then retail's instruction for instruction (false store ahead of the object's home store, EBX def, both homes); left are the
// timestamps in EDI/ESI, the object index loaded straight into EDI and no reload in the force arm. Update assigned in both arms
// re-measured 222 (aligned 7: the false store's place and the reload). Diagnostic (reverted at once, not keepable): update declared
// volatile on the after-return form 208, aligned 5: everything is retail's except the force arm's `mov ebx, [esp + 0x14]`, so retail's
// update is not an allocation candidate (ours, a candidate live from the entry, is what moves the sample timestamp to EDI and the
// object index off ESI); with the object looked up again in the force arm as well 6 (aligned 6, masked 2: a home store there for
// retail's reload, homes swapped). Tried to get that naturally, all on update after the return: the parameters declared (cache_index,
// priority, object_index) or (priority, cache_index, object_index), definition, declarations and call together, 21 / 21 on the kept
// body and 216 / 216 (same code, 0x3db00 stays matched); update bound to a `bool &` for every later access 216; a pointer to update for
// every later access 216; a one-member aggregate 216 (all three fold back to the after-return code); the force block as an inline
// helper taking `bool *update` or `bool &update` 138 / 138; the update-time test as an inline helper taking `bool &update` 216; update
// initialised from an inline helper returning false 216; force from an inline helper with literal returns 228; update, force and
// sample declared bare at the top in either order 216 / 216; update as long (diagnostic) 200; the update-time test moved right after
// or right before the force block 228 / 228; update initialised from the update-time test at its declaration 262 (802 bytes), after
// force 227; the force block after the update-time test 221; the clamp after the force block 232 (816 bytes).
// M3 lane 2 r99 (fast with 0x3d2c0, 0xd1850 and 0xd4080, control 21; aligned, the kept form differs only in the entry block:
// the object loaded into EAX and stored, then loaded into EBX ahead of the force branch, where retail keeps it in EBX from the
// lookup, stores the false update ahead of the object's home store and reloads EBX in the force arm only): the sample interval
// from a forceinline helper returning the threshold 207 (836 bytes); the object lookup through a local copy of the pool
// pointer 21 (same code), on update-first 218; update-first with `sample` started as the sample-time NONE test and the
// thresholds under `if (!sample)` 168 (sete/test; the timestamps stay EDI/ESI); the hidden-object test through a bool
// scoped to the else arm 21 (same code).
// M3 lane 7 r180 (fast with 0x3d2c0, 0xd1850, 0xd4080, 0x3e4e0; control 21, aligned 12): force as `initialized ? !valid :
// true` 225, with update declared ahead of force 228; update right after the zero-elapsed return re-measured 216 (aligned
// 26: ESI/EDI swapped for both timestamps and the late object index, which retail loads into ESI on both paths into the
// join and copies to EDI for 0xd1850; no force-arm reload), on it one game-time read shared by both elapsed values 216, the
// three late object-index uses through one named copy 216 (same code each). Eight dead locals at the top (diagnostic) 21:
// removed before allocation.
// M3 lane 16 r141 (fast with 0x3d2c0 and 0xd1850, control 21): force as `!initialized` then `if (!valid) force = true;` 76
// (834 B); force as `!(initialized & valid)` 239; the force arm as `update = force` 52; `update |= force;` then
// `if (!force && hidden) return;` 231; the update-time NONE test as a signed long 21 (same code).
// M3 lane 13 r113 (fast with 0xd1850, control 21; update-first re-measured 218: object EBX load, homes and slots as retail,
// left the timestamps in EDI/ESI, the false store after the home store and no force-arm reload): force and update in one
// declaration 21 (same code); the hidden return as `if (!force && flag) return;` with update as one predicate
// `force || update-time NONE || update elapsed > 1` declared at its test 220 (829 bytes); sample started from the
// sample-time NONE test with the thresholds under `if (!sample)` on the kept order 222 (831 bytes); the cache bound as a
// reference 21 (same code); update started from force with the hidden test on update 228.
// M3 lane 4 r220 (fast with 0x3d2c0 and 0xd1850, control 21): the object bound by reference to its lookup copy 21; the
// hidden-object test through a forced-inline accessor 21; the cache pointer const 21; the hidden test on a fresh lookup
// of the object 21 (same code each: folds into the first lookup); the sample decision moved right after the elapsed
// clamp, ahead of the force arm, 217; update-first re-measured 218 (retail's object EBX load and homes; the timestamps in
// EDI/ESI, the false store after the object's home store, no force-arm reload).
// M3 lane 4 r223 (fast with 0x3d2c0 and 0xd1850, control 21): a const position pointer (object + 0x30) for the
// stationary test and the position store in the sample block 195 (831 bytes); the special/type tail in its own block
// 21; the parent index read into a local before its lookup 21; the position store as memcpy 21; the blend clamp as one
// nested ternary 21 (same code each). Re-measured lane 14 r97's combined hidden return with update started from
// force 246 (804 bytes).
// M3 lane 15 r110 (fast with 0x3d2c0, 0xd1850 and 0x3db00, all MATCH; control 21): `(void)&cache_index;` at the top (its
// stack slot is where update lives) 21; `(void)&update;` after its declaration 21; `(void)&priority;` 21 (same code each:
// none takes update out of the allocator); the hidden bit read into a named bool ahead of force 237 (833 B); the object
// read through a const reference to its header entry 21 (same code).
// M3 lane 14 r125 (fast with 0x3d2c0 and 0xd1850, both MATCH; control 21; `bool update = false;` right after the zero-elapsed
// return re-measured 216, aligned 26: entry block as retail, the sample/update timestamps in EDI/ESI, the late object index
// in EDI, no force-arm reload): a redundant `update = false;` at the top of the not-force arm 222 (aligned 7), the same on
// the update-first form 222; on update-first, the hidden test through a fresh lookup expression (no second named pointer)
// 216 (same code), the force arm inverted as `if (!force) { hidden return } else update = true;` 216 (aligned 31), the
// sample decision as `if (sample time == NONE) sample = true; else { thresholds }` 166 (aligned 26: positional only).
// M3 lane 6 r206 (fast with 0x3d2c0 and 0xd1850, both matched; control 21, update right after the zero return re-measured
// 216): the kept order with a redundant `update = false;` added at the top of the not-force arm 222 (object straight into
// EBX, retail's homes and ESI/EDI timestamps, but the initialiser is then dead: the false store moves into that arm and the
// force arm has no reload); that with update after the zero return as well 222; the arms inverted (`if (!force) { update =
// false; if (hidden) return; } else update = true;`) 223; `bool update;` declared bare ahead of update elapsed and set in
// both arms 222; declared bare right before the force test, set in both arms 222.
// M3 lane 3 r277 (fast with 0x3d2c0, 0xd1850, 0xd4080, 0x3e4e0 and 0x3db00; control 21): the object typed through the
// existing object header and object views (include/unknown_0bad50.h, the hidden test on the word flags' 0x100 bit) 21
// (same code); the update-time test only while update is still false 201 (836 bytes); the not-force arm first with the
// hidden return in its own braces 25; the force arm as `update = force` 52; the zero return and the negative clamp
// nested under one `elapsed <= 0` test 46.
// M3 lane 13 r128 (screened, control 21): force as `long` 111 (839 B); the hidden test as `(bool)(object[0xc1] & 1)` 21
// (same code); the clamp ahead of `bool update = false;` with the not-force arm first (hidden return in braces, `else update
// = true;`) 28; `bool update = false, force = ...;` 218 (the update-first code); update as `byte` 21 (same code); re-measured
// update between update elapsed and the object 216.
// M3 lane 15 r116 (fast with 0xd1850 and 0x3db00, both MATCH; control 21, aligned 16): diagnostic, 0 to 7 dead initialised
// longs on `bool update = false;` right after the zero-elapsed return 216 each (the timestamps' ESI/EDI swap is not a
// symbol-count tie); the tail's update test through a `bool const &` binding 21, on update-first 216; force through a
// `bool const &` for its test 21 and for the 0xd1850 argument 21; the hidden test's object through a `byte *const &` 21, on
// update-first 216 (same code each); update assigned in both arms re-measured 222 (aligned: only the false store's place in
// the not-force arm and the missing force-arm reload differ); that with the object looked up again in the force arm 226 (813 B).
// M3 lane 15 r122 (fast with 0xd1850 and 0x3d2c0, both MATCH; control 21; update-first re-measured 218, where only the
// timestamps' ESI/EDI, the false store after the object's home store and the missing force-arm reload differ): on
// update-first, both timestamps named and declared update time first 219; the hidden return in its own braces under
// the else 218; the object through a named header-entry pointer 218 (same code each); update elapsed computed after the
// force flag 232. On the kept order, the two named timestamps declared update time first 35.
// M3 lane 3 r285 (h2m fast with 0x3d2c0 and 0xd1850, both MATCH; control 21, update-first re-measured 216): a redundant
// `update = false;` just ahead of `if (force)` 24 (the false store then follows the object's reload), the same on the
// update-first form 24; on update-first, `bool const force` 216 and the rest of the body nested under `if (elapsed)` 216
// (both the update-first code), the sample decision ahead of the force block 221.
// M3 lane 8 r206 (h2m fast with 0x3db00 and 0xd1850, both MATCH; control 21, aligned 12): the elapsed clamp between the
// force flag and `bool update = false;` 24 (aligned 12: the false store moves below the clamp); `bool update = false;` and
// the clamp ahead of force 239 (816 B); force computed ahead of the object lookup 241 (830 B); `bool update = false;`
// between the object lookup and force 218 (the update-first code: entry homes and EBX as retail, but ESI/EDI swapped for
// the two timestamps and the late object index, no force-arm reload); the hidden test through its own lookup with the
// object local declared after the force block 250 (838 B: the lookup is not merged).
// M3 lane 8 r208 (h2m fast with 0x3d2c0, 0x3db00, 0xd1850 and 0x3e4e0, all MATCH; control 21; `bool update = false;` just
// above force re-measured 218: entry, object EBX load and homes as retail, then the two timestamps in EDI/ESI, the false
// store after the object's home store and no force-arm reload): on that update-first form, the sample time read through a
// const reference to its field 218, the update time so 218, both 218, the sample time copied and bound by a const
// reference 218, force bound by a const reference for its test and the 0xd1850 argument 218 (same code each: none moves
// the timestamps' ESI/EDI tie).
// M3 lane 3 r288 (h2m fast with 0x3d2c0 and 0xd1850, both MATCH; control 21; update-first re-measured 218): force started
// false and set in an `if (!initialized || !valid)` with update first 237, with update right after the zero-elapsed return
// 235; the valid flag read once into a named local for the force test and the stationary test 168, that with update first
// 230, with update right after the zero-elapsed return 230. In the kept form the object's reload into EBX sits ahead of the
// force branch (both paths); retail reloads only in the force arm and keeps EBX from the lookup on the other path.
// M3 lane 1 r145 (h2m fast with 0xd1850, MATCH; control 21): the update-time test moved after the sample block 118; no
// update-time statement, its condition folded into the tail's `!update` test 217 (829 bytes); `bool update = force;`
// with one `if (!force && hidden) return;` 228; the hidden return first as `initialized && valid && hidden`, ahead of
// force, 228 (816 bytes); the hidden return deferred past the sample decision 74 (834 bytes). A compile-only build does
// not reproduce this function (87 aligned rows off).
// M3 lane 8 r216 (h2m fast with 0x3db00 and 0xd1850, both MATCH; control 21): force written out at its two uses (no
// named flag) 161 (first difference +0x21), the same on the update-first order 161; the object looked up right after the
// cache, ahead of the elapsed time, 124 (830 B); update elapsed computed between the object lookup and force 215;
// `bool update = false;` as the function's first statement 228, right after the cache lookup 228.
// M3 lane 16 r169 (h2m fast with 0xd1850 and 0x3d2c0, both MATCH; control 21): update typed `long` 65 (837 B); force as
// `!(cache->initialized && cache->valid)` 21 (same code); the hidden test in an else block through a `byte const *` copy of
// the object 21 (same code); the hidden bit read into a named bool next to `bool update = false;` 157 (829 B); the object's
// position named once as a `point3f *` for its three uses 245 (814 B).
// M3 lane 16 r171 (h2m fast; control 21; `bool update = false;` just above force re-measured 218): on that update-first
// form the hidden test on a fresh lookup 218 (the lookup merges; on the kept order 21, already listed); the object lookup
// ahead of update elapsed, then update and force (object, update elapsed, update, force) 186; on the kept order `bool update
// = force;` after the clamp with `if (!update && hidden) return;` 231; force renamed `stale` 21 and the object local renamed
// `body` 21 (name-order probes for the swapped homes: same code each).
// M3 lane 1 r152 (h2m fast with 0xd1850, MATCH; control 21): force started true and cleared in an `initialized && valid`
// arm holding the hidden return, update false ahead of it and set in the else arm, 240 (816 B); the object as `byte const *`
// 21 (same code); the update-time test as `update |= ...` 211 (842 B); force replaced by `current = initialized && valid`
// with `!current` at the force test and the 0xd1850 argument 106 (833 B); update started as the stale predicate and force
// as its copy, with one `if (!force && hidden) return;` 222.
// M3 lane 5 r230 (h2m fast with 0xd1850, MATCH; control 21; update-first re-measured 218, update right after the
// zero-elapsed return 216): the velocity vector declared at the top of the body 21, on update-first 218; the stationary
// test's delta declared at the top 21, on update-first 218; on the after-return form, update's two true stores through a
// byte cast of its address 216 and its false store so 216 (the address is seen through: update stays a candidate); on
// update-first, `bool sample;` declared right under update 218. Retail keeps the object in EBX from its lookup, stores
// its home after the false store and reloads it only in the force arm; the two homes are force 0x10, object 0x14.
// M3 lane 6 r228 (h2m fast with 0xd1850, MATCH; control 21): force started true and cleared under `initialized && valid`
// with no hidden return inside 49 (830 B); the object read by raw header arithmetic 21 (same code); the update-time test
// right after the hidden return 240; update declared ahead of force with the not-force arm first in braces (hidden
// return) and `else update = true;` 218; force ahead of the object lookup with update elapsed after the clamp 236.
// M3 lane 11 r154 (h2m fast with 0xd1850, MATCH; control 21, update-first 218, update right after the zero return 216):
// redundant paren pairs (1-3 around the blend products, or 1-3 around the two elapsed subtractions and the object index
// mask) on all three forms 21 / 218 / 216 each (the integer ties ignore the paren phase); a folded-looking `elapsed > 0`
// guard (not folded) on the threshold chain, the force block, the update-time test, or both of the first two: kept order
// 223 / 234 / 200 / 242, update-first 205 / 207 / 205 / 212; the cache lookup through a file-local inline or forceinline
// getter 21 / 21, a game-tick getter at the three reads 21, both 21, the same on update-first 218 each (same code); the
// stationary test as a file-local inline or forceinline helper (flag form or early-return form) 195 (831 B), on
// update-first 202; update-first with the force arm storing `update = force` 163.
// M3 lane 3 r298 (h2m fast with 0xd1850, MATCH; control 21): the elapsed clamp moved inside the sample-time NONE test
// (its only reader) 31; that with `bool update = false;` ahead of force 226; the update-time test moved into the
// not-force arm after the hidden return 242 (first +0x0); the blend clamp through the existing PIN_126660 macro 196 (882 B:
// the product is evaluated per use); a redundant `else update = false;` after the hidden return 222.
// M3 lane 12 r178 (h2m fast with 0xd1850 and 0x3d2c0, both MATCH; control 21, update right after the zero-elapsed return
// re-measured 216: its entry block is retail's instruction for instruction, left are the two timestamps and the late
// object index in EDI/ESI for ESI/EDI and the missing force-arm reload). Reading of retail's force arm: the object has
// a stack home and sits in EBX from its lookup to the hidden test; the force arm, which never reads it, loads EBX from
// that home again for the uses after the join, so its register range is cut in that arm. Tried for that reload, all on
// the after-return order unless noted: the stationary test's position through a fresh lookup 216, on the kept order 21
// (both merge with the first lookup: same code); a second pointer declared after the force block from a fresh lookup
// for the three later uses 216, on the kept order 21 (same code each); the object assigned from a fresh lookup in the
// force arm ahead of `update = true` 155 (the arm then stores EBX to the object's home where retail loads from it,
// the homes swap back to object 0x10 / force 0x14), after `update = true` 156; a second pointer set in both arms, from
// a fresh lookup in the force arm and from the object after the hidden return 211, from a fresh lookup in both 211,
// from the object in the force arm and a fresh lookup after the hidden return 211 (the object then takes ESI), that on
// the kept order 235; `if (!force && hidden) return;` then `if (force) update = true;` 216, the two statements the
// other way round 216, the hidden return nested under `if (!force)` then `if (force) update = true;` 216 (no reload).
// M3 lane 6 r233 (h2m fast with 0xd1850 and 0x3d2c0, both MATCH; control 21): `bool const force` 21; the object as a
// `byte *const` 21 (same code each); the update-time subtraction written inside its test (no update-elapsed local) 259
// (802 B: the subtraction moves to the test), the same with `bool update = false;` right after the zero-elapsed return
// 263; that update-first order with the object `byte *const` 216 (the update-first code).
// M3 lane 16 r182 (h2m fast with 0xd1850 and 0x3d2c0, both MATCH; control 21): the update-time test as two ifs (NONE, then
// `else if (update elapsed > 1)`) 21; the hidden test through a fresh element read of the header array 21; the header array
// as a named `s_3dd10_object_header *` local 21; the sample time named (`dword`) for the elapsed time and the NONE test 21
// (same code each); `else update = false;` as a third arm after the hidden return 222. Re-measured: update ahead of force
// 218, the not-force arm first in braces 25, the object looked up ahead of update elapsed 215.
// M3 lane 4 r262 (h2m fast with 0xd1850 and 0x3d2c0, both MATCH; control 21). Frame packer reading: the object's
// home counts four references before allocation (its store, one reload in each arm of the force test, the late reload)
// and the force flag's three, so the object's slot is made first and sits lowest; retail's three final references to
// the object's home (store, force-arm reload, late reload) would rank it below the force flag. A forced-ranking search over 102
// single decisions on the object's candidates (pieces, priorities, splits) finds nothing below 21, so the EAX head piece
// is decided before allocation. Forms: force typed `byte` 106 (833 B); force written with `== false` tests 21 (same
// code); force started true and cleared under `if (initialized && valid)` 49 (830 B); the hidden test through a `(bool)`
// cast 21 (same code); the force arm as `update = force;` 52.
// M3 lane 4 r263 (h2m fast with 0x3d2c0, 0xd1850 and 0x3e4e0; control 21): `update` typed `long` 65 (837 B); the
// 0xd1850 call's flag argument written as the two cache tests (the force local then used only by the update test) 237
// (845 B); the elapsed clamp moved inside the sample block (its only reader) 31, with `update` declared ahead of force
// 226; `update |= force;` with `if (!force && hidden) return;` 231. The frame packer puts the object (four spill
// references) in the lowest slot and the force flag (three) above it; retail has them the other way round, and forcing
// the object's first piece into EBX alone measures 22.
// M3 lane 15 r150 (h2m fast with 0xd1850 and 0x3d2c0, both MATCH; control 21). Allocator reading of the update-first
// form (216 here): the update time's candidate rises from 15 to 20 (its first block then references twelve candidates
// against eleven), so it is coloured before the 17-point candidate that takes EBX and picks ESI; on the kept order it is
// coloured after it, sees ESI scored 2000 and takes EDI, as retail. Forms: update first with the elapsed clamp after the
// force arm 232 (816 B); the clamp and the update-time difference both after the force arm 242; update declared ahead
// of force 218 (the update-first code); the hidden return tested ahead of force on `initialized && valid` with update
// started from force 224 (813 B); that with update first and `if (force) update = true;` 120 (first +0x8).
// M3 lane 6 r252 (fast screen with 0xd1850 and 0x3d2c0, both MATCH; control 21). Allocator reading: the object's
// whole-function candidate ranks -7 (50 from its five referencing blocks, -57 over the 26 blocks of the sample chain it
// only lives through), so it is never coloured in the first pass; its split piece from the hidden test onward takes EBX
// and the head piece stays in EAX. Forms: the object as `byte const *` 21; the hidden test as `(... & 1) != 0` 21; the
// object through the existing object_get accessor 21 (same code each); its position named once for the three
// position uses (`point3f *position`), after the hidden return 230, beside the object lookup 245 (814 B). All restored.
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
