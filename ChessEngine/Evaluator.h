#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

#pragma once

#include "State.h"
#include <unordered_map>

class Evaluator 
{
    enum Color 
    {
        WHITE,
        BLACK
    };
    enum Piece 
    {
        WHITE_PAWN,
        BLACK_PAWN,
        WHITE_KNIGHT,
        BLACK_KNIGHT,
        WHITE_BISHOP,
        BLACK_BISHOP,
        WHITE_ROOK,
        BLACK_ROOK,
        WHITE_QUEEN,
        BLACK_QUEEN,
        WHITE_KING,
        BLACK_KING
    };
    enum PieceType 
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    };
    static const int ENDGAME_BISHOP_TABLE[64], ENDGAME_KING_TABLE[64], ENDGAME_KNIGHT_TABLE[64], ENDGAME_PAWN_TABLE[64], ENDGAME_QUEEN_TABLE[64], ENDGAME_ROOK_TABLE[64];
    static const int ENDGAME_VALUES[6], GAME_PHASE_INCREMENT[12], MIDDLEGAME_VALUES[6];
    static const int MIDDLEGAME_BISHOP_TABLE[64], MIDDLEGAME_KING_TABLE[64], MIDDLEGAME_KNIGHT_TABLE[64], MIDDLEGAME_PAWN_TABLE[64], MIDDLEGAME_QUEEN_TABLE[64], MIDDLEGAME_ROOK_TABLE[64];
    static const int* ENDGAME_TABLES[6];
    static const int* MIDDLEGAME_TABLES[6];
    static const unordered_map<char, Piece> PIECE_TO_INDEX;
    static int ENDGAME_TABLE[12][64], MIDDLEGAME_TABLE[12][64];
    static bool isInitialized;
    static int getFlippedIndex(int i, int j);
    static bool isWhite(char piece);
public:
    static int getCentipawnEquivalent(char piece);
    static int getEvaluation(const State& state);
    static void initialize();
};

inline int Evaluator::getFlippedIndex(int i, int j)
{
    return 8 * (7 - i) + j;
}
inline bool Evaluator::isWhite(char piece)
{
    if (piece == '.')
        return false;
    return isupper(piece);
}
inline int Evaluator::getCentipawnEquivalent(char piece)
{
    if (piece == 'P' || piece == 'p')
        return 100;
    if (piece == 'N' || piece == 'n')
        return 320;
    if (piece == 'B' || piece == 'b')
        return 330;
    if (piece == 'R' || piece == 'r')
        return 500;
    if (piece == 'Q' || piece == 'q')
        return 900;
    if (piece == 'K' || piece == 'k')
        return 20000;
    return 0;
}

#endif // _EVALUATOR_H_
