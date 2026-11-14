// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_09A9F0.CPP: the turret simulation entity definition (vtable at
   0x452788) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include "unknown_xa19f52.h"
#include <string.h>

#define OBJECT_HEADER(index) (&((s_object_header *)g_4e0300->data)[(index) & 0xFFFF])
#define OBJECT(index) (OBJECT_HEADER(index)->object)

void object_dispatch_attach_simulation_entity(long object_index);
void object_dispatch_detach_simulation_entity(long object_index);
__declspec(noinline) void function_bb7b0(long object_index);
// @retail 0x9f9d0
long c_turret_entity_definition::v0()
{
	return 0xf;
}

// @retail 0xa3980
const char *c_turret_entity_definition::v1()
{
	return "turret";
}

// @retail 0xa06a0
long c_object_type_definition::v2()
{
	return 0x90;
}

// @retail 0x9fce0
long c_object_type_definition::v3()
{
	return 0x14;
}

// @retail 0xa0250
long c_object_type_definition::v4()
{
	return 0xa;
}

// @retail 0x9a9f0
long c_object_type_definition::v5()
{
	return 0x3f;
}

// @retail 0xa59d0
bool c_object_type_definition::v6(s_entity *entity)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		result = v28(entity->object_index);
	}
	return result;
}

// @retail 0xa5bb0
bool c_object_type_definition::v7(s_entity *entity)
{
	if (entity->object_index != NONE && entity->field_6)
	{
		return true;
	}
	return false;
}

// @retail 0xa5750
bool c_object_type_definition::v8(long a, long b)
{
	return true;
}

// @retail 0xa3b10
void c_turret_entity_definition::v9(long a, long b, long *size)
{
	*size = 0x74;
}

// @retail 0xa3b20
void c_turret_entity_definition::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
	{
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	}
	function_11c9c0(buffer, size, "turret creation: relevance=%5.3f", relevance);
}

// @retail 0xa0ab0
void c_object_type_definition::v11(long a, long b, long c)
{
	((s_flags_a6900 const *)b)->function_a6900((long *)c, this);
}

// @retail 0xa3b90
void c_turret_entity_definition::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
	c_turret_entity_definition *volatile self = this;
	function_a6660(info, stream);
	if (stream->size_in_bytes * 8 - stream->bit_position >= 1 && info->identifier != NONE)
	{
		stream->data[stream->bit_position / 8] |= (byte)(1 << (stream->bit_position % 8));
	}
	stream->bit_position++;
	if (info->identifier != NONE)
	{
		simulation_write_entity_identifier(info->identifier, stream);
	}
}

// @retail 0xa3c00
bool c_turret_entity_definition::v13(long a, s_entity_info *info, s_bitstream *stream)
{
    c_turret_entity_definition const *volatile unused_this = this;
	bool valid = function_a6810(info, stream);
	if (function_1957d0(stream))
	{
		long index = function_1959c0(stream, 10);
		info->identifier = ((byte)function_1959c0(stream, 4) << 28) | index;
	}
	else
	{
		info->identifier = NONE;
	}
	if (stream->bit_position <= stream->size_in_bytes * 8 && valid)
    {
        return true;
    }
    return false;
}

// @retail 0xa3cd0
bool c_object_type_definition::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
	bool result = false;
	if (function_a69a0(a1, a2 & 0x3ff, a3, a5, a7, v33(), a8))
	{
		result = true;
	}
	return result;
}

// @retail 0xa0960
bool c_object_type_definition::v15(long a, long b, long c, long d, s_bitstream *stream)
{
	c_object_type_definition const *volatile saved_definition = this;
	if (function_a6d50(b, d, stream) && stream->bit_position <= stream->size_in_bytes * 8)
	{
		return true;
	}
	return false;
}

// @retail 0xa3d10
bool c_object_type_definition::v16(long a, long b, long c)
{
	c_object_type_definition *volatile definition = this;
	return function_a7180(a, b);
}

// @retail 0xa4870
bool c_object_type_definition::v17(long a, long b, long c)
{
	return true;
}

// @retail 0xa59f0
void c_object_type_definition::v18(s_entity *entity, long b, s_entity_state *state)
{
	v26(entity->object_index, b, state);
}

// @retail 0xa07b0
bool c_object_type_definition::v19(long a, long b, long c, s_entity_data *data)
{
    c_object_type_definition *volatile self = this;
    (void)&self;
	bool result = false;
	memset(data, 0, sizeof(s_entity_data));
	if (function_a5bd0((s_entity_info const *)b, (s_z_initial_state *)data))
	{
		result = true;
	}
	return result;
}

// @retail 0xa5a00
bool c_object_type_definition::v20(s_entity *entity, long *index, long c, long d)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		long value = *index;
		if (OBJECT(entity->object_index)->type == 1)
		{
			value &= 0x3ff;
		}
		*index = v27(entity->object_index, value, c, d);
		result = true;
	}
	return result;
}

