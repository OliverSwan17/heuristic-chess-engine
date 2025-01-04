#include "chess.h"

uint64_t getColourLegalMoves(uint8_t* board, uint8_t colour){
    uint64_t moves = 0;
    for (int i = 0; i < 64; i++){
        if(COLOUR(board[i]) == colour)
            moves |= getLegalMoves(board, i);
    }

    return moves;
}

uint64_t getTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint8_t piece = board[pieceIndex];
    if ((piece & 0b111) == PAWN)
        return pawnTargetSquares(board, pieceIndex);
    if ((piece & 0b111) == KING)
        return kingTargetSquares(board, pieceIndex);
    if ((piece & 0b111) == KNIGHT)
        return knightTargetSquares(board, pieceIndex);
    if ((piece & 0b111) == BISHOP)
        return bishopTargetSquares(board, pieceIndex);
    if ((piece & 0b111) == ROOK)
        return rookTargetSquares(board, pieceIndex);
    if ((piece & 0b111) == QUEEN)
        return (bishopTargetSquares(board, pieceIndex) | rookTargetSquares(board, pieceIndex));
    return 0;
}

uint64_t getColourTargetSquares(uint8_t* board, uint8_t colour){
    uint64_t targetSquares = 0;
    uint8_t piece = 0;
    for(int i = 0; i < 64; i++){
        piece = board[i];
        if(COLOUR(piece) == colour){
            targetSquares = (targetSquares | getTargetSquares(board, i));
        }
    }

    return targetSquares;
}

void legalMoves(BoardState* s){
    Move *moves = malloc(218 * sizeof(Move));
    uint8_t moveCount = 0;

    uint8_t *board = s->board;

    // Calculating the castling bits
    s->castlingSquares = getCastlingSquares(s->board, s->turn);

    // Looping through each piece of the correct colour.
    for (int srcIndex = 0; srcIndex < 64; srcIndex++){
        uint8_t piece = board[srcIndex];

        // Clearing the en-passant bit.
        if ((piece & 0b111) == PAWN && COLOUR(piece) == s->turn)
            s->board[srcIndex] &= ~0b100000;

        if (COLOUR(piece) != s->turn || board[srcIndex] == EMPTY)
            continue;

        uint64_t targetSquares = getTargetSquares(board, srcIndex);
        targetSquares |= s->castlingSquares;

        for (int dstIndex = 0; dstIndex < 64; dstIndex++){
            if (targetSquares & (1ULL << dstIndex)) {
                uint8_t moveType = 0;
                uint8_t kingSquare = (s->turn == 0) ? s->bKingIndex : s->wKingIndex;
                uint8_t tempBoard[64];
                memcpy(tempBoard, board, 64);

                // First check for castling
                if ((s->castlingSquares) && (s->castlingSquares & (1ULL << dstIndex))){
                    if (s->turn == WHITE && srcIndex == 60){
                        if (dstIndex == 62){
                            tempBoard[dstIndex] = tempBoard[60];
                            tempBoard[srcIndex] = EMPTY;
                            tempBoard[61] = tempBoard[63];
                            tempBoard[63] = EMPTY;
                            moveType = KING_CASTLE;
                        }else if (dstIndex == 58){
                            tempBoard[dstIndex] = tempBoard[60];
                            tempBoard[srcIndex] = EMPTY;
                            tempBoard[59] = tempBoard[56];
                            tempBoard[56] = EMPTY;
                            moveType = QUEEN_CASTLE;
                        }
                    }else if (s->turn == BLACK && srcIndex == 4){
                        if (dstIndex == 6){
                            tempBoard[dstIndex] = tempBoard[4];
                            tempBoard[srcIndex] = EMPTY;
                            tempBoard[5] = tempBoard[7];
                            tempBoard[7] = EMPTY;
                            moveType = KING_CASTLE;
                        }else if (dstIndex == 2){
                            tempBoard[dstIndex] = tempBoard[4];
                            tempBoard[srcIndex] = EMPTY;
                            tempBoard[3] = tempBoard[0];
                            tempBoard[0] = EMPTY;
                            moveType = QUEEN_CASTLE;
                        }
                    }
                }else if ((PAWN == (tempBoard[srcIndex] & 0b111)) && (FILE(srcIndex) != FILE(dstIndex)) && (tempBoard[dstIndex] == EMPTY)){
                    moveType = EN_PASSANT;
                    if (s->turn == WHITE){
                        tempBoard[dstIndex] = tempBoard[srcIndex];
                        tempBoard[srcIndex] = EMPTY;
                        tempBoard[POSTERIOR_SQUARE(dstIndex, WHITE_DIRECTION)] = EMPTY;
                    }else{
                        tempBoard[dstIndex] = tempBoard[srcIndex];
                        tempBoard[srcIndex] = EMPTY;
                        tempBoard[POSTERIOR_SQUARE(dstIndex, BLACK_DIRECTION)] = EMPTY;
                    }
                }else{
                    if (tempBoard[dstIndex] == EMPTY)
                        moveType = QUIET_MOVE;
                    else
                        moveType = CAPTURE;

                    tempBoard[srcIndex] = EMPTY;
                    tempBoard[dstIndex] = piece;
                }

                if((piece & 0b111) == KING){
                    kingSquare = dstIndex;
                }

                // Checking for promotion
                if (((s->board[dstIndex] & 0b111) == PAWN) && (RANK(dstIndex) == 1 || RANK(dstIndex) == 8)){
                    moveType |= 0b1000;

                    if ((s->board[srcIndex] & 0b111) == BISHOP)
                        moveType |= 0b01;
                    else if ((s->board[srcIndex] & 0b111) == ROOK)
                        moveType |= 0b10;
                    else if ((s->board[srcIndex] & 0b111) == QUEEN)
                        moveType |= 0b11;
                }

                if (abs(((int) srcIndex) - ((int)dstIndex)) == 16){
                    moveType = DOUBLE_PAWN_PUSH;
                }

                // We require some more advanced checks here as some legal moves are being put through.
                if(!((getColourTargetSquares(tempBoard, (s->turn == 0) ? 1 : 0) >> kingSquare) & 1)){
                    moves[moveCount].type = moveType;
                    moves[moveCount].srcSquare = srcIndex;
                    moves[moveCount].dstSquare = dstIndex;
                    moveCount += 1;
                }
            }
        }
    }

    s->numberOfLegalmoves = moveCount;
    s->moves = moves;
}

