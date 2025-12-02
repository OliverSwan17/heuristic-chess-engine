#pragma once

#include <SDL2/SDL.h>
#include "types.h"
#include "board.h"

#define SCREEN_LENGTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (int) SCREEN_LENGTH / 8

void initSDL(SDL_Window** window, SDL_Renderer** renderer);

void drawSquares(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, Board *board);
void drawHighlightedSquares(Bitboard squares, SDL_Renderer* renderer);
void drawNumbers(SDL_Renderer* renderer);
