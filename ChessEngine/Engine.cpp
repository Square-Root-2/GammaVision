#include "Engine.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "MoveType.h"

pair<Move, double> Engine::negamax(State& state, int depth) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, double>(Move(0, 0, 0, 0, MoveType::TIMEOUT, false), 0);
    vector<Move> moves = MoveGenerator::getMoves(state);
    Move optimalMove;
    double alpha = -INFINITY;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        double evaluation = -negamax(state, 1, depth, -INFINITY, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2)
            return pair<Move, double>(Move(0, 0, 0, 0, MoveType::TIMEOUT, false), 0);
        state.setHashCode(hashCode);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
    }
    return pair<Move, double>(optimalMove, alpha);
}
double Engine::negamax(State& state, int currentDepth, int depth, double alpha, double beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 1 - currentDepth) : 0;
    if (currentDepth >= depth)
        return quiescenceSearch(state, currentDepth, alpha, beta);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        double evaluation = -negamax(state, currentDepth + 1, depth, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2)
            return -(Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2);
        state.setHashCode(hashCode);
        if (evaluation >= beta)
            return beta;
        alpha = max(alpha, evaluation);
    }
    return alpha;
}
double Engine::quiescenceSearch(State& state, int currentDepth, double alpha, double beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 1 - currentDepth) : 0;
    double standPat = Evaluator::getEvaluation(state);
    if (standPat >= beta)
        return beta;
    alpha = max(alpha, standPat);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        if (!moves[i].isCapture())
            continue;
        state.makeMove(moves[i]);
        double evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2)
            return -(Evaluator::getMaximumEvaluation() + maximumNegamaxDepth + maximumQuiescenceDepth + 2);
        state.setHashCode(hashCode);
        if (evaluation >= beta)
            return beta;
        alpha = max(alpha, evaluation);
    }
    return alpha;
}
tuple<Move, double, int> Engine::getOptimalMove(string FEN, int seconds) {
    pair<Move, double> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= maximumNegamaxDepth; depth++) {
        State state(FEN);
        pair<Move, double> move = negamax(state, depth);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, double, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, double, int>(optimalMove.first, optimalMove.second, maximumNegamaxDepth);
}
