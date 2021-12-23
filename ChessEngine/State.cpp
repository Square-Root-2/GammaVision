#include <chrono>
#include "State.h"
#include <random>

const unordered_map<char, State::BitboardType> State::PIECE_TO_INDEX = 
{ 
    { 'P', WHITE_PAWNS}, 
    { 'p', BLACK_PAWNS }, 
    { 'N', WHITE_KNIGHTS }, 
    { 'n', BLACK_KNIGHTS }, 
    { 'B', WHITE_BISHOPS }, 
    { 'b', BLACK_BISHOPS }, 
    { 'R', WHITE_ROOKS }, 
    { 'r', BLACK_ROOKS }, 
    { 'Q', WHITE_QUEENS }, 
    { 'q', BLACK_QUEENS }, 
    { 'K', WHITE_KING }, 
    { 'k', BLACK_KING } 
};
bool State::isInitialized = false;
unsigned long long State::Zobrist[793];
tuple<string, bool, int, int> State::getUniqueHash() {
    return uniqueHash;
}
bool State::isWhite(char piece) {
    if (piece == '.')
        return false;
    return isupper(piece);
}
State::State(string FEN) {
    initialize();
    for (int k = WHITE_PAWNS; k <= BLACK_PIECES; k++)
        bitboards[k] = 0;
    hash = 0;
    reverse(FEN.begin(), FEN.end());
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; ) {
            if (isalpha(FEN[FEN.size() - 1])) {
                bitboards[PIECE_TO_INDEX.find(FEN[FEN.size() - 1])->second] |= (unsigned long long)1 << (8 * i + j);
                bitboards[isWhite(FEN[FEN.size() - 1]) ? WHITE_PIECES : BLACK_PIECES] |= (unsigned long long)1 << (8 * i + j);
                hash ^= Zobrist[64 * PIECE_TO_INDEX.find(FEN[FEN.size() - 1])->second + (8 * i + j)];
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
    if (get<1>(uniqueHash))
        hash ^= Zobrist[BLACK_TO_MOVE];
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
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
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
    if (get<3>(uniqueHash) != -1)
        hash ^= Zobrist[POSSIBLE_EN_PASSANT_TARGET_COLUMN + get<3>(uniqueHash)];
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
unsigned long long State::getActiveColorKing() {
    return bitboards[getActiveColor() ? BLACK_KING : WHITE_KING];
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
unsigned long long State::getInactiveColorColumnAttackers() {
    return getActiveColor() ? bitboards[WHITE_ROOKS] | bitboards[WHITE_QUEENS] : bitboards[BLACK_ROOKS] | bitboards[BLACK_QUEENS];
}
unsigned long long State::getInactiveColorDiagonalAttackers() {
    return getActiveColor() ? bitboards[WHITE_BISHOPS] | bitboards[WHITE_QUEENS] : bitboards[BLACK_BISHOPS] | bitboards[BLACK_QUEENS];
}
unsigned long long State::getInactiveColorKing() {
    return bitboards[getActiveColor() ? WHITE_KING : BLACK_KING];
}
unsigned long long State::getInactiveColorKnights() {
    return bitboards[getActiveColor() ? WHITE_KNIGHTS : BLACK_KNIGHTS];
}
unsigned long long State::getInactiveColorPawns() {
    return bitboards[getActiveColor() ? WHITE_PAWNS : BLACK_PAWNS];
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
void State::initialize() {
    if (isInitialized)
        return;
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    for (int k = 0; k < 793; k++)
        Zobrist[k] = rng();
    isInitialized = true;
}
bool State::isActiveColorKing(int i, int j) {
    return bitboards[getActiveColor() ? BLACK_KING : WHITE_KING] & ((unsigned long long)1 << (8 * i + j));
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
    return bitboards[getActiveColor() ? WHITE_KING : BLACK_KING] & ((unsigned long long)1 << (8 * i + j));
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
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
    get<2>(uniqueHash) += (8 - 6 * getActiveColor()) * (canActiveColorCastleKingside - ((get<2>(uniqueHash) & 1 << (3 - 2 * getActiveColor())) > 0));
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
}
void State::setCanActiveColorCastleQueenside(bool canActiveColorCastleQueenside) {
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
    get<2>(uniqueHash) += (4 - 3 * getActiveColor()) * (canActiveColorCastleQueenside - ((get<2>(uniqueHash) & 1 << (2 - 2 * getActiveColor())) > 0));
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
}
void State::setPiece(int i, int j, char piece) {
    if (isPiece(i, j)) {
        hash ^= Zobrist[64 * PIECE_TO_INDEX.find(getPiece(i, j))->second + (8 * i + j)];
        bitboards[PIECE_TO_INDEX.find(getPiece(i, j))->second] -= (unsigned long long)1 << (8 * i + j);
        bitboards[isWhite(getPiece(i, j)) ? WHITE_PIECES : BLACK_PIECES] -= (unsigned long long)1 << (8 * i + j);
    }
    if (piece != '.') {
        hash ^= Zobrist[64 * PIECE_TO_INDEX.find(piece)->second + (8 * i + j)];
        bitboards[PIECE_TO_INDEX.find(piece)->second] |= (unsigned long long)1 << (8 * i + j);
        bitboards[isWhite(piece) ? WHITE_PIECES : BLACK_PIECES] |= (unsigned long long)1 << (8 * i + j);
    }
    get<0>(uniqueHash)[8 * i + j] = piece;
}
void State::setPossibleEnPassantTargetColumn(int possibleEnPassantTargetColumn) {
    if (getPossibleEnPassantTargetColumn() != -1)
        hash ^= Zobrist[POSSIBLE_EN_PASSANT_TARGET_COLUMN + getPossibleEnPassantTargetColumn()];
    if (possibleEnPassantTargetColumn != -1)
        hash ^= Zobrist[POSSIBLE_EN_PASSANT_TARGET_COLUMN + possibleEnPassantTargetColumn];
    get<3>(uniqueHash) = possibleEnPassantTargetColumn;
}
void State::toggleActiveColor() {
    hash ^= Zobrist[BLACK_TO_MOVE];
    get<1>(uniqueHash) = !get<1>(uniqueHash);
}
unsigned long long std::hash<State>::operator()(State state) const {
    return state.getHash();
}
