#include "chess.h"

Bitboard knightAttackMap[64];
Bitboard kingAttackMap[64];
Bitboard pawnAttackMap[2][64];
Bitboard rookBlockerMask[64];
Bitboard rookAttackMap[64][4096];

u64 rookMagics[64] = {36028936676704400ULL, 72095527461716226ULL, 36056422257070080ULL, 108103984559949824ULL, 10520417602942013452ULL, 10808647910376412176ULL, 
    864693332353230852ULL, 360290169854787840ULL, 1153062244246880300ULL, 9223442474320527936ULL, 9228016717574176896ULL, 1829656110041088ULL, 5909004255477432325ULL, 
    9296696818110824960ULL, 281526549878016ULL, 281477158097152ULL, 9223522120194473984ULL, 576531945683423232ULL, 576743327328637252ULL, 577164989769451520ULL, 594616438996599808ULL, 
    288512950674138112ULL, 11333765876945416ULL, 41678088303842305ULL, 2308412020280016899ULL, 117095515531772960ULL, 45352696425742464ULL, 2902712846385280ULL, 1298206577349914880ULL, 
    145522575844844544ULL, 4617333127328039170ULL, 8111551402175647876ULL, 36028934462112064ULL, 81064940399497216ULL, 92886881934525456ULL, 18031992851404804ULL, 19844106125576192ULL, 
    90635011253864484ULL, 17696674545954ULL, 178740466287649ULL, 158604571213824ULL, 5350418208274841632ULL, 108403063344136256ULL, 18093701893062672ULL, 2270491914076164ULL, 
    81628361855991828ULL, 2382406470757777421ULL, 1147855306756ULL, 579134834929058048ULL, 292874850710585472ULL, 72620820214219264ULL, 11542778630232899712ULL, 290273225867648ULL, 
    4536560257025ULL, 14411519943873020928ULL, 1189514420909507072ULL, 63966500570013953ULL, 2305913382397576209ULL, 576535590497945857ULL, 9367496158799200513ULL, 146930092730753058ULL, 
    18577627770785794ULL, 2308095393179631908ULL, 13855360134852904066ULL};

u8 rookShifts[64] = {
    52, 53, 53, 53, 53, 53, 53, 52,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    52, 53, 53, 53, 53, 53, 53, 52,
};

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
        Bitboard blockers = rookBlockerMask[i];
        u8 numBlockers = 0;
        for (int blockerIndex = 0; blockerIndex < 64; blockerIndex++) {
            if ((1ULL << blockerIndex) & blockers)
                numBlockers++;
        }

        for (u16 j = 0; j < (1 << numBlockers); j++) {
            Bitboard blockerCombo = 0;
            
            u32 tempBlockerComboNumber = j;
            for (int blockerIndex = 0; blockerIndex < 64; blockerIndex++) {
                if ((1ULL << blockerIndex) & blockers) {
                    if (tempBlockerComboNumber & 1) {
                        blockerCombo |= (1ULL << blockerIndex);
                    }
                    tempBlockerComboNumber >>= 1;
                }
            }

            u32 key = (rookMagics[i] * blockerCombo) >> rookShifts[i];
            
            int p = i;
            int file = FILE(p);
            if (!(file == 1)) {
                while (FILE(p) != 1 && !((1ULL << p) & blockerCombo)) {
                    p--;
                    rookAttackMap[i][key] |= (1ULL << p);
                }
            }

            p = i;
            file = FILE(p);
            if (!(file == 8)) {
                while (FILE(p) != 8 && !((1ULL << p) & blockerCombo)) {
                    p++;
                    rookAttackMap[i][key] |= (1ULL << p);
                }
            }

            p = i;
            int rank = RANK(p);
            if (!(rank == 8)) {
                while (RANK(p) != 8 && !((1ULL << p) & blockerCombo)) {
                    p += 8;
                    rookAttackMap[i][key] |= (1ULL << p);
                }
            }

            p = i;
            rank = RANK(p);
            if (!(rank == 1)) {
                while (RANK(p) != 1 && !((1ULL << p) & blockerCombo)) {
                    p -= 8;
                    rookAttackMap[i][key] |= (1ULL << p);
                }
            }
        }

    }

    /*
    u8 dummyLookup[4096];
    memset(&dummyLookup, 0, 4096);
    u8 done[64];
    memset(&done, 0, 64);


    for (int i = 0; i < 64; i++) {
        done[i] = 0;
    }
 
    for (int i = 0; i < 64; i++) {
        while (1) {
            if (done[i]){
                break;
            }

            u64 magic = generateRandomU64() & generateRandomU64() & generateRandomU64();

            Bitboard blockers = rookBlockerMask[i];
            u8 numBlockers = 0;
            for (int blockerIndex = 0; blockerIndex < 64; blockerIndex++) {
                if ((1ULL << blockerIndex) & blockers)
                    numBlockers++;
            }

            u8 targetShift = 64 - numBlockers;

            for (u32 blockerComboNumber = 0; blockerComboNumber < (1 << numBlockers); blockerComboNumber++) {
                u32 tempBlockerComboNumber = blockerComboNumber;
                Bitboard blockerCombo = 0;

                for (int blockerIndex = 0; blockerIndex < 64; blockerIndex++) {
                    if ((1ULL << blockerIndex) & blockers) {
                        if (tempBlockerComboNumber & 1) {
                            blockerCombo |= (1ULL << blockerIndex);
                        }
                        tempBlockerComboNumber >>= 1;
                    }
                }

                u32 key = (magic * blockerCombo) >> targetShift;
                if (dummyLookup[key])
                    goto tryNext;
                else
                    dummyLookup[key] = 1;
            }

            rookMagics[i] = magic;
            rookShifts[i] = targetShift;
            printf("Index: %d Magic: %llu Bits: %d\n", i, rookMagics[i], 64 - rookShifts[i]);
            done[i] = 1;

            tryNext:
            memset(&dummyLookup, 0, sizeof(u8) * 4096);
        }
    }
    */
}

u64 generateRandomU64() {
    static u64 seed = 0;
    if (seed == 0) {
        seed = (u64)time(NULL); 
    }

    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;
    return seed * 0x2545F4914F6CDD1DULL;
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

void rookMoves(Bitboard rooks, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber) {
    uint8_t i = 0;
    while (rooks) {
        i = __builtin_ffsll(rooks) - 1;
        Bitboard attackingSquares = rookAttackMap[i][(rookMagics[i] * (rookBlockerMask[i] & allPieces)) >> rookShifts[i]] &~ friendlyColour;
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }
        rooks &= rooks - 1;
    }
}

void printMoves(u16 *moves, u8 moveNumber) {
     for (int i = 0; i < moveNumber; i++) {
        printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6); 
    }
}

