#pragma once

#include "Move.h"
#include <queue>
#include "State.h"

class MoveGenerator {
    enum AttackSetType {
        WHITE_PAWN,
        BLACK_PAWN,
        KNIGHT,
        KING,
        NORTH,
        NORTHEAST,
        EAST,
        SOUTHEAST,
        SOUTH,
        SOUTHWEST,
        WEST,
        NORTHWEST
    };
    enum MagicBitboardType {
        COLUMN,
        DIAGONAL
    };
    static unsigned long long attackSets[12][64], magicNumbers[2][64];
    static bool isInitialized;
    static int keySizes[2][64];
    static vector<unsigned long long> magicAttackSets[2][64];
    static void generateEastAttackSets();
    static void generateKingAttackSets();
    static void generateKnightAttackSets();
    static void generateMaskedBlockersBitboards(vector<unsigned long long>& maskedBlockersBitboards, unsigned long long maskedBlockers, int k);
    static void generateNorthAttackSets();
    static void generateNortheastAttackSets();
    static void generateNorthwestAttackSets();
    static void generatePawnAttackSets();
    static void generateSouthAttackSets();
    static void generateSoutheastAttackSets();
    static void generateSouthwestAttackSets();
    static void generateWestAttackSets();
    static void getCastles(vector<Move>& moves, State& state);
    static void getColumnAttackerMoves(vector<Move>& moves, State& state);
    static void getDiagonalAttackerMoves(vector<Move>& moves, State& state);
    static unsigned long long getEastAttackSet(int k, unsigned long long maskedBlockers);
    static void getEnPassants(vector<Move>& moves, State& state);
    static unsigned long long getInactiveColorColumnAttackerAttackSets(State& state);
    static unsigned long long getInactiveColorDiagonalAttackerAttackSets(State& state);
    static unsigned long long getInactiveColorKingAttackSets(State& state);
    static unsigned long long getInactiveColorKnightAttackSets(State& state);
    static unsigned long long getInactiveColorPawnAttackSets(State& state);
    static unsigned long long getInactiveColorPieceAttackSets(State& state);
    static unsigned long long getKey(unsigned long long maskedBlockers, MagicBitboardType k, int l);
    static void getKingMoves(vector<Move>& moves, State& state);
    static void getKnightMoves(vector<Move>& moves, State& state);
    static unsigned long long getMagicAttackSet(unsigned long long maskedBlockers, MagicBitboardType k, int l);
    static unsigned long long getMaskedBlockers(unsigned long long pieces, MagicBitboardType k, int l);
    static unsigned long long getNorthAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getNortheastAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getNorthwestAttackSet(int k, unsigned long long maskedBlockers);
    static void getPawnCaptures(vector<Move>& moves, State& state);
    static void getPawnDoublePushes(vector<Move>& moves, State& state);
    static void getPawnSinglePushes(vector<Move>& moves, State& state);
    static unsigned long long getSouthAttackSet(int k, unsigned long long maskedBlockerse);
    static unsigned long long getSoutheastAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getSouthwestAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getWestAttackSet(int k, unsigned long long maskedBlockers);
    static void initializeMagicAttackSets();
    static bool isThereCollision(MagicBitboardType k, int l, vector<unsigned long long>& maskedBlockersBitboards);
public:
    static void getMoves(vector<Move>& moves, State& state);
    static void initialize();
    static bool isActiveColorInCheck(State& state);
};
