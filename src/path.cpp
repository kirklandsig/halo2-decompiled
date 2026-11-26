// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "path.h"
#include "globals.h"
#include "unknown_20fe20.h"
#include "unknown_0259a0.h"
#include "vector_products.h"
#include <float.h>
#include <string.h>

real function_30bf0(vector3f *v);

bool function_26c4e0(s_pathfinding_data *pathfinding, s_type_c3b527 const *start, long start_node_index,
	s_type_c3b527 const *end, long end_node_index, long flags, s_path_trace_result *result);

/* Local views of fields not yet named in path.h. */
struct s_path_input_view
{
	real radius;
	bool unknown04;
	byte unknown05[0xb];
	bool start_valid;
	s_type_c3b527 start;
	long start_node_index;
	bool attractor_valid;
	point3f attractor_point;
	long attractor_object_index;
	real attractor_radius;
	real field_40;
	bool unknown44;
	bool distance_limit_valid;
	byte unknown46[2];
	real distance_limit;
	real link_penalty;
};

struct s_path_destination_view
{
	byte unknown00[0x54];
	bool destination_valid;
	s_type_c3b527 destination;
	long destination_node_index;
	real destination_radius;
};

/* what a link crosses: its type (NONE for a sector edge) and the edge or surface index;
   a node keeps the one it was entered through */
struct s_path_link_target
{
	short type;
	short unknown02;
	long index;
};

/* The existing s_type_136112 view begins at the heap-index field. This view
   begins at the actual node's start and exposes the hash key at +8. */
struct s_path_node_key_view
{
	short child;
	short parent;
	long unknown04;
	long node_index;
	bool flag0c;
	bool flag0d;
	bool flag0e;
	byte unknown0f;
	s_path_link_target target;
	s_type_c3b527 entry_point;
	real entry_distance;
	real attractor_distance;
	real path_distance;
	real cost;
	real estimated_distance;
	short quantized_cost;
	short depth;
	short heap_index;
	short unknown42;
};

struct s_path_lookup_view
{
	byte unknown00[0xb0];
	s_path_node_key_view nodes[1024];
	short heap_count;
	path_heap_entry heap[1025];
	short hash_table[4096];
};

struct s_path_location_entry_view
{
	short unknown00;
	short unknown02;
	long node_index;
};

struct s_path_location_view
{
	short flags;
	short count;
	s_path_location_entry_view entries[3];
};

struct s_path_link_view
{
	long node_index;
	short flags;
	short unknown06;
	s_path_link_target target;
	s_type_c3b527 point;
	vector3f vector;
	bool unknown2c;
	bool unknown2d;
	bool unknown2e;
};

struct s_path_closest_view
{
	byte unknown00[0x90];
	short node_index;
	short unknown92;
	real distance;
	real estimated_distance;
	s_type_c3b527 point;
};

struct s_path_edge_view
{
	word vertices[2];
	signed char flags;
	byte unknown05[3];
	word next_edges[2];
	word nodes[2];
};

struct s_path_surface_link_view
{
	short type;
	short next;
	word unknown04;
	word unknown06;
	word unknown08;
	word unknown0a;
	byte unknown0c;
	signed char unknown0d;
	word unknown0e;
	short unknown10;
	short unknown12;
};

struct s_path_links_data_view
{
	long node_count;
	s_pathfinding_node *nodes;
	long edge_count;
	s_path_edge_view *edges;
	byte unknown10[0x2c - 0x10];
	point3f *vertices;
	byte unknown30[8];
	long surface_count;
	s_path_surface_link_view *surfaces;
};

struct s_path_step_view
{
	short type;
	short unknown02;
	long node_index;
	short link_index;
	short unknown0a;
	s_type_c3b527 point;
};

struct s_path_result_view
{
	bool valid;
	s_type_c3b527 destination;
	long destination_node_index;
	real destination_distance;
	s_type_c3b527 start;
	bool complete;
	signed char step_count;
	byte step_index;
	s_path_step_view steps[4];
	long object_index;
	short type;
	bool unknowna6;
	s_path_location_view location;
};

bool function_26f3f0(long actor_index, s_type_c3b527 const *entry, s_type_f17a25 *state,
	s_pathfinding_data *pathfinding, long surface_index, s_type_c3b527 const *parent_point, long parent_node_index,
	long *parent_node_index_out, s_type_c3b527 *out, long *out_node_index);
void __stdcall function_2c2060(s_type_f17a25 *state, short count, s_path_step_view const *steps,
	short *out_count, s_path_step_view *out, bool *complete);
bool __stdcall function_2c41b0(long actor_index, s_type_f17a25 *state, short count,
	s_path_step_view const *steps, bool avoid, short *out_count,
	s_path_step_view *out, bool *complete, long *object_index, long *type, bool *flag);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

bool function_26f150(short type, point3f const *start, point3f const *alternate_start,
	point3f const *end, point3f const *alternate_end);

PRIVATE short function_272020(s_pathfinding_data const *pathfinding,
	s_path_node_key_view const *node, s_path_link_view *links, s_type_f17a25 const *state);
PRIVATE bool function_271630(s_type_f17a25 *state);
PRIVATE void function_271ef0(s_type_f17a25 *state, short index);
PRIVATE real function_272810(s_type_f17a25 const *state, s_type_c3b527 const *start,
	point3f const *end, real *distance_out);

PRIVATE void function_271e50(s_type_f17a25 *state, short index);
PRIVATE void function_271fd0(s_type_f17a25 *state, short node_index, short cost);
short function_272700(s_type_f17a25 *state, long node_index);
PRIVATE __declspec(noinline) void function_272740(point3f const *attractor,
	point3f const *start, point3f const *end, point3f *out);

// @retail 0x270590
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->start_valid = true;
	input->start = *point;
	input->start_node_index = node_index;
}

// @retail 0x2705c0
void function_2705c0(s_path_source *source, point3f const *point,
	real radius, long object_index, real weight, bool unknown)
{
	s_path_input_view *input = (s_path_input_view *)source;
	input->attractor_valid = true;
	input->attractor_point = *point;
	input->attractor_object_index = object_index;
	input->attractor_radius = radius;
	input->field_40 = weight;
	input->unknown44 = unknown;
}

