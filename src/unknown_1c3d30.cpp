#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "havok_reference.h"
#include "unknown_184250.h"
#include <float.h>
// @flags /O2 /arch:SSE /Gr

void __cdecl function_2d8910(long arg_0);
void __cdecl function_2d8890();
class c_2d8ab0
{
public:
 void function_2d8ab0();
};
void function_146bf0();
void function_278f00();
void function_1c4260();
void function_1c4a20(hkEntity const *entity);
struct s_311100
{
 real field_0;
 real field_4;
};
class c_311100
{
public:
 void function_311100(s_311100 const *arg_0);
};
struct s_278300_references;
void function_278300(s_278300_references *references);
struct s_2797a0_group
{
 byte unknown00[0x40];
 long count;
};
struct s_2797a0_groups
{
 byte unknown00[8];
 s_2797a0_group **first;
 long first_count;
 long unknown10;
 s_2797a0_group **second;
 long second_count;
};
struct s_2797a0_iterator
{
 byte unknown00;
 bool second;
 byte unknown02[2];
 long group;
 long index;
 s_2797a0_groups *groups;
 long mode;
};
bool function_2797a0(s_2797a0_iterator *iterator);
struct s_1c3d30
{
 byte field_0[8];
 c_havok_reference_counted **field_8;
 long field_c;
 long field_10;
 long field_14;
 byte field_18;
};
struct s_1c3d31
{
 byte field_0[0x3c];
 hkEntity **field_3c;
};
extern hkWorld *g_51e9a4;
extern byte *g_51eca8;
extern short g_51ecb0;
c_2d8ab0 *g_47f044;

// MiniMax R (11-01): no new form, no lower score; the element select still loads the field pointer
// into EAX and the index into ECX where retail uses ECX and EDX, and the release loop's entry test
// still uses EDX and EAX where retail uses EAX and ECX.
// score 7 (was 7): this round: five more shapes of the group loop's call (the entity list in a
// named local, the element read through pointer arithmetic, the index read into a local before the
// select, both of those together, and the element pointer in a local) all compile to the same 507
// bytes and leave the same seven differences; four more shapes of the release loop's condition
// (the limit compared the other way round, the limit minus one, and a negated form) give 10, 12
// and 65. Still the element select loads the list into EAX and the index into ECX where retail
// uses ECX and EDX, and the release loop's entry test uses EDX and EAX where retail uses EAX and
// ECX.
// score 7 (was 7): this round: the listings were re-read only; no new shape of the two register
// pairs was found. The group loop still loads the entity list into EAX and the index into ECX
// where retail uses ECX and EDX, and the release loop's entry test uses EDX and EAX where retail
// uses EAX and ECX. The permuter (this round and the previous one) produced no candidate.
// score 7 (was 7): this round: seven more shapes for the group loop and the release loop: the
// field pointer, the index, the entity and the group index in named locals, the selected group
// list in a named local, the limit and the counter in named locals, and the release loop written
// as a for statement. The named-local forms all compile to the same 507 bytes and the same seven
// differences; the selected-list form gives 63 and the limit and counter locals 137. Still the
// element select loads the field into EAX and the index into ECX where retail uses ECX and EDX,
// and the release loop's entry test uses EDX and EAX where retail uses EAX and ECX.
// score 7 (was 7): this round's two more shapes: the entity list read as a pointer sum into the
// group entry, and the release loop written as a guarded do-while; both compile to the same 507
// bytes with the same seven differences. Still the entity list and the index of the group loop
// take EAX and ECX where retail uses ECX and EDX, and the release loop's entry test uses EDX and
// EAX where retail uses EAX and ECX.
// score 7 (was 7): the entity list and the index of the group loop and the two registers of
// the release loop's entry test still differ from retail's (ECX and EDX, EAX and ECX) in every
// shape tried: the list and the index as named locals, the index read before the select, the
// entity as a named local, the select reversed, the group entry as a pointer sum, the release
// condition reversed, `1 + field_14`, a for loop and `field_14 < field_c - 1`. All of them
// compile to the same bytes and leave the same seven differences.
// score 7 (was 7): the group loop still loads the entity list into EAX and the index
// into ECX where retail uses ECX and EDX, and the release loop's entry test still uses
// EDX and EAX where retail uses EAX and ECX; selecting the table and indexing it after
// gives retail's tail and entry test but breaks the select block (63), so the element
// select is kept.
// score 7 (was 7): in the loop over the groups the entity list and the index take
// different registers (retail ECX and EDX, ours EAX and ECX), and the release loop's
// entry test uses EAX and ECX where ours uses EDX and EAX.
// Shared/separate/entry control words 7/7/7; early step record 64; aggregate
// and scoped step records 7/7; early record with shared control 64; control bit updates 7.
// Iterator pointer 7; groups local 78; setup scope 7; iterator name 7;
// release for 7; world declaration at entry 7; group-entry scope 7;
// release-index pointer 10.
// group-table ternary reversed 11; entity list local 7; index local 7; both locals 7.
// @retail 0x1c3d30
void function_1c3d30()
{
 if (g_51e9a4)
 {
  real local_0 = g_510c54->rate;
  function_2e90a0(s_184251(false));
  ((s_1c3d30 *)g_51eca8)->field_18 = true;
  /* read through a pointer: retail stores field_18 before this load */
  if (*(c_2d8ab0 *const *)&g_47f044)
  {
   function_2d8910(2000000);
   function_2d8890();
  }
  s_311100 local_1;
  local_1.field_0 = local_0;
  local_1.field_4 = 1.0f / local_0;
  function_146bf0();
  _control87(0x9001f, 0x8001f);
  _mm_setcsr(_mm_getcsr() | 0x1f80);
  ((c_311100 *)g_51e9a4)->function_311100(&local_1);
  _mm_setcsr(_mm_getcsr() & 0xffffffc0);
  _clearfp();
  _control87(0x9001f, 0xfffff);
  function_278f00();
  function_1c4260();
  function_146bf0();
  if (g_47f044)
   g_47f044->function_2d8ab0();
  s_1c3d30 *local_2 = (s_1c3d30 *)g_51eca8;
  local_2->field_18 = false;
  s_2797a0_iterator local_3;
  local_3.groups = (s_2797a0_groups *)g_51e9a4;
  local_3.mode = 0;
  local_3.second = false;
  local_3.group = NONE;
  local_3.index = NONE;
  local_3.unknown00 = true;
  ++g_51ecb0;
  while (function_2797a0(&local_3))
  {
   s_1c3d31 *local_5 = (s_1c3d31 *)(!local_3.second ? local_3.groups->first[local_3.group] : local_3.groups->second[local_3.group]);
   function_1c4a20(local_5->field_3c[local_3.index]);
  }
  --g_51ecb0;
  while (local_2->field_14 + 1 < local_2->field_c)
  {
   ++local_2->field_14;
   c_havok_reference_counted *local_6 = local_2->field_8[local_2->field_14];
   havok_reference_remove(local_6);
   if (!((hkRigidBody *)local_6)->body_is_simulating().m_bool)
    function_1c4a20((hkEntity *)local_6);
   local_2 = (s_1c3d30 *)g_51eca8;
  }
  function_278300((s_278300_references *)local_2);
 }
}