uint64_t getLegalMoves(uint8_t* board, uint8_t pieceIndex){
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);
    uint8_t oppositeColour = (pieceColour == 0) ? 1 : 0;
    uint64_t kingSquare = 0;

    uint64_t legalSquares = 0;
    uint64_t targetSquares = 0;
    uint64_t oppositeColourTargetSquares = 0;

    for (int i = 0; i < 64; i++){
        if(((board[i] & 0b111) == KING) && COLOUR(board[i]) == pieceColour){
            kingSquare = i;
            break;
        }
    }

    targetSquares = getTargetSquares(board, pieceIndex);
    uint8_t tempBoard[64];

    for (int i = 0; i < 64; i++){
        if(targetSquares & (1ULL << i)){
            memcpy(tempBoard, board, 64);
            
            if ((PAWN == (tempBoard[pieceIndex] & 0b111)) && (FILE(pieceIndex) != FILE(i)) && (tempBoard[i] == EMPTY)){
                if (pieceColour == WHITE){
                    tempBoard[i] = tempBoard[pieceIndex];
                    tempBoard[pieceIndex] = EMPTY;
                    tempBoard[POSTERIOR_SQUARE(i, WHITE_DIRECTION)] = EMPTY;
                }else{
                    tempBoard[i] = tempBoard[pieceIndex];
                    tempBoard[pieceIndex] = EMPTY;
                    tempBoard[POSTERIOR_SQUARE(i, BLACK_DIRECTION)] = EMPTY;
                }
                
            }else{
                tempBoard[pieceIndex] = EMPTY;
                tempBoard[i] = piece;
            }
            
            if((piece & 0b111) == KING){
                kingSquare = i;
            }

            oppositeColourTargetSquares = getColourTargetSquares(tempBoard, oppositeColour);

            if(!((oppositeColourTargetSquares >> kingSquare) & 1)){
                legalSquares |= (1ULL << i);
            }
        }
    }

    return legalSquares;
}

