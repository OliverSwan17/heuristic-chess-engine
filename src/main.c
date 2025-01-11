#include "chess.h"

int main(int argc, char* argv[]) {
    Board board;
    memset(&board.pieces, 0, sizeof(Bitboard) * 12);
    board.pieces[W_PAWN]     = 0b0000000000000000000000000000000000000000000000001111111100000000;
    board.pieces[W_KNIGHT]   = 0b0000000000000000000000000000000000000000000000000000000001000010;
    board.pieces[W_BISHOP]   = 0b0000000000000000000000000000000000000000000000000000000000100100;
    board.pieces[W_ROOK]     = 0b0000000000000000000000000000000000000000000000000000000010000001;
    board.pieces[W_QUEEN]    = 0b0000000000000000000000000000000000000000000000000000000000010000;
    board.pieces[W_KING]     = 0b0000000000000000000000000000000000000000000000000000000000001000;
    board.wPieces = board.pieces[W_PAWN] | board.pieces[W_KNIGHT] | board.pieces[W_BISHOP] | board.pieces[W_ROOK] | board.pieces[W_QUEEN] | board.pieces[W_KING];

    board.pieces[B_PAWN]     = 0b0000000011111111000000000000000000000000000000000000000000000000;
    board.pieces[B_KNIGHT]   = 0b0100001000000000000000000000000000000000000000000000000000000000;
    board.pieces[B_BISHOP]   = 0b0010010000000000000000000000000000000000000000000000000000000000;
    board.pieces[B_ROOK]     = 0b1000000100000000000000000000000000000000000000000000000000000000;
    board.pieces[B_QUEEN]    = 0b0001000000000000000000000000000000000000000000000000000000000000;
    board.pieces[B_KING]     = 0b0000100000000000000000000000000000000000000000000000000000000000;
    board.bPieces = board.pieces[B_PAWN] | board.pieces[B_KNIGHT] | board.pieces[B_BISHOP] | board.pieces[B_ROOK] | board.pieces[B_QUEEN] | board.pieces[B_KING];

    generateKnightAttackMap();
    generateKingAttackMap();

    uint16_t *moves = malloc(256 * sizeof(uint16_t));
    memset(moves, 0, 256 * sizeof(uint16_t));


    board.pieces[W_KING] <<= 24;
    uint8_t numMoves = 0;
    kingMoves(board.pieces[W_KING], board.wPieces, moves, &numMoves);
    printMoves(moves, numMoves);

    Bitboard attackingSquares = 0;
    for (int i = 0; i < numMoves; i++){
        attackingSquares |= (1ULL << ((moves[i] & 0b111111000000) >> 6));
    }
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { goto error;}
    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_LENGTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) { goto error;}
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) { goto error;}

    initRectangles();
    initPiecesTexture(renderer);

    SDL_Event e;
    int quit = 0;

    while(!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    if(e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_ESCAPE) {
                            quit = 1;
                            break;
                        }
                    }
                break;
            }
        }

        SDL_RenderClear(renderer);
        drawSquares(renderer);
        drawPieces(renderer, &board);
        drawHighlightedSquares(attackingSquares, renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

    error:
    SDL_DestroyWindow(window);
    printf("ERROR! %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}