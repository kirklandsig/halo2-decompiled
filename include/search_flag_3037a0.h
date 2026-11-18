#ifndef SEARCH_FLAG_3037A0_H
#define SEARCH_FLAG_3037A0_H
// Havok: the flag 0x3037a0 takes for whether the triangle's own plane is
// tested first. Retail passes it as a small object built at the call (its
// callers construct it in place), not as a plain bool.
struct search_flag_3037a0
{
	search_flag_3037a0(bool value_) : value(value_) {}
	search_flag_3037a0(search_flag_3037a0 const &other) : value(other.value) {}
	bool value;
};
#endif
