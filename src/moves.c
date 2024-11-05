#include "chess.h"

uint64_t getMoves(uint8_t* board, uint8_t pieceIndex){
    uint8_t piece = board[pieceIndex];
    if ((piece & 0b111) == PAWN) {
        return getPawnSquares(board, pieceIndex);
    }
    if ((piece & KING) == KING) {
        return getKingSquares(board, pieceIndex);
    }
    return 0;
}

uint64_t getPawnSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t pawnSquares = 0;
    char piece = board[pieceIndex];
    char pieceColour = COLOUR(piece);
    char direction = COLOUR_DIRECTION(pieceColour);
    uint8_t anteriorSquare = ANTERIOR_SQUARE(pieceIndex, direction);

    // Checking if the pawn can take left
    if ((pieceIndex % 8 != 0 && pieceColour == WHITE)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == BLACK)){
        uint8_t sinistralSquare = SINISTRAL_SQAURE(anteriorSquare, direction);
        if(board[sinistralSquare]){
            if(COLOUR(board[sinistralSquare]) != pieceColour)
                pawnSquares |= (1ULL << sinistralSquare);
        }
    }

    // Checking if the pawn can take right
    if ((pieceIndex % 8 != 0 && pieceColour == BLACK)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == WHITE)){
        uint8_t dextralSquare = DEXTRAL_SQUARE(anteriorSquare, direction);
        if(board[dextralSquare]){
            if(COLOUR(board[dextralSquare]) != pieceColour)
                pawnSquares |= (1ULL << dextralSquare);
        }
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

uint64_t getKingSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t kingSquares = 0;
    char piece = board[pieceIndex];
    char pieceColour = COLOUR(piece);
    char direction = COLOUR_DIRECTION(pieceColour);
    char rank = GET_RANK(pieceIndex);
    char file = GET_FILE(pieceIndex);
    
    uint8_t cornerTargetSquare;
    uint8_t targetSquare;

    targetSquare = ANTERIOR_SQUARE(pieceIndex, direction);
    if((rank - direction) % 9 != 0){
        if(COLOUR(board[targetSquare]) != pieceColour || board[targetSquare] == EMPTY)
            kingSquares |= (1ULL << targetSquare);
        cornerTargetSquare = SINISTRAL_SQAURE(targetSquare, direction);
        if(((file + direction) % 9 != 0) && (COLOUR(board[cornerTargetSquare]) != pieceColour || board[cornerTargetSquare] == EMPTY))
            kingSquares |= (1ULL << cornerTargetSquare);
        cornerTargetSquare = DEXTRAL_SQUARE(targetSquare, direction);
        if(((file - direction) % 9 != 0) && (COLOUR(board[cornerTargetSquare]) != pieceColour || board[cornerTargetSquare] == EMPTY))
            kingSquares |= (1ULL << cornerTargetSquare);
    }

    targetSquare = POSTERIOR_SQUARE(pieceIndex, direction);
    if((rank + direction) % 9 != 0){
        if(COLOUR(board[targetSquare]) != pieceColour || board[targetSquare] == EMPTY)
            kingSquares |= (1ULL << targetSquare);
        cornerTargetSquare = SINISTRAL_SQAURE(targetSquare, direction);
        if(((file + direction) % 9 != 0) && (COLOUR(board[cornerTargetSquare]) != pieceColour || board[cornerTargetSquare] == EMPTY))
            kingSquares |= (1ULL << cornerTargetSquare);
        cornerTargetSquare = DEXTRAL_SQUARE(targetSquare, direction);
        if(((file - direction) % 9 != 0) && (COLOUR(board[cornerTargetSquare]) != pieceColour || board[cornerTargetSquare] == EMPTY))
            kingSquares |= (1ULL << cornerTargetSquare);
    }

    targetSquare = SINISTRAL_SQAURE(pieceIndex, direction);
    if(((file + direction) % 9 != 0) && (COLOUR(board[targetSquare]) != pieceColour || board[targetSquare] == EMPTY))
        kingSquares |= (1ULL << targetSquare);

    targetSquare = DEXTRAL_SQUARE(pieceIndex, direction);
    if(((file - direction) % 9 != 0) && (COLOUR(board[targetSquare]) != pieceColour || board[targetSquare] == EMPTY))
        kingSquares |= (1ULL << targetSquare);

    return kingSquares;
}