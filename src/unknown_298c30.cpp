#include "unknown_11c920.h"
#include "normalize3d.h"
#include "vector2f_products.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_26c380.h"
#include "unknown_2c0d00.h"
#include <math.h>
// @flags /O2 /arch:SSE /Gr

struct s_298c30
{
	byte field_0[0x20];
	real field_20;
	real field_24;
};

struct s_298c46
{
	byte field_0;
	bool field_1;
	byte field_2[2];
	real field_4;
	real field_8;
	point2f field_c;
	byte field_14[4];
	short field_18;
	byte field_1a[0xe];
};

// @retail 0x298c30
void function_298c30(long arg_0, s_298c30 const *arg_1, real arg_2,
	real arg_3, s_298c46 *arg_4, point2f const *arg_5)
{
	s_slot_object_view *local_0 = object_get(arg_0);
	bool local_1 = arg_4->field_4 < 0.0f;
	bool local_2 = arg_3 > 0.0;
	real local_3 = 10.0f;
	real local_4;
	if (local_0->type == 1)
	{
		local_4 = *(real *)(g_4e3b44[(*(long *)local_0) & 0xffff].bytes + 0x1f4);
		if (local_4 <= 0.0f)
			local_4 = 15.0f;
	}
	else
		local_4 = 15.0f;
	if ((local_1 || arg_4->field_1) && local_2)
		local_3 /= (arg_3 / local_4) * 0.5f + 1.0f;
	if (local_1 || arg_4->field_1)
		local_3 /= arg_1->field_20 * arg_2 * 0.03333333507180214f + 1.0f;
	if (arg_4->field_1)
		local_3 *= 0.8f;
	local_3 = (real)(local_3 / (fabs(arg_4->field_4) * 0.2f + 1.0f));
	if (arg_5)
		arg_4->field_8 = (arg_4->field_c.y * arg_5->y + arg_4->field_c.x * arg_5->x + 3.0f) * local_3 * 0.25f;
	else
		arg_4->field_8 = local_3;
}

s_298c46 g_5047f8[8];
short g_504938;

__forceinline real function_298df3(point2f *arg_0)
{
	real local_0 = (real)sqrt(arg_0->x * arg_0->x + arg_0->y * arg_0->y);
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_0->x *= local_1;
		arg_0->y *= local_1;
	}
	return local_0;
}

real function_50650(real cosine);

__forceinline void function_2992ba(short arg_0, bool arg_1, real arg_2, point2f const *arg_3,
	point2f const *arg_4, long arg_5, s_298c30 const *arg_6, real arg_7, real arg_8, point2f const *arg_9, point2f *arg_10)
{
	point2f &local_0 = *arg_10;
	if (arg_2 < 0.0f)
	{
		local_0.x = arg_3->x * -1.0f;
		local_0.y = arg_3->y * -1.0f;
	}
	else
		local_0 = *arg_3;
	g_5047f8[arg_0].field_4 = arg_2;
	g_5047f8[arg_0].field_c = *arg_3;
	g_5047f8[arg_0].field_0 = arg_1;
	g_5047f8[arg_0].field_1 = local_0.y * arg_4->y + local_0.x * arg_4->x < 0.0f;
	function_298c30(arg_5, arg_6, arg_7, arg_8, &g_5047f8[arg_0], arg_9);
	g_5047f8[arg_0].field_18 = arg_0;
}

