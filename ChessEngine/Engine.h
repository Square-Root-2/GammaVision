#ifndef _ENGINE_H_
#define _ENGINE_H_

#pragma once

#include <chrono>
#include <iostream>
#include "Move.h"
#include "MoveGenerator.h"
#include "State.h"
#include <unordered_set>

using namespace std;

class Engine 
{
    enum class NodeType 
    {
        PV,
        CUT,
        ALL
    };
    static const int DR = 4, 
        MAXIMUM_DEPTH = 60,
        MAXIMUM_EVALUATION = 24120,
        MAXIMUM_NEGAMAX_DEPTH = 30, 
        MAXIMUM_PERFT_DEPTH = 30,
        MAXIMUM_R = 4, 
        MINIMUM_R = 3,
        MATE_IN_ZERO = MAXIMUM_EVALUATION + MAXIMUM_DEPTH + 1,
        TIMEOUT = MATE_IN_ZERO + 1;
    static const unordered_map<MoveType, string> promotionToString;
    unordered_set<Move> killerMoves[MAXIMUM_DEPTH + 1];
    int seconds;
    chrono::time_point<chrono::steady_clock> start;
    unordered_map<State, tuple<int, NodeType, int, Move>> transpositionTable;
    bool makeMove(State& state, const Move& move);
    void makeNullMove(State& state);
    string moveToString(const Move& move);
    int negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck, bool hasThereBeenNullMove);
    pair<Move, int> negamax(State& state, int depth, int alpha, int beta);
    int perft(State& state, int currentDepth, int depth);
    void printSearchResult(int depth, const Move& optimalMove, int evaluation);
    int quiescenceSearch(State& state, int currentDepth, int alpha, int beta, bool hasThereBeenNullMove);
    void unmakeMove(State& state, const Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn);
    void unmakeNullMove(State& state, int possibleEnPassantTargetColumn);
public:
    Engine();
    void getOptimalMoveDepthVersion(const State& state, int maximumDepth);
    void getOptimalMoveMoveTimeVersion(const State& state, int seconds);
    void perft(const State& state, int depth);
};

inline bool Engine::makeMove(State& state, const Move& move)
{
    state.setPiece(move.getEndRow(), move.getEndColumn(), move.getAggressor());
    state.setPiece(move.getBeginRow(), move.getBeginColumn(), '.');
    if (move.getMoveType() == MoveType::KINGSIDE_CASTLE)
    {
        state.setPiece(move.getBeginRow(), 7, '.');
        state.setPiece(move.getBeginRow(), 5, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::QUEENSIDE_CASTLE)
    {
        state.setPiece(move.getBeginRow(), 0, '.');
        state.setPiece(move.getBeginRow(), 3, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::EN_PASSANT)
        state.setPiece(move.getBeginRow(), move.getEndColumn(), '.');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_BISHOP)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'b' : 'B');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_KNIGHT)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'n' : 'N');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_QUEEN)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'q' : 'Q');
    else if (move.getMoveType() == MoveType::PROMOTION_TO_ROOK)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'r' : 'R');
    if (move.getMoveType() == MoveType::KINGSIDE_CASTLE || move.getMoveType() == MoveType::QUEENSIDE_CASTLE)
    {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getAggressor() == 'K' || move.getAggressor() == 'k')
    {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if ((move.getAggressor() == 'R' || move.getAggressor() == 'r') && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 7)
        state.setCanActiveColorCastleKingside(false);
    else if ((move.getAggressor() == 'R' || move.getAggressor() == 'r') && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 0)
        state.setCanActiveColorCastleQueenside(false);
    if (move.getMoveType() == MoveType::PAWN_DOUBLE_PUSH)
        state.setPossibleEnPassantTargetColumn(move.getBeginColumn());
    else
        state.setPossibleEnPassantTargetColumn(-1);
    bool isLegal = !MoveGenerator::isActiveColorInCheck(state);
    state.toggleActiveColor();
    return isLegal;
}
inline void Engine::makeNullMove(State& state)
{
    state.setPossibleEnPassantTargetColumn(-1);
    state.toggleActiveColor();
}
inline string Engine::moveToString(const Move& move)
{
    return string{ char(move.getBeginColumn() + 'a') }
        + to_string(8 - move.getBeginRow())
        + string{ char(move.getEndColumn() + 'a') }
        + to_string(8 - move.getEndRow())
        + (move.isPromotion() ? promotionToString.find(move.getMoveType())->second : "");
}
inline void Engine::printSearchResult(int depth, const Move& optimalMove, int evaluation) {
    cout << "\nDepth: " << depth << "\n";
    cout << "Move: " << moveToString(optimalMove) << "\n";
    cout << "Evaluation: ";
    if (abs(evaluation) > MAXIMUM_EVALUATION)
        cout << (MATE_IN_ZERO - abs(evaluation) % 2 ? '+' : '-') << 'M' << (MATE_IN_ZERO - abs(evaluation) + 1) / 2 << "\n\n";
    else
        cout << evaluation << "\n\n";
}
inline void Engine::unmakeMove(State& state, const Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn)
{
    state.toggleActiveColor();
    state.setPossibleEnPassantTargetColumn(possibleEnPassantTargetColumn);
    state.setCanActiveColorCastleKingside(couldActiveColorCastleKingside);
    state.setCanActiveColorCastleQueenside(couldActiveColorCastleQueenside);
    if (move.getMoveType() == MoveType::KINGSIDE_CASTLE)
    {
        state.setPiece(move.getBeginRow(), 5, '.');
        state.setPiece(move.getBeginRow(), 7, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::QUEENSIDE_CASTLE)
    {
        state.setPiece(move.getBeginRow(), 3, '.');
        state.setPiece(move.getBeginRow(), 0, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getMoveType() == MoveType::EN_PASSANT)
        state.setPiece(move.getBeginRow(), move.getEndColumn(), move.getVictim());
    else if (move.isPromotion())
        state.setPiece(move.getEndRow(), move.getEndColumn(), move.getAggressor());
    if (move.getMoveType() == MoveType::EN_PASSANT)
        state.setPiece(move.getEndRow(), move.getEndColumn(), '.');
    else
        state.setPiece(move.getEndRow(), move.getEndColumn(), move.getVictim());
    state.setPiece(move.getBeginRow(), move.getBeginColumn(), move.getAggressor());
}
inline void Engine::unmakeNullMove(State& state, int possibleEnPassantTargetColumn)
{
    state.toggleActiveColor();
    state.setPossibleEnPassantTargetColumn(possibleEnPassantTargetColumn);
}

#endif //_ENGINE_H_
