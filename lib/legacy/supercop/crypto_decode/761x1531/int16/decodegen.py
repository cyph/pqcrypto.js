#!/usr/bin/env python3

import sys
from math import floor

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

top = 16384

print('/* auto-generated; do not edit */')
print('')
print('#include "crypto_decode.h"')
print('#include "crypto_int16.h"')
print('#include "crypto_int32.h"')
print('#define int16 crypto_int16')
print('#define int32 crypto_int32')

print("""
static int16 mullo(int16 x,int16 y)
{
  return x*y;
}

static int16 mulhi(int16 x,int16 y)
{
  return (x*(int32)y)>>16;
}
""")

print('void crypto_decode(void *v,const unsigned char *s)')
print('{')
print('  int16 *R = v;')

print('  long long i;')
print('  int16 a0,a1,ri,lo,hi,s0,s1;')

def poke(layer,pos,contents):
  if layer == 1 and div3:
    return 'R[%s] = 3*%s%+d;' % (pos,contents,-offset)
  if layer == 1 and offset != 0:
    return 'R[%s] = %s%+d;' % (pos,contents,-offset)
  return 'R[%s] = %s;' % (pos,contents)

def mulmoddata(c,q):
  y = (c<<16)%q
  if y*2 >= q: y -= q

  t = 0
  u = q
  while not u&1:
    t += 1
    u >>= 1

  uinv = pow(u,16383,65536)
  if uinv>=32768: uinv -= 65536
  assert (u*uinv)%65536 == 1

  z = (y-(c<<16))>>t
  z = (z*uinv)%65536
  if z >= 32768: z -= 65536
  assert (z*u-((y-(c<<16))>>t))%(2**16) == 0

  return y,t,u,uinv,z

def inner(indent,inpos,m0,m1,bytes,outpos0,outpos1):
  stanza = ''

  y,t,u,uinv,z = mulmoddata(256,m0)

  stanza += indent + 'ri = R[%s];\n' % (inpos)
  a0lower,a0upper = 0,1<<14

  for loop in reversed(range(bytes)):
    stanza += indent + 's%d = *--s;\n' % loop

  for loop in reversed(range(bytes)):
    if y > 0:
      a0lower,a0upper = a0lower*y,a0upper*y
    else:
      a0lower,a0upper = a0upper*y,a0lower*y
    a0lower,a0upper = a0lower-(m0<<15),a0upper+(m0<<15)
    a0lower,a0upper = a0lower>>16,a0upper>>16
    if loop == bytes-1:
      stanza += indent + 'lo = mullo(ri,%d);\n' % z
      stanza += indent + 'a0 = mulhi(ri,%d)-mulhi(lo,%d); /* %d...%d */\n' % (y,m0,a0lower,a0upper)
    else:
      stanza += indent + 'lo = mullo(a0,%d);\n' % z
      stanza += indent + 'a0 = mulhi(a0,%d)-mulhi(lo,%d); /* %d...%d */\n' % (y,m0,a0lower,a0upper)
  
    a0upper += 255
    stanza += indent + 'a0 += s%d; /* %d...%d */\n' % (loop,a0lower,a0upper)

  if bytes == 0:
    stanza += indent + 'a0 = ri;\n'

  if a0upper >= 2*m0:
    y1,t1,u1,uinv1,z1 = mulmoddata(1,m0)
    if y1 > 0:
      a0lower,a0upper = a0lower*y1,a0upper*y1
    else:
      a0lower,a0upper = a0upper*y1,a0lower*y1
    a0lower,a0upper = a0lower-(m0<<15),a0upper+(m0<<15)
    a0lower,a0upper = a0lower>>16,a0upper>>16
    if bytes == 0:
      stanza += indent + 'lo = mullo(a0,%d);\n' % z1
    else:
      stanza += indent + 'lo = mullo(a0,%d);\n' % z1
    stanza += indent + 'a0 = mulhi(a0,%d)-mulhi(lo,%d); /* %d...%d */\n' % (y1,m0,a0lower,a0upper)

  while a0upper >= m0:
    a0lower,a0upper = a0lower-m0,a0upper-m0
    stanza += indent + 'a0 -= %d; /* %d..>%d */\n' % (m0,a0lower,a0upper)

  while a0lower < 0:
    a0lower,a0upper = min(0,a0lower+m0),max(m0-1,a0upper)
    stanza += indent + 'a0 += (a0>>15)&%d; /* %d...%d */\n' % (m0,a0lower,a0upper)

  if bytes == 0:
    stanza += indent + 'a1 = (ri-a0)>>%d;\n' % t

  elif bytes == 1:
    if t == 0:
      stanza += indent + 'a1 = (ri<<8)+s0-a0;\n'
    elif t == 8:
      stanza += indent + 'a1 = ri+((s0-a0)>>8);\n'
    elif t < 8:
      stanza += indent + 'a1 = (ri<<%d)+((s0-a0)>>%d);\n' % (8-t,t)
    else:
      stanza += indent + 'a1 = (ri+((s0-a0)>>8))>>%d;\n' % t-8

  else:
    assert bytes == 2
    if t == 0:
      stanza += indent + 'a1 = (s1<<8)+s0-a0;\n'
    elif t == 8:
      stanza += indent + 'a1 = (ri<<8)+s1+((s0-a0)>>8);\n'
    elif t < 8:
      stanza += indent + 'a1 = (ri<<%d)+(s1<<%d)+((s0-a0)>>%d);\n' % (16-t,8-t,t)
    else:
      stanza += indent + 'a1 = ((((int32)ri)<<16)+(s1<<8)+s0-a0)>>%d;\n' % t

  stanza += indent + 'a1 = mullo(a1,%d);\n' % uinv
  stanza += '\n'
  stanza += indent + '/* invalid inputs might need reduction mod %d */\n' % m1
  stanza += indent + 'a1 -= %d;\n' % m1
  stanza += indent + 'a1 += (a1>>15)&%d;\n' % m1
  stanza += '\n'
  stanza += indent + '%s\n' % poke(layer,outpos0,'a0')
  stanza += indent + '%s\n' % poke(layer,outpos1,'a1')

  return stanza

