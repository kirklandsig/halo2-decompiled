#ifndef SERVICE_STREAMS_2D8780_H
#define SERVICE_STREAMS_2D8780_H
#include "reference_base_318970.h"
#include "sized_reference_a9ef0.h"
#include "stream_holder_2d8e70.h"

class class_2da270 : public reference_base_318970
{
public:
	REFERENCED_NEW_2D88A0(0x13)
};
class debug_writer_2d7c00;
class stream_writer_2d9e40;

// The stateless factory has three operations after its deleting destructor.
class debug_stream_factory_2d76f0 : public class_2da270
{
public:
	virtual debug_writer_2d7c00 *open(long stream_id);
	virtual c_sized_reference_a9ef0 *open_reader(char const *name);
	virtual stream_writer_2d9e40 *open_writer(char const *name);
};

// Existing prefix view used by the stream-pair constructor.
class standard_stream_factory_2d7570 : public c_sized_reference_a9ef0
{
public:
	virtual referenced_2d88a0 *open(long stream_id) = 0;
};
class c_stream_pair_2d7650 : public c_sized_reference_a9ef0
{
public:
	REFERENCED_NEW_2D88A0(0x13)
	c_stream_pair_2d7650();
	virtual stream_holder_2d8e70 *first_stream();
	virtual stream_holder_2d8e70 *second_stream();
	stream_holder_2d8e70 first;
	stream_holder_2d8e70 second;
};

class path_2d7f70
{
public:
	char const *function_2d7f70(char const *path) const;
	void function_2d7fb0(char const *text) const;
};
#endif
