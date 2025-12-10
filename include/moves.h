#pragma once

#include "types.h"
#include "board.h"

#define MAKE_MOVE(from, to) (((from) & 0x3F) | (((to) & 0x3F) << 6))
#define MAKE_MOVE_FLAGS(from, to, flags) (((from) & 0x3F) | (((to) & 0x3F) << 6) | (((flags) & 0xF) << 12))

#define GET_FROM(move) ((move) & 0x3F)
#define GET_TO(move) (((move) >> 6) & 0x3F)
#define GET_FLAGS(move) (((move) >> 12) & 0xF)

#define SET_FROM(from) ((u16) (from) & 0x3F)
#define SET_TO(to) ((u16) (to) << 6)
#define SET_FLAGS(flags) ((u16) (flags) << 12)

typedef struct {
    u16 moves[256];
    u8 count;
} MoveList;

void initMoveTables();
void genPseudoLegalMoves(Board *board, MoveList *moveList, u8 colour);
u8 isUnderAttack(Board *board, u8 index, u8 colour);

// Move encoding (32 bits):
// ┬────────┬────────┬────────┬
// │ 15-12  │ 11-6   │ 5-0    │
// │ flags  │ to_sq  │ fr_sq  │
// ┴────────┴────────┴────────┴
