#include "chess.h"

const uint8_t pieceLookupTable[256] = {
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

uint8_t* fenToArray(char* fen){
    uint8_t* board = (uint8_t*)malloc(64 * sizeof(uint8_t));
    if (board == NULL) {
        printf("malloc error");
        exit(1);
    }
    memset(board, 0, 64 * sizeof(uint8_t));

    int i = 0;
    uint8_t fenChar;
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
