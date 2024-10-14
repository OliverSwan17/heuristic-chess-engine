#include "chess.h"
// Refactor this to use ENUMS
const unsigned char pieceLookupTable[256] = {
    ['p'] = B_PAWN,
    ['n'] = B_KNIGHT,
    ['b'] = B_BISHOP,
    ['r'] = B_ROOK,
    ['q'] = B_QUEEN,
    ['k'] = B_KING,
    ['P'] = W_PAWN,
    ['N'] = W_KNIGHT,
    ['B'] = W_BISHOP,
    ['R'] = W_ROOK,
    ['Q'] = W_QUEEN,
    ['K'] = W_KING
};

unsigned char* fenToArray(char* fen){
    unsigned char* board = (unsigned char*)malloc(64 * sizeof(unsigned char));
    if (board == NULL) {
        printf("Memory allocation failed!");
        exit(1);
    }
    memset(board, 0, 64 * sizeof(char));

    int i = 0;
    unsigned char fenChar;
    while((fenChar = *fen) != '\0'){
        if(fenChar >= '1' && fenChar <= '8'){
            i += fenChar - 48;
        }
        else if(fenChar != '/'){
            *(board + i) = *(pieceLookupTable + fenChar);
            i += 1;
        }
        fen++;
    }

    return board;
}

void printBoard(unsigned char *board){
    for (int i = 0; i < 64; i++) {
        printf("%i ", board[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }
    printf("\n");
}