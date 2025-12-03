#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include <time.h>
#include "types.h"
#include "fen.h"
#include "draw.h"
#include "moves.h"
#include "perft.h"

static int terminalInput(Board *board, u16 *moves, u8 *moveNumber, Bitboard *attackingSquares);

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

int main(int argc, char* argv[]) {
    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

    Board board;
    fenToBoard(fen, &board);
    printf("Board size: %llu\n", sizeof(board));
    
    initMoveTables();
    initMailbox(&board);

    /*
    for (int depth = 1; depth <= 8; depth++) {
        clock_t start = clock();
        u64 nodes = perft(depth, board, WHITE);
        clock_t end = clock();
        double time_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
        printf("Depth %d: %llu nodes in %.2f ms\n", depth, nodes, time_ms);
    }

    return 0;
    */

    u8 moveNumber = 0;
    u16 moves[256];
    memset(moves, 0, 256 * sizeof(u16));
    Bitboard attackingSquares = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Event e;
    initSDL(&window, &renderer);

    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = 1;
                }
            }
        }

        SDL_RenderClear(renderer);
            drawSquares(renderer);
            drawPieces(renderer, &board);
            drawHighlightedSquares(attackingSquares, renderer);
            drawNumbers(renderer);
        SDL_RenderPresent(renderer);
        
        if (terminalInput(&board, moves, &moveNumber, &attackingSquares))
            break;
    }

    return 0;
}

static int terminalInput(Board *board, u16 *moves, u8 *moveNumber, Bitboard *attackingSquares) {
    char buffer[32];
    memset(&buffer, 0, 32);
    fgets(buffer, sizeof(buffer), stdin);

    if (buffer[0] == 'q')
        return 1;

    if (buffer[0] == 'm'){
        buffer[3] = '\0';
        u8 index = atoi(&buffer[1]);

        *attackingSquares = 0;
        *moveNumber = 0;
        memset(moves, 0, 256 * sizeof(u16));
        
        genPseudoLegalMoves(board, moves, moveNumber, WHITE);
        for (int i = 0; i < *moveNumber; i++) {
            if ((moves[i] & 0b111111) == index) {
                *attackingSquares |= (1ULL << ((moves[i] & 0b111111000000) >> 6));
                //printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6);
            }
        }
    } else {
        buffer[2] = '\0';
        buffer[5] = '\0';

        u8 srcSquare = atoi(&buffer[0]);
        u8 dstSquare = atoi(&buffer[3]);

        for (int i = 0; i < 12; i++) {
            if (board->pieces[i] & (1ULL << srcSquare)) {
                board->pieces[i] &= ~(1ULL << srcSquare);
                board->pieces[i] |= (1ULL << dstSquare);

                board->wPieces = board->pieces[W_PAWN] | board->pieces[W_KNIGHT] | board->pieces[W_BISHOP] | board->pieces[W_ROOK] | board->pieces[W_QUEEN] | board->pieces[W_KING];
                board->bPieces = board->pieces[B_PAWN] | board->pieces[B_KNIGHT] | board->pieces[B_BISHOP] | board->pieces[B_ROOK] | board->pieces[B_QUEEN] | board->pieces[B_KING];
                break;
            }
        }
    }

    if (buffer[0] == 'l') {
        *moveNumber = 0;
        genPseudoLegalMoves(board, moves, moveNumber, WHITE);
        
        for (int i = 0; i < *moveNumber; i++) {
            u8 from = moves[i] & 0x3F;
            u8 to = (moves[i] >> 6) & 0x3F;
            *attackingSquares = (1ULL << from) | (1ULL << to);

            for (int i = 0; i < 100; i++) {
                SDL_RenderClear(renderer);
                drawSquares(renderer);
                drawPieces(renderer, board);
                drawHighlightedSquares(*attackingSquares, renderer);
                drawNumbers(renderer);
                SDL_RenderPresent(renderer);
            }
        
            for (int j = 0; j < 100; j++) {
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT || 
                        (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                        return 1;
                    }
                }
                usleep(10000);  // 10ms * 100 = 1 second total
            }
        }
    }
    

    return 0;
}
