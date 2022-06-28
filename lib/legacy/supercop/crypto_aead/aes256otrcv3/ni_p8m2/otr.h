/* Copyright (c) 2014, NEC Corporation. All rights reserved.
*
* LICENSE
*
* 1. NEC Corporation ("NEC") hereby grants users to use and reproduce
*AES-OTR program ("Software") for testing and evaluation purpose for
*CAESAR (Competition for Authenticated Encryption: Security, Applicability,
*and Robustness). The users must not use the Software for any other purpose
*or distribute it to any third party.
*
* 2. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL NEC
*BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL
*DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OF THE
*SOFTWARE.
*
* 3. In the event of an user's failure to comply with any term of this License
*or wrongful act, NEC may terminate such user's use of the Software. Upon such
*termination the user must cease all use of the Software and destroy all copies
*of the Software.
*
* 4. The users shall comply with all applicable laws and regulations, including
*export and import control laws, which govern the usage of the Software.
*/
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#if __GNUC__
#define ALIGN(n) __attribute__ ((aligned(n)))
#elif _MSC_VER
#define ALIGN(n) __declspec(align(n))
#define __inline__ __inline
#else 
#define ALIGN(n)
#endif

typedef unsigned char	uint8;
typedef unsigned int	uint32;
typedef ALIGN(16)__m128i block;

#define BLOCK 16
#define DBLOCK 32
#define TAG_MATCH	 0
#define TAG_UNMATCH	-1
#define AUTHF 1
#define DECF 0

#define PIPE 8 // 4 to 8

//#define DOUBLING_TABLE //use doubling.h

#define SUCCESS 0
