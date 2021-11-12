#include "Engine.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include <random>

void Engine::makeMove(State& state, tuple<int, int, int, int, int> move) {
    state.setPiece(get<2>(move), get<3>(move), state.getPiece(get<0>(move), get<1>(move)));
    state.setPiece(get<0>(move), get<1>(move), '.');
    char pieces[8] = { 'N', 'B', 'R', 'Q', 'n', 'b', 'r', 'q' };
    if (abs(get<4>(move) - 1.5) <= 1.5)
        state.setPiece(get<2>(move), get<3>(move), pieces[4 * state.getActiveColor() + get<4>(move)]);
    if (get<4>(move) == 4)
        state.setPiece(get<0>(move), get<3>(move), '.');
    else if (get<4>(move) == 8) {
        state.setPiece(get<0>(move), 7, '.');
        state.setPiece(get<0>(move), 5, state.getActiveColor() ? 'r' : 'R');
    }
    else if (get<4>(move) == 9) {
        state.setPiece(get<0>(move), 0, '.');
        state.setPiece(get<0>(move), 3, state.getActiveColor() ? 'r' : 'R');
    }
    if (abs(get<4>(move) - 8.5) == 0.5) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (get<4>(move) == 7) {
        state.setCanActiveColorCastleKingside(false);
        state.setCanActiveColorCastleQueenside(false);
    }
    else if (get<4>(move) == 6 && abs(get<0>(move) - 3.5) == 3.5 && get<1>(move) == 7)
        state.setCanActiveColorCastleKingside(false);
    else if (get<4>(move) == 6 && abs(get<0>(move) - 3.5) == 3.5 && get<1>(move) == 0)
        state.setCanActiveColorCastleQueenside(false);
    if (get<4>(move) == 5 && abs(get<2>(move) - get<0>(move)) == 2)
        state.setPossibleEnPassantTargetColumn(get<1>(move));
    else
        state.setPossibleEnPassantTargetColumn(-1);
    state.toggleActiveColor();
}
tuple<int, int, int, int, int, double, int> Engine::negamax(State& state, int depths) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return tuple<int, int, int, int, int, double, int>(-1, 0, 0, 0, 0, 0, 0);
    vector<tuple<int, int, int, int, int>> moves = MoveGenerator::getMoves(state);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    tuple<int, int, int, int, int> optimalMove;
    double optimalEvaluation = -INFINITY;
    int minimumMoves = INT32_MAX;
    int maximumMoves = 0;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        pair<double, int> opponentOptimalEvaluation = negamax(state, depths - 1, INFINITY);
        if (opponentOptimalEvaluation.second == -1)
            return tuple<int, int, int, int, int, double, int>(-1, 0, 0, 0, 0, 0, 0);
        if (-opponentOptimalEvaluation.first > optimalEvaluation) {
            optimalMove = moves[i];
            optimalEvaluation = -opponentOptimalEvaluation.first;
            if (-opponentOptimalEvaluation.first == -INFINITY)
                maximumMoves = opponentOptimalEvaluation.second + 1;
            else if (-opponentOptimalEvaluation.first == INFINITY)
                minimumMoves = opponentOptimalEvaluation.second + 1;
        }
        else if (-opponentOptimalEvaluation.first == optimalEvaluation && -opponentOptimalEvaluation.first == -INFINITY && opponentOptimalEvaluation.second + 1 > maximumMoves) {
            optimalMove = moves[i];
            maximumMoves = opponentOptimalEvaluation.second + 1;
        }
        else if (-opponentOptimalEvaluation.first == optimalEvaluation && -opponentOptimalEvaluation.first == INFINITY && opponentOptimalEvaluation.second + 1 < minimumMoves) {
            optimalMove = moves[i];
            minimumMoves = opponentOptimalEvaluation.second + 1;
        }
        state.setHashCode(hashCode);
    }
    return tuple<int, int, int, int, int, double, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove), optimalEvaluation, optimalEvaluation == -INFINITY ? maximumMoves : optimalEvaluation == INFINITY ? minimumMoves : INT32_MAX);
}
pair<double, int> Engine::negamax(State& state, int depths, double maximumOptimalEvaluation) {
    if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
        return pair<double, int>(0, -1);
    vector<tuple<int, int, int, int, int>> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return pair<double, int>(state.isActiveColorInCheck() ? -INFINITY : 0, 0);
    if (depths == 0)
        return pair<double, int>(Evaluator::getEvaluation(state), INT32_MAX);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    double optimalEvaluation = -INFINITY;
    int minimumMoves = INT32_MAX;
    int maximumMoves = 0;
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        pair<double, int> opponentOptimalEvaluation = negamax(state, depths - 1, INFINITY);
        if (opponentOptimalEvaluation.second == -1)
            return pair<double, int>(0, -1);
        if (-opponentOptimalEvaluation.first > optimalEvaluation) {
            optimalEvaluation = -opponentOptimalEvaluation.first;
            if (-opponentOptimalEvaluation.first == -INFINITY)
                maximumMoves = opponentOptimalEvaluation.second + 1;
            else if (-opponentOptimalEvaluation.first == INFINITY)
                minimumMoves = opponentOptimalEvaluation.second + 1;
        }
        else if (-opponentOptimalEvaluation.first == optimalEvaluation && -opponentOptimalEvaluation.first == -INFINITY && opponentOptimalEvaluation.second + 1 > maximumMoves)
            maximumMoves = opponentOptimalEvaluation.second + 1;
        else if (-opponentOptimalEvaluation.first == optimalEvaluation && -opponentOptimalEvaluation.first == INFINITY && opponentOptimalEvaluation.second + 1 < minimumMoves)
            minimumMoves = opponentOptimalEvaluation.second + 1;
        state.setHashCode(hashCode);
        // alpha-beta pruning
        if (optimalEvaluation > maximumOptimalEvaluation || abs(optimalEvaluation) != INFINITY && optimalEvaluation == maximumOptimalEvaluation)
            return pair<double, int>(optimalEvaluation, INT32_MAX);
    }
    return pair<double, int>(optimalEvaluation, optimalEvaluation == -INFINITY ? maximumMoves : optimalEvaluation == INFINITY ? minimumMoves : INT32_MAX);
}
tuple<int, int, int, int, int, double, int, int> Engine::getOptimalMove(string FEN, int seconds) {
    tuple<int, int, int, int, int, double, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depths = 1; ; depths++) {
        State state(FEN);
        tuple<int, int, int, int, int, double, int> move = negamax(state, depths);
        if (get<0>(move) == -1)
            return tuple<int, int, int, int, int, double, int, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove), get<5>(optimalMove), get<6>(optimalMove), depths);
        optimalMove = move;
    }
}
