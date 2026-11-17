#include "unknown_4cf790.h"
// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_07A8A0.CPP: the registered XNet keys and the secure addresses
   they give (lane D, outside its regions: the network session and observer
   register and resolve their keys through them). The key registry g_4cf7d4
   is in unknown_07a9a0.cpp. */

#include "unknown_11c920.h"
#include "unknown_07aec0.h"
#include <xtl.h>

struct s_xnet_registry_entry
{
	bool valid;
	bool host;
	byte unknown2[2];
	long local;
	XNKID kid;
	XNKEY key;
};



#define BYTE_SWAP_LONG(v) ((((v) & 0xff0000) | ((v) >> 16)) >> 8 | ((((v) << 16) | ((v) & 0xff00)) << 8))

// @retail 0x7a920
bool transport_security_register_key(long index, long local, bool host, const XNKID *kid, const XNKEY *key)
{
	bool local_2 = false;
	const volatile long *local_3 = &local;
	const XNKID *const *local_0 = &kid;
	const XNKID *local_1 = *local_0;
	s_xnet_registry_entry *entry = &g_4cf7d4[index];

	if (entry->valid && g_4cf7d4[index].valid)
	{
		XNetUnregisterKey(&entry->kid);
		entry->valid = local_2;
	}
	if (*local_3 || XNetRegisterKey(local_1, key) == 0)
	{
		local_2 = true;
		entry->kid = *local_1;
		entry->key = *key;
		entry->host = host;
		entry->local = *local_3;
		entry->valid = local_2;
	}
	return local_2;
}

// @retail 0x7a8a0
bool transport_security_create_key(long local, long index, bool online)
{
	XNKID kid;
	XNKEY key;

	if (local)
	{
		memset(&kid, 0, sizeof(kid));
		memset(&key, 0, sizeof(key));
	}
	else if (XNetCreateKey(&kid, &key) == 0)
	{
		kid.ab[0] = (kid.ab[0] & 0xf) | (online ? XNET_XNKID_ONLINE_PEER : XNET_XNKID_SYSTEM_LINK);
	}
	if (transport_security_register_key(index, local, true, &kid, &key))
		return true;
	return false;
}

// @retail 0x7adf0
bool transport_security_get_address(long key_index, long local, const XNADDR *xnaddr, word port, s_type_99af70 *address)
{
	s_xnet_registry_entry *entry = &g_4cf7d4[key_index];
	bool result = false;

	if (!local && entry->valid && !entry->local)
	{
		IN_ADDR in_addr;
		result = XNetXnAddrToInAddr(xnaddr, &entry->kid, &in_addr) == 0;
		if (result)
		{
			address->ipv4_address = BYTE_SWAP_LONG(in_addr.s_addr);
			address->port = port;
			address->address_length = k_ipv4_address_length;
			result = function_7af40(address);
		}
	}
	return result;
}

// @retail 0x7ab10
bool function_07ab10(long key_index, long local, word port, const XNADDR *xnaddr, s_type_99af70 *address)
{
	if (key_index == NONE)
	{
		for (long i = 0; i < 8; i++)
		{
			if (transport_security_get_address(i, local, xnaddr, port, address))
				return true;
		}
		return false;
	}
	return transport_security_get_address(key_index, local, xnaddr, port, address);
}
