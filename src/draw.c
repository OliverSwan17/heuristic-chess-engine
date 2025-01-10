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
            if (!squaresRects[y * 8 + x]) {exit(1);}

            squaresRects[y * 8 + x]->x = SQUARE_SIZE * x;
            squaresRects[y * 8 + x]->y = SQUARE_SIZE * y;
            squaresRects[y * 8 + x]->w = SQUARE_SIZE;
            squaresRects[y * 8 + x]->h = SQUARE_SIZE;
        }
    }

    for (int i = 0; i < 12; i++){
        piecesRects[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
        piecesRects[i]->x = 20 * i;
        piecesRects[i]->y = 0;
        piecesRects[i]->w = 20;
        piecesRects[i]->h = 20;
    }
}

void drawPieces(SDL_Renderer* renderer, Board *board) {
    for (int i = 0; i < 12; i++){
        for (int j = 0; j < 64; j++){
            if (board->pieces[i] & (1Ull << j)){
                SDL_Rect destinationRect = {j % 8 * SQUARE_SIZE, j / 8 * SQUARE_SIZE, 100, 100};
                SDL_RenderCopy(renderer, piecesTexture, piecesRects[i], &destinationRect);
            }
        }
    }
}

void drawSquares(SDL_Renderer* renderer) {
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            if ((y + x) % 2) {
                SDL_SetRenderDrawColor(renderer, 177, 110, 65, 255);
            }else {
                SDL_SetRenderDrawColor(renderer, 255, 213, 153, 100);
            }

            SDL_RenderFillRect(renderer, squaresRects[y * 8 + x]);
        }
    }
}

