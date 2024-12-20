#include "chess.h"

#ifdef _WIN32
SOCKET client_sock;
struct sockaddr_in server_addr;
HANDLE hEvent;
#else
pthread_t thread;
int client_sock;
struct sockaddr_in server_addr;
#endif

static uint8_t selectionIndex;
static uint8_t captureIndex;
static uint64_t highlightedSquares;
static uint8_t selectorState;

int main(int argc, char* argv[]) {
    generateKnightLookupTable();

    BoardState s;
    //s.board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    s.board = fenToArray("rp2kpqr/8/8/8/8/8/8/RP2KPQR");
    s.turn = WHITE;
    s.castlingSquares = 0;
    s.wKingIndex = 60;
    s.bKingIndex = 4;

    highlightedSquares = 0;
    selectionIndex = 0;
    captureIndex = 0;
    selectorState = 0;

    #ifdef NETWORKING
    #ifdef _WIN32
    WSADATA wsaData;
    HANDLE thread;
    DWORD threadId;

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    thread = CreateThread(NULL, 0, move_sync_thread, (LPVOID)board, 0, &threadId);
    
    if (thread == NULL) {
        printf("Thread creation failed.\n");
        closesocket(client_sock);
        WSACleanup();
        return 1;
    }
    #else
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    pthread_create(&thread, NULL, move_sync_thread, (void*)board);
    #endif
    #endif

    if (SDL_Init(SDL_INIT_VIDEO) != 0) { goto error;}
    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_LENGTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) { goto error;}
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) { goto error;}

    initRectangles();
    initPiecesTexture(renderer);
    initSelector(renderer);

    SDL_Event e;
    
    while(1) {
        while (SDL_PollEvent(&e) != 0) {
            if(handleEvents(e, &s) == 1)
                goto end;
        }

        SDL_RenderClear(renderer);
        drawSquares(renderer);
        drawHighlightedSquares(highlightedSquares, renderer);
        drawPieces(renderer, s.board, WHITE_DIRECTION);
        drawSelector(renderer, captureIndex, selectorState);
        SDL_RenderPresent(renderer);
    }

    end:
    #ifdef NETWORKING
    #ifdef _WIN32
    CloseHandle(thread);
    closesocket(client_sock);
    WSACleanup();
    #else
    close(client_sock);
    #endif
    #endif

    SDL_DestroyWindow(window);
    SDL_Quit();
    free(s.board);
    return 0;

    error:
    SDL_DestroyWindow(window);
    printf("ERROR! %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

int handleEvents(SDL_Event e, BoardState *s){
    switch (e.type) {
        case SDL_QUIT:
            return 1;
        case SDL_KEYDOWN:
            if(e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_ESCAPE){
                    return 1;
                }
            }
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT){
                return handleSelection(e, s);
            }
            else if (e.button.button == SDL_BUTTON_RIGHT){
                return handleMove(e, s);
            }
            break;
    }
    return 0;
}

int handleSelection(SDL_Event e, BoardState *s){
    s->castlingSquares = 0;
    selectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
    if (COLOUR(s->board[selectionIndex]) != s->turn || s->board[selectionIndex] == EMPTY){
        highlightedSquares = 0;
        return 0;
    }

    captureIndex = selectionIndex;
    highlightedSquares = getLegalMoves(s->board, selectionIndex);

    // Check for castle
    if ((KING == (s->board[selectionIndex] & 0b111)) && COLOUR(s->board[selectionIndex]) == s->turn){
        uint64_t oppositeColourTargetSquares = getColourTargetSquares(s->board, !s->turn);
        s->castlingSquares = getCastlingSquares(s->board, s->turn, oppositeColourTargetSquares);
        highlightedSquares |= s->castlingSquares;
    }

    selectorState = 0;

    return 0;
}

int handleMove(SDL_Event e, BoardState *s){
    if (selectorState == 1)
        return 0;

    captureIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
    uint64_t legalSquares = getLegalMoves(s->board, selectionIndex);

    // First check for castling
    if ((s->castlingSquares) && (s->castlingSquares & (1ULL << captureIndex))){
        if (s->turn == WHITE && selectionIndex == 60){
            if (captureIndex == 62){
                s->board[captureIndex] = s->board[60];
                s->board[selectionIndex] = EMPTY;
                s->board[61] = s->board[63];
                s->board[63] = EMPTY;
            }else if (captureIndex == 58){
                s->board[captureIndex] = s->board[60];
                s->board[selectionIndex] = EMPTY;
                s->board[59] = s->board[56];
                s->board[56] = EMPTY;
            }
        }else if (s->turn == BLACK && selectionIndex == 4){
            if (captureIndex == 6){
                s->board[captureIndex] = s->board[4];
                s->board[selectionIndex] = EMPTY;
                s->board[5] = s->board[7];
                s->board[7] = EMPTY;
            }else if (captureIndex == 2){
                s->board[captureIndex] = s->board[4];
                s->board[selectionIndex] = EMPTY;
                s->board[3] = s->board[0];
                s->board[0] = EMPTY;
            }
        }
    }else if (!((legalSquares & (1ULL << captureIndex)))){
        captureIndex = selectionIndex;
        return 0;
    }else{ // Normal move
        s->board[captureIndex] = s->board[selectionIndex];
        s->board[selectionIndex] = EMPTY;
    }

    highlightedSquares = 0;
    selectorState = 1;

    // Checking for promotion
    if (((s->board[captureIndex] & 0b111) == PAWN) && (RANK(captureIndex) == 1 || RANK(captureIndex) == 8)){
        if(COLOUR(s->board[captureIndex]) == WHITE)
            s->board[captureIndex] = W_QUEEN;
        else
            s->board[captureIndex] = B_QUEEN;
    }

    #ifdef NETWORKING
    #ifdef _WIN32
    SetEvent(hEvent);
    #endif
    #endif

    // If the rook just moved, set the 5th bit to indicate that the piece has been moved
    if ((ROOK == (s->board[captureIndex] & 0b111)) || (KING == (s->board[captureIndex] & 0b111))){
        s->board[captureIndex] |= 1ULL << 5;
    }
    
    // Updating the King indexs in the boardstate
    if (KING == (s->board[captureIndex] & 0b111)){
        if (s->turn == WHITE)
            s->wKingIndex = captureIndex;
        else
            s->bKingIndex = captureIndex;
    }

    // Calculating the current colours attacking squares
    uint64_t attackingSquares = getColourLegalMoves(s->board, s->turn);

    // If the king opposite king has no moves, check if it is under attack for checkmate
    // else check if there are no legal moves for stalemate.

    // This switch helps to check for stalemate and checkmate.
    // It also switches the turn as the checkmate / stalemate is the last check in this function.
    s->turn = s->turn ^ 1;
    uint8_t tempKingIndex = s->turn ? s->wKingIndex : s->bKingIndex;
    
    if (getLegalMoves(s->board, tempKingIndex) == 0) {
        if (attackingSquares & (1ULL << tempKingIndex)){
            if (getColourLegalMoves(s->board, s->turn))
                return 0;

            printf("Checkmate!\n");
            return 1;
        }else if (getColourLegalMoves(s->board, s->turn) == 0){
            printf("Stalemate!\n");
            return 1;
        }
    }
        
    return 0;
}