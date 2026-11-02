#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19d220.h"
#include <string.h>
#include <wchar.h>

// @flags /O2 /Ob1 /Gr

long function_215c00(long arg_0, wchar_t const *arg_1);
void __stdcall function_215e60(long arg_0);
void function_1a0180(long arg_0, long arg_1, word *arg_2);
bool function_19d650(s_game_variant *arg_0);
bool function_212ec0(s_game_variant *arg_0, long arg_1);

// score 386 (was 386): Frame 0x464 and workspace homes agree; specialized flag/default stores and shared tail differ.
// Earlier: frame 0x464 and default/copy/text homes agree;
// inlined flag/value store scheduling and the shared tail differ. Inline depths
// one/two 388; global optimisation off 9513; size mode 1035; case nine first 389;
// terminator before copy 388; terminator via the copy result 398.
// Cases two/seven/eight first 414/407/409; zero inline depth 2361;
// separate save-assigned result 454 (fast).
// Case four first 388; five last 407; cases two/five adjacent 386;
// eight last 301 (executable sequence farther); high cases after one 423 (fast).
// AC338: case seven/two/four last 388/442/443; high pair exchanged 385; nine before high pair 302 (sequence worse); entry output 386.
// Round 121: Two/four exchanged 384; five before two/four 384; seven before four 386; eight before five 385; seven/eight after one 383; five/four/two tail 384; high pair with five/two/four or five/four/two 384/385. Original switch kept.
// Round 373: shared bounded text copy, orders 1245789/1789542/1254789/1425879
// 386/444/385/384; the same four with forced copy expansion 386/444/385/384.
// All restored; gains at most two, and the 384 form has a farther executable sequence.
// Case orders 2415789/2451789/2457189/2457819/1452789/1257489/1724589/1824579 417/417/416/418/389/387/386/384.
// Round 142: separate noinline declarations for create/normalize/save/dispose,
// each alone and create-dispose/normalize-save/save-dispose/all, all 386.
// Round 169: Public initializer ordinary/forced boundaries 386/386; each low specialization
// 386, high group 386, forced boundary with entry output 386. Original calls/declaration restored.
// Round 193: Low-tail/high-specialization orders 1542897/1542987/1542978/1542879;
// 1425897/1425987/1524897/1524987: 417/417/423/383/411/411/417/417. Original switch restored.
// Round 60: ours folds the types 4-6 bit-0 set into the defaults mask and stores the flags late;
// bit 0 through its bitfield, the mask split in two, bits 2-14 as single bitfields 386 each; the
// existing engine-to-type mapping call 1391; flags right after the name copy in the shared
// initializer 373 (0x19d220 then stops matching).
// Round 32: the flags store is what differs per case: retail computes and stores flags48 right
// after the name copy, before the copy's `add esp, 0xc`, so each case keeps its own store; ours
// schedules it after the stack adjustment, where it is identical in every case and is merged
// into the shared tail (each case one instruction shorter, the tail one longer, and the register
// rotation that follows). Variant declared at function scope 386; the failure path returning
// NONE directly 371 (only the length and table line up; the tail moves further from retail; not
// kept); the initializer's returned pointer used afterwards 386; an early NONE return 446.
// Round 66: output pointer throughout/init-only/tail-only/assigned from returns, output reference
// throughout/tail-only, shared source-name pointer and entry output with a pointer 386 each.
// The matched callees stayed exact; all executable forms restored.
// Round 388: Complete tail-family orders 1572489/1572849/1752489/1752849/
// 1248579/1284579/1248759/1284759: 385/386/386/387/385/387/383/384.
// Gains below the retention gate; original switch and shared initializer retained.
// Round 190: normalize/save/create caller declarations ordinary/forced inline, each and all,
// 386 each (eight forms). Original provider declarations, switch, copies and payload restored.
// Round 211: canonical-default/native mutable-output transfer by
// initialization/assignment/memcpy/memmove, with branch/entry default workspace, scores
// 408/408/408/420/408/408/408/420. Original executable form restored.
// @retail 0x212380
long function_212380(long arg_0, long arg_1, byte *arg_2)
{
    long const *local_0 = &arg_1;
    long local_2 = function_215c00(arg_0, (wchar_t const *)arg_2);
    if (local_2 != NONE)
    {
        s_game_variant local_6;
        switch (arg_0)
        {
        case 1:
            variant_defaults_initialize(&local_6, 0);
            break;
        case 2:
            variant_defaults_initialize(&local_6, 3);
            break;
        case 4:
            variant_defaults_initialize(&local_6, 1);
            break;
        case 5:
            variant_defaults_initialize(&local_6, 2);
            break;
        case 7:
            variant_defaults_initialize(&local_6, 4);
            break;
        case 8:
            variant_defaults_initialize(&local_6, 5);
            break;
        case 9:
            variant_defaults_initialize(&local_6, 6);
            break;
        default:
            __assume(0);
        }
        local_6.flags &= 0xfffe;
        function_19d650(&local_6);
        wcsncpy(local_6.name, (wchar_t const *)arg_2, 0x1f);
        local_6.name[0x1f] = 0;
        if (!function_212ec0(&local_6, local_2))
        {
            function_215e60(local_2);
            local_2 = NONE;
        }
    }
    return local_2;
}
