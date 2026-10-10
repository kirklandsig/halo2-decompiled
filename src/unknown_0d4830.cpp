#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "effects.h"

struct s_sort_record
{
	word index;
	word key;
	byte unknown04;
	byte group;
	word subkey;
	word value08;
	word unknown0a;
	dword value0c;
	dword value10;
	dword unknown14;
};

typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);
typedef void (__stdcall *t_41490_callback)(void *);

long function_15d70(long tag, long stage, long pass, bool first, bool second);
void function_41490(short group, long tag, short kind, real distance, t_record_fill fill,
	dword value, t_41490_callback callback, void *context, point3f const *position);

// @flags /O2 /Gr

/* the widget types (antenna, cloth, light volume...): a table of 0x38 byte
   entries keyed by the tag group, with a set of callbacks each */
struct s_widget_type
{
	long key;
	long unknown04;
	void (*initialize)(void);
	void (*field_c_5)(void);
	void (*field_10_2)(void);
	void *unknown14;
	long (__stdcall *create)(long arg, long object_index);
	void (__stdcall *dispose)(long handle);
	void (__stdcall *field_20)(long object_index);
	long unknown24;
	long tag_index;
	t_41490_callback callback;
	long (__stdcall *get_kind)(long handle);
	void (__stdcall *draw)(short group, long handle);
};

/* one widget attached to an object (12 bytes) */
struct s_widget
{
	word identifier;
	short type;
	long handle;
	long next;
};

struct s_widget_reference
{
	long key;
	long arg;
};

struct s_widget_object_tag_data
{
	byte unknown00[0x9c];
	long count;
	s_widget_reference *references;
};

struct s_widget_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

struct s_widget_object
{
	long tag_index;
	byte unknown04[0xdc - 4];
	long widget_head;
};

s_widget_type g_467498[3];

PRIVATE short widget_type_find(long key)
{
	short i;

	for (i = 0; i < 3; i++)
	{
		if (g_467498[i].key == key)
			return i;
	}

	return NONE;
}

// @retail 0xd4830
void function_d4830(void)
{
	short i;

	g_4e0320 = data_new_inlined("widget", 0x40, sizeof(s_widget), 0, g_510c2c);

	for (i = 0; i < 3; i++)
	{
		if (g_467498[i].initialize)
			g_467498[i].initialize();
	}
}

// @retail 0xd4890
void function_d4890(void)
{
	short i;

	g_4e0320->valid = 1;
	record_pool_release_all(g_4e0320);

	for (i = 0; i < 3; i++)
	{
		if (g_467498[i].field_c_5)
			g_467498[i].field_c_5();
	}
}

// @retail 0xd48d0
void function_d48d0(void)
{
	short i;

	for (i = 0; i < 3; i++)
	{
		if (g_467498[i].field_10_2)
			g_467498[i].field_10_2();
	}

	g_4e0320->valid = 0;
}

// @retail 0xd4db0
void __stdcall function_d4db0(long object_index)
{
	long const *index_reference = &object_index;
	s_widget_type *entry = g_467498;

	for (long remaining = 3; remaining != 0; --remaining, ++entry)
	{
		if (entry->field_20)
			entry->field_20(*index_reference);
	}
}

// @retail 0xd4900
void object_widgets_new(long object_index)
{
	s_widget_object_header *header = (s_widget_object_header *)(g_4e0300->data + (object_index & 0xffff) * 12);
	s_widget_object *object = (s_widget_object *)header->object;
	s_widget_object_tag_data *field_4_7 = (s_widget_object_tag_data *)g_4e3b44[object->tag_index & 0xffff].bytes;
	short i;

	object->widget_head = NONE;

	for (i = 0; i < field_4_7->count; i++)
	{
		s_widget_reference *reference = &field_4_7->references[i];
		short type = widget_type_find(reference->key);

		if (type != NONE && reference->arg != NONE)
		{
			s_widget_type *widget_type = &g_467498[type];
			long widget_index = record_pool_allocate(g_4e0320);

			if (widget_index != NONE)
			{
				s_widget *widget = (s_widget *)(g_4e0320->data + (widget_index & 0xffff) * 12);

				widget->type = type;

				if (widget_type->create)
				{
					widget->handle = widget_type->create(reference->arg, object_index);

					if (widget->handle != NONE)
					{
						widget->next = object->widget_head;
						object->widget_head = widget_index;
					}
					else
					{
						record_pool_release(g_4e0320, widget_index);
					}
				}
				else
				{
					widget->next = object->widget_head;
					object->widget_head = widget_index;
					widget->handle = NONE;
				}
			}
		}
	}
}