// @retail 0xa3c70
void c_turret_entity_definition::v21(s_entity *entity)
{
	s_entity_info *info = entity->info;
	if (entity->object_index != NONE)
	{
		OBJECT(entity->object_index)->field_d4 = entity->field0;
	}
	if (info->identifier != NONE)
	{
		long identifier = info->identifier;
		byte salt = (byte)(((long)((dword)identifier >> 28) + 1) % 16);
		info->identifier = (salt << 28) | (identifier & 0x3ff);
	}
}
// MiniMax G (11-14): no new form, no lower score (49): retail keeps the false result in bl and sets it only after the
// attachment, ours keeps it in ebp; retail reloads the object table's data pointer and recomputes the scaled index
// between the header flag test and the object read, ours shares one address between the two reads.
// MiniMax S (11-14): no new form, no lower score (49): retail keeps the false result in BL from before the creation call and sets it only after the attachment, branches on the creation call's result instead of materialising it as a 0/1 value, borrows EBP around the header flag test and loads the object table's data pointer again for the object read, where ours keeps the result in EBP and shares one header address between the two reads; the result byte at the entry and the data reload are not reachable from this file.
// score 49 (was 49; m3 lane 2 r79 at the end): unchanged this round: retail keeps the false result in BL from before the creation
// call and sets it only after the attachment, and it branches on the creation call's result instead of
// materialising that comparison as a 0/1 value; retail also borrows EBP around the header flag test and
// loads the object table's data pointer again for the object read, where ours shares one address between
// the two reads and keeps the result in EBP.
// Earlier score line (49, was 49): unchanged this round (m3 lane 13 r77 too): the head's register roles still differ - retail saves
// earlier first lines 49 (was 49), 49 (was 49); retail's bool result in BL, the EBP borrow around the
// header test and the data reload remain; round 151's entry is the last one above the marker.
// MiniMax A (11-11, run 58): no lower score (49) - taking the result from the entity's published index after
// the arm scores 66 and the bool result set after the attach scores 80; retail keeps the false result in BL from
// before the creation call, saves EBP only around the header test and looks the header up twice, while ours
// keeps the validity result in EBP and shares one header address between the flag test and the object lookup.
// MiniMax R (11-09): no new form, no lower score; retail keeps the false result in BL, xors BL before the creation call and sets BL after it, while ours returns the result in EAX and holds the last argument in EBP, so our frame is four bytes wider (77 against the baseline's 49); the result bool declared before the creation call (77), the object lookup written as an array element (77) and the creation call's output taken through the parameter's own address (87) all score worse.
// MiniMax A (11-06, run 52): no new form, no lower score (49) - a byte result set after the attach
// reaches retail's shape but scores 77, and reading the header through the entity changes nothing;
// retail keeps the result in bl and looks the header up twice.
// Earlier (49, was 49), merged from a second score line: unchanged this round: the head's register roles still differ - retail saves
// MiniMax D (run 46): the early-return body 77, the bool result 69, the bool result with the header
// pointer in a named local 77 (the header lookups still merge into one ECX entry and EBP becomes a
// scratch register); no lower score.
// Earlier note (49, was 49): unchanged this round (m3 lane 13 r77 too): the head's register roles still differ - retail saves
// EBX and keeps a false result in BL from before the creation call, pushes EBP only around the header
// test and leaves the table global in EDX, reloading its data and recomputing the index for the
// object; ours keeps the validity result in EBP and shares one header address between the flag test
// and the object lookup, so the index arithmetic stays common.
// MiniMax R (11-09): no new form, no lower score; retail keeps the false result in BL, xors BL before the creation call and sets BL after it, while ours returns the result in EAX and holds the last argument in EBP, so our frame is four bytes wider (77 against the baseline's 49); the result bool declared before the creation call (77), the object lookup written as an array element (77) and the creation call's output taken through the parameter's own address (87) all score worse.
// MiniMax A (11-06, run 52): no new form, no lower score (49) - a byte result set after the attach
// reaches retail's shape but scores 77, and reading the header through the entity changes nothing;
// retail keeps the result in bl and looks the header up twice.
// MiniMax D (run 46): the early-return body 77, the bool result 69, the bool result with the header
// pointer in a named local 77 (the header lookups still merge into one ECX entry and EBP becomes a
// scratch register); no lower score.
// MiniMax D (run 46): the early-return body 77, the bool result 69, the bool result with the header
// pointer in a named local 77 (the header lookups still merge into one ECX entry and EBP becomes a
// scratch register); no lower score.
// Retail holds a bool result in BL from before the creation call, saves EBP
// only around the header test (pushed and popped there) and keeps the table global in EDX, reloading
// its data and recomputing the index for the object; ours keeps a long result in EBP and shares one
// header address. With the bool result every header/object spelling still shares the address, and EBP
// then caches the last argument across both virtual calls (160 bytes). Zero frame agrees.
// Tried: incoming output flags 87; later result 80; both 89; invalid header first 75; shared header
// 77; incoming flags and header 87; guard exits 77; result from validity 69; byte/native/short result
// 76/49/52; published index 70; full acceptance 73; incoming flags with byte/native result 64/69;
// early long/byte/short/signed-byte success result 67/70/65/70; byte after notification 70; native
// header acceptance 65; native rejection first 52; combined native acceptance 67; early bool result
// set after the attach 77, with the header or the object read through the entity's stored index
// 77/77, with the result declared after the in/out value 77; native result with the header read
// through the entity's index 49; signed-byte/unsigned-word/unsigned-native validity 76/52/49; shared
// success exit bool/long/byte 75/67/70; shared validity exit native/word 52/55; result declaration
// first, output role name, both, native-result header record 49 each; shared header/object/both
// getters 49/49/49; both getters with early Boolean success 77; success-default bool/native/byte at
// entry 72/49/72; after creation 70/52/70; created-index validity return 70; actual attached-index
// result 72. Bool result with: a local for the object table 78; forceinline header-test and object
// getters 77; a named flag-test bool 77; the NONE test repeated in an else-if 75; header and object
// through the entity's stored index 77; cross-file getters, guard returns and == 0 test (screened)
// still share the address; the deleted test as a named bool 77; the deleted arm first 75; the result
// set before the attach call 72 (176 bytes, the result spills to the stack); the second virtual call
// after the block under `result && value` 77; the deleted arm returning at once 75.
// retail holds a bool result in BL from before the creation call, saves EBP only
// around the header test and, for the object, reloads the table's data and recomputes the index (the
// table global stays in EDX): a CSE kill, as a store between the flag test and the object read gives.
// Ours keeps a long result in EBP and shares one header address. Tried: incoming output flags 87;
// later result 80; both 89; invalid header first 75; shared header 77; incoming flags and header 87;
// guard exits 77; result from validity 69; byte/native/short result 76/49/52; published index 70;
// full acceptance 73; incoming flags with byte/native result 64/69; early long/byte/short/signed-byte
// success result 67/70/65/70; byte after notification 70; native header acceptance 65; native
// rejection first 52; combined native acceptance 67; early bool result set after the attach 77 (160
// bytes: BL right, but EBP then caches the last argument across both virtual calls and the header
// address is still shared), with the header or the object through the entity's stored index 77/77,
// with the result declared after the in/out value 77; native result with the header through the
// entity's index 49; signed-byte/unsigned-word/unsigned-native validity 76/52/49; shared success exit
// bool/long/byte 75/67/70; shared validity exit native/word 52/55; result declaration first, output
// role name, both, native-result header record 49 each; shared header/object/both getters 49/49/49;
// both getters with early bool success 77; success-default bool/native/byte at entry 72/49/72, after
// creation 70/52/70; created-index validity return 70; actual attached-index result 72; bool result
// with a table local/forceinline getters/named flag/else-if NONE retest/entity index 78/77/77/75/77;
// cross-file getters, guard returns and an == 0 test (screened) still share the address. Round 6:
// deleted index set to NONE then published and retested 38, as a ternary 38 (both give retail's object
// read; the join keeps `cmp; mov [edi+8]; je` because a join block holding a store is never threaded);
// ternary field store with a field retest 43; flagged-arm store with a field retest 55; repeated index
// store in the clear arm 42. Round 36: a compiler barrier after the header test with the bool result
// 42 (retail's EBP window and data reload; the index arithmetic stays shared), with a word index 39;
// the header flag as a value tested twice 66; a barrier after a join on the local 38; the index
// re-read from the entity after the clear arm 67 (none kept). Round 33: a compiled-out
// `do {} while (false)` before the object read 49, with the bool result 77 (the empty block gives no
// kill here); every read through the entity's stored index with a bool/long result 82/69; the table's
// data in a local for both lookups 49.
// retail holds a bool result in BL from before the creation call, saves EBP
// only around the header test, and reloads the table data and recomputes the index for the
// object read (only the global load is shared); ours keeps a long result in EBP and shares one
// header address. Tried: incoming output flags 87; later result 80; both 89; invalid header
// first 75; shared header 77; incoming flags and header 87; guard exits 77; result from validity
// 69; byte/native/short result 76/49/52; published index 70; full acceptance 73; incoming flags
// with byte/native result 64/69; early long/byte/short/signed-byte success result 67/70/65/70;
// byte after notification 70; native header acceptance 65; native rejection first 52; combined
// native acceptance 67; signed-byte/unsigned-word/unsigned-native validity 76/52/49; shared
// success exit bool/long/byte 75/67/70; shared validity exit native/word 52/55; result
// declaration first, output role name, both, native-result header record 49 each; shared
// header/object/both getters 49/49/49, both getters with early bool success 77; success-default
// bool/native/byte at entry 72/49/72, after creation 70/52/70; created-index validity return 70;
// attached-index result 72; early bool result set after the attach 77 (BL right, but EBP caches
// the last argument and the header address stays shared), with the header or the object read
// through the entity's index 77/77, result declared after the in/out value 77; native result
// with the header read through the entity's index 49. A store between the test and the object
// read (or a join holding one) gives retail's reload: deleted index set to NONE, published and
// retested 38 (join left unthreaded); ternary field store + field retest 43; flagged-arm store +
// field retest 55. Compiler barrier after the test (with the early bool) 42: then only the index
// arithmetic is still shared; plus a (word) index at the object read 39; barrier after a join
// on the local 38; flag value tested twice 66; index re-read from the entity after the clear 67.
// Round 6: deleted index set to NONE then published and retested 38 and as a ternary 38 (both
// retail's object read; the join keeps `cmp; mov [edi+8]; je` because a join block holding a store
// is never threaded); repeated index store in the clear arm 42. With the bool result every
// header/object spelling still shares the address, so EBP is free and caches the last argument
// across both virtual calls (160 bytes). Bool result with: a local for the object table 78;
// forceinline header-test and object getters 77; a named flag-test bool 77; the NONE test repeated
// in an else-if 75; header and object through the entity's stored index 77; cross-file getters,
// guard returns and == 0 test (screened) still share the address.
// Round 118: the object store and attach call as the existing 0xbb780 (src/unknown_0bb760.cpp, the
// only other caller of 0x108e10, same two statements) 49; with the early bool result 77 (160 bytes,
// LTCG still shares the header address with the inlined body), declared after the in/out value 77,
// as long 67; 0xbb780 given the entity's stored index 49; (diagnostic) 0xbb780 moved to a non-SSE
// file with the early bool 77.
// is never threaded); ternary field store with a field retest 43; flagged-arm store with a field
// retest 55; repeated index store in the clear arm 42.
// false BL lifetime, temporary EBP save and independent header/object lookups remain.
// retail holds a bool result in BL from before the creation call, saves
// EBP only around the header test and loads the header and the object separately; ours keeps
// a long result in EBP and shares one header address.
// EBP only around the header test and loads the header and the object separately; ours
// keeps a long result in EBP and shares one header address. Incoming output flags 87; later
// result 80; both 89; invalid header first 75; shared header 77; incoming flags and header
// 87; guard exits 77; result from validity 69; byte/native/short result 76/49/52; published
// index 70; full acceptance 73; incoming flags with byte/native result 64/69; early
// long/byte/short/signed-byte success result 67/70/65/70; byte after notification 70; native
// header acceptance 65; native rejection first 52; combined native acceptance 67; early bool
// result set after the attach 77 (160 bytes: BL right, but EBP then caches the last
// argument across both virtual calls and the header address is still shared), with the
// header or the object read through the entity's stored index 77/77, with the result
// declared after the in/out value 77; native result with the header read through the
// entity's index 49.
// Zero frame agrees; early false BL, EBP lifetime and separate header lookup remain.
// Earlier: result from validity improves the score; frame 4 versus retail 0, early EBP
// save and cached header lookup remain. Incoming output flags 87; later result 80; both 89;
// invalid header first 75; shared header 77; incoming flags and header 87; guard exits 77; result
// from validity 69.
// Earlier: native validity result removes the frame gap at retail length;
// EBP result, early false lifetime and shared header lookup still differ. Incoming flags 72;
// byte/native/short result 76/49/52; published index 70; full acceptance 73;
// incoming flags with byte/native result 64/69.
// Earlier: frame agrees; false-result BL, saved EBP and shared header lookup differ.
// Early long/byte/short/signed-byte success result 67/70/65/70; byte after notification 70;
// native header acceptance 65; native rejection first 52; combined native acceptance 67.
// Signed-byte/unsigned-word/unsigned-native validity 76/52/49; shared success exit bool/long/byte
// 75/67/70; shared validity exit native/word 52/55.
// EBP result, early false lifetime and shared header lookup remain.
// Result declaration first, output role name, both, native-result header record 49 each;
// shared header/object/both getters 49/49/49; both getters with early Boolean success 77.
// Success-default bool/native/byte at entry 72/49/72; after creation 70/52/70;
// created-index validity return 70; actual attached-index result 72.
// EBP only around the header test and reloads the table's data and recomputes the index for the
// object (the table global stays in EDX); ours keeps a long result in EBP and shares one header
// address. With the bool result every header/object spelling still shares the address, so EBP is
// free and caches the last argument across both virtual calls (160 bytes). Zero frame agrees.
// Tried: incoming output flags 87; later result 80; both 89; invalid header first 75; shared
// header 77; incoming flags and header 87; guard exits 77; result from validity 69;
// acceptance 67; early bool result set after the attach 77, with the header or the object read
// through the entity's stored index 77/77, with the result declared after the in/out value 77;
// native result with the header read through the entity's index 49; signed-byte/unsigned-word/
// unsigned-native validity 76/52/49; shared success exit bool/long/byte 75/67/70; shared validity
// exit native/word 52/55; result declaration first, output role name, both, native-result header
// record 49 each; shared header/object/both getters 49/49/49; both getters with early Boolean
// success 77. Bool result with: a local for the object table 78; forceinline header-test and
// object getters 77; a named flag-test bool 77; the NONE test repeated in an else-if 75; header
// and object through the entity's stored index 77; cross-file getters, guard returns and
// == 0 test (screened) still share the address.
// Round 38 (lane 9), on the early bool (77, reference rebuild): the attach called with the entity's
// stored index 82; `result = true` before the attach 72; result declared after the value 77; the
// NONE test on the entity's field 77; `result = true` after the second call 79. EBP keeps caching
// the last argument in each.
// Also: bool result with a table local/forceinline getters/named flag/else-if NONE retest 78/77/77/75;
// deleted index cleared and retested as a ternary 38; repeated index store in the clear arm 42;
// cross-file getters, guard returns and an == 0 test (screened) still share the header address.
// Round 99 (lane 6): bool result with the arms in retail's order, a header-table pointer local for
// the test and the local index elsewhere 77 (the local is propagated, the address still shared);
// the same through the entity's index 82 (157 bytes); without the table local 77; `if (index !=
// NONE && flagged) entity->object_index = NONE;` then `if (entity->object_index != NONE)` 61 (the
// memory retest is not threaded); the same on the local, published before the retest, 68.
// Lane 18 r1, all on the early bool with the last two arguments of the second virtual call read through
// `*(long const volatile *)&e` / `&f` (diagnostic: it stops EBP caching f, as retail; allowed only with a
// match): 53, only the shared header address left (retail reloads the table's data and recomputes the
// index for the object, borrowing EBP for the test); the header test or the object through the entity's
// index 53/53; the test indexed through a word cast 50 (index recomputed, data still shared); a repeated
// `entity->object_index = object_index;` between the test and the object read 47 (gives retail's EBP
// test and data reload exactly: something retail stores there blocks the reuse); the clear-in-test then
// memory retest form 35 (header and object computed apart as retail, but the retest is not threaded);
// the same on a local 53 (threaded, shared again); every index read from the entity 41 (159 bytes).
// Round 439: complete native 12-byte header value, native/library copies at lookup/entry
// storage, 104 each; actual post-attachment Boolean success with those four copies, 82 each.
// Both result graphs grow a frame; original pointer observations and virtual calls restored.
// Round 110 (fast): bool result as a success flag set in the unflagged arm, the attach in a following
// `if (result)` 72 (threaded early, address shared, result in a stack byte); the flagged arm first 52;
// `result = true` in the attach arm of a long false result 67; the long result cleared with the field
// in one `result && flagged` test, then `if (result)` 52; a validity bool cleared with the field, then
// `if (valid)` 73; guard return on the field after the clear 60. Long result false, the field cleared
// in an `index != NONE && flagged` test, then `if (entity->object_index != NONE)` 45 (180 bytes: retail's
// whole header region, EBP pushed around the test and the data reloaded for the object, because the
// object read then has a predecessor without the header address; but the memory re-test is not threaded
// and the result is long in EBX); the same clearing the local then publishing it 45 (179 bytes); with a
// bool result first 53. Not kept: a field cleared in a test and re-tested.
// Round 30 (lane 11, fast; none kept): early bool with the deleted arm first and the object, attach
// and update through the entity's stored index 82 (157 bytes: the stored index is forwarded, header
// still shared); result decided at the header test, object block under `if (result)` 72 (threaded
// early, header shared, result spills); clear-in-test then memory retest on the bool result 53 (177
// bytes: retail's data reload and recomputed index, with EBX for the data, but the retest stays and
// EBP caches the last argument); object read as a raw byte offset from the pool data 77 (folded back
// into the shared header address); the in/out mask parameter's own address instead of the local copy
// 87 (152 bytes; the copy is retail's).
// Round 17 (m3 lane 2, fast): the entity store moved into the arms (index in the unflagged arm,
// NONE in the flagged arm and on the NONE path) with a bool/long result 78/96 (188/203 bytes);
// bool result with one `index != NONE && !flagged` test and the clear in its else 77; a header
// pointer local after the store, flagged arm first 75; bool result with the clear-in-test then
// memory retest 53 (177 bytes); a validity bool cleared in the flagged arm and retested 73.
// Compile-only screens: word/short/dword index casts, pointer arithmetic or byte offsets for the
// test or the object read all keep one data load; only the memory retest splits it (and then also
// reloads the table global, which retail keeps in EDX).
// Round 25 (fast): the field store and attach as a call of the existing 0xbb780 (inlined) 49 (same
// code); with an early bool result 77, flagged arm first 75; bool result with the clear-in-test and
// memory retest, 0xbb780 on the stored/local index 67/53. Data and index still shared. Restored.
// Round 35 (lane 10, fast): early bool result set after the attach 77 (160 bytes, header address shared, EBP
// caches the last argument), with the flagged arm first 75; on the long result, `result = false` before the
// index clear 50, the object read through a one-use header pointer 49, the header test indexed by a (word)
// cast 54 (182 bytes).
// Round 58 (lane 12, fast; none kept): early bool with `__assume(object_index != NONE)` at the top of
// the unflagged arm 77; the header test indexed through an unsigned `(dword)` index 77 (same code); f or
// e read once through its own address in the second virtual call 77/77 (propagated, EBP still caches f);
// diagnostic, not keepable: a repeated `entity->object_index = object_index;` before the object read,
// also with the object read through the entity's index, 42/42 (g stays in EDX and the data reloads as
// retail, but idx*12 is still shared by `shl ecx, 2`; retail shares neither, so a store kill alone is
// not retail's cause). Repeats confirming listed scores: flagged-arm clear then field retest 55,
// clear-in-test then memory retest 53.
// Round 52 (lane 16, fast), all on the early bool result: `entity->object_index = object_index;` repeated at the
// top of the unflagged arm 42 (retail's prologue, no EBP cache of the last argument, retail's EBP window and data
// reload; the index arithmetic stays shared as `shl ecx, 2` and the extra store remains), with the object read
// through the entity's stored index 42 (same code: the stored value is forwarded); the flagged index cleared, then
// published and retested 68 (retail's object read, join unthreaded); the creation result stored in the entity
// and read back 77; one header pointer for the test and the object 77; `result = true` first in the arm 72
// (176 bytes, result spilled); an unsigned object index 77. Retail's object read needs both the data reload and
// a recomputed index, as a join present at CSE time and threaded later would give.
// Round 21 (m3 lane 2, fast): bool result first with the flagged arm first and the object through a
// header-pointer local 75 (160 bytes); bool result with the second virtual call after the block under
// `result && value` 77; bool result declared after the creation call 80 (158); a header-pointer local
// for the test on the long result 49 (same code); the flagged arm first on the long result 52. The EBP
// cache of the last argument follows the shared header address (fewer live registers at the test).
// Round 42 (lane 11, fast), all with the early bool result set after the attach: a header pointer from
// `index != NONE ? header : NULL` tested for NULL, then for the flag 72 (the NULL test stays); the same
// set in an if 72; the flag test indexed through `(word)index` 72; the flagged index cleared in an
// `index != NONE && flag` test indexed through `(word)index`, then `if (entity->object_index != NONE)`
// 48 at 176 bytes (the object read is then retail's exactly: data reloaded from the kept global, index
// recomputed; left: the memory retest, EBP caching the last argument, the false exit as `xor al, al`
// and the movzx index); the object read through a second header pointer on `(word)index` 71.
// Round 12 (lane 17, fast): `entity->object_index = object_index = v29(...)` (one chained value) on the long
// result 49, on the early bool result 77; the flag tested as `(flags >> 4) & 1` on the long/early bool result
// 49/77 (same code); the chained form with the early bool and the store and attach through the existing
// 0xbb780 77 (still one shared header address under the link's inline).
// Round 28 (m3 lane 2, fast with 0x108e10/0xbb780): early bool result, the flagged arm clearing both the
// entity's index and the local, then `if (object_index != NONE)` on the local 75 (160 bytes: the join is
// threaded before CSE, so the object read still shares the header address, and EBP caches the last
// argument); the long result computed before the entity store with `return false` in the flagged arm 48,
// with the test spelled `NONE != object_index` 48; early bool with `result = true` after the attach and
// `return false` in the flagged arm 77. Permuter (600 s, 24 tries): 46, only with the second virtual call's
// arguments reordered and `value` narrowed by a cast (both change behaviour; not usable).
// Round 60 (m3 lane 9): the store and attach as the existing 0xbb780 with the early bool result, first
// built plain (earlier only fast) 77 (160 bytes: the full link inlines it and still shares the header
// address); the object pointer read before the flag test on the long/early bool result 49/77; the header
// flags in a named byte with the early bool 77; the early bool with an `== 0` flag test built 77.
// Round 65 (lane 12, fast), all on the early bool result: the NONE and flag tests as one multi-return
// inline predicate, the clear under `else if (object_index != NONE)` 77 (the predicate's exit join is
// threaded before CSE; header address still shared), with the store and attach through the existing
// 0xbb780 77; a `word` absolute index named once for the test and the object read 84 (155 bytes); the
// header data pointer named once for both lookups 78; the test through the existing object_header_get
// (slot_handler.h) with the store and attach through 0xbb780 77.
// Round 58 (lane 16, fast): early bool with the flag test through an if/return-true inline helper (a join at
// its exit) 75 (160 bytes, threaded early, header shared); the object read's table data loaded through a
// dword view (diagnostic) 75 (still shared: not a load-type split); early bool, unflagged arm first, the
// update call through the existing v23 (qualified, inlined) 70 (176 bytes, retail's layout, but v23's NONE
// test stays and EBP caches the last argument); `result = true` after the update call 79 (158 bytes); the
// NONE test read from the stored field 77 (160 bytes); early bool, unflagged arm first, the field store and
// attach through the existing 0xbb780 (inlined) 77 (160 bytes).
// Round 17 (lane 17, fast), all on the early bool result: the header flag test as an inline predicate
// with an if-built result 77, with early returns 77 (the join is threaded before CSE: the header address
// stays shared and EBP caches the last argument); the second virtual call on the local index 81;
// `entity->object_index = object_index = NONE` in the flagged arm 77; the object index declared first 77.
// Round 51 (lane 10, fast): the entity index stored inside the NONE test (NONE in its else) on the long result 93
// (194 bytes), on the early bool 68 (179 bytes); the field cleared to NONE before the creation call and the index
// stored only in a `index != NONE && !flagged` success arm, long result 79 (191 bytes), early bool 72 (176 bytes:
// the arm store between the test and the object read reloads the table data with the global kept in EDX as
// retail, but the index arithmetic stays shared as `shl ecx, 2` and EBP caches the last argument); that with the
// object and attach through the stored field 71 (176 bytes; the field is forwarded for the object read and
// reloaded for the attach).
// Round 47 (lane 11, fast): the entity's index stored only in the success arm, NONE in the else of one
// `index != NONE && !flagged` test, early bool 66 (the store after the test reloads the table data with
// the global kept in EDX as retail, but the index arithmetic stays shared and EBP caches the last
// argument); that with the long result 75 (186 bytes); with the store and attach as the existing 0xbb780
// 66; nested NONE/flag tests with a NONE store in each else 78 (188 bytes); the inverted
// `index == NONE || flagged` test 66. Retail's recomputed index needs a CSE boundary, not only a store.
// Round 127 (lane 6, fast), all with a bool result declared first and set after the attach (retail's BL: false
// before the first call, the attach argument in EBX, true after it): flagged arm first 75 (160 bytes: f cached in
// EBP, header address shared, the flagged store laid out inline), unflagged arm first 77, the value copy declared
// before the result 75, `result = true` between the field store and the attach 72 (176 bytes, result spilled);
// the clear under `index != NONE && flagged` then `else if (index != NONE)` 75 (the repeated test is threaded
// before CSE, as round 65 found for the predicate form).
// Round 24 (lane 17, fast): the object read through a NONE-checked ternary (`index != NONE ? object : NULL`)
// on the early bool 77 (folded before CSE, address shared), on the long result 64 (185 bytes: the NONE test
// stays); a separate `deleted` bool set under `index != NONE`, then `if (deleted)` clear `else if (index !=
// NONE)` the attach block, with the early bool 72, with `return !deleted && index != NONE` 67; the same with
// `deleted` as one short-circuit expression 75 (160 bytes).
// Round 47 (lane 11, fast): the entity's index stored only in the success arm, NONE in the else of one
// `index != NONE && !flagged` test, early bool 66 (the store after the test reloads the table data with
// the global kept in EDX as retail, but the index arithmetic stays shared and EBP caches the last
// argument); that with the long result 75 (186 bytes); with the store and attach as the existing 0xbb780
// 66; nested NONE/flag tests with a NONE store in each else 78 (188 bytes); the inverted
// `index == NONE || flagged` test 66. Retail's recomputed index needs a CSE boundary, not only a store.
// Round 70 (lane 12, fast), early bool result set after the attach (77, 160 bytes): with a header pointer named
// in the arm for the object 77; the flag test as `== 0` 77; the NONE test, the flag test and the attach as one
// if/else-if/else chain with an empty NONE arm 75; the value call after the block under `result && value` 77;
// a bool result from the NONE test cleared in the flagged arm 69 (178 bytes). In retail EBP is pushed only around
// the test because seven values are live there (this, entity, index, result, table, scaled index, data), as in
// matched 0x656e0/0x17bf80/0x273480; ours shares the header address, so one register less is needed.
// Round 151 (lane 4, fast), on the early bool result set after the attach: the entity's field read into a
// local before the header test 77; an object pointer named in the unflagged arm 77; the NONE and flag tests as
// breaks out of a `do ... while (false)` 75; the store and attach through the existing 0xbb780 with the update
// through the existing v23 70 (176 bytes, v23's NONE test stays); that on the long result 57 (181 bytes).
// Round 154 (lane 4, one-link harness, exact at 49), all on the early bool result: dword-cast index for the flag test, byte
// arithmetic for the flag read or the object read 77 each; object read through entity->object_index with or without the macro
// 77/77; `(word)object_index` for the object read / flag test / both forms 71/72/72 (176 bytes, index recomputed as movzx, retail
// `and`); inline or forceinline lookup helper with its own pool pointer, index local or header local 77 each; one or two named
// pool pointers for the two reads 77-78; dword-bitfield view of the index for the flag / object / both 77 each.
// Round 462: multiply-used native absolute index with accepted-handle result at creation,
// attachment, notification or accepted-branch entry: long/dword each 74/68/68/51.
// Original validity result and independent source observations restored; BL/EBP and CSE remain.
// Round 328: creation/header failure result bool/byte/short/long 83/83/64/61;
// entry failure cleared after attachment in those domains 68/68/63/65.
// Original validity result, independent observations and complete body restored; BL/EBP and CSE remain.
// M320: Twice-used notification output at creation/publication/object-store/attachment,
// native validity 61/61/62/49; early Boolean success 75/75/53/77. All restored.
// Pre-attachment Boolean form recovers the prologue, but header CSE and EBP lifetime remain.
// M325: Zero-notification completion guard with bool/byte/short/long creation-validity
// results 63/93/68/65; entry-false success 70/68/73/71. All restored;
// false BL, temporary EBP and independent header/object computations remain.
// Round 200 (m3 lane 3, fast): retail shape (false result first, true after the attach, NONE in the
// flagged else) with the header test on the stored entity field 77 (160 bytes); the header test indexed
// with `(dword)object_index` 49 (same code: the lookup still shares); bool result 69 (178 bytes); the
// result from the stored field 49 (same code); v29's output through `&d` (no copy) 69. All restored.
// Round 124 (m3 lane 8, fast): the header test indexed through `(word)object_index` with the entity's index
// stored again at the top of the unflagged arm 67 (188 bytes); the entity store after the result 49; the object
// index declared before `value` 49; the entity's field read into a local before the header test 49; the header
// test or the object read through the entity's stored index (long result) 49/49 (forwarded, same code).
// m3 lane 11 r62 (fast): bool result false first, `result = true` right after the attach (retail's BL
// sequence) 77 re-measured (160 bytes: EBP then caches the last argument for both virtual calls); with the
// header flag tested through a bit-field view of the header's flags byte 77 (same code); the result set
// after the value call 79; the entity's stored index at every use 82; the result set before the attach 72
// (176 bytes).
// Lane 18 round 36 (fast): on the long result, the flag read as the byte at +2 of the header 49 (same code); the
// object store and attach through `entity->object_index` 52; the bool result cleared at entry and set after the
// attach (re-measured) 80 (158 bytes: retail's BL and branches, the header address still shared); the validity
// test written `object_index != NONE` beside the long result 76 (189 bytes); the entity's index and the local
// assigned in one chained statement 49 (same code).
// m3 lane 14 r56 (fast): the flag test through the existing object_header_get (slot_handler.h) 49 (same code);
// bool result preset false with the flagged arm first and `result = true` before the value test 75 (160 bytes);
// the long result with `if (result && flagged) { clear; result = false; }` then `if (result)` the attach block
// 52; the flagged arm setting `result = false` before the clear 50; a long result preset false and set true
// after the attach 67. All restored.
// m3 lane 11 r62 (fast): bool result false first, `result = true` right after the attach (retail's BL
// sequence) 77 re-measured (160 bytes: EBP then caches the last argument for both virtual calls); with the
// header flag tested through a bit-field view of the header's flags byte 77 (same code); the result set
// after the value call 79; the entity's stored index at every use 82; the result set before the attach 72
// (176 bytes).
// Round 208 (m3 lane 3; one-file LTCG harness, exact for this body; real-build figures in brackets): retail's
// reloaded data pointer and recomputed index for the object (pool kept in EDX) is what a join before the object
// lookup gives, as in the matched 0x10fa80/0xd1540: a memory re-test `if (entity->object_index != NONE)` after
// `if (object_index != NONE && header flag) entity->object_index = NONE;` gives retail's lookup and BL result but
// keeps the re-test 52 [53, 177 bytes]; with the entity's index everywhere 57; the local cleared and stored once
// 60; a local re-test (`entity->object_index = object_index = NONE;`, five spellings) 59-61 [75] and a flag set in
// the header test 64-75 [72] are threaded before CSE and share the lookup again; a compiled-out
// `do {} while (false);` at the arm top/before the test/both 49 (same code), at the re-test's join 52; the header
// test through same-file inline bool helpers (5 spellings) or link-inlined helpers in another file 49 (same code).
// Round 67 (m3 lane 1, fast): the header test through a forceinline bool getter 49 (same code), on the retail shape
// (false first, true after the attach) 77; retail shape with the flag test positive and the NONE arm first 75 (160
// bytes); the positive test with the NONE arm first on the long result 52; a header pointer named for the test with
// the object read through the macro 49. Lead: retail's reloaded table data and recomputed index look like an
// out-of-line getter that LTCG inlines; no existing one was found in src/.
// B415: Full validation byte in byte/word/long/dword storage, at the test then bare entry declaration:
// B415: 49/49/49/49/49/49/49/49. NOT kept: one-use field storage does not match. Original validity result,
// B415: virtual calls and complete body restored; false BL and independent header lookups remain.
// M363: Actual input-info and both repeated virtual-argument formal lexical roles,
// seven subsets, then all with complete incoming roles:
// 49/49/49/49/49/49/49/49. NOT kept: no eligible byte match or native-supported closer.
// Original complete body restored; false BL, temporary EBP and independent header/object lookup remain.
// M367: Complete output/index/result declarations split from original assignments,
// six ranks then coherent names at both endpoint ranks: 49 each at 176/176 bytes.
// NOT kept: no byte match; original initialized declarations and complete body restored.
// False BL, temporary EBP and independent header/object computations remain.
// Round 86 (m3 lane 16, fast with 0xa5b10): the bool result with a local index 75 (160 bytes; plain agrees, so the reduced image's
// global caveat hides nothing here); the entity's index at each use 82 (157 bytes); negated flag first with an else-if NONE 77;
// a named flagged bool 77, from a ternary 75; a ternary in the test 65 (176 bytes, shift and not for the flag); a byte flags
// local 75; `||` with a nested reset 77; early returns 77 (155 bytes). A retail scan finds the header flag test followed at
// once by a recomputed lookup in no other function; elsewhere a store or a loop stands between the two.
// Round 55 (m3 lane 15, fast): on the bool result (false first, true after the attach): both lookups as `[index & 0xFFFF].flags` and
// `.object` with no `&` 77, on the long result 49 (same code); `object_index == NONE || !flag` with the attach block under an inner
// `entity->object_index != NONE` or `object_index != NONE` and the store in the else 75/75 (threaded, the lookup is shared);
// the header flag through the stored index 77; the object store through it 77, with the attach too 82. Retail loads the pool after
// the NONE test, so no NONE path reaches the second lookup there (unlike 0x10fa80, which loads the pool above its test). BL also
// keeps `f` out of EBP only in retail: every bool form here caches `f` in EBP.
// M380: Native index/attached-identity/both publications through memcpy 49/49/49,
// memmove 109/76/122; both with early Boolean completion 77/94. NOT kept: no byte match;
// original scalar stores, validity result, virtual tuples and complete body restored.
// Round 153 (fast; 0xa5a00/0xa5b10/0xa5b40/0xa5b80/0x108e10 matched throughout): the validity branch on `object_index != NONE` with
// the long result kept for the return 76 (189 bytes); the flagged arm clearing the result before the entity store 50; the in/out value
// declared, then assigned 49; the creation result through a chained `entity->object_index = object_index = v29(...)` 49; `if (value != 0)`
// 49 (same code each). Re-measured: the store and attach through 0xbb780 49.
// Round 88 (m3 lane 16, fast with 0xa5b10/0xbb780/0x108e10): early bool with the store and attach through 0xbb780 77 (160 bytes),
// with `inline` on 0xbb780's definition 77; the long result with that inline helper 49; the helper's value named first 77; a header
// reference for the test 77; early bool with the in/out value copied to a local after the creation call 75 (176 bytes, aligned 35,
// masked 27: a one-use copy, not kept).
// P383: Post-attachment notification admission in bool/byte/short/long, positive/complementary:
// 49/49/49/49/49/49/49/49. NOT kept: no native false-BL/independent-lookup equality; original workspace
// observation, validity result and whole body restored.
// P392: Consistent twice-used incoming virtual options, e/f/both long then const-long, both int/const-int:
// 49/49/49/49/49/49/49/49. NOT kept: no native false-BL/temporary-EBP/independent-lookup equality.
// Original formal reads, virtual tuples and complete body restored.
// MiniMax S (11-09): no new form, no lower score; retail still keeps the false result in BL and
// re-reads the last argument from its stack slot, while ours promotes that argument into EBP and
// returns the result through EAX, which also shifts the frame by four bytes.
// Round 159 (lane 5, fast with 0xa5a00/0xa5b10/0xa5b40/0xa5b80/0x108e10/0xbb780, all matched throughout). Reading: retail's
// temporary EBP only means the pool pointer stays live in EDX across the first data load, so the pool global is shared while
// the data load and the index are not. On the bool result (false first, true after the attach): a pool-copy local at the top
// for both lookups 74, for the flag only 60, for the object only 61 (176 bytes each, but the copy is read ahead of the virtual
// call and held in EBP); the copy inside the NONE test, one or two copies, 78/77/77/77 (160, propagated); a named header
// pointer for the flag, for the object, both, a data local 77 each; the store and attach through 0xbb780 with the local or the
// entity's field as either index 77 each; no index local at all with 0xbb780 67 (176: the index is recomputed as retail does,
// the data load still shared and `f` cached in EBP), with a pool copy for the flag 67, hand-written 82 (157), with 0xbb780
// reading through a pool copy, a named object, a named header or both 67 each (0xbb780 unchanged); an address-taken flag byte
// or a by-value flag view with a test member 67 each. A reset store ahead of a second NONE test on the entity's field, flat or
// nested, 61 hand-written, 67 through 0xbb780 (the data load is then repeated with the pool kept, but the reset path is not
// threaded: store, reload, compare); the same with the local cleared too and re-tested 75 (160: threaded early, lookups
// merged). The local cleared in the flagged arm, stored to the entity again at the join and re-tested 38 (first difference
// +0x3a: retail's whole head with the temporary EBP, the repeated data load and index; left is the join, `or eax, -1; cmp;
// mov; je` for retail's `jne` to a constant store at the end), flat 64, without the first store 68/64; the entity reset plus
// the local cleared, re-tested on the entity's field 37, entity reset only 55, index from the field 61/82. The result cleared
// in the flagged arm and re-tested: bool 73 (178), long 45, long from the NONE test 52, bool from it 69. None kept: 37/38 need
// a local cleared inside a test and tested again, without a byte match.
// m3 lane 12 r103 (fast, 0xa5b10/0xbb780/0x108e10 matched throughout), all on the long result: a pool copy declared at
// the top for the flag test 72, for the object read 72, one for each 93 (188/188/194 bytes: the copies are read ahead
// of the creation call and held across it); the value call on the local index 67; the result assigned in every arm
// (no initializer) 71.
// m3 lane 10 r87 (fast, 0xa5b10/0xa5b40/0xa5b80/0xbb780/0x108e10 matched throughout; re-measures marked): `bool result`
// with the existing 0xbb780 for the store and attach, on the local 77, on the entity's field 77 (both re-measures of
// lane 5 r159), with the header test on the field too 77; the entity store moved into the valid arm with NONE stored in a joint else 66; `bool result` with the
// store and attach written out 77 (re-measure). Diagnostic only: a second `entity->object_index = object_index;` right after the
// header test 42 (retail's head with the temporary EBP and the data reload; ours keeps the index product where retail
// recomputes it). Lead: a memory write between the test and the object read in retail's source. Also new: `bool result`
// set true in the unflagged arm (NONE stored in the flagged arm) and the store and attach under `if (result)` 72 (the
// flagged path threads to the exit as retail, but the result is spilled to a stack byte and the header address stays
// shared), with 0xbb780 72, with the arms swapped 72, that with a long result 77 (188 bytes).
// m3 lane 13 r77 (fast, 0xa5a00/0xa5b10/0xa5b40/0xa5b80 matched throughout), on the early bool result set after the attach (77,
// re-measured): the second call's last argument read once through its address (`long const *last = &f;`) 77 (folded: EBP still
// caches f); the creation result stored and tested in one condition (`(entity->object_index = object_index = v29(...)) != NONE`)
// 77 (same code); the bool assigned in every arm with no initializer 84 (157 bytes); the header test kept in a `deleted` bool,
// `if (deleted) entity->object_index = NONE;` then `if (!deleted) {...}` 72. Lead (fast, reduced-image caveat, not kept: both
// parts only steer): on that bool form a repeated `entity->object_index = object_index;` first in the unflagged arm 42 (176
// bytes: without any volatile it gives retail's whole head, BL false before the call, f re-read from its slot, EBP saved only
// around the header test, the table global kept in EDX and the data reloaded; left the scaled index shared and the extra store),
// plus the test indexed through `(word)object_index` 14 (the index recomputed for the object read; left `movzx ecx, ax` for
// retail's `mov ecx, eax; and ecx, 0xffff` and the extra `mov [edi + 8], eax`). On the 42 form: the object read through
// `entity->object_index` 42 (forwarded), the test's index as `(dword)object_index` 42 (still shared); the word cast without
// the store 72, the dword cast without it 77. So retail needs one value-preserving kill between the test and the object read
// that leaves no instruction, and a second index computation that is not CSE'd with the first. Matched v22 siblings of other
// entity definitions (0x9ba90, 0xa4880, 0x9aeb0) clear a slot to NONE and then store the new value.
// m3 lane 15 r72 (fast with 0xa5a00/0xa5b10/0xa5b40/0xa5b80; the two stacked score lines merged into the first line).
// Re-measured: the local cleared in one flat `index != NONE && flagged` test, published once and retested, long result 62
// (177 bytes), bool 68 (176: the object read is retail's because the join holds the store), flat with the first store 64;
// the listed 38 form (first store, local cleared in a nested flagged arm, stored again at the join, retested) now 64 on the
// bool result. New, all match-only (a local cleared inside a test and retested, plus a redundant store): that 38 form with
// the bool's second call on the local 65, `result = true` before the attach 75 (183 bytes); with a long result 41 (178:
// `xor ebx, ebx` before the creation call, EBP pushed around the test, the data reloaded and the index recomputed as
// retail; left the join `or eax, -1; cmp; mov [edi + 8], eax; je` and the long result), flat 41, second call on the local 46,
// a short result 42 (179); a char or byte result 38 (176 bytes: retail's BL handling, `xor bl, bl` early and `mov bl, 1`;
// left the join and `test bl, bl; setne al` for retail's `mov al, bl`), flat 38, the retest on the entity's field 38, the
// second call on the local 51; the flagged arm also storing NONE to the entity 37 (178), with the retest on the field 37,
// the byte declared first 37, a short 42 (182), the second call on the local 49, without the join store 70. The bool result
// with the false assignment as its own statement before the call 64 (VC sinks the bool's false; the byte stays early).
// M3 lane 8 r151 (fast, 0xa5b10 and the file's other functions matched throughout): on the bool result set after the
// attach (77, re-measured; EBP caches f): the creation result stored into the entity and read back into the local 77; the
// attach call given `entity->object_index` 82 (157 bytes); `result = true` after the value call 79 (158); both first two 82.
// On the long result: the attach given `entity->object_index` 52; the creation result stored and read back 49 (same code).
// Diagnostic, not kept: the bool form with constants for the value call's last two arguments 64 (155 bytes): f no longer
// takes EBP and the head through the creation call is retail's, but the header test then puts the data in EDX (dropping
// the global) and needs no EBP borrow; retail keeps the global in EDX for the object read because the data is reloaded,
// which agrees with the store-between lead above.
// m3 lane 14 r84 (fast with 0xa5b10, 0xbb780 and 0x108e10, matched throughout): a bool result taken from the flag
// test inside the NONE test (`result = !(flags & 0x10); if (result) {...} else entity->object_index = NONE;`) 69 (176 B),
// the same with the arms swapped 73, as a long 76 (190 B). The other natural spellings are listed above (early returns,
// getters, 0xbb780/v23, result widths, deleted-arm orders); the matched siblings 0x9ba90/0xa4880/0x9aeb0 clear a slot to
// NONE before storing, a dead store that is not keepable here.
// Round 171 (m3 lane 5, fast with 0xa5b10, 0xa5b40, 0xa5b80, 0x108e10; control 49): the cleared index published at a
// join and re-tested, written as one `&&` chain, re-measured (bool 64, no first store 68, long 41, byte 38: the listed
// nested scores; with the entity reset as well 64/41). On the chain: the data reload and the recomputed index come
// from the join store (the unflagged path is threaded past it), the flagged path is not threaded (`or eax, -1; cmp;
// mov; je`, where retail stores the constant at the end), and the byte result needs `setne` at the return. New: the
// value call on the local index 65; the flagged arm `{ entity->object_index = NONE; return result; }` with the join
// store kept, byte 53, bool 49 (184 B: the flagged store is then retail's, but inline and its return folds to `xor al,
// al`), long 57, nested 49 (same code); that without the join store 75 (160 B, the header address shared again).
// MiniMax G (11-11): no new form, no lower score (49); retail reloads the table's data pointer and recomputes the
// scaled index between the flag test and the object read, which needs a store between them - the only spellings
// that add one either store the entity's index a second time (32, a redundant store) or clear it up front and
// store it in the valid arm (71, and 67 with the index narrowed), so our header address stays shared and the
// result stays in ebp where retail keeps it in bl.
// m3 lane 2 r75 (fast with 0xa5b10): on the bool result set after the attach (77): `(void)&f;` 77 (same code, `f` still
// cached in EBP); the flagged arm resetting the entity's field and clearing the local, no join store, retest of the local
// 70 (char result) / 75 (bool), both threaded early (header shared); the field reset with a field retest 61 (176 B, the
// memory retest is not forwarded). Match-only, on round 72's join form (first store, local cleared in the flagged arm,
// stored again at the join, retested) with a bool result: 38 at 176 B, and all of it is retail's (head, `xor bl, bl`, the
// EBP temporary, the data reload and recomputed index, `mov al, bl`) except the join itself: ours runs the flagged path
// through `or eax, -1; cmp; mov [edi + 8], eax; je`, retail jumps to a constant store at the end. The same with the
// retest outside the NONE test, the clear as a ternary, or the clear in an else arm: 38 each (same code); with the
// entity's field also reset in the flagged arm 36 (an extra store, still not threaded). Without the join store, the
// flagged arm's local as `object_index = entity->object_index = NONE` or read back from the reset field 75 each
// (threaded early, header shared): the join with no store is threaded before CSE, the join with one never.
// m3 lane 1 r98 (control 49): no new form run; every candidate checked against this
// note is listed (the `== 0` flag test, `NONE != object_index`, the result cleared before the entity store, `value != 0`, an int
// result, split declarations, getters, early returns, the clear in an else of one `&&` test). Two leads checked, neither applies:
// the virtual v30 that 0xa5b40 calls before its detach is not the header flag test (the base returns false, the turret override
// detaches), and src/ has no out-of-line predicate on the header's 0x10 flag to call (round 67's open lead). Retail recomputes the
// whole lookup after the flag test (pool kept in EDX, data and index reloaded) with no store in between; only the match-only
// forms above (join store with a local retest 36-38, a repeated entity store plus a word index 14) reproduce it.
// 0 trials: no unlisted natural form (m3 lane 14 r84 above: "the other natural spellings are listed above").
// M3 lane 2 r79 (fast with the file's functions, 0x108e10 and 0xbb780): no new form run; the int result re-measured 49 (same
// code as the long result). Reading of retail's object lookup: the pool global stays in EDX while the data load and the
// scaled index are both recomputed, and ECX still holds the scaled index when it is recomputed, so the second lookup was never
// in the same CSE region as the header test; that is what a join in front of the object read gives when it is threaded away
// only after CSE. Every natural spelling that adds such a join (bool or local retests, early returns, predicates, do/while,
// getters) is threaded before CSE in this build, and the forms that keep it (a store at the join, a cleared field retested)
// are match-only and leave the join itself in place.
// m3 lane 6 r178 (fast with 0xa5b10/0xa5b40/0xa5b80/0x108e10/0xbb780, matched throughout): on the bool result set
// after the attach (77, re-measured: f cached in EBP from entry, so no EBP is left to borrow around the header test
// and the header address stays shared) a folded test on the second virtual call to lower its block weight, `result
// && value`, `value && object_index != NONE` or `value && entity`, 77 each (folded before allocation: f still in
// EBP); `result = true` after the second virtual call 79 (158 bytes); the creation result stored straight into the
// entity and read back into the local 77. Body unchanged.
// @retail 0xa5a60
bool c_object_type_definition::v22(s_entity *entity, long b, s_entity_info *info, long d, long e, long f)
{
    long value = d;
    long object_index = v29(b, info, &value, e, f);
    entity->object_index = object_index;
    long result = object_index != NONE;
    if (result)
    {
        if (!(OBJECT_HEADER(object_index)->flags & 0x10))
        {
            OBJECT(object_index)->field_d4 = entity->field0;
            object_dispatch_attach_simulation_entity(object_index);
            if (value)
            {
                v31(entity->object_index, value, e, f);
            }
        }
        else
        {
            entity->object_index = NONE;
            result = false;
        }
    }
    return result;
}

