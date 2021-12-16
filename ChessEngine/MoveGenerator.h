#pragma once

#include "Move.h"
#include <queue>
#include "State.h"

class MoveGenerator {
    enum Direction {
        NORTH,
        NORTHEAST,
        EAST,
        SOUTHEAST,
        SOUTH,
        SOUTHWEST,
        WEST,
        NORTHWEST
    };
    enum PawnType {
        WHITE,
        BLACK
    };
    unsigned long long slidingPieceAttackSets[8][64];
    unsigned long long kingAttackSets[64], knightAttackSets[64];
    unsigned long long pawnAttackSets[2][64];
    void generateEastAttackSets();
    void generateKingAttackSets();
    void generateKnightAttackSets();
    void generateNorthAttackSets();
    void generateNortheastAttackSets();
    void generateNorthwestAttackSets();
    void generatePawnAttackSets();
    void generateSlidingPieceAttackSets();
    void generateSoutheastAttackSets();
    void generateSouthwestAttackSets();
    void generateSouthAttackSets();
    void generateWestAttackSets();
    queue<Move> getCastlings(State& state);
    queue<Move> getColumnAttackerMoves(State& state);
    queue<Move> getDiagonalAttackerMoves(State& state);
    unsigned long long getEastAttackSet(State& state, int i, int j);
    queue<Move> getEnPassants(State& state);
    queue<Move> getKingMoves(State& state);
    queue<Move> getKingMoves(State& state, int i, int j);
    queue<Move> getKnightMoves(State& state);
    unsigned long long getNorthAttackSet(State& state, int i, int j);
    unsigned long long getNortheastAttackSet(State& state, int i, int j);
    unsigned long long getNorthwestAttackSet(State& state, int i, int j);
    queue<Move> getPawnCaptures(State& state);
    queue<Move> getPawnDoublePushes(State& state);
    queue<Move> getPawnSinglePushes(State& state);
    unsigned long long getSouthAttackSet(State& state, int i, int j);
    unsigned long long getSoutheastAttackSet(State& state, int i, int j);
    unsigned long long getSouthwestAttackSet(State& state, int i, int j);
    unsigned long long getWestAttackSet(State& state, int i, int j);
public:
    MoveGenerator();
    vector<Move> getMoves(State& state);
};
