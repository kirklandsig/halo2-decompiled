#ifndef QUANTIZED_BOXES_2DFCC0_H
#define QUANTIZED_BOXES_2DFCC0_H

struct quantized_box_2dfcc0;

// Moves the boxes' endpoint indices past inserted or removed endpoints.
class quantized_boxes_2dfcc0
{
public:
	void function_2dfcc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits);
	void function_2dfdc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits);
};

#endif
