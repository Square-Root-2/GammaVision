#pragma once

#include "ColorType.h"
#include "PieceType.h"
#include <string>
#include <tuple>

using namespace std;

class ChessBoard {
    enum enumPiece;
    unsigned int pieceBitboards[8];
    tuple<string, bool, int, int> hashCode;
    bool isBishop(int i, int j);
    bool isKing(int i, int j);
    bool isKnight(int i, int j);
    bool isPawn(int i, int j);
    bool isQueen(int i, int j);
    bool isRook(int i, int j);
public:
    ChessBoard(string FEN);
    bool canActiveColorCastleKingside();
    bool canActiveColorCastleQueenside();
    bool getActiveColor();
    unsigned int getBishops(ColorType colorType);
    unsigned int getBlackBishops();
    unsigned int getBlackKings();
    unsigned int getBlackKnights();
    unsigned int getBlackPawns();
    unsigned int getBlackQueens();
    unsigned int getBlackRooks();
    tuple<string, bool, int, int> getHashCode();
    unsigned int getKings(ColorType colorType);
    unsigned int getKnights(ColorType colorType);
    unsigned int getPawns(ColorType colorType);
    char getPiece(int i, int j);
    unsigned int getPieceSet(PieceType pieceType);
    int getPossibleEnPassantTargetColumn();
    int getPossibleEnPassantTargetRow();
    unsigned int getQueens(ColorType colorType);
    unsigned int getRooks(ColorType colorType);
    unsigned int getWhiteBishops();
    unsigned int getWhiteKings();
    unsigned int getWhiteKnights();
    unsigned int getWhitePawns();
    unsigned int getWhiteQueens();
    unsigned int getWhiteRooks();
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
