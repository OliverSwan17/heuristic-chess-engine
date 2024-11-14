#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#ifdef _WIN32
    #include <SDL2/SDL_image.h>  // For Windows (MinGW/MSYS2)
#else
    #include <SDL_image.h>        // For Linux
#endif


//Piece Constants
#define EMPTY 0
#define BLACK 0 
#define WHITE 1

#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

// Misc
#define MOUSE_TO_SQUARE_INDEX(x, y) (((x) / 100) + ((y) / 100) * 8)
#define IS_EMPTY(index) !index
#define COLOUR(piece) ((piece >> 3) & 1)
#define IN_BOUNDS(x) ((x) >= 0 && (x) < 64)

// File and Rank
#define FILE(i) (((i) % 8) + 1)
#define RANK(i) (abs(((i) + 1 + 8 - 1) / 8 - 9))

//Directions
#define ANTERIOR_SQUARE(squareIndex, direction) (squareIndex + direction * 8)
#define SINISTRAL_SQAURE(squareIndex, direction) (squareIndex + direction)
#define DEXTRAL_SQUARE(squareIndex, direction) (squareIndex - direction)
#define POSTERIOR_SQUARE(squareIndex, direction) (squareIndex - direction * 8)
#define WHITE_DIRECTION (-1)
#define BLACK_DIRECTION (1)
#define COLOUR_DIRECTION(colour) ((colour) == 0 ? 1 : -1)

//Screen Constants
#define SCREEN_LENGTH (800)
#define SCREEN_HEIGHT (800)
#define SQUARE_SIZE (SCREEN_LENGTH / 8)

//Fen
extern uint8_t* fenToArray(char* fen);

//Draw
void initRectangles();
void drawSquares(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, uint8_t* board, int rotation);
void initPiecesTexture(SDL_Renderer* renderer);
void drawHighlightedSquares(uint64_t squares, SDL_Renderer* renderer);

//Moves
uint64_t getTargetSquares(uint8_t* board, uint8_t pieceIndex);
uint64_t pawnTargetSquares(uint8_t* board, uint8_t pieceIndex);
uint64_t kingTargetSquares(uint8_t* board, uint8_t pieceIndex);
uint64_t knightTargetSquares(uint8_t* board, uint8_t pieceIndex);
uint64_t bishopTargetSquares(uint8_t* board, uint8_t pieceIndex);
uint64_t rookTargetSquares(uint8_t* board, uint8_t pieceIndex);

//Lookup Tables
void generateKnightLookupTable();