// score 305 (was 305): m3 lane 1 r129 lead 310 plain / 284 fast below, not kept. The 0x64 frame and seven-argument cleanup agree. Unity uses XMM1 here and XMM2 in retail. The cosine spill and candidate ordinal still differ.
// GB r11 lead, held as a patch (not kept: the forms are kept only with a byte match): 256 against 305: frame 0x64, `ret 0x1c`, entry through +0x163 (unity in XMM2, global count in BP, outer counter in EBX) now match retail; loop-head distance compare/order, near cosine/sine spill slots (+0x14/+0x28 vs +0x28/+0x1c) and `cos*y` operand order, second normalization store order, and emitter angle load remain.
// MiniMax G (11-20): no new form, no lower score (305, 1732 bytes, retail 1732): the near arm's y row written as one
// expression (arg_3->x * local_12 + arg_3->y * local_11) takes the y row in SSE as retail does, but the cosine goes into
// the register and the facing y into the memory operand (retail the other way round), the y*cos product moves after the
// sine branch (retail hoists it before) and the two field_c stores come out reversed: 365 (1745 bytes), the same code with
// the sum staged or written with the y term first. Copying the facing vector into two locals keeps the products on x87 at
// 374 and 375, and holding the trig pair in a local point2f measures 382; the staged x*sin first form measures 365. The
// first difference is still the constant 1.0f in XMM1 where retail keeps it in XMM2, and the candidate count is still a
// byte against a short.
// MiniMax R (11-20): no new form, no lower score (305, 1732 bytes, retail 1732): the first difference is still the
// register that carries the constant 1.0f, which this build loads into XMM1 where retail loads it into XMM2, and the
// reciprocal copy and the two product registers follow that one choice, so the float register assignment of the whole
// function turns on it; the near arm also keeps the cosine on the x87 stack where retail spills it to a float slot and
// takes the y row in SSE, and the candidate count is a byte here against a short in retail. A permutation search over
// the body (four tries in 399 s) reached 301 only by naming the constant and the index, a form not kept.
// MiniMax D (11-20): no new form, no lower score (305, 1732 bytes, retail 1732). The first difference is still the constant
// 1.0f, which this build loads into XMM1 where retail loads it into XMM2; the reciprocal copy and the radius load then swap
// with it, so the whole float register assignment of the function turns on that one constant. Four source-order trials moved
// the negation of the radius ahead of the normalization (342, the negation materialised early), moved the endpoint block ahead
// of it (368, the frame restructures and the first difference moves to +0x3), swapped the y row's addend order (305, identical
// code, MSVC canonicalises the commutative add) and copied the facing vector's two components into locals (387, the frame
// grows to 0x70). Swapping the two multiplications in the normalizing helper measures 307.
// The near arm still differs in kind: this build keeps the cosine on the x87 stack and computes y*cos on x87 into a float
// slot, while retail spills the cosine to a float slot and computes y*cos with mulss (the y row in SSE) and keeps the x row
// on x87; the cosine's float rounding is what decides which of the two rows is evaluated in SSE.
// The candidate count is a byte here (the count in BL, the endpoint index in BP) where retail has the count in BP and the
// index in BX; as a short the count measures 469 at 1686 bytes (the length no longer matches retail and the caller
// 0x29a190 regresses), as earlier rounds recorded. Body restored to the kept form.
// Earlier header (305, was 305), merged into the note's first line: the 0x64 frame and seven-argument cleanup agree. Unity uses XMM1 here and XMM2 in retail. The cosine spill and candidate ordinal still differ.
// Earlier score 305 (was 305): The 0x64 frame and seven-argument cleanup agree. Unity uses XMM1 here and XMM2 in retail. The cosine spill and candidate ordinal still differ.
// MiniMax B (11-14): no new form, no lower score (382): retail carries the constant at +0x57 in xmm2 where this build
// carries it in xmm1, which then swaps the product registers, retail's counter is in BX where this build's is in BP, and
// the body is still 40 bytes shorter than retail's (1692 against 1732); the normalizer's two products follow the source
// order (x then y), and swapping them moves the x product into the copy register for 384, so the register choice is the
// whole difference there.
// MiniMax S (11-11): the far arm's rotated point now has storage of its own, as retail keeps it at +0x44/+0x48 while the
// difference vector stays at +0x3c/+0x40, and the sine argument is negated in double, which is retail's `fld` of the wrapped
// angle followed by `fchs`. The frame is now 0x64 as retail's and the score falls to 382. Still differs: the entry schedule,
// the normalized XY/magnitude homes and 40 bytes of length.
// MiniMax B (11-13): no new form, no lower score (382): the first difference is the carrier of the constant at +0x57
// (xmm1 here, xmm2 in retail), which then swaps the product registers, and the function is 40 bytes short of retail;
// the normalizer's two products follow the source order (x then y, as here) and swapping them moves the x product into
// the copy register for 384, so the difference is the register choice alone; the entry schedule, the normalized XY and
// magnitude homes and the candidate width are the rest.
// MiniMax I (11-15): no new form, no lower score (382): a permutation search over this body (4 tries in 327 s) found nothing
// better than its own baseline, and writing the far arm's difference vector as two statements measures 382 again (same code);
// what is left in the near arm is that retail keeps the loop distance on the x87 stack for its compare (`fst` of the magnitude,
// then `fld` of the threshold and `fcompi st(1)`) while this build pops it (`fstp` of the magnitude) and compares a reloaded
// float, and the body is still 40 bytes shorter than retail's.
// MiniMax R (11-16): no new form, no lower score (382): the first sum still lands in xmm3 where retail keeps it in xmm0, retail keeps the normalized XY in xmm1 and xmm3 where ours uses xmm2 and xmm4, and the inner cursor, the wrapped angle and the candidate width still take other homes.
// 1732 bytes, frame 0x64 and `ret 0x1c` as retail. First +0x57
// loads unity into XMM1 instead of XMM2. Near cosine precision, inner cursor,
// wrapped-angle homes and candidate width differ.
// Earlier 382 (1692 bytes), after 393, 408, and 395 before the two fixes below.
// Earlier 393 (was 393; 408, and 395 before the two retail-evidenced fixes of b3C below): entry schedule and normalized XY/magnitude homes differ,
// frame 0x60 against retail's 0x64.
// MiniMax S (11-06): The first arg_7 plus arg_8 sum lands in xmm3 where retail keeps it in xmm0, so the
// two byte streams part company at the third instruction and the rest of the function is compared out of
// step; retail also keeps the normalized XY in xmm1 and xmm3 where ours uses xmm2 and xmm4. Splitting the
// sum into two statements or two scalar locals did not move the register choice.
// M3 lane 10 r86 (fast with 0x299460, 0x298c30, 0x29a190, 0x29cbb0; kept form confirmed plain, 0x29a190 MATCH): retail takes the loop
// distance's differences on x87 unrounded (`fld [centre.x]; fsub [point.x]`, both rows, no store between subtract and square); ours
// subtracted x in SSE from a register copy of the centre and rounded it. `(double)` on the centre in the root's differences 393 (kept,
// 1711 bytes, aligned 556 -> 373, masked 169 -> 145; the centre x stays in memory as retail). On it: the sine argument as `-(double)a : (double)a`
// 432 (1683); the near arm's y rows written cosine-first or x-first 393 (same code); the cast on the point side instead 393 (same code);
// `(real)` on both near-arm rows 393 (same code); the near arm's sine before its cosine 393 (aligned 374); the root's y term first 393
// (same code). On the old body: the emission helper in retail's
// store order (angle, side, facing, direction) 416 (1718) and 0x29a190 falls to 362; with the `(double)` root 420 (1698); the sine form
// alone 416 (1714); all three 440 (1678). Left: retail stores the near arm's cosine and sine as floats and takes the y row in SSE, the
// 16-bit candidate count in BP (byte here; a short count breaks 0x29a190), and one more frame slot.
// b3C (11-06), statement walk against retail 0x298d90..0x299451, fast with callers/callees (0x29a190, 0x298c30 stay MATCH):
// FIXED (behaviour): both 0x298c30 calls pass the endpoint distance local_9 (`mov edx, [esp+0x20]; push edx` at 0x29933d/0x2993ec reads the slot of
// `fst [esp+0x1c]` at 0x298f28; the ratio local_13 is stored at 0x29906d (`fst [esp+0x18]`) and is dead after the arccosine), ours passed the clamped ratio: 395 -> 414.
// FIXED (type): the outer endpoint counter is 16-bit in retail (`movsx esi, bx` 0x298ef4, `cmp bx, 1` 0x298f0a, `cmp bx, 2` 0x299435): short 414 -> 408.
// NOT fixed (precision): retail keeps dx/dy of the loop's distance unrounded on the x87 stack (`fld [esp+0x54]; fsub [esp+esi*8+0x64]`, 0x298ef0-0x298f22);
// ours subtracts x with `subss` and rounds it to a float slot. `double dx, dy` gives that head (400) but 0x29a190's frame grows to 0x588 (45).
// NOT fixed (precision): retail rounds the near arm's cosine/sine to float (`fstp [esp+0x28]` 0x298f66, `fstp [esp+0x1c]` 0x298f82) and takes the y row in SSE
// (`mulss xmm1, [esp+0x28]`); ours keeps both on x87. NOT fixed (type): candidate count is a short in retail (`inc bp`, `mov [0x504938], bp`), short 476, 0x29a190 684.
// Emitter stores in retail's order (angle, side, facing, direction) 427, 0x29a190 362; dropping the `&arg_0` read 401, 0x29a190 345.
// Earlier: Frame 0x64 agrees; entry loads, normalization homes and 0x18 versus 0x1c cleanup differ.
// The pointer-to-parameter trick for arg_3 is gone and the score is unchanged; the parameter's own use
// still differs from retail's stack home.
// frame 0x64 agrees; entry loads, parameter cleanup and normalization/trig homes differ.
// Earlier frame 0x68 forms: shared normalizer 433, unsigned count 437, unsigned endpoint 433, both 437, angle end pointer 457.
// Frame 0x64 agrees; entry loads, normalization homes and 0x18 versus 0x1c cleanup differ.
// Earlier shared normalizer 433; unsigned count 437; unsigned endpoint 433; both 437; angle end pointer 457.
// frame now 0x64; entry load schedule, point/magnitude homes
// and candidate arithmetic differ. Byte candidate count 417; byte endpoint 427;
// later center sum 444; early facing result 441; short angle count 433; both byte counts 408.
// Separate trig pairs 408; byte angle countdown 408; retained candidate entry 441;
// angle result from branches 408; separate evaluation scratches 425; general arm first 441.
// Signed byte candidate/endpoint/both 408/408/408; quarter-turn table loop 481;
// short facing result 430. Entry loads and 0x18 versus 0x1c return cleanup remain.
// Helper-owned/returned evaluation 425/425; native endpoint result 443; endpoint value 455;
// ratio value result 408; consuming angle cursor 416; side before distance 408; angle pair initialization 414.
// Native real fixed cosine/sine/pairs 409/430/420; general cosine/sine/pair 408/408/409;
// all cosines 408; all sines 430; all real trig 430. Original double APIs restored.
// On recovered frame: unused/direction/both formal carrier removals 409/408/409; normalized-point
// name/all point homes 408/408; early evaluation/angle workspace 408/408; dead-carrier removal with
// point names 409.
// Existing endpoint helper at negative/positive/both outputs 408/409/409; rounded distance/projection/ratio
// in double 442/444/445; native velocity fields 408; indexed endpoint observations 458. Entry loads,
// XY/magnitude homes and 0x18 versus 0x1c cleanup remain.
// Normalizer input snapshots 448; native normalizer/distance/both roots 443/408/448; snapshots with
// native distance root 448; void normalizer alone/with snapshots/with native roots 408/448/448.
// Original helper and body restored; entry loads, XY/magnitude homes and return cleanup remain.
// Round 341: Persistent endpoint displacement with separate workspace, shared magnitude and word counters 447/465/447/462; existing direction workspace 474/483/474/483. Original body/helpers restored.
// Native normalization-input/evaluation/candidate-direction library copies, pair/all combinations,
// plus native endpoint-array initialization 408/408/408/408/408/408/408/408. Original body and helpers restored.
// Complementary facing bool/byte/short/long 408/408/430/420; with complementary first endpoint
// 442/442/413/411. All restored; incoming object and normalized homes remain.
// Round 138: existing inline vector normalizer at input/direction/both 408/408/408;
// shared endpoint magnitude alone/with input/with direction/with both 407 each
// (frame 0x6c); existing outlined normalizer at both 549. One-point forms rejected;
// original helpers, calls and includes restored. Entry/XY homes and cleanup remain.
// Sum/direction/evaluation point-name order, eight combinations 408.
// Round 240: Shared normalizer factor-first, early uninitialized factor and role names,
// all seven combinations 408; native epsilon-rejection return 408. Internal magnitude/
// reciprocal lifetime forms do not recover entry/XY homes or cleanup; helper restored.
// Round 153 (fast): nullable point through its address (diagnostic) 427; short counters 471, with the point
// through its address 471 (the facing argument moves to EBX); point and facing through their addresses 427
// (ret 0x1c), with short counters 433.
// Round 68: split center around the object lookup with the unused carrier removed 409;
// shared displacement helper 438; direction declared first 409; component direction copy 450;
// entry/both magnitude captures 409/409; object-index address at entry 409 and at all uses 405
// (three-point gain rejected); unused input moved behind the stack inputs at definition/caller 444.
// DH round 234: complete cosine products/all sine products, with reused/separate second
// trig producers 442/408/442/408; those graphs with native word ordinals 471/471/471/472.
// All restored; entry/normalized homes and cleanup differ on the retail-size frame.
// Round 237: arccosine inline declaration, ordinary inline normalizer and ordinary inline
// candidate emitter, all seven nonempty combinations 408 each; all with queued-function
// noinline declaration 408. Original helper bodies and declarations restored;
// matched scoring and the shared-normalizer/caller controls stay 0/749/445.
// Round 240: Direct two-angle array indexing with angle/candidate/endpoint
// byte-byte-byte, short-byte-byte, long-byte-byte, short-short-byte,
// short-byte-short, short-short-short, long-long-byte and long-long-long
// domains: 408/408/409/458/417/471/463/495. Original cursor, counts and body restored.
// Round 251: candidate point/scalar snapshots 401/488; early direction publication 407/459; candidate plus facing snapshots 487/450; early publication with both 487/453. Seven-point best rejected; original helper and body restored.
// The 401 point-snapshot form gives 0x1c cleanup with frame 0x74 at 1733/1732 bytes; restored.
// Round 424: candidate snapshot with helper-owned point/returned point/scalar evaluation
// 445/445/431; each with void normalizer 445/445/431; point/scalar with ordinary inline
// emitter 713/431. All restored; nullable-input residency, XY homes and cleanup remain.
// DL201: optional scoring-input point captured at entry/after projection, by aggregate or
// library copy, direct selection 474/455/474/455; meaningful shared input receiver
// 445/466/445/466. All actual inputs, candidate order, helpers, source fields and
// inherited formal storage restored; entry/XY homes and 0x18/0x1c cleanup remain.
// Round 31 (lane 11): retail keeps the nullable point on the stack because its 16-bit counters
// (loop in bx, index in bp) take EBX/EBP and ESI holds the endpoint pointer; our byte counters
// free ESI, so the point comes in EAX (ret 0x18). Retail also takes the loop's point distance on
// x87 from the centre point's stack home; ours keeps the centre in XMM3/XMM4 (SSE difference,
// two extra homes). Short counters 471 (point back on the stack, facing pointer moves to EBX,
// 1680 bytes, frame 0x68); per-file two-point distance helper with dx/dy locals 411; both 450;
// separate rotated point in the angle loop 430, with short counters 473; the unused first
// parameter's address read removed 409 (and the caller 0x29a190 445 -> 699: LTCG drops the argument).
// Round 246: complete candidate-direction snapshot before publication with
// candidate/endpoint short-byte, word-byte, long-byte, byte-short, byte-word,
// byte-long, short-short and word-word counts 443/443/442/409/409/415/438/438.
// Byte/short gives 1736 bytes and 0x1c cleanup but frame 0x78. Original helper and body restored.
// Round 260: Double endpoint differences with byte-byte/short-byte/word-byte/long-byte/byte-short/byte-
// word/short-short/word-word carriers 437/513/513/510/451/451/515/515. Original distance, counters,
// helpers and body restored.
// Round 479: complete normalizer-square/distance-square/dot product parts,
// masks 1/2/4/3/5/6/7: 407/408/407/407/406/407/406; all plus named
// squared sums 402. One/two/six-point reductions rejected; helpers and body restored.
// Candidate input retained on normalizer/dot, all-part and named-sum graphs
// 405/405/405; named-sum graph with signed-word counters 444. Original input
// observations, helpers, counters and complete body restored.
// Lane 18 round 24 (fast): the two-angle loop as a plain index loop instead of the hand-written cursor and count-down
// 408 (identical code; kept: retail's cursor and count-down are MSVC's strength reduction of this loop); with retail's
// 16-bit counters (`inc bp`, `cmp bx, 2`): both short 471 (0x29a190 445 -> 699), endpoint index only 417 (1720 bytes),
// candidate count only 458 (0x29a190 699); named dx/dy for the loop's point distance 411 (1738 bytes).
// DH round 266: actual two-angle emission table with emission/candidate/endpoint byte-or-short
// ordinals, all eight combinations 562/543/578/564/562/543/578/564. Plain emission
// induction shrinks to 1556-1594 bytes and misses frame/entry/cleanup; original restored.
// DH269: Existing emitter side/angle/direction publication phase subsets 1/2/4/3/5/6/7,
// 404/412/408/409/404/412/411; evaluation followed by angle/direction/side/facing 395 kept,
// plain 395 at 1732/1732 bytes; caller 0x29a190 445 -> 389, no other user regresses.
// Entry stack load and 0x1c cleanup recover; first +0xa and normalized XY/magnitude homes remain.
// DH272: On the retained publication graph, center scalar/paired dot/distance-difference subsets
// 1/2/3/4/5/6/7 give 395/401/401/451/451/449/449; complete graph with named squared sum 449. Original point
// storage, expressions and every helper restored. Frame and cleanup agree; normalized XY/magnitude homes and
// center retention remain.
// Round 44 (lane 15, fast): retail takes the loop's distance on x87 from the centre point's memory home, with dx and
// dy live on the x87 stack, and keeps the outer counter in EBX with a dword spill compared through bx. Double dx/dy
// 437 (1712 bytes, aligned 243, masked 111, against 235 and 111); with both 16-bit counters 488 (1669, masked 108);
// centre built through an output-pointer inline 395 (same code), with double dx/dy, dy first 437; the difference
// through a named point 450 (1751); centre built from the input copy 398.
// Round 331: Inner-angle word with byte-byte/short-byte/byte-short/short-short outer counts
// 395/464/402/471; with scalar center 472; inner-angle long with byte-byte/short-byte/byte-short
// 411/486/409. Original counters, center, helpers and complete body restored; normalized XY/magnitude
// homes remain.
// DH295: Retained evaluation-first emitter with candidate/endpoint short-byte, byte-short,
// short-short, long-byte, byte-long, short-long, long-short and long-long ordinals:
// 464/402/471/442/420/481/444/457. All restored; normalized homes and entry sum remain.
// Round 318: Native endpoint-pointer traversal with ordered/equality end tests and
// byte/short/word/long candidate counts; 432/457/457/435/432/457/457/435. Original whole body and
// helpers restored.
// N324: Raw/bounded projection split with ternary/branch/ceiling-default/independent-bound producers,
// original then test-produced wrapped-angle sum 395/395/404/421/400/400/407/411. Original complete body
// restored; entry and normalized XY/magnitude homes remain.
// DL249: Center-before-object, copy-before-object, both, split-center around object,
// split-center plus input copy, normalization-before-object, normalization before center,
// and split center around both: 395/397/407/395/397/407/415/413. All original producers restored.
// AC486: Double magnitude sum/product/pair/full-product intermediates 395/453/395/453;
// matching endpoint-distance intermediates 402/442/402/442. NOT kept: precision/grouping
// forms do not match; original normalizer, distance, helper and whole body restored.
// N328: Object receiver after sum/normalization/endpoints/at projection, original then native velocity
// fields on the retained emitter graph: 395/407/434/436/397/407/434/448. Original receiver timing and
// body restored; entry sum and normalized XY/magnitude homes remain.
// DH310: Complete normalized scalar outputs/native point/library copy/component point, original/early-reject guard,
// 395/467/467/385/395/467/467/385. NOT kept: the unmatched 385 groups outputs in another point record;
// only SSE ranks improve, native XY/magnitude homes remain. Original helper and entire body restored.
// AC492: Emitter direction after facing/exhaustive facing/direct output formal
// 412/398/395; their three pairs 408/412/398, all 408; native signed evaluation point 402.
// Original emitter and whole body restored; entry sum and normalized XY/magnitude homes remain.
// M3 lane 13 r54 (fast): the normalizer helper's locals named magnitude/inverse 395 (same code); the loop distance
// through the existing vector2d_from_points2d + magnitude2d 450 (1751 bytes); the distance declared at function
// scope 395; the negated radius written inline in the first endpoint 436 (1711 bytes); the centre assigned by
// component 395. Retail keeps the loop distance in a stack home (fst, then fdiv from memory) and 16-bit counters.
// F348: Existing shared 2D dot at input-facing, velocity, endpoint and signed-facing
// consumers, then input pairs and all consumers: 410/395/395/395/410/410/410/410.
// NOT kept: shared real-result/operand-order forms do not match; original expressions,
// helpers and full body restored; entry sum and normalized XY/magnitude homes remain.
// Round 554: On evaluation-first emission and recovered seven-word cleanup, complete
// normalizer input/native-root/void-contract subsets 437/437/395/395/437/437/395;
// all with complementary early rejection 395. Original helper and body restored; every
// shared user unchanged. Entry sum and normalized XY/magnitude homes remain.
// Round 44 (m3 lane 2, fast with caller 0x29a190 at 389): both now return with `ret 0x1c`. Retail stores the
// centre's x at once (`movss [esp+0x50]`), takes dx/dy on x87 from it, and keeps 16-bit counters (outer in
// EBX, compared through bx; candidate count in BP, stored as a word). The loop differences as `(double)c - p`
// in the root 418 (1698 bytes); short candidate count 464 (1689; caller 699), with double dx/dy locals 445
// (1694; caller 698), with the `(double)` differences 513 (1646), plus a short outer counter 504 (1651); short
// inner-angle counter 395 (same code), with the short candidate count 464; all three short with double dx/dy
// 488 (1669; caller 698).
// M3 lane 9 r83 (fast): the point locals renamed by role (center/direction/endpoints/offset/angles/candidate) 395
// (same code: names move no slot here); the far branch's difference through the existing vector2d_from_points2d 407
// (1743 bytes); the loop distance through a file-local accumulating distance2d 451 (1751); the root summed y-first
// 395 (same code). Retail negates the inner sine's angle on x87 (`fld [angle]; fchs`), ours with SSE `0 - x`: the
// sine argument as `-(double)a : (double)a` 425 (1709 bytes, fchs as retail but the entry moves to +0x2); the sine
// as an if/else of two calls 419 (1715 bytes).
// M3 lane 6 r142 (fast; both now `ret 0x1c`): local_2 renamed `direction` 395 (same code); the loop distance's
// squared sum built in two statements before the root 395 (same code); a pointer to the emission entry in the
// first arm 431 (1695 bytes); each emission's y written before x 421 (1743 bytes); diagnostic: the arg_0 address
// read removed 401 (1728 bytes, cleanup 0x18, caller 0x29a190 697). Retail spills the first arm's cosine to
// [esp+0x28] and takes the y product in SSE before the sine; ours keeps both on x87. Original body restored.
// Lane 14 r60 (fast with caller 0x29a190, base 395/389; "no-home" = aligned count ignoring stack offsets, base 277): the first
// arm's y product named before the sine in both emissions 412 (1743 bytes, no-home 265); double difference locals in the
// root, y-first or x-first, 437 (1712 bytes, no-home 284, frame moved); both counters short 471 (1680 bytes, no-home 262;
// caller 699); short candidate count alone 464 (no-home 273; caller 699); short outer counter alone 402 (no-home 305);
// double differences with both short 488 (no-home 312; caller 698); all three 464 (no-home 292; caller 698); short counters
// with the named y product 449 (1695 bytes, no-home 243, the closest structure; caller 699).
// AC507: Existing point setter at sum/endpoint/fixed-rotation producers, subsets
// 1/2/4/3/5/6/7 then general rotation too: 457/458/489/485/524/541/561/593.
// NOT kept: all regress and grow; original producers, setter declaration and full body restored.
// Unified endpoint-side predicate for all orientation consumers, second/first polarity
// as bool/byte/short/long: 415/436/437/436/438/438/446/430. All regress;
// original predicates, counters, helpers, publication and whole body restored.
// P305: Twice-used fixed-turn sine angle, double/real, near-arm/endpoint lifetime,
// conditional/test-produced: 414/414/417/417/407/410/408/411. All regress; original
// whole body, normalizer, emitter and double trig inputs restored; native XY homes remain.
// M352: First-trig product pairs before second trig at positive/negative/general rotations,
// masks 1/2/3/4/5/6/7 397/399/414/395/397/399/414; complete paired products 497.
// NOT kept: early one-use products and changed real rounding do not match; original whole body restored.
// M359: Fixed-pair sine-first/general-pair cosine-first producer subsets 1-7,
// then all with a separate negative fixed pair: 405/405/405/421/403/403/419/419.
// NOT kept: unmatched schedules regress; original trig producers and complete body restored.
// DH324: Whole native output-record references at first/second/repeated emission,
// seven subsets then all native pointers: 413/413/431/432/450/450/468/468. Original destinations,
// helper, publication graph and complete body restored; entry/normalized homes remain.
// DH326: Positive-default evaluation output; negative arm from original/stored input,
// pointer/reference output formals and void/ignored-pointer result contracts:
// 420/420/420/420/434/434/434/434. Original emitter and complete body restored;
// entry sum and normalized XY/magnitude homes remain.
// M374: Existing emitter returns next ordinal, byte/short/word/long result with
// byte/short candidate count: 428/428/428/428/419/464/464/464. NOT kept: all regress;
// short/word/long results on short count regress caller 389 to 699. Original emitter,
// counters and whole body restored; entry and native XY/magnitude homes remain.
// Round 134 (m3 lane 8, fast with caller 0x29a190): the first arm's rows through a pointer-output rotate inline, field_4 stored first 418 (1743
// bytes), stored after 421; with the loop's rows through it too, named or inline 416/418; double sine/cosine parameters 439 (caller 405); rows
// written operand-first 418; y row first 416. Retail stores the centre's x before computing y (x and y share XMM0), ours keeps x in XMM3 with 1.0
// and the temporaries rotated one register against retail's.
// DH361: Existing normalizer epsilon decision as rejection/normalization bool/byte/short/long,
// eight at 395; the other normalizer user stays 719 and scoring stays exact. NOT kept: no match
// or native XY/magnitude-home recovery; original comparison, shared helper and full body restored.
// DH363: Actual candidate-record flags bool/bool, byte/byte, bool/byte, signed-char/bool, byte/signed-char,
// bool/signed-char, signed-char/byte, signed-char/signed-char: 395/401/401/395/401/401/401/401. NOT kept: no match;
// original byte/Boolean declarations, record layout, publications and complete body restored.
// DH365: Original-site complete endpoint-distance admission, bool/byte/char/signed-char/
// short/word/int/long: 395/395/395/395/395/395/395/395. NOT kept: nonexact one-use predicate forms; original
// body restored. Frame 0x64 and cleanup agree; entry and normalized XY/magnitude homes remain.
// DH368: Forward/backward standard copies at original input/positive-evaluation boundaries,
// eight non-original pairs: 410/494/517/410/494/517/517/517. NOT kept: all regress;
// five additional candidate-publication copy graphs excluded for added shared-global addresses.
// Original input/evaluation copies, global assignment, emitter and complete body restored.
// Round 57 (m3 lane 15, fast; brackets are aligned counts with stack homes and registers masked, base [180]; 0x29a190 now matches and must stay
// matched): (source error since fixed by b3C, see the note at the top of this block) retail passes the distance local_9 to 0x298c30 (`mov edx, [esp+0x20]; push edx` reads
// the slot of `fst [esp+0x1c]`), ours passes the clamped ratio local_13. Corrected alone 414 [174] (1727 bytes). On it: short outer counter, `(double)`
// differences in the root, y product named before the sine 374 [129] (1711), alone 408/424/414 [169/160/159]; short candidate count 509 [134] (1652), but
// the facing pointer then comes in EBX and 0x29a190 falls to 684; `&arg_3` read to stop that 436 [135] (0x29a190 127). Named `double dx, dy` locals give retail's
// loop head (`fsub [esi + 4]`, `fld st(0); fmul st(1); fld st(2); fmul st(3)`) 375 [129], but their alignment grows 0x29a190's frame to 0x588 (45). Emission
// helper in retail's store order (angle, side, facing test, direction, call, ordinal) on the 374 form 367 [98] (1711), the best here, with 0x29a190 still
// matched; without the y product and casts 416 and 0x29a190 362. Near arm on 367: (real) casts on both rows 366; dir fields in locals first 430, after the cosine
// 394, no product named 416; y row stored first 458; rotation in a point2f 367; sine as if/else 443. Retail stores the near arm's cosine and sine as floats and
// takes the y row in SSE; ours keeps both on x87. Inline distance through point pointers 395 (same code); dx/dy locals in it 451, delta point 450; 2-D normalizer
// with `fabs < eps`, else-zero and return 395 (same code), `local_1 * x` 395.
// M3 lane 10 r88 (fast with 0x29a190, 0x299460, 0x298c30, 0x29cbb0; base 393, aligned 373, masked 145): one sine/cosine pair
// declared at the loop top and used by both arms 393 (same code); the distance assigned without the `(real)` cast 393 (same code); the
// test written `field_24 > distance` 393 (same code); on a separate rotated point in the angle loop (re-measured 399, aligned 382): the
// emitter's stores in retail's order (angle, side, facing test, direction) 452 (1698 bytes, masked 124; 0x29a190 362: the entry no
// longer reads the object index first and the frame grows to 0x6c); that with the short candidate count 502 (1657, masked 140; 0x29a190
// 684). Re-measured on this base: cosf/sinf in the near arm 398; short candidate count 510 (1651, 0x29a190 684); separate rotated point
// with the short count 502 (aligned 267). Retail's far arm stores the rotated point to its own temporaries (0x44/0x48, not the
// difference's 0x3c/0x40) and keeps its x bits in EBX across the emitter, so the outer counter's EBX is spilled and arg_3's EDI is
// reloaded after the emitter; with a short count ours gives arg_3 EBX for the whole body and LTCG then passes it in EBX.
// M3 lane 10 r90 (fast with 0x299460, 0x29a190, 0x298c30; 0x29a190 MATCH throughout): nothing kept. Lead, 382 (plain-confirmed; frame 0x64 as
// retail, first difference +0x2 -> +0x57, aligned 373 -> 239, masked 144 -> 140): a separate rotated point for the far arm (the difference
// vector no longer reused) together with the sine argument as `-(double)a : (double)a` (retail's `fld [angle]; fchs` on x87 frees the negated
// angle's slot). Not kept: the casts change no value (a float negation and its promotion are exact), so they only steer; without them the
// separate point alone is 399 (re-measured), a rise, and the casts alone were 432 on an earlier body. With the pair, homes agree except
// one: retail keeps the wrapped angle at +0x14 and the clamp/normalizer/loop-cursor group at +0x18, ours the other way round. On 382:
// the angle element read at each use instead of local_21 (retail reads `[eax]` at every use) 381 (aligned 278; 400 on the old body); the
// wrapped angle as if/else 382, the near arm's or the far arm's sine/cosine without the `(real)` casts 382 each, the side selection as one
// nested ternary 382 (all same code); the root's differences as `(double)(a - b)` 417; descriptive names for the wrapped angle and the
// rotated point 382 (same code: names do not order the pair). Re-measured: the short candidate count 520 (0x29a190 684: its own entry
// convention changes).
// m3 lane 7 r150 (fast with 0x29a190, which stays MATCH, 0x29cbb0, 0x299460, 0x298c30; a one-link LTCG harness entered
// through 0x29a190/0x29cbb0 reproduces this body exactly: 393, and lane 10's 382/399/502/520): retail's extra frame dword is the
// wrapped angle, kept as a memory float at +0x14 and negated on x87 (`fld; fchs`) for the sine; every small slot below +0x30 sits
// 4 bytes lower here. Retail's second side (no ratio arm) lays the `2pi - x` arm first, here and in 0x299460. The wrapped angle
// tested as `local_21 >= 0.0f ? local_21 - 2pi : local_21 + 2pi` (retail's `comiss angle, 0; jb`) 393 (register-masked aligned
// 342 -> 330); the second side's ternary with the facing test negated 399; both 399; on lane 10's 382 lead (separate rotated
// point, double sine argument) the compare 382 (masked 169 -> 159), the compare and the negated test 388.
// Then: the side flag as `local_7 != 0` 397 (1700 bytes), as `local_7 > 0` 397 (other code); the outer loop tested `local_7 != 2` 393
// (new code); the facing test against `0.0f` 403; the inner loop tested `!= 2` and the second clamp as one nested ternary 393 (same code).
// M3 lane 10 r92 (fast with 0x29a190, 0x299460, 0x298c30; base 393, aligned 373, masked 145; 0x29a190 MATCH throughout): nothing
// kept. Retail spills the near arm's cosine to +0x28 before the sine argument's branch, forms `y * cos` in SSE ahead of that branch,
// stores the sine to +0x1c (the distance's slot) and takes the x row on x87 from the two float slots; ours keeps both values on the
// x87 stack across the branch (one frame slot fewer). Near x rows as `(double)local_11 * x - (double)local_12 * y` 391, only the
// cosine term cast 391, the y rows cast `(real)(...)` 393 (same code); field_4 stored between the cosine and the sine (retail's
// store position) or first 393 (masked 130, the same code as each other); all four near-arm trig values computed before the two
// emissions 365 (1743 bytes, masked 163: only the first pair then spills to float slots; hoisted declarations, not kept).
// Variants of that (re-measured 365): both cosines first, each sine at its emission 397 (masked 144); both sines first 372 (1713
// bytes, masked 148); the second pair before the first 365 (masked 155); cosines then sines 365 (masked 163). The positional drops
// (372/365/365/365) come with masked 148/155/163/163 against the base's 145 (shifted alignment, not retail's float-slot cosine).
// Other blocks: the wrapped angle as
// `local_21 >= 0.0f ? local_21 - 2pi : local_21 + 2pi` (retail's `comiss x, 0; jb` to the add) 393 (masked 140), the same as an
// if/else 393 (same code); the far arm's facing dot written x term first 393 (same code); both 393 (masked 140); the ratio clamp
// written `1.0 < local_13` 393 (same code).
// M3 lane 10 r97 (fast with 0x29a190 MATCH, 0x298c30 MATCH, 0x299460 569 throughout; base 382; both builds `ret 0x1c` with the same
// argument setup at 0x29a190's call): nothing kept. The positive endpoint computed before the negative one 396; both near-arm y rows
// written sine term first 382 (same code; a listed form, r86's x-first); the sine assigned inside the y row with the x row after it (diagnostic, an embedded
// assignment) 382, same code: the cosine and sine stay on x87 across the sine argument's branch, where retail spills the cosine to
// +0x28 and forms `y * cos` in SSE before the branch; the emitter's facing output declared inside the inner loop 382, the rotated
// point declared outside it 382 (same code); the sine's angle selected into a named double after the cosine, so the branch is
// its own statement, 412 (0x29a190 45: the double local aligns the frame).
// MiniMax A (11-12): no new form, no lower score (382): retail loads its entries in another order and keeps the normalized
// point and the magnitude in homes of their own, while ours still shares them with the entry loads.
// P435: immutable projection, endpoint distance, opening/direction/candidate/wrapped
// angles and facing decision, plus direct complete rotated-point initialization, all 382.
// NOT kept: no gain. Entry and first-normalization homes agree; inner cursor and wrapped
// angle exchange +0x14/+0x18. Caller and scoring sibling stay matched in all eight trials.
// T97: Complete upper-turn wrap truth in bool/byte/char/signed-char/short/word/int/long:
// eight at 382. NOT kept: byte-neutral decision lifetimes; original array wrap,
// shared producers, all call tuples and whole body restored. Unity/XY/wrapped-angle homes remain.
// P455: Isolated function storage for evaluation output, endpoint array, fixed
// cosine/sine, endpoint distance, projection ratio, directional/wrapped angle:
// eight at 382. NOT kept: no match or native unity/angle-home repair; all
// producers, shared helpers, call inputs and original complete body restored.
// T102: Delegated radius/direction/both caller clamps 422/437/470;
// direct ratio/dot/both inputs 422/437/470; complete shared dot/normalizer
// with delegated bounds 470/470. NOT kept: no exact unity/angle-home
// repair; original caller clamps, inputs, helpers and whole body restored.
// M3 lane 12 r121 (one-file LTCG harness with the callee files, 1692 bytes, 341 for the plain 382; fast where given): ours
// returns with `ret 0x1c` as retail; candidate count `short` 520 (1638 bytes; EBX then goes unused and the caller 0x29a190
// changes its own convention, 684); the near arm's rotation through a forceinline helper taking cosine and sine by value,
// same code (parameters do not round); `cosf`/`sinf` with float angles 407; a short candidate count with a `long` outer
// counter 420 harness (1627 bytes); the near arm's cosine and sine declared at loop scope, same code.
// P467: Current ordinal consumed at first/second/both emission calls, byte count,
// then both with char/short/word/long/dword count: 382/382/382/382/520/520/430/430.
// NOT kept: no gain; short/word also lose the matched caller. Original whole body restored.
// Complete fixed-turn point producers, positive/negative/both, aggregate and
// component forms: 363/364/367/363/364/367; complete scalar rows at both fixed
// turns, then all rotations: 365/365. NOT kept: point forms grow frame 0x64
// to 0x6c or 0x74; scalar forms are nonexact one-use producers (1745 bytes).
// Original whole body restored; unity, wrapped-angle/cursor homes and count width remain.
// P494: negative radius before object/sum/motion-copy/normalization producers
// 420/420/419/419; immutable negative radius 382; normalizer magnitude/reciprocal
// immutable 382/382; existing shared magnitude interface inside normalizer 382.
// NOT kept: ties or regressions. Original whole body and shared normalizer restored;
// native unity carrier, wrapped-angle/cursor homes and near trig spills remain.
// M3 lane 7 r162 (fast with 0x29a190 and 0x298c30 matched; aligned 239 on the base): the emitter's stores in retail's
// order (angle, side, facing, direction) 391 (aligned 295); the wrapped angle as `>= 0.0f ? x - 2pi : x + 2pi` 382 (aligned 239,
// same code); both 391; the wrapped angle renamed to sort before the distance (the pair's slots do not move) 382; retail's
// store order with the angle element read at each use 389 (1691 bytes, aligned 228); the element at each use alone 381
// (aligned 278, re-measured).
// M3 lane 3 r246 (fast with 0x29a190 and 0x298c30, both matched unless noted; difflib ratio of the two listings in brackets,
// base 0.621): the candidate count `short` re-measured 520 at 1638 bytes (0x29a190 684; ours then returns `ret 0x18` and
// leaves EBX unused) [0.282]; the candidate entry through a named pointer in the near arm 434 (1649) [0.612], in 0x2992ba 419
// (1662) [0.600], in both 462 (1619) [0.591]; the near arm's `y * cos` named right after each cosine, ahead of the sine and
// the field_4 store, 349 (1693) [0.619]: positional only, the product stays on x87 (retail's is SSE from the spilled float
// cosine) and the form is match-only, not kept; the outer counter declared ahead of the count 382 (same code); the count
// declared at the function's top 433 (0x29a190 3); the inner counter `long` 411 (1671), `short` 382 (same code); the wrapped
// angle as `>= 0.0f ? x - 2pi : x + 2pi` 382 (same code, re-measured). The 40 missing bytes are the near arm's: retail
// rounds both trig values to float slots there and takes the y rows in SSE.
// P519: complete emitter-facing result at its original scoring boundary,
// bool/byte/short/long away truth then aligned complement: 391/389/381/383/389/389/381/383.
// NOT kept: no exact result-carrier match; original emitter, operands,
// publication graph and complete body restored. Native unity/angle homes remain.
// M3 lane 11 r106 (fast with 0x29a190 and 0x298c30, both MATCH throughout; base 382): diagnostic, the near arm's cosine and
// sine through a forceinline helper with two output pointers 382 (same code: the outputs are scalar-replaced and stay on
// x87, no float slots); field_8 stored right after field_4 in both entries 382; side/facing flags stored before the
// direction 381; the x rows written point first (`x * c - y * s`) 382; the second entry with its own cosine/sine locals 382.
// Endpoint-side sine test and unit-turn coefficients 382/382. Shared planar
// products 350, NOT kept: both rows stay on x87 and add sine negation.
// Reused cosine 406; one signed sine 390; captured direction 393. Staged y
// 305; widened x 305; staged y with cosf/sinf/both 305/360/360; y scaled
// in place 305. Staged y keeps the angle store and first product before the
// sine and rounds the sine contribution into SSE. Cosine still stays on x87.
// Round 615: unity still lands in XMM1, and retail spills the cosine before its y product.
// Signed candidate count with staged y 469 (caller 684); scored calls postincrement 305;
// separate fixed-turn scopes 305; captured x inputs 379; selected entry pointer 342;
// upper angle published after wrap 309; two-arm endpoint side 355; role names 305.
// Endpoint root compared wide with an immediate real distance 352; explicit wide numerator 305; wide
// radius quotient through the upper bound 348; wide tangent dot through both bounds 350; float outer
// bounds 398; wide near comparison with distant-arm rounding 384, with wide numerator 384; wide
// quotient and dot with bounded real results 353. Near cosine still stays on x87 and the candidate
// ordinal is still a byte.
// Table-driven fixed turns, real paired angles with component/point stores 481/505,
// real separate tables 491/510, double paired angles 592/610, double separate tables
// 578/591. NOT kept: length shrinks by at least 98 bytes and the unity register still differs.
// Complete central-angle selection using the endpoint ordinal or its existing
// positive-endpoint Boolean: four-way assignments 305/314, initial angle with
// quadrant adjustments 307/322, endpoint-first conditional initializer 305/314,
// facing-first conditional initializer 322/319. Unity remains in XMM1.
// Lane N round 411 (fast control 305).
// Shared displacement input 396.
// Sum from displacement copy 438.
// Componentwise displacement use 419.
// Shared input before object 368 (NOT kept: matched caller rose to 2 in the reduced image).
// Named facing projection 305.
// Projections before endpoints 379 (NOT kept: entry repaired through +0x9e but body score and extent regressed).
// Angle pair then wrap 304 (NOT kept: one-point gain without a match).
// Far endpoint workspace 368.
// Facing before endpoints 333.
// Facing between endpoints 336.
// Facing after first coordinate 332.
// Unity and radius still exchange XMM1/XMM2. Endpoint writes interleaved x first 309.
// y first 318.
// all endpoint products staged 305.
// products staged per axis 313.
// indexed signed-radius pair, short with component or point stores 396/416.
// indexed signed-radius pair, long with component or point stores 393/463.
// M3 lane 5 r197 (fast with 0x29a190 and 0x298c30, both MATCH unless noted; aligned exact / register-masked counts in
// brackets, base [257/123]): the candidate count `short` re-measured 469 (1686 B; ours then saves no EBX, takes the facing
// pointer in EBX, `ret 0x18`, and keeps the outer counter in EDI; 0x29a190 684). Near arm: both trig values as locals with
// the y row as one expression ahead of the x row (`turned_y = y * cos + x * sin;` after the sine) 365 (1745 B) [249/126]:
// the cosine and sine are then rounded to float slots, the y row is SSE and the x row x87 from the slots as retail, but
// `y * cos` and the load of x come after the sine where retail has them before the sine argument's branch (`movss xmm1,
// [y]; movss xmm0, [x]; fstp [cos]; mulss xmm1, [cos]; jne`), and ours multiplies the loaded cosine by [y]; the same with
// the y row written x term first 365 (same code); the rotated point through a new inline setter taking the two rows by
// value 365 (1745 B) [221/125]; the y term loaded and then scaled (`turned_y = y; turned_y *= cos;`) 305 (same code as the
// staged form). Compile-only (the screen reproduces this arm; 55 spellings): the sine written out in both rows is
// recomputed with a second branch and puts the y row on x87; an embedded sine assignment in the y row is evaluated before
// the row; rows passed by value to an inline setter (either parameter order), or the vector, cosine and sine passed to an
// inline rotation (either parameter order, four row orders, with none, one or both trig locals): none starts the y row
// before the sine's branch. Left as before: the short count (blocked by the EBX convention above), the constant's
// XMM1/XMM2 and the near arm's order.
// The cosine and unity still use different homes, and the candidate ordinal and far cursor keep their
// opening widths.
// Tried fixed turns retain their y input between both products 413, fixed turns snapshot their headings
// after angle publication 386, fixed turns name the wide cosine contribution to x 349, fixed turns select
// the sine result after the staged y product 405, distant arm first with a complementary distance test
// 354, distant arm exits the endpoint before fixed turns 354, distant arm first with retail candidate
// width 480, orientation tests reuse the shared endpoint side 364.
// NOT kept: The wide cosine term grows the body and leaves the first unity load different. The distant-
// first form with a signed candidate also changes the matched caller convention.
// Real radial upper bound 365, tangent upper/lower 366/361, lower-first
// tangent clamp 342. One real quarter turn 360, separate y products 305,
// near continue 305, complete signed endpoint initializer 305.
// NOT kept: the real radial bound restores the first unity load but shortens
// the body by 15 bytes. Near cosine and candidate ordinal homes remain.
// Signed candidate total with earlier projections (both, velocity, facing, facing between
// endpoints, velocity between, both between, both before radius sign) 471/469/468/469/469/469/471.
// Earlier projections with a cached positive radius 384. NOT kept: signed total shortens
// the body and changes the caller convention. The unity register remains on the cached-radius form.
// Unity still uses XMM1, and the near cosine still lacks the retail spill. Conditional radial cap 305, in-place
// radial quotient 305, conditional tangent cap 305, separate tangent bound tests 330, branch-specific radial angle
// call 331, shared signed endpoint writer 305, normalization before translated sum 359, unit score first 313.
// NOT kept: the clamp and normalization forms leave the unity allocation unresolved.
// Shared signed sine argument at admission or first use 329/323. Shared turn sign 316.
// Endpoint-address sine choice 332. Byte slots consumed separately or reserved together 355/370.
// Signed consumed slots 313. Shared sine argument with separately consumed byte slots 420.
// Unity, cosine spill and persistent candidate ordinal still differ.
// M3 lane 1 r122 (fast with 0x29a190 and 0x298c30; base 305, aligned 171; nothing kept). Lead: retail's count is the
// global itself. `g_504938 = 0;` with `g_504938` indexed, stored and incremented directly (no local) gives retail's
// `xor bp, bp`, a 16-bit `inc bp` at every increment (a short local gives `inc ebp`) and the store back between the
// exit pops: 468 (1688 bytes, aligned 234), because the facing pointer then arrives in EBX (`ret 0x18`, 0x29a190 684).
// On the global count: the emitter taking the direction by value 355 (frame 0x74, 0x29a190 MATCH); the emitter's
// stores in retail's order 469; both 373 (0x29a190 MATCH); the emitter's body written out at both calls with the
// rotated point read directly 468 (same bytes, diagnostic); the direction as two reals 469 (1634 bytes); field by
// field copies 469; the direction stored first 486; the direction copied to the facing point first and negated in
// place 390 (1673 bytes, frame 0x64, first +0x2e, aligned 179, 0x29a190 MATCH; by value the same bytes): that one has
// retail's registers (outer counter in EBX with its +0x28 spill and the `lea ebx, [ebx]` pad, facing pointer in EDI
// reloaded at the inner loop's bottom, the direction's x bits in EBX, `ret 0x1c`) but copies in both arms; the
// wrapped angle as `>= 0.0f ? x - 2pi : x + 2pi` 468; the far y row sine term first 468; the angle element passed to
// the first emission 470; those three 470, and with retail's store order 470 (0x29a190 684 in all five). The facing
// pointer leaves EBX only when the direction's x bits get a callee-saved register; here that needs an unconditional
// aggregate read of the direction at the top of each emission, and retail has the register (`mov ebx, [esp+0x44]`
// right after the x row) with the copy still in the non-negative arm only. The short count re-measured 469. An
// aggregate copy of the rotated point into the shared facing point in the caller, once per inner iteration ahead of
// both emissions, 468 (removed as a dead store: same bytes); the same with the angle element 470.
// Near arm, compile-only (the screen reproduces it; 30 shapes, none with retail's order): a product is SSE only when
// its vector component already sits in an XMM register, and x87 when its operands are memory and x87 values. The sine
// as an if/else of two calls on the staged y: cosine in a float slot, `y * cos` still x87, x row SSE; the y row as
// one expression after both locals, either operand order, or with the sine assigned inside it: cosine in a float
// slot and the y row SSE, but the sine's branch first; the sine argument in a float or double variable with sin and
// cos written in both rows: the repeated calls merge into float temps (y row SSE, x row x87 as retail), branch
// first; the sine with its ternary written in both rows: computed twice; cos written twice with a sine local;
// scaled by cos and then the perpendicular added, through a point or two scalars: all x87; x and y copied to locals:
// `y * cos` SSE with the cosine spilled as retail, the rest x87 from stack copies; x and y by value to an inline
// rotation, two parameter orders: all SSE; a by-value scale helper: same code as now; x and y scaled in place: x87.
// MiniMax E (11-19): score 305 (was 305): this round's function was 0x299460, so nothing was changed here; the body stands
// as the earlier rounds left it (the count is still a byte local where retail uses the global itself, and the facing pointer
// still leaves EBX in one arm only), and 0x298c30 and 0x29a190 still match.
// M3 lane 15 r104 (fast with 0x29a190 and 0x298c30, both matched throughout, and 0x29cbb0 378; control 305, all restored):
// the candidate count `short` (retail's 16-bit BP) 469 (1686 B, `ret 0x18`: arg_3 then arrives in a register and lives in
// EBX, the outer counter takes EDI; 0x29a190 684); that with arg_3 read once through its address into a local used at
// every arg_3 use 411 (1704 B, `ret 0x1c`; the magnitude then takes arg_3's dead home; 0x29a190 127); that with the outer
// counter `long` 483 (1675 B, `ret 0x18`); one shared inline rotation of the facing direction (cos x - sin y, cos y + sin x)
// for both near-arm emissions and the far arm 382 (1692 B); the existing normalize2d_fast for both normalizations (their
// results are unused) 305 (same code).
// Cosine from published angles 305, with shared direction components 387. Shared direction
// components before both turns / after each angle 387/426. Y stored before x 346.
// Named wide distance components 316. Planar normalizer at entry / tangent 391/305.
// NOT kept: entry normalization recovers unity in XMM2 through +0x9e, then schedules
// the y projection ahead of x and shortens the body to 1722 bytes.
// Round 436. Planar entry normalizer with named double or real projection products
// 319/316; accumulated double or real projection 320/315; products before endpoints
// 319/316; complete double or original predicate before endpoints 382/333.
// NOT kept: the six product forms retain 1732 bytes but keep unity in XMM1. The
// early predicates disrupt the entry. Original body restored.
// Staged near-turn y rows previously lowered the score from 382 to 305.
// Signed candidate word with cursor latch in pre-test or post-test loop 469/469, cursor consumed at read 479/479.
// Signed candidate word with end-pointer latch or read advance 489/493, byte or short countdown at latch 469/469.
// NOT kept: every signed-count form shortens the body and changes the matched caller.
// M3 lane 8 r199 (fast with 0x29a190 and 0x298c30, both MATCH throughout): lane 1 r129's lead rebuilt (global count, near rows in
// per-side arms, emitter storing angle and side first) 310 (1716 B, first difference +0x163, aligned 127, masked 89 against this
// body's 260 / 159). On it: the wrapped angle as `x >= 0.0f ? x - 2pi : x + 2pi` 311 (aligned 117, masked 84); the angle element
// read at each use 315 (1714 B, aligned 116, masked 79); both 314 (1717 B, aligned 104, masked 71); the near y row as
// `local_11 * arg_3->y` or y term first 310 (same code); the loop root summed y-first 310, on both 314 (same code); no `(real)` on
// the root 310 / 314 (same code). Diagnostic dead locals 1..7 on both: 314/316/314/314/314/314/313 (the loop head never moves).
// Our loop head stores the root and reloads it for the radius compare (`fstp`, two pops, `fld`) where retail keeps it on the x87
// stack (`fstp st(2); fstp st(0); fst`): one instruction more, shifting the near arm by a line.
// Near direction point per candidate 372, at near-arm scope 372, at endpoint scope 372, or shared
// with the far arm 370. Facing-copy point per candidate, per near arm, or shared 410/449/437.
// Separate pre-sine and completed y values 305. NOT kept: The point workspaces grow the body and
// some grow the frame. The separate y values tie and leave the cosine spill unchanged.
// M3 lane 1 r129 (fast with 0x29a190 and 0x298c30, both MATCH unless noted; base 305, aligned 171/167 = unmatched instructions
// ours/retail in an LCS-aligned diff; plain where said; nothing kept). Four retail-evidenced forms that work only together:
// (a) the emitter's stores in retail's order (angle, side, facing test, direction) 314 (aligned 140/136; plain 314);
// (b) the near arm's side as an if/else with one sine local assigned in each arm and the two rows written in both arms
// (retail forms `y * cos` in SSE and loads x ahead of the sine's branch, which only the rows inside the arms give; the
// cosine then goes to a float slot and unity to XMM2 as retail) 353 alone (1745 B, first difference +0x147);
// (c) r122's global count used directly; (d) the distance compared unrounded through a double local with a float copy
// for the later uses (retail's `fst` and compare on the x87 value) 352 alone (frame 0x68).
// (a)+(b)+(c) 310 fast and 310 plain (1716 B, first difference +0x163, aligned 72/76; count in BP, outer counter in
// EBX with its spill, x bits in EBX, `ret 0x1c`, caller MATCH: with the rows in the arms the count no longer sends the
// facing pointer to EBX); (a)+(b)+(c)+(d) 284 fast (aligned 70/75) but 334 plain: the plain build's frame is 0x68 (one
// unused dword at the bottom) wherever the double local is present (plain: (d) alone 352, (a)+(b)+(d) 351, (b)+(c)+(d)
// 334, (a)+(c)+(d) 473 with the caller 684). Saved as build/r129/lead-298d90-plain310.patch and
// lead-298d90-fast284-plain334.patch. Other sums (fast): (b)+(c) 315; (b)+(d) 327; (a)+(b) 342; (b)+(c)+(d) 283;
// (a)+(b)+(d) 310 (1741 B).
// On the four together (284 fast): wrapped angle as `>= 0.0f ? x - 2pi : x + 2pi` 284 (aligned 64/69); the emitter's
// angle through a pointer 291, the array element by value 286, the element read at each use with `>=` and no copy 284
// (aligned 56/60), that through a pointer 288 (aligned 53/57), the element with `<` 283; emitter dot x first 284; the
// second side's ternary negated 283 (aligned 52/56); six operand orders of the arm rows 284 (same code); the cosine
// assigned inside both arms 284 (same code); the angle store inside the arms 455 (caller 684); the second normalizer's
// dot, the root's sum or the velocity dot in the other order 284 (same code); the sine argument selected by an if/else
// into a double 284; float negation of the sine argument 310 (frame 0x68).
// Float dx/dy locals for the distance on (a)+(b)+(c) 426 (1746 B, frame differs, caller 362), alone 432.
// On the old body: the emitter's angle through a pointer or reference 325 (1736 B; first emission then reads the
// element from memory at each use as retail does, but the wrapped angle loses XMM7), the element by value with
// retail's store order 321; pointer forms without the `(double)` sine casts 373 (frame 0x68, SSE negation);
// y and x read into locals after the angle store with the x row through the pointer 378 (frame 0x6c, all x87), all
// through the locals 445; arms calling a two-real setter with `sin()`/`cos()` written in the rows 411 (1789 B, arms
// not merged), with temporaries 360, with the ternary in both rows 411 (frame 0x68); sine by if/else or ternary and one
// setter call 360; staged y with `cos()` written twice and the sine's angle in a double or float 306/360.
// Left on the 310 plain form: the unrounded distance compare without a double local, the near arm's `y * cos`
// (retail loads y and multiplies by the cosine's slot, ours loads the cosine), the first emission's angle read from
// the element at each use, the wrapped angle's slot (+0x14 in retail) and its per-arm x87 loads.
// Unity still uses XMM1 instead of XMM2. The fixed-turn cosine spill and candidate register still differ.
// Fixed x accumulator 306, separate products 306, wide fixed x 306.
// Distant x accumulator 305, distant y 320, both distant rows 320, all x rows 306, all rows 321.
// NOT kept: The distant x form ties. The other forms raise the score or leave the fixed-turn spill unchanged.
// M3 lane 16 r154 (fast with 0x29a190 and 0x298c30, both MATCH throughout; control 305; plain where said; nothing kept):
// the near arm's y row staged as `turned_y = arg_3->y; turned_y *= local_11;` 305 (same code); the side flag declared
// ahead of the distance 305 (same code); the facing x and y read into two locals at the near arm's top 387 (+0x2, 1740 B).
// Why unity takes XMM1: retail keeps 1.0f in XMM2 from the entry normalizer through the near arm's field_8 stores to the
// far arm's ratio clamp, because retail's near arm forms `y * cos` in XMM1; ours forms it on x87, so XMM1 is free for
// unity. On lane 1 r129's 310 lead (re-measured 310 fast): the loop distance with named `double dx, dy` (each used twice)
// 300 fast but 346 plain (frame padded: a double local); the same through a file-local inline helper with double locals
// 300 fast; through an inline helper taking the two differences as double parameters 310 (dy computed first), with the
// parameters (dy, dx) so that dx is computed first as retail 300 fast and 300 plain (1716 B, frame 0x64: a double
// parameter does not pad the frame). Left on that form: the root is still stored and reloaded at the loop head (retail
// `fstp st(2); fstp st(0); fst`), the near arm's cosine/sine slots (+0x18/+0x14 against retail's +0x28/+0x1c) and its
// `cos * y` operand order, and the far arm's normalizer, dot order, wrapped-angle compare and per-arm loads; the helper
// and the lead's forms need a match, so nothing is kept (build/r154/lead-298d90-plain300.patch in m3-lane16). On main's
// body the named double differences measure 316 (1740 B), as the note already records.
// shared scale with native admission 305, reciprocal at shared scale call 305, shared magnitude with native
// admission 305, shared magnitude and scale 305, shared facing dot 396, shared tangent dot 305, typed
// velocity projection 305, shared planar projections 396.
// The unity register, near cosine spill and candidate ordinal remain. All eight previews retain retail
// length.
// M3 lane 5 r216 (fast screen, plain where noted; 0x29a190 MATCH unless noted): the 2-D normalize helper with an early
// `return 0.0f` 305, in 0x2b400's shape (`x = x * inv; y = inv * y;`) 305, with the reciprocal first in both products 305,
// with both components divided by the length 305 (all the same code); the arg_0 read through its address removed 316
// (1726 B; plain agrees, 0x29a190 MATCH -> 345).
// M3 lane 15 r122 (fast with 0x29a190 and 0x298c30, both MATCH; control 305): lane 16 r154's lead rebuilt 300 (1716 B,
// first difference +0x194; left: the loop head stores the root and reloads it for the radius compare where retail keeps
// it on the x87 stack with `fst`, the near arm's cosine slot (+0x18 against retail's +0x28, the outer counter's spill
// slot) with `cos * y` where retail loads y and multiplies by the cosine's slot, and the second emitter's argument setup).
// On that lead: the near y row written `y * cos + x * sin` or `sin * x + cos * y` 300/300; the distance helper returning
// double with the cast at the call 300; the side flag computed ahead of the distance 300; the radius test written
// `field_24 > distance` 300 (all the same code). Body unchanged.
// M3 lane 4 r236 (fast with 0x298c30/0x29a190; control 305; the return is `ret 0x1c` in both builds): the negated radius
// written at its two uses instead of the named local 418 (1708 B); the distance's two differences named as double locals
// 316 (1740 B); the distance's y term ahead of its x term 305 (same code: retail subtracts x first, ours y first); the
// differences as a point2f local then sqrt of their squares 431 (1775 B); that through magnitude2d 431. The first
// difference is still the 1.0f/radius pair (ours 1.0f XMM1 and the radius XMM2, retail the reverse).
// M3 lane 9 r146 (fast with 0x29a190/0x298c30, both MATCH unless noted; base 305): the facing x and y read into two
// locals in each near emission 423 (1772 B); the facing point copied once at entry and read everywhere 435 (1728 B,
// 0x29a190 351); the near rotated point built in a named point2f and stored as one aggregate 375 (1725 B); the near side
// and hit flags stored right after the angle, ahead of the rows, 313; only the facing y named in each near emission 413
// (1772 B). All restored.
// M3 lane 12 r165 (fast screen with 0x29a190 and 0x298c30, both MATCH throughout; control 305, aligned 171/167 = unmatched
// instructions ours/retail in an LCS-aligned diff; "lead" = lane 1 r129's 310 form rebuilt on today's file, 310 at 1716 B,
// aligned 72/76; body unchanged, nothing kept). The far arm's difference vector declared at function scope, at the loop
// body's top, or declared in place and assigned by member 305 each (same code), on the lead at function or loop scope 310
// (same code): retail's second normalization stores x before it loads y (`movss [x], xmm2; movss xmm2, [y]; mulss xmm2,
// xmm0`), ours forms both products first at either scope. The distance assigned inside its own test (`if ((d = (real)sqrt(
// ...)) < field_24)`, with and without the cast; a diagnostic) 305 each, on the lead 310 each (same code: the root is
// still popped to its home ahead of the two dead differences and reloaded, where retail drops the differences with
// `fstp st(2); fstp st(0)` and keeps the root on the x87 stack behind one `fst`). On the lead, the distance through a
// file-local inline taking the differences as double parameters (dy, dx) with a real result, with a double result, or
// taking the two points with double locals, each as its own statement and assigned inside the test: 300 each (six forms,
// one code; aligned 67/71, lane 16 r154's count): the store and reload at the loop head stay in all of them. 1 to 8
// initialised unused locals (diagnostic): on this body 305 each (same code), on the lead 310/312/310/310/310/315/315/315
// (aligned 72/74/72/72/72/70/70/70; 6 to 8 locals shorten it to 1714 B). The lead's remaining blocks by the aligned
// diff: the loop head's subtraction order and root store, the near rows' `y * cos` operand sides and cosine/sine slots
// (+0x18/+0x14 for retail's +0x28/+0x1c), the second normalization's store order, the tangent dot's term order, the
// second side's arm order, the wrapped angle's compare and slot, and the emitter reading the angle element at each use.
// M3 lane 4 r241 (H2M_FAST with 0x298c30/0x29a190; control 305, aligned 175): retail's emission counter is a 16-bit BP
// (`movsx eax, bp`, the word store from BP), the outer counter BX and arg_3 EDI loaded after `push edi`. `short local_6` 469
// (1686 B, `ret 0x18`, arg_3 in EBX; 0x29a190 684); with a dead read of arg_3 through its address (the arg_0 idiom) after the
// arg_0 read 410 (1706 B, `ret 0x1c`, `movsx eax, bp` as retail, but arg_3 EBX and the outer counter DI; 0x29a190 127), ahead
// of the arg_0 read 410, the address read used in the side test 410. Body unchanged.
// Facing products staged across negative x, the negative endpoint or positive x, with real
// or double products, 316 in all six forms. Full facing test after negative x or after the
// negative endpoint 332/336. The 0x64 frame agrees and unity still uses XMM1 in the staged forms.
// M3 lane 15 r128 (H2M_FAST with 0x29a190 and 0x298c30, both MATCH; control 305, `ret 0x1c` in both): the radius test on a
// second unrounded `sqrt` of the same sum 411 (1756 B, the roots are not merged); the squared sum named as a `double` local
// 305 (same code); the near rows' y staged in the output element (`field_c.y = y * cos; field_c.y += x * sin;`) 392 (1745 B);
// separate cosine/sine/turned-y locals for the second near emission 305 (same code); the root summed y-first with
// `field_24 > distance` 305 (same code). Body unchanged.
// M3 lane 14 r141 (H2M_FAST with 0x29a190 and 0x298c30, both MATCH throughout; control 305, aligned 257; aligned = LCS-aligned
// differing instructions). Lane 16 r154's 300 lead rebuilt 300 (1716 B, aligned 98). LEAD, match-only, 229 (1732 B, aligned 59;
// build/r141/lead-298d90-229.patch in m3-lane14): on that lead the second side's ternary as `!local_4 ? 2pi - x : x`, the wrapped
// angle as `x >= 0.0f ? x - 2pi : x + 2pi` with the array element read at each use (no element local) 247 (1732 B, aligned 65);
// the emitter's angle taken by `real const &` 251 (1728 B, aligned 61: the first emission then reads the element from memory at
// each use and copies it through ECX as retail); the sine argument as `(double)-x : (double)x` 229 (aligned 59: retail's `fld`
// in each arm, where `-(double)x` hoists one load ahead of the branch). Also on 251: the sine as `-x : x` in float 309 (+0x2,
// 1747 B), an if/else with a sine call in each arm 251 (same code), the rotated y row's terms swapped 251 (same code). On 229:
// the far arm's trig pair renamed (sine/cosine) or the names swapped 229 (same code); cosine declared ahead of the sine 229;
// the rotated y row stored ahead of the x row 166 (positional only: aligned 70, retail stores x first); the rows' products
// written point-first 229; the sine, the cosine or both kept as double 242/240/246; the tangent dot, the emitter's facing dot or
// both through the existing dot_product2d 229 (same code). Left on 229: the loop head stores the root and reloads it (retail
// `fstp st(2); fstp st(0); fst`), the near arm's `cos * y` (retail `y * cos`, the cosine in +0x28 and the sine in +0x1c; ours
// +0x18/+0x14), the distance slot (+0x14 against +0x1c) and the wrapped angle's (+0x1c against +0x14), the second
// normalization (retail stores x before it forms `y * inv`), the tangent dot and the rotated y row taking the x product first
// where retail takes the y product first (every source order gives the same code), the emitter copy's x/y registers and its
// field_c stores after the first push.
// M3 lane 4 r247 (fast, 0x29a190 listed): the candidate count as short re-measured 469 (1686 B, 0x29a190 684), with the inner
// endpoint counter short 469, long 492; the read of arg_0 through its address removed 316 (0x29a190 345); `-arg_6` written at
// both uses 418 (1708 B); the count as the global itself with that read removed 476 (0x29a190 702); the near arm's two
// quarter-turn candidates through one forced-inline helper called twice 313 (1732 B, y times cosine still on x87), with a short
// count 469. Body unchanged.
// M3 lane 2 r134 (H2M_FAST with 0x29a190 and 0x298c30, both MATCH throughout; main 305): lane 14 r141's 229 lead rebuilt
// 229 (1734 B, first +0x194). On it, match-only: the distance helper returning double, held in a double local that the
// radius test compares, with local_9 its float copy, 249 fast (1722 B): the loop head is then retail's (`fstp st(2); fstp
// st(0); fst`, the root compared on the x87 stack) and the code is retail's length through +0x320, but the far arm's
// second normalization is 4 bytes short (retail stores x before it forms `y * inv`) and the inner loop's alignment pad
// goes; the double result cast at the call with no double local 229 (same code); on the 249 form one to three redundant
// paren pairs around the near rows' `y * cos` 249 each. Body unchanged.
// M3 lane 14 r147 (H2M_FAST with 0x29a190 and 0x298c30, both MATCH throughout; control 305): the normalizer as two
// divisions by the length 305 (same code: one reciprocal); its products as `x * inv` and `inv * y` 305; its reciprocal
// declared bare at the helper's top 305; the far arm's difference point set member by member 305 (all four the same
// code); that point copied from the sum and decremented in place 368 (1745 B). All restored.
// M3 lane 16 r180 (H2M_FAST with 0x29a190, MATCH throughout; main 305; all on lane 2 r134's p1 lead rebuilt, 249 at 1722 B,
// aligned 59): the shared normalizer's products as `y = inv * y`, both `inv *` or both `* inv` 249 each (same code: the far
// arm still forms both products before its stores); the near rows' `cos * y` or y term first in all four arms 249 (same
// code); the far difference through the existing vector2d_from_points2d 160 (1734 B, aligned 61: the inner loop's pad comes
// back with the code address, but the y difference is stored and reloaded and the normalization still differs); the far
// arm through the existing normalize2d_fast (cast or a vector2f local) 249 (same code); the float distance copy declared
// ahead of the double root 249 (same code). Body unchanged.
// M3 lane 4 r254 (H2M_FAST with 0x29a190 and 0x298c30, MATCH throughout; control 305, aligned 212 with stack offsets
// masked): the emitter facing dot written `arg_3->x * f.x + f.y * arg_3->y` 305 (same code); the side test against
// `0.0f` 361 (1740 B); both clamps against float constants 398 (1685 B); the first clamp as `!(local_13 <= 1.0)` 317
// (1727 B); the outer counter typed long 367; the endpoint pointer typed `point2f const *` 305 (same code); the radius
// test `field_24 > local_9` re-measured 305. Body unchanged.
// M3 lane 15 r141 (fast screen with 0x29a190, MATCH each time; control 305, aligned 271 rows, 57 of them order rows). The
// first difference read from the allocator's ranking: in the entry block the unity constant is one candidate living to
// the loop's end with six uses (the first normalisation, the near arm's two stores, the first clamp's copy, compare and
// store) and scores 38; the first normalisation's scaled component scores 24 and the two would tie in its favour.
// Retail colours the scaled component (and after it the turn radius) ahead of unity, XMM1 then XMM2; ours colours unity
// first. With the same six uses retail's unity must lose 14, or the entry block must reference eight float candidates
// where ours references four (the component's score is six per candidate there, unity's two). Forms, all with the
// entry otherwise unchanged: the negated radius declared ahead of the first normalisation 342, ahead of the direction
// copy 342, ahead of the centre 343, first in the function 343 (first difference +0x4b); the direction copy and its
// normalisation ahead of the centre 359 (first +0x7), with the negated radius first as well 366. Body unchanged.
// Unity stays in XMM1 and the near cosine product stays on x87 in the tied forms. Distance narrowed only in
// the distant arm 384, narrowed at each distant use 384, named wide differences with distant narrowing 380,
// named wide squares with distant narrowing 384. Fixed-turn cosine, sine, both names, and both with y renamed
// 305 each. NOT kept: raw-root comparisons grow the frame and require a byte match at the boundary.
// M3 lane 4 r260 (h2m fast with 0x29a190 and 0x298c30, both MATCH each run; control 305, aligned 171; body unchanged; all
// on lane 2 r134's p1 lead, rebuilt 249 at 1722 B, aligned 47 by this round's count). Diagnostic: 1 to 7 unused longs at
// the top of p1 give aligned 47, 49, 47, 34, 34, 46, 46 (positional 249 to 252): four or five more symbols put the
// emitter's copies and dot in retail's registers and order, so that group is a symbol-count phase. Left on p1 then:
// (1) the frame's 4-byte cells: retail's first cell holds the wrapped angle, its third the root and the sine, and the
// cosine joins a later cell; retail's sine and cosine cells have three references per quarter turn (one store, one SSE
// product, one x87 load), p1's duplicated rows give six and five, which puts them first; (2) the near arm: retail
// loads y, stores the cosine and multiplies y by the stored cosine, loads x, and only then branches for the sine's
// argument, so the y row holds the sine's evaluation and the x row reuses its stored value; (3) the far arm's second
// normalization (retail loads x and multiplies by the reciprocal, stores it, then forms y) and the inner loop's pad
// that follows from its four bytes. Forms on p1's near arm, none lower: both trig calls written inside the y row and
// again in the x row 446 (1739 B, aligned 234: no `push ebx`, the whole colouring moves); the cosine named and the
// sine written in both rows 443; a file-local inline taking (in, sine, cosine, out) with the calls as arguments 450
// (1709 B); a file-local inline taking the side and the angle with the calls in both rows 440 (1728 B); main's split y
// row with the sine assigned by an if/else 418 (1738 B), by the ternary 464 (1693 B). In each the unity constant goes
// back to XMM1: p1's duplicated arms are what lower its rank (four more blocks where it is live and unread).
// M3 lane 2 r149 (H2M_FAST with 0x29a190 and 0x298c30, MATCH each run; control 305, aligned 175, masked 93). Allocator
// reading of the first difference: unity (six references, priority 38) is placed ahead of the first normalisation's
// scaled x (24) and takes XMM1; a forced XMM2 for unity alone gives 293 with the first difference at +0x147. Forms: the
// direction copy as `point2f local_2 = { arg_8->x, arg_8->y };` 391 (1722 B, aligned 170, masked 100): the entry block
// is then retail's row for row (unity XMM2, the scaled x XMM1), but the facing dot loads y first and the near arm
// shortens; the same with the dot written y first 391, with the copy as two member assignments 391 (same code each);
// lane 2 r134's p1 lead rebuilt 249 (1722 B), with the member copy 412 (1706 B); local_1 as two member assignments 305,
// the negated radius as `0.0f - arg_6` 305 (same code each).
// Planar scaler for the cosine products on positive/negative/distant turns
// 356/352/360, positive-negative/positive-distant/negative-distant 370/403/399,
// all 445, cosine and sine on all 478. NOT kept: each grows the 0x64 frame and
// regresses. Unity, rounded cosine and candidate ordinal still differ.
// M3 lane GB r11 (fast with 0x29a190 and 0x298c30 MATCH, then plain-confirmed with 0x29a190/0x298c30 MATCH and 0x299460 569;
// control 305, 36 trials): lead 305 -> 256, held as a patch (not kept: a new one-caller helper, store and condition order and rereads are kept only with a byte match; the counter and sine forms alone give 460) (1738/1732 B, frame 0x64, first difference +0x57 -> +0x163, aligned 175 -> 51,
// register-masked 479 -> 57): alias-safe `rotate_vector2d_298d90` (y staged before x/y stores, called in both near `if/else`
// arms and the far arm, replacing staged `turned_y`), direct `g_504938` counter, retail `function_2992ba` store order
// (field_4, field_0, field_1, field_c), `!local_4` second-side branch, `>= 0.0f` wrapped angle reading `local_19[local_20]`
// directly, and `(real)(local_7 == 0 ? sin(-local_22) : sin(local_22))`. Not kept (fast): clean far+emit alone 302 (+vec2d 347),
// clean far alone 317 (+vec2d 362), global count with staged `turned_y` 457 (+vec2d 440, caller 684), near `if/else` without
// staged y 400 (1674 B, caller MATCH), sine-only `if/else` 379 (caller 684), whole-near `if/else` 451 (caller 684), `rotate_vector2d`
// with ternary sine 432 (caller 684), inline `cos()` args 363, with `byte local_6` 373, `emit_near_298d90` helper 257 (+vec2d 333),
// x-first rotate helper 328, (cos, sin) param order 332, emitter member stores 435 (caller 684), emitter `real const *arg_2` 253
// (1734 B, al 36; pointer to local, match-only), kept `local_21` 290, old `-(double)local_22` 272 (1732 B), old emitter order 267,
// in-place rotate helper 555, diagnostic double distance local 284 (224 with vec2d).
// M3 lane 16 r190 (fast screen with 0x29a190 and 0x298c30, both MATCH each run; control 305, aligned 260; body unchanged).
// The near arm's y row only. Retail keeps each quarter turn's cosine and sine in float cells read by both an SSE
// product and an x87 load, forms y times cosine in SSE and loads x before it branches for the sine's argument, and
// keeps that product in XMM1 across the branch; main's staged `turned_y = y * cosine` is an x87 product on the cosine
// still on the stack. Forms: the staged y as one sum after the sine, `x * sine + y * cosine`, 365 (1745 B, aligned
// 253: both cells and both SSE products as retail, but the y product comes after the sine and is formed cosine
// first), written `y * cosine + x * sine` 365 (same code), with the sine assigned inside the sum 365 (same code: the
// sine's branch is still evaluated ahead of the whole sum); the sine written inside the staged sum and again in
// the x row 389 (1756 B, aligned 273); the staged product kept and the later sum written `x * sine + turned_y`
// 305 (same code as the body); no staging, the x row then the y row as plain sums after the sine 382 (1692 B,
// aligned 242). So the SSE y product follows from the y row being one sum; what moves it ahead of the sine's
// branch is not found here (an identical product in both arms of a side test, hoisted, would give it).
// M3 lane 16 r191 (h2m fast with 0x29a190 and 0x298c30, both MATCH unless noted; control 305, aligned 257, masked 123).
// Ranking read for the first difference: the 1.0f constant is one float candidate live over the whole body (priority
// 38: the normalization block plus six uses in the outer loop at weight 2), so it is coloured before the normalization's
// product copy (24) and takes XMM1; retail has the copy in XMM1 and the constant in XMM2. Retail also keeps the candidate
// count a short in BP (`movsx`, `inc bp`) and the outer counter in BX, and compares the near-test distance with the
// unrounded sqrt still on the x87 stack (`fstp st(2); fstp st(0); fst`) where this build stores, pops and reloads it.
// Forms: the entry's read of arg_0 through its address removed 316 (1726 B, 0x29a190 345); the 0x88 pair read through
// the object cast at each use instead of the pointer local 307; both normalizations through the header's existing
// normalize2d_fast 305 (same code); the facing test's dot through the existing dot_product2d 396; with the far arm's
// dot through it too 396.
// M3 lane 4 r264 (h2m fast with 0x29a190 and 0x298c30, both MATCH each run; control 305; body unchanged). Ranking read:
// unity (38) is coloured ahead of the first normalisation's scaled x (24); the near arm's two scale stores give unity
// 8 each (loop weight 2, two float candidates in each block), and without them (wrong code, diagnostic) unity drops
// under the scaled x and the entry is retail's to +0x147 (348 at 1713 B). Forms: each quarter turn's stores through
// one entry pointer 342 (1693 B); the scale stored as a double literal 305 (same code); the scale store right after
// the angle store 313; the side, flag, scale and ordinal stores together after the angle store 313; the scale store
// after the ordinal store 305 (same code).
// M3 lane 1 r175 (fast screen with 0x29a190 and 0x298c30, both MATCH each run; control 305; body unchanged). Near arm, aimed at
// retail's float cosine cell and SSE `y * cos` ahead of the sine's branch: the cosine read through a `real const &` in the
// rows 305 and each quarter turn's cosine call bound to its own `real const &` 305 (both the same code: the references fold);
// the staged y declared double 378 (1697 B); the angle store ahead of the cosine 305 (same code); each `y * cos` bound to a
// `real const &` temporary, the y row then `x * sin + that`, 305 (only the staged product's home moves; it stays x87).
// M3 lane 11 r176 (fast screen with 0x29a190 and 0x298c30, both MATCH; control 305): the facing y read into a local just
// ahead of the first quarter turn's cosine and used for that entry's y product 349 (1741 B), but the entry then is
// retail's (unity in XMM2, first difference +0x57 -> +0x147; lead only, a one-use local); the same copy after the
// cosine 358 (1740 B); the facing y copy at every near-arm y read 392 (1756 B), with the candidate count as a short
// 377 (1714 B, 0x29a190 still MATCH here), the facing x and y both copied 387; the candidate count as a long 376
// (1698 B); the outer counter as a long 367; the negated radius written at each use 418 (1708 B). Body unchanged.
// M3 lane 1 r179 (h2m fast with 0x29a190 and 0x298c30, both MATCH unless noted; control 305). Ranking read: unity's 38 is
// +8 for the normalisation block, +8 for each near-arm scale store (two blocks), +8 for the far arm's `local_13 = 1.0f`,
// +8 and +4 for the arccosine clamp's compare and store, less 6 for two live-only blocks; the first normalisation's
// product copy has 24 and would win a tie (it is the later candidate). A forced-ranking search putting unity at 23 gives
// 293 with the entry exact to +0x147 (only a priority change on unity moves it; no range cut on it does). Retail's near
// arm stores each cosine to a float cell at once and forms `y * cos` in SSE ahead of the sine's branch (facing y and x
// loaded before it), ours keeps the cosine on the x87 stack. Forms: both quarter turns through one forceinline helper
// (two call sites) 364 (1745 B: the y row moves as in the 365 forms above), as a static inline 364; the cosines and sines
// through float-returning wrappers 336 (frame 0x68, the near arm unchanged; 0x29a190 45), near and far arms alike 336, as
// plain inline 336, the cosines alone 305 (same code). Body unchanged.
// @retail 0x298d90
void function_298d90(long arg_0, long arg_1, s_298c30 const *arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, real arg_6, point2f const *arg_7, point2f const *arg_8)
{
	long const *const local_28 = &arg_0;
	long local_29 = *local_28;
	(void)local_29;
	s_slot_object_view *local_0 = object_get(arg_1);
	point2f local_1 = { arg_7->x + arg_8->x, arg_7->y + arg_8->y };
	point2f local_2 = *arg_8;
	function_298df3(&local_2);
	point2f local_27;
	point2f local_3[2];
	real local_30 = -arg_6;
	local_3[0].x = arg_4->x * local_30 + arg_7->x;
	local_3[0].y = arg_4->y * local_30 + arg_7->y;
	local_3[1].x = arg_4->x * arg_6 + arg_7->x;
	local_3[1].y = arg_4->y * arg_6 + arg_7->y;
	bool local_4 = local_2.x * arg_3->x + local_2.y * arg_3->y >= 0.0;
	point2f const *local_31 = (point2f const *)((byte *)local_0 + 0x88);
	real local_5 = local_31->y * arg_3->y + arg_3->x * local_31->x;
	byte local_6 = 0;
	for (short local_7 = 0; local_7 < 2; local_7++)
	{
		point2f *local_8 = &local_3[local_7];
		real local_9 = (real)sqrt(((double)local_1.x - local_8->x) * ((double)local_1.x - local_8->x) + ((double)local_1.y - local_8->y) * ((double)local_1.y - local_8->y));
		bool local_10 = local_7 == 1;
		if (local_9 < arg_2->field_24)
		{
			real local_11 = (real)cos(1.5707963705062866);
			g_5047f8[local_6].field_4 = 1.5707963705062866f;
			real turned_y = arg_3->y * local_11;
			real local_12 = (real)sin(local_7 == 0 ? -1.5707963705062866 : 1.5707963705062866);
			turned_y += arg_3->x * local_12;
			g_5047f8[local_6].field_c.x = local_11 * arg_3->x - local_12 * arg_3->y;
			g_5047f8[local_6].field_c.y = turned_y;
			g_5047f8[local_6].field_0 = local_10;
			g_5047f8[local_6].field_1 = false;
			g_5047f8[local_6].field_8 = 1.0f;
			g_5047f8[local_6].field_18 = local_6;
			local_6++;
			local_11 = (real)cos(-1.5707963705062866);
			g_5047f8[local_6].field_4 = -1.5707963705062866f;
			turned_y = arg_3->y * local_11;
			local_12 = (real)sin(local_7 == 0 ? 1.5707963705062866 : -1.5707963705062866);
			turned_y += arg_3->x * local_12;
			g_5047f8[local_6].field_c.x = local_11 * arg_3->x - local_12 * arg_3->y;
			g_5047f8[local_6].field_c.y = turned_y;
			g_5047f8[local_6].field_0 = local_10;
			g_5047f8[local_6].field_1 = false;
			g_5047f8[local_6].field_8 = 1.0f;
			g_5047f8[local_6].field_18 = local_6;
			local_6++;
		}
		else
		{
			real local_13 = arg_6 / local_9;
			if (local_13 > 1.0)
				local_13 = 1.0f;
			real local_14 = function_50650(local_13);
			point2f local_15 = { local_1.x - local_8->x, local_1.y - local_8->y };
			function_298df3(&local_15);
			real local_16 = local_15.y * arg_4->y + local_15.x * arg_4->x;
			if (local_16 > 1.0)
				local_16 = 1.0f;
			else if (local_16 < -1.0)
				local_16 = -1.0f;
			real local_17 = function_50650(local_16);
			real local_18;
			if (local_7 != 0)
				local_18 = local_4 ? 3.1415927410125732f - local_17 : local_17 + 3.1415927410125732f;
			else
				local_18 = local_4 ? local_17 : 6.2831854820251465f - local_17;
			real local_19[2];
			local_19[0] = local_18 + local_14;
			if (local_19[0] > 6.2831854820251465f)
				local_19[0] -= 6.2831854820251465f;
			local_19[1] = local_18 - local_14;
			for (byte local_20 = 0; local_20 < 2; local_20++)
			{
				real local_21 = local_19[local_20];
				real local_22 = local_21 < 0.0f ? local_21 + 6.2831854820251465f : local_21 - 6.2831854820251465f;
				real local_23 = (real)sin(local_7 == 0 ? -(double)local_22 : (double)local_22);
				real local_24 = (real)cos(local_22);
				point2f local_32;
				local_32.x = local_24 * arg_3->x - local_23 * arg_3->y;
				local_32.y = local_24 * arg_3->y + local_23 * arg_3->x;
				function_2992ba(local_6, local_10, local_21, &local_32, &local_2, arg_1, arg_2, local_9, local_5, arg_5, &local_27);
				local_6++;
				function_2992ba(local_6, local_10, local_22, &local_32, &local_2, arg_1, arg_2, local_9, local_5, arg_5, &local_27);
				local_6++;
			}
		}
	}
	g_504938 = local_6;
}

