#ifndef STREAM_HOLDER_2D8E70_H
#define STREAM_HOLDER_2D8E70_H
// Havok (third-party physics library): a stream over another stream, holding
// a reference to the stream beneath it. Its constructor leaves the reference
// count alone, so its base has no constructor of its own.
#include "reference_counted_2d88a0.h"

class stream_base_2d8e70
{
public:
	unsigned short memory_size;
	unsigned short reference_count;

	virtual ~stream_base_2d8e70() {}
};

// a yes-or-no argument passed as an object
struct flag_2d8e70
{
	bool value;

	flag_2d8e70(bool value_)
	{
		value = value_;
	}

	flag_2d8e70(flag_2d8e70 const &other)
	{
		value = other.value;
	}

};

class stream_holder_2d8e70 : public stream_base_2d8e70
{
public:
	REFERENCED_NEW_2D88A0(0x15)

	referenced_2d88a0 *stream;

	stream_holder_2d8e70(referenced_2d88a0 *object);
	stream_holder_2d8e70(char *buffer, long size, flag_2d8e70 zero);
	stream_holder_2d8e70 *append_character(char value);
	// write a string, or "(null)" for none, or a number, signed (0x2d8f60) or
	// not (0x2d8fb0); each gives back the stream (a pointer, not a reference,
	// as the build's stand-ins cannot keep a reference)
	stream_holder_2d8e70 *function_2d8f10(char const *text);
	stream_holder_2d8e70 *function_2d8f60(int value);
	stream_holder_2d8e70 *function_2d8fb0(unsigned int value);
	// writes formatted text
	void function_2d9000(char const *format, ...);
	~stream_holder_2d8e70();
};

#endif
