// @flags /O2 /Gr
/* UNKNOWN_08C5B0.CPP: the settings bungie.net keeps for a signed-in
   user; each handler applies one to the user's controller (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
#include "pending_messages.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>

/* what a handler is given */
struct s_bungienet_user_request
{
	byte unknown00[0xe];
	short kind;
	long controller_index;
};

typedef bool (__stdcall *bungienet_user_handler)(s_bungienet_user_request *request);

struct s_bungienet_user_handler
{
	long unknown00;
	bungienet_user_handler handler;
};

/* the controllers' state (g_54e8e0, 0xc70 bytes each); this file reads only
   the voice setting at +0x204 */
struct s_controller_voice_view
{
	byte unknown000[0x204];
	long voice_setting;
	byte unknown208[0xc70 - 0x208];
};


/* The request header and its payload have separate lifetimes. */
extern s_pending_message_header g_479750;
struct s_user_settings_payload
{
 dword header[4];
 long voice_setting;
 byte field_14[0x300];
};
s_user_settings_payload g_4d8800;

void function_18fe9e(long index);

static inline s_controller_voice_view *controller_voice_view_get(long controller_index)
{
	return &((s_controller_voice_view *)g_54e8e0)[controller_index];
}

// @retail 0x8c5b0
bool __stdcall bungienet_user_voice_reset(s_bungienet_user_request *request)
{
	long controller_index = request->controller_index;
	s_controller_voice_view *controller = controller_voice_view_get(controller_index);
	long old_setting = controller->voice_setting;
	bool changed = false;
	if (old_setting != 1)
		changed = true;
	controller->voice_setting = 1;
	if (changed)
		function_18fe9e(controller_index);
	return true;
}

static inline bool bungienet_block_available(void)
{
	if (g_479750.kind != 0 && (g_479750.flags & 2))
		return true;
	return false;
}

static inline long bungienet_user_voice_setting(short kind)
{
	long setting;
	if (kind == 1)
	{
		if (bungienet_block_available())
			setting = g_4d8800.voice_setting;
		else
			setting = 1;
	}
	else
		setting = 1;
	return setting;
}