// whether the location holds an active entry for the node: retail's one caller (0x26c590) sets up
// the location (edx) before the node (ebx), so the node is declared first
// @retail 0x270600
PRIVATE bool function_270600(long node_index, s_path_location const *location)
{
	s_path_location_view const *view = (s_path_location_view const *)location;
	bool result = false;
	for (short i = 0; i < view->count; i++)
	{
		if (view->entries[i].unknown00 == NONE &&
			view->entries[i].node_index == node_index && (view->flags & (1 << i)))
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x270640
PRIVATE bool function_270640(s_type_f17a25 *state, s_type_c3b527 const *point,
	long node_index, bool *arg_c793c4, s_type_c3b527 *out)
{
	short index = function_272700(state, node_index);
	bool result = false;
	if (index != NONE)
	{
		s_path_lookup_view *lookup = (s_path_lookup_view *)state;
		s_path_node_key_view *node = &lookup->nodes[index];
		while (node->parent != NONE)
		{
			s_path_node_key_view *parent = &lookup->nodes[node->parent];
			if (point->output_index != parent->entry_point.output_index)
			{
				break;
			}
			s_path_trace_result trace;
			if (function_26c4e0((s_pathfinding_data *)state->pathfinding, point, node_index,
				&parent->entry_point, parent->node_index, 0, &trace))
			{
				break;
			}
			node = &lookup->nodes[node->parent];
		}
		if (node->parent == NONE)
		{
			*arg_c793c4 = true;
			*out = ((s_path_input_view *)&state->source)->start;
		}
		else
		{
			*arg_c793c4 = false;
			*out = node->entry_point;
		}
		result = true;
	}
	return result;
}

// @retail 0x270750
bool function_270750(byte *buffer, long unknown, s_actor_point_target const *target,
	real *distance, long a, long b)
{
	s_type_f17a25 *state = (s_type_f17a25 *)buffer;
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	real *attractor_distance = (real *)a;
	vector3f *direction = (vector3f *)b;
	bool result = false;
	short index = function_272700(state, unknown);
	if (index != NONE)
	{
		s_path_node_key_view *node = &lookup->nodes[index];
		real path_distance = function_210970(&node->entry_point, target) + node->path_distance;
		real closest_distance;
		if (input->attractor_valid)
		{
			point3f start_point, end_point, closest;
			function_210850(&node->entry_point, &start_point);
			function_210850(target, &end_point);
			point3f const *attractor = &input->attractor_point;
			function_272740(attractor, &start_point, &end_point, &closest);
			vector3f delta;
			vector3d_from_points3d(attractor, &closest, &delta);
			closest_distance = (real)sqrt(delta.j * delta.j + delta.i * delta.i + delta.k * delta.k);
			if (closest_distance > node->attractor_distance)
				closest_distance = node->attractor_distance;
		}
		else
		{
			closest_distance = 0.0f;
		}
		if (attractor_distance)
		{
			*attractor_distance = closest_distance;
		}
		*distance = path_distance;
		result = true;
		if (direction)
		{
			short child = NONE;
			short current = index;
			real accumulated = 0.0f;
			do
			{
				node = &lookup->nodes[current];
				node->child = child;
				child = current;
				current = node->parent;
			}
			while (current != NONE);
			current = child;
			while (current != NONE && accumulated < 0.8f)
			{
				node = &lookup->nodes[current];
				current = node->child;
				accumulated += node->entry_distance;
			}
			s_type_c3b527 const *end = current == NONE ? target : &node->entry_point;
			function_210be0(&input->start, end, direction);
			function_30bf0(direction);
		}
	}
	else
	{
		if (attractor_distance)
		{
			*attractor_distance = FLT_MAX;
		}
		if (direction)
		{
			*direction = *g_4687a4;
		}
		*distance = FLT_MAX;
	}
	return result;
}

// score 286 (was 286; m3 lane 11 r152 unchanged, its line just above the marker): frame 0xe8 versus retail 0xe4, the folded entry depth read, and the link/special flag
// homes remain; retail spills both flags to one byte slot (+0xf), so its source sets them by tests (match-only here).
// MiniMax B (11-20): no new form, no lower score (286): the link count's increment split out of the copy's index
// (`memcpy(&links[link_count], ...)` then `link_count++`) measures 298, so the post-increment form is the better one;
// retail still counts the links in a dword (`inc dword ptr [esp + 0x1c]`) but compares it as a word (`cmp dx, word ptr
// [esp + 0x114]`), which this build does not do, and the frame is still 0xe8 against retail's 0xe4.
// MiniMax A (11-19): no new form, no lower score (286): rebuilt the body on retail's own listing (the rep stosd fill, the
// split link copy, the walkable bit as a byte local, the dword previous/count homes, the node-index loop variable and the
// per-arm distance through vector3d_from_points3d/length_sq3f): the two fill loops and the flag form reach retail's frame
// and its `movsx`/`shr`/`and` walkable bit but score 403 and 362 (the byte local and the flag home move the whole frame),
// the dword previous/count homes with the split copy reach 343, and a `data` pointer at function scope changes the caller's
// convention (state leaves EBX) - all worse than the body as it stands, so the body is unchanged.
// MiniMax R (11-17): no new form, no lower score (286): the node pointer at function scope with the previous pointer's NULL
// initializer dropped gives retail's frame (0xe4) and the same parameter homes, but the body grows by three bytes and the score
// rises to 357, so the body is as it was.
// MiniMax C (11-14): no new form, no lower score (286): retail's frame is 0xe4 against this build's 0xe8 - retail keeps the edge's
// walkable bit in a byte local written only on the "index in range" path and reuses it in the special test, while this build keeps
// that bit in the flags and gives the frame an extra dword; the node depth is read at one address here and through a lea there.
// MiniMax I (11-15): no new form, no lower score (286): the frame is still 0xe8 against retail's 0xe4, retail keeps the edge's walkable bit in a byte local while this build keeps it in the flags, and the node depth is read through a lea here and at one address there.
// MiniMax R (11-16): no new form, no lower score (286): the frame is still 0xe8 against retail's 0xe4 (the extra dword is the
// `previous` pointer, set to NULL here and left alone there; dropping that initializer gives retail's frame but the body grows
// by two bytes and the score rises to 357), the node depth is still read at one address here and through a lea there, and the
// prologue's saves and stores are in another order; the permuter (27 tries in 313 s, annealing) found no variant under 286.
// M3 lane 8 r159 (fast with 0x270930, which stays matched): lane 11 r71's flag form rebuilt on the current file 2 again
// (1388 bytes, masked 0, re-measured; only `mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]` swapped at the scan's back
// edge). On that 2, looking for retail's extra loop block that is gone after allocation: the projection clamp with the
// three point stores written in each clamp arm (hoping they cross-jump after allocation) 145 (1408 bytes, not merged); the
// count set by `count = count == 0 ? node->depth + 1 : count;` 288 (the count block changes, the pair does not move);
// the 1600 distance test written at the end of each output-index arm 488 (1489 bytes; not merged, 0x270930 then 260);
// `margin * 2.0f` named once before the scan as `minimum_length` 89 (1392 bytes); the step's node index stored before the
// first-count update 294 (1391 bytes).
// No plain form for the main body found that the notes above do not list; the body below is unchanged.
// MiniMax G (11-12): no new form, no lower score (286): frame 0xe8 against retail's 0xe4; the acquisition, the flags
// and the scalar homes remain, and the geometry now uses shared differences.

// Earlier at 286 (was 286): frame 0xe8 against retail's 0xe4, and from there the link test, the flag homes and the
// scan differ; the two-flag form of round 71 below (match-only) gives 2 at 1388 bytes, left: two reloads at the scan
// loop's latch in the other order (`mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]`). Rounds follow, newest last.
// MiniMax A (11-14): no new form, no lower score (286): the frame is still 0xe8 against retail's 0xe4 (the extra dword
// is the `previous` pointer) and the node depth is read at one address here and through a materialised node there;
// dropping that pointer's initializer gives retail's frame but measures 357.
// MiniMax H (11-14): no new form, no lower score (286): this round re-read the diff: our frame is four bytes larger, so every home below the first difference sits higher, and retail reaches the node entry through a lea where we use an indexed address; the earlier rounds' frame trials reached 0xe4 only by wrecking the traversal.
// MiniMax D (11-14): no new form, no lower score (286): the frame is still 0xe8 against retail 0xe4, and the row field is still read through one folded address where retail forms the row address first and reads the field at its own offset.
// MiniMax B (11-13), NOT kept (coordinator: 4 points, under the 8-point bar; the two independent clamps also differ from the
// else-if when margin > length - margin): 282 from 286 with the projection clamp as two minimum-style assignments, which gives retail's exact 1388-byte body; what still differs is the frame (0xe8 against retail's 0xe4), the entry depth read (this build folds the node's offset into the address where retail materialises the node with a lea and reads the depth through it), the previous-node and flag homes, and the scan loop's back-edge reload pair order; a short link count scores 365 and the node pointer at function scope 282 (the same code), and the two test-set flags that reach 16 are match-only.
// MiniMax G (11-12): no new form, no lower score (286): nothing new this round; the frame 0xe8 against retail's 0xe4; the
// acquisition, the flags and the scalar homes remain, and the geometry uses shared differences.
// MiniMax H (11-12): no new form, no lower score (286): our frame is still 0xe8 against retail's 0xe4 and the node depth is still read with one instruction where retail uses a lea and a byte displacement; the inverted depth, previous-index, count and type tests score 307, 329, 286 and 286, the hoisted node pointer 286 and a separate first-node pointer 282.
// MiniMax A (11-12): no new form, no lower score (286); the first difference is the acquisition, where retail computes the node address with a lea and reads the depth at +0x3e while ours reads it at +0xee, and retail's frame (0xe4) is exactly filled by its locals while ours needs one dword more, so the two sides' slots stay four bytes apart.
// MiniMax G (11-11): no new form, no lower score (286); the frame is 0xe8 against retail's 0xe4 and the acquisition, flag and
// scalar homes still differ; the scan's previous point, the link copy as an aggregate assignment, the previous-node test with
// the arms swapped and one pathfinding-data local for the link and surface tests all give the same code or a higher score.
// MiniMax H (11-11): no new form, no lower score (286); our frame is 0xe8 against retail's 0xe4, and in the scan retail keeps the link index in a register and multiplies it by 0x44 while ours reloads it from a stack slot each time, so the frame slots on the two sides sit four bytes apart.
// MiniMax B (11-11): no new form, no lower score (286): the 200 of the round below comes from a flag that one test
// sets and the next test reads, which is kept only with a byte match, so it is not available here.
// Earlier status line, 286 (was 286): Frame 0xe8 versus retail 0xe4, acquisition, flags and scalar homes remain; geometry now uses shared differences.
// MiniMax C (11-10): 286 -> 200 not kept: it came from a bool started false and set by if/else for the link test, the same
// bool reset and reused for the surface test, an origin pointer and two-arm start/end fallbacks; test-set flags are kept only
// with a byte match (lane 13 r63 held 16 with the same kind of flags). Retail keeps the edge flag in a byte at esp+0xf and reads
// it back at the join; ours folds the bit into the test.
// MiniMax D (11-08): no new form, no lower score (286); our frame is 0xe8 against retail's 0xe4, and in the scan
// retail keeps the link index in a register (ECX) and multiplies it by 0x44 while ours reloads it from a stack slot
// each time, so the two sides' frame slots sit four bytes apart; the first loop's block count and the scan's flags
// still differ.
// MiniMax R (11-06): forms tried, none lower (286; the permuter's best was 281 in the full image, below the eight-point gate and using a pointer to a local); the frame is 0xe8 here and 0xe4 in retail, and every frame slot differs between the two sides.
// M3 lane 2 r47: the projection through the existing dot3f(&offset, &edge_vector) 286 plain (kept, 1389 bytes: retail's k, j, i
// sum order; written out ours summed i first), dot3f(&edge_vector, &offset) 286. On 286 (fast): short link count 359 (1375);
// the node type named once 345 (1400); count zeroed after the init loop 359 (1391); `special` false then set by an if 287
// (1388 bytes); that with the short link count 362.
// Round r4 (lane 17): our extra frame word is the links cursor (strength-reduced long link count); retail indexes
// links[link_count] with a short count. Short link count 368 (1368 bytes, frame 0xe4); with count assigned after
// the init loop and special set false-then-true 389; count after the init loop 379; separate increment after the
// copy 369; shifted flag-seven bit in the link test 369 (folds back to test 0x80); split increment with
// false-then-true special 380; the distance test as a call of 0x210a30 553 (called out of line, caller 0x270930
// lost), with a forceinline declaration 553. Retail inlines 0x210a30 here (the delta block and its xmm1/xmm2/xmm0
// sums are that function's body, with 0x210850 inlined inside it); `inline` on its definition (diagnostic, reverted)
// inlines it here 447 at 1288 bytes but 0x210850 stays a call; inline on both 554 and loses 0x210a30/0x270750/0x270930.
// Earlier: Frame 0xe8 versus retail 0xe4; maximum/node acquisition, scalar homes, flags and vector scheduling differ.
// Earlier: the 0xe4 frame and state register now agree; initial load order, clearing, flag width and vector scheduling differ.
// Tried: pathfinding field at each use 459; bounded short initialization count 406; both 451; edge memset 408;
// all three 434; field reads plus memset 436; all three plus point-difference helpers 443; traversal state first 396;
// point-difference helpers alone/everywhere 398/398; depth-indexed step fields 385; direct data and step fields 372 (kept);
// that with a short initialization count 402; separate clearing loops 394; both 384.
// Signed/unsigned Boolean bit 380/372; capped word on unsigned/signed graphs 401/398; signed word/CRT
// clear 376; signed/CRT clear 393; entry counts 376; origin/target names 380; promoted cap 398, with CRT
// clear 377. All restored.
// Round 274: full-width initialization/link/path/scan carriers 400/363/378/414; initialization+link 401;
// link+path 441 (parent 282); path+scan 422; all four 407; previous-node presence 409, with initialization 433.
// Only the actual bounded full-width link count is retained: 363 plain, 1390/1388 bytes; parent stays 155.
// Round 248: real signed flag byte 370; signed/unsigned native flag-seven fields 363/363;
// signed flag field with link/complete entry initializers 360 (three-point gain restored), all
// constant traversal initializers 384, guarded edge memset 393, native capped word 372, all 412.
// Complete body and edge declarations restored; initial acquisition, clearing and vector homes remain.
// Round 250: actual advancing initialization receiver, short/long ordinal 349/401;
// safely capped word 374/374; separate edge memset 384/353, with word 384/386.
// Keep only the receiver with the original short ordinal: 349 plain, 1395/1388 bytes.
// Round 173: Native bounded word link count on the advancing initialization receiver, with geometry and
// history/receiver name subsets, 359 each at 1388 bytes. Independent traversal defaults at entry on those
// four forms 383 each at 1411 bytes, first +0x19. Geometry names canonicalize. Original long link count and
// full body restored.
// Round 187: Shared initial/traversal node receiver with capped-word/CRT-edge-fill/early-constant
// masks 0-7 350/374/384/384/372/343/396/411. Original advancing step receiver, link count and
// complete body restored.
// DH round 239: Native node/output link-record assignment, split increment, bounded
// word link count and full-width previous-index combinations 360/360/373/373/385/385/393/393.
// Original node declaration and complete body restored; frame acquisition and geometry homes remain.
// DH round 252: Real remaining-step short/long counts with advancing edge receiver, separate CRT edge fill,
// and their early traversal-constant forms
// 372/401/353/353/332/353/391/390. Keep trial 05, plain 332.
// DH round 254: entry edge/offset, delta and converted-point workspace subsets 315/332/315/332/315/332/315;
// all with a long step count 378. Shared differences at original scopes 315 plain, 1388/1388 bytes, kept;
// all entry-scope changes restored. Initial frame and indexed link transport still differ.
// DH round 257: Short/word link count with first/second point homes and both delta helpers, eight
// combinations 368 each at 1368 bytes; frame recovers but traversal worsens. Original body restored.
// Round 429: special predicate at use/completion byte/both/traversal phase scope/
// predicate-scope/byte-scope/all/all with long completion 358/314/350/315/358/314/350/369.
// One-point byte gains rejected; original clearing, traversal and geometry restored.
// Round 320: advancing link-output receiver with long/short/word link counts 368/368/368;
// long previous-index combinations 366/382/365; long path count with long/short link count 316/316.
// First forms recover frame 0xe4 but regress; complete original body and memcpy restored.
// Round 134 (m3 lane 4): both builds now return with ret 0x24 and take the state in EBX. Retail keeps one
// memory bool at [esp+0xf] for the edge flag-seven bit (movsx/shr 7/and al,1, stored, then je) and for the
// special test (stored false at the else arm's start, `mov al, 1; je` / `mov al, [esp+0xf]`), and its
// type-2 arm stores the type from cx, not the constant 2. Short link count 368 (1368 bytes, frame 0xe4);
// with one node variable set at entry and in the loop 368 (copy-propagated, the entry depth read still
// folds; retail does `lea` then `movsx [eax+0x3e]`); an inline node getter (diagnostic) 368; one flag for
// the link test (assigned in the condition) and the special test, false then true, 363 (retail's special
// sequence and complete/flag slots); with count zeroed after the init loop 371; the shared flag on the
// long link count 315 (1391 bytes). All restored.
// Round 37 (lane 10, fast): the convention already agrees (state in EBX, ret 0x24, caller 0x270930 MATCH).
// Retail keeps `previous` in EDI inside the loop (reloaded from its home on both loop entries), keeps
// a default-false `special` byte at [esp+0xf] that also receives the edge flag bit of the link test,
// and computes the start node with `lea` before reading its depth. Short link count 368 (1368 bytes,
// frame 0xe4); with the node pointer declared at the top and reused in the loop 368 (same code, the
// address still folds); with `special` false then set by an if 368; on the long count: link count
// incremented after the copy 359; count defined after the initialization loop 354.
// Round 106 (fast): both builds return with ret 0x24. The step point renamed to sort after start 315 (same
// slots); `point3f end, start;` 315; special declared at function scope before complete 315; the distance
// test through length_sq3f 315; special false then set by an if on the long count 331 (1373 bytes).
// Round 30 (lane 14, fast): a `s_type_c3b527 const *origin = &input->start;` for the delta block 334: the
// delta block and the edge-geometry loop then are retail's code (`lea esi, [ebx + 0x14]`, `push esi`, the
// step index in ESI), but the extra frame word shifts every slot by 4 and the post-loop gains a `jmp` into
// the scan loop; with the short link count 377 (1358 bytes, frame 0xe4; link count then takes EDI where
// retail keeps `previous`); with `special` also false then set by an if 383; origin at the scan-loop top 400
// (1418 bytes); origin shared with the previous-point ternary 386 (1407 bytes).
// Round 120 (m3 lane 5): both builds return with ret 0x24. On the short link count (368, frame 0xe4,
// 1368 bytes) the scalar slots are rotated against retail (ours count/previous index/previous at
// +0x10/+0x14/+0x18, retail previous/count/previous index) and the entry zero sits in EDX where
// retail's is `previous = NULL` in EDI. On that graph: `previous` renamed 368 and `count` renamed 368
// (same code: names do not move these slots), `previous` declared before `previous_index` 368 (same
// code), the node type named once (`short type = node->unknown10;` for every test and store) 349
// (1388 bytes, frame 0xe4; the special test then threads straight into the first arm); the named
// type on the original long-count body 354 (1395 bytes). Original body restored.
// Round 33 (lane 15, fast; ret 0x24 in both builds): retail's distance test is the existing 0x210a30 inlined
// (its shared `(k*k + j*j) + i*i` tail, with 0x210850 inlined inside it); calling 0x210a30 here stays a call
// (its file is /Ob1 and it is not inline) 524 (1228 bytes, ret 0x28, 0x270930 260), with `__forceinline`
// declarations of 0x210a30 and 0x210850 here 524 (ignored); declaring it inline in its header (diagnostic,
// restored) inlines it but calls 0x210850 404 (1348 bytes), both inline 530 (0x210a30 itself loses). Short
// link count with the type named once 349 (1388 bytes, frame 0xe4); on it count zeroed after the init loop
// 354, the link count incremented after the copy 349, `special` false then set by an if 349; local names
// (two placeholder orders, diagnostic) leave the slots unchanged.
// Round 62 (m3 lane 16, fast/plain): the link test's edge bit as TEST_FIELD_BIT((flags >> 7) & 1) 295 (kept, plain;
// ours `sar dl, 7; test dl, 1`, retail `movsx; shr eax, 7; and al, 1` stored to the special byte); the same through a
// dword cast 353, a long cast 295, without the bool 315; on the kept form: short link count 366 (1368 bytes, frame 0xe4,
// aligned 273 -> 210: retail's `inc dword ptr [esp + 0x1c]` with `cmp dx, word ptr [max]` and `movsx ecx, dx` is a short
// count), increment split from the copy 305, both 367, special false then set by an if 298, one pathfinding-data local
// for the whole function 392 (0x270930 loses its match), for the node loop 353; the previous-point ternary as if/else
// 315; special declared at the top 315; complete initialized after the init loop 321. Retail also loads the
// pathfinding pointer before the `unknown10 != NONE` test (`mov ecx, [ebx + 0x8c]` ahead of `jne`), as if an inlined
// link predicate took it as an argument.
// Round 60 (m3 lane 9, fast with 0x270930; the convention already agrees, ret 0x24): on the short link
// count, the type named once 349 (1388 bytes, frame 0xe4) and the one flag shared by the link test and the
// special test 362 (1371; aligned 189, the best structure seen), both together 349 (366 instructions and
// 1388 bytes as retail, aligned 246), with that flag declared in the loop 349 (same code); the named type
// and shared flag on the long count 369 (1405); `origin = &input->start` for the delta block declared after
// the margin 400 (1418, caller 0x270930 lost at 260); that on the short count with the type and flag 361
// (1374, aligned 175), with the type alone 387 (0x270930 lost). Retail also reads the start depth through
// a `lea` node address, keeps previous in EDI and stores the start/end conversion copies in swapped homes.
// Round 37 (m3 lane 14, fast, positional/aligned against 315/273): the short link count with the node
// type named once 360/411 (1395 bytes); that with `special` false then set by an if 351/266 (1388), on the
// long count 365/272 (1406); `special` declared false at the else arm's start on those 351/266 and 365/272;
// the short count with only that early `special` 368/216 (1368: retail's special sequence, best aligned);
// that with the link count incremented after the copy 369/214.
// Round 64 (m3 lane 16, fast; positional/aligned against 295/240, 0x270930 kept throughout): short link count
// 366/210 (1368 bytes); with the type named once 347/290 (1388); the type alone 344/311; short count with `special`
// false at the in-range arm's start and set by an if 363/190 (retail's special sequence); that with count declared
// after the init loop 371/211, with the type 349/293, with the increment split from the copy 367/214; early special
// alone 315/217, with the type 338/309. New lead: the link test as an inline predicate taking the pathfinding data
// first (a new helper: diagnostic only) puts retail's pathfinding load ahead of the `jne` (short + early special
// 363/214, alone 313/269); with a result variable in it (`if (type != NONE) return true; bool result = false; if
// (valid) result = bit; return result;`) retail's `and al, 1; mov [esp + 0xf], al` store into the special slot appears,
// 361/186 (1374 bytes); its plain/dword/long bit spellings 361/352/361, on the long count 355, with the late count
// 365, with the named type 351, with the split increment 360.
// Round 66 (m3 lane 16, fast; 0x270930 kept): the special test through a surface-test helper shared with 0x271630's
// case 2 295 (same code), with special false then set by an if 298; short link count 366 and with the increment
// split after the copy 367 (ours keeps the count in EDX and increments it before the store; retail keeps `previous`
// in EDI, uses EDX for the copy and increments the count in memory); the link copy as a struct assignment 305, on
// the short count with the split increment 367.
// Round 233: Node-array base and CRT memmove step/fallback point-transfer masks 1-7:
// 430/377/378/307/418/389/374; all with short link count 377. Every node-base graph loses matched
// caller 0x270930. Original complete body, link memcpy and signed bounds restored; frame
// 0xe8/0xe4, initial acquisition and memory flag homes remain.
// Round 70 (m3 lane 16, fast): retail's [esp+0xf] byte behaves like an address-taken bool (no constant propagation):
// `special` as an address-taken local, false at the else arm's start and set by an if (diagnostic) gives retail's
// special sequence exactly (`mov byte ptr [esp+0xf], 0` beside the count test, `mov cl, 1; je` / `mov cl, [esp+0xf]`)
// 315; that flag at function scope also taking the link test's edge bit, on the short count (diagnostic) 363 (1370
// bytes, aligned 150; the link-bit store is still dropped as dead). The type named for the tests only (stores read
// the field, for retail's type store from CX) 342; one store block for the two-step and type-2 arms with the call
// under `if (two_step)` 364 (1375 bytes); `previous` declared first 295 (same code).
// Round 311: Copy-owned short/word/long link ordinal, increment before/after memcpy,
// 367/366/367/366/363/305; short counter with short/long ordinal 366/366.
// Original counter, indexed memcpy and full body restored; frame and flag homes remain.
// Round 275: Truncation polarity bool/byte/short/long, each with long/short
// link count, 333/362/333/362/356/347/357/348. Original completion, counters and full body restored.
// DH278: Separate short/long traversal index crossed with short/long previous and bounded link carriers 347/396/380/392/383/384/384/386.
// Complete opening body restored; no qualifying keeper.
// Round 257: Actual geometry step/edge cursors and remaining paired distance-difference helpers,
// seven subsets, then full-width geometry ordinal: 341/367/296/378/341/367/378/378. All regress;
// original traversal, initialization, link memcpy and complete body restored. Frame
// 0xe8/0xe4 and node/flag/previous homes remain.
// DH280: Admitted-depth/previous-point/converted-coordinate branch ownership, seven subsets then all with
// bounded short link count [350, 332, 336, 297, 354, 345, 334, 366]. Original executable body restored.
// Round 73 (m3 lane 16, fast; positional/aligned, 0x270930 kept throughout; 295 stands): short link count 366/210 (1368);
// with special false at the in-range arm's start and set by an if 363/190 (1370); plus count zeroed after the init loop
// 371/211; count after the loop alone 371/230; short + early special + link test as an inline predicate with a result
// variable 361/186 (1374); that with count after the loop 365/207; a named initial node pointer 361 (same code);
// increment split after the copy 360/210 (1373); with `(dword)flags >> 7` in the predicate 359/208 (1374: retail's
// movsx/shr/and/store sequence); `previous = NULL` assigned after the declarations or after the clamp 359 (same code).
// Left on that form: retail keeps previous in EDI from entry (`xor edi, edi` shared with the count test and the link count
// store, stored to its home, reloaded after the fill and at the back edge) where ours sinks the NULL past the fill, and the
// slots run previous/count/previous index where ours run count/previous index/previous.
// DH283: Repeated depth captured as short, with parent/edge/both field captures
// 425/401/378/420; corresponding long depth forms 426/402/406/420.
// Original indexed memcpy, field observations, private helpers and complete body restored.
// DH296: Complete projection/displacement/shared-setter producer subsets
// 313/295/313/297/313/297/313; all with bounded short link count 354.
// Original parallel products, point stores and entire body restored; frame and flag homes remain.
// DH303: Real eight-byte node metadata record with memcpy/aggregate transport
// and long/short/int/word link counts 295/305/366/367/295/305/365/367. Original
// layout and body restored; extra output cursor, initial zero and flag homes remain.
// M3 lane 13 r54 (fast; 0x270930 kept): the edge array declared after the flags 295; the clamp as
// `maximum_steps < initialize_count` 295; the converted points named begin/end 295; the output-index compare
// swapped 295; margin declared at the top 295 (all same code). Retail's start/end conversion homes stay swapped.
// B383: Predecessor retained only as its short/long index, direct array fields or
// a branch-local multi-use receiver, crossed with long/short link count:
// 294/391/378/399/295/373/382/386. NOT kept: the 294 form gains one point; retail retains
// both predecessor histories; wide forms lose the matched caller. Original body restored; frame/flag homes remain.
// DH312: Native coordinate-array projection with long/dword/short/byte ordinals
// 295/295/335/335; with both complete distance-delta array producers 296/296/451/451.
// The two narrow complete-delta forms also demote the parent. Original scalar producers,
// indexed link memcpy and full body restored; frame/flags remain.
// Round 554: Pointer-only predecessor on retained fill/difference/edge graph, all eight
// bounded link/path/initialization carrier combinations 372/409/389/379/408/409/398/395.
// NOT kept: nonexact single-history forms; each loses matched parent to 251. Original
// two histories, counters, indexed memcpy and full body restored; frame/flag homes remain.
// M3 lane 7 r126 (fast, 0x270930 kept): the distance sum as `(j*j + k*k) + i*i` 286 (same code); the distance
// computed in each arm with 0x210a30's two sum orders 312 (1412 bytes); `origin = &input->start` for the delta block
// 354 (1391), with the short link count 358 (1375); the existing 0x210850 for both conversions 356 (1320, called out
// of line). Retail's delta block is 0x210a30 inlined with 0x210850 inlined inside it while its out-of-line 0x210a30
// still calls 0x210850; ours cannot inline it across the /Ob1 file (a file-flags question, left to the user).
// M368: Link-admission-only data receiver before/inside capacity, direct/result
// predicate, long/short count: 282/282/282/282/359/359/359/359.
// NOT kept: four-point lead is below the bar; word forms shrink away from retail.
// Original memcpy, observations and body restored; frame and flag homes remain.
// AC521: Existing shared scaled-point publication with conditional clamp/distance-difference/short-link
// masks 0..7: 286/286/283/283/359/359/359/359. NOT kept: three-point gain only; bounded forms
// recover frame 0xe4 but shorten away from retail. Original projection, loops, memcpy and body restored.
// AC530: One bounded word ordinal owns both capacity and link-copy indexing, signed/unsigned,
// node/function scope, increment after/before memcpy: 361/361/359/359/361/361/359/359.
// NOT kept: all regress at 1375/1388 despite frame 0xe4; original total, memcpy and body restored.
// AC532: Native signed clearing bound with indexed/edge-cursor/step-cursor/both
// increasing walks 286/286/286/282; bounded short-link counterparts 359 each.
// NOT kept: four-point best gain below gate, frame still 0xe8; original full body restored.
// Y576: Link-metadata library memmove with long/short/word post-increment counts
// 356/361/346; separate increments 356/361/346; copy-owned long/short ordinals 345/348.
// NOT kept: all regress. Narrow transports recover frame 0xe4 but initial/flag homes still differ.
// P325: Current-node pointer while traversal, separate/shared initial receiver and long/short
// links: 405/385/402/372; corresponding canonical pointer for traversal 405/405/402/402.
// All regress and lengthen to 1407-1423 bytes; original current-index traversal, both predecessor
// histories and metadata memcpy restored. Native frame/zero/flag homes remain.
// N367: Capacity-owned link eligibility reused for surface work, expression bool/byte/short/long
// 286/286/290/289; branch producers 359/359/370/366. NOT kept: no equality or gain; original predicates,
// flags and indexed memcpy restored. Extra frame word, predecessor and flag homes remain.
// M3 lane 13 r63 (fast, then plain; masked = aligned with registers and stack offsets ignored): retail's code is the plain combination of the short link count, `special` false at the
// in-range arm's start and set by an if, count declared after the fill and `origin` for the delta block 336 (1388 bytes, aligned 234 -> 111, masked 87 -> 34). On it: split increment 245; the
// link test as a forceinline predicate with a result variable 139, and as a local `usable` with a `data` pointer local 114 (no helper); `real distance` per arm through vector3d_from_points3d
// 195 (retail's tail in xmm0); node at function scope for the entry read 256, with parent first in the two-step arm 75 (retail's type in CX and `mov al, 1` special sequence), length_sq3f in
// each arm 65, complete before the link count 66, the clamp through a MIN macro of the shape the project's other files define (its two reads keep retail's `lea`, `movsx [eax + 0x3e]`; one read 66): kept, 16.
// Left: the start/end homes swapped, and two reloads at the scan loop's tail. `point3f points[2]` for start/end gives retail's homes, 2, match-only (groups locals); then only `mov edx, [esp + 0x10]`
// and `mov ebp, [esp + 0x104]` are in the other order. On that 2: indexed fill loop 270; fill pointers declared swapped 2; `end`/`start` declared swapped, renamed, or in other scopes (without the
// array) 195 (same code); extra field references to start 211 and end 246 do not move them; complete assignment first 4; link cast dropped 2; dot arguments swapped 2; j declared at top, after the
// edges or before margin 2; while loop 2; shared v 2; previous point if/else 2; point before edge 332; edge first 25; point, edge, previous 167; origin at the scan top 384; data pointer in the scan
// 397; long count 396; long j 197; result copy of count 2; adjacent declaration swaps 2-53. Unused locals, an unused getter call, `if (false)`, `do {} while (false)` before the special test: same
// code. Without parent first 236; `long parent_index` for it 16 (one-use). Named `type` 351. A permuter run was killed by its 2400 s timeout before printing anything.
// Lane 13 r63 (held, coordinator): the 16 rests on two test-set flags, `usable` built by if / else if before the memcpy and
// `special` false at the arm's start then set by an if; written as direct conditions the same body scores 338 (usable only
// direct 322, special only direct 130), so none of it is kept under the match-only rule (kept only with a byte match);
// build/held/lane13-r63-270d90-flags.patch is the whole body, a lead toward the match.
// AC534: Positive-domain initialization with actual step/edge end pointers, range/equality
// termination and separate/chained NONE stores: 413/413/380/380/393/393/407/407.
// NOT kept: all regress and lengthen; original counters, signed empty domains, indexed
// metadata memcpy and whole body restored. Extra frame word and predecessor/flag homes remain.
// AC536: Complete geometry-scan edge acceptance/rejection result, bool/byte/short/long:
// 282/282/282/282/282/282/282/282. NOT kept: four-point gain below eight and no equality;
// original scan gates, indexed library copy and full body restored. Extra frame word and
// predecessor/flag homes remain.
// Y592: Standard edge fill_n/fill before/after step clearing, short/long remaining steps:
// 297/383/404/423/382/400/309/309. NOT kept: native-frame range-fill forms regress in score/extent; original clearing, metadata memcpy and body restored.
// F389: CRT memcpy destination/predecessor/paired conversion-fallback point transfers,
// seven subsets 286 each; all with native short link count 359. NOT kept: no gain;
// original aggregate transfers, metadata memcpy and complete body restored; frame/flag homes remain.
// B445: Actual signed link capacity consumed at successful copy, long/short totals,
// indexed/advancing receivers and consumption after/before memcpy: 331/331/344/350/359/335/344/350.
// NOT kept: all regress; original total, limits, memcpy and body restored. Frame/flag homes remain.
// DH365: Original-site first-predecessor outcome, bool/byte/char/signed-char/short/word/int/long:
// 282/282/282/282/282/282/282/282. NOT kept: four-point lead below eight; both predecessor histories,
// original point copies, link memcpy and full body restored. Frame/zero/flag homes remain.
// N376: Paired conversion success/fallback outcomes, independent/shared carriers and
// expression/branch producers: 282/282/282/305/282/282/282/305. NOT kept: four-point leads
// fall below the margin and no form matches; original conversion graph/body restored.
// AC558: Complete two-step admission/rejection truth in bool/byte/short/long:
// eight at 333, 1388 bytes (control 286/1389). NOT kept: all regress; original
// type-2 fallback, metadata stores, long link total and complete body restored.
// Y605: Complete transition-step pointer/reference, branch-owned/shared admitted metadata,
// original long then bounded short link total: 375/375/385/385/392/392/391/391.
// NOT kept: all regress and shorten away from retail; original indexed fields and body restored.
// F397: Geometry previous-point/edge/output object references, seven subsets and complete endpoint
// references: 282/286/286/282/282/286/282/282. NOT kept: best gains four only; original
// receivers, memcpy and body restored. Extra frame word and predecessor/flag homes remain.
// F401: Complete three-state transition publication/call result, expression byte/short/long/dword
// and complete-arm producers: 350/346/344/344/378/368/368/368. NOT kept: all regress and no equality; original
// metadata stores, calls and full body restored. Extra frame word and predecessor/flag homes remain.
// DH377: Integer-index for traversal, parent update through the retained node/embedded array,
// long link total 286/374; bounded word total 359/344; readonly node/predecessor counterparts
// 286/374/359/344. NOT kept: no gain; original while, both histories, metadata memcpy and body restored.
// Extra link cursor, frame 0xe8 versus 0xe4 and zero/flag/geometry homes remain.
// M3 lane 11 r71 (fast, plain where marked; masked = aligned with registers and stack offsets ignored). Rebuilt the
// flag
// form (short link count; `special` false at the arm's start and set by an if; `usable` by if / else if with a `data`
// pointer; count after the fill; `origin`; split
// increment; per-arm distance through vector3d_from_points3d and length_sq3f; node at function scope; parent first in
// the two-step arm; MIN clamp; complete before the link
// count) 16 at 1388 bytes, masked 0. On it: each conversion in 0x210850's own nested shape (`if (x != NONE) { if
// (!convert) copy; } else copy;`) first only 16, second only 23,
// both 2 with no array: retail's start/end homes come from the two later cross-jumped copies; the fill as a plain
// `for (short i = 0; i < n; i++)` over `edge_indices[i]` and `steps[i]` 2, same code. Plain: 2 at
// 1388 bytes, the file's other matches unchanged. Left only `mov edx, [esp + 0x10]` and `mov ebp, [esp + 0x104]` in
// the other order. NOT kept: this rests on the two test-set flags (match-only);
// direct conditions on it: usable as an expression 322 (masked 4), usable in the `if` 326, special direct 130, both
// 338/342. Clean parts alone on the body below: short-count fill 322, long-count fill 380, nested conversions 327,
// both 422.
// On the 2: other fills long index 38, two loops 385, memset plus loop 384, guarded do-while 367, while with pointers
// 2, short count with pointers 382, steps stores first 2; edge index at each use 2, read twice beside the local 175;
// `count - 1 > j` 4; nested distance test 2; `!(d <= 1600)` 45;
// depth test inverted 132; `!count` 2; type-2 arm nested 2; vertices nested 2; clamp ternary 2; `projection < margin`
// 2; output index compare swapped 3; arm `continue` 2; edge test inverted with `continue` 2; origin output local 2;
// far/valid/vertices/length/same as threaded flags 2 each; usable default true 2; usable nested 2;
// usable no initializer with else 321; usable set only by `if (bit)` 322; `usable == true` 2; special nested 2;
// special set both ways 214; surface test as an inline helper in the condition 2, assigned 12/248; count arm cast 2,
// `count = j; count++` 2; previous stores swapped 4; the two tail stores in both depth arms 383 (loses 0x270930);
// clamp limit named 2; empty vertices arm 2.
// Diagnostics, not kept: dead pointer chains 1-7 and 9-24 links leave the pair (8 links flip only the dot operands,
// 12; at the function's start 210, 8 or 16 links); `do { } while (false)` regions around the delta block, each
// conversion, both, the projection and the arm 2 each; 0x210a30 and 0x210850 as temporary forceinline copies 2, same
// code, as plain inline 539. Any new `if` with a body (on j, count, margin, edge index or link count), anywhere in
// the first loop or the scan, flips the pair to retail's order (with its own new lines),
// unless it reads `steps`; one between or after the loops does not; statements with no branch do not; the threaded
// flags above do not. So the pair follows a block count inside a loop, and retail has one more block there that is
// gone after allocation. A 600 s permuter run on the 2: 55 tries, best 2.
// B453: Standard initial-count and lower/upper/complete projection clamp producers, individual and joint
// boundaries: 384/351/342/338/429/413/409/410. NOT kept: no byte match; original complete body restored.
// Native frame, indexed link transport and predecessor/flag homes remain.
// AC558: Complete two-step admission/rejection truth in bool/byte/short/long:
// eight at 333, 1388 bytes (control 286/1389). NOT kept: all regress; original
// type-2 fallback, metadata stores, long link total and complete body restored.
// Y605: Complete transition-step pointer/reference, branch-owned/shared admitted metadata,
// original long then bounded short link total: 375/375/385/385/392/392/391/391.
// NOT kept: all regress and shorten away from retail; original indexed fields and body restored.
// F397: Geometry previous-point/edge/output object references, seven subsets and complete endpoint
// references: 282/286/286/282/282/286/282/282. NOT kept: best gains four only; original
// receivers, memcpy and body restored. Extra frame word and predecessor/flag homes remain.
// F401: Complete three-state transition publication/call result, expression byte/short/long/dword
// and complete-arm producers: 350/346/344/344/378/368/368/368. NOT kept: all regress and no equality; original
// metadata stores, calls and full body restored. Extra frame word and predecessor/flag homes remain.
// DH377: Integer-index for traversal, parent update through the retained node/embedded array,
// long link total 286/374; bounded word total 359/344; readonly node/predecessor counterparts
// 286/374/359/344. NOT kept: no gain; original while, both histories, metadata memcpy and body restored.
// Extra link cursor, frame 0xe8 versus 0xe4 and zero/flag/geometry homes remain.
// P394: Complete terminal-distance truncation admission, positive/complementary
// bool/byte/short/long results: 282/282/282/282/282/282/282/282. NOT kept: four-point leads are below eight,
// and one-use outcomes require equality. Original distance test and full body restored.
// Extra link-cursor frame word and flag homes remain.
// Round 163 (m3 lane 5, fast): lane 11 r71's flag recipe rebuilt on the current file 2 (1388 bytes), not kept (the
// two test-set flags need a byte match). Retail's last pair is in the order the loop head uses the values (j, then
// steps), as 0x113980's is (state, then tag table); ours is the reverse in both. On the 2: the step's point pointer
// declared at the loop head ahead of the edge index 74 (the steps reload moves to the loop head); a `&steps[j]`
// pointer at the loop head with the point taken from it 178; the previous point chosen at the loop head 332 (1389);
// link admission without the flag, the copy written in both admitting arms 352 (1407 bytes: the copies do not
// merge, frame 0xe8); link admission as one direct condition re-measured 322 (frame 0xe8); the link test as a
// plain inline predicate with a single-exit result, called in the condition, 2 (same code as the flag: with lane
// 11's inline surface test the body then holds no flag, but both are new helpers with one caller). Body restored.
// M3 lane 13 r72 (fast; 0x270930 kept; both builds return with `ret 0x24`, the frame is 0xe8 against 0xe4). Lane 11
// r71's flag form rebuilt from the r63 held patch with both conversions nested and the pointer fill: 2 again (the same
// reload pair). Its indexed fill with the long bound (`i < initialize_count`) 263. On the 2: an inline node getter for
// the three node lookups 2 (same code; with it the plain clamp was not measured); the link test as an inline predicate
// with a result variable called in the `if` 12 (moves the projection's xmm3/xmm4 homes, pair unchanged), with direct
// returns 321; both with the surface test as a second inline predicate 130/337; `edge_index != NONE &&` ahead of the
// range test 182 (1397 bytes), as `if (edge_index == NONE) continue;` 182; `if (node_index == NONE) break;` at the
// traversal's end 2 (same code). NOT kept: none matches and the flag form is match-only; body below unchanged.
// M3 lane 14 r79 (fast; 0x270930 kept; both builds end in `ret 0x24` here): lane 11 r71's flag form rebuilt (usable/special
// set by ifs, short link count, MIN clamp, nested conversions, per-arm distance) 2 again (`mov edx, [esp + 0x10]` and
// `mov ebp, [esp + 0x104]` at the scan's join in the other order). On that 2: a two-step bool set by an if 62 (arms laid out
// again); a first-step bool set by an if 2; the traversal as `for (; node_index != NONE; node_index = node->parent)` 2; the
// edge-index record as a separate `if (previous_index != NONE && ...)` after the point copy 2; margin computed inside the scan
// loop 49 (not hoisted); re-measured on this base: valid/far flags 2, one flag for the link test and special 2. Diagnostics
// (removed): a neutral `if` with a body at the end of the traversal loop flips the pair to retail's order in the same empty
// join block (210 with its own lines); at the end of the scan body it takes both reloads into its block in retail's order (38);
// inside the edge block it leaves them (63); a plain copy of `steps` for the scan or for the traversal 2 (propagated away).
// So the order follows the traversal loop's block count at allocation: retail has one more block there that is gone afterwards.
// M3 lane 9 r105 (fast, 0x270930 and the file's other matches kept): lane 13 r72's flag lead rebuilt at 2 (re-measured; only
// `mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]` swapped at the scan loop's back edge). On it: one bool shared by the
// link test and the special test 2 (same code); `break;` after `count = j + 1; complete = false;` 335 (1390 bytes);
// `edge_index != NONE &&` after the range test 179 (1397 B, not folded); the traversal as `for (; node_index != NONE;
// node_index = node->parent)` 2 (same code); one `short j` shared by an indexed fill and the scan 2 (control with
// separate indices 2); the scan index named i, k, index, step_index, n, a or z and the steps parameter named
// path_steps, output_steps, a_steps, z_steps or route 2 each (same code); the 0x26f3f0 call's result tested by an
// empty `if`, either sense, 2 (same code: an empty body leaves no block); the margin computed inside the scan loop
// 333; the step point written as `steps[j].point` at each use instead of the `point` local 375 (1421 B). 0x113980
// shows the same swapped back-edge pair with every callee matched, so an unmatched callee is not the cause. On the
// body below (286): `break;` after the count update 312 (1391 B); the traversal as a `for` with `node` declared before
// it 286 (same code); the margin computed inside the scan loop 300; one `short j` for an indexed fill and the scan
// 380 (1400 B).
// M3 lane 12 r110 (fast; 0x270930 kept unless noted): lane 11 r71's flag form rebuilt from the r63 held patch with both
// conversions nested, 2 (the same `mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]` pair). On it: the scan's previous
// point as `&input->start` then `if (j > 0)` 280 (1396 bytes, 0x270930 lost at 260); the link memcpy as an aggregate
// assignment 2 (same code; re-measure of round 66's aggregate-assignment link copy on this base); the previous-node test positive with the arms swapped 281; `special = false` after the count
// test 366 (1374 bytes); count declared at the top with the other locals 349; the link copy moved out of the capacity
// block (usable declared before it) 2 (same code); one pathfinding-data local for the link and surface tests 353 (re-measure of round 62's node-loop data local,
// same number).
// M3 lane 11 r95 (fast with the file's functions, 0x210a30 and 0x2104b0; base 286): both builds end in `ret 0x24` with one
// register argument. The delta block written out here is 0x210a30's body (with 0x210850 inside); calling 0x210a30 stays
// out of line while its file is /Ob1 (round 33's 524), so BLOCKED-BY 0x210a30's file flags for the clean form.
// Round 71's flag form rebuilt on the current names (usable/special set by tests, short link count, MIN clamp, nested
// conversions, per-arm distance; diff saved as build/r95/lead-0x270d90-score2.patch in m3-lane11's worktree): 2 again,
// only `mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]` swapped in the scan loop's back-edge reload block (retail
// reloads j first, then steps, after ebp held the step's point). New on the 2: the previous point as an if/else 2;
// the traversal as `if (node_index != NONE) do {...} while (node_index != NONE);` 2 (same code each); the projected point
// through the existing vector3f_add_scaled 2 (same code). Re-measured on the 2 (listed above): special nested 2, usable
// nested 2, the edge test inverted with `continue` 2, the depth test inverted 132. On main, kept: the projected point
// through the existing vector3f_add_scaled in place of its three rows written out, 286 (fast and plain; the edge and
// offset vectors trade slots, the rows' registers rotate, the count holds; 0x270930 and the file's matches kept).
// MiniMax A (11-13): no new form, no lower score (286): our frame is 0xe8 against retail's 0xe4; the extra dword is the `previous`
// pointer, set to NULL here and left alone there (dropping that initializer gives retail's 0xe4 frame and the first six instructions
// agree, but the body grows by two bytes and the score rises to 357, so it is not kept); both sides pass nine stack arguments in the
// same slots; the node depth is read at one address here (node*0x44 + 0xee) and through a lea there.
// M3 lane 9 r126 (fast with 0x270930, which stays matched): lane 11 r71's flag form rebuilt on the current body (keeping
// vector3f_add_scaled) is 2 again (only `mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]` swapped at the scan's join).
// On that 2: the clamp through the project's PIN macro shape 2 (same code); the clamp as two independent tests 112; the
// vertices test as two nested ifs 2; the length test as `margin * 2.0f < length` 2 (same code each). Diagnostics on the 2
// (removed): dead locals 1-7 at the top 2/210/210/210/210/210/210, 1-4 before the scan 2 each (not a symbol phase); an
// `if` with a body at the traversal's end gives retail's pair order (207 with its own lines), one between the loops does
// not (202), as round 71 found. 0x26f3f0 (unmatched) writes ECX and EDX and makes calls in both builds, so its LTCG
// clobber summary is not the cause. Body unchanged.
// MiniMax H (11-18): no new form, no lower score (286): retail's frame is 0xe4 with the state in ebx and nine stack
// words, ours 0xe8 with one local word more: the link output goes through a pointer that advances by eight (ours)
// where retail indexes the array with the count (0x270e7f-0x270e96). A link count of 16 bits gives retail's frame,
// its 16-bit compare and index (0x270e3f, 0x270e86) and retail's length, but scores 359; copying the two
// layout-identical views as one assignment gives retail's indexed copy, but scores 361. Still differs: retail
// materialises the node address with a lea before the depth load (0x270db1) where this build folds the whole
// displacement into one load; retail keeps the link-flag byte in memory (0x270e79, 0x270eb8, 0x270f61) where this
// build holds it in a register; retail zeroes the step count at the loop entry (0x270e16) where this build zeroes it
// in the prologue; retail reloads the previous node at the loop head (0x270e07, 0x270e22) where this build reloads it
// in the body. Original body restored.
// Later in the same round: giving that flag a memory home and clearing it in the depth branch reproduces retail's
// flag sequence (movsx, shr, and, the byte store) and its merge into the type test, but the extra copy decision
// costs 67 (353) and duplicating the copy costs 93 (379); a 16-bit link count on its own scores 359 and the
// retail-shaped root walk of 0x14df40 scores 109 against 43. Original body restored.
// MiniMax S (11-16): no new form, no lower score (286): retail carries state in ebx through a custom convention and stacks the other nine as (result, actor_index, node_index, maximum_steps, steps, complete_out, link_count_out, links, maximum_links), where this build passes actor_index first with all ten on the stack, so every parameter home sits in another slot and the frame is 0xe8 against retail's 0xe4; retail also materialises the node address with a lea where this build folds the whole displacement into one load, and a pointer to the node and a named node pointer compile the same way. Changing the parameter order is a form the rules forbid, so retail's order is reported instead.
// MiniMax C (11-16): no new form, no lower score (286): retail still carries the state in ebx through a custom convention and stacks the other nine as (result, actor_index, node_index, maximum_steps, steps, complete_out, link_count_out, links, maximum_links) where this build passes actor_index first with all ten on the stack, so every parameter home sits in another slot and the frame is 0xe8 against retail's 0xe4; retail's parameter order differs and changing a parameter order is a form the rules forbid, so retail's order is reported instead.
// MiniMax I (11-17): no new form, no lower score (286): retail carries the state in EBX through a custom convention and stacks the other nine as (result, actor_index, node_index, maximum_steps, steps, complete_out, link_count_out, links, maximum_links) where this build passes actor_index first with all ten on the stack, so every parameter home sits in another slot and the frame is 0xe8 against retail's 0xe4; retail also materialises the node address with a lea where this build folds the whole displacement into one load, and changing the parameter order is a form the rules forbid.
// MiniMax I (11-17): no new form, no lower score.
// b7I (11-17) walk against retail, fast build on r830: walked every block of 0x270d90 against retail statement by
// statement (field offsets and strides, branch senses, callee and register arguments, loop shapes, local widths and
// the float constants) and found no behaviour difference; the body is unchanged and the score stays 286 (1389
// bytes, caller 0x270930 MATCH). Checked and agreeing: the entry depth, min and fill (0x270db8-0x270dc3,
// 0x270de0-0x270e05: edge words, step +0, +8 and +2); the link test (0x270e50-0x270e7d: type NONE, else edge index
// in range and flag bit 7); the 8-byte link copy (0x270e7f-0x270e96); depth gate (0x270e9e) with count
// (0x270eb2-0x270ec2); step node, point and edge index stores (0x270ecf-0x270f3a); the special test and arm order
// 1, 6, 5, special, 2 (0x270f42-0x270f7f); the 0x26f3f0 call (eax pathfinding, ecx index, edi entry point, seven
// stack words from actor_index to the step node, 0x270fa3-0x270ffe); margin 1.2f (0x271031), 2.0f (0x271137),
// 1600.0f (0x271286); clamp senses (0x271186, 0x271194); k, j, i sum order (0x271166-0x27117c); the two
// conversions; the distance test before j < count - 1 (0x27128f-0x27129c); the outputs (0x2712c9-0x2712f9). Only
// width differences, not observable: retail compares, indexes and stores link_count as 16 bits (0x270e3f, 0x270e86,
// 0x2712e8) while incrementing a dword (0x270e96), and its vertex words are widened then compared with -1
// (0x27109b, 0x2710ac: never equal), as the body already does.
// M3 lane 15 r97 (fast with the file's 16 functions, 0x210a30, 0x2104b0, 0x26f3f0, 0x30bf0, 0x210850, 0x2c2060; base 286, LCS-aligned
// 134): lane 11 r71's flag form applied to the current file from its saved patch is 2 again (aligned 1: only
// `mov edx, [esp + 0x10]` / `mov ebp, [esp + 0x104]` swapped at the scan's back edge). On that 2, looking for the block
// retail has and ours lacks: the cross-link step (the two stores and the 0x26f3f0 call) as an inline helper called from
// the 1/6/5 arm and from the type-2 arm when its surface test passes, the plain type-2 stores in the else 353 (1388
// bytes: the two copies merge, but the frame is 0xe8 and the traversal's homes move); a step pointer for the cross-link
// arm 263 (1368 B), for the type-2 arm 230 (aligned 10), for both 264, for the current step's two stores 299 (1372 B);
// the edge projection block as an inline helper taking the edge pointer 601 (stays a call), the edge index 610,
// force-inlined 332 (1391 B) and 398 (1423 B, loses 0x270930). Plain, in the full image, the flag form is 2 as well.
// The same kind of adjacent reload pair in 0x24cf66 (another file, no shared callee) swapped this round when a helper
// was removed from src/unknown_223240.cpp, so such pairs follow whole-image state as well as this body. Body unchanged.
// Carried start node with pointer or indexed predecessor, pointer-tested for update, and index-tested carried node:
// 343/385/343/374 with long link count, 387/372/387/315 with short link count.
// NOT kept: pointer-tested predecessor omits the retail predecessor-index check and shortens the body.
// The index-tested form at retail length still has the extra frame word. Original node traversal restored.
// M3 lane 1 r122 (fast with 0x270930, 0x270750, 0x210a30, 0x2104b0, all MATCH unless noted, and 0x26f3f0 at 218;
// base 286): lane 11 r71's flag form rebuilt on this body is 2 again at 1388 bytes (the diff is saved as
// build/r122/lead-270d90-score2.patch in m3-lane1). The pair sits in the edge block from the scan body to its latch
// (0x2712ac, 0x2712b0): both are reloads after the body's calls, and the paths that skip the body jump past both to
// 0x2712b7. On the 2: the state as `this` (the function as a member, called on the state) 210 (aligned 14: the pair
// stays swapped, the type-2 arm stores a literal 2 and the surface test's loads change); the loop step written at
// the end of each arm of the depth test, or in the overflow arm with `continue`, 383 (1425 bytes, two copies kept,
// 0x270930 260); the two step assignments swapped 4; the type-2 stores as a separate `if (type == 2 && !special)`
// 258 (1400 bytes); the link copied by assignment 2; the count update after the node index store 294; the scan as a
// while with its own increment 2; the edge test inverted with `continue` 2 (same code each at 2). Diagnostics on
// the callee 0x26f3f0 (src/path_transitions.cpp, restored at once): one or two blocks added at its top through its
// parameters 382/236, one to three register-free blocks on two undefined globals 236 each (1391 bytes, aligned 46,
// the callee 385-421): this function's allocation follows that unmatched callee's code well beyond the pair, so
// the pair may not be decidable here while 0x26f3f0 differs. BLOCKED-BY 0x26f3f0 (not shown to be the cause).
// M3 lane 13 r117 (fast with the file's 16 functions, 0x210a30, 0x2104b0 and 0x26f3f0; the file's other functions
// MATCH each time): the two-flag form rebuilt from the saved lead on the current file 2 again, fast and plain. On that 2:
// one permuter run (fast, annealed, 84 tries in 905 s after a 112 s opening build) none below 2 (8 variants at 2,
// 10 at 4); the traversal's two predecessor stores under `if (node_index != NONE)` 358; the traversal written
// `if (node_index != NONE) do { } while (node_index != NONE);` 2 (same code); the whole scan in a new forceinline
// inner function taking steps by value 189 (1395 bytes); the scan loop's body in one called per step 118 (1389
// bytes); the scan through its own copy of the steps pointer 2 (same code, as listed). Body unchanged (286).
// M3 lane 2 r110 (fast with 0x270930/0x270750, matched; base 286; both builds return with `ret 0x24`): the two-flag lead applied
// from m3-lane1's saved patch is 2 again at 1388 bytes (the same reload pair). On it: the scan index pre-incremented 2 (same
// code); the scan condition as `count > j` 4; the step's point pointer declared ahead of the previous point and the edge 332,
// the edge first 25; a named `&steps[j]` pointer with the point taken from it 2 (same code); the truncation test nested with a
// named `long last = count - 1` 2, as `if (!(distance > 1600.0f)) continue;` then the index test 2 (same code each); `real const
// margin` 2 (same code); the delta block as a call to the existing 0x210a30 with `inline` on its /Ob1 definition 428 (1307
// bytes: it stays a call; 0x210a30 and its six matched callers were listed, its file restored). Body unchanged.
// The extra link-cursor word and flag homes remain. Short count and two flags with nested
// conversion fallbacks 398. Start, end or both conversion results taken from the conjunction
// 400/400/398, guarded success 400/400/398, guarded fallback 414/414. Later surface test
// 396, also with the widened edge bit 396, then with one transition type read 332.
// NOT kept: the 0xe4 forms lose the matched caller. The single type read recovers that
// caller and restores the extra frame word. None matches or improves the opening score.
// m3 lane 11 r130 (fast with 0x270930, 0x270750, 0x210a30, 0x2104b0, 0x26f3f0; both builds end in `ret 0x24`): lane 1
// r122's saved two-flag lead applied to this file is 2 again (the latch's two reloads, j from [esp + 0x10] and steps from
// [esp + 0x104], in the other order). Retail's traversal latch stores `previous` into [esp + 0x10], the slot the scan's j
// reuses, and previous_index into the margin's slot. On the lead: j declared at function scope, just before the margin
// and just after it 2/2/2 (same code); the fill loop as main's pointer walk with the if clamp 235; `previous` without
// its NULL initialiser 378 (1372 B).
// M3 lane 16 r154 (fast with 0x270930 and 0x210a30, both MATCH; both builds end in `ret 0x24`, frame 0xe8 against 0xe4):
// lane 11 r95's flag form re-applied to today's file 2 (only `mov ebp, [esp + 0x104]` / `mov edx, [esp + 0x10]` at the
// scan loop's back edge in the other order; build/r154/lead-270d90-score2.patch in m3-lane16). On that 2: the traversal
// reading its node through a second pointer local (`current`) instead of reassigning the entry `node` 210 (+0x8c); `usable`
// declared at the top of the traversal body, outside the capacity test, 2 (same code); the step point copied from a
// ternary-selected source with the edge-index record as its own `if` 388 (1357 B); the projected point written as three
// rows in place of vector3f_add_scaled 2 (same code). Body unchanged.
// scan completed-step count 287, distance-gated completed-step count 287, bounded link ordinal and scan
// count 366, step frame across conversion 332, indexed fill with completed-step count 295, indexed fill
// with completion update first 295, coordinate scope before scan latch 295, predecessor edge before point
// 353, late surface with indexed fill 343, wide edge shift with indexed fill 290, late surface and wide
// edge shift 344.
// The 0xe4 prologue returns with indexed fill, but the completion flag still uses +0xf and the body grows.
// The opening short scan and long link ordinal are restored.
// M3 lane 16 r158 (fast with 0x270930, 0x210a30 and 0x26f3f0, which now MATCH, so the r122 BLOCKED-BY 0x26f3f0 reading is
// gone; both builds end in `ret 0x24`): the two-flag lead re-applied from build/r154 is 2 again (only `mov ebp, [esp + 0x104]`
// / `mov edx, [esp + 0x10]` at the scan latch). On that 2: the fill split into an edge loop then a step loop 385 (1405 B),
// a step loop then an edge loop 368 (1404 B); the latch's `previous` stored before `previous_index` 4 (the two stores
// swap, the pair stays); const on lookup/input, on the margin, on the origin pointer or on the link data pointer 2 each
// (same code); the edge index read as `edge_indices[j]` at each test 2, and for the edge lookup too 2 (same code: extra
// references of j do not move the pair); the previous point chosen after the step point 164; an exit test
// `if (node->parent == NONE) break;` ahead of the latch statements 344 (1373 B), between them 344. Diagnostic (a probe
// global, reverted): a test-and-store between the depth test's join and the latch statements gives retail's pair order
// (312 with its own lines); after the latch statements, at the traversal's top or in the fill loop it does not.
// Also on the lead: both step point copies as memcpy 2, only the previous-node copy 2, only the destination copy 2 (same
// code each); the parent node pointer formed after the two cross-link stores (main's order) 237 (1392 B).
// M3 lane 8 r204 (fast with 0x270930, MATCH): m3 lane 16's saved two-flag lead re-applied to today's file 2 again (the same
// back-edge pair). On it: one `short i` shared by the fill loop and the scan loop, declared before the fill loop or at the
// top 2/2 (same code); the scan index renamed (six names) 2 each (same code); an element pointer for the two-step arm's
// `steps[node->depth - 1]` 264 (1366 B), for `steps[node->depth]` 299 (1372 B), both 389 (1358 B). Body unchanged.
// The extra link cursor, predecessor homes and scan-latch reloads still differ.
// Short-link test-set form with predecessor pointer alone/ordinal alone/unsigned word/parent first
// 409/297/399/396; scan previous point/current step/both/previous ordinal 366/430/193/230.
// Common flag and short-link copy 209; increment after copy, signed edge test, parent early and
// signed edge with split increment 300/210/326/273. Direct link/both predicates/both with parent early 211/339/353.
// NOT kept: the lower scores retain test-set flags, and the 193 form adds six bytes.
// Earlier note. frame 0xe8 against retail's 0xe4 (the extra dword is the `previous` pointer's home), the link
// count compared as a dword where retail compares it as a word, and the node depth read at one address here and
// through a lea there.
// m3 lane 4 r238 (fast with 0x270930, 0x270750, 0x210a30, 0x2104b0, 0x26f3f0, all MATCH; both builds end in `ret 0x24`
// in today's full build too; control 286): lane 16's saved two-flag lead on today's file 2 again at 1388 B, fast and
// plain (full image), the same back-edge pair (`mov ebp, [esp + 0x104]` / `mov edx, [esp + 0x10]`). In the scan block
// j's register is taken first (the previous point's address in EDX) and the steps' second (the step's point in EBP);
// retail reloads them in that order, ours in the reverse. On the 2: the link flag as one direct bool expression 322,
// the surface flag as one direct expression 130, as a ternary 188, both direct 338 (all at 1388 B with other homes:
// only the test-set flags give the 2); the link flag as an if / else-if / else chain with an explicit false arm 321;
// the parent read into a named short ahead of the latch's assignment 2 (same code). Diagnostics, reverted: one to
// three dead initialised locals in 0x270750 (earlier in the file) or one or two in the caller 0x270930 leave the
// pair as it is (2 each), so the pair does not follow the file's other functions' symbols.
// Word-count reconstruction with a retained edge index 396, signed flag 396, joined edge predicate 384,
// early parent read 396; completed-step count 390, nested long 394, nested short 343, short guard first 345.
// NOT kept: seven forms recover frame 0xe4, but all grow and score higher.
// Link-target reference before capacity, inside capacity, shared with step metadata, or with scoped edge admission:
// long count 286/286/286/357; short count 359/359/359/356.
// The short forms recover 0xe4, with the scoped admission at retail length, and all raise the score. Body restored.
// Earlier note: frame 0xe8 against retail's 0xe4 (the extra dword is the `previous` pointer's home), the link
// Initial node reference and ordinal-only predecessor, long/short link counts with fallback branch
// 287/396 or selected point and separate edge test 360/362. Pointer-only predecessor in the same
// forms 365/408 and 414/443. NOT kept: ordinal-only recovers the frame in three forms but raises
// the score. Pointer-only forms also raise the caller score.
// M3 lane 4 r240 (fast with 0x270930, 0x270750, 0x210a30, 0x2104b0, 0x26f3f0; control 286; the two-flag lead rebuilt 2):
// on the 2, the truncation as a nested test 2, the surface test nested in the type-2 test 2 (same code each), the edge
// record ahead of the point copy 136, an `else if (true)` conversion arm 2 (folded). Retail's only cross-jumped copy tails
// in the loops are the two conversion copies the lead already nests, so the extra vanishing block is not another copy.
// M3 lane 15 r123 (h2m fast screen with 0x270930 and 0x270750, MATCH each time, and 0x210a30, 0x2104b0, 0x26f3f0 MATCH on the
// first runs; both builds end in `ret 0x24`). Lane 16's saved two-flag lead applied to today's file is 2 again (build/r123/
// lead-270d90-score2.patch here): only the two reloads ahead of the scan latch, ours steps ([esp + 0x104]) then j
// ([esp + 0x10]), retail j then steps. The pair is not whole-image state: one to three unused globals, dummy functions
// or extra declarations ahead of the function in this file 2 each (same code). On the lead: the traversal as
// `for (; node_index != NONE; node_index = node->parent)` 2 (same code); a probe test-and-store on a new global ahead of
// the traversal's latch statements (diagnostic) 210 with its own lines, the pair unchanged; one dead initialised long
// at the function top 2, two to eight 210 (the type-2 arm then stores a literal 2 and the surface test's loads change;
// the pair unchanged), one to eight ahead of the scan 2 each (same code); the step point declared second of the three
// scan pointers 2 (same code), first 332 (the latch then compares j against the count in memory), the edge first 25 (the
// edge lookup's order changes, the pair unchanged), the edge then the point then the previous point 164; j declared
// first at function scope 2, with a copy of the steps pointer declared after it for the scan 2, the copy ahead of j 2,
// one copy at the function top for all three loops 2 (same code each: the copies are propagated). Nothing found that
// flips the pair. Body unchanged (286).
// Bounded indexed fill through the first node 282, with short link count and
// split copy increment 361, or target record lvalue 361. Short fill with link
// and special flags 357, shared origin 404, relative scan step 321, held scan
// successor 329, held node depth 432, or arm-local distance producers 401.
// NOT kept: 282 gains four with the extra frame dword. Several narrow-link
// forms recover 0xe4, and two reach retail length but regress.
// M3 lane 8 r219 (h2m fast with 0x270930 and 0x270750, both MATCH; control 286; both builds end in `ret 0x24`). Lane 16's saved
// two-flag lead applied to today's file is 2 again (the scan latch reloads EBP then EDX; retail EDX then EBP, ascending, as in
// 0x254490 and 0x2548f0). On the lead: one vector for both distance arms 2; `real distance` after the origin pointer 2; the
// margin declared at the top 2; `special` declared at the loop head 2; the edge and offset vectors declared together 2; the
// distance test nested 2; `++j` 2; the usable flag at the loop head 2; the link copy as an aggregate assignment 2 (same code each);
// the parent pointer ahead of the type chain 351 (1400 B); count declared at the top 348; the latch with `previous = node;` first
// 4 (the two latch stores swap: retail keeps the source order); previous from the lookup by index 2; the parent read through
// `previous` 2; the parent through `lookup->nodes[node_index]` 347 (1404 B); the depth test inverted 132; the link copy with
// `link_count++` in its index 304; special with an explicit else 130; `j + 1 < count` 5 and `count - 1 > j` 4 (the compare
// operands swap, the pair stays); the data pointer at the loop head 329; count as a ternary with a self-copy arm 288 (pair
// unchanged); the previous point as an aggregate ternary 388 (1357 B); special as `? true : false` 130. Diagnostic: the scan
// index renamed with 2 to 128 characters 2 each (no name-length effect). Body unchanged (286).
// Signed-word link ordinal with guarded admission 357, exhaustive admission
// 361, or a shared admission and transition target 361.
// With a visit-owned predecessor 367, one reconstruction depth 421, or an
// entry node receiver 361. Traversal-lifetime count with long/short link
// ordinal 341/373. NOT kept: the forms regress even when the frame is 0xe4.
// The entry depth load still folds and the predecessor/flag homes differ.
// Traversal predecessor/count names, edge-vector names first, converted-endpoint names
// first, and separate scan/history names with long link ordinal 286/286/286/286,
// short ordinal 359/359/359/359. Names leave frame and allocation unchanged
// at each width. The short ordinal removes the cursor but shortens to 1375.
// M3 lane 13 r140 (H2M_FAST with 0x270930, MATCH each run; control 286; both builds end in `ret 0x24` with the state in EBX,
// so the round prompt's `ret 0x28` was stale). Lane 15 r123's saved two-flag lead re-applied 2 at 1388 B (the scan latch's
// steps/j reloads in the other order). On it: a redundant `if (count > 0)` guard on the scan loop 187 (1395 B, the test is
// emitted); `if (node_index != NONE)` around the traversal `while` 2, around it as a `do ... while` 2 (same code each: the
// guard block folds before allocation); the truncation test written in both distance arms 488 (1489 B, the tails are not
// cross-jumped and the state leaves EBX); the truncation storing `complete` before `count` 4 (the two stores swap, the pair
// stays); the previous point chosen by an if/else statement in place of the ternary 2 (same code). Body unchanged.
// M3 lane 16 r173 (H2M_FAST with 0x270930, MATCH each run; control 286): the r154 two-flag lead re-applied to today's
// file 2 again (the same back-edge pair). On it: `special ||` tested first in the type chain 341 (1372 B); the special
// test as a one-case `switch (node->target.type)` 79; the scan's count read through `short const &` 2; the scan's steps
// read through `s_path_step_view *const &` 2 (same code each); the truncation as `count = ++j;` 35 (1392 B). Body unchanged.
// Short output ordinal and indexed fill with split negative/range/sentinel guards 391/391/391;
// short capped fill bound with the same guards 396/396/396. Separate seed node 390,
// shared edge-table receiver 391. NOT kept: frame 0xe4 returns but the state is stacked
// and the matched caller changes. The opening extra cursor and flag homes remain.
// M3 lane 11 r152 (H2M_FAST with 0x270930, MATCH each run): the two-flag lead rebuilt on today's body 2 again (the same
// scan-latch pair). On it: one file-local inline range predicate shared by the link test and the scan's edge test,
// expression return 2, `if` with `return true; return false;` 2 (same code each: no vanishing block); the link test
// through an inline edge getter returning the edge or NULL 314 (1389 B); the previous point as a default then
// `if (j > 0)` 280 (1396 B, 0x270930 260); `*const steps` in the definition 2 (same code). Body unchanged.
// M3 lane 2 r141 (H2M_FAST with 0x270930, MATCH each run; control 286): `previous` dropped, the predecessor read through
// `lookup->nodes[previous_index]` 287 (1393 B, but frame 0xe4 as retail: first difference +0x19); the truncation test
// as `j < count - 1 && distance > 1600` 287; the edge-index test with `!= 0xffff` first 286; the special test written
// into the type chain (no named bool) 330. Round 110's saved two-flag lead re-applied to today's file 2 (the latch's
// steps/j reload pair only); on it the truncation's two stores swapped 4. Retail's link test stores the edge bit to the
// +0xf byte that the surface flag reuses (two block-scoped bools sharing a slot). Body unchanged.
// M3 lane 6 r237 (H2M_FAST with 0x270930 and 0x270750, MATCH each run; control 286, aligned/masked 179/57): lane 16's
// saved two-flag lead applied to today's file 2 again at 1388 B (the scan latch's steps/j reloads swapped). Tested on it
// as a whole-function allocation phase (redundant parentheses and new used locals shift such phases elsewhere): the
// margin initializer in one to three redundant pairs 2/2/2, the radius leaf in one to three pairs and both operands in
// one 2/2/2/2, `count = (j) + 1` with one to three pairs 2/2/2, the projection clamp's `length - margin` named and used
// twice 2, the length test's `margin * 2.0f` named 2, both 2, both plus `short last = count - 1` for the truncation test
// 26 (first difference +0x2c2). The pair is not a parenthesis or symbol-count phase. Body unchanged.
// M3 lane 3 r305 (H2M_FAST with 0x270930 and 0x270750, MATCH each run; control 286). The saved two-flag lead re-applied
// to today's file is 2 again. Reading of the pair: both loads are reloads at the head of the scan latch (the step array
// parameter into EBP and the index into EDX, both live in registers across the latch), they form a block of their own
// ahead of the count load, and with equal scheduling priority they keep the order the reloads were emitted in, the
// array's first; a forced change of either load's height or ready cycle in that block gives a full match, a forced
// change of either value's colouring priority does not. On the lead: the fill count as an `if` clamp in place of MIN 176
// (1391 B); `distance` declared after the origin pointer 2; the origin pointer hoisted ahead of the scan loop 396 (1422 B);
// one `v` shared by both distance arms 2; the edge pointer formed ahead of the previous point 25; `++j` 2; `j` declared
// ahead of the loop, after or before the margin 2/2; the truncation's two stores swapped 4; a data pointer for the four
// pathfinding casts inside the scan loop 397 (1416 B), ahead of it 336 (1410 B); the current step through an element
// pointer 2; the edge index through a pointer to its slot 2; the truncation test with the index compare first 16; the
// clamp's `length - margin` as a named const used twice 27. Body unchanged.
// M3 lane 13 r154 (H2M_FAST with 0x270930 and 0x270750, MATCH each run; control 286). Lane 16 r154's saved two-flag lead
// applied to today's file 2 again (the scan latch's steps/j reload pair). On it, an empty `do {} while (false);` (the
// compiled-out-statement join marker that matched 0xbe240) at five joins: ahead of the traversal latch statements, at
// the end of the scan body, after the truncation test inside the edge block, ahead of the depth test and ahead of the
// distance block: 2 each (same code: the empty block folds away). Forced decisions on the lead: only a height change
// of the j reload in the latch's reload region (2 -> 3) gives the full match; ready-cycle changes of the count load and
// cuts of node_index's range leave 2. That region holds only the reloads (no use of j), so retail's j reload sits in
// the region of the latch's increment and compare. Body unchanged.
// M3 lane 15 r145 (H2M_FAST with 0x270930 and 0x270750, MATCH each run unless said; control 286). The saved two-flag
// lead applied to today's file is 2 again at 1388 B. Measured cause of the latch pair: both reloads belong to split
// pieces and come out in ascending piece number. Piece numbers are taken from the numbers freed earlier and, when
// those run out, from fresh higher ones. On the lead exactly ten pieces are made ahead of the index's (three for the
// constant NONE, one for zero, the stack parameters link_count_out, complete_out and steps, then complete, link_count
// and count) and exactly ten numbers are free (four constants with no register use: the fill loop's step, 6, 5 and
// 0xffff; and six float values dropped at the start of the float pass because they live across a call: the margin, the
// two float constants used in the scan loop and the edge offset's three forwarded components). So the step array's
// piece is numbered below the index's. Diagnostics (wrong code, reverted): one more float value live across a call
// frees an eleventh number and the latch reloads come out in retail's order; a new integer constant outside the loops
// changes nothing. So retail has one more discarded float value or loop constant, or one piece fewer ahead of the
// index. On the lead: one counter for the fill loop and the scan loop 2; the index declared at the top, after
// link_count, or ahead of count 2 / 2 / 2; count declared at the top 349; the step-point copies or the fallback point
// copies as memcpy 2 each; the usable bit as `(flags & 0x80) != 0` 2, without the dword cast 14; `count = (short)(j +
// 1)` 2; `!count` with a cast store 2; `edge_index != (word)NONE` 2; `j + 1 < count` 5; `j >= 1 ? ...` 15; `long
// link_count` 356 (1404 B); a `long` fill counter 40; `long initialize_count` 263; the fill loop through a walking
// step pointer 382 (1405 B), through a per-step pointer local 2; `previous == NULL` for the first-step test 329
// (0x270930 317); a block-local copy of the margin 2; the edge-margin clamp in a force-inlined helper taking the
// margin by value 119 (1391 B, still six float values); the margin computed inside the scan loop 333; an ignored
// pointer result from a force-inlined point copy 2. The distance through the existing 0x210a30 as a call 556 (1183 B);
// diagnostics, restored at once: `inline` on 0x210a30's definition 428 (1307 B, 0x1f45c0 250), also on 0x210850's 539
// with seven callers differing; file-local force-inlined copies of both expanded at the test 8 (the pair unchanged).
// BLOCKED-BY 0x210a30 / 0x210850 (retail expands them here only). Body unchanged.
// M3 lane 16 r194 (h2m fast with 0x270930, 0x26f3f0 and 0x2104b0, all MATCH; lane 15 r123's two-flag form, held as a
// patch, re-applied: 2 at 1388 B). Cause of the last pair, read from the allocator and the scheduler: the latch's join
// block holds only the reloads of the steps piece (EBP) and of j's piece (EDX); both get the same scheduling priority, so
// they stay in the order the allocator inserted them, which is ascending piece number. Both pieces come from the split
// pass that follows `state` taking EBX; ten numbers are free at that point (four early temporaries the optimizer removed,
// six float values spilled across the length call), and eleven pieces are made: steps' piece takes a recycled number and
// j's, the eleventh, a fresh higher one. Diagnostic, reverted at once: `complete` declared volatile (it is never in a
// register anyway) MATCHES byte for byte, because one piece fewer leaves j a recycled number below steps'; `special`
// volatile 249 instead. A forced-ranking search agrees (raising j's reload or lowering steps' matches). Forms on that
// base, all 2 with the pair unchanged unless given: `complete` cleared through an inline helper taking its address;
// `*(byte *)&complete = false`; the final read through a cast of its address; memcpy for the final store; the edge
// test as a `continue`; `++j`; the truncation test nested; `real const margin`; the clamp as one conditional; the link
// count's increment inside the copy 304; the count declared ahead of the fill loop 348; `complete` declared before the
// walk 337, after the link count 2; a `long` fill counter 40; a separate walk index for the node parameter 370 (frame
// 0xe8); a step pointer for the special arm 263, for the depth row 299, for the type-2 arm 230, in the fill loop 2;
// `MIN(maximum_steps, ...)` 4; `(short)` casts on the count stores 2 each, on the fill bound and the returns 2;
// explicit NULL tests at the end 2; the previous point by if/else 2; a pointer-sum link destination 2; a function-scope
// data pointer 2; a result copy of the count 2; `lookup` written out at each use 2; the link and depth limits compared
// with the parameter first 4 each. A 600 s permuter run (60 tries) found nothing below 2. Next: a legal form that keeps
// `complete` out of the register candidates (retail's address-taken flags, see `special`), or one more removed
// temporary ahead of that split. Body unchanged.
// @retail 0x270d90
PRIVATE short function_270d90(long actor_index, s_type_f17a25 *state,
	s_path_result_view const *result, short node_index, short maximum_steps,
	s_path_step_view *steps, bool *complete_out, short *link_count_out,
	s_path_location_entry_view *links, short maximum_links)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_input_view const *input = (s_path_input_view *)&state->source;
	long edge_indices[40];
	short previous_index = NONE;
	s_path_node_key_view *previous = NULL;
	short count = 0;
	long link_count = 0;
	bool complete = true;
	long initialize_count = lookup->nodes[node_index].depth + 1;
	if (initialize_count > maximum_steps)
		initialize_count = maximum_steps;
	s_path_step_view *initialize_step = steps;
	long *initialize_edge = edge_indices;
	for (short remaining = initialize_count; remaining > 0; --remaining)
	{
		*initialize_edge++ = NONE;
		initialize_step->type = NONE;
		initialize_step->link_index = NONE;
		initialize_step->unknown02 = 0;
		initialize_step++;
	}
	while (node_index != NONE)
	{
		s_path_node_key_view *node = &lookup->nodes[node_index];
		if (link_count < maximum_links && (node->target.type != NONE ||
			(node->target.index >= 0 && node->target.index < ((s_path_links_data_view const *)state->pathfinding)->edge_count &&
			TEST_FIELD_BIT((((s_path_links_data_view const *)state->pathfinding)->edges[node->target.index].flags >> 7) & 1))))
		{
			memcpy(&links[link_count++], &node->target, sizeof(*links));
		}
		if (node->depth >= maximum_steps)
		{
			complete = false;
		}
		else
		{
			if (count == 0)
				count = node->depth + 1;
			steps[node->depth].node_index = node->node_index;
			if (previous_index == NONE)
				steps[node->depth].point = result->destination;
			else
			{
				steps[node->depth].point = previous->entry_point;
				if (previous->target.type == NONE)
					edge_indices[node->depth] = previous->target.index;
			}
			bool special = node->target.type == 2 &&
				((s_path_links_data_view const *)state->pathfinding)->surfaces[node->target.index].unknown0a == 0;
			if (node->target.type == 1 || node->target.type == 6 ||
				node->target.type == 5 || special)
			{
				steps[node->depth - 1].type = node->target.type;
				steps[node->depth - 1].link_index = (short)node->target.index;
				s_path_node_key_view *parent = &lookup->nodes[node->parent];
				function_26f3f0(actor_index, &node->entry_point, state, (s_pathfinding_data *)state->pathfinding,
					node->target.index, &parent->entry_point, parent->node_index, &parent->node_index,
					&steps[node->depth - 1].point, &steps[node->depth - 1].node_index);
			}
			else if (node->target.type == 2)
			{
				steps[node->depth - 1].type = node->target.type;
				steps[node->depth - 1].link_index = (short)node->target.index;
			}
		}
		previous_index = node_index;
		previous = node;
		node_index = node->parent;
	}
	real margin = input->radius * 1.2f;
	for (short j = 0; j < count; j++)
	{
		long edge_index = edge_indices[j];
		if (edge_index >= 0 && edge_index < ((s_path_links_data_view const *)state->pathfinding)->edge_count && edge_index != 0xffff)
		{
			s_type_c3b527 const *previous_point = j > 0 ? &steps[j - 1].point : &input->start;
			s_path_edge_view const *edge = &((s_path_links_data_view const *)state->pathfinding)->edges[edge_index];
			s_type_c3b527 *point = &steps[j].point;
			/* Retail widens these unsigned words before comparing against NONE. */
			if ((long)edge->vertices[0] != NONE && (long)edge->vertices[1] != NONE)
			{
				point3f const *a = &((s_path_links_data_view const *)state->pathfinding)->vertices[edge->vertices[0]];
				point3f const *b = &((s_path_links_data_view const *)state->pathfinding)->vertices[edge->vertices[1]];
				vector3f edge_vector;
				vector3d_from_points3d(a, b, &edge_vector);
				vector3f offset;
				vector3d_from_points3d(a, &previous_point->point, &offset);
				real length = function_30bf0(&edge_vector);
				if (length > margin * 2.0f)
				{
					real projection = dot3f(&offset, &edge_vector);
					if (margin > projection)
						projection = margin;
					else if (projection > length - margin)
						projection = length - margin;
					vector3f_add_scaled((vector3f const *)a, &edge_vector, projection, (vector3f *)&point->point);
				}
			}
			vector3f delta;
			if (input->start.output_index == point->output_index)
			{
				delta.i = point->point.x - input->start.point.x;
				delta.j = point->point.y - input->start.point.y;
				delta.k = point->point.z - input->start.point.z;
			}
			else
			{
				point3f start, end;
				if (input->start.output_index == NONE ||
					!function_2104b0(input->start.output_index, &input->start.point, &start))
					start = input->start.point;
				if (point->output_index == NONE ||
					!function_2104b0(point->output_index, &point->point, &end))
					end = point->point;
				delta.i = end.x - start.x;
				delta.j = end.y - start.y;
				delta.k = end.z - start.z;
			}
			if (delta.k * delta.k + delta.j * delta.j + delta.i * delta.i > 1600.0f && j < count - 1)
			{
				count = j + 1;
				complete = false;
			}
		}
	}
	if (complete_out)
		*complete_out = complete;
	if (link_count_out)
		*link_count_out = link_count;
	return count;
}

// @retail 0x270930
bool function_270930(long actor_index, s_type_f17a25 *state, s_path_result_view *result)
{
	byte *actor = NULL;
	if (actor_index != NONE)
		actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	s_path_input_view const *input = (s_path_input_view *)&state->source;
	s_path_destination_view const *destination = (s_path_destination_view *)state;
	s_path_closest_view const *closest = (s_path_closest_view *)state;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	result->valid = false;
	result->location = *(s_path_location_view *)&state->location;
	result->unknowna6 = false;
	if (destination->destination_valid)
	{
		short index = function_272700(state, destination->destination_node_index);
		if (index != NONE)
		{
			memcpy(&result->destination, &destination->destination, 24);
			result->destination_distance = 0.0f;
		}
		else if (destination->destination_radius > closest->distance)
		{
			index = closest->node_index;
			result->destination = closest->point;
			result->destination_node_index = lookup->nodes[index].node_index;
			result->destination_distance = closest->distance;
		}
		if (index != NONE)
		{
			s_path_step_view local_0af630[40], name_b343c4[4], final_steps[4];
			s_path_location_entry_view links[10];
			short smoothed_count = 0, final_count = 0, link_count = 0;
			bool complete = true, success = true;
			short raw_count = function_270d90(actor_index, state, result, index, 40,
				local_0af630, &complete, &link_count, links, 10);
			function_2c2060(state, raw_count, local_0af630, &smoothed_count, name_b343c4, &complete);
			if (actor && actor[0x478])
			{
				final_count = smoothed_count > 4 ? 4 : smoothed_count;
				memcpy(final_steps, name_b343c4, final_count * sizeof(*final_steps));
			}
			else
			{
				long object_index = NONE, type = NONE;
				bool avoid = state->unknownac;
				if (!avoid && result->location.flags > 0)
					avoid = (result->location.flags / (1 << (byte)result->location.count)) % 2 == 1;
				success = function_2c41b0(actor_index, state, smoothed_count, name_b343c4, avoid,
					&final_count, final_steps, &complete, &object_index, &type, &result->unknowna6);
				if (success)
				{
					result->object_index = object_index;
					result->type = (short)type;
					if (object_index != NONE && actor_index != NONE && (short)type == 6)
					{
						real duration = (real)g_510c54->field_2_3 * 0.2f;
						long ticks;
						__asm { fld duration }
						__asm { fistp ticks }
						function_1a8220(actor_index, 9, (short)ticks, 3, 1, 42, 3);
					}
				}
				else
				{
					result->object_index = NONE;
					result->type = NONE;
				}
			}
			if (result->location.flags == 0)
			{
				s_path_location_view *location = &result->location;
				if (link_count > 0)
					location->entries[0] = links[0];
				if (link_count > 1)
					location->entries[1] = links[link_count - 1];
				if (link_count > 2)
					location->entries[2] = links[link_count / 2];
				location->count = link_count < 0 ? 0 : link_count > 3 ? 3 : link_count;
			}
			if (success)
			{
				result->complete = complete;
				result->step_count = (signed char)final_count;
				result->valid = true;
				result->step_index = 0;
				result->start = input->start;
				memcpy(result->steps, final_steps, final_count * sizeof(*final_steps));
				if (result->complete)
				{
					if (result->step_count > 0)
					{
						result->destination = result->steps[result->step_count - 1].point;
						result->destination_node_index = result->steps[result->step_count - 1].node_index;
					}
					else
					{
						result->destination = input->start;
						result->destination_node_index = input->start_node_index;
					}
					result->destination_distance = function_210970(&result->destination, &destination->destination);
					return result->valid;
				}
			}
			else if (++result->location.flags > 32)
				result->location.flags = 0;
		}
	}
	return result->valid;
}

// @retail 0x2713c0
void function_2713c0(s_type_f17a25 *state, s_type_c3b527 const *point,
	long node_index, real radius)
{
	s_path_destination_view *destination = (s_path_destination_view *)state;
	destination->destination_valid = true;
	destination->destination = *point;
	destination->destination_node_index = node_index;
	destination->destination_radius = radius;
}

// @retail 0x2713f0
PRIVATE bool function_2713f0(s_type_f17a25 *state)
{
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_destination_view *destination = (s_path_destination_view *)state;
	bool result = false;
	if (!state->pathfinding)
	{
		return false;
	}
	long node_count = *(long *)state->pathfinding;
	if (input->start_node_index >= 0 && input->start_node_index < node_count &&
		input->start.point.z > -1000.0f)
	{
		real distance;
		long quantized;
		if (destination->destination_valid)
		{
			distance = function_210970(&input->start, &destination->destination);
			quantized = (long)(distance * 10.0f);
		}
		else
		{
			distance = 0.0f;
			quantized = 0;
		}
		if (quantized < 32767)
		{
			s_pathfinding_node *sector = &((s_pathfinding_data *)state->pathfinding)->nodes[input->start_node_index];
			short index = state->unknownae++;
			s_path_lookup_view *lookup = (s_path_lookup_view *)state;
			s_path_node_key_view *node = &lookup->nodes[index];
			node->parent = NONE;
			node->unknown04 = NONE;
			node->node_index = input->start_node_index;
			node->entry_point = input->start;
			node->entry_distance = 0.0f;
			node->path_distance = 0.0f;
			node->attractor_distance = FLT_MAX;
			node->cost = 0.0f;
			node->estimated_distance = distance;
			node->quantized_cost = (short)quantized;
			node->depth = 0;
			node->flag0c = (sector->flags >> 12) & 1;
			node->flag0d = (sector->flags & 0x3c0) != 0;
			node->flag0e = false;
			node->target.type = NONE;
			node->target.index = NONE;
			if (destination->destination_valid)
			{
				state->unknown90 = index;
				*(real *)((byte *)state + 0x94) = distance;
				*(s_type_c3b527 *)((byte *)state + 0x9c) = input->start;
				*(real *)((byte *)state + 0x98) = distance;
			}
			lookup->hash_table[(node->node_index & 511) * 8] = index;
			function_271fd0(state, index, (short)quantized);
			result = true;
		}
	}
	return result;
}

// @retail 0x2715a0
bool function_2715a0(byte *buffer)
{
	s_type_f17a25 *state = (s_type_f17a25 *)buffer;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_closest_view *closest = (s_path_closest_view *)state;
	bool result = false;
	state->unknownae = 0;
	state->heap_count = 1;
	memset(lookup->hash_table, 0xff, sizeof(lookup->hash_table));
	closest->node_index = NONE;
	closest->distance = FLT_MAX;
	closest->estimated_distance = FLT_MAX;
	if (function_2713f0(state))
	{
		result = function_271630(state);
	}
	if (!result)
	{
		if (state->location.unknown00 == 0)
		{
			state->location.unknown00 = 32;
		}
		else
		{
			s_path_location *location = &state->location;
			if (++location->unknown00 > 32)
			{
				location->unknown00 = 0;
			}
		}
	}
	return result;
}

/* takes the cheapest node off the open heap; NONE when the heap is empty */
static __forceinline short path_heap_pop(s_type_f17a25 *state)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	if (state->heap_count <= 1)
	{
		return NONE;
	}
	short index = state->heap[1].node;
	lookup->nodes[index].heap_index = NONE;
	if (--state->heap_count > 1)
	{
		state->heap[1] = state->heap[state->heap_count];
		function_271ef0(state, 1);
	}
	return index;
}

