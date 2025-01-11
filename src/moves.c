#include "chess.h"

uint64_t knightAttackMap[64];
uint64_t kingAttackMap[64];
uint64_t pawnAttackMap[2][64];

void generateKnightAttackMap() {
    uint64_t squares = 0;
    uint8_t rank = 0;
    uint8_t file = 0;

    for (int i = 0; i < 64; i++) {
        squares = 0;
        rank = RANK(i);
        file = FILE(i);
        
        if (rank <= 6) {
            if (file != 1)
                squares |= (1ULL << (i - 17));
            if (file != 8)
                squares |= (1ULL << (i - 15));
        }

        if (rank >= 3) {
            if (file != 1)
                squares |= (1ULL << (i + 15));
            if (file != 8)
                squares |= (1ULL << (i + 17));
        }

        if (file <= 6) {
            if (rank <= 7)
                squares |= (1ULL << (i - 6));

            if (rank >= 2)
                squares |= (1ULL << (i + 10));
        }

        if (file >= 3) {
            if (rank <= 7)
                squares |= (1ULL << (i - 10));

            if (rank >= 2)
                squares |= (1ULL << (i + 6));
        }

        knightAttackMap[i] = squares;
    }
}

void generateKingAttackMap() {
    for (int i = 0; i < 64; i++) {
        uint64_t kingSquares = 0;

        if (i < 56)
            kingSquares |= (1ULL << (i + 8));
        if (i > 7)
            kingSquares |= (1ULL << (i - 8));
        if (i % 8)
            kingSquares |= (1ULL << (i - 1));
        if (i % 8 != 0)
            kingSquares |= (1ULL << (i + 1));
        if (!(i >= 56 || ((i + 1) % 8 == 0)))
            kingSquares |= (1ULL << (i + 9));
        if (!(i >= 56 || ((i % 8 == 0))))
            kingSquares |= (1ULL << (i + 7));
        if (!(i <= 7 || ((i % 8 == 0))))
            kingSquares |= (1ULL << (i - 9));
        if (!(i <= 7 || (((i + 1) % 8 == 0))))
            kingSquares |= (1ULL << (i - 7));

        kingAttackMap[i] = kingSquares;
    }
}

void generatePawnAttackMap(){

}

void knightMoves(Bitboard knights, Bitboard colouredPieces, uint16_t *moves, uint8_t *moveNumber) {
    uint8_t i = 0;
    while (knights) {
        i = __builtin_ffsll(knights) - 1;
        Bitboard attackingSquares = knightAttackMap[i];
        
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            
            if (!((1ULL << j) & colouredPieces)) {
                // Store the move (from square + to square)
                moves[*moveNumber] = (uint16_t)((i & 0b111111) | ((j & 0b111111) << 6));
                (*moveNumber)++;
            }

            // Pop the attacking squares bitboards LS1B
            attackingSquares &= attackingSquares - 1;
        }

        // Pop the knight bitboards LS1B
        knights &= knights - 1;
    }
}

void kingMoves(Bitboard kings, Bitboard colouredPieces, uint16_t *moves, uint8_t *moveNumber) {
    uint8_t i = 0;
    while (kings) {
        i = __builtin_ffsll(kings) - 1;
        Bitboard attackingSquares = kingAttackMap[i];
        
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            
            if (!((1ULL << j) & colouredPieces)) {
                // Store the move (from square + to square)
                moves[*moveNumber] = (uint16_t)((i & 0b111111) | ((j & 0b111111) << 6));
                (*moveNumber)++;
            }

            // Pop the attacking squares bitboards LS1B
            attackingSquares &= attackingSquares - 1;
        }

        // Pop the king bitboards LS1B
        kings &= kings - 1;
    }
}

void printMoves(uint16_t *moves, uint8_t moveNumber) {
     for (int i = 0; i < moveNumber; i++) {
        printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6); 
    }
}