struct s_299460
{
	long field_0;
	long field_4;
	point2f field_8;
	real field_10;
};

struct s_29946c
{
	short field_0;
	short field_2;
	byte field_4[8];
	s_299460 field_c[1];
};

short function_2108a0(long arg_0);

__forceinline s_pathfinding_data *function_29962f()
{
	s_pathfinding_data *local_0 = NULL;
	if (*(long *)((byte *)g_4e0348 + 0xc4) > 0)
		local_0 = *(s_pathfinding_data **)((byte *)g_4e0348 + 0xc8);
	return local_0;
}

__forceinline real function_299a36(point3f const *arg_0, point3f const *arg_1, vector3f *arg_2)
{
	arg_2->i = arg_1->x - arg_0->x;
	arg_2->j = arg_1->y - arg_0->y;
	arg_2->k = arg_1->z - arg_0->z;
	real local_0 = (real)sqrt(arg_2->j * arg_2->j + (arg_2->i * arg_2->i + arg_2->k * arg_2->k));
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		vector3f_scale(arg_2, local_1, arg_2);
	}
	else
		local_0 = 0.0f;
	return local_0;
}

__forceinline real function_299ae9(point3f const *arg_0, point3f const *arg_1)
{
	real local_0 = arg_0->x - arg_1->x;
	real local_1 = arg_0->y - arg_1->y;
	real local_2 = arg_0->z - arg_1->z;
	return (real)sqrt(local_0 * local_0 + local_1 * local_1 + local_2 * local_2);
}

