#include "chess.h"

uint64_t getPawnSquares(unsigned char* board, unsigned char pieceIndex){
    uint64_t pawnSquares = 0;
    char piece = board[pieceIndex];
    char pieceColour = COLOUR(piece);
    char direction = COLOUR_DIRECTION(pieceColour);
    unsigned char anteriorSquare = ANTERIOR_SQUARE(pieceIndex, direction);

    // Checking if the pawn can take left
    if ((pieceIndex % 8 != 0 && pieceColour == WHITE)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == BLACK)){
        unsigned char sinistralSquare = SINISTRAL_SQAURE(anteriorSquare, direction);
        if(COLOUR(board[sinistralSquare]) != pieceColour)
            pawnSquares |= (1ULL << sinistralSquare);
    }

    // Checking if the pawn can take right
    if ((pieceIndex % 8 != 0 && pieceColour == BLACK)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == WHITE)){
        unsigned char dextralSquare = DEXTRAL_SQUARE(anteriorSquare, direction);
        if(COLOUR(board[dextralSquare]) != pieceColour)
            pawnSquares |= (1ULL << dextralSquare);
    }

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