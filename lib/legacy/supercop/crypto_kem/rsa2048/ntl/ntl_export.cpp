#include "ntl_export.h"

void ntl_export(unsigned char *s,unsigned long long slen,ZZ z)
{
  if (NumBytes(z) > slen) throw "ntl_export out of space";
  BytesFromZZ(s,z,slen);
}
