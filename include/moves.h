#pragma once

#include "types.h"

void initMoveTables();

void bishopMoves(Bitboard bishops, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber);
void knightMoves(Bitboard knights, Bitboard colouredPieces, u16 *moves, u8 *moveNumber);
void kingMoves(Bitboard kings, Bitboard colouredPieces, u16 *moves, u8 *moveNumber);
void pawnMoves(Bitboard pawns, Bitboard colouredPieces, u16 *moves, u8 *moveNumber, u8 colour);
void rookMoves(Bitboard rooks, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber);