// @retail 0xa5b10
bool c_object_type_definition::v23(s_entity *entity, long b, long c, long d)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		v31(entity->object_index, b, c, d);
		result = true;
	}
	return result;
}

// @retail 0xa5b40
bool c_object_type_definition::v24(s_entity *entity)
{
	bool result = false;
	if (entity->object_index != NONE)
	{
		if (!v30(entity->object_index))
		{
			function_bb7b0(entity->object_index);
			function_b8540(entity->object_index);
		}
		entity->object_index = NONE;
		result = true;
	}
	return result;
}

// @retail 0xa5b80
bool c_object_type_definition::v25(s_entity *entity)
{
	long index = entity->object_index;
	bool result = false;
	if (index != NONE)
	{
		// Retail reloads the object index for the virtual call.
		result = v32(*(long volatile *)&entity->object_index);
	}
	return result;
}

// @retail 0xa3990
void c_turret_entity_definition::v26(long index, long b, s_entity_state *state)
{
	s_object_view *object = OBJECT(index);
	memset(state, 0, sizeof(*state));
	s_object_view *current = OBJECT(index);
	state->field4 = current->definition_index;
	state->field0 = current->field1a;
	state->field8 = current->field_af;
	state->fieldc = current->field24;
	state->field10 = function_a5930(object->field14);
}

