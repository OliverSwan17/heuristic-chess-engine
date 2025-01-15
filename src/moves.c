#include "chess.h"

Bitboard knightAttackMap[64];
Bitboard kingAttackMap[64];
Bitboard pawnAttackMap[2][64];
Bitboard rookBlockerMask[64];

void generateKnightAttackMap() {
    Bitboard squares = 0;
    u8 rank = 0;
    u8 file = 0;

    for (int i = 0; i < 64; i++) {
        squares = 0;
        rank = RANK(i);
        file = FILE(i);
        
        if (rank >= 6) {
            if (file != 1)
                squares |= (1ULL << (i - 17));
            if (file != 8)
                squares |= (1ULL << (i - 15));
        }

        if (rank <= 3) {
            if (file != 1)
                squares |= (1ULL << (i + 15));
            if (file != 8)
                squares |= (1ULL << (i + 17));
        }

        if (file <= 6) {
            if (rank >= 7)
                squares |= (1ULL << (i - 6));

            if (rank <= 2)
                squares |= (1ULL << (i + 10));
        }

        if (file >= 3) {
            if (rank >= 7)
                squares |= (1ULL << (i - 10));

            if (rank <= 2)
                squares |= (1ULL << (i + 6));
        }

        knightAttackMap[i] = squares;
    }
}

void generateKingAttackMap() {
    for (int i = 0; i < 64; i++) {
        Bitboard kingSquares = 0;

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

void generatePawnAttackMap() {
    Bitboard aFile = 0b100000001000000010000000100000001000000010000000100000001;
    Bitboard hFile = 0b1000000010000000100000001000000010000000100000001000000010000000;

    for (int i = 0; i < 64; i++) {
        if (i <= 7 || i >= 56)
            continue;

        if (!((1ULL << i) & aFile))
            pawnAttackMap[BLACK][i] |= (1ull << (i + 7));
        if (!((1ULL << i) & hFile))
            pawnAttackMap[BLACK][i] |= (1ull << (i + 9));
    }

    for (int i = 0; i < 64; i++) {
        if (i <= 7 || i >= 56)
            continue;

        if (!((1ULL << i) & aFile))
            pawnAttackMap[WHITE][i] |= (1ull << (i - 9));
        if (!((1ULL << i) & hFile))
            pawnAttackMap[WHITE][i] |= (1ull << (i - 7));
    }
}

void generateRookBlockerMask() {
    for (int i = 0; i < 64; i++) {
        
        int j = i;
        int file = FILE(j);
        if (!(file == 1 || file == 2)) {
            while (FILE(j) != 2) {
                j--;
                rookBlockerMask[i] |= (1ULL << j);
            }
        }

        j = i;
        file = FILE(j);
        if (!(file == 7 || file == 8)) {
            while (FILE(j) != 7) {
                j++;
                rookBlockerMask[i] |= (1ULL << j);
            }
        }

        j = i;
        int rank = RANK(j);
        if (!(rank == 7 || rank == 8)) {
            while (RANK(j) != 7) {
                j += 8;
                rookBlockerMask[i] |= (1ULL << j);
            }
        }

        j = i;
        rank = RANK(j);
        if (!(rank == 1 || rank == 2)) {
            while (RANK(j) != 2) {
                j -= 8;
                rookBlockerMask[i] |= (1ULL << j);
            }
        }

    }


    for (int i = 0; i < 64; i++) {
        printf("%llu\n", rookBlockerMask[i]);
    }
}


void knightMoves(Bitboard knights, Bitboard friendlyColour, uint16_t *moves, uint8_t *moveNumber) {
    u8 i = 0;
    while (knights) {
        i = __builtin_ffsll(knights) - 1;
        Bitboard attackingSquares = knightAttackMap[i] &~ friendlyColour;
        while (attackingSquares) {
            u8 j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }
        knights &= knights - 1;
    }
}

void kingMoves(Bitboard kings, Bitboard friendlyColour, u16 *moves, u8 *moveNumber) {
    uint8_t i = 0;
    while (kings) {
        i = __builtin_ffsll(kings) - 1;
        Bitboard attackingSquares = kingAttackMap[i] &~ friendlyColour;
        while (attackingSquares) {
            u8 j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }
        kings &= kings - 1;
    }
}

void pawnMoves(Bitboard pawns, Bitboard friendlyColour, u16 *moves, u8 *moveNumber, u8 colour) {
    uint8_t i = 0;
    while (pawns) {
        i = __builtin_ffsll(pawns) - 1;
        Bitboard attackingSquares = pawnAttackMap[colour][i] &~ friendlyColour;
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }
        pawns &= pawns - 1;
    }
}

void printMoves(u16 *moves, u8 moveNumber) {
     for (int i = 0; i < moveNumber; i++) {
        printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6); 
    }
}

