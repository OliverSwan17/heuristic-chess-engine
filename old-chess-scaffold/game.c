#include "chess.h"

void movePiece(unsigned char* board, Positions* moves, int pieceIndex, int targetIndex){
    // Ensures that the target square is in the array of possible moves.
    for(int i = 0; i < moves->length; i++) {
        if (moves->positions[i] == targetIndex){
            board[targetIndex] = board[pieceIndex];
            board[pieceIndex] = EMPTY;
            break;
        }
    }
}