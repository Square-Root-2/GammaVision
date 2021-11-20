#pragma once

#include <chrono>
#include <map>
#include "Move.h"
#include "State.h"
#include <unordered_set>
#include <vector>

using namespace std;

class Engine {
    static const int maximumNegamaxDepth = 14, maximumQuiescenceDepth = 30, R = 3;
    chrono::time_point<chrono::steady_clock> start;
    unordered_set<Move> killerMoves[maximumNegamaxDepth + 1];
    int seconds;
    pair<Move, double> negamax(State& state, int depth);
    double negamax(State& state, int currentDepth, int depth, double alpha, double beta);
    double quiescenceSearch(State& state, int currentDepth, double alpha, double beta);
public:
    static int getMaximumNegamaxDepth();
    static int getMaximumQuiescenceDepth();
    tuple<Move, double, int> getOptimalMove(string FEN, int seconds);
};
