#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>

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

#define B_PAWN 1
#define B_KNIGHT 2
#define B_BISHOP 3
#define B_ROOK 4
#define B_QUEEN 5
#define B_KING 6

#define W_PAWN 9
#define W_KNIGHT 10
#define W_BISHOP 11
#define W_ROOK 12
#define W_QUEEN 13
#define W_KING 14

#define MOUSE_TO_SQUARE_INDEX(x, y) (((x) / 100) + ((y) / 100) * 8)
#define IS_EMPTY(index) !index

//Positional Macros
#define COLOUR(piece) ((piece & (1 << 3)) % 7)
#define COLOUR_DIRECTION(colour) ((colour) == 0 ? 1 : -1)

//Directions
#define ANTERIOR_SQUARE(squareIndex, direction) (squareIndex + direction * 8)
#define SINISTRAL_SQAURE(squareIndex, direction) (squareIndex + direction)
#define DEXTRAL_SQUARE(squareIndex, direction) (squareIndex - direction)
#define POSTERIOR_SQUARE(squareIndex, direction) (squareIndex - direction * 8)

//Orientation 
#define WHITE_DIRECTION (-1)
#define BLACK_DIRECTION (1)

//Screen Constants
#define SCREEN_LENGTH (800)
#define SCREEN_HEIGHT (800)
#define SQUARE_SIZE (SCREEN_LENGTH / 8)

//Fen
unsigned char* fenToArray(char* fen);
void printBoard(unsigned char *board);
extern const unsigned char pieceLookupTable[256];

//Draw
void initRectangles();
void drawSquares(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, unsigned char* board, int rotation);
void initPiecesTexture(SDL_Renderer* renderer);
void drawHighlightedSquares(uint64_t squares, SDL_Renderer* renderer);

//Moves
uint64_t getMoves(unsigned char* board, unsigned char pieceIndex);
uint64_t getPawnSquares(unsigned char* board, unsigned char pieceIndex);