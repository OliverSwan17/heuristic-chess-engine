#include "chess.h"
// Refactor this to use ENUMS
const unsigned char pieceLookupTable[256] = {
    ['p'] = 1,
    ['n'] = 2,
    ['b'] = 3,
    ['r'] = 4,
    ['q'] = 5,
    ['k'] = 6,
    ['P'] = 9,
    ['N'] = 10,
    ['B'] = 11,
    ['R'] = 12,
    ['Q'] = 13,
    ['K'] = 14
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