// @retail 0xa3a00
long c_object_type_definition::v27(long a, long b, long c, long d)
{
	c_object_type_definition const *volatile saved_definition = this;
	return function_a5e70(a, b & 0x3ff, d);
}

// @retail 0xa09b0
bool c_object_type_definition::v28(long index)
{
	s_object_view *object = OBJECT(index);
	bool result = false;
	if (object->field_ab != 2 || !TEST_FIELD_BIT(object->flag2))
	{
		result = true;
	}
	return result;
}

// @retail 0xa3a20
long c_turret_entity_definition::v29(long a, s_entity_info *info, long *c, long d, long e)
{
	long result = NONE;
	long found = function_a58d0(info->identifier);
	if (found != NONE)
	{
		long child = OBJECT(found)->first_child;
		if (child != NONE)
		{
			long definition_index = info->definition_index;
			do
			{
				s_object_view *object = OBJECT(child);
				if (object->definition_index == definition_index && object->field_d4 == NONE && result == NONE)
				{
					result = child;
				}
				child = object->next_sibling;
			}
			while (child != NONE);
		}
	}
	return result;
}

// @retail 0xa3a90
bool c_turret_entity_definition::v30(long index)
{
	bool result = false;
	s_object_view *object = OBJECT(index);
	long field14 = object->field14;
	if (field14 != NONE)
	{
		object_dispatch_detach_simulation_entity(index);
		object->field_d4 = NONE;
		object->field_d8 = 0;
		result = true;
	}
	return result;
}

