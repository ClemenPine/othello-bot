# include "ai.h"

// creates an ai 
// b -> the current game state
// returns -> a pointer to an ai struct
AI *createAI(board *b)
{
    AI *ai = (AI *) malloc(sizeof(AI));
    ai->tree = createTree(b);
    ai->seconds = 600;
    // srand(time(NULL));
    return ai;
}

// deletes the ai before it becomes sentient
// ai -> the ai that is becoming sentient
void destroyAI(AI *ai)
{
    deleteTree(ai->tree);
    free(ai);
}

// figures out the "best" move to make
// ai -> the ai to use for the calculation
// returns -> a bitboard with a single bit set as the move
bitboard calcBestMove(AI *ai)
{
    time_t start_time = time(NULL);

    // play a set of rounds until there is no time left
    while(time(NULL) - start_time < ai->seconds / 32 - 1)
    {
        doRound(ai->tree);
    }

    // get move with highest number of plays
    bitboard best_move = 0x0; 
    int best_score;
    for (int i = 0; i < ai->tree->root->node_count; i++)
    {
        int score = ai->tree->root->next[i]->plays;
        if (!best_move || score > best_score)
        {
            best_move = ai->tree->root->next[i]->move;
            best_score = score;
        }
    }

    return best_move;
}

// informs ai of a new move that has been played
// ai -> the ai to inform
// move -> the move that was just made
void updateAI(AI *ai, bitboard move)
{
    updateTree(ai->tree, move);
}

// prints the ai
// ai -> the ai to print
void printAI(AI *ai)
{
    printTree(ai->tree);
}