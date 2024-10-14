#include "chess.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) { goto error;}
    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_LENGTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) { goto error;}
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) { goto error;}


    // Pawn Test r3k2r/p1pnp1pp/N1Q3B1/3p1p2/1N2RbRB/1pq1b1n1/PPPPPPPP/4K3
    // Rook Test r7/2r1rr2/1R5r/3Rr3/1r5r/2r2r2/6r1/1R6
    // Knight Test NNNNNNNN/NNNNNNNN/NNNNNNNN/NNNNNNNN/NNNNNNNN/NNNNNNNN/NNNNNNNN/NNNNNNNN
    // Random b3B1n1/3Pnp2/6p1/2p2PBk/2P5/1P1pP1p1/Pp1R4/1K6
    // Bishop Test 2B4B/4bb2/b2b1b2/2B2b2/6bb/bBBbbB2/8/1B1bB3
    // Queen Test Q3Q1Q1/2q5/1q3q2/4Q2Q/Q1q4q/5q2/q1Q3Q1/3q4
    // King Test k2r2b1/4k3/k1K3kK/2k1K1k1/k5k1/2K1kk2/2K4K/K3K1KK
    // Starting Position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    unsigned char* board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    printBoard(board);
    Positions* selectionPositions = NULL;
    testFENParsing();

    SDL_Event e;
    int quit = 0;
    int squareSelectionIndex = 0;
    int targetSquareIndex = 0;

    initRectangles();
    initPiecesTexture(renderer);

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
                        squareSelectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                        if (selectionPositions){
                            free(selectionPositions->positions);
                            free(selectionPositions);
                        }
                        selectionPositions = getMoves(board, squareSelectionIndex);
                    }else if(e.button.button == SDL_BUTTON_RIGHT) {
                        if (selectionPositions){
                            targetSquareIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                            movePiece(board, selectionPositions, squareSelectionIndex, targetSquareIndex);
                            free(selectionPositions->positions);
                            free(selectionPositions);
                            selectionPositions = NULL;
                        }
                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);

        drawSquares(renderer);
        if (selectionPositions && selectionPositions->length)
            drawHighlightedSquares(selectionPositions, renderer);
        drawPieces(renderer, board, WHITE_DIRECTION);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
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
