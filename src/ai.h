# include "tree.h"

// ai struct
// tree -> the search space 
// seconds -> the number of seconds allotted for the game
typedef struct
{
    tree *tree;
    int seconds;
} AI;

AI *createAI(board *b);

void destroyAI(AI *ai);

bitboard calcBestMove(AI *ai);

void updateAI(AI *ai, bitboard move);

void printAI(AI *ai);