__forceinline void function_29991b(point2f const *arg_0, real arg_1, real arg_2,
	point3f const *arg_3, real arg_4, point3f *arg_5)
{
	arg_5->x = arg_0->x * arg_2 - arg_0->y * arg_1 + arg_3->x;
	arg_5->y = arg_0->y * arg_2 + arg_0->x * arg_1 + arg_3->y;
	arg_5->z = arg_4;
}

__forceinline void function_2994a1(point2f const *arg_0, real arg_1,
	point2f const *arg_2, point2f *arg_3)
{
	arg_3->x = arg_1 * arg_2->x + arg_0->x;
	arg_3->y = arg_1 * arg_2->y + arg_0->y;
}

// score 569 (was 569): BLOCKED-BY 0x2108a0 (the radius is held in XMM5 from the entry because this build's link-time
// summary of 0x2108a0 says it keeps the XMM registers; retail reloads the radius in each block; the lead below needs
// `inline` on that callee's definition). M3 lane 7 r208 (H2M_FAST with 0x29a190, MATCH throughout; control 569; all
// restored): the first pivot offset as one call with `arg_1->field_0 ? arg_6 : -arg_6` 886 (3286 B); the negated
// radius named in the first pivot offset's false arm 569 (same code); the radius parameter declared `real const` 569
// (same code); the negated radius named in the second pivot offset's false arm 732 (3349 B), in both false arms 732.
// MiniMax R (11-20), NOT kept (coordinator review: an unread output buffer and a moved statement are match-only): 561
// (was 569): the forward trace writes to its own vector, which gives the pivot retail's
// +0x94 home, and the sector reads moved after both trace calls, which gives the trace results retail's homes. Still
// open: the radius is promoted into XMM5 at the entry where retail reloads it from its home in each block.
// MiniMax G (11-19): no new form, no lower score (569): the radius is still promoted into XMM5 at the entry and kept
// for the whole body where retail loads it from its home in each block, and that follows from this build's link-time
// register summary of 0x2108a0 and 0x210770, whose definitions are in another lane's file, so the radius, the pivot
// home (+0xa0 against retail's +0x94) and the sweep arms' XMM registers stay open: BLOCKED-BY. Body unchanged.
// MiniMax G (11-19): no new form, no lower score (569): the pivot offset's negative arm, the pivot home, the sweep
// arms' XMM registers and the three rotations in the loop are still open, and the forms tried on the scale
// expressions, the direction's components and the named scales all score the same or worse. Body unchanged.
// MiniMax R (11-16): no new form, no lower score (569): the body is still 39 bytes shorter than retail's, the three trace results still take the homes +0xf4 and +0xd0 against retail's +0xd0 and +0xf4, and the radius still comes from one read where retail re-reads the parameter nine times.
// Earlier 569 (was 569): m3 lane 16 r120 (fast control 569, 0x29a190 MATCH; all restored): the three trace results declared
// at first use (the third inside the sweep loop) and declared in the order local_13, local_12, local_14: same code (their
// +0xf4/+0xd0 homes stay swapped against retail's +0xd0/+0xf4). Diagnostic, reverted at once: the radius read through a
// volatile view at all 14 uses (retail reloads it from its argument slot 9 times) 786 at 3349 B, so the XMM5 radius is not
// the only blocker.
// MiniMax D (11-14): no new form, no lower score (569): the body is still 39 bytes shorter than retail's with the frame
// at 0x114 as retail, and the radius and the trace and pivot homes still differ; the forms the earlier rounds measured
// (the shared endpoint setters, the centre copy, the obstacle-radius observation and the trace-field sector forms) all
// measure 767 or more, so the body stands.
// Earlier 569 (was 569): earlier 621; 629, 733, 633 and 719 before that (the rise to 733 was the unrounded sine/cosine below); frame 0x114 as retail, 39 bytes short; radius and trace/pivot homes differ.
// MiniMax R (11-15, later): no new form, no lower score (569): the permuter over the full image (316 seconds, three
// tries) found no form better than the full image's own 539 differing instructions, so no rewrite of the body scores
// lower from the search and the body stands.
// MiniMax R (11-15): no new form, no lower score (569): the scale is also the multiplier of the sweep points in
// retail's loop (its inlined rotation reads the parameter's home and scales both components, which the shared
// rotation helper here does not), so retail's nine reads of the parameter and this build's one are not the same
// code; the permuter cannot search this function on the reduced image (the reduced image scores 662 against the
// full image's 539), so the body stands.
// MiniMax R (11-15): no new form, no lower score (569): retail loads the scale parameter from its home at each
// use, this build keeps it in xmm5 from the prologue to the last use, so every block after the first pair is
// shifted by four bytes; the frame is 0x114 as retail's and the body is 39 bytes short. The promotion only ends
// if the parameter's last use moves, which changes the code, so the body stands.
// Earlier 569 (was 569): earlier 621; 629, 733, 633 and 719 before that (the rise to 733 was the unrounded sine/cosine below); frame 0x114 as retail, 39 bytes short; radius and trace/pivot homes differ.
// MiniMax S (11-06): Our frame is 0x120 against retail's 0x114, so every esp-relative home is four bytes
// off; retail loads the scale factor into xmm1 after the first branch where ours loads xmm5 before it,
// and the four scale locals and the two trace results still sit at other offsets.
// Earlier 633 (was 719): frame 0x120 versus retail 0x114; radius and trace/pivot homes differ, 15 bytes long.
// Frame 0x11c versus 0x114, early radius retention and trace/pivot/result homes remain.
// Earlier: frame 0x11c versus 0x114; radius caching and trace/point homes differ.
// Shared endpoint setters 767; center CRT copy 767; step CRT copies 767;
// obstacle-radius observation 813; requested-angle observation 796; all restored.
// Trace-field forward sector 772, reverse 776, both 774; settings radius rereads 812; short-index obstacle cursor 814.
// Frame 0x11c versus 0x114 and radius/trace homes remain. Early settings radius 800; const center 767; early selector 847; final-angle selection first 798; reverse component assignments 767 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain.
// native endpoint return 947; native rotated-point return 961; endpoint input order 767;
// two-coordinate pivot 767; side transform first 822 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain. One sector sample 809; cosine first 767; signed step branches 848; normalization result 839; maximum rejection guard 767 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain. Absent-obstacle exit 768; positive center clearance 820; direct edge distances 916; early annular bounds 767; reverse-vector component loop 839 (fast).
// Frame 0x11c versus 0x114 and radius/trace homes remain. Selected obstacle pointer 802;
// forward state on hit 771; reverse state on hit 771; both states on hit 784; active edge updates 857;
// in-place initial endpoint 850 (fast).
// All parallel sweep scale terms 749; both endpoint distance carriers 791; scales plus distances 755;
// obstacle owner field rereads 767; separate annular failure guards 767; shared coordinate join 875
// (fast). Frame 0x11c versus 0x114 and radius/trace/pivot homes remain.
// Rotation output reference 767; endpoint output reference 767; endpoint input references 767;
// rotation height-source input 767; endpoint pointer result 767; native settings member 767 (fast).
// Original helper contracts and settings storage restored.
// Native rotation/endpoint/direction results 928/910/805; rotation+endpoint 1050;
// rotation+direction 978; endpoint+direction 966; all 1101; result memcpy 1050. All restored.
// Round 155: actual state-field references, then obstacle-element reference: side 749; angle 840; side angle 840; clearance 793; side clearance 793; angle clearance 845; side angle clearance 845; obstacle element reference 749. All restored.
// rotation-products 749; normalization-products 749.
// distance-products 749; rotation-normalization-products 749.
// rotation-distance-products 749; normalization-distance-products 749.
// rotation-normalization-distance-products 749; obstacle-rotation-products 749.
// Round 141: Native point input, vector input and both 749/749/749; direct actor center 815;
// point/vector/both native inputs with direct center 815 each; readonly actor with native inputs/direct
// center 815. Frame 0x11c versus 0x114 and eager radius remain.
// Round 141: Shared transform/index/both/trace/transform-trace/index-trace/all inline and transform
// noinline 749 each (3383 with transform or index; otherwise 3372). The inline forms recover
// branch-time radius loads but frame 0x11c remains. In that context: ordinary/after-actor/after-sector
// result 778/778/814; state names 749; ordinary state names 778; positive sector/common exit/common
// exit with names 762 each. All headers/providers/body restored.
// Round 180 trace-clear states reverse/forward/center/both edges/reverse-center/forward-center/all/byte
// center 802/802/778/754/771/764/782/778 (fast); all restored. Frame 0x11c/0x114, eager radius and
// trace/pivot homes remain.
// Round 185 obstacle-facing result bool/byte/short/long 749/749/775/777; complementary result
// 749/749/775/777 (fast). All restored; frame 0x11c/0x114 and radius/trace/pivot homes remain.
// Rotation publication xzy/yxz/yzx/zxy/zyx 749/749/748/749/748;
// endpoint yx alone/with yxz rotation 811/811; normalized kij publication 749.
// One-point forms rejected; original helper stores and body restored.
// Round 212: Split transformed output 749; planar pivot 755 (frame 0x114, first +0x14);
// producer scopes 749/755; reverse transforms with full/planar pivot 819/829, scoped 819/829.
// On the planar frame: initial/pivot/both radius joins 870/856/905; ordinary result 801;
// later ordinary result 822; ordinary common exit 753; late radius 826; endpoint inline 755.
// Round 174: shared three-use height 779; forward/reverse/both endpoint calculations at consumption
// 803/798/858; center-first/reverse-first initial points 836/802; reverse-first with late forward 792;
// center-first with late reverse 797. Original geometry production restored; frame 0x11c/0x114,
// eager radius and trace/pivot/point homes remain.
// Split-output planar frame with native trace/sample-point/state role-name groups,
// their pairs/all and reversed trace declarations: 755 each (fast control 749). All restored.
// Separate transform/planar pivot plus all sweep-rotation products, with complete
// normalizer-square/distance-square/obstacle-rotation product phases: all eight 755
// (3351 bytes, frame 0x114, first +0x14). Eager radius remains; all restored.
// Unsigned-word edge pair/center/both/facing/edge-facing/center-facing/all results
// 776/779/776/775/775/790/756; all four native-dword results 752 (fast control 749).
// Frame/radius and trace/pivot homes remain; original result widths and body restored.
// Round 382: maximum-selection presence/absence states as bool/byte/short/long
// 783/783/783/783/778/753/778/770; no exact match. Original ordinal and full body restored.
// Round 175: native state/settings/source-point reference formals, all seven subsets,
// 749 each; all five nonnull object/vector input formals 749. Original pointer
// definition, external declaration and both real calls restored.
// Round 393: Native planar pivot with eight coherent existing rotation-producer role orders:
// 755/755/755/755/755/755/755/755. Original pivot, helper contracts and both transform outputs restored.
// Round 396: Native real root APIs in direction/distance/planar normalizers, seven
// subsets and all with both obstacle roots: eight forms 749 each. All original APIs,
// helper bodies and precision restored; frame, eager radius and trace/pivot homes remain.
// Round 240: Split-output planar frame with shared difference/normalization, settings-radius
// rereads and result-first getter, seven subsets 776/841/788/797/784/835/812;
// full graph plus shared planar difference 808. Original helpers/body restored;
// frame/radius and trace/pivot/result homes remain.
// Round 249: Consistent side/whole-angle/both/bool-side+angle observations 781/767/755/813;
// on the split-output planar frame 756/786/805/806. Original reads, frame and helpers restored.
// Round 217: existing in-place normalized vector scale alone/with shared difference 749/749;
// plus reverse scale 742/742 (3374 bytes, seven-point gain rejected); planar pivot with those
// four forms 755/755/769/769. Frame and eager radius remain; all helpers and body restored.
// Shared sine/cosine/both across sweep and obstacle phases at sweep scope 743/743/748;
// at entry 743/743/748; consistent rotation-pair names at both scopes 748/748.
// Six-point isolated gain rejected. Shared sine/cosine plus existing in-place normalization
// and reverse-vector scale 734/733 fast; cosine form kept 733 plain at 3383/3372.
// Original magnitude tree, real trig rounding, trace calls, values and short loops retained.
// Round 313: Separate transform output, original/ordinary result 733/730; with shared difference
// 733/730. Planar pivot 762/761, with shared difference 762/761. Three-count ordinary-result gain
// rejected; frames 0x124/0x118 still differ from 0x114. Original helpers and body restored.
// Round 239: one actual reused trace context, crossed with independent transform
// output, ordinary result and shared sine: 854/854/826/826/850/850/814/814. Frames
// 0x120/0x124/0x128 versus 0x114; original context reads and complete body restored.
// Round 31 (lane 11): the three extra frame dwords are the sweep sine and the shared cosine (retail
// packs them with the trace distances at +0x28/+0x38) and the obstacle offset difference (retail
// writes it into the direction vector's dead home at +0x18/+0x1c). Sine/cosine in a block with
// a separate obstacle cosine 742 (frame 0x11c); obstacle difference written into local_6 763
// (3372 bytes, frame 0x118); both 772, separate cosines without the block plus that 772 (3351
// bytes, frame 0x114, homes then ordered by reference counts); blocked flags declared before
// the result 733; trace-origin names swapped 733 (no home moves); getter as one ternary 734;
// result-first getter 773 (retail's getter code at all five traces); frame-0x114 form plus the
// result-first getter 799. Entry still hoists the step into XMM5 and the NONE exit is inline.
// Round 452: sweep planar vectors 17/18/19 reused for obstacle direction 742 each;
// also for fallback rotation 740 each; fallback rotation alone through 17/18 738 each.
// All remain above 733; original native workspaces, helpers and complete body restored.
// Round 461: complete conditional admission bool/byte/short/long and exhaustive
// branch results 726 each (seven-point nonmatch gains restored). Conditional
// admission plus ordinary function result 719; direct test plus ordinary result
// 719 (plain kept), direct test with original result 726. Retail length 3372
// recovered; frame grows 0x120 to 0x124, radius and trace/pivot homes remain.
// Round 255: independent normalized sweep-direction output at sweep/iteration scope,
// crossed with separate transformed-forward output and separate phase cosines,
// 763/763/763/763/769/769/769/769. Original output ownership and helpers restored;
// frame 0x124/0x114, eager radius and trace/pivot homes remain.
// Round 476: Native indexed endpoints, paired long/short/word/unsigned-long coordinate loops 782/749/749/782; separate endpoint loops 738/819/819/738.
// All frames remain 0x124 versus 0x114; lengths 3356/3372/3373/3403 do not yield a gain. Original sample producers, complete body and helper contracts restored.
// Round 144 (lane 4, fast; counts are positional, with the mnemonic-aligned count in brackets, base 719 [244]):
// retail computes the reverse endpoint with one `0 - distance` value used for both products (ours folds each
// `i * -d + x` into `x - i * d`), and rotates the obstacle direction in x87 from an unrounded angle (`fld; fchs`
// or `fadd dword [pi]`, then `fld st0; fsin; fxch; fcos`, products consuming cosine and sine in place, each
// component stored as a float and reread by SSE). A named negated reverse distance 739 [244] (3387 bytes);
// a double angle with double sine/cosine for the obstacle rotation 850 [232] (3281 bytes, frame 0x118: the
// offset math then stays in x87), with the sweep cosine moved into its loop 850 [232], with the rotation
// stored field by field 850 [232], written into local_18 844 [232]; plus the existing 2-D point helper for
// the circle point and a difference vector 826 [218] (3313 bytes, frame 0x118, offset math in SSE as retail;
// sine/cosine still popped at the end instead of consumed), the difference subtracted in place 826 [218],
// rotation operands swapped 826 [218] (same code). Next: retail shares slots between sweep and obstacle
// locals (+0x14 angle/best index, +0x28 sine/best ratio, +0x44 reverse offset/rotation, +0x18 direction/
// obstacle difference), so its sweep locals end before the obstacle block.
// Lane 18 round 24 (fast; brackets: mnemonic-aligned differences, base [452]): the sweep phase in its own block 719
// [452] (identical code: homes are packed by live range, not scope); single exit `if (sector != NONE) { ...; return
// result; } return true;` 778 [445] (retail's end-placed `mov al, 1` epilogue); result-first pathfinding getter 775
// [443]; both 782 [436]; the trace results declared first 719 (identical). Retail reloads arg_6 after 0x2108a0 and
// 0x210770 where ours keeps it in XMM5, and stores/reloads the result byte at each exit where ours folds it.
// Round 282: complete sweep scope, separate forward output, separate phase cosines and both
// 719/719/716/710; corresponding per-use clearance forms 784/784/802/801. Unscoped output/cosine
// pair 710 plain, restored as a new unused one-use output workspace; existing reverse/side direction
// workspaces with separate cosines 767/717. Three- and two-point forms restored; original frame and body retained.
// Round 246: Complete reused obstacle center/radius/count observations on the ordinary-result
// graph, masks C/R/CR/N/CN/RN/CRN, 750/733/776/770/813/787/828; all with radius
// declared first 828. All restored; frame excess, eager radius and trace/pivot homes remain.
// Round 338: both shared trig workspaces on the ordinary-result/in-place-scale graph,
// with live radius/delegated clamp/shared difference subsets 730/784/774/876/730/784/774/876.
// Original trig homes, radius, clamp, direction helper and full body restored; frame/trace homes remain.
// Round 286: one result exit with bool/byte/short/long carriers 789/818/819/819;
// selected-index-derived obstacle results with those carriers 780/812/808/808.
// Frame 0x120 remains above 0x114; original result topology and whole body restored.
// Round 28 (lane 17, fast; base 719 [452], with registers and slots masked 159): result through a pointer local 719,
// the radius through a pointer local 719, both 719 (same code); the obstacle block as a force-inlined helper, result
// through a pointer 716 (frame 0x120), the same with the outer test kept out 716, returning a bool 820 [439] (3340
// bytes, frame 0x11c); both side-point pairs through one inline 719 (same code), with those two helpers 716/820.
// The entry hoist of the radius into XMM5 stays in every form, a volatile read of it included (772, 726).
// Round 504: shared planar/edge-distance/normalized-sweep differences, seven subsets
// 723/719/719/723/723/719/723; full graph with native sector-result records 723.
// Original helpers, native workspaces and body restored; frame 0x124/0x114 remains.
// M315: current ordinary-result graph with consistent index/transform/both inline contracts,
// without/with complete sweep scope 759 each; both with separate phase cosines, without/with
// full sweep scope 753/753. All body and declaration trials restored; frame/radius/trace homes remain.
// Round 490: Native two-edge trace/state/sector collections, seven subsets
// 763/735/739/780/772/780/738; complete graph with both point-pair collections 739.
// NOT kept: grouped-storage nonmatches; all regress. Original body/helpers restored;
// frame, radius and trace/trig/pivot homes remain.
// M3 lane 13 r54 (fast): one function-scope sine shared by the sweep and the obstacle rotation 730; the third
// trace result scoped to the sweep loop 719; the side endpoints assigned by component 719; the two trace flags
// declared at the top 749; the reversed direction formed after the NONE return 719. Frame 0x124 versus 0x114 remains.
// DH312: Existing direction producer as native coordinate-array loops, long/dword/short/byte
// ordinals, streaming 760/760/746/746; complete difference publication 785/785/828/828.
// Original scalar producer and whole body restored; frame/radius/trace homes remain.
// M3 lane 2 r47 (fast, on 719): the rotated points declared in the loop body 719 and the third trace result at its
// call 719 (same code); the pathfinding getter as a single-exit result (retail's `xor eax, eax; test; jle; mov eax`,
// g_4e0348 held in EBX for both calls) 775 (aligned 452 -> 443); with the body under `if (sector != NONE) { ... return
// local_0; } return true;` (retail's last block is a separate `mov al, 1` epilogue) 782 (aligned 436); that with the
// reverse vector in its own block around its trace 782 (frame stays 0x124). Retail packs the three bools at +0x11..+0x13
// (ours puts the result at +0x3f; renamed `result` on the nested form 782, home unchanged) and has one 12-byte home fewer
// between the pivot (+0x94) and the first trace (+0xb8).
// Round 554: Complete scalar direction-input/rotation-input/squared-distance boundaries,
// seven subsets 784/719/784/719/784/719/784; all with separate phase cosines 783.
// NOT kept: staging nonmatches; original helper inputs, scalar precision and whole body
// restored. Frame 0x124/0x114, eager radius and trace/trig/pivot homes remain.
// DH322: Complete direction deltas in real/double with shared setter, named squared length,
// magnitude before publication and named absolute length: 719/777/719/777/787/806/719/777.
// NOT kept: precision/staging nonmatches; double forms also raise the shared user 389 to 405.
// Original helper/body restored; frame/radius/trace homes remain.
// Round 50 (m3 lane 15, fast): the rotated obstacle offset as two scalars 853 (3285 bytes); the sweep arms
// setting only the three scales, the three point2f built after 868 (3287); the sweep points declared in the
// loop body 719 (same code); the reversed direction negated by component 822 (3355); the obstacle distance
// through the existing magnitude2d 719 (same code). Frame 0x124/0x114 and the cached radius (XMM5) remain.
// DL254: Current ordinary-result graph with byte/short/word/dword edge-center outcomes, then obstacle facing:
// 719/744/744/761/719/787/787/751. Original complete body and helper contracts restored;
// NOT kept: unsupported storage/return/domain nonmatches; native homes remain.
// J248: Completed three-dimensional direction/reverse/pivot workspaces for
// late obstacle deltas, rotated direction or both: 753/734/761/719/763/739/773/747.
// NOT kept: these native-workspace lifetime forms do not match; original
// independent workspaces, scalar observations, helpers and complete body restored.
// P302: Existing sweep-rotation actual offset/pivot references, subsets 1/2/3/5/6/7 then
// ordinary-inline input-pair/full graph: 719/719/719/719/719/719/719/719. Original contracts and whole body restored;
// frame/radius/trace and pivot homes remain.
// P304: Native settings/actor backing members and shared normalizer, masks 1/2/4/3/5/6/7,
// then all with direct actor-position uses: 719/719/756/719/756/756/756/790. Original types, helper and body restored;
// frame 0x124/0x114, radius and trace/pivot homes remain.
// DH332: Standard real sine/cosine overloads across the sweep and obstacle phases,
// seven subsets then native absolute/count overloads: 719/719/719/719/719/719/719/719.
// All canonicalize; original APIs and body restored. Frame/radius/trace homes remain.
// M374: Existing late annular endpoint producer in rotated/direction/forward-offset/
// reverse-offset workspaces, original/shared planar differences: 719/749/719/749/719/749/719/749.
// NOT kept: no native-frame or score gain; original workspaces, all math and body restored.
// DH357: Actual advancing obstacle-entry cursor, short/long/int/word ordinal,
// consumed/explicit head steps: 785/779/779/781/785/779/779/781.
// NOT kept: all regress and lengthen; original traversal and whole body restored.
// Round 134 (m3 lane 8, fast; al = aligned raw/masked/masked+stack, base 666/572/276). The NONE exit moved to the end (retail's far `mov al, 1`
// block) 778 (al -/560/264); the three sweep scales through a pointer-output scale inline, each followed by its products 797 (3355, al -/534/251),
// with the exit moved 800 (3354, al -/520/237); products with each scale written out 813 (3419), with the exit moved 791 (3418). `inline` on the
// definitions of 0x2108a0 and 0x210770 (retail reloads arg_6 after those calls, ours keeps XMM5 across them) 759 (3387, either or both, al 661/571/
// 272), with the exit moved 764, with the scale inline 756 at retail's 3372 bytes (al 674/532/246), with both 756 (3372, al 663/521/235). On that:
// the cosine scoped to each use 759 (frame 0x120); a block around the sweep 759 (same frame: scopes share no homes); a point local for the obstacle
// distance 756; inner return false 756; the sweep endpoints as scalars 778/769 (frame 0x11c, al masked+stack 214/201), without the scoped cosine
// 761, without inline 787, without the moved exit 766. Retail's points and traces take the same space; ours has four more scalar homes, and retail
// reuses the homes of the sweep endpoints for later temporaries.
// DH359: Conditional owner query excluded bool/byte/short/long 719/719/752/750;
// admitted counterparts 738/738/737/735. NOT kept: no gain or byte match;
// original query guards, outcome, geometry, helpers and complete body restored.
// DH361: Sweep/obstacle ordinals and selected-obstacle/maximum-ratio declarations at function
// scope, seven subsets, then the sweep bound too: eight at 719. Original declaration scopes,
// initializers, geometry and body restored; frame 0x124/0x114 and radius/trace/pivot homes remain.
// DH363: Actual count/table record fields, direct/const/mutable receivers with original guard,
// const/mutable conditional, const/mutable/direct default-result getters: 719/719/719/719/719/775/775/775.
// NOT kept: no native-frame or score gain; original getter, helpers and complete body restored.
// DH366: Original annular admission, inside/outside bool/byte/short/long outcomes:
// 719/719/719/719/720/720/720/720. NOT kept: no byte match or qualifying native repair; original complete
// body restored.
// DH368: Standard center/forward/reverse sweep-history transfers, seven subsets then
// the initial center copy too: 768/768/771/767/767/790/789/744. NOT kept: all regress;
// original point transfers, helpers and complete body restored; frame/radius/trace homes remain.
// Round 57 (m3 lane 15, fast; brackets are aligned counts with stack homes and registers masked, base 719 [276]): kept the result-first getter, the sign chosen
// with the product in each arm (retail's two `cvtsi2ss` arms) and one return (`break` plus `if (local_0 && arg_7)`; retail's store-and-reload result byte
// is a tail copy of that return): 633 [240] (3387 bytes, frame 0x120). Steps: getter 775 [255]; NONE exit at the end 778 [264], both 782 [244]; radius through a
// reference to the parameter 719 (same code); sign arms 770 [266], with getter and end exit 788 [232]; `goto` to one return with getter 786 [242], with sign arms 634
// [235] (the NONE return stays, so not kept), NONE through the goto 781 [239]; `break` form alone 723 [266], with getter 786 [242], getter and sign arms with `return` 690
// [245], kept form with the NONE exit at the
// end 793 [227]. On the kept form: normalize3d in the direction helper 636 [237]; dot_product2d at the facing and angle dots 633, 2-D normalizer's length form
// 633 (same code); `inline` on the declarations of 0x2108a0 and 0x210770 in this file 633 (same code). The radius cached in XMM5 across those two calls is the first
// difference; the lead is `inline` on their definitions (earlier 759), and src/unknown_20fe20.cpp was on this round's avoid list.
// w14I (fast, 2026-11-06): statement-by-statement walk against retail found no field, offset, condition, callee, argument or loop/switch-shape error.
// Only numeric precision differs. (1) Obstacle rotation, retail 0x29a041-0x29a085: the angle (fchs / fadd dword pi), the sine and the cosine stay unrounded in
// x87 (fsin, fxch, fcos, then fmul dword [arg_5] with no spill); ours rounded sine and cosine to float. (2) The facing-dot clamp (0x299f11-0x299f50) is
// behaviourally the same as ours (retail clamps the unrounded dot, then rounds; ours compares the unrounded dot too), so it was left alone.
// Any `double` LOCAL in this function (angle, sine, cosine or dot) takes the matched caller 0x29a190 from MATCH to 45 (frame 0x588 versus 0x580)
// while the same maths without a double local keeps it: double locals 664-721 (3293-3373 bytes, frame 0x114/0x11c), all rejected. Kept: sine and
// cosine called inside the expression with (double) operands, no double local: 733 (3291 bytes, frame 0x114), 0x29a190 MATCH, 0x29cbb0 unchanged.
// The angle itself is still rounded through `real local_52` (retail keeps it in x87): the ternary written into the cos/sin arguments recomputes it three
// times (644, 3372 bytes, 0x29a190 MATCH; not kept: unshared ternary, `fadd qword`). Also tried: the existing function_2994a1 for the obstacle offset
// (retail's SSE offset chain from stored local_55: 662 with double locals, 3354 bytes, but 0x29a190 broke; 778 with the cast form, 3344 bytes);
// float constants in the dot clamp 701 (3324 bytes); `1.0 < local_48` same code.
// M3 lane 10 r86 (fast with 0x298d90, 0x298c30, 0x29a190, 0x29cbb0; control 733 at 3291 bytes, aligned 486, masked 261): retail never
// keeps the radius in a register (reloads it from its stack home in every block, even across the 0x2108a0/0x210770 calls); ours
// hoists it into XMM5 at entry. `inline` on the definition of 0x2108a0, of 0x210770, or both 897 each (3275 bytes, masked 278; in
// their /Ob1 file the keyword now inlines them here, so the old 759 lead is spent). The sweep arms in retail compute each scale right
// before its pair of products (forward x, y; reverse x, y; centre x, y): that statement order 791 (3307, masked 218), with the
// products written scale-first 791 (same code), scale-first alone 733 (same code); the obstacle rotation's x row followed at once
// by its offset, then the y row and offset, 742 (3287, masked 253); the offsets written radius-first 733 (same code); on the sweep
// order, the body under `if (sector != NONE) { ... return result; } return true;` 818 (3322, masked 205), the rotation order 801
// (3303, masked 210). All restored.
// M3 lane 10 r88 (fast with 0x29a190, 0x298d90, 0x298c30, 0x29cbb0; base 733, aligned 486, masked 261; 0x29a190 MATCH throughout): kept 629
// (plain-confirmed). Retail stores the rotated obstacle point, the circle point and its difference as floats and rereads them in SSE: the circle
// point through the existing function_2994a1, the difference through vector2d_from_points2d and the distance through magnitude2d 671 (3328 bytes,
// masked 221). Retail clamps the facing dot on x87 with double constants as the clamped value (`fld qword [1.0]`, 0x299f11-0x299f50): the clamp as
// a double expression passed straight to the arccos helper 669 (that block then matches instruction for instruction); with the obstacle angle in
// double (`(double)field_4 + pi : -(double)field_4`, retail's unrounded `fadd`/`fchs` feeding `fld st(0); fsin; fxch; fcos`) 637 (3313, aligned
// 270, masked 197). Leave-one-out: without the angle 669, without the clamp 697, without the helpers 715; the angle alone 753. Then the obstacle
// direction through vector2d_from_points2d 629 (3317, masked 195). Also: the clamp assignments as `1.0`/`-1.0` 733 (same code); the rotation
// rows without the `(double)` operand casts 649 (sine/cosine rounded to SSE), `(real)` on each whole row 649, the y row first and sine-first
// 637/637 (same code); the bounds sum `local_3 + field_10 + arg_6` or `arg_6 + local_3 + field_10` 629 (same code), `field_10 + (local_3 +
// arg_6)` 629; the bounds' two declarations swapped 558 (masked 201: listing realignment, a declaration order kept only with a match); the
// side endpoints local_21/local_22 through function_2994a1 712 (masked 169), the reverse one alone 717 (masked 174), with the sweep scales each
// before its products 798 (masked 148); the sweep order alone (re-measured) 771 (masked 153), forward arm 770, else arm 824; the body under
// `if (local_11 != NONE) { ... } return local_0;` 825, with the endpoints 806, with the sweep order too 819, with `return true` after 807; the
// sweep distance helper's squares z-y-x 629 (same code), `x + (y + z)` 626, `x + (z + y)` 626, y-z-x 629; normalize2d_fast for the obstacle
// direction 629 (same code); the edge clearance as an if/else 629 (same code). Diagnostic: the obstacle owner's actor through a named pointer
// gives retail's dead `add eax, edx` before 0x1df560 (which reads only CX and SI) and realigns the obstacle loop up to the normalizer (where
// retail preloads the direction's y into XMM1), but moves function_299a36's x87 schedule: 689; a one-use pointer, not kept.
// Then (plain-confirmed, 0x29a190 MATCH): retail lays out the facing-false value `2pi - angle` first in the second side's selection
// (`test bl, bl; jne` to the plain angle), the reverse of the first side: that ternary written `!local_47 ? 2pi - angle : angle` 569 (3323
// bytes, aligned 261, masked 189; a lead: an arm order picked by score is kept only on a match, reverted at review, 629). Leave-one-out on 569: without the helpers 703, without the clamp 644, without the double angle 578
// (aligned 391), without the direction helper 629, without the arm order 629. The same arm order in 0x298d90's selection gives retail's
// block order there too (masked 145 -> 138) but 399, not kept. The owner actor pointer with the endpoint helpers 724 (masked 168), with the
// sweep order as well 797 (masked 147), with the sweep order alone 794; diagnostics, not kept.
// On 569: the first side's selection written `!local_47 ? angle + pi : pi - angle` 569 (masked 196), the clearance ternary as
// `!(arg_6 > distance) ? ... : ...` 569 (masked 194); the owner actor pointer again 743.
// M3 lane 10 r90 (fast; base 629, aligned 267, masked 182; 0x29a190 MATCH throughout): kept 621 (aligned 243, masked 175): the rotation's sine
// and cosine as named reals, sine first as in this function's sweep loop, each used twice, which gives retail's `fld st(0); fsin; fxch;
// fcos` (both stay on x87, no rounding); the same pair cosine first 623 (aligned 259, masked 182); `(real)` on the pair 621 (same code).
// Leads, not kept: the first arm's angle as the float sum `(double)(field_4 + pi)` (retail's `fadd dword [pi]`; main's `(double)field_4 + pi`
// gives `fadd qword`) with the named pair 570 (plain-confirmed), alone 572; both arms `(double)(field_4 + pi) : (double)-field_4` 629 alone and
// 621 with the pair (retail's hoisted `fld; je; fadd dword; ... fchs` block exactly, masked 174, but the tail then sits one instruction off).
// The 570 form keeps an `fld` in each arm, unlike retail, and only gains by offsetting a shift elsewhere; the casts only move the sum to x87.
// Also: the whole ternary cast 604; the angle as a local adjusted per arm (`+= pi` / negated) 770 (SSE); the rows operand-first or the casts
// on the sine/cosine 570 (same code, on the 570 form); double sine/cosine locals 635 (0x29a190 45); one endpoint call per site with
// `field_0 ? radius : -radius` 918; local_7 and local_25 names exchanged 629 (same code: names do not order these homes). Re-measured on 570:
// sweep scales each before their products 775 (masked 135); the body under `if (local_11 != NONE) { ... return local_0; } return true;`
// 805 (masked 163); both 788 (masked 122: every block after the radius region then sits four or five instructions behind, so the XMM5
// radius is what blocks those two). Left: retail's products `fld st(0); fmul dword [esi]` where ours loads the component first
// (`fld dword [esi]; fld st(1); fmulp`).
// m3 lane 7 r150 (fast with 0x29a190, which stays MATCH, 0x298d90, 0x298c30, 0x29cbb0; base 621, masked 175): retail's sweep arms
// are not tail-merged because retail keeps a zero in XMM3 live across the join (the reverse endpoint's `0 - distance`, shared by
// both products) and the false arm materializes it with `xorps`; ours folds each `i * -d + x` into `x - i * d`, so the arms end
// alike and cross-jump. Retail's `return true` for the missing sector is the function's last block. Re-measured on this base:
// sweep scales each before its products 769; reverse endpoint through function_2994a1 722; both endpoints 716; sweep order and both
// endpoints 800; the body under `if (local_11 != NONE) { ... return local_0; } return true;` 807, with the sweep order 791, with both
// endpoints 790, with both 810. New: sweep order with the reverse endpoint 800; sweep order, both endpoints and the second side's
// `!local_47 ? 2pi - x : x` 794; the nested body with that ternary 803; the first offset as one function_2994a1 call with
// `field_0 ? radius : -radius` 906 (3269 bytes); the second offset that way 887 (3275 bytes).
// Kept (plain-confirmed, 0x29a190 MATCH): the reverse endpoint's negated distance named once (`reverse_offset`) and used for both
// products, as 0x298d90 names its negated radius: 621 -> 569 (3333 bytes, masked 177). The zero is then hoisted above the sweep
// branch here where retail creates it in each arm. On 569: the forward distance named too 569 (same code); either or both trace
// distances named before the sweep arms 569 (same code); the trace results renamed by role (three spellings) 569 (same code: names do
// not move their homes, the two side traces stay swapped against retail's +0xd0/+0xf4); the sweep order 729; the offset defined before
// the arms and used in both reverse scales 836; defined right after its trace 786; local_20's copy after the endpoints 766; the
// endpoints' z from local_20 571; the forward endpoint through function_2994a1 716.
// M3 lane 10 r92 (fast with 0x29a190, 0x298d90, 0x298c30; base 621, aligned 243, masked 181; 0x29a190 MATCH throughout): nothing
// kept. The sweep's direction i and j read once into named locals (eight uses each) 825 (masked 153); with each scale declared
// right before its two products 571 (3332 bytes, masked 142; a moved declaration, 825 without it, so not kept); one `scale` local
// reassigned before each pair of products 774 (3372 bytes, retail's length, masked 130), with the direction locals 732 (3333 bytes,
// masked 131); one `scale` with the body under `if (local_11 != NONE) { ... return local_0; } return true;` 703 (3372 bytes, masked
// 121), with the direction locals too 807 (masked 116); the per-scale order with that nesting, with/without the direction locals
// 812/791; local_7 renamed local_07 621 (same code: names do not order the local_7/local_24 homes). At retail's length with masked
// 121 the positional count stays high: registers differ in every block from the XMM5 radius hoist at entry onward.
// MiniMax A (11-12): no new form, no lower score (569): retail loads the radius parameter at its first use while ours hoists it
// into xmm5 at the entry, and the two side traces stay swapped against retail's +0xd0 and +0xf4 homes.
// P423: Offset scalar reference 732; rotation scalar-pair references 569;
// height reference 569; direction input references 569; direction output reference 569;
// distance input references 569; all offset/rotation scalar references 732;
// direction/distance references together 569. NOT kept: no gain or byte match;
// original complete body and all existing helpers restored.
// T95: Explicit signed-radius producers at first/pivot/both offset calls 886/874/915;
// shared signed radius 939; magnitude-preserving first/pivot parameter scales 897/882;
// repeated negative-radius producer 875; center-scale-only join 840.
// NOT kept: no exact native-radius repair; original complete body restored.
// Round 175 (m3 lane 6, fast; base 569): retail 0x2108a0, 0x210770 and their callees 0x1427f0/0x210d10 use no XMM5,
// and both calls take the same custom registers in both builds, so the radius hoist into XMM5 is an allocation choice,
// not an unknown-callee clobber. The end block as `field_8 = ratio > 1.0 ? 0.0f : field_8 * (1.0f - ratio)` 610; the
// obstacle angle selection as one nested ternary 569 (same code); the break's ratio as `(local_27 - 1) / (real)local_23`
// 569 (same code); the stored angle un-negated with the loop's `if (!field_0) local_28 *= -1.0f;` then a plain store 628;
// the annulus test as a positive `if` around the rest of the obstacle body 569 (same code). Diagnostic: the first two
// trace results' names exchanged 569 (homes unmoved: +0xd0/+0xf4 stay swapped against retail).
// P433: Existing named forward/reverse/center trace records, seven subsets,
// then all with center at its loop producer: 569 each, 3333/3372 bytes.
// NOT kept: radius retention and pivot/trace homes unchanged; original body restored.
// T99: Actual center-trace outcome owned by the operation result, clear/blocked
// bool/byte/short/long: 780/783/781/785/792/792/783/798.
// NOT kept: no equality or native radius/trace-home recovery; original independent
// center result, complete sweep, every helper and all geometry restored.
// P455: Isolated function storage for initial endpoint, transformed side, pivot,
// reverse direction, forward/reverse/center offsets and persistent center,
// eight at 569. Producers and reads stayed fixed. NOT kept: no match or native
// radius/trace/pivot-home repair; original complete body and helpers restored.
// P459: Existing pivot, mutable/reverse direction, previous/current sample, offset and complete geometry role names on the native-size frame:
// 569/569/569/569/569/569/569/569. NOT kept: native radius retention and pivot/trace homes unchanged; original complete body restored.
// P462: Initial two-edge trace loop with real direction/output tables and sector pair:
// long/dword/short/word/byte/signed-char for, long/short while,
// 854/854/837/837/837/837/854/837. NOT kept: larger frame and shorter extent;
// original independent traces, sector scalars, helpers and full body restored.
// P464: Independent actor/settings/path-settings/origin/forward/side/radius/obstacle
// parameter role names: eight 569s. NOT kept: eager radius and pivot/trace homes
// unchanged; original parameter names, shared producers and complete body restored.
// T102: Explicit zero subtraction at offset/negative-scale/reverse-distance phases,
// seven subsets 569 each; shared rotation point-setter publication 569.
// NOT kept: no byte equality or radius/trace/pivot-home repair; original whole body
// and three-use rotation producer restored.
// Complete valid-sector work guarded to the sole result return, bool/byte/short/long
// result at entry 809/832/836/836, after sector acquisition 850/862/844/844.
// NOT kept: all regress and shorten away from 3372; short/long frames grow to
// 0x118. Original whole body restored; eager radius and pivot/trace homes remain.
// T106: Whole-operation failure bool/byte/short/long 730/730/806/805;
// actual sweep/obstacle failure reasons char/byte/short/long 730/730/806/805.
// NOT kept: eager radius remains; narrow results shorten to 3300, wider forms
// enlarge the frame. Original success result and complete body restored.
// m3 lane 3 r241 (fast with 0x29a190, which stays MATCH, 0x298d90, 0x298c30, 0x29cbb0; all restored): ours keeps the radius
// parameter in XMM5 from entry across the 0x2108a0 and 0x210770 calls, retail reloads it; neither callee (nor 0x1427f0 and
// 0x210d10 below 0x210770, all matched) touches XMM5 in retail. Diagnostic, reverted at once: src/unknown_20fe20.cpp without
// /Ob1 leaves this body at 569 (the calls stay, XMM5 is still hoisted), so the callee file's flags are not the cause here.
// Tried: the angle step as `(field_4 < 0 ? -i : i) * pi/4` 817 (3317 B); the forward sweep scale as `distance - radius`
// 817 (3317 B); the step chosen by if/else with the last step first 816 (3317 B); the sector index read once before
// 0x2108a0 into the local the NONE test uses 768 (3320 B); the two result flags declared after the NONE return 569 (other
// code). Same code: either sweep sum with its operands swapped.
// Round 198 (m3 lane 4, fast with the file's four functions; all restored): function_2994a1's scale taken as `real
// const &` (so the call takes the radius parameter's address) 732 (ours still loads it into XMM5 at entry); the
// radius through a local copy `real radius = arg_6;` 569 (same code); the negated radius as `0.0f - arg_6` in both
// first-offset arms 569 (same code); function_2994a1 as plain `inline` 569 (same code); the three sweep points
// declared in the loop body 569 (same code). Ours still allocates the radius to XMM5 for the whole body (and tail-
// merges the two first-offset arms on it); retail loads it from its home in each block.
// M3 lane 15 r89 (fast with 0x29a190, matched throughout; control 569 = full; all restored): the 2-D offset helper's scale as
// `real const &` 732 (3349 B: the second if's arms stop sharing their tail as in retail, but the radius still sits in XMM5 from
// the entry; the reference folds away when inlined); the body after the NONE test nested in `if (local_11 != NONE)` with the one
// `return local_0;` 809 (the je reaches the end as retail, but that exit reads local_0 and the three bool homes move); nested with
// `return local_0;` inside and `return true;` after it 790 (3348 B; retail's epilogue order and je, positions shifted by the
// missing radius reloads); the obstacle angle as a float sum without the double casts 601 (3353 B; SSE addss, retail x87 `fadd
// dword`); that sum written into the sin/cos arguments 808 (3402 B). Lead: retail reloads the radius after 0x2108a0/0x210770 though
// neither of them nor their callees 0x1427f0/0x210d10 touches XMM5 in either build, so retail's link-time compile treated one of
// them as clobbering the XMM registers; no source form here reproduces that.
// M3 lane 15 r92 (fast with 0x29a190 MATCH, control 569): each sweep scale declared just before its two products
// (retail computes scale, products, scale, products) 729 (3317 B); the two trace distances named before the sweep arms
// (retail holds them in XMM6/XMM7 from there) 569 (same code); the first offset point through one call with
// `field_0 ? arg_6 : -arg_6` 886 (3286 B); the early exit as `return local_0;` 569 (same code; retail places that exit
// after the main epilogue); the names local_12 and local_13 swapped 569 (same code: the +0xd0/+0xf4 homes do not follow
// the names); diagnostic, the radius read once through its own address 569 (same code). Retail loads the radius inside
// the first arm where ours hoists it into XMM5 at entry (+0x14).
// M3 lane 16 r129 (fast; control 569, aligned 254, register-masked 215): the obstacle loop under `if (arg_7->field_2 >
// 0)` 638 (3349 B); the sweep loop under `if (local_23 >= 1)` 569 (different code); `arg_7 && local_0` 657; the
// result test `local_51 > local_36 || local_35 == NONE` 574; the final bound as `1.0f` 608 (3326 B; retail compares
// against a double, `fld qword`). The three-term sums here (the obstacle bounds, both distance helpers, the sweep
// rotation rows) already round as retail's.
// Typed actor location fields 569; shared location receiver 766; explicit
// zero-minus sweep scales, with or without the reverse endpoint, 569 each;
// separate setup and sweep traces 571; one sweep receiver 651; typed sector
// results 569; shared sector variable across both reads 569.
// Radius still loads at entry, and the side trace and pivot homes differ.
// MiniMax D (11-17): no new form, no lower score (569): this build still allocates the radius to XMM5 at the entry and
// keeps it for the whole body, where retail loads it from its home in each block; the earlier rounds' forms (a named
// radius copy, the negated radius, the helper as plain inline, the sweep scales named before their products, the swapped
// trace names, the radius read through its own address) all rebuild the same code, and the reload follows from retail's
// link-time compile treating 0x2108a0/0x210770 as clobbering the XMM registers, which no source form here reproduces.
// The frame is still 0xa0 against retail's 0x94, so the three trace results' homes (+0xd0 and +0xf4) sit 12 bytes off.
// Round 615: radius remains in XMM5 from entry; the pivot and side trace homes still differ.
// Radius local at initial offsets 569, pivot offsets 569, trace scales 569, obstacle range 569;
// wide obstacle displacement 574; upper range before root 642; explicit clearance arms 569;
// scoped transform output with pivot height copy 562 (NOT kept: seven-point gain above 30).
// Negative arm first at initial offset 876, pivot 569, sweep 775, initial and pivot 876, initial and
// sweep 872, pivot and sweep 775, all three 872; distinct branch-local reverse radii 732 at 3349
// bytes. XMM5 still retains the radius from entry and retail reloads at the branches.
// Planar offset helper with staged products per row/pair 850/850, staged final
// scalar coordinates 569, paired accumulation 850. Planar displacement 830,
// planar result copy 851, accumulated planar result copy 901 or member stores 830.
// NOT kept: eager radius remains in XMM5, with larger differences or a tie.
// Round 620: radius remains live from entry in XMM5; pivot and side trace homes are displaced.
// Typed pivot 569; shared facing dot 617, arc dot 569, both 718, both with planar vector 718;
// wide arc dot 616, guarded wide clamp 616, and wide arc with shared facing dot 626.
// Lateral distance z/y/x sum 569, y/z and z/y subtotals 569/569, named real
// subtotals 569/569, named squares 569, wide subtotal 573, wide differences 573.
// NOT kept: no exact x87 or radius-load recovery; pivot and trace homes still differ.
// DH398: One point or vector for the reverse trace direction and sweep point,
// each alone, with forward direction reuse, with local cosine, and with both 719/777/719/777, 719/777/719/777.
// NOT kept: All forms regress. The radius still loads before actor indexing and the side trace homes still differ. The separate geometry workspaces are restored.
// Early radius retention and pivot/trace homes remain. Center formed around the two transforms in shared scope 574/569/575/750.
// center formed around the two transforms with the sector block-scoped 574/569/575/750.
// M3 lane 5 r197 (fast with 0x29a190 and 0x298c30, both MATCH throughout; aligned exact / register-masked counts in
// brackets, base [254/172]; all restored). Scope check: 0x2108a0 has one retail call site (here), and none of 0x210770's 18
// retail call sites keeps an XMM value live across it. Diagnostic, reverted at once: the 0x2108a0 call replaced by an
// undefined `__stdcall` extern with the same argument (so the link knows nothing about what it clobbers) 727 at 3349 B
// [247/169]: the entry is then retail's to +0xc2 (no XMM5; `movss xmm1, [radius]` loaded inside each offset arm, both
// blocks), so retail compiled this function without 0x2108a0's register summary: BLOCKED-BY 0x2108a0 (its definition is in
// src/unknown_20fe20.cpp, another lane's file this round; `inline` on it now expands it here, 897 above). With that
// diagnostic, the body under `if (local_11 != NONE) { ... return local_0; } return true;` 766 at 3348 B [230/152]; that
// nesting alone 790 [239/157]. Left after the diagnostic: the pivot offset's negative arm (retail loads each component and
// multiplies by the negated radius, with no shared tail; ours copies the radius and shares the tail), the sweep arms
// (retail keeps the two arms whole, 32 instructions more), the pivot home (+0x94, ours +0xa0). Also: the offset helper's
// products written component first 569 (same code); the reversed direction formed ahead of the pivot offset 784.
// MiniMax E (11-19): no new form, no lower score (569): the frame is mapped instruction by instruction.  Retail's pivot (the
// first 0x210770 output) is at F+0x84 and its first sweep point at F+0x90; here the two are the other way round (pivot
// F+0x90, first sweep point F+0x84) while the second sweep point (F+0x9c), the third (F+0x48), the two trace results (F+0xa8
// and F+0xcc) and the result byte (F+0x03) agree; this build puts the forward trace result at F+0xcc and the reverse at
// F+0xa8, retail the other way round.  Tried and restored: the trace results' names swapped 569 (same code); the rotation
// helper's products with their operands swapped 569 (same code: MSVC canonicalises the commutative product); the y row
// before the x row 569 (same code); the pivot typed as a vector3f 569 (same code); the rotation's four products through
// four one-use temporaries 716 (worse).  Lead: retail reloads the radius from its argument home at each of its nine uses
// and reloads the obstacle loop's arguments as well, and that is where its 39 extra bytes are; ours keeps the radius in
// xmm5 from the entry because this build's link-time summary of 0x2108a0 and 0x210770 says they preserve the XMM
// registers, so no source form here reproduces it.
// MiniMax A (11-18): no new form, no lower score (569): the radius is still promoted into xmm5 at the entry and kept for the
// whole body, where retail loads it from its home in each block, and that follows from this build's link-time summary of
// 0x2108a0 and 0x210770, whose definitions are in another lane's file; the frames agree at 0x114 and the two trace results
// sit at the same homes (+0xd0 and +0xf4) but are taken in the opposite order, and the pivot's home is +0xa0 here against
// retail's +0x94.
// Round 436. Planar vector input in the offset helper 569. Native origin,
// destination or direction templates 972 each; native origin/destination,
// origin/direction, destination/direction or all three 972 each. NOT kept:
// the templates delay radius acquisition but outline the first offsets and
// shorten the body to 3194 bytes. Original helper and body restored.
// M3 lane 6 r201 (fast with 0x2108a0, 0x29a190, 0x298d90, 0x298c30; the lead also plain-checked with 0x29cbb0, 0x210770,
// 0x210d10; aligned = instructions equal after alignment out of retail's 810, base 595; all restored). LEAD, not kept:
// three pieces together give retail's exact length and 490 (3372 B, aligned 625; plain check 490 with 0x2108a0,
// 0x210770, 0x210d10, 0x298c30 and 0x29a190 MATCH, 0x298d90 305 and 0x29cbb0 378 as before): (1) `inline` on the
// definition of 0x2108a0 in src/unknown_20fe20.cpp (its one call here stays out of line in the present tree and 0x2108a0
// still matches; the caller then has no register summary for it, as lane 5 r197's diagnostic showed for retail, and the
// radius is loaded in each block instead of sitting in XMM5); (2) each sweep scale assigned to one `scale` local right
// before its two products, in both arms (retail's order: scale, x and y products, three times; the side trace homes
// +0xd0/+0xf4 then agree); (3) the body after the sector read under `if (local_11 != NONE) { ... return local_0; }
// return true;` (retail's last block is that `mov al, 1` exit). Not kept: (2) is a statement order and (3) an if with
// its arms exchanged, both kept only with a match, and (1) alone raises the count. Steps: (1) alone 726 (3349 B, first
// difference +0xd0, aligned 602); (1)+(2) 769 (3351 B, aligned 617); (1)+(2)+(3) 490. On (1)+(2): the offset helper's
// products written component first 769 (same code); a direction pointer local for the pivot offset calls 769 (aligned
// 612). On the 490 form: the scale expressions written in each product 672 (3380 B); the direction's i and j named
// before the arms 781 (3333 B); three named scales each declared before its products 770 (3348 B); one `scale` declared
// ahead of the arms 676 (3372 B, aligned 625). With one `return local_0;` after the nested body: 566 (3348 B) with (2),
// 786 (3332 B) without. Left on the 490 form: the pivot offset's negative arm (retail loads each component and multiplies
// it by the negated radius, storing x before it loads y, with no shared tail), the pivot home (+0x94, ours +0xa0, the
// first sweep point's home), the sweep arms' XMM registers (retail: scale XMM2, i XMM0, j XMM1; the false arm copies
// each component and keeps the first y product in XMM3 until the arm's end), the endpoint block after the arms, and the
// three rotations in the loop. The patch is build/r201/lead-299460.patch in the lane 6 worktree.
// M3 lane 1 r129 (fast with 0x29a190, 0x298c30, 0x2108a0 and 0x210770, all MATCH throughout; control 569, aligned 208/215 =
// unmatched instructions ours/retail, masked 95/102; body unchanged). `inline` on 0x2108a0's definition re-measured 726
// (3349 B, first difference +0xd0, aligned 203/208): the head then agrees up to the second pivot offset, where our two
// arms share the y sum and store (the negative arm copies the negated radius and multiplies it by the stack component,
// then scales y in place) and retail's arms each load the component and store y themselves.
// The obstacle angle as float sums (`field_4 + pi : -field_4`, no double casts; retail's `fadd dword`) 601 (3353 B,
// aligned 367/371), with the keyword 739 (3372 B, first difference +0x2e); the obstacle rotation's rows without the
// double casts on the direction (retail's `fld st(0); fmul dword [mem]`) 625 (3349 B, aligned 266/275), with the keyword
// 671 (3372 B, first difference +0xd0, aligned 268/268); both 643 (3372 B, aligned 344/349), with the keyword 744
// (3387 B). The normalizer's squares as `j*j + (k*k + i*i)` 569 (same code; retail stores k and then takes i*i, k*k,
// j*j from memory, ours keeps k on the x87 stack and squares it first); the distance's squares as `x + (y + z)`,
// `z + y + x`, `(z + y) + x`, `x + (z + y)` 569 each (same code; retail squares z, then y, then x).
// M3 lane 6 r207 (fast with 0x29a190, which stays MATCH; al = aligned differing lines out of retail's 810; the 108 lead also
// plain-checked with 0x2108a0, 0x210770, 0x210d10, 0x26c590, 0x298c30 and 0x29a190 MATCH, 0x298d90 305 and 0x29cbb0 378 as
// before; all restored, body unchanged). LEAD, not kept: 108 at 3374 B (al 30), and 236 at retail's 3372 B (al 32) without
// its last step. It still needs the match-only pieces below (the nested body, two one-use locals, an arm order, the new
// rotation helper, the helper's added parameter); the natural pieces alone on this body, with the early NONE return, score
// 715 to 729. Built on the r201 lead (`inline` on 0x2108a0's definition, the body under `if (local_11 != NONE)
// { ... return local_0; } return true;`, 490), each step on the one before:
// - sweep arms: the true arm with one `scale` local assigned before each `planar_scale(&local_6, scale, &local_17)` call
//   (then local_18, local_19), the false arm with the three named scales first and then the three planar_scale calls: 761
//   (3349 B, al 204; the false arm is then retail's: scales in XMM0/1/2, each product's destination a copy of the component);
// - the pivot offset's false arm with the negated radius in a named local ahead of the function_2994a1 call: 435 (3380 B;
//   both pivot arms retail's, no shared tail);
// - both side endpoints through function_2994a1 (the forward one with local_12.distance, the reverse one with the named
//   negated distance), each followed by its own `.z = local_5->z`: 355 (3372 B, al 130; both sweep arms and the endpoint
//   block exact, the components now in XMM0/XMM1 ahead of the scale);
// - one `point2f rotated` declared in the caller and passed to function_29991b at all three sweep rotations (the helper
//   writes the rotated pair there, then adds the pivot): 298 (al 71; the rotations exact, sine and cosine in XMM2/XMM3);
// - the first 0x210770 output in a `vector3f` of its own instead of the pivot: 280 (al 55; the pivot's home +0x94);
// - both obstacle dots through dot_product2d: 279 (al 52; normalizer and facing-dot registers retail's, y preloaded);
// - the obstacle owner's actor in a named pointer (retail's dead `add eax, edx`), the second side written
//   `!local_47 ? 2pi - a : a`, the angle `field_0 ? (double)(field_4 + pi) : (double)-field_4` (retail's `fadd dword`): 246
//   (al 38; alone 190, 219 and 279);
// - one offset pair for both obstacle differences, through vector2d_from_points2d and magnitude2d: 236 (3372 B, al 32);
// - the obstacle rotation through an inline helper with the rows `v->x * (double)cosine - v->y * (double)sine` and
//   `v->y * (double)cosine + v->x * (double)sine`, called with `(real)sin(a)`, `(real)cos(a)`: 108 (3374 B, al 30).
// Left on the 108 form: the two sine products (retail copies the sine and multiplies it by the memory component, ours
// loads the component first; the products of the component at offset 0 take retail's form and those at offset 4 do not,
// whichever of sine and cosine they multiply), the homes of the rotated side and the obstacle offset (+0x44 and +0x18 in
// retail, exchanged here) and two x87 sum orders that flip with unrelated edits (the first side distance, the first
// obstacle distance). What moved things: a product's destination is the operand read through a helper's pointer and its
// source the scalar assigned in its own statement; an expression argument of an inline helper becomes the destination;
// named scalars take XMM registers in declaration order unless a value's later in-place use gives it more references.
// Other forms, on the step named (lockstep counts). On 490: the sweep scales as expression arguments of planar_scale 786;
// the false arm's sum operands exchanged, scale-first products or the expression in each product 490 (same code);
// `-(-r - d)` 760 (not folded); three named scales interleaved 496; the true arm as `d + -r` or with a named `-r` 785
// (folds to `d - r`); the false arm's named scales first with direct products 501; both arms that way 765; the arms
// exchanged (diagnostic: the first arm always gets the component as destination) 766; named i/j per arm 782, after the
// named scales in the false arm 715; named scales first with planar_scale in both arms 761; one `scale` with planar_scale
// in both arms 758; planar_scale in the false arm only 502. On 761: the false arm's two scale declarations exchanged 763
// (the registers follow the declarations); the true arm's scales as expression arguments or as negated named positives
// 782; three named scales interleaved in the true arm 761 (same code), with direct products 771; named x/y locals inside
// function_29991b 761 (same code). On 298: the pivot typed as a pair 683; the rotation outputs or the pivot declared
// earlier 298 (same code). On 280: the distance sum as `x*x + (y*y + z*z)` or `(z*z + y*y) + x*x` 277 (both distance sites
// exact, the obstacle bounds sum flips), `z*z + y*y + x*x` 280; the endpoint declarations exchanged 280; the end copies
// reordered 310. On 277: the 2-D normalizer's rows through planar_scale or as `scale * x` 277 (same code); one dot alone
// through dot_product2d 286 (facing) or 277 (angle). On 279: the rotation rows without the double casts 329, a float
// angle 392, both 418; the rotation through a helper with float rows and `(real)sin(a)`, `(real)cos(a)` arguments 333
// (421 with a float angle); with double parameters 320 (331); the trig calls inside float rows 329, through a helper
// taking the angle 338 (469 with a float angle); double trig calls inside the rows 273 (394; cosine first, the angle
// stored); double sine/cosine locals 273 (392); the casts on the sine/cosine, on both, component-first rows or the Y row
// sine-first 279 (same code). On 246: role names for the rotated side and the offset 246 (same code: names do not order
// these homes); the offset written into local_6 254 (local_3 and local_18 homes move). On 236: the first difference as
// two scalars with the pair declared ahead of them 246; the rotation helper with named sine/cosine 245. On 108: the
// helper with a by-value vector 380; its parameters reordered 108, cosine first 111; the components copied into locals
// 116 (254 with named sine/cosine); named sine/cosine passed to it 245; sine named and cosine an expression 191; sine
// named with the cosine call written in direct rows, or the reverse 236; each product's operands exchanged (six
// spellings) 108 (same code); the Y row first 110; the rotated side written into local_17/18/19 137/125/131; the
// components exchanged in the rows (diagnostic) 110; sine and cosine as the members of one pair (caller local with
// direct rows 240, passed to the helper 236, helper local 111 or 108, by value 529, array 245); the rotated side in the
// shared `rotated` 108 (same code); the offset in local_6 with the rotated side separate or in local_18 117 / 125 (128 /
// 136 with the first difference as scalars); one or two unused real parameters ahead of the sine, or the output first
// (diagnostic) 108 (same code); a helper taking the angle as a double 418 (frame grows) or 117 from a named angle; component-first rows
// with the float trig calls written in them 245 (direct) or 116 (through a helper taking the angle). Main's
// body with planar_scale in both arms, the false arm or the true arm 708 / 574 / 780; the same with `inline` on 0x2108a0
// 737 / 726 / 751 (726 control). The patches are build/r207/lead-299460-w10.patch (108) and
// build/r207/lead-299460-w9.patch (236) in the lane 6 worktree.
// M3 lane 4 r234 (fast with 0x29a190, 0x298c30, 0x2108a0 and 0x210770, all MATCH; main's body 569, unchanged): lane 6
// r207's 108 lead re-applied on current main 108 (the full image 106). The permuter (--fast, annealing) cannot search it:
// its reduced image scores the lead 409. On the lead: the first obstacle difference in a pair of its own, the offset pair
// declared after or before the rotation output 187 / 187 (3372 B; the 2-D normaliser after it loses retail's order); one
// function-scope offset pair for the sweep rotations' output and both obstacle differences 108 (same code: the rotation
// output keeps +0x18 and the offset +0x44 whichever variable holds them); the 3-D distance sum as `(z*z + y*y) + x*x`,
// `x*x + (z*z + y*y)` and `z*z + y*y + x*x` 108 each (same code: the first side distance's square order is not the
// source's).
// M3 lane 14 r129 (fast; control 569; all restored, body unchanged). The w10 lead re-applied 108 (3374 B, aligned 30): left
// are the first distance site's squares (ours y then z, retail z then y), the obstacle offset's 2-D squares, the two sine
// products and the exchanged +0x18/+0x44 homes. On it: 0 to 7 unused `long` locals at the top 108 / 188 / 188 / 188 / 178
// / 178 / 236 / 240 (the obstacle offset's squares are retail's from 4 to 7, but two other x87 sums after it flip; the
// first distance site and the sine products never move); the distance helper's sum spelled `(z*z + y*y) + x*x`,
// `x*x + (z*z + y*y)`, `z*z + y*y + x*x` or `x*x + y*y + z*z` 108 each (same code: the sum is canonicalised); both
// distances through the existing inline distance3d (0x3ea30) 247 (3372 B: its vector local squares destructively, with
// fmulp); the first or the second distance passed straight into its trace call (no named local) 640 / 657 (the whole
// allocation moves), with 1 to 7 unused locals 640 to 738. The exchanged homes: the rotated side's and the offset's names
// swapped 108, the rotated side declared ahead of the offset 108, or at the top of the function 108 (all the same code).
// M3 lane 3 r282 (H2M_FAST; on lane 6 r207's w10 lead re-applied, 108 at 3374 B; all restored, body unchanged).
// Sites: A first loop distance squares, O obstacle offset squares, N obstacle normaliser squares, D facing dot,
// S obstacle rotation sine products, H rotated/offset homes. Control wrong A O S H. Reading: the x87 operand order
// at A, O, N and D follows the count of inlined-helper symbols before the site (dead longs inside a helper move them
// like the top-of-function scan); S never moves. Forms: local_24..26 or local_21/22 declared in reverse, or apart
// 108 each (same code); the first loop distance written at its site 232 (3372 B, A O right, N D S H wrong); the
// normalised-difference helper's scale written as rows 104 (A right; an existing helper written out); the distance
// helper with one vector3f local 232 (3372 B, A O right, N D wrong, the cosine products then component-first too);
// dead longs in the distance helper 1..7 188/174/232 x5 (174: A O right, N D wrong), in the rotation helper 108 x7,
// in the normalised-difference helper 1..11 188/188/188/178/178/236/240/236/236/236/232; unused dot products before
// the rotation 108 x7; rotation rows with the casts on the components, on all four operands or the sine product
// trig-first 108 each; double helper parameters 169; parameter orders (sin, cos, v, out) / (v, cos, sin, out) /
// (cos, sin, v, out) / (out, v, sin, cos) 108/111/111/108 (the sine products stay component-first whichever value is
// computed first); the y row sine-first 108; sine and cosine as double locals 190 (then the x products are
// trig-first and the y products component-first), with double parameters 297, the sine alone a double 190.
// Also on the lead: the rotation helper returning its output pointer, passed straight to the offset call 108 (same
// code); float rows without casts 183; the components read into locals first 116 (re-measure); the helper's
// parameters renamed four ways 108 each (same code: operand order does not follow names).
// M3 lane 12 r163 (H2M_FAST with 0x29a190 MATCH; control 569): the facing dot through dot_product2d 617 (3329 B, either
// argument order); the angle dot through it 569 (same code, either order); both 718 (3332 B); the obstacle difference
// normalised through normalize2d_fast (its result unused there) in place of the file's 0x298df3 copy 569 (same code);
// the two side lengths through distance3d with the arguments reversed (same a - b deltas) 587, with the normaliser too
// 587. Body unchanged.
// M3 lane 6 r224 (H2M_FAST with 0x29a190 and 0x298c30, MATCH each run; control 569; all restored): `inline` on this file's
// declaration of 0x2108a0 569 (a caller-side declaration leaves the summary; XMM5 kept); the radius read once through its own
// address at its first use 569 (folded); the first 0x210770 output into its own unread vector 562 (the pivot's home; an unread
// buffer, match-only); on that, both sector reads after both trace calls 561, both trace distances named before the arms 562,
// the in-loop trace result declared in the loop 562; on this body both distances named 569 (same code); the body after the
// sector read nested under `if (local_11 != NONE)` with `return true;` last 790 (3348 B). Still BLOCKED-BY 0x2108a0 (its
// file is on this round's Avoid list).
// M3 lane 16 r180 (H2M_FAST; main 569, unchanged; on lane 6 r207's w10 lead re-applied, 108 at 3374 B, aligned 30): the
// first sweep distance site follows the tree of the distance helper's sum: one redundant pair around its z square
// (`x*x + (y*y + (z*z))`) 104 (aligned 26, that site exact; the pair around x*x as well 104; around y*y, or y*y and x*x,
// 112; around both inner squares or around x*x alone 108). One to three pairs on the sweep angle's products move only the
// obstacle bounds' SSE sum (odd counts 124, even 104). The obstacle offset's squares follow the shared magnitude helper's
// tree: a pair around its j square (diagnostic, shared header) 115 (3373 B, aligned 22: both magnitude sites take retail's
// y-first order; the bounds sum flips), with one sweep pair 95 (3374 B, aligned 16). Left on 95: the two obstacle sine
// products (retail copies the sine and multiplies it by the memory component) and the rotated/offset homes (+0x18/+0x44
// exchanged). On 95: pairs around the sine products in one row, the other or both, with 0 to 3 sweep pairs, 95 or 108
// (aligned 16 to 33, never the sine form); the helper reading the vector3f's i/j directly 95 (same code); trig-first
// products 95 (same code); the sine held in a double local 154; the second difference in its own local 174 (3372 B,
// aligned 26: homes still exchanged, two other sums flip); the rotation output declared at the loop top 95 (same code).
// Patch: build/r180/lead-299460-95.patch (m3-lane16; needs the held `inline` on 0x2108a0 and the header pair).
// M3 lane 5 r240 (H2M_FAST with 0x29a190 and 0x298c30, MATCH each run; control 569): the radius named once at the top
// (`real const radius = arg_6;`, every use through it) 569 (same code); bound by `real const &` and used everywhere 569
// (same code); the pivot field and the 0x2108a0 lookup ahead of the first offset pair 822 (3336 B); one signed side scale
// (`arg_1->field_0 ? arg_6 : -arg_6`) named and used by both offset pairs 939 (3226 B); the offset helper's scale taken
// by const reference 732 (3349 B). Body unchanged; still BLOCKED-BY 0x2108a0.
// M3 lane 4 r258 (H2M_FAST with 0x29a190 and 0x298c30 MATCH; control 569): the first pivot offset's arms exchanged under
// `!arg_1->field_0` 876 (3299 B); both negated radii written `0.0f - arg_6` 569 (same code); the lookup result held as a
// long 716; the definition distance read after the lookup call 578; the sweep counter as a long 786 (3357 B). Reading: the
// radius is one register candidate from the entry block through the first arm (loaded once into XMM5), where retail loads
// it inside each arm; the piece only ends at a call whose summary clobbers XMM, so this is still BLOCKED-BY 0x2108a0.
// M3 lane 14 r153 (fast screen with 0x29a190 and 0x298c30, MATCH each run; control 569; all restored): 0x2108a0 moved to
// a file of its own built without whole-program optimisation, with `__stdcall` on it and on this file's declaration
// (retail pushes its argument and it returns with `ret 4`): 749 at retail's 3372 B (the entry is then retail's, no XMM5),
// but 0x2108a0 itself 36 (its index in EDI and loop count in EBP, retail EBP and EBX), so retail built that callee with
// whole-program optimisation and its missing summary is not a build-flag effect; `__stdcall` on 0x2108a0 in place 569
// (same code); the point local declared first 569 (same code); the origin cast named once for both first-offset calls
// 569 (same code); the result flag declared with the two trace flags 698 (3320 B). Lane 16 r180's held lead re-measured
// on this main 95 (3374 B). Frame packing: the pivot and the first and second sweep points tie at 9 references each, so
// their order is the sort's (retail puts the pivot first); the reverse trace result has 7 references to the forward
// one's 6 and sits below it (retail the other way round); both counts follow the XMM5 radius.
// M3 lane 11 r168 (fast screen with 0x29a190, 0x298c30, 0x2108a0 and 0x210770, MATCH each run; control 569; all
// restored, body unchanged; still BLOCKED-BY 0x2108a0: lane 6 r207's lead needs `inline` on that definition, a held
// form). The 108 lead re-applied on this main 108 (3374 B, aligned 30); main's body with the keyword alone 726. A rule
// found on 0x516d0 this round (the operands of a commutative float product and the terms of a float sum are ordered by
// the age of their symbols, the younger first, age being the first assignment) tried on what the lead leaves: the
// distance helper's three deltas assigned x,z,y / y,x,z / y,z,x / z,x,y / z,y,x with the sum parenthesised as in the
// lead 116 / 120 / 128 / 124 / 124 and plain 116 / 116 / 120 / 120 / 116, x,y,z plain 108 (the assignment order does
// move the x87 squares, never to retail's order at the first sweep site); the obstacle rotation helper taking the two
// components by value ahead of the sine and cosine 122, after them 118, the same with trig-first rows 118; the
// components read into locals inside the helper 116; the sine products alone written trig-first 108 (same code); the
// y row's terms exchanged 108 (same code); double sine and cosine parameters 169.
// M3 lane 1 r174 (fast screen with 0x29a190, 0x298c30, 0x2108a0 and 0x210770, all MATCH; control 569): the radius is
// still held in XMM5 across the 0x2108a0 / 0x210770 calls where retail reloads it after each; 0x1f99d0 shows the same
// cause at its transform calls (both BLOCKED-BY the held `inline` lead on those callees). Forms: the side flag read once
// into a `bool const` for its five uses 835 (3321 B); the cosine declared in the loop body 569 (same code); the step
// height `arg_3->z` named once for the three rotations 775 (retail's 3372 B, but the frame moves, first +0x2); the
// result flag declared just ahead of the sweep loop 606; the actor lookup ahead of the result flag 569 (same code).
// Body unchanged.
// M3 lane 15 r154 (H2M_FAST with 0x29a190 and 0x298c30, MATCH each run; control 569; all restored): retail's 0x2108a0
// takes its argument on the stack and returns `ret 4` as ours does, and nothing in retail's image holds its address, so
// the missing summary is not an address-taken callee. Diagnostic, reverted at once: 0x2108a0's file built without its
// SSE flag 569 (XMM5 kept). Forms: the radius copied into a `real const` after the two transform calls and used for
// every later use 569 (same code); the reversed direction formed after the NONE return 569 (same code); both sector
// reads after both trace calls 568; the definition distance read after the NONE return 715 (3320 B); the sweep count
// formed ahead of the scale arms 776 (3381 B). Body unchanged; still BLOCKED-BY 0x2108a0.
// @retail 0x299460
bool function_299460(long arg_0, s_298c46 *arg_1, s_298c30 const *arg_2,
	point3f const *arg_3, vector3f const *arg_4, vector3f const *arg_5,
	real arg_6, s_29946c const *arg_7)
{
	bool local_0 = true;
	s_actor_view *local_1 = actor_get(arg_0);
	point2f local_2;
	if (arg_1->field_0)
	{
		function_2994a1((point2f const *)arg_3, arg_6, (point2f const *)arg_5, &local_2);
	}
	else
	{
		function_2994a1((point2f const *)arg_3, -arg_6, (point2f const *)arg_5, &local_2);
	}
	real local_3 = *(real *)((byte const *)arg_2 + 0x14);
	short local_4 = function_2108a0(*(long *)((byte *)local_1 + 0x28c));
	point3f *local_5 = (point3f *)((byte *)local_1 + 0x27c);
	vector3f local_6;
	point3f local_7;
	function_210770(local_4, arg_4, (vector3f *)&local_7);
	function_210770(local_4, arg_5, &local_6);
	if (arg_1->field_0)
	{
		function_2994a1((point2f const *)local_5, arg_6, (point2f const *)&local_6, (point2f *)&local_7);
	}
	else
	{
		function_2994a1((point2f const *)local_5, -arg_6, (point2f const *)&local_6, (point2f *)&local_7);
	}
	vector3f local_8;
	vector3f_scale(&local_6, -1.0f, &local_8);
	bool local_9 = false;
	bool local_10 = false;
	long local_11 = *(long *)((byte *)local_1 + 0x28c);
	if (local_11 == NONE)
		return true;
	s_path_trace_result local_12, local_13, local_14;
	function_26c590(function_29962f(), local_5, local_11, NONE, &local_6, local_3, NULL, &local_12);
	long local_15 = ((s_sector_trace_result *)&local_12)->sector_index;
	function_26c590(function_29962f(), local_5, local_11, NONE, &local_8, local_3, NULL, &local_13);
	long local_16 = ((s_sector_trace_result *)&local_13)->sector_index;
	point2f local_17, local_18, local_19;
	if (arg_1->field_0)
	{
		real forward_scale = -(arg_6 - local_12.distance);
		real reverse_scale = -(local_13.distance + arg_6);
		real center_scale = -arg_6;
		local_17.x = local_6.i * forward_scale;
		local_17.y = local_6.j * forward_scale;
		local_18.x = local_6.i * reverse_scale;
		local_18.y = local_6.j * reverse_scale;
		local_19.x = local_6.i * center_scale;
		local_19.y = local_6.j * center_scale;
	}
	else
	{
		real forward_scale = (local_12.distance + arg_6);
		real reverse_scale = (arg_6 - local_13.distance);
		real center_scale = arg_6;
		local_17.x = local_6.i * forward_scale;
		local_17.y = local_6.j * forward_scale;
		local_18.x = local_6.i * reverse_scale;
		local_18.y = local_6.j * reverse_scale;
		local_19.x = local_6.i * center_scale;
		local_19.y = local_6.j * center_scale;
	}
	point3f local_20 = *local_5;
	point3f local_21 = {local_6.i * local_12.distance + local_5->x, local_6.j * local_12.distance + local_5->y, local_5->z};
	real reverse_offset = -local_13.distance;
	point3f local_22 = {local_6.i * reverse_offset + local_5->x, local_6.j * reverse_offset + local_5->y, local_5->z};
	short local_23 = (short)ceil(fabs(arg_1->field_4) * 1.2732394933700562f);
	real angle_cosine;
	point3f local_24, local_25, local_26;
	for (short local_27 = 1; local_27 <= local_23; local_27++)
	{
		real local_28 = arg_1->field_4 < 0.0f ? -local_27 * 0.7853981852531433f : local_27 * 0.7853981852531433f;
		if (local_27 == local_23)
			local_28 = arg_1->field_4;
		if (!arg_1->field_0)
			local_28 *= -1.0f;
		real local_29 = (real)sin(local_28);
		angle_cosine = (real)cos(local_28);
		if (!local_10)
			function_29991b(&local_17, local_29, angle_cosine, &local_7, arg_3->z, &local_24);
		function_29991b(&local_19, local_29, angle_cosine, &local_7, arg_3->z, &local_25);
		if (!local_9)
			function_29991b(&local_18, local_29, angle_cosine, &local_7, arg_3->z, &local_26);
		real local_31 = function_299a36(&local_20, &local_25, &local_6);
		if (!local_10)
		{
			real local_32 = function_299ae9(&local_21, &local_24);
			local_10 = function_26c590(function_29962f(), &local_21, local_15, NONE, &local_6, local_32, NULL, &local_12);
			if (!local_10)
				local_15 = ((s_sector_trace_result *)&local_12)->sector_index;
			else
				arg_1->field_8 *= 0.8f;
		}
		bool local_33 = function_26c590(function_29962f(), &local_20, local_11, NONE, &local_6, local_31, NULL, &local_14);
		if (!local_33)
			local_11 = ((s_sector_trace_result *)&local_14)->sector_index;
		if (!local_9)
		{
			real local_34 = function_299ae9(&local_22, &local_26);
			local_9 = function_26c590(function_29962f(), &local_22, local_16, NONE, &local_6, local_34, NULL, &local_13);
			if (!local_9)
				local_16 = ((s_sector_trace_result *)&local_13)->sector_index;
			else
				arg_1->field_8 *= 0.8f;
		}
		if (local_33 || (local_10 && local_9))
		{
			arg_1->field_4 = arg_1->field_0 ? local_28 : -local_28;
			arg_1->field_8 *= (real)(local_27 - 1) / local_23;
			local_0 = false;
			break;
		}
		local_20 = local_25;
		local_21 = local_24;
		local_22 = local_26;
	}
	if (local_0 && arg_7)
	{
		short local_35 = NONE;
		real local_36 = 0.0f;
		for (short local_37 = 0; local_37 < arg_7->field_2; local_37++)
		{
			s_299460 const *local_38 = &arg_7->field_c[local_37];
			s_slot_object_view *local_39 = object_get(local_38->field_4);
			if (local_39->type == 0)
			{
				long local_40 = *(long *)((byte *)local_39 + 0x12c);
				if (local_40 != NONE && function_1df560(local_1->unknown024, actor_get(local_40)->unknown024))
					continue;
			}
			real local_41 = local_2.x - local_38->field_8.x;
			real local_42 = local_2.y - local_38->field_8.y;
			real local_43 = (real)sqrt(local_41 * local_41 + local_42 * local_42);
			real local_44 = local_38->field_10 + local_3 + arg_6;
			real local_45 = arg_6 - local_3 - local_38->field_10;
			if (!(local_44 > local_43 && local_43 > local_45))
				continue;
			point2f local_46;
			vector2d_from_points2d(&local_2, &local_38->field_8, &local_46);
			function_298df3(&local_46);
			bool local_47 = arg_4->j * local_46.y + arg_4->i * local_46.x >= 0.0f;
			real local_48 = local_46.x * arg_5->i + local_46.y * arg_5->j;
			// retail clamps the unrounded dot on x87 against double bounds (0x299f11-0x299f50)
			real local_49 = function_50650(local_48 > 1.0 ? 1.0 : local_48 < -1.0 ? -1.0 : local_48);
			real local_50;
			if (arg_1->field_0)
				local_50 = local_47 ? 3.1415927410125732f - local_49 : local_49 + 3.1415927410125732f;
			else
				local_50 = local_47 ? local_49 : 6.2831854820251465f - local_49;
			real local_51;
			if (arg_1->field_4 > 0.0 ? arg_1->field_4 > local_50 :
				local_50 > arg_1->field_4 + 6.2831854820251465f)
				local_51 = arg_6 > local_43 ? local_43 - local_45 : local_44 - local_43;
			else
			{
				// retail forms the angle on x87 and never rounds it before fsin/fcos (0x29a041-0x29a050)
				real local_52 = arg_1->field_0 ? (double)arg_1->field_4 + 3.1415927410125732f : -(double)arg_1->field_4;
				// retail multiplies the unrounded sine and cosine in x87 (0x29a04c-0x29a085)
				point2f local_55;
				real rotation_sine = (real)sin(local_52);
				real rotation_cosine = (real)cos(local_52);
				local_55.x = rotation_cosine * (double)arg_5->i - rotation_sine * (double)arg_5->j;
				local_55.y = rotation_cosine * (double)arg_5->j + rotation_sine * (double)arg_5->i;
				point2f local_56;
				function_2994a1(&local_2, arg_6, &local_55, &local_56);
				point2f local_57;
				vector2d_from_points2d(&local_38->field_8, &local_56, &local_57);
				local_51 = local_3 + local_38->field_10 - magnitude2d((vector2f const *)&local_57);
				if (!(local_51 > 0.0f))
					continue;
			}
			local_51 /= local_3;
			if (local_35 == NONE || local_51 > local_36)
			{
				local_35 = local_37;
				local_36 = local_51;
			}
		}
		if (local_35 != NONE)
		{
			local_0 = false;
			if (local_36 > 1.0)
				arg_1->field_8 = 0.0f;
			else
				arg_1->field_8 *= 1.0f - local_36;
		}
	}
	return local_0;
}

