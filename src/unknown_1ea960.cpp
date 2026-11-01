#include "unknown_11c920.h"
#include "unknown_1efac0.h"
#include "unknown_1eb350.h"
#include "havok_reference.h"

// @flags /O2 /Gr

#if 0 // Disabled: activating it may cost neighbouring matches (not checked with the original compiler).
#include <new>
// The shapes are constructed in place, in the tag's own data. Each class is named by its
// retail virtual table. Every one starts with the same header: the table, the word 0x80 at +6
// (0x455ef0's restored shapes take 1) and a dword at +8. A restored tag (restore true) gets
// only the header, with 0 at +8: retail leaves every other field as it is. A tag that has
// just loaded gets the pointers too.
struct s_shape_block_table;
void *function_1eaf30(short type, s_shape_block_table *table, short index);

class c_shape_header
{
public:
	virtual ~c_shape_header() {}
	c_shape_header() : value08(0) { value06 = 0x80; }
	c_shape_header(void *value) : value08(value) { value06 = 0x80; }
	byte unknown04[2];
	word value06;
	void *value08;
};

/* the first shape of a pair (physics_part_tables.cpp's c_part_a) */
class c_shape_4554c0 : public c_shape_header
{
public:
	virtual ~c_shape_4554c0();
	c_shape_4554c0() {}
	c_shape_4554c0(void *definition) : c_shape_header(definition) {}
};

/* the second shape of a pair, pointing at the first (c_part_b) */
class c_shape_455498 : public c_shape_header
{
public:
	virtual ~c_shape_455498();
	c_shape_455498() {}
	c_shape_455498(void *owner) : owner(owner) {}
	void *owner;
};

/* a single shape (c_part_c) */
class c_shape_455500 : public c_shape_header
{
public:
	virtual ~c_shape_455500();
	c_shape_455500() {}
	c_shape_455500(void *definition) : c_shape_header(definition) {}
};

class c_shape_456088 : public c_shape_header
{
public:
	virtual ~c_shape_456088();
	c_shape_456088() {}
	c_shape_456088(void *definition) : c_shape_header(definition) {}
};

/* the shape of the fourth block: restored ones get 1 at +6 */
class c_shape_455ef0 : public c_shape_header
{
public:
	virtual ~c_shape_455ef0();
	c_shape_455ef0() { value06 = 1; }
	c_shape_455ef0(void *definition) : c_shape_header(definition) {}
};

class c_shape_456048 : public c_shape_header
{
public:
	virtual ~c_shape_456048();
	c_shape_456048() {}
	c_shape_456048(void *definition) : c_shape_header(definition) {}
};

class c_shape_456008 : public c_shape_header
{
public:
	virtual ~c_shape_456008();
	c_shape_456008() {}
	c_shape_456008(void *definition, void *first, void *second) : c_shape_header(definition)
	{
		*(void **)(unknown0c + 0x24) = first;
		*(void **)(unknown0c + 0xc8) = second;
	}
	byte unknown0c[0xd8 - 0xc];
};

/* a shape with child shapes: its eleventh virtual function (+0x28) is the child count */
class c_shape_455fc0 : public c_shape_header
{
public:
	virtual ~c_shape_455fc0();
	virtual void v1();
	virtual void v2();
	virtual void v3();
	virtual void v4();
	virtual void v5();
	virtual void v6();
	virtual void v7();
	virtual void v8();
	virtual void v9();
	virtual long child_count();
	c_shape_455fc0() {}
	c_shape_455fc0(void *children) : children(children) {}
	void *children;
};

class c_shape_455f58 : public c_shape_header
{
public:
	virtual ~c_shape_455f58();
	c_shape_455f58() {}
	c_shape_455f58(void *child, void *data) : child(child), data(data) {}
	void *child;
	void *data;
};

class c_shape_455f30 : public c_shape_header
{
public:
	virtual ~c_shape_455f30();
	c_shape_455f30() {}
	c_shape_455f30(void *child, void *linked) : child(child), linked(linked) {}
	void *child;
	void *linked;
};

class c_shape_455f8c : public c_shape_header
{
public:
	virtual ~c_shape_455f8c();
	c_shape_455f8c() {}
	c_shape_455f8c(long saved_value) : c_shape_header((void *)saved_value) {}
};

