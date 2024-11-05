#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint8_t* fenToArray(char* fen);
void printBoard(uint8_t *board);

#define B_PAWN 1
#define B_KNIGHT 2
#define B_BISHOP 3
#define B_ROOK 4
#define B_QUEEN 5
#define B_KING 6

#define W_PAWN 9
#define W_KNIGHT 10
#define W_BISHOP 11
#define W_ROOK 12
#define W_QUEEN 13
#define W_KING 14