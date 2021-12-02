# About
This is a project for my intro to AI class to create an ai to play the game
Othello. It implements Monte Carlo Tree Search as its method of choice for 
optimal play.

![Initial State](https://i.ibb.co/THBFmmH/Screenshot-from-2021-12-02-17-02-17.png)

### Methodology
Bitboards are used for representing the game states to speed up calculation as
much as possible. 

Monte Carlo Tree Search uses the standard, random playouts strategy to find
promising moves

### Structure
- main.c handles IO
- board.c handles the game representation
- tree.c handles MCTS
- ai.c interfaces with tree.c

# Sources

### General Links
- https://en.wikipedia.org/wiki/Computer_Othello

### Search techniques
- https://en.wikipedia.org/wiki/Monte_Carlo_tree_search

### Bitboards
- http://pages.cs.wisc.edu/~psilord/blog/data/chess-pages/rep.html
- https://www.chessprogramming.org/Bitboards
- https://reversiworld.wordpress.com/

### Optimizations
- https://iq.opengenus.org/__builtin_popcount-in-c/

### Helpful Projects
- https://github.com/weltyc/ntest
- https://github.com/maksimKorzh/chess_programming
