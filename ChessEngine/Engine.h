#pragma once

#include <chrono>
#include <map>
#include "Move.h"
#include "State.h"
#include <unordered_set>
#include <vector>

using namespace std;

class Engine {
    static const int MAXIMUM_NEGAMAX_DEPTH = 14, MAXIMUM_QUIESCENCE_DEPTH = 30;
    int R;
    chrono::time_point<chrono::steady_clock> start;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int seconds;
    pair<Move, double> negamax(State& state, int depth);
    double negamax(State& state, int currentDepth, int depth, double alpha, double beta, int nullMoves);
    double quiescenceSearch(State& state, int currentDepth, double alpha, double beta);
public:
    static int getMaximumNegamaxDepth();
    static int getMaximumQuiescenceDepth();
    tuple<Move, double, int> getOptimalMove(string FEN, int seconds);
};
