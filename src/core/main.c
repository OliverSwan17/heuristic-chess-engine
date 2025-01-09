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

    s.halfMoves = 0;
    s.moves = malloc(256 * sizeof(Move));
    s.numberOfLegalmoves = 0;
    
    for (int i = 0; i < 64; i++){
        if (COLOUR(s.board[i]) == WHITE && (s.board[i] & 0b111) == KING)
            s.wKingIndex = i;
        else if (COLOUR(s.board[i]) == BLACK && (s.board[i] & 0b111) == KING)
            s.bKingIndex = i;
    }

    //printf("%llu\n", generate(&s, 3));

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

                highlightedSquares = 0;

                return handleMove(s, move);
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

    if (s->moves != NULL){
        free(s->moves);
    }
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

    // If the rook or king just moved, set the 5th bit to indicate that the piece has been moved
    if ((ROOK == ((s->board[move->dstSquare]) & 0b111)) || (KING == ((s->board[move->dstSquare]) & 0b111))){
        printf("Nigger");
        s->board[move->dstSquare] |= 0b10000;
    }
    
    // Updating the King indexs in the boardstate
    if (KING == (s->board[move->dstSquare] & 0b111)){
        if (s->turn == WHITE)
            s->wKingIndex = move->dstSquare;
        else
            s->bKingIndex = move->dstSquare;
    }

    BoardState nextBoard;
    memcpy(&nextBoard, s, sizeof(BoardState));
    nextBoard.moves = NULL;
    nextBoard.turn = !nextBoard.turn;
    legalMoves(&nextBoard);

    uint8_t kingSquare = (s->turn == 0) ? s->bKingIndex : s->wKingIndex;
    if (nextBoard.numberOfLegalmoves == 0){
        for (int i = 0; i < s->numberOfLegalmoves; i++){
            if ((1ULL << kingSquare) & s->moves[i].dstSquare){
                //printf("Checkmate!");
                s->turn = !s->turn;
                return 1;
            }
        }
        //printf("Stalemate");
        s->turn = !s->turn;
        return 1;
    }
    
    free(nextBoard.moves);
    s->turn = !s->turn;
    return 0;
}

uint64_t generate(BoardState* s, uint64_t depth) {
    if (depth == 0) {
        return 1;
    }

    legalMoves(s);
    uint64_t numberPos = 0;

    // Create a deep copy of the current BoardState
    BoardState* copy = malloc(sizeof(BoardState));
    copy->board = malloc(64);
    memcpy(copy->board, s->board, 64);

    copy->moves = malloc(256 * sizeof(Move));
    memcpy(copy->moves, s->moves, 256 * sizeof(Move));

    copy->turn = s->turn;
    copy->castlingSquares = s->castlingSquares;
    copy->wKingIndex = s->wKingIndex;
    copy->bKingIndex = s->bKingIndex;
    copy->halfMoves = s->halfMoves;
    copy->numberOfLegalmoves = s->numberOfLegalmoves;

    for (int i = 0; i < s->numberOfLegalmoves; i++) {
        // Apply the move
        handleMove(s, &s->moves[i]);

        // Temporary variable to store positions for this move
        uint64_t movePos = generate(s, depth - 1);

        // Add to the total number of positions
        numberPos += movePos;

        // Restore the original state
        memcpy(s->board, copy->board, 64);
        memcpy(s->moves, copy->moves, 256 * sizeof(Move));
        s->turn = copy->turn;
        s->castlingSquares = copy->castlingSquares;
        s->wKingIndex = copy->wKingIndex;
        s->bKingIndex = copy->bKingIndex;
        s->halfMoves = copy->halfMoves;
        s->numberOfLegalmoves = copy->numberOfLegalmoves;

        //Print positions for the current move at depth 5
        if (depth == 3)
            printf("Move %c%d%c%d: %llu positions\n",
        'a' + (s->moves[i].srcSquare % 8), 8 - (s->moves[i].srcSquare / 8),
        'a' + (s->moves[i].dstSquare % 8), 8 - (s->moves[i].dstSquare / 8),
        movePos);
    }

    // Free allocated memory for the copy
    free(copy->board);
    free(copy->moves);
    
    return numberPos;
}

