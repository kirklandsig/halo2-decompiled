// @flags /O2 /GL-
// These output callbacks have no effect in retail. Its dispatch retains four
// separate cases and a default call, all reaching the shared ret at 0x175f40.
// Separate compilation retains those calls; identical-code folding shares
// their empty bodies with the game's existing empty callback.
void __cdecl report_level_zero_2d8080(char const *) {}
void __cdecl report_level_one_2d8080(char const *) {}
void __cdecl report_level_two_2d8080(char const *) {}
void __cdecl report_level_three_2d8080(char const *) {}
void __cdecl report_unknown_level_2d8080() {}
