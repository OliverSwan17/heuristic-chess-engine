#pragma once

#include "types.h"

typedef struct {
    Bitboard pieces[12];
    u8 mailbox[64];
    Bitboard wPieces;
    Bitboard bPieces;
    Bitboard wAttacking;
    Bitboard bAttacking;
    u8 enPassant;
    u8 castling;
    u8 turn;
} Board;

enum PieceType {
    W_PAWN,
    W_KNIGHT,
    W_BISHOP,
    W_ROOK,
    W_QUEEN,
    W_KING,
    B_PAWN,
    B_KNIGHT,
    B_BISHOP,
    B_ROOK,
    B_QUEEN,
    B_KING,
    NO_PIECE,
};

void initMailbox(Board *board);
