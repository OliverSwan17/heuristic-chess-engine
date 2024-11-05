# Piece Representation

The pieces are represented using a `uint8_t`. Bits zero through three represent the piece type.
|Value | Piece|
|-----|-------|
`000` | Empty
`001` | Pawn
`010` | Knight
`011` | Bishop
`100` | Rook
`101` | Queen
`110` | King
`111` | Undefined

The fourth bit indicates the piece colour.
|Value|Colour|
|-----|------|
|`0XXX`|Black|
|`1XXX`|White|

The remaining four bits could potentially be used to indicate if en-passant is available and if the king has moved, the rook has moved etc.

