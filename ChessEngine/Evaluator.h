#pragma once

#include "State.h"
#include <unordered_map>

class Evaluator {
    enum Color {
        WHITE,
        BLACK
    };
    enum Piece {
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
    enum PieceType {
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
    static int getEvaluation(State& state);
    static void initialize();
};
