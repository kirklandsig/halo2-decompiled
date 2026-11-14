// @flags /O2 /GL-
// Havok (third-party physics library): an object that keeps the object it
// was made for, with one reference.
#include "owned_reference_318b90.h"

// @retail 0x319080
class_319080::class_319080(void *owner)
	: owner(owner)
{
}

class c_kind_319230
{
public:
	virtual ~c_kind_319230();
	virtual void slot_1() = 0;
	virtual void slot_2() = 0;
	virtual void slot_3() = 0;
	virtual void slot_4() = 0;
	virtual void slot_5() = 0;
	virtual long slot_6() = 0;
};

struct s_owner_319230
{
	char unknown0[0x3c];
	c_kind_319230 *field_3c;
	unsigned char field_40;
};

struct s_operand_319230
{
	char unknown0[0x20];
	s_owner_319230 const *owner;
};

struct s_result_319230
{
	unsigned char value;
	s_result_319230(unsigned char v) : value(v) {}
};

class c_query_319230
{
public:
	s_result_319230 function_319230(s_operand_319230 const *a, s_operand_319230 const *b) const;
};

// score 16 (was 16): the first receiver is loaded into ECX;
// retail loads EAX, its table into EDX, then copies EAX to ECX.
// Byte output, operand references, const/enum query, destructor slot,
// concrete slots, inherited slots/base, signed output, owner references,
// Boolean construction 22; result initialized false 36; separate result
// assignments 19 rejected; named receivers and named call results 21 rejected.
// Integer-input construction int/long/unsigned, explicit copy, destruction, both,
// existing byte Boolean result and aggregate return initialization 22 each.
// Native int/unsigned-int/unsigned-long kind results, ordinary/virtual callback
// contracts and native signed-byte flag with unsigned kind 22 each.
// Earlier result-construction forms restored; first virtual receiver setup still differs.
// Native Boolean/full-byte fields with cached owners 22; first owner read at each
// use 16; second owner per-use 20 rejected; both per-use with byte bitfield 31.
// First-owner per-use with the original byte field 16. Entry/receiver setup remains.
// Returned byte on the first-owner per-use graph: direct construction 29; false/true
// result 40/34; byte then kind 45; flag return and kind result 44; separate regions
// and negative flag region 16/16; entry result with separate production 29. All restored.
// Existing byte conversion types on per-use/cached/reference owner graphs
// 16/16, 22/22, 22/22; operand-reference inputs 16/16. All restored.
// Stored owner/kind references 16/16; immutable owner/kind links 16/16;
// on the current first-owner reads: const kind call 16, enum result 16,
// virtual query 16, Boolean constructor 16. All restored.
// The retail function is a free __stdcall function taking the result byte
// through a pointer: three stack words (`ret 0xc`), not a thiscall returning
// the struct. The method also passes three words (a hidden result pointer,
// `this`, a, b), so the length fits while the result role is wrong. With the
// explicit pointer the length is exact (79) and the prologue matches; only the
// first virtual receiver differs: retail loads the kind into EAX, its table
// into EDX, then copies EAX to ECX, while VC loads it into ECX directly.
// Caching the kinds (both or first only), binding a reference and an assignment
// expression kept that form or cost bytes, and the explicit-pointer form scores
// 22 against this method's 16, so it is not kept.
// @retail 0x319230
s_result_319230 c_query_319230::function_319230(s_operand_319230 const *a, s_operand_319230 const *b) const
{
	s_owner_319230 const *second = b->owner;
	if (a->owner->field_40 || second->field_40 || a->owner->field_3c->slot_6() == 6 || second->field_3c->slot_6() == 6)
	{
		return s_result_319230(1);
	}
	return s_result_319230(0);
}
