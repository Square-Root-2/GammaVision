#pragma once

#include <string>
#include <tuple>
#include <unordered_map>

using namespace std;

class State {
    enum BitboardType {
        WHITE_PAWNS,
        BLACK_PAWNS,
        WHITE_KNIGHTS,
        BLACK_KNIGHTS,
        WHITE_BISHOPS,
        BLACK_BISHOPS,
        WHITE_ROOKS,
        BLACK_ROOKS,
        WHITE_QUEENS,
        BLACK_QUEENS,
        WHITE_KINGS,
        BLACK_KINGS,
    };
    unsigned long long bitboards[12];
    unordered_map<char, BitboardType> pieceToIndex = { {'P', WHITE_PAWNS}, {'p',  BLACK_PAWNS}, {'N',  WHITE_KNIGHTS}, {'n',  BLACK_KNIGHTS}, {'B',  WHITE_BISHOPS}, {'b',  BLACK_BISHOPS}, {'R',  WHITE_ROOKS}, {'r',  BLACK_ROOKS}, {'Q',  WHITE_QUEENS}, {'q',  BLACK_QUEENS}, {'K',  WHITE_KINGS}, {'k',  BLACK_KINGS} };
    tuple<string, bool, int, int> hashCode;
    bool isBishop(int i, int j);
    bool isBlackBishop(int i, int j);
    bool isWhiteBishop(int i, int j);
    bool isBlackKing(int i, int j);
    bool isKing(int i, int j);
    bool isWhiteKing(int i, int j);
    bool isBlackKnight(int i, int j);
    bool isKnight(int i, int j);
    bool isWhiteKnight(int i, int j);
    bool isBlackPawn(int i, int j);
    bool isPawn(int i, int j);
    bool isWhitePawn(int i, int j);
    bool isBlackQueen(int i, int j);
    bool isQueen(int i, int j);
    bool isWhiteQueen(int i, int j);
    bool isBlackRook(int i, int j);
    bool isRook(int i, int j);
    bool isWhiteRook(int i, int j);
public:
    State(string FEN);
    bool canActiveColorCastleKingside();
    bool canActiveColorCastleQueenside();
    bool getActiveColor();
    tuple<string, bool, int, int> getHashCode();
    unsigned long long getActiveColorPawns();
    unsigned long long getEmptySquares();
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
