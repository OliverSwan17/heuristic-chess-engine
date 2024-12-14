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

static uint8_t* board;
static uint8_t turn;
static uint64_t castlingSquares;

static uint64_t highlightedSquares;
static uint8_t srcSelectionIndex;
static uint8_t selectorSelectionIndex;
static uint8_t selectorState;

int main(int argc, char* argv[]) {
    board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    turn = WHITE;
    castlingSquares = 0;

    highlightedSquares = 0;
    srcSelectionIndex = 0;
    selectorSelectionIndex = 0;
    selectorState = 0;
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    // rnbqkbnr/pppppppp/8/8/8/8/8/4K3

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

    generateKnightLookupTable();
    highlightedSquares = 0;
    srcSelectionIndex = 0;
    selectorSelectionIndex = 0;
    selectorState = 0;
    
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
            if(handleEvents(e) == 1)
                goto end;
        }

        SDL_RenderClear(renderer);
        drawSquares(renderer);
        drawHighlightedSquares(highlightedSquares, renderer);
        drawPieces(renderer, board, WHITE_DIRECTION);
        drawSelector(renderer, selectorSelectionIndex, selectorState);
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
    free(board);
    return 0;

    error:
    SDL_DestroyWindow(window);
    printf("ERROR! %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

int handleEvents(SDL_Event e){
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
                return handleSelection(e);
            }
            else if (e.button.button == SDL_BUTTON_RIGHT){
                return handleCapture(e);
            }
            break;
    }
    return 0;
}

int handleSelection(SDL_Event e){
    castlingSquares = 0;
    srcSelectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
    if (COLOUR(board[srcSelectionIndex]) != turn || board[srcSelectionIndex] == EMPTY){
        highlightedSquares = 0;
        return 0;
    }

    selectorSelectionIndex = srcSelectionIndex;
    highlightedSquares = getLegalMoves(board, srcSelectionIndex);

    // Check for castle
    if ((KING == (board[srcSelectionIndex] & 0b111)) && COLOUR(board[srcSelectionIndex]) == turn){
        uint64_t oppositeColourTargetSquares = getColourTargetSquares(board, !turn);
        castlingSquares = getCastlingSquares(board, turn, oppositeColourTargetSquares);
        highlightedSquares |= castlingSquares;
    }

    selectorState = 0;

    return 0;
}

int handleCapture(SDL_Event e){
    if (selectorState == 1)
        return 0;

    selectorSelectionIndex = MOUSE_TO_SQUARE_INDEX(e.button.x, e.button.y);
    uint64_t legalSquares = getLegalMoves(board, srcSelectionIndex);
    if (!((legalSquares & (1ULL << selectorSelectionIndex)) || castlingSquares)){
        selectorSelectionIndex = srcSelectionIndex;
        return 0;
    }
       
    highlightedSquares = 0;
    selectorState = 1;

    if (((board[selectorSelectionIndex] & 0b111) == PAWN) && (RANK(selectorSelectionIndex) == 1 || RANK(selectorSelectionIndex) == 8)){
        if(COLOUR(board[selectorSelectionIndex]) == WHITE)
            board[selectorSelectionIndex] = W_QUEEN;
        else
            board[selectorSelectionIndex] = B_QUEEN;
    }

    if (castlingSquares){ // Castling
        if (turn == WHITE){
            if ((castlingSquares & (1ULL << selectorSelectionIndex)) && srcSelectionIndex == 60){
                board[selectorSelectionIndex] = board[60];
                board[srcSelectionIndex] = EMPTY;
                board[61] = board[63];
                board[63] = EMPTY;
            }
        }else{

        }
    }else{ // Normal move
        board[selectorSelectionIndex] = board[srcSelectionIndex];
        board[srcSelectionIndex] = EMPTY;
    }

    #ifdef NETWORKING
    #ifdef _WIN32
    SetEvent(hEvent);
    #endif
    #endif

    if ((ROOK == (board[selectorSelectionIndex] & 0b111)) || (KING == (board[selectorSelectionIndex] & 0b111))){
        // Setting the 5th bit to indicate that the piece has been moved.
        board[selectorSelectionIndex] |= 1ULL << 5;
    }

    // Calculating the current colours attacking squares
    uint64_t attackingSquares = getColourLegalMoves(board, turn);

    // Switching the turn to the other coulour
    turn = turn ^ 1;

    // Search for the other colours king and checks if it has no moves.
    // If the king has no moves, check if it is under attack for checkmate
    // and check if there are no other moves (stalemate).
    for (int i = 0; i < 64; i++){
        if (KING == (board[i] & 0b111) && COLOUR(board[i]) == turn){
            if (getLegalMoves(board, i) == 0){
                if (attackingSquares & (1ULL << i)){
                    printf("Checkmate!\n");
                    return 1;
                }else if (getColourLegalMoves(board, turn) == 0){
                    printf("Stalemate!\n");
                    return 1;
                }
            }
        }
    }

    return 0;
}