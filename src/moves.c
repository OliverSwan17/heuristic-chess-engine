#include <stdio.h>
#include "moves.h"

static Bitboard knightAttackMap[64];
static Bitboard kingAttackMap[64];
static Bitboard pawnAttackMap[2][64];
static Bitboard rookBlockerMask[64];
static Bitboard rookAttackMap[64][4096];
static Bitboard bishopBlockerMask[64];
static Bitboard bishopAttackMap[64][512];

static u64 rookMagics[64] = {36028936676704400ULL, 72095527461716226ULL, 36056422257070080ULL, 108103984559949824ULL, 10520417602942013452ULL, 10808647910376412176ULL, 
    864693332353230852ULL, 360290169854787840ULL, 1153062244246880300ULL, 9223442474320527936ULL, 9228016717574176896ULL, 1829656110041088ULL, 5909004255477432325ULL, 
    9296696818110824960ULL, 281526549878016ULL, 281477158097152ULL, 9223522120194473984ULL, 576531945683423232ULL, 576743327328637252ULL, 577164989769451520ULL, 594616438996599808ULL, 
    288512950674138112ULL, 11333765876945416ULL, 41678088303842305ULL, 2308412020280016899ULL, 117095515531772960ULL, 45352696425742464ULL, 2902712846385280ULL, 1298206577349914880ULL, 
    145522575844844544ULL, 4617333127328039170ULL, 8111551402175647876ULL, 36028934462112064ULL, 81064940399497216ULL, 92886881934525456ULL, 18031992851404804ULL, 19844106125576192ULL, 
    90635011253864484ULL, 17696674545954ULL, 178740466287649ULL, 158604571213824ULL, 5350418208274841632ULL, 108403063344136256ULL, 18093701893062672ULL, 2270491914076164ULL, 
    81628361855991828ULL, 2382406470757777421ULL, 1147855306756ULL, 579134834929058048ULL, 292874850710585472ULL, 72620820214219264ULL, 11542778630232899712ULL, 290273225867648ULL, 
    4536560257025ULL, 14411519943873020928ULL, 1189514420909507072ULL, 63966500570013953ULL, 2305913382397576209ULL, 576535590497945857ULL, 9367496158799200513ULL, 146930092730753058ULL, 
    18577627770785794ULL, 2308095393179631908ULL, 13855360134852904066ULL};

static u8 rookShifts[64] = {
    52, 53, 53, 53, 53, 53, 53, 52,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    52, 53, 53, 53, 53, 53, 53, 52,
};

static u64 bishopMagics[64] = {9372002953948594208ULL,567365246959627ULL,5088557129531920ULL,4613948891222507776ULL,2320484243488145408ULL,1171375727633777158ULL,282093655441520ULL,141046860481024ULL,72070925924565504ULL,4706017275316480ULL,1175738793249490944ULL,1153040269923385992ULL,144186726126977028ULL,329353227759786008ULL,1172136587564290312ULL,1155877559133602816ULL,5053038799156938820ULL,5066687087059456ULL,40542382483079424ULL,1195705718292875298ULL,292875142791463168ULL,2307109693862232064ULL,73482569715288576ULL,581668589741019264ULL,4692768542554063110ULL,181274283149894673ULL,54976386957376ULL,2892437960460863616ULL,2310353755733787648ULL,37298732982616576ULL,9259563638912717856ULL,576298716043264ULL,52389598862574092ULL,3567704263341113600ULL,7211393374389142016ULL,4611756421665554944ULL,1731654036912881920ULL,4792049924105699584ULL,6541549381567643968ULL,9150144381485184ULL,577624045273485828ULL,19181053629564928ULL,18051784857616896ULL,2499679361026ULL,4531500020142592ULL,5765171091513311616ULL,27235478579319360ULL,578998427296211456ULL,2522582108673966145ULL,845054411800899ULL,2547585638465860ULL,160538907119872ULL,4629721582672151616ULL,72062009399185920ULL,583236097712193538ULL,585648288657588232ULL,5188166596635657224ULL,72348157232744516ULL,11531466881521946624ULL,4971980587926817792ULL,1158623022439596552ULL,306262402851668481ULL,144185574453149960ULL,6935547858699486384ULL};
static u8 bishopShifts[64] = {58,59,59,59,59,59,59,58,59,59,59,59,59,59,59,59,59,59,57,57,57,57,59,59,59,59,57,55,55,57,59,59,59,59,57,55,55,57,59,59,59,59,57,57,57,57,59,59,59,59,59,59,59,59,59,59,58,59,59,59,59,59,59,58};

