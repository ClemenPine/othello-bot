# include "ai.h"

// creates an ai 
// b -> the current game state
// returns -> a pointer to an ai struct
AI *createAI(board *b, int seconds)
{
    AI *ai = (AI *) malloc(sizeof(AI));
    ai->tree = createTree(b);
    ai->seconds = (double) seconds;
    srand(time(NULL));
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
    clock_t start_time = clock();
    while ((clock() - start_time) < (ai->seconds - 1) * CLOCKS_PER_SEC / 32)
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
    // stats of current position
    double score = ai->tree->root->wins / (double)ai->tree->root->plays;
    printf("C  AI Win: %+.2f\n", 64*(1 - score) - 32);
    printf("C  Plays: %i\n", ai->tree->root->plays);

    // stats for candidate moves
    for (int i = 0; i < ai->tree->root->node_count; i++)
    {
        node *node = ai->tree->root->next[i];
        double score = node->wins / (double)node->plays;

        if (node->move)
        {
            // get move
            int move = log(node->move) / log(2);
            char file = 97 + move % 8;
            int rank = move / 8 + 1;

            printf("C   - %c%i -> %+.2f of %i plays\n", file, rank, 64*score - 32, node->plays);
        }
        else
        {
            printf("C   - _p -> %+.2f of %i plays\n", 64*score - 32, node->plays);
        }
    }
}