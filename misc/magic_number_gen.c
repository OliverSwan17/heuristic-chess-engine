// Code that was used in moves.c to generate magics.
// The magics and shift values are now hard coded.
// Therfore, this code is no longer needed.
// Left here for reference.

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
    