#include "MoveGenerator.h"
#include "MoveType.h"

queue<Move> MoveGenerator::getBishopMoves(ChessBoard& chessBoard, int i, int j) {
    queue<Move> bishopMoves;
    int di[4] = { -1, 1, 1, -1 };
    int dj[4] = { 1, 1, -1, -1 };
    for (int k = 0; k < 4; k++)
        for (int l = 1; ; l++) {
            if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                break;
            if (chessBoard.isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                break;
            char piece = chessBoard.getPiece(i + l * di[k], j + l * dj[k]);
            chessBoard.setPiece(i, j, '.');
            chessBoard.setPiece(i + l * di[k], j + l * dj[k], chessBoard.getActiveColor() ? 'b' : 'B');
            if (!chessBoard.isActiveColorInCheck())
                bishopMoves.push(Move(i, j, i + l * di[k], j + l * dj[k], MoveType::NORMAL, chessBoard.getActiveColor() ? 'b' : 'B', piece));
            chessBoard.setPiece(i + l * di[k], j + l * dj[k], piece);
            chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'b' : 'B');
            if (chessBoard.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return bishopMoves;
}
queue<Move> MoveGenerator::getKingMoves(ChessBoard& chessBoard, int i, int j) {
    queue<Move> kingMoves;
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int k = 0; k < 8; k++) {
        if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
            continue;
        if (chessBoard.isActiveColorPiece(i + di[k], j + dj[k]))
            continue;
        char piece = chessBoard.getPiece(i + di[k], j + dj[k]);
        chessBoard.setPiece(i, j, '.');
        chessBoard.setPiece(i + di[k], j + dj[k], chessBoard.getActiveColor() ? 'k' : 'K');
        if (!chessBoard.isActiveColorInCheck())
            kingMoves.push(Move(i, j, i + di[k], j + dj[k], MoveType::KING_MOVE, chessBoard.getActiveColor() ? 'k' : 'K', piece));
        chessBoard.setPiece(i + di[k], j + dj[k], piece);
        chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'k' : 'K');
    }
    if (chessBoard.isActiveColorInCheck())
        return kingMoves;
    if (chessBoard.canActiveColorCastleKingside() && !chessBoard.isPiece(i, 5) && !chessBoard.isPiece(i, 6)) {
        chessBoard.setPiece(i, 4, '.');
        chessBoard.setPiece(i, 5, chessBoard.getActiveColor() ? 'k' : 'K');
        if (!chessBoard.isActiveColorInCheck()) {
            chessBoard.setPiece(i, 5, '.');
            chessBoard.setPiece(i, 6, chessBoard.getActiveColor() ? 'k' : 'K');
            if (!chessBoard.isActiveColorInCheck())
                kingMoves.push(Move(i, 4, i, 6, MoveType::KINGSIDE_CASTLE, chessBoard.getActiveColor() ? 'k' : 'K', '.'));
            chessBoard.setPiece(i, 6, '.');
            chessBoard.setPiece(i, 5, chessBoard.getActiveColor() ? 'k' : 'K');
        }
        chessBoard.setPiece(i, 5, '.');
        chessBoard.setPiece(i, 4, chessBoard.getActiveColor() ? 'k' : 'K');
    }
    if (chessBoard.canActiveColorCastleQueenside() && !chessBoard.isPiece(i, 1) && !chessBoard.isPiece(i, 2) && !chessBoard.isPiece(i, 3)) {
        chessBoard.setPiece(i, 4, '.');
        chessBoard.setPiece(i, 3, chessBoard.getActiveColor() ? 'k' : 'K');
        if (!chessBoard.isActiveColorInCheck()) {
            chessBoard.setPiece(i, 3, '.');
            chessBoard.setPiece(i, 2, chessBoard.getActiveColor() ? 'k' : 'K');
            if (!chessBoard.isActiveColorInCheck())
                kingMoves.push(Move(i, 4, i, 2, MoveType::QUEENSIDE_CASTLE, chessBoard.getActiveColor() ? 'k' : 'K', '.'));
            chessBoard.setPiece(i, 2, '.');
            chessBoard.setPiece(i, 3, chessBoard.getActiveColor() ? 'k' : 'K');
        }
        chessBoard.setPiece(i, 3, '.');
        chessBoard.setPiece(i, 4, chessBoard.getActiveColor() ? 'k' : 'K');
    }
    return kingMoves;
}
queue<Move> MoveGenerator::getKnightMoves(ChessBoard& chessBoard, int i, int j) {
    queue<Move> knightMoves;
    int di[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    int dj[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    for (int k = 0; k < 8; k++) {
        if (i + di[k] < 0 || i + di[k] >= 8 || j + dj[k] < 0 || j + dj[k] >= 8)
            continue;
        if (chessBoard.isActiveColorPiece(i + di[k], j + dj[k]))
            continue;
        char piece = chessBoard.getPiece(i + di[k], j + dj[k]);
        chessBoard.setPiece(i, j, '.');
        chessBoard.setPiece(i + di[k], j + dj[k], chessBoard.getActiveColor() ? 'n' : 'N');
        if (!chessBoard.isActiveColorInCheck())
            knightMoves.push(Move(i, j, i + di[k], j + dj[k], MoveType::NORMAL, chessBoard.getActiveColor() ? 'n' : 'N', piece));
        chessBoard.setPiece(i + di[k], j + dj[k], piece);
        chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'n' : 'N');
    }
    return knightMoves;
}
queue<Move> MoveGenerator::getPawnMoves(ChessBoard& chessBoard, int i, int j) {
    queue<Move> pawnMoves;
    int di[2] = { -1, 1 };
    if (!chessBoard.isPiece(i + di[chessBoard.getActiveColor()], j)) {
        chessBoard.setPiece(i, j, '.');
        chessBoard.setPiece(i + di[chessBoard.getActiveColor()], j, chessBoard.getActiveColor() ? 'p' : 'P');
        if (!chessBoard.isActiveColorInCheck()) {
            if (abs((i + di[chessBoard.getActiveColor()]) - 3.5) == 3.5) {
                MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                for (int k = 0; k < 4; k++)
                    pawnMoves.push(Move(i, j, i + di[chessBoard.getActiveColor()], j, moveTypes[k], chessBoard.getActiveColor() ? 'p' : 'P', '.'));
            }
            else
                pawnMoves.push(Move(i, j, i + di[chessBoard.getActiveColor()], j, MoveType::NORMAL, chessBoard.getActiveColor() ? 'p' : 'P', '.'));
        }
        chessBoard.setPiece(i + di[chessBoard.getActiveColor()], j, '.');
        chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'p' : 'P');
    }
    if ((!chessBoard.getActiveColor() && i == 6 || chessBoard.getActiveColor() && i == 1) && !chessBoard.isPiece(i + di[chessBoard.getActiveColor()], j) && !chessBoard.isPiece(i + 2 * di[chessBoard.getActiveColor()], j)) {
        chessBoard.setPiece(i, j, '.');
        chessBoard.setPiece(i + 2 * di[chessBoard.getActiveColor()], j, chessBoard.getActiveColor() ? 'p' : 'P');
        if (!chessBoard.isActiveColorInCheck())
            pawnMoves.push(Move(i, j, i + 2 * di[chessBoard.getActiveColor()], j, MoveType::PAWN_FORWARD_TWO, chessBoard.getActiveColor() ? 'p' : 'P', '.'));
        chessBoard.setPiece(i + 2 * di[chessBoard.getActiveColor()], j, '.');
        chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'p' : 'P');
    }
    int dj[2] = { -1, 1 };
    for (int k = 0; k < 2; k++) {
        if (j + dj[k] < 0 || j + dj[k] >= 8)
            continue;
        if (!chessBoard.isInactiveColorPiece(i + di[chessBoard.getActiveColor()], j + dj[k]) && (i != chessBoard.getPossibleEnPassantTargetRow() || j + dj[k] != chessBoard.getPossibleEnPassantTargetColumn()))
            continue;
        char piece = chessBoard.getPiece(i + di[chessBoard.getActiveColor()], j + dj[k]);
        bool isEnPassant = i == chessBoard.getPossibleEnPassantTargetRow() && j + dj[k] == chessBoard.getPossibleEnPassantTargetColumn();
        chessBoard.setPiece(i, j, '.');
        chessBoard.setPiece(i + di[chessBoard.getActiveColor()], j + dj[k], chessBoard.getActiveColor() ? 'p' : 'P');
        if (isEnPassant)
            chessBoard.setPiece(i, j + dj[k], '.');
        if (!chessBoard.isActiveColorInCheck()) {
            if (abs((i + di[chessBoard.getActiveColor()]) - 3.5) == 3.5) {
                MoveType moveTypes[4] = { MoveType::PROMOTION_TO_BISHOP, MoveType::PROMOTION_TO_KNIGHT, MoveType::PROMOTION_TO_QUEEN, MoveType::PROMOTION_TO_ROOK };
                for (int l = 0; l < 4; l++)
                    pawnMoves.push(Move(i, j, i + di[chessBoard.getActiveColor()], j + dj[k], moveTypes[l], chessBoard.getActiveColor() ? 'p' : 'P', piece));
            }
            else
                pawnMoves.push(Move(i, j, i + di[chessBoard.getActiveColor()], j + dj[k], isEnPassant ? MoveType::EN_PASSANT : MoveType::NORMAL, chessBoard.getActiveColor() ? 'p' : 'P', isEnPassant ? (chessBoard.getActiveColor() ? 'P' : 'p') : piece));
        }
        if (isEnPassant)
            chessBoard.setPiece(i, j + dj[k], chessBoard.getActiveColor() ? 'P' : 'p');
        chessBoard.setPiece(i + di[chessBoard.getActiveColor()], j + dj[k], piece);
        chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'p' : 'P');
    }
    return pawnMoves;
}
queue<Move> MoveGenerator::getQueenMoves(ChessBoard& chessBoard, int i, int j) {
    queue<Move> queenMoves;
    int di[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    int dj[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    for (int k = 0; k < 8; k++)
        for (int l = 1; ; l++) {
            if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                break;
            if (chessBoard.isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                break;
            char piece = chessBoard.getPiece(i + l * di[k], j + l * dj[k]);
            chessBoard.setPiece(i, j, '.');
            chessBoard.setPiece(i + l * di[k], j + l * dj[k], chessBoard.getActiveColor() ? 'q' : 'Q');
            if (!chessBoard.isActiveColorInCheck())
                queenMoves.push(Move(i, j, i + l * di[k], j + l * dj[k], MoveType::NORMAL, chessBoard.getActiveColor() ? 'q' : 'Q', piece));
            chessBoard.setPiece(i + l * di[k], j + l * dj[k], piece);
            chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'q' : 'Q');
            if (chessBoard.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return queenMoves;
}
queue<Move> MoveGenerator::getRookMoves(ChessBoard& chessBoard, int i, int j) {
    queue<Move> rookMoves;
    int di[4] = { -1, 0, 1, 0 };
    int dj[4] = { 0, 1, 0, -1 };
    for (int k = 0; k < 4; k++)
        for (int l = 1; ; l++) {
            if (i + l * di[k] < 0 || i + l * di[k] >= 8 || j + l * dj[k] < 0 || j + l * dj[k] >= 8)
                break;
            if (chessBoard.isActiveColorPiece(i + l * di[k], j + l * dj[k]))
                break;
            char piece = chessBoard.getPiece(i + l * di[k], j + l * dj[k]);
            chessBoard.setPiece(i, j, '.');
            chessBoard.setPiece(i + l * di[k], j + l * dj[k], chessBoard.getActiveColor() ? 'r' : 'R');
            if (!chessBoard.isActiveColorInCheck())
                rookMoves.push(Move(i, j, i + l * di[k], j + l * dj[k], MoveType::ROOK_MOVE, chessBoard.getActiveColor() ? 'r' : 'R', piece));
            chessBoard.setPiece(i + l * di[k], j + l * dj[k], piece);
            chessBoard.setPiece(i, j, chessBoard.getActiveColor() ? 'r' : 'R');
            if (chessBoard.isPiece(i + l * di[k], j + l * dj[k]))
                break;
        }
    return rookMoves;
}
vector<Move> MoveGenerator::getMoves(ChessBoard& chessBoard) {
    vector<Move> moves;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (chessBoard.isActiveColorPawn(i, j)) {
                queue<Move> pawnMoves = getPawnMoves(chessBoard, i, j);
                while (!pawnMoves.empty()) {
                    moves.push_back(pawnMoves.front());
                    pawnMoves.pop();
                }
            }
            else if (chessBoard.isActiveColorKnight(i, j)) {
                queue<Move> knightMoves = getKnightMoves(chessBoard, i, j);
                while (!knightMoves.empty()) {
                    moves.push_back(knightMoves.front());
                    knightMoves.pop();
                }
            }
            else if (chessBoard.isActiveColorBishop(i, j)) {
                queue<Move> bishopMoves = getBishopMoves(chessBoard, i, j);
                while (!bishopMoves.empty()) {
                    moves.push_back(bishopMoves.front());
                    bishopMoves.pop();
                }
            }
            else if (chessBoard.isActiveColorRook(i, j)) {
                queue<Move> rookMoves = getRookMoves(chessBoard, i, j);
                while (!rookMoves.empty()) {
                    moves.push_back(rookMoves.front());
                    rookMoves.pop();
                }
            }
            else if (chessBoard.isActiveColorQueen(i, j)) {
                queue<Move> queenMoves = getQueenMoves(chessBoard, i, j);
                while (!queenMoves.empty()) {
                    moves.push_back(queenMoves.front());
                    queenMoves.pop();
                }
            }
            else if (chessBoard.isActiveColorKing(i, j)) {
                queue<Move> kingMoves = getKingMoves(chessBoard, i, j);
                while (!kingMoves.empty()) {
                    moves.push_back(kingMoves.front());
                    kingMoves.pop();
                }
            }
    return moves;
}
