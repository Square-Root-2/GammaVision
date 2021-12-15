#include "MoveGenerator.h"
#include "MoveType.h"

void MoveGenerator::generateBishopAttackSets() {

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
queue<Move> MoveGenerator::getBishopMoves(State& state, int i, int j) {
    queue<Move> bishopMoves;
    int di[4] = { -1, 1, 1, -1 };
    int dj[4] = { 1, 1, -1, -1 };
    for (int k = 0; k < 4; k++)
        for (int l = 1; ; l++) {
            if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                break;
            if (state.isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                break;
            char piece = state.getPiece(i + l * di[k], j + l * dj[k]);
            state.setPiece(i, j, '.');
            state.setPiece(i + l * di[k], j + l * dj[k], state.getActiveColor() ? 'b' : 'B');
            if (!state.isActiveColorInCheck())
                bishopMoves.push(Move(i, j, i + l * di[k], j + l * dj[k], MoveType::NORMAL, state.getActiveColor() ? 'b' : 'B', piece));
            state.setPiece(i + l * di[k], j + l * dj[k], piece);
            state.setPiece(i, j, state.getActiveColor() ? 'b' : 'B');
            if (state.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return bishopMoves;
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
queue<Move> MoveGenerator::getKingMoves(State& state, int i, int j) {
    queue<Move> kingMoves;
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int k = 0; k < 8; k++) {
        if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
            continue;
        if (state.isActiveColorPiece(i + di[k], j + dj[k]))
            continue;
        char piece = state.getPiece(i + di[k], j + dj[k]);
        state.setPiece(i, j, '.');
        state.setPiece(i + di[k], j + dj[k], state.getActiveColor() ? 'k' : 'K');
        if (!state.isActiveColorInCheck())
            kingMoves.push(Move(i, j, i + di[k], j + dj[k], MoveType::KING_MOVE, state.getActiveColor() ? 'k' : 'K', piece));
        state.setPiece(i + di[k], j + dj[k], piece);
        state.setPiece(i, j, state.getActiveColor() ? 'k' : 'K');
    }
    if (state.isActiveColorInCheck())
        return kingMoves;
    if (state.canActiveColorCastleKingside() && !state.isPiece(i, 5) && !state.isPiece(i, 6)) {
        state.setPiece(i, 4, '.');
        state.setPiece(i, 5, state.getActiveColor() ? 'k' : 'K');
        if (!state.isActiveColorInCheck()) {
            state.setPiece(i, 5, '.');
            state.setPiece(i, 6, state.getActiveColor() ? 'k' : 'K');
            if (!state.isActiveColorInCheck())
                kingMoves.push(Move(i, 4, i, 6, MoveType::KINGSIDE_CASTLE, state.getActiveColor() ? 'k' : 'K', '.'));
            state.setPiece(i, 6, '.');
            state.setPiece(i, 5, state.getActiveColor() ? 'k' : 'K');
        }
        state.setPiece(i, 5, '.');
        state.setPiece(i, 4, state.getActiveColor() ? 'k' : 'K');
    }
    if (state.canActiveColorCastleQueenside() && !state.isPiece(i, 1) && !state.isPiece(i, 2) && !state.isPiece(i, 3)) {
        state.setPiece(i, 4, '.');
        state.setPiece(i, 3, state.getActiveColor() ? 'k' : 'K');
        if (!state.isActiveColorInCheck()) {
            state.setPiece(i, 3, '.');
            state.setPiece(i, 2, state.getActiveColor() ? 'k' : 'K');
            if (!state.isActiveColorInCheck())
                kingMoves.push(Move(i, 4, i, 2, MoveType::QUEENSIDE_CASTLE, state.getActiveColor() ? 'k' : 'K', '.'));
            state.setPiece(i, 2, '.');
            state.setPiece(i, 3, state.getActiveColor() ? 'k' : 'K');
        }
        state.setPiece(i, 3, '.');
        state.setPiece(i, 4, state.getActiveColor() ? 'k' : 'K');
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
            char piece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, state.getActiveColor() ? 'n' : 'N');
            if (!state.isActiveColorInCheck())
                knightMoves.push(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'n' : 'N', piece));
            state.setPiece(k, l, piece);
            state.setPiece(i, j, state.getActiveColor() ? 'n' : 'N');
            knightAttackSet -= (unsigned long long)1 << n;
        }
        activeColorKnights -= (unsigned long long)1 << m;
    }
    return knightMoves;
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
            char piece = state.getPiece(k, l);
            state.setPiece(i, j, '.');
            state.setPiece(k, l, state.getActiveColor() ? 'p' : 'P');
            if (!state.isActiveColorInCheck()) {
                if (abs(k - 3.5) == 3.5) {
                    MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                    for (int l = 0; l < 4; l++)
                        pawnCaptures.push(Move(i, j, k, l, moveTypes[l], state.getActiveColor() ? 'p' : 'P', piece));
                }
                else
                    pawnCaptures.push(Move(i, j, k, l, MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', piece));
            }
            state.setPiece(k, l, piece);
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
queue<Move> MoveGenerator::getQueenMoves(State& state, int i, int j) {
    queue<Move> queenMoves;
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int k = 0; k < 8; k++)
        for (int l = 1; ; l++) {
            if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                break;
            if (state.isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                break;
            char piece = state.getPiece(i + l * di[k], j + l * dj[k]);
            state.setPiece(i, j, '.');
            state.setPiece(i + l * di[k], j + l * dj[k], state.getActiveColor() ? 'q' : 'Q');
            if (!state.isActiveColorInCheck())
                queenMoves.push(Move(i, j, i + l * di[k], j + l * dj[k], MoveType::NORMAL, state.getActiveColor() ? 'q' : 'Q', piece));
            state.setPiece(i + l * di[k], j + l * dj[k], piece);
            state.setPiece(i, j, state.getActiveColor() ? 'q' : 'Q');
            if (state.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return queenMoves;
}
queue<Move> MoveGenerator::getRookMoves(State& state, int i, int j) {
    queue<Move> rookMoves;
    int di[4] = { -1, 0, 1, 0 };
    int dj[4] = { 0, 1, 0, -1 };
    for (int k = 0; k < 4; k++)
        for (int l = 1; ; l++) {
            if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                break;
            if (state.isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                break;
            char piece = state.getPiece(i + l * di[k], j + l * dj[k]);
            state.setPiece(i, j, '.');
            state.setPiece(i + l * di[k], j + l * dj[k], state.getActiveColor() ? 'r' : 'R');
            if (!state.isActiveColorInCheck())
                rookMoves.push(Move(i, j, i + l * di[k], j + l * dj[k], MoveType::ROOK_MOVE, state.getActiveColor() ? 'r' : 'R', piece));
            state.setPiece(i + l * di[k], j + l * dj[k], piece);
            state.setPiece(i, j, state.getActiveColor() ? 'r' : 'R');
            if (state.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return rookMoves;
}
MoveGenerator::MoveGenerator() {
    generatePawnAttackSets();
    generateKnightAttackSets();
    generateBishopAttackSets();
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
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (state.isActiveColorBishop(i, j)) {
                queue<Move> bishopMoves = getBishopMoves(state, i, j);
                while (!bishopMoves.empty()) {
                    moves.push_back(bishopMoves.front());
                    bishopMoves.pop();
                }
            }
            else if (state.isActiveColorRook(i, j)) {
                queue<Move> rookMoves = getRookMoves(state, i, j);
                while (!rookMoves.empty()) {
                    moves.push_back(rookMoves.front());
                    rookMoves.pop();
                }
            }
            else if (state.isActiveColorQueen(i, j)) {
                queue<Move> queenMoves = getQueenMoves(state, i, j);
                while (!queenMoves.empty()) {
                    moves.push_back(queenMoves.front());
                    queenMoves.pop();
                }
            }
            else if (state.isActiveColorKing(i, j)) {
                queue<Move> kingMoves = getKingMoves(state, i, j);
                while (!kingMoves.empty()) {
                    moves.push_back(kingMoves.front());
                    kingMoves.pop();
                }
            }
    return moves;
}

