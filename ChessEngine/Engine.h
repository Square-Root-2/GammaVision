#pragma once

#include <chrono>
#include "Evaluator.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "State.h"
#include <unordered_set>

using namespace std;

class Engine {
    enum class NodeType {
        ALL_NODE,
        CUT_NODE,
        PV_NODE
    };
    static const int DR = 4, MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 30, MAXIMUM_R = 4, MINIMUM_R = 3;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int mateValue = Evaluator::getMaximumEvaluation() + MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH + 1;
    Move move;
    MoveGenerator moveGenerator;
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    unordered_map<State, tuple<int, NodeType, int, Move>> transpositionTable;
    void makeMove(State& state, Move& move);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullOk, vector<Move>& activeColorMoves, bool isActiveColorInCheck);
    pair<Move, int> negamax(State& state, int depth, int alpha, int beta);
    int perft(State& state, int currentDepth, int depth);
    void printSearchResult(Move& optimalMove, int evaluation, int depth);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta, vector<Move>& activeColorMoves);
    void unmakeMove(State& state, Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn);
public:
    void getOptimalMove(State& state, int seconds);
    void perft(State& state, int depth);
};
