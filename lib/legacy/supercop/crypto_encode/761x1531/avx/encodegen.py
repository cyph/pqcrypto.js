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

round = False
if len(sys.argv) > 6: round = sys.argv[6]=='True'

reading = 'R0'
top = 16384

print('/* auto-generated; do not edit */')
print('')
print('#include <immintrin.h>')
print('#include "crypto_encode.h"')
print('#include "crypto_int16.h"')
print('#include "crypto_uint16.h"')
print('#include "crypto_uint32.h"')
print('#define int16 crypto_int16')
print('#define uint16 crypto_uint16')
print('#define uint32 crypto_uint32')
print('')

print('void crypto_encode(unsigned char *out,const void *v)')
print('{')
print('  const int16 *%s = v;' % reading)

if Mlen > 1:
  print('  /* XXX: caller could overlap R with input */')
  Rlen = (Mlen+1)//2
  print('  uint16 R[%d];' % Rlen)

if Mlen > 2**29:
  print('  long long i;')
else:
  print('  long i;')

print('  const uint16 *reading;')
print('  uint16 *writing;')
print('  uint16 r0,r1;')
print('  uint32 r2;')

def access(reading,pos):
  result = '%s[%s]' % (reading,pos)
  if reading == 'R0':
    if round:
      result = '3*((10923*%s+16384)>>15)'%result
    result = '((%s+%d)&16383)'%(result,offset)
    if div3:
      result = '(%s*10923)>>15'%result
  return result