/* whether a node already stands for a link's destination: the same sector node, entered at the same
   point (within 0.3) unless neither the node nor the link carries the special flag */
static __forceinline bool path_node_matches_link(s_path_node_key_view const *candidate, s_path_link_view const *link)
{
	if (candidate->node_index != link->node_index)
	{
		return false;
	}
	if (!candidate->flag0e && !link->unknown2e)
	{
		return true;
	}
	real dx = candidate->entry_point.point.x - link->point.point.x;
	real dy = candidate->entry_point.point.y - link->point.point.y;
	real dz = candidate->entry_point.point.z - link->point.point.z;
	real distance_squared = dx * dx;
	distance_squared += dz * dz;
	distance_squared += dy * dy;
	if (distance_squared > 0.09f)
	{
		return false;
	}
	return true;
}

/* the node to update for a link: the matching node when the new cost is lower and it is still in the
   heap, or a new node when none matches; NONE when the match is no better or the table is full */
static __forceinline short path_link_child_node(s_type_f17a25 *state, s_path_link_view const *link, long quantized)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	short result = NONE;
	short hash = (short)((link->node_index & 511) * 8);
	short next = lookup->hash_table[hash];
	while (next != NONE)
	{
		s_path_node_key_view *candidate = &lookup->nodes[next];
		if (path_node_matches_link(candidate, link))
		{
			if (quantized < candidate->quantized_cost && candidate->heap_index != NONE)
			{
				result = next;
			}
			return result;
		}
		hash = (short)((hash + 1) & 4095);
		next = lookup->hash_table[hash];
	}
	if (state->unknownae < 1024)
	{
		result = state->unknownae++;
		lookup->hash_table[hash] = result;
		lookup->nodes[result].heap_index = NONE;
	}
	return result;
}

