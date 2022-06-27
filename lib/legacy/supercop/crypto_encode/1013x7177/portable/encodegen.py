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

reading = 'R0'
top = 16384

print('/* auto-generated; do not edit */')
print('')
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
  print('  uint16 R[%d];' % ((Mlen+1)//2))

if Mlen > 2**29:
  print('  long long i;')
else:
  print('  long i;')

print('  uint16 r0,r1;')
print('  uint32 r2;')

def access(reading,pos):
  if reading == 'R0' and div3:
    return '(((%s[%s]%+d)&16383)*10923)>>15' % (reading,pos,offset)
  if reading == 'R0':
    return '(%s[%s]%+d)&16383' % (reading,pos,offset)
  return '%s[%s]' % (reading,pos)

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
