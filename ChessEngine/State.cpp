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
void State::initialize()
{
    if (isInitialized)
        return;
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    for (int k = 0; k < 793; k++)
        Zobrist[k] = rng();
    isInitialized = true;
}
State::State(const string& FEN) 
{
    string fen(FEN);
    initialize();
    for (int k = WHITE_PAWNS; k <= BLACK_PIECES; k++)
        bitboards[k] = 0;
    hash = 0;
    reverse(fen.begin(), fen.end());
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 8; ) 
        {
            if (isalpha(fen[fen.size() - 1])) 
            {
                bitboards[PIECE_TO_INDEX.find(fen[fen.size() - 1])->second] |= (unsigned long long)1 << (8 * i + j);
                bitboards[isWhite(fen[fen.size() - 1]) ? WHITE_PIECES : BLACK_PIECES] |= (unsigned long long)1 << (8 * i + j);
                hash ^= Zobrist[64 * PIECE_TO_INDEX.find(fen[fen.size() - 1])->second + (8 * i + j)];
                get<0>(uniqueHash) += fen[fen.size() - 1];
                j++;
            }
            else if (isdigit(fen[fen.size() - 1])) 
            {
                for (int k = 0; k < fen[fen.size() - 1] - '0'; k++)
                    get<0>(uniqueHash) += '.';
                j += fen[fen.size() - 1] - '0';
            }
            fen.pop_back();
        }
        fen.pop_back();
    }
    get<1>(uniqueHash) = fen[fen.size() - 1] == 'b';
    if (get<1>(uniqueHash))
        hash ^= Zobrist[BLACK_TO_MOVE];
    for (int k = 0; k < 2; k++)
        fen.pop_back();
    get<2>(uniqueHash) = 0;
    while (true) 
    {
        if (fen[fen.size() - 1] == ' ') 
        {
            fen.pop_back();
            break;
        }
        if (fen[fen.size() - 1] == 'K')
            get<2>(uniqueHash) += 8;
        else if (fen[fen.size() - 1] == 'Q')
            get<2>(uniqueHash) += 4;
        else if (fen[fen.size() - 1] == 'k')
            get<2>(uniqueHash) += 2;
        else if (fen[fen.size() - 1] == 'q')
            get<2>(uniqueHash)++;
        fen.pop_back();
    }
    hash ^= Zobrist[CASTLING_RIGHTS + get<2>(uniqueHash)];
    get<3>(uniqueHash) = -1;
    while (true) 
    {
        if (fen[fen.size() - 1] == ' ') 
        {
            fen.pop_back();
            break;
        }
        if (isalpha(fen[fen.size() - 1]))
            get<3>(uniqueHash) = fen[fen.size() - 1] - 'a';
        fen.pop_back();
    }
    if (get<3>(uniqueHash) != -1)
        hash ^= Zobrist[POSSIBLE_EN_PASSANT_TARGET_COLUMN + get<3>(uniqueHash)];
}
