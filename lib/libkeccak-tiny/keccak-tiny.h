#ifndef KECCAK_FIPS202_H
#define KECCAK_FIPS202_H
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define decshake(bits) \
  int shake##bits(uint8_t*, size_t, const uint8_t*, size_t); \
  int shakef##bits(uint8_t*, size_t, FILE **, uint8_t);

#define decsha3(bits) \
  int sha3_##bits(uint8_t*, size_t, const uint8_t*, size_t); \
  int sha3f_##bits(uint8_t*, size_t, FILE **, uint8_t);

decshake(128)
decshake(256)
decsha3(224)
decsha3(256)
decsha3(384)
decsha3(512)
#endif
