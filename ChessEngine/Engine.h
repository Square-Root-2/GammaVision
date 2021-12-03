#pragma once

#include <chrono>
#include "Move.h"
#include "State.h"
#include <unordered_set>

using namespace std;

class Engine {
    static const int DR = 4, MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 30, MAX_R = 4, MIN_R = 3;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    pair<Move, int> negamax(State& state, int depth);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool nullOk);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta);
public:
    static int getMaximumNegamaxDepth();
    static int getMaximumQuiescenceDepth();
    tuple<Move, int, int> getOptimalMove(string FEN, int seconds);
};