/* The search loop: pops the cheapest open node, stops at the destination node or when every open node is
   too far past the closest one, and opens or improves a child node for each link out of it. Matches retail
   with the heap pop, the child find-or-add and its same-node test as inline helpers (retail's jump-threaded
   early returns), the link point along its vector through vector3f_add_scaled, the root of the link's
   squared length bound by a const reference (rounded before the 0x210be0 call), the link target copied as
   one record, and each closest update storing its index and distance before the point, as 0x2713f0 does. */
// @retail 0x271630
PRIVATE bool function_271630(s_type_f17a25 *state)
{
	s_path_input_view *input = (s_path_input_view *)&state->source;
	s_path_destination_view *destination = (s_path_destination_view *)state;
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	s_path_closest_view *closest = (s_path_closest_view *)state;
	s_path_location_view *location = (s_path_location_view *)&state->location;
	real radius = 0.2f > input->radius ? 0.2f : input->radius;
	bool volatile result = true;
	real link_penalty = input->link_penalty;
	s_path_link_view links[64];
	short index;
	while ((index = path_heap_pop(state)) != NONE)
	{
		s_path_node_key_view *node = &lookup->nodes[index];
		if (destination->destination_valid)
		{
			if (node->node_index == destination->destination_node_index)
			{
				closest->node_index = index;
				closest->distance = 0.0f;
				closest->point = destination->destination;
				break;
			}
			real limit = 5.0f > closest->distance ? 5.0f : closest->distance;
			if (node->estimated_distance > limit * 10.0f + closest->estimated_distance)
			{
				break;
			}
		}
		short count = function_272020((s_pathfinding_data *)state->pathfinding, node, links, state);
		for (short i = 0; i < count; ++i)
		{
			s_path_link_view *link = &links[i];
			if (link->node_index == node->unknown04 && !link->unknown2e && !node->flag0e)
			{
				continue;
			}
			if (!(link->flags & 1) || (node->flag0c && (link->flags & 0x1000)))
			{
				continue;
			}
			bool blocked = false;
			if (state->location.unknown00 > 0)
			{
				for (short j = 0; j < location->count; ++j)
				{
					if ((location->flags & (1 << j)) &&
						link->target.type == location->entries[j].unknown00 &&
						link->target.index == location->entries[j].node_index)
					{
						blocked = true;
						break;
					}
				}
			}
			if (blocked || (!input->unknown04 && (link->flags & 2) &&
				function_1fa6b0((s_pathfinding_data *)state->pathfinding,
					&((s_pathfinding_data *)state->pathfinding)->nodes[link->node_index])))
			{
				continue;
			}
			real length_squared = link->vector.i * link->vector.i +
				link->vector.j * link->vector.j + link->vector.k * link->vector.k;
			if (link->unknown2c && !link->unknown2d && (radius * 2.0f) * (radius * 2.0f) > length_squared)
			{
				continue;
			}
			s_type_c3b527 point;
			vector3f_add_scaled((vector3f const *)&link->point.point, &link->vector, 0.5f, (vector3f *)&point.point);
			point.output_index = link->point.output_index;
			if (destination->destination_valid && length_squared > 16.0f &&
				length_squared > (radius * 2.0f) * (radius * 2.0f))
			{
				real const &length = (real)sqrt(length_squared);
				vector3f to_destination;
				function_210be0(&link->point, &destination->destination, &to_destination);
				real t = dot3f(&link->vector, &to_destination) / length_sq3f(&link->vector);
				real margin = radius / length;
				if (margin > t)
				{
					t = margin;
				}
				else if (t > 1.0f - margin)
				{
					t = 1.0f - margin;
				}
				vector3f_add_scaled((vector3f const *)&link->point.point, &link->vector, t, (vector3f *)&point.point);
			}
			real distance = function_210970(&node->entry_point, &point);
			real entry_distance = distance;
			real path_distance = distance + node->path_distance;
			real attractor_distance;
			real entry_cost;
			if (input->attractor_valid)
			{
				real weight = function_272810(state, &node->entry_point, &point.point, &attractor_distance);
				entry_cost = (weight + 1.0f) * entry_distance;
				attractor_distance = node->attractor_distance > attractor_distance ?
					attractor_distance : node->attractor_distance;
			}
			else
			{
				attractor_distance = 0.0f;
				entry_cost = entry_distance;
			}
			if (link_penalty > 0.0f && link->target.type != NONE)
			{
				switch (link->target.type)
				{
				case 1: case 2: case 5: case 6:
					entry_cost += link_penalty;
					break;
				}
			}
			real cost = node->cost + entry_cost;
			real estimated_distance = cost;
			real destination_distance;
			if (destination->destination_valid)
			{
				destination_distance = function_210970(&point, &destination->destination);
				estimated_distance = destination_distance + cost;
			}
			long quantized = (long)(estimated_distance * 10.0f);
			if (quantized >= 32767 || (input->distance_limit_valid && path_distance > input->distance_limit))
			{
				continue;
			}
			short next = path_link_child_node(state, link, quantized);
			if (next == NONE)
			{
				continue;
			}
			s_path_node_key_view *child = &lookup->nodes[next];
			child->parent = index;
			child->unknown04 = node->node_index;
			child->node_index = link->node_index;
			child->entry_point = point;
			child->entry_distance = entry_distance;
			child->attractor_distance = attractor_distance;
			child->path_distance = path_distance;
			child->cost = cost;
			child->estimated_distance = estimated_distance;
			child->quantized_cost = (short)quantized;
			child->flag0c = ((dword)link->flags >> 12) & 1;
			child->flag0d = (link->flags & 0x3c0) > 0;
			child->flag0e = link->unknown2e;
			child->target = link->target;
			switch (child->target.type)
			{
			case 1: case 5: case 6:
				child->depth = node->depth + 2;
				break;
			case 2:
				if (*(short *)((byte *)&((s_pathfinding_data *)state->pathfinding)->surfaces[child->target.index] + 0xa) == 0)
				{
					child->depth = node->depth + 2;
				}
				else
				{
					child->depth = node->depth + 1;
				}
				break;
			default:
				child->depth = node->depth + 1;
				break;
			}
			if (child->heap_index == NONE)
			{
				function_271fd0(state, next, (short)quantized);
			}
			else
			{
				state->heap[child->heap_index].cost = (short)quantized;
				function_271e50(state, child->heap_index);
			}
			if (destination->destination_valid)
			{
				s_type_c3b527 entry_point = child->entry_point;
				if (destination->destination_radius > 0.0f && closest->distance > destination_distance)
				{
					closest->node_index = next;
					closest->distance = destination_distance;
					closest->point = entry_point;
					closest->estimated_distance = estimated_distance;
				}
			}
		}
	}
	if (destination->destination_valid)
	{
		if (destination->destination_radius >= closest->distance)
		{
			return true;
		}
		return false;
	}
	return result;
}

