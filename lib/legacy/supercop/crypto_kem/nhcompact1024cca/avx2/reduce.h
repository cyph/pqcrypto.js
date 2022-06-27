#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

int16_t montgomery_reduce(int32_t a);
int16_t barrett_reduce(int16_t a);

#endif
