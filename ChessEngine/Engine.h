#pragma once

#include <chrono>
#include "Evaluator.h"
#include <map>
#include "Move.h"
#include "NodeType.h"
#include "State.h"
#include <unordered_set>

using namespace std;

class Engine {
    static const int MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 30, R = 2;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int mateValue = Evaluator::getMaximumEvaluation() + MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH + 1;
    Move move;
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    map<tuple<string, bool, int, int>, tuple<int, NodeType, int, Move>> transpositionTable;
    void makeMove(State& state, Move& move);
    pair<Move, int> negamax(State& state, int depth, int alpha, int beta);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, vector<Move>& activeColorMoves, bool isActiveColorInCheck);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta, vector<Move>& activeColorMoves);
public:
    int getMateValue();
    tuple<Move, int, int> getOptimalMove(string& FEN, int seconds);
};
