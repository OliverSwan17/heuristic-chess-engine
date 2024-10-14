#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define B_PAWN 1
#define B_KNIGHT 2
#define B_BISHOP 3
#define B_ROOK 4
#define B_QUEEN 5
#define B_KING 6

#define W_PAWN 7
#define W_KNIGHT 8
#define W_BISHOP 9
#define W_ROOK 10
#define W_QUEEN 11
#define W_KING 12

//Fen
unsigned char* fenToArray(char* fen);
void printBoard(unsigned char *board);
extern const unsigned char pieceLookupTable[256];