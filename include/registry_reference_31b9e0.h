#ifndef REGISTRY_REFERENCE_31B9E0_H
#define REGISTRY_REFERENCE_31B9E0_H
#include "unknown_0a9f20.h"
#include "reference_name_registry.h"

class __declspec(novtable) registry_reference_31b9e0 : public c_reference_a9f20
{
public:
 virtual unsigned int v1(char *buffer, unsigned int size)
 {
  if (flags & 0x8000)
   return g_48011c->copy_name((c_a const *)this, buffer, size);
  return 0;
 }
 virtual void v2(char const *name)
 {
  g_48011c->set_name((c_a const *)this, name);
  if (name) flags |= 0x8000;
  else flags &= 0x7fff;
 }
 virtual void *v3() { return &g_480120; }
 virtual void v4(dword unused) {}
};
#endif
