//  sneik_f512_avr_small.S
//  2019-02-14  Markku-Juhani O. Saarinen <mjos@pqshield.com>
//  Copyright (C) 2019, PQShield Ltd. Please see LICENSE.

//  Smaller 8-bit Atmel AVR implementation for the SNEIK v1.1 f512 function.

//  Prototype:
//      void sneik_f512(void *s, uint8_t dom, uint8_t rounds);

__zero_reg__ = 1

        .text

// load 4 bytes to 4 registers via y

.macro  ldy4    rr
        ld      \rr,        y+
        ld      \rr + 1,    y+
        ld      \rr + 2,    y+
        ld      \rr + 3,    y+
        .endm

// load 4 bytes to 4 registers via z

.macro  ldzv4   rr, rv
        ldd     \rr,        z + (4 * \rv)
        ldd     \rr + 1,    z + (4 * \rv + 1)
        ldd     \rr + 2,    z + (4 * \rv + 2)
        ldd     \rr + 3,    z + (4 * \rv + 3)
        .endm

// mixing function. macro loads v1

.macro  mix_f   t0, t1, t2, t3, tmp

        //      inner feedback left
        add     \t0,        \t3         //  t0 += t3;
        adc     \t0 + 1,    \t3 + 1
        adc     \t0 + 2,    \t3 + 2
        adc     \t0 + 3,    \t3 + 3

        //      p = x^25 + x^24 + 1
        mov     \tmp + 3,   \t0         //  tmp = t0 <<< 24
        mov     \tmp,       \t0 + 1
        mov     \tmp + 1,   \t0 + 2
        mov     \tmp + 2,   \t0 + 3
        eor     \t0,        \tmp        //  t0 = t0 ^ (t0 <<< 24)
        eor     \t0 + 1,    \tmp + 1
        eor     \t0 + 2,    \tmp + 2
        eor     \t0 + 3,    \tmp + 3
        lsl     \tmp                    //  tmp = t0 <<< 25
        rol     \tmp + 1
        rol     \tmp + 2
        rol     \tmp + 3
        adc     \tmp,   __zero_reg__
        eor     \t0,        \tmp        //  t0 ^ (t0 <<< 24) ^ (t0 <<< 25)
        eor     \t0 + 1,    \tmp + 1
        eor     \t0 + 2,    \tmp + 2
        eor     \t0 + 3,    \tmp + 3

        lsl     \t2                     //  t2 = t2 <<< 1
        rol     \t2 + 1
        rol     \t2 + 2
        rol     \t2 + 3
        adc     \t2,   __zero_reg__

        //      outer feedback
        eor     \t0,        \t2         //  t0 ^= t2;
        eor     \t0 + 1,    \t2 + 1
        eor     \t0 + 2,    \t2 + 2
        eor     \t0 + 3,    \t2 + 3

        ldy4    \t2                     //  vec[(pos + 2) & 0xF];

        add     \t0,        \t2         //  t0 += t2;
        adc     \t0 + 1,    \t2 + 1
        adc     \t0 + 2,    \t2 + 2
        adc     \t0 + 3,    \t2 + 3

        //      p = x^17 + x^9 + 1
        mov     \tmp + 1,   \t0         //  tmp = t0 <<< 8
        mov     \tmp + 2,   \t0 + 1
        mov     \tmp + 3,   \t0 + 2
        mov     \tmp,       \t0 + 3
        eor     \tmp + 2,   \t0         //  tmp = (t0 <<< 8) ^ (t0 <<< 16)
        eor     \tmp + 3,   \t0 + 1
        eor     \tmp,       \t0 + 2
        eor     \tmp + 1,   \t0 + 3
        lsl     \tmp                    //  tmp = (t0 <<< 9) ^ (t0 <<< 17)
        rol     \tmp + 1
        rol     \tmp + 2
        rol     \tmp + 3
        adc     \tmp,   __zero_reg__
        eor     \t0,        \tmp        //  t0 ^ (t0 <<< 9) ^ (t0 <<< 17)
        eor     \t0 + 1,    \tmp + 1
        eor     \t0 + 2,    \tmp + 2
        eor     \t0 + 3,    \tmp + 3

        //      inner feedback right
        eor     \t0,        \t1         //  t0 ^= t1;
        eor     \t0 + 1,    \t1 + 1
        eor     \t0 + 2,    \t1 + 2
        eor     \t0 + 3,    \t1 + 3

        //      store v[0]
        st      z+, \t0
        st      z+, \t0 + 1
        st      z+, \t0 + 2
        st      z+, \t0 + 3
        .endm

        //  aliases for 32-bit register sets
        W0      = 4                     //  "W0" is ( r4,  r5,  r6,  r7  )
        W1      = 8                     //  "W1" is ( r8,  r9,  r10, r11 )
        W2      = 12                    //  "W2" is ( r12, r13, r14, r15 )
        W3      = 16                    //  "W3" is ( r16, r17, r18, r19 )
        WT      = 20                    //  "WT" is ( r20, r21, r22, r23 )

.mixf4: mix_f   W0, W1, W2, W3, WT
        mix_f   W1, W2, W3, W0, WT
        sub     yl, r25                 //  Optionally wrap y back to start
        sbci    yh, 0
        mix_f   W2, W3, W0, W1, WT
        mix_f   W3, W0, W1, W2, WT
        ret

//  The C call convention with AVR is that:
//  R2  - R17, R28, R29 are call-saved
//  R18 - R27, R30, R31 are call-globbered

//  Prototype:
//  void sneik_f512(void *s, uint8_t r);

.global sneik_f512
        .type   sneik_f512, @function

sneik_f512:
        push    r3
        push    r4
        push    r5
        push    r6
        push    r7
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
        push    r16
        push    r17

        push    r28
        push    r29

        movw    z,      r24             //  state pointer   (arg 0 = r25:r24)
        movw    y,      r24
        mov     r24,    r22             //  domain          (arg 1 = 23:22)
        mov     r3,     r20             //  round count     (arg 2 = r21:r20

        ldi     xl,     lo8(rc)         //  round constant table
        ldi     xh,     hi8(rc)

        ldzv4   W2, 14                  //  W2 = v[14]
        ldzv4   W3, 15                  //  W3 = v[15]

        ldy4    W0
        ldy4    W1

.round:
        ld      WT, x+                  //  round constant
        eor     W0, WT
        eor     W1, r24                 //  domain

        ldi     r25, 0                  //  no wrapup on y
        rcall   .mixf4
        rcall   .mixf4
        rcall   .mixf4
        ldi     r25, 64                 //  wrap y back up
        rcall   .mixf4

        subi    zl, 64                  //  wrap z back up
        sbci    zh, 0

        dec     r3                      //  round count (arg 2 = r21:r20)
        brne    .round

        pop     r29
        pop     r28

        pop     r17
        pop     r16
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     r7
        pop     r6
        pop     r5
        pop     r4
        pop     r3

        ret

        // round constants
        .section    .rodata
rc:     .byte   0xEF, 0xE0, 0xD9, 0xD6, 0xBA, 0xB5, 0x8C, 0x83
        .byte   0x10, 0x1F, 0x26, 0x29, 0x45, 0x4A, 0x73, 0x7C

.global __do_copy_data
.global __do_clear_bss

