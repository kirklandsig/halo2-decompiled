#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"

// @flags /O2 /arch:SSE /Gr

/* ---- slots 14..16 of the retail table: wrappers around slots 41..43 of the
   peer vtable (they run on the peer object, not on the engine) ---- */
class c_engine_peer_derived : public c_engine_peer
{
public:
	virtual void q0(long, s_stats *);
	virtual void q1(dword *, long, long);
	virtual bool q2(dword, long, long);
};

// @retail 0x2bcdd0
void c_engine_peer_derived::q0(long, s_stats *stats)
{
	memset(stats, 0, sizeof(s_stats));
	p41(stats);
}

// @retail 0x2bce00
void c_engine_peer_derived::q1(dword *value, long, long c)
{
	long decoded_peer_bits = 0;
	dword m = *value & 0x1f;

	if (m)
		p42(m, &decoded_peer_bits, c);
	*value = decoded_peer_bits;
}

// @retail 0x2bce40
bool c_engine_peer_derived::q2(dword a, long, long c)
{
	dword m = a & 0x1f;
	bool result = true;

	if (m)
		result = p43(m, c) != 0;
	return result;
}