// @retail 0xd4a60
void object_widgets_delete(long object_index)
{
	s_widget_object_header *header = (s_widget_object_header *)(g_4e0300->data + (object_index & 0xffff) * 12);
	s_widget_object *object = (s_widget_object *)header->object;
	long widget_index = object->widget_head;

	while (widget_index != NONE)
	{
		s_widget *widget = (s_widget *)(g_4e0320->data + (widget_index & 0xffff) * 12);
		long next = widget->next;

		if (widget->handle != NONE)
			g_467498[widget->type].dispose(widget->handle);

		record_pool_release(g_4e0320, widget_index);
		widget_index = next;
	}

	object->widget_head = NONE;
}

// @retail 0xd4ae0
void object_widget_delete(long object_index, long handle)
{
    long const volatile *handle_reference = &handle;
    long handle_value = *handle_reference;
    if (handle_value != NONE)
    {
        s_widget_object_header *header = (s_widget_object_header *)(g_4e0300->data + (object_index & 0xffff) * 12);
        s_widget_object *object = (s_widget_object *)header->object;
        long widget_index = object->widget_head;

        if (widget_index != NONE)
        {
            s_widget *widget = (s_widget *)(g_4e0320->data + (widget_index & 0xffff) * 12);

            if (widget->handle == handle_value)
            {
                object->widget_head = widget->next;

                if (widget->handle != NONE)
                    g_467498[widget->type].dispose(widget->handle);

                record_pool_release(g_4e0320, widget_index);
            }
            else
            {
                long previous_index = widget_index;

                do
                {
                    s_widget *previous = (s_widget *)(g_4e0320->data + (previous_index & 0xffff) * 12);
                    long next_index = previous->next;

                    if (next_index == NONE)
                        break;

                    widget = (s_widget *)(g_4e0320->data + (next_index & 0xffff) * 12);

                    if (widget->handle == handle_value)
                    {
                        previous->next = widget->next;

                        if (widget->handle != NONE)
                            g_467498[widget->type].dispose(widget->handle);

                        record_pool_release(g_4e0320, next_index);
                        break;
                    }

                    previous_index = next_index;
                }
                while (true);
            }
        }
    }
}

// @retail 0xd4bc0
bool __stdcall function_d4bc0(long tag, long context, long pass, long stage,
	long entry, long handle, void *record)
{
	s_sort_record *out = (s_sort_record *)record;
	out->group = 0;
	out->value0c = NONE;
	out->subkey = 0xffff;
	out->unknown04 = function_15d70(tag, stage, pass, true, false);
	if (stage == 3)
		out->unknown04 = 2;
	else if (stage == 10)
		out->unknown04 = (bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1) ? 0 : 1;
	bool result = out->unknown04 != 0xff;
	if (result)
	{
		long const *reference;
		dword kind = *(dword *)g_4e3b44[(short)tag].unknown00;
		if (kind == 0x5052544d || kind == 0x70727433)
			reference = function_137bd0(tag)->function_x947334();
		else
			reference = *(long **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
		byte *groups = *(byte **)(g_4e3b44[*reference & 0xffff].bytes + 0x5c);
		long group = *(word *)(*(byte **)(groups + 4) + pass * 10) & 0x1ff;
		long first = (*(word **)(groups + 0xc))[group + stage] & 0x1ff;
		long material = *(long *)(*(byte **)(groups + 0x14) + (first + entry) * 10 + 4);
		byte *data = *(byte **)(*(byte **)(g_4e3b44[material & 0xffff].bytes + 0x20) + 4);
		out->value10 = (dword)data;
		out->value08 = 0;
		out->unknown14 = 0;
	}
	return result;
}



// @retail 0xd4cf0
void __stdcall function_d4cf0(long object_index, short group, dword flags)
{
    dword const *flags_reference = &flags;
    s_widget_object_header *header = (s_widget_object_header *)(g_4e0300->data + (object_index & 0xffff) * 12);
    s_widget_object *object = (s_widget_object *)header->object;
    long index = object->widget_head;
    while (index != NONE)
    {
        s_widget *widget = (s_widget *)(g_4e0320->data + (index & 0xffff) * 12);
        s_widget_type *type = &g_467498[widget->type];
        if ((group != 2 || (*flags_reference & 0x2000)) &&
            (group != 1 || (*flags_reference & 0x1000)))
        {
            if (type->tag_index)
            {
                function_41490(group, type->get_kind(widget->handle), NONE, 640.0f,
                    (t_record_fill)function_d4bc0, type->tag_index, type->callback,
                    (void *)widget->handle, (point3f const *)((byte *)object + 0x30));
            }
            if (type->draw)
                type->draw(group, widget->handle);
        }
        index = widget->next;
    }
}
