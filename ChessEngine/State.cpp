#include "Evaluator.h"
#include "State.h"

bool State::isBishop(int i, int j) {
    return isWhiteBishop(i, j) || isBlackBishop(i, j);
}
bool State::isBlackBishop(int i, int j) {
    return bitboards[pieceToIndex['b']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isWhiteBishop(int i, int j) {
    return bitboards[pieceToIndex['B']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isBlackKing(int i, int j) {
    return bitboards[pieceToIndex['k']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isKing(int i, int j) {
    return isWhiteKing(i, j) || isBlackKing(i, j);
}
bool State::isWhiteKing(int i, int j) {
    return bitboards[pieceToIndex['K']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isBlackKnight(int i, int j) {
    return bitboards[pieceToIndex['n']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isKnight(int i, int j) {
    return isWhiteKnight(i, j) || isBlackKnight(i, j);
}
bool State::isWhiteKnight(int i, int j) {
    return bitboards[pieceToIndex['N']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isBlackPawn(int i, int j) {
    return bitboards[pieceToIndex['p']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isPawn(int i, int j) {
    return isWhitePawn(i, j) || isBlackPawn(i, j);
}
bool State::isWhitePawn(int i, int j) {
    return bitboards[pieceToIndex['P']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isBlackQueen(int i, int j) {
    return bitboards[pieceToIndex['q']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isQueen(int i, int j) {
    return isWhiteQueen(i, j) || isBlackQueen(i, j);
}
bool State::isWhiteQueen(int i, int j) {
    return bitboards[pieceToIndex['Q']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isBlackRook(int i, int j) {
    return bitboards[pieceToIndex['r']] & (unsigned long long)1 << (8 * i + j);
}
bool State::isRook(int i, int j) {
    return isWhiteRook(i, j) || isBlackRook(i, j);
}
bool State::isWhiteRook(int i, int j) {
    return bitboards[pieceToIndex['R']] & (unsigned long long)1 << (8 * i + j);
}
State::State(string FEN) {
    for (int k = 0; k < 13; k++)
        bitboards[k] = 0;
    reverse(FEN.begin(), FEN.end());
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; ) {
            if (isalpha(FEN[FEN.size() - 1])) {
                bitboards[pieceToIndex[FEN[FEN.size() - 1]]] += (unsigned long long)1 << (8 * i + j);
                bitboards[OCCUPIED] += (unsigned long long)1 << (8 * i + j);
                get<0>(hashCode) += FEN[FEN.size() - 1];
                j++;
            }
            else if (isdigit(FEN[FEN.size() - 1])) {
                for (int k = 0; k < FEN[FEN.size() - 1] - '0'; k++)
                    get<0>(hashCode) += '.';
                j += FEN[FEN.size() - 1] - '0';
            }
            FEN.pop_back();
        }
        FEN.pop_back();
    }
    get<1>(hashCode) = FEN[FEN.size() - 1] == 'b';
    for (int k = 0; k < 2; k++)
        FEN.pop_back();
    get<2>(hashCode) = 0;
    while (true) {
        if (FEN[FEN.size() - 1] == ' ') {
            FEN.pop_back();
            break;
        }
        if (FEN[FEN.size() - 1] == 'K')
            get<2>(hashCode) += 8;
        else if (FEN[FEN.size() - 1] == 'Q')
            get<2>(hashCode) += 4;
        else if (FEN[FEN.size() - 1] == 'k')
            get<2>(hashCode) += 2;
        else if (FEN[FEN.size() - 1] == 'q') {
            get<2>(hashCode)++;
        }
        FEN.pop_back();
    }
    get<3>(hashCode) = -1;
    while (true) {
        if (FEN[FEN.size() - 1] == ' ') {
            FEN.pop_back();
            break;
        }
        if (isalpha(FEN[FEN.size() - 1]))
            get<3>(hashCode) = FEN[FEN.size() - 1] - 'a';
        FEN.pop_back();
    }
}
bool State::canActiveColorCastleKingside() {
    return (get<2>(hashCode) & 1 << (3 - 2 * getActiveColor())) > 0;
}
bool State::canActiveColorCastleQueenside() {
    return (get<2>(hashCode) & 1 << (2 - 2 * getActiveColor())) > 0;
}
bool State::getActiveColor() {
    return get<1>(hashCode);
}
tuple<string, bool, int, int> State::getHashCode() {
    return hashCode;
}
unsigned long long State::getActiveColorPawns() {
    return bitboards[getActiveColor() ? BLACK_PAWNS : WHITE_PAWNS];
}
unsigned long long State::getOccupiedSquares() {
    return bitboards[OCCUPIED];
}
char State::getPiece(int i, int j) {
    return get<0>(hashCode)[8 * i + j];
}
int State::getPossibleEnPassantTargetColumn() {
    return get<3>(hashCode);
}
int State::getPossibleEnPassantTargetRow() {
    return getActiveColor() ? 4 : 3;
}
bool State::isActiveColorBishop(int i, int j) {
    return getActiveColor() ? isBlackBishop(i, j) : isWhiteBishop(i, j);
}
bool State::isActiveColorInCheck() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            if (!isActiveColorKing(i, j))
                continue;
            if ((getActiveColor() ? i <= 5 : i >= 2) && (j - 1 >= 0 && isInactiveColorPawn(i + (getActiveColor() ? 1 : -1), j - 1) || j + 1 < 8 && isInactiveColorPawn(i + (getActiveColor() ? 1 : -1), j + 1)))
                return true;
            int di[16] = { -2, -1, 1, 2, 2, 1, -1, -2, -1, -1, 1, 1, -1, 0, 1, 0 };
            int dj[16] = { 1, 2, 2, 1, -1, -2, -2, -1, -1, 1, 1, -1, 0, 1, 0, -1 };
            for (int k = 0; k < 8; k++) {
                if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                    continue;
                if (!isInactiveColorKnight(i + di[k], j + dj[k]))
                    continue;
                return true;
            }
            for (int k = 8; k < 16; k++) {
                if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                    continue;
                if (!isInactiveColorKing(i + di[k], j + dj[k]))
                    continue;
                return true;
            }
            for (int k = 8; k < 12; k++) {
                for (int l = 1; ; l++) {
                    if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                        break;
                    if (isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                        break;
                    if (isInactiveColorBishop(i + l * di[k], j + l * dj[k]) || isInactiveColorQueen(i + l * di[k], j + l * dj[k]))
                        return true;
                    if (isInactiveColorPiece(i + l * di[k], j + l * dj[k]))
                        break;
                }
            }
            for (int k = 12; k < 16; k++) {
                for (int l = 1; ; l++) {
                    if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                        break;
                    if (isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                        break;
                    if (isInactiveColorRook(i + l * di[k], j + l * dj[k]) || isInactiveColorQueen(i + l * di[k], j + l * dj[k]))
                        return true;
                    if (isInactiveColorPiece(i + l * di[k], j + l * dj[k]))
                        break;
                }
            }
            return false;
        }
}
bool State::isActiveColorKing(int i, int j) {
    return getActiveColor() ? isBlackKing(i, j) : isWhiteKing(i, j);
}
bool State::isActiveColorKnight(int i, int j) {
    return getActiveColor() ? isBlackKnight(i, j) : isWhiteKnight(i, j);
}
bool State::isActiveColorPawn(int i, int j) {
    return getActiveColor() ? isBlackPawn(i, j) : isWhitePawn(i, j);
}
bool State::isActiveColorPiece(int i, int j) {
    return isActiveColorPawn(i, j) || isActiveColorKnight(i, j) || isActiveColorBishop(i, j) || isActiveColorRook(i, j) || isActiveColorQueen(i, j) || isActiveColorKing(i, j);
}
bool State::isActiveColorQueen(int i, int j) {
    return getActiveColor() ? isBlackQueen(i, j) : isWhiteQueen(i, j);
}
bool State::isActiveColorRook(int i, int j) {
    return getActiveColor() ? isBlackRook(i, j) : isWhiteRook(i, j);
}
bool State::isInactiveColorBishop(int i, int j) {
    return getActiveColor() ? isWhiteBishop(i, j) : isBlackBishop(i, j);
}
bool State::isInactiveColorKing(int i, int j) {
    return getActiveColor() ? isWhiteKing(i, j) : isBlackKing(i, j);
}
bool State::isInactiveColorKnight(int i, int j) {
    return getActiveColor() ? isWhiteKnight(i, j) : isBlackKnight(i, j);
}
bool State::isInactiveColorPawn(int i, int j) {
    return getActiveColor() ? isWhitePawn(i, j) : isBlackPawn(i, j);
}
bool State::isInactiveColorPiece(int i, int j) {
    return isInactiveColorPawn(i, j) || isInactiveColorKnight(i, j) || isInactiveColorBishop(i, j) || isInactiveColorRook(i, j) || isInactiveColorQueen(i, j) || isInactiveColorKing(i, j);
}
bool State::isInactiveColorQueen(int i, int j) {
    return getActiveColor() ? isWhiteQueen(i, j) : isBlackQueen(i, j);
}
bool State::isInactiveColorRook(int i, int j) {
    return getActiveColor() ? isWhiteRook(i, j) : isBlackRook(i, j);
}
bool State::isPiece(int i, int j) {
    return getPiece(i, j) != '.';
}
void State::setCanActiveColorCastleKingside(bool canActiveColorCastleKingside) {
    get<2>(hashCode) += (8 - 6 * getActiveColor()) * (canActiveColorCastleKingside - ((get<2>(hashCode) & 1 << (3 - 2 * getActiveColor())) > 0));
}
void State::setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside) {
    get<2>(hashCode) += (4 - 3 * getActiveColor()) * (canActiveColorCastleQueenside - ((get<2>(hashCode) & 1 << (2 - 2 * getActiveColor())) > 0));
}
void State::setHashCode(tuple<string, bool, int, int> hashCode) {
    for (int k = 0; k < 13; k++)
        bitboards[k] = 0;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            if (get<0>(hashCode)[8 * i + j] == '.')
                continue;
            bitboards[pieceToIndex[get<0>(hashCode)[8 * i + j]]] += (unsigned long long)1 << (8 * i + j);
            bitboards[OCCUPIED] += (unsigned long long)1 << (8 * i + j);
        }
    this->hashCode = hashCode;
}
void State::setPiece(int i, int j, char piece) {
    if (isPiece(i, j)) {
        bitboards[pieceToIndex[getPiece(i, j)]] -= (unsigned long long)1 << (8 * i + j);
        bitboards[OCCUPIED] -= (unsigned long long)1 << (8 * i + j);
    }
    if (piece != '.') {
        bitboards[pieceToIndex[piece]] += (unsigned long long)1 << (8 * i + j);
        bitboards[OCCUPIED] += (unsigned long long)1 << (8 * i + j);
    }
    get<0>(hashCode)[8 * i + j] = piece;
}
void State::setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn) {
    get<3>(hashCode) = possibleEnPassantTargetColumn;
}
void State::toggleActiveColor() {
    get<1>(hashCode) = !get<1>(hashCode);
}
