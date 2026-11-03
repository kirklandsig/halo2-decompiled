#include "unknown_11c920.h"
#include "frame_storage.h"
#include "globals.h"
#include "vector_products.h"
#include <xtl.h>
#include <math.h>
#include <stddef.h>

// @flags /O2 /arch:SSE /Gr

struct s_tag_data { long size; byte *address; };
struct s_particle_property
{
	short input_index, range_index;
	word modifier;
	short modifier_index;
	s_tag_data function;
};
struct s_draw_value_cache
{
	real values[17];
	dword field_44;
	void *field_48;
	void *field_4c;
	void const *field_50;
};
struct s_draw_index_span
{
	byte field_0[4];
	word first, count;
};
struct s_draw_mesh_definition
{
	long field_0;
	long orientation;
	byte field_8[0x20-8];
	s_particle_property scale_x, scale_y, scale_z, angle;
	byte field_60[0x80-0x60];
	long span_count;
	s_draw_index_span *spans;
	byte field_88[0x94-0x88];
	word *indices;
	byte field_98[0xd8-0x98];
	dword field_d8;
};
struct s_draw_mesh_context { s_draw_mesh_definition const *definition; long constant_index; };
struct s_draw_mesh_seed { byte field_0[0x1a]; word field_1a; };

typedef char check_draw_mesh_mask[offsetof(s_draw_mesh_definition, field_d8)==0xd8?1:-1];

void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values);
real function_246cd0(s_particle_property const *property, real const *values);
real function_30bf0(vector3f *v);
vector3f *function_11d000(vector3f const *v, vector3f *out);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
void function_1cf50();

/* the rotation by an angle (given as its sine and cosine) about a unit axis */
PRIVATE __forceinline void draw_rotation(vector3f const *axis, real sine, real cosine, matrix3x3 *out)
{
	vector3f axis_squared, axis_sine;
	axis_squared.k=axis->k*axis->k;
	axis_sine.k=axis->k*sine;
	axis_squared.i=axis->i*axis->i;
	axis_sine.j=axis->j*sine;
	axis_sine.i=sine*axis->i;
	real ij=(1.0f-cosine)*axis->j*axis->i;
	out->left.i=ij-axis_sine.k;
	out->forward.j=axis_sine.k+ij;
	axis_squared.j=axis->j*axis->j;
	real ik=(1.0f-cosine)*axis->k*axis->i;
	out->forward.i=(1.0f-axis_squared.i)*cosine+axis_squared.i;
	out->left.j=(1.0f-axis_squared.j)*cosine+axis_squared.j;
	out->up.i=axis_sine.j+ik;
	out->forward.k=ik-axis_sine.j;
	out->up.k=(1.0f-axis_squared.k)*cosine+axis_squared.k;
	real jk=(1.0f-cosine)*axis->k*axis->j;
	out->up.j=jk-axis_sine.i;
	out->left.k=axis_sine.i+jk;
}

