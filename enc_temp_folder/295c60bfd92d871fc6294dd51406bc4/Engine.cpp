#include "Engine.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include <random>
#include <iostream>

tuple<int, int, int, int, int, double, int> Engine::getOptimalMove(State& state, int depths, double maximumOptimalEvaluation) {
    tuple<string, bool, int, int, int> node(get<0>(state.getHashCode()), get<1>(state.getHashCode()), get<2>(state.getHashCode()), get<3>(state.getHashCode()), depths);
    if (principalMoves.find(node) != principalMoves.end())
        tuple<int, int, int, int, int, double, int> principalMove = principalMoves[node];
    /*
    if (refutationMoves.find(node) != refutationMoves.end()) {
        tuple<int, int, int, int, int, double, int> refutationMove = refutationMoves[node];
        if (get<5>(refutationMove) > maximumOptimalEvaluation || abs(get<5>(refutationMove)) != INFINITY && get<5>(refutationMove) == maximumOptimalEvaluation)
            return refutationMove;
    }
    */
    vector<tuple<int, int, int, int, int>> moves = MoveGenerator::getMoves(state);
    if (moves.empty())
        return principalMoves[node] = tuple<int, int, int, int, int, double, int>(-1, -1, -1, -1, -1, state.isActiveColorInCheck() ? -INFINITY : 0, 0);
    if (depths == 0)
        return principalMoves[node] = tuple<int, int, int, int, int, double, int>(-1, -1, -1, -1, -1, Evaluator::evaluateState(state), INT32_MAX);
    tuple<int, int, int, int, int> optimalMove;
    double optimalEvaluation = -INFINITY;
    int minimumMoves = INT32_MAX;
    int maximumMoves = 0;
    orderMoves(moves, node);
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
        state.setHashCode(tuple<string, bool, int, int>(get<0>(node), get<1>(node), get<2>(node), get<3>(node)));
        if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() >= seconds)
            return tuple<int, int, int, int, int, double, int>(-2, -2, -2, -2, -2, 0, INT32_MAX);
        /*
        if (get<5>(opponentOptimalMove) > maximumOptimalEvaluation || abs(get<5>(opponentOptimalMove)) != INFINITY && get<5>(opponentOptimalMove) == maximumOptimalEvaluation)
            return refutationMoves[node] = tuple<int, int, int, int, int, double, int>(get<0>(moves[i]), get<1>(moves[i]), get<2>(moves[i]), get<3>(moves[i]), get<4>(moves[i]), get<5>(opponentOptimalMove), INT32_MAX);
            */
    }
    return principalMoves[node] = tuple<int, int, int, int, int, double, int>(get<0>(optimalMove), get<1>(optimalMove), get<2>(optimalMove), get<3>(optimalMove), get<4>(optimalMove), optimalEvaluation, optimalEvaluation == -INFINITY ? maximumMoves : optimalEvaluation == INFINITY ? minimumMoves : INT32_MAX);
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
void Engine::orderMoves(vector<tuple<int, int, int, int, int>>& moves, tuple<string, bool, int, int, int> node) {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(moves.begin(), moves.end(), rng);
    get<4>(node)--;
    /*
    if (principalMoves.find(node) == principalMoves.end() && refutationMoves.find(node) == refutationMoves.end())
        return;
    */
    if (principalMoves.find(node) != principalMoves.end()) {
        tuple<int, int, int, int, int, double, int> principalMove = principalMoves[node];
        if (get<0>(principalMove) == -1)
            return;
        int j = -1;
        for (int i = 0; i < moves.size(); i++) {
            if (moves[i] != tuple<int, int, int, int, int>(get<0>(principalMove), get<1>(principalMove), get<2>(principalMove), get<3>(principalMove), get<4>(principalMove)))
                continue;
            j = i;
            break;
        }
        tuple<int, int, int, int, int> move = moves[0];
        moves[0] = moves[j];
        moves[j] = move;
    }
    /*
    else {
        tuple<int, int, int, int, int, double, int> refutationMove = refutationMoves[node];
        int j = -1;
        for (int i = 0; i < moves.size(); i++) {
            if (moves[i] != tuple<int, int, int, int, int>(get<0>(refutationMove), get<1>(refutationMove), get<2>(refutationMove), get<3>(refutationMove), get<4>(refutationMove)))
                continue;
            j = i;
            break;
        }
        tuple<int, int, int, int, int> move = moves[0];
        moves[0] = moves[j];
        moves[j] = move;
    }
    */
}
tuple<int, int, int, int, int, double, int, int> Engine::getOptimalMove(string FEN, int seconds) {
    principalMoves.clear();
    cout << "hi";
    refutationMoves.clear();
    cout << "hi";
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
vector<tuple<int, int, int, int, int, double, int>> Engine::getPrincipalVariation(string FEN, int depths) {
    vector<tuple<int, int, int, int, int, double, int>> principalVariation;
    State state(FEN);
    while (true) {
        tuple<int, int, int, int, int, double, int> principalMove = principalMoves[tuple<string, bool, int, int, int>(get<0>(state.getHashCode()), get<1>(state.getHashCode()), get<2>(state.getHashCode()), get<3>(state.getHashCode()), depths--)];
        if (get<0>(principalMove) == -1)
            return principalVariation;
        principalVariation.push_back(principalMove);
        makeMove(state, tuple<int, int, int, int, int>(get<0>(principalMove), get<1>(principalMove), get<2>(principalMove), get<3>(principalMove), get<4>(principalMove)));
    }
}

