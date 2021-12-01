#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparator.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

pair<Move, int> Engine::negamax(State& state, int depth) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
    vector<Move> moves = MoveGenerator::getMoves(state); 
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0]));
    Move optimalMove;
    int alpha = INT32_MIN;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        int evaluation = -negamax(state, 1, depth, INT32_MIN, -alpha, true);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return pair<Move, int>(Move(0, 0, 0, 0, MoveType::TIMEOUT, ' ', ' '), 0);
        state.setHashCode(hashCode);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
    }
    killerMoves[1].clear();
    return pair<Move, int>(optimalMove, alpha);
}
int Engine::negamax(State& state, int currentDepth, int depth, int alpha, int beta, bool nullOk) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    if (currentDepth >= depth)
        return quiescenceSearch(state, currentDepth, alpha, beta);
    if (nullOk && !state.isActiveColorInCheck()) {
        state.toggleActiveColor();
        int R = depth - currentDepth > 6 ? MAX_R : MIN_R;
        int evaluation = -negamax(state, currentDepth + 1, depth - R, -beta, -beta + 1, false);
        state.toggleActiveColor();
        if (evaluation >= beta) {
            depth -= DR;
            if (currentDepth >= depth)
                return quiescenceSearch(state, currentDepth, alpha, beta);
        }
    }
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[currentDepth]));
    int optimalEvaluation = INT32_MIN;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        int evaluation = -negamax(state, currentDepth + 1, depth, -beta, -alpha, true);
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
int Engine::quiescenceSearch(State& state, int currentDepth, int alpha, int beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return state.isActiveColorInCheck() ? -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 1 - currentDepth) : 0;
    int optimalEvaluation = INT32_MIN;
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    sort(moves.begin(), moves.end(), MoveComparator(killerMoves[0]));
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i].isQuiet()) {
            int evaluation = Evaluator::getEvaluation(state);
            if (evaluation >= beta)
                return evaluation;
            optimalEvaluation = max(optimalEvaluation, evaluation);
            alpha = max(alpha, evaluation);
            break;
        }
        state.makeMove(moves[i]);
        int evaluation = -quiescenceSearch(state, currentDepth + 1, -beta, -alpha);
        if (evaluation == Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2)
            return -(Evaluator::getMaximumEvaluation() + getMaximumNegamaxDepth() + getMaximumQuiescenceDepth() + 2);
        state.setHashCode(hashCode);
        if (evaluation >= beta)
            return evaluation;
        optimalEvaluation = max(optimalEvaluation, evaluation);
        alpha = max(alpha, evaluation);
    }
    return optimalEvaluation;
}
int Engine::getMaximumNegamaxDepth() {
    return MAXIMUM_NEGAMAX_DEPTH;
}
int Engine::getMaximumQuiescenceDepth() {
    return MAXIMUM_QUIESCENCE_DEPTH;
}
tuple<Move, int, int> Engine::getOptimalMove(string FEN, int seconds) {
    pair<Move, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depth = 1; depth <= getMaximumNegamaxDepth(); depth++) {
        State state(FEN);
        pair<Move, int> move = negamax(state, depth);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, int, int>(optimalMove.first, optimalMove.second, depth - 1);
        optimalMove = move;
    }
    return tuple<Move, int, int>(optimalMove.first, optimalMove.second, getMaximumNegamaxDepth());
}
