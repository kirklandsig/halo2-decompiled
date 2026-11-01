// @flags /O2 /Gr /arch:SSE
#include "unknown_044e90.h"
#include "normalize3d.h"

struct s_visibility_bound_record
{
    point3f center;
    real radius;
    byte transform_index;
    byte padding[3];
};

// score 406 (was 406): Frame 0x74a4 and list strides agree; entry/table and plane homes remain.
// Earlier: frame/list strides and entry/table/plane homes differed.
// Earlier: Frame 0x7494 versus 0x74a4; list strides, entry/table and plane homes differ.
// Storage headers are at 0xb0/0x10bc/0x20c8/0x40d4 versus retail 0xb0/0x10c0/0x20d0/0x40e0.
// Input plane homes are 0x2c/0x30/0x34/0x38 versus 0x34/0x38/0x3c/0x40; normal registers,
// inverse-scale reuse, distance division and capacity-release scheduling also differ.
// Scale-zero distance handling may differ semantically; it remains unchanged pending a behavior fix.
// plain plane for 2649; native view ranges 438; plane copy blocks 438; center point setter 438; views read
// before entry 442; entry bitfield 441
// Round 310: long bound index 438; explicit plane query 438; visibility-word pointer 438;
// capacity snapshot 438; guarded native input records 445 (plain). Entry/table and plane/list homes remain.
// Plane ordinal word 438; view ordinal word 438; frustum ordinal word 437; byte parts predicate 438; early containment byte
// 436 (plain).
// Entry-data cursor 438; selection flags 434; query count 438; inverse per frustum 525;
// captured frustum/view bounds 481/483; entry data plus inverse 525; complete matrix value 527.
// Round 166: query data result/conditional/negative-first 438/463/455; shared normal products
// 436; query result with products 440; const query 434; query result/products with const query
// 438/436. All product regroupings restored.
// Destination cursor 444; both plane cursors 484; destination/both with full publication
// 444/444; destination/both with outer plane 444/484; destination/both with normal declaration 444/484.
// Native input plane subobjects 468; separate shared-scaler normal output 476;
// multi-use distance scalar 453; input plus normal/distance 438/468;
// normal plus distance 471; all three 438; dword plane ordinal 434 (fast). All restored.
// DL131: Entry/tag-table registers and plane/list homes remain; existing geometry resolver
// before/after entry 443 each; native geometry record 434; resolver with native record in either
// entry order 443; resolver with shared normal scaling 443; native geometry with shared normal
// scaling 434; all three 443. Original source and resolver declaration restored; existing resolver
// body unchanged.
// Signed 32-bit flags 434; named section/both indices 455/455; structure-first/instance-first/
// combined selection 453/434/453; native normal/distance publication 441; source-plane cursor 445.
// Round 171 reduced control 434: direct entry 472; output planes 459; live transform pointer 434;
// entry/output planes 449; entry/live transform 472; planes/live transform 459; all three boundaries 449;
// entry/live selection flags 575.
// Round 317: Native rotation result 472; native center initializer 434; both 472; member alignment 435; aligned rotation/center/both 473/435/473; aligned rotation with shared normal scale 473. Header fully restored.
// Round 142: native tag/table/both observations 434/444/444; on the latter graph,
// workspace after selection/branch scope, signed flags, query count and entry-data receiver
// all 444; uniform selection-flag reads with tag-table/tag/tag-table-data 447/438/447.
// Round 211: Second/third/fourth capacity snapshots, each pair, all three and all four
// capacities at their sign/size consumers 434 each. Original capacity observations restored.
// Round 175: Native position/three rotation rows/both observations 475/556/586;
// with shared scale observation 477/556/586; independent scalar normals with
// all rows/position plus all rows 515/549. All original input ownership restored.
// Native storage-template alignment eight/sixteen 435/435; with scalar normal inputs
// 447/447; with whole-plane library publication 435/435; both 447/447. Header/body restored.
// Round 190: input-end/output-index 436; output-end/input-index 445; both cursors
// with input/output end do 486/451; input/output end for 473/451; input/output
// end while 473/451. Original fixed-six ordinal loop and all plane observations restored.
// Round 150: Native first/second, third/fourth and transform-array handles, seven subsets 434 each; all descriptor lifetimes beginning at first use 434. All handles restored.
// Round 146 (retail reads the flags inside each branch and the table into ESI before the tag test): entry flags
// at each use 438; table read once at the top 444; both 447 (the table lands in EDX); flags local per branch 441;
// entry declared before the workspace 434.
// Native stride-0x108 frustum source with raw/native count/native ranges/both 434 each;
// the same four forms with native six-plane array reference 434 each. Original source
// ownership, real record declarations and fixed-six loop restored.
// Round 240: Complete whole-query decision bool/byte/short/long 437/437/443/436;
// equivalent ordered alternatives 437/437/443/436. Entry/table and list/plane
// homes remain; all original selection observations and body restored.
// Round 162: Native selection fields alone/table/aligned/aligned-table 438/447/439/448; the same
// four with the existing normal scaler 438/447/439/448. Entry/table and plane/list homes remain;
// original external fields, storage alignment and all arithmetic restored.
// Round 273: native storage alignment eight/sixteen, crossed with original/frustum/view
// plane-normal declarations, eight forms 417/1742 each with frame 0x74a4. Four-point gain
// below gate. On that layout scale/plane-name/both/table/scale-table/plane-table/all/
// all-readonly-query 421/417/421/455/468/455/468/468. Header and entire body restored.
// Round 38: Independent list objects, list/transform lifetimes branch/branch, branch/entry,
// entry/branch and entry/entry, each with default/eight-byte alignment:
// 418/418/419/419/419/419/419/419 fast, all 1742 bytes. Branch/branch recovers frame 0x74a4;
// three-point gain rejected. Original combined workspace and complete body restored.
// Round 218: Live view source/direct ranges/both/both with native short view 481/474/453/450;
// native eight-byte storage alignment on those graphs 481/473/453/449. List/plane and entry/table
// homes remain; original header, observations and complete body restored.
// Round 435: Native eight-byte storage alignment plus chained counts and paired capacities,
// field orders pointer/count/capacity, pointer/capacity/count, count/pointer/capacity,
// count/capacity/pointer, capacity/pointer/count, capacity/count/pointer:
// 418/418/418/417/417/417; initialization by real capacity pairs, counts/capacities
// or capacities/counts 418/418. Four-point gain rejected; original header/body restored.
// Round 443: Native eight-byte storage plus ordinary-inline transform/containment/query
// declarations, seven subsets and all plus both publication boundaries, 417 each at 1742 bytes
// with frame 0x74a4. Four-point lead rejected; original header/body restored; entry/table and
// plane/list homes remain.
// Round 445: Real storage alignment eight with natural/eight plane-array alignment,
// each with aggregate/explicit query fields, 429 each; storage sixteen on the same four
// graphs 417 each. Four-point lead below keep gate; complete header and body restored.
// Round 97 (lane 7): retail stores 0 into the plane distance when the transform scale is 0 (a value
// difference: its zero path shares the distance store with the division path); `else p.d = 0.0f;` alone
// 444, on the 16-aligned storage template (417) 441. On that: the normal computed in both transform arms into
// a local vector and stored once at the join with `0.0f - p.d` (retail's join stores) 462; with
// `transform->scale != 0.0f` 450; with the normal locals defined after the distance block (the transform block
// is then retail's but for register rotation) 475 at 1700 bytes; with the release tests as
// `!(capacity & 0x80000000)` (retail's `test/js` and capacity reloads) 448 at 1744 bytes, aligned differences
// 294 -> 235; parts arms swapped 448 (the two calls' identical pushes stay hoisted, retail sets up each call in
// its arm). Left on that graph: the entry pointer in EDI (retail EBX, pushes interleaved), the tag table loaded
// in each branch (retail once into ESI before the tag test) and register rotations.
// Round 456: Persistent global entry reference; flags/table input graphs on native/default storage alignment:
// 421/457/433/454/417/455/432/452. Original executable body and supporting declarations restored.
// Round 467: Full-width input ordinals entry/section/both, then both with live
// selection fields, original storage 438/421/438/466; native eight-byte storage
// 435/417/435/465. Four-point aligned lead rejected; original header and body restored.
// Round 471: Native workspace list construction on eight-byte storage, original/reverse
// object order and six field-group orders 573 each (1620 bytes). Constructor and native
// alignment removed; original branch-local descriptor publication and full header restored.
// Round 474: Unsigned capacity/sign-mask on native four/eight storage 456/454;
// external-storage bit view 456/454; native allocation-count view 456/454;
// aligned whole-plane publication with bit/count or mask 480/480. Header and
// body restored; cleanup tests/preloads recover but entry roles and length regress.
// Round 230: Native bound record and sixteen-byte storage with point/vector copy/component
// construction/direct shared scale 417/417/450/424; per-part containment output on those
// graphs 406/406/449/421. Native point form kept at 406 after full validation; all call values
// and float grouping retained. Storage has its own type identity.
// Round 233: fixed-six indexed input with plane/normal scopes loop/branch, frustum/branch,
// view/branch, loop/frustum, loop/view, frustum/frustum, view/view and view/frustum
// 415 each at 1742 bytes. Original cursor, all observations and complete body restored.
// @retail 0x44e90
void __stdcall function_44e90(short entry_index, short section_index)
{
    s_visibility_query_workspace workspace;
    s_44940_entry *entry = &g_4ba138[entry_index];
    dword flags = entry->flags;
    byte *geometry;
    if (entry->tag != NONE)
    {
        byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
        byte *sections = *(byte **)(definition + 0x28);
        geometry = *(byte **)(sections + ((flags >> 9) & 0x1ff) * 0x5c + 0x34);
    }
    else
    {
        byte *section;
        if (!((entry->unknown00 >> 12) & 1))
            section = *(byte **)((byte *)g_4e0348 + 0xa0) + ((flags >> 9) & 0x1ff) * 0xb0;
        else
        {
            byte *instances = *(byte **)((byte *)g_4e0348 + 0x144);
            short index = *(short *)(instances + ((flags >> 18) & 0x7ff) * 0x58 + 0x34);
            section = *(byte **)((byte *)g_4e0348 + 0x13c) + index * 0xc8;
        }
        geometry = *(byte **)(section + 0x50);
    }
    byte *views = *(byte **)g_4c0b78.state->unknown00;
    byte *view_ranges = views + section_index * 0x1a + 0xa6e;
    if (*(long *)(geometry + 0x30) > 0) {
        bool parts = !g_4c0b78.state->active && (entry->unknown00 & 0x1000) &&
            *(long *)(geometry + 0x30) > 2 * *(long *)(geometry + 8);
        byte *query_data = *(byte **)(geometry + 0x2c);
        if (parts)
            query_data += (*(long *)(query_data + 0x20) + 15) & ~15;
        workspace.first.list.indices = workspace.first.indices;
        workspace.first.list.count = 0;
        workspace.first.list.capacity = (long)0x80000400;
        workspace.second.list.indices = workspace.second.indices;
        workspace.second.list.count = 0;
        workspace.second.list.capacity = (long)0x80000400;
        workspace.third.list.indices = workspace.third.indices;
        workspace.third.list.count = 0;
        workspace.third.list.capacity = (long)0x80000800;
        workspace.fourth.list.indices = workspace.fourth.indices;
        workspace.fourth.list.count = 0;
        workspace.fourth.list.capacity = (long)0x80000800;
        __declspec(align(16)) plane3f planes[6];
        s_visibility_plane_query query = { 6, planes };
        for (long view = 0; view < *(short *)views; ++view)
        {
            for (long i = 0; i < ((short *)view_ranges)[view + 1]; ++i)
            {
                long frustum_index = *(short *)(view_ranges + 0xe + view * 2) + i;
                plane3f const *source = (plane3f *)(views + 0x1974 + frustum_index * 0x108 + 0x78);
                transform4x3f const *transform = *(transform4x3f **)(entry->unknown10 + 0xc);
                long j = 0;
                do
                {
                    plane3f p = *source;
                    if (transform)
                    {
                        real n_i = p.i, n_k = p.k, n_j = p.j;
                        if (0.0f != transform->scale)
                        {
                            p.d -= transform->position.x * n_i + transform->position.z * n_k + transform->position.y * n_j;
                            if (transform->scale != 1.0f)
                                p.d /= transform->scale;
                        }
                        if (transform->scale != 1.0f)
                        {
                            real inverse = 1.0f / transform->scale;
                            n_i *= inverse; n_j *= inverse; n_k *= inverse;
                        }
                        planes[j].i = transform->forward.k * n_k + transform->forward.j * n_j + transform->forward.i * n_i;
                        planes[j].j = transform->left.k * n_k + transform->left.j * n_j + transform->left.i * n_i;
                        planes[j].k = transform->up.k * n_k + transform->up.j * n_j + transform->up.i * n_i;
                    }
                    else
                    {
                        planes[j].i = p.i;
                        planes[j].j = p.j;
                        planes[j].k = p.k;
                    }
                    p.d = -p.d;
                    planes[j].d = p.d;

                    ++source;
                    ++j;
                } while (j < 6);
                function_2dd930(query_data, &query, &workspace.first.list, &workspace.second.list);
            }
            if (parts)
                function_45a80(entry_index, true, (s_geometry_visibility_list *)&workspace.second.list,
                    (s_geometry_visibility_list *)&workspace.first.list);
            else
                function_458d0(entry_index, true, (s_geometry_visibility_list *)&workspace.second.list,
                    (s_geometry_visibility_list *)&workspace.first.list);
        }
        if (workspace.fourth.list.capacity >= 0)
            g_480118->allocate((long)workspace.fourth.list.indices, (workspace.fourth.list.capacity & 0x7fffffff) * 4, 0x12);
        if (workspace.third.list.capacity >= 0)
            g_480118->allocate((long)workspace.third.list.indices, (workspace.third.list.capacity & 0x7fffffff) * 4, 0x12);
        if (workspace.first.list.capacity >= 0)
            g_480118->allocate((long)workspace.first.list.indices, (workspace.first.list.capacity & 0x7fffffff) * 4, 0x12);
        if (workspace.second.list.capacity >= 0)
            g_480118->allocate((long)workspace.second.list.indices, (workspace.second.list.capacity & 0x7fffffff) * 4, 0x12);
    } else {
        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
        long object_index = **(long **)entry->unknown10;
        byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        long node_count = *(short *)(object + 0x114) / sizeof(transform4x3f);
        transform4x3f const *nodes = (transform4x3f *)(object + *(short *)(object + 0x116));
        for (long i = 0; i < node_count; ++i)
            function_142a60(&nodes[i], (transform4x3f *)(*(byte **)(tag + 0x4c) + i * 0x60 + 0x28), &workspace.transforms[i]);
        dword *visible = *(dword **)(entry->unknown10 + 4);
        for (long part = 0; part < *(long *)(geometry + 8); ++part)
        {
            dword bit = 1 << (part & 31);
            bool contained = false;
            if (!(visible[part >> 5] & bit))
            {
                word bound_index = *(word *)(*(byte **)(geometry + 0xc) + part * 8 + 4);
                s_visibility_bound_record const *bound = (s_visibility_bound_record const *)*(byte **)(geometry + 0x14) + bound_index;
                transform4x3f const *transform = &workspace.transforms[bound->transform_index];
                point3f local = bound->center;
                if (transform->scale != 1.0f)
                    vector3f_scale((vector3f const *)&local, transform->scale, (vector3f *)&local);
                point3f center;
                center.x = transform->forward.i * local.x + transform->left.i * local.y + transform->up.i * local.z + transform->position.x;
                center.y = transform->forward.j * local.x + transform->left.j * local.y + transform->up.j * local.z + transform->position.y;
                center.z = transform->forward.k * local.x + transform->left.k * local.y + transform->up.k * local.z + transform->position.z;
                if (function_165010((s_frustum_set_view *)views, section_index, &center, bound->radius, &contained))
                {
                    entry->unknown00 |= 1;
                    visible[part >> 5] |= bit;
                }
            }
        }
    }
}
