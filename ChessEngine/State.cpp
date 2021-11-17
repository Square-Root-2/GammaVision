#include "State.h"

State::State() {

}
bool State::isBishop(int i, int j) {
    return getPiece(i, j) == 'B' || getPiece(i, j) == 'b';
}
bool State::isKing(int i, int j) {
    return getPiece(i, j) == 'K' || getPiece(i, j) == 'k';
}
bool State::isKnight(int i, int j) {
    return getPiece(i, j) == 'N' || getPiece(i, j) == 'n';
}
bool State::isPawn(int i, int j) {
    return getPiece(i, j) == 'P' || getPiece(i, j) == 'p';
}
bool State::isQueen(int i, int j) {
    return getPiece(i, j) == 'Q' || getPiece(i, j) == 'q';
}
bool State::isRook(int i, int j) {
    return getPiece(i, j) == 'R' || getPiece(i, j) == 'r';
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
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            if (!isActiveColorKing(i, j))
                continue;
            if ((getActiveColor() ? i <= 5 : i >= 2) && (isInactiveColorPawn(i + (getActiveColor() ? 1 : -1), j - 1) || isInactiveColorPawn(i + (getActiveColor() ? 1 : -1), j + 1)))
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
void State::makeMove(Move move) {
    setPiece(move.getEndRow(), move.getEndColumn(), getPiece(move.getBeginRow(), move.getBeginColumn()));
    setPiece(move.getBeginRow(), move.getBeginColumn(), '.');
    if (move.getType() == MoveType::KINGSIDE_CASTLE) {
        setPiece(move.getBeginRow(), 7, '.');
        setPiece(move.getBeginRow(), 5, getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::QUEENSIDE_CASTLE) {
        setPiece(move.getBeginRow(), 0, '.');
        setPiece(move.getBeginRow(), 3, getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::EN_PASSANT)
        setPiece(move.getBeginRow(), move.getEndColumn(), '.');
    else if (move.getType() == MoveType::PROMOTION_TO_BISHOP)
        setPiece(move.getEndRow(), move.getEndColumn(), getActiveColor() ? 'b' : 'B');
    else if (move.getType() == MoveType::PROMOTION_TO_KNIGHT)
        setPiece(move.getEndRow(), move.getEndColumn(), getActiveColor() ? 'n' : 'N');
    else if (move.getType() == MoveType::PROMOTION_TO_QUEEN)
        setPiece(move.getEndRow(), move.getEndColumn(), getActiveColor() ? 'q' : 'Q');
    else if (move.getType() == MoveType::PROMOTION_TO_ROOK)
        setPiece(move.getEndRow(), move.getEndColumn(), getActiveColor() ? 'r' : 'R');
    if (move.getType() == MoveType::KINGSIDE_CASTLE || move.getType() == MoveType::QUEENSIDE_CASTLE) {
        setCanActiveColorCastleKingside(false);
        setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::KING_MOVE) {
        setCanActiveColorCastleKingside(false);
        setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 7)
        setCanActiveColorCastleKingside(false);
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 0)
        setCanActiveColorCastleQueenside(false);
    if (move.getType() == MoveType::PAWN_FORWARD_TWO)
        setPossibleEnPassantTargetColumn(move.getBeginColumn());
    else
        setPossibleEnPassantTargetColumn(-1);
    toggleActiveColor();
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
