#include "Engine.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "MoveType.h"

Engine::Engine(int maximumDepth) {
    this->maximumDepth = maximumDepth;
}
pair<Move, double> Engine::negamax(State& state, int depth) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, double>(Move(0, 0, 0, 0, MoveType::TIMEOUT), 0);
    vector<Move> moves = MoveGenerator::getMoves(state);
    Move optimalMove;
    double alpha = -INFINITY;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        double evaluation = -negamax(state, 1, depth, -INFINITY, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + maximumDepth + 2)
            return pair<Move, double>(Move(0, 0, 0, 0, MoveType::TIMEOUT), 0);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
        state.setHashCode(hashCode);
    }
    return pair<Move, double>(optimalMove, alpha);
}
double Engine::negamax(State& state, int currentDepth, int depth, double alpha, double beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + maximumDepth + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return -(Evaluator::getMaximumEvaluation() + maximumDepth + 1 - currentDepth);
    if (currentDepth >= depth)
        return Evaluator::getEvaluation(state);
    bool failHigh = -negamax(state, currentDepth + 1, depth - 3, -beta, -alpha) >= beta;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        double evaluation = -negamax(state, currentDepth + 1, depth - failHigh, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + maximumDepth + 2)
            return -(Evaluator::getMaximumEvaluation() + maximumDepth + 2);
        if (evaluation >= beta)
            return beta;
        alpha = max(alpha, evaluation);
        state.setHashCode(hashCode);
    }
    return alpha;
}
tuple<Move, double, int> Engine::getOptimalMove(string FEN, int seconds) {
    pair<Move, double> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= maximumDepth; depth++) {
        State state(FEN);
        pair<Move, double> move = negamax(state, depth);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, double, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, double, int>(optimalMove.first, optimalMove.second, maximumDepth);
}
