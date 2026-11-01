#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"
#include "unknown_1c3b70.h"

// @flags /O2 /Gr

struct c_shape_user_base : c_shape_counted_base
{
	dword unknown08;
	c_shape_user_base() : unknown08(0) {}
};

/* ---- c_d: a 12-byte object with a virtual destructor ---- */
struct c_d : c_shape_user_base
{

	virtual ~c_d();
	static void __cdecl operator delete(void *block)
	{
		// This concrete class owns a fixed twelve-byte tag-0x22 allocation.
		g_480118->allocate((long)block, 0xc, 0x22);
	}
	virtual void *v3() { return 0; }
	virtual long v5();
	virtual void v6() {}
	virtual real v7(long);
	virtual void v8(byte *, long, long);
};

// @retail 0x1efaf0 deleting
c_d::~c_d()
{
}

// @retail 0x1efad0
long c_d::v5()
{
	return 0x1b;
}

// @retail 0x1efac0
real c_d::v7(long)
{
	return g_45dbd8;
}

// @retail 0x1efae0
void c_d::v8(byte *a, long, long)
{
	*a = 0;
}

/* ---- object / tag lookups ---- */
struct s_tag_ref_data
{
	byte unknown00[0xc];
	long ref0c;
	byte unknown10[0x38 - 0x10];
	long ref38;
};

struct s_tag_instance_view
{
	byte unknown00[8];
	s_tag_ref_data *data;
	byte unknown0c[4];
};

struct s_object_view
{
	long tag_index;
	byte unknown04[0x116 - 4];
	short offset116;
	byte unknown118[2];
	short offset11a;
};

struct s_object_header_view
{
	byte unknown00[8];
	s_object_view *object;
};

// @retail 0x1efb40
bool s_lookup::initialize(long object_handle)
{
	s_object_header_view *headers = (s_object_header_view *)g_4e0300->data;
	s_tag_instance_view *tags = (s_tag_instance_view *)g_4e3b44;
	s_object_view *object = headers[object_handle & 0xffff].object;
	s_tag_ref_data *definition = tags[object->tag_index & 0xffff].data;
	bool result = false;

	if (definition->ref38 != NONE)
	{
		s_tag_ref_data *a = tags[definition->ref38 & 0xffff].data;

		if (a->ref0c != NONE)
		{
			s_tag_ref_data *b = tags[a->ref0c & 0xffff].data;

			void *pa = (byte *)object + *(short volatile const *)&object->offset11a;
			void *pb = (byte *)object + object->offset116;

			pointer_a = pa;
			handle = object_handle;
			tag_a = a;
			tag_b = b;
			pointer_b = pb;
			result = true;
		}
	}

	return result;
}

/* ---- a three-level table of items, walked by a packed position ---- */
struct s_item
{
	signed char first;
	byte unknown01[3];
	byte data[0x40];
};

struct s_sub
{
	long unknown00;
	long count;
	s_item *items;
	long unknown0c;
	long unknown10;
};

struct s_group
{
	long unknown00;
	long count;
	s_sub *subs;
};

struct s_table
{
	byte unknown00[0x1c];
	long count;
	s_group *groups;
};

struct s_table_holder
{
	byte unknown00[8];
	s_table *table;
};

// @retail 0x1efd80
void *function_1efd80(s_table_holder *holder, dword position)
{
	s_group *group = &holder->table->groups[position >> 24];
	s_sub *sub = &group->subs[(position >> 16) & 0xff];

	return sub->items[(position >> 8) & 0xff].data;
}

/* ---- the iterator ---- */
struct s_iterator
{
	s_table *table;
	signed char bytes[16];
	union
	{
		dword position;
		struct
		{
			byte first;
			byte item;
			byte sub;
			byte group;
		};
	};
	void *current;

	bool advance();
};

// @retail 0x1efca0
bool s_iterator::advance()
{
	long g = NONE;
	long sub_index;
	long item_index;
	s_sub *s = 0;
	bool next = true;

	if (position != NONE)
	{
  g = group;
  sub_index = sub;
		long current_item = item;
		s = &table->groups[g].subs[sub_index];

		if (s)
		{
			if (current_item + 1 < s->count)
			{
				item_index = current_item + 1;
				next = false;
			}
		}
	}

	if (next)
	{
		s_table *t = table;
		long count = t->count;

		for (;;)
		{
			g = g == NONE ? 0 : g + 1;

			if (g >= count)
			{
				g = NONE;
				break;
			}

			s_group *group_pointer = &t->groups[g];

			sub_index = bytes[g];
			if (sub_index >= 0 && sub_index < group_pointer->count)
			{
				s = &group_pointer->subs[sub_index];

				if (s->count > 0)
				{
					item_index = 0;
					break;
				}
			}
		}
	}

	if (g == NONE)
	{
		current = 0;
		position = NONE;
	}
	else
	{
		s_item *item_pointer = &s->items[item_index];

		position = ((((((sub_index & 0xff) | (g << 8)) << 8) | (item_index & 0xff)) << 8) | (item_pointer->first & 0xff));
		current = item_pointer->data;
	}

	return current != 0;
}

