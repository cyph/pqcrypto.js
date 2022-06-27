#include <stdio.h>
#include <stdlib.h>

int main()
{
  FILE *fi;
  int data[20];
  int i;

  fi = fopen("/sys/devices/system/cpu/cpu0/regs/identification/midr_el1","r");
  if (!fi) exit(1);

  for (i = 0;i < 20;++i) data[i] = fgetc(fi);
  if (data[19] != EOF) exit(1);
  if (data[18] != '\n') exit(1);

  for (i = 10;i < 19;++i) putchar(data[i]);
  exit(0);
}
