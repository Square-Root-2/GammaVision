#include "Engine.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include <random>

tuple<int, int, int, int, int, double, int> Engine::getOptimalMove(State& state, int depths, double maximumOptimalEvaluation) {
    tuple<string, bool, int, int> hashCode = state.getHashCode();
    if (principalMoves.find(hashCode) != principalMoves.end() && depths <= get<7>(principalMoves[hashCode])) {
        tuple<int, int, int, int, int, double, int, int> optimalMove = principalMoves[hashCode];
        return tuple<int, int, int, int, int, double, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove), get<5>(optimalMove), get<6>(optimalMove));
    }
    vector<tuple<int, int, int, int, int>> moves = MoveGenerator::getMoves(state);
    if (moves.empty()) {
        principalMoves[hashCode] = tuple<int, int, int, int, int, double, int, int>(-1, -1, -1, -1, -1, state.isActiveColorInCheck() ? -INFINITY : 0, 0, INT32_MAX);
        return tuple<int, int, int, int, int, double, int>(-1, -1, -1, -1, -1, state.isActiveColorInCheck() ? -INFINITY : 0, 0);
    }
    if (depths == 0) {
        principalMoves[hashCode] = tuple<int, int, int, int, int, double, int, int>(-1, -1, -1, -1, -1, Evaluator::evaluateState(state), INT32_MAX, 0);
        return tuple<int, int, int, int, int, double, int>(-1, -1, -1, -1, -1, Evaluator::evaluateState(state), INT32_MAX);
    }
    tuple<int, int, int, int, int> optimalMove;
    double optimalEvaluation = -INFINITY;
    int minimumMoves = INT32_MAX;
    int maximumMoves = 0;
    orderMoves(moves, hashCode);
    for (int i = 0; i < moves.size(); i++) {
        makeMove(state, moves[i]);
        tuple<int, int, int, int, int, double, int> opponentOptimalMove = getOptimalMove(state, depths - 1, -optimalEvaluation);
        get<5>(opponentOptimalMove) = -get<5>(opponentOptimalMove);
        if (get<5>(opponentOptimalMove) > optimalEvaluation || get<5>(opponentOptimalMove) == optimalEvaluation && get<0>(optimalMove) == get<2>(optimalMove) && get<1>(optimalMove) == get<3>(optimalMove)) {
            optimalMove = moves[i];
            optimalEvaluation = get<5>(opponentOptimalMove);
            if (get<5>(opponentOptimalMove) == -INFINITY)
                maximumMoves = get<6>(opponentOptimalMove) + 1;
            else if (get<5>(opponentOptimalMove) == INFINITY)
                minimumMoves = get<6>(opponentOptimalMove) + 1;
        }
        else if (get<5>(opponentOptimalMove) == optimalEvaluation && get<5>(opponentOptimalMove) == -INFINITY && get<6>(opponentOptimalMove) + 1 > maximumMoves) {
            optimalMove = moves[i];
            maximumMoves = get<6>(opponentOptimalMove) + 1;
        }
        else if (get<5>(opponentOptimalMove) == optimalEvaluation && get<5>(opponentOptimalMove) == INFINITY && get<6>(opponentOptimalMove) + 1 < minimumMoves) {
            optimalMove = moves[i];
            minimumMoves = get<6>(opponentOptimalMove) + 1;
        }
        state.setHashCode(hashCode);
        if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
            return tuple<int, int, int, int, int, double, int>(-2, -2, -2, -2, -2, 0, INT32_MAX);
        if (get<5>(opponentOptimalMove) > maximumOptimalEvaluation || abs(get<5>(opponentOptimalMove)) != INFINITY && get<5>(opponentOptimalMove) == maximumOptimalEvaluation)
            return tuple<int, int, int, int, int, double, int>(get<0>(moves[i]), get<1>(moves[i]), get<2>(moves[i]), get<3>(moves[i]), get<4>(moves[i]), get<5>(opponentOptimalMove), INT32_MAX);
    }
    principalMoves[hashCode] = tuple<int, int, int, int, int, double, int, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove), optimalEvaluation, optimalEvaluation == -INFINITY ? maximumMoves : optimalEvaluation == INFINITY ? minimumMoves : INT32_MAX, depths);
    return tuple<int, int, int, int, int, double, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove), optimalEvaluation, optimalEvaluation == -INFINITY ? maximumMoves : optimalEvaluation == INFINITY ? minimumMoves : INT32_MAX);
}
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
void Engine::orderMoves(vector<tuple<int, int, int, int, int>>& moves, tuple<string, bool, int, int> hash) {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    if (principalMoves.find(hash) == principalMoves.end() || get<0>(principalMoves[hash]) == -1)
        return;
    tuple<int, int, int, int, int, double, int, int> optimalMove = principalMoves[hash];
    int j = -1;
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i] != tuple<int, int, int, int, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove)))
            continue;
        j = i;
        break;
    }
    tuple<int, int, int, int, int> move = moves[0];
    moves[0] = moves[j];
    moves[j] = move;
}
tuple<int, int, int, int, int, double, int, int> Engine::getOptimalMove(string FEN, int seconds) {
    principalMoves.clear();
    tuple<int, int, int, int, int, double, int, int> optimalMove;
    this->seconds = seconds;
    start = chrono::steady_clock::now();
    for (int depths = 1; ; depths++) {
        State state(FEN);
        tuple<int, int, int, int, int, double, int> move = getOptimalMove(state, depths, INFINITY);
        if (get<0>(move) == -2)
            return optimalMove;
        optimalMove = tuple<int, int, int, int, int, double, int, int>(get<0>(move), get<1>(move), get<2>(move), get<3>(move), get<4>(move), get<5>(move), get<6>(move), depths);
    }
}
