#include "Evaluator.h"

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
double Evaluator::getEvaluation(State& state) {
    double evaluation = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            evaluation += (state.isActiveColorPiece(i, j) ? 1 : -1) * getPawnEquivalent(state.getPiece(i, j));
    return evaluation;
}
double Evaluator::getMaximumEvaluation() {
    return MAXIMUM_EVALUATION;
}
