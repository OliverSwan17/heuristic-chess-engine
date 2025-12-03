#include "board.h"
#include "string.h"

void initMailbox(Board *board) {
    memset(board->mailbox, NO_PIECE, 64);

    for (int bitboard = 0; bitboard < 12; bitboard++) {
        for (int index = 0; index < 64; index++) {
            if (board->pieces[bitboard] & (1ULL << index))
                board->mailbox[index] = bitboard;
        }
    }
}