def printloop(looplen,reading,todo,m0,bytes):
  if looplen <= 0: return
  if looplen == 1:
    print('  r0 = %s;' % access(reading,0))
    print('  r1 = %s;' % access(reading,1))
    print('  r2 = r0+r1*(uint32)%d;' % m0)
    for j in range(bytes):
      print('  *out++ = r2; r2 >>= 8;')
    print('  %s[0] = r2;' % todo)
    return

  if looplen >= 12 and bytes == 1:
    print('  reading = (uint16 *) %s;' % reading)
    print('  writing = %s;' % todo)
    print('  i = %d;' % ((looplen+7)//8))
    print('  while (i > 0) {')
    print('    __m256i x,y;')
    print('    --i;')
    if looplen%8:
      print('    if (!i) {')
      print('      reading -= %d;' % (2*(8-(looplen%8))))
      print('      writing -= %d;' % (8-(looplen%8)))
      print('      out -= %d;' % (8-(looplen%8)))
      print('    }')
    print('    x = _mm256_loadu_si256((__m256i *) reading);')
    if reading == 'R0' and round:
      print('    x = _mm256_mulhrs_epi16(x,_mm256_set1_epi16(10923));')
      if 0:
        print('    x = _mm256_mullo_epi16(x,_mm256_set1_epi16(3));')
      else:
        print('    x = _mm256_add_epi16(x,_mm256_add_epi16(x,x));')
    if reading == 'R0' and offset != 0:
      print('    x = _mm256_add_epi16(x,_mm256_set1_epi16(%d));' % offset)
    if reading == 'R0':
      print('    x &= _mm256_set1_epi16(16383);')
    if reading == 'R0' and div3:
      print('    x = _mm256_mulhi_epi16(x,_mm256_set1_epi16(21846));')

    print('    y = x & _mm256_set1_epi32(65535);')
    print('    x = _mm256_srli_epi32(x,16);')
    print('    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(%d));' % m0)
    print('    x = _mm256_add_epi32(y,x);')
    print('    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(')
    print('      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1,')
    print('      12,8,4,0,12,8,4,0,14,13,10,9,6,5,2,1')
    print('      ));')
    print('    x = _mm256_permute4x64_epi64(x,0xd8);')
    print('    _mm_storeu_si128((__m128i *) writing,_mm256_extractf128_si256(x,0));')
    print('    *((uint32 *) (out+0)) = _mm256_extract_epi32(x,4);')
    print('    *((uint32 *) (out+4)) = _mm256_extract_epi32(x,6);')
    print('    reading += 16;')
    print('    writing += 8;')
    print('    out += 8;')
    print('  }')
    return

  if looplen >= 24 and bytes == 2:
    print('  reading = (uint16 *) %s;' % reading)
    print('  writing = %s;' % todo)
    print('  i = %d;' % ((looplen+15)//16))
    print('  while (i > 0) {')
    print('    __m256i x,x2,y,y2;')
    print('    --i;')
    if looplen%16:
      print('    if (!i) {')
      print('      reading -= %d;' % (2*(16-(looplen%16))))
      print('      writing -= %d;' % (16-(looplen%16)))
      print('      out -= %d;' % (2*(16-(looplen%16))))
      print('    }')
    print('    x = _mm256_loadu_si256((__m256i *) (reading+0));')
    print('    x2 = _mm256_loadu_si256((__m256i *) (reading+16));')
    if reading == 'R0' and round:
      print('    x = _mm256_mulhrs_epi16(x,_mm256_set1_epi16(10923));')
      print('    x2 = _mm256_mulhrs_epi16(x2,_mm256_set1_epi16(10923));')
      if 0:
        print('    x = _mm256_mullo_epi16(x,_mm256_set1_epi16(3));')
        print('    x2 = _mm256_mullo_epi16(x2,_mm256_set1_epi16(3));')
      else:
        print('    x = _mm256_add_epi16(x,_mm256_add_epi16(x,x));')
        print('    x2 = _mm256_add_epi16(x2,_mm256_add_epi16(x2,x2));')
    if reading == 'R0' and offset != 0:
      print('    x = _mm256_add_epi16(x,_mm256_set1_epi16(%d));' % offset)
      print('    x2 = _mm256_add_epi16(x2,_mm256_set1_epi16(%d));' % offset)
    if reading == 'R0':
      print('    x &= _mm256_set1_epi16(16383);')
      print('    x2 &= _mm256_set1_epi16(16383);')
    if reading == 'R0' and div3:
      print('    x = _mm256_mulhi_epi16(x,_mm256_set1_epi16(21846));')
      print('    x2 = _mm256_mulhi_epi16(x2,_mm256_set1_epi16(21846));')

    print('    y = x & _mm256_set1_epi32(65535);')
    print('    y2 = x2 & _mm256_set1_epi32(65535);')
    print('    x = _mm256_srli_epi32(x,16);')
    print('    x2 = _mm256_srli_epi32(x2,16);')
    print('    x = _mm256_mullo_epi32(x,_mm256_set1_epi32(%d));' % m0)
    print('    x2 = _mm256_mullo_epi32(x2,_mm256_set1_epi32(%d));' % m0)
    print('    x = _mm256_add_epi32(y,x);')
    print('    x2 = _mm256_add_epi32(y2,x2);')
    print('    x = _mm256_shuffle_epi8(x,_mm256_set_epi8(')
    print('      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,')
    print('      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0')
    print('      ));')
    print('    x2 = _mm256_shuffle_epi8(x2,_mm256_set_epi8(')
    print('      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,')
    print('      15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0')
    print('      ));')
    print('    x = _mm256_permute4x64_epi64(x,0xd8);')
    print('    x2 = _mm256_permute4x64_epi64(x2,0xd8);')
    print('    _mm256_storeu_si256((__m256i *) writing,_mm256_permute2f128_si256(x,x2,0x31));')
    print('    _mm256_storeu_si256((__m256i *) out,_mm256_permute2f128_si256(x,x2,0x20));')
    print('    reading += 32;')
    print('    writing += 16;')
    print('    out += 32;')
    print('  }')
    return

  print('  for (i = 0;i < %d;++i) {' % looplen)
  print('    r0 = %s;' % access(reading,'2*i'))
  print('    r1 = %s;' % access(reading,'2*i+1'))
  print('    r2 = r0+r1*(uint32)%d;' % m0)
  for j in range(bytes):
    print('    *out++ = r2; r2 >>= 8;')
  print('    %s[i] = r2;' % todo)
  print('  }')

todo = 'R'
while Mlen > 1:
  print('  ')

  n0 = m0*m0
  bytes0 = 0
  while n0 >= top:
    bytes0 += 1
    n0 = (n0+255)>>8

  if Mlen&1:
    looplen = Mlen//2
    printloop(looplen,reading,todo,m0,bytes0)
    r0 = access(reading,2*looplen)
    print('  %s[%d] = %s;' % (todo,looplen,r0))
    n1 = m1

  else:
    n1 = m0*m1
    bytes1 = 0
    while n1 >= top:
      bytes1 += 1
      n1 = (n1+255)>>8

    if bytes1 == bytes0:
      looplen = (Mlen+1)//2
      printloop(looplen,reading,todo,m0,bytes0)
    else:
      looplen = (Mlen-1)//2
      printloop(looplen,reading,todo,m0,bytes0)
      print('  r0 = %s;' % access(reading,2*looplen))
      print('  r1 = %s;' % access(reading,2*looplen+1))
      print('  r2 = r0+r1*(uint32)%d;' % m0)
      for j in range(bytes1):
        print('  *out++ = r2; r2 >>= 8;')
      print('  %s[%d] = r2;' % (todo,looplen))

  m0,m1,Mlen = n0,n1,(Mlen+1)//2
  reading = todo

print('  ')
print('  r0 = %s;' % access(reading,0))
while m1 > 1:
  print('  *out++ = r0; r0 >>= 8;')
  m1 = (m1+255)>>8

print('}')
