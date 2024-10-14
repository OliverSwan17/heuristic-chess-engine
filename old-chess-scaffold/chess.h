#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>
#include <assert.h>

//Screen Constants
#define SCREEN_LENGTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (int) SCREEN_LENGTH / 8

//Piece Constants
#define EMPTY 0
#define BLACK 0 
#define WHITE 1

#define B_PAWN 1
#define B_KNIGHT 2
#define B_BISHOP 3
#define B_ROOK 4
#define B_QUEEN 5
#define B_KING 6

#define W_PAWN 7
#define W_KNIGHT 8
#define W_BISHOP 9
#define W_ROOK 10
#define W_QUEEN 11
#define W_KING 12

//Orientation
#define WHITE_DIRECTION -1
#define BLACK_DIRECTION 1

//Macros
#define IS_BLACK(x) ((x) >= 1 && (x) <= 6)
#define IS_WHITE(x) ((x) >= 7 && (x) <= 12)
#define IN_BOUNDS(x) ((x) >= 0 && (x) < 64)
#define MOUSE_TO_SQUARE_INDEX(x, y) (((x) / 100) + ((y) / 100) * 8)
#define HAS_RANK(r, i) ((64 - 8 * (r)) <= (i) &&  (i) <= (71 - 8 * (r)))
#define GET_FILE(i) (((i) % 8) + 1)
#define GET_RANK(i) (abs(((i) + 1 + 8 - 1) / 8 - 9))
#define F_AND_R_TO_INDEX(f, r) ((8 - (r)) * 8 + (f) - 1)
#define FILE_IN_BOUNDS(x) (x >= 1 && x <= 8)
#define RANK_IN_BOUNDS(x) (x >= 1 && x <= 8)

//Structs 
typedef struct {
    int* positions;
    int length;
} Positions;

//Fen
unsigned char* fenToArray(char* fen);
void printBoard(unsigned char *board);
void testFENParsing();
extern const unsigned char pieceLookupTable[256];

//Draw
void initRectangles();
void drawSquares(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, unsigned char* board, int rotation);
void initPiecesTexture(SDL_Renderer* renderer);
void drawHighlightedSquares(Positions* positions, SDL_Renderer* renderer);

//Logic 
Positions* getMoves(unsigned char* board, int pieceIndex);
Positions* getPawnMoves(unsigned char* board, int pieceIndex);
Positions* getRookMoves(unsigned char* board, int pieceIndex);
Positions* getKnightMoves(unsigned char* board, int pieceIndex);
Positions* getBishopMoves(unsigned char* board, int pieceIndex);
Positions* getQueenMoves(unsigned char* board, int pieceIndex);
Positions* getKingMoves(unsigned char* board, int pieceIndex);
extern const int positionOffsets[8];

//Game
void movePiece(unsigned char* board, Positions* moves, int pieceIndex, int targetIndex);