// @retail 0xa3ae0
void c_object_type_definition::v31(long a, long b, long c, long d)
{
	long index = b & 0x3ff;
	if (index)
	{
		function_a6430(this, index, a, d);
	}
}

// @retail 0x9bed0
bool c_object_type_definition::v33()
{
	return false;
}

// @retail 0xa0410
bool c_object_type_definition::v34(long a, s_entity_data *source, long *block)
{
	memcpy(block, source->block, sizeof(source->block));
	return true;
}

// @retail 0x9dcd0
void c_object_type_definition::v35(long a, s_entity_data *data, long *block)
{
	memcpy(data->block, block, sizeof(data->block));
}

#include "object_types_21_2.h"

class c_generic_type : public c_object_type_definition
{
public:
	virtual const char *v1();
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual bool v28(long index);
};

// @retail 0xa0690
const char *c_generic_type::v1()
{
	return "generic";
}

// @retail 0xa06b0
bool c_generic_type::v28(long index)
{
	s_object_view *object = OBJECT(index);
	bool result = false;
	if (object->field_ab != 2 || (((1 << object->type) & 2) && !TEST_FIELD_BIT(object->flag2)))
		result = true;
	return result;
}

// @retail 0xa0440
void c_item_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "item creation: relevance=%5.3f", relevance);
}

// @retail 0xa0890
void c_generic_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "generic creation: relevance=%5.3f", relevance);
}

// @retail 0xa0ee0
void c_projectile_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "projectile creation: relevance=%5.3f", relevance);
}

// @retail 0xa2310
void c_weapon_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "weapon creation: relevance=%5.3f", relevance);
}

// @retail 0xa40a0
void c_device_type::v10(s_creation_request *request, long parameter, long size, char *buffer)
{
	real relevance = -1.0f;
	s_creation_weight *entry = &g_4cef68[request->definition_index];
	if (!(entry->weight > g_45dbd8))
		relevance = function_aa4d0(1, &request->entity_index, entry->maximum_distance, (s_relevance_observers const *)parameter, 0);
	function_11c9c0(buffer, size, "device creation: relevance=%5.3f", relevance);
}


// @retail 0xa02e0
long c_item_type::v27(long a, long b, long c, long d)
{
 // Preserve the local definition pointer stored in the retail stack frame.
 c_object_type_definition *volatile definition = this;
 byte *object = (byte *)OBJECT(a);
 long result = function_a5e70(a, b & 0x3ff, d);
 if (b & 0x400)
 {
  bool flag = (object[0xc1] & 1) != 0;
  if (flag != *(bool *)(d + 0x90))
  {
   *(bool *)(d + 0x90) = flag;
   result |= 0x400;
  }
 }
 return result;
}

// @retail 0xa0be0
long c_projectile_type::v27(long a, long b, long c, long d)
{
 // Preserve the local definition pointer stored in the retail stack frame.
 c_object_type_definition *volatile definition = this;
 long result = function_a5e70(a, b & 0x3ff, d);
 if (b & 0x400)
 {
  byte *object = (byte *)OBJECT(a);
  bool flag = (object[0xc1] & 1) != 0;
  if (flag != *(bool *)(d + 0x90))
  {
   *(bool *)(d + 0x90) = flag;
   result |= 0x400;
  }
 }
 return result;
}

class c_handle_table_450cd0;
__declspec(noinline) bool function_99640(c_handle_table_450cd0 *self, long handle);

// @retail 0xa0e50
bool c_projectile_type::v8(long a, long b)
{
 bool result = true;
 byte const *state = *(byte const **)(a + 0x14);
 if (*(long const *)(state + 0x14) != NONE)
 {
  byte *table = *(byte **)(*(long *)(*(long *)b + 4) + 8);
  if (!function_99640((c_handle_table_450cd0 *)(table + 0x30), *(long const *)(state + 0x14))) result = false;
 }
 return result;
}

