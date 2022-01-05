#ifndef _STATE_H_
#define _STATE_H_

#pragma once

#include <string>
#include <unordered_map>

using namespace std;

class State 
{
    enum BitboardType 
    {
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
    static const unordered_map<char, BitboardType> PIECE_TO_INDEX;
    static bool isInitialized;
    static unsigned long long Zobrist[793];
    unsigned long long bitboards[14];
    unsigned long long hash;
    tuple<string, bool, int, int> uniqueHash;
    tuple<string, bool, int, int> getUniqueHash() const;
    bool isWhite(char piece) const;
public:
    static void initialize();
    State();
    State(const string& fen);
    bool operator==(const State& state) const;
    bool canActiveColorCastleKingside() const;
    bool canActiveColorCastleQueenside() const;
    bool getActiveColor() const;
    unsigned long long getActiveColorColumnAttackers() const;
    unsigned long long getActiveColorDiagonalAttackers() const;
    unsigned long long getActiveColorKing() const;
    unsigned long long getActiveColorKnights() const;
    unsigned long long getActiveColorPawns() const;
    unsigned long long getActiveColorPieces() const;
    unsigned long long getEmptySquares() const;
    unsigned long long getHash() const;
    unsigned long long getInactiveColorColumnAttackers() const;
    unsigned long long getInactiveColorDiagonalAttackers() const;
    unsigned long long getInactiveColorKing() const;
    unsigned long long getInactiveColorKnights() const;
    unsigned long long getInactiveColorPawns() const;
    unsigned long long getInactiveColorPieces() const;
    char getPiece(int i, int j) const;
    int getPossibleEnPassantTargetColumn() const;
    int getPossibleEnPassantTargetRow() const;
    bool isActiveColorKing(int i, int j) const;
    bool isActiveColorPawn(int i, int j) const;
    bool isActiveColorPiece(int i, int j) const;
    bool isActiveColorRook(int i, int j) const;
    bool isInactiveColorKing(int i, int j) const;
    bool isInactiveColorPawn(int i, int j) const;
    bool isInactiveColorPiece(int i, int j) const;
    bool isPiece(int i, int j) const;
    void setCanActiveColorCastleKingside(bool canActiveColorCastleKingside);
    void setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside);
    void setPiece(int i, int j, char piece);
    void setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn);
    void toggleActiveColor();
};

