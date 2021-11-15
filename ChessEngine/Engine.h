#pragma once

#include <chrono>
#include "Evaluation.h"
#include "Move.h"
#include "State.h"

using namespace std;

class Engine {
    chrono::time_point<chrono::steady_clock> start;
    int seconds;
    pair<Move, Evaluation> negamax(State& state, int depths);
    Evaluation negamax(State& state, int depths, Evaluation alpha, Evaluation beta);
public:
    tuple<Move, Evaluation, int> getOptimalMove(string FEN, int depths);
};
