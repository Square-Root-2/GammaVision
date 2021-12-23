#pragma once

#include <chrono>
#include "Move.h"
#include "State.h"
#include <unordered_set>

using namespace std;

class Engine {
    enum class NodeType {
        PV,
        CUT,
        ALL
    };
    static const int DR = 4, MAXIMUM_R = 4, MINIMUM_R = 3;
    static const int MAXIMUM_EVALUATION = 10400, MAXIMUM_NEGAMAX_DEPTH = 30, MAXIMUM_PERFT_DEPTH = 30, MAXIMUM_QUIESCENCE_DEPTH = 30;
    static const int MATE_IN_ZERO = MAXIMUM_EVALUATION + MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH + 1, TIMEOUT = MATE_IN_ZERO + 1;
    static const unordered_map<MoveType, string> promotionToString;
    unordered_set<Move> killerMoves[MAXIMUM_NEGAMAX_DEPTH + 1];
    unordered_map<State, Move> principalVariation[MAXIMUM_NEGAMAX_DEPTH + MAXIMUM_QUIESCENCE_DEPTH + 1];
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    unordered_map<State, tuple<NodeType, int, Move>> transpositionTable[MAXIMUM_NEGAMAX_DEPTH + 1][2];
    bool makeMove(State& state, Move& move);
    string moveToString(Move& move);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck, bool hasThereBeenNullMove);
    pair<Move, int> negamax(State& state, int depth, int alpha, int beta);
    int perft(State& state, int currentDepth, int depth);
    string principalVariationToString(State& state);
    void printSearchResult(int depth, Move& optimalMove, int evaluation, State& state);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta, bool hasThereBeenNullMove);
    void unmakeMove(State& state, Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn);
public:
    Engine();
    void getOptimalMoveDepthVersion(State& state, int maximumDepth);
    void getOptimalMoveMoveTimeVersion(State& state, int seconds);
    void perft(State& state, int depth);
};
