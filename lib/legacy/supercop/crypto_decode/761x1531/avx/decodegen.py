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

todo = 'R0'
top = 16384

print('/* auto-generated; do not edit */')
print('')
print('#include <immintrin.h>')
print('#include "crypto_decode.h"')
print('#include "crypto_int16.h"')
print('#include "crypto_int32.h"')
print('#define int16 crypto_int16')
print('#define int32 crypto_int32')

print("""
static inline int16 mullo(int16 x,int16 y)
{
  return x*y;
}

static inline int16 mulhi(int16 x,int16 y)
{
  return (x*(int32)y)>>16;
}

static inline __m256i add(__m256i x,__m256i y)
{
  return _mm256_add_epi16(x,y);
}

static inline __m256i sub(__m256i x,__m256i y)
{
  return _mm256_sub_epi16(x,y);
}

static inline __m256i shiftleftconst(__m256i x,int16 y)
{
  return _mm256_slli_epi16(x,y);
}

static inline __m256i signedshiftrightconst(__m256i x,int16 y)
{
  return _mm256_srai_epi16(x,y);
}

static inline __m256i addconst(__m256i x,int16 y)
{
  return add(x,_mm256_set1_epi16(y));
}

static inline __m256i subconst(__m256i x,int16 y)
{
  return sub(x,_mm256_set1_epi16(y));
}

static inline __m256i mulloconst(__m256i x,int16 y)
{
  return _mm256_mullo_epi16(x,_mm256_set1_epi16(y));
}

static inline __m256i mulhiconst(__m256i x,int16 y)
{
  return _mm256_mulhi_epi16(x,_mm256_set1_epi16(y));
}

static inline __m256i ifgesubconst(__m256i x,int16 y)
{
  __m256i y16 = _mm256_set1_epi16(y);
  __m256i top16 = _mm256_set1_epi16(y-1);
  return sub(x,_mm256_cmpgt_epi16(x,top16) & y16);
}

static inline __m256i ifnegaddconst(__m256i x,int16 y)
{
  return add(x,signedshiftrightconst(x,15) & _mm256_set1_epi16(y));
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
    tmparrays = 'int16 '
  else:
    tmparrays += ','
  tmparrays += 'R%d[%d]' % (layer,x)
  layer += 1

if tmparrays:
  print('  %s;' % tmparrays)

print('  long long i;')
print('  int16 a0,a1,a2;')
print('  __m256i A0,A1,A2,S0,S1,B0,B1,C0,C1;')

def poke(todo,pos,contents):
  if todo == 'R0' and div3:
    return '%s[%s] = 3*%s%+d;' % (todo,pos,contents,-offset)
  if todo == 'R0' and offset != 0:
    return '%s[%s] = %s%+d;' % (todo,pos,contents,-offset)
  return '%s[%s] = %s;' % (todo,pos,contents)

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

def inner(indent,reading,inpos,m0,m1,bytes,outpos0,outpos1):
  stanza = ''

  y,t,u,uinv,z = mulmoddata(256,m0)

  stanza += indent + 'a2 = a0 = %s[%s];\n' % (reading,inpos)
  a0lower,a0upper = 0,1<<14

  for loop in range(bytes):
    if y > 0:
      a0lower,a0upper = a0lower*y,a0upper*y
    else:
      a0lower,a0upper = a0upper*y,a0lower*y
    a0lower,a0upper = a0lower-(m0<<15),a0upper+(m0<<15)
    a0lower,a0upper = a0lower>>16,a0upper>>16
    stanza += indent + 'a0 = mulhi(a0,%d)-mulhi(mullo(a0,%d),%d); /* %d...%d */\n' % (y,z,m0,a0lower,a0upper)
  
    a0upper += 255
    stanza += indent + 'a0 += s[%d*i+%d]; /* %d...%d */\n' % (bytes,bytes-1-loop,a0lower,a0upper)

  if a0upper >= 2*m0:
    y1,t1,u1,uinv1,z1 = mulmoddata(1,m0)
    if y1 > 0:
      a0lower,a0upper = a0lower*y1,a0upper*y1
    else:
      a0lower,a0upper = a0upper*y1,a0lower*y1
    a0lower,a0upper = a0lower-(m0<<15),a0upper+(m0<<15)
    a0lower,a0upper = a0lower>>16,a0upper>>16
    stanza += indent + 'a0 = mulhi(a0,%d)-mulhi(mullo(a0,%d),%d); /* %d...%d */\n' % (y1,z1,m0,a0lower,a0upper)

  while a0upper >= m0:
    a0lower,a0upper = a0lower-m0,a0upper-m0
    stanza += indent + 'a0 -= %d; /* %d..>%d */\n' % (m0,a0lower,a0upper)

  while a0lower < 0:
    a0lower,a0upper = min(0,a0lower+m0),max(m0-1,a0upper)
    stanza += indent + 'a0 += (a0>>15)&%d; /* %d...%d */\n' % (m0,a0lower,a0upper)

  if bytes == 0:
    stanza += indent + 'a1 = (a2-a0)>>%d;\n' % t

  elif bytes == 1:
    if t == 0:
      stanza += indent + 'a1 = (a2<<8)+s[i]-a0;\n'
    elif t == 8:
      stanza += indent + 'a1 = a2+((s[i]-a0)>>8);\n'
    elif t < 8:
      stanza += indent + 'a1 = (a2<<%d)+((s[i]-a0)>>%d);\n' % (8-t,t)
    else:
      stanza += indent + 'a1 = (a2+((s[i]-a0)>>8))>>%d;\n' % t-8

  else:
    assert bytes == 2
    if t == 0:
      stanza += indent + 'a1 = (s[2*i+1]<<8)+s[2*i]-a0;\n'
    elif t == 8:
      stanza += indent + 'a1 = (a2<<8)+s[2*i+1]+((s[2*i]-a0)>>8);\n'
    elif t < 8:
      stanza += indent + 'a1 = (a2<<%d)+(s[2*i+1]<<%d)+((s[2*i]-a0)>>%d);\n' % (16-t,8-t,t)
    else:
      stanza += indent + 'a1 = ((((int32)a2)<<16)+(s[2*i+1]<<8)+s[2*i]-a0)>>%d;\n' % t

  stanza += indent + 'a1 = mullo(a1,%d);\n' % uinv
  stanza += '\n'
  stanza += indent + '/* invalid inputs might need reduction mod %d */\n' % m1
  stanza += indent + 'a1 -= %d;\n' % m1
  stanza += indent + 'a1 += (a1>>15)&%d;\n' % m1
  stanza += '\n'
  stanza += indent + '%s\n' % poke(todo,outpos0,'a0')
  stanza += indent + '%s\n' % poke(todo,outpos1,'a1')

  return stanza

# XXX: caller must ensure that outpos1 is outpos0+1
def inner16(indent,reading,inpos,m0,m1,bytes,outpos0,outpos1):
  stanza = ''

  y,t,u,uinv,z = mulmoddata(256,m0)

  stanza += indent + 'A2 = A0 = _mm256_loadu_si256((__m256i *) &%s[%s]);\n' % (reading,inpos)
  a0lower,a0upper = 0,1<<14

  if bytes == 1:
    stanza += indent + 'S0 = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i *) (s+i)));\n'

  if bytes == 2:
    stanza += indent + 'S0 = _mm256_loadu_si256((__m256i *) (s+2*i));\n'
    stanza += indent + 'S1 = _mm256_srli_epi16(S0,8);\n'
    stanza += indent + 'S0 &= _mm256_set1_epi16(255);\n'

  for loop in reversed(range(bytes)):
    if y > 0:
      a0lower,a0upper = a0lower*y,a0upper*y
    else:
      a0lower,a0upper = a0upper*y,a0lower*y
    a0lower,a0upper = a0lower-(m0<<15),a0upper+(m0<<15)
    a0lower,a0upper = a0lower>>16,a0upper>>16
    stanza += indent + 'A0 = sub(mulhiconst(A0,%d),mulhiconst(mulloconst(A0,%d),%d)); /* %d...%d */\n' % (y,z,m0,a0lower,a0upper)
  
    a0upper += 255
    stanza += indent + 'A0 = add(A0,S%d); /* %d...%d */\n' % (loop,a0lower,a0upper)

  if a0upper >= 2*m0:
    y1,t1,u1,uinv1,z1 = mulmoddata(1,m0)
    if y1 > 0:
      a0lower,a0upper = a0lower*y1,a0upper*y1
    else:
      a0lower,a0upper = a0upper*y1,a0lower*y1
    a0lower,a0upper = a0lower-(m0<<15),a0upper+(m0<<15)
    a0lower,a0upper = a0lower>>16,a0upper>>16
    stanza += indent + 'A0 = sub(mulhiconst(A0,%d),mulhiconst(mulloconst(A0,%d),%d)); /* %d...%d */\n' % (y1,z1,m0,a0lower,a0upper)

  while a0upper >= m0:
    a0lower,a0upper = a0lower-m0,a0upper-m0
    stanza += indent + 'A0 = subconst(A0,%d); /* %d...%d */\n' % (m0,a0lower,a0upper)

  while a0lower < 0:
    a0lower,a0upper = min(0,a0lower+m0),max(m0-1,a0upper)
    stanza += indent + 'A0 = ifnegaddconst(A0,%d); /* %d...%d */\n' % (m0,a0lower,a0upper)

  if bytes == 0:
    stanza += indent + 'A1 = signedshiftrightconst(sub(A2,A0),%d);\n' % t

  elif bytes == 1:
    if t == 0:
      stanza += indent + 'A1 = add(shiftleftconst(A2,8),sub(S0,A0));\n'
    elif t == 8:
      stanza += indent + 'A1 = add(A2,signedshiftrightconst(sub(S0,A0),8));\n'
    elif t < 8:
      stanza += indent + 'A1 = add(shiftleftconst(A2,%d),signedshiftrightconst(sub(S0,A0),%d));\n' % (8-t,t)
    else:
      raise Exception('shift distances above 8 unimplemented')

  else:
    assert bytes == 2
    if t == 0:
      stanza += indent + 'A1 = add(shiftleftconst(S1,8),sub(S0,A0));\n'
    elif t == 8:
      stanza += indent + 'A1 = add(add(shiftleftconst(A2,8),S1),signedshiftrightconst(sub(S0,A0),8));\n'
    elif t < 8:
      stanza += indent + 'A1 = add(add(shiftleftconst(A2,%d),shiftleftconst(S1,%d)),signedshiftrightconst(sub(S0,A0),%d));\n' % (16-t,8-t,t)
    else:
      raise Exception('shift distances above 8 unimplemented')

  stanza += indent + 'A1 = mulloconst(A1,%d);\n' % uinv
  stanza += '\n'
  stanza += indent + '/* invalid inputs might need reduction mod %d */\n' % m1
  # stanza += indent + 'A1 = subconst(A1,%d);\n' % m1
  # stanza += indent + 'A1 = ifnegaddconst(A1,%d);\n' % m1
  stanza += indent + 'A1 = ifgesubconst(A1,%d);\n' % m1
  stanza += '\n'

  if todo == 'R0' and div3:
    stanza += indent + 'A0 = mulloconst(A0,3);\n'
    stanza += indent + 'A1 = mulloconst(A1,3);\n'
  if todo == 'R0' and offset != 0:
    stanza += indent + 'A0 = subconst(A0,%d);\n' % offset
    stanza += indent + 'A1 = subconst(A1,%d);\n' % offset

  stanza += indent + '/* A0: r0r2r4r6r8r10r12r14 r16r18r20r22r24r26r28r30 */\n'
  stanza += indent + '/* A1: r1r3r5r7r9r11r13r15 r17r19r21r23r25r27r29r31 */\n'
  
  stanza += indent + 'B0 = _mm256_unpacklo_epi16(A0,A1);\n'
  stanza += indent + 'B1 = _mm256_unpackhi_epi16(A0,A1);\n'
  stanza += indent + '/* B0: r0r1r2r3r4r5r6r7 r16r17r18r19r20r21r22r23 */\n'
  stanza += indent + '/* B1: r8r9r10r11r12r13r14r15 r24r25r26r27r28r29r30r31 */\n'
  
  stanza += indent + 'C0 = _mm256_permute2x128_si256(B0,B1,0x20);\n'
  stanza += indent + 'C1 = _mm256_permute2x128_si256(B0,B1,0x31);\n'
  stanza += indent + '/* C0: r0r1r2r3r4r5r6r7 r8r9r10r11r12r13r14r15 */\n'
  stanza += indent + '/* C1: r16r17r18r19r20r21r22r23 r24r25r26r27r28r29r30r31 */\n'
  
  stanza += indent + '_mm256_storeu_si256((__m256i *) (&%s[%s]),C0);\n' % (todo,outpos0)
  stanza += indent + '_mm256_storeu_si256((__m256i *) (16+&%s[%s]),C1);\n' % (todo,outpos0)

  return stanza

def stanzaloop(looplen,reading,todo,m0,bytes):
  stanza = ''
  stanza += '  s -= %d;\n' % (bytes*looplen)
  if looplen < 1: return stanza
  if looplen % 16 == 0:
    stanza += '  for (i = %d;i >= 0;i -= 16) {\n' % (looplen-16)
    stanza += inner16('    ',reading,'i',m0,m0,bytes,'2*i','2*i+1')
    stanza += '  }\n'
    return stanza
  if looplen >= 16:
    stanza += '  i = %d;\n' % (looplen-16)
    stanza += '  for (;;) {\n'
    stanza += inner16('    ',reading,'i',m0,m0,bytes,'2*i','2*i+1')
    stanza += '    if (!i) break;\n'
    stanza += '    i = -16-((~15)&-i);\n'
    stanza += '  }\n'
    return stanza
  stanza += '  for (i = %d;i >= 0;--i) {\n' % (looplen-1)
  stanza += inner('    ',reading,'i',m0,m0,bytes,'2*i','2*i+1')
  stanza += '  }\n'
  return stanza

stanzas = []

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
      stanza = '  i = 0;\n'
      stanza += '  s -= %d;\n' % bytes1
      stanza += inner('  ',reading,looplen,m0,m1,bytes1,2*looplen,2*looplen+1)
      stanza += stanzaloop(looplen,reading,todo,m0,bytes0)

    stanzas += [stanza]

  if m0 == m1:
    stanzas += ['  /* %s ------> %s: reconstruct mod %d*[%d] */\n' % (reading,todo,Mlen,m0)]
  else:
    stanzas += ['  /* %s ------> %s: reconstruct mod %d*[%d]+[%d] */\n' % (reading,todo,Mlen-1,m0,m1)]

  m0,m1,Mlen = n0,n1,(Mlen+1)//2
  layer += 1
  todo = reading
  reading = 'R%d' % layer

stanza = ''
stanza += '  s += crypto_decode_STRBYTES;\n'

stanza += '  a1 = 0;\n'
q = m1
y,t,u,uinv,z = mulmoddata(256,m1)
a1lower = 0
a1upper = 0

while m1 > 1:
  if m1 != q:
    stanza += '  a1 = mulhi(a1,%d)-mulhi(mullo(a1,%d),%d);\n' % (y,z,q)
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
  
stanza += '  %s\n' % poke(todo,0,'a1')
stanzas += [stanza]

for stanza in reversed(stanzas):
  print('  ')
  sys.stdout.write(stanza)

print('}')
