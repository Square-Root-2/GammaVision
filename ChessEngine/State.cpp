#include "State.h"

bool State::isBishop(int i, int j) {
    return getPiece(i, j) == 'B' || getPiece(i, j) == 'b';
}
bool State::isBishopAttackingSquare(int i, int j, int k, int l) {
    int di[4] = { -1, 1, 1, -1 };
    int dj[4] = { 1, 1, -1, -1 };
    for (int m = 0; m < 4; m++)
        for (int n = 1; ; n++) {
            if (i + n * di[m] < 0 || i + n * di[m] >= 8 || j + n * dj[m] < 0 || j + n * dj[m] >= 8)
                break;
            if (isInactiveColorPiece(i + n * di[m], j + n * dj[m]))
                break;
            if (i + n * di[m] == k && j + n * dj[m] == l)
                return true;
            if (isPiece(i + n * di[m], j + n * dj[m]))
                break;
        }
    return false;
}
bool State::isKing(int i, int j) {
    return getPiece(i, j) == 'K' || getPiece(i, j) == 'k';
}
bool State::isKingAttackingSquare(int i, int j, int k, int l) {
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int m = 0; m < 8; m++) {
        if (i + di[m] < 0 || i + di[m] >= 8 || j + dj[m] < 0 || j + dj[m] >= 8)
            continue;
        if (i + di[m] != k || j + dj[m] != l)
            continue;
        return true;
    }
    return false;
}
bool State::isKnight(int i, int j) {
    return getPiece(i, j) == 'N' || getPiece(i, j) == 'n';
}
bool State::isKnightAttackingSquare(int i, int j, int k, int l) {
    int di[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int dj[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    for (int m = 0; m < 8; m++) {
        if (i + di[m] < 0 || i + di[m] >= 8 || j + dj[m] < 0 || j + dj[m] >= 8)
            continue;
        if (i + di[m] != k || j + dj[m] != l)
            continue;
        return true;
    }
    return false;
}
bool State::isPawn(int i, int j) {
    return getPiece(i, j) == 'P' || getPiece(i, j) == 'p';
}
bool State::isPawnAttackingSquare(int i, int j, int k, int l) {
    int di[2] = { 1, -1 };
    int dj[2] = { -1, 1 };
    for (int m = 0; m < 2; m++) {
        if (j + dj[m] < 0 || j + dj[m] >= 8)
            continue;
        if (i + di[get<1>(hashCode)] != k || j + dj[m] != l)
            continue;
        return true;
    }
    return false;
}
bool State::isQueen(int i, int j) {
    return getPiece(i, j) == 'Q' || getPiece(i, j) == 'q';
}
bool State::isQueenAttackingSquare(int i, int j, int k, int l) {
    return isBishopAttackingSquare(i, j, k, l) || isRookAttackingSquare(i, j, k, l);
}
bool State::isRook(int i, int j) {
    return getPiece(i, j) == 'R' || getPiece(i, j) == 'r';
}
bool State::isRookAttackingSquare(int i, int j, int k, int l) {
    int di[4] = { -1, 0, 1, 0 };
    int dj[4] = { 0, 1, 0, -1 };
    for (int m = 0; m < 4; m++)
        for (int n = 1; ; n++) {
            if (i + n * di[m] < 0 || i + n * di[m] >= 8 || j + n * dj[m] < 0 || j + n * dj[m] >= 8)
                break;
            if (isInactiveColorPiece(i + n * di[m], j + n * dj[m]))
                break;
            if (i + n * di[m] == k && j + n * dj[m] == l)
                return true;
            if (isPiece(i + n * di[m], j + n * dj[m]))
                break;
        }
    return false;
}
State::State(string FEN) {
    reverse(FEN.begin(), FEN.end());
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; ) {
            if (isalpha(FEN[FEN.size() - 1])) {
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
    return getPiece(i, j) == (getActiveColor() ? 'b' : 'B');
}
bool State::isActiveColorInCheck() {
    int k, l;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            if (!isActiveColorKing(i, j))
                continue;
            k = i;
            l = j;
            break;
        }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (isInactiveColorPawn(i, j) && isPawnAttackingSquare(i, j, k, l))
                return true;
            else if (isInactiveColorKnight(i, j) && isKnightAttackingSquare(i, j, k, l))
                return true;
            else if (isInactiveColorBishop(i, j) && isBishopAttackingSquare(i, j, k, l))
                return true;
            else if (isInactiveColorRook(i, j) && isRookAttackingSquare(i, j, k, l))
                return true;
            else if (isInactiveColorQueen(i, j) && isQueenAttackingSquare(i, j, k, l))
                return true;
            else if (isInactiveColorKing(i, j) && isKingAttackingSquare(i, j, k, l))
                return true;
    return false;
}
bool State::isActiveColorKing(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'k' : 'K');
}
bool State::isActiveColorKnight(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'n' : 'N');
}
bool State::isActiveColorPawn(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'p' : 'P');
}
bool State::isActiveColorPiece(int i, int j) {
    return isActiveColorPawn(i, j) || isActiveColorKnight(i, j) || isActiveColorBishop(i, j) || isActiveColorRook(i, j) || isActiveColorQueen(i, j) || isActiveColorKing(i, j);
}
bool State::isActiveColorQueen(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'q' : 'Q');
}
bool State::isActiveColorRook(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'r' : 'R');
}
bool State::isInactiveColorBishop(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'B' : 'b');
}
bool State::isInactiveColorKing(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'K' : 'k');
}
bool State::isInactiveColorKnight(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'N' : 'n');
}
bool State::isInactiveColorPawn(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'P' : 'p');
}
bool State::isInactiveColorPiece(int i, int j) {
    return isInactiveColorPawn(i, j) || isInactiveColorKnight(i, j) || isInactiveColorBishop(i, j) || isInactiveColorRook(i, j) || isInactiveColorQueen(i, j) || isInactiveColorKing(i, j);
}
bool State::isInactiveColorQueen(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'Q' : 'q');
}
bool State::isInactiveColorRook(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'R' : 'r');
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
    this->hashCode = hashCode;
}
void State::setPiece(int i, int j, char piece) {
    get<0>(hashCode)[8 * i + j] = piece;
}
void State::setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn) {
    get<3>(hashCode) = possibleEnPassantTargetColumn;
}
void State::toggleActiveColor() {
    get<1>(hashCode) = !get<1>(hashCode);
}
