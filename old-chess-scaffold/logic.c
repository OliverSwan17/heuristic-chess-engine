#include "chess.h"

const int positionOffsets[8] = {-9, -8, -7, -1, 1, 7, 8, 9};

Positions* getMoves(unsigned char* board, int pieceIndex) {
    if ((board[pieceIndex] == W_PAWN || board[pieceIndex] == B_PAWN)) 
        return getPawnMoves(board, pieceIndex);
    if ((board[pieceIndex] == W_ROOK || board[pieceIndex] == B_ROOK)) 
        return getRookMoves(board, pieceIndex);
    if ((board[pieceIndex] == W_KNIGHT || board[pieceIndex] == B_KNIGHT)) 
        return getKnightMoves(board, pieceIndex);
    if ((board[pieceIndex] == W_BISHOP || board[pieceIndex] == B_BISHOP))
        return getBishopMoves(board, pieceIndex); 
    if ((board[pieceIndex] == W_QUEEN || board[pieceIndex] == B_QUEEN))
        return getQueenMoves(board, pieceIndex);
    if ((board[pieceIndex] == W_KING || board[pieceIndex] == B_KING))
        return getKingMoves(board, pieceIndex);
    return NULL;
}

Positions* getPawnMoves(unsigned char* board, int pieceIndex) {
    int movesIndex = 0;
    unsigned char piece = board[pieceIndex];
    int *moves = malloc(sizeof(int) * 8);
    memset(moves, 0, 8 * sizeof(int));
    int colour = IS_WHITE(piece);
    

    int direction = BLACK_DIRECTION;
    if (colour == WHITE)
        direction = WHITE_DIRECTION;
    
    if IN_BOUNDS(pieceIndex + direction * 8) {
        // Checks if next square is vacant.
        if(board[pieceIndex + direction * 8] == EMPTY) {
            moves[movesIndex] = pieceIndex + direction * 8;
            movesIndex++;

            // Checks if a pawn hasn't mpved yet and can double jump
            int doubleMovePos = pieceIndex + 2 * (direction * 8);
            if(colour == WHITE){
                if(HAS_RANK(2, pieceIndex) && board[doubleMovePos] == EMPTY){
                    moves[movesIndex] = pieceIndex + 2 * (direction * 8);
                    movesIndex++;
                }
            }else{
                if(HAS_RANK(7, pieceIndex) && board[doubleMovePos] == EMPTY){
                    moves[movesIndex] = pieceIndex + 2 * (direction * 8);
                    movesIndex++;
                }
            }
        }

    }

    int canTakeLeft = 1;
    int canTakeRight = 1;

    if(GET_FILE(pieceIndex) == 1){
        if(colour == WHITE){
            canTakeLeft = 0;
        }else{
            canTakeRight = 0;
        }
    }

    if(GET_FILE(pieceIndex) == 8){
        if(colour == WHITE){
            canTakeRight = 0;
        }else{
            canTakeLeft = 0;
        }
    }

    if(colour == WHITE) {
        if(canTakeRight) {
            if(IS_BLACK(board[pieceIndex - 8 + 1])){
                moves[movesIndex] = pieceIndex - 8 + 1;
                movesIndex++;
            }
        }

        if(canTakeLeft) {
            if(IS_BLACK(board[pieceIndex - 8 - 1])){
                moves[movesIndex] = pieceIndex - 8 - 1;
                movesIndex++;
            }
        }
    }else{
        if(canTakeRight) {
            if(IS_WHITE(board[pieceIndex + 8 - 1])){
                moves[movesIndex] = pieceIndex + 8 - 1;
                movesIndex++;
            }
        }

        if(canTakeLeft) {
            if(IS_WHITE(board[pieceIndex + 8 + 1])){
                moves[movesIndex] = pieceIndex + 8 + 1;
                movesIndex++;
            }
        }
    }

    Positions *positions = (Positions *)malloc(sizeof(Positions));
    positions->positions = moves;
    positions->length = movesIndex;

    return positions;
}

