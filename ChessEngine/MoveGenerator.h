#pragma once

#include "Move.h"
#include <queue>
#include "State.h"

class MoveGenerator {
    queue<Move> getBishopMoves(State& state, int i, int j);
    queue<Move> getKingMoves(State& state, int i, int j);
    queue<Move> getKnightMoves(State& state, int i, int j);
    queue<Move> getPawnMoves(State& state, int i, int j);
    queue<Move> getPawnCaptures(State& state);
    queue<Move> getPawnDoublePushes(State& state);
    queue<Move> getPawnSinglePushes(State& state);
    queue<Move> getQueenMoves(State& state, int i, int j);
    queue<Move> getRookMoves(State& state, int i, int j);
public:
    vector<Move> getMoves(State& state);
};
