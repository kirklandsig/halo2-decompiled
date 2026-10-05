/* GLOBALS.H: globals shared by more than one source file (defined in
src/globals.cpp) */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "unknown_0259d0.h"
#include "data_array.h"

/* the game time globals (0x24 bytes); field_2_3 is read by
   firing position code; scale is 1.0 once initialized */
struct s_game_time_globals
{
	bool active;
	byte unknown01;
	short field_2_3;
	real rate;
	long game_time;
	real scale;
	real leftover_ticks;
	real speed_timer;
	real speed_duration;
	real speed_initial;
	real speed_final;
};

extern s_game_time_globals *g_510c54;

/* g_4e0300: the object header data, a data array (data_array.h) whose
   elements (12 bytes each: 8 unknown bytes, then the object pointer) each
   source file views through its own s_object_header */
extern s_record_pool *g_4e0300;

/* g_4e6948: the game options. 016a90 reads the state at +8, 03d380 and
   072c70 the mode at +0xc, 146240 the ticks per second at +0xe, the session
   states (058dd0) the ids and positions at +0x10..+0x20 and the flag at
   +0x1120 (also read by 096e90); the script functions (unknown_29f5b0)
   read the difficulty at +0x132 and the flags at +0x130 and +0x134, and
   set the value at +0x11fa and the object index at +0x11fc */
struct s_game_options_view
{
	byte unknown00;
	byte flag;
	short index;
	byte unknown04[4];
	long state;
	char mode;
	byte unknown0d;
	short field_2_3;
	long id_a;
	long id_b;
	byte unknown18[4];
	long position_a;
	long position_b;
	char name[0x100];
	byte unknown124[0x130 - 0x124];
	bool flag130;
	byte unknown131;
	short difficulty;
	bool flag134;
	byte unknown135[0x180 - 0x135];
	long mode_180;
	struct
	{
		dword bit0 : 1;
		dword bit1 : 1;
		dword bit2 : 1;
		dword unknown : 9;
		dword bit12 : 1;
		dword bit13 : 1;
	} flags184;
	byte unknown188[4];
	long score_to_win;
	byte unknown190[0x1b4 - 0x190];
	long value1b4;
	long value1b8;
	byte unknown1bc[0x22c - 0x1bc];
	union
	{
		byte flags22c;
		struct
		{
			dword bit0 : 1;
			dword bit1 : 1;
			dword bit2 : 1;
			dword bit3 : 1;
			dword bit4 : 1;
			dword bit5 : 1;
			dword bit6 : 1;
			dword unknown : 25;
		} flags22c_bits;
		struct
		{
			word w22c;
			short s22e;
		};
	};
	union
	{
		long divisor;
		struct { short s230, s232; };
	};
	short s234;
	short s236;
	byte unknown238[0x240 - 0x238];
	long team_mode;
	short scale_a;
	short scale_b;
	byte unknown248[0x1120 - 0x248];
	byte flag1120;
	bool flag1121;
	byte unknown1122[2];
	long ticks1124;
	bool flag1128;
	bool flag1129;
	byte unknown112a[2];
	long ticks112c;
	long value1130;
	byte unknown1134[0x11f8 - 0x1134];
	bool flag11f8;
	byte unknown11f9;
	short value11fa;
	union
	{
		long value11fc;
		short cluster11fc;
	};
};

extern s_game_options_view *g_4e6948;

/* the hud globals definition (g_510c94): message timing and colors, the
   motion sensor's range at +0x290 and the string list at +0x3fc */
struct s_hud_globals_definition
{
	byte unknown00[0x58];
	real fade_time;
	real display_time;
	byte unknown60[0x80 - 0x60];
	real line_spacing;
	byte unknown84[0xa4 - 0x84];
	dword color_a4;
	dword color_a8;
	byte unknownac[0xb6 - 0xac];
	byte flags_b6;
	byte unknownb7[0xcc - 0xb7];
	dword color_cc;
	byte unknownd0[0xe8 - 0xd0];
	short value_e8;
	short value_ea;
	byte unknownec[0x290 - 0xec];
	real motion_sensor_range;
	real motion_sensor_minimum_speed;
	byte unknown298[0x3fc - 0x298];
	long string_list;
};

extern s_hud_globals_definition *g_510c94;

/* the multiplayer globals (g_4e9ae8): the engine index at +0xc14 selects the
   engine object in g_55e4d0; value24 is read by 0a45d0; the 16 slot
   identifiers at +0x2c are read by the entity definitions of 09a5e0 */
