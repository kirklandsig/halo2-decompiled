// @flags /O2 /GL- /GX
#include "service_streams_2d8780.h"
#include "error_handler_2d8030.h"
#include "buffered_reader_2d9c00.h"
#include "stdio_writer_2d7aa0.h"
#include "unknown_146a20.h"

static __forceinline void initialize_service_array(array_2d91d0 *array)
{
	array->data = 0;
	array->size = 0;
	array->capacity_and_flags = 0x80000000;
}
class default_error_handler_2d8780 : public error_handler_2d8030
{
public:
	default_error_handler_2d8780()
	{
		initialize_service_array((array_2d91d0 *)&counts);
		initialize_service_array((array_2d91d0 *)&silenced);
	}
	// Unreconstructed repeated slots retain main's partial interface.
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}

};

class bootstrap_service_2d8780
{
public:
	virtual void slot0() = 0;
	virtual void initialize() = 0;
};
// Populated by service registration; its registration records are not modeled here.
bootstrap_service_2d8780 *g_480148;
extern unsigned char g_48014c;
class reporter_2ff210;
class error_handler_2d97d0;
c_havok_abstract_allocator *__fastcall function_1473b0();
void __cdecl function_2d6fe0(c_havok_abstract_allocator *);
void __cdecl function_2d74d0(c_sized_reference_a9ef0 *);
void __cdecl function_2d7460(reporter_2ff210 *);
void __cdecl function_2d73f0(error_handler_2d97d0 *);
void __cdecl initialize_registered_services_2d7ca0();

// @retail 0x2d8780
int __cdecl function_2d8780()
{
	if (flag_2d8e70(g_48014c == 0).value)
	{
		function_2d6fe0(function_1473b0());
		function_2d74d0((c_sized_reference_a9ef0 *)new debug_stream_factory_2d76f0);
		function_2d7460((reporter_2ff210 *)new c_stream_pair_2d7650);
		function_2d73f0((error_handler_2d97d0 *)new default_error_handler_2d8780);
		initialize_registered_services_2d7ca0();
		g_480148->initialize();
		g_48014c = 1;
	}
	return 0;
}