uint64_t pawnTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t pawnSquares = 0;
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);
    int8_t direction = COLOUR_DIRECTION(pieceColour);
    uint8_t anteriorSquare = ANTERIOR_SQUARE(pieceIndex, direction);

    if ((pieceIndex % 8 != 0 && pieceColour == WHITE)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == BLACK)){
        uint8_t sinistralSquare = SINISTRAL_SQAURE(anteriorSquare, direction);
        if(board[sinistralSquare]){
            if(COLOUR(board[sinistralSquare]) != pieceColour)
                pawnSquares |= (1ULL << sinistralSquare);
        }

        if (board[SINISTRAL_SQAURE(pieceIndex, direction)] & 0b100000)
           pawnSquares |= (1ULL << sinistralSquare);
    }

    if ((pieceIndex % 8 != 0 && pieceColour == BLACK)  || ((pieceIndex + 1) % 8 != 0 && pieceColour == WHITE)){
        uint8_t dextralSquare = DEXTRAL_SQUARE(anteriorSquare, direction);
        if(board[dextralSquare]){
            if(COLOUR(board[dextralSquare]) != pieceColour)
                pawnSquares |= (1ULL << dextralSquare);
        }

        if (board[DEXTRAL_SQUARE(pieceIndex, direction)] & 0b100000)
            pawnSquares |= (1ULL << dextralSquare);
    }

    if (IS_EMPTY(board[anteriorSquare])){
        pawnSquares |= (1ULL << anteriorSquare);
        anteriorSquare = ANTERIOR_SQUARE(anteriorSquare, direction);
        if((pieceColour == WHITE && pieceIndex >= 48 && pieceIndex <= 55) || (pieceColour == BLACK && pieceIndex >=8 && pieceIndex <= 15)){
            if(IS_EMPTY(board[anteriorSquare]))
                pawnSquares |= (1ULL << anteriorSquare);
        }
    }

    return pawnSquares;
}

uint64_t kingTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t kingSquares = 0;
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);
    int8_t direction = COLOUR_DIRECTION(pieceColour);
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

uint64_t knightMoveTable[64];
void generateKnightLookupTable(){
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

        knightMoveTable[i] = squares;
    }
}

uint64_t knightTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t knightMoves = 0;
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);

    memcpy(&knightMoves, &knightMoveTable[pieceIndex], sizeof(uint64_t));

    for(int i = 0; i < 64; i++){
        if((knightMoves >> i) & 1){
            if((board[i] != EMPTY) && (pieceColour == COLOUR(board[i])))
                knightMoves &= ~(1ULL << i);
        }
    }

    return knightMoves;
}

uint64_t rookTargetSquares(uint8_t* board, uint8_t pieceIndex){
    uint64_t rookSquares = 0;
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);
    uint8_t rank = RANK(pieceIndex);
    uint8_t file = FILE(pieceIndex);
    
    uint8_t targetSquare = pieceIndex; 
    for(int i = 1; i < file; i++){
        targetSquare--;
        if(board[targetSquare] == EMPTY)
            rookSquares |= (1ULL << targetSquare);
        else {
            if (COLOUR(board[targetSquare]) != pieceColour)
                rookSquares |= (1ULL << targetSquare);
            break;
        }
    }

    targetSquare = pieceIndex;
    for(int i = 8; i > file; i--){
        targetSquare++;
        if(board[targetSquare] == EMPTY)
            rookSquares |= (1ULL << targetSquare);
        else {
            if (COLOUR(board[targetSquare]) != pieceColour)
                rookSquares |= (1ULL << targetSquare);
            break;
        }
    }
    
    targetSquare = pieceIndex;
    for(int i = 1; i < rank; i++){
        targetSquare += 8;
        if(board[targetSquare] == EMPTY)
            rookSquares |= (1ULL << targetSquare);
        else {
            if (COLOUR(board[targetSquare]) != pieceColour)
                rookSquares |= (1ULL << targetSquare);
            break;
        }
    }

    targetSquare = pieceIndex;
    for(int i = 8; i > rank; i--){
        targetSquare -= 8;
        if(board[targetSquare] == EMPTY)
            rookSquares |= (1ULL << targetSquare);
        else {
            if (COLOUR(board[targetSquare]) != pieceColour)
                rookSquares |= (1ULL << targetSquare);
            break;
        }
    }

    return rookSquares;
}

