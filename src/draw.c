#include "chess.h"

SDL_Rect* squaresRects[64];
SDL_Rect* piecesRects[12];
SDL_Texture* piecesTexture;

void initPiecesTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/pieces/pieces.png");
    if (surface == NULL) {printf("%s\n", IMG_GetError());}
    piecesTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (piecesTexture == NULL) {printf("%s\n", SDL_GetError());}
    SDL_FreeSurface(surface);
}

void initRectangles() {
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            squaresRects[y * 8 + x] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            if (!squaresRects[y * 8 + x])
                exit(1);

            squaresRects[y * 8 + x]->x = SQUARE_SIZE * x;
            squaresRects[y * 8 + x]->y = SQUARE_SIZE * y;
            squaresRects[y * 8 + x]->w = SQUARE_SIZE;
            squaresRects[y * 8 + x]->h = SQUARE_SIZE;
        }
    }

    // Would have been easier to put black first on the sprite sheet but oh well.
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            piecesRects[(1 - i) * 6 + j] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            piecesRects[(1 - i) * 6 + j]->x = 20 * j + 20 * 6 * i;
            piecesRects[(1 - i) * 6 + j]->y = 0;
            piecesRects[(1 - i) * 6 + j]->w = 20;
            piecesRects[(1 - i) * 6 + j]->h = 20;
        }
    }
}

// Need to add bitmasks for the pieces once we start using the other bits for stuff
void drawPieces(SDL_Renderer* renderer, uint8_t* board, int rotation) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            // The spritesheet contains all 12 piece types in a single row, where each piece type
            // has a unique value as defined in chess.h: pawn = 1, knight = 2, bishop = 3, and so on.
            // For white pieces, the value is offset by 8 (e.g., a white pawn has a value of 9).
            // This makes it easy to distinguish piece colors based on their value by checking the 3rd bit.
            // Therefore, when indexing the spritesheet for white pieces we need to subtract 2 from the value of the white piece.
            // For example, the 7th piece on the spritesheet is the white pawn but a white pawn is represented by 9.
            // So we subtract by 2. This works for all of the white pieces. 

            int spriteOffset = board[y * 8 + x] - 1 + ((board[y * 8 + x] & (1 << 3)) ? -2 : 0);
            SDL_Rect destinationRect = {x * SQUARE_SIZE, y * SQUARE_SIZE, 100, 100};
            SDL_RenderCopy(renderer, piecesTexture, piecesRects[spriteOffset], &destinationRect);
        }
    }
}

void drawSquares(SDL_Renderer* renderer) {
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            if ((y + x) % 2)
                SDL_SetRenderDrawColor(renderer, 177, 110, 65, 255);
            else
                SDL_SetRenderDrawColor(renderer, 255, 213, 153, 100);
            
            SDL_RenderFillRect(renderer, squaresRects[y * 8 + x]);
        }
    }
}

void drawHighlightedSquares(uint64_t squares, SDL_Renderer* renderer) {
    for(int i = 0; i < 64; i++) {
        if((1ULL << i) & squares){
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, squaresRects[i]);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
}

