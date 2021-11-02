#pragma once

#include <chrono>
#include <map>
#include <vector>
#include "State.h"

using namespace std;

class Engine {
    map<tuple<string, bool, int, int>, tuple<int, int, int, int, int, double, int, int>> principalMoves;
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    tuple<int, int, int, int, int, double, int> getOptimalMove(State& state, int depths, double maximumOptimalEvaluation);
    void makeMove(State& state, tuple<int, int, int, int, int> move);
    void orderMoves(vector<tuple<int, int, int, int, int>>& moves, tuple<string, bool, int, int> hash);
public:
    tuple<int, int, int, int, int, double, int, int> getOptimalMove(string FEN, int seconds);
};
