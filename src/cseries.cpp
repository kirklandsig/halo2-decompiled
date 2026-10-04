// @flags /O2 /Gr
/* CSERIES.CPP: case-insensitive string comparison.
   See docs/cseries.md for the symbol mapping. Existing bounded-length and
   formatting helpers remain in their upstream translation units. */

#include "cseries.h"
#include <wctype.h>

// @retail 0x11c920
int csstricmp(char const *s1, char const *s2)
{
	int c1 = towlower(*s1);
	int c2 = towlower(*s2);
	while (c1 && c2 && c1 == c2)
	{
		c1 = towlower(*++s1);
		c2 = towlower(*++s2);
	}
	if (!c1)
		return c2 ? -1 : 0;
	if (!c2)
		return c1 ? 1 : 0;
	return c1 > c2 ? 1 : -1;
}
