#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "data_iterator.h"
#include "unknown_067e10.h"
#include "vehicle_ray.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"

struct s_player_creation_record;
class c_class_6a600;
extern byte g_4cf77b;
extern byte g_4cf77a;

#pragma pack(push, 2)
struct s_player_creation_record
{
	bool active;
	bool field_2_2;
	short controller_index;
	long value04;
	s_machine_address machine;
	dword key[3];
	byte unknown1a[2];
	dword configuration[0x24];
	byte unknownac[0xe4 - 0xac];
};
#pragma pack(pop)

void __stdcall function_14c090(long index, bool reuse, s_player_creation_record const *record);
void function_694c0(c_class_6a600 *world, long index);
void ai_player_add(long index);
void function_157be0(dword index);
void function_157ed0(long index);
void function_152df0(long index);
point3f *function_b9dd0(long object_index, point3f *point);
struct s_location;
void function_b75a0(long object_index, point3f const *position, vector3f const *forward,
	vector3f const *up, s_location const *location, bool update);
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray, long ignore, long ignore2);
void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
void function_1c7e70(long player_index, short value);
void __stdcall function_14e0f0(long player_index, long target_index, point3f const *position,
	point3f const *alternate);
void function_152340();

// @retail 0x14bf80
void __stdcall function_14bf80(long player_index, s_player_creation_record const *record)
{
	function_14c090(player_index, false, record);
	++*(long *)g_4e8c20;
	if (g_4cf770 && !g_4cf77b)
		function_694c0((c_class_6a600 *)g_4cf77c, player_index);
	function_157ed0(player_index);
}

// @retail 0x14bc00
long function_14bc00(long player_index, s_player_creation_record const *record)
{
	long index = function_16b990(g_4e8c24, player_index);
	if (index != NONE)
	{
		function_14c090(index, true, record);
		*(short *)(g_4e8c24->data + (index & 0xffff) * 0x21c + 0x218) = 0;
		if (!*((byte const *)record + 1))
		{
			++*(long *)g_4e8c20;
			if (g_4cf770 && !g_4cf77b)
				function_694c0((c_class_6a600 *)g_4cf77c, index);
			ai_player_add(index);
			function_157be0(index);
		}
	}
	return index;
}

void function_14bac0();

static __forceinline void players_pending_creation_process(s_record_pool_iterator * iterator_reference)
{
	long absolute_index;
	while ((absolute_index = data_next_absolute_index_inlined(iterator_reference->data, iterator_reference->index + 1)) != NONE)
	{
		byte *player = iterator_reference->data->data + iterator_reference->data->size * absolute_index;
		long index = (*(short *)player << 16) | absolute_index;
		iterator_reference->index = absolute_index;
		if (!(player[2] & 2))
		{
			s_player_creation_record record;
			memset(&record, 0, sizeof(record));
			record.machine = *(s_machine_address *)(player + 0x14);
			record.controller_index = *(short *)(player + 0x1c);
			record.value04 = *(long *)(player + 0x20);
			memcpy(record.key, player + 4, sizeof(record.key));
			record.active = true;
			memcpy(record.configuration, player + 0xd4, sizeof(record.configuration));
			function_152df0(index);
			function_14c090(index, false, &record);
			++*(long *)g_4e8c20;
			if (g_4cf770 && !g_4cf77b)
				function_694c0((c_class_6a600 *)g_4cf77c, index);
			function_157ed0(index);
		}
		if (!g_4cf77a)
			player[2] |= 8;
	}
}

