# Matching near misses with an agent and parallel lanes

This is how the Havok matches of 2026-11-16 (#1204 to #1228) were made: the
tooling, the loop, how a planning session adapts the setup to each new chunk of
code, and the compiler behaviour that the matches came from, each with the
function that proved it. It adds to [DECOMPILING.md](DECOMPILING.md); it does
not replace anything there.

Everything here was measured on MSVC 7.1 (13.10.3077) with this project's
flags. The rules are what the compiler was seen to do, not what its
documentation says, and each one names the function where it can be checked.

## The setup in one paragraph

One coordinator session of Claude Code (model Claude Fable 5.2, or Claude
Opus 5.5 when that was what was available; default settings) and four to eight
background subagents, called lanes here. A lane owns a short list of functions
and a theme. Lanes never edit the repository and never run git: they write
whole-file variants of a source file into a scratch folder, score them, and
hand back a report. The coordinator merges the best variant, runs
`tools/check.py --fast` on the function and on every other `// @retail` marker
of the file, writes the score note, commits, and opens the pull request.

## Planning sessions: adapting the setup to each chunk of code

The setup is not fixed. Before each new chunk of code (a library, a subsystem,
a family of functions) the coordinator runs one planning session, and what that
session produces decides how the lanes work. With Fable 5.2 this is a session
in plan mode: the model may only read, and it must hand back a written plan
before it changes anything. The plan for a chunk answers six questions.

1. **How is this code built, and what is the cheapest honest score?**
   Havok files are compiled without `/GL`, so one object file is the whole
   truth: compile the `.cpp`, read the function out of the COFF with
   relocations masked, compare. About two seconds a variant. Game functions are
   whole-program code: the function only exists after the link, so the score
   has to come from the reduced image of `check.py --fast`, 10 to 40 seconds a
   variant. The planning session picks the backend and says what a variant
   costs, because that sets the size of a variant family (200 for Havok, 8 to
   12 for game code).
2. **Does the cheap score agree with the project's check?** Calibrate before
   any lane starts: score a dozen functions that are already matched (they must
   come out matched) and a handful of unmatched ones (the number must equal
   what `tools/check.py` prints). A backend that disagrees is fixed or dropped
   in the planning session, not discovered by a lane three hours in.
3. **What can a change here break, and what is the gate?** For Havok the gate
   is the other markers of the same file. For game code a change can alter
   other functions through the whole-program link, so the gate is a full image
   compared function by function with a saved baseline: the target gained,
   nothing lost, no other score moved.
4. **Which functions, and grouped by what?** Not by score. The session reads
   the first divergence of each open function and sorts them by cause: wrong
   callee-saved register, wrong scratch register, wrong stack slot, operand
   order, block order, an expression the compiler folded. Functions with the
   same cause go to the same lane, because one fix usually moves all of them
   (the four-byte key read below fixed four functions at once).
5. **What does each lane get?** A list of functions, a theme (fresh rewrite
   from the retail listing; one shared problem as a compiler lab; a sweep of
   one technique over many functions; finishing near misses), the dead ends
   already known for each function, and a time box. Themed lanes found the
   matches. Lanes told only "try again" found almost nothing.
6. **What is already claimed?** The session reads the claims issue and the
   open pull requests and removes other people's rows before any lane starts.

The plan also fixes the stop rule (one round per theme; a second round only
where a lane reports a new structural lead) so the work ends instead of
grinding.

Two habits make the planning sessions pay off:

- **The lane guide is the memory.** Every lane begins by reading one file that
  holds the hard rules, the command lines, and every technique found so far
  with the function that proved it. When a lane reports a new rule, the
  coordinator adds it to the guide before starting the next lane. Seven of the
  later matches came from a rule that an earlier lane had found the same
  evening on a different function.
- **Re-plan when the tools change, not on a timer.** After the reference
  barrier (below) was found, functions that earlier lanes had declared stuck
  were given to new lanes with one instruction: the old "source spelling
  cannot fix it" conclusions predate this technique, so test them again. Two
  of those matched.

## Scoring

Three numbers per variant, because the project's score alone misleads on large
changes:

- `lock`: the lockstep count `tools/check.py` prints. It is positional, so one
  instruction inserted early makes everything after it count as different. A
  correct rewrite can score 250 where a wrong body scores 240.
- `dist`: an aligned edit distance between the two listings.
- `sem`: the multiset difference of masked instructions: which operations one
  side has and the other lacks, whatever their order and registers.

Steer by `sem` and `dist`, confirm by `lock`, and let `tools/check.py --fast`
on the function and its file's other markers be the judge. Several older "tried,
got worse" notes in the sources were judged by `lock` alone and turned out to
be the right shape (`0x2fb150`: the clean rewrite scored 257 against the old
body's 243, and was two changes away from 22).

Every scored variant goes into a small database keyed by the hash of its
source, so no lane compiles the same text twice and the next lane can read
what was tried.

## The loop for one function

1. Read the retail listing at the first difference and the instructions only
   retail has. Ask what source would make the compiler emit exactly that.
2. Write the hypothesis as a family: a script that takes the product of a few
   independent choices (which reads become locals, where they are declared,
   which helper form) and writes one whole-file variant per combination.
3. Score the family in one batch. Read the best variant's listing, not only
   its number.
4. Stop after about 30 minutes without movement and report what did not work,
   one line per idea, so nobody repeats it.

When a large function is stuck, do not patch it. Print retail's listing, find
an already matched function with the same opening or shape, and write the body
again in that function's style: the same constructors, the same member
helpers, the same declaration order. That took `0x307840` from 254 to 100,
`0x301a60` from 806 to 68, `0x2dc220` from 406 to 71 and `0x2fb150` from 244
to 22.

## Real source or a score steer?

A form is kept only if all of these hold:

- A person would write it: an accessor, a setter, a constructor, a helper that
  takes a member by reference, a block scope, a base class.
- It removes a difference; it does not move one somewhere else.
- No other marker of the file gets worse.
- A one-use local or a reference local is kept only with a byte match.
- A change that does not match is submitted only when the score drops by 8 or
  more.

## What the compiler was seen to do

### A local with a reference bound to it is not folded away

The most useful single fact. If a reference (or pointer) is bound to a local,
the compiler does not substitute that local back into the expressions that use
it. Four uses:

- **Keeping two sub-expressions apart.** Retail built a four-byte key as
  `shl edx, 0x10; shl edi, 8; add eax, edx; lea edx, [eax + edi]`; every
  single-expression spelling folded into a nested shift chain one byte shorter,
  which shifted the jump tables. Matched in `0x2ff210` and `0x2ff570`:

  ```cpp
  long &sum = offset;
  sum = READ_2_2FF210(p + 1) << 16;
  sum += READ_2_2FF210(p + 3);
  ```

- **Choosing the destination of an SSE operation.** Intrinsic operands are
  evaluated right to left and named locals fold back, so `and(sum, mask)`
  hoists the mask load and `and(mask, sum)` makes the mask the destination.
  Matched in `0x306b90`:

  ```cpp
  __m128 signs = _mm_add_ps(axis_direction, _mm_mul_ps(splat_306b90(side_value), nudge));
  __m128 &kept = signs;
  kept = _mm_and_ps(kept, *(__m128 const *)g_4124a0);
  ```

- **Keeping a pointer chain in one register** (`entity->owner->island` all in
  eax, `0x316110`), and **keeping a register busy** past a value's last use so
  the next value is allocated elsewhere (`0x319230`).
- **Stopping two inlined lookups from merging** (game function `0xe8c10`).

It does not stop strength reduction of a loop counter (`0x2fe550`).

### Callee-saved registers follow weighted use counts

Priority is roughly the number of uses, times two per loop level (a block's
weight is 2 to the power of its loop depth, measured in the back end), divided by
the basic blocks the value spans; esi, edi, ebx, ebp are handed out in that
order. Push order means nothing. Two consequences:

- A redundant loop guard written `if (0 < n) { for (...) }` (for a
  down-counting loop, `if (0 <= n - 1)`) emits no code but changes the
  assignment. `0x2e5590` matched on that alone. The spelling matters: `n > 0`
  adds code.
- When two values fight for a register, give the rival one fewer use. In
  `0x2fb150` a list pointer with six uses took ebx from a parameter; stepping
  the count through the original pointer (five uses of the local) gave it
  back.

### Scratch registers rotate

eax, ecx, edx are handed out in turn, skipping busy ones; a fixed-register need
(ecx for a receiver) does not advance the turn. A trivial accessor or setter
shifts the rotation without emitting code. `0x3173f0` matched when the
tolerance was read through `world->get_tolerance()` and the handler stored
through `dispatcher->set_handler(this)`; either alone did not match.

### XMM registers

The lowest free register is taken, the right operand first. A named copy
changes nothing, but `_mm_set_ss(constant)` named as a local is a real
operation, and its declaration order sets the registers (`0x306b90`,
`0x306d00`: zero first, then the tiny value).

### Stack layout

Four-byte cells sit at the bottom and aggregates above them, sorted by size.
Declaration order of uninitialised locals moves nothing. What does move slots:

- Locals in separate `{}` blocks can share or separate slots. Giving each of
  three groups its own `edge` and `lengths` took `0x306580` from 149 to 101.
- A one-byte struct result read through an inline member (`found.is_set()`)
  is laid out as an aggregate; read as a field it is a narrow scalar at the
  bottom, which shifts every four-byte home (`0x307c40`, 24 to 5).
- Spills reuse dead parameters' argument homes first.

### Shapes that point at their source

| Retail shows | The source had |
| --- | --- |
| A member's address formed, then read through (`add reg, N; mov reg, [reg]`) | That member passed by reference to an inline helper (`0x2e3370`, `0x2daae0`) |
| `dec dword ptr [reg + N]` in place | `long &size = list.size; --size;` (`0x31bc90`) |
| `fdivr [1.0]; fmulp` | A named reciprocal: `float const inverse = 1.0f / sqrt(x)` |
| The shared loop step right after the first branch | The step written in each branch; the compiler merges into the first copy (`0x3072f0`) |
| A fresh `xor reg, reg` on a failure path, result pointer in eax | An inline member returning a literal, in a function that returns the pointer it was given (`0x307f90`) |
| A member address in the wrong register and stores in the wrong order | The member is really a base class: pass `this` (`0x3173f0`) |
| Several values loaded before a sum that ours folds | A constructor taking them as parameters; parameter order matters (`0x30e720`) |
| `sub reg, K; mov reg, [reg + N]` unfolded | A pointer local stepped back in place inside a helper (`0x30e720`) |
| `mov eax, [mem]; cmp eax, ecx` where ours has `cmp [mem], eax` | The bound read through an accessor (`0x2fb150`) |
| A call and its EH state that ours lacks | An empty destructor defined in the same file is dropped; define it in another file (`0x30e720`) |
| A stack cell written and never read, its address never taken | The value is an argument for a parameter the callee does not read: the whole-program pass drops the argument and the stores stay. A local that nothing reads loses its stores (`0x279410`) |
| A pointer parameter loaded from its home at each use, the other registers placed as if it had no say | A one-member record passed by value; it is not a register candidate (`0x279410`) |

### Things that did not work

- The permuter on these functions: almost nothing in many hours.
- Reordering statements to fix register differences. Registers follow use
  counts and the rotation, not statement order.
- Duplicate `if` guards (only loop guards move registers).
- Compiler flags for the folding cases: `/G5`, `/G6`, `/G7`, `/O1`, `/Oa`,
  `/Ow` all fold the same way.

## Game functions

The same loop works on game code with two changes that the planning session
for it made: the score comes from the reduced image, and nothing is proposed
without a full image compared with a baseline. `0xe8c10` was matched this way
with the reference barrier; the comparison showed that function gained and no
other function's score changed.

## Limits

- These are observations from about fifty functions. A rule that held ten
  times can still fail on the eleventh; check it against the listing.
- A few functions survived hundreds of variants with every technique here
  (`0x2f3030`, `0x300290`, `0x3072f0`). Their differences depend on the
  context a helper is inlined into, not on how the helper is written.
- The work in this note was done without the XDK, so the Havok results were
  checked with `tools/check.py --fast` and the full check was left to the
  maintainers.