struct s_name18
{
	word c[9];
};

struct s_player_info
{
	byte b0;
	byte unknown01[3];
	point3f v;
	word w10;
	word w12;
	byte b14;
	byte unknown15[3];
};

struct s_stats
{
	long l[9];
};

/* the markers of types 7 and 8 found for one key (19cb20) */
struct s_marker_pair
{
	long first;
	long second;
	long unknown08;
	long unknown0c;
};

struct s_mp_globals
{
	byte unknown00[6];
	word w6;
	word w8;
	word wa;
	word wc;
	word we;
	s_name18 name;
	byte unknown22[2];
	dword value24;
	long value28;              /* the statborg's identifier (game_engine_entity_definitions.cpp) */
	long slots[16];
	short w6c;
	word w6e;
	byte unknown70[0xe0 - 0x70];
	word we0;
	byte unknowne2[0xfc - 0xe2];
	s_stats stats;
	byte unknown120[0x558 - 0x120];
	s_player_info players[1];
	byte unknown570[0x6dc - 0x570];
	long l6dc[4];
	byte unknown6ec[0xafc - 0x6ec];
	s_marker_pair marker_pairs[16];
	long marker_pair_count;
	byte unknownc00[4];
	long lc04;
	bool bc08;
	byte unknownc09[0xc14 - 0xc09];
	long engine_index;
	byte unknownc18[0x84];
};

extern s_mp_globals *g_4e9ae8;

/* the engine objects, indexed by g_4e9ae8->engine_index (engine_peer.h) */
class c_engine_peer;
extern c_engine_peer *g_55e4d0[256];

/* g_4e8c20: a table of indices (entries at +0xc) */
struct s_index_table
{
	byte unknown00[0xc];
	long entries[4];
};

extern s_index_table *g_4e8c20;

/* g_4701ec: a sequence the scripts step through (unknown_29f5b0 starts
   its stages and reads it; 03d380 and 225f80 reset it): the current stage,
   the time it started at +8, and two fields cleared on each new stage.
   unknown10 (0x4701fc, NONE in retail's data) is an index whose address
   0x226190 passes to several queries. That address escaping makes retail
   treat the whole struct as aliased: the stage setters of
   unknown_29f5b0 (0x2a9c30, 0x2a9c90, 0x2a9cd0, 0x2ac3a0) should match
   once 0x226190 is decompiled (all four matched in a test build that took
   the address). */
struct s_sequence_globals
{
	long stage;
	long unknown4;
	long start_time;
	short unknownc;
	long unknown10;
};

extern s_sequence_globals g_4701ec;

/* globals shared between the game state lifecycle callbacks (batch 24-1) and
   the game state code (03d380) */
struct s_simulation_world;
struct s_47f048_object;
extern byte g_4cf770;
extern s_simulation_world *g_4cf77c;
extern s_47f048_object *g_4cf780;
extern byte *g_4e8c34;
extern long *g_510c70;
extern s_record_pool *g_4ed28c;
extern s_record_pool *g_4ea950;
extern s_record_pool *g_4ee4e4;
extern s_record_pool *g_4ee4e8;

/* g_4ed288: the object looping sounds state (0x244 bytes, allocated by
   1887d0; defined in unknown_03d380.cpp) */
/* a sound and one of its permutations, as two indices */
struct s_sound_permutation_reference
{
	char pitch_range_index;
	char permutation_index;
};

/* a slot (0x10 bytes); the impulse sounds of 0x18bf90 use the same slots:
   their tag index, start time and object */
struct s_looping_sound_slot
{
	long datum_index;
	long end_time;
	long source_index;
	s_sound_permutation_reference permutation;
	bool active;
	byte unknown0f;
};

struct s_looping_sound_globals
{
	long indices[8];
	long value20;
	long value24;
	word scales[0x80];
	s_looping_sound_slot slots[16];
	real gains[4];
	long value238;
	real value23c;
	bool flag240;
	bool flag241;
	byte unknown242[2];
};

extern s_looping_sound_globals *g_4ed288;

/* g_4e9188: the Bink state (unknown_01e930.cpp); the memory callbacks are
   registered by video_playback_setup (155ea0) */
