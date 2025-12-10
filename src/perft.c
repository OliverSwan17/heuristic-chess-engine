#include "perft.h"
#include "board.h"
#include "string.h"
#include "moves.h"
#include "stdio.h"

u32 perft(int depth, Board board, u8 colour) {
    if (depth == 0)
        return 1;

    MoveList moveList = {0};

    genPseudoLegalMoves(&board, &moveList, colour);
    
    u32 moveCount = 0;
    for (int i = 0; i < moveList.count; i++) {
        Board newBoard = board;
        u8 from = moveList.moves[i] & 0x3F;
        u8 to = (moveList.moves[i] >> 6) & 0x3F;

        u8 fromPiece = newBoard.mailbox[from];
        u8 toPiece = newBoard.mailbox[to];

        // printf("From: %u, To: %u, FromPiece %u, ToPiece %u\n", from, to, fromPiece, toPiece);
        newBoard.mailbox[from] = NO_PIECE;
        newBoard.mailbox[to] = fromPiece;

        if (toPiece != NO_PIECE) // Cause we have 12 bitboards no bitboard for empty squares
            newBoard.pieces[toPiece] &= ~(1ULL << to); // Captured square bitboard clear
        newBoard.pieces[fromPiece] &= ~(1ULL << from); // From square bitboard clear
        newBoard.pieces[fromPiece] |= (1ULL << to); // To Square bitboard set

        newBoard.wPieces = newBoard.pieces[0] | newBoard.pieces[1] | newBoard.pieces[2] | newBoard.pieces[3] | newBoard.pieces[4] | newBoard.pieces[5];
        newBoard.bPieces = newBoard.pieces[6] | newBoard.pieces[7] | newBoard.pieces[8] | newBoard.pieces[9] | newBoard.pieces[10] | newBoard.pieces[11];
        newBoard.all = newBoard.wPieces | newBoard.bPieces;

        u8 kingIndex = 0;
        
        for (int j = 0; j < 64; j++) {
            if ((1ULL << j) & newBoard.pieces[W_KING + 6 * colour]) {
                kingIndex = j;
            }
        }

        if (isUnderAttack(&newBoard, kingIndex, colour)){
            continue;
        } 
        moveCount += perft(depth - 1, newBoard, colour ^ 1);
    }

    return moveCount;
}