/* ---- the table of selected values ---- */
struct s_mix_item
{
	byte unknown00[5];
	byte value;
	byte unknown06[2];
};

struct s_mix_entry
{
	byte unknown00[4];
	byte id;
	byte unknown05[3];
	long count;
	s_mix_item *items;
};

struct s_mix_source
{
	byte unknown00[0x70];
	long count;
	s_mix_entry *entries;
};

struct s_mix_values
{
	signed char values[16];
};

struct s_mix_output
{
	long id;
	s_mix_values values;
	long position;
};

// @retail 0x1efbd0
void function_1efbd0(dword id, s_mix_output *output, s_mix_source *source, signed char *indices)
{
	output->id = id;
	output->position = NONE;

	if (!indices)
	{
		memset(&output->values, 0, sizeof(output->values));
	}
	else if (!source)
	{
		output->values = *(s_mix_values *)indices;
	}
	else
	{
		long count = 16;
		long i;

		if (source->count <= 16)
			count = source->count;

		memset(&output->values, 0xff, sizeof(output->values));

		for (i = 0; i < count; i++)
		{
			s_mix_entry *entry = &source->entries[i];
			long index = indices[i];

			if (entry->id != 0xff && index >= 0 && index < entry->count)
			{
				s_mix_item *item = &entry->items[index];
				

				if (item->value != 0xff)
					output->values.values[(signed char)entry->id] = item->value;
			}
		}
	}
}

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
extern byte *g_4ed280;
extern long *g_51e9cc;
long function_184000(long index, long bit);

struct s_surface_flags
{
	byte field_0[4];
	byte flags;
	byte bit;
	byte field_6[2];
};

struct s_surface_list
{
	byte field_0[0x28];
	long count;
	s_surface_flags *surfaces;
};

struct s_surface_owner
{
	byte field_0[0x9c];
	s_surface_flags *surfaces;
	byte field_a0[0xc8 - 0xa0];
};

struct s_surface_instance
{
	byte field_0[0x34];
	short owner;
	byte field_36[0x58 - 0x36];
};

struct s_surface_globals
{
	byte field_0[0x13c];
	s_surface_owner *owners;
	long count;
	s_surface_instance *instances;
};

struct s_surface_object_header
{
	short salt;
	byte flags;
	byte type;
	byte field_4[4];
	s_object_view *object;
};

// score 8 (was 8): the record address is computed in ECX; retail computes it in EDX and reads
// the flags from it (then the object in ESI, the masked index in ECX: ours object ECX, index ESI).
// Tried: a byte flags local 8; a dword index 8; the object as bytes 8;
// `case 4: case 3:` 8; the value read as one sum 8; the parameters as (key, owner) 8;
// the byte read as `object[offset + (index & 31)]` 8; the masked index named first 8; no object
// or values locals 8; the comparison reversed 9; the object read before the flags test 58.
// The compile-only build reproduces this block and keeps it for every spelling of the case.
// Round 19: the record address as byte arithmetic on the data 8; the record written in full at
// both uses 8; a named region index with the object left a common subexpression 8; the
// permutation read through a forceinline helper 8; the two index locals declared surface first 10.
// Round 98: one surface pointer shared by the first two cases 8; result assigned from the mapped
// index 52; an early return for the value mismatch 69; all three failure guards as early returns 69;
// a read-only object pointer 8; a read-only values pointer 8; a bit-field discriminator 8;
// bit-field views for all three key fields 8. All restored.
// Round 99: a table pointer local in the object-backed case 8; the table pointer at function scope 83;
// a const globals pointer in case 5 8; a const bit number in case 1 8; the header flag as a one-bit
// boolean 8; a byte-width slot 43; result declaration and initialization split 8; result declared
// after the key fields 102. All restored.
// Round 69 (lane 16, fast): the header address written index first (`index * size + data`) 8 (same
// code); the three guards positive with `else result = false;` arms 8 (same code); the choice as
// `(byte)(surface_index >> 5)` 62 (the key moves to EAX); named region and choice indices beside a
// `choices` pointer 8 (same code); the object read through the file's header view while the header
// local serves the flags 8 (same code). A permuter run (codex AC88) found nothing either.
/* the first argument is the caller's shape owner (or its object reference); the body
   never reads it, but every retail caller still computes it before the call */