union vector2f;
real function_11cc90(vector2f const *a, vector2f const *b);
extern point2f *g_468778;
real function_30bf0(vector3f *arg_0);
real normalize2d(point2f *arg_0);
point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_26c180(long arg_0);
bool function_1f9760(long arg_0, long arg_1, point3f const *arg_2, real arg_3,
	bool *arg_4, bool *arg_5, vector3f *arg_6);
bool function_1f9240(long arg_0, s_path_settings *arg_1);
void obstacle_list_group(s_obstacle_list *arg_0, real arg_1);
void __stdcall function_2c0d60(vector3f const *arg_0, long arg_1, byte arg_2, s_path_settings const *arg_3,
	s_obstacle_list *arg_4, s_obstacle_list *arg_5, point3f const *arg_6, real arg_7, vector3f const *arg_8,
	long arg_9, long arg_10);
real function_1f99d0(long arg_0, long arg_1, short arg_2, point2f const *arg_3,
	point2f const *arg_4, point2f const *arg_5, s_obstacle_list const *arg_6);
real function_1f9e70(long arg_0, long arg_1, point2f const *arg_2, vector2f const *arg_3,
	vector2f const *arg_4, vector2f const *arg_5, vector2f const *arg_6, real arg_7, bool arg_8);

__forceinline void function_29a20e(vector3f *arg_0)
{
	arg_0->k = 0.0f;
	real local_0 = (real)sqrt(arg_0->i * arg_0->i + arg_0->j * arg_0->j);
	if (!(0.0001f > fabs(local_0)))
	{
		real local_1 = 1.0f / local_0;
		arg_0->i *= local_1;
		arg_0->j *= local_1;
		arg_0->k *= local_1;
	}
}