static void generateKnightAttackMap() {
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

static void generateKingAttackMap() {
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

static void generatePawnAttackMap() {
    Bitboard aFile = 0b100000001000000010000000100000001000000010000000100000001;
    Bitboard hFile = 0b1000000010000000100000001000000010000000100000001000000010000000;

    for (int i = 0; i < 64; i++) {
        if (i <= 7 || i >= 56)
            continue;

        if (!((1ULL << i) & aFile))
            pawnAttackMap[WHITE][i] |= (1ull << (i + 7));
        if (!((1ULL << i) & hFile))
            pawnAttackMap[WHITE][i] |= (1ull << (i + 9));
    }

    for (int i = 0; i < 64; i++) {
        if (i <= 7 || i >= 56)
            continue;

        if (!((1ULL << i) & aFile))
            pawnAttackMap[BLACK][i] |= (1ull << (i - 9));
        if (!((1ULL << i) & hFile))
            pawnAttackMap[BLACK][i] |= (1ull << (i - 7));
    }
}

static void generateRookBlockerMask() {
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
}

static void generateBishopBlockerMask() {
    for (int i = 0; i < 64; i++) {
        
        int j = i;
        int file = FILE(j);
        int rank = RANK(j);
        if (!(file == 1 || file == 2) && !(rank == 1 || rank == 2)) {
            while (FILE(j) != 2 && RANK(j) != 2) {
                j -= 9;
                bishopBlockerMask[i] |= (1ULL << j);
            }
        }

        j = i;
        file = FILE(j);
        rank = RANK(j);
        if (!(file == 7 || file == 8) && !(rank == 7 || rank == 8)) {
            while (FILE(j) != 7 && RANK(j) != 7) {
                j += 9;
                bishopBlockerMask[i] |= (1ULL << j);
            }
        }

        j = i;
        file = FILE(j);
        rank = RANK(j);
        if (!(rank == 7 || rank == 8) && !(file == 1 || file == 2)) {
            while (RANK(j) != 7 && FILE(j) != 2) {
                j += 7;
                bishopBlockerMask[i] |= (1ULL << j);
            }
        }

        j = i;
        file = FILE(j);
        rank = RANK(j);
        if (!(rank == 1 || rank == 2) && !(file == 7 || file == 8)) {
            while (RANK(j) != 2 && FILE(j) != 7) {
                j -= 7;
                bishopBlockerMask[i] |= (1ULL << j);
            }
        }
    }

    for (int i = 0; i < 64; i++) {
        Bitboard blockers = bishopBlockerMask[i];
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

            u32 key = (bishopMagics[i] * blockerCombo) >> bishopShifts[i];

            int p = i;
            int file = FILE(p);
            int rank = RANK(p);

            if (!(file == 1) && !(rank == 1)) {
                while (FILE(p) != 1 && RANK(p) != 1 && !((1ULL << p) & blockerCombo)) {
                    p -= 9;
                    bishopAttackMap[i][key] |= (1ULL << p);
                }
            }

            p = i;
            file = FILE(p);
            rank = RANK(p);
            if (!(file == 8) && !(rank == 8)) {
                while (FILE(p) != 8 && RANK(p) != 8 && !((1ULL << p) & blockerCombo)) {
                    p += 9;
                    bishopAttackMap[i][key] |= (1ULL << p);
                }
            }

            p = i;
            file = FILE(p);
            rank = RANK(p);
            if (!(rank == 8) && !(file == 1)) {
                while (RANK(p) != 8 && FILE(p) != 1 && !((1ULL << p) & blockerCombo)) {
                    p += 7;
                    bishopAttackMap[i][key] |= (1ULL << p);
                }
            }

            p = i;
            file = FILE(p);
            rank = RANK(p);
            if (!(rank == 1) && !(file == 8)) {
                while (RANK(p) != 1 && FILE(p) != 8 && !((1ULL << p) & blockerCombo)) {
                    p -= 7;
                    bishopAttackMap[i][key] |= (1ULL << p);
                }
            }
        }
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

void pawnMoves(Bitboard pawns, Bitboard enemyColour, u16 *moves, u8 *moveNumber, u8 colour) {
    uint8_t i = 0;
    while (pawns) {
        i = __builtin_ffsll(pawns) - 1;
        Bitboard attackingSquares = pawnAttackMap[colour][i] & enemyColour;
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }
        pawns &= pawns - 1;
    }
}

void whitePawnMoves(Bitboard whitePawns, Bitboard blackPieces, u16 *moves, u8 *moveNumber, Bitboard all) {
    uint8_t i = 0;
    while (whitePawns) {
        i = __builtin_ffsll(whitePawns) - 1;
        Bitboard attackingSquares = pawnAttackMap[WHITE][i] & blackPieces;
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }

        if ((1ULL << i) & 0xFF00) {
            if (~all & (1ULL << (i + 8))) {
                moves[*moveNumber] = (u16)((i & 0b111111) | (((i + 8) & 0b111111) << 6));
                (*moveNumber)++;
                if (~all & (1ULL << (i + 16))) {
                    moves[*moveNumber] = (u16)((i & 0b111111) | (((i + 16) & 0b111111) << 6)) | (1 << 13); // 1 << 13 for double push
                    (*moveNumber)++;
                }
            }
        }

        whitePawns &= whitePawns - 1;
    }
}

void blackPawnMoves(Bitboard blackPawns, Bitboard whitePieces, u16 *moves, u8 *moveNumber, Bitboard all) {
    uint8_t i = 0;
    while (blackPawns) {
        i = __builtin_ffsll(blackPawns) - 1;
        Bitboard attackingSquares = pawnAttackMap[WHITE][i] & BLACK;
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }

        if ((1ULL << i) & 0xFF000000000000) {
            if (~all & (1ULL << (i - 8))) {
                moves[*moveNumber] = (u16)((i & 0b111111) | (((i - 8) & 0b111111) << 6));
                (*moveNumber)++;
                if (~all & (1ULL << (i - 16))) {
                    moves[*moveNumber] = (u16)((i & 0b111111) | (((i - 16) & 0b111111) << 6)) | (1 << 13); // 1 << 13 for double push
                    (*moveNumber)++;
                }
            }
        }

        blackPawns &= blackPawns - 1;
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

void bishopMoves(Bitboard bishops, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber) {
    uint8_t i = 0;
    while (bishops) {
        i = __builtin_ffsll(bishops) - 1;
        Bitboard attackingSquares = bishopAttackMap[i][(bishopMagics[i] * (bishopBlockerMask[i] & allPieces)) >> bishopShifts[i]] &~ friendlyColour;
        while (attackingSquares) {
            uint8_t j = __builtin_ffsll(attackingSquares) - 1;
            moves[*moveNumber] = (u16)((i & 0b111111) | ((j & 0b111111) << 6));
            (*moveNumber)++;
            attackingSquares &= attackingSquares - 1;
        }
        bishops &= bishops - 1;
    }
}

void genPseudoLegalMoves(Board *board, u16 *moves, u8 *moveNumber) {
    Bitboard all;
    for (int i = 0; i < 12; i++) {
        all |= board->pieces[i];
    }

    knightMoves(board->pieces[W_KNIGHT], board->wPieces, moves, moveNumber);
    knightMoves(board->pieces[B_KNIGHT], board->bPieces, moves, moveNumber);
    kingMoves(board->pieces[W_KING], board->wPieces, moves, moveNumber);
    kingMoves(board->pieces[B_KING], board->bPieces, moves, moveNumber);
    
    whitePawnMoves(board->pieces[W_PAWN], board->bPieces, moves, moveNumber, all);
    blackPawnMoves(board->pieces[B_PAWN], board->wPieces, moves, moveNumber, all);

    rookMoves(board->pieces[W_ROOK], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    rookMoves(board->pieces[B_ROOK], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
    bishopMoves(board->pieces[W_BISHOP], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    bishopMoves(board->pieces[B_BISHOP], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);

    rookMoves(board->pieces[W_QUEEN], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    bishopMoves(board->pieces[W_QUEEN], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);

    rookMoves(board->pieces[B_QUEEN], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
    bishopMoves(board->pieces[B_QUEEN], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
    printf("Total Moves: %x\n", *moveNumber);
}

void initMoveTables() {
    generateKnightAttackMap();
    generateKingAttackMap();
    generatePawnAttackMap();
    generateRookBlockerMask();
    generateBishopBlockerMask();
}

/*
static void printMoves(u16 *moves, u8 moveNumber) {
     for (int i = 0; i < moveNumber; i++) {
        printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6); 
    }
}
*/
