#pragma once

#include <chrono>
#include "Move.h"
#include "State.h"
#include <vector>

using namespace std;

class Engine {
    chrono::time_point<chrono::steady_clock> start;
    int maximumDepth, seconds;
    pair<Move, double> negamax(State& state, int depth);
    double negamax(State& state, int currentDepth, int depth, double alpha, double beta);
public:
    Engine(int maximumDepth);
    tuple<Move, double, int> getOptimalMove(string FEN, int depths);
};
