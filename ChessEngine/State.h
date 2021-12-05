#pragma once

#include <string>
#include <tuple>

using namespace std;

class State {
    tuple<string, bool, int, int> hashCode;
    bool isBishop(int i, int j);
    bool isKing(int i, int j);
    bool isKnight(int i, int j);
    bool isPawn(int i, int j);
    bool isQueen(int i, int j);
    bool isRook(int i, int j);
public:
    State(string FEN);
    bool canActiveColorCastleKingside();
    bool canActiveColorCastleQueenside();
    bool getActiveColor();
    tuple<string, bool, int, int> getHashCode();
    char getPiece(int i, int j);
    int getPossibleEnPassantTargetColumn();
    int getPossibleEnPassantTargetRow();
    bool isActiveColorBishop(int i, int j);
    bool isActiveColorInCheck();
    bool isActiveColorKing(int i, int j);
    bool isActiveColorKnight(int i, int j);
    bool isActiveColorPawn(int i, int j);
    bool isActiveColorPiece(int i, int j);
    bool isActiveColorQueen(int i, int j);
    bool isActiveColorRook(int i, int j);
    bool isInactiveColorBishop(int i, int j);
    bool isInactiveColorKing(int i, int j);
    bool isInactiveColorKnight(int i, int j);
    bool isInactiveColorPawn(int i, int j);
    bool isInactiveColorPiece(int i, int j);
    bool isInactiveColorQueen(int i, int j);
    bool isInactiveColorRook(int i, int j);
    bool isPiece(int i, int j);
    void setCanActiveColorCastleKingside(bool canActiveColorCastleKingside);
    void setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside);
    void setHashCode(tuple<string, bool, int, int> hashCode);
    void setPiece(int i, int j, char piece);
    void setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn);
    void toggleActiveColor();
};
