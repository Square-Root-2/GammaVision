#include "MoveGenerator.h"
#include "MoveType.h"

void MoveGenerator::generateEastAttackSets() {
    for (int i = 0; i < 8; i++) {
        unsigned long long eastAttackSet = 0;
        for (int j = 7; j >= 0; j--) {
            slidingPieceAttackSets[EAST][8 * i + j] = eastAttackSet;
            eastAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::generateKingAttackSets() {
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            kingAttackSets[8 * i + j] = 0;
            for (int k = 0; k < 8; k++) {
                if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                    continue;
                kingAttackSets[8 * i + j] |= (unsigned long long)1 << (8 * (i + di[k]) + (j + dj[k]));
            }
        }
}
void MoveGenerator::generateKnightAttackSets() {
    int di[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int dj[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    for (int k = 0; k < 64; k++) {
        knightAttackSets[k] = 0;
        int i = k / 8;
        int j = k % 8;
        for (int l = 0; l < 8; l++) {
            if (i + di[l] < 0 || i + di[l] >= 8 || j + dj[l] < 0 || j + dj[l] >= 8)
                continue;
            knightAttackSets[k] |= (unsigned long long)1 << (8 * (i + di[l]) + (j + dj[l]));
        }
    }
}
void MoveGenerator::generateNorthAttackSets() {
    for (int j = 0; j < 8; j++) {
        unsigned long long northAttackSet = 0;
        for (int i = 0; i < 8; i++) {
            slidingPieceAttackSets[NORTH][8 * i + j] = northAttackSet;
            northAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::generateNortheastAttackSets() {
    for (int k = 0; k < 8; k++) {
        unsigned long long northeastAttackSet = 0;
        int i = 0;
        int j = k;
        while (i < 8 && j >= 0) {
            slidingPieceAttackSets[NORTHEAST][8 * i + j] = northeastAttackSet;
            northeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j--;
        }
    }
    for (int k = 1; k < 8; k++) {
        unsigned long long northeastAttackSet = 0;
        int i = k;
        int j = 7;
        while (i < 8 && j >= 0) {
            slidingPieceAttackSets[NORTHEAST][8 * i + j] = northeastAttackSet;
            northeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j--;
        }
    }
}
void MoveGenerator::generateNorthwestAttackSets() {
    for (int k = 0; k < 8; k++) {
        unsigned long long northwestAttackSet = 0;
        int i = 0;
        int j = k;
        while (i < 8 && j < 8) {
            slidingPieceAttackSets[NORTHWEST][8 * i + j] = northwestAttackSet;
            northwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j++;
        }
    }
    for (int k = 1; k < 8; k++) {
        unsigned long long northwestAttackSet = 0;
        int i = k;
        int j = 0;
        while (i < 8 && j < 8) {
            slidingPieceAttackSets[NORTHWEST][8 * i + j] = northwestAttackSet;
            northwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i++;
            j++;
        }
    }
}
void MoveGenerator::generatePawnAttackSets() {
    int di[2] = { -1, 1 };
    int dj[2] = { -1, 1 };
    for (int l = WHITE; l <= BLACK; l++)
        for (int m = 0; m < 64; m++) {
            pawnAttackSets[l][m] = 0;
            int i = m / 8;
            int j = m % 8;
            for (int k = 0; k < 2; k++) {
                if (i + di[l] < 0 || i + di[l] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
                    continue;
                pawnAttackSets[l][m] |= (unsigned long long)1 << (8 * (i + di[l]) + (j + dj[k]));
            }
        }
}
void MoveGenerator::generateSlidingPieceAttackSets() {
    generateNorthAttackSets();
    generateNortheastAttackSets();
    generateEastAttackSets();
    generateSoutheastAttackSets();
    generateSouthAttackSets();
    generateSouthwestAttackSets();
    generateWestAttackSets();
    generateNorthwestAttackSets();
}
void MoveGenerator::generateSouthAttackSets() {
    for (int j = 0; j < 8; j++) {
        unsigned long long southAttackSet = 0;
        for (int i = 7; i >= 0; i--) {
            slidingPieceAttackSets[SOUTH][8 * i + j] = southAttackSet;
            southAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
void MoveGenerator::generateSoutheastAttackSets() {
    for (int k = 0; k < 8; k++) {
        unsigned long long southeastAttackSet = 0;
        int i = 7;
        int j = k;
        while (i >= 0 && j >= 0) {
            slidingPieceAttackSets[SOUTHEAST][8 * i + j] = southeastAttackSet;
            southeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j--;
        }
    }
    for (int k = 0; k < 7; k++) {
        unsigned long long southeastAttackSet = 0;
        int i = k;
        int j = 7;
        while (i >= 0 && j >= 0) {
            slidingPieceAttackSets[SOUTHEAST][8 * i + j] = southeastAttackSet;
            southeastAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j--;
        }
    }
}
void MoveGenerator::generateSouthwestAttackSets() {
    for (int k = 0; k < 8; k++) {
        unsigned long long southwestAttackSet = 0;
        int i = 7;
        int j = k;
        while (i >= 0 && j < 8) {
            slidingPieceAttackSets[SOUTHWEST][8 * i + j] = southwestAttackSet;
            southwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j++;
        }
    }
    for (int k = 0; k < 7; k++) {
        unsigned long long southwestAttackSet = 0;
        int i = k;
        int j = 0;
        while (i >= 0 && j < 8) {
            slidingPieceAttackSets[SOUTHWEST][8 * i + j] = southwestAttackSet;
            southwestAttackSet |= (unsigned long long)1 << (8 * i + j);
            i--;
            j++;
        }
    }
}
void MoveGenerator::generateWestAttackSets() {
    for (int i = 0; i < 8; i++) {
        unsigned long long westAttackSet = 0;
        for (int j = 0; j < 8; j++) {
            slidingPieceAttackSets[WEST][8 * i + j] = westAttackSet;
            westAttackSet |= (unsigned long long)1 << (8 * i + j);
        }
    }
}
queue<Move> MoveGenerator::getCastlings(State& state) {
    queue<Move> castlings;
    if (state.isActiveColorInCheck())
        return castlings;
    if (state.canActiveColorCastleKingside() && !state.isPiece(state.getActiveColor() ? 0 : 7, 5) && !state.isPiece(state.getActiveColor() ? 0 : 7, 6)) {
        state.setPiece(state.getActiveColor() ? 0 : 7, 4, '.');
        state.setPiece(state.getActiveColor() ? 0 : 7, 5, state.getActiveColor() ? 'k' : 'K');
        if (!state.isActiveColorInCheck()) {
            state.setPiece(state.getActiveColor() ? 0 : 7, 5, '.');
            state.setPiece(state.getActiveColor() ? 0 : 7, 6, state.getActiveColor() ? 'k' : 'K');
            if (!state.isActiveColorInCheck())
                castlings.push(Move(state.getActiveColor() ? 0 : 7, 4, state.getActiveColor() ? 0 : 7, 6, MoveType::KINGSIDE_CASTLE, state.getActiveColor() ? 'k' : 'K', '.'));
            state.setPiece(state.getActiveColor() ? 0 : 7, 6, '.');
            state.setPiece(state.getActiveColor() ? 0 : 7, 5, state.getActiveColor() ? 'k' : 'K');
        }
        state.setPiece(state.getActiveColor() ? 0 : 7, 5, '.');
        state.setPiece(state.getActiveColor() ? 0 : 7, 4, state.getActiveColor() ? 'k' : 'K');
    }
    if (state.canActiveColorCastleQueenside() && !state.isPiece(state.getActiveColor() ? 0 : 7, 1) && !state.isPiece(state.getActiveColor() ? 0 : 7, 2) && !state.isPiece(state.getActiveColor() ? 0 : 7, 3)) {
        state.setPiece(state.getActiveColor() ? 0 : 7, 4, '.');
        state.setPiece(state.getActiveColor() ? 0 : 7, 3, state.getActiveColor() ? 'k' : 'K');
        if (!state.isActiveColorInCheck()) {
            state.setPiece(state.getActiveColor() ? 0 : 7, 3, '.');
            state.setPiece(state.getActiveColor() ? 0 : 7, 2, state.getActiveColor() ? 'k' : 'K');
            if (!state.isActiveColorInCheck())
                castlings.push(Move(state.getActiveColor() ? 0 : 7, 4, state.getActiveColor() ? 0 : 7, 2, MoveType::QUEENSIDE_CASTLE, state.getActiveColor() ? 'k' : 'K', '.'));
            state.setPiece(state.getActiveColor() ? 0 : 7, 2, '.');
            state.setPiece(state.getActiveColor() ? 0 : 7, 3, state.getActiveColor() ? 'k' : 'K');
        }
        state.setPiece(state.getActiveColor() ? 0 : 7, 3, '.');
        state.setPiece(state.getActiveColor() ? 0 : 7, 4, state.getActiveColor() ? 'k' : 'K');
    }
    return castlings;
}
queue<Move> MoveGenerator::getColumnAttackerMoves(State& state) {
    queue<Move> columnAttackerMoves;
    unsigned long long columnAttackers = state.getActiveColorColumnAttackers();
    while (columnAttackers > 0) {
        unsigned long m;
        _BitScanForward64(&m, columnAttackers);
        int i = m / 8;
        int j = m % 8;
        unsigned long long columnAttackerAttackSet = getNorthAttackSet(state, i, j) | getEastAttackSet(state, i, j) | getSouthAttackSet(state, i, j) | getWestAttackSet(state, i, j);
        while (columnAttackerAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, columnAttackerAttackSet);
            int k = n / 8;
            int l = n % 8;
            if (state.isActiveColorPiece(k, l)) {
                columnAttackerAttackSet -= (unsigned long long)1 << n;
                continue;
            }
            char activeColorPiece = state.getPiece(i, j);
            char inactiveColorPiece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, activeColorPiece);
            if (!state.isActiveColorInCheck())
                columnAttackerMoves.push(Move(i, j, k, l, MoveType::NORMAL, activeColorPiece, inactiveColorPiece));
            state.setPiece(k, l, inactiveColorPiece);
            state.setPiece(i, j, activeColorPiece);
            columnAttackerAttackSet -= (unsigned long long)1 << n;
        }
        columnAttackers -= (unsigned long long)1 << m;
    }
    return columnAttackerMoves;
}
queue<Move> MoveGenerator::getDiagonalAttackerMoves(State& state) {
    queue<Move> diagonalAttackerMoves;
    unsigned long long diagonalAttackers = state.getActiveColorDiagonalAttackers();
    while (diagonalAttackers > 0) {
        unsigned long m;
        _BitScanForward64(&m, diagonalAttackers);
        int i = m / 8;
        int j = m % 8;
        unsigned long long diagonalAttackerAttackSet = getNortheastAttackSet(state, i, j) | getSoutheastAttackSet(state, i, j) | getSouthwestAttackSet(state, i, j) | getNorthwestAttackSet(state, i, j);
        while (diagonalAttackerAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, diagonalAttackerAttackSet);
            int k = n / 8;
            int l = n % 8;
            if (state.isActiveColorPiece(k, l)) {
                diagonalAttackerAttackSet -= (unsigned long long)1 << n;
                continue;
            }
            char activeColorPiece = state.getPiece(i, j);
            char inactiveColorPiece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, activeColorPiece);
            if (!state.isActiveColorInCheck())
                diagonalAttackerMoves.push(Move(i, j, k, l, MoveType::NORMAL, activeColorPiece, inactiveColorPiece));
            state.setPiece(k, l, inactiveColorPiece);
            state.setPiece(i, j, activeColorPiece);
            diagonalAttackerAttackSet -= (unsigned long long)1 << n;
        }
        diagonalAttackers -= (unsigned long long)1 << m;
    }
    return diagonalAttackerMoves;
}
unsigned long long MoveGenerator::getEastAttackSet(State& state, int i, int j) {
    unsigned long long eastAttackSet = slidingPieceAttackSets[EAST][8 * i + j];
    unsigned long k;
    if (_BitScanForward64(&k, slidingPieceAttackSets[EAST][8 * i + j] & ~state.getEmptySquares()) == 0)
        return eastAttackSet;
    eastAttackSet &= ~slidingPieceAttackSets[EAST][k];
    return eastAttackSet;
}
queue<Move> MoveGenerator::getEnPassants(State& state) {
    queue<Move> enPassants;
    if (state.getPossibleEnPassantTargetRow() == -1)
        return enPassants;
    int dj[2] = { -1, 1 };
    for (int k = 0; k < 2; k++) {
        if (state.getPossibleEnPassantTargetColumn() + dj[k] < 0 || state.getPossibleEnPassantTargetColumn() + dj[k] >= 8 || !state.isActiveColorPawn(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn() + dj[k]))
            continue;
        state.setPiece(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn() + dj[k], '.');
        state.setPiece(state.getPossibleEnPassantTargetRow() + (state.getActiveColor() ? 1 : -1), state.getPossibleEnPassantTargetColumn(), state.getActiveColor() ? 'p' : 'P');
        state.setPiece(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn(), '.');
        if (!state.isActiveColorInCheck())
             enPassants.push(Move(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn() + dj[k], state.getPossibleEnPassantTargetRow() + (state.getActiveColor() ? 1 : -1), state.getPossibleEnPassantTargetColumn(), MoveType::EN_PASSANT, state.getActiveColor() ? 'p' : 'P', state.getActiveColor() ? 'P' : 'p'));
        state.setPiece(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn(), state.getActiveColor() ? 'P' : 'p');
        state.setPiece(state.getPossibleEnPassantTargetRow() + (state.getActiveColor() ? 1 : -1), state.getPossibleEnPassantTargetColumn(), '.');
        state.setPiece(state.getPossibleEnPassantTargetRow(), state.getPossibleEnPassantTargetColumn() + dj[k], state.getActiveColor() ? 'p' : 'P');
    }
    return enPassants;
}
queue<Move> MoveGenerator::getKingMoves(State& state) {
    queue<Move> kingMoves;
    unsigned long long kings = state.getActiveColorKings();
    while (kings > 0) {
        unsigned long m;
        _BitScanForward64(&m, kings);
        int i = m / 8;
        int j = m % 8;
        unsigned long long kingAttackSet = kingAttackSets[m] & ~state.getActiveColorPieces();
        while (kingAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, kingAttackSet);
            int k = n / 8;
            int l = n % 8;
            char inactiveColorPiece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, state.getActiveColor() ? 'k' : 'K');
            if (!state.isActiveColorInCheck())
                kingMoves.push(Move(i, j, k, l, MoveType::KING_MOVE, state.getActiveColor() ? 'k' : 'K', inactiveColorPiece));
            state.setPiece(k, l, inactiveColorPiece);
            state.setPiece(i, j, state.getActiveColor() ? 'k' : 'K');
            kingAttackSet -= (unsigned long long)1 << n;
        }
        kings -= (unsigned long long)1 << m;
    }
    return kingMoves;
}
queue<Move> MoveGenerator::getKnightMoves(State& state) {
    queue<Move> knightMoves;
    unsigned long long activeColorKnights = state.getActiveColorKnights();
    while (activeColorKnights > 0) {
        unsigned long m;
        _BitScanForward64(&m, activeColorKnights);
        int i = m / 8;
        int j = m % 8;
        unsigned long long knightAttackSet = knightAttackSets[m] & ~state.getActiveColorPieces();
        while (knightAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, knightAttackSet);
            int k = n / 8;
            int l = n % 8;
            char inactiveColorPiece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, state.getActiveColor() ? 'n' : 'N');
            if (!state.isActiveColorInCheck())
                knightMoves.push(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'n' : 'N', inactiveColorPiece));
            state.setPiece(k, l, inactiveColorPiece);
            state.setPiece(i, j, state.getActiveColor() ? 'n' : 'N');
            knightAttackSet -= (unsigned long long)1 << n;
        }
        activeColorKnights -= (unsigned long long)1 << m;
    }
    return knightMoves;
}
unsigned long long MoveGenerator::getNorthAttackSet(State& state, int i, int j) {
    unsigned long long northAttackSet = slidingPieceAttackSets[NORTH][8 * i + j];
    unsigned long k;
    if (_BitScanReverse64(&k, slidingPieceAttackSets[NORTH][8 * i + j] & ~state.getEmptySquares()) == 0)
        return northAttackSet;
    northAttackSet &= ~slidingPieceAttackSets[NORTH][k];
    return northAttackSet;
}
unsigned long long MoveGenerator::getNortheastAttackSet(State& state, int i, int j) {
    unsigned long long northeastAttackSet = slidingPieceAttackSets[NORTHEAST][8 * i + j];
    unsigned long k;
    if (_BitScanReverse64(&k, slidingPieceAttackSets[NORTHEAST][8 * i + j] & ~state.getEmptySquares()) == 0)
        return northeastAttackSet;
    northeastAttackSet &= ~slidingPieceAttackSets[NORTHEAST][k];
    return northeastAttackSet;
}
unsigned long long MoveGenerator::getNorthwestAttackSet(State& state, int i, int j) {
    unsigned long long northwestAttackSet = slidingPieceAttackSets[NORTHWEST][8 * i + j];
    unsigned long k;
    if (_BitScanReverse64(&k, slidingPieceAttackSets[NORTHWEST][8 * i + j] & ~state.getEmptySquares()) == 0)
        return northwestAttackSet;
    northwestAttackSet &= ~slidingPieceAttackSets[NORTHWEST][k];
    return northwestAttackSet;
}
queue<Move> MoveGenerator::getPawnCaptures(State& state) {
    queue<Move> pawnCaptures;
    unsigned long long activeColorPawns = state.getActiveColorPawns();
    while (activeColorPawns > 0) {
        unsigned long m;
        _BitScanForward64(&m, activeColorPawns);
        int i = m / 8;
        int j = m % 8;
        unsigned long long pawnAttackSet = pawnAttackSets[state.getActiveColor()][m] & state.getInactiveColorPieces();
        while (pawnAttackSet > 0) {
            unsigned long n;
            _BitScanForward64(&n, pawnAttackSet);
            int k = n / 8;
            int l = n % 8;
            char inactiveColorPiece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, state.getActiveColor() ? 'p' : 'P');
            if (!state.isActiveColorInCheck()) {
                if (abs(k - 3.5) == 3.5) {
                    MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                    for (int l = 0; l < 4; l++)
                        pawnCaptures.push(Move(i, j, k, l, moveTypes[l], state.getActiveColor() ? 'p' : 'P', inactiveColorPiece));
                }
                else
                    pawnCaptures.push(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', inactiveColorPiece));
            }
            state.setPiece(k, l, inactiveColorPiece);
            state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
            pawnAttackSet -= (unsigned long long)1 << n;
        }
        activeColorPawns -= (unsigned long long)1 << m;
    }
    return pawnCaptures;
}
queue<Move> MoveGenerator::getPawnDoublePushes(State& state) {
    queue<Move> pawnDoublePushes;
    unsigned long long activeColorPawns = state.getActiveColorPawns() & (state.getActiveColor() ? 0x0000000000FF00 & state.getEmptySquares() >> 8 & state.getEmptySquares() >> 16 : 0x00FF000000000000 & state.getEmptySquares() << 8 & state.getEmptySquares() << 16);
    while (activeColorPawns > 0) {
        unsigned long k;
        _BitScanForward64(&k, activeColorPawns);
        int i = k / 8;
        int j = k % 8;
        state.setPiece(i, j, '.');
        state.setPiece(i + 2 * (state.getActiveColor() ? 1 : -1), j, state.getActiveColor() ? 'p' : 'P');
        if (!state.isActiveColorInCheck())
            pawnDoublePushes.push(Move(i, j, i + 2 * (state.getActiveColor() ? 1 : -1), j, MoveType::PAWN_DOUBLE_PUSH, state.getActiveColor() ? 'p' : 'P', '.'));
        state.setPiece(i + 2 * (state.getActiveColor() ? 1 : -1), j, '.');
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
        activeColorPawns -= (unsigned long long)1 << k;
    }
    return pawnDoublePushes;
}
queue<Move> MoveGenerator::getPawnSinglePushes(State& state) {
    queue<Move> pawnSinglePushes;
    unsigned long long activeColorPawns = state.getActiveColorPawns() & (state.getActiveColor() ? state.getEmptySquares() >> 8 : state.getEmptySquares() << 8);
    while (activeColorPawns > 0) {
        unsigned long k;
        _BitScanForward64(&k, activeColorPawns);
        int i = k / 8;
        int j = k % 8;
        state.setPiece(i, j, '.');
        state.setPiece(i + (state.getActiveColor() ? 1 : -1), j, state.getActiveColor() ? 'p' : 'P');
        if (!state.isActiveColorInCheck()) {
            if (abs((i + (state.getActiveColor() ? 1 : -1)) - 3.5) == 3.5) {
                MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                for (int k = 0; k < 4; k++)
                    pawnSinglePushes.push(Move(i, j, i + (state.getActiveColor() ? 1 : -1), j, moveTypes[k], state.getActiveColor() ? 'p' : 'P', '.'));
            }
            else
                pawnSinglePushes.push(Move(i, j, i + (state.getActiveColor() ? 1 : -1), j, MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', '.'));
        }
        state.setPiece(i + (state.getActiveColor() ? 1 : -1), j, '.');
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
        activeColorPawns -= (unsigned long long)1 << k;
    }
    return pawnSinglePushes;
}
unsigned long long MoveGenerator::getSouthAttackSet(State& state, int i, int j) {
    unsigned long long southAttackSet = slidingPieceAttackSets[SOUTH][8 * i + j];
    unsigned long k;
    if (_BitScanForward64(&k, slidingPieceAttackSets[SOUTH][8 * i + j] & ~state.getEmptySquares()) == 0)
        return southAttackSet;
    southAttackSet &= ~slidingPieceAttackSets[SOUTH][k];
    return southAttackSet;
}
unsigned long long MoveGenerator::getSoutheastAttackSet(State& state, int i, int j) {
    unsigned long long southeastAttackSet = slidingPieceAttackSets[SOUTHEAST][8 * i + j];
    unsigned long k;
    if (_BitScanForward64(&k, slidingPieceAttackSets[SOUTHEAST][8 * i + j] & ~state.getEmptySquares()) == 0)
        return southeastAttackSet;
    southeastAttackSet &= ~slidingPieceAttackSets[SOUTHEAST][k];
    return southeastAttackSet;
}
unsigned long long MoveGenerator::getSouthwestAttackSet(State& state, int i, int j) {
    unsigned long long southwestAttackSet = slidingPieceAttackSets[SOUTHWEST][8 * i + j];
    unsigned long k;
    if (_BitScanForward64(&k, slidingPieceAttackSets[SOUTHWEST][8 * i + j] & ~state.getEmptySquares()) == 0)
        return southwestAttackSet;
    southwestAttackSet &= ~slidingPieceAttackSets[SOUTHWEST][k];
    return southwestAttackSet;
}
unsigned long long MoveGenerator::getWestAttackSet(State& state, int i, int j) {
    unsigned long long westAttackSet = slidingPieceAttackSets[WEST][8 * i + j];
    unsigned long k;
    if (_BitScanReverse64(&k, slidingPieceAttackSets[WEST][8 * i + j] & ~state.getEmptySquares()) == 0)
        return westAttackSet;
    westAttackSet &= ~slidingPieceAttackSets[WEST][k];
    return westAttackSet;
}
MoveGenerator::MoveGenerator() {
    generatePawnAttackSets();
    generateKnightAttackSets();
    generateSlidingPieceAttackSets();
    generateKingAttackSets();
}
vector<Move> MoveGenerator::getMoves(State& state) {
    vector<Move> moves;
    queue<Move> pawnSinglePushes = getPawnSinglePushes(state);
    while (!pawnSinglePushes.empty()) {
        moves.push_back(pawnSinglePushes.front());
        pawnSinglePushes.pop();
    }
    queue<Move> pawnDoublePushes = getPawnDoublePushes(state);
    while (!pawnDoublePushes.empty()) {
        moves.push_back(pawnDoublePushes.front());
        pawnDoublePushes.pop();
    }
    queue<Move> pawnCaptures = getPawnCaptures(state);
    while (!pawnCaptures.empty()) {
        moves.push_back(pawnCaptures.front());
        pawnCaptures.pop();
    }
    queue<Move> enPassants = getEnPassants(state);
    while (!enPassants.empty()) {
        moves.push_back(enPassants.front());
        enPassants.pop();
    }
    queue<Move> knightsMoves = getKnightMoves(state);
    while (!knightsMoves.empty()) {
        moves.push_back(knightsMoves.front());
        knightsMoves.pop();
    }
    queue<Move> diagonalAttackerMoves = getDiagonalAttackerMoves(state);
    while (!diagonalAttackerMoves.empty()) {
        moves.push_back(diagonalAttackerMoves.front());
        diagonalAttackerMoves.pop();
    }
    queue<Move> columnAttackerMoves = getColumnAttackerMoves(state);
    while (!columnAttackerMoves.empty()) {
        moves.push_back(columnAttackerMoves.front());
        columnAttackerMoves.pop();
    }
    queue<Move> kingMoves = getKingMoves(state);
    while (!kingMoves.empty()) {
        moves.push_back(kingMoves.front());
        kingMoves.pop();
    }
    queue<Move> castlings = getCastlings(state);
    while (!castlings.empty()) {
        moves.push_back(castlings.front());
        castlings.pop();
    }
    return moves;
}

