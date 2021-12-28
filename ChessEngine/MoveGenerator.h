#ifndef _MOVE_GENERATOR_H
#define _MOVE_GENERATOR_H

#pragma once

#include "Move.h"
#include <queue>
#include "State.h"

class MoveGenerator 
{
    enum AttackSetType 
    {
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
    enum MagicBitboardType 
    {
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
    static void getCastles(vector<Move>& moves, const State& state);
    static void getColumnAttackerMoves(vector<Move>& moves, const State& state);
    static void getDiagonalAttackerMoves(vector<Move>& moves, const State& state);
    static unsigned long long getEastAttackSet(int k, unsigned long long maskedBlockers);
    static void getEnPassants(vector<Move>& moves, const State& state);
    static unsigned long long getInactiveColorColumnAttackerAttackSets(const State& state);
    static unsigned long long getInactiveColorDiagonalAttackerAttackSets(const State& state);
    static unsigned long long getInactiveColorKingAttackSets(const State& state);
    static unsigned long long getInactiveColorKnightAttackSets(const State& state);
    static unsigned long long getInactiveColorPawnAttackSets(const State& state);
    static unsigned long long getInactiveColorPieceAttackSets(const State& state);
    static unsigned long long getKey(unsigned long long maskedBlockers, MagicBitboardType k, int l);
    static void getKingMoves(vector<Move>& moves, const State& state);
    static void getKnightMoves(vector<Move>& moves, const State& state);
    static unsigned long long getMagicAttackSet(unsigned long long maskedBlockers, MagicBitboardType k, int l);
    static unsigned long long getMaskedBlockers(unsigned long long pieces, MagicBitboardType k, int l);
    static unsigned long long getNorthAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getNortheastAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getNorthwestAttackSet(int k, unsigned long long maskedBlockers);
    static void getPawnCaptures(vector<Move>& moves, const State& state);
    static void getPawnDoublePushes(vector<Move>& moves, const State& state);
    static void getPawnSinglePushes(vector<Move>& moves, const State& state);
    static unsigned long long getSouthAttackSet(int k, unsigned long long maskedBlockerse);
    static unsigned long long getSoutheastAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getSouthwestAttackSet(int k, unsigned long long maskedBlockers);
    static unsigned long long getWestAttackSet(int k, unsigned long long maskedBlockers);
    static void initializeMagicAttackSets();
    static bool isThereCollision(MagicBitboardType k, int l, const vector<unsigned long long>& maskedBlockersBitboards);
public:
    static void getMoves(vector<Move>& moves, const State& state);
    static void initialize();
    static bool isActiveColorInCheck(const State& state);
};

inline void MoveGenerator::getCastles(vector<Move>& moves, const State& state) 
{
    if (isActiveColorInCheck(state))
        return;
    unsigned long long inactiveColorPieceAttackSets = getInactiveColorPieceAttackSets(state);
    int i = state.getActiveColor() ? 0 : 7;
    if (state.canActiveColorCastleKingside() && !state.isPiece(i, 5) && !state.isPiece(i, 6) && state.isActiveColorRook(i, 7) && !(inactiveColorPieceAttackSets & ((unsigned long long)1 << (8 * i + 5))))
        moves.push_back(Move(i, 4, i, 6, MoveType::KINGSIDE_CASTLE, state.getActiveColor() ? 'k' : 'K', '.'));
    if (state.canActiveColorCastleQueenside() && !state.isPiece(i, 3) && !state.isPiece(i, 2) && !state.isPiece(i, 1) && state.isActiveColorRook(i, 0) && !(inactiveColorPieceAttackSets & ((unsigned long long)1 << (8 * i + 3))))
        moves.push_back(Move(i, 4, i, 2, MoveType::QUEENSIDE_CASTLE, state.getActiveColor() ? 'k' : 'K', '.'));
}
inline unsigned long long MoveGenerator::getEastAttackSet(int k, unsigned long long maskedBlockers) 
{
    unsigned long long eastAttackSet = attackSets[EAST][k];
    unsigned long l;
    if (_BitScanForward64(&l, attackSets[EAST][k] & maskedBlockers) == 0)
        return eastAttackSet;
    eastAttackSet &= ~attackSets[EAST][l];
    return eastAttackSet;
}
inline unsigned long long MoveGenerator::getInactiveColorPieceAttackSets(const State& state)
{
    return getInactiveColorPawnAttackSets(state) | getInactiveColorKnightAttackSets(state) | getInactiveColorKingAttackSets(state) | getInactiveColorColumnAttackerAttackSets(state) | getInactiveColorDiagonalAttackerAttackSets(state);
}
inline unsigned long long MoveGenerator::getMagicAttackSet(unsigned long long maskedBlockers, MagicBitboardType k, int l) 
{
    return k == COLUMN ? getNorthAttackSet(l, maskedBlockers) | getEastAttackSet(l, maskedBlockers) | getSouthAttackSet(l, maskedBlockers) | getWestAttackSet(l, maskedBlockers) : getNortheastAttackSet(l, maskedBlockers) | getSoutheastAttackSet(l, maskedBlockers) | getSouthwestAttackSet(l, maskedBlockers) | getNorthwestAttackSet(l, maskedBlockers);
}
inline unsigned long long MoveGenerator::getMaskedBlockers(unsigned long long pieces, MagicBitboardType k, int l) 
{
    return pieces & (k == COLUMN ? attackSets[NORTH][l] & 0xFFFFFFFFFFFFFF00 | attackSets[EAST][l] & 0x7F7F7F7F7F7F7F7F | attackSets[SOUTH][l] & 0x00FFFFFFFFFFFFFF | attackSets[WEST][l] & 0xFEFEFEFEFEFEFEFE : attackSets[NORTHEAST][l] & 0x7F7F7F7F7F7F7F00 | attackSets[SOUTHEAST][l] & 0x007F7F7F7F7F7F7F | attackSets[SOUTHWEST][l] & 0x00FEFEFEFEFEFEFE | attackSets[NORTHWEST][l] & 0xFEFEFEFEFEFEFE00);
}
inline unsigned long long MoveGenerator::getNorthAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long northAttackSet = attackSets[NORTH][k];
    unsigned long l;
    if (_BitScanReverse64(&l, attackSets[NORTH][k] & maskedBlockers) == 0)
        return northAttackSet;
    northAttackSet &= ~attackSets[NORTH][l];
    return northAttackSet;
}
inline unsigned long long MoveGenerator::getNortheastAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long northeastAttackSet = attackSets[NORTHEAST][k];
    unsigned long l;
    if (_BitScanReverse64(&l, attackSets[NORTHEAST][k] & maskedBlockers) == 0)
        return northeastAttackSet;
    northeastAttackSet &= ~attackSets[NORTHEAST][l];
    return northeastAttackSet;
}
inline unsigned long long MoveGenerator::getNorthwestAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long northwestAttackSet = attackSets[NORTHWEST][k];
    unsigned long l;
    if (_BitScanReverse64(&l, attackSets[NORTHWEST][k] & maskedBlockers) == 0)
        return northwestAttackSet;
    northwestAttackSet &= ~attackSets[NORTHWEST][l];
    return northwestAttackSet;
}
inline unsigned long long MoveGenerator::getSouthAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long southAttackSet = attackSets[SOUTH][k];
    unsigned long l;
    if (_BitScanForward64(&l, attackSets[SOUTH][k] & maskedBlockers) == 0)
        return southAttackSet;
    southAttackSet &= ~attackSets[SOUTH][l];
    return southAttackSet;
}
inline unsigned long long MoveGenerator::getSoutheastAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long southeastAttackSet = attackSets[SOUTHEAST][k];
    unsigned long l;
    if (_BitScanForward64(&l, attackSets[SOUTHEAST][k] & maskedBlockers) == 0)
        return southeastAttackSet;
    southeastAttackSet &= ~attackSets[SOUTHEAST][l];
    return southeastAttackSet;
}
inline unsigned long long MoveGenerator::getSouthwestAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long southwestAttackSet = attackSets[SOUTHWEST][k];
    unsigned long l;
    if (_BitScanForward64(&l, attackSets[SOUTHWEST][k] & maskedBlockers) == 0)
        return southwestAttackSet;
    southwestAttackSet &= ~attackSets[SOUTHWEST][l];
    return southwestAttackSet;
}
inline unsigned long long MoveGenerator::getWestAttackSet(int k, unsigned long long maskedBlockers)
{
    unsigned long long westAttackSet = attackSets[WEST][k];
    unsigned long l;
    if (_BitScanReverse64(&l, attackSets[WEST][k] & maskedBlockers) == 0)
        return westAttackSet;
    westAttackSet &= ~attackSets[WEST][l];
    return westAttackSet;
}

#endif // _MOVE_GENERATOR_H_
