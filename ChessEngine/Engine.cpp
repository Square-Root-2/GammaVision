#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparator.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

void Engine::makeMove(State& state, Move& move) {
    state.setPiece(move.getEndRow(), move.getEndColumn(), move.getAggressor());
    state.setPiece(move.getBeginRow(), move.getBeginColumn(), '.');
    if (move.getType() == MoveType::KINGSIDE_CASTLE) {
        state.setPiece(move.getBeginRow(), 7, '.');
        state.setPiece(move.getBeginRow(), 5, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::QUEENSIDE_CASTLE) {
        state.setPiece(move.getBeginRow(), 0, '.');
        state.setPiece(move.getBeginRow(), 3, state.getActiveColor() ? 'r' : 'R');
    }
    else if (move.getType() == MoveType::EN_PASSANT)
        state.setPiece(move.getBeginRow(), move.getEndColumn(), '.');
    else if (move.getType() == MoveType::PROMOTION_TO_BISHOP)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'b' : 'B');
    else if (move.getType() == MoveType::PROMOTION_TO_KNIGHT)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'n' : 'N');
    else if (move.getType() == MoveType::PROMOTION_TO_QUEEN)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'q' : 'Q');
    else if (move.getType() == MoveType::PROMOTION_TO_ROOK)
        state.setPiece(move.getEndRow(), move.getEndColumn(), state.getActiveColor() ? 'r' : 'R');
    if (move.getType() == MoveType::KINGSIDE_CASTLE || move.getType() == MoveType::QUEENSIDE_CASTLE) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::KING_MOVE) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 7)
        state.setCanActiveColorCastleKingside(false);
    else if (move.getType() == MoveType::ROOK_MOVE && abs(move.getBeginRow() - 3.5) == 3.5 && move.getBeginColumn() == 0)
        state.setCanActiveColorCastleQueenside(false);
    if (move.getType() == MoveType::PAWN_FORWARD_TWO)
        state.setPossibleEnPassantTargetColumn(move.getBeginColumn());
    else
        state.setPossibleEnPassantTargetColumn(-1);
    state.toggleActiveColor();
}
pair<Move, int> Engine::negamax(State& state, int depth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    vector<Move> moves = MoveGenerator::getMoves(state); 
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    map<tuple<string, bool, int, int>, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(hashCode);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0], it != transpositionTable.end() ? get<3>(it->second) : move));
    Move optimalMove;
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        int evaluation = -negamax(state, 1, depth, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        state.setHashCode(hashCode);
        if (evaluation >= beta) {
            transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth, NodeType::CUT_NODE, beta, moves[i]);
            return pair<Move, int>(moves[i], beta);
        }
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
    }
    killerMoves[1].clear();
    if (optimalMove.getType() == MoveType::DEFAULT)
        transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth, NodeType::ALL_NODE, alpha, optimalMove);
    else
        transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth, NodeType::PV_NODE, alpha, optimalMove);
    return pair<Move, int>(optimalMove, alpha);
}
int Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    map<tuple<string, bool, int, int>, tuple<int, NodeType, int, Move>>::iterator it = transpositionTable.find(hashCode);
    if (it != transpositionTable.end() && get<0>(it->second) >= depth - currentDepth) {
        if ((get<1>(it->second) == NodeType::PV_NODE || get<1>(it->second) == NodeType::CUT_NODE) && get<2>(it->second) >= beta)
            return beta;
        if ((get<1>(it->second) == NodeType::PV_NODE || get<1>(it->second) == NodeType::ALL_NODE) && get<2>(it->second) <= alpha)
            return alpha;
        if (get<1>(it->second) == NodeType::PV_NODE)
            return get<2>(it->second);
        if (get<1>(it->second) == NodeType::CUT_NODE)
            alpha = max(alpha, get<2>(it->second));
        else if (get<1>(it->second) == NodeType::ALL_NODE)
            beta = min(beta, get<2>(it->second));
    }
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    if (currentDepth >= depth)
        return quiescenceSearch(state, currentDepth, alpha, beta);
    if (!state.isActiveColorInCheck()) {
        state.toggleActiveColor();
        int evaluation = -negamax(state, currentDepth + 1, depth - R, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.toggleActiveColor();
        if (evaluation >= beta)
            return beta;
    }
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[currentDepth], it != transpositionTable.end() ? get<3>(it->second) : move));
    Move optimalMove;
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        int evaluation = -negamax(state, currentDepth + 1, depth, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.setHashCode(hashCode);
        if (evaluation >= beta) {
            if (moves[i].isQuiet())
                killerMoves[currentDepth].insert(moves[i]);
            killerMoves[currentDepth + 1].clear();
            if (it == transpositionTable.end() || depth - currentDepth > get<0>(it->second))
                transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth - currentDepth, NodeType::CUT_NODE, beta, moves[i]);
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
            optimalMove = moves[i];
        }
    }
    killerMoves[currentDepth + 1].clear();
    if (it == transpositionTable.end() || depth - currentDepth > get<0>(it->second))
        if (optimalMove.getType() == MoveType::DEFAULT)
            transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth - currentDepth, NodeType::ALL_NODE, alpha, optimalMove);
        else
            transpositionTable[hashCode] = tuple<int, NodeType, int, Move>(depth - currentDepth, NodeType::PV_NODE, alpha, optimalMove);
    return alpha;
}
int Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    int standPat = Evaluator::getEvaluation(state);
    if (standPat >= beta)
        return beta;
    alpha = max(alpha, standPat);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0], move));
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        if (!moves[i].isCapture())
            break;
        makeMove(state, moves[i]);
        int evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.setHashCode(hashCode);
        if (evaluation >= beta)
            return beta;
        alpha = max(alpha, evaluation);
    }
    return alpha;
}
int Engine::getMaximumNegamaxDepth() {
    return MAXIMUM_NEGAMAX_DEPTH;
}
int Engine::getMaximumQuiescenceDepth() {
    return MAXIMUM_QUIESCENCE_DEPTH;
}
tuple<Move, int, int> Engine::getOptimalMove(string& FEN, int seconds) {
    transpositionTable.clear();
    pair<Move, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= getMaximumNegamaxDepth(); depth++) {
        State state(FEN);
        int alpha = optimalMove.second - 25;
        int beta = optimalMove.second + 25;
        pair<Move, int> move = negamax(state, depth, alpha, beta);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, int, int>(optimalMove.first, optimalMove.second, depth - 1);
        if (move.second == alpha)
            move = negamax(state, depth, -INT32_MAX, alpha + 1);
        else if (move.second == beta)
            move = negamax(state, depth, beta - 1, INT32_MAX);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, int, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, int, int>(optimalMove.first, optimalMove.second, getMaximumNegamaxDepth());
}