// @retail 0x1ef3e0
bool function_1ef3e0(void const *owner, dword key)
{
	bool result = true;
	long object_index = key & 0xffff;
	long surface_index = (key >> 16) & 0x1fff;
	switch (key >> 29)
	{
	case 5:
		{
			s_surface_globals *globals = (s_surface_globals *)g_4e0348;
			s_surface_flags *surface = &globals->owners[globals->instances[object_index].owner].surfaces[surface_index];
			if ((surface->flags & 8) && !(byte)function_184000(object_index, surface->bit))
				result = false;
		}
		break;
	case 1:
		{
			s_surface_flags *surface = &((s_surface_list *)g_4e0340)->surfaces[object_index];
			if (surface->flags & 8)
			{
				long bit = surface->bit;
				if (bit != NONE && !(((dword *)(g_4ed280 + 1))[g_4686c4 * 8 + (bit >> 5)] & (1 << (bit & 31))))
					result = false;
			}
		}
		break;
	case 2:
		break;
	case 3:
	case 4:
		{
			long index = g_51e9cc[object_index];
			if (index == NONE)
				result = false;
			else
			{
				s_surface_object_header *header = &((s_surface_object_header *)g_4e0300->data)[index & 0xffff];
				if (header->flags & 0x10)
					result = false;
				else
				{
					s_object_view *object = header->object;
					signed char *values = (signed char *)object + object->offset11a;
					if (values[surface_index & 31] != ((surface_index >> 5) & 0xff))
						result = false;
				}
			}
		}
		break;
	default:
		__assume(0);
	}
	return result;
}

struct s_surface_key_array
{
	dword *keys;
	long count;
};

// @retail 0x1eece0
void function_1eece0(s_surface_key_array *array, void *owner)
{
 long index = 0;
 if (array->count > 0)
 {
  do
  {
   /* the owner's object reference, c_shape_owner::object */
   if (!function_1ef3e0((byte *)owner + 0x14, array->keys[index]))
   {
    array->count--;
    for (long i = index; i < array->count; i++)
     array->keys[i] = array->keys[i + 1];
    index--;
   }
  } while (++index < array->count);
 }
}

struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

bool function_1efdc0(s_lookup *lookup, const point3f *point);

struct s_type_1a7926
{
 byte unknown00[8];
 transform4x3f root_matrix;
 byte unknown3c[0x48 - 0x3c];
 short *node_indices;
 byte unknown4c[4];
 transform4x3f *field_50;
};

bool function_20a9a0(s_type_1a7926 *matrices, long object_index);

struct s_model_choice
{
 long field_0;
 signed char first;
 signed char second;
 signed char third;
 byte field_7;
};

struct s_model_region
{
 long field_0;
 signed char first;
 signed char second;
 short field_6;
 long count;
 s_model_choice *choices;
};

struct s_model_regions
{
 byte field_0[0x70];
 long count;
 s_model_region *regions;
};

PRIVATE inline bool surface_object_has_physics(long object_index)
{
 s_type_1a7926 info;
 return function_20a9a0(&info, object_index);
}

