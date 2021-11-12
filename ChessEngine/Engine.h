#pragma once

#include <chrono>
#include "State.h"

using namespace std;

class Engine {
    chrono::time_point<chrono::steady_clock> start;
    int seconds;
    void makeMove(State& state, tuple<int, int, int, int, int> move);
    pair<double, int> negamax(State& state, int depths, double maximumOptimalEvaluation);
    tuple<int, int, int, int, int, double, int> negamax(State& state, int depths);
public:
    tuple<int, int, int, int, int, double, int, int> getOptimalMove(string FEN, int depths);
};
