#pragma once

#include "types.h"
#include "board.h"

void fenToBoard(char *fen, Board *board);
extern const unsigned char pieceLookupTable[256];