struct s_bink_globals
{
	byte initialized;
	byte flag1;
	byte unknown02;
	byte unknown03;
	byte finished;
	byte unknown05[3];
	dword flags;
	void *movie;
	short width;
	short height;
	dword copy_flags;
	struct D3DTexture *texture;
	byte texture_header[0x14];	/* the D3DTexture header 0x23e340 fills */
	byte unknown30[0x3c - 0x30];
	byte material[0xd4 - 0x3c];
	byte *permanent_memory;
	long permanent_memory_used;
	long permanent_memory_size;
};

extern s_bink_globals g_4e9188;

/* g_509448 (decals) and g_557c6c: tables of callbacks at the start, then the
   allocator that built them */
struct s_game_proc_table_509448
{
	byte unknown00[0x20];
	void (*proc20)(void);
	void (*proc24)(void);
	byte unknown28[0x6c - 0x28];
	c_data_allocator *allocator;
};

struct s_game_proc_table_557c6c
{
	byte unknown00[0x2c];
	long (__stdcall *proc2c)(long key);
	bool (__stdcall *proc30)(long a, long b);
	c_data_allocator *allocator;
};

extern s_game_proc_table_509448 *g_509448;
extern s_game_proc_table_557c6c *g_557c6c;

/* g_4e3b44: the tag instances (16 bytes each: the data pointer is at +8);
   batches 2-5 (animation tag data) and an earlier bitmap batch view the same
   data pointer with different types */
struct s_type_b8a6a0;
struct s_animation_tag_data;
struct s_sound_tag_data;
struct s_palette_tag_data;
struct s_tag_flags;
struct s_tag_instance
{
	byte unknown00[8];
	union
	{
		s_tag_flags *flags;
		s_animation_tag_data *data;
		s_type_b8a6a0 *group;
		s_palette_tag_data *palette;
		s_sound_tag_data *sound;
		byte *bytes;
	};
	byte unknown0c[4];
};

extern s_tag_instance *g_4e3b44;

/* a default point and vector, shared by the camera and animation code
   (a vector in 2-7, a point in 2-3: three reals either way) */
extern point3f *g_468788;

/* g_4e034c: the globals holding a tag header at +0xc0 (only used when the
   pointer at +0xc0 is set; the one at +0xc4 is read in that case), used by
   the animation (2-1) and sound (2-2) code */
struct s_tag_header
{
	byte unknown00[4];
	dword datum_index;
};

struct s_table_a;
struct s_table_b;

struct s_damage_table;

struct s_tag_header_globals
{
	byte unknown00[0xc0];
	s_tag_header *header;
	s_tag_header *header_alt;
	byte unknownc8[0xd4 - 0xc8];
	s_damage_table *damage_table;
	byte unknownd8[0x16c - 0xd8];
	long index;
	void *a_valid;
	s_table_a *a;
	void *b_valid;
	s_table_b *b;
};

extern s_tag_header_globals *g_4e034c;

/* g_4687a4: a default vector, read by the camera (2-10) and animation code */
extern vector3f *g_4687a4;

/* the rasterizer state flags, shared by 030290 and 0494b0 */
extern dword g_4ba014;

/* the animation sampling state, shared by the channel decoders of 28c510
   and 28cdb0 (each defines its own view of the animation data and output) */
struct s_animation_data;
struct s_animation_output;
extern long g_5044b4;
extern long g_5044b8;
extern long g_5044bc;
extern s_animation_output *g_5044c0;

/* the object type definitions, indexed by object type (108a90, 108fd0) */
struct s_object_type_definition;
extern s_object_type_definition *g_468630[16];

/* the game state allocator: game_state_globals (unknown_123b30.h) is at 0x4e6080 */

/* the default axis (a vector) and the pi constant of the vector math
   (11cc90, 11d180) */
extern vector3f *g_4687a8;
extern vector3f *g_4687ac;
extern vector3f *g_4687b0;
extern c_data_allocator *g_46875c;
extern real g_5476c4;

/* data arrays (data_array.h): g_4cf78c (0b49a0), g_4f55f0 (clumps, 26b230;
   slot owners, 1a8080), g_502420 and g_50241c (clumps, 26b230), g_502418
   (26bda0), g_4e8c24 (the players; 0699a0, 072c70, 096e90) and the arrays of
   03d380 */
extern s_record_pool *g_4cf78c;
extern s_record_pool *g_502418;
extern s_record_pool *g_50241c;

/* g_4e8c24: the players (elements of 0x21c bytes) */
extern s_record_pool *g_4e8c24;

/* g_4d87f8: the allocator interface (slots 0, 1, 5, 10 and 13 are used; get_info
   returns whether the block was found) and the count of live blocks, shared by
   081f80, 08ad30, 08b110, 096e90 and 097d80 */
