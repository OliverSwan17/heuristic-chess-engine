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
    //s.board = fenToArray("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1");
    s.turn = WHITE;
    s.castlingSquares = 0;
    s.wKingIndex = 60;
    s.bKingIndex = 4;
    s.halfMoves = 0;
    s.moves = NULL;
    s.numberOfLegalmoves = 0;

    /*
    BoardState *t = malloc(sizeof(BoardState));
    t->board = fenToArray("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1");
    t->turn = WHITE;
    t->castlingSquares = 0;
    t->wKingIndex = 60;
    t->bKingIndex = 5;
    t->halfMoves = 0;

    t->board[t->bKingIndex] = t->board[t->bKingIndex] & 0b11111;
    t->board[t->wKingIndex] = t->board[t->wKingIndex] & 0b11111;
    
    uint64_t count = 0;
    calculateNumberOfMoves(t, 3, &count);
    printf("Number of positions: %llu\n", count);
    */
    
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
                
                Move *move = NULL;
                for (int i = 0; i < s->numberOfLegalmoves; i++){
                    if (s->moves[i].srcSquare == *selectionIndex && s->moves[i].dstSquare == *captureIndex){
                        move = &s->moves[i];
                        break;
                    }
                }

                if (move == NULL)
                    return 0;

                uint8_t result = handleMove(s, move); // Add functionality for this in the ui later.
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
    highlightedSquares = 0;
    if (COLOUR(s->board[selectionIndex]) != s->turn || s->board[selectionIndex] == EMPTY){
        return 0;
    }

    if (s->moves == NULL)
        legalMoves(s);

    captureIndex = selectionIndex;

    for (int i = 0; i < s->numberOfLegalmoves; i++){
        if (s->moves[i].srcSquare == selectionIndex)
            highlightedSquares |= (1ULL << s->moves[i].dstSquare);
    }
    
    // Check for castle
    if ((KING == (s->board[selectionIndex] & 0b111)) && COLOUR(s->board[selectionIndex]) == s->turn){
        highlightedSquares |= s->castlingSquares;
    }

    return 0;
}

int handleMove(BoardState *s, Move *move){
    if (s->turn != COLOUR(s->board[move->srcSquare]) || s->board[move->srcSquare] == EMPTY)
        return 2;

    printf("%u\n", move->type);

    // First check for castling
    if (move->type == KING_CASTLE){
        if (s->turn == WHITE){
            s->board[move->dstSquare] = s->board[60];
            s->board[move->srcSquare] = EMPTY;
            s->board[61] = s->board[63];
            s->board[63] = EMPTY;
        }else{
            s->board[move->dstSquare] = s->board[4];
            s->board[move->srcSquare] = EMPTY;
            s->board[5] = s->board[7];
            s->board[7] = EMPTY;
        }
    }else if (move->type == QUEEN_CASTLE){
        if (s->turn == WHITE){
            s->board[move->dstSquare] = s->board[60];
            s->board[move->srcSquare] = EMPTY;
            s->board[59] = s->board[56];
            s->board[56] = EMPTY;
        }else{
            s->board[move->dstSquare] = s->board[4];
            s->board[move->srcSquare] = EMPTY;
            s->board[3] = s->board[0];
            s->board[0] = EMPTY;
        }
    }

    else if (move->type == EN_PASSANT){
        if (s->turn == WHITE){
            s->board[move->dstSquare] = s->board[move->srcSquare];
            s->board[move->srcSquare] = EMPTY;
            s->board[POSTERIOR_SQUARE(move->dstSquare, WHITE_DIRECTION)] = EMPTY;
        }else{
            s->board[move->dstSquare] = s->board[move->srcSquare];
            s->board[move->srcSquare] = EMPTY;
            s->board[POSTERIOR_SQUARE(move->dstSquare, BLACK_DIRECTION)] = EMPTY;
        }
        s->halfMoves = 0;
    }
    else {
        if (s->board[move->dstSquare] != EMPTY || PAWN == (s->board[move->srcSquare] & 0b111))
            s->halfMoves = 0;
        else
            s->halfMoves += 1;
        
        s->board[move->dstSquare] = s->board[move->srcSquare];
        s->board[move->srcSquare] = EMPTY;
    }

    // Checking for promotion
    if (move->type & 0b1000){
        uint8_t colourOffset = 0;
        if(COLOUR(s->board[move->dstSquare]) == WHITE)
            colourOffset = 8;

        if ((move->type & 0b11) == 0b0)
            s->board[move->dstSquare] = KNIGHT + colourOffset;
        else if ((move->type & 0b11) == 0b01)
            s->board[move->dstSquare] = BISHOP + colourOffset;
        else if ((move->type & 0b11) == 0b10)
            s->board[move->dstSquare] = ROOK + colourOffset;
        else if ((move->type & 0b11) == 0b11)
            s->board[move->dstSquare] = QUEEN + colourOffset;
    }

    // Setting en passant bit if required
    if (move->type == DOUBLE_PAWN_PUSH){
        s->board[move->dstSquare] |= 0b100000;
    }
    
    #ifdef NETWORKING
    #ifdef _WIN32
    SetEvent(hEvent);
    #endif
    #endif

    // If the rook just moved, set the 5th bit to indicate that the piece has been moved
    if ((ROOK == (s->board[move->dstSquare] & 0b111)) || (KING == (s->board[move->dstSquare] & 0b111))){
        s->board[move->dstSquare] |= 1ULL << 5;
    }
    
    // Updating the King indexs in the boardstate
    if (KING == (s->board[move->dstSquare] & 0b111)){
        if (s->turn == WHITE)
            s->wKingIndex = move->dstSquare;
        else
            s->bKingIndex = move->dstSquare;
    }

    s->turn = !s->turn;
    s->moves = NULL;

    return 0;
}

/*
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
                if ((PAWN == (s->board[selectionIndex] & 0b111)) && (RANK(captureIndex) == 1 || RANK(captureIndex) == 8)){
                    for (int promotionType = 2; promotionType < 6; promotionType++){
                        BoardState *newPosition = malloc(sizeof(BoardState));
            
                        memcpy(newPosition, s, sizeof(BoardState));
                        newPosition->board = malloc(64);
                        memcpy(newPosition->board, s->board, 64);

                        if(handleMove(newPosition, selectionIndex, captureIndex, promotionType) == 0){
                            calculateNumberOfMoves(newPosition, depth - 1, count);
                        }

                        free(newPosition);
                    }
                }else{
                    BoardState *newPosition = malloc(sizeof(BoardState));
            
                    memcpy(newPosition, s, sizeof(BoardState));
                    newPosition->board = malloc(64);
                    memcpy(newPosition->board, s->board, 64);

                    if(handleMove(newPosition, selectionIndex, captureIndex, 0) == 0){
                        calculateNumberOfMoves(newPosition, depth - 1, count);
                    }

                    free(newPosition);
                }
            }
        
        }

    }

    return;
}
*/