// score 4 (was 4): only the request pointer and the kind differ: retail ECX and AX, ours EAX
// and CX. Before round 55 retail kept the request pointer in ECX, the
// kind in AX, the setting in ECX and `changed` in DL; ours used EAX, CX, EDX and CL.
// The volatile read keeps retail's 16-bit load and register compare of the kind (a plain
// read compiles to `cmp word ptr [eax + 0xe], 1`, 32). Same output as the older
// local_0/local_1 copy form. Tried: plain read (32); kind read inside the condition (32);
// controller index read first (12); if/else and `setting = 1; if` forms (11, 15);
// `switch (kind)` (17, puts setting in ECX but movsx/dec the kind); word kind (11);
// bool temp for the condition (41); `changed = setting != old` (25); without the volatile:
// helper reading the kind from the request 32, kind by const reference 32, single-exit setting
// helper 30, nested kind test 32 (every matched function with retail's unfolded word load and
// register compare tests the value twice, e.g. `type == 3 || type == 4`).
// Round 55: the setting helper as a single exit with an else on each failing arm 4 (kept: setting ECX,
// changed DL); nested returns `if (kind == 1) { if (available) return x; return 1; } return 1;` 4; the
// same with a plain read 32 (95 bytes, folded compare), single exit with a plain read 32, the kind passed
// straight 32; an added `kind == 0` test first 32 (103 bytes, but retail's ECX/AX pair); the index read
// first 5; word/long kind parameter 4/4; inverted `kind != 1` chain 4; `old != setting` 5. Names, a
// byte-offset read, the dword's high half, a reference parameter: the compare still folds.
// Round 121: plain read through a `short const *` helper parameter 32 (95 bytes); forceinline helper 4 (same
// code); the controller pointer taken before the setting 15; the controller index read at each use 23 (99
// bytes); a one-expression ternary helper, plain read 32. One-file LTCG probe (exact here), plain read: a 16-bit
// bitfield kind, a byte-pointer read, the header type's view, by-reference/pointer parameters, `1 == kind`, the
// helper written in place: all fold the compare (`cmp word ptr [eax + 0xe], 1`).
// Round 466: Controller before selector 15; old state before selector 24; controller before kind 14;
// controller before kind then old state 24; complete controller/old state before kind 24;
// the latter three early-old-state graphs with direct change-result production 39/38/38.
// Original acquisition order restored; request/kind registers remain.
// Round 219: availability nested/negative-guard/true-result forms 4/4/4;
// native bit availability 31; named selection result 11; availability outside kind 9;
// nested availability with long change result 12, negative guards with short result 13.
// Original request/kind pair remains EAX/CX versus retail ECX/AX.
// Round 145: a plain read stays in a register when the kind has a second use that constant-propagates
// away: `setting = kind` in the not-available arm 4 (same code as the volatile read; not kept, the
// assignment only steers). Kind read, setting, then index 23 (kind AX, request EDX); index first with that
// arm 5; handler typed as the pending-message header with a last-result helper 32; `kind == 0` arm
// merged into the default 32 (103 bytes, retail's ECX/AX pair).
// Round 106: the kind read inside the selector, index first 5; selector first 23;
// selector without inline, availability force-inline, controller access force-inline,
// const request parameter, const scalar/pointer locals and external-inline selector 4 each.
// Original helper boundaries, parameter and local forms restored; request/kind registers remain.
// Round 58: plain reads: a bool from the kind 32 (95 bytes); kind plus an unused bool 32; `setting = 1` seeded 30
// (94 bytes); seeded `kind == setting` 30; the ternary with `: kind` in the not-available arm 4 (same code as the
// volatile read, the assignment only steers; not kept), with the index first 5, selector first 23 (kind AX, request EDX), if/else
// form 4. Field pointers: kind 32, index 32, both 32, index with the volatile read 4, kind or index with the `: kind` arm 4/4.
// `request->kind` written at each use (no local): index first 32, selector first 30, in both arms 32, `else if` setting 45 (110 bytes); a
// second `kind == 1` in the else 4.
// Round 159 (fast): plain read with the kind seeding the setting 31 (103 bytes; retail's ECX/AX pair, but a movsx survives);
// `long setting = kind; if (setting == 1 && available)` 32 (95 bytes); seeded `setting = 1; if (kind == setting && available)` 30
// (94 bytes); `setting = kind` first in the kind arm 12; inner `else setting = kind` with `available && kind == 1` 4 (same code).
// The pair follows the kind's surviving references: it outranks the request only with a third use that reaches allocation.
// Round 85 (fast, plain reads, no volatile): the existing selector called under an outer `kind == 1` test, as a
// ternary 4 and as if/else 4 (unfolded load, request/kind still EAX/CX); the selector under `switch (kind)` with
// case 1/default 38 (104 bytes); `kind != 1` chain written in place 32 (95 bytes); availability read into a bool
// first, `setting = 1` seeded 43 (108 bytes). The matched sibling 0x8c5b0 loads the request into EAX (no kind).
// Round 114: direct kind as the selector argument 5; register kind/request 4/4; pointer/reference request aliases 4/4
// (NOT kept: aliases require a match); request-taking selector with the index before/after it 5/23;
// request-member selector with a kind local/direct kind 4/5. Original helper and body restored.
// Round 75 (lane 1, fast): an `__assume` on the kind keeps its word load in a register without the
// volatile read: plain read with `__assume(kind == 1)` in the kind arm 4 (same code; not kept, it is always true where
// it stands and only holds the value in a register), `__assume(kind >= 0)` after the read, the index or the setting,
// or in the helper 4 each, two and three assumes 4/4. The pair stays EAX/CX; retail ranks the kind above the request.
// A 600 s permuter run on the plain-read form: 48 tries, none below 4. With the assume or the volatile read: `__assume(request != NULL)`
// 4 (plain 32), the index first 5, const reference or pointer local for the kind 4/4, const reference for the index
// 4, getter helper 4/4, the request read once through its own address 4/4. `setting = kind` on both failing arms 4,
// a byte-width second test of the kind 33 (103 bytes).
// Round 172 (m3 lane 4, fast): a shared inline controller-voice setter (index, setting) used by 0x8c5b0 as well
// (0x8c5b0 stays matched): the plain kind read as the selector's argument 30 (95 bytes), the volatile read in
// the call 23; the setter taking the request 30/23 (plain/volatile); the kind and index read first, then the
// setter 4 (same code). Original body restored.
// Round 156 (m3 lane 6): the selector writing through an out parameter 4 (fast; with a plain read 32); `bool result =
// true` declared first 4 (fast). LTCG one-file harness (this file, unknown_190001.cpp and the ballast reproduce 4): 45
// combinations of volatile/plain/cast kind read, five selector shapes, read order and compare sides best 4 (plain reads
// 11 or more); `switch (kind)` with `case 0:` sharing the default and a plain read 5 (29 instructions, `movsx; dec` for
// the kind), with the volatile read 30. CL-only: 0x8c790's identical `kind == 1` stays unfolded only beside its
// 0x80e14 call (nonzero arguments keep the unfold; removing the call folds its compare as well).
// Round 157 (m3 lane 6, harness): the controller and the setting take EAX and ECX first, then whichever of the request
// and the kind comes next takes EAX; retail allocated the kind first. Raw byte-offset reads as in 0x8c790, plain 29 (the
// compare folds); a seeded selector with a plain read 24; `available && kind == 1` 15 (fast 16); the volatile read with the index
// read after the selector 23; a plain read with a caller-side `kind != 1` test around the selector 4 (fast 4; the unfolded load
// without the volatile read, same pair; a repeated test, not kept).
// Round 161 (m3 lane 5, fast, plain reads): the default path as a call of the reset handler 76 (135 bytes, no merge); a word
// kind through the selector 32 (95 bytes); the selector on the field with the index after 30 (a repeat, re-measured); a
// switch with `case 1` falling into `default` 19 (95 bytes, `movsx; dec`); that switch with the index read after it 28 (95
// bytes: retail's ECX request and EAX kind, but the switch keeps `movsx; dec` for the 16-bit compare).
// Round 171 (m3 lane 6, round 157's harness): 1-8 initialised unused locals at the top 4 each (diagnostic); the
// volatile read with a seeded default (`long setting = 1; if (kind == 1 && available) ...`) 12; the request's fields
// read through a const source pointer local 4; index first with the volatile read as the selector's argument 5; the
// kind held as a long 4; the kind arm's choice as a ternary 4.
// Round 86 (m3 lane 14, fast, plain reads, no volatile): a top-level const request parameter 32 (95 bytes); the kind
// held as a long 32; the selection as one ternary in place of the helper call 32; `old_setting != setting` 32; the
// controller index read after the selector 30 (95 bytes). The compare folds in every plain-read form.
// Round 174 (m3 lane 6, round 157's harness, counts as its positional differences): the selector taking the
// request by value 37 (37 instructions, the copy stays), a by-value local copy for both reads 37, the by-value
// selector with the index read after it 37; the selector taking the request by const reference, plain read 29 (28
// instructions, folded). Diagnostic: the matched sibling 0x8c790's identical `kind == 1` stays unfolded only
// through its inlined 0x80e13 time-stamp read (a compile-only copy folds without it, with its result unused, or
// with 64-bit arithmetic alone); here a plain read still folds with an asm block written into the handler.
// Round 195 (m3 lane 4, fast, all with the volatile read): `setting = kind` in the not-available arm 4 (same code); a
// `kind == 0` arm ahead of the `kind == 1` arm 32 (103 bytes: retail's ECX/AX pair, the extra test survives); the volatile
// read passed straight as the selector's argument, index first 5; the availability test as one returned expression 4;
// `short const kind` 4.
// @retail 0x8c5f0
bool __stdcall bungienet_user_voice_apply(s_bungienet_user_request *request)
{
	short kind = *(volatile short *)&request->kind;
	long controller_index = request->controller_index;
	long setting = bungienet_user_voice_setting(kind);
	s_controller_voice_view *controller = controller_voice_view_get(controller_index);
	long old_setting = controller->voice_setting;
	bool changed = false;
	if (setting != old_setting)
		changed = true;
	controller->voice_setting = setting;
	if (changed)
		function_18fe9e(controller_index);
	return true;
}

