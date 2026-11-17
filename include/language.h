#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "unknown_11c920.h"
#include <xtl.h>

/* the language the game's text is in, NONE until first asked (g_47ff38,
   globals.cpp); unknown_11c9c0.cpp converts XGetLanguage's value */
extern long g_47ff38;

/* the game's languages: function_11ca80 maps XGetLanguage's XC_LANGUAGE_*
   values onto these (Portuguese and unknown values become English). The
   localized tables in src/saved_game_storage.cpp also have an entry for 8,
   which nothing maps to; it gets the English text. */
enum e_language
{
	_language_english,
	_language_japanese,
	_language_german,
	_language_french,
	_language_spanish,
	_language_italian,
	_language_korean,
	_language_chinese
};

long function_11ca80(long value);

inline long get_current_language(void)
{
	if (g_47ff38 == NONE)
	{
		g_47ff38 = function_11ca80(XGetLanguage());
	}
	return g_47ff38;
}

#endif
