#include <string.h>

#include "fen.h"

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
    ['K'] = W_KING,
};

void fenToBoard(char *fen, Board *board) {
    memset(&board->pieces, 0, sizeof(Bitboard) * 12);
    memset(&board->wPieces, 0, sizeof(Bitboard));
    memset(&board->bPieces, 0, sizeof(Bitboard));

    char ranks[8][8];
    memset(&ranks, 0, 64);
    int rank = 0;
    
    int i = 0;
    unsigned char fenChar;
    while((fenChar = *fen) != '\0') {
        if(fenChar == '/') {
            rank++;
            i = 0;
        }else {
            ranks[rank][i] = fenChar;
            i++;
        }
        fen++;
    }

    int targetSquare = 0;
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            unsigned char fen = ranks[i][j];
        
            if(fen >= '1' && fen <= '8'){
                targetSquare += fen - 48;
            }
            else if(fen == 0){
                break;
            }else{
                board->pieces[pieceLookupTable[fen]] |= (1ULL << targetSquare);
                targetSquare += 1;
            }
        }
    }

    board->wPieces = board->pieces[W_PAWN] | board->pieces[W_KNIGHT] | board->pieces[W_BISHOP] | board->pieces[W_ROOK] | board->pieces[W_QUEEN] | board->pieces[W_KING];
    board->bPieces = board->pieces[B_PAWN] | board->pieces[B_KNIGHT] | board->pieces[B_BISHOP] | board->pieces[B_ROOK] | board->pieces[B_QUEEN] | board->pieces[B_KING];
}