// score 5 (was 5): retail masks the 16-bit group index with and eax, 0xffff,
// ours truncates it with movzx eax, ax. Writing (index & 0xffff) gives the and
// but swaps the object (esi) and flags (edi) registers (12). Tried with the
// mask: flags declared first / long / b used directly (12), early returns
// (47), real pointer into the group (15), a 12 byte group struct indexed (12),
// one (flags & 0x800) && group != NONE test (24, 12), unsigned index (12),
// 0x9d450's object address form (12); (dword)(word)index is unchanged (5).
// Dropping the volatile this copy loses retail's push ecx/[esp] store (55).
// With the mask, any extra use of object (an added read at the end) gives it
// esi again, so the swap is a use-count tie that the separate word read of
// (word)index breaks. Also tried with the mask: bit tests as (flags >> 10) & 1
// (12), result declared first (12), the compares' operands swapped (17), a
// group_index local (12); the base class v27 call without the copy (52).
// Round 34 (LTCG harness, base 1 = 5): with the mask, the volatile copy, object and flags in the five
// other declaration orders 12/51/12/51/51; a plain long or dword index 12/12; the index field read at
// each use 12 (also with b direct 12); the group value through an inline value getter, read twice,
// 14 (b direct 14); a `real const &` for the two power reads 12 (b direct 12); permuter (600 s, 53
// tries) none better.
// Round 49 (one-link LTCG screen, exact at 5; with the mask 12 unless noted): 1-8 unused locals before the
// copy, the flags or the call 12 each (no numbering tie); every narrow read (`*(word *)`, `(short)` or
// `(word)` with or without `& 0xffff`, `(long)(word)`) compiles to movzx (5), every full-width mask to the
// swap: re-read through a dword or long pointer, `(dword)index`, a 12-byte group struct, a typed state
// pointer for d (word form 5), dead object loads or copies 12; object looked up again for the index/store
// 56/55; the base class v27 called inline 51 (as the listed base-class call). Fast: the result bit set
// before the store (mask) 14; the call's mask from b only (mask) 12; a typed state view for d with the word
// read 5, with the mask 12; the index's low word read from the object 5; the power value in a local (mask)
// 17. Re-measured: an index local (mask) 12; the 0x800 and NONE tests combined (mask) 12; a value pointer
// into the group (mask) 15.
// Round 32 (lane 17, fast): the masked group address written at both uses 15; with the mask, the index read
// into a value local right after the call 49 (188 bytes), a const object pointer 12, `b` passed to the call and
// the flags local declared after it 12, the 0x400 test and compare as nested ifs 12.
// Round 94, all with the full-width mask: a handle pointer 12; a reference test with a direct index
// read 12; a direct test with a reference index 12; a const object pointer 12; an un-named group
// entry 15; const flags 12; a mutable handle reference 12; the entry predeclared in the flag scope
// or the function scope 12/12. Original narrow-read body restored.
// Round 88 (one-link LTCG screen, exact at 5), all with the full-width mask: `do {} while (false);` at each of 20 statement
// boundaries 12 (47, 188 bytes, at the two after the 0x800 store); the power copied as long, both stores or the first only,
// 12/12; the index by value 12; the power through a const pointer 12; a group index local before the entry 12; the NONE
// test as `index + 1 != 0` 12; `b` throughout with no flags local 12.
// Round 215 (lane 3, fast), full-width mask unless noted: the mask alone 12; a dword bitfield's low word 12; a 12-byte group array indexed
// `[index & 0xffff]` 12; a by-value one-long handle with inline accessors 12; a group local masked after the test 12; `(dword)(index << 16) >> 16` 12;
// the object through a typed view 12; a word bitfield's low word 5 (movzx, no mask).
// Round 78 (m3 lane 1, fast; control 5), all with the full-width mask unless noted: a dword result 12; a groups
// pool local 12; the entry as `&data[(index & 0xffff) * 12]` 12; a const entry pointer 12; the multiply as
// `12 * (index & 0xffff)` 12; a dword result with the word read 5. Re-measured: const flags 12 (round 94); a const
// value local for the two group reads 24.
// Round 95 (m3 lane 16, fast), all with the full-width mask: the swap is a use-count tie (diagnostic: without
// the 0x400 test the object takes esi, 40); flags read once through the parameter's address 12; the call's mask
// read through the parameter's address with flags copied after or before the call 12/12 (the loads merge before
// allocation); an object header local 12; object and flags declared, then assigned 12. Retail's code in this file
// never zero-extends with a reg-reg movzx, only `and r, 0xffff`.
// Round 158 (fast), all with the full-width mask: the object local named `device` 12; the mask on a second read of
// the field through an indexed long view 12; the field read directly at the NONE test and the mask 12 (re-measured);
// the group index read into a local at entry 50; flags declared before the lookup with the call's mask from `b` 12;
// the object through the existing inline object_get 12.
// Round 165 (m3 lane 5, fast, 0xa5e70 kept): both value checks through one shared inline (value, source, bit) update
// helper returning the bit or 0, with `result |=` at each, 56 (190 bytes) with the word read and 58 (192) with the mask (the
// zero arm is not threaded); the 0xa5e70 call through an inline (object index, flags, update) forwarding helper with the mask
// 12, passing the masked flags 12, with the word read 5. The tie is not moved by the call's argument webs.
// Round 189 (m3 lane 4, fast, 0xa5e70 and the sibling v27s kept): the volatile definition copy declared after object
// and flags (mask) 51; `(word)(index & 0xffff)` 5 (same code as the word read: the narrowing conversion gives object
// ESI); the call's mask as `(word)(flags & 0x3ff)` with the index masked 12; `(dword)index % 0x10000` 12; the group
// value through the existing cross-file inline device_group_get at the compare and the store (mask) 15.
// Round 98 (m3 lane 10, fast with 0xa5e70 and the sibling v27s, which all stay matched; control 5), full-width mask unless noted:
// the base class's v27 called inline with the flags local / with `b` direct 51/51, with the word read 52 (the base is inlined,
// but its definition copy lands in an incoming argument slot, so the frame has no `push ecx`); eight renamings of the object
// local, the flags local and the `b` parameter (early and late names, the parameter itself named flags) 12 each: names do not
// order the two registers; the group value's whole address expression written at both uses 15 (word read 8: the shared address
// then includes the +4); 0xa5e70's flags parameter declared dword, word or short (definition and header; 0xa5e70 and its other
// callers still match) 12 each; the call's mask from a narrow read of the flags, `(word)flags`, `(word)b`, `(short)b` or a
// word read through the parameter's address, 12 each. Diagnostics (not kept): with the mask the flags take ESI at three uses
// each (12) and at four each (37); the object takes it with the 0x400 test removed (40); with the word read the object takes
// ESI even with one of its 0x140 reads removed (6), so the word read does not act through a use-count tie alone.
// Round 119 (m3 lane 9, fast with 0xa5e70, matched each time), with the full-width mask unless noted: the call's mask
// from the flags local and both tests from `b` 12; only the 0x800 test from `b` 12; the group value pointer (or NULL)
// from a forceinline taking the object 36 (189 bytes); the index reference bound at the function's top, before the
// call, 12, with the word read 5. In the mask listings the group data reuses ESI after the flags (retail: after the
// object). Diagnostic, compile-only: /G5, /G6, /G7 and /GB all keep `movzx eax, ax` for the word read.
// @retail 0xa3df0
long c_device_type::v27(long a, long b, long c, long d)
{
 // Preserve the local definition pointer stored in the retail stack frame.
 c_object_type_definition *volatile definition = this;
 byte *object = (byte *)OBJECT(a);
 dword flags = b;
 long result = function_a5e70(a, flags & 0x3ff, d);
 if ((flags & 0x400) && *(real *)(d + 0x90) != *(real *)(object + 0x140))
 {
  *(real *)(d + 0x90) = *(real *)(object + 0x140);
  result |= 0x400;
 }
 if (flags & 0x800)
 {
  long const &index = *(long const *)(object + 0x13c);
  if (index != NONE)
  {
   byte *entry = g_4e0328.groups->data + (word)index * 12;
   if (*(real *)(d + 0x94) != *(real *)(entry + 4))
   {
    *(real *)(d + 0x94) = *(real *)(entry + 4);
    result |= 0x800;
   }
  }
 }
 return result;
}

#include "unknown_1946f0.h"
#include "slot_handler.h"
static __forceinline long real_to_long(real value);
void simulation_write_position(s_bitstream *stream, real const *position, long bits, bool keep_inside);
void simulation_read_position(s_bitstream *stream, real *position, long bits);
void scenario_object_name_encode(long object_name, s_bitstream *stream);

struct s_z_impact_payload
{
 long object_name;
 real scale_a;
 real scale_b;
 vector3f direction;
 vector3f position;
 vector3f normal;
 word field_30;
 byte unknown32[2];
 bool field_34;
 dword field_38;
};

PRIVATE inline void z_write_impact_scale(s_bitstream *stream, real value)
{
 function_195720(stream, real_to_long(value * 126.0f), 7);
}

PRIVATE inline real z_read_impact_scale(s_bitstream *stream)
{
 long value = function_1959c0(stream, 7);
 real result;
 if (value == 0) result = 0.0f;
 else if (value >= 126) result = 1.0f;
 else result = ((126 - value) * 0.0f + value * 1.0f) * (1.0f / 126.0f);
 return result;
}

struct s_z_impact_scenario
{
 byte unknown000[0x3d8];
 long count;
 long *names;
};

PRIVATE __forceinline long z_impact_read_name(s_bitstream *stream)
{
 long result = NONE;
 long index = function_1959c0(stream, 9) - 1;
 if (index != NONE)
 {
  s_z_impact_scenario *scenario = (s_z_impact_scenario *)g_4e0350;
  result = NONE;
  if (scenario && scenario->count > 0)
   if ((index < 0 ? 0 : (index > scenario->count - 1 ? scenario->count - 1 : index)) == index)
    result = scenario->names[index];
 }
 return result;
}

// @retail 0xa18c0
void c_projectile_impact_effect_event::v9(long a, void const *data, s_bitstream *stream)
{
 s_z_impact_payload const *event = (s_z_impact_payload const *)data;
 scenario_object_name_encode(event->object_name, stream);
 z_write_impact_scale(stream, event->scale_a);
 z_write_impact_scale(stream, event->scale_b);
 function_194bc0(stream, &event->direction);
 simulation_write_position(stream, (real const *)&event->position, 12, true);
 function_194bc0(stream, &event->normal);
 function_1955d0(stream, &event->field_30, 16);
}

// @retail 0xa1ba0
void c_projectile_object_impact_effect_event::v9(long a, void const *data, s_bitstream *stream)
{
 s_z_impact_payload const *event = (s_z_impact_payload const *)data;
 scenario_object_name_encode(event->object_name, stream);
 z_write_impact_scale(stream, event->scale_a);
 z_write_impact_scale(stream, event->scale_b);
 function_194bc0(stream, &event->direction);
 simulation_write_position(stream, (real const *)&event->position, 12, true);
 function_194bc0(stream, &event->normal);
 function_1955d0(stream, &event->field_30, 16);
 stream_write_checked(stream, event->field_38, 8);
 stream_write_bit(stream, event->field_34);
}

// @retail 0xa1960
bool c_projectile_impact_effect_event::v10(long a, void *data, s_bitstream *stream)
{
 s_z_impact_payload *event = (s_z_impact_payload *)data;
 event->object_name = z_impact_read_name(stream);
 event->scale_a = z_read_impact_scale(stream);
 event->scale_b = z_read_impact_scale(stream);
 long packed_direction_1 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_1, &event->direction);
 simulation_read_position(stream, (real *)&event->position, 12);
 long packed_direction_2 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_2, &event->normal);
 function_195820(stream, &event->field_30, 16);
 return true;
}

// @retail 0xa1cc0
bool c_projectile_object_impact_effect_event::v10(long a, void *data, s_bitstream *stream)
{
 s_z_impact_payload *event = (s_z_impact_payload *)data;
 event->object_name = z_impact_read_name(stream);
 event->scale_a = z_read_impact_scale(stream);
 event->scale_b = z_read_impact_scale(stream);
 long packed_direction_3 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_3, &event->direction);
 simulation_read_position(stream, (real *)&event->position, 12);
 long packed_direction_4 = function_1959c0(stream, 17);
 function_24f6b0(packed_direction_4, &event->normal);
 function_195820(stream, &event->field_30, 16);
 event->field_38 = function_1959c0(stream, 8);
 event->field_34 = function_1957d0(stream);
 return true;
}

