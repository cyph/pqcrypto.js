#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include "osfreq.c"

static int fddev = -1;

long long cpucycles_perfevent(void)
{
  long long result;

  if (fddev == -1) {
    static struct perf_event_attr attr;

    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(struct perf_event_attr);
    attr.config = PERF_COUNT_HW_CPU_CYCLES;
    attr.disabled = 1;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;

    fddev = syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
    /* keep quiet on failure */

    ioctl(fddev, PERF_EVENT_IOC_RESET, 0);
    ioctl(fddev, PERF_EVENT_IOC_ENABLE, 0);
  }

  if (read(fddev,&result,sizeof result) < sizeof result) return 0;
  return result;
}

long long cpucycles_perfevent_persecond(void)
{
  return osfreq();
}
