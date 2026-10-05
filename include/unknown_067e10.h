/* UNKNOWN_067E10.H: the simulation world (g_4cf77c, 0x1220 bytes at
   0x544b20): its views, the 16 players and 16 actors it tracks, and the
   queue of 0x404c-byte blocks it buffers (lane D) */

#ifndef UNKNOWN_067E10_H
#define UNKNOWN_067E10_H

#include "unknown_11c920.h"
#include "input_record.h"

/* a machine's 6-byte address, compared with memcmp */
struct s_machine_address
{
	byte bytes[6];
};

/* a player's 12-byte key */
typedef dword t_player_key[3];

class c_class_6a600;
class c_simulation_view;
struct s_network_observer;

/* the per-view baseline of the replicated state (0x4cd0 bytes, at +0x6060 of
   the view's distribution data) */
struct s_simulation_view_baseline
{
	c_simulation_view *view;
	bool unknown04;
	bool active;
	bool unknown06;
	byte unknown07;
	long sequence;
	s_input_record state;
	long time;
};

/* the replication data a view owns (src/unknown_085540.cpp; only the fields
   the view code touches) */
struct s_simulation_view_data
{
	byte unknown00[0x30];
	byte handles[8];	/* a c_handle_table_450cd0 (unknown_096ed0.h) */
	byte unknown38;
	bool unknown39;
	bool unknown3a;
	byte unknown3b[0x5079 - 0x3b];
	bool unknown5079;
	byte unknown507a[0x6060 - 0x507a];
	s_simulation_view_baseline baseline;
};

/* a view of the world onto one remote machine; type 1/3 views face a remote
   authority, type 2/4 views a remote client */
class c_simulation_view
{
public:
	word unknown00;
	short type;
	long unknown04;
	s_simulation_view_data *data;
	c_class_6a600 *world;
	long world_index;
	s_machine_address address;
	byte unknown1a[2];
	long unknown1c;
	s_network_observer *observer;
	long channel_index;
	long failure_reason;
	long state;
	long state_id;
	long remote_state;
	long remote_id;
	long unknown3c;
	long unknown40;
	byte unknown44[0x75 - 0x44];
	byte flag75;
	byte unknown76[2];
	bool flag78;
	byte unknown79[3];
	dword player_mask;
	long unknown80;
	long unknown84;
	bool unknown88;
	byte unknown89[3];
	long time8c;
	long unknown90;
	byte *buffer;
	long unknown98;
	long unknown9c;
	long unknowna0;
	byte unknowna4[0xac - 0xa4];
	long unknownac;
	long unknownb0;

	void initialize(long unknown04, short type, s_simulation_view_data *data, const s_machine_address *address, long unknown1c);
	bool channel_ready(void);
	void set_state(long state, long id);
	void fail(long reason);
	void update_established(void);
	void release_buffer(void);
	void detach(void);
	void set_unknown88(bool value);
	bool has_pending_entity(void);
	bool function_85cb0(void);
	void update_baseline(void);
	bool update_player_mask(dword player_mask, dword valid_mask, const t_player_key *keys);
	void send_player_update(dword controller_mask, const struct s_simulation_player_state *states);
	bool handle_player_update(bool failed, long a, long b, dword controller_mask, const struct s_simulation_player_state *states);
	bool handle_establishment(long new_state, long new_id);
	bool join_data_begin(long field_0_4);
	bool join_data_receive(long offset, const void *data, long size);
	bool baseline_update(long id, long sequence, const struct s_input_update *update);

	bool established(void) const
	{
		return unknown3c != NONE && flag75;
	}
};

/* a player's simulation state (0x5c bytes) */
struct s_simulation_player_state
{
	dword data[0x17];
};

/* a player the world tracks (0x88 bytes) */
struct s_simulation_world_player
{
	long player_index;
	long unknown04;
	long unknown08;
	dword key[3];
	byte unknown18[0x20 - 0x18];
	long unknown20;
	bool flag24;
	bool flag25;
	byte unknown26[2];
	long state_time;
	s_simulation_player_state state;
};

/* an actor the world tracks (0x90 bytes) */
struct s_simulation_world_actor
{
	long actor_index;
	long unknown04;
	long unknown08;
	byte unknown0c[4];
	long time;
	dword state[0x1f];
};

/* one buffered block (0x404c bytes: its contents, then the link) */
struct s_simulation_block_data
{
	long size;
	byte bytes[0x4048 - 4];
};

struct s_simulation_block
{
	s_simulation_block_data data;
	s_simulation_block *next;
};

/* one of the 16 player records of the world's owner (0xb4 bytes) */
struct s_simulation_owner_player
{
	dword key[3];
	bool flag0c;			/* left the game */
	byte unknown0d[3];
	long time;			/* the game time it left */
	s_machine_address machine;
	byte unknown1a[2];
	long controller_index;
	long unknown20;
	dword configuration[0x24];
};

/* the 16 players a watcher knows of (unknown_084a90.cpp) */
struct s_type_c67652
{
	dword player_mask;
	s_simulation_owner_player players[16];
};

/* a change to a player collection (0xc8 bytes): a player left (type 0), two
   players swapped slots (1), a player was removed (2) or updated (3) */