Positions* getRookMoves(unsigned char* board, int pieceIndex) {
    int movesIndex = 0;
    int *moves = malloc(sizeof(int) * 16);
    memset(moves, 0, 16 * sizeof(int));
    int colour = IS_WHITE(board[pieceIndex]);

    // We don't care what colour the piece is because rooks can move in any adjacent direction
    // Up and Down
    int i = 1;
    while(GET_RANK(pieceIndex - 8 * i) <= 8){
        if(board[pieceIndex - 8 * i] == EMPTY){
            moves[movesIndex] = pieceIndex - 8 * i;
            movesIndex++;
            i++;
        }
        else {
            if(!colour == IS_WHITE(board[pieceIndex - 8 * i])){
                moves[movesIndex] = pieceIndex - 8 * i;
                movesIndex++;
            }
            break;
        }
    }
 
    i = 1;
    while(GET_RANK(pieceIndex + 8 * i) >= 1){
        if(board[pieceIndex + 8 * i] == EMPTY){
            moves[movesIndex] = pieceIndex + 8 * i;
            movesIndex++;
            i++;
        }else{
            if(!colour == IS_WHITE(board[pieceIndex + 8 * i])){
                moves[movesIndex] = pieceIndex + 8 * i;
                movesIndex++;
            }
            break;
        }
    }

    // Left and Right
    int rank = GET_RANK(pieceIndex);
    i = 1;
    while((pieceIndex - i) >= 0 && GET_RANK(pieceIndex - i) == rank){
        if(board[pieceIndex - i] == EMPTY) {
            moves[movesIndex] = pieceIndex - i;
            movesIndex++;
            i++;
        }else{
            if(!colour == IS_WHITE(board[pieceIndex - i])){
                moves[movesIndex] = pieceIndex - i;
                movesIndex++;
            }
            break;
        }
    }
    
    i = 1;
    while((pieceIndex + i) <= 63 && GET_RANK(pieceIndex + i) == rank){
        if(board[pieceIndex + i] == EMPTY) {
            moves[movesIndex] = pieceIndex + i;
            movesIndex++;
            i++;
        }else{
            if(!colour == IS_WHITE(board[pieceIndex + i])){
                moves[movesIndex] = pieceIndex + i;
                movesIndex++;
            }
            break;
        }
    }
    
    Positions *positions = (Positions *)malloc(sizeof(Positions));
    positions->positions = moves;
    positions->length = movesIndex;

    return positions;
}

Positions* getKnightMoves(unsigned char* board, int pieceIndex) {
    int movesIndex = 0;
    int *moves = malloc(sizeof(int) * 8);
    memset(moves, 0, 8 * sizeof(int));
    int colour = IS_WHITE(board[pieceIndex]);
    
    int file = GET_FILE(pieceIndex);
    int rank = GET_RANK(pieceIndex);
    
    if (file - 1 >= 1){
        if(rank + 2 >= 1 && rank + 2 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file - 1, rank + 2)]) || board[F_AND_R_TO_INDEX(file - 1, rank + 2)] == EMPTY)){
            moves[movesIndex] = F_AND_R_TO_INDEX(file - 1, rank + 2);
            movesIndex++;
        }
        if(rank - 2 >= 1 && rank - 2 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file - 1, rank - 2)]) || board[F_AND_R_TO_INDEX(file - 1, rank - 2)] == EMPTY)){
            moves[movesIndex] = F_AND_R_TO_INDEX(file - 1, rank - 2);
            movesIndex++;
        }

        if (file - 2 >= 1){
            if(rank + 1 >= 1 && rank + 1 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file - 2, rank + 1)]) || board[F_AND_R_TO_INDEX(file - 2, rank + 1)] == EMPTY)){
                moves[movesIndex] = F_AND_R_TO_INDEX(file - 2, rank + 1);
                movesIndex++;
            }
            if(rank - 1 >= 1 && rank - 1 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file - 2, rank - 1)]) || board[F_AND_R_TO_INDEX(file - 2, rank - 1)] == EMPTY)){
                moves[movesIndex] = F_AND_R_TO_INDEX(file - 2, rank - 1);
                movesIndex++;
            }
        }
    }

    if (file + 1 <= 8){
        if(rank - 2 >= 1 && rank - 2 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file + 1, rank - 2)]) || board[F_AND_R_TO_INDEX(file + 1, rank - 2)] == EMPTY)){
            moves[movesIndex] = F_AND_R_TO_INDEX(file + 1, rank - 2);
            movesIndex++;
        }
        if(rank + 2 >= 1 && rank + 2 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file + 1, rank + 2)]) || board[F_AND_R_TO_INDEX(file + 1, rank + 2)] == EMPTY)){
            moves[movesIndex] = F_AND_R_TO_INDEX(file + 1, rank + 2);
            movesIndex++;
        }

        if (file + 2 <= 8){
            if(rank - 1 >= 1 && rank - 1 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file + 2, rank - 1)]) || board[F_AND_R_TO_INDEX(file + 2, rank - 1)] == EMPTY)){
                moves[movesIndex] = F_AND_R_TO_INDEX(file + 2, rank - 1);
                movesIndex++;
            }
            if(rank + 1 >= 1 && rank + 1 <= 8 && (!colour == IS_WHITE(board[F_AND_R_TO_INDEX(file + 2, rank + 1)]) || board[F_AND_R_TO_INDEX(file + 2, rank + 1)] == EMPTY)){
                moves[movesIndex] = F_AND_R_TO_INDEX(file + 2, rank + 1);
                movesIndex++;
            }
        }
    }
        
    Positions *positions = (Positions *)malloc(sizeof(Positions));
    positions->positions = moves;
    positions->length = movesIndex;

    return positions;
}