// score 0 (was 8): old index and signed slot registers differ, including existing heap helper input;
// native long insertion index 21; increment the count field 8; postincrement count at insertion 8; shared
// native heap element pointer 16; guard return for full heap 8; name the next count expression 8; store
// cost before node 10; native element pointer with postincrement count 16; consistent existing heap helper
// parameter order 8.
// @retail 0x271fd0
PRIVATE void function_271fd0(s_type_f17a25 *state, short node_index, short cost)
{
	short index = state->heap_count;
	if (index < 1024)
	{
		state->heap_count = index + 1;
		state->heap[index].node = node_index;
		state->heap[index].cost = cost;
		function_271e50(state, index);
	}
}

/* Surface records interpret their payload differently by type. Retail's
   vertex indices are unsigned words: its comparisons with long NONE do not
   reject 0xffff. Only destination sector words have an effective NONE test.
   Link type stores write the low short; the upper short stays untouched. */
// fills links with the moves out of a node: its surface links, then its edge ring
// score 383 (was 383): Frame 0x30 versus retail 0x34; flag-mask, edge-index and endpoint homes differ.
// Earlier surface word-bit view 452 (from 475).
// Frame 0x2c/0x34, data/count registers and endpoint homes remain. Signed edge byte 464; direct parameter 452;
// native surface bits 452; long surface cursor 483; output cursor 541; indexed vector helper 452; native
// flags/direct parameter 464; paired cursors 555; ring-anchor reads at use 469; cached node array 474; reused
// start vertices 485. All restored.
// Frame 0x2c versus retail 0x34, root/count registers and endpoint spill ownership remain.
// Earlier: flag 1 of the surface read through a word bitfield view gives retail's load-and-shift; the rest still differs.
// DH round 254: both endpoints before copies in edge-surface/paired-surface/ring subsets
// 449/452/449/450/447/450/447; all with consistent endpoint storage names 447. All restored: gains
// are at most five, and the one-use end pointers require an exact match. Original guards and helpers retained.
// Round 429: direct parameter, early count 452; case orders 0,2,5,1/6 original/early count
// 456/456; 1/6,0,2,5 original/early 476/476; 2,5,1/6,0 original/early 461/461;
// 5,2,0,1/6 early 463. Seven forms reach retail length; all bodies restored.
// Earlier 256 (was 292): the ring walk names its two edge vertices so the point copy and the
// vector difference share one pair of vertex pointers, and the edge flag byte is stored
// before the two cleared flag bytes; the state and flag mask registers still differ.
// Earlier 292 (was 312): the surface list is walked as retail's do-while and the ring walk stores
// the edge flag byte in retail's order, so the loop tails and the ring entries now agree; the
// state pointer and the flag mask still use swapped registers, the case 0 edge comparison uses
// another register and the ring walk's edge pointer comes from a register, not retail's stack home.
// Earlier 312 (was 427): retail's frame (0x34), argument offsets and length agree with ours, the
// surface vertices are named locals so the link points and the segment ends land in retail's stack
// homes, the node distance is three named components summed in z, y, x order and the link fields
// are stored in retail's order; the state pointer and the flag mask still use swapped registers,
// the case 0 edge comparison uses another register and the ring walk still differs.
// Earlier 383 (was 452): retail's pathfinding, node-array, count and node-index registers and the
// local homes now agree with ours, and the length matches retail's 1760 bytes; the frame is 0x30
// against retail's 0x34, the flag mask and the edge index still use different registers, and the
// two edge endpoints are not kept in retail's stack homes.
// Coordinator review of the score-256 round: kept at 383 fast (main 452): the flat pathfinding fields (no stack copy
// of the pointer), the do/while with the count test first, the signed char edge flags (retail loads them with movsx,
// shr 7) and the shared vertex pointers. Case 0 must still write unknown2d = true and unknown2e = false: retail jumps
// to the shared tail at 0x27258b that stores them, and callers read both. The round's form with those two writes added
// back scores 256, but it hand-copies vector3d_from_points3d for the flag0d distance (three named components summed
// z, y, x) and adds one-use segment_start/segment_end (declared end first) and link_end pointers; those are not kept
// without a match. The swapped facing branch in 0x263810 alone scores 150 (was 152), not kept.
// Round 320: shared movement mask/long movement ordinal/converted-point lexical homes,
// nonempty subsets 448/392/448/383/448/392/448; all with entry point workspaces 448.
// Mask capture recovers frame 0x34 but regresses; original observations, loops, copies and body restored.
// @retail 0x272020
PRIVATE short function_272020(s_pathfinding_data const *pathfinding,
	s_path_node_key_view const *node, s_path_link_view *links, s_type_f17a25 const *state)
{
	short count = 0;
	s_pathfinding_node const *nodes = pathfinding->nodes;
	long node_index = node->node_index;
	short output_index = node->entry_point.output_index;
	short surface_index = nodes[node_index].first_surface;
	if (pathfinding->surface_count > 0 && surface_index != NONE)
	{
		do
		{
			if (count >= 64)
			{
				break;
			}
			s_path_surface_link_view const *surface =
				(s_path_surface_link_view const *)&pathfinding->surfaces[surface_index];
			if (state->settings.flags & (1 << surface->type))
			{
				switch (surface->type)
				{
				case 0:
				{
					long edge_index = *(long const *)&surface->unknown04;
					s_path_edge_view const *edge = (s_path_edge_view const *)&pathfinding->edges[edge_index];
					word next_node = edge->nodes[node_index != edge->nodes[1]];
					if ((long)next_node != NONE && next_node != (word)NONE)
					{
						s_path_link_view *link = &links[count];
						link->node_index = next_node;
						link->target.index = edge_index;
						link->target.type = NONE;
						link->flags = nodes[next_node].flags;
						point3f const *first_vertex = &pathfinding->vertices[edge->vertices[0]];
						point3f const *second_vertex = &pathfinding->vertices[edge->vertices[1]];
						link->point.point = *first_vertex;
						link->point.output_index = output_index;
						vector3d_from_points3d(first_vertex, second_vertex, &link->vector);
						link->unknown2c = ((dword)edge->flags >> 7) & 1;
						link->unknown2d = true;
						link->unknown2e = false;
						++count;
					}
					break;
				}
				case 1:
				case 6:
				{
					if ((long)surface->unknown04 == NONE || (long)surface->unknown06 == NONE ||
						(long)surface->unknown08 == NONE || (long)surface->unknown0a == NONE)
					{
						break;
					}
					if (!state->settings.unknown0c &&
						(state->settings.unknown08 & surface->unknown0d) <= 0)
					{
						break;
					}
					word next_node = surface->unknown0e;
					if ((long)next_node == NONE || next_node == (word)NONE)
					{
						break;
					}
					short flags = nodes[next_node].flags;
					if (node->flag0d)
					{
						vector3f delta;
						vector3d_from_points3d(&pathfinding->vertices[surface->unknown04],
							&node->entry_point.point, &delta);
						if (delta.i * delta.i + delta.j * delta.j + delta.k * delta.k > 0.09f)
						{
							break;
						}
					}
					if (!(surface->unknown0c & 1) && surface->unknown10 != surface->unknown12)
					{
						point3f const *from_vertex = &pathfinding->vertices[surface->unknown04];
						point3f const *end_vertex = &pathfinding->vertices[surface->unknown08];
						point3f start, end;
						function_2104b0(surface->unknown10, from_vertex, &start);
						function_2104b0(surface->unknown12, end_vertex, &end);
						bool traversable = false;
						for (short type = 0; type < 6; ++type)
						{
							long bit = 1 << type;
							if ((state->settings.unknown08 & bit) && function_26f150(type, &start, NULL, &end, NULL))
							{
								traversable = true;
								break;
							}
							if (bit & surface->unknown0d)
							{
								break;
							}
						}
						if (!traversable)
						{
							break;
						}
					}
					point3f const *link_point = &pathfinding->vertices[surface->unknown08];
					point3f const *link_end = &pathfinding->vertices[surface->unknown0a];
					s_path_link_view *link = &links[count];
					link->target.type = surface->type;
					link->target.index = surface_index;
					link->node_index = surface->unknown0e;
					link->unknown2c = true;
					link->unknown2d = true;
					link->flags = flags | 1;
					link->point.point = *link_point;
					link->point.output_index = surface->unknown12;
					link->unknown2e = surface->type == 6;
					vector3d_from_points3d(link_point, link_end, &link->vector);
					++count;
					break;
				}
				case 2:
				{
					if ((long)surface->unknown06 == NONE || (long)surface->unknown08 == NONE)
					{
						break;
					}
					word next_node = surface->unknown04;
					if ((long)next_node == NONE || next_node == (word)NONE)
					{
						break;
					}
					point3f const *point = &pathfinding->vertices[surface->unknown06];
					if (surface->unknown0a == 1 || surface->unknown0a == 2)
					{
						real dx = node->entry_point.point.x - point->x;
						real dy = node->entry_point.point.y - point->y;
						if (dy * dy + dx * dx > 0.09f)
						{
							break;
						}
					}
					s_path_link_view *link = &links[count];
					link->target.index = surface_index;
					link->target.type = 2;
					link->node_index = surface->unknown04;
					link->flags = nodes[link->node_index].flags | 1;
					link->point.point = *point;
					link->point.output_index = output_index;
					link->vector = *g_4687a4;
					link->unknown2c = true;
					link->unknown2d = true;
					link->unknown2e = surface->unknown0a == 3;
					++count;
					break;
				}
				case 5:
				{
					if ((long)surface->unknown06 == NONE || (long)surface->unknown08 == NONE ||
						(long)surface->unknown0a == NONE)
					{
						break;
					}
					word next_node = surface->unknown04;
					if ((long)next_node == NONE || next_node == (word)NONE)
					{
						break;
					}
					long flags = state->settings.unknown04;
					if (!(((flags & 0x400) && (surface->unknown0c & 1)) ||
						((flags & 0x800) && (surface->unknown0c & 2)) ||
						((flags & 0x1000) && (surface->unknown0c & 4))))
					{
						break;
					}
					point3f const *point = &pathfinding->vertices[surface->unknown08];
					s_path_link_view *link = &links[count];
					link->target.index = surface_index;
					link->target.type = 5;
					link->node_index = surface->unknown04;
					link->flags = nodes[link->node_index].flags;
					link->point.point = *point;
					link->point.output_index = output_index;
					link->vector = *g_4687a4;
					link->unknown2c = true;
					link->unknown2d = true;
					link->unknown2e = false;
					++count;
					break;
				}
				}
			}
			surface_index = surface->next;
		}
		while (surface_index != NONE);
	}
	/* Retail enters the ring even if the surface pass already filled 64 slots.
	   Its count check follows insertion; keep that behavior here. */
	if ((nodes[node_index].flags & 1) || (count == 0 && node->parent == NONE))
	{
		long edge_index = nodes[node_index].first_edge;
		do
		{
			s_path_edge_view const *edge = (s_path_edge_view const *)&pathfinding->edges[edge_index];
			bool reverse = node_index == edge->nodes[1];
			long next_node = edge->nodes[!reverse];
			if (next_node != NONE && next_node != 0xffff)
			{
				s_path_link_view *link = &links[count];
				link->target.index = edge_index;
				link->node_index = next_node;
				link->target.type = NONE;
				link->flags = nodes[next_node].flags;
				link->unknown2c = ((dword)edge->flags >> 7) & 1;
				link->unknown2d = false;
				link->unknown2e = false;
				point3f const *first_vertex = &pathfinding->vertices[edge->vertices[0]];
				point3f const *second_vertex = &pathfinding->vertices[edge->vertices[1]];
				link->point.point = *first_vertex;
				link->point.output_index = output_index;
				vector3d_from_points3d(first_vertex, second_vertex, &link->vector);
				++count;
			}
			if (count == 64)
			{
				break;
			}
			edge_index = edge->next_edges[reverse];
		}
		while (edge_index != nodes[node_index].first_edge);
	}
	return count;
}

