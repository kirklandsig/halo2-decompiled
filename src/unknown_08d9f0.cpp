// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_075870.h"
#include "unknown_059ad0.h"
#include "unknown_0662e0.h"
#include "network_message_types.h"
#include "online_attributes.h"
#include <xtl.h>
#include <string.h>
#include "online_result_cache.h"

class c_class_93590;
class c_class_938e0;
struct s_network_message_gateway;
struct s_network_session_list
{
	c_class_58d20 *sessions[3];
};
struct s_system_link_globals
{
	bool active;
	byte unknown01[7];
	s_session_id session_id;
};
#include "system_link_globals.h"
struct s_online_search_globals
{
	bool active;
	byte unknown01[3];
	s_range_input input;
	long task_index;
	long qos_handle;
	long capacity;
	void *entries;
	long start_time;
};
struct s_search_value_globals;
struct s_0b35e0_entry;
struct s_08d9f0_link
{
	byte unknown00[0x1c];
	void *consumer;
};
struct s_08d9f0_gateway
{
	void *vtable;
	bool initialized;
	byte unknown05[3];
	s_08d9f0_link *link;
	c_type_659ceb *messages;
	c_class_938e0 *handler;
	bool unknown14;
};
struct s_08d9f0_handler
{
	bool initialized;
	byte unknown01[3];
	c_class_93590 *link;
	c_type_659ceb *messages;
	s_network_message_gateway *gateway;
	s_network_observer *observer;
	s_network_session_list *sessions;
};

extern bool g_4d8ba0;
extern c_class_93590 *g_510560;
extern c_type_659ceb *g_510564;
extern s_network_message_gateway *g_510568;
extern c_class_938e0 *g_51056c;
extern s_network_observer *g_510570;
extern c_class_58d20 *g_510574;
s_network_session_list *g_510578;
extern s_network_session_list *g_510550;
extern s_link g_528000;
extern byte g_528b28[4];
extern byte g_529188[4];
extern byte g_5291a0[4];
c_type_659ceb g_528588;
byte g_52e0f8[3 * 0x78b8];
s_network_session_list g_52e0e8;
extern void *g_4d8eb0;
extern s_online_search_globals g_4d8ecc;
extern long g_4d8f04;
extern bool g_4d8f08;
extern long g_4d8f0c;
extern c_data_allocator *g_4d8f10;
extern long g_4d8f14;
extern s_0b35e0_entry *g_4d8f18;
extern s_search_value_globals g_509450;
extern long g_510530;
extern bool g_510554;
extern dword g_510558;
extern bool g_510580;
extern long g_510584;

bool network_link_initialize(c_class_93590 *link);
bool network_observer_initialize(s_network_observer *observer, void *unknown04,
	s_network_observer_configuration *configuration, void *link, void *unknown0c);
bool network_session_initialize(long owner_index, s_network_observer *observer, c_class_58d20 *session,
	c_class_58d20 **sessions, long value14, long value38, void *unknown04);
bool network_session_manager_initialize(long unknown40, long unknown44, void *unknown2c,
	c_class_58d20 *session_a, c_class_58d20 *session_c, c_class_58d20 *session_b);
bool network_session_interface_initialize(void *session_manager);
bool function_7f020(s_network_observer *observer, real *trim);
void __stdcall function_08e090(long unused);
void __stdcall function_08e0b0(long unused);
void __stdcall function_6cad0(long unused);
void transport_register_transition_functions(transport_transition_function startup, transport_transition_function shutdown,
	transport_transition_function reset, void *context);
