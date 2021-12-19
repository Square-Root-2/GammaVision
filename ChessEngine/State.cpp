#include "State.h"

bool State::isInactiveColorBishop(int i, int j) {
    return bitboards[getActiveColor() ? WHITE_BISHOPS : BLACK_BISHOPS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isInactiveColorKnight(int i, int j) {
    return bitboards[getActiveColor() ? WHITE_KNIGHTS : BLACK_KNIGHTS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isInactiveColorQueen(int i, int j) {
    return bitboards[getActiveColor() ? WHITE_QUEENS : BLACK_QUEENS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isInactiveColorRook(int i, int j) {
    return bitboards[getActiveColor() ? WHITE_ROOKS : BLACK_ROOKS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isWhite(char piece) {
    if (piece == '.')
        return false;
    return isupper(piece);
}
State::State(string FEN) {
    for (int k = WHITE_PAWNS; k <= BLACK_PIECES; k++)
        bitboards[k] = 0;
    reverse(FEN.begin(), FEN.end());
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; ) {
            if (isalpha(FEN[FEN.size() - 1])) {
                bitboards[pieceToIndex[FEN[FEN.size() - 1]]] |= (unsigned long long)1 << (8 * i + j);
                bitboards[isWhite(FEN[FEN.size() - 1]) ? WHITE_PIECES : BLACK_PIECES] |= (unsigned long long)1 << (8 * i + j);
                get<0>(uniqueHash) += FEN[FEN.size() - 1];
                j++;
            }
            else if (isdigit(FEN[FEN.size() - 1])) {
                for (int k = 0; k < FEN[FEN.size() - 1] - '0'; k++)
                    get<0>(uniqueHash) += '.';
                j += FEN[FEN.size() - 1] - '0';
            }
            FEN.pop_back();
        }
        FEN.pop_back();
    }
    get<1>(uniqueHash) = FEN[FEN.size() - 1] == 'b';
    for (int k = 0; k < 2; k++)
        FEN.pop_back();
    get<2>(uniqueHash) = 0;
    while (true) {
        if (FEN[FEN.size() - 1] == ' ') {
            FEN.pop_back();
            break;
        }
        if (FEN[FEN.size() - 1] == 'K')
            get<2>(uniqueHash) += 8;
        else if (FEN[FEN.size() - 1] == 'Q')
            get<2>(uniqueHash) += 4;
        else if (FEN[FEN.size() - 1] == 'k')
            get<2>(uniqueHash) += 2;
        else if (FEN[FEN.size() - 1] == 'q') {
            get<2>(uniqueHash)++;
        }
        FEN.pop_back();
    }
    get<3>(uniqueHash) = -1;
    while (true) {
        if (FEN[FEN.size() - 1] == ' ') {
            FEN.pop_back();
            break;
        }
        if (isalpha(FEN[FEN.size() - 1]))
            get<3>(uniqueHash) = FEN[FEN.size() - 1] - 'a';
        FEN.pop_back();
    }
    hash = 0;
    for (int k = WHITE_PAWNS; k <= BLACK_KINGS; k++)
        hash ^= bitboards[k];
}
bool State::operator==(State state) const {
    return uniqueHash == state.getUniqueHash();
}
bool State::canActiveColorCastleKingside() {
    return (get<2>(uniqueHash) & (1 << (3 - 2 * getActiveColor())));
}
bool State::canActiveColorCastleQueenside() {
    return (get<2>(uniqueHash) & (1 << (2 - 2 * getActiveColor())));
}
bool State::getActiveColor() {
    return get<1>(uniqueHash);
}
unsigned long long State::getActiveColorColumnAttackers() {
    return getActiveColor() ? bitboards[BLACK_ROOKS] | bitboards[BLACK_QUEENS] : bitboards[WHITE_ROOKS] | bitboards[WHITE_QUEENS];
}
unsigned long long State::getActiveColorDiagonalAttackers() {
    return getActiveColor() ? bitboards[BLACK_BISHOPS] | bitboards[BLACK_QUEENS] : bitboards[WHITE_BISHOPS] | bitboards[WHITE_QUEENS];
}
unsigned long long State::getActiveColorKings() {
    return bitboards[getActiveColor() ? BLACK_KINGS : WHITE_KINGS];
}
unsigned long long State::getActiveColorKnights() {
    return bitboards[getActiveColor() ? BLACK_KNIGHTS : WHITE_KNIGHTS];
}
unsigned long long State::getActiveColorPawns() {
    return bitboards[getActiveColor() ? BLACK_PAWNS : WHITE_PAWNS];
}
unsigned long long State::getActiveColorPieces() {
    return bitboards[getActiveColor() ? BLACK_PIECES : WHITE_PIECES];
}
unsigned long long State::getEmptySquares() {
    return ~bitboards[WHITE_PIECES] & ~bitboards[BLACK_PIECES];
}
unsigned long long State::getHash() {
    return hash;
}
unsigned long long State::getInactiveColorPieces() {
    return bitboards[getActiveColor() ? WHITE_PIECES : BLACK_PIECES];
}
char State::getPiece(int i, int j) {
    return get<0>(uniqueHash)[8 * i + j];
}
int State::getPossibleEnPassantTargetColumn() {
    return get<3>(uniqueHash);
}
int State::getPossibleEnPassantTargetRow() {
    if (getPossibleEnPassantTargetColumn() == -1)
        return -1;
    return getActiveColor() ? 4 : 3;
}
tuple<string, bool, int, int> State::getUniqueHash() {
    return uniqueHash;
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
    return false;
}
bool State::isActiveColorKing(int i, int j) {
    return bitboards[getActiveColor() ? BLACK_KINGS : WHITE_KINGS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isActiveColorPawn(int i, int j) {
    return bitboards[getActiveColor() ? BLACK_PAWNS : WHITE_PAWNS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isActiveColorPiece(int i, int j) {
    return getActiveColorPieces() & ((unsigned long long)1 << (8 * i + j));
}
bool State::isActiveColorRook(int i, int j) {
    return bitboards[getActiveColor() ? BLACK_ROOKS : WHITE_ROOKS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isInactiveColorKing(int i, int j) {
    return bitboards[getActiveColor() ? WHITE_KINGS : BLACK_KINGS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isInactiveColorPawn(int i, int j) {
    return bitboards[getActiveColor() ? WHITE_PAWNS : BLACK_PAWNS] & ((unsigned long long)1 << (8 * i + j));
}
bool State::isInactiveColorPiece(int i, int j) {
   return getInactiveColorPieces() & ((unsigned long long)1 << (8 * i + j));
}
bool State::isPiece(int i, int j) {
    return getPiece(i, j) != '.';
}
void State::setCanActiveColorCastleKingside(bool canActiveColorCastleKingside) {
    get<2>(uniqueHash) += (8 - 6 * getActiveColor()) * (canActiveColorCastleKingside - ((get<2>(uniqueHash) & 1 << (3 - 2 * getActiveColor())) > 0));
}
void State::setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside) {
    get<2>(uniqueHash) += (4 - 3 * getActiveColor()) * (canActiveColorCastleQueenside - ((get<2>(uniqueHash) & 1 << (2 - 2 * getActiveColor())) > 0));
}
void State::setPiece(int i, int j, char piece) {
    if (isPiece(i, j)) {
        hash ^= bitboards[pieceToIndex[getPiece(i, j)]];
        bitboards[pieceToIndex[getPiece(i, j)]] -= (unsigned long long)1 << (8 * i + j);
        hash ^= bitboards[pieceToIndex[getPiece(i, j)]];
        bitboards[isWhite(getPiece(i, j)) ? WHITE_PIECES : BLACK_PIECES] -= (unsigned long long)1 << (8 * i + j);
    }
    if (piece != '.') {
        hash ^= bitboards[pieceToIndex[piece]];
        bitboards[pieceToIndex[piece]] |= (unsigned long long)1 << (8 * i + j);
        hash ^= bitboards[pieceToIndex[piece]];
        bitboards[isWhite(piece) ? WHITE_PIECES : BLACK_PIECES] |= (unsigned long long)1 << (8 * i + j);
    }
    get<0>(uniqueHash)[8 * i + j] = piece;
}
void State::setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn) {
    get<3>(uniqueHash) = possibleEnPassantTargetColumn;
}
void State::toggleActiveColor() {
    get<1>(uniqueHash) = !get<1>(uniqueHash);
}
unsigned long long std::hash<State>::operator()(State state) const {
    return state.getHash();
}