inline State::State() : State("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") 
{

}
inline tuple<string, bool, int, int> State::getUniqueHash() const 
{
    return uniqueHash;
}
inline bool State::isWhite(char piece) const 
{
    if (piece == '.')
        return false;
    return isupper(piece);
}
inline bool State::operator==(const State& state) const 
{
    return uniqueHash == state.getUniqueHash();
}
inline bool State::canActiveColorCastleKingside() const 
{
    return (get<2>(uniqueHash) & (1 << (3 - 2 * getActiveColor())));
}
inline bool State::canActiveColorCastleQueenside() const 
{
    return (get<2>(uniqueHash) & (1 << (2 - 2 * getActiveColor())));
}
inline bool State::getActiveColor() const 
{
    return get<1>(uniqueHash);
}
inline unsigned long long State::getActiveColorColumnAttackers() const 
{
    return getActiveColor() ? bitboards[BLACK_ROOKS] | bitboards[BLACK_QUEENS] : bitboards[WHITE_ROOKS] | bitboards[WHITE_QUEENS];
}
inline unsigned long long State::getActiveColorDiagonalAttackers() const 
{
    return getActiveColor() ? bitboards[BLACK_BISHOPS] | bitboards[BLACK_QUEENS] : bitboards[WHITE_BISHOPS] | bitboards[WHITE_QUEENS];
}
inline unsigned long long State::getActiveColorKing() const 
{
    return bitboards[getActiveColor() ? BLACK_KING : WHITE_KING];
}
inline unsigned long long State::getActiveColorKnights() const 
{
    return bitboards[getActiveColor() ? BLACK_KNIGHTS : WHITE_KNIGHTS];
}
inline unsigned long long State::getActiveColorPawns() const 
{
    return bitboards[getActiveColor() ? BLACK_PAWNS : WHITE_PAWNS];
}
inline unsigned long long State::getActiveColorPieces() const 
{
    return bitboards[getActiveColor() ? BLACK_PIECES : WHITE_PIECES];
}
inline unsigned long long State::getEmptySquares() const 
{
    return ~bitboards[WHITE_PIECES] & ~bitboards[BLACK_PIECES];
}
inline unsigned long long State::getHash() const 
{
    return hash;
}
inline unsigned long long State::getInactiveColorColumnAttackers() const 
{
    return getActiveColor() ? bitboards[WHITE_ROOKS] | bitboards[WHITE_QUEENS] : bitboards[BLACK_ROOKS] | bitboards[BLACK_QUEENS];
}
inline unsigned long long State::getInactiveColorDiagonalAttackers() const 
{
    return getActiveColor() ? bitboards[WHITE_BISHOPS] | bitboards[WHITE_QUEENS] : bitboards[BLACK_BISHOPS] | bitboards[BLACK_QUEENS];
}
inline unsigned long long State::getInactiveColorKing() const 
{
    return bitboards[getActiveColor() ? WHITE_KING : BLACK_KING];
}
inline unsigned long long State::getInactiveColorKnights() const 
{
    return bitboards[getActiveColor() ? WHITE_KNIGHTS : BLACK_KNIGHTS];
}
inline unsigned long long State::getInactiveColorPawns() const 
{
    return bitboards[getActiveColor() ? WHITE_PAWNS : BLACK_PAWNS];
}
inline unsigned long long State::getInactiveColorPieces() const 
{
    return bitboards[getActiveColor() ? WHITE_PIECES : BLACK_PIECES];
}
inline char State::getPiece(int i, int j) const 
{
    return get<0>(uniqueHash)[8 * i + j];
}
inline int State::getPossibleEnPassantTargetColumn() const 
{
    return get<3>(uniqueHash);
}
inline int State::getPossibleEnPassantTargetRow() const
{
    if (getPossibleEnPassantTargetColumn() == -1)
        return -1;
    return getActiveColor() ? 4 : 3;
}
inline bool State::isActiveColorKing(int i, int j) const 
{
    return bitboards[getActiveColor() ? BLACK_KING : WHITE_KING] & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isActiveColorPawn(int i, int j) const 
{
    return bitboards[getActiveColor() ? BLACK_PAWNS : WHITE_PAWNS] & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isActiveColorPiece(int i, int j) const 
{
    return getActiveColorPieces() & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isActiveColorRook(int i, int j) const 
{
    return bitboards[getActiveColor() ? BLACK_ROOKS : WHITE_ROOKS] & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isInactiveColorKing(int i, int j) const 
{
    return bitboards[getActiveColor() ? WHITE_KING : BLACK_KING] & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isInactiveColorPawn(int i, int j) const 
{
    return bitboards[getActiveColor() ? WHITE_PAWNS : BLACK_PAWNS] & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isInactiveColorPiece(int i, int j) const 
{
    return getInactiveColorPieces() & ((unsigned long long)1 << (8 * i + j));
}
inline bool State::isPiece(int i, int j) const 
{
    return getPiece(i, j) != '.';
}
inline void State::setCanActiveColorCastleKingside(bool canActiveColorCastleKingside) 
{
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
    get<2>(uniqueHash) += (8 - 6 * getActiveColor()) * (canActiveColorCastleKingside - ((get<2>(uniqueHash) & 1 << (3 - 2 * getActiveColor())) > 0));
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
}
inline void State::setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside) 
{
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
    get<2>(uniqueHash) += (4 - 3 * getActiveColor()) * (canActiveColorCastleQueenside - ((get<2>(uniqueHash) & 1 << (2 - 2 * getActiveColor())) > 0));
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
}
inline void State::setPiece(int i, int j, char piece) 
{
    if (isPiece(i, j)) 
    {
        hash ^= Zobrist[64 * PIECE_TO_INDEX.find(getPiece(i, j))->second + (8 * i + j)];
        bitboards[PIECE_TO_INDEX.find(getPiece(i, j))->second] -= (unsigned long long)1 << (8 * i + j);
        bitboards[isWhite(getPiece(i, j)) ? WHITE_PIECES : BLACK_PIECES] -= (unsigned long long)1 << (8 * i + j);
    }
    if (piece != '.') 
    {
        hash ^= Zobrist[64 * PIECE_TO_INDEX.find(piece)->second + (8 * i + j)];
        bitboards[PIECE_TO_INDEX.find(piece)->second] |= (unsigned long long)1 << (8 * i + j);
        bitboards[isWhite(piece) ? WHITE_PIECES : BLACK_PIECES] |= (unsigned long long)1 << (8 * i + j);
    }
    get<0>(uniqueHash)[8 * i + j] = piece;
}
inline void State::setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn) 
{
    if (getPossibleEnPassantTargetColumn() != -1)
        hash ^= Zobrist[POSSIBLE_EN_PASSANT_TARGET_COLUMN + getPossibleEnPassantTargetColumn()];
    if (possibleEnPassantTargetColumn != -1)
        hash ^= Zobrist[POSSIBLE_EN_PASSANT_TARGET_COLUMN + possibleEnPassantTargetColumn];
    get<3>(uniqueHash) = possibleEnPassantTargetColumn;
}
inline void State::toggleActiveColor() 
{
    hash ^= Zobrist[BLACK_TO_MOVE];
    get<1>(uniqueHash) = !get<1>(uniqueHash);
}

namespace std 
{
    template<> struct hash<State> 
    {
        unsigned long long operator()(const State& state) const;
    };
}

inline unsigned long long std::hash<State>::operator()(const State& state) const 
{
    return state.getHash();
}

#endif // _STATE_H_
