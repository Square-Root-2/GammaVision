#pragma once

#include "Move.h"
#include <queue>
#include "State.h"

class MoveGenerator {
    enum AttackSetType {
        BLACK_PAWN,
        WHITE_PAWN,
        EAST,
        KING,
        KNIGHT,
        NORTH,
        NORTHEAST,
        NORTHWEST,
        SOUTH,
        SOUTHEAST,
        SOUTHWEST,
        WEST
    };
    enum MagicBitboardType {
        COLUMN,
        DIAGONAL
    };
    unsigned long long attackSets[12][64], magicNumbers[2][64];
    int keySizes[2][64];
    vector<unsigned long long> magicAttackSets[2][64];
    void generateEastAttackSets();
    void generateKingAttackSets();
    void generateKnightAttackSets();
    void generateMaskedBlockersBitboards(vector<unsigned long long>& maskedBlockersBitboards, unsigned long long maskedBlockers, int k);
    void generateNorthAttackSets();
    void generateNortheastAttackSets();
    void generateNorthwestAttackSets();
    void generatePawnAttackSets();
    void generateSouthAttackSets();
    void generateSoutheastAttackSets();
    void generateSouthwestAttackSets();
    void generateWestAttackSets();
    queue<Move> getCastlings(State& state);
    queue<Move> getColumnAttackerMoves(State& state);
    queue<Move> getDiagonalAttackerMoves(State& state);
    unsigned long long getEastAttackSet(int k, unsigned long long maskedBlockers);
    queue<Move> getEnPassants(State& state);
    unsigned long long getKey(unsigned long long maskedBlockers, MagicBitboardType k, int l);
    queue<Move> getKingMoves(State& state);
    queue<Move> getKnightMoves(State& state);
    unsigned long long getMagicAttackSet(unsigned long long maskedBlockers, MagicBitboardType k, int l);
    unsigned long long getMaskedBlockers(unsigned long long pieces, MagicBitboardType k, int l);
    unsigned long long getNorthAttackSet(int k, unsigned long long maskedBlockers);
    unsigned long long getNortheastAttackSet(int k, unsigned long long maskedBlockers);
    unsigned long long getNorthwestAttackSet(int k, unsigned long long maskedBlockers);
    queue<Move> getPawnCaptures(State& state);
    queue<Move> getPawnDoublePushes(State& state);
    queue<Move> getPawnSinglePushes(State& state);
    unsigned long long getSouthAttackSet(int k, unsigned long long maskedBlockerse);
    unsigned long long getSoutheastAttackSet(int k, unsigned long long maskedBlockers);
    unsigned long long getSouthwestAttackSet(int k, unsigned long long maskedBlockers);
    unsigned long long getWestAttackSet(int k, unsigned long long maskedBlockers);
    void initializeMagicAttackSets();
    bool isThereCollision(MagicBitboardType k, int l, vector<unsigned long long>& maskedBlockersBitboards);
public:
    MoveGenerator();
    vector<Move> getMoves(State& state);
};
