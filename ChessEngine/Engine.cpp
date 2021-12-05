#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparator.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

void Engine::makeMove(ChessBoard& chessBoard, Move& move) {
    chessBoard.setPiece(move.getEndRow(), move.getEndColumn(), move.getAggressor());
    chessBoard.setPiece(move.getBeginRow(), move.getBeginColumn(), '.');
    if (move.getType() == MoveType::KINGSIDE_CASTLE) {
        chessBoard.setPiece(move.getBeginRow(), 7, '.');
        chessBoard.setPiece(move.getBeginRow(), 5, chessBoard.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::QUEENSIDE_CASTLE) {
        chessBoard.setPiece(move.getBeginRow(), 0, '.');
        chessBoard.setPiece(move.getBeginRow(), 3, chessBoard.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::EN_PASSANT)
        chessBoard.setPiece(move.getBeginRow(), move.getEndColumn(), '.');
    else if (move.getType() == MoveType::PROMOTION_TO_BISHOP)
        chessBoard.setPiece(move.getEndRow(), move.getEndColumn(), chessBoard.getActiveColor() ? 'b' : 'B');
    else if (move.getType() == MoveType::PROMOTION_TO_KNIGHT)
        chessBoard.setPiece(move.getEndRow(), move.getEndColumn(), chessBoard.getActiveColor() ? 'n' : 'N');
    else if (move.getType() == MoveType::PROMOTION_TO_QUEEN)
        chessBoard.setPiece(move.getEndRow(), move.getEndColumn(), chessBoard.getActiveColor() ? 'q' : 'Q');
    else if (move.getType() == MoveType::PROMOTION_TO_ROOK)
        chessBoard.setPiece(move.getEndRow(), move.getEndColumn(), chessBoard.getActiveColor() ? 'r' : 'R');
    if (move.getType() == MoveType::KINGSIDE_CASTLE || move.getType() == MoveType::QUEENSIDE_CASTLE) {
        chessBoard.setCanActiveColorCastleKingside(false);
        chessBoard.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::KING_MOVE) {
        chessBoard.setCanActiveColorCastleKingside(false);
        chessBoard.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 7)
        chessBoard.setCanActiveColorCastleKingside(false);
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 0)
        chessBoard.setCanActiveColorCastleQueenside(false);
    if (move.getType() == MoveType::PAWN_FORWARD_TWO)
        chessBoard.setPossibleEnPassantTargetColumn(move.getBeginColumn());
    else
        chessBoard.setPossibleEnPassantTargetColumn(-1);
    chessBoard.toggleActiveColor();
}
pair<Move, int> Engine::negamax(ChessBoard& chessBoard, int depth) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    vector<Move> moves = MoveGenerator::getMoves(chessBoard); 
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0]));
    Move optimalMove;
    int alpha = -INT32_MAX;
    tuple<string, bool, int, int> hashCode = chessBoard.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        makeMove(chessBoard, moves[i]);
        int evaluation = -negamax(chessBoard, 1, depth, -INT32_MAX, -alpha, true);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        chessBoard.setHashCode(hashCode);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
    }
    killerMoves[1].clear();
    return pair<Move, int>(optimalMove, alpha);
}
int Engine::negamax(ChessBoard& chessBoard, int currentDepth, int depth, int alpha, int beta, bool isNullOk) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(chessBoard);
    if (moves.empty())
        return chessBoard.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    if (currentDepth >= depth)
        return quiescenceSearch(chessBoard, currentDepth, alpha, beta);
    if (isNullOk && !chessBoard.isActiveColorInCheck()) {
        chessBoard.toggleActiveColor();
        int evaluation = -negamax(chessBoard, currentDepth + 1, depth - (depth - currentDepth > 6 ? MAX_R : MIN_R), -beta, -beta + 1, false);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        chessBoard.toggleActiveColor();
        if (evaluation >= beta) {
            depth -= DR;
            if (currentDepth >= depth) {
                killerMoves[currentDepth + 1].clear();
                return quiescenceSearch(chessBoard, currentDepth, alpha, beta);
            }
        }
    }
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[currentDepth]));
    int optimalEvaluation = -INT32_MAX;
    tuple<string, bool, int, int> hashCode = chessBoard.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        makeMove(chessBoard, moves[i]);
        int evaluation = -negamax(chessBoard, currentDepth + 1, depth, -beta, -alpha, true);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        chessBoard.setHashCode(hashCode);
        if (evaluation >= beta) {
            if (moves[i].isQuiet())
                killerMoves[currentDepth].insert(moves[i]);
            killerMoves[currentDepth + 1].clear();
            return evaluation;
        }
        optimalEvaluation = max(optimalEvaluation, evaluation);
        alpha = max(alpha, evaluation);
    }
    killerMoves[currentDepth + 1].clear();
    return optimalEvaluation;
}
int Engine::quiescenceSearch(ChessBoard& chessBoard, int currentDepth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(chessBoard);
    if (moves.empty())
        return chessBoard.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    int standPat = Evaluator::getEvaluation(chessBoard);
    if (standPat >= beta)
        return standPat;
    alpha = max(alpha, standPat);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0]));
    tuple<string, bool, int, int> hashCode = chessBoard.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        if (!moves[i].isCapture())
            break;
        makeMove(chessBoard, moves[i]);
        int evaluation = -quiescenceSearch(chessBoard, currentDepth + 1, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        chessBoard.setHashCode(hashCode);
        if (evaluation >= beta)
            return evaluation;
        standPat = max(standPat, evaluation);
        alpha = max(alpha, evaluation);
    }
    return standPat;
}
int Engine::getMaximumNegamaxDepth() {
    return MAXIMUM_NEGAMAX_DEPTH;
}
int Engine::getMaximumQuiescenceDepth() {
    return MAXIMUM_QUIESCENCE_DEPTH;
}
tuple<Move, int, int> Engine::getOptimalMove(string& FEN, int seconds) {
    pair<Move, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= getMaximumNegamaxDepth(); depth++) {
        ChessBoard chessBoard(FEN);
        pair<Move, int> move = negamax(chessBoard, depth);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, int, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, int, int>(optimalMove.first, optimalMove.second, getMaximumNegamaxDepth());
}
