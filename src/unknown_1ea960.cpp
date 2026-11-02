#include "unknown_11c920.h"
#include "unknown_1efac0.h"
#include "unknown_1eb350.h"
#include "havok_reference.h"
#include "physics_part_tables.h"
#include "impacts.h"
#include <new>

// @flags /O2 /Gr

/* a physics model's shape: c_a's header, 0x80 at +6 and the tag element
   it belongs to (or 0) at +8 */
struct c_model_shape : c_a
{
	long element;

	c_model_shape() : element(0) {}
	c_model_shape(void *owner) : element((long)owner) {}
};

struct c_1ea960 : c_model_shape
{
	c_1ea960() { unknown06 = 0x80; }
	c_1ea960(void *owner) : c_model_shape(owner) { unknown06 = 0x80; }
	virtual ~c_1ea960();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea960
c_1ea960::~c_1ea960()
{
	__asm int 3
	__assume(0);
}

struct c_1ea970 : c_model_shape
{
	c_1ea970() { unknown06 = 1; }
	c_1ea970(void *owner) : c_model_shape(owner) { unknown06 = 0x80; }
	virtual ~c_1ea970();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea970
c_1ea970::~c_1ea970()
{
	__asm int 3
	__assume(0);
}

struct c_1ea980 : c_model_shape
{
	c_1ea980() { unknown06 = 0x80; }
	c_1ea980(void *owner) : c_model_shape(owner) { unknown06 = 0x80; }
	virtual ~c_1ea980();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea980
c_1ea980::~c_1ea980()
{
	__asm int 3
	__assume(0);
}

struct c_1ea990 : c_model_shape
{
	byte unknown0c[0x24];
	void *vertices;
	long vertex_count;
	dword vertex_capacity;
	byte unknown3c[4];
	byte inline_vertices[0x90];
	byte unknownd0[4];
	void *planes;
	long plane_count;

	c_1ea990() { unknown06 = 0x80; }
	c_1ea990(void *owner, void *first, void *second) : c_model_shape(owner),
		vertices(first), planes(second) { unknown06 = 0x80; }
	virtual ~c_1ea990();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea990
c_1ea990::~c_1ea990()
{
	__asm int 3
	__assume(0);
}

/* a child of a list shape: before the shapes are built, the key of its shape */
struct s_list_child
{
	union
	{
		s_physics_model_shape_key key;
		void *shape;
	};
	long filter;
};

struct c_1ea9a0 : c_model_shape
{
	s_list_child *children;
	long count;
	dword capacity;
	s_list_child inline_children[4];

	c_1ea9a0() { unknown06 = 0x80; }
	c_1ea9a0(s_list_child *list) : children(list) { unknown06 = 0x80; }
	virtual ~c_1ea9a0();
	virtual void *v3() { return 0; }
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual long child_count() { return 0; }
};

// @retail 0x1ea9a0
c_1ea9a0::~c_1ea9a0()
{
	__asm int 3
	__assume(0);
}

struct c_1ea9b0 : c_model_shape
{
	union
	{
		s_physics_model_shape_key child_key;
		void *child;
	};
	byte *code;

