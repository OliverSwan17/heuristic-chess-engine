#include "chess.h"

int main(int argc, char* argv[]) {
    char *fen = "3b2BK/pRP1pb2/N1kP3p/2p2PPq/4rQ2/Pp3PRp/2pp1PP1/Nn1n2B1";
    Board board;
    fenToBoard(fen, &board);
    
    generateKnightAttackMap();
    generateKingAttackMap();
    generatePawnAttackMap();
    generateRookBlockerMask();

    u8 moveNumber = 0;
    u16 moves[256];
    memset(moves, 0, 256 * sizeof(u16));

    getMoves(&board, moves, &moveNumber);

    Bitboard attackingSquares = 0;
    u8 index = 42;
    for (int i = 0; i < moveNumber; i++) {
        if ((moves[i] & 0b111111) == index) {
            attackingSquares |= (1ULL << ((moves[i] & 0b111111000000) >> 6));
            printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6);
        }
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize variables
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Create SDL window
    window = SDL_CreateWindow(
        "Chess", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_LENGTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Initialize game resources
    initRectangles();
    initPiecesTexture(renderer);
    initNumbersTextures(renderer);

    // Main game loop
    SDL_Event e;
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
    }

    // Cleanup resources
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void getMoves(Board *board, u16 *moves, u8 *moveNumber) {
    knightMoves(board->pieces[W_KNIGHT], board->wPieces, moves, moveNumber);
    knightMoves(board->pieces[B_KNIGHT], board->bPieces, moves, moveNumber);
    kingMoves(board->pieces[W_KING], board->wPieces, moves, moveNumber);
    kingMoves(board->pieces[B_KING], board->bPieces, moves, moveNumber);
    pawnMoves(board->pieces[W_PAWN], board->wPieces, moves, moveNumber, WHITE);
    pawnMoves(board->pieces[B_PAWN], board->bPieces, moves, moveNumber, BLACK);
    rookMoves(board->pieces[W_ROOK], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    rookMoves(board->pieces[B_ROOK], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
}
