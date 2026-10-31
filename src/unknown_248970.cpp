#include "unknown_11c920.h"
#include "effects.h"

// @flags /O2 /Ob1 /arch:SSE /Gr

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct s_247fd0;
struct s_object_2470e0;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_248970
{
	short field_0;
	byte field_2;
	byte field_3;
	long field_4;
	long field_8;
	long field_c;
	point3f field_10;
	real field_1c;
	byte field_20[0x34 - 0x20];
};

struct s_248971
{
	byte field_0[0x18];
	real field_18;
	real field_1c;
	real field_20;
	real field_24;
	real field_28;
	real field_2c;
	long field_30;
	byte *field_34;
};

class c_247470
{
public:
	word field_0;
	word field_2;
	long field_4;
	long field_8;
	real field_c;
	matrix3x3 field_10;
	point3f field_34;
	point3f field_40;
	void function_247470(real arg_0, s_particle_system_datum *arg_1, s_object_2470e0 *arg_2,
		s_247fd0 *arg_3, transform4x3f const *arg_4, dword arg_5);
};

// score 102 (was 198): the three coordinate differences stay in the x87 stack until the end of
// the loop body, so the squared sum is followed by the radius multiply and the three pops at the
// end; retail frees the differences right after the square root (fstp st(3) and two fstp st(0)),
// and neither a nested block nor writing each difference out at every use frees them that early
// (106); the child loop still takes the pool pointer in edi where retail takes ebx.
// score 102 (was 198): the position copy writes the location member, the activation branch
// lays the viewer arm first and the viewer's influence starts at zero, so the prologue, the
// copy and the viewer loop agree; the x87 cleanup and the child loop's registers still differ.
// score 172 (was 198): the head snapshot and the loop's head are separate locals, so the
// frame is 0x14 and the prologue agrees; the settings store and the position's z store are
// swapped, the activation branch lays the active arm first, the viewer's maximum and x87
// cleanup differ, and the child loop's offset and counter homes are swapped.
// score 198 (was 198): activation AL and stacked location agree; frame 0x0c/0x14,
// retained head/position homes and x87 cleanup differ.
// Earlier metadata/head/count homes differed.
// Earlier convention mismatch: activation was stacked with six words, while
// retail receives it in AL with five stacked words.
// Earlier score 227: activation AL and stacked location agree; frame 0x10/0x14,
// metadata/head/count homes and x87 cleanup differ.
// Earlier: retail's convention since round 67 (flag in AL, location stacked, 0x175270
// the child loop advances a 0xb8-stride cursor over the child table instead
// of recomputing field_34 + i * 0xb8, as retail does; field_4 is now read before the metadata
// call and the flag is stored after the position copy, as retail does.
// Still differs: our frame is 0x10 against retail's 0x14, we keep the metadata pointer and the
// child count in stack slots of our own, and the position copy uses EDX/ECX where retail uses
// ESI/EDX.
// Round 67: a position pointer for the copy and the three viewer reads, with field_4
// then matches). Left: frame 0x10 versus 0x14 (our position pointer folds into [edi+0x10] where
// retail keeps it in ECX and spills field_4 to a fifth slot), the inactive (viewer-loop) arm laid
// out first in retail, the x87 pops before the radius multiply, and the child link (retail inlines
// 0x2483f0). Round 67: a position pointer for the copy and the three viewer reads, with field_4
// read before the getter and the flag stored after the copy, 277 (692 bytes; 0x175270 100 ->
// match); plus the nested MAX fade combine 243; pointer and MAX in the original entry order 239
// (kept, 0x175270 still matches); MAX alone on the old body 230 (0x175270 100).
// Lane 10 round 13: MAX with the flag stored after the position copy 196, but the location stays
// in EBX and 0x175270 stays 100.
// Round 9 (lane 11, fast): location read once through its address with retail's entry order (head
// read, metadata call, position copy, then the flag store) 232 (location stacked, flag still
// stacked); flag-first parameter order through a temporary forwarder (diagnostic) 251, with the
// location address read 232; retail entry order alone 251 (caller 0x175270 21 -> 100); head read
// before the metadata call 216.
// Prior shared position base 256; child byte stride 258; direct input 265; radius expression 256;
// selected influence 259; active first 253; stored gate 256; activation default 284.
// Early activation 246 with inherited address read rejected; scalar homes 256; short viewer 299;
// allocation failure first 256; maximum branches 257; direct input/early activation 253;
// delayed child handle 252 kept; one initialized/link record pointer 300.
// Round 108 (221, was 252): frame 0x10/0x14, activation and viewer/child homes remain.
// Scan influence accumulator 285; closed double scope 252; shared salted index 228;
// explicit zero falloff 245; allocation-failure return 252; combined 221;
// shared child pointer 228; direct allocated handle 336; complete falloff arms 221.
// Later child count 282; viewer while 252; conditional viewer 282; allocation/link pool scope 252;
// entry child ordinal 252; native position member 252; later child handle 285;
// later child count/handle 280; published gate across getter 254; direct bitfield conditional 271.
// Shared point setter 287; complete position value 282; persistent override influence 289;
// viewer-row cursor 281; library position copy 252.
// Round 111: Per-viewer Boolean override 210 (its 1/0 value written out at each use, not kept); unsigned-word viewer 249; readonly metadata 221; activation-byte contract 221; Boolean/word combination 232; Boolean/late activation 243; Boolean/closed doubles 210.
// Direct sqrt/single-use maximum 221/221; selected falloff expression 241;
// immutable coordinates 221; entry metadata lifetime 221; immutable pool/location receivers
// 221/221; child age initialization first 221. Original arithmetic, receivers and stores restored.
// Round 201: Activation positions 1/3/4/5/6 221 each; reused influence 240; complete viewer arms 238; chained initialization 221; count/handle lexical pair 221; conditional accumulator update 276.
// Round 113: Native viewer state member 221; early child head with shared-index/falloff context 222; child metadata byte cursor 221; default-first reused override float 221; separate allocation handle 228.
// Round 117: Activation in viewer loop 247; child while 221; range-success first 221; native child table 221; reused range limits 243.
// Activation after copy 224; early head with late activation 223; count after position 221;
// combined entry publications 223; named scaled distance 221; coordinate names 221; NONE links first 221; predecessor before head 221.
// Override domain, squared magnitude, child zero chain, early zeros, override/zeros and
// magnitude/zeros 221 each; active arm with zeros 215; all three domains 221; isolated active arm
// 215 plain.
// Consistent live pool/direct location/both 215/215/215; all squared terms 215;
// live pool with squared terms/selected influence 215/238; native element index
// alone/with live pool 215/215.
// Round 157: Outcome-arm activation publication, active byte/bool 220/220; with early head 219/219;
// inactive byte/bool 224/224; with early head 223/223.
// Double coordinate orders xzy/yxz/yzx/zxy/zyx 217/217/218/218/217;
// coordinate declarations before viewer 215; complete scalar phase declared first 215;
// coordinate declarations at activation scope 215.
// Native Boolean activation destination, published gate, live child count and both 215/251/247/265;
// the same four with read-only metadata 215/251/247/265. Boolean published gate with one actual
// location-parameter-slot read 232 (exact-only, reverted).
// override-receiver 215; head-link 215.
// override-receiver-head-link 215; tail-link 215.
// override-receiver-tail-link 215; head-link-tail-link 215.
// override-receiver-head-link-tail-link 215; links-before-index-helper 215.
// Round 144: native flag word with real/bool/word/long override carriers 234/233/232/234;
// the corresponding mask extractions 234/233/232/234. Shared type and body restored;
// frame 0x10/0x14, activation and metadata/viewer/child homes remain.
// Round 368: publication/scan position and viewer-field pointer/reference pairs 274/274/274/274;
// the same receivers with closed coordinate-double phase 274/274/274/274.
// Round 195: bounded-viewer NONE test removed 259; native salted index 300;
// implicit Boolean override 260; both 300. Viewer-row cursor with those four 259/300/260/300.
// All regress; original ordinal test, field observations and child indexing restored.
// Getter inline/noinline/inline-noinline/force-inline declarations 215/215/215/397;
// child updater inline/noinline/inline-noinline/force-inline declarations 215 each.
// Getter force-inline grows to 875 bytes and loses the child match; all declarations restored.
// Activation bool/byte/long/inverse carriers 215/215/229/231; the same carriers with position-boundary publication 252/252/251/231. Frame 0x10/0x14 and activation/metadata/viewer/child homes remain.
// Complete metadata native/library copies 254/254; with complete viewer point 237/237;
// early head with native/library metadata 269/269, and with complete viewer point 237/237.
// Round 233: Paired head/activation timing, base/Boolean destination/short viewer/both
// 251/251/283/283; native index with base/Boolean/short/all 299/299/291/291.
// Round 235: Both range/both ramp/all maximum predicate operands reversed, singly,
// each pair and all three 215 each; all three with both native ramp boundaries named
// at their expressions 215. Frame and input allocation unchanged; original body restored.
// Viewer-bound signed short/long snapshots with long/short/word/while traversal
// 266/265/262/266/266/264/263/266; exact-only field snapshots restored. Frame/input homes remain.
// Native unsigned head/tail/traversal handle domains, seven combinations, 215 each;
// all three with unsigned produced handle 215. Original fields/carriers restored.
// Round 246: age/flag-word/both library fills 217/215/217;
// adjacent-link fills alone/with age/flags/both 219/229/219/229;
// all fields with individual link fills 217. Original scalar initialization restored.
// Round 191: Child receiver shared through initialization/link/update 226; link/update
// only 274; single post-allocation receiver 234; early new-child link clear 275.
// Those four with closed coordinate-double phase 226/274/234/275.
// Original acquisitions, double lifetimes and executable body restored.
// Round 254: completed geometry then global override, original/staged-square/closed-coordinate/
// shared-scale and all combinations 246 each (725 bytes). Original override guard, double
// producers and all executable source restored; frame and input homes remain.
// Round 195: Queued-function inline/noinline/combined/forceinline declarations 215 each;
// each with closed coordinate-double lifetime 215. Original boundary and body restored;
// frame and activation/location transport remain.
// Round 81: retail's child link is the existing 0x2483f0 inlined (salt from the element, ordinal from
// the pointer difference); calling it keeps a call in this /Ob1 file: alone 303, with the entry
// reordered as below 299, inline/forceinline declarations 299/299; field_4 read before the getter
// call and the activation store after the position copy 251 (fast). A /Gd compile-only first
// pass loads the location pointer once (so it becomes the register argument) and the activation
// twice. All restored.
// Round 81 lead (fast; all restored): a position pointer for the copy and the three loop-1 reads
// gives retail's convention (bool in AL, location stacked; 274); with field_4 read before the getter
// and the flag stored after the copy 277; plus a nested MAX for the fade combine 243; plus the loop
// arm first 247; plus the fade default-first in 0x249bb0's shape 246 (aligned similarity 0.32 ->
// 0.70). Left: our frame stays 0x10 because the position pointer folds into [edi+0x10] (retail keeps
// it in ecx and spills field_4 to a fifth slot); then the child link (retail inlines 0x2483f0) and
// the x87 cleanup after the square root.
// Round 250: complete native location layout 239; early head 240;
// inactive-first 243; both 244; those four with a closed coordinate
// phase 239/240/243/244. Original input type, header and body restored.
// Round 72: the child table read each pass (field_34 + i * 0xb8; retail re-reads [ebp+0x34]) 243;
// existing distance3d 241; vector3d_from_points3d + magnitude3d (retail's x87 pops before the
// radius multiply) 242, alone on this body 227; allocation result in its own local assigned at
// the join (retail tests it twice) 230, with magnitude3d 229; existing 0x249bb0 for the fade with
// an inline/forceinline declaration here 294/294 (not inlined); with `inline` on its definition
// 166 (retail inlines it: its zero default is the early `movaps xmm0, xmm3`), but 0x174420
// inlines it too (213 -> 337, retail calls it); inactive arm first on that 248; the existing
// forceinline link body (retail inlines 0x2483f0, salt read first) gives retail's ECX position and
// spilled head but moves the location into EBX, 253.
// Round 386: isolated metadata/head/count lexical homes in all seven subsets 227 each;
// all three with an earlier uninitialized count declaration 227. Original body restored.
// Round 173: existing falloff helper with ordinary/inline/forceinline/noinline contracts
// 307/226/226/307; each with the complete coordinate-double phase closed 307/226/226/307.
// One-count inline gain restored, including all provider declarations; input delivery
// agrees, frame 0x10/0x14 and metadata/x87/child allocation remain.
// Round 397: pool retained across the child loop 234; direct remaining count 226;
// both 231; pool/count/both with read-only metadata 234/226/231;
// pool/count with plain while, mutable/read-only metadata 231/231.
// Original body restored; frame, metadata/head homes and coordinate cleanup remain.
// Round 400: Native seventeen-value output array reference, with read-only metadata, word
// viewer ordinal and indexed child-table observations in all eight subsets
// 227/227/259/259/243/243/246/246. Original pointer contract and caller declaration restored.
// Round 251: actual metadata/allocated/linked/updated external object references
// singly 227 each; metadata with each child 227 each; all four 227. Original
// distinct pointer receivers restored; metadata/head/frame and x87 homes remain.
// Round 187: Complete squared-distance with scaled-double/coordinate-scope/early-falloff masks 0-7
// 227/227/227/227/227/227/227/227. Original arithmetic and full body restored.
// Round 274: Native shared signed count field with independent child-count declaration,
// bounded short viewer and indexed child-field subsets 227/227/259/259/243/243/264/264.
// Original shared header and complete body restored; frame and metadata/head homes remain.
// Round 411: Existing allocator inline/noinline/combined/forceinline declaration contracts,
// each with original/early child-counter declaration, 240/240/227/227/240/240/463/463.
// The frame remains 0x10; forced allocation expands to 917 bytes. Original counter,
// complete body and both allocator declarations restored; metadata/head and x87 homes remain.
// Round 214: salted index before disjoint initialization with an allocation/link-only receiver;
// receiver and index declaration priorities, later/earlier failure paths 235/235/238/238/235/235/238/238.
// Original acquisitions and stores restored; frame 0x10/0x14, metadata/head homes and x87 cleanup remain.
// Round 431: Complete viewer lookup via both branches then immutable conditional result, each with
// original/viewer/child-cursor/position receiver identities: 234/234/234/234/234/234/234/234. Original
// pointer producer, receiver identities and complete executable body restored.
// Round 317: Child byte-offset induction with readonly metadata/live count/independent ordinal
// subsets 243/243/198/198/243/243/198/198. Plain 198 with live tag count kept;
// original scalar math, allocation/link stores and callback tuple preserved.
// Round 319: Existing salt-first link on the byte-offset graph, live/early/late count,
// distinct or shared allocation/link receiver 285/286/286/285/286/286; separate
// allocation result with live/early count 282/283. All restored; frame and x87 homes remain.
// Round 321: Whole-scan maximum on the retained child byte-offset graph; short viewer/closed coordinate phase/child-count snapshot subsets: 251/250/251/250/241/246/241/246.
// Round 321: Frames stay 0x0c or 0x10 versus 0x14; original influence publication, live count, distance tree and body restored.
// @retail 0x248970
void function_248970(s_particle_location_datum *arg_0, bool arg_5, real arg_1, s_particle_system_datum *arg_2,
	real *arg_3, transform4x3f const *arg_4)
{
	s_248970 *local_20 = (s_248970 *)arg_0;
	long local_0 = local_20->field_4;
	s_248971 *local_1 = (s_248971 *)arg_2->function_1751d0();
	long child_count = local_1->field_30;
	local_20->field_10 = arg_4->position;
	point3f *position = &local_20->field_10;
	local_20->field_2 = arg_5;
	if (!arg_5)
	{
		local_20->field_1c = 0.f;
		for (long local_3 = 0; local_3 < *(short *)((byte *)g_4e8c20 + 8); ++local_3)
		{
			s_player_state const *local_6 = 0;
			if (local_3 != NONE && g_4686c4 != NONE)
				local_6 = (s_player_state const *)((byte const *)(g_4e9bd4 + local_3) + 0xb8);
			double local_7 = (double)position->x - local_6->position.x;
			double local_8 = (double)position->y - local_6->position.y;
			double local_9 = (double)position->z - local_6->position.z;
			double local_4 = sqrt(local_9 * local_9 + local_8 * local_8 + local_7 * local_7);
			real local_10 = (real)(local_4 * local_6->radius);
			real local_11 = 0.f;
			if (g_510c50 && ((byte *)g_510c50)[5])
				local_11 = 1.f;
			else if (local_10 > local_1->field_18 && local_1->field_24 > local_10)
			{
				if (local_1->field_1c + local_1->field_18 > local_10)
					local_11 = (local_10 - local_1->field_18) * local_1->field_20;
				else if (local_10 > local_1->field_24 - local_1->field_28)
					local_11 = (local_1->field_24 - local_10) * local_1->field_2c;
				else
					local_11 = 1.f;
			}
			local_20->field_1c = MAX(local_20->field_1c, MAX(local_11, TEST_FIELD_BIT(arg_2->flag10) ? 1.f : 0.f));
		}
	}
	else
		local_20->field_1c = 1.f;
	*(real *)((byte *)arg_3 + 0x1c) = local_20->field_1c;
	long child_offset = 0;
	long head = local_0;
	for (long local_14 = 0; local_14 < child_count; ++local_14)
	{
		s_record_pool *local_15 = g_51ec88;
		if (head == NONE)
		{
			head = record_pool_allocate(local_15);
			if (head != NONE)
			{
				c_247470 *local_16 = DATUM(local_15, c_247470, head);
				local_16->field_8 = NONE;
				local_16->field_2 = 0;
				local_16->field_4 = NONE;
				local_16->field_c = 0.f;
			}
			if (head == NONE)
				break;
			c_247470 *local_17 = DATUM(local_15, c_247470, head);
			long local_18 = data_datum_index(local_15, local_17 - (c_247470 *)local_15->data);
			local_17->field_8 = NONE;
			if (local_20->field_4 == NONE)
				local_20->field_4 = local_18;
			if (local_20->field_8 != NONE)
				DATUM(local_15, c_247470, local_20->field_8)->field_8 = local_18;
			local_20->field_8 = local_18;
		}
		c_247470 *local_19 = DATUM(local_15, c_247470, head);
		local_19->function_247470(arg_1, arg_2, (s_object_2470e0 *)(local_1->field_34 + child_offset), (s_247fd0 *)arg_3, arg_4, arg_2->color);
		child_offset += 0xb8;
		head = local_19->field_8;
	}
}
