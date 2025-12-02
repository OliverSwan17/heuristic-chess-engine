#include "chess.h"

int main(int argc, char* argv[]) {
    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    Board board;
    fenToBoard(fen, &board);
    printf("Board size: %llu\n", sizeof(board));
    
    generateKnightAttackMap();
    generateKingAttackMap();
    generatePawnAttackMap();
    generateRookBlockerMask();
    generateBishopBlockerMask();

    u8 moveNumber = 0;
    u16 moves[256];
    memset(moves, 0, 256 * sizeof(u16));
    Bitboard attackingSquares = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    window = SDL_CreateWindow(
        "Chess", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_LENGTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_SHOWN
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Init();
    initRectangles();
    initPiecesTexture(renderer);
    initNumbersTextures(renderer);

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
        
        if (terminalInput(&board, moves, &moveNumber, &attackingSquares))
            break;
    }

    return 0;
}

void getMoves(Board *board, u16 *moves, u8 *moveNumber) {
    knightMoves(board->pieces[W_KNIGHT], board->wPieces, moves, moveNumber);
    knightMoves(board->pieces[B_KNIGHT], board->bPieces, moves, moveNumber);
    kingMoves(board->pieces[W_KING], board->wPieces, moves, moveNumber);
    kingMoves(board->pieces[B_KING], board->bPieces, moves, moveNumber);
    pawnMoves(board->pieces[W_PAWN], board->bPieces, moves, moveNumber, WHITE);
    pawnMoves(board->pieces[B_PAWN], board->wPieces, moves, moveNumber, BLACK);
    rookMoves(board->pieces[W_ROOK], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    rookMoves(board->pieces[B_ROOK], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
    bishopMoves(board->pieces[W_BISHOP], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    bishopMoves(board->pieces[B_BISHOP], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);

    rookMoves(board->pieces[W_QUEEN], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);
    bishopMoves(board->pieces[W_QUEEN], board->wPieces | board->bPieces, board->wPieces, moves, moveNumber);

    rookMoves(board->pieces[B_QUEEN], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
    bishopMoves(board->pieces[B_QUEEN], board->wPieces | board->bPieces, board->bPieces, moves, moveNumber);
}

int terminalInput(Board *board, u16 *moves, u8 *moveNumber, Bitboard *attackingSquares) {
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
        
        getMoves(board, moves, moveNumber);
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
    

    return 0;
}