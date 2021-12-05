#include "Evaluator.h"

unordered_map<char, int> Evaluator::pieceToIndex = { { 'P', 0 }, { 'p', 1 }, { 'N', 2 }, { 'n', 3 }, { 'B', 4 }, { 'b', 5 }, { 'R', 6 }, { 'r', 7 }, { 'Q', 8 }, { 'q', 9 }, { 'K', 10 }, { 'k', 11 } };
bool Evaluator::isEndgame(ChessBoard& chessBoard) {
    bool isThereActiveColorQueen = false;
    bool isThereInactiveColorQueen = false;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            isThereActiveColorQueen = isThereActiveColorQueen || chessBoard.isActiveColorQueen(i, j);
            isThereInactiveColorQueen = isThereInactiveColorQueen || chessBoard.isInactiveColorQueen(i, j);
        }
    if (!isThereActiveColorQueen && !isThereInactiveColorQueen)
        return true;
    if (isThereActiveColorQueen) {
        int centipawns = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (chessBoard.isActiveColorRook(i, j))
                    return false;
                if (!chessBoard.isActiveColorKnight(i, j) && !chessBoard.isActiveColorBishop(i, j) && !chessBoard.isActiveColorQueen(i, j))
                    continue;
                centipawns += getCentipawnEquivalent(chessBoard.getPiece(i, j));
            }
        if (centipawns > 1230)
            return false;
    }
    if (isThereInactiveColorQueen) {
        int centipawns = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (chessBoard.isInactiveColorRook(i, j))
                    return false;
                if (!chessBoard.isInactiveColorKnight(i, j) && !chessBoard.isInactiveColorBishop(i, j) && !chessBoard.isInactiveColorQueen(i, j))
                    continue;
                centipawns += getCentipawnEquivalent(chessBoard.getPiece(i, j));
            }
        if (centipawns > 1230)
            return false;
    }
    return true;
}
int Evaluator::getAdjustedCentipawnEquivalent(ChessBoard& chessBoard, int i, int j) {
    if (chessBoard.getPiece(i, j) == '.')
        return 0;
    return getCentipawnEquivalent(chessBoard.getPiece(i, j)) + PIECE_SQUARE_TABLES[2 * pieceToIndex[chessBoard.getPiece(i, j)] + isEndgame(chessBoard)][i][j];
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
int Evaluator::getEvaluation(ChessBoard& chessBoard) {
    int evaluation = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            evaluation += (chessBoard.isActiveColorPiece(i, j) ? 1 : -1) * getAdjustedCentipawnEquivalent(chessBoard, i, j);
    return evaluation;
}
int Evaluator::getMaximumEvaluation() {
    return MAXIMUM_EVALUATION;
}