// @retail 0x272700
short function_272700(s_type_f17a25 *state, long node_index)
{
	s_path_lookup_view *lookup = (s_path_lookup_view *)state;
	short hash_index = (node_index & 511) * 8;
	short result;
	do
	{
		result = lookup->hash_table[hash_index];
		hash_index = (hash_index + 1) & 4095;
	}
	while (result != NONE && lookup->nodes[result].node_index != node_index);
	return result;
}

// score 16 (was 67): retail computes the segment i, j, k and sums (i + j) + k for the length and (j + k) + i for the projection, ours j, k, i and (k + j) + i for both; tried: `dot3f(&delta, &delta)` for the length 16, `dot3f(&delta, &offset)` 16, the projection written out in y, z, x order 36, all three written out 67, parameters reordered (start, end, attractor, out) 16; compile-only harness: every written-out sum order and grouping 27-64, all 24 parameter orders and the offset computed at the top no better.
// Complete output object/early admissions/uninitialized offset scope/role names/indexed interpolation/native output aggregate/parallel projection
// products/combined indexed projection 111/49/16/16/21/99/85/85; all restored.
// Indexed unit-scale segment/offset differences 16; all three named squares 62; native point publication 31;
// complete start/end values 139/131; named projection 16; indexed segment plus all named squares 62;
// all named squares plus named projection 62. Original complete body and noinline declaration retained.
// score 0 (was 16): matches retail: the point on the segment from start to end closest to the attractor, or the
// nearer end when the projection falls outside the segment; the segment's squared length is summed in the order
// i, j, k and the projection in the order j, k, i, the orders retail uses.
// score 16 (was 67): retail computes the segment i, j, k and sums (i + j) + k for the length and (j + k) + i for the projection, ours j, k, i and (k + j) + i for both; tried: `dot3f(&delta, &delta)` for the length 16, `dot3f(&delta, &offset)` 16, the projection written out in y, z, x order 36, all three written out 67, parameters reordered (start, end, attractor, out) 16; compile-only harness: every written-out sum order and grouping 27-64, all 24 parameter orders and the offset computed at the top no better.
/* Squared length of a vector, with the components summed in the order i, j, k. */
static inline real squared_length_ijk(vector3f const *v)
{
	real sum = v->i * v->i;
	sum += v->j * v->j;
	sum += v->k * v->k;
	return sum;
}

