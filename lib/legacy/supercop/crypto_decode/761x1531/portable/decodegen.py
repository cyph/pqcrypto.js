#!/usr/bin/env python3

import sys

Mlen = 761
if len(sys.argv) > 1: Mlen = int(sys.argv[1])

m0 = 4591
if len(sys.argv) > 2: m0 = int(sys.argv[2])

m1 = m0
if len(sys.argv) > 3: m1 = int(sys.argv[3])
# M is Mlen-1 copies of m0, plus 1 copy of m1

offset = m0//2
if len(sys.argv) > 4: offset = int(sys.argv[4])

div3 = False
if len(sys.argv) > 5: div3 = sys.argv[5]=='True'

todo = 'R0'
top = 16384

print('/* auto-generated; do not edit */')
print('')
print('#include "crypto_decode.h"')
print('#include "crypto_int16.h"')
print('#include "crypto_uint16.h"')
print('#include "crypto_uint32.h"')
print('#include "crypto_uint64.h"')
print('#define int16 crypto_int16')
print('#define uint16 crypto_uint16')
print('#define uint32 crypto_uint32')
print('#define uint64 crypto_uint64')

print("""
/*
CPU division instruction typically takes time depending on x.
This software is designed to take time independent of x.
Time still varies depending on m; user must ensure that m is constant.
Time also varies on CPUs where multiplication is variable-time.
There could be more CPU issues.
There could also be compiler issues. 
*/

static void uint32_divmod_uint14(uint32 *q,uint16 *r,uint32 x,uint16 m)
{
  uint32 v = 0x80000000;
  uint32 qpart;
  uint32 mask;

  v /= m;

  /* caller guarantees m > 0 */
  /* caller guarantees m < 16384 */
  /* vm <= 2^31 <= vm+m-1 */
  /* xvm <= 2^31 x <= xvm+x(m-1) */

  *q = 0;

  qpart = (x*(uint64)v)>>31;
  /* 2^31 qpart <= xv <= 2^31 qpart + 2^31-1 */
  /* 2^31 qpart m <= xvm <= 2^31 qpart m + (2^31-1)m */
  /* 2^31 qpart m <= 2^31 x <= 2^31 qpart m + (2^31-1)m + x(m-1) */
  /* 0 <= 2^31 newx <= (2^31-1)m + x(m-1) */
  /* 0 <= newx <= (1-1/2^31)m + x(m-1)/2^31 */
  /* 0 <= newx <= (1-1/2^31)(2^14-1) + (2^32-1)((2^14-1)-1)/2^31 */

  x -= qpart*m; *q += qpart;
  /* x <= 49146 */

  qpart = (x*(uint64)v)>>31;
  /* 0 <= newx <= (1-1/2^31)m + x(m-1)/2^31 */
  /* 0 <= newx <= m + 49146(2^14-1)/2^31 */
  /* 0 <= newx <= m + 0.4 */
  /* 0 <= newx <= m */

  x -= qpart*m; *q += qpart;
  /* x <= m */

  x -= m; *q += 1;
  mask = -(x>>31);
  x += mask&(uint32)m; *q += mask;
  /* x < m */

  *r = x;
}

static uint16 uint32_mod_uint14(uint32 x,uint16 m)
{
  uint32 q;
  uint16 r;
  uint32_divmod_uint14(&q,&r,x,m);
  return r;
}
""")

print('void crypto_decode(void *v,const unsigned char *s)')
print('{')
print('  int16 *%s = v;' % todo)

tmparrays = None

layer = 1
x = Mlen
while x > 1:
  x = (x+1)//2
  if tmparrays == None:
    tmparrays = 'uint16 '
  else:
    tmparrays += ','
  tmparrays += 'R%d[%d]' % (layer,x)
  layer += 1

if tmparrays:
  print('  %s;' % tmparrays)

print('  long long i;')
print('  uint16 r0;')
print('  uint32 r1,r2;')