// @retail 0x1ea9c0
void function_1ea9c0(long tag_index, long restore)
{
	byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
	s_shape_block_table *table = (s_shape_block_table *)definition;
	if (!(byte)restore)
		for (long i = 0; i < *(long *)(definition + 0x38); ++i)
		{
			byte *entry = *(byte **)(definition + 0x3c) + i * 0x90;
			long key = *(long *)(entry + 0x38);
			*(void **)(entry + 0x38) = function_1eaf30((short)key, table, (short)(key >> 16));
		}
	for (long i = 0; i < *(long *)(definition + 0x48); ++i)
	{
		byte *entry = *(byte **)(definition + 0x4c) + i * 0x80;
		if ((byte)restore)
		{
			new (entry + 0x20) c_shape_4554c0;
			new (entry + 0x30) c_shape_455498;
		}
		else
		{
			new (entry + 0x20) c_shape_4554c0(entry);
			new (entry + 0x30) c_shape_455498(entry + 0x20);
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x50); ++i)
	{
		byte *entry = *(byte **)(definition + 0x54) + i * 0xb0;
		if ((byte)restore) new (entry + 0x20) c_shape_456088;
		else new (entry + 0x20) c_shape_456088(entry);
	}
	for (long i = 0; i < *(long *)(definition + 0x58); ++i)
	{
		byte *entry = *(byte **)(definition + 0x5c) + i * 0x50;
		if ((byte)restore) new (entry + 0x20) c_shape_455500;
		else new (entry + 0x20) c_shape_455500(entry);
	}
	for (long i = 0; i < *(long *)(definition + 0x60); ++i)
	{
		byte *entry = *(byte **)(definition + 0x64) + i * 0x90;
		if ((byte)restore)
		{
			new (entry + 0x20) c_shape_455ef0;
			new (entry + 0x40) c_shape_455498;
		}
		else
		{
			new (entry + 0x20) c_shape_455ef0(entry);
			new (entry + 0x40) c_shape_455498(entry + 0x20);
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x68); ++i)
	{
		byte *entry = *(byte **)(definition + 0x6c) + i * 0x60;
		if ((byte)restore) new (entry + 0x20) c_shape_456048;
		else new (entry + 0x20) c_shape_456048(entry);
	}
	long first_count = 0, second_count = 0;
	for (long i = 0; i < *(long *)(definition + 0x70); ++i)
	{
		byte *entry = *(byte **)(definition + 0x74) + i * 0x100;
		byte *shape = entry + 0x20;
		if ((byte)restore) new (shape) c_shape_456008;
		else
		{
			bool inline_first = (*(dword *)(shape + 0x38) & 0x7fffffff) <= 3;
			byte *first = inline_first ? shape + 0x40 : *(byte **)(definition + 0x7c) + first_count * 0x30;
			byte *second = *(byte **)(definition + 0x84) + second_count * 0x10;
			new (shape) c_shape_456008(entry, first, second);
			if (!inline_first) first_count += *(long *)(shape + 0x34);
			second_count += *(long *)(shape + 0xd8);
		}
	}
	long child_count = 0;
	for (long i = 0; i < *(long *)(definition + 0x90); ++i)
	{
		byte *entry = *(byte **)(definition + 0x94) + i * 0x38;
		if ((byte)restore) new (entry) c_shape_455fc0;
		else
		{
			bool inline_children = (*(dword *)(entry + 0x14) & 0x7fffffff) <= 4;
			byte *children = inline_children ? entry + 0x18 : *(byte **)(definition + 0x9c) + child_count * 8;
			c_shape_455fc0 *shape = new (entry) c_shape_455fc0(children);
			for (long j = 0; j < shape->child_count(); ++j)
			{
				long key = *(long *)(children + j * 8);
				*(void **)(children + j * 8) = function_1eaf30((short)key, table, (short)(key >> 16));
				*(long *)(children + j * 8 + 4) = 0;
			}
			if (!inline_children) child_count += *(long *)(entry + 0x10);
		}
	}
	long data_offset = 0;
	for (long i = 0; i < *(long *)(definition + 0xa0); ++i)
	{
		byte *entry = *(byte **)(definition + 0xa4) + i * 0x14;
		if ((byte)restore) new (entry) c_shape_455f58;
		else
		{
			long key = *(long *)(entry + 0xc);
			byte *child = *(byte **)(definition + 0x94) + (key >> 16) * 0x38;
			byte *data = *(byte **)(definition + 0xac) + data_offset;
			new (entry) c_shape_455f58(child, data);
			data_offset += (*(long *)(data + 0x20) + 15) & ~15;
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x108); ++i)
	{
		byte *entry = *(byte **)(definition + 0x10c) + i * 0x20;
		if ((byte)restore)
		{
			new (entry) c_shape_455f30;
			new (entry + 0x14) c_shape_455f8c;
		}
		else
		{
			long key = *(long *)(entry + 0xc);
			long saved_value = *(long *)(entry + 0x1c);
			void *child = function_1eaf30((short)key, table, (short)(key >> 16));
			new (entry) c_shape_455f30(child, entry + 0x14);
			new (entry + 0x14) c_shape_455f8c(saved_value);
		}
	}
}
#endif

