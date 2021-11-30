#include "Evaluator.h"

bool Evaluator::isEndgame(State& state) {
    bool isThereActiveColorQueen = state.isThereActiveColorQueen();
    bool isThereInactiveColorQueen = state.isThereInactiveColorQueen();
    if (!isThereActiveColorQueen && !isThereInactiveColorQueen)
        return true;
    if (isThereActiveColorQueen) {
        double evaluation = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (!state.isActiveColorBishop(i, j) && !state.isActiveColorKnight(i, j) && !state.isActiveColorRook(i, j))
                    continue;
                evaluation += getPawnEquivalent(state.getPiece(i, j));
            }
        if (evaluation > 3)
            return false;
    }
    if (isThereInactiveColorQueen) {
        double evaluation = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (!state.isInactiveColorBishop(i, j) && !state.isInactiveColorKnight(i, j) && !state.isInactiveColorRook(i, j))
                    continue;
                evaluation += getPawnEquivalent(state.getPiece(i, j));
            }
        if (evaluation > 3)
            return false;
    }
    return true;
}
double Evaluator::getEvaluation(State& state) {
    double evaluation = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            evaluation += (state.isActiveColorPiece(i, j) ? 1 : -1) * getWeightedPawnEquivalent(state, i, j);
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
double Evaluator::getWeightedPawnEquivalent(State& state, int i, int j) {
    if (state.getPiece(i, j) == '.')
        return 0;
    if (state.getPiece(i, j) == 'P')
        return pawnPieceTable[i][j];
    if (state.getPiece(i, j) == 'p')
        return pawnPieceTable[8 - i][8 - j];
    if (state.getPiece(i, j) == 'N')
        return 3 * knightPieceTable[i][j];
    if (state.getPiece(i, j) == 'n')
        return 3 * knightPieceTable[8 - i][8 - j];
    if (state.getPiece(i, j) == 'B')
        return 3 * bishopPieceTable[i][j];
    if (state.getPiece(i, j) == 'b')
        return 3 * bishopPieceTable[8 - i][8 - j];
    if (state.getPiece(i, j) == 'R')
        return 5 * rookPieceTable[i][j];
    if (state.getPiece(i, j) == 'r')
        return 5 * rookPieceTable[8 - i][8 - j];
    if (state.getPiece(i, j) == 'Q')
        return 9 * queenPieceTable[i][j];
    if (state.getPiece(i, j) == 'q')
        return 9 * queenPieceTable[8 - i][8 - j];
    if (state.getPiece(i, j) == 'K')
        return 20 * (isEndgame(state) ? kingEndgamePieceTable[i][j] : kingMiddlegamePieceTable[i][j]);
    return 20 * (isEndgame(state) ? kingEndgamePieceTable[8 - i][8 - j] : kingMiddlegamePieceTable[8 - i][8 - j]);
}
