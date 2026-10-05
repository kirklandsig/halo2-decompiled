// @flags /O2 /Gr
#include <string.h>
#include "unknown_11c920.h"
#include "unknown_0662e0.h"
#include "unknown_059ad0.h"
#include <xtl.h>

/* The two vtables at 0x450b44 (slots 0..8 and 9..16) belong to two small
   helper classes sharing one layout; only the slots decompiled so far have
   bodies, the others are placeholders. */

struct s_helper_game
{
	byte unknown00[0x18];
	long l18;
	byte unknown1c[0x1118 - 0x1c];
	long l1118;
	byte unknown111c[0x4994 - 0x111c];
	long l4994;
	byte unknown4998[0x741c - 0x4998];
	long mode;
};

struct s_helper_source
{
	long type;
	byte unknown04[0x2c];
	s_helper_game *game;
};

struct s_helper_output
{
	long l0;
	long l4;
	long l8;
	long lc;
	long l10;
	long l14;
	long l18;
	long l1c;
	long l20;
	long l24;
	long l28;
};


struct s_online_match_session_info
{
	XNKEY key;
	XNKID session_id;
	XNADDR address;
	byte unknown3c[0x68 - 0x3c];
};

class c_helper_a
{
public:
	virtual void v0(long a, long b);
	virtual void v1();
	virtual void v2() {}
	virtual void v3(s_helper_output *out);
	virtual long v4();
	virtual void v5() {}
	virtual long v6();
	virtual bool v7();
	virtual void v8(long a);

	s_helper_source *source;
	bool b8;
	bool b9;
	bool ba;
	byte unknownb;
	long lc;
	long l10;
	long l14;
	long l18;
	long l1c;
	long l20;
	byte unknown24[8];
	long l2c;
	bool b30;
	byte unknown31[3];
	s_online_match_session_info info;
};

class c_helper_b
{
public:
	virtual void v0(long a, long b) {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual long v4();
	virtual void v5() {}
	virtual long v6();
	virtual void v7() {}
};

// @retail 0x72db0
void c_helper_a::v0(long a, long b)
{
	source = (s_helper_source *)a;
	lc = NONE;
	l14 = NONE;
	l1c = NONE;
	ba = 0;
	l10 = 0;
	l18 = 0;
	l20 = 0;
	l2c = b;
	b30 = 0;
	b9 = 0;
	b8 = 1;
}

// @retail 0x73750
void c_helper_a::v8(long a)
{
	source = (s_helper_source *)a;
	ba = 0;
	lc = NONE;
	l10 = 0;
	l14 = NONE;
	l18 = 0;
	l1c = NONE;
	l20 = 0;
	l2c = 3;
	b30 = 0;
	b9 = 0;
	b8 = 1;
}

// @retail 0x73790
long c_helper_a::v4()
{
	return 60000;
}

// @retail 0x73820
bool c_helper_a::v7()
{
	bool result = false;

	if (b9)
	{
		if (source->type != 0)
		{
			s_helper_game *game = source->game;

			long mode = game->mode;

			if (mode == 5 || mode == 6 || mode == 7 || mode == 8)
			{
				if (game->l18 != 0)
					result = true;
			}
		}
	}
	return result;
}

// @retail 0x73860
void c_helper_a::v3(s_helper_output *out)
{
	s_helper_game *game = source->game;

	memset(out, 0, sizeof(s_helper_output));
	out->l0 = out->l4 = out->l8 = out->lc = 0;
	out->l10 = NONE;
	out->l14 = NONE;
	long bits = 16;
	if (game->mode > 2 && game->mode <= 8)
		bits = game->l4994;
	bits -= game->l1118;
	out->l24 = 0;
	out->l1c = NONE;
	out->l20 = NONE;
	out->l18 = bits;
	out->l28 = (1 << bits) - 1;
}

// @retail 0x738d0
long c_helper_a::v6()
{
	long result = g_network_configuration.valueca0;
	long type = source->type;

	if (type == 3 || type == 8)
		result = g_network_configuration.valueca4;
	return result;
}

// @retail 0x73350
long c_helper_b::v4()
{
	return g_network_configuration.valuec94;
}

// @retail 0x73470
long c_helper_b::v6()
{
	return g_network_configuration.valuec9c;
}

void function_6b640(long task_index);

// @retail 0x73310
void c_helper_a::v1()
{
	if (lc != NONE)
	{
		function_6b640(lc);
		lc = NONE;
	}
	if (l14 != NONE)
	{
		function_6b640(l14);
		l14 = NONE;
	}
	b8 = false;
}

bool online_match_session_get_info(long task_index, s_online_match_session_info *info);
bool transport_security_register_key(long index, long local, bool host, const XNKID *kid, const XNKEY *key);

// @retail 0x72f00
void function_72f00(c_helper_a *helper)
{
	if (helper->lc != NONE && online_match_session_get_info(helper->lc, &helper->info))
	{
		const s_long_pair *id = (const s_long_pair *)&helper->info.session_id;
		if ((id->a | id->b) != 0 && *(long *)&helper->info.key != 0)
		{
			helper->ba = true;
			helper->b30 = true;
			if (!transport_security_register_key(helper->l2c, 0, false, &helper->info.session_id, &helper->info.key))
				helper->ba = false;
		}
		if (helper->lc != NONE)
		{
			function_6b640(helper->lc);
			helper->lc = NONE;
		}
	}
}

// @retail 0x737b0
void function_737b0(c_helper_a *helper)
{
	if (helper->v7())
	{
		c_class_58d20 *game = (c_class_58d20 *)helper->source->game;
		const s_long_pair *previous = NULL;
		if (game->state > 2 && game->state <= 8 && game->flag4998)
			previous = &game->data4999;
		const s_long_pair *current = NULL;
		if (helper->ba)
			current = (const s_long_pair *)&helper->info.session_id;
		if (current != previous && (!current || !previous || memcmp(current, previous, sizeof(*current)) != 0))
			game->set_data_4999(current);
	}
}
