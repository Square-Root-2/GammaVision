#include "Engine.h"
#include "Evaluator.h"
#include "MoveComparer.h"
#include "MoveGenerator.h"
#include "MoveType.h"

void Engine::getPrincipalVariation(vector<Move>& principalVariation, State& state, int depths) {
    vector<Move> moves = MoveGenerator::getMoves(state); 
    if (moves.empty())
        return;
    if (depths == 0)
        return;
    Move optimalMove;
    Evaluation alpha(-INFINITY, 0);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        map<pair<tuple<string, bool, int, int>, int>, Evaluation>::iterator it = evaluations.find(pair<tuple<string, bool, int, int>, int>(state.getHashCode(), depths - 1));
        if (it != evaluations.end() && it->second > alpha) {
            optimalMove = moves[i];
            alpha = it->second;
        }
        state.setHashCode(hashCode);
    }
    principalVariation.push_back(optimalMove);
    state.makeMove(optimalMove);
    getPrincipalVariation(principalVariation, state, depths - 1);
}
pair<Move, Evaluation> Engine::negamax(State& state, int depths) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<Move, Evaluation>(Move(0, 0, 0, 0, MoveType::TIMEOUT), Evaluation(0, 0));
    vector<Move> moves = MoveGenerator::getMoves(state);
    sort(moves.begin(), moves.end(), MoveComparer(evaluations, state, depths));
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
    return pair<Move, Evaluation>(optimalMove, evaluations[pair<tuple<string, bool, int, int>, int>(hashCode, depths)] = alpha);
}
Evaluation Engine::negamax(State& state, int depths, Evaluation alpha, Evaluation beta) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return Evaluation(0, INT32_MIN);
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    map<pair<tuple<string, bool, int, int>, int>, Evaluation>::iterator it = evaluations.find(pair<tuple<string, bool, int, int>, int>(hashCode, depths));
    if (it != evaluations.end())
        return it->second;
    vector<Move> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return evaluations[pair<tuple<string, bool, int, int>, int>(hashCode, depths)] = Evaluation(state.isActiveColorInCheck() ? -INFINITY : 0, state.isActiveColorInCheck() ? 0 : INT32_MAX);
    if (depths == 0)
        return evaluations[pair<tuple<string, bool, int, int>, int>(hashCode, depths)] = Evaluation(Evaluator::getEvaluation(state), INT32_MAX);
    sort(moves.begin(), moves.end(), MoveComparer(evaluations, state, depths));
    Move optimalMove;
    for (int i = 0; i < moves.size(); i++) {
        state.makeMove(moves[i]);
        Evaluation evaluation = negamax(state, depths - 1, Evaluation(-beta.getPawns(), abs(beta.getPawns()) == INFINITY ? beta.getMoves() - 1 : INT32_MAX), Evaluation(-alpha.getPawns(), abs(alpha.getPawns()) == INFINITY ? alpha.getMoves() - 1 : INT32_MAX));
        if (evaluation.getMoves() == INT32_MIN)
            return Evaluation(0, INT32_MIN);
        evaluation = Evaluation(-evaluation.getPawns(), abs(evaluation.getPawns()) == INFINITY ? evaluation.getMoves() + 1 : INT32_MAX);
        if (evaluation > alpha) {
            optimalMove = moves[i];
            alpha = evaluation;
        }
        state.setHashCode(hashCode);
    }
    if (optimalMove.getType() != MoveType::DEFAULT)
        evaluations[pair<tuple<string, bool, int, int>, int>(hashCode, depths)] = alpha;
    return alpha;
}
tuple<Move, Evaluation, int> Engine::getOptimalMove(string FEN, int seconds) {
    evaluations.clear();
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
vector<Move> Engine::getPrincipalVariation(string FEN, int depths) {
    vector<Move> principalVariation;
    State state(FEN);
    getPrincipalVariation(principalVariation, state, depths);
    return principalVariation;
}
