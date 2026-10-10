#include "unknown_11c920.h"
#include "globals.h"
#include "slot_owner.h"
#include "game_engine_events.h"

// @flags /O2 /Gr

/* an iterator over the players (g_4e8c24) that skips the players whose flag
   at +2 is set (its first parameter is passed as a pointer to longs, as the
   callers declare it) */
struct s_player_iterator
{
	byte *datum;
	s_record_pool *data;
	long datum_index;
	long index;
};

/* the players (0x21c bytes each) and their units' item slots */
struct s_player_record
{
	byte unknown00[0x2c];
	long unit_index;
};

struct s_item
{
	long tag_index;
};

struct s_unit_object
{
	byte unknown00[0x218];
	long items[4];
};

struct s_item_object_header
{
	short identifier;
	byte unknown02[6];
	void *object;
};

struct s_item_tag
{
	byte unknown00[0x290];
	short type;
};

struct s_object_header
{
	byte unknown00[8];
	byte *object;
};

/* record_pool_iterator_step, as these iterators inline it: once with its call to
   function_16bc00, then with that inlined too */
static inline byte *player_iterator_first(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	return result;
}

#include <xtl.h>
#include <math.h>
#include "effects.h"
#include "flexible_surface_calls.h"

extern point3f g_4b9da0;
extern byte *g_485a80;
void function_1ccb0(long format);
void function_1cd90();
void function_1cf50();

// @retail 0x19f240
bool function_19f240(long *iterator_)
{
	s_player_iterator *iterator = (s_player_iterator *)iterator_;
	s_record_pool_iterator *data_iterator = (s_record_pool_iterator *)&iterator->data;

	iterator->datum = player_iterator_first(data_iterator);
	while (iterator->datum && (iterator->datum[2] & 2))
		iterator->datum = data_iterator_next_inlined(data_iterator);

	return iterator->datum != 0;
}

// @retail 0x19f300
bool function_19f300(long *iterator_)
{
	s_player_iterator *iterator = (s_player_iterator *)iterator_;
	s_record_pool_iterator *data_iterator = (s_record_pool_iterator *)&iterator->data;

	iterator->datum = player_iterator_first(data_iterator);
	while (iterator->datum && ((s_player_record *)iterator->datum)->unit_index == NONE)
		iterator->datum = data_iterator_next_inlined(data_iterator);

	return iterator->datum != 0;
}

// @retail 0x19f3c0
long function_19f3c0(long player_index, long type)
{
	s_player_record *player = (s_player_record *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long result = NONE;

	if (player->unit_index != NONE)
	{
		s_object_header *headers = (s_object_header *)g_4e0300->data;
		s_unit_object *unit = (s_unit_object *)headers[player->unit_index & 0xffff].object;
		long i;

		for (i = 0; i < 4; i++)
		{
			long item_index = unit->items[i];

			if (item_index != NONE)
			{
				s_item *item = (s_item *)headers[item_index & 0xffff].object;
				s_item_tag *tag = (s_item_tag *)g_4e3b44[item->tag_index & 0xffff].bytes;

				if (tag->type == type)
				{
					result = item_index;
					break;
				}
			}
		}
	}

	return result;
}

bool function_15eaf0();

/* the players (0x21c bytes each), as the score events see them */
struct s_score_player
{
	byte unknown00[0xc0];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

/* the event initialize and set cause player functions of unknown_19de80.cpp,
   which retail inlines here */
static inline void score_event_initialize(s_event *event, long type, long subtype)
{
	event->type = type;
	event->subtype = subtype;
	event->a = NONE;
	event->cause_player_index = NONE;
	event->cause_team = NONE;
	event->effect_player_index = NONE;
	event->effect_team = NONE;
	event->f = 0;
	event->g = NONE;
}

static inline void score_event_set_cause_player(s_event *event, long player_index)
{
	event->cause_player_index = player_index;
	event->cause_team = ((s_score_player *)g_4e8c24->data)[player_index & 0xffff].team;
}

/* announces the points a player has left to win */
// @retail 0x19f470
void function_19f470(long player_index, long score)
{
	if (g_4e6948->score_to_win - score == 10)
	{
		s_event event;

		score_event_initialize(&event, 0, function_15eaf0() ? 0x30 : 0x2f);
		score_event_set_cause_player(&event, player_index);
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
	if (g_4e6948->score_to_win - score == 30)
	{
		s_event event;

		score_event_initialize(&event, 0, function_15eaf0() ? 10 : 9);
		score_event_set_cause_player(&event, player_index);
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
	if (g_4e6948->score_to_win - score == 60)
	{
		s_event event;

		score_event_initialize(&event, 0, function_15eaf0() ? 8 : 7);
		score_event_set_cause_player(&event, player_index);
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
	}
}
// @retail 0x1a6fe0
short function_1a6fe0(long owner_index, short type)
{
	short result = NONE;
	s_slot_owner_entry *owner = (s_slot_owner_entry *)(g_4f55f0->data + (owner_index & 0xffff) * sizeof(s_slot_owner_entry));
	short count = owner->current;
	short i;

	for (i = 0; i <= count; i++)
	{
		if (owner->slots[i].type == type)
		{
			result = i;
			break;
		}
	}

	return result;
}
