#include "unknown_11c920.h"
#include "unknown_1d5120.h"
#include <float.h>
#include <new>
// @flags /O2 /arch:SSE /Gr

extern hkWorld *g_51e9a4;
void havok_component_rigid_body_transform_set(long arg_0, s_havok_component *arg_1, hkTransform const *arg_2);


long __stdcall function_1d5940(s_havok_component *component, bool first, bool second, bool third);
__declspec(noinline) void function_1d4360(s_havok_component *component, transform4x3f *volatile result);
struct s_havok_transform;
void function_181f80(s_havok_transform *result, transform4x3f const *matrix);
bool function_f1320(long object_index);
bool function_205510(s_havok_component *component, void const *definition_physics, void *buffer);

class c_1d56f0_shape
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void slot4() = 0;
    virtual void slot5() = 0;
    virtual void slot6() = 0;
    virtual void slot7() = 0;
    virtual void slot8() = 0;
    virtual void slot9() = 0;
    virtual long slot10() = 0;
};

__declspec(noinline) void function_1d56f0(s_havok_component *component);

// @retail 0x1d56f0
void function_1d56f0(s_havok_component *component)
{
    byte *object = *(byte **)((byte *)g_4e0300->data + (component->object_index & 0xffff) * 12 + 8);
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    bool enabled = (bool)((object[0x348] >> 2) & 1);
    component->unknown1c = (byte)function_1d5940(component, enabled, false, true);
    char index = component->unknown18;
    if (index >= 0 && index < component->rigid_bodies.size)
    {
        short const &converted_index = (short)index;
        if (converted_index != NONE)
        {
            byte *physics = definition + 0x2ac;
            bool skip = false;
            if (*(long *)(definition + 0x2f8))
            {
                byte *shape = *(byte **)(physics + 0x50);
                if (*(long *)(shape + 0x38) > 0)
                {
                    if (((c_1d56f0_shape *)shape)->slot10() <= 1)
                        shape += 0x60;
                    if (shape)
                    {
                        c_contact_query_transform_info info;
                        transform4x3f matrix;
                        function_1d4360(component, &matrix);
                        info.shape = shape;
                        info.filter = ((component->object_index + 1) << 16) | 0xc;
                        function_181f80((s_havok_transform *)&info.transform, &matrix);
                        component->rigid_body = (hkRigidBody *)::new (contact_query_allocate(sizeof(c_contact_query_transform_volume))) c_contact_query_transform_volume(&info);
                    }
                }
                if (*(long *)(physics + 0x4c))
                {
                    skip = (bool)((*(dword *)(*(byte **)(physics + 0x50) + 0x3c) >> 1) & 1);
                }
            }
            if (skip || !function_f1320(component->object_index))
            {
                real bounds[6];
                if (function_205510(component, physics, bounds))
                {
                    c_contact_query_bounds_info info;
                    info.lower.set(bounds[0], bounds[2], bounds[4]);
                    info.upper.set(bounds[1], bounds[3], bounds[5]);
                    info.filter = ((component->object_index + 1) << 16) | 0xc;
                    component->unknown9c = (long)new c_contact_query_bounds_volume(&info);
                }
            }
        }
    }
}

