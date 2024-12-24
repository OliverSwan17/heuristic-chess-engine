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

static uint64_t highlightedSquares;

int main(int argc, char* argv[]) {
    generateKnightLookupTable();

    uint8_t selectionIndex = 0;
    uint8_t captureIndex = 0;

    BoardState s;
    s.board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    s.turn = WHITE;
    s.castlingSquares = 0;
    s.wKingIndex = 60;
    s.bKingIndex = 4;
    s.halfMoves = 0;

    BoardState *t = malloc(sizeof(BoardState));
    t->board = fenToArray("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R");
    t->turn = WHITE;
    t->castlingSquares = 0;
    t->wKingIndex = 60;
    t->bKingIndex = 5;
    t->halfMoves = 0;

    //t->board[t->bKingIndex] = t->board[t->bKingIndex] & 0b01111;
    
    uint64_t count = 0;
    calculateNumberOfMoves(t, 4, &count);
    printf("Number of positions: %llu\n", count);
    

    highlightedSquares = 0;
    selectionIndex = 0;
    captureIndex = 0;

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
    SDL_Window *window = NULL;
    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_LENGTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) { goto error;}
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) { goto error;}

    initRectangles();
    initPiecesTexture(renderer);
    initSelector(renderer);

    SDL_Event e;
    
    while(1) {
        while (SDL_PollEvent(&e) != 0) {
            if(handleEvents(e, &s, &selectionIndex, &captureIndex) == 1)
                goto end;
        }

        SDL_RenderClear(renderer);
        drawSquares(renderer);
        drawHighlightedSquares(highlightedSquares, renderer);
        drawPieces(renderer, s.board, WHITE_DIRECTION);
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

int handleEvents(SDL_Event e, BoardState *s, uint8_t *selectionIndex, uint8_t *captureIndex){
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
                *selectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                return handleSelection(s, *selectionIndex, *captureIndex);
            }
            else if (e.button.button == SDL_BUTTON_RIGHT){
                *captureIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
                uint8_t result = handleMove(s, *selectionIndex, *captureIndex);
                if (result == 2)
                    return 0;
                highlightedSquares = 0;
                return result;
            }
            break;
    }
    return 0;
}

int handleSelection(BoardState *s, uint8_t selectionIndex, uint8_t captureIndex){
    s->castlingSquares = 0;
    if (COLOUR(s->board[selectionIndex]) != s->turn || s->board[selectionIndex] == EMPTY){
        highlightedSquares = 0;
        return 0;
    }

    captureIndex = selectionIndex;
    highlightedSquares = getLegalMoves(s->board, selectionIndex);

    // Check for castle
    if ((KING == (s->board[selectionIndex] & 0b111)) && COLOUR(s->board[selectionIndex]) == s->turn){
        s->castlingSquares = getCastlingSquares(s->board, s->turn);
        highlightedSquares |= s->castlingSquares;
    }

    return 0;
}

int handleMove(BoardState *s, uint8_t selectionIndex, uint8_t captureIndex){
    if (s->turn != COLOUR(s->board[selectionIndex]) || s->board[selectionIndex] == EMPTY)
        return 2;

    // Clearing the en passant bits
    for (int i = 0; i < 64; i++){
        if ((PAWN == (s->board[i] & 0b111)) && COLOUR(s->board[i]) == s->turn)
            s->board[i] &= ~0b00100000;
    }

    // Calculating the castling bits
    s->castlingSquares = getCastlingSquares(s->board, s->turn);

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
    }else if (!((getLegalMoves(s->board, selectionIndex) & (1ULL << captureIndex)))){
        return 2;
    }else if ((PAWN == (s->board[selectionIndex] & 0b111)) && (FILE(selectionIndex) != FILE(captureIndex)) && (s->board[captureIndex] == EMPTY)){
        if (s->turn == WHITE){
            s->board[captureIndex] = s->board[selectionIndex];
            s->board[selectionIndex] = EMPTY;
            s->board[POSTERIOR_SQUARE(captureIndex, WHITE_DIRECTION)] = EMPTY;
        }else{
            s->board[captureIndex] = s->board[selectionIndex];
            s->board[selectionIndex] = EMPTY;
            s->board[POSTERIOR_SQUARE(captureIndex, BLACK_DIRECTION)] = EMPTY;
        }
        s->halfMoves = 0;
    }
    else { // Normal move
        if (s->board[captureIndex] != EMPTY || PAWN == (s->board[selectionIndex] & 0b111))
            s->halfMoves = 0;
        else
            s->halfMoves += 1;
        
        s->board[captureIndex] = s->board[selectionIndex];
        s->board[selectionIndex] = EMPTY;
    }

    // Checking for promotion
    if (((s->board[captureIndex] & 0b111) == PAWN) && (RANK(captureIndex) == 1 || RANK(captureIndex) == 8)){
        if(COLOUR(s->board[captureIndex]) == WHITE)
            s->board[captureIndex] = W_QUEEN;
        else
            s->board[captureIndex] = B_QUEEN;
    }

    // Setting en passant bit
    if (PAWN == (s->board[captureIndex] & 0b111)){
        if (abs(((int) selectionIndex) - ((int)captureIndex)) == 16){
            s->board[captureIndex] |= 0b100000;
        }
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
            //printf("Checkmate!\n");
            return 1;
        }else if (getColourLegalMoves(s->board, s->turn) == 0){
            //printf("Stalemate!\n");
            return 1;
        }
    }

    if (s->halfMoves == 100){
        printf("Draw by 50 move rule!\n");
        return 1;
    }
        
    return 0;
}

void calculateNumberOfMoves(BoardState *s, uint8_t depth, uint64_t *count){
    if (depth == 0){
        *count += 1;
        return;
    }

    for (int selectionIndex = 0; selectionIndex < 64; selectionIndex++){
        if ((COLOUR(s->board[selectionIndex]) != s->turn) || (s->board[selectionIndex] == EMPTY)){
            continue;
        }

        uint64_t attackingSquares = getLegalMoves(s->board, selectionIndex);
        if (KING == (s->board[selectionIndex] & 0b111))
            attackingSquares |= getCastlingSquares(s->board, s->turn);
        
        for (int captureIndex = 0; captureIndex < 64; captureIndex++){
            if (attackingSquares & (1ULL << captureIndex)){
                BoardState *newPosition = malloc(sizeof(BoardState));
            
                memcpy(newPosition, s, sizeof(BoardState));
                newPosition->board = malloc(64);
                memcpy(newPosition->board, s->board, 64);

                if(handleMove(newPosition, selectionIndex, captureIndex) != 1){
                    calculateNumberOfMoves(newPosition, depth - 1, count);
                }

                free(newPosition);
            }
        
        }

    }

    return;
}
