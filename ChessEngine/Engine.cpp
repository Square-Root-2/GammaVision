#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparer.h"
#include "MoveGenerator.h"
#include "MoveType.h"
#include <random>

pair<Move, Evaluation> Engine::negamax(State& state, int depths) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, Evaluation>(Move(0, 0, 0, 0, MoveType::TIMEOUT), Evaluation(0, 0));
    vector<Move> moves = MoveGenerator::getMoves(state);
    //mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    //shuffle(moves.begin(), moves.end(), rng);
    Move optimalMove;
    Evaluation alpha(-INFINITY, 0);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        Evaluation evaluation = negamax(state, depths - 1, Evaluation(-INFINITY, 0), Evaluation(-alpha.getPawns(), abs(alpha.getPawns()) == INFINITY ? alpha.getMoves() - 1 : INT32_MAX));
        if (evaluation.getMoves() == INT32_MIN)
            return pair<Move, Evaluation>(Move(0, 0, 0, 0, MoveType::TIMEOUT), Evaluation(0, 0));
        evaluation = Evaluation(-evaluation.getPawns(), abs(evaluation.getPawns()) == INFINITY ? evaluation.getMoves() + 1 : INT32_MAX);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
        state.setHashCode(hashCode);
    }
    return pair<Move, Evaluation>(optimalMove, alpha);
}
Evaluation Engine::negamax(State& state, int depths, Evaluation alpha, Evaluation beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return Evaluation(0, INT32_MIN);

    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return Evaluation(state.isActiveColorInCheck() ? -INFINITY : 0, state.isActiveColorInCheck() ? 0 : INT32_MAX);
    if (depths == 0)
        return Evaluation(Evaluator::getEvaluation(state), INT32_MAX);
    //mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    //shuffle(moves.begin(), moves.end(), rng);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        Evaluation evaluation = negamax(state, depths - 1, Evaluation(-beta.getPawns(), abs(beta.getPawns()) == INFINITY ? beta.getMoves() - 1 : INT32_MAX), Evaluation(-alpha.getPawns(), abs(alpha.getPawns()) == INFINITY ? alpha.getMoves() - 1 : INT32_MAX));
        if (evaluation.getMoves() == INT32_MIN)
            return Evaluation(0, INT32_MIN);
        evaluation = Evaluation(-evaluation.getPawns(), abs(evaluation.getPawns()) == INFINITY ? evaluation.getMoves() + 1 : INT32_MAX);
        // alpha-beta pruning
        if (evaluation >= beta)
            return beta;
        if (evaluation > alpha)
            alpha = evaluation;
        state.setHashCode(hashCode);
    }
    return Evaluation(-alpha.getPawns(), abs(alpha.getPawns()) == INFINITY ? alpha.getMoves() + 1 : INT32_MAX);
}
tuple<Move, Evaluation, int> Engine::getOptimalMove(string FEN, int seconds) {
    pair<Move, Evaluation> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depths = 1; ; depths++) {
        State state(FEN);
        pair<Move, Evaluation> move = negamax(state, depths);
        if (move.first.getType() == MoveType::TIMEOUT)
            return tuple<Move, Evaluation, int>(optimalMove.first, optimalMove.second, depths - 1);
        optimalMove = move;
    }
}