class c_allocator
{
public:
	virtual void release(void *block, long size) {}
	virtual bool get_info(void *block, void *info) { return false; }
	virtual void slot2() {}
	virtual void slot3() {}
	virtual void slot4() {}
	virtual void *allocate(long size, long a, long b) { return 0; }
	virtual void slot6() {}
	virtual void slot7() {}
	virtual void slot8() {}
	virtual void slot9() {}
	virtual void compact(long size) {}
	virtual void slot11() {}
	virtual void slot12() {}
	virtual void dispose(long flags) {}
};

struct s_allocator_globals
{
	c_allocator *allocator;
	long count;
	byte unknown08;

	~s_allocator_globals()
	{
		if (allocator)
		{
			allocator->dispose(1);
			allocator = 0;
		}
	}
};

extern s_allocator_globals *g_4d87f8;

/* g_4eca60: the identifiers in the slots of the multiplayer globals' slot
   table (09a5e0, 183c60) */
extern long g_4eca60[8];

/* g_480118: another allocator (1efac0) */
extern c_allocator *g_480118;

/* a real zero, read by 16ae0 and 1efac0 */
extern real g_45dbd8;

/* g_4f55f0: a datum array (clumps, 26b230; slot owners, 1a8080) */
extern s_record_pool *g_4f55f0;

/* g_502420: a datum array (clump objects; 26b230, 26bda0) */
extern s_record_pool *g_502420;

/* g_4e0350: the globals with the entry table at +0x10c (1eb8a0, 19c1d0) and
   the palette sources at +0x214 (0158f0) */
struct s_unknown_entry;
struct s_palette_source;

/* the entries of the marker table at +0x11c (2420a0), 32 bytes each */
struct s_marker_entry
{
	point3f position;
	byte unknown0c[4];
	short key_a;
	short key_b;
	short key_c;
	word flags;
	byte unknown18[8];
};

struct s_palette_source_globals
{
	byte unknown00[0x12];
	byte flags;
	byte unknown13[0x10c - 0x13];
	s_unknown_entry *entries;
	byte unknown110[0x118 - 0x110];
	long marker_count;
	s_marker_entry *marker_entries;
	byte unknown120[0x214 - 0x120];
	s_palette_source *sources;
};

extern s_palette_source_globals *g_4e0350;

/* g_4e0348: the match globals. The entries at +0x10 are read by 1ee340; the
   count and match entries at +0x22c are read by 0bfd20. */
struct s_tag_block_entry
{
	byte unknown00[8];
	short value08;
	byte unknown0a[6];
	short value10;
	byte unknown12[2];
};

struct s_match_entry
{
	dword key;
	short subkey;
	byte byte_06;
	byte byte_07;
	byte unknown08[0x54];
};

struct s_match_globals
{
	byte unknown00[0x10];
	s_tag_block_entry *entries;
	byte unknown14[0x9c - 0x14];
	long list_count;
	byte unknowna0[0x22c - 0xa0];
	long count;
	s_match_entry *match_entries;
};

extern s_match_globals *g_4e0348;

/* g_485ad4: the depth range (0350e0, 023540); initialised from 0x467014 by 0167a0 */
struct s_range
{
	real lo;
	real hi;
};

extern s_range g_485ad4;

/* g_4e9bd4: the local players (170d70, 03d380) */
struct s_player_state
{
	point3f position;
	byte unknown0c[0x50 - 0xc];
	real radius;
	byte unknown54[0xa0 - 0x54];
};

struct s_player_4e9bd4
{
	byte unknown00[0xb8];
	s_player_state state;
	byte unknown158[0x358 - 0x158];
};

extern s_player_4e9bd4 g_4e9bd4[4];

/* input-device state, shared by 196d20 (the whole array) and 1969d0.
   g_511000 holds 4 entries of 0xa4 bytes. 1969d0 reads the array from
   0x511034 (g_511000 + 0x34) with its own layout (s_input_device_view,
   which runs past the entry into the next one), through input_device(). */
struct s_input_vector
{
	dword v[4];
};

struct s_input_entry_state
{
	byte unknown00[4];
	byte active;
	byte unknown05[0x4f];
	s_input_vector vector;
	byte unknown64[0x2c];
	char value_90;
	byte unknown91[0xb];
	short value_9c;
	byte unknown9e[6];
};

struct s_input_counter
{
	word value : 15;
	word flag : 1;
};

