// @flags /O2 /Ob1 /Gr /arch:SSE
#include "unknown_1cec30.h"

struct s_278f00
{
	byte field_0[0x3c];
	hkRigidBody **field_3c;
	long field_40;
	byte field_44[0x6a];
	bool field_ae;
};

struct s_278f01
{
	s_278f00 **field_0;
	long field_4;
	long field_8;
};

struct s_278f02
{
	byte field_0[3];
	byte field_3;
	byte field_4[4];
	s_havok_object *field_8;
};

struct s_278f03
{
	byte field_0[0xc0];
	word : 5;
	word field_c0_5 : 1;
	word field_c0 : 1;
	word : 9;
	byte field_c2[0x12];
	long field_d4;
};

extern hkWorld *g_51e9a4;
extern bool g_55e4fc;
extern bool g_47f05b;
void __stdcall function_1c3770(long arg_0, dword arg_1);
void function_146bf0();
void function_1d1260(s_havok_component *arg_0);
void __stdcall function_b8540(long arg_0);
void __stdcall function_b8890(long arg_0);

bool function_a7670(long object_index);

/* processes the Havok islands flagged as pending in the world's two island lists */
// @retail 0x278f00
void function_278f00()
{
	long local_0[128];
	long local_1[128];
	if (g_55e4fc && !g_47f05b)
	{
		g_47f05b = true;
		for (long local_2 = 0; local_2 < 2; local_2++)
		{
			s_278f01 *local_3;
			if (local_2 == 0)
				local_3 = (s_278f01 *)((byte *)g_51e9a4 + 8);
			else
				local_3 = (s_278f01 *)((byte *)g_51e9a4 + 0x14);
			for (long island_index = 0; island_index < local_3->field_4; island_index++)
			{
				s_278f00 *local_5 = local_3->field_0[island_index];
				if (!local_5->field_ae)
					continue;
				island_index--;
				do
				{
					local_5->field_ae = false;
					long local_7 = 0;
					long local_8 = 0;
					long local_9 = 0;
					do
					{
						local_7 = 0;
						local_8 = 0;
						for (long local_10 = 0; local_10 < local_5->field_40; local_10++)
						{
							hkRigidBody *local_11 = local_5->field_3c[local_10];
							if (!local_11->m_fixed)
							{
								long local_12 = havok_component_get(havok_entity_property_get((hkEntity *)local_11, 0x2001))->object_index;
								s_278f02 *local_13 = &((s_278f02 *)g_4e0300->data)[local_12 & 0xffff];
								bool local_14 = local_9 > 0 || !(((1 << local_13->field_3) & 1) || ((1 << local_13->field_3) & 0x80));
								bool local_15 = false;
								long local_16;
								for (local_16 = 0; local_16 < local_7; local_16++)
								{
									if (local_0[local_16] == local_12)
									{
										local_15 = true;
										break;
									}
								}
								for (local_16 = 0; local_16 < local_8; local_16++)
								{
									if (local_1[local_16] == local_12)
									{
										local_15 = true;
										break;
									}
								}
								if (!local_15)
								{
									if (local_14)
										local_0[local_7++] = local_12;
									else
										local_1[local_8++] = local_12;
								}
							}
						}
						if (local_7 > 0)
							break;
						local_9++;
					} while (local_9 < 2);
					for (long local_10 = 0; local_10 < local_7; local_10++)
					{
						long local_11 = local_0[local_10];
						s_havok_object *local_12 = havok_object_get(local_11);
						if (function_a7670(local_11))
						{
							((s_278f03 *)local_12)->field_c0_5 = true;
							function_1c3770(local_11, 0);
						}
						else
						{
							bool local_13 = TEST_FIELD_BIT(((s_278f03 *)local_12)->field_c0);
							if (local_13)
								function_146bf0();
							local_12 = havok_object_get(local_11);
							long local_14 = local_12->havok_component_index;
							if (local_14 != NONE)
							{
								s_havok_component *local_15 = havok_component_get(local_14);
								long local_16 = local_15->object_index;
								local_15->~s_havok_component();
								record_pool_release(g_51e9b8, local_14);
								s_havok_object *local_17 = havok_object_get(local_16);
								if (*((byte *)local_17 + 0xc0) & 1)
								{
									local_17->havok_flag = false;
									(*g_51e9a0)--;
								}
								local_12->havok_component_index = NONE;
							}
							local_12 = havok_object_get(local_11);
							if (*((byte *)local_12 + 0xc0) & 1)
							{
								local_12->havok_flag = false;
								(*g_51e9a0)--;
							}
							if (local_13)
							{
								function_278f00();
								function_146bf0();
							}
							function_b8540(local_11);
						}
					}
					for (long local_10 = 0; local_10 < local_8; local_10++)
					{
						long local_11 = local_1[local_10];
						s_havok_object *local_12 = havok_object_get(local_11);
						if (local_12->havok_component_index != NONE)
							function_1d1260(havok_component_get(local_12->havok_component_index));
						((s_278f03 *)local_12)->field_c0 = false;
						function_b8890(local_11);
					}
				} while (false);
			}
		}
		g_47f05b = false;
		g_55e4fc = false;
	}
}
