Refactors
- Fix the functions in `draw.c` to use bitmasks for refering to pieces. This will ensure that once we start using other bits in the piece representation for indicators for en-passant and castling there will not be any miscalculations.

AI Things
- Method to evaluate position
- Code up a tree data structure (each board state is a different position)
- Read up on minimax