// MiniMax S (11-01): no new form, no lower score.
// score 219 (was 219): Frame 0x6c, length and setup prefix agree; axis-specific rotation/cosine spills, basis registers and the four-byte table placement remain.
// Earlier at 219 (was 380): frame 0x6c and length agree; the rotation case bodies are scheduled differently
// (retail squares k first and spills it, keeps the sine products in registers and reloads cosine), the
// case 0 and 1/2 basis crosses colour their registers differently, and the jump tables sit 4 bytes
// earlier (0x2256d0 against 0x2256d4).
// Round 34 (lane A, machine 3): retail's rotation switch has two case bodies (the g_4687ac and
// the g_4687b0 axis) and one shared tail; grouping the five cases by axis gives 435 and 430
// (reversed groups), so the five-case form stays; a 300 s permuter run reached 321 differing
// instructions, its only new suggestion is a wrong swap of the row-13 components.
// Scalar axis inputs 393 plain (391 fast, frame 0x6c); all axis snapshots 754; third component
// snapshot 820; first two 900; camera record snapshot 615. Scalar-axis factor names, product names
// and independent square order all 391 (fast). Basis scope ends after composition 380;
// diagonal-first stores 610; cross destination return 380; second-axis cases first 380; scales
// after rotation 802 (fast).
// Frame 0x64 versus 0x6c; independently scheduled axis expansions and cosine/product spills remain.
// Scalar axis inputs 393 plain (391 fast, frame 0x6c); all axis snapshots 754;
// third component snapshot 820; first two 900; camera record snapshot 615.
// Scalar-axis factor names, product names and independent square order all 391 (fast).
// Reverse scalar inputs 441; trig-first scalar inputs 441; reversed inputs with orientation snapshot 441; reversed inputs with shared axis cases 400; reversed inputs with separate trig declarations 441.
// Round 121: Both axis pointers retained 470; complete local matrix then publication 1142; second axis pointer across switches 501; double angle 393; all three scale products factor-first 380. Original body and both helper bodies restored.
// Complete rows forward-left-up 683; forward-up-left 1012; left-forward-up 664;
// left-up-forward 1188; up-forward-left 1324; up-left-forward 1259; complete columns
// jki 1056 and kij 1369. Original interleaved stores and helper bodies restored.
// Complete native axis input copy 424; copy with complement/sine products/both
// 430/470/418; diagonal-first copy with the same four forms 428/468/461/515.
// All recover frame 0x6c and first +0xaf but regress; original helper restored.
// Round 235: Real-rounded trigonometric results in double storage: sine/cosine/both
// 465/654/764; with native double angle 448/451/576; both results with scalar axis
// inputs 783, and with angle 449. Original real storage, helper interface and body restored.
// Round 237: Parallel diagonal remainders with sine/complement sharing
// 410/443/573/518; parallel diagonal cosine products with the same sharing
// 410/443/573/518. Original helper expressions restored.
// Rotation case orders 43210/32104/12340/02143/13042/31024/20413/41230:
// 449/450/454/454/450/450/449/449 fast; original order restored.
// Basis case groups 021/102/120/201/210 380/381/383/384/383; basis 102
// with rotation orders 01243/01324/10423 393/451/393. Original order restored.
// Round 160: native destination/both receivers 380/380; immutable product/all producer
// lifetimes 380/380; complete cross inputs alone/with receiver 380/380; uniform
// context-definition observations 449; combined cross/receiver/producers 380.
// Round 76 rotation helper forms: products in i,j,k order with a named complement 429; forward/left/up row
// stores with a named complement 1293 (frame 0x6c, cases expanded); sine products named first 921; named
// complement alone 429; axis cases grouped 0,1,3 / 2,4 435 (aligned listing 374 -> 323).
// Round 162: Shared row setters in FLU/FUL/LFU/LUF/UFL/ULF order 1041/1550/704/715/985/706;
// complete complement 398, with all sine products 983. Other complemented row orders
// 828/414/701/1561/764; complemented scalar/native axis inputs 1111/1357. The 398 form has frame
// 0x68 versus 0x6c but still regresses. Original helper bodies, fields, arithmetic and calls
// restored.
// DL175: Three independent/shared-counter row scaling loops [1630, 1631]; full-width unsigned
// count/first/both [395, 396, 402]; both with independent rows 1619; immutable word span carriers
// alone/with rows [380, 1630]. Original helper, scaling and span carriers restored.
// Round 309: Existing cross producer, scalar/native-vector inputs and pointer/native-result
// outputs 468/924/705/500; with scalar rotation inputs 389/617/840/517. The 389 form
// recovers frame 0x6c, first +0xaf, but regresses; original helpers and full body restored.
// Scaled rotation producer with native input/complement/sine-product subsets
// 464/639/799/979/765/605/627/612; original helper and post-switch scaling restored.
// Round 249: Shared staged cross-product families 0 / 1-2 / 3-4, all seven
// nonempty subsets 380 each; all with shared in-place row scaling 380. Frame
// remains 0x64/0x6c; original helpers, include set and complete body restored.
// Round 35 (lane 13): the camera forward read straight from the global in the case 3/4 cross (no copy)
// 451 (frame 0x58; that case then matches retail); with `inline` for `__forceinline` 451; with a squared-axis
// vector local in the rotation 406 (frame 0x60), with the forward copy kept 390, with grouped cases 450;
// squared-axis and sine-axis vector locals 468 (frame 0x6c, five cases); in the order k^2, k*s, i^2, j*s,
// s*i, forward.i, ij, ... 1137 (five cases unmerged, 2480 bytes), with the cases grouped by axis 357; ij before
// forward.i 333 (1836 bytes); 40 random dependency-respecting orders 219..456, best (kept) 219; product
// operand orders and the two locals' names/declaration order 218-219 (same code); scalar sine products
// with the squared vector 328 (frame 0x64), scalar squares with the sine vector 438 (frame 0x60).
// Round 149 (lane 4, fast): the file's own cross helper replaced by the shared cross3f (vector_products.h, the
// same body) 219 (same code, kept); case 0's up copied before the negated forward 221; the rotation's products in
// retail's second-case order (k^2, s*j, i*s, s*k, i^2) 382 (1812 bytes); j^2 named with the other squares 232
// (1820); retail's first-case data flow (forward.i right after the sine products, a named complement and a shared
// k*complement for ik/jk) 238 (1816). Retail's two rotation cases are scheduled differently from each other
// (case 1 spills k^2 to +0x24, case 2 spills k^2 to +0x30 and s*j to +0x20), so its two inline copies got
// different homes for the square/sine vectors.
// J221: On native squared/sine vectors: shared complement 451; complete axis input 589; both 589; staged crosses 219; staged with complement/input/both 451/589/589; shared orientation 495. Original helper bodies, observations and complete function restored.
// Round 129 (m3 lane 6, fast): case 0's up copied field by field k, j, i 354 (the jump tables then sit at
// retail's address, but the copy becomes float loads and the cross recolours); the forward negations
// written k, j, i 219 (same code); both 354; the rotation's j square moved ahead of ij 360 (1840 bytes);
// left.j moved ahead of ik 350 (1764 bytes, frame changes). Compile-only (non-LTCG) does not reproduce
// this function's rotation block (its schedule and loads differ from the real build), so it can't screen it.
// J221: On native squared/sine vectors: shared complement 451; complete axis input 589; both 589; staged crosses 219; staged with complement/input/both 451/589/589; shared orientation 495. Original helper bodies, observations and complete function restored.
// Round 452: basis before rotation, before scaling, after forward scaling or after left scaling, composed
// into rotation 490/536/473/447; composed into basis 489/541/473/447. Original matrix lifetimes, alias
// destination and switches restored; square/sine/cosine spills, basis registers and table placement remain.
// Round 300: Plain coordinate products: square ascending/descending 506/505; sine
// ascending/descending 373/501; independent square-sine/sine-square loops 518/505; paired
// square-sine/sine-square loops 518/505. Original interleaved native products and complete body
// restored; axis-specific square/sine/cosine homes and basis registers remain.
// Round 239: Existing square/sine vectors through the native vector setter and complete cosine-pair
// producer lifetime, seven subsets 422/444/351/364/407/369/452; all with reversed pair declarations 395.
// Original private rotation producer and complete function restored; axis/cosine spill scheduling remains.
// Round 27 (m3 lane 18, fast; register-masked aligned counts in brackets, kept form 87): the rotation in retail's first-case flow with scalar
// squares and sines 503 (1704, frame 0x54) [76]; all values named then all stores 461 (1752) [98]; vector locals with a shared complement and k*complement
// 396 (1844) [104]; the shared k*complement alone 220 (1836) [82]; complement alone 452 (1756) [72]; complement and k*complement 497/408 [95/82]; 28
// dependency-respecting statement orders around retail's first case with k*complement, best 240 (1812).
// Round 248: Native squared/sine aggregate initialization, singly/together and producer order,
// mutable/readonly complete products: 455/485/443/486/455/485/443/486.
// Original partial product vectors, helper and complete body restored.
// Round 311: Native matrix return from the existing repeated rotation producer, complete axis capture,
// immutable mixed products and staged-cross subsets 0..7: 488/531/488/531/488/531/488/531. Original
// helper and full body restored; axis-specific square/sine/cosine spills and table placement remain.
// Round 515: Existing shared row scaler with coherent cosine complement, shared k-complement
// part and paired matrix names: 219/451/220/367/219/451/220/367. Native frame/product spills remain;
// complete private producer, scaling statements, include set and body restored.
// Round 357: Complete scalar rotation results before original output stores, six row and two column production orders 380/380/380/380/380/380/380/380. Original helper and body restored; two-axis spills remain.
// Round 362: CRT direction/up basis inputs, singly/together and with shared row scaling, then both with shared complement and with both producers.
// 219/219/219/219/219/219/451/451. Original executable source restored.
// Round 288: Shared sine-vector scaling after k/before squares/after early squares/after
// complete squares/before complete squares 440/444/436/351/444; with staged crosses/
// complete cosine complement/native in-place sine input 440/223/404. Original helper,
// sine observations and complete body restored; axis-specific spills remain.
// Round 377: Native diagonal output stages and cosine-product parts 204/204/204; paired
// output stages/results 219/219; second-pair output stages 204; complete stages 190;
// diagonal stages with paired results 204. Original helper/body restored.
// NOT kept: 204 has only two fewer executable rows (198 -> 196); thirteen raw points are table data.
// NOT kept: 190 combines a tying paired-output form with diagonal stages; allocation steering.
// F338: Actual forward/left/up output-row receivers in the existing rotation producer,
// seven subsets then all declared at first output: 217/217/217/491/491/491/220/220.
// NOT kept: two-point gains and unproven field-receiver shape. Original helper/body restored;
// axis-specific product/cosine spills, basis homes and four-byte table placement remain.
// B381: native square/sine/both workspace input copies, square-first/sine-first, then CRT
// square/sine/both copies: 273/444/466/466/273/408/466/466. NOT kept: value-copy/input-ownership forms require
// exact matching. Original frame, axis-specific product/cosine spills and basis homes remain.
// Round 43 (m3 lane 2, fast): the rotation helper in retail's computation order (k square, sine
// products j/i/k, i square and forward.i, ij rows, j square and left.j, ik rows, up.k, jk rows) 368
// (1824 B); a named 1-cosine local 452 (1756 B, frame changes); the j/i/k sine products before the k
// sine and i square 382; forward.i right after the sine products 357; left.j right after the j
// square 350. Retail's second switch keeps its range check as ours does; the 4-byte table shift follows
// the case-body schedule. Original helper restored.
// B385: Existing camera/direction basis providers and composed-return receiver, subsets
// 1/2/4/3/5/6/7, 222/221/622/495/623/622/628; all with basis output 429.
// Original complete body/declarations restored; axis-specific spills and table placement remain.
// Round 64 (lane 10, fast; masked = aligned, registers and stack offsets ignored, 87 here): the rotation's statements in retail's order (forward.i straight after the sines, left.j before ik, up.k before jk) 368 (1812, masked 68);
// with j and k first in the products 347 (1816, masked 75); with a named complement and k*complement 386 (1804, masked 80); with k*complement alone 368 (1808, masked 79), and j first in ij as well 368 (1808, masked 79);
// complement alone on the current order 451 (1756, frame 0x68, masked 77); k*complement alone on the current order 220 (1836, masked 82). Retail's two arms multiply in different orders (j, complement, i against complement, i, j) from one helper.
// @retail 0x224fd0
void __stdcall function_224fd0(s_draw_mesh_seed const *seed, s_draw_value_cache *values,
	point3f const *position, vector3f const *direction, real scale, color4f const *color,
	s_draw_mesh_context const *context)
{
	s_draw_mesh_definition const *definition=context->definition;
	s_draw_index_span const *span=&definition->spans[seed->field_1a%definition->span_count];
	dword mask=definition->field_d8&0xf80f;
	function_173ba0(~values->field_44&mask,values->field_48,values->field_4c,values->field_50,values->values);
	values->field_44|=mask;
	real angle=function_246cd0(&definition->angle,values->values)*6.2831854820251465f;
	real sine=(real)sin(angle), cosine=(real)cos(angle);
	real scale_i=function_246cd0(&definition->scale_x,values->values)*scale;
	real scale_j=function_246cd0(&definition->scale_y,values->values)*scale;
	real scale_k=function_246cd0(&definition->scale_z,values->values)*scale;
	matrix3x3 rotation, basis;
	switch(definition->orientation)
	{
	case 0:
	case 1:
	case 3:
		draw_rotation(g_4687ac,sine,cosine,&rotation);
		break;
	case 2:
	case 4:
		draw_rotation(g_4687b0,sine,cosine,&rotation);
		break;
	default: __assume(0);
	}
	rotation.forward.i*=scale_i; rotation.forward.j*=scale_i; rotation.forward.k*=scale_i;
	rotation.left.i*=scale_j; rotation.left.j*=scale_j; rotation.left.k*=scale_j;
	rotation.up.i*=scale_k; rotation.up.j*=scale_k; rotation.up.k*=scale_k;
	switch(definition->orientation)
	{
	case 0:
		basis.forward.i=-g_485618.forward.i;
		basis.forward.j=-g_485618.forward.j;
		basis.forward.k=-g_485618.forward.k;
		basis.up=g_485618.up;
		cross3f(&basis.up,&basis.forward,&basis.left);
		break;
	case 1:
	case 2:
		basis.up=*direction;
		function_30bf0(&basis.up);
		function_11d000(&basis.up,&basis.forward);
		cross3f(&basis.up,&basis.forward,&basis.left);
		break;
	case 3:
	case 4:
		basis.up=*g_4687b0;
		cross3f(&basis.up,&g_485618.forward,&basis.left);
		cross3f(&basis.left,&basis.up,&basis.forward);
		break;
	}
	function_142eb0(&basis,&rotation,&rotation);
	word *indices=definition->indices;
	D3DDevice_SetVertexData4f(12,rotation.forward.i,rotation.left.i,rotation.up.i,position->x);
	D3DDevice_SetVertexData4f(13,rotation.forward.j,rotation.left.j,rotation.up.j,position->y);
	D3DDevice_SetVertexData4f(14,rotation.forward.k,rotation.left.k,rotation.up.k,position->z);
	if(context->constant_index!=NONE)
		D3DDevice_SetVertexData4f(context->constant_index,color->red,color->green,color->blue,color->alpha);
	word count=span->count, first=span->first;
	function_1cf50();
	D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST,count,indices+first);
}
