# GammaVision
GammaVision is a chess engine that implements many of the chess-programming techniques found on the [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page).

# Features
Board Representation
- bitboards
- Zobrist hashing

Move Generation
- bitboards for pawns, knights, kings
- magic bitboards for sliding pieces

Search
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

Evaluation
- piece-square tables
- tapered evaluation

# Documentation
Commands
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
  
Examples


\>> fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 <br/>
\>> perft 5 <br/>
\>> movetime 30000 <br/>
\>> depth 8 <br/>
\>> ponder 10000
