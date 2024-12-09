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

int main(int argc, char* argv[]) {
    // State
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    // rnbqkbnr/pppppppp/8/8/8/8/8/4K3
    uint8_t* board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/8/P3K3");
    uint8_t turn = WHITE;

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
    uint64_t highlightedSquares = 0;
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
                        if (COLOUR(board[srcSelectionIndex]) != turn || board[srcSelectionIndex] == EMPTY){
                            highlightedSquares = 0;
                            break;
                        }
                            

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

                            if (((board[selectorSelectionIndex] & 0b111) == PAWN) && (RANK(selectorSelectionIndex) == 1 || RANK(selectorSelectionIndex) == 8)){
                                if(COLOUR(board[selectorSelectionIndex]) == WHITE)
                                    board[selectorSelectionIndex] = W_QUEEN;
                                else
                                    board[selectorSelectionIndex] = B_QUEEN;
                            }

                            #ifdef NETWORKING
                            #ifdef _WIN32
                            SetEvent(hEvent);
                            #endif
                            #endif

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
                                        }else if (getColourLegalMoves(board, turn) == 0){
                                            printf("Stalemate!\n");
                                        }
                                    }
                                }
                            }
                        
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