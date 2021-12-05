#pragma once

#include "ChessBoard.h"
#include <chrono>
#include "Move.h"
#include <unordered_set>

using namespace std;

class Engine {
    static const int DR = 4, MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 30, MAX_R = 4, MIN_R = 3;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    void makeMove(ChessBoard& chessBoard, Move& move);
    pair<Move, int> negamax(ChessBoard& chessBoard, int depth);
    int negamax(ChessBoard& chessBoard, int currentDepth, int depth, int alpha, int beta, bool isNullOk);
    int quiescenceSearch(ChessBoard& chessBoard, int currentDepth, int alpha, int beta);
public:
    static int getMaximumNegamaxDepth();
    static int getMaximumQuiescenceDepth();
    tuple<Move, int, int> getOptimalMove(string& FEN, int seconds);
};
