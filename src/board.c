# include "board.h"

dir dirs[8] = {up, down, left, right, upleft, upright, downleft, downright};

// creates a board with the standard initial state
// returns -> the new board
board *createBoard()
{
    board *b = (board *) malloc(sizeof(board));
    // initial state
    b->pieces[0] = 0x810000000;
    b->pieces[1] = 0x1008000000;
    b->moves = 0x102004080000;
    b->turn = black;

    return b;
}

// creates a copy of a board
// b -> the board to copy
// returns -> a board copy
board *cloneBoard(board *b)
{
    board *new_b = (board *) malloc(sizeof(board));

    new_b->pieces[0] = b->pieces[0];
    new_b->pieces[1] = b->pieces[1];
    new_b->moves = b->moves;
    new_b->turn = b->turn;

    return new_b;
}

// prints the board and some useful data to stdout
// b -> the board to show 
void printBoard(board *b) 
{
    // if board is uninitialized, do nothing
    if (!b)
    {
        return;
    }

    int b_count = __builtin_popcountll(b->pieces[0]);
    int w_count = __builtin_popcountll(b->pieces[1]);
    printf("C   B: %i, W: %i\n", b_count, w_count);

    // iterate over ranks and files
    for (int rank = 0; rank < 8; rank++)
    {
        printf("C ");
        for (int file = 0; file < 8; file++)
        {
            int square = 8*rank + file;
            
            // rank labels
            if (!file)
            {
                printf("  %d", rank + 1);
            }
                
            // bits
            char piece;
            if (getBit(b->moves, square))
            {
                piece = '*'; // represents a legal move
            }
            else if (getBit(b->pieces[0], square)) 
            {
                piece = 'B'; // represents a black piece
            }
            else if (getBit(b->pieces[1], square))
            {
                piece = 'W'; // represents a white piece
            }
            else
            {
                piece = '-'; // represents an empty square
            }

            printf(" %c", piece);
        }
        printf("\n");
    }
    // file labels
    printf("C     a b c d e f g h\n");
    printf("C\n");

    // additional debugging info
    // printf("C     Bitboard Hex Codes:\n");
    // printf("C     Black - 0x%llx\n", b->pieces[0]);
    // printf("C     White - 0x%llx\n", b->pieces[1]);
    // printf("C     Moves - 0x%llx\n", b->moves);
}

// removes a board from memory
// b -> the board to remove
void deleteBoard(board *b)
{
    if (b)
    {
        free(b);
    }
}

// shifts a bitboard in a specific direction 
// bb -> the bitboard to shift
// d -> the direction to shift towards
bitboard shift(bitboard bb, dir d)
{
    switch (d)
    {
    case up:
        return bb >> 8;
    case right:
        return bb << 1;
    case down:
        return bb << 8;
    case left:
        return bb >> 1;
    
    case upright:
        return bb >> 7;
    case downright:
        return bb << 9;
    case downleft:
        return bb << 7;
    case upleft:
        return bb >> 9;
    };
    return bb;
}

// gets all legal moves
// b -> the board to get moves from
// returns -> a bitboard with all legal moves
bitboard getMoves(board *b)
{
    bitboard moves = 0ULL;

    for (int i = 0; i < sizeof(dirs) / sizeof(dir); i++)
    {
        // it's important to not include bits at the end of the direction we want
        // to go in, otherwise shifting would go off the edge of the board or into
        // the next row
        bitboard own = b->pieces[b->turn] & dirs[i];
        bitboard opp = b->pieces[b->turn^1] & dirs[i]; 
        // first adjacent opponent pieces
        bitboard moveset = opp & shift(own, dirs[i]);

        // shifts the bits in a direction, and iteratively builds up flippable
        // pieces by shifting previously found locations onto the opponents pieces,
        // and finding a match
        bitboard prev;
        do
        {
            prev = moveset;
            moveset |= opp & shift(moveset, dirs[i]);
        } while (prev != moveset);

        // overlap with empty locations
        bitboard new = ~(b->pieces[0] | b->pieces[1]) & shift(moveset, dirs[i]);
        moves |= new;
    }

    return moves;
}

// makes a move
// b -> the board to make the move on
// bb -> a bitboard with one bit set, corresponding to the move
void makeMove(board *b, bitboard bb)
{
    // flipping logic
    if (b->moves & bb)
    {
        bitboard flips = 0ULL;

        // for each direction
        for (int i = 0; i < sizeof(dirs) / sizeof(dir); i++)
        {
            bitboard flipset = bb & dirs[i];
            bitboard opp = b->pieces[b->turn^1] & dirs[i];

            // shifts the bits in a direction, and iteratively builds up flippable
            // pieces by shifting previous found locations onto the opponents pieces,
            // and finding a match
            bitboard prev;
            do
            {
                prev = flipset;
                flipset |= opp & shift(flipset, dirs[i]);
            } while (prev != flipset);

            // only add to flips if the next location in the direction is one of
            // your own pieces
            if (b->pieces[b->turn] & shift(flipset, dirs[i]))
            {
                flips |= flipset;
            }
        }

        // set pieces and change turn
        b->pieces[b->turn] |= flips;
        b->pieces[b->turn^1] ^= flips & ~bb;
        b->turn ^= 1;

    }
    // force pass
    else if (!b->moves)
    {
        b->turn ^= 1;
    }
    // regenerate legal moves
    b->moves = getMoves(b);
}