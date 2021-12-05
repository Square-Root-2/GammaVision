#include "ChessBoard.h"
#include "Evaluator.h"

enum ChessBoard::enumPiece {
    BLACK,
    WHITE,
    BISHOP,
    KING,
    KNIGHT,
    PAWN,
    QUEEN,
    ROOK
};
bool ChessBoard::isBishop(int i, int j) {
    return getPiece(i, j) == 'B' || getPiece(i, j) == 'b';
}
bool ChessBoard::isKing(int i, int j) {
    return getPiece(i, j) == 'K' || getPiece(i, j) == 'k';
}
bool ChessBoard::isKnight(int i, int j) {
    return getPiece(i, j) == 'N' || getPiece(i, j) == 'n';
}
bool ChessBoard::isPawn(int i, int j) {
    return getPiece(i, j) == 'P' || getPiece(i, j) == 'p';
}
bool ChessBoard::isQueen(int i, int j) {
    return getPiece(i, j) == 'Q' || getPiece(i, j) == 'q';
}
bool ChessBoard::isRook(int i, int j) {
    return getPiece(i, j) == 'R' || getPiece(i, j) == 'r';
}
ChessBoard::ChessBoard(string FEN) {
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
bool ChessBoard::canActiveColorCastleKingside() {
    return (get<2>(hashCode) & 1 << (3 - 2 * getActiveColor())) > 0;
}
bool ChessBoard::canActiveColorCastleQueenside() {
    return (get<2>(hashCode) & 1 << (2 - 2 * getActiveColor())) > 0;
}
bool ChessBoard::getActiveColor() {
    return get<1>(hashCode);
}
unsigned int ChessBoard::getBishops(ColorType colorType) {
    return pieceBitboards[BISHOP] & pieceBitboards[colorType];
}
unsigned int ChessBoard::getBlackBishops() {
    return pieceBitboards[BISHOP] & pieceBitboards[BLACK];
};
unsigned int ChessBoard::getBlackKings() {
    return pieceBitboards[KING] & pieceBitboards[BLACK];
}
unsigned int ChessBoard::getBlackKnights() {
    return pieceBitboards[KNIGHT] & pieceBitboards[BLACK];
}
unsigned int ChessBoard::getBlackPawns() {
    return pieceBitboards[PAWN] & pieceBitboards[BLACK];
}
unsigned int ChessBoard::getBlackQueens() {
    return pieceBitboards[QUEEN] & pieceBitboards[BLACK];
}
unsigned int ChessBoard::getBlackRooks() {
    return pieceBitboards[ROOK] & pieceBitboards[BLACK];
}
tuple<string, bool, int, int> ChessBoard::getHashCode() {
    return hashCode;
}
char ChessBoard::getPiece(int i, int j) {
    return get<0>(hashCode)[8 * i + j];
}
unsigned int ChessBoard::getKings(ColorType colorType) {
    return pieceBitboards[KING] & pieceBitboards[colorType];
}
unsigned int ChessBoard::getKnights(ColorType colorType) {
    return pieceBitboards[KNIGHT] & pieceBitboards[colorType];
}
unsigned int ChessBoard::getPawns(ColorType colorType) {
    return pieceBitboards[PAWN] & pieceBitboards[colorType];
}
unsigned int ChessBoard::getPieceSet(PieceType pieceType) {
    return pieceBitboards[pieceType];
}
int ChessBoard::getPossibleEnPassantTargetColumn() {
    return get<3>(hashCode);
}
int ChessBoard::getPossibleEnPassantTargetRow() {
    return getActiveColor() ? 4 : 3;
}
unsigned int ChessBoard::getQueens(ColorType colorType) {
    return pieceBitboards[QUEEN] & pieceBitboards[colorType];
}
unsigned int ChessBoard::getRooks(ColorType colorType) {
    return pieceBitboards[ROOK] & pieceBitboards[colorType];
}
unsigned int ChessBoard::getWhiteBishops() {
    return pieceBitboards[BISHOP] & pieceBitboards[WHITE];
};
unsigned int ChessBoard::getWhiteKings() {
    return pieceBitboards[KING] & pieceBitboards[WHITE];
}
unsigned int ChessBoard::getWhiteKnights() {
    return pieceBitboards[KNIGHT] & pieceBitboards[WHITE];
}
unsigned int ChessBoard::getWhitePawns() {
    return pieceBitboards[PAWN] & pieceBitboards[WHITE];
}
unsigned int ChessBoard::getWhiteQueens() {
    return pieceBitboards[QUEEN] & pieceBitboards[WHITE];
}
unsigned int ChessBoard::getWhiteRooks() {
    return pieceBitboards[ROOK] & pieceBitboards[WHITE];
}
bool ChessBoard::isActiveColorBishop(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'b' : 'B');
}
bool ChessBoard::isActiveColorInCheck() {
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
bool ChessBoard::isActiveColorKing(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'k' : 'K');
}
bool ChessBoard::isActiveColorKnight(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'n' : 'N');
}
bool ChessBoard::isActiveColorPawn(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'p' : 'P');
}
bool ChessBoard::isActiveColorPiece(int i, int j) {
    return isActiveColorPawn(i, j) || isActiveColorKnight(i, j) || isActiveColorBishop(i, j) || isActiveColorRook(i, j) || isActiveColorQueen(i, j) || isActiveColorKing(i, j);
}
bool ChessBoard::isActiveColorQueen(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'q' : 'Q');
}
bool ChessBoard::isActiveColorRook(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'r' : 'R');
}
bool ChessBoard::isInactiveColorBishop(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'B' : 'b');
}
bool ChessBoard::isInactiveColorKing(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'K' : 'k');
}
bool ChessBoard::isInactiveColorKnight(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'N' : 'n');
}
bool ChessBoard::isInactiveColorPawn(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'P' : 'p');
}
bool ChessBoard::isInactiveColorPiece(int i, int j) {
    return isInactiveColorPawn(i, j) || isInactiveColorKnight(i, j) || isInactiveColorBishop(i, j) || isInactiveColorRook(i, j) || isInactiveColorQueen(i, j) || isInactiveColorKing(i, j);
}
bool ChessBoard::isInactiveColorQueen(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'Q' : 'q');
}
bool ChessBoard::isInactiveColorRook(int i, int j) {
    return getPiece(i, j) == (getActiveColor() ? 'R' : 'r');
}
bool ChessBoard::isPiece(int i, int j) {
    return getPiece(i, j) != '.';
}
void ChessBoard::setCanActiveColorCastleKingside(bool canActiveColorCastleKingside) {
    get<2>(hashCode) += (8 - 6 * getActiveColor()) * (canActiveColorCastleKingside - ((get<2>(hashCode) & 1 << (3 - 2 * getActiveColor())) > 0));
}
void ChessBoard::setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside) {
    get<2>(hashCode) += (4 - 3 * getActiveColor()) * (canActiveColorCastleQueenside - ((get<2>(hashCode) & 1 << (2 - 2 * getActiveColor())) > 0));
}
void ChessBoard::setHashCode(tuple<string, bool, int, int> hashCode) {
    this->hashCode = hashCode;
}
void ChessBoard::setPiece(int i, int j, char piece) {
    get<0>(hashCode)[8 * i + j] = piece;
}
void ChessBoard::setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn) {
    get<3>(hashCode) = possibleEnPassantTargetColumn;
}
void ChessBoard::toggleActiveColor() {
    get<1>(hashCode) = !get<1>(hashCode);
}
