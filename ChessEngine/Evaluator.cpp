#include "Evaluator.h"

unordered_map<char, int> Evaluator::pieceToIndex = { { 'P', 0 }, { 'N', 1 }, { 'B', 2 }, { 'R', 3 }, { 'Q', 4 }, { 'K', 5 } };
double Evaluator::getAdjustedPawnEquivalent(State& state, int i, int j) {
    if (state.getPiece(i, j) == '.')
        return 0;
    return getPawnEquivalent(state.getPiece(i, j)) + PIECE_SQUARE_TABLES[2 * pieceToIndex[toupper(state.getPiece(i, j))] + state.isEndgame()][state.getActiveColor() ? 7 - i : i][state.getActiveColor() ? 7 - j : j];
}
double Evaluator::getEvaluation(State& state) {
    double evaluation = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            evaluation += (state.isActiveColorPiece(i, j) ? 1 : -1) * getAdjustedPawnEquivalent(state, i, j);
    return evaluation;
}
double Evaluator::getMaximumEvaluation() {
    return MAXIMUM_EVALUATION;
}
int Evaluator::getPawnEquivalent(char piece) {
    if (piece == 'P' || piece == 'p')
        return 1;
    if (piece == 'N' || piece == 'n')
        return 3;
    if (piece == 'B' || piece == 'b')
        return 3;
    if (piece == 'R' || piece == 'r')
        return 5;
    if (piece == 'Q' || piece == 'q')
        return 9;
    return 0;
}
