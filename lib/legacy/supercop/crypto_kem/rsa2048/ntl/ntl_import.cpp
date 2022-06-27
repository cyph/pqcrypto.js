#include "ntl_import.h"

ZZ ntl_import(const unsigned char *s,unsigned long long slen)
{
  return ZZFromBytes(s,slen);
}
