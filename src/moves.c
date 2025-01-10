#include "chess.h"

uint64_t knightMoveTable[64];

void generateKnightMoveTable(){
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

void knightMoves(Bitboard knights, uint16_t *moves, uint8_t *moveNumber) {
    uint8_t i = 0;
    while (knights) {
        i = __builtin_ffsll(knights) - 1;
        Bitboard attackingSquares = knightMoveTable[i];
        
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            
            // Store the move (from square + to square)
            moves[*moveNumber] = (uint16_t)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            
            // Pop the attacking squares bitboards LS1B
            attackingSquares &= attackingSquares - 1;
        }

        // Pop the knight bitboards LS1B
        knights &= knights - 1;
    }

    for (int i = 0; i < *moveNumber; i++){
        printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6); 
    }
}

