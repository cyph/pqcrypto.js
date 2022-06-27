#ifndef CLOCK_CYCLE_H
#define CLOCK_CYCLE_H

typedef unsigned long long int u64;

u64 start_rdtsc( )
{
        unsigned high, low;
        
        __asm__ volatile("CPUID\n\t"
                         "RDTSC\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax, %1\n\t": "=r" (high),
                         "=r" (low):: "%rax", "%rbx", "%rcx", "%rdx");
        return ( ((u64)low) | (((u64)high) << 32));
}

u64 end_rdtsc( )
{
        unsigned high, low;
        
        __asm__ volatile("RDTSCP\n\t"
                         "mov %%edx, %0\n\t"
                         "mov %%eax,%1\n\t"
                         "CPUID\n\t": "=r" (high), "=r" (low)::
                         "%rax", "%rbx", "%rcx", "%rdx");
        
        return ( ((u64)low) | (((u64)high) << 32));
}

static inline u64 cpucycles( )
{
        u64 result;
        asm volatile (".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
                      : "=a" (result) ::  "%rdx");
        return result;
}

#endif