struct s_input_device_view
{
	byte active;
	byte unknown01;
	byte unknown02[0xe];
	byte data[0x7c];
	char slot;
	byte unknown8d[0x13];
	char button;
	byte unknownA1;
	short axis;
};

extern byte g_510ca0;
extern byte g_510cb0;
extern byte g_510cb1;
extern dword g_510e2c;
extern dword g_510e30;
/* g_511bf4: the input counters, 0x4040 bytes. The record of one tick
   (unknown_1967d0.cpp) copies it whole. g_511c4e and g_511c90 are the
   counters at +0x5a and +0x9c of its first group; g_515294 is the pair block
   and g_515694 the per-controller block. */
struct s_input_counters
{
	union
	{
		s_input_counter all[0x2020];
		struct
		{
			s_input_counter groups[16][0x1b5];
			s_input_counter pairs[16][16][2];
			s_input_counter counters[16][0x2d];
		};
	};
};

extern s_input_counters g_511bf4;
#define g_511c4e (&g_511bf4.groups[0][0x2d])
#define g_511c90 (&g_511bf4.groups[0][0x4e])
#define g_515294 (&g_511bf4.pairs[0][0][0])
extern s_input_entry_state g_511000[4];

inline s_input_device_view *input_device(long index)
{
	return (s_input_device_view *)((byte *)g_511000 + 0x34) + index;
}

/* g_4cef68: the creation weights of the object types, 0x4c bytes per entry (turret and vehicle entity definitions) */
struct s_creation_weight
{
	real weight;
	real maximum_distance;
	byte unknown08[8];
	byte update[0x3c];       /* the update weights (src/unknown_0aa4d0.cpp) */
};

extern s_creation_weight g_4cef68[1];

/* g_51ebd4: the sound permutation tables (219110: the sets, chances, entries
   and bit data) and the entries at +0x4c (03d380) */
struct s_permutation_set
{
	byte unknown00[8];
	short first_index;
	short count;
};

struct s_permutation_chance
{
	byte unknown00[2];
	word chance;
	byte unknown04[12];
};

struct s_sound_globals
{
	byte unknown00[0x24];
	s_permutation_set *sets;
	byte unknown28[4];
	s_permutation_chance *chances;
	byte unknown30[4];
	byte *entries34;
	byte unknown38[4];
	byte *bits;
	byte unknown40[0x4c - 0x40];
	byte *entries;
};

extern s_sound_globals *g_51ebd4;

/* the current palette source index (0158f0, 03d380) */
extern short g_4686c4;

/* the physical memory map (0x4e6420, unknown_12b400.cpp): a stack of
   stages, each with the lowest allowed address and the current top of its
   heap; allocations take memory from the top (0b3d30, and the
   PHYSICAL_MEMORY_ALLOCATE macro of unknown_053310.h) */
struct s_physical_memory_globals
{
	long field_0;
	long base_address;
	long field_8_3;
	long field_c_6[5];
	long field_20[5];
};

extern s_physical_memory_globals g_global_f9ae07;

/* the time source: when g_510548 is set, g_51054c is the current time
   (otherwise GetTickCount is used); read by 058dd0 and 08b110 */
extern byte g_510548;
extern long g_51054c;

/* the allocator the data arrays of 106 and 116 are built through, the cloth data array (1169f0) and the prop state data array (25d690) */
extern c_data_allocator *g_510c2c;
extern s_record_pool *g_4e0338;
extern s_record_pool *g_4e0320;
extern s_record_pool *g_4cf8d8;
extern bool g_4cf8d4;
extern s_record_pool *g_502414;

/* the 16 player slots of 0xc70 bytes (058cb0, 1900a5, and the vibration
   setting in unknown_1248b0): the head holds the flags and the settings, and the
   range from +0x470 is a second base that 1900a5 reads the flags byte of */
struct s_player_slot_head
{
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword flag6 : 1;
	dword flag7 : 1;
	byte unknown004[0x114 - 0x4];
	struct
	{
		dword unknown0 : 1;
		dword vibration_disabled : 1;
	} settings;
	byte unknown118[0x470 - 0x118];
};

struct s_player_slot_flags
{
	byte unknown00[8];
	byte flags;
};

struct s_player_slot : s_player_slot_head, s_player_slot_flags
{
	byte unknown479[0xc70 - 0x479];
};

extern s_player_slot g_54e8e0[16];

/* g_4e7408: the random seeds (146240). The game, the scripts
   (unknown_29f5b0) and the slot handlers draw from the first, the
   deterministic seed; the second is seeded from the time. */
