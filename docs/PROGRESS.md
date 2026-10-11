# Progress log

The newest entry comes first.

## 2026-10-10: 7487 functions match

```
matched 7487 of 11318 game functions (877360 of 2784283 bytes, 31.51%)
```

6 new matches, none lost:
- Deep lane 2, helpers first in 0x160000 to 0x16ffff: the helpers 0x16c4f0 and 0x16c5f0, and their callers 0x2a89b0, 0x2a8a00, 0x2a8c70 and 0x2a8cc0, unchanged.

## 2026-10-10: 7481 functions match

```
matched 7481 of 11318 game functions (876545 of 2784283 bytes, 31.48%)
```

3 new matches, none lost:
- Deep lane 1, new code and mid-length tuning in lanes W, Z, AB and AD: 0xb9d70, 0x41c20 and 0xd4cf0; eight new bodies.

## 2026-10-10: 7478 functions match

```
matched 7478 of 11318 game functions (876188 of 2784283 bytes, 31.47%)
```

1 new matches, none lost:
- Deep lane 3, helpers first: 0x2be5d0 matches with no change to it, after its helpers were tuned.

## 2026-10-10: 7477 functions match

```
matched 7477 of 11318 game functions (876062 of 2784283 bytes, 31.46%)
```

1 new matches, none lost:
- Our permuter lane: 0xe5280, held back because it calls a function that doesn't match yet, then checked by hand and confirmed by a full build.

## 2026-10-10: 7476 functions match

```
matched 7476 of 11318 game functions (875935 of 2784283 bytes, 31.46%)
```

3 new matches, none lost:
- Deep lane 2, closest first in lanes T, R, H and the UI core: 0x176210, 0x17d690 and 0x22cd48.

## 2026-10-10: 7473 functions match

```
matched 7473 of 11318 game functions (874621 of 2784283 bytes, 31.41%)
```

1 new matches, none lost:
- Deep lane 3, second-level blockers: 0xbef30 (281 bytes, newly written); its fifth parameter is a bool, as retail's callers pass it.

## 2026-10-10: 7472 functions match

```
matched 7472 of 11318 game functions (874340 of 2784283 bytes, 31.40%)
```

1 new matches, none lost:
- Deep lane 3, closest first in lanes V, X, A and the UI screens: 0x26dc90 (334 bytes).

## 2026-10-10: 7471 functions match

```
matched 7471 of 11318 game functions (874006 of 2784283 bytes, 31.39%)
```

5 new matches, none lost:
- Our permuter lane: 0x1061c0 and 0x195720, written by the lane, and 0xbf890, 0x196390 and 0x19c120, which it held back because they call functions that don't match yet; each was checked by hand and confirmed by a full build.

## 2026-10-10: 7466 functions match

```
matched 7466 of 11318 game functions (873311 of 2784283 bytes, 31.37%)
```

