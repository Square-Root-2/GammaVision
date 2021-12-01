#include "Evaluator.h"

unordered_map<char, int> Evaluator::pieceToIndex = { { 'P', 0 }, { 'p', 1 }, { 'N', 2 }, { 'n', 3 }, { 'B', 4 }, { 'b', 5 }, { 'R', 6 }, { 'r', 7 }, { 'Q', 8 }, { 'q', 9 }, { 'K', 10 }, { 'k', 11 } };
int Evaluator::getAdjustedCentipawnEquivalent(State& state, int i, int j) {
    if (state.getPiece(i, j) == '.')
        return 0;
    return getCentipawnEquivalent(state.getPiece(i, j)) + PIECE_SQUARE_TABLES[2 * pieceToIndex[state.getPiece(i, j)] + state.isEndgame()][i][j];
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
    int evaluation = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            evaluation += (state.isActiveColorPiece(i, j) ? 1 : -1) * getAdjustedCentipawnEquivalent(state, i, j);
    return evaluation;
}
int Evaluator::getMaximumEvaluation() {
    return MAXIMUM_EVALUATION;
}
