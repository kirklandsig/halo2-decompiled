/* ENGINE_PEER.H: the engine objects in the table at 0x55e4d0 (indexed by the
   engine index of g_4e9ae8). Slots 41..43 are wrapped by the game engine
   table of 072c70 and 2bcdd0, slot 39 and slots 44..46 are called by 0a45d0,
   slots 47..49 by the entity definitions of 09a5e0.
   Their methods live in the library range or another source file; the slots
   are empty. */

#ifndef ENGINE_PEER_H
#define ENGINE_PEER_H

#include "unknown_11c920.h"

struct s_stats;
struct s_event;

class c_engine_peer
{
public:
	virtual long p0() { return 0; }
	virtual bool p1() { return false; }
	virtual void p2() {}
	virtual void p3() {}
	virtual bool p4(long) { return false; }
	virtual void p5(long) {}
	virtual void p6(long) {}
	virtual void p7(long) {}
	virtual void p8(long) {}
	virtual void p9(long, long) {}
	virtual void p10(long) {}
	virtual void p11() {}
	virtual void p12() {}
	virtual void p13(long) {}
	virtual void p14(long) {}
	virtual void p15(long) {}
	virtual void p16() {}
	virtual void p17(long, long) {}
	virtual void p18() {}
	virtual real p19(long) { return 0.0f; }
	virtual void p20() {}
	virtual void p21(long) {}
	virtual void p22(long) {}
	virtual void p23(long, long) {}
	virtual void p24(long, long) {}
	virtual long p25(long time_left, bool flag, bool unknown) { return 0; } /* the round time left (162470) */
	virtual long p26() { return 0; }
	virtual bool p27(short, short) { return false; }
	virtual void p28() {}
	virtual void p29(long, long, bool) {}
	virtual void p30(long, long, bool, long) {}
	virtual long p31(long, long, bool) { return 0; }
	virtual void p32() {}
	virtual void p33() {}
	virtual void p34() {}
	virtual bool p35(long, long) { return false; }
	virtual void p36(long) {}
	virtual long p37(long, bool *) { return 0; }
	virtual bool p38(long) { return false; }
	virtual long get_current_id() { return 0; }
	virtual void p40() {}
	virtual void p41(s_stats *) {}
	virtual long p42(long, long *, long) { return 0; }
	virtual byte p43(long, long) { return 0; }
	virtual long p44(long, long) { return 0; }
	virtual long p45(long, long, long) { return 0; }
	virtual bool p46(long, long, long) { return false; }
	virtual void p47(long, long, long) {}
	virtual void p48(long, long, long, long) {}
	virtual bool p49(long, long, long, long) { return false; }
	/* the text of a token of an event's message that the engine knows
	   (unknown_19de80.cpp), returning its length or 0 */
	virtual long get_event_token_text(word const *, long, s_event *, word *, long) { return 0; }
};

#endif
