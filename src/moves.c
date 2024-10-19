#include "chess.h"

uint64_t getPawnSquares(unsigned char* board, unsigned char pieceIndex){
    uint64_t pawnSquares = 0;
    char piece = board[pieceIndex];
    char pieceColour = COLOUR(piece);
    char direction = COLOUR_DIRECTION(pieceColour);
    unsigned char anteriorSquare = ANTERIOR_SQUARE(pieceIndex, direction);

    // Checking forward moves
    if (IS_EMPTY(board[anteriorSquare])){
        pawnSquares |= (1ULL << anteriorSquare);
        anteriorSquare = ANTERIOR_SQUARE(anteriorSquare, direction);
        if(IS_EMPTY(board[anteriorSquare])){
            if((pieceColour == WHITE && pieceIndex >= 48 && pieceIndex <= 55) || (pieceColour == BLACK && pieceIndex >=8 && pieceIndex <= 15)){
                pawnSquares |= (1ULL << anteriorSquare);
            }
        }
    }
    
    return pawnSquares;
}