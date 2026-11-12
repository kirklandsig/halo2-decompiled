// @flags /O2 /GL-
// Havok (third-party physics library): removing a pair of collidables from a
// cache of pairs: from the sorted pairs still being merged, or else from the
// unsorted pairs of either collidable's island, or from the agents of an
// island, ending each agent found.

class island_316110;
class cache_316110;

// a collidable: its id orders the pairs
class collidable_316110
{
public:
	long field_0;
	unsigned long id;
};

class owner_316110
{
public:
	char unknown0[0x44];
	island_316110 *island;
};

class base_316110
{
public:
	char unknown0[0x10];
};

// the entity a collidable belongs to
class entity_316110 : public base_316110, public collidable_316110
{
public:
	char unknown18[0x20 - 0x18];
	owner_316110 *owner;
};

class agent_owner_316110
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void agent_ended() {}
};

class agent_316110
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void end() {}

	long field_4;
	agent_owner_316110 *owner;
};

// a pair of collidables
struct pair_316110
{
	collidable_316110 *first;
	collidable_316110 *second;
};

// a pair with the agent that handles it
struct record_316110
{
	collidable_316110 *first;
	collidable_316110 *second;
	agent_316110 *agent;
	long field_c;
};

struct pairs_316110
{
	pair_316110 *data;
	int size;
};

struct records_316110
{
	record_316110 *data;
	int size;
};

// the pairs an island keeps: its agents' records, its unsorted pairs, and
// the sorted pairs being merged
class cache_316110
{
public:
	char unknown0[0xc];
	bool merging;
	char unknownd[0x4c - 0xd];
	records_316110 records;
	char unknown54[0x58 - 0x54];
	pairs_316110 pairs;
	char unknown60[0x80 - 0x60];
	record_316110 *next;
	record_316110 *end;
	record_316110 *written;

	void function_316110(pair_316110 const *pair);
};

class island_head_316110
{
public:
	char unknown0[0x1c];
};

class island_316110 : public island_head_316110, public cache_316110
{
};

// whether a record comes before a pair in the order of their ids
inline bool before_316110(record_316110 const *record, pair_316110 const *pair)
{
	if (record->first->id < pair->first->id)
	{
		return true;
	}
	if (record->first->id != pair->first->id)
	{
		return false;
	}
	return record->second->id < pair->second->id;
}

// the island of a collidable's entity
inline island_316110 *island_of_316110(collidable_316110 *collidable)
{
	return static_cast<entity_316110 *>(collidable)->owner->island;
}

// ends an agent, and tells its owner
inline void end_agent_316110(agent_316110 *agent)
{
	if (agent)
	{
		agent_owner_316110 *owner = agent->owner;
		agent->end();
		owner->agent_ended();
	}
}

// score 6 (was 185): the first cleanup still exchanges eax/edx, and the
// first owner load uses ecx between the two loads. A second-member reference
// with an element reference in each pair loop 6 (from 87); element references alone
// 84 rejected. First-member reference 170; second or both member references
// 141; ID references 87. On 6: owner reference 6, agent reference 73, helper
// early return 6, owner-object reference 6, record reference for cleanup 73,
// island snapshot 183; first element reference only 9; both pair references
// 6; first pair plus record references 9.
// Separate scan blocks with early handle snapshots 87 (from 167); fresh
// pair reads 141, as did a late second handle, end snapshot, early return,
// record base, force-inline cleanup, array pointers, split copy steps and
// pair/order references. Original merge prefix plus separate scans 87.
// At 167, the second collidable was taken up front, so the
// pair dies in edx as retail's does; retail still reloads the pair from its
// slot inside the loop and keeps the array pointer in esi, ours keeps the
// second in the slot and the pointer in edx, and the two virtual calls swap
// their vtable registers
// Isolated end/flag forms 87; delayed second handle and first-id per-use
// 141; cleanup receiver reference and caller null guards 87; first id once
// 240; shared equality predicate 87. Original body and inline functions retained.
// Cleanup as an agent method 6; entity and owner locals 6; an else arm,
// post-increment copy and member end on 6: 6. Cleanup as a record method 54,
// with an agent sample 6; record input 6; immutable callback receiver 6.
// Agent-pointer reference 200; record argument with a member reference 200;
// cleanup on the callback receiver with pointer or reference argument 6;
// explicit cdecl 6; pair-member reference lookup 227; collidable
// member lookup 6; equality template, inline or force-inline, 186; first
// array pointer 6; owner-pointer member reference 6; order-helper result 6;
// const record cleanup 54.
// Direct record pointer or reference cleanup 54; pair base with const
// cleanup 54; const record cleanup with an element reference 67; external
// pair-reference argument 6; pair base with agent-value cleanup 6. A single
// worker search of 474 variants found none below 6.
// Equal-first order test 241; compound order test 257; pair-loop continues 6;
// record-loop continue 6; while pair scans 6; source-index shift 52;
// shift data snapshot 36; earlier first handle 209. NOT kept: no gain.
// @retail 0x316110
void cache_316110::function_316110(pair_316110 const *pair)
{
	record_316110 *record = next;
	record_316110 *const last = end;
	merging = true;
	while (record < end && before_316110(record, pair))
	{
		record++;
	}
	if (record != last && record->first->id == pair->first->id && record->second->id == pair->second->id)
	{
		while (next < record)
		{
			*written = *next;
			written++;
			next++;
		}
		end_agent_316110(next->agent);
		next++;
		return;
	}
	collidable_316110 *first = pair->first;
	collidable_316110 *const &second = pair->second;
	{
		pairs_316110 &pairs = island_of_316110(first)->pairs;
		for (int i = 0; i < pairs.size; i++)
		{
			pair_316110 const &entry = pairs.data[i];
			if (entry.first->id == first->id && entry.second->id == second->id)
			{
				pairs.size--;
				pairs.data[i] = pairs.data[pairs.size];
				return;
			}
		}
	}
	{
		pairs_316110 &pairs = island_of_316110(second)->pairs;
		for (int i = 0; i < pairs.size; i++)
		{
			pair_316110 const &entry = pairs.data[i];
			if (entry.first->id == first->id && entry.second->id == second->id)
			{
				pairs.size--;
				pairs.data[i] = pairs.data[pairs.size];
				return;
			}
		}
	}
	{
		island_316110 *island = island_of_316110(first);
		if (island == static_cast<island_316110 *>(this))
		{
			island = island_of_316110(second);
		}
		records_316110 &records = island->records;
		for (int j = 0; j < records.size; j++)
		{
			if (records.data[j].first->id == first->id && records.data[j].second->id == second->id)
			{
				end_agent_316110(records.data[j].agent);
				records.size--;
				for (int k = j; k < records.size; k++)
				{
					records.data[k] = records.data[k + 1];
				}
				return;
			}
		}
	}
}
