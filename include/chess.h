#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>

//Screen Constants
#define SCREEN_LENGTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (int) SCREEN_LENGTH / 8

//Fen
unsigned char* fenToArray(char* fen);
void printBoard(unsigned char *board);
extern const unsigned char pieceLookupTable[256];

//Draw
void initRectangles();
void drawSquares(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, unsigned char* board);
void initPiecesTexture(SDL_Renderer* renderer);

// Bitboards
typedef uint64_t Bitboard;

typedef struct {
    Bitboard pieces[2][6];
} ChessBoard;

enum PieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};