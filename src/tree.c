# include "tree.h"

// creates a tree node
// parent -> the node connected above the new node
// b -> game state of the node
// move -> the move that, when played on the parent board, creates the node board
// returns -> the new node
node *createNode(node *parent, board *b, bitboard move)
{
    node *nn = malloc(sizeof(node));

    nn->parent = parent;
    nn->next = NULL;
    nn->node_count = 0;
    nn->sim_count = 0;
    nn->wins = 0;
    nn->plays = 0;

    nn->b = b;
    nn->move = move;

    return nn;
}

// creates a tree 
// b -> the root state of the tree
// returns -> the new tree
tree *createTree(board *b)
{
    // mallocate
    tree *tr = (tree *) malloc(sizeof(tree));
    tr->root = createNode(NULL, cloneBoard(b), 0x0);

    return tr;
}

// wrapper to delete the entire tree struct
// tr -> the tree to delete
void deleteTree(tree *tr)
{
    deleteNodes(tr->root);
    free(tr);
}

// deletes a subtree
// node -> the root of the subtree to delete
void deleteNodes(node *node)
{
    // delete node children
    if (node->next)
    {
        for (int i=0; i < node->node_count; i++)
        {
            deleteNodes(node->next[i]);
        }
    }

    // frees the current node
    free(node->next);
    deleteBoard(node->b);
    free(node);
}

// remove all but one root subtree
// tr -> the tree to perform the operation on
// move -> the move that determines which subtree to keep
void updateTree(tree *tr, bitboard move)
{
    // find new node
    node *nn = NULL;

    node *curr = NULL;
    for (int i = 0; i < tr->root->node_count; i++)
    {
        curr = tr->root->next[i];
        if (curr->move == move)
        {
            // keep subtree
            nn = curr;
        } 
        else 
        {
            // delete subtree
            deleteNodes(curr);
        }
    }

    // free root
    free(tr->root->next);
    deleteBoard(tr->root->b);
    free(tr->root);

    // set new node as root
    nn->parent = NULL;
    tr->root = nn;
}

// wrapper for printing the tree
// tr -> tree to print
void printTree(tree *tr)
{
    char buf[128] = ""; 
    printNodes(tr->root, buf, 1, 2);
    printf("C   \n");
}

// prints a subtree recursively
// node -> the root node of the subtree
// indent -> the level of indent to print the node (recursively set)
// last -> whether or not the node is a leaf
// depth -> how many nodes deep the function should print
// 
// https://stackoverflow.com/questions/1649027
void printNodes(node *node, char *indent, int last, int depth)
{
    if (depth == 0)
    {
        return;
    }

    // print node
    if (node->move)
    {
        // convert move to string
        int move = log(node->move) / log(2);
        char file = 97 + move % 8;
        int rank = move / 8 + 1;
        
        printf("C  %s+- %c%i %f/%f|%i\n", indent, file, rank, node->wins, node->plays-node->wins, node->plays);
    }
    else
    {
        printf("C  %s+- %f/%f|%i\n", indent, node->wins, node->plays-node->wins, node->plays);
    }

    // determine next indentation level
    char next_indent[128];
    strcpy(next_indent, indent);
    strcat(next_indent, last ? "   " : "|  ");

    // print child nodes
    for (int i=0; i < node->node_count; i++)
    {
        printNodes(node->next[i], next_indent, i == node->node_count - 1, depth - 1);
    }
}

// does one round of monte carlo tree search
// tr -> the tree to perform the round on
// 
// https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
void doRound(tree *tr)
{
    node *leaf = selectLeaf(tr);
    node *nn = expandTree(leaf);
    double res = simulateTree(nn);
    backpropagateTree(nn, res);
}

// finds the successor leaf
// tr -> the tree to find the leaf from
// returns -> the successor leaf
node *selectLeaf(tree *tr)
{
    node *curr = tr->root;

    // while curr is not a leaf
    while (
        curr->sim_count == curr->node_count && 
        curr->sim_count != 0
    )
    {
        // selection algorithm
        node *best_node = NULL;
        double best_score;

        for (int i = 0; i < curr->node_count; i++)
        {
            node *sub = curr->next[i];

            // UCT with draw calculation included
            double exploit = sub->wins / (double)sub->plays;
            double explore = sqrt(log((double)curr->plays) / (double)sub->plays);
            double score = exploit + 1.414*explore;

            if (score > best_score || best_node == NULL)
            {
                best_node = sub;
                best_score = score;
            }
        }
        curr = best_node;
    }
    return curr;
}

// selects a child node that hasn't yet been explored
// leaf -> the parent of the selected child node
// returns -> the selected child node
node *expandTree(node *leaf)
{
    // populate node with child nodes
    if (leaf->sim_count == 0)
    {
        bitboard moves = getMoves(leaf->b);
        leaf->node_count = __builtin_popcountll(moves);

        // ensures that pass moves are accounted for
        if (!leaf->node_count)
        {
            leaf->node_count = 1;
        }

        leaf->next = malloc(sizeof(node) * leaf->node_count);

        // adding moves
        for (int i=0; i < leaf->node_count; i++)
        {
            bitboard ls1b = moves & -moves;

            board *new_board = cloneBoard(leaf->b);
            makeMove(new_board, ls1b);
            leaf->next[i] = createNode(leaf, new_board, ls1b);

            moves &= (moves - 1);
        } 
    }

    // chooses a child node by randomly selecting a node until a node
    // with no plays has been found
    for (int i=0; i < 1000; i++)
    {
        node *rand_node = leaf->next[rand() % leaf->node_count];
        if (rand_node->plays == 0)
        {
            return rand_node;
        }   
    }

    // this should realistically never be printed
    printf("no child found\n");
    exit(1);
}

// evaluates the child node 
// done through standard random playouts
// leaf -> the child node to start the playout from
// returns -> the evaluation score
double simulateTree(node *leaf)
{
    board *b = cloneBoard(leaf->b);
    
    // random moves until both players pass consecutively
    int pass_count = 0;
    while (pass_count < 2)
    {
        bitboard moves = b->moves;
        int move_count = __builtin_popcountll(moves);

        if (move_count == 0)
        {
            // pass
            makeMove(b, 0x0);
            pass_count++;
        }
        else
        {
            // pick random index - find and play that move
            int index = rand() % move_count;
            for (int i = 0; i < index; i++)
            {
                moves &= (moves - 1);
            }

            makeMove(b, moves & -moves);
            pass_count = 0;
        }
    } 

    // count pieces
    bitboard pieces = b->pieces[!leaf->b->turn];
    int count = __builtin_popcountll(pieces);

    deleteBoard(b);

    // determine winner
    double score = (double)count / 64;
    return score;
}

// update the tree with the information from the simulation step
// leaf -> the child node of the selected leaf
// res -> the simulation function's score
void backpropagateTree(node *leaf, double res)
{
    // keep track of new simulations
    if (leaf->plays == 0)
    {
        leaf->parent->sim_count++;
    }

    // propagation
    node *curr = leaf;
    while (curr != NULL)
    {
        // count wins
        curr->plays++;
        curr->wins += res;

        // alternate up the tree
        res = 1 - res;
        curr = curr->parent;
    }
}
