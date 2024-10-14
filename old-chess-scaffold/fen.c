#include "chess.h"

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

void testFENParsing() {
    unsigned char* board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    unsigned char expectedBoard[64] = {
        B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK,
        B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN, B_PAWN,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN, W_PAWN,
        W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK
    };

    unsigned char* board2 = fenToArray("8/8/8/8/8/8/8/8");
    unsigned char expectedBoard2[64] = {
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
        EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
    };

    for (int i = 0; i < 64; i++) {
        assert(board[i] == expectedBoard[i]);
        assert(board2[i] == expectedBoard2[i]);
    }

    free(board);
    free(board2);
}