void online_tasks_initialize(void);
bool function_7fa40(void);
void function_7f930(void);
void pending_messages_reset(void);
void function_53210(void);
void function_8dd70(void);
// score 221 (was 221): four-byte frame and entry predicate agree; helper boundaries,
// readiness storage and global reloads differ.
// Round 110 (diagnostic chain, not kept): the six pointers 0x510560..0x510574 as one struct
// global whose address 0x8dd70 takes (a memset of it in place of the six clears) give retail's
// reload before every call and after every pointer store: 330 (984 bytes, 0x662f0/0xb2cc0 still
// inlined); with noinline declarations of those two in their headers 222 (876 bytes, every call
// as retail's); bool readiness 192 (892 bytes, prologue exact); the link result normalised
// (`!= false`, retail's test/setne) 161; the gateway and handler set-ups as inline helpers taking
// the pointers as arguments (retail's argument loads and its dead `lea ecx, [eax + 8]` exactly)
// 174, with the observer readiness as `link_ready && observer_initialize(...)` 151. Not kept: the
// memset contradicts retail's dispose (it clears the six with the function's EBX zero, 0x8dd70
// 29 -> 43), the helpers have one caller, and what takes the addresses in retail is unknown.
// On the original globals: bool readiness with the normalised link result and the && observer
// readiness 257 (858 bytes); without the normalisation 257.
// entry predicate, helper boundaries and global reloads differ;
// Lane 10 r11 (forms reverted: reading globals through casts of their own addresses is not kept
// without a match): message pointer read through its address 231; noinline 0x662f0 in its header
// 266, with 0xb2cc0 too 321 (calls pair with retail's); all seven network pointers (0x510560..
// 0x510578) read once through their addresses 207 at 892 bytes, each one alone dropped 226-276.
// Retail reloads them after every call and pointer store, as LTCG does for address-taken globals:
// find the original code that takes their addresses (a dispose or setup passing &g) rather than
// casting at the reads. Entry flag: retail's sete/cmp not produced by a negated or explicit test.
// Round 9 (lane 11, fast): a noinline declaration of 0x662f0 in this file (before or after its header)
// leaves it inlined 231/231; in include/unknown_0662e0.h it stays out of line 286; with 0xb2cc0 noinline in
// network_message_types.h too 379 (728 bytes, every registration called as in retail); plus one
// `*(c_type_659ceb *const *)&g_510564` read (diagnostic) 321: that one address-take makes LTCG reload
// the pointer before all nine registration calls exactly as retail does. Left: the prologue (retail keeps
// EBX zero, `push ecx` for a stacked link byte, `sete al; cmp al, bl` entry test), short versus
// materialized bool readiness, frame. All restored.
// short readiness/list value initialization 230; short readiness/list memset 230;
// session construction loop 352; separate session results 231; list/direct observer result 248.
// readiness homes, repeated pointer loads and out-of-line calls differ;
// short readiness 232; unsigned readiness 234; session-list value initialization 231; long observer result 250; long session prerequisite 243.
// Signed byte link/observer/session/all readiness 232/231/234/235;
// signed byte entry predicate 230; original short carriers and clear retained.
// entry predicate, out-of-line calls and global reloads still differ.
// Earlier: entry predicate, out-of-line calls and global reloads still differ.
// Native gateway initialized/zero and handler initialized byte fields 230/230/230;
// consumer/gateway/both receiver fields 230/230/230; all byte fields and all native fields 230/230.
// entry normalization, helper boundaries and global-pointer reloads differ;
// unsigned word link 230; unsigned word observer 230.
// unsigned word sessions 230; unsigned word all 230.
// byte link with word stages 232; byte observer with word stages 231.
// byte sessions with word stages 234; byte first two word session 231.
// Earlier: entry normalization, helper boundaries and global reloads differ.
// Actual link/observer/manager/trim/availability/interface return receivers 230 each;
// five receivers together 230; guarded interface with observer/trim/availability 230.
// entry normalization, helper boundaries and global reloads differ.
// entry normalization, helper boundaries and global reloads remain.
// Earlier: entry normalization, helper boundaries and global reloads remain.
// Shared setup link/message/gateway inputs 232/230/229; all three 233;
// retained observer 230; session-list publication 230; existing gateway reverse
// receiver 255; shared inputs with that receiver 251.
// entry normalization, helper expansion and pointer reloads remain.
// Nine-callback automatic/const/static/byte-index tables 252/252/328/253;
// last-two automatic/static tables 272/288; last-three automatic/static tables 273/285 (fast).
// predicate normalization, helper expansion and reloads remain;
// gateway input/consumer order 230; gateway flag first 230; handler flag first 230;
// handler input order 230; shared integer zero assignment 230; short stage names 230;
// short session prerequisite 236; nested manager/interface tests 230.
// Earlier baseline 234; global publication adjacent swaps, reverse and rotation 230 each.
// Complementary link/observer/session/link-observer/link-session/observer-session/all
// stage domains 229/230/230/229/229/230/229; all stages plus positive entry state 229. All restored.
// Round 164: Plain first-two session cohort long/short/word/byte ordinal
// 249/251/251/237; last-two cohort long/short/word/byte 241/251/251/234.
// All regress; original three-call chain, short stage results and clear restored.
// long link 232; long observer 228; long link observer 230; long sessions 231; long link sessions 231; long
// observer sessions 229; long all 231; word entry 230.
// Round 150: Signed-byte link/session/both returns 230/230/230; canonical link with link/both returns
// 228/228; inline link/session/both 228/223/223 (canonical link in first/third). New session contract
// with early observer/session/both constants 242/231/232; both early with bool/byte/char/canonical-
// short/long link result 224/224/224/240/230. Seven-point form rejected; original contracts and
// readiness restored.
// Round 210: entry constant homes with bool observer/session/both 228/228/235;
// bool link plus observer/session/both 234/235/234; long-bool/bool-long stages 229/229.
// Round 172: Short session accumulation with original/immutable link 231/231; direct list
// prerequisite 226/226; direct observer 234/234; both direct producers 237/237. The four-count forms
// fail the eight-count and match-only requirements; every body and producer restored. Frame four
// agrees; entry normalization, helper expansion and connection reloads remain.
// Constant observer/session/both homes at entry 244/225/224; entry long 230;
// with observer/session/both entry homes 244/225/224; unsigned entry with both 224.
// Both entry homes and bool/byte/char/long link result 228/228/228/225. All restored.
// Round 140: retail reloads the message collection pointer before every registration call
// as if the pointer globals shared an address-taken block with the 0x510580 block cleared
// below (its definitions sit in a contributor file, so they cannot be folded here). Entry
// test without the local 230; through an inline getter (result local, if/else) 230/230;
// `initialized == false` 230; observer readiness as a bool if/else assignment 248.
// Lane 8 r58: noinline 0x662f0 (retail calls it) 286 at 816 bytes; with noinline 0xb2cc0
// 379 at 728; inline+noinline registration callees 378/362 (eax reloaded, from esi);
// with a bool link result 382. Retail reloads the global block before every call and
// after every store through a pointer, as when these globals are address-taken.
// Entry constant homes with failed link/observer/link-observer/session/link-session/observer-session/all
// stages 232/230/235/225/231/229/235; all failed stages with immutable link 235. All restored.
// Round 356: Native handler tail clear 249; gateway pointer/byte tail clear 251;
// complete list clear 230; both tails 248; handler/list 249; gateway/list 251;
// all three and native message-table extent 248/248. Original clear boundaries restored.
// Complete link/search/scalar/gateway/ready publication orders 0-1-2-3-4, 1-0-2-3-4,
// 0-2-1-3-4, 0-2-3-1-4, 3-0-1-2-4, 0-4-1-2-3, 1-2-0-3-4 and 2-3-0-4-1 each 230.
// Each search reset clears before both sentinels; original complete publication order restored.
// Round 69: no retail code or data uses any of the seven pointer addresses as a value, and 0x51057c is
// unused, so the address-take was folded away in retail. Entry flag set by if/else (true first or false
// first), by a ternary, or as `!= true` 230/230/230/229; an early return in place of the wrapping
// block 230.
// Round 197: Definite short observer/list/both else 244/226/239; canonical link with observer/list/both else 240/229/237; both else with bool/byte link 232/232. All restored.
// Uninitialized function-scope link/observer/session/link-observer/link-session/observer-session/all
// short declarations and complete stage-role names 230 each; every assignment kept at its stage.
// Round 223: Canonical true tests on retained short link/observer/session stages, seven subsets 228/250/250/250/250/250/250; all with canonical entry consumption 250. Original truth tests and stage flow restored.
// Run 18 (lane D): entry flag compared against false and read a second time on the failure path
// (230 -> 221): the prologue now matches retail's sete/cmp byte for byte. Still differs: retail
// calls 0x662f0 and the nine registration wrappers out of line and reloads the seven network
// pointers after every call; ours expands them and keeps the link result in EBP.
// Original-stage Boolean observer/session/both 224/225/227; Boolean link with
// Boolean observer/session/both 222/230/227; byte link with both Boolean stages 227;
// all Boolean with immutable link 227. Original short stage storage restored.
// Round 173: complete link/message/both/gateway/link-gateway/message-gateway/all-three
// initialized root lifetimes 219/230/235/228/225/235/232; all seven roots 215 (897 bytes).
// Exact-only snapshots restored; frame/entry agree, helper calls, readiness and global
// reloads remain. The six-count form fails the eight-count gate as well.
// Round 172: coordinated native AL success return domains at the existing
// trim/interface providers, byte/bool, char/bool, bool/byte, bool/char,
// byte/byte, char/char, byte/char and char/byte: 221 each.
// Every declaration and both provider return contracts restored.
// Round 269: Native session-admission chain split at all seven call-boundary
// subsets, then all with separate manager/interface gates: 221 each. Original
// complete chain, short readiness, observations and entire body restored.
// Native gateway/handler entry declarations with immutable entry decision,
// seven subsets and complete receiver role names: 221 each. Original lifetimes restored.
// Final-completion bool/byte/short/long at admitted/entry scope all 230; original success return
// and cleanup restored. No flagged completion form matches.
// @retail 0x8d9f0
void function_8d9f0(void)
{
	bool initialize = (g_4d8ba0 == false);
	if (initialize)
	{
		network_configuration_initialize();
		g_510560 = (c_class_93590 *)&g_528000;
		g_510564 = &g_528588;
		g_510568 = (s_network_message_gateway *)g_528b28;
		g_51056c = (c_class_938e0 *)g_529188;
		g_510570 = (s_network_observer *)g_5291a0;
		g_510574 = (c_class_58d20 *)g_52e0f8;
		g_510578 = &g_52e0e8;
		short link_ready = network_link_initialize(g_510560);
		memset(g_510564, 0, 0x5a0);
		network_message_types_register_discovery(g_510564);
		network_message_types_register_connection(g_510564);
		function_adab0(g_510564);
		network_message_types_register_session_membership(g_510564);
		network_message_types_register_session_parameters(g_510564);
		network_message_types_register_view_establishment(g_510564);
		network_message_types_register_synchronous(g_510564);
		network_message_types_register_game_results(g_510564);
		network_message_types_register_test(g_510564);
		short observer_ready = false;
		if (link_ready)
		{
			s_08d9f0_gateway *gateway = (s_08d9f0_gateway *)g_510568;
			gateway->link = (s_08d9f0_link *)g_510560;
			gateway->link->consumer = gateway;
			gateway->messages = g_510564;
			gateway->handler = NULL;
			gateway->unknown14 = false;
			gateway->initialized = true;
			s_08d9f0_handler *handler = (s_08d9f0_handler *)g_51056c;
			handler->link = g_510560;
			handler->messages = g_510564;
			handler->gateway = g_510568;
			((s_08d9f0_gateway *)handler->gateway)->handler = g_51056c;
			handler->observer = NULL;
			handler->sessions = NULL;
			handler->initialized = true;
			if (network_observer_initialize(g_510570, g_510560,
				(s_network_observer_configuration *)((byte *)&g_network_configuration + 0x14a0), g_510568, g_510564))
				observer_ready = true;
		}
		((s_08d9f0_handler *)g_51056c)->observer = g_510570;
		short sessions_ready = false;
		if (observer_ready)
		{
			memset(g_510578->sessions, 0, sizeof(g_510578->sessions));
			sessions_ready = true;
		}
		g_510550 = g_510578;
		((s_08d9f0_handler *)g_51056c)->sessions = g_510578;
		if (sessions_ready &&
			network_session_initialize(0, g_510570, g_510574, g_510578->sessions, 1, 0, g_510568) &&
			network_session_initialize(1, g_510570, g_510574 + 1, g_510578->sessions, 1, 1, g_510568) &&
			network_session_initialize(2, g_510570, g_510574 + 2, g_510578->sessions, 2, 2, g_510568))
		{
			g_4d8ea8.link = g_510560;
			memset(&g_4d8ecc, 0, 0x38);
			g_4d8f04 = 0;
			g_4d8f08 = false;
			g_4d8f0c = 0;
			g_4d8f10 = NULL;
			g_4d8f14 = 0;
			g_4d8f18 = NULL;
			g_4d8eb0 = g_510568;
			g_4d8ea8.initialized = true;
			g_4d8ecc.task_index = NONE;
			g_4d8ecc.qos_handle = NONE;
			if (network_session_manager_initialize((long)g_510568, (long)g_510570, g_510578,
				g_510574, g_510574 + 1, g_510574 + 2) &&
				network_session_interface_initialize(g_510578))
			{
				memset(&g_509450, 0, 0x70f8);
				*(bool *)&g_509450 = true;
				g_510530 = NONE;
				if (function_7f020(g_510570, &g_network_configuration.real10))
				{
					transport_register_transition_functions((transport_transition_function)function_08e090, (transport_transition_function)function_08e0b0, NULL, NULL);
					online_tasks_initialize();
					transport_register_transition_functions(NULL, (transport_transition_function)function_6cad0, NULL, NULL);
					if (!function_7fa40())
						function_7f930();
					g_510558 = GetTickCount();
					g_510554 = true;
					pending_messages_reset();
					function_53210();
					memset(&g_510580, 0, 0x268);
					g_510584 = NONE;
					g_510588.registered_index = NONE;
					g_4d8ba0 = true;
					return;
				}
			}
		}
		if (initialize)
			function_8dd70();
	}
}
