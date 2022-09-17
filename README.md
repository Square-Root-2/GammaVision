# GammaVision
GammaVision is a chess engine that uses many standard chess-programming techniques, including principal-variation search, piece-square tables, and magic bitboards.

## Features
### Board Representation
- bitboards
- Zobrist hashing

### Move Generation
- bitboards
- magic bitboards

### Search
- negamax
- alpha-beta
- quiescence search
- iterative deepening
- null-move reductions
- killer heuristic
- most-valuable victim - least-valuable aggressor
- transposition table
- late-move reductions
- futility pruning
- aspiration windows
- principal-variation search

### Evaluation
- piece-square tables
- tapered evaluation

## Documentation
### Commands
- fen \<fenstring\> <br/>
  set up the position described in fenstring on the internal board
- perft \<x\> <br/>
  run perft for x plies only <br/>
- movetime \<x\> <br/>
  search exactly x mseconds
- depth \<x\> <br/>
  search x plies only
- ponder \<x\> <br/>
  ponder exactly x mseconds
  
### Examples
\>> fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 <br/>
\>> perft 5 <br/>
\>> movetime 30000 <br/>
\>> depth 8 <br/>
\>> ponder 10000
