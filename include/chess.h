#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6


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

//Fen
unsigned char* fenToArray(char* fen);
void printBoard(unsigned char *board);
extern const unsigned char pieceLookupTable[256];