// @retail 0x29a190
void function_29a190(long arg_0, long arg_1, vector3f const *arg_2, vector3f *arg_3,
	short *arg_4, real *arg_5, bool *arg_6, bool *arg_7, short arg_8)
{
	byte *local_0 = (byte *)actor_get(arg_0);
	s_slot_object_view *local_1 = object_get(arg_1);
	s_298c30 const *local_2 = (s_298c30 const *)function_1e5450(arg_0, *(long *)local_1);
	bool local_3 = false;
	bool local_4 = false;
	if (arg_8 == 0)
	{
		real const *local_rate = (real const *)(local_0 + 0x670);
		*(real *)(local_0 + 0x674) = *local_rate;
	}
	vector3f const *facing_axis = (vector3f const *)(local_0 + 0x290);
	vector3f local_5 = *facing_axis;
	bool *local_turning = (bool *)(local_0 + 0x6d1);
	*local_turning = false;
	function_29a20e(&local_5);
	point2f const *axis = g_468778;
	real local_6 = function_11cc90((vector2f const *)axis, (vector2f const *)&local_5);
	if (!*(bool *)(local_0 + 0x5d0) || *(bool *)(local_0 + 0x5d2) || !local_2)
	{
		*(real *)(local_0 + 0x670) = 0.0f;
		*(bool *)(local_0 + 0x678) = false;
	}
	else
	{
		point3f local_7;
		function_b9dd0(arg_1, &local_7);
		real local_8 = local_2->field_20;
		real local_9 = (real)sqrt(arg_2->i * arg_2->i + arg_2->j * arg_2->j + arg_2->k * arg_2->k);
		function_26c180(arg_0);
		vector3f local_10;
		if (function_1f9760(arg_0, arg_1, &local_7, local_9, &local_4, &local_3, &local_10))
		{
			*arg_7 = true;
		}
		else
		{
			if (local_3)
			{
				local_10.k = 0.0f;
				if (function_30bf0(&local_10) == 0.0f)
					local_3 = false;
			}
			vector3f local_11 = {-local_5.j, local_5.i, 0.0f};
			s_obstacle_list local_12;
			s_obstacle_list *local_13 = &local_12;
			s_path_settings local_14;
			if (*(bool *)(local_0 + 0x478))
				local_13 = NULL;
			else
			{
				function_1f9240(arg_0, &local_14);
				local_12.group_count = 0;
				local_12.count = 0;
				local_12.flag0_count = 0;
				local_12.flag3_count = 0;
				*(short *)local_12.unknown08 = 0;
				function_2c0d60(0, arg_0, true, &local_14, &local_12, 0, &local_7, local_8 * 3.0f, arg_2, arg_1, NONE);
				obstacle_list_group(&local_12, *(real *)((byte const *)local_2 + 0x14));
			}
			s_298c46 *local_15 = (s_298c46 *)(local_0 + 0x654);
			bool local_16 = true;
			bool local_21 = true;
			if (!*(bool *)(local_0 + 0x40) && *(bool *)(local_0 + 0x678))
			{
				real local_17 = dot_product2d((vector2f const *)&local_15->field_c, (vector2f const *)&local_5);
				if (!(local_17 > 0.0f))
					local_16 = false;
				else if (!(local_17 > 0.93f))
				{
					bool local_18 = dot_product2d((vector2f const *)&local_15->field_c, (vector2f const *)&local_11) > 0.0f;
					if (local_15->field_0)
						local_16 = (local_15->field_4 > 0.0f && !local_18) || (local_15->field_4 < 0.0f && local_18);
					else
						local_16 = (local_15->field_4 > 0.0f && local_18) || (local_15->field_4 < 0.0f && !local_18);
				}
			}
			if (local_16)
			{
				function_298d90(arg_0, arg_1, local_2, (point2f *)&local_5, (point2f *)&local_11,
					local_3 ? (point2f *)&local_10 : NULL, local_8, (point2f *)&local_7, (point2f const *)arg_2);
				short local_19 = *(short *)(local_0 + 0x66c);
				if (local_19 >= 0 && local_19 < g_504938)
					g_5047f8[local_19].field_8 *= 1.25f;
				short local_20 = 0;
				local_21 = false;
				for (;;)
				{
					short local_22 = NONE;
					real local_23 = 0.0f;
					for (short local_24 = 0; local_24 < g_504938; local_24++)
					{
						if (g_5047f8[local_24].field_8 > local_23)
						{
							local_23 = g_5047f8[local_24].field_8;
							local_22 = local_24;
						}
					}
					if (local_22 == NONE)
						break;
					if (local_20 & (1 << local_22))
					{
						if (!(g_5047f8[local_22].field_8 > 0.0f))
							break;
						local_21 = true;
					}
					if (function_299460(arg_0, &g_5047f8[local_22], local_2, &local_7,
						&local_5, &local_11, local_8, (s_29946c const *)local_13))
						local_21 = true;
					else
					{
						local_20 |= 1 << local_22;
						if (!local_21)
							continue;
					}
					local_15->field_0 = g_5047f8[local_22].field_0;
					local_15->field_1 = g_5047f8[local_22].field_1;
					local_15->field_4 = g_5047f8[local_22].field_4;
					local_15->field_8 = g_5047f8[local_22].field_8;
					local_15->field_c = g_5047f8[local_22].field_c;
					local_15->field_18 = g_5047f8[local_22].field_18;
					*(bool *)(local_0 + 0x678) = true;
					break;
				}
			}
			else
			{
				if (local_15->field_0)
					local_15->field_4 -= local_6 - *(real *)(local_0 + 0x668);
				else
					local_15->field_4 += local_6 - *(real *)(local_0 + 0x668);
				local_15->field_4 = -3.1415927410125732f > local_15->field_4 ? -3.1415927410125732f :
					(local_15->field_4 > 3.1415927410125732f ? 3.1415927410125732f : local_15->field_4);
			}
			if (local_21)
			{
				real local_27 = 0.0f;
				if (local_3 && local_15->field_4 > 0.0f && !local_15->field_1)
				{
					long path_offset = *(char *)(local_0 + 0x53a) * 0x1c;
					local_27 = function_1f99d0(arg_0, arg_1, (short)!local_4,
						(point2f const *)(local_0 + 0x548 + path_offset),
						(point2f const *)&local_7, (point2f const *)&local_10, local_13);
				}
				real local_28 = *(real *)((byte const *)local_2 + 0x30) * 0.01745329238474369f;
				real local_29 = *(real *)((byte const *)local_2 + 0x38);
				real local_30;
				if (local_15->field_0)
				{
					local_30 = (local_27 + local_15->field_4) * local_29;
					if (local_15->field_4 < 0.0f && local_15->field_1)
						local_30 = -local_28;
				}
				else
				{
					local_30 = (local_27 - local_15->field_4) * local_29;
					if (local_15->field_4 < 0.0f && local_15->field_1)
						local_30 = local_28;
				}
				if (-local_28 > local_30)
					local_30 = -local_28;
				else if (local_30 > local_28)
					local_30 = local_28;
				real local_31 = (real)sin(local_30);
				real local_32 = (real)cos(local_30);
				arg_3->k = 0.0f;
				arg_3->i = local_5.i * local_32 - local_31 * local_5.j;
				arg_3->j = local_31 * local_5.i + local_32 * local_5.j;
				if (function_30bf0(arg_3) == 0.0f)
					*arg_3 = *facing_axis;
				real local_33;
				if (local_15->field_4 >= 0.0f)
				{
					point2f local_34 = *(point2f const *)arg_2;
					normalize2d(&local_34);
					local_33 = function_1f9e70(arg_0, arg_1, (point2f *)&local_7, (vector2f *)&local_34,
						(vector2f *)arg_3, (vector2f *)&local_5, local_3 ? (vector2f *)&local_10 : NULL, local_9, false);
				}
				else
					local_33 = -1.0f;
				vector3f const *velocity = (vector3f const *)((byte *)local_1 + 0x88);
				real local_35 = velocity->j * local_5.j;
				local_35 += velocity->k * local_5.k;
				local_35 += velocity->i * local_5.i;
				real local_36 = *(real *)((byte const *)local_2 + 0x68);
				real local_37;
				if (local_36 > 0.0f)
					local_37 = *(real *)((byte const *)local_2 + 0x6c);
				else
				{
					local_37 = 4.019999980926514f;
					local_36 = 6.0f;
				}
				real local_38 = (local_33 - local_35 / local_36) * local_37 + local_33;
				if (local_38 > 0.0f && *(real *)((byte const *)local_2 + 0x60) > 0.0f)
				{
					real local_40 = local_38 - *(real *)(local_0 + 0x674);
					if (local_40 > *(real *)((byte const *)local_2 + 0x60))
						local_40 = *(real *)((byte const *)local_2 + 0x60);
					local_38 = *(real *)(local_0 + 0x674) + local_40;
				}
				*(real *)(local_0 + 0x670) = 0.0f > local_38 ? 0.0f : local_38;
				*arg_5 = local_38;
				*arg_5 = *arg_5 > *(real *)((byte const *)local_2 + 0x50) ?
					*(real *)((byte const *)local_2 + 0x50) :
					(-*(real *)((byte const *)local_2 + 0x50) > *arg_5 ?
						-*(real *)((byte const *)local_2 + 0x50) : *arg_5);
				*arg_5 = *arg_5 > 1.0f ? 1.0f : (-1.0f > *arg_5 ? -1.0f : *arg_5);
				*arg_6 = false;
			}
			else
			{
				*(bool *)(local_0 + 0x5d0) = false;
				*(short *)(local_0 + 0x5d6) = 0;
				*local_turning = false;
				*arg_6 = true;
			}
			*arg_4 = 0;
		}
	}
	*(real *)(local_0 + 0x668) = local_6;
}
