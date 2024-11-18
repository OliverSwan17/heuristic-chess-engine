
def gen_tuple(input_str):
    square = ''
    piece_col = ''
    piece = ''

    if input_str[0] == 'l':
        square = 'light'
    else:
        square = 'dark'

    if input_str[1] == 'b':
        piece_col = 'black'
    elif input_str[1] == 'w':
        piece_col = 'white'
    else:
        piece_col = 'undefined'


    if input_str[2] == 'p':
        piece = 'pawn'
    elif input_str[2] == 'r':
        piece = 'rook'
    elif input_str[2] == 'k':
        piece = 'knight'
    elif input_str[2] == 'b':
        piece = 'bishop'
    elif input_str[2] == 'q':
        piece = 'queen'
    elif input_str[2] == 'K':
        piece = 'king'
    elif input_str[2] == 'e':
        piece = 'empty'

labels_map = {
                               0: 'lbr',    # Light Black Rook
                               1: 'dbk',    # Dark  Black Knight
                               2: 'lbb',    # Light Black Bishop
                               3: 'dbq',    # Dark  Black Queen
                               4: 'lbK',    # Light Black King
                               5: 'dbb',    # Dark  Black Bishop
                               6: 'lbk',    # Light Black Knight
                               7: 'dbr',    # Dark  Black Rook
                               8: 'dbp',    # Dark  Black Pawn
                               9: 'lbp',    # Light Black Pawn
                               10: 'due',   # Dark  Undef Empty
                               11: 'lue',   # Light Undef Empty
                               12: 'dwp',   # Dark  White Pawn
                               13: 'lwp',   # Light White Pawn
                               14: 'dwr',   # Dark  White Rook
                               15: 'lwk',   # Light White Knight
                               16: 'dwb',   # Dark  White Bishop
                               17: 'lwq',   # Light White Queen
                               18: 'dwK',   # Dark  White King
                               19: 'lwb',   # Light White Bishop
                               20: 'dwk',   # Dark  White Knight
                               21: 'lwr',   # Light White Rook
                               #NOT ON BASE BOARD
                               22: 'dbK',   # Dark  Black King
                               23: 'lwK',   # Light White King
                               24: 'lbq',   # Light Black Queen
                               25: 'dwq'}   # Dark  White Queen


def gen_tuple(input_str):
    square = ''
    piece_col = ''
    piece = ''

    if input_str[0] == 'l':
        square = 'light'
    else:
        square = 'dark'

    if input_str[1] == 'b':
        piece_col = 'black'
    elif input_str[1] == 'w':
        piece_col = 'white'
    else:
        piece_col = 'undefined'


    if input_str[2] == 'p':
        piece = 'pawn'
    elif input_str[2] == 'r':
        piece = 'rook'
    elif input_str[2] == 'k':
        piece = 'knight'
    elif input_str[2] == 'b':
        piece = 'bishop'
    elif input_str[2] == 'q':
        piece = 'queen'
    elif input_str[2] == 'K':
        piece = 'king'
    elif input_str[2] == 'e':
        piece = 'empty'
    return (piece, piece_col, square)

new = {}
for k, v in labels_map.items():
    new[k] = gen_tuple(v)

print("\tself.__labels_map__ = {")
for k,v in new.items():
    print(f"\t\t{k}:{v},")
print("\t}")