// @retail 0x14bac0
void function_14bac0()
{
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	
	s_record_pool_iterator *iterator_reference = &iterator;
	players_pending_creation_process(iterator_reference);
}
// score 222 (was 222): Frame 0x50 (original 0x44) versus retail 0x3c; centre/second-iterator slot sharing, readiness homes and the second scan's call remain.
// Round 9 (lane 11, fast): second loop through data_datum_iterator_next as retail calls it 257 (frame 0x44:
// center 0x10, destination 0x1c, alternate 0x28, second iterator 0x34; retail packs the iterator over the
// probe point at 0x1c); plus a bare block around the probe/center code 257 (identical code); per-marker
// probe points with the center call written in both arms 302 (1080 bytes, not merged); first iterator
// declared in the first block 257; iterator and probe point renamed 257 (identical layout).
// Round 139: retail's two iterators are separate locals (first loop at frame 0x30, second at 0x1c
// sharing the marker probe point's slot) and the second loop calls 0x6b380; split iterators with
// the second through data_datum_iterator_next 257 (frame 0x44), both inline 234; per-marker probe
// point locals on top 257 (destination 0x10, probe/second iterator 0x1c, alternate 0x2c as in
// retail; only the df5f0 center stays above at 0x38); a bare block around the probe/center code
// 257 (link-time code generation ignores it); center/height/radius through an inline helper 257
// (center joins 0x1c, alternate drops to 0x28, iterator to the top); with the loop as an inline
// helper too 260; whole marker search as one inline helper 259/255; declaration order 257;
// alternate/destination renamed 257/257.
// Round 94 (not kept, gain under 8): first loop through the existing datum iterator 234; plus block-local marker
// points 231; plus readiness declarations swapped 230. Retail packs its second (16-byte, 0x6b380) iterator with
// offset, marker points and center at +0x2c and alternate with the first iterator at +0x3c (frame 0x3c). Second loop
// through its own datum iterator and 0x6b380 257/254/259; stamp-first visit test 230; for-loop spelling 230;
// marker memcpy 230; marker element pointer 246 (gives retail's add/add marker address, grows the first loop).
// Existing first/second/both iterator calls 280/247/263; typed request/marker/both records 234/234/234; read-only request record 234; accepted-point memcpy 234.
// frame 0x34 versus 0x3c, query/iterator homes and visit-counter reloads differ;
// destination byte 234; alternate byte 234.
// destination word 243; alternate word 264.
// destination long 244; alternate long 265.
// both words 271; both longs 258.
// Earlier: frame 0x34/0x3c, point/iterator homes and counter reloads remain.
// Existing point difference 234; height/radius/both scalar homes 234/234/234;
// destination/alternate/both marker results 237/243/244; both plus difference 244.
// Round 211: Destination/alternate/both result names 234/234/234; marker input memcpy 234; result
// names/native recipient 257. Frame, iterator sharing and result homes remain.
// Round 215: function-scope shared iterator 234; readiness declaration order 233;
// scenario receiver lexical order 234; function-scope translation 234; both empty probe do/while forms 234.
// Offset ikj/jik/jki/kij/kji 238/237/239/240/238;
// position xzy/yxz/yzx 234/233/234.
// Shared marker query output/pointer-result/reference 332/332/332, each
// with native recipient 321; native point result 303, with native recipient 280.
// Forced output/pointer-result/reference 234/234/234, each with native recipient
// 257; forced point result 269, with native recipient 302. Original restored;
// query/recipient overlap, result-byte homes and frame 0x34/0x3c remain.
// root slot 239; visit count 234; root slot visit 239; native root table 234; native root slot 239; native
// root visit 234; native root slot visit 239; readonly scenario 234.
// Round 329: coherent pool/request/scenario observations 240/234/264; pool/request 240;
// pool/scenario 284; request/scenario 264; all three 284; all three with shared point difference 284.
// Original receivers and the shared iterator body restored.
// Round 150: Actual accepted destination/alternate/both pointer results 271/255/278; with native
// result-tested recipients 261/266/266. Both optional points initialized NULL at request entry 282;
// with native recipients 289. Pointer domain and iterator combinations do not recover frame or
// readiness homes; every original body statement restored.
// Round 210: direct destination/alternate/both query buffers 256/255/272;
// with native recipient iterator 266/265/281; both with late center 272, native recipient/late center 281.
// Round 172: Native selected/current/both player receivers 234/234/234; with native recipients
// 257/257/257; readonly both 234 and with native recipients 257. Original body, iterator graph and
// include list restored. Frame 0x34 versus 0x3c, point/iterator and readiness homes and
// visit-counter scheduling remain.
// Actual ray storage 234; with relocation-point reuse 231; native recipients 257;
// both 254; reversed readiness with ray/reuse/recipients/all 233/230/262/259.
// Original storage and iterator restored; frame and readiness homes remain.
// One shared native datum iterator, called/inlined first scan and native/called second scan:
// 353/234/421/376; reversed readiness declarations 334/233/421/376. All restored.
// Round 197: Existing point-query/relocation ordinary-inline-noinline declarations, all eight
// alternatives, 234 each. Original point/iterator graph and all declarations restored.
// Round 352: readiness at request entry, alone and with point/bounds/both output declarations,
// 274 each; early point/bounds/both declarations without readiness 234 each; complete
// function-entry graph 288. Frame stays 0x34; original storage and observations restored.
// Marker-test readiness results, destination/alternate/both 237/243/244; with
// native recipients 262/278/261. Both results plus fallback query result 252;
// with native recipients 278. Original points, flags and shared iterator restored.
// Round 62: retail's frame fits two 16-byte datum iterators, the first sharing its slot with the
// alternate point and the second with the centre and offset. Second loop through the existing
// datum iterator 257 (frame 0x44; both readiness homes then agree); plus a block scoping the
// centre 257 (identical under LTCG, 0x38 compile-only); first loop on a datum iterator through an
// inline next 257 (frame 0x48); readiness declarations swapped 262; centre declared first 257.
// Lane 10 r11: first iterator scoped to its block with the existing datum iterator for the second
// loop 257 (frame 0x44); centre/height/radius in their own block 257 (no change); point names
// sorting as retail's slots 234 (identical code); final scan through an early-return do-while
// next-index helper 270 (1026 bytes; gives retail's or/inc/js scan exactly); with the datum
// iterator second loop 270 (1027; retail's call to 0x6b380 exactly). Retail also re-reads the
// visit counter at each test, where ours keeps it in EBX. Original body restored.
// Round 80: the visit test through the existing 0xbec70 285 (called, not inlined); the root search in
// 0xbaf80's own shape (NONE result, plain while) 298; first iterator scoped to its block with the second
// scan through 0x6b380 257 (frame 0x44); stamp-first comparison 234 (identical); the visit test through
// 0xbec70's inline stamp-parameter shape 239 (indexed store as retail, but no stamp copy). Retail's
// `mov edx, ecx` before the stamp store occurs in no other retail function. Original body restored.
// Lane 10 r13: kept 230: first scan through the existing data_datum_iterator_next_called (retail's
// 16-byte iterator, data at +4 and index at +0xc, the other stores dropped; frame 0x38, 234), each
// marker probe in its own block-local point with the centre declared for 0xdf5f0 (231; destination
// now at retail's 0x20), readiness declarations in the other order (230; the homes stay swapped).
// Second scan through 0x6b380 on its own iterator 257/259 (frame 0x48, no slot sharing); the centre
// probing in a bare block 257 (no sharing); second iterator scoped to its block 230 (identical);
// separate point declarations in five orders 233 each (layout unchanged).
// Round 377: one native iterator on the first-pass/probe graph, original/request/marker
// carrier combinations 334/333/333/334/333/333/332/333. All give frame 0x50 and 1084 bytes;
// original iterators, signed carriers, probes and readiness restored.
// Round 245: shared point setter at translation 230; overlap-safe marker inputs 230;
// both 230; overlap-safe accepted points 241, with translation/marker inputs/both 241/244/244;
// complete publication graph with native recipient iterator 262. Original source restored.
// Round 211: On the retained native-first-iterator graph, one point workspace shared with
// relocation/probes/both 233/233/230; each with native recipients 262/262/259; complete sharing
// with earlier bounds declarations 230, with native recipients 259. All restored; frame 0x44/0x3c,
// readiness and recipient storage remain.
// Existing object getter with original/shared displacement 230/230; each with native recipients
// 259/259; existing root helper with original/shared displacement 288/288, with native recipients
// 286/286. Original ancestry, queries and iterators restored; frame/readiness sharing remains.
// Round 171: unsigned player/unit/root datum-handle carriers in all seven subsets 230 each;
// all three with the native result-tested recipient iterator 259. Original signed handles and
// traversals restored; point/recipient sharing, readiness and frame 0x44/0x3c remain.
// Round 238: full signed request/target marker/both carriers 267/240/275;
// alternate marker 230, with request/target/both 267/242/276;
// all three with the native recipient iterator 276. Original short carriers and traversals restored.
// Round 216: first iterator complete defaults/aggregate/CRT clear 230 each; scoped recipient
// aggregate/CRT clear 230 each; both aggregates 230; first aggregate/native recipient aggregate
// 250; both cleared/native recipient 279. Original initialization and iterators restored; frame
// 0x44/0x3c, readiness homes and recipient overlap remain.
// Shared in-place vector addition with native recipient/difference/query-input combinations 230/259/230/259/230/259/230/259; original storage and helpers restored.
// Round 175: Complementary destination/alternate/both readiness 233/240/242;
// with the native result-tested recipient iterator 254/254/259; both as byte/long
// 242/286. Original readiness, iterators, points, counters and executable body restored.
// Existing in-place vector sum with shared displacement/live ancestry headers/both 230 each;
// sum alone 230; all four with the native recipient call 259 each. Original geometry,
// both iterators and includes restored; frame 0x44/0x3c and readiness homes remain.
// Round 89: the readiness homes follow the second scan (an in-memory iterator through 0x6b380 frees BL
// for the alternate flag, as in retail; frame 0x48 259); final scan through the existing inline record
// iterator 268 (retail's or/inc/js scan); with the 0x6b380 second scan 270; final scan on the global with
// the three `players` reloads dropped 295, with the 0x6b380 scan 294; with that scan, centre/height/radius
// or its iterator declared at the block top 259/259 (declaration scopes do not move the packing).
// Round 227: Existing final reset record-inline/datum-inline/called/step producers
// 268/268/306/290; with native recipient iterator
// 270/270/328/296. Original final scan and both request traversals restored;
// frame 0x44/0x3c, point/recipient sharing and readiness homes remain.
// Distinct native 16-byte iterator role names and entry recipient reservation, all seven
// non-original subsets, 259 each; both named native workspaces at entry 259. Frame 0x48;
// original iterator identities, reservation lifetimes and complete body restored.
// Returned relocation output 243; paired initial marker snapshots 230; both 243;
// selected-unit reuse alone 245; returned output plus unit reuse 222 kept (1036 bytes);
// marker/unit snapshots 245; complete graph 222; with native recipients 228. Four complete
// returned-point buffer/pointer role-name families 222 each, all frame 0x50. Original t05
// identities retained; selected-unit value begins at its original bounds read and serves every
// subsequent recipient call. Frame grows from 0x44 to 0x50; readiness and native recipient sharing remain.
// Round 212: complete origin value with pointer/value destination and scalar/shared difference
// 319/316/319/316; native recipient counterparts 318/361/318/361 (fast control 230). Original input
// observations, points and both traversals restored; frame/readiness sharing remains.
// Round 56: retail's frame is 0x3c and its locals (relative to the post-prologue esp E) are:
// the readiness byte at E+0x13, the player at E+0x14, the scenario (reused for the radius) at E+0x18,
// the height at E+0x1c, the destination point at E+0x20, one shared point at E+0x2c (the marker
// point, the alternate marker point and the centre written by the capsule call), the alternate
// point at E+0x3c and the first scan's iterator at E+0x40/E+0x48; the second scan's 16-byte
// iterator sits on the shared point's slot at E+0x2c and is stepped by the out-of-line 0x6b380.
// Our frame is 0x44. Merging the centre into the marker point and moving the second iterator
// into its block gives 0x34, keeping the marker point as a 16-byte iterator 0x38, and using the
// out-of-line 0x6b380 for the second scan 0x44/0x48; all of those score 233-262 against the
// retained 230, so the original body stands. What is left is the frame size itself and the
// point/destination slot order, which our compiler keeps the other way round.
// Returned relocation output 243; paired initial marker snapshots 230; both 243;
// selected-unit reuse alone 245; returned output plus unit reuse 222 kept (1036 bytes);
// marker/unit snapshots 245; complete graph 222; with native recipients 228. Four complete
// returned-point buffer/pointer role-name families 222 each, all frame 0x50. Original t05
// identities retained; selected-unit value begins at its original bounds read and serves every
// subsequent recipient call. Frame grows from 0x44 to 0x50; readiness and native recipient sharing remain.
// Round 431: relocation buffer reused as accepted destination, original/native recipients 222/237;
// with shared query/bounds point 233/237; accepted alternate 222/237, with shared query/bounds 231/237.
// Accepted-output reuse shrinks frame to 0x44; query/bounds sharing reaches 0x38, native recipients stay 0x44;
// retail needs 0x3c. Original returned-output/selected-unit graph restored at frame 0x50.
// Round 440: Complete native request-row value, native/library copy and producer/entry storage, original/native
// recipient boundary: 255/275 repeated four times. All native row storage and original field observations restored.
// Round 239: shared relocation/bounds buffer, probes, native scan representation and
// readiness order: 222/233/222/233/222/233/222/233; actual out-of-line recipient
// step 228/229/228/229, reaching frame 0x40 with shared probes. Complete selected-unit
// field observations on those four graphs 264/264/263/263 (1032 bytes). Original
// returned-output/selected-unit graph, point buffers and both traversals restored.
// @retail 0x14b6a0
void function_14b6a0()
{
	byte *globals = (byte *)g_4e8c20;
	s_record_pool *players = g_4e8c24;
	short request = *(short *)(globals + 0xa4);
	if (request != NONE)
	{
		s_record_pool_iterator iterator;
		long player_index = *(long *)(globals + 0xa8);
		byte *player = players->data + (player_index & 0xffff) * 0x21c;
		byte *scenario = (byte *)g_4e0350;
		byte *record = *(byte **)(scenario + 0x134) + request * 14;
		if (*(short *)(record + 8) != NONE)
		{
			byte *markers = *(byte **)(scenario + 0x1e4);
			point3f const *destination = (point3f *)(markers + *(short *)(record + 8) * 0x38 + 0x24);
			point3f const *origin;
			if (*(short *)(record + 6) != NONE)
				origin = (point3f *)(markers + *(short *)(record + 6) * 0x38 + 0x24);
			else
				origin = (point3f *)(*(byte **)(scenario + 0x10c) + *(short *)record * 0x44 + 0x24);
			vector3f offset;
			offset.i = destination->x - origin->x;
			offset.j = destination->y - origin->y;
			offset.k = destination->z - origin->z;
			++g_4de2fc;
			g_4de2f8 = true;
			s_data_datum_iterator player_walk;
			player_walk.data = players;
			player_walk.index = NONE;
			while (data_datum_iterator_next_called(&player_walk))
			{
				long unit = *(long *)(player_walk.datum + 0x2c);
				if (unit != NONE)
				{
					byte *headers = g_4e0300->data;
					long root;
					do
					{
						root = unit;
						byte *object = *(byte **)(headers + (unit & 0xffff) * 12 + 8);
						unit = *(long *)(object + 0x14);
					} while (unit != NONE);
					if (g_4de300[root & 0xffff] != g_4de2fc)
					{
						g_4de300[root & 0xffff] = g_4de2fc;
						point3f translated;
						point3f *position = function_b9dd0(root, &translated);
						position->x += offset.i;
						position->y += offset.j;
						position->z += offset.k;
						function_b75a0(root, position, NULL, NULL, NULL, false);
					}
					players = g_4e8c24;
				}
			}
			g_4de2f8 = false;
		}
		if (*(long *)g_4e8c20 > 1)
		{
			bool have_alternate = false;
			bool have_destination = false;
			point3f destination, alternate;
			short target_marker = *(short *)(record + 0xa);
			if (target_marker != NONE)
			{
				point3f point = *(point3f *)(*(byte **)(scenario + 0x1e4) + target_marker * 0x38 + 0x24);
				while (function_168f40(0x800005, (s_vehicle_ray const *)&point, NONE, NONE)) {}
				have_destination = true;
				destination = point;
			}
			short alternate_marker = *(short *)(record + 0xc);
			if (alternate_marker != NONE)
			{
				point3f point = *(point3f *)(*(byte **)(scenario + 0x1e4) + alternate_marker * 0x38 + 0x24);
				while (function_168f40(0x800005, (s_vehicle_ray const *)&point, NONE, NONE)) {}
				alternate = point;
				alternate.z += 1.0f;
				have_alternate = true;
			}
			point3f center;
			real height, radius;
			long target_unit = *(long *)(player + 0x2c);
			function_df5f0(target_unit, &center, &height, &radius);
			if (have_destination)
				destination.z += radius;
			else if (!function_168f40(0x800005, (s_vehicle_ray const *)&center, NONE, NONE))
			{
				destination = center;
				have_destination = true;
			}
			if (have_destination)
			{
				iterator.data = g_4e8c24;
				iterator.datum_index = NONE;
				iterator.index = NONE;
				while (data_iterator_next_calling(&iterator))
				{
					if (iterator.datum_index != *(long *)((byte *)g_4e8c20 + 0xa8))
					{
						function_1c7e70(iterator.datum_index, g_4686c4);
						function_14e0f0(iterator.datum_index, target_unit, &destination,
							have_alternate ? &alternate : NULL);
					}
				}
			}
			players = g_4e8c24;
		}
		*(short *)((byte *)g_4e8c20 + 0xa4) = NONE;
	}
	long index = NONE;
	while ((index = data_next_absolute_index_inlined(players, index + 1)) != NONE)
	{
		byte *player = players->data + players->size * index;
		if (!player) break;
		*(short *)(player + 0x2a) = NONE;
	}
	function_152340();
}
