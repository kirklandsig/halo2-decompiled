// @flags /O2 /GL-
// Havok (third-party physics library): copying an object's settings into a
// description and back.
#include "description_312430.h"
#include "unknown_1cec30.h"

ELEMENTS_2D91D0(array_312530, s_havok_entity_property);
typedef char assert_array_312530_size[(sizeof(array_312530) == sizeof(s_havok_array)) ? 1 : -1];

// @retail 0x312430
void class_312430::function_312430(description_312430 *description) const
{
	description->field_4 = field_c;
	description->field_8 = field_24;
	description->pairs.ensure_capacity(pairs.size);
	for (int i = 0; i < pairs.size; i++)
	{
		description->pairs.append_unchecked(&pairs.data[i]);
	}
	description->field_0 = field_28;
}

// Capture the pair and destination before either store: the first write can
// alias the array header. Retail uses the same destination for both words.
// @retail 0x3124c0
void class_312430::function_3124c0(description_312430 const *description)
{
	if (field_28 != description->field_0)
	{
		field_28 = description->field_0;
	}
	pairs.resize(description->pairs.size);
	for (int i = 0; i < description->pairs.size; i++)
	{
		long index = i;
		pair_312430 saved = description->pairs[i];
		pair_312430 *items = pairs.data;
		items[index].field_0 = saved.field_0;
		items[index].field_4 = saved.field_4;
	}
}

// @retail 0x312530
void hkEntity::entity_property_put(dword key, s_havok_property_word value)
{
	for (long i = 0; i < m_property_count; i++)
	{
		if (m_properties[i].m_key == key)
		{
			return;
		}
	}
	s_havok_entity_property property;
	property.m_key = key;
	property.m_value = value;
	((array_312530 *)&m_properties)->append_copy(property);
}
