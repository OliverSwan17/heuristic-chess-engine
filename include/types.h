#pragma once

#include <stdint.h>

#define FILE(i) (((i) % 8) + 1)
#define RANK(i) (i / 8 + 1)

#define WHITE 0
#define BLACK 1

typedef uint64_t Bitboard;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
