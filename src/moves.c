#include "chess.h"

uint64_t getTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint8_t piece = board[pieceIndex];
    if ((piece & PAWN) == PAWN)
        return pawnTargetSquares(board, pieceIndex);
    if ((piece & KING) == KING)
        return kingTargetSquares(board, pieceIndex);
    if ((piece & KNIGHT) == KNIGHT)
        return knightTargetSquares(board, pieceIndex);
    return 0;
}

uint64_t pawnTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t pawnSquares = 0;
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);
    char direction = COLOUR_DIRECTION(pieceColour);
    uint8_t anteriorSquare = ANTERIOR_SQUARE(pieceIndex, direction);

    if ((pieceIndex % 8 != 0 && pieceColour == WHITE)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == BLACK)){
        uint8_t sinistralSquare = SINISTRAL_SQAURE(anteriorSquare, direction);
        if(board[sinistralSquare]){
            if(COLOUR(board[sinistralSquare]) != pieceColour)
                pawnSquares |= (1ULL << sinistralSquare);
        }
    }

    if ((pieceIndex % 8 != 0 && pieceColour == BLACK)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == WHITE)){
        uint8_t dextralSquare = DEXTRAL_SQUARE(anteriorSquare, direction);
        if(board[dextralSquare]){
            if(COLOUR(board[dextralSquare]) != pieceColour)
                pawnSquares |= (1ULL << dextralSquare);
        }
    }

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

uint64_t kingTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t kingSquares = 0;
    uint64_t piece = board[pieceIndex];
    uint64_t pieceColour = COLOUR(piece);
    char direction = COLOUR_DIRECTION(pieceColour);
    uint8_t rank = RANK(pieceIndex);
    uint8_t file = FILE(pieceIndex);
    
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

uint64_t knightMovesTable[64];
void generateKnightLookupTable(){
    uint64_t* knightMoveTable = malloc(sizeof(uint64_t) * 64);
    memset(knightMoveTable, 0, sizeof(uint64_t) * 64);

    uint64_t squares = 0;
    uint8_t rank = 0;
    uint8_t file = 0;

    for (int i = 0; i < 64; i++){
        squares = 0;
        rank = RANK(i);
        file = FILE(i);
        
        if (rank <= 6){
            if (file != 1)
                squares |= (1ULL << (i - 17));
            if (file != 8)
                squares |= (1ULL << (i - 15));
        }

        if (rank >= 3){
            if (file != 1)
                squares |= (1ULL << (i + 15));
            if (file != 8)
                squares |= (1ULL << (i + 17));
        }

        if (file <= 6){
            if (rank <= 7)
                squares |= (1ULL << (i - 6));

            if (rank >= 2)
                squares |= (1ULL << (i + 10));
        }

        if (file >= 3){
            if (rank <= 7)
                squares |= (1ULL << (i - 10));

            if (rank >= 2)
                squares |= (1ULL << (i + 6));
        }

        knightMovesTable[i] = squares;
    }
}

uint64_t knightTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t knightMoves = 0;
    uint64_t piece = board[pieceIndex];
    uint64_t pieceColour = COLOUR(piece);

    memcpy(&knightMoves, &knightMovesTable[pieceIndex], sizeof(uint64_t));

    for(int i = 0; i < 64; i++){
        if((knightMoves >> i) & 1){
            if((board[i] != EMPTY) && (pieceColour == COLOUR(board[i])))
                knightMoves &= ~(1ULL << i);
        }
    }

    return knightMoves;
}
