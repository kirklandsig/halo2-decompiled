// @flags /O2 /GL-
// Contact-id indirection and collection. This is only the accessed storage
// prefix, not a reconstructed virtual interface or complete object layout.
#include "contact_manager_318c10.h"
// @retail 0x318f10
s_contact_point_318f10 *class_318e60::point(unsigned short id)
{
	return (s_contact_point_318f10 *)constraint.points.data + constraint.ids.data[id];
}
// @retail 0x318f30
s_contact_property_318f30 *class_318e60::property(unsigned short id)
{
	return (s_contact_property_318f30 *)constraint.properties.data + constraint.ids.data[id];
}
// @retail 0x318f50
void s_contact_ids_318f50::collect(array_2d91d0 *out) const
{
	for (long i = 0; i < size; ++i)
	{
		if (data[i] != 0xff)
		{
			if (out->size == (out->capacity_and_flags & 0x7fffffff))
				function_2d91d0(out,2);
			((unsigned short *)out->data)[out->size] = (unsigned short)i;
			out->size++;
		}
	}
}
// @retail 0x318fa0
void class_318e60::collect(array_2d91d0 *out) const
{
	((s_contact_ids_318f50 const *)&constraint.ids)->collect(out);
}
