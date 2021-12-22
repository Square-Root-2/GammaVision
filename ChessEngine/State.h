#pragma once

#include <string>
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
        WHITE_KING,
        BLACK_KING,
        WHITE_PIECES,
        BLACK_PIECES
    };
    static const int BLACK_TO_MOVE = 768, CASTLING_RIGHTS = 769, POSSIBLE_EN_PASSANT_TARGET_COLUMN = 785;
    static bool isZobristInitialized;
    static unsigned long long Zobrist[793];
    unsigned long long bitboards[14];
    unsigned long long hash;
    unordered_map<char, BitboardType> pieceToIndex = { {'P', WHITE_PAWNS}, {'p', BLACK_PAWNS}, {'N', WHITE_KNIGHTS}, {'n', BLACK_KNIGHTS}, {'B', WHITE_BISHOPS}, {'b', BLACK_BISHOPS}, {'R', WHITE_ROOKS}, {'r', BLACK_ROOKS}, {'Q', WHITE_QUEENS}, {'q', BLACK_QUEENS}, {'K', WHITE_KING}, {'k', BLACK_KING} };
    tuple<string, bool, int, int> uniqueHash;
    tuple<string, bool, int, int> getUniqueHash();
    bool isWhite(char piece);
public:
    State(string FEN);
    bool operator==(State state) const;
    bool canActiveColorCastleKingside();
    bool canActiveColorCastleQueenside();
    bool getActiveColor();
    unsigned long long getActiveColorColumnAttackers();
    unsigned long long getActiveColorDiagonalAttackers();
    unsigned long long getActiveColorKing();
    unsigned long long getActiveColorKnights();
    unsigned long long getActiveColorPawns();
    unsigned long long getActiveColorPieces();
    unsigned long long getEmptySquares();
    unsigned long long getHash();
    unsigned long long getInactiveColorColumnAttackers();
    unsigned long long getInactiveColorDiagonalAttackers();
    unsigned long long getInactiveColorKing();
    unsigned long long getInactiveColorKnights();
    unsigned long long getInactiveColorPawns();
    unsigned long long getInactiveColorPieces();
    char getPiece(int i, int j);
    int getPossibleEnPassantTargetColumn();
    int getPossibleEnPassantTargetRow();
    static void initialize();
    bool isActiveColorKing(int i, int j);
    bool isActiveColorPawn(int i, int j);
    bool isActiveColorPiece(int i, int j);
    bool isActiveColorRook(int i, int j);
    bool isInactiveColorKing(int i, int j);
    bool isInactiveColorPawn(int i, int j);
    bool isInactiveColorPiece(int i, int j);
    bool isPiece(int i, int j);
    void setCanActiveColorCastleKingside(bool canActiveColorCastleKingside);
    void setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside);
    void setPiece(int i, int j, char piece);
    void setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn);
    void toggleActiveColor();
};

namespace std {
    template<> struct hash<State> {
        unsigned long long operator()(State state) const;
    };
}
