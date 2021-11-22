# include <stdio.h>
# include <string.h>
# include <unistd.h>

# include "ai.h"

// prints the str representation of a move bitboard
// bb -> bitboard with a single bit set for the move
void printMove(bitboard bb)
{
    // if bitboard is empty, then it should pass
    if (!bb)
    {
        printf("\n");
    }
    else
    {
        // get index
        square sq = 0;
        while(bb >>= 1)
        {
            sq++;
        }

        printf(" %c %d\n", sq % 8 + 'a', sq / 8 + 1);
    }
}

// plays opponent's move, and return the response
// b -> the game board
// ai -> the ai 
// str -> the parsed input 
bitboard processMove(board *b, AI *ai, char *str)
{
    // opponent
    bitboard move = 0ULL;
    if (str[1] != '\n')
    {
        int col = str[2] - 'a';
        int row = str[4] - '0' - 1;
        square sq = row * 8 + col;
        move = 1ULL << sq;
    }
    
    makeMove(b, move);
    updateAI(ai, move);

    // ai
    return calcBestMove(ai);
}

// handle input and output for the program
int main(int argc, char const *argv[])
{
    int seconds = 90;

    // handle args
    if (argc == 2 && atoi(argv[1]) != 0)
    {
        seconds = atoi(argv[1]);    
    }
    
    board *b = createBoard();
    AI *ai = createAI(b, seconds);

    printf("C ╔═══╗ ╔╗ ╔╗      ╔╗ ╔╗         ╔══╗      ╔╗ \n");
    printf("C ║╔═╗║╔╝╚╗║║      ║║ ║║         ║╔╗║     ╔╝╚╗\n");
    printf("C ║║ ║║╚╗╔╝║╚═╗╔══╗║║ ║║ ╔══╗    ║╚╝╚╗╔══╗╚╗╔╝\n");
    printf("C ║║ ║║ ║║ ║╔╗║║╔╗║║║ ║║ ║╔╗║    ║╔═╗║║╔╗║ ║║ \n");
    printf("C ║╚═╝║ ║╚╗║║║║║║═╣║╚╗║╚╗║╚╝║    ║╚═╝║║╚╝║ ║╚╗\n");
    printf("C ╚═══╝ ╚═╝╚╝╚╝╚══╝╚═╝╚═╝╚══╝    ╚═══╝╚══╝ ╚═╝\n");
    printf("C\n");                 
    printf("C board .. standard\n");
    printf("C showMoves .. true\n");
    printf("C showTree ... true\n");
    printf("C showDebug .. true\n");
    printf("C\n");
    printf("C sec/move ... %.2f\n", (double) (0.99*seconds - 1) / 32.0);                                                     
    printf("C Enter 'I B' or 'I W' to begin\n");

    while (1)
    {
        // get input
        char str[16];
        fgets(str, sizeof(str), stdin);

        // parse input
        switch(str[0])
        {
            // init
            case 'I':

                // reset structs
                deleteBoard(b);
                destroyAI(ai);
                b = createBoard();
                ai = createAI(b, seconds);

                if (str[2] == 'B')
                {
                    bitboard move = calcBestMove(ai);
                    makeMove(b, move);

                    printBoard(b);
                    printAI(ai);
                    printf("R B\n");
                    printf("B");
                    printMove(move);

                    updateAI(ai, move);

                }
                else if (str[2] == 'W')
                {
                    printBoard(b);
                    printf("R W\n");
                    calcBestMove(ai);
                }

                break;

            // move from black
            case 'B':
                if (b->turn == black)
                {
                    bitboard move = processMove(b, ai, str);
                    makeMove(b, move);

                    printBoard(b);
                    printAI(ai);
                    printf("W");
                    printMove(move);

                    updateAI(ai, move);
                }

                break;

            // move from white
            case 'W':
                if (b->turn == white)
                {
                    bitboard move = processMove(b, ai, str);
                    makeMove(b, move);

                    printBoard(b);
                    printAI(ai);
                    printf("B");
                    printMove(move);

                    updateAI(ai, move);
                }

                break;

            // ignore comments & undefined commands
            case 'C':
            default:
                break;
        }

        fflush(stdout);
    }

    deleteBoard(b);
    destroyAI(ai);
    return 0;
}