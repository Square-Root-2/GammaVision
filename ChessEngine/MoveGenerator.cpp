#include <chrono>
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

unsigned long long MoveGenerator::attackSets[12][64];
unsigned long long MoveGenerator::magicNumbers[2][64];
bool MoveGenerator::isInitialized = false;
int MoveGenerator::keySizes[2][64];
vector<unsigned long long> MoveGenerator::magicAttackSets[2][64];
void MoveGenerator::generateEastAttackSets() 
{
    for (int i = 0; i < 8; i++) 
    {
        unsigned long long eastAttackSet = 0;
        for (int j = 7; j >= 0; j--) 
        {
            attackSets[EAST][8 * i + j] = eastAttackSet;
            eastAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::generateKingAttackSets() 
{
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) 
        {
            attackSets[KING][8 * i + j] = 0;
            for (int k = 0; k < 8; k++) 
            {
                if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                    continue;
                attackSets[KING][8 * i + j] |= (unsigned long long)1 << (8 * (i + di[k]) + (j + dj[k]));
            }
        }
}
void MoveGenerator::generateKnightAttackSets()
{
    int di[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int dj[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) 
        {
            attackSets[KNIGHT][8 * i + j] = 0;
            for (int k = 0; k < 8; k++) 
            {
                if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                    continue;
                attackSets[KNIGHT][8 * i + j] |= (unsigned long long)1 << (8 * (i + di[k]) + (j + dj[k]));
            }
        }
}
void MoveGenerator::generateMaskedBlockersBitboards(vector<unsigned long long>& maskedBlockersBitboards, unsigned long long maskedBlockers, int k) {
    if (k == 64)
    {
        maskedBlockersBitboards.push_back(maskedBlockers);
        return;
    }
    generateMaskedBlockersBitboards(maskedBlockersBitboards, maskedBlockers, k + 1);
    if (!(maskedBlockers & ((unsigned long long)1 << k)))
        return;
    maskedBlockers -= (unsigned long long)1 << k;
    generateMaskedBlockersBitboards(maskedBlockersBitboards, maskedBlockers, k + 1);
    maskedBlockers |= (unsigned long long)1 << k;
}
void MoveGenerator::generateNorthAttackSets() 
{
    for (int j = 0; j < 8; j++) 
    {
        unsigned long long northAttackSet = 0;
        for (int i = 0; i < 8; i++) 
        {
            attackSets[NORTH][8 * i + j] = northAttackSet;
            northAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::generateNortheastAttackSets() 
{
    for (int k = 0; k < 8; k++) 
    {
        unsigned long long northeastAttackSet = 0;
        int i = 0;
        int j = k;
        while (i < 8 && j >= 0) 
        {
            attackSets[NORTHEAST][8 * i + j] = northeastAttackSet;
            northeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j--;
        }
    }
    for (int k = 1; k < 8; k++) 
    {
        unsigned long long northeastAttackSet = 0;
        int i = k;
        int j = 7;
        while (i < 8 && j >= 0) 
        {
            attackSets[NORTHEAST][8 * i + j] = northeastAttackSet;
            northeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j--;
        }
    }
}
void MoveGenerator::generateNorthwestAttackSets()
{
    for (int k = 0; k < 8; k++) 
    {
        unsigned long long northwestAttackSet = 0;
        int i = 0;
        int j = k;
        while (i < 8 && j < 8) 
        {
            attackSets[NORTHWEST][8 * i + j] = northwestAttackSet;
            northwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j++;
        }
    }
    for (int k = 1; k < 8; k++) 
    {
        unsigned long long northwestAttackSet = 0;
        int i = k;
        int j = 0;
        while (i < 8 && j < 8) 
        {
            attackSets[NORTHWEST][8 * i + j] = northwestAttackSet;
            northwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j++;
        }
    }
}
void MoveGenerator::generatePawnAttackSets()
{
    int di[2] = { -1, 1 };
    int dj[2] = { -1, 1 };
    for (int l = WHITE_PAWN; l <= BLACK_PAWN; l++)
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) 
            {
                attackSets[l][8 * i + j] = 0;
                for (int k = 0; k < 2; k++) 
                {
                    if (i + di[l] < 0 || i + di[l] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                        continue;
                    attackSets[l][8 * i + j] |= (unsigned long long)1 << (8 * (i + di[l]) + (j + dj[k]));
                }
            }
}
void MoveGenerator::generateSouthAttackSets() 
{
    for (int j = 0; j < 8; j++) 
    {
        unsigned long long southAttackSet = 0;
        for (int i = 7; i >= 0; i--) 
        {
            attackSets[SOUTH][8 * i + j] = southAttackSet;
            southAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::generateSoutheastAttackSets() 
{
    for (int k = 0; k < 8; k++) 
    {
        unsigned long long southeastAttackSet = 0;
        int i = 7;
        int j = k;
        while (i >= 0 && j >= 0) 
        {
            attackSets[SOUTHEAST][8 * i + j] = southeastAttackSet;
            southeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j--;
        }
    }
    for (int k = 0; k < 7; k++) 
    {
        unsigned long long southeastAttackSet = 0;
        int i = k;
        int j = 7;
        while (i >= 0 && j >= 0) 
        {
            attackSets[SOUTHEAST][8 * i + j] = southeastAttackSet;
            southeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j--;
        }
    }
}
void MoveGenerator::generateSouthwestAttackSets() 
{
    for (int k = 0; k < 8; k++) 
    {
        unsigned long long southwestAttackSet = 0;
        int i = 7;
        int j = k;
        while (i >= 0 && j < 8) 
        {
            attackSets[SOUTHWEST][8 * i + j] = southwestAttackSet;
            southwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j++;
        }
    }
    for (int k = 0; k < 7; k++) 
    {
        unsigned long long southwestAttackSet = 0;
        int i = k;
        int j = 0;
        while (i >= 0 && j < 8) 
        {
            attackSets[SOUTHWEST][8 * i + j] = southwestAttackSet;
            southwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j++;
        }
    }
}
void MoveGenerator::generateWestAttackSets() 
{
    for (int i = 0; i < 8; i++) 
    {
        unsigned long long westAttackSet = 0;
        for (int j = 0; j < 8; j++) 
        {
            attackSets[WEST][8 * i + j] = westAttackSet;
            westAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::getColumnAttackerMoves(vector<Move>& moves, const State& state) 
{
    unsigned long long columnAttackers = state.getActiveColorColumnAttackers();
    while (columnAttackers > 0) 
    {
        unsigned long m;
        _BitScanForward64(&m, columnAttackers);
        int i = m / 8;
        int j = m % 8;
        unsigned long long columnAttackerAttackSet = getMagicAttackSet(getMaskedBlockers(~state.getEmptySquares(), COLUMN, m), COLUMN, m) & ~state.getActiveColorPieces();
        while (columnAttackerAttackSet > 0) 
        {
            unsigned long n;
            _BitScanForward64(&n, columnAttackerAttackSet);
            int k = n / 8;
            int l = n % 8;
            char activeColorPiece = state.getPiece(i, j);
            char inactiveColorPiece = state.getPiece(k, l);
            moves.push_back(Move(i, j, k, l, MoveType::NORMAL, activeColorPiece, inactiveColorPiece));
            columnAttackerAttackSet -= (unsigned long long)1 << n;
        }
        columnAttackers -= (unsigned long long)1 << m;
    }
}
void MoveGenerator::getDiagonalAttackerMoves(vector<Move>& moves, const State& state) 
{
    unsigned long long diagonalAttackers = state.getActiveColorDiagonalAttackers();
    while (diagonalAttackers > 0) 
    {
        unsigned long m;
        _BitScanForward64(&m, diagonalAttackers);
        int i = m / 8;
        int j = m % 8;
        unsigned long long diagonalAttackerAttackSet = getMagicAttackSet(getMaskedBlockers(~state.getEmptySquares(), DIAGONAL, m), DIAGONAL, m) & ~state.getActiveColorPieces();
        while (diagonalAttackerAttackSet > 0) 
        {
            unsigned long n;
            _BitScanForward64(&n, diagonalAttackerAttackSet);
            int k = n / 8;
            int l = n % 8;
            char activeColorPiece = state.getPiece(i, j);
            char inactiveColorPiece = state.getPiece(k, l);
            moves.push_back(Move(i, j, k, l, MoveType::NORMAL, activeColorPiece, inactiveColorPiece));
            diagonalAttackerAttackSet -= (unsigned long long)1 << n;
        }
        diagonalAttackers -= (unsigned long long)1 << m;
    }
}
void MoveGenerator::getEnPassants(vector<Move>& moves, const State& state) {
    if (state.getPossibleEnPassantTargetRow() == -1)
        return;
    int dj[2] = { -1, 1 };
    for (int k = 0; k < 2; k++) {
        if (state.getPossibleEnPassantTargetColumn() + dj[k] < 0 || state.getPossibleEnPassantTargetColumn() + dj[k] >= 8 || !state.isActiveColorPawn(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn() + dj[k]))
            continue;
        moves.push_back(Move(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn() + dj[k], state.getPossibleEnPassantTargetRow() + (state.getActiveColor() ? 1 : -1), state.getPossibleEnPassantTargetColumn(), MoveType::EN_PASSANT, state.getActiveColor() ? 'p' : 'P', state.getActiveColor() ? 'P' : 'p'));
    }
}
unsigned long long MoveGenerator::getInactiveColorColumnAttackerAttackSets(const State& state) 
{
    unsigned long long inactiveColorColumnAttackerAttackSets = 0;
    unsigned long long inactiveColorColumnAttackers = state.getInactiveColorColumnAttackers();
    while (inactiveColorColumnAttackers > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, inactiveColorColumnAttackers);
        inactiveColorColumnAttackerAttackSets |= getMagicAttackSet(getMaskedBlockers(~state.getEmptySquares(), COLUMN, k), COLUMN, k);
        inactiveColorColumnAttackers -= (unsigned long long)1 << k;
    }
    return inactiveColorColumnAttackerAttackSets;
}
unsigned long long MoveGenerator::getInactiveColorDiagonalAttackerAttackSets(const State& state) 
{
    unsigned long long inactiveColorDiagonalAttackerAttackSets = 0;
    unsigned long long inactiveColorDiagonalAttackers = state.getInactiveColorDiagonalAttackers();
    while (inactiveColorDiagonalAttackers > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, inactiveColorDiagonalAttackers);
        inactiveColorDiagonalAttackerAttackSets |= getMagicAttackSet(getMaskedBlockers(~state.getEmptySquares(), DIAGONAL, k), DIAGONAL, k);
        inactiveColorDiagonalAttackers -= (unsigned long long)1 << k;
    }
    return inactiveColorDiagonalAttackerAttackSets;
}
unsigned long long MoveGenerator::getInactiveColorKingAttackSets(const State& state) 
{
    unsigned long long inactiveColorKingAttackSets = 0;
    unsigned long long inactiveColorKing = state.getInactiveColorKing();
    while (inactiveColorKing > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, inactiveColorKing);
        inactiveColorKingAttackSets |= attackSets[KING][k];
        inactiveColorKing -= (unsigned long long)1 << k;
    }
    return inactiveColorKingAttackSets;
}
unsigned long long MoveGenerator::getInactiveColorKnightAttackSets(const State& state) 
{
    unsigned long long inactiveColorKnightAttackSets = 0;
    unsigned long long inactiveColorKnights = state.getInactiveColorKnights();
    while (inactiveColorKnights > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, inactiveColorKnights);
        inactiveColorKnightAttackSets |= attackSets[KNIGHT][k];
        inactiveColorKnights -= (unsigned long long)1 << k;
    }
    return inactiveColorKnightAttackSets;
}
unsigned long long MoveGenerator::getInactiveColorPawnAttackSets(const State& state) 
{
    unsigned long long inactiveColorPawnAttackSets = 0;
    unsigned long long inactiveColorPawns = state.getInactiveColorPawns();
    while (inactiveColorPawns > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, inactiveColorPawns);
        inactiveColorPawnAttackSets |= attackSets[state.getActiveColor() ? WHITE_PAWN : BLACK_PAWN][k];
        inactiveColorPawns -= (unsigned long long)1 << k;
    }
    return inactiveColorPawnAttackSets;
}

unsigned long long MoveGenerator::getKey(unsigned long long maskedBlockers, MagicBitboardType k, int l) {
    return (maskedBlockers * magicNumbers[k][l]) >> (64 - keySizes[k][l]);
}
void MoveGenerator::getKingMoves(vector<Move>& moves, const State& state) {
    unsigned long long king = state.getActiveColorKing();
    while (king > 0) {
        unsigned long m;
        _BitScanForward64(&m, king);
        int i = m / 8;
        int j = m % 8;
        unsigned long long kingAttackSet = attackSets[KING][m] & ~state.getActiveColorPieces();
        while (kingAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, kingAttackSet);
            int k = n / 8;
            int l = n % 8;
            char inactiveColorPiece = state.getPiece(k, l);
            moves.push_back(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'k' : 'K', inactiveColorPiece));
            kingAttackSet -= (unsigned long long)1 << n;
        }
        king -= (unsigned long long)1 << m;
    }
}
void MoveGenerator::getKnightMoves(vector<Move>& moves, const State& state) {
    unsigned long long activeColorKnights = state.getActiveColorKnights();
    while (activeColorKnights > 0) {
        unsigned long m;
        _BitScanForward64(&m, activeColorKnights);
        int i = m / 8;
        int j = m % 8;
        unsigned long long knightAttackSet = attackSets[KNIGHT][m] & ~state.getActiveColorPieces();
        while (knightAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, knightAttackSet);
            int k = n / 8;
            int l = n % 8;
            char inactiveColorPiece = state.getPiece(k, l);
            moves.push_back(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'n' : 'N', inactiveColorPiece));
            knightAttackSet -= (unsigned long long)1 << n;
        }
        activeColorKnights -= (unsigned long long)1 << m;
    }
}
void MoveGenerator::getPawnCaptures(vector<Move>& moves, const State& state) 
{
    unsigned long long activeColorPawns = state.getActiveColorPawns();
    while (activeColorPawns > 0) 
    {
        unsigned long m;
        _BitScanForward64(&m, activeColorPawns);
        int i = m / 8;
        int j = m % 8;
        unsigned long long pawnAttackSet = attackSets[state.getActiveColor() ? BLACK_PAWN : WHITE_PAWN][m] & state.getInactiveColorPieces();
        while (pawnAttackSet > 0) 
        {
            unsigned long n;
            _BitScanForward64(&n, pawnAttackSet);
            int k = n / 8;
            int l = n % 8;
            char inactiveColorPiece = state.getPiece(k, l);
            if (abs(k - 3.5) == 3.5) {
                MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                for (int o = 0; o < 4; o++)
                    moves.push_back(Move(i, j, k, l, moveTypes[o], state.getActiveColor() ? 'p' : 'P', inactiveColorPiece));
            }
            else
                moves.push_back(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', inactiveColorPiece));
            pawnAttackSet -= (unsigned long long)1 << n;
        }
        activeColorPawns -= (unsigned long long)1 << m;
    }
}
void MoveGenerator::getPawnDoublePushes(vector<Move>& moves, const State& state) 
{
    unsigned long long activeColorPawns = state.getActiveColorPawns() & (state.getActiveColor() ? 0x0000000000FF00 & state.getEmptySquares() >> 8 & state.getEmptySquares() >> 16 : 0x00FF000000000000 & state.getEmptySquares() << 8 & state.getEmptySquares() << 16);
    while (activeColorPawns > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, activeColorPawns);
        int i = k / 8;
        int j = k % 8;
        moves.push_back(Move(i, j, i + 2 * (state.getActiveColor() ? 1 : -1), j, MoveType::PAWN_DOUBLE_PUSH, state.getActiveColor() ? 'p' : 'P', '.'));
        activeColorPawns -= (unsigned long long)1 << k;
    }
}
void MoveGenerator::getPawnSinglePushes(vector<Move>& moves, const State& state) 
{
    unsigned long long activeColorPawns = state.getActiveColorPawns() & (state.getActiveColor() ? state.getEmptySquares() >> 8 : state.getEmptySquares() << 8);
    while (activeColorPawns > 0) 
    {
        unsigned long k;
        _BitScanForward64(&k, activeColorPawns);
        int i = k / 8;
        int j = k % 8;
        if (abs((i + (state.getActiveColor() ? 1 : -1)) - 3.5) == 3.5) 
        {
            MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
            for (int l = 0; l < 4; l++)
                moves.push_back(Move(i, j, i + (state.getActiveColor() ? 1 : -1), j, moveTypes[l], state.getActiveColor() ? 'p' : 'P', '.'));
        }
        else
            moves.push_back(Move(i, j, i + (state.getActiveColor() ? 1 : -1), j, MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', '.'));
        activeColorPawns -= (unsigned long long)1 << k;
    }
}
void MoveGenerator::initializeMagicAttackSets() {
    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) 
        {
            vector<unsigned long long> maskedBlockersBitboards;
            unsigned long long maskedBlockers = getMaskedBlockers(0xFFFFFFFFFFFFFFFF, COLUMN, 8 * i + j);
            generateMaskedBlockersBitboards(maskedBlockersBitboards, maskedBlockers, 0);
            for (int keySize = 3; ; keySize = min(keySize + 1, 12)) 
            {
                keySizes[COLUMN][8 * i + j] = keySize;
                magicAttackSets[COLUMN][8 * i + j] = vector<unsigned long long>(1 << keySizes[COLUMN][8 * i + j]);
                magicNumbers[COLUMN][8 * i + j] = rng() & rng();

                if (!isThereCollision(COLUMN, 8 * i + j, maskedBlockersBitboards))
                    break;
            }
        }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) 
        {
            vector<unsigned long long> maskedBlockersBitboards;
            unsigned long long maskedBlockers = getMaskedBlockers(0xFFFFFFFFFFFFFFFF, DIAGONAL, 8 * i + j);
            generateMaskedBlockersBitboards(maskedBlockersBitboards, maskedBlockers, 0);
            for (int keySize = 3; ; keySize = min(keySize + 1, 12)) 
            {
                keySizes[DIAGONAL][8 * i + j] = keySize;
                magicAttackSets[DIAGONAL][8 * i + j] = vector<unsigned long long>(1 << keySizes[DIAGONAL][8 * i + j]);
                magicNumbers[DIAGONAL][8 * i + j] = rng() & rng();
                if (!isThereCollision(DIAGONAL, 8 * i + j, maskedBlockersBitboards))
                    break;
            }
        }
}
bool MoveGenerator::isThereCollision(MagicBitboardType k, int l, const vector<unsigned long long>& maskedBlockersBitboards) 
{
    for (unsigned long long maskedBlockers : maskedBlockersBitboards) 
    {
        unsigned long long key = getKey(maskedBlockers, k, l);
        unsigned long long magicAttackSet = getMagicAttackSet(maskedBlockers, k, l);
        if (magicAttackSets[k][l][key] != 0 && magicAttackSets[k][l][key] != magicAttackSet)
            return true;
        magicAttackSets[k][l][key] = magicAttackSet;
    }
    return false;
}
void MoveGenerator::getMoves(vector<Move>& moves, const State& state) 
{
    initialize();
    getPawnSinglePushes(moves, state);
    getPawnDoublePushes(moves, state);
    getPawnCaptures(moves, state);
    getKnightMoves(moves, state);
    getKingMoves(moves, state);
    getColumnAttackerMoves(moves, state);
    getDiagonalAttackerMoves(moves, state);
    getEnPassants(moves, state);
    getCastles(moves, state);
}
void MoveGenerator::initialize() 
{
    if (isInitialized)
        return;
    generatePawnAttackSets();
    generateKnightAttackSets();
    generateKingAttackSets();
    generateNorthAttackSets();
    generateNortheastAttackSets();
    generateEastAttackSets();
    generateSoutheastAttackSets();
    generateSouthAttackSets();
    generateSouthwestAttackSets();
    generateWestAttackSets();
    generateNorthwestAttackSets();
    initializeMagicAttackSets();
    isInitialized = true;
}
bool MoveGenerator::isActiveColorInCheck(const State& state) 
{
    initialize();
    return getInactiveColorPieceAttackSets(state) & state.getActiveColorKing();
}