	c_1ea9b0() { unknown06 = 0x80; }
	c_1ea9b0(void *list, byte *data) : child(list), code(data) { unknown06 = 0x80; }
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
	c_shape_pair() : field_8(0) { unknown06 = 0x80; }
	c_shape_pair(c_havok_reference_counted *a, c_havok_reference_counted *b) : field_8(0),
		first(a), second(b) { unknown06 = 0x80; }
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
 word field_4;
 word field_6;
 long kind;
 c_shape_contact_listener(long value) : kind(value) { field_6 = 0x80; }
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

struct s_shape_block_table;
void *function_1eaf30(short type, s_shape_block_table *table, short index);

/* builds the shapes of a physics model in place: each shape gets its tag
   element and the shapes it refers to (their keys resolved to pointers); with
   `restore` set they are constructed empty and keep what they hold */
// score 138 (was a stub): retail copies the tree's child key to the stack and shifts the index
// out of it (ours reads the high word), reads the listener's value before the pair's shape lookup
// (ours after, at its constructor), and inside the inlined constructors stores the empty 0x455ef0
// shape's +6 before +8 and the vertex/plane pointers of 0x456008 and the children of 0x455fc0
// before the virtual table (ours after it). Tried: one counter for every loop 456 (1419 bytes);
// conditional offset adds 311 (1404); both 307; the tree's key read through its union 289 (1388);
// with the key copy, the 0x455ef0 shape as the new-expression's result 307, by a pointer in its
// build arm 17; that pointer declared for both arms (no key copy) 303;
// the pair on c_model_shape 14 (0x1eb4c0 MATCH -> 51, reverted). One-use locals, not kept without
// a match: the tree's key copied to a local 79, the listener's value and the shape lookup result
// held in locals before the pair 123, both 17; the lookup result named at the pair alone 138.
// @retail 0x1ea9c0
void function_1ea9c0(long tag_index, bool restore)
{
	byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
	s_shape_block_table *table = (s_shape_block_table *)definition;

	if (!restore)
	{
		for (long i = 0; i < *(long *)(definition + 0x38); i++)
		{
			byte *body = *(byte **)(definition + 0x3c) + i * 0x90;
			s_physics_model_shape_key key = *(s_physics_model_shape_key *)(body + 0x38);
			*(void **)(body + 0x38) = function_1eaf30(key.type, table, key.index);
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x48); i++)
	{
		s_pair_element *pair = (s_pair_element *)(*(byte **)(definition + 0x4c) + i * 0x80);
		if (restore)
		{
			new (&pair->a) c_part_a;
			new (&pair->b) c_part_b;
		}
		else
		{
			new (&pair->a) c_part_a(pair);
			new (&pair->b) c_part_b(&pair->a);
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x50); i++)
	{
		byte *entry = *(byte **)(definition + 0x54) + i * 0xb0;
		if (restore)
			new (entry + 0x20) c_1ea960;
		else
			new (entry + 0x20) c_1ea960(entry);
	}
	for (long i = 0; i < *(long *)(definition + 0x58); i++)
	{
		s_single_element *single = (s_single_element *)(*(byte **)(definition + 0x5c) + i * 0x50);
		if (restore)
			new (&single->c) c_part_c;
		else
			new (&single->c) c_part_c(single);
	}
	for (long i = 0; i < *(long *)(definition + 0x60); i++)
	{
		byte *entry = *(byte **)(definition + 0x64) + i * 0x90;
		if (restore)
		{
			new (entry + 0x20) c_1ea970;
			new (entry + 0x40) c_part_b;
		}
		else
		{
			c_1ea970 *shape = (c_1ea970 *)(entry + 0x20);
			new (shape) c_1ea970(entry);
			new (entry + 0x40) c_part_b((c_part *)shape);
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x68); i++)
	{
		byte *entry = *(byte **)(definition + 0x6c) + i * 0x60;
		if (restore)
			new (entry + 0x20) c_1ea980;
		else
			new (entry + 0x20) c_1ea980(entry);
	}
	long vertex_offset = 0;
	long plane_offset = 0;
	for (long i = 0; i < *(long *)(definition + 0x70); i++)
	{
		byte *entry = *(byte **)(definition + 0x74) + i * 0x100;
		c_1ea990 *shape = (c_1ea990 *)(entry + 0x20);
		if (restore)
			new (shape) c_1ea990;
		else
		{
			bool inline_vertices = (shape->vertex_capacity & 0x7fffffff) <= 3;
			void *vertices = inline_vertices ? shape->inline_vertices :
				*(byte **)(definition + 0x7c) + vertex_offset * 0x30;
			void *planes = *(byte **)(definition + 0x84) + plane_offset * 0x10;
			new (shape) c_1ea990(entry, vertices, planes);
			vertex_offset += inline_vertices ? 0 : shape->vertex_count;
			plane_offset += shape->plane_count;
		}
	}
	long child_offset = 0;
	for (long i = 0; i < *(long *)(definition + 0x90); i++)
	{
		c_1ea9a0 *list = (c_1ea9a0 *)(*(byte **)(definition + 0x94) + i * 0x38);
		if (restore)
			new (list) c_1ea9a0;
		else
		{
			bool inline_children = (list->capacity & 0x7fffffff) <= 4;
			s_list_child *children = inline_children ? list->inline_children :
				(s_list_child *)(*(byte **)(definition + 0x9c)) + child_offset;
			new (list) c_1ea9a0(children);
			for (long j = 0; j < list->child_count(); j++)
			{
				s_physics_model_shape_key key = children[j].key;
				children[j].shape = function_1eaf30(key.type, table, key.index);
				children[j].filter = 0;
			}
			child_offset += inline_children ? 0 : list->count;
		}
	}
	long code_offset = 0;
	for (long i = 0; i < *(long *)(definition + 0xa0); i++)
	{
		c_1ea9b0 *tree = (c_1ea9b0 *)(*(byte **)(definition + 0xa4) + i * 0x14);
		if (restore)
			new (tree) c_1ea9b0;
		else
		{
			byte *list = *(byte **)(definition + 0x94) + tree->child_key.index * 0x38;
			byte *code = *(byte **)(definition + 0xac) + code_offset;
			new (tree) c_1ea9b0(list, code);
			code_offset += (*(long *)(code + 0x20) + 15) & ~15;
		}
	}
	for (long i = 0; i < *(long *)(definition + 0x108); i++)
	{
		byte *entry = *(byte **)(definition + 0x10c) + i * 0x20;
		c_shape_contact_listener *listener = (c_shape_contact_listener *)(entry + 0x14);
		if (restore)
		{
			new (entry) c_shape_pair;
			new (listener) c_shape_contact_listener(0);
		}
		else
		{
			s_physics_model_shape_key key = *(s_physics_model_shape_key *)(entry + 0xc);
			new (entry) c_shape_pair((c_havok_reference_counted *)function_1eaf30(key.type, table, key.index),
				(c_havok_reference_counted *)listener);
			new (listener) c_shape_contact_listener(listener->kind);
		}
	}
}