struct s_simulation_player_update
{
	long player_index;
	dword key[3];
	long type;
	s_machine_address machine;
	byte unknown1a[2];
	long controller_index;
	long unknown20;
	bool field_2_2;
	byte unknown25[3];
	dword configuration[0x24];
	long other_player_index;
	dword other_key[3];
};

/* what the world belongs to (the simulation watcher, g_4cf780): its valid
   players; unknown1c is the mask of the machines in the game and unknown24
   their addresses */
class c_class_58d20;

struct s_simulation_world_owner
{
	byte unknown00[4];
	c_class_6a600 *world;
	byte unknown08[4];
	c_class_58d20 *session;
	long unknown10;
	long unknown14;
	long unknown18;
	long unknown1c;
	long unknown20;
	dword unknown24[0x18];
	bool unknown84;
	byte unknown85[3];
	s_type_c67652 players;
	long unknownbcc;
	dword unknownbd0[0x18];
	bool unknownc30;
};

void simulation_player_collection_apply_update(s_type_c67652 *collection, const s_simulation_player_update *update);

dword function_84be0(const s_type_c67652 *collection);
bool simulation_watcher_player_valid(long player_index, const s_simulation_world_owner *watcher, const t_player_key *key);
bool simulation_world_player_valid(long player_index, c_class_6a600 *world, const t_player_key *key);
dword function_696f0(c_class_6a600 *world);
bool simulation_watcher_get_players(s_simulation_world_owner *watcher, long *unknown1c, dword *player_mask, dword *in_game_mask, dword *state, t_player_key *keys, bool force);

struct s_key_450d14;
void function_6a7f0(c_class_6a600 *world, s_key_450d14 *key, dword controller_mask, const s_simulation_player_state *states);
void simulation_world_view_established(c_class_6a600 *world, c_simulation_view *view, bool established);
void simulation_world_view_synchronized(c_class_6a600 *world, c_simulation_view *view, bool synchronized);
void simulation_view_baseline_set_active(s_simulation_view_baseline *baseline, bool active);
void simulation_view_baseline_send(s_simulation_view_baseline *baseline);
void simulation_view_baseline_update(s_simulation_view_baseline *baseline);
void __stdcall simulation_view_buffer_disposed(byte *buffer, c_simulation_view *view);
void function_6b040(c_class_6a600 *world);

/* a replicated entity (0x20 bytes) and the type definition that handles it */
struct s_simulation_entity
{
	long handle;
	short type;
	byte unknown06[0x20 - 6];
};

class c_simulation_entity_definition
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual void v2() = 0;
	virtual void v3() = 0;
	virtual void v4() = 0;
	virtual void v5() = 0;
	virtual bool v6(s_simulation_entity *entity) = 0;
};

struct s_simulation_entity_definitions
{
	long count;
	c_simulation_entity_definition *definitions[1];
};

/* the entities the world replicates (0x400 of them) */
struct s_simulation_entity_database
{
	byte unknown00[0x10];
	s_simulation_entity_definitions *definitions;
	s_simulation_entity entities[0x400];
};

/* what the world distributes: the replicated handles (s_handle_peers,
   unknown_096ed0.h, 0x2048 bytes) and the entity database */
struct s_simulation_distribution
{
	byte peers[0x2048];
	byte unknown2048[0x2098 - 0x2048];
	s_simulation_entity_database field_2098;
};

class c_class_6a600
{
public:
	c_class_6a600();
	s_simulation_world_owner *owner;
	s_simulation_distribution *distribution;
	long state;
	byte unknown0c;
	s_machine_address local_address;
	byte unknown13[0x18 - 0x13];
	long unknown18;
	long unknown1c;
	long unknown20;
	bool flag24;
	bool flag25;
	byte unknown26[2];
	long unknown28;
	bool flag2c;
	byte unknown2d;
	byte flag2e;
	byte unknown2f;
	long unknown30;
	long time34;
	long unknown38;
	long view_count;
	c_simulation_view *views[15];
	s_simulation_world_player players[16];
	s_simulation_world_actor actors[16];
	byte flag11fc;
	byte unknown11fd[3];
	long buffer_size;
	byte *buffer;
	byte unknown1208[4];
	long unknown120c;
	long unknown1210;
	long block_count;
	s_simulation_block *first_block;
	s_simulation_block *last_block;

	void function_6a600(void);
	void function_6a6f0(void);
};

/* the world's record of a player, if it has one */
static __forceinline s_simulation_world_player *world_player_get(c_class_6a600 *world, long player_index)
{
	s_simulation_world_player *result = NULL;
	long index = player_index & 0xffff;
	if (index >= 0 && index < sizeof(world->players) / sizeof(world->players[0]))
	{
		s_simulation_world_player *player = &world->players[index];
		if (player->player_index != NONE)
			result = player;
	}
	return result;
}

/* a client world filling its join buffer with the authority's join data
   (buffer_size counts the bytes so far) */
inline bool world_receiving_join_data(c_class_6a600 *world)
{
	bool result = false;
	long state = world->state;
	if (state && (state == 3 || state == 5) && state != 4 && state != 5)
		result = world->buffer_size != NONE;
	return result;
}

bool world_buffer_allocate(c_class_6a600 *world);
bool world_buffer_append(c_class_6a600 *world, long size, const void *data, long offset);
bool world_buffer_complete(c_class_6a600 *world, long size);
void function_6ab10(c_class_6a600 *world);
void function_69350(c_class_6a600 *world, bool value);

#endif
