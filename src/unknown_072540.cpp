// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_072540.CPP: the matchmaking state's check of the players' rank
   values (retail builds it with conditional moves, so apart from the session
   states in unknown_058dd0.cpp) */

#include "unknown_11c920.h"
#include "unknown_058dd0.h"

// @retail 0x72540
void function_72540(c_session_state_matchmaking *state)
{
	c_class_58d20 *session = state->owner->session_a;
	s_matchmaking_player_values *ratings = (s_matchmaking_player_values *)state;
	if (state->mode == 3)
	{
		if (ratings->player_count == session->player_count)
		{
			long missing = 0;
			for (long i = 0; i < 16; i++)
			{
				if (session->player_mask & (1 << i))
				{
					s_network_session_player *player = &session->players[i];
					long index = function_063190(ratings->summary, (long)&player->user_id);
					if (index != NONE)
					{
						long &rank = ratings->ranks[index];
						if (!(player->user_flags & 3))
						{
							if (rank == NONE || ratings->values[index] == NONE)
							{
								s_player_properties *properties = (s_player_properties *)player->propertiesa8;
								missing++;
								if (properties->unknown84 == ratings->selected && properties->unknown8c != NONE && properties->unknown8a != NONE)
								{
									rank = properties->unknown8a;
									ratings->values[index] = properties->unknown8c;
									missing--;
								}
							}
						}
						else
						{
							rank = NONE;
							ratings->values[index] = NONE;
						}
					}
					else
						state->mode = 6;
				}
			}
			ratings->ready = missing == 0;
		}
		else
			state->mode = 6;
	}
	if (state->mode == 3)
	{
		for (long i = 0; i < session->member_count; i++)
		{
			if (session->members[i].properties.unknown84[ratings->selected] != 1)
			{
				state->mode = 6;
				break;
			}
		}
	}
	if (state->mode == 3 && ratings->ready)
	{
		long maximum = 0;
		long total = 0;
		long count = 0;
		long minimum = 127;
		long *ranks = ratings->ranks;
		for (long i = 0; i < ratings->player_count; i++)
		{
			long rank = ranks[i];
			if (rank != NONE)
			{
				if (minimum > rank) minimum = rank;
				if (maximum <= rank) maximum = rank;
				total += rank;
				count++;
			}
		}
		if (count > 0)
		{
			ratings->minimum = minimum;
			ratings->maximum = maximum;
			ratings->average = total / count;
		}
		else
			state->mode = 1;
	}
}
