#pragma once

#include "types.h"
#include "board.h"

#define GET_FROM(move) ((move) & 0x3F)
#define GET_TO(move) (((move) >> 6) & 0x3F)
#define GET_FLAGS(move) (((move) >> 12) & 0xF)

#define SET_FROM(from) ((u16) (from) & 0x3F)
#define SET_TO(to) ((u16) (to) << 6)
#define SET_FLAGS(flags) ((u16) (flags) << 12)

void initMoveTables();
void genPseudoLegalMoves(Board *board, u16 *moves, u8 *moveNumber, u8 colour);

void bishopMoves(Bitboard bishops, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber);
void knightMoves(Bitboard knights, Bitboard colouredPieces, u16 *moves, u8 *moveNumber);
void kingMoves(Bitboard kings, Bitboard colouredPieces, u16 *moves, u8 *moveNumber);
void pawnMoves(Bitboard pawns, Bitboard colouredPieces, u16 *moves, u8 *moveNumber, u8 colour);
void rookMoves(Bitboard rooks, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber);

// Move encoding (32 bits):
// ┬────────┬────────┬────────┬
// │ 15-12  │ 11-6   │ 5-0    │
// │ flags  │ to_sq  │ fr_sq  │
// ┴────────┴────────┴────────┴
