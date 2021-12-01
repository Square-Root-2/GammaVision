#pragma once

#include <chrono>
#include <map>
#include "Move.h"
#include "State.h"
#include <unordered_set>
#include <vector>

using namespace std;

class Engine {
    static const int DR = 4, MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 60, MAX_R = 4, MIN_R = 3;
    chrono::time_point<chrono::steady_clock> start;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int seconds;
    pair<Move, int> negamax(State& state, int depth);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool nullOk);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta, bool isQuietMove);
public:
    static int getMaximumNegamaxDepth();
    static int getMaximumQuiescenceDepth();
    tuple<Move, int, int> getOptimalMove(string FEN, int seconds);
};
