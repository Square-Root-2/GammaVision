#pragma once

#include <chrono>
#include <map>
#include "Move.h"
#include "State.h"
#include <vector>

using namespace std;

class Engine {
    static const int maximumNegamaxDepth = 14, maximumQuiescenceDepth = 30, R = 3;
    map<pair<tuple<string, bool, int, int>, int>, double> evaluations;
    chrono::time_point<chrono::steady_clock> start;
    int seconds;
    pair<Move, double> negamax(State& state, int depth);
    double negamax(State& state, int currentDepth, int depth, double alpha, double beta, int nullMoves);
    double quiescenceSearch(State& state, int currentDepth, double alpha, double beta);
public:
    static int getMaximumNegamaxDepth();
    static int getMaximumQuiescenceDepth();
    tuple<Move, double, int> getOptimalMove(string FEN, int seconds);
};