// @retail 0x1ef500
long function_1ef500(long object_index, long position)
{
 long region_index;
 long choice_index;
 long result = NONE;
 if (position == NONE)
 {
  region_index = 0;
  choice_index = NONE;
 }
 else
 {
  region_index = position & 31;
  choice_index = (position >> 5) & 255;
 }
 if (surface_object_has_physics(object_index))
 {
  s_type_1a7926 info;
  function_20a9a0(&info, object_index);
  s_model_regions *model = *(s_model_regions **)&info.unknown3c[8];
  byte *physics = (byte *)info.node_indices;
  for (; result == NONE && region_index < model->count; ++region_index)
  {
   s_model_region *region = &model->regions[region_index];
   if (region->second != NONE)
   {
    for (++choice_index; result == NONE && choice_index < region->count; ++choice_index)
    {
     s_model_choice *choice = &region->choices[choice_index];
     if (choice->third != NONE)
     {
      byte *groups = *(byte **)(physics + 0xc4);
      byte *entries = *(byte **)(groups + region->second * 12 + 8);
      byte *entry = entries + choice->third * 12;
      if (*(long *)(entry + 4) > 0)
      {
       long index = **(short **)(entry + 8);
       byte *bodies = *(byte **)(physics + 0x3c);
       if (*(short *)(bodies + index * 0x90 + 0x1e) > 1)
        result = region_index | (choice_index << 5);
      }
     }
    }
   }
   choice_index = NONE;
  }
 }
 else
 {
  s_object_view *object = ((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
  byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
  s_model_regions *model = (s_model_regions *)g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
  for (; result == NONE && region_index < model->count; ++region_index)
  {
   s_model_region *region = &model->regions[region_index];
   if (region->first != NONE)
   {
    for (++choice_index; result == NONE && choice_index < region->count; ++choice_index)
    {
     if (region->choices[choice_index].second != NONE)
     {
      result = region_index | (choice_index << 5);
      break;
     }
    }
   }
   choice_index = NONE;
  }
 }
 return result;
}

bool function_10a520(long object_index);

// @retail 0x1ef8d0
long function_1ef8d0(long object_index)
{
 long count = 0;
 if (function_10a520(object_index))
 {
  s_object_view *object = ((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
  byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
  s_model_regions *model = (s_model_regions *)g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
  if (model->count > 0)
  {
   long position = function_1ef500(object_index, NONE);
   while (position != NONE)
   {
    ++count;
    position = function_1ef500(object_index, position);
   }
  }
 }
 return count;
}

static __forceinline long surface_object_count()
{
 short offset = *(short *)((byte *)g_468630[6] + 0xa);
 return *(long *)((byte *)g_4e0350 + offset);
}

static __forceinline bool surface_object_has_choices(long index, long *objects, s_record_pool *pool)
{
 long object_index = objects[index];
 return object_index != NONE && function_1ef8d0(object_index) &&
  *(short *)((byte *)pool->data + (objects[index] & 0xffff) * 12 + 4) != NONE;
}

// @retail 0x1ef810
long function_1ef810(long index)
{
 long *objects = g_51e9cc;
 s_record_pool *pool = g_4e0300;
 ++index;
 if (index < surface_object_count())
 {
  long next = index + 1;
  while (!surface_object_has_choices(index, objects, pool))
  {
   if (next >= surface_object_count()) break;
   ++index;
   ++next;
  }
 }
 if (index >= surface_object_count() || !surface_object_has_choices(index, objects, pool))
  index = NONE;
 return index;
}

/* the key after the given one: the next surface, instance or object choice */
// @retail 0x1ef6d0
dword c_shape_global_owner::next_key(dword key)
{
 s_surface_list *world = (s_surface_list *)g_4e0340;
 long count = world->count;
 if (world->count > 0)
 {
  s_surface_flags const *last = &world->surfaces[world->count - 1];
  if (last->flags & 0x10) --count;
 }
 long *objects = g_51e9cc;
 long index = key & 0xffff;
 long position = (key >> 16) & 0x1fff;
 long kind = key >> 29;
 if (kind == 1)
 {
  if (index < count - 1) return (index + 1) | 0x20000000;
  index = NONE;
 }
 if (kind == 1 || kind == 2)
 {
  s_surface_globals *globals = (s_surface_globals *)g_4e0348;
  if (globals->count > 0)
  {
   for (; index < globals->count - 1; ++index)
   {
    s_surface_owner *owner = &globals->owners[globals->instances[index + 1].owner];
    if (*(long *)((byte *)owner + 0x98) > 0)
     return (index + 1) | 0x40000000;
   }
  }
  index = NONE;
 }
 long object_index;
 long next_position = NONE;
 if (index != NONE)
 {
  object_index = objects[index];
  if (object_index != NONE)
   next_position = function_1ef500(object_index, position);
 }
 if (next_position == NONE)
 {
  index = function_1ef810(index);
  if (index != NONE)
  {
   object_index = objects[index];
   next_position = function_1ef500(object_index, NONE);
  }
 }
 if (index != NONE)
 {
  s_type_1a7926 info;
  long next_kind = function_20a9a0(&info, object_index) ? 3 : 4;
  return (((next_kind << 13) | next_position) << 16) | index;
 }
 return (dword)NONE;
}

#include <new>

c_a *surface_empty_shape(void *storage)
{
 return new (storage) c_d;
}

struct s_1de6d2;
struct s_1de6d3
{
 real field_0;
 plane3f const *field_4;
 long field_8;
 long field_c;
 long field_10;
 byte field_14;
 byte field_15;
 short field_16;
 long field_18;
 long field_1c[256];
};

struct s_1de6d0
{
 dword field_0;
 s_1de6d2 const *field_4;
 short field_8;
 dword const *field_c;
 point3f const *field_10;
 vector3f const *field_14;
 s_1de6d3 *field_18;
 long field_1c;
 byte field_20;
 long field_24;
};

struct s_lookup_ray_result
{
 dword position;
 short node;
 short item;
 s_1de6d3 hit;
};

void function_141590(const transform4x3f *matrix, transform4x3f *inverse);
bool __stdcall function_1de6d0(s_1de6d0 *query, long root, real start, real end);
