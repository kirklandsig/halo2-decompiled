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

// score 49 (was 49): retail holds a bool result in BL from before the creation call, saves EBP
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
