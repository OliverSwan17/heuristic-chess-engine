#include "chess.h"

int main(int argc, char* argv[]) {
    char *fen = "3nr1Q1/2Prpq2/2B5/2nP4/N7/3P4/2P4p/2K1k3";
    Board board;
    fenToBoard(fen, &board);
    
    generateKnightAttackMap();
    generateKingAttackMap();
    generatePawnAttackMap();

    generateRookBlockerMask();

    u8 moveNumber = 0;
    u16 *moves = malloc(256 * sizeof(u16));
    memset(moves, 0, 256 * sizeof(u16));

    getMoves(&board, moves, &moveNumber);

    //printMoves(moves, moveNumber);

    Bitboard attackingSquares = 0;
    u8 pieceType = W_KNIGHT;
    for (int i = 0; i < moveNumber; i++){
        if (board.pieces[pieceType] & (1ULL << (moves[i] & 0b111111))){
            attackingSquares |= (1ULL << ((moves[i] & 0b111111000000) >> 6));
            printf("%u %u\n", moves[i] & 0b111111, (moves[i] & 0b111111000000) >> 6);
        }
    }

    
    extern Bitboard rookAttackMap[64][4096];
    extern Bitboard rookBlockerMask[64];
    extern u64 magics[64];
    extern u8 shifts[64];
    attackingSquares = rookAttackMap[61][(magics[61] * (rookBlockerMask[61] & (board.wPieces | board.bPieces))) >> shifts[61]];
    
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

void getMoves(Board *board, u16 *moves, u8 *moveNumber) {
    knightMoves(board->pieces[W_KNIGHT], board->wPieces, moves, moveNumber);
    knightMoves(board->pieces[B_KNIGHT], board->bPieces, moves, moveNumber);
    kingMoves(board->pieces[W_KING], board->wPieces, moves, moveNumber);
    kingMoves(board->pieces[B_KING], board->bPieces, moves, moveNumber);
    pawnMoves(board->pieces[W_PAWN], board->wPieces, moves, moveNumber, WHITE);
    pawnMoves(board->pieces[B_PAWN], board->bPieces, moves, moveNumber, BLACK);
}
