#pragma once

#include <queue>
#include "State.h"

class MoveGenerator {
    static queue<tuple<int, int, int, int, int>> getBishopMoves(State& state, int i, int j);
    static queue<tuple<int, int, int, int, int>> getKingMoves(State& state, int i, int j);
    static queue<tuple<int, int, int, int, int>> getKnightMoves(State& state, int i, int j);
    static queue<tuple<int, int, int, int, int>> getPawnMoves(State& state, int i, int j);
    static queue<tuple<int, int, int, int, int>> getQueenMoves(State& state, int i, int j);
    static queue<tuple<int, int, int, int, int>> getRookMoves(State& state, int i, int j);
public:
    static vector<tuple<int, int, int, int, int>> getMoves(State& state);
};
