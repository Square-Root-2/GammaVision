#include "Evaluator.h"

unordered_map<char, int> Evaluator::pieceToIndex = { { 'P', 0 }, { 'p', 1 }, { 'N', 2 }, { 'n', 3 }, { 'B', 4 }, { 'b', 5 }, { 'R', 6 }, { 'r', 7 }, { 'Q', 8 }, { 'q', 9 }, { 'K', 10 }, { 'k', 11 } };
bool Evaluator::isEndgame(State& state) {
    bool isThereActiveColorQueen = false;
    bool isThereInactiveColorQueen = false;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            isThereActiveColorQueen = isThereActiveColorQueen || state.isActiveColorQueen(i, j);
            isThereInactiveColorQueen = isThereInactiveColorQueen || state.isInactiveColorQueen(i, j);
        }
    if (!isThereActiveColorQueen && !isThereInactiveColorQueen)
        return true;
    if (isThereActiveColorQueen) {
        int centipawns = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (state.isActiveColorRook(i, j))
                    return false;
                if (!state.isActiveColorKnight(i, j) && !state.isActiveColorBishop(i, j) && !state.isActiveColorQueen(i, j))
                    continue;
                centipawns += getCentipawnEquivalent(state.getPiece(i, j));
            }
        if (centipawns > 1230)
            return false;
    }
    if (isThereInactiveColorQueen) {
        int centipawns = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (state.isInactiveColorRook(i, j))
                    return false;
                if (!state.isInactiveColorKnight(i, j) && !state.isInactiveColorBishop(i, j) && !state.isInactiveColorQueen(i, j))
                    continue;
                centipawns += getCentipawnEquivalent(state.getPiece(i, j));
            }
        if (centipawns > 1230)
            return false;
    }
    return true;
}
int Evaluator::getAdjustedCentipawnEquivalent(State& state, int i, int j, bool endgame) {
    if (state.getPiece(i, j) == '.')
        return 0;
    return getCentipawnEquivalent(state.getPiece(i, j)) + PIECE_SQUARE_TABLES[2 * pieceToIndex[state.getPiece(i, j)] + endgame][i][j];
}
int Evaluator::getCentipawnEquivalent(char piece) {
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
int Evaluator::getEvaluation(State& state) {
    bool endgame = isEndgame(state);
    int evaluation = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            evaluation += (state.isActiveColorPiece(i, j) ? 1 : -1) * getAdjustedCentipawnEquivalent(state, i, j, endgame);
    return evaluation;
}
int Evaluator::getMaximumEvaluation() {
    return MAXIMUM_EVALUATION;
}
