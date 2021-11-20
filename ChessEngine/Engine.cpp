#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparator.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

pair<Move, double> Engine::negamax(State& state, int depth) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, double>(Move(0, 0, 0, 0, MoveType::TIMEOUT, false), 0);
    vector<Move> moves = MoveGenerator::getMoves(state);
    Move optimalMove;
    double alpha = -INFINITY;
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0]));
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        double evaluation = -negamax(state, 1, depth, -INFINITY, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return pair<Move, double>(Move(0, 0, 0, 0, MoveType::TIMEOUT, false), 0);
        state.setHashCode(hashCode);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
    }
    killerMoves[1].clear();
    return pair<Move, double>(optimalMove, alpha);
}
double Engine::negamax(State& state, int currentDepth, int depth, double alpha, double beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    if (currentDepth >= depth)
        return quiescenceSearch(state, currentDepth, alpha, beta);
    if (!state.isActiveColorInCheck()) {
        state.toggleActiveColor();
        double evaluation = -negamax(state, currentDepth + 1, depth - R, -beta, -beta + 1);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.toggleActiveColor();
        if (evaluation >= beta)
            return evaluation;
    }
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[currentDepth]));
    double optimalEvaluation = -INFINITY;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        double evaluation = -negamax(state, currentDepth + 1, depth, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.setHashCode(hashCode);
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
double Engine::quiescenceSearch(State& state, int currentDepth, double alpha, double beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    double standPat = Evaluator::getEvaluation(state);
    if (standPat >= beta)
        return standPat;
    alpha = max(alpha, standPat);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i].isQuiet())
            continue;
        state.makeMove(moves[i]);
        double evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.setHashCode(hashCode);
        if (evaluation >= beta)
            return evaluation;
        standPat = max(standPat, evaluation);
        alpha = max(alpha, evaluation);
    }
    return standPat;
}
int Engine::getMaximumNegamaxDepth() {
    return maximumNegamaxDepth;
}
int Engine::getMaximumQuiescenceDepth() {
    return maximumQuiescenceDepth;
}
tuple<Move, double, int> Engine::getOptimalMove(string FEN, int seconds) {
    pair<Move, double> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= getMaximumNegamaxDepth(); depth++) {
        State state(FEN);
        pair<Move, double> move = negamax(state, depth);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, double, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, double, int>(optimalMove.first, optimalMove.second, getMaximumNegamaxDepth());
}