struct s_random_globals
{
	dword unknown0;
	dword seed;
};

extern s_random_globals *g_4e7408;

/* g_4e61cc: per controller values of the game options (138180, 23d970) */
extern dword g_4e61cc[4];

/* g_4417f0: 1026 random unit vectors (146240) */
extern vector3f g_4417f0[1026];

/* g_4f93a4: a 0x40 byte bit set of the match nodes (210db0, 20fe20) */
struct s_unknown_210db0
{
	byte unknown00[0x40];
};

extern s_unknown_210db0 *g_4f93a4;

/* g_4e0328: the device groups (1061c0, unknown_29f5b0), 12 byte
   elements */
struct s_device_group_globals
{
	s_record_pool *groups;
	bool initialized;
};

extern s_device_group_globals g_4e0328;

/* the actor slot handlers' shared values (slot_handler.h): the results the
   evaluate callbacks return (g_46fbe4 -1, finished; g_46fbe8 -2, continue),
   the unset reference some notify callbacks reset slot fields to (a pair of
   shorts, unset while negative), what enables a slot type (g_46f348,
   g_4ee4ec and the bits of g_557c40), and the data array of 0xbc byte
   elements the handlers keep indices into */
struct s_reference
{
	short unknown0;
	short unknown2;
};

extern short g_46fbe4;
extern short g_46fbe8;
extern s_reference g_470fa0;
extern long g_46f348;
extern dword g_4ee4ec;
extern dword g_557c40[5];
extern long g_46f34c;
/* 0x440070: twelve zero bytes (the empty XNADDR/XNKID the network code compares against) */
extern byte g_440070[12];
extern short g_46fbec;
extern s_record_pool *g_502408;
extern s_record_pool *g_51e9d8;
extern s_record_pool *g_502424;

/* g_4f55d0: the ai globals (0x374 bytes in the game state; ai.cpp builds
   them, the script functions of unknown_29f5b0.cpp set the flags) */
struct s_ai_index_pair
{
	long unknown0;
	long unknown4;

	void clear()
	{
		unknown0 = NONE;
		unknown4 = NONE;
	}
};

struct s_ai_globals
{
	bool enabled;
	bool active;
	bool unknown02;
	byte unknown03[0x14 - 0x3];
	long unknown14;
	byte unknown18[0x20 - 0x18];
	bool unknown20;
	byte unknown21;
	short unknown22;
	s_ai_index_pair unknown24;
	s_ai_index_pair unknown2c;
	s_ai_index_pair unknown34;
	byte unknown3c[0x340 - 0x3c];
	bool unknown340;
	byte unknown341[0x364 - 0x341];
	long unknown364;
	byte unknown368[4];
	long unknown36c;
	long unknown370;
};

extern s_ai_globals *g_4f55d0;

/* what the ai tracks of each local player (2 entries of 0x1c bytes in the
   game state; ai.cpp; unknown_1b8c80.cpp reads the vehicle
   seat at +8) */
struct s_ai_player
{
	long player_index;
	long unit_index;
	short unknown08;
	short unknown0a;
	long unknown0c;
	byte unknown10[0x1c - 0x10];
};

#define MAXIMUM_AI_PLAYERS 2

extern s_ai_player *g_4f55cc;

/* data arrays function_1c7790 (ai.cpp) builds: the dynamic firing points
   (g_51eca4; unknown_26e370.cpp reads a joint index at +4 of each) and
   g_4f9398 (unknown_20fe20.cpp's nodes) */
extern s_record_pool *g_51eca4;
extern s_record_pool *g_4f9398;

/* g_468758: an allocator data arrays are built through (the QoS pool of
   unknown_07a9a0.cpp, the online tasks of online_tasks.cpp, the havok
   components of unknown_1cec30.cpp; the default source of the loop
   allocators, loop_allocator.cpp) */
extern c_data_allocator *g_468758;

/* g_47989c: Havok's fixed buffer in the ai's scratch buffers
   (unknown_146a20.h); the ai pauses it while it borrows them */
class c_havok_fixed_memory;
extern c_havok_fixed_memory *g_47989c;

/* the havok components (unknown_1cec30.cpp): a data array of 0x200
   elements of 0xa0 bytes (unknown_183c60.cpp reads them as its manager
   entries), the count of objects that have one (g_51e9a0, in the game
   state) and the flag that selects which limit applies to new ones
   (g_47f058, set by unknown_03d380.cpp's callbacks) */
