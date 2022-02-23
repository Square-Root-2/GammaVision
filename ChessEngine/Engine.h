#ifndef _ENGINE_H_
#define _ENGINE_H_

#pragma once

#include <chrono>
#include <iostream>
#include "Move.h"
#include "MoveGenerator.h"
#include <stack>
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
    unordered_set<Move> killerMoves[MAXIMUM_DEPTH + 1];
    int milliseconds;
    State ponderState;
    static const unordered_map<MoveType, string> promotionToString;
    chrono::time_point<chrono::steady_clock> start;
    unordered_map<State, tuple<int, NodeType, int, Move>> transpositionTable;
    bool makeMove(State& state, const Move& move) const;
    void makeNullMove(State& state) const;
    string moveToString(const Move& move) const;
    pair<int, stack<Move>*> negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool isNullMoveOk, bool isActiveColorInCheck);
    pair<int, stack<Move>*> negamax(State& state, int depth, int alpha, int beta);
    int perft(State& state, int currentDepth, int depth) const;
    void printSearchResult(int depth, const pair<int, stack<Move>*> optimalEvaluation) const;
    pair<int, stack<Move>*> quiescenceSearch(State& state, int currentDepth, int alpha, int beta);
    void unmakeMove(State& state, const Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn) const;
    void unmakeNullMove(State& state, int possibleEnPassantTargetColumn) const;
public:
    void getOptimalMoveDepthVersion(const State& state, int maximumDepth);
    void getOptimalMoveMoveTimeVersion(const State& state, int milliseconds);
    void perft(const State& state, int depth) const;
    void ponder(int milliseconds);
};

inline bool Engine::makeMove(State& state, const Move& move) const
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
inline void Engine::makeNullMove(State& state) const
{
    state.setPossibleEnPassantTargetColumn(-1);
    state.toggleActiveColor();
}
inline string Engine::moveToString(const Move& move) const
{
    return string{ char(move.getBeginColumn() + 'a') }
        + to_string(8 - move.getBeginRow())
        + string{ char(move.getEndColumn() + 'a') }
        + to_string(8 - move.getEndRow())
        + (move.isPromotion() ? promotionToString.find(move.getMoveType())->second : "");
}
inline void Engine::unmakeMove(State& state, const Move& move, bool couldActiveColorCastleKingside, bool couldActiveColorCastleQueenside, int possibleEnPassantTargetColumn) const
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
inline void Engine::unmakeNullMove(State& state, int possibleEnPassantTargetColumn) const
{
    state.toggleActiveColor();
    state.setPossibleEnPassantTargetColumn(possibleEnPassantTargetColumn);
}
#endif //_ENGINE_H_
