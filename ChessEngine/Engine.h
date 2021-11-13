#pragma once

#include <chrono>
#include "State.h"

using namespace std;

class Engine {
    chrono::time_point<chrono::steady_clock> start;
    int seconds;
    void makeMove(State& state, tuple<int, int, int, int, int> move);
    tuple<int, int, int, int, int, double, int> negamax(State& state, int depths);
    pair<double, int> negamax(State& state, int depths, double alpha, double beta);
public:
    tuple<int, int, int, int, int, double, int, int> getOptimalMove(string FEN, int depths);
};