extern s_record_pool *g_51e9b8;
extern long *g_51e9a0;
extern bool g_47f058;

/* g_47ff38: the language the game's text is in, NONE until first asked
   (unknown_059ad0.cpp, unknown_1932c0.cpp; unknown_11c9c0.cpp converts
   XGetLanguage's value) */
extern long g_47ff38;

/* shared with lane D's network and simulation code (unknown_067e10.cpp,
   online_tasks.cpp, unknown_054fe0.cpp) */
extern byte g_4cf771; /* defined in unknown_03d380.cpp */
extern byte g_4cf772; /* defined in unknown_03d380.cpp */
typedef void (__stdcall *game_module_proc)(dword);
extern game_module_proc g_46e320[30]; /* the game module table (unknown_03d380.cpp) */
extern byte g_4cf7cc[6]; /* the local machine's address (unknown_07a9a0.cpp) */
/* the transport globals (0x4d8b18, src/transport.cpp): whether the transport
   is initialized and started, the link state, and the transition functions
   the network modules register (startup, shutdown, reset, with a context) */
typedef void (__stdcall *transport_transition_function)(void *context);
struct s_transport_globals
{
	bool initialized;
	bool started;
	bool link_up;
	byte unknown03;
	long transition_function_count;
	transport_transition_function field_8_6[8];
	transport_transition_function shutdown_functions[8];
	transport_transition_function reset_functions[8];
	void *contexts[8];
};
extern s_transport_globals g_transport_globals;
struct s_597d0_object;

/* the game speed (153870 allocates it, 153950 runs it, the script functions
   of unknown_29f5b0 set it): g_510c5c is a time dilation (the first 0x2c
   bytes) and 4 per player camera shake slots, g_502120 the 4 matching tables
   (2229d0) and g_4e8c28 the point the effect looks at; g_510c60 chooses
   between the point and the view points */
struct s_speed_shake
{
	real scale;
	vector3f vector;
};

struct s_speed_request
{
	short type;
	short priority;
	real duration;
	short curve;
	byte unknown0a[2];
	real amount;
	s_speed_shake shake;
};

struct s_speed_bounds
{
	real duration;
	real unknown04[3];
	real lower;
	real upper;
};

struct s_speed_values
{
	real value[7];
};

struct s_speed_slot
{
	vector3f forward;
	vector3f vector;
	s_speed_request request;
	s_speed_bounds bounds;
	s_speed_values values50;
	real values6c[4];
	short timer7c;
	short timer7e;
	short timer80;
	short timer82;
	byte decay[4];
	union
	{
		byte flags;
		struct
		{
			byte flag0 : 1;
			byte flag1 : 1;
			byte flag2 : 1;
		};
	};
	byte decay89;
	byte unknown8a[0x98 - 0x8a];
	real priority98;
	real priority9c;
};

struct s_game_speed
{
	long start_time;
	short duration;
	bool reverse;
	byte unknown07;
	point3f point;
	real angles[3];
	long value20;
	short timer24;
	short timer26;
	union
	{
		dword flags;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword : 30;
		};
	};
	long time2c;
	s_speed_slot slots[4];
};

struct s_speed_table_item
{
	long a;
	long b;
	real scale;
};

struct s_speed_table_entry
{
	s_speed_table_item items[8];
	real timers[8];
	real value80;
	real value84;
};

struct s_speed_table
{
	s_speed_table_entry entries[4];
	real value220;
	real value224;
	real value228;
};

extern s_game_speed *g_510c5c;
extern s_speed_table *g_502120;
extern point3f g_4e8c28;
extern byte g_510c60;

/* g_4ed284: the view globals (185ab0): the flags, then 4 entries */
struct s_185ab0_flags
{
	dword bit0 : 1;
	dword bit1 : 1;
	dword bit2 : 1;
	dword bit3 : 1;
	dword bit4 : 1;
	dword bit5 : 1;
	dword bit6 : 1;
	dword bit7 : 1;
	dword bit8 : 1;
	dword bit9 : 1;
	dword bit10 : 1;
	dword bit11 : 1;
	dword bit12 : 1;
	dword bit13 : 1;
	dword bit14 : 1;
	dword bit15 : 1;
	dword bit16 : 1;
	dword bit17 : 1;
	dword bit18 : 1;
	dword bit19 : 1;
	dword bit20 : 1;
	dword bit21 : 1;
	dword bit22 : 1;
	dword bit23 : 1;
	dword bit24 : 1;
	dword : 7;
};

