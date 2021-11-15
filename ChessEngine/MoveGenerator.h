#pragma once

#include <queue>
#include "Move.h"
#include "State.h"

class MoveGenerator {
    static queue<Move> getBishopMoves(State& state, int i, int j);
    static queue<Move> getKingMoves(State& state, int i, int j);
    static queue<Move> getKnightMoves(State& state, int i, int j);
    static queue<Move> getPawnMoves(State& state, int i, int j);
    static queue<Move> getQueenMoves(State& state, int i, int j);
    static queue<Move> getRookMoves(State& state, int i, int j);
public:
    static vector<Move> getMoves(State& state);
};
