#pragma once

#include <chrono>
#include "State.h"

using namespace std;

class Engine {
    chrono::time_point<chrono::steady_clock> start;
    int seconds;
    tuple<int, int, int, int, int, double, int> getOptimalMove(State& state, int depths, double maximumOptimalEvaluation);
    void makeMove(State& state, tuple<int, int, int, int, int> move);
public:
    tuple<int, int, int, int, int, double, int, int> getOptimalMove(string FEN, int depths);
};