def poke(todo,pos,contents):
  if todo == 'R0' and div3:
    return '%s[%s] = 3*%s%+d;' % (todo,pos,contents,-offset)
  if todo == 'R0' and offset != 0:
    return '%s[%s] = %s%+d;' % (todo,pos,contents,-offset)
  return '%s[%s] = %s;' % (todo,pos,contents)

stanzas = []

def stanzaloop(looplen,reading,todo,m0,bytes):
  stanza = ''
  if looplen == 1:
    stanza += '  r2 = %s[0];\n' % reading
    for j in range(bytes):
      stanza += '  r2 = (r2<<8)|*--s;\n';
    stanza += '  uint32_divmod_uint14(&r1,&r0,r2,%d);\n' % m0
    stanza += '  %s\n' % poke(todo,'0','r0')
    stanza += '  r1 = uint32_mod_uint14(r1,%d); /* needed only for invalid inputs */\n' % m0
    stanza += '  %s\n' % poke(todo,'1','r1')
  if looplen > 1:
    stanza += '  for (i = %d;i >= 0;--i) {\n' % (looplen-1)
    stanza += '    r2 = %s[i];\n' % reading
    for j in range(bytes):
      stanza += '    r2 = (r2<<8)|*--s;\n';
    stanza += '    uint32_divmod_uint14(&r1,&r0,r2,%d);\n' % m0
    stanza += '    %s\n' % poke(todo,'2*i','r0')
    stanza += '    r1 = uint32_mod_uint14(r1,%d); /* needed only for invalid inputs */\n' % m0
    stanza += '    %s\n' % poke(todo,'2*i+1','r1')
    stanza += '  }\n'
  return stanza

layer = 1
reading = 'R%d' % layer
while Mlen > 1:
  n0 = m0*m0
  bytes0 = 0
  while n0 >= top:
    bytes0 += 1
    n0 = (n0+255)>>8

  if Mlen&1:
    looplen = Mlen//2
    r0 = '%s[%s]' % (reading,looplen)
    stanza = '  %s\n' % poke(todo,2*looplen,r0)
    stanza += stanzaloop(looplen,reading,todo,m0,bytes0)
    n1 = m1
    stanzas += [stanza]

  else:
    n1 = m0*m1
    bytes1 = 0
    while n1 >= top:
      bytes1 += 1
      n1 = (n1+255)>>8

    if m1 == m0:
      looplen = (Mlen+1)//2
      stanza = stanzaloop(looplen,reading,todo,m0,bytes0)
    else:
      looplen = (Mlen-1)//2

      stanza = ''
      stanza += '  r2 = %s[%s];\n' % (reading,looplen)
      for j in range(bytes1):
        stanza += '  r2 = (r2<<8)|*--s;\n';
      stanza += '  uint32_divmod_uint14(&r1,&r0,r2,%d);\n' % m0
      stanza += '  %s\n' % poke(todo,2*looplen,'r0')
      stanza += '  r1 = uint32_mod_uint14(r1,%d); /* needed only for invalid inputs */\n' % m1
      stanza += '  %s\n' % poke(todo,2*looplen+1,'r1')

      stanza += stanzaloop(looplen,reading,todo,m0,bytes0)

    stanzas += [stanza]

  m0,m1,Mlen = n0,n1,(Mlen+1)//2
  layer += 1
  todo = reading
  reading = 'R%d' % layer

stanza = ''
stanza += '  s += crypto_decode_STRBYTES;\n'
stanza += '  r1 = 0;\n'
m1save = m1
while m1 > 1:
  stanza += '  r1 = (r1<<8)|*--s;\n';
  m1 = (m1+255)>>8
stanza += '  r1 = uint32_mod_uint14(r1,%d); /* needed only for invalid inputs */\n' % m1save
stanza += '  %s\n' % poke(todo,0,'r1')
stanzas += [stanza]

for stanza in reversed(stanzas):
  print('  ')
  sys.stdout.write(stanza)

print('}')
