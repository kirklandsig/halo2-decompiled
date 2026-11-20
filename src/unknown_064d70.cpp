// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_064D70.CPP: the session browser's summary of the current game
   session (the manager's session_a). The other queries on that session are in
   src/unknown_054fe0.cpp; this one is built with SSE code, as retail's is. */

#include "unknown_11c920.h"
#include <string.h>
#include <wchar.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

/* src/unknown_054fe0.cpp */
long network_session_interface_get_value_49a4(void);

/* a 12-byte key compared from its last dword to its first (src/unknown_054fe0.cpp) */
struct s_sort_key
{
	long low;
	long middle;
	long high;
};
bool __stdcall sort_key_less_than(const s_sort_key *a, const s_sort_key *b, void *context);

/* the session's membership, from its value4c on (src/unknown_054fe0.cpp) */
struct s_66050
{
    long field_0;
    long field_4;
    long field_8;
    s_session_member field_c[16];
    long field_10cc;
    dword field_10d0;
    s_network_session_player field_10d4[16];
};

typedef bool (__stdcall *t_session_summary_compare)(const void *, const void *, const void *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size,
 t_session_summary_compare compare, const void *context);
long network_session_get_maximum_players(c_class_58d20 *session);
long player_index_from_absolute_index(long index);
long function_1587b0(long player_index);
long function_1587f0(long team);
void function_158850(long *type, long *count);

#pragma pack(push, 1)
struct s_session_browser_summary
{
 short version;
 short field_02;
 long protocol;
 long build;
 long compatible_build;
 short type;
 short local;
 short mode;
 short field_16;
 wchar_t name[32];
 s_session_id id;
 byte key[16];
 dword identity[9];
 short public_open;
 short private_open;
 short public_used;
 short private_used;
 short variant;
 short state;
 long game_type;
 long map;
 long option;
 long map_variant;
 bool teams;
 byte field_b1[3];
 long score_type;
 long score_limit;
 short player_count;
 byte player_keys[16][12];
 wchar_t player_names[16][32];
 short field_57e;
 long scores[16];
 short player_teams[16];
 byte field_5e0[16][16];
 dword team_mask;
 long team_scores[8];
 byte field_704[16];
};
#pragma pack(pop)

struct s_summary_player
{
 dword key[3];
 byte properties[0x90];
};

struct s_summary_player_sort_entry
{
 s_sort_key sort;
 s_summary_player player;
};

// Matches now that it is built with /arch:SSE in its own file (339 in src/unknown_054fe0.cpp, whose flags lack it):
// retail sets the summary's state with a conditional move (cmovne at 0x64fac), which this compiler emits only for
// the SSE architecture. Retail also holds the session twice (ECX up to the map query, copied to EBX inside the
// state test for the code after that call), so the body reads it into two locals: `current`, used up to the
// query (state, type pointer, member index, membership, variant), and `session`, read again for what follows.
// The membership's fields, the id and key through network_session_get_key and the switch on the mode in the
// full-session arm are retail's loads and tests in retail's order.
// @retail 0x64d70
bool __stdcall function_64d70(s_session_browser_summary *output)
{
 bool result = false;
 if (g_527330.initialized)
 {
  c_class_58d20 *current = (c_class_58d20 *)g_527330.session_a;
  long state = current->state;
  if (state)
  {
   c_class_58d20 *session = (c_class_58d20 *)g_527330.session_a;
   const long *type = &current->value18;
   if (*type)
   {
    if (state > 2)
    {
     if (state <= 8)
     {
   long map = NONE;
   long map_variant = NONE;
   byte *map_data;
   bool statistics = false;
   dword team_mask = 0;
   long member_index = current->member_index;
   s_66050 *membership = (s_66050 *)&current->value4c;
   byte *variant = 0;
   if (SESSION_STATE_IS_LIVE(current->state)) variant = current->data4db0;
   const wchar_t *name = membership->field_c[membership->field_4].properties.description;
   current->get_values_4d08(&map, &map_variant, &map_data);
   memset(output, 0, sizeof(*output));
   output->version = 2;
   output->field_02 = 0;
   output->protocol = 4;
   output->build = 0x2651;
   output->compatible_build = 0x2651;
   output->type = (short)*type;
   output->local = (short)session->value14;
   long mode = 0;
   if (SESSION_STATE_IS_LIVE(session->state)) mode = session->value498c;
   output->mode = (short)mode;
   if (*name)
   {
    wchar_t *summary_name = output->name;
    wcsncpy(summary_name, name, 31);
    summary_name[31] = 0;
   }
   network_session_get_key(session, &output->id, output->key, 0, 0);
   memcpy(output->identity, membership->field_c[member_index].words, sizeof(output->identity));
   long maximum = 16;
   if (SESSION_STATE_IS_LIVE(session->state)) maximum = session->value4990;
   if (membership->field_8 >= maximum)
   {
    long used = membership->field_10cc;
    switch (output->mode)
    {
    case 0: output->public_used = (short)used; break;
    case 1: case 2: output->private_used = (short)used; break;
    }
   }
   else
   {
    long used = membership->field_10cc;
    long available = network_session_get_maximum_players(session) - used;
    switch (output->mode)
    {
    case 0: output->public_used = (short)used; output->public_open = (short)available; break;
    case 1: output->private_used = (short)used; output->private_open = (short)available; break;
    case 2: output->private_used = (short)used; break;
    }
   }
   output->variant = network_session_interface_get_value_49a4() == 2 ? 2 : 1;
   short state = 0;
   if (g_527330.initialized) state = (short)g_527330.state;
   output->state = state;
   output->game_type = *(long *)(variant + 0x44);
   output->map = map;
   short option = NONE;
   if (SESSION_STATE_IS_LIVE(session->state)) option = session->value5dd0;
   output->option = option;
   output->map_variant = map_variant;
   output->teams = (variant[0x48] & 1) != 0;
   if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 2 && state == 3)
   {
    statistics = true;
    function_158850(&output->score_type, &output->score_limit);
   }
   s_summary_player_sort_entry entries[16];
   long count = 0;
   for (long i = 0; i < 16; i++)
   {
    if (membership->field_10d0 & (1 << i))
    {
     const s_network_session_player *source = &membership->field_10d4[i];
     s_summary_player *player = &entries[count].player;
     memcpy(player->key, source, sizeof(player->key));
     memcpy(player->properties, source->propertiesa8, sizeof(player->properties));
     entries[count].sort.low = i;
     entries[count].sort.middle = 0;
     entries[count].sort.high = 0;
     if (statistics && i != NONE && i >= 0 && i < g_4e8c24->high_water_index)
     {
      byte *datum = (byte *)g_4e8c24->data + i * g_4e8c24->size;
      if (*(word *)datum && datum[0xc0] != 0xff)
      {
       entries[count].sort.middle = function_1587b0(player_index_from_absolute_index(i));
       entries[count].sort.high = function_1587f0(*(signed char *)(datum + 0xc0));
       team_mask |= 1 << datum[0xc0];
      }
     }
     count++;
    }
   }
   function_13da70(entries, count, sizeof(entries[0]), (t_session_summary_compare)sort_key_less_than, 0);
   output->player_count = (short)count;
   for (long j = 0; j < count; j++)
   {
    memcpy(output->player_keys[j], entries[j].player.key, sizeof(entries[j].player.key));
    memcpy(output->player_names[j], entries[j].player.properties, sizeof(output->player_names[j]));
    output->scores[j] = entries[j].sort.middle;
    output->player_teams[j] = *(signed char *)(entries[j].player.properties + 0x7c);
    memcpy(output->field_5e0[j], entries[j].player.properties + 0x40, sizeof(output->field_5e0[j]));
   }
   if ((variant[0x48] & 1) && statistics)
   {
    output->team_mask = 0;
    for (long team = 0; team < 8; team++)
    {
     long score = function_1587f0(team);
     output->team_scores[team] = score;
     if (score || (team_mask & (1 << team))) output->team_mask |= 1 << team;
    }
   }
   result = true;

     }
    }
   }
  }
 }
 return result;
}
