// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_03bcb0.h"

long function_13a690(long mode);
bool function_22acb4(long local_player_index);

struct s_bitmap_widget_22b7dd
{
    byte unknown00[0x1c];
    word mode;
    byte unknown1e[6];
    long bitmap_tag;
    byte unknown28[4];
    long reference0;
    byte unknown30[4];
    long reference1;
    byte unknown38[4];
    long reference2;
    char sequences[3];
};

struct s_bitmap_sprite_22b7dd
{
    short bitmap_index;
    byte unknown02[0x1e];
};

struct s_bitmap_sequence_22b7dd
{
    byte unknown00[0x20];
    short first_bitmap;
    short bitmap_count;
    byte unknown24[0x10];
    long sprite_count;
    s_bitmap_sprite_22b7dd *sprites;
};

struct s_bitmap_sequence_block_22b7dd
{
    long count;
    s_bitmap_sequence_22b7dd *elements;
};

struct s_bitmap_tag_22b7dd
{
    byte unknown00[0x3c];
    s_bitmap_sequence_block_22b7dd sequences;
    long bitmap_count;
    byte *bitmaps;
};

/* predicts the texture of the bitmap a widget shows in its current mode;
   false when the texture is not ready */
// @retail 0x22b7dd
bool function_22b7dd(byte const *arg_1, long arg_2)
{
    s_bitmap_widget_22b7dd const *widget = (s_bitmap_widget_22b7dd const *)arg_1;
    long mode = function_13a690(widget->mode);
    bool result = true;
    long reference;
    switch (mode)
    {
    case 0:
        reference = widget->reference0;
        break;
    case 1:
        reference = widget->reference1;
        break;
    case 2:
        reference = widget->reference2;
        break;
    default:
        goto done;
    }
    if (reference != NONE && widget->bitmap_tag != NONE)
    {
        s_bitmap_tag_22b7dd *tag = (s_bitmap_tag_22b7dd *)g_4e3b44[widget->bitmap_tag & 0xffff].bytes;
        long sequence_index = 0;
        switch (mode)
        {
        case 0:
            sequence_index = widget->sequences[0];
            break;
        case 1:
            sequence_index = widget->sequences[1];
            break;
        case 2:
            sequence_index = widget->sequences[2];
            break;
        }
        if (sequence_index >= 0)
        {
            s_bitmap_sequence_block_22b7dd const *sequences = &tag->sequences;
            if (sequence_index < sequences->count)
            {
                s_bitmap_sequence_22b7dd *sequence = &sequences->elements[sequence_index];
                if (sequence->sprite_count > 0)
                {
                    long index = 0;
                    if (sequence->sprite_count > 1 && function_22acb4(arg_2))
                        index++;
                    byte *bitmap = tag->bitmaps + sequence->sprites[index].bitmap_index * 0x74;
                    if (!function_3bcb0((s_bitmap_data *)bitmap))
                        result = false;
                }
                else if (sequence->bitmap_count > 0)
                {
                    long index = 0;
                    if (sequence->bitmap_count > 1 && function_22acb4(arg_2))
                        index++;
                    byte *bitmap = tag->bitmaps + (sequence->first_bitmap + index) * 0x74;
                    if (!function_3bcb0((s_bitmap_data *)bitmap))
                        result = false;
                }
            }
            else if (sequence_index == 0 && tag->sequences.count == 0 && tag->bitmap_count > 0)
            {
                if (!function_3bcb0((s_bitmap_data *)tag->bitmaps))
                    result = false;
            }
        }
    }
done:
    return result;
}