void __stdcall function_107520(long object_index);
void function_107370(long device_index, real value);
bool __stdcall function_1071e0(long group_index, real value);

// @retail 0xa3fd0
void function_a3fd0(long index, bool immediate, bool set_position, real position, bool set_power, real power)
{
 byte *object = (byte *)OBJECT(index);
 if (set_position && !(position < 0.0f || position > 1.0f))
 {
  *(real *)(object + 0x140) = position;
  *(real *)(object + 0x144) = 0.0f;
  function_107520(index);
  long group = *(long *)(object + 0x13c);
  if (group != NONE)
   *(real *)(g_4e0328.groups->data + (group & 0xffff) * 12 + 4) = position;
 }
 if (set_power && !(power < 0.0f || power > 1.0f))
 {
  long group = *(long *)(object + 0x13c);
  if (group != NONE)
  {
   if (immediate)
    function_107370(index, power);
   else
    function_1071e0(group, power);
  }
 }
}

__declspec(noinline) void *function_122c10(long group_tag, long tag_index);
void function_fd0e0(long definition_index, real scale_a, real scale_b, vector3f const *direction,
 point3f const *point, vector3f const *normal, word index, bool attached, long object_index,
 long node_index, bool alternate);

// score 28 (was 28): the effect call's arguments are pushed in the other order
// and the field is loaded into the low half of a cleared register instead of
// being zero extended
// payload reference 28; const payload pointer 28; const data parameter 28;
// validation name snapshot 28; shared name snapshot 42; late name snapshot 28;
// explicit lookup null test 28; separate negated scale tests 28; paired tests 28.
// @retail 0xa1a80
bool c_projectile_impact_effect_event::v11(long a, long const *entities, long c, void const *data)
{
 s_z_impact_payload const *event = (s_z_impact_payload const *)data;
 if (event->object_name != NONE && function_122c10(0x70726f6a, event->object_name) &&
  !(event->scale_a < 0.0f || event->scale_a > 1.0f || event->scale_b < 0.0f || event->scale_b > 1.0f))
 {
  function_fd0e0(event->object_name, event->scale_a, event->scale_b, &event->direction,
   (point3f const *)&event->position, &event->normal, event->field_30, false, NONE, NONE, false);
 }
 return true;
}


bool function_100f00(long weapon_index);

/* Copies the weapon's flag bits, counters and timer into the update record
   for each requested field that changed, after the item fields. */
// @retail 0xa1f70
long c_weapon_type::v27(long a, long b, long c, long d)
{
 long object_offset = (a & 0xffff) * 12;
 byte *object = *(byte **)(object_offset + g_4e0300->data + 8);
 long result = ((c_item_type *)this)->c_item_type::v27(a, b & 0x7ff, 0x94, d);
 dword requested = b;
 s_object_view *updated_object = OBJECT(a);
 byte *definition = g_4e3b44[updated_object->definition_index & 0xffff].bytes;
 if (*(short *)(definition + 0x290))
 {
  if (requested & 0x800)
  {
   dword source_flags = *(word *)(object + 0x16c);
   byte flags = (byte)((source_flags >> 8) & 1);
   if (source_flags & 0x200) flags |= 2; else flags &= ~2;
   if (source_flags & 0x400) flags |= 4; else flags &= ~4;
   if (source_flags & 0x800) flags |= 8; else flags &= ~8;
   if (*(byte *)(d + 0x94) != flags)
   {
    *(byte *)(d + 0x94) = flags;
    result |= 0x800;
   }
  }
  if ((requested & 0x1000) && *(short *)(d + 0x96) != *(short *)(object + 0x17e))
  {
   *(short *)(d + 0x96) = *(short *)(object + 0x17e);
   result |= 0x1000;
  }
 }
 if (requested & 0x2000)
 {
  definition = g_4e3b44[*(long *)object & 0xffff].bytes;
  bool changed = false;
  bool force = !function_100f00(a);
  if (*(real *)(d + 0x9c) != *(real *)(object + 0x184) || force)
  {
   *(real *)(d + 0x9c) = *(real *)(object + 0x184);
   result |= 0x2000;
   changed = true;
  }
  if (*(long *)(definition + 0x2c0) > 0 &&
   (*(short *)(d + 0x98) != *(short *)(object + 0x22c) ||
    *(short *)(d + 0x9a) != *(short *)(object + 0x22a) || changed || force))
  {
   *(real *)(d + 0x9c) = *(real *)(object + 0x184);
   *(short *)(d + 0x98) = *(short *)(object + 0x22c);
   *(short *)(d + 0x9a) = *(short *)(object + 0x22a);
   result |= 0x2000;
  }
 }
 return result;
}


// Retail calls this decoder out of line and still passes its two unused inputs.
__declspec(noinline) bool function_a0640(c_object_type_definition *definition, long a, long *flags, long state, s_bitstream *stream);

// @retail 0xa0640
bool function_a0640(c_object_type_definition *definition, long a, long *flags, long state, s_bitstream *stream)
{
 // Keep the two unused parameters in their retail stack slots.
 c_object_type_definition *const *definition_reference = &definition;
 long const *a_reference = &a;
 (void)*definition_reference;
 (void)*a_reference;
 bool result = function_a6d50((long)flags, state, stream);
 if (function_1957d0(stream))
 {
  *(bool *)(state + 0x90) = function_1957d0(stream);
  *flags |= 0x400;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}

// @retail 0xa0520
bool c_item_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 return function_a0640(this, a, (long *)b, d, stream);
}

// @retail 0xa1350
bool c_projectile_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a6d50(b, d, stream);
 if (function_1957d0(stream))
 {
  *(bool *)(d + 0x90) = function_1957d0(stream);
  *(long *)b |= 0x400;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}

PRIVATE inline real z_read_object_fraction(s_bitstream *stream, long bits, long maximum)
{
 long value = function_1959c0(stream, bits);
 real result;
 if (!value) result = 0.0f;
 else if (value >= maximum) result = 1.0f;
 else result = ((maximum - value) * 0.0f + value * 1.0f) * (1.0f / maximum);
 return result;
}

// @retail 0xa4320
bool c_device_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a6d50(b, d, stream);
 if (function_1957d0(stream))
 {
  *(real *)(d + 0x90) = z_read_object_fraction(stream, 14, 16383);
  *(long *)b |= 0x400;
 }
 if (function_1957d0(stream))
 {
  *(real *)(d + 0x94) = z_read_object_fraction(stream, 14, 16383);
  *(long *)b |= 0x800;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}

// @retail 0xa26d0
bool c_weapon_type::v15(long a, long b, long c, long d, s_bitstream *stream)
{
 bool result = function_a0640(this, a, (long *)b, d, stream) != false;
 if (function_1957d0(stream))
 {
  *(byte *)(d + 0x94) = (byte)function_1959c0(stream, 4);
  *(long *)b |= 0x800;
 }
 if (function_1957d0(stream))
 {
  *(short *)(d + 0x96) = (short)(function_1959c0(stream, 5) - 1);
  *(long *)b |= 0x1000;
  if (*(short *)(d + 0x96) != NONE)
   result = result && *(short *)(d + 0x96) >= 0 && *(short *)(d + 0x96) < 9;
 }
 if (function_1957d0(stream))
 {
  *(word *)(d + 0x98) = (word)function_1959c0(stream, 8);
  *(word *)(d + 0x9a) = (word)function_1959c0(stream, 11);
  *(real *)(d + 0x9c) = z_read_object_fraction(stream, 7, 126);
  *(long *)b |= 0x2000;
 }
 if (result && stream->bit_position <= (stream->size_in_bytes << 3)) return true;
 return false;
}


#include "flags_writer.h"
void function_194830(s_bitstream *stream, bool value);

// @retail 0xa0540
bool function_a0540(c_object_type_definition *definition, long a, long requested, long *written, long state, s_bitstream *stream, long reserve)
{
 bool result = false;
 long const *reserve_reference = &reserve;
 long total_reserve = reserve + 1;
 if (function_a69a0(a, requested & 0x3ff, (long)written, state, (long)stream, definition->v33(), total_reserve))
 {
  s_flags_writer writer;
  flags_writer_initialize(&writer, stream, 10, 1, requested & 0x400, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 10, "at-rest-exists"))
    function_194830(stream, *(bool const *)(state + 0x90));
   flags_writer_end(&writer);
   *written |= writer.written;
   result = true;
  }
 }
 return result;
}

// @retail 0xa1250
bool c_projectile_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 bool result = false;
 long const *reserve_reference = &a8;
 long reserve = a8 + 1;
 if (function_a69a0(a1, a2 & 0x3ff, a3, a5, a7, v33(), reserve))
 {
  s_flags_writer writer;
  s_bitstream *stream = (s_bitstream *)a7;
  flags_writer_initialize(&writer, stream, 10, 1, a2 & 0x400, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 10, "at-rest-exists"))
    function_194830(stream, *(bool const *)(a5 + 0x90));
   flags_writer_end(&writer);
   *(long *)a3 |= writer.written;
   result = true;
  }
 }
 return result;
}

PRIVATE inline void z_write_device_fraction(s_bitstream *stream, real value)
{
 real scaled = value * 16383.0f;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, 14);
}

// @retail 0xa41b0
bool c_device_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 bool result = false;
 long const *reserve_reference = &a8;
 long reserve = a8 + 2;
 if (function_a69a0(a1, a2 & 0x3ff, a3, a5, a7, v33(), reserve))
 {
  s_flags_writer writer;
  s_bitstream *stream = (s_bitstream *)a7;
  flags_writer_initialize(&writer, stream, 10, 2, a2 & 0xc00, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 10, "position-exists"))
    z_write_device_fraction(stream, *(real const *)(a5 + 0x90));
   flags_writer_end(&writer);
   if (flags_writer_begin(&writer, 11, "position-group-position-exists"))
    z_write_device_fraction(stream, *(real const *)(a5 + 0x94));
   flags_writer_end(&writer);
   *(long *)a3 |= writer.written;
   result = true;
  }
 }
 return result;
}

// @retail 0xa4170
bool c_device_type::v13(long a, s_entity_info *info, s_bitstream *stream)
{
 c_object_type_definition const *volatile definition = this;
 bool valid = function_a6810(info, stream);
 if (stream->bit_position <= (stream->size_in_bytes << 3) && valid)
  return true;
 return false;
}

// @retail 0xa23a0
bool c_weapon_type::v13(long a, s_entity_info *info, s_bitstream *stream)
{
 c_object_type_definition const *volatile definition = this;
 bool valid = function_a6810(info, stream);
 valid = stream->bit_position <= (stream->size_in_bytes << 3) && valid;
 if (stream->bit_position <= (stream->size_in_bytes << 3) && valid)
  return true;
 return false;
}

// @retail 0xa04f0
bool c_item_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 return function_a0540(this, a1, a2, (long *)a3, a5, (s_bitstream *)a7, a8);
}

PRIVATE inline void z_write_weapon_fraction(s_bitstream *stream, real value)
{
 function_195720(stream, real_to_long(value * 126.0f), 7);
}

// @retail 0xa2470
bool c_weapon_type::v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8)
{
 bool result = false;
 long const *reserve_reference = &a8;
 long reserve = a8 + 3;
 s_bitstream *stream = (s_bitstream *)a7;
 if (function_a0540(this, a1, a2 & 0x7ff, (long *)a3, a5, stream, reserve))
 {
  s_flags_writer writer;
  flags_writer_initialize(&writer, stream, 11, 3, a2 & 0x3800, *reserve_reference);
  if (writer.space)
  {
   if (flags_writer_begin(&writer, 11, "multiplayer-state-exists"))
    stream_write_checked(stream, *(byte const *)(a5 + 0x94), 4);
   flags_writer_end(&writer);
   if (flags_writer_begin(&writer, 12, "multiplayer-team-exists"))
    stream_write_checked(stream, *(short const *)(a5 + 0x96) + 1, 5);
   flags_writer_end(&writer);
   if (flags_writer_begin(&writer, 13, "weapon-ammo-exists"))
   {
    stream_write_checked(stream, *(short const *)(a5 + 0x98), 8);
    stream_write_checked(stream, *(short const *)(a5 + 0x9a), 11);
    z_write_weapon_fraction(stream, *(real const *)(a5 + 0x9c));
   }
   flags_writer_end(&writer);
   *(long *)a3 |= writer.written;
   result = true;
  }
 }
 return result;
}


