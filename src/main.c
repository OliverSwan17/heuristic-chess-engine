#include "chess.h"

int main(int argc, char* argv[]) {
    //uint8_t* board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    //uint8_t* board = fenToArray("2P1PP2/p1Pp3p/2p1P1PP/3P1p1p/Pp6/p1P4P/PP1ppPP1/8");
    generateKnightLookupTable();
    uint8_t* board = fenToArray("NNNNNNNN/nnnnnnnn/NNNNNNNN/NNNNnnNN/NNNNnnnN/NNNNNNNN/NNNNnnnn/NNNNNNNN");
    uint64_t highlightedSquares = 0;
    
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
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        uint8_t squareSelectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                        highlightedSquares = getTargetSquares(board, squareSelectionIndex);
                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);
        drawSquares(renderer);
        drawHighlightedSquares(highlightedSquares, renderer);
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