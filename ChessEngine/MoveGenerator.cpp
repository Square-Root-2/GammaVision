#include "MoveGenerator.h"

queue<tuple<int, int, int, int, int>> MoveGenerator::getBishopMoves(State& state, int i, int j) {
    queue<tuple<int, int, int, int, int>> bishopMoves;
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
                bishopMoves.push(tuple<int, int, int, int, int>(i, j, i + l * di[k], j + l * dj[k], -1));
            state.setPiece(i + l * di[k], j + l * dj[k], piece);
            state.setPiece(i, j, state.getActiveColor() ? 'b' : 'B');
            if (state.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return bishopMoves;
}
queue<tuple<int, int, int, int, int>> MoveGenerator::getKingMoves(State& state, int i, int j) {
    queue<tuple<int, int, int, int, int>> kingMoves;
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
            kingMoves.push(tuple<int, int, int, int, int>(i, j, i + di[k], j + dj[k], 7));
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
                kingMoves.push(tuple<int, int, int, int, int>(i, 4, i, 6, 8));
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
                kingMoves.push(tuple<int, int, int, int, int>(i, 4, i, 2, 9));
            state.setPiece(i, 2, '.');
            state.setPiece(i, 3, state.getActiveColor() ? 'k' : 'K');
        }
        state.setPiece(i, 3, '.');
        state.setPiece(i, 4, state.getActiveColor() ? 'k' : 'K');
    }
    return kingMoves;
}
queue<tuple<int, int, int, int, int>> MoveGenerator::getKnightMoves(State& state, int i, int j) {
    queue<tuple<int, int, int, int, int>> knightMoves;
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
            knightMoves.push(tuple<int, int, int, int, int>(i, j, i + di[k], j + dj[k], -1));
        state.setPiece(i + di[k], j + dj[k], piece);
        state.setPiece(i, j, state.getActiveColor() ? 'n' : 'N');
    }
    return knightMoves;
}
queue<tuple<int, int, int, int, int>> MoveGenerator::getPawnMoves(State& state, int i, int j) {
    queue<tuple<int, int, int, int, int>> pawnMoves;
    int di[2] = { -1, 1 };
    if (!state.isPiece(i + di[state.getActiveColor()], j)) {
        state.setPiece(i, j, '.');
        state.setPiece(i + di[state.getActiveColor()], j, state.getActiveColor() ? 'p' : 'P');
        if (!state.isActiveColorInCheck()) {
            if (abs((i + di[state.getActiveColor()]) - 3.5) == 3.5)
                for (int k = 0; k < 4; k++)
                    pawnMoves.push(tuple<int, int, int, int, int>(i, j, i + di[state.getActiveColor()], j, k));
            else
                pawnMoves.push(tuple<int, int, int, int, int>(i, j, i + di[state.getActiveColor()], j, -1));
        }
        state.setPiece(i + di[state.getActiveColor()], j, '.');
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
    }
    if ((!state.getActiveColor() && i == 6 || state.getActiveColor() && i == 1) && !state.isPiece(i + di[state.getActiveColor()], j) && !state.isPiece(i + 2 * di[state.getActiveColor()], j)) {
        state.setPiece(i, j, '.');
        state.setPiece(i + 2 * di[state.getActiveColor()], j, state.getActiveColor() ? 'p' : 'P');
        if (!state.isActiveColorInCheck())
            pawnMoves.push(tuple<int, int, int, int, int>(i, j, i + 2 * di[state.getActiveColor()], j, 4));
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
            if (abs((i + di[state.getActiveColor()]) - 3.5) == 3.5)
                for (int l = 0; l < 4; l++)
                    pawnMoves.push(tuple<int, int, int, int, int>(i, j, i + di[state.getActiveColor()], j + dj[k], l));
            else
                pawnMoves.push(tuple<int, int, int, int, int>(i, j, i + di[state.getActiveColor()], j + dj[k], isEnPassant ? 5 : -1));
        }
        if (isEnPassant)
            state.setPiece(i, j + dj[k], state.getActiveColor() ? 'P' : 'p');
        state.setPiece(i + di[state.getActiveColor()], j + dj[k], piece);
        state.setPiece(i, j, state.getActiveColor() ? 'p' : 'P');
    }
    return pawnMoves;
}
queue<tuple<int, int, int, int, int>> MoveGenerator::getQueenMoves(State& state, int i, int j) {
    queue<tuple<int, int, int, int, int>> queenMoves;
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
                queenMoves.push(tuple<int, int, int, int, int>(i, j, i + l * di[k], j + l * dj[k], -1));
            state.setPiece(i + l * di[k], j + l * dj[k], piece);
            state.setPiece(i, j, state.getActiveColor() ? 'q' : 'Q');
            if (state.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return queenMoves;
}
queue<tuple<int, int, int, int, int>> MoveGenerator::getRookMoves(State& state, int i, int j) {
    queue<tuple<int, int, int, int, int>> rookMoves;
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
                rookMoves.push(tuple<int, int, int, int, int>(i, j, i + l * di[k], j + l * dj[k], 6));
            state.setPiece(i + l * di[k], j + l * dj[k], piece);
            state.setPiece(i, j, state.getActiveColor() ? 'r' : 'R');
            if (state.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return rookMoves;
}
vector<tuple<int, int, int, int, int>> MoveGenerator::getMoves(State& state) {
    vector<tuple<int, int, int, int, int>> moves;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (state.isActiveColorPawn(i, j)) {
                queue<tuple<int, int, int, int, int>> pawnMoves = getPawnMoves(state, i, j);
                while (!pawnMoves.empty()) {
                    moves.push_back(pawnMoves.front());
                    pawnMoves.pop();
                }
            }
            else if (state.isActiveColorKnight(i, j)) {
                queue<tuple<int, int, int, int, int>> knightMoves = getKnightMoves(state, i, j);
                while (!knightMoves.empty()) {
                    moves.push_back(knightMoves.front());
                    knightMoves.pop();
                }
            }
            else if (state.isActiveColorBishop(i, j)) {
                queue<tuple<int, int, int, int, int>> bishopMoves = getBishopMoves(state, i, j);
                while (!bishopMoves.empty()) {
                    moves.push_back(bishopMoves.front());
                    bishopMoves.pop();
                }
            }
            else if (state.isActiveColorRook(i, j)) {
                queue<tuple<int, int, int, int, int>> rookMoves = getRookMoves(state, i, j);
                while (!rookMoves.empty()) {
                    moves.push_back(rookMoves.front());
                    rookMoves.pop();
                }
            }
            else if (state.isActiveColorQueen(i, j)) {
                queue<tuple<int, int, int, int, int>> queenMoves = getQueenMoves(state, i, j);
                while (!queenMoves.empty()) {
                    moves.push_back(queenMoves.front());
                    queenMoves.pop();
                }
            }
            else if (state.isActiveColorKing(i, j)) {
                queue<tuple<int, int, int, int, int>> kingMoves = getKingMoves(state, i, j);
                while (!kingMoves.empty()) {
                    moves.push_back(kingMoves.front());
                    kingMoves.pop();
                }
            }
    return moves;
}
