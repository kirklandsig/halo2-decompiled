#ifndef PAIR_NOTIFICATIONS_315E70_H
#define PAIR_NOTIFICATIONS_315E70_H
#include "holder_30dc80.h"

struct flag_315e70
{
	bool value;

	flag_315e70(bool value_)
	{
		value = value_;
	}

	flag_315e70(flag_315e70 const &other)
	{
		value = other.value;
	}
};

class entity_315e70;

class handler_315e70
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void pair_added(entity_315e70 *other, flag_315e70 flag) {}
	virtual void pair_removed(entity_315e70 *other) {}
};

struct head_315e70
{
	long field_0;
	long field_4;
	long field_8;
	long field_c;
};

struct collidable_315e70
{
	long field_0;
	long field_4;
	long type;
	long field_c;
};

class entity_315e70 : public head_315e70, public collidable_315e70
{
public:
	handler_315e70 *handler;
};

struct pair_315e70
{
	collidable_315e70 *a;
	collidable_315e70 *b;
};

class dispatcher_315e70 : public pair_handler_30dc80
{
public:
	virtual void slot_1(void const *pair);
	virtual void slot_2(void const *pair);
};

#endif
