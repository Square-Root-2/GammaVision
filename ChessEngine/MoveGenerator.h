#pragma once

#include "Move.h"
#include <queue>
#include "State.h"

class MoveGenerator {
    enum DiagonalDirection {
        NORTHEAST,
        SOUTHEAST,
        SOUTHWEST,
        NORTHWEST
    };
    enum PawnType {
        WHITE,
        BLACK
    };
    unsigned long long bishopAttackSets[4][64];
    unsigned long long knightAttackSets[64];
    unsigned long long pawnAttackSets[2][64];
    void generateBishopAttackSets();
    void generateKnightAttackSets();
    void generatePawnAttackSets();
    queue<Move> getBishopMoves(State& state, int i, int j);
    queue<Move> getEnPassants(State& state);
    queue<Move> getKingMoves(State& state, int i, int j);
    queue<Move> getKnightMoves(State& state);
    queue<Move> getPawnCaptures(State& state);
    queue<Move> getPawnDoublePushes(State& state);
    queue<Move> getPawnSinglePushes(State& state);
    queue<Move> getQueenMoves(State& state, int i, int j);
    queue<Move> getRookMoves(State& state, int i, int j);
public:
    MoveGenerator();
    vector<Move> getMoves(State& state);
};
