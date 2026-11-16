#ifndef REFERENCE_BASE_318970_H
#define REFERENCE_BASE_318970_H
// Havok (third-party physics library): the reference-counted object base
// (vtable 0x452c30, the size of its block and a reference count) with its
// destructor out of line (0x318970), as the destructors of the classes
// derived from it call it.
#include "reference_counted_2d88a0.h"

class reference_base_318970
{
public:
	// built with one reference
	reference_base_318970() { reference_count = 1; }
	// built leaving the count as it is (0x318bb0's contact manager: retail
	// stores no count there)
	enum count_untouched_318970 { count_untouched };
	explicit reference_base_318970(count_untouched_318970) {}

	unsigned short memory_size;
	unsigned short reference_count;

	virtual ~reference_base_318970();
};

#endif
