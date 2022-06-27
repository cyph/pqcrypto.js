#! /usr/bin/env python3

from pathlib import Path
import shutil
import subprocess
import tempfile

HERE = Path(__file__).parent

MAIN_C = r"""
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "blake3.h"

uint64_t time_ns() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_nsec + ((uint64_t)1000000000) * ((uint64_t)ts.tv_sec);
}

int main() {
  // Allocate and initialize 1 MiB of memory.
  size_t input_len = 1 << 20;
  uint8_t *input = malloc(input_len);
  assert(input != NULL);
  memset(input, 42, input_len);

  // Time how long it takes to hash it, three times.
  for (size_t i = 0; i < 10; i++) {
    uint8_t output[BLAKE3_OUT_LEN];
    uint64_t start_ns = time_ns();
    blake3_default_hash(input, input_len, output);
    uint64_t end_ns = time_ns();
    uint64_t diff_ns = end_ns - start_ns;
    double gbps = ((double)input_len) / ((double)diff_ns);
    printf("%.2f GB/s\n", gbps);
  }
  return 0;
}
"""


def main():
    tempdir = tempfile.mkdtemp(prefix="supercop_test.")
    print(tempdir)
    with (Path(tempdir) / "main.c").open("w") as mainfile:
        mainfile.write(MAIN_C)
    c_files = []
    for filepath in HERE.iterdir():
        if filepath.name.startswith("blake3"):
            shutil.copy(filepath, tempdir)
            if filepath.suffix in (".c", ".S"):
                c_files.append(filepath.name)
    cmd = ["gcc", "-O3", "-march=native", "-o", "blake3.exe", "main.c"]
    cmd += c_files
    print(" ".join(cmd))
    subprocess.run(cmd, check=True, cwd=tempdir)
    subprocess.run([Path(tempdir) / "blake3.exe"], check=True, cwd=tempdir)


if __name__ == "__main__":
    main()