def stanzaloop(looplen,layer,m0,bytes):
  stanza = ''
  if looplen >= 1:
    stanza += '  for (i = %d;i >= 0;--i) {\n' % (looplen-1)
    stanza += inner('    ','i',m0,m0,bytes,'2*i','2*i+1')
    stanza += '  }\n'
  return stanza

stanzas = []

layer = 1
while Mlen > 1:
  n0 = m0*m0
  bytes0 = 0
  while n0 >= top:
    bytes0 += 1
    n0 = (n0+255)>>8

  if Mlen&1:
    looplen = Mlen//2
    r0 = 'R[%s]' % (looplen)
    stanza = '  %s\n' % poke(layer,2*looplen,r0)
    stanza += stanzaloop(looplen,layer,m0,bytes0)
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
      stanza = stanzaloop(looplen,layer,m0,bytes0)
    else:
      looplen = (Mlen-1)//2
      stanza = inner('  ',looplen,m0,m1,bytes1,2*looplen,2*looplen+1)
      stanza += stanzaloop(looplen,layer,m0,bytes0)

    stanzas += [stanza]

  if m0 == m1:
    stanzas += ['  /* reconstruct mod %d*[%d] */\n' % (Mlen,m0)]
  else:
    stanzas += ['  /* reconstruct mod %d*[%d]+[%d] */\n' % (Mlen-1,m0,m1)]

  m0,m1,Mlen = n0,n1,(Mlen+1)//2
  layer += 1

stanza = ''
stanza += '  s += crypto_decode_STRBYTES;\n'

stanza += '  a1 = 0;\n'
q = m1
y,t,u,uinv,z = mulmoddata(256,m1)
a1lower = 0
a1upper = 0

while m1 > 1:
  if m1 != q:
    stanza += '  lo = mullo(a1,%d);\n' % z
    stanza += '  a1 = mulhi(a1,%d)-mulhi(lo,%d);\n' % (y,q)
    if y > 0:
      a1lower,a1upper = a1lower*y,a1upper*y
      a1lower,a1upper = a1lower-(q<<15),a1upper+(q<<15)-1
    else:
      a1lower,a1upper = a1upper*y,a1lower*y
      a1lower,a1upper = a1upper-(q<<15),a1lower+(q<<15)-1
    a1lower >>= 16
    a1upper >>= 16

  a1upper += 255
  stanza += '  a1 += *--s; /* %d...%d */\n' % (a1lower,a1upper)

  m1 = (m1+255)>>8

while a1upper >= q:
  a1lower,a1upper = a1lower-q,a1upper-q
  stanza += '  a1 -= %d; /* %d...%d */\n' % (q,a1lower,a1upper)

while a1lower < 0:
  a1lower,a1upper = min(0,a1lower+q),max(a1upper,q-1)
  stanza += '  a1 += (a1>>15)&%d; /* %d...%d */\n' % (q,a1lower,a1upper)
  
stanza += '  %s\n' % poke(layer,0,'a1')
stanzas += [stanza]

for stanza in reversed(stanzas):
  print('  ')
  sys.stdout.write(stanza)

print('}')
