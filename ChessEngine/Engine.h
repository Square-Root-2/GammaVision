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
        ALL,
        CUT,
        PV
    };
    static const int DR = 4, MAXIMUM_R = 4, MINIMUM_R = 3;
    static const int MAXIMUM_EVALUATION = 10400, MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_PERFT_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 30;
    static const int MATE_IN_ZERO = MAXIMUM_EVALUATION + MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH + 1, TIMEOUT = MATE_IN_ZERO + 1;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    int nodesSearched;
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    unordered_map<State, tuple<NodeType, int, Move>> transpositionTable[MAXIMUM_NEGAMAX_DEPTH + 1][2];
    bool makeMove(State& state, Move& move);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck);
    pair<Move, int> negamax(State& state, int depth, int alpha, int beta);
    int perft(State& state, int currentDepth, int depth);
    void printSearchResult(Move& optimalMove, int evaluation, int depth, int nodesSearched);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta);
    void unmakeMove(State& state, Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn);
public:
    Engine();
    void getOptimalMoveDepthVersion(State& state, int maximumDepth);
    void getOptimalMoveMoveTimeVersion(State& state, int seconds);
    void perft(State& state, int depth);
};
