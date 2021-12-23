#pragma once

#include "State.h"
#include <unordered_map>

class Evaluator {
    static const int PIECE_SQUARE_TABLES[24][8][8];
    static unordered_map<char, int> pieceToIndex;
    static int getAdjustedCentipawnEquivalent(State& state, int i, int j, bool endgame);
    static bool isEndgame(State& state);
public:
    static int getCentipawnEquivalent(char piece);
    static int getEvaluation(State& state);
    static void initialize();
};