Positions* getBishopMoves(unsigned char* board, int pieceIndex) {
    int file = GET_FILE(pieceIndex);
    int rank = GET_RANK(pieceIndex);
    
    int movesIndex = 0;
    int *moves = malloc(sizeof(int) * 16);
    memset(moves, 0, 16 * sizeof(int));
    int colour = IS_WHITE(board[pieceIndex]);

    int i = 1;
    int currentSquare = F_AND_R_TO_INDEX(file + i, rank + i);
    while(IN_BOUNDS(currentSquare) && FILE_IN_BOUNDS(file + i) && RANK_IN_BOUNDS(rank + i)){
        if(board[currentSquare] != EMPTY){  
            if(!colour == IS_WHITE(board[currentSquare])){
                moves[movesIndex] = currentSquare;
                movesIndex++;
            }
            break;
        }else{
            moves[movesIndex] = currentSquare;
            movesIndex++;
            currentSquare -= 7; // Top Right
            i++;
        }
    }

    i = 1;
    currentSquare = F_AND_R_TO_INDEX(file - i, rank + i);
    while(IN_BOUNDS(currentSquare) && FILE_IN_BOUNDS(file - i) && RANK_IN_BOUNDS(rank + i)){
        if(board[currentSquare] != EMPTY){  
            if(!colour == IS_WHITE(board[currentSquare])){
                moves[movesIndex] = currentSquare;
                movesIndex++;
            }
            break;
        }else{
            moves[movesIndex] = currentSquare;
            movesIndex++;
            currentSquare -= 9; // Top Left
            i++;
        }
    }

    i = 1;
    currentSquare = F_AND_R_TO_INDEX(file + i, rank - i);
    while(IN_BOUNDS(currentSquare) && FILE_IN_BOUNDS(file + i) && RANK_IN_BOUNDS(rank - i)){
        if(board[currentSquare] != EMPTY){  
            if(!colour == IS_WHITE(board[currentSquare])){
                moves[movesIndex] = currentSquare;
                movesIndex++;
            }
            break;
        }else{
            moves[movesIndex] = currentSquare;
            movesIndex++;
            currentSquare += 9; // Top Right
            i++;
        }
    }

    i = 1;
    currentSquare = F_AND_R_TO_INDEX(file - i, rank - i);
    while(IN_BOUNDS(currentSquare) && FILE_IN_BOUNDS(file - i) && RANK_IN_BOUNDS(rank - i)){
        if(board[currentSquare] != EMPTY){  
            if(!colour == IS_WHITE(board[currentSquare])){
                moves[movesIndex] = currentSquare;
                movesIndex++;
            }
            break;
        }else{
            moves[movesIndex] = currentSquare;
            movesIndex++;
            currentSquare += 7; // Top Right
            i++;
        }
    }

    Positions *positions = (Positions *)malloc(sizeof(Positions));
    positions->positions = moves;
    positions->length = movesIndex;

    return positions;
}

Positions* getQueenMoves(unsigned char* board, int pieceIndex) {
    // Queen is just rook + bishop
    Positions* rookMoves = getRookMoves(board, pieceIndex);
    Positions* bishopMoves = getBishopMoves(board, pieceIndex);

    // Initialise
    Positions *positions = (Positions *)malloc(sizeof(Positions));
    positions->length = rookMoves->length + bishopMoves->length;
    int* moves = malloc(sizeof(int) * (positions->length));
    positions->positions = moves;

    // Copy the rook and bishop moves into the queen moves
    memset(moves, 0, sizeof(int) * (positions->length));
    memcpy(moves, rookMoves->positions, rookMoves->length * sizeof(int));
    memcpy(moves + rookMoves->length, bishopMoves->positions, bishopMoves->length * sizeof(int));    
    
    //free the rook and bishop moves
    free(rookMoves->positions);
    free(rookMoves);
    free(bishopMoves->positions);
    free(bishopMoves);

    return positions;
}

Positions* getKingMoves(unsigned char* board, int pieceIndex) {
    int file = GET_FILE(pieceIndex);
    int rank = GET_RANK(pieceIndex);

    int movesIndex = 0;
    int *moves = malloc(sizeof(int) * 8);
    memset(moves, 0, 8 * sizeof(int));
    int colour = IS_WHITE(board[pieceIndex]);

    int index = 0;
    for(int i = 0; i < 8; i++){
        if(IN_BOUNDS(index = pieceIndex + positionOffsets[i])){
            if(abs(GET_FILE(index) - file) <= 1 && abs(GET_RANK(index) - rank) <= 1) {
                if(board[index] != EMPTY){  
                    if(!colour == IS_WHITE(board[index]) && board[index] != W_KING && board[index] != B_KING){
                        moves[movesIndex] = index;
                        movesIndex++;
                    }
                }else{
                    moves[movesIndex] = index;
                    movesIndex++;
                }
            }
        }  
    }

    Positions *positions = (Positions *)malloc(sizeof(Positions));
    positions->positions = moves;
    positions->length = movesIndex;

    return positions;
}