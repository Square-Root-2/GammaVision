#pragma once

#include "State.h"

using namespace std;

class Engine {
    tuple<int, int, int, int, int, double, int> getOptimalMove(State& state, int depths, double maximumOptimalEvaluation);
    void makeMove(State& state, tuple<int, int, int, int, int> move);
public:
    tuple<int, int, int, int, int, double, int> getOptimalMove(string FEN, int depths);
};
