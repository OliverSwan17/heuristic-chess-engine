#include "chess.h"

SDL_Rect* squaresRects[64];
SDL_Rect* piecesRects[12];
SDL_Texture* piecesTexture;

TTF_Font *numberFont;
SDL_Texture *numbersTextures[64];
SDL_Color fontColour = {100, 12, 200};

void initPiecesTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/pieces/pieces.png");
    if (surface == NULL) {printf("%s\n", IMG_GetError());}
    piecesTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (piecesTexture == NULL) {printf("%s\n", SDL_GetError());}
    SDL_FreeSurface(surface);
}

void initNumbersTextures(SDL_Renderer* renderer) {
    numberFont = TTF_OpenFont("assets/fonts/numbers.ttf", 32);

    for (int i = 0; i < 64; i++) {
        char text[2];
        sprintf(text, "%d", i);
        SDL_Surface *textSurface = TTF_RenderText_Solid(numberFont, text, fontColour);
        numbersTextures[i] = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_SetTextureAlphaMod(numbersTextures[i], 25);
        SDL_FreeSurface(textSurface);
    }
}

void initRectangles() {
    for (int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            squaresRects[y * 8 + x] = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            squaresRects[y * 8 + x]->x = SQUARE_SIZE * x;
            squaresRects[y * 8 + x]->y = 700 - SQUARE_SIZE * y;
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
        for (int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                if (board->pieces[i] & (1Ull << (y * 8 + x))){
                    SDL_RenderCopy(renderer, piecesTexture, piecesRects[i], squaresRects[y * 8 + x]);
                }
            }
        }
    }
}

void drawNumbers(SDL_Renderer* renderer) {
    for (int i = 0; i < 64; i++) {
        SDL_RenderCopy(renderer, numbersTextures[i], NULL, squaresRects[i]);
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

void drawHighlightedSquares(Bitboard squares, SDL_Renderer* renderer) {
    for(int i = 0; i < 64; i++) {
        if((1ULL << i) & squares){
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(renderer, squaresRects[i]);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
}