// MiniMax G (11-13): no new form, no lower score (77): retail's tail ends with the same inline
// `_mm_setcsr(_mm_getcsr() & 0xffffffc0)` before `__clearfp` (the mxcsr scratch at [esp+0x10] and the two
// control87 calls), so this source is right and the only difference there is the register of the mxcsr
// temp (EAX here, ECX in retail); dropping that line loses the scratch slot and moves every other home
// (85, 803 bytes against retail's 819).
// MiniMax G (11-13): no new form, no lower score (77): the rigid-body lookup still reads `[eax + edx + 0x40]`
// where retail reads `[edx + eax + 0x40]`, and the success tail still pushes what retail passes in EBX, ECX
// and EAX: BLOCKED-BY 0x1d0cf0, whose extra push ebx (needed only because the component arrives on the
// stack) is what makes its frame 0x84 against retail's 0x88 and shifts every esp offset.
// score 77 (was 77): the lookup still reads `[eax + edx + 0x40]` where retail reads `[edx + eax + 0x40]`
// and the success tail still pushes what retail passes in EAX/EBX/ECX: BLOCKED-BY 0x1d0cf0 (another file).
// Earlier (score 77, was 77): every frame home and the body up to the success tail agree; left: the rigid-body
// lookup's address operands (ours `[base+index]`, retail `[index+base]`) and the success tail, BLOCKED-BY
// 0x1d0cf0 (retail passes it the transform in EAX, the component in EBX and the index in ECX, plain ret;
// ours pushes index and component), which also moves the release path's scratch registers.
// Tried: rotation value 190; pose value 232; rotation simd 150; info first copy 87; source pose value 216;
// completed count 149; filter value 119; initial height value 79; inc/dec update alone 90; with the
// 12-byte callback 80; callback after the direction 80; callback declared at the world add 86; the 12-byte
// callback with the subtraction update 119; two-use body/motion sources 223/215; output columns
// 021/102/201/210 78/80/80/79; translation publication/input before rotation 84/83; body pointer local 77;
// filter first 102; shape before the transform copy 77; direct rigid_bodies indexing 77; transform by
// assignment 77; count publication after rotation columns 0/1/2, translation construction and pose copy 77
// each; query-derived success 110; derived success with late count 110; collision-first branch 77;
// shape/filter/both/reversed fields before lookup 110/109/107/105; shape/filter/both before snapshot
// 77/102/84; pose before input construction 217; a component-first getter 77; the tail through the
// existing matrix setter (0x1d0c20, called out of line here) 113; `(data + index)->rigid_body` 77; the
// body pointer declared at the top 77; a named index local 77; corrected-frame transform/translation/both
// at loop entry or function entry 77 each; native translation/all-column SIMD construction 77/138; direct
// `.data[index]` lookup 77; `index[data]` subscript 77; index copied to a local at entry 77; entry
// reference through the array's operator[] 77; forceinline getter declaration 77; the count parameter
// declared first 77 (a harness keeps base first for every entry statement order and lookup spelling).
// A member function of s_havok_component (`this` = the component, as matched 0x1d08e0)
// gives retail's index-first lookup: 76, with only the 0x1d0cf0 tail left (not kept: one point).
// body pointer declared at the top 77; a named index local 77.
// lookup address operands and the success-tail setter transport remain.
// Earlier frame 0x104 and MXCSR homes agreed; result/direction/callback homes and setter convention
// differ; tried rotation value 190; pose value 232; rotation simd 150; info first copy 87; source pose value
// 216; completed count 149; filter value 119; initial height value 79.
// the success byte's frame home sits eight bytes above
// retail's and the callback slot four bytes high, and the loop's branch
// update still takes the signed-subtraction form.
// with the presence callback's real 12-byte size, its false-initialising
// constructor, the inc/dec update and the callback declared first, every frame home and the whole
// body up to the success tail agree; left: the rigid-body lookup's address operand order and the
// success tail, BLOCKED-BY 0x1d0cf0 (retail passes it the component in EBX, ours pushes it).
// inc/dec update alone 90; with the 12-byte callback 80; callback after the direction 80; callback
// declared at the world add 86; the 12-byte callback with the subtraction update 119.
// rigid-body address operand order and success-tail setter convention remain.
// Two-use body/motion sources 223/215; output columns 021/102/201/210 78/80/80/79;
// translation publication/input before rotation 84/83. Frame and callback homes stay correct.
// first difference is still the lookup's `[base+index]` against retail's `[index+base]`; the tail waits on BLOCKED-BY 0x1d0cf0 (retail passes transform in EAX, component in EBX, index in ECX, plain ret). Body pointer local 77; filter first 102; shape before the transform copy 77; direct rigid_bodies indexing 77; transform by assignment 77; a harness keeps base first for every entry statement order and lookup spelling.
// lookup address operands and setter input convention remain; count publication after
// rotation columns 0/1/2, translation construction and pose copy 77 each; query-derived success 110;
// derived success with late count 110; collision-first branch 77. Correct frame homes preserved on restore.
// lookup address operands and setter component convention remain;
// shape/filter/both/reversed fields before lookup 110/109/107/105; shape/filter/both
// before snapshot 77/102/84; pose before input construction 217.
// Round 402: corrected-frame transform/translation/both at loop entry 77/77/77;
// at function entry 77/77/77; native translation/all-column SIMD construction 77/138.
// All restored; lookup operand order and success-tail setter transport remain.
// Round 7 (lane 13): the tail's stores match retail apart from the setter's pushed arguments (BLOCKED-BY
// 0x1d0cf0, whose file is held this round); matched 0x1d08e0 uses the same getter expression and gets
// retail's index-first `[eax+edx+0x40]`. Direct `.data[index]` 77; array pointer read into a local first
// 77; index-first subscript 77; the component parameter renamed to sort last 77; the source transform
// through a const reference 77; the setter call's index replaced by a constant (diagnostic) 87.
// Round 5: direct `.data[index]` lookup 77; `index[data]` subscript 77; index copied to a local at
// entry for the lookup and the setter 77; entry reference through the array's operator[] 77;
// forceinline getter declaration 77 (the SIB order ignores source operand order).
// ours pushes index and component), which also moves the release path's scratch registers. Retail
// inlines the matrix setter 0x1d0c20 here (this tail is its body).
// Round 37: as a member function of s_havok_component (`this` keeps its [ebp+8] home, the caller
// 0xe0c70 unchanged) the lookup becomes retail's index-first `[edx+eax+0x40]`, as in the matched
// member 0x1d08e0, and only the tail differs: 76 (not kept, a gain of one; apply it with the 0x1d0cf0
// fix). On that form: direct `rigid_bodies[index]` 76; result transform declared before the loop 76;
// count published after the setter call 63 (positional only: retail stores it before the call);
// translation built straight into the result 63 at 811 bytes (frame moves).
// Tried: rotation value 190; pose value 232; rotation simd 150; info first copy 87; source pose value
// 216; completed count 149; filter value 119; initial height value 79; inc/dec update alone 90; with
// the 12-byte callback 80; callback after the direction 80; callback declared at the world add 86; the
// 12-byte callback with the subtraction update 119; two-use body/motion sources 223/215; output
// columns 021/102/201/210 78/80/80/79; translation publication/input before rotation 84/83; body
// pointer local 77; filter first 102; shape before the transform copy 77; direct rigid_bodies indexing
// 77; transform by assignment 77; count publication after rotation columns 0/1/2, translation
// construction and pose copy 77 each; query-derived success 110; derived success with late count 110;
// collision-first branch 77; shape/filter/both/reversed fields before lookup 110/109/107/105;
// shape/filter/both before snapshot 77/102/84; pose before input construction 217; a component-first
// getter 77; the tail through the existing matrix setter (0x1d0c20, called out of line here) 113;
// `(data + index)->rigid_body` 77; the body pointer declared at the top 77; a named index local 77;
// corrected-frame transform/translation/both at loop entry 77/77/77 and at function entry 77/77/77;
// native translation/all-column SIMD construction 77/138; direct `.data[index]` 77; array pointer read
// into a local first 77; index-first subscript and `index[data]` 77; the component parameter renamed to
// sort last 77; the source transform through a const reference 77; the setter call's index replaced by a
// constant (diagnostic) 87; index copied to a local at entry 77; entry reference through the array's
// operator[] 77; forceinline getter declaration 77.
// Round 97 (lane 7): BLOCKED-BY 0x1d0cf0 again (its file is held this round); 19 aligned differences, all in
// that tail but the lookup's SIB order. A byte offset local added first, the component through a const
// reference, a pointer to the element's body field, data-first byte arithmetic and a named motion pointer:
// 77 each (still `[eax + edx + 0x40]`).
// A later round: no new source form of this body was found: the 19 aligned differences stay in
// the tail after the component lookup, whose calling convention is set by 0x1d0cf0 in another file, so the tail
// cannot be aligned from here, and the lookup still reads `[eax + edx + 0x40]` where retail's order differs.
// The body was left as it is.
// Round 130 (lane 6, fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x1d0ee0; the two stacked notes merged into this one,
// nothing dropped): BLOCKED-BY 0x1d0cf0, which sits on a long LTCG call cycle (0x1d1260 -> impacts.cpp ->
// unknown_175bd0.cpp -> damage.cpp -> 0x1d1b60 -> 0x1d0c20 -> 0x1d0cf0, and 0x1d1540 -> 0x1c39c0 -> ...). The
// translation built straight into the result (no `local_8`) 64 (811 bytes, frame 0xf4: positional only, retail
// keeps the temporary and its movaps copy); 0x1d0cf0's definition marked `inline` 77; 0x1d0cf0 as a
// s_havok_component member (`this` = the component, as the rigid_body_* members) 78 (0x1d1b60 lost at 2); the
// count stored after the setter call 64 (positional only: retail stores it before the call); `local_0 = true`
// before the setter call 78.
// Earlier at 77 (from 77): this round no new source form of this body was found: the 19 aligned differences stay in
// Round 69 (m3 lane 9, fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10): as a member of s_havok_component 76;
// that with 0x1d0cf0 a member too (rigid_body_transform_set(index, transform)) 77, losing matched 0x115b10 (4)
// and 0x1d1b60 (2); 0x1d0cf0 in retail's shape (set copy, body pointer, m_fixed first) 73, and with this body
// as a member 72: only the component's push for 0x1d0cf0 (retail EBX) and the frame and scratch shifts it
// causes are left, but 0x1d0cf0 goes 81 -> 88 and 0x1d0c20 38 -> 40, so not kept; that with 0x1d0cf0's
// fixed arm refetching through a const element pointer 72 (0x1d0cf0 85). Lead: retail's call cycle through
// 0x1d0cf0 runs 0x1d1260 -> 0x278f00 -> 0xb8540 -> 0x109400 -> 0xe4c10 -> 0xe18d0 -> 0xb75a0 -> 0xb7430 ->
// 0x1c4a80 -> 0x1d3fc0 -> 0x1d0c20; here 0xb8540 and 0xb7430 are disabled stubs, so LTCG walks another
// cycle (via 0x2266a0 ... 0xdbc80 -> 0x1d1b60), which may be what keeps the component on the stack
// (BLOCKED-BY 0xb8540).
// Earlier: this round no new source form of this body was found: the 19 aligned differences stay in
// Round 292: complete component/body, query filter/shape and step/count/output parameter
// identities in seven subsets and alternate complete roles 77/77/77/77/77/77/77/77.
// Original formal identities restored; lookup operand order and setter transport remain.
// Round 97 summary (score 77, was 77): no new source form of this body was found: the 19 aligned differences stay in
// Round 26 (lane 17): 0x1d0cf0 is now machine 3's, but its EBX component parameter still does not come:
// 0x1d1260/0x1d1540 declared first in its file (call-cycle walk order, plain) 77 (0x1d0cf0 81); that with
// 0x1d0cf0's 88 body (plain) 73 (0x1d0cf0 88, 0x1d0c20 40, component still loaded from [ebp + 8]); the 88
// body with flag5 read into a named bool before the body fetch 73, with a member-wise transform copy 73.
// Round 122 (fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10): on 0x1d0cf0's 88 body 73 (0x1d0cf0 88, 0x1d0c20 40);
// that with `inline` on 0x1d0c20 73; 0x1d0c20 and its frame helper ahead of 0x1d0cf0 in the file on the 88 body 73,
// on the original body 77; the component copied to a local at the 88 body's top 73; m_fixed in a named bool 73;
// the published count in a named local 77. Restored. BLOCKED-BY 0x1d0cf0 (component in EBX).
// Round 48 (fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10): the getter called with a const component 77 (same
// code, still `[eax + edx + 0x40]`); `arg_4 * (real)arg_6` 77 (same code); the result transform declared before
// the loop 77 (same code); the loop start as `local_4 ? arg_6 - 1 : arg_6 + 1` 113 (822 bytes); the step as
// `local_4 ? -arg_4 : arg_4` 93 (819 bytes; retail's step is the constant select times the step, as here).
// Round 140 (m3 lane 5, fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10): the result flag declared after the transform
// and volume 86 (first difference moves to +0x1b); the volume pointer declared before the transform 77; both
// declarations ahead of the floating-point control calls 77; the query info at function scope without its block 195
// (838 B), declared first at function scope 195. Diagnostic: one or two unused locals ahead of the lookup leave its
// `[eax + edx + 0x40]` order unchanged, so that order is not a symbol-numbering tie.
// Round 41 (m3 lane 17, fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10/0x1d56f0): the component copied to a local at
// entry 77 (same code), as a const pointer 77, with the rigid-body index copied too 77; the query info's transform
// set from the body first and the pose copied from it 85, with the component local 85. The tail still waits on
// BLOCKED-BY 0x1d0cf0 (component in EBX; src/unknown_1cec30.cpp avoid-listed again).
// Round 107: array pointer/reference receivers, index-first array receiver and component reference
// receiver 77 each; entry reference/pointer returns through the component, entry reference through
// the array and entry pointer through the array reference 77 each. All the same code; one-caller
// diagnostic helper removed and the original shared getter restored.
// Round 69 (lane 11, fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10; 0x1d0cf0's body varied, restored each time): set copy, body pointer, flag5
// first 15 (819 bytes, retail's length, aligned 5: the setter call and the frame are retail's but for the component in EDI where retail has EBX and
// the lookup's operands), 0x1d0cf0 27, but 0x1d0c20 49, and matched 0x1d1b60 59 and 0x115b10 56 are lost; the refetch assigned to the body
// pointer 15 (same losses); a two-use slot 15 (0x1d0cf0 60, same losses). m_fixed first 73 (820 bytes, nothing lost, 0x1d0cf0 88), with a named
// fixed bool 73, with a component copy 73. Still BLOCKED-BY 0x1d0cf0's EBX. The cycle lead above may need 0xb8540 and 0xb7430, not tried here: their files' notes say enabling them loses matches (23 for 0xb8540 alone when measured).
// T62: setter cdecl/stdcall 80/77; existing allocator placement 77; actual registration
// return 153; persistent body/query receivers 77/77; placement/registration and body/
// registration 153/153. NOT kept: no gain; registration moves the frame, and cdecl
// raises both setter users. Original contracts restored; lookup and setter transport remain.
// Round 153 (m3 lane 5, fast with 0x1d0cf0/0x1d0c20/0x1d1b60; 0x1d0cf0 body varied, restored each time): set copy with a
// body pointer, m_fixed first, refetch by direct indexing 73 (0x1d0cf0 88); set copy with a slot pointer fetched before the
// transformed block 73 (85); set copy, slot pointer, direct refetch 73 (90); constructor copy with a body pointer, getter or
// direct refetch 77/77 (85/85). 0x1d1b60 stays matched in all. A /Gd compile-only first pass puts the component in EBX for
// every m_fixed-first form (retail's final EBX/ESI/EDI roles) and in EDI for flag5-first ones; LTCG converts only the EDI
// case, and an EDI component passes through 0x1d0c20 from 0x1d1b60's EDI (retail 0x1d1b60 holds the component in EDI), which
// is why those forms lose 0x1d1b60. Still BLOCKED-BY 0x1d0cf0 (component in EBX).
// DH384: shared xyz rotation/translation/all 77/77/77; all-row scalar inputs 77;
// accepted-pose snapshot 180; shared point setter 77; output/moving-pose zero
// construction 116/244. NOT kept: no gain; record/zero forms grow the native body.
// Round 394: complementary blocked result bool/signed-byte/short/long
// 82/82/147/146; each with nonpositive direction 88/88/153/152. All grow the
// body; original result, direction, frame homes and complete executable source restored.
// MiniMax S (11-09): no new form, no lower score; the local block still sits eight bytes higher
// than retail's and the block around the call to 0x1d0cf0 still differs in register and stack slot
// use; 0x1d0cf0 is in another lane's file.
// R223 (lane 3, fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10; 0x1d0cf0 varied and restored): retail sets ECX = index, EBX =
// component, EAX = transform, in that order, which implies 0x1d0cf0(transform, component, index). All six parameter orders,
// definition, declaration and both calls together: on its current body 77/77/77 for the orders with the index ahead of the
// component (same code) and 78/78/78 for the others (0x115b10 lost at 4, 0x1d1b60 at 2); on its set-copy, body-pointer,
// fixed-first body 73 in all six (0x1d0cf0 88, 0x1d0c20 40). On that body, with this function's call removed (diagnostic)
// 0x1d0cf0 stays 88, with 0x1d0c20's call removed 86: neither caller keeps the component out of EBX. The transformed flag in
// a named bool ahead of the copy 73; the refetch assigned to the body pointer 73. Still BLOCKED-BY 0x1d0cf0.
// MiniMax R (11-10): no new form, no lower score (77): the component lookup still reads [eax + edx + 0x40]
// where retail reads [edx + eax + 0x40], and the tail's register and slot use is set by the callee in the
// other file.
// M3 lane 13 r82 (fast with 0x1d0cf0/0x1d0c20/0x1d1b60/0x115b10/0x1d0ee0/0x1d56f0/0xe0c70/0x1d3fc0/0xe1c40): this function,
// 0x1d0cf0 and 0x1d0c20 all as members of the component (inline free-function spellings kept for the other callers,
// diagnostic): 77 (only the tail differs), 0x1d0cf0 65 and 0x1d0c20 38 unchanged (component and index still pushed),
// matched 0x1d1b60 (2) and 0x115b10 (4) lost, 0xe0c70 63 -> 66. Restored. BLOCKED-BY 0x1d0cf0 (component in EBX).
// MiniMax D (11-11): no new form, no lower score (77): the component lookup still reads [eax + edx + 0x40] where retail
// reads [edx + eax + 0x40] for the same address, and the tail's registers and stack slots follow the callee, which is in
// another lane's file.
// MiniMax A (11-12): no new form, no lower score (77); the rigid-body lookup's two address operands
// still read in the other order than retail's, and the success tail still pushes what retail passes
// in EAX, EBX and ECX: BLOCKED-BY 0x1d0cf0 (src/unknown_1cec30.cpp) and the call cycle behind it.
// MiniMax H (11-10): no new form, no lower score (77): the rigid-body lookup's two address operands
// still read in the other order than retail's, and the success tail still pushes what retail passes in
// EAX, EBX and ECX: BLOCKED-BY 0x1d0cf0 (src/unknown_1cec30.cpp) and the call cycle behind it.
// MiniMax I (11-09): no new form, no lower score; the lookup still reads [edx + eax + 0x40] where retail reads [eax + edx + 0x40], and the rigid body, the motion transform and the index in named locals all leave the same 77; the success tail still pushes what retail passes in EAX, EBX and ECX: BLOCKED-BY 0x1d0cf0 (src/unknown_1cec30.cpp).
// MiniMax D (11-06): no new form, no lower score; the success tail cannot be matched from this file, because retail's 0x1d0cf0 takes the transform in EAX, the component in EBX and the index in ECX and returns plainly, while our call pushes the component and the index on the stack: BLOCKED-BY 0x1d0cf0 (src/unknown_1cec30.cpp).
// MiniMax I (11-06): no new form, no lower score; the lookup still reads [eax + edx + 0x40] where retail reads [edx + eax + 0x40], and the success tail still pushes what retail passes in EAX, EBX and ECX: BLOCKED-BY 0x1d0cf0 (src/unknown_1cec30.cpp).
// MiniMax G (11-05): no new form, no lower score; the rigid-body lookup's address operands still
// read `[eax + edx + 0x40]` where retail reads `[edx + eax + 0x40]`, and the success tail still
// pushes what retail passes in EAX, EBX and ECX: BLOCKED-BY 0x1d0cf0 (src/unknown_1cec30.cpp).
// MiniMax B (11-04): no new form, no lower score.
// M3 lane 12 r119: no trial. The listing is retail's except the lookup's `[eax + edx + 0x40]` and the 0x1d0cf0 call
// tail (retail: transform in EAX, index in ECX, component loaded into EBX before the call; ours pushes component and
// index). The forms on this side are listed above (the member form gives retail's lookup order, 76); the tail waits
// on 0x1d0cf0's EBX component, which lane 5 r153's /Gd screen and round 3 of #1044 tie to LTCG's walk of the call
// cycle through 0x1d1260/0x1d1540 (disabled 0xb8540/0xb7430 on retail's cycle). Still BLOCKED-BY 0x1d0cf0.
// Retail's 0x1d0c20 (fast listing) takes the component on the stack and loads it into EBX only just before the
// 0x1d0cf0 call, so 0x1d0c20 (38) and this tail wait on the same EBX parameter. The flag5-first 0x1d0cf0 body (15
// here) is not retail's test order and raises 0x1d0c20 and 0x1d3fc0 (avoid-listed this round), so it was not re-run.
// @retail 0x1d5120
bool function_1d5120(s_havok_component *arg_0, long arg_1, long arg_2, long arg_3,
 real arg_4, long *arg_5, long arg_6)
{
 bool local_0 = false;
 _control87(0x9001f, 0x8001f);
 _mm_setcsr(_mm_getcsr() | 0x1f80);
 hkTransform local_1;
 c_contact_query_transform_volume *local_2;
 {
  c_contact_query_transform_info local_3;
  local_1.set(havok_component_rigid_body_get(arg_1, arg_0)->m_motion->m_transform);
  local_3.transform.set(local_1);
  local_3.shape = (void *)arg_3;
  ((real *)&local_1.m_translation.m_quad)[2] += (real)arg_6 * arg_4;
  local_3.filter = ((arg_0->object_index + 1) << 16) | arg_2;
  local_2 = new c_contact_query_transform_volume(&local_3);
 }
 ((c_contact_query_world *)g_51e9a4)->add(local_2);
 c_contact_presence local_6;
 bool local_4 = arg_6 > 0;
 long local_5 = arg_6 - (local_4 ? 1 : -1);
 while (local_4 ? local_5 >= 0 : local_5 <= 0)
 {
  local_6.found = false;
  ((hkRigidBody *)local_2)->pose_write(local_1);
  ((c_contact_query_dispatch *)local_2)->query(&local_6);
  if (!local_6.found)
  {
   *arg_5 = arg_6 - local_5;
   hkTransform local_7;
   local_7.m_rotation.m_col0.set(local_1.m_rotation.m_col0(0), local_1.m_rotation.m_col0(1), local_1.m_rotation.m_col0(2));
   local_7.m_rotation.m_col1.set(local_1.m_rotation.m_col1(0), local_1.m_rotation.m_col1(1), local_1.m_rotation.m_col1(2));
   local_7.m_rotation.m_col2.set(local_1.m_rotation.m_col2(0), local_1.m_rotation.m_col2(1), local_1.m_rotation.m_col2(2));
   hkVector4 local_8;
   local_8.set(local_1.m_translation(0), local_1.m_translation(1), local_1.m_translation(2));
   local_7.m_translation = local_8;
   havok_component_rigid_body_transform_set(arg_1, arg_0, &local_7);
   local_0 = true;
   break;
  }
  ((real *)&local_1.m_translation.m_quad)[2] += (local_4 ? -1.0f : 1.0f) * arg_4;
  if (local_4)
   local_5--;
  else
   local_5++;
 }
 ((c_contact_query_world *)g_51e9a4)->remove(local_2);
 havok_reference_remove((c_havok_reference_counted *)local_2);
 _mm_setcsr(_mm_getcsr() & 0xffffffc0);
 _clearfp();
 _control87(0x9001f, 0xfffff);
 return local_0;
}
