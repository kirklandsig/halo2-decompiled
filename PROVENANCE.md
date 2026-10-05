# Provenance

This file describes where the information in this repository comes from, what
contributions may and may not contain, and known provenance issues that are
being worked on. The aim is that everything in the repository can be checked
by another researcher who has a retail copy of Halo 2 they are entitled to
use and the project's public tools: every recreated function is compared byte
for byte with the retail executable, and nothing in the repository depends on
leaked or internal material.

## Sources contributions may use

- Analysis of the retail Halo 2 executable from a copy the contributor is
  entitled to use: disassembly, decompilation, the call graph, data tables,
  and strings present in that executable.
- The program's observable behaviour, and runtime testing and debugging of the
  retail game.
- The project's own tools and their output: function discovery
  (`tools/functions.py`), the build and the byte-for-byte checker
  (`tools/check.py`).
- Publicly released documentation, and source code published by its authors
  under a licence that permits reuse.
- Independent research by others that was published openly, with credit. An
  example is BirchWoodGod/halo2-decomp's published analysis (see the README's
  Credits section); each lead taken from it was verified against the retail
  code.

## What contributions must not contain or copy

Contributions must not contain, or copy into the repository, code, symbols,
names, strings, comments, data or other material from:

- leaked Halo or Halo 2 source code, or any leaked Microsoft, Bungie or
  343 Industries source code;
- leaked program databases (PDBs) or leaked linker maps, or symbol data
  derived from them, including data propagated from them by third-party
  tools;
- internal or unreleased builds, including the symbols, assertion strings or
  other text inside them;
- confidential or internal documentation, material obtained in breach of an
  NDA, or stolen development material;
- proprietary SDK/XDK files.

Everything in a contribution must stand on its own: code verified against the
retail executable, and names that describe what the code does.

## How each kind of information enters the project

| Information | How it is derived |
| --- | --- |
| Function boundaries and the call graph | `tools/functions.py`, from the retail executable alone |
| Which code is game code and which is library code | Library byte signatures (`tools/libsig.py`) matched against the contributor's own SDK libraries, plus the rules in `config/owners.json` and the third-party name data described under "Known issues" |
| Calling conventions, structure layouts, behaviour | Contributors' own analysis of the retail code, checked by `tools/check.py` |
| Function, file, type and field names | Should be descriptive names chosen by contributors from the code's behaviour, or `function_<address>`/`unknown_<address>` placeholders. See "Known issues" for the names that were renamed |
| Match status | `tools/check.py`, by comparing the build's bytes with the retail executable |

## Known issues under review

The maintainers have found that some names in the repository were chosen with
the help of sources that this policy does not permit. These have been audited
and remediated as described below; item 3 is still under review.

1. **`config/functions.csv`, `name` and `object` columns.** These were taken
   from the third-party project halo-symbol-atlas. Its names for the retail
   executable were propagated from linker maps of Bungie's May 2003 Halo 2
   builds, which were not publicly released by the rights holder. About
   2,559 rows carried a name; most were SDK library functions, and about 90
   were game functions. About 1,854 rows carried an object file name.
   *Done:* that data has been removed, and the tools no longer import or
   read it. The `object` column is empty. The `name` column holds only the
   names of library functions (1,200 rows), re-derived independently from
   their signatures in the SDK libraries (`tools/libsig.py`); game functions
   are unnamed until a contributor names them from their own analysis. About
   1,100 library rows (mostly Havok) keep an `owner` that the independent
   rules do not yet reproduce.
2. **Names in `src/` and `include/`.** Some function, file, type and field names
   in the recreated source were chosen with the help of:
   - the atlas names above (about 68 of the 85 game-function names appear in
     the source);
   - object file names and function names from the same 2003 linker maps,
     which were used to group functions into source files and to name those
     files (pull request descriptions refer to these maps as the "2003
     profile/debug maps");
   - an internal 2007 debug build of Halo 2 (Title Update 1.5) that was not
     publicly released by the rights holder; its assertion strings were used
     as hints for source file names, parameter names and field names.

   *Done:* the identifiers and file names have been renamed. The recreated
   code itself was written from the retail executable and is checked against
   it byte for byte, and the renames left every function's match status
   unchanged. The method:
   - Every identifier and file name in `src/` and `include/` was compared
     with the names in those sources: the atlas names and the object and
     function names of the 2003 maps (with their class qualifiers and the
     types inside their decorated names), the identifiers in the contributor
     notes that cited the maps, and the identifiers and source file names in
     the debug build's strings. Debug-build matches did not count when the
     name was already in the source before that build was first used, or
     when the retail executable's own strings contain it.
   - 703 identifiers that matched exactly were renamed, whole word, in code
     and comments: functions to `function_<address>`, classes to
     `c_class_<address>`, fields to `field_<offset>`, and types, parameters,
     locals, globals and macros to hash-based placeholders. 44 widely used
     names received descriptive names chosen from what the code does, such as
     `point3f`, `string_handle` and `s_record_pool`.
   - 120 files were renamed to `unknown_<lowest address in the file>`, and
     the documents named after them followed.
   - Every later merge is rescanned the same way, comments and documents
     included. On 2026-10-05, a rescan renamed another 40 identifiers and 18
     files that recent work had introduced.
   - What was deliberately kept: single common words (`damage`, `path`,
     `units`, `distance3d` and the like), generic idioms made of common words
     with a generic suffix (`entity_index`, `edge_count`, `control_flags`,
     `cpu_size`, `motion_type`, `TEST_BIT`, `SET_BIT`), names listed under
     public SDK or C runtime libraries, and the SDK's own names (`XONLINE_USER`
     and the `ITitleXHV` methods). Distinctive compounds, Halo-specific terms
     and anything shaped like a function name were renamed even when generic
     in form.
   - Strings that are data in the retail executable (object type names such
     as `"light_fixture"`) were left alone.

   The comparison is by exact name, so a name that was altered slightly
   before it was committed is not caught; please report any such name.
3. **Halo CE reference names.** Some names follow the public Halo: Combat
   Evolved decompilation (punpckhdq/halo). How that project derived its names
   is being reviewed.
4. **Contributor documentation.** Several files in `docs/`, written by
   contributors, described using the 2003 linker maps to identify functions
   and file boundaries. Those passages have been removed, and the names the
   documents use, and the documents' own file names, were renamed with item 2.

## The toolchain

Matching the retail bytes requires the same compiler that built the retail
executable, which is part of a proprietary Microsoft SDK. The project does
not distribute it or explain how to obtain it. Contributors are responsible
for ensuring that any development tools they use are obtained and used
lawfully. Contributors without such a toolchain can still help with analysis
and documentation.

## Reporting a concern

If you believe material in this repository was derived from a source this
policy does not permit, please open an issue naming the files or commits
concerned, or see [LEGAL.md](LEGAL.md) for how to contact the maintainers.
