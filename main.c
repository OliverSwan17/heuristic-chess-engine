#include "chess.h"

int main(int argc, char* argv[]) {
    unsigned char* board = fenToArray("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    printBoard(board);
}