/* Dot product of two vectors, with the components summed in the order j, k, i. */
static inline real dot3f_jki(vector3f const *a, vector3f const *b)
{
	real sum = a->j * b->j;
	sum += a->k * b->k;
	sum += a->i * b->i;
	return sum;
}

// @retail 0x272740
PRIVATE void function_272740(point3f const *attractor,
	point3f const *start, point3f const *end, point3f *out)
{
	vector3f delta;
	vector3d_from_points3d(start, end, &delta);
	real length_squared = squared_length_ijk(&delta);
	if (length_squared > 0.0f)
	{
		vector3f offset;
		vector3d_from_points3d(attractor, start, &offset);
		real t = dot3f_jki(&offset, &delta) / length_squared;
		if (t < 0.0f || t > 1.0f)
		{
			*out = *end;
		}
		else
		{
			out->x = delta.i * t + start->x;
			out->y = delta.j * t + start->y;
			out->z = delta.k * t + start->z;
		}
	}
	else
	{
		*out = *start;
	}
}

// score 0 (was 8): matches retail: the weight of the point on the segment closest to the attractor, and that point's
// distance to the attractor; the segment's squared length is summed in the order k, i, j, the order retail uses.
// score 8 (was 97): retail computes the segment i, k, j and sums its square as (k + i) + j; ours computes j, k, i and sums
// (k + j) + i. Tried: the weight in float (no double local) 88 -> 82; weight declared first 81; written-out sums in every order
// 79-94; the segment, offset and both lengths through vector3d_from_points3d/length_sq3f/dot3f 21; the length test against g_45dbd8
// (retail reads 0x45dbd8; weight's zero is then not shared) 8; dot3f for either length and the swapped projection 8; separate
// segment/delta vectors 8; a `0.0` double test 102; the permuter (86 tries) found nothing below 8; distance_sq3f for the segment
// 132 fast/100 plain; vector3f_assign 8; offset or t at function scope 8 each; delta renamed segment 8; closest declared first 8;
// input declared later 8; length declaration split 8; aggregate segment initialization 83.
// Indexed unit-scale difference helper for the segment or all differences 8/8; all three named segment squares 83;
// all three named component differences plus the native setter 8; complete start-point value 117; named projection 8;
// complete native point publication 30; complete start-point value plus named projection 117. All restored.
/* Squared length of a vector, with the components summed in the order k, i, j. */
static inline real squared_length_kij(vector3f const *v)
{
	real sum = v->k * v->k;
	sum += v->i * v->i;
	sum += v->j * v->j;
	return sum;
}

