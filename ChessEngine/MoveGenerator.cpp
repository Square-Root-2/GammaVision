#include "MoveGenerator.h"
#include "MoveType.h"

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
queue<Move> MoveGenerator::getKnightMoves(State& state, int i, int j) {
    queue<Move> knightMoves;
    int di[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int dj[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    for (int k = 0; k < 8; k++) {
        if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
            continue;
        if (state.isActiveColorPiece(i + di[k], j + dj[k]))
            continue;
        char piece = state.getPiece(i + di[k], j + dj[k]);
        state.setPiece(i, j, '.');
        state.setPiece(i + di[k], j + dj[k], state.getActiveColor() ? 'n' : 'N');
        if (!state.isActiveColorInCheck())
            knightMoves.push(Move(i, j, i + di[k], j + dj[k], MoveType::NORMAL, state.getActiveColor() ? 'n' : 'N', piece));
        state.setPiece(i + di[k], j + dj[k], piece);
        state.setPiece(i, j, state.getActiveColor() ? 'n' : 'N');
    }
    return knightMoves;
}
queue<Move> MoveGenerator::getPawnMoves(State& state, int i, int j) {
    queue<Move> pawnMoves;
    int di[2] = { -1, 1 };
    if (!state.isPiece(i + di[state.getActiveColor()], j)) {
        state.setPiece(i, j, '.');
        state.setPiece(i + di[state.getActiveColor()], j, state.getActiveColor() ? 'p' : 'P');
        if (!state.isActiveColorInCheck()) {
            if (abs((i + di[state.getActiveColor()]) - 3.5) == 3.5) {
                MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                for (int k = 0; k < 4; k++)
                    pawnMoves.push(Move(i, j, i + di[state.getActiveColor()], j, moveTypes[k], state.getActiveColor() ? 'p' : 'P', '.'));
            }
            else
                pawnMoves.push(Move(i, j, i + di[state.getActiveColor()], j, MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', '.'));
        }
        state.setPiece(i + di[state.getActiveColor()], j, '.');
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
    }
    if ((!state.getActiveColor() && i == 6 || state.getActiveColor() && i == 1) && !state.isPiece(i + di[state.getActiveColor()], j) && !state.isPiece(i + 2 * di[state.getActiveColor()], j)) {
        state.setPiece(i, j, '.');
        state.setPiece(i + 2 * di[state.getActiveColor()], j, state.getActiveColor() ? 'p' : 'P');
        if (!state.isActiveColorInCheck())
            pawnMoves.push(Move(i, j, i + 2 * di[state.getActiveColor()], j, MoveType::PAWN_DOUBLE_PUSH, state.getActiveColor() ? 'p' : 'P', '.'));
        state.setPiece(i + 2 * di[state.getActiveColor()], j, '.');
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
    }
    int dj[2] = { -1, 1 };
    for (int k = 0; k < 2; k++) {
        if (j + dj[k] < 0 || j + dj[k] >= 8)
            continue;
        if (!state.isInactiveColorPiece(i + di[state.getActiveColor()], j + dj[k]) && (i != state.getPossibleEnPassantTargetRow() || j + dj[k] != state.getPossibleEnPassantTargetColumn()))
            continue;
        char piece = state.getPiece(i + di[state.getActiveColor()], j + dj[k]);
        bool isEnPassant = i == state.getPossibleEnPassantTargetRow() && j + dj[k] == state.getPossibleEnPassantTargetColumn();
        state.setPiece(i, j, '.');
        state.setPiece(i + di[state.getActiveColor()], j + dj[k], state.getActiveColor() ? 'p' : 'P');
        if (isEnPassant)
            state.setPiece(i, j + dj[k], '.');
        if (!state.isActiveColorInCheck()) {
            if (abs((i + di[state.getActiveColor()]) - 3.5) == 3.5) {
                MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                for (int l = 0; l < 4; l++)
                    pawnMoves.push(Move(i, j, i + di[state.getActiveColor()], j + dj[k], moveTypes[l], state.getActiveColor() ? 'p' : 'P', piece));
            }
            else
                pawnMoves.push(Move(i, j, i + di[state.getActiveColor()], j + dj[k], isEnPassant ? MoveType::EN_PASSANT : MoveType::NORMAL, state.getActiveColor() ? 'p' : 'P', isEnPassant ? (state.getActiveColor() ? 'P' : 'p') : piece));
        }
        if (isEnPassant)
            state.setPiece(i, j + dj[k], state.getActiveColor() ? 'P' : 'p');
        state.setPiece(i + di[state.getActiveColor()], j + dj[k], piece);
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
    }
    return pawnMoves;
}
queue<Move> MoveGenerator::getPawnSinglePushes(State& state) {
    unsigned long long pawns = state.getActiveColorPawns() & (state.getActiveColor() ? ~state.getOccupiedSquares() >> 8 : ~state.getOccupiedSquares() << 8);
    queue<Move> pawnSinglePushes;
    while (pawns > 0) {
        unsigned long i;
        _BitScanForward(&i, pawns);

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
unsigned long long MoveGenerator::shiftUp(unsigned long long bitboard, int k) {
    return bitboards >> 
}
vector<Move> MoveGenerator::getMoves(State& state) {
    vector<Move> moves;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (state.isActiveColorPawn(i, j)) {
                queue<Move> pawnMoves = getPawnMoves(state, i, j);
                while (!pawnMoves.empty()) {
                    moves.push_back(pawnMoves.front());
                    pawnMoves.pop();
                }
            }
            else if (state.isActiveColorKnight(i, j)) {
                queue<Move> knightMoves = getKnightMoves(state, i, j);
                while (!knightMoves.empty()) {
                    moves.push_back(knightMoves.front());
                    knightMoves.pop();
                }
            }
            else if (state.isActiveColorBishop(i, j)) {
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

