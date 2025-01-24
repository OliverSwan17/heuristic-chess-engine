#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>

//Screen Constants
#define SCREEN_LENGTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_SIZE (int) SCREEN_LENGTH / 8

//Misc
#define FILE(i) (((i) % 8) + 1)
#define RANK(i) (i / 8 + 1)
#define WHITE (0)
#define BLACK (1)

// Bitboards
typedef uint64_t Bitboard;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


typedef struct {
    Bitboard pieces[12];
    Bitboard wPieces;
    Bitboard bPieces;
} Board;

enum PieceType {
    W_PAWN,
    W_KNIGHT,
    W_BISHOP,
    W_ROOK,
    W_QUEEN,
    W_KING,
    B_PAWN,
    B_KNIGHT,
    B_BISHOP,
    B_ROOK,
    B_QUEEN,
    B_KING,
};

//Fen
void fenToBoard(char *fen, Board *board);
extern const unsigned char pieceLookupTable[256];

//Draw
void initRectangles();
void initPiecesTexture(SDL_Renderer* renderer);
void initNumbersTextures(SDL_Renderer* renderer);

void drawSquares(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, Board *board);
void drawHighlightedSquares(Bitboard squares, SDL_Renderer* renderer);
void drawNumbers(SDL_Renderer* renderer);

// Attack Maps
void generateKnightAttackMap();
void generateKingAttackMap();
void generatePawnAttackMap();
void generateRookBlockerMask();
u64 generateRandomU64();

void knightMoves(Bitboard knights, Bitboard colouredPieces, u16 *moves, u8 *moveNumber);
void kingMoves(Bitboard kings, Bitboard colouredPieces, u16 *moves, u8 *moveNumber);
void pawnMoves(Bitboard pawns, Bitboard colouredPieces, u16 *moves, u8 *moveNumber, u8 colour);
void rookMoves(Bitboard rooks, Bitboard allPieces, Bitboard friendlyColour, u16 *moves, u8 *moveNumber);

void printMoves(u16 *moves, u8 moveNumber);

// Main
void getMoves(Board *board, u16 *moves, u8 *moveNumber);
int terminalInput(Board *board, u16 *moves, u8 *moveNumber, Bitboard *attackingSquares);

