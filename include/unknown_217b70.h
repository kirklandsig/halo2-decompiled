#ifndef UNKNOWN_217B70_H
#define UNKNOWN_217B70_H

/* length of a wide string, at most 255 characters */
inline dword function_217b70(word const *text)
{
	dword length;
	for (length = 0; length < 255; length++)
	{
		if (!text[length])
			break;
	}
	return length;
}

#endif