uint64_t bishopTargetSquares(uint8_t* board, uint8_t pieceIndex) {
    uint64_t bishopSquares = 0;
    uint8_t piece = board[pieceIndex];
    uint8_t pieceColour = COLOUR(piece);

    uint8_t targetSquare = pieceIndex;
    if(FILE(pieceIndex) != 8 && RANK(pieceIndex) != 8){
        while (1)
        {
            targetSquare -= 7;
            if(board[targetSquare] == EMPTY)
                bishopSquares |= (1ULL << targetSquare);
            else {
                if (COLOUR(board[targetSquare]) != pieceColour)
                    bishopSquares |= (1ULL << targetSquare);
                break;
            }
            if(FILE(targetSquare) == 1 || RANK(targetSquare) == 8 || FILE(targetSquare) == 8 || RANK(targetSquare) == 1)
                break;
        }
    }

    targetSquare = pieceIndex;
    if(FILE(pieceIndex) != 1 && RANK(pieceIndex) != 1){
        while (1)
        {
            targetSquare += 7;
            if(board[targetSquare] == EMPTY)
                bishopSquares |= (1ULL << targetSquare);
            else {
                if (COLOUR(board[targetSquare]) != pieceColour)
                    bishopSquares |= (1ULL << targetSquare);
                break;
            }
            if(FILE(targetSquare) == 1 || RANK(targetSquare) == 8 || FILE(targetSquare) == 8 || RANK(targetSquare) == 1)
                break;
        }
    }

    targetSquare = pieceIndex;
    if(FILE(pieceIndex) != 8 && RANK(pieceIndex) != 1){
        while (1)
        {
            targetSquare += 9;
            if(board[targetSquare] == EMPTY)
                bishopSquares |= (1ULL << targetSquare);
            else {
                if (COLOUR(board[targetSquare]) != pieceColour)
                    bishopSquares |= (1ULL << targetSquare);
                break;
            }
            if(FILE(targetSquare) == 1 || RANK(targetSquare) == 8 || FILE(targetSquare) == 8 || RANK(targetSquare) == 1)
                break;
        }
    }

    targetSquare = pieceIndex;
    if(FILE(pieceIndex) != 1 && RANK(pieceIndex) != 8){
        while (1)
        {
            targetSquare -= 9;
            if(board[targetSquare] == EMPTY)
                bishopSquares |= (1ULL << targetSquare);
            else {
                if (COLOUR(board[targetSquare]) != pieceColour)
                    bishopSquares |= (1ULL << targetSquare);
                break;
            }
            if(FILE(targetSquare) == 1 || RANK(targetSquare) == 8 || FILE(targetSquare) == 8 || RANK(targetSquare) == 1)
                break;
        }
    }

    return bishopSquares;
}

uint64_t getCastlingSquares(uint8_t* board, uint8_t colour){
    uint64_t castlingSquares = 0;
    uint64_t attackedSquares = getColourTargetSquares(board, !colour);
    if ((KING == (board[4] & 0b111)) && (COLOUR(board[4]) == BLACK) && (!(board[4] & (1 << 5)))){ // Checks for King
        if ((ROOK == (board[7] & 0b111)) && (COLOUR(board[7]) == BLACK) && (!(board[7] & (1 << 5)))){// Checks kingside rook
            if (board[5] == EMPTY && board[6] == EMPTY) // Checks if there is empty space inbetween the King and kingside rook
                if (!(attackedSquares & ((1ULL << 4) | (1ULL << 5) | (1ULL << 6)))) // Checks if castle is legal.
                    castlingSquares |= (1ULL << 6);
        }
        if ((ROOK == (board[0] & 0b111)) && (COLOUR(board[0]) == BLACK) && (!(board[0] & (1 << 5)))) // Checks queenside rook
            if (board[1] == EMPTY && board[2] == EMPTY && board[3] == EMPTY) // Checks if there is empty space inbetween the King and queenside rook
                if (!(attackedSquares & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3) | (1ULL << 4)))) // Checks if castle is legal.
                    castlingSquares |= (1ULL << 2);
    }

    if ((KING == (board[60] & 0b111)) && (COLOUR(board[60]) == WHITE) && (!(board[60] & (1 << 5)))){ // Checks for King
        if ((ROOK == (board[63] & 0b111)) && (COLOUR(board[63]) == WHITE) && (!(board[63] & (1 << 5)))){// Checks kingside rook
            if (board[61] == EMPTY && board[62] == EMPTY) // Checks if there is empty space inbetween the King and kingside rook
                if (!(attackedSquares & ((1ULL << 60) | (1ULL << 61) | (1ULL << 62)))) // Checks if castle is legal.
                    castlingSquares |= (1ULL << 62);
        }
        if ((ROOK == (board[56] & 0b111)) && (COLOUR(board[56]) == WHITE) && (!(board[56] & (1 << 5)))) // Checks queenside rook
            if (board[57] == EMPTY && board[58] == EMPTY && board[59] == EMPTY) // Checks if there is empty space inbetween the King and queenside rook
                if (!(attackedSquares & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59) | (1ULL << 60)))) // Checks if castle is legal.
                    castlingSquares |= (1ULL << 58);
    }

    return castlingSquares;
}
