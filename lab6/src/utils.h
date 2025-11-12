#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

uint64_t factorial_range(int start, int end);
uint64_t mod_mult(uint64_t a, uint64_t b, uint64_t mod);

int send_range_and_receive_result(const char *ip, int port, int start, int end, uint64_t *result);

#endif // UTILS_H
