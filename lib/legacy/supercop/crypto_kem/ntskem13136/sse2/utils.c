/**
 *  utils.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: SSE2/SSE4.1
 *
 *  The code here is taken from amd64cpuinfo.c of supercop-20170904
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "utils.h"

inline uint64_t cpucycles(void)
{
#if defined(__ARM_ARCH)
#if defined(__LP64__)
    /* TODO */
#endif
    return 0;
#else
    uint64_t result;
    __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
                 : "=a" (result) ::  "%rdx");
    return result;
#endif
}

static double osfreq(void)
{
    FILE *f;
    char *x;
    double result;
    int s;
    
    f = fopen("/etc/cpucyclespersecond", "r");
    if (f) {
        s = fscanf(f,"%lf",&result);
        fclose(f);
        if (s > 0) return result;
    }
    
    f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed", "r");
    if (f) {
        s = fscanf(f,"%lf",&result);
        fclose(f);
        if (s > 0) return 1000.0 * result;
    }
    
    f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r");
    if (f) {
        s = fscanf(f,"%lf",&result);
        fclose(f);
        if (s > 0) return 1000.0 * result;
    }
    
    f = fopen("/sys/devices/system/cpu/cpu0/clock_tick", "r");
    if (f) {
        s = fscanf(f,"%lf",&result);
        fclose(f);
        if (s > 0) return result;
    }
    
    f = fopen("/proc/cpuinfo","r");
    if (f) {
        for (;;) {
            s = fscanf(f,"cpu MHz : %lf",&result);
            if (s > 0) break;
            if (s == 0) s = fscanf(f,"%*[^\n]\n");
            if (s < 0) { result = 0; break; }
        }
        fclose(f);
        if (result) return 1000000.0 * result;
    }
    
    f = fopen("/proc/cpuinfo","r");
    if (f) {
        for (;;) {
            s = fscanf(f,"clock : %lf",&result);
            if (s > 0) break;
            if (s == 0) s = fscanf(f,"%*[^\n]\n");
            if (s < 0) { result = 0; break; }
        }
        fclose(f);
        if (result) return 1000000.0 * result;
    }
    
    f = popen("sysctl hw.cpufrequency 2>/dev/null","r");
    if (f) {
        s = fscanf(f,"hw.cpufrequency: %lf",&result);
        pclose(f);
        if (s > 0) if (result > 0) return result;
    }
    
    f = popen("/usr/sbin/lsattr -E -l proc0 -a frequency 2>/dev/null","r");
    if (f) {
        s = fscanf(f,"frequency %lf",&result);
        pclose(f);
        if (s > 0) return result;
    }
    
    f = popen("/usr/sbin/psrinfo -v 2>/dev/null","r");
    if (f) {
        for (;;) {
            s = fscanf(f," The %*s processor operates at %lf MHz",&result);
            if (s > 0) break;
            if (s == 0) s = fscanf(f,"%*[^\n]\n");
            if (s < 0) { result = 0; break; }
        }
        pclose(f);
        if (result) return 1000000.0 * result;
    }
    
    x = getenv("cpucyclespersecond");
    if (x) {
        s = sscanf(x,"%lf",&result);
        if (s > 0) return result;
    }
    
    return 0;
}

uint64_t cpucycles_persecond(void)
{
    return (uint64_t)(osfreq());
}
