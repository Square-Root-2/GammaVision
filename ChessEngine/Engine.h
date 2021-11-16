#pragma once

#include <chrono>
#include "Evaluation.h"
#include <map>
#include "Move.h"
#include "State.h"
#include <vector>

using namespace std;

class Engine {
    map<pair<tuple<string, bool, int, int>, int>, Evaluation> evaluations;
    chrono::time_point<chrono::steady_clock> start;
    int seconds;
    void getPrincipalVariation(vector<Move>& principalVariation, State& state, int depths);
    pair<Move, Evaluation> negamax(State& state, int depths);
    Evaluation negamax(State& state, int depths, Evaluation alpha, Evaluation beta);
public:
    tuple<Move, Evaluation, int> getOptimalMove(string FEN, int depths);
    vector<Move> getPrincipalVariation(string FEN, int depths);
};
