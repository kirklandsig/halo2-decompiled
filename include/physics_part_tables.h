/* PHYSICS_PART_TABLES.H: the shape objects that tags hold in place
   (physics_part_tables.cpp constructs the biped and creature ones,
   unknown_1ea960.cpp those of a physics model). */

#ifndef PHYSICS_PART_TABLES_H
#define PHYSICS_PART_TABLES_H

#include "unknown_11c920.h"

/* a shape object: a virtual table, the value 0x80 at +6 and 0 (or the tag
   element it belongs to) at +8 */
class c_part
{
public:
	virtual ~c_part() {}
	c_part() : value08(0) {}
	c_part(void *element) : value08((long)element) {}
	byte unknown04[2];
	word value06;
	long value08;
};

/* the first shape of a pair */
class c_part_a : public c_part
{
public:
	virtual ~c_part_a();
	c_part_a() { value06 = 0x80; }
	c_part_a(void *element) : c_part(element) { value06 = 0x80; }
	byte unknown0c[4];
};

/* a shape that points at another */
class c_part_linked : public c_part
{
public:
	c_part_linked() {}
	c_part_linked(c_part *a) : owner(a) {}
	c_part *owner;
};

/* the second shape of a pair, pointing at the first (constructed without
   one, it keeps the pointer it holds) */
class c_part_b : public c_part_linked
{
public:
	virtual ~c_part_b();
	c_part_b() { value06 = 0x80; }
	c_part_b(c_part *a) : c_part_linked(a) { value06 = 0x80; }
};

/* the shape of a single element */
class c_part_c : public c_part
{
public:
	virtual ~c_part_c();
	c_part_c() { value06 = 0x80; }
	c_part_c(void *element) : c_part(element) { value06 = 0x80; }
	byte unknown0c[0x24];
};

/* an element of the first and third blocks (0x80 bytes): a pair of shapes */
struct s_pair_element
{
	byte unknown00[0x20];
	c_part_a a;
	c_part_b b;
	byte unknown40[0x40];
};

/* an element of the second block (0x50 bytes): one shape */
struct s_single_element
{
	byte unknown00[0x20];
	c_part_c c;
};

typedef char assert_pair_element_size[(sizeof(s_pair_element) == 0x80) ? 1 : -1];
typedef char assert_single_element_size[(sizeof(s_single_element) == 0x50) ? 1 : -1];

#endif