struct s_user_settings_definition
{
 long kind;
 const wchar_t *filename;
 long version;
 s_bungienet_user_handler handlers[2];
};

s_user_settings_definition g_46729c =
{
 1, L"bungienet_user.dat", 1,
 { {0, bungienet_user_voice_reset}, {0, bungienet_user_voice_apply} }
};
s_pending_message_header g_479750 =
{
 (dword)&g_46729c, 0, 0, 1, 0, 0, NONE,
 {NONE, NONE, 0}, NONE, &g_4d8800, sizeof(g_4d8800)
};

bool pending_message_request_receive(s_pending_message_header *header, long controller,
 s_pending_message_values values, long value06);

static inline XUID *user_settings_identity(XONLINE_USER *user)
{
 XUID *result = 0;
 if (user) result = &user->xuid;
 return result;
}

static __forceinline long user_settings_next_controller(long index)
{
 long result;
 switch (index + 1)
 {
 case 0: result = 0; break;
 case 1: result = 1; break;
 case 2: result = 2; break;
 case 3: result = 3; break;
 default: result = NONE; break;
 }
 return result;
}

static __forceinline void user_settings_request(long controller, XUID *identity)
{
 if (identity && g_479750.state == 0)
 {
  g_479750.flags &= ~2;
  pending_message_request_receive(&g_479750, controller,
   *(s_pending_message_values *)identity, 0);
 }
}

// @retail 0x8c660
void function_8c660(void)
{
 XONLINE_USER users[4];
 memset(users, 0, sizeof(users));
 XONLINE_USER *signed_in = XOnlineGetLogonUsers();
 if (signed_in) memcpy(users, signed_in, sizeof(users));
 for (long controller = 0; controller != NONE; controller = user_settings_next_controller(controller))
 {
  HRESULT status = users[controller].hr;
  XUID *identity = user_settings_identity(&users[controller]);
  if (identity && identity->qwUserID && SUCCEEDED(status))
  {
   if (!XOnlineIsUserGuest(users[controller].xuid.dwUserFlags))
   {
    if (controller_voice_view_get(controller)->voice_setting == 0)
     user_settings_request(controller, user_settings_identity(&users[controller]));
   }
   else
   {
    s_controller_voice_view *slot = controller_voice_view_get(controller);
    long old_setting = slot->voice_setting;
    bool changed = false;
    if (old_setting != 1) changed = true;
    slot->voice_setting = 1;
    if (changed) function_18fe9e(controller);
   }
  }

 }
}
