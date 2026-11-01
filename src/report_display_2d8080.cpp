// @flags /O2 /GL- /GX
#include "error_handler_2d8030.h"

void __cdecl report_level_zero_2d8080(char const *text);
void __cdecl report_level_one_2d8080(char const *text);
void __cdecl report_level_two_2d8080(char const *text);
void __cdecl report_level_three_2d8080(char const *text);
void __cdecl report_unknown_level_2d8080();

// Format a source location, report id, title and message, then dispatch by
// level. The retail output callbacks all share an empty implementation.
// @retail 0x2d8080
void error_handler_2d8030::show(long level, char const *title, long id,
	char const *message, char const *file, long line, flag_2d8e70 flag)
{
	char id_text[12];
	format_2d8cc0(id_text, "0x%x", id);
	char buffer[0x200];
	stream_holder_2d8e70 stream(buffer, sizeof(buffer), true);
	stream.function_2d8f10(file)->append_character('(')->function_2d8f60(line)
		->function_2d8f10("): [")->function_2d8f10(id_text)->function_2d8f10("] ")
		->function_2d8f10(title)->function_2d8f10(" : '")->function_2d8f10(message)
		->function_2d8f10("'\n");
	switch (level)
	{
	case 0: report_level_zero_2d8080(buffer); break;
	case 1: report_level_one_2d8080(buffer); break;
	case 2: report_level_two_2d8080(buffer); break;
	case 3: report_level_three_2d8080(buffer); break;
	default: report_unknown_level_2d8080(); break;
	}
}
