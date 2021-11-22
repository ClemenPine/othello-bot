# pragma GCC target("sse4") // popcnt instruction

# include <stdlib.h>
# include <time.h>
# include <string.h>
# include <math.h>
# include "board.h"

// store information for each node
// next -> array of pointers to child nodes
// parent -> pointer to parent node
// wins -> number of wins found in that branch
// plays -> number of times that node has been visited
typedef struct node
{
    struct node *parent;
    struct node **next;
    int node_count;
    int sim_count;
    int wins;
    int draws;
    int plays;
    board *b;
    bitboard move;
} node;

// tree structure
// root -> pointer to head of tree
// b -> the current state of the board to analyze
typedef struct
{
    node *root;
} tree;

node *createNode(node *parent, board *b, bitboard move);

tree *createTree(board *b);

void deleteTree(tree *tr);

void deleteNodes(node *node);

void updateTree(tree *tr, bitboard move);

void printTree(tree *tr);

void printNodes(node *node, char *indent, int last, int depth);

// mtcs methods
void doRound(tree *tr);

node *selectLeaf(tree *tr);

node *expandTree(node *leaf);

int simulateTree(node *leaf);

void backpropagateTree(node *leaf, int res);
