#include "chess.h"

int main(int argc, char* argv[]) {
    //unsigned char* board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    unsigned char* board = fenToArray("rnbqkbnr/pp2pppp/8/8/1p1p4/2P5/PP1PPPPP/RNBQKBNR");
    printBoard(board);
    uint64_t pawnSquares = getPawnSquares(board, 42);

    for(int i = 0; i < 64; i++){
        if(i % 8 == 0)
            printf("\n");
        if((1ULL << i) & pawnSquares)
            printf("X");
        else
            printf("-");
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
        drawPieces(renderer, board, WHITE_DIRECTION);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    free(board);
    return 0;

    error:
    SDL_DestroyWindow(window);
    printf("ERROR! %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}