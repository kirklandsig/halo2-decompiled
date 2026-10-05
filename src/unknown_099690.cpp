// @flags /O2 /Gr

#include "unknown_096ed0.h"

// @retail 0x99690
c_handle_table_450cd0 *function_99690(c_handle_table_450cd0 *self, long index, long new_state)
{
	long i = index & 0x3ff;
	short old_state = (short)self->entries[i].state;

	if (new_state == old_state)
		return self;
	s_handle_peer *peer = &self->table->peers[i];

	if (old_state == 1)
		self->unknown502c--;
	else if (old_state == 3)
	{
		if (peer->mask & (1 << self->shift))
			self->unknown503c--;
		else if (self->entries[i].unknown04)
			self->unknown5034--;
	}

	if (new_state == 1)
		self->unknown502c++;
	else if (new_state == 3)
	{
		if (peer->mask & (1 << self->shift))
			self->unknown503c++;
		else if (self->entries[i].unknown04)
			self->unknown5034++;
	}

	if (new_state == 0)
	{
		self->entries[i].handle = NONE;
		if (self->unknown5024 == i)
		{
			self->unknown5024 = self->entries[i].unknown0a;
		}
		else
		{
			long j = 0;
			do
			{
				if (self->entries[j].unknown0a == i)
				{
					self->entries[j].unknown0a = self->entries[i].unknown0a;
					break;
				}
				j++;
			}
			while (j < 0x400);
		}
		self->entries[i].unknown0a = (short)NONE;
		self->entries[i].unknown10 = 0;
	}
	else if (self->entries[i].state == 0)
	{
		self->entries[i].handle = index;
		self->entries[i].unknown04 = 0;
		self->entries[i].unknown0c = 0;
		self->entries[i].unknown0a = (short)self->unknown5024;
		self->unknown5024 = i;
	}
	self->entries[i].state = (short)new_state;
	return self;
}
