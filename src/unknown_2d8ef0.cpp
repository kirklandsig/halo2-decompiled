// @flags /O2 /GL-
// Havok (third-party physics library): writing one character to a stream.
#include "stream_holder_2d8e70.h"

class class_2d8ef0_stream
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual int write(const void *data, int size) = 0;
};

// @retail 0x2d8ef0
stream_holder_2d8e70 *stream_holder_2d8e70::append_character(char value)
{
	((class_2d8ef0_stream *)stream)->write(&value, 1);
	return this;
}
