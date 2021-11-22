# include <stdio.h>
# include <stdlib.h>
# include <stdint.h> 

// turns
typedef enum 
{
    black, white
} turn;

// coordinate system
typedef enum 
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
} square;

// directions
// the hexes are masks to not include the farmost edge of the direction
typedef enum 
{
      upleft = 0xFEFEFEFEFEFEFE00,     up = 0xFFFFFFFFFFFFFF00,     upright = 0x7F7F7F7F7F7F7F00,

        left = 0xFEFEFEFEFEFEFEFE,                                  right = 0x7F7F7F7F7F7F7F7F,

    downleft = 0x00FEFEFEFEFEFEFE,     down = 0x00FFFFFFFFFFFFFF,   downright = 0x007F7F7F7F7F7F7F
} dir;

extern dir dirs[8];

// definitions for board representation
typedef unsigned long long bitboard;

// board struct
// pieces -> pieces[0] is a bitboard of black pieces, pieces[1] is the same but for white
// moves -> the available moves for the board
// turn -> describes whose turn it is
typedef struct board
{
    bitboard pieces[2];
    bitboard moves;
    turn turn;
} board;


// bitwise operation macros
# define setBit(board, square) (board |= (1ULL << square))
# define getBit(board, square) (board & (1ULL << square))
# define popBit(board, square) (board &= ~(1ULL << square))


board *createBoard();

board *cloneBoard(board *b);

void printBoard(board *b);

void deleteBoard(board *b);

bitboard shift(bitboard bb, dir d);

bitboard getMoves(board *b);

void makeMove(board *b, bitboard bb);