struct s_unknown_185ab0_entry
{
	byte unknown00[0x10];
	real yaw;
	real pitch;
	byte unknown18[0x2e - 0x18];
	short index;
	byte unknown30[0x89 - 0x30];
	bool flag;
	byte unknown8a[0x94 - 0x8a];
};

struct s_unknown_185ab0
{
	byte flag;
	byte unknown01[3];
	union
	{
		dword flags4;
		s_185ab0_flags bits4;
	};
	union
	{
		dword flags8;
		s_185ab0_flags bits8;
	};
	union
	{
		dword flagsc;
		s_185ab0_flags bitsc;
	};
	dword flags10;
	s_unknown_185ab0_entry entries[4];
};

extern s_unknown_185ab0 *g_4ed284;

/* the points of the first (g_468710) and second view (g_468718) */
extern point3f *g_468710;
extern point3f *g_468718;

/* the peer list's state (0x4ee4c4, unknown_19987f.cpp) */
struct s_peer_list_globals
{
	bool active;
	/* the session id and membership value the players' properties were last
	   refreshed for (unknown_19987f.cpp) */
	bool field_1_2;
	byte session_id[8];
	byte unknown0a[2];
	long membership_value;
	/* the presence state last published, its minutes and when it began
	   (unknown_19987f.cpp) */
	long presence_state;
	short presence_minutes;
	short unknown16;
	dword presence_start_time;
	byte unknown1c;
	bool session_booted; /* set when a session the states drop had been booted (unknown_058dd0.cpp) */
	byte unknown1e[0x20 - 0x1e];
};

extern s_peer_list_globals g_4ee4c4;

/* g_4686cc: the global colours, white first (defined in unknown_13b390.cpp;
   read by the tag function evaluators and the text drawing state) */
extern color4f const *g_4686cc;

/* g_4e73a0: the text drawing state (unknown_13e8a0.cpp): font, colours,
   shadow, justification and tab stops (0x64 bytes) */
struct s_draw_string_globals
{
	long font;
	dword flags; /* bit 0: wrap lines only where breaking is allowed */
	long style;
	long justification;
	color4f color;
	bool shadow;
	byte unknown21[3];
	color4f field_24;
	/* moves the vertices of each character, with its parameter (0x24cedf) */
	bool (__stdcall *vertex_proc)(real *vertices, long parameter);
	long vertex_proc_parameter;
	short tab_stop_count;
	short tab_stops[16];
	short unknown5e;
	short unknown60;
	short unknown62;
};

extern s_draw_string_globals g_4e73a0;

/* g_4e0344: the structure bsp globals (defined in scenery.cpp): the count at
   +0x80 and the array at +0x84, whose elements scenery.cpp (the locations)
   and unknown_16e290.cpp (the geometry blocks) each view their own way */
struct s_bsp_locations;
struct s_16e290_bsp;
struct s_structure_bsp_globals
{
	byte unknown00[0x80];
	long count;
	union
	{
		s_bsp_locations *locations;
		s_16e290_bsp *bsp;
	};
};

extern s_structure_bsp_globals *g_4e0344;

/* g_4e9af0: the local players' engine state (0xc8 bytes, defined in
   unknown_157450.cpp): per local user a count of ticks, a quarter second
   each (161b60), then the local players */
struct s_local_engine_player
{
	long index;
	byte unknown04[8];
};

struct s_local_engine_state
{
	byte timers[4];
	byte unknown04[0x18 - 4];
	s_local_engine_player players[4];
	byte unknown48[0xc8 - 0x48];
};

extern s_local_engine_state g_4e9af0;

/* g_527104: the routes of the voice packets (network_voice.cpp), with the
   voice settings of the players at 0x527108; settings.unknownc8 and
   settings.unknown108 are per player the masks of the players muted and
   heard (read by unknown_1600f0.cpp) */
struct s_voice_player_settings
{
	bool initialized;
	byte unknown01[3];
	long unknown04[16];
	long unknown44[16];
	long unknown84[16];
	long unknownc4;
	long unknownc8[16];
	long unknown108[16];
	long unknown148[16];
	long unknown188;
	long unknown18c;
	bool unknown190;
	byte unknown191[3];
	long unknown194[16];
	long unknown1d4[16];
};

struct s_voice_routing
{
	bool enabled;
	byte unknown01[3];
	s_voice_player_settings settings;
};

extern s_voice_routing g_527104;

#endif
