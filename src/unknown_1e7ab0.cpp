// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E7AB0.CPP: whether a unit request can start, and whether it should
   end (built with SSE, apart from its neighbours in unknown_1e77c0.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include "units.h"
#include <xmmintrin.h>

/* a player (g_4e8c24, 0x21c bytes): +0x28 is its local player slot */
struct s_player_request_view
{
	byte unknown00[0x28];
	short local_player_index;
	byte unknown2a[2];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

struct s_time_entry
{
	long time;
	short a;
	short b;
};

struct s_request_transition
{
	short state;
	short ticks;
};

/* a local player's state in g_51e9c0 (src/player_request_state.cpp), 0x1b0 bytes:
   the four last unit requests at +0x150 */
struct s_local_player_state_view
{
	s_request_transition transitions[32];
	byte unknown080[0x150 - 0x80];
	s_time_entry requests[4];
	dword request_flags[8];
	long version;
	byte field_194;
	byte field_195;
	byte field_196;
	byte field_197;
	byte timer198;
	byte timer199;
	byte timer19a;
	byte field_19b;
	byte unknown19c[0x1b0 - 0x19c];
};

struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;

struct s_entry_420
{
	long value;
	word field_04;
	word limit;
	word field_08;
	short duration;
	byte field_0c[8];
};

struct s_table_420
{
	byte field_00[0x420];
	long count;
	s_entry_420 *entries;
};

inline s_entry_420 *entry_420_get(long index)
{
	s_entry_420 *result = NULL;
	s_table_420 *table = (s_table_420 *)g_510c94;
	if (table && index >= 0 && index < table->count)
		result = &table->entries[index];
	return result;
}

bool function_1e69d0(s_time_entry *entries, long type, long age_limit);
bool function_1e7a10(long player, long index);
long function_1e7a60(long player, long index);
bool function_1e8eb0(long object_index, long type, bool any_nonzero);

#include "unknown_1248b0.h"
#include <math.h>

struct s_weapon_status_magazine
{
 bool active;
 bool idle;
 short loaded;
 short loaded_maximum;
 short unloaded;
 short total_maximum;
};

struct s_weapon_status
{
 real value;
 real heat;
 bool flag8;
 bool flag9;
 byte unknown0a[2];
 real fraction;
 bool charging;
 bool target_available;
 bool target_charging;
 bool target_ready;
 real target_fraction;
 point3f target_position;
 short magazine_count;
 s_weapon_status_magazine magazines[2];
};

void function_100520(long weapon_index, s_weapon_status *status);
point3f *function_b9dd0(long object_index, point3f *result);
short function_c8860(long unit_index);
long function_cbd50(long unit_index, short weapon_index);
bool function_f42f0(long vehicle_index);
bool function_15eaf0(void);

PRIVATE __forceinline real request_angle_delta(real angle, real previous)
{
 real delta = angle - previous;
 if (delta > 3.1415927410125732f) delta -= 6.2831854820251465f;
 if (delta < -3.1415927410125732f) delta += 6.2831854820251465f;
 return delta;
}

bool function_13cb40(void);
bool motion_sensor_enemy_vehicle_ahead(long local_player_index);
bool motion_sensor_enemy_nearby(long local_player_index);
long function_162fd0(long index);
bool voice_port_can_talk(long port);
bool function_100f00(long weapon_index);
bool function_100fd0(long weapon_index);
short function_101010(long weapon_index, short index);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

struct s_unit_child_iterator
{
 long object_index;
 long unit_index;
 short seat_index;
 long next_index;
};
struct s_damage_object;
void function_d0590(s_unit_child_iterator *iterator, long object_index);
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);

PRIVATE inline byte *request_unit(long object_index, byte *headers)
{
 return *(byte **)(headers + (object_index & 0xffff) * 12 + 8);
}

PRIVATE inline long request_weapon(s_player_request_view *player, long slot, byte *headers)
{
 long unit_index = player->unit_index;
 byte *unit = request_unit(unit_index, headers);
 return function_cbd50(unit_index, *(signed char *)(unit + 0x212 + slot));
}

// Round 30 (lane D): the permuter's best variant scores 601 against its own 608 baseline (seven points,
// below the retention margin) and it adds volatile qualifiers and swapped call arguments, so it is not
// kept. Manual trials this round: a named table base 620 (a tie), the player index set to NONE and then
// conditionally assigned 623, that form with the named base 623, and dropping the (void)&local_index
// anchor 620 (a tie); all restored. What still differs: retail loads the state table into ESI and the
// state pointer into EDI (base + index * 0x1b0), then reuses ESI for the player index and the player
// pointer; ours folds the base into the add and keeps the state in ESI. The 0x54 frame, the EDX type
// argument and the 2308-byte length agree.
// score 620 (was 620): retail loads the state table into ESI and the state pointer into EDI, then reuses
// ESI for the player index and the player pointer and keeps the unit pointer in EBX; ours folds the base
// into the add and keeps the state in ESI, and retail returns from inside each switch case.
// Round 134: retail's case layout is 10-14, 2, 4, 15, 18, 0, 1, 31, 17, 30, ...: cases 0/1/31 after 18
// 656; plain blocked flag 645, both 656; with a plain player pointer too, retail's state edi, player esi,
// spilled unit index and result at entry, but request_weapon stops inlining: 769, case 2 as an if 785,
// plus plain blocked 790, plus a plain address-taken result 726; request_weapon forced inline 939
// (0xcbd50 inlined too, a noinline declaration no help), taking the unit index 658; plain player alone 779.
// Earlier 631 from 645: frame 0x54/type EDX agree; state/player/unit homes differ, retail length recovered.
// frame 0x54/type EDX agree; state/player/unit homes differ, retail length recovered.
// Earlier 649 from 649: frame 0x54 agrees; state/player/unit homes differ; inline depths 1/2 649, object accessor inline 649, array base 649, player-index first 656.
// Earlier 613 from 656: frame/type agree; state/player/unit allocation remains.
// Direct state receiver 656; unsigned unit carrier 656; early blocked local 656;
// multi-use magazine view 656; seat-before-parent acquisition 656; all restored.
// Earlier 649 from 649: Frame 0x54 agrees; state/player/unit homes remain. Native word 649; bits 650; call groups 656; reset groups 771; plain player/status 768 (frame and state/player agree); with selector inline 946, native word 768, bits 768 or index join 790 (plain).
// Earlier 649 from 658: Frame 0x54 agrees; state/player/unit homes remain. Shared seat fields kept 658 to 651, 2300/2308 bytes. Captured seat flags 658; short selector slot 662; typed object headers 658; magazine capacity 658 (plain).
// Frame 0x54 agrees; state/player/unit homes remain. Native word 649; bits 650; call groups 656; reset groups 771; plain player/status 768 (frame and state/player agree); with selector inline 946, native word 768, bits 768 or index join 790 (plain).
// Earlier 645 from 645: Frame 0x54 and EDX type agree; state/player/unit homes remain. Long seat index 647; early child iterator 645; seat disqualifier predicate 645; pooled seat reference 645; function-scope unit index 645 (plain).
// Earlier 645 from 645: Frame 0x54/type EDX agree; state/player/unit homes remain. Shared primary weapon 653;
// secondary 655; status weapon 645; all three observations 688; native input state fields 645 (fast); matched
// sibling preserved.
// DL134: byte state view 645; entry first 646; status/child/workspace cases first
// 642/641/642; direct flag cases first 631 kept; communication first 633; byte/status 642;
// flags with child/status first 647/631. Both keepers plain-confirmed; the latter tie is restored.
// Frame 0x54/type EDX agree; state, player and unit homes remain.
// frame 0x54 agrees; state/player/unit homes differ; inline depths 1/2 649, object accessor inline 649, array base 649, player-index first 656.
// frame/type agree; state/player/unit allocation remains.
// Frame 0x54 agrees; state/player/unit homes remain. Shared seat fields kept 658 to 651, 2300/2308 bytes. Captured seat flags 658; short selector slot 662; typed object headers 658; magazine capacity 658 (plain).
// Frame 0x54 and EDX type agree; state/player/unit homes remain. Long seat index 647; early child iterator 645; seat disqualifier predicate 645; pooled seat reference 645; function-scope unit index 645 (plain).
// Frame 0x54/type EDX agree; state/player/unit homes remain. Shared primary weapon 653;
// Result after lookup 654; unsigned player identifier 645; selector unit field at use 645;
// pooled recorded-type reference 645; selector without inline keyword 645.
// Paired Boolean-role names 631; player/unit handle names 631; receiver-role names 631; one failure
// exit 631; flag/empty cases first 631; flag/input-state cases first 653; flag/health cases first 643;
// flag/weapon-test cases first 653 (fast). All restored.
// Complete eligibility assigned to returned result in reset cases 10/11/12/13/2/16 632/632/640/640/627/643; all six 693; success result after reset 630. Case 2 kept, plain 627.
// Non-reset test-produced results: case 14 666; case 9 665; primary tests 664;
// local-state/vehicle tests 649; direct capability calls 643; health tests 653;
// status/child results 683; all non-reset groups 697. Original result graph restored.
// Round 197: direct-flag cases after case 18, retained case-two result: original local
// qualifications 656; plain player/result/blocked 769/651/656; player+result, player+blocked,
// result+blocked, all plain 701/785/665/736. All original cases, qualifications and bodies restored.
// Round 162: Consistent direct state-array observations with inherited local
// qualification removals none/result/player/result+player/blocked/result+blocked/
// player+blocked/all 827/769/649/671/811/750/660/674.
// All regress; original state receiver, case graph and local qualifiers restored.
// Complemented return with original locals 771; ordinary result/player/both 764/856/832;
// ordinary blocked/result-blocked/player-blocked/all 781/778/858/844. Original case-two
// result, case order, local qualifications and complete executable body restored.
// Shared 25/28, 26/27/29, empty cases, paired groups and all groups
// 634/634/627/634/634/634/634; all with flag cases after 18 651.
// Conditional player-index assignment 628; parent, seat, iterator, port, status
// and target names individually and together 627 each.
// Flag-case order permutations 627/627/627/626/628; individual moves after case 18 609/646/643.
// The 609 form worsens executable differing rows 552 to 561; restored.
// Case pairs 10/11, 12/13, 15/18, 17/30, 20/23, 22/21, 5/6 and 7/8
// reversed individually 627/629/625/627/627/630/633/636 (fast); original order restored.
// Round 78 (fast; aligned rows in brackets, baseline 450): plain player 779 [280] (helper out of line);
// with request_weapon forced inline 981 (0xcbd50 inlined too); with request_weapon taking the unit index
// 665 [413] (inlined again); plus cases 0/1/31 after 18 666 [355] (retail's case layout); plus the player
// index as NONE then an if (retail's or/cmp/je shape) 667 [361]. Register roles (retail: state EDI,
// player ESI, unit index spilled, unit EBX, headers EBP) still differ in all of them.
// Round 89 (fast; per-case byte sizes compared through both jump tables): cases 0/1/31
// after 18 650; plus the existing function_14de70 for the player index (retail's or/cmp/je entry, the
// state base loaded first) 652; plus a plain player pointer 772 at 2132 B, but structurally closest:
// retail's case order, 20 of 28 case sizes equal, 40 calls as retail, unit index spilled; left are
// request_weapon out of line (cases 15/18/16/23/9 short), state/player in ESI/EDI (retail EDI/ESI)
// and headers/unit in EBX/EBP (retail EBP/EBX); request_weapon forced inline 929 (more callees
// inline, the 0x1e8eb0 tails unmerge); request_weapon taking the unit index 657 (still out of line).
// Round 169: Native unit parent/seat/health-and-flags field subsets, all seven 620;
// complete readonly unit receiver 620. Original types and body restored; state/player/unit
// allocation remains despite the matching frame, selector and cleanup.
// Round 220: Entry-getter failure guards, per-use table, both, readonly table,
// readonly with guards/per-use/both, and conditional nullable result 620 each.
// Original getter, state/player/unit receivers and complete body restored.
// Round 272: native state byte bitfields, all seven returned-bit subsets and all
// with ordinary player 620/620/620/620/620/620/620/786. Original field and complete body restored.
// Round 393: Existing weapon-accessor role orders, header-first unit accessor and readonly player inputs:
// 620/620/620/620/620/620/620/620. All helper contracts and complete original case graph restored.
// Round 395: existing-result completion after reset pairs 10/11, 12/13, 2/16 and all six
// 630/630/670/670; with ordinary player carrier 792/792/805/805. All original cases restored.
// Round 395: player pool at use 742; object-table context once 669; both 758;
// each with ordinary child result 760/643/769; both with case-entry iterator
// 758, with ordinary child result 769. Original pools, qualifiers and body restored.
// Round 267: positive admission partitions 620/638/620/638/620/638; actual
// admission result 657, with unit-presence result 853. Original admission,
// cases, local qualifiers and complete executable body restored.
// Round 193: Existing masked-index domains word/short/dword 659/659/620; with Boolean slots 661/661/665;
// word/short indices with short slots 661/661. Original accessor domains, all case bodies, inherited
// qualifiers and matched sibling restored; state/player/unit residency remains.
// Y423: All 32 cases return the completed result, ordinary player/child flags
// and native long eligibility subsets 830/795/871/815/723/828/736/846 fast;
// control 620. All shorten away from retail; complete original body restored.
// DL200: All seven coordinated ordinary inline subsets of the existing test/counter/eligibility
// helpers 620 each; counter stdcall 620; coordinated status-producer inline contract 620. All nine
// retain 559 differing rows through the last retail return. Original target, helper
// definitions/declarations, native cases, pool observations, local qualifiers and all call values
// restored. Base/state and player/unit homes remain.
// DL209: Case-owned failure guards in reset/inventory/capability group subsets 1-7: 620 each; all
// with ordinary player pointer 786. Original cases and qualifier restored.
// Round 459: Uniform caller unit-field observations, bool/byte/short/long return carriers, inherited/plain
// player pointer 760/683/659/661/786/865/650/669. Original unit snapshot, cases and qualifiers restored.
// All executable trial forms restored.
// Round 262: Entry declaration scopes for status/child iterator/metadata result, all
// seven nonempty subsets 620 each; complete workspaces with ordinary player pointer 786 (2124/2308).
// Original scopes, cases, helper contracts and inherited local qualifications restored.
// Round 53 (m3 lane 1; kept: the `(void)&local_index;` line dropped, byte-identical 620: the index
// stays a stack argument without it): the existing function_14de70 for the player index 623 (retail's
// or/cmp/je entry, but the state base then held in EBX and the state in ESI); the same without the
// address line 623; the player index as the matched 0x1e83c0's ternary 622; result declared first 620
// (same code).
// Round N274: Result owned by admitted unit dispatch, one shared switch completion: bool with
// original/ordinary-player/ordinary-child/both local carriers 842/754/858/781; native long
// with those four carriers 721/701/704/671. Original result lifetime, qualifiers and all cases restored.
// Round 56 (m3 lane 1, fast; 0x1e83c0 kept its match, 0x1e6bd0 unchanged at 303): the weapon helper in
// src/unknown_1e77c0.cpp's form (`bool secondary`, 0x212/0x213 select, object table read inside) 653 (2256
// bytes); only the headers parameter dropped (table read inside) 653; the helper as a plain `inline` 620 (same
// code: PRIVATE is empty); the admission test as nested ifs 620 (same code); the request entry fetched before
// the player index 622. Retail tail-merges cases 24-29 into one 0x1e8eb0 call (each case only pushes its
// arguments) and case 18 into the same post-call test, with 19 returns against our 26.
// @retail 0x1e7ab0
bool function_1e7ab0(long local_index, long type)
{
 s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
 byte *data = (byte *)state;
 bool result = false;
 long player_index;
 if (local_index != NONE) player_index = g_4e8c20->entries[local_index];
 else player_index = NONE;
 s_entry_420 *entry = entry_420_get(type);
 if (player_index != NONE && entry &&
  (!(*((byte *)entry + 0x10) & 1) || g_4e6948->state != 2))
 {
  s_player_request_view *volatile player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
  long unit_index = player->unit_index;
  if (unit_index != NONE)
  {
   byte *unit = request_unit(unit_index, g_4e0300->data);
   switch (type)
   {
   case 0: result = (state->field_194 & 1) != 0; break;
   case 1: result = ((state->field_194 >> 1) & 1) != 0; break;
   case 31: result = ((state->field_194 >> 2) & 1) != 0; break;
   case 10:
    if (!function_13cb40() && *(short *)(unit + 0x1fc) != NONE && *(long *)(unit + 0x14) != NONE && function_1e8eb0(unit_index, NONE, true))
    { result = true; if (*(long *)(data + 0x80) != 10) data[0x1a0] = false; }
    break;
   case 11:
    if (!function_13cb40() && *(short *)(unit + 0x1fc) != NONE && *(long *)(unit + 0x14) != NONE && function_1e8eb0(unit_index, NONE, true))
    { result = true; if (*(long *)(data + 0x80) != 11) data[0x1a0] = false; }
    break;
   case 12:
    if (!function_13cb40() && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 12) data[0x1a0] = false; }
    break;
   case 13:
    if (!function_13cb40() && *(long *)(unit + 0x14) == NONE)
    { result = true; if (*(long *)(data + 0x80) != 13) data[0x1a0] = false; }
    break;
   case 14:
    if (function_1e7a60(local_index, 16) == 0 && function_c8860(unit_index) != NONE) result = true;
    break;
   case 2:
    result = !function_13cb40() && function_1e7a10(local_index, 12) && *(long *)(unit + 0x14) == NONE;
    if (result && *(long *)(data + 0x80) != 2) data[0x1a0] = false;
    break;
   case 4:
    if (!function_13cb40())
    {
     long parent = *(long *)(unit + 0x14);
     short seat = *(short *)(unit + 0x1fc);
     if (parent != NONE && seat != NONE)
     {
      byte *definition = g_4e3b44[*(long *)request_unit(parent, g_4e0300->data) & 0xffff].bytes;
      s_unit_seat_definition *seats = *(s_unit_seat_definition **)(definition + 0x1cc);
      s_unit_seat_definition const *seat_data = &seats[seat];
      if (!((bool)seat_data->flags.bit2) && !((bool)seat_data->flags.bit3) && !((bool)seat_data->flags.bit11))
      {
       s_unit_child_iterator iterator;
       function_d0590(&iterator, parent);
       volatile bool blocked = false;
       while (function_d05c0(&iterator))
       {
        if (iterator.seat_index != NONE && iterator.unit_index != unit_index &&
         ((bool)seats[iterator.seat_index].flags.bit2))
        { blocked = true; break; }
       }
       if (!blocked) result = true;
      }
     }
    }
    break;
   case 15:
    if (request_weapon(player, false, g_4e0300->data) != NONE && !motion_sensor_enemy_nearby(local_index)) result = true;
    break;
   case 18:
    if (request_weapon(player, false, g_4e0300->data) != NONE && !motion_sensor_enemy_nearby(local_index) && function_1e7a10(local_index, 15)) result = true;
    break;
   case 17: if (state->field_19b > 0 && *(short *)(unit + 0x1fc) == NONE) result = true; break;
   case 30:
    if (g_4e6948->state == 2 && function_15eaf0() && function_162fd0(local_index) != NONE)
    {
     long gamepad = *(long *)((byte *)player + 0x24);
     if (gamepad != NONE && voice_port_can_talk(gamepad)) result = true;
    }
    break;
   case 16:
    if (!motion_sensor_enemy_nearby(local_index) && request_weapon(player, false, g_4e0300->data) != NONE &&
     request_weapon(player, true, g_4e0300->data) == NONE && function_100fd0(request_weapon(player, false, g_4e0300->data)) &&
     function_101010(request_weapon(player, false, g_4e0300->data), 0) != NONE)
    { result = true; if (*(long *)(data + 0x80) != 16) data[0x1a0] = false; }
    break;
   case 20:
    if (motion_sensor_enemy_vehicle_ahead(local_index) && *(short *)(unit + 0x1fc) == NONE) result = true;
    break;
   case 23:
    if (!motion_sensor_enemy_nearby(local_index) && request_weapon(player, false, g_4e0300->data) != NONE)
    {
     s_weapon_status status;
     function_100520(request_weapon(player, false, g_4e0300->data), &status);
     if (status.magazine_count > 0 && status.magazines[0].loaded_maximum != 0 &&
      status.magazines[0].loaded * 2 <= status.magazines[0].loaded_maximum &&
      status.magazines[0].total_maximum != 0 && status.magazines[0].unloaded > 0) result = true;
    }
    break;
   case 9:
    if (request_weapon(player, true, g_4e0300->data) != NONE && function_100f00(request_weapon(player, true, g_4e0300->data))) result = true;
    break;
   case 25: if (function_1e8eb0(unit_index, 3, false)) result = true; break;
   case 28: if (function_1e8eb0(unit_index, 3, false)) result = true; break;
   case 24: if (function_1e8eb0(unit_index, 1, false)) result = true; break;
   case 26: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 27: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 29: if (function_1e8eb0(unit_index, 4, false)) result = true; break;
   case 22:
    if (*(real *)(unit + 0xf0) < 0.25f || (*(long *)(data + 0x80) == 22 && function_1e7a10(local_index, 21))) result = true;
    break;
   case 21:
    if ((bool)(((dword)*(word *)(unit + 0x10a) >> 9) & 1))
     if (*(real *)(unit + 0xf0) < 0.25f || *(long *)(data + 0x80) == 21) result = true;
    break;
   case 5:
    if (*(word *)(data + 0x140) == 3 && *(word *)(data + 0x148) >= 3) result = true;
    break;
   case 6:
    if (*(word *)(data + 0x140) == 5 && *(word *)(data + 0x148) >= 3)
    {
     byte *target = (byte *)function_badc0(*(long *)(data + 0x144), 2);
     if (target && *(short *)(g_4e3b44[*(long *)target & 0xffff].bytes + 0x21a) != 0) result = true;
    }
    break;
   case 7:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 3) && *(word *)(data + 0x148) >= 3) result = true;
    break;
   case 8:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 12) && state->field_197 >= 3) result = true;
    break;
   case 3: break;
   case 19: break;

   }
  }
 }
 return result;
}
// @retail 0x1e83c0
bool function_1e83c0(long local_index, long type)
{
 s_local_player_state_view *state = &((s_local_player_state_view *)g_51e9c0)[local_index];
 byte *data = (byte *)state;
 bool result = false;
 long player_index = local_index == NONE ? NONE : g_4e8c20->entries[local_index];
 if (player_index != NONE)
 {
  s_player_request_view *player = (s_player_request_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
  long unit_index = player->unit_index;
  if (unit_index != NONE)
  {
   byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
   switch (type)
   {
   case 10:
    if (*(long *)(data + 0x80) == 10)
    {
     if (!data[0x1a0])
     {
      *(point2f *)(data + 0x1a4) = *(point2f *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real yaw = *(real *)(data + 0x8c) - *(real *)(data + 0x1a4);
      real pitch = *(real *)(data + 0x90) - *(real *)(data + 0x1a8);
      if (yaw > 3.1415927410125732f) yaw -= 6.2831854820251465f;
      if (yaw < -3.1415927410125732f) yaw += 6.2831854820251465f;
      if (fabs(yaw) > 0.1745329201221466f || fabs(pitch) > 0.1745329201221466f) result = true;
     }
    }
    break;
   case 11:
    if (*(long *)(data + 0x80) == 11)
    {
     if (!data[0x1a0])
     {
      function_b9dd0(unit_index, (point3f *)(data + 0x1a4));
      data[0x1a0] = true;
     }
     else
     {
      point3f point;
      function_b9dd0(unit_index, &point);
      real x = *(real *)(data + 0x1a4) - point.x;
      real y = *(real *)(data + 0x1a8) - point.y;
      real z = *(real *)(data + 0x1ac) - point.z;
      if (z * z + y * y + x * x > 0.25f) result = true;
     }
    }
    break;
   case 12:
    if (*(long *)(data + 0x80) == 12)
    {
     if (!data[0x1a0])
     {
      *(point2f *)(data + 0x1a4) = *(point2f *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real yaw = *(real *)(data + 0x8c) - *(real *)(data + 0x1a4);
      real pitch = *(real *)(data + 0x90) - *(real *)(data + 0x1a8);
      if (yaw > 3.1415927410125732f) yaw -= 6.2831854820251465f;
      if (yaw < -3.1415927410125732f) yaw += 6.2831854820251465f;
      if (fabs(yaw) > 0.1745329201221466f || fabs(pitch) > 0.1745329201221466f) result = true;
     }
    }
    break;
   case 13:
    if (*(long *)(data + 0x80) == 13)
    {
     if (!data[0x1a0])
     {
      *(point3f *)(data + 0x1a4) = *(point3f *)(unit + 0x64);
      data[0x1a0] = true;
     }
     else
     {
      point3f point = *(point3f *)(unit + 0x64);
      real x = *(real *)(data + 0x1a4) - point.x;
      real y = *(real *)(data + 0x1a8) - point.y;
      real z = *(real *)(data + 0x1ac) - point.z;
      if (z * z + y * y + x * x > 0.25f) result = true;
     }
    }
    break;
   case 14: if (function_c8860(unit_index) == NONE) result = true; break;
   case 2:
    if (*(long *)(data + 0x80) == 2)
    {
     if (!data[0x1a0])
     {
      data[0x1ac] = false; data[0x1ad] = false;
      *(real *)(data + 0x1a4) = *(real *)(data + 0x8c);
      data[0x1a0] = true;
     }
     else
     {
      real delta = request_angle_delta(*(real *)(data + 0x8c), *(real *)(data + 0x1a4));
      if (delta < -0.3490658402442932f) data[0x1ac] = true;
      if (delta > 0.3490658402442932f) data[0x1ad] = true;
      if (data[0x1ad] && data[0x1ac]) result = true;
     }
    }
    break;
   case 4: if (function_1e69d0(state->requests, 0x1d, g_510c54->field_2_3)) result = true; break;
   case 15: if (state->timer19a > 0) { result = true; state->timer19a = 0; } break;
   case 18: if (state->timer199 > 0) { result = true; state->timer199 = 0; } break;
   case 0:
    if (data[0x88] & 4) { result = true; state->field_194 &= ~1; }
    break;
   case 1:
    if (data[0x88] & 1) { result = true; state->field_194 &= ~2; }
    break;
   case 31: if (data[0x88] & 4) { result = true; state->field_194 &= ~4; } break;   case 17: if (state->timer198 > 0) { result = true; state->timer198 = 0; } break;
   case 30:
    if (g_4e6948->state == 2 && function_15eaf0())
    {
     if (*(long *)((byte *)player + 0x24) != NONE)
     {
      const s_type_ff3a2a *input = function_1249a0(*(short *)((byte *)player + 0x24));
      if (input && (((const byte *)input)[0x15] > 0 || ((const byte *)input)[0x18] > 0)) result = true;
     }
    }
    break;
   case 16:
    if (*(long *)(data + 0x80) == 16)
    {
     if (!data[0x1a0]) { data[0x1a0] = true; *(long *)(data + 0x1a4) = 0; }
     else
     {
      if (function_c8860(unit_index) != NONE) ++*(long *)(data + 0x1a4);
      else *(long *)(data + 0x1a4) = 0;
      if (*(long *)(data + 0x1a4) > g_510c54->field_2_3 / 2) result = true;
     }
    }
    break;
   case 20: if (function_1e69d0(state->requests, 0x1f, g_510c54->field_2_3)) result = true; break;
   case 23:
    if (function_cbd50(player->unit_index, *(signed char *)(*(byte **)(g_4e0300->data + (player->unit_index & 0xffff) * 12 + 8) + 0x212)) != NONE)
    {
     s_weapon_status status;
     byte *current_unit = *(byte **)(g_4e0300->data + (player->unit_index & 0xffff) * 12 + 8);
     function_100520(function_cbd50(player->unit_index, *(signed char *)(current_unit + 0x212)), &status);
     if (status.magazine_count > 0 && status.magazines[0].loaded_maximum != 0 &&
      status.magazines[0].loaded < status.magazines[0].loaded_maximum && (*(dword *)(unit + 0x148) & 0x10000000)) result = true;
    }
    break;
   case 9: if (*(dword *)(data + 0x88) & 0x200000) result = true; break;
   case 25: if (*(dword *)(data + 0x88) & 0x10000) result = true; break;
   case 29:
    if (function_1e8eb0(unit_index, 4, false) && function_f42f0(*(long *)(unit + 0x14))) result = true;
    break;
   case 27: if (*(dword *)(data + 0x88) & 0x20000000) result = true; break;
   case 24: if (*(dword *)(data + 0x88) & 0x4000) result = true; break;
   case 26: if (*(dword *)(data + 0x88) & 0x800) result = true; break;
   case 28: if (*(dword *)(data + 0x88) & 0x800) result = true; break;
   case 21: if (*(real *)(unit + 0xf0) >= 0.9999f) result = true; break;
   case 22: if (*(real *)(unit + 0xf0) >= 0.9999f) result = true; break;
   case 5: if (state->field_195 > 0) result = true; break;
   case 6: if (function_1e69d0(state->requests, 0x1c, g_510c54->field_2_3)) result = true; break;
   case 7:
    if (*(word *)(data + 0x140) == 1 && ((*(word *)(data + 0x14c) & 1) || (*(word *)(data + 0x14c) & 2)) &&
     *(word *)(data + 0x148) >= 3 && (data[0xa4] & 2)) result = true;
    break;
   case 8:
    if (*(word *)(data + 0x140) == 1 && (data[0x14c] & 12) && (data[0xa4] & 0x20)) result = true;
    break;
   case 3: break;
   case 19: break;

   }
  }
 }
 return result;
}
