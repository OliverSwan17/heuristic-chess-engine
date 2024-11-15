#include "chess.h"

int main(int argc, char* argv[]) {
    generateKnightLookupTable();
    uint8_t* board = fenToArray("rnbqk1nr/ppp2ppp/4p3/3p3Q/1b1P4/2N1P3/PPP2PPP/R1B1KBNR");
    uint64_t highlightedSquares = getColourTargetSquares(board, WHITE);
    uint8_t srcSelectionIndex = 0;
    uint8_t selectorSelectionIndex = 0;
    uint8_t selectorState = 0;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { goto error;}
    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_LENGTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) { goto error;}
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) { goto error;}

    initRectangles();
    initPiecesTexture(renderer);
    initSelector(renderer);

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
                        srcSelectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                        selectorSelectionIndex = srcSelectionIndex;
                        highlightedSquares = getLegalMoves(board, srcSelectionIndex);
                        selectorState = 0;
                    }else if(e.button.button == SDL_BUTTON_RIGHT){
                        if (selectorState == 1)
                            break;

                        selectorSelectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                        uint64_t legalSquares = getLegalMoves(board, srcSelectionIndex);
                        if (legalSquares & (1ULL << selectorSelectionIndex)){
                            highlightedSquares = 0;
                            selectorState = 1;
                            board[selectorSelectionIndex] = board[srcSelectionIndex];
                            board[srcSelectionIndex] = EMPTY;
                        }else{
                            selectorSelectionIndex = srcSelectionIndex;
                        }



                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);
        drawSquares(renderer);
        drawHighlightedSquares(highlightedSquares, renderer);
        drawPieces(renderer, board, WHITE_DIRECTION);
        drawSelector(renderer, selectorSelectionIndex, selectorState);
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