// @retail 0xa03d0
void c_item_type::v31(long a, long b, long c, long d)
{
 if (b & 0x400)
  function_b9b90(a, *(bool const *)(d + 0x90));
 long mask = b & 0x3ff;
 if (mask)
  function_a6430(this, mask, a, d);
}

// @retail 0xa0e10
void c_projectile_type::v31(long a, long b, long c, long d)
{
 long mask = b & 0x3ff;
 if (mask)
  function_a6430(this, mask, a, d);
 if (b & 0x400)
  function_b9b90(a, *(bool const *)(d + 0x90));
}


// @retail 0xa27f0
bool c_weapon_type::v16(long a, long b, long c)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a7180(a, b) &&
  fabs(*(real const *)(b + 0x9c) - *(real const *)(a + 0x9c)) < 0.007936508394777775f;
 *(real *)(b + 0x9c) = 0.0f;
 *(real *)(a + 0x9c) = 0.0f;
 return result;
}

// @retail 0xa3f70
void c_device_type::v31(long a, long b, long c, long d)
{
 long mask = b & 0x3ff;
 if (mask)
  function_a6430(this, mask, a, d);
 if (b & 0xc00)
  function_a3fd0(a, false, (bool)(((dword)b >> 10) & 1), *(real const *)(d + 0x90),
   (bool)(((dword)b >> 11) & 1), *(real const *)(d + 0x94));
}

// @retail 0xa4410
bool c_device_type::v16(long a, long b, long c)
{
 c_object_type_definition *volatile definition = this;
 bool result = function_a7180(a, b) &&
  fabs(*(real const *)(b + 0x90) - *(real const *)(a + 0x90)) < 6.103888154029846e-05f;
 *(real *)(b + 0x90) = 0.0f;
 *(real *)(a + 0x90) = 0.0f;
 result = result &&
  fabs(*(real const *)(b + 0x94) - *(real const *)(a + 0x94)) < 6.103888154029846e-05f;
 *(real *)(b + 0x94) = 0.0f;
 *(real *)(a + 0x94) = 0.0f;
 return result;
}


void function_15e130(long object_index);
void function_15e050(long object_index, short value);

struct s_z_weapon_update_object
{
 long definition_index;
 byte unknown004[0x16c - 4];
 word flag0 : 1;
 word flag1 : 1;
 word flag2 : 1;
 word flag3 : 1;
 word flag4 : 1;
 word flag5 : 1;
 word flag6 : 1;
 word flag7 : 1;
 word flag8 : 1;
 word flag9 : 1;
 word flag10 : 1;
 word flag11 : 1;
 word flag12 : 1;
 word flag13 : 1;
 word flag14 : 1;
 word flag15 : 1;
 byte unknown16e[0x17e - 0x16e];
 short field17e;
 byte unknown180[4];
 long field184;
 byte unknown188[0x22a - 0x188];
 short field22a;
 short field22c;
};

// @retail 0xa2140
void c_weapon_type::v31(long a, long b, long c, long d)
{
 s_z_weapon_update_object *object = (s_z_weapon_update_object *)OBJECT(a);
 word mask = b & 0x7ff;
 if (mask)
 {
  if (mask & 0x400)
   function_b9b90(a, *(bool const *)(d + 0x90));
  long low_mask = mask & 0x3ff;
  if (low_mask)
   function_a6430(this, low_mask, a, d);
 }
 long state_mask = b & 0x800;
 if (state_mask || (b & 0x1000))
 {
  byte *tag = *(byte **)((byte *)g_4e3b44 + (OBJECT(a)->definition_index & 0xffff) * 16 + 8);
  if (*(short const *)(tag + 0x290))
  {
   if (state_mask)
   {
    if (*(byte const *)(d + 0x94) & 1) object->flag8 = true;
    else object->flag8 = false;
    if (*(byte const *)(d + 0x94) & 2) object->flag9 = true;
    else object->flag9 = false;
    if (*(byte const *)(d + 0x94) & 4) object->flag10 = true;
    else object->flag10 = false;
    if (*(byte const *)(d + 0x94) & 8) object->flag11 = true;
    else object->flag11 = false;
   }
   if ((b & 0x1000) && object->field17e != *(short const *)(d + 0x96))
   {
    if (TEST_FIELD_BIT(object->flag6))
     function_15e130(a);
    word value = *(word const *)(d + 0x96);
    if (value != 0xffff)
     function_15e050(a, value);
   }
  }
 }
 if (b & 0x2000)
 {
  byte *tag = *(byte **)((byte *)g_4e3b44 + (object->definition_index & 0xffff) * 16 + 8);
  object->field184 = *(long const *)(d + 0x9c);
  if (*(long const *)(tag + 0x2c0) > 0)
  {
   byte *entry = *(byte **)(tag + 0x2c4);
   short first = *(short const *)(d + 0x98);
   short second = *(short const *)(d + 0x9a);
   if (first <= *(short const *)(entry + 0xa) && second <= *(short const *)(entry + 0xc))
   {
    object->field22c = first;
    object->field22a = *(short const *)(d + 0x9a);
   }
  }
 }
}

// @retail 0xa1120
bool c_projectile_type::v13(long a, s_entity_info *info, s_bitstream *stream)
{
    c_object_type_definition const *volatile definition = this;
    bool valid = (unsigned char)function_a6810(info, stream) != 0;
    long *fields = (long *)info->vehicle_data;
    if (function_1957d0(stream))
    {
        fields[0] = function_1959c0(stream, 4);
        valid = valid && fields[0] >= 0 && fields[0] < 16;
    }
    else
        fields[0] = NONE;
    if (function_1957d0(stream))
    {
        long index = function_1959c0(stream, 10);
        byte salt = (byte)function_1959c0(stream, 4);
        fields[1] = index | ((dword)salt << 28);
        if (function_1957d0(stream))
            fields[2] = function_1959c0(stream, 5);
        else
            fields[2] = NONE;
    }
    else
    {
        fields[1] = NONE;
        fields[2] = NONE;
    }
    info->vehicle_data[12] = function_1957d0(stream);
    info->vehicle_data[13] = function_1957d0(stream);
    if (valid && stream->bit_position <= (stream->size_in_bytes << 3))
        return true;
    return false;
}


struct s_z_initial_state;
void __stdcall function_a6660(s_entity_info const *, s_bitstream *);
bool function_a5bd0(s_entity_info const *, s_z_initial_state *);

// @retail 0xa3db0
bool c_device_type::v19(long a, long info, long c, s_entity_data *data)
{
    c_device_type *volatile self = this;
    memset(data, 0, 0x98);
    bool result = false;
    if (function_a5bd0((s_entity_info *)info, (s_z_initial_state *)data)) result = true;
    return result;
}

// @retail 0xa02a0
bool c_item_type::v19(long a, long info, long c, s_entity_data *data)
{
    c_item_type *volatile self = this;
    memset(data, 0, 0x94);
    bool result = false;
    if (function_a5bd0((s_entity_info *)info, (s_z_initial_state *)data))
    {
        ((byte *)data)[0x90] = 0;
        result = true;
    }
    return result;
}

// @retail 0xa1f10
bool c_weapon_type::v19(long a, long info, long c, s_entity_data *data)
{
    c_weapon_type *volatile self = this;
    memset(data, 0, 0xa0);
    memset(data, 0, 0x94);
    bool result = false;
    if (function_a5bd0((s_entity_info *)info, (s_z_initial_state *)data))
    {
        ((byte *)data)[0x90] = 0;
        ((byte *)data)[0x94] = 0;
        *(short *)((byte *)data + 0x96) = NONE;
        result = true;
    }
    return result;
}

// @retail 0xa2380
void c_item_type::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
    c_item_type *volatile self = this;
    function_a6660(info, stream);
}



// @retail 0xa0f50
void c_projectile_type::v12(long a, s_entity_info *info, long c, s_bitstream *stream)
{
    c_projectile_type *volatile self = this;
    function_a6660(info, stream);
    byte const *data = (byte const *)info;
    stream_write_bit(stream, *(long const *)(data + 0x10) != NONE);
    long first = *(long const *)(data + 0x10);
    if (first != NONE) stream_write_checked(stream, first, 4);
    stream_write_bit(stream, *(long const *)(data + 0x14) != NONE);
    if (*(long const *)(data + 0x14) != NONE)
    {
        simulation_write_entity_identifier(*(long const *)(data + 0x14), stream);
        stream_write_bit(stream, *(long const *)(data + 0x18) != NONE);
        long third = *(long const *)(data + 0x18);
        if (third != NONE) stream_write_checked(stream, third, 5);
    }
    stream_write_bit(stream, data[0x1c] != 0);
    stream_write_bit(stream, data[0x1d] != 0);
}



// @retail 0xa0350
long c_item_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    c_item_type *volatile self = this;
    byte creation[0xc4];
    function_a5d90(creation, info, flags, state);
    long index;
    if (info->field0 != NONE) index = function_a73b0(info);
    else index = function_b7b40(creation);
    if (index != NONE) ((byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object)[0xaf] = info->byte8;
    return index;
}

// @retail 0xa3eb0
long c_device_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    c_device_type *volatile self = this;
    byte creation[0xc4];
    function_a5d90(creation, info, flags, state);
    long index;
    if (info->field0 != NONE) index = function_a73b0(info);
    else index = function_b7b40(creation);
    if (index != NONE)
    {
        ((byte *)((s_object_header *)g_4e0300->data)[index & 0xffff].object)[0xaf] = info->byte8;
        if (*flags & 0xc00)
        {
            function_a3fd0(index, true, (*flags & 0x400) != 0, *(real *)(state + 0x90),
                (*flags & 0x800) != 0, *(real *)(state + 0x94));
            *flags &= ~0xc00;
        }
    }
    return index;
}

struct s_projectile_target
{
    long object_index;
    short node_index;
    byte unknown06[2];
};
void projectile_set_target(long index, s_projectile_target const *target);

void function_0bfe20(dword *flags, long bit, bool value);

// @retail 0xa0c50
long c_projectile_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    c_projectile_type *volatile self = this;
    byte creation[0xc4];
    function_a5d90(creation, info, flags, state);
    byte *fields = (byte *)info;
    if (*(long *)(fields + 0x10) != NONE)
    {
        byte *record = datum_get_absolute_inlined(g_4e8c24, *(long *)(fields + 0x10));
        if (record)
        {
            long player_index = data_datum_index(g_4e8c24, *(long *)(fields + 0x10));
            *(long *)(creation + 0x60) = *(long *)(record + 0x2c);
            *(long *)(creation + 0x5c) = player_index;
            *(long *)(creation + 0x64) = (signed char)record[0xc0];
            *(long *)(creation + 0x6c) = *(long *)(record + 0x2c);
            *(long *)(creation + 0x68) = player_index;
            *(short *)(creation + 0x70) = (signed char)record[0xc0];
        }
    }
    long index;
    if (info->field0 != NONE) index = function_a73b0(info);
    else index = function_b7b40(creation);
    if (index != NONE)
    {
        long offset = (index & 0xffff) * 12;
        byte *object = *(byte **)(g_4e0300->data + offset + 8);
        object[0xaf] = info->byte8;
        if (fields[0x1d])
        {
            object = *(byte **)(g_4e0300->data + offset + 8);
            function_0bfe20((dword *)(object + 0x12c), 8, true);
        }
        long target = *(long *)(fields + 0x14);
        if (target != NONE)
        {
            s_simulation_world_view *world = (s_simulation_world_view *)g_4cf77c;
            s_simulation_entity *entity = simulation_entity_try_get(&world->database->table, target);
            if (entity && entity->object_index != NONE)
            {
                s_projectile_target request;
                request.object_index = entity->object_index;
                request.node_index = *(short *)(fields + 0x18);
                projectile_set_target(index, &request);
            }
        }
        if (!fields[0x1c])
        {
            object = *(byte **)(g_4e0300->data + offset + 8);
            function_0bfe20((dword *)(object + 0x12c), 1, false);
        }
    }
    return index;
}

// @retail 0xa2120
long c_weapon_type::v29(long a, s_entity_info *info, long *flags, long size, long state)
{
    return ((c_item_type *)this)->c_item_type::v29(0x10, info, flags, 0x94, state);
}