struct c_1ea960 : c_a
{
	virtual ~c_1ea960();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea960
c_1ea960::~c_1ea960()
{
	__asm int 3
	__assume(0);
}

struct c_1ea970 : c_a
{
	virtual ~c_1ea970();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea970
c_1ea970::~c_1ea970()
{
	__asm int 3
	__assume(0);
}

struct c_1ea980 : c_a
{
	virtual ~c_1ea980();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea980
c_1ea980::~c_1ea980()
{
	__asm int 3
	__assume(0);
}

struct c_1ea990 : c_a
{
	virtual ~c_1ea990();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea990
c_1ea990::~c_1ea990()
{
	__asm int 3
	__assume(0);
}

struct c_1ea9a0 : c_a
{
	virtual ~c_1ea9a0();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea9a0
c_1ea9a0::~c_1ea9a0()
{
	__asm int 3
	__assume(0);
}

struct c_1ea9b0 : c_a
{
	virtual ~c_1ea9b0();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea9b0
c_1ea9b0::~c_1ea9b0()
{
	__asm int 3
	__assume(0);
}

struct c_shape_adapter_a : c_shape_library_base_a {};
struct c_shape_adapter_b : c_shape_library_base_b {};

// @retail 0x1eb3b0 deleting c_shape_adapter_a
// @retail 0x1eb3e0 deleting c_shape_adapter_b

struct c_shape_reference : c_a
{
	long field_8;
	c_havok_reference_counted *object;
	virtual ~c_shape_reference();
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
	}
};

// @retail 0x1eb320 deleting c_shape_reference

// @retail 0x1eb350
c_shape_reference::~c_shape_reference()
{
	havok_reference_remove(object);
}

struct c_shape_pair : c_a
{
	long field_8;
	c_havok_reference_counted *first;
	c_havok_reference_counted *second;
	virtual ~c_shape_pair();
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
	}
};

// @retail 0x1ea930 deleting c_shape_pair

// @retail 0x1eb4c0
c_shape_pair::~c_shape_pair()
{
	havok_reference_remove(second);
	havok_reference_remove(first);
}

#include "unknown_1cec30.h"

long havok_entity_component_index_get(hkEntity const *entity);
long havok_entity_property_2002_get(hkEntity const *entity);
void function_1d2070(hkEntity const *entity, s_havok_component *component,
 long component_index, long kind, long shape_index);

struct s_contact_body
{
 byte field_0[0x18];
 long kind;
 long field_1c;
 hkEntity *entity;
};

struct c_shape_contact_listener
{
 long field_4;
 long kind;
 virtual void added(s_contact_body const *a, s_contact_body const *b, void *contact);
 virtual void removed(s_contact_body const *a, s_contact_body const *b);
};

// @retail 0x1eb2a0
void c_shape_contact_listener::removed(s_contact_body const *a, s_contact_body const *b)
{
 long contact_kind = kind;
 hkEntity *first = a->kind == 1 ? a->entity : NULL;
 hkEntity *second = b->kind == 1 ? b->entity : NULL;
 if (first && second)
 {
  long component_index = havok_entity_component_index_get(first);
  if (component_index != NONE)
  {
   long shape_index = havok_entity_property_2002_get(first);
   function_1d2070(second, havok_component_get(component_index), component_index,
    shape_index, contact_kind);
  }
 }
}

struct c_material_shape
{
 long field_4;
 byte *user;
 c_material_shape *child;
 virtual void v0() {}
 virtual void v1() {}
 virtual void v2() {}
 virtual void v3() {}
 virtual void v4() {}
 virtual long kind() { return 0; }
 virtual void v6() {}
 virtual void v7() {}
 virtual void v8() {}
 virtual void v9() {}
 virtual void v10() {}
 virtual long first_key(void *buffer) { return NONE; }
 virtual void v12() {}
 virtual c_material_shape *get_child(long key) { return NULL; }
};

struct s_globals_element;
s_globals_element *function_188640(long key);

// @retail 0x1eb020
long function_1eb020(long tag_index, long body_index, short *material_index)
{
 (void)&material_index;
 __m128 buffer[16];
 byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
 byte *body = *(byte **)(definition + 0x3c) + body_index * 0x90;
 c_material_shape *shape = *(c_material_shape **)(body + 0x38);
 byte *volatile definition_home = definition;
 long result = 0;
 if (shape->kind() == 0x13) shape = shape->child;
 long kind = shape->kind();
 bool collection_kind = kind == 2 || (kind > 9 && kind <= 11);
 if (collection_kind)
 {
  shape = shape->get_child(shape->first_key(buffer));
  if (shape->kind() == 0x11) shape = shape->child;
  if (shape->kind() == 0x15) shape = shape->child;
  definition = definition_home;
 }
 else
 {
  if (shape->kind() == 0x11) shape = shape->child;
  if (shape->kind() == 0x15) shape = shape->child;
 }
 byte *data = shape->user;
 short index = *(short *)(data + 4);
 if (index != NONE)
 {
  long *entry = (long *)(*(byte **)(definition + 0x44) + index * 12);
  result = entry[1];
  if (!function_188640(result)) result = entry[0];
 }
 *material_index = *(short *)(data + 6);
 return result;
}

void function_1d1e40(long component_index, s_havok_component *component, hkEntity const *entity, long kind, long shape_index);

// @retail 0x1eb220
void c_shape_contact_listener::added(s_contact_body const *a, s_contact_body const *b, void *contact)
{
 long contact_kind = kind;
 hkEntity *first = a->kind == 1 ? a->entity : NULL;
 hkEntity *second = b->kind == 1 ? b->entity : NULL;
 if (first && second)
 {
  long component_index = havok_entity_component_index_get(first);
  if (component_index != NONE)
  {
   long shape_index = havok_entity_property_2002_get(first);
   function_1d1e40(component_index, havok_component_get(component_index), second, shape_index, contact_kind);
  }
 }
}