30 new matches, none lost:
- Machine 2's lane D lower half, deep round 12 (#300): 0x533e0, 0x534a0, 0x544e0, 0x58b00, 0x5f6a0, 0x600f0, 0x62e70, 0x62eb0, 0x64c70, 0x64d30, 0x680c0, 0x6f050, 0x765c0 and 0x8fa80.
- Lane I, deep round 10 (#301): 0x258480, 0x25bf10, 0x25c570, 0x25e800, 0x25ed60. Lane C, deep round 45 (#303): 0x1cb410, 0x1d70d0, and 0x1086e0 and 0x1087c0 as side effects.
- Lane P, deep round 14 (#297): 0x136970, and 0x2abd80 as a side effect. Lane AC (#298): 0x277a00. Lane D upper (#302): 0x8cfe0. Lane U (#304): 0x20aa70.
- Machine 2's permuter lane: 0x18e600 (#299) and 0x90d90 (#305).

## 2026-10-10: 7436 functions match

```
matched 7436 of 11318 game functions (866949 of 2784283 bytes, 31.14%)
```

7 new matches, none lost:
- Deep lane 3, helpers first: tuning the helpers at the first difference of close callers made seven callers match with no change to them: 0x1ada70, 0x1f5110, 0x1f90f0, 0x1f9490, 0x1fed70, 0x257a90 and 0x26c240.

## 2026-10-10: 7429 functions match

```
matched 7429 of 11318 game functions (864966 of 2784283 bytes, 31.07%)
```

6 new matches, none lost:
- Deep lane 1, helpers first: the helpers 0xa9440 and 0xb9c60 match, and with them their callers 0xd0d20, 0xd2bb0, 0x14e970 and 0x2a1610, with no change to those.

## 2026-10-10: 7423 functions match

```
matched 7423 of 11318 game functions (864283 of 2784283 bytes, 31.04%)
```

1 new matches, none lost:
- Deep lane 1, closest first in lanes S, N and Q: 0x101490 (266 bytes).

## 2026-10-10: 7422 functions match

```
matched 7422 of 11318 game functions (864017 of 2784283 bytes, 31.03%)
```

2 new matches, none lost:
- Deep lane 3, closest first in lanes V, X, A, the UI screens, W, Z, AB and AD: 0xd35a0 and 0x2aa2c0.

## 2026-10-10: 7420 functions match

```
matched 7420 of 11318 game functions (863786 of 2784283 bytes, 31.02%)
```

12 new matches, none lost:
- Machine 2's lane D lower half, deep round 11 (#292): 0x61e00, 0x662f0, 0x69260, 0x6aee0.
- Lane D upper half, deep round 11 (#293): 0x80aa0, 0x87ac0, 0x8c3a0, and 0x1a4141 as a side effect; also a behaviour fix in 0x8ef60.
- Lane O, deep round 19 (#294): 0x242510. Lane K, deep round 16 (#295): 0x2277b0, and 0x2079f0 as a side effect. Lane M, deep round 17 (#296): 0x1acfd0.
- Lane B, deep round 11 (#291): 0x1f2250 now returns bool (code only).

## 2026-10-10: 7408 functions match

```
matched 7408 of 11318 game functions (860814 of 2784283 bytes, 30.92%)
```

12 new matches, none lost:
- Deep lane 2, closest first in lanes S, T, R, Q, N, H and the UI core: 0x192b90, 0x16a0a0 and 0x17b490, whose first two parameters are now swapped at the definition and every call site.
- Nine more follow from the same round's changes: 0x168a10, 0x168e20, 0x1785c0, 0x17cc60, 0x17d350, 0x194a10, 0x214a60, 0x23da00 and 0x25217b.

## 2026-10-10: 7396 functions match

```
matched 7396 of 11318 game functions (857602 of 2784283 bytes, 30.80%)
```

2 new matches, none lost:
- The permuter's first matches: 0x158a50 and 0x2b540, found by its new annealing search and @coldspear's mutations (#286), each confirmed by a full build with nothing lost.

## 2026-10-09: 7394 functions match

```
matched 7394 of 11318 game functions (857436 of 2784283 bytes, 30.80%)
```

7 new matches, none lost:
- Deep lane 1, closest first in lanes S, N and Q: 0x1e110, 0x102c60, 0x102d60, 0x14b580, 0x14b600, 0x1502e0, 0x1537a0.

## 2026-10-09: 7387 functions match

```
matched 7387 of 11318 game functions (856593 of 2784283 bytes, 30.77%)
```

24 new matches, none lost:
- Merge batch r55: the second machine's lane D lower 10 (#282), C 43 (#283, #86 item 50), D upper 10 (#284, +4), M 16 (#285, +6 and a side effect), K 15 (#287, +3), F 16 (#288, #86 item 68), L 12 (#289, +9: 0x12e3a0 and the callers of 0x12de70) and AC 23 (#290, #86 item 66). Several of machine 1's functions waiting on those helpers matched with them (0x4ede0, 0xd1d70, 0x449e0, 0x16e290, 0xb57d0, 0xb5830).
- Permuter mutations learned from the near-to-matched history, by @coldspear (#286).

## 2026-10-09: 7363 functions match

```
matched 7363 of 11318 game functions (850898 of 2784283 bytes, 30.56%)
```

7 new matches, none lost:
- Deep lane 3, closest first: 0xa5b40, 0xa74c0, 0xd2dc0 (454 bytes), 0xab7f0 (353 bytes).
- Deep lane 2, closest first: 0x16cee0, 0x235d69; and 0x1724a0 matched as a side effect.

## 2026-10-09: 7356 functions match

```
matched 7356 of 11318 game functions (849316 of 2784283 bytes, 30.50%)
```

23 new matches, none lost:
- Deep lane 2, near-length (lanes S, T, R, Q, N, H and the UI core): 16 matches including 0x1015a0, 0x10f3b0, 0x10fcd0, 0x1922a0 and 0x252ed8.
- Deep lane 1, new bodies and mid-length tuning in the same lanes: 0x10a040, 0x14bf80, 0x156fe0, 0x213d20, 0x2a1800.
- Deep lane 3, large functions: 0x2bff80 (688 bytes) and 0x15ee30 (556 bytes).

## 2026-10-09: 7333 functions match

```
matched 7333 of 11318 game functions (844696 of 2784283 bytes, 30.34%)
```

6 new matches, none lost:
- Deep lane 3, round 13 (in lanes W, Z, AB and AD): 0xa4170, 0xa23a0, 0xa1120, 0xb5f10, 0x24970, 0x3dc90; sixteen newly written bodies are active.

## 2026-10-09: 7327 functions match

```
matched 7327 of 11318 game functions (843896 of 2784283 bytes, 30.31%)
```

9 new matches, none lost:
- Merge batch r54: the second machine's lane F round 15 (#278, +2), lane AC round 22 (#279, +2; also 0x2a81b0 in machine 1's lane A, which calls the newly matched 0x276b40), lane L round 11 (#280, +2) and lane B round 10 (#281, +2).

## 2026-10-09: 7318 functions match

```
matched 7318 of 11318 game functions (841924 of 2784283 bytes, 30.24%)
```

7 new matches, none lost:
- Deep lane 1, near-length tuning (lanes W, Z, AB and AD): 0x1dc40, 0x27960, 0x3d380, 0x41c80, 0x44ac0, 0xa4e20, 0xae7f0.

## 2026-10-09: 7311 functions match

```
matched 7311 of 11318 game functions (838786 of 2784283 bytes, 30.13%)
```

11 new matches, none lost:
- Merge batch r53: the second machine's lane B round 9 (#274, +4), lane D lower round 9 (#275, +2), lane D upper round 9 (#276, +3) and lane C round 42 (#277, +2).

## 2026-10-09: 7300 functions match

```
matched 7300 of 11318 game functions (836500 of 2784283 bytes, 30.04%)
```

4 new matches, none lost:
- Deep lane 3, round 12: 0x18cb10, 0x1fb940, 0x263ed0, 0x26bc60. Two are the other machine's functions that matched once function_249c20 and function_249d60 took retail's parameter order.

## 2026-10-09: 7296 functions match

```
matched 7296 of 11318 game functions (835327 of 2784283 bytes, 30.00%)
```

12 new matches, none lost:
- Deep lane 3, round 11 (closest first): 0x1aea20, 0x1b4bd0, 0x1b4d90, 0x1e3370, 0x1eb220, 0x25ac00, 0x25d420, 0x266540, 0x26d9c0, 0x272af0, 0x2a0650, 0x2bb801. Six of them are the other machine's functions that matched once the parameter order of a helper they call (function_1fb7e0, function_14a280) was corrected.

## 2026-10-09: 7284 functions match

```
matched 7284 of 11318 game functions (832782 of 2784283 bytes, 29.91%)
```

45 new matches, none lost:
- Merge batch r52: the second machine's lanes L, U, F, AC (with @coldspear's #86 fixes), B (+7), D lower (+3), D upper (+14), C (+8), M (+2), P (+1), K (+4) and O (+2); @coldspear's interface quad draw analysis (#260) and a decomp.dev progress-report workflow (#265).

## 2026-10-09: 7239 functions match

```
matched 7239 of 11318 game functions (823009 of 2784283 bytes, 29.56%)
```

4 new matches, none lost:
- Deep lane 3, round 10 (lanes V, X, A and the UI screens): 0x26c2d0, 0x26dde0, 0x265550, 0x2bacbc; eight newly written bodies are active.

## 2026-10-09: 7235 functions match

```
matched 7235 of 11318 game functions (821770 of 2784283 bytes, 29.51%)
```

11 new matches, none lost:
- Deep lane 1, round 9 (lanes W, Z, AB and AD): 0x1cdd0, 0x30710, 0x3c9a0, 0xa07f0, 0xa0a60, 0xa3c00, 0xa6810, 0xaf320, 0xaf5f0, 0xb91d0, 0xbfc30, and the retail-confirmed points of @coldspear's reviews (#86 items 29-31 and 39).

## 2026-10-09: 7224 functions match

```
matched 7224 of 11318 game functions (819704 of 2784283 bytes, 29.44%)
```

4 new matches, none lost:
- Deep lane 3, near-length tuning (lanes V, X, A and the UI screens): 0x1e4390, 0x260160, 0x2a40e0, 0x2b83db, and closer bodies for 29 more.

## 2026-10-08: 7220 functions match

```
matched 7220 of 11318 game functions (819010 of 2784283 bytes, 29.42%)
```

12 new matches, none lost:
- Deep lane 2, second exact-length pass: 0x1076e0, 0x1088a0, 0x108d30, 0x108d90, 0x10e480, 0x1520f0, 0x1523c0, 0x162c50, 0x22dfa9, 0x23ba90, 0x2520ff.
- Deep lane 3: 0x1eed40, and the retail-confirmed points of @coldspear's review of 0x2c0d60 (#86 item 38).

## 2026-10-08: 7208 functions match

```
matched 7208 of 11318 game functions (816793 of 2784283 bytes, 29.34%)
```

22 new matches, none lost:
- Merge batch r51: the second machine's lane C round 40 (#257, +2), lane B round 7 (#256, +3), lane D lower round 7 (#258, +11) and lane D upper round 7 (#259, +6).

## 2026-10-08: 7186 functions match

```
matched 7186 of 11318 game functions (812356 of 2784283 bytes, 29.18%)
```

6 new matches, none lost:
- Deep lane 1, exact-length tuning (lanes W, Z, AB and AD): 0x122c0, 0x123b0, 0xa0960, 0xa3a00, 0xa7ab0, 0xad940.

## 2026-10-08: 7180 functions match

```
matched 7180 of 11318 game functions (811828 of 2784283 bytes, 29.16%)
```

16 new matches, none lost:
- Deep lane 2, exact-length tuning (lanes S, T, R, Q, N, H and the UI core): 0xc3e90, 0x103bd0, 0x10ae60, 0x10cf50, 0x10ee20, 0x1407d0, 0x142bf0, 0x146240, 0x15ca10, 0x15cbf0, 0x163140, 0x1631f0, 0x1632a0, 0x163350, 0x16a7c0, 0x16d9c0.

## 2026-10-08: 7164 functions match

```
matched 7164 of 11318 game functions (808913 of 2784283 bytes, 29.05%)
```

13 new matches, none lost:
- Deep lane 3, exact-length tuning (lanes V, X, A and the UI screens): 0x1e3790, 0x1ec500, 0x1ef500, 0x1efb40, 0x1efbd0, 0x261280, 0x26add0, 0x26bf10, 0x2a1f40, 0x2a9640, 0x2bd020, 0x2bd140, 0x2c4030. Several needed a helper's parameter order corrected (0x20a9a0, 0xd8a40, 0x154220, recorded_animation_find, 0x1df770).

## 2026-10-08: 7151 functions match

```
matched 7151 of 11318 game functions (806412 of 2784283 bytes, 28.96%)
```

7 new matches, none lost:
- Deep lanes, helper-first rounds: deep lane 1 +5 (0xb9dd0, 0xa5e70, 0x3eb20, 0xbc100, and 0x2a2190, a near miss that matched once its callee 0xbc100 did), deep lane 2 +1 (0x15b270), deep lane 3 +1 (0x2bf8d0).

## 2026-10-08: 7144 functions match

```
matched 7144 of 11318 game functions (804185 of 2784283 bytes, 28.88%)
```

9 new matches, none lost:
- Deep lanes switch to tuning near misses: deep lane 1 +8 (0x1d2f0, 0xa1410, 0xa1670, 0xa18c0, 0xa1ba0, 0xa3600, 0xa8f10, 0xb4a90; reorders the parameters of simulation_write_position, 0x86d20), deep lane 3 +1 (0x1e22d0).

## 2026-10-08: 7135 functions match

```
matched 7135 of 11318 game functions (802521 of 2784283 bytes, 28.82%)
```

26 new matches, none lost:
- Merge batch r49: the second machine's lane K round 13 (#253, +1), lane M round 14 (#250, +5), lane P round 11 (#248, +5) and lane C round 39 (#251, +15, one of them 0x2a5250 as a side effect).
- The deep lanes' complete drafts of large functions (rounds 1-3) are on main as work in progress; none matches exactly yet.

## 2026-10-08: 7109 functions match

```
matched 7109 of 11318 game functions (796701 of 2784283 bytes, 28.61%)
```

15 new matches, none lost:
- Merge batch r48: the second machine's lane O round 16 (#244, +1), lane C round 38 (#245, +6), lane U round 11 (#246, +4) and lane AC round 20 (#247, +4, one of them a side effect); documentation follow-ups from @coldspear (#243).

## 2026-10-08: 7094 functions match

```
matched 7094 of 11318 game functions (794052 of 2784283 bytes, 28.52%)
```

28 new matches, none lost:
- Merge batch r47: the second machine's lane D rounds 2-6 (upper #227, #231, #234, #237, #240, +9; lower #229, #233, #235, #238, #241, +13), lane B rounds 2-6 (#230, #232, #236, #239, #242, +5) and lane M round 13 (#228, +1).

## 2026-10-08: 7066 functions match

```
matched 7066 of 11318 game functions (789417 of 2784283 bytes, 28.35%)
```

4 new matches, none lost:
- Merge batch r46: the second machine's first rounds of lanes D (#225, +3) and B (#226, +1).

## 2026-10-08: 7062 functions match

```
matched 7062 of 11318 game functions (788532 of 2784283 bytes, 28.32%)
```

7062 new matches, none lost:
- Merge batch r44: blocker lane BX1 (+1, 0xd47d0).

## 2026-10-08: 7061 functions match

```
matched 7061 of 11318 game functions (788448 of 2784283 bytes, 28.32%)
```

7061 new matches, none lost:
- Merge batch r45: blocker lane BX2 (+1, 0x2b7abe). The full check also showed 0x12ba90 had stopped matching after #222's new 0x12b450 body; 0x12b450 is back to its stub, which restores 0x12ba90 and matches 0x12b690.

## 2026-10-08: 7059 functions match

```
matched 7059 of 11318 game functions (788228 of 2784283 bytes, 28.31%)
```

7059 new matches, none lost:
- Merge batch r43: the second machine's #222 to #224 (lanes L, K and M; +2).

## 2026-10-08: 7057 functions match

```
matched 7057 of 11318 game functions (788080 of 2784283 bytes, 28.30%)
```

7057 new matches, none lost:
- Merge batch r41: the resting-lane sweep, round 3 (+4).

## 2026-10-08: 7053 functions match

```
matched 7053 of 11318 game functions (787749 of 2784283 bytes, 28.29%)
```

7053 new matches, none lost:
- Merge batch r42: the second machine's #219 to #221 (lanes C, P and U; +8 including 0x190e37).

## 2026-10-08: 7045 functions match

```
matched 7045 of 11318 game functions (786974 of 2784283 bytes, 28.26%)
```

7045 new matches, none lost:
- Merge batch r40: the second machine's #218 (lane K round 11, +4), #216, #217 and the #215 follow-up.

## 2026-10-08: 7041 functions match

```
matched 7041 of 11318 game functions (786455 of 2784283 bytes, 28.25%)
```

7041 new matches, none lost:
- Merge batch r39: near-match polish round 3 (+1, 0x86b40).

## 2026-10-08: 7040 functions match

```
matched 7040 of 11318 game functions (786342 of 2784283 bytes, 28.24%)
```

7040 new matches, none lost:
- Merge batch r38: the second machine's #212 to #215 (lanes M, P, O and J; +5).

## 2026-10-08: 7035 functions match

```
matched 7035 of 11318 game functions (785210 of 2784283 bytes, 28.20%)
```

7035 new matches, none lost:
- Merge batch r37: lane Z rounds 11 to 13 (+8), after fixing 0xb9b90's calling convention and the base v16 return type.

## 2026-10-08: 7027 functions match

```
matched 7027 of 11318 game functions (784651 of 2784283 bytes, 28.18%)
```

7027 new matches, none lost:
- Merge batch r36: lane D round 30 (+1).

## 2026-10-07: 7026 functions match

```
matched 7026 of 11318 game functions (784516 of 2784283 bytes, 28.18%)
```

7026 new matches, none lost:
- Merge batch r35: lane W round 20 (+1) and the second machine's #204 to #211 (lanes AC, U, C and F; +12).

## 2026-10-07: 7013 functions match

```
matched 7013 of 11318 game functions (782562 of 2784283 bytes, 28.11%)
```

7013 new matches, none lost:
- Merge batch r34: UI-core rounds 16 and 17 (+2).

## 2026-10-07: 7011 functions match

```
matched 7011 of 11318 game functions (781916 of 2784283 bytes, 28.08%)
```

7011 new matches, none lost:
- Merge batch r33: lane H rounds 5 and 6 (+7).

## 2026-10-07: 7004 functions match

```
matched 7004 of 11318 game functions (780544 of 2784283 bytes, 28.03%)
```

7004 new matches, none lost:
- Merge batch r32: the second machine's #203 (lane J round 7, +1) and #202 (lane L round 8, written code).

## 2026-10-07: 7003 functions match

```
matched 7003 of 11318 game functions (780087 of 2784283 bytes, 28.02%)
```

7003 new matches, none lost:
- Merge lane AD rounds 1 to 3 (+10): network message codecs, session search, surface queries and weapon object type, in documented gaps between finished ranges.

## 2026-10-07: 6993 functions match

```
matched 6993 of 11318 game functions (779165 of 2784283 bytes, 27.98%)
```

6993 new matches, none lost:
- Merge batch r31: lane A rounds 18 and 19 (+7).

## 2026-10-07: 6986 functions match

```
matched 6986 of 11318 game functions (777995 of 2784283 bytes, 27.94%)
```

6986 new matches, none lost:
- Merge batch r30: the resting-lane sweep, rounds 1 and 2 (+5), including seven new source files for groups that had no home.

## 2026-10-07: 6981 functions match

```
matched 6981 of 11318 game functions (777464 of 2784283 bytes, 27.92%)
```

6981 new matches, none lost:
- Merge batch r29: lane H rounds 3 and 4 (+7).

## 2026-10-07: 6974 functions match

```
matched 6974 of 11318 game functions (776356 of 2784283 bytes, 27.88%)
```

6974 new matches, none lost:
- Merge batch r28: the second machine's #196 to #200 (lanes J, K, P and O; +10) and @BrassMonkey71's #163 (0xe59e0).

## 2026-10-07: 6963 functions match

```
matched 6963 of 11318 game functions (774630 of 2784283 bytes, 27.82%)
```

6963 new matches, none lost:
- Merge batch r27: UI-core rounds 14 and 15 (+11), including the score-display group in a new src/unknown_23f260.cpp.

## 2026-10-07: 6952 functions match

```
matched 6952 of 11318 game functions (772152 of 2784283 bytes, 27.73%)
```

6952 new matches, none lost:
- Merge batch r26: lane W round 18 (+2) and the second machine's #193 to #195 (+5).

## 2026-10-07: 6945 functions match

```
matched 6945 of 11318 game functions (771250 of 2784283 bytes, 27.70%)
```

6945 new matches, none lost:
- Merge batch r25: the second machine's #180 to #192 (lanes K, Y, O, M, F, L, P and C; +33).

## 2026-10-07: 6912 functions match

```
matched 6912 of 11318 game functions (763805 of 2784283 bytes, 27.43%)
```

6912 new matches, none lost:
- Merge batch r24: lane H rounds 1 and 2 (+8), the first Codex rounds in 0x190000-0x19ffff.

## 2026-10-07: 6904 functions match

```
matched 6904 of 11318 game functions (762636 of 2784283 bytes, 27.39%)
```

6904 new matches, none lost:
- Merge batch r23: UI-screens rounds 10 and 11 (+3).

## 2026-10-07: 6901 functions match

```
matched 6901 of 11318 game functions (762446 of 2784283 bytes, 27.38%)
```

6901 new matches, none lost:
- Merge batch r21: UI-core rounds 11 to 13 (+9: the main menu and window-manager initialisers, two text-widget helpers and lobby/settings screen code).

## 2026-10-07: 6892 functions match

```
matched 6892 of 11318 game functions (760427 of 2784283 bytes, 27.31%)
```

6892 new matches, none lost:
- Merge batch r20: the second machine's #169 to #179 (lanes AC, P, U, L, K, J, M and F; +51) and lane W rounds 16 and 17 (+6).

## 2026-10-07: 6835 functions match

```
matched 6835 of 11318 game functions (752377 of 2784283 bytes, 27.02%)
```

6835 new matches, none lost:
- Merge batch r18: lane W round 15 (+5) and the second machine's #168 (lane U round 4, +3).

## 2026-10-07: 6827 functions match

```
matched 6827 of 11318 game functions (750623 of 2784283 bytes, 26.96%)
```

6827 new matches, none lost:
- Merge batch r17: the second machine's #166 (lane AC round 13, +4) and #167 (lane C round 31, +2, including 0xf06e0 in vehicles.cpp as a side effect), and the blocker hunt's one fix (0x1ee50).

## 2026-10-07: 6820 functions match

```
matched 6820 of 11318 game functions (749301 of 2784283 bytes, 26.91%)
```

6820 new matches, none lost:
- Merge batch r16: Codex lanes X round 6 (+6) and UI-core rounds 9 and 10 (+8), the second machine's #164 (lane C round 30, +6) and #165 (lane U round 3, +8), and side effects 0x2730c0 and 0x2a4e80.

## 2026-10-07: 6790 functions match

```
matched 6790 of 11318 game functions (744337 of 2784283 bytes, 26.73%)
```

6790 new matches, none lost:
- Merge batch r15: the second machine's #162 (lane AC round 12, +7) and Codex lane D round 29 (written source only).

## 2026-10-07: 6783 functions match

```
matched 6783 of 11318 game functions (743501 of 2784283 bytes, 26.70%)
```

6783 new matches, none lost:
- Merge batch r14: Codex lane B round 12 (+4) and the second machine's #161 (lane I round 6, +1).

## 2026-10-07: 6778 functions match

```
matched 6778 of 11318 game functions (742632 of 2784283 bytes, 26.67%)
```

6778 new matches, none lost:
- Merge batch r13: the second machine's #160 (lane C round 29, +11, unlocked by the caller fixes in #130) and #159 (lane O round 11, +1).

## 2026-10-07: 6766 functions match

```
matched 6766 of 11318 game functions (740974 of 2784283 bytes, 26.61%)
```

6766 new matches, none lost:
- Merge batch r12: Codex lanes D round 28 (+2), UI-screens round 9 (+4) and V round 7 (+1), and the second machine's #158 (lane I round 5, +5) and #157 (lane F round 7, +1).

## 2026-10-07: 6753 functions match

```
matched 6753 of 11318 game functions (738337 of 2784283 bytes, 26.52%)
```

6753 new matches, none lost:
- Merge batch r11: Codex lanes B round 11 (+5), W round 14 (+1) and AB round 9 (+2), and the second machine's #154 (lane F round 6, +3), #155 (lane I round 4, +5) and #156 (lane O round 10, +3).

## 2026-10-07: 6734 functions match

```
matched 6734 of 11318 game functions (736034 of 2784283 bytes, 26.44%)
```

6734 new matches, none lost:
- Merge batch r10: Codex lanes D round 27 (+6), Z round 9 (+1) and UI-screens round 8 (+7); the second machine's 19 PRs #130-#133 and #139-#153 (+40: lane Y rounds 1-7, F 4-5, O 8-9, I 3, U 1, M 5, C 28, AC 11, and caller fixes in lane C round 27); @BrassMonkey71's #129 (0xe5980).

## 2026-10-07: 6679 functions match

```
matched 6679 of 11318 game functions (728159 of 2784283 bytes, 26.15%)
```

6679 new matches, none lost:
- Merge batch r9: Codex lanes AB round 8 (+5, including 0x241900 and 0x26df40) and R round 7 (+2), the second machine's #126 (+1) and #127 (+4), @BrassMonkey71's #125 (0xe58e0) and @coldspear's #128 (docs).

## 2026-10-06: 6666 functions match

```
matched 6666 of 11318 game functions (725899 of 2784283 bytes, 26.07%)
```

6666 new matches, none lost:
- Codex lane W round 13: 0x2c340 and 0x3bf00, plus 0x11d790, 0x22a4b0 and 0x2901e0 once 0x30bf0 kept retail's call boundary.

## 2026-10-06: 6661 functions match

```
matched 6661 of 11318 game functions (725450 of 2784283 bytes, 26.06%)
```

6661 new matches, none lost:
- Codex lane D round 26: 0x5d5a0, 0x687b0, 0x68e20, 0x693a0, 0x82b30 and 0x843f0.

## 2026-10-06: 6655 functions match

```
matched 6655 of 11318 game functions (723443 of 2784283 bytes, 25.98%)
```

6655 new matches, none lost:
- @BrassMonkey71's first contributions: 0xe5240 (#119), 0xd4db0 (#122) and 0xe5670 (#115).

## 2026-10-06: 6652 functions match

```
matched 6652 of 11318 game functions (723327 of 2784283 bytes, 25.98%)
```

6652 new matches, none lost:
- Merge batch r8: the second machine's lane C rounds 22-26 (+19), lane AC rounds 6-9 (+12, plus 0x262a90), lane P round 3 (+2) and the side-effect match 0xc6740 (PRs #109, #111, #112, #114, #117, #118, #120, #121, #123, #124).

## 2026-10-06: 6617 functions match

```
matched 6617 of 11318 game functions (715693 of 2784283 bytes, 25.70%)
```

6617 new matches, none lost:
- Codex lane R rounds 5 and 6: 16 effects and decals functions (0x1730a0 to 0x17d900), plus lane K's 0x22a060 through its callee 0x177260; also fixes the inverted colour-query test in 0x17b5d0 that @coldspear found.

## 2026-10-06: 6600 functions match

```
matched 6600 of 11318 game functions (713625 of 2784283 bytes, 25.63%)
```

6600 new matches, none lost:
- Codex lane B round 10: 0x1b3f60 and 0x1bbc00.

## 2026-10-06: 6598 functions match

```
matched 6598 of 11318 game functions (713331 of 2784283 bytes, 25.62%)
```

6598 new matches, none lost:
- Codex lane U round 11: 0x204ca0, and lane L's 0x124360, 0x124790, 0x124800 and 0x124840 once their callees 0x2174b0 and 0x217520 became real code.

## 2026-10-06: 6593 functions match

```
matched 6593 of 11318 game functions (712902 of 2784283 bytes, 25.60%)
```

6593 new matches, none lost:
- Codex UI-core lane rounds 7 and 8: 16 more UI-core functions (0x22df04 to 0x23dda0).

## 2026-10-06: 6577 functions match

```
matched 6577 of 11318 game functions (709648 of 2784283 bytes, 25.49%)
```

6577 new matches, none lost:
- Codex UI-screens lane round 7: nine screen and list functions (0x2b0b96 to 0x2cad45), and lane O's 0x24a7ec through its callee 0x2b10a3.

## 2026-10-06: 6567 functions match

```
matched 6567 of 11318 game functions (708667 of 2784283 bytes, 25.45%)
```

6567 new matches, none lost:
- Codex lane Q round 6: 0x150820, 0x151c10 and 0x151c90.

## 2026-10-06: 6564 functions match

```
matched 6564 of 11318 game functions (707829 of 2784283 bytes, 25.42%)
```

6564 new matches, none lost:
- Codex UI-core lane rounds 5 and 6: 27 matches in the UI core (0x22d0e3 to 0x23c0e0, plus 0x1489b5 and 0x148c3e).

## 2026-10-06: 6537 functions match

```
matched 6537 of 11318 game functions (704840 of 2784283 bytes, 25.31%)
```

6537 new matches, none lost:
- Codex UI-screens lane round 6: 0x2b635f, 0x2b9670, 0x2be650, 0x2c5224 and 0x2c52af.
- Codex lane D round 25: 0x53610 and 0x56b70.

## 2026-10-06: 6530 functions match

```
matched 6530 of 11318 game functions (703949 of 2784283 bytes, 25.28%)
```

6530 new matches, none lost:
- Merge batch r7: the second machine's lane C rounds 20 and 21 (+27; PRs #106, #108), lane L rounds 2 and 3 (+6 including 0x2155f4; #105, #107), and written source for lanes O and J (#100, #103).

## 2026-10-06: 6497 functions match

```
matched 6497 of 11318 game functions (701247 of 2784283 bytes, 25.19%)
```

6497 new matches, none lost:
- Codex lane D round 24: 0x68f30, 0x69880, 0x69c80, 0x69d50, 0x6f700 and 0x860b0.
- Codex lane AB round 7: 0xb75a0 and 0xb8460.

## 2026-10-06: 6489 functions match

```
matched 6489 of 11318 game functions (699767 of 2784283 bytes, 25.13%)
```

6489 new matches, none lost:
- Codex lane Z round 8: four more event senders (0xa8c10, 0xa8cf0, 0xa8dd0, 0xa9120).

## 2026-10-06: 6485 functions match

```
matched 6485 of 11318 game functions (699052 of 2784283 bytes, 25.11%)
```

6485 new matches, none lost:
- Codex lane D round 23: 0x5d9e0, 0x68670, 0x69040, 0x6f940, 0x84fb0 and 0x86aa0, plus the wrappers 0xb2710 and 0xb2730.

## 2026-10-06: 6477 functions match

```
matched 6477 of 11318 game functions (698071 of 2784283 bytes, 25.07%)
```

6477 new matches, none lost:
- Codex lane Z rounds 6 and 7: 17 network event senders (0xa7c50 to 0xa9400), once 0xa76b0 and 0xa5980 kept retail's call boundaries.

## 2026-10-06: 6460 functions match

```
matched 6460 of 11318 game functions (695856 of 2784283 bytes, 24.99%)
```

6460 new matches, none lost:
- Merge batch r6: the second machine's PRs #98 (lane O round 6: 0x244610, 0x249fa3, 0x24a80d), #97 and #99 (lane AA's last functions, written), #76 (lane K) and #82 (lane I), the last four adding written source only.

## 2026-10-06: 6457 functions match

```
matched 6457 of 11318 game functions (695776 of 2784283 bytes, 24.99%)
```

6457 new matches, none lost:
- Codex lane N round 5: 0x14c320, 0x14c540 and 0x14c630, and five more through them: 0xa94b0, 0x152cf0 and 0x158090 (callers of 0x14cad0, whose parameters it fixed) and 0x24f850 and 0x24f880.

## 2026-10-06: 6449 functions match

```
matched 6449 of 11318 game functions (693834 of 2784283 bytes, 24.92%)
```

6449 new matches, none lost:
- Codex lane X round 5: 0x1e17d0, 0x1e2150, 0x1e3400 and 0x1e4570, and lane V's 0x26abd0 through 0x1e2150's argument order.
- Codex lane T round 7: 0x16f570.

## 2026-10-06: 6443 functions match

```
matched 6443 of 11318 game functions (690371 of 2784283 bytes, 24.80%)
```

6443 new matches, none lost:
- Merge batch r5: the second machine's Codex PRs #73, #74, #75, #77, #78, #79, #80, #81 and #83 (lanes O, M, I, P, L and J): +15 that main didn't already have.

## 2026-10-06: 6428 functions match

```
matched 6428 of 11318 game functions (686770 of 2784283 bytes, 24.67%)
```

6428 new matches, none lost:
- Codex lane W round 12: 0x18ee0 and 0x36560.
- Codex lane U round 10: 0x20d220 and 0x20e460.

## 2026-10-06: 6424 functions match

```
matched 6424 of 11318 game functions (684019 of 2784283 bytes, 24.57%)
```

6424 new matches, none lost:
- Codex lane D round 22: 0x6c4a0 and 0x84560.

## 2026-10-06: 6422 functions match

```
matched 6422 of 11318 game functions (683641 of 2784283 bytes, 24.55%)
```

6422 new matches, none lost:
- Codex lane C round 19: 0x1d05d0.

## 2026-10-06: 6421 functions match

```
matched 6421 of 11318 game functions (683406 of 2784283 bytes, 24.55%)
```

6421 new matches, none lost:
- Codex lane V round 6: 0x268700, 0x2691b0, 0x269de0 and 0x26a480.

## 2026-10-06: 6417 functions match

```
matched 6417 of 11318 game functions (682868 of 2784283 bytes, 24.53%)
```

6417 new matches, none lost:
- Codex lane Q round 5: 0x155b60, 0x155c60 and 0x155d80.
- Codex lane FP round 1: the inline and call boundaries around 0x214ac0 and 0x214b80 are now explicit, so those two no longer depend on inlining choices elsewhere.

## 2026-10-06: 6414 functions match

```
matched 6414 of 11318 game functions (682310 of 2784283 bytes, 24.51%)
```

6414 new matches, none lost:
- Codex lane AB round 6: 0xb6df0, 0xb7150 and 0xc1670.
- Codex near-match polish round 2: 0x2566c0.
- @coldspear's #96: retail's values for the constants, string and script definitions from #90 and #91.

## 2026-10-06: 6410 functions match

```
matched 6410 of 11318 game functions (681349 of 2784283 bytes, 24.47%)
```

6410 new matches, none lost:
- Codex near-match polish round 1: 0x3d4f0, 0x3f220, 0x191300 and 0x28c470, four near matches finished by small rewrites.

## 2026-10-06: 6406 functions match

```
matched 6406 of 11318 game functions (680567 of 2784283 bytes, 24.44%)
```

6406 new matches, none lost:
- Codex lane V round 5: 10 matches in 0x260000-0x26e36f, plus 0x1a9e00 and 0x1b3820, whose stub callees became real code.

## 2026-10-06: 6394 functions match

```
matched 6394 of 11318 game functions (678059 of 2784283 bytes, 24.35%)
```

6394 new matches, none lost:
- Codex lane AB round 5: 8 matches in the object core (0xb7300, 0xb8840, 0xb8890, 0xbd020, 0xbe690, 0xc00a0, 0xc01c0, 0xc3f90).
- Codex lane Y round 6: 0x74970.

## 2026-10-06: 6385 functions match

```
matched 6385 of 11318 game functions (677037 of 2784283 bytes, 24.32%)
```

7 new matches, none lost:
- **Codex lane D**, round 21 (4): session and network helpers in the 0x50000–0x6ffff and 0x80000–0x8ffff ranges, plus lane J's 0x94100 through a now-real callee.
- **Codex lane AA**, round 2 (3): unit and object helpers in the 0x110000–0x11ffff range.

## 2026-10-06: 6378 functions match

```
matched 6378 of 11318 game functions (676266 of 2784283 bytes, 24.29%)
```

4 new matches, none lost:
- **Codex lane U**, round 9 (4): squad and image helpers in the 0x200000–0x217fff range.

## 2026-10-06: 6374 functions match

```
matched 6374 of 11318 game functions (675720 of 2784283 bytes, 24.27%)
```

3 new matches, none lost:
- **Codex lane C**, round 18 (3): actor and havok helpers in the 0x1c0000–0x1dffff range.

## 2026-10-06: 6371 functions match

```
matched 6371 of 11318 game functions (675272 of 2784283 bytes, 24.25%)
```

32 new matches, none lost:
- **Codex lane AA**, round 1 (25): unit, weapon, object attachment and physics helpers in the new 0x110000–0x11ffff range; its corrected prototypes and new callees also make @Banshee64's 0xce040 and the existing 0xeb5a0 match.
- **Codex lane P**, round 2 (7): three string and list helpers in the 0x130000–0x13ffff range, and four lane A callers that now match through them.

## 2026-10-06: 6339 functions match

```
matched 6339 of 11318 game functions (670909 of 2784283 bytes, 24.10%)
```

7 new matches, none lost:
- **Codex lane D**, round 20 (7): friend presence, matchmaking and voice queue helpers in the 0x50000–0x6ffff and 0x80000–0x8ffff ranges, plus two callers that now match (0x943d0, 0x2359ce).

## 2026-10-06: 6332 functions match

```
matched 6332 of 11318 game functions (668717 of 2784283 bytes, 24.02%)
```

11 new matches, none lost:
- **Codex lane W**, round 11 (1): one more core utility.
- **Codex lane F** (10): player control and looping-sound helpers in the 0x180000–0x18ffff range, and lane K's 0x21db80 through its now-real callee.

## 2026-10-06: 6321 functions match

```
matched 6321 of 11318 game functions (667886 of 2784283 bytes, 23.99%)
```

1 new matches, none lost:
- **UI-core lane**, round 4 (1): a director camera helper at 0x23d790.

## 2026-10-06: 6320 functions match

```
matched 6320 of 11318 game functions (667562 of 2784283 bytes, 23.98%)
```

5 new matches, none lost:
- **Codex lane AB**, round 4 (5): object connection, cluster and light helpers in the 0x0b3d30–0x0c42df range.

## 2026-10-06: 6315 functions match

```
matched 6315 of 11318 game functions (666928 of 2784283 bytes, 23.95%)
```

9 new matches, none lost:
- **Codex lane D**, round 19 (8): network voice, observer and session join-state helpers in the 0x50000–0x6ffff and 0x80000–0x8ffff ranges.
- **Codex lane B**, round 9 (1): an actor helper in the 0x1b0000–0x1bffff range.

## 2026-10-06: 6306 functions match

```
matched 6306 of 11318 game functions (664864 of 2784283 bytes, 23.88%)
```

12 new matches, none lost:
- **Codex lane R** (2): an effect helper in the 0x170000–0x17ffff range, and its now-matching caller 0x16f190.
- **Codex lane Y**, round 5 (10): six session and network-estimation helpers, and four codec functions that now match through them.

## 2026-10-06: 6294 functions match

```
matched 6294 of 11318 game functions (659320 of 2784283 bytes, 23.68%)
```

4 new matches, none lost:
- **Codex lane U**, round 8 (4): game variant, saved-game and image helpers in the 0x200000–0x217fff range.

## 2026-10-06: 6290 functions match

```
matched 6290 of 11318 game functions (658972 of 2784283 bytes, 23.67%)
```

10 new matches, none lost:
- **Codex lane X**, round 4 (7): actor, prop and shape helpers in the 0x1e0000–0x1effff range.
- **Codex lane C**, round 17 (3): cluster and object list helpers in the 0x1c0000–0x1dffff range.

## 2026-10-06: 6280 functions match

```
matched 6280 of 11318 game functions (657692 of 2784283 bytes, 23.62%)
```

6 new matches, none lost:
- **Codex lane J** (4): network stream and session search helpers in the 0x90000–0x9ffff range.
- **Codex lane L** (2): a font helper in the 0x120000–0x12ffff range, and its caller 0x22d7ce.

## 2026-10-06: 6274 functions match

```
matched 6274 of 11318 game functions (655763 of 2784283 bytes, 23.55%)
```

10 new matches, none lost:
- **Codex lane P** (8): team colours, palette, string and HUD helpers in the 0x130000–0x13ffff range, plus 0x7f720 and 0x7f790; a corrected prototype also makes UI functions 0x2c0400 and 0x24ce9d match.

## 2026-10-06: 6264 functions match

```
matched 6264 of 11318 game functions (653773 of 2784283 bytes, 23.48%)
```

3 new matches, none lost:
- **Codex lane S**, round 6 (3): weapon, device and item helpers in the 0x100000–0x10ffff range.

## 2026-10-06: 6261 functions match

```
matched 6261 of 11318 game functions (652805 of 2784283 bytes, 23.45%)
```

6 new matches, none lost:
- **Codex lane AC**, round 3 (6): interface constructors and callbacks in the 0x2729b0–0x29ffff range.

## 2026-10-06: 6255 functions match

```
matched 6255 of 11318 game functions (650905 of 2784283 bytes, 23.38%)
```

2 new matches, none lost:
- **Codex lane W**, round 10 (2): two more core utilities in the 0x11000–0x4ffff range.

## 2026-10-06: 6253 functions match

```
matched 6253 of 11318 game functions (650189 of 2784283 bytes, 23.35%)
```

11 new matches, none lost:
- **Codex lane Z**, round 4 (4): more object type and event helpers in the 0xa0000–0xac48f range.
- **Codex lane V**, round 4 (7): path and clump helpers in the 0x260000–0x26e36f range.

## 2026-10-06: 6242 functions match

```
matched 6242 of 11318 game functions (647596 of 2784283 bytes, 23.26%)
```

18 new matches, none lost:
- **Codex lane AC**, round 2 (18): object-chain flags, groups and interface helpers in the 0x2729b0–0x29ffff range.

## 2026-10-06: 6224 functions match

```
matched 6224 of 11318 game functions (644727 of 2784283 bytes, 23.16%)
```

4 new matches, none lost:
- **Codex lane B**, round 8 (4): actor slot handlers in the 0x1b0000–0x1bffff and 0x1f0000–0x1fffff ranges.

## 2026-10-05: 6220 functions match

```
matched 6220 of 11318 game functions (643669 of 2784283 bytes, 23.12%)
```

7 new matches, none lost:
- **Codex lane Q**, round 4 (7): player pickup, weapon and HUD helpers in the 0x150000–0x15ffff range.

## 2026-10-05: 6213 functions match

```
matched 6213 of 11318 game functions (642366 of 2784283 bytes, 23.07%)
```

10 new matches, none lost:
- **Codex lane D**, round 18 (10): network session, voice and presence helpers in the 0x50000–0x6ffff and 0x80000–0x8ffff ranges.

## 2026-10-05: 6203 functions match

```
matched 6203 of 11318 game functions (641123 of 2784283 bytes, 23.03%)
```

25 new matches, none lost:
- **Codex lane AC**, round 1 (25): interface destructors, tracking hooks, iterators and actor helpers in the new 0x2729b0–0x29ffff range.

## 2026-10-05: 6178 functions match

```
matched 6178 of 11318 game functions (639613 of 2784283 bytes, 22.97%)
```

3 new matches, none lost:
- **Codex lane U**, round 7 (3): two more helpers in the 0x200000–0x217fff range, and a prototype fix that makes lane B's 0x1bcc90 match.

## 2026-10-05: 6175 functions match

```
matched 6175 of 11318 game functions (638810 of 2784283 bytes, 22.94%)
```

8 new matches, none lost:
- **Codex lane Y**, round 4 (8): session and gateway stream helpers in the 0x70000–0x7ffff range.

## 2026-10-05: 6167 functions match

```
matched 6167 of 11318 game functions (637310 of 2784283 bytes, 22.89%)
```

3 new matches, none lost:
- **Codex lane C**, round 16 (3): havok component and actor helpers in the 0x1c0000–0x1dffff range.

## 2026-10-05: 6164 functions match

```
matched 6164 of 11318 game functions (636418 of 2784283 bytes, 22.86%)
```

9 new matches, none lost:
- **Codex lane W**, round 9 (9): more core utilities in the 0x11000–0x4ffff range.

## 2026-10-05: 6155 functions match

```
matched 6155 of 11318 game functions (634827 of 2784283 bytes, 22.80%)
```

14 new matches, none lost:
- **Codex lane Z**, round 3 (14): object creation codecs, event definitions and device helpers in the 0xa0000–0xac48f range.

## 2026-10-05: 6141 functions match

```
matched 6141 of 11318 game functions (630928 of 2784283 bytes, 22.66%)
```

11 new matches, none lost:
- **Codex lane AB**, round 3 (11): more object core iterators, storage and light helpers; one lane O function (0x240fe0) now matches too.

## 2026-10-05: 6130 functions match

```
matched 6130 of 11318 game functions (629567 of 2784283 bytes, 22.61%)
```

10 new matches, none lost:
- **Codex lane U**, round 6 (10): actor and squad iterators, script and widget helpers in the 0x200000–0x217fff range; its callee 0x216120 also makes two UI functions (0x2380a6, 0x2380c0) match.

## 2026-10-05: 6120 functions match

```
matched 6120 of 11318 game functions (628337 of 2784283 bytes, 22.57%)
```

10 new matches, none lost:
- **Codex lane W**, round 8 (10): more core utilities in the 0x11000–0x4ffff range, including the 0x43850 callback with its shared type fixed.

## 2026-10-05: 6110 functions match

```
matched 6110 of 11318 game functions (625818 of 2784283 bytes, 22.48%)
```

12 new matches, none lost:
- **Codex lane Y**, round 3 (12): session search registration, player caches and network estimation helpers in the 0x70000–0x7ffff range.

## 2026-10-05: 6098 functions match

```
matched 6098 of 11318 game functions (622981 of 2784283 bytes, 22.37%)
```

21 new matches, none lost:
- **Codex lane Y**, round 2 (21): game-session state, network bandwidth estimation and observer helpers in the 0x70000–0x7ffff range.

## 2026-10-05: 6077 functions match

```
matched 6077 of 11318 game functions (619215 of 2784283 bytes, 22.24%)
```

14 new matches, none lost:
- **Codex lane AB**, round 2 (14): more of the object core, plus four round-1 functions finished.

## 2026-10-05: 6063 functions match

```
matched 6063 of 11318 game functions (617294 of 2784283 bytes, 22.17%)
```

16 new matches, none lost:
- **Codex lane W**, round 7 (8): more core utilities, and its new callee makes @Banshee64's 0x117060 match.
- **Codex lane C**, round 15 (8): AI, actor and havok helpers in the 0x1c0000–0x1dffff range; one lane A caller (0x2a10d0) now matches too.

## 2026-10-05: 6047 functions match

```
matched 6047 of 11318 game functions (614962 of 2784283 bytes, 22.09%)
```

45 new matches, none lost:
- **Codex lane AB**, round 1 (43): the object core (object defaults, velocities, deletion, map connection, cluster iterators, havok components) and lights and liquids, using @coldspear's analysis documents (#47, #61, #68) as the map.
- **Status fixes** (2): 0x240290 and 0x2432e0 already had matching source; their rows are now recorded.

## 2026-10-05: 6002 functions match

```
matched 6002 of 11318 game functions (611428 of 2784283 bytes, 21.96%)
```

20 new matches, none lost:
- **Codex lane W**, round 6 (20): more core utilities in the 0x11000–0x4ffff range; its new 0x47870 also makes @Banshee64's flexible chain function 0x116080 match.

## 2026-10-05: 5982 functions match

```
matched 5982 of 11318 game functions (608994 of 2784283 bytes, 21.87%)
```

14 new matches, none lost:
- **Codex lane Z**, round 2 (14): more game event and object creation codecs in the 0xa0000–0xac48f range.

## 2026-10-05: 5968 functions match

```
matched 5968 of 11318 game functions (605855 of 2784283 bytes, 21.76%)
```

12 new matches, none lost:
- **Codex lane U**, round 5 (12): squad and script helpers, pulse timing and vehicle rate integration in the 0x200000–0x217fff range.

## 2026-10-05: 5956 functions match

```
matched 5956 of 11318 game functions (603265 of 2784283 bytes, 21.67%)
```

17 new matches, none lost:
- **Codex lanes from the second machine** (17): lane O round 3 (12), lane M round 3 (3) and lane K round 3 (2); lane F round 3 wrote 5 functions that don't match yet.
- **Docs:** @coldspear's object core analysis, parts 1 and 2 (#47, #61), lights and liquids analysis (#68), and a synthetic link-map test fixture (#46).

## 2026-10-05: 5939 functions match

```
matched 5939 of 11318 game functions (600381 of 2784283 bytes, 21.56%)
```

49 new matches, none lost:
- **Codex lane Z**, round 1 (49): game event definitions, game engine entity definitions, device and item helpers in the new 0xa0000–0xac48f range.

## 2026-10-05: 5890 functions match

```
matched 5890 of 11318 game functions (596443 of 2784283 bytes, 21.42%)
```

22 new matches, none lost:
- **Codex lane W**, round 5 (22): more core utilities in the 0x11000–0x4ffff range.

## 2026-10-05: 5868 functions match

```
matched 5868 of 11318 game functions (594072 of 2784283 bytes, 21.34%)
```

20 new matches, none lost:
- **Codex lane Y**, round 1 (18): game-session state helpers and network bandwidth code in the new 0x70000–0x7ffff range.
- **Codex lane X**, round 3 (2): two more shape helpers.

## 2026-10-05: 5848 functions match

```
matched 5848 of 11318 game functions (591684 of 2784283 bytes, 21.25%)
```

37 new matches, none lost:
- **@Banshee64** (15): two marker accessors (#54), the online result cache and address registration (#58), and path transition geometry (#60).
- **Codex lanes from the second machine** (22): lane O round 2 (12), lane M rounds 1–2 (3), lane K round 2 (2) and lane F round 2 (5).
- **Docs:** @BirchWoodGod's session search analysis (#52) and observer field names (#53).

## 2026-10-05: 5811 functions match

```
matched 5811 of 11318 game functions (586075 of 2784283 bytes, 21.05%)
```

26 new matches, none lost:
- **Codex lane W**, round 4 (21): more core utilities in the 0x11000–0x4ffff range.
- **Codex lane U**, round 4 (5): more of the 0x200000–0x217fff range.

## 2026-10-05: 5785 functions match

```
matched 5785 of 11318 game functions (583882 of 2784283 bytes, 20.97%)
```

8 new matches, none lost:
- **Codex lane V**, round 3 (8): more of the 0x260000–0x26e36f range.

## 2026-10-05: 5777 functions match

```
matched 5777 of 11318 game functions (582240 of 2784283 bytes, 20.91%)
```

29 new matches, none lost:
- **Codex lane X**, round 2 (29): unit, biped and damage helpers and shape geometry in the 0x1e0000–0x1effff range.

## 2026-10-05: 5748 functions match

```
matched 5748 of 11318 game functions (579184 of 2784283 bytes, 20.80%)
```

33 new matches, none lost:
- **Codex lane U**, round 3 (11): input preferences, storage, and cached locations.
- **Codex lane W**, round 3 (22): core utilities in the 0x2f000–0x4efff range.

## 2026-10-05: 5715 functions match

```
matched 5715 of 11318 game functions (575151 of 2784283 bytes, 20.66%)
```

**The second machine's Codex lanes**: 12 new matches, none lost.
- **Lane F** (4): player control and sound portals.
- **Lane K** (3): the sound driver and impacts.
- **Lane O** (5).

The lanes were re-merged under the project's no-reply identity, and two of their identifiers were renamed in the provenance rescan.

## 2026-10-05: 5703 functions match

```
matched 5703 of 11318 game functions (573097 of 2784283 bytes, 20.58%)
```

32 new matches, none lost:
- **Codex lane W**, round 2 (12): core utilities, visibility slots, and a shader fallback table.
- **Codex lane X**, round 1 (20), a newly opened region (0x1e0000–0x1effff): AI and actor support.

## 2026-10-05: 5671 functions match

```
matched 5671 of 11318 game functions (568668 of 2784283 bytes, 20.42%)
```

**Codex lane V**, round 2: 9 new matches, none lost. They cover prop wrappers, hash tables, location entries and record-sector maps.

## 2026-10-05: 5662 functions match

```
matched 5662 of 11318 game functions (567386 of 2784283 bytes, 20.38%)
```

**Codex lane U**, round 2: 15 new matches, none lost. They cover storage requests, cached locations, audio queue nodes, and memory-source helpers.

## 2026-10-05: provenance clean-up, part 3

A rescan of the whole tree, now extended to comments and documents, renamed 18 source and header files whose names matched non-permitted sources. They became `unknown_<lowest address>` files. Includes, guards and the function inventory were updated to match. No match status changed.

## 2026-10-05: 5647 functions match

```
matched 5647 of 11318 game functions (566269 of 2784283 bytes, 20.34%)
```

**Codex lane W**, round 1: 22 new matches, none lost. The region, 0x011000–0x04ffff, is newly opened. It covers low-level core code: allocators and arenas, data structures and utilities.

## 2026-10-05: 5625 functions match

```
matched 5625 of 11318 game functions (564812 of 2784283 bytes, 20.29%)
```

**Codex lane V**, round 1: 23 new matches, none lost. The region, 0x260000–0x26e36f, is newly opened. The work covers props, clump iteration, AI state callbacks and a rebuilt callback table.

## 2026-10-05: 5602 functions match

```
matched 5602 of 11318 game functions (562650 of 2784283 bytes, 20.21%)
```

**Contributor pull requests**, merged after independent review: 30 new matches, none lost.
- @Banshee64: the unit object type (#33, 22 matches), cloth simulation (#48), flexible chain callbacks (#50, 7 matches), and 0x23e340's seventh argument (#51).
- @coldspear: synthetic class names in the linkmap tests (#45).

**Provenance clean-up.** A rescan found identifiers that recent merges had brought in and that match non-permitted sources exactly. 40 names were renamed to behavioural names or placeholders. The rescan now also covers names that appear only in comments and docs.

## 2026-10-05: 5572 functions match

```
matched 5572 of 11318 game functions (560032 of 2784283 bytes, 20.11%)
```

**Codex lane U**, round 1: 33 new matches, none lost. This is a newly opened region (0x200000–0x217fff) covering AI squads and memory sources, along with a dozen smaller groups.

## 2026-10-05: 5539 functions match

```
matched 5539 of 11318 game functions (557648 of 2784283 bytes, 20.03%)
```

**Codex lane D**, round 17: 4 new matches, none lost. They cover voice packet submission, the simulation-world replication reset, a session helper, and connection initialisation.

## 2026-10-05: 5535 functions match

```
matched 5535 of 11318 game functions (557104 of 2784283 bytes, 20.01%)
```

**Codex lane A**, round 17: 4 new matches, none lost. They are AI-script actor helpers and saved-game file tasks.

## 2026-10-05: 5531 functions match

```
matched 5531 of 11318 game functions (556335 of 2784283 bytes, 19.98%)
```

**Codex lane B**, round 7: 5 new matches, none lost. They are actor slot handlers and their AI support. One new function's file broke two functions in another lane under LTCG, so the function was folded into an existing file with the same flags.

## 2026-10-05: 5526 functions match

```
matched 5526 of 11318 game functions (555509 of 2784283 bytes, 19.95%)
```

**Codex lane A**, round 16: 8 new matches, none lost. They include the progress screen's remaining method, the flock helpers, an AI-script object list, and two script evaluators.

## 2026-10-05: 5518 functions match

```
matched 5518 of 11318 game functions (554367 of 2784283 bytes, 19.91%)
```

**Codex lane N**, round 4: 4 new matches, none lost. They cover cross products, structure cluster bit vectors, and a screen helper.

## 2026-10-05: 5514 functions match

```
matched 5514 of 11318 game functions (553807 of 2784283 bytes, 19.89%)
```

**Codex UI-screens lane**, round 2: 4 new matches, none lost. They are a territories update, the start of the avoidance search, a ball item-position callback, and a score update.

## 2026-10-05: 5510 functions match

```
matched 5510 of 11318 game functions (553146 of 2784283 bytes, 19.87%)
```

**Codex lane T**, round 6: 7 new matches, none lost. They cover scoreboard and score queries, loading-screen helpers, prediction clusters, and the cache-slot ordering.

## 2026-10-05: 5503 functions match

```
matched 5503 of 11318 game functions (552195 of 2784283 bytes, 19.83%)
```

**Codex lane A**, round 15: 12 new matches, none lost. Most are the progress screen's class; the rest are saved-game file helpers and the streamed-sound update.

## 2026-10-05: 5491 functions match

```
matched 5491 of 11318 game functions (551269 of 2784283 bytes, 19.80%)
```

**Codex lane D**, round 16: 9 new matches, none lost. They cover voice modes and status text, session helpers, connection allocation, and replication.

## 2026-10-05: 5482 functions match

```
matched 5482 of 11318 game functions (549732 of 2784283 bytes, 19.74%)
```

**Codex lanes S (round 5) and Q (round 3)**: 8 new matches, none lost. They cover weapon and item helpers and a game-engine helper; three callers in other lanes also match now that their callees are real code.

Reading a shared global through a `volatile` cast takes its address. Under LTCG that stops load hoisting in every function that uses the global, so the verifier rejected the one edit that did it.

## 2026-10-05: 5474 functions match

```
matched 5474 of 11318 game functions (549106 of 2784283 bytes, 19.72%)
```

**Codex lane A**, round 14: 9 new matches, none lost. Four are script built-in evaluators; the other five are scenario flock and AI-script helpers.

## 2026-10-05: 5465 functions match

```
matched 5465 of 11318 game functions (548252 of 2784283 bytes, 19.69%)
```

**Codex lane D**, round 15: 13 new matches, none lost. They cover session parameters, voice, network streams and payloads, and connection helpers.

## 2026-10-05: 5452 functions match

```
matched 5452 of 11318 game functions (547125 of 2784283 bytes, 19.65%)
```

**Codex lane T**, round 5: 4 new matches, none lost. They are a resource-type switch, two cache-copy helpers and a slot selector.

## 2026-10-05: 5448 functions match

```
matched 5448 of 11318 game functions (546434 of 2784283 bytes, 19.63%)
```

**Codex lane C**, round 14: 3 new matches, none lost. They are an animation flag reader, its caller in the weapons code, and a Havok translation helper.

## 2026-10-05: 5445 functions match

```
matched 5445 of 11318 game functions (545940 of 2784283 bytes, 19.61%)
```

**Codex UI-core lane**: 8 new matches, none lost. Two of them are the function rows split by the never-returning-call change. The others are window-manager widget helpers, a lobby handler, and a lane H caller.

## 2026-10-05: 5437 functions match

```
matched 5437 of 11318 game functions (544930 of 2784283 bytes, 19.57%)
```

**Codex lane C**, round 13: 5 new matches, none lost. They cover rotation unpacking, an animation field setter, a marker lookup, a node count and a small accessor.

## 2026-10-05: 5432 functions match

```
matched 5432 of 11318 game functions (544488 of 2784283 bytes, 19.56%)
```

**Codex lane D**, round 14: 27 new matches, none lost.
- **0x641a0 written and 0x58d90 isolated.** 0x58d90 now sits in its own file, which fixes the register its callers use; six of them now match.
- **Voice and connection helpers.**
- **Callers in other lanes:** two in lane H and two UI callers.

## 2026-10-05: 5405 functions match

```
matched 5405 of 11318 game functions (541881 of 2784283 bytes, 19.46%)
```

**Codex UI-screens lane**: 4 new matches, none lost. They are an obstacle-avoidance pass, the territories score update, the hill's spawn influences and a profile query. The rest of the territories chain is written but not yet matching.

## 2026-10-05: 5401 functions match

```
matched 5401 of 11318 game functions (540582 of 2784283 bytes, 19.42%)
```

**Codex lane T**, round 4: 13 new matches, none lost. Most cover cache-file copying and map-slot bookkeeping; there are also a frustum sphere classifier and two first-person helpers.

## 2026-10-05: 5388 functions match

```
matched 5388 of 11318 game functions (539462 of 2784283 bytes, 19.38%)
```

**Codex lane R**, round 3: 6 new matches, none lost. They are decal cell lists and links, particle counts and callbacks, and the decal sequence picker.

## 2026-10-05: 5382 functions match

```
matched 5382 of 11318 game functions (538516 of 2784283 bytes, 19.34%)
```

**Lane H**, round 9: 20 new matches, none lost. The work covers widgets, dialogs and game variant code. Two UI functions also match now: one helper is kept out of line, and another value is kept on the stack by taking its address.

## 2026-10-05: 5362 functions match

```
matched 5362 of 11318 game functions (535967 of 2784283 bytes, 19.25%)
```

**Lane D**, round 13: 48 new matches, none lost. Most are network voice functions, including the voice state re-reads; the rest are session-manager code and host requests. Fixing the session calling conventions also matched one function in lane H and one in UI-core.

## 2026-10-05: 5314 functions match

```
matched 5314 of 11318 game functions (530033 of 2784283 bytes, 19.04%)
```

**Codex lane N**, round 3: 8 new matches, none lost. They cover screen event handling, a music-silence query and two position helpers. UI-core's 0x23d030 also matches now, thanks to an `/Ob1` file flag.

## 2026-10-05: 5306 functions match

```
matched 5306 of 11318 game functions (529115 of 2784283 bytes, 19.00%)
```

**Codex lane O**, round 3: 5 new matches, none lost. They cover particle emitter transforms, a list refresh, a HUD marker test and direction encoding.

## 2026-10-05: 5301 functions match

```
matched 5301 of 11318 game functions (528478 of 2784283 bytes, 18.98%)
```

**Codex lanes M and J**: 3 new matches (0x2161d0, 0x1a3003 and 0x1a301f), none lost. Lane J's round adds written network-event and session code that does not match yet.

## 2026-10-05: 5298 functions match

```
matched 5298 of 11318 game functions (528316 of 2784283 bytes, 18.97%)
```

**Codex lane Q**, round 2: 8 new matches, none lost. They cover game-engine callbacks, player-change notifications, a label lookup, a proximity query and the round-time text, plus that text's caller in the UI.

## 2026-10-05: 5290 functions match

```
matched 5290 of 11318 game functions (527350 of 2784283 bytes, 18.94%)
```

**The UI-core lane**, stint 2: 18 new matches, none lost. They include the widget item constructor and more of the lobby, matchmaking and screen code.

## 2026-10-05: 5272 functions match

```
matched 5272 of 11318 game functions (525065 of 2784283 bytes, 18.86%)
```

**Codex lane P**, round 3: 6 new matches, none lost. They cover scenario interpolators, the HUD fade and a font escape table. 0x7f720's team argument is now `long`, as its callers show.

## 2026-10-05: 5266 functions match

```
matched 5266 of 11318 game functions (524276 of 2784283 bytes, 18.83%)
```

**Codex lane Q**, round 1: 7 new matches, none lost. Four are game-engine helpers in its region. The other three are UI-screens' ball and hill handlers, which now match because lane Q fixed the calling conventions of the callees they depend on.

## 2026-10-05: tooling, function boundaries

```
matched 5259 of 11318 game functions (523598 of 2784283 bytes, 18.81%)
```

- **Game code now ends at 0x2cb510.** The four functions above it are zlib from the SDK's D3DX library.
- **Function discovery recognises functions that never return.** A call to one ends the caller, so two rows that ran into the next function are now split. One misdetected start is excluded.
- **New marker for compiler-generated vcall thunks** (`// @retail 0x<va> vcall <offset>`). Its first use matches 0x234c5f.

## 2026-10-05: 5258 functions match

```
matched 5258 of 11321 game functions (523594 of 2785198 bytes, 18.80%)
```

34 new matches, none lost:
- **The UI-core lane**, stint 1 (31): the window manager, cameras, event sounds, the UI heap, and the lobby and matchmaking screens.
- **Codex lane J**, round 5 (3): 0x92450, 0x9b180 and 0x9bf30.

## 2026-10-05: 5224 functions match

```
matched 5224 of 11321 game functions (517738 of 2785198 bytes, 18.59%)
```

**The UI-screens lane**, stints 1 and 2: 44 new matches, none lost. They include the passcode and emblem screens, the ball, hill and territories game-engine handlers, the ground-obstacle list and avoidance heap, and the recorded unit-control reader.

## 2026-10-05: 5180 functions match

```
matched 5180 of 11321 game functions (510156 of 2785198 bytes, 18.32%)
```

**The first two Codex lanes:** 11 new matches, none lost. Codex wrote the code; a Claude agent checked and committed it.
- **Lane J (network handle tables):** 0x995c0, 0x99690 and their callers. This also completed two of lane D's functions.
- **Lane L (preference getters and font loading):** 0x121100, 0x1210a0, 0x1223a0, and the UI caller 0x2bba01.

## 2026-10-05: 5169 functions match

```
matched 5169 of 11321 game functions (508526 of 2785198 bytes, 18.26%)
```

42 new matches, none lost:
- **Lane D**, round 12: network voice, session and observer code.
- **Lane H**, round 8: input, widgets and the game variant code. 0x19a76d now matches, which finally settles its calling convention.
- **The first Codex-assisted commit:** four near misses fixed (0x1b7900, 0x1bba20, 0x223b20 and 0x248d50).

## 2026-10-05: 5127 functions match

```
matched 5127 of 11321 game functions (503104 of 2785198 bytes, 18.06%)
```

**Contributor pull requests**, merged after independent review:
- @Banshee64: bipeds (#28), Euler-vector helpers (#38), vehicle SSE tuning (#40) and the cached language getter (#42);
- @coldspear: a decomp.dev objdiff export (#30), `tools/near.py` (#32) and follow-ups to earlier tooling (#41);
- @BirchWoodGod: an analysis of the network bandwidth controller (#44).

**Lane S**, round 4: weapons, devices, scenery and items.

Together: 29 new matches, none lost. Leftover include guards from the provenance renames now follow their file names.

## 2026-10-05: 5098 functions match

```
matched 5098 of 11321 game functions (499631 of 2785198 bytes, 17.94%)
```

**Lanes C (round 12) and A (round 13)**, merged together: 22 new matches, none
lost. They include render model marker lookups, actor masks, object-tree
resource prediction and script evaluators.

## 2026-10-04: 5076 functions match

```
matched 5076 of 11321 game functions (497600 of 2785198 bytes, 17.87%)
```

**The UI lane**, round 14: 14 new matches, none lost. They include game engine
handlers, playlist lists and clan member data.

## 2026-10-04: provenance clean-up, part 2

Identifiers and file names that matched non-permitted sources exactly are
renamed: 703 identifiers, mostly to address-based placeholders such as
`function_<va>` and `field_<offset>`, with descriptive names for the most
used ones (`point3f`, `string_handle`, `s_record_pool`), and 120 source files
to `unknown_<va>`. Generic idioms and single common words are kept.
[PROVENANCE.md](../PROVENANCE.md) describes the method. The match count is
unchanged at 5062, and no function changed status.

## 2026-10-04: provenance clean-up, part 1

The `name` and `object` columns of `config/functions.csv`, which came from a
third-party dataset, are now empty. Library functions are named instead by
the project's own byte-signature matching against the contributor's SDK
libraries. The tools no longer read that dataset. Comments and documents that
cited unreleased builds as a source have been revised. The match count is
unchanged at 5062. The next step renames identifiers whose names came from
those sources; see [PROVENANCE.md](../PROVENANCE.md).

## 2026-10-04: 5062 functions match; legal notice and provenance policy

```
matched 5062 of 11321 game functions (496330 of 2785198 bytes, 17.82%)
```

**Project policy.**
- New [LEGAL.md](../LEGAL.md) and [PROVENANCE.md](../PROVENANCE.md) set out the
  project's scope: it is independent, non-commercial preservation and
  research, it does not contain game files, executables or SDK files, and
  users supply their own lawfully owned copy.
- They also state the contribution rule: contributions must not contain or
  copy material from leaked or internal sources.
- Pull requests now include a provenance checklist.
- Name data that came from a third-party dataset derived from unreleased
  builds' linker maps is being removed, as PROVENANCE.md describes.

**Progress.** 58 new matches, none lost:
- **Lane L**, round 7, and **the UI lane**, round 13 (the dashboard dialog
  callbacks and more);
- **a build fix:** the compiler's link-time inliner changed its decisions as
  the program grew. A discarded "ballast" object now keeps them stable, so
  adding code no longer costs matches elsewhere.

**Tooling from @coldspear:**
- safer disc-image extraction and XBE parsing;
- jump tables shown as data in the disassembler;
- `ready.py --claims` to skip claimed ranges;
- fixes to the LTCG probe and the permuter.

## 2026-10-04: 5004 functions match

```
matched 5004 of 11321 game functions (492273 of 2785198 bytes, 17.67%)
```

**Lane C**, round 11: 4 new matches, none lost. They include sector geometry
for pathfinding and animation helpers.

## 2026-10-04: 5000 functions match

```
matched 5000 of 11321 game functions (491662 of 2785198 bytes, 17.65%)
```

Five thousand of the game's functions now compile to exactly retail's bytes.
This batch added 20 new matches and lost none:
- **Lane D**, round 11: network connections, the player configuration cache
  and online game invites. Lanes J, M and the UI lane gain functions as a
  result.
- **Lane I**, round 4: AI props and firing positions.
- **A research finding.** VC7.1's link-time code generation tracks which
  registers each out-of-line callee really writes. Retail's math helpers were
  `inline`, so callers assume the standard registers are clobbered. Marking
  one helper `inline` matched two more functions.

## 2026-10-04: 4980 functions match; 17.5%

```
matched 4980 of 11321 game functions (487361 of 2785198 bytes, 17.50%)
```

**The UI lane**, round 12: 40 new matches, none lost. They are the virtual
keyboard screen (31 functions) and the main menu music.

## 2026-10-04: 4940 functions match

```
matched 4940 of 11321 game functions (483661 of 2785198 bytes, 17.37%)
```

**@Banshee64**: `path.cpp` (#26), with all 18 path search routines written
and 4 new matches, and the `unknown_0259d0` rectangle helpers (#29).

## 2026-10-04: 4935 functions match

```
matched 4935 of 11321 game functions (482890 of 2785198 bytes, 17.34%)
```

**Lane J**, round 7: 11 new matches, none lost. They are network message
handlers, link routes and transport endpoint options.

## 2026-10-04: 4924 functions match

```
matched 4924 of 11321 game functions (482034 of 2785198 bytes, 17.31%)
```

**Lane B**, round 8: 7 new matches, none lost. They are unit helpers, actor
slot handlers and path code.

## 2026-10-04: 4917 functions match

```
matched 4917 of 11321 game functions (480513 of 2785198 bytes, 17.25%)
```

**Lane M**, round 5: 9 new matches, none lost.
- They include friends-list functions, player-profile reads and AI action
  nodes.
- The friends globals are one structure, as in retail. Its address is
  taken, so fields reload after calls the way retail's do.
- Two UI functions match as a result.

## 2026-10-04: 4908 functions match

```
matched 4908 of 11321 game functions (478795 of 2785198 bytes, 17.19%)
```

**Lane T**, round 4: 17 new matches, none lost. It wrote the rest of the
first-person weapons file, models, scoreboard rows, the loading screen,
camera scripting and the Windows cache-file helpers. It also replaced many
stubs that other lanes had been calling, so two of lane A's script functions
now match.

## 2026-10-04: 4891 functions match; past 17%

```
matched 4891 of 11321 game functions (476869 of 2785198 bytes, 17.12%)
```

**The UI lane**, round 11: 43 new matches, none lost.
- The postgame statistics screen family is written: the screen, six tabs and
  their lists.
- A long-standing puzzle is solved: retail's zero-extended flag-bit test
  comes from `(bool)(((dword)flags >> bit) & 1)` on a short flags word, not
  from a bitfield. Six UI functions match with it, and the same shape turns
  up in other lanes' code.

## 2026-10-04: 4848 functions match

```
matched 4848 of 11321 game functions (473081 of 2785198 bytes, 16.99%)
```

**Lane D**, round 10: 22 new matches, none lost. They include online mute
lists, the online message block chain, voice mail and voice observers. Four
UI functions match as a result.

## 2026-10-04: 4826 functions match

```
matched 4826 of 11321 game functions (469815 of 2785198 bytes, 16.87%)
```

**Lane A**, round 12: 26 new matches, none lost. Every script built-in
function in `0x2a0000`–`0x2affff` is now written. New matches include the
object list helpers and their callers, more evaluators, and AI-script
helpers.

## 2026-10-04: 4800 functions match

```
matched 4800 of 11321 game functions (467763 of 2785198 bytes, 16.79%)
```

**Lane P**, round 4: 29 new matches, none lost.
- `ascii_string_to_unicode` takes its arguments in a different order,
  copied into locals, so six UI functions that call it now match.
- New in the region: the interface game system, new-HUD and visibility pool
  functions, game options and scenario fog.

## 2026-10-04: 4771 functions match

```
matched 4771 of 11321 game functions (464312 of 2785198 bytes, 16.67%)
```

**Lane F**, round 5: 19 new matches, none lost. They include player control
entries rebuilt in a new shape, sound cluster lookups and a batch
helper. Lanes N and A each gain one function as a result.

## 2026-10-04: 4752 functions match

```
matched 4752 of 11321 game functions (461659 of 2785198 bytes, 16.58%)
```

**Lane C**, round 10: 7 more animation samplers match (37 of 60), none lost.
Each sampler shares or copies its loop variables across the three passes in
its own pattern, and a generic loop now takes them by reference.

## 2026-10-04: 4745 functions match

```
matched 4745 of 11321 game functions (454928 of 2785198 bytes, 16.33%)
```

**The UI lane**, round 10: 33 new matches, none lost. They are the Xbox Live
message display screen and list, and six postgame statistics screens.

## 2026-10-04: 4712 functions match

```
matched 4712 of 11321 game functions (451105 of 2785198 bytes, 16.20%)
```

**Lane S**, round 3: 13 new matches, none lost. They include weapons, devices
and items code, and a lane A script function. Several functions that retail
calls out of line now live in their own `/Ob1` files.

## 2026-10-04: 4699 functions match

```
matched 4699 of 11321 game functions (449650 of 2785198 bytes, 16.14%)
```

**Lane N**, round 4: 13 new matches, none lost. They are the font cache,
player speed requests and window manager clan tasks, plus one of lane A's
script functions that matches once lane N's callee passes its reals in
retail's registers.

## 2026-10-04: 4686 functions match; past 16%

```
matched 4686 of 11321 game functions (447822 of 2785198 bytes, 16.08%)
```

**Lane J**, round 6: 19 new matches, none lost. They are the network link's
packet decode, send and receive loop, and the whole winsock transport
endpoint.

## 2026-10-04: 4667 functions match; almost 16% of the code

```
matched 4667 of 11321 game functions (444572 of 2785198 bytes, 15.96%)
```

**Lane C**, round 9: 20 new matches, none lost. Half of the 60 animation
samplers now match. These are large functions, so the matched share of the
code rose by almost a whole point. The fix was a missing root-z offset
statement and one shared sampling-state structure.

## 2026-10-04: 4647 functions match

```
matched 4647 of 11321 game functions (421089 of 2785198 bytes, 15.12%)
```

**Lane A**, round 11: 25 new matches, none lost. They are script built-in
evaluators and helpers. `location_for_point` (0x11bed0) now takes
its arguments in Halo CE's order, which matches two more callers.

## 2026-10-04: 4622 functions match; past 15%

```
matched 4622 of 11321 game functions (418535 of 2785198 bytes, 15.03%)
```

**The UI lane**, round 9: 71 new matches, none lost. They include the voice
message record screen, the online Y menu's player selected list, five more clan
tasks, and the window channel methods. Retail built those methods without
link-time code generation, so they live in a `/GL-` file.

## 2026-10-04: 4551 functions match

```
matched 4551 of 11321 game functions (412721 of 2785198 bytes, 14.82%)
```

**@Banshee64**: `vehicles.cpp` (#27). All 52 functions of the vehicle object
type are written; 2 match so far.

## 2026-10-04: 4549 functions match

```
matched 4549 of 11321 game functions (412575 of 2785198 bytes, 14.81%)
```

**Lane K**, round 5: 23 new matches, none lost. They include the rest of the
DirectSound stream code, input abstraction, the sound class lookup, sound
tracks, the attract mode and marketing screens' helpers, and S3TC texture
helpers.

## 2026-10-04: 4526 functions match

```
matched 4526 of 11321 game functions (409673 of 2785198 bytes, 14.71%)
```

**Lane H**, round 7: 8 new matches beyond those lane D's round 9 already
brought, none lost. They include HUD messaging, the multiplayer game variant
menu and a dialog helper the UI lane needed. Lane H's game variant type is
folded into the UI lane's.

## 2026-10-04: 4518 functions match

```
matched 4518 of 11321 game functions (408082 of 2785198 bytes, 14.65%)
```

**Lane D**, round 9: 26 new matches, none lost. They include voice port
checks, network session and observer code, and online Bungie.net user
functions. Eight of the UI, H and J lanes' functions match as a result.

## 2026-10-04: 4492 functions match

```
matched 4492 of 11321 game functions (405232 of 2785198 bytes, 14.55%)
```

**Lane L**, round 6: 14 new matches, none lost.
- The sound manager's table lookups now have retail's conventions.
- The main loop's byte globals turned out to be one structure, `main_globals`.
- A sound stream flush that retail calls out of line now sits in its own
  `/Ob1` file.
- Three of lane A's script functions match as a result.

## 2026-10-04: 4478 functions match; past 14%

```
matched 4478 of 11321 game functions (402586 of 2785198 bytes, 14.45%)
```

27 new matches, none lost:
- **Lane K**, round 4: the sound cache's `0x218850`, the first use of the
  `standard` marker; a dispose helper that retail calls out of line, now in its
  own `/Ob1` file; and with lane A's fixes, two DirectSound stream functions.
- **Lane C**, round 8: all 60 animation samplers written from one shared
  inline body (14 match so far), and three animation-graph functions.

## 2026-10-04: 4451 functions match

```
matched 4451 of 11321 game functions (389205 of 2785198 bytes, 13.97%)
```

**The UI lane**, round 8: 47 new matches, none lost. They include the player
selected list, Havok's fixed memory allocator and hkMemory, the window
manager's update, and the clan (online team) tasks.

## 2026-10-04: 4404 functions match

```
matched 4404 of 11321 game functions (385576 of 2785198 bytes, 13.84%)
```

**Lane A**, round 10: 17 new matches, none lost. They are script built-in
evaluators and the sound stream class's conventions, which unblock lane K's
DirectSound code.

## 2026-10-04: 4387 functions match

```
matched 4387 of 11321 game functions (384213 of 2785198 bytes, 13.79%)
```

54 new matches, none lost:
- **@Banshee64**: `projectiles.cpp` (#15, 11 match), `unknown_207b90.cpp` (#19),
  `unknown_0a76b0.cpp` (#23, 8 of 100 written match), crates (#24) and
  the unknown_11c920 string comparator (#25).
- **Lane K**, round 3: the DirectSound driver layer and the sound voice pool.
- **Lane B**, round 7: ten actor getters and lane A's `0x11ade0`, now that
  the seat lookup has retail's convention.
- **Lane J**, round 5: network link sends and the message gateway flush.

Tools: a `standard` marker, a last resort for functions that retail keeps in
the stack convention although nothing takes their address (see
docs/DECOMPILING.md).

## 2026-10-03: 4333 functions match

```
matched 4333 of 11321 game functions (377820 of 2785198 bytes, 13.57%)
```

**@Banshee64**: `garbage.cpp` (#20, written, near), `unknown_0b9fc0.cpp`
(#21, 1 of 7 match) and `unknown_11bdc0.cpp` (#22, 1 of 1).

## 2026-10-03: 4331 functions match

```
matched 4331 of 11321 game functions (377541 of 2785198 bytes, 13.56%)
```

**Lane D**, round 8: 20 new matches, none lost. They are the simulation
entity table's virtual methods, a view buffer release that keeps its size
argument on the stack because the function takes its address, voice port
flags, and the online message of the day.

## 2026-10-03: 4311 functions match; six more contributions from @Banshee64

```
matched 4311 of 11321 game functions (375948 of 2785198 bytes, 13.50%)
```

83 new matches, none lost:
- **@Banshee64**, six files:
  - `unknown_0b7300.cpp` (#12), 10 of 20 match;
  - `unknown_050650.cpp` (#13), 3 of 17;
  - `unknown_29ed40.cpp` (#14) and its legacy v1 format (#16),
    9 of 13 and 11 of 13;
  - `equipment.cpp` (#17), 2 of 3;
  - `unknown_11b980.cpp` (#18), 3 of 4.
- **The UI lane**, round 7: squad settings, sending Xbox Live messages, the
  multiple-choice dialog, variant parameter editing, clan and friends
  handlers.

## 2026-10-03: 4228 functions match

```
matched 4228 of 11321 game functions (368996 of 2785198 bytes, 13.25%)
```

**Lane N**, round 3: 10 new matches, none lost. They include player and
structure functions, and a UI function that matches now that its callee is
real. `function_1420f0` now takes its arguments in the debug
build's order, which gives it retail's register convention.

## 2026-10-03: 4218 functions match

```
matched 4218 of 11321 game functions (367773 of 2785198 bytes, 13.20%)
```

**Lane L**, round 5: 28 new matches, none lost. It fixed the conventions of
the datum-array unlink helper (`0x13d830`) and of `0x12d520`, which also
matched about ten simulation view functions in lane D's area. A texture cache
update now gets its own `/Ob1` file, so retail's out-of-line calls to it
match, and there is more sound manager code.

## 2026-10-03: 4190 functions match; function discovery fixes

```
matched 4190 of 11321 game functions (363435 of 2785198 bytes, 13.05%)
```

Function discovery (`tools/functions.py`, `tools/inventory.py`) now handles:
- tail jumps to the next function;
- `int3` bytes inside a looping function's own instructions;
- guessed starts that land inside an instruction;
- callbacks passed as pushed immediates;
- code reached only by a jump past the next function's start;
- library signatures that matched small game functions by chance.

The inventory gained 146 rows, mostly library code that no row covered
before. In game code, 19 rows were added and 20 removed, and 3 functions now
match because their extents are right. The checker also tells apart overloads
with pointer and class parameters.

## 2026-10-03: 4185 functions match; past 13%

```
matched 4185 of 11317 game functions (363298 of 2783395 bytes, 13.05%)
```

**The UI lane**, round 6: 69 new matches, none lost.
`c_class_1473c9::build` now has retail's register convention. The fix was
reading the pane array into a local as soon as the count is known. Ten screen
functions that call it match with it. Also new: list item handlers for the
pause, main menu, handicap, difficulty and team screens, the gamertag select
screen, and controller sign-in. The checker now tells overloaded constructors
apart by their parameter types.

## 2026-10-03: 4116 functions match

```
matched 4116 of 11317 game functions (357078 of 2783395 bytes, 12.83%)
```

**Lane A**, round 9: 34 new matches, none lost. They are script built-in
functions and the AI-script, flock and unit helpers behind them.

## 2026-10-03: 4082 functions match

```
matched 4082 of 11317 game functions (353593 of 2783395 bytes, 12.70%)
```

**Lane C**, round 7: 19 new matches, none lost. They are the animation
sampler dispatcher and its ten sub-dispatchers, plus animation graph lookups
that retail inlines. A sorted-array helper that retail passes everything on
the stack to now matches when it is built without link-time code generation.

## 2026-10-03: 4063 functions match

```
matched 4063 of 11317 game functions (351907 of 2783395 bytes, 12.64%)
```

**Lane D**, round 7: 16 new matches in the simulation world, view, players
and watcher, the network observer and network sessions; none lost.

## 2026-10-03: 4047 functions match; past 4000

```
matched 4047 of 11317 game functions (349798 of 2783395 bytes, 12.57%)
```

**The UI lane**, rounds 3 to 5: 233 new matches and none lost. They include
the text parser, the virtual keyboard, the online Y menu (friends, players and
recent players lists), the press-start and four-way sign-in screens, campaign
level select, playlists, the squad browser, clan and Live message screens, and
the screen, button, bitmap and model widget classes.

## 2026-10-03: 3814 functions match; past 12%

```
matched 3814 of 11317 game functions (335076 of 2783395 bytes, 12.04%)
```

**Lane H**, rounds 5 and 6 (`0x190000`): error dialogs, input and HUD code,
78 new matches and none lost. It found that some retail functions were built
without link-time code generation: building `player_slot_get` with `/GL-` in
its own file matched it and 13 of its callers.

## 2026-10-03: 3736 functions match; the backlog of lane rounds is merged

```
matched 3736 of 11317 game functions (325759 of 2783395 bytes, 11.70%)
```

116 new matches, none lost:
- **@Banshee64**: the updated `damage.cpp` (#8, #10), now calling the
  shared functions in place of its stubs;
- **lane C**, round 6: animation codecs, sampling and AI;
- **lane D**, round 6: simulation world, view players and the transport
  layer;
- **lane Q**, round 2 (`0x150000`): the object deletion callbacks and more;
- **lane A**, rounds 7 and 8: AI-script and script built-ins, flocks and unit
  helpers.

## 2026-10-03: 3620 functions match; damage and looping sounds

```
matched 3620 of 11317 game functions (310801 of 2783395 bytes, 11.17%)
```

113 new matches, none lost:
- **@Banshee64**: all of `damage.cpp` written (#8, #10; 9 match so far)
  and `unknown_12a1b0.cpp` (#11; 24 of 44 match);
- **lane L**, round 3 (`0x120000`): physical memory, game state globals and
  the texture cache;
- **lane F**, round 4: sound records and effects, now built on
  @Banshee64's looping sound controller;
- **lane P**, round 2 (`0x130000`): string drawing, interpolators and lists;
- **lane T**, round 2 (`0x160000`).

## 2026-10-03: 3507 functions match; past 10%

```
matched 3507 of 11317 game functions (294394 of 2783395 bytes, 10.58%)
```

One batch merge of seven lane rounds, 103 new matches and none lost:
- **lane B**, round 6: actor slot-handler callbacks;
- **lane S**, round 2 (`0x100000`): weapons, devices, scenery and items;
- **lane R**, round 2 (`0x170000`): effects and particle systems;
- **lane N**, round 2 (`0x140000`);
- **lane O**, round 2 (`0x240000`);
- **lane M**, round 3 (`0x1a0000`);
- **lane J**, round 4: network code and five unit event handlers that
  match now that the shared `0xe6900` helper is on main.

## 2026-10-03: 3404 functions match; nearly 10%

```
matched 3404 of 11317 game functions (274476 of 2783395 bytes, 9.86%)
```

One batch merge of eight lane rounds, 343 new matches and none lost:
- **lane P**, round 1 (`0x130000`): sorting, string drawing and the tag
  function evaluator;
- **lane T**, round 1 (`0x160000`): first-person weapons;
- **lane A**, round 6: script built-ins and the shared `0xe6900` helper that
  68 callers use;
- **lane C**, round 5: animation graphs and AI;
- **lane Q**, round 1 (`0x150000`);
- **lane K**, round 2: impacts;
- **UI lane**, round 2: about 170 screens, lists and their destructors;
- **lane I**, round 3: props and command scripts.

The work now runs as three agents at a time instead of a dozen or more:
one merges, two decompile.

## 2026-10-03: 3061 functions match; past 3000

```
matched 3061 of 11317 game functions (247847 of 2783395 bytes, 8.90%)
```

- **lane S**, round 1 (`0x100000`): weapons, devices, scenery and items;
- **lane R**, round 1 (`0x170000`): effects, particle systems, contrails and
  decals;
- **lane L**, round 2: the async job queue's last pieces, physical memory and
  the texture cache;
- **lane M**, round 2: the friends list and online-task screens;
- **lane H**, round 4: game engine events, and stack conventions for two
  widely used helpers.

## 2026-10-03: 2883 functions match; past 8%

```
matched 2883 of 11317 game functions (229262 of 2783395 bytes, 8.24%)
```

- **lane O**, round 1 (`0x240000`): the CTF/assault game engine and HUD
  messages;
- **lane D**, round 5: mute lists, online presence, friends, messages and
  matchmaking;
- **lane N**, round 1 (`0x140000`): the font cache and players code;
- **lane J**, round 2: the network message handlers and gateway, and the
  sequence windows.

## 2026-10-03: 2765 functions match

```
matched 2765 of 11317 game functions (217777 of 2783395 bytes, 7.82%)
```

- **the UI lane**, round 1: 114 screen, list and widget functions, the window
  manager as one object, and a new marker form for a class's implicit,
  non-deleting destructor (see docs/DECOMPILING.md);
- **lane B**, round 5: `transform4x3f_apply_point`. Retail keeps its matrix
  argument on the stack because the body takes the parameter's address; that
  one finding matched it and five functions built on it;
- **lane C**, round 4: the Havok component functions other lanes were waiting
  on;
- **lane I**, round 2: command scripts and props.

## 2026-10-03: 2604 functions match

```
matched 2604 of 11317 game functions (205329 of 2783395 bytes, 7.38%)
```

- **lane J**, round 1 (`0x090000`): network links and streams, team
  balancing, game engine entity definitions and online session search;
- **lane H**, round 3: the level list functions, the player iterators and the
  rest of the Juggernaut engine;
- **lane A**, round 5: the AI-script index register (the cause was in the
  function's own body), more script evaluators and localized text lookups;
- **lane M**, round 1 (`0x1a0000`): the motion sensor and actor behaviour
  handlers.

Two source patterns that unlocked many functions this round, now in our
notes for every lane: a getter that returns `valid ? p : NULL` matches when
written as if/else with the caller assigning its result right after the
call, and a stack parameter among register parameters usually means its
address is taken somewhere in the body.

## 2026-10-03: 2423 functions match

```
matched 2423 of 11317 game functions (191340 of 2783395 bytes, 6.87%)
```

- **lane D**, round 4: the network session manager and voice chat. It also
  found the source of a dead stack store in retail's inlined `host_check` check:
  a `volatile` local on the non-host path. That one change matched the check
  and the session setters it is inlined into.
- **lane L**, round 1 (`0x120000`): the async job queue and worker thread,
  global preferences, font loading, cache files, the sound manager and the
  texture cache.

## 2026-10-03: 2283 functions match

```
matched 2283 of 11317 game functions (179180 of 2783395 bytes, 6.44%)
```

- **lane H**, round 2 (`0x190000`): the Juggernaut game engine, game engine
  events, voice DSP effects and HUD messaging;
- **lane C**, round 3: the animation graph, which the animation channels
  and AI were waiting on;
- **lane I**, round 1 (`0x250000`): props, AI slot handlers, command scripts
  and firing-position evaluators;
- **lane K**, round 1 (`0x220000`): impacts, the sound driver and timing.

## 2026-10-03: 2152 functions match; 6% of the game's code

```
matched 2152 of 11317 game functions (167081 of 2783395 bytes, 6.00%)
```

- **lane D**, round 3: `c_class_58d20` (about 110 methods), the network
  observer, connections and transport keys;
- **lane B**, round 4: actor code at `0x1f0000`, and a finding about the build.
  An internal function's register convention comes from its own body and its
  callees, not its callers. So a function whose body is right can still be
  waiting on a callee that takes an argument differently in retail;
- **lane A**, round 4: the WMA and PCM sound codecs and the sound-effects
  class, signed saved-game file tasks and more AI-script functions;
- **lane F**, round 3: the outside functions its sound-source callbacks need.

New lanes have started on regions nobody had touched (`0x250000`, `0x090000`,
`0x220000`, `0x120000`), and lanes E and G now continue as a single UI lane.

## 2026-10-03: 2049 functions match; past 2000

```
matched 2049 of 11317 game functions (157385 of 2783395 bytes, 5.65%)
matched 2050 of 17069 functions in scope (157397 of 3731252 bytes, 4.22%)
```

Six more lane stints landed:
- **lane B**, round 3: the outside functions its slot handlers call (character
  block getters, unit seats, node points, clumps);
- **lane C**, round 2: AI scratch buffers, physics and animation channels;
- **lane E**, round 2: list constructors, settings-edit lists and the widget
  base methods;
- **lane F**, round 2: sound decibel conversion and object queries;
- **lane G**, round 1 (`0x230000`): screen widgets, window manager channels and
  2D polygon clipping;
- **lane H**, round 1 (`0x190000`): local controllers, game-variant checks and
  data compression.

A lesson from this round: in an LTCG build, a function's argument registers
follow its callers. Many functions with correct bodies wait only for more of
their callers to be decompiled before they match.

## 2026-10-03: 1809 functions match; lanes go vertical

```
matched 1809 of 11317 game functions (142921 of 2783395 bytes, 5.13%)
matched 1809 of 17069 functions in scope (142921 of 3731252 bytes, 3.83%)
```

Most functions left in each lane's region call code outside it, which takes
its arguments in registers that a stub can't reproduce. So the lanes now
decompile those outside functions first, then their callers:
- **lane A**, round 3: 98 more, among them the AI-script and squad functions
  the script evaluators call, and `function_1dee50`;
- **lane D**, round 2: network configuration, `xuid_equal` and the online game
  invite.

Contributors can see who is working where in the pinned
[Active claims](https://github.com/kirklandsig/halo2-decompiled/issues/9)
issue.

## 2026-10-02: 1703 functions match; five region lanes merged

```
matched 1703 of 11317 game functions (131004 of 2783395 bytes, 4.71%)
matched 1703 of 17069 functions in scope (131004 of 3731252 bytes, 3.51%)
```

Five region lanes landed their stints:
- **lane A** (0x2A0000): 127 more of the script engine's built-in functions;
- **lane B** (0x1B0000): 20 more actor slot handlers;
- **lane D** (0x060000): 91 functions of the simulation world, online tasks and
  network sessions;
- **lane E** (0x2B0000–0x2CB8C0): 103 UI screen, list and widget methods, three
  game engines and the uncompressed animation codecs;
- **lane F** (0x180000): 51 functions for local player slots, player control,
  looping sounds, sound sources and the loop allocator.

Two more regular batches added game-speed, camera and quaternion code. The
checker now treats libcmt's `memmove` and `memcpy` as one function, since they
are the same code.

The README and the new [CONTRIBUTING.md](../CONTRIBUTING.md) explain how to
join in.

## 2026-10-02: 1292 functions match; AI code and nine more batches

```
matched 1292 of 11317 game functions (101876 of 2783395 bytes, 3.66%)
matched 1292 of 17069 functions in scope (101876 of 3731252 bytes, 2.73%)
```

- **Lane C** (0x1C0000–0x1CFFFF) finished its first stint: 64 AI and actor
  slot-handler functions, including `src/ai.cpp`. The link now reads its
  object list from a response file, since the command line had outgrown
  Windows' 32K limit.
- **Nine regular batches** landed: geometry and axis tables, input records,
  visibility tests and render state, handle tables, aim assist, transport
  addresses and more. Their duplicated constants, inline vector helpers and
  types now live in shared headers.

## 2026-10-02: 1183 functions match; joint behaviour and Bink playback

```
matched 1183 of 11317 game functions (89984 of 2783395 bytes, 3.23%)
matched 1183 of 17069 functions in scope (89984 of 3731252 bytes, 2.41%)
```

Two more files from @Banshee64 are merged: `unknown_26e370.cpp` (actor joint
behaviour, whose seven callbacks are now wired into the slot-handler tables
with their retail `__stdcall` convention) and `unknown_01e930.cpp` (Bink movie
playback and its memory callbacks).

The checker now handles identical functions that the linker folded into one
body: a call into one of them matches through any of their names.

## 2026-10-02: 1166 functions match; shared engine headers

```
matched 1166 of 11317 game functions (88136 of 2783395 bytes, 3.17%)
matched 1166 of 17069 functions in scope (88136 of 3731252 bytes, 2.36%)
```

Six more batches landed: text formatting, game-engine marker objects,
geometry helpers, random-number users and player state. Duplicated
declarations from those batches are now shared: one `c_game_engine` class
in `include/unknown_1523c0.h`, common float helpers in `include/unknown_0259d0.h`,
and the player-state and match-globals layouts in `include/globals.h`.

## 2026-10-02: 1125 functions match; independent region lanes

```
matched 1125 of 11317 game functions (83039 of 2783395 bytes, 2.98%)
matched 1125 of 17069 functions in scope (83039 of 3731252 bytes, 2.23%)
```

**Lanes.** Work now also runs in "lanes". A lane is an independent agent that
owns one address region and works through it on its own branch, like an
outside contributor. Two lanes finished their first stint:
- one decompiled 143 of the script engine's built-in function evaluators, each
  followed by its retail definition data;
- one decompiled 65 actor slot-handler (AI behaviour) callbacks, with their
  handler structs at retail addresses.

**Contributors.** @Banshee64's `unknown_07aec0.cpp` is merged, and more
files are in progress. Claimed address ranges are kept free of our
automated work.

## 2026-10-02: 913 functions match; network message codecs

```
matched 913 of 11317 game functions (67094 of 2783395 bytes, 2.41%)
matched 913 of 17069 functions in scope (67094 of 3731252 bytes, 1.80%)
```

**Network messages.** Each network message type has an encoder, a decoder and
a clear/compare callback. A registration function stores them in the
message-type table. The codecs, and the bit-stream module they write
through, are now decompiled, and most of them match byte for byte.

**The data arrays are complete.** All 18 core handle-pool routines match.

**More contributors.** @Banshee64 contributed `unknown_1c9830.cpp` and
`unknown_1248b0.cpp`. Work is coordinated by address range, so contributors don't
collide.

## 2026-10-02: 723 functions match; subsystem lifecycle callbacks

```
matched 723 of 11317 game functions (40978 of 2783395 bytes, 1.47%)
matched 723 of 17069 functions in scope (40978 of 3731252 bytes, 1.10%)
```

**Subsystem lifecycle.** The engine starts and stops its subsystems through a
table of 68 entries (`0x440DD8`). Each entry has up to nine callbacks:
initialize, dispose, per-map and per-BSP set-up and teardown, and change
notifications. 55 of 58 callbacks attempted so far match. They are named after
their subsystems: `function_14b4b0`, `function_17d2a0`,
`arena_initialize_for_new_map` and so on.

**Also matching:**
- the session-state manager;
- script value casts;
- `function_11c9c0`;
- color conversions;
- binary search and short sort;
- HUD helpers;
- model variant lookups;
- path-finding heap operations.

## 2026-10-02: 612 functions match; the core data arrays

```
matched 612 of 11317 game functions (34854 of 2783395 bytes, 1.25%)
matched 612 of 17069 functions in scope (34854 of 3731252 bytes, 0.93%)
```

**Data arrays.** The engine keeps most of its runtime state in handle-addressed
pools: players, objects, effects, AI and more. 16 of the 18 core routines now
match:
- create and dispose;
- allocating an element at the next free slot or a given one;
- deleting by handle;
- handle-to-pointer lookup;
- iteration.

`include/data_array.h` is the single type every subsystem now uses.

**Also matching:**
- the unit, item, weapon, projectile and device object types;
- the game-engine player entity and breakable-surface entity definitions;
- about 25 event-definition classes;
- surface descriptions;
- tag lookups.

**Contributions.** The tools now also run on Linux under Wine, thanks to a
pull request from @Banshee64.

## 2026-10-02: 442 functions match; more than 1% of the game's code

```
matched 442 of 11317 game functions (29446 of 2783395 bytes, 1.06%)
matched 442 of 17069 functions in scope (29446 of 3731252 bytes, 0.79%)
```

**Switch statements are whole functions now.** Function discovery used to
record each `switch` case label, and the jump tables themselves, as separate
functions. Now they belong to the function that owns them: 525 fragment rows
are gone, and 126 functions have their full extent. The game-function total
dropped from 11,802 to 11,317 for that reason, not because work was lost.

**Also matching:**
- the game-engine class;
- network session state classes;
- UI widgets;
- the vehicle and turret type classes;
- table-driven data readers;
- input mapping;
- and more.

**Next.** The engine's core data-array routines (handle-addressed pools that
nearly every subsystem uses) are decompiled and waiting to merge. They
unblock many callers whose calls into those routines could not match while
the routines were stubs.

## 2026-10-02: 354 functions match; object types as C++ classes

```
matched 354 of 11802 game functions (26036 of 2782989 bytes, 0.94%)
matched 354 of 17586 functions in scope (26036 of 3730854 bytes, 0.70%)
```

**Object types are a class hierarchy.** The vtables for vehicles, turrets and
related object types share most of their slots. Every slot that two or more
of these vtables share is now a method of one base class,
`c_object_type_definition`, in `include/unknown_0a58d0.h`. Each type's
own overrides live in its derived class. The methods decompiled so far match
with the class written as plain C++.

**Also matching:**
- two interface vtables of 21 slots each, almost entirely;
- table-driven data readers;
- 256-bit bit-vector helpers;
- string and Unicode helpers, including variadic formatting functions.

**Build fixes.** Stand-ins now handle variadic functions. Stubs for code that
is not decompiled yet are built without function-level linking, so the
linker no longer folds identical ones into one address.

## 2026-10-02 (night): 271 functions match

```
matched 271 of 11802 game functions (22986 of 2782989 bytes, 0.83%)
matched 271 of 17586 functions in scope (22986 of 3730854 bytes, 0.62%)
```

**Tables lead to whole families of functions.** Retail's data holds tables of
function pointers: callback tables and C++ vtables. The functions one table
points at usually come from one source file and share one shape. A table at
`0x470828` led to 62 field-descriptor callbacks. All 62 now match, with the
table itself rebuilt entry for entry. A scanner now finds such tables. The
next batches are drawn from them: more vtables and callback tables.

**Also matching since 163:**
- input-device state;
- timed screen effects;
- Direct3D texture and palette set-up;
- a page heap that implements a library allocator;
- object iteration helpers;
- input mapping;
- more class hierarchies, with their deleting destructors.

## 2026-10-02 (later): 163 functions match; C++ destructors and library callers

`python tools/check.py` reports:

```
matched 163 of 11802 game functions (17564 of 2782989 bytes, 0.63%)
matched 163 of 17586 functions in scope (17564 of 3730854 bytes, 0.47%)
```

**What matches now:**
- Random numbers: the seed, and random vectors in a cone.
- Hash tables, bit vectors and integer log2.
- Network-message counters.
- An actor action-slot system.
- A handle table whose classes use virtual methods.

**What the build now handles:**
- **Functions that library code calls.** Havok, the C runtime and the XDK
  libraries were not built with link-time code generation. Any game function
  they call keeps its standard calling convention. The build reads those
  callers from the inventory and models them. Three Havok-called methods now
  match without any changes to their source.
- **Deleting destructors.** The destructor slot of a vtable holds a function
  the compiler generates: it calls the destructor, then `operator delete`.
  A `deleting` marker now ties that function to its class, so a class is
  written as Bungie wrote it, even when its destructor is implicit.
- **Data tables of function pointers.** These are written only where retail's
  data actually holds them, at their retail addresses. For example, the
  handler struct at `0x47d930` is reproduced field for field.

**What the build taught us:**
- VC7.1 fully unrolls `for (i = 0; i < 3; i++)` over a small body. Retail's
  3-iteration loops were written `do { ... } while (i < 3);`.

## 2026-10-02: 132 functions match; virtual methods supported

`python tools/check.py` reports:

```
matched 132 of 11802 game functions (14647 of 2782989 bytes, 0.53%)
matched 132 of 17586 functions in scope (14647 of 3730854 bytes, 0.39%)
```

**How the work is organised.** Parallel workers each take a batch of
neighbouring functions. A batch is usually one source file. Workers
send their work back in waves. Each wave is merged into `main`, and the
globals that several files share are unified in `include/globals.h`. Wave 2
and wave 3 together added 91 matches.

**What matches now:**
- Object list management and the object header table.
- Localized wide-string getters.
- Game state globals, and more of the AI and animation code.
- Several C++ classes, including a class that overrides two virtual methods of
  its interface.

**What the build taught us:**
- **Bit flags:** flag words are 1-bit bitfields tested with a `bool` cast
  (`TEST_FIELD_BIT` in `unknown_11c920.h`). That is the only form that compiles to
  retail's `shr reg, N; test reg, 1` sequence. It appears about 385 times in the
  game.
- **Virtual methods:** stand-ins now call methods by qualified name and
  copy-construct each class that has a vtable. A virtual method's address then
  escapes into the vtable, as in retail, and LTCG keeps its `thiscall`
  convention. Virtual methods are written as Bungie wrote them, with no
  workarounds.
- **Inlining across files:** LTCG inlines a small function into callers in
  other files unless that function's own file is built with `/Ob1`. Per-file
  flags therefore matter across files.
- **Caller-driven conventions:** some near-misses differ only in which
  registers carry their arguments. LTCG picks those registers from the
  callers, which are not decompiled yet. The checker re-tests every function
  on each build, so these can turn into matches later.
- **More code outside Bungie's:** a Havok collision query inlined into game
  code at `0x183910` was excluded from game code.

## 2026-10-02: decompilation has started: 41 functions match

`python tools/check.py` reports:

```
matched 41 of 11815 game functions (4923 of 2785826 bytes, 0.18%)
matched 41 of 17599 functions in scope (4923 of 3733691 bytes, 0.13%)
```

**What matches.** 41 retail functions rebuild byte for byte:
- File path helpers: `function_137320`,
  `function_137370` and `function_1373c0`.
- Unicode classification and UTF-8 encoding.
- 3x3 and 4x3 matrix maths, including two hand-written assembly routines.
- AI firing-position evaluation and AI clumps.
- Recorded-animation playback readers.
- S3TC and texture helpers.
- The CRC functions and the game state allocator from the spike.

About 20 more are near-misses: the same length, but the register allocation or
operand order differs.

**Where Bungie's code ends.** Bungie's code ends at `0x2cb8c0`. Everything
above that in `.text` is Xbox SDK libraries and third-party code: Havok, Bink,
the C runtime, voice, WMA, DSOUND and compiler-generated stubs. Applying that
boundary in `config/owners.json` cut the game-code total from 12,959 to 11,815
functions. Two regions that had been taken for game code turned out to be
Havok physics code.

**What the build taught us:**
- Floating-point code needs `/arch:SSE` (some files need `/arch:SSE2`), because
  Bungie's build used SSE.
- Callbacks stored in tables are `__stdcall`.
- Some maths routines are hand-written inline assembly.
- Game code that calls Direct3D called the public D3D API. The SDK's
  `d3d8ltcg.lib` is linked, and LTCG inlines parts of it, as in retail.

**Tooling changes:**
- Per-file compiler flags now live in the source, in a `// @flags` comment.
- The build links the SDK libraries.
- The checker also verifies call targets. A call must reach the function with
  the same retail address or the same name.
- Stand-in callers are generated inside each source file's own translation
  unit.
- `tools/permute.py` searches variants of a source function for near-misses.

**Next.** Fix the near-misses, then continue up from the leaf functions with
`tools/ready.py`.

## 2026-10-01: project set-up: inventory, whole-game build, checker

The set-up is in place. `python tools/check.py` reports:

```
matched 8 of 12959 game functions (421 of 2891676 bytes, 0.01%)
matched 8 of 17599 functions in scope (421 of 3733691 bytes, 0.01%)
```

Eight functions are MATCH: `function_163ba0`, `function_163c00`,
`function_123d40`, `distance3d`, `function_259d0` and three game state
initializers. `game_state_malloc_aligned` (`0x123d80`) is the one near-miss: a
single `lea` operand order.

**The inventory** (`config/functions.csv`) has 19,509 functions. By owner:

| Owner | Functions |
| --- | ---: |
| `game` | 12,959 |
| `eh` (MSVC exception-handling stubs) | 586 |
| `third:havok` | 1,034 |
| `third:bink` | 290 |
| `xdk:xonline` | 915 |
| `xdk:xvoice` | 816 |
| `xdk:wmadec` | 649 |
| `xdk:dsound` | 554 |
| `xdk:xnet` | 416 |
| `xdk:libcmt` | 394 |
| `xdk:xapi` | 270 |
| `xdk:d3d8` | 254 |
| `xdk:xapilib` | 219 |
| `xdk:libcpmt` | 93 |
| `xdk:d3dx` | 36 |
| `xdk:xonlines` | 23 |
| `xdk:rockall` | 1 |

Only `game` functions are ours to decompile. "In scope" is everything except
`eh`, Havok and Bink (19,509 less 586, 1,034 and 290 is 17,599).

**What changed from the spike's tool.** `tools/check.py` replaces
`tools/match.py`:
- Exact extents. Ours come from the linker map and retail's from the
  inventory. When the rest of our function is `0xCC` fill, retail's size is
  compared.
- Exact masks. The masked bytes are the base relocations plus the linker's
  `/MAPINFO:FIXUPS` relative fields, not guesses from values.
- Each masked field is validated against retail: an absolute field must hold a
  retail-image address, a relative field must leave the function, and every
  field must lie inside both extents. A field that points inside the function
  (a jump table entry, a self-call) must point at the same offset in retail.
- Status (`matched`, `near`, `todo`) is written back to
  `config/functions.csv`.

**What we found while building the inventory:**
- Discovery needed end clamping, and had to drop weak starts that land in the
  middle of an instruction.
- Library code is recognised by byte signature from the SDK's own `.lib`
  files (CRT, XAPI, DSOUND, XONLINE, XVOICE, XNET, D3DX) and by section (D3D8,
  XPP, Bink, WMA).
- Unnamed functions take the owner of their neighbours.
- The 586 MSVC exception-handling stubs are classed `eh`: they are
  compiler-generated, not work items, and the checker does not check them.
- A bug that let the checker accept a wrong address field (an absolute field
  was masked without checking that retail held an address there) was fixed
  before first use.

**What we found about `PRIVATE`.** Bungie's static functions can be
compiled with external linkage (the `PRIVATE` macro is empty). That did not
change `function_163c00`'s code, which still matches. So generated stand-in
callers in other files can reach static functions.

**Note.** The disassembler is `tools/disasm.py`, not `dis.py`: a `dis.py`
would shadow Python's standard-library `dis` module.

**Next.** Decompile from the leaf functions up, picking work with
`tools/ready.py`.

## 2026-10-01: the spike's answer: the LTCG build can be matched

Eight retail functions now rebuild byte for byte, and a ninth is one
instruction short:

| Function | Retail | What it tests |
| --- | --- | --- |
| `function_163ba0` | `0x163ba0` | a custom calling convention |
| `function_163c00` | `0x163c00` | a custom calling convention |
| `function_123d40` | `0x123d40` | an argument moved from the stack to `eax` |
| a game state initializer | `0x1edbc0` | `function_123d40` inlined into a caller optimized for speed |
| two game state initializers | `0x24c819`, `0x165cc3` | callers optimized for size, which call it out of line |
| `distance3d` | `0x3ea30` | x87 floating point and evaluation order (the body only) |
| `function_259d0` | `0x259d0` | LTCG deleting unused arguments (the body only) |
| `game_state_malloc_aligned` | `0x123d80` | one instruction short: `lea eax, [ebx + ecx]` against our `[ecx + ebx]` |

The game state functions and both CRC functions match together in one LTCG
image, with each source file built with its own flags. "The body only" means
the function matches when kept out of line. What keeps retail's copies of
these two out of line is not yet known.

**What the spike found about Bungie's build:**

- **Two kinds of code.** Most is optimized for speed (`/O2`): functions
  aligned to 16 bytes, no frame pointer. Some is optimized for size (`/O1`):
  functions packed without padding, `ebp` frames, `push 4; pop ecx`. LTCG
  keeps each source file's flags.
- **Inlining follows the flags.** `function_123d40` is inlined at about 51
  call sites, all in code optimized for speed. It is called at 7, all in code
  optimized for size. Our compiler makes the same choices from the same
  flags.
- **Some files need `/Ob1`.** With `/Ob2`, our compiler inlines
  `function_163ba0` into `function_123d40`, which retail does not. With
  `crc.cpp` built `/Ob1`, everything matches. Which files differ like this
  is still to be mapped.
- **The source's shape matters, as in any matching decompilation.** Examples:
  - `short` loop counters.
  - The order of the terms in `distance3d`.
  - Checksumming a local copy rather than a parameter. Taking a parameter's
    address keeps it on the stack.
- **SSE.** Retail uses `movss`, `ucomiss`, `xorps` and `fcomi` in some float
  code. We have not reproduced that in a test yet.

**The tool.** `tools/match.py` now takes several source files, each with its
own flags, and links them into one image. It masks only the bytes that the
test image relocates; before, it guessed from values.

**Next.** We will design the project set-up:
- the build, with per-file flags;
- a function inventory of the retail XBE;
- a way to tell each function's flags from its code;
- progress tracking.

## 2026-10-01: the first two functions match

**LTCG can be matched.** Two retail functions now rebuild byte for byte:

| Function | Retail address | Arguments in retail |
| --- | --- | --- |
| `function_163ba0` | `0x163ba0` | buffer in `eax`, size in `edi`, CRC pointer on the stack (`ret 4`) |
| `function_163c00` | `0x163c00` | table in `edx` |

- **The source:** Halo CE's `crc.c` from the
  [punpckhdq/halo](https://github.com/punpckhdq/halo) decompilation (CC0),
  compiled as C++.
- **The toolchain:** XDK 5849's compiler, `/O2 /GL /Gr` (fastcall by default),
  linked with `/LTCG`.
- **The calling conventions:** the compiler chose the same custom ones as the
  retail build, from the source alone.
- **What "byte for byte" means here:** every instruction and every byte is
  equal, except the data addresses, which differ because the test image lays
  out its own data.
- **One source detail mattered:** the loop counters must be `short`, as in
  Halo CE. With `long` ones, the compiler unrolls the inner loop.

This answers the spike's main question for simple functions: the XDK 5849
compiler and these flags reproduce Bungie's LTCG output. The next tests use
harder cases: floating point, C++ member functions, and functions whose
callees were inlined.

Reproduce it with `tools/match.py`; the command is at the top of
`spike/crc.cpp`.

## 2026-10-01: the target, the toolchain, and LTCG

**The target build.** The retail disc's `default.xbe`:

- SHA-256 `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`.
- Internal name `c:\halo2\bin\halo2ship.exe`, linked 2004-09-28.
- Title ID `4D530064`, all regions.
- `.text` is 0x367C4C bytes (3.4 MiB) at 0x12000.
- Linked libraries:
  - XAPILIB, XBOXKRNL, LIBCMT and LIBCPMT
  - DSOUND, XVOICE, VOICMAIL and XONLINES
  - D3D8LTCG and XGRAPHCL

  Every one is version 1.0.5849.
- Its own sections hold DSOUND, WMADEC, XONLINE, XNET, Bink, D3D and XPP.

**Earlier work.** We found no existing Halo 2 decompilation.

**LTCG.** The retail game code was compiled with link-time code generation:

- `tools/ltcg_probe.py` finds 9,954 direct call targets in `.text`.
- 4,232 of them (42.5%) read `eax`, `ebx`, `esi` or `edi` at entry before
  writing them. MSVC's standard conventions never pass arguments in those
  registers.

Some examples:

| Function | Arguments |
| --- | --- |
| `0x128c0` | A pointer in `eax` |
| `0x14390` | Values in `edi` and `esi` |
| `0x16b10` | A pointer in `esi` |

That rules out the usual decompilation workflow, where each function is
compiled alone and compared with the original. Under LTCG the code is
generated for the whole program at once, when it is linked.

**The toolchain.** XDK 5849 ships its own compiler in `xbox\bin\vc71`:

- `cl` 13.10.3077 and `link` 7.10.3077 (Visual C++ .NET 2003).
- Both run natively on Windows 11.
- The SDK's `d3d8ltcg.lib` and `xgraphicsltcg.lib` are the LTCG forms of the
  libraries linked into the XBE.

**Next.** The feasibility spike asks whether XDK 5849's compiler, in LTCG
mode, can reproduce retail functions byte for byte.
