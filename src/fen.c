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
    ['K'] = W_KING,
};

void fenToBoard(char *fen, Board *board) {
    memset(&board->pieces, 0, sizeof(Bitboard) * 12);
    memset(&board->wPieces, 0, sizeof(Bitboard));
    memset(&board->bPieces, 0, sizeof(Bitboard));

    int i = 0;
    unsigned char fenChar;
    while((fenChar = *fen) != '\0'){
        if(fenChar >= '1' && fenChar <= '8'){
            i += fenChar - 48;
        }
        else if(fenChar != '/'){
            board->pieces[pieceLookupTable[fenChar]] |= (1ULL << (63 - i));
            i += 1;
        }
        fen++;
    }
    
    board->wPieces = board->pieces[W_PAWN] | board->pieces[W_KNIGHT] | board->pieces[W_BISHOP] | board->pieces[W_ROOK] | board->pieces[W_QUEEN] | board->pieces[W_KING];
    board->bPieces = board->pieces[B_PAWN] | board->pieces[B_KNIGHT] | board->pieces[B_BISHOP] | board->pieces[B_ROOK] | board->pieces[B_QUEEN] | board->pieces[B_KING];
}