// @retail 0x272810
PRIVATE real function_272810(s_type_f17a25 const *state, s_type_c3b527 const *start,
	point3f const *end, real *distance_out)
{
	s_path_input_view const *input = (s_path_input_view const *)&state->source;
	real weight = 0.0f;
	real distance = FLT_MAX;
	point3f local_end;
	point3f local_attractor;
	function_210690(start->output_index, end, &local_end);
	function_210690(start->output_index, &input->attractor_point, &local_attractor);
	vector3f delta;
	vector3d_from_points3d(&start->point, &local_end, &delta);
	real length_squared = squared_length_kij(&delta);
	point3f closest;
	if (length_squared > g_45dbd8)
	{
		vector3f offset;
		vector3d_from_points3d(&local_attractor, &start->point, &offset);
		real t = dot3f(&offset, &delta) / length_squared;
		if (t < 0.0f || t > 1.0f)
		{
			closest = local_end;
		}
		else
		{
			closest.x = delta.i * t + start->point.x;
			closest.y = delta.j * t + start->point.y;
			closest.z = delta.k * t + start->point.z;
		}
	}
	else
	{
		closest = start->point;
	}
	vector3d_from_points3d(&local_attractor, &closest, &delta);
	real distance_squared = length_sq3f(&delta);
	if (input->attractor_radius * input->attractor_radius > distance_squared)
	{
		distance = (real)sqrt(distance_squared);
		weight = (1.0f - distance / input->attractor_radius) * input->field_40;
	}
	*distance_out = distance;
	return weight;
}
