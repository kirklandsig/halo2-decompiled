// @flags /O2 /Ob1 /Gr /GL-
#include "unknown_11c920.h"
#include "globals.h"
#include "pair_filter_class.h"

class c_1c3450;
void function_1c3450(c_1c3450 *arg_0);

class c_1c3410;

/* the object's three bases each reach its deleting destructor (0x1c3410)
   through slot 0 of their vtable: the bases at +0xc and +0x10 through the
   compiler's this-adjustor thunks (0x1c58d0, 0x1c58e0) */
class c_1c3410_base
{
public:
 virtual c_1c3410 *function_1c3410(dword arg_0) { return 0; }
 word field_4;
 dword field_8;
};

class c_1c3410_interface_c
{
public:
 virtual c_1c3410 *function_1c3410(dword arg_0) { return 0; }
};

class c_1c3410_interface_10
{
public:
 virtual c_1c3410 *function_1c3410(dword arg_0) { return 0; }
};

class c_1c3410 : public c_1c3410_base, public c_1c3410_interface_c, public c_1c3410_interface_10
{
public:
 c_1c3410 *function_1c3410(dword arg_0);
};

// @retail 0x1c3410
c_1c3410 *c_1c3410::function_1c3410(dword arg_0)
{
 ((pair_filter_base *)this)->pair_filter_base::~pair_filter_base();
 if (arg_0 & 1) g_480118->allocate((long)this, field_4, 0x22);
 return this;
}

// @retail 0x1c58d0 adjustor c_1c3410::function_1c3410 0xc
// @retail 0x1c58e0 adjustor c_1c3410::function_1c3410 0x10
